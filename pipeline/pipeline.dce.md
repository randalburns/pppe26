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
clang++ -O2 -o pipeline.dce pipeline.dce.cpp && ./pipeline.dce
```

## Results (Apple M-series, `-O2`, N=100M iterations)

| Pattern | Before | After |
|---|---|---|
| Dead store (2 overwrites before the live write) | 35.96 ms | 0.00 ms |
| Dead branch (compile-time-false condition) | 0.00 ms | 0.00 ms |
| Dead pure call (return value discarded) | 53.18 ms | 0.00 ms |

## Analysis

The dead-branch case is 0 ms in *both* variants: the compiler already
applies DCE to a `constexpr false` branch automatically, even in the
"before" code — the "after" version just makes that already-happening
elimination explicit in source. The other two patterns need
`doNotOptimize()` fences to stop the compiler from doing the elimination
itself, which is closer to the realistic case: dead code the compiler
*can't* prove is dead — because it's hidden behind a runtime condition or
an opaque function call the compiler can't see through — has to be removed
by hand, or it silently costs real cycles forever.

This is really more of a compiler-optimization fact than a coding
technique: the honest takeaway is to not write code that doesn't do
anything, and to trust the compiler to clean up what it can prove is
provably dead.

## Key takeaways

1. **Dead code costs real cycles when the compiler can't prove it's dead.**
   A discarded pure-function call or an overwritten store still executes if
   nothing tells the optimizer the result is unused.
2. **A compile-time-constant branch is free — the compiler already removes
   it.** The "optimization" here is source clarity, not a runtime win.
3. **`doNotOptimize()` fences exist to make an already-fast case visible for
   measurement**, not because real code should routinely fight the
   optimizer — in most code, "don't write dead code" is the whole
   technique.
