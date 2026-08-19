# Branch Optimizations under Clang — AMD Ryzen AI 9 HX 370

All three branch-optimization examples built and run with **Clang 18.1.3** on AMD Ryzen AI 9 HX 370
(Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu. No GCC results appear in this document.

The purpose is a **Clang-vs-Clang comparison against the Apple M5**, holding the compiler family
constant so that any remaining difference is attributable to hardware rather than toolchain.

> **Scope warning — read before using the M5 comparisons below.**
> Only one of the three M5 baselines was produced by Clang. [branch_free.md](branch_free.md) and
> [lut_branch.md](lut_branch.md) both build with `g++-15` (real Homebrew GCC on macOS), not Apple
> Clang; only [loop_unswitch.md](loop_unswitch.md) builds with `g++`, which on macOS is Apple Clang.
> Those two M5 numbers are therefore **GCC results and have been excluded**, which leaves those
> examples with no M5 side to compare against yet. The commands to generate them are in
> [Completing the comparison](#completing-the-comparison). Only `loop_unswitch` has a genuine
> Clang-vs-Clang result today.

---

## Build

```
clang++ --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/13 -O<n> -o <bin> <src>.cpp
```

`--gcc-install-dir` is required on this machine: Clang picks the newest GCC toolchain directory it
finds (`.../gcc/x86_64-linux-gnu/14`) but only `libstdc++-13-dev` is installed, so `#include
<iostream>` fails. Pinning to 13 fixes it. (`-stdlib=libc++` also compiles but swaps the standard
library as well; pinning keeps it stock.)

No source changes were needed — the `(long long)` cast on `duration_cast<...>::count()` is already
committed, and Clang requires it for the same libstdc++ template-deduction reason.

### `-fno-if-conversion` does not exist in Clang

Both `branch_free.md` and `lut_branch.md` specify `-fno-if-conversion`. Clang rejects it outright:

```
clang++: error: unknown argument: '-fno-if-conversion'
```

There is no equivalent. `-mllvm -two-entry-phi-node-folding-threshold=0` and
`-mllvm -phi-node-folding-threshold=0` are both genuinely accepted (verified: a deliberately bogus
`-mllvm` option errors, these do not) but change nothing — the if-conversion happens in the LLVM
backend, below the SimplifyCFG thresholds those knobs control.

This is the finding, not a workaround failure. [branch_free.md](branch_free.md) already states it
for Apple Clang — *"Apple Clang always converts simple `if`/ternary to CSEL at `-O1`, so GCC is
required to observe the full branchy penalty."* Linux Clang on x86 behaves the same way. **Under
any Clang, at any `-O` level, the version named "branchy" does not contain the branches it is named
for.** Two of these three examples are GCC-only demonstrations.

---

## Measurement protocol

The existing docs report results "stable within ~10%". That understates the noise here. Running the
byte-identical committed `branch_free_ryzen` binary four times, pinned to one core:

```
200 ms  →  152 ms  →  149 ms  →  153 ms
```

±30%, on a `powersave` governor and a thermally-constrained laptop SoC.

All numbers below are the **minimum across 3 separate invocations** of each binary, each already an
internal best-of-5 (so min-of-15), pinned with `taskset -c 2` (verified a 5.16 GHz Zen 5 core, not
a 3.29 GHz Zen 5c core), run **rep-major** — every configuration once per round — so thermal drift
biases all configurations equally rather than penalizing whichever ran last.

All builds report `Correctness: PASS`.

---

## 1. branch_free (Clang, N=32M bytes)

| level | branchy | ternary | arith | branchy→ternary | branchy→arith |
|---|---:|---:|---:|---:|---:|
| O0 | 191 / 33 ms | 164 / 52 ms | 65 / 65 ms | 1.16x | 2.94x |
| **O1** | **64 / 15 ms** | **69 / 15 ms** | **19 / 19 ms** | **0.93x** | 3.37x |
| O2 | 15 / 14 ms | 15 / 15 ms | 21 / 21 ms | 1.00x | 0.71x |
| O3 | 16 / 15 ms | 16 / 15 ms | 23 / 22 ms | 1.00x | 0.70x |

(random / sorted)

**The example's central comparison does not exist under Clang.** At `-O1`, branchy (64 ms) is
marginally *slower* than ternary (69 ms) — 0.93x. Disassembly shows the two functions compile to
near-identical instruction sequences, differing only in the scheduling of one `xor` and a `jmp`.
Clang if-converts two of the three `if`-statements: the clamp-LO becomes `cmovb`, and
`count += (v > MID)` becomes `cmp` + `sbb` (borrow-based conditional increment). Only the clamp-HI
survives as a real branch.

At `-O2`/`-O3` both are vectorized to 40 packed SSE instructions and converge at 15–16 ms.

**Branchless-vs-branchy is invisible; what remains visible is `arith` being the wrong choice.** The
hand-written bitmask version is the fastest variant at `-O0`/`-O1` (19 ms at `-O1`, 3.37x) but
becomes the *slowest* at `-O2`/`-O3` (21–23 ms vs 15–16 ms, i.e. 0.70x). The explicit shift-and-mask
arithmetic vectorizes worse than the plain source Clang is free to rewrite itself. The
hand-optimization stops paying and starts costing at exactly the level most people ship.

**Sorted-vs-random confirms there are no branches left.** From `-O1` up, every variant reports the
same time on sorted and random data (15/15, 16/15, 21/21). Data-pattern sensitivity — the entire
phenomenon the example demonstrates — is absent from `-O1` onward.

---

## 2. lut_branch (Clang, N=32M bytes)

| level | branchy | lut | speedup |
|---|---:|---:|---:|
| O0 | 205 ms | 43 ms | 4.77x |
| O1 | 33 ms | 12 ms | 2.75x |
| **O2** | **5 ms** | 12 ms | **0.42x — inverted** |
| O3 | 4 ms | 12 ms | 0.33x — inverted |

**The LUT is only a win at `-O0`/`-O1`, and loses badly from `-O2` on.** At `-O2` Clang
auto-vectorizes `encode_branchy` (`movdqu`, `pcmpeqb`, `pand`, `pandn`, `por`, `paddb`,
`punpcklbw` — 30 packed instructions), reinventing the arithmetic-bitmask technique from
`branch_free.cpp` on its own, vectorized, 16 bytes per iteration. The hand-written table cannot
compete: `encode_lut` never picks up a single packed instruction at any level, because
table-lookup-by-index does not auto-vectorize on this target (no AVX2 gather is emitted). It sits
at 12 ms from `-O1` onward regardless of optimization level.

Even the `-O1` 2.75x is not measuring what the example claims. `encode_branchy` there is already
branch-free — `lea`/`or`/`cmp`/`cmovb` — so the 33 ms vs 12 ms gap is scalar-select overhead plus
loop shape, not misprediction cost.

**The 12 ms LUT figure is worth noting for a different reason:** it is identical to the 12 ms the
M5 doc reports. A 16-byte L1-resident table is genuinely microarchitecture-independent — that
particular number transfers across ISAs even though the surrounding comparison does not.

---

## 3. loop_unswitch — the one genuine Clang-vs-Clang result

This is the only example whose M5 baseline was produced by a Clang (Apple Clang via `g++` on
macOS), so it is the only one where a cross-machine comparison is valid.

### Ryzen, Clang, all levels (switched / unswitched)

| level | add | mul | abssum |
|---|---|---|---|
| O0 | 40 / 35 ms — 1.14x | 47 / 36 ms — 1.31x | 50 / 41 ms — 1.22x |
| O1 | 21 / 18 ms — 1.17x | 21 / 17 ms — 1.24x | 25 / 19 ms — 1.32x |
| **O2** | **24 / 16 ms — 1.50x** | **21 / 17 ms — 1.24x** | **27 / 18 ms — 1.50x** |
| O3 | 17 / 17 ms — 1.00x | 17 / 17 ms — 1.00x | 17 / 17 ms — 1.00x |

### Cross-machine, `-O2`, Clang both sides

| mode | M5 switched | M5 unswitched | M5 | Ryzen switched | Ryzen unswitched | Ryzen |
|---|---:|---:|---:|---:|---:|---:|
| add | 13 ms | 4 ms | 3.25x | 24 ms | 16 ms | 1.50x |
| mul | 8 ms | 4 ms | 2.00x | 21 ms | 17 ms | 1.24x |
| abssum | 12 ms | 4 ms | 3.00x | 27 ms | 18 ms | 1.50x |

**The mechanism reproduces exactly.** Disassembly confirms that at `-O2` Clang vectorizes
`apply_unswitched` (24 packed instructions) while leaving `apply_switched` fully scalar — precisely
the scalar-vs-SIMD story [loop_unswitch.md](loop_unswitch.md) describes. At `-O3` Clang performs the
unswitching automatically and the gap closes to exactly 1.00x on all three modes, also as that doc
predicts. Holding the compiler constant, the qualitative behavior is identical on both machines.

**The magnitude difference is hardware, and it is memory bandwidth.** Both loops are bandwidth-bound
at 3 × 32M floats = 384 MB. The unswitched loops converge to 16–18 ms here (≈24 GB/s effective
single-thread) against M5's 4 ms (≈96 GB/s). With the ceiling roughly 4x lower, the same
transformation yields a compressed win — 1.50x instead of 3.25x — even though the compiler is doing
the same thing. 24 GB/s single-thread is unremarkable for LPDDR5 on a laptop SoC, where full
platform bandwidth is much higher but not reachable from one core.

This is the cleanest hardware-only comparison in the set: same compiler family, same
transformation, same generated instruction classes, difference attributable to memory subsystem.

---

## Completing the comparison

`branch_free` and `lut_branch` currently have no M5 Clang baseline. To produce one, run on the M5
with Apple Clang — note the deliberate absence of `-fno-if-conversion`, which Clang cannot accept:

```bash
for lvl in O0 O1 O2 O3; do
  clang++ -$lvl -o bf_$lvl  branch_free.cpp
  clang++ -$lvl -o lut_$lvl lut_branch.cpp
done
```

Run each binary 3 times and take the minimum, to match the protocol above.

The expected result is that both examples collapse the same way they do here — the "branchy"
variant if-converted at `-O1` and vectorized at `-O2` — since that is Apple Clang behavior the M5
doc already documents. If so, the useful cross-machine numbers will be the *branchless* floors
(`arith`, `lut`) rather than any speedup ratio.

---

## Key takeaways

1. **Two of these three examples cannot be demonstrated under Clang at all.** `branch_free` and
   `lut_branch` both depend on `-fno-if-conversion` to have a branchy baseline, and Clang has no
   such flag. Under Clang their "branchy" versions are branch-free from `-O1` up — measured 0.93x
   and inverted-to-0.42x respectively. They are GCC-only demonstrations and should be labeled as
   such.

2. **`loop_unswitch` reproduces on Zen 5 with the same compiler family.** Clang vectorizes the
   unswitched loop and not the switched one at `-O2` on both Apple Silicon and x86, and closes the
   gap at `-O3` on both. The transformation's *behavior* is a Clang property and it ports cleanly.

3. **The one real hardware difference is memory bandwidth, ~4x.** ≈24 GB/s single-thread on this
   Ryzen laptop part vs ≈96 GB/s on M5, which fully accounts for 1.50x vs 3.25x on a bandwidth-bound
   kernel. No branch-prediction or ISA effect is needed to explain it.

4. **Hand-written branch-free code loses to Clang's autovectorizer from `-O2`.** `arith` goes from
   fastest (3.37x at `-O1`) to slowest (0.70x at `-O2`); the LUT goes from 4.77x to 0.33x. The
   principle — eliminate unpredictable branches — survives; the hand-coded technique does not, once
   the compiler is allowed to vectorize.

5. **Check the noise floor before trusting any of these ratios.** ±30% run-to-run variance on this
   machine is enough to manufacture or erase most of the effects being compared.
