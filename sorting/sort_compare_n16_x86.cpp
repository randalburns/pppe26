// Sorting Algorithm Comparison at n=16 — x86-64 (AVX2) edition
//
// Runs seven sort implementations on the same 16-element random int array
// and prints a single speedup table normalised to quicksort.  This is the
// x86 counterpart of sort_compare_n16.cpp (which uses ARM NEON).
//
// Implementations:
//   std::sort         — libstdc++ introsort
//   quicksort         — recursive median-of-3, no small-partition cutoff
//   insertion sort    — classic O(n^2) in-place sort
//   bitonic scalar    — branchless min/max network, 80 CAS ops
//   bitonic AVX2 128b — 4 x __m128i, direct translation of the NEON version
//   bitonic AVX2 256b — 2 x __m256i, full-width x86 formulation
//   bitonic Highway   — Google Highway portable SIMD (FixedTag<int32_t,4>)
//
// Build:
//   g++ -O2 -std=c++17 -mavx2 -I<hwy include> \
//     -o sort_compare_n16_x86 sort_compare_n16_x86.cpp -lhwy && ./sort_compare_n16_x86

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iomanip>
#include <random>
#include <climits>
#include <string>
#include <sstream>
#include <cmath>
#include <immintrin.h>
#include "hwy/highway.h"

using namespace std;
using namespace std::chrono;
namespace hn = hwy::HWY_NAMESPACE;

static const int RUNS = 7;
static const int N    = 16;

// ================================================================
// std::sort
// ================================================================

void sort_std(vector<int>& v) { sort(v.begin(), v.end()); }

// ================================================================
// Quicksort — median-of-3, no small-partition cutoff
// ================================================================

static int qs_median3(int* a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    if (a[lo] > a[mid]) swap(a[lo], a[mid]);
    if (a[lo] > a[hi])  swap(a[lo], a[hi]);
    if (a[mid] > a[hi]) swap(a[mid], a[hi]);
    swap(a[mid], a[hi - 1]);
    return a[hi - 1];
}

static void qs_impl(int* a, int lo, int hi) {
    if (hi <= lo) return;
    if (hi - lo == 1) { if (a[lo] > a[hi]) swap(a[lo], a[hi]); return; }
    int pivot = qs_median3(a, lo, hi);
    int i = lo, j = hi - 1;
    for (;;) {
        while (a[++i] < pivot) {}
        while (a[--j] > pivot) {}
        if (i >= j) break;
        swap(a[i], a[j]);
    }
    swap(a[i], a[hi - 1]);
    qs_impl(a, lo, i - 1);
    qs_impl(a, i + 1, hi);
}

void sort_quicksort(vector<int>& v) {
    if (v.size() > 1) qs_impl(v.data(), 0, (int)v.size() - 1);
}

// ================================================================
// Insertion sort
// ================================================================

void sort_insertion(vector<int>& v) {
    int n = v.size();
    for (int i = 1; i < n; i++) {
        int key = v[i], j = i - 1;
        while (j >= 0 && v[j] > key) { v[j + 1] = v[j]; j--; }
        v[j + 1] = key;
    }
}

// ================================================================
// Bitonic sort — scalar branchless min/max, 10 steps, 80 CAS
// ================================================================

static inline void cas_asc (int& a, int& b) { int lo=min(a,b),hi=max(a,b); a=lo; b=hi; }
static inline void cas_desc(int& a, int& b) { int lo=min(a,b),hi=max(a,b); a=hi; b=lo; }

