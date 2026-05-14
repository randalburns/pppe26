// Loop Unrolling Example
// Demonstrates manual unrolling of a data-driven loop over an array of
// unknown length.  The key challenge is handling the "tail" — the leftover
// elements when n is not a multiple of the unroll factor.
//
// Three strategies are shown:
//   1. Scalar baseline
//   2. 4x unroll with a scalar cleanup loop for the tail
//   3. Duff's Device — a switch/while hybrid that jumps into the middle of
//      the unrolled body to handle the tail without a separate loop
//
// Build:
//   g++ -O1 -o unroll_O1 loop_unrolling.cpp && ./unroll_O1
//   g++ -O2 -o unroll_O2 loop_unrolling.cpp && ./unroll_O2
//
// -O1 enables register allocation and basic optimisations but not
// auto-vectorisation, so the speedup from manual unrolling is visible.
// -O2 enables auto-vectorisation; the compiler may match or beat the
// hand-unrolled code.

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

static const int N       = 100'000'000;
static const int RUNS    = 5;

// Prevent the compiler from optimizing away a result.
template <typename T>
static void sink(T const& val) { asm volatile("" : : "m"(val) : "memory"); }

template <typename Func>
long long bench(Func f) {
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        asm volatile("" ::: "memory");
        auto t0 = high_resolution_clock::now();
        auto v  = f();
        sink(v);
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

void report(const char* title,
            const char* a_label, long long a_ms,
            const char* b_label, long long b_ms) {
    double speedup = (double)a_ms / max(1LL, b_ms);
    cout << "\n" << string(64, '-') << "\n";
    cout << title << "\n";
    cout << string(64, '-') << "\n";
    cout << "  BASELINE  " << left  << setw(40) << a_label
         << right << setw(5) << a_ms << " ms\n";
    cout << "  UNROLLED  " << left  << setw(40) << b_label
         << right << setw(5) << b_ms  << " ms\n";
    cout << "  Speedup:  " << fixed << setprecision(2) << speedup << "x\n";
}

// ================================================================
// 1. Scalar baseline — one add per iteration
// ================================================================

long long sum_scalar(const int* data, int n) {
    long long s = 0;
    for (int i = 0; i < n; i++)
        s += data[i];
    return s;
}

// ================================================================
// 2. 4x unroll with a scalar cleanup loop
//
// Main loop processes 4 elements at once, reducing loop-control
// overhead (branch, increment) by 4x.  A trailing scalar loop
// handles the remaining 0–3 elements.
// ================================================================

long long sum_unrolled4(const int* data, int n) {
    long long s = 0;
    int i = 0;

    // Process 4 elements per iteration.
    int n4 = n - (n % 4);   // largest multiple of 4 <= n
    for (; i < n4; i += 4) {
        s += data[i];
        s += data[i + 1];
        s += data[i + 2];
        s += data[i + 3];
    }

    // Cleanup: handle tail elements (0, 1, 2, or 3 remaining).
    for (; i < n; i++)
        s += data[i];

    return s;
}

// ================================================================
// 3. Duff's Device — unrolled body with a switch-driven entry point
//
// The switch jumps into the *middle* of the unrolled loop body so
// that the first "iteration" processes exactly (n % 4) elements,
// aligning subsequent iterations to a 4-element boundary.
// No separate cleanup loop is required.
//
// Named after Tom Duff (Bell Labs, 1983) who used it to accelerate
// a byte-copy routine on a PDP-11.
// ================================================================

long long sum_duffs(const int* data, int n) {
    if (n == 0) return 0;

    long long s = 0;
    int i = 0;
    int count = (n + 3) / 4;   // number of times the unrolled body fires

    // switch selects the fall-through entry point for the first pass.
    switch (n % 4) {
        case 0: do { s += data[i++];  // falls through
        case 3:      s += data[i++];  // falls through
        case 2:      s += data[i++];  // falls through
        case 1:      s += data[i++];
                } while (--count > 0);
    }

    return s;
}

// ================================================================
// main — build the array and run benchmarks
// ================================================================

int main() {
    // Use a non-trivial array so the compiler cannot constant-fold results.
    vector<int> data(N);
    for (int i = 0; i < N; i++)
        data[i] = (i % 7) + 1;   // values 1–7, non-power-of-2 length pattern

    // Test with lengths that are *not* multiples of 4 to exercise the tail.
    // n = N - 3  =>  remainder 1
    // n = N      =>  remainder 0  (no tail work needed)
    const int* p = data.data();
    int n_odd = N - 3;   // exercises tail in both unrolled and Duff's versions

    cout << "Array length (odd test): " << n_odd
         << "  (tail = " << (n_odd % 4) << " element(s))\n";
    cout << "Array length (even test): " << N
         << "  (tail = " << (N % 4) << " element(s))\n";

    // Verify all three produce the same answer.
    long long r1 = sum_scalar  (p, n_odd);
    long long r2 = sum_unrolled4(p, n_odd);
    long long r3 = sum_duffs   (p, n_odd);
    cout << "\nCorrectness check (n_odd):\n";
    cout << "  scalar=" << r1 << "  unrolled4=" << r2 << "  duffs=" << r3;
    cout << (r1 == r2 && r2 == r3 ? "  PASS" : "  FAIL") << "\n";

    // ---- Benchmark: odd-length array --------------------------------
    auto t_scalar   = bench([&]{ return sum_scalar  (p, n_odd); });
    auto t_unrolled = bench([&]{ return sum_unrolled4(p, n_odd); });
    auto t_duffs    = bench([&]{ return sum_duffs   (p, n_odd); });

    report("Odd-length array (n % 4 == 1) — loop-overhead reduction",
           "scalar loop",        t_scalar,
           "4x unroll + cleanup", t_unrolled);
    report("Odd-length array (n % 4 == 1) — Duff's Device vs scalar",
           "scalar loop",        t_scalar,
           "Duff's Device",      t_duffs);

    // ---- Benchmark: even-length array (no tail) ---------------------
    auto t_scalar_e   = bench([&]{ return sum_scalar  (p, N); });
    auto t_unrolled_e = bench([&]{ return sum_unrolled4(p, N); });
    auto t_duffs_e    = bench([&]{ return sum_duffs   (p, N); });

    report("Even-length array (n % 4 == 0) — loop-overhead reduction",
           "scalar loop",        t_scalar_e,
           "4x unroll + cleanup", t_unrolled_e);
    report("Even-length array (n % 4 == 0) — Duff's Device vs scalar",
           "scalar loop",        t_scalar_e,
           "Duff's Device",      t_duffs_e);

    cout << "\nNote: at -O2 the compiler auto-vectorises the scalar loop,\n"
            "      often matching or beating manual unrolling.\n"
            "      Build with -O0 to see raw unrolling benefits.\n";
    return 0;
}
