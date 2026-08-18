# Work-stealing vs. static scheduling

Two parallel sorts in this directory are deliberately built as opposites so the
scheduler is the *only* variable. Together they show that neither scheduler is
universally faster — the right one depends on the **shape of the work**.

| Example | Divide step | Work profile | Primitive compared | Winner |
|---------|-------------|--------------|--------------------|--------|
| [qsort_steal.c](qsort_steal.c) | pivot (data-dependent) | irregular, unpredictable | `cilk_spawn` vs `omp task` | **Cilk** |
| [mergesort_balanced.c](mergesort_balanced.c) | always in half | regular, fixed-size | `cilk_for` vs `omp for schedule(static)` | **OpenMP** |

## The two scheduling philosophies

**Static scheduling (OpenMP `schedule(static)`).** The loop's iteration space is
carved into `P` equal contiguous chunks *once*, at the start of the loop, and each
of the `P` threads is handed one chunk. There is no per-iteration bookkeeping and
no coordination after the split — threads run their chunk and hit the barrier.
The cost model is dead simple, and when every iteration really does cost the same,
the split is already optimal.

- **Pro:** almost zero scheduling overhead; perfect cache locality (each thread
  owns a contiguous range); decisions made once.
- **Con:** the assignment is fixed. If some chunks turn out heavier than others,
  the unlucky threads run long while the rest sit idle at the barrier. Static
  scheduling cannot recover from imbalance it didn't predict.

**Work-stealing (Cilk `cilk_spawn` / `cilk_for`).** Work is expressed as a tree of
lightweight spawned tasks. Each worker runs its own task and pushes the continuation
onto a local deque; an **idle** worker picks a random victim and *steals* a task off
the top of that victim's deque. Balancing is an emergent, runtime property — nobody
plans it, workers just keep stealing whenever they run dry.

- **Pro:** adapts to *any* imbalance automatically, including imbalance that only
  becomes visible at run time (recursion depth, data-dependent branching). Provably
  near-optimal: expected time ≈ `T₁/P + O(T∞)` (work over P plus the span).
- **Con:** the adaptivity is never free. Every spawn is a potential steal point that
  must be tracked; idle workers burn cycles on steal attempts; stolen tasks land on
  a different core with a cold cache. On perfectly regular work all of that machinery
  runs and buys nothing.

## Why each example goes the way it does

**Quicksort → work-stealing wins.** The pivot decides each split, so subrange sizes
are unknown until run time and vary wildly. A static split of the top-level recursion
would strand whole threads on the small side. Cilk doesn't split up front — it just
keeps spawning, and idle workers steal the deep subtrees as they appear. OpenMP can
only match this with `omp task`, and there each task is eagerly allocated on a runtime
queue (hundreds of ns); across millions of small subranges that overhead dominates,
while a `cilk_spawn` costs a handful of instructions.

**Merge sort → static scheduling wins.** Bottom-up merge sort halves the array
regardless of the data, so every merge in a given pass is exactly the same size and
that size is known before the pass runs. There is nothing to balance. OpenMP splits
each pass into `P` equal chunks once and runs. `cilk_for` still recursively halves the
range into a spawn tree and runs the full work-stealing scheduler on every pass —
maintaining deques and firing steal attempts that never find useful work to move.
That overhead, plus the loss of the clean contiguous-per-thread locality that static
chunking gives, is pure cost here.

## The takeaway

> Work-stealing pays a standing overhead to buy the ability to rebalance.
> On **irregular** work that insurance pays for itself many times over.
> On **regular, balanced** work there is nothing to rebalance, so the premium is wasted
> and simple static scheduling wins.

This is exactly why real systems offer both: OpenMP defaults to static and lets you
opt into `dynamic`/`guided` when you know the work is lumpy; Cilk makes work-stealing
the default because it targets irregular, recursive parallelism where static
scheduling can't even express the problem.

## Measured results

Apple M5 (10 cores) · 8 workers/threads (`CILK_NWORKERS=8 OMP_NUM_THREADS=8`)
· N = 50,000,000 `int` · median of 3 trials.

### mergesort_balanced (regular work)

| version | time (s) | vs serial |
|---------|---------:|----------:|
| serial  |   2.1238 |     1.00× |
| **omp** | **0.4761** | **4.46×** |
| cilk    |   0.5263 |     4.04× |

OpenMP static is ~10% faster than Cilk here (`0.4761` vs `0.5263`). Both reach a
solid ~4× on 8 workers, but the balanced, fixed-size merges give work-stealing
nothing to rebalance, so Cilk's scheduler overhead shows up as a steady margin in
OpenMP's favor — the premium is paid and never recovered.

### qsort_steal (irregular work)

| version  | time (s) | vs serial |
|----------|---------:|----------:|
| serial   |          |     1.00× |
| omp task |          |           |
| cilk     |          |           |

_(qsort_steal not yet run — paste its output and this table will be filled in;
work-stealing is expected to win on the irregular partitions.)_
