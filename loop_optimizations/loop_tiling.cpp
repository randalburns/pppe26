// Loop Tiling Example: Matrix Transpose
//
// Naive transpose reads input row-major (stride 1, prefetchable) but writes
// output column-major (stride N — a different cache line per write).  The
// hardware prefetcher cannot help with stride-N writes: each write targets a
// distinct cache line that is evicted before the next write to that line.
// For a 4096×4096 double matrix (128 MB), the output matrix is reloaded from
// DRAM once per output row — O(N) cold misses.
//
// Tiling fixes both sides simultaneously.  A B×B input tile is read
// sequentially; the transposed B×B output tile writes to B consecutive
// elements of B different rows — but those B rows all fall within the same
// B×B block in the output, so they stay in L1 cache for the duration of the
// tile.  Each cache line is written B times before being evicted.
//
// Working set per tile: 2 × B² × 8 bytes (input tile + output tile).
// L1 = 64 KB  →  B ≤ 64  (B=64 uses exactly 64 KB).
//
// Build:
//   g++ -O1 -o tiling_O1 loop_tiling.cpp && ./tiling_O1

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

static const int N    = 4096;   // matrix dimension (128 MB per matrix)
static const int RUNS = 5;

static const int L1_BYTES = 65536;    // 64 KB
static const int L2_BYTES = 4194304;  // 4 MB

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

using Matrix = vector<double>;

Matrix make_matrix() { return Matrix((size_t)N * N, 0.0); }

inline double& at(Matrix& m, int r, int c) { return m[(size_t)r * N + c]; }
inline double  at(const Matrix& m, int r, int c) { return m[(size_t)r * N + c]; }

void fill_matrix(Matrix& m) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            at(m, i, j) = i * N + j;
}

// ================================================================
// Naive transpose — reads stride-1, writes stride-N
//
// out[j][i] = in[i][j]: as j increments, the write address jumps
// by N doubles (one full row) in the output — a cache miss on every
// write once N exceeds L1/L2 capacity.
// ================================================================

void transpose_naive(const Matrix& in, Matrix& out) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            at(out, j, i) = at(in, i, j);
}

// ================================================================
// Tiled transpose — B×B blocks fit in L1
//
// Within each tile, the B writes to output land in B rows of a B×B
// block.  That block stays in L1 cache for the entire tile, so each
// output cache line is written B times before eviction instead of once.
// ================================================================

void transpose_tiled(const Matrix& in, Matrix& out, int tile) {
    for (int ii = 0; ii < N; ii += tile)
    for (int jj = 0; jj < N; jj += tile) {
        int ilim = min(ii + tile, N);
        int jlim = min(jj + tile, N);
        for (int i = ii; i < ilim; i++)
        for (int j = jj; j < jlim; j++)
            at(out, j, i) = at(in, i, j);
    }
}

// ================================================================
// main
// ================================================================

int main() {
    Matrix in = make_matrix(), out_ref = make_matrix(), out_t = make_matrix();
    fill_matrix(in);

    // Correctness check.
    transpose_naive(in, out_ref);
    transpose_tiled(in, out_t, 32);
    bool ok = true;
    for (size_t k = 0; k < (size_t)N * N && ok; k++)
        if (out_ref[k] != out_t[k]) ok = false;
    cout << "Correctness (tile=32): " << (ok ? "PASS" : "FAIL") << "\n";

    // Naive baseline.
    auto t_naive = bench([&]{
        transpose_naive(in, out_ref);
        sink(out_ref[0]);
    });

    // Tile sweep.
    cout << "\n" << string(64, '-') << "\n";
    cout << "Matrix transpose  " << N << "x" << N
         << "  (" << (size_t)N*N*8/1024/1024 << " MB per matrix)"
         << "  L1=" << L1_BYTES/1024 << " KB\n";
    cout << string(64, '-') << "\n";
    cout << left  << setw(10) << "tile"
         << left  << setw(12) << "work.set"
         << left  << setw(8)  << "L1"
         << right << setw(8)  << "time"
         << right << setw(10) << "speedup" << "\n";
    cout << string(64, '-') << "\n";

    cout << left  << setw(10) << "naive"
         << left  << setw(12) << "-"
         << left  << setw(8)  << "-"
         << right << setw(7)  << t_naive << " ms"
         << right << setw(9)  << "1.00x" << "\n";

    for (int tile : {8, 16, 32, 48, 64, 80, 128, 256, 512}) {
        auto t = bench([&]{
            transpose_tiled(in, out_t, tile);
            sink(out_t[0]);
        });

        int ws      = 2 * tile * tile * (int)sizeof(double);
        bool fits   = ws <= L1_BYTES;
        double speedup = (double)t_naive / max(1LL, t);

        cout << left  << setw(10) << tile
             << left  << setw(12) << (to_string(ws / 1024) + " KB")
             << left  << setw(8)  << (fits ? "yes" : "-")
             << right << setw(7)  << t << " ms"
             << right << setw(8)  << fixed << setprecision(2) << speedup << "x"
             << "\n";
    }

    int row_stride_bytes = N * (int)sizeof(double);
    int cache_line       = 64;
    int l1_sets          = L1_BYTES / (8 * cache_line);  // 8-way associative
    int lines_per_stride = row_stride_bytes / cache_line;
    int set_alias        = lines_per_stride % l1_sets;    // 0 -> full aliasing
    // With full aliasing, at most 8 output rows coexist (one per way).
    cout << "\nOutput row stride : " << row_stride_bytes/1024 << " KB"
            "  (" << row_stride_bytes << " bytes)\n"
         << "L1 sets           : " << l1_sets << "  (64 KB / 8-way / 64-byte line)\n"
         << "Lines per stride  : " << lines_per_stride
         << "  mod " << l1_sets << " = " << set_alias
         << (set_alias == 0 ? "  <- full set aliasing" : "") << "\n"
            "\nWith full aliasing, output rows stride into the same cache sets.\n"
            "8-way associativity allows 8 rows to coexist — B=8 matches this exactly.\n"
            "Tiles larger than B=8 cause evictions on every additional output row.\n"
            "The naive formula (2xB^2x8 <= L1) predicts B=64 but ignores set conflicts.\n"
            "N=4096 is a power of 2, which maximises the aliasing effect.\n";
    return 0;
}
