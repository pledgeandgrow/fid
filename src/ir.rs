// Pledge IR — Typed SSA with effect and region annotations
//
// Design principles:
// - Every value has a type and a unique ID (SSA form)
// - Every instruction tracks its effects (alloc, io, divergence)
// - Memory is organized into regions (stack, heap, static)
// - IR is serializable for incremental compilation and hot reload
// - IR is interpretable for comptime and debugging
//
// This IR is the heart of Pledge — it's what makes the language unique:
// no other language has effect-aware, region-annotated SSA IR that's
// also interpretable and serializable.

use crate::ast::{BinOp, UnaryOp, Type as AstType};
use std::collections::HashMap;
use std::fmt;

// ─── IR Types ───────────────────────────────────────────────────────────

/// IR-level types — simpler than AST types, lowered to primitives + pointers
#[derive(Debug, Clone, PartialEq)]
pub enum IrType {
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Ptr(Box<IrType>),       // raw pointer to type
    Struct(String),         // struct name (layout in module)
    Array(Box<IrType>, usize),  // [T; N]
    Void,
    /// Never type — for instructions that don't produce a value (panic, unreachable)
    Never,
    /// Opaque type — used for types not yet lowered (strings, generics, etc.)
    /// The string is the original type name for debugging
    Opaque(String),
    /// Generic type — carries type parameters: Vec<i64>, Option<T>, Result<T,E>
    Generic(String, Vec<IrType>),
}

impl IrType {
    /// Size in bytes for this type on a 64-bit target
    pub fn size(&self) -> u64 {
        match self {
            IrType::I8 | IrType::U8 | IrType::Bool => 1,
            IrType::I16 | IrType::U16 => 2,
            IrType::I32 | IrType::U32 | IrType::F32 => 4,
            IrType::I64 | IrType::U64 | IrType::F64 | IrType::Ptr(_) => 8,
            IrType::Struct(name) => {
                // Will be looked up from module; return 0 as placeholder
                // Real size computed during lowering when struct layouts are known
                let _ = name;
                0
            }
            IrType::Array(elem, count) => elem.size() * (*count as u64),
            IrType::Void => 0,
            IrType::Never => 0,
            IrType::Opaque(_) => 8, // pointer-sized by default
            IrType::Generic(_, _) => 8, // pointer-sized (opaque handle)
        }
    }

    /// Alignment in bytes for this type on a 64-bit target
    pub fn align(&self) -> u64 {
        match self {
            IrType::I8 | IrType::U8 | IrType::Bool => 1,
            IrType::I16 | IrType::U16 => 2,
            IrType::I32 | IrType::U32 | IrType::F32 => 4,
            IrType::I64 | IrType::U64 | IrType::F64 | IrType::Ptr(_) => 8,
            IrType::Struct(_) => 8, // structs align to largest field (simplified)
            IrType::Array(elem, _) => elem.align(),
            IrType::Void => 1,
            IrType::Never => 1,
            IrType::Opaque(_) => 8,
            IrType::Generic(_, _) => 8,
        }
    }

    /// Is this an integer type?
    pub fn is_int(&self) -> bool {
        matches!(self,
            IrType::I8 | IrType::I16 | IrType::I32 | IrType::I64
            | IrType::U8 | IrType::U16 | IrType::U32 | IrType::U64
            | IrType::Bool
        )
    }

    /// Is this a floating-point type?
    pub fn is_float(&self) -> bool {
        matches!(self, IrType::F32 | IrType::F64)
    }

    /// Is this a pointer type?
    pub fn is_ptr(&self) -> bool {
        matches!(self, IrType::Ptr(_))
    }

    /// Check if this type is a named opaque or generic with the given name
    /// e.g. `is_opaque_named("Vec")` matches both `Opaque("Vec")` and `Generic("Vec", _)`
    pub fn is_opaque_named(&self, name: &str) -> bool {
        match self {
            IrType::Opaque(n) => n == name,
            IrType::Generic(n, _) => n == name,
            _ => false,
        }
    }

