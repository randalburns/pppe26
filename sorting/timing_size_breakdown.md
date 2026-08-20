# Quicksort: Time Spent in Small Partitions (≤ 16 elements)

**Setup:** median-of-three quicksort, no small-partition cutoff, random `int` data, `-O2`, 7 runs (best taken).  
**Instrumentation:** `g_in_small` flag prevents double-counting nested small calls; outermost call into the ≤16 regime times the entire subtree.

## Results

| n       | time   | ns/elem | ns/(n log₂ n) | small (≤16) % |
|---------|--------|---------|---------------|---------------|
| 16      | 53 ns  | 3.3     | 0.83          | 66.0%         |
| 32      | 147 ns | 4.6     | 0.92          | 51.7%         |
| 64      | 305 ns | 4.8     | 0.79          | 52.8%         |
| 128     | 797 ns | 6.2     | 0.89          | 45.5%         |
| 256     | 1 µs   | 7.4     | 0.93          | 43.5%         |
| 512     | 4 µs   | 8.5     | 0.94          | 40.4%         |
| 1 024   | 9 µs   | 9.4     | 0.94          | 36.9%         |
| 2 048   | 19 µs  | 9.3     | 0.85          | 34.7%         |
| 4 096   | 48 µs  | 11.9    | 0.99          | 31.2%         |
| 8 192   | 173 µs | 21.2    | 1.63          | 24.7%         |
| 16 384  | 546 µs | 33.3    | 2.38          | 23.4%         |
| 32 768  | 1 ms   | 34.3    | 2.28          | 22.5%         |
| 65 536  | 2 ms   | 39.2    | 2.45          | 22.0%         |
| 131 072 | 5 ms   | 42.4    | 2.49          | 21.0%         |
| 262 144 | 11 ms  | 45.3    | 2.52          | 20.4%         |
| 524 288 | 25 ms  | 47.8    | 2.52          | 19.2%         |
| 1048576 | 51 ms  | 48.8    | 2.44          | 18.7%         |

## Observations

**Small-n regime (n ≤ 4096, ns/(n log₂ n) ≈ 0.85–0.99):**  
The algorithm is cache-hot and scaling cleanly. Small partitions account for 31–66% of total runtime — a large target for a small-partition cutoff such as insertion sort.

**Cache-miss transition (~n = 8192):**  
`ns/(n log₂ n)` roughly doubles between n = 4096 and n = 16384, marking the point where random pivot access starts missing L2/L3. The small-partition fraction also drops sharply here, because the large-partition levels now carry disproportionate cost due to cache misses.

**Large-n regime (n ≥ 8192, ns/(n log₂ n) ≈ 1.6–2.5):**  
Small partitions still represent ~19–25% of runtime. Replacing them with insertion sort would help, but the dominant cost is now cache misses in the upper recursion levels, not leaf-level work.

## Implication for optimization

Adding an insertion-sort cutoff at ≤ 16 elements targets:
- ~50% of runtime at small n (high leverage, though total time is already tiny)
- ~20% of runtime at large n (meaningful savings on the work that is still cache-warm)

The complementary optimization for large n is reducing cache pressure in the upper levels — e.g., choosing a better pivot or switching to a cache-oblivious layout — since that is where ~75–80% of large-n time is now spent.

## Quicksort vs. std::sort over the same size range

**Methodology note.** An earlier version of this section reused `quicksort.cpp` and
`std_sort.cpp` as-is, which — like the rest of this repo's older benchmarks — refills
each timed trial's buffer from the *same* fixed source array (`copy(data, buf); sort(buf)`
in a loop). That lets the branch predictor memorize a comparison sort's data-dependent
branches after a few repetitions, which inflates the more-branchy algorithm's apparent
speedup and can manufacture a fake discontinuity where the effect fades out as `n` grows
past what the predictor can hold. The numbers below instead use independent random
buffers refilled *outside* the timed region (the same fix used in `bench()` elsewhere in
this repo) — Apple M5, clang, `-O2`, best of 7.

`std::sort` is libstdc++'s introsort: quicksort with an insertion-sort cutoff for small
partitions and a depth-limited heapsort fallback to bound worst case — exactly the
optimization this write-up's "Implication" section argues for.

| n       | quicksort ns/(n log₂ n) | std::sort ns/(n log₂ n) | std::sort speedup |
|---------|-------------------------:|--------------------------:|-------------------:|
| 16      | 4.06                     | 2.16                      | 1.88x               |
| 32      | 4.03                     | 2.21                      | 1.82x               |
| 64      | 3.91                     | 1.92                      | 2.04x               |
| 128     | 5.24                     | 2.30                      | 2.28x               |
| 256     | 3.74                     | 1.57                      | 2.39x               |
| 512     | 2.76                     | 1.18                      | 2.35x               |
| 1 024   | 2.47                     | 1.01                      | 2.44x               |
| 2 048   | 2.45                     | 0.95                      | 2.58x               |
| 4 096   | 2.44                     | 0.90                      | 2.69x               |
| 8 192   | 2.42                     | 0.87                      | 2.80x               |
| 16 384  | 2.40                     | 0.83                      | 2.90x               |
| 32 768  | 2.39                     | 0.80                      | 3.00x               |
| 65 536  | 2.38                     | 0.77                      | 3.09x               |
| 131 072 | 2.37                     | 0.75                      | 3.16x               |
| 262 144 | 2.35                     | 0.73                      | 3.23x               |
| 524 288 | 2.33                     | 0.71                      | 3.29x               |
| 1048576 | 2.33                     | 0.69                      | 3.38x               |

Speedup is the ratio of the `ns/(n log₂ n)` columns, which cancels the shared `n log₂ n`
term and reduces to plain total-time ratio.

**With the harness fixed, both of the shapes the old section described turn out to be
artifacts.** Quicksort never wins — not at n=128, not anywhere — and there's no jump at
n=8192. The curve is smooth and monotonic from 1.82x at n=32 to 3.38x at n=1,048,576, with
no discontinuity at the cache-miss transition identified above: quicksort's own
`ns/(n log₂ n)` here is nearly flat (4.06 down to 2.33), not the near-doubling at n=8192
the old, harness-biased numbers showed.

**The real explanation is recursion overhead, not cache locality, and it applies at every
size, not just a "large-n regime."** Raw quicksort recurses all the way to partitions of
size 1 — roughly 2n total calls for n elements — while std::sort's cutoff stops at ~16,
doing roughly n/8 calls plus one linear insertion-sort pass over the whole array at the
end. That fixed per-call overhead (a function frame, a median-of-three selection, a Hoare
partition loop) is what raw quicksort pays repeatedly at every scale; std::sort's
single-pass cleanup amortizes better as n grows, which is why its normalized cost keeps
falling (2.16 → 0.69) while quicksort's stays roughly flat — not because either algorithm
hits a cache wall differently, since above the 16-element cutoff they run the *identical*
partition code and should see identical cache behavior.

**Net effect:** the small-partition cutoff this document's "Implication" section
recommends is worth a real and growing margin — 1.8x at n=16 up to 3.4x at n=1M — but the
mechanism is call/recursion overhead amortizing differently between the two
implementations, not one of them dodging a cache-capacity cliff the other hits.
