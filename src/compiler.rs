use crate::ast::Program;
use crate::autograd;
use crate::codegen::CodeGen;
use crate::codegen_c;
use crate::comptime;
use crate::interp;
use crate::lexer::Lexer;
use crate::lower;
use crate::mono;
use crate::parser::Parser;
use crate::typechecker::TypeChecker;
use crate::x86;
use crate::pe;
use crate::pir;
use crate::ir::{Effect, EffectSet, IrModule, Inst};
use crate::wasm;

pub use crate::interp::RunResult;
pub use crate::interp::Value;

use std::path::Path;
use std::collections::{HashMap, HashSet};

pub struct CompileResult {
    pub assembly: String,
}

// ─── Stdlib auto-include ─────────────────────────────────────────────────
// Phase 1: stdlib/core.fd is automatically prepended to every program.
// User definitions take priority over stdlib duplicates (user can override).

const STDLIB_CORE: &str = include_str!("../stdlib/core.fd");

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
/// When source_path is provided, `use foo::bar;` will look for `foo.fd` or
/// `foo/mod.fd` relative to the source file's directory.
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

            // Try: <base_dir>/<path>.fd  and  <base_dir>/<path>/mod.fd
            let path_str = import.path.replace("::", "/");
            let candidates = [
                base_dir.join(format!("{}.fd", path_str)),
                base_dir.join(format!("{}/mod.fd", path_str)),
                base_dir.join("fides_modules").join(format!("{}.fd", path_str)),
                base_dir.join("fides_modules").join(format!("{}/mod.fd", path_str)),
                // Fallback: stdlib/ directory (for json, http, db, etc.)
                std::path::Path::new("stdlib").join(format!("{}.fd", path_str)),
                std::path::Path::new("stdlib").join(format!("{}/mod.fd", path_str)),
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
                            if name.ends_with(".fd") {
                                suggestions.push(name.trim_end_matches(".fd").to_string());
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

    // Desugar layer/model declarations into structs + impls + functions
    desugar_layers_models(&mut program);

    // Phase 3A: Process comptime gen blocks (before typechecking so generated
    // code is visible to the typechecker)
    program = comptime::process_gen(program);

    // Phase 3B.1: Fill in default trait methods.
    fill_default_trait_methods(&mut program);

    // Type check — register each module with its own name for visibility enforcement
    let mut tc = TypeChecker::new();
    let stdlib_module = stdlib.module.clone().unwrap_or_else(|| "core".to_string());
    tc.register_program(&stdlib, &stdlib_module);

    for (mod_name, mod_prog) in &imported_modules {
        tc.register_program(mod_prog, mod_name);
    }

    let user_module = user.module.clone().unwrap_or_else(|| "main".to_string());
    tc.register_program(&user, &user_module);

    // Re-register trait impl methods from the merged program so that
    // default trait methods (filled in by fill_default_trait_methods) are
    // visible to the typechecker.
    tc.register_trait_impls(&program);

    // Register comptime-generated functions and structs from the merged program
    // (these are not in the original user or stdlib programs)
    tc.register_generated_decls(&program);

    // Phase 3B.4: Verify trait impls match trait definitions
    tc.verify_trait_impls(&program);

    // Collect capabilities from user's use statements (with { IO, ... })
    tc.collect_capabilities(&user);

    // Check bodies of the merged program from the user's module context
    tc.current_module = user_module.clone();
    tc.check_bodies(&program);
    if !tc.errors.is_empty() {
        return Err(tc.errors);
    }

    // Phase 3A: comptime substitution
    let program = match comptime::substitute(&program) {
        Ok(p) => p,
        Err(errs) => return Err(errs),
    };

    // Monomorphize generics
    Ok(mono::monomorphize(&program))
}

/// Desugar layer/model declarations into structs + impls + constructor functions.
///
/// `layer Dense { w: Tensor, b: Tensor, forward(self, x: Tensor) -> Tensor { ... } }`
/// desugars to:
///   struct Dense { w: Tensor, b: Tensor }
///   impl Dense { pf forward(self: Dense, x: Tensor) -> Tensor { ... } }
///   pf Dense_new(w: Tensor, b: Tensor) -> Dense { Dense { w, b } }  // constructor
///
/// `model MLP { l1: Dense(784, 128), l2: Dense(128, 10), forward(self, x) -> T { ... } }`
/// desugars to:
///   struct MLP { l1: Dense, l2: Dense }
///   impl MLP { pf forward(self: MLP, x: Tensor) -> Tensor { ... } }
///   pf MLP_new() -> MLP { MLP { l1: Dense_new(784, 128), l2: Dense_new(128, 10) } }
fn desugar_layers_models(program: &mut Program) {
    use crate::ast::{Func, ImplDef, Param, StructDef, Type, Expr, Stmt, Annotation};

    // Desugar layers
    for layer in program.layers.drain(..) {
        // Create struct
        program.structs.push(StructDef {
            name: layer.name.clone(),
            fields: layer.fields.clone(),
            is_pub: layer.is_pub,
        });

        // Create impl block with forward method
        let mut forward = layer.forward.clone();
        forward.is_pub = true;
        program.impls.push(ImplDef {
            struct_name: layer.name.clone(),
            type_params: Vec::new(),
            methods: vec![forward],
        });

        // Create constructor function: LayerName_new(fields...) -> LayerName
        let ctor_name = format!("{}_new", layer.name);
        let ctor_params = layer.fields.clone();
        let ctor_body = vec![Stmt::Return(
            Some(Expr::StructLit {
                name: layer.name.clone(),
                fields: layer.fields.iter().map(|f| {
                    (f.name.clone(), Expr::Ident(f.name.clone()))
                }).collect(),
                spread: None,
            }),
            0,
        )];
        program.funcs.push(Func {
            name: ctor_name,
            type_params: Vec::new(),
            type_param_bounds: Vec::new(),
            const_params: Vec::new(),
            params: ctor_params,
            ret_ty: Type::Named(layer.name.clone()),
            body: ctor_body,
            is_pub: layer.is_pub,
            is_comptime: false,
            declared_effects: Vec::new(),
            has_effect_annotation: false,
            is_interrupt: false,
            annotations: Vec::new(),
        });
    }

    // Desugar models
    for model in program.models.drain(..) {
        // Create struct with layer fields
        let fields: Vec<Param> = model.layers.iter().map(|(name, layer_type, _)| {
            Param {
                name: name.clone(),
                ty: Type::Named(layer_type.clone()),
            }
        }).collect();

        program.structs.push(StructDef {
            name: model.name.clone(),
            fields,
            is_pub: model.is_pub,
        });

        // Create impl block with forward method
        let mut forward = model.forward.clone();
        forward.is_pub = true;
        program.impls.push(ImplDef {
            struct_name: model.name.clone(),
            type_params: Vec::new(),
            methods: vec![forward],
        });

        // Create constructor: ModelName_new() -> ModelName
        // Body: ModelName { l1: LayerName_new(args...), l2: LayerName_new(args...), ... }
        let ctor_name = format!("{}_new", model.name);
        let struct_lit = Expr::StructLit {
            name: model.name.clone(),
            fields: model.layers.iter().map(|(field_name, layer_type, ctor_args)| {
                let mut args = Vec::new();
                for arg in ctor_args {
                    // Try to parse as int
                    if let Ok(n) = arg.parse::<i64>() {
                        args.push(Expr::IntLit(n));
                    } else {
                        args.push(Expr::Ident(arg.clone()));
                    }
                }
                let layer_ctor = format!("{}_new", layer_type);
                (field_name.clone(), Expr::Call {
                    callee: layer_ctor,
                    args,
                })
            }).collect(),
            spread: None,
        };
        let ctor_body = vec![Stmt::Return(Some(struct_lit), 0)];
        program.funcs.push(Func {
            name: ctor_name,
            type_params: Vec::new(),
            type_param_bounds: Vec::new(),
            const_params: Vec::new(),
            params: Vec::new(),
            ret_ty: Type::Named(model.name.clone()),
            body: ctor_body,
            is_pub: model.is_pub,
            is_comptime: false,
            declared_effects: Vec::new(),
            has_effect_annotation: false,
            is_interrupt: false,
            annotations: Vec::new(),
        });
    }
}

/// Phase 3B.1: Fill in default trait methods.
fn fill_default_trait_methods(program: &mut Program) {
    use crate::ast::{Func, Param, Type};

    let trait_map: HashMap<String, &crate::ast::TraitDef> = program.traits.iter()
        .map(|t| (t.name.clone(), t))
        .collect();

    for ti in program.trait_impls.iter_mut() {
        let trait_def = match trait_map.get(&ti.trait_name) {
            Some(t) => t,
            None => continue,
        };

        let provided: HashSet<String> = ti.methods.iter().map(|m| m.name.clone()).collect();

        for tm in &trait_def.methods {
            if provided.contains(&tm.name) {
                continue;
            }
            let default_body = match &tm.default_body {
                Some(b) => b.clone(),
                None => continue,
            };

            let self_type = Type::Named(ti.type_name.clone());
            let params: Vec<Param> = tm.params.iter().map(|p| {
                if p.name == "self" {
                    Param { name: p.name.clone(), ty: self_type.clone() }
                } else {
                    p.clone()
                }
            }).collect();

            ti.methods.push(Func {
                name: tm.name.clone(),
                type_params: Vec::new(),
                type_param_bounds: Vec::new(),
                const_params: Vec::new(),
                params,
                ret_ty: tm.ret_ty.clone(),
                body: default_body,
                is_pub: tm.is_pub,
                is_comptime: false,
                declared_effects: Vec::new(),
                has_effect_annotation: false,
                is_interrupt: false,
            annotations: Vec::new(),
            });

        }
    }
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

/// Lower source to an IR module (for verification and analysis).
pub fn lower_to_ir_module(source: &str) -> Result<crate::ir::IrModule, Vec<String>> {
    let program = build_program(source)?;
    Ok(lower::lower(&program))
}

/// Lower source to PIR L2 (for hot-reload change detection).
/// Returns the L2 module which can be compared across reloads.
pub fn lower_to_l2(source: &str, source_path: &str) -> Result<crate::pir::L2Module, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;
    let module = lower::lower(&program);
    let l1 = pir::lower_ir_to_l1(&module);
    let l2 = pir::lower_l1_to_l2(&l1);
    Ok(l2)
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
    Ok(interp::interpret(module, verbose, false))
}

pub fn interpret_source_with_path(source: &str, source_path: &str, verbose: bool) -> Result<RunResult, Vec<String>> {
    interpret_source_with_path_release(source, source_path, verbose, false)
}

pub fn interpret_source_with_path_release(source: &str, source_path: &str, verbose: bool, release: bool) -> Result<RunResult, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Interpret
    Ok(interp::interpret(module, verbose, release))
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

/// Compile to WASM binary (Phase 5D.1).
/// Parses, typechecks, lowers to IR, then generates WASM bytecode.
pub fn compile_to_wasm(source: &str, source_path: &str, wat_mode: bool) -> Result<Vec<u8>, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;
    let module = lower::lower(&program);
    if wat_mode {
        Ok(wasm::dump_wat(&module).into_bytes())
    } else {
        Ok(wasm::generate_wasm(&module))
    }
}

