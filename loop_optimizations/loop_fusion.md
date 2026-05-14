# Loop Fusion Example

Demonstrates computing mean and variance in two separate loops versus one
fused loop, using the computational formula to eliminate the data dependency
that forces a second pass in the naive version.

## Source

[loop_fusion.cpp](loop_fusion.cpp)

## The problem with two loops

Variance requires mean, and mean requires a full pass over the data.  The
naive implementation is therefore forced into two sequential passes:

```cpp
// Pass 1: mean
double sum = 0.0;
for (int i = 0; i < n; i++) sum += data[i];
double mean = sum / n;

// Pass 2: variance — cannot start until mean is known
double m2 = 0.0;
for (int i = 0; i < n; i++) {
    double d = data[i] - mean;
    m2 += d * d;
}
double variance = m2 / n;
```

With N = 50M ints (200 MB), the array far exceeds L3 cache.  Pass 2 forces a
full cold-cache reload of the data.

## Fused version — computational formula

Uses the identity **Var(X) = E[X²] - E[X]²** to accumulate `sum` and
`sum_sq` in a single loop.  Both mean and variance are derived after the loop
with no extra data reads.

```cpp
double sum = 0.0, sum_sq = 0.0;
for (int i = 0; i < n; i++) {
    double x = data[i];
    sum    += x;
    sum_sq += x * x;
}
double mean     = sum / n;
double variance = (sum_sq / n) - mean * mean;
```

## Build

```bash
g++ -O1 -o fusion_O1 loop_fusion.cpp && ./fusion_O1
```

## Results (Apple M-series, 50M elements)

| Flag | Unfused (2 passes) | Fused (1 pass) | Speedup |
|------|--------------------|----------------|---------|
| `-O1` | 69 ms | 39 ms | **1.77x** |

Theoretical ceiling is 2x (2 reads → 1 read).  The 1.77x result is close;
the gap is the extra `x * x` multiply per element in the fused loop.

## Numerical accuracy

The computational formula accumulates large `x * x` values and subtracts two
large numbers at the end, which can lose precision for datasets with large
values and small variance (catastrophic cancellation).  For this example the
delta between the two methods is ~0.88 on a variance of ~3.6 × 10⁸ — about
2.5 × 10⁻⁹ relative error, acceptable for demonstration purposes.

For production use on large or high-dynamic-range datasets, prefer the
two-pass formula (numerically exact) or Welford's algorithm (stable and
single-pass, but slower due to per-element division).

## Attempted fusion with Welford's algorithm

Before landing on the computational formula we tried Welford's online
algorithm, which is the textbook single-pass approach for computing mean and
variance without knowing the mean in advance.

### How Welford's works

At each step, given a new element `x` and the current count `k`:

```
delta  = x - mean          // deviation from current running mean
mean  += delta / k         // update running mean
delta2 = x - mean          // deviation from *updated* mean
M2    += delta * delta2    // accumulate sum of squared deviations
```

After n elements: `variance = M2 / n`.  The algorithm is numerically stable
because deviations are always computed relative to the current running mean
rather than a fixed estimate computed upfront.

### The implementation

```cpp
Stats compute_fused_welford(const int* data, int n) {
    double mean = 0.0;
    double m2   = 0.0;
    for (int i = 0; i < n; i++) {
        double delta = data[i] - mean;
        mean += delta / (i + 1);
        m2   += delta * (data[i] - mean);
    }
    return {mean, m2 / n};
}
```

### Results

| Version | Time | vs unfused |
|---------|------|------------|
| Unfused (2 passes) | 71 ms | baseline |
| Fused — Welford's  | 176 ms | **0.40x (slower)** |
| Fused — computational formula | 39 ms | **1.77x (faster)** |

### Why it was slower

Welford's is compute-bound, not memory-bound, for three reasons:

**1. Division inside the loop.**  `delta / (i + 1)` executes a floating-point
divide on every single iteration.  Division is ~15–20 cycles on modern
hardware versus ~4–5 cycles for multiply.  With 50M iterations that adds up.

**2. Sequential data dependency.**  Each iteration's `mean` update depends on
the previous iteration's result (`mean += delta / k` uses the previous
`mean` to compute `delta`).  This is a loop-carried dependency that prevents
the CPU from executing multiple iterations in parallel.

The result: Welford's reads the 200 MB array once (good) but processes each
element ~4× more slowly (bad), and the compute overhead outweighs the
memory savings by a factor of 2.5.

### When Welford's is the right choice

Welford's wins when memory bandwidth is *not* the bottleneck — for example,
when the data is already in cache, when n is small, or when numerical
stability matters more than throughput.  For a streaming computation over
data that arrives one element at a time (and cannot be stored for a second
pass), it is also the only option.

## Key takeaway

Loop fusion eliminates redundant memory reads by restructuring what is
accumulated in the loop body.  The obstacle here was a real data dependency
(variance needs mean), which the computational formula resolves by deferring
the derivation of both statistics to after the loop.  The speedup (~1.77x)
approaches the theoretical 2x memory-bandwidth limit.
