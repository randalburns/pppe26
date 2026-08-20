# Out-of-Order Execution — AMD Ryzen AI 9 HX 370 (Zen 5)

Rerun of [out_of_order.cpp](out_of_order.cpp) on x86-64. See
[out_of_order.md](out_of_order.md) for the Apple M5 baseline this compares against.

**Machine.** AMD Ryzen AI 9 HX 370 ("Strix Point" mobile, 12C/24T), Ubuntu 24.04, GCC 13.3.0.
L1d 48 KB/core, L2 1 MB/core, L3 24 MB. Governor `powersave` / `amd-pstate-epp` at
`balance_performance` (not root, so not changed for these runs).

**Method.** All runs pinned to CPU 0 with `taskset -c 0` and taken warm — see *Two things this
machine forces you to control for* below. Best of 5 internal runs; the table is stable to ±1 ms
across five repetitions.

---

## Build

One portability fix was needed, the same one already applied across `branch_optimizations/`:
`std::min(best, duration_cast<milliseconds>(...).count())` fails to deduce a common type under
libstdc++ on LP64 Linux, where `milliseconds::rep` is `long` but `best` is `long long`.

```cpp
best = min(best, (long long)duration_cast<milliseconds>(t1 - t0).count());
```

After that, the doc's build line works unchanged:

```bash
g++ -O1 -o ooo_O1 out_of_order.cpp && ./ooo_O1
```

---

## Two things this machine forces you to control for

**1. It is a hybrid CPU.** The HX 370 has 4 Zen 5 cores (CPUs 0–3, 5.16 GHz max) and 8 Zen 5c
dense cores (CPUs 4–11, 3.29 GHz max), plus SMT siblings at 12–23. Which core you land on changes
the answer substantially:

| | serial | 2 acc | 4 acc | 8 acc |
|---|---:|---:|---:|---:|
| Zen 5 P-core (`taskset -c 0`) | 41 ms | 20 ms | 14 ms | 14 ms |
| Zen 5c dense core (`taskset -c 4`) | 63 ms | 32 ms | 17 ms | 15 ms |
| unpinned | 40–77 ms | 20–44 ms | 13–23 ms | 13–17 ms |

The M5 has a P/E split too, but the Apple doc never pinned. Unpinned numbers here swing by nearly
2x run to run. **Every number below is pinned to CPU 0.**

**2. The first run is cold.** First touch of the 512 MB array costs page faults; run 1 reports
57–87 ms for the serial case and runs 2–5 settle at 40–41 ms. Numbers below are warm.

---

## Measured clock

The source hardcodes `CPU_GHZ = 4.0` (an Apple M5 figure), so the program's printed `cycles/elem`
and `CPI` columns are wrong on this machine. The real clock was measured with a dependent chain of
integer `add`s (latency exactly 1 cycle on any x86 core):

```
2e9 dependent adds in 0.560 s  ->  3.57 GHz    (Zen 5 P-core)
2e9 dependent adds in 0.858 s  ->  2.33 GHz    (Zen 5c dense core)
```

So the P core sustains ~3.5–3.6 GHz under this policy, not its 5.16 GHz max and not the 4.0 GHz
the source assumes. **The tables below recompute cycles/elem and CPI at a measured 3.5 GHz.**
Speedup ratios are unaffected — both sides scale with the clock.

Cross-check, using the same technique on a chain of `addsd`:

```
5e8 dependent addsd in 0.420 s -> 0.84 ns/add = 3.00 cycles/add at 3.57 GHz
```

**Zen 5's FADD latency is 3 cycles — identical to the M-series value the example is built around.**
That a clean integer 3.00 falls out also cross-validates the clock calibration. The example's
`FADD_LATENCY = 3` and its `K_min = 3` prediction therefore carry over to Zen 5 unchanged.

---

## Instruction counts differ from ARM64

The source's `INSNS[] = {4.00, 3.50, 2.25, 2.00}` was read off ARM64 assembly. The x86 inner loops
(`g++ -O1 -S`) are shorter, because x86 folds the load into the add — `addsd (%rax), %xmm0` is one
instruction where ARM64 needs a separate `ldr` plus `fadd`:

```asm
.L3:                              ; sum_serial — 4 insns / 1 elem
        addsd   (%rax), %xmm0     ; load+add folded into one instruction
        addq    $8, %rax
        cmpq    %rdx, %rax
        jne     .L3

.L16:                             ; sum_4acc — 7 insns / 4 elems
        addsd   (%rax), %xmm1
        addsd   8(%rax), %xmm3
        addsd   16(%rax), %xmm2
        addsd   24(%rax), %xmm0
        addq    $32, %rax
        cmpq    %rcx, %rax
        jne     .L16
```

| version | ARM64 insns/elem | x86 insns/elem |
|---------|-----------------:|---------------:|
| serial | 4.00 | 4.00 |
| 2 accumulators | 3.50 | 2.50 |
| 4 accumulators | 2.25 | 1.75 |
| 8 accumulators | 2.00 | 1.375 |

CPI below uses the x86 counts. Note this makes x86 CPI look *worse* for the same work — fewer
instructions doing the same job means each one absorbs more of the stall.

---

## Results (Zen 5 P-core, N=64M doubles, 512 MB, `-O1`)

