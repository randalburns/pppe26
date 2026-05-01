#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;
using namespace std::chrono;

// optnone disables ALL compiler optimization on a function, ensuring the
// expensive operation in BEFORE functions actually executes as written rather
// than being strength-reduced away by the compiler.  optnone implies noinline.
//
// AFTER functions use noinline (prevents constant-folding at the call site)
// but get full -O2 optimization, reflecting what the compiler produces after
// applying strength reduction.
#define BEFORE __attribute__((optnone))
//#define AFTER  __attribute__((noinline))
#define AFTER  __attribute__((optpnone))

static const int ITERATIONS = 100000000;

// Force the compiler to treat val as consumed without generating a visible store.
// Using "m" (memory) works for both integer and floating-point types.
template <typename T>
static void sink(T const& val) {
    asm volatile("" : : "m"(val) : "memory");
}

template <typename Func>
long long benchmark(Func func, int runs = 3) {
    long long minTime = LLONG_MAX;
    for (int r = 0; r < runs; r++) {
        asm volatile("" ::: "memory");
        auto start = high_resolution_clock::now();
        auto result = func();
        sink(result);
        auto end = high_resolution_clock::now();
        long long time = duration_cast<milliseconds>(end - start).count();
        minTime = min(minTime, time);
    }
    return minTime;
}

void printResult(const string& testName,
                 const string& before, long long timeBefore,
                 const string& after,  long long timeAfter) {
    double speedup = (double)timeBefore / max(1LL, timeAfter);
    cout << "\n" << string(60, '=') << endl;
    cout << testName << endl;
    cout << string(60, '-') << endl;
    cout << "  " << setw(30) << left << before << ": "
         << setw(6) << right << timeBefore << " ms" << endl;
    cout << "  " << setw(30) << left << after  << ": "
         << setw(6) << right << timeAfter  << " ms" << endl;
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x" << endl;
}

// ============================================================
// TEST 1: Loop multiply → induction accumulate
// ============================================================

BEFORE long long test1_multiply() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += i * 4;
    }
    return sum;
}

AFTER long long test1_addition() {
    long long sum = 0;
    int temp = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += temp;
        temp += 4;
    }
    return sum;
}

// ============================================================
// TEST 2: Multiply by power-of-2 → left shift
// ============================================================

BEFORE long long test2_multiply() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int val = i;
        val = val * 8;
        val = val * 16;
        val = val * 32;
        sum += val;
    }
    return sum;
}

AFTER long long test2_shift() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int val = i;
        val = val << 3;
        val = val << 4;
        val = val << 5;
        sum += val;
    }
    return sum;
}

// ============================================================
// TEST 3: Division by power-of-2 → right shift
// ============================================================

BEFORE long long test3_divide() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        unsigned int val = i;
        val = val / 4;
        val = val / 8;
        val = val / 16;
        sum += val;
    }
    return sum;
}

AFTER long long test3_shift() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        unsigned int val = i;
        val = val >> 2;
        val = val >> 3;
        val = val >> 4;
        sum += val;
    }
    return sum;
}

// ============================================================
// TEST 4: Modulo power-of-2 → bitwise AND
// ============================================================

BEFORE long long test4_modulo() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned int val = i;
        sum += val % 16;
        sum += val % 64;
        sum += val % 256;
    }
    return sum;
}

AFTER long long test4_bitmask() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned int val = i;
        sum += val & 15;
        sum += val & 63;
        sum += val & 255;
    }
    return sum;
}

// ============================================================
// TEST 5: pow() → repeated multiplication
// ============================================================

BEFORE double test5_pow() {
    double sum = 0;
    for (int i = 1; i < ITERATIONS / 10; i++) {
        double val = i * 0.0001;
        sum += pow(val, 2);
        sum += pow(val, 3);
        sum += pow(val, 4);
    }
    return sum;
}

AFTER double test5_multiply() {
    double sum = 0;
    for (int i = 1; i < ITERATIONS / 10; i++) {
        double val = i * 0.0001;
        sum += val * val;
        sum += val * val * val;
        sum += val * val * val * val;
    }
    return sum;
}

// ============================================================
// TEST 6: Repeated division → reciprocal multiply
// ============================================================

BEFORE double test6_division() {
    double sum = 0;
    double divisor = 3.14159;
    for (int i = 1; i < ITERATIONS; i++) {
        sum += i / divisor;
    }
    return sum;
}

