use crate::ast::*;
use std::collections::{HashMap, HashSet};

pub struct TypeChecker {
    scopes: Vec<HashMap<String, Type>>,
    funcs: HashMap<String, (Vec<Type>, Type)>,
    structs: HashMap<String, Vec<(String, Type)>>,
    enums: HashSet<String>,
    consts: HashMap<String, Type>,
    type_aliases: HashMap<String, Type>,
    methods: HashMap<String, HashMap<String, (Vec<Type>, Type, bool, bool)>>,
    /// Current function's type parameters (treated as valid types)
    current_type_params: HashSet<String>,
    /// Current function's type param trait bounds: "T" -> "Printable"
    current_type_param_bounds: HashMap<String, String>,
    /// Trait definitions: trait_name -> {method_name -> (params, ret_ty, has_self)}
    traits: HashMap<String, HashMap<String, (Vec<Type>, Type, bool)>>,
    /// Trait associated type declarations: trait_name -> {assoc_type_name}
    trait_assoc_types: HashMap<String, HashSet<String>>,
    /// Trait impl associated type bindings: (trait_name, type_name) -> {assoc_name -> concrete_type}
    trait_impl_assoc_bindings: HashMap<(String, String), HashMap<String, Type>>,
    /// Track which module each item belongs to ("func_name" → "core")
    item_modules: HashMap<String, String>,
    /// Track visibility of each item ("func_name" → true if pub)
    item_visibility: HashMap<String, bool>,
    /// The module being compiled (user's module)
    pub current_module: String,
    /// Type checking errors
    pub errors: Vec<String>,
    /// Capabilities granted to the current module (e.g. "IO", "FS", "Net")
    granted_caps: HashSet<String>,
    /// Whether strict capability enforcement is active (true if any `with` clauses present)
    strict_caps: bool,
    /// Track which functions produce which effects (func_name → set of effect names)
    func_effects: HashMap<String, HashSet<String>>,
    /// Stack of active region scopes (region name → depth)
    /// When a `region r { ... }` block is entered, `r` is pushed.
    /// When exited, `r` is popped and any references to it become invalid.
    region_stack: Vec<String>,
    /// Map from variable name to the region it was allocated in.
    /// Used to detect use-after-free and dangling pointers.
    var_regions: HashMap<String, String>,
    /// Set of region names that have been freed (popped from region_stack).
    /// Variables still referencing these regions are use-after-free.
    freed_regions: HashSet<String>,
    /// Functions that have been capability-verified at compile time.
    /// Calls to these functions can skip runtime capability checks (elision).
    cap_verified: HashSet<String>,
    /// Name of the function currently being checked (for cap_verified tracking)
    current_func_name: Option<String>,
    /// Current Self type (set when checking impl methods, for bare `self` params)
    current_self_type: Option<String>,
    /// Sandbox depth — when > 0, all capabilities are revoked.
    /// Any IO/FS/Net/Spawn call inside a sandbox block is a compile error.
    sandbox_depth: usize,
    /// Stack of (region_name, budget_bytes, current_usage_bytes) for active
    /// region blocks with budgets. Used to track and enforce memory budgets.
    region_budgets: Vec<(String, Option<u64>, u64)>,
}

impl TypeChecker {
    pub fn new() -> Self {
        TypeChecker {
            scopes: vec![HashMap::new()],
            funcs: HashMap::new(),
            structs: HashMap::new(),
            enums: HashSet::new(),
            consts: HashMap::new(),
            type_aliases: HashMap::new(),
            methods: HashMap::new(),
            current_type_params: HashSet::new(),
            current_type_param_bounds: HashMap::new(),
            traits: HashMap::new(),
            trait_assoc_types: HashMap::new(),
            trait_impl_assoc_bindings: HashMap::new(),
            item_modules: HashMap::new(),
            item_visibility: HashMap::new(),
            current_module: String::new(),
            errors: Vec::new(),
            granted_caps: HashSet::new(),
            strict_caps: false,
            func_effects: HashMap::new(),
            region_stack: Vec::new(),
            var_regions: HashMap::new(),
            freed_regions: HashSet::new(),
            cap_verified: HashSet::new(),
            current_func_name: None,
            current_self_type: None,
            sandbox_depth: 0,
            region_budgets: Vec::new(),
        }
    }

    pub fn check(&mut self, program: &Program) -> Result<(), Vec<String>> {
        // Single-program mode: all items belong to the same module
        let module = program.module.clone().unwrap_or_else(|| "main".to_string());
        self.register_program(program, &module);
        self.check_bodies(program);
        if self.errors.is_empty() {
            Ok(())
        } else {
            Err(self.errors.clone())
        }
    }

    /// Register a program's items under a specific module name.
    /// Called once for stdlib (module="core") and once for user code.
    pub fn register_program(&mut self, program: &Program, module: &str) {
        // The first program registered sets the current module for body checking
        // (user code is registered second, so it overwrites — which is correct)
        self.current_module = module.to_string();

        for s in &program.structs {
            let fields: Vec<(String, Type)> = s.fields.iter().map(|f| (f.name.clone(), f.ty.clone())).collect();
            self.structs.insert(s.name.clone(), fields);
            self.item_modules.insert(s.name.clone(), module.to_string());
            self.item_visibility.insert(s.name.clone(), s.is_pub);
        }

        for e in &program.enums {
            self.enums.insert(e.name.clone());
            self.item_modules.insert(e.name.clone(), module.to_string());
            self.item_visibility.insert(e.name.clone(), e.is_pub);
        }

        for c in &program.consts {
            self.consts.insert(c.name.clone(), c.ty.clone());
            self.item_modules.insert(c.name.clone(), module.to_string());
            self.item_visibility.insert(c.name.clone(), c.is_pub);
        }

        for ta in &program.type_aliases {
            self.type_aliases.insert(ta.name.clone(), ta.alias_for.clone());
            self.item_modules.insert(ta.name.clone(), module.to_string());
            self.item_visibility.insert(ta.name.clone(), ta.is_pub);
        }

        for func in &program.funcs {
            let param_types: Vec<Type> = func.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
            self.funcs.insert(func.name.clone(), (param_types, self.resolve_type(&func.ret_ty)));
            self.item_modules.insert(func.name.clone(), module.to_string());
            self.item_visibility.insert(func.name.clone(), func.is_pub);
        }

        // Register extern function declarations
        for ext in &program.extern_decls {
            let param_types: Vec<Type> = ext.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
            self.funcs.insert(ext.pledge_name.clone(), (param_types, self.resolve_type(&ext.ret_ty)));
            self.item_modules.insert(ext.pledge_name.clone(), module.to_string());
            self.item_visibility.insert(ext.pledge_name.clone(), true);
        }

        for imp in &program.impls {
            let struct_name = imp.struct_name.clone();
            let mut method_map = HashMap::new();
            for method in &imp.methods {
                let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                let has_self = method.params.first().map_or(false, |p| p.name == "self");
                method_map.insert(method.name.clone(), (param_types, self.resolve_type(&method.ret_ty), has_self, method.is_pub));
            }
            // Merge methods — don't overwrite if struct already has methods from another module
            self.methods.entry(struct_name).or_insert(method_map);
        }

        // Register trait impl methods — they're methods on the type
        for ti in &program.trait_impls {
            let type_name = ti.type_name.clone();
            // Compute method info first to avoid borrow conflicts
            let mut new_methods: Vec<(String, Vec<Type>, Type, bool, bool)> = Vec::new();
            for method in &ti.methods {
                let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                let has_self = method.params.first().map_or(false, |p| p.name == "self");
                new_methods.push((method.name.clone(), param_types, self.resolve_type(&method.ret_ty), has_self, method.is_pub));
            }
            let method_map = self.methods.entry(type_name).or_insert_with(HashMap::new);
            for (name, param_types, ret_ty, has_self, is_pub) in new_methods {
                method_map.insert(name, (param_types, ret_ty, has_self, is_pub));
            }
        }

        // Register trait definitions
        for trait_def in &program.traits {
            let mut method_map = HashMap::new();
            for method in &trait_def.methods {
                let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                method_map.insert(method.name.clone(), (param_types, self.resolve_type(&method.ret_ty), method.has_self));
            }
            self.traits.insert(trait_def.name.clone(), method_map);
            // Register associated type declarations
            let assoc_set: HashSet<String> = trait_def.associated_types.iter().map(|(name, _)| name.clone()).collect();
            self.trait_assoc_types.insert(trait_def.name.clone(), assoc_set);
        }
    }

