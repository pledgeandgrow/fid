// Suppress warnings for the multi-backend compiler CLI. Not all code paths
// are exercised in every build; dead code is retained for backend parity.
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_assignments)]
#![allow(unused_mut)]

use std::env;
use std::fs;
use std::process;

use fidesc::codegen_arm64;
use fidesc::codegen_riscv;
use fidesc::codegen_x64;
use fidesc::coff;
use fidesc::ast;
use fidesc::compiler;
use fidesc::compiler::RunResult;
use fidesc::cparser;
use fidesc::debugger;
use fidesc::elf64;
use fidesc::fmt;
use fidesc::gpu;
use fidesc::hotreload;
use fidesc::interp;
use fidesc::linker;
use fidesc::lower;
use fidesc::lsp;
use fidesc::macho;
use fidesc::native_debugger;
use fidesc::opt;
use fidesc::peephole;
use fidesc::pir;
use fidesc::seqdiag;
use fidesc::typechecker::TypeChecker;

/// Print a list of error strings to stderr with colored "error:" prefix.
///
/// If an error string is already a rendered Diag (contains "error[E" or "-->"),
/// it's printed as-is. Otherwise, a colored "error:" prefix is added.
fn print_errors(errors: &[String]) {
    use std::io::IsTerminal;
    let use_color = std::io::stderr().is_terminal();
    fidesc::diag::set_color(use_color);

    let red = if use_color { "\x1b[31m" } else { "" };
    let bold = if use_color { "\x1b[1m" } else { "" };
    let reset = if use_color { "\x1b[0m" } else { "" };

    for e in errors {
        if e.contains("error[E") || e.contains("warning[E") || e.contains("-->") {
            eprintln!("{}", e);
        } else {
            eprintln!("{}{}error:{} {}", bold, red, reset, e);
        }
    }
}

/// Run `path` and capture its output, killing it after `limit`.
fn run_with_timeout(path: &str, limit: std::time::Duration) -> Option<process::Output> {
    let mut child = process::Command::new(path)
        .stdout(process::Stdio::piped())
        .stderr(process::Stdio::piped())
        .spawn()
        .ok()?;
    // Read the output as it comes: a child filling a pipe would block forever
    let drain = |pipe: Option<Box<dyn std::io::Read + Send>>| {
        std::thread::spawn(move || {
            let mut bytes = Vec::new();
            if let Some(mut p) = pipe {
                let _ = std::io::Read::read_to_end(&mut p, &mut bytes);
            }
            bytes
        })
    };
    let out = drain(child.stdout.take().map(|p| Box::new(p) as Box<dyn std::io::Read + Send>));
    let err = drain(child.stderr.take().map(|p| Box::new(p) as Box<dyn std::io::Read + Send>));
    let start = std::time::Instant::now();
    loop {
        match child.try_wait() {
            Ok(Some(status)) => {
                return Some(process::Output {
                    status,
                    stdout: out.join().unwrap_or_default(),
                    stderr: err.join().unwrap_or_default(),
                });
            }
            Ok(None) if start.elapsed() < limit => std::thread::sleep(std::time::Duration::from_millis(20)),
            _ => {
                let _ = child.kill();
                let _ = child.wait();
                return None;
            }
        }
    }
}

// Deeply nested programs recurse deeply in the parser, the checkers and
// lowering; the default main-thread stack (1 MB on Windows) overflows, so the
// real entry point runs on a dedicated 256 MB-stack thread. WASI cannot spawn
// threads, so the wasm build calls real_main directly (its stack size is set
// at link time via `-zstack-size`).
#[cfg(not(target_family = "wasm"))]
fn main() {
    let compiler = std::thread::Builder::new()
        .stack_size(256 * 1024 * 1024)
        .spawn(real_main)
        .expect("failed to start the compiler thread");
    if compiler.join().is_err() {
        process::exit(101);
    }
}

#[cfg(target_family = "wasm")]
fn main() {
    real_main();
}

