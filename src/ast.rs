#[derive(Debug, Clone, PartialEq)]
pub enum Type {
    I8,
    I16,
    I32,
    I64,
    I128,
    U8,
    U16,
    U32,
    U64,
    U128,
    F32,
    F64,
    Bool,
    Str,
    Void,
    Never,
    /// Inferred type — used for `-> _` return type inference.
    /// The typechecker replaces this with the inferred type from the function body.
    Infer,
    Named(String),
    Array { elem: Box<Type>, size: usize },
    /// Array with const generic size: [T; N] where N is a type param
    ArrayConst { elem: Box<Type>, size_param: String },
    Ref { inner: Box<Type>, is_mut: bool },
    /// Raw pointer type: *mut T or *const T
    /// Only allowed inside unsafe {} blocks. Like Rust's raw pointers.
    RawPtr { inner: Box<Type>, is_mut: bool },
    Generic { name: String, args: Vec<Type> },
    Tuple(Vec<Type>),
    Func {
        params: Vec<Type>,
        ret: Box<Type>,
    },
    /// An associated type: `T::Item` where `T` is a type parameter with a
    /// trait bound. Resolved to a concrete type during monomorphization.
    /// (type_param, trait_name, assoc_name, type_args) — type_args for GATs
    Assoc(String, String, String, Vec<Type>),
    /// A trait object type: `dyn TraitName` — enables dynamic dispatch.
    Dyn(String),
    /// A shaped tensor type: Tensor<f32, [784, 128]> or Tensor<f32, [batch, 784]>
    /// The shape dimensions are either integer literals or symbolic names.
    /// The type checker solves a constraint system at compile time to verify
    /// shape compatibility for matmul, add, etc.
    ShapedTensor {
        elem: Box<Type>,
        shape: Vec<ShapeDim>,
    },
}

/// A dimension in a tensor shape: either a concrete integer or a symbolic name.
#[derive(Debug, Clone, PartialEq)]
pub enum ShapeDim {
    /// Concrete dimension: 784, 128, etc.
    Known(i64),
    /// Symbolic dimension: batch, seq_len, etc.
    /// Two uses of the same symbolic name must match.
    Symbolic(String),
}

impl std::fmt::Display for ShapeDim {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ShapeDim::Known(n) => write!(f, "{}", n),
            ShapeDim::Symbolic(s) => write!(f, "{}", s),
        }
    }
}

