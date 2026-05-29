// Compile: clang++ -O2 -fno-tree-vectorize -std=c++20 -o prefetch.example prefetch.example.cpp
//
// Software prefetch is most effective when:
//   1. The access pattern is irregular (the hardware prefetcher can't predict it).
//   2. Memory latency is high enough to overlap with computation.
//
// On x86 the canonical intrinsic is _mm_prefetch(); on all GCC/Clang targets
// __builtin_prefetch(addr, rw, locality) is portable:
//   rw       : 0 = read, 1 = write
//   locality : 0 = streaming (no reuse), 1 = L3, 2 = L2, 3 = L1

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

// -----------------------------------------------------------------------
// Case 1: Sequential — hardware prefetcher already handles this.
// Adding __builtin_prefetch is pure overhead here.
// -----------------------------------------------------------------------

__attribute__((noinline))
long long sum_sequential(const int* data, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i) s += data[i];
    return s;
}

__attribute__((noinline))
long long sum_sequential_prefetch(const int* data, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i) {
        __builtin_prefetch(&data[i + 16], 0, 1); // redundant: CPU already does this
        s += data[i];
    }
    return s;
}

// -----------------------------------------------------------------------
// Case 2: Pointer chasing — the hardware cannot predict `node->next->next`
// because it depends on data, not address arithmetic.
// Prefetching two hops ahead overlaps the cache miss with computation.
// -----------------------------------------------------------------------

struct Node {
    Node* next;
    int   value;
    char  pad[52]; // one node = one 64-byte cache line
};

__attribute__((noinline))
long long sum_list(Node* head) {
    long long s = 0;
    for (Node* n = head; n; n = n->next)
        s += n->value;
    return s;
}

__attribute__((noinline))
long long sum_list_prefetch(Node* head) {
    long long s = 0;
    for (Node* n = head; n; n = n->next) {
        // n->next is already in cache (we just loaded it to branch here).
        // Dereference it cheaply to get next->next's address, then prefetch.
        if (n->next) __builtin_prefetch(n->next->next, 0, 1);
        s += n->value;
    }
    return s;
}

// -----------------------------------------------------------------------

int main() {
    static constexpr int N    = 1 << 20; // 1M elements / nodes
    static constexpr int RUNS = 5;

    std::mt19937 rng{42};

    // --- Sequential data ---
    std::vector<int> arr(N);
    for (auto& x : arr) x = rng() & 0xFFFF;

    // --- Linked list with randomised traversal order ---
    std::vector<Node> pool(N);
    std::vector<int> order(N);
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), rng);
    for (int i = 0; i < N; ++i)          pool[order[i]].value = i;
    for (int i = 0; i + 1 < N; ++i)      pool[order[i]].next  = &pool[order[i + 1]];
    pool[order[N - 1]].next = nullptr;
    Node* head = &pool[order[0]];

    // Large flush buffer to evict caches between runs.
    std::vector<char> flush(256 << 20, 1);

    auto bench = [&](const char* label, auto fn, auto&&... args) {
        double best = 1e9;
        for (int r = 0; r < RUNS; ++r) {
            volatile char s = std::accumulate(flush.begin(), flush.end(), char(0)); (void)s;
            auto t0 = std::chrono::high_resolution_clock::now();
            volatile long long res = fn(std::forward<decltype(args)>(args)...); (void)res;
            auto t1 = std::chrono::high_resolution_clock::now();
            best = std::min(best, std::chrono::duration<double, std::milli>(t1 - t0).count());
        }
        std::cout << label << ": " << best << " ms\n";
    };

    std::cout << "=== Sequential access (hardware prefetcher already handles this) ===\n";
    bench("  plain     ", sum_sequential,        arr.data(), N);
    bench("  prefetched", sum_sequential_prefetch, arr.data(), N);

    std::cout << "=== Pointer chasing (hardware can't predict; software hint helps) ===\n";
    bench("  plain     ", sum_list,         head);
    bench("  prefetched", sum_list_prefetch, head);

    std::cout << "\nNote: benefit of software prefetch depends heavily on memory latency.\n"
              << "It is most visible on x86 or systems where DRAM latency >> compute time.\n"
              << "Apple Silicon's unified memory architecture reduces the gap.\n";
}
