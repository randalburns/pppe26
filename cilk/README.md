# Cilk parallelization primitives

Minimal, one-primitive-per-file examples for the [OpenCilk](https://www.opencilk.org)
extensions to C. Each file is short and heavily commented so the primitive is the
only thing on display.

| File | Primitive | What it shows |
|------|-----------|---------------|
| [fib.c](fib.c) | `cilk_spawn` / `cilk_sync` | Fork two independent recursive calls, then join. The scheduler *may* run them in parallel. See the [spawn–join graph](fib_dag.html). |
| [vector_add.c](vector_add.c) | `cilk_for` | Parallel loop over independent iterations; O(log N) span via a spawn tree. |
| [sum_reducer.c](sum_reducer.c) | `cilk_reducer` | Lock-free, deterministic reduction (`sum += a[i]`) inside a `cilk_for`. |
| [qsort_steal.c](qsort_steal.c) | `cilk_spawn` vs `omp task` | Parallel quicksort — recursive, *irregular* work where Cilk's work-stealing beats OpenMP tasks. |
| [mergesort_balanced.c](mergesort_balanced.c) | `cilk_for` vs `omp for schedule(static)` | Bottom-up merge sort — *regular, balanced* work where OpenMP's static scheduling beats Cilk. |

The two sorts above are a matched pair; see
[work_stealing_vs_static.md](work_stealing_vs_static.md) for the write-up and results,
and [work_stealing.html](work_stealing.html) for a diagram of how the deque scheduler works.

## The whole vocabulary

There are really only three things to learn:

- **`cilk_spawn f()`** — the caller may keep going in parallel with `f()`.
- **`cilk_sync`** — wait for all spawns made in this function (implicit at function end).
- **`cilk_for`** — sugar for a balanced tree of spawns over a loop's index range.

`cilk_reducer` is the standard way to update a shared accumulator from parallel
strands without a race or a lock.

## Build & run

Requires the OpenCilk compiler. The Makefile assumes it lives at `~/opencilk/bin/clang`.

```sh
make            # build all four
./fib 40
./vector_add
./sum_reducer
CILK_NWORKERS=8 OMP_NUM_THREADS=8 ./qsort_steal   # Cilk vs OpenMP, matched workers
```

`qsort_steal` additionally needs libomp (`brew install libomp`); the Makefile
points at `/opt/homebrew/opt/libomp`.

Control the worker count with the environment variable:

```sh
CILK_NWORKERS=1 ./fib 40      # serial
CILK_NWORKERS=8 ./fib 40      # 8 workers
```

## See also

[../openmp/cilk_prefix_sum.c](../openmp/cilk_prefix_sum.c) — a fuller example using
`cilk_for` for a three-pass parallel prefix sum, including a work-stealing-aware
software barrier.
