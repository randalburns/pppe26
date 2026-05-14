// Loop Unrolling Example — Rust port of loop_unrolling.cpp
//
// Three strategies for handling the tail of an array whose length
// is not a multiple of the unroll factor:
//   1. Scalar baseline
//   2. 4x unroll with a scalar cleanup loop
//   3. Duff's Device equivalent (see note below)
//
// Translation notes:
//   - const int* data, int n  →  &[i32]  (slice carries pointer + length)
//   - long long               →  i64
//   - asm volatile sink       →  std::hint::black_box
//   - LLONG_MAX               →  i64::MAX / u128::MAX
//   - vector<int>             →  Vec<i32>
//   - high_resolution_clock   →  std::time::Instant  (returns u128 ms)
//
// Duff's Device cannot be translated mechanically: Rust's match has no
// fallthrough, so a switch that jumps into the middle of a do/while loop
// is inexpressible.  The equivalent behaviour — process the tail first,
// then the aligned bulk — is semantically identical for a commutative
// operation like addition.
//
// Build:
//   rustc -C opt-level=1 -o unroll_O1 loop_unrolling.rs && ./unroll_O1
//   rustc -C opt-level=2 -o unroll_O2 loop_unrolling.rs && ./unroll_O2

use std::hint::black_box;
use std::time::Instant;

const N: usize = 100_000_000;
const RUNS: usize = 5;

// ----------------------------------------------------------------
// Benchmarking helpers
// ----------------------------------------------------------------

fn bench<F: Fn() -> i64>(f: F) -> u128 {
    let mut best = u128::MAX;
    for _ in 0..RUNS {
        let t0 = Instant::now();
        let v = f();
        let elapsed = t0.elapsed().as_millis();
        let _ = black_box(v);  // prevent dead-code elimination
        best = best.min(elapsed);
    }
    best
}

fn report(title: &str, a_label: &str, a_ms: u128, b_label: &str, b_ms: u128) {
    let speedup = a_ms as f64 / b_ms.max(1) as f64;
    println!("\n{}", "-".repeat(64));
    println!("{}", title);
    println!("{}", "-".repeat(64));
    println!("  BASELINE  {:<40} {:>5} ms", a_label, a_ms);
    println!("  UNROLLED  {:<40} {:>5} ms", b_label, b_ms);
    println!("  Speedup:  {:.2}x", speedup);
}

// ================================================================
// 1. Scalar baseline — one add per iteration
// ================================================================

fn sum_scalar(data: &[i32]) -> i64 {
    let mut s: i64 = 0;
    for &x in data {
        s += x as i64;
    }
    s
}

// ================================================================
// 2. 4x unroll with a scalar cleanup loop
//
// Main loop processes 4 elements at once, reducing loop-control
// overhead (branch, increment) by 4x.  A trailing loop handles
// the remaining 0–3 elements.
// ================================================================

fn sum_unrolled4(data: &[i32]) -> i64 {
    let n = data.len();
    let mut s: i64 = 0;
    let n4 = n - (n % 4);   // largest multiple of 4 <= n
    let mut i = 0;

    while i < n4 {
        s += data[i]     as i64;
        s += data[i + 1] as i64;
        s += data[i + 2] as i64;
        s += data[i + 3] as i64;
        i += 4;
    }

    // Cleanup: handle tail elements (0, 1, 2, or 3 remaining).
    while i < n {
        s += data[i] as i64;
        i += 1;
    }

    s
}

// ================================================================
// 3. Duff's Device equivalent
//
// The original C uses switch fallthrough to jump into the middle of
// a do/while loop.  Rust's match does not fall through, so the same
// effect is achieved by:
//   a) processing the tail elements first  (the "entry point" of
//      the C switch), then
//   b) processing the remaining bulk in groups of 4.
//
// The result is identical for addition (commutative, order-independent).
// ================================================================

fn sum_duffs(data: &[i32]) -> i64 {
    let n = data.len();
    if n == 0 { return 0; }

    let mut s: i64 = 0;
    let tail = n % 4;

    // Handle tail first (maps to the switch entry point in the C version).
    for i in 0..tail {
        s += data[i] as i64;
    }

    // Process remaining elements in groups of 4.
    let mut i = tail;
    while i < n {
        s += data[i]     as i64;
        s += data[i + 1] as i64;
        s += data[i + 2] as i64;
        s += data[i + 3] as i64;
        i += 4;
    }

    s
}

// ================================================================
// main — build the array and run benchmarks
// ================================================================

fn main() {
    // Non-trivial values so the compiler cannot constant-fold.
    let data: Vec<i32> = (0..N).map(|i| (i % 7) as i32 + 1).collect();

    let n_odd = N - 3;   // tail = 1 element, exercises cleanup path
    println!("Array length (odd test):  {}  (tail = {} element(s))", n_odd, n_odd % 4);
    println!("Array length (even test): {}  (tail = {} element(s))", N, N % 4);

    // Correctness check.
    let r1 = sum_scalar   (&data[..n_odd]);
    let r2 = sum_unrolled4(&data[..n_odd]);
    let r3 = sum_duffs    (&data[..n_odd]);
    println!("\nCorrectness check (n_odd):");
    print!  ("  scalar={}  unrolled4={}  duffs={}", r1, r2, r3);
    println!("  {}", if r1 == r2 && r2 == r3 { "PASS" } else { "FAIL" });

    // ---- Benchmark: odd-length array --------------------------------
    let t_scalar   = bench(|| sum_scalar   (&data[..n_odd]));
    let t_unrolled = bench(|| sum_unrolled4(&data[..n_odd]));
    let t_duffs    = bench(|| sum_duffs    (&data[..n_odd]));

    report("Odd-length array (n % 4 == 1) — loop-overhead reduction",
           "scalar loop",         t_scalar,
           "4x unroll + cleanup", t_unrolled);
    report("Odd-length array (n % 4 == 1) — Duff's Device equiv. vs scalar",
           "scalar loop",         t_scalar,
           "Duff's Device equiv.", t_duffs);

    // ---- Benchmark: even-length array (no tail) ---------------------
    let t_scalar_e   = bench(|| sum_scalar   (&data));
    let t_unrolled_e = bench(|| sum_unrolled4(&data));
    let t_duffs_e    = bench(|| sum_duffs    (&data));

    report("Even-length array (n % 4 == 0) — loop-overhead reduction",
           "scalar loop",         t_scalar_e,
           "4x unroll + cleanup", t_unrolled_e);
    report("Even-length array (n % 4 == 0) — Duff's Device equiv. vs scalar",
           "scalar loop",         t_scalar_e,
           "Duff's Device equiv.", t_duffs_e);

    println!("\nNote: at opt-level=2 the compiler auto-vectorises the scalar loop,");
    println!("      often matching or beating manual unrolling.");
    println!("      Build with opt-level=1 to see raw unrolling benefits.");
}
