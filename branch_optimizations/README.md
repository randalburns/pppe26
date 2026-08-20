# Branch optimizations

Three ways to remove or defuse conditional branches, and why each one matters
for a different reason: eliminating mispredictions, trading memory for
comparisons, and unblocking SIMD vectorization. Measured on Apple M5, Apple Clang 17.

Each example is a single `.cpp` file; build with `clang++ -O2 -o <name> <name>.cpp`
and run it directly — no shared Makefile.

---

## Background: what is a branch miss?

A **branch** is any instruction where the CPU has to pick which instruction
runs next — an `if`, a loop condition, a `switch`. Modern CPUs are
**pipelined**: they don't run one instruction start-to-finish before starting
the next. Instead, dozens of instructions are in flight at once, each partway
through stages like fetch, decode, and execute. This only works if the CPU
knows *which* instructions to fetch several steps ahead of actually
evaluating the branch — but the branch's outcome (true or false) isn't known
until it executes, which is late in the pipeline.

The CPU's solution is **branch prediction**: a piece of hardware guesses
which way the branch will go — based on that branch's history — and the
pipeline starts fetching and executing instructions from the guessed path
*before* the branch is actually resolved. This is called **speculative
execution**. If the guess is right, the CPU gained a head start for free.

If the guess is wrong — a **branch misprediction** — every instruction that
was speculatively started down the wrong path has to be thrown away, and the
pipeline restarts from the correct path. This is a **pipeline flush**, and it
costs a fixed penalty of roughly 15–20 cycles on modern CPUs (measured at
≈15 on Apple M-series, ≈15–20 on x86). That penalty is paid in full no matter
how simple the branch itself is — an `if (x > 0)` mispredicted costs the same
15+ cycles as a mispredicted branch guarding a hundred lines of code.

**Why it matters:** branch predictors are excellent at finding patterns —
loops with a fixed trip count, branches that are almost always taken, `if`
statements on sorted data. But when the outcome is effectively random (as
with unpredictable input, hashing, or data-dependent conditionals), the
predictor is right about as often as a coin flip, and every wrong guess costs
that full 15–20 cycle flush. For a branch inside a hot loop processing
millions of elements, that overhead can dominate total runtime — turning a
loop that should take a few milliseconds into one that takes over 100. The
three examples below are three different techniques for avoiding that cost.

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
