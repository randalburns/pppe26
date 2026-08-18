#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/*
 *  mergesort_balanced.c  --  where OpenMP beats Cilk work-stealing
 *
 *  The mirror image of qsort_steal.c. Bottom-up merge sort divides the
 *  array in HALF every time, independent of the data, so the work is
 *  perfectly REGULAR and BALANCED:
 *
 *    - Leaf pass:  n/LEAF blocks, every block exactly LEAF elements.
 *    - Merge pass w: n/(2w) merges, every merge exactly the same size.
 *
 *  Every iteration of every parallel loop costs the same, and the sizes
 *  are known before the loop runs. There is nothing to load-balance.
 *
 *  That is precisely the case OpenMP is built for and where Cilk loses:
 *
 *    - OpenMP `for schedule(static)` splits each pass into P equal
 *      contiguous chunks ONCE, with essentially zero per-iteration
 *      scheduling cost. Static division is optimal because the work
 *      already is balanced.
 *    - Cilk `cilk_for` still recursively halves the range into a tree of
 *      spawns and runs the work-stealing scheduler -- maintaining deques
 *      and firing random steal attempts -- every single pass. On balanced
 *      work that adaptivity buys nothing and its overhead is pure cost.
 *
 *  Both versions run the identical algorithm and buffers; the only
 *  difference is the parallel-loop primitive. So the timing gap is the
 *  scheduler, nothing else.
 *
 *  Compile (needs both OpenCilk and libomp):
 *    ~/opencilk/bin/clang -fopencilk -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      mergesort_balanced.c -o mergesort_balanced
 *
 *  Usage: ./mergesort_balanced [N]        (default N=50000000)
 *
 *  Match worker counts for a fair fight:
 *    CILK_NWORKERS=8 OMP_NUM_THREADS=8 ./mergesort_balanced
 */

long N = 50000000;        /* elements to sort                */
#define LEAF 32           /* leaf block sorted with insertion */
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

static inline long lmin(long a, long b) { return a < b ? a : b; }

static void insertion_sort(int *a, long lo, long hi)   /* [lo,hi) */
{
    for (long i = lo + 1; i < hi; i++) {
        int v = a[i];
        long j = i - 1;
        while (j >= lo && a[j] > v) { a[j+1] = a[j]; j--; }
        a[j+1] = v;
    }
}

/* merge src[lo,mid) and src[mid,hi) into dst[lo,hi) */
static void merge(const int *src, int *dst, long lo, long mid, long hi)
{
    long i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
        dst[k++] = (src[i] <= src[j]) ? src[i++] : src[j++];
    while (i < mid) dst[k++] = src[i++];
    while (j < hi)  dst[k++] = src[j++];
}

/* ---- serial baseline -------------------------------------------------- */
static void mergesort_serial(int *a, int *buf, long n)
{
    for (long lo = 0; lo < n; lo += LEAF)
        insertion_sort(a, lo, lmin(lo + LEAF, n));

    int *src = a, *dst = buf;
    for (long w = LEAF; w < n; w *= 2) {
        for (long lo = 0; lo < n; lo += 2*w)
            merge(src, dst, lo, lmin(lo + w, n), lmin(lo + 2*w, n));
        int *t = src; src = dst; dst = t;
    }
    if (src != a) memcpy(a, src, n * sizeof(int));
}

/* ---- OpenMP: static-scheduled parallel loops -------------------------- */
static void mergesort_omp(int *a, int *buf, long n, int nthreads)
{
    /* Each thread keeps its own src/dst pointers but computes the identical
       swap sequence, so they stay in lock-step across the implicit barrier
       at the end of every `omp for`. */
    #pragma omp parallel num_threads(nthreads)
    {
        #pragma omp for schedule(static)
        for (long lo = 0; lo < n; lo += LEAF)
            insertion_sort(a, lo, lmin(lo + LEAF, n));

        int *src = a, *dst = buf;
        for (long w = LEAF; w < n; w *= 2) {
            #pragma omp for schedule(static)
            for (long lo = 0; lo < n; lo += 2*w)
                merge(src, dst, lo, lmin(lo + w, n), lmin(lo + 2*w, n));
            int *t = src; src = dst; dst = t;
        }
    }

    /* After k merge passes the result sits in `a` when k is even, `buf` when
       odd. Same copy-back all three versions pay, so the comparison stays fair. */
    long k = 0;
    for (long w = LEAF; w < n; w *= 2) k++;
    if (k & 1) memcpy(a, buf, n * sizeof(int));
}

/* ---- Cilk: cilk_for parallel loops (work-stealing) -------------------- */
static void mergesort_cilk(int *a, int *buf, long n)
{
    cilk_for (long lo = 0; lo < n; lo += LEAF)
        insertion_sort(a, lo, lmin(lo + LEAF, n));

    int *src = a, *dst = buf;
    for (long w = LEAF; w < n; w *= 2) {
        cilk_for (long lo = 0; lo < n; lo += 2*w)
            merge(src, dst, lo, lmin(lo + w, n), lmin(lo + 2*w, n));
        int *t = src; src = dst; dst = t;
    }
    if (src != a) memcpy(a, src, n * sizeof(int));
}

int sorted(int *a, long n)
{
    for (long i = 1; i < n; i++)
        if (a[i-1] > a[i]) return 0;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc > 1) N = atol(argv[1]);

    int nworkers = __cilkrts_get_nworkers();
    int nthreads = omp_get_max_threads();
    printf("N = %ld   leaf = %d   cilk workers = %d   omp threads = %d\n\n",
           N, LEAF, nworkers, nthreads);

    int *orig = malloc(N * sizeof(int));
    int *work = malloc(N * sizeof(int));
    int *buf  = malloc(N * sizeof(int));
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
    RUN("serial", mergesort_serial(work, buf, N));
    RUN("omp",    mergesort_omp(work, buf, N, nthreads));
    RUN("cilk",   mergesort_cilk(work, buf, N));

    printf("\n%-14s  %s\n", "version", "time");
    printf("%-14s  %s\n", "-------", "----");
    for (int t = 0; t < TRIALS; t++) RUN("serial", mergesort_serial(work, buf, N));
    for (int t = 0; t < TRIALS; t++) RUN("cilk",   mergesort_cilk(work, buf, N));
    for (int t = 0; t < TRIALS; t++) RUN("omp",    mergesort_omp(work, buf, N, nthreads));

    free(orig); free(work); free(buf);
    return 0;
}
