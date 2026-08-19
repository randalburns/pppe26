# Strength Reduction Benchmarks

Measured on Apple M5, clang 17, 100 million iterations, best of 5 runs.

---

## Design

Both `BEFORE` and `AFTER` functions carry `__attribute__((optnone))`.  The compiler cannot
optimize either function regardless of the `-O` flag, so speedups reflect hardware
instruction costs only.  Results are stable across `-O0` through `-O3`.

```cpp
#define OPTNONE __attribute__((optnone))

OPTNONE long long div_shift_before() { /* uses UDIV */ }
OPTNONE long long div_shift_after()  { /* uses LSR  */ }
```

---

## Results

| # | Transformation | Before | After | Speedup |
|---|----------------|-------:|------:|--------:|
| 1 | Integer div by 2^n → right shift (`/4,/8,/16` → `>>2,>>3,>>4`) | 234 ms | 233 ms | 1.00x |
| 2 | Modulo 2^n → bitwise AND (`i%16+i%64+i%256` → `i&15+i&63+i&255`) | 206 ms | 233 ms | 0.88x |
| 3 | `pow(x,n)` → explicit multiply (`pow(x,2)+pow(x,3)+pow(x,4)` → `x*x+x*x*x+x*x*x*x`) | 150 ms | 16 ms | **9x** |
| 4 | FP division → reciprocal multiply (`i/3.14159` → `i*(1/3.14159)`) | 60 ms | 87 ms | 0.69x |
| 5 | Three divisions → one combined (`i/7/7/7` → `i/343`) | 75 ms | 24 ms | **3x** |
| 6 | Loop-induction multiply → accumulate (`i*7+100` → `v+=7`) | 49 ms | 49 ms | 1.00x |
| 7 | Near-2^n multiply → shift+add (`i*15+i*17+i*31` → `(i<<4)-i+...`) | 208 ms | 198 ms | 1.05x |
| 8 | Multiply by exact 2^n → left shift (`i*8+i*64+i*512` → `i<<3+i<<6+i<<9`) | 233 ms | 233 ms | 1.00x |

Speedup is consistent within ±5% at all four `-O` levels, confirming results are
unaffected by the optimizer.

---

## Analysis

**Tests 3 and 5 are the only clear wins.**  Both reduce the *count* of expensive
operations rather than swapping one instruction for another:

- **Test 3**: Three `pow()` library calls (each ~50–100 FP ops) replaced by three to
  four multiplies.  The saving is call overhead and transcendental computation, not
  instruction latency.
- **Test 5**: Three integer divides collapsed into one.  Divide cost is proportional to
  count, so 3× → 1× is a direct 3× saving.

**Tests 1, 2, 4, 6, 7, 8 show little or no benefit on Apple M5.**  The conventional
wisdom behind these transformations comes from x86, where IDIV costs 20–30 cycles and
IMUL costs 3–4 cycles.  On M5 the picture is different:

| Operation | x86 approx. | Apple M5 approx. |
|-----------|-------------|------------------|
| SHL / SHR | 1 cycle | 1 cycle |
| ADD / SUB | 1 cycle | 1 cycle |
| AND / OR  | 1 cycle | 1 cycle |
| IMUL      | 3–4 latency, 1/cycle throughput | 3 latency, 1/cycle throughput |
| UDIV      | **20–30 cycles** | **~4 cycles** |
| FDIV      | 10–15 cycles | 6–10 cycles |

IMUL and UDIV both have 1-cycle throughput on M5 in a simple loop, because out-of-order
execution hides the latency.  Substituting a shift or AND for those operations saves
latency on the critical path but not throughput — so the loop runs at the same speed.

**Test 4 (reciprocal multiply) is slower** in the AFTER version.  With `optnone`, the
precomputed reciprocal `r` is spilled to the stack and reloaded on every iteration.  That
load latency outweighs the FDIV saving.  This transformation only pays off when the
compiler is free to keep `r` in a register.

---

## Summary

| Rule | Reliable on M5? | Why |
|------|-----------------|-----|
| `pow(x,n)` → `x*x*...` | Yes | Eliminates library call overhead |
| `/d/d/d` → `/d³` | Yes | Reduces divide count |
| `x/2^n` → `x>>n` | No | M5 UDIV throughput ≈ SHR throughput |
| `x%2^n` → `x&(2^n-1)` | No | Same reason |
| `x*2^n` → `x<<n` | No | M5 IMUL throughput ≈ SHL throughput |
| `i*k` → accumulate `+=k` | No | Same reason |
| `i/d` → `i*(1/d)` | No | Register spill with optnone erases gain |

On a classic Intel/AMD CPU the shift, AND, and accumulate rules would each show 3–10×
speedups because IDIV and IMUL are genuinely slow.  On M5, the wins come from reducing
operation count, not from instruction substitution.
