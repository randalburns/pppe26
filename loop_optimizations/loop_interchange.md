# Loop Interchange Example

Demonstrates reordering nested loop indices to convert stride-N column access
into stride-1 row access for matrix-vector multiplication.

## Source

[loop_interchange.cpp](loop_interchange.cpp)

## What is loop interchange?

Loop interchange swaps the order of two nested loops.  When the loop body
accesses a multi-dimensional array, the order of the indices determines the
memory access stride.  In row-major storage, the rightmost index is
contiguous in memory.  If the inner loop increments a non-rightmost index,
each iteration steps by a full row — potentially thousands of bytes — causing
a cache miss on every access.  Swapping the loops so the inner loop increments
the rightmost index restores stride-1 access with no algorithmic change.

---

## How it works

**j,i order — column access of A (cache-unfriendly):**

```cpp
for (int j = 0; j < N; j++) {
    double xj = x[j];
    for (int i = 0; i < M; i++)
        y[i] += A[i][j] * xj;
}
```

The inner loop increments `i`, stepping through column `j` of A.  In
row-major storage, `A[i][j] = A[i*N + j]`, so consecutive `i` values are
`N` doubles apart — a stride of `N × 8 = 32 KB` per step.  For N=4096 every
access to A is a cold cache miss.

**i,j order — row access of A (cache-friendly):**

```cpp
for (int i = 0; i < M; i++) {
    double acc = 0.0;
    for (int j = 0; j < N; j++)
        acc += A[i][j] * x[j];
    y[i] = acc;
}
```

The inner loop increments `j`, scanning row `i` of A sequentially — stride 8
bytes, one cache line covers 8 elements.  `y[i]` is promoted to a scalar
accumulator held in a register for the entire inner loop.  The inner working
set is one A row (32 KB) plus the full `x` vector (32 KB), totalling 64 KB —
exactly the L1 data cache size, so both streams stay in L1 for the duration
of the inner loop.

---

## Build

```bash
g++ -O1 -o interchange_O1 loop_interchange.cpp && ./interchange_O1
```

---

## Results (Apple M-series, M=N=4096, A=128 MB, L1=64 KB)

| Version | Access pattern | Time | Speedup |
|---------|---------------|------|---------|
| j,i order | stride-N column access, one miss per element | 47 ms | baseline |
| i,j order | stride-1 row access, L1 reuse | 12 ms | **3.92x** |

Access pattern summary:

```
A row (one inner loop pass):  32 KB
x vector:                     32 KB
i,j inner working set:        64 KB  (fits L1 exactly)
j,i inner stride on A:        32 KB per step  (one miss per element)
```

---

## Key takeaways

1. **The inner loop index should be the rightmost array index.**  In row-major
   storage this is the only way to achieve stride-1 access.  Any other
   ordering produces stride proportional to the row width.

2. **Loop interchange is a pure reordering — no algorithmic change.**  The two
   versions compute identical results (max_err = 0).  All the benefit comes
   from access pattern, not from doing less work.

3. **The working-set argument makes the benefit predictable.**  At N=4096 the
   j,i stride is 32 KB, which exceeds any cache line size by a factor of 512.
   Every inner-loop access to A is a miss.  After interchange, the 64 KB
   working set fits exactly in L1, so both A and x are served from L1 on
   every access.

4. **The hardware prefetcher cannot rescue stride-N access.**  The prefetcher
   recognises constant strides, but a 32 KB stride means each prefetch targets
   a distinct cache line that is immediately evicted — there is nothing to
   prefetch ahead of time that would stay in cache.
