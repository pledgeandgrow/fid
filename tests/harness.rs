//! Automated test harness for the Pledge compiler.
//!
//! Tests three backends against every `.p` file in the `tests/` directory:
//!   1. **Interpreter** (`pledgec run`) — baseline; all tests must pass.
//!   2. **PE Emitter** (`pledgec emit`) — validates PE structure (cannot
//!      execute on Linux without Wine).
//!   3. **C Codegen** (`pledgec ccompile`) — compiles to C, builds with gcc,
//!      executes, and compares output against the interpreter.
//!
//! Run with: `cargo test`

use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// Discover all `.p` test files in the `tests/` directory.
fn discover_tests() -> Vec<PathBuf> {
    // cargo test runs from the package root, so "tests/" should work.
    // Fall back to a path relative to the test file for robustness.
    let test_dir = PathBuf::from("tests");
    let dir = if test_dir.exists() {
        test_dir
    } else {
        // Fall back: CARGO_MANIFEST_DIR is set by cargo
        let manifest = std::env::var("CARGO_MANIFEST_DIR").unwrap_or_else(|_| ".".to_string());
        PathBuf::from(manifest).join("tests")
    };

    let mut tests: Vec<PathBuf> = fs::read_dir(&dir)
        .expect("Could not open tests/ directory")
        .filter_map(|e| e.ok())
        .map(|e| e.path())
        .filter(|p| p.extension().map_or(false, |e| e == "p"))
        .collect();
    tests.sort();
    tests
}

/// Build the pledgec binary if not already built.
fn ensure_binary() -> PathBuf {
    let bin = PathBuf::from("target/debug/pledgec");
    if !bin.exists() {
        let status = Command::new("cargo")
            .args(["build"])
            .status()
            .expect("Failed to run `cargo build`");
        assert!(status.success(), "cargo build failed");
    }
    bin.canonicalize().unwrap_or(bin)
}

/// Run a test file through the interpreter and return (exit_code, stdout).
fn run_interpreter(bin: &Path, test: &Path) -> Option<(i32, String)> {
    let output = Command::new(bin)
        .arg("run")
        .arg(test)
        .output()
        .ok()?;
    let code = output.status.code().unwrap_or(-1);
    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    Some((code, stdout))
}

/// Run a test file through the PE emitter and validate the PE structure.
/// Returns Ok(()) if the PE is valid, Err(message) otherwise.
fn run_pe_emitter(bin: &Path, test: &Path, tmp: &Path) -> Result<(), String> {
    let exe_path = tmp.join(format!(
        "{}.exe",
        test.file_stem().unwrap().to_string_lossy()
    ));

    let output = Command::new(bin)
        .arg("emit")
        .arg(test)
        .arg("-o")
        .arg(&exe_path)
        .output()
        .map_err(|e| format!("Failed to run pledgec emit: {}", e))?;

    if !output.status.success() {
        return Err(format!(
            "pledgec emit failed: {}",
            String::from_utf8_lossy(&output.stderr)
        ));
    }

    let data = fs::read(&exe_path).map_err(|e| format!("Failed to read .exe: {}", e))?;

    // Validate PE structure
    if data.len() < 64 {
        return Err("File too small for DOS header".to_string());
    }
    if &data[0..2] != b"MZ" {
        return Err("Missing MZ signature".to_string());
    }
    let pe_offset = u32::from_le_bytes([data[0x3C], data[0x3D], data[0x3E], data[0x3F]]) as usize;
    if pe_offset + 24 > data.len() {
        return Err("PE header offset out of bounds".to_string());
    }
    if &data[pe_offset..pe_offset + 4] != b"PE\0\0" {
        return Err("Missing PE signature".to_string());
    }
    let machine = u16::from_le_bytes([data[pe_offset + 4], data[pe_offset + 5]]);
    if machine != 0x8664 {
        return Err(format!("Wrong machine type: {:#x} (expected 0x8664)", machine));
    }
    let num_sections = u16::from_le_bytes([data[pe_offset + 6], data[pe_offset + 7]]);
    if num_sections < 2 {
        return Err(format!("Too few sections: {}", num_sections));
    }
    let opt_hdr_size = u16::from_le_bytes([data[pe_offset + 20], data[pe_offset + 21]]) as usize;
    let opt_offset = pe_offset + 24;
    if opt_offset + opt_hdr_size > data.len() {
        return Err("Optional header out of bounds".to_string());
    }
    let magic = u16::from_le_bytes([data[opt_offset], data[opt_offset + 1]]);
    if magic != 0x20b {
        return Err(format!("Wrong optional header magic: {:#x} (expected 0x20b)", magic));
    }

    let _ = fs::remove_file(&exe_path);
    Ok(())
}

/// Run a test file through the C codegen, compile with gcc, execute, and
/// return (exit_code, stdout). Returns None if compilation fails.
fn run_c_codegen(bin: &Path, test: &Path, tmp: &Path) -> Option<(i32, String)> {
    let stem = test.file_stem().unwrap().to_string_lossy();
    // Use .exe extension so the compiler's .c file path derivation works
    // (it replaces ".exe" with ".c" in the output path).
    let exe_path = tmp.join(format!("{}_c.exe", stem));

    let output = Command::new(bin)
        .arg("ccompile")
        .arg(test)
        .arg("-o")
        .arg(&exe_path)
        .output()
        .ok()?;

    if !output.status.success() {
        let _ = fs::remove_file(&exe_path);
        return None;
    }

    if !exe_path.exists() {
        return None;
    }

    let exe_output = Command::new(&exe_path).output().ok()?;
    let _ = fs::remove_file(&exe_path);
    // Also clean up the .c file (same path with .c instead of .exe)
    let c_path = tmp.join(format!("{}_c.c", stem));
    let _ = fs::remove_file(c_path);

    let code = exe_output.status.code().unwrap_or(-1);
    let stdout = String::from_utf8_lossy(&exe_output.stdout).to_string();
    Some((code, stdout))
}

