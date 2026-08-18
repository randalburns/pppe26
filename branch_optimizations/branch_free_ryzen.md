# Branch-Free Code Benchmarks — AMD Ryzen AI 9 HX 370

Measured on AMD Ryzen AI 9 HX 370 (Zen 5, "Strix Point" mobile, 12C/24T), Ubuntu, GCC 13.3.0,
N=32M bytes, best of 5 runs. See [branch_free.md](branch_free.md) for the Apple M4 baseline this
compares against.

---

## Build note

The doc's build line requires GCC specifically, since Apple Clang if-converts the `if`-statements
to CSEL at `-O1` before the branchy cost can be observed. On this machine the system `g++` is real
GCC (not an alias for clang, as on macOS), so no compiler substitution was needed — same command as
the doc:

```
g++ -O1 -fno-if-conversion -o branch_free branch_free.cpp
```

Confirmed via `objdump` that `process_branchy` compiles to real `jle`/`jmp` conditional jumps and
`process_ternary` compiles to `cmov`, as expected.

One portability fix was required to compile at all: `std::min(best, duration_cast<milliseconds>(...).count())`
fails to deduce a common type under GCC/libstdc++ on Linux, because `milliseconds::rep` is `long`
while `best` is `long long` (two distinct 64-bit types on LP64 Linux, unlike the single 64-bit
`long`/`long long` overlap this pattern gets away with elsewhere). Fixed with an explicit cast,
matching the same fix already applied in [strength_reduction.cpp](../strength.reduction.example/strength_reduction.cpp)
for the same reason:

```cpp
best = min(best, (long long)duration_cast<milliseconds>(t1 - t0).count());
```

---

## Results (`-O1 -fno-if-conversion`, N=32M bytes; stable within ~10%)

| version | random | sorted | random speedup | sorted speedup |
|---------|-------:|-------:|----------------:|----------------:|
| branchy (3 ifs) | 148 ms | 12 ms | 1.00x | 1.00x |
| ternary (CSEL) | 24 ms | 24 ms | **6.2x** | 0.46x (slower) |
| arith (bitmask) | 28 ms | 28 ms | **5.3x** | 0.39x (slower) |

Measured misprediction overhead (branchy − arith, random): ~120 ms.
Expected overhead (32M × 1.0 × 15 / 4GHz): 126 ms — close agreement, same as M4.

---

## Comparison to Apple M4

| version | M4 random | Ryzen random | M4 sorted | Ryzen sorted |
|---|---:|---:|---:|---:|
| branchy | 123 ms | 148 ms | 13 ms | 12 ms |
| ternary | 14 ms | 24 ms | 14 ms | 24 ms |
| arith | 16 ms | 28 ms | 15 ms | 28 ms |
| **random speedup (branchy→ternary)** | 8.79x | **6.2x** | — | — |

**The qualitative story is identical, the magnitudes are not.** Both machines show the same shape:
branchy collapses on random data and matches branchless on sorted data; branchless is invariant to
data pattern. But the branchless versions are noticeably slower in absolute terms on Ryzen (24–28 ms
vs 14–16 ms), while the branchy-random case is only modestly slower (148 ms vs 123 ms). Net effect:
the win from going branchless is smaller here (6.2x vs 8.79x).

**Why the branchless floor is higher on Ryzen.** `-fno-if-conversion` is a GCC-wide flag — it
doesn't just leave the `if`-statements alone, it also makes GCC's codegen for the *branchless*
`ternary`/`arith` paths less aggressive than what Apple Clang produces at `-O1` (which converts to
CSEL unconditionally, flag or no flag). GCC at `-O1` under this flag emits comparably naive scalar
code throughout, so the 1.7–2x gap between M4 and Ryzen branchless numbers is a
GCC-vs-Clang-codegen difference layered on top of the ARM-vs-x86 difference, not purely a hardware
one. `cpe` (cycles/element, using each doc's own 4 GHz reference) confirms this: ternary computes
to 2.86 cycles/byte on Ryzen vs an implied ~1.67 cycles/byte on M4 (from its 14 ms figure); arith
computes to 3.34 here vs an implied ~1.91 on M4.

**The misprediction penalty itself matches almost exactly.** 126 ms expected vs ~120 ms measured on
Ryzen is the same close fit the M4 doc reports (126 ms expected, 110 ms measured: 123−16≈107, close
enough). This confirms the ≈15-cycle misprediction penalty assumption used in the source (calibrated
for Apple M-series) is *also* a reasonable estimate for Zen 5 at a 4 GHz reference clock — x86
misprediction penalties (commonly cited as 15–20 cycles) land in the same ballpark as Apple's
performance cores here.

**Sorted-branchy is identical (~11–13 ms both machines).** A near-perfectly predicted branch costs
the same on both microarchitectures — unsurprising, since a correctly predicted branch is close to
free on any modern out-of-order core, so this pathway isn't testing hardware differences.

---

## Results across optimization levels

The doc's build line only specifies `-O1`. Rebuilding at `-O0`–`-O3` (`-fno-if-conversion` kept
throughout) shows the `-O1` picture is not representative of higher levels:

| level | branchy | ternary (CSEL) | arith (bitmask) |
|---|---:|---:|---:|
| O0 | 1.00x (185 ms) | 1.28x (145 ms) | 3.78x (49 ms) |
| O1 | 1.00x (147 ms) | **6.12x** (24 ms) | 5.25x (28 ms) |
| O2 | 1.00x (21 ms) | 2.33x (9 ms) | 1.91x (11 ms) |
| O3 | 1.00x (21 ms) | 2.33x (9 ms) | 1.91x (11 ms) |

(speedup relative to branchy at the same level; random data)

**`-fno-if-conversion` only holds up its side of the bargain at `-O0`/`-O1`.** The flag disables
GCC's *scalar* if-conversion RTL pass, but at `-O2` and above GCC's tree-vectorizer runs
independently of that pass and is free to vectorize `process_branchy` itself. `objdump` confirms
it: at `-O2`, `process_branchy` compiles to `pcmpgtd`/`pand` — packed SIMD compare-and-mask, the
exact bitmask trick the `arith` version writes out by hand — despite the flag that was supposed to
force it to stay branchy. That's why branchy collapses from 147 ms to 21 ms going O1→O2: not
because branches got cheaper, but because the compiler silently replaced them with the same
technique the example is trying to demonstrate manually.

**Practical implication:** this example's *intended* comparison (real branches vs. branch-free
code) is only actually being measured at `-O1`. At `-O2`/`-O3` all three versions are branch-free
under the hood, and the remaining gap (branchy 21 ms vs ternary/arith 9–11 ms) reflects
autovectorization overhead/tuning differences between GCC's independently-generated SIMD kernels,
not branch prediction.

---

## Key takeaways (Ryzen-specific)

1. **The core lesson transfers unchanged**: branches are cheap only when predictable, and random
   data is worst-case for any predictor, x86 or ARM.

2. **The size of the branchless win depends on compiler codegen, not just hardware.** Suppressing
   if-conversion with `-fno-if-conversion` also dulls GCC's branchless codegen relative to what
   Apple Clang emits unconditionally at `-O1`, shrinking the measured speedup from ~8.8x to ~6.2x
   even though the misprediction penalty itself is essentially unchanged.

3. **Don't compare `-fno-if-conversion` GCC numbers directly against Apple Clang numbers as a
   hardware benchmark** — the flag has compiler-specific side effects beyond the one intended
   ("only affects `if`-statements"). For an apples-to-apples read of the misprediction penalty
   alone, compare branchy against arith on the *same* build, not across toolchains.
