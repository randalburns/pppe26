# Loop optimizations

Five classic loop transformations, each isolated in its own example so the
win (or the surprising non-win) is unambiguous. Measured on Apple M-series /
Apple Clang; see each write-up for build flags and exact numbers.

Each example is a single source file; build with `g++ <flags> -o <name>
<name>.cpp` and run it directly — no shared Makefile.

---

## Why loops

Most of the time a real program spends is spent inside a small number of
loops — the ones that touch every element of an array, walk a matrix, or
accumulate a sum. A program might run thousands of distinct lines of code
over its lifetime, but a handful of loop bodies account for nearly all the
actual work, because each one executes not once but thousands, millions, or
billions of times. Shave one cache miss or one wasted instruction off a
single iteration of a loop that runs a million times, and you've removed a
million cache misses or a million wasted instructions. That leverage — fix
it once, benefit every iteration — is why loops get disproportionate
attention compared to code that only runs once.

It's also *why loops are unusually amenable to a fixed toolkit*. A loop body
is the same operation applied to different data, iteration after iteration,
so a handful of general-purpose rewrites — reorder the iterations, split the
loop, merge two loops, do several iterations at once — tend to work across
completely unrelated problems: image filters, matrix multiplication,
running sums, string search. That's what this directory is: five such
rewrites, each demonstrated on the example where its effect is clearest.

Importantly, none of the five change *what* the loop computes — only how the
work inside it is arranged, so the compiler, the cache, and the CPU's
execution units spend less time waiting and more time working. Whether a
given rearrangement is even allowed — whether it's still guaranteed to
compute the same thing — comes down to one question, covered next.

---

## Loop independence — what makes a transformation legal

Every rewrite below reorders, splits, merges, regroups, or overlaps loop
iterations. That's only a valid rewrite — same answer, different code — if
the iterations are **independent**: iteration `i`'s computation doesn't
depend on any other iteration's result, and doesn't feed into one either.
Equivalently, there's no *loop-carried dependency* — no value written in one
iteration that a different iteration reads.

That's what each transformation actually needs independence for:

* **Fission** splits one loop into several — legal when nothing in the body
  spans the split point.
* **Fusion** merges two loops into one — legal when neither loop's iteration
  needs output the other loop hasn't produced yet at the point they'd now
  run interleaved.
* **Interchange** swaps which index is outer and which is inner — legal when
  neither index's loop feeds into the other's computation.
* **Tiling** reorders iterations into blocks — the same requirement as
  interchange, at a coarser grouping.
* **Unrolling** runs several iterations per pass through the loop-control
  code — legal for the same reason interchange is: overlapping iterations
  changes nothing if they don't depend on each other.

A loop where iteration `i` genuinely needs iteration `i-1`'s result — a
running sum, a recurrence, an accumulator — *is* loop-carried, and none of
these rewrites are free there. [../ILP/out_of_order.md](../ILP/out_of_order.md)
and [../ILP/sep_dependent.md](../ILP/sep_dependent.md) measure what a
loop-carried dependency costs on real hardware, and how breaking one apart
(multiple accumulators) or hiding it (interleaving independent work)
recovers performance without changing the loop's dependency structure.

**Independent isn't automatically "safe to reorder bit-for-bit," though.**
[loop_fusion.md](loop_fusion.md)'s mean/variance example makes the case: the
two-pass and fused versions both sum the same values in the same left-to-right
order — genuinely independent per-element work, combined by one reduction —
so fusing them doesn't touch the arithmetic. But floating-point addition
isn't associative, so the compiler won't auto-vectorize either version's
reduction without `-ffast-math`: packing several additions into one SIMD
instruction changes the order partial sums combine in, which can change the
last bit of the result. Welford's algorithm, by contrast, maintains a running
mean and variance that genuinely *is* loop-carried — each iteration needs the
previous iteration's mean — so it can't be reordered at all, and pays for
that with a serial dependency chain despite doing the fusion "for free" in a
single pass.

So there are really two questions hiding inside "independent": can this loop
be reordered at all (the dependency-graph question — what makes a
transformation legal), and does reordering it change the answer (the
floating-point-associativity question — why even a legal reordering needs a
flag like `-ffast-math` to happen automatically). Every example below has
settled the first question already; only loop fusion's FP reduction runs
into the second.

---

## 1 · Loop fission — trade a memory pass for fewer live registers

