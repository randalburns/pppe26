#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/*
 *  stencil.c
 *
 *  Successively optimized implementations of stencil computations:
 *    - randomly initialize a 2-D array with doubles in [0,1]
 *    - average a stencil of 2*HWIDTH+1 for each cell
 *    - sum two averaged stencils
 *
 *  Compile (clang + libomp on macOS):
 *    clang -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      stencil.c -o stencil
 *
 *  Compile (gcc):
 *    gcc -fopenmp -O3 stencil.c -o stencil
 */

const int DIM      = 4096;
const int TRIALS   = 4;
// HWIDTH = 2 matches the unrolled code; changing it breaks those comparisons.
const int HWIDTH   = 2;
const int NUM_THREADS = 10;

/* Subtract struct timeval values X and Y, storing result in RESULT.
   Returns 1 if the difference is negative. */
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

/* Initialize in column-major order (strided — slower). */
void initializeyx(double *array)
{
    for (int y = 0; y < DIM; y++)
        for (int x = 0; x < DIM; x++)
            array[x*DIM+y] = (double)rand() / RAND_MAX;
}

/* Initialize in row-major order (sequential — faster). */
void initializexy(double *array)
{
    for (int x = 0; x < DIM; x++)
        for (int y = 0; y < DIM; y++)
            array[x*DIM+y] = (double)rand() / RAND_MAX;
}

/* Naive serial averaging stencil over the interior. */
void stencil_average(double *input_ar, double *output_ar)
{
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial = 0.0;
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++)
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++)
                    partial += input_ar[DIM*(x+xs)+(y+ys)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
        }
    }
}

/* Outer loop parallelized with OpenMP. */
void stencil_average_omp(double *input_ar, double *output_ar)
{
    #pragma omp parallel for
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial = 0.0;
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++)
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++)
                    partial += input_ar[DIM*(x+xs)+(y+ys)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
        }
    }
}

/* Inner loop parallelized — loses performance due to per-row thread overhead. */
void stencil_average_omp_inner(double *input_ar, double *output_ar)
{
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        #pragma omp parallel for
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial = 0.0;
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++)
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++)
                    partial += input_ar[DIM*(x+xs)+(y+ys)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
        }
    }
}

/* Shared variable — loses performance due to implicit serialization. */
void stencil_average_omp_bad(double *input_ar, double *output_ar)
{
    double partial = 0.0;
    #pragma omp parallel for
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++)
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++)
                    partial += input_ar[DIM*(x+xs)+(y+ys)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
            partial = 0.0;
        }
    }
}

/* Per-thread partial array — demonstrates false sharing on the partial[] array. */
void stencil_average_omp_false(double *input_ar, double *output_ar)
{
    double partial[NUM_THREADS];
    memset(partial, 0, NUM_THREADS * sizeof(double));

    #pragma omp parallel for
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        int thread = omp_get_thread_num();
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++)
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++)
                    partial[thread] += input_ar[DIM*(x+xs)+(y+ys)];
            output_ar[DIM*x+y] = partial[thread] / ((2*HWIDTH+1)*(2*HWIDTH+1));
            partial[thread] = 0.0;
        }
    }
}

/* Unrolled inner stencil (HWIDTH=2 only). */
void stencil_average_unrolled(double *input_ar, double *output_ar)
{
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial;
            partial  = input_ar[DIM*(x-2)+(y-2)];
            partial += input_ar[DIM*(x-2)+(y-1)];
            partial += input_ar[DIM*(x-2)+(y  )];
            partial += input_ar[DIM*(x-2)+(y+1)];
            partial += input_ar[DIM*(x-2)+(y+2)];
            partial += input_ar[DIM*(x-1)+(y-2)];
            partial += input_ar[DIM*(x-1)+(y-1)];
            partial += input_ar[DIM*(x-1)+(y  )];
            partial += input_ar[DIM*(x-1)+(y+1)];
            partial += input_ar[DIM*(x-1)+(y+2)];
            partial += input_ar[DIM*(x  )+(y-2)];
            partial += input_ar[DIM*(x  )+(y-1)];
            partial += input_ar[DIM*(x  )+(y  )];
            partial += input_ar[DIM*(x  )+(y+1)];
            partial += input_ar[DIM*(x  )+(y+2)];
            partial += input_ar[DIM*(x+1)+(y-2)];
            partial += input_ar[DIM*(x+1)+(y-1)];
            partial += input_ar[DIM*(x+1)+(y  )];
            partial += input_ar[DIM*(x+1)+(y+1)];
            partial += input_ar[DIM*(x+1)+(y+2)];
            partial += input_ar[DIM*(x+2)+(y-2)];
            partial += input_ar[DIM*(x+2)+(y-1)];
            partial += input_ar[DIM*(x+2)+(y  )];
            partial += input_ar[DIM*(x+2)+(y+1)];
            partial += input_ar[DIM*(x+2)+(y+2)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
        }
    }
}

