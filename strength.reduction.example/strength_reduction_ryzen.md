# Strength Reduction Benchmarks — AMD Ryzen AI 9 HX 370

Measured on AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu clang 18.1.3,
libc++, 100 million iterations, best of 5 runs. See [strength_reduction.md](strength_reduction.md)
for the Apple M5 baseline this compares against.

---

## Design

Same source as the M5 run (`strength_reduction.cpp`), built with clang instead of GCC so that
`__attribute__((optnone))` is honored — GCC silently ignores this attribute, which would let the
optimizer erase the BEFORE/AFTER distinction at `-O1` and above. The build also links libc++
(`-stdlib=libc++`) rather than the system default libstdc++, both to match the M5 clang/libc++
toolchain and to avoid a GCC/clang header-version mismatch on this system.

```
CXXFLAGS := -std=c++14 -stdlib=libc++
```

---

## Results (`-O0`; stable within ~5% through `-O3`)

| # | Transformation | Before | After | Speedup |
|---|----------------|-------:|------:|--------:|
| 1 | Integer div by 2^n → right shift (`/4,/8,/16` → `>>2,>>3,>>4`) | 81 ms | 63 ms | **1.29x** |
| 2 | Modulo 2^n → bitwise AND (`i%16+i%64+i%256` → `i&15+i&63+i&255`) | 63 ms | 63 ms | 1.00x |
| 3 | `pow(x,n)` → explicit multiply (`pow(x,2)+pow(x,3)+pow(x,4)` → `x*x+x*x*x+x*x*x*x`) | 283 ms | 23 ms | **12.3x** |
| 4 | FP division → reciprocal multiply (`i/3.14159` → `i*(1/3.14159)`) | 234 ms | 235 ms | 1.00x |
| 5 | Three divisions → one combined (`i/7/7/7` → `i/343`) | 182 ms | 60 ms | **3.03x** |
| 6 | Loop-induction multiply → accumulate (`i*7+100` → `v+=7`) | 40 ms | 41 ms | 0.98x |
| 7 | Near-2^n multiply → shift+add (`i*15+i*17+i*31` → `(i<<4)-i+...`) | 80 ms | 84 ms | 0.95x |
| 8 | Multiply by exact 2^n → left shift (`i*8+i*64+i*512` → `i<<3+i<<6+i<<9`) | 81 ms | 75 ms | 1.08x |

---

## Comparison to Apple M5

| # | Transformation | M5 speedup | Ryzen speedup | Verdict |
|---|---|---:|---:|---|
| 1 | div by 2ⁿ → shift | 1.00x | **1.29x** | x86 shows a real win M5 doesn't |
| 2 | mod 2ⁿ → AND | 0.88x (slower) | 1.00x (flat) | Neither machine benefits |
| 3 | `pow(x,n)` → mults | 9x | **12.3x** | Big win on both, x86 bigger |
| 4 | FP div → reciprocal | 0.69x (slower) | 1.00x (flat) | Ryzen doesn't regress like M5 |
| 5 | 3×div → 1×div | 3x | 3.03x | Matches almost exactly |
| 6 | induction mul → accumulate | 1.00x | 0.98x | No benefit on either |
| 7 | near-2ⁿ mul → shift+add | 1.05x | 0.95x | Wash on both |
| 8 | exact-2ⁿ mul → shift | 1.00x | 1.08x | Mild win on both |

**Test 1 is the standout divergence.** On M5, IDIV/UDIV throughput (~4 cycles) is fast enough that
shifting doesn't help. On Zen 5, integer divide is still expensive enough relative to a shift that
swapping in `>>` gives a genuine 1.29x — the one case where the "classic x86 folklore" cited in the
M5 doc actually holds up on real x86 hardware.

**Test 2 does not follow test 1, which is a surprise.** Same IDIV→cheap-op substitution, same CPU,
yet modulo-by-larger-constants (16/64/256) shows no win at all, while division-by-smaller-constants
(4/8/16) does. Possibly a different microcode/division-algorithm path is selected for the larger
divisors, or out-of-order scheduling hides the cost differently depending on surrounding
instruction mix. Worth investigating further if this pattern matters for real code.

**Test 4 is the other interesting flip.** On M5, the "optimized" version got *slower* because the
compiler spilled the precomputed reciprocal to the stack under `optnone`, and the reload latency
outweighed the FDIV savings. On Ryzen the same spill occurs, but the loss is a wash rather than a
regression — FDIV here isn't as disproportionately expensive relative to a spilled load as it is on
M5.

**Tests 3 and 5 are the only transformations robust across both architectures**, and by a wide
margin. Both eliminate operation *count* (a library call, or a divide count) rather than substitute
one instruction for a nominally cheaper one. This reinforces the M5 doc's core thesis: on modern
out-of-order cores — ARM or x86 alike — single-cycle-throughput IMUL/AND/shift make most
latency-only substitutions pointless, because the CPU already hides the latency behind independent
work in the loop. The wins that survive are the ones that remove real work, not the ones that just
make it "look" cheaper.

**Tests 6, 7, 8** land close to neutral on both machines, confirming IMUL throughput is not the
bottleneck on either CPU.

---

## Summary

**How the x86 (Ryzen) picture differs from ARM (M5):** on M5, none of the classic x86 "IDIV/IMUL
are slow" folklore held up — modern out-of-order execution hides divide/multiply latency as long
as there's independent work in the loop, so only the two count-reducing tests (eliminating
`pow()` calls, collapsing 3 divides into 1) showed real wins. On Ryzen, that same "OoO hides it"
story holds for most of the substitution tests too — mod→AND, mul→shift, induction→accumulate are
all still roughly neutral, same as on M5. But two things break the pattern:

- **Div→shift actually wins on Ryzen (1.29x)** but not on M5 (1.00x) — the one case where the
  textbook x86 IDIV-is-expensive story is true on real x86 hardware, even though it isn't on ARM.
- **Reciprocal multiply doesn't regress on Ryzen**, whereas on M5 it got *slower*. That M5
  regression was a register-spill artifact from `optnone`; the same spill happens on Ryzen but
  doesn't cost as much relative to the FDIV it replaces.

So the headline: the two "always win" transformations (count reduction) are architecture-independent
and validated on both chips. Nearly everything else is a wash on both, with div→shift being the sole
case where x86 actually rewards the classic substitution and ARM doesn't.

| Rule | Reliable on Ryzen (Zen 5)? | Reliable on M5? |
|------|:---:|:---:|
| `pow(x,n)` → `x*x*...` | Yes | Yes |
| `/d/d/d` → `/d³` | Yes | Yes |
| `x/2^n` → `x>>n` | Yes | No |
| `x%2^n` → `x&(2^n-1)` | No | No |
| `x*2^n` → `x<<n` | Marginal | No |
| `i*k` → accumulate `+=k` | No | No |
| `i/d` → `i*(1/d)` | No (flat) | No (regresses) |

The two "always win" rules — reducing library-call overhead and reducing divide *count* — are
architecture-independent. Everything else in the classic strength-reduction playbook is, at best,
CPU-specific, and on both of these modern out-of-order cores it mostly doesn't pay off the way
textbook cycle-cost tables suggest.
