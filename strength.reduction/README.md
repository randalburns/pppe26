# Strength reduction

Strength reduction replaces an expensive operation with a cheaper one that
computes the same result — a shift instead of a divide, an accumulate
instead of a multiply, an explicit product instead of a library call. It's
one of the oldest tricks in the optimizing-compiler playbook, and one of the
most over-cited: the textbook cycle-cost tables it's based on come from
20-year-old x86 pipelines, and modern out-of-order cores often make the
"expensive" operation just as fast as its replacement.

This directory tests that folklore directly. One source file,
[strength_reduction.cpp](strength_reduction.cpp), implements eight classic
substitutions as BEFORE/AFTER pairs and benchmarks each in isolation.

---

## Design

Every BEFORE and AFTER function carries `__attribute__((optnone))`:

```cpp
#define OPTNONE __attribute__((optnone))

OPTNONE long long div_shift_before() { /* uses i / 4 */ }
OPTNONE long long div_shift_after()  { /* uses i >> 2 */ }
```

`optnone` blocks the compiler from doing the substitution itself, so the
BEFORE/AFTER gap can't be erased by the optimizer — it stays meaningful from
`-O0` through `-O3`. That's also *why* a single source file works here,
unlike the [loop_optimizations](../loop_optimizations/README.md) examples,
which need the optimizer active to show their effect: strength reduction is
a hardware-instruction-cost question, not a compiler-transformation
question, so pinning both sides to `optnone` isolates exactly that.

Build with `make` (produces `sr_O0` .. `sr_O3`) or `make run` to sweep all
four levels.

---

## 1 · Integer division by 2ⁿ → right shift

`i / 4` → `i >> 2`. Textbook claim: IDIV costs 20–30 cycles, SHR costs 1.

## 2 · Modulo by 2ⁿ → bitwise AND

`i % 16` → `i & 15`. Same IDIV-avoidance logic as #1, applied to remainder
instead of quotient.

## 3 · `pow(x, n)` → explicit multiplication

`pow(x, 2) + pow(x, 3) + pow(x, 4)` → `x2 + x2*x + x2*x2`. Not a
single-instruction swap — it trades a general transcendental library call
(tens to hundreds of FP ops, argument reduction, edge-case handling) for
three or four FMULs.

## 4 · FP division by a loop-invariant constant → reciprocal multiply

Compute `r = 1/d` once outside the loop, then use `i * r` instead of `i / d`
inside it. FDIV (10–15 cycles) becomes FMUL (4–5 cycles), paid once instead
of N times — *if* `r` stays in a register.

## 5 · Chained division by the same constant → one combined division

`v /= 7; v /= 7; v /= 7` → `v / 343`. Cuts divide *count* directly: three
IDIVs become one.

## 6 · Loop-induction multiply → running accumulate

`i * 7 + 100` recomputed every iteration → carry `v` forward and do
`v += 7`. The classical strength-reduction example from compiler textbooks:
turn a multiply that depends on the induction variable into an add that
depends on the previous value.

## 7 · Multiply by a near-power-of-2 → shift plus add/subtract

`x * 15` → `(x << 4) - x`. `x * 17` → `(x << 4) + x`. Exploits
`2ⁿ ± 1 ≈ 2ⁿ` to replace one IMUL with a SHL and an ADD/SUB.

## 8 · Multiply by an exact power-of-2 → left shift

`x * 8` → `x << 3`. The mirror image of #1: the simplest, most textbook
substitution of the set.

Each technique above is one `report()` block in `main()` — run the binary
and the eight results print in order, followed by a reference table of
approximate instruction costs.

---

## Zen vs. M5: does the folklore hold?

The two write-ups — [strength_reduction.md](strength_reduction.md) (Apple
M5) and [strength_reduction_ryzen.md](strength_reduction_ryzen.md) (AMD
Ryzen AI 9 HX 370, Zen 5) — run the identical binary on two very different
out-of-order microarchitectures. The headline: **the folklore is mostly
wrong on both, but wrong in different places.**

| # | Transformation | M5 speedup | Zen 5 speedup | Verdict |
|---|---|---:|---:|---|
| 1 | div by 2ⁿ → shift | 1.00x | **1.29x** | Zen shows the textbook win; M5 doesn't |
| 2 | mod 2ⁿ → AND | 0.88x (slower) | 1.00x (flat) | Neither machine benefits |
| 3 | `pow(x,n)` → mults | **9x** | **12.3x** | Big win on both |
| 4 | FP div → reciprocal | 0.69x (slower) | 1.00x (flat) | M5 regresses, Zen doesn't |
| 5 | 3×div → 1×div | **3x** | **3.03x** | Matches almost exactly |
| 6 | induction mul → accumulate | 1.00x | 0.98x | No benefit on either |
| 7 | near-2ⁿ mul → shift+add | 1.05x | 0.95x | Wash on both |
| 8 | exact-2ⁿ mul → shift | 1.00x | 1.08x | Mild win on both |

**Only tests 3 and 5 are reliable wins on both chips**, and they share a
property the other six don't: they reduce operation *count* (fewer library
calls, fewer divides) rather than substituting one instruction for a
nominally cheaper one. On a modern out-of-order core, single-cycle-throughput
SHL/AND/IMUL let the pipeline hide latency behind independent work in the
loop — so swapping IDIV for SHR doesn't speed up the loop unless IDIV was
actually the throughput bottleneck.

**Test 1 is the one place x86 folklore is actually true on x86.** Zen 5's
integer divider is still slow enough relative to a shift that div→shift
gives a genuine 1.29x; M5's UDIV throughput (~4 cycles) is already fast
enough that shifting buys nothing. This is the single clearest
architecture-dependent result in the set.

**Test 4 is the one that inverts expectation, and only on M5.** Under
`optnone`, the precomputed reciprocal `r` can't be kept in a register — it
spills to the stack and reloads every iteration. On M5 that reload cost
exceeds the FDIV saving, so the "optimized" version is *slower*. On Zen 5
the same spill happens, but FDIV there is expensive enough relative to a
spilled load that the two roughly cancel out. Neither result reflects how
this transformation performs in real (non-`optnone`) code, where the
compiler keeps `r` live in a register and the reciprocal trick reliably
wins — it's a warning about benchmark methodology as much as about hardware.

| Rule | Reliable on Zen 5? | Reliable on M5? |
|------|:---:|:---:|
| `pow(x,n)` → `x*x*...` | Yes | Yes |
| `/d/d/d` → `/d³` | Yes | Yes |
| `x/2ⁿ` → `x>>n` | Yes | No |
| `x%2ⁿ` → `x&(2ⁿ-1)` | No | No |
| `x*2ⁿ` → `x<<n` | Marginal | No |
| `i*k` → accumulate `+=k` | No | No |
| `i/d` → `i*(1/d)` | No (flat) | No (regresses) |

The two count-reducing transformations are architecture-independent wins.
Everything else in the classic strength-reduction playbook is, at best,
CPU-specific — and on two current-generation out-of-order cores, it mostly
doesn't pay off the way the cycle-cost tables in compiler textbooks suggest.

---

## Files at a glance

**Source** — [strength_reduction.cpp](strength_reduction.cpp) — all eight
BEFORE/AFTER pairs, `optnone`-pinned, benchmarked via `bench()`/`report()`
**Build** — [Makefile](Makefile) — `make` / `make run` / `make clean`,
produces `sr_O0` .. `sr_O3`
**Write-ups** — [strength_reduction.md](strength_reduction.md) (Apple M5),
[strength_reduction_ryzen.md](strength_reduction_ryzen.md) (AMD Ryzen Zen 5)
