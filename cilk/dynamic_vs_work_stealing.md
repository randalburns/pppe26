# Dynamic scheduling vs. work-stealing

[work_stealing_vs_static.md](work_stealing_vs_static.md) framed the contrast as
**static scheduling** (decide the split up front) vs. **work-stealing** (rebalance
at run time). But that skips the obvious middle option: OpenMP's own
`schedule(dynamic)`, which *also* rebalances at run time. So the real spectrum has
three points, not two:

| Approach | When work is assigned | Coordination structure |
|----------|-----------------------|------------------------|
| `omp for schedule(static)` | once, before the loop | none — fixed P-way split |
| `omp for schedule(dynamic,c)` | on demand, during the loop | **one shared queue** (a global counter) |
| Cilk `cilk_for` / `cilk_spawn` | on demand, during the loop | **P private deques** + random steals |

Both dynamic and work-stealing hand work out lazily, so both fix load imbalance that
static can't. The question this file answers: **if dynamic scheduling already
rebalances, what does work-stealing add?** The answer is in *how* the two hand work
out.

## How each one hands out work

**OpenMP `schedule(dynamic, c)` — a central queue.** The loop's iterations are a pile
of chunks of size `c`. There is one shared cursor. Whenever a thread finishes its
chunk it does an atomic fetch-and-add on that cursor to grab the next `c` iterations.
Idle threads never sit still while work remains — but every dispatch touches the *same*
shared variable.

- **Pro:** dead simple, and genuinely adaptive. On a flat loop with lumpy iterations it
  gets close to perfect balance with almost no code change (`static` → `dynamic`).
- **Con:** that single cursor is a serialization point. With many threads and small
  chunks, they collide on it; the atomic traffic and cache-line ping-pong on the cursor
  become the bottleneck. Chunks are also handed out in arbitrary order, so a thread does
  not keep a contiguous, cache-friendly region — locality suffers.
- **Hard limit:** it only schedules a *loop*. It has no way to express parallelism that
  is generated recursively.

**Cilk work-stealing — P private deques.** Each worker owns its own deque and runs it
from the bottom with cheap, uncontended local operations. Coordination happens *only*
when a worker goes idle: then it randomly picks a victim and steals one task from the
top. (See [work_stealing.html](work_stealing.html) for the picture.)

- **Pro:** no shared cursor, so no central contention — the common case (a worker with
  its own work) touches nothing shared at all. Stealing the *oldest* task moves a large
  subtree per steal, so steals stay rare. A worker keeps working its own contiguous
  region, so locality is good. And the same mechanism expresses **recursive** parallelism
  natively.
- **Con:** a more complex runtime, and the standing bookkeeping cost that buys nothing
  when the work is already balanced (the point of the static example).

## Where they land, dimension by dimension

| Dimension | `schedule(dynamic)` | work-stealing |
|-----------|---------------------|---------------|
| Adapts to imbalance | yes | yes |
| Coordination | central queue (shared counter) | distributed deques |
| Contention at high thread count | grows — all threads hit one cursor | low — only on steals |
| Locality | poor (chunks in arbitrary order) | good (worker keeps its own region) |
| Cost when perfectly balanced | an atomic per chunk | near-zero for owner, but scheduler still runs |
| Recursion / nested parallelism | **not supported** (loops only) | native |
| Tuning knob | chunk size `c` (sensitive) | grain size (usually fine at default) |

## What the examples say

**Regular work — [mergesort_balanced.c](mergesort_balanced.c).** Nothing to rebalance,
so dynamic scheduling only adds its per-chunk atomic on top of what static already does
for free. Expect the ordering **static ≤ dynamic ≤ cilk-ish**: `dynamic` pays overhead
for adaptivity it never uses, and it will not beat `static` here (measured: `omp static`
0.476 s vs `cilk` 0.526 s on the M5; a `dynamic` pass lands at or behind `static`).
On balanced work, *every* runtime rebalancer is a tax.

**Flat but irregular work — [sparse_col_sum.c](sparse_col_sum.c).**
This is the case that actually separates dynamic from work-stealing. It counts
non-zeros per column of a CSR matrix whose first rows are dense and the rest sparse —
severe imbalance, but a *flat* loop, so all three schedulers apply. It already
benchmarks `omp static`, `omp dynamic(1)`, and `cilk grainsize(1)` head to head.
Full results and per-version analysis are in [sparse_col_sum.md](sparse_col_sum.md).

The short version: `static` is worst (imbalance). `dynamic(1)` and `cilk` are both far
better because both rebalance — **this is the case where dynamic scheduling and
work-stealing are genuinely comparable.** With chunk size 1 on a fairly coarse per-row
cost, dynamic's central-queue contention is modest and it competes closely; work-stealing's
edge is locality and lower coordination cost, and it widens as you add cores or shrink the
per-item work.

**Recursive irregular work — [qsort_steal.c](qsort_steal.c).** Here `schedule(dynamic)`
is simply **not on the menu** — there is no loop to schedule, the parallelism is the
recursion tree. OpenMP's only recursive tool is `omp task`, whose eager per-task
allocation is exactly what work-stealing's lazy `cilk_spawn` beats. So on recursive
workloads the comparison isn't dynamic-vs-stealing at all; dynamic can't play.

## So — how does dynamic scheduling compare to work-stealing?

> **On a flat, irregular loop, they are close.** Both rebalance at run time, and OpenMP
> `dynamic` is far simpler to reach for. It is often the right, pragmatic choice — which
> is exactly why the course notes credit *both* dynamic scheduling and work-stealing for
> the sparse-column-sum speedup.

Work-stealing pulls ahead in three specific situations:

1. **Fine granularity or high core counts** — dynamic's one shared cursor becomes a
   contention bottleneck; the distributed deques don't.
2. **Locality matters** — a worker keeps its own contiguous region; dynamic scatters
   chunks across threads.
3. **The parallelism is recursive** — dynamic can't express it at all, while work-stealing
   was built for it.

And dynamic wins on one axis that isn't performance: **simplicity**. If the workload is a
flat loop and the imbalance is mild, `schedule(dynamic)` (or `guided`, which starts with
big chunks and shrinks them to cut both imbalance *and* contention) gets you most of the
benefit with a one-word change.

**Rule of thumb:** flat loop, mild-to-moderate imbalance → `schedule(dynamic)` /
`guided`. Recursive, deeply irregular, or contention-sensitive at scale → work-stealing.
