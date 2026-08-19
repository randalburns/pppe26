// Build: clang++ -std=c++17 -stdlib=libc++ -O2 -pthread -o false_sharing false_sharing.cpp
// Or:    make && make run          (builds -O0/-O1/-O2/-O3 and runs all four)
//
// Note: -O0 makes every case slow and compresses the ratios; -O2 is where the
//       cache-line effect stands out.  The Makefile builds all four levels so
//       you can see how the speedups move with optimization.
//
// Ryzen/Linux notes:
//   * Threads are PINNED to specific logical CPUs.  Without pinning, the
//     scheduler migrates threads between cache domains mid-run and the timings
//     swing by 2-3x from run to run.
//   * The placement sweep exploits the fact that this CPU (Ryzen AI 9 HX 370,
//     Strix Point) has TWO separate L3 domains (CCXs).  False sharing between
//     two cores in the same CCX and between two cores in different CCXs are
//     very different costs: the latter has to cross the Infinity Fabric.

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>
#include <algorithm>
#include <climits>
#include <cstdlib>

#if defined(__linux__)
  #include <pthread.h>
  #include <sched.h>
#endif

using namespace std;
using namespace std::chrono;

static const int       CACHE_LINE = 64;
static long long       ITERS      = 20'000'000LL;

// 8 counters of 8 bytes fill exactly one 64-byte line, so the packed test uses
// 8 threads by default: one writer per slot, all slots on ONE cache line.
static const int COUNTERS_PER_LINE = CACHE_LINE / sizeof(long long);

// Prevent dead-code elimination without forcing a visible store.
template <typename T>
static void sink(T const& val) {
    asm volatile("" : : "m"(val) : "memory");
}

// ============================================================
// TOPOLOGY  (Linux sysfs)
//
// For each logical CPU we record its physical core and its L3
// domain.  On Strix Point the L3 id separates the 4 Zen 5 cores
// (CCX0) from the 8 Zen 5c cores (CCX1).
// ============================================================

struct CpuInfo { int cpu, core, l3; };

static int readIntFile(const string& path, int fallback = -1) {
    ifstream f(path);
    int v;
    return (f >> v) ? v : fallback;
}

struct Topology {
    vector<CpuInfo> cpus;
    vector<int>     firstSiblingOfCore;   // one logical CPU per physical core
    vector<int>     smtSiblingOfCore;     // its SMT partner, or -1

    bool valid() const { return !firstSiblingOfCore.empty(); }

    // Physical cores that live in the same L3 domain as `cpu`.
    vector<int> coresInSameL3(int cpu) const {
        vector<int> out;
        int l3 = l3Of(cpu);
        for (int c : firstSiblingOfCore)
            if (l3Of(c) == l3 && c != cpu) out.push_back(c);
        return out;
    }
    vector<int> coresInOtherL3(int cpu) const {
        vector<int> out;
        int l3 = l3Of(cpu);
        for (int c : firstSiblingOfCore)
            if (l3Of(c) != l3) out.push_back(c);
        return out;
    }
    int l3Of(int cpu) const {
        for (auto const& ci : cpus) if (ci.cpu == cpu) return ci.l3;
        return -1;
    }
    int smtPartner(int cpu) const {
        for (size_t i = 0; i < firstSiblingOfCore.size(); i++)
            if (firstSiblingOfCore[i] == cpu) return smtSiblingOfCore[i];
        return -1;
    }
    int numL3Domains() const {
        vector<int> ids;
        for (int c : firstSiblingOfCore) ids.push_back(l3Of(c));
        sort(ids.begin(), ids.end());
        return (int)(unique(ids.begin(), ids.end()) - ids.begin());
    }
};

static Topology detectTopology() {
    Topology t;
#if defined(__linux__)
    int n = (int)thread::hardware_concurrency();
    for (int i = 0; i < n; i++) {
        string base = "/sys/devices/system/cpu/cpu" + to_string(i);
        int core = readIntFile(base + "/topology/core_id");
        int l3   = readIntFile(base + "/cache/index3/id", 0);
        if (core < 0) return Topology{};          // sysfs unavailable
        t.cpus.push_back({i, core, l3});
    }
    // First logical CPU seen for each physical core; second one is its SMT twin.
    vector<int> seenCores;
    for (auto const& ci : t.cpus) {
        auto it = find(seenCores.begin(), seenCores.end(), ci.core);
        if (it == seenCores.end()) {
            seenCores.push_back(ci.core);
            t.firstSiblingOfCore.push_back(ci.cpu);
            t.smtSiblingOfCore.push_back(-1);
        } else {
            size_t idx = it - seenCores.begin();
            if (t.smtSiblingOfCore[idx] < 0) t.smtSiblingOfCore[idx] = ci.cpu;
        }
    }
#endif
    return t;
}

