// Build: clang++ -std=c++17 -O3 -o peak_flops peak_flops.cpp
// Measures peak single-threaded FP32 throughput: SIMD (NEON) and scalar.
//
// SIMD strategy:  8 independent vfmaq_f32 chains
//   - Each vfmaq_f32: 4 floats × 2 ops = 8 FLOP
//   - 8 chains × 8 FLOP = 64 FLOP/iter
//
// Scalar strategy: 8 independent fmaf chains
//   - Each fmaf: 1 float × 2 ops = 2 FLOP
//   - 8 chains × 2 FLOP = 16 FLOP/iter
//
// Both use 8 independent accumulators to hide the 4-cycle FMA latency.

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

// --- Scalar: 8 × float FMA chains ---
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
    double best_simd_t   = 1e9, best_scalar_t = 1e9;
    for (int k = 0; k < NTIMES; k++) {
        double t = run_simd(ITERS);
        if (t < best_simd_t)   best_simd_t   = t;
        t = run_scalar(ITERS);
        if (t < best_scalar_t) best_scalar_t = t;
    }

    double simd_gf   = (double)ITERS * 8 * 4 * 2 / best_simd_t   / 1e9;
    double scalar_gf = (double)ITERS * 8 * 1 * 2 / best_scalar_t / 1e9;

    printf("FP32 SIMD   (NEON x4): %7.2f GFLOPS/s\n", simd_gf);
    printf("FP32 scalar           : %7.2f GFLOPS/s\n", scalar_gf);
    printf("SIMD speedup          : %.2fx\n", simd_gf / scalar_gf);
    printf("PEAK_GFLOPS_SIMD=%.2f\n",   simd_gf);
    printf("PEAK_GFLOPS_SCALAR=%.2f\n", scalar_gf);
    return 0;
}
