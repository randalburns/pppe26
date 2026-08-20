# Work and span

A quick model for reasoning about how fast a parallel program *can* run, before
you measure anything. Think of the computation as a **DAG** of tasks — exactly the
[fib spawn–join graph](fib_dag.html) — where an edge means "this must finish before
that can start."

## The two numbers

- **Work — T₁** — the total time on **1 processor**. Add up every task in the DAG.
  This is the *amount* of computation.
- **Span — T∞** — the time on **infinitely many processors**. It is the **longest
  dependency chain** through the DAG (the critical path). More cores can never beat
  it, because those tasks must run one after another.

The subscript is literally the processor count: `T₁` = 1 core, `T∞` = ∞ cores.

## What they tell you

**Parallelism = T₁ / T∞.** The maximum speedup possible, ever. If work is 100× the
span, no more than ~100 cores can help — beyond that they sit idle.

**Running time on P cores** is bounded on both sides:

> **T_P ≥ max(T₁ / P, T∞)**

You can never beat work-divided-by-cores (`T₁/P`), and you can never beat the span
(`T∞`). A good scheduler — like Cilk's work-stealing — gets close to the sum:

> **T_P ≈ T₁ / P + T∞**

## The intuition

- **Work** = how *much* there is to do.
- **Span** = how *deep* the unavoidable sequential chain is.
- You want **lots of work relative to span** — that is "plenty of parallelism to
  hand the scheduler."

## fib(4), worked out

From the [diagram](fib_dag.html):

| Quantity | Value | Grows as | Meaning |
|----------|------:|----------|---------|
| Work `T₁` | 9 nodes | Θ(φⁿ) | every call in the tree — the serial running time |
| Span `T∞` | 7 nodes | Θ(n) | longest fork→sync path, down one spine and back |
| Parallelism `T₁/T∞` | ≈ 1.3× | Θ(φⁿ ⁄ n) | tiny at n = 4, explodes for large n |

Parallelism is negligible at `n = 4`, but because work grows **exponentially** while
span grows only **linearly**, for large `n` it explodes as Θ(φⁿ ⁄ n). That gap is the
whole point of divide-and-conquer parallelism.

## The one rule to remember

> **Shrink the span; grow the work-to-span ratio.**

Same work, different span, wildly different parallelism:

- A **balanced tree** of spawns (what `cilk_for` builds) has span Θ(log n) — huge
  parallelism.
- A **sequential chain** of spawns (spawn, then depend on it, then spawn again) has
  span Θ(n) — almost none, even though the work is identical.

The scheduler can only exploit parallelism the DAG actually exposes. Work and span
tell you how much is there before you run a single trial.

## See also

- [fib_dag.html](fib_dag.html) — the DAG these numbers are counted from.
- [work_stealing.html](work_stealing.html) — how the scheduler turns available
  parallelism into `T_P ≈ T₁/P + T∞`.
- [work_stealing_vs_static.md](work_stealing_vs_static.md) — measured speedups, the
  empirical side of this model.
