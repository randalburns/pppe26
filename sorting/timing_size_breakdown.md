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