void sort_bitonic_scalar(int* a) {
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

void sort_bitonic_scalar(vector<int>& v) { sort_bitonic_scalar(v.data()); }

// ================================================================
// Bitonic sort — AVX2, 4 x __m128i
//
// Mechanical translation of the ARM NEON version in sort_compare_n16.cpp:
//   vminq_s32/vmaxq_s32  -> _mm_min_epi32/_mm_max_epi32   (SSE4.1)
//   vrev64q_s32(v)       -> _mm_shuffle_epi32(v, 0xB1)    [b,a,d,c]
//   vextq_s32(v,v,2)     -> _mm_shuffle_epi32(v, 0x4E)    [c,d,a,b]
//   vbslq_s32(sel,lo,hi) -> _mm_blend_epi32(hi,lo,imm)    (AVX2, 1 uop)
//
// The lane-select masks become blend immediates:
//   MIXED  {~0,0,0,~0} -> 0x9    ASC_ADJ  {~0,0,~0,0} -> 0x5
//   DESC_ADJ {0,~0,0,~0} -> 0xA  ASC_SKIP2 {~0,~0,0,0} -> 0x3
//   DESC_SKIP2 {0,0,~0,~0} -> 0xC
// ================================================================

template <int SEL>
static inline __m128i x86_cas_adj(__m128i v) {
    __m128i rv = _mm_shuffle_epi32(v, _MM_SHUFFLE(2,3,0,1));
    return _mm_blend_epi32(_mm_max_epi32(v, rv), _mm_min_epi32(v, rv), SEL);
}

template <int SEL>
static inline __m128i x86_cas_skip2(__m128i v) {
    __m128i rv = _mm_shuffle_epi32(v, _MM_SHUFFLE(1,0,3,2));
    return _mm_blend_epi32(_mm_max_epi32(v, rv), _mm_min_epi32(v, rv), SEL);
}

void sort_bitonic_avx128(int* a) {
    const __m128i* p = reinterpret_cast<const __m128i*>(a);
    __m128i v0 = _mm_loadu_si128(p),     v1 = _mm_loadu_si128(p + 1);
    __m128i v2 = _mm_loadu_si128(p + 2), v3 = _mm_loadu_si128(p + 3);

    v0=x86_cas_adj<0x9>(v0);  v1=x86_cas_adj<0x9>(v1);    // step 1
    v2=x86_cas_adj<0x9>(v2);  v3=x86_cas_adj<0x9>(v3);

    v0=x86_cas_skip2<0x3>(v0); v1=x86_cas_skip2<0xC>(v1);  // step 2
    v2=x86_cas_skip2<0x3>(v2); v3=x86_cas_skip2<0xC>(v3);

    v0=x86_cas_adj<0x5>(v0);  v1=x86_cas_adj<0xA>(v1);     // step 3
    v2=x86_cas_adj<0x5>(v2);  v3=x86_cas_adj<0xA>(v3);

    __m128i t;                                             // step 4
    t=_mm_min_epi32(v0,v1); v1=_mm_max_epi32(v0,v1); v0=t;
    t=_mm_max_epi32(v2,v3); v3=_mm_min_epi32(v2,v3); v2=t;

    v0=x86_cas_skip2<0x3>(v0); v1=x86_cas_skip2<0x3>(v1);  // step 5
    v2=x86_cas_skip2<0xC>(v2); v3=x86_cas_skip2<0xC>(v3);

    v0=x86_cas_adj<0x5>(v0);  v1=x86_cas_adj<0x5>(v1);     // step 6
    v2=x86_cas_adj<0xA>(v2);  v3=x86_cas_adj<0xA>(v3);

    t=_mm_min_epi32(v0,v2); v2=_mm_max_epi32(v0,v2); v0=t; // step 7
    t=_mm_min_epi32(v1,v3); v3=_mm_max_epi32(v1,v3); v1=t;

    t=_mm_min_epi32(v0,v1); v1=_mm_max_epi32(v0,v1); v0=t; // step 8
    t=_mm_min_epi32(v2,v3); v3=_mm_max_epi32(v2,v3); v2=t;

    v0=x86_cas_skip2<0x3>(v0); v1=x86_cas_skip2<0x3>(v1);  // step 9
    v2=x86_cas_skip2<0x3>(v2); v3=x86_cas_skip2<0x3>(v3);

    v0=x86_cas_adj<0x5>(v0);  v1=x86_cas_adj<0x5>(v1);     // step 10
    v2=x86_cas_adj<0x5>(v2);  v3=x86_cas_adj<0x5>(v3);

    __m128i* q = reinterpret_cast<__m128i*>(a);
    _mm_storeu_si128(q, v0);     _mm_storeu_si128(q + 1, v1);
    _mm_storeu_si128(q + 2, v2); _mm_storeu_si128(q + 3, v3);
}

void sort_bitonic_avx128(vector<int>& v) { sort_bitonic_avx128(v.data()); }

// ================================================================
// Bitonic sort — AVX2, 2 x __m256i (full-width x86 formulation)
//
// 16 int32 live in two 256-bit registers: v0 = a[0..7], v1 = a[8..15].
// Standard bitonic network: stages k = 2,4,8,16, each with passes at
// distance d = k/2 ... 1.  Element g is compared with g^d and keeps the
// min when ((g & d) == 0) XOR ((g & k) != 0).  That predicate collapses
// into a per-register 8-bit blend immediate, computed once per step.
//
//   d = 8 : cross-register (only in the final stage, all ascending)
//   d = 4 : _mm256_permute2x128_si256(v, v, 0x01)  — swap 128-bit halves
//   d = 2 : _mm256_shuffle_epi32(v, 0x4E)
//   d = 1 : _mm256_shuffle_epi32(v, 0xB1)
// ================================================================

template <int SEL>
static inline __m256i y_cas(__m256i v, __m256i rv) {
    return _mm256_blend_epi32(_mm256_max_epi32(v, rv), _mm256_min_epi32(v, rv), SEL);
}
static inline __m256i y_rot1(__m256i v) { return _mm256_shuffle_epi32(v, _MM_SHUFFLE(2,3,0,1)); }
static inline __m256i y_rot2(__m256i v) { return _mm256_shuffle_epi32(v, _MM_SHUFFLE(1,0,3,2)); }
static inline __m256i y_rot4(__m256i v) { return _mm256_permute2x128_si256(v, v, 0x01); }

void sort_bitonic_avx256(int* a) {
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

void sort_bitonic_avx256(vector<int>& v) { sort_bitonic_avx256(v.data()); }

// ================================================================
// Bitonic sort — Google Highway (portable SIMD)
// ================================================================

using D4 = hn::FixedTag<int32_t, 4>;
using D2 = hn::Half<D4>;
using V4 = hn::Vec<D4>;
using V2 = hn::Vec<D2>;

static inline V4 hwy_cas_asc_adj(V4 v) {
    const D4 d4;
    V4 rv = hn::Reverse2(d4, v), lo = hn::Min(v,rv), hi = hn::Max(v,rv);
    return hn::OddEven(hi, lo);
}
static inline V4 hwy_cas_desc_adj(V4 v) {
    const D4 d4;
    V4 rv = hn::Reverse2(d4, v), lo = hn::Min(v,rv), hi = hn::Max(v,rv);
    return hn::OddEven(lo, hi);
}
static inline V4 hwy_cas_mixed_adj(V4 v) {
    const D4 d4; const D2 d2;
    V4 rv = hn::Reverse2(d4, v), lo = hn::Min(v,rv), hi = hn::Max(v,rv);
    V4 asc = hn::OddEven(hi,lo), desc = hn::OddEven(lo,hi);
    return hn::Combine(d4, hn::UpperHalf(d2,desc), hn::LowerHalf(asc));
}
static inline V4 hwy_cas_asc_skip2(V4 v) {
    const D4 d4; const D2 d2;
    V4 rv = hn::CombineShiftRightBytes<8>(d4,v,v), lo = hn::Min(v,rv), hi = hn::Max(v,rv);
    return hn::Combine(d4, hn::UpperHalf(d2,hi), hn::LowerHalf(lo));
}
static inline V4 hwy_cas_desc_skip2(V4 v) {
    const D4 d4; const D2 d2;
    V4 rv = hn::CombineShiftRightBytes<8>(d4,v,v), lo = hn::Min(v,rv), hi = hn::Max(v,rv);
    return hn::Combine(d4, hn::UpperHalf(d2,lo), hn::LowerHalf(hi));
}

void sort_bitonic_highway(int* a) {
    const D4 d4;
    V4 v0=hn::LoadU(d4,a), v1=hn::LoadU(d4,a+4), v2=hn::LoadU(d4,a+8), v3=hn::LoadU(d4,a+12);

    v0=hwy_cas_mixed_adj(v0);  v1=hwy_cas_mixed_adj(v1);   // step 1
    v2=hwy_cas_mixed_adj(v2);  v3=hwy_cas_mixed_adj(v3);

    v0=hwy_cas_asc_skip2(v0);  v1=hwy_cas_desc_skip2(v1);  // step 2
    v2=hwy_cas_asc_skip2(v2);  v3=hwy_cas_desc_skip2(v3);

    v0=hwy_cas_asc_adj(v0);    v1=hwy_cas_desc_adj(v1);    // step 3
    v2=hwy_cas_asc_adj(v2);    v3=hwy_cas_desc_adj(v3);

    { V4 t=hn::Min(v0,v1); v1=hn::Max(v0,v1); v0=t; }      // step 4
    { V4 t=hn::Max(v2,v3); v3=hn::Min(v2,v3); v2=t; }

    v0=hwy_cas_asc_skip2(v0);  v1=hwy_cas_asc_skip2(v1);   // step 5
    v2=hwy_cas_desc_skip2(v2); v3=hwy_cas_desc_skip2(v3);

    v0=hwy_cas_asc_adj(v0);    v1=hwy_cas_asc_adj(v1);     // step 6
    v2=hwy_cas_desc_adj(v2);   v3=hwy_cas_desc_adj(v3);

    { V4 t=hn::Min(v0,v2); v2=hn::Max(v0,v2); v0=t; }      // step 7
    { V4 t=hn::Min(v1,v3); v3=hn::Max(v1,v3); v1=t; }

    { V4 t=hn::Min(v0,v1); v1=hn::Max(v0,v1); v0=t; }      // step 8
    { V4 t=hn::Min(v2,v3); v3=hn::Max(v2,v3); v2=t; }

    v0=hwy_cas_asc_skip2(v0);  v1=hwy_cas_asc_skip2(v1);   // step 9
    v2=hwy_cas_asc_skip2(v2);  v3=hwy_cas_asc_skip2(v3);

    v0=hwy_cas_asc_adj(v0);    v1=hwy_cas_asc_adj(v1);     // step 10
    v2=hwy_cas_asc_adj(v2);    v3=hwy_cas_asc_adj(v3);

    hn::StoreU(v0,d4,a); hn::StoreU(v1,d4,a+4);
    hn::StoreU(v2,d4,a+8); hn::StoreU(v3,d4,a+12);
}

void sort_bitonic_highway(vector<int>& v) { sort_bitonic_highway(v.data()); }

// ================================================================
// Benchmark harness
// ================================================================

template <typename SortFn>
double bench_ns(const vector<int>& data, SortFn fn) {
    int reps = max(1, 100000 / max(N, 1));
    vector<vector<int>> copies(RUNS, data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), copies[r].begin());
            fn(copies[r]);
        }
        auto t1 = high_resolution_clock::now();
        long long elapsed = duration_cast<nanoseconds>(t1 - t0).count();
        best = min(best, elapsed / reps);
    }
    return (double)best;
}

