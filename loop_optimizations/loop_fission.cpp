// Loop Fission Example
// Demonstrates splitting a high-register-pressure loop into two lower-pressure
// loops to eliminate compiler spills to the stack.
//
// Problem: compute 16 independent dot products (correlations) of one input
// signal against 16 reference vectors, all in a single loop.  The loop body
// needs 16 accumulators + 16 reference pointers + signal pointer + loop
// counter + current sample alive simultaneously — far more than the ~16
// available FP registers.  The compiler spills accumulators to the stack,
// adding load/store instructions on every iteration.
//
// Fission splits the 16-correlation loop into two 8-correlation loops.
// Each half fits comfortably in the register file; spills disappear.
//
// To see the spill/reload instructions:
//   g++ -O1 -S -o fission.s loop_fission.cpp
//   grep -c "mov.*%rsp\|spill\|stack" fission.s   # rough spill count
//
// Build:
//   g++ -O1 -o fission_O1 loop_fission.cpp && ./fission_O1

#include <iostream>
#include <chrono>
#include <climits>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

static const int N       = 10'000'000;   // samples per signal
static const int REFS    = 32;           // number of reference vectors
static const int RUNS    = 5;

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

void report(const char* title,
            const char* a_label, long long a_ms,
            const char* b_label, long long b_ms) {
    double speedup = (double)a_ms / max(1LL, b_ms);
    cout << "\n" << string(64, '-') << "\n";
    cout << title << "\n";
    cout << string(64, '-') << "\n";
    cout << "  FUSED    " << left  << setw(44) << a_label
         << right << setw(5) << a_ms << " ms\n";
    cout << "  FISSION  " << left  << setw(44) << b_label
         << right << setw(5) << b_ms  << " ms\n";
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
}

// ================================================================
// Unfissioned — all 32 correlations in one loop
//
// Live inside the loop body:
//   32 accumulators (c[0..31])
//   32 reference pointers (ref[0..31])
//    1 signal pointer
//    1 loop counter
//    1 current sample (s)
//   --------------------------------
//   ~67 values competing for 32 FP + 31 GP registers (ARM64)
//   -> compiler must spill accumulators to the stack
// ================================================================

void correlate_fused(const float* signal,
                     const float* const* ref,
                     float* c, int n) {
    for (int k = 0; k < REFS; k++) c[k] = 0.0f;

    for (int i = 0; i < n; i++) {
        float s = signal[i];
        c[ 0] += s * ref[ 0][i];
        c[ 1] += s * ref[ 1][i];
        c[ 2] += s * ref[ 2][i];
        c[ 3] += s * ref[ 3][i];
        c[ 4] += s * ref[ 4][i];
        c[ 5] += s * ref[ 5][i];
        c[ 6] += s * ref[ 6][i];
        c[ 7] += s * ref[ 7][i];
        c[ 8] += s * ref[ 8][i];
        c[ 9] += s * ref[ 9][i];
        c[10] += s * ref[10][i];
        c[11] += s * ref[11][i];
        c[12] += s * ref[12][i];
        c[13] += s * ref[13][i];
        c[14] += s * ref[14][i];
        c[15] += s * ref[15][i];
        c[16] += s * ref[16][i];
        c[17] += s * ref[17][i];
        c[18] += s * ref[18][i];
        c[19] += s * ref[19][i];
        c[20] += s * ref[20][i];
        c[21] += s * ref[21][i];
        c[22] += s * ref[22][i];
        c[23] += s * ref[23][i];
        c[24] += s * ref[24][i];
        c[25] += s * ref[25][i];
        c[26] += s * ref[26][i];
        c[27] += s * ref[27][i];
        c[28] += s * ref[28][i];
        c[29] += s * ref[29][i];
        c[30] += s * ref[30][i];
        c[31] += s * ref[31][i];
    }
}

