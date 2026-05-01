/*
 * pipeline.cse.cpp
 *
 * Demonstrates how Common Subexpression Elimination (CSE) removes pipeline
 * stalls caused by data hazards (Read-After-Write / RAW dependencies).
 *
 * Background
 * ----------
 * Modern CPUs execute instructions in a pipeline (IF → ID → EX → MEM → WB).
 * A RAW hazard occurs when instruction N+1 reads a register that instruction N
 * hasn't finished writing yet.  The CPU must insert "bubble" stall cycles until
 * the result is ready.  Reusing an already-computed value avoids re-issuing the
 * slow instruction and eliminates the dependency chain entirely.
 *
 * Compile and benchmark:
 *   clang++ -O2 -o pipeline.cse pipeline.cse.cpp -lm && ./pipeline.cse
 *
 * Works at any optimisation level: NOINLINE + doNotOptimize() barriers prevent
 * the compiler from collapsing the before* variants even under -O3.  The after*
 * variants are intentionally left free to optimise so the comparison is fair.
 */

#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <functional>

using namespace std;
using namespace std::chrono;

// ============================================================
// Optimisation-prevention utilities
// ============================================================

// NOINLINE — prevents inter-procedural CSE across call sites.
#if defined(__GNUC__) || defined(__clang__)
#  define NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#  define NOINLINE __declspec(noinline)
#else
#  define NOINLINE
#endif

// doNotOptimize<T> — makes a value appear potentially modified to the compiler.
// The "+r,m" read-write constraint forces the compiler to treat val as unknown
// after the call, preventing CSE from reusing any expression that depends on it.
// Zero instructions are emitted; this is purely an optimiser fence.
template <typename T>
inline void doNotOptimize(T& val)
{
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : "+r,m"(val));
#elif defined(_MSC_VER)
    #include <intrin.h>
    _ReadWriteBarrier();
    (void)val;
#else
    (void)val;   // fallback: compile with -O0
#endif
}

// benchMs — runs fn() and returns wall-clock milliseconds elapsed.
static double benchMs(const function<void()>& fn)
{
    auto t0 = steady_clock::now();
    fn();
    auto t1 = steady_clock::now();
    return duration<double, milli>(t1 - t0).count();
}

// ============================================================
// SECTION 1 — Integer arithmetic: a*b + c reused 3×
// ============================================================
//
// BEFORE CSE — sub-expression recomputed each time.
//
// Pseudo-assembly (RISC, 5-stage pipeline, MUL latency = 3 cycles):
//
//   Cycle  Instruction
//   -----  -----------
//    1     MUL  t0, a, b        ; a*b  (result ready cycle 4)
//    2     --- stall ---
//    3     --- stall ---
//    4     ADD  x,  t0, c       ; x = a*b + c
//    5     MUL  t1, a, b        ; DUPLICATE (result ready cycle 8)
//    6     --- stall ---
//    7     --- stall ---
//    8     ADD  y,  t1, c       ; y = a*b + c  (same value!)
//    9     ADD  y,  y,  1
//   10     MUL  t2, a, b        ; DUPLICATE again
//   11     --- stall ---
//   12     --- stall ---
//   13     ADD  z,  t2, c       ; z = a*b + c  (same value again!)
//   14     ADD  z,  z,  2
//                                            ↑ 6 wasted stall cycles

NOINLINE static int beforeCse(int a, int b, int c)
{
    int x = a * b + c;          // chain 1: MUL → stall stall → ADD
    doNotOptimize(a); doNotOptimize(b); doNotOptimize(c);
    int y = a * b + c + 1;      // chain 2: MUL → stall stall → ADD → ADD
    doNotOptimize(a); doNotOptimize(b); doNotOptimize(c);
    int z = a * b + c + 2;      // chain 3: MUL → stall stall → ADD → ADD
    return x + y + z;
}

// AFTER CSE — compute once, reuse a register.
//
//   Cycle  Instruction
//   -----  -----------
//    1     MUL  t0, a, b        ; a*b once (result ready cycle 4)
//    2     --- stall ---
//    3     --- stall ---
//    4     ADD  base, t0, c     ; base = a*b + c
//    5     MOV  x,   base       ; x = base     ← no stall: base ready
//    6     ADD  y,   base, 1    ; y = base + 1  ← no stall
//    7     ADD  z,   base, 2    ; z = base + 2  ← no stall
//                                            ↑ 0 wasted stall cycles

NOINLINE static int afterCse(int a, int b, int c)
{
    const int base = a * b + c; // ONE multiply, ONE dependency chain
    const int x = base;         // register rename / copy — free
    const int y = base + 1;     // ADD on already-available register — free
    const int z = base + 2;     // ADD on already-available register — free
    return x + y + z;
}

// ============================================================
// SECTION 2 — Loop body: stride*i address calculation reused 3×
// ============================================================

