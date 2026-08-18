// Lookup Table Instead of Branches: Hex Encoding
//
// A lookup table (LUT) replaces a conditional expression with a precomputed
// array indexed by the input value.  Instead of evaluating a condition and
// choosing between two code paths, the CPU does a single memory load:
//
//   Branch approach — nibble to hex character:
//     if (nibble < 10)
//         return '0' + nibble;      // digits 0–9
//     else
//         return 'a' + nibble - 10; // letters a–f
//
//   LUT approach — same result, no branch:
//     static const char HEX[] = "0123456789abcdef";
//     return HEX[nibble];           // index into 16-byte table
//
// The LUT fits in a single 16-byte cache line.  After the first access it
// lives in L1 cache and each subsequent lookup costs 1–4 cycles.  No
// condition is evaluated; no prediction is required; no pipeline flush is
// possible.
//
// This example encodes N bytes as hexadecimal (two characters per byte).
// Each byte produces two nibbles, each nibble hits the branch above.
// With uniformly random input, nibble values 0–9 and a–f each appear with
// probability 10/16 ≈ 62.5% (digit) vs. 37.5% (letter), giving a
// misprediction rate of ~37% per branch on random data.
//
// Build:
//   g++-15 -O1 -fno-if-conversion -o lut_branch lut_branch.cpp && ./lut_branch
//
// -fno-if-conversion keeps the ternary in encode_branchy as a real branch
// instruction.  Without it GCC (and Apple Clang) emit CSEL, which is already
// branchless and closes the performance gap.  The flag is needed only to show
// the full cost of the branchy version.

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

static const int N    = 32 * 1024 * 1024;  // 32M bytes in → 64M chars out
static const int RUNS = 5;

// Misprediction penalty on Apple M-series ≈ 15 cycles.
static const int MISPREDICT_PENALTY = 15;
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
        best = min(best, (long long)duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

// ================================================================
// Branchy: ternary per nibble — becomes a branch instruction with
// -fno-if-conversion; otherwise the compiler emits CSEL (branchless).
//
// Two branches per byte, each with a ~37.5% misprediction rate on
// random data.  Assembly (GCC -O1 -fno-if-conversion):
//
//   ldrb   w3, [x0], #1       ; load byte
//   lsr    w4, w3, #4         ; hi nibble
//   cmp    w4, #9
//   ble    .L_digit_hi        ; branch: nibble < 10?
//   add    w4, w4, #87        ; 'a' - 10 = 87
//   b      .L_store_hi
//   .L_digit_hi:
//   add    w4, w4, #48        ; '0'
//   .L_store_hi:
//   strb   w4, [x1], #1
//   and    w3, w3, #0xf       ; lo nibble
//   cmp    w3, #9
//   ble    .L_digit_lo        ; branch: nibble < 10?
//   ...
// ================================================================

void encode_branchy(const uint8_t* in, char* out, int n) {
    for (int i = 0; i < n; i++) {
        int hi = in[i] >> 4;
        int lo = in[i] & 0xF;
        out[i*2]   = hi < 10 ? '0' + hi : 'a' + hi - 10;
        out[i*2+1] = lo < 10 ? '0' + lo : 'a' + lo - 10;
    }
}

// ================================================================
// LUT: index a 16-byte table — no branch, no condition, no prediction.
//
// The entire table fits in a single L1 cache line (16 bytes).  After
// the first access, subsequent lookups cost 1–4 cycles each.
//
// Assembly (GCC -O1):
//
//   ldrb   w3, [x0], #1       ; load byte
//   lsr    w4, w3, #4         ; hi nibble
//   ldrb   w4, [x2, x4]       ; HEX[hi] — one load, no branch
//   strb   w4, [x1]
//   and    w3, w3, #0xf       ; lo nibble
//   ldrb   w3, [x2, x3]       ; HEX[lo] — one load, no branch
//   strb   w3, [x1, #1]
//   add    x1, x1, #2
// ================================================================

static const char HEX[17] = "0123456789abcdef";

void encode_lut(const uint8_t* in, char* out, int n) {
    for (int i = 0; i < n; i++) {
        out[i*2]   = HEX[in[i] >> 4];
        out[i*2+1] = HEX[in[i] & 0xF];
    }
}

// ================================================================
// main
// ================================================================

int main() {
    vector<uint8_t> data(N);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(0, 255);
    for (auto& x : data) x = static_cast<uint8_t>(dist(rng));

    vector<char> out_b(N * 2), out_l(N * 2);

    // Correctness check
    encode_branchy(data.data(), out_b.data(), N);
    encode_lut    (data.data(), out_l.data(), N);
    bool ok = equal(out_b.begin(), out_b.end(), out_l.begin());
    cout << "Correctness: " << (ok ? "PASS" : "FAIL")
         << "  sample: " << string(out_b.data(), 16) << "\n";

    // Expected misprediction overhead
    // 2 branches per byte; each nibble is a digit 62.5% of the time.
    // On random data a 62.5/37.5 branch mispredicts ≈ 37.5% of the time.
    double miss_per_byte   = 2.0 * 0.375;
    double expected_ms     = (double)N * miss_per_byte * MISPREDICT_PENALTY
                             / (CPU_GHZ * 1e9) * 1e3;
    cout << "\nExpected misprediction overhead: "
         << fixed << setprecision(0) << expected_ms << " ms\n";

    auto tb = bench([&]{ encode_branchy(data.data(), out_b.data(), N); sink(out_b[0]); });
    auto tl = bench([&]{ encode_lut    (data.data(), out_l.data(), N); sink(out_l[0]); });

    auto cpe = [&](long long ms) {
        return (double)ms * CPU_GHZ * 1e6 / (double)N;
    };

    cout << "\n" << string(60, '-') << "\n";
    cout << "hex encode   N=32M bytes → 64M chars   -O1 -fno-if-conversion\n";
    cout << string(60, '-') << "\n";
    cout << left  << setw(16) << "version"
         << right << setw(10) << "time"
         << right << setw(12) << "cycles/byte"
         << right << setw(10) << "speedup" << "\n";
    cout << string(60, '-') << "\n";

    cout << left  << setw(16) << "branchy"
         << right << setw(8)  << tb << " ms"
         << right << setw(11) << fixed << setprecision(2) << cpe(tb)
         << right << setw(9)  << "1.00x" << "\n";
    cout << left  << setw(16) << "lut"
         << right << setw(8)  << tl << " ms"
         << right << setw(11) << fixed << setprecision(2) << cpe(tl)
         << right << setw(9)  << fixed << setprecision(2)
         << (double)tb / max(1LL, tl) << "x" << "\n";

    cout << "\nMisprediction cost: " << tb - tl << " ms measured"
         << " vs " << fixed << setprecision(0) << expected_ms << " ms expected\n";

    cout << "\nWhy a LUT works here:\n"
            "  - Table is 16 bytes — fits in one L1 cache line.\n"
            "  - After warmup, each lookup costs ~4 cycles (L1 hit).\n"
            "  - Two branches eliminated per byte; no prediction required.\n"
            "  - LUT cost is the same regardless of data pattern.\n";

    return 0;
}
