// Find the index of the first element in an array equal to a target.
//
//   clang++ -O3 -mavx2 -std=c++17 -stdlib=libc++ find_first.cpp -o find_first
//   ./find_first
//
// A third version using Google Highway is compiled in automatically if the
// headers are present.  Highway picks its instruction set from the -m flags, and
// a bare -mavx2 is not enough to select the AVX2 target, so build it with:
//
//   clang++ -O3 -std=c++17 -stdlib=libc++ -mavx2 -mbmi -mbmi2 -mfma -mf16c
//       -maes -mpclmul find_first.cpp -o find_first
//
// The program prints which target Highway actually selected, so you can see when
// you have got this wrong.
//
// This is a loop the compiler cannot vectorize, and it will tell you so:
//
//   $ clang++ -O3 -mavx2 -Rpass-analysis=loop-vectorize -c find_first.cpp
//   remark: loop not vectorized: could not determine number of loop iterations
//
// A vectorizer needs to know the trip count before the loop starts, so it can
// run whole vectors and then handle the remainder.  This loop exits when it
// finds something, so the trip count depends on the data.  The compiler emits
// pure scalar code -- one element per iteration, no vector instructions.
//
// Doing it by hand is legal because we only ever load elements that are inside
// the array.  We check a vector's worth at a time -- four int64 on AVX2, two on
// NEON -- and stop at the first vector containing a match, then work out which
// lane it was.  The answer is identical to the scalar version for every possible
// input: same index, same -1 when absent.  Nothing is assumed about n, about the
// values, or about how many matches there are.

#if defined(__x86_64__) || defined(__i386__)
#define HAVE_SIMD_INTRINSICS 1
#define SIMD_ARCH_X86 1
#include <immintrin.h>
#elif defined(__aarch64__) || defined(__arm__)
#define HAVE_SIMD_INTRINSICS 1
#define SIMD_ARCH_NEON 1
#include <arm_neon.h>
#endif

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <random>
#include <vector>

#if __has_include(<hwy/highway.h>)
#define HAVE_HIGHWAY 1
#include <hwy/contrib/algo/find-inl.h>
#include <hwy/highway.h>
namespace hn = hwy::HWY_NAMESPACE;
#endif

// Scalar.  The compiler leaves this alone.
long find_scalar(const int64_t* data, long n, int64_t target) {
  for (long i = 0; i < n; i++) {
    if (data[i] == target) return i;
  }
  return -1;
}

#ifdef SIMD_ARCH_X86
// Hand-written AVX2.  A 256-bit register holds four int64s.
long find_simd(const int64_t* data, long n, int64_t target) {
  const __m256i wanted = _mm256_set1_epi64x(target);
  long i = 0;

  // i + 4 <= n guarantees this load stays inside the array.
  for (; i + 4 <= n; i += 4) {
    __m256i v = _mm256_loadu_si256((const __m256i*)(data + i));
    __m256i eq = _mm256_cmpeq_epi64(v, wanted);

    // One bit per lane: bit k is set if lane k matched.
    int mask = _mm256_movemask_pd(_mm256_castsi256_pd(eq));
    if (mask) {
      // Lowest set bit = earliest matching lane, which is the first match.
      return i + __builtin_ctz(mask);
    }
  }

  // Fewer than four elements left over.
  for (; i < n; i++) {
    if (data[i] == target) return i;
  }
  return -1;
}
#elif defined(SIMD_ARCH_NEON)
// Hand-written NEON.  A 128-bit register holds two int64s.
long find_simd(const int64_t* data, long n, int64_t target) {
  const int64x2_t wanted = vdupq_n_s64(target);
  long i = 0;

  // i + 2 <= n guarantees this load stays inside the array.
  for (; i + 2 <= n; i += 2) {
    int64x2_t v = vld1q_s64(data + i);
    uint64x2_t eq = vceqq_s64(v, wanted);  // each lane: all-1s if equal, else 0

    // Only two lanes -- no cross-lane movemask needed, just check each.
    if (vgetq_lane_u64(eq, 0)) return i;
    if (vgetq_lane_u64(eq, 1)) return i + 1;
  }

  // Fewer than two elements left over.
  for (; i < n; i++) {
    if (data[i] == target) return i;
  }
  return -1;
}
#endif

