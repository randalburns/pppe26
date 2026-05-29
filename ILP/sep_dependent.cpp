// Separating Dependent Instructions: Hiding Latency with Instruction Scheduling
//
// When instruction B reads a register written by instruction A, the pipeline
// must wait for A's result — a read-after-write (RAW) hazard.  The fix is to
// insert independent instructions between A and B, filling the pipeline while
// A's result propagates through the functional unit.
//
// This example computes two reductions over the same array:
//
//   sum   = Σ x[i]         (one FADD per element, loop-carried dep on sum)
//   sumsq = Σ x[i]²        (one FMUL + one FADD, loop-carried dep on sumsq)
//
// In separate loops the dependency chains run back-to-back:
//
//   [sum loop]    fadd sum sum x[0]   ← stalls here for 3 cycles
//                 fadd sum sum x[1]
//                 ...
//   [sumsq loop]  fmul t   x[0] x[0]  ← waits for sum loop to finish
//                 fadd sumsq sumsq t
//                 ...
//   Total ≈ 2 × N × FADD_latency cycles
//
// In a fused loop both chains run in the SAME loop body.  Because sum and sumsq
// carry no dependency between them, the two FADD chains are independent — and the
// FMUL for sumsq fills the stall slot between consecutive sum updates:
//
//   fadd sum   sum   x[i]     ← dep on previous sum   (stall 3 cycles)
//   fmul t     x[i]  x[i]    ← INDEPENDENT; fills 1 stall cycle
//   fadd sumsq sumsq t        ← dep on previous sumsq (stall 3 cycles)
//   fadd sum   sum   x[i+1]  ← sum dep resolved while sumsq was computing
//   ...
//
// The dependent sum instructions are "separated" by the independent sumsq work,
// and vice versa.  Each chain runs at the throughput limit of one FADD per cycle
// while the other fills the gap.  Total ≈ N × FADD_latency cycles — a 2× win.
//
// A fused loop with multiple accumulators per chain combines this with the
// multiple-accumulator technique from out_of_order.cpp, removing the remaining
// loop-carried bottleneck.
//
// The OOO engine CAN execute independent instructions out of order within its
// reorder buffer (ROB), but it CANNOT look past the end of a completed loop to
// start the next one.  The loop boundary is an ordering barrier.  Fusing the
// loops eliminates that barrier and exposes the inter-chain independence to the
// hardware scheduler.
//
// Build:
//   g++ -O1 -o sep_O1 sep_dependent.cpp && ./sep_O1
//
// Use -O1: -O2 and above may auto-vectorize, masking the scalar ILP effect.

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace std::chrono;

static const int N    = 64 * 1024 * 1024;  // 64M floats = 256 MB
static const int RUNS = 5;

// Apple M-series: FADD float latency = 3 cycles, 4 FP execution units.
// x86-64 (Skylake): ADDSS latency = 4 cycles.
static const int FADD_LATENCY = 3;

// CPU frequency (Apple M4 performance core).
// Adjust: M1/M2 ≈ 3.2 GHz, M3 ≈ 3.7 GHz, M4 ≈ 4.0 GHz.
static const double CPU_GHZ = 4.0;

// Instructions per element (approximate, from -O1 ARM64 assembly):
//   two_passes:    ldr, fadd, loop / 1   → 3 per chain, ~6 total / elem
//   fused_1acc:    ldr, fadd, fmul, fadd, loop / 1   → ~6 / elem
//   fused_2acc:    ldp, fadd×2, fmul×2, fadd×2, loop / 2 → ~5 / elem
static const double INSNS[3] = { 6.0, 6.0, 5.0 };

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
// Two separate loops — sequential, no instruction-level overlap
//
// The sum loop carries a dependency on s through every iteration:
//   fadd s, s, x[0]    ← issues; s not ready for 3 cycles
//   fadd s, s, x[1]    ← must wait; stalls 2 cycles each iteration
//
// The sumsq loop cannot begin until the sum loop fully completes.
// The OOO engine's reorder buffer cannot look past the barrier
// between the two loops.  Total ≈ 2 × N × FADD_latency cycles.
// ================================================================

void two_passes(const float* x, int n, float& s_out, float& ss_out) {
    float s = 0.0f;
    for (int i = 0; i < n; i++)
        s += x[i];

    float ss = 0.0f;
    for (int i = 0; i < n; i++)
        ss += x[i] * x[i];

    s_out = s;  ss_out = ss;
}

// ================================================================
// Fused loop — one pass, both chains interleaved
//
// s and ss carry no dependency between them.  The instruction sequence
// for one iteration at -O1 looks like:
//
//   ldr  s2, [x0]            ; load x[i]
//   fadd s0, s0, s2          ; sum  += x[i]    ← dep on previous s0
//   fmul s3, s2,  s2         ; x[i]*x[i]       ← INDEPENDENT (fills gap)
//   fadd s1, s1,  s3         ; sumsq += x[i]²  ← dep on previous s1
//
// The fmul fills 1 of the 3 stall cycles between s0[i] and s0[i+1].
// The sumsq fadd fills 1 more.  By the time the loop returns to
// update s0 again, 2 independent instructions have issued — reducing
// effective stall from 2 cycles to ~0 cycles on an OOO core.
// Total ≈ N × FADD_latency cycles — ~2× faster than two_passes.
// ================================================================