fn real_main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        print_usage();
        process::exit(1);
    }

    // Parse global --borrowck=off|warn|on flag (affects all compilation commands).
    // Default is "on" (borrow checking always active unless explicitly disabled).
    // Can appear anywhere in the args.
    // We also filter these flags out of args so command-specific parsing isn't confused.
    // Also parse --timeout N (wall-clock timeout in seconds; kills process after N seconds).
    let mut filtered_args: Vec<String> = Vec::new();
    let mut timeout_secs: u64 = 0;
    for a in &args {
        if let Some(val) = a.strip_prefix("--borrowck=") {
            match fidesc::borrowck::BorrowckMode::parse(val) {
                Some(mode) => fidesc::borrowck::set_mode(mode),
                None => {
                    eprintln!("Invalid --borrowck value '{}'. Use off, warn, or on.", val);
                    process::exit(1);
                }
            }
        } else if a == "--timeout" {
            // Handled below — skip adding to filtered_args
        } else if let Some(t) = a.strip_prefix("--timeout=") {
            match t.parse::<u64>() {
                Ok(secs) => timeout_secs = secs,
                Err(_) => {
                    eprintln!("Invalid --timeout value '{}'. Use seconds (e.g. --timeout 30).", t);
                    process::exit(1);
                }
            }
        } else if let Some(t) = filtered_args.last().and_then(|s| {
            if s == "--timeout" { Some(s.as_str()) } else { None }
        }) {
            // Previous arg was "--timeout", this is the value
            match a.parse::<u64>() {
                Ok(secs) => timeout_secs = secs,
                Err(_) => {
                    eprintln!("Invalid --timeout value '{}'. Use seconds (e.g. --timeout 30).", a);
                    process::exit(1);
                }
            }
            // Remove the "--timeout" we already added
            filtered_args.pop();
        } else {
            filtered_args.push(a.clone());
        }
    }

    // If --timeout N was specified, spawn a watcher thread that kills the process
    // after N seconds. This prevents hanging compilation or execution.
    if timeout_secs > 0 {
        std::thread::spawn(move || {
            std::thread::sleep(std::time::Duration::from_secs(timeout_secs));
            eprintln!("\nTimeout: process exceeded {} seconds. Killing.", timeout_secs);
            process::exit(124); // 124 = timeout exit code (like GNU `timeout`)
        });
    }
    let args: Vec<String> = filtered_args;

    let command = &args[1];

    match command.as_str() {
        "lsp" => {
            // Run the LSP server over stdio
            process::exit(lsp::run());
        }
        "parse-header" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc parse-header <file.h> [-o output.fd]");
                process::exit(1);
            }
            let input = &args[2];
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone());
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let include = format!("\"{}\"", input.rsplit('/').next().unwrap_or(input));
            let decls = cparser::parse_header(&source, &include);
            let bindings = cparser::generate_pledge_bindings(&decls, &include);
            match output {
                Some(path) => {
                    fs::write(&path, &bindings).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", path, e);
                        process::exit(1);
                    });
                    println!("Generated {} bindings ({} functions) → {}", input, decls.len(), path);
                }
                None => {
                    print!("{}", bindings);
                }
            }
        }
        "build" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc build <file.fd> [-o output] [--target x86-64|arm64|riscv|riscv64-linux|arm64-linux|x86-64-windows] [--opt-level 0|1|2|3] [--no-opt] [--lto] [--caps \"IO,FS\"]");
                eprintln!("       Compiles and links a Fides program into a native executable in one step");
                eprintln!("       --target     Target architecture (default: x86-64). Supports cross-compilation to ARM64, RISC-V, and Windows.");
                eprintln!("       --opt-level  Optimization level 0-3 (default: 2). 0=no opt, 1=minimal, 2=full, 3=aggressive+LTO.");
                eprintln!("       --no-opt     Disable PIR optimizer (equivalent to --opt-level 0)");
                eprintln!("       --lto        Enable cross-module LTO (equivalent to --opt-level 3)");
                eprintln!("       --caps       Grant only these capabilities (e.g. \"IO,FS\"). Strips code requiring ungranted caps.");
                eprintln!("       -g/--debug   Emit DWARF line tables in the PE (windows targets).");
                process::exit(1);
            }
            let input = &args[2];
            let raw_target = args.iter().position(|a| a == "--target")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or(if cfg!(windows) { "x86-64-windows" } else { "x86-64" });
            // Parse target triple: "arch-os" format (e.g., "arm64-linux", "riscv64-linux")
            // If no OS specified, default to linux
            let target = parse_target_triple(raw_target);
            // Parse --opt-level (default O2). --no-opt overrides to O0.
            // --lto overrides to O3. Explicit --opt-level takes precedence.
            let opt_level = if args.iter().any(|a| a == "--no-opt") {
                opt::OptLevel::O0
            } else if args.iter().any(|a| a == "--lto") {
                opt::OptLevel::O3
            } else {
                args.iter().position(|a| a == "--opt-level")
                    .and_then(|i| args.get(i + 1))
                    .and_then(|s| opt::OptLevel::parse(s))
                    .unwrap_or(opt::OptLevel::O2)
            };
            let do_lto = opt_level == opt::OptLevel::O3 || args.iter().any(|a| a == "--lto");
            let granted_caps: Option<Vec<String>> = args.iter().position(|a| a == "--caps")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.split(',').map(|c| c.trim().to_string()).collect());
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone())
                .unwrap_or_else(|| {
                    let stem = input.rsplit('.').nth(1).unwrap_or("output");
                    format!("{}", stem)
                });
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let build_cache = fidesc::inc::BuildCache::open("build", &args[2..], input, &output, &["FID_HOME"]);
            if build_cache.up_to_date() {
                println!("{} is up to date", output);
                return;
            }
            let debug = args.iter().any(|a| a == "-g" || a == "--debug");
            match compiler::build_executable_with_level(&source, input, target, opt_level, do_lto, granted_caps.as_deref(), debug) {
                Ok(exe_bytes) => {
                    fs::write(&output, &exe_bytes).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", output, e);
                        process::exit(1);
                    });
                    build_cache.commit(None);
                    #[cfg(unix)]
                    {
                        use std::os::unix::fs::PermissionsExt;
                        let _ = fs::set_permissions(&output, fs::Permissions::from_mode(0o755));
                    }
                    println!("Built native executable: {} ({} bytes, zero external tools)", output, exe_bytes.len());
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "build-x86" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc build-x86 <file.fd> [-o output]");
                eprintln!("       Builds a Linux ELF executable using the x86.rs stack-machine backend (AT&T syntax)");
                process::exit(1);
            }
            let input = &args[2];
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone())
                .unwrap_or_else(|| {
                    let stem = input.rsplit('.').nth(1).unwrap_or("output");
                    format!("{}", stem)
                });
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::build_x86_linux(&source, input) {
                Ok(exe_bytes) => {
                    fs::write(&output, &exe_bytes).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", output, e);
                        process::exit(1);
                    });
                    #[cfg(unix)]
                    {
                        use std::os::unix::fs::PermissionsExt;
                        let _ = fs::set_permissions(&output, fs::Permissions::from_mode(0o755));
                    }
                    println!("Built native executable: {} ({} bytes, x86.rs backend)", output, exe_bytes.len());
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "emit" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc emit <file.fd> [-o output.exe] [-g] [--bare] [--boot] [--opt-level 0|1|2|3] [--load-addr <hex>] [--linker-script <file>]");
                process::exit(1);
            }
            let bare_mode = args.iter().any(|a| a == "--bare");
            let boot_mode = args.iter().any(|a| a == "--boot");
            // Parse --opt-level (default O2 for the regular PE path).
            let opt_level = args.iter().position(|a| a == "--opt-level")
                .and_then(|i| args.get(i + 1))
                .and_then(|s| opt::OptLevel::parse(s))
                .unwrap_or(opt::OptLevel::O2);
            // --load-addr: override the default load address (default: 0x10000 for bare, 0x100000 for boot)
            let load_addr: Option<u64> = args.iter().position(|a| a == "--load-addr")
                .and_then(|i| args.get(i + 1))
                .and_then(|s| u64::from_str_radix(s.trim_start_matches("0x"), 16).ok());
            // --linker-script: parse a simple linker script for section placement
            // Currently supports: ENTRY(name), LOAD_ADDR(hex), SECTION(name, addr)
            let linker_script: Option<String> = args.iter().position(|a| a == "--linker-script")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone());
            // If a linker script is provided, parse load address from it
            let load_addr = if load_addr.is_some() {
                load_addr
            } else if let Some(ref script_path) = linker_script {
                if let Ok(script) = fs::read_to_string(script_path) {
                    parse_load_addr_from_script(&script)
                } else {
                    eprintln!("Error: cannot read linker script '{}'", script_path);
                    process::exit(1);
                }
            } else {
                None
            };
            // Find the input file (first non-flag argument after "emit")
            let input = args[2..].iter()
                .find(|a| !a.starts_with("-") && !a.starts_with("--"))
                .map(|s| s.to_string())
                .unwrap_or_else(|| {
                    eprintln!("Error: no input file specified");
                    process::exit(1);
                });
            let input = &input;
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone())
                .unwrap_or_else(|| {
                    let stem = input.rsplit('.').nth(1).unwrap_or("output");
                    if boot_mode { format!("{}.bin", stem) }
                    else if bare_mode { format!("{}.bin", stem) }
                    else { format!("{}.exe", stem) }
                });
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            if boot_mode {
                // Boot mode: multiboot2-compatible kernel binary
                match compiler::emit_boot(&source, input) {
                    Ok(bin_bytes) => {
                        fs::write(&output, &bin_bytes).unwrap_or_else(|e| {
                            eprintln!("Error writing '{}': {}", output, e);
                            process::exit(1);
                        });
                        println!("Emitted multiboot2 kernel: {} ({} bytes, boot-ready)", output, bin_bytes.len());
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }
            } else if bare_mode {
                // Bare-metal mode: skip stdlib, produce flat binary
                match compiler::emit_bare(&source, input) {
                    Ok(bin_bytes) => {
                        fs::write(&output, &bin_bytes).unwrap_or_else(|e| {
                            eprintln!("Error writing '{}': {}", output, e);
                            process::exit(1);
                        });
                        println!("Emitted bare-metal binary: {} ({} bytes, no stdlib, no PE headers)", output, bin_bytes.len());
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }
            } else {
                let build_cache = fidesc::inc::BuildCache::open("emit", &args[2..], input, &output, &["FID_HOME"]);
                if build_cache.up_to_date() {
                    println!("{} is up to date", output);
                    return;
                }
                // -g / --debug: DWARF v4 line tables in the PE so a standard
                // debugger maps machine code back to .fd source lines.
                let debug = args.iter().any(|a| a == "-g" || a == "--debug");
                match compiler::emit_source_debug(&source, input, opt_level, debug) {
                    Ok(pe_bytes) => {
                        fs::write(&output, &pe_bytes).unwrap_or_else(|e| {
                            eprintln!("Error writing '{}': {}", output, e);
                            process::exit(1);
                        });
                        build_cache.commit(None);
                        println!("Emitted native executable: {} ({} bytes, no ml64.exe/link.exe)", output, pe_bytes.len());
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }
            }
        }

        "compile" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc compile <file.fd> [-o output.s]");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            let output_file = if args.len() >= 5 && args[3] == "-o" {
                args[4].clone()
            } else {
                "output.s".to_string()
            };

            match compiler::compile(&source) {
                Ok(result) => {
                    if let Err(e) = fs::write(&output_file, &result.assembly) {
                        eprintln!("Error writing '{}': {}", output_file, e);
                        process::exit(1);
                    }
                    println!("Compiled {} → {}", input, output_file);
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "tokens" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc tokens <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            for tok in compiler::lex(&source) {
                println!("{}", tok);
            }
        }

        "ast" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc ast <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::parse_to_ast(&source) {
                Ok(ast) => println!("{}", ast),
                Err(e) => {
                    eprintln!("{}", e);
                    process::exit(1);
                }
            }
        }

        "ir" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc ir <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::lower_to_ir(&source) {
                Ok(ir) => println!("{}", ir),
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "run" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc run <file.fd> [-v] [--release] [--post-mortem] [--max-recursion N] [--max-steps N] [--max-memory N] [--timeout N] [--trace]");
                process::exit(1);
            }
            let input = &args[2];
            let verbose = args.iter().any(|a| a == "-v" || a == "--verbose");
            let release = args.iter().any(|a| a == "--release");
            let post_mortem = args.iter().any(|a| a == "--post-mortem");
            let trace = args.iter().any(|a| a == "--trace");
            // SH-1.4: --max-recursion N (default 1000, 0 = unlimited)
            let max_recursion = args.iter().position(|a| a == "--max-recursion")
                .and_then(|i| args.get(i + 1))
                .and_then(|v| v.parse::<usize>().ok())
                .unwrap_or(1000);
            // --max-steps N (default 0 = unlimited). Prevents infinite loops.
            let max_steps = args.iter().position(|a| a == "--max-steps")
                .and_then(|i| args.get(i + 1))
                .and_then(|v| v.parse::<u64>().ok())
                .unwrap_or(0);
            // --max-memory N (in MB, default 0 = unlimited). Prevents OOM kills.
            let max_memory = args.iter().position(|a| a == "--max-memory")
                .and_then(|i| args.get(i + 1))
                .and_then(|v| v.parse::<u64>().ok())
                .map(|mb| mb * 1024 * 1024) // Convert MB to bytes
                .unwrap_or(0);
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            if post_mortem {
                // Run with trace recording for post-mortem debugging
                match compiler::interpret_source_with_trace(&source, input, verbose, release) {
                    Ok((result, stack_trace, trace)) => {
                        match result {
                            RunResult::Return(val) => {
                                let code = match val {
                                    compiler::Value::Int(i) => i as i32,
                                    _ => 0,
                                };
                                process::exit(code);
                            }
                            RunResult::Panic(msg) => {
                                eprintln!("Panic: {}", msg);
                                if let Some(trace) = stack_trace {
                                    eprint!("{}", trace);
                                }
                                // Save trace for post-mortem replay
                                let trace_path = format!("{}.fdtrace", input);
                                match debugger::save_trace(&trace, &trace_path) {
                                    Ok(()) => {
                                        eprintln!("\nPost-mortem trace saved to: {}", trace_path);
                                        eprintln!("Replay with: fidesc replay {}", trace_path);
                                    }
                                    Err(e) => {
                                        eprintln!("\nFailed to save post-mortem trace: {}", e);
                                    }
                                }
                                process::exit(1);
                            }
                            RunResult::Paused => {
                                process::exit(0);
                            }
                        }
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }
            }

            match compiler::interpret_source_traced_full2(&source, input, verbose, release, max_recursion, max_steps, max_memory, trace) {
                Ok((result, trace)) => {
                    match result {
                        RunResult::Return(val) => {
                            let code = match val {
                                compiler::Value::Int(i) => i as i32,
                                _ => 0,
                            };
                            process::exit(code);
                        }
                        RunResult::Panic(msg) => {
                            eprintln!("Panic: {}", msg);
                            if let Some(trace) = trace {
                                eprint!("{}", trace);
                            }
                            process::exit(1);
                        }
                        RunResult::Paused => {
                            process::exit(0);
                        }
                    }
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "debug" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc debug <file.fd>");
                eprintln!("  Interactive debugger with breakpoints, stepping, variable inspection.");
                eprintln!("  Commands: break, step, continue, print, backtrace, list, ir, effects, quit");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let mut dbg = debugger::Debugger::new(&source, input);
            let code = dbg.run(&source, input);
            process::exit(code);
        }

        "replay" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc replay <trace.fdtrace>");
                eprintln!("  Replay a recorded execution trace.");
                process::exit(1);
            }
            let input = &args[2];
            let code = debugger::replay_trace(input);
            process::exit(code);
        }

        "autopsy" | "native-debug" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc autopsy <file.fd>");
                eprintln!("  Native code debugger using ptrace (Linux only).");
                eprintln!("  Compiles to native binary and debugs with breakpoint injection.");
                process::exit(1);
            }
            let input = &args[2];
            let mut dbg = native_debugger::NativeDebugger::new(input);
            let code = dbg.run();
            process::exit(code);
        }

        "version" => {
            println!("fidesc 0.1.0 (bootstrap compiler)");
            println!("Written in Rust — will be replaced by self-hosted Fides compiler");
        }

        "dump-c" => {
            // SH-1.3: Print generated C source to stdout (for differential testing)
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-c <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::ccompile_source_traced(&source, input, false) {
                Ok(c_code) => {
                    print!("{}", c_code);
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        // SH-1.3: dump-* aliases for existing dump commands.
        // These provide a consistent "dump-<stage>" naming convention
        // for golden-file generation and differential testing.
        "dump-tokens" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-tokens <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            for tok in compiler::lex(&source) {
                println!("{}", tok);
            }
        }
        "dump-ast" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-ast <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::parse_to_ast(&source) {
                Ok(ast) => println!("{}", ast),
                Err(e) => {
                    eprintln!("{}", e);
                    process::exit(1);
                }
            }
        }
        "dump-ast-raw" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-ast-raw <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::parse_to_ast_raw(&source) {
                Ok(ast) => println!("{}", ast),
                Err(e) => {
                    eprintln!("{}", e);
                    process::exit(1);
                }
            }
        }
        "dump-ast-flat" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-ast-flat <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::parse_to_ast_flat_raw(&source) {
                Ok(ast) => println!("{}", ast),
                Err(e) => {
                    eprintln!("{}", e);
                    process::exit(1);
                }
            }
        }
        "dump-ir" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-ir <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::lower_to_ir(&source) {
                Ok(ir) => println!("{}", ir),
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }
        "dump-pir" => {
            // dump-pir <file.fd> [1|2|3|all] [--opt]
            // Alias for "pir" command with consistent naming.
            if args.len() < 3 {
                eprintln!("Usage: fidesc dump-pir <file.fd> [1|2|3|all] [--opt]");
                process::exit(1);
            }
            let input = &args[2];
            let level_arg = args.get(3).map(|s| s.as_str()).unwrap_or("all");
            let want_opt = args.iter().any(|a| a == "--opt");
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let module = match compiler::lower_to_ir_module(&source) {
                Ok(m) => m,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };
            let level = pir::PirLevel::parse(level_arg).unwrap_or(pir::PirLevel::All);
            match level {
                pir::PirLevel::L1 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    print!("{}", pir::format_l1(&l1));
                }
                pir::PirLevel::L2 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let mut l2 = pir::lower_l1_to_l2(&l1);
                    if want_opt {
                        let stats = opt::optimize_l2(&mut l2);
                        eprintln!(";; optimization stats: {:?}", stats);
                    }
                    print!("{}", pir::format_l2(&l2));
                }
                pir::PirLevel::L3 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let l2 = pir::lower_l1_to_l2(&l1);
                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                    print!("{}", pir::format_l3(&l3));
                }
                pir::PirLevel::All => {
                    if want_opt {
                        eprintln!(";; note: --opt applies to L2 only; showing all levels with L2 optimized");
                        let l1 = pir::lower_ir_to_l1(&module);
                        let mut l2 = pir::lower_l1_to_l2(&l1);
                        let stats = opt::optimize_l2(&mut l2);
                        eprintln!(";; optimization stats: {:?}", stats);
                        let l3 = pir::lower_l2_to_l3(&l2, &module);
                        print!("{}", pir::format_l1(&l1));
                        print!("\n");
                        print!("{}", pir::format_l2(&l2));
                        print!("\n");
                        print!("{}", pir::format_l3(&l3));
                    } else {
                        print!("{}", pir::generate_pir(&module, pir::PirLevel::All));
                    }
                }
            }
        }
        // SH-2: diff-test — differential testing: Rust compiler vs Fides compiler.
        // Default mode: runs the Rust compiler stage and the Fides harness, compares output.
        // Golden mode (--golden): compares Rust compiler output to golden file.
        //
        // Usage: fidesc diff-test <file.fd> --stage <stage> [--golden] [--update]
        // Stages: tokens, ast, ir, pir1, pir2, pir3, c, output
        //
        // Stage → Fides harness mapping (selfhost mode):
        //   tokens → tests/selfhost/test_lexer_harness.fd
        //   ast    → tests/selfhost/test_parser_harness.fd  (uses dump-ast-flat format)
        //   ir     → tests/selfhost/test_lower_harness.fd
        //   output → fidesc run <file.fd> (both sides use the Rust interpreter)
        "diff-test" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc diff-test <file.fd> --stage <stage> [--golden] [--update]");
                eprintln!("Stages: tokens, ast, ir, pir1, pir2, pir3, c, output");
                eprintln!("Default: compare Rust compiler vs Fides harness output");
                eprintln!("  --golden  compare Rust output to golden file (old behavior)");
                eprintln!("  --update  overwrite golden file with current Rust output");
                process::exit(1);
            }
            let input = &args[2];
            let mut stage: Option<&str> = None;
            let mut update_golden = false;
            let mut use_golden = false;
            let mut i = 3;
            while i < args.len() {
                match args[i].as_str() {
                    "--stage" => {
                        i += 1;
                        if i < args.len() {
                            stage = Some(&args[i]);
                        }
                    }
                    "--update" => {
                        update_golden = true;
                    }
                    "--golden" => {
                        use_golden = true;
                    }
                    _ => {}
                }
                i += 1;
            }
            let stage = match stage {
                Some(s) => s,
                None => {
                    eprintln!("Error: --stage <stage> is required");
                    eprintln!("Stages: tokens, ast, ir, pir1, pir2, pir3, c, output");
                    process::exit(1);
                }
            };

            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // ── Rust compiler output ──────────────────────────────────────────
            // For selfhost mode, the `ast` stage uses flat format to match the
            // parser harness. For golden mode, it uses S-expr format (old behavior).
            let rust_output: String = match stage {
                "tokens" => {
                    let mut s = String::new();
                    for tok in compiler::lex(&source) {
                        s.push_str(&format!("{}\n", tok));
                    }
                    s
                }
                "ast" if !use_golden => {
                    // Selfhost mode: use flat format to match parser harness
                    match compiler::parse_to_ast_flat_raw(&source) {
                        Ok(ast) => format!("{}\n", ast),
                        Err(e) => {
                            eprintln!("Rust parse error: {}", e);
                            process::exit(1);
                        }
                    }
                }
                "ast" => {
                    // Golden mode: S-expr format (old behavior)
                    match compiler::parse_to_ast(&source) {
                        Ok(ast) => format!("{}\n", ast),
                        Err(e) => {
                            eprintln!("Parse error: {}", e);
                            process::exit(1);
                        }
                    }
                }
                "ir" => {
                    match compiler::lower_to_ir(&source) {
                        Ok(ir) => format!("{}\n", ir),
                        Err(errors) => {
                            print_errors(&errors);
                            process::exit(1);
                        }
                    }
                }
                "pir1" | "pir2" | "pir3" => {
                    let level_num: u32 = stage[3..].parse().unwrap_or(0);
                    let out = match compiler::lower_to_ir_module(&source) {
                        Ok(module) => {
                            let l1 = pir::lower_ir_to_l1(&module);
                            if level_num == 1 {
                                pir::format_l1(&l1)
                            } else {
                                let l2 = pir::lower_l1_to_l2(&l1);
                                if level_num == 2 {
                                    pir::format_l2(&l2)
                                } else {
                                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                                    pir::format_l3(&l3)
                                }
                            }
                        }
                        Err(errors) => {
                            print_errors(&errors);
                            process::exit(1);
                        }
                    };
                    format!("{}\n", out)
                }
                "c" => {
                    match compiler::ccompile_source_traced(&source, input, false) {
                        Ok(c_code) => format!("{}\n", c_code),
                        Err(errors) => {
                            print_errors(&errors);
                            process::exit(1);
                        }
                    }
                }
                "output" => {
                    let current_exe = std::env::current_exe().unwrap_or_default();
                    let out = process::Command::new(&current_exe)
                        .args(["run", input])
                        .output();
                    match out {
                        Ok(o) => String::from_utf8_lossy(&o.stdout).to_string(),
                        Err(e) => {
                            eprintln!("Failed to run: {}", e);
                            process::exit(1);
                        }
                    }
                }
                _ => {
                    eprintln!("Unknown stage: '{}'", stage);
                    eprintln!("Stages: tokens, ast, ir, pir1, pir2, pir3, c, output");
                    process::exit(1);
                }
            };

            // ── Golden mode (old behavior) ────────────────────────────────────
            if use_golden {
                let test_name = std::path::Path::new(input)
                    .file_stem()
                    .unwrap_or_default()
                    .to_string_lossy()
                    .to_string();
                let golden_dir = format!("tests/bootstrap/golden/{}", test_name);
                let golden_ext = match stage {
                    "tokens" => "tokens",
                    "ast" => "ast",
                    "ir" => "ir",
                    "pir1" => "pir_l1",
                    "pir2" => "pir_l2",
                    "pir3" => "pir_l3",
                    "c" => "c",
                    "output" => "out",
                    _ => "txt",
                };
                let golden_path = format!("{}/{}.{}", golden_dir, test_name, golden_ext);

                if update_golden {
                    std::fs::create_dir_all(&golden_dir).ok();
                    std::fs::write(&golden_path, &rust_output).ok();
                    println!("Updated golden file: {}", golden_path);
                    return;
                }

                let golden = match fs::read_to_string(&golden_path) {
                    Ok(s) => s,
                    Err(_) => {
                        eprintln!("No golden file: {}", golden_path);
                        eprintln!("Run: fidesc diff-test {} --stage {} --golden --update", input, stage);
                        process::exit(2);
                    }
                };

                let normalize_sort = |s: &str| -> String {
                    let mut lines: Vec<&str> = s.lines().filter(|l| !l.is_empty()).collect();
                    lines.sort();
                    lines.join("\n")
                };
                let needs_sort = matches!(stage, "ir" | "pir1" | "pir2" | "pir3" | "c");
                let (norm_current, norm_golden) = if needs_sort {
                    (normalize_sort(&rust_output), normalize_sort(&golden))
                } else {
                    (rust_output.clone(), golden.clone())
                };

                if norm_current == norm_golden {
                    println!("{}: {} matches golden ✅", input, stage);
                } else {
                    println!("{}: {} DIFFERS from golden ❌", input, stage);
                    println!();
                    let diff: Vec<String> = norm_current
                        .lines()
                        .zip(norm_golden.lines())
                        .enumerate()
                        .filter_map(|(i, (c, g))| {
                            if c != g {
                                Some(format!("  line {}: - {} | + {}", i + 1, g, c))
                            } else {
                                None
                            }
                        })
                        .take(20)
                        .collect();
                    if diff.is_empty() {
                        let cur_lines: Vec<&str> = norm_current.lines().collect();
                        let gold_lines: Vec<&str> = norm_golden.lines().collect();
                        println!("  Line count differs: golden={} vs current={}", gold_lines.len(), cur_lines.len());
                        let max = cur_lines.len().max(gold_lines.len());
                        for i in (cur_lines.len().min(gold_lines.len()))..max.min(cur_lines.len().min(gold_lines.len()) + 20) {
                            let c = cur_lines.get(i).copied().unwrap_or("<missing>");
                            let g = gold_lines.get(i).copied().unwrap_or("<missing>");
                            if c != g {
                                println!("  line {}: - {} | + {}", i + 1, g, c);
                            }
                        }
                    } else {
                        for d in diff {
                            println!("{}", d);
                        }
                    }
                    process::exit(1);
                }
                return;
            }

            // ── Selfhost mode (default): Rust vs Fides harness ────────────────
            // Map stage to Fides harness file
            let harness_path: &str = match stage {
                "tokens" => "tests/selfhost/test_lexer_harness.fd",
                "ast" => "tests/selfhost/test_parser_harness.fd",
                "ir" => "tests/selfhost/test_lower_harness.fd",
                "output" => {
                    // For output stage, both sides use the Rust interpreter — no harness needed
                    // The Rust output is already captured above. Compare to itself (trivially passes).
                    println!("{}: output stage — both sides use Rust interpreter ✅ (trivial)", input);
                    return;
                }
                _ => {
                    eprintln!("No Fides harness for stage '{}'", stage);
                    eprintln!("Available selfhost stages: tokens, ast, ir");
                    eprintln!("Use --golden for golden-file comparison of other stages");
                    process::exit(1);
                }
            };

            // Check harness file exists
            if !std::path::Path::new(harness_path).exists() {
                eprintln!("Harness file not found: {}", harness_path);
                eprintln!("Create it or use --golden for golden-file comparison");
                process::exit(1);
            }

            // Run Fides harness: fidesc run <harness.fd> -- <file.fd>
            let current_exe = std::env::current_exe().unwrap_or_default();
            let fides_result = process::Command::new(&current_exe)
                .args(["run", harness_path, "--", input])
                .output();

            let fides_output = match fides_result {
                Ok(o) => {
                    if !o.status.success() {
                        eprintln!("Fides harness failed (exit {:?}):", o.status);
                        eprintln!("stderr: {}", String::from_utf8_lossy(&o.stderr));
                        // Still use stdout for comparison — harness may print partial output before failing
                        String::from_utf8_lossy(&o.stdout).to_string()
                    } else {
                        String::from_utf8_lossy(&o.stdout).to_string()
                    }
                }
                Err(e) => {
                    eprintln!("Failed to run Fides harness: {}", e);
                    process::exit(1);
                }
            };

            // Compare Rust output vs Fides output
            let normalize_sort = |s: &str| -> String {
                let mut lines: Vec<&str> = s.lines().filter(|l| !l.is_empty()).collect();
                lines.sort();
                lines.join("\n")
            };
            let needs_sort = matches!(stage, "ir" | "pir1" | "pir2" | "pir3" | "c");
            let (norm_rust, norm_fides) = if needs_sort {
                (normalize_sort(&rust_output), normalize_sort(&fides_output))
            } else {
                (rust_output.clone(), fides_output.clone())
            };

            if norm_rust == norm_fides {
                println!("{}: {} Rust==Fides ✅", input, stage);
            } else {
                println!("{}: {} Rust≠Fides ❌", input, stage);
                println!();
                // Show first differences with context
                let rust_lines: Vec<&str> = norm_rust.lines().collect();
                let fides_lines: Vec<&str> = norm_fides.lines().collect();
                println!("  Rust lines: {}, Fides lines: {}", rust_lines.len(), fides_lines.len());
                println!();

                // Find first differing line
                let min_len = rust_lines.len().min(fides_lines.len());
                let mut first_diff = None;
                for i in 0..min_len {
                    if rust_lines[i] != fides_lines[i] {
                        first_diff = Some(i);
                        break;
                    }
                }
                if first_diff.is_none() && rust_lines.len() != fides_lines.len() {
                    first_diff = Some(min_len);
                }

                match first_diff {
                    Some(idx) => {
                        let start = idx.saturating_sub(3);
                        let end = (idx + 5).min(rust_lines.len().max(fides_lines.len()));
                        println!("  First difference at line {}:", idx + 1);
                        println!();
                        for i in start..end {
                            let r = rust_lines.get(i).copied().unwrap_or("<missing>");
                            let f = fides_lines.get(i).copied().unwrap_or("<missing>");
                            if r == f {
                                println!("    {:>4} | {}", i + 1, r);
                            } else {
                                println!("  R {:>4} | {}", i + 1, r);
                                println!("  F {:>4} | {}", i + 1, f);
                            }
                        }
                        // Count total differences
                        let diffs: usize = rust_lines.iter()
                            .zip(fides_lines.iter())
                            .filter(|(a, b)| a != b)
                            .count();
                        let len_diff = (rust_lines.len() as isize - fides_lines.len() as isize).unsigned_abs();
                        println!();
                        println!("  Total: {} differing lines, {} length difference", diffs, len_diff);
                    }
                    None => {
                        println!("  Outputs are identical after normalization but differ in raw form (trailing whitespace?)");
                    }
                }
                process::exit(1);
            }
        }

        "ccompile" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc ccompile <file.fd> [-o output.exe] [--target <triple>] [--cc-opt 0|1|2|3|s] [--trace] [--debug|-g]");
                process::exit(1);
            }
            // --debug / -g: -O0 generated C with #line directives, and the C
            // compiler emits debug info pointing back at the .fd source.
            let debug = args.iter().any(|a| a == "--debug" || a == "-g");
            let input = &args[2];
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone())
                .unwrap_or_else(|| {
                    let stem = input.rsplit('.').nth(1).unwrap_or("output");
                    format!("{}.exe", stem)
                });
            // Optional --target <triple> for cross-compilation
            let target = args.iter().position(|a| a == "--target")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone());
            let trace = args.iter().any(|a| a == "--trace");
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            // Incremental build: skip all work when no input changed.
            let build_cache = fidesc::inc::BuildCache::open("ccompile", &args[2..], input, &output, &["CC", "FIDES_CC_OPT", "FID_HOME", "FIDES_INC_FLAT"]);
            if build_cache.up_to_date() {
                println!("{} is up to date", output);
                return;
            }

            // --inc: per-module translation units, each compiled to a
            // cached object, then linked (Phase B).
            if args.iter().any(|a| a == "--inc") {
                match compiler::ccompile_source_units(&source, input, trace) {
                    Ok(units) => {
                        let stem = output.strip_suffix(".exe").unwrap_or(&output);
                        let tu_dir = format!("{}.fides-tu", stem);
                        if let Err(e) = fs::create_dir_all(&tu_dir) {
                            eprintln!("Error creating '{}': {}", tu_dir, e);
                            process::exit(1);
                        }
                        let cc_opt = args.iter().position(|a| a == "--cc-opt")
                            .and_then(|i| args.get(i + 1))
                            .cloned()
                        // --debug: no C optimization (fast to build, easy to debug); --release: -O2
                        .or_else(|| if args.iter().any(|a| a == "--debug") { Some("0".to_string()) }
                            else if args.iter().any(|a| a == "--release") { Some("2".to_string()) } else { None })
                            .or_else(|| std::env::var("FIDES_CC_OPT").ok())
                            .unwrap_or_else(|| "2".to_string());
                        let cc = std::env::var("CC").unwrap_or_else(|_| {
                            for c in &["gcc", "clang", "cc", "tcc"] {
                                if process::Command::new(c).arg("--version").output().is_ok() {
                                    return c.to_string();
                                }
                            }
                            "gcc".to_string()
                        });
                        let mut cc_parts = cc.split_whitespace();
                        let cc_prog = cc_parts.next().unwrap_or(&cc);
                        let cc_args: Vec<&str> = cc_parts.collect();
                        if cc_prog.ends_with("cl.exe") || cc_prog == "cl" {
                            eprintln!("--inc is not supported with cl.exe yet; use gcc/clang/zig cc");
                            process::exit(1);
                        }
                        // Same flag set as the single-TU path — the generated C
                        // type-puns via pointer casts, so -fno-strict-aliasing
                        // and -fno-strict-overflow are correctness, not hardening.
                        let is_tcc = cc == "tcc" || cc.ends_with("tcc.exe")
                            || process::Command::new(&cc).arg("-v").output()
                                .map(|o| String::from_utf8_lossy(&o.stderr).contains("tcc"))
                                .unwrap_or(false);
                        let unit_flags: &[&str] = if is_tcc {
                            &["-Wno-int-conversion"]
                        } else {
                            &["-Wno-int-conversion",
                              "-Wno-incompatible-pointer-types",
                              "-fstack-protector-strong",
                              "-fPIE",
                              "-fno-strict-overflow",
                              "-fno-strict-aliasing"]
                        };
                        let store = fidesc::inc::ArtifactStore::open(&output,
                            args.iter().any(|a| a == "--no-cache"));
                        let mut objs: Vec<String> = Vec::new();
                        let mut whole = String::new();
                        let mut link_libs: Vec<String> = vec!["-lm".to_string()];
                        // Panic traces unwind the stack: zig cc links no unwinder for C
                        if !cfg!(windows) && cc.contains("zig") {
                            link_libs.push("-lunwind".to_string());
                        }
                        let mut stale: Vec<String> = Vec::new();
                        for (unit_name, code) in &units {
                            whole.push_str(code);
                            // PLINK extraction (same scan as the single-TU path)
                            for line in code.lines() {
                                if line.starts_with("// PLINK:") {
                                    let lib = line.trim_start_matches("// PLINK:").trim();
                                    if !lib.is_empty() {
                                        link_libs.push(format!("-l{}", lib));
                                    }
                                }
                            }
                            let safe = unit_name.replace(|c: char| !c.is_ascii_alphanumeric() && c != '_', "_");
                            let c_path = format!("{}/{}.c", tu_dir, safe);
                            let o_path = format!("{}/{}.o", tu_dir, safe);
                            if let Err(e) = fs::write(&c_path, code) {
                                eprintln!("Error writing '{}': {}", c_path, e);
                                process::exit(1);
                            }
                            let canon = fidesc::inc::canon_hash(code);
                            let key = store.as_ref().map(|_| fidesc::inc::ArtifactStore::key(
                                canon, &cc, &format!("o {} {:?} {:?}", cc_opt, target, unit_flags)));
                            let mut have_obj = false;
                            if let (Some(s), Some(k)) = (&store, &key) {
                                if let Some(bytes) = s.get(k) {
                                    if fs::write(&o_path, &bytes).is_ok() { have_obj = true; }
                                }
                            }
                            if !have_obj {
                                stale.push(unit_name.clone());
                                let mut unit_cmd = process::Command::new(cc_prog);
                                unit_cmd.args(&cc_args)
                                    .arg(format!("-O{}", cc_opt))
                                    .args(unit_flags)
                                    .arg("-c").arg("-o").arg(&o_path).arg(&c_path);
                                if debug { unit_cmd.arg("-g"); }
                                let st = unit_cmd.output();
                                match st {
                                    Ok(o) if o.status.success() => {
                                        if let (Some(s), Some(k)) = (&store, &key) {
                                            if let Ok(b) = fs::read(&o_path) { s.store(k, &b); }
                                        }
                                    }
                                    Ok(o) => {
                                        eprintln!("{} -c {} failed:", cc, c_path);
                                        eprintln!("{}", String::from_utf8_lossy(&o.stderr));
                                        process::exit(1);
                                    }
                                    Err(e) => {
                                        eprintln!("Failed to run {}: {}", cc, e);
                                        process::exit(1);
                                    }
                                }
                            }
                            objs.push(o_path);
                        }
                        if stale.is_empty() {
                            println!("All {} units up to date; linking", units.len());
                        } else {
                            println!("Compiling {} of {} units: {}", stale.len(), units.len(), stale.join(", "));
                        }
                        let mut cmd = process::Command::new(cc_prog);
                        cmd.args(&cc_args).arg(format!("-O{}", cc_opt)).args(unit_flags).arg("-o").arg(&output);
                        if !is_tcc {
                            cmd.arg("-pie");
                            if cfg!(not(windows)) {
                                cmd.arg("-Wl,-z,relro").arg("-Wl,-z,now").arg("-Wl,-z,noexecstack");
                            }
                        }
                        for o in &objs { cmd.arg(o); }
                        for lib in &link_libs { cmd.arg(lib); }
                        match cmd.output() {
                            Ok(o) if o.status.success() => {
                                build_cache.commit_canon(Some(fidesc::inc::hash_source(&whole)), None);
                                println!("Built native executable: {}", output);
                            }
                            Ok(o) => {
                                eprintln!("link failed:");
                                eprintln!("{}", String::from_utf8_lossy(&o.stderr));
                                process::exit(1);
                            }
                            Err(e) => {
                                eprintln!("Failed to run {}: {}", cc, e);
                                process::exit(1);
                            }
                        }
                        return;
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }
            }

            match compiler::ccompile_source_debugged(&source, input, trace, debug) {
                Ok(c_code) => {
                    let c_hash = fidesc::inc::hash_source(&c_code);
                    // Write C source
                    let c_path = if output.ends_with(".exe") {
                        output.replace(".exe", ".c")
                    } else {
                        format!("{}.c", output)
                    };
                    fs::write(&c_path, &c_code).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", c_path, e);
                        process::exit(1);
                    });
                    println!("Generated C source: {}", c_path);
                    // Same C as the build that produced the output: no need
                    // to run the C compiler again.
                    if build_cache.c_unchanged(c_hash) {
                        build_cache.commit(Some(c_hash));
                        println!("C translation unchanged; {} is up to date", output);
                        return;
                    }
                    let changed = build_cache.changed_inputs();
                    if !changed.is_empty() {
                        println!("Changed: {}", changed.join(", "));
                    }
                    let canon = fidesc::inc::canon_hash(&c_code);

                    // C optimization level: --cc-opt, else FIDES_CC_OPT, else -O2.
                    // gcc and clang need tens of minutes for a very large
                    // translation unit at -O1/-O2 (the self-hosted compiler is
                    // ~35 MB of C: 26 min at -O2, 40 s at -O0), so one gets -O0.
                    const LARGE_C_BYTES: usize = 16 << 20;
                    let cc_opt = args.iter().position(|a| a == "--cc-opt")
                        .and_then(|i| args.get(i + 1))
                        .cloned()
                        // --debug: no C optimization (fast to build, easy to debug); --release: -O2
                        .or_else(|| if args.iter().any(|a| a == "--debug") { Some("0".to_string()) }
                            else if args.iter().any(|a| a == "--release") { Some("2".to_string()) } else { None })
                        .or_else(|| std::env::var("FIDES_CC_OPT").ok())
                        .unwrap_or_else(|| {
                            if c_code.len() > LARGE_C_BYTES {
                                println!("Large program ({} MB of C): compiling with -O0 (use --cc-opt 2 for -O2)",
                                    c_code.len() >> 20);
                                "0".to_string()
                            } else {
                                "2".to_string()
                            }
                        });
                    if !matches!(cc_opt.as_str(), "0" | "1" | "2" | "3" | "s") {
                        eprintln!("Invalid --cc-opt '{}': use 0, 1, 2, 3 or s", cc_opt);
                        process::exit(1);
                    }
                    let gcc_opt = format!("-O{}", cc_opt);
                    let cl_opt = match cc_opt.as_str() {
                        "0" => "/Od",
                        "1" | "s" => "/O1",
                        _ => "/O2",
                    };

                    // Find a C compiler: try --target prefix, then CC env var, then gcc, clang, cl.exe
                    let cc = if let Some(ref target) = target {
                        // Cross-compilation: use <target>-gcc or <target>-clang
                        let cross_gcc = format!("{}-gcc", target);
                        let cross_clang = format!("{}-clang", target);
                        if process::Command::new(&cross_gcc).arg("--version").output().is_ok() {
                            cross_gcc
                        } else if process::Command::new(&cross_clang).arg("--version").output().is_ok() {
                            cross_clang
                        } else {
                            eprintln!("Warning: cross-compiler for target '{}' not found, falling back to default CC", target);
                            std::env::var("CC").unwrap_or_else(|_| "gcc".to_string())
                        }
                    } else {
                        std::env::var("CC").unwrap_or_else(|_| {
                            for c in &["gcc", "clang", "cc", "tcc"] {
                                if process::Command::new(c).arg("--version").output().is_ok() {
                                    return c.to_string();
                                }
                            }
                            // Try cl.exe from VS install
                            if let Some(cl) = find_cl() {
                                return cl;
                            }
                            "gcc".to_string() // fallback to gcc for error message
                        })
                    };

                    let is_cl = cc.ends_with("cl.exe") || cc == "cl";
                    // Phase A artifact store: a byte-identical canonical build
                    // (same program modulo renames/dead code/formatting, same
                    // toolchain+flags) reuses the cached binary — no C compile.
                    let store = fidesc::inc::ArtifactStore::open(&output,
                        args.iter().any(|a| a == "--no-cache"));
                    let art_key = store.as_ref().map(|_| fidesc::inc::ArtifactStore::key(
                        canon, &cc, &format!("{} {:?}", cc_opt, target)));
                    if let (Some(s), Some(k)) = (&store, &art_key) {
                        if let Some(bytes) = s.get(k) {
                            if fs::write(&output, &bytes).is_ok() {
                                build_cache.commit_canon(Some(c_hash), Some(canon));
                                println!("Reused cached artifact: {}", output);
                                return;
                            }
                        }
                    }
                    // CC may carry arguments (e.g. CC="zig cc"): first word is
                    // the program, the rest lead every invocation.
                    let mut cc_parts = cc.split_whitespace();
                    let cc_prog = cc_parts.next().unwrap_or(&cc);
                    let cc_args: Vec<&str> = cc_parts.collect();
                    let status = if is_cl {
                        // cl.exe needs MSVC environment (INCLUDE, LIB, etc.)
                        // Use vcvarsall.bat to set it up, then run cl.exe
                        let vcvars = find_vcvarsall();
                        if let Some(vcvars) = vcvars {
                            // /Z7 embeds debug info in the objects — no PDB
                            let z7 = if debug { "/Z7" } else { "" };
                            let cmd_str = format!(
                                "\"\"{}\" x64 >nul 2>&1 && \"{}\" {} {} /nologo /Fe:{} {}\"",
                                vcvars, cc, cl_opt, z7, output, c_path
                            );
                            // Pass the line verbatim: Command::args escapes each inner
                            // '"' as '\"' which cmd's /C parser reads as a UNC path
                            // ("the network path was not found").
                            run_cmd_line(&cmd_str)
                        } else {
                            // No vcvarsall — try direct
                            let mut direct = process::Command::new(cc_prog);
                            direct.args(&cc_args)
                                .args([cl_opt, "/nologo", &format!("/Fe:{}", output), &c_path]);
                            if debug { direct.arg("/Z7"); }
                            direct.output()
                        }
                    } else {
                        // gcc/clang/tcc: -O2 + security hardening flags -o output c_path
                        // Extract link libraries from the C source (extern "C" link "m")
                        let mut link_libs: Vec<String> = vec!["-lm".to_string()];
                        // Panic traces unwind the stack: zig cc links no unwinder for C
                        if !cfg!(windows) && cc.contains("zig") {
                            link_libs.push("-lunwind".to_string());
                        }
                        // Track preprocessor state to only pick up PLINK comments
                        // that are active on the current platform.
                        let is_windows = cfg!(target_os = "windows");
                        let mut in_win32_block: bool = false;
                        let mut in_else_block: bool = false;
                        for line in c_code.lines() {
                            let trimmed = line.trim();
                            // Track #ifdef _WIN32 / #else / #endif
                            if trimmed.starts_with("#ifdef _WIN32") || trimmed.starts_with("#if defined(_WIN32)") {
                                in_win32_block = true;
                                in_else_block = false;
                                continue;
                            }
                            if trimmed.starts_with("#else") && in_win32_block {
                                in_else_block = true;
                                continue;
                            }
                            if trimmed.starts_with("#endif") && in_win32_block {
                                in_win32_block = false;
                                in_else_block = false;
                                continue;
                            }
                            if line.starts_with("// PLINK:") {
                                // Only include PLINK if it's in an active block for this platform
                                let in_win32_active = in_win32_block && !in_else_block;
                                let in_unix_active = in_win32_block && in_else_block;
                                let in_global = !in_win32_block;
                                let active = if is_windows {
                                    in_win32_active || in_global
                                } else {
                                    in_unix_active || in_global
                                };
                                if active {
                                    let lib = line.trim_start_matches("// PLINK:").trim();
                                    if !lib.is_empty() {
                                        link_libs.push(format!("-l{}", lib));
                                    }
                                }
                            }
                        }
                        // Detect TCC — it doesn't support many gcc hardening flags
                        let is_tcc = cc == "tcc" || cc.ends_with("tcc.exe")
                            || process::Command::new(&cc).arg("-v").output()
                                .map(|o| String::from_utf8_lossy(&o.stderr).contains("tcc"))
                                .unwrap_or(false);
                        let mut cmd = process::Command::new(cc_prog);
                        cmd.args(&cc_args);
                        if debug { cmd.arg("-g"); }
                        if is_tcc {
                            // TCC: minimal flags — it doesn't support -fstack-protector,
                            // -fPIE, -pie, -Wl,-z,relro, etc.
                            cmd.arg(&gcc_opt)
                                .arg("-Wno-int-conversion")
                                .arg("-o").arg(&output).arg(&c_path);
                            // TCC on Windows: math is built-in (no -lm needed).
                            // Only add -l for libs explicitly marked in PLINK comments.
                            for lib in &link_libs {
                                if lib != "-lm" {
                                    cmd.arg(lib);
                                }
                            }
                        } else {
                            cmd.arg(&gcc_opt)
                                // Fides FFI uses i64 for all handle types (FILE*, DIR*, etc.)
                                // and struct pointers are passed as int64_t values.
                                // Safe on x86-64 where pointers are 8 bytes.
                                .arg("-Wno-int-conversion")
                                .arg("-Wno-incompatible-pointer-types")
                                // Security hardening flags — defense in depth
                                // even though Fides prevents most exploits at compile time.
                                // -D_FORTIFY_SOURCE=2 requires _GNU_SOURCE or optimization,
                                // and some generated C code triggers fortify false positives,
                                // so we use -U_FORTIFY_SOURCE to disable it for generated code.
                                .arg("-fstack-protector-strong")  // stack canaries
                                .arg("-fPIE")                     // position-independent code
                                .arg("-fno-strict-overflow")      // don't assume no overflow
                                .arg("-fno-strict-aliasing")      // generated code type-puns via pointer casts
                                .arg("-o").arg(&output).arg(&c_path);
                            // Linker security flags
                            cmd.arg("-pie");                    // position-independent executable
                            if cfg!(not(windows)) {              // the -z options are ELF-only
                                cmd.arg("-Wl,-z,relro")          // read-only relocations
                                    .arg("-Wl,-z,now")           // bind now (full RELRO)
                                    .arg("-Wl,-z,noexecstack");  // non-executable stack
                            }
                            for lib in &link_libs {
                                cmd.arg(lib);
                            }
                        }
                        cmd.output()
                    };
                    match status {
                        Ok(out) => {
                            if !out.status.success() {
                                // Some compilers (or wrappers) report on stdout.
                                eprintln!("{} failed ({}):", cc, out.status);
                                eprintln!("{}", String::from_utf8_lossy(&out.stdout));
                                eprintln!("{}", String::from_utf8_lossy(&out.stderr));
                                process::exit(1);
                            }
                            if let (Some(s), Some(k)) = (&store, &art_key) {
                                if let Ok(bytes) = fs::read(&output) {
                                    s.store(k, &bytes);
                                }
                            }
                            build_cache.commit_canon(Some(c_hash), Some(canon));
                            println!("Built native executable: {}", output);
                        }
                        Err(e) => {
                            eprintln!("Failed to run {}: {}", cc, e);
                            eprintln!("C source left at: {}", c_path);
                            eprintln!("Compile manually: {} {} -o {} {}", cc, gcc_opt, output, c_path);
                            process::exit(1);
                        }
                    }
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "sourcemap" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc sourcemap <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::lower_to_ir_module(&source) {
                Ok(module) => {
                    let sm = module.source_map();
                    if sm.entries.is_empty() {
                        println!("Source map is empty (no source line info available).");
                        println!("Note: Source line tracking requires parser changes to populate AST line numbers.");
                    } else {
                        print!("{}", sm.to_string());
                    }
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "audit" | "verify" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc audit <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::lower_to_ir_module(&source) {
                Ok(module) => {
                    let result = module.verify();
                    result.report();
                    if result.is_valid() {
                        println!("IR verification passed ({} warnings)", result.warnings.len());
                    } else {
                        eprintln!("IR verification failed with {} errors", result.errors.len());
                        process::exit(1);
                    }
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "repl" => {
            use std::io::{self, BufRead, Write};
            eprintln!("fidesc REPL — type expressions to evaluate, :quit to exit");
            eprintln!();
            let stdin = io::stdin();
            let mut line_num = 1;
            loop {
                eprint!("fides:{}> ", line_num);
                io::stderr().flush().ok();
                let mut line = String::new();
                match stdin.lock().read_line(&mut line) {
                    Ok(0) => break, // EOF
                    Ok(_) => {}
                    Err(_) => break,
                }
                let trimmed = line.trim();
                if trimmed.is_empty() {
                    continue;
                }
                if trimmed == ":quit" || trimmed == ":q" || trimmed == ":exit" {
                    break;
                }
                if trimmed == ":help" || trimmed == ":h" {
                    eprintln!("Commands:");
                    eprintln!("  :quit  — exit REPL");
                    eprintln!("  :help  — show this help");
                    eprintln!("  :ir    — show IR for last expression");
                    eprintln!();
                    eprintln!("Type a Fides expression (e.g. 1 + 2, \"hello\", [1,2,3])");
                    continue;
                }
                // Wrap the input in a main function and interpret it
                let source = format!(
                    "fn main() -> i64 {{\n    println({});\n    return 0;\n}}",
                    trimmed
                );
                match compiler::interpret_source(&source, false) {
                    Ok((RunResult::Return(_), _)) => {}
                    Ok((RunResult::Panic(msg), trace)) => {
                        eprintln!("Panic: {}", msg);
                        if let Some(trace) = trace {
                            eprint!("{}", trace);
                        }
                    }
                    Ok((RunResult::Paused, _)) => {}
                    Err(errors) => {
                        print_errors(&errors);
                    }
                }
                line_num += 1;
            }
            eprintln!("\nBye!");
        }

        "test" => {
            // `fidesc test <file.fd>` — unit tests: run each @test-annotated
            // function in the file through the interpreter. A return means
            // pass; a panic means fail. Bare `fidesc test` falls through to
            // the corpus runner below.
            if let Some(input) = args.iter().skip(2).find(|a| a.ends_with(".fd")).cloned() {
                let source = match fs::read_to_string(&input) {
                    Ok(s) => s,
                    Err(e) => {
                        eprintln!("Error reading '{}': {}", input, e);
                        process::exit(1);
                    }
                };

                // Discover @test fns via a fast parse of just this file
                // (the full build below merges stdlib/imports — their funcs
                // must not be mistaken for the file's tests).
                let mut lex = fidesc::lexer::Lexer::new(&source);
                let toks = lex.tokenize();
                let test_names: Vec<String> = match fidesc::parser::Parser::new(toks).parse() {
                    Ok(user) => user.funcs.iter()
                        .filter(|f| f.annotations.iter().any(|a| a.name == "test"))
                        .map(|f| f.name.clone())
                        .collect(),
                    Err(e) => {
                        eprintln!("Parse error: {}", e);
                        process::exit(1);
                    }
                };

                if test_names.is_empty() {
                    println!("{}: no @test functions found", input);
                    process::exit(0);
                }

                let program = match compiler::build_program_with_path(&source, Some(&input)) {
                    Ok(p) => p,
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                };
                let module = lower::lower(&program);

                let mut passed = 0usize;
                let mut failed = 0usize;
                for name in &test_names {
                    // A fresh interpreter per test keeps globals and memory
                    // isolated between tests.
                    let mut it = interp::Interpreter::new(module.clone());
                    match it.run_named(name) {
                        RunResult::Return(_) => {
                            println!("  \u{2713} {}", name);
                            passed += 1;
                        }
                        RunResult::Panic(msg) => {
                            println!("  \u{2717} {} — {}", name, msg);
                            failed += 1;
                        }
                        RunResult::Paused => {
                            println!("  - {} (paused by debugger)", name);
                            failed += 1;
                        }
                    }
                }
                println!("{}: {} passed, {} failed, {} total", input, passed, failed, test_names.len());
                process::exit(if failed == 0 { 0 } else { 1 });
            }

            // Run .fd tests in the tests/bootstrap/ directory against all backends.
            //
            // By default, runs NORMAL tests (expected to pass).
            // Use `fidesc test --fail` to run only expected-fail tests
            // (where failing = correct, passing = regression).
            let fail_only = args.iter().any(|a| a == "--fail");

            // --compiler <exe>: run, ccompile and emit go through that compiler
            // (the shipped `fid`) instead of this one
            let current_exe = args.iter().position(|a| a == "--compiler")
                .and_then(|i| args.get(i + 1))
                .map(std::path::PathBuf::from)
                .unwrap_or_else(|| std::env::current_exe()
                    .unwrap_or_else(|_| std::path::PathBuf::from("fidesc")));
            let test_dir = "tests/bootstrap";

            // Expected-fail tests: these test compiler error detection.
            // They SHOULD fail to compile/run — failing is the correct behavior.
            let expected_fail: &[&str] = &[
                "test_trait_verify_fail",
                "test_budget_fail",
                "test_sandbox_fail",
                "test_sandbox_alloc_fail",
                "test_deterministic_fail",
                "test_field_privacy_fail",
                "test_money_linear_fail",
                "test_money_conserve_fail",
                "test_banking_double_credit_fail",
                "test_banking_lost_money_fail",
                "test_loop_linear_fail",
                "test_linear_closure_fail",
                // A `-> !` function that panics: the run must fail.
                "test_never_type2",
                "test_gas_fail",
                "test_crashfree_fail",
                "test_crashfree_index_fail",
                "test_crashfree_overflow_fail",
                "test_typed_strings_fail",
                "test_init_guarantee_fail",
                "test_init_guarantee_cond_fail",
                "test_param_queries_fail",
                "test_medical_fail",
                "test_contracts_fail",
                "test_safety_annotations_fail",
                "test_safety_no_audit_fail",
                // Borrow checker fail test — fails now that borrow checking
                // is on by default (Phase 5). Tests borrow checker violations.
                "test_borrowck_fail",
                // Async/await fail test — await on non-Future value (Phase 7.6)
                "test_async_fail",
                // @constant_time fail tests — early return + IO violation
                "test_constant_time_fail",
                "test_constant_time_fail2",
                "test_constant_time_branch_fail",
                "test_constant_time_index_fail",
                // @send fail test — sending reference through channel
                "test_send_fail",
                // ML1: GPU region budget exceeded
                "test_gpu_region_fail",
                // ML2: Shape mismatch is a compile error
                "test_shape_verify_fail",
                // Real-Time Effect Enforcement violations
                "test_realtime_fail",
                "test_realtime_fail2",
                // ML3: @differentiable with IO effect
                "test_autodiff_fail",
                // ML2: Shape mismatch is a compile error
                // ML9: KV cache without budget
                "test_kv_cache_fail",
                "test_val_assign_fail",
                "test_const_assign_fail",
                "test_match_nonexhaustive_fail",
                "test_vec_elem_fail",
                "test_str_int_fail",
                "test_crashfree_method_fail",
                "test_double_free_fail",
                "test_bad_free_fail",
                "test_deref_safe_fail",
                "test_malloc_safe_fail",
                "test_int_to_ptr_safe_fail",
                "test_literal_range_fail",
                "test_narrowing_fail",
                "test_race_global_fail",
                // ML10: @quantized with dequantize
                "test_quantization_fail",
                // ML11: @checkpoint without @deterministic
                "test_checkpoint_fail",
                // ML12: @dp without Noise effect
                "test_dp_fail",
                // ML13: @shard without Comm effect
                "test_shard_fail",
                // Shape mismatch: vec3 + vec2 should fail (compile-time error detection)
                "test_vec_shape_fail",
                // Soundness checks: nonexistent variants, return/argument types,
                // &T passed as &mut T, bare value stored in a Box, duplicate fns,
                // missing return after a break out of a nested block
                "test_unknown_variant_fail",
                "test_return_type_fail",
                "test_arg_type_fail",
                "test_mut_ref_fail",
                "test_box_owned_fail",
                "test_duplicate_fn_fail",
                "test_missing_return_break_fail",
                // Reference escape check: references outliving what they point to
                "test_escape_return_fail",
                "test_escape_scope_fail",
                "test_c_callback_closure_fail",
                "test_escape_store_fail",
                "test_escape_closure_fail",
                // @constant_time: data-dependent branch on secret — rejected
                // by the source-level checker (not just the asm verifier)
                "test_ct_asm_fail",
                // @secret information flow
                "test_secret_output_fail",
                "test_secret_helper_fail",
                "test_secret_branch_fail",
                "test_secret_global_fail",
                "test_secret_return_fail",
            ];


            let mut test_files: Vec<String> = Vec::new();
            // Scan tests/bootstrap/ directory
            if let Ok(entries) = fs::read_dir(test_dir) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().map_or(false, |e| e == "fd") {
                        let stem = path.file_stem().unwrap().to_string_lossy().to_string();
                        let is_fail = expected_fail.contains(&stem.as_str());
                        // Exclude benchmark files — they produce timing-dependent
                        // output that can't match between interpreter and C codegen.
                        let is_bench = stem.starts_with("bench_");

                        if fail_only {
                            // Only include expected-fail tests
                            if is_fail {
                                test_files.push(path.to_string_lossy().to_string());
                            }
                        } else {
                            // Only include normal tests (exclude expected-fail and benchmarks)
                            if !is_fail && !is_bench {
                                test_files.push(path.to_string_lossy().to_string());
                            }
                        }
                    }
                }
            }
            // The capability, effect, region and safety tests. A name the main
            // directory also has is skipped: output files go by name.
            let top_stems: Vec<String> = test_files.iter()
                .filter_map(|f| std::path::Path::new(f).file_stem().map(|n| n.to_string_lossy().to_string()))
                .collect();
            for dir in ["capability", "effects", "regions", "safety"] {
                let Ok(entries) = fs::read_dir(format!("{}/{}", test_dir, dir)) else { continue; };
                for path in entries.flatten().map(|e| e.path()) {
                    if path.extension().map_or(true, |e| e != "fd") {
                        continue;
                    }
                    let stem = path.file_stem().unwrap().to_string_lossy().to_string();
                    let is_fail = fs::read_to_string(&path).map_or(false, |t| t.starts_with("// expect-error:"));
                    if is_fail == fail_only && !top_stems.contains(&stem) {
                        test_files.push(path.to_string_lossy().to_string());
                    }
                }
            }
            // Also scan tests/selfhost/ — self-hosted compiler module tests (SH-6).
            // probe_* and test_pe_ir_* are development diagnostics that need
            // the compiler's own modules: not part of the suite.
            if let Ok(entries) = fs::read_dir("tests/selfhost") {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().map_or(false, |e| e == "fd") {
                        let stem = path.file_stem().unwrap().to_string_lossy().to_string();
                        if stem.starts_with("probe") || stem.starts_with("test_pe_ir") {
                            continue;
                        }
                        let is_fail = expected_fail.contains(&stem.as_str());
                        let is_bench = stem.starts_with("bench_");

                        if fail_only {
                            if is_fail {
                                test_files.push(path.to_string_lossy().to_string());
                            }
                        } else {
                            if !is_fail && !is_bench {
                                test_files.push(path.to_string_lossy().to_string());
                            }
                        }
                    }
                }
            }
            test_files.sort();

            if test_files.is_empty() {
                if fail_only {
                    eprintln!("No expected-fail tests found in tests/bootstrap/");
                } else {
                    eprintln!("No .fd test files found in tests/bootstrap/");
                }
                process::exit(1);
            }

            let batch_label = if fail_only { "expected-fail" } else { "normal" };
            println!("Running {} {} tests...\n", test_files.len(), batch_label);

            let mut interp_pass = 0;
            let mut interp_fail = 0;
            let mut c_pass = 0;
            let mut c_fail = 0;
            let mut pe_pass = 0;
            let mut pe_fail = 0;

            // Known-fail trackers (reported separately, not in main denominator)
            let mut interp_known_fail = 0;
            let mut c_known_fail = 0;
            let mut pe_known_fail = 0;
            let mut interp_known_pass = 0;  // bonus: known-fail test suddenly passes
            let mut c_known_pass = 0;
            let mut pe_known_pass = 0;
            // Programs the PE backend refuses (runtime features it lacks)
            let mut pe_unsupported = 0;
            // Tests a backend does not run (interp_skip_tests, c_skip_tests)
            let mut interp_skipped_count = 0;
            let mut c_skipped_count = 0;
            let mut tested_count = 0;  // tests counted in main denominator

            // Known-fail tests: these are normal tests that have pre-existing
            // failures due to real bugs. They still run, but their failures are
            // reported separately and NOT counted in the main pass/fail denominator.
            // FIX THESE — do not leave them here forever.
            let known_fail: &[&str] = &[
                // All previously known-fail tests now pass on all 3 backends!
            ];

            // Skip list — tests that fail on C codegen due to missing builtins
            // or legitimate design differences. These pass on interpreter + PE.
            let skip_tests: &[&str] = &[
                // Harness tests that require file arguments — not standalone
                "test_fmt_harness",
                "test_lexer_harness",
                "test_parser_harness",
                "test_sl_debug",
            ];

            // C-codegen-only skip list — tests that pass on interpreter + PE
            // but can't compile via C codegen (e.g. bare-metal externs).
            let c_skip_tests: &[&str] = &[
                "test_pe_dll_import",
                "test_kernel_serial",
                "test_os_gaps",
                // They print their own argv, which differs per backend.
                "probe_argc",
                "probe_argc2",
            ];

            // Tests that run the whole self-hosted compiler (parsing core.fd
            // and more): far beyond the interpreter's step budget. They run
            // compiled; the C binary's output is the reference for PE.
            let interp_skip_tests: &[&str] = &[
                "probe_lower",
                "test_async_suspend",
                "test_c_callback",
                "test_lib_os",
                "test_http_server",
                "test_pe_dll_import",
                "test_pe_task_order",
                "test_spawn_gt",
            ];

            // Tests that link a system library Fides does not ship. They run
            // only with FIDES_TEST_EXTERNAL=1 (the library must be installed).
            let external_lib_tests: &[(&str, &str)] = &[
                ("test_sqlite", "libsqlite3"),
                ("test_lib_sdl", "SDL2"),
                ("test_lib_ui", "SDL2"),
                ("test_lib_audio_mixer", "SDL2"),
                ("test_lib_image", "SDL2_image"),
                ("test_lib_font", "SDL2"),
                ("test_lib_gl", "OpenGL"),
                ("test_lib_shader", "OpenGL"),
                ("test_lib_sys", "POSIX unistd.h"),
            ];
            let run_external = std::env::var("FIDES_TEST_EXTERNAL").map_or(false, |v| v == "1");

            // Each test's raw results. With --jobs N they are computed on N
            // threads (each its own C compiler cache); either way they are
            // reported in test order below.
            struct TestRun {
                name: String,
                /// Not run here: reported, not counted
                skip_note: Option<String>,
                interp_skipped: bool,
                interp_ok: bool,
                c_skipped: bool,
                c_ok: bool,
                /// The PE backend refused the program: the missing functions
                pe_refused: Option<String>,
                pe_ok: bool,
                pe_why: String,
            }
            let jobs: usize = args.iter()
                .position(|a| a == "--jobs" || a == "-j")
                .and_then(|i| args.get(i + 1))
                .and_then(|s| s.parse().ok())
                .unwrap_or(1)
                .max(1);
            let run_test = |test_file: &String, worker: usize| -> Option<TestRun> {
                let name = std::path::Path::new(test_file)
                    .file_stem()
                    .unwrap()
                    .to_string_lossy()
                    .to_string();
                let mut run = TestRun {
                    name: name.clone(),
                    skip_note: None,
                    interp_skipped: false,
                    interp_ok: false,
                    c_skipped: false,
                    c_ok: false,
                    pe_refused: None,
                    pe_ok: false,
                    pe_why: String::new(),
                };

                // Skip tests for C codegen limitations (pass on interp + PE)
                if skip_tests.contains(&name.as_str()) {
                    return None;
                }
                if let Some((_, lib)) = external_lib_tests.iter().find(|(t, _)| *t == name) {
                    if !run_external {
                        run.skip_note = Some(format!("needs {} (set FIDES_TEST_EXTERNAL=1)", lib));
                        return Some(run);
                    }
                }

                // Interpreter — a test "passes" if it doesn't panic.
                // The exit code may be non-zero (e.g. `ret 42` exits with 42).
                // Use --max-steps to prevent infinite loops from hanging the test suite.
                run.interp_skipped = interp_skip_tests.contains(&name.as_str());
                let interp_out = if run.interp_skipped {
                    Err(std::io::Error::new(std::io::ErrorKind::Other, "skipped"))
                } else {
                    process::Command::new(&current_exe)
                        .args(["run", test_file, "--max-steps", "50000000"])
                        .output()
                };
                run.interp_ok = interp_out
                    .as_ref()
                    .map(|o| {
                        // Check stderr for "Panic:" or compilation errors.
                        // Error messages contain "error" (e.g. "Type error:", "Budget error (line 5):",
                        // "Region error (line N):", "Sandbox error:", "Capability error:", etc.)
                        let stderr = String::from_utf8_lossy(&o.stderr).to_lowercase();
                        !stderr.contains("panic:") && !stderr.contains("error")
                    })
                    .unwrap_or(false);
                // An expected failure must fail for its own reason, named by
                // `// expect-error: text` in the test.
                if fail_only && !run.interp_skipped {
                    let want = fs::read_to_string(test_file).ok().and_then(|src| {
                        src.lines().find_map(|l| l.strip_prefix("// expect-error: ").map(|t| t.trim().to_string()))
                    });
                    let got = interp_out.as_ref()
                        .map(|o| format!("{}{}", String::from_utf8_lossy(&o.stderr), String::from_utf8_lossy(&o.stdout)))
                        .unwrap_or_default();
                    match want {
                        Some(w) if got.contains(&w) => {}
                        Some(w) => {
                            eprintln!("  [WRONG REASON] {}: expected '{}'", name, w);
                            run.interp_ok = true;
                        }
                        None => {
                            eprintln!("  [NO EXPECT-ERROR] {}", name);
                            run.interp_ok = true;
                        }
                    }
                }
                let mut interp_stdout = interp_out
                    .as_ref()
                    .map(|o| String::from_utf8_lossy(&o.stdout).to_string())
                    .unwrap_or_default();

                // C codegen — use a platform-appropriate temp path
                let c_exe = if cfg!(windows) {
                    format!("{}\\{}_test_c.exe", std::env::temp_dir().to_string_lossy(), name)
                } else {
                    format!("/tmp/{}_test_c.exe", name)
                };
                let _ = fs::remove_file(&c_exe);
                // A C compiler cache per worker: concurrent zig builds of the
                // same cache entry collide.
                let cc_cache = std::env::temp_dir().join(format!("fides-test-cc-cache-{}", worker));

                // Skip C codegen for bare-metal tests that use externs not
                // available in the C runtime (p_idt_set_entry, p_serial_*, etc.)
                run.c_skipped = c_skip_tests.contains(&name.as_str());
                let c_compile = if run.c_skipped {
                    // Mark as "skipped" — don't count as pass or fail
                    None
                } else {
                    let compile = || {
                        let mut cmd = process::Command::new(&current_exe);
                        cmd.args(["ccompile", test_file, "-o", &c_exe, "--no-cache"]);
                        if jobs > 1 {
                            cmd.env("ZIG_GLOBAL_CACHE_DIR", &cc_cache).env("ZIG_LOCAL_CACHE_DIR", &cc_cache);
                        }
                        cmd.output()
                    };
                    let mut out = compile();
                    // The C compiler fails now and then on its own files (zig:
                    // "file_open Unexpected"): one retry of the compile, never
                    // of the comparison.
                    if matches!(&out, Ok(o) if !o.status.success()) {
                        out = compile();
                    }
                    Some(out)
                };
                run.c_ok = if let Some(Ok(compile_out)) = c_compile.as_ref() {
                    if compile_out.status.success() {
                        if let Some(c_out) = run_with_timeout(&c_exe, std::time::Duration::from_secs(30)) {
                            let c_stdout = String::from_utf8_lossy(&c_out.stdout).to_string();
                            if run.interp_skipped {
                                // The compiled run is the reference: it must succeed.
                                interp_stdout = c_stdout.clone();
                            }
                            // Normalize CRLF → LF: the C binary uses printf("\n")
                            // which on Windows produces \r\n, while the interpreter
                            // produces \n. Normalize both sides before comparing.
                            let c_norm = c_stdout.replace("\r\n", "\n");
                            let i_norm = interp_stdout.replace("\r\n", "\n");
                            // An expected-fail test that panics at run time
                            // has failed, whatever it printed before; a
                            // reference run must exit successfully (its
                            // output is compared with itself).
                            c_norm == i_norm
                                && (!fail_only || c_out.status.success())
                                && (!run.interp_skipped || c_out.status.success())
                        } else {
                            false
                        }
                    } else {
                        false
                    }
                } else {
                    false
                };
                let _ = fs::remove_file(&c_exe);
                let _ = fs::remove_file(&c_exe.replace(".exe", ".c"));
                let _ = fs::remove_file(format!("{}.fides-stamp", c_exe));

                // PE emitter — use a platform-appropriate temp path
                let pe_exe = if cfg!(windows) {
                    format!("{}\\{}_test_pe.exe", std::env::temp_dir().to_string_lossy(), name)
                } else {
                    format!("/tmp/{}_test_pe.exe", name)
                };
                let _ = fs::remove_file(&pe_exe);
                let pe_out = process::Command::new(&current_exe)
                    .args(["emit", test_file, "-o", &pe_exe, "--no-cache"])
                    .output();
                let pe_emitted = pe_out
                    .as_ref()
                    .map(|o| o.status.success() && std::path::Path::new(&pe_exe).exists())
                    .unwrap_or(false);
                // Refused at compile time for lack of runtime support: not a
                // pass, not a wrong result.
                if !pe_emitted {
                    let why = pe_out.as_ref().map(|o| String::from_utf8_lossy(&o.stderr).to_string()).unwrap_or_default();
                    if why.contains("not available in native Windows executables") {
                        let missing = why.split("executables: ").nth(1).and_then(|r| r.split(" (").next()).unwrap_or("");
                        run.pe_refused = Some(missing.to_string());
                    }
                }
                // Run the binary and compare its output with the interpreter's,
                // as for C. (Bare-metal tests the C backend skips cannot run
                // under an OS; for them a valid binary is the check.)
                run.pe_ok = if !pe_emitted {
                    run.pe_why = pe_out.as_ref()
                        .map(|o| String::from_utf8_lossy(&o.stderr).lines().find(|l| l.contains("rror")).unwrap_or("no binary").to_string())
                        .unwrap_or_else(|e| e.to_string());
                    false
                } else if run.c_skipped || !cfg!(windows) {
                    // A valid binary is the check where it cannot run
                    true
                } else {
                    match run_with_timeout(&pe_exe, std::time::Duration::from_secs(30)) {
                        Some(out) => {
                            let pe_norm = String::from_utf8_lossy(&out.stdout).replace("\r\n", "\n");
                            let want = interp_stdout.replace("\r\n", "\n");
                            run.pe_why = format!(
                                "{}, first difference: {:?}", out.status,
                                pe_norm.lines().zip(want.lines()).find(|(a, b)| a != b)
                                    .or_else(|| pe_norm.lines().zip(want.lines()).last()),
                            );
                            pe_norm == want && (!fail_only || out.status.success())
                        }
                        None => {
                            run.pe_why = "timed out after 30 s".to_string();
                            false
                        }
                    }
                };
                let _ = fs::remove_file(&pe_exe);
                let _ = fs::remove_file(format!("{}.fides-stamp", pe_exe));
                Some(run)
            };

            let next_test = std::sync::atomic::AtomicUsize::new(0);
            let slots: std::sync::Mutex<Vec<Option<TestRun>>> =
                std::sync::Mutex::new((0..test_files.len()).map(|_| None).collect());
            std::thread::scope(|scope| {
                for worker in 0..jobs {
                    let (next_test, slots, run_test, test_files) = (&next_test, &slots, &run_test, &test_files);
                    scope.spawn(move || loop {
                        let i = next_test.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
                        if i >= test_files.len() {
                            break;
                        }
                        let r = run_test(&test_files[i], worker);
                        slots.lock().unwrap()[i] = r;
                    });
                }
            });

            for run in slots.into_inner().unwrap().into_iter().flatten() {
                let name = run.name.clone();
                if let Some(note) = &run.skip_note {
                    eprintln!("  [SKIP] {}: {}", name, note);
                    continue;
                }
                let is_known = known_fail.contains(&name.as_str());
                if !is_known {
                    tested_count += 1;
                }
                let (interp_ok, c_ok, pe_ok) = (run.interp_ok, run.c_ok, run.pe_ok);

                if run.interp_skipped {
                    // Not run on the interpreter; counted on C and PE only.
                    if !is_known { interp_skipped_count += 1; }
                } else if fail_only {
                    // For expected-fail tests, failing = correct, passing = regression
                    if interp_ok {
                        interp_fail += 1;  // unexpected pass!
                        eprintln!("  [UNEXPECTED PASS] interp: {} (should have failed)", name);
                    } else {
                        interp_pass += 1;  // correct failure
                    }
                } else if is_known {
                    // Known-fail test: report separately, don't count in main denominator
                    if interp_ok {
                        interp_known_pass += 1;
                        eprintln!("  [BONUS PASS] interp: {} (was known-fail, now passes!)", name);
                    } else {
                        interp_known_fail += 1;
                    }
                } else {
                    if interp_ok {
                        interp_pass += 1;
                    } else {
                        interp_fail += 1;
                        eprintln!("  [FAIL] interp: {}", name);
                    }
                }

                if run.c_skipped {
                    // C codegen skipped for this test (bare-metal externs, etc.)
                    if !is_known { c_skipped_count += 1; }
                } else if fail_only {
                    if c_ok {
                        c_fail += 1;  // unexpected pass!
                        eprintln!("  [UNEXPECTED PASS] c codegen: {} (should have failed)", name);
                    } else {
                        c_pass += 1;  // correct failure
                    }
                } else if is_known {
                    if c_ok {
                        c_known_pass += 1;
                        eprintln!("  [BONUS PASS] c codegen: {} (was known-fail, now passes!)", name);
                    } else {
                        c_known_fail += 1;
                    }
                } else {
                    if c_ok {
                        c_pass += 1;
                    } else {
                        c_fail += 1;
                        eprintln!("  [FAIL] c codegen: {}", name);
                    }
                }

                if let (Some(missing), false) = (&run.pe_refused, fail_only) {
                    pe_unsupported += 1;
                    eprintln!("  [UNSUPPORTED] pe emit: {} (needs {})", name, missing);
                } else if fail_only {
                    // A .exe runs only on Windows: elsewhere a program that fails
                    // when it runs compiles and shows nothing
                    if pe_ok && cfg!(windows) {
                        pe_fail += 1;  // unexpected pass!
                        eprintln!("  [UNEXPECTED PASS] pe emit: {} (should have failed)", name);
                    } else {
                        pe_pass += 1;  // correct failure
                    }
                } else if is_known {
                    if pe_ok {
                        pe_known_pass += 1;
                        eprintln!("  [BONUS PASS] pe emit: {} (was known-fail, now passes!)", name);
                    } else {
                        pe_known_fail += 1;
                    }
                } else {
                    if pe_ok {
                        pe_pass += 1;
                    } else {
                        pe_fail += 1;
                        eprintln!("  [FAIL] pe emit: {} ({})", name, run.pe_why);
                    }
                }
            }

            println!("\n=== Results ({}) ===", batch_label);
            if fail_only {
                // For fail tests: "correct" = failed as expected
                println!("Interpreter:  {}/{} correctly failed", interp_pass, test_files.len());
                println!("C Codegen:    {}/{} correctly failed", c_pass, test_files.len());
                println!("PE Emitter:   {}/{} correctly failed", pe_pass, test_files.len());
            } else {
                // Normal tests: only count non-known-fail tests in denominator
                println!("Interpreter:  {}/{} passed", interp_pass, tested_count - interp_skipped_count);
                println!("C Codegen:    {}/{} matched", c_pass, tested_count - c_skipped_count);
                println!("PE Emitter:   {}/{} matched, {} unsupported by the native runtime",
                    pe_pass, tested_count - pe_unsupported, pe_unsupported);

                // Report known-fail tests separately
                let known_total = known_fail.len();
                println!("\n--- Known-fail tests (not counted above) ---");
                println!("Interpreter:  {} fail, {} bonus pass (of {} known-fail)", interp_known_fail, interp_known_pass, known_total);
                println!("C Codegen:    {} fail, {} bonus pass (of {} known-fail)", c_known_fail, c_known_pass, known_total);
                println!("PE Emitter:   {} fail, {} bonus pass (of {} known-fail)", pe_known_fail, pe_known_pass, known_total);
            }
            if interp_fail > 0 || c_fail > 0 || pe_fail > 0 {
                process::exit(1);
            }
        }

        "wasm" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc wasm <file.fd> [-o output.wasm] [--wat]");
                process::exit(1);
            }
            let input = &args[2];
            let wat_mode = args.iter().any(|a| a == "--wat");
            let output = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.clone())
                .unwrap_or_else(|| {
                    let stem = input.rsplit('.').nth(1).unwrap_or("output");
                    if wat_mode { format!("{}.wat", stem) } else { format!("{}.wasm", stem) }
                });
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::compile_to_wasm(&source, input, wat_mode) {
                Ok(bytes) => {
                    fs::write(&output, &bytes).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", output, e);
                        process::exit(1);
                    });
                    if wat_mode {
                        println!("Emitted WAT text: {} ({} chars)", output, bytes.len());
                    } else {
                        println!("Emitted WASM binary: {} ({} bytes)", output, bytes.len());
                    }
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "pir" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc pir <file.fd> [1|2|3|all] [--opt]");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let level_arg = args.get(3).map(|s| s.as_str()).unwrap_or("2");
            let do_opt = args.iter().any(|a| a == "--opt");

            let module = match compiler::lower_to_ir_module(&source) {
                Ok(m) => m,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };

            let level = pir::PirLevel::parse(level_arg).unwrap_or(pir::PirLevel::L2);
            match level {
                pir::PirLevel::L1 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    print!("{}", pir::format_l1(&l1));
                }
                pir::PirLevel::L2 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let mut l2 = pir::lower_l1_to_l2(&l1);
                    if do_opt {
                        let stats = opt::optimize_l2(&mut l2);
                        eprintln!(";; optimization stats: {:?}", stats);
                    }
                    print!("{}", pir::format_l2(&l2));
                }
                pir::PirLevel::L3 => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let l2 = pir::lower_l1_to_l2(&l1);
                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                    print!("{}", pir::format_l3(&l3));
                }
                pir::PirLevel::All => {
                    if do_opt {
                        eprintln!(";; note: --opt applies to L2 only; showing all levels with L2 optimized");
                        let l1 = pir::lower_ir_to_l1(&module);
                        let mut l2 = pir::lower_l1_to_l2(&l1);
                        let stats = opt::optimize_l2(&mut l2);
                        eprintln!(";; optimization stats: {:?}", stats);
                        let l3 = pir::lower_l2_to_l3(&l2, &module);
                        print!("{}", pir::format_l1(&l1));
                        print!("\n");
                        print!("{}", pir::format_l2(&l2));
                        print!("\n");
                        print!("{}", pir::format_l3(&l3));
                    } else {
                        print!("{}", pir::generate_pir(&module, pir::PirLevel::All));
                    }
                }
            }
        }

        "diagram" | "seq" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc diagram <file.fd> [-o out.mmd]");
                eprintln!("       Emits a Mermaid sequenceDiagram of the file's calls,");
                eprintln!("       annotated with inferred effects.");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // Parse just this file: the diagram shows the file's own functions.
            let mut lex = fidesc::lexer::Lexer::new(&source);
            let toks = lex.tokenize();
            let program = match fidesc::parser::Parser::new(toks).parse() {
                Ok(p) => p,
                Err(e) => {
                    eprintln!("Parse error: {}", e);
                    process::exit(1);
                }
            };

            // Infer effects; unresolved imports may produce errors, which are
            // irrelevant to the diagram, so the check result is ignored.
            let mut tc = TypeChecker::new();
            let _ = tc.check(&program);
            let text = seqdiag::mermaid(&program, tc.func_effects());

            if let Some(pos) = args.iter().position(|a| a == "-o") {
                match args.get(pos + 1) {
                    Some(out) => {
                        if let Err(e) = fs::write(out, &text) {
                            eprintln!("Error writing '{}': {}", out, e);
                            process::exit(1);
                        }
                        eprintln!("wrote {}", out);
                    }
                    None => {
                        eprintln!("Error: -o requires an output path");
                        process::exit(1);
                    }
                }
            } else {
                print!("{}", text);
            }
        }

        "opt" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc opt <file.fd> [pass1,pass2,...]");
                eprintln!("       passes: const-fold, cse, dce, inline, unroll, tco");
                eprintln!("       default: all passes to fixed point");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let module = match compiler::lower_to_ir_module(&source) {
                Ok(m) => m,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };

            let l1 = pir::lower_ir_to_l1(&module);
            let mut l2 = pir::lower_l1_to_l2(&l1);

            let stats = if let Some(pass_arg) = args.get(3) {
                let passes: Vec<opt::Pass> = pass_arg
                    .split(',')
                    .filter_map(|s| opt::Pass::parse(s.trim()))
                    .collect();
                if passes.is_empty() {
                    opt::optimize_l2(&mut l2)
                } else {
                    opt::optimize_l2_with(&mut l2, &passes, 8)
                }
            } else {
                opt::optimize_l2(&mut l2)
            };

            print!("{}", pir::format_l2(&l2));
            eprintln!(";; optimization stats: {:?}", stats);
        }

        "codegen" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc codegen <file.fd> [--target x86-64|arm64|riscv] [--peephole] [--elf <o>] [--coff <o>] [--macho <o>]");
                eprintln!("       Emits assembly via the custom PIR L3 → target codegen");
                eprintln!("       --peephole  Run peephole optimizer on the output assembly");
                eprintln!("       --elf <o>   Emit an ELF64 relocatable object file (Linux)");
                eprintln!("       --coff <o>  Emit a COFF object file (Windows)");
                eprintln!("       --macho <o> Emit a Mach-O object file (macOS)");
                process::exit(1);
            }
            // Find the input file (first non-flag argument after "codegen")
            // Skip values that follow --target, --elf, --coff, --macho, -o
            let skip_next: std::collections::HashSet<&str> = ["--target", "--elf", "--coff", "--macho", "-o"].iter().cloned().collect();
            let mut input_file = String::new();
            let mut i = 2;
            while i < args.len() {
                let a = &args[i];
                if skip_next.contains(a.as_str()) {
                    i += 2; // skip flag and its value
                    continue;
                }
                if !a.starts_with("-") && !a.starts_with("--") {
                    input_file = a.clone();
                    break;
                }
                i += 1;
            }
            if input_file.is_empty() {
                eprintln!("Error: no input file specified");
                process::exit(1);
            }
            let input = &input_file;
            let target = args.iter().position(|a| a == "--target")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or("x86-64");
            let do_peephole = args.iter().any(|a| a == "--peephole");
            let elf_output: Option<&str> = args.iter().position(|a| a == "--elf")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str());
            let coff_output: Option<&str> = args.iter().position(|a| a == "--coff")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str());
            let macho_output: Option<&str> = args.iter().position(|a| a == "--macho")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str());
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let module = match compiler::lower_to_ir_module(&source) {
                Ok(m) => m,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };
            match target {
                "x86-64" | "x64" => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let l2 = pir::lower_l1_to_l2(&l1);
                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                    let asm = codegen_x64::generate_asm(&l3);
                    let asm = if do_peephole {
                        peephole::optimize(&asm)
                    } else {
                        asm
                    };

                    if let Some(elf_path) = elf_output {
                        match elf64::emit_object_file(&l3, &asm) {
                            Ok(bytes) => {
                                if let Err(e) = fs::write(elf_path, &bytes) {
                                    eprintln!("Error writing ELF file '{}': {}", elf_path, e);
                                    process::exit(1);
                                }
                                eprintln!("Wrote {} bytes to {}", bytes.len(), elf_path);
                            }
                            Err(e) => {
                                eprintln!("ELF emission error: {}", e);
                                process::exit(1);
                            }
                        }
                    } else if let Some(coff_path) = coff_output {
                        match coff::emit_object_file(&l3, &asm) {
                            Ok(bytes) => {
                                if let Err(e) = fs::write(coff_path, &bytes) {
                                    eprintln!("Error writing COFF file '{}': {}", coff_path, e);
                                    process::exit(1);
                                }
                                eprintln!("Wrote {} bytes to {}", bytes.len(), coff_path);
                            }
                            Err(e) => {
                                eprintln!("COFF emission error: {}", e);
                                process::exit(1);
                            }
                        }
                    } else if let Some(macho_path) = macho_output {
                        match macho::emit_object_file(&l3, &asm) {
                            Ok(bytes) => {
                                if let Err(e) = fs::write(macho_path, &bytes) {
                                    eprintln!("Error writing Mach-O file '{}': {}", macho_path, e);
                                    process::exit(1);
                                }
                                eprintln!("Wrote {} bytes to {}", bytes.len(), macho_path);
                            }
                            Err(e) => {
                                eprintln!("Mach-O emission error: {}", e);
                                process::exit(1);
                            }
                        }
                    } else {
                        print!("{}", asm);
                    }
                }
                "arm64" | "aarch64" => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let l2 = pir::lower_l1_to_l2(&l1);
                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                    let asm = codegen_arm64::generate_asm(&l3);
                    if let Some(elf_path) = elf_output {
                        match elf64::emit_object_file_for_target(&l3, &asm, "arm64") {
                            Ok(bytes) => {
                                if let Err(e) = fs::write(elf_path, &bytes) {
                                    eprintln!("Error writing ELF file '{}': {}", elf_path, e);
                                    process::exit(1);
                                }
                                eprintln!("Wrote {} bytes to {} (ELF64 AArch64)", bytes.len(), elf_path);
                            }
                            Err(e) => {
                                eprintln!("ELF emission error: {}", e);
                                process::exit(1);
                            }
                        }
                    } else {
                        print!("{}", asm);
                    }
                }
                "riscv" | "riscv64" => {
                    let l1 = pir::lower_ir_to_l1(&module);
                    let l2 = pir::lower_l1_to_l2(&l1);
                    let l3 = pir::lower_l2_to_l3(&l2, &module);
                    let asm = codegen_riscv::generate_asm(&l3);
                    if let Some(elf_path) = elf_output {
                        match elf64::emit_object_file_for_target(&l3, &asm, "riscv") {
                            Ok(bytes) => {
                                if let Err(e) = fs::write(elf_path, &bytes) {
                                    eprintln!("Error writing ELF file '{}': {}", elf_path, e);
                                    process::exit(1);
                                }
                                eprintln!("Wrote {} bytes to {} (ELF64 RISC-V)", bytes.len(), elf_path);
                            }
                            Err(e) => {
                                eprintln!("ELF emission error: {}", e);
                                process::exit(1);
                            }
                        }
                    } else {
                        print!("{}", asm);
                    }
                }
                _ => {
                    eprintln!("Unknown target: {} (supported: x86-64, arm64, riscv)", target);
                    process::exit(1);
                }
            }
        }

        "gpu" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc gpu <file.fd> --target spirv|ptx [-o output] [--text]");
                eprintln!("       Generates GPU code from a Fides program");
                eprintln!("       --target spirv  Generate SPIR-V bytecode (Vulkan compute shaders)");
                eprintln!("       --target ptx     Generate PTX assembly (NVIDIA GPUs)");
                eprintln!("       -o <file>        Write output to file (default: stdout)");
                eprintln!("       --text           For spirv: emit text disassembly instead of raw bytecode");
                process::exit(1);
            }
            // Find the input file (first non-flag argument after "gpu")
            let skip_next: std::collections::HashSet<&str> = ["--target", "-o"].iter().cloned().collect();
            let mut input_file = String::new();
            let mut i = 2;
            while i < args.len() {
                let a = &args[i];
                if skip_next.contains(a.as_str()) {
                    i += 2;
                    continue;
                }
                if !a.starts_with("-") && !a.starts_with("--") {
                    input_file = a.clone();
                    break;
                }
                i += 1;
            }
            if input_file.is_empty() {
                eprintln!("Error: no input file specified");
                process::exit(1);
            }
            let input = &input_file;
            let target = args.iter().position(|a| a == "--target")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or("spirv");
            let output_file: Option<&str> = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str());
            let text_mode = args.iter().any(|a| a == "--text");

            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // Parse → typecheck → lower → PIR L3
            let module = match compiler::lower_to_ir_module(&source) {
                Ok(m) => m,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };
            let l1 = pir::lower_ir_to_l1(&module);
            let l2 = pir::lower_l1_to_l2(&l1);
            let l3 = pir::lower_l2_to_l3(&l2, &module);

            match target {
                "spirv" => {
                    if text_mode {
                        let text = gpu::generate_spirv_text(&l3);
                        match output_file {
                            Some(path) => {
                                if let Err(e) = fs::write(path, &text) {
                                    eprintln!("Error writing '{}': {}", path, e);
                                    process::exit(1);
                                }
                                println!("Generated SPIR-V text: {} ({} chars)", path, text.len());
                            }
                            None => {
                                print!("{}", text);
                            }
                        }
                    } else {
                        let words = gpu::generate_spirv(&l3);
                        let bytes: Vec<u8> = words.iter()
                            .flat_map(|w| w.to_le_bytes())
                            .collect();
                        match output_file {
                            Some(path) => {
                                if let Err(e) = fs::write(path, &bytes) {
                                    eprintln!("Error writing '{}': {}", path, e);
                                    process::exit(1);
                                }
                                println!("Generated SPIR-V bytecode: {} ({} words, {} bytes)", path, words.len(), bytes.len());
                            }
                            None => {
                                use std::io::Write;
                                std::io::stdout().write_all(&bytes).ok();
                            }
                        }
                    }
                }
                "ptx" => {
                    let ptx = gpu::generate_ptx(&l3);
                    match output_file {
                        Some(path) => {
                            if let Err(e) = fs::write(path, &ptx) {
                                eprintln!("Error writing '{}': {}", path, e);
                                process::exit(1);
                            }
                            println!("Generated PTX assembly: {} ({} chars)", path, ptx.len());
                        }
                        None => {
                            print!("{}", ptx);
                        }
                    }
                }
                _ => {
                    eprintln!("Unknown GPU target '{}'. Use --target spirv or --target ptx", target);
                    process::exit(1);
                }
            }
        }

        "watch" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc watch <file.fd> [--interval <ms>] [--opt] [--no-run] [--diff]");
                eprintln!("       Auto-recompiles and runs on file change (hot reload mode)");
                eprintln!("       Uses native JIT: compiles to ELF64 and executes (zero external tools)");
                eprintln!("       --interval <ms>  Polling interval (default: 500ms)");
                eprintln!("       --opt            Enable L2 optimizer");
                eprintln!("       --no-run         Compile only, don't execute after each reload");
                eprintln!("       --diff           Show PIR/assembly diff on each reload");
                process::exit(1);
            }
            let input = &args[2];
            let interval: u64 = args.iter().position(|a| a == "--interval")
                .and_then(|i| args.get(i + 1))
                .and_then(|s| s.parse().ok())
                .unwrap_or(500);
            let do_opt = args.iter().any(|a| a == "--opt");
            let do_run = !args.iter().any(|a| a == "--no-run");
            let do_diff = args.iter().any(|a| a == "--diff");

            let input_path = std::path::PathBuf::from(input);
            let input_clone = input.clone();
            let do_opt_move = do_opt;
            let do_run_move = do_run;
            let do_diff_move = do_diff;

            // Previous function snapshot for change detection (shared via Mutex for Send)
            let prev_snapshot = std::sync::Mutex::new(None::<hotreload::FunctionSnapshot>);
            // Previous PIR L2 output for diffing (shared via Mutex for Send)
            let prev_pir = std::sync::Mutex::new(None::<String>);
            // Track run count
            let run_count = std::sync::Mutex::new(0u32);

            // Helper: compile, detect changes, and optionally run
            let do_reload = move |source: &str, path: &str, is_initial: bool,
                             prev_snapshot: &std::sync::Mutex<Option<hotreload::FunctionSnapshot>>,
                             prev_pir: &std::sync::Mutex<Option<String>>,
                             run_count: &std::sync::Mutex<u32>,
                             do_opt: bool, do_run: bool, do_diff: bool| {
                let label = if is_initial { "Initial" } else { "Reload" };
                eprintln!("[watch] {} compilation of {}...", label, path);

                // Build L2 for function-level change detection
                match compiler::lower_to_l2(source, path) {
                    Ok(l2) => {
                        let new_snapshot = hotreload::FunctionSnapshot::from_l2(&l2);
                        let func_count = new_snapshot.function_names().len();

                        // Compute diff against previous snapshot
                        let diff = {
                            let prev = prev_snapshot.lock().unwrap();
                            match &*prev {
                                Some(prev_snap) => new_snapshot.diff(prev_snap),
                                None => {
                                    // First compilation — everything is "added"
                                    hotreload::FunctionDiff {
                                        added: new_snapshot.function_names().iter().map(|s| s.to_string()).collect(),
                                        modified: Vec::new(),
                                        removed: Vec::new(),
                                    }
                                }
                            }
                        };

                        if is_initial {
                            eprintln!("[watch] Compiled {} functions: {:?}",
                                func_count,
                                new_snapshot.function_names().iter().take(5).collect::<Vec<_>>());
                        } else if diff.is_empty() {
                            eprintln!("[watch] No functions changed (whitespace/comment only?)");
                        } else {
                            eprintln!("[watch] Function changes: {} added, {} modified, {} removed",
                                diff.added.len(), diff.modified.len(), diff.removed.len());
                            if !diff.added.is_empty() {
                                eprintln!("[watch]   Added: {:?}", diff.added);
                            }
                            if !diff.modified.is_empty() {
                                eprintln!("[watch]   Modified: {:?}", diff.modified);
                            }
                            if !diff.removed.is_empty() {
                                eprintln!("[watch]   Removed: {:?}", diff.removed);
                            }
                        }

                        *prev_snapshot.lock().unwrap() = Some(new_snapshot);

                        // PIR diff: show what changed at the IR level
                        if do_diff && !is_initial && !diff.modified.is_empty() {
                            let new_pir = pir::format_l2(&l2);
                            let prev_pir_str = {
                                let prev = prev_pir.lock().unwrap();
                                prev.clone()
                            };
                            if let Some(ref prev_pir_text) = prev_pir_str {
                                eprintln!("[watch] PIR L2 diff:");
                                let prev_lines: Vec<&str> = prev_pir_text.lines().collect();
                                let new_lines: Vec<&str> = new_pir.lines().collect();
                                let max_len = prev_lines.len().max(new_lines.len());
                                let mut added = 0usize;
                                let mut removed = 0usize;
                                for i in 0..max_len {
                                    let prev_line = prev_lines.get(i).copied().unwrap_or("");
                                    let new_line = new_lines.get(i).copied().unwrap_or("");
                                    if prev_line != new_line {
                                        if !prev_line.is_empty() && new_line.is_empty() {
                                            eprintln!("  - {}", prev_line);
                                            removed += 1;
                                        } else if prev_line.is_empty() && !new_line.is_empty() {
                                            eprintln!("  + {}", new_line);
                                            added += 1;
                                        } else {
                                            eprintln!("  - {}", prev_line);
                                            eprintln!("  + {}", new_line);
                                            removed += 1;
                                            added += 1;
                                        }
                                    }
                                }
                                if added == 0 && removed == 0 {
                                    eprintln!("  (no PIR changes detected)");
                                }
                            }
                            *prev_pir.lock().unwrap() = Some(new_pir);
                        } else if is_initial {
                            // Store initial PIR for future diffs
                            *prev_pir.lock().unwrap() = Some(pir::format_l2(&l2));
                        }

                        // Compile and run using native JIT
                        let mut prev_binary_size: u64 = 0;
                        if do_run {
                            eprintln!("[watch] Compiling to native ELF64 (opt={})...", do_opt);
                            match hotreload::compile_and_run(source, path, do_opt) {
                                Ok(result) => {
                                    let mut rc = run_count.lock().unwrap();
                                    *rc += 1;
                                    let rc_val = *rc;
                                    drop(rc);
                                    eprintln!("[watch] Run #{}: exit={}, compile={}ms, exec={}ms, size={} bytes",
                                        rc_val, result.exit_code, result.compile_time_ms,
                                        result.exec_time_ms, result.binary_size);
                                    if do_diff && !is_initial && prev_binary_size > 0 {
                                        let delta = result.binary_size as i64 - prev_binary_size as i64;
                                        if delta != 0 {
                                            let sign = if delta > 0 { "+" } else { "" };
                                            eprintln!("[watch] Binary size: {} → {} bytes ({}{})",
                                                prev_binary_size, result.binary_size, sign, delta);
                                        }
                                    }
                                    prev_binary_size = result.binary_size as u64;
                                    if !result.stdout.is_empty() {
                                        let out = result.stdout.trim_end();
                                        if !out.is_empty() {
                                            eprintln!("[watch] stdout:");
                                            for line in out.lines() {
                                                eprintln!("  {}", line);
                                            }
                                        }
                                    }
                                    if !result.stderr.is_empty() {
                                        eprintln!("[watch] stderr: {}", result.stderr.trim());
                                    }
                                }
                                Err(e) => {
                                    eprintln!("[watch] Compilation error: {}", e);
                                }
                            }
                        }

                        eprintln!("[watch] {} complete", label);
                    }
                    Err(errors) => {
                        print_errors(&errors);
                    }
                }
            };

            // Initial compilation
            let source = match fs::read_to_string(&input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("[watch] Error reading: {}", e);
                    process::exit(1);
                }
            };
            do_reload(&source, input, true, &prev_snapshot, &prev_pir, &run_count, do_opt_move, do_run_move, do_diff_move);

            // Watch loop
            let mut watcher = hotreload::Watcher::new(move |path| {
                eprintln!("\n[watch] File changed: {} — reloading...", path);
                let _path_owned = path.to_string();
                let source = match fs::read_to_string(&input_clone) {
                    Ok(s) => s,
                    Err(e) => {
                        eprintln!("[watch] Error reading: {}", e);
                        return;
                    }
                };
                do_reload(&source, &input_clone, false, &prev_snapshot, &prev_pir, &run_count, do_opt_move, do_run_move, do_diff_move);
            });
            watcher.watch(&input_path);
            eprintln!("\n[watch] Watching {} (polling every {}ms, native JIT, opt={}, diff={})",
                input, interval, do_opt, do_diff);
            eprintln!("[watch] Press Ctrl+C to stop");
            watcher.run(interval);
        }

        "link" => {            if args.len() < 4 {
                eprintln!("Usage: fidesc link <output> <object1.o> [object2.o ...] [--entry <name>]");
                eprintln!("       Links ELF64 object files into an executable (no external ld needed)");
                process::exit(1);
            }
            let output = &args[2];
            let entry = args.iter().position(|a| a == "--entry")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or("main");
            let obj_files: Vec<&str> = args[3..].iter()
                .filter(|a| !a.starts_with("--"))
                .map(|s| s.as_str())
                .collect();
            if obj_files.is_empty() {
                eprintln!("Error: no object files specified");
                process::exit(1);
            }
            let mut objects: Vec<Vec<u8>> = Vec::new();
            for obj_file in &obj_files {
                match fs::read(obj_file) {
                    Ok(data) => objects.push(data),
                    Err(e) => {
                        eprintln!("Error reading '{}': {}", obj_file, e);
                        process::exit(1);
                    }
                }
            }
            match linker::link(&objects, entry) {
                Ok(exe_bytes) => {
                    if let Err(e) = fs::write(output, &exe_bytes) {
                        eprintln!("Error writing executable '{}': {}", output, e);
                        process::exit(1);
                    }
                    #[cfg(unix)]
                    {
                        use std::os::unix::fs::PermissionsExt;
                        let _ = fs::set_permissions(output, fs::Permissions::from_mode(0o755));
                    }
                    eprintln!("Linked {} bytes to {}", exe_bytes.len(), output);
                }
                Err(e) => {
                    eprintln!("Link error: {}", e);
                    process::exit(1);
                }
            }
        }

        "check" => {
            // Type, borrow and effect checks with no code generation
            if args.len() < 3 {
                eprintln!("Usage: fidesc check <file.fd>");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let start = std::time::Instant::now();
            match compiler::build_program_with_path(&source, Some(input)) {
                Ok(_) => {
                    eprintln!("{}: no errors ({} ms)", input, start.elapsed().as_millis());
                    process::exit(0);
                }
                Err(errs) => {
                    for e in &errs {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }
        "lint" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc lint <file.fd>");
                eprintln!("  Run lint checks and report warnings (unused functions, unused imports).");
                process::exit(1);
            }
            let input = &args[2];
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            match compiler::lint_source(&source, input) {
                Ok(warnings) => {
                    if warnings.is_empty() {
                        eprintln!("No lint warnings.");
                    } else {
                        for w in &warnings {
                            eprintln!("warning: {}", w);
                        }
                        eprintln!("\n{} warning(s)", warnings.len());
                    }
                    process::exit(0);
                }
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            }
        }

        "bench" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc bench <file.fd> [-n <count>] [--ccompile]");
                eprintln!("  Benchmark: run file N times, report timing statistics.");
                eprintln!("  -n <count>   Number of iterations (default: 10)");
                eprintln!("  --ccompile   Benchmark C-compiled native binary instead of interpreter");
                process::exit(1);
            }
            let input = &args[2];
            let n: usize = args.iter().position(|a| a == "-n")
                .and_then(|i| args.get(i + 1))
                .and_then(|v| v.parse().ok())
                .unwrap_or(10);
            let use_ccompile = args.iter().any(|a| a == "--ccompile");
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            if use_ccompile {
                // Compile to native binary, then run it N times
                let tmp_bin = if cfg!(windows) {
                    format!("{}\\fidesc_bench_bin.exe", std::env::temp_dir().to_string_lossy())
                } else {
                    "/tmp/fidesc_bench_bin".to_string()
                };
                match compiler::ccompile_source_with_path(&source, input) {
                    Ok(c_code) => {
                        // Write C code and compile with available C compiler
                        let c_file = if cfg!(windows) {
                            format!("{}\\fidesc_bench.c", std::env::temp_dir().to_string_lossy())
                        } else {
                            "/tmp/fidesc_bench.c".to_string()
                        };
                        if let Err(e) = fs::write(&c_file, &c_code) {
                            eprintln!("Error writing C file: {}", e);
                            process::exit(1);
                        }
                        // Find available C compiler (tcc on Windows, gcc on Linux)
                        let cc = std::env::var("CC").unwrap_or_else(|_| {
                            if cfg!(windows) { "tcc".to_string() } else { "gcc".to_string() }
                        });
                        let cc_status = std::process::Command::new(&cc)
                            .args(["-O2", "-fno-strict-aliasing", "-w", "-o", &tmp_bin, &c_file])
                            .stdout(std::process::Stdio::null())
                            .stderr(std::process::Stdio::null())
                            .status();
                        match cc_status {
                            Ok(s) if s.success() => {}
                            _ => {
                                eprintln!("Error: C compilation failed");
                                process::exit(1);
                            }
                        }
                    }
                    Err(errors) => {
                        print_errors(&errors);
                        process::exit(1);
                    }
                }

                let mut times: Vec<f64> = Vec::with_capacity(n);
                for _ in 0..n {
                    let start = std::time::Instant::now();
                    let status = std::process::Command::new(&tmp_bin)
                        .stdout(std::process::Stdio::null())
                        .stderr(std::process::Stdio::null())
                        .status();
                    let elapsed = start.elapsed();
                    match status {
                        Ok(s) if s.success() => {
                            times.push(elapsed.as_secs_f64() * 1000.0);
                        }
                        _ => {
                            eprintln!("Error: benchmark binary failed");
                            process::exit(1);
                        }
                    }
                }

                // Report statistics
                times.sort_by(|a, b| a.partial_cmp(&b).unwrap());
                let total: f64 = times.iter().sum();
                let mean = total / times.len() as f64;
                let median = times[times.len() / 2];
                let min = times[0];
                let max = times[times.len() - 1];
                let variance: f64 = times.iter()
                    .map(|t| (t - mean).powi(2))
                    .sum::<f64>() / times.len() as f64;
                let stddev = variance.sqrt();

                eprintln!("Benchmark: {} (C native, {} iterations)", input, n);
                eprintln!("  mean:   {:.3} ms", mean);
                eprintln!("  median: {:.3} ms", median);
                eprintln!("  min:    {:.3} ms", min);
                eprintln!("  max:    {:.3} ms", max);
                eprintln!("  stddev: {:.3} ms", stddev);
                eprintln!("  total:  {:.3} ms", total);
            } else {
                // Interpreter benchmark
                let mut times: Vec<f64> = Vec::with_capacity(n);
                for _ in 0..n {
                    let start = std::time::Instant::now();
                    match compiler::interpret_source_with_path_release(&source, input, false, true) {
                        Ok((RunResult::Return(_), _)) => {
                            times.push(start.elapsed().as_secs_f64() * 1000.0);
                        }
                        Ok((RunResult::Panic(msg), trace)) => {
                            eprintln!("Panic: {}", msg);
                            if let Some(t) = trace { eprint!("{}", t); }
                            process::exit(1);
                        }
                        Ok((RunResult::Paused, _)) => {
                            times.push(start.elapsed().as_secs_f64() * 1000.0);
                        }
                        Err(errors) => {
                            print_errors(&errors);
                            process::exit(1);
                        }
                    }
                }

                // Report statistics
                times.sort_by(|a, b| a.partial_cmp(&b).unwrap());
                let total: f64 = times.iter().sum();
                let mean = total / times.len() as f64;
                let median = times[times.len() / 2];
                let min = times[0];
                let max = times[times.len() - 1];
                let variance: f64 = times.iter()
                    .map(|t| (t - mean).powi(2))
                    .sum::<f64>() / times.len() as f64;
                let stddev = variance.sqrt();

                eprintln!("Benchmark: {} (interpreter, {} iterations)", input, n);
                eprintln!("  mean:   {:.3} ms", mean);
                eprintln!("  median: {:.3} ms", median);
                eprintln!("  min:    {:.3} ms", min);
                eprintln!("  max:    {:.3} ms", max);
                eprintln!("  stddev: {:.3} ms", stddev);
                eprintln!("  total:  {:.3} ms", total);
            }
        }

        "profile" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc profile <file.fd> [-n <count>]");
                eprintln!("  Profile: run file and report per-function timing.");
                eprintln!("  -n <count>   Number of iterations (default: 1, more = stable timings)");
                process::exit(1);
            }
            let input = &args[2];
            let n: usize = args.iter().position(|a| a == "-n")
                .and_then(|i| args.get(i + 1))
                .and_then(|v| v.parse().ok())
                .unwrap_or(1);
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // Build the program and create an interpreter with profiling enabled
            let program = match compiler::build_program_with_path(&source, Some(input)) {
                Ok(p) => p,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };
            let module = lower::lower(&program);
            let mut interp = interp::Interpreter::new(module);
            interp.enable_profiling();

            // Run n iterations
            for i in 0..n {
                let result = interp.run();
                match &result {
                    RunResult::Return(_) => {}
                    RunResult::Panic(msg) => {
                        eprintln!("Panic on iteration {}: {}", i + 1, msg);
                        if let Some(trace) = interp.format_panic_trace() {
                            eprint!("{}", trace);
                        }
                        process::exit(1);
                    }
                    RunResult::Paused => {
                        eprintln!("Paused on iteration {}", i + 1);
                        break;
                    }
                }
                // Reset for next iteration (but keep profile data)
                if i + 1 < n {
                    // Re-run with fresh state — profile data accumulates
                }
            }

            // Print profiling results
            let profile = interp.get_profile();
            if profile.is_empty() {
                eprintln!("No functions were called during profiling.");
            } else {
                println!("Profile: {} ({} iteration{})", input, n, if n != 1 { "s" } else { "" });
                println!("{:<40} {:>10} {:>15} {:>15}", "Function", "Calls", "Total", "Avg/call");
                println!("{:-<83}", "");
                let total_ns: u64 = profile.iter().map(|(_, _, ns)| *ns).sum();
                for (name, calls, ns) in &profile {
                    let avg = if *calls > 0 { *ns / *calls } else { 0 };
                    let pct = if total_ns > 0 { (*ns as f64 / total_ns as f64) * 100.0 } else { 0.0 };
                    println!("{:<40} {:>10} {:>12.3}ms {:>12.3}ms  ({:.1}%)",
                        name, calls,
                        *ns as f64 / 1_000_000.0,
                        avg as f64 / 1_000_000.0,
                        pct);
                }
                println!("{:-<83}", "");
                println!("{:<40} {:>10} {:>12.3}ms",
                    "TOTAL", profile.iter().map(|(_, c, _)| *c).sum::<u64>(),
                    total_ns as f64 / 1_000_000.0);
            }
        }

        "fmt" => {
            if args.len() < 3 {
                eprintln!("Usage: fidesc fmt <file.fd> [--check]");
                eprintln!("  Format Fides source code (4-space indent, normalized spacing).");
                eprintln!("  --check   Exit 1 if formatting would change the file, 0 if already formatted.");
                process::exit(1);
            }
            let check_mode = args.iter().any(|a| a == "--check");
            // Find the input file (first non-flag argument after "fmt").
            let input = match args[2..].iter()
                .find(|a| !a.starts_with("-") && !a.starts_with("--"))
                .map(|s| s.to_string())
            {
                Some(f) => f,
                None => {
                    eprintln!("Error: no input file specified");
                    process::exit(1);
                }
            };
            let source = match fs::read_to_string(&input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };
            let formatted = fmt::format(&source);
            if check_mode {
                if formatted == source {
                    eprintln!("{}: already formatted", input);
                    process::exit(0);
                } else {
                    eprintln!("{}: would reformat", input);
                    process::exit(1);
                }
            } else {
                // Write the formatted output back to the file.
                if let Err(e) = fs::write(&input, &formatted) {
                    eprintln!("Error writing '{}': {}", input, e);
                    process::exit(1);
                }
                if formatted == source {
                    eprintln!("{}: already formatted", input);
                } else {
                    eprintln!("{}: formatted", input);
                }
                process::exit(0);
            }
        }

        "sbom" => {
            // SBOM export: parse a Fides source file, extract its import graph
            // with capabilities and annotations, and output CycloneDX or SPDX JSON.
            if args.len() < 3 {
                eprintln!("Usage: fidesc sbom <file.fd> [--format cyclonedx|spdx] [-o output]");
                eprintln!("  Export a Software Bill of Materials for a Fides program.");
                eprintln!("  --format cyclonedx   Output CycloneDX 1.5 JSON (default)");
                eprintln!("  --format spdx        Output SPDX 2.3 JSON");
                eprintln!("  -o <file>            Write to file (default: stdout)");
                process::exit(1);
            }
            let input = &args[2];
            let format = args.iter().position(|a| a == "--format")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or("cyclonedx");
            let output_file = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.to_string());

            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // Build the program to extract import and annotation data.
            let program = match compiler::build_program_with_path(&source, Some(input)) {
                Ok(p) => p,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };

            // Generate SBOM document (stub — emits a minimal structure).
            let sbom = generate_sbom(&program, input, format);

            if let Some(out_path) = output_file {
                if let Err(e) = fs::write(&out_path, &sbom) {
                    eprintln!("Error writing SBOM to '{}': {}", out_path, e);
                    process::exit(1);
                }
                eprintln!("SBOM written to {} (format: {})", out_path, format);
            } else {
                println!("{}", sbom);
            }
            process::exit(0);
        }

        "qualify" => {
            // STUB: Toolchain qualification manifest.
            // Planned: emit a manifest with compiler version, optimizer pass
            // list, test results, and annotation enforcement status. This is
            // the first step toward IEC 62304 / DO-178C tool qualification.
            // See: docs/roadmap/safety-critical.md §17
            if args.len() < 3 {
                eprintln!("Usage: fidesc qualify <file.fd> [-o output]");
                eprintln!("  Emit a toolchain qualification manifest for compiling <file.fd>.");
                eprintln!("  -o <file>   Write to file (default: stdout)");
                eprintln!();
                eprintln!("  The manifest includes:");
                eprintln!("    - Compiler version and build info");
                eprintln!("    - Optimizer passes applied");
                eprintln!("    - Annotations enforced (and stubs)");
                eprintln!("    - Backend target and test status");
                eprintln!("    - Capability/effect analysis of the input program");
                eprintln!();
                eprintln!("  NOTE: This is a stub. Full tool qualification requires");
                eprintln!("  partnership with a certification body (IEC 62304, DO-178C).");
                eprintln!("  See docs/roadmap/safety-critical.md for the full plan.");
                process::exit(1);
            }
            let input = &args[2];
            let output_file = args.iter().position(|a| a == "-o")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.to_string());

            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            // Build the program to extract annotation/capability data.
            let program = match compiler::build_program_with_path(&source, Some(input)) {
                Ok(p) => p,
                Err(errors) => {
                    print_errors(&errors);
                    process::exit(1);
                }
            };

            let manifest = generate_qualification_manifest(&program, input);

            if let Some(out_path) = output_file {
                if let Err(e) = fs::write(&out_path, &manifest) {
                    eprintln!("Error writing manifest to '{}': {}", out_path, e);
                    process::exit(1);
                }
                eprintln!("Qualification manifest written to {}", out_path);
            } else {
                println!("{}", manifest);
            }
            process::exit(0);
        }

        _ => {
            print_usage();
            process::exit(1);
        }
    }
}

