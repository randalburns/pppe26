#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

using namespace std;
using namespace std::chrono;

const int ITERATIONS = 100000000;

// Prevent compiler optimization
static void clobber() {
    asm volatile("" ::: "memory");
}

template <typename T>
static void doNotOptimize(T const& val) {
    asm volatile("" : : "r,m"(val) : "memory");
}

// Benchmark helper
template <typename Func>
long long benchmark(Func func, int runs = 3) {
    long long minTime = LLONG_MAX;
    
    for (int r = 0; r < runs; r++) {
        clobber();
        auto start = high_resolution_clock::now();
        clobber();
        
        auto result = func();
        doNotOptimize(result);
        
        clobber();
        auto end = high_resolution_clock::now();
        clobber();
        
        long long time = duration_cast<milliseconds>(end - start).count();
        minTime = min(minTime, time);
    }
    
    return minTime;
}

void printResult(const string& testName, 
                 const string& before, long long timeBefore,
                 const string& after, long long timeAfter) {
    double speedup = (double)timeBefore / max(1LL, timeAfter);
    
    cout << "\n" << string(60, '=') << endl;
    cout << testName << endl;
    cout << string(60, '-') << endl;
    cout << "  " << setw(30) << left << before << ": " 
         << setw(6) << right << timeBefore << " ms" << endl;
    cout << "  " << setw(30) << left << after << ": " 
         << setw(6) << right << timeAfter << " ms" << endl;
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x" << endl;
}

// ============================================================
// TEST 1: Multiplication vs Addition
// ============================================================

long long test1_multiply() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += i * 4;  // Multiplication each iteration
    }
    return sum;
}

long long test1_addition() {
    long long sum = 0;
    int temp = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += temp;
        temp += 4;  // Addition instead
    }
    return sum;
}

// ============================================================
// TEST 2: Multiplication vs Bit Shift
// ============================================================

long long test2_multiply() {
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

long long test2_shift() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        int val = i;
        val = val << 3;   // * 8
        val = val << 4;   // * 16
        val = val << 5;   // * 32
        sum += val;
    }
    return sum;
}

// ============================================================
// TEST 3: Division vs Bit Shift
// ============================================================

long long test3_divide() {
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

long long test3_shift() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        unsigned int val = i;
        val = val >> 2;   // / 4
        val = val >> 3;   // / 8
        val = val >> 4;   // / 16
        sum += val;
    }
    return sum;
}

// ============================================================
// TEST 4: Modulo vs Bit Mask
// ============================================================

long long test4_modulo() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned int val = i;
        sum += val % 16;
        sum += val % 64;
        sum += val % 256;
    }
    return sum;
}

long long test4_bitmask() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        unsigned int val = i;
        sum += val & 15;    // % 16
        sum += val & 63;    // % 64
        sum += val & 255;   // % 256
    }
    return sum;
}

// ============================================================
// TEST 5: Exponentiation vs Multiplication
// ============================================================

double test5_pow() {
    double sum = 0;
    for (int i = 1; i < ITERATIONS / 10; i++) {
        double val = i * 0.0001;
        sum += pow(val, 2);
        sum += pow(val, 3);
        sum += pow(val, 4);
    }
    return sum;
}

double test5_multiply() {
    double sum = 0;
    for (int i = 1; i < ITERATIONS / 10; i++) {
        double val = i * 0.0001;
        sum += val * val;                 // x^2
        sum += val * val * val;           // x^3
        sum += val * val * val * val;     // x^4
    }
    return sum;
}

// ============================================================
// TEST 6: Division vs Reciprocal Multiplication
// ============================================================

double test6_division() {
    double sum = 0;
    double divisor = 3.14159;
    for (int i = 1; i < ITERATIONS; i++) {
        sum += i / divisor;
    }
    return sum;
}

double test6_reciprocal() {
    double sum = 0;
    double reciprocal = 1.0 / 3.14159;  // Precompute
    for (int i = 1; i < ITERATIONS; i++) {
        sum += i * reciprocal;
    }
    return sum;
}

