// Loop Fusion Example
// Two separate loops vs one fused loop to compute mean and variance.
//
// Unfused — two passes:
//   Pass 1: sum all elements -> mean = sum / n
//   Pass 2: sum squared deviations (x - mean)^2 -> variance
//   Pass 2 cannot be merged with Pass 1 naively because it requires
//   mean, which is only known after Pass 1 completes.
//
// Fused — one pass (computational formula):
//   Accumulate sum and sum-of-squares simultaneously in one loop.
//   After the loop:
//     mean     = sum / n
//     variance = (sum_sq / n) - mean * mean
//   Both statistics fall out of a single read of the data.
//
// Build:
//   g++ -O1 -o fusion_O1 loop_fusion.cpp && ./fusion_O1
//   g++ -O2 -o fusion_O2 loop_fusion.cpp && ./fusion_O2

#include <iostream>
#include <chrono>
#include <climits>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

static const int N    = 50'000'000;
static const int RUNS = 5;

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

void report(const char* title,
            const char* a_label, long long a_ms,
            const char* b_label, long long b_ms) {
    double speedup = (double)a_ms / max(1LL, b_ms);
    cout << "\n" << string(64, '-') << "\n";
    cout << title << "\n";
    cout << string(64, '-') << "\n";
    cout << "  UNFUSED  " << left  << setw(44) << a_label
         << right << setw(5) << a_ms << " ms\n";
    cout << "  FUSED    " << left  << setw(44) << b_label
         << right << setw(5) << b_ms  << " ms\n";
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
}

// ================================================================
// Unfused — two passes
// ================================================================

struct Stats { double mean; double variance; };

Stats compute_unfused(const int* data, int n) {
    // Pass 1: mean
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += data[i];
    double mean = sum / n;

    // Pass 2: variance — must wait for mean; reads all N elements again
    double m2 = 0.0;
    for (int i = 0; i < n; i++) {
        double d = data[i] - mean;
        m2 += d * d;
    }
    return {mean, m2 / n};
}

// ================================================================
// Fused — one pass (computational formula)
//
// Uses the identity:  Var(X) = E[X^2] - E[X]^2
//
// Accumulate sum and sum-of-squares in a single loop; derive both
// mean and variance after the loop with no extra data reads.
// ================================================================

Stats compute_fused(const int* data, int n) {
    double sum    = 0.0;
    double sum_sq = 0.0;
    for (int i = 0; i < n; i++) {
        double x = data[i];
        sum    += x;
        sum_sq += x * x;
    }
    double mean     = sum / n;
    double variance = (sum_sq / n) - mean * mean;
    return {mean, variance};
}

// ================================================================
// main
// ================================================================

int main() {
    vector<int> data(N);
    for (int i = 0; i < N; i++)
        data[i] = (i * 2654435761u) >> 16 & 0xFFFF;   // pseudo-random 0-65535

    const int* p = data.data();

    auto r1 = compute_unfused(p, N);
    auto r2 = compute_fused  (p, N);

    cout << fixed << setprecision(6);
    cout << "Correctness check:\n";
    cout << "  mean     unfused=" << r1.mean     << "  fused=" << r2.mean     << "\n";
    cout << "  variance unfused=" << r1.variance << "  fused=" << r2.variance << "\n";
    cout << "  delta mean:     " << abs(r1.mean     - r2.mean)     << "\n";
    cout << "  delta variance: " << abs(r1.variance - r2.variance) << "\n";

    auto t_unfused = bench([&]{
        auto r = compute_unfused(p, N);
        sink(r.mean); sink(r.variance);
    });
    auto t_fused = bench([&]{
        auto r = compute_fused(p, N);
        sink(r.mean); sink(r.variance);
    });

    report("mean + variance  (N = 50M)",
           "2 loops — 2 full reads (pass 2 needs mean)",  t_unfused,
           "1 loop  — sum + sum_sq, derive post-loop",     t_fused);

    return 0;
}
