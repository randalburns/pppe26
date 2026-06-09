# Sorting on Apple M4: Best Method vs std::sort

**Platform:** Apple M4, ARM NEON (128-bit / 4-lane int32), clang++ -O2  
**Data:** random int32, best of 7 runs  
**Reference:** `std::sort` (libc++ pdqsort-derived introsort)

---

## Headline: bitonic_simd at n = 16

At n = 16 in isolation, the NEON bitonic sorting network (`bitonic_sort_simd.cpp`)
is **3.18× faster** than `std::sort`.

| Algorithm | Time | ns/elem | vs std::sort |
|---|---|---|---|
| `bitonic_simd` (NEON) | **11 ns** | 0.7 | **3.18× faster** |
| `bitonic_highway` (portable) | 14 ns | 0.9 | 2.50× faster |
| `bitonic_scalar` | 24 ns | 1.5 | 1.46× faster |
| insertion sort | 29 ns | 1.8 | 1.21× faster |
| `std::sort` | 35 ns | 2.2 | 1.00× (reference) |
| quicksort (median-of-3) | 38 ns | 2.4 | 0.92× (slower) |

`std::sort` fires its insertion-sort base case immediately at n = 16 but still
pays for function-call and dispatch overhead. The NEON bitonic network has no
branches, no recursion, and fits entirely in four `int32x4_t` registers —
10 parallel compare-and-swap steps touching no memory between load and store.

---

## Introsort (quicksort + bitonic_simd base case) vs std::sort

`newintrosort_heapsortfallback.cpp` uses bitonic_simd as the leaf sorter for
partitions of ≤ 16 elements, with heapsort fallback to guarantee O(n log n).
This is the best general-purpose implementation that incorporates the SIMD
base case.

| n | introsort | std::sort | speedup | region |
|---|---|---|---|---|
| 1 024 | 8 µs | 8 µs | **1.00×** | break-even |
| 2 048 | 17 µs | 13 µs | 0.76× | std wins |
| 4 096 | 33 µs | 25 µs | 0.76× | std wins |
| 8 192 | 128 µs | 46 µs | 0.36× | std wins |
| 16 384 | 350 µs | 115 µs | 0.33× | std wins |
| 32 768 | 867 µs | 329 µs | 0.38× | std wins |
| 65 536 | 1 ms | 716 µs | 0.72× | std wins |
| 131 072 | 4 ms | 1 ms | 0.37× | std wins |
| 262 144 | 9 ms | 3 ms | 0.37× | std wins |
| 524 288 | 19 ms | 6 ms | 0.35× | std wins |
| 1 048 576 | 41 ms | 14 ms | 0.35× | std wins |

Speedup = std::sort time ÷ introsort time; values < 1.0 mean std::sort is faster.

The SIMD base case yields a measurable advantage at the leaf level (3.18× at
n = 16 in isolation), but Apple's libc++ `std::sort` dominates at all larger
sizes. Its pdqsort-derived implementation uses pattern detection, block
partitioning, and a heavily tuned insertion-sort fallback that collectively
outperform a hand-rolled median-of-three introsort regardless of base-case
quality.

---

## Where bitonic_simd Wins

The bitonic NEON sorter is the right choice when n = 16 is a fixed, repeated
inner-loop operation — for example, as the base case of a merge sort called
millions of times per second, or sorting fixed-size register tiles in a SIMD
pipeline. In that context the 3.18× advantage over std::sort accumulates
directly into end-to-end throughput.

For general sorting of variable-length arrays, `std::sort` is the correct
choice at all sizes benchmarked here.

---

## Summary

| Scenario | Best method | vs std::sort |
|---|---|---|
| n = 16, fixed, called in a tight loop | `bitonic_simd` (NEON) | **3.18× faster** |
| n = 16, portable SIMD needed | `bitonic_highway` | 2.50× faster |
| n > 16, general use | `std::sort` | reference |
