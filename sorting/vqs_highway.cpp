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
//   clang++ -O2 -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -lhwy \
//     -o vqs_highway vqs_highway.cpp && ./vqs_highway

#include "hwy/highway.h"

#include <algorithm>
#include <arm_neon.h>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
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
// bitonic16 — raw ARM NEON, 10-step network for exactly 16 int32 elements
// Paper's Algorithm 1/2.  Uses vbslq_s32 blend in one instruction where the
// Highway version needs Combine/LowerHalf/UpperHalf (three instructions).
// ================================================================
alignas(16) static const uint32_t MASK_MIXED [4] = {~0u, 0,   0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_A [4] = {~0u, 0,   ~0u, 0  };
alignas(16) static const uint32_t MASK_DESC_A[4] = {0,   ~0u, 0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_S [4] = {~0u, ~0u, 0,   0  };
alignas(16) static const uint32_t MASK_DESC_S[4] = {0,   0,   ~0u, ~0u};

static inline int32x4_t cas_adj (int32x4_t v, uint32x4_t s) {
    int32x4_t r = vrev64q_s32(v);
    return vbslq_s32(s, vminq_s32(v, r), vmaxq_s32(v, r));
}
static inline int32x4_t cas_skip2(int32x4_t v, uint32x4_t s) {
    int32x4_t r = vextq_s32(v, v, 2);
    return vbslq_s32(s, vminq_s32(v, r), vmaxq_s32(v, r));
}

static void bitonic16(int* a) {
    int32x4_t v0 = vld1q_s32(a),     v1 = vld1q_s32(a + 4);
    int32x4_t v2 = vld1q_s32(a + 8), v3 = vld1q_s32(a + 12);
    uint32x4_t mm  = vld1q_u32(MASK_MIXED),  ma = vld1q_u32(MASK_ASC_A);
    uint32x4_t md  = vld1q_u32(MASK_DESC_A), ms = vld1q_u32(MASK_ASC_S);
    uint32x4_t mds = vld1q_u32(MASK_DESC_S);

    // Phase 1 (k=2)
    v0=cas_adj(v0,mm); v1=cas_adj(v1,mm); v2=cas_adj(v2,mm); v3=cas_adj(v3,mm);
    // Phase 2 (k=4)
    v0=cas_skip2(v0,ms);  v1=cas_skip2(v1,mds); v2=cas_skip2(v2,ms);  v3=cas_skip2(v3,mds);
    v0=cas_adj(v0,ma);    v1=cas_adj(v1,md);     v2=cas_adj(v2,ma);    v3=cas_adj(v3,md);
    // Phase 3 (k=8)
    int32x4_t t;
    t=vminq_s32(v0,v1); v1=vmaxq_s32(v0,v1); v0=t;
    t=vmaxq_s32(v2,v3); v3=vminq_s32(v2,v3); v2=t;
    v0=cas_skip2(v0,ms);  v1=cas_skip2(v1,ms);  v2=cas_skip2(v2,mds); v3=cas_skip2(v3,mds);
    v0=cas_adj(v0,ma);    v1=cas_adj(v1,ma);     v2=cas_adj(v2,md);    v3=cas_adj(v3,md);
    // Phase 4 (k=16)
    t=vminq_s32(v0,v2); v2=vmaxq_s32(v0,v2); v0=t;
    t=vminq_s32(v1,v3); v3=vmaxq_s32(v1,v3); v1=t;
    t=vminq_s32(v0,v1); v1=vmaxq_s32(v0,v1); v0=t;
    t=vminq_s32(v2,v3); v3=vmaxq_s32(v2,v3); v2=t;
    v0=cas_skip2(v0,ms); v1=cas_skip2(v1,ms); v2=cas_skip2(v2,ms); v3=cas_skip2(v3,ms);
    v0=cas_adj(v0,ma);   v1=cas_adj(v1,ma);   v2=cas_adj(v2,ma);   v3=cas_adj(v3,ma);

    vst1q_s32(a,     v0); vst1q_s32(a + 4,  v1);
    vst1q_s32(a + 8, v2); vst1q_s32(a + 12, v3);
}

// ================================================================
// sort_small — dispatcher: raw NEON bitonic for n==16, insertion sort otherwise
// Highway is intentionally NOT used here — vbslq_s32 beats the Highway blend.
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
template <typename Fn>
static double bench_ns(const vector<int>& data, Fn fn) {
    const int n    = (int)data.size();
    const int reps = max(1, 100000 / max(n, 1));
    vector<vector<int>> copies(RUNS, data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), copies[r].begin());
            fn(copies[r]);
        }
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<nanoseconds>(t1 - t0).count() / reps);
    }
    return (double)best;
}

static string fmt_time(double ns) {
    if (ns < 1e3) return to_string((int)ns)         + " ns";
    if (ns < 1e6) return to_string((int)(ns / 1e3)) + " µs";
    if (ns < 1e9) return to_string((int)(ns / 1e6)) + " ms";
    return          to_string((int)(ns / 1e9))        + " s";
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
        vector<int> data(n);
        for (int& x : data) x = dist(rng);

        const double ns_v  = bench_ns(data, vqs_fn);
        const double ns_s  = bench_ns(data, std_fn);
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