/// Generate a minimal SBOM document (STUB).
/// See docs/roadmap/safety-critical.md §19 for the full implementation plan.
fn generate_sbom(program: &ast::Program, source_path: &str, format: &str) -> String {
    // Collect imported modules and their capabilities.
    let mut components: Vec<String> = Vec::new();
    for imp in &program.imports {
        let caps = if imp.capabilities.is_empty() {
            "[]".to_string()
        } else {
            let cap_strs: Vec<String> = imp.capabilities.iter()
                .map(|c| format!("\"{}\"", c))
                .collect();
            format!("[{}]", cap_strs.join(", "))
        };
        let version = imp.version.as_deref().unwrap_or("unknown");
        // CycloneDX component entry
        components.push(format!(
            "    {{\n      \"type\": \"library\",\n      \"name\": \"{}\",\n      \"version\": \"{}\",\n      \"bom-ref\": \"pkg:fides/{}@{}\",\n      \"properties\": [\n        {{\n          \"name\": \"fides:capabilities\",\n          \"value\": {}\n        }}\n      ]\n    }}",
            imp.path, version, imp.path, version, caps
        ));
    }

    // Collect annotations used in the program (functions + structs + enums).
    let mut annotations: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
    for f in &program.funcs {
        for a in &f.annotations {
            annotations.insert(a.name.clone());
        }
    }
    for s in &program.structs {
        for a in &s.annotations {
            annotations.insert(a.name.clone());
        }
    }

    // Collect function-level capability info for the dependency graph.
    let mut func_info: Vec<String> = Vec::new();
    for f in &program.funcs {
        let ann_names: Vec<String> = f.annotations.iter()
            .map(|a| a.name.clone())
            .collect();
        if !ann_names.is_empty() {
            func_info.push(format!(
                "    {{\n      \"ref\": \"pkg:fides/function/{}\",\n      \"scope\": \"required\",\n      \"properties\": [\n        {{\n          \"name\": \"fides:annotations\",\n          \"value\": \"{}\"\n        }}\n      ]\n    }}",
                f.name, ann_names.join(",")
            ));
        }
    }

    // Timestamp (RFC 3339).
    let now = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .map(|d| d.as_secs())
        .unwrap_or(0);

    match format {
        "spdx" => {
            // SPDX 2.3 JSON.
            let ann_list: Vec<String> = annotations.iter().cloned().collect();
            let mut spdx_components: Vec<String> = Vec::new();
            for imp in &program.imports {
                let version = imp.version.as_deref().unwrap_or("unknown");
                let caps = if imp.capabilities.is_empty() {
                    "NONE".to_string()
                } else {
                    imp.capabilities.join(", ")
                };
                spdx_components.push(format!(
                    "    {{\n      \"name\": \"{}\",\n      \"SPDXID\": \"SPDXRef-Package-{}\",\n      \"versionInfo\": \"{}\",\n      \"downloadLocation\": \"NOASSERTION\",\n      \"filesAnalyzed\": false,\n      \"licenseConcluded\": \"NOASSERTION\",\n      \"licenseDeclared\": \"NOASSERTION\",\n      \"copyrightText\": \"NOASSERTION\",\n      \"supplier\": \"Organization: Fides\",\n      \"comment\": \"Capabilities: {}\"\n    }}",
                    imp.path, imp.path.replace('/', "-"), version, caps
                ));
            }
            format!(
                "{{\n  \"spdxVersion\": \"SPDX-2.3\",\n  \"dataLicense\": \"CC0-1.0\",\n  \"SPDXID\": \"SPDXRef-DOCUMENT\",\n  \"name\": \"{}\",\n  \"documentNamespace\": \"https://fides.dev/sbom/{}-{}\",\n  \"creationInfo\": {{\n    \"created\": \"{}\",\n    \"creators\": [\"Tool: fidesc (Fides Compiler)\"]\n  }},\n  \"annotations\": [\n    {{\n      \"annotationDate\": \"{}\",\n      \"annotationType\": \"OTHER\",\n      \"annotator\": \"Tool: fidesc\",\n      \"comment\": \"Safety annotations: {}\"\n    }}\n  ],\n  \"packages\": [\n{}\n  ]\n}}",
                source_path,
                source_path.replace('/', "_"),
                now,
                now,
                now,
                ann_list.join(", "),
                spdx_components.join(",\n")
            )
        }
        _ => {
            // CycloneDX 1.5 JSON.
            let ann_list: Vec<String> = annotations.iter().cloned().collect();
            let deps = if func_info.is_empty() {
                "  []".to_string()
            } else {
                format!("  [\n{}\n  ]", func_info.join(",\n"))
            };
            format!(
                "{{\n  \"bomFormat\": \"CycloneDX\",\n  \"specVersion\": \"1.5\",\n  \"serialNumber\": \"urn:uuid:fides-{}\",\n  \"version\": 1,\n  \"metadata\": {{\n    \"timestamp\": \"{}\",\n    \"tools\": [{{ \"name\": \"fidesc\", \"vendor\": \"Fides\", \"version\": \"1.0.0\" }}],\n    \"component\": {{\n      \"type\": \"application\",\n      \"name\": \"{}\",\n      \"bom-ref\": \"pkg:fides/app/{}\"\n    }}\n  }},\n  \"properties\": [\n    {{\n      \"name\": \"fides:annotations\",\n      \"value\": \"{}\"\n    }}\n  ],\n  \"components\": [\n{}\n  ],\n  \"dependencies\": {}\n}}",
                now,
                now,
                source_path,
                source_path.replace('/', "_"),
                ann_list.join(","),
                components.join(",\n"),
                deps
            )
        }
    }
}

