use crate::ast::Program;
use crate::codegen::CodeGen;
use crate::codegen_c;
use crate::interp;
use crate::lexer::Lexer;
use crate::lower;
use crate::mono;
use crate::parser::Parser;
use crate::typechecker::TypeChecker;
use crate::x86;
use crate::pe;

pub use crate::interp::RunResult;
pub use crate::interp::Value;

use std::path::Path;
use std::collections::{HashMap, HashSet};

pub struct CompileResult {
    pub assembly: String,
}

// ─── Stdlib auto-include ─────────────────────────────────────────────────
// Phase 1: stdlib/core.p is automatically prepended to every program.
// User definitions take priority over stdlib duplicates (user can override).

const STDLIB_CORE: &str = include_str!("../stdlib/core.p");

fn parse_source(source: &str) -> Result<Program, Vec<String>> {
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::new(tokens);
    parser.parse().map_err(|e| vec![e])
}

fn build_program(source: &str) -> Result<Program, Vec<String>> {
    build_program_with_path(source, None)
}

/// Build a program with optional file path for resolving `use` imports.
/// When source_path is provided, `use foo::bar;` will look for `foo.p` or
/// `foo/mod.p` relative to the source file's directory.
fn build_program_with_path(source: &str, source_path: Option<&str>) -> Result<Program, Vec<String>> {
    // Parse stdlib first
    let stdlib = parse_source(STDLIB_CORE)?;

    // Extract stdlib struct/enum names so the user parser recognizes them
    let mut struct_names: Vec<String> = stdlib.structs.iter().map(|s| s.name.clone()).collect();
    let mut enum_names: Vec<String> = stdlib.enums.iter().map(|e| e.name.clone()).collect();

    // Parse user code with stdlib names pre-populated
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::with_names(tokens, struct_names.clone(), enum_names.clone());
    let user = parser.parse().map_err(|e| vec![e])?;

    // Collect all known struct/enum names (stdlib + user + resolved modules)
    struct_names.extend(user.structs.iter().map(|s| s.name.clone()));
    enum_names.extend(user.enums.iter().map(|e| e.name.clone()));

    // Resolve use imports — load files from disk
    let mut imported_modules: Vec<(String, Program)> = Vec::new();
    if let Some(src_path) = source_path {
        let base_dir = Path::new(src_path)
            .parent()
            .unwrap_or(Path::new("."));

        for import in &user.imports {
            // Skip "core" — it's auto-included
            if import.path == "core" {
                continue;
            }

            // Try: <base_dir>/<path>.p  and  <base_dir>/<path>/mod.p
            let path_str = import.path.replace("::", "/");
            let candidates = [
                base_dir.join(format!("{}.p", path_str)),
                base_dir.join(format!("{}/mod.p", path_str)),
                base_dir.join("pledge_modules").join(format!("{}.p", path_str)),
                base_dir.join("pledge_modules").join(format!("{}/mod.p", path_str)),
            ];

            let mut found = None;
            for candidate in &candidates {
                if candidate.exists() {
                    found = Some(candidate.clone());
                    break;
                }
            }

            if let Some(file_path) = found {
                let module_source = std::fs::read_to_string(&file_path)
                    .map_err(|e| vec![format!("Error reading '{}': {}", file_path.display(), e)])?;

                // Parse the imported module with known names
                let mut lex = Lexer::new(&module_source);
                let toks = lex.tokenize();
                let mut p = Parser::with_names(toks, struct_names.clone(), enum_names.clone());
                let imported = p.parse().map_err(|e| vec![e])?;

                let module_name = imported.module.clone()
                    .unwrap_or_else(|| import.path.clone());

                // Extract names from imported module for future imports
                struct_names.extend(imported.structs.iter().map(|s| s.name.clone()));
                enum_names.extend(imported.enums.iter().map(|e| e.name.clone()));

                imported_modules.push((module_name, imported));
            } else {
                // Suggest available modules in the base directory
                let mut suggestions: Vec<String> = Vec::new();
                if let Ok(entries) = std::fs::read_dir(base_dir) {
                    for entry in entries.flatten() {
                        if let Some(name) = entry.file_name().to_str() {
                            if name.ends_with(".p") {
                                suggestions.push(name.trim_end_matches(".p").to_string());
                            }
                        }
                    }
                }
                let suggestion = if !suggestions.is_empty() {
                    // Find closest match
                    let mut best: Option<(usize, &String)> = None;
                    for s in &suggestions {
                        let dist = edit_distance(&import.path, s);
                        if dist <= 3 && best.map_or(true, |(b, _)| dist < b) {
                            best = Some((dist, s));
                        }
                    }
                    if let Some((_, s)) = best {
                        format!(" — did you mean module '{}'?", s)
                    } else {
                        format!(" — available modules: {}", suggestions.join(", "))
                    }
                } else {
                    String::new()
                };
                return Err(vec![format!(
                    "Import error: could not resolve module '{}'{} (searched: {})",
                    import.path,
                    suggestion,
                    candidates.iter().map(|c| c.display().to_string()).collect::<Vec<_>>().join(", ")
                )]);
            }
        }
    }

    // Merge: start with stdlib, then add imported modules, then user (user wins)
    let mut program = stdlib.clone();

    // Merge imported modules
    for (_, mod_prog) in &imported_modules {
        program = merge_programs(program, mod_prog.clone());
    }

    // Merge user code (highest priority)
    program = merge_programs(program, user.clone());

    // Type check — register each module with its own name for visibility enforcement
    let mut tc = TypeChecker::new();
    let stdlib_module = stdlib.module.clone().unwrap_or_else(|| "core".to_string());
    tc.register_program(&stdlib, &stdlib_module);

    for (mod_name, mod_prog) in &imported_modules {
        tc.register_program(mod_prog, mod_name);
    }

    let user_module = user.module.clone().unwrap_or_else(|| "main".to_string());
    tc.register_program(&user, &user_module);

    // Collect capabilities from user's use statements (with { IO, ... })
    tc.collect_capabilities(&user);

    // Check bodies of the merged program from the user's module context
    tc.current_module = user_module.clone();
    tc.check_bodies(&program);
    if !tc.errors.is_empty() {
        return Err(tc.errors);
    }

    // Monomorphize generics
    Ok(mono::monomorphize(&program))
}

