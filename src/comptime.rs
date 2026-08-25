// Fides comptime evaluation engine (Phase 3A)
//
// Evaluates `comptime <expr>`, `comptime { block }`, and `comptime reflect_op(T)`
// at compile time, substituting the result as a literal into the AST.
//
// Comptime functions (`comptime pf name(...) { ... }`) are evaluated by
// directly interpreting their AST bodies when called from a comptime context.

use crate::ast::*;
use std::collections::HashMap;

/// A comptime evaluation failure.
#[derive(Debug)]
pub struct ComptimeError(pub String);

/// Process `comptime gen` blocks in the program, generating new code.
/// This should be called BEFORE typechecking so generated declarations are visible.
/// Returns a new program with generated functions/structs injected.
pub fn process_gen(program: Program) -> Program {
    let mut errs: Vec<String> = Vec::new();
    let mut prog = program;

    // First, substitute comptime expressions inside comptime gen blocks
    // so that comptime function calls and reflection are resolved before
    // evaluating the gen blocks.
    let snapshot = prog.clone();
    for func in prog.funcs.iter_mut() {
        substitute_comptime_gen_blocks(func, &snapshot, &mut errs);
    }

    // Now process the gen blocks (comptime expressions inside should be resolved)
    let mut generated_funcs: Vec<Func> = Vec::new();
    let mut generated_structs: Vec<StructDef> = Vec::new();
    let func_count = prog.funcs.len();
    for idx in 0..func_count {
        let snapshot = prog.clone();
        let func = &mut prog.funcs[idx];
        process_comptime_gen_in_func(func, &snapshot, &mut generated_funcs, &mut generated_structs, &mut errs);
    }
    prog.funcs.extend(generated_funcs);
    prog.structs.extend(generated_structs);

    prog
}

/// Substitute comptime expressions inside comptime gen blocks.
fn substitute_comptime_gen_blocks(func: &mut Func, original: &Program, errs: &mut Vec<String>) {
    for stmt in func.body.iter_mut() {
        if let Stmt::ComptimeGen { block, .. } = stmt {
            substitute_stmts(block, original, errs);
        }
    }
}

/// Recursively substitute every comptime expression in `program` with an AST
/// literal carrying its compile-time value.
pub fn substitute(program: &Program) -> Result<Program, Vec<String>> {
    let mut errs: Vec<String> = Vec::new();
    let mut prog = program.clone();

    // Phase 1: Pre-substitute comptime expressions inside comptime function
    // bodies so that comptime function calls from comptime context see
    // already-substituted bodies. Iterate a bounded number of times.
    for _ in 0..8 {
        let snapshot = prog.clone();
        for func in prog.funcs.iter_mut() {
            if func.is_comptime {
                substitute_func(func, &snapshot, &mut errs);
            }
        }
    }

    // Phase 2: Substitute comptime expressions everywhere else.
    let snapshot2 = prog.clone();
    for func in prog.funcs.iter_mut() {
        substitute_func(func, &snapshot2, &mut errs);
    }
    for imp in prog.impls.iter_mut() {
        for method in imp.methods.iter_mut() {
            substitute_func(method, &snapshot2, &mut errs);
        }
    }
    for ti in prog.trait_impls.iter_mut() {
        for method in ti.methods.iter_mut() {
            substitute_func(method, &snapshot2, &mut errs);
        }
    }

    // I-5A: Aggressive comptime evaluation — automatically evaluate calls to
    // `comptime pf` functions when ALL arguments are literals, without requiring
    // an explicit `comptime` wrapper. This moves more work to compile time.
    // Iterate to fixed point (bounded) to handle chains of comptime calls.
    for _ in 0..4 {
        let snapshot3 = prog.clone();
        let mut errs3: Vec<String> = Vec::new();
        for func in prog.funcs.iter_mut() {
            aggressive_comptime_func(func, &snapshot3, &mut errs3);
        }
        for imp in prog.impls.iter_mut() {
            for method in imp.methods.iter_mut() {
                aggressive_comptime_func(method, &snapshot3, &mut errs3);
            }
        }
        for ti in prog.trait_impls.iter_mut() {
            for method in ti.methods.iter_mut() {
                aggressive_comptime_func(method, &snapshot3, &mut errs3);
            }
        }
        // Don't add aggressive errors — if it fails, just leave the call as-is
    }

    if errs.is_empty() {
        Ok(prog)
    } else {
        Err(errs)
    }
}

/// Process `comptime gen` statements in a function, extracting generated code.
fn process_comptime_gen_in_func(
    func: &mut Func,
    original: &Program,
    generated_funcs: &mut Vec<Func>,
    generated_structs: &mut Vec<StructDef>,
    errs: &mut Vec<String>,
) {
    let mut new_body = Vec::new();
    for stmt in func.body.drain(..) {
        if let Stmt::ComptimeGen { block, line } = stmt {
            // Evaluate the comptime block to get a string
            match evaluate_block(&block, original) {
                Ok(result) => {
                    if let Expr::StringLit(source) = result {
                        // Parse the generated source code
                        match parse_generated_code(&source) {
                            Ok((funcs, structs)) => {
                                generated_funcs.extend(funcs);
                                generated_structs.extend(structs);
                            }
                            Err(e) => {
                                errs.push(format!("comptime gen at line {}: failed to parse generated code: {}", line, e));
                            }
                        }
                    } else {
                        errs.push(format!("comptime gen at line {}: block must evaluate to a string", line));
                    }
                }
                Err(e) => {
                    errs.push(format!("comptime gen at line {}: {}", line, e.0));
                }
            }
        } else {
            new_body.push(stmt);
        }
    }
    func.body = new_body;
}

/// Parse generated Fides source code into functions and structs.
fn parse_generated_code(source: &str) -> Result<(Vec<Func>, Vec<StructDef>), String> {
    let mut lexer = crate::lexer::Lexer::new(source);
    let tokens = lexer.tokenize();
    let mut parser = crate::parser::Parser::new(tokens);
    let program = parser.parse()?;

    let funcs = program.funcs;
    let structs = program.structs;
    Ok((funcs, structs))
}

fn substitute_func(func: &mut Func, original: &Program, errs: &mut Vec<String>) {
    for stmt in func.body.iter_mut() {
        substitute_stmt(stmt, original, errs);
    }
}

