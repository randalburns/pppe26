// SIMD Bitonic Sort for 16 int32 Elements — ARM NEON
//
// Algorithm 1 in arXiv:1704.08579 implements bitonic sort for 16 int32
// values using AVX-512: all 16 elements fit in one 512-bit ZMM register,
// and each comparator step is a (VPERMPS permute) + (VPMINSD/VPMAXSD) pair.
//
// This file ports the same algorithm to ARM NEON.  NEON registers are
// 128 bits (4 × int32), so four int32x4_t registers replace the single ZMM:
//
//   v0 = [a0 .. a3]   v1 = [a4 .. a7]
//   v2 = [a8 .. a11]  v3 = [a12 .. a15]
//
// The 10 comparator steps map to two classes of NEON operation:
//
//   Intra-register (partners inside the same vector):
//     Adjacent pairs  (i,i+1): vrev64q_s32 → vminq/vmaxq → vbslq blend
//     Stride-2 pairs  (i,i+2): vextq_s32(...,2) → vminq/vmaxq → vbslq blend
//
//   Cross-register (partners in different vectors):
//     Element-wise min/max:    new_va = vminq(va,vb),  new_vb = vmaxq(va,vb)
//
// A blend mask (vbslq_s32) selects which lane receives the min and which
// the max, encoding the ascending/descending direction of each comparator.
//
// Build:
//   g++ -O2 -o bitonic_sort_simd bitonic_sort_simd.cpp && ./bitonic_sort_simd

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iomanip>
#include <random>
#include <climits>
#include <string>
#include <cmath>
#include <arm_neon.h>

using namespace std;
using namespace std::chrono;

static const int RUNS = 7;
static const int N    = 16;

// ================================================================
// Blend masks — select SMALLER value where mask=0xFFFFFFFF,
//               LARGER  value where mask=0x00000000.
//
//  M_MIXED : asc pair (0,1)↑, desc pair (2,3)↓  → {1,0,0,1}
//  M_ASC_A : both asc adjacent  (0,1)↑,(2,3)↑   → {1,0,1,0}
//  M_DESC_A: both desc adjacent (0,1)↓,(2,3)↓   → {0,1,0,1}
//  M_ASC_S : both asc stride-2  (0,2)↑,(1,3)↑   → {1,1,0,0}
//  M_DESC_S: both desc stride-2 (0,2)↓,(1,3)↓   → {0,0,1,1}
// ================================================================

