# Common Subexpression Elimination: Paying a Stall Once, Not Three Times

Recomputing the same expression re-issues the same multi-cycle instruction
and re-pays its stall every time. Caching the result in a local variable —
common subexpression elimination (CSE) — means the expensive operation
executes once, and every reuse reads an already-available register.

## Source

[pipeline.cse.cpp](pipeline.cse.cpp)

## How it works

**Before — the same multiply repeated three times:**

```cpp
int beforeCse(int a, int b, int c) {
    int x = a * b + c;          // chain 1: MUL → stall → ADD
    int y = a * b + c + 1;      // chain 2: MUL again → stall → ADD → ADD
    int z = a * b + c + 2;      // chain 3: MUL again → stall → ADD → ADD
    return x + y + z;
}
```

```
Cycle  Instruction
-----  -----------
 1     MUL  t0, a, b        ; a*b  (result ready cycle 4)
 2     --- stall ---
 3     --- stall ---
 4     ADD  x,  t0, c       ; x = a*b + c
 5     MUL  t1, a, b        ; DUPLICATE (result ready cycle 8)
 6     --- stall ---
 7     --- stall ---
 8     ADD  y,  t1, c       ; y = a*b + c  (same value!)
                              ↑ 6 wasted stall cycles, and a third repeat below
```

**After — computed once, reused as a ready register:**

```cpp
int afterCse(int a, int b, int c) {
    const int base = a * b + c; // ONE multiply, ONE dependency chain
    const int x = base;         // register rename / copy — free
    const int y = base + 1;     // ADD on already-available register — free
    const int z = base + 2;     // ADD on already-available register — free
    return x + y + z;
}
```

```
Cycle  Instruction
-----  -----------
 1     MUL  t0, a, b        ; a*b once (result ready cycle 4)
 2     --- stall ---
 3     --- stall ---
 4     ADD  base, t0, c     ; base = a*b + c
 5     MOV  x,   base       ; x = base     ← no stall: base ready
 6     ADD  y,   base, 1    ; y = base + 1  ← no stall
 7     ADD  z,   base, 2    ; z = base + 2  ← no stall
                              ↑ 0 wasted stall cycles
```

The source benchmarks this shape three times: a plain integer multiply
(`a*b + c`), an array-index calculation (`stride*i`, recomputed inside a
loop), and a floating-point `sqrt()` — the highest-latency of the three at
~14 cycles on typical x86. Each `before*` function is marked `NOINLINE` and
fenced with `doNotOptimize()` between repeats, so the compiler can't quietly
apply the same CSE for you even at `-O3`; the `after*` versions are left
free to optimize normally.

## Build

```bash
clang++ -O2 -o pipeline.cse pipeline.cse.cpp -lm && ./pipeline.cse
```

## Results (Apple M-series, `-O2`)

| Section | Before | After | Speedup |
|---|---|---|---|
| Integer: `a*b + c` reused 3x | 108.39 ms | 45.19 ms | 2.40x |
| Loop index: `stride*i` reused 3x | 0.17 ms | 0.01 ms | 17x |
| FP: `sqrt(x²+y²+z²)` reused 3x | 20.93 ms | 11.62 ms | 1.80x |

## Verified optimization levels

Rebuilt and re-run at every level (`clang++`, best of 3 runs per cell —
`main()` here doesn't loop internally, so single runs are noisier than
[pipeline.cpp](pipeline.cpp)'s):

| Level | Integer speedup | Loop-index speedup | `sqrt` speedup |
|---|---|---|---|
| `-O0` | 2.4x | 1.4x | 1.2x |
| `-O1` | 2.0x | 2.1x | 1.8x |
| `-O2` | 2.0x | **17x** | 1.8x |
| `-O3` | 2.1x | **17x** | 1.8x |

**All four levels show a real before/after gap** — confirming the source
comment's claim that the `NOINLINE`/`doNotOptimize()` fences work "at any
optimisation level." But the loop-index case is *not* level-invariant: its
dramatic 17x only appears at `-O2`/`-O3`. At `-O0`/`-O1` the manual CSE
alone is worth roughly 1.4–2.1x, the same ballpark as the other two
sections; the extra 8x only shows up once the compiler's own optimizations
(loop-invariant code motion, stronger address-calculation folding) start
compounding with the hand-written fix on top of it. The integer and `sqrt`
cases don't show this — they're consistently ~2x and ~1.2–1.8x at every
level, since there's no comparable second optimization for the compiler to
add on top of the manual fix there.

## Analysis

Same underlying mechanism throughout: each repeated recomputation re-issues
a multi-cycle instruction (multiply, load, or `sqrt`) and re-pays its
stall, instead of reading a register the first computation already filled.
The loop-index case shows the largest speedup (17x) because `stride*i` sits
inside a hot loop — CSE there removes a redundant multiply from every single
iteration, not just once per call. The `sqrt` case shows the smallest
relative speedup despite the highest per-call latency, because the
surrounding division and floating-point adds dilute the fraction of total
time the redundant `sqrt` calls actually account for.

## Key takeaways

1. **CSE eliminates redundant stalls, not redundant instructions.** The win
   isn't "fewer lines of code" — it's paying a multi-cycle latency once
   instead of N times.
2. **The effect scales with the instruction's latency and how many times
   it's actually repeated per unit of surrounding work** — a cheap
   expression inside a hot loop (loop-index CSE) can outperform an
   expensive one called rarely (the `sqrt` case), because the stall's share
   of total runtime differs.
3. **This is a compiler pass you can usually rely on** — `NOINLINE` and
   `doNotOptimize()` exist here specifically to defeat the optimizer so the
   "before" cost is even observable. Ordinary code rarely needs to force
   this by hand.
4. **A speedup number can itself be level-dependent, even when the effect
   is real at every level.** The loop-index case's headline 17x needs
   `-O2`/`-O3` — at `-O0`/`-O1` the same fix is genuinely present but worth
   only ~1.4–2.1x, because the rest of that speedup comes from a second
   compiler optimization compounding with the manual one, not from CSE
   alone.
