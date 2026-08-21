// Shared helpers for the multicore ("_mt") companions to the single-thread
// benchmarks in this directory. Kept separate from the single-thread files
// so none of their already-verified kernels needed touching.
#pragma once

#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

// Runs `body(tid, nthreads)` on `nthreads` threads (tid 0..nthreads-1) and
// blocks until all have finished. One call = one trial: for benchmarking,
// wrap this in a best-of-NTIMES loop the same way the single-thread files
// wrap a single kernel call, since threads must run concurrently as a unit
// — there's no meaningful "best of N" *within* one parallel region.
template <typename F>
static void run_on_threads(unsigned nthreads, F body) {
    std::vector<std::thread> pool;
    pool.reserve(nthreads);
    for (unsigned t = 0; t < nthreads; t++) pool.emplace_back(body, t, nthreads);
    for (auto& th : pool) th.join();
}

// Times NTIMES trials of `body` run across `nthreads` threads, returns the
// best (minimum) wall-clock time across trials, same convention as every
// single-thread benchmark's `best_t`.
template <typename F>
static double best_of_mt(unsigned nthreads, int ntimes, F body) {
    double best_t = 1e9;
    for (int k = 0; k < ntimes; k++) {
        auto t0 = std::chrono::high_resolution_clock::now();
        run_on_threads(nthreads, body);
        auto t1 = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(t1 - t0).count();
        if (dt < best_t) best_t = dt;
    }
    return best_t;
}

// Splits [0, n) into `nthreads` contiguous, near-equal ranges and returns
// the [begin, end) for thread `tid`.
static inline void thread_range(size_t n, unsigned tid, unsigned nthreads,
                                 size_t& begin, size_t& end) {
    size_t chunk = n / nthreads, rem = n % nthreads;
    begin = tid * chunk + std::min<size_t>(tid, rem);
    end   = begin + chunk + (tid < rem ? 1 : 0);
}
