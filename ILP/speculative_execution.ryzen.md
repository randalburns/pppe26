# Speculative Execution — AMD Ryzen AI 9 HX 370 (Zen 5)

Rerun of [speculative_execution.cpp](speculative_execution.cpp) on x86-64. See
[speculative_execution.md](speculative_execution.md) for the Apple M5 baseline this compares
against.

**Machine.** AMD Ryzen AI 9 HX 370 ("Strix Point" mobile, 12C/24T), Ubuntu 24.04, GCC 13.3.0.
Governor `powersave` / `amd-pstate-epp` at `balance_performance`.

**Method.** Pinned to a Zen 5 P-core with `taskset -c 0`, warm. Best of 5 internal runs; stable to
±1 ms across five repetitions. See [out_of_order.ryzen.md](out_of_order.ryzen.md) for the hybrid
pinning requirement and the clock-calibration method.

---

## Build

The doc calls for `g++-15`; the system `g++` here is real GCC 13.3.0 (not a clang alias as on
macOS), so the command works as written apart from the same `long`/`long long` fix the other two
examples needed:

```bash
g++ -O1 -fno-if-conversion -o spec_O1 speculative_execution.cpp && ./spec_O1
```

**`-fno-if-conversion` is genuinely required on x86 too.** Without it GCC if-converts
`sum_conditional` to `cmov` and the branch disappears, exactly as Apple Clang does. Verified in the
generated assembly below.

---

## Measured clock

`CPU_GHZ = 4.0` in the source is an Apple M5 figure. Measured here with a dependent integer-add
chain: **3.57 GHz** sustained on the P core, against a 5.16 GHz sticker. Tables below recompute at
a measured 3.5 GHz.

---

## The x86 assembly

`-fno-if-conversion` preserves a real conditional branch, and the branchless version lowers to
`cmov` — structurally the same outcome the M5 doc reports for ARM64 `b.le` / `csel`:

```asm
_Z15sum_conditionalPKii:            ; branchy, -O1 -fno-if-conversion
.L7:
        addq    $4, %rax
        cmpq    %rsi, %rax
        je      .L5
.L8:
        movl    (%rax), %edx
        cmpl    $127, %edx
        jle     .L7                 ; <-- real branch, subject to misprediction
        movslq  %edx, %rdx
        addq    %rdx, %rcx
        jmp     .L7

_Z14sum_branchlessPKii:             ; branchless
.L13:
        movl    (%rax), %ecx
        movslq  %ecx, %rdx
        cmpl    $127, %ecx
        cmovle  %rdi, %rdx          ; <-- conditional move, no branch
        addq    %rdx, %rsi
        addq    $4, %rax
        cmpq    %r8, %rax
        jne     .L13
```

### Instruction counts differ from ARM64

The source hardcodes `INSNS_PER_ELEM = 7.0` from an ARM64 count of 6 (not taken) / 8 (taken). The
x86 loop costs one more on the taken path, because GCC lays the body out with an extra `jmp .L7`
to rejoin the loop tail rather than falling through:

| path | ARM64 | x86 |
|------|------:|----:|
| not taken (`data[i] < 128`) | 6 | 6 |
| taken (`data[i] >= 128`) | 8 | **9** |
| average at 50% taken | 7.0 | **7.5** |
| branchless (constant) | — | 8.0 |

The program's printed CPI is therefore ~7% low for the branchy rows. Corrected below.

Note also that the x86 branchy loop contains *two* branches per element — the data-dependent `jle`
and the loop-back `je`. Only the first is unpredictable; the loop-back is perfectly predicted and
contributes nothing to the effect being measured.

---

## Results (Zen 5 P-core, N=32M ints, 128 MB, threshold=128, `-O1 -fno-if-conversion`)

| version | time | cycles/elem | CPI | speedup |
|---------|-----:|------------:|----:|--------:|
| branchy + shuffled (50% mispredict) | 97 ms | 10.12 | 1.35 | 1.00x |
| branchy + sorted (1 mispredict) | 9 ms | 0.94 | 0.13 | **10.78x** |
| branchless + shuffled (no branch) | 13 ms | 1.36 | 0.17 | **7.46x** |

*cycles/elem and CPI at the measured 3.5 GHz, using x86 instruction counts (7.5 branchy, 8.0
branchless).*

