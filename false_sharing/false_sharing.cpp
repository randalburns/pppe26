// Build: clang++ -std=c++17 -O2 -pthread -o false_sharing false_sharing.cpp
// Note:  -O2 is required; -O0 makes all cases slow and hides the cache effect.

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;
using namespace std::chrono;

static const int       CACHE_LINE = 64;
static const long long ITERS      = 50'000'000LL;

// Prevent dead-code elimination without forcing a visible store.
template <typename T>
static void sink(T const& val) {
    asm volatile("" : : "m"(val) : "memory");
}

template <typename Func>
long long bench(Func func, int runs = 3) {
    long long best = LLONG_MAX;
    for (int r = 0; r < runs; r++) {
        auto t0 = high_resolution_clock::now();
        func();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

void printResult(const string& title,
                 const string& slowLabel, long long slowMs,
                 const string& fastLabel, long long fastMs) {
    double speedup = (double)slowMs / max(1LL, fastMs);
    cout << "\n" << string(60, '=') << "\n" << title << "\n"
         << string(60, '-') << "\n"
         << "  " << setw(34) << left << slowLabel
         << ": " << setw(6) << right << slowMs << " ms\n"
         << "  " << setw(34) << left << fastLabel
         << ": " << setw(6) << right << fastMs << " ms\n"
         << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
}

// ============================================================
// TEST 1: FALSE SHARING
//
// Each thread increments its own element of a packed array.
// 8 longs × 8 bytes = 64 bytes = one cache line.
//
// Even though threads never touch each other's element, every
// write sets the line to Modified, invalidating all other
// cores' copies (MESI protocol). The next core must wait for
// ownership before it can write — threads serialize on the
// cache line, not on data.
// ============================================================

void test_false_sharing(int nthreads) {
    // Aligned so all counters land on ONE cache line (max contention).
    alignas(CACHE_LINE) volatile long long c[16] = {};

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&c, t]() {
            for (long long i = 0; i < ITERS; i++)
                c[t]++;
        });
    }
    for (auto& th : threads) th.join();
    sink(c[0]);
}

// ============================================================
// TEST 2: PADDED COUNTERS  (structural fix)
//
// Each counter is padded to 64 bytes so it occupies its own
// cache line.  Threads write in parallel with zero cross-core
// invalidations — each core owns its line exclusively.
// ============================================================

struct alignas(CACHE_LINE) PaddedCounter {
    volatile long long value = 0;
    char _pad[CACHE_LINE - sizeof(long long)];
};
static_assert(sizeof(PaddedCounter) == CACHE_LINE, "padding mismatch");

void test_padded(int nthreads) {
    PaddedCounter counters[16];

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&counters, t]() {
            for (long long i = 0; i < ITERS; i++)
                counters[t].value++;
        });
    }
    for (auto& th : threads) th.join();
    sink(counters[0].value);
}

// ============================================================
// TEST 3: THREAD-LOCAL ACCUMULATION  (algorithmic fix)
//
// Accumulate in a local variable (kept in a CPU register by
// -O2), then write to shared memory exactly once at the end.
// Zero cache traffic during the hot loop.
//
// With -O2 the compiler collapses "local++" × ITERS into a
// single assignment — the loop vanishes. That is correct
// behavior: the compiler can legally eliminate a loop whose
// only effect is updating a local variable.
// ============================================================

void test_local_accum(int nthreads) {
    long long results[16] = {};

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&results, t]() {
            long long local = 0;
            for (long long i = 0; i < ITERS; i++)
                local++;              // stays in a register
            results[t] = local;      // one shared write total
        });
    }
    for (auto& th : threads) th.join();

    volatile long long s = 0;
    for (int t = 0; t < nthreads; t++) s += results[t];
    sink(s);
}

int main() {
    int nthreads = max(2, min(16, (int)thread::hardware_concurrency()));

    cout << "╔══════════════════════════════════════════════════════════╗\n"
         << "║           FALSE SHARING TIMING EXAMPLES                 ║\n"
         << "╚══════════════════════════════════════════════════════════╝\n";
    cout << "Threads: "    << nthreads
         << "  |  Iterations/thread: " << ITERS
         << "  |  Cache line: " << CACHE_LINE << " B\n"
         << "sizeof(long long): " << sizeof(long long)
         << " B  →  " << (CACHE_LINE / sizeof(long long))
         << " counters per cache line\n"
         << "\nRunning benchmarks (may take 20–60 s) ...\n";

    long long t_false  = bench([&]{ test_false_sharing(nthreads); });
    long long t_padded = bench([&]{ test_padded(nthreads); });
    long long t_local  = bench([&]{ test_local_accum(nthreads); });

    printResult("TEST 1: Packed Array vs Cache-Line-Padded Struct",
                "Packed  (false sharing)",    t_false,
                "Padded  (64-byte aligned)",  t_padded);

    printResult("TEST 2: Packed Array vs Thread-Local Accumulation",
                "Packed  (false sharing)",    t_false,
                "Local   (one shared write)", t_local);

    cout << "\n" << string(60, '=') << "\n"
         << "SUMMARY — FALSE SHARING\n"
         << string(60, '=') << "\n"
         << R"(
  What is false sharing?
    Two threads write to DIFFERENT variables that happen to
    share a cache line.  The hardware treats the whole line
    as the unit of ownership — every write causes every other
    core to re-fetch the line before its next write.

  Cache line layout (64 bytes, long long = 8 bytes):

    PACKED (false sharing):
      [ t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7 ]  ← 1 line, 8 writers
      Each write invalidates 7 other cores' copies.

    PADDED (no false sharing):
      [ t0 + 56 pad ] [ t1 + 56 pad ] ...  ← 1 line per thread
      Each core owns its line exclusively.

  Fixes:

    Structural  — pad the shared struct to 64 bytes:
      struct alignas(64) Counter { long long v; char pad[56]; };

    Algorithmic — accumulate locally, write once:
      long long local = 0;
      for (...) local++;   // stays in register
      shared[t] = local;   // one write at the end

  Test 3 near 0 ms?  The compiler (-O2) eliminated the loop:
    "local++" × N collapses to "local = N" — correct because
    the loop has no observable side effect.  Cache-friendly
    code is also compiler-friendly.
)" << "\n";

    return 0;
}
