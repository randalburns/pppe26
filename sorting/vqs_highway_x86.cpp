// Vectorized Quicksort — Algorithm 4 from Bramas (arxiv:1704.08579)
// implemented with Google Highway for portable SIMD.
//
// Paper: "A Novel Hybrid Quicksort Algorithm Vectorized using AVX-512 on Intel Skylake"
// Original uses AVX-512 intrinsics (_mm512_mask_compressstoreu_epi32).
// This version uses Highway's CompressStore, which maps to:
//   vpcompressd   on AVX-512  (16 int32 lanes)
//   LUT sequence  on NEON     ( 4 int32 lanes, 128-bit)
//   permd         on AVX2     ( 8 int32 lanes)
//
// Four components from the paper:
//   insertion_sort  — base case for n < 16
//   bitonic16       — 10-step sorting network for n == 16 (paper's small-array sort)
//   simd_partition  — vectorized partition via CompressStore (Algorithm 3)
//   simd_qs_core    — median-of-3 pivot + recursive driver  (Algorithm 4)
//
// sort_small dispatches between insertion_sort and bitonic16.
// Scratch buffers (2 × n ints) allocated once at the public entry point and
// threaded through recursion to avoid per-call heap allocation.
//
// Build:
//   g++ -O2 -std=c++17 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul \
//     -o vqs_x86 vqs_highway_x86.cpp -lhwy && ./vqs_x86

#include "hwy/highway.h"

#include <algorithm>
#include <immintrin.h>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;
namespace hn = hwy::HWY_NAMESPACE;

static constexpr int RUNS       = 7;
static constexpr int SORT_BOUND = 16;  // dispatch to sort_small below this size

// ================================================================
// bitonic16 — AVX2, 2 x __m256i, 10-step network for exactly 16 int32
//
// x86 replacement for the ARM NEON base case in vqs_highway.cpp.  All 16
// values live in two 256-bit registers, so a comparator step handles 8
// lanes instead of 4.  Standard bitonic recurrence: stages k = 2,4,8,16,
// passes at distance d = k/2 … 1; element g keeps the min when
// ((g & d) == 0) XOR ((g & k) != 0).  That predicate collapses to one
// 8-bit blend immediate per register per step, computed offline.
//
// Distance 8 is the only cross-register step and falls in the final
// all-ascending stage, so it costs a bare min/max pair with no shuffle.
// Measured at 14 ns for an isolated n=16 sort on Zen 5, against 22 ns for
// the 4x128-bit form that mirrors the NEON original.
// ================================================================
template <int SEL>
static inline __m256i y_cas(__m256i v, __m256i rv) {
    return _mm256_blend_epi32(_mm256_max_epi32(v, rv), _mm256_min_epi32(v, rv), SEL);
}
static inline __m256i y_rot1(__m256i v) { return _mm256_shuffle_epi32(v, _MM_SHUFFLE(2,3,0,1)); }
static inline __m256i y_rot2(__m256i v) { return _mm256_shuffle_epi32(v, _MM_SHUFFLE(1,0,3,2)); }
static inline __m256i y_rot4(__m256i v) { return _mm256_permute2x128_si256(v, v, 0x01); }

static void bitonic16(int* a) {
    __m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a));
    __m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a + 8));

    v0 = y_cas<0x99>(v0, y_rot1(v0)); v1 = y_cas<0x99>(v1, y_rot1(v1)); // k=2  d=1
    v0 = y_cas<0xC3>(v0, y_rot2(v0)); v1 = y_cas<0xC3>(v1, y_rot2(v1)); // k=4  d=2
    v0 = y_cas<0xA5>(v0, y_rot1(v0)); v1 = y_cas<0xA5>(v1, y_rot1(v1)); // k=4  d=1
    v0 = y_cas<0x0F>(v0, y_rot4(v0)); v1 = y_cas<0xF0>(v1, y_rot4(v1)); // k=8  d=4
    v0 = y_cas<0x33>(v0, y_rot2(v0)); v1 = y_cas<0xCC>(v1, y_rot2(v1)); // k=8  d=2
    v0 = y_cas<0x55>(v0, y_rot1(v0)); v1 = y_cas<0xAA>(v1, y_rot1(v1)); // k=8  d=1
    { __m256i lo = _mm256_min_epi32(v0, v1);                            // k=16 d=8
      v1 = _mm256_max_epi32(v0, v1); v0 = lo; }
    v0 = y_cas<0x0F>(v0, y_rot4(v0)); v1 = y_cas<0x0F>(v1, y_rot4(v1)); // k=16 d=4
    v0 = y_cas<0x33>(v0, y_rot2(v0)); v1 = y_cas<0x33>(v1, y_rot2(v1)); // k=16 d=2
    v0 = y_cas<0x55>(v0, y_rot1(v0)); v1 = y_cas<0x55>(v1, y_rot1(v1)); // k=16 d=1

    _mm256_storeu_si256(reinterpret_cast<__m256i*>(a),     v0);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(a + 8), v1);
}

