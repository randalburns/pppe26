# Small-Array Sorting: Algorithms and SIMD at n=16

## Overview

Sorting small, fixed-size arrays is a surprisingly deep problem.  At large n
(thousands to millions of elements), O(n log n) algorithms dominate and the
differences between them are modest.  At n=16 the landscape inverts: recursion
overhead, branch mispredictions, and loop setup costs often exceed the actual
comparison work, and the choice of algorithm can change the result by 5×.

This document covers six implementations benchmarked at n=16 on an Apple M4
(ARM NEON, 4.0 GHz).  All times are the best of 7 trials on the same random
`int32` array.

## Results

| Algorithm | Time | ns/elem | vs quicksort |
|---|---|---|---|
| std::sort (introsort) | 25 ns | 1.6 | 1.72× faster |
| quicksort (median-of-3) | 43 ns | 2.7 | 1.00× (baseline) |
| insertion sort | 37 ns | 2.3 | 1.16× faster |
| bitonic scalar (80 CAS) | 56 ns | 3.5 | 1.30× **slower** |
| bitonic NEON | 13 ns | 0.8 | 3.31× faster |
| bitonic Highway (portable) | 17 ns | 1.1 | 2.53× faster |

## Algorithm Analysis

### std::sort — fastest scalar option (25 ns)

`std::sort` in libc++/libstdc++ is an introsort: quicksort with a heapsort
fallback and, crucially, an insertion-sort cutoff for small partitions.  At
n=16 the cutoff fires immediately — the implementation never recurses at all.
It effectively runs a hand-tuned insertion sort with compiler-optimised
register allocation, which is why it beats the hand-written quicksort by 1.72×.

### Quicksort — median-of-3, no cutoff (43 ns, baseline)

Classic recursive quicksort with median-of-three pivot selection but no
small-partition optimisation.  At n=16 the recursion tree is 4 levels deep,
each level paying a function-call frame, stack manipulation, and branch
prediction for the partition loop.  The algorithm does correct O(n log n) work
but the constant factor is large relative to n=16's tiny data size.  This
implementation intentionally omits the small-partition cutoff to isolate
recursion overhead.

### Insertion sort (37 ns, 1.16× faster than quicksort)

Insertion sort beats pure quicksort at n=16 because it is iterative, has
near-zero setup cost, and accesses memory sequentially (good prefetch
behaviour).  At n=16, O(n²) = 256 operations, but each operation is a simple
shift — no recursive calls, no pivot selection.  The ns/n² constant (~0.14)
remains stable from n=4 up to ~n=32, after which cache-friendly O(n log n)
sorts pull ahead.

### Bitonic scalar — 80 CAS, branchless (56 ns, 1.30× slower)

The bitonic sorting network executes a predetermined sequence of 80
compare-and-swap (CAS) operations with no branches, no recursion, and no
data-dependent control flow.  Despite being branchless, it is the *slowest*
implementation at n=16.  The reason is instruction count: 80 CAS operations
each expand to two instructions (min + max), giving 160 scalar instructions
versus ~50 for insertion sort.  The branch-free property matters only when
mispredictions dominate; at n=16 they do not.

### Bitonic NEON — fastest overall (13 ns, 3.31× faster than quicksort)

The same 10-step bitonic network ported to ARM NEON processes all 16 elements
packed into four `int32x4_t` registers.  Each comparator step operates on 4
elements simultaneously with `vminq`/`vmaxq`/`vrev64q`/`vextq` instructions,
reducing 80 scalar CAS operations to roughly 40 NEON instructions.  The result
is a 4.3× reduction in instruction count and a 3.31× wall-clock speedup over
quicksort.  This implementation is the ARM-specific version of Algorithm 1 from
[arXiv:1704.08579](https://arxiv.org/abs/1704.08579).

### Bitonic Highway — portable SIMD (17 ns, 2.53× faster)

Google Highway rewrites the same network using portable SIMD intrinsics
(`Reverse2`, `CombineShiftRightBytes`, `OddEven`, `LowerHalf`/`UpperHalf`/
`Combine`) that compile to NEON on ARM and to SSE4/AVX2/AVX-512 on x86 from
a single source file.  On this machine it selects NEON and achieves 17 ns —
about 30% slower than the raw NEON version.  The gap comes from the
`Combine`/`LowerHalf`/`UpperHalf` triple used to implement the "lower half
from one result, upper half from another" blends that the raw NEON `vbslq_s32`
does in a single instruction.  The portability is the payoff.

## Key Takeaways

**Recursion overhead dominates at n=16.**  The hand-written quicksort is the
second-slowest algorithm despite having O(n log n) complexity.  Function-call
overhead, stack frames, and branch prediction for tiny partitions cost more
than the sorting work itself.

**Branch-free ≠ fast for scalar code.**  Bitonic scalar is branchless but
executes 80 CAS pairs — the highest instruction count of any implementation.
Branchlessness only wins when mispredictions are the bottleneck, which is not
the case at n=16 with a 4-level recursion tree.

**SIMD is the right tool for fixed small-n sorts.**  When n is a power of two
and small enough to fit in a handful of registers, a sorting network is the
ideal structure: it maps directly onto SIMD min/max/permute pipelines and
eliminates all control flow.  The NEON version runs 4 comparators per
instruction and saturates the execution units.

**std::sort's cutoff is the right engineering trade-off.**  Library introsort
wins the scalar category not through a clever algorithm but through an
engineering choice: fall back to insertion sort below a threshold (typically
16–32 elements).  This is the correct decision for general-purpose code and
explains why `std::sort` outperforms hand-rolled quicksort at small n.

**Portable SIMD incurs a measurable but acceptable cost.**  Highway is 30%
slower than raw NEON at n=16 (17 ns vs 13 ns), but it is correct on every
Highway-supported target.  For an embedded sort kernel used as a building
block in a larger hybrid algorithm (e.g., as the base case of a vectorised
merge sort), this trade-off is usually worthwhile.

## Files

| File | Description |
|---|---|
| `std_sort.cpp` | `std::sort` benchmark, n=16 to 2²⁰ |
| `quicksort.cpp` | Recursive median-of-3 quicksort, n=16 to 2²⁰ |
| `insertion_sort.cpp` | Insertion sort with ns/n² column, n=16 to 2¹⁶ |
| `bitonic_sort.cpp` | Scalar bitonic network, n=16 only |
| `bitonic_sort_simd.cpp` | ARM NEON bitonic network, n=16 only |
| `bitonic_sort_highway.cpp` | Google Highway bitonic network, n=16 only |
| `sort_compare_n16.cpp` | Combined benchmark producing this table |
