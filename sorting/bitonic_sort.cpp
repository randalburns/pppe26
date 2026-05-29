// Bitonic Sorting Network for 16 Elements
//
// A sorting network is a fixed sequence of compare-and-swap (CAS) operations
// whose wiring is independent of the data.  Because every comparator is
// predetermined, there are no branches, no recursion, and no data-dependent
// control flow — the same instruction sequence executes for any input.
//
// Bitonic sort for n=2^k builds the network in phases.  Each phase doubles
// the length of the sorted sub-sequences by first creating a "bitonic"
// sequence (one that rises then falls) and then merging it into a sorted one.
// For n=16 this produces log2(16) = 4 phases, totalling:
//
//   log2(n) * (log2(n)+1) / 2  parallel steps  =  4*5/2  = 10 steps
//   n/2 comparators per step                    =  8      comparators/step
//   total comparators                           =  80     CAS operations
//
// The direction of each CAS (ascending or descending) is determined by the
// phase structure: alternating groups are sorted in opposite directions to
// create the bitonic sequences that subsequent steps can merge.  Steps 7-10
// are all ascending because by that point both halves are already bitonic
// and the final merge needs no direction change.
//
// All 10 steps are unrolled explicitly below; the compiler sees a straight
// sequence of branchless min/max operations with no loop overhead.
//
// Build:
//   g++ -O2 -o bitonic_sort bitonic_sort.cpp && ./bitonic_sort

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
static const int N    = 16;

// ================================================================
// Compare-and-swap primitives (branchless via min/max)
// ================================================================

static inline void cas_asc(int& a, int& b) {    // keep smaller at a
    int lo = min(a, b), hi = max(a, b);
    a = lo; b = hi;
}
static inline void cas_desc(int& a, int& b) {   // keep larger at a
    int lo = min(a, b), hi = max(a, b);
    a = hi; b = lo;
}

// ================================================================
// Bitonic sorting network — 16 elements, 10 parallel steps
//
// Constructed with:
//   for k in {2,4,8,16}:
//     for j in {k/2, k/4, ..., 1}:
//       for each pair (i, i^j) where i^j > i:
//         direction = ascending if (i & k)==0, descending otherwise
//
// Steps 1-6 build four sorted 4-element runs then merge them into
// two sorted 8-element halves (one ascending, one descending).
// Steps 7-10 merge those halves into one sorted 16-element sequence.
// ================================================================

void bitonic_sort(int* a) {
    // ---- Phase 1 (k=2): sort pairs, alternating direction ----
    cas_asc (a[ 0],a[ 1]); cas_desc(a[ 2],a[ 3]);   // step 1
    cas_asc (a[ 4],a[ 5]); cas_desc(a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_desc(a[10],a[11]);
    cas_asc (a[12],a[13]); cas_desc(a[14],a[15]);

    // ---- Phase 2 (k=4): build sorted 4-element runs ----
    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);   // step 2
    cas_desc(a[ 4],a[ 6]); cas_desc(a[ 5],a[ 7]);
    cas_asc (a[ 8],a[10]); cas_asc (a[ 9],a[11]);
    cas_desc(a[12],a[14]); cas_desc(a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);   // step 3
    cas_desc(a[ 4],a[ 5]); cas_desc(a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_asc (a[10],a[11]);
    cas_desc(a[12],a[13]); cas_desc(a[14],a[15]);

    // ---- Phase 3 (k=8): build sorted 8-element runs ----
    cas_asc (a[ 0],a[ 4]); cas_asc (a[ 1],a[ 5]);   // step 4
    cas_asc (a[ 2],a[ 6]); cas_asc (a[ 3],a[ 7]);
    cas_desc(a[ 8],a[12]); cas_desc(a[ 9],a[13]);
    cas_desc(a[10],a[14]); cas_desc(a[11],a[15]);

    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);   // step 5
    cas_asc (a[ 4],a[ 6]); cas_asc (a[ 5],a[ 7]);
    cas_desc(a[ 8],a[10]); cas_desc(a[ 9],a[11]);
    cas_desc(a[12],a[14]); cas_desc(a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);   // step 6
    cas_asc (a[ 4],a[ 5]); cas_asc (a[ 6],a[ 7]);
    cas_desc(a[ 8],a[ 9]); cas_desc(a[10],a[11]);
    cas_desc(a[12],a[13]); cas_desc(a[14],a[15]);

    // ---- Phase 4 (k=16): final merge, all ascending ----
    cas_asc (a[ 0],a[ 8]); cas_asc (a[ 1],a[ 9]);   // step 7
    cas_asc (a[ 2],a[10]); cas_asc (a[ 3],a[11]);
    cas_asc (a[ 4],a[12]); cas_asc (a[ 5],a[13]);
    cas_asc (a[ 6],a[14]); cas_asc (a[ 7],a[15]);

    cas_asc (a[ 0],a[ 4]); cas_asc (a[ 1],a[ 5]);   // step 8
    cas_asc (a[ 2],a[ 6]); cas_asc (a[ 3],a[ 7]);
    cas_asc (a[ 8],a[12]); cas_asc (a[ 9],a[13]);
    cas_asc (a[10],a[14]); cas_asc (a[11],a[15]);

    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);   // step 9
    cas_asc (a[ 4],a[ 6]); cas_asc (a[ 5],a[ 7]);
    cas_asc (a[ 8],a[10]); cas_asc (a[ 9],a[11]);
    cas_asc (a[12],a[14]); cas_asc (a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);   // step 10
    cas_asc (a[ 4],a[ 5]); cas_asc (a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_asc (a[10],a[11]);
    cas_asc (a[12],a[13]); cas_asc (a[14],a[15]);
}

void bitonic_sort(vector<int>& v) { bitonic_sort(v.data()); }

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
            bitonic_sort(copies[r]);
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

    // Correctness check.
    {
        vector<int> v = {15,3,9,1,7,13,5,11,2,14,6,10,4,8,12,0};
        bitonic_sort(v);
        cout << "Correctness: " << (is_sorted(v.begin(), v.end()) ? "PASS" : "FAIL") << "\n\n";
    }

    vector<int> data(N);
    for (int& x : data) x = dist(rng);

    double ns            = bench_sort_ns(data);
    double ns_per_elem   = ns / N;
    double nlog2n        = (double)N * log2((double)N);
    double ns_per_nlog2n = ns / nlog2n;

    cout << string(62, '-') << "\n";
    cout << "bitonic sorting network (n=16, 10 steps, 80 CAS)\n";
    cout << string(62, '-') << "\n";
    cout << right << setw(12) << "n"
         << right << setw(12) << "time"
         << right << setw(14) << "ns/elem"
         << right << setw(16) << "ns/(n log2 n)" << "\n";
    cout << string(62, '-') << "\n";
    cout << right << setw(12) << N
         << right << setw(12) << fmt_time(ns)
         << right << setw(13) << fixed << setprecision(1) << ns_per_elem
         << right << setw(15) << fixed << setprecision(2) << ns_per_nlog2n
         << "\n";
    cout << string(62, '-') << "\n";
    cout << "No branches, no recursion — fixed comparator sequence.\n";

    return 0;
}
