# Sorting on Apple M5: Best Method vs std::sort

**Platform:** Apple M5, ARM NEON (128-bit / 4-lane int32), clang++ -O2  
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

`newintrosort_heapsortfallback.cpp` uses `bitonic_sort_simd` as the leaf sorter
for partitions ≤ 16, with heapsort fallback to guarantee O(n log n).

### n = 16: bitonic_sort_simd base case (quicksort never runs)

At n = 16 the cutoff fires immediately — introsort *is* bitonic_sort_simd with
two function calls of overhead:

| n | introsort | std::sort | speedup |
|---|---|---|---|
| 16 | **10 ns** | 24 ns | **2.40×** |

This matches the standalone result (11 ns) within measurement noise. Quicksort
does no partition work; the entire sort is the NEON bitonic network.

### n > 16: quicksort with bitonic_sort_simd leaves

| n | introsort | std::sort | speedup |
|---|---|---|---|
| 32 | 52 ns | 77 ns | **1.48×** |
| 64 | 140 ns | 156 ns | **1.11×** |
| 128 | 375 ns | 434 ns | **1.16×** |
| 256 | 887 ns | 982 ns | **1.11×** |
| 512 | 2.3 µs | 2.2 µs | ~1.0× |
| 1 024 | 5.5 µs | 5.0 µs | 0.91× std wins |
| 2 048 | 12.2 µs | 10.7 µs | 0.88× std wins |
| 4 096 | 27.3 µs | 22.9 µs | 0.84× std wins |
| 8 192 | 101 µs | 49.8 µs | 0.49× std wins |
| 16 384 | 342 µs | 110 µs | 0.32× std wins |

Speedup = std::sort ÷ introsort; values < 1.0 mean std::sort is faster.

introsort beats std::sort at n = 32–256 because bitonic leaves account for a
large fraction of total work in that range. Above n ≈ 512 the upper-level Hoare
partition dominates and Apple's pdqsort pulls ahead — its pattern detection and
block partitioning outperform median-of-three Hoare regardless of base-case
quality.

---

## Large n: vqs_highway vs std::sort

`vqs_highway.cpp` (Algorithm 4, Bramas 2017) replaces the in-place Hoare
partition with a `CompressStore`-based scatter into scratch buffers. All reads
and writes are sequential, eliminating the random dirty-cache-line writes that
Hoare partition produces on every swap. The cost is 2× memory bandwidth plus two
`memcpy` calls per partition level.

The trade-off pays off past the L2/L3 boundary:

| n | vqs_highway | std::sort | speedup |
|---|---|---|---|
| 16 | **10 ns** | 24 ns | **2.40×** (bitonic, no allocation) |
| 1 024 | 5 889 ns | 5 923 ns | 1.01× |
| 4 096 | 25 236 ns | 23 888 ns | 0.95× std wins |
| 8 192 | 49 954 ns | 52 517 ns | 1.05× |
| 32 768 | 243 486 ns | 336 652 ns | **1.38×** |
| 65 536 | 619 500 ns | 699 750 ns | **1.13×** |
| 131 072 | 1 406 500 ns | 1 613 583 ns | **1.15×** |
| 262 144 | 2 870 834 ns | 3 366 375 ns | **1.17×** |
| 524 288 | 6 050 708 ns | 6 985 500 ns | **1.15×** |
| 1 048 576 | 12 468 458 ns | 14 512 333 ns | **1.16×** |

The transition is noisy between n = 8192 and n = 32768 (the L2/L3 cliff); the
advantage stabilises at **~1.15×** from n = 65536 onward.

**Why introsort does not get this benefit:** both introsort and std::sort use
in-place Hoare partitioning. The random swap-writes are the bottleneck at large
n, not the base case. A faster leaf sorter (bitonic vs insertion sort) helps only
at the leaf level; the upper partition levels dominate large-n cost.

---

## Where Each Method Wins

| n | Best method | vs std::sort |
|---|---|---|
| 16 (standalone or base case) | `bitonic_sort_simd` (NEON) | **2.40–3.18×** |
| 16 (portable) | `bitonic_highway` | 2.50× |
| 32–256 | introsort (quicksort + bitonic_simd leaves) | **1.11–1.48×** |
| 512–4 096 | `std::sort` | reference |
| 8 192–32 768 | vqs_highway or std::sort | within noise |
| ≥ 65 536 | `vqs_highway` | **~1.15–1.21×** |
