**RB**

Zen5 only.

Standalone, one file, no Makefile needed:

```
clang++ -O3 -mavx2 -std=c++17 -stdlib=libc++ find_first.cpp -o find_first
./find_first
```

[find_first_diagram.html](find_first_diagram.html) draws the computation: the
scalar and vector scans side by side, what happens inside one vector step, and
why the vectorizer refuses this loop. Open it in a browser.

---

# Vectorization: a loop the compiler will not do for you

Auto-vectorization handles most simple loops well enough that hand-written
intrinsics are usually not worth it. This is one of the cases where they are,
and the reason has nothing to do with being clever about the data.

Find the index of the first element equal to a target:

```c
long find_first(const int64_t* data, long n, int64_t target) {
    for (long i = 0; i < n; i++) {
        if (data[i] == target) return i;
    }
    return -1;
}
```

Ask the compiler why it did not vectorize this and it tells you:

```
$ clang++ -O3 -mavx2 -Rpass-analysis=loop-vectorize -c find_first.cpp
remark: loop not vectorized: could not determine number of loop iterations
```

A vectorizer plans the loop before running it: process `n/4` whole vectors, then
clean up the remainder. That plan needs the trip count up front. This loop exits
when it finds something, so the trip count depends on the data and there is no
plan to make. The compiler emits plain scalar code — `cmp` / `je`, one element
per iteration, no vector instructions at any optimization level.

By hand it is straightforward. Compare four at a time, stop at the first vector
that contains a match, then find which lane:

```c
__m256i eq = _mm256_cmpeq_epi64(v, wanted);
int mask = _mm256_movemask_pd(_mm256_castsi256_pd(eq));   // one bit per lane
if (mask) return i + __builtin_ctz(mask);                 // lowest set bit wins
```

## Why this one counts

The interesting part is what the hand-written version does *not* do. It never
loads outside the array — the loop condition is `i + 4 <= n`, and the leftovers
are handled scalar. It returns the same index as the scalar version for every
input, and `-1` in the same cases. It assumes nothing about `n`, nothing about
the values, and nothing about how many matches exist.

So this is not the usual intrinsics trade, where you go faster by quietly
accepting a restriction the compiler was obliged to reject. The compiler is not
declining to vectorize this loop because it doubts you. It is declining because
its vectorizer is built around a trip count, and this loop does not have one.

## Results

8192 int64 values (64 KiB, L1-resident), Ryzen AI 9 HX 370, clang 18, `-O3 -mavx2`:

| first match at | scalar | simd | speedup |
|---|---:|---:|---:|
| 0 | 0.6 ns | 1.3 ns | 0.46x |
| 16 | 4.2 ns | 2.1 ns | 1.97x |
| 256 | 55.1 ns | 19.1 ns | 2.88x |
| 4096 | 702.7 ns | 204.8 ns | 3.43x |
| 8191 | 1160.3 ns | 524.3 ns | 2.21x |
| no match | 956.9 ns | 421.4 ns | 2.27x |

The crossover is about a dozen elements. If the match is at index 0 the scalar
version wins outright — it compares once and returns, while the vector version
still loads and tests a whole register. Past that, 2-3.5x.

This holds up across `-O2`, `-O3`, `-march=native`, and gcc; it is not an
artifact of one flag combination. It is worth checking that, because plenty of
"intrinsics beat the compiler" results evaporate the moment you let the compiler
target a newer instruction set.

---

# A third option: Highway

Writing AVX2 intrinsics means the routine only runs on x86 with AVX2.
[Google Highway](https://github.com/google/highway) is a portable SIMD library —
the same source compiles to AVX2, AVX-512, NEON, SVE, and RISC-V V. It is already
used in [../sorting/](../sorting/).

It ships this exact algorithm:

```c
long find_highway(const int64_t* data, long n, int64_t target) {
  const hn::ScalableTag<int64_t> d;
  size_t pos = hn::Find(d, target, data, (size_t)n);
  return pos == (size_t)n ? -1 : (long)pos;   // Find returns count when absent
}
```

`hn::Find` is the same loop as the hand-written version — whole vectors while
they fit, then the remainder — written against an abstract vector type. The
interesting declaration is `ScalableTag<int64_t>`: *however many int64 lanes this
target has*. Four on AVX2, eight on AVX-512, two on NEON, and on SVE or RISC-V V
a width that is not known until the program runs. `Lanes(d)` is not a compile-time
constant, and the code never needs it to be.

## What it costs

Nothing measurable. Median of 7 pinned runs, same machine and flags:

| first match at | scalar | simd | highway | simd | highway |
|---|---:|---:|---:|---:|---:|
| 0 | 0.5 ns | 0.9 ns | 1.0 ns | 0.56x | 0.50x |
| 16 | 6.8 ns | 1.8 ns | 1.8 ns | 3.78x | 3.78x |
| 256 | 60.6 ns | 25.3 ns | 26.6 ns | 2.40x | 2.28x |
| 4096 | 909.1 ns | 329.9 ns | 312.6 ns | 2.76x | 2.91x |
| 8191 | 1596.1 ns | 773.9 ns | 698.5 ns | 2.06x | 2.29x |
| none | 1318.8 ns | 666.0 ns | 651.1 ns | 1.98x | 2.03x |

The 256 row is short enough to be noisy — it lands on either side of the
hand-written version between runs. Everywhere else the two are the same speed.

The generated inner loops explain why. Hand-written:

```asm
vpcmpeqq  ymm1, ymm0, ymmword ptr [rdi + 8*rcx]
vmovmskpd r8d, ymm1
test      r8d, r8d
je        .LBB1_1
```

Highway, from portable source:

```asm
vpcmpeqq  ymm2, ymm1, ymmword ptr [rdi + 8*rcx - 32]
vmovmskpd edx, ymm2
test      edx, edx
je        .LBB2_1
```

The abstraction compiles away completely. Highway is a set of thin inline wrappers
over the same instructions, so the portability is free at run time — you pay for
it at build time and in a dependency, not in cycles.

It also writes the fiddly parts for you: the remainder handling, and turning a
comparison mask into a lane index. Those are the two places the hand-written
version is easiest to get wrong.

## One trap

Highway picks its instruction set from the `-m` flags, not from the CPU it is
running on. `-mavx2` alone is **not** enough to select the AVX2 target — that
target also requires BMI2, FMA, and AES — so a bare `-mavx2` silently falls back
to SSSE3 and runs at half width:

```
$ clang++ -O3 -mavx2 ... && ./find_first
highway target: SSSE3, 2 lanes per vector      <-- half the lanes

$ clang++ -O3 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul ... && ./find_first
highway target: AVX2, 4 lanes per vector
```

The program prints the selected target for this reason. If you benchmark Highway
against hand-written AVX2 without checking this line, Highway looks about twice
as slow as it is. (Same flag set, and the same reason, as the note at the top of
[../sorting/Makefile](../sorting/Makefile).)
