/* spin_cost.c -- what the ping-pong benchmark measures besides data movement.
 *
 * core_to_core.c reports 18 ns to hand a line between two SMT threads of one
 * core -- threads that share an L1 whose measured hit latency is 0.78 ns.  The
 * gap is not cache latency.  It is the cost of the *waiting* side noticing,
 * plus the cost of the writing side making its store visible.  This measures
 * those pieces directly, and the answer is that PAUSE costs ~62 cycles: the
 * spin loop cannot see a change sooner than that, no matter how fast the cache
 * underneath it is.
 *
 * Run it on both core types.  PAUSE is a fixed cycle count, so its cost in
 * nanoseconds scales with the clock -- 12.9 ns on a Zen 5 core at 4.85 GHz,
 * 20.1 ns on a Zen 5c core at 3.06 GHz.  That difference alone accounts for
 * most of the gap between the 20 ns and 30 ns intra-complex handoffs.
 *
 * Usage: spin_cost [cpu]        Feeds section 04 of cache_map.html.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sched.h>

static double now_s(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t);
                           return t.tv_sec + 1e-9*t.tv_nsec; }

static double measure_ghz(double sec){
    long n = 100L*1000*1000;
    for(;;){ uint64_t x=0; double t0=now_s();
        for(long i=0;i<n;i++) __asm__ volatile("addq $1, %0":"+r"(x));
        double dt=now_s()-t0; __asm__ volatile(""::"r"(x));
        if(dt>=sec) return n/dt/1e9; n*=2; }
}

static _Alignas(64) volatile long line;

int main(int argc, char **argv){
    int cpu = (argc > 1) ? atoi(argv[1]) : 0;
    cpu_set_t s; CPU_ZERO(&s); CPU_SET(cpu,&s); sched_setaffinity(0,sizeof s,&s);
    measure_ghz(1.0);
    double ghz = measure_ghz(0.1);
    printf("cpu%d, core clock: %.2f GHz\n\n", cpu, ghz);

    const long N = 20L*1000*1000;
    double t0, dt;

    /* 1. one PAUSE */
    t0=now_s();
    for(long i=0;i<N;i++) __builtin_ia32_pause();
    dt=now_s()-t0;
    printf("PAUSE alone              %7.2f ns  %6.1f cycles\n", dt*1e9/N, dt*1e9/N*ghz);

    /* 2. one poll iteration without PAUSE: load from L1 + compare + branch */
    t0=now_s();
    for(long i=0;i<N;i++){ if(line==-1) break; __asm__ volatile("":::"memory"); }
    dt=now_s()-t0;
    printf("poll, no PAUSE           %7.2f ns  %6.1f cycles\n", dt*1e9/N, dt*1e9/N*ghz);

    /* 3. the actual spin-loop body used by core_to_core.c */
    t0=now_s();
    for(long i=0;i<N;i++){ if(line==-1) break; __builtin_ia32_pause(); }
    dt=now_s()-t0;
    double poll = dt*1e9/N;
    printf("poll + PAUSE (as used)   %7.2f ns  %6.1f cycles   <- polling quantum\n",
           poll, poll*ghz);

    /* 4. store then immediately load it back, same thread: store buffer only */
    t0=now_s();
    for(long i=0;i<N;i++){ line = i; __asm__ volatile("":::"memory"); if(line<0) break; }
    dt=now_s()-t0;
    printf("store + reload (1 thread)%7.2f ns  %6.1f cycles\n", dt*1e9/N, dt*1e9/N*ghz);

    printf("\nA waiter using this loop sees a change on average half a quantum late,\n");
    printf("and at worst a full quantum: %.1f - %.1f ns before any coherence traffic.\n",
           poll/2, poll);
    return 0;
}
