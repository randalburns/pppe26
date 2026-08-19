# Pipeline Stalls: Findings

Four small benchmarks, all built around one idea: a CPU pipeline stalls when
an instruction must wait on a value that hasn't finished computing yet (a
RAW / Read-After-Write hazard). Breaking dependency chains — via independent
accumulators, temporaries, common subexpression elimination, or dead code
removal — lets the CPU overlap work that would otherwise serialize.

Measured on this machine (clang++, `-std=c++17`, ARRAY_SIZE = 100M unless
noted). Numbers are `min` of 5 runs, milliseconds.

## 1. `pipeline.cpp` — dependent chain vs. independent accumulators

Two experiments, compiled at `-O0`..`-O3` (`buildandrun.sh` / `pipeline_O0..O3`):

- **Dependent chain**: one accumulator `x`, each of 4 ops reads the result of
  the previous line — a strict RAW chain every iteration.
- **Independent chains**: four separate accumulators (`x1..x4`) with no
  cross-dependencies, so the CPU can execute them in parallel/overlapped.
- **Single vs. multiple accumulators**: same idea for a plain summation loop
  (1 accumulator vs. 8).

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
`duration_cast<milliseconds>` rounds it to 0. The 2.5–3.8x speedups from
breaking the dependency chain are real and visible at -O0/-O1, where the
compiler still emits a literal instruction-per-line translation. Confirming
what actually happened at -O3 requires dumping `-S` and checking for
vectorized instructions (`vmulq`/`vpaddq` and friends) vs. a constant-folded
result, or switching the timer to microseconds.

## 2. `pipeline.tempvar.cpp` — temporaries split a stall chain into two phases

`noTempVars`: one accumulator `x`, updated 4x per iteration, each update
reading `x` that the previous line just wrote (`x = (x + data[i]) * 3 ^ (x >> 2)`,
chained). `withTempVars`: a load phase computes 4 independent temporaries
`t0..t3` from `x0..x3` (unchanged that iteration), then a compute phase
updates each `xi` from only its own `ti` — no cross-dependency in either
phase, so the CPU can overlap all 4 loads and all 4 compute ops.

```
Without temporaries (stalled):   90 ms
With temporaries (pipelined):    28 ms
Speedup: 3.21x
```

The `* 3 ^ (t >> 2)` in both variants isn't a meaningful computation — it's
a complexity anchor. A plain linear recurrence (`x = x*3 + data[i]`) is
simple enough that the compiler can recognize and collapse the whole loop;
XOR-mixing with a shifted copy makes the dependency non-linear so each
iteration has to actually execute, keeping the benchmark honest at
lower `-O` levels.

## 3. `pipeline.cse.cpp` — common subexpression elimination

Three sections, each comparing a "before" variant that recomputes the same
subexpression 3x (forced honest via `NOINLINE` + `doNotOptimize()` fences)
against an "after" variant that computes it once and reuses the result.

| Section | Before | After | Speedup |
|---|---|---|---|
| Integer: `a*b + c` reused 3x | 108.39 ms | 45.19 ms | 2.40x |
| Loop index: `stride*i` reused 3x | 0.17 ms | 0.01 ms | 17x |
| FP: `sqrt(x²+y²+z²)` reused 3x | 20.93 ms | 11.62 ms | 1.80x |

Same underlying mechanism as sections 1–2: each repeated recomputation
re-issues a multi-cycle instruction (multiply, load, or `sqrt`, ~14 cycle
latency) and re-pays its stall, instead of reading an already-available
register.

## 4. `pipeline.dce.cpp` — dead code elimination

Not a stall-elimination technique in the same sense — this is the compiler
removing instructions that never affect an observable result, so they never
enter the pipeline at all (no fetch, no decode, no stall to eliminate).

| Pattern | Before | After |
|---|---|---|
| Dead store (2 overwrites before the live write) | 35.96 ms | 0.00 ms |
| Dead branch (compile-time-false condition) | 0.00 ms | 0.00 ms |
| Dead pure call (return value discarded) | 53.18 ms | 0.00 ms |

The dead-branch case is 0 ms in both variants: the compiler already applies
DCE to a `constexpr false` branch automatically, even in the "before" code —
the "after" version just makes that already-happening elimination explicit
in source. The other two patterns need `doNotOptimize()` fences to stop the
compiler from doing the elimination itself, which is the more realistic
situation: dead code the compiler *can't* prove is dead (e.g. hidden behind
a runtime condition or an opaque call) has to be removed by hand.

## Takeaways

- The dominant cost in every "before" case is a RAW hazard: an instruction
  waiting on a register that isn't ready yet. All four techniques
  (independent accumulators, temporaries, CSE, DCE) are different ways of
  giving the CPU more independent work to overlap while a slow instruction
  (multiply, `sqrt`, memory load) is in flight.
- Effects are clearest at `-O0`/`-O1`. At `-O2`/`-O3` the compiler frequently
  finds the same restructuring itself (or vectorizes, or constant-folds),
  which can make a real effect *look* like it disappeared — always check
  the generated assembly (`-S`) before concluding an optimization had no
  effect at high `-O`.
- Benchmarking code that's "too easy" to optimize away is itself a pitfall:
  several of these files use `doNotOptimize()`/`clobber()` fences or
  bit-mixing (`^ (x >> 2)`) specifically to stop the compiler from proving
  a loop's result is unused or trivially predictable — without that, the
  benchmark measures the optimizer, not the technique.
