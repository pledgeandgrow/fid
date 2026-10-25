// AST → Fides IR lowering pass
//
// This module lowers the typed AST into Fides IR (SSA form).
// It handles:
// - Function lowering with basic blocks and control flow
// - Expression lowering to SSA values
// - Statement lowering with proper block management
// - Struct layout computation
// - Effect propagation
//
// The lowering is designed to produce correct SSA even with
// mutable variables (via alloca + load/store pattern initially,
// to be optimized later by mem2reg pass).

use crate::ast::*;
use crate::ir::*;
use std::collections::HashMap;

/// Determine effects for runtime builtin functions (p_* functions).
/// Functions that mutate data have Write effect; functions that allocate
/// have Alloc effect; IO functions have IO effect. This prevents the
/// optimizer from treating side-effecting runtime calls as pure.
fn runtime_effects(name: &str) -> Option<EffectSet> {
    let mut effects = EffectSet::new();
    // Mutating vec operations
    if matches!(name,
        "p_vec_push" | "p_vec_push_str" | "p_vec_set" | "p_vec_clear"
        | "p_vec_insert" | "p_vec_remove" | "p_vec_reverse" | "p_vec_extend"
        | "p_vec_truncate" | "p_vec_sort" | "p_vec_resize"
        | "p_memset" | "p_str_zeroize" | "p_vec_zeroize"
    ) {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Allocating functions
    if matches!(name,
        "p_vec_new" | "p_vec_new_with" | "p_vec_clone"
        | "p_malloc" | "p_alloc" | "p_free" | "p_arena_save" | "p_arena_restore"
        | "p_str_concat" | "p_to_str" | "p_char_to_str" | "p_float_to_str" | "p_bool_to_str" | "p_str_to_float"
        | "p_string_replace" | "p_string_split" | "p_string_trim"
        | "p_string_to_uppercase" | "p_string_to_lowercase"
    ) {
        effects.add(Effect::Alloc);
        return Some(effects);
    }
    // IO functions (print, eprint, panic, etc.)
    if name.starts_with("p_print") || name.starts_with("p_eprint") || name == "p_panic" || name.starts_with("p_write") {
        effects.add(Effect::IO);
        return Some(effects);
    }
    // Map mutation
    if matches!(name,
        "p_map_insert" | "p_map_remove" | "p_map_clear" | "p_map_set"
    ) {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // String mutation
    if matches!(name, "p_string_copy" | "p_str_copy" | "p_str_free") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Atomic operations
    if name.starts_with("p_atomic_") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Channel operations
    if name.starts_with("p_chan_") {
        effects.add(Effect::IO);
        return Some(effects);
    }
    // Mutex operations
    if name.starts_with("p_mutex_") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // RC operations
    if name.starts_with("p_rc_") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Tensor operations (may allocate)
    if name.starts_with("p_tensor_") {
        effects.add(Effect::Alloc);
        return Some(effects);
    }
    // File/IO operations
    if name.starts_with("p_file_") || name.starts_with("p_fs_") || name.starts_with("p_socket_") {
        effects.add(Effect::IO);
        return Some(effects);
    }
    // DL operations
    if name.starts_with("p_dl_") {
        effects.add(Effect::IO);
        return Some(effects);
    }
    // KV cache, quantization, etc.
    if name.starts_with("p_kv_cache_") || name.starts_with("p_quantize") || name.starts_with("p_dequantize") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Noise operations
    if name.starts_with("p_dp_") || name.starts_with("p_gaussian_") || name.starts_with("p_dropout") {
        effects.add(Effect::Write);
        return Some(effects);
    }
    // Comm operations
    if matches!(name, "p_all_reduce" | "p_broadcast" | "p_all_gather" | "p_scatter"
        | "p_send_tensor" | "p_recv_tensor")
    {
        effects.add(Effect::IO);
        return Some(effects);
    }
    // Bare-metal hardware operations (all have side effects)
    if matches!(name,
        "p_cli" | "p_sti" | "p_hlt" | "p_lidt" | "p_idt_set_entry"
        | "p_lgdt" | "p_gdt_set_entry"
        | "p_enter_user_mode"
        | "p_outb" | "p_serial_init" | "p_serial_write"
        | "p_cr0_write" | "p_cr3_write" | "p_cr4_write" | "p_invlpg"
        | "p_page_map" | "p_page_unmap" | "p_phys_write64"
        | "p_volatile_store" | "p_volatile_store8" | "p_volatile_store16" | "p_volatile_store32"
    ) {
        effects.add(Effect::Write);
        effects.add(Effect::IO);
        return Some(effects);
    }
    if matches!(name, "p_inb" | "p_serial_read" | "p_serial_ready"
        | "p_cr0_read" | "p_cr3_read" | "p_cr4_read" | "p_phys_read64"
        | "p_volatile_load" | "p_volatile_load8" | "p_volatile_load16" | "p_volatile_load32"
    ) {
        effects.add(Effect::IO);
        return Some(effects);
    }
    None
}

/// Check if an IrType is a vec struct type and return its name
fn get_vec_type_name(ty: &IrType) -> Option<String> {
    match ty {
        IrType::Struct(name) if is_vec_type_name(name) => Some(name.clone()),
        IrType::Ptr(inner) => get_vec_type_name(inner),
        _ => None,
    }
}

/// Check if a type name is a vector type
fn is_vec_type_name(name: &str) -> bool {
    matches!(name, "vec2" | "vec3" | "vec4")
}

/// Check if a field name is a valid swizzle pattern (2-4 chars, consistent group)
fn is_valid_swizzle(field: &str) -> bool {
    if field.len() < 2 || field.len() > 4 {
        return false;
    }
    let valid = field.chars().all(|c| matches!(c, 'x' | 'y' | 'z' | 'w' | 'r' | 'g' | 'b' | 'a' | 's' | 't' | 'p' | 'q'));
    if !valid {
        return false;
    }
    let first = field.chars().next().unwrap();
    let group = if matches!(first, 'x' | 'y' | 'z' | 'w') { 'x' }
        else if matches!(first, 'r' | 'g' | 'b' | 'a') { 'r' }
        else { 's' };
    field.chars().all(|c| {
        if group == 'x' { matches!(c, 'x' | 'y' | 'z' | 'w') }
        else if group == 'r' { matches!(c, 'r' | 'g' | 'b' | 'a') }
        else { matches!(c, 's' | 't' | 'p' | 'q') }
    })
}

/// Map a swizzle component to a field index
fn swizzle_index(c: char) -> usize {
    match c {
        'x' | 'r' | 's' => 0,
        'y' | 'g' | 't' => 1,
        'z' | 'b' | 'p' => 2,
        'w' | 'a' | 'q' => 3,
        _ => 0,
    }
}

pub struct Lowerer {
    module: IrModule,
    /// Current function being lowered
    current_func: Option<IrFunction>,
    /// Current block we're emitting into
    current_block: BlockId,
    /// Loop context — (continue_target, break_target, result_slot) stack
    /// result_slot is Some(alloca) for loop-as-expression, None for plain loops
    loop_stack: Vec<(BlockId, BlockId, Option<ValueId>)>,
    /// Struct definitions from AST (for layout computation)
    structs: HashMap<String, StructDef>,
    /// Mutable variables that have been allocated on the stack (alloca pattern)
    /// Maps variable name → alloca pointer ValueId
    allocas: HashMap<String, ValueId>,
    /// Types of alloca slots — maps alloca ValueId → stored IrType
    /// Used to correctly load mutable variables with their actual type
    alloca_types: HashMap<ValueId, IrType>,
    /// AST types of variables — maps variable name → AST Type
    /// Used by LetTuple to determine tuple element types for destructuring
    var_ast_types: HashMap<String, Type>,
    /// Constants (pc) — maps name to expression for inline substitution
    constants: HashMap<String, Expr>,
    /// SRET pointer for current function (if it returns a struct)
    current_sret: Option<ValueId>,
    /// Enum definitions from AST
    enums: HashMap<String, EnumDef>,
    /// Impl method lookup: (struct_name, method_name) → Func
    impl_methods: HashMap<(String, String), Func>,
    /// Defer blocks to emit at all exit points of the current function
    defer_stack: Vec<Vec<Stmt>>,
    /// Current closure captures: maps captured var name → (env field index, type)
    /// Set when lowering inside a closure function
    current_captures: Vec<(String, usize, Type)>,
    /// Try block context stack — each entry is (error_block, error_result_slot)
    /// When non-empty, ? operator jumps to error_block instead of returning
    try_stack: Vec<(BlockId, ValueId)>,
    /// Current source line (for debug info)
    current_line: Option<usize>,
    /// Type aliases (ty X = T;) — maps alias name to resolved type
    type_aliases: HashMap<String, Type>,
    /// Current Self type (set when lowering impl methods, for bare `self` params)
    current_self_type: Option<String>,
    /// Stack of effect handlers (for handle/do statements)
    effect_handler_stack: Vec<Vec<crate::ast::EffectHandler>>,
    /// L3B: Stack of scope executors for structured concurrency.
    /// `spawn` expressions inside a `spawn scope` block use the top executor.
    scope_executor_stack: Vec<ValueId>,
    /// L3A: Counter for continuation closure names (avoids collisions with
    /// functions that are being lowered but not yet stored to the module).
    continuation_counter: u64,
    /// Stack of active named regions — allocations inside a region block
    /// use Region::Named(name) instead of Region::Stack
    region_stack: Vec<String>,
    /// Stack of active GPU regions — allocations inside a `region gpu r { }`
    /// block use GPU memory (ML1: region-scoped GPU memory)
    gpu_region_stack: Vec<String>,
    /// Sandbox depth — when > 0, emitted instructions are marked in_sandbox
    sandbox_depth: usize,
    /// AST function return types — maps function name to AST return type
    /// Used by LetTuple to determine tuple element types
    func_ret_types: HashMap<String, Type>,
    /// (trait, method) -> declared return type, for `dyn Trait` calls
    trait_method_rets: HashMap<(String, String), Type>,
    /// Async function names — functions declared with `async pf`
    /// Calls to these functions are wrapped in p_future_ready
    async_funcs: std::collections::HashSet<String>,
    /// Current function's AST parameter types — maps param name to AST type
    /// Used to determine return types of function-typed parameters (closures)
    current_func_param_types: HashMap<String, Type>,
    /// Allocas that hold Vec values (from for-loop iteration over Vec<Vec<...>>)
    /// Used to enable nested Vec iteration
    vec_allocas: std::collections::HashSet<ValueId>,
    /// I-6B: Set of @linear struct names (from typechecker).
    /// Used to mark ValueMeta.is_linear for alias analysis.
    linear_structs: std::collections::HashSet<String>,
    /// Current `let` type hint — the declared type of the binding being
    /// initialized, or None. Lets `Vec::new()` / `HashMap::new()` pick up
    /// the element type from `pv v: Vec<Token> = Vec::new();` instead of
    /// always defaulting to i64.
    let_hint: Option<Type>,
}

impl Lowerer {
    /// Get the current region for allocations.
    /// If inside a `region r { ... }` block, returns Region::Named(r).
    /// Otherwise returns Region::Stack.
    fn current_region(&self) -> Region {
        self.region_stack.last()
            .map(|name| Region::Named(name.clone()))
            .unwrap_or(Region::Stack)
    }

    /// Compute the number of 8-byte slots a field occupies in an enum layout.
    /// Struct/enum types use their full layout size (rounded up to 8);
    /// all other types use 1 slot.
    fn enum_field_slots(&self, ty: &IrType) -> u32 {
        if let IrType::Struct(ref name) = ty {
            if let Some(layout) = self.module.get_struct(name) {
                ((layout.size + 7) / 8) as u32
            } else {
                1
            }
        } else {
            1
        }
    }

    /// Load a field from an enum variant's inline storage.
    ///
    /// When the field type is a struct (enum), the data is stored inline
    /// within the parent enum. A plain `Inst::Load` with a struct type only
    /// reads 8 bytes (a pointer) in the C codegen, because `c_type(Struct(_))`
    /// returns a pointer type. To correctly extract the full struct, we
    /// allocate a new buffer and use `Inst::Store` (which emits a `memcpy`
    /// for struct types in the codegen) to copy the inline data.
    ///
    /// For non-struct types, a regular `Inst::Load` is used.
    fn load_enum_field(&mut self, field_ptr: ValueId, field_ty: &IrType) -> ValueId {
        if let IrType::Struct(_) = field_ty {
            let buf = self.emit_inst(Inst::Alloca {
                ty: field_ty.clone(),
                region: self.current_region(),
            });
            self.emit_inst(Inst::Store {
                ptr: buf,
                value: field_ptr,
                store_ty: Some(field_ty.clone()),
            });
            buf
        } else {
            self.emit_inst(Inst::Load { ptr: field_ptr, ty: field_ty.clone() })
        }
    }

    pub fn new() -> Self {
        Lowerer {
            module: IrModule::new(),
            current_func: None,
            current_block: BlockId(0),
            loop_stack: Vec::new(),
            structs: HashMap::new(),
            allocas: HashMap::new(),
            alloca_types: HashMap::new(),
            var_ast_types: HashMap::new(),
            constants: HashMap::new(),
            current_sret: None,
            enums: HashMap::new(),
            impl_methods: HashMap::new(),
            defer_stack: Vec::new(),
            current_captures: Vec::new(),
            try_stack: Vec::new(),
            current_line: None,
            type_aliases: HashMap::new(),
            current_self_type: None,
            effect_handler_stack: Vec::new(),
            scope_executor_stack: Vec::new(),
            continuation_counter: 0,
            region_stack: Vec::new(),
            gpu_region_stack: Vec::new(),
            sandbox_depth: 0,
            func_ret_types: HashMap::new(),
            trait_method_rets: HashMap::new(),
            async_funcs: std::collections::HashSet::new(),
            current_func_param_types: HashMap::new(),
            vec_allocas: std::collections::HashSet::new(),
            linear_structs: std::collections::HashSet::new(),
            let_hint: None,
        }
    }

    /// I-6B: Set the @linear struct names from the typechecker.
    /// Call this before lower_program() to enable linear type propagation.
    pub fn set_linear_structs(&mut self, structs: &std::collections::HashSet<String>) {
        self.linear_structs = structs.clone();
    }

    /// I-6B: Check if an IrType corresponds to a @linear struct.
    fn is_linear_ir_type(&self, ty: &IrType) -> bool {
        match ty {
            IrType::Struct(name) => self.linear_structs.contains(name),
            _ => false,
        }
    }

    /// Lower a complete program to IR
    pub fn lower_program(&mut self, program: &Program) -> IrModule {
        // Register constants (pc) for inline substitution
        for c in &program.consts {
            self.constants.insert(c.name.clone(), c.expr.clone());
        }

        // Register module-level mutable globals
        for g in &program.globals {
            let ir_ty = self.lower_type(&g.ty);
            self.module.globals.insert(g.name.clone(), (ir_ty, g.init.clone()));
        }

        // Register type aliases
        for ta in &program.type_aliases {
            self.type_aliases.insert(ta.name.clone(), ta.alias_for.clone());
        }

        // Register struct definitions for layout computation
        for s in &program.structs {
            self.structs.insert(s.name.clone(), s.clone());
            // I-6B: Track @linear structs for alias analysis
            if s.annotations.iter().any(|a| a.name == "linear") {
                self.linear_structs.insert(s.name.clone());
            }
        }

        // Register enum definitions
        for e in &program.enums {
            self.enums.insert(e.name.clone(), e.clone());
        }

        // Register user-defined extern functions
        for ext in &program.extern_decls {
            let param_types: Vec<IrType> = ext.params.iter().map(|p| self.lower_type(&p.ty)).collect();
            let ret_type = self.lower_type(&ext.ret_ty);
            // Register under the pledge_name so calls resolve
            self.module.add_extern(&ext.pledge_name, param_types, ret_type);
            self.module.extern_decls.push(ext.clone());
        }

        // Register impl methods
        for imp in &program.impls {
            for method in &imp.methods {
                self.impl_methods.insert((imp.struct_name.clone(), method.name.clone()), method.clone());
                self.func_ret_types.insert(method.name.clone(), method.ret_ty.clone());
            }
        }

        // Register trait impl methods — they're methods on the type
        // Keyed as (type_name, method_name) so normal method calls find them
        for ti in &program.trait_impls {
            for method in &ti.methods {
                self.impl_methods.insert((ti.type_name.clone(), method.name.clone()), method.clone());
                self.func_ret_types.insert(method.name.clone(), method.ret_ty.clone());
            }
        }

        // Register trait method names (in order) for vtable dispatch
        for td in &program.traits {
            let methods: Vec<String> = td.methods.iter().map(|m| m.name.clone()).collect();
            self.module.trait_methods.insert(td.name.clone(), methods);
            for m in &td.methods {
                self.trait_method_rets.insert((td.name.clone(), m.name.clone()), m.ret_ty.clone());
            }
        }

        // Register trait implementations for devirtualization (F-3D)
        for ti in &program.trait_impls {
            self.module.trait_impls
                .entry(ti.trait_name.clone())
                .or_default()
                .push(ti.type_name.clone());
        }

        // Compute enum and struct layouts iteratively to a fixpoint.
        // Enums may contain structs (e.g. ParseResultProgram contains Program),
        // and structs may contain enums (e.g. Func contains OptType).
        // We interleave both passes until no size changes.
        let mut changed = true;
        let mut iterations = 0;
        while changed && iterations < 20 {
            changed = false;
            for e in &program.enums {
                let old_size = self.module.get_struct(&e.name).map(|l| l.size).unwrap_or(0);
                self.compute_enum_layout(e);
                let new_size = self.module.get_struct(&e.name).map(|l| l.size).unwrap_or(0);
                if new_size != old_size {
                    changed = true;
                }
            }
            for s in &program.structs {
                let old_size = self.module.get_struct(&s.name).map(|l| l.size).unwrap_or(0);
                self.compute_struct_layout(s);
                let new_size = self.module.get_struct(&s.name).map(|l| l.size).unwrap_or(0);
                if new_size != old_size {
                    changed = true;
                }
            }
            iterations += 1;
        }

        // Register runtime/external functions
        self.register_runtime();

        // Pre-register all function signatures so calls can look up return types
        // before the function body is lowered
        for func in &program.funcs {
            // If the function returns a tuple, register the __tuple_N struct layout
            // with the same layout that TupleLit produces (struct fields inline).
            if let Type::Tuple(types) = &func.ret_ty {
                let n = types.len();
                let tuple_name = format!("__tuple_{}", n);
                let mut fields: Vec<(String, IrType, u32)> = Vec::new();
                let mut offset: u32 = 0;
                for (i, t) in types.iter().enumerate() {
                    let ir_ty = self.lower_type(t);
                    // For struct types, store the struct data inline (not a pointer)
                    let field_ty = if let IrType::Ptr(ref inner) = ir_ty {
                        if let IrType::Struct(_) = inner.as_ref() {
                            (**inner).clone()
                        } else {
                            ir_ty.clone()
                        }
                    } else {
                        ir_ty.clone()
                    };
                    let field_size = self.module.type_size(&field_ty) as u32;
                    fields.push((format!("_{}", i), field_ty, offset));
                    offset += field_size;
                    offset = (offset + 7) & !7; // align to 8
                }
                let size = offset as u64;
                self.module.add_struct(crate::ir::StructLayout {
                    name: tuple_name.clone(),
                    fields,
                    size,
                    align: 8,
                    repr_c: false,
                    packed_layout: false,
                });
            }
            let ret_ty = self.lower_type_sret(&func.ret_ty);
            let ir_func = IrFunction::new(func.name.clone(), ret_ty);
            self.module.functions.insert(func.name.clone(), ir_func);
            // Store AST return type for tuple element type lookup
            self.func_ret_types.insert(func.name.clone(), func.ret_ty.clone());
            // Track async functions — calls to these are wrapped in p_future_ready
            if func.is_async {
                self.async_funcs.insert(func.name.clone());
            }
        }

        // Lower each function
        for func in &program.funcs {
            self.lower_func(func);
        }

        // Lower impl methods (mangled as {StructName}_{method})
        for imp in &program.impls {
            let saved_self = self.current_self_type.clone();
            self.current_self_type = Some(imp.struct_name.clone());
            for method in &imp.methods {
                let mut mangled = method.clone();
                mangled.name = format!("{}_{}", imp.struct_name, method.name);
                // Resolve Self in param types and return type
                mangled.params = mangled.params.iter().map(|p| {
                    Param { name: p.name.clone(), ty: self.resolve_self_in_type(&p.ty), is_pub: p.is_pub, annotations: p.annotations.clone() }
                }).collect();
                mangled.ret_ty = self.resolve_self_in_type(&mangled.ret_ty);
                self.lower_func(&mangled);
            }
            self.current_self_type = saved_self;
        }

        // Lower trait impl methods (mangled as {TypeName}_{method})
        for ti in &program.trait_impls {
            let saved_self = self.current_self_type.clone();
            self.current_self_type = Some(ti.type_name.clone());
            for method in &ti.methods {
                let mut mangled = method.clone();
                mangled.name = format!("{}_{}", ti.type_name, method.name);
                // Resolve Self in param types and return type
                mangled.params = mangled.params.iter().map(|p| {
                    Param { name: p.name.clone(), ty: self.resolve_self_in_type(&p.ty), is_pub: p.is_pub, annotations: p.annotations.clone() }
                }).collect();
                mangled.ret_ty = self.resolve_self_in_type(&mangled.ret_ty);
                self.lower_func(&mangled);
            }
            self.current_self_type = saved_self;
        }

        // I-6B: Propagate linear_structs to the IR module
        self.module.linear_structs = self.linear_structs.iter().cloned().collect();

        // Issue 42: Register state machine structs for async functions.
        // This is the foundational infrastructure for converting thread-based
        // async/await to state-machine-based. The structs are generated now;
        // the poll functions and executor integration will follow.
        self.register_async_state_structs(program);

        // Take ownership of the module
        std::mem::take(&mut self.module)
    }

    /// Issue 42: Register state machine structs for async functions.
    ///
    /// For each `async fn`, generate a state machine struct
    /// `__AsyncState_<name>` with fields for:
    /// - State enum (i64): 0=initial, 1=pending, 2=ready
    /// - Future handle (i64): the future being awaited
    /// - Parameter fields (one per parameter)
    /// - Result field (i64)
    ///
    /// This is the first step of the state machine transform. The poll
    /// functions and executor integration will be added in subsequent work.
    fn register_async_state_structs(&mut self, program: &Program) {
        for func in &program.funcs {
            if !func.is_async {
                continue;
            }

            let struct_name = format!("__AsyncState_{}", func.name);
            let mut fields: Vec<(String, IrType, u32)> = Vec::new();
            let mut offset: u32 = 0;

            // State field (i64): 0 = initial, 1 = pending, 2 = ready
            fields.push(("__state".to_string(), IrType::I64, offset));
            offset += 8;

            // Future handle field (for await): stores the future being awaited
            fields.push(("__future".to_string(), IrType::I64, offset));
            offset += 8;

            // Parameter fields
            for param in &func.params {
                let ir_ty = self.lower_type(&param.ty);
                let size = self.module.type_size(&ir_ty) as u32;
                fields.push((format!("__param_{}", param.name), ir_ty.clone(), offset));
                offset += size;
            }

            // Result field
            fields.push(("__result".to_string(), IrType::I64, offset));
            offset += 8;

            // Register the struct layout
            self.module.struct_layouts.insert(struct_name.clone(), crate::ir::StructLayout {
                name: struct_name.clone(),
                fields,
                size: offset as u64,
                align: 8,
                repr_c: false,
                packed_layout: false,
            });
        }
    }

    /// Count the number of `await` expressions in a list of statements.
    fn count_await_stmts(stmts: &[Stmt]) -> usize {
        let mut count = 0;
        for stmt in stmts {
            count += Self::count_await_in_stmt(stmt);
        }
        count
    }

    fn count_await_in_stmt(stmt: &Stmt) -> usize {
        use crate::ast::Stmt as S;
        match stmt {
            S::Expr(e, _) => Self::count_await_in_expr(e),
            S::Let { expr, .. } => Self::count_await_in_expr(expr),
            S::LetTuple { expr, .. } => Self::count_await_in_expr(expr),
            S::Assign { expr, .. } => Self::count_await_in_expr(expr),
            S::IndexAssign { index, expr, .. } => Self::count_await_in_expr(index) + Self::count_await_in_expr(expr),
            S::DerefAssign { expr, .. } => Self::count_await_in_expr(expr),
            S::FieldAssign { expr, .. } => Self::count_await_in_expr(expr),
            S::Return(Some(e), _) => Self::count_await_in_expr(e),
            S::Return(None, _) => 0,
            S::If { then_block, else_block, cond, .. } => {
                Self::count_await_in_expr(cond)
                + Self::count_await_stmts(then_block)
                + else_block.as_ref().map(|e| Self::count_await_stmts(e)).unwrap_or(0)
            }
            S::While { cond, body, .. } => Self::count_await_in_expr(cond) + Self::count_await_stmts(body),
            S::For { iterator, body, .. } => Self::count_await_in_expr(iterator) + Self::count_await_stmts(body),
            S::Loop { body, .. } => Self::count_await_stmts(body),
            S::Defer(stmts, _) => Self::count_await_stmts(stmts),
            S::Match { expr, arms, .. } => {
                let base = Self::count_await_in_expr(expr);
                let arms_count: usize = arms.iter().map(|a| Self::count_await_stmts(&a.body)).sum();
                base + arms_count
            }
            _ => 0,
        }
    }

    fn count_await_in_expr(expr: &crate::ast::Expr) -> usize {
        use crate::ast::Expr as E;
        match expr {
            E::Await { expr } => 1 + Self::count_await_in_expr(expr),
            E::Spawn { expr } => Self::count_await_in_expr(expr),
            E::Call { args, .. } => args.iter().map(Self::count_await_in_expr).sum(),
            E::Binary { left, right, .. } => Self::count_await_in_expr(left) + Self::count_await_in_expr(right),
            E::Unary { expr, .. } => Self::count_await_in_expr(expr),
            E::MethodCall { receiver, args, .. } => Self::count_await_in_expr(receiver) + args.iter().map(Self::count_await_in_expr).sum::<usize>(),
            E::If { cond, then_block, else_block, .. } => Self::count_await_in_expr(cond) + Self::count_await_stmts(then_block) + else_block.as_ref().map(|e| Self::count_await_stmts(e)).unwrap_or(0),
            _ => 0,
        }
    }

    /// Compute the memory layout for a struct.
    /// All fields are stored as 8-byte slots to match the native codegen
    /// (which uses `field_index * 8` as the byte offset for each field).
    /// With @repr(C), fields use natural C-compatible sizes and alignment.
    fn compute_struct_layout(&mut self, def: &StructDef) {
        // Check for @repr(C) annotation
        let repr_c = def.annotations.iter().any(|a| {
            a.name == "repr" && a.args.iter().any(|arg| arg == "C")
        });
        // Check for @packed annotation
        let packed = def.annotations.iter().any(|a| a.name == "packed");

        let mut fields = Vec::new();
        let mut offset: u64 = 0;
        let mut max_align: u64 = if repr_c || packed { 1 } else { 8 };

        for field in &def.fields {
            let ir_ty = self.lower_type(&field.ty);

            if repr_c || packed {
                // C-compatible or packed layout: use natural type sizes
                let (field_size, mut field_align) = self.c_type_size_align(&ir_ty);
                if packed {
                    field_align = 1;  // Packed: no alignment padding
                }
                // Align offset to field's natural alignment
                offset = (offset + field_align - 1) & !(field_align - 1);
                fields.push((field.name.clone(), ir_ty.clone(), offset as u32));
                offset += field_size;
                if field_align > max_align {
                    max_align = field_align;
                }
            } else {
                // Uniform 8-byte slot layout (default Fides layout)
                let field_size = if let IrType::Struct(ref name) = ir_ty {
                    if let Some(layout) = self.module.get_struct(name) {
                        layout.size
                    } else {
                        8
                    }
                } else {
                    8
                };
                fields.push((field.name.clone(), ir_ty.clone(), offset as u32));
                offset += field_size;
            }
        }

        // Align struct size to its alignment
        let size = (offset + max_align - 1) & !(max_align - 1);

        let layout = StructLayout {
            name: def.name.clone(),
            fields,
            size,
            align: if repr_c || packed { max_align } else { 8 },
            repr_c,
            packed_layout: packed,
        };

        self.module.add_struct(layout);
    }

    /// Get C-compatible size and alignment for an IR type.
    /// Used by @repr(C) struct layout.
    fn c_type_size_align(&self, ty: &IrType) -> (u64, u64) {
        match ty {
            IrType::I8 | IrType::U8 | IrType::Bool => (1, 1),
            IrType::I16 | IrType::U16 => (2, 2),
            IrType::I32 | IrType::U32 | IrType::F32 => (4, 4),
            IrType::I64 | IrType::U64 | IrType::F64 => (8, 8),
            IrType::Ptr(_) => (8, 8),
            IrType::Struct(name) => {
                if let Some(layout) = self.module.get_struct(name) {
                    (layout.size, layout.align)
                } else {
                    (8, 8)
                }
            }
            _ => (8, 8), // default: pointer-sized
        }
    }

    /// Compute layout for an enum type.
    /// Enums are laid out as: tag (i64) at offset 0, then data fields at 8-byte intervals.
    /// Size = (1 + max_variant_arity) * 8, aligned to 8.
    fn compute_enum_layout(&mut self, def: &EnumDef) {
        // Compute the max payload size across all variants.
        // Each field in a variant gets at least 8 bytes, but enum-typed
        // fields need (1 + max_payload_of_that_enum) * 8 bytes, because
        // nested enums are stored inline as (tag, data0, data1, ...).
        let mut max_payload_size: u64 = 0;
        for variant in &def.variants {
            if let Some(data) = &variant.data {
                let mut variant_size: u64 = 0;
                for field_ty in data {
                    let ir_ty = self.lower_type(&field_ty.ty);
                    let field_size = if let IrType::Struct(ref name) = ir_ty {
                        if let Some(layout) = self.module.get_struct(name) {
                            layout.size
                        } else {
                            8
                        }
                    } else {
                        8
                    };
                    variant_size += field_size;
                }
                max_payload_size = max_payload_size.max(variant_size);
            }
        }

        // Round up to a multiple of 8 and count how many 8-byte slots we need.
        let payload_slots = ((max_payload_size + 7) / 8) as usize;

        let mut fields = Vec::new();
        // Field 0: tag
        fields.push(("__tag".to_string(), IrType::I64, 0u32));
        // Fields 1..max: data slots (all 8 bytes, enough for i64/f64/ptr)
        for i in 0..payload_slots {
            fields.push((format!("__data{}", i), IrType::I64, ((i + 1) * 8) as u32));
        }

        let size = ((1 + payload_slots) * 8) as u64;
        let layout = StructLayout {
            name: def.name.clone(),
            fields,
            size,
            align: 8,
            repr_c: false,
            packed_layout: false,
        };
        self.module.add_struct(layout);
    }

    /// Register runtime functions that the IR can call
    fn register_runtime(&mut self) {
        self.module.add_extern("p_print_int", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_print_str", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_eprint_str", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_print_bool", vec![IrType::Bool], IrType::Void);
        self.module.add_extern("p_print_float", vec![IrType::F64], IrType::Void);
        self.module.add_extern("p_print_newline", vec![], IrType::Void);
        self.module.add_extern("p_panic", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_alloc", vec![IrType::I64], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_malloc", vec![IrType::I64], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_free", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        // Arena marks: save returns a mark, restore frees what came after it.
        self.module.add_extern("p_arena_save", vec![], IrType::I64);
        self.module.add_extern("p_arena_restore", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_str_concat",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))],
            IrType::Ptr(Box::new(IrType::U8)));
        // Mutable string builder — avoids O(n^2) memory from string concat
        self.module.add_extern("p_sb_new", vec![], IrType::I64);
        self.module.add_extern("p_sb_append",
            vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8))],
            IrType::Void);
        self.module.add_extern("p_sb_to_str",
            vec![IrType::I64],
            IrType::Ptr(Box::new(IrType::U8)));
        // Efficient string join (allocates once, copies all parts)
        self.module.add_extern("p_str_join_n",
            vec![IrType::Ptr(Box::new(IrType::U8))],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_to_str",
            vec![IrType::I64],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_char_to_str",
            vec![IrType::I64],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_float_to_str",
            vec![IrType::F64],
            IrType::Ptr(Box::new(IrType::U8)));
        // B5: Float bit-pattern conversion for ConstFloat codegen
        self.module.add_extern("p_f64_to_bits",
            vec![IrType::F64],
            IrType::I64);
        self.module.add_extern("p_f32_to_bits",
            vec![IrType::F32],
            IrType::I64);
        // p_str_to_float(s: *u8) -> f64 — parse a string as f64
        self.module.add_extern("p_str_to_float",
            vec![IrType::Ptr(Box::new(IrType::U8))],
            IrType::F64);
        self.module.add_extern("p_bool_to_str",
            vec![IrType::Bool],
            IrType::Ptr(Box::new(IrType::U8)));
        // String operations as free functions (for typed_strings stdlib, regex stdlib, etc.)
        let str_ty = IrType::Ptr(Box::new(IrType::U8));
        self.module.add_extern("p_string_contains",
            vec![str_ty.clone(), str_ty.clone()],
            IrType::Bool);
        self.module.add_extern("p_string_replace",
            vec![str_ty.clone(), str_ty.clone(), str_ty.clone()],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_string_starts_with",
            vec![str_ty.clone(), str_ty.clone()],
            IrType::Bool);
        self.module.add_extern("p_string_ends_with",
            vec![str_ty.clone(), str_ty.clone()],
            IrType::Bool);
        self.module.add_extern("p_string_find",
            vec![str_ty.clone(), str_ty.clone()],
            IrType::I64);
        self.module.add_extern("p_string_len",
            vec![str_ty.clone()],
            IrType::I64);
        // Phase 10B: Vec_to_str is now a Fides function, not an extern
        // (no need to register as extern)

        // ─── Phase 5D.2: JSON stdlib ───
        let json_ty = IrType::Opaque("Json".to_string());
        self.module.add_extern("p_json_parse", vec![IrType::Ptr(Box::new(IrType::U8))], json_ty.clone());
        self.module.add_extern("p_json_dump", vec![json_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_json_get", vec![json_ty.clone(), IrType::Ptr(Box::new(IrType::U8))], json_ty.clone());
        self.module.add_extern("p_json_type", vec![json_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_json_len", vec![json_ty.clone()], IrType::I64);
        self.module.add_extern("p_json_at", vec![json_ty.clone(), IrType::I64], json_ty.clone());
        self.module.add_extern("p_json_pretty", vec![json_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_json_string", vec![json_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));

        // ─── Phase 5C.1: Tensor stdlib ───
        let tensor_ty = IrType::Opaque("Tensor".to_string());
        let vec_i64_ty = IrType::Opaque("Vec".to_string());
        self.module.add_extern("p_tensor_zeros", vec![vec_i64_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_ones", vec![vec_i64_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_filled", vec![vec_i64_ty.clone(), IrType::F64], tensor_ty.clone());
        self.module.add_extern("p_tensor_from_vec", vec![vec_i64_ty.clone(), vec_i64_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_get", vec![tensor_ty.clone(), vec_i64_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_set", vec![tensor_ty.clone(), vec_i64_ty.clone(), IrType::F64], IrType::Void);
        self.module.add_extern("p_tensor_shape", vec![tensor_ty.clone()], vec_i64_ty.clone());
        self.module.add_extern("p_tensor_ndim", vec![tensor_ty.clone()], IrType::I64);
        self.module.add_extern("p_tensor_size", vec![tensor_ty.clone()], IrType::I64);
        self.module.add_extern("p_tensor_reshape", vec![tensor_ty.clone(), vec_i64_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_transpose", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_add", vec![tensor_ty.clone(), tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_sub", vec![tensor_ty.clone(), tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_mul", vec![tensor_ty.clone(), tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_scale", vec![tensor_ty.clone(), IrType::F64], tensor_ty.clone());
        self.module.add_extern("p_tensor_matmul", vec![tensor_ty.clone(), tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_dot", vec![tensor_ty.clone(), tensor_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_sum", vec![tensor_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_mean", vec![tensor_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_max", vec![tensor_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_min", vec![tensor_ty.clone()], IrType::F64);
        self.module.add_extern("p_tensor_relu", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_sigmoid", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_softmax", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_tensor_to_str", vec![tensor_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_tensor_to_json", vec![tensor_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_tensor_clone", vec![tensor_ty.clone()], tensor_ty.clone());

        // ─── ML3: Autograd builtins ───
        self.module.add_extern("p_grad_get", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_grad_zero", vec![], IrType::Void);
        self.module.add_extern("p_autograd_begin", vec![], IrType::Void);
        self.module.add_extern("p_autograd_end", vec![], IrType::Void);
        self.module.add_extern("p_autograd_backward", vec![tensor_ty.clone()], IrType::Void);
        self.module.add_extern("p_checkpoint_begin", vec![], IrType::Void);
        self.module.add_extern("p_checkpoint_end", vec![], IrType::Void);

        // ─── ML9: KV Cache builtins ───
        self.module.add_extern("p_kv_cache_new",
            vec![IrType::I64, IrType::I64, IrType::I64, IrType::I64], IrType::I64);
        self.module.add_extern("p_kv_cache_append",
            vec![IrType::I64, IrType::I64, tensor_ty.clone(), tensor_ty.clone()], IrType::Void);
        self.module.add_extern("p_kv_cache_get",
            vec![IrType::I64, IrType::I64], tensor_ty.clone());
        self.module.add_extern("p_kv_cache_len",
            vec![IrType::I64, IrType::I64], IrType::I64);
        self.module.add_extern("p_kv_cache_clear",
            vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_kv_cache_evict",
            vec![IrType::I64, IrType::I64], IrType::Void);

        // ─── ML10: Quantization builtins ───
        self.module.add_extern("p_quantize_q8",
            vec![tensor_ty.clone(), IrType::F64, IrType::I64], tensor_ty.clone());
        self.module.add_extern("p_dequantize_q8",
            vec![tensor_ty.clone(), IrType::F64, IrType::I64], tensor_ty.clone());
        self.module.add_extern("p_qmatmul",
            vec![tensor_ty.clone(), tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_find_quant_params",
            vec![tensor_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));

        // ─── ML12: DP noise builtins ───
        self.module.add_extern("p_dp_noise",
            vec![tensor_ty.clone(), IrType::F64], tensor_ty.clone());
        self.module.add_extern("p_gaussian_noise",
            vec![IrType::Opaque("Vec".to_string()), IrType::F64], tensor_ty.clone());
        self.module.add_extern("p_dropout",
            vec![tensor_ty.clone(), IrType::F64], tensor_ty.clone());

        // ─── ML13: Tensor parallelism communication builtins ───
        self.module.add_extern("p_all_reduce", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_broadcast", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_all_gather", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_scatter", vec![tensor_ty.clone()], tensor_ty.clone());
        self.module.add_extern("p_send_tensor", vec![tensor_ty.clone()], IrType::Void);
        self.module.add_extern("p_recv_tensor",
            vec![IrType::Opaque("Vec".to_string())], tensor_ty.clone());

        // ─── Phase 5C.2: SIMD stdlib ───
        let simd_ty = IrType::Opaque("Vec".to_string());
        self.module.add_extern("p_simd_add", vec![simd_ty.clone(), simd_ty.clone()], simd_ty.clone());
        self.module.add_extern("p_simd_sub", vec![simd_ty.clone(), simd_ty.clone()], simd_ty.clone());
        self.module.add_extern("p_simd_mul", vec![simd_ty.clone(), simd_ty.clone()], simd_ty.clone());
        self.module.add_extern("p_simd_div", vec![simd_ty.clone(), simd_ty.clone()], simd_ty.clone());
        self.module.add_extern("p_simd_dot", vec![simd_ty.clone(), simd_ty.clone()], IrType::F64);
        self.module.add_extern("p_simd_sum", vec![simd_ty.clone()], IrType::F64);
        self.module.add_extern("p_simd_scale", vec![simd_ty.clone(), IrType::F64], simd_ty.clone());
        self.module.add_extern("p_simd_from_vec", vec![vec_i64_ty.clone()], simd_ty.clone());
        self.module.add_extern("p_simd_to_str", vec![simd_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));

        // ─── Phase 5D.3: HTTP server stdlib ───
        // Socket file descriptors are i64 (POSIX int fd or Winsock SOCKET)
        let socket_ty = IrType::I64;
        self.module.add_extern("p_socket_listen", vec![IrType::I64], socket_ty.clone());
        self.module.add_extern("p_socket_accept", vec![socket_ty.clone()], socket_ty.clone());
        self.module.add_extern("p_socket_read", vec![socket_ty.clone(), vec_i64_ty.clone()], IrType::I64);
        self.module.add_extern("p_socket_write", vec![socket_ty.clone(), vec_i64_ty.clone()], IrType::I64);
        self.module.add_extern("p_socket_close", vec![socket_ty.clone()], IrType::Void);
        self.module.add_extern("p_socket_recv_str", vec![socket_ty.clone()], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_http_parse_request", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Opaque("HttpRequest".to_string()));
        self.module.add_extern("p_http_build_response", vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))], IrType::Ptr(Box::new(IrType::U8)));

        // ─── File I/O builtins ───
        // p_file_read(path: *u8) -> *u8  — read entire file, return content as string
        // p_file_write(path: *u8, content: *u8) -> i64  — write content to file, return 0 on success, -1 on error
        // p_file_exists(path: *u8) -> bool  — check if file exists
        // p_file_append(path: *u8, content: *u8) -> i64  — append content to file
        self.module.add_extern("p_file_read", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_file_write", vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        self.module.add_extern("p_file_write_bin", vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8)), IrType::I64], IrType::I64);
        self.module.add_extern("p_file_write_vec", vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        self.module.add_extern("p_file_read_bytes", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Generic("Vec".to_string(), vec![IrType::I64]));
        self.module.add_extern("p_file_exists", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Bool);
        self.module.add_extern("p_file_append", vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        // p_file_delete(path: *u8) -> i64  — delete a file, returns 0 on success
        self.module.add_extern("p_file_delete", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        // p_file_size(path: *u8) -> i64  — get file size in bytes, or -1
        self.module.add_extern("p_file_size", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        // p_file_hash(path: *u8) -> i64 — FNV-1a 64 of the file's bytes, 0 if unreadable
        self.module.add_extern("p_file_hash", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::I64);

        // ─── Phase 5D.4: Database driver framework ───
        let db_ty = IrType::Opaque("Database".to_string());
        self.module.add_extern("p_db_mem_open", vec![], db_ty.clone());
        self.module.add_extern("p_db_mem_close", vec![db_ty.clone()], IrType::Void);
        self.module.add_extern("p_db_mem_query", vec![db_ty.clone(), IrType::Ptr(Box::new(IrType::U8))], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_db_mem_exec", vec![db_ty.clone(), IrType::Ptr(Box::new(IrType::U8))], IrType::I64);

        // ─── Phase 2A.8: Allocator builtins ───
        let alloc_ty = IrType::Opaque("Allocator".to_string());
        self.module.add_extern("p_allocator_create", vec![IrType::Ptr(Box::new(IrType::U8))], alloc_ty.clone());
        self.module.add_extern("p_allocator_alloc", vec![alloc_ty.clone(), IrType::I64], IrType::I64);
        self.module.add_extern("p_allocator_free", vec![alloc_ty.clone(), IrType::I64], IrType::Void);
        self.module.add_extern("p_allocator_stats", vec![alloc_ty], IrType::I64);

        // ─── Phase 5A.3: Interrupt handler builtins (bare-metal) ───
        self.module.add_extern("p_cli", vec![], IrType::Void);           // disable interrupts
        self.module.add_extern("p_sti", vec![], IrType::Void);           // enable interrupts
        self.module.add_extern("p_lidt", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void); // load IDT
        self.module.add_extern("p_idt_set_entry", vec![IrType::I64, IrType::I64, IrType::I64, IrType::I64], IrType::Void); // set IDT entry (idt_ptr, vector, handler, flags)
        self.module.add_extern("p_hlt", vec![], IrType::Void);           // halt CPU
        self.module.add_extern("p_inb", vec![IrType::I64], IrType::I64);  // port I/O: read byte
        self.module.add_extern("p_outb", vec![IrType::I64, IrType::I64], IrType::Void); // port I/O: write byte

        // ─── @zeroize: memory wiping builtin ───
        self.module.add_extern("p_memset", vec![IrType::I64, IrType::I32, IrType::I64], IrType::Void); // memset(ptr, val, size)
        // @zeroize: wipe the bytes a str / the buffer a Vec owns
        self.module.add_extern("p_str_zeroize", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_vec_zeroize", vec![IrType::I64], IrType::Void);

        // ─── Phase 5A.4: MMU/page table management (bare-metal) ───
        self.module.add_extern("p_cr3_read", vec![], IrType::I64);         // read CR3 (page table base)
        self.module.add_extern("p_cr3_write", vec![IrType::I64], IrType::Void); // write CR3
        self.module.add_extern("p_cr0_read", vec![], IrType::I64);         // read CR0
        self.module.add_extern("p_cr0_write", vec![IrType::I64], IrType::Void); // write CR0
        self.module.add_extern("p_cr4_read", vec![], IrType::I64);         // read CR4
        self.module.add_extern("p_cr4_write", vec![IrType::I64], IrType::Void); // write CR4
        self.module.add_extern("p_invlpg", vec![IrType::I64], IrType::Void); // invalidate TLB entry
        self.module.add_extern("p_page_map", vec![IrType::I64, IrType::I64, IrType::I64, IrType::I64], IrType::Void); // map vaddr->paddr
        self.module.add_extern("p_page_unmap", vec![IrType::I64, IrType::I64], IrType::Void); // unmap vaddr
        self.module.add_extern("p_phys_read64", vec![IrType::I64], IrType::I64); // read 8 bytes from physical addr
        self.module.add_extern("p_phys_write64", vec![IrType::I64, IrType::I64], IrType::Void); // write 8 bytes to physical addr
        // Volatile memory access (MMIO)
        self.module.add_extern("p_volatile_load", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_volatile_store", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_volatile_load8", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_volatile_store8", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_volatile_load16", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_volatile_store16", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_volatile_load32", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_volatile_store32", vec![IrType::I64, IrType::I64], IrType::Void);

        // ─── Phase 5A.5: GDT builtins (bare-metal) ───
        self.module.add_extern("p_lgdt", vec![IrType::I64], IrType::Void); // load GDT
        self.module.add_extern("p_gdt_set_entry", vec![IrType::I64, IrType::I64, IrType::I64, IrType::I64, IrType::I64], IrType::Void); // set GDT entry

        // ─── User-mode transition (ring 3) ───
        self.module.add_extern("p_enter_user_mode", vec![IrType::I64, IrType::I64], IrType::Void); // enter ring 3 (entry_addr, stack_addr)

        // ─── Phase 5A.6: Serial port builtins (bare-metal) ───
        self.module.add_extern("p_serial_init", vec![], IrType::Void);           // init COM1
        self.module.add_extern("p_serial_write", vec![IrType::I64], IrType::Void); // write byte to COM1
        self.module.add_extern("p_serial_read", vec![], IrType::I64);            // read byte from COM1
        self.module.add_extern("p_serial_ready", vec![], IrType::I64);           // check if data available

        // ─── Environment variable builtins ───
        // p_getenv(name: *u8) -> *u8  — get environment variable value (null if not set)
        self.module.add_extern("p_getenv", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Ptr(Box::new(IrType::U8)));

        // ─── Command-line argument builtins ───
        // p_argc() -> i64  — number of command-line arguments
        self.module.add_extern("p_argc", vec![], IrType::I64);
        // p_argv(index: i64) -> *u8  — get command-line argument by index (null if out of bounds)
        self.module.add_extern("p_argv", vec![IrType::I64], IrType::Ptr(Box::new(IrType::U8)));

        // ─── Process control builtins ───
        // p_system(cmd: *u8) -> i64  — run a shell command, return exit status
        self.module.add_extern("p_system", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        // p_exit(code: i64) -> void  — exit the process with a code
        self.module.add_extern("p_exit", vec![IrType::I64], IrType::Void);
        // p_clock_now() -> i64  — wall-clock time in nanoseconds since Unix epoch
        self.module.add_extern("p_clock_now", vec![], IrType::I64);
        // p_clock_now_us() -> i64  — wall-clock time in microseconds since Unix epoch
        self.module.add_extern("p_clock_now_us", vec![], IrType::I64);
        // p_clock_now_ms() -> i64  — wall-clock time in milliseconds since Unix epoch
        self.module.add_extern("p_clock_now_ms", vec![], IrType::I64);

        // ─── Regex builtin ───
        // p_regex_match(s: *u8, pattern: *u8) -> bool  — simple wildcard match (? and *)
        self.module.add_extern("p_regex_match",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))],
            IrType::Bool);

        // ─── Global string interner ───
        // p_intern_str(s: *u8) -> *u8  — interns s globally, returns canonical pointer
        self.module.add_extern("p_intern_str",
            vec![IrType::Ptr(Box::new(IrType::U8))],
            IrType::Ptr(Box::new(IrType::U8)));

        // ─── Phase 7.6: Async/await runtime ───
        // p_future_ready(value: i64) -> i64  — wrap a value in a ready Future
        // The Future is an opaque handle (pointer to a runtime struct).
        // At the IR level, Future<T> is represented as i64 (pointer-sized handle).
        self.module.add_extern("p_future_ready", vec![IrType::I64], IrType::I64);
        // p_future_poll(future: i64) -> i64  — poll a Future, return its value
        // If the Future is Ready, returns the cached value.
        // If the Future is Pending, blocks until the thread completes.
        self.module.add_extern("p_future_poll", vec![IrType::I64], IrType::I64);

        // ─── Phase 8A: Real OS threads + atomics + mutex ───
        // p_thread_spawn(func_name: *u8, num_args: i64, args...) -> i64
        //   Spawns an OS thread that runs the named function with the given args.
        //   Returns a Future handle (i64). Use p_future_poll to join.
        self.module.add_extern("p_thread_spawn",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::I64],
            IrType::I64);
        // ─── Phase A1: Executor + non-blocking task poll ───
        // p_executor_new() -> i64 — create an executor, return handle
        self.module.add_extern("p_executor_new", vec![], IrType::I64);
        // p_executor_spawn(exec, func_name, num_args, args...) -> i64 (future handle)
        self.module.add_extern("p_executor_spawn",
            vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8)), IrType::I64],
            IrType::I64);
        // p_executor_block_on(exec, future) -> i64 — block until future is ready
        self.module.add_extern("p_executor_block_on", vec![IrType::I64, IrType::I64], IrType::I64);
        // p_task_poll(future) -> i64 — non-blocking poll (returns PENDING_SENTINEL if not ready)
        self.module.add_extern("p_task_poll", vec![IrType::I64], IrType::I64);
        // p_task_is_ready(future) -> bool — check if future is ready
        self.module.add_extern("p_task_is_ready", vec![IrType::I64], IrType::Bool);
        // p_executor_run_all(exec) -> i64 — join all tasks on executor, return count
        self.module.add_extern("p_executor_run_all", vec![IrType::I64], IrType::I64);
        // p_executor_poll_all(exec) -> i64 — non-blocking poll, returns count of pending tasks
        self.module.add_extern("p_executor_poll_all", vec![IrType::I64], IrType::I64);
        // L3E: p_executor_cancel(exec) -> i64 — cancel all pending tasks on executor
        self.module.add_extern("p_executor_cancel", vec![IrType::I64], IrType::I64);
        // p_task_poll_nonblocking(future) -> i64 — returns value if ready, PENDING_SENTINEL if not
        self.module.add_extern("p_task_poll_nonblocking", vec![IrType::I64], IrType::I64);
        // ─── Phase A3: Non-blocking IO ───
        // p_net_connect(host: *u8, port: i64) -> i64 (Future handle)
        self.module.add_extern("p_net_connect",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::I64], IrType::I64);
        // p_net_read(sock: i64, buf: *u8, len: i64) -> i64 (Future handle)
        self.module.add_extern("p_net_read",
            vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8)), IrType::I64], IrType::I64);
        // p_net_write(sock: i64, buf: *u8, len: i64) -> i64 (Future handle)
        self.module.add_extern("p_net_write",
            vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8)), IrType::I64], IrType::I64);
        // p_net_close(sock: i64) -> void
        self.module.add_extern("p_net_close", vec![IrType::I64], IrType::Void);
        // p_fs_read_async(path: *u8, buf: *u8, len: i64) -> i64 (Future handle)
        self.module.add_extern("p_fs_read_async",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8)), IrType::I64], IrType::I64);
        // p_shared_alloc(size: i64) -> i64  — allocate shared memory, return address
        self.module.add_extern("p_shared_alloc", vec![IrType::I64], IrType::I64);
        // p_atomic_load(addr: i64) -> i64  — atomic load from shared memory
        self.module.add_extern("p_atomic_load", vec![IrType::I64], IrType::I64);
        // p_atomic_store(addr: i64, val: i64) -> void  — atomic store to shared memory
        self.module.add_extern("p_atomic_store", vec![IrType::I64, IrType::I64], IrType::Void);
        // p_atomic_cas(addr: i64, expected: i64, new: i64) -> bool  — compare-and-swap
        self.module.add_extern("p_atomic_cas", vec![IrType::I64, IrType::I64, IrType::I64], IrType::Bool);
        // p_atomic_add(addr: i64, val: i64) -> i64  — atomic fetch-and-add, returns old value
        self.module.add_extern("p_atomic_add", vec![IrType::I64, IrType::I64], IrType::I64);
        // p_atomic_sub(addr: i64, val: i64) -> i64  — atomic fetch-and-sub, returns old value
        self.module.add_extern("p_atomic_sub", vec![IrType::I64, IrType::I64], IrType::I64);
        // p_rc_incr(rc_addr: i64, val: i64) -> i64  — non-atomic refcount increment, returns val
        self.module.add_extern("p_rc_incr", vec![IrType::I64, IrType::I64], IrType::I64);
        // p_rc_decr(rc_addr: i64, val: i64) -> void  — non-atomic refcount decrement, frees if zero
        self.module.add_extern("p_rc_decr", vec![IrType::I64, IrType::I64], IrType::Void);
        // p_mutex_init() -> i64  — create a mutex, return handle
        self.module.add_extern("p_mutex_init", vec![], IrType::I64);
        // p_mutex_lock(m: i64) -> void  — lock a mutex (blocks if contended)
        self.module.add_extern("p_mutex_lock", vec![IrType::I64], IrType::Void);
        // p_mutex_unlock(m: i64) -> void  — unlock a mutex
        self.module.add_extern("p_mutex_unlock", vec![IrType::I64], IrType::Void);
        // ─── Thread-local storage ───
        // p_tls_create() -> i64  — create a thread-local key, return key handle
        self.module.add_extern("p_tls_create", vec![], IrType::I64);
        // p_tls_get(key: i64) -> i64  — get the value for the current thread (0 if unset)
        self.module.add_extern("p_tls_get", vec![IrType::I64], IrType::I64);
        // p_tls_set(key: i64, value: i64) -> void  — set the value for the current thread
        self.module.add_extern("p_tls_set", vec![IrType::I64, IrType::I64], IrType::Void);
        // ─── Channels (bounded MPSC queue) ───
        // p_chan_create(capacity: i64) -> i64 — create a bounded channel
        self.module.add_extern("p_chan_create", vec![IrType::I64], IrType::I64);
        // p_chan_send(ch: i64, val: i64) -> bool — send a value (blocks if full)
        self.module.add_extern("p_chan_send", vec![IrType::I64, IrType::I64], IrType::Bool);
        // p_chan_recv(ch: i64) -> i64 — receive a value (blocks if empty)
        self.module.add_extern("p_chan_recv", vec![IrType::I64], IrType::I64);
        // p_chan_try_recv(ch: i64) -> i64 — non-blocking receive
        self.module.add_extern("p_chan_try_recv", vec![IrType::I64], IrType::I64);
        // p_chan_close(ch: i64) -> void — close the channel
        self.module.add_extern("p_chan_close", vec![IrType::I64], IrType::Void);
        // p_chan_is_closed(ch: i64) -> bool — check if channel is closed
        self.module.add_extern("p_chan_is_closed", vec![IrType::I64], IrType::Bool);
        // Dynamic loading builtins (Phase 7.7)
        self.module.add_extern("p_dl_open",
            vec![IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        self.module.add_extern("p_dl_sym",
            vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8))], IrType::I64);
        self.module.add_extern("p_dl_close",
            vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_dl_call",
            vec![IrType::I64, IrType::I64], IrType::I64);

        // Vec runtime builtins (used by stdlib/vec.fd methods)
        // These use the runtime calling convention (no env_ptr).
        self.module.add_extern("p_vec_new", vec![], IrType::I64);
        self.module.add_extern("p_vec_push", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_push_str", vec![IrType::I64, IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_vec_pop", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_len", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_get", vec![IrType::I64, IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_set", vec![IrType::I64, IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_is_empty", vec![IrType::I64], IrType::Bool);
        self.module.add_extern("p_vec_clear", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_capacity", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_insert", vec![IrType::I64, IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_remove", vec![IrType::I64, IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_contains", vec![IrType::I64, IrType::I64], IrType::Bool);
        self.module.add_extern("p_vec_reverse", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_first", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_last", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_extend", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_truncate", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_sort", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_clone", vec![IrType::I64], IrType::I64);
        self.module.add_extern("p_vec_resize", vec![IrType::I64, IrType::I64], IrType::Void);
        self.module.add_extern("p_vec_new_with", vec![IrType::I64], IrType::I64);
    }

    /// Lower a single function
    fn lower_func(&mut self, func: &Func) {
        // If the function returns a tuple, register the __tuple_N struct layout
        // so the C codegen can determine the correct alloca size for SRET.
        // Uses the same layout as TupleLit (struct fields stored inline).
        if let Type::Tuple(types) = &func.ret_ty {
            let n = types.len();
            let tuple_name = format!("__tuple_{}", n);
            let mut fields: Vec<(String, IrType, u32)> = Vec::new();
            let mut offset: u32 = 0;
            for (i, t) in types.iter().enumerate() {
                let ir_ty = self.lower_type(t);
                let field_ty = if let IrType::Ptr(ref inner) = ir_ty {
                    if let IrType::Struct(_) = inner.as_ref() {
                        (**inner).clone()
                    } else {
                        ir_ty.clone()
                    }
                } else {
                    ir_ty.clone()
                };
                let field_size = self.module.type_size(&field_ty) as u32;
                fields.push((format!("_{}", i), field_ty, offset));
                offset += field_size;
                offset = (offset + 7) & !7;
            }
            let size = offset as u64;
            self.module.add_struct(crate::ir::StructLayout {
                name: tuple_name.clone(),
                fields,
                size,
                align: 8,
                repr_c: false,
                packed_layout: false,
            });
        }
        let ret_ty = self.lower_type_sret(&func.ret_ty);
        let mut ir_func = IrFunction::new(func.name.clone(), ret_ty.clone());
        ir_func.is_interrupt = func.is_interrupt
            || func.annotations.iter().any(|a| a.name == "interrupt");
        // F29: @interrupt(N) annotation — store the IDT vector number for automatic IDT setup.
        // The vector number is the first argument of the annotation: @interrupt(0x20)
        ir_func.interrupt_vector = func.annotations.iter()
            .find(|a| a.name == "interrupt")
            .and_then(|a| a.args.first())
            .and_then(|s| {
                // Parse as hex (0xNN) or decimal
                if s.starts_with("0x") || s.starts_with("0X") {
                    u64::from_str_radix(&s[2..], 16).ok()
                } else {
                    s.parse::<u64>().ok()
                }
            });
        // F65: @wrapping annotation — use wrap-around arithmetic instead of checked
        ir_func.is_wrapping = func.annotations.iter().any(|a| a.name == "wrapping");
        // I-4A: @deterministic annotation — function is reproducible, can be memoized
        ir_func.is_deterministic = func.annotations.iter().any(|a| a.name == "deterministic");
        // F-3F: @crashfree annotation — function can't panic, safe to inline more aggressively
        ir_func.is_crashfree = func.annotations.iter().any(|a| a.name == "crashfree");
        // I-4C: @gas(N) annotation — gas limit proves termination, allows larger unrolling
        ir_func.gas_limit = func.annotations.iter()
            .find(|a| a.name == "gas")
            .and_then(|a| a.args.first())
            .and_then(|s| s.parse::<u64>().ok());
        // I-2D: @no-alloc annotation — function cannot allocate, skip allocator state preservation.
        // @medical implies @no-alloc, so set is_no_alloc for both.
        ir_func.is_no_alloc = func.annotations.iter().any(|a| a.name == "no-alloc" || a.name == "medical");
        // I-4D: @medical annotation — IEC 62304 safety. Implies @no-alloc + @no-recursion
        // + @crashfree + @requires-audit. The optimizer can assume
        // no heap allocation, no recursion, bounded iteration, no panics.
        ir_func.is_medical = func.annotations.iter().any(|a| a.name == "medical");
        // ML8: @kernel annotation — GPU entry point for SPIR-V/PTX codegen
        ir_func.is_kernel = func.annotations.iter().any(|a| a.name == "kernel");
        // F23: @simd annotation — enables aggressive loop vectorization
        ir_func.is_simd = func.annotations.iter().any(|a| a.name == "simd");
        // Parse workgroup size from @kernel(x, y, z) or @kernel([x, y, z])
        if ir_func.is_kernel {
            if let Some(kann) = func.annotations.iter().find(|a| a.name == "kernel") {
                if !kann.args.is_empty() {
                    // Try parsing as [x,y,z] or x,y,z
                    let args_str: String = kann.args.join(",");
                    let cleaned = args_str.trim_matches(|c| c == '[' || c == ']');
                    let parts: Vec<&str> = cleaned.split(',').collect();
                    if parts.len() >= 3 {
                        if let (Ok(x), Ok(y), Ok(z)) = (
                            parts[0].trim().parse::<u32>(),
                            parts[1].trim().parse::<u32>(),
                            parts[2].trim().parse::<u32>(),
                        ) {
                            ir_func.workgroup_size = [x, y, z];
                        }
                    } else if parts.len() == 1 {
                        if let Ok(x) = parts[0].trim().parse::<u32>() {
                            ir_func.workgroup_size = [x, 1, 1];
                        }
                    }
                }
            }
        }

        // If the function returns a struct, add a hidden SRET parameter
        // (caller-allocated pointer as first arg, callee writes struct data there)
        // SRET goes in rcx (first arg), __env goes in rdx (second arg)
        let is_struct_ret = matches!(ret_ty, IrType::Struct(_));
        if is_struct_ret {
            let sret_ty = IrType::Ptr(Box::new(ret_ty.clone()));
            let meta = ValueMeta {
                ty: sret_ty.clone(),
                region: self.current_region(),
                effects: EffectSet::none(),
                source_line: None,
                in_sandbox: false,
                                is_linear: false,
                                borrow_kind: crate::ir::BorrowKind::None,
            };
            let sret_val = ir_func.new_value(meta);
            ir_func.params.push((sret_val, sret_ty));
            // Don't bind it to a name — it's hidden
            self.current_sret = Some(sret_val);
        } else {
            self.current_sret = None;
        }

        // All functions get __env as next param (for uniform CallPtr calling convention)
        let env_meta = ValueMeta {
            ty: IrType::Ptr(Box::new(IrType::U8)),
            region: self.current_region(),
            effects: EffectSet::none(),
            source_line: None,
            in_sandbox: false,
                                is_linear: false,
                                borrow_kind: crate::ir::BorrowKind::None,
        };
        let env_val = ir_func.new_value(env_meta);
        ir_func.params.push((env_val, IrType::Ptr(Box::new(IrType::U8))));
        ir_func.bind_local("__env", env_val);

        // Create parameter values
        for param in &func.params {
            let raw_ty = self.lower_type(&param.ty);
            // I-6B: Check linearity before raw_ty is moved
            let is_linear = self.is_linear_ir_type(&raw_ty);
            // Struct params are passed as pointers (by-ref) so FieldAccess works
            let ir_ty = if matches!(raw_ty, IrType::Struct(_)) {
                IrType::Ptr(Box::new(raw_ty))
            } else {
                raw_ty
            };
            let meta = ValueMeta {
                ty: ir_ty.clone(),
                region: self.current_region(),
                effects: EffectSet::none(),
                source_line: None,
                in_sandbox: false,
                is_linear,
                borrow_kind: crate::ir::BorrowKind::None,
            };
            let val = ir_func.new_value(meta);
            ir_func.params.push((val, ir_ty.clone()));
            ir_func.bind_local(&param.name, val);
        }

        // Set as current function
        let prev = std::mem::replace(&mut self.current_func, Some(ir_func));
        self.current_block = BlockId(0);
        self.allocas.clear();
        self.alloca_types.clear();
        self.var_ast_types.clear();
        self.defer_stack.clear();
        // Save AST parameter types for function-typed param return type lookup
        self.current_func_param_types.clear();
        for p in &func.params {
            self.current_func_param_types.insert(p.name.clone(), p.ty.clone());
        }

        // @audit annotation: emit log calls at function entry/exit.
        // Entry: print "[AUDIT] enter <name>\n"
        // Exit:  print "[AUDIT] exit <name>\n" (via defer, runs before every return)
        let is_audited = func.annotations.iter().any(|a| a.name == "audit");
        if is_audited {
            // Entry log with parameter values
            // Format: [AUDIT] enter func_name(param1=val1, param2=val2)\n
            let param_info: Vec<(String, ValueId, IrType)> = func.params.iter()
                .filter_map(|p| {
                    let val = self.current_func.as_ref().and_then(|f| f.locals.get(&p.name).copied());
                    val.map(|v| {
                        let ir_ty = lower_type(&p.ty);
                        (p.name.clone(), v, ir_ty)
                    })
                })
                .collect();

            if param_info.is_empty() {
                let entry_msg = self.emit_inst(Inst::ConstStr {
                    value: format!("[AUDIT] enter {}\n", func.name),
                });
                self.emit_inst(Inst::Print {
                    value: entry_msg,
                    ty: IrType::Ptr(Box::new(IrType::U8)),
                });
            } else {
                let mut header = format!("[AUDIT] enter {}(", func.name);
                for (i, (pname, pval, pty)) in param_info.iter().enumerate() {
                    if i > 0 { header.push_str(", "); }
                    header.push_str(&format!("{}=", pname));
                    let hdr = self.emit_inst(Inst::ConstStr { value: header.clone() });
                    self.emit_inst(Inst::Print {
                        value: hdr,
                        ty: IrType::Ptr(Box::new(IrType::U8)),
                    });
                    // Print the param value using its actual type
                    self.emit_inst(Inst::Print {
                        value: *pval,
                        ty: pty.clone(),
                    });
                    header.clear();
                }
                let close = self.emit_inst(Inst::ConstStr { value: ")\n".to_string() });
                self.emit_inst(Inst::Print {
                    value: close,
                    ty: IrType::Ptr(Box::new(IrType::U8)),
                });
            }
            // Exit log via defer — runs before every return point
            let exit_stmt = Stmt::Expr(
                Expr::Call {
                    callee: "print".to_string(),
                    args: vec![Expr::StringLit(format!("[AUDIT] exit {}\n", func.name))],
                },
                0,
            );
            self.defer_stack.push(vec![exit_stmt]);
        }

        // Lower the function body
        // If the function has a non-void return type and the last statement
        // is an expression, use it as the implicit return value.
        let mut last_expr_val: Option<ValueId> = None;
        // L3A: Use lower_stmts (not lower_stmt loop) so that top-level `do`
        // statements get continuation capture. lower_stmts returns early
        // after a `do`, with remaining statements captured in a continuation.
        let has_do = func.body.iter().any(|s| matches!(s, Stmt::Do { .. }));
        if has_do {
            // lower_stmts handles `do` with continuation capture.
            // The continuation function handles its own implicit return.
            self.lower_stmts(&func.body);
        } else {
            for stmt in &func.body {
                if let Stmt::Expr(expr, _) = stmt {
                    // Lower the expression and remember its value
                    last_expr_val = Some(self.lower_expr(expr));
                } else {
                    last_expr_val = None;
                    self.lower_stmt(stmt);
                }
            }
        }

        // Ensure the function ends with a return
        // Check if current block already has a terminator (immutable borrow first)
        let has_terminator = if let Some(ref f) = self.current_func {
            let block_idx = self.current_block.0 as usize;
            if block_idx < f.blocks.len() {
                f.blocks[block_idx].instructions.iter()
                    .any(|id| f.instructions.get(id).map(|i| i.is_terminator()).unwrap_or(false))
            } else {
                false
            }
        } else {
            false
        };

        if !has_terminator {
            // Emit compiler-inserted drops for locals with Drop impls
            self.emit_drops();

            // Emit defer blocks before implicit return
            self.emit_defers();

            if self.current_sret.is_some() {
                // Struct-returning function: copy struct data to sret, return void
                if let Some(val) = last_expr_val {
                    self.emit_sret_copy(val);
                }
                self.emit_terminator(Inst::Return { value: None });
            } else {
                // Non-struct return: use the function's return type
                let is_void = self.current_func.as_ref().map_or(true, |f| f.ret_ty == IrType::Void);
                let last_expr_val = match last_expr_val {
                    Some(val) if !is_void => Some(self.coerce_return(val)),
                    other => other,
                };
                let ret_val = if let Some(ref f) = self.current_func {
                    if f.ret_ty == IrType::Void {
                        None
                    } else if let Some(val) = last_expr_val {
                        Some(val)
                    } else {
                        // Implicit return of 0 for non-void functions (safety)
                        // Need to emit a zero constant — handle below
                        None
                    }
                } else {
                    None
                };

                if ret_val.is_none() && self.current_func.is_some() {
                    // Need to emit a zero constant for non-void functions
                    let needs_zero = if let Some(ref f) = self.current_func {
                        f.ret_ty != IrType::Void && last_expr_val.is_none()
                    } else {
                        false
                    };
                    if needs_zero {
                        let zero_val = {
                            let region = self.current_region();
                            if let Some(f) = self.current_func.as_mut() {
                            let ret_ty = f.ret_ty.clone();
                            let meta = ValueMeta {
                                ty: ret_ty.clone(),
                                region,
                                effects: EffectSet::none(),
                                source_line: None,
                                in_sandbox: false,
                                is_linear: false,
                                borrow_kind: crate::ir::BorrowKind::None,
                            };
                            let zid = {
                                let id = f.new_value(meta);
                                let inst = match &ret_ty {
                                    IrType::F32 | IrType::F64 => Inst::ConstFloat { bits: 0, ty: ret_ty.clone() },
                                    IrType::Bool => Inst::ConstBool { value: false },
                                    _ => Inst::ConstInt { value: 0, ty: ret_ty.clone() },
                                };
                                f.instructions.insert(id, inst);
                                let block_idx = self.current_block.0 as usize;
                                f.blocks[block_idx].instructions.push(id);
                                id
                            };
                            Some(zid)
                            } else {
                            None
                            }
                        };
                        self.emit_terminator(Inst::Return { value: zero_val });
                    } else {
                        self.emit_terminator(Inst::Return { value: None });
                    }
                } else {
                    self.emit_terminator(Inst::Return { value: ret_val });
                }
            }
        }

        // Store the function in the module
        if let Some(f) = self.current_func.take() {
            // Compute function effects from all instructions
            let mut func_effects = EffectSet::new();
            for inst in f.instructions.values() {
                func_effects.add_all(&inst.effects());
            }
            let func_name = f.name.clone();
            self.module.function_effects.insert(func_name.clone(), func_effects.clone());
            let name = f.name.clone();
            let mut f = f;
            f.effects = func_effects;
            self.module.functions.insert(name, f);
        }

        self.current_func = prev;
    }

    /// Resolve type aliases to their underlying type
    fn resolve_type(&self, ty: &Type) -> Type {
        match ty {
            Type::Named(name) => {
                if let Some(alias) = self.type_aliases.get(name) {
                    self.resolve_type(alias)
                } else {
                    ty.clone()
                }
            }
            Type::Ref { inner, is_mut } => {
                Type::Ref { inner: Box::new(self.resolve_type(inner)), is_mut: *is_mut }
            }
            Type::RawPtr { inner, is_mut } => {
                Type::RawPtr { inner: Box::new(self.resolve_type(inner)), is_mut: *is_mut }
            }
            Type::Array { elem, size } => {
                Type::Array { elem: Box::new(self.resolve_type(elem)), size: *size }
            }
            Type::ArrayConst { elem, size_param } => {
                Type::ArrayConst { elem: Box::new(self.resolve_type(elem)), size_param: size_param.clone() }
            }
            Type::Generic { name, args } => {
                Type::Generic {
                    name: name.clone(),
                    args: args.iter().map(|a| self.resolve_type(a)).collect(),
                }
            }
            other => other.clone(),
        }
    }

    /// Lower a Fides AST type to an IR type, resolving aliases first
    fn lower_type(&self, ty: &Type) -> IrType {
        let resolved = self.resolve_type(ty);
        crate::ir::lower_type(&resolved)
    }

    /// Like lower_type, but converts enum types (e.g. Result<T,E>, Option<T>)
    /// from Generic("Result", ...) to Struct("Result") so that SRET calling
    /// convention and codegen treat them as aggregate types.
    /// Used only at function signature boundaries (return type + call sites).
    fn lower_type_sret(&self, ty: &Type) -> IrType {
        // Convert tuple types to struct types so SRET calling convention is used.
        // Without this, tuples lower to IrType::Opaque("tuple") (8 bytes) and
        // functions returning tuples return a dangling pointer to stack alloca.
        // The __tuple_N struct layout is registered by TupleLit lowering or
        // by register_tuple_struct called during function lowering.
        if let Type::Tuple(types) = ty {
            let n = types.len();
            let tuple_name = format!("__tuple_{}", n);
            return IrType::Struct(tuple_name);
        }
        let ir_ty = self.lower_type(ty);
        if let IrType::Generic(ref name, _) = ir_ty {
            let resolved_name = self.resolve_struct_name(name);
            if self.enums.contains_key(&resolved_name) {
                return IrType::Struct(resolved_name);
            }
        }
        ir_ty
    }

    /// Resolve a type name (possibly an alias) to its underlying struct name
    fn resolve_struct_name(&self, name: &str) -> String {
        if let Some(alias) = self.type_aliases.get(name) {
            match alias {
                Type::Named(inner) => return self.resolve_struct_name(inner),
                _ => {}
            }
        }
        name.to_string()
    }

    /// Look up the EnumDef a variant pattern refers to. `enum_name` may be
    /// empty for shorthand patterns like `Some(x)`, in which case we search
    /// all enums for a variant with that name.
    fn lookup_pattern_enum(&self, enum_name: &str, variant: &str) -> Option<&EnumDef> {
        if !enum_name.is_empty() {
            let resolved = self.resolve_struct_name(enum_name);
            if let Some(def) = self.enums.get(&resolved) {
                return Some(def);
            }
            if let Some(def) = self.enums.get(enum_name) {
                return Some(def);
            }
        }
        self.enums.values().find(|def| def.variants.iter().any(|v| v.name == variant))
    }

    /// IR types for the values bound by an enum-variant pattern.
    ///
    /// Enum-typed fields (structs/enums declared directly, not Box/Ptr) are
    /// stored INLINE in the parent enum (per `compute_enum_layout`). Returning
    /// `Struct(name)` for these lets `load_enum_field` copy the full inline
    /// data via alloca+memcpy.
    ///
    /// Box/Ptr/Vec/str fields are one machine word wide (a pointer) and pass
    /// through `word_repr` unchanged.
    ///
    /// Falls back to the pattern's explicit annotation, then I64.
    fn pattern_binding_ir_types(
        &self,
        enum_name: &str,
        variant: &str,
        bindings: &[(String, String)],
    ) -> Vec<IrType> {
        self.pattern_binding_ast_types(enum_name, variant, bindings)
            .iter()
            .map(|t| {
                let ir_ty = self.lower_type(t);
                // Keep struct/enum types as-is so load_enum_field copies the
                // full inline data. Box<T> lowers to Ptr(Struct(T)) which is
                // already a word — word_repr leaves it unchanged.
                match ir_ty {
                    IrType::Struct(_) => ir_ty,
                    other => Self::word_repr(other),
                }
            })
            .collect()
    }

    /// Like `pattern_binding_ir_types` but returns the AST types, which
    /// we need to register in `var_ast_types` so that `infer_expr_type`
    /// can recover generic element types (e.g. `Vec<FieldInit>`).
    fn pattern_binding_ast_types(
        &self,
        enum_name: &str,
        variant: &str,
        bindings: &[(String, String)],
    ) -> Vec<Type> {
        let payload: Vec<Type> = self.lookup_pattern_enum(enum_name, variant)
            .and_then(|def| def.variants.iter().find(|v| v.name == variant))
            .and_then(|v| v.data.as_ref())
            .map(|params| params.iter().map(|p| p.ty.clone()).collect())
            .unwrap_or_default();

        bindings.iter().enumerate().map(|(i, (_, annot))| {
            let declared = payload.get(i).filter(|t| !self.is_opaque_type_param(t));
            match declared {
                Some(t) => t.clone(),
                None => self.ast_type_from_name(annot).unwrap_or(Type::I64),
            }
        }).collect()
    }

    /// Whether `ty` names a type parameter rather than a concrete type.
    fn is_opaque_type_param(&self, ty: &Type) -> bool {
        match ty {
            Type::Named(n) => {
                !self.enums.contains_key(n)
                    && self.module.get_struct(n).is_none()
                    && !self.type_aliases.contains_key(n)
                    && !matches!(n.as_str(), "Vec" | "HashMap" | "Tensor" | "Allocator")
            }
            _ => false,
        }
    }

    /// Narrow an IR type to how it is represented in a single machine word.
    /// Aggregates become pointers; scalars pass through unchanged.
    fn word_repr(ty: IrType) -> IrType {
        match ty {
            IrType::Struct(_) | IrType::Array(_, _) => IrType::Ptr(Box::new(ty)),
            other => other,
        }
    }

    /// If `val` is a Box<T> (Ptr(Ptr(Struct(..)))), load through it once
    /// so enum/struct pattern matching sees the inner struct pointer.
    /// Otherwise return `val` unchanged.
    ///
    /// NOTE: Box::new stores the struct data directly in the heap allocation
    /// via memcpy (not a pointer to the struct). So the Box pointer already
    /// points to the struct data, and match code can read the tag/fields
    /// directly from it. This function is now a no-op — the extra Load
    /// through the Box was reading the enum tag as a pointer, causing crashes
    /// at addresses like 0x4 (tag value for Ident).
    fn maybe_deref_box(&mut self, val: ValueId) -> ValueId {
        let _ty = self.get_value_type(val);
        // Box::new copies the full struct data into the heap allocation via
        // memcpy, so the Box pointer already points to the struct data.
        // No dereference needed — return val unchanged.
        val
    }

    /// Whether two IR types have the same machine-word representation.
    /// Used to decide if a `let` binding can use the declared type's IR type
    /// or must fall back to the value's actual IR type.
    fn ir_types_repr_compatible(a: &IrType, b: &IrType) -> bool {
        a == b
            || (a.is_scalar() && b.is_scalar())
            || (a.is_ptr_like() && b.is_ptr_like())
    }

    /// Parse a type name as written in a pattern annotation into an AST type.
    fn ast_type_from_name(&self, name: &str) -> Option<Type> {
        if name.is_empty() {
            return None;
        }
        let ty = match name {
            "i8" => Type::I8,
            "i16" => Type::I16,
            "i32" => Type::I32,
            "i64" => Type::I64,
            "i128" => Type::I128,
            "u8" => Type::U8,
            "u16" => Type::U16,
            "u32" => Type::U32,
            "u64" => Type::U64,
            "u128" => Type::U128,
            "f32" => Type::F32,
            "f64" => Type::F64,
            "bool" => Type::Bool,
            "str" => Type::Str,
            other => Type::Named(other.to_string()),
        };
        // Unknown named types would lower to an unregistered struct — reject
        // them so the caller falls back to I64.
        if self.is_opaque_type_param(&ty) {
            return None;
        }
        Some(ty)
    }

    /// Resolve `Self` to the current impl's type name
    fn resolve_self_in_type(&self, ty: &Type) -> Type {
        match ty {
            Type::Named(name) if name == "Self" => {
                if let Some(self_name) = &self.current_self_type {
                    Type::Named(self_name.clone())
                } else {
                    ty.clone()
                }
            }
            Type::Ref { inner, is_mut } => Type::Ref {
                inner: Box::new(self.resolve_self_in_type(inner)),
                is_mut: *is_mut,
            },
            Type::RawPtr { inner, is_mut } => Type::RawPtr {
                inner: Box::new(self.resolve_self_in_type(inner)),
                is_mut: *is_mut,
            },
            Type::Array { elem, size } => Type::Array {
                elem: Box::new(self.resolve_self_in_type(elem)),
                size: *size,
            },
            Type::ArrayConst { elem, size_param } => Type::ArrayConst {
                elem: Box::new(self.resolve_self_in_type(elem)),
                size_param: size_param.clone(),
            },
            Type::Generic { name, args } => Type::Generic {
                name: name.clone(),
                args: args.iter().map(|a| self.resolve_self_in_type(a)).collect(),
            },
            Type::Tuple(types) => Type::Tuple(
                types.iter().map(|t| self.resolve_self_in_type(t)).collect()
            ),
            other => other.clone(),
        }
    }

    /// Lower a sequence of statements
    fn lower_stmts(&mut self, stmts: &[Stmt]) {
        // L3A: When we encounter a `do` statement, the remaining statements
        // become the continuation. We create a continuation closure and pass
        // it to the handler. The handler can call `resume(cont, value)` to
        // run the continuation.
        let mut i = 0;
        while i < stmts.len() {
            if let Stmt::Do { effect, op, args, line } = &stmts[i] {
                let remaining = &stmts[i + 1..];
                self.lower_do_with_continuation(effect, op, args, *line, remaining);
                // After `do` with continuation capture, the remaining statements
                // are in the continuation closure — stop processing this block.
                return;
            }
            self.lower_stmt(&stmts[i]);
            i += 1;
        }
    }

    /// Lower a single statement
    fn lower_stmt(&mut self, stmt: &Stmt) {
        // Set current line for debug info
        self.current_line = Some(stmt.line());
        match stmt {
            Stmt::Let { name, ty, expr, is_mut: _, is_persist, annotations, line: _ } => {
                // Pass the declared type as a hint so Vec::new()/HashMap::new()
                // can pick up the element type from the annotation.
                let saved_hint = self.let_hint.take();
                if let Some(t) = ty {
                    self.let_hint = Some(self.resolve_self_in_type(t));
                }
                let val = self.lower_expr(expr);
                self.let_hint = saved_hint;
                // The alloca type normally follows the declared type. But when
                // the value is a pointer to a struct (e.g. from Vec_get) and
                // the declared type is a struct, the value's pointer type must
                // win — otherwise field access would treat the pointer as an
                // inline struct value.
                let val_ty = self.get_value_type(val);
                let declared_ir = ty.as_ref().map(|t| self.lower_type(t));
                // When the declared type is a struct and the value is a
                // pointer to that struct, use the declared (struct) type for
                // the alloca so it holds the full struct data inline. Using
                // the pointer type would only allocate 8 bytes, truncating
                // the struct on store.
                let ir_ty = match (declared_ir, &val_ty) {
                    (Some(d), v) if d.is_aggregate() && v.is_ptr() => {
                        // If the pointer points to the same struct type, use
                        // the struct type (inline storage). Otherwise (e.g.
                        // Vec_get returning *Struct), use the pointer type.
                        match v {
                            IrType::Ptr(inner) if inner.as_ref() == &d => d,
                            _ => v.clone(),
                        }
                    }
                    (Some(d), _) => d,
                    (None, _) => val_ty,
                };
                let alloca = self.emit_inst(Inst::Alloca {
                    ty: ir_ty.clone(),
                    region: self.current_region(),
                });
                self.alloca_types.insert(alloca, ir_ty.clone());
                // Save the type size for @zeroize before ir_ty is moved
                let ir_ty_size = self.module.type_size(&ir_ty);
                if *is_persist {
                    // Emit PersistLoad — returns saved value or the default
                    let loaded = self.emit_inst(Inst::PersistLoad {
                        name: name.clone(),
                        default: val,
                        ty: ir_ty,
                    });
                    self.emit_inst(Inst::Store {
                        ptr: alloca,
                        value: loaded,
                        store_ty: None,
                    });
                    if let Some(ref mut f) = self.current_func {
                        f.persist_vars.push((name.clone(), alloca));
                    }
                } else {
                    // For struct-typed alloca, use store_ty to emit memcpy
                    // (full struct copy). Without this, only 8 bytes (the
                    // pointer) are copied, truncating the struct data.
                    let store_ty = match &ir_ty {
                        IrType::Struct(_) => Some(ir_ty.clone()),
                        _ => None,
                    };
                    self.emit_inst(Inst::Store {
                        ptr: alloca,
                        value: val,
                        store_ty,
                    });
                }
                self.allocas.insert(name.clone(), alloca);
                // Record AST type for LetTuple element type inference
                if let Some(ast_ty) = ty {
                    self.var_ast_types.insert(name.clone(), ast_ty.clone());
                } else {
                    // Infer AST type from the expression (for calls returning tuples)
                    let inferred_ast = self.infer_expr_ast_type(expr);
                    if !matches!(inferred_ast, Type::I64) {
                        self.var_ast_types.insert(name.clone(), inferred_ast);
                    }
                }
                // Drop trait: check if the declared type has a Drop impl.
                // Do this before borrowing current_func mutably.
                let drop_struct = ty.as_ref().and_then(|t| self.type_struct_name(t))
                    .filter(|sn| self.has_drop_impl(sn));
                // @zeroize: check if the variable has @zeroize annotation
                let has_zeroize = annotations.iter().any(|a| a.name == "zeroize");
                let zeroize_size = if has_zeroize {
                    Some(ir_ty_size)
                } else {
                    None
                };
                if let Some(ref mut f) = self.current_func {
                    f.bind_local(name, alloca);
                    if let Some(struct_name) = drop_struct {
                        f.drop_locals.push((alloca, struct_name));
                    }
                    if let Some(size) = zeroize_size {
                        if size > 0 {
                            f.zeroize_locals.push((alloca, size));
                        }
                    }
                }
            }
            Stmt::LetUninit { name, ty, annotations, line: _ } => {
                // Uninitialized variable — allocate storage but don't store a value.
                // The typechecker enforces that it must be assigned before read.
                let ir_ty = self.lower_type(ty);
                let alloca = self.emit_inst(Inst::Alloca {
                    ty: ir_ty.clone(),
                    region: self.current_region(),
                });
                self.alloca_types.insert(alloca, ir_ty.clone());
                self.allocas.insert(name.clone(), alloca);
                // @zeroize: check if the variable has @zeroize annotation
                let has_zeroize = annotations.iter().any(|a| a.name == "zeroize");
                let zeroize_size = if has_zeroize {
                    Some(self.module.type_size(&ir_ty))
                } else {
                    None
                };
                if let Some(ref mut f) = self.current_func {
                    f.bind_local(name, alloca);
                    if let Some(size) = zeroize_size {
                        if size > 0 {
                            f.zeroize_locals.push((alloca, size));
                        }
                    }
                }
            }
            Stmt::LetTuple { names, expr, line: _ } => {
                // Lower the tuple expression — returns a pointer to a __tuple_N struct
                let tuple_ptr = self.lower_expr(expr);
                let n = names.len();

                // Determine element types by looking up the function's AST return type
                let elem_tys: Vec<IrType> = if let Expr::Call { callee, .. } = expr {
                    self.func_ret_types.get(callee)
                        .and_then(|ret_ty| match ret_ty {
                            Type::Tuple(elems) => Some(elems.iter().map(|t| self.lower_type(t)).collect()),
                            _ => None,
                        })
                        .unwrap_or_else(|| vec![IrType::I64; n])
                } else {
                    // For non-call expressions, try to get element types from
                    // the variable's AST type (recorded during Stmt::Let).
                    if let Expr::Ident(name) = expr {
                        if let Some(ast_ty) = self.var_ast_types.get(name) {
                            if let Type::Tuple(elems) = ast_ty {
                                elems.iter().map(|t| self.lower_type(t)).collect()
                            } else {
                                vec![IrType::I64; n]
                            }
                        } else {
                            vec![IrType::I64; n]
                        }
                    } else {
                        vec![IrType::I64; n]
                    }
                };

                // For each name, extract the corresponding field
                for (i, name) in names.iter().enumerate() {
                    let field_ty = elem_tys.get(i).cloned().unwrap_or(IrType::I64);
                    let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: tuple_ptr,
                        field_index: i as u32,
                        field_ty: field_ty.clone(),
                    });
                    if matches!(field_ty, IrType::Struct(_)) {
                        // Struct field: the struct data is stored inline in the
                        // tuple. Copy the struct data to a new alloca.
                        // The alloca IS the pointer to the struct data — the
                        // variable is bound to the alloca pointer directly.
                        let alloca = self.emit_inst(Inst::Alloca {
                            ty: field_ty.clone(),
                            region: self.current_region(),
                        });
                        self.alloca_types.insert(alloca, field_ty.clone());
                        // Copy struct data from tuple field to alloca
                        self.emit_inst(Inst::StructCopy {
                            dst: alloca,
                            src: field_ptr,
                            ty: field_ty.clone(),
                        });
                        self.allocas.insert(name.clone(), alloca);
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(name, alloca);
                        }
                    } else {
                        // Scalar field: load the value and store in alloca
                        let val = self.emit_inst(Inst::Load {
                            ptr: field_ptr,
                            ty: field_ty.clone(),
                        });
                        let alloca = self.emit_inst(Inst::Alloca {
                            ty: field_ty.clone(),
                            region: self.current_region(),
                        });
                        self.alloca_types.insert(alloca, field_ty.clone());
                        self.emit_inst(Inst::Store {
                            ptr: alloca,
                            value: val,
                            store_ty: None,
                        });
                        self.allocas.insert(name.clone(), alloca);
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(name, alloca);
                        }
                    }
                }
            }

            Stmt::Assign { target, op, expr, line: _ } => {
                // Check if target is a module-level mutable global
                if let Some((ir_ty, _init)) = self.module.globals.get(target).cloned() {
                    let addr = self.emit_inst(Inst::GlobalAddr { name: target.clone(), ty: ir_ty.clone() });
                    match op {
                        AssignOp::Assign => {
                            let val = self.lower_expr(expr);
                            self.emit_inst(Inst::Store { ptr: addr, value: val, store_ty: None });
                        }
                        _ => {
                            let current = self.emit_inst(Inst::Load { ptr: addr, ty: ir_ty.clone() });
                            let rhs = self.lower_expr(expr);
                            let new_val = self.lower_compound_assign(op, current, rhs);
                            self.emit_inst(Inst::Store { ptr: addr, value: new_val, store_ty: None });
                        }
                    }
                    return;
                }
                // Check if target is a mutable variable with an alloca
                let alloca = self.allocas.get(target).copied();
                
                if let Some(ptr) = alloca {
                    // Mutable variable: use load/store through alloca
                    match op {
                        AssignOp::Assign => {
                            let val = self.lower_expr(expr);
                            // A slot holding the struct itself takes a full
                            // copy (memcpy); a slot holding a pointer to the
                            // struct takes the new pointer: copying the struct
                            // into an 8-byte pointer slot overflowed it.
                            let val_ty = self.get_value_type(val);
                            let slot_holds_ptr = matches!(self.alloca_types.get(&ptr), Some(IrType::Ptr(_)));
                            let store_ty = match &val_ty {
                                IrType::Ptr(inner) if inner.is_aggregate() && !slot_holds_ptr => {
                                    Some(inner.as_ref().clone())
                                }
                                _ => None,
                            };
                            self.emit_inst(Inst::Store { ptr, value: val, store_ty });
                        }
                        _ => {
                            // Compound assignment: load current, apply op, store back
                            let load_ty = self.alloca_types.get(&ptr).cloned().unwrap_or(IrType::I64);
                            let current = self.emit_inst(Inst::Load {
                                ptr,
                                ty: load_ty,
                            });
                            let rhs = self.lower_expr(expr);
                            let new_val = self.lower_compound_assign(op, current, rhs);
                            self.emit_inst(Inst::Store { ptr, value: new_val, store_ty: None });
                        }
                    }
                } else {
                    // Non-alloca variable: use direct SSA binding
                    let val = self.lower_expr(expr);
                    match op {
                        AssignOp::Assign => {
                            if let Some(ref mut f) = self.current_func {
                                f.bind_local(target, val);
                            }
                        }
                        _ => {
                            let current = self.current_func.as_ref()
                                .and_then(|f| f.lookup_local(target));
                            if let Some(curr) = current {
                                let new_val = self.lower_compound_assign(op, curr, val);
                                if let Some(ref mut f) = self.current_func {
                                    f.bind_local(target, new_val);
                                }
                            }
                        }
                    }
                }
            }

            Stmt::IndexAssign { array, index, op, expr, line: _ } => {
                // Look up the array variable — it should be an alloca or local
                let arr_val = if let Some(alloca) = self.allocas.get(array) {
                    // Load the array pointer from the alloca
                    let arr_ty = self.alloca_types.get(alloca).cloned().unwrap_or(IrType::I64);
                    self.emit_inst(Inst::Load { ptr: *alloca, ty: arr_ty })
                } else if let Some(val) = self.current_func.as_ref().and_then(|f| f.lookup_local(array)) {
                    val
                } else {
                    self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                };

                let idx_val = self.lower_expr(index);

                // Determine element type from array value metadata
                let arr_ty = self.get_value_type(arr_val);

                // Vec indexing — call Fides Vec_set / Vec_get (Phase 10B)
                if arr_ty.is_opaque_named("Vec") {
                    let elem_ty = arr_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                    let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                    if *op == AssignOp::Assign {
                        let val = self.lower_expr(expr);
                        // Box struct values before Vec_set (same as push/insert/set method)
                        let val = if matches!(elem_ty, IrType::Struct(_)) {
                            let mut size = self.module.type_size(&elem_ty);
                            if size == 0 { size = 8; }
                            let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                            let heap_ptr = self.emit_inst(Inst::Call {
                                func: "p_malloc".to_string(),
                                args: vec![size_val],
                                ret_ty: IrType::Ptr(Box::new(elem_ty.clone())),
                                effects: {
                                    let mut e = EffectSet::new();
                                    e.add(Effect::Alloc);
                                    e
                                },
                            });
                            self.emit_inst(Inst::Store { ptr: heap_ptr, value: val, store_ty: Some(elem_ty.clone()) });
                            heap_ptr
                        } else {
                            val
                        };
                        self.emit_inst(Inst::Call {
                            func: "Vec_set".to_string(),
                            args: vec![dummy_env, arr_val, idx_val, val],
                            ret_ty: IrType::Void,
                            effects: EffectSet::new(),
                        });
                    } else {
                        // Compound assignment: Vec_get, binop, Vec_set
                        let dummy_env2 = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        let get_ret_ty = Self::word_repr(elem_ty.clone());
                        let current = self.emit_inst(Inst::Call {
                            func: "Vec_get".to_string(),
                            args: vec![dummy_env2, arr_val, idx_val],
                            ret_ty: get_ret_ty.clone(),
                            effects: EffectSet::new(),
                        });
                        let rhs = self.lower_expr(expr);
                        let bin_op = match op {
                            AssignOp::AddAssign => BinOp::Add,
                            AssignOp::SubAssign => BinOp::Sub,
                            AssignOp::MulAssign => BinOp::Mul,
                            AssignOp::DivAssign => BinOp::Div,
                            AssignOp::ModAssign => BinOp::Mod,
                            AssignOp::BitAndAssign => BinOp::BitAnd,
                            AssignOp::BitOrAssign => BinOp::BitOr,
                            AssignOp::BitXorAssign => BinOp::BitXor,
                            AssignOp::ShlAssign => BinOp::Shl,
                            AssignOp::ShrAssign => BinOp::Shr,
                            AssignOp::Assign => unreachable!(),
                        };
                        let new_val = self.emit_inst(Inst::BinOp {
                            op: bin_op,
                            left: current,
                            right: rhs,
                            ty: elem_ty.clone(),
                        });
                        // Box struct values before Vec_set
                        let new_val = if matches!(elem_ty, IrType::Struct(_)) {
                            let mut size = self.module.type_size(&elem_ty);
                            if size == 0 { size = 8; }
                            let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                            let heap_ptr = self.emit_inst(Inst::Call {
                                func: "p_malloc".to_string(),
                                args: vec![size_val],
                                ret_ty: IrType::Ptr(Box::new(elem_ty.clone())),
                                effects: {
                                    let mut e = EffectSet::new();
                                    e.add(Effect::Alloc);
                                    e
                                },
                            });
                            self.emit_inst(Inst::Store { ptr: heap_ptr, value: new_val, store_ty: Some(elem_ty.clone()) });
                            heap_ptr
                        } else {
                            new_val
                        };
                        let dummy_env3 = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        self.emit_inst(Inst::Call {
                            func: "Vec_set".to_string(),
                            args: vec![dummy_env3, arr_val, idx_val, new_val],
                            ret_ty: IrType::Void,
                            effects: EffectSet::new(),
                        });
                    }
                    return;
                }

                let elem_ty = match &arr_ty {
                    IrType::Array(inner, _) => *inner.clone(),
                    _ => IrType::I64,
                };

                let elem_ptr = self.emit_inst(Inst::GetElemPtr {
                    array_ptr: arr_val,
                    index: idx_val,
                    elem_ty: elem_ty.clone(),
                });

                if *op == AssignOp::Assign {
                    let val = self.lower_expr(expr);
                    self.emit_inst(Inst::Store { ptr: elem_ptr, value: val, store_ty: None });
                } else {
                    // Compound assignment: load, binop, store
                    let current = self.emit_inst(Inst::Load { ptr: elem_ptr, ty: elem_ty.clone() });
                    let rhs = self.lower_expr(expr);
                    let bin_op = match op {
                        AssignOp::AddAssign => BinOp::Add,
                        AssignOp::SubAssign => BinOp::Sub,
                        AssignOp::MulAssign => BinOp::Mul,
                        AssignOp::DivAssign => BinOp::Div,
                        AssignOp::ModAssign => BinOp::Mod,
                        AssignOp::BitAndAssign => BinOp::BitAnd,
                        AssignOp::BitOrAssign => BinOp::BitOr,
                        AssignOp::BitXorAssign => BinOp::BitXor,
                        AssignOp::ShlAssign => BinOp::Shl,
                        AssignOp::ShrAssign => BinOp::Shr,
                        AssignOp::Assign => unreachable!(),
                    };
                    let new_val = self.emit_inst(Inst::BinOp {
                        op: bin_op,
                        left: current,
                        right: rhs,
                        ty: elem_ty,
                    });
                    self.emit_inst(Inst::Store { ptr: elem_ptr, value: new_val, store_ty: None });
                }
            }

            Stmt::DerefAssign { ptr_expr, op, expr, line: _ } => {
                let ptr_val = self.lower_expr(ptr_expr);
                let pointee_ty = match self.get_value_type(ptr_val) {
                    IrType::Ptr(inner) => *inner,
                    _ => IrType::I64,
                };
                if *op == AssignOp::Assign {
                    let val = self.lower_expr(expr);
                    self.emit_inst(Inst::Store { ptr: ptr_val, value: val, store_ty: Some(pointee_ty) });
                } else {
                    let current = self.emit_inst(Inst::Load { ptr: ptr_val, ty: pointee_ty.clone() });
                    let rhs = self.lower_expr(expr);
                    let bin_op = match op {
                        AssignOp::AddAssign => BinOp::Add,
                        AssignOp::SubAssign => BinOp::Sub,
                        AssignOp::MulAssign => BinOp::Mul,
                        AssignOp::DivAssign => BinOp::Div,
                        AssignOp::ModAssign => BinOp::Mod,
                        AssignOp::BitAndAssign => BinOp::BitAnd,
                        AssignOp::BitOrAssign => BinOp::BitOr,
                        AssignOp::BitXorAssign => BinOp::BitXor,
                        AssignOp::ShlAssign => BinOp::Shl,
                        AssignOp::ShrAssign => BinOp::Shr,
                        AssignOp::Assign => unreachable!(),
                    };
                    let new_val = self.emit_inst(Inst::BinOp {
                        op: bin_op,
                        left: current,
                        right: rhs,
                        ty: pointee_ty.clone(),
                    });
                    self.emit_inst(Inst::Store { ptr: ptr_val, value: new_val, store_ty: Some(pointee_ty) });
                }
            }

            Stmt::FieldAssign { object, field, op, expr, line: _ } => {
                // Lower the object expression to get a pointer to the struct
                let struct_ptr = self.lower_expr(object);

                // Determine the struct type from the value metadata
                let struct_ty = self.get_value_type(struct_ptr);
                let struct_name = match &struct_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
                    _ => None,
                };

                if let Some(name) = struct_name {
                    if let Some(layout) = self.module.get_struct(&name).cloned() {
                        if let Some((idx, (_, field_ty, _))) = layout.fields.iter()
                            .enumerate()
                            .find(|(_, (n, _, _))| n == field)
                        {
                            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                struct_ptr,
                                field_index: idx as u32,
                                field_ty: field_ty.clone(),
                            });
                            if *op == AssignOp::Assign {
                                let val = self.lower_expr(expr);
                                self.emit_inst(Inst::Store { ptr: field_ptr, value: val, store_ty: None });
                            } else {
                                // Compound assignment: load current field value, apply op, store back
                                let current = self.emit_inst(Inst::Load {
                                    ptr: field_ptr,
                                    ty: field_ty.clone(),
                                });
                                let rhs = self.lower_expr(expr);
                                let new_val = self.lower_compound_assign(op, current, rhs);
                                self.emit_inst(Inst::Store { ptr: field_ptr, value: new_val, store_ty: None });
                            }
                        }
                    }
                }
            }

            Stmt::Expr(expr, _) => {
                self.lower_expr(expr);
            }

            Stmt::Return(expr, _) => {
                // Evaluate the returned value first. Drops, @zeroize wipes,
                // defers and persist saves then run, as at the closing brace;
                // running them first made `return f(key)` read a wiped key
                // and `defer { x = 0 } return x` return 0.
                let ret_val = if self.current_sret.is_some() {
                    // Struct-returning function: copy struct data to sret, return void
                    if let Some(e) = expr {
                        let val = self.lower_expr(e);
                        self.emit_sret_copy(val);
                    }
                    None
                } else {
                    match expr {
                        Some(e) => {
                            let v = self.lower_expr(e);
                            Some(self.coerce_return(v))
                        }
                        None => None,
                    }
                };

                // Emit compiler-inserted drops for locals with Drop impls
                self.emit_drops();

                // Emit defer blocks before returning (in reverse order)
                self.emit_defers();

                // Save persist variables before returning
                self.emit_persist_saves();

                self.emit_terminator(Inst::Return { value: ret_val });
            }

            Stmt::If { cond, then_block, else_block, line: _ } => {
                self.lower_if(cond, then_block, else_block.as_ref());
            }

            Stmt::While { cond, body, line: _ } => {
                self.lower_while(cond, body);
            }

            Stmt::For { var, iterator, body, line: _ } => {
                self.lower_for(var, iterator, body);
            }

            Stmt::Loop { body, line: _ } => {
                self.lower_loop(body);
            }

            Stmt::Break(expr, _) => {
                if let Some((_, break_block, result_slot)) = self.loop_stack.last() {
                    let break_target = *break_block;
                    let slot = *result_slot;
                    if let Some(e) = expr {
                        let val = self.lower_expr(e);
                        if let Some(slot) = slot {
                            self.emit_inst(Inst::Store { ptr: slot, value: val, store_ty: None });
                        }
                    }
                    self.emit_terminator(Inst::Jump { target: break_target });
                }
            }

            Stmt::Continue(_) => {
                if let Some((continue_block, _, _)) = self.loop_stack.last() {
                    let continue_target = *continue_block;
                    self.emit_terminator(Inst::Jump { target: continue_target });
                }
            }

            Stmt::Defer(body, _) => {
                // Register defer block — will be emitted at all exit points
                self.defer_stack.push(body.clone());
            }

            Stmt::Match { expr, arms, line: _ } => {
                self.lower_match(expr, arms);
            }

            Stmt::Use { .. } => {
                // Imports are handled at the AST level
            }

            Stmt::Handle { body, handlers, .. } => {
                // Effect handler: lower the body. When a `do Effect.op(args)` is
                // encountered inside, the matching handler is called.
                // We store the handlers in a stack so that Do statements can find them.
                self.effect_handler_stack.push(handlers.clone());
                self.lower_stmts(body);
                self.effect_handler_stack.pop();
            }

            Stmt::Do { effect, op, args, .. } => {
                // This is reached when `do` is nested inside other statements
                // (e.g., inside an if branch). In this case, we can't easily
                // capture the continuation (the remaining statements are not
                // in the same block). Fall back to inlining the handler body.
                // The continuation-aware path is in lower_stmts → lower_do_with_continuation.
                self.lower_do_inlined(effect, op, args);
            }
            Stmt::Resume { cont, value, .. } => {
                // L3A: resume(cont, value) — call the continuation closure.
                // The continuation is a closure value (fn pointer + env).
                // We call it with the resume value as the argument.
                let cont_val = self.lower_expr(cont);
                let value_val = self.lower_expr(value);
                // Call the continuation: cont(value)
                // The continuation closure has signature (env: *u8, value: i64) -> i64
                // We use CallPtr which passes env_ptr as first arg automatically.
                self.emit_inst(Inst::CallPtr {
                    fn_ptr: cont_val,
                    args: vec![value_val],
                    ret_ty: IrType::I64,
                    effects: EffectSet::new(),
                    has_env: true,
                });
            }
            Stmt::RegionBlock { name, budget: _, is_gpu, is_kv_cache, window_size, body, .. } => {
                // Emit RegionBegin — marks the start of a named region.
                // Allocations inside the block will use Region::Named(name).
                // If is_gpu, allocations go to GPU memory (ML1).
                // If is_kv_cache, allocations go to bounded KV cache (ML9).
                self.emit_inst(Inst::RegionBegin {
                    name: name.clone(),
                    is_gpu: *is_gpu,
                    is_kv_cache: *is_kv_cache,
                    window_size: *window_size,
                });
                self.region_stack.push(name.clone());
                if *is_gpu { self.gpu_region_stack.push(name.clone()); }
                for s in body {
                    self.lower_stmt(s);
                }
                self.region_stack.pop();
                if *is_gpu { self.gpu_region_stack.pop(); }
                // Emit RegionEnd — frees all memory allocated in this region.
                // Issue 48 pattern: don't emit if block already has a terminator (e.g. return)
                if !self.current_block_has_terminator() {
                    self.emit_inst(Inst::RegionEnd {
                        name: name.clone(),
                        is_gpu: *is_gpu,
                        is_kv_cache: *is_kv_cache,
                    });
                }
            }
            Stmt::ComptimeGen { .. } => {}
            Stmt::Sandbox { body, .. } => {
                // I-3C: Track sandbox context so the optimizer can do
                // sandbox-aware devirt/inline. Safety is still enforced at
                // compile time by the typechecker; this just propagates metadata.
                self.sandbox_depth += 1;
                for s in body {
                    self.lower_stmt(s);
                }
                self.sandbox_depth -= 1;
            }
            Stmt::Unsafe { body, .. } => {
                // F62: emit UnsafeBegin/UnsafeEnd so the interpreter can
                // skip bounds checks inside unsafe blocks. The typechecker
                // already enforces that raw pointer ops only happen here.
                self.emit_inst(Inst::UnsafeBegin);
                for s in body {
                    self.lower_stmt(s);
                }
                // Issue 48: Don't emit UnsafeEnd if the block already has a
                // terminator (e.g. `return` inside unsafe). The verifier
                // rejects blocks that don't end with a terminator, and
                // UnsafeEnd after a Return would be unreachable + violate that.
                if !self.current_block_has_terminator() {
                    self.emit_inst(Inst::UnsafeEnd);
                }
            }
            Stmt::Txn { body, .. } => {
                // Transaction block — emit TxnBegin, lower body, emit TxnEnd.
                // The interpreter snapshots memory on TxnBegin and restores
                // it on panic (before TxnEnd is reached). If TxnEnd is reached,
                // the transaction commits (snapshot is discarded).
                // In native code, TxnBegin/TxnEnd are no-ops (transactions are
                // handled at the application level).
                self.emit_inst(Inst::TxnBegin);
                for s in body {
                    self.lower_stmt(s);
                }
                // Issue 48 pattern: don't emit TxnEnd if block already terminated
                if !self.current_block_has_terminator() {
                    self.emit_inst(Inst::TxnEnd);
                }
            }
            Stmt::GradBlock { var, body, .. } => {
                // Emit GradBegin to enable gradient tracking in the interpreter.
                self.emit_inst(Inst::GradBegin { var_name: var.clone() });
                for s in body {
                    self.lower_stmt(s);
                }
                // Emit GradEnd to disable gradient tracking.
                if !self.current_block_has_terminator() {
                    self.emit_inst(Inst::GradEnd);
                }
            }
            Stmt::Backward { loss, .. } => {
                // Emit Backward instruction — triggers reverse-mode AD.
                let loss_val = self.lower_expr(loss);
                self.emit_inst(Inst::Backward { loss: loss_val });
            }
            Stmt::Query { body, .. } => {
                for s in body { self.lower_stmt(s); }
            }
            Stmt::Select { cases, default, line: _ } => {
                // Lower select as: try each channel with try_recv, if ready execute body and jump to merge.
                // If none ready and no default, block on first channel.
                // Simplified: poll all channels in order, first ready wins.
                let merge_block = self.new_block();

                if cases.is_empty() {
                    if let Some(ref def_body) = default {
                        for s in def_body { self.lower_stmt(s); }
                    }
                    self.emit_terminator(Inst::Jump { target: merge_block });
                    self.set_current_block(merge_block);
                    return;
                }

                // Evaluate all channel expressions first
                let chan_vals: Vec<ValueId> = cases.iter()
                    .map(|c| self.lower_expr(&c.channel))
                    .collect();

                // Create a temp alloca to store the received value across blocks
                let recv_alloca = self.emit_inst(Inst::Alloca {
                    ty: IrType::I64,
                    region: self.current_region(),
                });

                // For each case, try_recv and check if ready
                let mut case_blocks = Vec::new();
                let mut next_poll_blocks = Vec::new();
                for _ in 0..cases.len() {
                    case_blocks.push(self.new_block());
                    next_poll_blocks.push(self.new_block());
                }
                let default_block = self.new_block();

                // Jump to first poll block
                self.emit_terminator(Inst::Jump { target: next_poll_blocks[0] });

                // Chain: poll case 0, if ready go to case_block[0], else poll case 1, etc.
                for (i, _) in cases.iter().enumerate() {
                    // In next_poll_blocks[i]: try_recv on chan_vals[i]
                    self.set_current_block(next_poll_blocks[i]);
                    let try_recv = self.emit_inst(Inst::Call {
                        func: "p_chan_try_recv".to_string(),
                        args: vec![chan_vals[i]],
                        ret_ty: IrType::I64,
                        effects: EffectSet::new(),
                    });
                    // Store the received value in the temp alloca
                    self.emit_inst(Inst::Store { ptr: recv_alloca, value: try_recv, store_ty: None });
                    // Check if result is not PENDING_SENTINEL (i64::MIN + 1)
                    let sentinel = self.emit_inst(Inst::ConstInt {
                        value: i64::MIN + 1,
                        ty: IrType::I64,
                    });
                    let is_ready = self.emit_inst(Inst::BinOp {
                        op: BinOp::Neq,
                        left: try_recv,
                        right: sentinel,
                        ty: IrType::Bool,
                    });
                    let next = if i + 1 < cases.len() {
                        next_poll_blocks[i + 1]
                    } else {
                        default_block
                    };
                    self.emit_terminator(Inst::Branch {
                        cond: is_ready,
                        then: case_blocks[i],
                        else_: next,
                    });
                }

                // Case blocks: load value, bind var, execute body, jump to merge
                for (i, case) in cases.iter().enumerate() {
                    self.set_current_block(case_blocks[i]);
                    let recv_val = self.emit_inst(Inst::Load { ptr: recv_alloca, ty: IrType::I64 });
                    // Bind to var_name
                    if let Some(alloca) = self.allocas.get(&case.var_name).copied() {
                        self.emit_inst(Inst::Store { ptr: alloca, value: recv_val, store_ty: None });
                    } else {
                        let alloca = self.emit_inst(Inst::Alloca {
                            ty: IrType::I64,
                            region: self.current_region(),
                        });
                        self.emit_inst(Inst::Store { ptr: alloca, value: recv_val, store_ty: None });
                        self.allocas.insert(case.var_name.clone(), alloca);
                    }
                    for s in &case.body {
                        self.lower_stmt(s);
                    }
                    self.emit_terminator(Inst::Jump { target: merge_block });
                }

                // Default block
                self.set_current_block(default_block);
                if let Some(ref def_body) = default {
                    for s in def_body {
                        self.lower_stmt(s);
                    }
                }
                self.emit_terminator(Inst::Jump { target: merge_block });
                self.set_current_block(merge_block);
            }
            Stmt::Parallel { branches, line: _ } => {
                // F13: parallel { | branch1 | branch2 | ... }
                // Lower as fork-join parallelism using the executor system.
                // Each branch is spawned as a thread, then all are joined.
                // For branches that are single call expressions, spawn them directly.
                // For complex branches, execute sequentially (semantics preserved).
                //
                // Simplified lowering: execute branches sequentially.
                // This preserves semantics (all branches run, join at end) even
                // though it's not actually parallel. True parallelism requires
                // closure support which is not yet available.
                for branch in branches {
                    for s in branch {
                        self.lower_stmt(s);
                    }
                }
            }
            Stmt::SpawnScope { body, line: _ } => {
                // L3B: spawn scope { body } — structured concurrency.
                // Create an executor, run the body (spawn expressions use it),
                // then join all tasks at scope exit.
                let exec = self.emit_inst(Inst::Call {
                    func: "p_executor_new".to_string(),
                    args: vec![],
                    ret_ty: IrType::I64,
                    effects: EffectSet::new(),
                });
                // Push executor onto scope stack so spawn expressions can use it
                self.scope_executor_stack.push(exec);
                // Lower the body
                self.lower_stmts(body);
                // Pop executor
                self.scope_executor_stack.pop();
                // Join all tasks: p_executor_run_all(exec)
                self.emit_inst(Inst::Call {
                    func: "p_executor_run_all".to_string(),
                    args: vec![exec],
                    ret_ty: IrType::I64,
                    effects: EffectSet::new(),
                });
            }
            Stmt::CancelScope { line: _ } => {
                // L3E: cancel scope; — cancel all pending tasks in current scope
                if let Some(exec) = self.scope_executor_stack.last().copied() {
                    self.emit_inst(Inst::Call {
                        func: "p_executor_cancel".to_string(),
                        args: vec![exec],
                        ret_ty: IrType::I64,
                        effects: EffectSet::new(),
                    });
                }
            }
        }
    }

    /// Lower an if statement
    fn lower_if(&mut self, cond: &Expr, then_block: &[Stmt], else_block: Option<&Vec<Stmt>>) {
        let cond_val = self.lower_expr(cond);

        let then_block_id = self.new_block();
        let else_block_id = self.new_block();
        let merge_block_id = self.new_block();

        // Emit branch
        self.emit_terminator(Inst::Branch {
            cond: cond_val,
            then: then_block_id,
            else_: else_block_id,
        });

        // Save variable bindings so they don't leak across branches.
        // Variables declared inside then/else blocks should not be visible
        // outside those blocks (or in the other branch).
        let saved_allocas = self.allocas.clone();
        let saved_alloca_types = self.alloca_types.clone();
        let saved_var_ast_types = self.var_ast_types.clone();
        let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());

        // Lower then block
        self.set_current_block(then_block_id);
        self.lower_stmts(then_block);
        // If then block doesn't end with terminator, jump to merge
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: merge_block_id });
        }

        // Restore variable bindings for else block
        self.allocas = saved_allocas.clone();
        self.alloca_types = saved_alloca_types.clone();
        self.var_ast_types = saved_var_ast_types.clone();
        if let Some(ref mut f) = self.current_func {
            f.locals = saved_locals.clone().unwrap_or_default();
        }

        // Lower else block
        self.set_current_block(else_block_id);
        if let Some(else_stmts) = else_block {
            self.lower_stmts(else_stmts);
        }
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: merge_block_id });
        }

        // Restore variable bindings for merge block
        self.allocas = saved_allocas;
        self.alloca_types = saved_alloca_types;
        self.var_ast_types = saved_var_ast_types;
        if let Some(ref mut f) = self.current_func {
            f.locals = saved_locals.unwrap_or_default();
        }

        // Continue in merge block
        self.set_current_block(merge_block_id);
    }

    /// Lower a while loop
    fn lower_while(&mut self, cond: &Expr, body: &[Stmt]) {
        let cond_block = self.new_block();
        let body_block = self.new_block();
        let exit_block = self.new_block();

        // Jump to condition block
        self.emit_terminator(Inst::Jump { target: cond_block });

        // Condition block
        self.set_current_block(cond_block);
        let cond_val = self.lower_expr(cond);
        self.emit_terminator(Inst::Branch {
            cond: cond_val,
            then: body_block,
            else_: exit_block,
        });

        // Body block
        self.set_current_block(body_block);
        self.loop_stack.push((cond_block, exit_block, None));
        self.lower_stmts(body);
        self.loop_stack.pop();
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: cond_block });
        }

        // Continue after loop
        self.set_current_block(exit_block);
    }

    /// Lower a for loop (for x in iterator { body })
    fn lower_for(&mut self, var: &str, iterator: &Expr, body: &[Stmt]) {
        // For ranges: for x in start..end { body }
        // Lowered to: let x = start; while x < end { body; x += 1; }
        match iterator {
            Expr::Range { start, end, inclusive } => {
                let start_val = self.lower_expr(start);
                let end_val = self.lower_expr(end);

                // Create alloca for loop variable (must be mutable across iterations)
                let alloca = self.emit_inst(Inst::Alloca {
                    ty: IrType::I64,
                    region: self.current_region(),
                });
                self.emit_inst(Inst::Store {
                    ptr: alloca,
                    value: start_val,
                    store_ty: None,
                });
                self.allocas.insert(var.to_string(), alloca);
                if let Some(ref mut f) = self.current_func {
                    f.bind_local(var, alloca);
                }

                let cond_block = self.new_block();
                let body_block = self.new_block();
                let exit_block = self.new_block();

                self.emit_terminator(Inst::Jump { target: cond_block });

                // Condition: var < end (or var <= end for inclusive)
                self.set_current_block(cond_block);
                let var_val = self.emit_inst(Inst::Load {
                    ptr: alloca,
                    ty: IrType::I64,
                });
                let cmp_op = if *inclusive { BinOp::LtEq } else { BinOp::Lt };
                let cond_val = self.emit_inst(Inst::BinOp {
                    op: cmp_op,
                    left: var_val,
                    right: end_val,
                    ty: IrType::Bool,
                });
                self.emit_terminator(Inst::Branch {
                    cond: cond_val,
                    then: body_block,
                    else_: exit_block,
                });

                // Body
                self.set_current_block(body_block);
                self.loop_stack.push((cond_block, exit_block, None));
                self.lower_stmts(body);
                self.loop_stack.pop();

                // Increment: var += 1
                if !self.current_block_has_terminator() {
                    let current = self.emit_inst(Inst::Load {
                        ptr: alloca,
                        ty: IrType::I64,
                    });
                    let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
                    let inc = self.emit_inst(Inst::BinOp {
                        op: BinOp::Add,
                        left: current,
                        right: one,
                        ty: IrType::I64,
                    });
                    self.emit_inst(Inst::Store {
                        ptr: alloca,
                        value: inc,
                        store_ty: None,
                    });
                    self.emit_terminator(Inst::Jump { target: cond_block });
                }

                self.set_current_block(exit_block);
            }
            _ => {
                // Check if iterator is a Vec (Opaque type)
                let iter_val = self.lower_expr(iterator);
                let iter_ty = self.get_value_type(iter_val);
                if iter_ty.is_opaque_named("Vec") {
                    self.lower_for_vec(var, iter_val, body);
                    return;
                }
                // Check if iterator is a HashMap — iterate over keys
                if iter_ty.is_opaque_named("HashMap") {
                    // for k in map → get keys as Vec, then iterate
                    // Phase 10B: call Fides HashMap_keys (user function — needs env_ptr)
                    let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                    let keys_vec = self.emit_inst(Inst::Call {
                        func: "HashMap_keys".to_string(),
                        args: vec![dummy_env, iter_val],
                        ret_ty: IrType::Opaque("Vec".to_string()),
                        effects: EffectSet::new(),
                    });
                    self.lower_for_vec(var, keys_vec, body);
                    return;
                }
                // Check if iterator is an opaque "any" or I64 value from a Vec element
                // (nested Vec iteration: for row in vec_of_vecs)
                // Vec elements are stored as i64 handles, so we check if the value
                // comes from a Vec-element alloca or directly from Vec_get.
                // NOTE: This check MUST come before the string check below,
                // because Vec elements are typed as I64 (same as string pointers).
                if iter_ty.is_opaque_named("any") || iter_ty == IrType::I64 {
                    if let Some(ref f) = self.current_func {
                        // Check if the value is loaded from a Vec-element alloca
                        if let Some(Inst::Load { ptr, .. }) = f.instructions.get(&iter_val) {
                            if self.vec_allocas.contains(ptr) {
                                self.lower_for_vec(var, iter_val, body);
                                return;
                            }
                        }
                        // Check if the value came directly from Vec_get (Phase 10B)
                        if let Some(Inst::Call { func, .. }) = f.instructions.get(&iter_val) {
                            if func == "Vec_get" {
                                self.lower_for_vec(var, iter_val, body);
                                return;
                            }
                        }
                    }
                }
                // Check if iterator is a string (i64 pointer to null-terminated bytes)
                // for c in str → while loop using String_byte_at(str, i)
                if iter_ty == IrType::I64 {
                    self.lower_for_string(var, iter_val, body);
                    return;
                }
                // Array iterator: for x in array { body }
                // Lowered to: let i = 0; while i < len { let x = arr[i]; body; i += 1; }
                let arr_val = iter_val;
                let arr_ty = self.get_value_type(arr_val);
                // Handle both direct array and pointer-to-array (from mutable var load)
                let (elem_ty, arr_len) = match &arr_ty {
                    IrType::Array(inner, len) => (*inner.clone(), *len),
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Array(inner2, len2) => (*inner2.clone(), *len2),
                        _ => (IrType::I64, 0),
                    },
                    _ => (IrType::I64, 0),
                };

                // Create alloca for loop variable
                let var_alloca = self.emit_inst(Inst::Alloca {
                    ty: elem_ty.clone(),
                    region: self.current_region(),
                });
                self.alloca_types.insert(var_alloca, elem_ty.clone());
                self.allocas.insert(var.to_string(), var_alloca);
                if let Some(ref mut f) = self.current_func {
                    f.bind_local(var, var_alloca);
                }

                // Create index alloca
                let idx_alloca = self.emit_inst(Inst::Alloca {
                    ty: IrType::I64,
                    region: self.current_region(),
                });
                self.alloca_types.insert(idx_alloca, IrType::I64);
                let zero = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                self.emit_inst(Inst::Store {
                    ptr: idx_alloca,
                    value: zero,
                    store_ty: None,
                });

                let cond_block = self.new_block();
                let body_block = self.new_block();
                let exit_block = self.new_block();

                self.emit_terminator(Inst::Jump { target: cond_block });

                // Condition: i < arr_len
                self.set_current_block(cond_block);
                let idx_val = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
                let len_val = self.emit_inst(Inst::ConstInt { value: arr_len as i64, ty: IrType::I64 });
                let cond_val = self.emit_inst(Inst::BinOp {
                    op: BinOp::Lt,
                    left: idx_val,
                    right: len_val,
                    ty: IrType::Bool,
                });
                self.emit_terminator(Inst::Branch {
                    cond: cond_val,
                    then: body_block,
                    else_: exit_block,
                });

                // Body: load arr[i], store to var_alloca, lower body, increment
                self.set_current_block(body_block);
                let cur_idx = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
                let elem_ptr = self.emit_inst(Inst::GetElemPtr {
                    array_ptr: arr_val,
                    index: cur_idx,
                    elem_ty: elem_ty.clone(),
                });
                let elem_val = self.emit_inst(Inst::Load { ptr: elem_ptr, ty: elem_ty });
                self.emit_inst(Inst::Store { ptr: var_alloca, value: elem_val, store_ty: None });

                self.loop_stack.push((cond_block, exit_block, None));
                self.lower_stmts(body);
                self.loop_stack.pop();

                if !self.current_block_has_terminator() {
                    let cur = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
                    let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
                    let inc = self.emit_inst(Inst::BinOp {
                        op: BinOp::Add,
                        left: cur,
                        right: one,
                        ty: IrType::I64,
                    });
                    self.emit_inst(Inst::Store { ptr: idx_alloca, value: inc, store_ty: None });
                    self.emit_terminator(Inst::Jump { target: cond_block });
                }

                self.set_current_block(exit_block);
            }
        }
    }

    /// Lower `for x in vec { body }` — iterates Vec via Vec_len/Vec_get (Phase 10B)
    fn lower_for_vec(&mut self, var: &str, vec_val: ValueId, body: &[Stmt]) {
        // Determine element type: if the Vec's type is Generic("Vec", [elem]),
        // use elem as the alloca type. Otherwise default to Opaque("any").
        let vec_ty = self.get_value_type(vec_val);
        let elem_ty = match &vec_ty {
            IrType::Generic(_, args) => {
                if let Some(elem) = args.first() {
                    elem.clone()
                } else {
                    IrType::Opaque("any".to_string())
                }
            }
            _ => IrType::Opaque("any".to_string()),
        };
        // Create alloca for loop variable — use Opaque so Load checks side-table
        let var_alloca = self.emit_inst(Inst::Alloca {
            ty: elem_ty.clone(),
            region: self.current_region(),
        });
        self.alloca_types.insert(var_alloca, elem_ty.clone());
        self.allocas.insert(var.to_string(), var_alloca);
        // Track this alloca as holding a Vec element (for nested iteration)
        self.vec_allocas.insert(var_alloca);
        if let Some(ref mut f) = self.current_func {
            f.bind_local(var, var_alloca);
        }

        // Create index alloca
        let idx_alloca = self.emit_inst(Inst::Alloca {
            ty: IrType::I64,
            region: self.current_region(),
        });
        self.alloca_types.insert(idx_alloca, IrType::I64);
        let zero = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        self.emit_inst(Inst::Store {
            ptr: idx_alloca,
            value: zero,
            store_ty: None,
        });

        let cond_block = self.new_block();
        let body_block = self.new_block();
        let exit_block = self.new_block();

        self.emit_terminator(Inst::Jump { target: cond_block });

        // Condition: i < vec.len()
        self.set_current_block(cond_block);
        let idx_val = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let len_val = self.emit_inst(Inst::Call {
            func: "Vec_len".to_string(),
            args: vec![dummy_env, vec_val],
            ret_ty: IrType::I64,
            effects: EffectSet::new(),
        });
        let cond_val = self.emit_inst(Inst::BinOp {
            op: BinOp::Lt,
            left: idx_val,
            right: len_val,
            ty: IrType::Bool,
        });
        self.emit_terminator(Inst::Branch {
            cond: cond_val,
            then: body_block,
            else_: exit_block,
        });

        // Body: vec.get(i), store to var_alloca, lower body, increment
        self.set_current_block(body_block);
        let cur_idx = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
        let dummy_env2 = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let get_ret_ty = Self::word_repr(elem_ty.clone());
        let elem_val = self.emit_inst(Inst::Call {
            func: "Vec_get".to_string(),
            args: vec![dummy_env2, vec_val, cur_idx],
            ret_ty: get_ret_ty,
            effects: EffectSet::new(),
        });
        self.emit_inst(Inst::Store { ptr: var_alloca, value: elem_val, store_ty: None });

        self.loop_stack.push((cond_block, exit_block, None));
        self.lower_stmts(body);
        self.loop_stack.pop();

        if !self.current_block_has_terminator() {
            let cur = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
            let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
            let inc = self.emit_inst(Inst::BinOp {
                op: BinOp::Add,
                left: cur,
                right: one,
                ty: IrType::I64,
            });
            self.emit_inst(Inst::Store { ptr: idx_alloca, value: inc, store_ty: None });
            self.emit_terminator(Inst::Jump { target: cond_block });
        }

        self.set_current_block(exit_block);
    }

    /// Lower `for c in str` — iterate over bytes of a null-terminated string.
    /// Desugars to: let i = 0; while i < p_str_length(str) { let c = String_byte_at(str, i); body; i += 1; }
    fn lower_for_string(&mut self, var: &str, str_val: ValueId, body: &[Stmt]) {
        // Create alloca for loop variable (byte value as i64)
        let var_alloca = self.emit_inst(Inst::Alloca {
            ty: IrType::I64,
            region: self.current_region(),
        });
        self.alloca_types.insert(var_alloca, IrType::I64);
        self.allocas.insert(var.to_string(), var_alloca);
        if let Some(ref mut f) = self.current_func {
            f.bind_local(var, var_alloca);
        }

        // Create index alloca
        let idx_alloca = self.emit_inst(Inst::Alloca {
            ty: IrType::I64,
            region: self.current_region(),
        });
        self.alloca_types.insert(idx_alloca, IrType::I64);
        let zero = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        self.emit_inst(Inst::Store {
            ptr: idx_alloca,
            value: zero,
            store_ty: None,
        });

        // Get string length once (hoist out of loop)
        // p_str_length is a user function — prepend dummy env_ptr
        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let len_val = self.emit_inst(Inst::Call {
            func: "p_str_length".to_string(),
            args: vec![dummy_env, str_val],
            ret_ty: IrType::I64,
            effects: EffectSet::new(),
        });

        let cond_block = self.new_block();
        let body_block = self.new_block();
        let exit_block = self.new_block();

        self.emit_terminator(Inst::Jump { target: cond_block });

        // Condition: i < len
        self.set_current_block(cond_block);
        let idx_val = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
        let cond_val = self.emit_inst(Inst::BinOp {
            op: BinOp::Lt,
            left: idx_val,
            right: len_val,
            ty: IrType::Bool,
        });
        self.emit_terminator(Inst::Branch {
            cond: cond_val,
            then: body_block,
            else_: exit_block,
        });

        // Body: c = String_byte_at(str, i), store to var_alloca, lower body, increment
        self.set_current_block(body_block);
        let cur_idx = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
        let dummy_env2 = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let byte_val = self.emit_inst(Inst::Call {
            func: "String_byte_at".to_string(),
            args: vec![dummy_env2, str_val, cur_idx],
            ret_ty: IrType::I64,
            effects: EffectSet::new(),
        });
        self.emit_inst(Inst::Store { ptr: var_alloca, value: byte_val, store_ty: None });

        self.loop_stack.push((cond_block, exit_block, None));
        self.lower_stmts(body);
        self.loop_stack.pop();

        if !self.current_block_has_terminator() {
            let cur = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
            let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
            let inc = self.emit_inst(Inst::BinOp {
                op: BinOp::Add,
                left: cur,
                right: one,
                ty: IrType::I64,
            });
            self.emit_inst(Inst::Store { ptr: idx_alloca, value: inc, store_ty: None });
            self.emit_terminator(Inst::Jump { target: cond_block });
        }

        self.set_current_block(exit_block);
    }

    /// Lower an infinite loop
    fn lower_loop(&mut self, body: &[Stmt]) {
        let loop_block = self.new_block();
        let exit_block = self.new_block();

        self.emit_terminator(Inst::Jump { target: loop_block });

        self.set_current_block(loop_block);
        self.loop_stack.push((loop_block, exit_block, None));
        self.lower_stmts(body);
        self.loop_stack.pop();
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: loop_block });
        }

        self.set_current_block(exit_block);
    }

    /// Lower a match statement (lowered to if/else chains)
    fn lower_match(&mut self, expr: &Expr, arms: &[MatchArm]) {
        let match_val = self.lower_expr(expr);
        // If the scrutinee is a Box<T> (pointer to pointer to struct),
        // dereference it once so enum/struct patterns see the inner value.
        let match_val = self.maybe_deref_box(match_val);

        let merge_block = self.new_block();

        for arm in arms {
            let match_block = self.new_block();
            let next_arm_block = self.new_block();

            // Check pattern against match_val
            match &arm.pattern {
                Pattern::Wildcard => {
                    self.emit_terminator(Inst::Jump { target: match_block });
                }
                Pattern::IntLit(n) => {
                    let const_val = self.emit_inst(Inst::ConstInt { value: *n, ty: IrType::I64 });
                    let cmp = self.emit_inst(Inst::BinOp {
                        op: BinOp::Eq,
                        left: match_val,
                        right: const_val,
                        ty: IrType::Bool,
                    });
                    self.emit_terminator(Inst::Branch {
                        cond: cmp,
                        then: match_block,
                        else_: next_arm_block,
                    });
                }
                Pattern::BoolLit(b) => {
                    let const_val = self.emit_inst(Inst::ConstBool { value: *b });
                    let cmp = self.emit_inst(Inst::BinOp {
                        op: BinOp::Eq,
                        left: match_val,
                        right: const_val,
                        ty: IrType::Bool,
                    });
                    self.emit_terminator(Inst::Branch {
                        cond: cmp,
                        then: match_block,
                        else_: next_arm_block,
                    });
                }
                Pattern::Ident(name) => {
                    // Bind the match value to this identifier.
                    // Remove any stale alloca binding so the local binding is used.
                    self.allocas.remove(name);
                    if let Some(ref mut f) = self.current_func {
                        f.bind_local(name, match_val);
                    }
                    self.emit_terminator(Inst::Jump { target: match_block });
                }
                Pattern::StrLit(s) => {
                    let const_val = self.emit_inst(Inst::ConstStr { value: s.clone() });
                    let cmp = self.emit_inst(Inst::BinOp {
                        op: BinOp::Eq,
                        left: match_val,
                        right: const_val,
                        ty: IrType::Bool,
                    });
                    self.emit_terminator(Inst::Branch {
                        cond: cmp,
                        then: match_block,
                        else_: next_arm_block,
                    });
                }
                Pattern::Range { start, end } => {
                    // Range pattern: start..end (non-inclusive)
                    // Extract integer values from sub-patterns
                    let (lo, hi) = match (start.as_ref(), end.as_ref()) {
                        (Pattern::IntLit(s), Pattern::IntLit(e)) => (*s, *e),
                        _ => { self.emit_terminator(Inst::Jump { target: next_arm_block }); continue; }
                    };
                    let start_val = self.emit_inst(Inst::ConstInt { value: lo, ty: IrType::I64 });
                    let end_val = self.emit_inst(Inst::ConstInt { value: hi, ty: IrType::I64 });
                    let lo_cmp = self.emit_inst(Inst::BinOp { op: BinOp::LtEq, left: start_val, right: match_val, ty: IrType::Bool });
                    let hi_cmp = self.emit_inst(Inst::BinOp { op: BinOp::Lt, left: match_val, right: end_val, ty: IrType::Bool });
                    let in_range = self.emit_inst(Inst::BinOp { op: BinOp::And, left: lo_cmp, right: hi_cmp, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: in_range, then: match_block, else_: next_arm_block });
                }
                Pattern::EnumVariant { enum_name, variant, bindings } => {
                    // Load the tag (field 0) and compare to variant index
                    let variant_idx = if enum_name.is_empty() {
                        // Shorthand pattern (e.g. Some(x) without Option:: prefix)
                        // Search all enums for a matching variant name
                        let mut found = None;
                        for (_, enum_def) in &self.enums {
                            if let Some(idx) = enum_def.variants.iter().position(|v| v.name == *variant) {
                                found = Some(idx as i64);
                                break;
                            }
                        }
                        found.unwrap_or(0)
                    } else if let Some(enum_def) = self.enums.get(enum_name) {
                        enum_def.variants.iter().position(|v| v.name == *variant).unwrap_or(0) as i64
                    } else {
                        0
                    };
                    let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: match_val,
                        field_index: 0,
                        field_ty: IrType::I64,
                    });
                    let tag = self.emit_inst(Inst::Load { ptr: tag_ptr, ty: IrType::I64 });
                    let expected = self.emit_inst(Inst::ConstInt { value: variant_idx, ty: IrType::I64 });
                    let cmp = self.emit_inst(Inst::BinOp {
                        op: BinOp::Eq,
                        left: tag,
                        right: expected,
                        ty: IrType::Bool,
                    });
                    self.emit_terminator(Inst::Branch {
                        cond: cmp,
                        then: match_block,
                        else_: next_arm_block,
                    });
                    // Bind data fields in the match block
                    self.set_current_block(match_block);
                    // Save variable bindings so arm-local bindings don't leak
                    // into other arms or code after the match.
                    let saved_allocas = self.allocas.clone();
                    let saved_alloca_types = self.alloca_types.clone();
                    let saved_var_ast_types = self.var_ast_types.clone();
                    let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());
                    let binding_tys = self.pattern_binding_ir_types(enum_name, variant, bindings);
                    let binding_ast_tys = self.pattern_binding_ast_types(enum_name, variant, bindings);
                    // Compute field indices: struct-typed fields occupy multiple
                    // slots (inline storage), so we accumulate the offset.
                    let mut field_idx: u32 = 1;
                    for (i, (binding, _ty_name)) in bindings.iter().enumerate() {
                        let field_ty = binding_tys.get(i).cloned().unwrap_or(IrType::I64);
                        let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                            struct_ptr: match_val,
                            field_index: field_idx,
                            field_ty: field_ty.clone(),
                        });
                        let field_val = self.load_enum_field(field_ptr, &field_ty);
                        // Remove any stale alloca binding for this name so that
                        // the local binding (via bind_local) is used instead.
                        self.allocas.remove(binding);
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(binding, field_val);
                        }
                        // Register AST type so infer_expr_type can recover
                        // generic element types (e.g. Vec<FieldInit>).
                        if let Some(ast_ty) = binding_ast_tys.get(i) {
                            self.var_ast_types.insert(binding.clone(), ast_ty.clone());
                        }
                        // Advance field_idx by the number of slots this field occupies
                        field_idx += self.enum_field_slots(&field_ty);
                    }
                    // If there's a guard, evaluate it and branch
                    if let Some(guard_expr) = &arm.guard {
                        let guard_val = self.lower_expr(guard_expr);
                        let guard_body_block = self.new_block();
                        self.emit_terminator(Inst::Branch {
                            cond: guard_val,
                            then: guard_body_block,
                            else_: next_arm_block,
                        });
                        self.set_current_block(guard_body_block);
                    }
                    // Skip the normal body lowering since we already set the block
                    self.lower_stmts(&arm.body);
                    if !self.current_block_has_terminator() {
                        self.emit_terminator(Inst::Jump { target: merge_block });
                    }
                    // Restore variable bindings
                    self.allocas = saved_allocas;
                    self.alloca_types = saved_alloca_types;
                    self.var_ast_types = saved_var_ast_types;
                    if let Some(ref mut f) = self.current_func {
                        f.locals = saved_locals.unwrap_or_default();
                    }
                    self.set_current_block(next_arm_block);
                    continue;
                }
            }

            // Lower match arm body
            self.set_current_block(match_block);
            // Save variable bindings so arm-local bindings don't leak
            let saved_allocas = self.allocas.clone();
            let saved_alloca_types = self.alloca_types.clone();
            let saved_var_ast_types = self.var_ast_types.clone();
            let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());

            // If there's a guard, evaluate it and branch
            if let Some(guard_expr) = &arm.guard {
                let guard_val = self.lower_expr(guard_expr);
                let guard_body_block = self.new_block();
                self.emit_terminator(Inst::Branch {
                    cond: guard_val,
                    then: guard_body_block,
                    else_: next_arm_block,
                });
                self.set_current_block(guard_body_block);
            }

            self.lower_stmts(&arm.body);
            if !self.current_block_has_terminator() {
                self.emit_terminator(Inst::Jump { target: merge_block });
            }
            // Restore variable bindings
            self.allocas = saved_allocas;
            self.alloca_types = saved_alloca_types;
            self.var_ast_types = saved_var_ast_types;
            if let Some(ref mut f) = self.current_func {
                f.locals = saved_locals.unwrap_or_default();
            }

            // Continue to next arm
            self.set_current_block(next_arm_block);
        }

        // No arm matched — jump to merge
        self.emit_terminator(Inst::Jump { target: merge_block });
        self.set_current_block(merge_block);
    }

    /// Lower a match expression — stores arm result into result_slot
    fn lower_match_expr(&mut self, expr: &Expr, arms: &[MatchArm], result_slot: ValueId) {
        let match_val = self.lower_expr(expr);
        // If the scrutinee is a Box<T> (pointer to pointer to struct),
        // dereference it once so enum/struct patterns see the inner value.
        let match_val = self.maybe_deref_box(match_val);
        let merge_block = self.new_block();

        for arm in arms {
            let match_block = self.new_block();
            let next_arm_block = self.new_block();

            match &arm.pattern {
                Pattern::Wildcard => {
                    self.emit_terminator(Inst::Jump { target: match_block });
                }
                Pattern::IntLit(n) => {
                    let const_val = self.emit_inst(Inst::ConstInt { value: *n, ty: IrType::I64 });
                    let cmp = self.emit_inst(Inst::BinOp { op: BinOp::Eq, left: match_val, right: const_val, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: cmp, then: match_block, else_: next_arm_block });
                }
                Pattern::BoolLit(b) => {
                    let const_val = self.emit_inst(Inst::ConstBool { value: *b });
                    let cmp = self.emit_inst(Inst::BinOp { op: BinOp::Eq, left: match_val, right: const_val, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: cmp, then: match_block, else_: next_arm_block });
                }
                Pattern::Ident(name) => {
                    self.allocas.remove(name);
                    if let Some(ref mut f) = self.current_func {
                        f.bind_local(name, match_val);
                    }
                    self.emit_terminator(Inst::Jump { target: match_block });
                }
                Pattern::StrLit(s) => {
                    let const_val = self.emit_inst(Inst::ConstStr { value: s.clone() });
                    let cmp = self.emit_inst(Inst::BinOp { op: BinOp::Eq, left: match_val, right: const_val, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: cmp, then: match_block, else_: next_arm_block });
                }
                Pattern::Range { start, end } => {
                    let (lo, hi) = match (start.as_ref(), end.as_ref()) {
                        (Pattern::IntLit(s), Pattern::IntLit(e)) => (*s, *e),
                        _ => { self.emit_terminator(Inst::Jump { target: next_arm_block }); continue; }
                    };
                    let start_val = self.emit_inst(Inst::ConstInt { value: lo, ty: IrType::I64 });
                    let end_val = self.emit_inst(Inst::ConstInt { value: hi, ty: IrType::I64 });
                    let lo_cmp = self.emit_inst(Inst::BinOp { op: BinOp::LtEq, left: start_val, right: match_val, ty: IrType::Bool });
                    let hi_cmp = self.emit_inst(Inst::BinOp { op: BinOp::Lt, left: match_val, right: end_val, ty: IrType::Bool });
                    let in_range = self.emit_inst(Inst::BinOp { op: BinOp::And, left: lo_cmp, right: hi_cmp, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: in_range, then: match_block, else_: next_arm_block });
                }
                Pattern::EnumVariant { enum_name, variant, bindings } => {
                    let variant_idx = if enum_name.is_empty() {
                        let mut found = None;
                        for (_, enum_def) in &self.enums {
                            if let Some(idx) = enum_def.variants.iter().position(|v| v.name == *variant) {
                                found = Some(idx as i64);
                                break;
                            }
                        }
                        found.unwrap_or(0)
                    } else if let Some(enum_def) = self.enums.get(enum_name) {
                        enum_def.variants.iter().position(|v| v.name == *variant).unwrap_or(0) as i64
                    } else { 0 };
                    let tag_ptr = self.emit_inst(Inst::GetFieldPtr { struct_ptr: match_val, field_index: 0, field_ty: IrType::I64 });
                    let tag = self.emit_inst(Inst::Load { ptr: tag_ptr, ty: IrType::I64 });
                    let expected = self.emit_inst(Inst::ConstInt { value: variant_idx, ty: IrType::I64 });
                    let cmp = self.emit_inst(Inst::BinOp { op: BinOp::Eq, left: tag, right: expected, ty: IrType::Bool });
                    self.emit_terminator(Inst::Branch { cond: cmp, then: match_block, else_: next_arm_block });
                    self.set_current_block(match_block);
                    // Save variable bindings so arm-local bindings don't leak
                    let saved_allocas = self.allocas.clone();
                    let saved_alloca_types = self.alloca_types.clone();
                    let saved_var_ast_types = self.var_ast_types.clone();
                    let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());
                    let binding_tys = self.pattern_binding_ir_types(enum_name, variant, bindings);
                    let binding_ast_tys = self.pattern_binding_ast_types(enum_name, variant, bindings);
                    let mut field_idx: u32 = 1;
                    for (i, (binding, _ty_name)) in bindings.iter().enumerate() {
                        let field_ty = binding_tys.get(i).cloned().unwrap_or(IrType::I64);
                        let field_ptr = self.emit_inst(Inst::GetFieldPtr { struct_ptr: match_val, field_index: field_idx, field_ty: field_ty.clone() });
                        let field_val = self.load_enum_field(field_ptr, &field_ty);
                        self.allocas.remove(binding);
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(binding, field_val);
                        }
                        // Register AST type so infer_expr_type can recover
                        // generic element types (e.g. Vec<FieldInit>).
                        if let Some(ast_ty) = binding_ast_tys.get(i) {
                            self.var_ast_types.insert(binding.clone(), ast_ty.clone());
                        }
                        field_idx += self.enum_field_slots(&field_ty);
                    }
                    // If there's a guard, evaluate it and branch
                    if let Some(guard_expr) = &arm.guard {
                        let guard_val = self.lower_expr(guard_expr);
                        let guard_body_block = self.new_block();
                        self.emit_terminator(Inst::Branch {
                            cond: guard_val,
                            then: guard_body_block,
                            else_: next_arm_block,
                        });
                        self.set_current_block(guard_body_block);
                    }
                    // Lower body and store result
                    let mut last_val = None;
                    for stmt in &arm.body {
                        if let Stmt::Expr(e, _) = stmt {
                            last_val = Some(self.lower_expr(e));
                        } else {
                            last_val = None;
                            self.lower_stmt(stmt);
                        }
                    }
                    if let Some(v) = last_val {
                        self.emit_inst(Inst::Store { ptr: result_slot, value: v, store_ty: None });
                    }
                    if !self.current_block_has_terminator() {
                        self.emit_terminator(Inst::Jump { target: merge_block });
                    }
                    // Restore variable bindings
                    self.allocas = saved_allocas;
                    self.alloca_types = saved_alloca_types;
                    self.var_ast_types = saved_var_ast_types;
                    if let Some(ref mut f) = self.current_func {
                        f.locals = saved_locals.unwrap_or_default();
                    }
                    self.set_current_block(next_arm_block);
                    continue;
                }
            }

            // Lower match arm body and store result
            self.set_current_block(match_block);
            // Save variable bindings so arm-local bindings don't leak
            let saved_allocas = self.allocas.clone();
            let saved_alloca_types = self.alloca_types.clone();
            let saved_var_ast_types = self.var_ast_types.clone();
            let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());

            // If there's a guard, evaluate it and branch
            if let Some(guard_expr) = &arm.guard {
                let guard_val = self.lower_expr(guard_expr);
                let guard_body_block = self.new_block();
                self.emit_terminator(Inst::Branch {
                    cond: guard_val,
                    then: guard_body_block,
                    else_: next_arm_block,
                });
                self.set_current_block(guard_body_block);
            }

            let mut last_val = None;
            for stmt in &arm.body {
                if let Stmt::Expr(e, _) = stmt {
                    last_val = Some(self.lower_expr(e));
                } else {
                    last_val = None;
                    self.lower_stmt(stmt);
                }
            }
            if let Some(v) = last_val {
                self.emit_inst(Inst::Store { ptr: result_slot, value: v, store_ty: None });
            }
            if !self.current_block_has_terminator() {
                self.emit_terminator(Inst::Jump { target: merge_block });
            }
            // Restore variable bindings
            self.allocas = saved_allocas;
            self.alloca_types = saved_alloca_types;
            self.var_ast_types = saved_var_ast_types;
            if let Some(ref mut f) = self.current_func {
                f.locals = saved_locals.unwrap_or_default();
            }

            self.set_current_block(next_arm_block);
        }

        self.emit_terminator(Inst::Jump { target: merge_block });
        self.set_current_block(merge_block);
    }

    /// Infer the result type of a match expression from its arms
    fn infer_match_type(&self, arms: &[MatchArm]) -> IrType {
        for arm in arms {
            for stmt in &arm.body {
                if let Stmt::Expr(e, _) = stmt {
                    return self.infer_expr_type(e);
                }
            }
        }
        IrType::I64
    }

    /// L3A: Lower `do Effect.op(args)` with continuation capture.
    /// The remaining statements in the block become a continuation closure.
    /// The handler receives the args + a `cont` closure that it can `resume`.
    fn lower_do_with_continuation(
        &mut self,
        effect: &str,
        op: &str,
        args: &[Expr],
        _line: usize,
        remaining: &[Stmt],
    ) {
        // Look up the handler for this effect.op
        let handler_info = self.effect_handler_stack.iter().rev().find_map(|handlers| {
            handlers.iter().find(|h| h.effect == effect && h.op == op)
                .map(|h| (h.params.clone(), h.body.clone()))
        });

        if let Some((params, body)) = handler_info {
            // Lower the args
            let arg_vals: Vec<ValueId> = args.iter()
                .map(|a| self.lower_expr(a))
                .collect();

            // Create a continuation closure from the remaining statements.
            // The closure takes one parameter (the resume value) and executes
            // the remaining statements.
            let has_continuation = !remaining.is_empty();
            let cont_val = if has_continuation {
                self.create_continuation_closure(remaining)
            } else {
                // No remaining statements — create a no-op continuation
                self.create_noop_continuation()
            };

            // Bind handler params: args first, then `cont` as last param
            for (i, arg_val) in arg_vals.iter().enumerate() {
                if i < params.len() {
                    let slot = self.emit_inst(Inst::Alloca {
                        ty: IrType::I64,
                        region: self.current_region(),
                    });
                    self.emit_inst(Inst::Store { ptr: slot, value: *arg_val, store_ty: None });
                    self.allocas.insert(params[i].clone(), slot);
                    self.alloca_types.insert(slot, IrType::I64);
                }
            }
            // Bind `cont` variable in the handler's scope
            // The continuation is a closure env struct — use Opaque type so the
            // interpreter stores it in the side-table (preserves Value::Ptr)
            let cont_slot = self.emit_inst(Inst::Alloca {
                ty: IrType::Opaque("closure".to_string()),
                region: self.current_region(),
            });
            self.emit_inst(Inst::Store { ptr: cont_slot, value: cont_val, store_ty: None });
            self.allocas.insert("cont".to_string(), cont_slot);
            self.alloca_types.insert(cont_slot, IrType::Opaque("closure".to_string()));

            // Lower the handler body
            self.lower_stmts(&body);
        } else {
            // No handler found — just lower the args (no-op)
            for arg in args {
                self.lower_expr(arg);
            }
            // Also lower remaining statements (no handler = passthrough)
            for s in remaining {
                self.lower_stmt(s);
            }
        }
    }

    /// L3A: Inlined `do` (fallback for nested do inside if/for/etc.)
    fn lower_do_inlined(&mut self, effect: &str, op: &str, args: &[Expr]) {
        let handler_info = self.effect_handler_stack.iter().rev().find_map(|handlers| {
            handlers.iter().find(|h| h.effect == effect && h.op == op)
                .map(|h| (h.params.clone(), h.body.clone()))
        });

        if let Some((params, body)) = handler_info {
            for (i, arg) in args.iter().enumerate() {
                if i < params.len() {
                    let arg_val = self.lower_expr(arg);
                    let slot = self.emit_inst(Inst::Alloca {
                        ty: IrType::I64,
                        region: self.current_region(),
                    });
                    self.emit_inst(Inst::Store { ptr: slot, value: arg_val, store_ty: None });
                    self.allocas.insert(params[i].clone(), slot);
                    self.alloca_types.insert(slot, IrType::I64);
                }
            }
            self.lower_stmts(&body);
        } else {
            for arg in args {
                self.lower_expr(arg);
            }
        }
    }

    /// L3A: Create a continuation closure from remaining statements.
    /// The closure takes one i64 parameter (the resume value) and executes
    /// the remaining statements. Captures all in-scope variables.
    fn create_continuation_closure(&mut self, remaining: &[Stmt]) -> ValueId {
        self.continuation_counter += 1;
        let closure_id = self.continuation_counter;
        let closure_name = format!("__cont_{}", closure_id);

        // Find free variables (captures) in the remaining statements
        let param_names: Vec<String> = vec!["__resume_val".to_string()];
        let captures = self.find_captures(remaining, &param_names);

        // Save current captures and set new ones for this closure
        let saved_captures = std::mem::take(&mut self.current_captures);
        for (i, (cap_name, cap_ty)) in captures.iter().enumerate() {
            self.current_captures.push((cap_name.clone(), i + 1, cap_ty.clone()));
        }

        // Build the closure function
        let resume_param = crate::ast::Param {
            name: "__resume_val".to_string(),
            ty: Type::I64,
            is_pub: false,
            annotations: vec![],
        };

        let closure_func = crate::ast::Func {
            name: closure_name.clone(),
            type_params: vec![],
            type_param_bounds: vec![],
            const_params: vec![],
            params: vec![resume_param],
            ret_ty: Type::I64,
            body: remaining.to_vec(),
            is_pub: false,
            is_comptime: false,
            declared_effects: vec![],
            has_effect_annotation: false,
            is_interrupt: false,
            annotations: vec![],
            preconditions: vec![],
            postconditions: vec![],
            is_async: false,
            line: 0,
        };

        // Save current function context
        let saved_func = self.current_func.take();
        let saved_allocas = std::mem::take(&mut self.allocas);
        let saved_alloca_types = std::mem::take(&mut self.alloca_types);
        let saved_var_ast_types = std::mem::take(&mut self.var_ast_types);
        let saved_vec_allocas = std::mem::take(&mut self.vec_allocas);
        let saved_block = self.current_block;
        self.lower_func(&closure_func);
        self.current_func = saved_func;
        self.allocas = saved_allocas;
        self.alloca_types = saved_alloca_types;
        self.var_ast_types = saved_var_ast_types;
        self.vec_allocas = saved_vec_allocas;
        self.current_block = saved_block;
        self.current_captures = saved_captures;

        // Build env struct with captures at the creation site
        if captures.is_empty() {
            self.make_fn_env(&closure_name, &[])
        } else {
            let cap_vals: Vec<(String, ValueId)> = captures.iter()
                .map(|(name, _)| {
                    let val = self.lower_expr(&Expr::Ident(name.clone()));
                    (name.clone(), val)
                })
                .collect();
            self.make_fn_env(&closure_name, &cap_vals)
        }
    }

    /// L3A: Create a no-op continuation (for `do` with no remaining statements)
    fn create_noop_continuation(&mut self) -> ValueId {
        self.continuation_counter += 1;
        let closure_id = self.continuation_counter;
        let closure_name = format!("__cont_noop_{}", closure_id);

        let resume_param = crate::ast::Param {
            name: "__resume_val".to_string(),
            ty: Type::I64,
            is_pub: false,
            annotations: vec![],
        };

        let closure_func = crate::ast::Func {
            name: closure_name.clone(),
            type_params: vec![],
            type_param_bounds: vec![],
            const_params: vec![],
            params: vec![resume_param],
            ret_ty: Type::I64,
            body: vec![Stmt::Return(Some(Expr::IntLit(0)), 0)],
            is_pub: false,
            is_comptime: false,
            declared_effects: vec![],
            has_effect_annotation: false,
            is_interrupt: false,
            annotations: vec![],
            preconditions: vec![],
            postconditions: vec![],
            is_async: false,
            line: 0,
        };

        let saved_func = self.current_func.take();
        let saved_allocas = std::mem::take(&mut self.allocas);
        let saved_alloca_types = std::mem::take(&mut self.alloca_types);
        let saved_var_ast_types = std::mem::take(&mut self.var_ast_types);
        let saved_vec_allocas = std::mem::take(&mut self.vec_allocas);
        let saved_block = self.current_block;
        self.lower_func(&closure_func);
        self.current_func = saved_func;
        self.allocas = saved_allocas;
        self.alloca_types = saved_alloca_types;
        self.var_ast_types = saved_var_ast_types;
        self.vec_allocas = saved_vec_allocas;
        self.current_block = saved_block;

        self.make_fn_env(&closure_name, &[])
    }

    /// Lower an expression to an SSA value
    fn lower_expr(&mut self, expr: &Expr) -> ValueId {
        match expr {
            Expr::IntLit(n) => {
                self.emit_inst(Inst::ConstInt { value: *n, ty: IrType::I64 })
            }

            Expr::FloatLit(f) => {
                let bits = f.to_bits();
                self.emit_inst(Inst::ConstFloat { bits, ty: IrType::F64 })
            }

            Expr::BoolLit(b) => {
                self.emit_inst(Inst::ConstBool { value: *b })
            }

            Expr::StringLit(s) => {
                // Intern the string and return a pointer to it
                if let Some(ref mut _f) = self.current_func {
                    let _label = self.module.intern_string(s);
                }
                self.emit_inst(Inst::ConstStr { value: s.clone() })
            }

            Expr::Ident(name) => {
                // Check if this is a captured variable in a closure
                if let Some((_, idx, cap_ty)) = self.current_captures.iter().find(|(n, _, _)| n == name) {
                    let idx = *idx;
                    let cap_ir_ty = self.lower_type(&cap_ty);
                    // Load from env struct: env_ptr is the first param of the closure function
                    if let Some(ref func) = self.current_func {
                        if let Some(env_val) = func.params.first().map(|p| p.0) {
                            // Get element pointer: env_ptr + idx * 8
                            let idx_val = self.emit_inst(Inst::ConstInt { value: idx as i64, ty: IrType::I64 });
                            let elem_ptr = self.emit_inst(Inst::GetElemPtr {
                                array_ptr: env_val,
                                index: idx_val,
                                elem_ty: IrType::I64,
                            });
                            return self.emit_inst(Inst::Load { ptr: elem_ptr, ty: cap_ir_ty });
                        }
                    }
                }
                // Check if this is a mutable variable with an alloca
                if let Some(alloca) = self.allocas.get(name) {
                    let load_ty = self.alloca_types.get(alloca).cloned().unwrap_or(IrType::I64);
                    // If the alloca holds a struct pointer, return the pointer directly
                    if matches!(&load_ty, IrType::Ptr(inner) if matches!(inner.as_ref(), IrType::Struct(_))) {
                        return self.emit_inst(Inst::Load { ptr: *alloca, ty: load_ty });
                    }
                    // If the alloca holds an inline struct, return the
                    // alloca pointer directly — the alloca IS the struct
                    // data, so the pointer to it is the struct value.
                    if matches!(&load_ty, IrType::Struct(_)) {
                        return *alloca;
                    }
                    return self.emit_inst(Inst::Load {
                        ptr: *alloca,
                        ty: load_ty,
                    });
                }
                // Check if it's a local variable
                if let Some(val) = self.current_func.as_ref().and_then(|f| f.lookup_local(name)) {
                    return val;
                }
                // Check if it's a constant (pc)
                if let Some(const_expr) = self.constants.get(name).cloned() {
                    return self.lower_expr(&const_expr);
                }
                // Check if it's a module-level mutable global
                if let Some((ir_ty, _init)) = self.module.globals.get(name).cloned() {
                    let addr = self.emit_inst(Inst::GlobalAddr { name: name.clone(), ty: ir_ty.clone() });
                    // For struct types, GlobalAddr already gives a pointer to the struct.
                    // Return it directly without a Load (the C codegen emits struct globals
                    // as values, not pointers).
                    if matches!(ir_ty, IrType::Struct(_)) {
                        return addr;
                    }
                    return self.emit_inst(Inst::Load { ptr: addr, ty: ir_ty });
                }
                // Check if it's a known function name — emit as env struct with fn addr
                if self.module.functions.contains_key(name) || self.module.extern_functions.contains_key(name) {
                    return self.make_fn_env(name, &[]);
                }
                // Unknown identifier — emit zero as fallback
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }

            Expr::Binary { op: op @ (BinOp::And | BinOp::Or), left, right } => {
                // Short-circuit: the right operand runs only when it decides
                // the result (`i < n && v[i] == x` must not read v[n]).
                let slot = self.emit_inst(Inst::Alloca { ty: IrType::Bool, region: self.current_region() });
                let left_val = self.lower_expr(left);
                self.emit_inst(Inst::Store { ptr: slot, value: left_val, store_ty: None });
                let rhs_bb = self.new_block();
                let merge_bb = self.new_block();
                let (then, else_) = if matches!(op, BinOp::And) { (rhs_bb, merge_bb) } else { (merge_bb, rhs_bb) };
                self.emit_terminator(Inst::Branch { cond: left_val, then, else_ });
                self.set_current_block(rhs_bb);
                let right_val = self.lower_expr(right);
                self.emit_inst(Inst::Store { ptr: slot, value: right_val, store_ty: None });
                self.emit_terminator(Inst::Jump { target: merge_bb });
                self.set_current_block(merge_bb);
                self.emit_inst(Inst::Load { ptr: slot, ty: IrType::Bool })
            }
            Expr::Binary { op, left, right } => {
                // Operator overloading for vec types (Phase 8A)
                // Check if either operand is a vec type
                let left_ir_ty = self.infer_expr_type(left);
                let right_ir_ty = self.infer_expr_type(right);
                let left_vec_name = get_vec_type_name(&left_ir_ty);
                let right_vec_name = get_vec_type_name(&right_ir_ty);

                if let Some(vn) = &left_vec_name {
                    if right_vec_name.as_deref() == Some(vn.as_str()) {
                        // vec + vec, vec - vec → call vecN_add / vecN_sub
                        let method = match op {
                            BinOp::Add => "add",
                            BinOp::Sub => "sub",
                            _ => "",
                        };
                        if !method.is_empty() {
                            return self.lower_vec_method_call(vn, method, left, right);
                        }
                    } else if matches!(op, BinOp::Mul) {
                        // vec * scalar → call vecN_scale
                        return self.lower_vec_method_call(vn, "scale", left, right);
                    } else if matches!(op, BinOp::Div) {
                        // vec / scalar = vec * (1/scalar)
                        let left_val = self.lower_expr(left);
                        let right_val = self.lower_expr(right);
                        let one = self.emit_inst(Inst::ConstFloat { bits: 1.0_f64.to_bits(), ty: IrType::F64 });
                        let inv = self.emit_inst(Inst::BinOp { op: BinOp::Div, left: one, right: right_val, ty: IrType::F64 });
                        let mangled = format!("{}_scale", vn);
                        let ret_ty = IrType::Struct(vn.clone());
                        return self.lower_vec_sret_call(&mangled, &ret_ty, left_val, inv);
                    }
                } else if let Some(vn) = &right_vec_name {
                    if matches!(op, BinOp::Mul) {
                        // scalar * vec → call vecN_scale(vec, scalar)
                        return self.lower_vec_method_call(vn, "scale", right, left);
                    }
                }

                let mut left_val = self.lower_expr(left);
                let mut right_val = self.lower_expr(right);
                let mut result_ty = self.infer_binop_type(op, left, right);

                // An integer operand next to a float one is converted to that
                // float type (C's usual arithmetic conversion): `f < 0`, `2 * f`.
                let numeric = matches!(op,
                    BinOp::Add | BinOp::Sub | BinOp::Mul | BinOp::Div | BinOp::Mod
                    | BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq);
                let plain_int = |t: &IrType| t.is_int() && *t != IrType::Bool;
                if numeric && left_ir_ty.is_float() && plain_int(&right_ir_ty) {
                    right_val = self.emit_inst(Inst::IntToFloat { value: right_val, from: right_ir_ty.clone(), to: left_ir_ty.clone() });
                    if !matches!(result_ty, IrType::Bool) { result_ty = left_ir_ty.clone(); }
                } else if numeric && plain_int(&left_ir_ty) && right_ir_ty.is_float() {
                    left_val = self.emit_inst(Inst::IntToFloat { value: left_val, from: left_ir_ty.clone(), to: right_ir_ty.clone() });
                    if !matches!(result_ty, IrType::Bool) { result_ty = right_ir_ty.clone(); }
                }

                // String concatenation: when + is used with two str operands,
                // emit a call to p_str_concat instead of integer addition.
                // Only when BOTH are strings — str + i64 is pointer arithmetic.
                let str_ty = IrType::Ptr(Box::new(IrType::U8));
                if matches!(op, BinOp::Add)
                    && left_ir_ty == str_ty && right_ir_ty == str_ty
                {
                    return self.emit_inst(Inst::Call {
                        func: "p_str_concat".to_string(),
                        args: vec![left_val, right_val],
                        ret_ty: str_ty,
                        effects: {
                            let mut e = EffectSet::new();
                            e.add(Effect::Alloc);
                            e.add(Effect::IO);
                            e
                        },
                    });
                }

                // String equality: when == or != has a str operand, emit a call
                // to p_str_equal for content comparison. R1.1: either side
                // suffices — the typechecker already forces both operands to the
                // same type, and the other side may have an erased (i64) IR type
                // (Vec<str>.get(), enum payloads). p_str_equal checks pointer
                // identity and null first, so `s == 0`-style checks still hold.
                // Without this, native codegen does pointer comparison (wrong).
                if matches!(op, BinOp::Eq | BinOp::Neq)
                    && (left_ir_ty == str_ty || right_ir_ty == str_ty)
                {
                    let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                    let eq_val = self.emit_inst(Inst::Call {
                        func: "p_str_equal".to_string(),
                        args: vec![dummy_env, left_val, right_val],
                        ret_ty: IrType::Bool,
                        effects: EffectSet::new(),
                    });
                    if matches!(op, BinOp::Neq) {
                        return self.emit_inst(Inst::UnaryOp {
                            op: UnaryOp::Not,
                            operand: eq_val,
                            ty: IrType::Bool,
                        });
                    }
                    return eq_val;
                }

                // R1.2: `==` / `!=` on enum values call the synthesized
                // `__eq_<Enum>` (tag + payload) instead of comparing pointers.
                if matches!(op, BinOp::Eq | BinOp::Neq) {
                    let enum_of = |t: &IrType| -> Option<String> {
                        match t {
                            IrType::Struct(n) => Some(n.clone()),
                            IrType::Ptr(inner) => match inner.as_ref() {
                                IrType::Struct(n) => Some(n.clone()),
                                _ => None,
                            },
                            _ => None,
                        }
                    };
                    let eq_func = enum_of(&left_ir_ty)
                        .or_else(|| enum_of(&right_ir_ty))
                        .filter(|n| self.enums.contains_key(n))
                        .map(|n| format!("__eq_{}", n))
                        .filter(|f| self.module.functions.contains_key(f));
                    if let Some(func) = eq_func {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        let eq_val = self.emit_inst(Inst::Call {
                            func,
                            args: vec![dummy_env, left_val, right_val],
                            ret_ty: IrType::Bool,
                            effects: EffectSet::new(),
                        });
                        if matches!(op, BinOp::Neq) {
                            return self.emit_inst(Inst::UnaryOp {
                                op: UnaryOp::Not,
                                operand: eq_val,
                                ty: IrType::Bool,
                            });
                        }
                        return eq_val;
                    }
                }

                self.emit_inst(Inst::BinOp {
                    op: op.clone(),
                    left: left_val,
                    right: right_val,
                    ty: result_ty,
                })
            }

            Expr::Unary { op, expr } => {
                let operand = self.lower_expr(expr);
                let result_ty = self.infer_unary_type(op, expr);
                self.emit_inst(Inst::UnaryOp {
                    op: op.clone(),
                    operand,
                    ty: result_ty,
                })
            }

            Expr::Call { callee, args } => {
                let arg_vals: Vec<ValueId> = args.iter()
                    .map(|a| self.lower_expr(a))
                    .collect();

                // Check if callee is a local variable (function-typed param)
                let is_local = self.current_func.as_ref().and_then(|f| f.lookup_local(callee)).is_some()
                    || self.allocas.contains_key(callee);

                if is_local {
                    // Indirect call through function pointer variable
                    let fn_val = if let Some(alloca) = self.allocas.get(callee) {
                        let load_ty = self.alloca_types.get(alloca).cloned().unwrap_or(IrType::I64);
                        self.emit_inst(Inst::Load { ptr: *alloca, ty: load_ty })
                    } else if let Some(val) = self.current_func.as_ref().and_then(|f| f.lookup_local(callee)) {
                        val
                    } else {
                        self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                    };
                    let ret_ty = self.current_func_param_types.get(callee)
                        .and_then(|ty| {
                            if let crate::ast::Type::Func { ret, .. } = ty {
                                Some(self.lower_type(ret))
                            } else {
                                None
                            }
                        })
                        .unwrap_or(IrType::I64);
                    let mut effects = EffectSet::new();
                    effects.add(Effect::IO); // Conservative
                    // All function references are env structs, and all functions have __env param
                    return self.emit_inst(Inst::CallPtr {
                        fn_ptr: fn_val,
                        args: arg_vals,
                        ret_ty,
                        effects,
                        has_env: true,
                    });
                }

                // GPU kernel builtins: thread_id(dim), workgroup_size(dim), etc.
                // On CPU, these return 0 (single-threaded fallback).
                // On GPU, they map to SPIR-V/PTX builtins.
                if callee == "thread_id" || callee == "workgroup_size" || callee == "workgroup_id" || callee == "num_workgroups" {
                    // Return 0 as a placeholder for CPU execution
                    return self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                }

                // Special case: print() builtin — emit as Inst::Print (not Inst::Call)
                // so the codegen can route to p_print_int/p_print_str/etc.
                if callee == "print" && !arg_vals.is_empty() {
                    let arg_ty = self.get_value_type(arg_vals[0]);
                    return self.emit_inst(Inst::Print {
                        value: arg_vals[0],
                        ty: arg_ty,
                    });
                }

                // Special case: println() builtin — emit as Print + Print("\n")
                // This avoids calling the stdlib println (which always uses p_print_str)
                // and instead routes to the correct p_print_* based on the argument type.
                if callee == "println" && !arg_vals.is_empty() {
                    let arg_ty = self.get_value_type(arg_vals[0]);
                    self.emit_inst(Inst::Print {
                        value: arg_vals[0],
                        ty: arg_ty,
                    });
                    let newline = self.emit_inst(Inst::ConstStr { value: "\n".to_string() });
                    self.emit_inst(Inst::Print {
                        value: newline,
                        ty: IrType::Ptr(Box::new(IrType::U8)),
                    });
                    return ValueId(0); // void
                }

                // Determine return type — look up function or default to I64
                let ret_ty = self.lookup_func_ret_ty(callee);

                // If the function returns a struct, use SRET calling convention:
                // 1. Alloca space for the struct in the caller's frame
                // 2. Pass the alloca pointer as a hidden first arg
                // 3. The call result is the alloca pointer (not rax)
                if let IrType::Struct(ref name) = ret_ty {
                    let sret_ptr = self.emit_inst(Inst::Alloca {
                        ty: IrType::Struct(name.clone()),
                        region: self.current_region(),
                    });
                    let mut all_args = vec![sret_ptr];
                    // Prepend dummy env_ptr for user functions (not externs, builtins, or runtime funcs)
                    let is_user_func = self.module.functions.contains_key(callee);
                    let is_runtime = !is_user_func && (callee.starts_with("p_") || callee.starts_with("__ecs_"));
                    if !self.module.extern_functions.contains_key(callee) && callee != "print" && !is_runtime {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        all_args.push(dummy_env);
                    }
                    all_args.extend(arg_vals);

                    // Determine effects
                    let mut effects = EffectSet::new();
                    if callee.starts_with("__pledge") {
                        effects.add(Effect::IO);
                    } else if let Some(e) = runtime_effects(callee) {
                        effects.add_all(&e);
                    } else if let Some(e) = self.module.function_effects.get(callee) {
                        effects.add_all(e);
                    }

                    self.emit_inst(Inst::Call {
                        func: callee.clone(),
                        args: all_args,
                        ret_ty: IrType::Void,
                        effects,
                    });
                    // The result is the sret pointer, not the call's return value
                    sret_ptr
                } else {
                    // Determine effects
                    let mut effects = EffectSet::new();
                    if callee.starts_with("p_print") {
                        effects.add(Effect::IO);
                    } else if callee.starts_with("__pledge") {
                        effects.add(Effect::IO);
                    } else if let Some(e) = runtime_effects(callee) {
                        effects.add_all(&e);
                    } else {
                        // User function — look up effects
                        if let Some(e) = self.module.function_effects.get(callee) {
                            effects.add_all(e);
                        }
                    }

                    // Prepend dummy env_ptr for user functions (not externs, builtins, or runtime funcs)
                    let mut all_args = arg_vals;
                    let is_user_func = self.module.functions.contains_key(callee);
                    let is_runtime = !is_user_func && (callee.starts_with("p_") || callee.starts_with("__ecs_"));
                    if !self.module.extern_functions.contains_key(callee) && callee != "print" && !is_runtime {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        all_args.insert(0, dummy_env);
                    }

                    let call_result = self.emit_inst(Inst::Call {
                        func: callee.clone(),
                        args: all_args,
                        ret_ty,
                        effects,
                    });

                    // Phase A2: If calling an async function, spawn it as a
                    // concurrent task via p_thread_spawn. The async function
                    // body runs in a real OS thread, and the returned Future
                    // handle can be polled/awaited.
                    // For functions with no args, we still use p_future_ready
                    // (eager) as a fast path for trivial async functions.
                    if self.async_funcs.contains(callee) {
                        // Build p_thread_spawn(func_name, num_args, args...)
                        // But we already emitted the call above — we need to
                        // NOT emit the eager call and instead spawn a thread.
                        // Since we can't undo the emitted call, we use a different
                        // approach: wrap the already-computed result in p_future_ready.
                        // The real concurrent spawn happens via `spawn <expr>`.
                        // For direct async calls, we keep the eager model but
                        // wrap in a Future. A2's improvement is that `await`
                        // now properly drives the executor for spawned tasks.
                        self.emit_inst(Inst::Call {
                            func: "p_future_ready".to_string(),
                            args: vec![call_result],
                            ret_ty: IrType::I64,
                            effects: EffectSet::new(),
                        })
                    } else {
                        call_result
                    }
                }
            }

            Expr::MethodCall { receiver, method, args } => {
                // Check if this is Box::new(x) — built-in heap allocation
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Box" && method == "new" {
                        // Box::new(x) = malloc(sizeof(x)), store x, return ptr
                        let val = self.lower_expr(&args[0]);
                        let val_ty = self.get_value_type(val);
                        // For struct/enum types, the value is a pointer to an
                        // alloca (enums are passed by pointer). We need to:
                        // 1. Allocate the full struct size (not just 8 bytes)
                        // 2. Use Store with store_ty=Struct to emit memcpy
                        // This prevents dangling pointers when the creating
                        // function returns and the alloca is freed.
                        let (alloc_size, store_ty) = match &val_ty {
                            IrType::Ptr(inner) if inner.is_aggregate() => {
                                // val is a pointer to a struct/enum alloca
                                let struct_ty = inner.as_ref().clone();
                                let size = self.module.type_size(&struct_ty);
                                (size, Some(struct_ty))
                            }
                            _ => {
                                let size = self.module.type_size(&val_ty);
                                (size, None)
                            }
                        };
                        // Ensure at least 1 byte to avoid malloc(0)
                        let alloc_size = if alloc_size == 0 { 8 } else { alloc_size };
                        let size_val = self.emit_inst(Inst::ConstInt { value: alloc_size as i64, ty: IrType::I64 });
                        let typed_ptr_ty = IrType::Ptr(Box::new(val_ty.clone()));
                        let ptr = self.emit_inst(Inst::Call {
                            func: "p_malloc".to_string(),
                            args: vec![size_val],
                            ret_ty: typed_ptr_ty.clone(),
                            effects: {
                                let mut e = EffectSet::new();
                                e.add(Effect::Alloc);
                                e
                            },
                        });
                        // Store the value at the heap address.
                        // For struct types, store_ty=Some(Struct(_)) causes
                        // the C codegen to emit memcpy, copying the full
                        // struct data from the alloca to the heap.
                        self.emit_inst(Inst::Store { ptr, value: val, store_ty });
                        return ptr;
                    }
                    // Rc::new(x) — allocate [refcount:i64 | value:T], return ptr to value
                    if name == "Rc" && method == "new" {
                        let val = self.lower_expr(&args[0]);
                        let val_ty = self.get_value_type(val);
                        let val_size = self.module.type_size(&val_ty) as i64;
                        // Total = refcount (8 bytes) + value
                        let total = val_size + 8;
                        let size_val = self.emit_inst(Inst::ConstInt { value: total, ty: IrType::I64 });
                        let raw_ptr = self.emit_inst(Inst::Call {
                            func: "p_malloc".to_string(),
                            args: vec![size_val],
                            ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                            effects: {
                                let mut e = EffectSet::new();
                                e.add(Effect::Alloc);
                                e
                            },
                        });
                        // Store refcount = 1 at offset 0
                        let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
                        self.emit_inst(Inst::Store { ptr: raw_ptr, value: one, store_ty: None });
                        // Value pointer = raw_ptr + 8
                        let eight = self.emit_inst(Inst::ConstInt { value: 8, ty: IrType::I64 });
                        let rc_ty = IrType::Opaque("Rc".to_string());
                        let val_ptr = self.emit_inst(Inst::BinOp { op: BinOp::Add, left: raw_ptr, right: eight, ty: rc_ty.clone() });
                        // Store the value at val_ptr
                        let typed_ptr = self.emit_inst(Inst::BitCast { value: val_ptr, from: rc_ty.clone(), to: IrType::Ptr(Box::new(val_ty.clone())) });
                        self.emit_inst(Inst::Store { ptr: typed_ptr, value: val, store_ty: None });
                        // Return the value pointer (user never sees the refcount header)
                        return val_ptr;
                    }
                    // Arc::new(x) — same layout as Rc, but uses atomic increment/decrement
                    if name == "Arc" && method == "new" {
                        let val = self.lower_expr(&args[0]);
                        let val_ty = self.get_value_type(val);
                        let val_size = self.module.type_size(&val_ty) as i64;
                        let total = val_size + 8;
                        let size_val = self.emit_inst(Inst::ConstInt { value: total, ty: IrType::I64 });
                        let raw_ptr = self.emit_inst(Inst::Call {
                            func: "p_malloc".to_string(),
                            args: vec![size_val],
                            ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                            effects: {
                                let mut e = EffectSet::new();
                                e.add(Effect::Alloc);
                                e
                            },
                        });
                        let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
                        self.emit_inst(Inst::Store { ptr: raw_ptr, value: one, store_ty: None });
                        let eight = self.emit_inst(Inst::ConstInt { value: 8, ty: IrType::I64 });
                        let arc_ty = IrType::Opaque("Arc".to_string());
                        let val_ptr = self.emit_inst(Inst::BinOp { op: BinOp::Add, left: raw_ptr, right: eight, ty: arc_ty.clone() });
                        let typed_ptr = self.emit_inst(Inst::BitCast { value: val_ptr, from: arc_ty.clone(), to: IrType::Ptr(Box::new(val_ty.clone())) });
                        self.emit_inst(Inst::Store { ptr: typed_ptr, value: val, store_ty: None });
                        return val_ptr;
                    }
                }
                // Check if this is Vec::new() — Phase 10B: now calls Fides implementation
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Vec" && method == "new" {
                        // Infer element type from the typechecker's result
                        let elem_ty = self.infer_vec_elem_type();
                        // Vec_new is a user function — prepend dummy env_ptr
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        return self.emit_inst(Inst::Call {
                            func: "Vec_new".to_string(),
                            args: vec![dummy_env],
                            ret_ty: IrType::Generic("Vec".to_string(), vec![elem_ty]),
                            effects: EffectSet::new(),
                        });
                    }
                }
                // Check if this is HashMap::new() — Phase 10B: calls Fides implementation
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "HashMap" && method == "new" {
                        let (key_ty, val_ty) = self.infer_map_types();
                        // HashMap_new is a user function — prepend dummy env_ptr
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        return self.emit_inst(Inst::Call {
                            func: "HashMap_new".to_string(),
                            args: vec![dummy_env],
                            ret_ty: IrType::Generic("HashMap".to_string(), vec![key_ty, val_ty]),
                            effects: EffectSet::new(),
                        });
                    }
                }
                // Check if this is String::from(x) — Phase 10B: calls Fides implementation
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "String" && method == "from" {
                        let arg = self.lower_expr(&args[0]);
                        // String_from is a user function — prepend dummy env_ptr
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        return self.emit_inst(Inst::Call {
                            func: "String_from".to_string(),
                            args: vec![dummy_env, arg],
                            ret_ty: IrType::Opaque("String".to_string()),
                            effects: EffectSet::new(),
                        });
                    }
                }
                // Check if this is an associated function call (e.g. Point::new(x, y))
                // where receiver is Expr::Ident("Point") and "Point" is a struct name
                if let Expr::Ident(name) = receiver.as_ref() {
                    if self.structs.contains_key(name) {
                        // Associated function call: mangle as StructName_method
                        let mangled_name = format!("{}_{}", name, method);
                        let mut all_args = Vec::new();
                        for a in args {
                            all_args.push(self.lower_expr(a));
                        }
                        // Look up return type
                        let ret_ty = if let Some(meth) = self.impl_methods.get(&(name.clone(), method.clone())) {
                            self.lower_type(&meth.ret_ty)
                        } else {
                            IrType::I64
                        };
                        // Handle struct return with SRET
                        if let IrType::Struct(ref sname) = ret_ty {
                            let sret_ptr = self.emit_inst(Inst::Alloca {
                                ty: IrType::Struct(sname.clone()),
                                region: self.current_region(),
                            });
                            let mut sret_args = vec![sret_ptr];
                            // Prepend dummy env_ptr for user functions
                            if !self.module.extern_functions.contains_key(&mangled_name) && mangled_name != "print" {
                                let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                                sret_args.push(dummy_env);
                            }
                            sret_args.extend(all_args);
                            let mut effects = EffectSet::new();
                            if let Some(e) = self.module.function_effects.get(&mangled_name) {
                                effects.add_all(e);
                            }
                            self.emit_inst(Inst::Call {
                                func: mangled_name,
                                args: sret_args,
                                ret_ty: IrType::Void,
                                effects,
                            });
                            return sret_ptr;
                        } else {
                            let mut effects = EffectSet::new();
                            if let Some(e) = self.module.function_effects.get(&mangled_name) {
                                effects.add_all(e);
                            }
                            // Prepend dummy env_ptr for user functions
                            if !self.module.extern_functions.contains_key(&mangled_name) && mangled_name != "print" {
                                let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                                all_args.insert(0, dummy_env);
                            }
                            return self.emit_inst(Inst::Call {
                                func: mangled_name,
                                args: all_args,
                                ret_ty,
                                effects,
                            });
                        }
                    }
                }
                // Regular method call: receiver is a value, pass as first arg
                let mut recv_val = self.lower_expr(receiver);

                // Rc<T> / Arc<T> method dispatch — refcount operations
                let recv_ty = self.get_value_type(recv_val);
                if recv_ty.is_opaque_named("Rc") || recv_ty.is_opaque_named("Arc") {
                    let is_arc = recv_ty.is_opaque_named("Arc");
                    let elem_ty = recv_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                    let elem_ptr = Self::word_repr(elem_ty.clone());
                    match method.as_str() {
                        "clone" => {
                            // Increment refcount at ptr-8, return same ptr
                            let eight = self.emit_inst(Inst::ConstInt { value: 8, ty: IrType::I64 });
                            let rc_addr = self.emit_inst(Inst::BinOp { op: BinOp::Sub, left: recv_val, right: eight, ty: IrType::I64 });
                            // Both Rc and Arc use p_rc_incr at the IR level.
                            // The C codegen implements p_rc_incr with atomic ops for Arc
                            // (detected via the function name convention).
                            let ret_ty = if is_arc { IrType::Opaque("Arc".to_string()) } else { IrType::Opaque("Rc".to_string()) };
                            return self.emit_inst(Inst::Call {
                                func: "p_rc_incr".to_string(),
                                args: vec![rc_addr, recv_val],
                                ret_ty,
                                effects: EffectSet::new(),
                            });
                        }
                        "drop" => {
                            // Decrement refcount at ptr-8, free if zero
                            let eight = self.emit_inst(Inst::ConstInt { value: 8, ty: IrType::I64 });
                            let rc_addr = self.emit_inst(Inst::BinOp { op: BinOp::Sub, left: recv_val, right: eight, ty: IrType::I64 });
                            return self.emit_inst(Inst::Call {
                                func: "p_rc_decr".to_string(),
                                args: vec![rc_addr, recv_val],
                                ret_ty: IrType::Void,
                                effects: EffectSet::new(),
                            });
                        }
                        "get" => {
                            // Load value from the pointer
                            let typed_ptr = self.emit_inst(Inst::BitCast { value: recv_val, from: IrType::I64, to: IrType::Ptr(Box::new(elem_ty.clone())) });
                            return self.emit_inst(Inst::Load { ptr: typed_ptr, ty: elem_ptr });
                        }
                        "count" => {
                            // Load refcount from ptr-8
                            let eight = self.emit_inst(Inst::ConstInt { value: 8, ty: IrType::I64 });
                            let rc_addr = self.emit_inst(Inst::BinOp { op: BinOp::Sub, left: recv_val, right: eight, ty: IrType::I64 });
                            let rc_ptr = self.emit_inst(Inst::BitCast { value: rc_addr, from: IrType::I64, to: IrType::Ptr(Box::new(IrType::I64)) });
                            return self.emit_inst(Inst::Load { ptr: rc_ptr, ty: IrType::I64 });
                        }
                        _ => {}
                    }
                }

                // Builtin collection method dispatch
                let recv_ty = self.get_value_type(recv_val);
                // Unwrap Ptr (from &mut / & references) for collection method dispatch
                let effective_ty = match &recv_ty {
                    IrType::Ptr(inner) => inner.as_ref().clone(),
                    _ => recv_ty.clone(),
                };
                let is_builtin_collection = effective_ty.is_opaque_named("Vec")
                    || effective_ty.is_opaque_named("HashMap")
                    || effective_ty.is_opaque_named("String");
                if is_builtin_collection {
                    // For &mut Vec, load the Vec pointer from the reference
                    let effective_val = if matches!(recv_ty, IrType::Ptr(_)) {
                        self.emit_inst(Inst::Load {
                            ptr: recv_val,
                            ty: effective_ty.clone(),
                        })
                    } else {
                        recv_val
                    };
                    let mut runtime_args = vec![effective_val];
                    for a in args {
                        runtime_args.push(self.lower_expr(a));
                    }
                    let (func_name, ret_ty) = if effective_ty.is_opaque_named("Vec") {
                        // Phase 10B: Vec methods now call Fides implementation (Vec_*)
                        // Prepend dummy env_ptr for user functions
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        runtime_args.insert(0, dummy_env);
                        // Element type from Generic args, default to I64.
                        // Use effective_ty (unwrapped from Ptr) for generic_elem.
                        let mut elem_ty = effective_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                        // Fallback: if recv_ty is Opaque("Vec") (from Vec::new()),
                        // generic_elem() returns None. Try looking up the AST type
                        // from var_ast_types to recover the element type.
                        if matches!(elem_ty, IrType::I64) && effective_ty.is_opaque_named("Vec") {
                            if let Expr::Ident(var_name) = receiver.as_ref() {
                                if let Some(ast_ty) = self.var_ast_types.get(var_name) {
                                    // Unwrap &mut / & references to get the inner type
                                    let inner_ty = match ast_ty {
                                        Type::Ref { inner, .. } => inner.as_ref(),
                                        other => other,
                                    };
                                    if let Type::Generic { name, args } = inner_ty {
                                        if name == "Vec" && !args.is_empty() {
                                            elem_ty = self.lower_type(&args[0]);
                                        }
                                    }
                                }
                            }
                        }
                        // Box struct elements before pushing/inserting into Vec.
                        // The Vec runtime stores elements as i64 (8-byte slots).
                        // For struct elements (which are >8 bytes), we must heap-allocate
                        // the struct and store the pointer in the Vec. Otherwise the
                        // stack-allocated SRET buffer would become a dangling pointer
                        // when the calling function returns.
                        if matches!(elem_ty, IrType::Struct(_)) {
                            match method.as_str() {
                                "push" => {
                                    // runtime_args = [dummy_env, vec, value]
                                    // Box the value at index 2
                                    let val = runtime_args[2];
                                    let mut size = self.module.type_size(&elem_ty);
                                    if size == 0 { size = 8; }  // fallback for unknown structs
                                    let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                                    let heap_ptr = self.emit_inst(Inst::Call {
                                        func: "p_malloc".to_string(),
                                        args: vec![size_val],
                                        ret_ty: IrType::Ptr(Box::new(elem_ty.clone())),
                                        effects: {
                                            let mut e = EffectSet::new();
                                            e.add(Effect::Alloc);
                                            e
                                        },
                                    });
                                    // Use store_ty: Some(elem_ty) so the codegen
                                    // emits memcpy for the full struct, not just
                                    // an 8-byte pointer store.
                                    self.emit_inst(Inst::Store { ptr: heap_ptr, value: val, store_ty: Some(elem_ty.clone()) });
                                    runtime_args[2] = heap_ptr;
                                }
                                "insert" => {
                                    // runtime_args = [dummy_env, vec, index, value]
                                    // Box the value at index 3
                                    let val = runtime_args[3];
                                    let mut size = self.module.type_size(&elem_ty);
                                    if size == 0 { size = 8; }  // fallback for unknown structs
                                    let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                                    let heap_ptr = self.emit_inst(Inst::Call {
                                        func: "p_malloc".to_string(),
                                        args: vec![size_val],
                                        ret_ty: IrType::Ptr(Box::new(elem_ty.clone())),
                                        effects: {
                                            let mut e = EffectSet::new();
                                            e.add(Effect::Alloc);
                                            e
                                        },
                                    });
                                    self.emit_inst(Inst::Store { ptr: heap_ptr, value: val, store_ty: Some(elem_ty.clone()) });
                                    runtime_args[3] = heap_ptr;
                                }
                                "set" => {
                                    // runtime_args = [dummy_env, vec, index, value]
                                    // Box the value at index 3
                                    let val = runtime_args[3];
                                    let mut size = self.module.type_size(&elem_ty);
                                    if size == 0 { size = 8; }
                                    let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                                    let heap_ptr = self.emit_inst(Inst::Call {
                                        func: "p_malloc".to_string(),
                                        args: vec![size_val],
                                        ret_ty: IrType::Ptr(Box::new(elem_ty.clone())),
                                        effects: {
                                            let mut e = EffectSet::new();
                                            e.add(Effect::Alloc);
                                            e
                                        },
                                    });
                                    self.emit_inst(Inst::Store { ptr: heap_ptr, value: val, store_ty: Some(elem_ty.clone()) });
                                    runtime_args[3] = heap_ptr;
                                }
                                _ => {}
                            }
                        } else {
                            // Fallback: check the value's IR type directly.
                            // If it's Ptr(Struct(..)) (from SRET), box it.
                            // This handles cases where the receiver is a field
                            // access (self.vec.push(..)) and the AST type
                            // lookup fails to determine the element type.
                            let val_idx = if method == "insert" || method == "set" { 3 } else { 2 };
                            if val_idx < runtime_args.len() {
                                let val = runtime_args[val_idx];
                                let val_ty = self.get_value_type(val);
                                if let IrType::Ptr(inner) = &val_ty {
                                    if let IrType::Struct(_) = inner.as_ref() {
                                        let struct_ty = inner.as_ref().clone();
                                        let mut size = self.module.type_size(&struct_ty);
                                        if size == 0 { size = 8; }
                                        let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
                                        let heap_ptr = self.emit_inst(Inst::Call {
                                            func: "p_malloc".to_string(),
                                            args: vec![size_val],
                                            ret_ty: IrType::Ptr(Box::new(struct_ty.clone())),
                                            effects: {
                                                let mut e = EffectSet::new();
                                                e.add(Effect::Alloc);
                                                e
                                            },
                                        });
                                        self.emit_inst(Inst::Store { ptr: heap_ptr, value: val, store_ty: Some(struct_ty) });
                                        runtime_args[val_idx] = heap_ptr;
                                    }
                                }
                            }
                        }
                        // Vec_get/pop/first/last/remove return a pointer when
                        // the element is a struct (the runtime stores structs
                        // by reference). Wrap it so field access works.
                        let elem_ptr = Self::word_repr(elem_ty.clone());
                        let rt = match method.as_str() {
                            "push" => IrType::Void,
                            "pop" => elem_ptr.clone(),
                            "len" => IrType::I64,
                            "is_empty" => IrType::Bool,
                            "get" => elem_ptr.clone(),
                            "set" => IrType::Void,
                            "clear" => IrType::Void,
                            "capacity" => IrType::I64,
                            "insert" => IrType::Void,
                            "remove" => elem_ptr.clone(),
                            "contains" => IrType::Bool,
                            "reverse" => IrType::Void,
                            "first" => elem_ptr.clone(),
                            "last" => elem_ptr.clone(),
                            "extend" => IrType::Void,
                            "truncate" => IrType::Void,
                            "sort" => IrType::Void,
                            "clone" => recv_ty.clone(),
                            // Higher-order methods from stdlib/vec.fd
                            "map" => recv_ty.clone(),
                            "filter" => recv_ty.clone(),
                            "for_each" => IrType::Void,
                            "fold" => IrType::I64,
                            "any" => IrType::Bool,
                            "all" => IrType::Bool,
                            "find" => IrType::I64,
                            "position" => IrType::I64,
                            _ => {
                                // For user-defined Vec methods, look up return type
                                if let Some(meth) = self.impl_methods.get(&("Vec".to_string(), method.clone())) {
                                    self.lower_type(&meth.ret_ty)
                                } else {
                                    IrType::I64
                                }
                            }
                        };
                        (format!("Vec_{}", method), rt)
                    } else if effective_ty.is_opaque_named("HashMap") {
                        // Phase 10B: HashMap methods call Fides implementation (HashMap_*)
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        runtime_args.insert(0, dummy_env);
                        let rt = match method.as_str() {
                            "insert" => IrType::Void,
                            "get" => IrType::I64,
                            "contains" => IrType::Bool,
                            "remove" => IrType::Void,
                            "len" => IrType::I64,
                            "is_empty" => IrType::Bool,
                            "clear" => IrType::Void,
                            "keys" => IrType::Generic("Vec".to_string(), vec![IrType::Ptr(Box::new(IrType::U8))]),
                            "values" => IrType::Generic("Vec".to_string(), vec![IrType::I64]),
                            "get_or" => IrType::I64,
                            "clone" => recv_ty.clone(),
                            _ => IrType::I64,
                        };
                        (format!("HashMap_{}", method), rt)
                    } else if effective_ty.is_opaque_named("String") {
                        // Phase 10B: String methods call Fides implementation (String_*)
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        runtime_args.insert(0, dummy_env);
                        let rt = match method.as_str() {
                            "len" => IrType::I64,
                            "is_empty" => IrType::Bool,
                            "push_str" => IrType::Opaque("String".to_string()),
                            "to_string" => IrType::Opaque("String".to_string()),
                            "contains" => IrType::Bool,
                            "starts_with" => IrType::Bool,
                            "ends_with" => IrType::Bool,
                            "trim" => IrType::Opaque("String".to_string()),
                            "split" => IrType::Generic("Vec".to_string(), vec![IrType::Opaque("String".to_string())]),
                            "replace" => IrType::Opaque("String".to_string()),
                            "char_at" => IrType::Opaque("String".to_string()),
                            "to_uppercase" => IrType::Opaque("String".to_string()),
                            "to_lowercase" => IrType::Opaque("String".to_string()),
                            "parse_int" => IrType::I64,
                            "find" => IrType::I64,
                            "rfind" => IrType::I64,
                            "repeat" => IrType::Opaque("String".to_string()),
                            "bytes" => IrType::I64,
                            "clone" => IrType::Opaque("String".to_string()),
                            "substring" => IrType::Opaque("String".to_string()),
                            _ => IrType::I64,
                        };
                        let fname = if method == "substring" {
                            "String_slice".to_string()
                        } else {
                            format!("String_{}", method)
                        };
                        (fname, rt)
                    } else {
                        (method.clone(), IrType::I64)
                    };
                    return self.emit_inst(Inst::Call {
                        func: func_name,
                        args: runtime_args,
                        ret_ty,
                        effects: EffectSet::new(),
                    });
                }

                let mut all_args = vec![recv_val];
                for a in args {
                    all_args.push(self.lower_expr(a));
                }
                // Determine the struct type from receiver to mangle method name
                let recv_ty = self.get_value_type(recv_val);

                // Check if receiver is a trait object (dyn Trait) — use dynamic dispatch
                if let IrType::Opaque(ref name) = recv_ty {
                    if name.starts_with("dyn:") {
                        let trait_name = name[4..].to_string();
                        let ret_ty = match self.trait_method_rets.get(&(trait_name.clone(), method.clone())) {
                            Some(t) => self.lower_type(&t.clone()),
                            None => IrType::I64,
                        };
                        // Any implementation may run: assume every effect.
                        let mut effects = EffectSet::new();
                        for e in [Effect::Read, Effect::Write, Effect::IO, Effect::Alloc] {
                            effects.add(e);
                        }
                        return self.emit_inst(Inst::DynCall {
                            trait_name,
                            method: method.clone(),
                            args: all_args,
                            ret_ty,
                            effects,
                        });
                    }
                }

                // Issue 45: Auto-deref &mut T for method calls.
                // If recv_ty is Ptr(Ptr(...)), load through the reference first.
                let recv_ty = if let IrType::Ptr(ref inner) = recv_ty {
                    if let IrType::Ptr(_) = inner.as_ref() {
                        recv_val = self.emit_inst(Inst::Load { ptr: recv_val, ty: IrType::I64 });
                        self.get_value_type(recv_val)
                    } else {
                        recv_ty
                    }
                } else {
                    recv_ty
                };

                let struct_name = match &recv_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        // Auto-deref: Ptr(Ptr(Struct(name))) → &mut T where T is a struct
                        IrType::Ptr(inner2) => match inner2.as_ref() {
                            IrType::Struct(name) => Some(name.clone()),
                            _ => None,
                        },
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
                    IrType::Generic(name, _) => Some(name.clone()),
                    IrType::Opaque(name) => Some(name.clone()),
                    _ => None,
                };
                // Mangle as struct_name_method_name
                let mangled_name = if let Some(ref sn) = struct_name {
                    format!("{}_{}", sn, method)
                } else {
                    method.clone()
                };
                // Look up return type from the impl method
                let ret_ty = if let Some(ref sn) = struct_name {
                    if let Some(meth) = self.impl_methods.get(&(sn.clone(), method.clone())) {
                        self.lower_type(&meth.ret_ty)
                    } else {
                        IrType::I64
                    }
                } else {
                    IrType::I64
                };
                // Handle struct return with SRET
                if let IrType::Struct(ref name) = ret_ty {
                    let sret_ptr = self.emit_inst(Inst::Alloca {
                        ty: IrType::Struct(name.clone()),
                        region: self.current_region(),
                    });
                    let mut sret_args = vec![sret_ptr];
                    // Prepend dummy env_ptr for user functions
                    if !self.module.extern_functions.contains_key(&mangled_name) && mangled_name != "print" {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        sret_args.push(dummy_env);
                    }
                    sret_args.extend(all_args);
                    let mut effects = EffectSet::new();
                    if let Some(e) = self.module.function_effects.get(&mangled_name) {
                        effects.add_all(e);
                    }
                    self.emit_inst(Inst::Call {
                        func: mangled_name,
                        args: sret_args,
                        ret_ty: IrType::Void,
                        effects,
                    });
                    sret_ptr
                } else {
                    let mut effects = EffectSet::new();
                    if let Some(e) = self.module.function_effects.get(&mangled_name) {
                        effects.add_all(e);
                    }
                    // Prepend dummy env_ptr for user functions
                    if !self.module.extern_functions.contains_key(&mangled_name) && mangled_name != "print" {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        all_args.insert(0, dummy_env);
                    }
                    self.emit_inst(Inst::Call {
                        func: mangled_name,
                        args: all_args,
                        ret_ty,
                        effects,
                    })
                }
            }

            Expr::FieldAccess { expr, field } => {
                // The expression should evaluate to a pointer to the struct
                let struct_ptr = self.lower_expr(expr);

                // Look up the struct layout to find the field index and type
                // We need to determine the struct type from the value metadata
                let struct_ty = self.get_value_type(struct_ptr);
                let mut struct_name = match &struct_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        IrType::Opaque(name) => {
                            if self.module.get_struct(name).is_some() {
                                Some(name.clone())
                            } else {
                                None
                            }
                        }
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
                    IrType::Opaque(name) => {
                        if self.module.get_struct(name).is_some() {
                            Some(name.clone())
                        } else {
                            None
                        }
                    }
                    _ => None,
                };
                // Fallback: try infer_expr_type if get_value_type didn't find a struct
                // (needed when struct_ptr comes from Vec_get which returns I64)
                if struct_name.is_none() {
                    let inferred_ty = self.infer_expr_type(expr);
                    struct_name = match &inferred_ty {
                        IrType::Ptr(inner) => match inner.as_ref() {
                            IrType::Struct(name) => Some(name.clone()),
                            IrType::Opaque(name) => {
                                if self.module.get_struct(name).is_some() {
                                    Some(name.clone())
                                } else {
                                    None
                                }
                            }
                            _ => None,
                        },
                        IrType::Struct(name) => Some(name.clone()),
                        IrType::Opaque(name) => {
                            if self.module.get_struct(name).is_some() {
                                Some(name.clone())
                            } else {
                                None
                            }
                        }
                        _ => None,
                    };
                }

                // Swizzle access for vec types (Phase 8A)
                if let Some(ref sn) = struct_name {
                    if is_vec_type_name(sn) && field.len() > 1 {
                        // Check if this is a valid swizzle pattern
                        if is_valid_swizzle(field) {
                            return self.lower_swizzle(sn, struct_ptr, field);
                        }
                    }
                }
                if let Some(name) = struct_name {
                    if let Some(layout) = self.module.get_struct(&name).cloned() {
                        if let Some((idx, (_, field_ty, _))) = layout.fields.iter()
                            .enumerate()
                            .find(|(_, (n, _, _))| n == field)
                        {
                            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                struct_ptr,
                                field_index: idx as u32,
                                field_ty: field_ty.clone(),
                            });
                            // If the field is a struct, return the pointer (not a load)
                            // — structs can't be loaded as scalar values
                            if matches!(field_ty, IrType::Struct(_)) {
                                return field_ptr;
                            }
                            return self.emit_inst(Inst::Load {
                                ptr: field_ptr,
                                ty: field_ty.clone(),
                            });
                        }
                    }
                }

                // Fallback: return 0
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }

            Expr::OptionalFieldAccess { expr, field } => {
                // `expr?.field` — if expr is None, return 0; else unwrap and access field.
                // The expression is an Option (struct { tag: i64, value: i64 }).
                // If Some (tag 0), the value (field 1) is a pointer to the struct.
                // If None (tag 1), return 0.
                let option_ptr = self.lower_expr(expr);

                // Allocate result slot
                let result_slot = self.emit_inst(Inst::Alloca {
                    ty: IrType::I64,
                    region: self.current_region(),
                });

                // Load the tag (field 0)
                let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: option_ptr,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                let tag = self.emit_inst(Inst::Load {
                    ptr: tag_ptr,
                    ty: IrType::I64,
                });

                let some_block = self.new_block();
                let none_block = self.new_block();
                let merge_block = self.new_block();

                let zero_val = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                let is_some = self.emit_inst(Inst::BinOp {
                    op: BinOp::Eq,
                    left: tag,
                    right: zero_val,
                    ty: IrType::Bool,
                });

                self.emit_terminator(Inst::Branch {
                    cond: is_some,
                    then: some_block,
                    else_: none_block,
                });

                // Some: load the value (field 1 = struct pointer), then access field
                self.set_current_block(some_block);
                let val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: option_ptr,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                let struct_ptr = self.emit_inst(Inst::Load {
                    ptr: val_ptr,
                    ty: IrType::I64,
                });

                // Now access the field on the struct pointer
                // Try to find the struct layout — we need to infer the struct type
                // For now, use the struct name from the field access context
                let field_val = if let Some(ref mut f) = self.current_func {
                    // Try to find the struct type from the option's metadata
                    // This is a simplification — in a full implementation, we'd track types
                    // through the option. For now, search for a struct that has this field.
                    let mut found = None;
                    for (name, layout) in &self.module.struct_layouts {
                        if layout.fields.iter().any(|(n, _, _)| n == field) {
                            found = Some(name.clone());
                            break;
                        }
                    }
                    if let Some(struct_name) = found {
                        if let Some(layout) = self.module.get_struct(&struct_name).cloned() {
                            if let Some((idx, (_, field_ty, _))) = layout.fields.iter()
                                .enumerate()
                                .find(|(_, (n, _, _))| n == field)
                            {
                                let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                    struct_ptr,
                                    field_index: idx as u32,
                                    field_ty: field_ty.clone(),
                                });
                                if matches!(field_ty, IrType::Struct(_)) {
                                    field_ptr
                                } else {
                                    self.emit_inst(Inst::Load {
                                        ptr: field_ptr,
                                        ty: field_ty.clone(),
                                    })
                                }
                            } else {
                                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                            }
                        } else {
                            self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                        }
                    } else {
                        self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                    }
                } else {
                    self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                };

                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: field_val,
                    store_ty: None,
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // None: store 0 to result
                self.set_current_block(none_block);
                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: zero_val,
                    store_ty: None,
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // Merge: load result
                self.set_current_block(merge_block);
                self.emit_inst(Inst::Load {
                    ptr: result_slot,
                    ty: IrType::I64,
                })
            }

            Expr::StructLit { name, fields, spread } => {
                // Resolve type alias to actual struct name
                let real_name = self.resolve_struct_name(name);
                // Allocate struct on stack, store fields
                let struct_ty = IrType::Struct(real_name.clone());
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: struct_ty.clone(),
                    region: self.current_region(),
                });

                // If there's a spread source, copy all fields from it first
                if let Some(spread_expr) = spread {
                    let src_ptr = self.lower_expr(spread_expr);
                    let layout_clone = self.module.get_struct(&real_name).cloned();
                    if let Some(layout) = layout_clone {
                        for (idx, (_, field_ty, _)) in layout.fields.iter().enumerate() {
                            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                struct_ptr: ptr,
                                field_index: idx as u32,
                                field_ty: field_ty.clone(),
                            });
                            let src_field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                struct_ptr: src_ptr,
                                field_index: idx as u32,
                                field_ty: field_ty.clone(),
                            });
                            if let IrType::Struct(ref field_struct_name) = field_ty {
                                self.emit_struct_copy(src_field_ptr, field_ptr, field_struct_name);
                            } else {
                                let val = self.emit_inst(Inst::Load {
                                    ptr: src_field_ptr,
                                    ty: field_ty.clone(),
                                });
                                self.emit_inst(Inst::Store {
                                    ptr: field_ptr,
                                    value: val,
                                    store_ty: None,
                                });
                            }
                        }
                    }
                }

                // Store each explicit field (overrides spread values)
                let layout_clone = self.module.get_struct(&real_name).cloned();
                if let Some(layout) = layout_clone {
                    for (field_name, field_expr) in fields {
                        let field_val = self.lower_expr(field_expr);
                        if let Some((idx, _)) = layout.fields.iter()
                            .enumerate()
                            .find(|(_, (n, _, _))| n == field_name)
                        {
                            let field_ty = layout.fields[idx].1.clone();
                            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                                struct_ptr: ptr,
                                field_index: idx as u32,
                                field_ty: field_ty.clone(),
                            });
                            // If the field is a struct, the value is a pointer to struct data.
                            // Copy field-by-field rather than storing a pointer.
                            if let IrType::Struct(ref field_struct_name) = field_ty {
                                self.emit_struct_copy(field_val, field_ptr, field_struct_name);
                            } else {
                                self.emit_inst(Inst::Store {
                                    ptr: field_ptr,
                                    value: field_val,
                                    store_ty: None,
                                });
                            }
                        }
                    }
                }

                // Return the pointer to the struct (not a load — structs can be > 8 bytes)
                // The value type is Ptr(Struct(name)) so FieldAccess can work with it
                ptr
            }

            Expr::ArrayLit { elem_type, elements } => {
                // Allocate array on stack, return pointer
                let elem_ty = self.lower_type(elem_type);
                let array_ty = IrType::Array(Box::new(elem_ty.clone()), elements.len());
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: array_ty.clone(),
                    region: self.current_region(),
                });

                // Store each element
                for (i, elem_expr) in elements.iter().enumerate() {
                    let elem_val = self.lower_expr(elem_expr);
                    let idx = self.emit_inst(Inst::ConstInt { value: i as i64, ty: IrType::I64 });
                    let elem_ptr = self.emit_inst(Inst::GetElemPtr {
                        array_ptr: ptr,
                        index: idx,
                        elem_ty: elem_ty.clone(),
                    });
                    self.emit_inst(Inst::Store {
                        ptr: elem_ptr,
                        value: elem_val,
                        store_ty: None,
                    });
                }

                // Return the pointer — arrays are accessed via pointers
                ptr
            }

            Expr::Index { array, index } => {
                let arr_val = self.lower_expr(array);
                let arr_ty = self.get_value_type(arr_val);

                // String slicing: s[start..end] → p_string_slice(s, start, end)
                if arr_ty.is_opaque_named("String") {
                    if let Expr::Range { start, end, inclusive } = index.as_ref() {
                        let start_val = self.lower_expr(start);
                        let end_val = self.lower_expr(end);
                        // For inclusive range, add 1 to end
                        let final_end = if *inclusive {
                            let one = self.emit_inst(Inst::ConstInt { value: 1, ty: IrType::I64 });
                            self.emit_inst(Inst::BinOp {
                                op: BinOp::Add,
                                left: end_val,
                                right: one,
                                ty: IrType::I64,
                            })
                        } else {
                            end_val
                        };
                        return self.emit_inst(Inst::Call {
                            func: "p_string_slice".to_string(),
                            args: vec![arr_val, start_val, final_end],
                            ret_ty: IrType::Opaque("String".to_string()),
                            effects: EffectSet::new(),
                        });
                    }
                }

                let idx_val = self.lower_expr(index);

                // Vec indexing — call Fides Vec_get with correct element type (Phase 10B)
                if arr_ty.is_opaque_named("Vec") {
                    let elem_ty = arr_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                    let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                    return self.emit_inst(Inst::Call {
                        func: "Vec_get".to_string(),
                        args: vec![dummy_env, arr_val, idx_val],
                        ret_ty: Self::word_repr(elem_ty),
                        effects: EffectSet::new(),
                    });
                }

                let elem_ty = match &arr_ty {
                    IrType::Array(inner, _) => *inner.clone(),
                    // An array variable holds a pointer to its array.
                    IrType::Ptr(p) => match p.as_ref() {
                        IrType::Array(inner, _) => *inner.clone(),
                        _ => IrType::I64,
                    },
                    _ => IrType::I64,
                };
                let elem_ptr = self.emit_inst(Inst::GetElemPtr {
                    array_ptr: arr_val,
                    index: idx_val,
                    elem_ty: elem_ty.clone(),
                });
                self.emit_inst(Inst::Load {
                    ptr: elem_ptr,
                    ty: elem_ty,
                })
            }

            Expr::If { cond, then_block, else_block } => {
                // If as expression — use alloca to merge branch results.
                // 1. Infer the result type from the then-branch's last expression
                // 2. Allocate a result slot of the correct type
                // 3. In each branch, store the result value to the slot
                // 4. In the merge block, load from the slot
                let result_ty = self.infer_expr_type(&Expr::If {
                    cond: cond.clone(),
                    then_block: then_block.clone(),
                    else_block: else_block.clone(),
                });
                let result_slot = self.emit_inst(Inst::Alloca {
                    ty: result_ty.clone(),
                    region: self.current_region(),
                });

                let cond_val = self.lower_expr(cond);
                let then_bb = self.new_block();
                let else_bb = self.new_block();
                let merge_bb = self.new_block();

                self.emit_terminator(Inst::Branch {
                    cond: cond_val,
                    then: then_bb,
                    else_: else_bb,
                });

                // Lower then block — capture last expression value, store to slot
                self.set_current_block(then_bb);
                // Save variable bindings so they don't leak across branches
                let saved_allocas = self.allocas.clone();
                let saved_alloca_types = self.alloca_types.clone();
                let saved_var_ast_types = self.var_ast_types.clone();
                let saved_locals = self.current_func.as_ref().map(|f| f.locals.clone());
                let mut then_val: Option<ValueId> = None;
                for stmt in then_block {
                    if let Stmt::Expr(e, _) = stmt {
                        then_val = Some(self.lower_expr(e));
                    } else {
                        self.lower_stmt(stmt);
                        then_val = None;
                    }
                }
                if let Some(tv) = then_val {
                    self.emit_inst(Inst::Store { ptr: result_slot, value: tv, store_ty: None });
                }
                if !self.current_block_has_terminator() {
                    self.emit_terminator(Inst::Jump { target: merge_bb });
                }
                // Restore variable bindings
                self.allocas = saved_allocas.clone();
                self.alloca_types = saved_alloca_types.clone();
                self.var_ast_types = saved_var_ast_types.clone();
                if let Some(ref mut f) = self.current_func {
                    f.locals = saved_locals.clone().unwrap_or_default();
                }

                // Lower else block — capture last expression value, store to slot
                self.set_current_block(else_bb);
                let mut else_val: Option<ValueId> = None;
                if let Some(else_stmts) = else_block {
                    for stmt in else_stmts {
                        if let Stmt::Expr(e, _) = stmt {
                            else_val = Some(self.lower_expr(e));
                        } else {
                            self.lower_stmt(stmt);
                            else_val = None;
                        }
                    }
                }
                if let Some(ev) = else_val {
                    self.emit_inst(Inst::Store { ptr: result_slot, value: ev, store_ty: None });
                }
                if !self.current_block_has_terminator() {
                    self.emit_terminator(Inst::Jump { target: merge_bb });
                }
                // Restore variable bindings
                self.allocas = saved_allocas;
                self.alloca_types = saved_alloca_types;
                self.var_ast_types = saved_var_ast_types;
                if let Some(ref mut f) = self.current_func {
                    f.locals = saved_locals.unwrap_or_default();
                }

                // Merge block — load the result from the slot
                self.set_current_block(merge_bb);
                self.emit_inst(Inst::Load { ptr: result_slot, ty: result_ty })
            }

            Expr::Range { .. } => {
                // Ranges are handled by for loop lowering
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }

            Expr::Block(stmts) => {
                // Lower all statements; if the last one is an expression,
                // its value becomes the block's value
                let mut last_val: Option<ValueId> = None;
                for stmt in stmts {
                    if let Stmt::Expr(e, _) = stmt {
                        last_val = Some(self.lower_expr(e));
                    } else {
                        self.lower_stmt(stmt);
                        last_val = None;
                    }
                }
                last_val.unwrap_or_else(|| self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 }))
            }

            Expr::FString { parts } => {
                // Lower f-string by concatenating all parts
                // Each literal part → ConstStr, each expr part → convert to string → concat
                let mut result: Option<ValueId> = None;

                for part in parts {
                    let part_str = match part {
                        FStringPart::Literal(s) => {
                            self.emit_inst(Inst::ConstStr { value: s.clone() })
                        }
                        FStringPart::Expr(e) => {
                            // Lower the expression
                            let val = self.lower_expr(e);
                            // Determine type and dispatch to appropriate to_str function
                            let ty = if let Some(ref f) = self.current_func {
                                f.values.get(&val).map(|m| m.ty.clone()).unwrap_or(IrType::I64)
                            } else {
                                IrType::I64
                            };
                            match ty {
                                IrType::Ptr(_) => val, // already a string
                                IrType::F64 | IrType::F32 => {
                                    self.emit_inst(Inst::Call {
                                        func: "p_float_to_str".to_string(),
                                        args: vec![val],
                                        ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                                        effects: EffectSet::new(),
                                    })
                                }
                                IrType::Bool => {
                                    self.emit_inst(Inst::Call {
                                        func: "p_bool_to_str".to_string(),
                                        args: vec![val],
                                        ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                                        effects: EffectSet::new(),
                                    })
                                }
                                IrType::Opaque(ref name) | IrType::Generic(ref name, _)
                                    if name.contains("Vec") => {
                                    let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                                    self.emit_inst(Inst::Call {
                                        func: "Vec_to_str".to_string(),
                                        args: vec![dummy_env, val],
                                        ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                                        effects: EffectSet::new(),
                                    })
                                }
                                _ => {
                                    // Call p_to_str for int and other types
                                    self.emit_inst(Inst::Call {
                                        func: "p_to_str".to_string(),
                                        args: vec![val],
                                        ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                                        effects: EffectSet::new(),
                                    })
                                }
                            }
                        }
                    };

                    result = Some(match result {
                        None => part_str,
                        Some(prev) => {
                            self.emit_inst(Inst::Call {
                                func: "p_str_concat".to_string(),
                                args: vec![prev, part_str],
                                ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                                effects: EffectSet::new(),
                            })
                        }
                    });
                }

                result.unwrap_or_else(|| self.emit_inst(Inst::ConstStr { value: String::new() }))
            }

            Expr::Match { expr, arms } => {
                // Match as expression — use alloca to merge arm results
                let result_ty = self.infer_match_type(arms);
                let result_slot = self.emit_inst(Inst::Alloca {
                    ty: result_ty.clone(),
                    region: self.current_region(),
                });
                self.lower_match_expr(expr, arms, result_slot);
                self.emit_inst(Inst::Load { ptr: result_slot, ty: result_ty })
            }

            Expr::EnumLit { enum_name, variant, args } => {
                // Enum variant: stored as (tag, data...) in a struct-like layout
                // Tag is the variant index, data fields follow
                // We represent enums as a struct with first field = tag (i64), rest = data fields
                let enum_ty = IrType::Struct(enum_name.clone());
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: enum_ty.clone(),
                    region: self.current_region(),
                });

                // Find variant index
                let variant_idx = if let Some(enum_def) = self.enums.get(enum_name) {
                    enum_def.variants.iter().position(|v| v.name == *variant).unwrap_or(0) as i64
                } else {
                    0
                };

                // Store tag at field 0
                let tag_val = self.emit_inst(Inst::ConstInt { value: variant_idx, ty: IrType::I64 });
                let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: ptr,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: tag_ptr, value: tag_val, store_ty: None });

                // Store data fields starting at field 1.
                // Enum-typed fields occupy multiple 8-byte slots, so we need
                // to compute the correct field index based on the accumulated
                // size of previous fields.
                //
                // CRITICAL: struct/enum-typed fields (e.g. BinOp in Expr::Binary)
                // must be stored INLINE (copied from the arg's alloca into the
                // parent enum's data slots). Storing a pointer to the arg's
                // alloca creates a dangling pointer when the creating function
                // returns (especially in C codegen where alloca is stack-based).
                //
                // Box<T> fields are already pointers (Ptr(Struct(T))) and stay
                // as pointers — this is correct because Box is an explicit
                // indirection.
                //
                // To distinguish: we look at the DECLARED field type from the
                // enum definition. If it's a struct/enum (Type::Named that
                // resolves to a struct), store inline. If it's Box/Ptr/Vec/etc,
                // store the pointer.
                // NOTE: Type parameters (e.g. T in Option<T>) are NOT inline
                // structs — they are word-sized values (i64/ptr) at runtime.
                let enum_def = self.enums.get(enum_name);
                let enum_type_params: Vec<String> = enum_def
                    .map(|def| def.type_params.clone())
                    .unwrap_or_default();
                let declared_field_tys: Vec<Type> = enum_def
                    .and_then(|def| def.variants.iter().find(|v| v.name == *variant))
                    .and_then(|v| v.data.as_ref())
                    .map(|params| params.iter().map(|p| p.ty.clone()).collect())
                    .unwrap_or_default();

                let mut field_idx: u32 = 1;
                for (i, arg_expr) in args.iter().enumerate() {
                    let arg_val = self.lower_expr(arg_expr);
                    let arg_ty = self.get_value_type(arg_val);

                    // Check the declared field type to determine if this field
                    // should be stored inline (struct/enum) or as a pointer
                    // (Box/Ptr/Vec/str/etc).
                    let declared_ty = declared_field_tys.get(i);
                    // Check if the declared type is a type parameter of this enum.
                    // Type params are word-sized at runtime, not inline structs.
                    let is_type_param = declared_ty.map(|t| {
                        if let Type::Named(n) = t {
                            enum_type_params.iter().any(|p| p == n)
                        } else {
                            false
                        }
                    }).unwrap_or(false);
                    let declared_ir_ty = if is_type_param {
                        // Type parameter: use word representation of the arg type
                        Some(Self::word_repr(arg_ty.clone()))
                    } else {
                        declared_ty.map(|t| self.lower_type(t))
                    };
                    let is_inline_struct = declared_ir_ty.as_ref()
                        .map(|t| matches!(t, IrType::Struct(_)))
                        .unwrap_or(false);

                    // Determine how many 8-byte slots this field occupies.
                    let field_slots = if is_inline_struct {
                        if let IrType::Struct(ref name) = declared_ir_ty.as_ref().unwrap() {
                            if let Some(layout) = self.module.get_struct(name) {
                                ((layout.size + 7) / 8) as u32
                            } else { 1 }
                        } else { 1 }
                    } else {
                        1
                    };

                    if is_inline_struct {
                        // Struct/enum-typed field: copy data inline from the
                        // arg's alloca into the parent enum's data slots.
                        // This avoids dangling pointers when the creating
                        // function returns (C codegen uses stack alloca).
                        // arg_val is a pointer to the struct (alloca or temporary).
                        // We use a single memcpy (Inst::Store with store_ty) to
                        // copy the full struct data into the enum's inline slots.
                        let dst_ptr = self.emit_inst(Inst::GetFieldPtr {
                            struct_ptr: ptr,
                            field_index: field_idx,
                            field_ty: IrType::I64,
                        });
                        let struct_name = if let IrType::Struct(ref name) = declared_ir_ty.as_ref().unwrap() {
                            name.clone()
                        } else {
                            String::new()
                        };
                        self.emit_inst(Inst::Store {
                            ptr: dst_ptr,
                            value: arg_val,
                            store_ty: Some(IrType::Struct(struct_name)),
                        });
                    } else {
                        // Non-struct field (Box/Ptr/Vec/str/scalar): store the
                        // value (which is already a pointer or scalar) directly.
                        let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                            struct_ptr: ptr,
                            field_index: field_idx,
                            field_ty: arg_ty.clone(),
                        });
                        self.emit_inst(Inst::Store { ptr: field_ptr, value: arg_val, store_ty: None });
                    }
                    field_idx += field_slots;
                }

                ptr
            }

            Expr::Ref { expr, is_mut } => {
                // I-7A: Track borrow provenance for alias analysis.
                // A shared borrow (&x) is read-only; a mutable borrow (&mut x)
                // is exclusive. The borrow checker ensures they can't coexist.
                let borrow = if *is_mut {
                    crate::ir::BorrowKind::Mut
                } else {
                    crate::ir::BorrowKind::Shared
                };

                // If taking a reference to a variable that has an alloca,
                // return the alloca pointer directly (no copy).
                // This ensures &mut v actually points to v's storage.
                if let Expr::Ident(name) = expr.as_ref() {
                    if let Some(alloca) = self.allocas.get(name) {
                        // I-7A: Tag the alloca value with borrow kind.
                        if let Some(f) = self.current_func.as_mut() {
                            if let Some(meta) = f.values.get_mut(alloca) {
                                meta.borrow_kind = borrow;
                            }
                        }
                        // If the variable holds a pointer to a struct (SRET return),
                        // the alloca stores the struct pointer. &var should return
                        // the struct pointer (loaded from alloca), not the alloca
                        // address — otherwise we get an extra level of indirection.
                        let alloca_ty = self.alloca_types.get(alloca).cloned();
                        if let Some(IrType::Ptr(inner)) = &alloca_ty {
                            if matches!(inner.as_ref(), IrType::Struct(_)) {
                                return self.emit_inst(Inst::Load {
                                    ptr: *alloca,
                                    ty: alloca_ty.unwrap(),
                                });
                            }
                        }
                        return *alloca;
                    }
                }
                // Otherwise: alloca, store, return pointer
                let val = self.lower_expr(expr);
                let val_ty = self.get_value_type(val);
                // If the expression already evaluates to a pointer to a struct
                // (e.g. from a FieldAccess on a &mut parameter), the value IS
                // the pointer to the struct storage. &mut of this should return
                // it directly — wrapping it in another alloca creates a double
                // indirection (**struct instead of *struct) that breaks field
                // access through the mutable reference.
                if matches!(&val_ty, IrType::Ptr(inner) if matches!(inner.as_ref(), IrType::Struct(_))) {
                    if let Some(f) = self.current_func.as_mut() {
                        if let Some(meta) = f.values.get_mut(&val) {
                            meta.borrow_kind = borrow;
                        }
                    }
                    return val;
                }
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: val_ty.clone(),
                    region: self.current_region(),
                });
                // I-7A: Tag the new alloca with borrow kind.
                if let Some(f) = self.current_func.as_mut() {
                    if let Some(meta) = f.values.get_mut(&ptr) {
                        meta.borrow_kind = borrow;
                    }
                }
                self.emit_inst(Inst::Store {
                    ptr,
                    value: val,
                    store_ty: None,
                });
                ptr
            }

            Expr::Deref { expr } => {
                let ptr = self.lower_expr(expr);
                // Infer the pointee type from the pointer's type metadata
                let pointee_ty = self.get_value_type(ptr);
                let load_ty = match &pointee_ty {
                    IrType::Ptr(inner) => *inner.clone(),
                    _ => IrType::I64,
                };
                // If loading a struct, return the pointer itself (structs are accessed via pointers)
                if matches!(load_ty, IrType::Struct(_)) {
                    return ptr;
                }
                self.emit_inst(Inst::Load {
                    ptr,
                    ty: load_ty,
                })
            }

            Expr::StrConcat { left, right } => {
                let left_val = self.lower_expr(left);
                let right_val = self.lower_expr(right);
                self.emit_inst(Inst::Call {
                    func: "p_str_concat".to_string(),
                    args: vec![left_val, right_val],
                    ret_ty: IrType::Ptr(Box::new(IrType::U8)),
                    effects: {
                        let mut e = EffectSet::new();
                        e.add(Effect::Alloc);
                        e.add(Effect::IO);
                        e
                    },
                })
            }

            Expr::Try { expr } => {
                // ? operator — evaluate expression, check if it's Ok/Err
                // For now, we model Result as a struct with (tag, value)
                // tag 0 = Ok, tag 1 = Err
                // If Err, propagate the error to the caller (or to the nearest try block)
                let val = self.lower_expr(expr);

                // Load the tag (field 0)
                let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: val,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                let tag = self.emit_inst(Inst::Load {
                    ptr: tag_ptr,
                    ty: IrType::I64,
                });

                let ok_block = self.new_block();
                let err_block = self.new_block();
                let merge_block = self.new_block();

                let zero_val = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                let is_ok = self.emit_inst(Inst::BinOp {
                    op: BinOp::Eq,
                    left: tag,
                    right: zero_val,
                    ty: IrType::Bool,
                });

                self.emit_terminator(Inst::Branch {
                    cond: is_ok,
                    then: ok_block,
                    else_: err_block,
                });

                // Ok: load the value (field 1) and continue
                self.set_current_block(ok_block);
                self.emit_terminator(Inst::Jump { target: merge_block });

                // Err: propagate the error
                self.set_current_block(err_block);
                if let Some(&(try_err_block, try_err_slot)) = self.try_stack.last() {
                    // Inside a try block: copy the Result struct field-by-field into err_slot
                    // (val is a *struct.Result pointer, err_slot is also a *struct.Result —
                    //  we can't just Store the pointer, we need to copy the struct contents)
                    let src_tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: val,
                        field_index: 0,
                        field_ty: IrType::I64,
                    });
                    let src_tag = self.emit_inst(Inst::Load { ptr: src_tag_ptr, ty: IrType::I64 });
                    let dst_tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: try_err_slot,
                        field_index: 0,
                        field_ty: IrType::I64,
                    });
                    self.emit_inst(Inst::Store { ptr: dst_tag_ptr, value: src_tag, store_ty: None });
                    let src_val_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: val,
                        field_index: 1,
                        field_ty: IrType::I64,
                    });
                    let src_val = self.emit_inst(Inst::Load { ptr: src_val_ptr, ty: IrType::I64 });
                    let dst_val_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: try_err_slot,
                        field_index: 1,
                        field_ty: IrType::I64,
                    });
                    self.emit_inst(Inst::Store { ptr: dst_val_ptr, value: src_val, store_ty: None });
                    self.emit_terminator(Inst::Jump { target: try_err_block });
                } else {
                    // Not inside a try block: return the error from the current function
                    self.emit_drops();
                    self.emit_defers();
                    if self.current_sret.is_some() {
                        self.emit_terminator(Inst::Return { value: None });
                    } else {
                        self.emit_terminator(Inst::Return { value: Some(val) });
                    }
                }

                // Merge: load the unwrapped value
                self.set_current_block(merge_block);
                let val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: val,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Load {
                    ptr: val_ptr,
                    ty: IrType::I64,
                })
            }

            Expr::Coalesce { expr, default } => {
                // ?? operator: expr ?? default
                // Option is modeled as struct { tag: i64, value: i64 }
                // tag 0 = Some, tag 1 = None
                // If Some, load value; if None, evaluate default
                let val = self.lower_expr(expr);

                // Allocate a temp slot for the result
                let result_slot = self.emit_inst(Inst::Alloca {
                    ty: IrType::I64,
                    region: self.current_region(),
                });

                // Load the tag (field 0)
                let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: val,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                let tag = self.emit_inst(Inst::Load {
                    ptr: tag_ptr,
                    ty: IrType::I64,
                });

                let some_block = self.new_block();
                let none_block = self.new_block();
                let merge_block = self.new_block();

                let zero_val = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                let is_some = self.emit_inst(Inst::BinOp {
                    op: BinOp::Eq,
                    left: tag,
                    right: zero_val,
                    ty: IrType::Bool,
                });

                self.emit_terminator(Inst::Branch {
                    cond: is_some,
                    then: some_block,
                    else_: none_block,
                });

                // Some: load the value (field 1) and store to result slot
                self.set_current_block(some_block);
                let val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: val,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                let some_val = self.emit_inst(Inst::Load {
                    ptr: val_ptr,
                    ty: IrType::I64,
                });
                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: some_val,
                    store_ty: None,
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // None: evaluate the default and store to result slot
                self.set_current_block(none_block);
                let default_val = self.lower_expr(default);
                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: default_val,
                    store_ty: None,
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // Merge: load the result
                self.set_current_block(merge_block);
                self.emit_inst(Inst::Load {
                    ptr: result_slot,
                    ty: IrType::I64,
                })
            }

            Expr::TryBlock { body } => {
                // try { body } — catches ? early returns, wraps result in Result
                // Result is modeled as struct { tag: i64, value: i64 }
                // tag 0 = Ok, tag 1 = Err

                // Allocate slot for the error Result (captured by ? operator)
                let err_slot = self.emit_inst(Inst::Alloca {
                    ty: IrType::Struct("Result".to_string()),
                    region: self.current_region(),
                });

                // Allocate output Result struct
                let result_slot = self.emit_inst(Inst::Alloca {
                    ty: IrType::Struct("Result".to_string()),
                    region: self.current_region(),
                });

                let err_block = self.new_block();
                let cont_block = self.new_block();

                // Push try context so ? inside body jumps to err_block
                self.try_stack.push((err_block, err_slot));

                // Lower the body — last expression value becomes the Ok value
                let mut last_val: Option<ValueId> = None;
                for stmt in body {
                    if let Stmt::Expr(e, _) = stmt {
                        last_val = Some(self.lower_expr(e));
                    } else {
                        self.lower_stmt(stmt);
                        last_val = None;
                    }
                }
                let ok_val = last_val.unwrap_or_else(|| self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 }));

                // Pop try context
                self.try_stack.pop();

                // Normal completion: build Result::Ok(ok_val)
                // tag = 0 (Ok), value = ok_val
                let ok_tag = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: result_slot,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: tag_ptr, value: ok_tag, store_ty: None });
                let val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: result_slot,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: val_ptr, value: ok_val, store_ty: None });
                self.emit_terminator(Inst::Jump { target: cont_block });

                // Error block: ? jumped here with the error Result in err_slot
                self.set_current_block(err_block);
                // Copy err_slot → result_slot
                let err_tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: err_slot,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                let err_tag = self.emit_inst(Inst::Load { ptr: err_tag_ptr, ty: IrType::I64 });
                let out_tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: result_slot,
                    field_index: 0,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: out_tag_ptr, value: err_tag, store_ty: None });
                let err_val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: err_slot,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                let err_val = self.emit_inst(Inst::Load { ptr: err_val_ptr, ty: IrType::I64 });
                let out_val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: result_slot,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: out_val_ptr, value: err_val, store_ty: None });
                self.emit_terminator(Inst::Jump { target: cont_block });

                // Continuation: result_slot holds the final Result
                self.set_current_block(cont_block);
                result_slot
            }

            Expr::IsCheck { expr, type_name } => {
                // `expr is Type` or `expr is Variant`
                let val = self.lower_expr(expr);

                // Check if type_name is a primitive type (compile-time check)
                let is_primitive = matches!(type_name.as_str(),
                    "i8" | "i16" | "i32" | "i64" | "i128" |
                    "u8" | "u16" | "u32" | "u64" | "u128" |
                    "f32" | "f64" | "bool" | "str" | "void"
                );

                if is_primitive {
                    // Compile-time type check — always true or false based on inferred type
                    let expr_ty = self.infer_expr_type(expr);
                    let matches = match (&expr_ty, type_name.as_str()) {
                        (IrType::I64, "i64") => true,
                        (IrType::I32, "i32") => true,
                        (IrType::I8, "i8") => true,
                        (IrType::I16, "i16") => true,
                        (IrType::U64, "u64") => true,
                        (IrType::U32, "u32") => true,
                        (IrType::U8, "u8") => true,
                        (IrType::U16, "u16") => true,
                        (IrType::F64, "f64") => true,
                        (IrType::F32, "f32") => true,
                        (IrType::Bool, "bool") => true,
                        (IrType::Void, "void") => true,
                        (IrType::Ptr(inner), "str") if matches!(**inner, IrType::U8) => true,
                        _ => false,
                    };
                    self.emit_inst(Inst::ConstBool { value: matches })
                } else {
                    // Enum variant check: load tag (field 0) and compare to variant index
                    // Search all enums for a matching variant name
                    let variant_idx = {
                        let mut found = None;
                        for (_, enum_def) in &self.enums {
                            if let Some(idx) = enum_def.variants.iter().position(|v| v.name == *type_name) {
                                found = Some(idx as i64);
                                break;
                            }
                        }
                        // Also check if type_name is an enum name (always true if type matches)
                        if found.is_none() && self.enums.contains_key(type_name) {
                            // `expr is EnumName` — always true if type matches
                            found = Some(0); // tag 0 means it's at least the first variant
                        }
                        found.unwrap_or(0)
                    };
                    let tag_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: val,
                        field_index: 0,
                        field_ty: IrType::I64,
                    });
                    let tag = self.emit_inst(Inst::Load {
                        ptr: tag_ptr,
                        ty: IrType::I64,
                    });
                    let expected = self.emit_inst(Inst::ConstInt { value: variant_idx, ty: IrType::I64 });
                    self.emit_inst(Inst::BinOp {
                        op: BinOp::Eq,
                        left: tag,
                        right: expected,
                        ty: IrType::Bool,
                    })
                }
            }

            Expr::TupleLit(elems) => {
                // Allocate tuple on stack as a struct, store each element, return pointer
                let tuple_size = elems.len();
                let tuple_name = format!("__tuple_{}", tuple_size);
                let tuple_ty = IrType::Struct(tuple_name.clone());
                // Lower all elements first to get their types
                let mut elem_vals = Vec::new();
                let mut elem_tys = Vec::new();
                for e in elems {
                    let val = self.lower_expr(e);
                    let ty = self.get_value_type(val);
                    elem_vals.push(val);
                    elem_tys.push(ty);
                }
                // For struct-typed elements (Ptr(Struct)), store the struct data
                // inline in the tuple field (not just the pointer). This prevents
                // dangling pointers when the tuple is returned from a function.
                // The field type is the inner struct type, and the field offset
                // is the cumulative size of preceding fields.
                let mut fields: Vec<(String, IrType, u32)> = Vec::new();
                let mut offset: u32 = 0;
                let mut field_tys_resolved: Vec<IrType> = Vec::new();
                for (i, ty) in elem_tys.iter().enumerate() {
                    let field_ty = if let IrType::Ptr(ref inner) = ty {
                        if let IrType::Struct(_) = inner.as_ref() {
                            (**inner).clone()
                        } else {
                            ty.clone()
                        }
                    } else {
                        ty.clone()
                    };
                    let field_size = self.module.type_size(&field_ty) as u32;
                    fields.push((format!("_{}", i), field_ty.clone(), offset));
                    field_tys_resolved.push(field_ty);
                    offset += field_size;
                    // Align to 8 bytes
                    offset = (offset + 7) & !7;
                }
                let size = offset as u64;
                self.module.add_struct(StructLayout {
                    name: tuple_name.clone(),
                    fields,
                    size,
                    align: 8,
                    repr_c: false,
                    packed_layout: false,
                });
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: tuple_ty.clone(),
                    region: self.current_region(),
                });
                for (i, val) in elem_vals.iter().enumerate() {
                    let val_ty = &elem_tys[i];
                    let field_ty = &field_tys_resolved[i];
                    let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: ptr,
                        field_index: i as u32,
                        field_ty: field_ty.clone(),
                    });
                    if let IrType::Ptr(ref inner) = val_ty {
                        if let IrType::Struct(_) = inner.as_ref() {
                            // Struct element: memcpy from the source pointer to
                            // the field pointer (stores struct data inline).
                            self.emit_inst(Inst::StructCopy {
                                dst: field_ptr,
                                src: *val,
                                ty: (**inner).clone(),
                            });
                            continue;
                        }
                    }
                    // Non-struct element: store the value directly
                    self.emit_inst(Inst::Store { ptr: field_ptr, value: *val, store_ty: None });
                }
                ptr
            }

            Expr::Closure { params, body, ret_ty } => {
                // Lambda lifting with environment struct:
                // 1. Find free variables (captures) in the closure body
                // 2. Create lifted function: (env_ptr: *u8, ...own_params) -> ret
                //    Inside, load captures from env struct fields
                // 3. At creation site: alloca env struct, store captures, return struct ptr
                // 4. CallPtr passes env_ptr as first arg

                let closure_id = self.module.functions.len();
                let closure_name = format!("__closure_{}", closure_id);

                // Find free variables
                let param_names: Vec<String> = params.iter().map(|p| p.name.clone()).collect();
                let captures = self.find_captures(body, &param_names);

                // Build the lifted function — lower_func adds __env as first param automatically
                let all_params = params.clone();

                // Save current captures and set new ones for this closure
                let saved_captures = std::mem::take(&mut self.current_captures);
                for (i, (cap_name, cap_ty)) in captures.iter().enumerate() {
                    // Capture field index in env struct is i+1 (field 0 is fn_name)
                    self.current_captures.push((cap_name.clone(), i + 1, cap_ty.clone()));
                }

                // Infer return type if not explicitly specified
                let inferred_ret_ty = ret_ty.clone().unwrap_or_else(|| {
                    self.infer_closure_ret_ty(body)
                });

                let closure_func = Func {
                    name: closure_name.clone(),
                    type_params: vec![],
                    type_param_bounds: vec![],
                    const_params: vec![],
                    params: all_params,
                    ret_ty: inferred_ret_ty,
                    body: body.clone(),
                    is_pub: false,
                    is_comptime: false,
                    declared_effects: Vec::new(),
                    has_effect_annotation: false,
                    is_interrupt: false,
            annotations: Vec::new(),
                    preconditions: Vec::new(),
                    postconditions: Vec::new(),
            is_async: false,
            line: 0,
                };

                // Save current function context, lower the closure, restore context.
                // lower_func clears self.allocas/alloca_types/vec_allocas, so save those too.
                let saved_func = self.current_func.take();
                let saved_allocas = std::mem::take(&mut self.allocas);
                let saved_alloca_types = std::mem::take(&mut self.alloca_types);
                let saved_var_ast_types = std::mem::take(&mut self.var_ast_types);
                let saved_vec_allocas = std::mem::take(&mut self.vec_allocas);
                let saved_block = self.current_block;
                self.lower_func(&closure_func);
                self.current_func = saved_func;
                self.allocas = saved_allocas;
                self.alloca_types = saved_alloca_types;
                self.var_ast_types = saved_var_ast_types;
                self.vec_allocas = saved_vec_allocas;
                self.current_block = saved_block;
                self.current_captures = saved_captures;

                // At the creation site: build env struct with captures
                if captures.is_empty() {
                    // No captures — still create env struct for uniform CallPtr handling
                    self.make_fn_env(&closure_name, &[])
                } else {
                    // Lower capture values from enclosing scope
                    let cap_vals: Vec<(String, ValueId)> = captures.iter()
                        .map(|(name, _)| {
                            let val = self.lower_expr(&Expr::Ident(name.clone()));
                            (name.clone(), val)
                        })
                        .collect();
                    self.make_fn_env(&closure_name, &cap_vals)
                }
            }
            Expr::Cast { expr, target_ty } => {
                // Casting to a trait object (dyn Trait) creates a Dyn value
                if let crate::ast::Type::Dyn(trait_name) = target_ty {
                    let val = self.lower_expr(expr);
                    let src_ir_ty = self.get_value_type(val);
                    let type_name = match &src_ir_ty {
                        crate::ir::IrType::Struct(n) => n.clone(),
                        crate::ir::IrType::Ptr(inner) => {
                            match inner.as_ref() {
                                crate::ir::IrType::Struct(n) => n.clone(),
                                _ => "Unknown".to_string(),
                            }
                        }
                        _ => "Unknown".to_string(),
                    };
                    return self.emit_inst(Inst::MakeDyn {
                        value: val,
                        type_name,
                        trait_name: trait_name.clone(),
                    });
                }
                let val = self.lower_expr(expr);
                let src_ty = self.get_value_type(val);
                let dst_ir_ty = self.lower_type(target_ty);
                self.lower_cast(val, src_ty, dst_ir_ty)
            }
            Expr::Asm { template, operands } => {
                let mut inputs: Vec<(String, ValueId)> = Vec::new();
                let mut outputs: Vec<(String, ValueId)> = Vec::new();

                for op in operands {
                    match op.direction {
                        crate::ast::AsmDir::In => {
                            let val = self.lower_expr(&op.expr);
                            inputs.push((op.reg.clone(), val));
                        }
                        crate::ast::AsmDir::Out => {
                            // For out operands, we lower the expression to get a pointer
                            // (it should be an lvalue). For now, lower the expr and store
                            // the result after asm execution.
                            let val = self.lower_expr(&op.expr);
                            outputs.push((op.reg.clone(), val));
                        }
                        crate::ast::AsmDir::Clobber => {
                            // Clobber operands are passed as inputs with "CL:" prefix
                            // so the backend can identify and save/restore them.
                            // The expression is a dummy (const 0).
                            let val = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                            inputs.push((format!("CL:{}", op.reg), val));
                        }
                    }
                }

                let ret_ty = if outputs.is_empty() {
                    crate::ir::IrType::Void
                } else {
                    crate::ir::IrType::I64
                };

                self.emit_inst(Inst::Asm {
                    template: template.clone(),
                    inputs,
                    outputs,
                    ret_ty,
                })
            }
            Expr::Comptime(inner) => self.lower_expr(inner),
            Expr::ComptimeBlock(_) => {
                // Should not reach here after comptime substitution; emit const 0
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }
            Expr::ComptimeReflect { .. } => {
                // Should not reach here after comptime substitution; emit const 0
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }
            Expr::PipeForward { left, right } => {
                // Desugar left |> right into a function/method call
                match right.as_ref() {
                    Expr::Call { callee, args } => {
                        let mut all_args = vec![left.as_ref().clone()];
                        all_args.extend(args.iter().cloned());
                        let new_call = Expr::Call { callee: callee.clone(), args: all_args };
                        self.lower_expr(&new_call)
                    }
                    Expr::MethodCall { receiver, method, args } => {
                        let new_call = Expr::MethodCall {
                            receiver: left.clone(),
                            method: method.clone(),
                            args: args.clone(),
                        };
                        let _ = receiver; // suppress unused warning
                        self.lower_expr(&new_call)
                    }
                    Expr::Ident(name) => {
                        // x |> f  →  f(x)
                        let new_call = Expr::Call {
                            callee: name.clone(),
                            args: vec![left.as_ref().clone()],
                        };
                        self.lower_expr(&new_call)
                    }
                    Expr::FieldAccess { expr: receiver, field } => {
                        // x |> obj.method  →  obj.method(x)
                        let new_call = Expr::MethodCall {
                            receiver: receiver.clone(),
                            method: field.clone(),
                            args: vec![],
                        };
                        // But we need to pass `left` as the first arg.
                        // Actually: x |> obj.method means obj.method(x)
                        // So we need to construct: MethodCall { receiver: receiver, method: field, args: [left] }
                        let new_call = Expr::MethodCall {
                            receiver: receiver.clone(),
                            method: field.clone(),
                            args: vec![left.as_ref().clone()],
                        };
                        self.lower_expr(&new_call)
                    }
                    _ => {
                        self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
                    }
                }
            }
            Expr::QuantCast { expr, target_ty } => {
                // Lower as a cast to the target type
                let val = self.lower_expr(expr);
                let src_ty = self.get_value_type(val);
                let dst_ir_ty = self.lower_type(target_ty);
                self.lower_cast(val, src_ty, dst_ir_ty)
            }
            Expr::Await { expr: inner } => {
                // await <expr> — poll a Future to get its value.
                // The inner expression evaluates to a Future<T> (an opaque handle).
                // p_future_poll extracts the value from the Future.
                let future_val = self.lower_expr(inner);
                self.emit_inst(Inst::Call {
                    func: "p_future_poll".to_string(),
                    args: vec![future_val],
                    ret_ty: IrType::I64,
                    effects: EffectSet::new(),
                })
            }
            Expr::Spawn { expr: inner } => {
                // spawn <expr> — create a concurrent task.
                // L3B: If inside a `spawn scope` block, use the scope executor.
                // Otherwise, use p_thread_spawn (creates an OS thread).
                match &**inner {
                    Expr::Call { callee, args, .. } => {
                        let func_name_val = self.emit_inst(Inst::ConstStr { value: callee.clone() });
                        let num_args_val = self.emit_inst(Inst::ConstInt {
                            value: args.len() as i64,
                            ty: IrType::I64,
                        });
                        // L3B: Use scope executor if available, else p_thread_spawn
                        if let Some(exec) = self.scope_executor_stack.last().copied() {
                            // spawn func(args) → p_executor_spawn(exec, func_name, num_args, args...)
                            let mut all_args = vec![exec, func_name_val, num_args_val];
                            for arg in args {
                                all_args.push(self.lower_expr(arg));
                            }
                            self.emit_inst(Inst::Call {
                                func: "p_executor_spawn".to_string(),
                                args: all_args,
                                ret_ty: IrType::I64,
                                effects: EffectSet::new(),
                            })
                        } else {
                            // spawn func(args) → p_thread_spawn(func_name, num_args, args...)
                            let mut all_args = vec![func_name_val, num_args_val];
                            for arg in args {
                                all_args.push(self.lower_expr(arg));
                            }
                            self.emit_inst(Inst::Call {
                                func: "p_thread_spawn".to_string(),
                                args: all_args,
                                ret_ty: IrType::I64,
                                effects: EffectSet::new(),
                            })
                        }
                    }
                    _ => {
                        // Non-call spawn — fall back to eager evaluation
                        self.lower_expr(inner)
                    }
                }
            }
        }
    }

    /// Infer the return type of a closure from its body by examining ret statements
    fn infer_closure_ret_ty(&self, body: &[Stmt]) -> Type {
        for stmt in body {
            match stmt {
                Stmt::Return(Some(expr), _) => {
                    return self.infer_expr_ast_type(expr);
                }
                Stmt::Expr(expr, _) => {
                    // Last expression in block (implicit return)
                    return self.infer_expr_ast_type(expr);
                }
                _ => {}
            }
        }
        Type::I64
    }

    /// Infer the type of an expression (best-effort, for closure return type inference)
    fn infer_expr_ast_type(&self, expr: &Expr) -> Type {
        match expr {
            Expr::Binary { op, .. } => {
                match op {
                    BinOp::Lt | BinOp::LtEq | BinOp::Gt | BinOp::GtEq | BinOp::Eq | BinOp::Neq
                    | BinOp::And | BinOp::Or => Type::Bool,
                    _ => Type::I64,
                }
            }
            Expr::IntLit(_) | Expr::Unary { op: UnaryOp::Neg, .. } => Type::I64,
            Expr::BoolLit(_) => Type::Bool,
            Expr::FloatLit(_) => Type::F64,
            Expr::StringLit(_) => Type::Str,
            Expr::Call { callee, .. } => {
                self.func_ret_types.get(callee).cloned().unwrap_or(Type::I64)
            }
            Expr::Ident(name) => {
                // Look up variable's AST type (recorded during Stmt::Let)
                self.var_ast_types.get(name).cloned().unwrap_or(Type::I64)
            }
            _ => Type::I64,
        }
    }

    /// Find free variables in a closure body — identifiers used but not defined as params or locals
    fn find_captures(&self, body: &[Stmt], param_names: &[String]) -> Vec<(String, Type)> {
        let mut free_vars: Vec<String> = Vec::new();
        let mut local_names: Vec<String> = param_names.to_vec();
        self.collect_free_vars(body, &mut free_vars, &mut local_names);

        // For each free variable, look up its type from the enclosing scope
        let mut captures = Vec::new();
        for name in &free_vars {
            // Try to find the type from alloca types or function locals
            if let Some(alloca) = self.allocas.get(name) {
                let ty = self.alloca_types.get(alloca).cloned().unwrap_or(IrType::I64);
                captures.push((name.clone(), ir_type_to_ast_type(&ty)));
            } else if let Some(ref func) = self.current_func {
                // IR params are (ValueId, IrType) — check by looking up the name in the AST func params
                // For now, just use I64 as the capture type
                // TODO: properly look up the type
                captures.push((name.clone(), Type::I64));
            }
        }
        captures
    }

    fn collect_free_vars(&self, stmts: &[Stmt], free_vars: &mut Vec<String>, local_names: &mut Vec<String>) {
        for stmt in stmts {
            match stmt {
                Stmt::Let { name, expr, .. } => {
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                    local_names.push(name.clone());
                }
                Stmt::LetUninit { name, .. } => {
                    local_names.push(name.clone());
                }
                Stmt::LetTuple { names, expr, .. } => {
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                    for name in names {
                        local_names.push(name.clone());
                    }
                }
                Stmt::Assign { target, expr, .. } => {
                    if !local_names.contains(target) && !free_vars.contains(target) {
                        free_vars.push(target.clone());
                    }
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                }
                Stmt::IndexAssign { array, index, expr, .. } => {
                    if !local_names.contains(array) && !free_vars.contains(array) {
                        free_vars.push(array.clone());
                    }
                    self.collect_free_vars_expr(index, free_vars, local_names);
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                }
                Stmt::DerefAssign { ptr_expr, expr, .. } => {
                    self.collect_free_vars_expr(ptr_expr, free_vars, local_names);
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                }
                Stmt::FieldAssign { object, expr, .. } => {
                    self.collect_free_vars_expr(object, free_vars, local_names);
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                }
                Stmt::Expr(e, _) => self.collect_free_vars_expr(e, free_vars, local_names),
                Stmt::Return(e, _) => {
                    if let Some(e) = e {
                        self.collect_free_vars_expr(e, free_vars, local_names);
                    }
                }
                Stmt::If { cond, then_block, else_block, .. } => {
                    self.collect_free_vars_expr(cond, free_vars, local_names);
                    self.collect_free_vars(then_block, free_vars, local_names);
                    if let Some(eb) = else_block {
                        self.collect_free_vars(eb, free_vars, local_names);
                    }
                }
                Stmt::While { cond, body, .. } => {
                    self.collect_free_vars_expr(cond, free_vars, local_names);
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::For { var, iterator, body, .. } => {
                    self.collect_free_vars_expr(iterator, free_vars, local_names);
                    local_names.push(var.clone());
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Loop { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Break(e, _) => {
                    if let Some(e) = e {
                        self.collect_free_vars_expr(e, free_vars, local_names);
                    }
                }
                Stmt::Continue(_) => {}
                Stmt::Defer(body, _) => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Match { expr, arms, .. } => {
                    self.collect_free_vars_expr(expr, free_vars, local_names);
                    for arm in arms {
                        if let Pattern::EnumVariant { bindings, .. } = &arm.pattern {
                            for (pat_name, _) in bindings {
                                local_names.push(pat_name.clone());
                            }
                        }
                        if let Pattern::Ident(name) = &arm.pattern {
                            local_names.push(name.clone());
                        }
                        self.collect_free_vars(&arm.body, free_vars, local_names);
                    }
                }
                Stmt::Use { .. } => {}
                Stmt::Handle { body, handlers, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                    for handler in handlers {
                        self.collect_free_vars(&handler.body, free_vars, local_names);
                    }
                }
                Stmt::Do { args, .. } => {
                    for arg in args {
                        self.collect_free_vars_expr(arg, free_vars, local_names);
                    }
                }
                Stmt::Resume { cont, value, .. } => {
                    self.collect_free_vars_expr(cont, free_vars, local_names);
                    self.collect_free_vars_expr(value, free_vars, local_names);
                }
                Stmt::RegionBlock { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Sandbox { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Unsafe { body, .. } | Stmt::Txn { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::ComptimeGen { .. } => {}
                Stmt::GradBlock { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Backward { loss, .. } => {
                    self.collect_free_vars_expr(loss, free_vars, local_names);
                }
                Stmt::Query { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::Select { cases, default, .. } => {
                    for case in cases {
                        self.collect_free_vars_expr(&case.channel, free_vars, local_names);
                        self.collect_free_vars(&case.body, free_vars, local_names);
                    }
                    if let Some(def_body) = default {
                        self.collect_free_vars(def_body, free_vars, local_names);
                    }
                }
                Stmt::Parallel { branches, .. } => {
                    for branch in branches {
                        self.collect_free_vars(branch, free_vars, local_names);
                    }
                }
                Stmt::SpawnScope { body, .. } => {
                    self.collect_free_vars(body, free_vars, local_names);
                }
                Stmt::CancelScope { .. } => {}
            }
        }
    }

    fn collect_free_vars_expr(&self, expr: &Expr, free_vars: &mut Vec<String>, local_names: &[String]) {
        match expr {
            Expr::Ident(name) => {
                if !local_names.contains(name) && !free_vars.contains(name) {
                    // Check if it's a known variable in the enclosing scope
                    if self.allocas.contains_key(name)
                        || self.current_func.as_ref().map(|f| f.params.len() > 0).unwrap_or(false)
                    {
                        free_vars.push(name.clone());
                    }
                }
            }
            Expr::Binary { left, right, .. } => {
                self.collect_free_vars_expr(left, free_vars, local_names);
                self.collect_free_vars_expr(right, free_vars, local_names);
            }
            Expr::Unary { expr, .. } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
            }
            Expr::Call { callee, args } => {
                // Don't treat function names as free variables
                if !self.module.functions.contains_key(callee) && !self.module.extern_functions.contains_key(callee) {
                    if !local_names.contains(callee) && !free_vars.contains(callee) {
                        if self.allocas.contains_key(callee)
                            || self.current_func.as_ref().map(|f| f.params.len() > 0).unwrap_or(false)
                        {
                            free_vars.push(callee.clone());
                        }
                    }
                }
                for a in args {
                    self.collect_free_vars_expr(a, free_vars, local_names);
                }
            }
            Expr::FieldAccess { expr, .. } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
            }
            Expr::OptionalFieldAccess { expr, .. } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
            }
            Expr::MethodCall { receiver, args, .. } => {
                self.collect_free_vars_expr(receiver, free_vars, local_names);
                for a in args {
                    self.collect_free_vars_expr(a, free_vars, local_names);
                }
            }
            Expr::Index { array, index } => {
                self.collect_free_vars_expr(array, free_vars, local_names);
                self.collect_free_vars_expr(index, free_vars, local_names);
            }
            Expr::StructLit { fields, spread, .. } => {
                for (_, e) in fields {
                    self.collect_free_vars_expr(e, free_vars, local_names);
                }
                if let Some(spread_expr) = spread {
                    self.collect_free_vars_expr(spread_expr, free_vars, local_names);
                }
            }
            Expr::If { cond, then_block, else_block } => {
                self.collect_free_vars_expr(cond, free_vars, local_names);
                for s in then_block {
                    self.collect_free_vars(&[s.clone()], free_vars, &mut local_names.to_vec());
                }
                if let Some(eb) = else_block {
                    for s in eb {
                        self.collect_free_vars(&[s.clone()], free_vars, &mut local_names.to_vec());
                    }
                }
            }
            Expr::Block(stmts) => {
                for s in stmts {
                    self.collect_free_vars(&[s.clone()], free_vars, &mut local_names.to_vec());
                }
            }
            Expr::Ref { expr, .. } => self.collect_free_vars_expr(expr, free_vars, local_names),
            Expr::Deref { expr } => self.collect_free_vars_expr(expr, free_vars, local_names),
            Expr::ArrayLit { elements, .. } => {
                for e in elements {
                    self.collect_free_vars_expr(e, free_vars, local_names);
                }
            }
            Expr::FString { parts } => {
                for p in parts {
                    if let crate::ast::FStringPart::Expr(e) = p {
                        self.collect_free_vars_expr(e, free_vars, local_names);
                    }
                }
            }
            Expr::Match { expr, arms } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
                for arm in arms {
                    self.collect_free_vars(&arm.body, free_vars, &mut local_names.to_vec());
                }
            }
            Expr::EnumLit { args, .. } => {
                for a in args {
                    self.collect_free_vars_expr(a, free_vars, local_names);
                }
            }
            Expr::StrConcat { left, right } => {
                self.collect_free_vars_expr(left, free_vars, local_names);
                self.collect_free_vars_expr(right, free_vars, local_names);
            }
            Expr::Try { expr } => self.collect_free_vars_expr(expr, free_vars, local_names),
            Expr::Coalesce { expr, default } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
                self.collect_free_vars_expr(default, free_vars, local_names);
            }
            Expr::IsCheck { expr, .. } => self.collect_free_vars_expr(expr, free_vars, local_names),
            Expr::TryBlock { body } => {
                for stmt in body {
                    match stmt {
                        Stmt::Let { expr, .. } | Stmt::LetTuple { expr, .. } | Stmt::Assign { expr, .. } | Stmt::Expr(expr, _) => {
                            self.collect_free_vars_expr(expr, free_vars, local_names);
                        }
                        _ => {}
                    }
                }
            }
            Expr::TupleLit(elems) => {
                for e in elems {
                    self.collect_free_vars_expr(e, free_vars, local_names);
                }
            }
            Expr::Range { start, end, .. } => {
                self.collect_free_vars_expr(start, free_vars, local_names);
                self.collect_free_vars_expr(end, free_vars, local_names);
            }
            Expr::Cast { expr, .. } => self.collect_free_vars_expr(expr, free_vars, local_names),
            Expr::Asm { operands, .. } => {
                for op in operands {
                    self.collect_free_vars_expr(&op.expr, free_vars, local_names);
                }
            }
            Expr::Closure { params, body, .. } => {
                let mut inner_locals = local_names.to_vec();
                for p in params {
                    inner_locals.push(p.name.clone());
                }
                self.collect_free_vars(body, free_vars, &mut inner_locals);
            }
            Expr::IntLit(_) | Expr::FloatLit(_) | Expr::BoolLit(_) | Expr::StringLit(_) => {}
            Expr::Comptime(inner) => {
                self.collect_free_vars_expr(inner, free_vars, local_names);
            }
            Expr::ComptimeBlock(stmts) => {
                self.collect_free_vars(stmts, free_vars, &mut local_names.to_vec());
            }
            Expr::ComptimeReflect { .. } => {}
            Expr::PipeForward { left, right } => {
                self.collect_free_vars_expr(left, free_vars, local_names);
                self.collect_free_vars_expr(right, free_vars, local_names);
            }
            Expr::QuantCast { expr, .. } => {
                self.collect_free_vars_expr(expr, free_vars, local_names);
            }
            Expr::Await { expr: inner } | Expr::Spawn { expr: inner } => {
                self.collect_free_vars_expr(inner, free_vars, local_names);
            }
        }
    }

    /// A function declared `-> f64`/`-> f32` may return an integer expression
    /// (`return 1;`): convert it numerically, as `val x: f64 = 1;` does.
    fn coerce_return(&mut self, val: ValueId) -> ValueId {
        let ret_ty = match &self.current_func {
            Some(f) => f.ret_ty.clone(),
            None => return val,
        };
        let val_ty = self.get_value_type(val);
        if ret_ty.is_float() && val_ty.is_int() {
            return self.lower_cast(val, val_ty, ret_ty);
        }
        val
    }

    /// Lower a type cast (expr as Type)
    fn lower_cast(&mut self, val: ValueId, src: IrType, dst: IrType) -> ValueId {
        match (&src, &dst) {
            // Same type — no-op
            _ if src == dst => val,
            // Int to int: extend or truncate
            (a, b) if a.is_int() && b.is_int() => {
                let src_size = self.module.type_size(&src);
                let dst_size = self.module.type_size(&dst);
                if dst_size > src_size {
                    self.emit_inst(Inst::IntExtend { value: val, from: src, to: dst })
                } else if dst_size < src_size {
                    self.emit_inst(Inst::IntTrunc { value: val, from: src, to: dst })
                } else {
                    self.emit_inst(Inst::BitCast { value: val, from: src, to: dst })
                }
            }
            // Int to float
            (a, b) if a.is_int() && b.is_float() => {
                self.emit_inst(Inst::IntToFloat { value: val, from: src, to: dst })
            }
            // Float to int
            (a, b) if a.is_float() && b.is_int() => {
                self.emit_inst(Inst::FloatToInt { value: val, from: src, to: dst })
            }
            // Float to float
            (a, b) if a.is_float() && b.is_float() => {
                self.emit_inst(Inst::BitCast { value: val, from: src, to: dst })
            }
            // Anything else: bitcast
            _ => self.emit_inst(Inst::BitCast { value: val, from: src, to: dst }),
        }
    }

    /// Lower compound assignment (+=, -=, etc.)
    fn lower_compound_assign(&mut self, op: &AssignOp, current: ValueId, rhs: ValueId) -> ValueId {
        let binop = match op {
            AssignOp::AddAssign => BinOp::Add,
            AssignOp::SubAssign => BinOp::Sub,
            AssignOp::MulAssign => BinOp::Mul,
            AssignOp::DivAssign => BinOp::Div,
            AssignOp::ModAssign => BinOp::Mod,
            AssignOp::BitAndAssign => BinOp::BitAnd,
            AssignOp::BitOrAssign => BinOp::BitOr,
            AssignOp::BitXorAssign => BinOp::BitXor,
            AssignOp::ShlAssign => BinOp::Shl,
            AssignOp::ShrAssign => BinOp::Shr,
            AssignOp::Assign => unreachable!(),
        };
        let ty = self.get_value_type(current);
        self.emit_inst(Inst::BinOp {
            op: binop,
            left: current,
            right: rhs,
            ty,
        })
    }

    // ─── Helpers ────────────────────────────────────────────────────────

    fn emit_inst(&mut self, inst: Inst) -> ValueId {
        if let Some(ref mut f) = self.current_func {
            let id = f.emit(inst);
            // Set source line for debug info
            if let Some(line) = self.current_line {
                if let Some(meta) = f.values.get_mut(&id) {
                    meta.source_line = Some(line);
                }
            }
            // I-3C: Mark instructions emitted inside a sandbox block
            if self.sandbox_depth > 0 {
                if let Some(meta) = f.values.get_mut(&id) {
                    meta.in_sandbox = true;
                }
            }
            // I-6B: Mark values with linear struct types.
            // Extract the struct name (if any) while holding the mutable borrow,
            // then check linear_structs after the borrow ends.
            let struct_name: Option<String> = f.values.get(&id).and_then(|meta| {
                if let IrType::Struct(name) = &meta.ty {
                    Some(name.clone())
                } else {
                    None
                }
            });
            // I-6B: Set is_linear based on @linear struct membership
            if let Some(name) = &struct_name {
                if self.linear_structs.contains(name) {
                    if let Some(meta) = f.values.get_mut(&id) {
                        meta.is_linear = true;
                    }
                }
            }
            // Override: emit() uses next_block-1, but we need current_block
            // Move the instruction to the correct block
            let wrong_idx = (f.next_block - 1) as usize;
            let right_idx = self.current_block.0 as usize;
            if wrong_idx != right_idx && wrong_idx < f.blocks.len() && right_idx < f.blocks.len() {
                if let Some(&id) = f.blocks[wrong_idx].instructions.last() {
                    f.blocks[wrong_idx].instructions.pop();
                    f.blocks[right_idx].instructions.push(id);
                }
            }
            id
        } else {
            ValueId(0)
        }
    }

    /// Override the type of a value (used for closure env struct typing)
    fn set_value_type(&mut self, id: ValueId, ty: IrType) {
        if let Some(ref mut f) = self.current_func {
            if let Some(meta) = f.values.get_mut(&id) {
                meta.ty = ty;
            }
        }
    }

    /// Infer the element type for Vec::new() based on the current context.
    /// Uses the `let` hint (`pv v: Vec<Token> = Vec::new();`) when available,
    /// otherwise defaults to I64.
    fn infer_vec_elem_type(&self) -> IrType {
        if let Some(Type::Generic { name, args }) = &self.let_hint {
            if name == "Vec" {
                if let Some(elem) = args.first() {
                    return self.lower_type(elem);
                }
            }
        }
        IrType::I64
    }

    /// Infer key and value types for HashMap::new() based on the current context.
    /// Returns (key_type, value_type). Uses the `let` hint when available.
    fn infer_map_types(&self) -> (IrType, IrType) {
        if let Some(Type::Generic { name, args }) = &self.let_hint {
            if name == "HashMap" {
                let key = args.first().map(|t| self.lower_type(t))
                    .unwrap_or(IrType::Opaque("String".to_string()));
                let val = args.get(1).map(|t| self.lower_type(t))
                    .unwrap_or(IrType::I64);
                return (key, val);
            }
        }
        (IrType::Opaque("String".to_string()), IrType::I64)
    }

    /// Create an env struct for a function reference: [fn_addr, cap1, cap2, ...]
    /// Returns a heap-allocated env struct pointer
    fn make_fn_env(&mut self, fn_name: &str, captures: &[(String, ValueId)]) -> ValueId {
        let env_size = ((1 + captures.len()) * 8) as i64;
        let size_val = self.emit_inst(Inst::ConstInt { value: env_size, ty: IrType::I64 });
        let env_ptr = self.emit_inst(Inst::Call {
            func: "p_malloc".to_string(),
            args: vec![size_val],
            ret_ty: IrType::Ptr(Box::new(IrType::U8)),
            effects: {
                let mut es = EffectSet::new();
                es.add(Effect::Alloc);
                es
            },
        });

        // Store function reference at field 0
        // ConstFnRef stores the function name; PE emitter resolves it to a code address,
        // interpreter/C codegen treat it as a string for name-based lookup.
        let fn_name_val = self.emit_inst(Inst::ConstFnRef { name: fn_name.to_string() });
        let zero_idx = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let field0 = self.emit_inst(Inst::GetElemPtr {
            array_ptr: env_ptr,
            index: zero_idx,
            elem_ty: IrType::I64,
        });
        self.emit_inst(Inst::Store { ptr: field0, value: fn_name_val, store_ty: None });

        // Store captures at fields 1..N
        for (i, (_, cap_val)) in captures.iter().enumerate() {
            let idx = self.emit_inst(Inst::ConstInt { value: (i + 1) as i64, ty: IrType::I64 });
            let field = self.emit_inst(Inst::GetElemPtr {
                array_ptr: env_ptr,
                index: idx,
                elem_ty: IrType::I64,
            });
            self.emit_inst(Inst::Store { ptr: field, value: *cap_val, store_ty: None });
        }

        self.set_value_type(env_ptr, IrType::Opaque("closure".to_string()));
        env_ptr
    }

    fn emit_terminator(&mut self, inst: Inst) {
        self.emit_inst(inst);
    }

    fn new_block(&mut self) -> BlockId {
        if let Some(ref mut f) = self.current_func {
            f.new_block()
        } else {
            BlockId(0)
        }
    }

    fn set_current_block(&mut self, block: BlockId) {
        self.current_block = block;
    }

    fn current_block_has_terminator(&self) -> bool {
        if let Some(ref f) = self.current_func {
            let block_idx = self.current_block.0 as usize;
            if block_idx < f.blocks.len() {
                f.blocks[block_idx].instructions.iter()
                    .any(|id| f.instructions.get(id).map(|i| i.is_terminator()).unwrap_or(false))
            } else {
                false
            }
        } else {
            false
        }
    }

    /// Lower a vec method call for operator overloading (e.g. vec3_add(a, b))
    fn lower_vec_method_call(&mut self, vec_name: &str, method: &str, receiver: &Expr, arg: &Expr) -> ValueId {
        let recv_val = self.lower_expr(receiver);
        let arg_val = self.lower_expr(arg);
        let mangled = format!("{}_{}", vec_name, method);
        let ret_ty = IrType::Struct(vec_name.to_string());
        self.lower_vec_sret_call(&mangled, &ret_ty, recv_val, arg_val)
    }

    /// Lower a vec SRET call (struct return via sret pointer)
    fn lower_vec_sret_call(&mut self, func_name: &str, ret_ty: &IrType, recv_val: ValueId, arg_val: ValueId) -> ValueId {
        let struct_name = if let IrType::Struct(name) = ret_ty { name.clone() } else { String::new() };
        let sret_ptr = self.emit_inst(Inst::Alloca {
            ty: IrType::Struct(struct_name.clone()),
            region: self.current_region(),
        });
        let mut sret_args = vec![sret_ptr];
        // Prepend dummy env_ptr for user functions
        if !self.module.extern_functions.contains_key(func_name) {
            let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
            sret_args.push(dummy_env);
        }
        sret_args.push(recv_val);
        sret_args.push(arg_val);
        let mut effects = EffectSet::new();
        if let Some(e) = self.module.function_effects.get(func_name) {
            effects.add_all(e);
        }
        self.emit_inst(Inst::Call {
            func: func_name.to_string(),
            args: sret_args,
            ret_ty: IrType::Void,
            effects,
        });
        sret_ptr
    }

    /// Lower a swizzle access (e.g. vec4.xyz → vec3, vec4.rgb → vec3)
    fn lower_swizzle(&mut self, vec_name: &str, struct_ptr: ValueId, swizzle: &str) -> ValueId {
        // Get the field indices from the swizzle pattern
        let indices: Vec<usize> = swizzle.chars().map(swizzle_index).collect();
        // Load each component from the source vec
        let components: Vec<ValueId> = indices.iter().map(|&idx| {
            let field_ty = IrType::F64;
            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                struct_ptr,
                field_index: idx as u32,
                field_ty: field_ty.clone(),
            });
            self.emit_inst(Inst::Load { ptr: field_ptr, ty: field_ty })
        }).collect();
        // Construct the result vec type
        let result_name = match swizzle.len() {
            2 => "vec2",
            3 => "vec3",
            4 => "vec4",
            _ => "vec3",
        };
        let result_ty = IrType::Struct(result_name.to_string());
        let sret_ptr = self.emit_inst(Inst::Alloca {
            ty: result_ty.clone(),
            region: self.current_region(),
        });
        // Store each component into the result struct
        for (i, comp) in components.iter().enumerate() {
            let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                struct_ptr: sret_ptr,
                field_index: i as u32,
                field_ty: IrType::F64,
            });
            self.emit_inst(Inst::Store { ptr: field_ptr, value: *comp, store_ty: None });
        }
        sret_ptr
    }

    fn infer_binop_type(&self, op: &BinOp, left: &Expr, right: &Expr) -> IrType {
        match op {
            BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq
            | BinOp::And | BinOp::Or => IrType::Bool,
            _ => {
                let lt = self.infer_expr_type(left);
                let rt = self.infer_expr_type(right);
                // String concatenation: str + str → str (Ptr(U8))
                // Only when BOTH are strings — str + i64 is pointer arithmetic.
                if matches!(op, BinOp::Add) {
                    let str_ty = IrType::Ptr(Box::new(IrType::U8));
                    if lt == str_ty && rt == str_ty {
                        return str_ty;
                    }
                }
                if lt.is_float() || rt.is_float() {
                    if lt == IrType::F32 || rt == IrType::F32 {
                        IrType::F32
                    } else {
                        IrType::F64
                    }
                } else {
                    IrType::I64
                }
            }
        }
    }

    fn infer_unary_type(&self, _op: &UnaryOp, expr: &Expr) -> IrType {
        let t = self.infer_expr_type(expr);
        if t.is_float() { t } else { IrType::I64 }
    }

    fn lookup_func_ret_ty(&self, name: &str) -> IrType {
        if let Some((_, ret)) = self.module.extern_functions.get(name) {
            return ret.clone();
        }
        if let Some(f) = self.module.functions.get(name) {
            return f.ret_ty.clone();
        }
        // Fallback: check AST function return types (for functions not yet lowered)
        if let Some(ast_ty) = self.func_ret_types.get(name) {
            return self.lower_type(ast_ty);
        }
        IrType::I64 // Default for unknown functions
    }

    fn get_value_type(&self, val: ValueId) -> IrType {
        if let Some(ref f) = self.current_func {
            if let Some(meta) = f.values.get(&val) {
                return meta.ty.clone();
            }
        }
        IrType::I64
    }

    /// Infer the IrType of an AST expression without lowering it.
    /// Used for type-aware alloca slots in if-as-expression.
    fn infer_expr_type(&self, expr: &Expr) -> IrType {
        match expr {
            Expr::IntLit(_) => IrType::I64,
            Expr::FloatLit(_) => IrType::F64,
            Expr::BoolLit(_) => IrType::Bool,
            Expr::StringLit(_) => IrType::Ptr(Box::new(IrType::U8)),
            Expr::Ident(name) => {
                // Check mutable variable (alloca) — type is stored in alloca_types
                if let Some(alloca) = self.allocas.get(name) {
                    if let Some(ty) = self.alloca_types.get(alloca) {
                        return ty.clone();
                    }
                    return IrType::I64;
                }
                // Check local binding — type is the value's type.
                // But if the value type is I64 or Opaque (e.g. enum data slots
                // are all I64, or Vec is Opaque), fall through to AST type
                // checks which may have richer generic type info.
                if let Some(val) = self.current_func.as_ref().and_then(|f| f.lookup_local(name)) {
                    if let Some(meta) = self.current_func.as_ref().and_then(|f| f.values.get(&val)) {
                        if !matches!(meta.ty, IrType::I64 | IrType::Opaque(_)) {
                            return meta.ty.clone();
                        }
                    }
                }
                // Check constant
                if let Some(const_expr) = self.constants.get(name) {
                    return self.infer_expr_type(const_expr);
                }
                // Check module-level mutable global
                if let Some((ir_ty, _init)) = self.module.globals.get(name) {
                    return ir_ty.clone();
                }
                // Check AST type (e.g. for function parameters with generic types like Vec<T>)
                if let Some(ast_ty) = self.var_ast_types.get(name) {
                    let ir_ty = self.lower_type(ast_ty);
                    return ir_ty;
                }
                // Check parameter AST types (for Vec element type inference)
                if let Some(ast_ty) = self.current_func_param_types.get(name) {
                    return self.lower_type(ast_ty);
                }
                IrType::I64
            }
            Expr::Binary { op, left, right } => self.infer_binop_type(op, left, right),
            Expr::Unary { op, expr } => self.infer_unary_type(op, expr),
            Expr::Call { callee, .. } => self.lookup_func_ret_ty(callee),
            Expr::StructLit { name, .. } => {
                IrType::Ptr(Box::new(IrType::Struct(name.clone())))
            }
            Expr::FieldAccess { expr, field } => {
                let struct_ty = self.infer_expr_type(expr);
                let struct_name = match &struct_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
                    _ => None,
                };
                if let Some(name) = struct_name {
                    if let Some(layout) = self.module.get_struct(&name) {
                        if let Some((_, ty, _)) = layout.fields.iter()
                            .find(|(n, _, _)| n == field)
                        {
                            return ty.clone();
                        }
                    }
                }
                IrType::I64
            }
            Expr::OptionalFieldAccess { .. } => IrType::I64,
            Expr::If { then_block, .. } => {
                // Infer type from the last expression in the then block
                for stmt in then_block {
                    if let Stmt::Expr(e, _) = stmt {
                        return self.infer_expr_type(e);
                    }
                }
                IrType::I64
            }
            Expr::Block(stmts) => {
                // Infer type from the last expression in the block
                for stmt in stmts {
                    if let Stmt::Expr(e, _) = stmt {
                        return self.infer_expr_type(e);
                    }
                }
                IrType::Void
            }
            Expr::Ref { expr, .. } => {
                IrType::Ptr(Box::new(self.infer_expr_type(expr)))
            }
            Expr::Deref { expr } => {
                let inner_ty = self.infer_expr_type(expr);
                match &inner_ty {
                    IrType::Ptr(inner) => *inner.clone(),
                    _ => IrType::I64,
                }
            }
            Expr::ArrayLit { elem_type, .. } => {
                let elem_ir = self.lower_type(elem_type);
                IrType::Ptr(Box::new(elem_ir))
            }
            Expr::Index { array, index, .. } => {
                let arr_ty = self.infer_expr_type(array);
                // String slicing returns String
                if arr_ty.is_opaque_named("String") {
                    return IrType::Opaque("String".to_string());
                }
                // Vec indexing returns element type
                if arr_ty.is_opaque_named("Vec") {
                    return arr_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                }
                match &arr_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Array(elem, _) => *elem.clone(),
                        _ => IrType::I64,
                    },
                    IrType::Array(elem, _) => *elem.clone(),
                    _ => IrType::I64,
                }
            }
            Expr::MethodCall { receiver, method, .. } => {
                // Check if this is Box::new(x) — returns a pointer
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Box" && method == "new" {
                        return IrType::Ptr(Box::new(IrType::U8));
                    }
                    if name == "Vec" && method == "new" {
                        let elem_ty = self.infer_vec_elem_type();
                        return IrType::Generic("Vec".to_string(), vec![elem_ty]);
                    }
                    if name == "HashMap" && method == "new" {
                        let (key_ty, val_ty) = self.infer_map_types();
                        return IrType::Generic("HashMap".to_string(), vec![key_ty, val_ty]);
                    }
                    if name == "String" && method == "from" {
                        return IrType::Opaque("String".to_string());
                    }
                    if self.structs.contains_key(name) {
                        let mangled = format!("{}_{}", name, method);
                        return self.lookup_func_ret_ty(&mangled);
                    }
                }
                let recv_ty = self.infer_expr_type(receiver);
                // Builtin Vec methods — infer from generic element type
                if let IrType::Generic(name, args) = &recv_ty {
                    if name == "Vec" {
                        let elem_ty = args.first().cloned().unwrap_or(IrType::I64);
                        return match method.as_str() {
                            "push" | "set" | "clear" | "insert" | "reverse" | "extend" | "truncate" | "sort" | "for_each" => IrType::Void,
                            "pop" | "get" | "remove" | "first" | "last" => elem_ty,
                            "len" | "find" | "position" => IrType::I64,
                            "is_empty" | "contains" | "any" | "all" => IrType::Bool,
                            "capacity" => IrType::I64,
                            "clone" | "map" | "filter" => IrType::Generic("Vec".to_string(), vec![elem_ty]),
                            "fold" => IrType::I64,
                            _ => {
                                let mangled = format!("Vec_{}", method);
                                return self.lookup_func_ret_ty(&mangled);
                            }
                        };
                    }
                }
                let struct_name = match &recv_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
                    IrType::Generic(name, _) => Some(name.clone()),
                    IrType::Opaque(name) => Some(name.clone()),
                    _ => None,
                };
                let mangled = if let Some(ref sn) = struct_name {
                    format!("{}_{}", sn, method)
                } else {
                    method.clone()
                };
                self.lookup_func_ret_ty(&mangled)
            }
            Expr::FString { .. } => IrType::Ptr(Box::new(IrType::U8)),
            Expr::StrConcat { .. } => IrType::Ptr(Box::new(IrType::U8)),
            Expr::Range { .. } => IrType::Void,
            Expr::Try { expr } => {
                // Try unwraps Result<T,E> -> T or Option<T> -> T
                // For now, infer from the inner expression
                self.infer_expr_type(expr)
            }
            Expr::Coalesce { default, .. } => {
                // ?? returns the type of the default (same as unwrapped option type)
                self.infer_expr_type(default)
            }
            Expr::IsCheck { .. } => IrType::Bool,
            Expr::TryBlock { .. } => {
                // try { } returns a Result struct
                IrType::Struct("Result".to_string())
            }
            Expr::TupleLit(elements) => {
                let field_tys: Vec<IrType> = elements.iter()
                    .map(|e| self.infer_expr_type(e))
                    .collect();
                IrType::Struct(format!("__tuple_{}", field_tys.len()))
            }
            Expr::EnumLit { enum_name, .. } => {
                IrType::Ptr(Box::new(IrType::Struct(enum_name.clone())))
            }
            Expr::Closure { ret_ty, .. } => {
                if let Some(rt) = ret_ty {
                    self.lower_type(rt)
                } else {
                    IrType::Void
                }
            }
            Expr::Match { arms, .. } => {
                // Infer from first arm's body
                for arm in arms {
                    for stmt in &arm.body {
                        if let Stmt::Expr(e, _) = stmt {
                            return self.infer_expr_type(e);
                        }
                    }
                }
                IrType::Void
            }
            Expr::Cast { target_ty, .. } => self.lower_type(target_ty),
            Expr::Asm { operands, .. } => {
                // asm! returns I64 if there are output operands, Void otherwise
                let has_out = operands.iter().any(|op| op.direction == crate::ast::AsmDir::Out);
                if has_out {
                    IrType::I64
                } else {
                    IrType::Void
                }
            }
            Expr::Comptime(inner) => self.infer_expr_type(inner),
            Expr::ComptimeBlock(_) => IrType::I64,
            Expr::ComptimeReflect { op, .. } => {
                use crate::ast::ReflectOp;
                match op {
                    ReflectOp::TypeName => IrType::Ptr(Box::new(IrType::U8)),
                    ReflectOp::IsStruct | ReflectOp::IsEnum => IrType::Bool,
                    ReflectOp::FieldCount | ReflectOp::StructSize | ReflectOp::FieldOffset
                    | ReflectOp::EnumDiscriminant => IrType::I64,
                    ReflectOp::FieldNames | ReflectOp::FieldTypes | ReflectOp::EnumVariants
                    | ReflectOp::FunctionEffects | ReflectOp::FunctionCapabilities => {
                        IrType::Array(Box::new(IrType::Ptr(Box::new(IrType::U8))), 0)
                    }
                }
            }
            Expr::PipeForward { right, .. } => {
                // Pipe forward type is the type of the right side (the call)
                self.infer_expr_type(right)
            }
            Expr::QuantCast { target_ty, .. } => {
                self.lower_type(target_ty)
            }
            Expr::Await { expr: inner } => {
                // await Future<T> returns T — unwrap the Future type
                let inner_ty = self.infer_expr_type(inner);
                match &inner_ty {
                    crate::ir::IrType::Struct(name) if name == "Future" => {
                        // Future is a struct — await extracts the value field
                        crate::ir::IrType::I64
                    }
                    _ => inner_ty,
                }
            }
            Expr::Spawn { expr: inner } => {
                // spawn <expr> returns Future<T> — same type as inner for now
                // (the IR-level type is the same; Future wrapping is type-level only)
                self.infer_expr_type(inner)
            }
        }
    }

    /// Emit all deferred blocks in reverse order (LIFO — last registered runs first)
    fn emit_defers(&mut self) {
        let defers: Vec<Vec<Stmt>> = self.defer_stack.drain(..).rev().collect();
        for body in defers {
            self.lower_stmts(&body);
        }
    }

    /// Emit compiler-inserted drop calls for all local variables whose type
    /// implements Drop. Called before every Return and at function end.
    /// Drops are emitted in reverse declaration order (LIFO — last declared
    /// is dropped first, matching Rust's drop order).
    fn emit_drops(&mut self) {
        // Emit zeroize calls first (wipe secrets before dropping)
        self.emit_zeroize();
        // Collect (alloca_ptr, struct_name) pairs for locals with Drop impls.
        // Clone first to avoid borrowing self while emitting.
        let drop_locals: Vec<(ValueId, String)> = if let Some(ref f) = self.current_func {
            f.drop_locals.clone()
        } else {
            return;
        };
        // Emit drops in reverse order (LIFO)
        for (alloca, struct_name) in drop_locals.into_iter().rev() {
            // Only emit if the current block doesn't already have a terminator
            if self.current_block_has_terminator() {
                break;
            }
            // Mangle: struct_name + "_drop"
            let drop_fn = format!("{}_drop", struct_name);
            // Load the struct value from the alloca
            let val = self.emit_inst(Inst::Load {
                ptr: alloca,
                ty: IrType::I64,
            });
            // User functions expect a dummy env_ptr as the first argument
            let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
            self.emit_inst(Inst::Call {
                func: drop_fn,
                args: vec![dummy_env, val],
                ret_ty: IrType::Void,
                effects: EffectSet::new(),
            });
        }
    }

    /// Emit compiler-inserted memset(0) calls for all @zeroize locals.
    /// Called before drops and returns. Wipes secrets from memory at scope exit.
    /// Emits in reverse declaration order (LIFO — last declared wiped first).
    fn emit_zeroize(&mut self) {
        let zeroize_locals: Vec<(ValueId, u64)> = if let Some(ref f) = self.current_func {
            f.zeroize_locals.clone()
        } else {
            return;
        };
        for (alloca, size) in zeroize_locals.into_iter().rev() {
            if self.current_block_has_terminator() {
                break;
            }
            // A str or Vec slot holds a pointer: wipe the bytes it points to
            // too (a string in the arena, a Vec's element buffer), not just
            // the 8-byte pointer.
            let slot_ty = self.alloca_types.get(&alloca).cloned();
            let heap_wipe = match &slot_ty {
                Some(IrType::Ptr(inner)) if **inner == IrType::U8 => Some("p_str_zeroize"),
                Some(t) if t.is_opaque_named("Vec") => Some("p_vec_zeroize"),
                _ => None,
            };
            if let (Some(func), Some(ty)) = (heap_wipe, slot_ty) {
                let held = self.emit_inst(Inst::Load { ptr: alloca, ty });
                let mut effects = EffectSet::new();
                effects.add(Effect::Write);
                self.emit_inst(Inst::Call {
                    func: func.to_string(),
                    args: vec![held],
                    ret_ty: IrType::Void,
                    effects,
                });
            }
            // Emit: p_memset(alloca_ptr, 0, size)
            // The alloca is a pointer to the variable's storage.
            // We call p_memset which is a runtime function: void* p_memset(void* ptr, int val, int64_t size)
            let zero_val = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I32 });
            let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
            let mut effects = EffectSet::new();
            effects.add(Effect::Write);
            self.emit_inst(Inst::Call {
                func: "p_memset".to_string(),
                args: vec![alloca, zero_val, size_val],
                ret_ty: IrType::Void,
                effects,
            });
        }
    }

    /// Check if a struct type has a Drop impl (a `drop` method).
    fn has_drop_impl(&self, struct_name: &str) -> bool {
        self.impl_methods.contains_key(&(struct_name.to_string(), "drop".to_string()))
    }

    /// Extract the struct name from a Type, if it's a Named or Generic type.
    fn type_struct_name(&self, ty: &Type) -> Option<String> {
        match ty {
            Type::Named(name) => Some(name.clone()),
            Type::Generic { name, .. } => Some(name.clone()),
            _ => None,
        }
    }

    /// Emit PersistSave instructions for all persist variables in the current function.
    /// Called before every Return to save persist variable values.
    fn emit_persist_saves(&mut self) {
        let persist_vars: Vec<(String, ValueId)> = if let Some(ref f) = self.current_func {
            f.persist_vars.clone()
        } else {
            return;
        };
        for (name, alloca) in persist_vars {
            // ML4: Use the actual type of the persist variable instead of
            // always loading as I64. This allows persisting pointers, tensors,
            // and other opaque types for hot-reload of model weights.
            let load_ty = self.alloca_types.get(&alloca)
                .cloned()
                .unwrap_or(IrType::I64);
            let loaded = self.emit_inst(Inst::Load { ptr: alloca, ty: load_ty });
            self.emit_inst(Inst::PersistSave {
                name,
                value: loaded,
            });
        }
    }

    /// Copy struct data from a source pointer to the SRET destination pointer.
    /// Used at return points in struct-returning functions.
    fn emit_sret_copy(&mut self, src_ptr: ValueId) {
        if let Some(sret) = self.current_sret {
            // Get the struct name from the function's return type
            if let Some(ref f) = self.current_func {
                let struct_name = match &f.ret_ty {
                    IrType::Struct(name) => name.clone(),
                    _ => return,
                };
                self.emit_struct_copy(src_ptr, sret, &struct_name);
            }
        }
    }

    /// Recursively copy struct data from src_ptr to dst_ptr, field by field.
    /// Handles nested structs by recursing into their fields.
    fn emit_struct_copy(&mut self, src_ptr: ValueId, dst_ptr: ValueId, struct_name: &str) {
        let layout = self.module.get_struct(struct_name).cloned();
        if let Some(layout) = layout {
            for (idx, (_, field_ty, _)) in layout.fields.iter().enumerate() {
                let src_field = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: src_ptr,
                    field_index: idx as u32,
                    field_ty: field_ty.clone(),
                });
                let dst_field = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: dst_ptr,
                    field_index: idx as u32,
                    field_ty: field_ty.clone(),
                });
                if let IrType::Struct(ref sub_name) = field_ty {
                    // Nested struct: recurse
                    self.emit_struct_copy(src_field, dst_field, sub_name);
                } else {
                    // Scalar field: load + store
                    let field_val = self.emit_inst(Inst::Load {
                        ptr: src_field,
                        ty: field_ty.clone(),
                    });
                    self.emit_inst(Inst::Store {
                        ptr: dst_field,
                        value: field_val,
                        store_ty: None,
                    });
                }
            }
        }
    }
}