    /// Register trait impl methods from a program (e.g. the merged program
    /// that includes default trait methods filled in by the compiler).
    /// This is called after all modules are registered to ensure default
    /// trait methods are visible to the typechecker.
    pub fn register_trait_impls(&mut self, program: &Program) {
        for ti in &program.trait_impls {
            let type_name = ti.type_name.clone();
            let mut new_methods: Vec<(String, Vec<Type>, Type, bool, bool)> = Vec::new();
            for method in &ti.methods {
                let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                let has_self = method.params.first().map_or(false, |p| p.name == "self");
                new_methods.push((method.name.clone(), param_types, self.resolve_type(&method.ret_ty), has_self, method.is_pub));
            }
            let method_map = self.methods.entry(type_name).or_insert_with(HashMap::new);
            for (name, param_types, ret_ty, has_self, is_pub) in new_methods {
                method_map.insert(name, (param_types, ret_ty, has_self, is_pub));
            }
            // Register associated type bindings from the trait impl
            let key = (ti.trait_name.clone(), ti.type_name.clone());
            let bindings: HashMap<String, Type> = ti.associated_types.iter().cloned().collect();
            if !bindings.is_empty() {
                self.trait_impl_assoc_bindings.insert(key, bindings);
            }
        }
    }

    /// Register comptime-generated functions and structs that were injected
    /// into the merged program but aren't in the original user/stdlib programs.
    pub fn register_generated_decls(&mut self, program: &Program) {
        // Register any structs not already known
        for s in &program.structs {
            if !self.structs.contains_key(&s.name) {
                let fields: Vec<(String, Type)> = s.fields.iter()
                    .map(|f| (f.name.clone(), self.resolve_type(&f.ty)))
                    .collect();
                self.structs.insert(s.name.clone(), fields);
            }
        }
        // Register any functions not already known
        for func in &program.funcs {
            if !self.funcs.contains_key(&func.name) {
                let param_types: Vec<Type> = func.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                let ret_ty = self.resolve_type(&func.ret_ty);
                self.funcs.insert(func.name.clone(), (param_types, ret_ty));
            }
        }
    }

    /// Phase 3B.4: Verify that trait impls match their trait definitions.
    pub fn verify_trait_impls(&mut self, program: &Program) {
        let trait_defs: HashMap<String, &TraitDef> = program.traits.iter()
            .map(|t| (t.name.clone(), t))
            .collect();

        for ti in &program.trait_impls {
            let trait_def = match trait_defs.get(&ti.trait_name) {
                Some(t) => *t,
                None => {
                    self.errors.push(format!(
                        "Type error: trait '{}' is not defined (in impl for {})",
                        ti.trait_name, ti.type_name
                    ));
                    continue;
                }
            };

            let implemented: HashSet<String> = ti.methods.iter()
                .map(|m| m.name.clone())
                .collect();

            for tm in &trait_def.methods {
                if !implemented.contains(&tm.name) && tm.default_body.is_none() {
                    self.errors.push(format!(
                        "Type error: impl {} for {} is missing method '{}'",
                        ti.trait_name, ti.type_name, tm.name
                    ));
                }
            }

            // Check that all declared associated types are provided
            let provided_assoc: HashSet<String> = ti.associated_types.iter()
                .map(|(name, _)| name.clone())
                .collect();
            for (assoc_name, _type_params) in &trait_def.associated_types {
                if !provided_assoc.contains(assoc_name) {
                    self.errors.push(format!(
                        "Type error: impl {} for {} is missing associated type '{}'",
                        ti.trait_name, ti.type_name, assoc_name
                    ));
                }
            }
        }
    }

    /// Check function and method bodies after all items are registered.
    pub fn check_bodies(&mut self, program: &Program) {
        // Pre-pass: compute direct effects of each function (e.g., which functions call print)
        self.compute_func_effects(program);

        // Verify declared effects match inferred effects
        self.verify_declared_effects(program);

        // Verify allocator-as-parameter: functions with !Alloc must take an Allocator param
        self.verify_allocator_params(program);

        // Mark all functions as capability-verified before checking.
        // If any function fails capability checking, it will be removed
        // from the verified set (and an error will be reported).
        for func in &program.funcs {
            self.cap_verified.insert(func.name.clone());
        }
        for imp in &program.impls {
            for method in &imp.methods {
                self.cap_verified.insert(method.name.clone());
            }
        }

        for func in &program.funcs {
            self.check_func(func);
        }

        for imp in &program.impls {
            let prev_type_params = self.current_type_params.clone();
            let prev_self = self.current_self_type.clone();
            if !imp.type_params.is_empty() {
                self.current_type_params = imp.type_params.iter().cloned().collect();
            }
            self.current_self_type = Some(imp.struct_name.clone());
            for method in &imp.methods {
                self.check_func(method);
            }
            self.current_type_params = prev_type_params;
            self.current_self_type = prev_self;
        }

        // Check trait impl method bodies
        for ti in &program.trait_impls {
            let prev_type_params = self.current_type_params.clone();
            let prev_self = self.current_self_type.clone();
            if !ti.type_params.is_empty() {
                self.current_type_params = ti.type_params.iter().cloned().collect();
            }
            self.current_self_type = Some(ti.type_name.clone());
            for method in &ti.methods {
                self.check_func(method);
            }
            self.current_type_params = prev_type_params;
            self.current_self_type = prev_self;
        }
    }

    /// Verify that declared effect annotations match the inferred effects.
    /// If a function declares `!IO` but actually has `!Net` too, that's an error
    /// (under-declared). If it declares `!{}` (pure) but has `!IO`, that's also
    /// an error. Over-declaring (declaring more than needed) is allowed.
    fn verify_declared_effects(&mut self, program: &Program) {
        for func in &program.funcs {
            if !func.has_effect_annotation {
                continue; // No annotation — nothing to verify
            }

            let inferred = match self.func_effects.get(&func.name) {
                Some(e) => e,
                None => continue,
            };

            // Check that all inferred effects are declared (under-declaration is an error)
            for eff in inferred {
                if !func.declared_effects.contains(eff) {
                    self.errors.push(format!(
                        "Effect error: function '{}' has inferred effect '!{}' but only declared '!{}'",
                        func.name, eff, func.declared_effects.join(",")
                    ));
                }
            }
        }
    }

    /// Verify that functions declaring `!Alloc` have an explicit `Allocator` (or `&Allocator`)
    /// parameter. This is the "allocator-as-parameter" rule (like Zig, but verified).
    /// Functions that allocate must receive their allocator explicitly — no ambient allocation.
    fn verify_allocator_params(&mut self, program: &Program) {
        let check_one = |func: &Func| -> Option<String> {
            if !func.has_effect_annotation {
                return None;
            }
            // If the function declares !Alloc (or !{Alloc, ...}), it must have an Allocator param
            if !func.declared_effects.iter().any(|e| e == "Alloc") {
                return None;
            }
            // Look for a parameter of type Allocator or &Allocator
            let has_alloc_param = func.params.iter().any(|p| {
                match &p.ty {
                    Type::Named(name) => name == "Allocator",
                    Type::Ref { inner, .. } => {
                        matches!(inner.as_ref(), Type::Named(name) if name == "Allocator")
                    }
                    _ => false,
                }
            });
            if has_alloc_param {
                None
            } else {
                Some(format!(
                    "Allocator error: function '{}' declares '!Alloc' but has no Allocator parameter. \
                     Functions that allocate must receive an allocator explicitly (like Zig). \
                     Add a parameter of type 'Allocator' or '&Allocator'.",
                    func.name
                ))
            }
        };

        for func in &program.funcs {
            if let Some(err) = check_one(func) {
                self.errors.push(err);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                if let Some(err) = check_one(method) {
                    self.errors.push(err);
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                if let Some(err) = check_one(method) {
                    self.errors.push(err);
                }
            }
        }
    }

    /// Scan all function bodies to determine which functions produce IO effects.
    /// This is a simple direct-scan — it detects `print()` calls in function bodies.
    fn compute_func_effects(&mut self, program: &Program) {
        // First pass: collect direct effects (print calls etc.)
        let mut results: Vec<(String, HashSet<String>)> = Vec::new();

        for func in &program.funcs {
            let mut effects = HashSet::new();
            self.scan_stmts_for_effects(&func.body, &mut effects);
            results.push((func.name.clone(), effects));
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let mut effects = HashSet::new();
                self.scan_stmts_for_effects(&method.body, &mut effects);
                results.push((method.name.clone(), effects));
            }
        }

        // Insert into the map
        for (name, effects) in &results {
            self.func_effects.insert(name.clone(), effects.clone());
        }

        // Fixpoint iteration: propagate transitive effects through call graph
        // (if A calls B and B has IO, then A has IO too)
        let mut changed = true;
        while changed {
            changed = false;
            // Re-scan with the now-populated func_effects map
            for (name, _) in &results {
                let func = program.funcs.iter().find(|f| &f.name == name);
                let body = if let Some(f) = func {
                    &f.body
                } else {
                    // Find in impls
                    let mut found = None;
                    for imp in &program.impls {
                        if let Some(m) = imp.methods.iter().find(|m| &m.name == name) {
                            found = Some(&m.body);
                            break;
                        }
                    }
                    match found {
                        Some(b) => b,
                        None => continue,
                    }
                };
                let mut new_effects = HashSet::new();
                self.scan_stmts_for_effects(body, &mut new_effects);
                if let Some(existing) = self.func_effects.get(name) {
                    if !new_effects.is_subset(existing) {
                        self.func_effects.insert(name.clone(), new_effects);
                        changed = true;
                    }
                }
            }
        }
    }

