

Run in this directory. The Makefile builds all four optimization
levels (`fs_O0` .. `fs_O3`); `-O0` makes every case slow and
compresses the ratios, so `-O2` is where the effect reads cleanest.

```
make && make run          # builds -O0/-O1/-O2/-O3 and runs all four
./fs_O2                   # or run one level directly
./fs_O2 5000000           # optional arg: iterations per thread
```

Measured results on AMD Ryzen AI 9 HX 370 (Zen 5): see
[false_sharing_ryzen.md](false_sharing_ryzen.md).

Example prompt for Claude:

> Run false_sharing under -O0, -O1, -O2, -O3 and build a table of
> speedups. Explain why the speedup changes at each level.

---

**User**
What is false sharing?

**Claude**
# False Sharing

**False sharing** occurs when two threads write to *different* variables
that happen to occupy the *same cache line*.  The hardware's cache
coherence protocol treats the cache line as the atomic unit of
ownership, so every write by one core invalidates all other cores'
copies — even though they are writing to completely independent data.

## Why it happens

Modern CPUs transfer memory between cores in 64-byte cache lines,
not individual bytes.  If `counter[0]` and `counter[1]` are adjacent
`long long` values, they share one cache line:

```
Cache line (64 bytes):
┌────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐
│counter0│counter1│counter2│counter3│counter4│counter5│counter6│counter7│
│ 8 B    │ 8 B    │ 8 B    │ 8 B    │ 8 B    │ 8 B    │ 8 B    │ 8 B    │
└────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘
                   ← one cache line: 64 bytes, owned by one core at a time →
```

When Thread A writes `counter[0]`, the MESI protocol sets the line to
**Modified** and invalidates every other core's copy.  Thread B, about
to write `counter[1]`, must wait for the line to be transferred back
before it can proceed.  The two threads serialize on the cache line
rather than on any shared data — this is the "false" in false sharing.

## Performance impact

```
With false sharing:   threads effectively take turns per cache line write
Without false sharing: threads write in parallel, each owning their line
Typical slowdown:     5–50× depending on core count and access rate
```

## The two fixes

### 1. Structural — pad each counter to a full cache line

```cpp
struct alignas(64) Counter {
    long long value = 0;
    char _pad[56];         // 64 - sizeof(long long) = 56
};
Counter counters[NTHREADS];  // each on its own line
```

### 2. Algorithmic — accumulate locally, write once

```cpp
// Instead of: shared[t]++  every iteration
long long local = 0;
for (long long i = 0; i < N; i++)
    local++;            // stays in a CPU register
shared[t] = local;     // one write at the very end
```

The algorithmic fix is often better: it eliminates cache traffic
entirely during the hot loop, and with -O2 the compiler may collapse
the loop into a single assignment.

## Visual comparison

```
PACKED (false sharing — 8 writers, 1 line):
  Core 0 writes → invalidates cores 1-7 → they stall
  Core 1 writes → invalidates cores 0,2-7 → they stall
  ...  (effectively serialized)

PADDED (no sharing — 1 writer per line):
  Core 0 writes its line  ─┐
  Core 1 writes its line   ├─ all in parallel
  Core 2 writes its line   │
  ...                     ─┘
```

## Rule of thumb

> If multiple threads write to different fields of the same struct,
> or to adjacent elements of the same array, check whether those
> fields share a 64-byte cache line.  If they do, you have false sharing.