// Pin the calling thread to one logical CPU.  Returns false if unsupported.
static bool pinSelf(int cpu) {
#if defined(__linux__)
    if (cpu < 0) return false;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    return pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == 0;
#else
    (void)cpu; return false;
#endif
}

// ============================================================
// TIMING
// ============================================================

template <typename Func>
double bench(Func func, int runs = 5) {
    double best = 1e18;
    for (int r = 0; r < runs; r++) {
        auto t0 = high_resolution_clock::now();
        func();
        auto t1 = high_resolution_clock::now();
        best = min(best, duration<double, milli>(t1 - t0).count());
    }
    return best;
}

static void printResult(const string& title,
                        const string& slowLabel, double slowMs,
                        const string& fastLabel, double fastMs) {
    cout << "\n" << string(64, '=') << "\n" << title << "\n"
         << string(64, '-') << "\n"
         << "  " << setw(36) << left << slowLabel
         << ": " << setw(9) << right << fixed << setprecision(1) << slowMs << " ms\n"
         << "  " << setw(36) << left << fastLabel
         << ": " << setw(9) << right << fixed << setprecision(1) << fastMs << " ms\n"
         << "  Speedup: " << fixed << setprecision(2)
         << (slowMs / max(0.001, fastMs)) << "x\n";
}

// ============================================================
// WORKLOAD A: FALSE SHARING  (the problem)
//
// Each thread increments its own element of a packed array.
// With 8 threads: 8 longs x 8 bytes = 64 bytes = ONE cache line.
//
// Threads never touch each other's element, but every write sets
// the line to Modified and invalidates all other cores' copies
// (MESI).  The next core must acquire ownership before it can
// write, so the threads serialize on the line, not on the data.
// ============================================================

static void test_false_sharing(const vector<int>& cpuSet) {
    alignas(CACHE_LINE) volatile long long c[COUNTERS_PER_LINE] = {};
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&c, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            for (long long i = 0; i < ITERS; i++)
                c[t % COUNTERS_PER_LINE]++;
        });
    }
    for (auto& th : threads) th.join();
    sink(c[0]);
}

// ============================================================
// WORKLOAD B: PADDED COUNTERS  (structural fix)
//
// Each counter is padded to 64 bytes so it owns its own cache
// line.  Threads write in parallel with no cross-core
// invalidations.
// ============================================================

struct alignas(CACHE_LINE) PaddedCounter {
    volatile long long value = 0;
    char _pad[CACHE_LINE - sizeof(long long)];
};
static_assert(sizeof(PaddedCounter) == CACHE_LINE, "padding mismatch");

static void test_padded(const vector<int>& cpuSet) {
    vector<PaddedCounter> counters(cpuSet.size());
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&counters, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            for (long long i = 0; i < ITERS; i++)
                counters[t].value++;
        });
    }
    for (auto& th : threads) th.join();
    sink(counters[0].value);
}

// ============================================================
// WORKLOAD C: THREAD-LOCAL ACCUMULATION  (algorithmic fix)
//
// Accumulate in a local (register) variable, then write to
// shared memory exactly once.  Zero cache traffic in the loop.
//
// At -O1 and above the compiler collapses "local++" x ITERS into
// a single assignment and the loop disappears -> ~0 ms.  That is
// correct: the loop has no observable side effect.  At -O0 the
// loop really runs, which is why the -O sweep is interesting.
// ============================================================

static void test_local_accum(const vector<int>& cpuSet) {
    vector<long long> results(cpuSet.size(), 0);
    int nthreads = (int)cpuSet.size();

    vector<thread> threads;
    for (int t = 0; t < nthreads; t++) {
        threads.emplace_back([&results, t, cpu = cpuSet[t]]() {
            pinSelf(cpu);
            long long local = 0;
            for (long long i = 0; i < ITERS; i++)
                local++;                 // stays in a register
            results[t] = local;          // one shared write total
        });
    }
    for (auto& th : threads) th.join();

    volatile long long s = 0;
    for (int t = 0; t < nthreads; t++) s += results[t];
    sink(s);
}