NOINLINE static void arraySumBefore(const array<int, 1024>& arr, int stride, int n, long& out)
{
    long sum = 0;
    for (int i = 0; i < n; i++) {
        // stride*i recomputed three times; each is a MUL → stall chain.
        sum += arr[stride * i];
        doNotOptimize(stride); doNotOptimize(i);
        sum += arr[stride * i] * 2;
        doNotOptimize(stride); doNotOptimize(i);
        sum += arr[stride * i] + 10;
    }
    out = sum;
}

NOINLINE static void arraySumAfter(const array<int, 1024>& arr, int stride, int n, long& out)
{
    long sum = 0;
    for (int i = 0; i < n; i++) {
        const int idx  = stride * i; // single MUL per iteration
        const int elem = arr[idx];   // single LOAD — cached in register
        sum += elem;                 // no stall: elem already available
        sum += elem * 2;             // no stall
        sum += elem + 10;            // no stall
    }
    out = sum;
}

// ============================================================
// SECTION 3 — Floating-point: sqrt() reused 3×
//
// sqrt has ~14-cycle latency on typical x86.  Calling it three times
// with the same argument triples the stall budget.
// ============================================================

NOINLINE static double normalizeBefore(double x, double y, double z)
{
    const double nx = x / sqrt(x*x + y*y + z*z); // sqrt → ~14-cycle stall
    doNotOptimize(x); doNotOptimize(y); doNotOptimize(z);
    const double ny = y / sqrt(x*x + y*y + z*z); // sqrt again → stall
    doNotOptimize(x); doNotOptimize(y); doNotOptimize(z);
    const double nz = z / sqrt(x*x + y*y + z*z); // sqrt again → stall
    return nx + ny + nz;
}

NOINLINE static double normalizeAfter(double x, double y, double z)
{
    const double lenSq  = x*x + y*y + z*z;    // one FMA chain
    const double invLen = 1.0 / sqrt(lenSq);   // one sqrt, one division
    const double nx = x * invLen;              // FMUL — no stall
    const double ny = y * invLen;              // FMUL — no stall
    const double nz = z * invLen;              // FMUL — no stall
    return nx + ny + nz;
}

// ============================================================
// main
// ============================================================

static const int REPS = 100'000'000;

int main()
{
    volatile long   sinkI = 0;   // volatile prevents dead-code elimination
    volatile double sinkD = 0.0;
    long   tmpI = 0;
    double tmpD = 0.0;
    double ms = 0.0;

    printf("=== Pipeline Stall Elimination via Duplicate Code Elimination ===\n\n");

    // ---- Section 1: integer CSE ----
    printf("[ 1 ] Integer common sub-expression  (a*b + c reused 3x)\n");

    ms = benchMs([&]{ for (int i = 0; i < REPS; i++) sinkI += beforeCse(i, 3, 7); });
    printf("    BEFORE CSE : %6.2f ms  (result=%ld)\n", ms, (long)sinkI);

    sinkI = 0;
    ms = benchMs([&]{ for (int i = 0; i < REPS; i++) sinkI += afterCse(i, 3, 7); });
    printf("    AFTER  CSE : %6.2f ms  (result=%ld)\n\n", ms, (long)sinkI);

    // ---- Section 2: array index CSE ----
    printf("[ 2 ] Loop-body address calculation  (stride*i reused 3x)\n");

    array<int, 1024> arr;
    for (int i = 0; i < 1024; i++) arr[i] = i;

    ms = benchMs([&]{ for (int r = 0; r < 1000; r++) arraySumBefore(arr, 1, 100, tmpI); });
    sinkI = tmpI;
    printf("    BEFORE CSE : %6.2f ms  (result=%ld)\n", ms, (long)sinkI);

    ms = benchMs([&]{ for (int r = 0; r < 1000; r++) arraySumAfter(arr, 1, 100, tmpI); });
    sinkI = tmpI;
    printf("    AFTER  CSE : %6.2f ms  (result=%ld)\n\n", ms, (long)sinkI);

    // ---- Section 3: FP transcendental CSE ----
    printf("[ 3 ] Floating-point sqrt() reused 3x in normalize()\n");

    ms = benchMs([&]{
        for (int i = 1; i <= REPS / 10; i++)
            tmpD += normalizeBefore((double)i, i + 1.0, i + 2.0);
    });
    sinkD = tmpD;
    printf("    BEFORE CSE : %6.2f ms  (result=%.4f)\n", ms, (double)sinkD);

    tmpD = 0.0;
    ms = benchMs([&]{
        for (int i = 1; i <= REPS / 10; i++)
            tmpD += normalizeAfter((double)i, i + 1.0, i + 2.0);
    });
    sinkD = tmpD;
    printf("    AFTER  CSE : %6.2f ms  (result=%.4f)\n\n", ms, (double)sinkD);

    printf("Key takeaway:\n");
    printf("  Duplicate sub-expressions create redundant RAW dependency chains.\n");
    printf("  Caching the result in a local variable (CSE) means the expensive\n");
    printf("  operation executes once; subsequent uses read a ready register,\n");
    printf("  paying zero additional stall cycles.\n");

    return 0;
}
