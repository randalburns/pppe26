# The memory hierarchy

A measured description of one machine's memory system: an **AMD Ryzen AI 9 HX 370**
(Zen 5, "Strix Point"), 12 cores / 24 threads, LPDDR5X, one NVMe SSD, Linux, gcc 13.3.
Nothing here is quoted from a datasheet — every number comes from one of the nine
benchmarks in this directory, and every figure is drawn from the `.csv` files they emit.

Cache geometry is read from `/sys/devices/system/cpu/cpu*/cache/`, so the write-up
still tells the truth if you run it somewhere else.

---

## Start here

| If you want | Open | |
|---|---|---|
| The full treatment, with the code that produced each plot | [cache_hierarchy.ipynb](cache_hierarchy.ipynb) | 12 sections, 6 figures |
| The same ground visually, in a browser, no server | [cache_map.html](cache_map.html) | floorplan, latency ladder, coherence map |
| Why a 1024×1024 matrix is slow down its columns | [why_1024_is_slow.html](why_1024_is_slow.html) | four bits of an address |
| Why an array that fits in L3 still misses | [tlb_cliff.html](tlb_cliff.html) | address translation and huge pages |

The three HTML pages are standalone — double-click them. They cross-link to each
other and need no network beyond web fonts, which fall back to system faces.

---

## What this machine turns out to be

| | Measured | Where |
|---|---|---|
| Cache line | **64 bytes** at every level | §6 |
| Latency, L1 → DRAM | **4 → 530 cycles** (0.78 ns → 104 ns) | §3 |
| Hardware prefetching | worth up to **77×** on a predictable pattern | §4 |
| Address translation | **2.4×** on an L3-resident random pattern | §5 |
| Line utilization | **8×** swing between stride-8 and stride-64 | §6 |
| DRAM bandwidth | **38.5 GB/s** from one core — and 38.8 from twelve | §7 |
| Array dimensions | a power-of-two row length costs up to **32×** | §8 |
| Sharing one cache line | **18 ns** SMT, ~20 ns intra-complex, **~167 ns** across | §9 |
| Below DRAM | NVMe SSD at **45.8 µs** — 59,000 L1 hits | §10 |

The chip is not the uniform 12-core part its spec sheet implies. It is two core
complexes with different core types and separate L3 slices — 4 × Zen 5 on 16 MB,
8 × Zen 5c on 8 MB — so a thread's last-level cache and its cost to synchronize
both depend on where the scheduler put it. That asymmetry runs through §2, §7 and §9.

---

## The benchmarks

Every number above traces back to one of these. Each `.c` file opens with a comment
on what it measures and, more usefully, which measurement traps it had to avoid.

| Program | Measures | Emits | § |
|---|---|---|---|
| [cache_latency.c](cache_latency.c) | pointer-chase latency vs working set, 4 KB vs 2 MB pages | `latency.csv` | 3, 4, 5 |
| [stride_bandwidth.c](stride_bandwidth.c) | line utilization vs stride; bandwidth vs working set | `stride_bandwidth.csv` | 6, 7 |
| [bandwidth_scaling.c](bandwidth_scaling.c) | aggregate DRAM bandwidth vs thread count | `bandwidth_scaling.csv` | 7 |
| [row_column.c](row_column.c) | nested-loop order and the power-of-two set conflict | `row_column.csv` | 8 |
| [core_to_core.c](core_to_core.c) | cache-line handoff latency, all 66 core pairs | `core_to_core.csv` | 9 |
| [storage_latency.c](storage_latency.c) | NVMe random-read latency and sequential bandwidth | `storage.csv` | 10 |
| [tlb_walk.c](tlb_walk.c) | TLB capacity, by touching many pages and little data | `tlb_walk.csv` | 5 |
| [spin_cost.c](spin_cost.c) | what a PAUSE spin loop costs, per core type | `spin_and_sets.txt` | — |
| [set_conflict.c](set_conflict.c) | traversal time against L1 sets reachable | `spin_and_sets.txt` | — |

The last two support the HTML pages rather than the notebook.

```
make            # build all nine
make data       # run them all and overwrite the .csv files
```

`make data` takes roughly twelve minutes and wants an otherwise idle machine —
every one of these measures a shared resource. The notebook plots the committed
`.csv` files by default; set `RUN_BENCHMARKS = True` in its first cell to
regenerate them from the notebook instead.

---

## Measurement notes

These are the transferable part. Two of the benchmarks here had to be rewritten
after their first run produced confident, reproducible, wrong answers.

**The clock moves.** This part runs between 0.6 and 5.16 GHz. Measuring it once at
startup and converting nanoseconds to cycles for the rest of the run silently
corrupts every number — an early `cache_latency` reported L1 at 2.8 cycles and L2 at
9.8, when the true values are 4 and 14, because the core boosted from 3.58 to
5.07 GHz partway through. Every program here re-measures the clock with a dependent
add chain immediately before each data point.

**A fixed stride measures the prefetcher, not the cache.** LMBench's constant-stride
walk is the easiest possible pattern for hardware to predict. Latency here comes from
a *randomized* pointer chase, where each load's address is the previous load's result.

**Aggregate over the slowest thread.** `bandwidth_scaling` first summed per-thread
rates, which credits threads that finish early with bandwidth they only got because
the others had stopped. Total bytes over the slowest thread's elapsed time is the
honest figure.

**One accumulator measures the adder.** The bandwidth loops use four, or they report
FP latency instead of memory throughput — the same trick as [../ILP/sep_dependent.cpp](../ILP/sep_dependent.cpp).
Integer accumulators, deliberately: FP reductions are not reassociable, so the
compiler will not vectorize them without `-ffast-math` and the read test silently
becomes compute-bound.

**Watch for the other effect.** `tlb_walk` rotates which line it touches inside each
page. Without that, every access would land in the same L1 set and it would measure
the set conflicts from [why_1024_is_slow.html](why_1024_is_slow.html) instead of the TLB.

**Report the spread when there is one.** Cross-complex handoffs scatter between 67 and
171 ns run to run while intra-complex pairs repeat to within a few percent, so
`core_to_core` reports min, median and max. Collapsing that to one number would hide
the most useful thing it has to say.

---

## Where this connects

| Topic | Directory |
|---|---|
| Loop interchange, tiling, fusion, fission | [../loop_optimizations/](../loop_optimizations/README.md) |
| False sharing, measured at 19× | [../false_sharing/](../false_sharing/false_sharing_ryzen.md) |
| Software prefetching for gathers | [../prefetch.example/](../prefetch.example/prefetch.md) |
| Arithmetic intensity and the bandwidth ceiling | [../roofline/](../roofline/) |
| Vectorization, which multiplies the cost of a bad layout | [../vectorization/](../vectorization/README.md) |
| Sorting: the canonical cache-sensitive algorithm | [../sorting/](../sorting/intro.md) |