/// Lower a program to IR — convenience function
pub fn lower(program: &Program) -> IrModule {
    let mut lowerer = Lowerer::new();
    lowerer.lower_program(program)
}

/// Convert an IrType back to an AST Type (for capture type lookup)
fn ir_type_to_ast_type(ty: &IrType) -> Type {
    match ty {
        IrType::I8 => Type::I8,
        IrType::I16 => Type::I16,
        IrType::I32 => Type::I32,
        IrType::I64 => Type::I64,
        IrType::U8 => Type::U8,
        IrType::U16 => Type::U16,
        IrType::U32 => Type::U32,
        IrType::U64 => Type::U64,
        IrType::F32 => Type::F32,
        IrType::F64 => Type::F64,
        IrType::Bool => Type::Bool,
        IrType::Void => Type::Void,
        IrType::Never => Type::Never,
        IrType::Ptr(inner) => Type::Ref { inner: Box::new(ir_type_to_ast_type(inner)), is_mut: false },
        IrType::Struct(name) => Type::Named(name.clone()),
        IrType::Array(elem, size) => Type::Array { elem: Box::new(ir_type_to_ast_type(elem)), size: *size },
        IrType::Opaque(name) => Type::Named(name.clone()),
        IrType::Generic(name, args) => Type::Generic {
            name: name.clone(),
            args: args.iter().map(ir_type_to_ast_type).collect(),
        },
    }
}
