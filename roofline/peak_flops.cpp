// Build: clang++ -std=c++17 -O3 -o peak_flops peak_flops.cpp
// Measures peak single-threaded FP32 throughput across four ceilings.
//
// SIMD FMA:        8 × vfmaq_f32  → 4 floats × 2 ops = 8 FLOP/instr, 64 FLOP/iter
// SIMD no-FMA:     8 × vmulq_f32  → 4 floats × 1 op  = 4 FLOP/instr, 32 FLOP/iter
// Scalar FMA:      8 × fmaf       → 1 float  × 2 ops = 2 FLOP/instr, 16 FLOP/iter
// Scalar no-FMA:   8 × float mul  → 1 float  × 1 op  = 1 FLOP/instr,  8 FLOP/iter
//
// All use 8 independent accumulators to hide the 4-cycle FMA latency.

#include <arm_neon.h>
#include <chrono>
#include <cstdio>

using namespace std::chrono;

template <typename T>
static void sink(T const& v) { asm volatile("" : : "m"(v) : "memory"); }

static const long long WARMUP = 1LL << 26; // longer warmup to ramp clock
static const long long ITERS  = 1LL << 28; // ~268 M iterations per run
static const int       NTIMES = 5;         // take best of N runs

// --- SIMD: 8 × float32x4 FMA chains ---
__attribute__((noinline))
static double run_simd(long long iters) {
    float32x4_t b = vdupq_n_f32(1.0001f);
    float32x4_t c = vdupq_n_f32(1.0002f);
    float32x4_t a0 = vdupq_n_f32(0.1f), a1 = vdupq_n_f32(0.2f);
    float32x4_t a2 = vdupq_n_f32(0.3f), a3 = vdupq_n_f32(0.4f);
    float32x4_t a4 = vdupq_n_f32(0.5f), a5 = vdupq_n_f32(0.6f);
    float32x4_t a6 = vdupq_n_f32(0.7f), a7 = vdupq_n_f32(0.8f);

    auto t0 = high_resolution_clock::now();
    for (long long i = 0; i < iters; i++) {
        a0 = vfmaq_f32(a0, b, c);
        a1 = vfmaq_f32(a1, b, c);
        a2 = vfmaq_f32(a2, b, c);
        a3 = vfmaq_f32(a3, b, c);
        a4 = vfmaq_f32(a4, b, c);
        a5 = vfmaq_f32(a5, b, c);
        a6 = vfmaq_f32(a6, b, c);
        a7 = vfmaq_f32(a7, b, c);
    }
    auto t1 = high_resolution_clock::now();

    sink(a0); sink(a1); sink(a2); sink(a3);
    sink(a4); sink(a5); sink(a6); sink(a7);
    return duration<double>(t1 - t0).count();
}

// --- SIMD no-FMA: 8 × float32x4 multiply-only chains ---
// vmulq_f32 is 4 FLOP/instr (mul only) vs vfmaq_f32's 8 FLOP/instr (mul+add).
// Same instruction throughput → half the GFLOPS.
__attribute__((noinline))
static double run_simd_nofma(long long iters) {
    float32x4_t b = vdupq_n_f32(1.0001f);
    float32x4_t a0 = vdupq_n_f32(0.1f), a1 = vdupq_n_f32(0.2f);
    float32x4_t a2 = vdupq_n_f32(0.3f), a3 = vdupq_n_f32(0.4f);
    float32x4_t a4 = vdupq_n_f32(0.5f), a5 = vdupq_n_f32(0.6f);
    float32x4_t a6 = vdupq_n_f32(0.7f), a7 = vdupq_n_f32(0.8f);

    auto t0 = high_resolution_clock::now();
    for (long long i = 0; i < iters; i++) {
        a0 = vmulq_f32(a0, b);
        a1 = vmulq_f32(a1, b);
        a2 = vmulq_f32(a2, b);
        a3 = vmulq_f32(a3, b);
        a4 = vmulq_f32(a4, b);
        a5 = vmulq_f32(a5, b);
        a6 = vmulq_f32(a6, b);
        a7 = vmulq_f32(a7, b);
    }
    auto t1 = high_resolution_clock::now();

    sink(a0); sink(a1); sink(a2); sink(a3);
    sink(a4); sink(a5); sink(a6); sink(a7);
    return duration<double>(t1 - t0).count();
}