impl std::fmt::Display for Type {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Type::I8 => write!(f, "i8"),
            Type::I16 => write!(f, "i16"),
            Type::I32 => write!(f, "i32"),
            Type::I64 => write!(f, "i64"),
            Type::I128 => write!(f, "i128"),
            Type::U8 => write!(f, "u8"),
            Type::U16 => write!(f, "u16"),
            Type::U32 => write!(f, "u32"),
            Type::U64 => write!(f, "u64"),
            Type::U128 => write!(f, "u128"),
            Type::F32 => write!(f, "f32"),
            Type::F64 => write!(f, "f64"),
            Type::Bool => write!(f, "bool"),
            Type::Str => write!(f, "str"),
            Type::Void => write!(f, "void"),
            Type::Never => write!(f, "never"),
            Type::Infer => write!(f, "_"),
            Type::Named(s) => write!(f, "{}", s),
            Type::Array { elem, size } => write!(f, "[{}; {}]", elem, size),
            Type::ArrayConst { elem, size_param } => write!(f, "[{}; {}]", elem, size_param),
            Type::Ref { inner, is_mut } => {
                if *is_mut { write!(f, "&mut ")? } else { write!(f, "&")? }
                write!(f, "{}", inner)
            }
            Type::RawPtr { inner, is_mut } => {
                if *is_mut { write!(f, "*mut ")? } else { write!(f, "*const ")? }
                write!(f, "{}", inner)
            }
            Type::Generic { name, args } => {
                write!(f, "{}<", name)?;
                for (i, a) in args.iter().enumerate() {
                    if i > 0 { write!(f, ", ")?; }
                    write!(f, "{}", a)?;
                }
                write!(f, ">")
            }
            Type::Tuple(types) => {
                write!(f, "(")?;
                for (i, t) in types.iter().enumerate() {
                    if i > 0 { write!(f, ", ")?; }
                    write!(f, "{}", t)?;
                }
                write!(f, ")")
            }
            Type::Func { params, ret } => {
                write!(f, "fn(")?;
                for (i, p) in params.iter().enumerate() {
                    if i > 0 {
                        write!(f, ", ")?;
                    }
                    write!(f, "{}", p)?;
                }
                write!(f, ") -> {}", ret)
            }
            Type::Assoc(tp, trait_name, name, args) => {
                if args.is_empty() {
                    write!(f, "{}::{}({})", tp, name, trait_name)
                } else {
                    let args_str: Vec<String> = args.iter().map(|t| t.to_string()).collect();
                    write!(f, "{}::{}<{}>({})", tp, name, args_str.join(", "), trait_name)
                }
            }
            Type::Dyn(name) => write!(f, "dyn {}", name),
            Type::ShapedTensor { elem, shape } => {
                write!(f, "Tensor<{}", elem)?;
                if !shape.is_empty() {
                    write!(f, ", [")?;
                    for (i, d) in shape.iter().enumerate() {
                        if i > 0 { write!(f, ", ")?; }
                        match d {
                            ShapeDim::Known(n) => write!(f, "{}", n)?,
                            ShapeDim::Symbolic(s) => write!(f, "{}", s)?,
                        }
                    }
                    write!(f, "]")?;
                }
                write!(f, ">")
            }
        }
    }
}

#[derive(Debug, Clone)]
pub enum Expr {
    IntLit(i64),
    FloatLit(f64),
    StringLit(String),
    BoolLit(bool),
    Ident(String),
    Binary {
        op: BinOp,
        left: Box<Expr>,
        right: Box<Expr>,
    },
    Unary {
        op: UnaryOp,
        expr: Box<Expr>,
    },
    Call {
        callee: String,
        args: Vec<Expr>,
    },
    FieldAccess {
        expr: Box<Expr>,
        field: String,
    },
    /// Optional field access: `expr?.field` — if expr is None, return 0; else access field.
    OptionalFieldAccess {
        expr: Box<Expr>,
        field: String,
    },
    StructLit {
        name: String,
        fields: Vec<(String, Expr)>,
        /// Spread source: `Point { x: 1, ..p }` copies all fields from p, then overrides x.
        /// The expression must be of the same struct type.
        spread: Option<Box<Expr>>,
    },
    If {
        cond: Box<Expr>,
        then_block: Vec<Stmt>,
        else_block: Option<Vec<Stmt>>,
    },
    Range {
        start: Box<Expr>,
        end: Box<Expr>,
        inclusive: bool,
    },
    Block(Vec<Stmt>),
    MethodCall {
        receiver: Box<Expr>,
        method: String,
        args: Vec<Expr>,
    },
    ArrayLit {
        elem_type: Box<Type>,
        elements: Vec<Expr>,
    },
    Index {
        array: Box<Expr>,
        index: Box<Expr>,
    },
    FString {
        parts: Vec<FStringPart>,
    },
    Match {
        expr: Box<Expr>,
        arms: Vec<MatchArm>,
    },
    EnumLit {
        enum_name: String,
        variant: String,
        args: Vec<Expr>,
    },
    Ref {
        expr: Box<Expr>,
        is_mut: bool,
    },
    Deref {
        expr: Box<Expr>,
    },
    StrConcat {
        left: Box<Expr>,
        right: Box<Expr>,
    },
    Try {
        expr: Box<Expr>,
    },
    /// Coalescing: `expr ?? default` — if expr is None, return default; else unwrap and return.
    Coalesce {
        expr: Box<Expr>,
        default: Box<Expr>,
    },
    /// Type/variant check: `expr is Type` or `expr is Variant`.
    /// For enum variants, this is a runtime tag check.
    /// For types, this is a compile-time type check.
    IsCheck {
        expr: Box<Expr>,
        /// Type name or enum variant name (e.g., "i64", "str", "Some", "None")
        type_name: String,
    },
    TryBlock {
        body: Vec<Stmt>,
    },
    TupleLit(Vec<Expr>),
    Closure {
        params: Vec<Param>,
        body: Vec<Stmt>,
        ret_ty: Option<Type>,
    },
    Cast {
        expr: Box<Expr>,
        target_ty: Type,
    },
    /// Inline assembly — Phase 5A.1
    /// asm!("template", in("reg") expr, out("reg") ident, ...)
    Asm {
        template: String,
        operands: Vec<AsmOperand>,
    },
    /// comptime <expr> — evaluate at compile time, substitute the result
    Comptime(Box<Expr>),
    /// comptime { stmts... } — block evaluated at compile time
    ComptimeBlock(Vec<Stmt>),
    /// comptime reflect_op(Type) — type reflection at compile time
    ComptimeReflect {
        op: ReflectOp,
        ty: Type,
    },
    /// Pipeline operator: left |> right
    /// Desugars to: right(left) if right is a function call,
    /// or right.forward(left) if right is a method call.
    PipeForward {
        left: Box<Expr>,
        right: Box<Expr>,
    },
    /// Quantization cast: expr ~> target_ty
    /// Converts a tensor to a quantized representation (q8, q4, etc.)
    QuantCast {
        expr: Box<Expr>,
        target_ty: Type,
    },
    /// await <expr> — suspend current function until the Future is ready (F: async/await)
    /// The expression must evaluate to a Future<T>. Returns T.
    Await {
        expr: Box<Expr>,
    },
    /// spawn <expr> — create a concurrent task, returns Future<T> (F: async/await)
    /// The expression is evaluated lazily in a new concurrent context.
    Spawn {
        expr: Box<Expr>,
    },
}

