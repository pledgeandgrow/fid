//! Borrow checker for Fides — compile-time aliasing analysis.
//!
//! Runs as a separate pass AFTER typechecking succeeds and BEFORE comptime
//! substitution / monomorphization. Catches three classes of bugs at zero
//! runtime cost:
//!
//! 1. Creating `&mut x` while any borrow (`&` or `&mut`) on `x` is active.
//! 2. Creating `&x` while `&mut x` is active.
//! 3. Mutating `x` (assign, index-assign, deref-assign, method call on `x`)
//!    while any borrow on `x` is active.
//!
//! `unsafe { ... }` blocks skip all checks (reuses the same escape hatch as
//! raw pointers).
//!
//! Borrows end lexically: when the borrower goes out of scope or is
//! reassigned. This is correct (never allows a real bug) but occasionally
//! over-restrictive — upgradeable to NLL later by computing last-use on SSA.
//!
//! See `docs/borrowchecker.md` for the full design.

use crate::ast::{Expr, Func, Pattern, Program, Stmt, Type};
use crate::typechecker::TypeChecker;
use std::collections::{HashMap, HashSet};
use std::sync::atomic::{AtomicU8, Ordering};

/// Global borrow-check mode, set once at startup from CLI flags.
/// 0 = Off, 1 = Warn, 2 = On. Default is 2 (On) — borrow checking is always
/// active unless explicitly disabled with `--borrowck=off`.
static BORROWCK_MODE: AtomicU8 = AtomicU8::new(2);

/// Set the global borrow-check mode (called from main.rs during CLI parsing).
pub fn set_mode(mode: BorrowckMode) {
    BORROWCK_MODE.store(mode as u8, Ordering::Relaxed);
}

/// Get the current global borrow-check mode.
pub fn get_mode() -> BorrowckMode {
    match BORROWCK_MODE.load(Ordering::Relaxed) {
        1 => BorrowckMode::Warn,
        2 => BorrowckMode::On,
        _ => BorrowckMode::Off,
    }
}

/// Borrow checker mode, controlled by `--borrowck=off|warn|on`.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BorrowckMode {
    /// Skip borrow checking entirely (current default).
    Off,
    /// Emit warnings but never fail compilation.
    Warn,
    /// Emit errors and fail compilation on violations.
    On,
}

impl BorrowckMode {
    /// Parse from a string (used by CLI flag `--borrowck=...`).
    pub fn parse(s: &str) -> Option<Self> {
        match s.to_ascii_lowercase().as_str() {
            "off" | "0" | "no" => Some(BorrowckMode::Off),
            "warn" | "warning" | "w" => Some(BorrowckMode::Warn),
            "on" | "1" | "yes" | "err" | "error" => Some(BorrowckMode::On),
            _ => None,
        }
    }
}

/// The borrow checker. Holds three maps that together encode the borrow state
/// of every variable in the current function.
pub struct BorrowChecker<'tc> {
    /// Reference to the typechecker — used to read region info and types.
    /// We do NOT mutate it; borrowck is a read-only consumer of typecheck results.
    _tc: &'tc TypeChecker,

    /// var → set of variables currently holding a shared borrow (`&x`) of it.
    shared_borrows: HashMap<String, HashSet<String>>,

    /// var → the single variable currently holding a mutable borrow (`&mut x`)
    /// of it. `None` means no active mutable borrow.
    mut_borrow: HashMap<String, Option<String>>,

    /// borrower → (borrowed_var, is_mut). Used to release a borrow when the
    /// borrower goes out of scope or is reassigned.
    borrow_of: HashMap<String, (String, bool)>,

    /// Stack of scopes; each scope is the set of variable names declared in it.
    /// On scope pop, all borrows held by names in that scope are released.
    scope_stack: Vec<HashSet<String>>,

    /// Current `unsafe { ... }` nesting depth. When > 0, all checks are
    /// skipped (same escape hatch as raw pointers).
    unsafe_depth: usize,

    /// Current function name (for error messages).
    current_func: Option<String>,

    /// Counter for synthetic temporary borrower names (e.g. `__tmp_ref_0`).
    /// Used when a `&x` / `&mut x` is passed directly to a function call
    /// without being assigned to a variable. These borrows are released
    /// when the enclosing `Call` / `MethodCall` completes.
    temp_counter: usize,

    /// Stack of "checkpoints" — each entry records the `temp_counter` value
    /// at the start of a `Call` / `MethodCall`. After the call completes,
    /// all temps with index >= checkpoint are released. Nested calls work
    /// correctly because each call pushes its own checkpoint.
    temp_checkpoints: Vec<usize>,

    /// Phase 4: Two-phase borrow support.
    /// During argument evaluation in a Call/MethodCall, `&mut` borrows are
    /// "reserved" rather than "activated". A reserved borrow doesn't conflict
    /// with shared borrows (`&x`) created in the same argument list. After all
    /// arguments are evaluated, reserved borrows are "activated" (become real
    /// mutable borrows). This allows patterns like `foo(&mut v, &v)` where the
    /// shared borrow is consumed before the mutable borrow takes effect.
    /// Each entry is (path, borrower_name, line).
    reserved_mut_borrows: Vec<(String, String, usize)>,

    /// Phase 4: When true, `&mut` borrows are collected as "reserved" instead
    /// of immediately activated. Set during argument evaluation in Call/MethodCall.
    in_reservation_phase: bool,

    /// Phase 6: NLL (Non-Lexical Lifetimes). Maps each variable name to the
    /// index of the last statement that references it. Borrows are released
    /// when we pass the borrower's last use, not at scope end.
    last_use: HashMap<String, usize>,

    /// Phase 6: Current statement index within the function body. Used to
    /// compare against `last_use` to determine if a borrower is still live.
    stmt_index: usize,

    /// Collected errors (only populated in `On` mode; `Warn` mode uses
    /// `warnings` instead).
    pub errors: Vec<String>,

    /// Collected warnings (populated in `Warn` mode).
    pub warnings: Vec<String>,

    /// Active mode — controls whether violations become errors or warnings.
    mode: BorrowckMode,
}

