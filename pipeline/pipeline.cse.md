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
