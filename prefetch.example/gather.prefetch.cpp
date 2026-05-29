// Compile: clang++ -O2 -fno-tree-vectorize -std=c++20 -o gather.prefetch gather.prefetch.cpp
//
// Replicates the random-access benchmark from:
//   https://johnnysswlab.com/the-pros-and-cons-of-explicit-software-prefetching/
//
// Scenario: look up random keys in a large table (like a hash map).
// The hardware prefetcher handles the sequential index walk but cannot
// predict data[idx[i]] — every lookup is a DRAM miss once the table
// exceeds L3 cache.
//
// Apple M-series L3: 8–48 MB depending on chip.
// Data array here:   256 MB  → every lookup goes to DRAM.
//
// Two software-prefetch techniques from the blog:
//
//   CONSTANT-OFFSET  prefetch data[idx[i+DIST]] DIST iterations ahead.
//                    Works when  DIST × iteration_cycles ≥ DRAM latency.
//                    Apple Silicon: ~100 ns × ~3.2 GHz ≈ 320 cycles.
//                    At ~5 cycles/iteration (prefetched): DIST ≥ 64.
//
//   BATCH            Issue BATCH prefetches in a burst LOOKAHEAD batches
//                    ahead. Gives memory-level parallelism: the controller
//                    services all BATCH outstanding requests concurrently.
//                    Effective MLP = BATCH; latency budget = BATCH×LOOKAHEAD.
//
// The blog reported 1.55× speedup on ARM at 256 MB. Apple Silicon's unified
// memory is lower-latency than the Cortex-A78AE they used, so the absolute
// gap is smaller, but the speedup is still clearly measurable.
//
// ── Tuning knobs ─────────────────────────────────────────────────────────────
//   DIST      — constant-offset lookahead (iterations)
//   BATCH     — requests per burst
//   LOOKAHEAD — bursts ahead to prefetch  (effective distance = BATCH×LOOKAHEAD)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>

static constexpr int DATA_N    = 1 << 26;  // 64 M ints = 256 MB
static constexpr int QUERY_N   = 1 << 22;  //  4 M random lookups (idx = 16 MB)
static constexpr int RUNS      = 5;

static constexpr int DIST      = 64;  // constant-offset lookahead
static constexpr int BATCH     = 16;  // requests per burst
static constexpr int LOOKAHEAD =  4;  // bursts ahead (total distance = BATCH×LOOKAHEAD = 64)