impl<'tc> BorrowChecker<'tc> {
    pub fn new(tc: &'tc TypeChecker, mode: BorrowckMode) -> Self {
        BorrowChecker {
            _tc: tc,
            shared_borrows: HashMap::new(),
            mut_borrow: HashMap::new(),
            borrow_of: HashMap::new(),
            scope_stack: vec![HashSet::new()],
            unsafe_depth: 0,
            current_func: None,
            temp_counter: 0,
            temp_checkpoints: Vec::new(),
            reserved_mut_borrows: Vec::new(),
            in_reservation_phase: false,
            last_use: HashMap::new(),
            stmt_index: 0,
            errors: Vec::new(),
            warnings: Vec::new(),
            mode,
        }
    }

    /// Check an entire program. Walks every function and impl method body.
    pub fn check(&mut self, program: &Program) {
        for func in &program.funcs {
            self.check_func(func);
        }
        for impl_ in &program.impls {
            for method in &impl_.methods {
                self.check_func(method);
            }
        }
    }

    /// Check a single function. Resets all borrow state between functions.
    fn check_func(&mut self, func: &Func) {
        // Reset state — borrows don't cross function boundaries.
        self.shared_borrows.clear();
        self.mut_borrow.clear();
        self.borrow_of.clear();
        self.scope_stack = vec![HashSet::new()];
        self.unsafe_depth = 0;
        self.current_func = Some(func.name.clone());
        self.temp_counter = 0;
        self.temp_checkpoints.clear();
        self.reserved_mut_borrows.clear();
        self.in_reservation_phase = false;
        self.last_use.clear();
        self.stmt_index = 0;

        // Phase 6: NLL pre-pass. Compute the last statement index that
        // references each variable. This lets us release borrows at the
        // borrower's last use rather than at scope end.
        self.compute_last_use(&func.body);

        // Parameters are in scope at function entry.
        for p in &func.params {
            self.scope_stack.last_mut().unwrap().insert(p.name.clone());
        }

        for (i, stmt) in func.body.iter().enumerate() {
            self.stmt_index = i;
            // Phase 6: Release borrows for borrowers whose last use was
            // before this statement (NLL).
            self.release_dead_borrows();
            self.check_stmt(stmt);
        }

        // Pop the function scope — releases any remaining borrows.
        self.pop_scope();
        self.current_func = None;
    }

    /// Phase 6: Compute the last statement index that references each variable.
    /// This is a simple scan of the function body — for each statement, we find
    /// all variable references and update their last-use index.
    fn compute_last_use(&mut self, body: &[Stmt]) {
        for (i, stmt) in body.iter().enumerate() {
            self.collect_var_uses_stmt(stmt, i);
        }
    }

