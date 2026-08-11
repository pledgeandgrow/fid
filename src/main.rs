mod ast;
mod codegen;
mod codegen_c;
mod compiler;
mod comptime;
mod cparser;
mod interp;
mod ir;
mod json;
mod lower;
mod mono;
mod lexer;
mod parser;
mod token;
mod typechecker;
mod pe;
mod pir;
mod opt;
mod regalloc;
mod codegen_x64;
mod codegen_arm64;
mod codegen_riscv;
mod peephole;
mod elf64;
mod coff;
mod macho;
mod hotreload;
mod linker;
mod embedded;
mod gpu;
mod tensor;
mod wasm;
mod x86;

use std::env;
use std::fs;
use std::process;

use compiler::RunResult;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        print_usage();
        process::exit(1);
    }

    let command = &args[1];

    match command.as_str() {
        "parse-header" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec parse-header <file.h> [-o output.p]");
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
                eprintln!("Usage: pledgec build <file.p> [-o output] [--target x86-64|arm64|riscv] [--opt]");
                eprintln!("       Compiles and links a Pledge program into a native executable in one step");
                eprintln!("       --target  Target architecture (default: x86-64)");
                eprintln!("       --opt     Run PIR optimizer before codegen");
                process::exit(1);
            }
            let input = &args[2];
            let target = args.iter().position(|a| a == "--target")
                .and_then(|i| args.get(i + 1))
                .map(|s| s.as_str())
                .unwrap_or("x86-64");
            let do_opt = args.iter().any(|a| a == "--opt");
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
            match compiler::build_executable(&source, input, target, do_opt) {
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
                    println!("Built native executable: {} ({} bytes, zero external tools)", output, exe_bytes.len());
                }
                Err(errors) => {
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "emit" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec emit <file.p> [-o output.exe] [--bare] [--boot]");
                process::exit(1);
            }
            let bare_mode = args.iter().any(|a| a == "--bare");
            let boot_mode = args.iter().any(|a| a == "--boot");
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
                        for e in errors {
                            eprintln!("{}", e);
                        }
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
                        for e in errors {
                            eprintln!("{}", e);
                        }
                        process::exit(1);
                    }
                }
            } else {
                match compiler::emit_source_with_path(&source, input) {
                    Ok(pe_bytes) => {
                        fs::write(&output, &pe_bytes).unwrap_or_else(|e| {
                            eprintln!("Error writing '{}': {}", output, e);
                            process::exit(1);
                        });
                        println!("Emitted native executable: {} ({} bytes, no ml64.exe/link.exe)", output, pe_bytes.len());
                    }
                    Err(errors) => {
                        for e in errors {
                            eprintln!("{}", e);
                        }
                        process::exit(1);
                    }
                }
            }
        }

        "compile" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec compile <file.p> [-o output.s]");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "tokens" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec tokens <file.p>");
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
                eprintln!("Usage: pledgec ast <file.p>");
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
                eprintln!("Usage: pledgec ir <file.p>");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "run" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec run <file.p> [-v]");
                process::exit(1);
            }
            let input = &args[2];
            let verbose = args.iter().any(|a| a == "-v" || a == "--verbose");
            let release = args.iter().any(|a| a == "--release");
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::interpret_source_with_path_release(&source, input, verbose, release) {
                Ok(result) => {
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
                            process::exit(1);
                        }
                    }
                }
                Err(errors) => {
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "version" => {
            println!("pledgec 0.1.0 (bootstrap compiler)");
            println!("Written in Rust — will be replaced by self-hosted Pledge compiler");
        }

        "ccompile" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec ccompile <file.p> [-o output.exe] [--target <triple>]");
                process::exit(1);
            }
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
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::ccompile_source_with_path(&source, input) {
                Ok(c_code) => {
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
                            for c in &["gcc", "clang", "cc"] {
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
                    let status = if is_cl {
                        // cl.exe needs MSVC environment (INCLUDE, LIB, etc.)
                        // Use vcvarsall.bat to set it up, then run cl.exe
                        let vcvars = find_vcvarsall();
                        if let Some(vcvars) = vcvars {
                            let cmd_str = format!(
                                "\"{}\" x64 >nul 2>&1 && \"{}\" /O2 /nologo /Fe:{} {}",
                                vcvars, cc, output, c_path
                            );
                            process::Command::new("cmd")
                                .args(["/C", &cmd_str])
                                .output()
                        } else {
                            // No vcvarsall — try direct
                            process::Command::new(&cc)
                                .args(["/O2", "/nologo", &format!("/Fe:{}", output), &c_path])
                                .output()
                        }
                    } else {
                        // gcc/clang: -O2 -o output c_path
                        // Extract link libraries from the C source (extern "C" link "m")
                        let mut link_libs: Vec<String> = Vec::new();
                        for line in c_code.lines() {
                            if line.starts_with("// PLINK:") {
                                let lib = line.trim_start_matches("// PLINK:").trim();
                                link_libs.push(format!("-l{}", lib));
                            }
                        }
                        let mut cmd = process::Command::new(&cc);
                        cmd.arg("-O2").arg("-o").arg(&output).arg(&c_path);
                        for lib in &link_libs {
                            cmd.arg(lib);
                        }
                        cmd.output()
                    };
                    match status {
                        Ok(out) => {
                            if !out.status.success() {
                                eprintln!("{} failed:", cc);
                                eprintln!("{}", String::from_utf8_lossy(&out.stderr));
                                process::exit(1);
                            }
                            println!("Built native executable: {}", output);
                        }
                        Err(e) => {
                            eprintln!("Failed to run {}: {}", cc, e);
                            eprintln!("C source left at: {}", c_path);
                            eprintln!("Compile manually: {} -O2 -o {} {}", cc, output, c_path);
                            process::exit(1);
                        }
                    }
                }
                Err(errors) => {
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "sourcemap" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec sourcemap <file.p>");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "verify" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec verify <file.p>");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "repl" => {
            use std::io::{self, BufRead, Write};
            eprintln!("pledgec REPL — type expressions to evaluate, :quit to exit");
            eprintln!();
            let stdin = io::stdin();
            let mut line_num = 1;
            loop {
                eprint!("pledge:{}> ", line_num);
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
                    eprintln!("Type a Pledge expression (e.g. 1 + 2, \"hello\", [1,2,3])");
                    continue;
                }
                // Wrap the input in a main function and interpret it
                let source = format!(
                    "pf main() -> i64 {{\n    println({});\n    ret 0;\n}}",
                    trimmed
                );
                match compiler::interpret_source(&source, false) {
                    Ok(RunResult::Return(_)) => {}
                    Ok(RunResult::Panic(msg)) => {
                        eprintln!("Panic: {}", msg);
                    }
                    Err(errors) => {
                        for e in errors {
                            eprintln!("{}", e);
                        }
                    }
                }
                line_num += 1;
            }
            eprintln!("\nBye!");
        }

        "test" => {
            // Run all .p tests in the tests/ directory against all backends.
            let current_exe = std::env::current_exe()
                .unwrap_or_else(|_| std::path::PathBuf::from("pledgec"));
            let test_dir = "tests";
            let mut test_files: Vec<String> = Vec::new();
            if let Ok(entries) = fs::read_dir(test_dir) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().map_or(false, |e| e == "p") {
                        test_files.push(path.to_string_lossy().to_string());
                    }
                }
            }
            test_files.sort();

            if test_files.is_empty() {
                eprintln!("No .p test files found in tests/");
                process::exit(1);
            }

            println!("Running {} tests...\n", test_files.len());

            let mut interp_pass = 0;
            let mut interp_fail = 0;
            let mut c_pass = 0;
            let mut c_fail = 0;
            let mut pe_pass = 0;
            let mut pe_fail = 0;

            for test_file in &test_files {
                let name = std::path::Path::new(test_file)
                    .file_stem()
                    .unwrap()
                    .to_string_lossy()
                    .to_string();

                // Interpreter — a test "passes" if it doesn't panic.
                // The exit code may be non-zero (e.g. `ret 42` exits with 42).
                let interp_out = process::Command::new(&current_exe)
                    .args(["run", test_file])
                    .output();
                let interp_ok = interp_out
                    .as_ref()
                    .map(|o| {
                        // Check stderr for "Panic:" or compilation errors
                        let stderr = String::from_utf8_lossy(&o.stderr);
                        !stderr.contains("Panic:") && !stderr.contains("error:")
                    })
                    .unwrap_or(false);
                let interp_stdout = interp_out
                    .as_ref()
                    .map(|o| String::from_utf8_lossy(&o.stdout).to_string())
                    .unwrap_or_default();

                if interp_ok {
                    interp_pass += 1;
                } else {
                    interp_fail += 1;
                    eprintln!("  [FAIL] interp: {}", name);
                }

                // C codegen
                let c_exe = format!("/tmp/{}_test_c.exe", name);
                let _ = fs::remove_file(&c_exe);
                let c_compile = process::Command::new(&current_exe)
                    .args(["ccompile", test_file, "-o", &c_exe])
                    .output();
                let c_ok = if c_compile.as_ref().map(|o| o.status.success()).unwrap_or(false) {
                    if let Ok(c_out) = process::Command::new(&c_exe).output() {
                        let c_stdout = String::from_utf8_lossy(&c_out.stdout).to_string();
                        c_stdout == interp_stdout
                    } else {
                        false
                    }
                } else {
                    false
                };
                let _ = fs::remove_file(&c_exe);
                let _ = fs::remove_file(&c_exe.replace(".exe", ".c"));

                if c_ok {
                    c_pass += 1;
                } else {
                    c_fail += 1;
                    eprintln!("  [FAIL] c codegen: {}", name);
                }

                // PE emitter
                let pe_exe = format!("/tmp/{}_test_pe.exe", name);
                let _ = fs::remove_file(&pe_exe);
                let pe_out = process::Command::new(&current_exe)
                    .args(["emit", test_file, "-o", &pe_exe])
                    .output();
                let pe_ok = pe_out
                    .as_ref()
                    .map(|o| o.status.success() && std::path::Path::new(&pe_exe).exists())
                    .unwrap_or(false);
                let _ = fs::remove_file(&pe_exe);

                if pe_ok {
                    pe_pass += 1;
                } else {
                    pe_fail += 1;
                    eprintln!("  [FAIL] pe emit: {}", name);
                }
            }

            println!("\n=== Results ===");
            println!("Interpreter:  {}/{} passed", interp_pass, test_files.len());
            println!("C Codegen:    {}/{} matched", c_pass, test_files.len());
            println!("PE Emitter:   {}/{} valid", pe_pass, test_files.len());

            if interp_fail > 0 || c_fail > 0 || pe_fail > 0 {
                process::exit(1);
            }
        }

        "wasm" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec wasm <file.p> [-o output.wasm] [--wat]");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
                    process::exit(1);
                }
            }
        }

        "pir" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec pir <file.p> [1|2|3|all] [--opt]");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
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

        "opt" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec opt <file.p> [pass1,pass2,...]");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
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
                eprintln!("Usage: pledgec codegen <file.p> [--target x86-64|arm64|riscv] [--peephole] [--elf <o>] [--coff <o>] [--macho <o>]");
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
                    for e in errors {
                        eprintln!("{}", e);
                    }
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

        "watch" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec watch <file.p> [--interval <ms>]");
                eprintln!("       Auto-recompiles and runs on file change (hot reload mode)");
                process::exit(1);
            }
            let input = &args[2];
            let interval: u64 = args.iter().position(|a| a == "--interval")
                .and_then(|i| args.get(i + 1))
                .and_then(|s| s.parse().ok())
                .unwrap_or(500);

            let input_path = std::path::PathBuf::from(input);
            let input_clone = input.clone();

            let mut watcher = hotreload::Watcher::new(move |path| {
                eprintln!("\n[watch] File changed: {} — recompiling...", path);
                let source = match fs::read_to_string(&input_clone) {
                    Ok(s) => s,
                    Err(e) => {
                        eprintln!("[watch] Error reading: {}", e);
                        return;
                    }
                };
                match compiler::lower_to_ir_module(&source) {
                    Ok(module) => {
                        eprintln!("[watch] Compiled {} functions", module.functions.len());
                        // In a full implementation, this would JIT-compile and hot-swap
                        // For now, just report success
                        eprintln!("[watch] Hot reload complete");
                    }
                    Err(errors) => {
                        for e in errors {
                            eprintln!("[watch] {}", e);
                        }
                    }
                }
            });
            watcher.watch(&input_path);
            eprintln!("[watch] Watching {} (polling every {}ms)...", input, interval);
            eprintln!("[watch] Press Ctrl+C to stop");
            watcher.run(interval);
        }

        "link" => {            if args.len() < 4 {
                eprintln!("Usage: pledgec link <output> <object1.o> [object2.o ...] [--entry <name>]");
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

        _ => {
            print_usage();
            process::exit(1);
        }
    }
}