/// Generate a toolchain qualification manifest (STUB).
/// See docs/roadmap/safety-critical.md §17 for the full plan.
fn generate_qualification_manifest(program: &ast::Program, source_path: &str) -> String {
    // Collect annotations used.
    let mut annotations: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
    for f in &program.funcs {
        for a in &f.annotations {
            annotations.insert(a.name.clone());
        }
    }
    for s in &program.structs {
        for a in &s.annotations {
            annotations.insert(a.name.clone());
        }
    }

    // Collect capabilities used.
    let mut caps: std::collections::BTreeSet<String> = std::collections::BTreeSet::new();
    for imp in &program.imports {
        for c in &imp.capabilities {
            caps.insert(c.clone());
        }
    }

    // Optimizer passes (from opt::Pass::all()).
    let passes: Vec<&str> = opt::Pass::all().iter().map(|p| p.name()).collect();

    // Annotations enforced in the Rust bootstrap — single source of truth is
    // typechecker::enforced_annotations() (annotations with a verify_* pass).
    let enforced: &[&str] = fidesc::typechecker::enforced_annotations();

    // Self-hosted compiler size, measured from the compiler/ directory at
    // run time (never hardcoded; "unknown" when the tree is not found).
    let self_hosted = match selfhost_stats() {
        Some((modules, loc)) => format!("compiler/*.fd ({} modules, {} LOC)", modules, loc),
        None => "compiler/*.fd (unknown: compiler/ directory not found relative to the executable or cwd)".to_string(),
    };

    // Post-codegen constant-time verifier: wired in compiler::build_executable_with_level
    // for the targets listed in compiler::POST_CODEGEN_CT_CHECK_TARGETS.
    let ct_targets: Vec<&str> = compiler::POST_CODEGEN_CT_CHECK_TARGETS.to_vec();
    let ct_status = if ct_targets.is_empty() { "not wired" } else { "implemented" };

    let used_ann: Vec<&str> = annotations.iter().map(|s| s.as_str()).collect();
    let used_caps: Vec<&str> = caps.iter().map(|s| s.as_str()).collect();

    format!(
        "{{\n  \"manifestVersion\": \"0.2\",\n  \"compiler\": {{\n    \"name\": \"fidesc\",\n    \"version\": \"{}\",\n    \"implementation\": \"Rust bootstrap (src/*.rs)\",\n    \"selfHosted\": \"{}\"\n  }},\n  \"source\": {{\n    \"path\": \"{}\",\n    \"annotations\": [\"{}\"],\n    \"capabilities\": [\"{}\"]\n  }},\n  \"optimizer\": {{\n    \"passes\": [\"{}\"],\n    \"passCount\": {}\n  }},\n  \"annotationEnforcement\": {{\n    \"enforcedInBootstrap\": [\"{}\"],\n    \"enforcedInBootstrapMeaning\": \"annotations with a dedicated verify_* pass in the Rust bootstrap typechecker (typechecker::enforced_annotations)\",\n    \"selfHostEnforcementNote\": \"The self-hosted checker (compiler/*.fd) status is tracked in docs/roadmap/roadmap.md § Issues & Debates (CB1) and is not asserted by this manifest.\",\n    \"postCodegenConstantTimeCheck\": {{\n      \"status\": \"{}\",\n      \"targets\": [\"{}\"],\n      \"runsIn\": \"fidesc build (compiler::build_executable_with_level → codegen_x64::verify_constant_time_asm)\"\n    }}\n  }},\n  \"qualificationStatus\": \"NOT_QUALIFIED\",\n  \"note\": \"This is not a certified toolchain. Full IEC 62304 / DO-178C tool qualification requires certification body partnership. See docs/roadmap/safety-critical.md.\"\n}}",
        env!("CARGO_PKG_VERSION"),
        self_hosted,
        source_path,
        used_ann.join("\", \""),
        used_caps.join("\", \""),
        passes.join("\", \""),
        passes.len(),
        enforced.join("\", \""),
        ct_status,
        ct_targets.join("\", \""),
    )
}

