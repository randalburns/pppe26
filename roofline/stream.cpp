// Build: clang++ -std=c++17 -O3 -o stream stream.cpp
// Measures peak DRAM bandwidth via four STREAM kernels.
// Arrays are sized to 8x the M4 SLC (~12 MB) so data comes from DRAM.

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>

using namespace std::chrono;

template <typename T>
static void sink(T const& v) { asm volatile("" : : "m"(v) : "memory"); }

static const size_t N      = 32ULL * 1024 * 1024; // 32M doubles = 256 MB/array
static const int    NTIMES = 5;

int main() {
    std::vector<double> a(N, 1.0), b(N, 2.0), c(N, 0.0);
    double scalar = 3.0;
    double times[4][NTIMES];

    for (int k = 0; k < NTIMES; k++) {
        auto t0 = high_resolution_clock::now();
        for (size_t i = 0; i < N; i++) c[i] = a[i];
        auto t1 = high_resolution_clock::now();
        times[0][k] = duration<double>(t1 - t0).count();
        sink(c[0]);

        t0 = high_resolution_clock::now();
        for (size_t i = 0; i < N; i++) b[i] = scalar * c[i];
        t1 = high_resolution_clock::now();
        times[1][k] = duration<double>(t1 - t0).count();
        sink(b[0]);

        t0 = high_resolution_clock::now();
        for (size_t i = 0; i < N; i++) c[i] = a[i] + b[i];
        t1 = high_resolution_clock::now();
        times[2][k] = duration<double>(t1 - t0).count();
        sink(c[0]);

        t0 = high_resolution_clock::now();
        for (size_t i = 0; i < N; i++) a[i] = b[i] + scalar * c[i];
        t1 = high_resolution_clock::now();
        times[3][k] = duration<double>(t1 - t0).count();
        sink(a[0]);
    }

    const char* names[]  = {"Copy", "Scale", "Add", "Triad"};
    size_t      bytes[]  = {
        2 * N * sizeof(double),   // Copy:  1 read, 1 write
        2 * N * sizeof(double),   // Scale: 1 read, 1 write
        3 * N * sizeof(double),   // Add:   2 reads, 1 write
        3 * N * sizeof(double),   // Triad: 2 reads, 1 write
    };

    double peak = 0.0;
    printf("%-8s %10s\n", "Kernel", "GB/s");
    printf("-------- ----------\n");
    for (int op = 0; op < 4; op++) {
        double best_t = *std::min_element(times[op], times[op] + NTIMES);
        double bw     = bytes[op] / best_t / 1e9;
        printf("%-8s %10.2f\n", names[op], bw);
        peak = std::max(peak, bw);
    }
    printf("\nPEAK_BANDWIDTH=%.2f\n", peak);
    return 0;
}
