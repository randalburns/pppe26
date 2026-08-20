/* stride_bandwidth.c -- two experiments about the *width* of the memory system.
 *
 * Part A (stride sweep): touch one 8-byte word every `stride` bytes in a
 * 128MB array.  DRAM moves whole 64-byte lines no matter how little of the
 * line you asked for, so once the stride passes 64 the time per access stops
 * improving while the fraction of each fetched line you actually use
 * collapses.  This is the quantitative form of "sequential access is parallel
 * access".
 *
 * Part B (bandwidth vs working-set size): stream-read and stream-write arrays
 * sized to land in L1, L2, L3 and DRAM.  Latency (cache_latency.c) is how long
 * one *dependent* access takes; bandwidth is how much *independent* traffic a
 * level sustains.  Different numbers, different shapes, both matter.
 *
 * Both loops use multiple accumulators.  With a single accumulator the adds
 * form a serial dependency chain and the loop measures the adder's latency
 * rather than the memory system -- the same multiple-accumulator trick as in
 * ../ILP/sep_dependent.cpp.  Integer accumulators are used deliberately:
 * floating-point reductions are not reassociable, so the compiler will not
 * vectorize them without -ffast-math and the read test silently becomes
 * compute-bound at ~54 GB/s on every size.
 *
 * Output is CSV on stdout, two blocks tagged by the first column.
 *
 * Build: gcc -O3 -march=native -o stride_bandwidth stride_bandwidth.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

static void pin(int cpu) {
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(cpu, &set);
    sched_setaffinity(0, sizeof(set), &set);
}

static double measure_ghz(double seconds) {
    long n = 100L * 1000 * 1000;
    for (;;) {
        uint64_t x = 0;
        double t0 = now_s();
        for (long i = 0; i < n; i++) __asm__ volatile ("addq $1, %0" : "+r"(x));
        double dt = now_s() - t0;
        __asm__ volatile ("" :: "r"(x));
        if (dt >= seconds) return n / dt / 1e9;
        n *= 2;
    }
}

volatile uint64_t sink;

int main(int argc, char **argv) {
    int cpu = (argc > 1) ? atoi(argv[1]) : 0;
    pin(cpu);
    measure_ghz(1.0);                        /* boost the core before measuring */
    double ghz = measure_ghz(0.1);
    fprintf(stderr, "cpu%d at %.2f GHz\n", cpu, ghz);

    /* ---------------- Part A: stride sweep ---------------- */
    const size_t N = 128UL << 20;            /* 128 MB: far beyond L3 */
    uint64_t *a = NULL;
    if (posix_memalign((void **)&a, 2UL << 20, N)) { perror("alloc"); return 1; }
    madvise(a, N, MADV_HUGEPAGE);            /* take the TLB out of the picture */
    memset(a, 1, N);
    size_t n = N / sizeof(uint64_t);

    printf("part,stride,ns_per_access,useful_GBps,fetched_GBps\n");
    for (size_t stride = 8; stride <= 8192; stride *= 2) {
        size_t step = stride / sizeof(uint64_t);
        double best = 1e30;
        for (int rep = 0; rep < 5; rep++) {
            uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
            double t0 = now_s();
            for (size_t i = 0; i + 3 * step < n; i += 4 * step) {
                s0 += a[i];
                s1 += a[i + step];
                s2 += a[i + 2 * step];
                s3 += a[i + 3 * step];
            }
            double dt = now_s() - t0;
            sink = s0 + s1 + s2 + s3;
            if (dt < best) best = dt;
        }
        size_t accesses = n / step;
        double ns = best * 1e9 / accesses;
        /* useful = the 8 bytes we asked for; fetched = the whole line DRAM moved */
        double useful   = accesses * 8.0 / best / 1e9;
        double fetched  = accesses * (double)(stride < 64 ? stride : 64) / best / 1e9;
        printf("stride,%zu,%.3f,%.2f,%.2f\n", stride, ns, useful, fetched);
        fflush(stdout);
    }
    free(a);

    /* ---------------- Part B: bandwidth vs working set ---------------- */
    printf("part,bytes,read_GBps,write_GBps,\n");
    for (size_t bytes = 16UL << 10; bytes <= 256UL << 20; bytes *= 2) {
        uint64_t *b = NULL;
        if (posix_memalign((void **)&b, 2UL << 20, bytes)) { perror("alloc"); return 1; }
        madvise(b, bytes, MADV_HUGEPAGE);
        memset(b, 1, bytes);
        size_t m = bytes / sizeof(uint64_t);

        long reps = (long)((4.0 * (1UL << 30)) / bytes);   /* ~4 GB of traffic */
        if (reps < 3) reps = 3;

        double bestr = 1e30, bestw = 1e30;
        for (int trial = 0; trial < 3; trial++) {
            double t0 = now_s();
            for (long r = 0; r < reps; r++) {
                uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
                for (size_t i = 0; i + 3 < m; i += 4) {
                    s0 += b[i]; s1 += b[i+1]; s2 += b[i+2]; s3 += b[i+3];
                }
                sink = s0 + s1 + s2 + s3;
            }
            double dt = (now_s() - t0) / reps;
            if (dt < bestr) bestr = dt;

            t0 = now_s();
            for (long r = 0; r < reps; r++) {
                for (size_t i = 0; i < m; i++) b[i] = (uint64_t)r;
                sink = b[0];
            }
            dt = (now_s() - t0) / reps;
            if (dt < bestw) bestw = dt;
        }
        printf("bw,%zu,%.2f,%.2f,\n", bytes, bytes / bestr / 1e9, bytes / bestw / 1e9);
        fflush(stdout);
        free(b);
    }
    return 0;
}
