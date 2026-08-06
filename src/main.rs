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
                eprintln!("Usage: pledgec ccompile <file.p> [-o output.exe]");
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

            match compiler::ccompile_source_with_path(&source, input) {
                Ok(c_code) => {
                    // Write C source
                    let c_path = output.replace(".exe", ".c");
                    fs::write(&c_path, &c_code).unwrap_or_else(|e| {
                        eprintln!("Error writing '{}': {}", c_path, e);
                        process::exit(1);
                    });
                    println!("Generated C source: {}", c_path);

                    // Find a C compiler: try CC env var, then gcc, clang, cl.exe
                    let cc = std::env::var("CC").unwrap_or_else(|_| {
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
                    });

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
    eprintln!("  pledgec compile <file.p> [-o output.s]    Compile to ARM64 assembly (legacy)");
    eprintln!("  pledgec run     <file.p> [-v]             Interpret Pledge IR (execute)");
    eprintln!("  pledgec ir      <file.p>                  Show Pledge IR");
    eprintln!("  pledgec tokens  <file.p>                  Show tokens");
    eprintln!("  pledgec ast     <file.p>                  Show AST");
    eprintln!("  pledgec version                           Show version");
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