// ============================================================
// TEST 7: Array Indexing - Multiply vs Pointer Increment
// ============================================================

long long test7_indexing() {
    vector<int> arr(10000);
    for (int i = 0; i < 10000; i++) arr[i] = i;
    
    long long sum = 0;
    int stride = 3;
    
    for (int iter = 0; iter < 10000; iter++) {
        for (int i = 0; i < 3000; i++) {
            sum += arr[i * stride];  // Multiplication in indexing
        }
    }
    return sum;
}

long long test7_pointer() {
    vector<int> arr(10000);
    for (int i = 0; i < 10000; i++) arr[i] = i;
    
    long long sum = 0;
    int stride = 3;
    
    for (int iter = 0; iter < 10000; iter++) {
        int* p = arr.data();
        for (int i = 0; i < 3000; i++) {
            sum += *p;
            p += stride;  // Addition only
        }
    }
    return sum;
}

// ============================================================
// TEST 8: Complex Loop Index Expression
// ============================================================

long long test8_complex_multiply() {
    long long sum = 0;
    int width = 7;
    int scale = 13;
    int offset = 100;
    
    for (int i = 0; i < ITERATIONS; i++) {
        int index = i * width + offset;
        int value = i * scale;
        sum += index + value;
    }
    return sum;
}

long long test8_strength_reduced() {
    long long sum = 0;
    int width = 7;
    int scale = 13;
    int offset = 100;
    
    int index = offset;
    int value = 0;
    
    for (int i = 0; i < ITERATIONS; i++) {
        sum += index + value;
        index += width;   // Was: i * width + offset
        value += scale;   // Was: i * scale
    }
    return sum;
}

// ============================================================
// TEST 9: Multiply by Constant vs Shift-Add Combo
// ============================================================

long long test9_multiply() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += i * 15;   // 15 = 16 - 1
        sum += i * 17;   // 17 = 16 + 1
        sum += i * 31;   // 31 = 32 - 1
    }
    return sum;
}

long long test9_shift_add() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        sum += (i << 4) - i;   // i * 15 = i * 16 - i
        sum += (i << 4) + i;   // i * 17 = i * 16 + i
        sum += (i << 5) - i;   // i * 31 = i * 32 - i
    }
    return sum;
}

// ============================================================
// TEST 10: Floating Point Division vs Integer Approximation
// ============================================================

long long test10_float_division() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        sum += (int)(i / 3.0);
    }
    return sum;
}

long long test10_integer_trick() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS; i++) {
        // Division by 3 using multiply + shift
        // i / 3 ≈ (i * 0xAAAAAAAB) >> 33 for 32-bit
        // Simplified version:
        sum += (int)(((long long)i * 2863311531ULL) >> 33);
    }
    return sum;
}

// ============================================================
// TEST 11: Multiple Divisions in Loop
// ============================================================

long long test11_repeated_division() {
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

long long test11_single_division() {
    long long sum = 0;
    for (int i = 1; i < ITERATIONS / 2; i++) {
        int val = i;
        val = val / 343;  // 7 * 7 * 7 = 343
        sum += val;
    }
    return sum;
}

// ============================================================
// TEST 12: Branch vs Arithmetic (Bonus)
// ============================================================

long long test12_branch() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        // Get absolute value with branch
        int val = i - ITERATIONS / 2;
        if (val < 0) {
            sum += -val;
        } else {
            sum += val;
        }
    }
    return sum;
}

long long test12_arithmetic() {
    long long sum = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        // Get absolute value without branch (strength reduction of control flow)
        int val = i - ITERATIONS / 2;
        int mask = val >> 31;           // All 1s if negative, all 0s if positive
        sum += (val ^ mask) - mask;     // Branchless absolute value
    }
    return sum;
}

