#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/*
 *  sparse_col_sum.c
 *
 *  Count non-zeros per column of a sparse matrix stored in CSR format.
 *
 *  col_count[j] = number of non-zeros in column j
 *
 *  The matrix is constructed with severe load imbalance:
 *    - First NHEAVY rows are dense (HEAVY_NNZ elements each)
 *    - Remaining rows are sparse (LIGHT_NNZ elements each)
 *    - All heavy rows placed at the start so static scheduling
 *      assigns them all to the first thread(s)
 *
 *  This makes the example interesting:
 *    - OpenMP static:   thread 0 gets all heavy rows → severe imbalance
 *    - OpenMP dynamic:  adapts but has per-row scheduling overhead
 *    - Cilk cilk_for:   work-stealing adapts with very low overhead
 *
 *  All parallel versions use per-worker local histograms to avoid
 *  atomic updates to col_count, then merge at the end.
 *
 *  Compile:
 *    ~/opencilk/bin/clang -fopencilk -Xpreprocessor -fopenmp -O3 \
 *      -I/opt/homebrew/opt/libomp/include \
 *      -L/opt/homebrew/opt/libomp/lib -lomp \
 *      sparse_col_sum.c -o sparse_col_sum
 *
 *  Usage: ./sparse_col_sum [nrows] [ncols] [nheavy] [heavy_nnz] [light_nnz]
 */

int NROWS      = 10000;
int NCOLS      = 10000;
int NHEAVY     = 100;     /* dense rows at the start */
int HEAVY_NNZ  = 5000;    /* non-zeros per heavy row */
int LIGHT_NNZ  = 10;      /* non-zeros per light row */
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

/* Build a CSR matrix with the prescribed heavy/light row pattern.
   Column indices within each row are chosen randomly without replacement. */
void build_csr(int **row_ptr_out, int **col_idx_out)
{
    int nnz = NHEAVY * HEAVY_NNZ + (NROWS - NHEAVY) * LIGHT_NNZ;
    int *row_ptr = malloc((NROWS + 1) * sizeof(int));
    int *col_idx = malloc(nnz * sizeof(int));
    int *tmp     = malloc(NCOLS * sizeof(int));

    int pos = 0;
    for (int r = 0; r < NROWS; r++) {
        row_ptr[r] = pos;
        int k = (r < NHEAVY) ? HEAVY_NNZ : LIGHT_NNZ;

        /* Fisher-Yates shuffle on [0,NCOLS) to pick k distinct columns */
        for (int c = 0; c < NCOLS; c++) tmp[c] = c;
        for (int i = 0; i < k; i++) {
            int j = i + rand() % (NCOLS - i);
            int t = tmp[i]; tmp[i] = tmp[j]; tmp[j] = t;
            col_idx[pos++] = tmp[i];
        }
    }
    row_ptr[NROWS] = pos;
    free(tmp);
    *row_ptr_out = row_ptr;
    *col_idx_out = col_idx;
}

/* Serial baseline */
void count_serial(int *row_ptr, int *col_idx, int *col_count)
{
    memset(col_count, 0, NCOLS * sizeof(int));
    for (int r = 0; r < NROWS; r++)
        for (int p = row_ptr[r]; p < row_ptr[r+1]; p++)
            col_count[col_idx[p]]++;
}

/* OpenMP static schedule — threads get fixed contiguous row ranges.
   Thread 0 receives all NHEAVY dense rows → load imbalance. */
void count_omp_static(int *row_ptr, int *col_idx, int *col_count)
{
    int *local = calloc(NUM_THREADS * NCOLS, sizeof(int));

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        int *lc = local + tid * NCOLS;
        #pragma omp for schedule(static)
        for (int r = 0; r < NROWS; r++)
            for (int p = row_ptr[r]; p < row_ptr[r+1]; p++)
                lc[col_idx[p]]++;
    }

    memset(col_count, 0, NCOLS * sizeof(int));
    for (int t = 0; t < NUM_THREADS; t++)
        for (int c = 0; c < NCOLS; c++)
            col_count[c] += local[t * NCOLS + c];
    free(local);
}

/* OpenMP dynamic schedule — threads pull rows from a shared queue.
   Adapts to imbalance but incurs per-row scheduling overhead. */
