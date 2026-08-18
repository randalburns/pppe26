# Loop Unswitching Benchmarks — AMD Ryzen AI 9 HX 370

Measured on AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu, GCC 13.3.0,
N=32M floats, best of 5 runs. See [loop_unswitch.md](loop_unswitch.md) for the Apple M4 baseline
this compares against.

---

## Build note

Same command as the doc:

```
g++ -O2 -o loop_unswitch loop_unswitch.cpp
```

Required the same `(long long)` cast fix to `duration_cast<milliseconds>(...).count()` as the
other two examples in this set, for the same GCC/libstdc++ template-deduction reason (see
[branch_free_ryzen.md](branch_free_ryzen.md)).

---

## Results (`-O2`, N=32M floats; stable within ~10%)

| mode | switched | unswitched | speedup |
|------|---------:|-----------:|--------:|
| 0 — add | 20 ms | 18 ms | 1.11x |
| 1 — mul | 19 ms | 18 ms | 1.06x |
| 2 — abssum | 21 ms | 18 ms | 1.15x |

This is a much smaller gap than the M4 doc's 2.00x–3.25x. `objdump` explains why: **at `-O2` on
this GCC/x86 build, neither the switched nor the unswitched loop is vectorized.** Both compile to
scalar `movss`/`addss`/`mulss` — no `addps`/`mulps` packed instructions anywhere in either function.
The measured 1.06–1.15x gap here is purely the cost of the loop-invariant branch/CMOV bookkeeping
per element, not a scalar-vs-SIMD difference — the mechanism the M4 doc's "why it matters" section
is actually about.

## Results (`-O3`, N=32M floats; stable within ~5%)

| mode | switched | unswitched | speedup |
|------|---------:|-----------:|--------:|
| 0 — add | 16 ms | 16 ms | 1.00x–1.07x |
| 1 — mul | 16 ms | 16 ms | 1.00x |
| 2 — abssum | 16 ms | 16 ms | 1.00x |

At `-O3`, `objdump` shows both functions now use packed `addps`/`mulps` (128-bit SSE, 4 floats/
instruction) — GCC performs the unswitching transformation automatically here too, exactly as the
source doc predicts, closing the gap between switched and unswitched to a wash (1.00x–1.07x),
matching the qualitative claim in [loop_unswitch.md](loop_unswitch.md) even though that doc only
demonstrated it in prose.

---

## Results across optimization levels

Filling in `-O0` and `-O1` alongside the `-O2`/`-O3` numbers above gives the full picture (speedup
= unswitched vs. switched, averaged across the three modes):

| level | add | mul | abssum | avg |
|---|---:|---:|---:|---:|
| O0 | 1.15x | 1.14x | 1.22x | 1.17x |
| O1 | 1.11x | 1.26x | 1.15x | 1.17x |
| O2 | 0.95x | 1.14x | 1.18x | 1.09x |
| O3 | 1.06x | 1.06x | 1.00x | 1.04x |

**The benefit shrinks monotonically as optimization level rises, and briefly inverts** (add mode
at `-O2`: switched is 5% *faster* than unswitched — noise-level, but notably not a win). This is
the opposite of the M4 story, where the gap is largest exactly at `-O2` (2.00x–3.25x) because that's
where Clang vectorizes one variant and not the other. Here, no level shows a vectorization-driven
gap: `-O0`–`-O2` are scalar-vs-scalar (branch bookkeeping only, ~10–17%), and `-O3` is
vectorized-vs-vectorized (compiler unswitches both, gap collapses further to ~4%). There's no
`-O` level on this GCC/x86 build that reproduces the M4 doc's headline scalar-vs-SIMD comparison.

---

## Comparison to Apple M4

| mode | M4 switched | M4 unswitched | M4 speedup | Ryzen -O2 switched | Ryzen -O2 unswitched | Ryzen -O2 speedup |
|---|---:|---:|---:|---:|---:|---:|
| add | 13 ms | 4 ms | 3.25x | 20 ms | 18 ms | 1.11x |
| mul | 8 ms | 4 ms | 2.00x | 19 ms | 18 ms | 1.06x |
| abssum | 12 ms | 4 ms | 3.00x | 21 ms | 18 ms | 1.15x |

**This is the largest divergence found across the three branch-optimization examples, and it isn't
really about branches.** On M4, Apple Clang at `-O2` vectorizes the unswitched loop with NEON
(4 floats/instruction) but not the switched one, so the reported win is dominated by scalar-vs-SIMD
throughput, not the branch itself. On this machine, GCC's `-O2` doesn't auto-vectorize *either*
loop — GCC's default `-O2` tree-vectorizer pass runs, but for this loop shape and without an
explicit `-march=` target enabling wider registers, it evidently doesn't judge either loop
profitable to vectorize at `-O2` on x86 (unlike Clang's more aggressive default at the same
optimization level). Confirmed by disassembly: both `apply_switched` and `apply_unswitched` compile
to scalar `movss`/`addss`/`mulss` at `-O2`.

**The "why it matters: vectorization" framing in the M4 doc does not hold on this build at `-O2`.**
On Ryzen at `-O2`, the measured 1.06–1.15x speedup is real but reflects only the branch/CMOV
overhead per iteration — the vectorization story only shows up once you move to `-O3`, where GCC
vectorizes both loops and the switched/unswitched gap nearly disappears (1.00–1.07x), consistent
with the M4 doc's own note that `-O3` closes the gap by unswitching automatically. In other words:
Ryzen at `-O2` behaves like a slowed-down version of what the M4 doc calls the `-O3` outcome —
scalar-vs-scalar with only the branch bookkeeping as overhead — rather than the `-O2` outcome the
M4 doc describes (scalar-vs-SIMD).

**Absolute vectorized throughput is also lower here.** At `-O3`, both loops converge to 16 ms
(384 MB / 16 ms ≈ 24 GB/s effective single-thread bandwidth) vs M4's 4 ms convergence (≈96 GB/s).
This gap is expected: M4's doc figure reflects Apple Silicon's unified, very high single-core
memory bandwidth, while a single x86 core's achievable bandwidth is typically a fraction of the
platform's aggregate — 24 GB/s single-thread is unremarkable for LPDDR5 on a laptop SoC where full
platform bandwidth is much higher but not reachable from one core.

---

## Key takeaways (Ryzen-specific)

1. **The compiler-specific vectorization threshold matters more than the ISA here.** Whether the
   "loop unswitching helps because of vectorization" story applies depends on whether the compiler
   actually vectorizes at the optimization level being tested — true for Clang at `-O2` on M4, false
   for GCC at `-O2` on this x86 build, true again for GCC at `-O3`.

2. **Don't reuse an `-O2`-derived conclusion across toolchains without checking codegen.** The same
   source, same flag, same nominal optimization level produced qualitatively different compiler
   behavior (vectorized vs. not) on the two machines — always confirm with `objdump` rather than
   assuming the optimization level alone determines what happens.

3. **When both loops are scalar, the invariant-branch cost alone is small (~10%)** — consistent
   with the general finding across this branch-optimization series that a single, well-isolated
   branch is cheap; the earlier large multiplier only appears when it's compounded with (or
   confused for) a vectorization difference.
