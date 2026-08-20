/* cache_latency.c -- LMBench-style pointer-chase latency vs working-set size.
 *
 * Modernized from the 1996 LMBench idea.  LMBench walked a fixed 128-byte
 * stride; every prefetcher built after about 2005 recognizes that instantly,
 * so a fixed stride now measures the prefetcher, not the cache.  We chase a
 * *random* permutation of cache lines instead: each load's address comes from
 * the previous load's result, so nothing can be issued early and the measured
 * time is true load-to-use latency.
 *
 * Three modes per working-set size:
 *   random  -- random permutation of 64B lines: defeats the prefetcher
 *   seq     -- lines in address order: the prefetcher hides the latency
 *   huge    -- random, on 2MB transparent huge pages: removes TLB misses
 *
 * Two measurement details matter on a mobile part like Strix Point:
 *   1. The core clock moves (3.5 GHz cold, 5.1 GHz boosted, less when hot), so
 *      the clock is re-measured immediately before every single data point with
 *      a serial dependent-add chain.  A clock measured once at startup is stale
 *      by the end of the run and silently corrupts every cycle count.
 *   2. Modes are interleaved within each size, so thermal drift affects all
 *      three modes at a given size equally.
 *
 * Output is CSV on stdout: mode,bytes,ns,cycles,ghz
 *
 * Build: gcc -O2 -o cache_latency cache_latency.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>

#define LINE 64

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

/* Core clock from a serial dependent add chain: each addq depends on the
 * previous one, so the chain retires at exactly one per cycle. */
static double measure_ghz(double seconds) {
    long n = 100L * 1000 * 1000;
    for (;;) {
        uint64_t x = 0;
        double t0 = now_s();
        for (long i = 0; i < n; i++)
            __asm__ volatile ("addq $1, %0" : "+r"(x));
        double dt = now_s() - t0;
        __asm__ volatile ("" :: "r"(x));
        if (dt >= seconds) return n / dt / 1e9;
        n *= 2;
    }
}

static void pin(int cpu) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    sched_setaffinity(0, sizeof(set), &set);
}

static uint64_t rng_state = 88172645463325252ULL;
static uint64_t rng(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return rng_state;
}

/* One cycle through all `n` lines of `buf`; the next pointer lives in the
 * first 8 bytes of each line. */
static void build_chain(char *buf, size_t n, int randomize) {
    size_t *order = malloc(n * sizeof(size_t));
    for (size_t i = 0; i < n; i++) order[i] = i;
    if (randomize)
        for (size_t i = n - 1; i > 0; i--) {   /* Fisher-Yates */
            size_t j = rng() % (i + 1);
            size_t t = order[i]; order[i] = order[j]; order[j] = t;
        }
    for (size_t i = 0; i < n; i++)
        *(void **)(buf + order[i] * LINE) = (void *)(buf + order[(i + 1) % n] * LINE);
    free(order);
}

static double chase(char *buf, size_t n, long accesses) {
    void **p = (void **)buf;
    for (size_t i = 0; i < n; i++) p = (void **)*p;   /* warm every line */

    double best = 1e30;
    for (int rep = 0; rep < 3; rep++) {
        double t0 = now_s();
        for (long i = 0; i < accesses; i++) p = (void **)*p;
        double ns = (now_s() - t0) * 1e9 / accesses;
        if (ns < best) best = ns;
    }
    __asm__ volatile ("" :: "r"(p));
    return best;
}

/* Ask /proc/self/smaps how much of this range the kernel actually backed with
 * 2MB pages.  MADV_HUGEPAGE is a request, not a guarantee. */
static long anon_huge_kb(void *addr) {
    FILE *f = fopen("/proc/self/smaps", "r");
    if (!f) return -1;
    char line[512];
    unsigned long lo, hi, want = (unsigned long)addr;
    int in_range = 0;
    long kb = -1;
    while (fgets(line, sizeof line, f)) {
        if (sscanf(line, "%lx-%lx", &lo, &hi) == 2)
            in_range = (want >= lo && want < hi);
        long v;
        if (in_range && sscanf(line, "AnonHugePages: %ld kB", &v) == 1) { kb = v; break; }
    }
    fclose(f);
    return kb;
}

int main(int argc, char **argv) {
    int cpu = (argc > 1) ? atoi(argv[1]) : 0;
    pin(cpu);

    /* Spin for a second so the core is out of its low-power state before the
     * first data point; otherwise 8KB is measured at 3.5 GHz and everything
     * after it at 5.1 GHz. */
    fprintf(stderr, "warming up core %d...\n", cpu);
    measure_ghz(1.0);

    printf("mode,bytes,ns,cycles,ghz\n");

    size_t sizes[64]; int nsizes = 0;
    for (size_t s = 8UL << 10; s <= 256UL << 20; s = s * 3 / 2)
        sizes[nsizes++] = (s / LINE) * LINE;

    for (int i = 0; i < nsizes; i++) {
        size_t bytes = sizes[i], n = bytes / LINE;
        if (n < 16) continue;

        for (int m = 0; m < 3; m++) {
            const char *name = (m == 0) ? "random" : (m == 1) ? "seq" : "huge";

            char *buf = NULL;
            if (posix_memalign((void **)&buf, 2UL << 20, bytes) != 0) {
                perror("posix_memalign"); return 1;
            }
            /* modes 0/1 explicitly refuse huge pages, so the "huge" delta
             * isolates page size rather than luck of the allocator */
            madvise(buf, bytes, (m == 2) ? MADV_HUGEPAGE : MADV_NOHUGEPAGE);
            memset(buf, 0, bytes);
            build_chain(buf, n, m != 1);

            long accesses = 20L * 1000 * 1000;
            if (bytes > (32UL << 20)) accesses = 4L * 1000 * 1000;
            if ((size_t)accesses < 4 * n) accesses = 4 * n;

            double ghz = measure_ghz(0.05);   /* right before, not once at startup */
            double ns = chase(buf, n, accesses);
            printf("%s,%zu,%.3f,%.2f,%.3f\n", name, bytes, ns, ns * ghz, ghz);
            fflush(stdout);

            if (m == 2 && bytes >= (4UL << 20))
                fprintf(stderr, "  %zu MB: AnonHugePages=%ld kB\n",
                        bytes >> 20, anon_huge_kb(buf));
            free(buf);
        }
    }
    return 0;
}
