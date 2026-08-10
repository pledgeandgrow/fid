//! Performance benchmarks for the Pledge compiler.
//!
//! Run with: `cargo bench`
//!
//! These benchmarks measure:
//! - Lexing speed
//! - Parsing speed
//! - IR lowering speed
//! - Interpretation speed
//! - C codegen speed
//! - PE emission speed

use std::process::Command;
use std::time::Instant;

/// A simple benchmark helper that runs a function N times and reports stats.
pub struct BenchResult {
    pub name: String,
    pub iterations: usize,
    pub total_ms: f64,
    pub avg_ms: f64,
    pub min_ms: f64,
    pub max_ms: f64,
}

impl BenchResult {
    pub fn report(&self) {
        println!(
            "{:30} {:>5} iters  {:>8.2}ms total  {:>6.3}ms avg  {:>6.3}ms min  {:>6.3}ms max",
            self.name, self.iterations, self.total_ms, self.avg_ms, self.min_ms, self.max_ms
        );
    }
}

/// Run a closure `n` times and collect timing statistics.
pub fn bench<F: FnMut()>(name: &str, n: usize, mut f: F) -> BenchResult {
    let mut times: Vec<f64> = Vec::with_capacity(n);
    let mut total = 0.0;

    for _ in 0..n {
        let start = Instant::now();
        f();
        let elapsed = start.elapsed().as_secs_f64() * 1000.0;
        times.push(elapsed);
        total += elapsed;
    }

    times.sort_by(|a, b| a.partial_cmp(b).unwrap());
    let min = times.first().copied().unwrap_or(0.0);
    let max = times.last().copied().unwrap_or(0.0);
    let avg = total / n as f64;

    BenchResult {
        name: name.to_string(),
        iterations: n,
        total_ms: total,
        avg_ms: avg,
        min_ms: min,
        max_ms: max,
    }
}

/// Get the pledgec binary path.
fn pledgec_bin() -> String {
    let bin = std::env::var("CARGO_BIN_EXE_pledgec")
        .unwrap_or_else(|_| "target/debug/pledgec".to_string());
    bin
}

#[test]
fn bench_lexing() {
    let bin = pledgec_bin();
    let source = include_str!("../tests/hello.p");
    let n = 50;
    let result = bench("lexing (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["tokens", "tests/hello.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_parsing() {
    let bin = pledgec_bin();
    let n = 50;
    let result = bench("parsing (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["ast", "tests/hello.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_ir_lowering() {
    let bin = pledgec_bin();
    let n = 50;
    let result = bench("IR lowering (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["ir", "tests/hello.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_interpretation() {
    let bin = pledgec_bin();
    let n = 50;
    let result = bench("interpretation (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["run", "tests/hello.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_c_codegen() {
    let bin = pledgec_bin();
    let n = 10;
    let result = bench("C codegen (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["ccompile", "tests/hello.p", "-o", "/tmp/bench_hello.exe"])
            .output();
        std::fs::remove_file("/tmp/bench_hello.exe").ok();
        std::fs::remove_file("/tmp/bench_hello.c").ok();
    });
    result.report();
}

#[test]
fn bench_pe_emission() {
    let bin = pledgec_bin();
    let n = 10;
    let result = bench("PE emission (hello.p)", n, || {
        let _ = Command::new(&bin)
            .args(["emit", "tests/hello.p", "-o", "/tmp/bench_hello_pe.exe"])
            .output();
        std::fs::remove_file("/tmp/bench_hello_pe.exe").ok();
    });
    result.report();
}

#[test]
fn bench_interpretation_collections() {
    let bin = pledgec_bin();
    let n = 20;
    let result = bench("interpretation (collections)", n, || {
        let _ = Command::new(&bin)
            .args(["run", "tests/test_collections.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_interpretation_closures() {
    let bin = pledgec_bin();
    let n = 20;
    let result = bench("interpretation (closures)", n, || {
        let _ = Command::new(&bin)
            .args(["run", "tests/test_closure.p"])
            .output();
    });
    result.report();
}

#[test]
fn bench_all_tests_interpreter() {
    let bin = pledgec_bin();
    let n = 3;
    let result = bench("all tests (interpreter)", n, || {
        let _ = Command::new(&bin).arg("test").output();
    });
    result.report();
}
