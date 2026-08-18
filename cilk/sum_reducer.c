#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>

/*
 *  sum_reducer.c  --  cilk_reducer (hyperobjects)
 *
 *  A cilk_for cannot safely do  sum += a[i]  with a plain variable: many
 *  workers would race on the same location.  A REDUCER fixes this without
 *  a lock.  Each worker gets its own private "view" of the variable; when
 *  a steal joins two strands back together the runtime calls your REDUCE
 *  function to combine them.  The result is deterministic and lock-free.
 *
 *  A reducer needs two callbacks:
 *    identity(v)     initialise a fresh view to the identity element (0 for +)
 *    reduce(l, r)    fold the right view into the left:  *l = *l (op) *r
 *
 *  Declare one by qualifying a variable's type with cilk_reducer(id, reduce).
 *  Inside the loop use it like a normal variable; read the final value after.
 *
 *  Compile (OpenCilk):
 *    ~/opencilk/bin/clang -fopencilk -O3 sum_reducer.c -o sum_reducer
 *
 *  Usage: ./sum_reducer [N]    (default N = 100000000)
 */

void zero_double(void *v)          { *(double *)v = 0.0; }
void add_double (void *l, void *r) { *(double *)l += *(double *)r; }

int main(int argc, char *argv[])
{
    long N = argc > 1 ? atol(argv[1]) : 100000000L;

    double *a = malloc(N * sizeof(double));
    cilk_for (long i = 0; i < N; i++)
        a[i] = 1.0 / (i + 1);        /* harmonic series terms */

    double cilk_reducer(zero_double, add_double) sum = 0.0;

    cilk_for (long i = 0; i < N; i++)
        sum += a[i];                 /* safe: each worker updates its own view */

    printf("sum of 1/i for i=1..%ld = %.10f\n", N, (double)sum);

    free(a);
    return 0;
}
