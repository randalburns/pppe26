// Branch-Free Code: Arithmetic and Bitwise Replacements for Conditionals
//
// Every conditional branch has two costs:
//
//   1. Prediction cost — the branch predictor must guess which path to take.
//      Correct predictions are "free" (speculative execution was on the right
//      path).  Wrong predictions flush the pipeline: ≈15–20 cycles wasted.
//
//   2. Pattern cost — even perfect prediction requires a pattern.  Data that
//      has no pattern (uniformly random) can only be predicted at 50% accuracy
//      for a 50/50 branch, yielding a misprediction every other iteration.
//
// Branch-free code eliminates the branch instruction entirely.  The condition
// is computed as an integer (0 or 1) or bit mask (0x00000000 or 0xFFFFFFFF),
// and arithmetic replaces the conditional path selection.  With no branch:
//
//   - No prediction is required.
//   - No pipeline flush is possible.
//   - Execution time is identical regardless of data pattern.
//
// This example processes N byte values per element:
//
//   - Clamp to [LO, HI]   — two branches in the naive version
//   - Count values > MID  — one branch in the naive version
//
// Three implementations are compared:
//
//   branchy    — explicit if-statements, real branch instructions
//   ternary    — C++ ternary ?:, compiler emits CSEL/CMOV (no branch)
//   arith      — pure arithmetic and bitwise ops (the focus of this example)
//
// The three core bit tricks used in the arith version:
//
//   1. Signed arithmetic right shift as a mask:
//
//        int mask = x >> 31;
//        // mask == 0x00000000 when x >= 0
//        // mask == 0xFFFFFFFF when x < 0  (sign-extended)
//
//   2. Branchless max(v, LO) using the mask:
//
//        int diff = v - LO;
//        int mask = diff >> 31;         // -1 if v < LO, else 0
//        v -= diff & mask;              // v = v if diff >= 0; v = LO if diff < 0
//
//      Equivalent to: v = v < LO ? LO : v
//
//   3. Branchless min(v, HI) using the inverted mask:
//
//        int diff = v - HI;
//        int mask = diff >> 31;         // 0 if v > HI, else -1
//        v -= diff & ~mask;             // v = v if diff <= 0; v = HI if diff > 0
//
//      Equivalent to: v = v > HI ? HI : v
//
//   4. Comparison as integer (no trick needed — C++ defines it):
//
//        count += (v > MID);            // 0 or 1, no branch
//
// Build (GCC required — Apple Clang converts if-statements to CSEL at -O1,
// eliminating branches before we can observe their cost):
//
//   g++-15 -O1 -fno-if-conversion -o branch_free branch_free.cpp && ./branch_free
//
// -fno-if-conversion: prevents GCC from converting if-statements to CSEL.
// This is needed only to demonstrate the branchy version's cost.
// The ternary and arith versions have no if-statements and are unaffected.

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

static const int N         = 32 * 1024 * 1024;  // 32M bytes = 32 MB
static const int LO        = 64;                  // clamp floor
static const int HI        = 192;                 // clamp ceiling
static const int MID       = 128;                 // count threshold
static const int RUNS      = 5;

// Misprediction penalty on Apple M-series ≈ 15 cycles.
// x86-64 (Skylake): ≈ 15–20 cycles.
static const int MISPREDICT_PENALTY = 15;

// CPU frequency (Apple M4 performance core).
// Adjust: M1/M2 ≈ 3.2 GHz, M3 ≈ 3.7 GHz, M4 ≈ 4.0 GHz.
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

struct Result { int64_t sum; int64_t count; };

// ================================================================
// Branchy: three if-statements, three conditional branch instructions
//
// With uniformly random data in [0, 255]:
//   Branch 1 (v < LO=64):   ~25% taken → ~25% misprediction rate
//   Branch 2 (v > HI=192):  ~25% taken → ~25% misprediction rate
//   Branch 3 (v > MID=128): ~50% taken → ~50% misprediction rate
//
// Expected overhead: N × avg_mispredict × penalty / freq
//   = 32M × 1.0 × 15 / 4GHz ≈ 120 ms
//
// Assembly (GCC -O1 -fno-if-conversion):
//   ldrb  w2, [x3]          ; load v
//   cmp   w2, #63
//   ble   .L_clamp_lo        ; branch 1: v < LO?
//   cmp   w2, #192
//   ble   .L_skip_hi         ; branch 2: v > HI?
//   ...
//   cmp   w2, #128
//   ble   .L_skip_count      ; branch 3: v > MID?
//   add   x1, x1, #1        ; count++
// ================================================================

