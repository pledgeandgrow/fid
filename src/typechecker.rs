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
    /// Field-level visibility: (struct_name, field_name) → is_pub
    /// Private fields are only accessible from within their defining module.
    field_visibility: HashMap<(String, String), bool>,
    /// The module where each struct was defined (struct_name → module_name)
    struct_modules: HashMap<String, String>,
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
    /// Map from region name to the source line where it was declared.
    /// Used to produce better error messages (show where the region block is).
    region_lines: HashMap<String, usize>,
    /// Map from variable name to the source line where it was allocated.
    /// Used to produce better error messages (show where the variable was created).
    var_alloc_lines: HashMap<String, usize>,
    /// Set of struct names that are @linear (values can be moved but not copied).
    linear_structs: HashSet<String>,
    /// Set of variable names that have been consumed (moved) in the current scope.
    /// A linear variable can only be used once — after it's moved, it's in this set.
    consumed_vars: HashSet<String>,
    /// Track linear variables declared in each scope, so we can check they're consumed.
    /// Maps scope depth → set of (var_name, line) pairs that are linear.
    scope_linear_vars: Vec<Vec<(String, usize)>>,
    /// Current loop nesting depth — when > 0, linear variables cannot be consumed.
    /// This prevents double-consumption across loop iterations.
    loop_depth: usize,
    /// Current unsafe block nesting depth — when > 0, raw pointer operations
    /// (dereference, arithmetic) are permitted.
    unsafe_depth: usize,
    /// Set of variables declared with `pm` that have not yet been assigned.
    /// Reading a variable in this set is a compile error (F63: init guarantee).
    uninit_vars: HashSet<String>,
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
            field_visibility: HashMap::new(),
            struct_modules: HashMap::new(),
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
            region_lines: HashMap::new(),
            var_alloc_lines: HashMap::new(),
            linear_structs: HashSet::new(),
            consumed_vars: HashSet::new(),
            scope_linear_vars: vec![Vec::new()],
            loop_depth: 0,
            unsafe_depth: 0,
            uninit_vars: HashSet::new(),
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
            self.struct_modules.insert(s.name.clone(), module.to_string());
            // Register field-level visibility
            for f in &s.fields {
                self.field_visibility.insert((s.name.clone(), f.name.clone()), f.is_pub);
            }
            // Check for @linear annotation
            if s.annotations.iter().any(|a| a.name == "linear") {
                self.linear_structs.insert(s.name.clone());
            }
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
                // Track which module this method was defined in (for field privacy)
                self.item_modules.insert(method.name.clone(), module.to_string());
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
                // Track which module this method was defined in (for field privacy)
                self.item_modules.insert(method.name.clone(), module.to_string());
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
        // Register any impl methods not already known (from desugared layer/model)
        // Compute all method info first to avoid borrow conflicts
        let mut new_methods: Vec<(String, HashMap<String, (Vec<Type>, Type, bool, bool)>)> = Vec::new();
        let mut merged_methods: Vec<(String, HashMap<String, (Vec<Type>, Type, bool, bool)>)> = Vec::new();
        for imp in &program.impls {
            if !self.methods.contains_key(&imp.struct_name) {
                let mut method_map = HashMap::new();
                for method in &imp.methods {
                    let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                    let has_self = method.params.first().map_or(false, |p| p.name == "self");
                    method_map.insert(method.name.clone(), (param_types, self.resolve_type(&method.ret_ty), has_self, method.is_pub));
                }
                new_methods.push((imp.struct_name.clone(), method_map));
            } else {
                let existing = self.methods.get(&imp.struct_name).unwrap();
                let mut to_add = HashMap::new();
                for method in &imp.methods {
                    if !existing.contains_key(&method.name) {
                        let param_types: Vec<Type> = method.params.iter().map(|p| self.resolve_type(&p.ty)).collect();
                        let has_self = method.params.first().map_or(false, |p| p.name == "self");
                        to_add.insert(method.name.clone(), (param_types, self.resolve_type(&method.ret_ty), has_self, method.is_pub));
                    }
                }
                if !to_add.is_empty() {
                    merged_methods.push((imp.struct_name.clone(), to_add));
                }
            }
        }
        for (name, methods) in new_methods {
            self.methods.insert(name, methods);
        }
        for (name, methods) in merged_methods {
            if let Some(existing) = self.methods.get_mut(&name) {
                for (m_name, m_info) in methods {
                    existing.insert(m_name, m_info);
                }
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

        // Verify @deterministic annotation constraints
        self.verify_deterministic(program);

        // Verify @conserve annotation constraints
        self.verify_conserve(program);

        // Verify @gas annotation constraints
        self.verify_gas(program);

        // Verify @crashfree annotation constraints
        self.verify_crashfree(program);

        // Verify safety primitives and domain presets (F48/F44/F52/F56)
        // @medical, @safety-critical, @autosar, @dal-a are all aliases that
        // expand to composable primitives: @no-alloc, @no-recursion, etc.
        self.verify_safety_annotations(program);

        // Verify given/ensure contracts (F10: Design by Contract)
        self.verify_contracts(program);

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
                    // Build the correct effect annotation for the fix suggestion
                    let mut correct_effects: Vec<String> = func.declared_effects.clone();
                    if !correct_effects.contains(eff) {
                        correct_effects.push(eff.clone());
                    }
                    let correct_annotation = if correct_effects.is_empty() {
                        "!{}".to_string()
                    } else if correct_effects.len() == 1 {
                        format!("!{}", correct_effects[0])
                    } else {
                        format!("!{{{}}}", correct_effects.join(", "))
                    };
                    self.errors.push(format!(
                        "Effect error: function '{}' has inferred effect '!{}' but only declared '!{}'.\n\
                         \x20\x20\x20\x20The function performs an operation that produces the '{}' effect,\n\
                         \x20\x20\x20\x20but the effect annotation doesn't include it.\n\
                         \x20\x20\x20\x20Fix: change the annotation to `pf {}() {}` (add '{}' to the effect set).",
                        func.name, eff, func.declared_effects.join(","),
                        eff,
                        func.name, correct_annotation, eff
                    ));
                }
            }
        }
    }

    /// Verify @deterministic annotation constraints.
    ///
    /// A @deterministic function must:
    ///   - Have no IO, Net, Spawn, or Diverge effects (use the effect system)
    ///   - Contain no floating-point operations (F32/F64 literals, variables, or arithmetic)
    ///   - Not call any function that is not itself @deterministic (transitive)
    ///
    /// Alloc and Read effects are allowed (allocation and pure memory reads are deterministic).
    /// Write effect is allowed (writing to a local mutable variable is deterministic).
    ///
    /// The goal: same inputs → same outputs, always. No wall clock, no network, no randomness,
    /// no floating-point nondeterminism (e.g., different FP results on different CPUs).
    fn verify_deterministic(&mut self, program: &Program) {
        let check_one = |func: &Func| -> Vec<String> {
            let is_det = func.annotations.iter().any(|a| a.name == "deterministic");
            if !is_det { return Vec::new(); }

            let mut errs = Vec::new();

            // Check effects — reject IO, Net, Spawn, Diverge
            if let Some(effects) = self.func_effects.get(&func.name) {
                for eff in effects {
                    match eff.as_str() {
                        "IO" | "Net" | "Spawn" | "Diverge" => {
                            errs.push(format!(
                                "Determinism error: function '{}' is @deterministic but has effect '!{}'.\n\
                                 \x20\x20\x20\x20@deterministic functions must be reproducible — same inputs → same outputs.\n\
                                 \x20\x20\x20\x20Effect '!{}' introduces nondeterminism (IO/Net depend on external state,\n\
                                 \x20\x20\x20\x20Spawn introduces concurrency, Diverge introduces control flow uncertainty).\n\
                                 \x20\x20\x20\x20Fix: remove the @deterministic annotation, or remove the operation that\n\
                                 \x20\x20\x20\x20produces the '!{}' effect.",
                                func.name, eff, eff, eff
                            ));
                        }
                        _ => {} // Alloc, Read, Write are allowed
                    }
                }
            }

            // Check for floating-point operations — reject F32/F64
            let mut fp_found = scan_for_floats(&func.body);
            // Also check function parameters for float types
            for param in &func.params {
                if type_contains_float(&param.ty) {
                    fp_found.push(format!("parameter '{}: {}'", param.name, param.ty));
                }
            }
            // Check return type
            if type_contains_float(&func.ret_ty) {
                fp_found.push(format!("return type '{}'", func.ret_ty));
            }
            if !fp_found.is_empty() {
                errs.push(format!(
                    "Determinism error: function '{}' is @deterministic but uses floating-point.\n\
                     \x20\x20\x20\x20Floating-point results vary across CPUs (different rounding, FMA contraction, etc.).\n\
                     \x20\x20\x20\x20Found float in: {}.\n\
                     \x20\x20\x20\x20Fix: use integer arithmetic (e.g., fixed-point with i64) instead of f32/f64,\n\
                     \x20\x20\x20\x20or remove the @deterministic annotation.",
                    func.name, fp_found.join(", ")
                ));
            }

            errs
        };

        for func in &program.funcs {
            for err in check_one(func) {
                self.errors.push(err);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                for err in check_one(method) {
                    self.errors.push(err);
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                for err in check_one(method) {
                    self.errors.push(err);
                }
            }
        }
    }

    /// Verify @conserve annotation constraints.
    ///
    /// A @conserve function guarantees that no Money is created, destroyed, or
    /// duplicated. This is enforced by:
    ///   1. No IO/Net/Spawn/Diverge effects (same as @deterministic for these)
    ///   2. No floating-point (same as @deterministic)
    ///   3. Cannot call functions that are NOT @conserve (transitive — prevents
    ///      calling a helper that might mint money)
    ///   4. Cannot call Money::new or Money::from_cents (the mint functions)
    ///   5. All linear (Money) parameters must be consumed (enforced by linear type system)
    ///
    /// The conservation proof is a consequence of rules 3-5 + linear types:
    /// - Linear types prevent duplication (can't copy Money)
    /// - No mint (rule 4) prevents creation
    /// - Must consume (rule 5, linear) prevents destruction
    /// - Only call @conserve funcs (rule 3) ensures transitive conservation
    fn verify_conserve(&mut self, program: &Program) {
        // Collect all @conserve function names
        let mut conserve_funcs: HashSet<String> = HashSet::new();
        for func in &program.funcs {
            if func.annotations.iter().any(|a| a.name == "conserve") {
                conserve_funcs.insert(func.name.clone());
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                if method.annotations.iter().any(|a| a.name == "conserve") {
                    // Method names are qualified: TypeName::method_name
                    conserve_funcs.insert(format!("{}::{}", imp.struct_name, method.name));
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                if method.annotations.iter().any(|a| a.name == "conserve") {
                    conserve_funcs.insert(format!("{}::{}", ti.trait_name, method.name));
                }
            }
        }

        // Names of functions that create Money (the "mint" functions)
        let mint_funcs: &[&str] = &["Money::new", "Money::from_cents", "Money::from_cents"];

        let check_one = |func: &Func, conserve_funcs: &HashSet<String>| -> Vec<String> {
            let is_conserve = func.annotations.iter().any(|a| a.name == "conserve");
            if !is_conserve { return Vec::new(); }

            let mut errs = Vec::new();

            // 1. Check effects — reject IO, Net, Spawn, Diverge
            if let Some(effects) = self.func_effects.get(&func.name) {
                for eff in effects {
                    match eff.as_str() {
                        "IO" | "Net" | "Spawn" | "Diverge" => {
                            errs.push(format!(
                                "Conservation error: function '{}' is @conserve but has effect '!{}'.\n\
                                 \x20\x20\x20\x20@conserve functions must not interact with external state —\n\
                                 \x20\x20\x20\x20IO/Net can bring in external money, Spawn introduces concurrency,\n\
                                 \x20\x20\x20\x20Diverge introduces control flow uncertainty.\n\
                                 \x20\x20\x20\x20Fix: remove the @conserve annotation, or remove the '!{}' effect.",
                                func.name, eff, eff
                            ));
                        }
                        _ => {}
                    }
                }
            }

            // 2. No floating-point
            let mut fp_found = scan_for_floats(&func.body);
            for param in &func.params {
                if type_contains_float(&param.ty) {
                    fp_found.push(format!("parameter '{}: {}'", param.name, param.ty));
                }
            }
            if type_contains_float(&func.ret_ty) {
                fp_found.push(format!("return type '{}'", func.ret_ty));
            }
            if !fp_found.is_empty() {
                errs.push(format!(
                    "Conservation error: function '{}' is @conserve but uses floating-point.\n\
                     \x20\x20\x20\x20Floating-point arithmetic is not exact — money could be lost\n\
                     \x20\x20\x20\x20to rounding errors, violating conservation.\n\
                     \x20\x20\x20\x20Found float in: {}.\n\
                     \x20\x20\x20\x20Fix: use integer arithmetic (fixed-point with i64) instead.",
                    func.name, fp_found.join(", ")
                ));
            }

            // 3. Check that all called functions are also @conserve
            let mut called_funcs = HashSet::new();
            scan_stmts_for_calls(&func.body, &mut called_funcs);
            for called in &called_funcs {
                // Skip built-in runtime functions (p_print_*, p_malloc, etc.)
                if called.starts_with("p_") { continue; }
                // Skip print/println etc.
                if matches!(called.as_str(), "print" | "println" | "print_str" | "print_nl" | "print_int" | "print_map" | "print_vec") {
                    continue;
                }
                if !conserve_funcs.contains(called) {
                    // Check if it's a known function at all
                    if self.funcs.contains_key(called) {
                        errs.push(format!(
                            "Conservation error: function '{}' is @conserve but calls '{}',\n\
                             \x20\x20\x20\x20which is not @conserve.\n\
                             \x20\x20\x20\x20A @conserve function can only call other @conserve functions —\n\
                             \x20\x20\x20\x20otherwise the callee could create or destroy money.\n\
                             \x20\x20\x20\x20Fix: add @conserve to '{}', or remove the call from '{}'.",
                            func.name, called, called, func.name
                        ));
                    }
                }
            }

            // 4. Check that no mint functions are called
            for called in &called_funcs {
                for mint in mint_funcs {
                    if called == mint || called.ends_with(&format!("::{}", mint)) {
                        errs.push(format!(
                            "Conservation error: function '{}' is @conserve but calls '{}',\n\
                             \x20\x20\x20\x20which creates new Money. @conserve functions cannot mint money —\n\
                             \x20\x20\x20\x20they can only move, split, and merge existing Money.\n\
                             \x20\x20\x20\x20Fix: remove the @conserve annotation, or remove the mint call.",
                            func.name, called
                        ));
                    }
                }
                // Also check for bare "new" or "from_cents" calls on Money
                if called == "Money::new" || called == "Money::from_cents" {
                    errs.push(format!(
                        "Conservation error: function '{}' is @conserve but calls '{}',\n\
                         \x20\x20\x20\x20which creates new Money. @conserve functions cannot mint money.\n\
                         \x20\x20\x20\x20Fix: remove the @conserve annotation, or remove the mint call.",
                        func.name, called
                    ));
                }
            }

            errs
        };

        for func in &program.funcs {
            for err in check_one(func, &conserve_funcs) {
                self.errors.push(err);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                // Temporarily set the function name for the check
                let qualified = format!("{}::{}", imp.struct_name, method.name);
                let is_conserve = method.annotations.iter().any(|a| a.name == "conserve");
                if !is_conserve { continue; }

                // Check effects
                if let Some(effects) = self.func_effects.get(&qualified) {
                    for eff in effects {
                        if matches!(eff.as_str(), "IO" | "Net" | "Spawn" | "Diverge") {
                            self.errors.push(format!(
                                "Conservation error: method '{}' is @conserve but has effect '!{}'.",
                                qualified, eff
                            ));
                        }
                    }
                }

                // Check for float
                let mut fp_found = scan_for_floats(&method.body);
                for param in &method.params {
                    if type_contains_float(&param.ty) {
                        fp_found.push(format!("parameter '{}: {}'", param.name, param.ty));
                    }
                }
                if type_contains_float(&method.ret_ty) {
                    fp_found.push(format!("return type '{}'", method.ret_ty));
                }
                if !fp_found.is_empty() {
                    self.errors.push(format!(
                        "Conservation error: method '{}' is @conserve but uses floating-point. Found: {}.",
                        qualified, fp_found.join(", ")
                    ));
                }

                // Check calls
                let mut called_funcs = HashSet::new();
                scan_stmts_for_calls(&method.body, &mut called_funcs);
                for called in &called_funcs {
                    if called.starts_with("p_") { continue; }
                    if matches!(called.as_str(), "print" | "println" | "print_str" | "print_nl" | "print_int" | "print_map" | "print_vec") {
                        continue;
                    }
                    if called == "Money::new" || called == "Money::from_cents" {
                        self.errors.push(format!(
                            "Conservation error: method '{}' is @conserve but calls mint function '{}'.",
                            qualified, called
                        ));
                    }
                    if self.funcs.contains_key(called) && !conserve_funcs.contains(called) {
                        self.errors.push(format!(
                            "Conservation error: method '{}' is @conserve but calls '{}', which is not @conserve.",
                            qualified, called
                        ));
                    }
                }
            }
        }
    }

    /// Verify @gas annotation constraints.
    ///
    /// A @gas(N) function has a compile-time gas budget of N. The gas cost
    /// is the number of AST operations (statements + expressions) in the
    /// function body, plus the gas cost of all called functions (transitive).
    ///
    /// This is the Solidity gas model, but at compile time:
    ///   - Solidity: gas checked at runtime, transaction fails mid-execution
    ///   - Fides:    gas checked at compile time, rejected before deployment
    ///
    /// The gas cost is propagated through the call graph:
    ///   total_gas(func) = own_cost(func) + sum(total_gas(callee) for callee in calls(func))
    ///
    /// If total_gas > N, it's a compile error.
    fn verify_gas(&mut self, program: &Program) {
        // Collect all function gas limits: name -> limit
        let mut gas_limits: HashMap<String, u64> = HashMap::new();
        for func in &program.funcs {
            for ann in &func.annotations {
                if ann.name == "gas" {
                    if let Some(arg) = ann.args.first() {
                        if let Ok(limit) = arg.parse::<u64>() {
                            gas_limits.insert(func.name.clone(), limit);
                        }
                    }
                }
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                for ann in &method.annotations {
                    if ann.name == "gas" {
                        if let Some(arg) = ann.args.first() {
                            if let Ok(limit) = arg.parse::<u64>() {
                                gas_limits.insert(format!("{}_{}", imp.struct_name, method.name), limit);
                            }
                        }
                    }
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                for ann in &method.annotations {
                    if ann.name == "gas" {
                        if let Some(arg) = ann.args.first() {
                            if let Ok(limit) = arg.parse::<u64>() {
                                gas_limits.insert(format!("{}_{}", ti.type_name, method.name), limit);
                            }
                        }
                    }
                }
            }
        }

        // Build call graph: func name -> set of called function names
        let mut call_graph: HashMap<String, HashSet<String>> = HashMap::new();
        for func in &program.funcs {
            let mut calls = HashSet::new();
            scan_stmts_for_calls(&func.body, &mut calls);
            call_graph.insert(func.name.clone(), calls);
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let mangled = format!("{}_{}", imp.struct_name, method.name);
                let mut calls = HashSet::new();
                scan_stmts_for_calls(&method.body, &mut calls);
                call_graph.insert(mangled, calls);
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                let mangled = format!("{}_{}", ti.type_name, method.name);
                let mut calls = HashSet::new();
                scan_stmts_for_calls(&method.body, &mut calls);
                call_graph.insert(mangled, calls);
            }
        }

        // Compute own cost (AST node count) for each function
        let mut own_costs: HashMap<String, u64> = HashMap::new();
        for func in &program.funcs {
            own_costs.insert(func.name.clone(), count_gas_cost_stmts(&func.body));
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let mangled = format!("{}_{}", imp.struct_name, method.name);
                own_costs.insert(mangled, count_gas_cost_stmts(&method.body));
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                let mangled = format!("{}_{}", ti.type_name, method.name);
                own_costs.insert(mangled, count_gas_cost_stmts(&method.body));
            }
        }

        // Compute total gas (own + callees) with memoization and cycle detection
        let mut total_gas_cache: HashMap<String, u64> = HashMap::new();
        for (name, &limit) in &gas_limits {
            let mut visited: HashSet<String> = HashSet::new();
            let total = compute_total_gas(name, &own_costs, &call_graph, &mut visited, &mut total_gas_cache);
            if total > limit {
                self.errors.push(format!(
                    "Gas error: function '{}' has @gas({}) but total gas cost is {} (own: {} + callees: {}).\n\
                     \x20\x20\x20\x20The function exceeds its compile-time gas budget.\n\
                     \x20\x20\x20\x20Unlike Solidity (runtime gas — transaction fails mid-execution),\n\
                     \x20\x20\x20\x20Fides rejects gas overruns at compile time — before deployment.\n\
                     \x20\x20\x20\x20Fix: increase the gas limit, simplify the function, or reduce calls.",
                    name, limit, total,
                    own_costs.get(name).copied().unwrap_or(0),
                    total - own_costs.get(name).copied().unwrap_or(0)
                ));
            }
        }
    }

    /// Verify @crashfree annotation constraints.
    ///
    /// A @crashfree function is proven to never panic at runtime. The compiler
    /// rejects patterns that can crash:
    ///   1. `unwrap()` on Option/Result — calls p_panic on None/Err
    ///   2. `p_panic()` — explicit panic
    ///   3. Division `/` or modulo `%` — division by zero aborts
    ///      (unless divisor is a non-zero constant)
    ///   4. `while` loops without a bound — can hang or overflow stack
    ///      (for loops over finite ranges/arrays are safe)
    ///   5. Calling non-@crashfree functions — transitive crash risk
    ///
    /// This is the SPARK Ada "proof of absence of run-time errors" model,
    /// but as a simple pattern-based check rather than full formal proof.
    ///
    /// Primary use case: aerospace (DO-178C), medical (IEC 62304), automotive
    /// (ISO 26262) — any domain where a crash = catastrophic failure.
    fn verify_crashfree(&mut self, program: &Program) {
        // Collect all @crashfree function names (free functions + methods)
        let mut crashfree_funcs: HashSet<String> = HashSet::new();
        for func in &program.funcs {
            if func.annotations.iter().any(|a| a.name == "crashfree") {
                crashfree_funcs.insert(func.name.clone());
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                if method.annotations.iter().any(|a| a.name == "crashfree") {
                    crashfree_funcs.insert(format!("{}_{}", imp.struct_name, method.name));
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                if method.annotations.iter().any(|a| a.name == "crashfree") {
                    crashfree_funcs.insert(format!("{}_{}", ti.type_name, method.name));
                }
            }
        }

        // Functions that are known to panic (stdlib)
        let panic_funcs: &[&str] = &["p_panic", "Option_unwrap", "Result_unwrap"];
        // Method names that panic (called as Type::unwrap)
        let panic_methods: &[&str] = &["unwrap"];

        // Check each @crashfree function
        let check_body = |stmts: &[Stmt], func_name: &str, crashfree_funcs: &HashSet<String>| -> Vec<String> {
            let mut errs = Vec::new();
            let mut calls = HashSet::new();
            scan_stmts_for_calls(stmts, &mut calls);

            // 1. Check called functions for panic risk
            for called in &calls {
                // Direct panic functions
                if panic_funcs.contains(&called.as_str()) {
                    errs.push(format!(
                        "Crashfree error: function '{}' is @crashfree but calls '{}'.\n\
                         \x20\x20\x20\x20'{}' can panic at runtime — @crashfree functions must never panic.\n\
                         \x20\x20\x20\x20Fix: use match/unwrap_or instead of unwrap, or remove @crashfree.",
                        func_name, called, called
                    ));
                    continue;
                }
                // Method calls that panic (unwrap on Option/Result)
                if panic_methods.iter().any(|m| called == m || called.ends_with(&format!("_{}", m))) {
                    if called == "unwrap" {
                        errs.push(format!(
                            "Crashfree error: function '{}' is @crashfree but calls 'unwrap'.\n\
                             \x20\x20\x20\x20unwrap() panics on None/Err — @crashfree functions must never panic.\n\
                             \x20\x20\x20\x20Fix: use match, unwrap_or, or the ? operator instead.",
                            func_name
                        ));
                    }
                    continue;
                }
                // Skip built-in runtime functions (p_print_*, p_malloc, etc. — these don't panic)
                if called.starts_with("p_") && !panic_funcs.contains(&called.as_str()) {
                    continue;
                }
                // Skip print/println
                if matches!(called.as_str(), "print" | "println" | "print_str" | "print_nl" | "print_int") {
                    continue;
                }
                // Skip unknown functions (might be builtins we don't track)
                if !self.funcs.contains_key(called) {
                    continue;
                }
                // Check transitive: can only call other @crashfree functions
                if !crashfree_funcs.contains(called) {
                    errs.push(format!(
                        "Crashfree error: function '{}' is @crashfree but calls '{}',\n\
                         \x20\x20\x20\x20which is not @crashfree.\n\
                         \x20\x20\x20\x20A @crashfree function can only call other @crashfree functions —\n\
                         \x20\x20\x20\x20otherwise the callee could panic.\n\
                         \x20\x20\x20\x20Fix: add @crashfree to '{}', or remove the call from '{}'.",
                        func_name, called, called, func_name
                    ));
                }
            }

            // 2. Check for division/modulo (division by zero risk)
            let mut div_sites = Vec::new();
            scan_for_division(stmts, &mut div_sites);
            for (line, op) in div_sites {
                errs.push(format!(
                    "Crashfree error (line {}): function '{}' is @crashfree but uses '{}'.\n\
                     \x20\x20\x20\x20Division/modulo by zero aborts at runtime.\n\
                     \x20\x20\x20\x20@crashfree functions cannot use '{}' unless the divisor is proven non-zero.\n\
                     \x20\x20\x20\x20Fix: use a checked-divide helper, or remove @crashfree.",
                    line, func_name, op, op
                ));
            }

            // 3. Check for unbounded while loops
            let mut while_sites = Vec::new();
            scan_for_while_loops(stmts, &mut while_sites);
            for line in while_sites {
                errs.push(format!(
                    "Crashfree error (line {}): function '{}' is @crashfree but uses a 'while' loop.\n\
                     \x20\x20\x20\x20While loops can be unbounded — potential infinite loop or stack overflow.\n\
                     \x20\x20\x20\x20@crashfree functions must use 'for' loops over finite ranges/arrays instead.\n\
                     \x20\x20\x20\x20Fix: replace 'while cond {{ ... }}' with 'for i in 0..N {{ ... }}',\n\
                     \x20\x20\x20\x20or remove @crashfree.",
                    line, func_name
                ));
            }

            errs
        };

        // Check free functions
        for func in &program.funcs {
            if !func.annotations.iter().any(|a| a.name == "crashfree") { continue; }
            for err in check_body(&func.body, &func.name, &crashfree_funcs) {
                self.errors.push(err);
            }
        }
        // Check impl methods
        for imp in &program.impls {
            for method in &imp.methods {
                if !method.annotations.iter().any(|a| a.name == "crashfree") { continue; }
                let qualified = format!("{}_{}", imp.struct_name, method.name);
                for err in check_body(&method.body, &qualified, &crashfree_funcs) {
                    self.errors.push(err);
                }
            }
        }
        // Check trait impl methods
        for ti in &program.trait_impls {
            for method in &ti.methods {
                if !method.annotations.iter().any(|a| a.name == "crashfree") { continue; }
                let qualified = format!("{}_{}", ti.type_name, method.name);
                for err in check_body(&method.body, &qualified, &crashfree_funcs) {
                    self.errors.push(err);
                }
            }
        }
    }

    /// Verify safety annotation constraints using composable primitives.
    ///
    /// Instead of having separate verify_* functions for each domain annotation
    /// (@medical, @safety-critical, @autosar, @dal-a, etc.), we implement 7 core
    /// safety primitives and domain annotations as aliases that expand to them.
    ///
    /// Core primitives (compiler enforces these directly):
    ///   @no-alloc       — no dynamic allocation (p_vec_new, p_malloc, etc.)
    ///   @no-recursion   — no direct or indirect self-calls
    ///   @no-unbounded   — no while loops, only for with bounded ranges
    ///   @no-panic       — no unwrap, p_panic, non-const div/mod (= @crashfree)
    ///   @no-indirect    — no function pointers, no dynamic dispatch
    ///   @requires-audit — must have @audit
    ///   @bounded-stack  — static call depth analysis (TODO: full impl)
    ///
    /// Domain presets (aliases that expand to primitives):
    ///   @medical         = @no-alloc + @no-recursion + @no-unbounded + @no-panic + @requires-audit
    ///   @safety-critical = @no-alloc + @no-recursion + @no-unbounded + @no-panic + @no-indirect
    ///   @autosar         = @no-alloc + @no-recursion + @no-unbounded + @no-panic
    ///   @dal-a           = @no-alloc + @no-recursion + @no-unbounded + @no-panic + @requires-audit
    ///
    /// Users can also compose custom safety profiles:
    ///   @no-alloc @no-recursion pf foo() { ... }
    fn verify_safety_annotations(&mut self, program: &Program) {
        // Domain preset → list of primitives (static for lifetime safety)
        static DOMAIN_PRESETS: &[(&str, &[&str])] = &[
            ("medical", &["no-alloc", "no-recursion", "no-unbounded", "no-panic", "requires-audit"]),
            ("safety-critical", &["no-alloc", "no-recursion", "no-unbounded", "no-panic", "no-indirect"]),
            ("autosar", &["no-alloc", "no-recursion", "no-unbounded", "no-panic"]),
            ("dal-a", &["no-alloc", "no-recursion", "no-unbounded", "no-panic", "requires-audit"]),
        ];

        // Collect all functions and their expanded primitive set
        // Map: func_name → set of active primitives
        let mut func_primitives: HashMap<String, HashSet<&'static str>> = HashMap::new();

        let expand_annotations = |annotations: &[Annotation]| -> HashSet<&'static str> {
            let mut primitives: HashSet<&'static str> = HashSet::new();
            for ann in annotations {
                // Direct primitives — use static string literals
                match ann.name.as_str() {
                    "no-alloc" => { primitives.insert("no-alloc"); }
                    "no-recursion" => { primitives.insert("no-recursion"); }
                    "no-unbounded" => { primitives.insert("no-unbounded"); }
                    "no-panic" => { primitives.insert("no-panic"); }
                    "no-indirect" => { primitives.insert("no-indirect"); }
                    "requires-audit" => { primitives.insert("requires-audit"); }
                    "bounded-stack" => { primitives.insert("bounded-stack"); }
                    _ => {}
                }
                // Domain presets — expand to their primitives
                for (preset_name, preset_prims) in DOMAIN_PRESETS {
                    if &ann.name == preset_name {
                        for p in *preset_prims {
                            primitives.insert(p);
                        }
                    }
                }
            }
            primitives
        };

        for func in &program.funcs {
            let prims = expand_annotations(&func.annotations);
            if !prims.is_empty() {
                func_primitives.insert(func.name.clone(), prims);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let prims = expand_annotations(&method.annotations);
                if !prims.is_empty() {
                    func_primitives.insert(format!("{}_{}", imp.struct_name, method.name), prims);
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                let prims = expand_annotations(&method.annotations);
                if !prims.is_empty() {
                    func_primitives.insert(format!("{}_{}", ti.type_name, method.name), prims);
                }
            }
        }

        // Functions that allocate memory (forbidden by @no-alloc)
        let alloc_funcs: &[&str] = &[
            "p_vec_new", "p_vec_push", "p_vec_push_str", "p_vec_insert",
            "p_vec_resize", "p_vec_extend",
            "p_hashmap_new", "p_hashmap_insert",
            "p_string_new", "p_string_push", "p_string_concat",
            "p_malloc", "p_alloc", "p_realloc",
            "p_arena_new", "p_arena_alloc",
            "Vec_new", "Vec_push", "Vec_insert", "Vec_resize",
            "HashMap_new", "HashMap_insert",
            "String_new", "String_push",
        ];
        let alloc_methods: &[&str] = &["push", "insert", "resize", "extend", "push_str"];

        // Panic functions (forbidden by @no-panic)
        let panic_funcs: &[&str] = &["p_panic", "Option_unwrap", "Result_unwrap"];
        let panic_methods: &[&str] = &["unwrap"];

        // Indirect call patterns (forbidden by @no-indirect)
        let indirect_funcs: &[&str] = &["p_make_dyn", "p_call_ptr", "p_dyn_call"];

        // Domain label for error messages (use the original annotation name)
        let domain_label = |prims: &HashSet<&str>| -> &str {
            if prims.contains("requires-audit") && prims.contains("no-indirect") {
                "safety-critical (ISO 26262 / DO-178C)"
            } else if prims.contains("requires-audit") {
                "medical (IEC 62304) / dal-a (DAL-A)"
            } else if prims.contains("no-indirect") {
                "safety-critical"
            } else {
                "safety-annotated"
            }
        };

        let check_body = |stmts: &[Stmt],
                          func_name: &str,
                          prims: &HashSet<&str>,
                          func_primitives: &HashMap<String, HashSet<&'static str>>|
            -> Vec<String> {
            let mut errs = Vec::new();
            let label = domain_label(prims);
            let mut calls = HashSet::new();
            scan_stmts_for_calls(stmts, &mut calls);

            for called in &calls {
                // @no-alloc: check for dynamic allocation
                if prims.contains("no-alloc") {
                    if alloc_funcs.contains(&called.as_str()) {
                        errs.push(format!(
                            "Safety error ({}): function '{}' is @no-alloc but calls '{}'.\n\
                             \x20\x20\x20\x20Dynamic allocation is forbidden — deterministic memory usage required.\n\
                             \x20\x20\x20\x20Fix: use stack-allocated arrays, or remove the safety annotation.",
                            label, func_name, called
                        ));
                        continue;
                    }
                    if alloc_methods.contains(&called.as_str()) {
                        errs.push(format!(
                            "Safety error ({}): function '{}' is @no-alloc but calls '{}'.\n\
                             \x20\x20\x20\x20Dynamic allocation is forbidden.\n\
                             \x20\x20\x20\x20Fix: use stack-allocated arrays, or remove the safety annotation.",
                            label, func_name, called
                        ));
                        continue;
                    }
                }

                // @no-panic: check for panic risk
                if prims.contains("no-panic") {
                    if panic_funcs.contains(&called.as_str()) {
                        errs.push(format!(
                            "Safety error ({}): function '{}' is @no-panic but calls '{}'.\n\
                             \x20\x20\x20\x20'{}' can panic — safety-critical functions must never panic.\n\
                             \x20\x20\x20\x20Fix: use match/unwrap_or instead of unwrap, or remove the safety annotation.",
                            label, func_name, called, called
                        ));
                        continue;
                    }
                    if called == "unwrap" {
                        errs.push(format!(
                            "Safety error ({}): function '{}' is @no-panic but calls 'unwrap'.\n\
                             \x20\x20\x20\x20unwrap() panics on None/Err — safety-critical functions must never panic.\n\
                             \x20\x20\x20\x20Fix: use match, unwrap_or, or the ? operator instead.",
                            label, func_name
                        ));
                        continue;
                    }
                }

                // @no-indirect: check for function pointers / dynamic dispatch
                if prims.contains("no-indirect") {
                    if indirect_funcs.contains(&called.as_str()) {
                        errs.push(format!(
                            "Safety error ({}): function '{}' is @no-indirect but calls '{}'.\n\
                             \x20\x20\x20\x20Indirect calls (function pointers, dynamic dispatch) are forbidden —\n\
                             \x20\x20\x20\x20they make call graph analysis impossible.\n\
                             \x20\x20\x20\x20Fix: use direct calls only, or remove the safety annotation.",
                            label, func_name, called
                        ));
                        continue;
                    }
                }

                // Skip built-in runtime functions (p_print_*, etc.)
                if called.starts_with("p_")
                    && !panic_funcs.contains(&called.as_str())
                    && !alloc_funcs.contains(&called.as_str())
                    && !indirect_funcs.contains(&called.as_str())
                {
                    continue;
                }
                if matches!(called.as_str(), "print" | "println" | "print_str" | "print_nl" | "print_int") {
                    continue;
                }

                // @no-recursion: check for direct self-call
                if prims.contains("no-recursion") && called == func_name {
                    errs.push(format!(
                        "Safety error ({}): function '{}' is @no-recursion but calls itself.\n\
                         \x20\x20\x20\x20Recursion is forbidden — stack overflow is a safety risk,\n\
                         \x20\x20\x20\x20and recursion makes WCET unbounded.\n\
                         \x20\x20\x20\x20Fix: convert to an iterative loop, or remove the safety annotation.",
                        label, func_name
                    ));
                    continue;
                }

                // Transitive: can only call other safety-annotated functions
                // (with compatible or superset primitives)
                if !self.funcs.contains_key(called) {
                    continue; // unknown — might be builtin
                }
                if let Some(callee_prims) = func_primitives.get(called) {
                    // Callee has safety annotations — check it has all required primitives
                    for required in prims {
                        if !callee_prims.contains(required) {
                            errs.push(format!(
                                "Safety error ({}): function '{}' is safety-annotated but calls '{}',\n\
                                 \x20\x20\x20\x20which lacks the '{}' primitive.\n\
                                 \x20\x20\x20\x20A safety-annotated function can only call functions with\n\
                                 \x20\x20\x20\x20compatible safety guarantees.\n\
                                 \x20\x20\x20\x20Fix: add the missing safety annotation to '{}', or remove the call.",
                                label, func_name, called, required, called
                            ));
                            break;
                        }
                    }
                } else {
                    // Callee has no safety annotations
                    errs.push(format!(
                        "Safety error ({}): function '{}' is safety-annotated but calls '{}',\n\
                         \x20\x20\x20\x20which has no safety annotations.\n\
                         \x20\x20\x20\x20A safety-annotated function can only call other safety-annotated functions —\n\
                         \x20\x20\x20\x20otherwise the callee could allocate, recurse, or panic.\n\
                         \x20\x20\x20\x20Fix: add safety annotations to '{}', or remove the call from '{}'.",
                        label, func_name, called, called, func_name
                    ));
                }
            }

            // @no-unbounded: check for while loops
            if prims.contains("no-unbounded") {
                let mut while_sites = Vec::new();
                scan_for_while_loops(stmts, &mut while_sites);
                for line in while_sites {
                    errs.push(format!(
                        "Safety error ({}) (line {}): function '{}' is @no-unbounded but uses a 'while' loop.\n\
                         \x20\x20\x20\x20While loops are unbounded — use 'for' loops\n\
                         \x20\x20\x20\x20over finite ranges/arrays for deterministic execution time.\n\
                         \x20\x20\x20\x20Fix: replace 'while cond {{ ... }}' with 'for i in 0..N {{ ... }}',\n\
                         \x20\x20\x20\x20or remove the safety annotation.",
                        label, line, func_name
                    ));
                }
            }

            errs
        };

        // Check all functions with safety annotations
        let check_func = |func: &Func,
                          qualified_name: &str,
                          annotations: &[Annotation],
                          func_primitives: &HashMap<String, HashSet<&'static str>>|
            -> Vec<String> {
            let prims = func_primitives.get(qualified_name);
            if prims.is_none() || prims.unwrap().is_empty() {
                return Vec::new();
            }
            let prims = prims.unwrap();
            let label = domain_label(prims);
            let mut errs = Vec::new();

            // @requires-audit: must have @audit
            if prims.contains("requires-audit") {
                let has_audit = annotations.iter().any(|a| a.name == "audit");
                if !has_audit {
                    errs.push(format!(
                        "Safety error ({}): function '{}' is @requires-audit but not @audit.\n\
                         \x20\x20\x20\x20Safety-critical functions must have @audit for regulatory traceability —\n\
                         \x20\x20\x20\x20every function call must be logged for compliance.\n\
                         \x20\x20\x20\x20Fix: add @audit to '{}', or remove the safety annotation.",
                        label, qualified_name, qualified_name
                    ));
                }
            }

            // Check body for all primitives
            errs.extend(check_body(&func.body, qualified_name, prims, func_primitives));
            errs
        };

        for func in &program.funcs {
            for err in check_func(func, &func.name, &func.annotations, &func_primitives) {
                self.errors.push(err);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let qualified = format!("{}_{}", imp.struct_name, method.name);
                for err in check_func(method, &qualified, &method.annotations, &func_primitives) {
                    self.errors.push(err);
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                let qualified = format!("{}_{}", ti.type_name, method.name);
                for err in check_func(method, &qualified, &method.annotations, &func_primitives) {
                    self.errors.push(err);
                }
            }
        }
    }

    /// Verify given/ensure contracts (F10: Design by Contract).
    ///
    /// `given` preconditions are checked at call sites: if the argument values
    /// are compile-time constants, the precondition expression is evaluated.
    /// If it evaluates to false → compile error. If unknown → no error (runtime
    /// assertion would be emitted in debug mode, but we keep it simple here).
    ///
    /// `ensure` postconditions are checked by verifying that the function body
    /// doesn't violate obvious cases (e.g., if ensure says `result >= 0`, we
    /// don't do full proof, but we do check that the function doesn't return
    /// a known-negative constant).
    ///
    /// This is a pragmatic implementation — not full formal proof, but catches
    /// the most common contract violations at compile time.
    fn verify_contracts(&mut self, program: &Program) {
        // Build a map of function name → preconditions
        let mut func_preconditions: HashMap<String, Vec<Expr>> = HashMap::new();
        let mut func_params: HashMap<String, Vec<String>> = HashMap::new();
        for func in &program.funcs {
            if !func.preconditions.is_empty() {
                func_preconditions.insert(func.name.clone(), func.preconditions.clone());
                func_params.insert(func.name.clone(), func.params.iter().map(|p| p.name.clone()).collect());
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                if !method.preconditions.is_empty() {
                    let qualified = format!("{}_{}", imp.struct_name, method.name);
                    func_preconditions.insert(qualified.clone(), method.preconditions.clone());
                    func_params.insert(qualified, method.params.iter().map(|p| p.name.clone()).collect());
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                if !method.preconditions.is_empty() {
                    let qualified = format!("{}_{}", ti.type_name, method.name);
                    func_preconditions.insert(qualified.clone(), method.preconditions.clone());
                    func_params.insert(qualified, method.params.iter().map(|p| p.name.clone()).collect());
                }
            }
        }

        // Scan all function bodies for calls to functions with preconditions
        let check_calls = |stmts: &[Stmt], caller_name: &str| -> Vec<String> {
            let mut errs = Vec::new();
            let mut calls = Vec::new();
            scan_stmts_for_call_sites(stmts, &mut calls);
            for (callee, args, line) in calls {
                if let Some(pres) = func_preconditions.get(&callee) {
                    if let Some(params) = func_params.get(&callee) {
                        for pre in pres {
                            // Try to evaluate the precondition with known constant args
                            if let Some(result) = eval_contract(pre, params, &args) {
                                if !result {
                                    errs.push(format!(
                                        "Contract error (F10) (line {}): precondition violated in call to '{}'.\n\
                                         \x20\x20\x20\x20The 'given' contract is not satisfied with the provided arguments.\n\
                                         \x20\x20\x20\x20Fix: ensure the arguments satisfy the precondition, or relax the contract.",
                                        line, callee
                                    ));
                                }
                            }
                            // If result is None, we can't evaluate — skip (runtime check needed)
                        }
                    }
                }
            }
            errs
        };

        for func in &program.funcs {
            for err in check_calls(&func.body, &func.name) {
                self.errors.push(err);
            }
        }
        for imp in &program.impls {
            for method in &imp.methods {
                let qualified = format!("{}_{}", imp.struct_name, method.name);
                for err in check_calls(&method.body, &qualified) {
                    self.errors.push(err);
                }
            }
        }
        for ti in &program.trait_impls {
            for method in &ti.methods {
                let qualified = format!("{}_{}", ti.type_name, method.name);
                for err in check_calls(&method.body, &qualified) {
                    self.errors.push(err);
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
            Stmt::LetTuple { expr, .. } => self.scan_expr_for_effects(expr, effects),
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
            Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
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
                    "p_spawn_process" | "p_spawn_thread" | "p_thread_spawn" => {
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
            Expr::OptionalFieldAccess { expr, .. } => self.scan_expr_for_effects(expr, effects),
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
                "Sandbox error: '{}' requires '{}' capability — all capabilities are revoked \
                 inside `sandbox {{ }}`.\n\
                 \x20\x20\x20\x20Hint: move the '{}' call outside the sandbox block, or remove the \
                 sandbox if the operation is safe.",
                context, cap, context
            ));
            if let Some(fname) = &self.current_func_name {
                self.cap_verified.remove(fname);
            }
            return;
        }
        if self.strict_caps && !self.granted_caps.contains(cap) {
            self.errors.push(format!(
                "Capability error: '{}' requires '{}' capability (not granted to module '{}').\n\
                 \x20\x20\x20\x20Fix: add `use {} with {{ {} }}` at the top of the file to grant \
                 the '{}' capability to this module.",
                context, cap, self.current_module,
                self.current_module, cap, cap
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
        self.scope_linear_vars.push(Vec::new());
    }

    fn pop_scope(&mut self) {
        // Check that all linear variables in this scope were consumed
        if let Some(linear_vars) = self.scope_linear_vars.pop() {
            for (name, line) in &linear_vars {
                if !self.consumed_vars.contains(name) {
                    self.errors.push(format!(
                        "Linear type error (line {}): variable '{}' is linear but was never consumed.\n\
                         \x20\x20\x20\x20Linear values must be used exactly once — pass them to a function,\n\
                         \x20\x20\x20\x20return them, or move them to another variable.\n\
                         \x20\x20\x20\x20Fix: use '{}' somewhere, or remove the binding.",
                        line, name, name
                    ));
                }
            }
        }
        self.scopes.pop();
    }

    fn declare(&mut self, name: &str, ty: Type) {
        self.scopes.last_mut().unwrap().insert(name.to_string(), ty);
    }

    fn lookup(&self, name: &str) -> Option<&Type> {
        self.scopes.iter().rev().find_map(|s| s.get(name))
    }

    /// Check if a type is linear (i.e., it's a @linear struct or contains one).
    fn is_linear_type(&self, ty: &Type) -> bool {
        match ty {
            Type::Named(name) => self.linear_structs.contains(name),
            Type::Generic { name, .. } => self.linear_structs.contains(name),
            Type::Tuple(types) => types.iter().any(|t| self.is_linear_type(t)),
            Type::Array { elem, .. } | Type::ArrayConst { elem, .. } => self.is_linear_type(elem),
            Type::Ref { inner, .. } => self.is_linear_type(inner),
            _ => false,
        }
    }

    /// Mark a variable as consumed (moved). If it's already consumed, error.
    fn consume_var(&mut self, name: &str, line: usize) {
        if self.loop_depth > 0 {
            self.errors.push(format!(
                "Linear type error (line {}): variable '{}' cannot be consumed inside a loop.\n\
                 \x20\x20\x20\x20Linear values can only be used once — consuming them inside a loop\n\
                 \x20\x20\x20\x20would fail on the second iteration. Move the consumption outside\n\
                 \x20\x20\x20\x20the loop, or use a non-linear type.",
                line, name
            ));
        }
        if self.consumed_vars.contains(name) {
            self.errors.push(format!(
                "Linear type error (line {}): variable '{}' was already consumed (moved).\n\
                 \x20\x20\x20\x20Linear values can only be used once. After being passed to a function,\n\
                 \x20\x20\x20\x20returned, or moved, they cannot be used again.\n\
                 \x20\x20\x20\x20Fix: use a different variable, or restructure the code so '{}' is only used once.",
                line, name, name
            ));
        } else {
            self.consumed_vars.insert(name.to_string());
        }
    }

    /// Check if consuming an expression would consume any linear variables.
    /// For `Expr::Ident(name)`, this marks the variable as consumed.
    /// For compound expressions, recurse into sub-expressions.
    fn consume_linear_in_expr(&mut self, expr: &Expr, line: usize) {
        match expr {
            Expr::Ident(name) => {
                if let Some(ty) = self.lookup(name) {
                    if self.is_linear_type(ty) {
                        self.consume_var(name, line);
                    }
                }
            }
            Expr::Binary { left, right, .. } => {
                self.consume_linear_in_expr(left, line);
                self.consume_linear_in_expr(right, line);
            }
            Expr::Call { args, .. } => {
                for arg in args {
                    self.consume_linear_in_expr(arg, line);
                }
            }
            Expr::MethodCall { receiver, args, .. } => {
                self.consume_linear_in_expr(receiver, line);
                for arg in args {
                    self.consume_linear_in_expr(arg, line);
                }
            }
            Expr::FieldAccess { expr, .. } => {
                // Field access on a linear type does NOT consume the struct.
                // Reading a scalar field (e.g. m.cents, acc.id) is a borrow —
                // it reads the value without moving the whole struct.
                // Only passing the whole struct to a function, returning it, or
                // assigning it consumes it.
                // This allows reading multiple fields from a linear struct:
                //   pv id = acc.id;       // borrow
                //   pv bal = acc.balance; // borrow (but bal is Money — linear!)
                // Note: if the field itself is linear (e.g. balance: Money),
                // accessing it DOES consume that field's value. But the struct
                // itself is not consumed — other non-linear fields can still be read.
            }
            Expr::StructLit { fields, spread, .. } => {
                for (_, e) in fields {
                    self.consume_linear_in_expr(e, line);
                }
                if let Some(s) = spread {
                    self.consume_linear_in_expr(s, line);
                }
            }
            Expr::ArrayLit { elements, .. } => {
                for e in elements {
                    self.consume_linear_in_expr(e, line);
                }
            }
            Expr::TupleLit(elements) => {
                for e in elements {
                    self.consume_linear_in_expr(e, line);
                }
            }
            Expr::Ref { expr, .. } => {
                // Taking a reference does NOT consume — the original is still usable.
                // (Borrowing is allowed; only moving consumes.)
            }
            Expr::Deref { expr } => {
                self.consume_linear_in_expr(expr, line);
            }
            Expr::Cast { expr, .. } => {
                self.consume_linear_in_expr(expr, line);
            }
            Expr::If { cond, then_block, else_block } => {
                self.consume_linear_in_expr(cond, line);
                // Don't consume in blocks — they're checked separately by check_stmt
            }
            Expr::Block(_) => {
                // Block is checked by check_stmt
            }
            Expr::Match { expr, .. } => {
                self.consume_linear_in_expr(expr, line);
            }
            Expr::Coalesce { expr, default } => {
                self.consume_linear_in_expr(expr, line);
                self.consume_linear_in_expr(default, line);
            }
            Expr::Try { expr } => {
                self.consume_linear_in_expr(expr, line);
            }
            Expr::StrConcat { left, right } => {
                self.consume_linear_in_expr(left, line);
                self.consume_linear_in_expr(right, line);
            }
            Expr::Range { start, end, .. } => {
                self.consume_linear_in_expr(start, line);
                self.consume_linear_in_expr(end, line);
            }
            _ => {}
        }
    }

    fn check_func(&mut self, func: &Func) {
        self.current_func_name = Some(func.name.clone());
        // Set current_module to the module where this function was defined,
        // so field privacy checks work correctly (private fields are accessible
        // from within their defining module).
        let saved_module = self.current_module.clone();
        if let Some(mod_name) = self.item_modules.get(&func.name) {
            self.current_module = mod_name.clone();
        }
        // Reset linear tracking for this function
        let saved_consumed = std::mem::take(&mut self.consumed_vars);
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
            self.declare(&param.name, resolved_ty.clone());
            // Linear parameters are consumed at the call site (by the caller).
            // Inside the function, the parameter is a new binding that can be
            // used freely (field access, passed to other functions, returned).
            // So we do NOT track linear params in scope_linear_vars.
        }

        for stmt in &func.body {
            self.check_stmt(stmt);
        }

        self.current_type_params = saved_tp;
        self.current_type_param_bounds = saved_bounds;
        self.pop_scope();
        self.consumed_vars = saved_consumed;
        self.current_module = saved_module;
        self.current_func_name = None;
    }

    fn check_stmt(&mut self, stmt: &Stmt) {
        match stmt {
            Stmt::Let { name, ty, expr, is_mut: _, is_persist: _, line } => {
                let expr_ty = self.check_expr(expr);
                // Consume linear values in the initializer expression
                self.consume_linear_in_expr(expr, *line);
                let final_ty = if let Some(declared) = ty {
                    if !self.types_compatible(declared, &expr_ty) {
                        self.errors.push(format!(
                            "Type error (line {}): cannot assign {} to variable '{}' of type {}.\n\
                             \x20\x20\x20\x20Expected: {}\n\
                             \x20\x20\x20\x20Found:    {}\n\
                             \x20\x20\x20\x20Fix: change the type annotation or the expression.",
                            line, expr_ty, name, declared,
                            declared, expr_ty
                        ));
                    }
                    declared.clone()
                } else {
                    expr_ty
                };
                // Track region: if inside a region block, record the allocation region
                if let Some(region) = self.region_stack.last() {
                    self.var_regions.insert(name.clone(), region.clone());
                    self.var_alloc_lines.insert(name.clone(), *line);
                    // Track allocation size for budget checking
                    let size = self.type_size(&final_ty);
                    if let Some(entry) = self.region_budgets.last_mut() {
                        entry.2 += size;
                    }
                }
                // Track linear variables — they must be consumed before scope exit
                if self.is_linear_type(&final_ty) {
                    if let Some(scope) = self.scope_linear_vars.last_mut() {
                        scope.push((name.clone(), *line));
                    }
                }
                self.declare(name, final_ty);
            }
            Stmt::LetTuple { names, expr, line } => {
                let expr_ty = self.check_expr(expr);
                self.consume_linear_in_expr(expr, *line);
                match &expr_ty {
                    Type::Tuple(elem_types) => {
                        if names.len() != elem_types.len() {
                            self.errors.push(format!(
                                "Type error (line {}): tuple destructuring expects {} names but expression has {} elements.",
                                line, names.len(), elem_types.len()
                            ));
                        }
                        for (name, elem_ty) in names.iter().zip(elem_types.iter()) {
                            if self.is_linear_type(elem_ty) {
                                if let Some(scope) = self.scope_linear_vars.last_mut() {
                                    scope.push((name.clone(), *line));
                                }
                            }
                            self.declare(name, elem_ty.clone());
                        }
                    }
                    _ => {
                        self.errors.push(format!(
                            "Type error (line {}): cannot destructure {} as a tuple — expected a tuple type.",
                            line, expr_ty
                        ));
                    }
                }
            }
            Stmt::LetUninit { name, ty, line } => {
                let resolved_ty = self.resolve_type(ty);
                // Declare the variable with its type
                self.declare(name, resolved_ty.clone());
                // Mark as uninitialized — reading before assignment is a compile error
                self.uninit_vars.insert(name.clone());
                // Track linear variables
                if self.is_linear_type(&resolved_ty) {
                    if let Some(scope) = self.scope_linear_vars.last_mut() {
                        scope.push((name.clone(), *line));
                    }
                }
            }
            Stmt::Expr(expr, line) => {
                self.check_expr(expr);
                self.consume_linear_in_expr(expr, *line);
            }
            Stmt::Return(expr, line) => {
                if let Some(e) = expr {
                    self.check_expr(e);
                    self.consume_linear_in_expr(e, *line);
                }
            }
            Stmt::If {
                cond,
                then_block,
                else_block,
                line,
            } => {
                let cond_ty = self.check_expr(cond);
                self.consume_linear_in_expr(cond, *line);
                if cond_ty != Type::Bool && cond_ty != Type::I32 && cond_ty != Type::I64 {
                    self.errors.push(format!(
                        "Type error: if condition must be bool, got {}",
                        cond_ty
                    ));
                }
                // For linear types: variables consumed in then-branch must also be
                // consumed in else-branch (and vice versa). We snapshot consumed_vars
                // before each branch and merge after.
                let consumed_before = self.consumed_vars.clone();
                // F63: snapshot uninit vars before branches for definite assignment
                let uninit_before = self.uninit_vars.clone();
                self.push_scope();
                for s in then_block {
                    self.check_stmt(s);
                }
                self.pop_scope();
                let consumed_after_then = self.consumed_vars.clone();
                let uninit_after_then = self.uninit_vars.clone();
                // Restore to before-then state for else branch
                self.consumed_vars = consumed_before.clone();
                self.uninit_vars = uninit_before.clone();
                if let Some(else_b) = else_block {
                    self.push_scope();
                    for s in else_b {
                        self.check_stmt(s);
                    }
                    self.pop_scope();
                }
                // Merge: a variable is consumed after the if if it was consumed in BOTH branches.
                // (If only consumed in one branch, that's a conditional move — the variable might
                // still be live in the other path. For linear types, we require consumption in
                // all paths, so we take the intersection.)
                let consumed_after_else = self.consumed_vars.clone();
                self.consumed_vars = consumed_after_then.intersection(&consumed_after_else).cloned().collect();
                // F63: a variable is initialized after if/else only if it was initialized
                // in BOTH branches. Take the union of uninit sets (vars uninit in either
                // branch are still uninit after the if/else).
                self.uninit_vars = uninit_after_then.union(&self.uninit_vars).cloned().collect();
            }
            Stmt::Assign { target, op: _, expr, line } => {
                let _expr_ty = self.check_expr(expr);
                // Consume linear values in the RHS
                self.consume_linear_in_expr(expr, *line);
                // Mark as initialized (F63: init guarantee)
                self.uninit_vars.remove(target);
                // Check target exists
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
                } else if let Some(ty) = self.lookup(target).cloned() {
                    // Re-assigning a linear variable: the old value must have been consumed
                    // (otherwise we'd be silently destroying a linear value).
                    // The new value takes ownership.
                    if self.is_linear_type(&ty) && !self.consumed_vars.contains(target) {
                        self.errors.push(format!(
                            "Linear type error (line {}): cannot reassign '{}' — the old value \
                             would be silently destroyed.\n\
                             \x20\x20\x20\x20Linear values must be explicitly consumed (passed to a function,\n\
                             \x20\x20\x20\x20returned, or moved) before reassignment.\n\
                             \x20\x20\x20\x20Fix: consume '{}' first, or use a different variable name.",
                            line, target, target
                        ));
                    }
                    if self.is_linear_type(&ty) {
                        // Mark as unconsumed again (new value takes ownership)
                        self.consumed_vars.remove(target);
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
                self.loop_depth += 1;
                for s in body {
                    self.check_stmt(s);
                }
                self.loop_depth -= 1;
                self.pop_scope();
            }
            Stmt::For { var, iterator, body, line: _ } => {
                let iter_ty = self.check_expr(iterator);
                // Extract element type: Vec<T> → T, [T; N] → T, range → I64,
                // HashMap<K, V> → K (iterate over keys)
                let elem_ty = match &iter_ty {
                    Type::Generic { name, args } if name == "Vec" => {
                        args.first().cloned().unwrap_or(Type::I64)
                    }
                    Type::Generic { name, args } if name == "HashMap" => {
                        // for k in map → iterate over keys (type K)
                        args.first().cloned().unwrap_or(Type::Str)
                    }
                    Type::Array { elem, .. } => *elem.clone(),
                    _ => iter_ty,
                };
                self.push_scope();
                self.declare(var, elem_ty);
                self.loop_depth += 1;
                for s in body {
                    self.check_stmt(s);
                }
                self.loop_depth -= 1;
                self.pop_scope();
            }
            Stmt::Loop { body, line: _ } => {
                self.push_scope();
                self.loop_depth += 1;
                for s in body {
                    self.check_stmt(s);
                }
                self.loop_depth -= 1;
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
            Stmt::RegionBlock { name, budget, body, line } => {
                // Push region scope
                self.region_stack.push(name.clone());
                self.push_scope();
                // Push budget tracker
                self.region_budgets.push((name.clone(), *budget, 0));
                // Record the source line for better error messages
                self.region_lines.insert(name.clone(), *line);

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
                            let alloc_line = self.var_alloc_lines.get(var).copied().unwrap_or(0);
                            let region_line = self.region_lines.get(&region_name).copied().unwrap_or(0);
                            self.errors.push(format!(
                                "Region error (line {}): variable '{}' escapes region '{}'.\n\
                                 \x20\x20\x20\x20'{}' was allocated at line {} inside `region {} {{ ... }}` (line {}),\n\
                                 \x20\x20\x20\x20but it is still accessible after the region block ended.\n\
                                 \x20\x20\x20\x20The region frees all its data when the block exits — accessing '{}' \
                                 after that is a use-after-free.\n\
                                 \x20\x20\x20\x20Fix: move the use of '{}' inside the region block, or allocate '{}' \
                                 outside the region if it needs to live longer.",
                                region_line, var, region_name,
                                var, alloc_line, region_name, region_line,
                                var,
                                var, var
                            ));
                        }
                    }
                }

                // Check memory budget
                if let Some((_, budget_opt, usage)) = self.region_budgets.pop() {
                    if let Some(budget) = budget_opt {
                        if usage > budget {
                            let region_line = self.region_lines.get(&region_name).copied().unwrap_or(0);
                            self.errors.push(format!(
                                "Budget error (line {}): region '{}' used {} bytes but budget is {} bytes \
                                 ({} bytes over).\n\
                                 \x20\x20\x20\x20Hint: reduce allocations inside the region block, or increase the budget \
                                 in `region {} with budget({}) {{ ... }}`.",
                                region_line, region_name, usage, budget, usage - budget,
                                region_name, budget
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
            Stmt::Unsafe { body, .. } => {
                // Push scope and enter unsafe mode (permits raw pointer ops)
                self.push_scope();
                self.unsafe_depth += 1;
                for s in body {
                    self.check_stmt(s);
                }
                self.unsafe_depth -= 1;
                self.pop_scope();
            }
            Stmt::Txn { body, .. } => {
                // Transaction block — typecheck body normally.
                // The rollback semantics are handled at runtime (interpreter)
                // or at the application level (native code).
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
            }
            Stmt::ComptimeGen { .. } => {}
            Stmt::GradBlock { var, body, .. } => {
                // Typecheck the body, mark the variable as requiring gradient tracking
                self.push_scope();
                for s in body {
                    self.check_stmt(s);
                }
                self.pop_scope();
                // Mark the variable as requiring gradient tracking
                if let Some(ty) = self.lookup(var) {
                    let _ = ty; // variable exists — gradient tracking is handled later
                }
            }
            Stmt::Backward { loss, .. } => {
                // Typecheck the loss expression — must be F64 or Tensor
                let loss_ty = self.check_expr(loss);
                let is_f64 = loss_ty == Type::F64;
                let is_tensor = match &loss_ty {
                    Type::Named(name) => name == "Tensor",
                    Type::Generic { name, .. } => name == "Tensor",
                    Type::ShapedTensor { .. } => true,
                    _ => false,
                };
                if !is_f64 && !is_tensor {
                    self.errors.push(format!(
                        "Type error: backward() loss must be F64 or Tensor, got {}",
                        loss_ty
                    ));
                }
            }
        }
    }

    fn check_expr(&mut self, expr: &Expr) -> Type {
        match expr {
            Expr::IntLit(v) => Self::infer_int_type(*v),
            Expr::FloatLit(_) => Type::F64,
            Expr::StringLit(_) => Type::Str,
            Expr::BoolLit(_) => Type::Bool,
            Expr::Ident(name) => {
                // F63: init guarantee — check if variable is read before assignment
                if self.uninit_vars.contains(name) {
                    self.errors.push(format!(
                        "Init error: variable '{}' is used before being assigned a value.\n\
                         \x20\x20\x20\x20It was declared with `pm` (uninitialized mutable).\n\
                         \x20\x20\x20\x20Fix: assign a value to '{}' before reading it, or use `pv {} = ...` to initialize at declaration.",
                        name, name, name
                    ));
                }
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
                        // F68: parameterized queries — reject string concatenation
                        // on typed strings (SQLString, ShellString, HTMLString, etc.)
                        // to prevent injection attacks. Typed strings must use
                        // their bind/escape methods instead of +.
                        if op == &BinOp::Add {
                            if self.is_typed_string(&lt) || self.is_typed_string(&rt) {
                                self.errors.push(format!(
                                    "Injection error (F68): cannot use + to concatenate typed string '{}'.\n\
                                     \x20\x20\x20\x20Typed strings prevent injection — use bind/escape methods instead.\n\
                                     \x20\x20\x20\x20For SQLString: use sql_query() + sql_bind() instead of string +.\n\
                                     \x20\x20\x20\x20For ShellString: use shell_cmd() + shell_escape() instead of string +.",
                                    if self.is_typed_string(&lt) { &lt } else { &rt }
                                ));
                                return Type::Never;
                            }
                        }
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
                                "Type error: arithmetic requires numeric types.\n\
                                 \x20\x20\x20\x20Left operand:  {}\n\
                                 \x20\x20\x20\x20Right operand: {}\n\
                                 \x20\x20\x20\x20Fix: convert one or both operands to a numeric type (i8, i16, i32, i64, u8, u16, u32, u64, f32, f64).",
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
                    "thread_id" | "workgroup_size" | "workgroup_id" | "num_workgroups" => {
                        // GPU kernel builtins — return i64
                        // These are only valid inside @kernel functions
                        return Type::I64;
                    }
                    "quantize_q8" | "dequantize_q8" | "qmatmul" | "find_quant_params" => {
                        // Quantization builtins — return Tensor or str
                        if callee == "find_quant_params" {
                            return Type::Str;
                        }
                        // quantize/dequantize/qmatmul return Tensor
                        if let Some((_, ret)) = self.funcs.get(callee).cloned() {
                            return ret;
                        }
                        return Type::Generic { name: "Tensor".to_string(), args: vec![Type::F64] };
                    }
                    "http_get" | "http_post" | "socket_connect" | "socket_send" | "socket_recv" => {
                        self.check_capability("Net", &format!("{}()", callee));
                    }
                    "spawn" | "exec" | "system" | "p_thread_spawn" => {
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

                // Shape-level type checking for tensor operations
                // Verifies shape compatibility at compile time
                match callee.as_str() {
                    "matmul" => {
                        if arg_types.len() == 2 {
                            let a_shape = self.get_tensor_shape(&arg_types[0]);
                            let b_shape = self.get_tensor_shape(&arg_types[1]);
                            if let (Some(a), Some(b)) = (&a_shape, &b_shape) {
                                if a.len() >= 2 && b.len() >= 2 {
                                    let a_last = a.last().unwrap();
                                    let b_first = b.first().unwrap();
                                    if !self.shapes_compatible(a_last, b_first) {
                                        self.errors.push(format!(
                                            "Shape error: matmul incompatible — a last dim {:?} != b first dim {:?}",
                                            a_last, b_first
                                        ));
                                    }
                                    // Result shape: a[..-1] ++ b[1..]
                                    let mut result_shape = a[..a.len()-1].to_vec();
                                    result_shape.extend(b[1..].iter().cloned());
                                    return Type::ShapedTensor {
                                        elem: Box::new(Type::F64),
                                        shape: result_shape,
                                    };
                                }
                            }
                        }
                    }
                    "add" | "sub" | "mul" => {
                        if arg_types.len() == 2 {
                            let a_shape = self.get_tensor_shape(&arg_types[0]);
                            let b_shape = self.get_tensor_shape(&arg_types[1]);
                            if let (Some(a), Some(b)) = (&a_shape, &b_shape) {
                                if a.len() == b.len() {
                                    for (i, (da, db)) in a.iter().zip(b.iter()).enumerate() {
                                        if !self.shapes_compatible(da, db) {
                                            self.errors.push(format!(
                                                "Shape error: {} incompatible — dim {}: {:?} != {:?}",
                                                callee, i, da, db
                                            ));
                                        }
                                    }
                                    return arg_types[0].clone();
                                }
                            }
                        }
                    }
                    "transpose" => {
                        if arg_types.len() == 1 {
                            let shape = self.get_tensor_shape(&arg_types[0]);
                            if let Some(s) = &shape {
                                if s.len() == 2 {
                                    let mut rev = s.clone();
                                    rev.reverse();
                                    return Type::ShapedTensor {
                                        elem: Box::new(Type::F64),
                                        shape: rev,
                                    };
                                }
                            }
                        }
                    }
                    "reshape" => {
                        // reshape(t, new_shape) — result shape is new_shape
                        // Size must match, but we can't verify that at compile time
                        // without knowing the symbolic dims. Just return the new shape.
                        if arg_types.len() == 2 {
                            // The new shape is passed as a Vec<i64> — we can't extract it
                            // at type-check time. Return a generic Tensor.
                        }
                    }
                    "relu" | "sigmoid" | "softmax" | "scale" | "clone" => {
                        // Elementwise — result shape = input shape
                        if arg_types.len() >= 1 {
                            return arg_types[0].clone();
                        }
                    }
                    "sum" | "mean" | "max" | "min" | "dot" => {
                        // Reductions — result is scalar F64
                        if arg_types.len() >= 1 {
                            return Type::F64;
                        }
                    }
                    _ => {}
                }

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
                    // Check argument types match parameter types
                    // Only enforce strict checking for named struct types (e.g. SQLString)
                    // to prevent injection via typed strings. Primitive types (str, i64, etc.)
                    // are checked loosely since the IR represents strings as pointers (i64).
                    for (i, (param, arg)) in params.iter().zip(arg_types.iter()).enumerate() {
                        let param_is_named = matches!(self.resolve_type(param), Type::Named(_));
                        let is_generic_param = matches!(param, Type::Named(name) if name.len() == 1 && name.chars().next().unwrap().is_uppercase());
                        if param_is_named && !is_generic_param && !self.types_compatible(param, arg) {
                            self.errors.push(format!(
                                "Type error: function '{}' parameter {} expects '{}', got '{}'",
                                callee, i + 1, param, arg
                            ));
                        }
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
                    // String-returning builtins return str, not i64
                    match callee.as_str() {
                        "p_str_concat" | "p_to_str" | "p_float_to_str"
                        | "p_string_replace" | "p_string_trim"
                        | "p_string_to_uppercase" | "p_string_to_lowercase"
                        | "p_string_char_at" | "p_string_repeat" => {
                            return Type::Str;
                        }
                        "p_bool_to_str" => {
                            return Type::Str;
                        }
                        "p_string_contains" | "p_string_starts_with"
                        | "p_string_ends_with" | "p_string_is_empty" => {
                            return Type::Bool;
                        }
                        "p_string_len" | "p_string_find" | "p_string_rfind"
                        | "p_string_bytes" | "p_string_parse_int" => {
                            return Type::I64;
                        }
                        _ => {
                            // Builtins are variadic — return I64 as a safe default
                            return Type::I64;
                        }
                    }
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
                            // Check field privacy: private fields are only accessible
                            // from within their defining module
                            let is_pub = self.field_visibility
                                .get(&(name.clone(), field.clone()))
                                .copied()
                                .unwrap_or(true); // default to pub if not tracked
                            if !is_pub {
                                let struct_module = self.struct_modules.get(name).cloned().unwrap_or_default();
                                if struct_module != self.current_module {
                                    self.errors.push(format!(
                                        "Privacy error: field '{}' of struct '{}' is private.\n\
                                         \x20\x20\x20\x20It was defined in module '{}', which is not the current module '{}'.\n\
                                         \x20\x20\x20\x20Fix: mark the field as 'pub' in its definition, or access it only\n\
                                         \x20\x20\x20\x20from within module '{}'.",
                                        field, name, struct_module, self.current_module, struct_module
                                    ));
                                }
                            }
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
            Expr::OptionalFieldAccess { expr, field } => {
                // `expr?.field` — if expr is an Option wrapping a struct, unwrap and access field.
                // If None, returns 0 (i64).
                let obj_ty = self.check_expr(expr);
                // For now, treat it like field access — the lowerer handles the None check
                if let Type::Named(name) = &obj_ty {
                    if let Some(fields) = self.structs.get(name) {
                        if let Some((_, fty)) = fields.iter().find(|(n, _)| n == field) {
                            return fty.clone();
                        }
                    }
                }
                // Default: returns i64 (0 if None)
                Type::I64
            }
            Expr::StructLit { name, fields, spread } => {
                // Resolve type alias to actual struct name
                let real_name = self.resolve_struct_name(name);
                if let Some(struct_fields) = self.structs.get(&real_name) {
                    let struct_fields = struct_fields.clone();
                    // Check spread source type matches struct type
                    if let Some(spread_expr) = spread {
                        let spread_ty = self.check_expr(spread_expr);
                        if !self.types_compatible(&Type::Named(real_name.clone()), &spread_ty) {
                            self.errors.push(format!(
                                "Type error: spread source expected {}, got {}",
                                real_name, spread_ty
                            ));
                        }
                    }
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
                                            trait_name, method, expected_args, arg_types.len()
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
                } else if let Type::Generic { name: gen_name, .. } = &recv_ty {
                    // Generic type (e.g. Option<i64>): look up methods by the base name
                    if let Some(method_map) = self.methods.get(gen_name) {
                        if let Some((params, ret, has_self, is_pub)) = method_map.get(method) {
                            let expected_args = if *has_self { if params.is_empty() { 0 } else { params.len() - 1 } } else { params.len() };
                            if expected_args != arg_types.len() {
                                self.errors.push(format!(
                                    "Type error: method '{}.{}' expects {} args, got {}",
                                    gen_name, method, expected_args, arg_types.len()
                                ));
                            }
                            return ret.clone();
                        }
                    }
                    self.errors.push(format!(
                        "Type error: no method '{}' on generic type '{}'",
                        method, gen_name
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
            Expr::Ref { expr, is_mut } => {
                let inner_ty = self.check_expr(expr);
                Type::Ref { inner: Box::new(inner_ty), is_mut: *is_mut }
            }
            Expr::Deref { expr } => {
                let ty = self.check_expr(expr);
                match ty {
                    Type::Ref { inner, .. } => *inner,
                    Type::RawPtr { inner, .. } => *inner,
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
                        "Type error: string concat requires str or String.\n\
                         \x20\x20\x20\x20Left operand: {}\n\
                         \x20\x20\x20\x20Fix: convert the left operand to str (e.g. call .to_str() or use a string literal).",
                        lt
                    ));
                }
                if rt != Type::Str && rt != Type::Named("String".to_string()) {
                    self.errors.push(format!(
                        "Type error: string concat requires str or String.\n\
                         \x20\x20\x20\x20Right operand: {}\n\
                         \x20\x20\x20\x20Fix: convert the right operand to str (e.g. call .to_str() or use a string literal).",
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
                            "Type error: ? operator requires Option<T> or Result<T, E>.\n\
                             \x20\x20\x20\x20Found: {}\n\
                             \x20\x20\x20\x20The ? operator unwraps Option or Result — it propagates the error/None \
                             to the caller.\n\
                             \x20\x20\x20\x20Fix: wrap the value in Option or Result, or remove the ? operator.",
                            ty
                        ));
                        Type::Never
                    }
                }
            }
            Expr::Coalesce { expr, default } => {
                // ?? operator: expr ?? default
                // If expr is Option<T>, return T (unwrapped) or default (which must be T)
                let ty = self.check_expr(expr);
                let default_ty = self.check_expr(default);
                match &ty {
                    Type::Generic { name, args } if name == "Option" && args.len() == 1 => {
                        if !self.types_compatible(&args[0], &default_ty) {
                            self.errors.push(format!(
                                "Type error: ?? default value type mismatch.\n\
                                 \x20\x20\x20\x20Option inner type: {}\n\
                                 \x20\x20\x20\x20Default value type: {}\n\
                                 \x20\x20\x20\x20Fix: make the default value type {} to match the Option's inner type.",
                                args[0], default_ty, args[0]
                            ));
                        }
                        args[0].clone()
                    }
                    Type::Named(name) if name == "Option" => {
                        if !self.types_compatible(&Type::I64, &default_ty) {
                            self.errors.push(format!(
                                "Type error: ?? default value type mismatch.\n\
                                 \x20\x20\x20\x20Option inner type: i64\n\
                                 \x20\x20\x20\x20Default value type: {}\n\
                                 \x20\x20\x20\x20Fix: make the default value type i64 to match the Option's inner type.",
                                default_ty
                            ));
                        }
                        Type::I64
                    }
                    _ => {
                        self.errors.push(format!(
                            "Type error: ?? operator requires Option<T>.\n\
                             \x20\x20\x20\x20Found: {}\n\
                             \x20\x20\x20\x20The ?? operator unwraps an Option, returning the inner value \
                             or the default if None.\n\
                             \x20\x20\x20\x20Fix: wrap the value in Option, or remove the ?? operator.",
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
            Expr::IsCheck { expr, type_name } => {
                // `expr is Type` or `expr is Variant`
                // Returns bool. For enum variants, it's a runtime tag check.
                // For types, it's a compile-time type comparison.
                let expr_ty = self.check_expr(expr);

                // Check if type_name is a known type
                let is_type = matches!(type_name.as_str(),
                    "i8" | "i16" | "i32" | "i64" | "i128" |
                    "u8" | "u16" | "u32" | "u64" | "u128" |
                    "f32" | "f64" | "bool" | "str" | "void"
                ) || self.structs.contains_key(type_name) || self.enums.contains(type_name);

                if is_type {
                    // Compile-time type check — always evaluates to a constant bool
                    Type::Bool
                } else {
                    // Could be an enum variant name (e.g., "Some", "None")
                    // The typechecker only stores enum names, not variants.
                    // The lowerer will validate the variant at lowering time.
                    // For now, just accept any identifier as a potential variant.
                    Type::Bool
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
            Expr::PipeForward { left, right } => {
                // Typecheck left, then typecheck right as a function applied to left's type
                let left_ty = self.check_expr(left);
                // Desugar: if right is a Call, typecheck as call with left prepended to args
                match right.as_ref() {
                    Expr::Call { callee, args } => {
                        let mut all_args = vec![left.as_ref().clone()];
                        all_args.extend(args.iter().cloned());
                        let new_call = Expr::Call { callee: callee.clone(), args: all_args };
                        self.check_expr(&new_call)
                    }
                    Expr::MethodCall { receiver, method, args } => {
                        let new_call = Expr::MethodCall {
                            receiver: left.clone(),
                            method: method.clone(),
                            args: args.clone(),
                        };
                        let _ = receiver; // suppress unused warning
                        self.check_expr(&new_call)
                    }
                    Expr::Ident(name) => {
                        // x |> f  →  f(x)
                        let new_call = Expr::Call {
                            callee: name.clone(),
                            args: vec![left.as_ref().clone()],
                        };
                        self.check_expr(&new_call)
                    }
                    Expr::FieldAccess { expr: receiver, field } => {
                        // x |> obj.method  →  obj.method(x)
                        let new_call = Expr::MethodCall {
                            receiver: receiver.clone(),
                            method: field.clone(),
                            args: vec![left.as_ref().clone()],
                        };
                        self.check_expr(&new_call)
                    }
                    _ => {
                        // Fallback: typecheck right and return its type
                        let _ = left_ty;
                        self.check_expr(right)
                    }
                }
            }
            Expr::QuantCast { expr, target_ty } => {
                // Typecheck the inner expr, return the target type
                self.check_expr(expr);
                self.resolve_type(target_ty)
            }
            Expr::Await { expr: inner } => {
                // await Future<T> → T
                // Unwrap the Future<T> type to get the inner type T.
                let inner_ty = self.check_expr(inner);
                match &inner_ty {
                    Type::Generic { name, args } if name == "Future" && args.len() == 1 => {
                        args[0].clone()
                    }
                    Type::Named(name) if name == "Future" => {
                        // Bare Future without type parameter — default to I64
                        Type::I64
                    }
                    _ => {
                        self.errors.push(format!(
                            "Type error: await requires a Future<T> operand, got {}.\n\
                             \x20\x20\x20\x20Hint: async pf functions return Future<T>.\n\
                             \x20\x20\x20\x20     spawn <expr> also returns Future<T>.",
                            inner_ty
                        ));
                        Type::I64
                    }
                }
            }
            Expr::Spawn { expr: inner } => {
                // spawn <expr> → Future<T>
                // The inner expression should produce a Future<T> (e.g. by calling
                // an async function). spawn schedules it for concurrent execution
                // and returns the same Future<T>.
                let inner_ty = self.check_expr(inner);
                match &inner_ty {
                    Type::Generic { name, args } if name == "Future" && args.len() == 1 => {
                        // Already a Future<T> — spawn returns it as-is
                        inner_ty
                    }
                    Type::Named(name) if name == "Future" => {
                        // Bare Future — return as-is
                        inner_ty
                    }
                    _ => {
                        // Wrap non-Future in Future<T>
                        Type::Generic {
                            name: "Future".to_string(),
                            args: vec![inner_ty],
                        }
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

    /// Extract the shape from a tensor type. Returns None for untyped tensors.
    fn get_tensor_shape(&self, ty: &Type) -> Option<Vec<ShapeDim>> {
        match ty {
            Type::ShapedTensor { shape, .. } => Some(shape.clone()),
            Type::Generic { name, .. } if name == "Tensor" => None,
            Type::Named(name) if name == "Tensor" => None,
            _ => None,
        }
    }

    /// Check if two shape dimensions are compatible.
    /// Two Known dims are compatible if they're equal.
    /// A Known dim is compatible with a Symbolic dim (the symbolic dim is a wildcard).
    /// Two Symbolic dims are compatible if they have the same name.
    fn shapes_compatible(&self, a: &ShapeDim, b: &ShapeDim) -> bool {
        match (a, b) {
            (ShapeDim::Known(a), ShapeDim::Known(b)) => a == b,
            (ShapeDim::Known(_), ShapeDim::Symbolic(_)) => true,
            (ShapeDim::Symbolic(_), ShapeDim::Known(_)) => true,
            (ShapeDim::Symbolic(a), ShapeDim::Symbolic(b)) => a == b || a == "_" || b == "_",
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

    /// F68: Check if a type is a typed string (SQLString, ShellString, HTMLString,
    /// JSONString, PathString). These prevent injection attacks by disallowing
    /// string concatenation with +.
    fn is_typed_string(&self, ty: &Type) -> bool {
        match ty {
            Type::Named(name) => {
                matches!(
                    name.as_str(),
                    "SQLString" | "ShellString" | "HTMLString" | "JSONString" | "PathString"
                )
            }
            _ => false,
        }
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
        // Generic enum/struct types: Option is compatible with Option<T>, Result with Result<T, E>, etc.
        // This allows using the generic syntax (Option<i64>) with enums declared as generic.
        if let (Type::Named(decl_name), Type::Generic { name: gen_name, .. }) = (&declared, &actual) {
            if decl_name == gen_name { return true; }
        }
        if let (Type::Generic { name: gen_name, .. }, Type::Named(decl_name)) = (&declared, &actual) {
            if decl_name == gen_name { return true; }
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
        // Raw pointers are compatible with each other (same mutability + inner type)
        // and with i64 (pointers are stored as i64 internally)
        if let (Type::RawPtr { inner: d_inner, is_mut: d_mut },
                Type::RawPtr { inner: a_inner, is_mut: a_mut }) = (&declared, &actual) {
            return d_mut == a_mut && self.types_compatible(d_inner, a_inner);
        }
        // Allow i64 ↔ *mut T / *const T (pointers are i64 internally)
        if matches!(declared, Type::RawPtr { .. }) && self.is_numeric(&actual) {
            return true;
        }
        if matches!(actual, Type::RawPtr { .. }) && self.is_numeric(&declared) {
            return true;
        }
        // Allow &T → *mut T / *const T (ref to raw pointer coercion)
        if let (Type::RawPtr { inner: d_inner, .. },
                Type::Ref { inner: a_inner, .. }) = (&declared, &actual) {
            return self.types_compatible(d_inner, a_inner);
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
            Type::RawPtr { inner, .. } => self.type_contains_param(inner),
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

// ─── @deterministic helpers ───────────────────────────────────────────────

/// Check if a type contains F32 or F64 (directly or nested in generics/tuples).
fn type_contains_float(ty: &Type) -> bool {
    match ty {
        Type::F32 | Type::F64 => true,
        Type::Generic { args, .. } => args.iter().any(type_contains_float),
        Type::Tuple(types) => types.iter().any(type_contains_float),
        Type::Func { params, ret } => {
            params.iter().any(type_contains_float) || type_contains_float(ret)
        }
        _ => false,
    }
}

/// Count the gas cost of a list of statements (AST node count).
/// Each statement and sub-expression costs 1 gas unit.
fn count_gas_cost_stmts(stmts: &[Stmt]) -> u64 {
    stmts.iter().map(|s| count_gas_cost_stmt(s)).sum()
}

fn count_gas_cost_stmt(stmt: &Stmt) -> u64 {
    let mut cost = 1; // this statement
    match stmt {
        Stmt::Let { expr, .. } | Stmt::LetTuple { expr, .. } => cost += count_gas_cost_expr(expr),
        Stmt::Expr(e, _) => cost += count_gas_cost_expr(e),
        Stmt::Assign { expr, .. } => cost += count_gas_cost_expr(expr),
        Stmt::IndexAssign { index, expr, .. } => cost += count_gas_cost_expr(index) + count_gas_cost_expr(expr),
        Stmt::DerefAssign { ptr_expr, expr, .. } => cost += count_gas_cost_expr(ptr_expr) + count_gas_cost_expr(expr),
        Stmt::Return(e, _) => { if let Some(e) = e { cost += count_gas_cost_expr(e); } }
        Stmt::If { cond, then_block, else_block, .. } => {
            cost += count_gas_cost_expr(cond);
            cost += count_gas_cost_stmts(then_block);
            if let Some(eb) = else_block { cost += count_gas_cost_stmts(eb); }
        }
        Stmt::While { cond, body, .. } => {
            cost += count_gas_cost_expr(cond);
            cost += count_gas_cost_stmts(body);
        }
        Stmt::Loop { body, .. } => cost += count_gas_cost_stmts(body),
        Stmt::For { iterator, body, .. } => {
            cost += count_gas_cost_expr(iterator);
            cost += count_gas_cost_stmts(body);
        }
        Stmt::Defer(b, _) => cost += count_gas_cost_stmts(b),
        Stmt::Match { expr, arms, .. } => {
            cost += count_gas_cost_expr(expr);
            for arm in arms {
                cost += count_gas_cost_stmts(&arm.body);
            }
        }
        Stmt::RegionBlock { body, .. } | Stmt::Sandbox { body, .. } | Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            cost += count_gas_cost_stmts(body);
        }
        Stmt::GradBlock { body, .. } => cost += count_gas_cost_stmts(body),
        Stmt::ComptimeGen { block, .. } => cost += count_gas_cost_stmts(block),
        _ => {}
    }
    cost
}

fn count_gas_cost_expr(expr: &Expr) -> u64 {
    let mut cost = 1; // this expression
    match expr {
        Expr::Binary { left, right, .. } => cost += count_gas_cost_expr(left) + count_gas_cost_expr(right),
        Expr::Unary { expr, .. } => cost += count_gas_cost_expr(expr),
        Expr::Call { args, .. } => { for a in args { cost += count_gas_cost_expr(a); } }
        Expr::MethodCall { receiver, args, .. } => {
            cost += count_gas_cost_expr(receiver);
            for a in args { cost += count_gas_cost_expr(a); }
        }
        Expr::FieldAccess { expr, .. } => cost += count_gas_cost_expr(expr),
        Expr::Index { index, .. } => cost += count_gas_cost_expr(index),
        Expr::StructLit { fields, spread, .. } => {
            for (_, e) in fields { cost += count_gas_cost_expr(e); }
            if let Some(s) = spread { cost += count_gas_cost_expr(s); }
        }
        Expr::ArrayLit { elements, .. } => { for e in elements { cost += count_gas_cost_expr(e); } }
        Expr::TupleLit(elems) => { for e in elems { cost += count_gas_cost_expr(e); } }
        Expr::Ref { expr, .. } => cost += count_gas_cost_expr(expr),
        Expr::Deref { expr } => cost += count_gas_cost_expr(expr),
        Expr::Cast { expr, .. } => cost += count_gas_cost_expr(expr),
        Expr::If { cond, then_block, else_block } => {
            cost += count_gas_cost_expr(cond);
            cost += count_gas_cost_stmts(then_block);
            if let Some(eb) = else_block { cost += count_gas_cost_stmts(eb); }
        }
        Expr::Block(stmts) => cost += count_gas_cost_stmts(stmts),
        Expr::Match { expr, arms } => {
            cost += count_gas_cost_expr(expr);
            for arm in arms { cost += count_gas_cost_stmts(&arm.body); }
        }
        Expr::Coalesce { expr, default } => cost += count_gas_cost_expr(expr) + count_gas_cost_expr(default),
        Expr::Try { expr } => cost += count_gas_cost_expr(expr),
        Expr::StrConcat { left, right } => cost += count_gas_cost_expr(left) + count_gas_cost_expr(right),
        Expr::Range { start, end, .. } => cost += count_gas_cost_expr(start) + count_gas_cost_expr(end),
        _ => {}
    }
    cost
}

/// Compute total gas cost (own + transitive callees) with cycle detection.
fn compute_total_gas(
    name: &str,
    own_costs: &HashMap<String, u64>,
    call_graph: &HashMap<String, HashSet<String>>,
    visited: &mut HashSet<String>,
    cache: &mut HashMap<String, u64>,
) -> u64 {
    if let Some(&cached) = cache.get(name) {
        return cached;
    }
    // Cycle detection — if we've already visited this node in the current
    // traversal, return its own cost (don't recurse into the cycle).
    if visited.contains(name) {
        return own_costs.get(name).copied().unwrap_or(0);
    }
    visited.insert(name.to_string());

    let own = own_costs.get(name).copied().unwrap_or(0);
    let callee_total: u64 = call_graph.get(name)
        .map(|calls| {
            calls.iter()
                .map(|c| compute_total_gas(c, own_costs, call_graph, visited, cache))
                .sum()
        })
        .unwrap_or(0);

    let total = own + callee_total;
    cache.insert(name.to_string(), total);
    total
}

/// Scan function body statements for all function calls.
/// Collects both direct calls (callee name) and method calls (Type::method).
fn scan_stmts_for_calls(stmts: &[Stmt], calls: &mut HashSet<String>) {
    for stmt in stmts {
        scan_stmt_for_calls(stmt, calls);
    }
}

fn scan_stmt_for_calls(stmt: &Stmt, calls: &mut HashSet<String>) {
    match stmt {
        Stmt::Let { expr, .. } => scan_expr_for_calls(expr, calls),
        Stmt::LetTuple { expr, .. } => scan_expr_for_calls(expr, calls),
        Stmt::Expr(e, _) => scan_expr_for_calls(e, calls),
        Stmt::Assign { expr, .. } => scan_expr_for_calls(expr, calls),
        Stmt::IndexAssign { index, expr, .. } => {
            scan_expr_for_calls(index, calls);
            scan_expr_for_calls(expr, calls);
        }
        Stmt::DerefAssign { ptr_expr, expr, .. } => {
            scan_expr_for_calls(ptr_expr, calls);
            scan_expr_for_calls(expr, calls);
        }
        Stmt::Return(e, _) => {
            if let Some(e) = e { scan_expr_for_calls(e, calls); }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            scan_expr_for_calls(cond, calls);
            for s in then_block { scan_stmt_for_calls(s, calls); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_calls(s, calls); }
            }
        }
        Stmt::While { cond, body, .. } => {
            scan_expr_for_calls(cond, calls);
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::For { iterator, body, .. } => {
            scan_expr_for_calls(iterator, calls);
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Loop { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Break(e, _) => {
            if let Some(e) = e { scan_expr_for_calls(e, calls); }
        }
        Stmt::Defer(stmts, _) => {
            for s in stmts { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Match { expr, arms, .. } => {
            scan_expr_for_calls(expr, calls);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_calls(s, calls); }
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
            for h in handlers {
                for s in &h.body { scan_stmt_for_calls(s, calls); }
            }
        }
        Stmt::RegionBlock { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Sandbox { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Stmt::Backward { loss, .. } => {
            scan_expr_for_calls(loss, calls);
        }
        _ => {}
    }
}

fn scan_expr_for_calls(expr: &Expr, calls: &mut HashSet<String>) {
    match expr {
        Expr::Call { callee, args } => {
            calls.insert(callee.clone());
            for a in args { scan_expr_for_calls(a, calls); }
        }
        Expr::MethodCall { receiver, method, args } => {
            // For method calls, we need the type name to qualify.
            // Since we don't have it here, we insert both the bare name
            // and let the caller check. The caller will need to resolve.
            // For now, insert the method name as "Type::method" if we can
            // infer the type, or just the bare method name.
            calls.insert(method.clone());
            scan_expr_for_calls(receiver, calls);
            for a in args { scan_expr_for_calls(a, calls); }
        }
        Expr::Binary { left, right, .. } => {
            scan_expr_for_calls(left, calls);
            scan_expr_for_calls(right, calls);
        }
        Expr::Unary { expr, .. } => scan_expr_for_calls(expr, calls),
        Expr::FieldAccess { expr, .. } => scan_expr_for_calls(expr, calls),
        Expr::OptionalFieldAccess { expr, .. } => scan_expr_for_calls(expr, calls),
        Expr::StructLit { fields, spread, .. } => {
            for (_, e) in fields { scan_expr_for_calls(e, calls); }
            if let Some(s) = spread { scan_expr_for_calls(s, calls); }
        }
        Expr::If { cond, then_block, else_block, .. } => {
            scan_expr_for_calls(cond, calls);
            for s in then_block { scan_stmt_for_calls(s, calls); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_calls(s, calls); }
            }
        }
        Expr::ArrayLit { elements, .. } => {
            for e in elements { scan_expr_for_calls(e, calls); }
        }
        Expr::Index { array, index } => {
            scan_expr_for_calls(array, calls);
            scan_expr_for_calls(index, calls);
        }
        Expr::TupleLit(elements) => {
            for e in elements { scan_expr_for_calls(e, calls); }
        }
        Expr::Ref { expr, .. } => scan_expr_for_calls(expr, calls),
        Expr::Deref { expr } => scan_expr_for_calls(expr, calls),
        Expr::Cast { expr, .. } => scan_expr_for_calls(expr, calls),
        Expr::Coalesce { expr, default } => {
            scan_expr_for_calls(expr, calls);
            scan_expr_for_calls(default, calls);
        }
        Expr::Try { expr } => scan_expr_for_calls(expr, calls),
        Expr::StrConcat { left, right } => {
            scan_expr_for_calls(left, calls);
            scan_expr_for_calls(right, calls);
        }
        Expr::Range { start, end, .. } => {
            scan_expr_for_calls(start, calls);
            scan_expr_for_calls(end, calls);
        }
        Expr::Match { expr, arms, .. } => {
            scan_expr_for_calls(expr, calls);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_calls(s, calls); }
            }
        }
        Expr::TryBlock { body } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Expr::Block(stmts) => {
            for s in stmts { scan_stmt_for_calls(s, calls); }
        }
        Expr::Closure { body, .. } => {
            for s in body { scan_stmt_for_calls(s, calls); }
        }
        Expr::Comptime(e) => scan_expr_for_calls(e, calls),
        Expr::ComptimeBlock(stmts) => {
            for s in stmts { scan_stmt_for_calls(s, calls); }
        }
        _ => {}
    }
}

/// Scan function body statements for floating-point usage.
/// Returns a list of descriptions of where floats were found.
fn scan_for_floats(stmts: &[Stmt]) -> Vec<String> {
    let mut found = Vec::new();
    for stmt in stmts {
        scan_stmt_for_floats(stmt, &mut found);
    }
    found
}

fn scan_stmt_for_floats(stmt: &Stmt, found: &mut Vec<String>) {
    match stmt {
        Stmt::Let { name, ty, expr, line, .. } => {
            if let Some(t) = ty {
                if type_contains_float(t) {
                    found.push(format!("variable '{}: {}' (line {})", name, t, line));
                }
            }
            scan_expr_for_floats(expr, found);
        }
        Stmt::LetTuple { expr, .. } => scan_expr_for_floats(expr, found),
        Stmt::Expr(e, _) => scan_expr_for_floats(e, found),
        Stmt::Assign { expr, .. } => scan_expr_for_floats(expr, found),
        Stmt::IndexAssign { index, expr, .. } => {
            scan_expr_for_floats(index, found);
            scan_expr_for_floats(expr, found);
        }
        Stmt::DerefAssign { expr, .. } => scan_expr_for_floats(expr, found),
        Stmt::Return(e, _) => {
            if let Some(e) = e { scan_expr_for_floats(e, found); }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            scan_expr_for_floats(cond, found);
            for s in then_block { scan_stmt_for_floats(s, found); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_floats(s, found); }
            }
        }
        Stmt::While { cond, body, .. } => {
            scan_expr_for_floats(cond, found);
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::For { iterator, body, .. } => {
            scan_expr_for_floats(iterator, found);
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::Loop { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::Break(e, _) => {
            if let Some(e) = e { scan_expr_for_floats(e, found); }
        }
        Stmt::Defer(stmts, _) => {
            for s in stmts { scan_stmt_for_floats(s, found); }
        }
        Stmt::Match { expr, arms, .. } => {
            scan_expr_for_floats(expr, found);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_floats(s, found); }
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
            for h in handlers {
                for s in &h.body { scan_stmt_for_floats(s, found); }
            }
        }
        Stmt::RegionBlock { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::Sandbox { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Stmt::Backward { loss, .. } => {
            scan_expr_for_floats(loss, found);
        }
        Stmt::Do { args, .. } => {
            for a in args { scan_expr_for_floats(a, found); }
        }
        Stmt::ComptimeGen { block, .. } => {
            for s in block { scan_stmt_for_floats(s, found); }
        }
        Stmt::Use { .. } | Stmt::Continue(_) => {}
        _ => {}
    }
}

fn scan_expr_for_floats(expr: &Expr, found: &mut Vec<String>) {
    match expr {
        Expr::FloatLit(_) => {
            found.push("float literal".to_string());
        }
        Expr::Binary { left, right, .. } => {
            scan_expr_for_floats(left, found);
            scan_expr_for_floats(right, found);
        }
        Expr::Unary { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::Call { args, .. } => {
            for a in args { scan_expr_for_floats(a, found); }
        }
        Expr::MethodCall { receiver, args, .. } => {
            scan_expr_for_floats(receiver, found);
            for a in args { scan_expr_for_floats(a, found); }
        }
        Expr::FieldAccess { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::OptionalFieldAccess { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::Index { array, index } => {
            scan_expr_for_floats(array, found);
            scan_expr_for_floats(index, found);
        }
        Expr::TupleLit(exprs) => {
            for e in exprs { scan_expr_for_floats(e, found); }
        }
        Expr::Cast { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::StructLit { fields, spread, .. } => {
            for (_, e) in fields { scan_expr_for_floats(e, found); }
            if let Some(s) = spread { scan_expr_for_floats(s, found); }
        }
        Expr::If { cond, then_block, else_block } => {
            scan_expr_for_floats(cond, found);
            for s in then_block { scan_stmt_for_floats(s, found); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_floats(s, found); }
            }
        }
        Expr::Range { start, end, .. } => {
            scan_expr_for_floats(start, found);
            scan_expr_for_floats(end, found);
        }
        Expr::Block(stmts) => {
            for s in stmts { scan_stmt_for_floats(s, found); }
        }
        Expr::ArrayLit { elements, .. } => {
            for e in elements { scan_expr_for_floats(e, found); }
        }
        Expr::Match { expr, arms } => {
            scan_expr_for_floats(expr, found);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_floats(s, found); }
            }
        }
        Expr::EnumLit { args, .. } => {
            for a in args { scan_expr_for_floats(a, found); }
        }
        Expr::Ref { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::Deref { expr } => scan_expr_for_floats(expr, found),
        Expr::StrConcat { left, right } => {
            scan_expr_for_floats(left, found);
            scan_expr_for_floats(right, found);
        }
        Expr::Try { expr } => scan_expr_for_floats(expr, found),
        Expr::Coalesce { expr, default } => {
            scan_expr_for_floats(expr, found);
            scan_expr_for_floats(default, found);
        }
        Expr::IsCheck { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::TryBlock { body } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Expr::Closure { body, .. } => {
            for s in body { scan_stmt_for_floats(s, found); }
        }
        Expr::Comptime(e) => scan_expr_for_floats(e, found),
        Expr::ComptimeBlock(stmts) => {
            for s in stmts { scan_stmt_for_floats(s, found); }
        }
        Expr::PipeForward { left, right } => {
            scan_expr_for_floats(left, found);
            scan_expr_for_floats(right, found);
        }
        Expr::QuantCast { expr, .. } => scan_expr_for_floats(expr, found),
        Expr::Asm { .. } => {}
        Expr::ComptimeReflect { .. } => {}
        Expr::FString { .. } => {}
        _ => {}
    }
}

// ─── @crashfree helpers ───

/// Scan statements for division/modulo operations.
/// Returns (line, operator_string) for each div/mod site.
fn scan_for_division(stmts: &[Stmt], sites: &mut Vec<(usize, &'static str)>) {
    for stmt in stmts {
        scan_stmt_for_division(stmt, sites);
    }
}

fn scan_stmt_for_division(stmt: &Stmt, sites: &mut Vec<(usize, &'static str)>) {
    match stmt {
        Stmt::Let { expr, .. } | Stmt::LetTuple { expr, .. } => scan_expr_for_division(expr, sites),
        Stmt::Expr(e, _) => scan_expr_for_division(e, sites),
        Stmt::Assign { expr, .. } => scan_expr_for_division(expr, sites),
        Stmt::IndexAssign { index, expr, .. } => {
            scan_expr_for_division(index, sites);
            scan_expr_for_division(expr, sites);
        }
        Stmt::DerefAssign { ptr_expr, expr, .. } => {
            scan_expr_for_division(ptr_expr, sites);
            scan_expr_for_division(expr, sites);
        }
        Stmt::Return(e, _) => {
            if let Some(e) = e { scan_expr_for_division(e, sites); }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            scan_expr_for_division(cond, sites);
            for s in then_block { scan_stmt_for_division(s, sites); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_division(s, sites); }
            }
        }
        Stmt::While { cond, body, .. } => {
            scan_expr_for_division(cond, sites);
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Stmt::For { iterator, body, .. } => {
            scan_expr_for_division(iterator, sites);
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Stmt::Loop { body, .. } => {
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Stmt::Defer(stmts, _) => {
            for s in stmts { scan_stmt_for_division(s, sites); }
        }
        Stmt::Match { expr, arms, .. } => {
            scan_expr_for_division(expr, sites);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_division(s, sites); }
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            for s in body { scan_stmt_for_division(s, sites); }
            for h in handlers {
                for s in &h.body { scan_stmt_for_division(s, sites); }
            }
        }
        Stmt::RegionBlock { body, .. } | Stmt::Sandbox { body, .. } | Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Stmt::Backward { loss, .. } => scan_expr_for_division(loss, sites),
        Stmt::ComptimeGen { block, .. } => {
            for s in block { scan_stmt_for_division(s, sites); }
        }
        _ => {}
    }
}

fn scan_expr_for_division(expr: &Expr, sites: &mut Vec<(usize, &'static str)>) {
    match expr {
        Expr::Binary { op, left, right } => {
            if matches!(op, BinOp::Div | BinOp::Mod) {
                // Check if divisor is a non-zero constant — if so, it's safe
                let is_safe = matches!(right.as_ref(), Expr::IntLit(n) if *n != 0);
                if !is_safe {
                    let op_str = if matches!(op, BinOp::Div) { "/" } else { "%" };
                    sites.push((0, op_str));
                }
            }
            scan_expr_for_division(left, sites);
            scan_expr_for_division(right, sites);
        }
        Expr::Unary { expr, .. } => scan_expr_for_division(expr, sites),
        Expr::Call { args, .. } => {
            for a in args { scan_expr_for_division(a, sites); }
        }
        Expr::MethodCall { receiver, args, .. } => {
            scan_expr_for_division(receiver, sites);
            for a in args { scan_expr_for_division(a, sites); }
        }
        Expr::FieldAccess { expr, .. } => scan_expr_for_division(expr, sites),
        Expr::Index { array, index } => {
            scan_expr_for_division(array, sites);
            scan_expr_for_division(index, sites);
        }
        Expr::StructLit { fields, spread, .. } => {
            for (_, e) in fields { scan_expr_for_division(e, sites); }
            if let Some(s) = spread { scan_expr_for_division(s, sites); }
        }
        Expr::ArrayLit { elements, .. } => {
            for e in elements { scan_expr_for_division(e, sites); }
        }
        Expr::TupleLit(elems) => {
            for e in elems { scan_expr_for_division(e, sites); }
        }
        Expr::Ref { expr, .. } => scan_expr_for_division(expr, sites),
        Expr::Deref { expr } => scan_expr_for_division(expr, sites),
        Expr::Cast { expr, .. } => scan_expr_for_division(expr, sites),
        Expr::If { cond, then_block, else_block } => {
            scan_expr_for_division(cond, sites);
            for s in then_block { scan_stmt_for_division(s, sites); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_division(s, sites); }
            }
        }
        Expr::Block(stmts) => {
            for s in stmts { scan_stmt_for_division(s, sites); }
        }
        Expr::Match { expr, arms } => {
            scan_expr_for_division(expr, sites);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_division(s, sites); }
            }
        }
        Expr::StrConcat { left, right } => {
            scan_expr_for_division(left, sites);
            scan_expr_for_division(right, sites);
        }
        Expr::Try { expr } => scan_expr_for_division(expr, sites),
        Expr::Coalesce { expr, default } => {
            scan_expr_for_division(expr, sites);
            scan_expr_for_division(default, sites);
        }
        Expr::IsCheck { expr, .. } => scan_expr_for_division(expr, sites),
        Expr::TryBlock { body } => {
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Expr::Closure { body, .. } => {
            for s in body { scan_stmt_for_division(s, sites); }
        }
        Expr::Comptime(e) => scan_expr_for_division(e, sites),
        Expr::ComptimeBlock(stmts) => {
            for s in stmts { scan_stmt_for_division(s, sites); }
        }
        Expr::PipeForward { left, right } => {
            scan_expr_for_division(left, sites);
            scan_expr_for_division(right, sites);
        }
        Expr::QuantCast { expr, .. } => scan_expr_for_division(expr, sites),
        _ => {}
    }
}

/// Scan statements for `while` loops (unbounded iteration risk).
/// Returns line numbers of each while loop.
fn scan_for_while_loops(stmts: &[Stmt], sites: &mut Vec<usize>) {
    for stmt in stmts {
        scan_stmt_for_while(stmt, sites);
    }
}

fn scan_stmt_for_while(stmt: &Stmt, sites: &mut Vec<usize>) {
    match stmt {
        Stmt::While { cond, body, line } => {
            sites.push(*line);
            scan_expr_for_while(cond, sites);
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Stmt::If { cond, then_block, else_block, .. } => {
            scan_expr_for_while(cond, sites);
            for s in then_block { scan_stmt_for_while(s, sites); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_while(s, sites); }
            }
        }
        Stmt::For { iterator, body, .. } => {
            scan_expr_for_while(iterator, sites);
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Stmt::Loop { body, .. } => {
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Stmt::Defer(stmts, _) => {
            for s in stmts { scan_stmt_for_while(s, sites); }
        }
        Stmt::Match { expr, arms, .. } => {
            scan_expr_for_while(expr, sites);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_while(s, sites); }
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            for s in body { scan_stmt_for_while(s, sites); }
            for h in handlers {
                for s in &h.body { scan_stmt_for_while(s, sites); }
            }
        }
        Stmt::RegionBlock { body, .. } | Stmt::Sandbox { body, .. } | Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Stmt::ComptimeGen { block, .. } => {
            for s in block { scan_stmt_for_while(s, sites); }
        }
        Stmt::Let { expr, .. } | Stmt::LetTuple { expr, .. } => scan_expr_for_while(expr, sites),
        Stmt::Expr(e, _) => scan_expr_for_while(e, sites),
        Stmt::Assign { expr, .. } => scan_expr_for_while(expr, sites),
        Stmt::Return(e, _) => {
            if let Some(e) = e { scan_expr_for_while(e, sites); }
        }
        _ => {}
    }
}

fn scan_expr_for_while(expr: &Expr, sites: &mut Vec<usize>) {
    match expr {
        Expr::Binary { left, right, .. } => {
            scan_expr_for_while(left, sites);
            scan_expr_for_while(right, sites);
        }
        Expr::Unary { expr, .. } => scan_expr_for_while(expr, sites),
        Expr::Call { args, .. } => {
            for a in args { scan_expr_for_while(a, sites); }
        }
        Expr::MethodCall { receiver, args, .. } => {
            scan_expr_for_while(receiver, sites);
            for a in args { scan_expr_for_while(a, sites); }
        }
        Expr::FieldAccess { expr, .. } => scan_expr_for_while(expr, sites),
        Expr::Index { array, index } => {
            scan_expr_for_while(array, sites);
            scan_expr_for_while(index, sites);
        }
        Expr::If { cond, then_block, else_block } => {
            scan_expr_for_while(cond, sites);
            for s in then_block { scan_stmt_for_while(s, sites); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_while(s, sites); }
            }
        }
        Expr::Block(stmts) => {
            for s in stmts { scan_stmt_for_while(s, sites); }
        }
        Expr::Match { expr, arms } => {
            scan_expr_for_while(expr, sites);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_while(s, sites); }
            }
        }
        Expr::TryBlock { body } => {
            for s in body { scan_stmt_for_while(s, sites); }
        }
        Expr::Closure { body, .. } => {
            for s in body { scan_stmt_for_while(s, sites); }
        }
        _ => {}
    }
}

// ─── F10: Contract verification helpers ─────────────────────────────────

/// Scan statements for call sites: (callee_name, arg_exprs, line).
/// Unlike scan_stmts_for_calls (which collects just names), this collects
/// the actual argument expressions so we can evaluate preconditions.
fn scan_stmts_for_call_sites(stmts: &[Stmt], calls: &mut Vec<(String, Vec<Expr>, usize)>) {
    for stmt in stmts {
        scan_stmt_for_call_sites(stmt, calls);
    }
}

fn scan_stmt_for_call_sites(stmt: &Stmt, calls: &mut Vec<(String, Vec<Expr>, usize)>) {
    match stmt {
        Stmt::Let { expr, line, .. } => { scan_expr_for_call_sites(expr, calls, *line); }
        Stmt::LetUninit { .. } => {}
        Stmt::Assign { expr, line, .. } => scan_expr_for_call_sites(expr, calls, *line),
        Stmt::IndexAssign { expr, line, .. } => scan_expr_for_call_sites(expr, calls, *line),
        Stmt::DerefAssign { expr, line, .. } => scan_expr_for_call_sites(expr, calls, *line),
        Stmt::Expr(expr, line) => scan_expr_for_call_sites(expr, calls, *line),
        Stmt::Return(expr, line) => {
            if let Some(e) = expr { scan_expr_for_call_sites(e, calls, *line); }
        }
        Stmt::If { cond, then_block, else_block, line, .. } => {
            scan_expr_for_call_sites(cond, calls, *line);
            for s in then_block { scan_stmt_for_call_sites(s, calls); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_call_sites(s, calls); }
            }
        }
        Stmt::While { cond, body, line, .. } => {
            scan_expr_for_call_sites(cond, calls, *line);
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::For { iterator, body, line, .. } => {
            scan_expr_for_call_sites(iterator, calls, *line);
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Loop { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Break(e, _) => {
            if let Some(e) = e { scan_expr_for_call_sites(e, calls, 0); }
        }
        Stmt::Defer(stmts, _) => {
            for s in stmts { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Match { expr, arms, line, .. } => {
            scan_expr_for_call_sites(expr, calls, *line);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_call_sites(s, calls); }
            }
        }
        Stmt::Handle { body, handlers, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
            for h in handlers {
                for s in &h.body { scan_stmt_for_call_sites(s, calls); }
            }
        }
        Stmt::RegionBlock { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Sandbox { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::GradBlock { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Stmt::Backward { loss, line, .. } => {
            scan_expr_for_call_sites(loss, calls, *line);
        }
        _ => {}
    }
}

fn scan_expr_for_call_sites(expr: &Expr, calls: &mut Vec<(String, Vec<Expr>, usize)>, line: usize) {
    match expr {
        Expr::Call { callee, args } => {
            calls.push((callee.clone(), args.clone(), line));
            for arg in args {
                scan_expr_for_call_sites(arg, calls, line);
            }
        }
        Expr::Binary { left, right, .. } => {
            scan_expr_for_call_sites(left, calls, line);
            scan_expr_for_call_sites(right, calls, line);
        }
        Expr::Unary { expr, .. } => scan_expr_for_call_sites(expr, calls, line),
        Expr::Index { array, index, .. } => {
            scan_expr_for_call_sites(array, calls, line);
            scan_expr_for_call_sites(index, calls, line);
        }
        Expr::FieldAccess { expr, .. } => scan_expr_for_call_sites(expr, calls, line),
        Expr::MethodCall { receiver, args, .. } => {
            scan_expr_for_call_sites(receiver, calls, line);
            for arg in args { scan_expr_for_call_sites(arg, calls, line); }
        }
        Expr::If { cond, then_block, else_block, .. } => {
            scan_expr_for_call_sites(cond, calls, line);
            for s in then_block { scan_stmt_for_call_sites(s, calls); }
            if let Some(eb) = else_block {
                for s in eb { scan_stmt_for_call_sites(s, calls); }
            }
        }
        Expr::Match { expr, arms, .. } => {
            scan_expr_for_call_sites(expr, calls, line);
            for arm in arms {
                for s in &arm.body { scan_stmt_for_call_sites(s, calls); }
            }
        }
        Expr::TryBlock { body } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        Expr::Block(stmts) => {
            for s in stmts { scan_stmt_for_call_sites(s, calls); }
        }
        Expr::Closure { body, .. } => {
            for s in body { scan_stmt_for_call_sites(s, calls); }
        }
        _ => {}
    }
}

/// Try to evaluate a contract expression with the given parameter bindings.
/// Returns Some(true) if the contract is satisfied, Some(false) if violated,
/// or None if we can't evaluate it (non-constant arguments).
///
/// This handles simple cases:
/// - `param >= N`, `param <= N`, `param > N`, `param < N`, `param == N`, `param != N`
/// - `param >= N && param <= M` (conjunction)
/// - `param >= N || param <= M` (disjunction)
fn eval_contract(expr: &Expr, params: &[String], args: &[Expr]) -> Option<bool> {
    // Build a binding of param name → constant value (if arg is a constant)
    let mut bindings: HashMap<String, i64> = HashMap::new();
    for (i, param) in params.iter().enumerate() {
        if let Some(arg) = args.get(i) {
            if let Some(val) = eval_const_int(arg) {
                bindings.insert(param.clone(), val);
            }
        }
    }

    eval_contract_expr(expr, &bindings)
}

/// Evaluate an expression as a constant integer.
/// Returns None if the expression is not a compile-time constant.
fn eval_const_int(expr: &Expr) -> Option<i64> {
    match expr {
        Expr::IntLit(n) => Some(*n),
        Expr::Unary { op: UnaryOp::Neg, expr, .. } => {
            eval_const_int(expr).map(|v| -v)
        }
        Expr::Binary { op, left, right, .. } => {
            let l = eval_const_int(left)?;
            let r = eval_const_int(right)?;
            match op {
                BinOp::Add => l.checked_add(r),
                BinOp::Sub => l.checked_sub(r),
                BinOp::Mul => l.checked_mul(r),
                BinOp::Div => if r != 0 { Some(l / r) } else { None },
                BinOp::Mod => if r != 0 { Some(l % r) } else { None },
                _ => None,
            }
        }
        _ => None,
    }
}

/// Evaluate a contract expression with the given bindings.
/// Returns Some(true/false) if we can determine the result, None if unknown.
fn eval_contract_expr(expr: &Expr, bindings: &HashMap<String, i64>) -> Option<bool> {
    match expr {
        Expr::Binary { op, left, right, .. } => {
            match op {
                BinOp::And => {
                    // Both must be true (short-circuit)
                    let l = eval_contract_expr(left, bindings)?;
                    if !l { return Some(false); }
                    eval_contract_expr(right, bindings)
                }
                BinOp::Or => {
                    // Either can be true (short-circuit)
                    let l = eval_contract_expr(left, bindings)?;
                    if l { return Some(true); }
                    eval_contract_expr(right, bindings)
                }
                BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq | BinOp::Eq | BinOp::Neq => {
                    let l = eval_expr_with_bindings(left, bindings)?;
                    let r = eval_expr_with_bindings(right, bindings)?;
                    let result = match op {
                        BinOp::Lt => l < r,
                        BinOp::Gt => l > r,
                        BinOp::LtEq => l <= r,
                        BinOp::GtEq => l >= r,
                        BinOp::Eq => l == r,
                        BinOp::Neq => l != r,
                        _ => return None,
                    };
                    Some(result)
                }
                _ => None,
            }
        }
        Expr::BoolLit(b) => Some(*b),
        _ => None,
    }
}

/// Evaluate an expression that may reference bound variables.
/// Returns the integer value, or None if it can't be evaluated.
fn eval_expr_with_bindings(expr: &Expr, bindings: &HashMap<String, i64>) -> Option<i64> {
    match expr {
        Expr::IntLit(n) => Some(*n),
        Expr::Ident(name) => bindings.get(name).copied(),
        Expr::Unary { op: UnaryOp::Neg, expr, .. } => {
            eval_expr_with_bindings(expr, bindings).map(|v| -v)
        }
        Expr::Binary { op, left, right, .. } => {
            let l = eval_expr_with_bindings(left, bindings)?;
            let r = eval_expr_with_bindings(right, bindings)?;
            match op {
                BinOp::Add => l.checked_add(r),
                BinOp::Sub => l.checked_sub(r),
                BinOp::Mul => l.checked_mul(r),
                BinOp::Div => if r != 0 { Some(l / r) } else { None },
                BinOp::Mod => if r != 0 { Some(l % r) } else { None },
                _ => None,
            }
        }
        _ => None,
    }
}
