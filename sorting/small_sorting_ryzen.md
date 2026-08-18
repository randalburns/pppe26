# Small-Array Sorting at n=16 — Highway vs. hand-written AVX on Ryzen

Measured on AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu 24.04,
GCC 13.3 and clang 18.1.3, Google Highway 1.0.7, best of 7 trials, process pinned to one
core with `taskset`. See [small_sorting.md](small_sorting.md) for the Apple M5 / NEON
baseline this compares against.

The M5 result being retested: **hand-written NEON beat portable Highway by ~1.3x** on a
16-element bitonic network. This document asks whether the same holds for hand-written AVX
on x86.

Short answer: **it depends entirely on the compiler.** Under GCC, Highway loses by 1.24x,
reproducing the M5 finding. Under clang, the gap vanishes completely — Highway ties the
hand-written intrinsics. Neither compiler closes the separate 1.5x gap to a 256-bit-wide
formulation, which no `FixedTag<int32_t, 4>` Highway code can reach.

---

## Design

[sort_compare_n16_x86.cpp](sort_compare_n16_x86.cpp) is the x86 port of
[sort_compare_n16.cpp](sort_compare_n16.cpp). The scalar implementations (`std::sort`,
quicksort, insertion sort, bitonic scalar) and the benchmark harness are unchanged. The
Highway implementation is copied verbatim except for `Load`/`Store` → `LoadU`/`StoreU`
(the M5 version relied on `std::vector` data happening to be 16-byte aligned). The NEON
implementation is replaced by two AVX versions:

**`bitonic AVX2 (4x128b)`** — a mechanical, one-for-one translation of the NEON code, and
the fair structural peer of the Highway version, since `hn::FixedTag<int32_t, 4>` pins
Highway to 128-bit vectors on every target:

| NEON | AVX2 |
|---|---|
| `vminq_s32` / `vmaxq_s32` | `_mm_min_epi32` / `_mm_max_epi32` |
| `vrev64q_s32(v)` | `_mm_shuffle_epi32(v, 0xB1)` |
| `vextq_s32(v, v, 2)` | `_mm_shuffle_epi32(v, 0x4E)` |
| `vbslq_s32(sel, lo, hi)` | `_mm_blend_epi32(hi, lo, imm)` |

The NEON lane-select mask vectors become blend immediates, so the five `alignas(16)`
constant arrays disappear entirely — on x86 the selection is encoded in the instruction.

**`bitonic AVX2 (2x256b)`** — the formulation you would actually write on x86: all 16
`int32` in two `__m256i` registers. Rather than transcribe a network, the standard bitonic
recurrence is used directly (stages `k = 2,4,8,16`, passes at distance `d = k/2 … 1`;
element `g` keeps the min when `((g & d) == 0) XOR ((g & k) != 0)`). That predicate
collapses to one 8-bit blend immediate per register per step, computed offline. Distance 8
is the only cross-register step and occurs once, in the final all-ascending stage, so it
costs a bare min/max pair with no shuffle at all.

---

## The build-flag trap

Highway's *static* target is selected from the `-m` flags, **not** from the host CPU. A
plain `-mavx2` silently produces the **SSSE3** target, because `HWY_BASELINE_SSE4` requires
AES and PCLMUL in addition to SSE4.1, and `HWY_BASELINE_AVX2` is gated on SSE4 being
enabled first:

```c
// hwy/detect_targets.h
#if HWY_ARCH_X86 && (HWY_WANT_SSE4 || (HWY_CHECK_SSE4 && HWY_CHECK_PCLMUL_AES))
#define HWY_BASELINE_SSE4 HWY_SSE4
...
#if HWY_BASELINE_AVX2 != 0 && HWY_CHECK_BMI2_FMA && HWY_CHECK_F16C && defined(__AVX2__)
```

SSSE3 has no `pminsd`/`pmaxsd` — integer min/max on `int32` gets emulated with
compare-and-blend — so the benchmark reports Highway at 58 ns instead of 26 ns and the
"portable SIMD is slow" conclusion writes itself, incorrectly. The minimum flag set that
actually selects `HWY_AVX2` is:

```
-mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul
```

Every number below is from a build that prints its target, and the harness prints
`hwy::TargetName(HWY_TARGET)` next to the correctness check for exactly this reason.

---

## Results

Times are per 16-element sort, and include the ~16-element input copy the harness performs
before each call, so all rows carry the same fixed overhead.

### GCC 13.3, `-O2`

