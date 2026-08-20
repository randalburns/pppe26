# Branch optimizations — a guided tour

Three ways to remove or defuse conditional branches, and why each one matters
for a different reason: eliminating mispredictions, trading memory for
comparisons, and unblocking SIMD vectorization. Measured on Apple M5 / Apple
Clang 17 — see each write-up for why Ryzen isn't broken out separately
(no qualitative difference at this level).

Each example is a single `.cpp` file; build with `clang++ -O2 -o <name> <name>.cpp`
and run it directly — no shared Makefile.

---

## 1 · Branch-free arithmetic — when the branch is unpredictable

Replace a data-dependent `if` with arithmetic and bitwise ops so there is no
branch left to mispredict.

| Step | Open | What it teaches |
|------|------|-----------------|
| Read | [branch_free.md](branch_free.md) | signed-shift masks, branchless min/max, comparison-as-integer |
| Run  | [branch_free.cpp](branch_free.cpp) | clamp + threshold-count over 32M random bytes |

Random data defeats the branch predictor outright — branchless code turns a
121 ms misprediction-bound loop into a 5 ms straight-line one (~24x).

---

## 2 · Lookup table — when the branch is small-range

Replace a conditional with an indexed load into a precomputed table.

| Step | Open | What it teaches |
|------|------|-----------------|
| Read | [lut_branch.md](lut_branch.md) | trading memory (a 16-byte L1-resident table) for branches |
| Run  | [lut_branch.cpp](lut_branch.cpp) | hex-encoding 32M bytes, nibble-by-nibble |

Same idea as branch-free arithmetic, applied where the output space is small
enough to fit in cache: 121 ms branchy → 10 ms LUT (~12x).

---

## 3 · Loop unswitching — when the branch is loop-invariant

Hoist a branch that never changes value out of the loop, so the inner loop
body is clean enough for the compiler to vectorize.

| Step | Open | What it teaches |
|------|------|-----------------|
| Read | [loop_unswitch.md](loop_unswitch.md) | why the win here is SIMD, not prediction — the branch is already perfectly predicted |
| Run  | [loop_unswitch.cpp](loop_unswitch.cpp) | `mode`-dispatched elementwise op over 32M floats |

Here the branch was never the misprediction problem — it's the thing standing
between the compiler and NEON. Unswitched loops hit the same 3 ms memory-bandwidth
floor regardless of `mode`; switched loops stay scalar and 2–5x slower.

---

## Files at a glance

**Sources** — `branch_free.cpp`, `lut_branch.cpp`, `loop_unswitch.cpp`
**Write-ups** — `branch_free.md`, `lut_branch.md`, `loop_unswitch.md`

Each write-up's Build section notes the `KEEP_BRANCH()` compiler barrier used
to keep the "branchy" baseline from being auto-optimized into branchless code
by the compiler itself — without it, `-O2`/`-O3` can rewrite the very branch
each example is trying to measure.