/// I-5A: Aggressively evaluate calls to `comptime pf` functions when all
/// arguments are literals. This runs after normal comptime substitution and
/// automatically converts `comptime_func(1, 2)` → literal result, without
/// requiring an explicit `comptime` wrapper.
fn aggressive_comptime_func(func: &mut Func, original: &Program, errs: &mut Vec<String>) {
    for stmt in func.body.iter_mut() {
        aggressive_comptime_stmt(stmt, original, errs);
    }
}

fn aggressive_comptime_stmt(stmt: &mut Stmt, original: &Program, errs: &mut Vec<String>) {
    match stmt {
        Stmt::Let { expr, .. } => aggressive_comptime_expr(expr, original, errs),
        Stmt::LetTuple { expr, .. } => aggressive_comptime_expr(expr, original, errs),
        Stmt::Assign { expr, .. } => aggressive_comptime_expr(expr, original, errs),
        Stmt::IndexAssign { index, expr, .. } => {
            aggressive_comptime_expr(index, original, errs);
            aggressive_comptime_expr(expr, original, errs);
        }
        Stmt::Expr(e, _) => aggressive_comptime_expr(e, original, errs),
        Stmt::Return(opt, _) => {
            if let Some(e) = opt {
                aggressive_comptime_expr(e, original, errs);
            }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            aggressive_comptime_expr(cond, original, errs);
            for s in then_block.iter_mut() {
                aggressive_comptime_stmt(s, original, errs);
            }
            if let Some(eb) = else_block {
                for s in eb.iter_mut() {
                    aggressive_comptime_stmt(s, original, errs);
                }
            }
            // I-5A: If condition is a literal bool, eliminate the dead branch
            if let Expr::BoolLit(b) = cond {
                if *b {
                    // Replace if with then_block — but we can't replace a stmt
                    // with a list of stmts in place. Just clear the else block.
                    *else_block = None;
                } else {
                    // Replace if with else block — clear then block
                    then_block.clear();
                }
            }
        }
        Stmt::For { var, iterator, body, .. } => {
            aggressive_comptime_expr(iterator, original, errs);
            for s in body.iter_mut() {
                aggressive_comptime_stmt(s, original, errs);
            }
            // I-5D: Comptime loop unrolling — if the iterator is a Range with
            // literal bounds and the body is small (≤ 16 statements), unroll
            // the loop at the AST level. This makes the unrolled body available
            // for further comptime optimization (const folding, etc.).
            if let Expr::Range { start, end, inclusive } = iterator {
                if let (Expr::IntLit(s), Expr::IntLit(e)) = (start.as_ref(), end.as_ref()) {
                    let trip = if *inclusive { e - s + 1 } else { e - s };
                    if trip > 0 && trip <= 16 && body.len() <= 16 {
                        // Build unrolled body: for each iteration, clone the body
                        // with the loop variable replaced by a literal.
                        let mut unrolled: Vec<Stmt> = Vec::new();
                        let last = if *inclusive { *e + 1 } else { *e };
                        for i in *s..last {
                            for s in body.iter() {
                                let mut cloned = s.clone();
                                replace_ident_in_stmt(&mut cloned, var, &Expr::IntLit(i));
                                unrolled.push(cloned);
                            }
                        }
                        // Replace the entire for loop with an `if true` block
                        // containing the unrolled statements. This ensures the
                        // unrolled body actually executes. (We use `if true`
                        // because Stmt has no Block variant.)
                        *stmt = Stmt::If {
                            cond: Expr::BoolLit(true),
                            then_block: unrolled,
                            else_block: None,
                            line: 0,
                        };
                        return;
                    }
                }
            }
        }
        Stmt::While { cond, body, .. } => {
            aggressive_comptime_expr(cond, original, errs);
            for s in body.iter_mut() {
                aggressive_comptime_stmt(s, original, errs);
            }
        }
        _ => {}
    }
}

fn aggressive_comptime_expr(expr: &mut Expr, original: &Program, errs: &mut Vec<String>) {
    // First recurse into children
    match expr {
        Expr::Binary { left, right, .. } => {
            aggressive_comptime_expr(left, original, errs);
            aggressive_comptime_expr(right, original, errs);
        }
        Expr::Unary { expr: inner, .. } => aggressive_comptime_expr(inner, original, errs),
        Expr::Call { args, .. } => {
            for a in args.iter_mut() {
                aggressive_comptime_expr(a, original, errs);
            }
        }
        Expr::MethodCall { receiver, args, .. } => {
            aggressive_comptime_expr(receiver, original, errs);
            for a in args.iter_mut() {
                aggressive_comptime_expr(a, original, errs);
            }
        }
        Expr::If { cond, then_block, else_block, .. } => {
            aggressive_comptime_expr(cond, original, errs);
            for s in then_block.iter_mut() {
                aggressive_comptime_stmt(s, original, errs);
            }
            if let Some(eb) = else_block {
                for s in eb.iter_mut() {
                    aggressive_comptime_stmt(s, original, errs);
                }
            }
        }
        Expr::Block(stmts) => {
            for s in stmts.iter_mut() {
                aggressive_comptime_stmt(s, original, errs);
            }
        }
        Expr::Index { array, index } => {
            aggressive_comptime_expr(array, original, errs);
            aggressive_comptime_expr(index, original, errs);
        }
        Expr::StructLit { fields, .. } => {
            for (_, e) in fields.iter_mut() {
                aggressive_comptime_expr(e, original, errs);
            }
        }
        Expr::Match { expr: inner, arms } => {
            aggressive_comptime_expr(inner, original, errs);
            for arm in arms.iter_mut() {
                for s in arm.body.iter_mut() {
                    aggressive_comptime_stmt(s, original, errs);
                }
            }
        }
        _ => {}
    }

    // I-5A: If this is a call to a comptime pf function with all-literal args,
    // evaluate it at compile time and replace with the result.
    if let Expr::Call { callee, args } = expr {
        // Check if the callee is a comptime function
        let is_comptime_func = original.funcs.iter()
            .any(|f| f.name == *callee && f.is_comptime);
        if is_comptime_func && args.iter().all(is_literal) {
            match evaluate(expr, original) {
                Ok(result) => { *expr = result; }
                Err(_) => {} // Don't report errors — just leave the call as-is
            }
        }
    }
}

/// Check if an expression is a literal value (int, float, bool, string).
fn is_literal(expr: &Expr) -> bool {
    matches!(expr,
        Expr::IntLit(_) | Expr::FloatLit(_) | Expr::BoolLit(_) | Expr::StringLit(_)
    )
}