/// Compile-time reflection operations (Phase 3A)
#[derive(Debug, Clone, PartialEq)]
pub enum ReflectOp {
    TypeName,
    IsStruct,
    IsEnum,
    FieldCount,
    FieldNames,
    FieldTypes,
    EnumVariants,
}

#[derive(Debug, Clone)]
pub struct AsmOperand {
    pub direction: AsmDir,
    pub reg: String,
    pub expr: Expr,
}

#[derive(Debug, Clone, PartialEq)]
pub enum AsmDir {
    In,
    Out,
    Clobber,
}

#[derive(Debug, Clone)]
pub enum FStringPart {
    Literal(String),
    Expr(Expr),
}

#[derive(Debug, Clone)]
pub struct MatchArm {
    pub pattern: Pattern,
    /// Optional guard: `pattern if guard => body`
    pub guard: Option<Expr>,
    pub body: Vec<Stmt>,
}

#[derive(Debug, Clone)]
pub enum Pattern {
    Wildcard,
    IntLit(i64),
    BoolLit(bool),
    Ident(String),
    EnumVariant { enum_name: String, variant: String, bindings: Vec<(String, String)> },
    StrLit(String),
    Range { start: Box<Pattern>, end: Box<Pattern> },
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum BinOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Eq,
    Neq,
    Lt,
    Gt,
    LtEq,
    GtEq,
    And,
    Or,
    BitAnd,
    BitOr,
    BitXor,
    Shl,
    Shr,
}

#[derive(Debug, Clone, PartialEq)]
pub enum UnaryOp {
    Neg,
    Not,
    BitNot,
}

