// Quicksort vs std::sort — large-array comparison
//
// Runs both algorithms on identical random data across sizes 2^10 to 2^20
// (1K to 1M elements).  The same seed is used so each row is a fair
// apples-to-apples comparison.
//
// std::sort is an introsort: quicksort + insertion-sort for small partitions
// + heapsort fallback for worst-case depth.  The "ratio" column shows
// std::sort time / quicksort time; < 1.0 means std::sort is faster.
//
// Build:
//   g++ -O2 -o sort_compare_large sort_compare_large.cpp && ./sort_compare_large

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
    swap(a[mid], a[hi - 1]);
    return a[hi - 1];
}

static void qs(int* a, int lo, int hi) {
    if (hi <= lo) return;
    if (hi - lo == 1) {
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
    swap(a[i], a[hi - 1]);
    qs(a, lo, i - 1);
    qs(a, i + 1, hi);
}

static void quicksort(vector<int>& v) {
    if (v.size() > 1) qs(v.data(), 0, (int)v.size() - 1);
}

// ================================================================
// Benchmark: returns best-of-RUNS time in nanoseconds
// ================================================================

static long long bench_ns(const vector<int>& data,
                           void (*sorter)(vector<int>&)) {
    int n = data.size();
    int reps = max(1, 100000 / max(n, 1));

    vector<int> buf(data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), buf.begin());
            sorter(buf);
        }
        auto t1 = high_resolution_clock::now();
        long long elapsed = duration_cast<nanoseconds>(t1 - t0).count();
        best = min(best, elapsed / reps);
    }
    return best;
}

static void std_sort_wrap(vector<int>& v) {
    sort(v.begin(), v.end());
}

static string fmt_time(long long ns) {
    if (ns < 1000LL)  return to_string(ns)              + " ns";
    if (ns < 1000000LL) return to_string(ns / 1000)     + " µs";
    if (ns < 1000000000LL) return to_string(ns / 1000000) + " ms";
    return                  to_string(ns / 1000000000LL) + " s";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // 2^10 to 2^20
    vector<int> sizes;
    for (int e = 10; e <= 20; e++) sizes.push_back(1 << e);

    // Correctness check
    {
        vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6, 0};
        quicksort(v);
        cout << "Quicksort correctness: " << (is_sorted(v.begin(), v.end()) ? "PASS" : "FAIL") << "\n\n";
    }

    const int W = 14;
    cout << string(78, '-') << "\n";
    cout << "quicksort (median-of-3) vs std::sort (introsort)   random int data\n";
    cout << string(78, '-') << "\n";
    cout << right
         << setw(10) << "n"
         << setw(W)  << "quicksort"
         << setw(W)  << "std::sort"
         << setw(12) << "ns/(nlogn) qs"
         << setw(14) << "ns/(nlogn) std"
         << setw(10) << "ratio"
         << "\n";
    cout << string(78, '-') << "\n";

    for (int n : sizes) {
        vector<int> data(n);
        for (int& x : data) x = dist(rng);

        long long qs_ns  = bench_ns(data, quicksort);
        long long std_ns = bench_ns(data, std_sort_wrap);

        double nlogn = (double)n * log2((double)n);
        double qs_norm  = qs_ns  / nlogn;
        double std_norm = std_ns / nlogn;
        double ratio    = (double)std_ns / (double)qs_ns;

        cout << right
             << setw(10) << n
             << setw(W)  << fmt_time(qs_ns)
             << setw(W)  << fmt_time(std_ns)
             << setw(12) << fixed << setprecision(2) << qs_norm
             << setw(14) << fixed << setprecision(2) << std_norm
             << setw(10) << fixed << setprecision(2) << ratio
             << "\n";
    }

    cout << string(78, '-') << "\n";
    cout << "ratio < 1.0 → std::sort faster; > 1.0 → quicksort faster\n";
    cout << "ns/(n log2 n) rising at large n → cache-miss pressure\n";
    return 0;
}
