use crate::ast::Program;
use crate::autograd;
use crate::codegen::CodeGen;
use crate::codegen_c;
use crate::comptime;
use crate::diag::ErrCode;
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

/// Enrich a list of error strings with source context display.
///
/// Parses the "line N" or "at L:C" from each error string, extracts the
/// corresponding source line, and renders a full Diag with caret.
/// Errors that don't have line info are passed through unchanged.
fn enrich_errors(errors: &[String], source: &str, file: &str) -> Vec<String> {
    use crate::diag::{ErrCode, error_with_source};

    let source_lines: Vec<&str> = source.lines().collect();
    let mut result = Vec::new();

    for err in errors {
        let (line, col) = extract_line_col(err);
        if line == 0 || line > source_lines.len() {
            result.push(err.clone());
            continue;
        }

        let source_line = source_lines[line - 1];
        let code = classify_error(err);
        let span_len = estimate_span(err, source_line, col);

        let mut diag = error_with_source(code, err.clone(), file, line, col, source, span_len);

        if let Some(help) = extract_help(err) {
            diag = diag.with_help(help);
        }

        result.push(diag.render());
    }

    result
}

/// Extract (line, col) from an error string.
fn extract_line_col(err: &str) -> (usize, usize) {
    // "Type error (line N): ..."
    if let Some(idx) = err.find("(line ") {
        let rest = &err[idx + 6..];
        if let Some(end) = rest.find(')') {
            if let Ok(line) = rest[..end].parse::<usize>() {
                return (line, 1);
            }
        }
    }
    // "Parse error at L:C: ..."
    if let Some(idx) = err.find("at ") {
        let rest = &err[idx + 3..];
        if let Some(colon) = rest.find(':') {
            if let Ok(line) = rest[..colon].parse::<usize>() {
                let after_colon = &rest[colon + 1..];
                if let Some(end) = after_colon.find(':') {
                    if let Ok(col) = after_colon[..end].parse::<usize>() {
                        return (line, col);
                    }
                }
                return (line, 1);
            }
        }
    }
    // "Error:L:C: ..." (lexer format)
    if err.starts_with("Error:") {
        let rest = &err[6..];
        if let Some(colon1) = rest.find(':') {
            if let Ok(line) = rest[..colon1].parse::<usize>() {
                let after = &rest[colon1 + 1..];
                if let Some(colon2) = after.find(':') {
                    if let Ok(col) = after[..colon2].parse::<usize>() {
                        return (line, col);
                    }
                }
                return (line, 1);
            }
        }
    }
    (0, 0)
}

/// Classify an error string into an ErrCode.
fn classify_error(err: &str) -> ErrCode {
    use crate::diag::ErrCode;
    // Normalize: strip "(line N)" from "Type error (line N): ..." → "Type error: ..."
    let normalized = if err.starts_with("Type error (line ") {
        if let Some(idx) = err.find("): ") {
            format!("Type error: {}", &err[idx + 3..])
        } else {
            err.to_string()
        }
    } else {
        err.to_string()
    };
    let e = normalized.as_str();
    if e.starts_with("Type error: undefined variable") { ErrCode::UndefinedVariable }
    else if e.starts_with("Type error: undefined function") { ErrCode::UndefinedFunction }
    else if e.starts_with("Type error: undefined struct") { ErrCode::UndefinedStruct }
    else if e.starts_with("Type error: struct") && e.contains("has no field") { ErrCode::UndefinedField }
    else if e.contains("cannot assign") { ErrCode::TypeMismatch }
    else if e.starts_with("Type error: if condition must be bool") { ErrCode::IfConditionNotBool }
    else if e.contains("expects") && e.contains("args") { ErrCode::ArgCountMismatch }
    else if e.contains("missing return") { ErrCode::MissingReturn }
    else if e.starts_with("Init error") { ErrCode::InitBeforeAssign }
    else if e.starts_with("Linear error") || e.contains("linear") { ErrCode::NotLinear }
    else if e.starts_with("Capability error") { ErrCode::CapabilityDenied }
    else if e.starts_with("Sandbox error") { ErrCode::SandboxViolation }
    else if e.starts_with("Conservation error") { ErrCode::ConservationViolation }
    else if e.starts_with("Gas error") { ErrCode::GasExceeded }
    else if e.starts_with("Effect error") { ErrCode::EffectMismatch }
    else if e.starts_with("Parse error") { ErrCode::ExpectedToken }
    else if e.starts_with("Error:") { ErrCode::UnexpectedChar }
    else if e.starts_with("borrow error") { ErrCode::BorrowConflict }
    else if e.starts_with("Panic:") { ErrCode::NoMain }
    else { ErrCode::Custom }
}

/// Estimate the span length for the caret.
fn estimate_span(err: &str, source_line: &str, _col: usize) -> usize {
    if let Some(idx) = err.find("variable '") {
        let rest = &err[idx + 10..];
        if let Some(end) = rest.find('\'') {
            let name = &rest[..end];
            if source_line.contains(name) { return name.len(); }
        }
    }
    if let Some(idx) = err.find("function '") {
        let rest = &err[idx + 10..];
        if let Some(end) = rest.find('\'') {
            let name = &rest[..end];
            if source_line.contains(name) { return name.len(); }
        }
    }
    if let Some(idx) = err.find("field '") {
        let rest = &err[idx + 7..];
        if let Some(end) = rest.find('\'') {
            let name = &rest[..end];
            if source_line.contains(name) { return name.len(); }
        }
    }
    1
}

/// Extract help text from an error that already contains "Fix:" or "Hint:".
fn extract_help(err: &str) -> Option<String> {
    if let Some(idx) = err.find("Fix: ") {
        return Some(err[idx + 5..].trim().to_string());
    }
    if let Some(idx) = err.find("Hint: ") {
        return Some(err[idx + 6..].trim().to_string());
    }
    None
}

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

pub fn build_program(source: &str) -> Result<Program, Vec<String>> {
    build_program_with_path(source, None)
}

