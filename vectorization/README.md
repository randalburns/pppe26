
Standalone, one file, no Makefile needed. Both a hand-written path and
Highway build on either chip — AVX2 on x86, NEON on arm64 — picked at compile
time by `__x86_64__`/`__i386__` vs `__aarch64__`/`__arm__`:

```
# x86 (Zen 5, AVX2) — scalar, hand-written AVX2, and Highway
clang++ -O3 -mavx2 -std=c++17 -stdlib=libc++ find_first.cpp -o find_first -lhwy

# arm64 (Apple M5) — scalar, hand-written NEON, and Highway
clang++ -O3 -std=c++17 -stdlib=libc++ find_first.cpp -o find_first -lhwy

./find_first
```

[find_first_diagram.html](find_first_diagram.html) draws the computation: the
scalar and vector scans side by side, what happens inside one vector step, and
why the vectorizer refuses this loop. Open it in a browser.

---

# Vectorization: a loop the compiler will not do for you

Auto-vectorization handles most simple loops well enough that hand-written
SIMD is usually not worth it. This is one of the cases where it is, and the
reason has nothing to do with being clever about the data.

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
$ clang++ -O3 -Rpass-analysis=loop-vectorize -c find_first.cpp
remark: loop not vectorized: could not determine number of loop iterations
```

A vectorizer plans the loop before running it: process `n/k` whole vectors,
then clean up the remainder. That plan needs the trip count up front. This
loop exits when it finds something, so the trip count depends on the data
and there is no plan to make. The compiler emits plain scalar code —
`cmp` / `je`, one element per iteration, no vector instructions at any
optimization level.

## Why this one counts

By hand it is straightforward. On x86, compare four at a time with AVX2,
stop at the first vector that contains a match, then find which lane:

```c
__m256i eq = _mm256_cmpeq_epi64(v, wanted);
int mask = _mm256_movemask_pd(_mm256_castsi256_pd(eq));   // one bit per lane
if (mask) return i + __builtin_ctz(mask);                 // lowest set bit wins
```

Done correctly it never loads outside the array — the loop condition is
`i + 4 <= n`, and the leftovers are handled scalar. It returns the same
index as the scalar version for every input, and `-1` in the same cases —
it assumes nothing about `n`, nothing about the values, and nothing about
how many matches exist.

NEON has no direct equivalent of `movemask`, so the arm64 version checks its
two lanes directly instead of building a bitmask:

```c
uint64x2_t eq = vceqq_s64(v, wanted);           // each lane: all-1s or 0
if (vgetq_lane_u64(eq, 0)) return i;
if (vgetq_lane_u64(eq, 1)) return i + 1;
```

Same contract, same guarantees — just two lanes instead of four, and two
branches instead of one `ctz`.

So this is not the usual SIMD trade, where you go faster by quietly
accepting a restriction the compiler was obliged to reject. The compiler is
not declining to vectorize this loop because it doubts you. It is declining
because its vectorizer is built around a trip count, and this loop does not
have one.

## Results: hand-written AVX2

8192 `int64` values (64 KiB, L1-resident), Ryzen AI 9 HX 370, clang 18,
`-O3 -mavx2`:

| first match at | scalar | simd | speedup |
|---|---:|---:|---:|
| 0 | 0.6 ns | 1.3 ns | 0.46x |
| 16 | 4.2 ns | 2.1 ns | 1.97x |
| 256 | 55.1 ns | 19.1 ns | 2.88x |
| 4096 | 702.7 ns | 204.8 ns | 3.43x |
| 8191 | 1160.3 ns | 524.3 ns | 2.21x |
| no match | 956.9 ns | 421.4 ns | 2.27x |

The crossover is about a dozen elements. If the match is at index 0 the
scalar version wins outright — it compares once and returns, while the
vector version still loads and tests a whole register. Past that, 2–3.5x.
This holds up across `-O2`, `-O3`, `-march=native`, and gcc — not an
artifact of one flag combination. But it only exists on x86: there is no
`<immintrin.h>` on arm64, so this hand-written path has no equivalent on
the M5 build at all.

---

# The fix: Highway

Hand-writing intrinsics for one instruction set means the routine only runs
on that architecture. [Google Highway](https://github.com/google/highway)
is a portable SIMD library — the same source compiles to NEON, SVE, AVX2,
AVX-512, and RISC-V V. It is already used in [../sorting/](../sorting/).

It ships this exact algorithm:

```c
long find_highway(const int64_t* data, long n, int64_t target) {
  const hn::ScalableTag<int64_t> d;
  size_t pos = hn::Find(d, target, data, (size_t)n);
  return pos == (size_t)n ? -1 : (long)pos;   // Find returns count when absent
}
```

`hn::Find` is the same loop shape as the hand-written version — whole
vectors while they fit, then the remainder — written against an abstract
vector type. The interesting declaration is `ScalableTag<int64_t>`:
*however many int64 lanes this target has*. Two on NEON, four on AVX2, eight
on AVX-512, and on SVE or RISC-V V a width that is not known until the
program runs. `Lanes(d)` is not a compile-time constant, and the code never
needs it to be.

## What Highway costs on Zen

Nothing measurable. Median of 7 pinned runs, same machine and flags as above:

| first match at | scalar | simd | highway | simd | highway |
|---|---:|---:|---:|---:|---:|
| 0 | 0.5 ns | 0.9 ns | 1.0 ns | 0.56x | 0.50x |
| 16 | 6.8 ns | 1.8 ns | 1.8 ns | 3.78x | 3.78x |
| 256 | 60.6 ns | 25.3 ns | 26.6 ns | 2.40x | 2.28x |
| 4096 | 909.1 ns | 329.9 ns | 312.6 ns | 2.76x | 2.91x |
| 8191 | 1596.1 ns | 773.9 ns | 698.5 ns | 2.06x | 2.29x |
| none | 1318.8 ns | 666.0 ns | 651.1 ns | 1.98x | 2.03x |

The 256 row is short enough to be noisy — it lands on either side of the
hand-written version between runs. Everywhere else the two are the same
speed. The generated inner loops explain why. Hand-written:

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

The abstraction compiles away completely — Highway is a set of thin inline
wrappers over the platform's native vector instructions, so `ScalableTag`
compiles down to exactly the instructions a hand-written version would use.
The portability is paid for at build time and in a dependency, not in
cycles. It also writes the fiddly parts for you: remainder handling, and
turning a comparison mask into a lane index — the two places a hand-written
version is easiest to get wrong.

## Same source, two chips: Zen vs M5

Both `find_simd` and `find_highway` now build on either chip — AVX2 by hand
and Highway/AVX2 on the Ryzen, NEON by hand and Highway/NEON on the M5:

```
$ clang++ -O3 -std=c++17 find_first.cpp -o find_first -lhwy   # on the M5
highway target: NEON, 2 lanes per vector
```

Two `int64` lanes instead of four, because NEON registers are 128 bits, half
the width of AVX2's 256. Median of several pinned runs on each machine, same
source, same `n = 8192`, `clang -O3`, all times in ns:

| first match at | Zen scalar | Zen simd | Zen highway | M5 scalar | M5 simd | M5 highway |
|---|---:|---:|---:|---:|---:|---:|
| 0 | 0.5 | 0.9 | 1.0 | 0.2 | 0.3 | 0.5 |
| 16 | 6.8 | 1.8 | 1.8 | 4.5 | 3.0 | 2.4 |
| 256 | 60.6 | 25.3 | 26.6 | 66.0 | 42.7 | 41.7 |
| 4096 | 909.1 | 329.9 | 312.6 | 931.9 | 639.4 | 536.9 |
| 8191 | 1596.1 | 773.9 | 698.5 | 1856.9 | 1241.2 | 1043.2 |
| no match | 1318.8 | 666.0 | 651.1 | 1855.9 | 1244.5 | 1076.1 |

| first match at | Zen simd speedup | Zen highway speedup | M5 simd speedup | M5 highway speedup |
|---|---:|---:|---:|---:|
| 0 | 0.56x | 0.50x | 0.67x | 0.40x |
| 16 | 3.78x | 3.78x | 1.50x | 1.88x |
| 256 | 2.40x | 2.28x | 1.55x | 1.58x |
| 4096 | 2.76x | 2.91x | 1.46x | 1.74x |
| 8191 | 2.06x | 2.29x | 1.50x | 1.78x |
| no match | 1.98x | 2.03x | 1.49x | 1.72x |

Three things stand out. First, the scalar columns are close — same
algorithm, same `n`, and neither chip is starved for single-element
compares, so per-element scalar cost is in the same ballpark on both.

Second, the speedup ceiling is lower on the M5 because it is bounded by lane
count: doubling the work per instruction buys less than quadrupling it does.
AVX2's four lanes pull 2.0–3.8x out of this loop past the first few
elements; NEON's two lanes pull 1.5–1.9x. Same portable source, same
algorithm — the ceiling is set by the width of the register.

Third, and unexpected: on Zen, hand-written and Highway are the same speed
(the asm above shows why — they compile to the same instructions), but on
the M5, Highway beats the hand-written version at every row past index 0,
by 5–19%. AVX2 has `movemask`, so both versions turn a lane comparison into
one bitmask and one `ctz`. NEON has nothing equivalent, so `find_simd` above
falls back to checking each of its two lanes with a branch. Highway's NEON
`FindFirstTrue` does better: it packs the comparison mask into 4-bit
"nibbles" and finds the lane with a single trailing-zero count — no
data-dependent branch per lane (see `FindFirstTrue` in Highway's
`arm_neon-inl.h`). The straightforward hand-written port loses to the
library here, for the same reason it usually doesn't on x86: x86 has an
instruction the naive approach can use directly, and NEON does not.

The index-0 row is the exception on both chips, and for the same structural
reason: a matched first element makes the scalar loop return on the very
first compare, while the vector path always pays for one load-and-test of a
full register before it can check anything. Narrower or wider, that setup
cost does not go away.

## One trap

Highway picks its instruction set from the `-m` flags, not from the CPU it
is running on. `-mavx2` alone is **not** enough to select the AVX2 target —
that target also requires BMI2, FMA, and AES — so a bare `-mavx2` silently
falls back to SSSE3 and runs at half width:

```
$ clang++ -O3 -mavx2 ... && ./find_first
highway target: SSSE3, 2 lanes per vector      <-- half the lanes

$ clang++ -O3 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul ... && ./find_first
highway target: AVX2, 4 lanes per vector
```

The program prints the selected target for this reason. If you benchmark
Highway against hand-written AVX2 without checking this line, Highway looks
about twice as slow as it is.

The simpler fix is to stop listing individual `-m` flags and name a concrete
CPU instead — `-march=<cpu>` sets every flag Highway checks for in one shot:

```
$ clang++ -O3 -march=znver5 ... && ./find_first    # this machine
highway target: AVX2, 4 lanes per vector

$ clang++ -O3 -march=native ... && ./find_first    # any machine
highway target: AVX2, 4 lanes per vector
```

Two flags that look like they should work do not. `-march=x86-64-v3` — the
generic "AVX2-generation" portability level — omits AES and PCLMUL, so it
still falls back to SSSE3. So, surprisingly, does `-march=haswell`: real
Haswell silicon has AES-NI, but clang's target definition for it does not
set `__AES__`. Verified by dumping predefined macros for each target
(`clang++ -march=<x> -dM -E -x c++ /dev/null`) rather than trusting the
flag's name — only a concrete, current CPU model reliably sets all six.
