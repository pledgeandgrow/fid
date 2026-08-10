mod ast;
mod codegen;
mod codegen_c;
mod compiler;
mod interp;
mod ir;
mod lower;
mod mono;
mod lexer;
mod parser;
mod token;
mod typechecker;
mod pe;
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
        "build" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec build <file.p> [-o output.exe]");
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
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::build_source_with_path(&source, input) {
                Ok(asm) => {
                    // Write assembly
                    let asm_path = output.replace(".exe", ".asm");
                    fs::write(&asm_path, &asm).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", asm_path, e);
                        process::exit(1);
                    });

                    // Assemble with ml64.exe
                    let ml64 = find_ml64();
                    let obj_path = output.replace(".exe", ".obj");
                    if let Some(ml64) = ml64 {
                        let status = process::Command::new(&ml64)
                            .args(["/c", "/nologo", &format!("/Fo{}", obj_path), &asm_path])
                            .output();
                        match status {
                            Ok(out) => {
                                if !out.status.success() {
                                    eprintln!("ml64.exe failed:");
                                    eprintln!("STDOUT: {}", String::from_utf8_lossy(&out.stdout));
                                    eprintln!("STDERR: {}", String::from_utf8_lossy(&out.stderr));
                                    process::exit(1);
                                }
                            }
                            Err(e) => {
                                eprintln!("Failed to run ml64.exe: {}", e);
                                eprintln!("Output assembly left at: {}", asm_path);
                                process::exit(1);
                            }
                        }
                    } else {
                        eprintln!("ml64.exe not found. Assembly written to: {}", asm_path);
                        eprintln!("Assemble manually: ml64 /c /Fo{} {}", obj_path, asm_path);
                        process::exit(1);
                    }

                    // Link with link.exe
                    let link = find_link();
                    let kernel32_lib = find_kernel32_lib();
                    if let (Some(link), Some(lib_path)) = (link, kernel32_lib) {
                        let status = process::Command::new(&link)
                            .args([
                                "/nologo",
                                "/SUBSYSTEM:CONSOLE",
                                "/ENTRY:_main",
                                &format!("/LIBPATH:{}", lib_path),
                                &obj_path,
                                "kernel32.lib",
                                &format!("/OUT:{}", output),
                            ])
                            .output();
                        match status {
                            Ok(out) => {
                                if !out.status.success() {
                                    eprintln!("link.exe failed:");
                                    eprintln!("{}", String::from_utf8_lossy(&out.stderr));
                                    process::exit(1);
                                }
                            }
                            Err(e) => {
                                eprintln!("Failed to run link.exe: {}", e);
                                eprintln!("Object file left at: {}", obj_path);
                                process::exit(1);
                            }
                        }
                        println!("Built native executable: {}", output);
                    } else {
                        eprintln!("link.exe or kernel32.lib not found. Object file at: {}", obj_path);
                        eprintln!("Link manually: link /SUBSYSTEM:CONSOLE /ENTRY:_main /LIBPATH:<path> {} kernel32.lib /OUT:{}", obj_path, output);
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

        "emit" => {
            if args.len() < 3 {
                eprintln!("Usage: pledgec emit <file.p> [-o output.exe]");
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
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

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
            let source = match fs::read_to_string(input) {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Error reading '{}': {}", input, e);
                    process::exit(1);
                }
            };

            match compiler::interpret_source_with_path(&source, input, verbose) {
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
                    let c_path = output.replace(".exe", ".c");
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
                        process::Command::new(&cc)
                            .args(["-O2", "-o", &output, &c_path])
                            .output()
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
    eprintln!("  pledgec build   <file.p> [-o output.exe]  Build native x86_64 executable");
    eprintln!("  pledgec emit    <file.p> [-o output.exe]  Emit PE directly (no ml64.exe/link.exe)");
    eprintln!("  pledgec ccompile <file.p> [-o output.exe]  Compile via C codegen (gcc/clang)");
    eprintln!("           [--target <triple>]                Cross-compile (e.g. aarch64-linux-gnu)");
    eprintln!("  pledgec compile <file.p> [-o output.s]    Compile to ARM64 assembly (legacy)");
    eprintln!("  pledgec run     <file.p> [-v]             Interpret Pledge IR (execute)");
    eprintln!("  pledgec ir      <file.p>                  Show Pledge IR");
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
