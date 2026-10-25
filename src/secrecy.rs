//! `@secret` information-flow check: a secret never reaches an output.
//!
//! Sources of secrets:
//! - a parameter marked `@secret` (the function promises not to leak it),
//! - a local marked `@secret` (`@secret val key = load_key();`),
//! - the result of a function marked `@secret` (its result stays secret).
//!
//! Sinks: output and logging calls, network and file writes, globals, writes
//! through a reference parameter, `spawn`, and calls through function values
//! (which could do any of these).
//!
//! A value's label records where it may have come from: a real secret, or the
//! function's `i`-th parameter. A parameter label reaching a sink means the
//! function outputs that parameter, which becomes part of its summary, so a
//! secret passed to a helper that prints it is caught at the call. Summaries
//! (parameters a function outputs, parameters reaching its result, whether it
//! outputs at all) are inferred to a fixpoint. Code inside a branch or loop
//! whose condition depends on a secret carries that secret too, so an output
//! there leaks it ("implicit flow").
//!
//! A function may return a secret only if it is marked `@secret` (callers then
//! treat its result as secret) or `@declassify` (an audited release: callers
//! treat its result as public).
//!
//! Runs after type checking. See `docs/guides/secrets.md`.

use crate::ast::{Expr, FStringPart, Func, MatchArm, Pattern, Program, Stmt};
use std::collections::{BTreeSet, HashMap, HashSet};

#[derive(Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
enum Origin {
    /// A real secret.
    Secret,
    /// The function's `i`-th parameter.
    Param(usize),
}

type Label = BTreeSet<Origin>;

#[derive(Clone, Debug, Default, PartialEq)]
struct Summary {
    /// Parameters the function may output.
    leaks: BTreeSet<usize>,
    /// Parameters that may reach the function's result.
    returns: BTreeSet<usize>,
    /// Whether the function may produce output at all.
    outputs: bool,
}

const OUTPUT_SINKS: &[&str] = &[
    "print", "println", "eprint", "eprintln",
    "p_print_int", "p_print_str", "p_print_float", "p_print_bool", "p_print_newline",
    "p_eprint_str", "p_print_vec",
    "p_socket_send", "p_socket_send_str", "p_http_send_response",
    "p_file_write", "p_file_append", "p_file_write_bin",
    "log_info", "log_warn", "log_error", "log_debug",
    "p_chan_send",
];

/// Methods that read their receiver without storing their arguments in it.
const READ_ONLY_METHODS: &[&str] = &[
    "get", "len", "is_empty", "contains", "index_of", "find", "starts_with", "ends_with",
    "first", "last", "eq", "cmp", "compare", "to_str", "to_string", "clone", "get_or",
];

fn has_annotation(f: &Func, name: &str) -> bool {
    f.annotations.iter().any(|a| a.name == name)
}

fn union(a: &Label, b: &Label) -> Label {
    a.union(b).cloned().collect()
}

pub struct SecrecyChecker<'a> {
    all: Vec<(String, &'a Func)>,
    funcs: HashMap<String, &'a Func>,
    methods: HashMap<String, Vec<String>>,
    globals: HashSet<String>,
    summaries: HashMap<String, Summary>,
    // Summaries the current function's analysis read (its callees).
    reads: HashSet<String>,
    // Per-function state. Labels are flow-insensitive: a variable's label is
    // the union over every value it is ever given.
    vars: HashMap<String, Label>,
    ref_params: HashSet<String>,
    pc: Vec<Label>,
    func_name: String,
    may_return_secret: bool,
    declassify: bool,
    cur: Summary,
    changed: bool,
    line: usize,
    report: bool,
    pub errors: Vec<String>,
}