// ================================================================
// sort_small — dispatcher: AVX2 bitonic for n==16, insertion sort otherwise
//
// The ARM original used raw NEON here because vbslq_s32 blends in one
// instruction where Highway's Combine/UpperHalf/LowerHalf needs three.
// That rationale does NOT carry over to x86: measured on this machine,
// Highway's 128-bit form ties the hand-written 128-bit intrinsics under
// clang (22 ns each) and loses only under GCC (26 vs 21 ns).  The reason
// to hand-write the base case on x86 is width, not blend lowering —
// Highway is pinned to FixedTag<int32_t,4> by the network's structure,
// while 2x256-bit reaches 14 ns.  See small_sorting_ryzen.md.
// ================================================================
static void insertion_sort(int* a, int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i], j = i - 1;
        while (j >= 0 && a[j] > key) { a[j+1] = a[j]; j--; }
        a[j+1] = key;
    }
}
static void sort_small(int* a, int n) {
    if (n == 16) bitonic16(a);
    else         insertion_sort(a, n);
}

// ================================================================
// simd_partition — Algorithm 3 (vectorized Lomuto-style partition)
//
// Partitions a[lo..hi) — hi is EXCLUSIVE; pivot is NOT in this range.
// Loads VEC_LANES elements per iteration, compares against broadcast pivot,
// and CompressStore-scatters elements < pivot into left_buf and
// elements >= pivot into right_buf.  Scalar loop handles the tail.
// Writes both buffers back contiguously to a[lo..hi).
// Returns lc: number of elements < pivot; caller places pivot at a[lo+lc].
// ================================================================
static int simd_partition(int* a, int lo, int hi, int pivot,
                           int* left_buf, int* right_buf) {
    const HWY_FULL(int32_t) d;
    const int N = (int)hn::Lanes(d);

    const auto pivot_v = hn::Set(d, pivot);
    int lc = 0, rc = 0;

    // Vectorized loop — process N elements per iteration
    const int vend = lo + ((hi - lo) / N) * N;
    for (int i = lo; i < vend; i += N) {
        const auto v       = hn::LoadU(d, a + i);
        const auto lt_mask = hn::Lt(v, pivot_v);   // v[i] < pivot
        const auto ge_mask = hn::Ge(v, pivot_v);   // v[i] >= pivot
        lc += (int)hn::CompressStore(v, lt_mask, d, left_buf  + lc);
        rc += (int)hn::CompressStore(v, ge_mask, d, right_buf + rc);
    }
    // Scalar tail for the remaining < N elements
    for (int i = vend; i < hi; i++) {
        if (a[i] < pivot) left_buf[lc++] = a[i];
        else              right_buf[rc++] = a[i];
    }

    // Write classified elements back in order: [left | right]
    memcpy(a + lo,      left_buf,  lc * sizeof(int));
    memcpy(a + lo + lc, right_buf, rc * sizeof(int));
    return lc;
}

// ================================================================
// simd_qs_core — Algorithm 4 (recursive SIMD quicksort)
//
// Invariant after median-of-3:
//   a[lo] = min, a[mid] = max, a[hi] = median = pivot
// simd_partition fills a[lo..hi-1] with [lt-elems | ge-elems].
// Swapping a[lo+lc] with a[hi] places pivot at its final position p.
// ================================================================
static void simd_qs_core(int* a, int lo, int hi,
                          int* left_buf, int* right_buf) {
    if (hi - lo + 1 <= SORT_BOUND) {
        sort_small(a + lo, hi - lo + 1);
        return;
    }

    // Median-of-3: sort a[lo] ≤ a[mid] ≤ a[hi], then swap median to a[hi]
    int mid = lo + (hi - lo) / 2;
    if (a[lo]  > a[mid]) swap(a[lo],  a[mid]);
    if (a[lo]  > a[hi])  swap(a[lo],  a[hi]);
    if (a[mid] > a[hi])  swap(a[mid], a[hi]);
    swap(a[mid], a[hi]);               // median → a[hi], max → a[mid]
    const int pivot = a[hi];

    // Partition a[lo..hi-1]; pivot is excluded (sits at a[hi])
    const int lc = simd_partition(a, lo, hi, pivot, left_buf, right_buf);

    // Place pivot: a[lo..lo+lc-1] < pivot, a[lo+lc..hi-1] ≥ pivot, a[hi] = pivot
    // Swap a[lo+lc] ↔ a[hi] to insert pivot between the two groups.
    // a[hi] (an element ≥ pivot) moves into the right subarray [p+1..hi]. ✓
    swap(a[lo + lc], a[hi]);
    const int p = lo + lc;

    simd_qs_core(a, lo,    p - 1, left_buf, right_buf);
    simd_qs_core(a, p + 1, hi,    left_buf, right_buf);
}

