# Sorting on Zen 5: What Actually Beats `std::sort`

**Platform:** AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point", 12C/24T)
**Build:** `g++ -O2 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul`, Highway 1.0.7
**Data:** uniform random `int32`, best of 7, pinned to one core with `taskset`
**Source:** [`sort_survey_x86.cpp`](sort_survey_x86.cpp) — every number below comes from this one program

---

## The short answer

There are three size regimes, and a different technique wins each one.

| n | Use | Speedup vs `std::sort` | Why |
|---|---|---|---|
| **exactly 16** | AVX2 bitonic network | **~20×** | No branches at all |
| **32 – ~4 000** | introsort with SIMD leaves | 1.7× → 1.15× | Better leaf sorter |
| **≥ ~8 000** | vectorized quicksort (vqs) | 3.0× → **3.9×** | Sequential writes |

If you only remember one thing: **`vqs` wins everywhere above n = 32 and its lead grows with n.**
The reason is not that it does less work — it does *more* — but that its memory writes
are sequential instead of scattered.

---

## Before the numbers: how they were measured

This matters more than usual here, because the obvious way to benchmark a sort is wrong,
and it was wrong in this repository's earlier benchmarks.

The natural harness looks like this:

```cpp
for (k = 0; k < reps; k++) { copy(src -> buf); sort(buf); }   // WRONG
```

`src` is the same array every iteration. A comparison sort branches on the data, so after
a few repetitions **the branch predictor has memorised the entire comparison sequence**.
The sort then runs with near-zero mispredictions — something that never happens in real use.

The effect is large, and it is not uniform:

| `std::sort`, n = 4096 | measured | ns/(n log₂ n) |
|---|---:|---:|
| re-sorting one fixed array | 28 µs | 0.57 |
| independent random arrays | **260 µs** | **5.29** |

A **9× error**. It shrinks as n grows (the branch history stops fitting the predictor),
which manufactures a fake discontinuity — `std::sort` appearing to jump from 27 µs at
n = 4096 to 512 µs at n = 8192, for twice the data.

This systematically flatters branchy sorts and does nothing for branchless ones, so
**comparing a SIMD sorting network against `std::sort` on that harness is not a fair test** —
it is a test of the branch predictor.

Everything below instead pre-builds K independent random buffers, refills them *outside*
the timed region, and times sorting all K. The harness floor was measured at ~0 ns, so the
reported times are sort work.

> **Note on older numbers.** Figures elsewhere in this repo that show `std::sort` at
> 44–64 ns for n = 16 come from the old harness. The honest figure for sorting an
> *unseen* 16-element array is ~235 ns. The bitonic timings are unchanged, because a
> sorting network has no data-dependent branches to mispredict.

---

## Regime 1 — n = 16: sorting networks

A **sorting network** is a fixed sequence of compare-exchange operations, decided before
the data exists. For 16 elements it is 80 comparators in 10 columns. It always executes
exactly those 80 operations, whatever the input.
[`bitonic_diagram.html`](bitonic_diagram.html) builds the network up step by step, from a
single comparator to a full traced run.

That sounds wasteful — insertion sort averages far fewer comparisons — and it is, in
operation count. It wins anyway:

| Method | Time | vs `std::sort` |
|---|---:|---:|
| **bitonic AVX2 (2×256-bit)** | **9–12 ns** | **~19×** |
| bitonic Highway (portable, 128-bit) | 17–24 ns | ~10× |
| `std::sort` | 170–235 ns | 1.00× |
| insertion sort | 173–242 ns | 0.97× |
| heapsort | 370–515 ns | 0.46× |

Absolute times at this size move with clock ramp, so ranges are given across runs; the
ratios hold to within a few percent (bitonic measured 18.7×, 19.6× and 20.6× on three
builds). The ratio is the result here, not the nanoseconds.

Three things are happening:

