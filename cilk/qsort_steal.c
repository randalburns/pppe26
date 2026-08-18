#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/*
 *  qsort_steal.c  --  where Cilk work-stealing beats OpenMP
 *
 *  Parallel quicksort. The parallelism here is RECURSIVE and IRREGULAR:
 *  every partition splits the range into two pieces of unpredictable,
 *  data-dependent size, and each piece recursively spawns more work.
 *  Nobody knows in advance how deep or how wide any subtree will be.
 *
 *  This is the workload work-stealing was built for, and it is exactly
 *  where OpenMP struggles:
 *
 *    - An OpenMP `parallel for` cannot express recursion at all.
 *    - OpenMP `task` can, but every task is eagerly allocated on a heap
 *      queue and handed to the runtime -- hundreds of ns per task. With
 *      millions of small subranges that overhead dominates.
 *    - Cilk `cilk_spawn` is nearly free: the continuation is just marked
 *      stealable and the worker keeps running. An idle worker only pays
 *      the cost when it actually steals. Load balancing is automatic:
 *      whichever worker goes idle grabs the deepest available subtree.
 *
 *  Both parallel versions share the SAME partition, the SAME median-of-three
 *  pivot, and the SAME serial cutoff -- the only difference is the primitive
 *  used to go parallel (cilk_spawn vs omp task). So the timing gap is the
 *  scheduler, nothing else.
 *
 *  Compile (needs both OpenCilk and libomp):
 *    ~/opencilk/bin/clang -fopencilk -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      qsort_steal.c -o qsort_steal
 *
 *  Usage: ./qsort_steal [N] [cutoff]     (defaults: N=50000000, cutoff=8192)
 *
 *  Match worker counts for a fair fight:
 *    CILK_NWORKERS=8 OMP_NUM_THREADS=8 ./qsort_steal
 */

long N      = 50000000;   /* elements to sort                              */
long CUTOFF = 8192;       /* below this, sort serially (no more spawning)  */
const int TRIALS = 3;

int timeval_subtract(struct timeval *result, struct timeval *y, struct timeval *x)
{
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec  += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec  -= nsec;
    }
    result->tv_sec  = x->tv_sec  - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    return x->tv_sec < y->tv_sec;
}

static inline void swap(int *a, long i, long j) { int t = a[i]; a[i] = a[j]; a[j] = t; }

/* insertion sort for the very bottom of the recursion */
static void insertion_sort(int *a, long lo, long hi)
{
    for (long i = lo + 1; i <= hi; i++) {
        int v = a[i];
        long j = i - 1;
        while (j >= lo && a[j] > v) { a[j+1] = a[j]; j--; }
        a[j+1] = v;
    }
}

/* median-of-three pivot value from a[lo], a[mid], a[hi] */
static inline int median3(int *a, long lo, long hi)
{
    long mid = lo + (hi - lo) / 2;
    int x = a[lo], y = a[mid], z = a[hi];
    if (x < y) { if (y < z) return y; return (x < z) ? z : x; }
    else       { if (x < z) return x; return (y < z) ? z : y; }
}

/* Hoare partition: returns j; caller recurses on [lo,j] and [j+1,hi] */
static long partition(int *a, long lo, long hi)
{
    int pivot = median3(a, lo, hi);
    long i = lo - 1, j = hi + 1;
    for (;;) {
        do { i++; } while (a[i] < pivot);
        do { j--; } while (a[j] > pivot);
        if (i >= j) return j;
        swap(a, i, j);
    }
}

/* Plain serial quicksort -- also used as the base case by both parallel versions */
static void qsort_serial(int *a, long lo, long hi)
{
    while (hi - lo > 16) {
        long p = partition(a, lo, hi);
        /* recurse into the smaller side, loop on the larger (bounded stack) */
        if (p - lo < hi - (p + 1)) { qsort_serial(a, lo, p); lo = p + 1; }
        else                       { qsort_serial(a, p + 1, hi); hi = p; }
    }
    insertion_sort(a, lo, hi);
}

/* Cilk: spawn the left side, recurse right, sync. */
static void qsort_cilk(int *a, long lo, long hi)
{
    if (hi - lo <= CUTOFF) { qsort_serial(a, lo, hi); return; }
    long p = partition(a, lo, hi);
    cilk_spawn qsort_cilk(a, lo, p);
    qsort_cilk(a, p + 1, hi);
    cilk_sync;
}

/* OpenMP: same shape, but each half is an omp task. */
static void qsort_omp(int *a, long lo, long hi)
{
    if (hi - lo <= CUTOFF) { qsort_serial(a, lo, hi); return; }
    long p = partition(a, lo, hi);
    #pragma omp task
    qsort_omp(a, lo, p);
    qsort_omp(a, p + 1, hi);
    #pragma omp taskwait
}

static void run_omp(int *a, long n, int nthreads)
{
    #pragma omp parallel num_threads(nthreads)
    #pragma omp single nowait
    qsort_omp(a, 0, n - 1);
}

int sorted(int *a, long n)
{
    for (long i = 1; i < n; i++)
        if (a[i-1] > a[i]) return 0;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc > 1) N      = atol(argv[1]);
    if (argc > 2) CUTOFF = atol(argv[2]);

    int nworkers = __cilkrts_get_nworkers();
    int nthreads = omp_get_max_threads();
    printf("N = %ld   cutoff = %ld   cilk workers = %d   omp threads = %d\n\n",
           N, CUTOFF, nworkers, nthreads);

    int *orig = malloc(N * sizeof(int));
    int *work = malloc(N * sizeof(int));
    srand(12345);
    for (long i = 0; i < N; i++) orig[i] = rand();

    struct timeval begin, end, tresult;

    #define RUN(label, call) do { \
        memcpy(work, orig, N * sizeof(int)); \
        gettimeofday(&begin, NULL); \
        call; \
        gettimeofday(&end, NULL); \
        timeval_subtract(&tresult, &begin, &end); \
        double secs = (double)tresult.tv_sec + (double)tresult.tv_usec/1e6; \
        printf("%-14s  %.4f s   sorted=%s\n", label, secs, sorted(work, N) ? "yes" : "NO"); \
    } while(0)

    /* warm up + correctness */
    RUN("serial",  qsort_serial(work, 0, N - 1));
    RUN("cilk",    qsort_cilk(work, 0, N - 1));
    RUN("omp task", run_omp(work, N, nthreads));
    printf("\n%-14s  %s\n", "version", "time");
    printf("%-14s  %s\n", "-------", "----");
    for (int t = 0; t < TRIALS; t++) RUN("serial",   qsort_serial(work, 0, N - 1));
    for (int t = 0; t < TRIALS; t++) RUN("omp task", run_omp(work, N, nthreads));
    for (int t = 0; t < TRIALS; t++) RUN("cilk",     qsort_cilk(work, 0, N - 1));

    free(orig); free(work);
    return 0;
}
