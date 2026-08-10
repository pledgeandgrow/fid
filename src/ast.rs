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
    Ref { inner: Box<Type>, is_mut: bool },
    Generic { name: String, args: Vec<Type> },
    Tuple(Vec<Type>),
    Func {
        params: Vec<Type>,
        ret: Box<Type>,
    },
    /// An associated type: `T::Item` where `T` is a type parameter with a
    /// trait bound. Resolved to a concrete type during monomorphization.
    /// (type_param, trait_name, assoc_name)
    Assoc(String, String, String),
    /// A trait object type: `dyn TraitName` — enables dynamic dispatch.
    Dyn(String),
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
            Type::Assoc(tp, trait_name, name) => {
                write!(f, "{}::{}({})", tp, name, trait_name)
            }
            Type::Dyn(name) => write!(f, "dyn {}", name),
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
    StructLit {
        name: String,
        fields: Vec<(String, Expr)>,
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
            | Stmt::Do { line, .. } => *line,
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
    /// Associated type declarations: `type Item;`
    pub associated_types: Vec<String>,
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
}
