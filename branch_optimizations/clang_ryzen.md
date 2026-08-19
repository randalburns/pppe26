# Branch Optimizations under Clang — AMD Ryzen AI 9 HX 370

All three branch-optimization examples built with **Clang 18.1.3** on AMD Ryzen AI 9 HX 370
(Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu. No GCC results appear in this document.

These numbers use the **`KEEP_BRANCH()` sources**, which replaced the old `-fno-if-conversion`
build flag. That change is what makes a Clang-vs-Clang comparison possible at all — see
[Why the flag had to go](#why-the-flag-had-to-go).

**Status:** the Ryzen side is complete. The M5 side needs a re-run with the new sources; commands
are in [Completing the comparison](#completing-the-comparison).

---

## Build

```
clang++ --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/13 -O<n> -o <bin> <src>.cpp
```

`--gcc-install-dir` is needed only on this machine: Clang picks the newest GCC toolchain directory
it finds (`.../gcc/x86_64-linux-gnu/14`) but only `libstdc++-13-dev` is installed, so
`#include <iostream>` fails. Pinning to 13 fixes it and keeps Clang and GCC on the same standard
library. On the M5 no such flag is needed.

## Why the flag had to go

`branch_free.md` and `lut_branch.md` previously built with `g++-15 -O1 -fno-if-conversion`, to stop
the compiler turning the "branchy" version's `if` statements into branchless conditional moves.
Three things were wrong with that:

1. **Clang has no such flag.** `clang++: error: unknown argument: '-fno-if-conversion'`. The
   `-mllvm` phi-folding thresholds are accepted but inert — the if-conversion happens in LLVM's
   backend, below those knobs.
2. **It was a no-op at `-O1`, the level both docs specified.** Building `branch_free.cpp` with and
   without it under GCC 13 gives **byte-identical** code for `process_branchy`, `process_ternary`
   and `process_arith`. GCC does not if-convert these at `-O1` regardless.
3. **It did not survive `-O2`.** At higher levels the compiler reached the branchless form by other
   routes, and in `lut_branch` auto-vectorized the branchy path into a SIMD select that beat the
   hand-written LUT outright.

The replacement is a compiler barrier in the branch body:

```cpp
#define KEEP_BRANCH() asm volatile("" ::: "memory")

if (v < LO)  { KEEP_BRANCH(); v = LO; }
```

The optimizer may not speculate across it, so the branch cannot become a CMOV or fold into a
vectorized select. It emits no instructions and costs nothing at run time. In `lut_branch` the
barrier must sit in a **single-armed** `if`: with one in each arm of an `if`/`else`, Clang factors
the common barrier out and re-forms the CMOV.

Verified on both compilers at every level — `process_branchy` and `encode_branchy` keep real
conditional jumps, zero CMOV, zero packed SIMD, at `-O0` through `-O3`. The branchless functions
(`ternary`, `arith`, `encode_lut`) are **byte-identical** to before the patch, and all builds
report `Correctness: PASS` with unchanged checksums.

## Measurement protocol

Minimum across **3 separate invocations** of each binary, each an internal best-of-5 (min-of-15),
pinned with `taskset -c 2` (a 5.16 GHz Zen 5 core, not a 3.29 GHz Zen 5c core), run **rep-major** so
thermal drift biases all configurations equally.

This matters more than it sounds: the byte-identical committed binary produced
`200 → 152 → 149 → 153 ms` across four runs. Variance here is ~30%, not the ~10% the older docs
claim.

---

## 1. branch_free (N=32M bytes, random / sorted)

| level | branchy | ternary | arith | branchy→ternary |
|---|---:|---:|---:|---:|
| O0 | 176 / 28 ms | 151 / 43 ms | 55 / 55 ms | 1.17x |
| O1 | 147 / 13 ms | 61 / 14 ms | 17 / 17 ms | 2.41x |
| **O2** | **144 / 12 ms** | **14 / 14 ms** | 20 / 20 ms | **10.29x** |
| O3 | 147 / 11 ms | 14 / 14 ms | 20 / 20 ms | **10.50x** |

**The example now demonstrates what it claims, at every optimization level.** Branchy stays at
144–147 ms on random data from `-O1` up, while ternary drops to 14 ms — a 10.3–10.5x win that is
stable across `-O2` and `-O3` rather than collapsing.

**The data-pattern signature is back and unambiguous.** Branchy runs 144 ms on random data and
12 ms on sorted — a 12x swing driven purely by whether the predictor can learn the pattern. The
branchless versions show no such sensitivity (14/14, 20/20): identical time on random and sorted
data, which is the entire point of the technique.

**`arith` costs more than `ternary` from `-O2` on** (20 ms vs 14 ms). Both are branchless; the
explicit shift-and-mask arithmetic is simply more work than the CMOV the compiler generates from
plain ternaries. The hand-written bitmask is worth writing for SIMD lanes, GPUs, or in-order cores
— but on an out-of-order x86 core with a competent compiler, `?:` wins.

## 2. lut_branch (N=32M bytes)

| level | branchy | lut | speedup |
|---|---:|---:|---:|
| O0 | 208 ms | 37 ms | 5.62x |
| O1 | 155 ms | 10 ms | **15.50x** |
| O2 | 152 ms | 10 ms | **15.20x** |
| O3 | 157 ms | 10 ms | **15.70x** |

**The `-O3` inversion is gone.** Previously the compiler auto-vectorized the branchy path at `-O2`
and beat the LUT 3-to-1, making the example argue against its own thesis. With the branch held, the
LUT wins 15.2–15.7x consistently from `-O1` through `-O3`.

The LUT itself is flat at 10 ms regardless of optimization level — it is a load and a store, and
there is nothing for the optimizer to improve. All of the variation is on the branchy side.

## 3. loop_unswitch (N=32M floats, switched / unswitched)

Unmodified — this example never used `-fno-if-conversion` and needed no barrier.

| level | add | mul | abssum |
|---|---|---|---|
| O0 | 35 / 27 ms — 1.30x | 37 / 28 ms — 1.32x | 41 / 32 ms — 1.28x |
| O1 | 20 / 18 ms — 1.11x | 21 / 17 ms — 1.24x | 24 / 18 ms — 1.33x |
| **O2** | 19 / 16 ms — 1.19x | 19 / 16 ms — 1.19x | 23 / 16 ms — **1.44x** |
| O3 | 16 / 16 ms — 1.00x | 16 / 16 ms — 1.00x | 16 / 16 ms — 1.00x |

At `-O2` Clang vectorizes `apply_unswitched` while leaving `apply_switched` scalar — the
scalar-vs-SIMD mechanism [loop_unswitch.md](loop_unswitch.md) describes. At `-O3` it unswitches
automatically and the gap closes to exactly 1.00x, also as that doc predicts.

Against the M5's `-O2` figures (3.25x / 2.00x / 3.00x), the mechanism matches but the magnitude does
not, and the reason is **memory bandwidth**: both loops are bandwidth-bound over 384 MB, and the
unswitched variants converge to 16 ms here (≈24 GB/s single-thread) against M5's 4 ms (≈96 GB/s).
A 4x lower ceiling compresses the same transformation into a smaller win.

---

## Completing the comparison

Run on the M5 with Apple Clang, from the same commit:

```bash
for lvl in O0 O1 O2 O3; do
  clang++ -$lvl -o bf_$lvl  branch_free.cpp
  clang++ -$lvl -o lut_$lvl lut_branch.cpp
  clang++ -$lvl -o lu_$lvl  loop_unswitch.cpp
done
```

Run each binary **3 times and take the minimum** — a single run is not enough to separate these
effects from noise. No `-fno-if-conversion`: it no longer appears in any build line, and Clang
would reject it.

`loop_unswitch` should be re-run too. Its existing M5 numbers are Apple Clang and still valid, but
predate the min-of-3 protocol, so regenerating them puts both machines on equal footing.

---

## Key takeaways

1. **All three examples now work under Clang at every optimization level.** Replacing a fragile
   GCC-only flag with a compiler barrier turned two broken demonstrations (measuring 0.93x and an
   inverted 0.42x) into stable ones (10.3x and 15.2x).

2. **`branch_free` shows a 12x random-vs-sorted swing on the same code** — 144 ms vs 12 ms. That
   single comparison is the cleanest statement of the lesson in the whole set, and it was invisible
   before the fix.

3. **Hand-written bitmask arithmetic is not the fastest branchless form on this hardware.** `arith`
   (20 ms) loses to plain `?:` (14 ms) from `-O2` on. Write the ternary and let the compiler emit
   the CMOV, unless targeting SIMD lanes, GPUs, or in-order cores.

4. **`loop_unswitch`'s remaining M5 gap is hardware, not compiler.** Same Clang, same
   transformation, same instruction classes; ≈4x less single-thread memory bandwidth accounts for
   1.44x here vs 3.00x there.

5. **A build flag that "only affects `if`-statements" deserves verification.** `-fno-if-conversion`
   was doing nothing at the level it was used at, and the docs built three separate conclusions on
   top of it. Diffing the disassembly with and without takes one command.
