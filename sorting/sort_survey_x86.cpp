// Sorting survey on x86-64 (AVX2) — every number in full_sorting_ryzen.md
//
// One program, one measurement methodology, covering all three size regimes:
//   n = 16          bitonic networks vs the scalar field
//   n = 32 .. 16k   introsort with AVX2 bitonic leaves vs std::sort
//   n >= 32k        vqs (CompressStore partition) vs std::sort
//
// MEASUREMENT.  Timing `copy(src -> buf); sort(buf)` in a loop over one fixed
// src array makes every data-dependent branch perfectly predictable after
// warm-up.  Below n ~ 4096 the whole branch history fits the predictor and
// std::sort measures 28 us against a true 260 us — a 9x error that decays as n
// grows, producing a fake discontinuity.  Comparing a branchless SIMD sort to a
// branchy comparison sort that way is not a fair test.  Instead: pre-build K
// independent random buffers, refill them OUTSIDE the timed region, and time
// sorting all K.
//
// Build:
//   g++ -O2 -std=c++17 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul \
//     -o sort_survey_x86 sort_survey_x86.cpp -lhwy && ./sort_survey_x86

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "hwy/highway.h"

using namespace std;
using namespace std::chrono;
namespace hn = hwy::HWY_NAMESPACE;

static constexpr int RUNS   = 7;
static constexpr int CUTOFF = 16;

// ================================================================
// bitonic16 — AVX2, 2 x __m256i.  Standard bitonic recurrence: stages
// k = 2,4,8,16, passes at distance d = k/2 … 1; element g keeps the min when
// ((g & d) == 0) XOR ((g & k) != 0).  That predicate becomes one 8-bit blend
// immediate per register per step.  Distance 8 is the only cross-register
// step and lands in the final all-ascending stage, so it is a bare min/max.
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
    v0 = y_cas<0x99>(v0, y_rot1(v0)); v1 = y_cas<0x99>(v1, y_rot1(v1));
    v0 = y_cas<0xC3>(v0, y_rot2(v0)); v1 = y_cas<0xC3>(v1, y_rot2(v1));
    v0 = y_cas<0xA5>(v0, y_rot1(v0)); v1 = y_cas<0xA5>(v1, y_rot1(v1));
    v0 = y_cas<0x0F>(v0, y_rot4(v0)); v1 = y_cas<0xF0>(v1, y_rot4(v1));
    v0 = y_cas<0x33>(v0, y_rot2(v0)); v1 = y_cas<0xCC>(v1, y_rot2(v1));
    v0 = y_cas<0x55>(v0, y_rot1(v0)); v1 = y_cas<0xAA>(v1, y_rot1(v1));
    { __m256i lo = _mm256_min_epi32(v0, v1); v1 = _mm256_max_epi32(v0, v1); v0 = lo; }
    v0 = y_cas<0x0F>(v0, y_rot4(v0)); v1 = y_cas<0x0F>(v1, y_rot4(v1));
    v0 = y_cas<0x33>(v0, y_rot2(v0)); v1 = y_cas<0x33>(v1, y_rot2(v1));
    v0 = y_cas<0x55>(v0, y_rot1(v0)); v1 = y_cas<0x55>(v1, y_rot1(v1));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(a),     v0);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(a + 8), v1);
}

// bitonic16 via portable Highway, pinned to 128-bit by the network's structure
using D4 = hn::FixedTag<int32_t,4>;  using D2 = hn::Half<D4>;  using V4 = hn::Vec<D4>;
static inline V4 h_asc_adj(V4 v){ const D4 d; V4 r=hn::Reverse2(d,v); return hn::OddEven(hn::Max(v,r),hn::Min(v,r)); }
static inline V4 h_desc_adj(V4 v){ const D4 d; V4 r=hn::Reverse2(d,v); return hn::OddEven(hn::Min(v,r),hn::Max(v,r)); }
static inline V4 h_mixed_adj(V4 v){ const D4 d; const D2 d2; V4 r=hn::Reverse2(d,v),lo=hn::Min(v,r),hi=hn::Max(v,r);
    return hn::Combine(d, hn::UpperHalf(d2,hn::OddEven(lo,hi)), hn::LowerHalf(hn::OddEven(hi,lo))); }
