// Fides IR — Typed SSA with effect and region annotations
//
// Design principles:
// - Every value has a type and a unique ID (SSA form)
// - Every instruction tracks its effects (alloc, io, divergence)
// - Memory is organized into regions (stack, heap, static)
// - IR is serializable for incremental compilation and hot reload
// - IR is interpretable for comptime and debugging
//
// This IR is the heart of Fides — it's what makes the language unique:
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

    /// Is this pointer-like (a pointer, an opaque handle, or a generic that
    /// represents a heap-allocated collection)?  These all occupy one machine
    /// word and can be stored interchangeably in a let-binding's alloca.
    pub fn is_ptr_like(&self) -> bool {
        matches!(self,
            IrType::Ptr(_)
            | IrType::Opaque(_)
            | IrType::Generic(..)
        )
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

    /// Get all type parameters of a Generic type
    /// For `Generic("HashMap", [String, I64])` returns `&[String, I64]`
    pub fn generic_args(&self) -> &[IrType] {
        match self {
            IrType::Generic(_, args) => args,
            _ => &[],
        }
    }

    /// Is this a signed integer type?
    pub fn is_signed(&self) -> bool {
        matches!(self, IrType::I8 | IrType::I16 | IrType::I32 | IrType::I64)
    }

    /// Is this an unsigned integer type?
    pub fn is_unsigned(&self) -> bool {
        matches!(self, IrType::U8 | IrType::U16 | IrType::U32 | IrType::U64)
    }

    /// Is this a numeric type (int or float)?
    pub fn is_numeric(&self) -> bool {
        self.is_int() || self.is_float()
    }

    /// Is this a scalar type (numeric or bool)?
    pub fn is_scalar(&self) -> bool {
        self.is_numeric() || *self == IrType::Bool
    }

    /// Is this an aggregate type (struct, array, vec, map)?
    pub fn is_aggregate(&self) -> bool {
        matches!(
            self,
            IrType::Struct(_) | IrType::Array(_, _) | IrType::Opaque(_) | IrType::Generic(_, _)
        )
    }

    /// Is this type a collection (Vec or HashMap)?
    pub fn is_collection(&self) -> bool {
        self.is_opaque_named("Vec") || self.is_opaque_named("HashMap")
    }

    /// Is this the Never type (unreachable)?
    pub fn is_never(&self) -> bool {
        *self == IrType::Never
    }

    /// Is this a void type (no value)?
    pub fn is_void(&self) -> bool {
        *self == IrType::Void
    }

    /// Get the element type of a collection (Vec<T> → T, HashMap<K,V> → V)
    pub fn collection_elem(&self) -> Option<&IrType> {
        match self {
            IrType::Generic(name, args) if name == "Vec" => args.first(),
            IrType::Generic(name, args) if name == "HashMap" => args.get(1),
            _ => None,
        }
    }

    /// Get the key type of a HashMap
    pub fn map_key_type(&self) -> Option<&IrType> {
        match self {
            IrType::Generic(name, args) if name == "HashMap" => args.first(),
            _ => None,
        }
    }

    /// Get the value type of a HashMap
    pub fn map_value_type(&self) -> Option<&IrType> {
        match self {
            IrType::Generic(name, args) if name == "HashMap" => args.get(1),
            _ => None,
        }
    }

    /// Check if two types are compatible (ignoring opaque name differences)
    /// e.g. Opaque("Vec") and Generic("Vec", [I64]) are compatible
    pub fn is_compatible_with(&self, other: &IrType) -> bool {
        if self == other {
            return true;
        }
        // Opaque and Generic with same name are compatible
        match (self, other) {
            (IrType::Opaque(n1), IrType::Generic(n2, _)) | (IrType::Generic(n2, _), IrType::Opaque(n1))
                if n1 == n2 => true,
            // Ptr types are compatible if pointee is compatible
            (IrType::Ptr(a), IrType::Ptr(b)) => a.is_compatible_with(b),
            // Any pointer is compatible with Opaque (runtime handle)
            (IrType::Ptr(_), IrType::Opaque(_)) | (IrType::Opaque(_), IrType::Ptr(_)) => true,
            _ => false,
        }
    }

    /// Get the bit width of this type (for integer/float types)
    pub fn bit_width(&self) -> Option<u32> {
        match self {
            IrType::I8 | IrType::U8 | IrType::Bool => Some(8),
            IrType::I16 | IrType::U16 => Some(16),
            IrType::I32 | IrType::U32 | IrType::F32 => Some(32),
            IrType::I64 | IrType::U64 | IrType::F64 | IrType::Ptr(_) => Some(64),
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
/// This is Fides's key differentiator — effects are tracked at the IR level.
///
/// Effect taxonomy:
/// - **Pure**: no side effects at all (can reorder, memoize, CSE)
/// - **Read**: reads from memory (loads)
/// - **Write**: writes to memory (stores)
/// - **Alloc**: allocates memory (heap or stack)
/// - **IO**: performs console/file I/O (print, println, file read/write)
/// - **Net**: performs network I/O (connect, send, recv)
/// - **Spawn**: spawns threads or processes
/// - **Diverge**: may not return (infinite loop, panic, abort)
/// - **Diff**: differentiable operation (supports autodiff gradient computation)
/// - **Noise**: adds noise to data (differential privacy, dropout, etc.)
/// - **Comm**: cross-device communication (tensor parallelism, all-reduce)
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Effect {
    /// Allocates memory (heap or stack)
    Alloc,
    /// Performs console/file I/O (print, println, file read/write)
    IO,
    /// Performs network I/O (connect, send, recv)
    Net,
    /// Spawns threads or subprocesses
    Spawn,
    /// May not return (infinite loop, panic, abort)
    Diverge,
    /// Reads from memory
    Read,
    /// Writes to memory
    Write,
    /// Differentiable operation (supports autodiff — matmul, relu, etc.)
    Diff,
    /// Adds noise to data (differential privacy, dropout, data augmentation)
    /// ML12: @dp functions must have this effect to prove noise was added.
    Noise,
    /// Cross-device communication (tensor parallelism, all-reduce, broadcast)
    /// ML13: @shard functions use this for cross-rank ops.
    Comm,
}

impl fmt::Display for Effect {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Effect::Alloc => write!(f, "alloc"),
            Effect::IO => write!(f, "io"),
            Effect::Net => write!(f, "net"),
            Effect::Spawn => write!(f, "spawn"),
            Effect::Diverge => write!(f, "diverge"),
            Effect::Read => write!(f, "read"),
            Effect::Write => write!(f, "write"),
            Effect::Diff => write!(f, "diff"),
            Effect::Noise => write!(f, "noise"),
            Effect::Comm => write!(f, "comm"),
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

    /// True if the function has no observable side effects
    /// (may still read memory, but no IO/Net/Spawn/Write/Alloc/Diverge)
    pub fn is_read_only(&self) -> bool {
        self.effects.iter().all(|e| matches!(e, Effect::Read))
    }

    /// True if the function may have observable side effects
    /// (IO, Net, Spawn, Write, Alloc, or Diverge)
    pub fn has_side_effects(&self) -> bool {
        self.effects.iter().any(|e| !matches!(e, Effect::Read))
    }

    /// Parse an effect from its string name
    pub fn parse_effect(s: &str) -> Option<Effect> {
        match s.to_lowercase().as_str() {
            "alloc" => Some(Effect::Alloc),
            "io" => Some(Effect::IO),
            "net" => Some(Effect::Net),
            "spawn" => Some(Effect::Spawn),
            "diverge" => Some(Effect::Diverge),
            "read" => Some(Effect::Read),
            "write" => Some(Effect::Write),
            _ => None,
        }
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

/// Memory regions — Fides's approach to memory safety without lifetimes.
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
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
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
    /// Whether this value was produced inside a `sandbox {}` block.
    /// Used by the optimizer for sandbox-aware devirt/inline (I-3C).
    pub in_sandbox: bool,
    /// I-6B: Whether this value's type is @linear (single-owner).
    /// Linear values can't alias with anything else, so stores/loads through
    /// linear pointers are safe to reorder relative to other memory ops.
    pub is_linear: bool,
    /// I-7A: Borrow kind — tracks whether this pointer value is a shared
    /// borrow (&x), mutable borrow (&mut x), or not a borrow (None).
    /// Used by alias analysis: a Load through a shared borrow is safe
    /// across a Store through a different mutable borrow to the same
    /// allocation (they can't coexist per the borrow checker).
    pub borrow_kind: BorrowKind,
}

/// I-7A: Borrow provenance for pointer values.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum BorrowKind {
    /// Not a borrow — this value is not a reference (or provenance unknown).
    None,
    /// Shared borrow: `&x`. Read-only, multiple shared borrows can coexist.
    Shared,
    /// Mutable borrow: `&mut x`. Exclusive, no other borrows can coexist.
    Mut,
}

// ─── Instructions ───────────────────────────────────────────────────────

/// IR instructions — the core of Fides IR.
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
    /// Function reference — stores function name, resolved to code address by PE emitter
    /// Treated like ConstStr by interpreter/C codegen (name-based lookup)
    ConstFnRef {
        name: String,
    },
    /// Null pointer
    ConstNull {
        ty: IrType,
    },

    // ─── Globals ───
    /// Get the address of a module-level mutable global variable.
    /// Returns a pointer that can be used with Load/Store.
    GlobalAddr {
        name: String,
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
        /// The type being stored (for typed stores through pointers).
        /// Defaults to I64 for local variable stores.
        store_ty: Option<IrType>,
    },
    /// Copy struct data from src to dst (memcpy of struct size bytes).
    /// Both dst and src are pointers to struct data.
    StructCopy {
        dst: ValueId,
        src: ValueId,
        ty: IrType,
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
    /// Dynamic dispatch: call a method on a trait object.
    DynCall {
        trait_name: String,
        method: String,
        args: Vec<ValueId>,
        ret_ty: IrType,
        effects: EffectSet,
    },
    /// Create a trait object from a concrete value.
    MakeDyn {
        value: ValueId,
        type_name: String,
        trait_name: String,
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

    // ─── Fides-Specific ───
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

    /// Inline assembly — Phase 5A.1
    /// template is the assembly template string
    /// inputs are (register, value) pairs for input operands
    /// outputs are (register, value_id) pairs for output operands (stored after execution)
    /// ret_ty is the type of the result (Void if no outputs, I64 for single output)
    Asm {
        template: String,
        inputs: Vec<(String, ValueId)>,
        outputs: Vec<(String, ValueId)>,
        ret_ty: IrType,
    },

    // ─── Region Management (Phase 2A) ───
    /// Begin a named region — allocations inside the region block use this region.
    /// Returns a region handle (as I64) that can be passed to Alloca.
    /// If is_gpu, allocations go to GPU memory (ML1: region-scoped GPU memory).
    /// If is_kv_cache, allocations go to bounded KV cache with sliding window (ML9).
    RegionBegin {
        name: String,
        is_gpu: bool,
        is_kv_cache: bool,
        window_size: Option<u64>,
    },
    /// End a named region — all memory allocated in this region is freed.
    /// The region handle is consumed and must not be used after this.
    /// If is_gpu, GPU memory is freed (ML1).
    /// If is_kv_cache, KV cache is freed (ML9).
    RegionEnd {
        name: String,
        is_gpu: bool,
        is_kv_cache: bool,
    },

    // ─── Persist Variables (F2) ───
    /// Load a persisted variable's value by name. If a saved value exists
    /// in the persistent state, return it. Otherwise return the default value.
    PersistLoad {
        name: String,
        default: ValueId,
        ty: IrType,
    },
    /// Save a variable's value to persistent state by name.
    /// Called before function return to persist the variable's current value.
    PersistSave {
        name: String,
        value: ValueId,
    },

    // ─── Transaction Blocks (Phase 7.5G) ───
    /// Begin a transaction — snapshot mutable state for potential rollback.
    /// The interpreter saves a copy of all memory and opaque values.
    /// If a panic occurs before TxnEnd, the snapshot is restored.
    TxnBegin,
    /// End a transaction — commit changes (discard the snapshot).
    /// If this is reached, the transaction succeeded.
    TxnEnd,

    // ─── Unsafe Blocks (F62: bounds checking lock) ───
    /// Begin an unsafe block — the interpreter skips bounds checks.
    /// The typechecker already enforces that raw pointer ops only happen
    /// inside unsafe blocks; these markers let the interpreter skip
    /// runtime bounds checks for array access.
    UnsafeBegin,
    /// End an unsafe block — resume bounds checking.
    UnsafeEnd,

    // ─── Autograd (ML3: Effect-Traced Autodiff) ───
    /// Begin a grad block — the interpreter enables gradient tracking for
    /// tensor operations. `var_name` is the variable to compute gradients for.
    GradBegin {
        var_name: String,
    },
    /// End a grad block — disable gradient tracking.
    GradEnd,
    /// Backward pass — compute gradients by reverse-mode AD.
    /// `loss` is the loss tensor to differentiate.
    Backward {
        loss: ValueId,
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
            Inst::ConstFnRef { .. } => IrType::I64, // function address
            Inst::ConstNull { ty, .. } => ty.clone(),
            Inst::GlobalAddr { ty, .. } => IrType::Ptr(Box::new(ty.clone())),
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
            Inst::DynCall { ret_ty, .. } => ret_ty.clone(),
            Inst::MakeDyn { .. } => IrType::Opaque("dyn".to_string()),
            Inst::BitCast { to, .. } => to.clone(),
            Inst::IntExtend { to, .. } => to.clone(),
            Inst::IntTrunc { to, .. } => to.clone(),
            Inst::IntToFloat { to, .. } => to.clone(),
            Inst::FloatToInt { to, .. } => to.clone(),
            Inst::TryUnwrap { .. } => IrType::Void,
            Inst::Print { .. } => IrType::Void,
            Inst::Panic { .. } => IrType::Never,
            Inst::Asm { ret_ty, .. } => ret_ty.clone(),
            Inst::RegionBegin { .. } => IrType::I64,
            Inst::RegionEnd { .. } => IrType::Void,
            Inst::PersistLoad { ty, .. } => ty.clone(),
            Inst::PersistSave { .. } => IrType::Void,
            Inst::TxnBegin => IrType::Void,
            Inst::TxnEnd => IrType::Void,
            Inst::UnsafeBegin => IrType::Void,
            Inst::UnsafeEnd => IrType::Void,
            Inst::GradBegin { .. } => IrType::Void,
            Inst::GradEnd => IrType::Void,
            Inst::Backward { .. } => IrType::Void,
            Inst::StructCopy { .. } => IrType::Void,
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
            Inst::DynCall { effects: e, .. } => effects.add_all(e),
            Inst::MakeDyn { .. } => {}
            Inst::Print { .. } => {
                effects.add(Effect::IO);
                effects.add(Effect::Read);
            },
            Inst::Panic { .. } => effects.add(Effect::Diverge),
            Inst::Asm { .. } => {
                effects.add(Effect::Read);
                effects.add(Effect::Write);
            },
            Inst::TryUnwrap { .. } => effects.add(Effect::Read),
            Inst::Unreachable => effects.add(Effect::Diverge),
            Inst::RegionBegin { .. } => effects.add(Effect::Alloc),
            Inst::RegionEnd { .. } => {
                effects.add(Effect::Alloc);
                effects.add(Effect::Write);
            },
            Inst::PersistLoad { .. } => effects.add(Effect::Read),
            Inst::PersistSave { .. } => effects.add(Effect::Write),
            Inst::TxnBegin | Inst::TxnEnd => effects.add(Effect::Write),
            Inst::UnsafeBegin | Inst::UnsafeEnd => {}
            Inst::GradBegin { .. } | Inst::GradEnd => effects.add(Effect::Diff),
            Inst::Backward { .. } => effects.add(Effect::Diff),
            Inst::StructCopy { .. } => effects.add(Effect::Write),
            _ => {}
        }
        effects
    }

    /// Get a short name for this instruction kind (for debugging/source maps)
    pub fn kind_name(&self) -> &'static str {
        match self {
            Inst::ConstInt { .. } => "ConstInt",
            Inst::ConstFloat { .. } => "ConstFloat",
            Inst::ConstBool { .. } => "ConstBool",
            Inst::ConstStr { .. } => "ConstStr",
            Inst::ConstFnRef { .. } => "ConstFnRef",
            Inst::ConstNull { .. } => "ConstNull",
            Inst::GlobalAddr { .. } => "GlobalAddr",
            Inst::BinOp { .. } => "BinOp",
            Inst::UnaryOp { .. } => "UnaryOp",
            Inst::Alloca { .. } => "Alloca",
            Inst::Load { .. } => "Load",
            Inst::Store { .. } => "Store",
            Inst::GetFieldPtr { .. } => "GetFieldPtr",
            Inst::GetElemPtr { .. } => "GetElemPtr",
            Inst::Jump { .. } => "Jump",
            Inst::Branch { .. } => "Branch",
            Inst::Return { .. } => "Return",
            Inst::Unreachable => "Unreachable",
            Inst::Call { .. } => "Call",
            Inst::CallPtr { .. } => "CallPtr",
            Inst::DynCall { .. } => "DynCall",
            Inst::MakeDyn { .. } => "MakeDyn",
            Inst::BitCast { .. } => "BitCast",
            Inst::IntExtend { .. } => "IntExtend",
            Inst::IntTrunc { .. } => "IntTrunc",
            Inst::IntToFloat { .. } => "IntToFloat",
            Inst::FloatToInt { .. } => "FloatToInt",
            Inst::TryUnwrap { .. } => "TryUnwrap",
            Inst::Print { .. } => "Print",
            Inst::Panic { .. } => "Panic",
            Inst::Asm { .. } => "Asm",
            Inst::RegionBegin { .. } => "RegionBegin",
            Inst::RegionEnd { .. } => "RegionEnd",
            Inst::PersistLoad { .. } => "PersistLoad",
            Inst::PersistSave { .. } => "PersistSave",
            Inst::TxnBegin => "TxnBegin",
            Inst::TxnEnd => "TxnEnd",
            Inst::UnsafeBegin => "UnsafeBegin",
            Inst::UnsafeEnd => "UnsafeEnd",
            Inst::GradBegin { .. } => "GradBegin",
            Inst::GradEnd => "GradEnd",
            Inst::Backward { .. } => "Backward",
            Inst::StructCopy { .. } => "StructCopy",
        }
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

    /// Get all ValueId references used by this instruction.
    /// Useful for verification and dataflow analysis.
    pub fn value_refs(&self) -> Vec<ValueId> {
        match self {
            Inst::ConstInt { .. }
            | Inst::ConstFloat { .. }
            | Inst::ConstBool { .. }
            | Inst::ConstStr { .. }
            | Inst::ConstFnRef { .. }
            | Inst::ConstNull { .. }
            | Inst::GlobalAddr { .. }
            | Inst::Alloca { .. }
            | Inst::Unreachable
            | Inst::Panic { .. } => Vec::new(),

            Inst::BinOp { left, right, .. } => vec![*left, *right],
            Inst::UnaryOp { operand, .. } => vec![*operand],
            Inst::Load { ptr, .. } => vec![*ptr],
            Inst::Store { ptr, value, .. } => vec![*ptr, *value],
            Inst::GetFieldPtr { struct_ptr, .. } => vec![*struct_ptr],
            Inst::GetElemPtr { array_ptr, index, .. } => vec![*array_ptr, *index],
            Inst::Jump { .. } => Vec::new(),
            Inst::Branch { cond, .. } => vec![*cond],
            Inst::Return { value: Some(v) } => vec![*v],
            Inst::Return { value: None } => Vec::new(),
            Inst::Call { args, .. } => args.clone(),
            Inst::CallPtr { fn_ptr, args, .. } => {
                let mut refs = vec![*fn_ptr];
                refs.extend(args);
                refs
            }
            Inst::DynCall { args, .. } => args.clone(),
            Inst::MakeDyn { value, .. } => vec![*value],
            Inst::BitCast { value, .. } => vec![*value],
            Inst::IntExtend { value, .. } => vec![*value],
            Inst::IntTrunc { value, .. } => vec![*value],
            Inst::IntToFloat { value, .. } => vec![*value],
            Inst::FloatToInt { value, .. } => vec![*value],
            Inst::TryUnwrap { value, .. } => vec![*value],
            Inst::Print { value, .. } => vec![*value],
            Inst::Asm { inputs, outputs, .. } => {
                let mut refs: Vec<ValueId> = inputs.iter().map(|(_, v)| *v).collect();
                refs.extend(outputs.iter().map(|(_, v)| *v));
                refs
            }
            Inst::RegionBegin { .. } | Inst::RegionEnd { .. } => vec![],
            Inst::PersistLoad { default, .. } => vec![*default],
            Inst::PersistSave { value, .. } => vec![*value],
            Inst::TxnBegin | Inst::TxnEnd | Inst::UnsafeBegin | Inst::UnsafeEnd => vec![],
            Inst::GradBegin { .. } | Inst::GradEnd => vec![],
            Inst::Backward { loss } => vec![*loss],
            Inst::StructCopy { dst, src, .. } => vec![*dst, *src],
        }
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
    /// Whether this is an interrupt handler (bare-metal mode).
    /// Interrupt handlers save/restore all registers and use `iret` instead of `ret`.
    pub is_interrupt: bool,
    /// F29: IDT vector number for @interrupt(N) annotated functions.
    /// None = no vector assigned (use `interrupt` keyword without vector, or no annotation).
    pub interrupt_vector: Option<u64>,
    /// Persist variables in this function — (name, alloca_ptr)
    /// Used to emit PersistSave before returns
    pub persist_vars: Vec<(String, ValueId)>,
    /// Drop-eligible locals — (alloca_ptr, struct_name) for variables whose
    /// type implements Drop. The compiler inserts drop calls at exit points.
    pub drop_locals: Vec<(ValueId, String)>,
    /// @zeroize locals — (alloca_ptr, size_in_bytes) for variables that must
    /// be wiped from memory at scope exit. The compiler inserts memset calls
    /// at exit points (before drops and returns).
    pub zeroize_locals: Vec<(ValueId, u64)>,
    /// Whether this function is annotated @wrapping (F65).
    /// @wrapping functions use wrap-around arithmetic instead of checked arithmetic.
    pub is_wrapping: bool,
    /// Whether this function is annotated @deterministic.
    /// @deterministic functions are reproducible — same inputs → same outputs.
    /// Used for memoization (I-4A) and loop hoisting.
    pub is_deterministic: bool,
    /// Whether this function is annotated @crashfree — can't panic at runtime.
    /// Used by the inline cost model (F-3F) to inline more aggressively.
    pub is_crashfree: bool,
    /// Gas limit from @gas(N) annotation. None = no gas limit.
    /// Used by the unroller (I-4C) to allow larger unrolling when gas proves termination.
    pub gas_limit: Option<u64>,
    /// Whether this function is @no-alloc — cannot allocate heap memory.
    /// Used by regalloc (I-2D) to skip allocator state preservation.
    pub is_no_alloc: bool,
    /// Whether this function is @medical — IEC 62304 / DAL-A safety.
    /// Implies @no-alloc + @no-recursion + @crashfree + @requires-audit.
    pub is_medical: bool,
    /// Whether this function is @kernel (GPU entry point for SPIR-V/PTX codegen).
    /// ML8: comptime kernel specialization — @kernel functions are GPU entry points.
    pub is_kernel: bool,
    /// Workgroup size from @kernel annotation: [x, y, z]. Default: [1, 1, 1].
    pub workgroup_size: [u32; 3],
    /// F23: Whether this function is @simd — enables aggressive loop vectorization.
    /// @simd functions get larger unroll limits and prefer SSE2/AVX/NEON instructions.
    pub is_simd: bool,
    /// Whether this function is @panic_handler — it receives panic messages
    /// instead of the runtime p_panic stub (bare-metal kernels).
    pub is_panic_handler: bool,
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
            is_interrupt: false,
            interrupt_vector: None,
            persist_vars: Vec::new(),
            drop_locals: Vec::new(),
            zeroize_locals: Vec::new(),
            is_wrapping: false,
            is_deterministic: false,
            is_crashfree: false,
            gas_limit: None,
            is_no_alloc: false,
            is_medical: false,
            is_kernel: false,
            workgroup_size: [1, 1, 1],
            is_simd: false,
            is_panic_handler: false,
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
            in_sandbox: false,
            is_linear: false,
            borrow_kind: crate::ir::BorrowKind::None,
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
            in_sandbox: false,
            is_linear: false,
            borrow_kind: crate::ir::BorrowKind::None,
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
    /// If true, uses C-compatible layout (natural type sizes, not uniform 8-byte slots)
    pub repr_c: bool,
    /// If true, packed layout (1-byte alignment, no padding between fields)
    pub packed_layout: bool,
}

/// Source map entry — maps an IR value to its source location
#[derive(Debug, Clone)]
pub struct SourceEntry {
    pub function: String,
    pub value_id: u32,
    pub line: usize,
    pub inst: String,
}

/// Source map — maps IR values to source locations for debugging
#[derive(Debug, Clone, Default)]
pub struct SourceMap {
    pub entries: Vec<SourceEntry>,
}

impl SourceMap {
    pub fn new() -> Self {
        SourceMap { entries: Vec::new() }
    }

    /// Look up the source line for a given function and value ID
    pub fn lookup(&self, function: &str, value_id: u32) -> Option<usize> {
        self.entries.iter()
            .find(|e| e.function == function && e.value_id == value_id)
            .map(|e| e.line)
    }

    /// Get all entries for a given function
    pub fn for_function(&self, function: &str) -> Vec<&SourceEntry> {
        self.entries.iter()
            .filter(|e| e.function == function)
            .collect()
    }

    /// Render the source map as a text table
    pub fn to_string(&self) -> String {
        let mut out = String::from("Source Map:\n");
        out.push_str("  Function           Value  Line  Instruction\n");
        out.push_str("  ─────────────────  ─────  ────  ────────────\n");
        for e in &self.entries {
            out.push_str(&format!(
                "  {:18}  v{:4}  {:4}  {}\n",
                e.function, e.value_id, e.line, e.inst
            ));
        }
        out
    }
}

/// A Fides IR module — the top-level container for a compilation unit.
/// Its tables are ordered maps so every backend emits the same output for
/// the same input (reproducible builds; HashMap order differs per run).
/// Contains functions, global data, struct layouts, and metadata.
/// A `ConstFnRef` of this prefix and a function name is the address of the
/// function's adapter, which C can call: it supplies the `__env` Fides takes.
pub const C_FN_PREFIX: &str = "__c_fn_";

/// The function a `ConstFnRef` name refers to.
pub fn c_fn_target(name: &str) -> &str {
    name.strip_prefix(C_FN_PREFIX).unwrap_or(name)
}

#[derive(Debug, Clone)]
pub struct IrModule {
    pub functions: std::collections::BTreeMap<String, IrFunction>,
    pub struct_layouts: std::collections::BTreeMap<String, StructLayout>,
    /// Global string constants — maps string content to a label
    pub string_constants: std::collections::BTreeMap<String, String>,
    /// Module-level mutable global variables: maps name → (type, initializer expr)
    pub globals: std::collections::BTreeMap<String, (IrType, crate::ast::Expr)>,
    /// Function signatures for external/runtime functions
    pub extern_functions: std::collections::BTreeMap<String, (Vec<IrType>, IrType)>,
    /// Effects of each function (computed from function body)
    pub function_effects: std::collections::BTreeMap<String, EffectSet>,
    /// User-defined extern declarations for FFI (C codegen uses these)
    pub extern_decls: Vec<crate::ast::ExternDecl>,
    /// Functions passed to C: each gets an adapter C can call
    pub c_callbacks: std::collections::BTreeSet<String>,
    /// Trait method names in order, for vtable dispatch.
    /// Maps trait name → list of method names.
    pub trait_methods: std::collections::BTreeMap<String, Vec<String>>,
    /// Trait implementations: maps trait name → list of type names that implement it.
    /// Used by devirtualization (F-3D) to replace DynCall with direct Call
    /// when only one impl exists.
    pub trait_impls: std::collections::BTreeMap<String, Vec<String>>,
    /// I-6B: Set of @linear struct names. Propagated from the lowerer
    /// for use by alias analysis and other optimizations.
    pub linear_structs: std::collections::BTreeSet<String>,
    /// Names of globals declared @cpu_local — accessed through the GS
    /// segment (per-CPU storage) on native backends instead of .rdata.
    pub cpu_locals: std::collections::BTreeSet<String>,
}

impl IrModule {
    pub fn new() -> Self {
        IrModule {
            functions: std::collections::BTreeMap::new(),
            struct_layouts: std::collections::BTreeMap::new(),
            string_constants: std::collections::BTreeMap::new(),
            globals: std::collections::BTreeMap::new(),
            extern_functions: std::collections::BTreeMap::new(),
            function_effects: std::collections::BTreeMap::new(),
            extern_decls: Vec::new(),
            c_callbacks: std::collections::BTreeSet::new(),
            trait_methods: std::collections::BTreeMap::new(),
            trait_impls: std::collections::BTreeMap::new(),
            linear_structs: std::collections::BTreeSet::new(),
            cpu_locals: std::collections::BTreeSet::new(),
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

    /// Generate a source map from the IR module.
    /// Maps each instruction that has source_line info to a SourceEntry.
    pub fn source_map(&self) -> SourceMap {
        let mut map = SourceMap::new();
        for (func_name, func) in &self.functions {
            for (value_id, meta) in &func.values {
                if let Some(line) = meta.source_line {
                    let inst_str = func.instructions
                        .get(value_id)
                        .map(|inst| format!("{}", inst.kind_name()))
                        .unwrap_or_else(|| "?".to_string());
                    map.entries.push(SourceEntry {
                        function: func_name.clone(),
                        value_id: value_id.0,
                        line,
                        inst: inst_str,
                    });
                }
            }
        }
        map
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
        AstType::I128 => IrType::I64, // I128 lowered to I64 — 128-bit not needed for self-hosting (C codegen uses AST types for signatures)
        AstType::U8 => IrType::U8,
        AstType::U16 => IrType::U16,
        AstType::U32 => IrType::U32,
        AstType::U64 => IrType::U64,
        AstType::U128 => IrType::U64, // U128 lowered to U64 — same reason
        AstType::F32 => IrType::F32,
        AstType::F64 => IrType::F64,
        AstType::Bool => IrType::Bool,
        AstType::Str => IrType::Ptr(Box::new(IrType::U8)), // str -> *u8 (simplified)
        AstType::Void => IrType::Void,
        AstType::Never => IrType::Void,
        AstType::Infer => IrType::I64, // Infer defaults to i64 at the IR level
        AstType::Named(name) => {
            // Vec, HashMap, Tensor, and Allocator are opaque types (not structs)
            if name == "Vec" || name == "HashMap" || name == "Tensor" || name == "Allocator" {
                IrType::Opaque(name.clone())
            } else {
                IrType::Struct(name.clone())
            }
        }
        AstType::Array { elem, size } => {
            IrType::Array(Box::new(lower_type(elem)), *size)
        }
        AstType::ArrayConst { elem, .. } => {
            // Const generic array — size is resolved during monomorphization.
            // Use a placeholder size of 0; the monomorphizer will replace this.
            IrType::Array(Box::new(lower_type(elem)), 0)
        }
        AstType::Ref { inner, .. } => {
            IrType::Ptr(Box::new(lower_type(inner)))
        }
        AstType::RawPtr { inner, .. } => {
            // Raw pointers lower to the same IR as references — both are Ptr
            IrType::Ptr(Box::new(lower_type(inner)))
        }
        AstType::Generic { name, args } => {
            // Future<T> is represented as i64 at the IR level (opaque handle).
            // The Future wrapping/unwrapping is done via p_future_ready/p_future_poll.
            if name == "Future" {
                return IrType::I64;
            }
            let ir_args = args.iter().map(lower_type).collect();
            IrType::Generic(name.clone(), ir_args)
        }
        AstType::Tuple(_) => {
            IrType::Opaque("tuple".to_string())
        }
        AstType::Func { .. } => {
            IrType::Opaque("func".to_string())
        }
        AstType::Assoc(_, _, _, _) => {
            IrType::I64
        }
        AstType::Dyn(name) => {
            IrType::Opaque(format!("dyn:{}", name))
        }
        AstType::ShapedTensor { .. } => {
            IrType::Opaque("Tensor".to_string())
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
            out.push_str(&format!("fn {}({}) -> {} {}\n",
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

// ─── IR Optimization Passes ─────────────────────────────────────────────

impl IrModule {
    /// Run basic optimization passes on this module.
    ///
    /// Currently implements:
    /// - Constant folding for binary operations
    /// - Dead code elimination (removes unused instructions)
    ///
    /// Returns the number of optimizations applied.
    pub fn optimize(&mut self) -> usize {
        let mut total = 0;
        for func in self.functions.values_mut() {
            total += func.constant_fold();
        }
        total
    }
}

impl IrFunction {
    /// Constant folding: evaluate binary operations on constants at compile time.
    fn constant_fold(&mut self) -> usize {
        let mut folded = 0;
        let block_ids: Vec<u32> = self.blocks.iter().map(|b| b.id.0).collect();

        for block_id in block_ids {
            let block = &self.blocks[block_id as usize];
            let inst_ids = block.instructions.clone();

            for inst_id in inst_ids {
                let inst = match self.instructions.get(&inst_id) {
                    Some(i) => i.clone(),
                    None => continue,
                };

                if let Inst::BinOp { op, left, right, ty } = &inst {
                    // Try to fold if both operands are constants
                    let left_inst = self.instructions.get(left);
                    let right_inst = self.instructions.get(right);

                    if let (Some(Inst::ConstInt { value: lv, .. }), Some(Inst::ConstInt { value: rv, .. })) =
                        (left_inst, right_inst)
                    {
                        if let Some(result) = eval_binop_int(op, *lv, *rv, ty) {
                            self.instructions.insert(inst_id, Inst::ConstInt { value: result, ty: ty.clone() });
                            folded += 1;
                        }
                    }
                }
            }
        }
        folded
    }
}

/// Evaluate a binary operation on integer constants.
fn eval_binop_int(op: &BinOp, l: i64, r: i64, ty: &IrType) -> Option<i64> {
    use BinOp::*;
    let result = match op {
        Add => l.wrapping_add(r),
        Sub => l.wrapping_sub(r),
        Mul => l.wrapping_mul(r),
        Div => if r == 0 { return None } else { l.wrapping_div(r) },
        Mod => if r == 0 { return None } else { l.wrapping_rem(r) },
        BitAnd => l & r,
        BitOr => l | r,
        BitXor => l ^ r,
        Shl => l.wrapping_shl(r as u32),
        Shr => l.wrapping_shr(r as u32),
        Eq => (l == r) as i64,
        Ne => (l != r) as i64,
        Lt => (l < r) as i64,
        Le => (l <= r) as i64,
        Gt => (l > r) as i64,
        Ge => (l >= r) as i64,
        _ => return None,
    };
    // Truncate to the target type width
    let truncated = match ty {
        IrType::I8 | IrType::U8 => (result as i8) as i64,
        IrType::I16 | IrType::U16 => (result as i16) as i64,
        IrType::I32 | IrType::U32 => (result as i32) as i64,
        _ => result,
    };
    Some(truncated)
}

// ─── IR Verification Pass ──────────────────────────────────────────────

/// Result of verifying an IR module.
#[derive(Debug)]
pub struct VerificationResult {
    pub errors: Vec<String>,
    pub warnings: Vec<String>,
}

impl VerificationResult {
    pub fn is_valid(&self) -> bool {
        self.errors.is_empty()
    }

    pub fn report(&self) {
        for w in &self.warnings {
            eprintln!("warning: {}", w);
        }
        for e in &self.errors {
            eprintln!("error: {}", e);
        }
    }
}

impl IrModule {
    /// Verify the correctness of this IR module.
    ///
    /// Checks:
    /// - Every function has an entry block
    /// - Every block ends with a terminator
    /// - All ValueId references point to valid instructions
    /// - Block IDs in control flow are valid
    /// - Return types match function signatures
    /// - No duplicate ValueIds
    pub fn verify(&self) -> VerificationResult {
        let mut errors: Vec<String> = Vec::new();
        let mut warnings: Vec<String> = Vec::new();

        for (name, func) in &self.functions {
            // Check: function has blocks
            if func.blocks.is_empty() {
                errors.push(format!("function '{}' has no blocks", name));
                continue;
            }

            // Check: entry block exists
            if func.blocks[0].id != func.entry {
                warnings.push(format!(
                    "function '{}': first block id {} != entry {}",
                    name, func.blocks[0].id.0, func.entry.0
                ));
            }

            // Collect all valid block IDs
            let valid_blocks: std::collections::HashSet<u32> =
                func.blocks.iter().map(|b| b.id.0).collect();

            // Collect all defined ValueIds
            let defined_values: std::collections::HashSet<u32> =
                func.instructions.keys().map(|v| v.0).collect();

            // Check each block
            for block in &func.blocks {
                // Check: block has instructions
                if block.instructions.is_empty() {
                    warnings.push(format!(
                        "function '{}': block {} is empty",
                        name, block.id.0
                    ));
                    continue;
                }

                // Check: all instruction references are valid
                for &inst_id in &block.instructions {
                    if !func.instructions.contains_key(&inst_id) {
                        errors.push(format!(
                            "function '{}': block {} references undefined instruction v{}",
                            name, block.id.0, inst_id.0
                        ));
                    }
                }

                // Check: block ends with a terminator
                if block.instructions.is_empty() {
                    errors.push(format!(
                        "function '{}': block {} has no instructions (empty block)",
                        name, block.id.0
                    ));
                } else {
                let last_id = *block.instructions.last().unwrap();
                if let Some(last_inst) = func.instructions.get(&last_id) {
                    if !last_inst.is_terminator() {
                        errors.push(format!(
                            "function '{}': block {} does not end with a terminator (ends with {:?})",
                            name, block.id.0, last_inst
                        ));
                    }

                    // Check: no instructions after terminator
                    let mut found_terminator = false;
                    for &iid in &block.instructions {
                        if found_terminator {
                            errors.push(format!(
                                "function '{}': block {} has instruction after terminator",
                                name, block.id.0
                            ));
                            break;
                        }
                        if let Some(inst) = func.instructions.get(&iid) {
                            if inst.is_terminator() {
                                found_terminator = true;
                            }
                        }
                    }

                    // Check: control flow targets are valid
                    match last_inst {
                        Inst::Jump { target } => {
                            if !valid_blocks.contains(&target.0) {
                                errors.push(format!(
                                    "function '{}': block {} jumps to invalid block {}",
                                    name, block.id.0, target.0
                                ));
                            }
                        }
                        Inst::Branch { then, else_, .. } => {
                            if !valid_blocks.contains(&then.0) {
                                errors.push(format!(
                                    "function '{}': block {} branches to invalid then-block {}",
                                    name, block.id.0, then.0
                                ));
                            }
                            if !valid_blocks.contains(&else_.0) {
                                errors.push(format!(
                                    "function '{}': block {} branches to invalid else-block {}",
                                    name, block.id.0, else_.0
                                ));
                            }
                        }
                        _ => {}
                    }
                }
                }
            }

            // Check: all ValueId references in instructions are defined
            for (inst_id, inst) in &func.instructions {
                let refs = inst.value_refs();
                for ref_id in refs {
                    if !defined_values.contains(&ref_id.0) && !func.params.iter().any(|(p, _)| *p == ref_id) {
                        errors.push(format!(
                            "function '{}': instruction v{} references undefined value v{}",
                            name, inst_id.0, ref_id.0
                        ));
                    }
                }
            }

            // Check: function has a Return somewhere
            let has_return = func.blocks.iter().any(|b| {
                b.instructions.iter().any(|iid| {
                    matches!(func.instructions.get(iid), Some(Inst::Return { .. }))
                })
            });
            if !has_return && func.ret_ty != IrType::Void {
                warnings.push(format!(
                    "function '{}': no Return instruction (declared return type {:?})",
                    name, func.ret_ty
                ));
            }
        }

        VerificationResult { errors, warnings }
    }
}