    fn scan_stmts_for_effects(&self, stmts: &[Stmt], effects: &mut HashSet<String>) {
        for stmt in stmts {
            self.scan_stmt_for_effects(stmt, effects);
        }
    }

    fn scan_stmt_for_effects(&self, stmt: &Stmt, effects: &mut HashSet<String>) {
        match stmt {
            Stmt::Expr(e, _) => self.scan_expr_for_effects(e, effects),
            Stmt::Let { expr, .. } => self.scan_expr_for_effects(expr, effects),
            Stmt::Assign { expr, .. } => self.scan_expr_for_effects(expr, effects),
            Stmt::IndexAssign { index, expr, .. } => {
                self.scan_expr_for_effects(index, effects);
                self.scan_expr_for_effects(expr, effects);
            }
            Stmt::DerefAssign { expr, .. } => self.scan_expr_for_effects(expr, effects),
            Stmt::Return(e, _) => {
                if let Some(e) = e { self.scan_expr_for_effects(e, effects); }
            }
            Stmt::If { cond, then_block, else_block, .. } => {
                self.scan_expr_for_effects(cond, effects);
                self.scan_stmts_for_effects(then_block, effects);
                if let Some(eb) = else_block { self.scan_stmts_for_effects(eb, effects); }
            }
            Stmt::While { cond, body, .. } => {
                self.scan_expr_for_effects(cond, effects);
                self.scan_stmts_for_effects(body, effects);
            }
            Stmt::For { iterator, body, .. } => {
                self.scan_expr_for_effects(iterator, effects);
                self.scan_stmts_for_effects(body, effects);
            }
            Stmt::Loop { body, .. } => self.scan_stmts_for_effects(body, effects),
            Stmt::Break(e, _) => {
                if let Some(e) = e { self.scan_expr_for_effects(e, effects); }
            }
            Stmt::Defer(stmts, _) => self.scan_stmts_for_effects(stmts, effects),
            Stmt::Match { expr, arms, .. } => {
                self.scan_expr_for_effects(expr, effects);
                for arm in arms {
                    self.scan_stmts_for_effects(&arm.body, effects);
                }
            }
            Stmt::Handle { body, handlers, .. } => {
                self.scan_stmts_for_effects(body, effects);
                for handler in handlers {
                    self.scan_stmts_for_effects(&handler.body, effects);
                }
            }
            Stmt::Do { effect, .. } => {
                effects.insert(effect.clone());
            }
            Stmt::RegionBlock { body, .. } => {
                self.scan_stmts_for_effects(body, effects);
            }
            Stmt::Sandbox { body, .. } => {
                self.scan_stmts_for_effects(body, effects);
            }
            _ => {}
        }
    }

    fn scan_expr_for_effects(&self, expr: &Expr, effects: &mut HashSet<String>) {
        match expr {
            Expr::Call { callee, args } => {
                // Direct effect mapping for built-in functions
                match callee.as_str() {
                    "print" | "println" | "print_int" | "print_float" | "print_str"
                    | "p_print_str" | "p_print_int" | "p_println_str" | "p_println_int"
                    | "p_fs_read" | "p_fs_write" => {
                        effects.insert("IO".to_string());
                    }
                    "p_net_connect" | "p_net_send" | "p_net_recv" | "p_http_get" => {
                        effects.insert("Net".to_string());
                    }
                    "p_spawn_process" | "p_spawn_thread" => {
                        effects.insert("Spawn".to_string());
                    }
                    "p_panic" => {
                        effects.insert("Diverge".to_string());
                    }
                    // Allocation functions
                    "p_vec_new" | "p_map_new" | "p_vec_push" | "p_vec_insert"
                    | "p_map_insert" | "p_string_concat" | "p_vec_clone"
                    | "p_map_clone" | "p_string_clone" | "p_vec_new_with"
                    | "p_alloc_new" => {
                        effects.insert("Alloc".to_string());
                    }
                    _ => {}
                }
                // Check if callee is a known function with effects
                if let Some(func_effs) = self.func_effects.get(callee) {
                    for e in func_effs {
                        effects.insert(e.clone());
                    }
                }
                for arg in args {
                    self.scan_expr_for_effects(arg, effects);
                }
            }
            Expr::MethodCall { receiver, args, .. } => {
                self.scan_expr_for_effects(receiver, effects);
                for arg in args {
                    self.scan_expr_for_effects(arg, effects);
                }
            }
            Expr::Binary { left, right, .. } => {
                self.scan_expr_for_effects(left, effects);
                self.scan_expr_for_effects(right, effects);
            }
            Expr::Unary { expr, .. } => self.scan_expr_for_effects(expr, effects),
            Expr::FieldAccess { expr, .. } => self.scan_expr_for_effects(expr, effects),
            Expr::Index { array, index } => {
                self.scan_expr_for_effects(array, effects);
                self.scan_expr_for_effects(index, effects);
            }
            _ => {}
        }
    }

    /// Check if an item is accessible from the current module.
    /// Private items are only accessible from within their own module.
    fn check_visibility(&mut self, name: &str) {
        let item_module = self.item_modules.get(name).cloned();
        if let Some(item_module) = item_module {
            if item_module != self.current_module {
                let is_pub = self.item_visibility.get(name).copied().unwrap_or(false);
                if !is_pub {
                    self.errors.push(format!(
                        "Visibility error: '{}' is private to module '{}' (not accessible from '{}')",
                        name, item_module, self.current_module
                    ));
                }
            }
        }
    }

    /// Collect capabilities from use statements' `with { ... }` clauses.
    /// If any capabilities are declared, strict mode is enabled.
    pub fn collect_capabilities(&mut self, program: &Program) {
        for import in &program.imports {
            if !import.capabilities.is_empty() {
                self.strict_caps = true;
                for cap in &import.capabilities {
                    self.granted_caps.insert(cap.clone());
                }
            }
        }
    }

    /// Estimate the size in bytes of a type for budget checking.
    /// This is a conservative estimate — uses platform defaults.
    fn type_size(&self, ty: &Type) -> u64 {
        match ty {
            Type::I8 | Type::U8 | Type::Bool => 1,
            Type::I16 | Type::U16 => 2,
            Type::I32 | Type::U32 | Type::F32 => 4,
            Type::I64 | Type::U64 | Type::F64 => 8,
            Type::I128 | Type::U128 => 16,
            Type::Str => 16, // ptr + len
            Type::Named(name) => {
                // Check if it's a known struct
                if let Some(fields) = self.structs.get(name) {
                    fields.len() as u64 * 8 // conservative: 8 per field
                } else if name == "String" {
                    16
                } else {
                    8
                }
            }
            Type::Generic { name, args } => {
                match name.as_str() {
                    "Vec" => 24,        // ptr + len + cap
                    "HashMap" => 48,    // conservative
                    "Option" => 16,     // tag + payload
                    "Result" => 16,     // tag + payload
                    _ => 8,
                }
            }
            Type::Ref { .. } => 8, // pointer
            _ => 8, // conservative default
        }
    }

    /// Check if a capability is granted to the current module.
    /// In non-strict mode (no `with` clauses), all capabilities are granted.
    /// Inside a `sandbox { }` block, ALL capabilities are revoked.
    fn check_capability(&mut self, cap: &str, context: &str) {
        if self.sandbox_depth > 0 {
            self.errors.push(format!(
                "Sandbox error: '{}' requires '{}' capability — all capabilities are revoked inside sandbox {{ }}",
                context, cap
            ));
            if let Some(fname) = &self.current_func_name {
                self.cap_verified.remove(fname);
            }
            return;
        }
        if self.strict_caps && !self.granted_caps.contains(cap) {
            self.errors.push(format!(
                "Capability error: '{}' requires '{}' capability (not granted to module '{}')",
                context, cap, self.current_module
            ));
            // Mark the current function as not capability-verified
            if let Some(fname) = &self.current_func_name {
                self.cap_verified.remove(fname);
            }
        }
    }

    /// Returns the set of functions that passed capability verification.
    /// Calls to these functions can skip runtime capability checks.
    pub fn cap_verified_functions(&self) -> &HashSet<String> {
        &self.cap_verified
    }

