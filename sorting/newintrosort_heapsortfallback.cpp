// Introsort: quicksort (median-of-3) + SIMD bitonic base case + heapsort fallback
//
// Standard introsort structure:
//   - Recurse with quicksort (median-of-three pivot)
//   - Switch to bitonic_sort_simd for partitions of 16 or fewer elements
//   - Fall back to heapsort after 2*log2(n) recursion levels (worst-case guard)
//
// The heapsort fallback guarantees O(n log n) worst-case — see newintrosort.cpp
// for the version without it.
//
// Build:
//   g++ -O2 -o newintrosort_heapsortfallback newintrosort_heapsortfallback.cpp && ./newintrosort_heapsortfallback

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

static const int RUNS   = 7;
static const int CUTOFF = 16;

// ================================================================
// SIMD bitonic sort — exactly 16 int32 elements, 10 parallel steps
// (ARM NEON: 4 × int32x4_t replace one AVX-512 ZMM register)
// ================================================================

alignas(16) static const uint32_t MASK_MIXED [4] = {~0u, 0,   0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_A [4] = {~0u, 0,   ~0u, 0  };
alignas(16) static const uint32_t MASK_DESC_A[4] = {0,   ~0u, 0,   ~0u};
alignas(16) static const uint32_t MASK_ASC_S [4] = {~0u, ~0u, 0,   0  };
alignas(16) static const uint32_t MASK_DESC_S[4] = {0,   0,   ~0u, ~0u};

static inline int32x4_t cas_adj(int32x4_t v, uint32x4_t sel) {
    int32x4_t rv = vrev64q_s32(v);
    return vbslq_s32(sel, vminq_s32(v, rv), vmaxq_s32(v, rv));
}

static inline int32x4_t cas_skip2(int32x4_t v, uint32x4_t sel) {
    int32x4_t rv = vextq_s32(v, v, 2);
    return vbslq_s32(sel, vminq_s32(v, rv), vmaxq_s32(v, rv));
}

static void bitonic_sort_simd(int* a) {
    int32x4_t v0 = vld1q_s32(a),      v1 = vld1q_s32(a + 4);
    int32x4_t v2 = vld1q_s32(a + 8),  v3 = vld1q_s32(a + 12);

    uint32x4_t m_mixed  = vld1q_u32(MASK_MIXED);
    uint32x4_t m_asc_a  = vld1q_u32(MASK_ASC_A);
    uint32x4_t m_desc_a = vld1q_u32(MASK_DESC_A);
    uint32x4_t m_asc_s  = vld1q_u32(MASK_ASC_S);
    uint32x4_t m_desc_s = vld1q_u32(MASK_DESC_S);

    // Phase 1 (k=2)
    v0 = cas_adj(v0, m_mixed);  v1 = cas_adj(v1, m_mixed);
    v2 = cas_adj(v2, m_mixed);  v3 = cas_adj(v3, m_mixed);

    // Phase 2 (k=4)
    v0 = cas_skip2(v0, m_asc_s);  v1 = cas_skip2(v1, m_desc_s);
    v2 = cas_skip2(v2, m_asc_s);  v3 = cas_skip2(v3, m_desc_s);
    v0 = cas_adj(v0, m_asc_a);    v1 = cas_adj(v1, m_desc_a);
    v2 = cas_adj(v2, m_asc_a);    v3 = cas_adj(v3, m_desc_a);

    // Phase 3 (k=8)
    int32x4_t t;
    t = vminq_s32(v0, v1); v1 = vmaxq_s32(v0, v1); v0 = t;
    t = vmaxq_s32(v2, v3); v3 = vminq_s32(v2, v3); v2 = t;
    v0 = cas_skip2(v0, m_asc_s);  v1 = cas_skip2(v1, m_asc_s);
    v2 = cas_skip2(v2, m_desc_s); v3 = cas_skip2(v3, m_desc_s);
    v0 = cas_adj(v0, m_asc_a);    v1 = cas_adj(v1, m_asc_a);
    v2 = cas_adj(v2, m_desc_a);   v3 = cas_adj(v3, m_desc_a);

    // Phase 4 (k=16)
    t = vminq_s32(v0, v2); v2 = vmaxq_s32(v0, v2); v0 = t;
    t = vminq_s32(v1, v3); v3 = vmaxq_s32(v1, v3); v1 = t;
    t = vminq_s32(v0, v1); v1 = vmaxq_s32(v0, v1); v0 = t;
    t = vminq_s32(v2, v3); v3 = vmaxq_s32(v2, v3); v2 = t;
    v0 = cas_skip2(v0, m_asc_s);  v1 = cas_skip2(v1, m_asc_s);
    v2 = cas_skip2(v2, m_asc_s);  v3 = cas_skip2(v3, m_asc_s);
    v0 = cas_adj(v0, m_asc_a);    v1 = cas_adj(v1, m_asc_a);
    v2 = cas_adj(v2, m_asc_a);    v3 = cas_adj(v3, m_asc_a);

    vst1q_s32(a,      v0);  vst1q_s32(a + 4,  v1);
    vst1q_s32(a + 8,  v2);  vst1q_s32(a + 12, v3);
}

// Partition of n ≤ 16: pad scratch to 16 with INT_MAX, sort, copy n back.
// INT_MAX padding is safe because it sinks to the tail of the sorted result.
static void sort_small(int* a, int n) {
    if (n <= 1) return;
    alignas(16) int buf[16];
    for (int i = 0;  i < n;  i++) buf[i] = a[i];
    for (int i = n; i < 16; i++) buf[i] = INT_MAX;
    bitonic_sort_simd(buf);
    for (int i = 0; i < n; i++) a[i] = buf[i];
}

// ================================================================
// Heapsort — worst-case O(n log n) fallback
// ================================================================

static void sift_down(int* a, int i, int n) {
    while (true) {
        int largest = i, l = 2*i + 1, r = 2*i + 2;
        if (l < n && a[l] > a[largest]) largest = l;
        if (r < n && a[r] > a[largest]) largest = r;
        if (largest == i) break;
        swap(a[i], a[largest]);
        i = largest;
    }
}

static void heapsort(int* a, int n) {
    for (int i = n/2 - 1; i >= 0; i--) sift_down(a, i, n);
    for (int i = n - 1; i > 0; i--) { swap(a[0], a[i]); sift_down(a, 0, i); }
}

// ================================================================
// Quicksort with median-of-three pivot
// ================================================================

static int median3(int* a, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    if (a[lo] > a[mid]) swap(a[lo], a[mid]);
    if (a[lo] > a[hi])  swap(a[lo], a[hi]);
    if (a[mid] > a[hi]) swap(a[mid], a[hi]);
    swap(a[mid], a[hi - 1]);
    return a[hi - 1];
}

static void introsort_rec(int* a, int lo, int hi, int depth) {
    int n = hi - lo + 1;
    if (n <= CUTOFF) { sort_small(a + lo, n); return; }
    if (depth == 0)  { heapsort(a + lo, n);   return; }

    int pivot = median3(a, lo, hi);
    int i = lo, j = hi - 1;
    for (;;) {
        while (a[++i] < pivot) {}
        while (a[--j] > pivot) {}
        if (i >= j) break;
        swap(a[i], a[j]);
    }
    swap(a[i], a[hi - 1]);
    introsort_rec(a, lo,    i - 1, depth - 1);
    introsort_rec(a, i + 1, hi,    depth - 1);
}

void introsort(vector<int>& v) {
    int n = (int)v.size();
    if (n > 1)
        introsort_rec(v.data(), 0, n - 1, 2 * (int)log2((double)n));
}

// ================================================================
// Reference algorithms (same data, fair comparison)
// ================================================================

static void median3_qs_rec(int* a, int lo, int hi) {
    if (hi <= lo) return;
    if (hi - lo == 1) { if (a[lo] > a[hi]) swap(a[lo], a[hi]); return; }
    int pivot = median3(a, lo, hi);
    int i = lo, j = hi - 1;
    for (;;) {
        while (a[++i] < pivot) {}
        while (a[--j] > pivot) {}
        if (i >= j) break;
        swap(a[i], a[j]);
    }
    swap(a[i], a[hi - 1]);
    median3_qs_rec(a, lo, i - 1);
    median3_qs_rec(a, i + 1, hi);
}

static void quicksort(vector<int>& v) {
    if (v.size() > 1) median3_qs_rec(v.data(), 0, (int)v.size() - 1);
}

static void std_sort(vector<int>& v) { sort(v.begin(), v.end()); }

// ================================================================
// Benchmarking
// ================================================================

static long long bench_ns(const vector<int>& data, void (*fn)(vector<int>&)) {
    int n = data.size();
    int reps = max(1, 100000 / max(n, 1));
    vector<int> buf(data);
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        auto t0 = high_resolution_clock::now();
        for (int k = 0; k < reps; k++) {
            copy(data.begin(), data.end(), buf.begin());
            fn(buf);
        }
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<nanoseconds>(t1 - t0).count() / reps);
    }
    return best;
}

