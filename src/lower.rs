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
    /// Stack of active named regions — allocations inside a region block
    /// use Region::Named(name) instead of Region::Stack
    region_stack: Vec<String>,
    /// AST function return types — maps function name to AST return type
    /// Used by LetTuple to determine tuple element types
    func_ret_types: HashMap<String, Type>,
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

    pub fn new() -> Self {
        Lowerer {
            module: IrModule::new(),
            current_func: None,
            current_block: BlockId(0),
            loop_stack: Vec::new(),
            structs: HashMap::new(),
            allocas: HashMap::new(),
            alloca_types: HashMap::new(),
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
            region_stack: Vec::new(),
            func_ret_types: HashMap::new(),
        }
    }

    /// Lower a complete program to IR
    pub fn lower_program(&mut self, program: &Program) -> IrModule {
        // Register constants (pc) for inline substitution
        for c in &program.consts {
            self.constants.insert(c.name.clone(), c.expr.clone());
        }

        // Register type aliases
        for ta in &program.type_aliases {
            self.type_aliases.insert(ta.name.clone(), ta.alias_for.clone());
        }

        // Register struct definitions for layout computation
        for s in &program.structs {
            self.structs.insert(s.name.clone(), s.clone());
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
        }

        // Compute struct layouts
        for s in &program.structs {
            self.compute_struct_layout(s);
        }

        // Compute enum layouts
        for e in &program.enums {
            self.compute_enum_layout(e);
        }

        // Register runtime/external functions
        self.register_runtime();

        // Pre-register all function signatures so calls can look up return types
        // before the function body is lowered
        for func in &program.funcs {
            let ret_ty = self.lower_type(&func.ret_ty);
            let ir_func = IrFunction::new(func.name.clone(), ret_ty);
            self.module.functions.insert(func.name.clone(), ir_func);
            // Store AST return type for tuple element type lookup
            self.func_ret_types.insert(func.name.clone(), func.ret_ty.clone());
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
                    Param { name: p.name.clone(), ty: self.resolve_self_in_type(&p.ty), is_pub: p.is_pub }
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
                    Param { name: p.name.clone(), ty: self.resolve_self_in_type(&p.ty), is_pub: p.is_pub }
                }).collect();
                mangled.ret_ty = self.resolve_self_in_type(&mangled.ret_ty);
                self.lower_func(&mangled);
            }
            self.current_self_type = saved_self;
        }

        // Take ownership of the module
        std::mem::take(&mut self.module)
    }

    /// Compute the memory layout for a struct.
    /// All fields are stored as 8-byte slots to match the native codegen
    /// (which uses `field_index * 8` as the byte offset for each field).
    fn compute_struct_layout(&mut self, def: &StructDef) {
        let mut fields = Vec::new();
        let mut offset: u64 = 0;

        for field in &def.fields {
            let ir_ty = self.lower_type(&field.ty);
            // Every field gets an 8-byte slot (uniform representation).
            fields.push((field.name.clone(), ir_ty.clone(), offset as u32));
            offset += 8;
        }

        // Align to 8 bytes (all structs are 8-byte aligned in the native codegen).
        let size = (offset + 7) & !7;

        let layout = StructLayout {
            name: def.name.clone(),
            fields,
            size,
            align: 8,
        };

        self.module.add_struct(layout);
    }

    /// Compute layout for an enum type.
    /// Enums are laid out as: tag (i64) at offset 0, then data fields at 8-byte intervals.
    /// Size = (1 + max_variant_arity) * 8, aligned to 8.
    fn compute_enum_layout(&mut self, def: &EnumDef) {
        let mut max_fields = 0usize;
        for variant in &def.variants {
            if let Some(data) = &variant.data {
                max_fields = max_fields.max(data.len());
            }
        }

        let mut fields = Vec::new();
        // Field 0: tag
        fields.push(("__tag".to_string(), IrType::I64, 0u32));
        // Fields 1..max: data slots (all 8 bytes, enough for i64/f64/ptr)
        for i in 0..max_fields {
            fields.push((format!("__data{}", i), IrType::I64, ((i + 1) * 8) as u32));
        }

        let size = ((1 + max_fields) * 8) as u64;
        let layout = StructLayout {
            name: def.name.clone(),
            fields,
            size,
            align: 8,
        };
        self.module.add_struct(layout);
    }

    /// Register runtime functions that the IR can call
    fn register_runtime(&mut self) {
        self.module.add_extern("p_print_int", vec![IrType::I64], IrType::Void);
        self.module.add_extern("p_print_str", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_print_bool", vec![IrType::Bool], IrType::Void);
        self.module.add_extern("p_print_float", vec![IrType::F64], IrType::Void);
        self.module.add_extern("p_print_newline", vec![], IrType::Void);
        self.module.add_extern("p_panic", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_alloc", vec![IrType::I64], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_malloc", vec![IrType::I64], IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_free", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Void);
        self.module.add_extern("p_str_concat",
            vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_to_str",
            vec![IrType::I64],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_float_to_str",
            vec![IrType::F64],
            IrType::Ptr(Box::new(IrType::U8)));
        self.module.add_extern("p_bool_to_str",
            vec![IrType::Bool],
            IrType::Ptr(Box::new(IrType::U8)));
        let vec_ty = IrType::Opaque("Vec".to_string());
        self.module.add_extern("p_vec_to_str",
            vec![vec_ty.clone()],
            IrType::Ptr(Box::new(IrType::U8)));

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
        self.module.add_extern("p_file_exists", vec![IrType::Ptr(Box::new(IrType::U8))], IrType::Bool);
        self.module.add_extern("p_file_append", vec![IrType::Ptr(Box::new(IrType::U8)), IrType::Ptr(Box::new(IrType::U8))], IrType::I64);

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
        self.module.add_extern("p_idt_set_entry", vec![IrType::I64, IrType::I64], IrType::Void); // set IDT entry
        self.module.add_extern("p_hlt", vec![], IrType::Void);           // halt CPU
        self.module.add_extern("p_inb", vec![IrType::I64], IrType::I64);  // port I/O: read byte
        self.module.add_extern("p_outb", vec![IrType::I64, IrType::I64], IrType::Void); // port I/O: write byte

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
    }

    /// Lower a single function
    fn lower_func(&mut self, func: &Func) {
        let ret_ty = self.lower_type(&func.ret_ty);
        let mut ir_func = IrFunction::new(func.name.clone(), ret_ty.clone());
        ir_func.is_interrupt = func.is_interrupt;

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
        };
        let env_val = ir_func.new_value(env_meta);
        ir_func.params.push((env_val, IrType::Ptr(Box::new(IrType::U8))));
        ir_func.bind_local("__env", env_val);

        // Create parameter values
        for param in &func.params {
            let raw_ty = self.lower_type(&param.ty);
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
        self.defer_stack.clear();

        // @audit annotation: emit log calls at function entry/exit.
        // Entry: print "[AUDIT] enter <name>\n"
        // Exit:  print "[AUDIT] exit <name>\n" (via defer, runs before every return)
        let is_audited = func.annotations.iter().any(|a| a.name == "audit");
        if is_audited {
            // Entry log
            let entry_msg = self.emit_inst(Inst::ConstStr {
                value: format!("[AUDIT] enter {}\n", func.name),
            });
            self.emit_inst(Inst::Print {
                value: entry_msg,
                ty: IrType::Ptr(Box::new(IrType::U8)),
            });
            // Exit log via defer — a synthetic print statement that runs
            // before every return point (including implicit returns).
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
        for stmt in &func.body {
            if let Stmt::Expr(expr, _) = stmt {
                // Lower the expression and remember its value
                last_expr_val = Some(self.lower_expr(expr));
            } else {
                last_expr_val = None;
                self.lower_stmt(stmt);
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
                        let zero_val = if let Some(ref f) = self.current_func {
                            let ret_ty = f.ret_ty.clone();
                            let meta = ValueMeta {
                                ty: ret_ty.clone(),
                                region: self.current_region(),
                                effects: EffectSet::none(),
                                source_line: None,
                            };
                            let zid = {
                                let fmut = self.current_func.as_mut().unwrap();
                                let id = fmut.new_value(meta);
                                let inst = match &ret_ty {
                                    IrType::F32 | IrType::F64 => Inst::ConstFloat { bits: 0, ty: ret_ty.clone() },
                                    IrType::Bool => Inst::ConstBool { value: false },
                                    _ => Inst::ConstInt { value: 0, ty: ret_ty.clone() },
                                };
                                fmut.instructions.insert(id, inst);
                                let block_idx = self.current_block.0 as usize;
                                fmut.blocks[block_idx].instructions.push(id);
                                id
                            };
                            Some(zid)
                        } else {
                            None
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
            Type::Array { elem, size } => {
                Type::Array { elem: Box::new(self.resolve_type(elem)), size: *size }
            }
            Type::ArrayConst { elem, size_param } => {
                Type::ArrayConst { elem: Box::new(self.resolve_type(elem)), size_param: size_param.clone() }
            }
            other => other.clone(),
        }
    }

    /// Lower a Fides AST type to an IR type, resolving aliases first
    fn lower_type(&self, ty: &Type) -> IrType {
        let resolved = self.resolve_type(ty);
        crate::ir::lower_type(&resolved)
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
        for stmt in stmts {
            self.lower_stmt(stmt);
        }
    }

    /// Lower a single statement
    fn lower_stmt(&mut self, stmt: &Stmt) {
        // Set current line for debug info
        self.current_line = Some(stmt.line());
        match stmt {
            Stmt::Let { name, ty, expr, is_mut: _, is_persist, line: _ } => {
                let val = self.lower_expr(expr);
                let ir_ty = ty.as_ref().map(|t| self.lower_type(t)).unwrap_or_else(|| {
                    self.get_value_type(val)
                });
                let alloca = self.emit_inst(Inst::Alloca {
                    ty: ir_ty.clone(),
                    region: self.current_region(),
                });
                self.alloca_types.insert(alloca, ir_ty.clone());
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
                    });
                    if let Some(ref mut f) = self.current_func {
                        f.persist_vars.push((name.clone(), alloca));
                    }
                } else {
                    self.emit_inst(Inst::Store {
                        ptr: alloca,
                        value: val,
                    });
                }
                self.allocas.insert(name.clone(), alloca);
                if let Some(ref mut f) = self.current_func {
                    f.bind_local(name, alloca);
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
                    // For non-call expressions, try to get types from the value
                    vec![IrType::I64; n]
                };

                // For each name, extract the corresponding field
                for (i, name) in names.iter().enumerate() {
                    let field_ty = elem_tys.get(i).cloned().unwrap_or(IrType::I64);
                    let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: tuple_ptr,
                        field_index: i as u32,
                        field_ty: field_ty.clone(),
                    });
                    // Load from the tuple field. For struct fields, use Ptr type so
                    // the interpreter returns Value::Ptr. For scalars, use the field type.
                    let load_ty = if matches!(field_ty, IrType::Struct(_)) {
                        IrType::Ptr(Box::new(IrType::I64))
                    } else {
                        field_ty.clone()
                    };
                    let val = self.emit_inst(Inst::Load {
                        ptr: field_ptr,
                        ty: load_ty.clone(),
                    });
                    // For struct fields, the alloca holds a pointer to the struct.
                    // Use Ptr(Struct(...)) as the alloca type so that variable loads
                    // return the pointer correctly.
                    let alloca_ty = if matches!(field_ty, IrType::Struct(_)) {
                        IrType::Ptr(Box::new(field_ty.clone()))
                    } else {
                        field_ty.clone()
                    };
                    let alloca = self.emit_inst(Inst::Alloca {
                        ty: alloca_ty.clone(),
                        region: self.current_region(),
                    });
                    self.alloca_types.insert(alloca, alloca_ty.clone());
                    self.emit_inst(Inst::Store {
                        ptr: alloca,
                        value: val,
                    });
                    self.allocas.insert(name.clone(), alloca);
                    if let Some(ref mut f) = self.current_func {
                        f.bind_local(name, alloca);
                    }
                }
            }

            Stmt::Assign { target, op, expr, line: _ } => {
                // Check if target is a mutable variable with an alloca
                let alloca = self.allocas.get(target).copied();
                
                if let Some(ptr) = alloca {
                    // Mutable variable: use load/store through alloca
                    match op {
                        AssignOp::Assign => {
                            let val = self.lower_expr(expr);
                            self.emit_inst(Inst::Store { ptr, value: val });
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
                            self.emit_inst(Inst::Store { ptr, value: new_val });
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

                // Vec indexing — call runtime p_vec_set / p_vec_get
                if arr_ty.is_opaque_named("Vec") {
                    let elem_ty = arr_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                    if *op == AssignOp::Assign {
                        let val = self.lower_expr(expr);
                        self.emit_inst(Inst::Call {
                            func: "p_vec_set".to_string(),
                            args: vec![arr_val, idx_val, val],
                            ret_ty: IrType::Void,
                            effects: EffectSet::new(),
                        });
                    } else {
                        // Compound assignment: p_vec_get, binop, p_vec_set
                        let current = self.emit_inst(Inst::Call {
                            func: "p_vec_get".to_string(),
                            args: vec![arr_val, idx_val],
                            ret_ty: elem_ty.clone(),
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
                        self.emit_inst(Inst::Call {
                            func: "p_vec_set".to_string(),
                            args: vec![arr_val, idx_val, new_val],
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
                    self.emit_inst(Inst::Store { ptr: elem_ptr, value: val });
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
                    self.emit_inst(Inst::Store { ptr: elem_ptr, value: new_val });
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
                    self.emit_inst(Inst::Store { ptr: ptr_val, value: val });
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
                        ty: pointee_ty,
                    });
                    self.emit_inst(Inst::Store { ptr: ptr_val, value: new_val });
                }
            }

            Stmt::Expr(expr, _) => {
                self.lower_expr(expr);
            }

            Stmt::Return(expr, _) => {
                // Emit defer blocks before returning (in reverse order)
                self.emit_defers();

                // Save persist variables before returning
                self.emit_persist_saves();

                if self.current_sret.is_some() {
                    // Struct-returning function: copy struct data to sret, return void
                    if let Some(e) = expr {
                        let val = self.lower_expr(e);
                        self.emit_sret_copy(val);
                    }
                    self.emit_terminator(Inst::Return { value: None });
                } else {
                    let ret_val = match expr {
                        Some(e) => Some(self.lower_expr(e)),
                        None => None,
                    };
                    self.emit_terminator(Inst::Return { value: ret_val });
                }
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
                            self.emit_inst(Inst::Store { ptr: slot, value: val });
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
                // Look up the handler for this effect.op
                let handler_info = self.effect_handler_stack.iter().rev().find_map(|handlers| {
                    handlers.iter().find(|h| h.effect == *effect && h.op == *op)
                        .map(|h| (h.params.clone(), h.body.clone()))
                });

                if let Some((params, body)) = handler_info {
                    // Lower the handler body with the args bound to params
                    for (i, arg) in args.iter().enumerate() {
                        if i < params.len() {
                            let arg_val = self.lower_expr(arg);
                            let slot = self.emit_inst(Inst::Alloca {
                                ty: IrType::I64,
                                region: self.current_region(),
                            });
                            self.emit_inst(Inst::Store { ptr: slot, value: arg_val });
                            self.allocas.insert(params[i].clone(), slot);
                            self.alloca_types.insert(slot, IrType::I64);
                        }
                    }
                    self.lower_stmts(&body);
                } else {
                    // No handler found — just lower the args (no-op)
                    for arg in args {
                        self.lower_expr(arg);
                    }
                }
            }
            Stmt::RegionBlock { name, budget: _, body, .. } => {
                // Emit RegionBegin — marks the start of a named region.
                // Allocations inside the block will use Region::Named(name).
                self.emit_inst(Inst::RegionBegin {
                    name: name.clone(),
                });
                self.region_stack.push(name.clone());
                for s in body {
                    self.lower_stmt(s);
                }
                self.region_stack.pop();
                // Emit RegionEnd — frees all memory allocated in this region.
                self.emit_inst(Inst::RegionEnd {
                    name: name.clone(),
                });
            }
            Stmt::ComptimeGen { .. } => {}
            Stmt::Sandbox { body, .. } => {
                // Sandbox is transparent at the IR level — safety is
                // enforced entirely at compile time by the typechecker.
                // Just lower the body as normal statements.
                for s in body {
                    self.lower_stmt(s);
                }
            }
            Stmt::Unsafe { body, .. } => {
                // Unsafe is transparent at the IR level — safety is
                // enforced entirely at compile time by the typechecker.
                // Just lower the body as normal statements.
                for s in body {
                    self.lower_stmt(s);
                }
            }
            Stmt::Txn { body, .. } => {
                // Transaction block — lower body as normal statements.
                // In the interpreter, txn blocks get snapshot/rollback semantics
                // via a special marker. In native code, transactions are handled
                // at the application level (database transactions, etc.).
                // For now, txn is transparent — just lower the body.
                for s in body {
                    self.lower_stmt(s);
                }
            }
            Stmt::GradBlock { body, .. } => {
                // The autograd transform happens later; lower the body as normal statements.
                for s in body {
                    self.lower_stmt(s);
                }
            }
            Stmt::Backward { .. } => {
                // Marker for the autograd pass — emit nothing here.
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

        // Lower then block
        self.set_current_block(then_block_id);
        self.lower_stmts(then_block);
        // If then block doesn't end with terminator, jump to merge
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: merge_block_id });
        }

        // Lower else block
        self.set_current_block(else_block_id);
        if let Some(else_stmts) = else_block {
            self.lower_stmts(else_stmts);
        }
        if !self.current_block_has_terminator() {
            self.emit_terminator(Inst::Jump { target: merge_block_id });
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
                self.emit_inst(Inst::Store { ptr: var_alloca, value: elem_val });

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
                    self.emit_inst(Inst::Store { ptr: idx_alloca, value: inc });
                    self.emit_terminator(Inst::Jump { target: cond_block });
                }

                self.set_current_block(exit_block);
            }
        }
    }

    /// Lower `for x in vec { body }` — iterates Vec via p_vec_len/p_vec_get
    fn lower_for_vec(&mut self, var: &str, vec_val: ValueId, body: &[Stmt]) {
        // Create alloca for loop variable — use Opaque so Load checks side-table
        let var_alloca = self.emit_inst(Inst::Alloca {
            ty: IrType::Opaque("any".to_string()),
            region: self.current_region(),
        });
        self.alloca_types.insert(var_alloca, IrType::Opaque("any".to_string()));
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
        });

        let cond_block = self.new_block();
        let body_block = self.new_block();
        let exit_block = self.new_block();

        self.emit_terminator(Inst::Jump { target: cond_block });

        // Condition: i < vec.len()
        self.set_current_block(cond_block);
        let idx_val = self.emit_inst(Inst::Load { ptr: idx_alloca, ty: IrType::I64 });
        let len_val = self.emit_inst(Inst::Call {
            func: "p_vec_len".to_string(),
            args: vec![vec_val],
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
        let elem_val = self.emit_inst(Inst::Call {
            func: "p_vec_get".to_string(),
            args: vec![vec_val, cur_idx],
            ret_ty: IrType::Opaque("any".to_string()),
            effects: EffectSet::new(),
        });
        self.emit_inst(Inst::Store { ptr: var_alloca, value: elem_val });

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
            self.emit_inst(Inst::Store { ptr: idx_alloca, value: inc });
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
                    // Bind the match value to this identifier
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
                    for (i, (binding, _ty_name)) in bindings.iter().enumerate() {
                        let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                            struct_ptr: match_val,
                            field_index: (i + 1) as u32,
                            field_ty: IrType::I64,
                        });
                        let field_val = self.emit_inst(Inst::Load { ptr: field_ptr, ty: IrType::I64 });
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(binding, field_val);
                        }
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
                    self.set_current_block(next_arm_block);
                    continue;
                }
            }

            // Lower match arm body
            self.set_current_block(match_block);

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
                    for (i, (binding, _ty_name)) in bindings.iter().enumerate() {
                        let field_ptr = self.emit_inst(Inst::GetFieldPtr { struct_ptr: match_val, field_index: (i + 1) as u32, field_ty: IrType::I64 });
                        let field_val = self.emit_inst(Inst::Load { ptr: field_ptr, ty: IrType::I64 });
                        if let Some(ref mut f) = self.current_func {
                            f.bind_local(binding, field_val);
                        }
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
                        self.emit_inst(Inst::Store { ptr: result_slot, value: v });
                    }
                    if !self.current_block_has_terminator() {
                        self.emit_terminator(Inst::Jump { target: merge_block });
                    }
                    self.set_current_block(next_arm_block);
                    continue;
                }
            }

            // Lower match arm body and store result
            self.set_current_block(match_block);

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
                self.emit_inst(Inst::Store { ptr: result_slot, value: v });
            }
            if !self.current_block_has_terminator() {
                self.emit_terminator(Inst::Jump { target: merge_block });
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
                // Check if it's a known function name — emit as env struct with fn addr
                if self.module.functions.contains_key(name) || self.module.extern_functions.contains_key(name) {
                    return self.make_fn_env(name, &[]);
                }
                // Unknown identifier — emit zero as fallback
                self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 })
            }

            Expr::Binary { op, left, right } => {
                let left_val = self.lower_expr(left);
                let right_val = self.lower_expr(right);
                let result_ty = self.infer_binop_type(op, left, right);
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
                    let ret_ty = IrType::I64; // Default return type for indirect calls
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
                    // Prepend dummy env_ptr for user functions (not externs or builtins)
                    if !self.module.extern_functions.contains_key(callee) && callee != "print" {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        all_args.push(dummy_env);
                    }
                    all_args.extend(arg_vals);

                    // Determine effects
                    let mut effects = EffectSet::new();
                    if callee.starts_with("__pledge") {
                        effects.add(Effect::IO);
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
                    } else {
                        // User function — look up effects
                        if let Some(e) = self.module.function_effects.get(callee) {
                            effects.add_all(e);
                        }
                    }

                    // Prepend dummy env_ptr for user functions (not externs or builtins)
                    let mut all_args = arg_vals;
                    if !self.module.extern_functions.contains_key(callee) && callee != "print" {
                        let dummy_env = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
                        all_args.insert(0, dummy_env);
                    }

                    self.emit_inst(Inst::Call {
                        func: callee.clone(),
                        args: all_args,
                        ret_ty,
                        effects,
                    })
                }
            }

            Expr::MethodCall { receiver, method, args } => {
                // Check if this is Box::new(x) — built-in heap allocation
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Box" && method == "new" {
                        // Box::new(x) = malloc(sizeof(x)), store x, return ptr
                        let val = self.lower_expr(&args[0]);
                        let val_ty = self.get_value_type(val);
                        let size = self.module.type_size(&val_ty);
                        let size_val = self.emit_inst(Inst::ConstInt { value: size as i64, ty: IrType::I64 });
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
                        // Store the value at the heap address
                        self.emit_inst(Inst::Store { ptr, value: val });
                        return ptr;
                    }
                }
                // Check if this is Vec::new() — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "Vec" && method == "new" {
                        // Infer element type from the typechecker's result
                        let elem_ty = self.infer_vec_elem_type();
                        return self.emit_inst(Inst::Call {
                            func: "p_vec_new".to_string(),
                            args: vec![],
                            ret_ty: IrType::Generic("Vec".to_string(), vec![elem_ty]),
                            effects: EffectSet::new(),
                        });
                    }
                }
                // Check if this is HashMap::new() — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "HashMap" && method == "new" {
                        let (key_ty, val_ty) = self.infer_map_types();
                        return self.emit_inst(Inst::Call {
                            func: "p_map_new".to_string(),
                            args: vec![],
                            ret_ty: IrType::Generic("HashMap".to_string(), vec![key_ty, val_ty]),
                            effects: EffectSet::new(),
                        });
                    }
                }
                // Check if this is String::from(x) — built-in
                if let Expr::Ident(name) = receiver.as_ref() {
                    if name == "String" && method == "from" {
                        let arg = self.lower_expr(&args[0]);
                        return self.emit_inst(Inst::Call {
                            func: "p_string_from".to_string(),
                            args: vec![arg],
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
                let recv_val = self.lower_expr(receiver);

                // Builtin collection method dispatch
                let recv_ty = self.get_value_type(recv_val);
                let is_builtin_collection = recv_ty.is_opaque_named("Vec")
                    || recv_ty.is_opaque_named("HashMap")
                    || recv_ty.is_opaque_named("String");
                if is_builtin_collection {
                    let mut runtime_args = vec![recv_val];
                    for a in args {
                        runtime_args.push(self.lower_expr(a));
                    }
                    let (func_name, ret_ty) = if recv_ty.is_opaque_named("Vec") {
                        // Element type from Generic args, default to I64
                        let elem_ty = recv_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                        let rt = match method.as_str() {
                            "push" => IrType::Void,
                            "pop" => elem_ty.clone(),
                            "len" => IrType::I64,
                            "is_empty" => IrType::Bool,
                            "get" => elem_ty.clone(),
                            "set" => IrType::Void,
                            "clear" => IrType::Void,
                            "capacity" => IrType::I64,
                            "insert" => IrType::Void,
                            "remove" => elem_ty.clone(),
                            "contains" => IrType::Bool,
                            "reverse" => IrType::Void,
                            "first" => elem_ty.clone(),
                            "last" => elem_ty.clone(),
                            "extend" => IrType::Void,
                            "truncate" => IrType::Void,
                            "sort" => IrType::Void,
                            "clone" => recv_ty.clone(),
                            _ => IrType::I64,
                        };
                        (format!("p_vec_{}", method), rt)
                    } else if recv_ty.is_opaque_named("HashMap") {
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
                        (format!("p_map_{}", method), rt)
                    } else if recv_ty.is_opaque_named("String") {
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
                            _ => IrType::I64,
                        };
                        (format!("p_string_{}", method), rt)
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
                        let ret_ty = IrType::I64;
                        let effects = EffectSet::new();
                        return self.emit_inst(Inst::DynCall {
                            trait_name,
                            method: method.clone(),
                            args: all_args,
                            ret_ty,
                            effects,
                        });
                    }
                }

                let struct_name = match &recv_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
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
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // None: store 0 to result
                self.set_current_block(none_block);
                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: zero_val,
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

                // Vec indexing — call runtime p_vec_get with correct element type
                if arr_ty.is_opaque_named("Vec") {
                    let elem_ty = arr_ty.generic_elem().cloned().unwrap_or(IrType::I64);
                    return self.emit_inst(Inst::Call {
                        func: "p_vec_get".to_string(),
                        args: vec![arr_val, idx_val],
                        ret_ty: elem_ty,
                        effects: EffectSet::new(),
                    });
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
                    self.emit_inst(Inst::Store { ptr: result_slot, value: tv });
                }
                if !self.current_block_has_terminator() {
                    self.emit_terminator(Inst::Jump { target: merge_bb });
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
                    self.emit_inst(Inst::Store { ptr: result_slot, value: ev });
                }
                if !self.current_block_has_terminator() {
                    self.emit_terminator(Inst::Jump { target: merge_bb });
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
                                    self.emit_inst(Inst::Call {
                                        func: "p_vec_to_str".to_string(),
                                        args: vec![val],
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
                self.emit_inst(Inst::Store { ptr: tag_ptr, value: tag_val });

                // Store data fields starting at field 1
                for (i, arg_expr) in args.iter().enumerate() {
                    let arg_val = self.lower_expr(arg_expr);
                    let arg_ty = self.get_value_type(arg_val);
                    let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: ptr,
                        field_index: (i + 1) as u32,
                        field_ty: arg_ty.clone(),
                    });
                    self.emit_inst(Inst::Store { ptr: field_ptr, value: arg_val });
                }

                ptr
            }

            Expr::Ref { expr, is_mut: _ } => {
                // If taking a reference to a variable that has an alloca,
                // return the alloca pointer directly (no copy).
                // This ensures &mut v actually points to v's storage.
                if let Expr::Ident(name) = expr.as_ref() {
                    if let Some(alloca) = self.allocas.get(name) {
                        return *alloca;
                    }
                }
                // Otherwise: alloca, store, return pointer
                let val = self.lower_expr(expr);
                let val_ty = self.get_value_type(val);
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: val_ty.clone(),
                    region: self.current_region(),
                });
                self.emit_inst(Inst::Store {
                    ptr,
                    value: val,
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
                    self.emit_inst(Inst::Store { ptr: dst_tag_ptr, value: src_tag });
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
                    self.emit_inst(Inst::Store { ptr: dst_val_ptr, value: src_val });
                    self.emit_terminator(Inst::Jump { target: try_err_block });
                } else {
                    // Not inside a try block: return the error from the current function
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
                });
                self.emit_terminator(Inst::Jump { target: merge_block });

                // None: evaluate the default and store to result slot
                self.set_current_block(none_block);
                let default_val = self.lower_expr(default);
                self.emit_inst(Inst::Store {
                    ptr: result_slot,
                    value: default_val,
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
                self.emit_inst(Inst::Store { ptr: tag_ptr, value: ok_tag });
                let val_ptr = self.emit_inst(Inst::GetFieldPtr {
                    struct_ptr: result_slot,
                    field_index: 1,
                    field_ty: IrType::I64,
                });
                self.emit_inst(Inst::Store { ptr: val_ptr, value: ok_val });
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
                self.emit_inst(Inst::Store { ptr: out_tag_ptr, value: err_tag });
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
                self.emit_inst(Inst::Store { ptr: out_val_ptr, value: err_val });
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
                // Register or update the tuple struct layout with actual element types
                // Always update — different tuples with same arity may have different element types
                let fields: Vec<(String, IrType, u32)> = elem_tys.iter().enumerate()
                    .map(|(i, ty)| (format!("_{}", i), ty.clone(), (i * 8) as u32))
                    .collect();
                let size = (tuple_size * 8) as u64;
                self.module.add_struct(StructLayout {
                    name: tuple_name.clone(),
                    fields,
                    size,
                    align: 8,
                });
                let ptr = self.emit_inst(Inst::Alloca {
                    ty: tuple_ty.clone(),
                    region: self.current_region(),
                });
                for (i, val) in elem_vals.iter().enumerate() {
                    let val_ty = &elem_tys[i];
                    let field_ptr = self.emit_inst(Inst::GetFieldPtr {
                        struct_ptr: ptr,
                        field_index: i as u32,
                        field_ty: val_ty.clone(),
                    });
                    // Store the value — for structs, this is already a pointer
                    // (structs are passed by pointer in Fides). Don't do struct copy
                    // into tuple fields — just store the pointer.
                    self.emit_inst(Inst::Store { ptr: field_ptr, value: *val });
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

                let closure_func = Func {
                    name: closure_name.clone(),
                    type_params: vec![],
                    type_param_bounds: vec![],
                    const_params: vec![],
                    params: all_params,
                    ret_ty: ret_ty.clone().unwrap_or(Type::I64),
                    body: body.clone(),
                    is_pub: false,
                    is_comptime: false,
                    declared_effects: Vec::new(),
                    has_effect_annotation: false,
                    is_interrupt: false,
            annotations: Vec::new(),
                };

                // Save current function context, lower the closure, restore context
                let saved_func = self.current_func.take();
                self.lower_func(&closure_func);
                self.current_func = saved_func;
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
        }
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
    /// Since the typechecker already resolved the type, we default to I64
    /// (the most common element type) but could be extended to use type
    /// annotations from the AST.
    fn infer_vec_elem_type(&self) -> IrType {
        // Default to I64 — the typechecker hardcodes Vec<i64> for Vec::new()
        // Future: infer from type annotations or first push() call
        IrType::I64
    }

    /// Infer key and value types for HashMap::new() based on the current context.
    /// Returns (key_type, value_type).
    fn infer_map_types(&self) -> (IrType, IrType) {
        // Default to (String, I64) — the typechecker hardcodes HashMap<String, i64>
        // Future: infer from type annotations or first insert() call
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

        // Store function address at field 0
        let fn_name_val = self.emit_inst(Inst::ConstStr { value: fn_name.to_string() });
        let zero_idx = self.emit_inst(Inst::ConstInt { value: 0, ty: IrType::I64 });
        let field0 = self.emit_inst(Inst::GetElemPtr {
            array_ptr: env_ptr,
            index: zero_idx,
            elem_ty: IrType::I64,
        });
        self.emit_inst(Inst::Store { ptr: field0, value: fn_name_val });

        // Store captures at fields 1..N
        for (i, (_, cap_val)) in captures.iter().enumerate() {
            let idx = self.emit_inst(Inst::ConstInt { value: (i + 1) as i64, ty: IrType::I64 });
            let field = self.emit_inst(Inst::GetElemPtr {
                array_ptr: env_ptr,
                index: idx,
                elem_ty: IrType::I64,
            });
            self.emit_inst(Inst::Store { ptr: field, value: *cap_val });
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

    fn infer_binop_type(&self, op: &BinOp, left: &Expr, right: &Expr) -> IrType {
        match op {
            BinOp::Eq | BinOp::Neq | BinOp::Lt | BinOp::Gt | BinOp::LtEq | BinOp::GtEq
            | BinOp::And | BinOp::Or => IrType::Bool,
            _ => {
                let lt = self.infer_expr_type(left);
                let rt = self.infer_expr_type(right);
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
                // Check local binding — type is the value's type
                if let Some(val) = self.current_func.as_ref().and_then(|f| f.lookup_local(name)) {
                    if let Some(meta) = self.current_func.as_ref().and_then(|f| f.values.get(&val)) {
                        return meta.ty.clone();
                    }
                }
                // Check constant
                if let Some(const_expr) = self.constants.get(name) {
                    return self.infer_expr_type(const_expr);
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
                let struct_name = match &recv_ty {
                    IrType::Ptr(inner) => match inner.as_ref() {
                        IrType::Struct(name) => Some(name.clone()),
                        _ => None,
                    },
                    IrType::Struct(name) => Some(name.clone()),
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
                    ReflectOp::FieldCount => IrType::I64,
                    ReflectOp::FieldNames | ReflectOp::FieldTypes | ReflectOp::EnumVariants => {
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
        }
    }

    /// Emit all deferred blocks in reverse order (LIFO — last registered runs first)
    fn emit_defers(&mut self) {
        let defers: Vec<Vec<Stmt>> = self.defer_stack.drain(..).rev().collect();
        for body in defers {
            self.lower_stmts(&body);
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
            let loaded = self.emit_inst(Inst::Load { ptr: alloca, ty: IrType::I64 });
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