// ─────────────────────────────────────────────────────────────────────────────
// Baseline: hardware cannot predict random data accesses — every lookup stalls
// ─────────────────────────────────────────────────────────────────────────────
__attribute__((noinline))
long long gather_plain(const int* __restrict__ data,
                       const int* __restrict__ idx, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i)
        s += data[idx[i]];
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constant-offset prefetch (blog technique 1)
//
// While computing element i, issue a hint for element i+DIST.
// The idx array is sequential so the HW prefetcher has idx[i+DIST] ready;
// we use its value to prefetch the unpredictable data address ahead of time.
// ─────────────────────────────────────────────────────────────────────────────
__attribute__((noinline))
long long gather_prefetch_simple(const int* __restrict__ data,
                                 const int* __restrict__ idx, int n) {
    long long s = 0;
    const int limit = n - DIST;
    for (int i = 0; i < limit; ++i) {
        __builtin_prefetch(&data[idx[i + DIST]], 0, 1); // rw=0 (read), locality=1 (L2)
        s += data[idx[i]];
    }
    for (int i = limit; i < n; ++i)
        s += data[idx[i]];
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
// Batch prefetch (blog technique 2)
//
// Prefetch an entire batch of BATCH addresses at once, LOOKAHEAD batches
// ahead of the current computation. The memory controller sees BATCH
// concurrent requests and pipelines them — true memory-level parallelism.
// Latency budget = BATCH × LOOKAHEAD × cycles_per_element.
// ─────────────────────────────────────────────────────────────────────────────
__attribute__((noinline))
long long gather_prefetch_batch(const int* __restrict__ data,
                                const int* __restrict__ idx, int n) {
    long long s = 0;
    const int batches = n / BATCH;

    // Warm up: prefetch first LOOKAHEAD batches before the compute loop starts
    for (int b = 0; b < LOOKAHEAD && b < batches; ++b)
        for (int j = 0; j < BATCH; ++j)
            __builtin_prefetch(&data[idx[b * BATCH + j]], 0, 1);

    for (int b = 0; b < batches; ++b) {
        // Prefetch batch (b + LOOKAHEAD) — consumed LOOKAHEAD iterations later
        const int pb = b + LOOKAHEAD;
        if (pb < batches)
            for (int j = 0; j < BATCH; ++j)
                __builtin_prefetch(&data[idx[pb * BATCH + j]], 0, 1);

        // Consume batch b — requested LOOKAHEAD batches ago; should be arriving
        const int cb = b * BATCH;
        for (int j = 0; j < BATCH; ++j)
            s += data[idx[cb + j]];
    }

    for (int i = batches * BATCH; i < n; ++i)
        s += data[idx[i]];
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
// Small-dataset control: same access pattern on 512 KB (fits in L2/L3).
// Explicit prefetch adds no benefit — or even hurts (extra instructions).
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int SMALL_N = 1 << 17;  // 128 K ints = 512 KB

__attribute__((noinline))
long long gather_small_plain(const int* __restrict__ data,
                             const int* __restrict__ idx, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i)
        s += data[idx[i] % SMALL_N];
    return s;
}

__attribute__((noinline))
long long gather_small_prefetch(const int* __restrict__ data,
                                const int* __restrict__ idx, int n) {
    long long s = 0;
    const int limit = n - DIST;
    for (int i = 0; i < limit; ++i) {
        __builtin_prefetch(&data[idx[i + DIST] % SMALL_N], 0, 1);
        s += data[idx[i] % SMALL_N];
    }
    for (int i = limit; i < n; ++i)
        s += data[idx[i] % SMALL_N];
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────

int main() {
    std::mt19937 rng{42};

    std::vector<int> data(DATA_N);
    for (auto& x : data) x = static_cast<int>(rng() & 0xFFFF);

    std::vector<int> idx(QUERY_N);
    std::uniform_int_distribution<int> udist(0, DATA_N - 1);
    for (auto& x : idx) x = udist(rng);

    auto bench = [&](const char* label, auto fn) -> double {
        double best = 1e9;
        long long chk = 0;
        for (int r = 0; r < RUNS; ++r) {
            auto t0 = std::chrono::high_resolution_clock::now();
            volatile long long v = fn();
            auto t1 = std::chrono::high_resolution_clock::now();
            chk = v;
            best = std::min(best, std::chrono::duration<double, std::milli>(t1 - t0).count());
        }
        printf("  %-44s  %7.1f ms  (sum=%lld)\n", label, best, chk);
        return best;
    };

    printf("=== LARGE dataset: 256 MB (exceeds all M-series L3 caches) ===\n");
    printf("    DIST=%d  BATCH=%d  LOOKAHEAD=%d  (effective lookahead=%d elements)\n\n",
           DIST, BATCH, LOOKAHEAD, BATCH * LOOKAHEAD);

    double t_plain  = bench("plain (no prefetch)",
                            [&]{ return gather_plain(data.data(), idx.data(), QUERY_N); });
    double t_simple = bench("constant-offset prefetch (DIST=64)",
                            [&]{ return gather_prefetch_simple(data.data(), idx.data(), QUERY_N); });
    double t_batch  = bench("batch prefetch (BATCH=16, LOOKAHEAD=4)",
                            [&]{ return gather_prefetch_batch(data.data(), idx.data(), QUERY_N); });

    printf("\n  Speedup vs plain:  constant-offset %.2fx    batch %.2fx\n",
           t_plain / t_simple, t_plain / t_batch);

    printf("\n=== SMALL dataset: 512 KB (fits in L2/L3 — HW prefetcher handles it) ===\n\n");

    double ts_plain    = bench("plain (no prefetch)",
                               [&]{ return gather_small_plain(data.data(), idx.data(), QUERY_N); });
    double ts_prefetch = bench("constant-offset prefetch (DIST=64)",
                               [&]{ return gather_small_prefetch(data.data(), idx.data(), QUERY_N); });

    printf("\n  Speedup vs plain:  %.2fx  (near 1.0 expected — no benefit, just overhead)\n",
           ts_plain / ts_prefetch);
}
