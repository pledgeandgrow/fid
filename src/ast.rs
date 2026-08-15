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
    Named(String),
    Array { elem: Box<Type>, size: usize },
    /// Array with const generic size: [T; N] where N is a type param
    ArrayConst { elem: Box<Type>, size_param: String },
    Ref { inner: Box<Type>, is_mut: bool },
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
            Type::Named(s) => write!(f, "{}", s),
            Type::Array { elem, size } => write!(f, "[{}; {}]", elem, size),
            Type::ArrayConst { elem, size_param } => write!(f, "[{}; {}]", elem, size_param),
            Type::Ref { inner, is_mut } => {
                if *is_mut { write!(f, "&mut ")? } else { write!(f, "&")? }
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

#[derive(Debug, Clone, PartialEq)]
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
    RegionBlock {
        name: String,
        budget: Option<u64>,
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
            | Stmt::Assign { line, .. }
            | Stmt::IndexAssign { line, .. }
            | Stmt::DerefAssign { line, .. }
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
            | Stmt::ComptimeGen { line, .. }
            | Stmt::GradBlock { line, .. }
            | Stmt::Backward { line, .. } => *line,
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
}

#[derive(Debug, Clone)]
pub struct ConstDef {
    pub name: String,
    pub ty: Type,
    pub expr: Expr,
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