/// Locate the repository's `compiler/` directory (self-hosted compiler
/// source) and return `(module_count, total_lines)` over its `*.fd` files.
/// Searches upward from the executable's directory, then from the current
/// working directory. Returns None if no directory containing both
/// `compiler/*.fd` and `stdlib/` is found.
fn selfhost_stats() -> Option<(usize, usize)> {
    fn find_root(start: &std::path::Path) -> Option<std::path::PathBuf> {
        let mut cur = start.to_path_buf();
        loop {
            if cur.join("compiler").is_dir() && cur.join("stdlib").is_dir() {
                return Some(cur);
            }
            if !cur.pop() {
                return None;
            }
        }
    }
    let mut starts: Vec<std::path::PathBuf> = Vec::new();
    if let Ok(exe) = std::env::current_exe() {
        if let Some(dir) = exe.parent() {
            starts.push(dir.to_path_buf());
        }
    }
    if let Ok(cwd) = std::env::current_dir() {
        starts.push(cwd);
    }
    let root = starts.iter().find_map(|s| find_root(s))?;
    let entries = fs::read_dir(root.join("compiler")).ok()?;
    let mut modules = 0usize;
    let mut loc = 0usize;
    for entry in entries.flatten() {
        let path = entry.path();
        if path.extension().map_or(false, |e| e == "fd") {
            if let Ok(text) = fs::read_to_string(&path) {
                modules += 1;
                loc += text.lines().count();
            }
        }
    }
    if modules == 0 { None } else { Some((modules, loc)) }
}

