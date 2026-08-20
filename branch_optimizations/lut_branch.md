# Lookup Table Instead of Branches: Hex Encoding

Demonstrates replacing a conditional expression with a precomputed table
indexed by the input value, eliminating branches entirely.

## Source

[lut_branch.cpp](lut_branch.cpp)

## The idea

A lookup table trades memory for branches: precompute all possible outputs,
store them in an array, and index by input.  One memory load replaces one or
more conditional tests.

```cpp
// Branch: evaluate a condition, choose a path
char to_hex(int nibble) {
    if (nibble < 10) return '0' + nibble;
    return 'a' + nibble - 10;
}

// LUT: index a precomputed 16-byte table — no condition, no prediction
static const char HEX[] = "0123456789abcdef";
char to_hex(int nibble) { return HEX[nibble]; }
```

The table fits in a single 16-byte L1 cache line.  After the first access
it stays in L1 and each subsequent lookup costs 1–4 cycles with no
misprediction risk.

```
HEX[] = "0123456789abcdef"        16 bytes — one L1 cache line

        0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
       ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
 value │ 0  │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │ 7  │ 8  │ 9  │ a  │ b  │ c  │ d  │ e  │ f  │
       └────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
                                                                 ▲
                                                   nibble = 11 ──┘

        out = HEX[nibble]  →  'b'   (one load, no branch)
```

No condition is ever evaluated: the nibble *is* the address offset. Compare
that to the branchy version, which has to test `nibble < 10` and take one of
two paths before it knows what to store — a decision the predictor can get
wrong.

---

## This example: hex encoding

Converts N bytes to their 2-character hex representation.  Each byte splits
into two 4-bit nibbles; each nibble hits the branch above.

With uniformly random input, nibble values 0–9 (digits) appear 62.5% of the
time and a–f (letters) appear 37.5% of the time.  The branch mispredicts
≈ 37.5% of the time per nibble — close to worst case.

Expected misprediction overhead:

```
N × 2 nibbles × 37.5% miss × 15 cycles / 4 GHz ≈ 94 ms
```

---

## Build

```bash
clang++ -O2 -o lut_branch lut_branch.cpp && ./lut_branch
```

No special flags are required, and any optimization level works.
`encode_branchy` uses single-armed `if` statements each containing a
`KEEP_BRANCH()` compiler barrier, which holds them as real conditional jumps
on both GCC and Clang at every optimization level.  (The barrier must sit in
only one arm: with a barrier in both arms of an `if`/`else`, Clang factors the
common barrier out and re-forms a CMOV anyway.)

This replaces the earlier `g++-15 -O1 -fno-if-conversion` build line, which was
GCC-only and did not survive `-O2`, where the compiler auto-vectorized the
branchy path into a SIMD select and beat the LUT outright.

---

## Results (Apple M5, Apple Clang 17, N=32M bytes)

Minimum across 3 invocations, each an internal best-of-5.

| version | time | cycles/byte | speedup |
|---------|------|-------------|---------|
| branchy | 121 ms | 14.42 | 1.00x |
| lut     | 10 ms  | 1.19  | **12.10x** |

Measured misprediction overhead: 111 ms (expected: 94 ms).

---

## When a LUT makes sense

| good fit | poor fit |
|---|---|
| Input range is small (≤ 256 values → fits in L1) | Input range is large (table doesn't fit in cache) |
| Output is complex to compute per-element | Output is trivial (one instruction) |
| Branch outcome is unpredictable | Branch is highly predictable |
| Same table reused across many elements | Table used only once |