static inline V4 h_asc_s2(V4 v){ const D4 d; const D2 d2; V4 r=hn::CombineShiftRightBytes<8>(d,v,v);
    return hn::Combine(d, hn::UpperHalf(d2,hn::Max(v,r)), hn::LowerHalf(hn::Min(v,r))); }
static inline V4 h_desc_s2(V4 v){ const D4 d; const D2 d2; V4 r=hn::CombineShiftRightBytes<8>(d,v,v);
    return hn::Combine(d, hn::UpperHalf(d2,hn::Min(v,r)), hn::LowerHalf(hn::Max(v,r))); }
static void bitonic16_hwy(int* a){
    const D4 d; V4 v0=hn::LoadU(d,a),v1=hn::LoadU(d,a+4),v2=hn::LoadU(d,a+8),v3=hn::LoadU(d,a+12);
    v0=h_mixed_adj(v0); v1=h_mixed_adj(v1); v2=h_mixed_adj(v2); v3=h_mixed_adj(v3);
    v0=h_asc_s2(v0); v1=h_desc_s2(v1); v2=h_asc_s2(v2); v3=h_desc_s2(v3);
    v0=h_asc_adj(v0); v1=h_desc_adj(v1); v2=h_asc_adj(v2); v3=h_desc_adj(v3);
    { V4 t=hn::Min(v0,v1); v1=hn::Max(v0,v1); v0=t; } { V4 t=hn::Max(v2,v3); v3=hn::Min(v2,v3); v2=t; }
    v0=h_asc_s2(v0); v1=h_asc_s2(v1); v2=h_desc_s2(v2); v3=h_desc_s2(v3);
    v0=h_asc_adj(v0); v1=h_asc_adj(v1); v2=h_desc_adj(v2); v3=h_desc_adj(v3);
    { V4 t=hn::Min(v0,v2); v2=hn::Max(v0,v2); v0=t; } { V4 t=hn::Min(v1,v3); v3=hn::Max(v1,v3); v1=t; }
    { V4 t=hn::Min(v0,v1); v1=hn::Max(v0,v1); v0=t; } { V4 t=hn::Min(v2,v3); v3=hn::Max(v2,v3); v2=t; }
    v0=h_asc_s2(v0); v1=h_asc_s2(v1); v2=h_asc_s2(v2); v3=h_asc_s2(v3);
    v0=h_asc_adj(v0); v1=h_asc_adj(v1); v2=h_asc_adj(v2); v3=h_asc_adj(v3);
    hn::StoreU(v0,d,a); hn::StoreU(v1,d,a+4); hn::StoreU(v2,d,a+8); hn::StoreU(v3,d,a+12);
}

// ================================================================
// Scalar reference sorts
// ================================================================
static void insertion_sort(int* a, int n) {
    for (int i = 1; i < n; i++) { int k=a[i], j=i-1; while (j>=0 && a[j]>k) { a[j+1]=a[j]; j--; } a[j+1]=k; }
}
static void sift_down(int* a, int i, int n) {
    for (;;) { int m=i,l=2*i+1,r=2*i+2;
        if (l<n && a[l]>a[m]) m=l;
        if (r<n && a[r]>a[m]) m=r;
        if (m==i) break; swap(a[i],a[m]); i=m; }
}
static void heapsort(int* a, int n) {
    for (int i=n/2-1;i>=0;i--) sift_down(a,i,n);
    for (int i=n-1;i>0;i--){ swap(a[0],a[i]); sift_down(a,0,i); }
}
static int median3(int* a, int lo, int hi) {
    int mid = lo + (hi-lo)/2;
    if (a[lo]>a[mid]) swap(a[lo],a[mid]);
    if (a[lo]>a[hi])  swap(a[lo],a[hi]);
    if (a[mid]>a[hi]) swap(a[mid],a[hi]);
    swap(a[mid],a[hi-1]); return a[hi-1];
}