Result process_branchy(const uint8_t* data, int n) {
    int64_t sum = 0, count = 0;
    for (int i = 0; i < n; i++) {
        int v = data[i];
        if (v < LO)  v = LO;
        if (v > HI)  v = HI;
        if (v > MID) count++;
        sum += v;
    }
    return {sum, count};
}

// ================================================================
// Ternary: C++ ?: operators — compiler emits CSEL on ARM, CMOV on x86
//
// These are conditional select instructions: they compute both paths
// and select the result based on a flag bit.  No branch instruction
// is emitted; no prediction is required; no pipeline flush is possible.
//
// Assembly (GCC -O1):
//   ldrb  w2, [x3]         ; load v
//   cmp   w2, #63
//   csel  w2, w4, w2, le   ; v = (v < LO) ? LO : v  — no branch
//   cmp   w2, #192
//   csel  w2, w5, w2, gt   ; v = (v > HI) ? HI : v  — no branch
//   cmp   w2, #128
//   cinc  x1, x1, gt       ; count += (v > MID)     — no branch
//
// This is the "zero-effort" branchless approach: write natural C++
// ternary expressions and let the compiler eliminate the branches.
// ================================================================

Result process_ternary(const uint8_t* data, int n) {
    int64_t sum = 0, count = 0;
    for (int i = 0; i < n; i++) {
        int v = data[i];
        v = v < LO ? LO : v;
        v = v > HI ? HI : v;
        count += (v > MID);
        sum += v;
    }
    return {sum, count};
}

// ================================================================
// Arith: pure arithmetic and bitwise ops — branch-free by construction
//
// Useful when:
//   - Writing SIMD intrinsics (no CSEL available for vectors)
//   - Targeting GPUs or shader languages (divergent branches are costly)
//   - Porting to architectures without conditional select instructions
//   - Making the branch-free invariant explicit and visible in source
//
// Technique 1 — signed right shift as a bit mask:
//
//   int mask = diff >> 31;
//   // Arithmetic right shift fills with the sign bit.
//   // diff >= 0  →  mask = 0x00000000 (all zeros)
//   // diff < 0   →  mask = 0xFFFFFFFF (all ones)
//
// Technique 2 — branchless max(v, LO):
//
//   int diff = v - LO;
//   int mask = diff >> 31;       // -1 (keep LO) or 0 (keep v)
//   v -= diff & mask;
//   // diff >= 0 (v >= LO): diff & 0 = 0 → v unchanged    ✓
//   // diff < 0  (v < LO):  diff & -1 = diff → v -= diff = v-(v-LO) = LO  ✓
//
// Technique 3 — branchless min(v, HI):
//
//   int diff = v - HI;
//   int mask = diff >> 31;       // 0 (clip to HI) or -1 (keep v)
//   v -= diff & ~mask;
//   // diff <= 0 (v <= HI): ~mask = 0 → v unchanged  ✓
//   // diff > 0  (v > HI):  ~mask = -1, diff & -1 = diff → v -= diff = HI  ✓
//
// Technique 4 — comparison as integer:
//
//   count += (v > MID);
//   // C++ comparison operators return exactly 0 or 1.
//   // The compiler emits CMP + CINC (conditional increment) — no branch.
//
// Assembly (GCC -O1):
//   ldrb  w2, [x4], #1
//   sub   w3, w2, #64
//   and   w3, w3, w3, asr #31   ; diff & mask  (max step)
//   sub   w2, w2, w3
//   sub   w3, w2, #192
//   asr   w5, w3, #31
//   bic   w3, w3, w5             ; diff & ~mask  (min step)
//   sub   w2, w2, w3
//   cmp   w2, #128
//   cinc  x1, x1, gt             ; count++  (no branch)
// ================================================================

Result process_arith(const uint8_t* data, int n) {
    int64_t sum = 0, count = 0;
    for (int i = 0; i < n; i++) {
        int v = data[i];

        // max(v, LO): clamp low — no branch
        int diff_lo = v - LO;
        v -= diff_lo & (diff_lo >> 31);

        // min(v, HI): clamp high — no branch
        int diff_hi = v - HI;
        v -= diff_hi & ~(diff_hi >> 31);

        // count threshold — no branch
        count += (v > MID);

        sum += v;
    }
    return {sum, count};
}