    /// Get the element type of a Generic or Opaque type
    /// For `Generic("Vec", [I64])` returns `Some(I64)`
    /// For `Opaque("Vec")` returns `None` (element type unknown)
    pub fn generic_elem(&self) -> Option<&IrType> {
        match self {
            IrType::Generic(_, args) => args.first(),
            _ => None,
        }
    }
}

impl fmt::Display for IrType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            IrType::I8 => write!(f, "i8"),
            IrType::I16 => write!(f, "i16"),
            IrType::I32 => write!(f, "i32"),
            IrType::I64 => write!(f, "i64"),
            IrType::U8 => write!(f, "u8"),
            IrType::U16 => write!(f, "u16"),
            IrType::U32 => write!(f, "u32"),
            IrType::U64 => write!(f, "u64"),
            IrType::F32 => write!(f, "f32"),
            IrType::F64 => write!(f, "f64"),
            IrType::Bool => write!(f, "bool"),
            IrType::Ptr(inner) => write!(f, "*{}", inner),
            IrType::Struct(name) => write!(f, "struct.{}", name),
            IrType::Array(elem, count) => write!(f, "[{}; {}]", elem, count),
            IrType::Void => write!(f, "void"),
            IrType::Never => write!(f, "never"),
            IrType::Opaque(name) => write!(f, "opaque.{}", name),
            IrType::Generic(name, args) => {
                write!(f, "generic.{}<", name)?;
                for (i, a) in args.iter().enumerate() {
                    if i > 0 { write!(f, ", ")?; }
                    write!(f, "{}", a)?;
                }
                write!(f, ">")
            }
        }
    }
}

// ─── Effects ────────────────────────────────────────────────────────────

/// Effects that an instruction or function can have.
/// This is Pledge's key differentiator — effects are tracked at the IR level.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Effect {
    /// Allocates memory (heap or stack)
    Alloc,
    /// Performs I/O (print, file, network)
    IO,
    /// May not return (infinite loop, panic, abort)
    Diverge,
    /// Reads from memory
    Read,
    /// Writes to memory
    Write,
}

impl fmt::Display for Effect {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Effect::Alloc => write!(f, "alloc"),
            Effect::IO => write!(f, "io"),
            Effect::Diverge => write!(f, "diverge"),
            Effect::Read => write!(f, "read"),
            Effect::Write => write!(f, "write"),
        }
    }
}

/// A set of effects — used to annotate instructions and functions
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct EffectSet {
    effects: Vec<Effect>,
}

impl EffectSet {
    pub fn new() -> Self {
        EffectSet { effects: Vec::new() }
    }

    pub fn none() -> Self {
        EffectSet { effects: Vec::new() }
    }

    pub fn add(&mut self, effect: Effect) {
        if !self.effects.contains(&effect) {
            self.effects.push(effect);
        }
    }

    pub fn add_all(&mut self, other: &EffectSet) {
        for e in &other.effects {
            self.add(*e);
        }
    }

    pub fn contains(&self, effect: Effect) -> bool {
        self.effects.contains(&effect)
    }

    pub fn is_pure(&self) -> bool {
        self.effects.is_empty()
    }

    pub fn iter(&self) -> impl Iterator<Item = &Effect> {
        self.effects.iter()
    }
}

impl fmt::Display for EffectSet {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if self.effects.is_empty() {
            return write!(f, "pure");
        }
        let parts: Vec<String> = self.effects.iter().map(|e| e.to_string()).collect();
        write!(f, "!{}", parts.join(","))
    }
}

// ─── Regions ────────────────────────────────────────────────────────────

/// Memory regions — Pledge's approach to memory safety without lifetimes.
/// Each allocation belongs to a region, and the IR tracks which region.
/// The type checker proves that references don't outlive their region.
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum Region {
    /// Stack-allocated — lives for the duration of the function frame
    Stack,
    /// Heap-allocated — lives until explicitly freed or GC'd
    Heap,
    /// Static/global — lives for the entire program
    Static,
    /// A named region — for region-based memory management
    /// (future: explicit region blocks with `region { ... }`)
    Named(String),
}