// ================================================================
// introsort — Hoare quicksort, AVX2 bitonic leaves, heapsort fallback
// ================================================================
static void pad_sort_small(int* a, int n) {
    if (n <= 1) return;
    alignas(32) int buf[16];
    for (int i=0;i<n;i++) buf[i]=a[i];
    for (int i=n;i<16;i++) buf[i]=INT_MAX;
    bitonic16(buf);
    for (int i=0;i<n;i++) a[i]=buf[i];
}
static void introsort_rec(int* a, int lo, int hi, int depth) {
    int n = hi-lo+1;
    if (n <= CUTOFF) { pad_sort_small(a+lo,n); return; }
    if (depth == 0)  { heapsort(a+lo,n); return; }
    int pivot = median3(a,lo,hi);
    int i=lo, j=hi-1;
    for (;;) { while (a[++i]<pivot){} while (a[--j]>pivot){} if (i>=j) break; swap(a[i],a[j]); }
    swap(a[i],a[hi-1]);
    introsort_rec(a,lo,i-1,depth-1);
    introsort_rec(a,i+1,hi,depth-1);
}
static void introsort(vector<int>& v) {
    int n=(int)v.size();
    if (n>1) introsort_rec(v.data(),0,n-1,2*(int)log2((double)n));
}

// ================================================================
// vqs — CompressStore partition (Bramas Algorithm 3/4)
// ================================================================
static void vqs_small(int* a, int n) { if (n==16) bitonic16(a); else insertion_sort(a,n); }

static int simd_partition(int* a, int lo, int hi, int pivot, int* lb, int* rb) {
    const HWY_FULL(int32_t) d;
    const int N = (int)hn::Lanes(d);
    const auto pv = hn::Set(d, pivot);
    int lc=0, rc=0;
    const int vend = lo + ((hi-lo)/N)*N;
    for (int i=lo;i<vend;i+=N) {
        const auto v = hn::LoadU(d, a+i);
        lc += (int)hn::CompressStore(v, hn::Lt(v,pv), d, lb+lc);
        rc += (int)hn::CompressStore(v, hn::Ge(v,pv), d, rb+rc);
    }
    for (int i=vend;i<hi;i++) { if (a[i]<pivot) lb[lc++]=a[i]; else rb[rc++]=a[i]; }
    memcpy(a+lo,    lb, lc*sizeof(int));
    memcpy(a+lo+lc, rb, rc*sizeof(int));
    return lc;
}
static void vqs_core(int* a, int lo, int hi, int* lb, int* rb) {
    if (hi-lo+1 <= CUTOFF) { vqs_small(a+lo, hi-lo+1); return; }
    int mid = lo + (hi-lo)/2;
    if (a[lo]>a[mid]) swap(a[lo],a[mid]);
    if (a[lo]>a[hi])  swap(a[lo],a[hi]);
    if (a[mid]>a[hi]) swap(a[mid],a[hi]);
    swap(a[mid],a[hi]);
    const int pivot=a[hi];
    const int lc = simd_partition(a,lo,hi,pivot,lb,rb);
    swap(a[lo+lc],a[hi]);
    const int p=lo+lc;
    vqs_core(a,lo,p-1,lb,rb);
    vqs_core(a,p+1,hi,lb,rb);
}
static void vqs_sort(vector<int>& v) {
    const int n=(int)v.size();
    if (n<=CUTOFF) { vqs_small(v.data(),n); return; }
    vector<int> lb(n), rb(n);
    vqs_core(v.data(),0,n-1,lb.data(),rb.data());
}

