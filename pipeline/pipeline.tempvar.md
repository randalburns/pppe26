# Temporaries: Splitting a Stall Chain into Independent Phases

A single accumulator updated by a multi-step computation stalls on itself —
every step reads the value the step above just wrote. Splitting the loop
body into a load phase and a compute phase, each internally independent,
lets the CPU overlap all four elements of an unrolled iteration instead of
serializing them.

## Source

[pipeline.tempvar.cpp](pipeline.tempvar.cpp)

## How it works

**Without temporaries — each step reads `x` the line above just wrote:**

```cpp
long long x = 1;
for (int i = 0; i < ARRAY_SIZE; i += 4) {
    x = (x + data[i])     * 3 ^ (x >> 2);   // stall: reads x just written
    x = (x + data[i + 1]) * 3 ^ (x >> 2);   // stall: depends on line above
    x = (x + data[i + 2]) * 3 ^ (x >> 2);   // stall: depends on line above
    x = (x + data[i + 3]) * 3 ^ (x >> 2);   // stall: depends on line above
}
```

**With temporaries — a load phase, then a compute phase, each independent:**

```cpp
long long x0 = 1, x1 = 2, x2 = 3, x3 = 4;
for (int i = 0; i < ARRAY_SIZE; i += 4) {
    // LOAD PHASE: four independent additions into temporaries
    long long t0 = x0 + data[i];
    long long t1 = x1 + data[i + 1];   // independent of t0
    long long t2 = x2 + data[i + 2];   // independent of t0, t1
    long long t3 = x3 + data[i + 3];   // independent of t0, t1, t2

    // COMPUTE PHASE: four independent multiply-XORs, each uses only its own temp
    x0 = t0 * 3 ^ (t0 >> 2);
    x1 = t1 * 3 ^ (t1 >> 2);
    x2 = t2 * 3 ^ (t2 >> 2);
    x3 = t3 * 3 ^ (t3 >> 2);
}
```

Neither phase has a cross-dependency: all four loads in phase one are
independent of each other, and each compute in phase two depends only on
its own temporary, not on any other chain. The pipeline never has to wait
for a value it needs right now — it always has three other independent
operations to fill the gap with.

The `* 3 ^ (t >> 2)` in both variants isn't a meaningful computation — it's
a complexity anchor. A plain linear recurrence (`x = x*3 + data[i]`) is
simple enough that the compiler can recognize and collapse the whole loop;
XOR-mixing with a shifted copy makes the dependency non-linear so each
iteration has to actually execute, keeping the benchmark honest.

## Build

```bash
clang++ -std=c++17 -O2 -o pipeline.tempvar pipeline.tempvar.cpp && ./pipeline.tempvar
```

## Results (Apple M-series, N=100M ints)

```
Without temporaries (stalled):   90 ms
With temporaries (pipelined):    28 ms
Speedup: 3.21x
```

## A diagnostic note: why timers can lie at high `-O`

The same benchmark, timed in milliseconds, can misreport a real effect as
"gone" once the compiler starts auto-vectorizing or constant-folding at
higher optimization levels — see [pipeline.md](pipeline.md)'s `-O2`/`-O3`
rows for the direct example. Two things can cause a 0 ms result:

1. **Auto-vectorization.** `withTempVars` is a perfect vectorizer candidate
   — four independent chains, no cross-dependencies — so the compiler can
   rewrite it into SIMD instructions that finish in microseconds, which
   `duration_cast<milliseconds>` rounds down to 0.
2. **The computation gets eliminated entirely.** If every intermediate
   value lives in a register and never touches memory, an aggressive
   optimizer can see through the whole loop and constant-fold the result at
   compile time — timing a benchmark that no longer does any real work.

To tell which one happened, dump the assembly (`-S`) and look for packed
instructions (`vmulq`/`vpaddq` and similar — vectorized) versus the result
being loaded as an immediate constant (folded away). Switching the timer to
microseconds is the quick fix if the goal is just to see a nonzero number.

## Key takeaways

1. **Splitting a computation into an independent load phase and an
   independent compute phase** hides the same RAW hazard `pipeline.md`
   breaks with parallel accumulators — same principle, different shape.
2. **A benchmark that's too easy to optimize measures the optimizer, not
   the technique.** The bit-mixing here exists specifically to stop the
   compiler from collapsing the loop into a closed form.
3. **A 0 ms (or suspiciously fast) result needs a second look** before
   being read as "no effect" — check the generated assembly before drawing
   conclusions from wall-clock time alone.
