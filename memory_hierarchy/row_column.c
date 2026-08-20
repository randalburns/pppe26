/* row_column.c -- the canonical nested-loop experiment, measured per element,
 * with the power-of-two trap separated out.
 *
 * The same N*N writes in the two possible loop orders.  Row order walks memory
 * with stride 8 bytes; column order walks it with stride lda*8 bytes, so every
 * access lands on a different cache line -- and, past a page, a different page.
 *
 * Cache-line granularity alone predicts 8x: a 64-byte line holds 8 doubles and
 * column order uses one of them per fetch.  The measured ratio is much larger
 * than 8x, and the reason is a second effect hiding inside the first.  L1 has
 * 64 sets and picks the set with address bits 6-11.  When the row length is a
 * power of two, a column stride of lda*8 bytes advances the set index by a
 * fixed power-of-two amount, so an entire column lands in a handful of sets
 * and evicts itself despite the cache having plenty of room.
 *
 * So each size is run twice: with the rows packed (lda = n, power of two) and
 * with one element of padding (lda = n+1), which makes the column stride odd
 * in units of cache lines and spreads a column over every set.  The padded
 * column time is the honest "cost of the wrong loop order"; the gap between
 * padded and unpadded is the cost of the power-of-two layout.
 *
 * Output is CSV on stdout: n,lda,row_ns,col_ns,ratio
 *
 * Build: gcc -O2 -fno-tree-vectorize -o row_column row_column.c
 * (-fno-tree-vectorize because GCC vectorizes the row-order loop into wide
 *  stores at -O2 and the comparison stops being about access order alone.)
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

static void run(int n, int lda) {
    size_t elems = (size_t)n * lda;
    double *a = malloc(elems * sizeof(double));
    if (!a) { perror("malloc"); exit(1); }
    memset(a, 0, elems * sizeof(double));          /* fault every page in first */

    size_t touched = (size_t)n * n;
    int reps = (int)(1e8 / touched); if (reps < 2) reps = 2;

    double rbest = 1e30, cbest = 1e30;
    for (int t = 0; t < 3; t++) {
        double t0 = now_s();
        for (int r = 0; r < reps; r++)
            for (int x = 0; x < n; x++)            /* row order: y contiguous */
                for (int y = 0; y < n; y++)
                    a[(size_t)x * lda + y] = 1.0;
        double dt = (now_s() - t0) / reps;
        if (dt < rbest) rbest = dt;

        t0 = now_s();
        for (int r = 0; r < reps; r++)
            for (int y = 0; y < n; y++)            /* column order: stride lda */
                for (int x = 0; x < n; x++)
                    a[(size_t)x * lda + y] = 1.0;
        dt = (now_s() - t0) / reps;
        if (dt < cbest) cbest = dt;
    }
    printf("%d,%d,%.3f,%.3f,%.2f\n", n, lda,
           rbest * 1e9 / touched, cbest * 1e9 / touched, cbest / rbest);
    fflush(stdout);
    free(a);
}

int main(int argc, char **argv) {
    int cpu = (argc > 1) ? atoi(argv[1]) : 0;
    cpu_set_t set; CPU_ZERO(&set); CPU_SET(cpu, &set);
    sched_setaffinity(0, sizeof(set), &set);

    printf("n,lda,row_ns,col_ns,ratio\n");
    for (int n = 256; n <= 8192; n *= 2) {
        run(n, n);                                  /* packed: power of two */
        run(n, n + 1);                              /* padded by one element */
    }
    return 0;
}