1. **No branches.** `std::sort` at n = 16 immediately falls back to insertion sort, whose
   inner loop exits unpredictably. At ~16 mispredictions × ~15 cycles, misprediction alone
   costs more than the whole bitonic network. Note `std::sort` and insertion sort measure
   within 3% of each other — at this size they are the same algorithm.
2. **8 comparators per instruction.** All 16 values live in two 256-bit registers, so one
   `vpminsd`/`vpmaxsd` pair does 8 comparisons.
3. **No memory traffic.** Between the load and the store, nothing touches memory.

**Width beats portability here.** The Highway version is 2× slower — not because Highway
generates bad code, but because the network's structure pins it to `FixedTag<int32_t,4>`,
i.e. 128-bit vectors. At equal width Highway is competitive: compiled with clang it ties
hand-written 128-bit intrinsics exactly (22 ns each). The 12 ns comes from using 256-bit
registers, which this formulation of the network cannot express portably.

---

## Regime 2 — n = 32 to ~4 000: better leaves

`introsort` here is a textbook Hoare quicksort with median-of-3 pivots and a heapsort
fallback, but with the 16-element base case replaced by the AVX2 bitonic network.

| n | introsort | `std::sort` | speedup |
|---|---:|---:|---:|
| 32 | 472 ns | 802 ns | **1.70×** |
| 64 | 1.3 µs | 2.0 µs | 1.48× |
| 128 | 3.3 µs | 4.6 µs | 1.39× |
| 256 | 8.0 µs | 10.6 µs | 1.31× |
| 512 | 19.3 µs | 24.0 µs | 1.25× |
| 1 024 | 44.5 µs | 53.3 µs | 1.20× |
| 4 096 | 222.3 µs | 256.1 µs | 1.15× |
| 1 048 576 | 72.1 ms | 76.6 ms | 1.06× |

The pattern is a **steady decay from 1.70× toward 1.0×**, and the reason is simple
arithmetic: the leaves are where the SIMD network runs, and leaves are a shrinking
fraction of total work as n grows. At n = 32 the sort is essentially two bitonic calls.
At n = 1M the leaves are a rounding error and everything is partitioning — which introsort
does exactly the same way `std::sort` does.

**So swapping in a faster base case has a ceiling.** It cannot fix the partition, and
above a few thousand elements the partition is the whole cost.

---

## Regime 3 — n ≥ 8 000: fixing the partition