impl fmt::Display for Region {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Region::Stack => write!(f, "stack"),
            Region::Heap => write!(f, "heap"),
            Region::Static => write!(f, "static"),
            Region::Named(name) => write!(f, "region.{}", name),
        }
    }
}

// ─── SSA Values ─────────────────────────────────────────────────────────

/// A unique SSA value identifier.
/// Values are numbered per-function, starting from 0.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct ValueId(pub u32);

impl fmt::Display for ValueId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "%{}", self.0)
    }
}

/// A basic block identifier.
/// Blocks are numbered per-function, starting from 0.
/// Block 0 is always the entry block.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct BlockId(pub u32);

impl fmt::Display for BlockId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "bb{}", self.0)
    }
}

/// Metadata attached to a value — its type, region, and effects
#[derive(Debug, Clone)]
pub struct ValueMeta {
    pub ty: IrType,
    pub region: Region,
    pub effects: EffectSet,
    /// Source location for debugging (line number in source)
    pub source_line: Option<usize>,
}

// ─── Instructions ───────────────────────────────────────────────────────

/// IR instructions — the core of Pledge IR.
/// Every instruction produces a value (even if it's void).
/// Instructions are organized into basic blocks.
#[derive(Debug, Clone)]
pub enum Inst {
    // ─── Constants ───
    /// Integer constant
    ConstInt {
        value: i64,
        ty: IrType,
    },
    /// Floating-point constant (stored as bits for f32/f64)
    ConstFloat {
        bits: u64,
        ty: IrType,
    },
    /// Boolean constant
    ConstBool {
        value: bool,
    },
    /// String constant — returns a pointer to static string data
    ConstStr {
        value: String,
    },
    /// Null pointer
    ConstNull {
        ty: IrType,
    },

    // ─── Arithmetic ───
    /// Binary operation on two values
    BinOp {
        op: BinOp,
        left: ValueId,
        right: ValueId,
        ty: IrType,
    },
    /// Unary operation on a value
    UnaryOp {
        op: UnaryOp,
        operand: ValueId,
        ty: IrType,
    },

    // ─── Memory ───
    /// Allocate space on the stack
    Alloca {
        ty: IrType,
        region: Region,
    },
    /// Load a value from a pointer
    Load {
        ptr: ValueId,
        ty: IrType,
    },
    /// Store a value to a pointer
    Store {
        ptr: ValueId,
        value: ValueId,
    },
    /// Get the address of a struct field
    GetFieldPtr {
        struct_ptr: ValueId,
        field_index: u32,
        field_ty: IrType,
    },
    /// Get the address of an array element
    GetElemPtr {
        array_ptr: ValueId,
        index: ValueId,
        elem_ty: IrType,
    },

    // ─── Control Flow ───
    /// Unconditional jump
    Jump {
        target: BlockId,
    },
    /// Conditional branch — jumps to `then` if cond is true, else `else_`
    Branch {
        cond: ValueId,
        then: BlockId,
        else_: BlockId,
    },
    /// Return from function with optional value
    Return {
        value: Option<ValueId>,
    },
    /// Unreachable — for code after return, diverge, etc.
    Unreachable,

    // ─── Function Calls ───
    /// Call a function by name
    Call {
        func: String,
        args: Vec<ValueId>,
        ret_ty: IrType,
        effects: EffectSet,
    },
    /// Call a function through a pointer (indirect call)
    /// fn_ptr is a ValueId holding an env struct pointer
    /// has_env indicates if the target function expects __env as first param
    CallPtr {
        fn_ptr: ValueId,
        args: Vec<ValueId>,
        ret_ty: IrType,
        effects: EffectSet,
        has_env: bool,
    },

