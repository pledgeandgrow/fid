//! Reference escape check: references are second-class values.
//!
//! Fides has no lifetime annotations. A reference (`&T`, `&mut T`) may be
//! passed to a call, held in a local and returned when it comes from one of
//! the function's reference parameters, but never kept anywhere it could
//! outlive what it points to:
//!
//! 1. A returned reference must be rooted in a reference parameter. Which
//!    parameters can reach a function's result is inferred, so callers need
//!    no annotations.
//! 2. Storage cannot hold a reference: struct fields, enum payloads, globals,
//!    array and collection elements, `Box`/`Rc`/`Arc`, generic arguments other
//!    than `Option`/`Result`/tuples, and references themselves (no `&&T`).
//! 3. A reference assigned to a variable must not point into a block that ends
//!    before the variable does, and the value of a block, `if`, `match` or
//!    loop must not point at one of its own locals.
//! 4. Closures capture by value, so a closure that captures a reference is
//!    rooted like that reference and may not escape further than it.
//! 5. Code inside `unsafe { }` is not checked.
//!
//! Runs after type checking. See `docs/guides/references.md`.

use crate::ast::{Expr, Func, MatchArm, Pattern, Program, Stmt, Type};
use std::collections::{BTreeSet, HashMap, HashSet};

/// What a reference-holding value points into.
#[derive(Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
enum Root {
    /// A local variable (or by-value parameter) declared at block depth `.1`.
    Local(String, usize),
    /// The function's `i`-th parameter, a reference the caller supplied.
    Param(usize),
}

type Roots = BTreeSet<Root>;

struct Var {
    depth: usize,
    roots: Roots,
    /// The value is itself a reference (rather than an `Option`, `Result` or
    /// tuple holding one): what it points to holds no reference (rule 2).
    is_ref: bool,
}

/// Read-only builtins that may be handed a reference.
const READ_ONLY_CALLS: &[&str] = &["print", "println", "eprint", "eprintln", "assert", "assert_eq", "format", "panic", "debug"];
const READ_ONLY_METHODS: &[&str] = &[
    "contains", "starts_with", "ends_with", "find", "index_of", "eq", "equals", "cmp", "compare", "len", "get",
];

/// `Option`, `Result` and tuples pass a reference through without storing it.
fn is_transparent(name: &str) -> bool {
    matches!(name, "Option" | "Result")
}

/// Whether `ty` holds a reference anywhere (function types do not hold their
/// parameters' or result's references).
fn contains_ref(ty: &Type) -> bool {
    match ty {
        Type::Ref { .. } => true,
        Type::Generic { args, .. } | Type::Tuple(args) => args.iter().any(contains_ref),
        Type::Array { elem, .. } | Type::ArrayConst { elem, .. } | Type::ShapedTensor { elem, .. } => contains_ref(elem),
        _ => false,
    }
}

fn union(sets: &[Roots]) -> Roots {
    sets.iter().flat_map(|s| s.iter().cloned()).collect()
}

/// Names of the variables a closure body reads (a superset of its captures).
fn idents_in_stmts(stmts: &[Stmt], out: &mut HashSet<String>) {
    for s in stmts {
        idents_in_stmt(s, out);
    }
}

fn idents_in_stmt(s: &Stmt, out: &mut HashSet<String>) {
    match s {
        Stmt::Let { expr, .. } | Stmt::LetTuple { expr, .. } | Stmt::Assign { expr, .. } | Stmt::Expr(expr, _) => idents_in_expr(expr, out),
        Stmt::IndexAssign { index, expr, .. } => { idents_in_expr(index, out); idents_in_expr(expr, out); }
        Stmt::DerefAssign { ptr_expr, expr, .. } => { idents_in_expr(ptr_expr, out); idents_in_expr(expr, out); }
        Stmt::FieldAssign { object, expr, .. } => { idents_in_expr(object, out); idents_in_expr(expr, out); }
        Stmt::Return(Some(e), _) | Stmt::Break(Some(e), _) => idents_in_expr(e, out),
        Stmt::If { cond, then_block, else_block, .. } => {
            idents_in_expr(cond, out);
            idents_in_stmts(then_block, out);
            if let Some(b) = else_block { idents_in_stmts(b, out); }
        }
        Stmt::While { cond, body, .. } => { idents_in_expr(cond, out); idents_in_stmts(body, out); }
        Stmt::For { iterator, body, .. } => { idents_in_expr(iterator, out); idents_in_stmts(body, out); }
        Stmt::Match { expr, arms, .. } => {
            idents_in_expr(expr, out);
            for a in arms { idents_in_stmts(&a.body, out); }
        }
        Stmt::Loop { body, .. } | Stmt::Defer(body, _) | Stmt::Unsafe { body, .. } | Stmt::Sandbox { body, .. }
        | Stmt::Txn { body, .. } | Stmt::Handle { body, .. } | Stmt::RegionBlock { body, .. }
        | Stmt::SpawnScope { body, .. } | Stmt::Query { body, .. } | Stmt::GradBlock { body, .. } => idents_in_stmts(body, out),
        _ => {}
    }
}