fn merge_programs(stdlib: Program, user: Program) -> Program {
    let user_func_names: std::collections::HashSet<String> =
        user.funcs.iter().map(|f| f.name.clone()).collect();
    let user_struct_names: std::collections::HashSet<String> =
        user.structs.iter().map(|s| s.name.clone()).collect();
    let user_enum_names: std::collections::HashSet<String> =
        user.enums.iter().map(|e| e.name.clone()).collect();
    let user_const_names: std::collections::HashSet<String> =
        user.consts.iter().map(|c| c.name.clone()).collect();
    let user_alias_names: std::collections::HashSet<String> =
        user.type_aliases.iter().map(|t| t.name.clone()).collect();
    let user_impl_keys: std::collections::HashSet<(String, Vec<String>)> =
        user.impls.iter().map(|i| (i.struct_name.clone(), i.type_params.clone())).collect();

    let mut merged = user;

    // Add stdlib items that don't conflict with user definitions
    for f in stdlib.funcs {
        if !user_func_names.contains(&f.name) {
            merged.funcs.push(f);
        }
    }
    for s in stdlib.structs {
        if !user_struct_names.contains(&s.name) {
            merged.structs.push(s);
        }
    }
    for e in stdlib.enums {
        if !user_enum_names.contains(&e.name) {
            merged.enums.push(e);
        }
    }
    for c in stdlib.consts {
        if !user_const_names.contains(&c.name) {
            merged.consts.push(c);
        }
    }
    for t in stdlib.type_aliases {
        if !user_alias_names.contains(&t.name) {
            merged.type_aliases.push(t);
        }
    }
    for i in stdlib.impls {
        let key = (i.struct_name.clone(), i.type_params.clone());
        if !user_impl_keys.contains(&key) {
            merged.impls.push(i);
        }
    }

    // Merge traits (user wins on name conflicts)
    let user_trait_names: std::collections::HashSet<String> =
        merged.traits.iter().map(|t| t.name.clone()).collect();
    for t in stdlib.traits {
        if !user_trait_names.contains(&t.name) {
            merged.traits.push(t);
        }
    }

    // Merge trait impls (user wins on (trait, type) conflicts)
    let user_trait_impl_keys: std::collections::HashSet<(String, String)> =
        merged.trait_impls.iter().map(|ti| (ti.trait_name.clone(), ti.type_name.clone())).collect();
    for ti in stdlib.trait_impls {
        let key = (ti.trait_name.clone(), ti.type_name.clone());
        if !user_trait_impl_keys.contains(&key) {
            merged.trait_impls.push(ti);
        }
    }

    merged
}

