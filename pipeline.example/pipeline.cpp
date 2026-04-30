#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>

using namespace std;
using namespace std::chrono;

const int ARRAY_SIZE = 100000000;

// Memory barrier
static void clobber() {
    asm volatile("" ::: "memory");
}

// ============================================================
// EXAMPLE 1: Dependent Chain vs Independent Operations
// ============================================================

// SLOW: Each operation depends on the previous one
// Pipeline STALLS waiting for each result
long long dependentChain(vector<int>& data) {
    long long x = 1;
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // DEPENDENCY CHAIN: each line needs previous result
        x = x + data[i];
        x = x ^ (x >> 1);
        x = x * 3;
        x = x ^ (x >> 2);
    }

    return x;
}

// FAST: Four independent chains processed simultaneously
// Pipeline stays FULL - CPU executes 4 operations in parallel
long long independentChains(vector<int>& data) {
    // Four SEPARATE accumulators - no dependencies between them
    long long x1 = 1, x2 = 2, x3 = 3, x4 = 4;
    
    for (int i = 0; i < ARRAY_SIZE; i += 4) {
        // Chain 1 - independent
        x1 = x1 + data[i];
        x1 = x1 ^ (x1 >> 1);
        x1 = x1 * 3;
        x1 = x1 ^ (x1 >> 2);

        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Chain 2 - independent of chain 1
        x2 = x2 + data[i + 1];
        x2 = x2 ^ (x2 >> 1);
        x2 = x2 * 3;
        x2 = x2 ^ (x2 >> 2);

        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Chain 3 - independent of chains 1,2
        x3 = x3 + data[i + 2];
        x3 = x3 ^ (x3 >> 1);
        x3 = x3 * 3;
        x3 = x3 ^ (x3 >> 2);

        std::atomic_thread_fence(std::memory_order_seq_cst);
        
        // Chain 4 - independent of chains 1,2,3
        x4 = x4 + data[i + 3];
        x4 = x4 ^ (x4 >> 1);
        x4 = x4 * 3;
        x4 = x4 ^ (x4 >> 2);

        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
    
    return x1 + x2 + x3 + x4;
}

// explicit reordering (compiler did it in AI version before)
long long independentChains2(vector<int>& data) {
    // Four SEPARATE accumulators - no dependencies between them
    long long x1 = 1, x2 = 2, x3 = 3, x4 = 4;
    
    for (int i = 0; i < ARRAY_SIZE; i += 4) {
        // Chain 1 - independent
        x1 = x1 + data[i];
        x1 = x1 ^ (x1 >> 1);
        x1 = x1 * 3;
        x1 = x1 ^ (x1 >> 2);
        
        // Chain 2 - independent of chain 1
        x2 = x2 + data[i + 1];
        x2 = x2 ^ (x2 >> 1);
        x2 = x2 * 3;
        x2 = x2 ^ (x2 >> 2);
        
        // Chain 3 - independent of chains 1,2
        x3 = x3 + data[i + 2];
        x3 = x3 ^ (x3 >> 1);
        x3 = x3 * 3;
        x3 = x3 ^ (x3 >> 2);
        
        // Chain 4 - independent of chains 1,2,3
        x4 = x4 + data[i + 3];
        x4 = x4 ^ (x4 >> 1);
        x4 = x4 * 3;
        x4 = x4 ^ (x4 >> 2);
    }
    
    return x1 + x2 + x3 + x4;
}

// ============================================================
// EXAMPLE 2: Single Accumulator vs Multiple Accumulators
// ============================================================

// SLOW: Single accumulator - dependency every iteration
long long singleAccumulator(vector<int>& data) {
    long long sum = 0;
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += data[i];  // Must wait for previous sum
    }
    
    return sum;
}

// FAST: Multiple accumulators - can add in parallel
long long multipleAccumulators(vector<int>& data) {
    long long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    long long sum5 = 0, sum6 = 0, sum7 = 0, sum8 = 0;
    
    for (int i = 0; i < ARRAY_SIZE; i += 8) {
        sum1 += data[i];      // These 8 additions are
        sum2 += data[i + 1];  // INDEPENDENT of each other
        sum3 += data[i + 2];  // CPU can execute them
        sum4 += data[i + 3];  // simultaneously in the
        sum5 += data[i + 4];  // pipeline!
        sum6 += data[i + 5];
        sum7 += data[i + 6];
        sum8 += data[i + 7];
    }
    
    return sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8;
}


// Benchmark helper
template <typename Func>
pair<long long, long long> benchmark(const string& name, Func func, 
                                      vector<int>& data, int runs = 5) {
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
        
        long long time = duration_cast<milliseconds>(end - start).count();
        minTime = min(minTime, time);
    }
    
    // Prevent optimization
    volatile long long dummy = result;
    (void)dummy;
    
    return {minTime, result};
}

int main() {
    cout << "=== CORRECT Pipeline Demonstration ===" << endl;
    cout << "Array size: " << ARRAY_SIZE << " elements\n" << endl;
    
    vector<int> data(ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = (i % 100) + 1;
    }
    clobber();
    
    // Warmup
    volatile long long warmup = dependentChain(data);
    warmup += independentChains(data);
    (void)warmup;
    
    cout << "=============================================" << endl;
    cout << "TEST 1: Dependent Chain vs Independent Chains" << endl;
    cout << "=============================================" << endl;
    
    auto [time1a, res1a] = benchmark("Dependent", dependentChain, data);
    auto [time1b, res1b] = benchmark("Independent", independentChains, data);
    auto [time1c, res1c] = benchmark("Independent2", independentChains2, data);
    
    cout << "Dependent chain:     " << setw(5) << time1a << " ms" << endl;
    cout << "Independent chains:  " << setw(5) << time1b << " ms" << endl;
    cout << "Independent chains2:  " << setw(5) << time1c << " ms" << endl;
    cout << "Speedup: " << fixed << setprecision(2) 
         << (double)time1a / max(1LL, time1b) << "x" << endl;
    
    cout << "\n=============================================" << endl;
    cout << "TEST 2: Single vs Multiple Accumulators" << endl;
    cout << "=============================================" << endl;
    
    auto [time2a, res2a] = benchmark("Single", singleAccumulator, data);
    auto [time2b, res2b] = benchmark("Multiple", multipleAccumulators, data);
    
    cout << "Single accumulator:    " << setw(5) << time2a << " ms" << endl;
    cout << "Multiple accumulators: " << setw(5) << time2b << " ms" << endl;
    cout << "Speedup: " << fixed << setprecision(2) 
         << (double)time2a / max(1LL, time2b) << "x" << endl;
    
   
    
    // Prevent all results from being optimized away
    volatile long long final_check = res1a + res1b +res1c + res2a + res2b;
    cout << "(Checksum: " << final_check << ")\n" << endl;
    
    return 0;
}