// ================================================================
// main
// ================================================================

int main() {
    vector<uint8_t> data_rand(N), data_sorted(N);

    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 255);
    for (auto& x : data_rand) x = static_cast<uint8_t>(dist(rng));

    data_sorted = data_rand;
    sort(data_sorted.begin(), data_sorted.end());

    // Correctness check
    Result r_b = process_branchy(data_rand.data(), N);
    Result r_t = process_ternary(data_rand.data(), N);
    Result r_a = process_arith  (data_rand.data(), N);
    bool ok = (r_b.sum == r_t.sum && r_b.sum == r_a.sum &&
               r_b.count == r_t.count && r_b.count == r_a.count);
    cout << "Correctness: " << (ok ? "PASS" : "FAIL")
         << "  sum=" << r_b.sum << "  count=" << r_b.count << "\n";

    // Expected misprediction overhead on random data
    double miss_rate = 0.25 + 0.25 + 0.50;   // branches 1 + 2 + 3
    double expected_overhead_ms =
        (double)N * miss_rate * MISPREDICT_PENALTY / (CPU_GHZ * 1e9) * 1e3;
    cout << "\nExpected misprediction overhead (random): "
         << fixed << setprecision(0) << expected_overhead_ms << " ms\n";

    // ---- Random data ----
    auto tb_r = bench([&]{ auto r = process_branchy(data_rand.data(), N); sink(r.sum); });
    auto tt_r = bench([&]{ auto r = process_ternary(data_rand.data(), N); sink(r.sum); });
    auto ta_r = bench([&]{ auto r = process_arith  (data_rand.data(), N); sink(r.sum); });

    // ---- Sorted data ----
    auto tb_s = bench([&]{ auto r = process_branchy(data_sorted.data(), N); sink(r.sum); });
    auto tt_s = bench([&]{ auto r = process_ternary(data_sorted.data(), N); sink(r.sum); });
    auto ta_s = bench([&]{ auto r = process_arith  (data_sorted.data(), N); sink(r.sum); });

    auto cpe = [&](long long ms) {
        return (double)ms * CPU_GHZ * 1e6 / (double)N;
    };

    cout << "\n" << string(76, '-') << "\n";
    cout << "N=" << N/(1024*1024) << "M bytes   LO=" << LO << " HI=" << HI
         << " MID=" << MID << "   -O1 -fno-if-conversion   CPU=" << CPU_GHZ << " GHz\n";
    cout << string(76, '-') << "\n";
    cout << left  << setw(18) << "version"
         << right << setw(14) << "random (ms)"
         << right << setw(10) << "cpe"
         << right << setw(10) << "speedup"
         << right << setw(14) << "sorted (ms)"
         << right << setw(10) << "speedup" << "\n";
    cout << string(76, '-') << "\n";

    struct Row { const char* name; long long rand_ms; long long sort_ms; };
    Row rows[3] = {
        { "branchy (3 ifs)",  tb_r, tb_s },
        { "ternary (csel)",   tt_r, tt_s },
        { "arith (bitmask)",  ta_r, ta_s },
    };
    for (auto& row : rows) {
        cout << left  << setw(18) << row.name
             << right << setw(12) << row.rand_ms << " ms"
             << right << setw(9)  << fixed << setprecision(2) << cpe(row.rand_ms)
             << right << setw(9)  << fixed << setprecision(2) << (double)tb_r / max(1LL, row.rand_ms) << "x"
             << right << setw(12) << row.sort_ms << " ms"
             << right << setw(9)  << fixed << setprecision(2) << (double)tb_s / max(1LL, row.sort_ms) << "x"
             << "\n";
    }

    cout << "\nBranch misprediction cost (random, branchy vs arith):\n"
         << "  measured overhead: " << tb_r - ta_r << " ms\n"
         << "  expected overhead: " << fixed << setprecision(0) << expected_overhead_ms << " ms\n";

    cout << "\nNote: ternary and arith produce identical assembly (CSEL/CINC).\n"
            "      arith shows the arithmetic identities explicitly — useful\n"
            "      when writing SIMD intrinsics or targeting architectures\n"
            "      without a conditional-select instruction.\n";

    return 0;
}