/// I-5D: Replace all occurrences of an identifier in a statement with a literal.
/// Used for comptime loop unrolling — the loop variable is replaced with the
/// iteration value.
fn replace_ident_in_stmt(stmt: &mut Stmt, name: &str, replacement: &Expr) {
    match stmt {
        Stmt::Let { expr, .. } => replace_ident_in_expr(expr, name, replacement),
        Stmt::LetTuple { expr, .. } => replace_ident_in_expr(expr, name, replacement),
        Stmt::Assign { expr, .. } => replace_ident_in_expr(expr, name, replacement),
        Stmt::IndexAssign { index, expr, .. } => {
            replace_ident_in_expr(index, name, replacement);
            replace_ident_in_expr(expr, name, replacement);
        }
        Stmt::Expr(e, _) => replace_ident_in_expr(e, name, replacement),
        Stmt::Return(opt, _) => {
            if let Some(e) = opt {
                replace_ident_in_expr(e, name, replacement);
            }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            replace_ident_in_expr(cond, name, replacement);
            for s in then_block.iter_mut() {
                replace_ident_in_stmt(s, name, replacement);
            }
            if let Some(eb) = else_block {
                for s in eb.iter_mut() {
                    replace_ident_in_stmt(s, name, replacement);
                }
            }
        }
        Stmt::For { var: v, iterator, body, .. } => {
            // Don't replace if the loop variable shadows our target
            if v != name {
                replace_ident_in_expr(iterator, name, replacement);
                for s in body.iter_mut() {
                    replace_ident_in_stmt(s, name, replacement);
                }
            }
        }
        Stmt::While { cond, body, .. } => {
            replace_ident_in_expr(cond, name, replacement);
            for s in body.iter_mut() {
                replace_ident_in_stmt(s, name, replacement);
            }
        }
        _ => {}
    }
}

fn replace_ident_in_expr(expr: &mut Expr, name: &str, replacement: &Expr) {
    match expr {
        Expr::Ident(n) if n == name => *expr = replacement.clone(),
        Expr::Binary { left, right, .. } => {
            replace_ident_in_expr(left, name, replacement);
            replace_ident_in_expr(right, name, replacement);
        }
        Expr::Unary { expr: inner, .. } => replace_ident_in_expr(inner, name, replacement),
        Expr::Call { args, .. } => {
            for a in args.iter_mut() {
                replace_ident_in_expr(a, name, replacement);
            }
        }
        Expr::MethodCall { receiver, args, .. } => {
            replace_ident_in_expr(receiver, name, replacement);
            for a in args.iter_mut() {
                replace_ident_in_expr(a, name, replacement);
            }
        }
        Expr::FieldAccess { expr: inner, .. } => replace_ident_in_expr(inner, name, replacement),
        Expr::Index { array, index } => {
            replace_ident_in_expr(array, name, replacement);
            replace_ident_in_expr(index, name, replacement);
        }
        Expr::StructLit { fields, .. } => {
            for (_, e) in fields.iter_mut() {
                replace_ident_in_expr(e, name, replacement);
            }
        }
        Expr::If { cond, then_block, else_block, .. } => {
            replace_ident_in_expr(cond, name, replacement);
            for s in then_block.iter_mut() {
                replace_ident_in_stmt(s, name, replacement);
            }
            if let Some(eb) = else_block {
                for s in eb.iter_mut() {
                    replace_ident_in_stmt(s, name, replacement);
                }
            }
        }
        Expr::Block(stmts) => {
            for s in stmts.iter_mut() {
                replace_ident_in_stmt(s, name, replacement);
            }
        }
        Expr::Range { start, end, .. } => {
            replace_ident_in_expr(start, name, replacement);
            replace_ident_in_expr(end, name, replacement);
        }
        Expr::Ref { expr: inner, .. } => replace_ident_in_expr(inner, name, replacement),
        Expr::Deref { expr: inner } => replace_ident_in_expr(inner, name, replacement),
        Expr::Cast { expr: inner, .. } => replace_ident_in_expr(inner, name, replacement),
        _ => {}
    }
}

fn substitute_stmts(stmts: &mut [Stmt], original: &Program, errs: &mut Vec<String>) {
    for stmt in stmts.iter_mut() {
        substitute_stmt(stmt, original, errs);
    }
}

fn substitute_stmt(stmt: &mut Stmt, original: &Program, errs: &mut Vec<String>) {
    match stmt {
        Stmt::Let { expr, .. } => substitute_expr(expr, original, errs),
        Stmt::LetUninit { .. } => {} // No expression to substitute
        Stmt::LetTuple { expr, .. } => substitute_expr(expr, original, errs),
        Stmt::Txn { body, .. } => substitute_stmts(body, original, errs),
        Stmt::Assign { expr, .. } => substitute_expr(expr, original, errs),
        Stmt::IndexAssign { index, expr, .. } => {
            substitute_expr(index, original, errs);
            substitute_expr(expr, original, errs);
        }
        Stmt::DerefAssign { ptr_expr, expr, .. } => {
            substitute_expr(ptr_expr, original, errs);
            substitute_expr(expr, original, errs);
        }
        Stmt::FieldAssign { object, expr, .. } => {
            substitute_expr(object, original, errs);
            substitute_expr(expr, original, errs);
        }
        Stmt::Expr(e, _) => substitute_expr(e, original, errs),
        Stmt::Return(opt, _) => {
            if let Some(e) = opt {
                substitute_expr(e, original, errs);
            }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            substitute_expr(cond, original, errs);
            substitute_stmts(then_block, original, errs);
            if let Some(eb) = else_block {
                substitute_stmts(eb, original, errs);
            }
        }
        Stmt::While { cond, body, .. } => {
            substitute_expr(cond, original, errs);
            substitute_stmts(body, original, errs);
        }
        Stmt::For { iterator, body, .. } => {
            substitute_expr(iterator, original, errs);
            substitute_stmts(body, original, errs);
        }
        Stmt::Match { expr, arms, .. } => {
            substitute_expr(expr, original, errs);
            for arm in arms.iter_mut() {
                substitute_stmts(&mut arm.body, original, errs);
            }
        }
        Stmt::Defer(stmts, _) => substitute_stmts(stmts, original, errs),
        Stmt::Break(_, _) | Stmt::Continue(_) => {}
        Stmt::Use { .. } | Stmt::Loop { .. } => {}
        Stmt::Handle { body, handlers, .. } => {
            substitute_stmts(body, original, errs);
            for h in handlers.iter_mut() {
                substitute_stmts(&mut h.body, original, errs);
            }
        }
        Stmt::Do { args, .. } => {
            for a in args.iter_mut() {
                substitute_expr(a, original, errs);
            }
        }
        Stmt::RegionBlock { body, .. } => {
            substitute_stmts(body, original, errs);
        }
        Stmt::ComptimeGen { block, .. } => {
            substitute_stmts(block, original, errs);
        }
        Stmt::Sandbox { body, .. } => {
            substitute_stmts(body, original, errs);
        }
        Stmt::Unsafe { body, .. } => {
            substitute_stmts(body, original, errs);
        }
        Stmt::GradBlock { body, .. } => {
            substitute_stmts(body, original, errs);
        }
        Stmt::Backward { loss, .. } => {
            substitute_expr(loss, original, errs);
        }
        Stmt::Query { body, .. } => substitute_stmts(body, original, errs),
        Stmt::Select { cases, default, .. } => {
            for case in cases {
                substitute_expr(&mut case.channel, original, errs);
                substitute_stmts(&mut case.body, original, errs);
            }
            if let Some(ref mut def) = default {
                substitute_stmts(def, original, errs);
            }
        }
    }
}