void fused_1acc(const float* x, int n, float& s_out, float& ss_out) {
    float s = 0.0f, ss = 0.0f;
    for (int i = 0; i < n; i++) {
        s  += x[i];
        ss += x[i] * x[i];
    }
    s_out = s;  ss_out = ss;
}

// ================================================================
// Fused loop with 2 accumulators per chain
//
// Combines instruction scheduling (fusing) with the multiple-accumulator
// technique from out_of_order.cpp.  Two accumulators per chain double the
// number of independent instructions between each loop-carried fadd,
// matching FADD_latency = 3 exactly and eliminating all stalls.
//
//   fadd s0, s0, x[i]          \
//   fadd s1, s1, x[i+1]        |  two independent sum updates
//   fmul t0, x[i],   x[i]      |  four independent ops between s0[i]
//   fmul t1, x[i+1], x[i+1]   |  and s0[i+2]: latency fully hidden
//   fadd q0, q0, t0            |
//   fadd q1, q1, t1            /
// ================================================================

void fused_2acc(const float* x, int n, float& s_out, float& ss_out) {
    float s0 = 0.0f, s1 = 0.0f;
    float q0 = 0.0f, q1 = 0.0f;
    int n2 = n & ~1;
    for (int i = 0; i < n2; i += 2) {
        s0 += x[i];
        s1 += x[i+1];
        q0 += x[i]   * x[i];
        q1 += x[i+1] * x[i+1];
    }
    if (n & 1) { s0 += x[n-1]; q0 += x[n-1] * x[n-1]; }
    s_out  = s0 + s1;
    ss_out = q0 + q1;
}

// ================================================================
// main
// ================================================================

int main() {
    vector<float> x(N);
    for (int i = 0; i < N; i++)
        x[i] = (i % 1000) * 0.001f + 0.5f;

    float s1, ss1, s2, ss2, s3, ss3;

    // Correctness check on a small slice (float precision is lost at large N:
    // when sum ≈ 64M the float ULP is ~8, swamping individual additions of ~1.0).
    static const int NC = 10000;
    float cs1, css1, cs2, css2, cs3, css3;
    two_passes (x.data(), NC, cs1, css1);
    fused_1acc (x.data(), NC, cs2, css2);
    fused_2acc (x.data(), NC, cs3, css3);
    float es  = max(abs(cs1-cs2),  abs(cs1-cs3));
    float eq  = max(abs(css1-css2), abs(css1-css3));
    float res = abs(cs1)  > 0 ? es  / abs(cs1)  : es;
    float req = abs(css1) > 0 ? eq  / abs(css1) : eq;
    cout << "Correctness (N=" << NC << "): rel_sum_err=" << res << "  rel_sumsq_err=" << req
         << "  " << (res < 0.001f && req < 0.001f ? "PASS" : "FAIL") << "\n";

    long long mb = (long long)N * sizeof(float) / (1024*1024);
    cout << "\nArray: " << N/(1024*1024) << "M floats  (" << mb << " MB)\n";
    cout << "FADD latency (Apple M-series): " << FADD_LATENCY
         << " cycles  →  both chains fill each other's stall slots when fused\n";

    auto t1 = bench([&]{ two_passes(x.data(), N, s1, ss1); sink(s1); sink(ss1); });
    auto t2 = bench([&]{ fused_1acc(x.data(), N, s2, ss2); sink(s2); sink(ss2); });
    auto t3 = bench([&]{ fused_2acc(x.data(), N, s3, ss3); sink(s3); sink(ss3); });

    long long times[3] = { t1, t2, t3 };
    auto cpe = [&](int k) {
        return (double)times[k] * CPU_GHZ * 1e6 / (double)N;
    };
    auto cpi = [&](int k) {
        return cpe(k) / INSNS[k];
    };

    cout << "\n" << string(76, '-') << "\n";
    cout << "sum + sumsq   N=64M floats  256 MB   -O1 (no SIMD)   CPU=" << CPU_GHZ << " GHz\n";
    cout << string(76, '-') << "\n";
    cout << left  << setw(22) << "version"
         << right << setw(8)  << "time"
         << right << setw(12) << "cycles/elem"
         << right << setw(7)  << "CPI"
         << right << setw(10) << "speedup" << "\n";
    cout << string(76, '-') << "\n";

    const char* labels[3] = {
        "two_passes",
        "fused_1acc",
        "fused_2acc"
    };
    for (int k = 0; k < 3; k++) {
        cout << left  << setw(22) << labels[k]
             << right << setw(7)  << times[k] << " ms"
             << right << setw(11) << fixed << setprecision(2) << cpe(k)
             << right << setw(7)  << fixed << setprecision(2) << cpi(k)
             << right << setw(9)  << fixed << setprecision(2)
             << (double)t1 / max(1LL, times[k]) << "x\n";
    }

    cout << "\nTheoretical cycles per element:\n"
         << "  two_passes: 2 loops × " << FADD_LATENCY << " cycles/elem = "
         << 2*FADD_LATENCY << " cycles  (chains serialized at loop boundary)\n"
         << "  fused_1acc: 1 pass, chains run in parallel ≈ "
         << FADD_LATENCY << " cycles  (sumsq fills sum's stall slots)\n"
         << "  fused_2acc: 2 accumulators × 2 chains, zero stalls ≈ "
         << FADD_LATENCY/2 + 1 << " cycles\n";

    cout << "\nNote: at -O2/-O3 the compiler may auto-vectorise or restructure,\n"
            "      masking the scalar ILP effect.  Build with -O1 to see it.\n";

    return 0;
}