### Side by side with Apple M5

| version | M5 | Zen 5 | M5 speedup | Zen 5 speedup |
|---------|---:|------:|-----------:|--------------:|
| branchy + shuffled | 85 ms | 97 ms | 1.00x | 1.00x |
| branchy + sorted | 8 ms | 9 ms | 10.62x | 10.78x |
| branchless + shuffled | 8 ms | 13 ms | **10.62x** | **7.46x** |

The first two rows reproduce the M5 result almost exactly. The third does not, and that is the
interesting finding.

---

## Measured misprediction penalty

The extra time the shuffled case pays over the sorted case is all mispredict cost:

```
extra = 97 - 9 = 88 ms
mispredicts = 0.5 x 32M = 16.78M
penalty = 88ms x 3.5 GHz / 16.78M = 18.4 cycles per mispredict
```

**~18 cycles**, against the 15 the source assumes. Applying the same arithmetic to the M5 numbers
(77 ms extra at 4 GHz) gives ~18.4 cycles there too — so both chips are near 18, and the source's
15-cycle constant is a mild underestimate on both. The program's printed "expected overhead ~84 ms"
consequently comes in under the measured 88 ms.

---

## Analysis

**Branch misprediction costs the same on both chips.** 10.78x vs 10.62x on the sorted/shuffled
comparison — the two architectures are within 2% of each other on the effect this example exists to
teach. Same code, same data, same instruction count, only the data order differs.

**Where Zen 5 diverges: branchless is not free here.**

On the M5, sorted-branchy and branchless tie at 8 ms. On Zen 5 branchless costs 13 ms against
sorted-branchy's 9 ms — 44% slower. The reason is in the assembly above: `cmovle` sits **inside the
accumulator dependency chain**.

```
movl   (%rax), %ecx     ; load
cmovle %rdi, %rdx       ; must complete before...
addq   %rdx, %rsi       ; ...the accumulator update
```

Every element's `addq` to `%rsi` waits on that element's `cmov`, which waits on that element's
load. The chain is strictly serial, one element deep, and no amount of out-of-order execution can
overlap it — the same loop-carried-dependency problem as `out_of_order.cpp`, reintroduced by the
branchless rewrite.

The sorted branchy version has no such chain. The predictor is right ~100% of the time, so the CPU
speculates straight through the branch and issues loads and adds from many iterations concurrently.
Its 0.13 CPI reflects roughly 8 instructions retiring per cycle across several in-flight
iterations — near Zen 5's 8-wide retire width.

So on Zen 5 the ranking is **sorted-branchy > branchless > shuffled-branchy**, where on M5 the top
two tie. Branchless still wins by 7.5x when the data is unpredictable, which is the point of the
technique — but it is no longer free when the data *is* predictable.

**This sharpens the lesson rather than weakening it.** Branchless code does not make a loop fast;
it trades an unpredictable control dependency for a guaranteed data dependency. That trade is
strongly positive on random data and mildly negative on sorted data. The M5's wider FP/integer
issue happens to hide the data dependency; Zen 5 does not, which makes the trade visible.

---

## Key takeaways

1. **The misprediction effect reproduces almost exactly.** 10.78x on Zen 5 vs 10.62x on M5. Data
   order alone, with identical code and identical instruction counts, is worth an order of
   magnitude on both architectures.

2. **The measured penalty is ~18 cycles, not 15.** Same on both chips, by the same arithmetic. The
   source's constant should be 18 if the goal is to match measurement.

3. **Branchless is a trade, not a free win — and Zen 5 shows the cost.** `cmov` moves the
   conditional into the data dependency chain. Against unpredictable data that is a large win
   (7.5x); against predictable data it is a 44% loss. On the M5 the loss is invisible.

4. **GCC on x86 if-converts just like Apple Clang.** `-fno-if-conversion` is required on both
   platforms to observe the raw hardware behavior, and both compilers reach for a conditional move
   (`cmov` / `csel`) for the branchless formulation unprompted.

5. **Speedup = CPI ratio still holds** for the two branchy rows, where instruction count is
   identical: the CPI ratio is 10.8x against a measured 10.78x — exact, since equal instruction
   counts make the CPI ratio and the time ratio the same number. It does *not* hold for the branchless
   row, which executes a different instruction mix (8.0 vs 7.5 insns/elem).
