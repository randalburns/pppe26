// Quicksort Timing: std::sort over a range of input sizes
//
// std::sort in libstdc++ and libc++ is an introsort: quicksort with
// insertion-sort for small partitions and heapsort fallback to guarantee
// O(n log n) worst-case.  This benchmark measures wall-clock sort time
// across input sizes from 10 to 10M elements.
//
// The ns/(n log2 n) column normalizes by the expected O(n log n) work.
// It should be roughly constant if sorting scales as expected.  Increases
// at larger sizes indicate cache pressure: once the array exceeds a cache
// level the random-access pattern of quicksort incurs more cache misses
// per comparison.
//
// Build:
//   g++ -O2 -o quicksort quicksort.cpp && ./quicksort

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iomanip>
#include <random>
#include <climits>
#include <string>
#include <cmath>

using namespace std;
using namespace std::chrono;

static const int RUNS = 7;   // take best of RUNS trials

// Pre-generates RUNS copies of data so only std::sort is timed.
// For small n, repeats many sorts per trial to overcome timer resolution.
double bench_sort_ns(const vector<int>& data) {
    int n = data.size();
    // Choose reps so each trial takes at least ~10 µs
    int reps = max(1, 100000 / max(n, 1));

    vector<vector<int>> copies(RUNS, data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), copies[r].begin());
            sort(copies[r].begin(), copies[r].end());
        }
        auto t1 = high_resolution_clock::now();
        long long elapsed = duration_cast<nanoseconds>(t1 - t0).count();
        best = min(best, elapsed / reps);
    }
    return (double)best;
}

static string fmt_time(double ns) {
    if (ns < 1000.0)   return to_string((int)ns)            + " ns";
    if (ns < 1e6)      return to_string((int)(ns / 1e3))    + " µs";
    if (ns < 1e9)      return to_string((int)(ns / 1e6))    + " ms";
    return               to_string((int)(ns / 1e9))          + " s";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // Powers of 2 from 2^4 to 2^20
    vector<int> sizes;
    for (int e = 4; e <= 20; e++) sizes.push_back(1 << e);

    cout << string(62, '-') << "\n";
    cout << "std::sort (introsort)  random int data\n";
    cout << string(62, '-') << "\n";
    cout << right << setw(12) << "n"
         << right << setw(12) << "time"
         << right << setw(14) << "ns/elem"
         << right << setw(16) << "ns/(n log2 n)" << "\n";
    cout << string(62, '-') << "\n";

    for (int n : sizes) {
        vector<int> data(n);
        for (int& x : data) x = dist(rng);

        double ns            = bench_sort_ns(data);
        double ns_per_elem   = ns / n;
        double nlog2n        = (double)n * log2((double)n);
        double ns_per_nlog2n = ns / nlog2n;

        cout << right << setw(12) << n
             << right << setw(12) << fmt_time(ns)
             << right << setw(13) << fixed << setprecision(1) << ns_per_elem
             << right << setw(15) << fixed << setprecision(2) << ns_per_nlog2n
             << "\n";
    }

    cout << string(62, '-') << "\n";
    cout << "ns/(n log2 n) constant → O(n log n) scaling.\n";
    cout << "Rising value at large n → cache misses on random access.\n";

    return 0;
}