// ================================================================
// Harness — independent buffers, refilled outside the timed region
// ================================================================
template <typename Fn>
static double bench(int n, mt19937& rng, Fn fn) {
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
    const size_t cap = (8u<<20) / (n*sizeof(int));
    const size_t K   = max<size_t>(3, min<size_t>(256, cap));
    vector<vector<int>> bufs(K, vector<int>(n));
    long long best = LLONG_MAX;
    for (int r=0;r<RUNS;r++) {
        for (auto& b : bufs) for (int& x : b) x = dist(rng);
        auto t0 = high_resolution_clock::now();
        for (auto& b : bufs) fn(b);
        auto t1 = high_resolution_clock::now();
        best = min<long long>(best, duration_cast<nanoseconds>(t1-t0).count()/(long long)K);
    }
    return (double)best;
}
static string fmt(double ns) {
    char b[32];
    if (ns<1e3) snprintf(b,sizeof b,"%.0f ns",ns);
    else if (ns<1e6) snprintf(b,sizeof b,"%.1f us",ns/1e3);
    else if (ns<1e9) snprintf(b,sizeof b,"%.2f ms",ns/1e6);
    else snprintf(b,sizeof b,"%.2f s",ns/1e9);
    return b;
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // ---- correctness ----
    {
        bool ok=true; mt19937 r(7);
        for (int trial=0; trial<400 && ok; trial++) {
            int n = 1 + (int)(r() % 5000);
            vector<int> v(n); for (int& x:v) x=dist(r);
            vector<int> ref=v; sort(ref.begin(),ref.end());
            vector<int> t;
            t=v; introsort(t); ok &= (t==ref);
            t=v; vqs_sort(t);  ok &= (t==ref);
            if (n>=16) { t.assign(v.begin(),v.begin()+16); vector<int> r16=t;
                sort(r16.begin(),r16.end());
                vector<int> u=t; bitonic16(u.data());      ok &= (u==r16);
                u=t; bitonic16_hwy(u.data());              ok &= (u==r16); }
        }
        printf("Correctness: %s   HWY target: %s\n\n", ok?"PASS":"FAIL", hwy::TargetName(HWY_TARGET));
    }

    // ---- n = 16 field ----
    printf("n = 16, isolated\n%s\n", string(58,'-').c_str());
    printf("%-26s %10s %10s\n","method","time","vs std");
    printf("%s\n", string(58,'-').c_str());
    {
        double s = bench(16,rng,[](vector<int>& v){ sort(v.begin(),v.end()); });
        struct E { const char* nm; double ns; };
        vector<E> es = {
          {"bitonic AVX2 (2x256b)", bench(16,rng,[](vector<int>& v){ bitonic16(v.data()); })},
          {"bitonic Highway",       bench(16,rng,[](vector<int>& v){ bitonic16_hwy(v.data()); })},
          {"insertion sort",        bench(16,rng,[](vector<int>& v){ insertion_sort(v.data(),16); })},
          {"heapsort",              bench(16,rng,[](vector<int>& v){ heapsort(v.data(),16); })},
          {"std::sort",             s},
        };
        for (auto& e : es)
            printf("%-26s %10s %9.2fx\n", e.nm, fmt(e.ns).c_str(), s/e.ns);
    }

    // ---- sweep ----
    printf("\n%s\n","n sweep: introsort (AVX2 leaves) and vqs vs std::sort");
    printf("%s\n", string(78,'-').c_str());
    printf("%10s %11s %11s %11s %9s %9s\n","n","introsort","vqs","std::sort","intro/std","vqs/std");
    printf("%s\n", string(78,'-').c_str());
    for (int e=5;e<=20;e++) {
        const int n = 1<<e;
        const double ni = bench(n,rng,[](vector<int>& v){ introsort(v); });
        const double nv = bench(n,rng,[](vector<int>& v){ vqs_sort(v); });
        const double ns = bench(n,rng,[](vector<int>& v){ sort(v.begin(),v.end()); });
        printf("%10d %11s %11s %11s %8.2fx %8.2fx\n",
               n, fmt(ni).c_str(), fmt(nv).c_str(), fmt(ns).c_str(), ns/ni, ns/nv);
    }
    printf("%s\n", string(78,'-').c_str());
    { const HWY_FULL(int32_t) d;
      printf("CUTOFF=%d  VEC_LANES=%d  target=%s\n", CUTOFF, (int)hn::Lanes(d), hwy::TargetName(HWY_TARGET)); }
    return 0;
}