static string fmt_time(double ns) {
    if (ns < 1000.0) return to_string((int)ns) + " ns";
    return to_string((int)(ns/1e3)) + " µs";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // Correctness check — every implementation, 1000 random permutations
    {
        bool ok = true;
        mt19937 crng(7);
        for (int trial = 0; trial < 1000 && ok; trial++) {
            vector<int> v(N);
            for (int& x : v) x = dist(crng);
            vector<int> ref = v;
            sort(ref.begin(), ref.end());
            vector<int> t;
            t=v; sort_std(t);             ok &= (t==ref);
            t=v; sort_quicksort(t);       ok &= (t==ref);
            t=v; sort_insertion(t);       ok &= (t==ref);
            t=v; sort_bitonic_scalar(t);  ok &= (t==ref);
            t=v; sort_bitonic_avx128(t);  ok &= (t==ref);
            t=v; sort_bitonic_avx256(t);  ok &= (t==ref);
            t=v; sort_bitonic_highway(t); ok &= (t==ref);
        }
        cout << "Correctness: " << (ok ? "PASS" : "FAIL")
             << "  (HWY target: " << hwy::TargetName(HWY_TARGET) << ")\n\n";
    }

    vector<int> data(N);
    for (int& x : data) x = dist(rng);

    struct Entry { const char* name; double ns; };
    vector<Entry> results = {
        { "std::sort (introsort)",      bench_ns(data, sort_std)             },
        { "quicksort (median-of-3)",    bench_ns(data, sort_quicksort)       },
        { "insertion sort",             bench_ns(data, sort_insertion)        },
        { "bitonic scalar (80 CAS)",    bench_ns(data, [](vector<int>& v){ sort_bitonic_scalar(v); }) },
        { "bitonic AVX2 (4x128b)",      bench_ns(data, [](vector<int>& v){ sort_bitonic_avx128(v); }) },
        { "bitonic AVX2 (2x256b)",      bench_ns(data, [](vector<int>& v){ sort_bitonic_avx256(v); }) },
        { "bitonic Highway",            bench_ns(data, [](vector<int>& v){ sort_bitonic_highway(v);}) },
    };

    double qs_ns = results[1].ns;   // quicksort is the baseline

    cout << string(72, '-') << "\n";
    cout << "Sorting algorithms at n=16  (random int32, best of 7 trials)\n";
    cout << string(72, '-') << "\n";
    cout << left  << setw(30) << "algorithm"
         << right << setw(8)  << "time"
         << right << setw(12) << "ns/elem"
         << right << setw(14) << "vs quicksort" << "\n";
    cout << string(72, '-') << "\n";

    for (auto& e : results) {
        double speedup = qs_ns / e.ns;
        ostringstream su;
        su << fixed << setprecision(2);
        if (speedup >= 1.0) su << speedup << "x faster";
        else                su << (1.0/speedup) << "x slower";
        cout << left  << setw(30) << e.name
             << right << setw(8)  << fmt_time(e.ns)
             << right << setw(11) << fixed << setprecision(1) << e.ns / N
             << right << setw(16) << su.str() << "\n";
    }

    cout << string(72, '-') << "\n";
    cout << "Bitonic networks have no branches and no data-dependent control flow.\n";
    cout << "SIMD versions execute 4 (128b) or 8 (256b) comparators per instruction.\n";
    return 0;
}