Split a wide loop body into narrower loops to cut register pressure and
eliminate spills to the stack.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [loop_fission.md](loop_fission.md) | why 32 simultaneous accumulators exhaust ARM64's register file, and how to read spill/reload annotations in the assembly |
| Run  | [loop_fission.cpp](loop_fission.cpp) | 32 simultaneous cross-correlations, split into two loops of 16 |

Fissioning trades an extra read of the signal array for the removal of
per-iteration FP spills: 202 ms → 90 ms (**2.24x**).

---

## 2 · Loop fusion — trade compute for a memory pass

Merge two loops that scan the same data into one, when the data dependency
between them can be worked around algebraically.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [loop_fusion.md](loop_fusion.md) | using Var(X) = E[X²] − E[X]² to fuse mean+variance into one pass, and why the "textbook" single-pass alternative (Welford's) is *slower* |
| Run  | [loop_fusion.cpp](loop_fusion.cpp) | mean/variance over 50M ints — two-pass, fused, and Welford's, side by side |

Fusing removes a full cold-cache reread of a 200 MB array: 69 ms → 39 ms
(**1.77x**), close to the 2x memory-bandwidth ceiling. Welford's fuses the
passes too but adds a per-iteration division and a loop-carried dependency —
0.40x, *slower* than doing two passes.

---

## 3 · Loop interchange — align the inner index with memory layout

Swap nested loop indices so the inner loop walks memory contiguously instead
of striding across rows.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [loop_interchange.md](loop_interchange.md) | row-major stride math, and why a 64 KB working set that exactly fills L1 is not a coincidence |
| Run  | [loop_interchange.cpp](loop_interchange.cpp) | matrix–vector multiply, column access vs. row access over a 128 MB matrix |

Reordering with no algorithmic change: stride-N column access (one miss per
element) vs. stride-1 row access (L1-resident) — 47 ms → 12 ms (**3.92x**).

---

## 4 · Loop tiling — block the iteration space to fit a cache level

Restructure a loop into rectangular blocks so each block's working set is
reused from cache instead of reloaded from DRAM.

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [loop_tiling.md](loop_tiling.md) | two failed candidates (matmul, box filter) before landing on matrix transpose, and a cache **set-conflict** that beats the textbook working-set formula |
| Run  | [loop_tiling.cpp](loop_tiling.cpp) | 4096×4096 transpose, tile size swept from 8 to 256+ |

Naive transpose writes stride-N (unpredictable for the prefetcher); tiling
gets 74 ms → 25 ms (**2.96x**) — but at tile=8, not the tile=64 the
`2·B²·8 ≤ L1` formula predicts, because N is a power of 2 and every output row
aliases the same 128 L1 sets. The real constraint is `B ≤ associativity`, not
raw byte capacity.

---

## 5 · Loop unrolling — trade branch/loop overhead for code size

Process multiple elements per iteration to amortize the loop-control
overhead, and handle whatever doesn't divide evenly (the **tail problem**).

| Step | Open | What it teaches |
|------|------|------------------|
| Read | [loop_unrolling.md](loop_unrolling.md) | 4x unroll + cleanup loop vs. Duff's Device, and why the winner flips across `-O0`/`-O1`/`-O2` |
| Run  | [loop_unrolling.cpp](loop_unrolling.cpp) | sum-reduction over 100M ints, three strategies × three optimization levels |
| Port | [loop_unrolling.rs](loop_unrolling.rs) | the same three strategies in Rust — and why Duff's Device doesn't translate mechanically (no fallthrough in `match`) |

Unrolling only wins in the narrow window where register allocation is on but
auto-vectorization isn't: `-O0` 0.69x (spills dominate) → `-O1` **1.35x**
(register reuse) → `-O2` 0.83x (the compiler's own SIMD beats hand-unrolled
scalar code).

---

## Files at a glance

**Sources** — `loop_fission.cpp`, `loop_fusion.cpp`, `loop_interchange.cpp`, `loop_tiling.cpp`, `loop_unrolling.cpp`, `loop_unrolling.rs`
**Write-ups** — `loop_fission.md`, `loop_fusion.md`, `loop_interchange.md`, `loop_tiling.md`, `loop_unrolling.md`
**Reference** — [loop_optimizations_overview.md](loop_optimizations_overview.md) — non-benchmarked software examples of fusion and fission across domains (image processing, DB queries, compilers, video/audio, ML pipelines, network packet processing)

Every write-up here follows the same arc: state the transformation, show why
the *obvious* example is a poor fit (fusion tempts you toward Welford's,
tiling tempts you toward matmul), then land on the example that isolates the
real effect and measure it.
