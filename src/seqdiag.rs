//! Sequence-diagram generator — `fidesc diagram <file.fd>` emits a Mermaid
//! `sequenceDiagram` of the calls in the file's functions, annotated with the
//! inferred effect set of each callee. Purely a visualization tool: it reads
//! the typechecker's effect map and never changes semantics.

use crate::ast::{Expr, Program, Stmt};
use std::collections::{HashMap, HashSet};
use std::fmt::Write;

/// Render `program`'s user functions as a Mermaid sequence diagram.
/// `effects` maps function name → inferred effect names (see
/// `TypeChecker::func_effects`).
pub fn mermaid(program: &Program, effects: &HashMap<String, HashSet<String>>) -> String {
    let mut out = String::from("sequenceDiagram\n");
    for f in &program.funcs {
        writeln!(out, "    participant {}", f.name).unwrap();
    }
    for imp in &program.impls {
        for m in &imp.methods {
            writeln!(out, "    participant {}", m.name).unwrap();
        }
    }

    for f in &program.funcs {
        let mut edges = Vec::new();
        collect_stmts(&f.body, &mut edges);
        emit_edges(&mut out, &f.name, &edges, effects);
        emit_note(&mut out, &f.name, effects);
    }
    for imp in &program.impls {
        for m in &imp.methods {
            let mut edges = Vec::new();
            collect_stmts(&m.body, &mut edges);
            emit_edges(&mut out, &m.name, &edges, effects);
            emit_note(&mut out, &m.name, effects);
        }
    }
    out
}

fn emit_edges(
    out: &mut String,
    caller: &str,
    edges: &[(String, String)],
    effects: &HashMap<String, HashSet<String>>,
) {
    let mut seen: HashSet<(String, String)> = HashSet::new();
    for (target, label) in edges {
        if !seen.insert((target.clone(), label.clone())) {
            continue;
        }
        let eff = effects
            .get(target.as_str())
            .map(|e| {
                let mut v: Vec<&String> = e.iter().collect();
                v.sort();
                if v.is_empty() {
                    String::new()
                } else {
                    format!(" !{{{}}}", v.iter().map(|s| s.as_str()).collect::<Vec<_>>().join(", "))
                }
            })
            .unwrap_or_default();
        writeln!(out, "    {}->>{}: {}{}", caller, target, label, eff).unwrap();
    }
}

fn emit_note(out: &mut String, name: &str, effects: &HashMap<String, HashSet<String>>) {
    if let Some(effs) = effects.get(name) {
        let mut v: Vec<&String> = effs.iter().collect();
        v.sort();
        let body = if v.is_empty() {
            "pure".to_string()
        } else {
            format!("!{{{}}}", v.iter().map(|s| s.as_str()).collect::<Vec<_>>().join(", "))
        };
        writeln!(out, "    Note over {}: {}", name, body).unwrap();
    }
}

fn collect_stmts(stmts: &[Stmt], edges: &mut Vec<(String, String)>) {
    for s in stmts {
        collect_stmt(s, edges);
    }
}