// --- Scalar no-FMA: 8 × float multiply-only chains ---
__attribute__((noinline))
static double run_scalar_nofma(long long iters) {
    float b = 1.0001f;
    float a0 = 0.1f, a1 = 0.2f, a2 = 0.3f, a3 = 0.4f;
    float a4 = 0.5f, a5 = 0.6f, a6 = 0.7f, a7 = 0.8f;

    auto t0 = high_resolution_clock::now();
    for (long long i = 0; i < iters; i++) {
        a0 *= b;
        a1 *= b;
        a2 *= b;
        a3 *= b;
        a4 *= b;
        a5 *= b;
        a6 *= b;
        a7 *= b;
    }
    auto t1 = high_resolution_clock::now();

    sink(a0); sink(a1); sink(a2); sink(a3);
    sink(a4); sink(a5); sink(a6); sink(a7);
    return duration<double>(t1 - t0).count();
}

// --- Scalar FMA: 8 × float FMA chains ---
__attribute__((noinline))
static double run_scalar(long long iters) {
    float b = 1.0001f, c = 1.0002f;
    float a0 = 0.1f, a1 = 0.2f, a2 = 0.3f, a3 = 0.4f;
    float a4 = 0.5f, a5 = 0.6f, a6 = 0.7f, a7 = 0.8f;

    auto t0 = high_resolution_clock::now();
    for (long long i = 0; i < iters; i++) {
        a0 = __builtin_fmaf(b, c, a0);
        a1 = __builtin_fmaf(b, c, a1);
        a2 = __builtin_fmaf(b, c, a2);
        a3 = __builtin_fmaf(b, c, a3);
        a4 = __builtin_fmaf(b, c, a4);
        a5 = __builtin_fmaf(b, c, a5);
        a6 = __builtin_fmaf(b, c, a6);
        a7 = __builtin_fmaf(b, c, a7);
    }
    auto t1 = high_resolution_clock::now();

    sink(a0); sink(a1); sink(a2); sink(a3);
    sink(a4); sink(a5); sink(a6); sink(a7);
    return duration<double>(t1 - t0).count();
}

int main() {
    // Longer warmup: ramp CPU clock and keep it there before measuring
    run_simd(WARMUP);

    // Best-of-N like STREAM: peak time = peak clock
    double best_simd_t = 1e9, best_simd_nofma_t = 1e9;
    double best_scalar_t = 1e9, best_scalar_nofma_t = 1e9;
    for (int k = 0; k < NTIMES; k++) {
        double t = run_simd(ITERS);
        if (t < best_simd_t)         best_simd_t         = t;
        t = run_simd_nofma(ITERS);
        if (t < best_simd_nofma_t)   best_simd_nofma_t   = t;
        t = run_scalar(ITERS);
        if (t < best_scalar_t)       best_scalar_t       = t;
        t = run_scalar_nofma(ITERS);
        if (t < best_scalar_nofma_t) best_scalar_nofma_t = t;
    }

    double simd_fma_gf    = (double)ITERS * 8 * 4 * 2 / best_simd_t         / 1e9; // 64 FLOP/iter
    double simd_nofma_gf  = (double)ITERS * 8 * 4 * 1 / best_simd_nofma_t   / 1e9; // 32 FLOP/iter
    double scalar_fma_gf  = (double)ITERS * 8 * 1 * 2 / best_scalar_t       / 1e9; // 16 FLOP/iter
    double scalar_nofma_gf= (double)ITERS * 8 * 1 * 1 / best_scalar_nofma_t / 1e9; //  8 FLOP/iter

    printf("FP32 SIMD FMA   (NEON x4): %7.2f GFLOPS/s\n", simd_fma_gf);
    printf("FP32 SIMD no-FMA         : %7.2f GFLOPS/s\n", simd_nofma_gf);
    printf("FP32 scalar FMA          : %7.2f GFLOPS/s\n", scalar_fma_gf);
    printf("FP32 scalar no-FMA       : %7.2f GFLOPS/s\n", scalar_nofma_gf);
    printf("PEAK_GFLOPS_SIMD_FMA=%.2f\n",    simd_fma_gf);
    printf("PEAK_GFLOPS_SIMD_NOFMA=%.2f\n",  simd_nofma_gf);
    printf("PEAK_GFLOPS_SCALAR_FMA=%.2f\n",  scalar_fma_gf);
    printf("PEAK_GFLOPS_SCALAR_NOFMA=%.2f\n",scalar_nofma_gf);
    return 0;
}