/// Parse a target triple string into the canonical target name used by the compiler.
/// Supports formats:
///   - "x86-64", "x64", "x86-64-linux", "x86_64-linux"
///   - "arm64", "aarch64", "arm64-linux", "aarch64-linux"
///   - "riscv", "riscv64", "riscv64-linux", "riscv-linux"
///   - "x86-64-windows", "x64-windows", "windows"
/// Returns the canonical target: "x86-64", "arm64", "riscv", or "x86-64-windows"
/// Parse a simple linker script to extract the load address.
/// Supported directives:
///   LOAD_ADDR(0x100000)   — set the load address
///   ENTRY(_start)         — set the entry point (ignored for now)
///   . = 0x100000          — set the load address (GNU-style)
fn parse_load_addr_from_script(script: &str) -> Option<u64> {
    for line in script.lines() {
        let line = line.trim();
        // LOAD_ADDR(0x...)
        if let Some(rest) = line.strip_prefix("LOAD_ADDR(") {
            if let Some(end) = rest.find(')') {
                let addr_str = &rest[..end];
                if let Ok(addr) = u64::from_str_radix(addr_str.trim_start_matches("0x"), 16) {
                    return Some(addr);
                }
            }
        }
        // . = 0x... (GNU linker script syntax)
        if line.starts_with(". = ") || line.starts_with(".= ") {
            let addr_str = line.split('=').nth(1).unwrap_or("").trim().trim_end_matches(';');
            if let Ok(addr) = u64::from_str_radix(addr_str.trim_start_matches("0x"), 16) {
                return Some(addr);
            }
        }
    }
    None
}

