// Loop Interchange Example: Matrix-Vector Multiplication
//
// Computes y = A * x where A is M×N (row-major), x is N×1, y is M×1.
//
// Two loop orderings produce identical results but very different performance:
//
//   j,i order (column access of A):
//     for j: for i: y[i] += A[i][j] * x[j]
//     The inner loop increments i, stepping through column j of A.
//     In row-major storage, A[i][j] = A[i*N + j], so consecutive i values
//     are N doubles apart — stride N*8 bytes per step.  For N=4096 that is
//     32 KB per step: every access is a cache miss.
//
//   i,j order (row access of A):
//     for i: for j: y[i] += A[i][j] * x[j]
//     The inner loop increments j, scanning row i of A sequentially.
//     Stride is 8 bytes — one cache line covers 8 elements.  x[j] is also
//     accessed sequentially and fits entirely in L1 (N*8 = 32 KB < 64 KB L1).
//
// Loop interchange transforms j,i -> i,j, converting stride-N column
// access into stride-1 row access.  No algorithmic change; pure reordering.
//
// Build:
//   g++ -O1 -o interchange_O1 loop_interchange.cpp && ./interchange_O1

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
using namespace std::chrono;

static const int M    = 4096;   // matrix rows
static const int N    = 4096;   // matrix cols  (A = 4096x4096 x 8B = 128 MB)
static const int RUNS = 5;

static const int L1_BYTES = 65536;  // 64 KB

template <typename T>
static void sink(T const& val) { asm volatile("" : : "m"(val) : "memory"); }

template <typename Func>
long long bench(Func f) {
    long long best = LLONG_MAX;
    for (int r = 0; r < RUNS; r++) {
        asm volatile("" ::: "memory");
        auto t0 = high_resolution_clock::now();
        f();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration_cast<milliseconds>(t1 - t0).count());
    }
    return best;
}

void report(const char* title,
            const char* a_label, long long a_ms,
            const char* b_label, long long b_ms) {
    double speedup = (double)a_ms / max(1LL, b_ms);
    cout << "\n" << string(64, '-') << "\n";
    cout << title << "\n";
    cout << string(64, '-') << "\n";
    cout << "  BEFORE  " << left  << setw(42) << a_label
         << right << setw(5) << a_ms << " ms\n";
    cout << "  AFTER   " << left  << setw(42) << b_label
         << right << setw(5) << b_ms  << " ms\n";
    cout << "  Speedup: " << fixed << setprecision(2) << speedup << "x\n";
}

using Matrix = vector<double>;
using Vec    = vector<double>;

inline double& at(Matrix& A, int r, int c) { return A[(size_t)r * N + c]; }
inline double  at(const Matrix& A, int r, int c) { return A[(size_t)r * N + c]; }

// ================================================================
// j,i order — column access of A (cache-unfriendly)
//
// Outer loop: j (column index)
// Inner loop: i (row index) — strides N doubles through A
//
// For each j, x[j] is a scalar (one load, fine).
// A[i][j] with i incrementing: step = N * sizeof(double) = 32 KB.
// Every element of A is a cold cache miss.
// ================================================================

void matvec_ji(const Matrix& A, const Vec& x, Vec& y) {
    fill(y.begin(), y.end(), 0.0);
    for (int j = 0; j < N; j++) {
        double xj = x[j];
        for (int i = 0; i < M; i++)
            y[i] += at(A, i, j) * xj;
    }
}

// ================================================================
// i,j order — row access of A (cache-friendly)
//
// Outer loop: i (row index)
// Inner loop: j (column index) — scans row i of A sequentially
//
// y[i] is a scalar accumulator held in a register for the full
// inner loop.  A[i][j] and x[j] are both stride-1 streams that
// fit in L1 (one row of A = 32 KB, x = 32 KB; total = 64 KB = L1).
// ================================================================

void matvec_ij(const Matrix& A, const Vec& x, Vec& y) {
    fill(y.begin(), y.end(), 0.0);
    for (int i = 0; i < M; i++) {
        double acc = 0.0;
        for (int j = 0; j < N; j++)
            acc += at(A, i, j) * x[j];
        y[i] = acc;
    }
}

// ================================================================
// main
// ================================================================

int main() {
    Matrix A(M * N);
    Vec    x(N), y_ji(M), y_ij(M);

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            at(A, i, j) = (i * N + j) * 0.0001;
    for (int j = 0; j < N; j++)
        x[j] = j * 0.001;

    // Correctness check.
    matvec_ji(A, x, y_ji);
    matvec_ij(A, x, y_ij);
    double max_err = 0.0;
    for (int i = 0; i < M; i++)
        max_err = max(max_err, abs(y_ji[i] - y_ij[i]));
    cout << "Correctness check: max_err = " << max_err
         << "  " << (max_err < 1e-6 ? "PASS" : "FAIL") << "\n";

    // Access pattern summary.
    int row_bytes   = N * (int)sizeof(double);
    int x_bytes     = N * (int)sizeof(double);
    int inner_bytes = row_bytes + x_bytes;   // one row A + full x
    cout << "\nAccess pattern:\n";
    cout << "  A row (one inner loop pass): " << row_bytes/1024 << " KB\n";
    cout << "  x vector:                   " << x_bytes/1024   << " KB\n";
    cout << "  i,j inner working set:      " << inner_bytes/1024
         << " KB  " << (inner_bytes <= L1_BYTES ? "(fits L1)" : "(exceeds L1)") << "\n";
    cout << "  j,i inner stride on A:      " << row_bytes/1024
         << " KB per step  (one miss per element)\n";

    auto t_ji = bench([&]{ matvec_ji(A, x, y_ji); sink(y_ji[0]); });
    auto t_ij = bench([&]{ matvec_ij(A, x, y_ij); sink(y_ij[0]); });

    report("Matrix-vector multiply  M=N=4096  (A = 128 MB)",
           "j,i order  (stride-N column access, cache misses)", t_ji,
           "i,j order  (stride-1 row access,    L1 reuse)",     t_ij);

    return 0;
}
