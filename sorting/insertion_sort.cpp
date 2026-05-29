// Insertion Sort Implementation and Timing
//
// Insertion sort builds a sorted prefix one element at a time.  For each
// element at position i, it scans left through the sorted prefix, shifting
// elements right until it finds the correct insertion point.
//
// Time complexity: O(n²) average and worst case, O(n) best case (sorted input).
// The ns/(n log2 n) column will rise with n, reflecting the O(n²) growth.
// Compare with quicksort.cpp and std_sort.cpp to see the crossover point
// where O(n log n) algorithms pull ahead.
//
// Build:
//   g++ -O2 -o insertion_sort insertion_sort.cpp && ./insertion_sort

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

static const int RUNS = 7;

// ================================================================
// Insertion sort
// ================================================================

void insertion_sort(vector<int>& a) {
    int n = a.size();
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

// ================================================================
// Benchmarking helpers (same pattern as std_sort.cpp)
// ================================================================

double bench_sort_ns(const vector<int>& data) {
    int n = data.size();
    int reps = max(1, 100000 / max(n, 1));

    vector<vector<int>> copies(RUNS, data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), copies[r].begin());
            insertion_sort(copies[r]);
        }
        auto t1 = high_resolution_clock::now();
        long long elapsed = duration_cast<nanoseconds>(t1 - t0).count();
        best = min(best, elapsed / reps);
    }
    return (double)best;
}

static string fmt_time(double ns) {
    if (ns < 1000.0) return to_string((int)ns)         + " ns";
    if (ns < 1e6)    return to_string((int)(ns / 1e3))  + " µs";
    if (ns < 1e9)    return to_string((int)(ns / 1e6))  + " ms";
    return             to_string((int)(ns / 1e9))        + " s";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // Powers of 2 from 2^4 to 2^16 — capped at 2^16 because O(n²) at
    // 2^20 would take ~7 minutes (ns/n² ≈ 0.06 → 0.06 × 2^40 ns ≈ 66000 s).
    vector<int> sizes;
    for (int e = 4; e <= 16; e++) sizes.push_back(1 << e);

    // Correctness check.
    {
        vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
        insertion_sort(v);
        cout << "Correctness: " << (is_sorted(v.begin(), v.end()) ? "PASS" : "FAIL") << "\n\n";
    }

    cout << string(74, '-') << "\n";
    cout << "insertion sort  random int data\n";
    cout << string(74, '-') << "\n";
    cout << right << setw(12) << "n"
         << right << setw(12) << "time"
         << right << setw(14) << "ns/elem"
         << right << setw(16) << "ns/(n log2 n)"
         << right << setw(12) << "ns/n^2" << "\n";
    cout << string(74, '-') << "\n";

    for (int n : sizes) {
        vector<int> data(n);
        for (int& x : data) x = dist(rng);

        double ns            = bench_sort_ns(data);
        double ns_per_elem   = ns / n;
        double nlog2n        = (double)n * log2((double)n);
        double ns_per_nlog2n = ns / nlog2n;
        double ns_per_n2     = ns / ((double)n * n);

        cout << right << setw(12) << n
             << right << setw(12) << fmt_time(ns)
             << right << setw(13) << fixed << setprecision(1) << ns_per_elem
             << right << setw(15) << fixed << setprecision(2) << ns_per_nlog2n
             << right << setw(11) << fixed << setprecision(3) << ns_per_n2
             << "\n";
    }

    cout << string(74, '-') << "\n";
    cout << "ns/(n log2 n) rising → O(n²) growth.\n";
    cout << "ns/n^2 constant → confirms O(n²) complexity.\n";

    return 0;
}