`vqs` (Bramas, [arXiv:1704.08579](https://arxiv.org/abs/1704.08579)) keeps quicksort's
recursion and pivot selection and changes only the partition step.

Hoare's partition walks two pointers inward and swaps out-of-place pairs **in place**.
The comparisons are cheap; the problem is that the writes land wherever the pointers
happen to be, scattered across the subarray. Once the subarray is bigger than cache,
every scattered write dirties a cache line that must later be written back.

`vqs` instead streams the subarray through a SIMD loop:

```
pivot_v = broadcast(pivot)
for each 8-lane block:
    v = load(a + i)
    CompressStore(v, v <  pivot_v, left_buf  + lc)   // packs matching lanes, contiguously
    CompressStore(v, v >= pivot_v, right_buf + rc)
scalar tail, then memcpy both buffers back
```

`CompressStore` writes only the lanes the mask selects, packed together with no gaps.
[`vqs_diagram.html`](vqs_diagram.html) draws one block through the partition, a full
partition pass, and the two write patterns side by side.

| n | vqs | `std::sort` | speedup |
|---|---:|---:|---:|
| 1 024 | 21.5 µs | 53.3 µs | 2.48× |
| 8 192 | 130.3 µs | 396.8 µs | 3.05× |
| 65 536 | 1.09 ms | 3.77 ms | 3.47× |
| 262 144 | 4.51 ms | 16.94 ms | 3.75× |
| 1 048 576 | 19.6 ms | 76.6 ms | **3.9×** |

**The trade is more traffic for better-shaped traffic.** `vqs` touches *more* memory than
Hoare — n sequential writes into scratch, then n more copying back, plus 2n ints of scratch
allocation — and still wins by 3.9×, because sequential streams are far cheaper per byte
than scattered dirty writes. The hardware prefetcher handles all of them; none alias.

Unlike the small-n regime, this advantage **grows** with n (2.5× → 3.9×), because the
scattered-write penalty grows as the working set escapes each level of cache.

---

## Two results worth knowing

### AVX-512 is slower than AVX2 here

Zen 5 supports AVX-512, including `vpcompressd` — the single instruction the paper was
designed around. Highway selects it at `-march=native`, giving 16 lanes instead of 8.
It loses:

| build | Highway target | lanes | vqs at n = 1M |
|---|---|---:|---:|
| `-mavx2 …` | AVX2 | 8 | **19.6 ms** |
| `-march=native` | AVX3_DL | 16 | 21.9 ms |

That is a 12% penalty for the wider vector. The standalone
[`vqs_highway_x86.cpp`](vqs_highway_x86.cpp) shows the same direction at about 7%, under
both GCC and clang. Wider vectors mean fewer, larger blocks and a longer
scalar tail, and AMD's 512-bit compress is not twice the throughput of the 256-bit path.
**The instruction the algorithm was designed for is available and is the wrong choice on
this machine** — worth measuring rather than assuming.

### The standard library is not the variable

`libstdc++` and `libc++` land within ~1% of each other at large n (76.4 ms vs 76.7 ms at
n = 1M), so none of the results above depend on which one you link. `libc++` is modestly
faster at small n (185 ns vs 235 ns at n = 16).

---

## Reading the numbers honestly

**Ratios are stable; absolute times are not.** Across repeated runs the speedup columns
reproduce to about ±0.03, while absolute times at mid n vary by up to 25% with clock ramp.
Trust the ratios. The n = 1M row is the exception worth naming: `vqs/std` samples at
3.81–3.94× over four runs, so it is quoted as ~3.9× rather than from a single run. The
first run after a cold start reads low (3.70×); discard it.

**Below n = 32, allocation dominates.** `vqs` allocates two scratch buffers of size n per
call. At n = 16 it skips this entirely and calls the network directly; at n = 32 the two
allocations cost more than the sort. This is a real cost of the API as written, not a
measurement artifact, but it means small-n `vqs` numbers say more about `malloc` than
about sorting.

**This is one data distribution.** Everything here is uniform random `int32`. Partially
sorted input, many duplicates, or larger keys would change the picture — `std::sort`
implementations detect existing runs, and `vqs`'s pivot quality degrades with heavy
duplication.

---

## Files

| File | Description |
|---|---|
| [`sort_survey_x86.cpp`](sort_survey_x86.cpp) | All three regimes, one harness — produces every table above |
| [`sort_compare_n16_x86.cpp`](sort_compare_n16_x86.cpp) | n = 16 field in isolation, Highway vs hand-written AVX |
| [`vqs_highway_x86.cpp`](vqs_highway_x86.cpp) | Vectorized quicksort, standalone with its own sweep |
| [`small_sorting_ryzen.md`](small_sorting_ryzen.md) | Deeper dive on the n = 16 networks |
| [`bitonic_diagram.html`](bitonic_diagram.html) | Diagram: how a bitonic sorting network works, built up from one comparator |
| [`vqs_diagram.html`](vqs_diagram.html) | Diagram: the CompressStore partition, and why its write pattern wins |

```
g++ -O2 -std=c++17 -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul \
  -o sort_survey_x86 sort_survey_x86.cpp -lhwy && taskset -c 4 ./sort_survey_x86
```

The `-maes -mpclmul` flags are not decorative: Highway's `HWY_BASELINE_SSE4` requires
AES and PCLMUL, and AVX2 is gated behind SSE4. Without them a `-mavx2` build silently
selects the **SSSE3** target, which has no `pminsd`, and Highway measures ~2× slower than
it is. Print `hwy::TargetName(HWY_TARGET)` in any Highway benchmark.