    /// Recursively collect all variable references in a statement and update
    /// their last-use index.
    fn collect_var_uses_stmt(&mut self, stmt: &Stmt, idx: usize) {
        match stmt {
            Stmt::Let { name, expr, .. } => {
                self.collect_var_uses_expr(expr, idx);
                self.last_use.insert(name.clone(), idx);
            }
            Stmt::LetTuple { names, expr, .. } => {
                self.collect_var_uses_expr(expr, idx);
                for n in names {
                    self.last_use.insert(n.clone(), idx);
                }
            }
            Stmt::LetUninit { name, .. } => {
                self.last_use.insert(name.clone(), idx);
            }
            Stmt::Assign { target, expr, .. } => {
                self.last_use.insert(target.clone(), idx);
                self.collect_var_uses_expr(expr, idx);
            }
            Stmt::IndexAssign { array, index, expr, .. } => {
                self.last_use.insert(array.clone(), idx);
                self.collect_var_uses_expr(index, idx);
                self.collect_var_uses_expr(expr, idx);
            }
            Stmt::DerefAssign { ptr_expr, expr, .. } => {
                self.collect_var_uses_expr(ptr_expr, idx);
                self.collect_var_uses_expr(expr, idx);
            }
            Stmt::Expr(e, _) => self.collect_var_uses_expr(e, idx),
            Stmt::Return(e, _) => {
                if let Some(e) = e { self.collect_var_uses_expr(e, idx); }
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                self.collect_var_uses_expr(cond, idx);
                for s in then_block { self.collect_var_uses_stmt(s, idx); }
                if let Some(eb) = else_block {
                    for s in eb { self.collect_var_uses_stmt(s, idx); }
                }
            }
            Stmt::While { cond, body, .. } => {
                self.collect_var_uses_expr(cond, idx);
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::For { var, iterator, body, .. } => {
                self.collect_var_uses_expr(iterator, idx);
                self.last_use.insert(var.clone(), idx);
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Loop { body, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Break(e, _) => {
                if let Some(e) = e { self.collect_var_uses_expr(e, idx); }
            }
            Stmt::Continue(_) => {}
            Stmt::Defer(body, _) => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Match { expr, arms, .. } => {
                self.collect_var_uses_expr(expr, idx);
                for arm in arms {
                    for name in pattern_bindings(&arm.pattern) {
                        self.last_use.insert(name, idx);
                    }
                    if let Some(g) = &arm.guard { self.collect_var_uses_expr(g, idx); }
                    for s in &arm.body { self.collect_var_uses_stmt(s, idx); }
                }
            }
            Stmt::Use { .. } => {}
            Stmt::Handle { body, handlers, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
                for h in handlers {
                    for p in &h.params { self.last_use.insert(p.clone(), idx); }
                    for s in &h.body { self.collect_var_uses_stmt(s, idx); }
                }
            }
            Stmt::Do { args, .. } => {
                for a in args { self.collect_var_uses_expr(a, idx); }
            }
            Stmt::RegionBlock { body, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Sandbox { body, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Unsafe { body, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Txn { body, .. } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::ComptimeGen { block, .. } => {
                for s in block { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::GradBlock { var, body, .. } => {
                self.last_use.insert(var.clone(), idx);
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Stmt::Backward { loss, .. } => {
                self.collect_var_uses_expr(loss, idx);
            }
        }
    }

    /// Recursively collect all variable references in an expression.
    fn collect_var_uses_expr(&mut self, expr: &Expr, idx: usize) {
        match expr {
            Expr::Ident(name) => {
                self.last_use.insert(name.clone(), idx);
            }
            Expr::Binary { left, right, .. } => {
                self.collect_var_uses_expr(left, idx);
                self.collect_var_uses_expr(right, idx);
            }
            Expr::Unary { expr, .. } => self.collect_var_uses_expr(expr, idx),
            Expr::Call { args, .. } => {
                for a in args { self.collect_var_uses_expr(a, idx); }
            }
            Expr::MethodCall { receiver, args, .. } => {
                self.collect_var_uses_expr(receiver, idx);
                for a in args { self.collect_var_uses_expr(a, idx); }
            }
            Expr::FieldAccess { expr, .. } => self.collect_var_uses_expr(expr, idx),
            Expr::OptionalFieldAccess { expr, .. } => self.collect_var_uses_expr(expr, idx),
            Expr::StructLit { fields, spread, .. } => {
                for (_, e) in fields { self.collect_var_uses_expr(e, idx); }
                if let Some(s) = spread { self.collect_var_uses_expr(s, idx); }
            }
            Expr::If { cond, then_block, else_block } => {
                self.collect_var_uses_expr(cond, idx);
                for s in then_block { self.collect_var_uses_stmt(s, idx); }
                if let Some(eb) = else_block {
                    for s in eb { self.collect_var_uses_stmt(s, idx); }
                }
            }
            Expr::Range { start, end, .. } => {
                self.collect_var_uses_expr(start, idx);
                self.collect_var_uses_expr(end, idx);
            }
            Expr::Block(body) => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Expr::ArrayLit { elements, .. } => {
                for e in elements { self.collect_var_uses_expr(e, idx); }
            }
            Expr::Index { array, index } => {
                self.collect_var_uses_expr(array, idx);
                self.collect_var_uses_expr(index, idx);
            }
            Expr::FString { parts } => {
                for p in parts {
                    if let crate::ast::FStringPart::Expr(e) = p {
                        self.collect_var_uses_expr(e, idx);
                    }
                }
            }
            Expr::Match { expr, arms } => {
                self.collect_var_uses_expr(expr, idx);
                for arm in arms {
                    for name in pattern_bindings(&arm.pattern) {
                        self.last_use.insert(name, idx);
                    }
                    if let Some(g) = &arm.guard { self.collect_var_uses_expr(g, idx); }
                    for s in &arm.body { self.collect_var_uses_stmt(s, idx); }
                }
            }
            Expr::EnumLit { args, .. } => {
                for a in args { self.collect_var_uses_expr(a, idx); }
            }
            Expr::Ref { expr, .. } => self.collect_var_uses_expr(expr, idx),
            Expr::Deref { expr } => self.collect_var_uses_expr(expr, idx),
            Expr::StrConcat { left, right } => {
                self.collect_var_uses_expr(left, idx);
                self.collect_var_uses_expr(right, idx);
            }
            Expr::Try { expr } => self.collect_var_uses_expr(expr, idx),
            Expr::Coalesce { expr, default } => {
                self.collect_var_uses_expr(expr, idx);
                self.collect_var_uses_expr(default, idx);
            }
            Expr::TryBlock { body } => {
                for s in body { self.collect_var_uses_stmt(s, idx); }
            }
            Expr::Await { expr } | Expr::Spawn { expr } => self.collect_var_uses_expr(expr, idx),
            _ => {}
        }
    }

    /// Phase 6: Release borrows for borrowers whose last use was before the
    /// current statement index. This is the core of NLL — borrows end at the
    /// borrower's last use, not at scope end.
    fn release_dead_borrows(&mut self) {
        let current_idx = self.stmt_index;
        let dead: Vec<String> = self
            .borrow_of
            .keys()
            .filter(|borrower| {
                if let Some(&last) = self.last_use.get(*borrower) {
                    last < current_idx
                } else {
                    false
                }
            })
            .cloned()
            .collect();
        for borrower in dead {
            self.release_borrow_by(&borrower);
        }
    }

    // ---------------------------------------------------------------------
    // Scope management
    // ---------------------------------------------------------------------

    fn push_scope(&mut self) {
        self.scope_stack.push(HashSet::new());
    }

    fn pop_scope(&mut self) {
        if let Some(dead) = self.scope_stack.pop() {
            for name in dead {
                self.release_borrow_by(&name);
                // The variable itself is gone — any borrows ON it are stale.
                // (We keep them in the map; they'll just never be matched.
                // Alternatively we could clean up, but it's harmless.)
                self.shared_borrows.remove(&name);
                self.mut_borrow.remove(&name);
            }
        }
    }

    /// Release any borrow held by `borrower`. Called when the borrower goes
    /// out of scope or is reassigned.
    fn release_borrow_by(&mut self, borrower: &str) {
        if let Some((borrowed, _is_mut)) = self.borrow_of.remove(borrower) {
            if let Some(set) = self.shared_borrows.get_mut(&borrowed) {
                set.remove(borrower);
                if set.is_empty() {
                    // leave empty set or remove — leaving is harmless
                }
            }
            if let Some(Some(b)) = self.mut_borrow.get(&borrowed) {
                if b == borrower {
                    self.mut_borrow.insert(borrowed, None);
                }
            }
        }
    }

    /// Declare a variable in the current scope. If it was previously a
    /// borrower (reassignment), release its old borrow first.
    fn declare_var(&mut self, name: &str) {
        // Reassignment: if this name was a borrower, release the old borrow.
        self.release_borrow_by(name);
        self.scope_stack.last_mut().unwrap().insert(name.to_string());
    }

    /// Create a temporary borrow with a synthetic name and return that name.
    /// Used for `&x` / `&mut x` that appear directly inside a call argument
    /// list (not assigned to a variable). The caller is responsible for
    /// releasing the borrow via `release_borrow_by` when the enclosing
    /// expression completes.
    fn create_temp_borrow(&mut self, borrowed_var: &str, is_mut: bool, line: usize) {
        let name = format!("__tmp_ref_{}", self.temp_counter);
        self.temp_counter += 1;
        self.create_borrow(borrowed_var, &name, is_mut, line);
        // Track it in a scope so pop_scope can clean it up if forgotten,
        // but it should be released explicitly by the Call/MethodCall handler.
        self.scope_stack.last_mut().unwrap().insert(name.clone());
    }

    /// Release all temporary borrows created since the checkpoint value was
    /// recorded. Called after a `Call` / `MethodCall` completes — temporary
    /// borrows passed as arguments don't outlive the call.
    fn release_temps_since(&mut self, checkpoint: usize) {
        for i in checkpoint..self.temp_counter {
            let name = format!("__tmp_ref_{}", i);
            self.release_borrow_by(&name);
            // Also remove from the current scope so it doesn't linger.
            if let Some(scope) = self.scope_stack.last_mut() {
                scope.remove(&name);
            }
        }
    }

    /// Phase 4: Release only temp SHARED borrows created since the checkpoint.
    /// Used in the first phase of two-phase borrow processing: shared borrows
    /// from call arguments are consumed before &mut borrows are activated.
    /// Reserved &mut temp borrows are NOT released here — they're still needed
    /// for activation.
    fn release_shared_temps_since(&mut self, checkpoint: usize) {
        for i in checkpoint..self.temp_counter {
            let name = format!("__tmp_ref_{}", i);
            // Only release if this is a shared borrow (not in reserved_mut_borrows).
            let is_reserved_mut = self.reserved_mut_borrows
                .iter()
                .any(|(_, borrower, _)| borrower == &name);
            if !is_reserved_mut {
                self.release_borrow_by(&name);
                if let Some(scope) = self.scope_stack.last_mut() {
                    scope.remove(&name);
                }
            }
        }
    }

    /// Extract a field path from an expression. Returns `Some("s.x.y")` for
    /// `s.x.y`, `Some("s")` for `s`, and `None` for anything else.
    /// This is used to determine what a `&expr` or `&mut expr` borrows.
    fn extract_field_path(&self, expr: &Expr) -> Option<String> {
        match expr {
            Expr::Ident(name) => Some(name.clone()),
            Expr::FieldAccess { expr: inner, field } => {
                // Recurse: s.x.y → "s.x.y"
                self.extract_field_path(inner).map(|p| format!("{}.{}", p, field))
            }
            _ => None,
        }
    }

    // ---------------------------------------------------------------------
    // Borrow state queries
    // ---------------------------------------------------------------------

    fn has_shared_borrow(&self, var: &str) -> bool {
        self.shared_borrows
            .get(var)
            .map(|s| !s.is_empty())
            .unwrap_or(false)
    }

    fn has_mut_borrow(&self, var: &str) -> bool {
        self.mut_borrow
            .get(var)
            .map(|o| o.is_some())
            .unwrap_or(false)
    }

    /// Check if any shared borrow exists on `var` OR any field path under it
    /// (e.g. `s` covers `s.x`, `s.y`, `s.inner.z`). Used when the whole
    /// variable is mutated (e.g. method call `s.method()`).
    fn has_shared_borrow_inclusive(&self, var: &str) -> bool {
        if self.has_shared_borrow(var) {
            return true;
        }
        let prefix = format!("{}.", var);
        for path in self.shared_borrows.keys() {
            if path.starts_with(&prefix) {
                if let Some(set) = self.shared_borrows.get(path) {
                    if !set.is_empty() {
                        return true;
                    }
                }
            }
        }
        false
    }

    /// Check if any mutable borrow exists on `var` OR any field path under it.
    fn has_mut_borrow_inclusive(&self, var: &str) -> bool {
        if self.has_mut_borrow(var) {
            return true;
        }
        let prefix = format!("{}.", var);
        for path in self.mut_borrow.keys() {
            if path.starts_with(&prefix) {
                if let Some(Some(_)) = self.mut_borrow.get(path) {
                    return true;
                }
            }
        }
        false
    }

    /// Check if any shared borrow exists on a PREFIX of `path`.
    /// E.g. path="s.x" checks for borrows on "s" (the parent).
    /// This catches: &s.x conflicts with &mut s (whole-var borrow).
    fn has_shared_borrow_on_prefix(&self, path: &str) -> bool {
        let mut current = path;
        while let Some(dot) = current.rfind('.') {
            current = &current[..dot];
            if self.has_shared_borrow(current) {
                return true;
            }
        }
        false
    }

    /// Check if any mutable borrow exists on a PREFIX of `path`.
    fn has_mut_borrow_on_prefix(&self, path: &str) -> bool {
        let mut current = path;
        while let Some(dot) = current.rfind('.') {
            current = &current[..dot];
            if self.has_mut_borrow(current) {
                return true;
            }
        }
        false
    }

    // ---------------------------------------------------------------------
    // The three rules
    // ---------------------------------------------------------------------

    /// Rule 1 & 2: creating a borrow. `is_mut` true = `&mut x`, false = `&x`.
    /// `borrowed_path` is a field path like `"s"` or `"s.x"` or `"s.inner.z"`.
    fn create_borrow(&mut self, borrowed_path: &str, borrower: &str, is_mut: bool, line: usize) {
        if is_mut {
            // Phase 4: Two-phase borrows. If we're in the reservation phase
            // (evaluating call arguments), don't activate the &mut yet — just
            // reserve it. This allows `foo(&mut v, &v)` because the &v is
            // created while &mut v is only reserved, not active.
            if self.in_reservation_phase {
                // Check for conflicts with EXISTING active borrows (not other
                // reserved ones — two reserved &mut on the same path IS a conflict).
                if self.has_shared_borrow_inclusive(borrowed_path)
                    || self.has_shared_borrow_on_prefix(borrowed_path)
                {
                    self.violation(line, format!(
                        "cannot create &mut {}: shared borrow(s) already active on it or a related path",
                        borrowed_path));
                    return;
                }
                if self.has_mut_borrow_inclusive(borrowed_path)
                    || self.has_mut_borrow_on_prefix(borrowed_path)
                {
                    self.violation(line, format!(
                        "cannot create &mut {}: a mutable borrow is already active on it or a related path",
                        borrowed_path));
                    return;
                }
                // Check for conflicts with other reserved &mut on the same path.
                for (existing_path, _, _) in &self.reserved_mut_borrows {
                    if existing_path == borrowed_path
                        || existing_path.starts_with(&format!("{}.", borrowed_path))
                        || borrowed_path.starts_with(&format!("{}.", existing_path))
                    {
                        self.violation(line, format!(
                            "cannot create &mut {}: a mutable borrow is already reserved on it or a related path",
                            borrowed_path));
                        return;
                    }
                }
                // Reserve the borrow — don't activate it yet.
                self.reserved_mut_borrows.push((
                    borrowed_path.to_string(),
                    borrower.to_string(),
                    line,
                ));
                self.borrow_of
                    .insert(borrower.to_string(), (borrowed_path.to_string(), is_mut));
                return;
            }

            // Rule 1: &mut path requires NO active borrows on:
            // - path itself
            // - any prefix of path (e.g. &mut s conflicts with &s)
            // - any extension of path (e.g. &mut s conflicts with &s.x)
            if self.has_shared_borrow_inclusive(borrowed_path)
                || self.has_shared_borrow_on_prefix(borrowed_path)
            {
                self.violation(line, format!(
                    "cannot create &mut {}: shared borrow(s) already active on it or a related path",
                    borrowed_path));
                return;
            }
            if self.has_mut_borrow_inclusive(borrowed_path)
                || self.has_mut_borrow_on_prefix(borrowed_path)
            {
                self.violation(line, format!(
                    "cannot create &mut {}: a mutable borrow is already active on it or a related path",
                    borrowed_path));
                return;
            }
            self.mut_borrow.insert(borrowed_path.to_string(), Some(borrower.to_string()));
        } else {
            // Rule 2: &path requires no active &mut on:
            // - path itself
            // - any prefix of path (e.g. &s.x conflicts with &mut s)
            // Phase 4: reserved &mut borrows don't conflict with shared borrows.
            // Only ACTIVE &mut borrows block shared borrows.
            if self.has_mut_borrow(borrowed_path)
                || self.has_mut_borrow_on_prefix(borrowed_path)
            {
                self.violation(line, format!(
                    "cannot create &{}: a mutable borrow is already active on it or a parent path",
                    borrowed_path));
                return;
            }
            self.shared_borrows
                .entry(borrowed_path.to_string())
                .or_default()
                .insert(borrower.to_string());
        }
        self.borrow_of.insert(borrower.to_string(), (borrowed_path.to_string(), is_mut));
    }

    /// Phase 4: Activate all reserved &mut borrows. Called after all call
    /// arguments have been evaluated. This converts reserved borrows into
    /// active mutable borrows. At this point, any shared borrows created
    /// during argument evaluation have already been released (as temps).
    fn activate_reserved_borrows(&mut self) {
        let reserved = std::mem::take(&mut self.reserved_mut_borrows);
        for (path, borrower, line) in reserved {
            // Now activate the borrow. Shared borrows from temp args should
            // have been released by release_temps_since. But there could be
            // named shared borrows still active — check for conflicts.
            if self.has_shared_borrow_inclusive(&path)
                || self.has_shared_borrow_on_prefix(&path)
            {
                self.violation(line, format!(
                    "cannot activate &mut {}: shared borrow(s) are still active on it or a related path",
                    path));
                continue;
            }
            self.mut_borrow.insert(path.clone(), Some(borrower.clone()));
        }
    }

    /// Rule 3: mutating a variable. Fails if any borrow is active on it
    /// OR on any field path under it (e.g. mutating `s` conflicts with `&s.x`).
    fn check_mutation(&mut self, var: &str, line: usize) {
        if self.has_shared_borrow_inclusive(var) {
            self.violation(line, format!(
                "cannot mutate {}: shared borrow(s) are active on it or its fields",
                var));
        }
        if self.has_mut_borrow_inclusive(var) {
            self.violation(line, format!(
                "cannot mutate {}: a mutable borrow is active on it or its fields",
                var));
        }
    }

    // ---------------------------------------------------------------------
    // Violation reporting
    // ---------------------------------------------------------------------

    fn violation(&mut self, line: usize, msg: String) {
        let full = match &self.current_func {
            Some(f) => format!("borrow error in {} (line {}): {}", f, line, msg),
            None => format!("borrow error (line {}): {}", line, msg),
        };
        match self.mode {
            BorrowckMode::On => self.errors.push(full),
            BorrowckMode::Warn => self.warnings.push(full),
            BorrowckMode::Off => {}
        }
    }

    // ---------------------------------------------------------------------
    // Statement walker
    // ---------------------------------------------------------------------

    fn check_stmt(&mut self, stmt: &Stmt) {
        match stmt {
            Stmt::Let { name, expr, line, .. } => {
                // Check the initializer first (it may borrow from other vars).
                self.check_expr(expr);
                // Then declare the new variable (releases any old borrow it held).
                self.declare_var(name);
                // If the initializer is itself a borrow, record it.
                self.maybe_record_borrow_from_expr(name, expr, *line);
            }
            Stmt::LetTuple { names, expr, line: _ } => {
                self.check_expr(expr);
                for n in names {
                    self.declare_var(n);
                }
            }
            Stmt::LetUninit { name, .. } => {
                self.declare_var(name);
            }
            Stmt::Assign { target, op, expr, line } => {
                // Mutating `target` — Rule 3. Check if anyone is borrowing target.
                self.check_mutation(target, *line);
                // Compound assignment (+=, etc.) reads target too, but the
                // mutation check above already covers the borrow conflict.
                let _ = op;
                // If target was itself a borrower, release its old borrow BEFORE
                // checking the RHS. This is correct because the assignment overwrites
                // target, so its old borrow is no longer valid. This must happen
                // before check_expr so that `mr = &mut z` doesn't conflict with
                // mr's previous borrow on z.
                self.release_borrow_by(target);
                // The RHS may create new borrows.
                self.check_expr(expr);
                // If the RHS is a Ref expression assigned to target, record the
                // borrow with target as the borrower.
                self.maybe_record_borrow_from_expr(target, expr, *line);
            }
            Stmt::IndexAssign { array, index, op: _, expr, line } => {
                // Mutating `array` — Rule 3.
                self.check_mutation(array, *line);
                self.check_expr(index);
                self.check_expr(expr);
            }
            Stmt::DerefAssign { ptr_expr, op: _, expr, line } => {
                // Writing through a pointer. We can't easily know which var
                // the pointer borrows without tracking it, so we only check
                // the RHS for nested borrows. The deref itself is safe to
                // allow (the borrow was already validated when created).
                self.check_expr(ptr_expr);
                self.check_expr(expr);
                let _ = line;
            }
            Stmt::Expr(e, _) => self.check_expr(e),
            Stmt::Return(e, _) => {
                if let Some(e) = e { self.check_expr(e); }
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                self.check_expr(cond);
                // Each branch gets its own scope; borrows inside don't escape.
                self.push_scope();
                for s in then_block { self.check_stmt(s); }
                self.pop_scope();
                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b { self.check_stmt(s); }
                    self.pop_scope();
                }
            }
            Stmt::While { cond, body, .. } => {
                self.check_expr(cond);
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::For { var, iterator, body, .. } => {
                self.check_expr(iterator);
                self.push_scope();
                self.declare_var(var);
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Loop { body, .. } => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Break(e, _) => {
                if let Some(e) = e { self.check_expr(e); }
            }
            Stmt::Continue(_) => {}
            Stmt::Defer(body, _) => {
                // Defer body runs at function exit; check it in a fresh scope.
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Match { expr, arms, .. } => {
                self.check_expr(expr);
                for arm in arms {
                    self.push_scope();
                    // Bind pattern variables.
                    for name in pattern_bindings(&arm.pattern) {
                        self.declare_var(&name);
                    }
                    if let Some(guard) = &arm.guard {
                        self.check_expr(guard);
                    }
                    for s in &arm.body { self.check_stmt(s); }
                    self.pop_scope();
                }
            }
            Stmt::Use { .. } => {}
            Stmt::Handle { body, handlers, .. } => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
                for h in handlers {
                    self.push_scope();
                    for p in &h.params { self.declare_var(p); }
                    for s in &h.body { self.check_stmt(s); }
                    self.pop_scope();
                }
            }
            Stmt::Do { args, .. } => {
                for a in args { self.check_expr(a); }
            }
            Stmt::RegionBlock { body, .. } => {
                // Region blocks create a new scope. Borrows inside don't
                // escape (the typechecker already enforces this via
                // freed_regions; we just track scope here).
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Sandbox { body, .. } => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Unsafe { body, .. } => {
                // Skip all borrow checks inside unsafe blocks.
                self.unsafe_depth += 1;
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
                self.unsafe_depth -= 1;
            }
            Stmt::Txn { body, .. } => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::ComptimeGen { block, .. } => {
                self.push_scope();
                for s in block { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::GradBlock { var, body, .. } => {
                self.push_scope();
                self.declare_var(var);
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Stmt::Backward { loss, .. } => {
                self.check_expr(loss);
            }
        }
    }

    // ---------------------------------------------------------------------
    // Expression walker
    // ---------------------------------------------------------------------

    fn check_expr(&mut self, expr: &Expr) {
        match expr {
            Expr::IntLit(_)
            | Expr::FloatLit(_)
            | Expr::StringLit(_)
            | Expr::BoolLit(_) => {}
            Expr::Ident(_) => {}
            Expr::Binary { left, right, .. } => {
                self.check_expr(left);
                self.check_expr(right);
            }
            Expr::Unary { expr, .. } => self.check_expr(expr),
            Expr::Call { args, .. } => {
                // Phase 4: Two-phase borrows. Evaluate all arguments in
                // "reservation" mode — &mut borrows are reserved, not activated.
                // This allows `foo(&mut v, &v)` because the &v is created while
                // &mut v is only reserved. After all args are evaluated:
                // 1. Release temp shared borrows (they've been consumed by the call)
                // 2. Activate reserved &mut borrows (no shared borrows conflict now)
                // 3. Release the now-activated &mut temp borrows (they don't outlive the call)
                let checkpoint = self.temp_counter;
                let prev_reservation = self.in_reservation_phase;
                self.in_reservation_phase = true;
                for a in args {
                    self.check_expr(a);
                }
                self.in_reservation_phase = prev_reservation;
                // 1. Release temp shared borrows only (reserved &mut temps stay).
                self.release_shared_temps_since(checkpoint);
                // 2. Activate reserved &mut borrows (no shared borrows to conflict).
                self.activate_reserved_borrows();
                // 3. Release ALL temp borrows (including now-activated &mut ones).
                self.release_temps_since(checkpoint);
            }
            Expr::FieldAccess { expr, .. } => self.check_expr(expr),
            Expr::OptionalFieldAccess { expr, .. } => self.check_expr(expr),
            Expr::StructLit { fields, spread, .. } => {
                for (_, e) in fields { self.check_expr(e); }
                if let Some(s) = spread { self.check_expr(s); }
            }
            Expr::If { cond, then_block, else_block } => {
                self.check_expr(cond);
                self.push_scope();
                for s in then_block { self.check_stmt(s); }
                self.pop_scope();
                if let Some(eb) = else_block {
                    self.push_scope();
                    for s in eb { self.check_stmt(s); }
                    self.pop_scope();
                }
            }
            Expr::Range { start, end, .. } => {
                self.check_expr(start);
                self.check_expr(end);
            }
            Expr::Block(body) => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Expr::MethodCall { receiver, args, .. } => {
                // A method call on `receiver` may mutate it (e.g. arr.push()).
                // If receiver is a simple Ident, treat it as a mutation.
                // Phase 4: The receiver mutation check is deferred to after
                // argument evaluation (reservation phase), so that shared
                // borrows in args don't conflict with the receiver mutation.
                let receiver_path = if let Expr::Ident(name) = receiver.as_ref() {
                    Some(name.clone())
                } else {
                    None
                };
                // Phase 4: Evaluate receiver + args in reservation mode.
                let checkpoint = self.temp_counter;
                let prev_reservation = self.in_reservation_phase;
                self.in_reservation_phase = true;
                self.check_expr(receiver);
                for a in args {
                    self.check_expr(a);
                }
                self.in_reservation_phase = prev_reservation;
                // 1. Release temp shared borrows only (reserved &mut temps stay).
                self.release_shared_temps_since(checkpoint);
                // 2. Activate reserved &mut borrows.
                self.activate_reserved_borrows();
                // 3. Release ALL temp borrows (including now-activated &mut ones).
                self.release_temps_since(checkpoint);
                // Now check the receiver mutation (after args are consumed).
                if let Some(name) = receiver_path {
                    self.check_mutation(&name, 0);
                }
            }
            Expr::ArrayLit { elements, .. } => {
                for e in elements { self.check_expr(e); }
            }
            Expr::Index { array, index } => {
                self.check_expr(array);
                self.check_expr(index);
            }
            Expr::FString { parts } => {
                for p in parts {
                    if let crate::ast::FStringPart::Expr(e) = p {
                        self.check_expr(e);
                    }
                }
            }
            Expr::Match { expr, arms } => {
                self.check_expr(expr);
                for arm in arms {
                    self.push_scope();
                    for name in pattern_bindings(&arm.pattern) {
                        self.declare_var(&name);
                    }
                    if let Some(g) = &arm.guard { self.check_expr(g); }
                    // Match arm bodies are statements in the arm.
                    for s in &arm.body { self.check_stmt(s); }
                    self.pop_scope();
                }
            }
            Expr::EnumLit { args, .. } => {
                for a in args { self.check_expr(a); }
            }
            Expr::Ref { expr, is_mut } => {
                // Creating a reference. The borrowed variable is the inner
                // expression — either a simple Ident (`&x`) or a field path
                // (`&s.x.y`). Field paths allow simultaneous borrows of
                // different fields (e.g. `&s.x` and `&s.y` coexist).
                self.check_expr(expr);
                if self.unsafe_depth == 0 {
                    if let Some(path) = self.extract_field_path(expr) {
                        // If this ref is assigned to a variable (e.g.
                        // `pv r = &x`), the Let handler calls
                        // `maybe_record_borrow_from_expr` which records the
                        // borrow with the variable name as borrower. But we
                        // ALSO need to record it here in case the ref is NOT
                        // assigned (e.g. passed directly to a function call).
                        // The temp borrow is released by the enclosing
                        // Call/MethodCall via the checkpoint mechanism. If
                        // there's no enclosing call, the temp lives until
                        // scope pop (harmless — it just prevents conflicting
                        // borrows for the rest of the scope, which is correct).
                        self.create_temp_borrow(&path, *is_mut, 0);
                    }
                }
            }
            Expr::Deref { expr } => self.check_expr(expr),
            Expr::StrConcat { left, right } => {
                self.check_expr(left);
                self.check_expr(right);
            }
            Expr::Try { expr } => self.check_expr(expr),
            Expr::Coalesce { expr, default } => {
                self.check_expr(expr);
                self.check_expr(default);
            }
            Expr::TryBlock { body } => {
                self.push_scope();
                for s in body { self.check_stmt(s); }
                self.pop_scope();
            }
            Expr::Await { expr } | Expr::Spawn { expr } => self.check_expr(expr),
            // Other expression variants — recurse into children if any.
            other => {
                // Fallback: nothing to do for leaf expressions we haven't
                // matched. This keeps the checker forward-compatible with
                // new Expr variants (they just won't be borrow-checked until
                // explicitly handled).
                let _ = other;
            }
        }
    }

    /// If `expr` is a `Ref` expression, record the borrow with `name` as the
    /// borrower. Called from `Stmt::Let` after the variable is declared.
    /// This "promotes" the temporary borrow created by `check_expr` into a
    /// named borrow tied to the variable. The temp is released first to
    /// avoid a double-borrow (which would cause a false positive for `&mut`).
    fn maybe_record_borrow_from_expr(&mut self, name: &str, expr: &Expr, line: usize) {
        if let Expr::Ref { expr: inner, is_mut } = expr {
            if self.unsafe_depth == 0 {
                if let Some(path) = self.extract_field_path(inner) {
                    // Release the temp borrow that check_expr created for this
                    // Ref. It's the most recent temp (highest index).
                    if self.temp_counter > 0 {
                        let last_temp = format!("__tmp_ref_{}", self.temp_counter - 1);
                        self.release_borrow_by(&last_temp);
                        if let Some(scope) = self.scope_stack.last_mut() {
                            scope.remove(&last_temp);
                        }
                        self.temp_counter -= 1;
                    }
                    // Now create the named borrow.
                    self.create_borrow(&path, name, *is_mut, line);
                }
            }
        }
    }
}

// Type inspection helpers (currently unused but kept for Phase 3 field-wise
// borrows, which will need to know struct field types).
#[allow(dead_code)]
fn is_ref_type(ty: &Type) -> bool {
    matches!(ty, Type::Ref { .. })
}

/// Extract variable names bound by a match pattern.
fn pattern_bindings(p: &Pattern) -> Vec<String> {
    match p {
        Pattern::Ident(name) => vec![name.clone()],
        Pattern::EnumVariant { bindings, .. } => {
            bindings.iter().map(|(name, _)| name.clone()).collect()
        }
        _ => Vec::new(),
    }
}
