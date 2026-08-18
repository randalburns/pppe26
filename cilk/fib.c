#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>

/*
 *  fib.c  --  cilk_spawn and cilk_sync
 *
 *  The two fundamental Cilk primitives:
 *
 *    cilk_spawn f()   The caller MAY continue past this line in parallel
 *                     with the call to f().  It does not have to -- the
 *                     work-stealing scheduler decides at run time whether
 *                     an idle worker steals the continuation.
 *
 *    cilk_sync        Wait here until every cilk_spawn made in THIS function
 *                     has completed.  There is an implicit cilk_sync at the
 *                     end of every function that spawns.
 *
 *  Fibonacci is the textbook example: not efficient (exponential work), but
 *  the two recursive calls are independent, so it shows the primitives clearly.
 *
 *  Compile (OpenCilk):
 *    ~/opencilk/bin/clang -fopencilk -O3 fib.c -o fib
 *
 *  Usage: ./fib [n]        (default n = 40)
 */

long fib(int n)
{
    if (n < 2)
        return n;

    long a, b;

    a = cilk_spawn fib(n - 1);   /* may run in parallel ...            */
    b =            fib(n - 2);    /* ... with this continuation         */

    cilk_sync;                    /* both must finish before we add     */

    return a + b;
}

int main(int argc, char *argv[])
{
    int n = argc > 1 ? atoi(argv[1]) : 40;

    printf("fib(%d) = %ld\n", n, fib(n));
    return 0;
}