impl<'a> SecrecyChecker<'a> {
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
            for (pos, _) in key.match_indices('_') {
                methods.entry(key[pos + 1..].to_string()).or_default().push(key.clone());
            }
        }
        let mut globals: HashSet<String> = program.globals.iter().map(|g| g.name.clone()).collect();
        globals.extend(program.consts.iter().map(|c| c.name.clone()));
        SecrecyChecker {
            all,
            funcs,
            methods,
            globals,
            summaries: HashMap::new(),
            reads: HashSet::new(),
            vars: HashMap::new(),
            ref_params: HashSet::new(),
            pc: Vec::new(),
            func_name: String::new(),
            may_return_secret: false,
            declassify: false,
            cur: Summary::default(),
            changed: false,
            line: 0,
            report: false,
            errors: Vec::new(),
        }
    }

    fn error(&mut self, msg: String) {
        if self.report {
            let full = format!("Security error (@secret) (line {}): in '{}', {}", self.line, self.func_name, msg);
            if !self.errors.contains(&full) {
                self.errors.push(full);
            }
        }
    }

    fn pc(&self) -> Label {
        self.pc.last().cloned().unwrap_or_default()
    }

    fn push_pc(&mut self, extra: &Label) {
        let next = union(&self.pc(), extra);
        self.pc.push(next);
    }

    /// Give `name` (at least) the label `l`.
    fn assign_var(&mut self, name: &str, l: &Label) {
        let entry = self.vars.entry(name.to_string()).or_default();
        let before = entry.len();
        entry.extend(l.iter().cloned());
        if entry.len() != before {
            self.changed = true;
        }
    }

    fn note_leak(&mut self, i: usize) {
        if self.cur.leaks.insert(i) {
            self.changed = true;
        }
    }

    fn note_output(&mut self) {
        if !self.cur.outputs {
            self.cur.outputs = true;
            self.changed = true;
        }
    }

    /// `data` reaches the output `what` while the code runs under `pc`.
    fn sink(&mut self, what: &str, data: &Label, pc: &Label) {
        self.note_output();
        if data.contains(&Origin::Secret) {
            self.error(format!("a secret value reaches {}", what));
        } else if pc.contains(&Origin::Secret) {
            self.error(format!("{} runs inside a branch or loop that depends on a secret", what));
        }
        for o in data.iter().chain(pc.iter()) {
            if let Origin::Param(i) = o {
                self.note_leak(*i);
            }
        }
    }

    fn ret(&mut self, l: &Label) {
        if self.declassify {
            return;
        }
        for o in l {
            match o {
                Origin::Param(i) => {
                    if self.cur.returns.insert(*i) {
                        self.changed = true;
                    }
                }
                Origin::Secret => {
                    if !self.may_return_secret {
                        self.error(
                            "returns a secret value; mark the function @secret (callers keep its result secret) or @declassify (an audited release)"
                                .to_string(),
                        );
                    }
                }
            }
        }
    }

    // --- Functions ---------------------------------------------------------

    fn analyze_func(&mut self, key: &str, f: &Func) {
        self.func_name = f.name.clone();
        self.line = f.line;
        self.may_return_secret = has_annotation(f, "secret");
        self.declassify = has_annotation(f, "declassify");
        self.cur = self.summaries.get(key).cloned().unwrap_or_default();
        self.vars.clear();
        self.ref_params.clear();
        for (i, p) in f.params.iter().enumerate() {
            let l: Label = if p.annotations.iter().any(|a| a.name == "secret") {
                [Origin::Secret].into_iter().collect()
            } else {
                [Origin::Param(i)].into_iter().collect()
            };
            self.vars.insert(p.name.clone(), l);
            if matches!(p.ty, crate::ast::Type::Ref { .. }) {
                self.ref_params.insert(p.name.clone());
            }
        }
        // Labels only grow, so a few passes reach the function's fixpoint.
        for _ in 0..8 {
            self.changed = false;
            self.pc.clear();
            let tail = self.stmts_value(&f.body);
            if !matches!(f.ret_ty, crate::ast::Type::Void) {
                self.ret(&tail);
            }
            if !self.changed {
                break;
            }
        }
        self.summaries.insert(key.to_string(), self.cur.clone());
    }

    // --- Statements --------------------------------------------------------

    fn stmts(&mut self, stmts: &[Stmt]) {
        for s in stmts {
            self.stmt(s);
        }
    }

    /// Process `stmts` and return the label of their value (the last statement's).
    fn stmts_value(&mut self, stmts: &[Stmt]) -> Label {
        let n = stmts.len();
        for s in stmts.iter().take(n.saturating_sub(1)) {
            self.stmt(s);
        }
        match stmts.last() {
            Some(Stmt::Expr(e, line)) => {
                self.line = *line;
                self.expr(e)
            }
            Some(Stmt::If { cond, then_block, else_block, line }) => {
                self.line = *line;
                let c = self.expr(cond);
                self.push_pc(&c);
                let mut l = self.stmts_value(then_block);
                if let Some(b) = else_block {
                    l.extend(self.stmts_value(b));
                }
                self.pc.pop();
                union(&l, &c)
            }
            Some(Stmt::Match { expr, arms, line }) => {
                self.line = *line;
                let s = self.expr(expr);
                let l = self.arms(arms, &s);
                union(&l, &s)
            }
            Some(other) => {
                self.stmt(other);
                Label::new()
            }
            None => Label::new(),
        }
    }

    fn arms(&mut self, arms: &[MatchArm], scrutinee: &Label) -> Label {
        self.push_pc(scrutinee);
        let pc = self.pc();
        let mut out = Label::new();
        for arm in arms {
            let bound = union(scrutinee, &pc);
            match &arm.pattern {
                Pattern::Ident(name) => self.assign_var(name, &bound),
                Pattern::EnumVariant { bindings, .. } => {
                    for (name, _) in bindings {
                        self.assign_var(name, &bound);
                    }
                }
                _ => {}
            }
            if let Some(g) = &arm.guard {
                self.expr(g);
            }
            out.extend(self.stmts_value(&arm.body));
        }
        self.pc.pop();
        out
    }

    /// The variable a place expression (`x`, `x.f`, `x[i]`, `*x`) writes into.
    fn base_var(e: &Expr) -> Option<&str> {
        match e {
            Expr::Ident(n) => Some(n),
            Expr::FieldAccess { expr, .. } | Expr::OptionalFieldAccess { expr, .. } | Expr::Deref { expr } => Self::base_var(expr),
            Expr::Index { array, .. } => Self::base_var(array),
            _ => None,
        }
    }

    /// `name` (directly or through a field, element or dereference) receives `l`.
    fn store(&mut self, name: &str, l: &Label) {
        if self.vars.contains_key(name) || !self.globals.contains(name) {
            self.assign_var(name, l);
            if self.ref_params.contains(name) && l.contains(&Origin::Secret) && !self.may_return_secret && !self.declassify {
                // The caller sees what is written through a reference parameter.
                self.error(format!(
                    "writes a secret value through reference parameter '{}'; mark the function @secret or @declassify",
                    name
                ));
            }
        } else {
            let pc = Label::new();
            self.sink(&format!("global '{}'", name), l, &pc);
        }
    }

    fn stmt(&mut self, s: &Stmt) {
        self.line = s.line();
        let pc = self.pc();
        match s {
            Stmt::Let { name, expr, annotations, .. } => {
                let mut l = union(&self.expr(expr), &pc);
                if annotations.iter().any(|a| a.name == "secret") {
                    l.insert(Origin::Secret);
                }
                self.assign_var(name, &l);
            }
            Stmt::LetTuple { names, expr, .. } => {
                let l = union(&self.expr(expr), &pc);
                for n in names {
                    self.assign_var(n, &l);
                }
            }
            Stmt::LetUninit { name, annotations, .. } => {
                let mut l = pc.clone();
                if annotations.iter().any(|a| a.name == "secret") {
                    l.insert(Origin::Secret);
                }
                self.assign_var(name, &l);
            }
            Stmt::Assign { target, expr, .. } => {
                let l = union(&self.expr(expr), &pc);
                self.store(target, &l);
            }
            Stmt::IndexAssign { array, index, expr, .. } => {
                let mut l = union(&self.expr(expr), &pc);
                l.extend(self.expr(index));
                self.store(array, &l);
            }
            Stmt::FieldAssign { object, expr, .. } => {
                self.expr(object);
                let l = union(&self.expr(expr), &pc);
                if let Some(base) = Self::base_var(object) {
                    let base = base.to_string();
                    self.store(&base, &l);
                }
            }
            Stmt::DerefAssign { ptr_expr, expr, .. } => {
                self.expr(ptr_expr);
                let l = union(&self.expr(expr), &pc);
                if let Some(base) = Self::base_var(ptr_expr) {
                    let base = base.to_string();
                    self.store(&base, &l);
                }
            }
            Stmt::Expr(e, _) => {
                self.expr(e);
            }
            Stmt::Return(Some(e), _) => {
                let l = union(&self.expr(e), &pc);
                self.ret(&l);
            }
            Stmt::Return(None, _) => {}
            Stmt::Break(Some(e), _) => {
                self.expr(e);
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                let c = self.expr(cond);
                self.push_pc(&c);
                self.stmts(then_block);
                if let Some(b) = else_block {
                    self.stmts(b);
                }
                self.pc.pop();
            }
            Stmt::While { cond, body, .. } => {
                let c = self.expr(cond);
                self.push_pc(&c);
                self.stmts(body);
                self.pc.pop();
            }
            Stmt::For { var, iterator, body, .. } => {
                let it = self.expr(iterator);
                self.assign_var(var, &union(&it, &pc));
                // The number of iterations depends on the iterator.
                self.push_pc(&it);
                self.stmts(body);
                self.pc.pop();
            }
            Stmt::Match { expr, arms, .. } => {
                let sl = self.expr(expr);
                self.arms(arms, &sl);
            }
            Stmt::Loop { body, .. } | Stmt::Defer(body, _) | Stmt::Unsafe { body, .. } | Stmt::Sandbox { body, .. }
            | Stmt::Txn { body, .. } | Stmt::Handle { body, .. } | Stmt::RegionBlock { body, .. }
            | Stmt::SpawnScope { body, .. } | Stmt::Query { body, .. } | Stmt::GradBlock { body, .. } => self.stmts(body),
            Stmt::Parallel { branches, .. } => {
                for b in branches {
                    self.stmts(b);
                }
            }
            Stmt::Select { cases, default, .. } => {
                for c in cases {
                    let l = self.expr(&c.channel);
                    self.assign_var(&c.var_name, &union(&l, &pc));
                    self.stmts(&c.body);
                }
                if let Some(d) = default {
                    self.stmts(d);
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

    // --- Expressions -------------------------------------------------------

    fn exprs(&mut self, es: &[Expr]) -> Vec<Label> {
        es.iter().map(|e| self.expr(e)).collect()
    }

    fn expr(&mut self, e: &Expr) -> Label {
        match e {
            Expr::Ident(n) => self.vars.get(n).cloned().unwrap_or_default(),
            Expr::Binary { left, right, .. } | Expr::StrConcat { left, right } | Expr::Range { start: left, end: right, .. }
            | Expr::Coalesce { expr: left, default: right } | Expr::PipeForward { left, right } => {
                let l = self.expr(left);
                union(&l, &self.expr(right))
            }
            Expr::Index { array, index } => {
                let l = self.expr(array);
                union(&l, &self.expr(index))
            }
            Expr::Unary { expr, .. } | Expr::FieldAccess { expr, .. } | Expr::OptionalFieldAccess { expr, .. }
            | Expr::Ref { expr, .. } | Expr::Deref { expr } | Expr::Try { expr } | Expr::IsCheck { expr, .. }
            | Expr::Cast { expr, .. } | Expr::QuantCast { expr, .. } | Expr::Await { expr } => self.expr(expr),
            Expr::Spawn { expr } => {
                let mut l = self.expr(expr);
                if let Expr::Call { args, .. } = &**expr {
                    for a in args {
                        l.extend(self.expr(a));
                    }
                }
                let pc = self.pc();
                self.sink("another task (spawn)", &l, &pc);
                Label::new()
            }
            Expr::Call { callee, args } => self.call(callee, args),
            Expr::MethodCall { receiver, method, args } => self.method_call(receiver, method, args),
            Expr::StructLit { fields, spread, .. } => {
                let mut l = Label::new();
                for (_, v) in fields {
                    l.extend(self.expr(v));
                }
                if let Some(s) = spread {
                    l.extend(self.expr(s));
                }
                l
            }
            Expr::EnumLit { args, .. } | Expr::TupleLit(args) | Expr::ArrayLit { elements: args, .. } => {
                let ls = self.exprs(args);
                ls.iter().flatten().cloned().collect()
            }
            Expr::If { cond, then_block, else_block } => {
                let c = self.expr(cond);
                self.push_pc(&c);
                let mut l = self.stmts_value(then_block);
                if let Some(b) = else_block {
                    l.extend(self.stmts_value(b));
                }
                self.pc.pop();
                union(&l, &c)
            }
            Expr::Match { expr, arms } => {
                let s = self.expr(expr);
                let l = self.arms(arms, &s);
                union(&l, &s)
            }
            Expr::Block(b) | Expr::TryBlock { body: b } => self.stmts_value(b),
            Expr::Closure { params, body, .. } => {
                // The body runs where the closure is called; check it here with
                // the captured labels, and give the closure those labels.
                let mut captured = Label::new();
                let mut used = HashSet::new();
                crate::escape::idents_in_stmts(body, &mut used);
                for name in &used {
                    if params.iter().any(|p| &p.name == name) {
                        continue;
                    }
                    if let Some(l) = self.vars.get(name) {
                        captured.extend(l.iter().cloned());
                    }
                }
                let saved_returns = self.cur.returns.clone();
                let saved_secret = self.may_return_secret;
                let saved_declassify = self.declassify;
                // A closure's `return` is its own, not the function's.
                self.may_return_secret = true;
                self.declassify = false;
                self.stmts_value(body);
                self.cur.returns = saved_returns;
                self.may_return_secret = saved_secret;
                self.declassify = saved_declassify;
                captured
            }
            Expr::FString { parts } => {
                let mut l = Label::new();
                for p in parts {
                    if let FStringPart::Expr(x) = p {
                        l.extend(self.expr(x));
                    }
                }
                l
            }
            _ => Label::new(),
        }
    }

    fn call(&mut self, callee: &str, args: &[Expr]) -> Label {
        let ls = self.exprs(args);
        let all: Label = ls.iter().flatten().cloned().collect();
        let pc = self.pc();
        if OUTPUT_SINKS.contains(&callee) {
            self.sink(&format!("output '{}'", callee), &all, &pc);
            return Label::new();
        }
        // A function value: it could output its arguments.
        if let Some(fl) = self.vars.get(callee).cloned() {
            self.sink(&format!("a call through the function value '{}'", callee), &all, &pc);
            return union(&fl, &all);
        }
        let Some(f) = self.funcs.get(callee).copied() else {
            // A builtin computes its result from its arguments.
            return all;
        };
        self.reads.insert(callee.to_string());
        let summ = self.summaries.get(callee).cloned().unwrap_or_default();
        self.apply_summary(callee, f, &summ, &ls, &pc, 0)
    }

    /// Apply callee `f`'s summary to argument labels `ls`, where `ls[0]`
    /// corresponds to parameter `offset`.
    fn apply_summary(&mut self, name: &str, f: &Func, summ: &Summary, ls: &[Label], pc: &Label, offset: usize) -> Label {
        for &i in &summ.leaks {
            if i < offset {
                continue;
            }
            if let Some(l) = ls.get(i - offset) {
                let pname = f.params.get(i).map(|p| p.name.clone()).unwrap_or_default();
                if l.contains(&Origin::Secret) {
                    self.error(format!("a secret value is passed to '{}', which outputs its parameter '{}'", name, pname));
                }
                for o in l.clone() {
                    if let Origin::Param(j) = o {
                        self.note_leak(j);
                    }
                }
            }
        }
        if summ.outputs {
            self.note_output();
            if pc.contains(&Origin::Secret) {
                self.error(format!("'{}' produces output and is called inside a branch or loop that depends on a secret", name));
            }
            for o in pc.clone() {
                if let Origin::Param(j) = o {
                    self.note_leak(j);
                }
            }
        }
        let mut out = Label::new();
        for &i in &summ.returns {
            if i >= offset {
                if let Some(l) = ls.get(i - offset) {
                    out.extend(l.iter().cloned());
                }
            }
        }
        if has_annotation(f, "secret") {
            out.insert(Origin::Secret);
        }
        if has_annotation(f, "declassify") {
            out.clear();
        }
        out
    }

    fn method_call(&mut self, receiver: &Expr, method: &str, args: &[Expr]) -> Label {
        let recv = self.expr(receiver);
        let mut ls = vec![recv.clone()];
        ls.extend(self.exprs(args));
        let all: Label = ls.iter().flatten().cloned().collect();
        let pc = self.pc();
        let keys: Vec<String> = self.methods.get(method).cloned().unwrap_or_default();
        let candidates: Vec<(String, &'a Func)> = keys.into_iter().filter_map(|k| self.funcs.get(&k).map(|f| (k.clone(), *f))).collect();
        // A method may store its arguments into its receiver.
        if !READ_ONLY_METHODS.contains(&method) {
            let stored: Label = ls[1..].iter().flatten().chain(pc.iter()).cloned().collect();
            if !stored.is_empty() {
                if let Some(base) = Self::base_var(receiver) {
                    let base = base.to_string();
                    self.store(&base, &stored);
                }
            }
        }
        if candidates.is_empty() {
            if method == "send" {
                self.sink("a channel or socket ('send')", &all, &pc);
            }
            return all;
        }
        let mut out = Label::new();
        for (key, f) in &candidates {
            self.reads.insert(key.clone());
            let summ = self.summaries.get(key).cloned().unwrap_or_default();
            out.extend(self.apply_summary(method, f, &summ, &ls, &pc, 0));
        }
        out
    }
}

/// Run the `@secret` check over every function of `program`.
pub fn check(program: &Program) -> Vec<String> {
    let mut c = SecrecyChecker::new(program);
    let all = c.all.clone();
    // Worklist fixpoint: every function once, then again whenever a summary
    // it read has grown. Summaries only grow, so this terminates, and it
    // reaches the same fixpoint as re-analyzing everything until stable.
    let mut readers: HashMap<String, HashSet<usize>> = HashMap::new();
    let mut queue: std::collections::VecDeque<usize> = (0..all.len()).collect();
    let mut queued = vec![true; all.len()];
    while let Some(i) = queue.pop_front() {
        queued[i] = false;
        let (key, f) = &all[i];
        let before = c.summaries.get(key).cloned();
        c.reads.clear();
        c.analyze_func(key, f);
        for r in c.reads.drain() {
            readers.entry(r).or_default().insert(i);
        }
        if c.summaries.get(key) != before.as_ref() {
            if let Some(rs) = readers.get(key) {
                for &r in rs {
                    if !queued[r] {
                        queued[r] = true;
                        queue.push_back(r);
                    }
                }
            }
        }
    }
    c.report = true;
    for (key, f) in &all {
        c.analyze_func(key, f);
    }
    c.errors
}