/// Build a program with optional file path for resolving `use` imports.
/// When source_path is provided, `use foo::bar;` will look for `foo.fd` or
/// `foo/mod.fd` relative to the source file's directory.
pub fn build_program_with_path(source: &str, source_path: Option<&str>) -> Result<Program, Vec<String>> {
    // Auto-detect color from TTY (can be overridden by main.rs before calling)
    use std::io::IsTerminal;
    crate::diag::set_color(std::io::stderr().is_terminal());

    // Parse stdlib first
    let stdlib = parse_source(STDLIB_CORE)?;

    // Extract stdlib struct/enum names so the user parser recognizes them
    let mut struct_names: Vec<String> = stdlib.structs.iter().map(|s| s.name.clone()).collect();
    let mut enum_names: Vec<String> = stdlib.enums.iter().map(|e| e.name.clone()).collect();

    // Parse user code with stdlib names pre-populated
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    // Check for lexer errors before parsing
    if !lexer.errors.is_empty() {
        let file = source_path.unwrap_or("<stdin>");
        return Err(enrich_errors(&lexer.errors, source, file));
    }
    let mut parser = Parser::with_names(tokens, struct_names.clone(), enum_names.clone());
    let user = parser.parse().map_err(|e| {
        let file = source_path.unwrap_or("<stdin>");
        enrich_errors(&[e], source, file)
    })?;

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

    // Phase 7.7: Conditional compilation — filter out items with @cfg annotations
    // that don't match the current platform/features.
    apply_cfg_filter(&mut program);

    // Phase 7.7: Process @derive annotations — generate Debug/Clone impl methods
    process_derive(&mut program);

    // Desugar layer/model declarations into structs + impls + functions
    desugar_layers_models(&mut program);

    // Phase 8B: Desugar ECS/StateMachine/EventHandler declarations
    desugar_phase8b(&mut program);

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
        let file = source_path.unwrap_or("<stdin>");
        return Err(enrich_errors(&tc.errors, source, file));
    }

    // Run lint checks (warnings only, never errors)
    tc.run_lints(&program);

    // Borrow check — runs after typecheck succeeds, before comptime/mono.
    // Mode is controlled by the global BORROWCK_MODE (set via --borrowck CLI flag).
    // Off by default; Warn emits warnings; On fails compilation on violations.
    let borrowck_mode = crate::borrowck::get_mode();
    if borrowck_mode != crate::borrowck::BorrowckMode::Off {
        let mut bc = crate::borrowck::BorrowChecker::new(&tc, borrowck_mode);
        bc.check(&program);
        match borrowck_mode {
            crate::borrowck::BorrowckMode::Warn => {
                for w in &bc.warnings {
                    eprintln!("borrow warning: {}", w);
                }
            }
            crate::borrowck::BorrowckMode::On => {
                if !bc.errors.is_empty() {
                    let file = source_path.unwrap_or("<stdin>");
                    return Err(enrich_errors(&bc.errors, source, file));
                }
            }
            crate::borrowck::BorrowckMode::Off => {}
        }
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
            annotations: Vec::new(),
            type_params: Vec::new(),
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
            preconditions: Vec::new(),
            postconditions: Vec::new(),
            is_async: false,
        });
    }

    // Desugar models
    for model in program.models.drain(..) {
        // Create struct with layer fields
        let fields: Vec<Param> = model.layers.iter().map(|(name, layer_type, _)| {
            Param {
                name: name.clone(),
                ty: Type::Named(layer_type.clone()),
                is_pub: false,
                annotations: Vec::new(),
            }
        }).collect();

        program.structs.push(StructDef {
            name: model.name.clone(),
            fields,
            is_pub: model.is_pub,
            annotations: Vec::new(),
            type_params: Vec::new(),
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
            preconditions: Vec::new(),
            postconditions: Vec::new(),
            is_async: false,
        });
    }
}

/// Phase 8B: Desugar ECS components, systems, state machines, and event handlers
/// into regular Fides constructs (structs, functions, enums, match statements).
fn desugar_phase8b(program: &mut Program) {
    use crate::ast::{Func, Param, StructDef, EnumDef, EnumVariant, Type, Expr, Stmt, MatchArm, Pattern};

    let make_func = |name: String, params: Vec<Param>, ret_ty: Type, body: Vec<Stmt>, is_pub: bool| Func {
        name,
        type_params: Vec::new(),
        type_param_bounds: Vec::new(),
        const_params: Vec::new(),
        params,
        ret_ty,
        body,
        is_pub,
        is_comptime: false,
        declared_effects: Vec::new(),
        has_effect_annotation: false,
        is_interrupt: false,
        annotations: Vec::new(),
        preconditions: Vec::new(),
        postconditions: Vec::new(),
        is_async: false,
    };

    // ── F18: Desugar components → structs ──
    for comp in program.components.drain(..) {
        program.structs.push(StructDef {
            name: comp.name.clone(),
            fields: comp.fields.clone(),
            is_pub: comp.is_pub,
            annotations: Vec::new(),
            type_params: Vec::new(),
        });
    }

    // ── F20: Desugar event handlers → functions ──
    for eh in program.event_handlers.drain(..) {
        let func_name = format!("__on_{}", eh.event_name);
        let mut params = Vec::new();
        params.extend(eh.params.clone());
        program.funcs.push(make_func(
            func_name,
            params,
            Type::Void,
            eh.body.clone(),
            eh.is_pub,
        ));
    }

    // ── F19: Desugar state machines → enum + initial/handle functions ──
    for sm in program.state_machines.drain(..) {
        let sm_name = sm.name.clone();

        // Create enum with one variant per state (no data)
        let variants: Vec<EnumVariant> = sm.states.iter().map(|s| EnumVariant {
            name: s.name.clone(),
            data: None,
        }).collect();
        program.enums.push(EnumDef {
            name: sm_name.clone(),
            variants,
            type_params: Vec::new(),
            is_pub: sm.is_pub,
        });

        // Create <SM>_initial() function — returns first state
        let first_state = sm.states.first().map(|s| s.name.clone()).unwrap_or("Error".to_string());
        let initial_name = format!("{}_initial", sm_name);
        program.funcs.push(make_func(
            initial_name,
            vec![],
            Type::Named(sm_name.clone()),
            vec![Stmt::Return(Some(Expr::EnumLit {
                enum_name: sm_name.clone(),
                variant: first_state.clone(),
                args: Vec::new(),
            }), 0)],
            sm.is_pub,
        ));

        // Create <SM>_handle(state, event) -> state function
        let handle_name = format!("{}_handle", sm_name);
        // Build match arms: one per state, each matching event → target
        let mut arms: Vec<MatchArm> = Vec::new();
        for state in &sm.states {
            // Pattern: StateName
            let pattern = Pattern::EnumVariant {
                enum_name: sm_name.clone(),
                variant: state.name.clone(),
                bindings: Vec::new(),
            };
            // Body: match on event string
            let mut event_arms: Vec<MatchArm> = Vec::new();
            for trans in &state.transitions {
                event_arms.push(MatchArm {
                    pattern: Pattern::StrLit(trans.event.clone()),
                    guard: None,
                    body: vec![Stmt::Return(Some(Expr::EnumLit {
                        enum_name: sm_name.clone(),
                        variant: trans.target.clone(),
                        args: Vec::new(),
                    }), 0)],
                });
            }
            // Default arm: return current state (no transition)
            event_arms.push(MatchArm {
                pattern: Pattern::Wildcard,
                guard: None,
                body: vec![Stmt::Return(Some(Expr::Ident("__state".to_string())), 0)],
            });
            let body = vec![Stmt::Match {
                expr: Expr::Ident("__event_type".to_string()),
                arms: event_arms,
                line: 0,
            }];
            arms.push(MatchArm {
                pattern,
                guard: None,
                body,
            });
        }
        // Default arm for unknown state
        arms.push(MatchArm {
            pattern: Pattern::Wildcard,
            guard: None,
            body: vec![Stmt::Return(Some(Expr::Ident("__state".to_string())), 0)],
        });

        program.funcs.push(make_func(
            handle_name,
            vec![
                Param {
                    name: "__state".to_string(),
                    ty: Type::Named(sm_name.clone()),
                    is_pub: false,
                annotations: Vec::new(),
                },
                Param {
                    name: "__event_type".to_string(),
                    ty: Type::Str,
                    is_pub: false,
                annotations: Vec::new(),
                },
            ],
            Type::Named(sm_name.clone()),
            vec![Stmt::Match {
                expr: Expr::Ident("__state".to_string()),
                arms,
                line: 0,
            }],
            sm.is_pub,
        ));
    }

    // ── F18: Desugar systems → functions (query statements stay as Stmt::Query) ──
    for sys in program.systems.drain(..) {
        let mut params = Vec::new();
        params.extend(sys.params.clone());
        program.funcs.push(make_func(
            sys.name.clone(),
            params,
            Type::Void,
            sys.body.clone(),
            sys.is_pub,
        ));
    }

    // ── F18: Desugar query statements → loops over entities ──
    desugar_query_stmts_in_program(program);
}

