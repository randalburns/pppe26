# Loop Unrolling Example

Demonstrates manual unrolling of a data-driven loop over an array of unknown
length, with a focus on the **tail problem** — handling leftover elements when
`n` is not a multiple of the unroll factor.

## Source

[loop_unrolling.cpp](loop_unrolling.cpp)

## Three strategies

### 1. Scalar baseline
One add per iteration. Simple, predictable, easy for the CPU pipeline.

### 2. 4x unroll with cleanup loop
Process `n - (n % 4)` elements in the fast body (4 adds per iteration), then a
short scalar loop finishes the 0–3 remainder elements.

```cpp
int n4 = n - (n % 4);
for (; i < n4; i += 4) {
    s += data[i];
    s += data[i + 1];
    s += data[i + 2];
    s += data[i + 3];
}
for (; i < n; i++)   // cleanup tail
    s += data[i];
```

### 3. Duff's Device
A `switch` jumps into the *middle* of a `do/while` unrolled body so the first
iteration handles only the tail elements — no separate cleanup loop needed.
Named after Tom Duff (Bell Labs, 1983).

```cpp
int count = (n + 3) / 4;
switch (n % 4) {
    case 0: do { s += data[i++];  // falls through
    case 3:      s += data[i++];  // falls through
    case 2:      s += data[i++];  // falls through
    case 1:      s += data[i++];
            } while (--count > 0);
}
```

## Build

```bash
g++ -O0 -o unroll_O0 loop_unrolling.cpp && ./unroll_O0
g++ -O1 -o unroll_O1 loop_unrolling.cpp && ./unroll_O1
g++ -O2 -o unroll_O2 loop_unrolling.cpp && ./unroll_O2
```

## Results (Apple M-series, 100M elements)

| Flag | Scalar | 4x unroll + cleanup | Duff's Device |
|------|-------:|--------------------:|--------------:|
| `-O0` | 58 ms | 84 ms (0.69x) | 161 ms (0.36x) |
| `-O1` | 23 ms | 17 ms (**1.35x**) | 17 ms (**1.35x**) |
| `-O2` | 5 ms | 6 ms (0.83x) | 6 ms (0.83x) |

## Analysis

### Why `-O0` makes unrolling *slower*

Without optimisation the compiler spills everything through the stack frame
between statements — there is no register-keeping across the four
`s += data[i+k]` lines.  The unrolled body gets 4x the memory traffic with
none of the register-reuse benefit.  The simple scalar loop's predictable
one-load/one-add/one-branch structure pipelines better under these conditions.

Duff's Device is worst at `-O0` because the computed `switch` jump adds
dispatch overhead on every outer entry, on top of the memory-spill problem.

### Why `-O1` shows the real benefit

At `-O1` the compiler assigns registers, so the accumulator `s` stays in a
register across all four adds rather than being reloaded from the stack each
time.  Loop-control instructions (increment, compare, branch) drop by 4x, and
register reuse across the unrolled body is the mechanism that converts that
into a real speedup (~1.35x here).

### Why `-O2` reverses the result

`-O2` enables auto-vectorisation.  The compiler rewrites the scalar loop using
SIMD instructions (e.g., NEON on ARM, SSE/AVX on x86) that process 4–16
elements per instruction.  The hand-unrolled scalar code cannot match this, and
the extra code complexity slightly hurts instruction-cache density.

### Key takeaway

Manual loop unrolling is most valuable at `-O1`-equivalent optimisation levels
(register allocation on, vectorisation off) and in situations where the
compiler cannot auto-vectorise (e.g., loops with pointer aliasing, non-trivial
dependencies, or target architectures without SIMD).  At modern `-O2`/`-O3`
levels, prefer letting the compiler unroll and vectorise unless profiling proves
otherwise.
