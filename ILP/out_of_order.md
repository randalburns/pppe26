# Out-of-Order Execution Example

Demonstrates how a loop-carried dependency chain serializes execution despite
available hardware parallelism, and how independent accumulators break the chain
to expose instruction-level parallelism to the out-of-order engine.

## Source

[out_of_order.cpp](out_of_order.cpp)

## What is out-of-order execution?

Modern CPUs maintain a **reorder buffer (ROB)** that tracks all in-flight
instructions and their readiness.  When an instruction's inputs are available,
the ROB issues it to an execution unit immediately — regardless of program
order.  Instructions that appear later in the code can execute before earlier
ones, as long as no data dependency prevents it.

The constraint is the **data dependency graph**, not the instruction sequence.
If every instruction depends on the result of the previous one, the ROB cannot
reorder anything: it must wait for each result before issuing the next
instruction.  This is a **loop-carried dependency chain**, and it serializes
execution even on wide-issue, out-of-order machines.

---

## How it works

**Serial accumulation — one dependency chain:**

```cpp
double s = 0.0;
for (int i = 0; i < n; i++)
    s += a[i];
```

Every `fadd` reads `s` and writes `s`.  The ROB sees:

```
fadd s, s, a[0]   ← issues; result ready after 3 cycles
fadd s, s, a[1]   ← must wait: reads s, which fadd[0] hasn't written yet
fadd s, s, a[2]   ← must wait for fadd[1] ...
```

With FADD latency = 3 cycles, the loop runs at **one add per 3 cycles**,
regardless of how many FP execution units are available.

**Four independent accumulators — four parallel chains:**

```cpp
double s0=0, s1=0, s2=0, s3=0;
for (int i = 0; i < n; i += 4) {
    s0 += a[i];    s1 += a[i+1];
    s2 += a[i+2];  s3 += a[i+3];
}
```

`s0`, `s1`, `s2`, `s3` share no dependencies.  The ROB issues all four adds
in rapid succession to separate FP pipelines.  With 3-cycle latency, by the
time the ROB needs to issue the next `s0 += a[i+4]`, the result from
`s0 += a[i]` (issued 4 adds ago, 3 cycles ago) is already ready.  The
pipeline is full: **one add issues every cycle**.

---

## The pipeline saturation formula

The minimum accumulator count to saturate one FP unit at throughput 1/cycle:

```
K_min = FADD_latency / issue_throughput = 3 / 1 = 3
```

At `K = 4 >= K_min`, the single FP unit is fully pipelined.  Adding more
accumulators beyond this saturates additional FP units (Apple M5 has 4), until
memory bandwidth becomes the floor:

```
memory floor = array_size / memory_bandwidth = 512 MB / ~100 GB/s ≈ 5 ms
```

---

## Build

```bash
g++ -O1 -o ooo_O1 out_of_order.cpp && ./ooo_O1
```

`-O1` is required.  At `-O2` the compiler auto-vectorises the loops (SIMD),
which is a different optimization that masks the scalar ILP effect.

---

## Results (Apple M5, N=64M doubles, 512 MB)

| version | time | speedup |
|---------|------|---------|
| serial (1 accumulator) | 38 ms | 1.00x |
| 2 accumulators | 17 ms | 2.24x |
| 4 accumulators | 8 ms | **4.75x** |
| 8 accumulators | 7 ms | 5.43x |

---

## Analysis

**1 → 2 accumulators: 2.24x**
Two independent chains run concurrently.  The speedup is close to 2x,
confirming the bottleneck is the serial dependency and not memory bandwidth.

**2 → 4 accumulators: 2.12x additional**
Four chains fully hide the 3-cycle FADD latency on a single FP unit.
The speedup from 1 to 4 is 4.75x — close to the theoretical ceiling of
`FADD_latency = 3x` for one unit, and somewhat above it because the M5
has multiple FP units that begin to share the load.

**4 → 8 accumulators: 1.14x additional**
Diminishing returns.  The single-unit pipeline is already full at K=4.
The modest further gain from K=8 suggests a second FP unit is engaged,
but memory bandwidth (~5 ms floor) is the new constraint.

**Why K=8 doesn't double K=4:**
The M5 can sustain ~100 GB/s on sequential reads.  At 8 ms (K=8), the
computation is close to the memory bandwidth limit.  More accumulators cannot
reduce time below the time required to stream 512 MB from memory.

---

## Key takeaways

1. **A loop-carried dependency chain serializes an out-of-order CPU.**  The
   ROB cannot reorder instructions when every instruction reads the register
   that the previous instruction writes.

2. **Independent accumulators expose ILP without changing the algorithm.**
   The transformation is purely a code restructuring — the mathematical result
   is identical.

3. **The saturation point is `K = latency × units`.**  For one FP unit at
   3-cycle latency, K=3 is sufficient.  On a machine with multiple units, more
   accumulators engage more units until memory bandwidth becomes the ceiling.

4. **The compiler eliminates this problem at `-O2`.**  Auto-vectorization
   converts the scalar accumulation into SIMD, achieving the same effect
   automatically.  `-O1` is needed to observe the raw out-of-order behavior.
