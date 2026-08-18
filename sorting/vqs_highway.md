# vqs_highway: Vectorized Quicksort

**File:** `vqs_highway.cpp`  
**Paper:** Bramas, "A Novel Hybrid Quicksort Algorithm Vectorized using AVX-512 on Intel Skylake," arxiv:1704.08579  
**Platform:** Apple M5, ARM NEON (4-lane int32); portable via Google Highway  
**Build:**
```
clang++ -O2 -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -lhwy \
  -o vqs_highway vqs_highway.cpp
```

---

## Structure

Four components, in order of call depth:

| Component | Fires when | Key operation |
|---|---|---|
| `bitonic16` | n == 16 (leaf) | NEON 10-step sorting network |
| `insertion_sort` | n < 16 (leaf) | scalar fallback |
| `simd_partition` | n > 16 | CompressStore scatter to scratch buffers |
| `simd_qs_core` | always | median-of-3 + recurse |

Entry point `vqs_sort` allocates two scratch buffers of size n once, then calls `simd_qs_core`.  For n ≤ 16 it short-circuits directly to `sort_small` before any allocation.

---

## simd_partition (Algorithm 3)

The core of the paper. Replaces the Hoare two-pointer scan with a
**CompressStore scatter**:

```
pivot_v = broadcast(pivot)                      // N copies of pivot in a register

for each N-element block a[i..i+N):
    v       = LoadU(a + i)
    lt_mask = Lt(v, pivot_v)                    // lane-wise v[j] < pivot
    ge_mask = Ge(v, pivot_v)
    CompressStore(v, lt_mask, left_buf  + lc)   // pack selected lanes, advance lc
    CompressStore(v, ge_mask, right_buf + rc)   // pack selected lanes, advance rc

scalar tail for remaining < N elements

memcpy left_buf  → a[lo .. lo+lc-1]
memcpy right_buf → a[lo+lc .. hi-1]
return lc
```

`CompressStore(v, mask, d, ptr)` writes only the lanes where `mask` is true,
contiguously from `ptr`, and returns the count written.  On AVX-512 this is a
single `vpcompressd` instruction.  On NEON, Highway implements it with a
4-bit LUT (16 entries × one `vtbl` + one `vst1` sequence).

**Memory traffic vs Hoare partition:**

| | Hoare (introsort) | CompressStore (vqs) |
|---|---|---|
| Reads | n (two-pointer scan) | n |
| Writes | 2 per swap, random positions | n sequential (scratch) + n (memcpy back) |
| Aliasing | in-place, self-aliasing swaps | no aliasing |

Hoare's random dirty writes cause cache-line evictions past the L2/L3 boundary.
CompressStore's writes are fully sequential; the hardware prefetcher handles all
six streams (read a, write left_buf, write right_buf, read left_buf, write a,
read right_buf + write a for the second memcpy).

---

## simd_qs_core (Algorithm 4)

```
if hi - lo + 1 ≤ 16:
    sort_small(a + lo, n)
    return

// Median-of-3: sort a[lo], a[mid], a[hi] in place
// then swap median to a[hi]
// Result: a[lo] = min, a[mid] = max, a[hi] = median = pivot

lc = simd_partition(a, lo, hi, pivot, left_buf, right_buf)
// After: a[lo..lo+lc-1] < pivot, a[lo+lc..hi-1] ≥ pivot, a[hi] = pivot

swap(a[lo + lc], a[hi])     // insert pivot; displaced element is ≥ pivot → stays right
p = lo + lc

recurse on [lo,   p-1]
recurse on [p+1,  hi]
```

The median-of-3 trick leaves `a[lo] = min` and `a[mid] = max` as natural
sentinels. The max (at `a[mid]`) ends up inside the partition range `[lo, hi-1]`
but is classified correctly as ≥ pivot — no special handling needed.

After `simd_partition` the array looks like:

```
a: [ < pivot ... | ≥ pivot ... | pivot ]
    lo        lo+lc-1  lo+lc    hi-1    hi
```

