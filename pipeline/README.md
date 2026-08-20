# Pipeline Stalls

## What is a pipeline

A CPU doesn't run one instruction to completion before starting the next.
It splits each instruction into stages — fetch, decode, execute, access
memory, write the result back — and works on several instructions at once,
each sitting in a different stage at the same time, the way a factory
assembly line has a different car under each station simultaneously. When
every instruction is independent of its neighbors, a new one can enter the
pipeline every cycle, and the pipeline retires one instruction per cycle
even though each individual instruction takes several cycles start to
finish.

That throughput depends on independence. If instruction B needs a value
that instruction A hasn't finished computing — a **RAW (Read-After-Write)
hazard** — B can't enter its execute stage until A's result exists. The
pipeline **stalls**: it inserts bubble cycles instead of useful work,
waiting for the value B needs. A loop whose accumulator is read and written
every iteration hits this hazard once per element, and the loop runs no
faster than the latency of that one dependency, no matter how many
execution units the chip has sitting idle.

Not all instructions stall the same amount, because not all operations take
the same number of cycles to produce a result. A simple integer add is
typically ready in about a cycle; a floating-point multiply or add usually
costs a handful of cycles; division and functions like `sqrt` cost tens of
cycles. The longer an instruction's latency, the more a dependent
instruction behind it has to wait — and the more valuable it becomes to
give the pipeline *other*, independent work to fill that wait with instead
of leaving it idle. That's the single idea behind every example below:
breaking or hiding a dependency chain so the pipeline has something to do
while a slow result is still in flight.

## The pipeline on this machine

The examples here were built and measured on Apple silicon (M-series),
clocked at a measured/assumed **4.0 GHz** — the constant these benchmarks
use to convert wall-clock time into cycles. Some relevant, independently
measured characteristics of this specific pipeline, drawn from the more
detailed cycle-level analysis in [../ILP/](../ILP/):

- **10-wide superscalar, out-of-order.** The core can have many
  instructions in flight at once and issue several per cycle when they're
  independent — [speculative_execution.md](../ILP/speculative_execution.md)
  measures a sustained CPI of 0.14 (about 7 instructions retiring per
  cycle) on a loop with no stalls, well below the "1 instruction per cycle"
  a purely sequential model would predict.
- **Floating-point add latency: 3 cycles**, across **4 independent FP
  execution units** — [out_of_order.md](../ILP/out_of_order.md) derives the
  minimum accumulator count needed to keep one FP unit fully busy
  (`K_min = latency / throughput = 3`), then shows the four independent
  accumulators in `pipeline.cpp` engaging more than one of those units at
  once.
- **Branch misprediction flush cost: ~15 cycles nominal, ~18 measured.**
  [speculative_execution.md](../ILP/speculative_execution.md) assumes 15
  cycles per wrong guess; cross-checking the arithmetic against measured
  timings (and against a Zen 5 machine benchmarked the same way) puts the
  real number closer to 18 on this chip.

None of the examples below need those exact numbers to make sense, but
they're what "the pipeline stalls" cashes out to concretely on this
hardware: a wrong guess or an unready operand costs a specific, measurable
number of cycles, and the point of every technique here is to avoid paying
that cost more than once.

Each example is a single source file; build and run it directly — no
shared Makefile.

---

## 1 · Dependent chains vs. independent accumulators

A single accumulator forces every step to wait on the one before it; several
independent accumulators give the pipeline other work to do while each
result is still in flight.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [pipeline.md](pipeline.md) | the RAW hazard, and why a 0 ms result at `-O2`/`-O3` means the compiler solved the problem for you, not that it went away |
| Run  | [pipeline.cpp](pipeline.cpp) | dependent-chain vs. 4 independent accumulators, and single vs. 8 accumulators over a plain sum, swept across `-O0`..`-O3` |

Breaking one dependency chain into four independent ones: 496 ms → 198 ms
at `-O0` (**2.51x**), widening to **3.59x** at `-O1` before the compiler
starts solving it automatically at `-O2`/`-O3`.

---

## 2 · Temporaries — splitting a stall chain into two phases

A load phase and a compute phase, each internally independent, let the
pipeline overlap all four elements of an unrolled iteration instead of
serializing them one at a time.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [pipeline.tempvar.md](pipeline.tempvar.md) | why splitting a computation into independent phases hides the same hazard as parallel accumulators, plus a worked diagnosis of when a benchmark's timer is the thing that's lying, not the hardware |
| Run  | [pipeline.tempvar.cpp](pipeline.tempvar.cpp) | a single-accumulator multiply-XOR chain vs. a load-phase/compute-phase split, over 100M elements |

Splitting the stall chain into independent phases: 90 ms → 28 ms
(**3.21x**).

---

## 3 · Common subexpression elimination — pay the stall once

Recomputing the same expression re-issues the same multi-cycle instruction
and re-pays its stall every time; caching the result pays it once.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [pipeline.cse.md](pipeline.cse.md) | three latencies side by side (multiply, address arithmetic, `sqrt`), and why the effect's size depends on how much of total runtime the redundant stall actually accounts for |
| Run  | [pipeline.cse.cpp](pipeline.cse.cpp) | `a*b+c`, a loop-index calculation, and `sqrt(x²+y²+z²)`, each reused 3x, before vs. after CSE |

Caching instead of recomputing: 2.40x (integer multiply), **17x** (loop
index, reused inside a hot loop), 1.80x (`sqrt`).

---

## 4 · Dead code elimination — work that never enters the pipeline

Different in kind from the other three: this is about instructions that
never get fetched at all, because their result is never observable.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [pipeline.dce.md](pipeline.dce.md) | three patterns (dead store, dead branch, dead call) and why a compile-time-constant branch is already free, with no runtime win left to claim |
| Run  | [pipeline.dce.cpp](pipeline.dce.cpp) | each pattern, before vs. after removing the dead code by hand |

Removing genuinely unobservable work: 35.96 ms → 0 ms (dead store), 53.18 ms
→ 0 ms (dead call) — the dead-branch case is 0 ms either way, since the
compiler already strips a `constexpr false` branch on its own.

---

## Files at a glance

**Sources** — `pipeline.cpp`, `pipeline.tempvar.cpp`, `pipeline.cse.cpp`, `pipeline.dce.cpp`
**Write-ups** — `pipeline.md`, `pipeline.tempvar.md`, `pipeline.cse.md`, `pipeline.dce.md`
**Build** — `buildandrun.sh` sweeps `pipeline.cpp` across `-O0`..`-O3`; the other three sources document a single `clang++` command in their own write-up

The first two examples break a dependency chain apart (more accumulators,
or a load/compute phase split); the third avoids re-paying a stall that's
already been paid once; the fourth removes work that was never going to
affect the result at all. [../ILP/](../ILP/) picks up where the first two
leave off, with cycle-accurate analysis of the same dependent-chain
mechanism on doubles instead of ints, plus a Zen 5 comparison this
directory doesn't have.
