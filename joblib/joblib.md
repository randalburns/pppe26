# JobLib: A First Parallel Program

> This example is derived from Chapter 1 of Mattson, *Parallel Computing for Data Science*. The example in that chapter has been reimplemented in Python and the content and tone do not follow Mattson exactly.

## Mutual Outlinks — A First Parallel Program

Big data analytics and graph processing grew out of the desire to understand the structure of Internet content and user behavior. The most famous example is the [PageRank algorithm](https://en.wikipedia.org/wiki/PageRank), which changed the nature of Internet search and was crucial in the creation and early success of Google.

For now, we look at a simpler but typical and important problem: identifying Web pages that link to the same content. Multiple Web pages that link to the same content may represent:
- **content clusters** — similar pages
- **search engine gamification** — unimportant pages trying to increase the ranking of target pages

The input is a directed graph. Each Web page is a node and edges represent outlinks.

## The Algorithm

The natural algorithm uses a binary (0/1) matrix representation of the graph where rows and columns are node identifiers and nonzero entries indicate an outbound edge. The algorithm uses nested loops to iterate over all pairs of rows, computing a vector dot product that counts mutual outlinks.

The output is an upper-right triangular matrix. The lower-left half would be symmetric and redundant — computing only the upper triangle avoids unnecessary work.

### Aside: The Power of Python

Python has emerged as the language of choice for data science, Web development, and machine learning. Its simplicity and concision demonstrate why. This example leverages three important data science packages:

- **numpy** — dense vector and matrix operations
- **networkx** — graph and network analysis
- **matplotlib** — visualization

These packages encapsulate complex functions so the programmer does not have to write them. In this example: generate an Erdős–Rényi random graph, display a planar representation, convert to a binary matrix, and compute inner products of row vectors.

Ironically, Python is **the worst** language for parallelism. There is a single choke point that prevents two operations from running in parallel: the **Global Interpreter Lock (GIL)**. To realize parallelism in Python, one must work around this lock with creative methods.

## Parallelization with joblib

To parallelize the outer loop, each row's computation is extracted into a function (`inner_loop`) that can be dispatched independently. `joblib` manages the worker pool:

```python
from joblib import Parallel, delayed

partials = Parallel(n_jobs=8)(delayed(inner_loop)(i) for i in range(gmat.shape[0]))
outmat = np.array(partials)
```

What `joblib` is doing:
- **`delayed`**: wraps a function call, deferring its execution
- **generator**: enumerates all deferred calls
- **`Parallel`**: creates `n_jobs` independent workers that split the list of work and run the deferred functions in parallel

The result is faster, but not proportionally faster. The next topic — **Amdahl's Law** — explains some of the reasons. Python's overhead accounts for the rest.

## Poor Parallelism in Python and the GIL

The **Global Interpreter Lock (GIL)** makes it so that a Python runtime instance accepts only a single instruction at a time. Running multiple threads provides *concurrency* but not *parallelism*:

- **concurrent**: multiple asynchronous actions are defined (a programming abstraction)
- **parallel**: multiple resources computing simultaneously (a hardware reality)

Parallel programs must be concurrent. Concurrent programs are not necessarily parallel.

### Threads: Concurrent but Not Parallel

Because of the GIL, Python threads interleave execution through the single interpreter. Running the same workload serially and with 4 threads takes roughly the same time — the GIL prevents true simultaneity.

### Processes: Parallel but Expensive

Using `multiprocessing.Process` creates separate Python interpreter instances, each with its own GIL. This realizes true parallelism, but spawning multiple Python interpreters carries significant overhead.

### GIL-Releasing Packages

Some packages (notably NumPy for certain operations) release the GIL during execution, allowing other threads to use the interpreter while the function runs. Each thread:

1. acquires the GIL
2. executes a command in the Python interpreter
3. releases the GIL at the start of a long native operation
4. continues execution in native code

This works when every function called inside the thread releases the GIL. In practice, this requires careful coordination by package maintainers and is fragile to achieve across a full call chain.

## Key Takeaways

| Approach | Parallelism | Cost |
|---|---|---|
| Serial Python | None | Baseline |
| Python threads | Concurrent only (GIL) | Low overhead |
| `multiprocessing` | True parallel | High spawn cost |
| `joblib` (processes) | True parallel | Moderate overhead |

For CPU-bound work in Python, `joblib` with process-based workers is the practical entry point. Understanding the GIL and its workarounds is essential for writing performant Python parallel programs.