fn idents_in_expr(e: &Expr, out: &mut HashSet<String>) {
    match e {
        Expr::Ident(n) => { out.insert(n.clone()); }
        Expr::Binary { left, right, .. } | Expr::StrConcat { left, right } | Expr::PipeForward { left, right }
        | Expr::Range { start: left, end: right, .. } => { idents_in_expr(left, out); idents_in_expr(right, out); }
        Expr::Unary { expr, .. } | Expr::FieldAccess { expr, .. } | Expr::OptionalFieldAccess { expr, .. }
        | Expr::Ref { expr, .. } | Expr::Deref { expr } | Expr::Try { expr } | Expr::IsCheck { expr, .. }
        | Expr::Cast { expr, .. } | Expr::QuantCast { expr, .. } | Expr::Await { expr } | Expr::Spawn { expr }
        | Expr::Comptime(expr) => idents_in_expr(expr, out),
        Expr::Coalesce { expr, default } => { idents_in_expr(expr, out); idents_in_expr(default, out); }
        Expr::Call { callee, args } => { out.insert(callee.clone()); for a in args { idents_in_expr(a, out); } }
        Expr::MethodCall { receiver, args, .. } => { idents_in_expr(receiver, out); for a in args { idents_in_expr(a, out); } }
        Expr::EnumLit { args, .. } | Expr::TupleLit(args) | Expr::ArrayLit { elements: args, .. } => {
            for a in args { idents_in_expr(a, out); }
        }
        Expr::StructLit { fields, spread, .. } => {
            for (_, v) in fields { idents_in_expr(v, out); }
            if let Some(s) = spread { idents_in_expr(s, out); }
        }
        Expr::Index { array, index } => { idents_in_expr(array, out); idents_in_expr(index, out); }
        Expr::If { cond, then_block, else_block } => {
            idents_in_expr(cond, out);
            idents_in_stmts(then_block, out);
            if let Some(b) = else_block { idents_in_stmts(b, out); }
        }
        Expr::Match { expr, arms } => { idents_in_expr(expr, out); for a in arms { idents_in_stmts(&a.body, out); } }
        Expr::Block(b) | Expr::TryBlock { body: b } | Expr::ComptimeBlock(b) | Expr::Closure { body: b, .. } => idents_in_stmts(b, out),
        Expr::FString { parts } => {
            for p in parts {
                if let crate::ast::FStringPart::Expr(x) = p { idents_in_expr(x, out); }
            }
        }
        _ => {}
    }
}

pub struct EscapeChecker<'a> {
    /// Every function body with its key: the name it is called by
    /// (`Type_method` for methods).
    all: Vec<(String, &'a Func)>,
    funcs: HashMap<String, &'a Func>,
    /// Method name -> keys of the functions a `.method(..)` call may resolve to.
    methods: HashMap<String, Vec<String>>,
    globals: HashSet<String>,
    /// Function key -> indices of the parameters that may reach its result.
    summaries: HashMap<String, BTreeSet<usize>>,
    scopes: Vec<HashMap<String, Var>>,
    func_name: String,
    flows: BTreeSet<usize>,
    loop_depths: Vec<usize>,
    /// Block depth a closure body starts at (0 outside closures).
    closure_base: usize,
    unsafe_depth: usize,
    line: usize,
    report: bool,
    pub errors: Vec<String>,
}

