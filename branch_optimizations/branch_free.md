# Branch-Free Code: Arithmetic and Bitwise Replacements

Demonstrates how replacing conditional branches with arithmetic and bitwise
operations eliminates misprediction penalties and produces execution time that
is independent of data pattern.

## Source

[branch_free.cpp](branch_free.cpp)

## The problem with branches

Every conditional branch requires the CPU to predict which path to take.
Modern branch predictors are excellent at exploiting patterns, but when
the data is random — as in sorting, hashing, image processing, or crypto —
the predictor has nothing to learn from, and mispredictions become constant.

Each misprediction flushes the pipeline and discards speculative work:
≈ 15 cycles wasted on Apple M-series (≈ 15–20 on x86 Skylake).

This example processes 32M byte values (e.g., pixel data):

```
clamp to [64, 192]      — 2 branches per element
count values > 128      — 1 branch per element
```

With uniformly random input, each branch fires ~25–50% of the time — the
worst case for prediction.

Expected overhead:

```
N × avg_misprediction × penalty = 32M × 1.0 × 15 / 4 GHz ≈ 126 ms
```

---

## The three branchless techniques

### Technique 1 — Signed right shift as a bit mask

```cpp
int mask = diff >> 31;
// Arithmetic right shift propagates the sign bit:
//   diff >= 0  →  mask = 0x00000000
//   diff <  0  →  mask = 0xFFFFFFFF
```

The result is a full-width zero or all-ones mask — perfect for bit selection.

### Technique 2 — Branchless max(v, LO)

```cpp
int diff = v - LO;
int mask = diff >> 31;   // -1 if v < LO, else 0
v -= diff & mask;
```

Proof:
- `v >= LO` → `diff >= 0` → `mask = 0` → `diff & 0 = 0` → `v` unchanged ✓
- `v < LO`  → `diff < 0`  → `mask = -1` → `diff & -1 = diff` → `v -= (v-LO)` = LO ✓

Equivalent to `v = v < LO ? LO : v`, but expressed entirely in arithmetic.

### Technique 3 — Branchless min(v, HI)

```cpp
int diff = v - HI;
int mask = diff >> 31;   // 0 if v > HI, else -1
v -= diff & ~mask;
```

Proof:
- `v <= HI` → `diff <= 0` → `mask = -1` → `~mask = 0` → `diff & 0 = 0` → `v` unchanged ✓
- `v > HI`  → `diff > 0`  → `mask = 0` → `~mask = -1` → `diff & -1 = diff` → `v -= (v-HI)` = HI ✓

### Technique 4 — Comparison as integer

```cpp
count += (v > MID);
```

C++ comparison operators are defined to return exactly 0 or 1.  The compiler
emits `CMP` followed by `CINC` (conditional increment) — no branch instruction,
no prediction needed.

---

## The ternary shortcut

```cpp
v = v < LO ? LO : v;    // compiler emits CSEL on ARM, CMOV on x86
count += (v > MID);
```

For scalar C++ code, ternary operators already produce branchless CSEL/CMOV
instructions at `-O1`.  The explicit arithmetic form is useful when:

- Writing **SIMD intrinsics** — vector lanes have no conditional select
- Targeting **GPUs** or shader languages — divergent branches serialize lanes
- Targeting **in-order cores** — where there is no CSEL equivalent
- Making the **invariant explicit** in source for documentation or audit

---

## Build

```bash
g++-15 -O1 -fno-if-conversion -o branch_free branch_free.cpp && ./branch_free
```

`-fno-if-conversion` prevents GCC from converting the `if` statements in
`process_branchy` into CSEL before we can observe their misprediction cost.
The `ternary` and `arith` versions have no `if` statements and are unaffected.

Apple Clang always converts simple `if`/ternary to CSEL at `-O1`, so GCC
is required to observe the full branchy penalty.

---

## Results (Apple M4, N=32M bytes)

| version | random | sorted | random speedup |
|---------|--------|--------|----------------|
| branchy (3 ifs) | 123 ms | 13 ms | 1.00x |
| ternary (CSEL) | 14 ms | 14 ms | **8.79x** |
| arith (bitmask) | 16 ms | 15 ms | **7.69x** |

---

## Analysis

**Branchy on random data: 123 ms**

The three branches fire unpredictably.  The expected overhead from
mispredictions alone is `32M × 1.0 × 15 / 4GHz ≈ 126 ms`, which closely
matches the measurement.  Virtually all execution time is wasted flushing
and re-filling the pipeline.

**Branchless on random data: 14–16 ms**

No branches → no mispredictions → no flushes.  The loop body is a straight
sequence of arithmetic and compare instructions.  Execution time reflects
only the actual work: one load, three arithmetic ops, one conditional
increment, one add, one store per element.

**Branchy on sorted data: 13 ms**

When the data is sorted, all `< LO` elements appear first, then `LO..HI`,
then `> HI`.  The predictor learns "always not taken" then "always taken",
mispredicting exactly once per transition.  Prediction is near-perfect, so
the branchy loop runs at roughly the same throughput as the branchless loop.

**Branchless on sorted vs. random: identical**

Branchless code has no concept of "data pattern" — it executes the same
instructions unconditionally every time.  The timing is the same whether
the data is random, sorted, or pathological.

**Branchy vs. branchless tradeoff:**

| data pattern | branchy | branchless | winner |
|---|---|---|---|
| random | 123 ms | 15 ms | branchless (~8x) |
| sorted | 13 ms | 15 ms | branchy (slightly) |
| unknown | unpredictable | 15 ms | branchless (safe choice) |

When the data access pattern is unknown or adversarially controlled (e.g.,
from user input), branchless code provides a predictable performance floor.

---

## Key takeaways

1. **Branches are free when perfectly predicted; expensive when not.**
   Random data breaks prediction and turns every conditional into a potential
   15-cycle penalty.

2. **Arithmetic and bitwise ops replace branches completely.**
   The three techniques — signed-shift mask, masked conditional subtract, and
   comparison-as-integer — cover the majority of practical conditional patterns.

3. **Ternary `?:` is the easy path for scalar code.**
   The compiler generates CSEL/CMOV already.  Explicit arithmetic is for cases
   where the compiler cannot: SIMD, shaders, or in-order cores.

4. **Branchless code trades occasional slowness for uniform throughput.**
   On predictable data, a well-predicted branch skips unused arithmetic work.
   Branchless always pays the full arithmetic cost.  Choose based on whether
   the data pattern is known and stable.