int main() {
    cout << "╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "║        STRENGTH REDUCTION TIMING EXAMPLES                ║" << endl;
    cout << "║        Iterations: " << setw(10) << ITERATIONS << "                         ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════╝" << endl;
    
    // Warmup
    cout << "\nWarming up..." << endl;
    volatile long long warmup = test1_multiply() + test1_addition();
    doNotOptimize(warmup);
    
    cout << "Running benchmarks..." << endl;
    
    // Test 1: Multiplication vs Addition
    {
        long long t1 = benchmark(test1_multiply);
        long long t2 = benchmark(test1_addition);
        printResult("TEST 1: Multiplication → Addition",
                   "i * 4 each iteration", t1,
                   "temp += 4 (accumulate)", t2);
    }
    
    // Test 2: Multiplication vs Bit Shift
    {
        long long t1 = benchmark(test2_multiply);
        long long t2 = benchmark(test2_shift);
        printResult("TEST 2: Multiplication → Bit Shift",
                   "val * 8, val * 16, val * 32", t1,
                   "val << 3, val << 4, val << 5", t2);
    }
    
    // Test 3: Division vs Bit Shift
    {
        long long t1 = benchmark(test3_divide);
        long long t2 = benchmark(test3_shift);
        printResult("TEST 3: Division → Bit Shift",
                   "val / 4, val / 8, val / 16", t1,
                   "val >> 2, val >> 3, val >> 4", t2);
    }
    
    // Test 4: Modulo vs Bit Mask
    {
        long long t1 = benchmark(test4_modulo);
        long long t2 = benchmark(test4_bitmask);
        printResult("TEST 4: Modulo → Bit Mask",
                   "val % 16, % 64, % 256", t1,
                   "val & 15, & 63, & 255", t2);
    }
    
    // Test 5: Exponentiation vs Multiplication
    {
        long long t1 = benchmark(test5_pow);
        long long t2 = benchmark(test5_multiply);
        printResult("TEST 5: Exponentiation → Multiplication",
                   "pow(x, 2), pow(x, 3), pow(x, 4)", t1,
                   "x*x, x*x*x, x*x*x*x", t2);
    }
    
    // Test 6: Division vs Reciprocal
    {
        long long t1 = benchmark(test6_division);
        long long t2 = benchmark(test6_reciprocal);
        printResult("TEST 6: Division → Reciprocal Multiply",
                   "i / 3.14159", t1,
                   "i * (1.0 / 3.14159)", t2);
    }
    
    // Test 7: Array Indexing
    {
        long long t1 = benchmark(test7_indexing);
        long long t2 = benchmark(test7_pointer);
        printResult("TEST 7: Array Index → Pointer Increment",
                   "arr[i * stride]", t1,
                   "*p; p += stride", t2);
    }
    
    // Test 8: Complex Loop Expression
    {
        long long t1 = benchmark(test8_complex_multiply);
        long long t2 = benchmark(test8_strength_reduced);
        printResult("TEST 8: Loop Index Multiply → Accumulate",
                   "i*width+offset, i*scale", t1,
                   "index+=width, value+=scale", t2);
    }
    
    // Test 9: Multiply vs Shift-Add
    {
        long long t1 = benchmark(test9_multiply);
        long long t2 = benchmark(test9_shift_add);
        printResult("TEST 9: Multiply → Shift-Add Combo",
                   "i*15, i*17, i*31", t1,
                   "(i<<4)-i, (i<<4)+i, (i<<5)-i", t2);
    }
    
    // Test 10: Float Division vs Integer Trick
    {
        long long t1 = benchmark(test10_float_division);
        long long t2 = benchmark(test10_integer_trick);
        printResult("TEST 10: Float Division → Integer Magic",
                   "(int)(i / 3.0)", t1,
                   "(i * MAGIC) >> 33", t2);
    }
    
    // Test 11: Multiple vs Single Division
    {
        long long t1 = benchmark(test11_repeated_division);
        long long t2 = benchmark(test11_single_division);
        printResult("TEST 11: Multiple Divisions → Single Division",
                   "val/7/7/7", t1,
                   "val/343", t2);
    }
    
    // Test 12: Branch vs Arithmetic
    {
        long long t1 = benchmark(test12_branch);
        long long t2 = benchmark(test12_arithmetic);
        printResult("TEST 12: Branch → Branchless Arithmetic",
                   "if (x<0) -x else x", t1,
                   "(x^mask)-mask", t2);
    }
    
    // Summary
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