impl<'a> EscapeChecker<'a> {
    fn new(program: &'a Program) -> Self {
        let mut all: Vec<(String, &'a Func)> = Vec::new();
        for f in &program.funcs {
            all.push((f.name.clone(), f));
        }
        for imp in &program.impls {
            for m in &imp.methods {
                all.push((format!("{}_{}", imp.struct_name, m.name), m));
            }
        }
        for ti in &program.trait_impls {
            for m in &ti.methods {
                all.push((format!("{}_{}", ti.type_name, m.name), m));
            }
        }
        let mut funcs: HashMap<String, &'a Func> = HashMap::new();
        let mut methods: HashMap<String, Vec<String>> = HashMap::new();
        for (key, f) in &all {
            funcs.entry(key.clone()).or_insert(f);
            // `Type_method` is callable as `.method()`; the type name may
            // itself contain underscores, so register every suffix.
            for (pos, _) in key.match_indices('_') {
                methods.entry(key[pos + 1..].to_string()).or_default().push(key.clone());
            }
        }
        let mut globals: HashSet<String> = program.globals.iter().map(|g| g.name.clone()).collect();
        globals.extend(program.consts.iter().map(|c| c.name.clone()));
        EscapeChecker {
            all,
            funcs,
            methods,
            globals,
            summaries: HashMap::new(),
            scopes: Vec::new(),
            func_name: String::new(),
            flows: BTreeSet::new(),
            loop_depths: Vec::new(),
            closure_base: 0,
            unsafe_depth: 0,
            line: 0,
            report: false,
            errors: Vec::new(),
        }
    }

    fn error(&mut self, msg: String) {
        if self.report && self.unsafe_depth == 0 {
            let full = format!("Reference error (line {}): in '{}', {}", self.line, self.func_name, msg);
            if !self.errors.contains(&full) {
                self.errors.push(full);
            }
        }
    }

    // --- Types -------------------------------------------------------------

    /// A parameter, local or return type: a reference may sit at the top or
    /// inside `Option`/`Result`/tuples, never inside storage.
    fn check_value_type(&mut self, ty: &Type, what: &str) {
        match ty {
            Type::Ref { inner, .. } => {
                if contains_ref(inner) {
                    self.error(format!("{} has type '{}': a reference cannot point to another reference", what, ty));
                }
            }
            Type::Generic { name, args } if is_transparent(name) => {
                for a in args {
                    self.check_value_type(a, what);
                }
            }
            Type::Tuple(args) => {
                for a in args {
                    self.check_value_type(a, what);
                }
            }
            _ => self.check_storage_type(ty, what),
        }
    }

    fn check_storage_type(&mut self, ty: &Type, what: &str) {
        if contains_ref(ty) {
            self.error(format!(
                "{} has type '{}', which stores a reference; references can only be passed to calls, held in locals and returned",
                what, ty
            ));
        }
    }

    fn check_decls(&mut self, program: &Program) {
        self.report = true;
        for s in &program.structs {
            self.func_name = s.name.clone();
            self.line = s.line;
            for f in &s.fields {
                self.check_storage_type(&f.ty, &format!("field '{}.{}'", s.name, f.name));
            }
        }
        for e in &program.enums {
            self.func_name = e.name.clone();
            self.line = e.line;
            for v in &e.variants {
                for p in v.data.iter().flatten() {
                    self.check_storage_type(&p.ty, &format!("variant '{}::{}'", e.name, v.name));
                }
            }
        }
        for g in &program.globals {
            self.func_name = g.name.clone();
            self.line = 0;
            self.check_storage_type(&g.ty, &format!("global '{}'", g.name));
        }
        self.report = false;
    }

    // --- Scopes ------------------------------------------------------------

    fn depth(&self) -> usize {
        self.scopes.len()
    }

    fn declare(&mut self, name: &str, roots: Roots, is_ref: bool) {
        let depth = self.depth();
        self.scopes.last_mut().unwrap().insert(name.to_string(), Var { depth, roots, is_ref });
    }

    /// Whether `e` evaluates to a reference itself (so a `match` on it binds
    /// plain payload values, which hold no reference).
    fn is_reference(&self, e: &Expr) -> bool {
        match e {
            Expr::Ref { .. } => true,
            Expr::Ident(name) => self.lookup(name).map_or(false, |v| v.is_ref),
            Expr::Call { callee, .. } => self.funcs.get(callee).map_or(false, |f| matches!(f.ret_ty, Type::Ref { .. })),
            Expr::MethodCall { method, .. } => self.methods.get(method).map_or(false, |keys| {
                !keys.is_empty() && keys.iter().all(|k| self.funcs.get(k).map_or(false, |f| matches!(f.ret_ty, Type::Ref { .. })))
            }),
            _ => false,
        }
    }

    /// The roots a `match` on `e` passes to its bindings.
    fn binding_roots(&mut self, e: &Expr) -> Roots {
        let r = self.expr(e);
        if self.is_reference(e) {
            Roots::new()
        } else {
            r
        }
    }

    fn lookup(&self, name: &str) -> Option<&Var> {
        self.scopes.iter().rev().find_map(|s| s.get(name))
    }

    /// Drop the roots that point into blocks deeper than `depth`, reporting
    /// each one: a value leaving those blocks would dangle.
    fn leave_blocks(&mut self, roots: Roots, depth: usize, what: &str) -> Roots {
        let mut kept = Roots::new();
        for r in roots {
            match &r {
                Root::Local(name, d) if *d > depth => {
                    self.error(format!("{} refers to '{}', which is freed at the end of its block", what, name));
                }
                _ => {
                    kept.insert(r);
                }
            }
        }
        kept
    }

    // --- Functions ---------------------------------------------------------

    fn analyze_func(&mut self, key: &str, f: &Func) {
        self.func_name = f.name.clone();
        self.line = f.line;
        self.flows.clear();
        self.scopes = vec![HashMap::new()];
        self.loop_depths.clear();
        self.closure_base = 0;
        self.unsafe_depth = 0;
        for (i, p) in f.params.iter().enumerate() {
            self.check_value_type(&p.ty, &format!("parameter '{}'", p.name));
            let roots: Roots = if contains_ref(&p.ty) { [Root::Param(i)].into_iter().collect() } else { Roots::new() };
            self.declare(&p.name, roots, matches!(p.ty, Type::Ref { .. }));
        }
        self.check_value_type(&f.ret_ty, "the return type");
        if matches!(f.ret_ty, Type::Void) {
            for s in &f.body {
                self.stmt(s);
            }
        } else {
            let tail = self.stmts_value(&f.body);
            self.check_return(tail);
        }
        self.summaries.insert(key.to_string(), self.flows.clone());
    }

    fn check_return(&mut self, roots: Roots) {
        for r in roots {
            match r {
                Root::Param(i) => {
                    if self.closure_base == 0 {
                        self.flows.insert(i);
                    }
                }
                Root::Local(name, d) => {
                    if d > self.closure_base {
                        let f = self.func_name.clone();
                        let what = if self.closure_base == 0 { format!("'{}'", f) } else { "a closure".to_string() };
                        self.error(format!(
                            "{} returns a reference to local '{}', which is freed when it returns",
                            what, name
                        ));
                    }
                }
            }
        }
    }

    // --- Statements --------------------------------------------------------

    /// Process `stmts` in the current scope and return the roots of their
    /// value (the last statement's, when it is an expression or a branch).
    fn stmts_value(&mut self, stmts: &[Stmt]) -> Roots {
        let n = stmts.len();
        for s in stmts.iter().take(n.saturating_sub(1)) {
            self.stmt(s);
        }
        match stmts.last() {
            None => Roots::new(),
            Some(last) => self.stmt_value(last),
        }
    }

    fn stmt_value(&mut self, s: &Stmt) -> Roots {
        self.line = s.line();
        match s {
            Stmt::Expr(e, _) => self.expr(e),
            Stmt::If { cond, then_block, else_block, .. } => {
                self.expr(cond);
                let mut r = self.block_value(then_block, "the value of this branch");
                if let Some(b) = else_block {
                    r.extend(self.block_value(b, "the value of this branch"));
                }
                r
            }
            Stmt::Match { expr, arms, .. } => {
                let scrutinee = self.binding_roots(expr);
                self.arms(arms, &scrutinee, true)
            }
            Stmt::Unsafe { body, .. } => {
                self.unsafe_depth += 1;
                let r = self.block_value(body, "the value of this block");
                self.unsafe_depth -= 1;
                r
            }
            Stmt::Sandbox { body, .. } | Stmt::Txn { body, .. } | Stmt::RegionBlock { body, .. } | Stmt::Handle { body, .. } => {
                self.block_value(body, "the value of this block")
            }
            _ => {
                self.stmt(s);
                Roots::new()
            }
        }
    }

    /// Evaluate a nested block's value; it must not point at the block's own locals.
    fn block_value(&mut self, stmts: &[Stmt], what: &str) -> Roots {
        let outer = self.depth();
        self.scopes.push(HashMap::new());
        let r = self.stmts_value(stmts);
        self.scopes.pop();
        self.leave_blocks(r, outer, what)
    }

    fn block(&mut self, stmts: &[Stmt]) {
        self.scopes.push(HashMap::new());
        for s in stmts {
            self.stmt(s);
        }
        self.scopes.pop();
    }

    /// Check the arms of a `match`; with `want_value`, also return the roots
    /// of the value it produces.
    fn arms(&mut self, arms: &[MatchArm], scrutinee: &Roots, want_value: bool) -> Roots {
        let outer = self.depth();
        let mut out = Roots::new();
        for arm in arms {
            self.scopes.push(HashMap::new());
            match &arm.pattern {
                // A binding is the reference an Option/Result/tuple held.
                Pattern::Ident(name) => self.declare(name, scrutinee.clone(), !scrutinee.is_empty()),
                Pattern::EnumVariant { bindings, .. } => {
                    for (name, _) in bindings {
                        self.declare(name, scrutinee.clone(), !scrutinee.is_empty());
                    }
                }
                _ => {}
            }
            if let Some(g) = &arm.guard {
                self.expr(g);
            }
            if want_value {
                let r = self.stmts_value(&arm.body);
                self.scopes.pop();
                out.extend(self.leave_blocks(r, outer, "the value of this match arm"));
            } else {
                for s in &arm.body {
                    self.stmt(s);
                }
                self.scopes.pop();
            }
        }
        out
    }

    fn loop_body(&mut self, body: &[Stmt]) {
        self.loop_depths.push(self.depth());
        self.block(body);
        self.loop_depths.pop();
    }

    fn stmt(&mut self, s: &Stmt) {
        self.line = s.line();
        match s {
            Stmt::Let { name, ty, expr, .. } => {
                if let Some(t) = ty {
                    self.check_value_type(t, &format!("local '{}'", name));
                }
                let r = self.expr(expr);
                let is_ref = match ty {
                    Some(t) => matches!(t, Type::Ref { .. }),
                    None => self.is_reference(expr),
                };
                self.declare(name, r, is_ref);
            }
            Stmt::LetTuple { names, expr, .. } => {
                let r = self.expr(expr);
                for n in names {
                    self.declare(n, r.clone(), false);
                }
            }
            Stmt::LetUninit { name, ty, .. } => {
                self.check_value_type(ty, &format!("local '{}'", name));
                self.declare(name, Roots::new(), matches!(ty, Type::Ref { .. }));
            }
            Stmt::Assign { target, expr, .. } => {
                let r = self.expr(expr);
                self.assign_to(target, r);
            }
            Stmt::IndexAssign { array, index, expr, .. } => {
                self.expr(index);
                let r = self.expr(expr);
                if !r.is_empty() {
                    self.error(format!("a reference cannot be stored in an element of '{}'", array));
                }
            }
            Stmt::FieldAssign { object, field, expr, .. } => {
                self.expr(object);
                let r = self.expr(expr);
                if !r.is_empty() {
                    if field.chars().all(|c| c.is_ascii_digit()) {
                        if let Expr::Ident(base) = object {
                            let base = base.clone();
                            self.assign_to(&base, r);
                        }
                    } else {
                        self.error(format!("a reference cannot be stored in field '{}'", field));
                    }
                }
            }
            Stmt::DerefAssign { ptr_expr, expr, .. } => {
                self.expr(ptr_expr);
                let r = self.expr(expr);
                if !r.is_empty() {
                    self.error("a reference cannot be stored through another reference".to_string());
                }
            }
            Stmt::Expr(e, _) => {
                self.expr(e);
            }
            Stmt::Return(Some(e), _) => {
                let r = self.expr(e);
                self.check_return(r);
            }
            Stmt::Break(Some(e), _) => {
                let r = self.expr(e);
                if let Some(&d) = self.loop_depths.last() {
                    self.leave_blocks(r, d, "the value of this loop");
                }
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                self.expr(cond);
                self.block(then_block);
                if let Some(b) = else_block {
                    self.block(b);
                }
            }
            Stmt::While { cond, body, .. } => {
                self.expr(cond);
                self.loop_body(body);
            }
            Stmt::For { var, iterator, body, .. } => {
                self.expr(iterator);
                self.loop_depths.push(self.depth());
                self.scopes.push(HashMap::new());
                self.declare(var, Roots::new(), false);
                for st in body {
                    self.stmt(st);
                }
                self.scopes.pop();
                self.loop_depths.pop();
            }
            Stmt::Loop { body, .. } => self.loop_body(body),
            Stmt::Match { expr, arms, .. } => {
                let r = self.binding_roots(expr);
                self.arms(arms, &r, false);
            }
            Stmt::Unsafe { body, .. } => {
                self.unsafe_depth += 1;
                self.block(body);
                self.unsafe_depth -= 1;
            }
            Stmt::Defer(body, _) | Stmt::Sandbox { body, .. } | Stmt::Txn { body, .. } | Stmt::Handle { body, .. }
            | Stmt::RegionBlock { body, .. } | Stmt::SpawnScope { body, .. } | Stmt::Query { body, .. }
            | Stmt::GradBlock { body, .. } => self.block(body),
            Stmt::Parallel { branches, .. } => {
                for b in branches {
                    self.block(b);
                }
            }
            Stmt::Select { cases, default, .. } => {
                for c in cases {
                    self.expr(&c.channel);
                    self.scopes.push(HashMap::new());
                    self.declare(&c.var_name, Roots::new(), false);
                    for st in &c.body {
                        self.stmt(st);
                    }
                    self.scopes.pop();
                }
                if let Some(d) = default {
                    self.block(d);
                }
            }
            Stmt::Do { args, .. } => {
                for a in args {
                    self.expr(a);
                }
            }
            Stmt::Resume { cont, value, .. } => {
                self.expr(cont);
                self.expr(value);
            }
            Stmt::Backward { loss, .. } => {
                self.expr(loss);
            }
            _ => {}
        }
    }

    /// `target = <value with roots r>`: the target must not outlive any root.
    fn assign_to(&mut self, target: &str, r: Roots) {
        if r.is_empty() {
            return;
        }
        let target_depth = match self.lookup(target) {
            Some(v) => v.depth,
            None => {
                if self.globals.contains(target) {
                    self.error(format!("global '{}' cannot hold a reference", target));
                }
                return;
            }
        };
        let kept = self.leave_blocks_for(r, target_depth, target);
        for s in self.scopes.iter_mut().rev() {
            if let Some(v) = s.get_mut(target) {
                v.roots.extend(kept);
                break;
            }
        }
    }

    fn leave_blocks_for(&mut self, roots: Roots, depth: usize, target: &str) -> Roots {
        let mut kept = Roots::new();
        for r in roots {
            match &r {
                Root::Local(name, d) if *d > depth => {
                    self.error(format!(
                        "'{}' would outlive '{}': '{}' is freed at the end of its block while '{}' is still in scope",
                        target, name, name, target
                    ));
                }
                _ => {
                    kept.insert(r);
                }
            }
        }
        kept
    }

    // --- Expressions -------------------------------------------------------

    /// The roots of `e`'s value (empty when it holds no reference).
    fn expr(&mut self, e: &Expr) -> Roots {
        match e {
            Expr::Ident(name) => self.lookup(name).map(|v| v.roots.clone()).unwrap_or_default(),
            Expr::Ref { expr, .. } => self.place(expr),
            Expr::Deref { expr } | Expr::FieldAccess { expr, .. } | Expr::OptionalFieldAccess { expr, .. } => {
                let r = self.expr(expr);
                // A tuple field passes its reference through; any other field,
                // and a dereferenced value, cannot hold one (rule 2).
                if let Expr::FieldAccess { field, .. } = e {
                    if field.chars().all(|c| c.is_ascii_digit()) {
                        return r;
                    }
                }
                Roots::new()
            }
            Expr::Index { array, index } => {
                self.expr(array);
                self.expr(index);
                Roots::new()
            }
            Expr::Binary { left, right, .. } | Expr::StrConcat { left, right } | Expr::Range { start: left, end: right, .. } => {
                self.expr(left);
                self.expr(right);
                Roots::new()
            }
            Expr::Unary { expr, .. } | Expr::IsCheck { expr, .. } | Expr::Cast { expr, .. } | Expr::QuantCast { expr, .. }
            | Expr::Comptime(expr) => {
                self.expr(expr);
                Roots::new()
            }
            Expr::Await { expr } => self.expr(expr),
            Expr::Spawn { expr } => {
                // Everything handed to the task: a call's arguments as well as its result.
                let mut r = self.expr(expr);
                match &**expr {
                    Expr::Call { args, .. } => {
                        for a in args {
                            r.extend(self.expr(a));
                        }
                    }
                    Expr::MethodCall { receiver, args, .. } => {
                        r.extend(self.expr(receiver));
                        for a in args {
                            r.extend(self.expr(a));
                        }
                    }
                    _ => {}
                }
                if !r.is_empty() {
                    self.error("a reference cannot be sent to another task".to_string());
                }
                Roots::new()
            }
            Expr::Try { expr } => self.expr(expr),
            Expr::Coalesce { expr, default } => {
                let mut r = self.expr(expr);
                r.extend(self.expr(default));
                r
            }
            Expr::Call { callee, args } => self.call(callee, args),
            Expr::MethodCall { receiver, method, args } => self.method_call(receiver, method, args),
            Expr::PipeForward { left, right } => {
                let mut r = self.expr(left);
                r.extend(self.expr(right));
                r
            }
            Expr::StructLit { name, fields, spread } => {
                for (fname, v) in fields {
                    let r = self.expr(v);
                    if !r.is_empty() {
                        self.error(format!("a reference cannot be stored in field '{}.{}'", name, fname));
                    }
                }
                if let Some(s) = spread {
                    self.expr(s);
                }
                Roots::new()
            }
            Expr::EnumLit { enum_name, variant, args } => {
                let rs: Vec<Roots> = args.iter().map(|a| self.expr(a)).collect();
                if is_transparent(enum_name) {
                    return union(&rs);
                }
                if rs.iter().any(|r| !r.is_empty()) {
                    self.error(format!("a reference cannot be stored in variant '{}::{}'", enum_name, variant));
                }
                Roots::new()
            }
            Expr::TupleLit(items) => {
                let rs: Vec<Roots> = items.iter().map(|a| self.expr(a)).collect();
                union(&rs)
            }
            Expr::ArrayLit { elements, .. } => {
                for el in elements {
                    let r = self.expr(el);
                    if !r.is_empty() {
                        self.error("a reference cannot be stored in an array".to_string());
                    }
                }
                Roots::new()
            }
            Expr::If { cond, then_block, else_block } => {
                self.expr(cond);
                let mut r = self.block_value(then_block, "the value of this `if`");
                if let Some(b) = else_block {
                    r.extend(self.block_value(b, "the value of this `if`"));
                }
                r
            }
            Expr::Match { expr, arms } => {
                let s = self.binding_roots(expr);
                self.arms(arms, &s, true)
            }
            Expr::Block(b) | Expr::TryBlock { body: b } => self.block_value(b, "the value of this block"),
            Expr::ComptimeBlock(_) => Roots::new(),
            Expr::Closure { params, body, .. } => self.closure(params, body),
            Expr::FString { parts } => {
                for p in parts {
                    if let crate::ast::FStringPart::Expr(x) = p {
                        self.expr(x);
                    }
                }
                Roots::new()
            }
            _ => Roots::new(),
        }
    }

    /// The roots of `&place`.
    fn place(&mut self, e: &Expr) -> Roots {
        match e {
            // The variable's own storage (for a reference variable `r`, `&r`
            // points at `r` itself, a local).
            Expr::Ident(name) => match self.lookup(name) {
                Some(v) => [Root::Local(name.clone(), v.depth)].into_iter().collect(),
                None => Roots::new(),
            },
            Expr::FieldAccess { expr, .. } | Expr::OptionalFieldAccess { expr, .. } => self.place_inside(expr),
            Expr::Index { array, index } => {
                self.expr(index);
                self.place_inside(array)
            }
            Expr::Deref { expr } => self.expr(expr),
            other => {
                // `&f()`, `&(a + b)`: a temporary in the current block.
                self.expr(other);
                [Root::Local("a temporary value".to_string(), self.depth())].into_iter().collect()
            }
        }
    }

    /// The roots of a place inside `base` (`base.field`, `base[i]`): when
    /// `base` is a reference the place is behind it, otherwise it is part of
    /// `base`'s own storage.
    fn place_inside(&mut self, base: &Expr) -> Roots {
        let through = self.expr(base);
        if through.is_empty() {
            self.place(base)
        } else {
            through
        }
    }

    fn call(&mut self, callee: &str, args: &[Expr]) -> Roots {
        let rs: Vec<Roots> = args.iter().map(|a| self.expr(a)).collect();
        if matches!(callee, "Some" | "Ok" | "Err") {
            return union(&rs);
        }
        // Calling a closure held in a local: its result may be any reference
        // it captured or was given.
        if let Some(v) = self.lookup(callee) {
            let mut r = v.roots.clone();
            r.extend(union(&rs));
            return r;
        }
        if let Some(f) = self.funcs.get(callee).copied() {
            for (i, r) in rs.iter().enumerate() {
                if r.is_empty() || READ_ONLY_CALLS.contains(&callee) {
                    continue;
                }
                if let Some(p) = f.params.get(i) {
                    if !contains_ref(&p.ty) {
                        let what = if matches!(args[i], Expr::Closure { .. }) { "a closure that captures a reference" } else { "a reference" };
                        self.error(format!(
                            "{} cannot be passed to parameter '{}' of '{}' (type '{}'), which may keep it; only reference parameters can borrow",
                            what, p.name, callee, p.ty
                        ));
                    }
                }
            }
            let flows = self.summaries.get(callee).cloned().unwrap_or_default();
            let picked: Vec<Roots> = flows.iter().filter_map(|&i| rs.get(i).cloned()).collect();
            return union(&picked);
        }
        Roots::new()
    }

    fn method_call(&mut self, receiver: &Expr, method: &str, args: &[Expr]) -> Roots {
        let recv = self.expr(receiver);
        let rs: Vec<Roots> = args.iter().map(|a| self.expr(a)).collect();
        let keys: Vec<String> = self.methods.get(method).cloned().unwrap_or_default();
        let candidates: Vec<(String, &'a Func)> = keys.into_iter().filter_map(|k| self.funcs.get(&k).map(|f| (k.clone(), *f))).collect();
        for (j, r) in rs.iter().enumerate() {
            if r.is_empty() {
                continue;
            }
            // Argument j is the method's parameter j + 1 (after `self`).
            let borrows = candidates.iter().any(|(_, f)| f.params.get(j + 1).map_or(false, |p| contains_ref(&p.ty)));
            if !borrows && !READ_ONLY_METHODS.contains(&method) {
                self.error(format!("a reference cannot be passed to method '{}', which may keep it", method));
            }
        }
        let mut out = Roots::new();
        for (key, f) in &candidates {
            if !contains_ref(&f.ret_ty) {
                continue;
            }
            let flows = self.summaries.get(key).cloned().unwrap_or_default();
            for i in flows {
                if i == 0 {
                    out.extend(recv.iter().cloned());
                } else if let Some(r) = rs.get(i - 1) {
                    out.extend(r.iter().cloned());
                }
            }
        }
        out
    }

    fn closure(&mut self, params: &[crate::ast::Param], body: &[Stmt]) -> Roots {
        // Captured references root the closure value (captures are by value).
        let mut used = HashSet::new();
        idents_in_stmts(body, &mut used);
        let param_names: HashSet<&str> = params.iter().map(|p| p.name.as_str()).collect();
        let mut roots = Roots::new();
        for name in &used {
            if param_names.contains(name.as_str()) {
                continue;
            }
            if let Some(v) = self.lookup(name) {
                roots.extend(v.roots.iter().cloned());
            }
        }
        // Check the body as its own function: its locals must not escape it.
        let saved_base = self.closure_base;
        let saved_loops = std::mem::take(&mut self.loop_depths);
        self.scopes.push(HashMap::new());
        self.closure_base = self.depth() - 1;
        for p in params {
            self.check_value_type(&p.ty, &format!("closure parameter '{}'", p.name));
            self.declare(&p.name, Roots::new(), matches!(p.ty, Type::Ref { .. }));
        }
        let tail = self.stmts_value(body);
        self.check_return(tail);
        self.scopes.pop();
        self.closure_base = saved_base;
        self.loop_depths = saved_loops;
        roots
    }
}

/// Run the escape check over every function of `program`.
pub fn check(program: &Program) -> Vec<String> {
    let mut c = EscapeChecker::new(program);
    c.check_decls(program);
    let all = c.all.clone();
    // Infer which parameters reach each function's result, to a fixpoint.
    loop {
        let before = c.summaries.clone();
        for (key, f) in &all {
            c.analyze_func(key, f);
        }
        if c.summaries == before {
            break;
        }
    }
    c.report = true;
    for (key, f) in &all {
        c.analyze_func(key, f);
    }
    c.errors
}