/// Build a native executable in one step: parse → typecheck → lower → PIR →
/// optimize → codegen → link. Zero external tools required.
/// Supports x86-64 (full ELF linking), arm64 and riscv (assembly text output).
pub fn build_executable(source: &str, source_path: &str, target: &str, do_opt: bool, granted_caps: Option<&[String]>) -> Result<Vec<u8>, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;
    let mut module = lower::lower(&program);

    // Autograd pass: process grad blocks, generate backward code
    autograd::process_module(&mut module);

    // F5: Capability-aware linking — strip functions requiring ungranted capabilities
    if let Some(caps) = granted_caps {
        strip_ungranted_caps(&mut module, caps);
    }

    let l1 = pir::lower_ir_to_l1(&module);
    let mut l2 = pir::lower_l1_to_l2(&l1);

    if do_opt {
        let _ = crate::opt::optimize_l2(&mut l2);
    }

    let l3 = pir::lower_l2_to_l3(&l2, &module);

    match target {
        "x86-64" | "x64" => {
            let asm = crate::codegen_x64::generate_asm(&l3);
            // Always run peephole optimization on the generated assembly —
            // this catches redundant moves, zero idioms, dead stores, etc.
            // These are safe local transformations that don't affect semantics.
            let asm = crate::peephole::optimize(&asm);
            let obj = crate::elf64::emit_object_file(&l3, &asm)
                .map_err(|e| vec![e])?;
            // Link with the pre-compiled Linux runtime (raw machine code)
            let (rt_text, rt_syms, rt_rodata, rt_rodata_syms) = crate::runtime_linux::runtime_parts();
            let rt_relocs = crate::runtime_linux::runtime_relocations();
            crate::linker::link_with_runtime(
                &[obj], "_start",
                &rt_text, &rt_syms,
                &rt_rodata, &rt_rodata_syms,
                &rt_relocs,
            )
                .map_err(|e| vec![e])
        }
        "arm64" | "aarch64" => {
            let asm = crate::codegen_arm64::generate_asm(&l3);
            // Emit a valid ELF64 relocatable object for AArch64
            let obj = crate::elf64::emit_object_file_for_target(&l3, &asm, "arm64")
                .map_err(|e| vec![e])?;
            Ok(obj)
        }
        "riscv" | "riscv64" => {
            let asm = crate::codegen_riscv::generate_asm(&l3);
            // Emit a valid ELF64 relocatable object for RISC-V
            let obj = crate::elf64::emit_object_file_for_target(&l3, &asm, "riscv")
                .map_err(|e| vec![e])?;
            Ok(obj)
        }
        _ => Err(vec![format!("Unknown target: {}", target)])
    }
}