fn collect_stmt(s: &Stmt, edges: &mut Vec<(String, String)>) {
    match s {
        Stmt::Expr(e, _) => collect_expr(e, edges),
        Stmt::Let { expr, .. }
        | Stmt::LetTuple { expr, .. }
        | Stmt::Assign { expr, .. }
        | Stmt::DerefAssign { expr, .. } => collect_expr(expr, edges),
        Stmt::IndexAssign { index, expr, .. } => {
            collect_expr(index, edges);
            collect_expr(expr, edges);
        }
        Stmt::FieldAssign { object, expr, .. } => {
            collect_expr(object, edges);
            collect_expr(expr, edges);
        }
        Stmt::Return(e, _) | Stmt::Break(e, _) => {
            if let Some(e) = e {
                collect_expr(e, edges);
            }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            collect_expr(cond, edges);
            collect_stmts(then_block, edges);
            if let Some(eb) = else_block {
                collect_stmts(eb, edges);
            }
        }
        Stmt::While { cond, body, .. } => {
            collect_expr(cond, edges);
            collect_stmts(body, edges);
        }
        Stmt::For { iterator, body, .. } => {
            collect_expr(iterator, edges);
            collect_stmts(body, edges);
        }
        Stmt::Loop { body, .. }
        | Stmt::Defer(body, _)
        | Stmt::RegionBlock { body, .. }
        | Stmt::Sandbox { body, .. }
        | Stmt::Unsafe { body, .. }
        | Stmt::Txn { body, .. } => collect_stmts(body, edges),
        Stmt::Match { expr, arms, .. } => {
            collect_expr(expr, edges);
            for arm in arms {
                collect_stmts(&arm.body, edges);
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            collect_stmts(body, edges);
            for h in handlers {
                collect_stmts(&h.body, edges);
            }
        }
        Stmt::Do { effect, op, args, .. } => {
            for a in args {
                collect_expr(a, edges);
            }
            edges.push((format!("«{}»", effect), format!("do {}", op)));
        }
        Stmt::Resume { cont, value, .. } => {
            collect_expr(cont, edges);
            collect_expr(value, edges);
            edges.push(("«handler»".to_string(), "resume".to_string()));
        }
        Stmt::SpawnScope { body, .. } => {
            edges.push(("«scope»".to_string(), "spawn scope".to_string()));
            collect_stmts(body, edges);
        }
        _ => {}
    }
}

fn collect_expr(e: &Expr, edges: &mut Vec<(String, String)>) {
    match e {
        Expr::Call { callee, args } => {
            for a in args {
                collect_expr(a, edges);
            }
            edges.push((callee.clone(), format!("{}()", callee)));
        }
        Expr::MethodCall { receiver, method, args } => {
            collect_expr(receiver, edges);
            for a in args {
                collect_expr(a, edges);
            }
            edges.push((method.clone(), format!("{}()", method)));
        }
        Expr::Binary { left, right, .. }
        | Expr::StrConcat { left, right }
        | Expr::Coalesce { expr: left, default: right }
        | Expr::Range { start: left, end: right, .. }
        | Expr::PipeForward { left, right } => {
            collect_expr(left, edges);
            collect_expr(right, edges);
        }
        Expr::Unary { expr, .. }
        | Expr::FieldAccess { expr, .. }
        | Expr::OptionalFieldAccess { expr, .. }
        | Expr::Ref { expr, .. }
        | Expr::Deref { expr }
        | Expr::Try { expr }
        | Expr::Cast { expr, .. }
        | Expr::Comptime(expr)
        | Expr::IsCheck { expr, .. } => collect_expr(expr, edges),
        Expr::If { cond, then_block, else_block } => {
            collect_expr(cond, edges);
            collect_stmts(then_block, edges);
            if let Some(eb) = else_block {
                collect_stmts(eb, edges);
            }
        }
        Expr::Block(stmts) | Expr::TryBlock { body: stmts } | Expr::ComptimeBlock(stmts) => {
            collect_stmts(stmts, edges);
        }
        Expr::Match { expr, arms } => {
            collect_expr(expr, edges);
            for arm in arms {
                collect_stmts(&arm.body, edges);
            }
        }
        Expr::StructLit { fields, spread, .. } => {
            for (_, fe) in fields {
                collect_expr(fe, edges);
            }
            if let Some(s) = spread {
                collect_expr(s, edges);
            }
        }
        Expr::ArrayLit { elements, .. } | Expr::TupleLit(elements) => {
            for el in elements {
                collect_expr(el, edges);
            }
        }
        Expr::EnumLit { args, .. } => {
            for a in args {
                collect_expr(a, edges);
            }
        }
        Expr::Index { array, index } => {
            collect_expr(array, edges);
            collect_expr(index, edges);
        }
        Expr::Closure { body, .. } => collect_stmts(body, edges),
        Expr::Spawn { expr } => {
            edges.push(("«task»".to_string(), "spawn".to_string()));
            collect_expr(expr, edges);
        }
        _ => {}
    }
}