/* Unrolled stencil parallelized with OpenMP. */
void stencil_average_unrolled_omp(double *input_ar, double *output_ar)
{
    #pragma omp parallel for
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial;
            partial  = input_ar[DIM*(x-2)+(y-2)];
            partial += input_ar[DIM*(x-2)+(y-1)];
            partial += input_ar[DIM*(x-2)+(y  )];
            partial += input_ar[DIM*(x-2)+(y+1)];
            partial += input_ar[DIM*(x-2)+(y+2)];
            partial += input_ar[DIM*(x-1)+(y-2)];
            partial += input_ar[DIM*(x-1)+(y-1)];
            partial += input_ar[DIM*(x-1)+(y  )];
            partial += input_ar[DIM*(x-1)+(y+1)];
            partial += input_ar[DIM*(x-1)+(y+2)];
            partial += input_ar[DIM*(x  )+(y-2)];
            partial += input_ar[DIM*(x  )+(y-1)];
            partial += input_ar[DIM*(x  )+(y  )];
            partial += input_ar[DIM*(x  )+(y+1)];
            partial += input_ar[DIM*(x  )+(y+2)];
            partial += input_ar[DIM*(x+1)+(y-2)];
            partial += input_ar[DIM*(x+1)+(y-1)];
            partial += input_ar[DIM*(x+1)+(y  )];
            partial += input_ar[DIM*(x+1)+(y+1)];
            partial += input_ar[DIM*(x+1)+(y+2)];
            partial += input_ar[DIM*(x+2)+(y-2)];
            partial += input_ar[DIM*(x+2)+(y-1)];
            partial += input_ar[DIM*(x+2)+(y  )];
            partial += input_ar[DIM*(x+2)+(y+1)];
            partial += input_ar[DIM*(x+2)+(y+2)];
            output_ar[DIM*x+y] = partial / ((2*HWIDTH+1)*(2*HWIDTH+1));
        }
    }
}

/* Serial array sum. */
void array_sum(double *input_ar1, double *input_ar2, double *output_ar)
{
    for (int x = 0; x < DIM; x++)
        for (int y = 0; y < DIM; y++)
            output_ar[x*DIM+y] = input_ar1[x*DIM+y] + input_ar2[x*DIM+y];
}

/* Array sum parallelized with OpenMP. */
void array_sum_omp(double *input_ar1, double *input_ar2, double *output_ar)
{
    #pragma omp parallel for
    for (int x = 0; x < DIM; x++)
        for (int y = 0; y < DIM; y++)
            output_ar[x*DIM+y] = input_ar1[x*DIM+y] + input_ar2[x*DIM+y];
}

/* Fused stencil + sum in a single parallel loop (loop fusion). */
void fused_stencil_sum_omp(double *input_ar1, double *input_ar2, double *output_ar)
{
    #pragma omp parallel for
    for (int x = HWIDTH; x < DIM-HWIDTH; x++) {
        for (int y = HWIDTH; y < DIM-HWIDTH; y++) {
            double partial1 = 0.0, partial2 = 0.0;
            for (int xs = -HWIDTH; xs <= HWIDTH; xs++) {
                for (int ys = -HWIDTH; ys <= HWIDTH; ys++) {
                    partial1 += input_ar1[DIM*(x+xs)+(y+ys)];
                    partial2 += input_ar2[DIM*(x+xs)+(y+ys)];
                }
            }
            double denom = (2*HWIDTH+1) * (2*HWIDTH+1);
            output_ar[DIM*x+y] = partial1/denom + partial2/denom;
        }
    }
}

