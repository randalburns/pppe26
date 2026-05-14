#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <string>
#include <climits>

using namespace std;
using namespace std::chrono;

// Both BEFORE and AFTER carry optnone.  The compiler cannot optimize
// either function regardless of -O level, so speedups reflect true
// hardware instruction costs — not compiler cleverness.  Results are
// therefore consistent and meaningful at -O0 through -O3.
#define OPTNONE __attribute__((optnone))

static const int N = 100'000'000;

template <typename T>
static void sink(T const& val) {
    asm volatile("" : : "m"(val) : "memory");
}

template <typename Func>
long long bench(Func f, int runs = 5) {
    long long best = LLONG_MAX;
    for (int r = 0; r < runs; r++) {
        asm volatile("" ::: "memory");
        auto t0 = high_resolution_clock::now();
        auto v  = f();
        sink(v);
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

void report(const char* name,
            const char* before_label, long long before_ms,
            const char* after_label,  long long after_ms) {
    double speedup = (double)before_ms / max(1LL, after_ms);
    cout << "\n" << string(64, '-') << "\n";
    cout << name << "\n";
    cout << string(64, '-') << "\n";
    cout << "  BEFORE  " << left  << setw(42) << before_label
         << right << setw(5) << before_ms << " ms\n";
    cout << "  AFTER   " << left  << setw(42) << after_label
         << right << setw(5) << after_ms  << " ms\n";
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
}

// ================================================================
// 1. Integer division by 2^n  ->  right shift
//    IDIV: ~20-30 cycles      SHR: 1 cycle
// ================================================================

OPTNONE long long div_shift_before() {
    long long s = 0;
    for (unsigned i = 1; i < (unsigned)N; i++) {
        s += i / 4;
        s += i / 8;
        s += i / 16;
    }
    return s;
}

OPTNONE long long div_shift_after() {
    long long s = 0;
    for (unsigned i = 1; i < (unsigned)N; i++) {
        s += i >> 2;
        s += i >> 3;
        s += i >> 4;
    }
    return s;
}

// ================================================================
// 2. Modulo power-of-2  ->  bitwise AND
//    IDIV: ~20-30 cycles    AND: 1 cycle
// ================================================================

OPTNONE long long mod_and_before() {
    long long s = 0;
    for (unsigned i = 0; i < (unsigned)N; i++) {
        s += i % 16;
        s += i % 64;
        s += i % 256;
    }
    return s;
}

OPTNONE long long mod_and_after() {
    long long s = 0;
    for (unsigned i = 0; i < (unsigned)N; i++) {
        s += i & 15;
        s += i & 63;
        s += i & 255;
    }
    return s;
}

// ================================================================
// 3. pow(x, n)  ->  explicit multiplications
//    Library call + ~50-100 FP ops    vs    3-4 FMUL at 4-5 cycles each
// ================================================================

OPTNONE double pow_multiply_before() {
    double s = 0;
    for (int i = 1; i < N / 10; i++) {
        double x = i * 0.0001;
        s += pow(x, 2) + pow(x, 3) + pow(x, 4);
    }
    return s;
}

OPTNONE double pow_multiply_after() {
    double s = 0;
    for (int i = 1; i < N / 10; i++) {
        double x  = i * 0.0001;
        double x2 = x * x;
        s += x2 + x2 * x + x2 * x2;
    }
    return s;
}

// ================================================================
// 4. FP division by loop-invariant constant  ->  reciprocal multiply
//    FDIV: ~10-15 cycles    FMUL: 4-5 cycles
//    One reciprocal computed before the loop; N divisions -> N multiplies
// ================================================================

OPTNONE double fdiv_recip_before() {
    double s = 0, d = 3.14159265358979;
    for (int i = 1; i < N; i++) s += i / d;
    return s;
}

OPTNONE double fdiv_recip_after() {
    double s = 0, r = 1.0 / 3.14159265358979;
    for (int i = 1; i < N; i++) s += i * r;
    return s;
}

// ================================================================
// 5. Three divisions by the same constant  ->  one combined division
//    3x IDIV (~20-30 cycles each)    vs    1x IDIV
// ================================================================

OPTNONE long long chain_div_before() {
    long long s = 0;
    for (int i = 1; i < N / 2; i++) {
        int v = i;
        v /= 7;
        v /= 7;
        v /= 7;
        s += v;
    }
    return s;
}

OPTNONE long long chain_div_after() {
    long long s = 0;
    for (int i = 1; i < N / 2; i++)
        s += i / 343;   // 7^3 = 343, one IDIV replaces three
    return s;
}

// ================================================================
// 6. Loop-induction multiply  ->  addition accumulate
//    IMUL each iteration (3-4 cycles)  vs  ADD each iteration (1 cycle)
//    Classic strength reduction: replace i*k with a running total
// ================================================================

OPTNONE long long induction_before() {
    long long s = 0;
    for (int i = 0; i < N; i++) s += i * 7 + 100;
    return s;
}

OPTNONE long long induction_after() {
    long long s = 0, v = 100;
    for (int i = 0; i < N; i++) { s += v; v += 7; }
    return s;
}

// ================================================================
// 7. Multiply by near-power-of-2  ->  shift + add/subtract
//    IMUL: 3-4 cycles    SHL+ADD or SHL+SUB: 1+1 = 2 cycles
//    x*15 = (x<<4)-x,  x*17 = (x<<4)+x,  x*31 = (x<<5)-x
// ================================================================

OPTNONE long long near_pow2_before() {
    long long s = 0;
    for (int i = 0; i < N; i++) {
        s += i * 15;
        s += i * 17;
        s += i * 31;
    }
    return s;
}

OPTNONE long long near_pow2_after() {
    long long s = 0;
    for (int i = 0; i < N; i++) {
        s += (i << 4) - i;    // i * 15
        s += (i << 4) + i;    // i * 17
        s += (i << 5) - i;    // i * 31
    }
    return s;
}

// ================================================================
// 8. Multiply by exact power-of-2  ->  left shift
//    IMUL: 3-4 cycles    SHL: 1 cycle
// ================================================================

OPTNONE long long mul_shift_before() {
    long long s = 0;
    for (int i = 0; i < N; i++) {
        s += i * 8;
        s += i * 64;
        s += i * 512;
    }
    return s;
}

OPTNONE long long mul_shift_after() {
    long long s = 0;
    for (int i = 0; i < N; i++) {
        s += i << 3;
        s += i << 6;
        s += i << 9;
    }
    return s;
}

int main() {
    cout << "================================================================\n";
    cout << "  STRENGTH REDUCTION -- HARDWARE INSTRUCTION COST COMPARISON\n";
    cout << "  optnone on BOTH functions: speedups hold at -O0 through -O3\n";
    cout << "================================================================\n";

    { volatile auto w = div_shift_before() + div_shift_after(); (void)w; }

    cout << "\nRunning benchmarks (" << N << " iterations, best of 5 runs)...\n";

    report("1. Integer div by 2^n  ->  right shift  (IDIV -> SHR)",
           "/4, /8, /16",
           bench(div_shift_before),
           ">>2, >>3, >>4",
           bench(div_shift_after));

    report("2. Modulo power-of-2  ->  bitwise AND  (IDIV -> AND)",
           "i%16 + i%64 + i%256",
           bench(mod_and_before),
           "i&15 + i&63 + i&255",
           bench(mod_and_after));

    report("3. pow(x, n)  ->  explicit multiplications  (libcall -> FMUL)",
           "pow(x,2)+pow(x,3)+pow(x,4)",
           bench(pow_multiply_before),
           "x*x + x*x*x + x*x*x*x",
           bench(pow_multiply_after));

    report("4. FP division by constant  ->  reciprocal multiply  (FDIV -> FMUL)",
           "i / 3.14159  each iter",
           bench(fdiv_recip_before),
           "i * (1/3.14159)  each iter",
           bench(fdiv_recip_after));

    report("5. Three divisions by same constant  ->  one combined  (3xIDIV -> 1xIDIV)",
           "i/7/7/7",
           bench(chain_div_before),
           "i/343  (7^3)",
           bench(chain_div_after));

    report("6. Loop-induction multiply  ->  addition accumulate  (IMUL -> ADD)",
           "i*7 + 100  each iteration",
           bench(induction_before),
           "v += 7  each iteration",
           bench(induction_after));

    report("7. Multiply near-2^n  ->  shift + add/sub  (IMUL -> SHL+ADD)",
           "i*15 + i*17 + i*31",
           bench(near_pow2_before),
           "(i<<4)-i + (i<<4)+i + (i<<5)-i",
           bench(near_pow2_after));

    report("8. Multiply by exact 2^n  ->  left shift  (IMUL -> SHL)",
           "i*8 + i*64 + i*512",
           bench(mul_shift_before),
           "i<<3 + i<<6 + i<<9",
           bench(mul_shift_after));

    cout << "\n================================================================\n";
    cout << "INSTRUCTION COST REFERENCE  (approximate cycles, modern x86)\n";
    cout << "----------------------------------------------------------------\n";
    cout << left;
    auto row = [](const char* op, const char* cyc, const char* replace) {
        cout << "  " << setw(26) << op << setw(8) << cyc << replace << "\n";
    };
    row("Bit shift (SHL/SHR)",     "1",        "—");
    row("Add / Subtract",           "1",        "—");
    row("Bitwise AND/OR/XOR",       "1",        "—");
    row("Integer mul (IMUL)",       "3-4",      "shifts + adds");
    row("FP multiply (FMUL)",       "4-5",      "precompute constant");
    row("FP divide (FDIV)",         "10-15",    "reciprocal * FMUL");
    row("Integer divide (IDIV)",    "20-30",    "shift (2^n) or reduce count");
    row("pow(x, n)",                "50-100+",  "x*x*... explicit");
    cout << "================================================================\n\n";

    return 0;
}