// ============================================================
// PLACEMENT SWEEP  (Ryzen / chiplet specific)  -> reported as TEST 3
//
// Two threads, one contended cache line, several placements.
// Each placement is also measured with the two threads writing
// to SEPARATE lines ("own-line" control).  The control matters:
// Strix Point mixes fast Zen 5 cores (CPUs 0-3) with slower
// Zen 5c cores (CPUs 4-11), so raw ms conflates core speed with
// coherence cost.  The penalty ratio shared/own-line is the
// number to compare across placements.
// ============================================================

// Both threads hammer the SAME cache line (c[0], c[1] are adjacent).
static double bench_pair_shared(int cpuA, int cpuB) {
    return bench([&]{
        alignas(CACHE_LINE) volatile long long c[COUNTERS_PER_LINE] = {};
        int cpus[2] = {cpuA, cpuB};
        vector<thread> threads;
        for (int t = 0; t < 2; t++) {
            threads.emplace_back([&c, t, cpu = cpus[t]]() {
                pinSelf(cpu);
                for (long long i = 0; i < ITERS; i++)
                    c[t]++;
            });
        }
        for (auto& th : threads) th.join();
        sink(c[0]);
    });
}

// Control: same two cores, same work, but one cache line each.
static double bench_pair_ownline(int cpuA, int cpuB) {
    return bench([&]{
        alignas(CACHE_LINE) volatile long long c[2 * COUNTERS_PER_LINE] = {};
        int cpus[2] = {cpuA, cpuB};
        vector<thread> threads;
        for (int t = 0; t < 2; t++) {
            threads.emplace_back([&c, t, cpu = cpus[t]]() {
                pinSelf(cpu);
                int idx = t * COUNTERS_PER_LINE;   // 64 bytes apart
                for (long long i = 0; i < ITERS; i++)
                    c[idx]++;
            });
        }
        for (auto& th : threads) th.join();
        sink(c[0]);
    });
}