fn substitute_expr(expr: &mut Expr, original: &Program, errs: &mut Vec<String>) {
    // First recurse into children
    match expr {
        Expr::Binary { left, right, .. } => {
            substitute_expr(left, original, errs);
            substitute_expr(right, original, errs);
        }
        Expr::Unary { expr: inner, .. } => substitute_expr(inner, original, errs),
        Expr::Call { args, .. } => {
            for a in args.iter_mut() {
                substitute_expr(a, original, errs);
            }
        }
        Expr::MethodCall { receiver, args, .. } => {
            substitute_expr(receiver, original, errs);
            for a in args.iter_mut() {
                substitute_expr(a, original, errs);
            }
        }
        Expr::FieldAccess { expr: inner, .. } => substitute_expr(inner, original, errs),
        Expr::OptionalFieldAccess { expr: inner, .. } => substitute_expr(inner, original, errs),
        Expr::StructLit { fields, .. } => {
            for (_, e) in fields.iter_mut() {
                substitute_expr(e, original, errs);
            }
        }
        Expr::If { cond, then_block, else_block, .. } => {
            substitute_expr(cond, original, errs);
            substitute_stmts(then_block, original, errs);
            if let Some(eb) = else_block {
                substitute_stmts(eb, original, errs);
            }
        }
        Expr::Block(stmts) => substitute_stmts(stmts, original, errs),
        Expr::Index { array, index } => {
            substitute_expr(array, original, errs);
            substitute_expr(index, original, errs);
        }
        Expr::ArrayLit { elements, .. } => {
            for e in elements.iter_mut() {
                substitute_expr(e, original, errs);
            }
        }
        Expr::FString { .. } => { /* TODO: substitute inside f-string parts */ }
        Expr::Match { expr: inner, arms } => {
            substitute_expr(inner, original, errs);
            for arm in arms.iter_mut() {
                substitute_stmts(&mut arm.body, original, errs);
            }
        }
        Expr::Range { start, end, .. } => {
            substitute_expr(start, original, errs);
            substitute_expr(end, original, errs);
        }
        Expr::Ref { expr: inner, .. } => substitute_expr(inner, original, errs),
        Expr::Deref { expr: inner } => substitute_expr(inner, original, errs),
        Expr::StrConcat { left, right } => {
            substitute_expr(left, original, errs);
            substitute_expr(right, original, errs);
        }
        Expr::Try { expr: inner } => substitute_expr(inner, original, errs),
        Expr::Coalesce { expr, default } => {
            substitute_expr(expr, original, errs);
            substitute_expr(default, original, errs);
        }
        Expr::IsCheck { expr, .. } => substitute_expr(expr, original, errs),
        Expr::TryBlock { body } => substitute_stmts(body, original, errs),
        Expr::TupleLit(exprs) => {
            for e in exprs.iter_mut() {
                substitute_expr(e, original, errs);
            }
        }
        Expr::Closure { body, .. } => substitute_stmts(body, original, errs),
        Expr::Cast { expr: inner, .. } => substitute_expr(inner, original, errs),
        Expr::Asm { .. } => {}
        Expr::Comptime(inner) => {
            substitute_expr(inner, original, errs);
            // Now evaluate the comptime expression
            match evaluate(inner, original) {
                Ok(result) => { *expr = result; }
                Err(e) => errs.push(format!("comptime error: {}", e.0)),
            }
        }
        Expr::ComptimeBlock(stmts) => {
            substitute_stmts(stmts, original, errs);
            match evaluate_block(stmts, original) {
                Ok(result) => { *expr = result; }
                Err(e) => errs.push(format!("comptime error: {}", e.0)),
            }
        }
        Expr::ComptimeReflect { op, ty } => {
            match evaluate_reflect(op, ty, original) {
                Ok(result) => { *expr = result; }
                Err(e) => errs.push(format!("comptime error: {}", e.0)),
            }
        }
        Expr::IntLit(_) | Expr::FloatLit(_) | Expr::StringLit(_)
        | Expr::BoolLit(_) | Expr::Ident(_) | Expr::EnumLit { .. } => {}
        Expr::PipeForward { left, right } => {
            substitute_expr(left, original, errs);
            substitute_expr(right, original, errs);
        }
        Expr::QuantCast { expr: inner, .. } => {
            substitute_expr(inner, original, errs);
        }
        Expr::Await { expr: inner } | Expr::Spawn { expr: inner } => {
            substitute_expr(inner, original, errs);
        }
    }
}

// ─── Evaluation ───────────────────────────────────────────────────────────