    // ─── Conversions ───
    /// Cast between types of the same size (bitcast)
    BitCast {
        value: ValueId,
        from: IrType,
        to: IrType,
    },
    /// Extend integer to wider type
    IntExtend {
        value: ValueId,
        from: IrType,
        to: IrType,
    },
    /// Truncate integer to narrower type
    IntTrunc {
        value: ValueId,
        from: IrType,
        to: IrType,
    },
    /// Convert int to float
    IntToFloat {
        value: ValueId,
        from: IrType,
        to: IrType,
    },
    /// Convert float to int
    FloatToInt {
        value: ValueId,
        from: IrType,
        to: IrType,
    },

    // ─── Pledge-Specific ───
    /// Try operator — check if value is Ok/Some, branch accordingly
    /// If Ok: store unwrapped value to `ok_dst`, jump to `ok_block`
    /// If Err/None: store error to `err_dst` (if present), jump to `err_block`
    TryUnwrap {
        value: ValueId,
        ok_dst: ValueId,
        err_block: BlockId,
        ok_block: BlockId,
    },

    /// Print a value (runtime call, but tracked as IO effect)
    Print {
        value: ValueId,
        ty: IrType,
    },

    /// Panic — abort execution with message
    Panic {
        message: String,
    },
}

impl Inst {
    /// Get the type produced by this instruction
    pub fn result_type(&self) -> IrType {
        match self {
            Inst::ConstInt { ty, .. } => ty.clone(),
            Inst::ConstFloat { ty, .. } => ty.clone(),
            Inst::ConstBool { .. } => IrType::Bool,
            Inst::ConstStr { .. } => IrType::Ptr(Box::new(IrType::U8)),
            Inst::ConstNull { ty, .. } => ty.clone(),
            Inst::BinOp { ty, .. } => ty.clone(),
            Inst::UnaryOp { ty, .. } => ty.clone(),
            Inst::Alloca { ty, .. } => IrType::Ptr(Box::new(ty.clone())),
            Inst::Load { ty, .. } => ty.clone(),
            Inst::Store { .. } => IrType::Void,
            Inst::GetFieldPtr { field_ty, .. } => IrType::Ptr(Box::new(field_ty.clone())),
            Inst::GetElemPtr { elem_ty, .. } => IrType::Ptr(Box::new(elem_ty.clone())),
            Inst::Jump { .. } => IrType::Void,
            Inst::Branch { .. } => IrType::Void,
            Inst::Return { .. } => IrType::Void,
            Inst::Unreachable => IrType::Void,
            Inst::Call { ret_ty, .. } => ret_ty.clone(),
            Inst::CallPtr { ret_ty, .. } => ret_ty.clone(),
            Inst::BitCast { to, .. } => to.clone(),
            Inst::IntExtend { to, .. } => to.clone(),
            Inst::IntTrunc { to, .. } => to.clone(),
            Inst::IntToFloat { to, .. } => to.clone(),
            Inst::FloatToInt { to, .. } => to.clone(),
            Inst::TryUnwrap { .. } => IrType::Void,
            Inst::Print { .. } => IrType::Void,
            Inst::Panic { .. } => IrType::Never,
        }
    }

    /// Get the effects of this instruction
    pub fn effects(&self) -> EffectSet {
        let mut effects = EffectSet::new();
        match self {
            Inst::Alloca { .. } => effects.add(Effect::Alloc),
            Inst::Load { .. } => effects.add(Effect::Read),
            Inst::Store { .. } => effects.add(Effect::Write),
            Inst::Call { effects: e, .. } => effects.add_all(e),
            Inst::CallPtr { effects: e, .. } => effects.add_all(e),
            Inst::Print { .. } => {
                effects.add(Effect::IO);
                effects.add(Effect::Read);
            },
            Inst::Panic { .. } => effects.add(Effect::Diverge),
            Inst::TryUnwrap { .. } => effects.add(Effect::Read),
            Inst::Unreachable => effects.add(Effect::Diverge),
            _ => {}
        }
        effects
    }

    /// Is this a terminator instruction? (must be last in a block)
    pub fn is_terminator(&self) -> bool {
        matches!(self,
            Inst::Jump { .. }
            | Inst::Branch { .. }
            | Inst::Return { .. }
            | Inst::Unreachable
            | Inst::Panic { .. }
        )
    }
}

