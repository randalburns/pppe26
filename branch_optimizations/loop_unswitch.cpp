// Loop Unswitching: Hoisting a Loop-Invariant Branch Outside the Loop
//
// A loop-invariant branch is one whose condition does not change during
// the loop — it evaluates to the same outcome on every iteration.  Leaving
// it inside the loop has two costs:
//
//   1. Redundant work: the condition is tested N times instead of once.
//
//   2. Blocked vectorization: the presence of a branch inside the loop body
//      prevents the compiler from generating SIMD vector instructions, even
//      though both paths are independent of the loop variable.
//
// Loop unswitching moves the branch outside:
//
//   Before:
//     for (int i = 0; i < n; i++) {
//         if (mode == 0) c[i] = a[i] + b[i];   // checked every iteration
//         else           c[i] = a[i] * b[i];
//     }
//
//   After:
//     if (mode == 0) {
//         for (int i = 0; i < n; i++) c[i] = a[i] + b[i];  // clean, vectorizes
//     } else {
//         for (int i = 0; i < n; i++) c[i] = a[i] * b[i];  // clean, vectorizes
//     }
//
// Each version of the unswitched inner loop is a single, branch-free
// statement — the compiler can apply SIMD vectorization, unrolling, and
// scheduling optimizations that are blocked by the presence of the branch.
//
// This example demonstrates three element-wise operations on two arrays:
//
//   Mode 0: c[i] = a[i] + b[i]              (add)
//   Mode 1: c[i] = a[i] * b[i]              (multiply)
//   Mode 2: c[i] = |a[i]| + |b[i]|          (sum of absolutes)
//
// The mode is a function argument — fixed for the duration of the call.
// Assembly at -O2 confirms the difference:
//
//   switched   → scalar registers (s0..s31): no vectorization
//   unswitched → SIMD registers  (v0..v31, .4s lanes): 4× throughput
//
// The compiler performs loop unswitching automatically at -O3.  At -O2
// it does not, making the manual transformation necessary — and making
// the benefit visible and measurable.
//
// Build:
//   g++ -O2 -o loop_unswitch loop_unswitch.cpp && ./loop_unswitch

#include <iostream>
#include <chrono>
#include <climits>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

static const int N    = 32 * 1024 * 1024;  // 32M floats = 128 MB per array
static const int RUNS = 5;
static const double CPU_GHZ = 4.0;

template <typename T>
static void sink(T const& val) { asm volatile("" : : "m"(val) : "memory"); }

