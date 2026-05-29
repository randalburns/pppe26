// Speculative Execution Example: Branch Prediction and Pipeline Flush Cost
//
// Out-of-order CPUs speculatively execute instructions past a branch before
// knowing the branch outcome.  The branch predictor guesses the direction,
// the pipeline fetches and executes along that path, and:
//
//   Correct prediction:  speculative work commits — full pipeline utilization.
//   Wrong prediction:    pipeline is flushed back to the branch; all speculative
//                        work is discarded.  Penalty ≈ 15–20 cycles on Apple
//                        M-series (the depth of the out-of-order window).
//
// This example runs the same conditional sum over the same data values in two
// different orderings:
//
//   Shuffled data — random branch outcomes:
//     Values are uniformly distributed in [0, 255] with threshold = 128.
//     ~50% of branches are taken.  The predictor has no pattern to exploit
//     and mispredicts ≈ 50% of the time.
//
//     Expected overhead:
//       N × 0.5 × penalty / clock = 32M × 0.5 × 15 / 3×10⁹ ≈ 80 ms
//
//   Sorted data — predictable branch:
//     The same values, sorted ascending.  All elements below threshold appear
//     first; all elements above appear last.  The predictor learns "not taken"
//     for the first half, "taken" for the second, and mispredicts exactly once
//     at the transition between the two halves.  Speculative execution is
//     correct for N–1 of the N branches.
//
// A branchless version is also shown: it replaces the conditional with an
// arithmetic expression (v * (v >= THRESHOLD)) that the compiler lowers to a
// compare + multiply, eliminating the branch entirely.  No speculation occurs;
// no misprediction penalty is possible.  On random data, branchless removes
// the 80 ms penalty; on sorted data it trades an extra multiply for the
// negligible misprediction cost — often a wash.
//
// Compiler note:
//   Apple Clang converts simple if-statements to CSEL (conditional select) even
//   at -O1, eliminating the branch — and with it the misprediction penalty.
//   GCC with -fno-if-conversion keeps a real branch instruction, which is
//   required to observe the speculative execution effect.
//
// Build:
//   g++-15 -O1 -fno-if-conversion -o spec_O1 speculative_execution.cpp && ./spec_O1

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;
using namespace std::chrono;

static const int N         = 32 * 1024 * 1024;  // 32M ints = 128 MB
static const int THRESHOLD = 128;                 // ~50% of [0,255] values exceed this
static const int RUNS      = 5;

// Misprediction penalty on Apple M-series ≈ 15 cycles.
// x86-64 (Skylake): ≈ 15–20 cycles.
static const int MISPREDICT_PENALTY = 15;

// CPU frequency for CPI calculation (Apple M4 performance core).
// Adjust for your chip: M1/M2 ≈ 3.2 GHz, M3 ≈ 3.7 GHz, M4 ≈ 4.0 GHz.
static const double CPU_GHZ = 4.0;

// Instructions per element in the branchy inner loop (from GCC -O1 assembly):
//   not-taken path (data < threshold): ldr, cmp, ble, add(ptr), cmp(ptr), beq = 6 insns
//   taken path     (data >= threshold): same 6 + add(sum) + b(loop) = 8 insns
//   average at 50% taken: 7 insns/element
static const double INSNS_PER_ELEM = 7.0;

template <typename T>
static void sink(T const& val) { asm volatile("" : : "m"(val) : "memory"); }