int main(int argc, char** argv) {
    if (argc > 1) ITERS = atoll(argv[1]);

    Topology topo = detectTopology();

    // Default: one thread per physical core, up to 8 (one full cache line of
    // counters).  Falls back to unpinned round-robin if sysfs is unavailable.
    vector<int> cpuSet;
    if (topo.valid()) {
        for (int c : topo.firstSiblingOfCore) {
            if ((int)cpuSet.size() >= COUNTERS_PER_LINE) break;
            cpuSet.push_back(c);
        }
    } else {
        int n = max(2, min(COUNTERS_PER_LINE, (int)thread::hardware_concurrency()));
        for (int i = 0; i < n; i++) cpuSet.push_back(-1);   // -1 = do not pin
    }
    int nthreads = (int)cpuSet.size();

    cout << "+----------------------------------------------------------------+\n"
         << "|              FALSE SHARING TIMING EXAMPLES                     |\n"
         << "+----------------------------------------------------------------+\n";
    cout << "Threads: " << nthreads
         << "  |  Iterations/thread: " << ITERS
         << "  |  Cache line: " << CACHE_LINE << " B\n"
         << "sizeof(long long): " << sizeof(long long) << " B  ->  "
         << COUNTERS_PER_LINE << " counters per cache line\n";
    if (topo.valid()) {
        cout << "Logical CPUs: " << topo.cpus.size()
             << "  |  Physical cores: " << topo.firstSiblingOfCore.size()
             << "  |  L3 domains (CCX): " << topo.numL3Domains() << "\n";
        cout << "Pinned to CPUs:";
        for (int c : cpuSet) cout << " " << c << "(L3=" << topo.l3Of(c) << ")";
        cout << "\n";
    } else {
        cout << "Topology unavailable - threads are NOT pinned; expect noise.\n";
    }
    cout << "\nRunning benchmarks ...\n";

    double t_false  = bench([&]{ test_false_sharing(cpuSet); });
    double t_padded = bench([&]{ test_padded(cpuSet); });
    double t_local  = bench([&]{ test_local_accum(cpuSet); });

    printResult("TEST 1: Packed Array vs Cache-Line-Padded Struct",
                "Packed  (false sharing)",    t_false,
                "Padded  (64-byte aligned)",  t_padded);

    printResult("TEST 2: Packed Array vs Thread-Local Accumulation",
                "Packed  (false sharing)",    t_false,
                "Local   (one shared write)", t_local);

    // ---- TEST 3: how far does the contended line have to travel? ----
    if (topo.valid() && topo.numL3Domains() > 1) {
        int  base   = topo.firstSiblingOfCore[0];
        int  smt    = topo.smtPartner(base);
        auto sameL3 = topo.coresInSameL3(base);
        auto othL3  = topo.coresInOtherL3(base);

        struct Placement { string label; int a, b; };
        vector<Placement> places;
        if (smt >= 0)
            places.push_back({"SMT siblings (one core)", base, smt});
        if (!sameL3.empty())
            places.push_back({"Same CCX as core 0",      base, sameL3[0]});
        if (othL3.size() >= 2)
            places.push_back({"Same CCX, other CCX",     othL3[0], othL3[1]});
        if (!othL3.empty())
            places.push_back({"Cross CCX (via fabric)",  base, othL3[0]});

        if (!places.empty()) {
            cout << "\n" << string(64, '=') << "\n"
                 << "TEST 3: Coherence Distance (2 threads, 1 contended line)\n"
                 << string(64, '-') << "\n"
                 << "  " << setw(24) << left << "placement"
                 << setw(11) << left << "cpus"
                 << setw(11) << right << "shared"
                 << setw(11) << right << "own-line"
                 << setw(11) << right << "penalty" << "\n";
            for (auto const& p : places) {
                double sh = bench_pair_shared(p.a, p.b);
                double ol = bench_pair_ownline(p.a, p.b);
                cout << "  " << setw(24) << left << p.label
                     << setw(11) << left
                     << (to_string(p.a) + "+" + to_string(p.b))
                     << setw(9) << right << fixed << setprecision(1) << sh << " ms"
                     << setw(9) << right << fixed << setprecision(1) << ol << " ms"
                     << setw(10) << right << fixed << setprecision(2)
                     << (sh / max(0.001, ol)) << "x" << "\n";
            }
            cout << "\n"
                 << "  'penalty' = shared / own-line: the false-sharing cost with\n"
                 << "  per-core throughput divided out.  Compare THAT across rows,\n"
                 << "  not the raw ms - CPUs 0-3 are Zen 5 and 4-11 are Zen 5c, so\n"
                 << "  the two core types differ in speed before sharing enters.\n";
        }
    }

    cout << "\n" << string(64, '=') << "\n"
         << "SUMMARY - FALSE SHARING\n"
         << string(64, '=') << "\n"
         << R"(
  What is false sharing?
    Two threads write to DIFFERENT variables that happen to
    share a cache line.  The hardware treats the whole line
    as the unit of ownership, so every write forces every
    other core to re-acquire the line before its next write.

  Cache line layout (64 bytes, long long = 8 bytes):

    PACKED (false sharing):
      [ t0 | t1 | t2 | t3 | t4 | t5 | t6 | t7 ]  <- 1 line, 8 writers
      Each write invalidates the other 7 cores' copies.

    PADDED (no false sharing):
      [ t0 + 56 pad ] [ t1 + 56 pad ] ...  <- 1 line per thread
      Each core owns its line exclusively.

  Fixes:

    Structural  - pad the shared struct to 64 bytes:
      struct alignas(64) Counter { long long v; char pad[56]; };

    Algorithmic - accumulate locally, write once:
      long long local = 0;
      for (...) local++;   // stays in register
      shared[t] = local;   // one write at the end

  Placement matters too (TEST 3) - but not the way you expect:
    The textbook prediction is that crossing a CCX boundary
    makes false sharing worse, because invalidations travel
    the Infinity Fabric instead of staying under one L3.
    On this part the measurement says the opposite: the
    cross-CCX penalty is the SMALLEST and by far the most
    repeatable, while same-CCX pairs are both worse and wildly
    variable run to run.
    The likely reason is batching.  A higher handoff latency
    means the core that owns the line keeps it LONGER, so it
    retires more increments per ownership and needs fewer
    total handoffs.  Raising the cost of a transfer lowers the
    number of transfers, and here the second effect wins.
    Space the writes out with independent work and the naive
    latency ordering comes back.
    Take the lesson as: false-sharing cost is set by the
    handoff RATE as much as the handoff LATENCY, and it must
    be measured on the target part, not predicted from a
    topology diagram.

  Local-accumulation test near 0 ms?  The compiler (-O1+)
  eliminated the loop: "local++" x N collapses to "local = N",
  which is legal because the loop has no observable effect.
  Cache-friendly code is also compiler-friendly.
)" << "\n";

    return 0;
}
