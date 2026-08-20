/* set_conflict.c -- is the power-of-two penalty really about L1 set indexing?
 *
 * L1d here is 48 KB, 12-way, 64 sets, 64-byte lines.  A line's set is
 * (address / 64) % 64.  Walking down a column of a row-major matrix steps by
 * one row = lda*8 bytes = (lda/8) lines, so the set index advances by
 * ((lda/8) % 64) per step.  When that advance is 0, every element of the
 * column lands in ONE set, and a 1024-element column fights over 12 ways.
 *
 * Prediction: the traversal time should track the number of distinct sets the
 * column reaches -- not the row length, not the array size.  So pad the
 * leading dimension to hit chosen set counts and see if the prediction holds.
 * It does, monotonically, and it collapses once 16 sets are reachable: 16 ways
 * x 12 lines is enough to hold the column's working set.
 *
 * Note 1152, which is not a power of two and is still slow.  The rule is not
 * "avoid powers of two" -- it is how many factors of two are in the row length
 * measured in cache lines.
 *
 * Feeds why_1024_is_slow.html.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sched.h>

#define N 1024

static double now_s(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t);
                           return t.tv_sec + 1e-9*t.tv_nsec; }

static long gcd(long a,long b){ while(b){ long t=a%b; a=b; b=t; } return a; }

int main(void){
    cpu_set_t s; CPU_ZERO(&s); CPU_SET(0,&s); sched_setaffinity(0,sizeof s,&s);

    int ldas[] = {1024, 1152, 1088, 1056, 1040, 1032, 1025};
    printf("%6s %10s %8s %8s %10s   %s\n",
           "lda","row bytes","lines","set adv","sets used","ns/elem");
    for(unsigned k=0;k<sizeof ldas/sizeof ldas[0];k++){
        long lda = ldas[k];
        long row_bytes = lda*8;
        double *a = malloc((size_t)N*lda*sizeof(double));
        memset(a,0,(size_t)N*lda*sizeof(double));

        double best=1e30;
        for(int t=0;t<3;t++){
            double t0=now_s();
            for(int r=0;r<3;r++)
                for(int y=0;y<N;y++)
                    for(int x=0;x<N;x++)
                        a[(size_t)x*lda+y] = 1.0;
            double dt=(now_s()-t0)/3;
            if(dt<best) best=dt;
        }

        char sets[24];
        if(row_bytes % 64){
            snprintf(sets,sizeof sets,"64 (drifts)");
        } else {
            long lines = row_bytes/64, adv = lines % 64;
            if(adv==0) snprintf(sets,sizeof sets,"1");
            else       snprintf(sets,sizeof sets,"%ld", 64/gcd(adv,64));
        }
        printf("%6ld %10ld %8.2f %8s %10s   %7.3f\n",
               lda, row_bytes, row_bytes/64.0,
               row_bytes%64 ? "frac" : "int", sets, best*1e9/((double)N*N));
        free(a);
    }
    return 0;
}