// ================================================================
// Fissioned — split into two loops of 16 correlations each
//
// Live inside each loop body:
//   16 accumulators
//   16 reference pointers
//    1 signal pointer
//    1 loop counter
//    1 current sample
//   --------------------------------
//   ~35 values — fits in the register file, no spills
// ================================================================

void correlate_fission(const float* signal,
                       const float* const* ref,
                       float* c, int n) {
    for (int k = 0; k < REFS; k++) c[k] = 0.0f;

    // First half: correlations 0-15
    for (int i = 0; i < n; i++) {
        float s = signal[i];
        c[ 0] += s * ref[ 0][i];
        c[ 1] += s * ref[ 1][i];
        c[ 2] += s * ref[ 2][i];
        c[ 3] += s * ref[ 3][i];
        c[ 4] += s * ref[ 4][i];
        c[ 5] += s * ref[ 5][i];
        c[ 6] += s * ref[ 6][i];
        c[ 7] += s * ref[ 7][i];
        c[ 8] += s * ref[ 8][i];
        c[ 9] += s * ref[ 9][i];
        c[10] += s * ref[10][i];
        c[11] += s * ref[11][i];
        c[12] += s * ref[12][i];
        c[13] += s * ref[13][i];
        c[14] += s * ref[14][i];
        c[15] += s * ref[15][i];
    }

    // Second half: correlations 16-31
    for (int i = 0; i < n; i++) {
        float s = signal[i];
        c[16] += s * ref[16][i];
        c[17] += s * ref[17][i];
        c[18] += s * ref[18][i];
        c[19] += s * ref[19][i];
        c[20] += s * ref[20][i];
        c[21] += s * ref[21][i];
        c[22] += s * ref[22][i];
        c[23] += s * ref[23][i];
        c[24] += s * ref[24][i];
        c[25] += s * ref[25][i];
        c[26] += s * ref[26][i];
        c[27] += s * ref[27][i];
        c[28] += s * ref[28][i];
        c[29] += s * ref[29][i];
        c[30] += s * ref[30][i];
        c[31] += s * ref[31][i];
    }
}

// ================================================================
// main
// ================================================================

int main() {
    // Build signal and reference vectors with distinct values.
    vector<float> signal(N);
    vector<vector<float>> ref_data(REFS, vector<float>(N));
    for (int i = 0; i < N; i++) signal[i] = sinf(i * 0.001f);
    for (int k = 0; k < REFS; k++)
        for (int i = 0; i < N; i++)
            ref_data[k][i] = sinf(i * 0.001f * (k + 1));

    // Build pointer array for easy passing.
    vector<const float*> refs(REFS);
    for (int k = 0; k < REFS; k++) refs[k] = ref_data[k].data();

    const float* sig = signal.data();
    const float* const* rp = refs.data();

    vector<float> c1(REFS), c2(REFS);

    // Correctness check.
    correlate_fused  (sig, rp, c1.data(), N);
    correlate_fission(sig, rp, c2.data(), N);
    cout << "Correctness check:\n";
    bool ok = true;
    for (int k = 0; k < REFS; k++) {
        float delta = fabsf(c1[k] - c2[k]);
        if (delta > 1e-3f) { ok = false; cout << "  FAIL k=" << k << "\n"; }
    }
    cout << "  " << (ok ? "PASS" : "FAIL") << "\n";

    auto t_fused   = bench([&]{ correlate_fused  (sig, rp, c1.data(), N); sink(c1[0]); });
    auto t_fission = bench([&]{ correlate_fission(sig, rp, c2.data(), N); sink(c2[0]); });

    report("32 correlations — register pressure vs loop fission",
           "1 loop  (32 accumulators, spills to stack)", t_fused,
           "2 loops (16 accumulators each, no spills)",  t_fission);

    cout << "\nTo inspect spills:\n"
            "  g++ -O1 -S -o fission.s loop_fission.cpp\n"
            "  grep -A1 'rsp' fission.s | head -40\n";
    return 0;
}