| Algorithm | AVX2 target | AVX3_DL target (`-march=native`) | ns/elem |
|---|---:|---:|---:|
| **bitonic AVX2 (2x256b)** | **14 ns** | **14 ns** | **0.9** |
| bitonic AVX2 (4x128b) | 21 ns | 21 ns | 1.3 |
| bitonic Highway | 26 ns | 26 ns | 1.6 |
| bitonic scalar (80 CAS) | 33 ns | 28 ns | 1.8 |
| insertion sort | 55 ns | 55 ns | 3.4 |
| quicksort (median-of-3) | 58 ns | 58 ns | 3.6 |
| std::sort (introsort) | 63 ns | 64 ns | 4.0 |

### clang 18.1.3, `-O2 -stdlib=libc++`

| Algorithm | AVX2 target | AVX3_DL target (`-march=native`) | ns/elem |
|---|---:|---:|---:|
| **bitonic AVX2 (2x256b)** | **14 ns** | **14 ns** | **0.9** |
| bitonic AVX2 (4x128b) | 22 ns | 22 ns | 1.4 |
| bitonic Highway | 22 ns | 23 ns | 1.4 |
| std::sort (introsort) | 44 ns | 45 ns | 2.8 |
| insertion sort | 46 ns | 46 ns | 2.9 |
| bitonic scalar (80 CAS) | 54 ns | 54 ns | 3.4 |
| quicksort (median-of-3) | 64 ns | 68 ns | 4.0 |

### The headline comparison

| | M5 (NEON) | Ryzen, GCC | Ryzen, clang |
|---|---:|---:|---:|
| hand-written SIMD, 128-bit | 19 ns | 21 ns | 22 ns |
| Highway, 128-bit | 24 ns | 26 ns | 22 ns |
| **hand-written advantage** | **1.26x** | **1.24x** | **1.00x** |
| hand-written SIMD, 256-bit | n/a | 14 ns | 14 ns |

---

## Why GCC loses and clang does not

Same source, same target, same machine — the difference is entirely in how the two
compilers lower Highway's half-vector idiom. Instruction mix for the three 10-step
networks:

| | hand AVX2 128b | Highway (GCC) | Highway (clang) | hand AVX2 256b |
|---|---:|---:|---:|---:|
| `vpminsd` | 34 | 34 | 34 | 19 |
| `vpmaxsd` | 34 | 34 | 34 | 19 |
| `vpshufd` | 28 | 16 | 32 | 14 |
| `vpblendd` / `vblendps` | 28 | 20 | 24 | 18 |
| `vpunpcklqdq` / `vpunpckhqdq` | — | 32 | 4 | — |
| `vpalignr` | — | 12 | — | — |
| `vperm2i128` / `vpermq` | — | — | — | 4 |
| **total instructions** | **136** | **159** | **142** | **83** |

All three 128-bit versions issue exactly 34 `vpminsd` and 34 `vpmaxsd` — the comparator
work is identical, as it must be for the same network. The entire difference is in the
permute-and-select domain: 56 such instructions by hand, 80 for Highway under GCC, 60 for
Highway under clang.

The culprit is the same construct the M5 write-up identified. Highway expresses "lower half
from one result, upper half from another" as
`Combine(d4, UpperHalf(d2, x), LowerHalf(y))`, which the hand-written version does with a
single `vpblendd`. GCC lowers each of those triples literally into a
`vpunpcklqdq`/`vpunpckhqdq` pair, and 16 uses become 32 extra shuffle-port instructions.
Clang recognizes the pattern and folds it back into `vpblendd`/`vpshufd`, leaving only 4
`vpunpcklqdq` behind — which is why Highway ties on clang and loses by 1.24x on GCC.

Note that the 17% instruction-count gap under GCC (159 vs 136) produces a 24% time gap.
`vpblendd` runs on multiple ports, while the unpack instructions contend for the shuffle
unit, so the extra instructions are worse than average for throughput. The bitonic network
is a pure dependency chain with no memory traffic to hide behind.

**Width beats hand-tuning.** The 256-bit version wins by 1.5x over every 128-bit
implementation, and no amount of intrinsic-level tuning closes that. It executes 83
instructions to the same 136 — 8 comparators per instruction instead of 4, plus the
distance-8 step costing a bare min/max pair. This is the ceiling `FixedTag<int32_t, 4>`
cannot reach: it is a property of the algorithm's data layout, not of the SIMD dialect.
Highway can express the 256-bit formulation, but only by rewriting the network for
`ScalableTag`, at which point the code is no longer target-independent in its *structure*.

---

## Comparison to Apple M5

**The M5 conclusion replicates on x86 — under GCC.** 1.24x on Ryzen vs. 1.26x on M5 is a
close match, and for the same mechanism: `Combine`/`UpperHalf`/`LowerHalf` lowering to
multiple instructions where the native ISA has a single-instruction select (`vbslq_s32` on
NEON, `vpblendd` on AVX2). The M5 write-up attributed the gap to the abstraction; more
precisely, it is the abstraction *plus* a compiler that does not fold it away.

