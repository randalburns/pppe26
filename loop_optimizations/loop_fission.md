# Loop Fission Example

# TODO RB ask the students to build an example for cache overflow. HW or in class.

Demonstrates splitting a high-register-pressure loop into two lower-pressure
loops to eliminate compiler spills to the stack.

## Source

[loop_fission.cpp](loop_fission.cpp)

## What is loop fission?

Loop fission (also called loop distribution) is the inverse of loop fusion: a
single loop with a wide body is split into two or more loops over the same
iteration range.  The motivation is register pressure — when a loop body
requires more simultaneously-live values than the CPU has registers, the
compiler spills the excess to the stack, adding load/store instructions on
every iteration of the hot path.  Splitting the loop into narrower pieces
reduces the number of live values per loop and can eliminate those spills
entirely.

## Why this example?

We considered several candidates before settling on multiple simultaneous dot
products (cross-correlations):

- **Statistics (sum, mean, variance, max, min)** — only 4–6 accumulators;
  nowhere near enough to exhaust a modern register file.
- **Polynomial moments (sum, sum², sum³, sum⁴)** — same problem, plus the
  higher-power terms add compute rather than register pressure.
- **Multiple correlations** — each correlation needs its own independent FP
  accumulator.  With 32 correlations you need 32 accumulator registers plus
  32 reference pointers plus a signal pointer and loop counter — well over
  the 32 FP and 31 GP registers available on ARM64.

The correlation structure is also practically motivated: matched filters,
pattern matching, and audio cross-correlation all compute a signal against
many reference vectors simultaneously.  It is a realistic case where a
programmer might naively write one large loop and be surprised by the
performance impact of spilling.

## Picking the right width — 16 vs 32 correlations

The first attempt used 16 correlations.  It ran faster with fission (1.47x)
but showed **no FP register spills** in the assembly — the speedup was coming
from instruction throughput and pipeline effects, not from eliminating spills.

ARM64 (Apple Silicon) has 32 FP registers (v0–v31), so 16 accumulators fit
comfortably alongside the other live values.  Bumping to 32 correlations
exhausted the register file and produced real per-iteration spills in the
fused version, making the demonstration correct and honest.

**Lesson:** register pressure is architecture-specific.  On x86-64 (16 FP
registers), 16 accumulators would have caused spills; on ARM64 you need 32.

## How it works

**Unfissioned — one loop, 32 accumulators:**

```cpp
for (int i = 0; i < n; i++) {
    float s = signal[i];
    c[ 0] += s * ref[ 0][i];
    c[ 1] += s * ref[ 1][i];
    // ...
    c[31] += s * ref[31][i];
}
```

Live values inside the loop body: 32 accumulators + 32 reference pointers +
1 signal pointer + 1 loop counter + 1 current sample = ~67 values competing
for 32 FP + 31 GP registers.  The compiler must spill.

**Fissioned — two loops of 16 accumulators each:**

```cpp
// Loop 1: correlations 0-15
for (int i = 0; i < n; i++) {
    float s = signal[i];
    c[0] += s * ref[0][i];  // ...  c[15] += s * ref[15][i];
}

// Loop 2: correlations 16-31
for (int i = 0; i < n; i++) {
    float s = signal[i];
    c[16] += s * ref[16][i];  // ...  c[31] += s * ref[31][i];
}
```

Each loop has ~35 live values — fits in the register file, no spills.

## Build

```bash
g++ -O1 -o fission_O1 loop_fission.cpp && ./fission_O1
```

## Results (Apple M-series, 32 correlations, N = 10M samples)

| Version | Time | Speedup |
|---------|------|---------|
| Unfissioned (1 loop, 32 accumulators) | 202 ms | baseline |
| Fissioned (2 loops, 16 accumulators each) | 90 ms | **2.24x** |

## Measuring register spills

The compiler annotates its own assembly output with spill/reload comments
when it runs out of registers.  To inspect them:

```bash
g++ -O1 -S loop_fission.cpp -o fission.s
```

Then extract just the two functions and filter for spill annotations:

```bash
# Spills inside the fused (unfissioned) function
awk '/^__Z15correlate_fused/{p=1} /^__Z17correlate_fission/{p=0} p' fission.s \
  | grep -E "Spill|Reload" | grep -E " s[0-9]+| d[0-9]+"

# Spills inside the fissioned function
awk '/^__Z17correlate_fission/{p=1} p' fission.s \
  | grep -E "Spill|Reload" | grep -E " s[0-9]+| d[0-9]+"
```

### Actual output

**Unfissioned — FP spills inside the hot loop:**
```
stp  d15, d14, [sp, #80]    ; 16-byte Folded Spill
stp  d13, d12, [sp, #96]    ; 16-byte Folded Spill
stp  d11, d10, [sp, #112]   ; 16-byte Folded Spill
stp  d9,  d8,  [sp, #128]   ; 16-byte Folded Spill
stp  s0,  s1,  [sp, #68]    ; 8-byte  Folded Spill   ← accumulator spill
ldp  d9,  d8,  [sp, #128]   ; 16-byte Folded Reload
ldp  d11, d10, [sp, #112]   ; 16-byte Folded Reload
ldp  d13, d12, [sp, #96]    ; 16-byte Folded Reload
ldp  d15, d14, [sp, #80]    ; 16-byte Folded Reload
```

The `stp s0, s1` line is the critical one: two FP accumulator values are
spilled to the stack **inside the hot loop**, meaning every iteration pays an
extra store and later a reload.  The d8–d15 saves/restores are callee-saved
register saves at function entry/exit — normal overhead, not per-iteration.

**Fissioned — FP spills:**
```
stp  d9, d8, [sp, #48]    ; 16-byte Folded Spill
ldp  d9, d8, [sp, #48]    ; 16-byte Folded Reload
```

Only d8–d9 appear, and only in the function prologue/epilogue (callee-save
convention).  No accumulator spills inside either hot loop.

## Key takeaway

Loop fission trades extra iterations over the data (the signal array is read
twice instead of once) for the elimination of per-iteration stack spills.
When spills are on the critical path — as they are here, on every iteration
of a tight accumulation loop — the reduced instruction count per iteration
more than compensates for the extra memory pass.  The 2.24x speedup shows
that spill elimination was the dominant bottleneck, not memory bandwidth.