/* Race condition — shared max_el with no synchronization (intentionally wrong). */
void max_el_shared(double *input_ar)
{
    double max_el = 0;
    #pragma omp parallel for shared(max_el)
    for (int x = 0; x < DIM; x++)
        for (int y = 0; y < DIM; y++)
            max_el = max_el > input_ar[x*DIM+y] ? max_el : input_ar[x*DIM+y];
}

/* Critical section — correct but serializes every update (very slow). */
void max_el_critical(double *input_ar)
{
    double max_el = 0;
    #pragma omp parallel
    for (int x = 0; x < DIM; x++) {
        for (int y = 0; y < DIM; y++) {
            #pragma omp critical
            max_el = max_el > input_ar[x*DIM+y] ? max_el : input_ar[x*DIM+y];
        }
    }
}

/* Reduction — correct and efficient. */
void max_el_reduce(double *input_ar)
{
    double max_el = 0;
    #pragma omp parallel for reduction(max: max_el)
    for (int x = 0; x < DIM; x++)
        for (int y = 0; y < DIM; y++)
            max_el = max_el > input_ar[x*DIM+y] ? max_el : input_ar[x*DIM+y];
}

#define TIME(label, call) \
    gettimeofday(&begin, NULL); \
    call; \
    gettimeofday(&end, NULL); \
    timeval_subtract(&tresult, &begin, &end); \
    printf("%-40s %f\n", label, (double)tresult.tv_sec + (double)tresult.tv_usec/1000000.0);

int main()
{
    omp_set_num_threads(NUM_THREADS);

    double *rand_ar1 = malloc(DIM * DIM * sizeof(double));
    double *rand_ar2 = malloc(DIM * DIM * sizeof(double));
    double *avg_ar1  = malloc(DIM * DIM * sizeof(double));
    double *avg_ar2  = malloc(DIM * DIM * sizeof(double));
    double *sum_ar   = malloc(DIM * DIM * sizeof(double));

    struct timeval begin, end, tresult;

    // warm up
    initializexy(rand_ar1);
    initializexy(rand_ar2);

    printf("\n--- Initialization order ---\n");
    for (int t = 0; t < TRIALS; t++) {
        TIME("xy (row-major)",    initializexy(rand_ar1));
        TIME("yx (column-major)", initializeyx(rand_ar1));
    }

    // warm up stencil output arrays
    stencil_average(rand_ar1, avg_ar1);
    stencil_average(rand_ar2, avg_ar2);

    printf("\n--- Stencil variants ---\n");
    for (int t = 0; t < TRIALS; t++) TIME("serial",                stencil_average(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("omp outer",             stencil_average_omp(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("omp inner (slow)",      stencil_average_omp_inner(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("omp shared var (bad)",  stencil_average_omp_bad(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("omp false sharing",     stencil_average_omp_false(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("unrolled serial",       stencil_average_unrolled(rand_ar1, avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("unrolled omp",          stencil_average_unrolled_omp(rand_ar1, avg_ar1));

    // warm up sum array
    array_sum(avg_ar1, avg_ar2, sum_ar);

    printf("\n--- Fusion ---\n");
    for (int t = 0; t < TRIALS; t++) {
        TIME("separate loops", {
            stencil_average_omp(rand_ar1, avg_ar1);
            stencil_average_omp(rand_ar2, avg_ar2);
            array_sum_omp(avg_ar1, avg_ar2, sum_ar);
        });
        TIME("fused loop", fused_stencil_sum_omp(rand_ar1, rand_ar2, sum_ar));
    }

    printf("\n--- Reduction patterns ---\n");
    for (int t = 0; t < TRIALS; t++) TIME("max shared (race)",  max_el_shared(avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("max reduction",      max_el_reduce(avg_ar1));
    for (int t = 0; t < TRIALS; t++) TIME("max critical (slow)", max_el_critical(avg_ar1));

    free(rand_ar1); free(rand_ar2);
    free(avg_ar1);  free(avg_ar2);
    free(sum_ar);
    return 0;
}