/// Bare-metal mode: skip stdlib, produce a flat binary (no PE headers, no C runtime).
/// The entry point is the `main` function. The binary is loaded at address 0x10000.
/// This is for No-OS / kernel / bootloader development (Phase 5A.2).
pub fn emit_bare(source: &str, source_path: &str) -> Result<Vec<u8>, Vec<String>> {
    // Parse user code WITHOUT stdlib
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::new(tokens);
    let user = parser.parse().map_err(|e| vec![e])?;

    // Typecheck user code (no stdlib registration)
    let mut tc = TypeChecker::new();
    let user_module = user.module.clone().unwrap_or_else(|| "bare".to_string());
    tc.register_program(&user, &user_module);
    tc.collect_capabilities(&user);
    tc.check_bodies(&user);
    if !tc.errors.is_empty() {
        return Err(tc.errors);
    }

    // Lower to IR
    let module = lower::lower(&user);

    // Generate flat binary (raw machine code, no PE headers)
    Ok(pe::generate_flat_binary(&module))
}

/// Boot mode: generate a Multiboot2-compatible kernel binary (Phase 5A.5).
/// Same as bare mode but with a multiboot2 header and boot entry point.
pub fn emit_boot(source: &str, source_path: &str) -> Result<Vec<u8>, Vec<String>> {
    // Parse user code WITHOUT stdlib
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::new(tokens);
    let user = parser.parse().map_err(|e| vec![e])?;

    // Typecheck user code (no stdlib registration)
    let mut tc = TypeChecker::new();
    let user_module = user.module.clone().unwrap_or_else(|| "boot".to_string());
    tc.register_program(&user, &user_module);
    tc.collect_capabilities(&user);
    tc.check_bodies(&user);
    if !tc.errors.is_empty() {
        return Err(tc.errors);
    }

    // Lower to IR
    let module = lower::lower(&user);

    // Generate multiboot2-compatible boot binary
    Ok(pe::generate_boot_binary(&module))
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

/// F5: Strip functions that require capabilities not in the granted set.
/// Maps capability names to effects: IO->IO, FS->IO, Net->Net, Spawn->Spawn.
/// After removing ungranted-cap functions, does a reachability check from
/// "main" to remove any functions that are now unreachable.
fn strip_ungranted_caps(module: &mut IrModule, granted_caps: &[String]) {
    use std::collections::{HashMap, HashSet};

    let granted: HashSet<String> = granted_caps.iter().map(|s| s.to_string()).collect();
    let _ = granted;

    // Map capability names to the effects they grant
    let granted_effects: HashSet<Effect> = granted_caps.iter().filter_map(|cap| {
        match cap.as_str() {
            "IO" | "FS" => Some(Effect::IO),
            "Net" => Some(Effect::Net),
            "Spawn" => Some(Effect::Spawn),
            _ => None,
        }
    }).collect();

    // Find functions that require effects not in the granted set
    let mut to_remove: HashSet<String> = HashSet::new();
    for (name, effects) in &module.function_effects {
        if name == "main" || name == "_start" {
            continue;
        }
        for eff in effects.iter() {
            match eff {
                Effect::IO | Effect::Net | Effect::Spawn => {
                    if !granted_effects.contains(&eff) {
                        to_remove.insert(name.clone());
                        break;
                    }
                }
                _ => {}
            }
        }
    }

    if to_remove.is_empty() {
        return;
    }

    // Build call graph
    let mut call_graph: HashMap<String, Vec<String>> = HashMap::new();
    for (name, func) in &module.functions {
        let mut callees = Vec::new();
        for (_, inst) in &func.instructions {
            if let Inst::Call { func: callee, .. } = inst {
                callees.push(callee.clone());
            }
        }
        call_graph.insert(name.clone(), callees);
    }

    // BFS from main to find reachable functions (skipping ungranted-cap ones)
    let mut reachable: HashSet<String> = HashSet::new();
    let mut queue: Vec<String> = vec!["main".to_string()];
    while let Some(name) = queue.pop() {
        if reachable.contains(&name) || to_remove.contains(&name) {
            continue;
        }
        reachable.insert(name.clone());
        if let Some(callees) = call_graph.get(&name) {
            for callee in callees {
                if !reachable.contains(callee) {
                    queue.push(callee.clone());
                }
            }
        }
    }

    // Remove unreachable functions
    let removed: Vec<String> = module.functions.keys()
        .filter(|k| !reachable.contains(*k))
        .cloned()
        .collect();
    for name in &removed {
        module.functions.remove(name);
        module.function_effects.remove(name);
    }
}