**The absolute times are close across the two machines** — 19 vs. 21 ns for hand-written
128-bit SIMD, 24 vs. 26 ns for Highway. The M5 at 4.0 GHz and Zen 5 at ~5.1 GHz land within
10% of each other on a pure ALU/shuffle dependency chain, which is what a 16-element
bitonic network is.

**The scalar rankings do not replicate at all.** On M5, `std::sort` was the fastest scalar
option and quicksort the slowest. On Ryzen the ordering shifts with the compiler: under GCC
the bitonic scalar network is the fastest scalar code (33 ns) and `std::sort` the slowest
(63 ns); under clang, `std::sort` is fastest (44 ns) and the bitonic scalar network
collapses to 54 ns — a 1.6x *regression* from source GCC compiles to 33 ns.

The cause is instruction selection, and it is the opposite of what the M5 write-up's
"branch-free" framing would predict. From the same 80-CAS source:

| | GCC | clang |
|---|---:|---:|
| total instructions | 694 | 433 |
| `vpmaxsd` (SIMD min/max in scalar code) | 80 | 0 |
| `cmovl` | 0 | 160 |
| conditional branches | **80** | **0** |
| register moves | 438 | 179 |

Clang emits the textbook branchless form — 160 `cmov`, zero branches. GCC emits a hybrid:
it computes each max with a vector `vpmaxsd` but takes a *conditional branch* for the other
half of the CAS, 80 branches in total. The branchy version is the faster one here, 33 ns
vs. 54 ns.

That inversion is partly a harness artifact, and it is worth stating plainly because the
M5 numbers inherit it. The benchmark copies **the same 16-element array** before every
repetition, so those 80 branches resolve identically every time and the predictor learns
all of them. Re-running the scalar network against a rotating pool of 64 L1-resident random
arrays instead of one fixed array:

| | same input every rep | varying inputs |
|---|---:|---:|
| GCC (80 branches) | 29 ns | 41 ns |
| clang (branchless `cmov`) | 52 ns | 60 ns |

GCC's branchy code pays 1.41x on unpredictable input against clang's 1.15x, so the fixed
input flatters it — though not enough to flip the ordering; GCC's version is faster either
way. **This caveat does not touch the Highway-vs-AVX comparison above:** all three SIMD
kernels disassemble to zero conditional branches under both compilers, so their timings are
input-independent by construction. It does mean every *scalar* row in this document and in
[small_sorting.md](small_sorting.md) is a best case.

**Note on the M5 baseline.** [small_sorting.md](small_sorting.md) has a results table and a
prose section that disagree (table: NEON 19 ns, Highway 24 ns; prose: 13 ns and 17 ns). The
table is used above. The hand-vs-Highway *ratio* is 1.26x by the table and 1.31x by the
prose, so the comparison holds either way.

---

## Key Takeaways

**Portable SIMD costs 0-24% here, and the compiler decides which.** The same Highway source
on the same machine and target is 26 ns under GCC and 22 ns under clang. Benchmarking a
portability layer against hand-written intrinsics with one compiler measures that
compiler's pattern-matching as much as the library.

**Check the target Highway actually selected.** `-mavx2` alone yields the SSSE3 target and
a 2.2x pessimistic result. Print `hwy::TargetName(HWY_TARGET)` in any Highway benchmark;
the number is meaningless without it.

**Vector width is the decision that matters.** Choosing 256-bit over 128-bit is worth 1.5x
— more than the entire portable-vs-hand-written question. The instinct to reach for
intrinsics for the last 20% is less valuable than picking the right register width first,
and pinning to `FixedTag<int32_t, 4>` for portability forfeits the larger win.

**Pin the core.** At 14-60 ns per sort, an unpinned run of identical binaries varied by 2x
(insertion sort measured 27 ns and 55 ns; `std::sort` 64 ns and 119 ns) purely from
frequency ramp and core migration. Under `taskset -c 4` every number above reproduces to
±1 ns across runs.

**Sorting one fixed array measures a warm branch predictor.** The harness (inherited from
the M5 version) re-copies the same input every repetition, so data-dependent branches
become perfectly predictable and branchy scalar sorts read ~1.4x faster than they are. The
SIMD rows are branch-free and therefore unaffected, but scalar comparisons on this harness
are best-case numbers.

---

## Files

| File | Description |
|---|---|
| `sort_compare_n16_x86.cpp` | x86 port: scalar sorts + AVX2 128b + AVX2 256b + Highway |
| `Makefile` | Builds all four compiler/target combinations, `make run` to reproduce |
| `sort_compare_n16.cpp` | Original ARM NEON version (M5) |
