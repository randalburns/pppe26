# Sorting

A tour through why `std::sort` is built the way it is, and how far you can
push each of its pieces with SIMD.

## 1. How `std::sort` works: three regimes, and why small-sort matters

`std::sort` is introsort: quicksort with a depth-limited heapsort fallback
and an insertion-sort cutoff for small partitions. It isn't one algorithm —
it's three, dispatched by partition size:

- **The Leaf** — partitions below a threshold (~16 elements) fall to
  insertion sort.
- **The Widening Middle** — mid-size partitions run standard quicksort.
- **The Safety Net** — recursion depth past `2·log₂n` switches to heapsort,
  bounding worst-case behavior.

[three_regimes.html](three_regimes.html) walks through all three.
[sort_mechanisms.html](sort_mechanisms.html) covers the mechanics of each
underlying algorithm (insertion sort, quicksort, heapsort) individually.

The reason the leaf regime exists at all: [timing_size_breakdown.md](timing_size_breakdown.md)
instruments a small-partition-cutoff-free quicksort and shows that 20–66% of
total runtime is spent inside partitions of ≤16 elements, at every array size
from n=16 to n=1,048,576. Raw quicksort recurses all the way down to
partitions of size 1 — roughly 2n calls — while each call pays a fixed cost
(function frame, median-of-3, partition loop) that dwarfs the actual
comparison work at that size. `std::sort`'s insertion-sort cutoff amortizes
that overhead into a single linear pass and is 1.8×–3.4× faster than
cutoff-free quicksort across the whole size range. That gap is the
motivation for everything in section 2.

**Benchmarks:** [std_sort.cpp](std_sort.cpp), [quicksort.cpp](quicksort.cpp),
[insertion_sort.cpp](insertion_sort.cpp)

## 2. Bitonic sorting and optimizing the small-sort base case

If insertion sort is already a win over recursing, can a fixed sorting
network beat insertion sort at the leaf? [small_sorting.md](small_sorting.md)
benchmarks six n=16 implementations on Apple M5/NEON:

| Algorithm | Time | vs std::sort |
|---|---|---|
| bitonic NEON | 19 ns | 2.95× faster |
| bitonic Highway (portable) | 24 ns | 2.33× faster |
| bitonic scalar (80 CAS) | 23 ns | 2.43× faster |
| insertion sort | 49 ns | 1.14× faster |
| std::sort (introsort) | 56 ns | 1.00× (reference) |
| quicksort (median-of-3) | 75 ns | 1.34× slower |

The scalar bitonic network (80 branchless compare-and-swaps) is actually the
*slowest* option — branch-free doesn't help when instruction count, not
misprediction, is the bottleneck. Packing the same 10-step network into NEON
registers (4 elements per instruction) is what flips it into the fastest
implementation. [bitonic_diagram.html](bitonic_diagram.html) builds the
network step by step, from a single comparator to a full traced run.

**Implementations:** [bitonic_sort.cpp](bitonic_sort.cpp) (scalar),
[bitonic_sort_simd.cpp](bitonic_sort_simd.cpp) (raw ARM NEON),
[bitonic_sort_highway.cpp](bitonic_sort_highway.cpp) (portable Google
Highway) — compared in [sort_compare_n16.cpp](sort_compare_n16.cpp).

## 3. vqs_highway: vectorizing the large-n path too

Section 2 speeds up the leaf. [vqs_highway.md](vqs_highway.md) applies the
same "replace scalar control flow with SIMD" idea to the *partition* step
itself — the part of quicksort that dominates at large n. Following Bramas
([arXiv:1704.08579](https://arxiv.org/abs/1704.08579)), it replaces the
classic Hoare two-pointer scan with a **CompressStore scatter**: each
N-element SIMD block is classified against the pivot and packed into two
scratch buffers with `vpcompressd` (AVX-512) or a NEON LUT-based equivalent.
The payoff is memory-access pattern, not instruction count — Hoare's
in-place swaps produce random dirty writes that evict cache lines past
L2/L3, while CompressStore's writes are fully sequential and prefetcher
friendly.

[vqs_diagram.html](vqs_diagram.html) walks through one NEON partition block,
a full pass, and the write-pattern comparison against Hoare.

**Implementation:** [vqs_highway.cpp](vqs_highway.cpp), built on
[bitonic_sort_simd.cpp](bitonic_sort_simd.cpp)'s network as its n=16 base
case.

## 4. Putting it all together

`vqs_highway` combines both optimizations into one hybrid sort with the same
three-regime shape as `std::sort`, but SIMD at both ends:

| Component | Regime | Implementation |
|---|---|---|
| `bitonic16` | leaf (n = 16) | NEON 10-step sorting network (section 2) |
| `insertion_sort` | leaf (n < 16) | scalar fallback |
| `simd_partition` | widening middle | CompressStore scatter (section 3) |
| `simd_qs_core` | widening middle | median-of-3 pivot + recurse |

Results on Apple M5, random `int32`, best of 7 runs:

| n | vqs_highway | std::sort | speedup |
|---|---|---|---|
| 16 | 10 ns | 24 ns | 2.40× |
| 1 024 | 5.0 µs | 5.0 µs | 1.00× |
| 8 192 | 44.9 µs | 49.8 µs | 1.07× |
| 65 536 | 687 µs | 745 µs | 1.09× |
| 131 072 | 1.4 ms | 1.7 ms | 1.24× |
| 1 048 576 | 12.8 ms | 15.5 ms | 1.21× |

The crossover where `vqs_highway` pulls ahead of `std::sort` is around
n = 8192 — the point where the working set overflows L2/L3 and `std::sort`'s
Hoare partition starts paying for random dirty writes that
`simd_partition`'s sequential access pattern avoids. Below that, the two are
close, since the leaf-level bitonic win (2.4× at n=16) is a fixed amount of
time that shrinks in proportion as n grows and the partition cost dominates.

Comparable-scale sorts for context: [newintrosort.cpp](newintrosort.cpp) /
[newintrosort_heapsortfallback.cpp](newintrosort_heapsortfallback.cpp)
(introsort variants with an explicit heapsort safety net) and
[sort_compare_large.cpp](sort_compare_large.cpp) (large-n comparison
harness).
