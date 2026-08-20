# Dead Code Elimination: Work That Never Enters the Pipeline

The other three write-ups in this directory are about hiding a stall once
an instruction is already in the pipeline. Dead code elimination (DCE) is
different in kind: it removes instructions whose results are never
observable, so they never get fetched or decoded at all — no stall to
eliminate, because there's nothing there to stall on.

## Source

[pipeline.dce.cpp](pipeline.dce.cpp)

## How it works

Three patterns, each with a `before` version the compiler is prevented from
optimizing (`doNotOptimize()` fences, or a runtime-opaque call) and an
`after` version with the dead code removed by hand:

**1. Dead store** — a value written but overwritten before any read:

```cpp
long long x = 0;
for (int i = 0; i < n; i++) {
    x = i * i;              // dead: overwritten next line before read
    x = i * i * i;         // dead: overwritten on the line below
    x = i + 1;             // live: this is the value actually accumulated
    doNotOptimize(x);      // prevent the compiler from applying DCE for us
}
```

**2. Dead branch** — a compile-time-constant condition makes one arm
permanently unreachable:

```cpp
static constexpr bool FEATURE_ENABLED = false;
for (int i = 0; i < n; i++) {
    sum += i;
    if (FEATURE_ENABLED) {          // always false — dead branch
        sum *= i;                   // dead: block never entered
        sum ^= (sum >> 3);          // dead
    }
}
```

**3. Dead pure call** — the return value of a side-effect-free function is
discarded:

```cpp
for (int i = 0; i < n; i++) {
    expensivePure(i);    // return value discarded — dead call
    doNotOptimize(sum);  // prevent hoisting sum out of the loop
    sum += i;
}
```

## Build

```bash
clang++ -O1 -o pipeline.dce pipeline.dce.cpp && ./pipeline.dce
```

## Verified optimization levels (Apple M-series, N=100M iterations, min of 3 runs)

| Level | Dead store | Dead branch | Dead pure call |
|---|---|---|---|
| `-O0` | 104.83 ms → 43.82 ms | 47.04 ms → 47.09 ms (**no win**) | 5782 ms → 47.38 ms |
| `-O1` | 22.55 ms → **0.00 ms** | 0.00 ms → 0.00 ms | 28.14 ms → **0.00 ms** |
| `-O2` | 22.55 ms → **0.00 ms** | 0.00 ms → 0.00 ms | 25.10 ms → **0.00 ms** |
| `-O3` | 22.58 ms → **0.00 ms** | 0.00 ms → 0.00 ms | 28.15 ms → **0.00 ms** |

**`-O1` through `-O3` all show the intended pattern**, and are effectively
interchangeable for this example — dead store and dead call both collapse
to 0.00 ms "after," and dead branch is 0.00 ms either way.

**`-O0` is a genuinely different case, not just a weaker one.** Two things
change:

1. **The dead-branch "free win" disappears.** Without optimization enabled,
   the compiler doesn't fold the `constexpr false` condition at all — it
   compiles a real (if perfectly predicted and cheap) branch, so `before`
   and `after` are statistically indistinguishable (~47 ms either way).
   This is the one row where "before vs. after" shows nothing, because the
   thing being demonstrated — automatic DCE of a compile-time-false branch
   — is itself an optimization, and `-O0` turns optimizations off.
2. **The dead call becomes enormously expensive: ~5.8 seconds**, roughly
   100x its cost at `-O1`+. `expensivePure()`'s internal 64-iteration
   mixing loop gets no optimization of its own at `-O0`, so every discarded
   call still executes the full unoptimized loop — the "before" cost here
   isn't really about DCE at all, it's the raw cost of calling an
   unoptimized function 100M times.

## Analysis

At `-O1` and above, the dead-branch case is 0 ms in *both* variants: the
compiler already applies DCE to a `constexpr false` branch automatically,
even in the "before" code — the "after" version just makes that
already-happening elimination explicit in source. The other two patterns
need `doNotOptimize()` fences to stop the compiler from doing the
elimination itself, which is closer to the realistic case: dead code the
compiler *can't* prove is dead — because it's hidden behind a runtime
condition or an opaque function call the compiler can't see through — has
to be removed by hand, or it silently costs real cycles forever.

This is really more of a compiler-optimization fact than a coding
technique: the honest takeaway is to not write code that doesn't do
anything, and to trust the compiler to clean up what it can prove is
provably dead.

## Key takeaways

1. **Dead code costs real cycles when the compiler can't prove it's dead.**
   A discarded pure-function call or an overwritten store still executes if
   nothing tells the optimizer the result is unused.
2. **A compile-time-constant branch is free — but only once optimization is
   turned on.** At `-O0` the "free win" this example demonstrates doesn't
   exist yet; it's an artifact of the same optimization passes that also
   remove the dead store and dead call starting at `-O1`.
3. **`doNotOptimize()` fences exist to make an already-fast case visible for
   measurement**, not because real code should routinely fight the
   optimizer — in most code, "don't write dead code" is the whole
   technique.
4. **Unoptimized code isn't just "slower" — it can change which pattern
   dominates.** At `-O0`, calling an unoptimized pure function 100M times
   (~5.8 s) dwarfs the cost of the other two patterns combined; at `-O1`+ it
   collapses to the smallest of the three. Which "dead code" pattern costs
   the most depends entirely on the optimization level you measure it at.