#[derive(Debug, Clone)]
pub enum Stmt {
    Let {
        name: String,
        ty: Option<Type>,
        expr: Expr,
        is_mut: bool,
        is_persist: bool,
        annotations: Vec<Annotation>,
        line: usize,
    },
    /// Tuple destructuring: `pv (a, b, ...) = expr`
    /// Binds each name to the corresponding element of the tuple returned by expr.
    LetTuple {
        names: Vec<String>,
        expr: Expr,
        line: usize,
    },
    /// Uninitialized mutable declaration: `pm x: i64;`
    /// The variable is declared without an initializer.
    /// The typechecker enforces that it must be assigned before read (F63: init guarantee).
    LetUninit {
        name: String,
        ty: Type,
        annotations: Vec<Annotation>,
        line: usize,
    },
    Assign {
        target: String,
        op: AssignOp,
        expr: Expr,
        line: usize,
    },
    IndexAssign {
        array: String,
        index: Expr,
        op: AssignOp,
        expr: Expr,
        line: usize,
    },
    DerefAssign {
        ptr_expr: Expr,
        op: AssignOp,
        expr: Expr,
        line: usize,
    },
    FieldAssign {
        object: Expr,
        field: String,
        op: AssignOp,
        expr: Expr,
        line: usize,
    },
    Expr(Expr, usize),
    Return(Option<Expr>, usize),
    If {
        cond: Expr,
        then_block: Vec<Stmt>,
        else_block: Option<Vec<Stmt>>,
        line: usize,
    },
    While {
        cond: Expr,
        body: Vec<Stmt>,
        line: usize,
    },
    For {
        var: String,
        iterator: Expr,
        body: Vec<Stmt>,
        line: usize,
    },
    Loop {
        body: Vec<Stmt>,
        line: usize,
    },
    Break(Option<Expr>, usize),
    Continue(usize),
    Defer(Vec<Stmt>, usize),
    Match {
        expr: Expr,
        arms: Vec<MatchArm>,
        line: usize,
    },
    Use { path: String, items: Vec<String>, line: usize },
    /// Effect handler: `handle { body } with { Effect.op(args) => handler_body, ... }`
    Handle {
        body: Vec<Stmt>,
        handlers: Vec<EffectHandler>,
        line: usize,
    },
    /// Perform an effect operation: `do Effect.op(args)`
    Do {
        effect: String,
        op: String,
        args: Vec<Expr>,
        line: usize,
    },
    /// Region block: `region r { body }` — creates a named region,
    /// allocates within it, and frees it at the end of the block.
    /// References to region-allocated data cannot escape the block.
    /// Optional `budget` specifies a compile-time memory budget in bytes.
    /// `region r with budget(4096) { body }` — compile error if exceeded.
    /// `region gpu r { body }` — GPU memory region (ML1: region-scoped GPU memory).
    /// Allocations use GPU memory; freed automatically at region exit.
    /// `region kv_cache r with budget(N) { body }` — KV cache region (ML9).
    /// Bounded attention cache with sliding window eviction.
    RegionBlock {
        name: String,
        budget: Option<u64>,
        is_gpu: bool,
        /// True if this is a KV cache region (ML9: bounded LLM attention cache).
        /// Implies sliding window eviction and session-scoped lifetime.
        is_kv_cache: bool,
        /// Sliding window size for KV cache regions (number of tokens to keep).
        /// None = no sliding window (keep all until budget exhausted).
        window_size: Option<u64>,
        body: Vec<Stmt>,
        line: usize,
    },
    /// Sandbox block: `sandbox { body }` — strips all capabilities for the
    /// scope. Any IO/FS/Net/Spawn call inside is a compile error.
    /// The body is otherwise a normal block — safety is compile-time only.
    Sandbox {
        body: Vec<Stmt>,
        line: usize,
    },
    /// Unsafe block: `unsafe { body }` — permits operations that would
    /// otherwise be rejected by the safety type system (e.g. raw pointer
    /// dereference, FFI calls without capability checks).
    /// The body is otherwise a normal block — unsafe is compile-time only.
    Unsafe {
        body: Vec<Stmt>,
        line: usize,
    },
    /// Transaction block: `txn { body }` — atomic transaction with rollback.
    /// If any operation in the body fails (returns error/panic), all changes
    /// to mutable variables made inside the block are rolled back.
    /// In the interpreter, this snapshots mutable variables on entry and
    /// restores them on failure. In native code, this is a no-op wrapper
    /// (transactions are handled at the application level).
    Txn {
        body: Vec<Stmt>,
        line: usize,
    },
    /// comptime gen { block } — evaluate block at compile time, parse the
    /// resulting string as Fides source code, and inject the declarations
    /// into the program. This enables comptime code generation.
    ComptimeGen {
        block: Vec<Stmt>,
        line: usize,
    },
    /// grad(var) { body; backward(loss); }
    /// Compile-time reverse-mode automatic differentiation.
    /// The compiler traces operations on `var` within the block,
    /// builds a backward computation graph, and emits gradient code.
    GradBlock {
        var: String,
        body: Vec<Stmt>,
        line: usize,
    },
    /// backward(loss) — triggers reverse-mode AD within a grad block.
    /// Populates .grad on all tracked variables.
    Backward {
        loss: Expr,
        line: usize,
    },
    /// Phase 8B (F18): ECS query statement.
    /// `query(ComponentA, ComponentB) { body }` — iterates over all entities
    /// that have all the specified components. The variable `entity` (i64)
    /// is bound to the current entity ID within the body.
    Query {
        components: Vec<String>,
        body: Vec<Stmt>,
        line: usize,
    },
    /// Select statement for channels: `select { case ch => x => body, ... default => body }`
    /// Polls all channels and executes the first ready one. If none ready and
    /// no default, blocks until one is ready.
    Select {
        cases: Vec<SelectCase>,
        default: Option<Vec<Stmt>>,
        line: usize,
    },
    /// F13: Parallel fork-join block: `parallel { | branch1 | branch2 | ... }`
    /// Each branch is executed in parallel (spawned as a thread), and all branches
    /// are joined before continuing. Branch results are discarded (use shared
    /// mutable state or channels for communication).
    Parallel {
        branches: Vec<Vec<Stmt>>,
        line: usize,
    },
}

