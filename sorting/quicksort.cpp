// Quicksort Implementation and Timing
//
// Classic recursive quicksort using median-of-three pivot selection.
// No small-partition optimizations — pure quicksort down to partitions of 1.
//
// Pivot selection: median of first, middle, and last elements.  This avoids
// the O(n²) worst case on already-sorted input that naive first-element pivot
// selection produces.
//
// The ns/(n log2 n) column normalizes by expected O(n log n) work and should
// be roughly constant.  Compare with std_sort.cpp to see how this hand-written
// implementation compares to the library introsort.
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

static const int RUNS = 7;

// ================================================================
// Quicksort — median-of-three pivot, no small-partition cutoff
// ================================================================

static int median3(int* a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    if (a[lo] > a[mid]) swap(a[lo], a[mid]);
    if (a[lo] > a[hi])  swap(a[lo], a[hi]);
    if (a[mid] > a[hi]) swap(a[mid], a[hi]);
    // a[lo] <= a[mid] <= a[hi]; place pivot at hi-1
    swap(a[mid], a[hi - 1]);
    return a[hi - 1];
}

static void quicksort(int* a, int lo, int hi) {
    if (hi <= lo) return;
    if (hi - lo == 1) {            // two elements: one compare suffices
        if (a[lo] > a[hi]) swap(a[lo], a[hi]);
        return;
    }
    int pivot = median3(a, lo, hi);
    int i = lo, j = hi - 1;
    for (;;) {
        while (a[++i] < pivot) {}
        while (a[--j] > pivot) {}
        if (i >= j) break;
        swap(a[i], a[j]);
    }
    swap(a[i], a[hi - 1]);   // restore pivot
    quicksort(a, lo, i - 1);
    quicksort(a, i + 1, hi);
}

void quicksort(vector<int>& v) {
    if (v.size() > 1)
        quicksort(v.data(), 0, (int)v.size() - 1);
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
            quicksort(copies[r]);
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

    // Powers of 2 from 2^4 to 2^20
    vector<int> sizes;
    for (int e = 4; e <= 20; e++) sizes.push_back(1 << e);

    // Correctness check on a small array.
    {
        vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
        quicksort(v);
        bool ok = is_sorted(v.begin(), v.end());
        cout << "Correctness: " << (ok ? "PASS" : "FAIL") << "\n\n";
    }

    cout << string(62, '-') << "\n";
    cout << "quicksort (median-of-3, no small-partition cutoff)  random int data\n";
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