`swap(a[lo+lc], a[hi])` drops the pivot into its final position without
shifting the right group; the displaced element (≥ pivot) becomes `a[hi]` and
is included in the right subarray `[p+1, hi]`.

---

## Base Cases

### bitonic16 (n == 16)

The 16-element array is loaded into four `int32x4_t` registers (v0–v3).  Ten
phases of parallel compare-and-swap produce a sorted result with no branches
and no memory accesses between `vld1q` and `vst1q`.

Each CAS step uses two helpers:

```
cas_adj(v, sel):   r = vrev64q_s32(v)          // swap adjacent pairs
                   vbslq_s32(sel, vmin, vmax)   // blend min or max per lane

cas_skip2(v, sel): r = vextq_s32(v, v, 2)      // rotate by 2 lanes
                   vbslq_s32(sel, vmin, vmax)
```

`vbslq_s32` (bitwise select) does the blend in one instruction.  The Highway
equivalent requires `Combine`/`LowerHalf`/`UpperHalf` — three instructions —
which is why the base case uses raw `<arm_neon.h>` rather than Highway.

### insertion_sort (n < 16)

Standard O(n²) insertion sort.  Faster than the padding-to-16 trick
(`memcpy` + `INT_MAX` fill + `bitonic16` + `memcpy`) for very small n.

---

## Why Highway for the Partition, raw NEON for the Base Case

| Operation | Implementation | Reason |
|---|---|---|
| `CompressStore` | Highway | NEON has no native compress; Highway's LUT is optimal |
| `bitonic16` blend | raw NEON `vbslq_s32` | single instruction vs Highway's 3-instruction sequence |

Highway handles the portability where it matters — the partition fires O(n/N)
times per level and is the dominant cost.  The base case fires at leaves, where
the 4 ns per-call difference versus Highway's equivalent (17 ns vs 13 ns for
isolated n=16) is negligible at large n.

---

## Scratch Buffer Design

```cpp
void vqs_sort(vector<int>& v) {
    const int n = v.size();
    if (n <= SORT_BOUND) { sort_small(v.data(), n); return; }
    vector<int> left_buf(n), right_buf(n);
    simd_qs_core(v.data(), 0, n-1, left_buf.data(), right_buf.data());
}
```

Two buffers of size n are allocated once and passed by pointer through all
recursive calls.  Each call to `simd_partition` resets its local `lc`/`rc`
counters from zero, so subarray partitions of any size stay within bounds.
Total extra allocation: 2n integers = 8n bytes (8 MB for n = 1M).

The early exit (`n ≤ SORT_BOUND`) avoids allocation entirely for small n,
giving the same 10 ns bitonic result as introsort at n = 16.

---

## Performance (Apple M5, random int32, best of 7 runs)

| n | vqs_highway | std::sort | speedup | ns/(n log₂ n) |
|---|---|---|---|---|
| 16 | 10 ns | 24 ns | **2.40×** | 0.16 |
| 1 024 | 5.0 µs | 5.0 µs | 1.00× | 0.58 |
| 8 192 | 44.9 µs | 49.8 µs | 1.07× | 0.47 |
| 65 536 | 687 µs | 745 µs | **1.09×** | 0.59 |
| 131 072 | 1.4 ms | 1.7 ms | **1.24×** | 0.63 |
| 524 288 | 6.3 ms | 7.3 ms | **1.16×** | 0.61 |
| 1 048 576 | 12.8 ms | 15.5 ms | **1.21×** | 0.60 |

`ns/(n log₂ n)` is roughly constant from n = 1024 onward, confirming clean
O(n log n) scaling.  std::sort's coefficient rises to ~0.69–0.72 at large n
as its Hoare partition accumulates random cache-line evictions; vqs holds
~0.60 due to the sequential partition access pattern.

The crossover where vqs_highway beats std::sort is around **n = 8192** — the
point where the working set overflows L2/L3 on the M5 and random dirty writes
become expensive.