template <typename Func>
long long bench(Func f) {
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        asm volatile("" ::: "memory");
        auto t0 = high_resolution_clock::now();
        f();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

// ================================================================
// Branchy conditional sum
//
// Emits a compare + conditional branch.  Performance depends entirely
// on whether the branch predictor can anticipate the outcome.
// ================================================================

int64_t sum_conditional(const int* data, int n) {
    int64_t sum = 0;
    for (int i = 0; i < n; i++)
        if (data[i] >= THRESHOLD)
            sum += data[i];
    return sum;
}

// ================================================================
// Branchless conditional sum
//
// v * (v >= THRESHOLD): the comparison yields 0 or 1, multiplying by
// v gives 0 or v.  The compiler lowers this to a compare + multiply
// (or CSEL/CMOV) with no branch instruction — no speculation, no
// misprediction penalty regardless of data order.
// ================================================================

int64_t sum_branchless(const int* data, int n) {
    int64_t sum = 0;
    for (int i = 0; i < n; i++) {
        int v = data[i];
        sum += (int64_t)v * (v >= THRESHOLD);
    }
    return sum;
}

// ================================================================
// main
// ================================================================

int main() {
    // Build shuffled dataset, then sort a copy.
    vector<int> shuffled(N), sorted(N);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < N; i++)
        shuffled[i] = dist(rng);
    sorted = shuffled;
    std::sort(sorted.begin(), sorted.end());

    // Correctness check: both orderings must produce the same sum.
    int64_t r_shuf = sum_conditional(shuffled.data(), N);
    int64_t r_sort = sum_conditional(sorted.data(),   N);
    int64_t r_brnl = sum_branchless (shuffled.data(), N);
    cout << "Correctness: shuffled=" << r_shuf
         << "  sorted=" << r_sort
         << "  branchless=" << r_brnl
         << "  " << (r_shuf == r_sort && r_shuf == r_brnl ? "PASS" : "FAIL") << "\n";

    // Summary of expected overhead.
    double above_frac = 0.5;   // uniform [0,255], threshold=128 → ~50%
    double mispredict_rate = 2.0 * above_frac * (1.0 - above_frac);  // 2pq for random
    double predicted_overhead_ms =
        (double)N * mispredict_rate * MISPREDICT_PENALTY / 3.0e6;  // at 3 GHz
    cout << fixed << setprecision(0);
    cout << "\nData:  " << N/1000000 << "M ints in [0,255]  threshold=" << THRESHOLD
         << "  ~" << (int)(above_frac*100) << "% above\n";
    cout << "Expected misprediction overhead (shuffled): ~"
         << predicted_overhead_ms << " ms  "
         << "(" << (int)(mispredict_rate*100) << "% mispredict × "
         << MISPREDICT_PENALTY << " cycles × 32M branches at 3 GHz)\n";

    auto t_shuf = bench([&]{ sink(sum_conditional(shuffled.data(), N)); });
    auto t_sort = bench([&]{ sink(sum_conditional(sorted.data(),   N)); });
    auto t_brnl = bench([&]{ sink(sum_branchless (shuffled.data(), N)); });

    // CPI calculation: cycles = time_ms * CPU_GHZ * 1e6
    //                  cycles/elem = cycles / N
    //                  CPI = cycles / (N * INSNS_PER_ELEM)
    auto cpi = [&](long long ms) -> double {
        double cycles = (double)ms * CPU_GHZ * 1e6;
        return cycles / ((double)N * INSNS_PER_ELEM);
    };
    auto cpe = [&](long long ms) -> double {
        return (double)ms * CPU_GHZ * 1e6 / (double)N;
    };

    cout << "\n" << string(80, '-') << "\n";
    cout << "Conditional sum  N=32M ints  128 MB   threshold=128"
         << "   CPU=" << CPU_GHZ << " GHz\n";
    cout << string(80, '-') << "\n";
    cout << left  << setw(36) << "version"
         << right << setw(8)  << "time"
         << right << setw(12) << "cycles/elem"
         << right << setw(7)  << "CPI"
         << right << setw(10) << "speedup" << "\n";
    cout << string(80, '-') << "\n";

    auto row = [&](const char* label, long long ms) {
        cout << left  << setw(36) << label
             << right << setw(7)  << ms << " ms"
             << right << setw(11) << fixed << setprecision(2) << cpe(ms)
             << right << setw(7)  << fixed << setprecision(2) << cpi(ms)
             << right << setw(9)  << fixed << setprecision(2)
             << (double)t_shuf / max(1LL, ms) << "x\n";
    };

    row("branchy  + shuffled  (50% mispredict)", t_shuf);
    row("branchy  + sorted    (1 mispredict)",   t_sort);
    row("branchless + shuffled (no branch)",     t_brnl);

    double cpi_slow = cpi(t_shuf);
    double cpi_fast = cpi(t_sort);
    cout << "\n(CPI from " << (int)INSNS_PER_ELEM << " insns/elem avg; "
         << "speedup ≈ CPI_slow / CPI_fast = "
         << fixed << setprecision(2) << cpi_slow << " / "
         << fixed << setprecision(2) << cpi_fast << " = "
         << fixed << setprecision(1) << cpi_slow / cpi_fast << "x"
         << "  [measured: "
         << fixed << setprecision(1) << (double)t_shuf / max(1LL, t_sort) << "x])\n";

    return 0;
}