pub fn compile(source: &str) -> Result<CompileResult, Vec<String>> {
    let program = build_program(source)?;

    // Code generation
    let mut codegen = CodeGen::new();
    let assembly = codegen.generate(&program);

    Ok(CompileResult { assembly })
}

pub fn lower_to_ir(source: &str) -> Result<String, Vec<String>> {
    let program = build_program(source)?;

    // Lower to IR
    let module = lower::lower(&program);

    Ok(module.to_string())
}

pub fn lex(source: &str) -> Vec<String> {
    let mut lexer = Lexer::new(source);
    lexer
        .tokenize()
        .iter()
        .map(|t| format!("  {:>4}:{:<3} {:?}", t.line, t.col, t.kind))
        .collect()
}

pub fn parse_to_ast(source: &str) -> Result<String, String> {
    let stdlib = parse_source(STDLIB_CORE).map_err(|e| e.join("\n"))?;
    let struct_names: Vec<String> = stdlib.structs.iter().map(|s| s.name.clone()).collect();
    let enum_names: Vec<String> = stdlib.enums.iter().map(|e| e.name.clone()).collect();
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::with_names(tokens, struct_names, enum_names);
    let user = parser.parse()?;
    let program = merge_programs(stdlib, user);
    Ok(format!("{:#?}", program))
}

pub fn interpret_source(source: &str, verbose: bool) -> Result<RunResult, Vec<String>> {
    let program = build_program(source)?;

    // Lower to IR
    let module = lower::lower(&program);

    // Interpret
    Ok(interp::interpret(module, verbose))
}

pub fn interpret_source_with_path(source: &str, source_path: &str, verbose: bool) -> Result<RunResult, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Interpret
    Ok(interp::interpret(module, verbose))
}

/// Build source to x86_64 assembly (Stage 2 — no C runtime)
pub fn build_source(source: &str) -> Result<String, Vec<String>> {
    let program = build_program(source)?;

    // Lower to IR
    let module = lower::lower(&program);

    // Generate x86_64 assembly
    Ok(x86::generate_asm(&module))
}

/// Compile to C source code, then compile with gcc/clang to native binary
pub fn ccompile_source_with_path(source: &str, source_path: &str) -> Result<String, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Generate C code
    Ok(codegen_c::generate_c(&module))
}

pub fn build_source_with_path(source: &str, source_path: &str) -> Result<String, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Generate x86_64 assembly
    Ok(x86::generate_asm(&module))
}

/// Direct PE emission — generate native .exe without ml64.exe or link.exe
pub fn emit_source_with_path(source: &str, source_path: &str) -> Result<Vec<u8>, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Generate PE file directly
    Ok(pe::generate_pe(&module))
}

/// Simple Levenshtein edit distance for module name suggestions.
fn edit_distance(a: &str, b: &str) -> usize {
    let a: Vec<char> = a.chars().collect();
    let b: Vec<char> = b.chars().collect();
    let mut dp = vec![vec![0; b.len() + 1]; a.len() + 1];
    for i in 0..=a.len() { dp[i][0] = i; }
    for j in 0..=b.len() { dp[0][j] = j; }
    for i in 1..=a.len() {
        for j in 1..=b.len() {
            if a[i-1] == b[j-1] {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + dp[i-1][j].min(dp[i][j-1]).min(dp[i-1][j-1]);
            }
        }
    }
    dp[a.len()][b.len()]
}
