/*
 * pipeline.tempvar.cpp
 *
 * Splitting a single stalled dependency chain into a load phase and a
 * compute phase, each internally independent, so the CPU can overlap all
 * four elements of an unrolled iteration instead of serializing them.
 *
 * Compile and benchmark:
 *   clang++ -std=c++17 -O2 -o pipeline.tempvar pipeline.tempvar.cpp && ./pipeline.tempvar
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace std;
using namespace std::chrono;

const int ARRAY_SIZE = 100000000;

static void clobber() {
    asm volatile("" ::: "memory");
}

// ============================================================
// EXAMPLE: Temporary Variables to Eliminate Pipeline Stalls
// ============================================================

// SLOW: Single accumulator updated with a multi-step computation each element.
// Every step reads x immediately after writing it — a RAW (Read After Write) hazard.
// The pipeline stalls waiting for each result before the next operation can start.
long long noTempVars(vector<int>& data) {
    long long x = 1;
    for (int i = 0; i < ARRAY_SIZE; i += 4) {
        x = (x + data[i])     * 3 ^ (x >> 2);   // stall: reads x just written
        x = (x + data[i + 1]) * 3 ^ (x >> 2);   // stall: depends on line above
        x = (x + data[i + 2]) * 3 ^ (x >> 2);   // stall: depends on line above
        x = (x + data[i + 3]) * 3 ^ (x >> 2);   // stall: depends on line above
    }
    return x;
}

// FAST: Temporaries split each loop body into two fully independent phases.
//
// LOAD PHASE  — t0..t3 are computed from x0..x3 which haven't changed yet.
//               No dependency exists between t0, t1, t2, t3 — CPU executes all four
//               in parallel (or overlapped) without stalling.
//
// COMPUTE PHASE — each xi uses only its own ti. Again no cross-dependencies.
//               CPU overlaps all four multiply-XOR operations simultaneously.
//
// Result: pipeline stays full; no value is read before its producer finishes.
long long withTempVars(vector<int>& data) {
    long long x0 = 1, x1 = 2, x2 = 3, x3 = 4;

    for (int i = 0; i < ARRAY_SIZE; i += 4) {
        // LOAD PHASE: four independent additions into temporaries
        long long t0 = x0 + data[i];
        long long t1 = x1 + data[i + 1];   // independent of t0
        long long t2 = x2 + data[i + 2];   // independent of t0, t1
        long long t3 = x3 + data[i + 3];   // independent of t0, t1, t2

        // COMPUTE PHASE: four independent multiply-XORs, each uses only its own temp
        x0 = t0 * 3 ^ (t0 >> 2);
        x1 = t1 * 3 ^ (t1 >> 2);
        x2 = t2 * 3 ^ (t2 >> 2);
        x3 = t3 * 3 ^ (t3 >> 2);
    }
    return x0 + x1 + x2 + x3;
}

template <typename Func>
pair<long long, long long> benchmark(Func func, vector<int>& data, int runs = 5) {
    long long minTime = LLONG_MAX;
    long long result = 0;

    for (int r = 0; r < runs; r++) {
        clobber();
        auto start = high_resolution_clock::now();
        clobber();

        result = func(data);

        clobber();
        auto end = high_resolution_clock::now();
        clobber();

        long long t = duration_cast<milliseconds>(end - start).count();
        minTime = min(minTime, t);
    }

    volatile long long dummy = result;
    (void)dummy;

    return {minTime, result};
}

int main() {
    cout << "=== Temporary Variables to Eliminate Pipeline Stalls ===" << endl;
    cout << "Array size: " << ARRAY_SIZE << " elements\n" << endl;

    vector<int> data(ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = (i % 100) + 1;
    }
    clobber();

    // Warmup
    volatile long long w = noTempVars(data);
    w += withTempVars(data);
    (void)w;

    auto [timeA, resA] = benchmark(noTempVars,   data);
    auto [timeB, resB] = benchmark(withTempVars,  data);

    cout << "Without temporaries (stalled):  " << setw(5) << timeA << " ms" << endl;
    cout << "With temporaries (pipelined):   " << setw(5) << timeB << " ms" << endl;
    cout << "Speedup: " << fixed << setprecision(2)
         << (double)timeA / max(1LL, timeB) << "x" << endl;


    volatile long long checksum = resA + resB;
    cout << "(Checksum: " << checksum << ")" << endl;

    return 0;
}
