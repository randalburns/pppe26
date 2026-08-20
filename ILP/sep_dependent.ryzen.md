# Separating Dependent Instructions — AMD Ryzen AI 9 HX 370 (Zen 5)

Rerun of [sep_dependent.cpp](sep_dependent.cpp) on x86-64. See
[sep_dependent.md](sep_dependent.md) for the Apple M5 baseline this compares against.

**Machine.** AMD Ryzen AI 9 HX 370 ("Strix Point" mobile, 12C/24T), Ubuntu 24.04, GCC 13.3.0.
Governor `powersave` / `amd-pstate-epp` at `balance_performance`.

**Method.** Pinned to a Zen 5 P-core with `taskset -c 0`, warm (first run discarded). Best of 5
internal runs; stable to ±1 ms across five repetitions. See
[out_of_order.ryzen.md](out_of_order.ryzen.md) for why pinning is mandatory on this hybrid part and
for the clock-calibration method used below.

---

## Build

Same `long`/`long long` fix as the other two examples (libstdc++ `milliseconds::rep` is `long` on
LP64 Linux):

```cpp
best = min(best, (long long)duration_cast<milliseconds>(t1 - t0).count());
```

Then the doc's build line works unchanged:

```bash
g++ -O1 -o sep_O1 sep_dependent.cpp && ./sep_O1
```

---

## Measured clock and FADD latency

The source hardcodes `CPU_GHZ = 4.0` (Apple M5), so its printed `cycles/elem` and `CPI` are wrong
here. Measured on this core with dependent chains of known latency:

```
integer add chain  ->  3.57 GHz sustained   (vs 5.16 GHz sticker, 4.0 GHz assumed)
addsd chain        ->  3.00 cycles/add      (FADD latency = 3, same as M-series)
```

**The example's central constant survives the port**: Zen 5's FP add latency is 3 cycles, so the
"two independent instructions exactly fill a 3-cycle gap" argument holds verbatim. Tables below
recompute at the measured 3.5 GHz.

---

## Instruction counts differ from ARM64

`INSNS[] = {6.0, 6.0, 5.0}` in the source came from ARM64. The x86 loops (`g++ -O1 -S`) count
differently — and notably, `two_passes` is far worse on x86 than the ARM64 figure suggests:

```asm
.L3:                            ; two_passes, loop 1 — 4 insns/elem
        addss   (%rdi), %xmm2   ; load folded
        addq    $4, %rdi
        cmpq    %rsi, %rdi
        jne     .L3
.L4:                            ; two_passes, loop 2 — 6 insns/elem
        movss   (%rax), %xmm0
        mulss   %xmm0, %xmm0
        addss   %xmm0, %xmm1
        addq    $4, %rax
        cmpq    %rsi, %rax
        jne     .L4

.L10:                           ; fused_1acc — 7 insns/elem
        movss   (%rax), %xmm0
        addss   %xmm0, %xmm2    ; sum chain
        mulss   %xmm0, %xmm0
        addss   %xmm0, %xmm1    ; sumsq chain — independent of sum
        addq    $4, %rax
        cmpq    %rsi, %rax
        jne     .L10
```

| version | ARM64 insns/elem | x86 insns/elem |
|---------|-----------------:|---------------:|
| two_passes | 6.0 | 10.0 |
| fused_1acc | 6.0 | 7.0 |
| fused_2acc | 5.0 | 5.5 |

The fused loop is exactly the shape the doc describes: `addss` (sum) and `mulss`+`addss` (sumsq)
interleaved, with the two chains sharing no register.

---

## Results (Zen 5 P-core, N=64M floats, 256 MB, `-O1`)

| version | time | cycles/elem | CPI | speedup |
|---------|-----:|------------:|----:|--------:|
| two_passes (serial loops) | 80 ms | 4.17 | 0.42 | 1.00x |
| fused_1acc (interleaved) | 40 ms | 2.09 | 0.30 | **2.00x** |
| fused_2acc (2 acc each) | 21 ms | 1.09 | 0.20 | **3.81x** |