/// A case in a select statement.
#[derive(Debug, Clone)]
pub struct SelectCase {
    /// Channel expression (evaluates to i64 channel handle)
    pub channel: Expr,
    /// Variable name to bind the received value to
    pub var_name: String,
    /// Body to execute when this case is selected
    pub body: Vec<Stmt>,
}

/// A handler for one effect operation within a `handle` block.
#[derive(Debug, Clone)]
pub struct EffectHandler {
    /// Effect name (e.g. "IO")
    pub effect: String,
    /// Operation name (e.g. "print")
    pub op: String,
    /// Parameter names for the handler function
    pub params: Vec<String>,
    /// Handler body
    pub body: Vec<Stmt>,
}

impl Stmt {
    /// Get the source line for this statement.
    pub fn line(&self) -> usize {
        match self {
            Stmt::Let { line, .. }
            | Stmt::LetTuple { line, .. }
            | Stmt::LetUninit { line, .. }
            | Stmt::Assign { line, .. }
            | Stmt::IndexAssign { line, .. }
            | Stmt::DerefAssign { line, .. }
            | Stmt::FieldAssign { line, .. }
            | Stmt::Expr(_, line)
            | Stmt::Return(_, line)
            | Stmt::If { line, .. }
            | Stmt::While { line, .. }
            | Stmt::For { line, .. }
            | Stmt::Loop { line, .. }
            | Stmt::Break(_, line)
            | Stmt::Continue(line)
            | Stmt::Defer(_, line)
            | Stmt::Match { line, .. }
            | Stmt::Use { line, .. }
            | Stmt::Handle { line, .. }
            | Stmt::Do { line, .. }
            | Stmt::RegionBlock { line, .. }
            | Stmt::Sandbox { line, .. }
            | Stmt::Unsafe { line, .. }
            | Stmt::Txn { line, .. }
            | Stmt::ComptimeGen { line, .. }
            | Stmt::GradBlock { line, .. }
            | Stmt::Backward { line, .. }
            | Stmt::Query { line, .. }
            | Stmt::Select { line, .. }
            | Stmt::Parallel { line, .. } => *line,
        }
    }
}