// ─── Basic Blocks ───────────────────────────────────────────────────────

/// A basic block — a sequence of instructions with a single entry point
/// and a single exit (terminator). Control flow only enters at the top
/// and only leaves at the bottom.
#[derive(Debug, Clone)]
pub struct BasicBlock {
    pub id: BlockId,
    pub instructions: Vec<ValueId>,
    /// Predecessors — blocks that jump to this block
    pub predecessors: Vec<BlockId>,
    /// Parameters — SSA values that this block receives from predecessors
    /// (used for phi-like semantics without explicit phi nodes)
    pub params: Vec<(ValueId, IrType)>,
}

impl BasicBlock {
    pub fn new(id: BlockId) -> Self {
        BasicBlock {
            id,
            instructions: Vec::new(),
            predecessors: Vec::new(),
            params: Vec::new(),
        }
    }
}

// ─── Functions ──────────────────────────────────────────────────────────

/// An IR function — contains basic blocks, value metadata, and effect info
#[derive(Debug, Clone)]
pub struct IrFunction {
    pub name: String,
    pub params: Vec<(ValueId, IrType)>,
    pub ret_ty: IrType,
    pub blocks: Vec<BasicBlock>,
    pub values: HashMap<ValueId, ValueMeta>,
    /// Instructions keyed by the value ID they produce
    pub instructions: HashMap<ValueId, Inst>,
    pub effects: EffectSet,
    /// Next available value ID
    next_value: u32,
    /// Next available block ID
    pub next_block: u32,
    /// Entry block ID (always 0)
    pub entry: BlockId,
    /// Local variables — maps source variable names to SSA values
    pub locals: HashMap<String, ValueId>,
}

impl IrFunction {
    pub fn new(name: String, ret_ty: IrType) -> Self {
        let mut func = IrFunction {
            name,
            params: Vec::new(),
            ret_ty,
            blocks: Vec::new(),
            values: HashMap::new(),
            instructions: HashMap::new(),
            effects: EffectSet::new(),
            next_value: 0,
            next_block: 0,
            entry: BlockId(0),
            locals: HashMap::new(),
        };
        // Create entry block
        func.new_block();
        func
    }

    /// Allocate a new SSA value ID
    pub fn new_value(&mut self, meta: ValueMeta) -> ValueId {
        let id = ValueId(self.next_value);
        self.next_value += 1;
        self.values.insert(id, meta);
        id
    }

    /// Allocate a new block ID
    pub fn new_block(&mut self) -> BlockId {
        let id = BlockId(self.next_block);
        self.next_block += 1;
        self.blocks.push(BasicBlock::new(id));
        id
    }

    /// Get the current (last) block
    pub fn current_block(&self) -> BlockId {
        BlockId(self.next_block - 1)
    }

    /// Append an instruction to the current block
    pub fn emit(&mut self, inst: Inst) -> ValueId {
        let ty = inst.result_type();
        let effects = inst.effects();
        let is_terminator = inst.is_terminator();

        let meta = ValueMeta {
            ty,
            region: Region::Stack,
            effects,
            source_line: None,
        };

        let id = self.new_value(meta);
        let block_idx = (self.next_block - 1) as usize;
        self.blocks[block_idx].instructions.push(id);

        // Store the instruction alongside the value for later lookup
        // We store it in the values map as a side channel
        // Actually, we need a separate instruction store
        // Let's store instructions in a HashMap<ValueId, Inst>
        self.instructions.insert(id, inst);

        if is_terminator {
            // Next emit goes to a new block (caller should create one)
        }

        id
    }

    /// Add a parameter to a block
    pub fn add_block_param(&mut self, block: BlockId, ty: IrType) -> ValueId {
        let meta = ValueMeta {
            ty: ty.clone(),
            region: Region::Stack,
            effects: EffectSet::none(),
            source_line: None,
        };
        let id = self.new_value(meta);
        let block_idx = block.0 as usize;
        self.blocks[block_idx].params.push((id, ty));
        id
    }

