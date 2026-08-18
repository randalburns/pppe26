#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>

/*
 *  vector_add.c  --  cilk_for
 *
 *  cilk_for is the parallel loop.  Iterations must be INDEPENDENT (no
 *  iteration reads a value another writes).  The scheduler recursively
 *  halves the index range into a tree of cilk_spawn calls, so the loop
 *  runs with O(log N) span instead of O(N).
 *
 *    cilk_for (int i = 0; i < N; i++)
 *        c[i] = a[i] + b[i];       // each i touches a different slot
 *
 *  Grain size (how many iterations one leaf runs serially) is tuned with
 *    #pragma cilk grainsize = G
 *  placed immediately before the loop.  The default is usually fine.
 *
 *  Compile (OpenCilk):
 *    ~/opencilk/bin/clang -fopencilk -O3 vector_add.c -o vector_add
 *
 *  Usage: ./vector_add [N]     (default N = 100000000)
 */

int main(int argc, char *argv[])
{
    long N = argc > 1 ? atol(argv[1]) : 100000000L;

    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    double *c = malloc(N * sizeof(double));

    cilk_for (long i = 0; i < N; i++) {
        a[i] = (double)i;
        b[i] = 2.0 * i;
    }

    cilk_for (long i = 0; i < N; i++)
        c[i] = a[i] + b[i];

    printf("c[0] = %.1f   c[%ld] = %.1f\n", c[0], N - 1, c[N - 1]);

    free(a); free(b); free(c);
    return 0;
}