AFTER double test6_reciprocal() {
    double sum = 0;
    double reciprocal = 1.0 / 3.14159;
    for (int i = 1; i < ITERATIONS; i++) {
        sum += i * reciprocal;
    }
    return sum;
}

// ============================================================
// TEST 7: Array stride multiply → pointer advance
// ============================================================

BEFORE long long test7_indexing() {
    vector<int> arr(10000);
    for (int i = 0; i < 10000; i++) arr[i] = i;
    long long sum = 0;
    int stride = 3;
    for (int iter = 0; iter < 10000; iter++) {
        for (int i = 0; i < 3000; i++) {
            sum += arr[i * stride];
        }
    }
    return sum;
}

AFTER long long test7_pointer() {
    vector<int> arr(10000);
    for (int i = 0; i < 10000; i++) arr[i] = i;
    long long sum = 0;
    int stride = 3;
    for (int iter = 0; iter < 10000; iter++) {
        int* p = arr.data();
        for (int i = 0; i < 3000; i++) {
            sum += *p;
            p += stride;
        }
    }
    return sum;
}

// ============================================================
// TEST 8: Complex loop multiply → two accumulators
// ============================================================

BEFORE long long test8_complex_multiply() {
    long long sum = 0;
    int width = 7, scale = 13, offset = 100;
    for (int i = 0; i < ITERATIONS; i++) {
        int index = i * width + offset;
        int value = i * scale;
        sum += index + value;
    }
    return sum;
}

AFTER long long test8_strength_reduced() {
    long long sum = 0;
    int width = 7, scale = 13, offset = 100;
    int index = offset, value = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += index + value;
        index += width;
        value += scale;
    }
    return sum;
}

// ============================================================
// TEST 9: Multiply near-power-of-2 → shift-add combo
// ============================================================

BEFORE long long test9_multiply() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += i * 15;
        sum += i * 17;
        sum += i * 31;
    }
    return sum;
}

AFTER long long test9_shift_add() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += (i << 4) - i;
        sum += (i << 4) + i;
        sum += (i << 5) - i;
    }
    return sum;
}

// ============================================================
// TEST 10: Float division by 3 → integer reciprocal trick
// ============================================================

BEFORE long long test10_float_division() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        sum += (int)(i / 3.0);
    }
    return sum;
}

AFTER long long test10_integer_trick() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        // i / 3 via multiply-high + shift (compiler-generated magic for /3)
        sum += (int)(((long long)i * 2863311531ULL) >> 33);
    }
    return sum;
}

// ============================================================
// TEST 11: Three separate divisions → one combined division
// ============================================================

BEFORE long long test11_repeated_division() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS / 2; i++) {
        int val = i;
        val = val / 7;
        val = val / 7;
        val = val / 7;
        sum += val;
    }
    return sum;
}

AFTER long long test11_single_division() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS / 2; i++) {
        sum += i / 343;  // 7^3
    }
    return sum;
}

// ============================================================
// TEST 12: Branchy absolute value → branchless arithmetic
// ============================================================

BEFORE long long test12_branch() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int val = i - ITERATIONS / 2;
        if (val < 0) sum += -val;
        else         sum += val;
    }
    return sum;
}

AFTER long long test12_arithmetic() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int val  = i - ITERATIONS / 2;
        int mask = val >> 31;        // all 1s if negative, all 0s if positive
        sum += (val ^ mask) - mask;  // branchless absolute value
    }
    return sum;
}

