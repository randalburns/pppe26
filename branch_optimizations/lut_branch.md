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
g++-15 -O1 -fno-if-conversion -o lut_branch lut_branch.cpp && ./lut_branch
```

`-fno-if-conversion` keeps the ternary in `encode_branchy` as a real branch
instruction.  Without it the compiler emits CSEL (conditional select), which
is already branchless and narrows the gap considerably.

---

## Results (Apple M4, N=32M bytes)

| version | time | cycles/byte | speedup |
|---------|------|-------------|---------|
| branchy | 125 ms | 14.9 | 1.00x |
| lut     | 12 ms  | 1.4  | **10.42x** |

Measured misprediction overhead: 113 ms (expected: 94 ms).

---

## When a LUT makes sense

| good fit | poor fit |
|---|---|
| Input range is small (≤ 256 values → fits in L1) | Input range is large (table doesn't fit in cache) |
| Output is complex to compute per-element | Output is trivial (one instruction) |
| Branch outcome is unpredictable | Branch is highly predictable |
| Same table reused across many elements | Table used only once |