alignas(16) static const uint32_t MASK_MIXED [4] = {~0u, 0,   0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_A [4] = {~0u, 0,   ~0u, 0  };
alignas(16) static const uint32_t MASK_DESC_A[4] = {0,   ~0u, 0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_S [4] = {~0u, ~0u, 0,   0  };
alignas(16) static const uint32_t MASK_DESC_S[4] = {0,   0,   ~0u, ~0u};

// ================================================================
// CAS primitives
//
// cas_adj:   compare adjacent pairs (0↔1, 2↔3) via vrev64
//            vrev64([a,b,c,d]) = [b,a,d,c]
//
// cas_skip2: compare stride-2 pairs (0↔2, 1↔3) via vext rotate-by-2
//            vext([a,b,c,d],[a,b,c,d],2) = [c,d,a,b]
// ================================================================

static inline int32x4_t cas_adj(int32x4_t v, uint32x4_t sel) {
    int32x4_t rv = vrev64q_s32(v);
    int32x4_t lo = vminq_s32(v, rv);
    int32x4_t hi = vmaxq_s32(v, rv);
    return vbslq_s32(sel, lo, hi);   // sel=1 → lo (smaller), sel=0 → hi (larger)
}

static inline int32x4_t cas_skip2(int32x4_t v, uint32x4_t sel) {
    int32x4_t rv = vextq_s32(v, v, 2);
    int32x4_t lo = vminq_s32(v, rv);
    int32x4_t hi = vmaxq_s32(v, rv);
    return vbslq_s32(sel, lo, hi);
}

// ================================================================
// SIMD bitonic sort — 16 int32 elements, 10 parallel steps
// ================================================================

void bitonic_sort_simd(int* a) {
    int32x4_t v0 = vld1q_s32(a);
    int32x4_t v1 = vld1q_s32(a + 4);
    int32x4_t v2 = vld1q_s32(a + 8);
    int32x4_t v3 = vld1q_s32(a + 12);

    uint32x4_t m_mixed  = vld1q_u32(MASK_MIXED );
    uint32x4_t m_asc_a  = vld1q_u32(MASK_ASC_A );
    uint32x4_t m_desc_a = vld1q_u32(MASK_DESC_A);
    uint32x4_t m_asc_s  = vld1q_u32(MASK_ASC_S );
    uint32x4_t m_desc_s = vld1q_u32(MASK_DESC_S);

    // ---- Phase 1 (k=2): sort adjacent pairs, alternating ↑↓ ----
    // Step 1: (0,1)↑(2,3)↓ in each vector
    v0 = cas_adj(v0, m_mixed);
    v1 = cas_adj(v1, m_mixed);
    v2 = cas_adj(v2, m_mixed);
    v3 = cas_adj(v3, m_mixed);

    // ---- Phase 2 (k=4): build sorted 4-element runs ----
    // Step 2: (0,2)↑(1,3)↑ in v0,v2 ; (0,2)↓(1,3)↓ in v1,v3
    v0 = cas_skip2(v0, m_asc_s);
    v1 = cas_skip2(v1, m_desc_s);
    v2 = cas_skip2(v2, m_asc_s);
    v3 = cas_skip2(v3, m_desc_s);

    // Step 3: (0,1)↑(2,3)↑ in v0,v2 ; (0,1)↓(2,3)↓ in v1,v3
    v0 = cas_adj(v0, m_asc_a);
    v1 = cas_adj(v1, m_desc_a);
    v2 = cas_adj(v2, m_asc_a);
    v3 = cas_adj(v3, m_desc_a);

    // ---- Phase 3 (k=8): build sorted 8-element runs ----
    // Step 4: (v0,v1) ascending ; (v2,v3) descending  [cross-register]
    int32x4_t t;
    t = vminq_s32(v0, v1); v1 = vmaxq_s32(v0, v1); v0 = t;
    t = vmaxq_s32(v2, v3); v3 = vminq_s32(v2, v3); v2 = t;

    // Step 5: (0,2)↑(1,3)↑ in v0,v1 ; (0,2)↓(1,3)↓ in v2,v3
    v0 = cas_skip2(v0, m_asc_s);
    v1 = cas_skip2(v1, m_asc_s);
    v2 = cas_skip2(v2, m_desc_s);
    v3 = cas_skip2(v3, m_desc_s);

    // Step 6: (0,1)↑(2,3)↑ in v0,v1 ; (0,1)↓(2,3)↓ in v2,v3
    v0 = cas_adj(v0, m_asc_a);
    v1 = cas_adj(v1, m_asc_a);
    v2 = cas_adj(v2, m_desc_a);
    v3 = cas_adj(v3, m_desc_a);

    // ---- Phase 4 (k=16): final merge, all ascending ----
    // Step 7: (v0,v2) ascending ; (v1,v3) ascending  [cross-register]
    t = vminq_s32(v0, v2); v2 = vmaxq_s32(v0, v2); v0 = t;
    t = vminq_s32(v1, v3); v3 = vmaxq_s32(v1, v3); v1 = t;

    // Step 8: (v0,v1) ascending ; (v2,v3) ascending  [cross-register]
    t = vminq_s32(v0, v1); v1 = vmaxq_s32(v0, v1); v0 = t;
    t = vminq_s32(v2, v3); v3 = vmaxq_s32(v2, v3); v2 = t;

    // Step 9: (0,2)↑(1,3)↑ in all vectors
    v0 = cas_skip2(v0, m_asc_s);
    v1 = cas_skip2(v1, m_asc_s);
    v2 = cas_skip2(v2, m_asc_s);
    v3 = cas_skip2(v3, m_asc_s);

    // Step 10: (0,1)↑(2,3)↑ in all vectors
    v0 = cas_adj(v0, m_asc_a);
    v1 = cas_adj(v1, m_asc_a);
    v2 = cas_adj(v2, m_asc_a);
    v3 = cas_adj(v3, m_asc_a);

    vst1q_s32(a,      v0);
    vst1q_s32(a + 4,  v1);
    vst1q_s32(a + 8,  v2);
    vst1q_s32(a + 12, v3);
}

// Public API: handles n ≤ 16 by padding to exactly 16 with INT_MAX.
// INT_MAX sinks to the tail of the sorted result and is ignored on copy-back.
void bitonic_sort_simd(vector<int>& v) {
    int n = (int)v.size();
    if (n == 16) { bitonic_sort_simd(v.data()); return; }
    alignas(16) int buf[16];
    for (int i = 0; i < n;  i++) buf[i] = v[i];
    for (int i = n; i < 16; i++) buf[i] = INT_MAX;
    bitonic_sort_simd(buf);
    for (int i = 0; i < n; i++) v[i] = buf[i];
}

// ================================================================
// Scalar bitonic sort (from bitonic_sort.cpp) for comparison
// ================================================================

static inline void cas_asc (int& a, int& b) { int lo=min(a,b),hi=max(a,b); a=lo; b=hi; }
static inline void cas_desc(int& a, int& b) { int lo=min(a,b),hi=max(a,b); a=hi; b=lo; }

void bitonic_sort_scalar(int* a) {
    cas_asc (a[ 0],a[ 1]); cas_desc(a[ 2],a[ 3]);
    cas_asc (a[ 4],a[ 5]); cas_desc(a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_desc(a[10],a[11]);
    cas_asc (a[12],a[13]); cas_desc(a[14],a[15]);

    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);
    cas_desc(a[ 4],a[ 6]); cas_desc(a[ 5],a[ 7]);
    cas_asc (a[ 8],a[10]); cas_asc (a[ 9],a[11]);
    cas_desc(a[12],a[14]); cas_desc(a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);
    cas_desc(a[ 4],a[ 5]); cas_desc(a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_asc (a[10],a[11]);
    cas_desc(a[12],a[13]); cas_desc(a[14],a[15]);

    cas_asc (a[ 0],a[ 4]); cas_asc (a[ 1],a[ 5]);
    cas_asc (a[ 2],a[ 6]); cas_asc (a[ 3],a[ 7]);
    cas_desc(a[ 8],a[12]); cas_desc(a[ 9],a[13]);
    cas_desc(a[10],a[14]); cas_desc(a[11],a[15]);

    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);
    cas_asc (a[ 4],a[ 6]); cas_asc (a[ 5],a[ 7]);
    cas_desc(a[ 8],a[10]); cas_desc(a[ 9],a[11]);
    cas_desc(a[12],a[14]); cas_desc(a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);
    cas_asc (a[ 4],a[ 5]); cas_asc (a[ 6],a[ 7]);
    cas_desc(a[ 8],a[ 9]); cas_desc(a[10],a[11]);
    cas_desc(a[12],a[13]); cas_desc(a[14],a[15]);

    cas_asc (a[ 0],a[ 8]); cas_asc (a[ 1],a[ 9]);
    cas_asc (a[ 2],a[10]); cas_asc (a[ 3],a[11]);
    cas_asc (a[ 4],a[12]); cas_asc (a[ 5],a[13]);
    cas_asc (a[ 6],a[14]); cas_asc (a[ 7],a[15]);

    cas_asc (a[ 0],a[ 4]); cas_asc (a[ 1],a[ 5]);
    cas_asc (a[ 2],a[ 6]); cas_asc (a[ 3],a[ 7]);
    cas_asc (a[ 8],a[12]); cas_asc (a[ 9],a[13]);
    cas_asc (a[10],a[14]); cas_asc (a[11],a[15]);

    cas_asc (a[ 0],a[ 2]); cas_asc (a[ 1],a[ 3]);
    cas_asc (a[ 4],a[ 6]); cas_asc (a[ 5],a[ 7]);
    cas_asc (a[ 8],a[10]); cas_asc (a[ 9],a[11]);
    cas_asc (a[12],a[14]); cas_asc (a[13],a[15]);

    cas_asc (a[ 0],a[ 1]); cas_asc (a[ 2],a[ 3]);
    cas_asc (a[ 4],a[ 5]); cas_asc (a[ 6],a[ 7]);
    cas_asc (a[ 8],a[ 9]); cas_asc (a[10],a[11]);
    cas_asc (a[12],a[13]); cas_asc (a[14],a[15]);
}

void bitonic_sort_scalar(vector<int>& v) { bitonic_sort_scalar(v.data()); }

// ================================================================
// Benchmarking helpers (same pattern as std_sort.cpp)
// ================================================================

template <typename SortFn>
double bench_sort_ns(const vector<int>& data, SortFn sort_fn) {
    int n = data.size();
    int reps = max(1, 100000 / max(n, 1));

    vector<vector<int>> copies(RUNS, data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), copies[r].begin());
            sort_fn(copies[r]);
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
    return             to_string((int)(ns / 1e6))        + " ms";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // Correctness check: exhaustive on a known permutation + random
    {
        vector<int> v = {15,3,9,1,7,13,5,11,2,14,6,10,4,8,12,0};
        vector<int> ref = v;
        sort(ref.begin(), ref.end());
        bitonic_sort_simd(v);
        cout << "Correctness (SIMD):   " << (v == ref ? "PASS" : "FAIL") << "\n";

        vector<int> v2(N);
        for (int& x : v2) x = dist(rng);
        vector<int> ref2 = v2;
        sort(ref2.begin(), ref2.end());
        bitonic_sort_simd(v2);
        cout << "Correctness (random): " << (v2 == ref2 ? "PASS" : "FAIL") << "\n\n";
    }

    vector<int> data(N);
    for (int& x : data) x = dist(rng);

    double ns_simd   = bench_sort_ns(data, [](vector<int>& v){ bitonic_sort_simd(v); });
    double ns_scalar = bench_sort_ns(data, [](vector<int>& v){ bitonic_sort_scalar(v); });

    double nlog2n = (double)N * log2((double)N);

    cout << string(72, '-') << "\n";
    cout << "Bitonic sort  n=16  10 steps  80 CAS   ARM NEON vs scalar\n";
    cout << string(72, '-') << "\n";
    cout << left  << setw(32) << "version"
         << right << setw(10) << "time"
         << right << setw(14) << "ns/elem"
         << right << setw(16) << "ns/(n log2 n)" << "\n";
    cout << string(72, '-') << "\n";

    auto row = [&](const char* label, double ns) {
        cout << left  << setw(32) << label
             << right << setw(10) << fmt_time(ns)
             << right << setw(13) << fixed << setprecision(1) << ns / N
             << right << setw(15) << fixed << setprecision(2) << ns / nlog2n
             << "\n";
    };

    row("scalar (branchless min/max)", ns_scalar);
    row("SIMD (ARM NEON 4×int32)",     ns_simd);

    cout << string(72, '-') << "\n";
    cout << fixed << setprecision(2)
         << "SIMD speedup: " << ns_scalar / max(1.0, ns_simd) << "x\n";
    cout << "\nEach NEON step processes 4 comparators in parallel.\n";
    cout << "10 steps × 4-wide = 40 NEON instructions vs 80 scalar CAS.\n";

    return 0;
}