#[derive(Debug, Clone, PartialEq)]
pub enum AssignOp {
    Assign,       // =
    AddAssign,   // +=
    SubAssign,   // -=
    MulAssign,   // *=
    DivAssign,   // /=
    ModAssign,   // %=
    BitAndAssign, // &=
    BitOrAssign,  // |=
    BitXorAssign, // ^=
    ShlAssign,    // <<=
    ShrAssign,    // >>=
}

#[derive(Debug, Clone)]
pub struct Param {
    pub name: String,
    pub ty: Type,
    /// Whether this parameter/field is public (visible outside the defining module).
    /// Only meaningful for struct fields. Function params are always visible.
    pub is_pub: bool,
    /// Annotations on this parameter (e.g., @tainted for untrusted input)
    pub annotations: Vec<Annotation>,
}

#[derive(Debug, Clone)]
pub struct Func {
    pub name: String,
    pub type_params: Vec<String>,
    /// Trait bounds for type params: ("T" -> "Printable")
    pub type_param_bounds: Vec<(String, String)>,
    /// Const generic params: ("N" -> "i64") — comptime values used in array sizes
    pub const_params: Vec<(String, String)>,
    pub params: Vec<Param>,
    pub ret_ty: Type,
    pub body: Vec<Stmt>,
    pub is_pub: bool,
    /// Whether this is a comptime function (evaluated at compile time)
    pub is_comptime: bool,
    /// Explicit effect annotations (e.g. `!IO`, `!Net,Spawn`)
    /// Empty = no annotation (effects will be inferred)
    pub declared_effects: Vec<String>,
    /// Whether the function has an explicit effect annotation (`!IO`, `!{}`, etc.)
    /// If true and declared_effects is empty, the function is explicitly pure.
    pub has_effect_annotation: bool,
    /// Whether this is an interrupt handler (bare-metal mode).
    /// Interrupt handlers save/restore all registers and use `iret` instead of `ret`.
    pub is_interrupt: bool,
    /// Annotations attached to this function (e.g. @kernel, @serve, @on(gpu))
    pub annotations: Vec<Annotation>,
    /// Precondition contracts (F10: `given <expr>`). Must be true on entry.
    /// The typechecker verifies these at call sites with known argument values.
    pub preconditions: Vec<Expr>,
    /// Postcondition contracts (F10: `ensure <expr>`). Must be true on exit.
    /// `result` is a special identifier bound to the return value.
    pub postconditions: Vec<Expr>,
    /// Whether this is an async function (Phase 7.6: async/await).
    /// Async functions return Future<T> and can use `await` inside.
    pub is_async: bool,
    /// Source line number (1-based) of the `fn` keyword — for LSP go-to-definition.
    pub line: usize,
}

/// An annotation: @name or @name(args)
#[derive(Debug, Clone, PartialEq)]
pub struct Annotation {
    pub name: String,
    pub args: Vec<String>,
}

#[derive(Debug, Clone)]
pub struct StructDef {
    pub name: String,
    pub fields: Vec<Param>,
    pub is_pub: bool,
    /// Annotations attached to this struct (e.g. @linear)
    pub annotations: Vec<Annotation>,
    /// Type parameters for generic structs (e.g. ["T"] for Box<T>)
    pub type_params: Vec<String>,
    /// Source line number (1-based) of the `struct` keyword — for LSP go-to-definition.
    pub line: usize,
}

#[derive(Debug, Clone)]
pub struct EnumVariant {
    pub name: String,
    pub data: Option<Vec<Param>>,
}

#[derive(Debug, Clone)]
pub struct EnumDef {
    pub name: String,
    pub variants: Vec<EnumVariant>,
    pub is_pub: bool,
    /// Type parameters for generic enums (e.g. ["T"] for Option<T>, ["T", "E"] for Result<T, E>)
    pub type_params: Vec<String>,
    /// Source line number (1-based) of the `enum` keyword — for LSP go-to-definition.
    pub line: usize,
}