/// Evaluate a comptime expression to a literal AST value.
fn evaluate(expr: &Expr, original: &Program) -> Result<Expr, ComptimeError> {
    match expr {
        Expr::IntLit(n) => Ok(Expr::IntLit(*n)),
        Expr::FloatLit(f) => Ok(Expr::FloatLit(*f)),
        Expr::StringLit(s) => Ok(Expr::StringLit(s.clone())),
        Expr::BoolLit(b) => Ok(Expr::BoolLit(*b)),
        Expr::Binary { op, left, right } => {
            let l = evaluate(left, original)?;
            let r = evaluate(right, original)?;
            eval_binop(op, &l, &r)
        }
        Expr::Unary { op, expr: inner } => {
            let v = evaluate(inner, original)?;
            eval_unaryop(op, &v)
        }
        Expr::Call { callee, args } => {
            // Look up comptime function
            let func = original.funcs.iter()
                .find(|f| f.name == *callee && f.is_comptime)
                .ok_or_else(|| ComptimeError(format!("comptime function '{}' not found", callee)))?;

            let arg_vals: Vec<Expr> = args.iter()
                .map(|a| evaluate(a, original))
                .collect::<Result<_, _>>()?;

            call_comptime_func(func, &arg_vals, original)
        }
        Expr::If { cond, then_block, else_block } => {
            let c = evaluate(cond, original)?;
            match c {
                Expr::BoolLit(true) => evaluate_block(then_block, original),
                Expr::BoolLit(false) => {
                    if let Some(eb) = else_block {
                        evaluate_block(eb, original)
                    } else {
                        Ok(Expr::IntLit(0))
                    }
                }
                _ => Err(ComptimeError("comptime if condition must be boolean".into())),
            }
        }
        Expr::Block(stmts) => evaluate_block(stmts, original),
        // Fallback: return as-is (already a literal)
        other => Ok(other.clone()),
    }
}

/// Evaluate a block of statements, returning the value of the last expression.
fn evaluate_block(stmts: &[Stmt], original: &Program) -> Result<Expr, ComptimeError> {
    let mut env: HashMap<String, Expr> = HashMap::new();
    let mut result = Expr::IntLit(0);

    for stmt in stmts {
        match stmt {
            Stmt::Let { name, expr, .. } => {
                let v = evaluate_with_env(expr, &env, original)?;
                env.insert(name.clone(), v);
            }
            Stmt::Assign { target, op, expr, .. } => {
                let v = evaluate_with_env(expr, &env, original)?;
                let current = env.get(target).cloned().unwrap_or(Expr::IntLit(0));
                let new_val = match op {
                    AssignOp::Assign => v,
                    AssignOp::AddAssign => eval_binop(&BinOp::Add, &current, &v)?,
                    AssignOp::SubAssign => eval_binop(&BinOp::Sub, &current, &v)?,
                    AssignOp::MulAssign => eval_binop(&BinOp::Mul, &current, &v)?,
                    _ => v,
                };
                env.insert(target.clone(), new_val);
            }
            Stmt::Expr(e, _) => {
                result = evaluate_with_env(e, &env, original)?;
            }
            Stmt::Return(opt, _) => {
                if let Some(e) = opt {
                    return evaluate_with_env(e, &env, original);
                }
                return Ok(Expr::IntLit(0));
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                let c = evaluate_with_env(cond, &env, original)?;
                match c {
                    Expr::BoolLit(true) => {
                        let mut sub_env = env.clone();
                        for s in then_block {
                            match s {
                                Stmt::Let { name, expr, .. } => {
                                    let v = evaluate_with_env(expr, &sub_env, original)?;
                                    sub_env.insert(name.clone(), v);
                                }
                                Stmt::Expr(e, _) => {
                                    result = evaluate_with_env(e, &sub_env, original)?;
                                }
                                Stmt::Return(opt, _) => {
                                    if let Some(e) = opt {
                                        return evaluate_with_env(e, &sub_env, original);
                                    }
                                    return Ok(Expr::IntLit(0));
                                }
                                _ => {}
                            }
                        }
                    }
                    Expr::BoolLit(false) => {
                        if let Some(eb) = else_block {
                            let mut sub_env = env.clone();
                            for s in eb {
                                match s {
                                    Stmt::Let { name, expr, .. } => {
                                        let v = evaluate_with_env(expr, &sub_env, original)?;
                                        sub_env.insert(name.clone(), v);
                                    }
                                    Stmt::Expr(e, _) => {
                                        result = evaluate_with_env(e, &sub_env, original)?;
                                    }
                                    Stmt::Return(opt, _) => {
                                        if let Some(e) = opt {
                                            return evaluate_with_env(e, &sub_env, original);
                                        }
                                        return Ok(Expr::IntLit(0));
                                    }
                                    _ => {}
                                }
                            }
                        }
                    }
                    _ => return Err(ComptimeError("comptime if condition must be boolean".into())),
                }
            }
            Stmt::Break(_, _) | Stmt::Continue(_) => {}
            Stmt::For { var, iterator, body, .. } => {
                // Evaluate the iterator (must be a Range)
                let iter_val = evaluate_with_env(iterator, &env, original)?;
                let (start, end) = match iter_val {
                    Expr::Range { start, end, .. } => {
                        let s = evaluate_with_env(&start, &env, original)?;
                        let e = evaluate_with_env(&end, &env, original)?;
                        match (s, e) {
                            (Expr::IntLit(a), Expr::IntLit(b)) => (a, b),
                            _ => return Err(ComptimeError("comptime for range must be ints".into())),
                        }
                    }
                    _ => return Err(ComptimeError("comptime for requires a range".into())),
                };
                for i in start..end {
                    env.insert(var.clone(), Expr::IntLit(i));
                    for s in body {
                        match s {
                            Stmt::Let { name, expr, .. } => {
                                let v = evaluate_with_env(expr, &env, original)?;
                                env.insert(name.clone(), v);
                            }
                            Stmt::Assign { target, op, expr, .. } => {
                                let v = evaluate_with_env(expr, &env, original)?;
                                let current = env.get(target).cloned().unwrap_or(Expr::IntLit(0));
                                let new_val = match op {
                                    AssignOp::Assign => v,
                                    AssignOp::AddAssign => eval_binop(&BinOp::Add, &current, &v)?,
                                    AssignOp::SubAssign => eval_binop(&BinOp::Sub, &current, &v)?,
                                    AssignOp::MulAssign => eval_binop(&BinOp::Mul, &current, &v)?,
                                    _ => v,
                                };
                                env.insert(target.clone(), new_val);
                            }
                            Stmt::Expr(e, _) => {
                                result = evaluate_with_env(e, &env, original)?;
                            }
                            Stmt::Return(opt, _) => {
                                if let Some(e) = opt {
                                    return evaluate_with_env(e, &env, original);
                                }
                                return Ok(Expr::IntLit(0));
                            }
                            _ => {}
                        }
                    }
                }
            }
            _ => {}
        }
    }

    Ok(result)
}