// ─── Test entry points ─────────────────────────────────────────────────

#[test]
fn interpreter_all_tests() {
    let bin = ensure_binary();
    let tests = discover_tests();
    assert!(!tests.is_empty(), "No .p test files found in tests/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        match run_interpreter(&bin, test) {
            Some((code, _stdout)) => {
                // Interpreter should not panic (exit code 1 with "Panic:" is a
                // failure). A normal return with any exit code is a pass.
                if code == 1 && _stdout.is_empty() {
                    failed += 1;
                    failures.push(format!(
                        "{}: exit=1 with no output",
                        test.file_name().unwrap().to_string_lossy()
                    ));
                } else {
                    passed += 1;
                }
            }
            None => {
                failed += 1;
                failures.push(format!(
                    "{}: failed to run",
                    test.file_name().unwrap().to_string_lossy()
                ));
            }
        }
    }

    if !failures.is_empty() {
        panic!(
            "Interpreter: {}/{} passed, {} failed:\n{}",
            passed,
            tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("Interpreter: {}/{} tests passed", passed, tests.len());
}

#[test]
fn pe_emitter_all_tests() {
    let bin = ensure_binary();
    let tests = discover_tests();
    assert!(!tests.is_empty(), "No .p test files found in tests/");

    let tmp = PathBuf::from("/tmp/pledge_pe_tests");
    let _ = fs::create_dir(&tmp);

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        match run_pe_emitter(&bin, test, &tmp) {
            Ok(()) => passed += 1,
            Err(e) => {
                failed += 1;
                failures.push(format!(
                    "{}: {}",
                    test.file_name().unwrap().to_string_lossy(),
                    e
                ));
            }
        }
    }

    let _ = fs::remove_dir_all(&tmp);

    if !failures.is_empty() {
        panic!(
            "PE Emitter: {}/{} passed, {} failed:\n{}",
            passed,
            tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("PE Emitter: {}/{} tests passed (structure validated)", passed, tests.len());
}

#[test]
fn c_codegen_matches_interpreter() {
    let bin = ensure_binary();
    let tests = discover_tests();
    assert!(!tests.is_empty(), "No .p test files found in tests/");

    let tmp = PathBuf::from("/tmp/pledge_c_tests");
    let _ = fs::create_dir(&tmp);

    let mut matched = 0;
    let mut compile_failed = 0;
    let mut mismatched = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        let name = test.file_name().unwrap().to_string_lossy();

        // Get interpreter output as baseline
        let (interp_code, interp_out) = match run_interpreter(&bin, test) {
            Some(r) => r,
            None => {
                compile_failed += 1;
                failures.push(format!("{}: interpreter failed", name));
                continue;
            }
        };

        // Get C codegen output
        match run_c_codegen(&bin, test, &tmp) {
            Some((c_code, c_out)) => {
                if c_code == interp_code && c_out == interp_out {
                    matched += 1;
                } else {
                    mismatched += 1;
                    failures.push(format!(
                        "{}: output mismatch\n  interp (exit={}): {:?}\n  c     (exit={}): {:?}",
                        name, interp_code, interp_out, c_code, c_out
                    ));
                }
            }
            None => {
                compile_failed += 1;
                failures.push(format!("{}: C compilation failed", name));
            }
        }
    }

    let _ = fs::remove_dir_all(&tmp);

    // C codegen now passes all tests. Report any failures.
    println!(
        "C Codegen: {}/{} matched interpreter, {} compile failures, {} mismatches",
        matched,
        tests.len(),
        compile_failed,
        mismatched
    );

    if !failures.is_empty() {
        println!("\nC Codegen failures:\n{}", failures.join("\n"));
    }

    // All 38 tests should match the interpreter.
    let threshold = 38;
    assert!(
        matched >= threshold,
        "C Codegen regression: only {}/{} matched interpreter (threshold: {}).\nFailures:\n{}",
        matched,
        tests.len(),
        threshold,
        failures.join("\n")
    );
}

/// Golden file test: compare interpreter output against stored golden files.
/// To regenerate golden files: `pledgec run tests/<test>.p > tests/golden/<test>.txt`
#[test]
fn golden_files_match() {
    let bin = ensure_binary();
    let tests = discover_tests();
    assert!(!tests.is_empty(), "No .p test files found in tests/");

    let golden_dir = PathBuf::from("tests/golden");
    if !golden_dir.exists() {
        println!("Golden file directory not found, skipping golden file test.");
        return;
    }

    let mut matched = 0;
    let mut missing = 0;
    let mut mismatched = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        let name = test.file_stem().unwrap().to_string_lossy().to_string();
        let golden_path = golden_dir.join(format!("{}.txt", name));

        if !golden_path.exists() {
            missing += 1;
            continue;
        }

        let golden = fs::read_to_string(&golden_path).unwrap_or_default();

        match run_interpreter(&bin, test) {
            Some((_code, stdout)) => {
                if stdout == golden {
                    matched += 1;
                } else {
                    mismatched += 1;
                    failures.push(format!(
                        "{}: golden mismatch\n  golden: {:?}\n  actual: {:?}",
                        name, golden, stdout
                    ));
                }
            }
            None => {
                mismatched += 1;
                failures.push(format!("{}: interpreter failed", name));
            }
        }
    }

    println!(
        "Golden files: {}/{} matched, {} missing, {} mismatched",
        matched,
        tests.len(),
        missing,
        mismatched
    );

    if !failures.is_empty() {
        panic!(
            "Golden file mismatch:\n{}",
            failures.join("\n")
        );
    }
}
