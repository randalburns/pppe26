/* core_to_core.c -- cache-line ping-pong latency between every pair of cores.
 *
 * Two threads share one cache line.  A writes an odd value, B spins until it
 * sees it and writes the next even value, and so on.  Each full exchange is
 * one round trip of the coherence protocol: the line must be invalidated in
 * one core's L1 and handed to the other through whatever level the two cores
 * share.  Half a round trip is the number quoted.
 *
 * This is the cost of *sharing*.  It is what a lock handoff, a producer /
 * consumer queue, an atomic counter, or a false-shared array element actually
 * pays.  On a uniform desktop chip it is one number.  On this part it is not:
 * Strix Point has two core complexes with different core types and separate
 * L3 slices, so the answer depends on which two cores are talking.
 *
 * Measurement notes, both learned the hard way on this machine:
 *   - Each thread runs a compute warmup before the timed section.  A spin loop
 *     built out of PAUSE does not raise the core's clock, so without the
 *     warmup the Zen 5c cores stay near idle frequency and cross-complex
 *     results scatter over 74-180 ns run to run.
 *   - The timed region is measured *inside* the parity-0 thread, after a
 *     barrier, so thread creation and warmup are outside the clock.
 *
 * Output is CSV on stdout: cpu_a,cpu_b,ns_min,ns_median,ns_max
 *
 * Build: gcc -O2 -pthread -o core_to_core core_to_core.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define ITERS   200000
#define WARMUP   20000

/* the contended line, alone on its own 64 bytes */
static _Alignas(64) _Atomic long seq;
static _Alignas(64) char keep_line_private[64];

static pthread_barrier_t barrier;

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

static void pin_self(int cpu) {
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(cpu, &set);
    pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
}

/* run a dependent add chain for `seconds` to pull this core up to its
 * boost clock before anything is timed */
static void spin_up(double seconds) {
    uint64_t x = 0;
    double t0 = now_s();
    while (now_s() - t0 < seconds)
        for (long i = 0; i < 20L * 1000 * 1000; i++)
            __asm__ volatile ("addq $1, %0" : "+r"(x));
    __asm__ volatile ("" :: "r"(x));
}

struct arg { int cpu, parity; double ns; };

static void *pong(void *v) {
    struct arg *a = v;
    pin_self(a->cpu);
    spin_up(0.2);

    /* warmup exchanges: get the line into the coherence steady state */
    pthread_barrier_wait(&barrier);
    for (long i = 0; i < WARMUP; i++) {
        long want = 2 * i + a->parity;
        while (atomic_load_explicit(&seq, memory_order_acquire) != want)
            __builtin_ia32_pause();
        atomic_store_explicit(&seq, want + 1, memory_order_release);
    }

    pthread_barrier_wait(&barrier);
    double t0 = now_s();
    for (long i = WARMUP; i < WARMUP + ITERS; i++) {
        long want = 2 * i + a->parity;
        while (atomic_load_explicit(&seq, memory_order_acquire) != want)
            __builtin_ia32_pause();
        atomic_store_explicit(&seq, want + 1, memory_order_release);
    }
    /* one iteration of each thread = one round trip = two handoffs */
    a->ns = (now_s() - t0) * 1e9 / ITERS / 2.0;
    return NULL;
}

static int cmp_double(const void *x, const void *y) {
    double a = *(const double *)x, b = *(const double *)y;
    return (a > b) - (a < b);
}

#define TRIALS 5

/* Reports min, median and max rather than a single number.  Within a complex
 * the five trials agree to a few percent; across complexes they do not -- the
 * same pair can measure 75 ns and 175 ns on consecutive trials, because the
 * cross-complex path on a mobile part is power-managed.  Collapsing that to
 * one number would hide the most useful thing the benchmark has to say. */
static void pair_ns(int cpu_a, int cpu_b, double out[3]) {
    double t[TRIALS];
    for (int trial = 0; trial < TRIALS; trial++) {
        atomic_store(&seq, 0);
        pthread_barrier_init(&barrier, NULL, 2);
        struct arg aa = { cpu_a, 0, 0 }, ab = { cpu_b, 1, 0 };
        pthread_t ta, tb;
        pthread_create(&ta, NULL, pong, &aa);
        pthread_create(&tb, NULL, pong, &ab);
        pthread_join(ta, NULL);
        pthread_join(tb, NULL);
        pthread_barrier_destroy(&barrier);
        t[trial] = aa.ns;
    }
    qsort(t, TRIALS, sizeof t[0], cmp_double);
    out[0] = t[0];
    out[1] = t[TRIALS / 2];
    out[2] = t[TRIALS - 1];
}

int main(int argc, char **argv) {
    int ncpu = (argc > 1) ? atoi(argv[1]) : 12;
    double r[3];
    printf("cpu_a,cpu_b,ns_min,ns_median,ns_max\n");
    for (int i = 0; i < ncpu; i++)
        for (int j = i + 1; j < ncpu; j++) {
            pair_ns(i, j, r);
            printf("%d,%d,%.1f,%.1f,%.1f\n", i, j, r[0], r[1], r[2]);
            fflush(stdout);
        }
    /* SMT siblings: cpu k and cpu k+12 are the two threads of one core and
     * share an L1, so the line never leaves the core */
    for (int i = 0; i < ncpu; i += 4) {
        pair_ns(i, i + 12, r);
        printf("%d,%d,%.1f,%.1f,%.1f\n", i, i + 12, r[0], r[1], r[2]);
        fflush(stdout);
    }
    (void)keep_line_private;
    return 0;
}
