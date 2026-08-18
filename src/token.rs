use std::fmt;

#[derive(Debug, Clone, PartialEq)]
pub enum TokenKind {
    // Keywords
    Pf,          // pf — Fides Function
    Pv,          // pv — Fides Variable
    Mut,         // mut
    Pc,          // pc — Fides Constant
    Cap,         // cap — capability/trait
    Ty,          // ty — type alias
    Ret,         // ret — return
    If,
    Else,
    Match,
    Struct,
    Enum,
    Impl,
    Pub,
    Use,         // use — import
    Module,      // module — module declaration
    With,        // with — capability grant in imports
    Async,       // async
    Await,       // await
    Comptime,    // comptime
    Defer,       // defer
    Spawn,       // spawn
    Move,        // move
    Static,      // static
    Try,         // try
    In,          // in
    As,          // as
    Dyn,         // dyn — trait object
    Where,       // where
    Region,      // region
    Sandbox,     // sandbox — capability revocation block
    Unsafe,      // unsafe — unsafe block (gates raw pointer ops)
    Txn,         // txn — transaction block with rollback
    Persist,     // persist — variable persists across hot reloads
    For,         // for
    While,       // while
    Loop,        // loop
    Break,       // break
    Continue,    // continue
    Handle,      // handle — effect handler
    Do,          // do — perform effect operation
    Interrupt,   // interrupt — interrupt handler (bare-metal)
    Extern,      // extern — FFI declarations
    Guard,       // guard — early exit: guard cond => action
    Is,          // is — type/variant check
    True,
    False,

    // ML keywords
    Layer,       // layer — neural network layer declaration
    Model,       // model — neural network model declaration
    Grad,        // grad — autograd block
    Backward,    // backward — trigger reverse-mode AD
    Kernel,      // kernel — GPU kernel keyword (used with @)

    // Types
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

    // Literals
    IntLit(i64),
    FloatLit(f64),
    StringLit(String),
    FString(String),
    Ident(String),

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Eq,
    EqEq,
    BangEq,
    Lt,
    Gt,
    LtEq,
    GtEq,
    Bang,
    And,
    Ampersand,   // & for references
    Or,
    Pipe,        // | for bitwise OR
    PipeArrow,   // |> pipeline operator
    Caret,       // ^ for bitwise XOR
    Tilde,       // ~ for bitwise NOT
    Shl,         // <<
    Shr,         // >>
    DotDot,      // ..
    DotDotEq,    // ..=
    TildeArrow,  // ~> quantization cast
    // Compound assignment
    PlusEq,
    MinusEq,
    StarEq,
    SlashEq,
    PercentEq,
    AmpEq,       // &=
    PipeEq,      // |=
    CaretEq,     // ^=
    ShlEq,       // <<=
    ShrEq,       // >>=
    Arrow,       // ->
    FatArrow,    // =>
    Dot,
    Comma,
    Colon,
    Semicolon,
    DoubleColon, // ::
    Question,    // ?
    QuestionQuestion, // ?? coalescing operator
    QuestionDot, // ?. optional chaining operator
    Hash,        // #
    At,          // @ for annotations (@kernel, @on, @serve, etc.)
    LBracket,
    RBracket,
    LParen,
    RParen,
    LBrace,
    RBrace,

    // Special
    Eof,
}

#[derive(Debug, Clone)]
pub struct Token {
    pub kind: TokenKind,
    pub line: usize,
    pub col: usize,
}

impl Token {
    pub fn new(kind: TokenKind, line: usize, col: usize) -> Self {
        Token { kind, line, col }
    }
}

impl fmt::Display for TokenKind {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            TokenKind::Pf => write!(f, "pf"),
            TokenKind::Pv => write!(f, "pv"),
            TokenKind::Mut => write!(f, "mut"),
            TokenKind::Pc => write!(f, "pc"),
            TokenKind::Cap => write!(f, "cap"),
            TokenKind::Ty => write!(f, "ty"),
            TokenKind::Ret => write!(f, "ret"),
            TokenKind::If => write!(f, "if"),
            TokenKind::Else => write!(f, "else"),
            TokenKind::Match => write!(f, "match"),
            TokenKind::Struct => write!(f, "struct"),
            TokenKind::Enum => write!(f, "enum"),
            TokenKind::Impl => write!(f, "impl"),
            TokenKind::Pub => write!(f, "pub"),
            TokenKind::Use => write!(f, "use"),
            TokenKind::Module => write!(f, "module"),
            TokenKind::With => write!(f, "with"),
            TokenKind::Async => write!(f, "async"),
            TokenKind::Await => write!(f, "await"),
            TokenKind::Comptime => write!(f, "comptime"),
            TokenKind::Defer => write!(f, "defer"),
            TokenKind::Spawn => write!(f, "spawn"),
            TokenKind::Move => write!(f, "move"),
            TokenKind::Static => write!(f, "static"),
            TokenKind::Try => write!(f, "try"),
            TokenKind::In => write!(f, "in"),
            TokenKind::As => write!(f, "as"),
            TokenKind::Dyn => write!(f, "dyn"),
            TokenKind::Where => write!(f, "where"),
            TokenKind::Region => write!(f, "region"),
            TokenKind::Sandbox => write!(f, "sandbox"),
            TokenKind::Unsafe => write!(f, "unsafe"),
            TokenKind::Txn => write!(f, "txn"),
            TokenKind::Persist => write!(f, "persist"),
            TokenKind::For => write!(f, "for"),
            TokenKind::While => write!(f, "while"),
            TokenKind::Loop => write!(f, "loop"),
            TokenKind::Break => write!(f, "break"),
            TokenKind::Continue => write!(f, "continue"),
            TokenKind::Handle => write!(f, "handle"),
            TokenKind::Do => write!(f, "do"),
            TokenKind::Interrupt => write!(f, "interrupt"),
            TokenKind::Guard => write!(f, "guard"),
            TokenKind::Is => write!(f, "is"),
            TokenKind::True => write!(f, "true"),
            TokenKind::False => write!(f, "false"),
            TokenKind::IntLit(n) => write!(f, "{}", n),
            TokenKind::StringLit(s) => write!(f, "\"{}\"", s),
            TokenKind::Ident(s) => write!(f, "{}", s),
            TokenKind::Eof => write!(f, "EOF"),
            _ => write!(f, "{:?}", self),
        }
    }
}