void count_omp_dynamic(int *row_ptr, int *col_idx, int *col_count)
{
    int *local = calloc(NUM_THREADS * NCOLS, sizeof(int));

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();
        int *lc = local + tid * NCOLS;
        #pragma omp for schedule(dynamic, 1)
        for (int r = 0; r < NROWS; r++)
            for (int p = row_ptr[r]; p < row_ptr[r+1]; p++)
                lc[col_idx[p]]++;
    }

    memset(col_count, 0, NCOLS * sizeof(int));
    for (int t = 0; t < NUM_THREADS; t++)
        for (int c = 0; c < NCOLS; c++)
            col_count[c] += local[t * NCOLS + c];
    free(local);
}

/*
 * Cilk cilk_for — work-stealing scheduler distributes rows automatically.
 * Uses __cilkrts_get_worker_number() for per-worker local histograms.
 *
 * Grain size = 1: each row is a separate stealable task.  Without this,
 * the default grain lumps all heavy rows into one indivisible chunk,
 * eliminating the load-balancing benefit of work-stealing.
 */
void count_cilk(int *row_ptr, int *col_idx, int *col_count)
{
    int nworkers = __cilkrts_get_nworkers();
    int *local   = calloc(nworkers * NCOLS, sizeof(int));

    #pragma cilk grainsize(1)
    cilk_for (int r = 0; r < NROWS; r++) {
        int wid = __cilkrts_get_worker_number();
        int *lc = local + wid * NCOLS;
        for (int p = row_ptr[r]; p < row_ptr[r+1]; p++)
            lc[col_idx[p]]++;
    }

    memset(col_count, 0, NCOLS * sizeof(int));
    for (int w = 0; w < nworkers; w++)
        for (int c = 0; c < NCOLS; c++)
            col_count[c] += local[w * NCOLS + c];
    free(local);
}

int verify(int *a, int *b)
{
    for (int c = 0; c < NCOLS; c++)
        if (a[c] != b[c]) return 0;
    return 1;
}

#define TIME(label, call) do { \
    gettimeofday(&begin, NULL); \
    call; \
    gettimeofday(&end, NULL); \
    timeval_subtract(&tresult, &begin, &end); \
    double secs = (double)tresult.tv_sec + (double)tresult.tv_usec/1000000.0; \
    printf("%-22s  %.4f s\n", label, secs); \
} while(0)

int main(int argc, char *argv[])
{
    if (argc > 1) NROWS      = atoi(argv[1]);
    if (argc > 2) NCOLS      = atoi(argv[2]);
    if (argc > 3) NHEAVY     = atoi(argv[3]);
    if (argc > 4) HEAVY_NNZ  = atoi(argv[4]);
    if (argc > 5) LIGHT_NNZ  = atoi(argv[5]);

    int nnz = NHEAVY * HEAVY_NNZ + (NROWS - NHEAVY) * LIGHT_NNZ;
    printf("matrix: %d x %d   nnz=%d   heavy=%d rows x %d nnz   light=%d rows x %d nnz\n\n",
           NROWS, NCOLS, nnz, NHEAVY, HEAVY_NNZ, NROWS-NHEAVY, LIGHT_NNZ);

    int *row_ptr, *col_idx;
    build_csr(&row_ptr, &col_idx);

    int *ref    = malloc(NCOLS * sizeof(int));
    int *result = malloc(NCOLS * sizeof(int));

    count_serial(row_ptr, col_idx, ref);

    // verify all parallel versions
    count_omp_static(row_ptr, col_idx, result);
    printf("omp static  correct: %s\n", verify(ref, result) ? "yes" : "NO");
    count_omp_dynamic(row_ptr, col_idx, result);
    printf("omp dynamic correct: %s\n", verify(ref, result) ? "yes" : "NO");
    count_cilk(row_ptr, col_idx, result);
    printf("cilk        correct: %s\n\n", verify(ref, result) ? "yes" : "NO");

    struct timeval begin, end, tresult;

    printf("%-22s  %s\n", "version", "time");
    printf("%-22s  %s\n", "-------", "----");
    for (int t = 0; t < TRIALS; t++) TIME("serial",       count_serial(row_ptr, col_idx, ref));
    for (int t = 0; t < TRIALS; t++) TIME("omp static",   count_omp_static(row_ptr, col_idx, result));
    for (int t = 0; t < TRIALS; t++) TIME("omp dynamic",  count_omp_dynamic(row_ptr, col_idx, result));
    for (int t = 0; t < TRIALS; t++) TIME("cilk",         count_cilk(row_ptr, col_idx, result));

    free(row_ptr); free(col_idx); free(ref); free(result);
    return 0;
}