int main() {
    cout << "╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "║        STRENGTH REDUCTION TIMING EXAMPLES                ║" << endl;
    cout << "║        Iterations: " << setw(10) << ITERATIONS << "                         ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════╝" << endl;

    cout << "\nWarming up..." << endl;
    {
        volatile long long w = test1_multiply() + test1_addition();
        (void)w;
    }

    cout << "Running benchmarks..." << endl;

    {
        long long t1 = benchmark(test1_multiply);
        long long t2 = benchmark(test1_addition);
        printResult("TEST 1: Loop Multiply → Induction Accumulate",
                    "i * 4 each iteration", t1,
                    "temp += 4 (accumulate)", t2);
    }
    {
        long long t1 = benchmark(test2_multiply);
        long long t2 = benchmark(test2_shift);
        printResult("TEST 2: Multiply by 2^n → Left Shift",
                    "val * 8, * 16, * 32", t1,
                    "val << 3, << 4, << 5", t2);
    }
    {
        long long t1 = benchmark(test3_divide);
        long long t2 = benchmark(test3_shift);
        printResult("TEST 3: Divide by 2^n → Right Shift",
                    "val / 4, / 8, / 16", t1,
                    "val >> 2, >> 3, >> 4", t2);
    }
    {
        long long t1 = benchmark(test4_modulo);
        long long t2 = benchmark(test4_bitmask);
        printResult("TEST 4: Modulo 2^n → Bitwise AND",
                    "val % 16, % 64, % 256", t1,
                    "val & 15, & 63, & 255", t2);
    }
    {
        long long t1 = benchmark(test5_pow);
        long long t2 = benchmark(test5_multiply);
        printResult("TEST 5: pow() → Repeated Multiply",
                    "pow(x,2), pow(x,3), pow(x,4)", t1,
                    "x*x, x*x*x, x*x*x*x", t2);
    }
    {
        long long t1 = benchmark(test6_division);
        long long t2 = benchmark(test6_reciprocal);
        printResult("TEST 6: Repeated Division → Reciprocal Multiply",
                    "i / 3.14159", t1,
                    "i * (1.0 / 3.14159)", t2);
    }
    {
        long long t1 = benchmark(test7_indexing);
        long long t2 = benchmark(test7_pointer);
        printResult("TEST 7: Stride Index Multiply → Pointer Advance",
                    "arr[i * stride]", t1,
                    "*p; p += stride", t2);
    }
    {
        long long t1 = benchmark(test8_complex_multiply);
        long long t2 = benchmark(test8_strength_reduced);
        printResult("TEST 8: Loop Expr Multiply → Two Accumulators",
                    "i*width+offset, i*scale", t1,
                    "index+=width, value+=scale", t2);
    }
    {
        long long t1 = benchmark(test9_multiply);
        long long t2 = benchmark(test9_shift_add);
        printResult("TEST 9: Multiply Near-2^n → Shift-Add Combo",
                    "i*15, i*17, i*31", t1,
                    "(i<<4)-i, (i<<4)+i, (i<<5)-i", t2);
    }
    {
        long long t1 = benchmark(test10_float_division);
        long long t2 = benchmark(test10_integer_trick);
        printResult("TEST 10: Float Div by 3 → Integer Magic",
                    "(int)(i / 3.0)", t1,
                    "(i * MAGIC) >> 33", t2);
    }
    {
        long long t1 = benchmark(test11_repeated_division);
        long long t2 = benchmark(test11_single_division);
        printResult("TEST 11: Three Divisions → One Combined",
                    "val/7/7/7", t1,
                    "val/343", t2);
    }
    {
        long long t1 = benchmark(test12_branch);
        long long t2 = benchmark(test12_arithmetic);
        printResult("TEST 12: Branchy Abs → Branchless Arithmetic",
                    "if (x<0) -x else x", t1,
                    "(x^mask)-mask", t2);
    }

    cout << "\n" << string(60, '=') << endl;
    cout << "SUMMARY - STRENGTH REDUCTION HIERARCHY" << endl;
    cout << string(60, '=') << endl;
    cout << R"(
    EXPENSIVE (Avoid)          CHEAP (Prefer)
    ─────────────────────────────────────────
    pow(x, n)           →      x * x * ...
    x / constant        →      x * reciprocal
    x / (2^n)           →      x >> n
    x % (2^n)           →      x & (2^n - 1)
    x * (2^n)           →      x << n
    x * constant        →      shifts + adds
    i * stride          →      ptr += stride
    loop: i * k         →      loop: temp += k
    if (x<0) -x else x  →      (x^(x>>31))-(x>>31)

    Typical Latencies (cycles):
    ┌────────────────┬──────────┐
    │ Operation      │ Cycles   │
    ├────────────────┼──────────┤
    │ Bit shift      │ 1        │
    │ Add/Subtract   │ 1        │
    │ Bitwise AND/OR │ 1        │
    │ Multiply       │ 3-4      │
    │ FP Multiply    │ 4-5      │
    │ FP Division    │ 10-15    │
    │ Int Division   │ 15-30    │
    │ pow()          │ 50-100+  │
    └────────────────┴──────────┘
)" << endl;

    return 0;
}
