# Separating Dependent Instructions

Demonstrates how placing independent instructions between dependent ones hides
pipeline latency, and how loop fusion enables that placement in practice.

## Source

[sep_dependent.cpp](sep_dependent.cpp)

## What is a data hazard?

A **read-after-write (RAW) hazard** occurs when instruction B reads a register
that instruction A just wrote.  The pipeline must stall until A's result is
ready — typically 3–4 cycles for floating-point operations on modern hardware.

```
fadd sum, sum, x[0]   ← issues; result ready after 3 cycles
fadd sum, sum, x[1]   ← must wait: reads sum, which fadd[0] hasn't written yet
fadd sum, sum, x[2]   ← must wait for fadd[1] ...
```

This is a **loop-carried dependency**: every iteration reads the accumulator
written by the previous iteration.  The loop runs at one add per 3 cycles
regardless of how many execution units are available.

---

## The fix: separate the dependent instructions

Insert independent work between the producer and its consumer.  The independent
instructions fill the pipeline while the result propagates:

```
fadd sum,   sum,   x[i]      ← dep on previous sum   (3-cycle latency)
fmul t,     x[i],  x[i]     ← INDEPENDENT: fills 1 stall cycle
fadd sumsq, sumsq, t         ← dep on previous sumsq  (3-cycle latency)
fadd sum,   sum,   x[i+1]   ← sum dep resolved during sumsq computation
```

The key insight: `sum += x[i]` and `sumsq += x[i]²` are **independent
dependency chains** — sumsq never reads sum.  Interleaving them means
each chain fills the other's stall slots.

---

## How it works

This example computes both `sum = Σ x[i]` and `sumsq = Σ x[i]²`.

**Two separate passes — serialized at the loop boundary:**

```cpp
float s = 0;
for (int i = 0; i < n; i++) s += x[i];      // loop-carried dep on s

float ss = 0;
for (int i = 0; i < n; i++) ss += x[i]*x[i]; // cannot start until loop 1 ends
```

The OOO engine's reorder buffer (ROB) cannot look past the end of the first
loop to begin the second.  Total ≈ 2 × N × FADD_latency cycles.

**Fused single pass — chains run in parallel:**

```cpp
float s = 0, ss = 0;
for (int i = 0; i < n; i++) {
    s  += x[i];           // dep on s
    ss += x[i] * x[i];   // dep on ss — independent of s chain
}
```

By fusing the loops, the compiler places the `fmul` and `sumsq fadd`
*between* consecutive `sum fadd` instructions.  Two independent instructions
now separate each `sum[i]` from `sum[i+1]`, reducing stalls from 2 cycles
to near zero.  Total ≈ N × FADD_latency cycles.

**Fused with 2 accumulators per chain:**

```cpp
float s0=0, s1=0, q0=0, q1=0;
for (int i = 0; i < n; i += 2) {
    s0 += x[i];           s1 += x[i+1];
    q0 += x[i]*x[i];      q1 += x[i+1]*x[i+1];
}
```

Four independent instructions (s1, q0, q1, the fmul) separate consecutive
`s0` updates, exactly matching FADD_latency = 3.  Zero stalls.

---

## The pipeline view

With FADD latency = 3 cycles, the instruction sequence for `fused_1acc` at −O1:

```
cycle 1: fadd s  ← dep on cycle 1-of-prev-iter  (ready cycle 4)
cycle 2: fmul t  ← INDEPENDENT: x[i]*x[i]
cycle 3: fadd ss ← dep on prev ss               (ready cycle 6)
cycle 4: fadd s  ← s from cycle 1 is ready ✓   (no stall)
cycle 5: fmul t
cycle 6: fadd ss ← ss from cycle 3 is ready ✓
...
```

One instruction issues every cycle.  Both chains run at full throughput.

---

## Build

```bash
g++ -O1 -o sep_O1 sep_dependent.cpp && ./sep_O1
```

Use `-O1`.  At `-O2`/`-O3` the compiler auto-vectorises the loops (SIMD),
which achieves the same effect automatically and masks the scalar ILP behavior.

---

## Results (Apple M4, N=64M floats, 256 MB)

| version | time | speedup |
|---------|------|---------|
| two_passes (serial loops) | 90 ms | 1.00x |
| fused_1acc (interleaved) | 53 ms | **1.70x** |
| fused_2acc (2 acc each) | 28 ms | **3.21x** |

---

## Analysis

**two_passes → fused_1acc: 1.70x**

Fusing the loops exposes the independence of the two chains.  The sumsq `fmul`
and `fadd` fill the stall slots of the sum `fadd` chain and vice versa.
Measured cycles/elem drops from 5.36 to 3.16, approaching the theoretical
floor of FADD_latency = 3 cycles.

**fused_1acc → fused_2acc: 1.89x additional**

A second accumulator per chain doubles the independent instructions between
consecutive updates to each variable.  At 4 independent instructions per
latency gap (≥ FADD_latency = 3), all stalls are eliminated.  Cycles/elem
falls to 1.67, approaching the throughput limit of the M4's four FP units.

**Why two_passes is slow:**

The OOO engine tracks in-flight instructions in its reorder buffer (ROB).
The ROB can issue independent instructions out of program order — but only
within the instructions it can see.  A for loop ends with a backwards branch
back to its top.  Before the OOO can fetch the *second* loop's instructions,
it must drain the ROB past the end of the *first* loop.  The loop boundary
is a hard ordering barrier.

Fusing the loops removes the boundary.  Both dependency chains live in the
same ROB window, and the hardware sees their independence directly.

---

## Key takeaways

1. **Dependent instructions must be separated to hide latency.**  A loop that
   accumulates into a single variable runs at one operation per FADD_latency
   cycles regardless of hardware width.

2. **Independent work between producers and consumers hides latency.**
   Any instruction that does not read or write the stalled register fills the
   gap and keeps the pipeline busy.

3. **Loop fusion is the code transformation that enables separation.**
   Merging two sequential loops that iterate over the same range puts their
   independent dependency chains in the same instruction window, allowing the
   OOO engine (and the compiler's scheduler) to interleave them.

4. **The pattern compounds with multiple accumulators.**  Combining loop
   fusion with multiple accumulators per chain (from `out_of_order.cpp`)
   provides both latency hiding and throughput scaling.
