/* tlb_walk.c -- measuring the TLB instead of quoting it.
 *
 * The trick is to grow the number of *pages* touched while keeping the amount
 * of *data* touched small: chase one 64-byte line per 4 KB page, in random
 * page order.  Touching N pages needs N translations but only N*64 bytes of
 * data, so translation runs out long before the data caches do, and the steps
 * in the curve are TLB capacity rather than cache capacity.
 *
 * Two details that would otherwise wreck the measurement:
 *
 *   1. The line offset inside each page is rotated (page i uses line i % 64).
 *      Using offset 0 in every page would give every access the same L1 set
 *      index -- all N accesses would fight over one 12-way set and we would be
 *      measuring conflict misses, not translation.  See why_1024_is_slow.html.
 *
 *   2. The same sweep is run on 2 MB huge pages.  The data footprint is
 *      identical, so the gap between the two curves is translation cost and
 *      nothing else.  With 2 MB pages, 512 of these "pages" fit behind a
 *      single translation.
 *
 * What comes out: the 4 KB curve is flat at an L1 hit until 96 pages and steps
 * to 2.14 ns at 112, which locates the L1 DTLB at 96 entries.  Below a 2 MB
 * span the "2 MB" run is identical to the 4 KB one, because transparent huge
 * pages cannot back a region smaller than a huge page -- that is why the two
 * curves only separate from 512 pages on.
 *
 * The L2 TLB boundary does not give a clean step here: past 1024 pages the
 * data footprint (64 KB) leaves L1d, so cache cost and translation cost grow
 * together.  The ratio column is the honest read of the translation tax.
 *
 * Output is CSV on stdout: pages,span_kb,data_kb,ns_4k,ns_2m,ratio
 *
 * Build: gcc -O2 -o tlb_walk tlb_walk.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>
#include <sys/mman.h>

#define PAGE 4096
#define LINE 64

static double now_s(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t);
                           return t.tv_sec + 1e-9*t.tv_nsec; }

static double measure_ghz(double sec){
    long n = 100L*1000*1000;
    for(;;){ uint64_t x=0; double t0=now_s();
        for(long i=0;i<n;i++) __asm__ volatile("addq $1, %0":"+r"(x));
        double dt=now_s()-t0; __asm__ volatile(""::"r"(x));
        if(dt>=sec) return n/dt/1e9;
        n*=2;
    }
}

static uint64_t rs = 88172645463325252ULL;
static uint64_t rng(void){ rs^=rs<<13; rs^=rs>>7; rs^=rs<<17; return rs; }

int main(int argc, char **argv){
    int cpu = (argc>1)?atoi(argv[1]):0;
    cpu_set_t s; CPU_ZERO(&s); CPU_SET(cpu,&s); sched_setaffinity(0,sizeof s,&s);
    measure_ghz(1.0);
    fprintf(stderr,"cpu%d warmed, clock %.2f GHz\n", cpu, measure_ghz(0.1));

    printf("pages,span_kb,data_kb,ns_4k,ns_2m,ratio\n");

    long sizes[] = {16,24,32,48,64,80,96,112,128,160,192,256,384,512,768,1024,
                    1536,2048,3072,4096,6144,8192,12288,16384,24576,32768,49152,65536};
    for(unsigned k=0;k<sizeof sizes/sizeof sizes[0];k++){
        long np = sizes[k];
        size_t bytes = (size_t)np*PAGE;
        double ns[2];

        for(int mode=0;mode<2;mode++){
            char *buf;
            if(posix_memalign((void**)&buf, 2UL<<20, bytes)){ perror("alloc"); return 1; }
            madvise(buf, bytes, mode ? MADV_HUGEPAGE : MADV_NOHUGEPAGE);
            memset(buf,0,bytes);

            long *order = malloc(np*sizeof(long));
            for(long i=0;i<np;i++) order[i]=i;
            for(long i=np-1;i>0;i--){ long j=rng()%(i+1); long t=order[i]; order[i]=order[j]; order[j]=t; }
            for(long i=0;i<np;i++){
                long p=order[i], q=order[(i+1)%np];
                *(void**)(buf+p*PAGE+(p%64)*LINE) = buf+q*PAGE+(q%64)*LINE;
            }
            void **ptr = (void**)(buf+order[0]*PAGE+(order[0]%64)*LINE);
            free(order);

            long acc = 5L*1000*1000; if(acc < 8*np) acc = 8*np;
            for(long i=0;i<np;i++) ptr=(void**)*ptr;          /* warm */

            double best=1e30;
            for(int r=0;r<3;r++){
                double t0=now_s();
                for(long i=0;i<acc;i++) ptr=(void**)*ptr;
                double v=(now_s()-t0)*1e9/acc;
                if(v<best) best=v;
            }
            __asm__ volatile(""::"r"(ptr));
            ns[mode]=best;
            free(buf);
        }
        printf("%ld,%zu,%ld,%.3f,%.3f,%.2f\n",
               np, ((size_t)np*PAGE)>>10, np*LINE>>10, ns[0], ns[1], ns[0]/ns[1]);
        fflush(stdout);
    }
    return 0;
}