static string fmt_time(long long ns) {
    if (ns < 1000LL)       return to_string(ns)              + " ns";
    if (ns < 1000000LL)    return to_string(ns / 1000)       + " µs";
    if (ns < 1000000000LL) return to_string(ns / 1000000)    + " ms";
    return                  to_string(ns / 1000000000LL)      + " s";
}

int main() {
    mt19937 rng(42);
    uniform_int_distribution<int> dist(INT_MIN, INT_MAX);

    // Correctness
    {
        auto check = [](const char* label, void (*fn)(vector<int>&)) {
            vector<int> v = {9,3,7,1,5,15,11,13,2,14,6,10,4,8,12,0};
            vector<int> ref = v;
            sort(ref.begin(), ref.end());
            fn(v);
            cout << label << ": " << (v == ref ? "PASS" : "FAIL") << "\n";
        };
        check("introsort n=16", introsort);

        // larger random check
        vector<int> big(100000);
        for (int& x : big) x = dist(rng);
        vector<int> ref = big;
        sort(ref.begin(), ref.end());
        introsort(big);
        cout << "introsort n=100000: " << (big == ref ? "PASS" : "FAIL") << "\n\n";
    }

    // 2^10 to 2^20
    vector<int> sizes;
    for (int e = 10; e <= 20; e++) sizes.push_back(1 << e);

    const int W = 14;
    cout << string(84, '-') << "\n";
    cout << "introsort (qs + SIMD bitonic ≤16 + heapsort fallback) vs quicksort vs std::sort\n";
    cout << string(84, '-') << "\n";
    cout << right
         << setw(10) << "n"
         << setw(W)  << "introsort"
         << setw(W)  << "quicksort"
         << setw(W)  << "std::sort"
         << setw(12) << "vs qs"
         << setw(10) << "vs std"
         << "\n";
    cout << string(84, '-') << "\n";

    for (int n : sizes) {
        vector<int> data(n);
        for (int& x : data) x = dist(rng);

        long long is_ns  = bench_ns(data, introsort);
        long long qs_ns  = bench_ns(data, quicksort);
        long long std_ns = bench_ns(data, std_sort);

        double r_qs  = (double)is_ns / (double)qs_ns;
        double r_std = (double)is_ns / (double)std_ns;

        cout << right
             << setw(10) << n
             << setw(W)  << fmt_time(is_ns)
             << setw(W)  << fmt_time(qs_ns)
             << setw(W)  << fmt_time(std_ns)
             << setw(11) << fixed << setprecision(2) << r_qs  << "x"
             << setw(9)  << fixed << setprecision(2) << r_std << "x"
             << "\n";
    }

    cout << string(84, '-') << "\n";
    cout << "ratio < 1.0 → introsort faster; ratio > 1.0 → introsort slower\n";
    return 0;
}
