// Build: clang++ -std=c++17 -stdlib=libc++ -O2 -pthread -o false_sharing false_sharing.cpp
// Or:    make && make run          (builds -O0/-O1/-O2/-O3 and runs all four)
//
// Note: -O0 makes every case slow and compresses the ratios; -O2 is where the
//       cache-line effect stands out.  The Makefile builds all four levels so
//       you can see how the speedups move with optimization.
//
// Threads are PINNED to specific logical CPUs where the platform supports it
// (Linux, via pthread_setaffinity_np).  Without pinning, the scheduler
// migrates threads between cores mid-run and the timings swing by 2-3x from
// run to run; on platforms without that API (e.g. macOS) the threads simply
// run unpinned and the results are noisier but still directionally correct.

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cstdlib>

#if defined(__linux__)
  #include <pthread.h>
  #include <sched.h>
#endif

using namespace std;
using namespace std::chrono;

static const int       CACHE_LINE = 64;
static long long       ITERS      = 20'000'000LL;

// 8 counters of 8 bytes fill exactly one 64-byte line, so the packed test uses
// 8 threads by default: one writer per slot, all slots on ONE cache line.
static const int COUNTERS_PER_LINE = CACHE_LINE / sizeof(long long);

// Prevent dead-code elimination without forcing a visible store.
template <typename T>
static void sink(T const& val) {
    asm volatile("" : : "m"(val) : "memory");
}

// Pin the calling thread to one logical CPU.  Returns false if unsupported.
static bool pinSelf(int cpu) {
#if defined(__linux__)
    if (cpu < 0) return false;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    return pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == 0;
#else
    (void)cpu; return false;
#endif
}

// ============================================================
// TIMING
// ============================================================

template <typename Func>
double bench(Func func, int runs = 5) {
    double best = 1e18;
    for (int r = 0; r < runs; r++) {
        auto t0 = high_resolution_clock::now();
        func();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration<double, milli>(t1 - t0).count());
    }
    return best;
}

static void printResult(const string& title,
                        const string& slowLabel, double slowMs,
                        const string& fastLabel, double fastMs) {
    cout << "\n" << string(64, '=') << "\n" << title << "\n"
         << string(64, '-') << "\n"
         << "  " << setw(36) << left << slowLabel
         << ": " << setw(9) << right << fixed << setprecision(1) << slowMs << " ms\n"
         << "  " << setw(36) << left << fastLabel
         << ": " << setw(9) << right << fixed << setprecision(1) << fastMs << " ms\n"
         << "  Speedup: " << fixed << setprecision(2)
         << (slowMs / max(0.001, fastMs)) << "x\n";
}

// ============================================================
// WORKLOAD A: FALSE SHARING  (the problem)
//
// Each thread increments its own element of a packed array.
// With 8 threads: 8 longs x 8 bytes = 64 bytes = ONE cache line.
//
// Threads never touch each other's element, but every write sets
// the line to Modified and invalidates all other cores' copies
// (MESI).  The next core must acquire ownership before it can
// write, so the threads serialize on the line, not on the data.
// ============================================================

static void test_false_sharing(const vector<int>& cpuSet) {
    alignas(CACHE_LINE) volatile long long c[COUNTERS_PER_LINE] = {};
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&c, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            for (long long i = 0; i < ITERS; i++)
                c[t % COUNTERS_PER_LINE]++;
        });
    }
    for (auto& th : threads) th.join();
    sink(c[0]);
}

// ============================================================
// WORKLOAD B: PADDED COUNTERS  (structural fix)
//
// Each counter is padded to 64 bytes so it owns its own cache
// line.  Threads write in parallel with no cross-core
// invalidations.
// ============================================================

struct alignas(CACHE_LINE) PaddedCounter {
    volatile long long value = 0;
    char _pad[CACHE_LINE - sizeof(long long)];
};
static_assert(sizeof(PaddedCounter) == CACHE_LINE, "padding mismatch");

static void test_padded(const vector<int>& cpuSet) {
    vector<PaddedCounter> counters(cpuSet.size());
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&counters, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            for (long long i = 0; i < ITERS; i++)
                counters[t].value++;
        });
    }
    for (auto& th : threads) th.join();
    sink(counters[0].value);
}