/// Recursively desugar all `Stmt::Query` nodes in a program's function bodies.
fn desugar_query_stmts_in_program(program: &mut Program) {
    use crate::ast::{Stmt, Expr};
    for func in &mut program.funcs {
        desugar_query_stmts(&mut func.body);
    }
    for imp in &mut program.impls {
        for m in &mut imp.methods {
            desugar_query_stmts(&mut m.body);
        }
    }
}

fn desugar_query_stmts(stmts: &mut Vec<crate::ast::Stmt>) {
    use crate::ast::{Stmt, Expr};
    for stmt in stmts.iter_mut() {
        match stmt {
            Stmt::Query { components, body, .. } => {
                desugar_query_stmts(body);
                // Build: for entity in 0..__ecs_entity_count() { if has_all { body } }
                let entity_var = "entity".to_string();
                let mut cond: Option<Expr> = None;
                for comp in components {
                    let check = Expr::Binary {
                        op: crate::ast::BinOp::Neq,
                        left: Box::new(Expr::Call {
                            callee: "__ecs_has".to_string(),
                            args: vec![
                                Expr::StringLit(comp.clone()),
                                Expr::Ident(entity_var.clone()),
                            ],
                        }),
                        right: Box::new(Expr::IntLit(0)),
                    };
                    cond = Some(match cond {
                        None => check,
                        Some(prev) => Expr::Binary {
                            op: crate::ast::BinOp::And,
                            left: Box::new(prev),
                            right: Box::new(check),
                        },
                    });
                }
                let if_stmt = Stmt::If {
                    cond: cond.unwrap_or(Expr::BoolLit(true)),
                    then_block: body.clone(),
                    else_block: None,
                    line: 0,
                };
                let for_stmt = Stmt::For {
                    var: entity_var,
                    iterator: Expr::Range {
                        start: Box::new(Expr::IntLit(0)),
                        end: Box::new(Expr::Call {
                            callee: "__ecs_entity_count".to_string(),
                            args: Vec::new(),
                        }),
                        inclusive: false,
                    },
                    body: vec![if_stmt],
                    line: 0,
                };
                *stmt = for_stmt;
            }
            Stmt::If { then_block, else_block, .. } => {
                desugar_query_stmts(then_block);
                if let Some(eb) = else_block {
                    desugar_query_stmts(eb);
                }
            }
            Stmt::While { body, .. } => desugar_query_stmts(body),
            Stmt::For { body, .. } => desugar_query_stmts(body),
            Stmt::Loop { body, .. } => desugar_query_stmts(body),
            Stmt::Match { arms, .. } => {
                for arm in arms {
                    desugar_query_stmts(&mut arm.body);
                }
            }
            Stmt::Defer(body, _) => desugar_query_stmts(body),
            Stmt::Handle { body, handlers, .. } => {
                desugar_query_stmts(body);
                for h in handlers {
                    desugar_query_stmts(&mut h.body);
                }
            }
            Stmt::RegionBlock { body, .. } => desugar_query_stmts(body),
            Stmt::Sandbox { body, .. } => desugar_query_stmts(body),
            Stmt::Unsafe { body, .. } => desugar_query_stmts(body),
            Stmt::Txn { body, .. } => desugar_query_stmts(body),
            Stmt::ComptimeGen { block, .. } => desugar_query_stmts(block),
            Stmt::GradBlock { body, .. } => desugar_query_stmts(body),
            _ => {}
        }
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
                    Param { name: p.name.clone(), ty: self_type.clone(), is_pub: false, annotations: Vec::new() }
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
            preconditions: Vec::new(),
            postconditions: Vec::new(),
            is_async: false,
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

    // Merge extern_decls (user wins on conflicts by pledge_name)
    let user_extern_names: std::collections::HashSet<String> =
        merged.extern_decls.iter().map(|e| e.pledge_name.clone()).collect();
    for e in stdlib.extern_decls {
        if !user_extern_names.contains(&e.pledge_name) {
            merged.extern_decls.push(e);
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

/// Get diagnostics (errors) for a source file — used by LSP.
/// Returns a list of (line, column, message) tuples.
pub fn get_diagnostics(source: &str, source_path: Option<&str>) -> Vec<(usize, usize, String)> {
    match build_program_with_path(source, source_path) {
        Ok(_) => Vec::new(),
        Err(errors) => {
            errors.iter().map(|e| {
                // Parse error format: "Error:line:col: message" or "Type error (line X): message"
                // Try to extract line and column from the error string
                let (line, col, msg) = parse_error_position(e);
                (line, col, msg)
            }).collect()
        }
    }
}

/// Build a program and typechecker for LSP use (completion, hover, goto-definition).
/// Unlike `build_program_with_path`, this does NOT fail on type errors — it
/// registers all symbols that were successfully parsed so the LSP can provide
/// completions and hover info even for partially-correct code.
/// Returns (Program, TypeChecker) with symbol tables populated.
pub fn build_program_for_lsp(source: &str) -> (Program, TypeChecker) {
    // Parse stdlib first
    let stdlib = match parse_source(STDLIB_CORE) {
        Ok(p) => p,
        Err(_) => Program::default(),
    };
    let stdlib_module = stdlib.module.clone().unwrap_or_else(|| "core".to_string());

    // Extract stdlib struct/enum names so the user parser recognizes them
    let struct_names: Vec<String> = stdlib.structs.iter().map(|s| s.name.clone()).collect();
    let enum_names: Vec<String> = stdlib.enums.iter().map(|e| e.name.clone()).collect();

    // Parse user code (tolerate errors — we still want partial symbols)
    let mut lexer = Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = Parser::with_names(tokens, struct_names, enum_names);
    let user = parser.parse().unwrap_or_default();
    let user_module = user.module.clone().unwrap_or_else(|| "main".to_string());

    // Merge programs (simple merge — no desugaring/comptime for LSP)
    let mut program = Program {
        module: Some(user_module.clone()),
        ..Default::default()
    };

    let mut tc = TypeChecker::new();
    tc.register_program(&stdlib, &stdlib_module);
    tc.register_program(&user, &user_module);

    // Copy user items into the merged program for the LSP to traverse
    for f in &user.funcs { program.funcs.push(f.clone()); }
    for s in &user.structs { program.structs.push(s.clone()); }
    for e in &user.enums { program.enums.push(e.clone()); }
    for c in &user.consts { program.consts.push(c.clone()); }
    for ta in &user.type_aliases { program.type_aliases.push(ta.clone()); }
    for i in &user.impls { program.impls.push(i.clone()); }
    for ti in &user.trait_impls { program.trait_impls.push(ti.clone()); }
    program.imports = user.imports.clone();
    program.traits = user.traits.clone();

    (program, tc)
}

/// Parse an error string to extract (line, column, message).
fn parse_error_position(error: &str) -> (usize, usize, String) {
    // Format 1: "Error:line:col: message"
    if let Some(rest) = error.strip_prefix("Error:") {
        let parts: Vec<&str> = rest.splitn(3, ':').collect();
        if parts.len() >= 3 {
            let line = parts[0].parse::<usize>().unwrap_or(1);
            let col = parts[1].parse::<usize>().unwrap_or(1);
            return (line, col, parts[2].trim().to_string());
        }
    }
    // Format 2: "Type error (line X): message" or "Type error:line:col: message"
    if error.starts_with("Type error") {
        if let Some(start) = error.find("(line ") {
            let rest = &error[start + 6..];
            if let Some(end) = rest.find(')') {
                let line = rest[..end].parse::<usize>().unwrap_or(1);
                return (line, 1, error.to_string());
            }
        }
        if let Some(rest) = error.strip_prefix("Type error:") {
            let parts: Vec<&str> = rest.splitn(3, ':').collect();
            if parts.len() >= 3 {
                let line = parts[0].trim().parse::<usize>().unwrap_or(1);
                let col = parts[1].trim().parse::<usize>().unwrap_or(1);
                return (line, col, parts[2].trim().to_string());
            }
        }
    }
    // Format 3: "Parse error at line:col: message"
    if error.starts_with("Parse error at ") {
        let rest = &error[14..];
        let parts: Vec<&str> = rest.splitn(3, ':').collect();
        if parts.len() >= 3 {
            let line = parts[0].parse::<usize>().unwrap_or(1);
            let col = parts[1].parse::<usize>().unwrap_or(1);
            return (line, col, parts[2].trim().to_string());
        }
    }
    (1, 1, error.to_string())
}

pub fn interpret_source(source: &str, verbose: bool) -> Result<(RunResult, Option<String>), Vec<String>> {
    let program = build_program(source)?;

    // Lower to IR
    let module = lower::lower(&program);

    // Interpret
    Ok(interp::interpret(module, verbose, false))
}

pub fn interpret_source_with_path(source: &str, source_path: &str, verbose: bool) -> Result<(RunResult, Option<String>), Vec<String>> {
    interpret_source_with_path_release(source, source_path, verbose, false)
}

/// Run lint checks on source code and return warnings.
/// Returns Ok(warnings) if compilation succeeds, Err(errors) if it fails.
pub fn lint_source(source: &str, source_path: &str) -> Result<Vec<String>, Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;
    // build_program_with_path already runs lints and stores warnings in tc,
    // but we don't have access to tc here. So we re-run lints on the program.
    // We need to reconstruct a typechecker and run just the lint portion.
    // Actually, build_program_with_path runs run_lints internally, but
    // doesn't expose the warnings. Let's use get_diagnostics instead.
    
    // Re-run with a fresh typechecker to get warnings
    let (program, _tc) = build_program_with_lints(source, Some(source_path))?;
    Ok(_tc.warnings)
}

/// Build program and return both the program and the typechecker (with warnings).
fn build_program_with_lints(source: &str, source_path: Option<&str>) -> Result<(Program, TypeChecker), Vec<String>> {
    // Parse user code
    let mut lex = Lexer::new(source);
    let toks = lex.tokenize();
    let lexer_errors = lex.errors.clone();
    if !lexer_errors.is_empty() {
        return Err(lexer_errors);
    }
    let mut p = Parser::new(toks);
    let user = p.parse().map_err(|e| vec![e])?;

    // Load imports
    let mut imported_modules: Vec<(String, Program)> = Vec::new();
    if let Some(src_path) = source_path {
        let base_dir = std::path::Path::new(src_path)
            .parent()
            .unwrap_or(std::path::Path::new("."));

        for import in &user.imports {
            if import.path == "core" { continue; }
            let path_str = import.path.replace("::", "/");
            let candidates = [
                base_dir.join(format!("{}.fd", path_str)),
                base_dir.join(format!("{}/mod.fd", path_str)),
                base_dir.join("fides_modules").join(format!("{}.fd", path_str)),
                base_dir.join("fides_modules").join(format!("{}/mod.fd", path_str)),
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
                let mut lex2 = Lexer::new(&module_source);
                let toks2 = lex2.tokenize();
                let mut p2 = Parser::new(toks2);
                let imported = p2.parse().map_err(|e| vec![e])?;
                let module_name = imported.module.clone()
                    .unwrap_or_else(|| import.path.clone());
                imported_modules.push((module_name, imported));
            }
        }
    }

    // Load core stdlib
    let core_source = include_str!("../stdlib/core.fd");
    let mut lex_core = Lexer::new(core_source);
    let toks_core = lex_core.tokenize();
    let mut p_core = Parser::new(toks_core);
    let core_program = p_core.parse().map_err(|e| vec![e])?;

    // Merge programs
    let user_module = user.module.clone().unwrap_or_else(|| "main".to_string());
    let mut program = Program {
        module: Some(user_module.clone()),
        ..Default::default()
    };

    // Register core first
    let mut tc = TypeChecker::new();
    tc.register_program(&core_program, "core");

    // Register imported modules
    for (mod_name, mod_prog) in &imported_modules {
        tc.register_program(mod_prog, mod_name);
        for f in &mod_prog.funcs { program.funcs.push(f.clone()); }
        for s in &mod_prog.structs { program.structs.push(s.clone()); }
        for e in &mod_prog.enums { program.enums.push(e.clone()); }
        for c in &mod_prog.consts { program.consts.push(c.clone()); }
        for ta in &mod_prog.type_aliases { program.type_aliases.push(ta.clone()); }
        for i in &mod_prog.impls { program.impls.push(i.clone()); }
        for ti in &mod_prog.trait_impls { program.trait_impls.push(ti.clone()); }
    }

    // Register user program
    tc.register_program(&user, &user_module);
    for f in &user.funcs { program.funcs.push(f.clone()); }
    for s in &user.structs { program.structs.push(s.clone()); }
    for e in &user.enums { program.enums.push(e.clone()); }
    for c in &user.consts { program.consts.push(c.clone()); }
    for ta in &user.type_aliases { program.type_aliases.push(ta.clone()); }
    for i in &user.impls { program.impls.push(i.clone()); }
    for ti in &user.trait_impls { program.trait_impls.push(ti.clone()); }
    program.imports = user.imports.clone();
    program.extern_decls = user.extern_decls.clone();

    // Type check
    tc.current_module = user_module.clone();
    tc.collect_capabilities(&user);
    tc.check_bodies(&program);
    if !tc.errors.is_empty() {
        let file = source_path.unwrap_or("<stdin>");
        return Err(enrich_errors(&tc.errors, source, file));
    }

    // Run lints
    tc.run_lints(&program);

    Ok((program, tc))
}

pub fn interpret_source_with_path_release(source: &str, source_path: &str, verbose: bool, release: bool) -> Result<(RunResult, Option<String>), Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;

    // Lower to IR
    let module = lower::lower(&program);

    // Interpret
    Ok(interp::interpret(module, verbose, release))
}

/// Interpret with trace recording enabled (for post-mortem debugging).
/// Returns the run result, stack trace string, and the execution trace.
pub fn interpret_source_with_trace(source: &str, source_path: &str, verbose: bool, release: bool) -> Result<(RunResult, Option<String>, Vec<interp::TraceEntry>), Vec<String>> {
    let program = build_program_with_path(source, Some(source_path))?;
    let module = lower::lower(&program);
    let mut interp = interp::Interpreter::new(module);
    if verbose {
        // verbose() takes self by value, so we need to call it before other methods
        interp = interp.verbose();
    }
    interp.enable_recording();
    let result = interp.run();
    let trace = interp.get_trace().to_vec();
    let stack_trace = interp.format_panic_trace();
    Ok((result, stack_trace, trace))
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
    build_executable_with_lto(source, source_path, target, do_opt, false, granted_caps)
}

/// Build a native executable with optional LTO (Link-Time Optimization).
/// LTO mode uses more aggressive inline limits for cross-module inlining.
pub fn build_executable_with_lto(source: &str, source_path: &str, target: &str, do_opt: bool, do_lto: bool, granted_caps: Option<&[String]>) -> Result<Vec<u8>, Vec<String>> {
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

    if do_lto {
        let _ = crate::opt::optimize_l2_lto(&mut l2);
    } else if do_opt {
        let _ = crate::opt::optimize_l2(&mut l2);
    }

    let l3 = pir::lower_l2_to_l3(&l2, &module);

    // DEBUG: dump p_str_copy_n L3 PIR
    for f in &l3.functions {
        if f.name == "p_str_copy_n" {
            eprintln!("=== BUILD p_str_copy_n L3 ===\n{}", pir::format_l3(&l3));
        }
    }

    match target {
        "x86-64" | "x64" => {
            let asm = crate::codegen_x64::generate_asm(&l3);
            // DEBUG: dump p_str_copy_n assembly
            if let Some(idx) = asm.find("p_str_copy_n:") {
                let end = asm[idx..].find("\n\n").map(|e| idx + e).unwrap_or(idx + 800);
                eprintln!("=== BUILD p_str_copy_n asm ===\n{}", &asm[idx..end]);
            }
            // DEBUG: dump String_from assembly
            if let Some(idx) = asm.find("String_from:") {
                let end = asm[idx..].find("\n\n").map(|e| idx + e).unwrap_or(idx + 800);
                eprintln!("=== BUILD String_from asm ===\n{}", &asm[idx..end]);
            }
            // DEBUG: dump p_str_length assembly
            if let Some(idx) = asm.find("p_str_length:") {
                let end = asm[idx..].find("\n\n").map(|e| idx + e).unwrap_or(idx + 800);
                eprintln!("=== BUILD p_str_length asm ===\n{}", &asm[idx..end]);
            }
            // Always run peephole optimization on the generated assembly —
            // this catches redundant moves, zero idioms, dead stores, etc.
            // These are safe local transformations that don't affect semantics.
            let asm = crate::peephole::optimize(&asm);
            let obj = crate::elf64::emit_object_file(&l3, &asm)
                .map_err(|e| vec![e])?;
            // Link with the pre-compiled Linux runtime (raw machine code)
            // F-1B: Use capability-aware runtime when caps are granted.
            let (rt_text, rt_syms, rt_rodata, rt_rodata_syms) = if let Some(caps) = granted_caps {
                let cap_set: std::collections::HashSet<String> = caps.iter().cloned().collect();
                crate::runtime_linux::runtime_parts_with_caps(&cap_set)
            } else {
                crate::runtime_linux::runtime_parts()
            };
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
            // Assemble the ARM64 code using our built-in encoder
            let (user_text, user_syms, user_relocs) = crate::arm64_enc::assemble(&asm)
                .map_err(|e| vec![format!("ARM64 assembly error: {}", e)])?;
            // Get the ARM64 runtime
            let (rt_text, rt_syms, rt_rodata, rt_rodata_syms) = if let Some(caps) = granted_caps {
                let cap_set: std::collections::HashSet<String> = caps.iter().cloned().collect();
                crate::runtime_arm64::runtime_parts_with_caps(&cap_set)
            } else {
                crate::runtime_arm64::runtime_parts()
            };
            let rt_relocs = crate::runtime_arm64::runtime_relocations();
            // Link user code + runtime
            let user_syms_owned: Vec<(String, u64)> = user_syms.iter()
                .map(|(n, o)| (n.clone(), *o))
                .collect();
            crate::linker::link_raw_with_runtime_for_target(
                &user_text, &user_syms_owned, &user_relocs,
                &[], &[],  // user rodata (empty for now)
                "_start",
                &rt_text, &rt_syms,
                &rt_rodata, &rt_rodata_syms,
                &rt_relocs,
                "arm64",
            )
                .map_err(|e| vec![e])
        }
        "riscv" | "riscv64" => {
            let asm = crate::codegen_riscv::generate_asm(&l3);
            // Assemble the RISC-V code using our built-in encoder
            let (user_text, user_syms, user_relocs) = crate::riscv_enc::assemble(&asm)
                .map_err(|e| vec![format!("RISC-V assembly error: {}", e)])?;
            // Get the RISC-V runtime
            let (rt_text, rt_syms, rt_rodata, rt_rodata_syms) = if let Some(caps) = granted_caps {
                let cap_set: std::collections::HashSet<String> = caps.iter().cloned().collect();
                crate::runtime_riscv::runtime_parts_with_caps(&cap_set)
            } else {
                crate::runtime_riscv::runtime_parts()
            };
            let rt_relocs = crate::runtime_riscv::runtime_relocations();
            let user_syms_owned: Vec<(String, u64)> = user_syms.iter()
                .map(|(n, o)| (n.clone(), *o))
                .collect();
            crate::linker::link_raw_with_runtime_for_target(
                &user_text, &user_syms_owned, &user_relocs,
                &[], &[],
                "_start",
                &rt_text, &rt_syms,
                &rt_rodata, &rt_rodata_syms,
                &rt_relocs,
                "riscv",
            )
                .map_err(|e| vec![e])
        }
        "x86-64-windows" | "x64-windows" | "windows" => {
            // Windows PE cross-compilation — use the direct PE emitter
            // which generates x86-64 Windows .exe files with kernel32.dll imports
            Ok(crate::pe::generate_pe(&module))
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
/// Generate a default expression for a given type (used by @derive(Deserialize)).
fn default_expr_for_type(ty: &crate::ast::Type) -> crate::ast::Expr {
    use crate::ast::{Type, Expr};
    match ty {
        Type::I64 | Type::I32 | Type::I16 | Type::I8 => Expr::IntLit(0),
        Type::U64 | Type::U32 | Type::U16 | Type::U8 => Expr::IntLit(0),
        Type::F64 | Type::F32 => Expr::FloatLit(0.0),
        Type::Bool => Expr::BoolLit(false),
        Type::Str => Expr::StringLit("".to_string()),
        Type::Named(name) => Expr::StructLit {
            name: name.clone(),
            fields: Vec::new(),
            spread: None,
        },
        _ => Expr::IntLit(0),
    }
}

/// Process @derive annotations — generate methods based on the derive list.
/// Supported derives:
///   @derive(Debug)  — generates a debug_str() method that returns a string representation
///   @derive(Clone)  — generates a clone() method that copies the struct
fn process_derive(program: &mut Program) {
    use crate::ast::{Func, Param, Type, Expr, Stmt};

    let mut generated: Vec<Func> = Vec::new();

    for s in &program.structs {
        for ann in &s.annotations {
            if ann.name == "derive" {
                for arg in &ann.args {
                    let arg = arg.trim();
                    if arg == "Debug" {
                        let func_name = format!("{}_debug_str", s.name);
                        let self_type = Type::Named(s.name.clone());
                        let field_list = s.fields.iter()
                            .map(|f| format!("{}: ?", f.name))
                            .collect::<Vec<_>>()
                            .join(", ");
                        let debug_str = format!("{}({})", s.name, field_list);
                        let func = Func {
                            name: func_name,
                            type_params: Vec::new(),
                            type_param_bounds: Vec::new(),
                            const_params: Vec::new(),
                            params: vec![Param { name: "self".to_string(), ty: self_type, is_pub: false, annotations: Vec::new() }],
                            ret_ty: Type::Str,
                            body: vec![Stmt::Return(Some(Expr::StringLit(debug_str)), 0)],
                            is_pub: true,
                            is_comptime: false,
                            declared_effects: Vec::new(),
                            has_effect_annotation: false,
                            is_interrupt: false,
                            annotations: Vec::new(),
                            preconditions: Vec::new(),
                            postconditions: Vec::new(),
                            is_async: false,
                        };
                        generated.push(func);
                    } else if arg == "Clone" {
                        let func_name = format!("{}_clone", s.name);
                        let self_type = Type::Named(s.name.clone());
                        let func = Func {
                            name: func_name,
                            type_params: Vec::new(),
                            type_param_bounds: Vec::new(),
                            const_params: Vec::new(),
                            params: vec![Param { name: "self".to_string(), ty: self_type.clone(), is_pub: false, annotations: Vec::new() }],
                            ret_ty: self_type,
                            body: vec![Stmt::Return(Some(Expr::Ident("self".to_string())), 0)],
                            is_pub: true,
                            is_comptime: false,
                            declared_effects: Vec::new(),
                            has_effect_annotation: false,
                            is_interrupt: false,
                            annotations: Vec::new(),
                            preconditions: Vec::new(),
                            postconditions: Vec::new(),
                            is_async: false,
                        };
                        generated.push(func);
                    } else if arg == "Serialize" {
                        // Generate: pf to_json(self: StructName) -> str
                        // Returns a JSON string representation of the struct.
                        let func_name = format!("{}_to_json", s.name);
                        let self_type = Type::Named(s.name.clone());
                        let fields_json = s.fields.iter()
                            .map(|f| format!("\"{}\":0", f.name))
                            .collect::<Vec<_>>()
                            .join(",");
                        let json_str = format!("{{{}}}", fields_json);
                        let func = Func {
                            name: func_name,
                            type_params: Vec::new(),
                            type_param_bounds: Vec::new(),
                            const_params: Vec::new(),
                            params: vec![Param { name: "self".to_string(), ty: self_type, is_pub: false, annotations: Vec::new() }],
                            ret_ty: Type::Str,
                            body: vec![Stmt::Return(Some(Expr::StringLit(json_str)), 0)],
                            is_pub: true,
                            is_comptime: false,
                            declared_effects: Vec::new(),
                            has_effect_annotation: false,
                            is_interrupt: false,
                            annotations: Vec::new(),
                            preconditions: Vec::new(),
                            postconditions: Vec::new(),
                            is_async: false,
                        };
                        generated.push(func);
                    } else if arg == "Deserialize" {
                        // Generate: pf from_json(json: str) -> StructName
                        // Creates a struct from a JSON string (default-constructed for now).
                        let func_name = format!("{}_from_json", s.name);
                        let self_type = Type::Named(s.name.clone());
                        let func = Func {
                            name: func_name,
                            type_params: Vec::new(),
                            type_param_bounds: Vec::new(),
                            const_params: Vec::new(),
                            params: vec![Param { name: "json".to_string(), ty: Type::Str, is_pub: false, annotations: Vec::new() }],
                            ret_ty: self_type.clone(),
                            body: vec![Stmt::Return(Some(Expr::StructLit {
                                name: s.name.clone(),
                                fields: s.fields.iter().map(|f| {
                                    (f.name.clone(), default_expr_for_type(&f.ty))
                                }).collect(),
                                spread: None,
                            }), 0)],
                            is_pub: true,
                            is_comptime: false,
                            declared_effects: Vec::new(),
                            has_effect_annotation: false,
                            is_interrupt: false,
                            annotations: Vec::new(),
                            preconditions: Vec::new(),
                            postconditions: Vec::new(),
                            is_async: false,
                        };
                        generated.push(func);
                    }
                }
            }
        }
    }

    program.funcs.extend(generated);
}

/// Apply conditional compilation filtering — remove items with @cfg annotations
/// that don't match the current platform/features.
/// Supported @cfg args:
///   @cfg(platform = "linux")   — only include on Linux
///   @cfg(platform = "windows") — only include on Windows
///   @cfg(platform = "macos")   — only include on macOS
///   @cfg(feature = "name")     — only include if feature "name" is enabled
///   @cfg(debug)                — only include in debug builds
///   @cfg(release)              — only include in release builds
fn apply_cfg_filter(program: &mut Program) {
    let is_linux = cfg!(target_os = "linux");
    let is_windows = cfg!(target_os = "windows");
    let is_macos = cfg!(target_os = "macos");
    let is_debug = cfg!(debug_assertions);

    let check_cfg = |annotations: &[crate::ast::Annotation]| -> bool {
        for ann in annotations {
            if ann.name == "cfg" {
                for arg in &ann.args {
                    let arg = arg.trim();
                    if arg.starts_with("platform") {
                        if let Some(eq_pos) = arg.find('=') {
                            let val = arg[eq_pos+1..].trim().trim_matches('"');
                            let include = match val {
                                "linux" => is_linux,
                                "windows" => is_windows,
                                "macos" => is_macos,
                                _ => true,
                            };
                            if !include { return false; }
                        }
                    } else if arg.starts_with("feature") {
                        // Features are not yet supported via CLI — always include
                        // (feature flags will be added in a future phase)
                    } else if arg == "debug" {
                        if !is_debug { return false; }
                    } else if arg == "release" {
                        if is_debug { return false; }
                    } else if arg == "linux" {
                        if !is_linux { return false; }
                    } else if arg == "windows" {
                        if !is_windows { return false; }
                    } else if arg == "macos" {
                        if !is_macos { return false; }
                    }
                }
            }
        }
        true
    };

    program.funcs.retain(|f| check_cfg(&f.annotations));
    program.structs.retain(|s| check_cfg(&s.annotations));
}

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

/// I-3A: Transitive Capability DCE — strip functions requiring capabilities not
/// in the granted set, then transitively eliminate all functions only reachable
/// through the stripped functions.
///
/// Improvements over the original F5 implementation:
///   1. Starts reachability BFS from ALL entry points ("main" and "_start"),
///      not just "main" — prevents stripping _start or its unique callees.
///   2. Tracks function references through `ConstStr` (used by `spawn` which
///      passes the callee name as a string to `p_thread_spawn`).
///   3. Conservatively keeps functions whose address is taken (indirect calls
///      via `CallPtr` or `DynCall` can't be statically resolved).
///   4. Tracks transitive closure: if `foo()` is stripped, everything only
///      callable through `foo()` is also stripped (BFS naturally handles this).
fn strip_ungranted_caps(module: &mut IrModule, granted_caps: &[String]) {
    use std::collections::{HashMap, HashSet};

    // Map capability names to the effects they grant
    let granted_effects: HashSet<Effect> = granted_caps.iter().filter_map(|cap| {
        match cap.as_str() {
            "IO" | "FS" => Some(Effect::IO),
            "Net" => Some(Effect::Net),
            "Spawn" => Some(Effect::Spawn),
            _ => None,
        }
    }).collect();

    // Find functions that require effects not in the granted set.
    // Entry points (main, _start) are never stripped.
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

    // Collect all known function names for ConstStr reference tracking.
    let known_functions: HashSet<String> = module.functions.keys().cloned().collect();

    // Build call graph, including indirect references via ConstStr.
    // Also track which functions have their address taken (for conservative
    // retention — indirect calls via CallPtr/DynCall can target them).
    let mut call_graph: HashMap<String, Vec<String>> = HashMap::new();
    let mut address_taken: HashSet<String> = HashSet::new();

    for (name, func) in &module.functions {
        let mut callees = Vec::new();
        for (_, inst) in &func.instructions {
            match inst {
                Inst::Call { func: callee, .. } => {
                    callees.push(callee.clone());
                }
                Inst::ConstStr { value } => {
                    // ConstStr containing a function name = address taken
                    // (used by spawn: p_thread_spawn(func_name, ...))
                    if known_functions.contains(value) {
                        address_taken.insert(value.clone());
                        callees.push(value.clone());
                    }
                }
                Inst::CallPtr { .. } | Inst::DynCall { .. } => {
                    // Indirect call — can't determine target statically.
                    // We conservatively keep all address-taken functions.
                    // (No specific edge added here; address_taken set handles retention.)
                }
                _ => {}
            }
        }
        call_graph.insert(name.clone(), callees);
    }

    // BFS from all entry points to find reachable functions.
    // Skip functions in to_remove (ungranted capabilities).
    // Address-taken functions are kept even if not directly reachable
    // (they may be called indirectly via CallPtr/DynCall), UNLESS they
    // require ungranted capabilities (in to_remove).
    let mut reachable: HashSet<String> = HashSet::new();
    let mut queue: Vec<String> = Vec::new();

    // Start from all entry points.
    for entry in &["main", "_start"] {
        if module.functions.contains_key(*entry) && !to_remove.contains(*entry) {
            queue.push(entry.to_string());
        }
    }

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

    // Conservatively keep address-taken functions (indirect call targets),
    // unless they require ungranted capabilities.
    for fname in &address_taken {
        if !to_remove.contains(fname) {
            reachable.insert(fname.clone());
        }
    }

    // Remove unreachable functions (transitive closure of capability stripping).
    let removed: Vec<String> = module.functions.keys()
        .filter(|k| !reachable.contains(*k))
        .cloned()
        .collect();
    for name in &removed {
        module.functions.remove(name);
        module.function_effects.remove(name);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ir::{IrModule, IrFunction, BlockId, Effect, EffectSet, IrType, ValueMeta, Region};

    /// Build a minimal IrModule with the given functions and effects.
    fn make_module(
        funcs: &[(&str, &[&str], Option<&[Effect]>)],
    ) -> IrModule {
        let mut module = IrModule::new();
        for &(name, callees, effects) in funcs {
            let mut func = IrFunction::new(name.to_string(), IrType::I64);
            for callee in callees {
                let vid = func.new_value(ValueMeta {
                    ty: IrType::I64,
                    region: Region::Stack,
                    effects: EffectSet::new(),
                    source_line: None,
                    in_sandbox: false,
                    is_linear: false,
                    borrow_kind: crate::ir::BorrowKind::None,
                });
                func.instructions.insert(vid, Inst::Call {
                    func: callee.to_string(),
                    args: vec![],
                    ret_ty: IrType::I64,
                    effects: EffectSet::new(),
                });
            }
            module.functions.insert(name.to_string(), func);
            let eff = match effects {
                Some(e) => {
                    let mut es = EffectSet::new();
                    for &effect in e {
                        es.add(effect);
                    }
                    es
                }
                None => EffectSet::new(),
            };
            module.function_effects.insert(name.to_string(), eff);
        }
        module
    }

    #[test]
    fn strip_caps_removes_ungranted_net_function() {
        // main calls net_func (requires Net) and pure_func (no caps).
        // Granting only IO should strip net_func and keep pure_func.
        let mut module = make_module(&[
            ("main", &["net_func", "pure_func"], None),
            ("net_func", &[], Some(&[Effect::Net])),
            ("pure_func", &[], None),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string()]);

        assert!(module.functions.contains_key("main"), "main must survive");
        assert!(!module.functions.contains_key("net_func"), "net_func requires Net — should be stripped");
        assert!(module.functions.contains_key("pure_func"), "pure_func has no caps — should survive");
    }

    #[test]
    fn strip_caps_transitive_closure() {
        // main calls helper, helper calls net_func.
        // If net_func is stripped, helper becomes unreachable (only called by main
        // but main no longer calls it through a stripped path).
        // Actually: main calls helper directly, helper calls net_func.
        // helper itself doesn't require Net, but its callee net_func does.
        // After stripping net_func, helper is still reachable from main,
        // but net_func is gone. helper's call to net_func becomes a dead edge.
        let mut module = make_module(&[
            ("main", &["helper"], None),
            ("helper", &["net_func"], None),
            ("net_func", &[], Some(&[Effect::Net])),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string()]);

        assert!(module.functions.contains_key("main"));
        assert!(module.functions.contains_key("helper"), "helper doesn't require Net — should survive");
        assert!(!module.functions.contains_key("net_func"), "net_func should be stripped");
    }

    #[test]
    fn strip_caps_transitive_closure_eliminates_orphans() {
        // main calls foo, foo calls bar, bar calls net_func.
        // If net_func is stripped, bar is still reachable from main→foo→bar.
        // But if ONLY bar calls net_func, and bar itself doesn't require Net,
        // bar survives. The transitive closure removes net_func only.
        // Now test: orphan_func is only called by net_func.
        // After stripping net_func, orphan_func becomes unreachable.
        let mut module = make_module(&[
            ("main", &["net_func"], None),
            ("net_func", &["orphan_func"], Some(&[Effect::Net])),
            ("orphan_func", &[], None),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string()]);

        assert!(module.functions.contains_key("main"));
        assert!(!module.functions.contains_key("net_func"), "net_func should be stripped");
        assert!(!module.functions.contains_key("orphan_func"), "orphan_func only called by stripped net_func — transitive DCE");
    }

    #[test]
    fn strip_caps_keeps_all_when_granted() {
        // When all capabilities are granted, nothing should be stripped.
        let mut module = make_module(&[
            ("main", &["net_func", "io_func"], None),
            ("net_func", &[], Some(&[Effect::Net])),
            ("io_func", &[], Some(&[Effect::IO])),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string(), "Net".to_string()]);

        assert!(module.functions.contains_key("main"));
        assert!(module.functions.contains_key("net_func"));
        assert!(module.functions.contains_key("io_func"));
    }

    #[test]
    fn strip_caps_keeps_entry_points() {
        // _start should survive even if it's not called by main.
        let mut module = make_module(&[
            ("main", &[], None),
            ("_start", &["helper"], None),
            ("helper", &[], None),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string()]);

        assert!(module.functions.contains_key("main"), "main must survive");
        assert!(module.functions.contains_key("_start"), "_start must survive (entry point)");
        assert!(module.functions.contains_key("helper"), "helper called by _start — should survive");
    }

    #[test]
    fn strip_caps_noop_when_nothing_to_strip() {
        // No functions require ungranted caps — should be a no-op.
        let mut module = make_module(&[
            ("main", &["pure_func"], None),
            ("pure_func", &[], None),
        ]);

        strip_ungranted_caps(&mut module, &["IO".to_string()]);

        assert!(module.functions.contains_key("main"));
        assert!(module.functions.contains_key("pure_func"));
    }
}