    /// Map a source variable name to an SSA value
    pub fn bind_local(&mut self, name: &str, value: ValueId) {
        self.locals.insert(name.to_string(), value);
    }

    /// Look up a source variable name
    pub fn lookup_local(&self, name: &str) -> Option<ValueId> {
        self.locals.get(name).copied()
    }

    /// Get an instruction by value ID
    pub fn get_inst(&self, id: ValueId) -> Option<&Inst> {
        self.instructions.get(&id)
    }
}

// ─── Module ─────────────────────────────────────────────────────────────

/// Struct layout — field offsets and total size
#[derive(Debug, Clone)]
pub struct StructLayout {
    pub name: String,
    pub fields: Vec<(String, IrType, u32)>,  // (name, type, offset)
    pub size: u64,
    pub align: u64,
}

/// A Pledge IR module — the top-level container for a compilation unit.
/// Contains functions, global data, struct layouts, and metadata.
#[derive(Debug, Clone)]
pub struct IrModule {
    pub functions: HashMap<String, IrFunction>,
    pub struct_layouts: HashMap<String, StructLayout>,
    /// Global string constants — maps string content to a label
    pub string_constants: HashMap<String, String>,
    /// Function signatures for external/runtime functions
    pub extern_functions: HashMap<String, (Vec<IrType>, IrType)>,
    /// Effects of each function (computed from function body)
    pub function_effects: HashMap<String, EffectSet>,
}

impl IrModule {
    pub fn new() -> Self {
        IrModule {
            functions: HashMap::new(),
            struct_layouts: HashMap::new(),
            string_constants: HashMap::new(),
            extern_functions: HashMap::new(),
            function_effects: HashMap::new(),
        }
    }

    /// Register a struct layout
    pub fn add_struct(&mut self, layout: StructLayout) {
        self.struct_layouts.insert(layout.name.clone(), layout);
    }

    /// Get a struct layout by name
    pub fn get_struct(&self, name: &str) -> Option<&StructLayout> {
        self.struct_layouts.get(name)
    }

    /// Intern a string constant — returns the label
    pub fn intern_string(&mut self, s: &str) -> String {
        if let Some(label) = self.string_constants.get(s) {
            return label.clone();
        }
        let label = format!(".str_{}", self.string_constants.len());
        self.string_constants.insert(s.to_string(), label.clone());
        label
    }

    /// Register an external/runtime function
    pub fn add_extern(&mut self, name: &str, params: Vec<IrType>, ret: IrType) {
        self.extern_functions.insert(name.to_string(), (params, ret));
    }

    /// Compute the size of a struct type (looks up layout)
    pub fn type_size(&self, ty: &IrType) -> u64 {
        match ty {
            IrType::Struct(name) => {
                self.struct_layouts.get(name)
                    .map(|l| l.size)
                    .unwrap_or(0)
            }
            _ => ty.size(),
        }
    }

    /// Compute the alignment of a struct type
    pub fn type_align(&self, ty: &IrType) -> u64 {
        match ty {
            IrType::Struct(name) => {
                self.struct_layouts.get(name)
                    .map(|l| l.align)
                    .unwrap_or(8)
            }
            _ => ty.align(),
        }
    }
}

impl Default for IrModule {
    fn default() -> Self {
        Self::new()
    }
}

// ─── Type Lowering ──────────────────────────────────────────────────────

