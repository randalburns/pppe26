# Speculative Execution Example

Demonstrates the cost of branch misprediction by running the same conditional
loop over sorted vs randomly shuffled data, isolating the effect of speculation
accuracy on pipeline performance.

## Source

[speculative_execution.cpp](speculative_execution.cpp)

## What is speculative execution?

When a CPU reaches a conditional branch, it does not know the outcome until the
condition has been evaluated — which may be several cycles later due to pipeline
depth.  Rather than stalling, the CPU **predicts** the branch direction and
continues fetching and executing instructions along the predicted path.  This is
speculative execution.

- **Correct prediction:** the speculative work commits; the pipeline runs at
  full utilization as if the branch did not exist.
- **Wrong prediction:** the pipeline is flushed back to the branch point; all
  instructions issued along the wrong path are discarded.  On Apple M4 this
  flush costs approximately **15 cycles**.

The branch predictor learns patterns from recent branch history.  Branches that
follow a predictable pattern (always taken, never taken, alternating) are
predicted with near-perfect accuracy.  Random or data-dependent branches with
no repeating structure approach 50% accuracy — equivalent to a coin flip.

---

## How it works

Both versions of the branchy loop execute **identical instructions** on
**identical data values**.  Only the order in which elements are visited differs.

**Shuffled data — unpredictable branch:**

```cpp
if (data[i] >= THRESHOLD)   // data in random order
    sum += data[i];
```

Values are uniformly distributed in `[0, 255]` with `THRESHOLD = 128`.
Approximately 50% of branches are taken, in no repeating pattern.  The
predictor is correct ~50% of the time.

Expected overhead per element: `0.5 mispredict × 15 cycles = 7.5 extra cycles`

**Sorted data — predictable branch:**

Same code, same values, sorted ascending.  All elements below threshold appear
first; all elements above appear last.  The predictor learns "not taken" for the
first half and "taken" for the second, mispredicting exactly once at the
transition.  The 15-cycle penalty is paid once across all N elements.

**Branchless — no branch instruction:**

```cpp
sum += (int64_t)v * (v >= THRESHOLD);
```

The boolean `(v >= THRESHOLD)` evaluates to 0 or 1.  Multiplying by `v` gives
0 or `v` with no branch instruction in the generated code — no prediction, no
speculation, no flush possible.  Performance is independent of data order.

---

## Compiler note

Apple Clang converts simple `if`-statements to `csel` (conditional select) even
at `-O1`, eliminating the branch instruction before we can observe its cost.
GCC with `-fno-if-conversion` preserves the real branch.

```bash
g++-15 -O1 -fno-if-conversion -o spec_O1 speculative_execution.cpp && ./spec_O1
```

Verifying with the assembler output (`g++-15 -O1 -fno-if-conversion -S`):

```
; GCC -O1 -fno-if-conversion: real branch
ldr   w1, [x2]
cmp   w1, 127
ble   skip          ← conditional branch — subject to misprediction
add   x0, x0, w1, sxtw
skip: ...

; Apple Clang -O1: conditional select (no branch)
ldr   w10, [x0], #4
cmp   w10, #127
csel  w10, w10, wzr, gt   ← no branch; always executes both paths
add   x8, x8, x10
```

The fact that Clang converts to `csel` automatically is itself a lesson: the
compiler applies the same branchless transformation the programmer can apply
manually, eliminating the misprediction penalty without any source change.

---

## Inner loop instruction count

From the GCC `-O1 -fno-if-conversion` assembly, the inner loop has:

| path | instructions |
|------|-------------|
| not taken (`data[i] < THRESHOLD`) | ldr, cmp, ble, add(ptr), cmp(ptr), beq = **6** |
| taken (`data[i] >= THRESHOLD`) | same 6 + add(sum) + b(loop) = **8** |
| average at 50% taken | **7 insns/elem** |

The instruction count is identical across all three benchmark versions —
sorted, shuffled, and branchless — so speedup equals the CPI ratio directly.

---

## Results (Apple M4, N=32M ints, 128 MB, threshold=128)

| version | time | cycles/elem | CPI | speedup |
|---------|------|-------------|-----|---------|
| branchy + shuffled (50% mispredict) | 85 ms | 10.13 | 1.45 | 1.00x |
| branchy + sorted (1 mispredict) | 8 ms | 0.95 | 0.14 | **10.62x** |
| branchless + shuffled (no branch) | 8 ms | 0.95 | 0.14 | **10.62x** |

Expected misprediction overhead: `32M × 0.5 × 15 cycles / 4 GHz ≈ 60 ms` added
to the base 8 ms gives ~68 ms predicted vs 85 ms measured — close, with the
remainder attributable to pipeline refill after each flush.

---

## Correlation: speedup = CPI ratio

Because instruction count is the same for all three versions, time is
proportional to CPI, and speedup equals the CPI ratio exactly:

```
speedup = CPI_shuffled / CPI_sorted = 1.45 / 0.14 = 10.6x  [measured: 10.6x]
```

The 0.14 CPI of the sorted version (< 1) reflects the M4's 10-wide
superscalar OOO engine issuing ~7 instructions per cycle across multiple
in-flight iterations when no branch stalls occur.

---

## Key takeaways

1. **Branch misprediction serializes the pipeline.** Each wrong guess discards
   15+ cycles of speculative work and re-fetches from the correct path.  At 50%
   misprediction, roughly half the CPU's time is wasted on discarded work.

2. **Data order determines prediction accuracy.** The same code, the same
   values, the same instruction count — only memory layout differs.  Sorted
   data gives 10x better performance than random data on this benchmark.

3. **Branchless code avoids the problem entirely.** Replacing a conditional
   branch with arithmetic (`v * (v >= threshold)`) removes the prediction
   problem at the cost of always doing the multiply.  On random data, branchless
   matches sorted performance.

4. **Modern compilers apply branchless transformation automatically.** Apple
   Clang at `-O1` emits `csel` for the simple `if`, making the compiler-
   generated code as fast as the hand-written branchless version.
   `-fno-if-conversion` (GCC) is required to expose the raw hardware effect.

5. **Speedup = CPI ratio when instruction count is constant.** Unlike the
   out-of-order accumulator example (where both CPI and instruction count
   change), here only CPI changes — making the speedup a direct, unambiguous
   measure of the misprediction cost.
