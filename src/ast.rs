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
    },
    Assign {
        target: String,
        op: AssignOp,
        expr: Expr,
    },
    IndexAssign {
        array: String,
        index: Expr,
        op: AssignOp,
        expr: Expr,
    },
    DerefAssign {
        ptr_expr: Expr,
        op: AssignOp,
        expr: Expr,
    },
    Expr(Expr),
    Return(Option<Expr>),
    If {
        cond: Expr,
        then_block: Vec<Stmt>,
        else_block: Option<Vec<Stmt>>,
    },
    While {
        cond: Expr,
        body: Vec<Stmt>,
    },
    For {
        var: String,
        iterator: Expr,
        body: Vec<Stmt>,
    },
    Loop {
        body: Vec<Stmt>,
    },
    Break(Option<Expr>),
    Continue,
    Defer(Vec<Stmt>),
    Match {
        expr: Expr,
        arms: Vec<MatchArm>,
    },
    Use { path: String, items: Vec<String> },
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
