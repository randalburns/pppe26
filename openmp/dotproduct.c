#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

/*
 *  dotproduct.c
 *
 *  Vector dot product: result = sum( A[i] * B[i] )
 *  Compares serial vs OpenMP parallel reduction.
 *
 *  Compile (clang + libomp on macOS):
 *    clang -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      dotproduct.c -o dotproduct
 *
 *  Usage: ./dotproduct [N]   (default N=100000000)
 */

int N = 100000000;
const int TRIALS = 4;
int NUM_THREADS = 10;

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

double dot_serial(double *A, double *B)
{
    double sum = 0.0;
    for (int i = 0; i < N; i++)
        sum += A[i] * B[i];
    return sum;
}

double dot_omp(double *A, double *B)
{
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++)
        sum += A[i] * B[i];
    return sum;
}

#define TIME(label, call) do { \
    gettimeofday(&begin, NULL); \
    double result = call; \
    gettimeofday(&end, NULL); \
    timeval_subtract(&tresult, &begin, &end); \
    double secs = (double)tresult.tv_sec + (double)tresult.tv_usec/1000000.0; \
    printf("%-20s result=%.6f  time=%.4f s  GB/s=%.2f\n", \
           label, result, secs, \
           2.0*N*sizeof(double) / secs / 1e9); \
} while(0)

int main(int argc, char *argv[])
{
    if (argc > 1) N = atoi(argv[1]);
    if (argc > 2) NUM_THREADS = atoi(argv[2]);
    printf("N = %d  threads = %d\n\n", N, NUM_THREADS);
    omp_set_num_threads(NUM_THREADS);

    double *A = malloc(N * sizeof(double));
    double *B = malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }

    struct timeval begin, end, tresult;

    printf("%-20s %-28s %s\n", "version", "result", "time       GB/s");
    printf("%-20s %-28s %s\n", "-------", "------", "----       ----");
    for (int t = 0; t < TRIALS; t++) TIME("serial",     dot_serial(A, B));
    for (int t = 0; t < TRIALS; t++) TIME("omp reduction", dot_omp(A, B));

    free(A); free(B);
    return 0;
}
