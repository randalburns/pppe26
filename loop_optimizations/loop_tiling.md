# Loop Tiling Example

Demonstrates loop tiling (loop blocking) for matrix transpose, including the
process of finding the right example and the surprising cache set-conflict
result that emerges when the matrix dimension is a power of 2.

## Source

[loop_tiling.cpp](loop_tiling.cpp)

## What is loop tiling?

Loop tiling (also called loop blocking) restructures a loop's iteration space
into smaller rectangular blocks so that the working set for each block fits in
a fast cache level (L1 or L2).  Without tiling, the same data may be loaded
from DRAM O(N) times as the cache evicts it before it is reused.  With tiling,
data is loaded once per block and reused as many times as the block allows.

---

## Finding the right example

### Attempt 1 — Matrix multiplication

The classic loop tiling textbook example.  The naive triple loop (`i,j,k`
order) accesses `B[k][j]` down a column of B — stride N — causing cache misses.

**What we found:** most of the benefit came from loop *reordering* (switching
to `i,k,j` order, which turns the column access into a row access), not from
blocking itself.  Adding a reordered-but-not-tiled baseline showed:

| version | time | vs naive |
|---------|------|----------|
| naive (i,j,k) | 811 ms | 1.00x |
| reordered (i,k,j, no tiling) | 265 ms | **3.06x** |
| tiled (best) | 286 ms | 2.82x |

The tiled version never beat the reordered baseline.  The hardware prefetcher
handles sequential row access of B regardless of tile size, so blocking into
L1 added nothing beyond what reordering already achieved.

**Lesson:** matrix multiply is a poor isolation example because the loop-order
transformation is conflated with the blocking transformation.

### Attempt 2 — 2D box-filter smoothing (stencil)

A 2D stencil over a 2048×2048 matrix with 3×3, 5×5, and 7×7 kernels.  Each
output element reads a `(2r+1)×(2r+1)` neighborhood of input.

**What we found:** tiling was uniformly *slower* than naive for all three
kernel sizes.  The naive inner loop scans `in[i+ki][j+kj]` with `j`
incrementing — that is `2r+1` simultaneous stride-1 streams (one per kernel
row).  The Apple M-series prefetcher handles up to ~8 simultaneous streams,
covering all three kernel sizes (3, 5, 7 streams).  There were no cold misses
for tiling to eliminate.

**Lesson:** the 2D box filter is the wrong stencil for demonstrating tiling on
modern hardware with aggressive hardware prefetchers.  The textbook motivation
(avoid repeated row reloads) does not apply when the prefetcher pre-empts
those misses.

### Attempt 3 — Matrix transpose ✓

Matrix transpose has a cache miss problem that the hardware prefetcher
**cannot** fix:

- Reading `in[i][j]` with `j` incrementing: stride-1, prefetchable.
- Writing `out[j][i]` with `j` incrementing: stride N (one full row per
  write), a different cache line each time, **unpredictable** for a stride-1
  prefetcher.

For a 4096×4096 double matrix (128 MB), every write to the output evicts a
cache line that was never fully used.  Tiling fixes this by processing a B×B
block: the B output rows of the tile stay in cache and receive B writes each
before eviction.

---

## How it works

**Naive transpose — reads stride-1, writes stride-N:**

```cpp
for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
        out[j][i] = in[i][j];   // write jumps N doubles per step
```

**Tiled transpose — B×B blocks:**

```cpp
for (int ii = 0; ii < N; ii += tile)
for (int jj = 0; jj < N; jj += tile) {
    int ilim = min(ii + tile, N);
    int jlim = min(jj + tile, N);
    for (int i = ii; i < ilim; i++)
    for (int j = jj; j < jlim; j++)
        out[j][i] = in[i][j];   // output rows stay in cache for the tile
}
```

Within each tile the B writes to the output all target the same B-row block of
the output matrix.  That block stays in L1 for the duration of the tile, so
each output cache line is written B times before eviction.

---

## Build

```bash
g++ -O1 -o tiling_O1 loop_tiling.cpp && ./tiling_O1
```

---

## Results (Apple M-series, N=4096, L1=64 KB)

| tile | work.set | L1 | time | speedup |
|------|----------|----|------|---------|
| naive | — | — | 74 ms | 1.00x |
| 8 | 1 KB | yes | 25 ms | **2.96x** |
| 16 | 4 KB | yes | 42 ms | 1.76x |
| 32 | 16 KB | yes | 64 ms | 1.16x |
| 48 | 36 KB | yes | 66 ms | 1.12x |
| 64 | 64 KB | yes | 66 ms | 1.12x |
| 80 | 100 KB | — | 36 ms | 2.06x |
| 128 | 256 KB | — | 75 ms | 0.99x |
| 256+ | >1 MB | — | ~75 ms | ~1.00x |

---

## The cache set-conflict surprise

The naive working-set formula predicts B=64 as the optimal tile:

```
2 × B² × 8 bytes ≤ L1
2 × 64² × 8 = 65536 bytes = 64 KB  ✓
```

But the empirical best is **B=8**, and performance degrades from B=8 to B=64
despite all those tiles fitting in L1 by raw byte count.

### Why: power-of-2 set aliasing

The output write stride is `N × 8 = 4096 × 8 = 32768 bytes = 32 KB`.

With a 64 KB, 8-way set-associative L1 (64-byte lines):

```
L1 sets           = 64 KB / (8 ways × 64 bytes) = 128 sets
Lines per stride  = 32768 / 64 = 512
512 mod 128       = 0  ← full set aliasing
```

Because `N = 4096` is a power of 2, the row stride is an exact multiple of the
cache size.  Every output row maps to the **same** 128 cache sets.  With 8-way
associativity, at most **8 output rows** can coexist in L1 simultaneously
before the least-recently-used row is evicted.

B=8 places exactly 8 output rows in the tile — a perfect fit for the 8-way
limit.  B=16 needs 16 rows; rows 9–16 evict rows 1–8 before they are finished,
undoing the benefit of tiling.

### The simple formula is necessary but not sufficient

`2×B²×8 ≤ L1` only accounts for raw byte capacity.  When the access stride is
a large power-of-2 multiple of the cache line size, the correct constraint is:

```
B ≤ cache associativity   (for full-aliasing strides)
```

Here that gives B ≤ 8, matching the empirical result exactly.  The anomalous
B=80 result (2.06x) occurs because 80 is not a power of 2 — it disrupts the
aliasing pattern and partially avoids the conflict.

### Avoiding the problem in practice

- **Pad the matrix width** to a non-power-of-2 (e.g., N+8) so the row stride
  breaks the aliasing.  This is why BLAS and NumPy often pad array dimensions.
- **Use non-power-of-2 tile sizes** when N is a power of 2.

---

## Key takeaways

1. **Loop reordering and loop tiling are distinct transformations.**  Matrix
   multiply conflates them; matrix transpose isolates the blocking benefit.

2. **Hardware prefetchers can eliminate the motivation for tiling.** The 2D
   box filter showed no benefit because the prefetcher handled all simultaneous
   row streams.  Tiling helps only when there are cold misses the prefetcher
   cannot predict.

3. **The working-set formula ignores cache set conflicts.**  For power-of-2
   strides, the effective capacity is `associativity × cache_line`, not
   `L1_size`.  The empirical sweep is the only reliable way to find the true
   sweet spot.

4. **The speedup is real and significant (3x) but at a much smaller tile size
   than theory predicts.**  B=8 rather than B=64.
