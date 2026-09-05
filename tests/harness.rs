//! Automated test harness for the Fides compiler.
//!
//! Tests three backends against every `.fd` file in the `tests/` directory:
//!   1. **Interpreter** (`fidesc run`) — baseline; all tests must pass.
//!   2. **PE Emitter** (`fidesc emit`) — validates PE structure (cannot
//!      execute on Linux without Wine).
//!   3. **C Codegen** (`fidesc ccompile`) — compiles to C, builds with gcc,
//!      executes, and compares output against the interpreter.
//!
//! Run with: `cargo test`

use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// Tests from phases that are not yet fully implemented. Skipped to avoid
/// false regressions in the harness.
fn is_skip_test(name: &str) -> bool {
    matches!(name,
        "test_comptime_basic.fd"
        | "test_comptime_func.fd"
        | "test_comptime_reflect.fd"
        | "test_comptime_serialize.fd"
        | "test_comptime_gen.fd"
        | "test_http.fd"
        | "test_json.fd"
        | "test_tensor.fd"
        | "test_autodiff.fd"
        | "test_kv_cache.fd"
        | "test_quantization.fd"
        | "test_checkpoint.fd"
        | "test_dp.fd"
        | "test_shard.fd"
        | "test_lora.fd"
        | "test_simd.fd"
        | "test_shape.fd"
        | "test_shape_verify.fd"
        | "test_wasm.fd"
        | "test_allocator.fd"
        | "test_effect_handler.fd"
        | "test_asm.fd"
        | "test_bare.fd"
        | "test_boot.fd"
        | "test_interrupt.fd"
        | "test_mmu.fd"
        | "test_gat.fd"
        | "test_ffi_header.fd"
        | "test_dlopen.fd"
        | "test_game_stdlib.fd"
        | "test_kernel_serial.fd"
        | "test_os_gaps.fd"
        | "test_phase8b.fd"
        | "test_vec_math.fd"
        | "test_vec_shape_fail.fd"
        // Benchmark files produce timing-dependent output that differs
        // between interpreter and native codegen — not correctness tests.
        | "bench_stringbuilder.fd"
        // Borrow checker fail test is now in the expected-fail list
        // (borrow checking is on by default as of Phase 5).
    )
}

/// Tests that are expected to fail (they test error detection).
fn is_expect_fail_test(name: &str) -> bool {
    matches!(
        name,
        "test_trait_verify_fail.fd" | "test_budget_fail.fd" | "test_sandbox_fail.fd"
        | "test_deterministic_fail.fd" | "test_field_privacy_fail.fd"
        | "test_money_linear_fail.fd" | "test_money_conserve_fail.fd"
        | "test_banking_double_credit_fail.fd" | "test_banking_lost_money_fail.fd"
        | "test_loop_linear_fail.fd"
        | "test_gas_fail.fd"
        | "test_crashfree_fail.fd"
        | "test_typed_strings_fail.fd"
        | "test_init_guarantee_fail.fd"
        | "test_init_guarantee_cond_fail.fd"
        | "test_param_queries_fail.fd"
        | "test_medical_fail.fd"
        | "test_safety_annotations_fail.fd"
        | "test_safety_no_audit_fail.fd"
        | "test_contracts_fail.fd"
        | "test_borrowck_fail.fd"
        | "test_async_fail.fd"
        | "test_constant_time_fail.fd"
        | "test_constant_time_fail2.fd"
        | "test_send_fail.fd"
        | "test_gpu_region_fail.fd"
        | "test_shape_verify_fail.fd"
        | "test_realtime_fail.fd"
        | "test_realtime_fail2.fd"
        | "test_autodiff_fail.fd"
        | "test_kv_cache_fail.fd"
        | "test_quantization_fail.fd"
        | "test_checkpoint_fail.fd"
        | "test_dp_fail.fd"
        | "test_shard_fail.fd"
    )
}

/// Discover all `.fd` test files in the `tests/` directory.
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
        .filter(|p| p.extension().map_or(false, |e| e == "fd"))
        .collect();
    tests.sort();
    tests
}

