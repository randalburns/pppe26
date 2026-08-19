#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

/*
 *  prefixsum.c
 *
 *  Prefix (inclusive scan): output[i] = input[0] + input[1] + ... + input[i]
 *
 *  The naive parallel attempt is wrong because each element depends on all
 *  previous elements. The correct parallel approach uses two passes:
 *
 *    Pass 1 (parallel): each thread prefix-sums its own chunk and saves the total.
 *    Pass 2 (serial):   prefix-sum the per-thread totals to get chunk offsets.
 *    Pass 3 (parallel): each thread adds its offset to every element in its chunk.
 *
 *  Compile (clang + libomp on macOS):
 *    clang -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      prefixsum.c -o prefixsum
 *
 *  Usage: ./prefixsum [N] [threads]
 */

int N = 100000000;
int NUM_THREADS = 10;
const int TRIALS = 4;

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

/* Serial prefix sum. */
void prefix_serial(double *in, double *out)
{
    out[0] = in[0];
    for (int i = 1; i < N; i++)
        out[i] = out[i-1] + in[i];
}

/*
 * Parallel prefix sum using the three-pass approach.
 *
 * Each thread works on a contiguous chunk of size ~N/T:
 *   Pass 1: local prefix sum within the chunk (parallel)
 *   Pass 2: prefix sum of the T chunk totals (serial, T is tiny)
 *   Pass 3: add the chunk offset to every element (parallel)
 */
void prefix_omp(double *in, double *out)
{
    double chunk_total[NUM_THREADS];

    /* Pass 1 — local prefix sum per thread */
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid   = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int chunk = (N + nthreads - 1) / nthreads;
        int start = tid * chunk;
        int end   = start + chunk < N ? start + chunk : N;

        if (start < N) {
            out[start] = in[start];
            for (int i = start + 1; i < end; i++)
                out[i] = out[i-1] + in[i];
            chunk_total[tid] = out[end - 1];
        } else {
            chunk_total[tid] = 0.0;
        }
    }

    /* Pass 2 — prefix sum of chunk totals (serial over T elements) */
    for (int t = 1; t < NUM_THREADS; t++)
        chunk_total[t] += chunk_total[t-1];

    /* Pass 3 — add offset to each chunk (parallel) */
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid   = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        int chunk = (N + nthreads - 1) / nthreads;
        int start = tid * chunk;
        int end   = start + chunk < N ? start + chunk : N;

        if (tid > 0 && start < N) {
            double offset = chunk_total[tid - 1];
            for (int i = start; i < end; i++)
                out[i] += offset;
        }
    }
}

/* Floating-point addition is not associative, so parallel and serial results
   will differ slightly in order. Check relative error instead of exact equality. */
int verify(double *a, double *b)
{
    for (int i = 0; i < N; i++) {
        double rel_err = fabs(a[i] - b[i]) / fabs(a[i]);
        if (rel_err > 1e-10) return 0;
    }
    return 1;
}

#define TIME(label, call) do { \
    gettimeofday(&begin, NULL); \
    call; \
    gettimeofday(&end, NULL); \
    timeval_subtract(&tresult, &begin, &end); \
    double secs = (double)tresult.tv_sec + (double)tresult.tv_usec/1000000.0; \
    printf("%-22s  time=%.4f s  GB/s=%.2f\n", label, secs, \
           2.0*N*sizeof(double) / secs / 1e9); \
} while(0)

int main(int argc, char *argv[])
{
    if (argc > 1) N = atoi(argv[1]);
    if (argc > 2) NUM_THREADS = atoi(argv[2]);
    printf("N = %d  threads = %d\n\n", N, NUM_THREADS);

    double *in      = malloc(N * sizeof(double));
    double *out_s   = malloc(N * sizeof(double));
    double *out_omp = malloc(N * sizeof(double));

    for (int i = 0; i < N; i++)
        in[i] = (double)rand() / RAND_MAX;

    struct timeval begin, end, tresult;

    // warm up
    prefix_serial(in, out_s);
    prefix_omp(in, out_omp);
    printf("results match: %s\n\n", verify(out_s, out_omp) ? "yes" : "NO");

    printf("%-22s  %s\n", "version", "time       GB/s");
    printf("%-22s  %s\n", "-------", "----       ----");
    for (int t = 0; t < TRIALS; t++) TIME("serial",       prefix_serial(in, out_s));
    for (int t = 0; t < TRIALS; t++) TIME("omp two-pass", prefix_omp(in, out_omp));

    free(in); free(out_s); free(out_omp);
    return 0;
}