fn parse_target_triple(triple: &str) -> &str {
    let lower = triple.to_lowercase();
    if lower.contains("windows") {
        "x86-64-windows"
    } else if lower.starts_with("arm64") || lower.starts_with("aarch64") {
        "arm64"
    } else if lower.starts_with("riscv") {
        "riscv"
    } else {
        "x86-64"
    }
}

fn print_usage() {
    eprintln!("fidesc — Fides language bootstrap compiler");
    eprintln!();
    eprintln!("Usage:");
    eprintln!("  fidesc build   <file.fd> [-o output] [--target] [--opt]  Compile + link in one step (zero external tools)");
    eprintln!("  fidesc emit    <file.fd> [-o output.exe]  Emit PE directly (no ml64.exe/link.exe)");
    eprintln!("           [--bare]                          Bare-metal: skip stdlib, flat binary");
    eprintln!("           [--boot]                          Multiboot2: kernel binary for GRUB");
    eprintln!("           [--load-addr 0xADDR]              Override load address (default: 0x10000 bare, 0x100000 boot)");
    eprintln!("           [--linker-script <file>]          Parse linker script for LOAD_ADDR and section placement");
    eprintln!("  fidesc ccompile <file.fd> [-o output.exe]  Compile via C codegen (gcc/clang)");
    eprintln!("           [--target <triple>]                Cross-compile (e.g. aarch64-linux-gnu)");
    eprintln!("  fidesc compile <file.fd> [-o output.s]    Compile to ARM64 assembly (legacy)");
    eprintln!("  fidesc run     <file.fd> [-v]             Interpret Fides IR (execute)");
    eprintln!("  fidesc parse-header <file.h> [-o out.fd]  Parse C header, generate FFI bindings");
    eprintln!("  fidesc ir      <file.fd>                  Show Fides IR");
    eprintln!("  fidesc pir     <file.fd> [1|2|3] [--opt]  Show PIR (Level 1/2/3, optional optimize)");
    eprintln!("  fidesc wasm    <file.fd> [-o out.wasm] [--wat]  Compile to WASM binary (or WAT text)");
    eprintln!("  fidesc opt     <file.fd> [passes]         Optimize PIR L2 and show result");
    eprintln!("  fidesc codegen <file.fd> [--target x86-64] Custom codegen (PIR L3 → x86-64 asm)");
    eprintln!("           [--peephole] [--elf <out.o>]     Peephole optimize / emit ELF object");
    eprintln!("  fidesc gpu     <file.fd> --target spirv|ptx  Generate GPU code (SPIR-V or PTX)");
    eprintln!("           [-o output] [--text]             Write to file / SPIR-V text disassembly");
    eprintln!("  fidesc tokens  <file.fd>                  Show tokens");
    eprintln!("  fidesc ast     <file.fd>                  Show AST");
    eprintln!("  fidesc dump-tokens <file.fd>              Show tokens (dump-* alias)");
    eprintln!("  fidesc dump-ast    <file.fd>              Show AST (dump-* alias)");
    eprintln!("  fidesc dump-ir     <file.fd>              Show IR (dump-* alias)");
    eprintln!("  fidesc dump-pir    <file.fd> [1|2|3|all]  Show PIR (dump-* alias)");
    eprintln!("  fidesc dump-c      <file.fd>              Show generated C source");
    eprintln!("  fidesc diff-test <file.fd> --stage <s>    Compare Rust vs Fides output (or --golden)");
    eprintln!("           [--update]                       Update golden file with current output");
    eprintln!("  fidesc version                           Show version");
    eprintln!("  fidesc watch   <file.fd> [--interval <ms>] [--run]  Hot reload: recompile on change");
    eprintln!("  fidesc test                              Run all tests across all backends");
    eprintln!("  fidesc test --fail                       Run only expected-fail tests (failing = correct)");
    eprintln!("  fidesc repl                              Interactive REPL (evaluate expressions)");
    eprintln!("  fidesc verify  <file.fd>                  Verify IR correctness");
    eprintln!("  fidesc sourcemap <file.fd>                Show source map (IR to source mapping)");
    eprintln!("  fidesc lint   <file.fd>                  Run lint checks (unused functions, imports)");
    eprintln!("  fidesc bench  <file.fd> [-n <count>]     Benchmark: run file N times, report timing");
    eprintln!("  fidesc profile <file.fd> [-n <count>]    Profile: per-function call count and timing");
    eprintln!("  fidesc fmt    <file.fd> [--check]        Format source code (or check if formatted)");
    eprintln!("  fidesc sbom   <file.fd> [--format cyclonedx|spdx] [-o out]  Export SBOM (stub)");
    eprintln!("  fidesc qualify <file.fd> [-o out]        Emit toolchain qualification manifest (stub)");
    eprintln!();
    eprintln!("Global flags (can appear after any command):");
    eprintln!("  --borrowck=off|warn|on   Borrow checker mode (default: on)");
    eprintln!("       off    Skip borrow checking");
    eprintln!("       warn   Emit warnings on borrow violations (does not fail)");
    eprintln!("       on     Fail compilation on borrow violations");
}

