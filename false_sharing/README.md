# False Sharing

**False sharing** happens when two threads write to *different*
variables that happen to live on the *same 64-byte cache line*. The
cache coherence protocol (MESI) treats the line as one indivisible
unit of ownership: every write by one core invalidates every other
core's copy of the line, even though the cores aren't touching any
data the other one cares about. The threads end up serializing on
the cache line instead of running in parallel.

`false_sharing.cpp` demonstrates the effect and its two standard
fixes:

1. **Structural** — pad each counter out to 64 bytes so every thread
   owns its own cache line.
2. **Algorithmic** — accumulate into a thread-local register and
   write the shared counter once at the end, instead of on every
   iteration.

See [false_sharing_diagram.html](false_sharing_diagram.html) for a
visual walkthrough of the packed-vs-padded cache line layout, and
`false_sharing.md` for the original write-up and rule of thumb.

```
make && ./false_sharing
```

Build with `-O2`; `-O0` makes every case slow enough that the false
sharing effect gets buried in interpreter-level overhead.

## Results

### Apple Silicon (arm64, unpinned — macOS has no thread-affinity API)

10 threads, 50,000,000 iterations/thread:

| Test | Packed (false sharing) | Fixed | Speedup |
|---|---|---|---|
| Padded to 64 B | 106 ms | 74 ms | 1.43x |
| Thread-local accumulate | 106 ms | 0 ms* | 106x |

\* The compiler (-O2) proves `local++` repeated N times has no
observable effect other than the final value, and collapses the
loop to `local = N`. Cache-friendly code turned out to also be
compiler-friendly code — the "fix" removed the loop entirely.

### Ryzen (AI 9 HX 370, Strix Point, Linux, threads pinned per-core)

TBD — not yet re-run against the current portable version of the
benchmark. The earlier Ryzen-specific build of this file also swept
thread placement across the chip's two L3/CCX domains (SMT
siblings, same-CCX, cross-CCX) and found a counterintuitive result:
the cross-CCX pairing had the *smallest and most repeatable*
false-sharing penalty, while same-CCX pairs were both worse and
noisier. The likely explanation was handoff batching — a slower
transfer means the owning core holds the line longer and retires
more increments per handoff, so raising per-transfer latency lowered
the total number of transfers. That sweep was removed to keep the
benchmark portable; worth re-running standalone on the Ryzen box
if the topology-dependent behavior is interesting again.
