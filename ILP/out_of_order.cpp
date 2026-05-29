// Out-of-Order Execution Example: Dependency Chains vs Independent Accumulators
//
// Modern out-of-order (OOO) CPUs scan ahead in the instruction stream,
// find instructions whose inputs are already available, and issue them to
// execution units before earlier instructions have finished — provided no
// data dependency prevents it.  The hardware unit that enables this is the
// reorder buffer (ROB), which tracks in-flight instructions and their
// readiness.
//
// A loop-carried dependency defeats OOO execution:
//
//   Single accumulator — every add reads and writes s:
//     fadd s, s, a[0]   // s not ready until 3 cycles later
//     fadd s, s, a[1]   // must wait for s from the line above
//     fadd s, s, a[2]   // must wait ...
//
//   The ROB sees that fadd[i+1] depends on fadd[i]'s output.  Regardless
//   of how many execution units are available, each add stalls for
//   FADD_LATENCY cycles.  The loop runs at 1 add per FADD_LATENCY cycles.
//
// Independent accumulators break the chain:
//
//   Four accumulators — s0, s1, s2, s3 share no dependencies:
//     fadd s0, s0, a[0]  \
//     fadd s1, s1, a[1]  | ROB issues all four to separate FP units
//     fadd s2, s2, a[2]  | in the same (or back-to-back) cycles
//     fadd s3, s3, a[3]  /
//     fadd s0, s0, a[4]  <- s0's result from 3 adds ago is ready; no stall
//     ...
//
//   With FADD latency = 3 cycles and 1 FP unit issuing 1 add/cycle,
//   4 accumulators exactly hides the latency: a new add to s0 issues every
//   4th iteration, and by then the result from 4 iterations ago (3 cycles
//   ago) is already written.  One add issues every cycle; the pipeline is full.
//
//   Adding a 5th or 6th accumulator provides no further benefit for a single
//   FP unit — the bottleneck shifts from latency to throughput.  If the CPU
//   has multiple FP units (Apple M-series has 4), more accumulators allow
//   multiple adds to issue per cycle until memory bandwidth becomes the limit.
//
// Speedup ceiling from latency alone: FADD_LATENCY x (achieved at K >= latency).
//
// Build:
//   g++ -O1 -o ooo_O1 out_of_order.cpp && ./ooo_O1

#include <iostream>
#include <chrono>
#include <climits>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace std::chrono;

static const int N    = 64 * 1024 * 1024;  // 64M doubles = 512 MB
static const int RUNS = 5;

// Apple M-series: FADD double latency = 3 cycles, 4 FP execution units.
// x86-64 (Skylake+): ADDSD latency = 4 cycles, 2 FP execution units.
static const int FADD_LATENCY = 3;

// CPU frequency for CPI calculation (Apple M4 performance core).
// Adjust: M1/M2 ≈ 3.2 GHz, M3 ≈ 3.7 GHz, M4 ≈ 4.0 GHz.
static const double CPU_GHZ = 4.0;

// Instructions per element in each inner loop (from -O1 assembly):
//   serial: ldr, fadd, subs, b.ne                     = 4.00 / elem
//   2acc:   ldp, fadd×2, add, add, cmp, b.lo  / 2     = 3.50 / elem
//   4acc:   ldp×2, fadd×4, add, cmp, b.lo     / 4     = 2.25 / elem
//   8acc:   ldp×4, fadd×8, add, add, cmp, b.lo / 8    = 2.00 / elem
static const double INSNS[4] = { 4.00, 3.50, 2.25, 2.00 };

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

// ================================================================
// Serial accumulation — single loop-carried dependency chain
//
// Every fadd reads s then writes s.  The OOO engine cannot issue
// fadd[i+1] until fadd[i] commits its result: one add per latency cycle.
// ================================================================

double sum_serial(const double* a, int n) {
    double s = 0.0;
    for (int i = 0; i < n; i++)
        s += a[i];
    return s;
}

// ================================================================
// 2 independent accumulators
//
// s0 and s1 carry no dependency between them.  The ROB issues both
// add-chains in parallel.  Latency of each chain is still 3 cycles,
// but the chains run concurrently: effective throughput doubles.
// ================================================================

double sum_2acc(const double* a, int n) {
    double s0 = 0.0, s1 = 0.0;
    int n2 = n - (n % 2);
    for (int i = 0; i < n2; i += 2) {
        s0 += a[i];
        s1 += a[i + 1];
    }
    for (int i = n2; i < n; i++) s0 += a[i];
    return s0 + s1;
}

// ================================================================
// 4 independent accumulators
//
// With FADD latency = 3 cycles and 1 FP issue port:
//   cycle 0: issue s0 += a[0]
//   cycle 1: issue s1 += a[1]
//   cycle 2: issue s2 += a[2]
//   cycle 3: issue s3 += a[3]
//            s0 += a[0] result is ready (latency = 3 done)
//   cycle 3: issue s0 += a[4]   <- no stall
//   ...
// One new fadd issues every cycle; no stall ever occurs.  This is the
// minimum accumulator count to saturate a single FP unit at 3-cycle latency.
// ================================================================