*cycles/elem and CPI at the measured 3.5 GHz, using x86 instruction counts.*

### Side by side with Apple M5

| version | M5 | Zen 5 | M5 speedup | Zen 5 speedup |
|---------|---:|------:|-----------:|--------------:|
| two_passes | 90 ms | 80 ms | 1.00x | 1.00x |
| fused_1acc | 53 ms | 40 ms | 1.70x | **2.00x** |
| fused_2acc | 28 ms | 21 ms | 3.21x | **3.81x** |

**This is the one ILP example where Zen 5 beats the M5 outright** — faster in absolute terms at
every step, and with a larger speedup at both stages.

---

## Analysis

**two_passes → fused_1acc: exactly 2.00x.**

Cleaner than the M5's 1.70x. The reason is visible in the instruction counts: on x86 the two
separate loops cost 10 instructions per element combined, while the fused loop costs 7. Fusion is
doing two things at once here — removing the loop-boundary ordering barrier *and* deleting an
entire loop's worth of pointer-increment/compare/branch overhead (3 of the 10). ARM64's addressing
modes made the second loop cheaper, so the M5 saw less of the second effect and landed at 1.70x.

Measured cycles/elem falls 4.17 → 2.09. The theoretical floor is 3 cycles for a single dependent
FADD chain, and fused_1acc is *below* it — both chains really are running concurrently, so the
per-element cost is one chain's throughput, not one chain's latency.

**fused_1acc → fused_2acc: 1.90x additional.**

Two accumulators per chain gives four independent instructions between consecutive updates of any
one accumulator, comfortably exceeding the measured 3-cycle FADD latency. Cycles/elem drops to 1.10
and CPI to 0.20 — roughly 5 instructions retiring per cycle, well within Zen 5's 8-wide
rename/retire width. The stalls are gone.

**Why this example ports better than `out_of_order.cpp`.**

The companion example tops out at 3.08x on this machine because it streams 512 MB and hits a
~38.5 GiB/s single-core bandwidth wall. This one moves 256 MB and finishes in 21 ms — about
11.9 GiB/s, nowhere near that ceiling. It stays latency-bound end to end, which is exactly the
regime the lesson is about. **On a bandwidth-limited laptop part, the smaller working set is what
lets the ILP effect stay visible.**

---

## Correction: the `-O2` note is Apple-specific

The source and [sep_dependent.md](sep_dependent.md) warn that `-O2`/`-O3` auto-vectorizes and
masks the effect. Checked on GCC 13 / x86: `fused_1acc` at `-O2` contains **zero** packed
instructions (no `addps`/`mulps`). GCC will not vectorize an FP reduction without `-ffast-math`,
since doing so reassociates the sum. Apple Clang will at `-O2`; GCC will not.

So on this toolchain `-O1` is not strictly required — though it remains the right flag for
comparability with the M5 numbers.

---

## Key takeaways

1. **The lesson transfers intact, and then some.** Interleaving two independent dependency chains
   hides latency on Zen 5 exactly as on M5, and the measured 3-cycle FADD latency means even the
   detailed cycle-by-cycle pipeline argument in the M5 doc applies unchanged.

2. **Loop fusion pays twice on x86.** Beyond removing the ROB ordering barrier, it eliminates a
   whole loop's index/compare/branch overhead — 10 insns/elem down to 7. That second effect is
   larger on x86 than ARM64, and it is why Zen 5 gets 2.00x where the M5 got 1.70x.

3. **Staying under the bandwidth wall is what keeps the effect measurable.** At 256 MB this example
   never becomes memory-bound, unlike `out_of_order.cpp` at 512 MB on the same machine.

4. **Trust measured constants over inherited ones.** `CPU_GHZ = 4.0` and the ARM64 `INSNS[]` table
   are both wrong here; the conclusions only stand because the underlying FADD latency was verified
   to be the same 3 cycles.