    /// Find the closest matching name from a set of candidates.
    /// Uses a simple edit distance metric. Returns None if no close match.
    fn suggest_closest<'a>(&self, name: &str, candidates: impl Iterator<Item = &'a String>) -> Option<String> {
        let mut best: Option<(usize, &String)> = None;
        for candidate in candidates {
            let dist = Self::edit_distance(name, candidate);
            // Only suggest if reasonably close (distance <= 3 or name is a prefix)
            if dist <= 3 || candidate.starts_with(name) || name.starts_with(candidate) {
                if best.map_or(true, |(b, _)| dist < b) {
                    best = Some((dist, candidate));
                }
            }
        }
        best.map(|(_, s)| s.clone())
    }

    /// Simple Levenshtein edit distance.
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

    /// Format a "did you mean...?" suggestion suffix for error messages.
    fn suggestion_suffix(&self, name: &str) -> String {
        // Gather all known names (functions, consts, structs, type aliases, enums)
        let builtin_names = vec!["print".to_string(), "assert".to_string(), "assert_eq".to_string()];
        let all_names: Vec<&String> = self.funcs.keys()
            .chain(self.consts.keys())
            .chain(self.structs.keys())
            .chain(self.type_aliases.keys())
            .chain(self.item_modules.keys())
            .chain(builtin_names.iter())
            .collect();
        if let Some(suggestion) = self.suggest_closest(name, all_names.into_iter()) {
            format!(" — did you mean '{}'?", suggestion)
        } else {
            String::new()
        }
    }

    fn push_scope(&mut self) {
        self.scopes.push(HashMap::new());
    }

    fn pop_scope(&mut self) {
        self.scopes.pop();
    }

    fn declare(&mut self, name: &str, ty: Type) {
        self.scopes.last_mut().unwrap().insert(name.to_string(), ty);
    }

    fn lookup(&self, name: &str) -> Option<&Type> {
        self.scopes.iter().rev().find_map(|s| s.get(name))
    }

    fn check_func(&mut self, func: &Func) {
        self.current_func_name = Some(func.name.clone());
        self.push_scope();

        // Track type parameters for this function
        let saved_tp = self.current_type_params.clone();
        self.current_type_params = func.type_params.iter().cloned().collect();

        // Track trait bounds for type parameters
        let saved_bounds = self.current_type_param_bounds.clone();
        self.current_type_param_bounds = func.type_param_bounds.iter().cloned().collect();

        // Declare const generic params as variables (e.g. N: i64 → N is an i64 value)
        for (name, ty_name) in &func.const_params {
            let ty = match ty_name.as_str() {
                "i8" => Type::I8, "i16" => Type::I16, "i32" => Type::I32, "i64" => Type::I64,
                "i128" => Type::I128, "u8" => Type::U8, "u16" => Type::U16, "u32" => Type::U32,
                "u64" => Type::U64, "u128" => Type::U128, _ => Type::I64,
            };
            self.declare(name, ty);
        }

        for param in &func.params {
            let resolved_ty = self.resolve_self_type(&param.ty);
            self.declare(&param.name, resolved_ty);
        }

        for stmt in &func.body {
            self.check_stmt(stmt);
        }

        self.current_type_params = saved_tp;
        self.current_type_param_bounds = saved_bounds;
        self.pop_scope();
        self.current_func_name = None;
    }

    fn check_stmt(&mut self, stmt: &Stmt) {
        match stmt {
            Stmt::Let { name, ty, expr, is_mut: _, is_persist: _, line: _ } => {
                let expr_ty = self.check_expr(expr);
                let final_ty = if let Some(declared) = ty {
                    if !self.types_compatible(declared, &expr_ty) {
                        self.errors.push(format!(
                            "Type error: cannot assign {} to variable of type {}",
                            expr_ty, declared
                        ));
                    }
                    declared.clone()
                } else {
                    expr_ty
                };
                // Track region: if inside a region block, record the allocation region
                if let Some(region) = self.region_stack.last() {
                    self.var_regions.insert(name.clone(), region.clone());
                    // Track allocation size for budget checking
                    let size = self.type_size(&final_ty);
                    if let Some(entry) = self.region_budgets.last_mut() {
                        entry.2 += size;
                    }
                }
                self.declare(name, final_ty);
            }
            Stmt::Expr(expr, _) => {
                self.check_expr(expr);
            }
            Stmt::Return(expr, _) => {
                if let Some(e) = expr {
                    self.check_expr(e);
                }
            }
            Stmt::If {
                cond,
                then_block,
                else_block,
                line: _,
            } => {
                let cond_ty = self.check_expr(cond);
                if cond_ty != Type::Bool && cond_ty != Type::I32 && cond_ty != Type::I64 {
                    self.errors.push(format!(
                        "Type error: if condition must be bool, got {}",
                        cond_ty
                    ));
                }
                self.push_scope();
                for s in then_block {
                    self.check_stmt(s);
                }
                self.pop_scope();
                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
            }
            Stmt::Assign { target, op: _, expr, line: _ } => {
                let _expr_ty = self.check_expr(expr);
                if self.lookup(target).is_none() && !self.consts.contains_key(target) {
                    // Check if it was in a freed region (use-after-free)
                    if let Some(region) = self.var_regions.get(target) {
                        if self.freed_regions.contains(region) {
                            self.errors.push(format!(
                                "Region error: use-after-free — variable '{}' was in region '{}' \
                                 which has been freed",
                                target, region
                            ));
                        } else {
                            self.errors.push(format!(
                                "Type error: cannot assign to undefined variable '{}'",
                                target
                            ));
                        }
                    } else {
                        self.errors.push(format!(
                            "Type error: cannot assign to undefined variable '{}'",
                            target
                        ));
                    }
                }
            }
            Stmt::IndexAssign { array, index, op: _, expr, line: _ } => {
                self.check_expr(index);
                self.check_expr(expr);
                if self.lookup(array).is_none() {
                    self.errors.push(format!(
                        "Type error: cannot index undefined variable '{}'",
                        array
                    ));
                }
                // Compile-time bounds checking for constant indices
                let const_idx = match &index {
                    Expr::IntLit(v) => Some(*v),
                    Expr::Unary { op: UnaryOp::Neg, expr, .. } => {
                        if let Expr::IntLit(v) = expr.as_ref() { Some(-*v) } else { None }
                    }
                    _ => None,
                };
                if let Some(idx_val) = const_idx {
                    if let Some(var_ty) = self.lookup(array) {
                        match &var_ty {
                            Type::Array { size, .. } => {
                                let len = *size as i64;
                                if idx_val < 0 || idx_val >= len {
                                    self.errors.push(format!(
                                        "Compile error: array index {} out of bounds for array of length {}",
                                        idx_val, len
                                    ));
                                }
                            }
                            _ => {}
                        }
                    }
                }
            }
            Stmt::DerefAssign { ptr_expr, op: _, expr, line: _ } => {
                self.check_expr(ptr_expr);
                self.check_expr(expr);
            }
            Stmt::While { cond, body, line: _ } => {
                let cond_ty = self.check_expr(cond);
                if cond_ty != Type::Bool && cond_ty != Type::I32 && cond_ty != Type::I64 {
                    self.errors.push(format!(
                        "Type error: while condition must be bool, got {}",
                        cond_ty
                    ));
                }
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::For { var, iterator, body, line: _ } => {
                let iter_ty = self.check_expr(iterator);
                // Extract element type: Vec<T> → T, [T; N] → T, range → I64
                let elem_ty = match &iter_ty {
                    Type::Generic { name, args } if name == "Vec" => {
                        args.first().cloned().unwrap_or(Type::I64)
                    }
                    Type::Array { elem, .. } => *elem.clone(),
                    _ => iter_ty,
                };
                self.push_scope();
                self.declare(var, elem_ty);
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::Loop { body, line: _ } => {
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::Break(expr, _) => {
                if let Some(e) = expr {
                    self.check_expr(e);
                }
            }
            Stmt::Continue(_) => {}
            Stmt::Defer(body, _) => {
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::Match { expr, arms, line: _ } => {
                let match_ty = self.check_expr(expr);
                for arm in arms {
                    self.push_scope();
                    // Bind pattern variables
                    match &arm.pattern {
                        Pattern::Ident(name) => {
                            self.declare(name, match_ty.clone());
                        }
                        Pattern::EnumVariant { enum_name: _, variant: _, bindings } => {
                            for (bname, _bty) in bindings {
                                self.declare(bname, Type::I32);
                            }
                        }
                        Pattern::StrLit(_) => {}
                        Pattern::Range { .. } => {}
                        _ => {}
                    }
                    // Check guard expression if present
                    if let Some(guard) = &arm.guard {
                        self.check_expr(guard);
                    }
                    for s in &arm.body {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
            }
            Stmt::Use { .. } => {}
            Stmt::Handle { body, handlers, line: _ } => {
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
                for handler in handlers {
                    self.push_scope();
                    for param in &handler.params {
                        self.declare(param, Type::I64);
                    }
                    for s in &handler.body {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
            }
            Stmt::Do { effect: _, op: _, args, line: _ } => {
                for arg in args {
                    self.check_expr(arg);
                }
            }
            Stmt::RegionBlock { name, budget, body, .. } => {
                // Push region scope
                self.region_stack.push(name.clone());
                self.push_scope();
                // Push budget tracker
                self.region_budgets.push((name.clone(), *budget, 0));

                // Snapshot variables before the region block
                let vars_before: HashSet<String> = self
                    .var_regions
                    .keys()
                    .cloned()
                    .collect();

                for s in body {
                    self.check_stmt(s);
                }

                // Detect variables that were allocated in this region
                let region_name = name.clone();
                let escaped: Vec<String> = self
                    .var_regions
                    .iter()
                    .filter(|(k, v)| {
                        v.as_str() == region_name.as_str() && !vars_before.contains(k.as_str())
                    })
                    .map(|(k, _)| k.clone())
                    .collect();

                // Pop scope and region
                self.pop_scope();
                self.region_stack.pop();

                // Mark region as freed — any future access to its variables is use-after-free
                self.freed_regions.insert(region_name.clone());

                // Remove region-allocated variables from tracking
                for var in &escaped {
                    self.var_regions.remove(var);
                }

                // Check if any escaped variables are still referenced in
                // an outer scope (use-after-free / dangling pointer)
                // This is a conservative check: if the variable is still in
                // a scope, it's a potential escape.
                if let Some(outer_scope) = self.scopes.last() {
                    for var in &escaped {
                        if outer_scope.contains_key(var) {
                            self.errors.push(format!(
                                "Region error: variable '{}' allocated in region '{}' \
                                 may escape the region block",
                                var, region_name
                            ));
                        }
                    }
                }

                // Check memory budget
                if let Some((_, budget_opt, usage)) = self.region_budgets.pop() {
                    if let Some(budget) = budget_opt {
                        if usage > budget {
                            self.errors.push(format!(
                                "Budget error: region '{}' used {} bytes but budget is {} bytes ({} bytes over)",
                                region_name, usage, budget, usage - budget
                            ));
                        }
                    }
                }
            }
            Stmt::Sandbox { body, .. } => {
                // Push scope and enter sandbox mode (all capabilities revoked)
                self.push_scope();
                self.sandbox_depth += 1;
                for s in body {
                    self.check_stmt(s);
                }
                self.sandbox_depth -= 1;
                self.pop_scope();
            }
            Stmt::ComptimeGen { .. } => {}
        }
    }

    fn check_expr(&mut self, expr: &Expr) -> Type {
        match expr {
            Expr::IntLit(v) => Self::infer_int_type(*v),
            Expr::FloatLit(_) => Type::F64,
            Expr::StringLit(_) => Type::Str,
            Expr::BoolLit(_) => Type::Bool,
            Expr::Ident(name) => {
                if let Some(ty) = self.lookup(name) {
                    return ty.clone();
                }
                if let Some(ty) = self.consts.get(name).cloned() {
                    self.check_visibility(name);
                    return ty;
                }
                // Check if it's a function name — return function type
                if let Some((params, ret)) = self.funcs.get(name).cloned() {
                    self.check_visibility(name);
                    return Type::Func {
                        params,
                        ret: Box::new(ret),
                    };
                }
                self.errors
                    .push(format!("Type error: undefined variable '{}'{}", name, self.suggestion_suffix(name)));
                Type::Never
            }
            Expr::Binary { op, left, right } => {
                let lt = self.check_expr(left);
                let rt = self.check_expr(right);
                let lt = self.resolve_type(&lt);
                let rt = self.resolve_type(&rt);
                match op {
                    BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div | BinOp::Mod => {
                        // String concatenation with +
                        if op == &BinOp::Add && (lt == Type::Str || rt == Type::Str) {
                            if lt == Type::Str && rt == Type::Str {
                                return Type::Str;
                            }
                        }
                        if self.is_numeric(&lt) && self.is_numeric(&rt) {
                            if self.is_integer(&lt) && self.is_integer(&rt) {
                                // Auto-promote for safety: result is one level wider than the wider operand
                                let wider = Self::wider_type(&lt, &rt);
                                Self::promote_for_arith(&wider)
                            } else if self.is_float(&lt) || self.is_float(&rt) {
                                // Float arithmetic: use wider float
                                if lt == Type::F64 || rt == Type::F64 {
                                    Type::F64
                                } else {
                                    Type::F32
                                }
                            } else {
                                // Same type
                                lt
                            }
                        } else {
                            self.errors.push(format!(
                                "Type error: arithmetic requires numeric types, got {} and {}",
                                lt, rt
                            ));
                            Type::Never
                        }
                    }
                    BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq => {
                        Type::Bool
                    }
                    BinOp::And | BinOp::Or => {
                        if lt != Type::Bool || rt != Type::Bool {
                            self.errors.push(format!(
                                "Type error: logical operators require bool, got {} and {}",
                                lt, rt
                            ));
                        }
                        Type::Bool
                    }
                    BinOp::BitAnd | BinOp::BitOr | BinOp::BitXor | BinOp::Shl | BinOp::Shr => {
                        if self.is_numeric(&lt) && self.is_numeric(&rt) {
                            if self.is_integer(&lt) && self.is_integer(&rt) {
                                // Auto-widen to the wider operand type
                                Self::wider_type(&lt, &rt)
                            } else if lt == rt {
                                lt
                            } else {
                                self.errors.push(format!(
                                    "Type error: cannot mix {} and {} in bitwise operation",
                                    lt, rt
                                ));
                                Type::Never
                            }
                        } else {
                            self.errors.push(format!(
                                "Type error: bitwise operators require integer types, got {} and {}",
                                lt, rt
                            ));
                            Type::Never
                        }
                    }
                }
            }
            Expr::Unary { op, expr } => {
                let ty = self.check_expr(expr);
                match op {
                    UnaryOp::Neg => {
                        if !self.is_numeric(&ty) {
                            self.errors
                                .push(format!("Type error: cannot negate {}", ty));
                        }
                        ty
                    }
                    UnaryOp::Not => {
                        if ty != Type::Bool {
                            self.errors
                                .push(format!("Type error: cannot logically negate {}", ty));
                        }
                        Type::Bool
                    }
                    UnaryOp::BitNot => {
                        if !self.is_numeric(&ty) {
                            self.errors
                                .push(format!("Type error: cannot bitwise negate {}", ty));
                        }
                        ty
                    }
                }
            }
            Expr::Call { callee, args } => {
                // Check capabilities for built-in functions that produce effects
                match callee.as_str() {
                    "print" | "println" | "print_int" | "print_float" | "print_str" => {
                        self.check_capability("IO", &format!("{}()", callee));
                    }
                    "read_file" | "write_file" | "file_open" | "file_read" | "file_write" => {
                        self.check_capability("FS", &format!("{}()", callee));
                    }
                    "http_get" | "http_post" | "socket_connect" | "socket_send" | "socket_recv" => {
                        self.check_capability("Net", &format!("{}()", callee));
                    }
                    "spawn" | "exec" | "system" => {
                        self.check_capability("Spawn", &format!("{}()", callee));
                    }
                    _ => {}
                }
                if callee == "print" {
                    for arg in args {
                        self.check_expr(arg);
                    }
                    return Type::Void;
                }

                let arg_types: Vec<Type> = args.iter().map(|a| self.check_expr(a)).collect();

                // Check if callee is a known function
                if let Some((params, ret)) = self.funcs.get(callee).cloned() {
                    self.check_visibility(callee);
                    // Check if called function has effects requiring capabilities
                    let func_effs = self.func_effects.get(callee).cloned();
                    if let Some(effects) = func_effs {
                        for eff in &effects {
                            self.check_capability(eff, &format!("call to '{}'", callee));
                        }
                    }
                    if params.len() != arg_types.len() {
                        self.errors.push(format!(
                            "Type error: function '{}' expects {} args, got {}",
                            callee,
                            params.len(),
                            arg_types.len()
                        ));
                    }
                    // For generic functions, the return type contains type params
                    // that will be resolved by monomorphization. Return Never as
                    // a sentinel that's compatible with any type.
                    if self.type_contains_param(&ret) {
                        return Type::Never;
                    }
                    return ret;
                }

                // Check if callee is a local variable with function type
                if let Some(var_ty) = self.lookup(callee).cloned() {
                    if let Type::Func { params, ret } = &var_ty {
                        if params.len() != arg_types.len() {
                            self.errors.push(format!(
                                "Type error: function variable '{}' expects {} args, got {}",
                                callee,
                                params.len(),
                                arg_types.len()
                            ));
                        }
                        return (**ret).clone();
                    }
                }

                // Check for runtime builtin functions (p_*)
                if callee.starts_with("p_") {
                    // Builtins are variadic — return I64 as a safe default
                    return Type::I64;
                }

                self.errors
                    .push(format!("Type error: undefined function '{}'{}", callee, self.suggestion_suffix(callee)));
                Type::Never
            }
            Expr::FieldAccess { expr, field } => {
                let obj_ty = self.check_expr(expr);
                if let Type::Named(name) = &obj_ty {
                    if let Some(fields) = self.structs.get(name) {
                        if let Some((_, fty)) = fields.iter().find(|(n, _)| n == field) {
                            return fty.clone();
                        } else {
                            self.errors.push(format!(
                                "Type error: struct '{}' has no field '{}'",
                                name, field
                            ));
                        }
                    } else {
                        self.errors.push(format!(
                            "Type error: '{}' is not a known struct type",
                            name
                        ));
                    }
                } else {
                    self.errors.push(format!(
                        "Type error: cannot access field '{}' on type {}",
                        field, obj_ty
                    ));
                }
                Type::Never
            }
            Expr::StructLit { name, fields } => {
                // Resolve type alias to actual struct name
                let real_name = self.resolve_struct_name(name);
                if let Some(struct_fields) = self.structs.get(&real_name) {
                    let struct_fields = struct_fields.clone();
                    for (fname, fexpr) in fields {
                        let fty = self.check_expr(fexpr);
                        if let Some((_, expected_ty)) = struct_fields.iter().find(|(n, _)| n == fname) {
                            if !self.types_compatible(expected_ty, &fty) {
                                self.errors.push(format!(
                                    "Type error: field '{}' expected {}, got {}",
                                    fname, expected_ty, fty
                                ));
                            }
                        } else {
                            self.errors.push(format!(
                                "Type error: struct '{}' has no field '{}'",
                                name, fname
                            ));
                        }
                    }
                    Type::Named(real_name.clone())
                } else {
                    self.errors.push(format!(
                        "Type error: unknown struct '{}'",
                        name
                    ));
                    Type::Never
                }
            }
            Expr::If {
                cond,
                then_block,
                else_block,
            } => {
                self.check_expr(cond);
                self.push_scope();
                for s in then_block {
                    self.check_stmt(s);
                }
                self.pop_scope();
                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
                Type::Void
            }
            Expr::Range { start, end, .. } => {
                let st = self.check_expr(start);
                let et = self.check_expr(end);
                if !self.is_numeric(&st) || !self.is_numeric(&et) {
                    self.errors.push(format!(
                        "Type error: range bounds must be numeric, got {} and {}",
                        st, et
                    ));
                }
                st
            }
            Expr::Block(stmts) => {
                self.push_scope();
                let mut last_ty = Type::Void;
                for s in stmts {
                    if let Stmt::Expr(e, _) = s {
                        last_ty = self.check_expr(e);
                    } else {
                        self.check_stmt(s);
                        last_ty = Type::Void;
                    }
                }
                self.pop_scope();
                last_ty
            }
            Expr::MethodCall { receiver, method, args } => {
                // Check if this is Box::new(x) — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Box" && method == "new" {
                        let arg_ty = self.check_expr(&args[0]);
                        return Type::Ref { inner: Box::new(arg_ty), is_mut: true };
                    }
                }
                // Check if this is Vec::new() — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Vec" && method == "new" {
                        return Type::Generic { name: "Vec".to_string(), args: vec![Type::I64] };
                    }
                }
                // Check if this is HashMap::new() — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "HashMap" && method == "new" {
                        return Type::Generic { name: "HashMap".to_string(), args: vec![Type::Str, Type::I64] };
                    }
                }
                // Check if this is String::from(x) — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "String" && method == "from" {
                        let _arg_ty = self.check_expr(&args[0]);
                        return Type::Named("String".to_string());
                    }
                }
                // Check if this is an associated function call (e.g. Point::new(x, y))
                if let Expr::Ident(name) = receiver.as_ref() {
                    if self.structs.contains_key(name) {
                        let arg_types: Vec<Type> = args.iter().map(|a| self.check_expr(a)).collect();
                        if let Some(method_map) = self.methods.get(name) {
                            if let Some((params, ret, _has_self, is_pub)) = method_map.get(method) {
                                // Visibility check: if struct is from another module, method must be pub
                                if let Some(struct_mod) = self.item_modules.get(name) {
                                    if struct_mod != &self.current_module && !is_pub {
                                        self.errors.push(format!(
                                            "Visibility error: method '{}::{}' is private to module '{}'",
                                            name, method, struct_mod
                                        ));
                                    }
                                }
                                // Associated function: all params are real args (no self)
                                if params.len() != arg_types.len() {
                                    self.errors.push(format!(
                                        "Type error: associated function '{}::{}' expects {} args, got {}",
                                        name, method, params.len(), arg_types.len()
                                    ));
                                }
                                return ret.clone();
                            }
                        }
                        self.errors.push(format!(
                            "Type error: no associated function '{}' on struct '{}'",
                            method, name
                        ));
                        return Type::Never;
                    }
                }
                let recv_ty = self.check_expr(receiver);
                let arg_types: Vec<Type> = args.iter().map(|a| self.check_expr(a)).collect();

                // Builtin Vec methods — use element type from Generic args
                if let Type::Generic { name, args } = &recv_ty {
                    if name == "Vec" {
                        let elem_ty = args.first().cloned().unwrap_or(Type::I64);
                        return match method.as_str() {
                            "push" => Type::Void,
                            "pop" => elem_ty.clone(),
                            "len" => Type::I64,
                            "is_empty" => Type::Bool,
                            "get" => elem_ty.clone(),
                            "set" => Type::Void,
                            "clear" => Type::Void,
                            "capacity" => Type::I64,
                            "insert" => Type::Void,
                            "remove" => elem_ty.clone(),
                            "contains" => Type::Bool,
                            "reverse" => Type::Void,
                            "first" => elem_ty.clone(),
                            "last" => elem_ty.clone(),
                            "extend" => Type::Void,
                            "truncate" => Type::Void,
                            "sort" => Type::Void,
                            "clone" => Type::Generic { name: "Vec".to_string(), args: vec![elem_ty] },
                            _ => {
                                self.errors.push(format!("Type error: no method '{}' on Vec", method));
                                Type::Never
                            }
                        };
                    }
                    if name == "HashMap" {
                        let key_ty = args.first().cloned().unwrap_or(Type::Str);
                        let val_ty = args.get(1).cloned().unwrap_or(Type::I64);
                        return match method.as_str() {
                            "insert" => Type::Void,
                            "get" => val_ty.clone(),
                            "contains" => Type::Bool,
                            "remove" => Type::Void,
                            "len" => Type::I64,
                            "is_empty" => Type::Bool,
                            "clear" => Type::Void,
                            "keys" => Type::Generic { name: "Vec".to_string(), args: vec![key_ty] },
                            "values" => Type::Generic { name: "Vec".to_string(), args: vec![val_ty.clone()] },
                            "get_or" => val_ty,
                            "clone" => Type::Generic { name: "HashMap".to_string(), args: args.clone() },
                            _ => {
                                self.errors.push(format!("Type error: no method '{}' on HashMap", method));
                                Type::Never
                            }
                        };
                    }
                }

                // Builtin String methods
                if let Type::Named(n) = &recv_ty {
                    if n == "String" {
                        return match method.as_str() {
                            "len" => Type::I64,
                            "is_empty" => Type::Bool,
                            "push_str" => Type::Named("String".to_string()),
                            "to_string" => Type::Named("String".to_string()),
                            "contains" => Type::Bool,
                            "starts_with" => Type::Bool,
                            "ends_with" => Type::Bool,
                            "trim" => Type::Named("String".to_string()),
                            "split" => Type::Generic { name: "Vec".to_string(), args: vec![Type::Named("String".to_string())] },
                            "replace" => Type::Named("String".to_string()),
                            "char_at" => Type::Named("String".to_string()),
                            "to_uppercase" => Type::Named("String".to_string()),
                            "to_lowercase" => Type::Named("String".to_string()),
                            "parse_int" => Type::I64,
                            "find" => Type::I64,
                            "rfind" => Type::I64,
                            "repeat" => Type::Named("String".to_string()),
                            "bytes" => Type::I64,
                            "clone" => Type::Named("String".to_string()),
                            _ => {
                                self.errors.push(format!("Type error: no method '{}' on String", method));
                                Type::Never
                            }
                        };
                    }
                }

                // Check if receiver is a trait object (dyn Trait)
                if let Type::Dyn(trait_name) = &recv_ty {
                    if let Some(trait_methods) = self.traits.get(trait_name) {
                        if let Some((params, ret, has_self)) = trait_methods.get(method) {
                            let expected_args = if *has_self { if params.is_empty() { 0 } else { params.len() - 1 } } else { params.len() };
                            if expected_args != arg_types.len() {
                                self.errors.push(format!(
                                    "Type error: method '{}.{}' expects {} args, got {}",
                                    trait_name, method, expected_args, arg_types.len()
                                ));
                            }
                            return ret.clone();
                        }
                    }
                    self.errors.push(format!(
                        "Type error: no method '{}' on trait '{}'",
                        method, trait_name
                    ));
                    return Type::Never;
                }

                if let Type::Named(struct_name) = &recv_ty {
                    if let Some(method_map) = self.methods.get(struct_name) {
                        if let Some((params, ret, has_self, is_pub)) = method_map.get(method) {
                                // Visibility check
                                if let Some(struct_mod) = self.item_modules.get(struct_name) {
                                    if struct_mod != &self.current_module && !is_pub {
                                        self.errors.push(format!(
                                            "Visibility error: method '{}.{}' is private to module '{}'",
                                            struct_name, method, struct_mod
                                        ));
                                    }
                                }
                                // If has_self, first param is self/receiver, so expected args = params.len() - 1
                            let expected_args = if *has_self { if params.is_empty() { 0 } else { params.len() - 1 } } else { params.len() };
                            if expected_args != arg_types.len() {
                                self.errors.push(format!(
                                    "Type error: method '{}.{}' expects {} args, got {}",
                                    struct_name, method, expected_args, arg_types.len()
                                ));
                            }
                            return ret.clone();
                        }
                    }
                    // Check if this is a type param with a trait bound
                    if self.current_type_params.contains(struct_name) {
                        if let Some(trait_name) = self.current_type_param_bounds.get(struct_name) {
                            if let Some(trait_methods) = self.traits.get(trait_name) {
                                if let Some((params, ret, has_self)) = trait_methods.get(method) {
                                    let expected_args = if *has_self { if params.is_empty() { 0 } else { params.len() - 1 } } else { params.len() };
                                    if expected_args != arg_types.len() {
                                        self.errors.push(format!(
                                            "Type error: method '{}.{}' expects {} args, got {}",
                                            struct_name, method, expected_args, arg_types.len()
                                        ));
                                    }
                                    return ret.clone();
                                }
                            }
                        }
                    }
                    self.errors.push(format!(
                        "Type error: no method '{}' on struct '{}'",
                        method, struct_name
                    ));
                } else {
                    self.errors.push(format!(
                        "Type error: cannot call method '{}' on type {}",
                        method, recv_ty
                    ));
                }
                Type::Never
            }
            Expr::ArrayLit { elem_type, elements } => {
                for e in elements {
                    let et = self.check_expr(e);
                    if !self.types_compatible(elem_type, &et) {
                        self.errors.push(format!(
                            "Type error: array element type mismatch, expected {}, got {}",
                            elem_type, et
                        ));
                    }
                }
                Type::Array { elem: elem_type.clone(), size: elements.len() }
            }
            Expr::Index { array, index } => {
                let arr_ty = self.check_expr(array);
                // String slicing: s[start..end] returns String
                if let Type::Named(ref name) = arr_ty {
                    if name == "String" {
                        // Index can be a range — no numeric check needed
                        return Type::Named("String".to_string());
                    }
                }
                let idx_ty = self.check_expr(index);
                if !self.is_numeric(&idx_ty) {
                    self.errors.push(format!(
                        "Type error: array index must be numeric, got {}",
                        idx_ty
                    ));
                }

                // Compile-time bounds checking: if the index is a constant
                // integer and the array has a known size, verify the index
                // is in bounds. This catches buffer overflows at compile time.
                let const_idx = match index.as_ref() {
                    Expr::IntLit(v) => Some(*v),
                    Expr::Unary { op: UnaryOp::Neg, expr, .. } => {
                        if let Expr::IntLit(v) = expr.as_ref() { Some(-*v) } else { None }
                    }
                    _ => None,
                };
                if let Some(idx_val) = const_idx {
                    match &arr_ty {
                        Type::Array { size, .. } => {
                            let len = *size as i64;
                            if idx_val < 0 || idx_val >= len {
                                self.errors.push(format!(
                                    "Compile error: array index {} out of bounds for array of length {}",
                                    idx_val, len
                                ));
                            }
                        }
                        _ => {}
                    }
                }

                match arr_ty {
                    Type::Array { elem, .. } => *elem.clone(),
                    Type::ArrayConst { elem, .. } => *elem.clone(),
                    Type::Generic { name, args } if name == "Vec" => {
                        args.first().cloned().unwrap_or(Type::I64)
                    }
                    _ => {
                        self.errors.push(format!(
                            "Type error: cannot index type {}",
                            arr_ty
                        ));
                        Type::Never
                    }
                }
            }
            Expr::FString { parts } => {
                for part in parts {
                    if let FStringPart::Expr(e) = part {
                        self.check_expr(e);
                    }
                }
                Type::Str
            }
            Expr::Match { expr, arms } => {
                let match_ty = self.check_expr(expr);
                for arm in arms {
                    self.push_scope();
                    match &arm.pattern {
                        Pattern::Ident(name) => {
                            self.declare(name, match_ty.clone());
                        }
                        Pattern::EnumVariant { enum_name: _, variant: _, bindings } => {
                            for (bname, _bty) in bindings {
                                self.declare(bname, Type::I32);
                            }
                        }
                        Pattern::StrLit(_) => {}
                        Pattern::Range { .. } => {}
                        _ => {}
                    }
                    if let Some(guard) = &arm.guard {
                        self.check_expr(guard);
                    }
                    for s in &arm.body {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
                Type::Void
            }
            Expr::EnumLit { enum_name, variant: _, args } => {
                for a in args {
                    self.check_expr(a);
                }
                Type::Named(enum_name.clone())
            }
            Expr::Ref { expr, is_mut: _ } => {
                let inner_ty = self.check_expr(expr);
                Type::Ref { inner: Box::new(inner_ty), is_mut: false }
            }
            Expr::Deref { expr } => {
                let ty = self.check_expr(expr);
                match ty {
                    Type::Ref { inner, .. } => *inner,
                    _ => {
                        self.errors.push(format!(
                            "Type error: cannot dereference non-reference type {}",
                            ty
                        ));
                        Type::Never
                    }
                }
            }
            Expr::StrConcat { left, right } => {
                let lt = self.check_expr(left);
                let rt = self.check_expr(right);
                if lt != Type::Str && lt != Type::Named("String".to_string()) {
                    self.errors.push(format!(
                        "Type error: string concat requires str, got {}",
                        lt
                    ));
                }
                if rt != Type::Str && rt != Type::Named("String".to_string()) {
                    self.errors.push(format!(
                        "Type error: string concat requires str, got {}",
                        rt
                    ));
                }
                Type::Str
            }
            Expr::Try { expr } => {
                // ? operator: unwrap Result<T, E> -> T, or Option<T> -> T
                let ty = self.check_expr(expr);
                match &ty {
                    Type::Generic { name, args } if name == "Result" && args.len() == 2 => {
                        args[0].clone()
                    }
                    Type::Generic { name, args } if name == "Option" && args.len() == 1 => {
                        args[0].clone()
                    }
                    Type::Named(name) if name == "Result" || name == "Option" => {
                        Type::I64
                    }
                    _ => {
                        self.errors.push(format!(
                            "Type error: ? operator requires Option<T> or Result<T, E>, got {}",
                            ty
                        ));
                        Type::Never
                    }
                }
            }
            Expr::TryBlock { body } => {
                // try { } returns Result<T, E>
                // Check body statements, infer Ok value type from last expression
                let mut last_ty = Type::I64;
                for stmt in body {
                    if let Stmt::Expr(e, _) = stmt {
                        last_ty = self.check_expr(e);
                    } else {
                        self.check_stmt(stmt);
                    }
                }
                // Return Result<last_ty, i64>
                Type::Generic {
                    name: "Result".to_string(),
                    args: vec![last_ty, Type::I64],
                }
            }
            Expr::TupleLit(elems) => {
                let types: Vec<Type> = elems.iter().map(|e| self.check_expr(e)).collect();
                Type::Tuple(types)
            }
            Expr::Closure { params, body, ret_ty } => {
                self.push_scope();
                for p in params {
                    self.declare(&p.name, self.resolve_type(&p.ty));
                }
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
                ret_ty.clone().unwrap_or(Type::Void)
            }
            Expr::Cast { expr, target_ty } => {
                self.check_expr(expr);
                self.resolve_type(target_ty)
            }
            Expr::Asm { template: _, operands } => {
                // Type-check each operand expression
                let mut has_out = false;
                for op in operands {
                    match op.direction {
                        crate::ast::AsmDir::In => {
                            self.check_expr(&op.expr);
                        }
                        crate::ast::AsmDir::Out => {
                            // out operands are lvalues — check the expression
                            self.check_expr(&op.expr);
                            has_out = true;
                        }
                    }
                }
                // asm! returns void if no outputs, i64 if there's an output
                if has_out {
                    Type::I64
                } else {
                    Type::Void
                }
            }
            Expr::Comptime(inner) => self.check_expr(inner),
            Expr::ComptimeBlock(stmts) => {
                for s in stmts {
                    self.check_stmt(s);
                }
                Type::I64
            }
            Expr::ComptimeReflect { op, ty: _ } => {
                use crate::ast::ReflectOp;
                match op {
                    ReflectOp::TypeName => Type::Str,
                    ReflectOp::IsStruct | ReflectOp::IsEnum => Type::Bool,
                    ReflectOp::FieldCount => Type::I64,
                    ReflectOp::FieldNames | ReflectOp::FieldTypes | ReflectOp::EnumVariants => {
                        Type::Array { elem: Box::new(Type::Str), size: 0 }
                    }
                }
            }
        }
    }

    fn resolve_type(&self, ty: &Type) -> Type {
        match ty {
            Type::Named(name) => {
                if let Some(alias) = self.type_aliases.get(name) {
                    self.resolve_type(alias)
                } else {
                    ty.clone()
                }
            }
            Type::ArrayConst { .. } => ty.clone(),
            other => other.clone(),
        }
    }

    /// Resolve a type name (possibly an alias) to its underlying struct name
    fn resolve_struct_name(&self, name: &str) -> String {
        if let Some(alias) = self.type_aliases.get(name) {
            if let Type::Named(inner) = alias {
                return self.resolve_struct_name(inner);
            }
        }
        name.to_string()
    }

    /// Resolve `Self` to the current impl's type name
    fn resolve_self_type(&self, ty: &Type) -> Type {
        match ty {
            Type::Named(name) if name == "Self" => {
                if let Some(self_name) = &self.current_self_type {
                    Type::Named(self_name.clone())
                } else {
                    ty.clone()
                }
            }
            Type::Ref { inner, is_mut } => Type::Ref {
                inner: Box::new(self.resolve_self_type(inner)),
                is_mut: *is_mut,
            },
            Type::Array { elem, size } => Type::Array {
                elem: Box::new(self.resolve_self_type(elem)),
                size: *size,
            },
            Type::ArrayConst { elem, size_param } => Type::ArrayConst {
                elem: Box::new(self.resolve_self_type(elem)),
                size_param: size_param.clone(),
            },
            Type::Generic { name, args } => Type::Generic {
                name: name.clone(),
                args: args.iter().map(|a| self.resolve_self_type(a)).collect(),
            },
            Type::Tuple(types) => Type::Tuple(
                types.iter().map(|t| self.resolve_self_type(t)).collect()
            ),
            other => other.clone(),
        }
    }

    /// Substitute associated type names in a type with `Type::Assoc`.
    fn resolve_assoc_in_type(&self, ty: &Type, type_param: &str, trait_name: &str) -> Type {
        match ty {
            Type::Named(name) => {
                if let Some(assoc_types) = self.trait_assoc_types.get(trait_name) {
                    if assoc_types.contains(name) {
                        return Type::Assoc(type_param.to_string(), trait_name.to_string(), name.clone(), Vec::new());
                    }
                }
                ty.clone()
            }
            Type::Assoc(_, _, name, args) => {
                Type::Assoc(type_param.to_string(), trait_name.to_string(), name.clone(), args.clone())
            }
            _ => ty.clone(),
        }
    }

    fn is_numeric(&self, ty: &Type) -> bool {
        matches!(
            ty,
            Type::I8 | Type::I16 | Type::I32 | Type::I64 | Type::I128
                | Type::U8 | Type::U16 | Type::U32 | Type::U64 | Type::U128
                | Type::F32 | Type::F64
        )
    }

    fn is_integer(&self, ty: &Type) -> bool {
        matches!(
            ty,
            Type::I8 | Type::I16 | Type::I32 | Type::I64 | Type::I128
                | Type::U8 | Type::U16 | Type::U32 | Type::U64 | Type::U128
        )
    }

    fn is_float(&self, ty: &Type) -> bool {
        matches!(ty, Type::F32 | Type::F64)
    }

    /// Infer the smallest signed integer type that can hold `value`.
    /// Falls back to i64 for values that don't fit in i32.
    fn infer_int_type(value: i64) -> Type {
        if value >= -128 && value <= 127 {
            Type::I8
        } else if value >= -32768 && value <= 32767 {
            Type::I16
        } else if value >= -2147483648 && value <= 2147483647 {
            Type::I32
        } else {
            Type::I64
        }
    }

    /// Return the rank of an integer type for comparison (higher = wider).
    fn int_rank(ty: &Type) -> u8 {
        match ty {
            Type::I8 | Type::U8 => 0,
            Type::I16 | Type::U16 => 1,
            Type::I32 | Type::U32 => 2,
            Type::I64 | Type::U64 => 3,
            Type::I128 | Type::U128 => 4,
            _ => 255,
        }
    }

    /// Return the wider of two integer types.
    fn wider_type(a: &Type, b: &Type) -> Type {
        if Self::int_rank(a) >= Self::int_rank(b) {
            a.clone()
        } else {
            b.clone()
        }
    }

    /// Promote an integer type one level wider for arithmetic safety.
    /// i8 -> i16, i16 -> i32, i32 -> i64, i64 -> i64 (stays).
    fn promote_for_arith(ty: &Type) -> Type {
        match ty {
            Type::I8 | Type::U8 => Type::I16,
            Type::I16 | Type::U16 => Type::I32,
            Type::I32 | Type::U32 => Type::I64,
            Type::I64 | Type::U64 => Type::I64,
            Type::I128 | Type::U128 => Type::I128,
            other => other.clone(),
        }
    }

    /// Check if `from` can be implicitly widened to `to`.
    /// Allows i8 -> i16 -> i32 -> i64, and any int -> float.
    fn can_implicitly_widen(from: &Type, to: &Type) -> bool {
        if from == to {
            return true;
        }
        // Any integer can widen to any wider integer
        let int_types = [
            Type::I8, Type::I16, Type::I32, Type::I64, Type::I128,
            Type::U8, Type::U16, Type::U32, Type::U64, Type::U128,
        ];
        let from_idx = int_types.iter().position(|t| t == from);
        let to_idx = int_types.iter().position(|t| t == to);
        if let (Some(fi), Some(ti)) = (from_idx, to_idx) {
            return ti >= fi;
        }
        // Int can widen to float
        if matches!(from, Type::I8 | Type::I16 | Type::I32 | Type::I64 | Type::U8 | Type::U16 | Type::U32 | Type::U64)
            && matches!(to, Type::F32 | Type::F64)
        {
            return true;
        }
        false
    }

    fn types_compatible(&self, declared: &Type, actual: &Type) -> bool {
        let declared = self.resolve_type(declared);
        let actual = self.resolve_type(actual);
        if declared == actual {
            return true;
        }
        if actual == Type::Never {
            return true;
        }
        // Associated types are compatible with anything (resolved during mono)
        if matches!(declared, Type::Assoc(_, _, _, _)) || matches!(actual, Type::Assoc(_, _, _, _)) {
            return true;
        }
        // Trait objects (dyn Trait) are compatible with any concrete type
        // that implements the trait (checked at the cast site)
        if matches!(declared, Type::Dyn(_)) || matches!(actual, Type::Dyn(_)) {
            return true;
        }
        // Trait objects (dyn Trait) are compatible with their concrete types
        // Type parameters are compatible with anything
        if let Type::Named(name) = &declared {
            if self.current_type_params.contains(name) {
                return true;
            }
        }
        if let Type::Named(name) = &actual {
            if self.current_type_params.contains(name) {
                return true;
            }
        }
        // Const generic arrays are compatible with concrete arrays of any size
        if matches!(declared, Type::ArrayConst { .. }) || matches!(actual, Type::ArrayConst { .. }) {
            return true;
        }
        // Allow implicit widening (e.g., i8 literal assigned to i32 variable)
        if Self::can_implicitly_widen(&actual, &declared) {
            return true;
        }
        // Recurse into tuple types for element-wise compatibility
        if let (Type::Tuple(declared_elems), Type::Tuple(actual_elems)) = (&declared, &actual) {
            if declared_elems.len() != actual_elems.len() {
                return false;
            }
            return declared_elems.iter().zip(actual_elems.iter())
                .all(|(d, a)| self.types_compatible(d, a));
        }
        self.is_numeric(&declared) && self.is_numeric(&actual)
    }

    /// Check if a type contains any type parameter (Named type that's not a struct/enum)
    fn type_contains_param(&self, ty: &Type) -> bool {
        match ty {
            Type::Named(name) => {
                !self.structs.contains_key(name) && !self.is_known_enum(name)
            }
            Type::Array { elem, .. } => self.type_contains_param(elem),
            Type::Ref { inner, .. } => self.type_contains_param(inner),
            Type::Generic { args, .. } => args.iter().any(|a| self.type_contains_param(a)),
            Type::Tuple(types) => types.iter().any(|t| self.type_contains_param(t)),
            Type::Func { params, ret } => {
                params.iter().any(|t| self.type_contains_param(t)) || self.type_contains_param(ret)
            }
            _ => false,
        }
    }

    fn is_known_enum(&self, name: &str) -> bool {
        self.enums.contains(name)
    }
}
