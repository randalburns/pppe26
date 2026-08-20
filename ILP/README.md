# Instruction-level parallelism

## Overview

Instruction-level parallelism (ILP) is the parallel execution of instructions
*within* a single serial thread. It is not concurrency — there is one
instruction stream — but a processor still completes more than one
instruction per cycle by overlapping and reordering the work inside that
stream. Four techniques make this possible:

* **Instruction pipelining** — an instruction executes in stages (fetch,
  decode, execute, ...), and independent instructions overlap those stages
  instead of running start-to-finish one at a time.
* **Out-of-order execution** — the hardware issues each instruction as soon
  as its inputs are ready, not in program order. The limit is the data
  dependency graph, not the instruction sequence: a chain where every
  instruction reads the previous one's result can't be reordered around.
* **Speculative execution** — at a branch, the CPU predicts which way
  control will go and keeps executing down that path rather than stalling
  until the condition resolves. A correct guess costs nothing; a wrong one
  discards everything issued past the branch and restarts.
* **Vector processing (SIMD)** — a single instruction operates on several
  data elements packed into one register at once. This is the one technique
  with a direct programming interface (compiler auto-vectorization or
  intrinsics); see [../vectorization/](../vectorization/) for that side of
  it. Pipelining, out-of-order execution, and speculation, by contrast, are
  managed by the hardware — you don't call them directly. But how you write
  code still determines how well the hardware can use them: a dependency
  chain, a loop boundary, or an unpredictable branch each hide available
  parallelism from the processor, and restructuring the code is often
  enough to expose it. That's what the three examples below measure.

Throughput is usually reported as **cycles per instruction (CPI)** — clock
cycles divided by instructions retired — or its reciprocal, **instructions
per cycle (IPC)**. One instruction per cycle is not a given; it's a ceiling
that data dependencies, stalls, and mispredictions pull you away from, and a
wide out-of-order core with enough exposed parallelism can beat it (CPI < 1)
by retiring several instructions in the same cycle — see the CPI column in
[speculative_execution.md](speculative_execution.md) for a measured example.
Simple instructions typically cost about a cycle and complex ones (integer
division, for instance) cost tens of cycles; for exact per-instruction
latencies on real hardware, see Agner Fog's
[instruction tables](https://www.agner.org/optimize/instruction_tables.pdf).

## The examples

Three examples, each isolating one source of ILP a modern out-of-order CPU can
exploit — or fail to, when the code gets in its way. Measured on Apple M5
(Apple Clang & GCC) and cross-checked on AMD Zen 5 (GCC 13.3.0, AMD Ryzen
AI 9 HX 370); see each write-up for build flags and exact numbers.

Each example is a single source file; build with `g++ <flags> -o <name>
<name>.cpp` and run it directly — no shared Makefile.

---

### 1 · Out-of-order execution — break a loop-carried dependency chain

A single accumulator forces every `fadd` to wait on the previous one; the
reorder buffer can't reorder around a chain where each instruction depends on
the last.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [out_of_order.md](out_of_order.md) | the ROB, the loop-carried dependency chain, and the `K_min = latency / throughput` saturation formula |
| Run  | [out_of_order.cpp](out_of_order.cpp) | sum-reduction over 64M doubles with 1, 2, 4, and 8 independent accumulators |
| Compare | [out_of_order.ryzen.md](out_of_order.ryzen.md) | the same rerun on a Zen 5 P-core |

Splitting one dependency chain into four independent ones hides the 3-cycle
FADD latency: 38 ms → 8 ms (**4.75x**), then flattens as memory bandwidth
becomes the floor. Zen 5 measures the same 3-cycle FADD latency and the same
mechanism, but tops out at **3.08x** — its single-core memory bandwidth wall
arrives at K=4 instead of K=8, so the gap between chips here is a memory
story, not an ILP one.

---

### 2 · Separating dependent instructions — fuse loops to interleave chains

Two separate accumulation loops each stall on their own dependency chain, and
the loop boundary is a hard ordering barrier the ROB can't see past. Fusing
them puts both chains in the same instruction window.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [sep_dependent.md](sep_dependent.md) | why a loop boundary blocks the ROB from reordering across it, and how independent work fills the stall slots of a dependent chain |
| Run  | [sep_dependent.cpp](sep_dependent.cpp) | `sum` and `sumsq` over 64M floats — two passes, fused single-pass, fused with 2 accumulators per chain |
| Compare | [sep_dependent.ryzen.md](sep_dependent.ryzen.md) | the same rerun on a Zen 5 P-core |

Fusing exposes the two chains' independence: 90 ms → 53 ms (**1.70x**);
doubling accumulators per chain removes the remaining stalls: 53 ms → 28 ms
(**3.21x** total). Zen 5 does even better here — **2.00x** then **3.81x** —
because fusion also strips a whole loop's worth of x86 pointer/compare/branch
overhead, and the working set is small enough to stay latency-bound rather
than hitting the bandwidth wall seen in example 1.

---

### 3 · Speculative execution — the cost of guessing wrong

A branch the CPU can't predict flushes 15+ cycles of speculative work on
every miss. Sorting the data (or removing the branch entirely) removes the
guesswork.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [speculative_execution.md](speculative_execution.md) | why sorted vs. shuffled data isolates prediction accuracy from instruction count, and why Apple Clang's automatic `csel` conversion hides the effect at `-O1` |
| Run  | [speculative_execution.cpp](speculative_execution.cpp) | a threshold filter over 32M ints — branchy+shuffled, branchy+sorted, branchless |
| Compare | [speculative_execution.ryzen.md](speculative_execution.ryzen.md) | the same rerun on a Zen 5 P-core |

Sorting turns ~50% misprediction into one mispredict total: 85 ms → 8 ms
(**10.62x**) — matched exactly by the branchless version, since instruction
count is constant and speedup here equals the CPI ratio. Zen 5 reproduces the
misprediction result almost exactly (**10.78x**, an ~18-cycle penalty on both
chips), but *not* the branchless tie: `cmov` sits inside x86's accumulator
dependency chain, so branchless is only **7.46x** there — a real trade, not a
free win, that the M5's wider issue happens to hide.

---

### Files at a glance

**Sources** — `out_of_order.cpp`, `sep_dependent.cpp`, `speculative_execution.cpp`
**Write-ups (Apple M5)** — `out_of_order.md`, `sep_dependent.md`, `speculative_execution.md`
**Write-ups (AMD Zen 5)** — `out_of_order.ryzen.md`, `sep_dependent.ryzen.md`, `speculative_execution.ryzen.md`
**Assembly** — `ooo.s`, `sep_dep.s`, `spec.s` — reference disassembly used in the write-ups
**Binaries** — `ooo_O1`, `sep_O0`, `sep_O1`, `spec_O1` — prebuilt from the commands in each write-up

All three examples share one theme: the out-of-order engine can only exploit
the parallelism it can *see*. A loop-carried dependency, a loop boundary, and
an unpredictable branch are three different ways of hiding available
parallelism from the ROB — and independent accumulators, loop fusion, and
branchless arithmetic are the matching fixes.

The Zen 5 reruns confirm the mechanisms are architecture-independent — each
fix works for the same reason on both chips — while showing where the
*numbers* diverge: Zen 5's single-core memory bandwidth ceiling arrives
sooner (example 1), its x86 addressing modes make loop fusion pay off more
(example 2), and its `cmov` lowering reintroduces a data dependency that
makes branchless code a real trade rather than a free win (example 3).
