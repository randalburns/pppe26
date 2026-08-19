# Loop Unswitching

Demonstrates moving a loop-invariant branch outside the loop so each inner
loop body is branch-free and the compiler can apply SIMD vectorization.

## Source

[loop_unswitch.cpp](loop_unswitch.cpp)

## The transformation

A **loop-invariant branch** evaluates to the same outcome on every iteration —
it depends on a value that doesn't change during the loop.  Leaving it inside
wastes N comparisons and, more importantly, blocks the compiler from generating
SIMD vector code.

```cpp
// Switched: branch checked every iteration — compiler generates scalar code
void apply_switched(const float* a, const float* b, float* c, int n, int mode) {
    for (int i = 0; i < n; i++) {
        if      (mode == 0) c[i] = a[i] + b[i];
        else if (mode == 1) c[i] = a[i] * b[i];
        else                c[i] = fabsf(a[i]) + fabsf(b[i]);
    }
}

// Unswitched: branch checked once — each inner loop is clean and vectorizes
void apply_unswitched(const float* a, const float* b, float* c, int n, int mode) {
    if (mode == 0)
        for (int i = 0; i < n; i++) c[i] = a[i] + b[i];
    else if (mode == 1)
        for (int i = 0; i < n; i++) c[i] = a[i] * b[i];
    else
        for (int i = 0; i < n; i++) c[i] = fabsf(a[i]) + fabsf(b[i]);
}
```

`mode` is a function argument — it does not change during the loop.

---

## Why it matters: vectorization

The key benefit is not branch prediction (the loop-invariant branch is
perfectly predicted — always the same outcome).  The key benefit is that
a branch inside the loop body prevents the compiler from generating SIMD
instructions.

```
switched   → scalar:   fadd  s0, s0, s1     (1 float per instruction)
unswitched → SIMD:     fadd.4s  v0, v0, v1  (4 floats per instruction)
```

The clean inner loops in the unswitched version let the compiler process
four elements per instruction using 128-bit NEON registers.

---

## Build

```bash
clang++ -O2 -o loop_unswitch loop_unswitch.cpp && ./loop_unswitch
```

(The original build line was `g++ -O2`, which on macOS is Apple Clang — so the
M5 results below are already Clang results.  Naming the compiler explicitly
keeps the cross-machine comparison honest.)

`-O2` is required.  At `-O3` the compiler performs loop unswitching
automatically, closing the performance gap.  The manual transformation
ensures the benefit at any optimization level and makes the invariant
explicit in source.

---

## Results (Apple M5, N=32M floats)

| mode | switched | unswitched | speedup |
|------|----------|------------|---------|
| 0 — add | 13 ms | 4 ms | **3.25x** |
| 1 — mul | 8 ms | 4 ms | **2.00x** |
| 2 — abssum | 12 ms | 4 ms | **3.00x** |

All three unswitched loops converge to 4 ms: memory bandwidth limited
(384 MB / ~100 GB/s ≈ 3.8 ms).  The switched loops are compute-limited
by their scalar throughput.

---

## Key takeaways

1. **Loop-invariant branches belong outside the loop.**  The test is: does the
   condition ever change while `i` advances?  If not, hoist it.

2. **The benefit is vectorization, not misprediction.**  The branch is
   perfectly predictable (always same outcome), but its presence prevents
   SIMD code generation.

3. **The compiler does this at `-O3` but not `-O2`.**  Writing the
   transformation explicitly guarantees the benefit at all optimization
   levels and documents the intent clearly.
