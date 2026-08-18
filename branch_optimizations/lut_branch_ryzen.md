# Lookup Table vs. Branch Benchmarks — AMD Ryzen AI 9 HX 370

Measured on AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu, GCC 13.3.0,
N=32M bytes, best of 5 runs. See [lut_branch.md](lut_branch.md) for the Apple M5 baseline this
compares against.

---

## Build note

Same command as the doc, using the system's real GCC (not an Apple-Clang alias):

```
g++ -O1 -fno-if-conversion -o lut_branch lut_branch.cpp
```

Required the same `(long long)` cast fix to `duration_cast<milliseconds>(...).count()` as
[branch_free_ryzen.md](branch_free_ryzen.md), for the same GCC/libstdc++ template-deduction reason.

---

## Results (`-O1 -fno-if-conversion`, N=32M bytes; stable within ~1%)

| version | time | cycles/byte | speedup |
|---------|-----:|------------:|--------:|
| branchy | 152 ms | 18.1 | 1.00x |
| lut | 12 ms | 1.4 | **12.7x** |

---

## Comparison to Apple M5

| version | M5 | Ryzen |
|---|---:|---:|
| branchy | 125 ms | 152 ms |
| lut | 12 ms | 12 ms |
| speedup | 10.42x | **12.7x** |

**The LUT result is the tightest cross-machine match in this whole benchmark set.** The `lut`
version lands at exactly 12 ms on both an Apple M5 and a Zen 5 laptop chip — unsurprising, since a
16-byte table pinned in L1 makes the lookup latency (1–4 cycles either way) essentially
microarchitecture-independent at this scale, and the loop is otherwise just a load + store.

**Branchy is worse on Ryzen (152 ms vs 125 ms), so the speedup is actually larger here (12.7x vs
10.42x)** — the opposite direction from the branch_free example, where GCC's `-fno-if-conversion`
made the *branchless* path slower rather than the branchy path. Here the mechanism is different:
`lut_branch`'s branchy path is a ternary-style conditional (`nibble < 10 ? ... : ...`) forced back
into a real branch by the same flag, and GCC's branchy codegen for this pattern is apparently less
efficient on this run than Apple Clang's — 18.1 cycles/byte here vs an implied ~14.9 cycles/byte
on M5 (matching the M5 doc's own reported cpe of 14.9), even before considering misprediction cost.

**Both machines confirm the core thesis**: when the branch outcome is genuinely unpredictable
(hex digit vs. letter, ~37.5% either way) and the table is small enough to live in L1, trading the
branch for a memory load is a decisive win — double digits either way, on both ISAs.

---

## Results across optimization levels

Rebuilding at `-O0`–`-O3` (`-fno-if-conversion` kept throughout) reveals a flip that doesn't show
up in the `-O1` numbers above:

| level | branchy | lut |
|---|---:|---:|
| O0 | 1.00x (201 ms) | 5.03x (40 ms) |
| O1 | 1.00x (154 ms) | 12.83x (12 ms) |
| O2 | 1.00x (148 ms) | 12.33x (12 ms) |
| O3 | 1.00x (**4 ms**) | **0.33x** (12 ms) — inverted |

(speedup relative to branchy at the same level)

**At `-O3`, `lut` becomes the *slower* of the two.** `encode_branchy` drops from 148 ms to 4 ms —
faster than the hand-optimized `lut` version, which stays flat at 12 ms across O1–O3.
`objdump` explains it: at `-O3`, GCC's vectorizer auto-vectorizes `encode_branchy`'s ternary into a
16-bytes-per-iteration branch-free SIMD kernel (`movdqu` → `psubusb`/`pcmpeqb`/`pandn`/`por`, then
`punpcklbw`/`punpckhbw` to interleave nibble pairs) — essentially reinventing the arithmetic
bitmask trick from `branch_free.cpp`, but for four bits at a time across a whole vector register,
entirely on its own and despite `-fno-if-conversion`. Meanwhile `encode_lut` never picks up any
packed instructions at any `-O` level — table-lookup-by-index doesn't auto-vectorize on this
target (no AVX2 gather is emitted), so it stays pinned at scalar `movzx` loads throughout, at the
same ~12 ms regardless of optimization level.

**This means the example's headline conclusion ("LUT beats branch, 12.7x") only holds through
`-O2`.** At `-O3`, the compiler-generated SIMD replacement for the branch beats the hand-written
table by 3x. The general lesson underneath
still holds — eliminate the *unpredictable branch* — but which branch-free technique wins (SIMD
arithmetic vs. table lookup) depends on how amenable the specific computation is to
autovectorization, and a human-chosen LUT can lose to what the compiler does automatically once
it's allowed to.

---

## Key takeaways (Ryzen-specific)

1. **L1-resident LUTs are as fast on Zen 5 as on Apple M5** — 12 ms on both machines for this
   workload. A cache-resident table lookup is about as portable a win as branch elimination gets.

2. **The branchy baseline is compiler-codegen-sensitive, not just hardware-sensitive** — GCC under
   `-fno-if-conversion` produces a slower branchy loop on this x86 build than Apple Clang does on
   M5, inflating the measured speedup (12.7x vs 10.42x) beyond what a pure hardware comparison
   would show. As with [branch_free_ryzen.md](branch_free_ryzen.md), treat cross-toolchain speedup
   ratios as directional, not a precise ISA comparison.
