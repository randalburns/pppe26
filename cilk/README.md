# Cilk examples — a guided tour

A short course in [OpenCilk](https://www.opencilk.org): the parallel primitives,
the cost model for reasoning about them, and the work-stealing scheduler that runs
them. Work through the four stages in order — each builds on the last.

Everything builds with one Makefile (needs `~/opencilk/bin/clang`; the scheduler
benchmarks also need libomp):

```sh
make
```

---

## 1 · Fork and join — start here

`cilk_spawn` and `cilk_sync` are the heart of Cilk. Fibonacci is the smallest example
that shows them: two independent recursive calls forked apart, then joined. Its call
tree *is* the graph the scheduler reasons about.

| Step | Open | What it teaches |
|------|------|-----------------|
| Read | [cilk_primitives.md](cilk_primitives.md) | the overview and the whole vocabulary |
| Run  | [fib.c](fib.c) | `cilk_spawn` / `cilk_sync` — fork two calls, then join |
| View | [fib_dag.html](fib_dag.html) | the `fib(4)` spawn–join graph the scheduler sees |

---

## 2 · Work and span — the cost model

With the fib graph in hand, learn to reason about how fast a computation *can* run
before measuring anything. Two numbers read straight off that graph — total work and
the critical-path span — bound every parallel program, and their ratio is the most
parallelism any scheduler could ever extract.

| Step | Open | What it teaches |
|------|------|-----------------|
| Read | [work_span.md](work_span.md) | `T₁`, `T∞`, parallelism, and `T_P ≈ T₁/P + T∞` |
| View | [fib_dag.html](fib_dag.html) | the "Work vs. span" panel counts `T₁` and `T∞` on the DAG |

---

## 3 · The parallel loop and the reducer

The other two primitives, now with the cost model to judge them. `cilk_for` matters
precisely *because* it builds a balanced spawn tree — Θ(log n) span, huge parallelism —
rather than a sequential chain; a reducer lets a parallel loop accumulate safely.

| Step | Open | What it teaches |
|------|------|-----------------|
| Run  | [vector_add.c](vector_add.c) | `cilk_for` — a parallel loop with Θ(log n) span |
| Run  | [sum_reducer.c](sum_reducer.c) | `cilk_reducer` — a lock-free reduction |

---

## 4 · Work stealing — the scheduler

How Cilk turns available parallelism into real speedup, and when its work-stealing
scheduler wins or loses against OpenMP's static and dynamic scheduling.

| Step | Open | What it teaches |
|------|------|-----------------|
| View | [work_stealing.html](work_stealing.html) | the deque mechanism: owners pop the bottom, thieves steal the top |
| Read | [work_stealing_vs_static.md](work_stealing_vs_static.md) | stealing vs. static, with measured results |
| Run  | [qsort_steal.c](qsort_steal.c) | *irregular* recursive work — stealing wins (`cilk_spawn` vs `omp task`) |
| Run  | [mergesort_balanced.c](mergesort_balanced.c) | *regular* balanced work — static wins (`cilk_for` vs `omp for`) |
| Read | [dynamic_vs_work_stealing.md](dynamic_vs_work_stealing.md) | where OpenMP `schedule(dynamic)` competes with stealing |
| Run  | [sparse_col_sum.c](sparse_col_sum.c) | *irregular* flat loop — static vs `dynamic` vs `cilk` side by side |
| Read | [sparse_col_sum.md](sparse_col_sum.md) | the three-way results and per-version analysis |

---

## Files at a glance

**Primitives** — `fib.c`, `vector_add.c`, `sum_reducer.c`
**Scheduler benchmarks** — `qsort_steal.c`, `mergesort_balanced.c`, `sparse_col_sum.c`
**Write-ups** — `cilk_primitives.md`, `work_span.md`, `work_stealing_vs_static.md`, `dynamic_vs_work_stealing.md`, `sparse_col_sum.md`
**Figures** — `fib_dag.html`, `work_stealing.html` (standalone pages — open in any browser)

See also [../openmp/cilk_prefix_sum.c](../openmp/cilk_prefix_sum.c) for a larger
`cilk_for` example.