#[derive(Debug, Clone)]
pub struct ConstDef {
    pub name: String,
    pub ty: Type,
    pub expr: Expr,
    pub is_pub: bool,
    /// Source line number (1-based) of the `const` keyword — for LSP go-to-definition.
    pub line: usize,
}

/// A module-level mutable global variable: `var g_x: i64 = 0;` or `pub var g_x: i64 = 0;`
#[derive(Debug, Clone)]
pub struct GlobalVar {
    pub name: String,
    pub ty: Type,
    pub init: Expr,
    pub is_pub: bool,
}

#[derive(Debug, Clone)]
pub struct TypeAlias {
    pub name: String,
    pub alias_for: Type,
    pub is_pub: bool,
}

#[derive(Debug, Clone)]
pub struct ImplDef {
    pub struct_name: String,
    pub type_params: Vec<String>,
    pub methods: Vec<Func>,
}

/// A trait/capability definition: `cap Display { pf fmt(self) -> str; }`
#[derive(Debug, Clone)]
pub struct TraitDef {
    pub name: String,
    pub methods: Vec<TraitMethod>,
    /// Associated type declarations: `ty Item;` or GAT `ty Item<T, U>;`
    /// (name, type_params) — type_params is empty for non-GAT associated types
    pub associated_types: Vec<(String, Vec<String>)>,
    pub is_pub: bool,
}

/// A method signature in a trait definition
#[derive(Debug, Clone)]
pub struct TraitMethod {
    pub name: String,
    pub params: Vec<Param>,
    pub ret_ty: Type,
    pub has_self: bool,
    pub is_pub: bool,
    /// Default implementation body (None = no default)
    pub default_body: Option<Vec<Stmt>>,
}

/// A trait implementation: `impl Display for Point { ... }`
#[derive(Debug, Clone)]
pub struct TraitImpl {
    pub trait_name: String,
    pub type_name: String,
    pub type_params: Vec<String>,
    pub methods: Vec<Func>,
    /// Associated type bindings: `type Item = i64;` → ("Item", I64)
    pub associated_types: Vec<(String, Type)>,
}

#[derive(Debug, Clone)]
pub struct ImportDef {
    pub path: String,
    pub items: Vec<String>,
    pub capabilities: Vec<String>,
    pub version: Option<String>,
    /// Alias for the module path (e.g. `use core as c` → alias = "c")
    pub alias: Option<String>,
    /// Whether this is a re-export (`pub use`)
    pub is_pub: bool,
    /// Whether this is a glob import (`use core::*`)
    pub is_glob: bool,
}

#[derive(Debug, Clone)]
pub struct Attribute {
    pub name: String,
    pub args: Vec<String>,
}

#[derive(Debug, Clone, Default)]
pub struct Program {
    pub module: Option<String>,
    pub funcs: Vec<Func>,
    pub structs: Vec<StructDef>,
    pub enums: Vec<EnumDef>,
    pub consts: Vec<ConstDef>,
    /// Module-level mutable global variables: `var g_x: i64 = 0;`
    pub globals: Vec<GlobalVar>,
    pub type_aliases: Vec<TypeAlias>,
    pub impls: Vec<ImplDef>,
    pub imports: Vec<ImportDef>,
    pub attributes: Vec<Attribute>,
    pub traits: Vec<TraitDef>,
    pub trait_impls: Vec<TraitImpl>,
    pub extern_decls: Vec<ExternDecl>,
    /// Layer definitions: `layer Dense { fields... forward(self, x) -> T { ... } }`
    pub layers: Vec<LayerDef>,
    /// Model definitions: `model MLP { layers... forward(self, x) -> T { ... } }`
    pub models: Vec<ModelDef>,
    /// Phase 8B: ECS component declarations (F18)
    pub components: Vec<ComponentDef>,
    /// Phase 8B: ECS system declarations (F18)
    pub systems: Vec<SystemDef>,
    /// Phase 8B: State machine declarations (F19)
    pub state_machines: Vec<StateMachineDef>,
    /// Phase 8B: Event handler declarations (F20)
    pub event_handlers: Vec<EventHandlerDef>,
}