double sum_4acc(const double* a, int n) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    int n4 = n - (n % 4);
    for (int i = 0; i < n4; i += 4) {
        s0 += a[i];
        s1 += a[i + 1];
        s2 += a[i + 2];
        s3 += a[i + 3];
    }
    for (int i = n4; i < n; i++) s0 += a[i];
    return s0 + s1 + s2 + s3;
}

// ================================================================
// 8 independent accumulators
//
// Apple M-series has 4 FP execution units.  With 4 units and 3-cycle
// latency, the fully-pipelined throughput needs 4*3 = 12 independent
// streams to saturate all units.  8 accumulators may provide additional
// gain over 4 on wide-issue machines; beyond that memory bandwidth
// (512 MB / memory_bw) becomes the floor.
// ================================================================

double sum_8acc(const double* a, int n) {
    double s0=0, s1=0, s2=0, s3=0, s4=0, s5=0, s6=0, s7=0;
    int n8 = n - (n % 8);
    for (int i = 0; i < n8; i += 8) {
        s0 += a[i];
        s1 += a[i + 1];
        s2 += a[i + 2];
        s3 += a[i + 3];
        s4 += a[i + 4];
        s5 += a[i + 5];
        s6 += a[i + 6];
        s7 += a[i + 7];
    }
    for (int i = n8; i < n; i++) s0 += a[i];
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
}

// ================================================================
// main
// ================================================================

int main() {
    vector<double> a(N);
    for (int i = 0; i < N; i++)
        a[i] = (i % 1000) * 0.001 + 1.0;   // non-trivial values

    // Correctness check.
    double r1 = sum_serial(a.data(), N);
    double r2 = sum_2acc  (a.data(), N);
    double r4 = sum_4acc  (a.data(), N);
    double r8 = sum_8acc  (a.data(), N);
    double max_err = max({abs(r1-r2), abs(r1-r4), abs(r1-r8)});
    cout << "Correctness check: max_err = " << scientific << max_err
         << "  " << (max_err < 1.0 ? "PASS" : "FAIL") << "\n" << defaultfloat;

    long long mb = (long long)N * sizeof(double) / (1024 * 1024);
    cout << "\nArray:  " << N / (1024*1024) << "M doubles  (" << mb << " MB)\n";
    cout << "FADD latency (Apple M-series): " << FADD_LATENCY
         << " cycles  →  pipeline-full at K >= " << FADD_LATENCY
         << " accumulators\n";

    auto t1 = bench([&]{ sink(sum_serial(a.data(), N)); });
    auto t2 = bench([&]{ sink(sum_2acc  (a.data(), N)); });
    auto t4 = bench([&]{ sink(sum_4acc  (a.data(), N)); });
    auto t8 = bench([&]{ sink(sum_8acc  (a.data(), N)); });

    long long times[4] = { t1, t2, t4, t8 };
    auto cpe = [&](int k) {   // cycles per element
        return (double)times[k] * CPU_GHZ * 1e6 / (double)N;
    };
    auto cpi = [&](int k) {   // cycles per instruction
        return cpe(k) / INSNS[k];
    };

    cout << "\n" << string(76, '-') << "\n";
    cout << "Array sum  N=64M doubles  512 MB   -O1 (no SIMD)   CPU=" << CPU_GHZ << " GHz\n";
    cout << string(76, '-') << "\n";
    cout << left  << setw(24) << "version"
         << right << setw(8)  << "time"
         << right << setw(12) << "cycles/elem"
         << right << setw(7)  << "CPI"
         << right << setw(10) << "speedup" << "\n";
    cout << string(76, '-') << "\n";

    const char* labels[4] = {
        "serial (1 accumulator)",
        "2 accumulators",
        "4 accumulators",
        "8 accumulators"
    };
    for (int k = 0; k < 4; k++) {
        cout << left  << setw(24) << labels[k]
             << right << setw(7)  << times[k] << " ms"
             << right << setw(11) << fixed << setprecision(2) << cpe(k)
             << right << setw(7)  << fixed << setprecision(2) << cpi(k)
             << right << setw(9)  << fixed << setprecision(2)
             << (double)t1 / max(1LL, times[k]) << "x\n";
    }

    // Correlate: speedup = (CPI_serial / CPI_fast) × (insns_serial / insns_fast)
    double cpi_ratio  = cpi(0) / cpi(2);
    double insn_ratio = INSNS[0] / INSNS[2];
    cout << fixed << setprecision(2)
         << "\nSpeedup (1→4 acc) = CPI ratio × insn reduction\n"
         << "  = (" << cpi(0) << " / " << cpi(2) << ") × "
         << "(" << INSNS[0] << " / " << INSNS[2] << ")"
         << " = " << cpi_ratio << "x × " << insn_ratio << "x"
         << " = " << cpi_ratio * insn_ratio << "x"
         << "  [measured: " << (double)t1/max(1LL,t4) << "x]\n";

    cout << "\nNote: at -O2 the compiler auto-vectorises these loops (SIMD),\n"
            "      masking the ILP effect.  Build with -O1 to see it.\n";

    return 0;
}