| version | time | cycles/elem | CPI | speedup |
|---------|-----:|------------:|----:|--------:|
| serial (1 accumulator) | 40 ms | 2.09 | 0.52 | 1.00x |
| 2 accumulators | 20 ms | 1.04 | 0.42 | 2.00x |
| 4 accumulators | 13 ms | 0.68 | 0.39 | **3.08x** |
| 8 accumulators | 13 ms | 0.68 | 0.49 | 3.08x |

*cycles/elem and CPI computed at the measured 3.5 GHz, using x86 instruction counts.*

### Side by side with Apple M5

| version | M5 | Zen 5 | M5 speedup | Zen 5 speedup |
|---------|---:|------:|-----------:|--------------:|
| serial | 38 ms | 40 ms | 1.00x | 1.00x |
| 2 acc | 17 ms | 20 ms | 2.24x | 2.00x |
| 4 acc | 8 ms | 13 ms | 4.75x | 3.08x |
| 8 acc | 7 ms | 13 ms | 5.43x | 3.08x |

---

## Analysis

**The mechanism reproduces exactly; the ceiling does not.**

1 → 2 accumulators gives a clean 2.00x, and 2 → 4 adds another 1.54x. The loop-carried dependency
chain is the bottleneck on Zen 5 just as it is on M5, and independent accumulators break it. With
FADD latency measured at 3 cycles and the serial loop measured at 2.09 cycles/elem... note that
serial is running *faster* than the 3-cycle latency floor would allow for a pure dependent chain.
That is the memory system, not the FPU: at 40 ms for 512 MB the serial loop is only pulling
12.5 GiB/s, and the dependent-chain stall and the DRAM latency overlap.

**Where Zen 5 diverges: the memory wall arrives two steps earlier.**

The M5 keeps improving out to K=8 (5.43x). Zen 5 flatlines at K=4 — 4 and 8 accumulators are
identical at 13 ms. That 13 ms for 512 MB is **~38.5 GiB/s**, and no accumulator count gets below it:

```
K=4:  13 ms  ->  38.5 GiB/s
K=8:  13 ms  ->  38.5 GiB/s
```

This is consistent with the single-core streaming limit of this part. A lone core can only keep so
many cache-line fills in flight, and on LPDDR5x with one thread that ceiling lands near 40 GiB/s —
well under the package's theoretical ~120 GB/s, which needs several cores to reach. The M5's
unified memory sustains ~100 GB/s to a single core, so its arithmetic ceiling is still visible at
K=8 where Zen 5's is already buried under the bandwidth floor.

So the headline speedup is 3.08x on Zen 5 vs 4.75x on M5, and the difference is **entirely a
memory-bandwidth story, not an ILP story.** Both chips break the dependency chain equally well;
Zen 5 just runs out of memory to feed it sooner.

**CPI is lowest at K=4, then rises at K=8.** 0.39 → 0.49. Since time is unchanged, the rise is
arithmetic: K=8 does the same work in fewer instructions (1.375/elem vs 1.75), so the same cycles
divided by fewer instructions gives a higher CPI. Once you are bandwidth-bound, CPI stops being a
useful figure of merit — it measures how tersely you encoded the waiting.

---

## Correction: the `-O2` note in the source is Apple-specific

Both the source and [out_of_order.md](out_of_order.md) state that `-O2` auto-vectorizes these
loops and masks the effect. **That is not true for GCC on x86.** Checking the generated code:

| flags | sum_serial inner loop |
|-------|----------------------|
| `-O2` | `addsd` only — scalar |
| `-O3` | `addsd` only — scalar |
| `-O3 -ffast-math` | `addpd` — vectorized |

GCC will not vectorize a floating-point reduction without `-ffast-math`, because FP addition is not
associative and vectorizing reorders the sum. Apple Clang is willing to do it at `-O2`; GCC is not.

`sum_4acc` *does* pick up 2 `addpd` at `-O2` — pairing two independent accumulators into one vector
register is legal without reassociation, since it reorders nothing within a chain.

Practical consequence: on this toolchain `-O1` is not actually required to observe the effect. It
is still the right flag for comparing against the M5 numbers.

---

## Key takeaways

1. **The ILP lesson is architecture-independent.** Breaking a loop-carried dependency with
   independent accumulators works identically on Zen 5 and M5, and for the same reason. Zen 5's
   measured 3-cycle FADD latency means even the `K_min = 3` prediction transfers unchanged.

2. **The ceiling is set by memory, and memory is where the two chips differ.** Zen 5 saturates at
   K=4 / ~38.5 GiB/s; M5 keeps scaling to K=8. Same optimization, different stopping point.

3. **On a hybrid CPU, pin the benchmark.** A Zen 5c core reports 63 ms where a Zen 5 core reports
   41 ms. Unpinned runs vary ~2x. This is a measurement-methodology requirement on any
   modern hybrid part, not a Zen 5 quirk.

4. **Do not trust a hardcoded clock constant.** `CPU_GHZ = 4.0` overstates the derived cycle counts
   by ~14% here; the core actually sustains ~3.5 GHz under the default governor and never
   approaches its 5.16 GHz sticker. Measure the clock with a known-latency dependent chain.

5. **Compiler claims do not port.** The "`-O2` vectorizes this away" warning is true of Apple Clang
   and false of GCC/x86 without `-ffast-math`.