/// A neural network layer definition.
/// Syntactic sugar for a struct with a `forward` method.
/// `layer Dense { w: Tensor, b: Tensor, forward(self, x: Tensor) -> Tensor { ... } }`
/// desugars to:
///   struct Dense { w: Tensor, b: Tensor }
///   impl Dense { pf forward(self: Dense, x: Tensor) -> Tensor { ... }
///   pf Dense_new(...) -> Dense { ... }  // constructor
#[derive(Debug, Clone)]
pub struct LayerDef {
    pub name: String,
    pub fields: Vec<Param>,
    /// Constructor args (e.g. Dense(784, 128) → args = ["784", "128"])
    pub constructor_args: Vec<String>,
    pub forward: Func,
    pub is_pub: bool,
}

/// A neural network model definition.
/// Syntactic sugar for a struct containing layers with a `forward` method.
/// `model MLP { l1: Dense(784, 128), l2: Dense(128, 10), forward(self, x) -> T { ... } }`
#[derive(Debug, Clone)]
pub struct ModelDef {
    pub name: String,
    /// Layer instantiations: (field_name, layer_type, constructor_args)
    pub layers: Vec<(String, String, Vec<String>)>,
    pub forward: Func,
    pub is_pub: bool,
}

// ─── Phase 8B: Game Architecture AST Nodes ───────────────────────────

/// ECS component declaration (F18).
/// `component Position { x: f64, y: f64 }`
/// Desugars to: `struct Position { x: f64, y: f64 }`
#[derive(Debug, Clone)]
pub struct ComponentDef {
    pub name: String,
    pub fields: Vec<Param>,
    pub is_pub: bool,
}

/// ECS system declaration (F18).
/// `system Move(dt: f64) { query(Position, Velocity) { ... } }`
/// Desugars to: `pf Move(dt: f64) { ... }` with query statements
#[derive(Debug, Clone)]
pub struct SystemDef {
    pub name: String,
    pub params: Vec<Param>,
    pub body: Vec<Stmt>,
    pub is_pub: bool,
}

/// State machine declaration (F19).
/// `state PlayerState { Idle { on move => Running } Running { on stop => Idle } }`
/// Desugars to: enum + initial/handle functions
#[derive(Debug, Clone)]
pub struct StateMachineDef {
    pub name: String,
    pub states: Vec<StateDef>,
    pub is_pub: bool,
}

/// A state within a state machine (F19).
#[derive(Debug, Clone)]
pub struct StateDef {
    pub name: String,
    pub transitions: Vec<StateTransition>,
}

/// A transition within a state (F19).
/// `on move => Running` means: when event "move" is received, transition to Running
#[derive(Debug, Clone)]
pub struct StateTransition {
    pub event: String,
    pub target: String,
}

/// Event handler declaration (F20).
/// `on click(x: i64, y: i64) { ... }`
/// Desugars to: `pf __on_click(x: i64, y: i64) { ... }`
#[derive(Debug, Clone)]
pub struct EventHandlerDef {
    pub event_name: String,
    pub params: Vec<Param>,
    pub body: Vec<Stmt>,
    pub is_pub: bool,
}

/// An extern declaration for FFI: `extern "C" pf name(params) -> ret_ty;`
#[derive(Debug, Clone)]
pub struct ExternDecl {
    /// ABI convention (e.g. "C", "stdcall")
    pub abi: String,
    /// C function name (as seen by the linker)
    pub c_name: String,
    /// Fides function name (what Fides code calls)
    pub pledge_name: String,
    /// Parameter types
    pub params: Vec<Param>,
    /// Return type
    pub ret_ty: Type,
    /// Optional C header to include (e.g. "<stdio.h>")
    pub include: Option<String>,
    /// Optional library to link (e.g. "m" for libm)
    pub link_lib: Option<String>,
}