/// Evaluate an expression with variable bindings from the environment.
fn evaluate_with_env(expr: &Expr, env: &HashMap<String, Expr>, original: &Program) -> Result<Expr, ComptimeError> {
    match expr {
        Expr::Ident(name) => {
            env.get(name).cloned()
                .ok_or_else(|| ComptimeError(format!("undefined variable '{}' in comptime", name)))
        }
        Expr::IntLit(n) => Ok(Expr::IntLit(*n)),
        Expr::FloatLit(f) => Ok(Expr::FloatLit(*f)),
        Expr::StringLit(s) => Ok(Expr::StringLit(s.clone())),
        Expr::BoolLit(b) => Ok(Expr::BoolLit(*b)),
        Expr::Binary { op, left, right } => {
            let l = evaluate_with_env(left, env, original)?;
            let r = evaluate_with_env(right, env, original)?;
            eval_binop(op, &l, &r)
        }
        Expr::Unary { op, expr: inner } => {
            let v = evaluate_with_env(inner, env, original)?;
            eval_unaryop(op, &v)
        }
        Expr::Call { callee, args } => {
            // Look up comptime function
            let func = original.funcs.iter()
                .find(|f| f.name == *callee && f.is_comptime)
                .ok_or_else(|| ComptimeError(format!("comptime function '{}' not found", callee)))?;

            let arg_vals: Vec<Expr> = args.iter()
                .map(|a| evaluate_with_env(a, env, original))
                .collect::<Result<_, _>>()?;

            call_comptime_func(func, &arg_vals, original)
        }
        Expr::If { cond, then_block, else_block } => {
            let c = evaluate_with_env(cond, env, original)?;
            match c {
                Expr::BoolLit(true) => {
                    let mut sub_env = env.clone();
                    let mut result = Expr::IntLit(0);
                    for s in then_block {
                        match s {
                            Stmt::Let { name, expr, .. } => {
                                let v = evaluate_with_env(expr, &sub_env, original)?;
                                sub_env.insert(name.clone(), v);
                            }
                            Stmt::Expr(e, _) => {
                                result = evaluate_with_env(e, &sub_env, original)?;
                            }
                            Stmt::Return(opt, _) => {
                                if let Some(e) = opt {
                                    return evaluate_with_env(e, &sub_env, original);
                                }
                                return Ok(Expr::IntLit(0));
                            }
                            _ => {}
                        }
                    }
                    Ok(result)
                }
                Expr::BoolLit(false) => {
                    if let Some(eb) = else_block {
                        let mut sub_env = env.clone();
                        let mut result = Expr::IntLit(0);
                        for s in eb {
                            match s {
                                Stmt::Let { name, expr, .. } => {
                                    let v = evaluate_with_env(expr, &sub_env, original)?;
                                    sub_env.insert(name.clone(), v);
                                }
                                Stmt::Expr(e, _) => {
                                    result = evaluate_with_env(e, &sub_env, original)?;
                                }
                                Stmt::Return(opt, _) => {
                                    if let Some(e) = opt {
                                        return evaluate_with_env(e, &sub_env, original);
                                    }
                                    return Ok(Expr::IntLit(0));
                                }
                                _ => {}
                            }
                        }
                        Ok(result)
                    } else {
                        Ok(Expr::IntLit(0))
                    }
                }
                _ => Err(ComptimeError("comptime if condition must be boolean".into())),
            }
        }
        Expr::Block(stmts) => {
            let mut sub_env = env.clone();
            let mut result = Expr::IntLit(0);
            for s in stmts {
                match s {
                    Stmt::Let { name, expr, .. } => {
                        let v = evaluate_with_env(expr, &sub_env, original)?;
                        sub_env.insert(name.clone(), v);
                    }
                    Stmt::Expr(e, _) => {
                        result = evaluate_with_env(e, &sub_env, original)?;
                    }
                    Stmt::Return(opt, _) => {
                        if let Some(e) = opt {
                            return evaluate_with_env(e, &sub_env, original);
                        }
                        return Ok(Expr::IntLit(0));
                    }
                    _ => {}
                }
            }
            Ok(result)
        }
        Expr::FString { parts } => {
            // Evaluate f-string at comptime
            let mut result = String::new();
            for part in parts {
                match part {
                    FStringPart::Literal(s) => result.push_str(s),
                    FStringPart::Expr(e) => {
                        let v = evaluate_with_env(e, env, original)?;
                        match v {
                            Expr::IntLit(n) => result.push_str(&n.to_string()),
                            Expr::FloatLit(f) => result.push_str(&f.to_string()),
                            Expr::StringLit(s) => result.push_str(&s),
                            Expr::BoolLit(b) => result.push_str(&b.to_string()),
                            _ => result.push_str("?"),
                        }
                    }
                }
            }
            Ok(Expr::StringLit(result))
        }
        Expr::Index { array, index } => {
            // Array indexing at comptime (for field_names[i] etc.)
            let arr = evaluate_with_env(array, env, original)?;
            let idx = evaluate_with_env(index, env, original)?;
            match (&arr, &idx) {
                (Expr::ArrayLit { elements, .. }, Expr::IntLit(i)) => {
                    if *i >= 0 && (*i as usize) < elements.len() {
                        Ok(elements[*i as usize].clone())
                    } else {
                        Err(ComptimeError(format!("comptime index {} out of bounds", i)))
                    }
                }
                _ => Err(ComptimeError("comptime indexing requires array and int".into())),
            }
        }
        Expr::ArrayLit { elements, .. } => {
            // Evaluate each element
            let evals: Vec<Expr> = elements.iter()
                .map(|e| evaluate_with_env(e, env, original))
                .collect::<Result<_, _>>()?;
            Ok(Expr::ArrayLit {
                elem_type: Box::new(Type::Named("str".to_string())),
                elements: evals,
            })
        }
        Expr::StrConcat { left, right } => {
            let l = evaluate_with_env(left, env, original)?;
            let r = evaluate_with_env(right, env, original)?;
            match (&l, &r) {
                (Expr::StringLit(a), Expr::StringLit(b)) => Ok(Expr::StringLit(format!("{}{}", a, b))),
                _ => Err(ComptimeError("comptime str_concat requires strings".into())),
            }
        }
        // Fallback: try plain evaluate
        other => evaluate(other, original),
    }
}