// ================================================================
// Public entry point
// ================================================================
void vqs_sort(vector<int>& v) {
    const int n = (int)v.size();
    if (n <= SORT_BOUND) { sort_small(v.data(), n); return; }
    vector<int> left_buf(n), right_buf(n);
    simd_qs_core(v.data(), 0, n - 1, left_buf.data(), right_buf.data());
}

// ================================================================
// Benchmark harness
// ================================================================
// bench_sort — time one sort of n random int32.
//
// NOTE ON METHOD.  The ARM original timed `copy(src -> buf); fn(buf)` in a
// tight loop over the SAME src array.  That makes every data-dependent branch
// in a comparison sort perfectly predictable after warm-up, and at n <= 4096
// the whole branch history fits the predictor: std::sort measured 28 us where
// its true cost is 260 us, a 9x flattering error that vanishes at large n
// (where reps == 1).  Comparing a branchless SIMD sort against a branchy one
// on that harness is not a fair test.
//
// Instead: pre-build K independent random buffers, refill them OUTSIDE the
// timed region, and time sorting all K.  Every sort sees data it has not seen
// before, so branch behaviour is representative.
template <typename Fn>
static double bench_sort(int n, mt19937& rng, Fn fn) {
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
    const size_t cap = (8u << 20) / (n * sizeof(int));          // ~8 MB of buffers
    const size_t K   = max<size_t>(3, min<size_t>(64, cap));
    vector<vector<int>> bufs(K, vector<int>(n));
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        for (auto& b : bufs) for (int& x : b) x = dist(rng);    // outside the timer
        auto t0 = high_resolution_clock::now();
        for (auto& b : bufs) fn(b);
        auto t1 = high_resolution_clock::now();
        best = min<long long>(best,
               duration_cast<nanoseconds>(t1 - t0).count() / (long long)K);
    }
    return (double)best;
}

static string fmt_time(double ns) {
    char b[32];
    if (ns < 1e3) { snprintf(b, sizeof b, "%.0f ns", ns);       return b; }
    if (ns < 1e6) { snprintf(b, sizeof b, "%.1f \u00b5s", ns/1e3); return b; }
    if (ns < 1e9) { snprintf(b, sizeof b, "%.2f ms", ns/1e6);   return b; }
    snprintf(b, sizeof b, "%.2f s", ns/1e9);                    return b;
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // --- Correctness ---
    {
        auto check = [](vector<int> v, const char* label) {
            vector<int> ref = v;
            sort(ref.begin(), ref.end());
            vqs_sort(v);
            cout << label << (v == ref ? "PASS" : "FAIL") << "\n";
        };
        check({5, 3, 8, 1, 9, 2, 7, 4, 6, 0},      "Correctness (n=10):   ");
        check({42},                                    "Correctness (n=1):    ");
        check({2, 1},                                  "Correctness (n=2):    ");
        check({1, 1, 1, 1},                            "Correctness (all-eq): ");

        mt19937 r2(99);
        uniform_int_distribution<int> d2(INT_MIN, INT_MAX);
        vector<int> big(10000);
        for (int& x : big) x = d2(r2);
        check(big, "Correctness (n=10k):  ");

        cout << "HWY target: " << hwy::TargetName(HWY_TARGET) << "\n\n";
    }

    // --- Benchmark: vqs_highway vs std::sort ---
    auto std_fn = [](vector<int>& v) { sort(v.begin(), v.end()); };
    auto vqs_fn = [](vector<int>& v) { vqs_sort(v); };

    constexpr int W = 78;
    cout << string(W, '-') << "\n";
    cout << "vqs_highway vs std::sort   (Algorithm 4 / CompressStore partition)\n";
    cout << string(W, '-') << "\n";
    cout << right << setw(10) << "n"
         << right << setw(12) << "vqs"
         << right << setw(12) << "std::sort"
         << right << setw(10) << "speedup"
         << right << setw(14) << "ns/elem"
         << right << setw(16) << "ns/(n log2 n)" << "\n";
    cout << string(W, '-') << "\n";

    for (int e = 4; e <= 20; e++) {
        const int n = 1 << e;
        const double ns_v  = bench_sort(n, rng, vqs_fn);
        const double ns_s  = bench_sort(n, rng, std_fn);
        const double nlogn = (double)n * log2((double)n);

        cout << right << setw(10) << n
             << right << setw(12) << fmt_time(ns_v)
             << right << setw(12) << fmt_time(ns_s)
             << right << setw(9)  << fixed << setprecision(2) << ns_s / ns_v << "x"
             << right << setw(13) << fixed << setprecision(1) << ns_v / n
             << right << setw(15) << fixed << setprecision(2) << ns_v / nlogn
             << "\n";
    }

    cout << string(W, '-') << "\n";
    {
        const HWY_FULL(int32_t) d;
        cout << "SORT_BOUND=" << SORT_BOUND
             << "  VEC_LANES=" << hn::Lanes(d)
             << "  target=" << hwy::TargetName(HWY_TARGET) << "\n";
    }
    return 0;
}