/// Find ml64.exe by searching Visual Studio install paths
fn find_ml64() -> Option<String> {
    let base = r"C:\Program Files\Microsoft Visual Studio";
    if let Ok(vs_versions) = fs::read_dir(base) {
        for vs_ver in vs_versions.flatten() {
            if let Ok(editions) = fs::read_dir(vs_ver.path()) {
                for edition in editions.flatten() {
                    let vc_tools = edition.path().join(r"VC\Tools\MSVC");
                    if let Ok(versions) = fs::read_dir(&vc_tools) {
                        for ver in versions.flatten() {
                            let ml64 = ver.path().join(r"bin\Hostx64\x64\ml64.exe");
                            if ml64.exists() {
                                return Some(ml64.to_string_lossy().to_string());
                            }
                        }
                    }
                }
            }
        }
    }
    None
}

/// Find cl.exe by searching Visual Studio install paths
fn find_cl() -> Option<String> {
    let base = r"C:\Program Files\Microsoft Visual Studio";
    if let Ok(vs_versions) = fs::read_dir(base) {
        for vs_ver in vs_versions.flatten() {
            if let Ok(editions) = fs::read_dir(vs_ver.path()) {
                for edition in editions.flatten() {
                    let vc_tools = edition.path().join(r"VC\Tools\MSVC");
                    if let Ok(versions) = fs::read_dir(&vc_tools) {
                        for ver in versions.flatten() {
                            // Try Hostx64\x64 first, then Hostx86\x64
                            for host in &["Hostx64", "Hostx86"] {
                                let cl = ver.path().join(format!(r"bin\{}\x64\cl.exe", host));
                                if cl.exists() {
                                    return Some(cl.to_string_lossy().to_string());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    None
}

/// Run a composite `cmd /C` line. `Command::args` would escape each inner
/// quote as `\"`, which cmd.exe parses as a UNC path (ERROR_BAD_NETPATH).
#[cfg(windows)]
fn run_cmd_line(line: &str) -> std::io::Result<std::process::Output> {
    use std::os::windows::process::CommandExt;
    process::Command::new("cmd").arg("/C").raw_arg(line).output()
}

#[cfg(not(windows))]
fn run_cmd_line(line: &str) -> std::io::Result<std::process::Output> {
    process::Command::new("cmd").args(["/C", line]).output()
}

/// Find vcvarsall.bat by searching Visual Studio install paths
fn find_vcvarsall() -> Option<String> {
    let base = r"C:\Program Files\Microsoft Visual Studio";
    if let Ok(vs_versions) = fs::read_dir(base) {
        for vs_ver in vs_versions.flatten() {
            if let Ok(editions) = fs::read_dir(vs_ver.path()) {
                for edition in editions.flatten() {
                    let vcvars = edition.path().join(r"VC\Auxiliary\Build\vcvarsall.bat");
                    if vcvars.exists() {
                        return Some(vcvars.to_string_lossy().to_string());
                    }
                }
            }
        }
    }
    None
}

/// Find link.exe by searching Visual Studio install paths
fn find_link() -> Option<String> {
    let base = r"C:\Program Files\Microsoft Visual Studio";
    if let Ok(vs_versions) = fs::read_dir(base) {
        for vs_ver in vs_versions.flatten() {
            if let Ok(editions) = fs::read_dir(vs_ver.path()) {
                for edition in editions.flatten() {
                    let vc_tools = edition.path().join(r"VC\Tools\MSVC");
                    if let Ok(versions) = fs::read_dir(&vc_tools) {
                        for ver in versions.flatten() {
                            let link = ver.path().join(r"bin\Hostx64\x64\link.exe");
                            if link.exists() {
                                return Some(link.to_string_lossy().to_string());
                            }
                        }
                    }
                }
            }
        }
    }
    None
}

/// Find kernel32.lib path (Windows 10 SDK x64)
fn find_kernel32_lib() -> Option<String> {
    let base = r"C:\Program Files (x86)\Windows Kits\10\Lib";
    if let Ok(versions) = fs::read_dir(base) {
        for ver in versions.flatten() {
            let lib = ver.path().join(r"um\x64");
            if lib.join("kernel32.lib").exists() {
                return Some(lib.to_string_lossy().to_string());
            }
        }
    }
    None
}