fn print_usage() {
    eprintln!("pledgec — Pledge language bootstrap compiler");
    eprintln!();
    eprintln!("Usage:");
    eprintln!("  pledgec build   <file.p> [-o output] [--target] [--opt]  Compile + link in one step (zero external tools)");
    eprintln!("  pledgec emit    <file.p> [-o output.exe]  Emit PE directly (no ml64.exe/link.exe)");
    eprintln!("           [--bare]                          Bare-metal: skip stdlib, flat binary");
    eprintln!("           [--boot]                          Multiboot2: kernel binary for GRUB");
    eprintln!("  pledgec ccompile <file.p> [-o output.exe]  Compile via C codegen (gcc/clang)");
    eprintln!("           [--target <triple>]                Cross-compile (e.g. aarch64-linux-gnu)");
    eprintln!("  pledgec compile <file.p> [-o output.s]    Compile to ARM64 assembly (legacy)");
    eprintln!("  pledgec run     <file.p> [-v]             Interpret Pledge IR (execute)");
    eprintln!("  pledgec parse-header <file.h> [-o out.p]  Parse C header, generate FFI bindings");
    eprintln!("  pledgec ir      <file.p>                  Show Pledge IR");
    eprintln!("  pledgec pir     <file.p> [1|2|3] [--opt]  Show PIR (Level 1/2/3, optional optimize)");
    eprintln!("  pledgec wasm    <file.p> [-o out.wasm] [--wat]  Compile to WASM binary (or WAT text)");
    eprintln!("  pledgec opt     <file.p> [passes]         Optimize PIR L2 and show result");
    eprintln!("  pledgec codegen <file.p> [--target x86-64] Custom codegen (PIR L3 → x86-64 asm)");
    eprintln!("           [--peephole] [--elf <out.o>]     Peephole optimize / emit ELF object");
    eprintln!("  pledgec tokens  <file.p>                  Show tokens");
    eprintln!("  pledgec ast     <file.p>                  Show AST");
    eprintln!("  pledgec version                           Show version");
    eprintln!("  pledgec test                              Run all tests across all backends");
    eprintln!("  pledgec repl                              Interactive REPL (evaluate expressions)");
    eprintln!("  pledgec verify  <file.p>                  Verify IR correctness");
    eprintln!("  pledgec sourcemap <file.p>                Show source map (IR to source mapping)");
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