// ============================================================
// WORKLOAD C: THREAD-LOCAL ACCUMULATION  (algorithmic fix)
//
// Accumulate in a local (register) variable, then write to
// shared memory exactly once.  Zero cache traffic in the loop.
//
// At -O1 and above the compiler collapses "local++" x ITERS into
// a single assignment and the loop disappears -> ~0 ms.  That is
// correct: the loop has no observable side effect.  At -O0 the
// loop really runs, which is why the -O sweep is interesting.
// ============================================================

static void test_local_accum(const vector<int>& cpuSet) {
    vector<long long> results(cpuSet.size(), 0);
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&results, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            long long local = 0;
            for (long long i = 0; i < ITERS; i++)
                local++;                 // stays in a register
            results[t] = local;          // one shared write total
        });
    }
    for (auto& th : threads) th.join();

    volatile long long s = 0;
    for (int t = 0; t < nthreads; t++) s += results[t];
    sink(s);
}

int main(int argc, char** argv) {
    if (argc > 1) ITERS = atoll(argv[1]);

    // One thread per logical core, up to 8 (one full cache line of counters).
    // Pinned sequentially to CPUs 0..n-1 where pinning is supported (Linux);
    // -1 means "do not pin" and is what pinSelf gets on unsupported platforms.
    int n = max(2, min(COUNTERS_PER_LINE, (int)thread::hardware_concurrency()));
    vector<int> cpuSet;
#if defined(__linux__)
    for (int i = 0; i < n; i++) cpuSet.push_back(i);
#else
    for (int i = 0; i < n; i++) cpuSet.push_back(-1);
#endif
    int nthreads = (int)cpuSet.size();

    cout << "+----------------------------------------------------------------+\n"
         << "|              FALSE SHARING TIMING EXAMPLES                     |\n"
         << "+----------------------------------------------------------------+\n";
    cout << "Threads: " << nthreads
         << "  |  Iterations/thread: " << ITERS
         << "  |  Cache line: " << CACHE_LINE << " B\n"
         << "sizeof(long long): " << sizeof(long long) << " B  ->  "
         << COUNTERS_PER_LINE << " counters per cache line\n";
#if defined(__linux__)
    cout << "Pinned to CPUs 0.." << (nthreads - 1) << "\n";
#else
    cout << "Pinning unsupported on this platform; threads run unpinned.\n";
#endif
    cout << "\nRunning benchmarks ...\n";

    double t_false  = bench([&]{ test_false_sharing(cpuSet); });
    double t_padded = bench([&]{ test_padded(cpuSet); });
    double t_local  = bench([&]{ test_local_accum(cpuSet); });

    printResult("TEST 1: Packed Array vs Cache-Line-Padded Struct",
                "Packed  (false sharing)",    t_false,
                "Padded  (64-byte aligned)",  t_padded);

    printResult("TEST 2: Packed Array vs Thread-Local Accumulation",
                "Packed  (false sharing)",    t_false,
                "Local   (one shared write)", t_local);


    cout << "\n" << string(64, '=') << "\n"
         << "SUMMARY - FALSE SHARING\n"
         << string(64, '=') << "\n"
         << R"(
  What is false sharing?
    Two threads write to DIFFERENT variables that happen to
    share a cache line.  The hardware treats the whole line
    as the unit of ownership, so every write forces every
    other core to re-acquire the line before its next write.

  Cache line layout (64 bytes, long long = 8 bytes):

    PACKED (false sharing):
      [ t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7 ]  <- 1 line, 8 writers
      Each write invalidates the other 7 cores' copies.

    PADDED (no false sharing):
      [ t0 + 56 pad ] [ t1 + 56 pad ] ...  <- 1 line per thread
      Each core owns its line exclusively.

  Fixes:

    Structural  - pad the shared struct to 64 bytes:
      struct alignas(64) Counter { long long v; char pad[56]; };

    Algorithmic - accumulate locally, write once:
      long long local = 0;
      for (...) local++;   // stays in register
      shared[t] = local;   // one write at the end

  Local-accumulation test near 0 ms?  The compiler (-O1+)
  eliminated the loop: "local++" x N collapses to "local = N",
  which is legal because the loop has no observable effect.
  Cache-friendly code is also compiler-friendly.
)" << "\n";

    return 0;
}
