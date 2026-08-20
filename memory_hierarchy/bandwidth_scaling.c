/* bandwidth_scaling.c -- aggregate memory bandwidth vs thread count.
 *
 * One core cannot saturate DRAM.  A single Zen 5 core can have only so many
 * cache-line fills outstanding (its miss queue is finite), so single-thread
 * bandwidth is a latency-times-concurrency number, not the memory system's
 * limit.  Adding cores adds miss queues, and aggregate bandwidth climbs until
 * the memory controllers -- not the cores -- become the constraint.
 *
 * Where that curve flattens is the number that matters for parallel code: it
 * is the horizontal ceiling in a roofline model (see ../roofline/), and it is
 * why memory-bound loops stop speeding up long before you run out of cores.
 *
 * Each thread streams its own contiguous slice of one large shared array, so
 * there is no sharing and no false sharing -- only capacity.
 *
 * Output is CSV on stdout: threads,read_GBps
 *
 * Build: gcc -O3 -march=native -pthread -o bandwidth_scaling bandwidth_scaling.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

#define TOTAL_BYTES (1024UL << 20)      /* 1 GB: far past the 24 MB of L3 */

static uint64_t *arr;
static size_t total_elems;
static pthread_barrier_t barrier;
static volatile uint64_t sink;
static int reps;

struct targ { int cpu; int id; int nthreads; double seconds; };

static void *worker(void *v) {
    struct targ *a = v;
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(a->cpu, &set);
    pthread_setaffinity_np(pthread_self(), sizeof(set), &set);

    size_t chunk = total_elems / a->nthreads;
    uint64_t *base = arr + (size_t)a->id * chunk;

    for (int warm = 0; warm < 2; warm++) {          /* touch + raise the clock */
        uint64_t s = 0;
        for (size_t i = 0; i < chunk; i += 8) s += base[i];
        sink = s;
    }

    pthread_barrier_wait(&barrier);
    double t0 = now_s();
    for (int r = 0; r < reps; r++) {
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        for (size_t i = 0; i + 3 < chunk; i += 4) {
            s0 += base[i]; s1 += base[i+1]; s2 += base[i+2]; s3 += base[i+3];
        }
        sink = s0 + s1 + s2 + s3;
    }
    a->seconds = now_s() - t0;
    return NULL;
}

int main(void) {
    if (posix_memalign((void **)&arr, 2UL << 20, TOTAL_BYTES)) { perror("alloc"); return 1; }
    madvise(arr, TOTAL_BYTES, MADV_HUGEPAGE);
    memset(arr, 1, TOTAL_BYTES);
    total_elems = TOTAL_BYTES / sizeof(uint64_t);

    /* cpu 0-3 are the four Zen 5 cores, 4-11 the eight Zen 5c cores; taking
     * them in that order means thread counts 1-4 are all full-speed cores */
    int cpus[12]; for (int i = 0; i < 12; i++) cpus[i] = i;

    printf("threads,read_GBps\n");
    for (int n = 1; n <= 12; n++) {
        reps = 6;
        double best = 0;
        for (int trial = 0; trial < 3; trial++) {
            pthread_barrier_init(&barrier, NULL, n);
            pthread_t th[12]; struct targ ta[12];
            for (int i = 0; i < n; i++) {
                ta[i] = (struct targ){ cpus[i], i, n, 0 };
                pthread_create(&th[i], NULL, worker, &ta[i]);
            }
            /* aggregate over the slowest thread, not the sum of per-thread
             * rates: threads that finish early would otherwise be credited
             * with bandwidth they only got because the others had stopped */
            double slowest = 0;
            for (int i = 0; i < n; i++) {
                pthread_join(th[i], NULL);
                if (ta[i].seconds > slowest) slowest = ta[i].seconds;
            }
            pthread_barrier_destroy(&barrier);
            size_t chunk = total_elems / n;
            double gbps = (double)chunk * sizeof(uint64_t) * n * reps / slowest / 1e9;
            if (gbps > best) best = gbps;
        }
        printf("%d,%.2f\n", n, best);
        fflush(stdout);
    }
    return 0;
}
