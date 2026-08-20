# Pipeline Stalls: Dependent Chains vs. Independent Accumulators

A CPU pipeline stalls when an instruction must wait on a value that hasn't
finished computing yet — a RAW (Read-After-Write) hazard. Two experiments
here show the same fix from two angles: a single accumulator forces every
step to wait on the last, while several independent accumulators give the
CPU other work to do while each result is still in flight.

## Source

[pipeline.cpp](pipeline.cpp)

## What is a RAW hazard

A pipelined CPU overlaps the stages (fetch, decode, execute, ...) of
consecutive instructions. That only works if the instructions are
independent. When instruction B reads a value that instruction A hasn't
finished writing, B can't enter the execute stage until A's result is
ready — the pipeline stalls, inserting bubble cycles instead of useful work.
A loop whose accumulator is read and written every iteration is a RAW
hazard repeated once per element.

## How it works

**Dependent chain — one accumulator, four ops each reading the line above:**

```cpp
long long x = 1;
for (int i = 0; i < ARRAY_SIZE; i++) {
    x = x + data[i];
    x = x ^ (x >> 1);
    x = x * 3;
    x = x ^ (x >> 2);
}
```

Every line depends on the one before it. No instruction here can start
before the previous one finishes.

**Independent chains — four accumulators, no cross-dependencies:**

```cpp
long long x1 = 1, x2 = 2, x3 = 3, x4 = 4;
for (int i = 0; i < ARRAY_SIZE; i += 4) {
    x1 = x1 + data[i];      x1 = x1 ^ (x1 >> 1);  x1 = x1 * 3;  x1 = x1 ^ (x1 >> 2);
    x2 = x2 + data[i + 1];  x2 = x2 ^ (x2 >> 1);  x2 = x2 * 3;  x2 = x2 ^ (x2 >> 2);
    x3 = x3 + data[i + 2];  x3 = x3 ^ (x3 >> 1);  x3 = x3 * 3;  x3 = x3 ^ (x3 >> 2);
    x4 = x4 + data[i + 3];  x4 = x4 ^ (x4 >> 1);  x4 = x4 * 3;  x4 = x4 ^ (x4 >> 2);
}
```

`x1`..`x4` share no dependency, so the CPU can overlap all four chains. A
second, simpler experiment in the same source (`singleAccumulator` vs.
`multipleAccumulators`, 1 vs. 8 accumulators over a plain `sum += data[i]`)
isolates the identical effect with no bit-mixing at all.

## Build

```bash
clang++ -std=c++17 -O1 -o pipeline pipeline.cpp && ./pipeline
./buildandrun.sh   # sweeps -O0 through -O3
```

## Results (Apple M-series, N=100M ints, min of 5 runs)

| Level | Dependent | Independent | Speedup | Single acc | Multi acc | Speedup |
|-------|-----------|-------------|---------|-------------|-----------|---------|
| -O0   | 496 ms    | 198 ms      | 2.51x   | 112 ms      | 76 ms     | 1.47x   |
| -O1   | 158 ms    | 44 ms       | 3.59x   | 23 ms       | 6 ms      | 3.83x   |
| -O2   | 0 ms*     | 45 ms       | —       | 0 ms*       | 2 ms      | —       |
| -O3   | 0 ms*     | 0 ms*       | —       | 0 ms*       | 0 ms*     | —       |

`*` — the effect doesn't vanish at higher `-O`; the *measurement* does. At
-O2/-O3 the compiler recognizes `dependentChain`'s recurrence and either
strength-reduces it or auto-vectorizes the independent-chain loops (4
independent accumulators is exactly the shape the auto-vectorizer wants),
so the timed region finishes in well under a millisecond and
`duration_cast<milliseconds>` rounds it to 0.

## Verified optimization levels

Re-built and re-run at each level to confirm the table above (`clang++
-std=c++17`, same source, same machine):

| Level | Shows the effect? |
|---|---|
| `-O0` | **Yes** — 2.51x / 1.50x |
| `-O1` | **Yes** — 3.59x / 3.83x, the cleanest run |
| `-O2` | No — the dependent-chain side already reports 0 ms |
| `-O3` | No — both sides report 0 ms |

**Only `-O0` and `-O1` demonstrate this example as a timing comparison.**
From `-O2` on, the compiler has already applied an equivalent fix on its
own, so there's no gap left for the benchmark to show.

## Analysis

The 2.5–3.8x speedups from breaking the dependency chain are real and
visible at -O0/-O1, where the compiler still emits something close to a
literal instruction-per-line translation. Confirming what actually happened
at -O3 requires dumping `-S` and checking for vectorized instructions
(`vmulq`/`vpaddq` and friends) vs. a constant-folded result, or switching the
timer to microseconds — see [pipeline.tempvar.md](pipeline.tempvar.md) for a
worked example of exactly that diagnosis.

This is the same phenomenon [../ILP/out_of_order.md](../ILP/out_of_order.md)
and [../ILP/sep_dependent.md](../ILP/sep_dependent.md) measure far more
precisely — reorder buffer behavior, cycles/elem, CPI, and a `K_min`
saturation formula, on doubles rather than ints. This file predates that
analysis and is the rougher, optimization-level-sweep version of the same
idea: it shows *that* the effect exists and *when* it's masked, without the
cycle-accounting those two go on to do.

## Key takeaways

1. **A loop-carried dependency chain serializes the pipeline** regardless of
   how many execution units are available, because each instruction can't
   start until the one before it finishes.
2. **Independent accumulators expose that concurrency** without changing
   the arithmetic — same total, more overlap.
3. **The effect survives past -O1; the measurement doesn't.** A 0 ms result
   at -O2/-O3 means the compiler already solved the problem for you
   (vectorization or strength reduction), not that the RAW hazard stopped
   existing.