#ifdef HAVE_HIGHWAY
// Portable SIMD.  hn::Find is the same algorithm as find_simd above -- whole
// vectors while they fit, then the remainder -- but written against Highway's
// abstract vector type instead of AVX2 intrinsics.
//
// ScalableTag<int64_t> means "however many int64 lanes this target has": four on
// AVX2, eight on AVX-512, two on SSE or NEON, and a length decided at runtime on
// SVE and RISC-V V.  Lanes(d) is not a constant, and the code does not care.
//
// The same source compiles for all of them.  Nothing here is x86.
long find_highway(const int64_t* data, long n, int64_t target) {
  const hn::ScalableTag<int64_t> d;
  size_t pos = hn::Find(d, target, data, (size_t)n);
  return pos == (size_t)n ? -1 : (long)pos;  // Find returns count when absent
}
#endif

// ---------------------------------------------------------------------------

typedef long (*FindFn)(const int64_t*, long, int64_t);

static double time_it(FindFn f, const int64_t* data, long n, int64_t target,
                      int reps) {
  auto t0 = std::chrono::steady_clock::now();
  volatile long sink = 0;
  for (int r = 0; r < reps; r++) {
    // Keeps the compiler from hoisting the call out of the timing loop.
    asm volatile("" ::"r"(data) : "memory");
    sink += f(data, n, target);
  }
  auto t1 = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(t1 - t0).count() / reps * 1e9;  // ns
}

int main() {
  const long n = 8192;  // 64 KiB of int64, fits in L1
  const int64_t target = 7;
  const int reps = 20000;

  std::vector<int64_t> data((size_t)n);
  std::mt19937_64 rng(42);
  for (long i = 0; i < n; i++) data[(size_t)i] = (int64_t)(rng() % 1000000) + 1000;

  printf("%ld int64 values, searching for the first match\n", n);
#ifdef HAVE_HIGHWAY
  printf("highway target: %s, %zu lanes per vector\n\n",
         hwy::TargetName(HWY_TARGET), hn::Lanes(hn::ScalableTag<int64_t>()));
#else
  printf("(built without highway)\n\n");
#endif
#if defined(HAVE_SIMD_INTRINSICS) && defined(HAVE_HIGHWAY)
  printf("  %-10s %10s %10s %10s %9s %9s\n", "match at", "scalar", "simd",
         "highway", "simd", "highway");
#elif defined(HAVE_SIMD_INTRINSICS)
  printf("  %-10s %10s %10s %9s\n", "match at", "scalar", "simd", "simd");
#elif defined(HAVE_HIGHWAY)
  printf("  %-10s %10s %10s %9s\n", "match at", "scalar", "highway", "highway");
#else
  printf("  %-10s %10s\n", "match at", "scalar");
#endif

  const long positions[] = {0, 16, 256, 4096, 8191, -1};  // -1 == no match

  for (long p : positions) {
    std::vector<int64_t> probe = data;
    if (p >= 0) probe[(size_t)p] = target;

    // All versions must agree on every case, present or absent.
    long a = find_scalar(probe.data(), n, target);
#ifdef HAVE_SIMD_INTRINSICS
    long b = find_simd(probe.data(), n, target);
    if (a != b) {
      printf("  MISMATCH: scalar %ld, simd %ld\n", a, b);
      return 1;
    }
#endif
#ifdef HAVE_HIGHWAY
    long c = find_highway(probe.data(), n, target);
    if (a != c) {
      printf("  MISMATCH: scalar %ld, highway %ld\n", a, c);
      return 1;
    }
#endif

    double s = time_it(find_scalar, probe.data(), n, target, reps);
#ifdef HAVE_SIMD_INTRINSICS
    double v = time_it(find_simd, probe.data(), n, target, reps);
#endif
#ifdef HAVE_HIGHWAY
    double h = time_it(find_highway, probe.data(), n, target, reps);
#endif

    char label[24];
    if (p < 0) snprintf(label, sizeof label, "none");
    else snprintf(label, sizeof label, "%ld", p);

#if defined(HAVE_SIMD_INTRINSICS) && defined(HAVE_HIGHWAY)
    printf("  %-10s %9.1f %9.1f %9.1f %8.2fx %8.2fx\n", label, s, v, h, s / v,
           s / h);
#elif defined(HAVE_SIMD_INTRINSICS)
    printf("  %-10s %9.1f %9.1f %8.2fx\n", label, s, v, s / v);
#elif defined(HAVE_HIGHWAY)
    printf("  %-10s %9.1f %9.1f %8.2fx\n", label, s, h, s / h);
#else
    printf("  %-10s %9.1f\n", label, s);
#endif
  }
  return 0;
}