/// Call a comptime function with evaluated arguments.
fn call_comptime_func(func: &Func, args: &[Expr], original: &Program) -> Result<Expr, ComptimeError> {
    if args.len() != func.params.len() {
        return Err(ComptimeError(format!(
            "comptime function '{}' expects {} args, got {}",
            func.name, func.params.len(), args.len()
        )));
    }

    let mut env: HashMap<String, Expr> = HashMap::new();
    for (param, arg) in func.params.iter().zip(args.iter()) {
        env.insert(param.name.clone(), arg.clone());
    }

    // Execute the function body
    let mut result = Expr::IntLit(0);
    for stmt in &func.body {
        match stmt {
            Stmt::Let { name, expr, .. } => {
                let v = evaluate_with_env(expr, &env, original)?;
                env.insert(name.clone(), v);
            }
            Stmt::Assign { target, op, expr, .. } => {
                let v = evaluate_with_env(expr, &env, original)?;
                let current = env.get(target).cloned().unwrap_or(Expr::IntLit(0));
                let new_val = match op {
                    AssignOp::Assign => v,
                    AssignOp::AddAssign => eval_binop(&BinOp::Add, &current, &v)?,
                    AssignOp::SubAssign => eval_binop(&BinOp::Sub, &current, &v)?,
                    AssignOp::MulAssign => eval_binop(&BinOp::Mul, &current, &v)?,
                    _ => v,
                };
                env.insert(target.clone(), new_val);
            }
            Stmt::Expr(e, _) => {
                result = evaluate_with_env(e, &env, original)?;
            }
            Stmt::Return(opt, _) => {
                if let Some(e) = opt {
                    return evaluate_with_env(e, &env, original);
                }
                return Ok(Expr::IntLit(0));
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                let c = evaluate_with_env(cond, &env, original)?;
                match c {
                    Expr::BoolLit(true) => {
                        let mut sub_env = env.clone();
                        for s in then_block {
                            match s {
                                Stmt::Let { name, expr, .. } => {
                                    let v = evaluate_with_env(expr, &sub_env, original)?;
                                    sub_env.insert(name.clone(), v);
                                }
                                Stmt::Return(opt, _) => {
                                    if let Some(e) = opt {
                                        return evaluate_with_env(e, &sub_env, original);
                                    }
                                    return Ok(Expr::IntLit(0));
                                }
                                Stmt::Expr(e, _) => {
                                    result = evaluate_with_env(e, &sub_env, original)?;
                                }
                                _ => {}
                            }
                        }
                    }
                    Expr::BoolLit(false) => {
                        if let Some(eb) = else_block {
                            let mut sub_env = env.clone();
                            for s in eb {
                                match s {
                                    Stmt::Let { name, expr, .. } => {
                                        let v = evaluate_with_env(expr, &sub_env, original)?;
                                        sub_env.insert(name.clone(), v);
                                    }
                                    Stmt::Return(opt, _) => {
                                        if let Some(e) = opt {
                                            return evaluate_with_env(e, &sub_env, original);
                                        }
                                        return Ok(Expr::IntLit(0));
                                    }
                                    Stmt::Expr(e, _) => {
                                        result = evaluate_with_env(e, &sub_env, original)?;
                                    }
                                    _ => {}
                                }
                            }
                        }
                    }
                    _ => return Err(ComptimeError("comptime if condition must be boolean".into())),
                }
            }
            Stmt::Break(_, _) | Stmt::Continue(_) => {}
            Stmt::For { var, iterator, body, .. } => {
                // Evaluate the iterator (must be a Range)
                let iter_val = evaluate_with_env(iterator, &env, original)?;
                let (start, end) = match iter_val {
                    Expr::Range { start, end, .. } => {
                        let s = evaluate_with_env(&start, &env, original)?;
                        let e = evaluate_with_env(&end, &env, original)?;
                        match (s, e) {
                            (Expr::IntLit(a), Expr::IntLit(b)) => (a, b),
                            _ => return Err(ComptimeError("comptime for range must be ints".into())),
                        }
                    }
                    _ => return Err(ComptimeError("comptime for requires a range".into())),
                };
                for i in start..end {
                    env.insert(var.clone(), Expr::IntLit(i));
                    for s in body {
                        match s {
                            Stmt::Let { name, expr, .. } => {
                                let v = evaluate_with_env(expr, &env, original)?;
                                env.insert(name.clone(), v);
                            }
                            Stmt::Assign { target, op, expr, .. } => {
                                let v = evaluate_with_env(expr, &env, original)?;
                                let current = env.get(target).cloned().unwrap_or(Expr::IntLit(0));
                                let new_val = match op {
                                    AssignOp::Assign => v,
                                    AssignOp::AddAssign => eval_binop(&BinOp::Add, &current, &v)?,
                                    AssignOp::SubAssign => eval_binop(&BinOp::Sub, &current, &v)?,
                                    AssignOp::MulAssign => eval_binop(&BinOp::Mul, &current, &v)?,
                                    _ => v,
                                };
                                env.insert(target.clone(), new_val);
                            }
                            Stmt::Expr(e, _) => {
                                result = evaluate_with_env(e, &env, original)?;
                            }
                            Stmt::Return(opt, _) => {
                                if let Some(e) = opt {
                                    return evaluate_with_env(e, &env, original);
                                }
                                return Ok(Expr::IntLit(0));
                            }
                            _ => {}
                        }
                    }
                }
            }
            _ => {}
        }
    }

    Ok(result)
}