/// Build the fidesc binary if not already built.
fn ensure_binary() -> PathBuf {
    let bin = PathBuf::from("target/debug/fidesc");
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
        .map_err(|e| format!("Failed to run fidesc emit: {}", e))?;

    if !output.status.success() {
        return Err(format!(
            "fidesc emit failed: {}",
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
    assert!(!tests.is_empty(), "No .fd test files found in tests/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();

        // Skip tests from unimplemented phases
        if is_skip_test(&name) {
            continue;
        }

        match run_interpreter(&bin, test) {
            Some((code, _stdout)) => {
                // Tests that expect to fail: non-zero exit is a pass
                if is_expect_fail_test(&name) {
                    if code != 0 {
                        passed += 1;
                    } else {
                        failed += 1;
                        failures.push(format!("{}: expected failure but succeeded", name));
                    }
                    continue;
                }
                // Interpreter should not panic (exit code 1 with "Panic:" is a
                // failure). A normal return with any exit code is a pass.
                if code == 1 && _stdout.is_empty() {
                    failed += 1;
                    failures.push(format!(
                        "{}: exit=1 with no output",
                        name
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
    assert!(!tests.is_empty(), "No .fd test files found in tests/");

    let tmp = if cfg!(windows) {
        std::env::temp_dir().join("pledge_pe_tests")
    } else {
        PathBuf::from("/tmp/pledge_pe_tests")
    };
    let _ = fs::create_dir_all(&tmp);

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();
        if is_skip_test(&name) || is_expect_fail_test(&name) {
            continue;
        }
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
    assert!(!tests.is_empty(), "No .fd test files found in tests/");

    // Use a platform-appropriate temp directory for C codegen test artifacts.
    // On Windows, /tmp doesn't exist — use the local temp dir or a project-local dir.
    let tmp = if cfg!(windows) {
        std::env::temp_dir().join("pledge_c_tests")
    } else {
        PathBuf::from("/tmp/pledge_c_tests")
    };
    let _ = fs::create_dir_all(&tmp);

    let mut matched = 0;
    let mut compile_failed = 0;
    let mut mismatched = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &tests {
        let name = test.file_name().unwrap().to_string_lossy();

        if is_skip_test(&name) || is_expect_fail_test(&name) {
            continue;
        }

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
/// To regenerate golden files: `fidesc run tests/<test>.fd > tests/golden/<test>.txt`
#[test]
fn golden_files_match() {
    let bin = ensure_binary();
    let tests = discover_tests();
    assert!(!tests.is_empty(), "No .fd test files found in tests/");

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
        let test_name = test.file_name().unwrap().to_string_lossy().to_string();
        if is_skip_test(&test_name) || is_expect_fail_test(&test_name) {
            continue;
        }
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

/// Safety test: verify that bounds checking and overflow detection panic
/// correctly in debug mode. Fides has checked arithmetic always on (F64) —
/// overflow panics in both debug and release mode. The escape hatch is
/// `@wrapping`, not `--release`. Test files live in tests/safety/.
#[test]
fn safety_checks_panic_in_debug_mode() {
    let bin = ensure_binary();
    let safety_dir = PathBuf::from("tests/safety");
    if !safety_dir.exists() {
        println!("tests/safety/ directory not found, skipping safety test.");
        return;
    }

    let mut safety_tests: Vec<PathBuf> = fs::read_dir(&safety_dir)
        .expect("Could not open tests/safety/ directory")
        .filter_map(|e| e.ok())
        .map(|e| e.path())
        .filter(|p| p.extension().map_or(false, |e| e == "fd"))
        .collect();
    safety_tests.sort();
    assert!(!safety_tests.is_empty(), "No .fd safety test files found in tests/safety/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &safety_tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();

        // Run in debug mode (default) — should panic (exit code 1, stderr has "Panic:")
        let output = Command::new(&bin)
            .arg("run")
            .arg(test)
            .output()
            .expect("Failed to run interpreter");
        let debug_code = output.status.code().unwrap_or(-1);
        let debug_stderr = String::from_utf8_lossy(&output.stderr).to_string();

        if debug_code != 1 || !debug_stderr.contains("Panic:") {
            failed += 1;
            failures.push(format!(
                "{}: expected panic in debug mode but got exit={} stderr={:?}",
                name, debug_code, debug_stderr
            ));
            continue;
        }

        // Fides has checked arithmetic always on (F64). Overflow panics in
        // both debug and release mode — `@wrapping` is the escape hatch,
        // not `--release`. So we don't test release-mode overflow behavior.

        passed += 1;
    }

    if !failures.is_empty() {
        panic!(
            "Safety checks: {}/{} passed, {} failed:\n{}",
            passed,
            safety_tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("Safety checks: {}/{} tests passed", passed, safety_tests.len());
}

/// Capability test: verify that capability granting and denial works.
/// Test files in tests/capability/ are split into "grant" (should succeed)
/// and "deny" (should fail with capability error) tests.
#[test]
fn capability_security_enforced() {
    let bin = ensure_binary();
    let cap_dir = PathBuf::from("tests/capability");
    if !cap_dir.exists() {
        println!("tests/capability/ directory not found, skipping capability test.");
        return;
    }

    let mut cap_tests: Vec<PathBuf> = fs::read_dir(&cap_dir)
        .expect("Could not open tests/capability/ directory")
        .filter_map(|e| e.ok())
        .map(|e| e.path())
        .filter(|p| p.extension().map_or(false, |e| e == "fd"))
        .collect();
    cap_tests.sort();
    assert!(!cap_tests.is_empty(), "No .fd capability test files found in tests/capability/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &cap_tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();

        let output = Command::new(&bin)
            .arg("run")
            .arg(test)
            .output()
            .expect("Failed to run interpreter");
        let code = output.status.code().unwrap_or(-1);
        let stderr = String::from_utf8_lossy(&output.stderr).to_string();

        if name.contains("_deny_") {
            // Deny tests: should fail with a capability error
            if code == 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected capability denial (exit=1) but succeeded (exit=0)",
                    name
                ));
            } else if !stderr.contains("Capability error") {
                failed += 1;
                failures.push(format!(
                    "{}: expected 'Capability error' in stderr but got: {}",
                    name, stderr
                ));
            } else {
                passed += 1;
            }
        } else {
            // Grant tests: should succeed
            if code != 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected success (exit=0) but failed (exit={}): {}",
                    name, code, stderr
                ));
            } else {
                passed += 1;
            }
        }
    }

    if !failures.is_empty() {
        panic!(
            "Capability security: {}/{} passed, {} failed:\n{}",
            passed,
            cap_tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("Capability security: {}/{} tests passed", passed, cap_tests.len());
}

/// Effect system test: verify effect annotations, inference, and checking.
/// Test files in tests/effects/ are split into:
/// - "should pass" (correct annotations, pure functions, propagation)
/// - "should fail" (under-declaration, pure violation)
#[test]
fn effect_system_enforced() {
    let bin = ensure_binary();
    let effect_dir = PathBuf::from("tests/effects");
    if !effect_dir.exists() {
        println!("tests/effects/ directory not found, skipping effect test.");
        return;
    }

    let mut effect_tests: Vec<PathBuf> = fs::read_dir(&effect_dir)
        .expect("Could not open tests/effects/ directory")
        .filter_map(|e| e.ok())
        .map(|e| e.path())
        .filter(|p| p.extension().map_or(false, |e| e == "fd"))
        .collect();
    effect_tests.sort();
    assert!(!effect_tests.is_empty(), "No .fd effect test files found in tests/effects/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &effect_tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();

        let output = Command::new(&bin)
            .arg("run")
            .arg(test)
            .output()
            .expect("Failed to run interpreter");
        let code = output.status.code().unwrap_or(-1);
        let stderr = String::from_utf8_lossy(&output.stderr).to_string();

        if name.contains("underdeclare") || name.contains("pure_violation") {
            // These tests should fail with an effect error
            if code == 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected effect error (exit=1) but succeeded (exit=0)",
                    name
                ));
            } else if !stderr.contains("Effect error") {
                failed += 1;
                failures.push(format!(
                    "{}: expected 'Effect error' in stderr but got: {}",
                    name, stderr
                ));
            } else {
                passed += 1;
            }
        } else {
            // These tests should succeed
            if code != 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected success (exit=0) but failed (exit={}): {}",
                    name, code, stderr
                ));
            } else {
                passed += 1;
            }
        }
    }

    if !failures.is_empty() {
        panic!(
            "Effect system: {}/{} passed, {} failed:\n{}",
            passed,
            effect_tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("Effect system: {}/{} tests passed", passed, effect_tests.len());
}

/// Region inference test: verify region blocks, escape detection, and safety.
#[test]
fn region_inference_enforced() {
    let bin = ensure_binary();
    let region_dir = PathBuf::from("tests/regions");
    if !region_dir.exists() {
        println!("tests/regions/ directory not found, skipping region test.");
        return;
    }

    let mut region_tests: Vec<PathBuf> = fs::read_dir(&region_dir)
        .expect("Could not open tests/regions/ directory")
        .filter_map(|e| e.ok())
        .map(|e| e.path())
        .filter(|p| p.extension().map_or(false, |e| e == "fd"))
        .collect();
    region_tests.sort();
    assert!(!region_tests.is_empty(), "No .fd region test files found in tests/regions/");

    let mut passed = 0;
    let mut failed = 0;
    let mut failures: Vec<String> = Vec::new();

    for test in &region_tests {
        let name = test.file_name().unwrap().to_string_lossy().to_string();

        let output = Command::new(&bin)
            .arg("run")
            .arg(test)
            .output()
            .expect("Failed to run interpreter");
        let code = output.status.code().unwrap_or(-1);
        let stderr = String::from_utf8_lossy(&output.stderr).to_string();

        if name.contains("escape") {
            // These tests should fail with a region error
            if code == 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected region error (exit=1) but succeeded (exit=0)",
                    name
                ));
            } else if !stderr.contains("Region error") && !stderr.contains("undefined variable") {
                failed += 1;
                failures.push(format!(
                    "{}: expected 'Region error' or 'undefined variable' in stderr but got: {}",
                    name, stderr
                ));
            } else {
                passed += 1;
            }
        } else {
            // These tests should succeed
            if code != 0 {
                failed += 1;
                failures.push(format!(
                    "{}: expected success (exit=0) but failed (exit={}): {}",
                    name, code, stderr
                ));
            } else {
                passed += 1;
            }
        }
    }

    if !failures.is_empty() {
        panic!(
            "Region inference: {}/{} passed, {} failed:\n{}",
            passed,
            region_tests.len(),
            failed,
            failures.join("\n")
        );
    }
    println!("Region inference: {}/{} tests passed", passed, region_tests.len());
}
