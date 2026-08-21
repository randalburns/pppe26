// Build: clang++ -std=c++17 -O3 -march=native -o stream_mt stream_mt.cpp
// Multicore companion to stream.cpp: measures AGGREGATE DRAM bandwidth when
// every core runs the same STREAM kernels concurrently, instead of one
// core's bandwidth in isolation. This is the ceiling multicore roofline
// plots actually need — the memory controller is a shared resource, so it's
// not simply (single-core bandwidth x core count).
//
// Each thread gets its own private a/b/c arrays (no false sharing, no
// coherence traffic between threads) sized the same as stream.cpp's per-core
// working set (well beyond the M5 SLC), so every thread is genuinely
// DRAM-bound on its own, same as the single-thread version.

#include "mt_common.hpp"
#include <algorithm>
#include <cstdio>
#include <thread>
#include <vector>

template <typename T>
static void sink(T const& v) { asm volatile("" : : "m"(v) : "memory"); }

static const size_t N      = 4ULL * 1024 * 1024; // 4M doubles = 32 MB/array/thread
static const int    NTIMES = 5;

struct ThreadBW { double copy, scale, add, triad; };

int main() {
    unsigned nthreads = std::thread::hardware_concurrency();
    std::vector<std::vector<double>> a(nthreads), b(nthreads), c(nthreads);
    for (unsigned t = 0; t < nthreads; t++) {
        a[t].assign(N, 1.0);
        b[t].assign(N, 2.0);
        c[t].assign(N, 0.0);
    }
    std::vector<ThreadBW> per_thread(nthreads);

    double best_total_bytes_per_sec[4] = {0, 0, 0, 0};
    for (int k = 0; k < NTIMES; k++) {
        double t_copy, t_scale, t_add, t_triad;
        double times[4];
        for (int op = 0; op < 4; op++) {
            double dt = best_of_mt(nthreads, 1, [&](unsigned tid, unsigned) {
                double* A = a[tid].data(); double* B = b[tid].data(); double* C = c[tid].data();
                double scalar = 3.0;
                switch (op) {
                    case 0: for (size_t i = 0; i < N; i++) C[i] = A[i]; break;
                    case 1: for (size_t i = 0; i < N; i++) B[i] = scalar * C[i]; break;
                    case 2: for (size_t i = 0; i < N; i++) C[i] = A[i] + B[i]; break;
                    case 3: for (size_t i = 0; i < N; i++) A[i] = B[i] + scalar * C[i]; break;
                }
                sink(A[0]); sink(B[0]); sink(C[0]);
            });
            times[op] = dt;
        }
        t_copy = times[0]; t_scale = times[1]; t_add = times[2]; t_triad = times[3];
        size_t bytes_copy_per_thread  = 2 * N * sizeof(double);
        size_t bytes_scale_per_thread = 2 * N * sizeof(double);
        size_t bytes_add_per_thread   = 3 * N * sizeof(double);
        size_t bytes_triad_per_thread = 3 * N * sizeof(double);
        double bw_copy  = (double)(bytes_copy_per_thread  * nthreads) / t_copy  / 1e9;
        double bw_scale = (double)(bytes_scale_per_thread * nthreads) / t_scale / 1e9;
        double bw_add   = (double)(bytes_add_per_thread   * nthreads) / t_add   / 1e9;
        double bw_triad = (double)(bytes_triad_per_thread * nthreads) / t_triad / 1e9;
        best_total_bytes_per_sec[0] = std::max(best_total_bytes_per_sec[0], bw_copy);
        best_total_bytes_per_sec[1] = std::max(best_total_bytes_per_sec[1], bw_scale);
        best_total_bytes_per_sec[2] = std::max(best_total_bytes_per_sec[2], bw_add);
        best_total_bytes_per_sec[3] = std::max(best_total_bytes_per_sec[3], bw_triad);
    }

    const char* names[] = {"Copy", "Scale", "Add", "Triad"};
    double peak = 0.0;
    printf("Threads: %u\n", nthreads);
    printf("%-8s %10s\n", "Kernel", "GB/s (aggregate)");
    printf("-------- -----------------\n");
    for (int op = 0; op < 4; op++) {
        printf("%-8s %10.2f\n", names[op], best_total_bytes_per_sec[op]);
        peak = std::max(peak, best_total_bytes_per_sec[op]);
    }
    printf("\nPEAK_BANDWIDTH_MT=%.2f\n", peak);
    printf("MT_THREADS=%u\n", nthreads);
    return 0;
}