/// Lower an AST type to an IR type
pub fn lower_type(ast_ty: &AstType) -> IrType {
    match ast_ty {
        AstType::I8 => IrType::I8,
        AstType::I16 => IrType::I16,
        AstType::I32 => IrType::I32,
        AstType::I64 => IrType::I64,
        AstType::I128 => IrType::I64, // I128 lowered to I64 for now
        AstType::U8 => IrType::U8,
        AstType::U16 => IrType::U16,
        AstType::U32 => IrType::U32,
        AstType::U64 => IrType::U64,
        AstType::U128 => IrType::U64, // U128 lowered to U64 for now
        AstType::F32 => IrType::F32,
        AstType::F64 => IrType::F64,
        AstType::Bool => IrType::Bool,
        AstType::Str => IrType::Ptr(Box::new(IrType::U8)), // str -> *u8 (simplified)
        AstType::Void => IrType::Void,
        AstType::Never => IrType::Void,
        AstType::Named(name) => IrType::Struct(name.clone()),
        AstType::Array { elem, size } => {
            IrType::Array(Box::new(lower_type(elem)), *size)
        }
        AstType::Ref { inner, .. } => {
            IrType::Ptr(Box::new(lower_type(inner)))
        }
        AstType::Generic { name, args } => {
            let ir_args = args.iter().map(lower_type).collect();
            IrType::Generic(name.clone(), ir_args)
        }
        AstType::Tuple(_) => {
            IrType::Opaque("tuple".to_string())
        }
        AstType::Func { .. } => {
            IrType::Opaque("func".to_string())
        }
    }
}

// ─── IR Printing (for debugging and IR output) ──────────────────────────

impl IrModule {
    /// Print the entire module as readable IR text
    pub fn to_string(&self) -> String {
        let mut out = String::new();

        // Struct layouts
        if !self.struct_layouts.is_empty() {
            out.push_str("; ─── Struct Layouts ───\n");
            for layout in self.struct_layouts.values() {
                out.push_str(&format!("struct {} {{\n", layout.name));
                out.push_str(&format!("  size: {}, align: {}\n", layout.size, layout.align));
                for (name, ty, offset) in &layout.fields {
                    out.push_str(&format!("  +{} {}: {}\n", offset, name, ty));
                }
                out.push_str("}\n\n");
            }
        }

        // String constants
        if !self.string_constants.is_empty() {
            out.push_str("; ─── String Constants ───\n");
            for (s, label) in &self.string_constants {
                out.push_str(&format!("{} = \"{}\"\n", label, s));
            }
            out.push('\n');
        }

        // External functions
        if !self.extern_functions.is_empty() {
            out.push_str("; ─── External Functions ───\n");
            for (name, (params, ret)) in &self.extern_functions {
                let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
                out.push_str(&format!("extern {}({}) -> {}\n",
                    name, param_strs.join(", "), ret));
            }
            out.push('\n');
        }

        // Functions
        for (name, func) in &self.functions {
            out.push_str(&format!("; ─── Function: {} ───\n", name));
            let param_strs: Vec<String> = func.params.iter()
                .map(|(id, ty)| format!("{}: {}", id, ty))
                .collect();
            out.push_str(&format!("pf {}({}) -> {} {}\n",
                name,
                param_strs.join(", "),
                func.ret_ty,
                func.effects,
            ));
            out.push_str("{\n");

            for block in &func.blocks {
                out.push_str(&format!("  {}:", block.id));
                if !block.params.is_empty() {
                    let params: Vec<String> = block.params.iter()
                        .map(|(id, ty)| format!("{}: {}", id, ty))
                        .collect();
                    out.push_str(&format!(" ({})", params.join(", ")));
                }
                if !block.predecessors.is_empty() {
                    let preds: Vec<String> = block.predecessors.iter()
                        .map(|b| b.to_string())
                        .collect();
                    out.push_str(&format!("  ; preds: {}", preds.join(", ")));
                }
                out.push('\n');

                for &inst_id in &block.instructions {
                    let inst = func.instructions.get(&inst_id);
                    let meta = func.values.get(&inst_id);
                    if let (Some(inst), Some(meta)) = (inst, meta) {
                        out.push_str(&format!("    {} = {:?}  ; {} {}\n",
                            inst_id, inst, meta.ty, meta.effects));
                    }
                }
            }

            out.push_str("}\n\n");
        }

        out
    }
}

// We need to store instructions per-function. Add instructions field to IrFunction.
// Since we can't easily modify the struct after definition, we include it here.