/// Evaluate a binary operation on literal expressions.
fn eval_binop(op: &BinOp, l: &Expr, r: &Expr) -> Result<Expr, ComptimeError> {
    match (l, r) {
        (Expr::IntLit(a), Expr::IntLit(b)) => {
            let v = match op {
                BinOp::Add => a.wrapping_add(*b),
                BinOp::Sub => a.wrapping_sub(*b),
                BinOp::Mul => a.wrapping_mul(*b),
                BinOp::Div => {
                    if *b == 0 { return Err(ComptimeError("division by zero in comptime".into())); }
                    a.wrapping_div(*b)
                }
                BinOp::Mod => {
                    if *b == 0 { return Err(ComptimeError("modulo by zero in comptime".into())); }
                    a.wrapping_rem(*b)
                }
                BinOp::Eq => return Ok(Expr::BoolLit(a == b)),
                BinOp::Neq => return Ok(Expr::BoolLit(a != b)),
                BinOp::Lt => return Ok(Expr::BoolLit(a < b)),
                BinOp::LtEq => return Ok(Expr::BoolLit(a <= b)),
                BinOp::Gt => return Ok(Expr::BoolLit(a > b)),
                BinOp::GtEq => return Ok(Expr::BoolLit(a >= b)),
                BinOp::And => return Ok(Expr::BoolLit(*a != 0 && *b != 0)),
                BinOp::Or => return Ok(Expr::BoolLit(*a != 0 || *b != 0)),
                _ => return Err(ComptimeError(format!("unsupported binop {:?} for ints", op))),
            };
            Ok(Expr::IntLit(v))
        }
        (Expr::FloatLit(a), Expr::FloatLit(b)) => {
            let v = match op {
                BinOp::Add => a + b,
                BinOp::Sub => a - b,
                BinOp::Mul => a * b,
                BinOp::Div => a / b,
                BinOp::Eq => return Ok(Expr::BoolLit(a == b)),
                BinOp::Neq => return Ok(Expr::BoolLit(a != b)),
                BinOp::Lt => return Ok(Expr::BoolLit(a < b)),
                BinOp::LtEq => return Ok(Expr::BoolLit(a <= b)),
                BinOp::Gt => return Ok(Expr::BoolLit(a > b)),
                BinOp::GtEq => return Ok(Expr::BoolLit(a >= b)),
                _ => return Err(ComptimeError(format!("unsupported binop {:?} for floats", op))),
            };
            Ok(Expr::FloatLit(v))
        }
        (Expr::StringLit(a), Expr::StringLit(b)) => {
            match op {
                BinOp::Add => Ok(Expr::StringLit(format!("{}{}", a, b))),
                BinOp::Eq => Ok(Expr::BoolLit(a == b)),
                BinOp::Neq => Ok(Expr::BoolLit(a != b)),
                _ => Err(ComptimeError(format!("unsupported binop {:?} for strings", op))),
            }
        }
        (Expr::BoolLit(a), Expr::BoolLit(b)) => {
            match op {
                BinOp::Eq => Ok(Expr::BoolLit(a == b)),
                BinOp::Neq => Ok(Expr::BoolLit(a != b)),
                BinOp::And => Ok(Expr::BoolLit(*a && *b)),
                BinOp::Or => Ok(Expr::BoolLit(*a || *b)),
                _ => Err(ComptimeError(format!("unsupported binop {:?} for bools", op))),
            }
        }
        _ => Err(ComptimeError(format!("cannot evaluate binop {:?} on {:?} and {:?}", op, l, r))),
    }
}

/// Evaluate a unary operation on a literal expression.
fn eval_unaryop(op: &UnaryOp, v: &Expr) -> Result<Expr, ComptimeError> {
    match (op, v) {
        (UnaryOp::Neg, Expr::IntLit(n)) => Ok(Expr::IntLit(-n)),
        (UnaryOp::Neg, Expr::FloatLit(f)) => Ok(Expr::FloatLit(-f)),
        (UnaryOp::Not, Expr::BoolLit(b)) => Ok(Expr::BoolLit(!b)),
        (UnaryOp::Not, Expr::IntLit(n)) => Ok(Expr::IntLit(!n)),
        (UnaryOp::BitNot, Expr::IntLit(n)) => Ok(Expr::IntLit(!n)),
        _ => Err(ComptimeError(format!("cannot evaluate unaryop {:?} on {:?}", op, v))),
    }
}

// ─── Reflection ───────────────────────────────────────────────────────────

fn evaluate_reflect(op: &ReflectOp, ty: &Type, original: &Program) -> Result<Expr, ComptimeError> {
    let type_name = match ty {
        Type::Named(name) => name.clone(),
        Type::I8 => "i8".into(),
        Type::I16 => "i16".into(),
        Type::I32 => "i32".into(),
        Type::I64 => "i64".into(),
        Type::I128 => "i128".into(),
        Type::U8 => "u8".into(),
        Type::U16 => "u16".into(),
        Type::U32 => "u32".into(),
        Type::U64 => "u64".into(),
        Type::U128 => "u128".into(),
        Type::F32 => "f32".into(),
        Type::F64 => "f64".into(),
        Type::Bool => "bool".into(),
        Type::Str => "str".into(),
        Type::Void => "void".into(),
        _ => return Err(ComptimeError(format!("reflect: unsupported type {:?}", ty))),
    };

    match op {
        ReflectOp::TypeName => Ok(Expr::StringLit(type_name)),
        ReflectOp::IsStruct => {
            Ok(Expr::BoolLit(original.structs.iter().any(|s| s.name == type_name)))
        }
        ReflectOp::IsEnum => {
            Ok(Expr::BoolLit(original.enums.iter().any(|e| e.name == type_name)))
        }
        ReflectOp::FieldCount => {
            let s = original.structs.iter()
                .find(|s| s.name == type_name)
                .ok_or_else(|| ComptimeError(format!("type_name: '{}' is not a struct", type_name)))?;
            Ok(Expr::IntLit(s.fields.len() as i64))
        }
        ReflectOp::FieldNames => {
            let s = original.structs.iter()
                .find(|s| s.name == type_name)
                .ok_or_else(|| ComptimeError(format!("field_names: '{}' is not a struct", type_name)))?;
            let elements: Vec<Expr> = s.fields.iter()
                .map(|f| Expr::StringLit(f.name.clone()))
                .collect();
            // Element type is Str (lowers to *u8, matching StringLit → ConstStr)
            Ok(Expr::ArrayLit {
                elem_type: Box::new(Type::Str),
                elements,
            })
        }
        ReflectOp::FieldTypes => {
            let s = original.structs.iter()
                .find(|s| s.name == type_name)
                .ok_or_else(|| ComptimeError(format!("field_types: '{}' is not a struct", type_name)))?;
            let elements: Vec<Expr> = s.fields.iter()
                .map(|f| Expr::StringLit(format!("{}", f.ty)))
                .collect();
            Ok(Expr::ArrayLit {
                elem_type: Box::new(Type::Str),
                elements,
            })
        }
        ReflectOp::EnumVariants => {
            let e = original.enums.iter()
                .find(|e| e.name == type_name)
                .ok_or_else(|| ComptimeError(format!("enum_variants: '{}' is not an enum", type_name)))?;
            let elements: Vec<Expr> = e.variants.iter()
                .map(|v| Expr::StringLit(v.name.clone()))
                .collect();
            Ok(Expr::ArrayLit {
                elem_type: Box::new(Type::Str),
                elements,
            })
        }
    }
}