template <typename Func>
long long bench(Func f) {
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        asm volatile("" ::: "memory");
        auto t0 = high_resolution_clock::now();
        f();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

// ================================================================
// Switched: mode branch inside the loop
//
// At -O2, the compiler cannot vectorize because it cannot prove the
// branch is loop-invariant (or more precisely, the multi-way branch
// disrupts the straight-line code structure that SIMD requires).
// Each iteration evaluates two comparisons and takes a branch.
//
// Assembly (-O2, scalar):
//   .L_loop:
//     ldr  s2, [x0], #4     ; load a[i]
//     ldr  s3, [x1], #4     ; load b[i]
//     cmp  w3, #0            ; mode == 0?
//     beq  .L_add
//     cmp  w3, #1            ; mode == 1?
//     beq  .L_mul
//     fabs s2, s2            ; mode == 2: |a[i]|
//     fabs s3, s3            ;            |b[i]|
//     fadd s2, s2, s3
//     b    .L_store
//   .L_add:  fadd  s2, s2, s3
//   .L_store: str  s2, [x2], #4
//     subs  x4, x4, #1
//     b.ne  .L_loop
// ================================================================

void apply_switched(const float* a, const float* b, float* c, int n, int mode) {
    for (int i = 0; i < n; i++) {
        if      (mode == 0) c[i] = a[i] + b[i];
        else if (mode == 1) c[i] = a[i] * b[i];
        else                c[i] = fabsf(a[i]) + fabsf(b[i]);
    }
}

// ================================================================
// Unswitched: mode branch outside the loop
//
// Each inner loop is a single straight-line statement with no branches.
// At -O2, the compiler vectorizes with 4-wide SIMD (128-bit NEON):
//
// Assembly (-O2, SIMD):
//   .L_loop:
//     ldr  q0, [x0], #16    ; load 4 floats from a
//     ldr  q1, [x1], #16    ; load 4 floats from b
//     fadd.4s  v0, v0, v1   ; add 4 pairs simultaneously
//     str  q0, [x2], #16    ; store 4 results
//     subs x4, x4, #4
//     b.ne .L_loop
//
// Four elements per iteration instead of one: 4× the throughput.
// ================================================================

void apply_unswitched(const float* a, const float* b, float* c, int n, int mode) {
    if (mode == 0)
        for (int i = 0; i < n; i++) c[i] = a[i] + b[i];
    else if (mode == 1)
        for (int i = 0; i < n; i++) c[i] = a[i] * b[i];
    else
        for (int i = 0; i < n; i++) c[i] = fabsf(a[i]) + fabsf(b[i]);
}

// ================================================================
// main
// ================================================================

int main() {
    vector<float> a(N), b(N), c1(N), c2(N);
    for (int i = 0; i < N; i++) {
        a[i] =  (i % 1000) * 0.001f - 0.5f;   // values in [-0.5, 0.5)
        b[i] = -(i % 997)  * 0.001f + 0.3f;
    }

    // Correctness check for all three modes
    bool ok = true;
    for (int mode = 0; mode <= 2; mode++) {
        apply_switched  (a.data(), b.data(), c1.data(), N, mode);
        apply_unswitched(a.data(), b.data(), c2.data(), N, mode);
        for (int i = 0; i < N; i++) {
            if (c1[i] != c2[i]) { ok = false; break; }
        }
    }
    cout << "Correctness: " << (ok ? "PASS" : "FAIL") << "\n";

    long long mb = 3LL * N * sizeof(float) / (1024 * 1024);
    cout << "Arrays: 3 × " << N/(1024*1024) << "M floats  (" << mb << " MB)\n";

    auto cpe = [&](long long ms) {
        return (double)ms * CPU_GHZ * 1e6 / (double)N;
    };

    cout << "\n" << string(72, '-') << "\n";
    cout << "apply(a, b, c, N, mode)   N=32M floats   -O2   CPU=" << CPU_GHZ << " GHz\n";
    cout << string(72, '-') << "\n";
    cout << left  << setw(12) << "mode"
         << right << setw(14) << "switched"
         << right << setw(14) << "unswitched"
         << right << setw(8)  << "cpe(sw)"
         << right << setw(10) << "cpe(unsw)"
         << right << setw(10) << "speedup" << "\n";
    cout << string(72, '-') << "\n";

    const char* mode_names[3] = { "0 (add)", "1 (mul)", "2 (abssum)" };
    for (int mode = 0; mode <= 2; mode++) {
        auto ts = bench([&]{ apply_switched  (a.data(), b.data(), c1.data(), N, mode); sink(c1[0]); });
        auto tu = bench([&]{ apply_unswitched(a.data(), b.data(), c2.data(), N, mode); sink(c2[0]); });
        cout << left  << setw(12) << mode_names[mode]
             << right << setw(12) << ts << " ms"
             << right << setw(12) << tu << " ms"
             << right << setw(8)  << fixed << setprecision(2) << cpe(ts)
             << right << setw(10) << fixed << setprecision(2) << cpe(tu)
             << right << setw(9)  << fixed << setprecision(2)
             << (double)ts / max(1LL, tu) << "x\n";
    }

    cout << "\nWhy unswitched is faster at -O2:\n"
            "  switched:   branch inside loop → compiler keeps scalar code (1 element/cycle)\n"
            "  unswitched: branch outside loop → compiler vectorizes with SIMD (4 elements/cycle)\n"
            "\n"
            "Note: at -O3 the compiler performs loop unswitching automatically,\n"
            "      closing the gap.  At -O2 the manual transformation is required.\n";

    return 0;
}
