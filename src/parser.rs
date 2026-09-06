use crate::ast::*;
use crate::token::{Token, TokenKind};

enum ImplOrTrait {
    StructImpl(ImplDef),
    TraitImpl(TraitImpl),
}

struct IfExpr {
    cond: Box<Expr>,
    then_block: Vec<Stmt>,
    else_block: Option<Vec<Stmt>>,
}

pub struct Parser {
    tokens: Vec<Token>,
    pos: usize,
    struct_names: Vec<String>,
    enum_names: Vec<String>,
    /// Type alias names that resolve to struct types (for struct literal parsing)
    alias_to_struct: Vec<String>,
    /// Collected parse errors for error recovery (task 2.4).
    errors: Vec<String>,
    /// Current expression nesting depth (SH-1.4 / 0.3 recursion depth audit).
    /// Incremented each time we enter a parenthesized expression.
    /// When this exceeds `max_expr_depth`, the parser switches from recursive
    /// descent to an iterative shunting-yard parser to avoid stack overflow.
    expr_depth: usize,
    /// Maximum expression nesting depth before switching to iterative parser.
    /// Default: 256 (well within the 8MB stack limit, but safe).
    max_expr_depth: usize,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens, pos: 0, struct_names: Vec::new(), enum_names: Vec::new(), alias_to_struct: Vec::new(), errors: Vec::new(), expr_depth: 0, max_expr_depth: 256 }
    }

    /// Create a parser with pre-populated struct/enum names (for stdlib auto-include)
    pub fn with_names(tokens: Vec<Token>, struct_names: Vec<String>, enum_names: Vec<String>) -> Self {
        Parser { tokens, pos: 0, struct_names, enum_names, alias_to_struct: Vec::new(), errors: Vec::new(), expr_depth: 0, max_expr_depth: 256 }
    }

    pub fn parse(&mut self) -> Result<Program, String> {
        let mut program = Program::default();

        while !self.is_at_end() {
            // Check for pub prefix
            let is_pub = if self.check(&TokenKind::Pub) {
                self.advance(); // consume pub
                true
            } else {
                false
            };

            // Parse one top-level item; on error, record it and recover.
            if let Err(e) = self.parse_top_level_item(&mut program, is_pub) {
                self.errors.push(e);
                self.recover_to_sync_token();
                // If recovery stopped at an orphaned `}`, skip past it so we
                // don't get stuck retrying the same token (task 2.4).
                if self.check(&TokenKind::RBrace) {
                    self.advance();
                }
            }
        }

        if !self.errors.is_empty() {
            return Err(self.errors.join("\n"));
        }

        Ok(program)
    }

    /// Parse a single top-level declaration and push it into `program`.
    /// Returns `Err(msg)` on parse failure so the caller can record the error
    /// and perform synchronization recovery (task 2.4).
    fn parse_top_level_item(&mut self, program: &mut Program, is_pub: bool) -> Result<(), String> {
        match self.peek() {
                TokenKind::Module => {
                    self.advance(); // consume module
                    let mut name = String::new();
                    loop {
                        let part = self.expect_ident()?;
                        name.push_str(&part);
                        if self.match_tok(TokenKind::DoubleColon) {
                            name.push_str("::");
                        } else {
                            break;
                        }
                    }
                    self.match_tok(TokenKind::Semicolon);
                    program.module = Some(name);
                }
                TokenKind::Struct => {
                    let mut s = self.parse_struct()?;
                    s.is_pub = is_pub;
                    program.structs.push(s);
                }
                TokenKind::Enum => {
                    let mut e = self.parse_enum()?;
                    e.is_pub = is_pub;
                    program.enums.push(e);
                }
                TokenKind::Const => {
                    let mut c = self.parse_const()?;
                    c.is_pub = is_pub;
                    program.consts.push(c);
                }
                TokenKind::Var => {
                    let mut g = self.parse_global_var()?;
                    g.is_pub = is_pub;
                    program.globals.push(g);
                }
                TokenKind::Static => {
                    // `static` is treated as a module-level mutable global,
                    // same as `var`. In bare-metal mode, these are placed in
                    // a fixed .data section for kernel development.
                    let mut g = self.parse_global_var()?;
                    g.is_pub = is_pub;
                    program.globals.push(g);
                }
                TokenKind::Type => {
                    let mut ta = self.parse_type_alias()?;
                    ta.is_pub = is_pub;
                    program.type_aliases.push(ta);
                }
                TokenKind::Cap => {
                    let mut t = self.parse_trait()?;
                    t.is_pub = is_pub;
                    program.traits.push(t);
                }
                TokenKind::Impl => {
                    // Check if this is `impl Trait for Type` or `impl Type`
                    let imp = self.parse_impl_or_trait_impl()?;
                    match imp {
                        ImplOrTrait::StructImpl(si) => program.impls.push(si),
                        ImplOrTrait::TraitImpl(ti) => program.trait_impls.push(ti),
                    }
                }
                TokenKind::Use => {
                    let mut imp = self.parse_use()?;
                    imp.is_pub = is_pub;
                    program.imports.push(imp);
                }
                TokenKind::Hash => {
                    let attr = self.parse_attribute()?;
                    program.attributes.push(attr);
                }
                TokenKind::At => {
                    // @annotation(s) followed by a function or struct
                    let mut annotations = Vec::new();
                    while self.check(&TokenKind::At) {
                        annotations.push(self.parse_at_annotation()?);
                    }
                    // The annotated item may have its own 'pub' prefix
                    let item_is_pub = if self.check(&TokenKind::Pub) {
                        self.advance();
                        true
                    } else {
                        is_pub
                    };
                    // Check if the annotated item is a struct or function
                    if self.check(&TokenKind::Struct) {
                        let mut sd = self.parse_struct()?;
                        sd.is_pub = item_is_pub;
                        sd.annotations = annotations;
                        program.structs.push(sd);
                    } else {
                        let mut func = self.parse_func()?;
                        func.is_pub = item_is_pub;
                        func.annotations = annotations;
                        program.funcs.push(func);
                    }
                }
                TokenKind::Layer => {
                    let mut l = self.parse_layer()?;
                    l.is_pub = is_pub;
                    program.layers.push(l);
                }
                TokenKind::Model => {
                    let mut m = self.parse_model()?;
                    m.is_pub = is_pub;
                    program.models.push(m);
                }
                TokenKind::Component => {
                    let mut c = self.parse_component()?;
                    c.is_pub = is_pub;
                    program.components.push(c);
                }
                TokenKind::System => {
                    let mut s = self.parse_system()?;
                    s.is_pub = is_pub;
                    program.systems.push(s);
                }
                TokenKind::State => {
                    let mut sm = self.parse_state_machine()?;
                    sm.is_pub = is_pub;
                    program.state_machines.push(sm);
                }
                TokenKind::On => {
                    let mut eh = self.parse_event_handler()?;
                    eh.is_pub = is_pub;
                    program.event_handlers.push(eh);
                }
                TokenKind::Comptime => {
                    self.advance(); // consume comptime
                    // comptime pf name(...) { ... }
                    let mut func = self.parse_func()?;
                    func.is_pub = is_pub;
                    func.is_comptime = true;
                    program.funcs.push(func);
                }
                TokenKind::Interrupt => {
                    self.advance(); // consume interrupt
                    // interrupt pf name(...) { ... } — bare-metal interrupt handler
                    let mut func = self.parse_func()?;
                    func.is_pub = is_pub;
                    func.is_interrupt = true;
                    program.funcs.push(func);
                }
                TokenKind::Async => {
                    self.advance(); // consume async
                    // async pf name(...) -> Future<T> { ... }
                    let mut func = self.parse_func()?;
                    func.is_pub = is_pub;
                    func.is_async = true;
                    program.funcs.push(func);
                }
                TokenKind::Extern => {
                    self.advance(); // consume extern
                    // extern "C" pf name(...);  or  extern "C" { pf name(...); ... }
                    // Also: extern "C" include "<stdio.h>" link "m"
                    let abi = if let TokenKind::StringLit(ref s) = self.peek().clone() {
                        self.advance();
                        s.clone()
                    } else {
                        String::from("C")
                    };

                    // Optional: include "<header>" and link "lib"
                    let mut include: Option<String> = None;
                    let mut link_lib: Option<String> = None;
                    loop {
                        if let TokenKind::Ident(ref kw) = self.peek().clone() {
                            if kw == "include" {
                                self.advance();
                                if let TokenKind::StringLit(ref s) = self.peek().clone() {
                                    include = Some(s.clone());
                                    self.advance();
                                }
                                continue;
                            }
                            if kw == "link" {
                                self.advance();
                                if let TokenKind::StringLit(ref s) = self.peek().clone() {
                                    link_lib = Some(s.clone());
                                    self.advance();
                                }
                                continue;
                            }
                        }
                        break;
                    }

                    if self.check(&TokenKind::LBrace) {
                        // Block of extern declarations
                        self.advance();
                        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
                            self.expect(TokenKind::Fn, "fn")?;
                            let decl = self.parse_extern_func_decl(&abi, &include, &link_lib)?;
                            program.extern_decls.push(decl);
                        }
                        self.expect(TokenKind::RBrace, "}")?;
                    } else {
                        // Single extern function
                        self.expect(TokenKind::Fn, "fn")?;
                        let decl = self.parse_extern_func_decl(&abi, &include, &link_lib)?;
                        program.extern_decls.push(decl);
                    }
                }
                _ => {
                    let mut func = self.parse_func()?;
                    func.is_pub = is_pub;
                    program.funcs.push(func);
                }
            }

        Ok(())
    }

    fn peek(&self) -> &TokenKind {
        &self.tokens[self.pos].kind
    }

    fn peek_token(&self) -> &Token {
        &self.tokens[self.pos]
    }

    fn advance(&mut self) -> Token {
        let tok = self.tokens[self.pos].clone();
        if !matches!(tok.kind, TokenKind::Eof) {
            self.pos += 1;
        }
        tok
    }

    fn is_at_end(&self) -> bool {
        matches!(self.peek(), TokenKind::Eof)
    }

    fn check(&self, kind: &TokenKind) -> bool {
        std::mem::discriminant(self.peek()) == std::mem::discriminant(kind)
    }

    fn match_tok(&mut self, kind: TokenKind) -> bool {
        if self.check(&kind) {
            self.advance();
            true
        } else {
            false
        }
    }

    /// Skip tokens until a synchronization point is reached (task 2.4).
    ///
    /// Top-level sync tokens: `pf`, `pub`, `struct`, `enum`, `impl`, `use`, `}`
    /// Statement-level sync token: `;`
    ///
    /// The `;` is consumed (statement-level recovery).  `}` and top-level
    /// keywords are left unconsumed so the caller can act on them.
    fn recover_to_sync_token(&mut self) {
        while !self.is_at_end() {
            match self.peek() {
                TokenKind::Semicolon => {
                    self.advance(); // consume ; and resume
                    return;
                }
                TokenKind::RBrace => return, // leave for caller
                TokenKind::Fn
                | TokenKind::Pub
                | TokenKind::Struct
                | TokenKind::Enum
                | TokenKind::Impl
                | TokenKind::Use => return,
                _ => {
                    self.advance();
                }
            }
        }
    }

    fn expect(&mut self, kind: TokenKind, msg: &str) -> Result<Token, String> {
        if self.check(&kind) {
            Ok(self.advance())
        } else {
            let tok = self.peek_token();
            Err(format!(
                "Parse error at {}:{}: expected {} but got '{}'",
                tok.line,
                tok.col,
                msg,
                tok.kind
            ))
        }
    }

    fn expect_ident(&mut self) -> Result<String, String> {
        let tok = self.peek_token().clone();
        if let TokenKind::Ident(s) = &tok.kind {
            self.advance();
            Ok(s.clone())
        } else {
            Err(format!(
                "Parse error at {}:{}: expected identifier but got '{}'",
                tok.line, tok.col, tok.kind
            ))
        }
    }

    /// Like expect_ident, but also accepts keywords (useful for method names
    /// like `query`, event names like `move`, etc.)
    fn expect_ident_or_keyword(&mut self) -> Result<String, String> {
        let tok = self.peek_token().clone();
        if let TokenKind::Ident(s) = &tok.kind {
            self.advance();
            return Ok(s.clone());
        }
        // Accept word-like keywords as identifiers (e.g. `match`, `query`, `state`).
        // Reject punctuation/operator tokens (e.g. `=`, `;`, `{`).
        if tok.kind.is_word_keyword() {
            let s = format!("{}", tok.kind);
            self.advance();
            Ok(s)
        } else {
            Err(format!(
                "Parse error at {}:{}: expected identifier but got '{}'",
                tok.line, tok.col, tok.kind
            ))
        }
    }

    fn is_struct_name(&self, name: &str) -> bool {
        self.struct_names.iter().any(|s| s == name)
            || self.alias_to_struct.iter().any(|s| s == name)
    }

    fn is_enum_name(&self, name: &str) -> bool {
        self.enum_names.iter().any(|s| s == name)
    }

    fn read_type_name(&mut self) -> Result<String, String> {
        let tok = self.peek_token().clone();
        let name = match &tok.kind {
            TokenKind::Ident(s) => s.clone(),
            TokenKind::I8 => "i8".to_string(),
            TokenKind::I16 => "i16".to_string(),
            TokenKind::I32 => "i32".to_string(),
            TokenKind::I64 => "i64".to_string(),
            TokenKind::I128 => "i128".to_string(),
            TokenKind::U8 => "u8".to_string(),
            TokenKind::U16 => "u16".to_string(),
            TokenKind::U32 => "u32".to_string(),
            TokenKind::U64 => "u64".to_string(),
            TokenKind::U128 => "u128".to_string(),
            TokenKind::F32 => "f32".to_string(),
            TokenKind::F64 => "f64".to_string(),
            TokenKind::Bool => "bool".to_string(),
            TokenKind::Str => "str".to_string(),
            TokenKind::Void => "void".to_string(),
            _ => return Err(format!(
                "Parse error at {}:{}: expected type name but got '{}'",
                tok.line, tok.col, tok.kind
            )),
        };
        self.advance();
        Ok(name)
    }

    fn parse_struct(&mut self) -> Result<StructDef, String> {
        self.expect(TokenKind::Struct, "struct")?;
        let name = self.expect_ident()?;

        // Generic type params: struct Foo<T, U> { ... }
        let mut type_params = Vec::new();
        if self.check(&TokenKind::Lt) {
            self.advance();
            if !self.check(&TokenKind::Gt) {
                loop {
                    let tp = self.expect_ident()?;
                    type_params.push(tp);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::Gt, ">")?;
        }

        self.expect(TokenKind::LBrace, "{")?;

        let mut fields = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let is_pub = if self.check(&TokenKind::Pub) {
                self.advance();
                true
            } else {
                false
            };
            let fname = self.expect_ident_or_keyword()?;
            self.expect(TokenKind::Colon, ":")?;
            let fty = self.parse_type()?;
            fields.push(Param { name: fname, ty: fty, is_pub, annotations: Vec::new() });
            self.match_tok(TokenKind::Comma);
        }
        self.expect(TokenKind::RBrace, "}")?;

        self.struct_names.push(name.clone());
        Ok(StructDef { name, fields, is_pub: false, annotations: Vec::new(), type_params })
    }

    /// Parse a layer declaration:
    /// `layer Dense { w: Tensor, b: Tensor, forward(self, x: Tensor) -> Tensor { ... } }`
    /// Also supports constructor args: `layer Dense(784, 128) { ... }`
    fn parse_layer(&mut self) -> Result<LayerDef, String> {
        self.expect(TokenKind::Layer, "layer")?;
        let name = self.expect_ident()?;

        // Optional constructor args: Dense(784, 128)
        let mut constructor_args = Vec::new();
        if self.match_tok(TokenKind::LParen) {
            if !self.check(&TokenKind::RParen) {
                loop {
                    let arg = self.parse_annotation_arg()?;
                    constructor_args.push(arg);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;
        }

        self.expect(TokenKind::LBrace, "{")?;

        let mut fields = Vec::new();
        let mut forward: Option<Func> = None;

        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            if self.check(&TokenKind::Fn) {
                // Parse the forward method
                let mut func = self.parse_func()?;
                if func.name == "forward" {
                    forward = Some(func);
                }
            } else {
                // Parse a field: name: Type,
                let fname = self.expect_ident_or_keyword()?;
                self.expect(TokenKind::Colon, ":")?;
                let fty = self.parse_type()?;
                fields.push(Param { name: fname, ty: fty, is_pub: false, annotations: Vec::new() });
                self.match_tok(TokenKind::Comma);
            }
        }
        self.expect(TokenKind::RBrace, "}")?;

        let forward = forward.ok_or_else(|| {
            format!("Layer '{}' must have a 'forward' method", name)
        })?;

        self.struct_names.push(name.clone());
        Ok(LayerDef {
            name,
            fields,
            constructor_args,
            forward,
            is_pub: false,
        })
    }

    /// Parse a model declaration:
    /// `model MLP { l1: Dense(784, 128), l2: Dense(128, 10), forward(self, x) -> T { ... } }`
    fn parse_model(&mut self) -> Result<ModelDef, String> {
        self.expect(TokenKind::Model, "model")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut layers = Vec::new();
        let mut forward: Option<Func> = None;

        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            if self.check(&TokenKind::Fn) {
                let mut func = self.parse_func()?;
                if func.name == "forward" {
                    forward = Some(func);
                }
            } else {
                // Layer instantiation: l1: Dense(784, 128)
                let field_name = self.expect_ident()?;
                self.expect(TokenKind::Colon, ":")?;
                let layer_type = self.expect_ident()?;

                // Optional constructor args
                let mut ctor_args = Vec::new();
                if self.match_tok(TokenKind::LParen) {
                    if !self.check(&TokenKind::RParen) {
                        loop {
                            let arg = self.parse_annotation_arg()?;
                            ctor_args.push(arg);
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::RParen, ")")?;
                }
                self.match_tok(TokenKind::Comma);
                layers.push((field_name, layer_type, ctor_args));
            }
        }
        self.expect(TokenKind::RBrace, "}")?;

        let forward = forward.ok_or_else(|| {
            format!("Model '{}' must have a 'forward' method", name)
        })?;

        self.struct_names.push(name.clone());
        Ok(ModelDef {
            name,
            layers,
            forward,
            is_pub: false,
        })
    }

    // ─── Phase 8B: Game Architecture Parsers ───────────────────────────

    /// Parse `component Name { field: type, ... }`
    fn parse_component(&mut self) -> Result<ComponentDef, String> {
        self.expect(TokenKind::Component, "component")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::LBrace, "{")?;
        let mut fields = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let field_name = self.expect_ident()?;
            self.expect(TokenKind::Colon, ":")?;
            let field_ty = self.parse_type()?;
            fields.push(Param {
                name: field_name,
                ty: field_ty,
                is_pub: false,
                annotations: Vec::new(),
            });
            self.match_tok(TokenKind::Comma);
        }
        self.expect(TokenKind::RBrace, "}")?;
        // Register component name as a struct name so struct literals work
        self.struct_names.push(name.clone());
        Ok(ComponentDef {
            name,
            fields,
            is_pub: false,
        })
    }

    /// Parse `system Name(params) { body }`
    fn parse_system(&mut self) -> Result<SystemDef, String> {
        self.expect(TokenKind::System, "system")?;
        let name = self.expect_ident()?;
        // Parse parameters
        self.expect(TokenKind::LParen, "(")?;
        let params = self.parse_params()?;
        self.expect(TokenKind::RParen, ")")?;
        // Parse body
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(SystemDef {
            name,
            params,
            body,
            is_pub: false,
        })
    }

    /// Parse `state Name { StateA { on event => Target } ... }`
    fn parse_state_machine(&mut self) -> Result<StateMachineDef, String> {
        self.expect(TokenKind::State, "state")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::LBrace, "{")?;
        let mut states = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let state_name = self.expect_ident_or_keyword()?;
            self.expect(TokenKind::LBrace, "{")?;
            let mut transitions = Vec::new();
            while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
                self.expect(TokenKind::On, "on")?;
                let event = self.expect_ident_or_keyword()?;
                self.expect(TokenKind::FatArrow, "=>")?;
                let target = self.expect_ident_or_keyword()?;
                transitions.push(StateTransition {
                    event,
                    target,
                });
                self.match_tok(TokenKind::Comma);
            }
            self.expect(TokenKind::RBrace, "}")?;
            states.push(StateDef {
                name: state_name,
                transitions,
            });
            self.match_tok(TokenKind::Comma);
        }
        self.expect(TokenKind::RBrace, "}")?;
        // Register state machine name as an enum name
        self.enum_names.push(name.clone());
        Ok(StateMachineDef {
            name,
            states,
            is_pub: false,
        })
    }

    /// Parse `on event_name(params) { body }`
    fn parse_event_handler(&mut self) -> Result<EventHandlerDef, String> {
        self.expect(TokenKind::On, "on")?;
        let event_name = self.expect_ident_or_keyword()?;
        // Parse parameters
        self.expect(TokenKind::LParen, "(")?;
        let params = self.parse_params()?;
        self.expect(TokenKind::RParen, ")")?;
        // Parse body
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(EventHandlerDef {
            event_name,
            params,
            body,
            is_pub: false,
        })
    }

    fn parse_enum(&mut self) -> Result<EnumDef, String> {
        self.expect(TokenKind::Enum, "enum")?;
        let name = self.expect_ident()?;

        // Generic type params: enum Option<T> { ... }
        let mut type_params = Vec::new();
        if self.check(&TokenKind::Lt) {
            self.advance();
            if !self.check(&TokenKind::Gt) {
                loop {
                    let tp = self.expect_ident()?;
                    type_params.push(tp);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::Gt, ">")?;
        }

        self.expect(TokenKind::LBrace, "{")?;

        let mut variants = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let vname = self.expect_ident()?;
            let data = if self.match_tok(TokenKind::LParen) {
                let mut params = Vec::new();
                if !self.check(&TokenKind::RParen) {
                    loop {
                        // Try to parse as named param: name: type
                        // Or anonymous: just type
                        let pname_or_ty = self.peek_token().clone();
                        // Check if next is ident followed by colon
                        let is_named = matches!(&pname_or_ty.kind, TokenKind::Ident(_))
                            && self.tokens.get(self.pos + 1).map_or(false, |t| t.kind == TokenKind::Colon);
                        if is_named {
                            let pname = self.expect_ident()?;
                            self.expect(TokenKind::Colon, ":")?;
                            let pty = self.parse_type()?;
                            params.push(Param { name: pname, ty: pty, is_pub: false, annotations: Vec::new() });
                        } else {
                            // Anonymous: just a type
                            let pty = self.parse_type()?;
                            let pname = format!("_{}", params.len());
                            params.push(Param { name: pname, ty: pty, is_pub: false, annotations: Vec::new() });
                        }
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::RParen, ")")?;
                Some(params)
            } else {
                None
            };
            variants.push(EnumVariant { name: vname, data });
            self.match_tok(TokenKind::Comma);
        }
        self.expect(TokenKind::RBrace, "}")?;

        self.enum_names.push(name.clone());
        Ok(EnumDef { name, variants, is_pub: false, type_params })
    }

    fn parse_const(&mut self) -> Result<ConstDef, String> {
        self.expect(TokenKind::Const, "const")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::Colon, ":")?;
        let ty = self.parse_type()?;
        self.expect(TokenKind::Eq, "=")?;
        let expr = self.parse_expr()?;
        self.match_tok(TokenKind::Semicolon);
        Ok(ConstDef { name, ty, expr, is_pub: false })
    }

    /// Parse a module-level mutable global variable:
    /// `var name: Type = expr;` or `var name: Type;` (zero-initialized)
    fn parse_global_var(&mut self) -> Result<GlobalVar, String> {
        // Accept either `var` or `static` keyword (static is an alias for var)
        if !self.match_tok(TokenKind::Var) {
            self.expect(TokenKind::Static, "var or static")?;
        }
        let name = self.expect_ident()?;
        self.expect(TokenKind::Colon, ":")?;
        let ty = self.parse_type()?;
        // Initializer is optional — defaults to zero-value of the type
        let init = if self.check(&TokenKind::Eq) {
            self.advance();
            self.parse_expr()?
        } else {
            // Zero-initializer: pick a sensible default per type
            match &ty {
                Type::I64 | Type::I32 | Type::I16 | Type::I8 | Type::U64
                | Type::U32 | Type::U16 | Type::U8 | Type::Bool => Expr::IntLit(0),
                Type::F64 | Type::F32 => Expr::FloatLit(0.0),
                Type::Str => Expr::StringLit(String::new()),
                _ => Expr::IntLit(0), // generic zero for pointers/structs
            }
        };
        self.match_tok(TokenKind::Semicolon);
        Ok(GlobalVar { name, ty, init, is_pub: false })
    }

    fn parse_type_alias(&mut self) -> Result<TypeAlias, String> {
        self.expect(TokenKind::Type, "type")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::Eq, "=")?;
        let alias_for = self.parse_type()?;
        self.match_tok(TokenKind::Semicolon);
        // If the alias points to a named type that's a struct, track it
        if let Type::Named(ref target) = alias_for {
            if self.struct_names.iter().any(|s| s == target) {
                self.alias_to_struct.push(name.clone());
            }
        }
        Ok(TypeAlias { name, alias_for, is_pub: false })
    }

    fn parse_impl_or_trait_impl(&mut self) -> Result<ImplOrTrait, String> {
        self.expect(TokenKind::Impl, "impl")?;

        // Generic type params: impl<T, U> ...
        let mut type_params = Vec::new();
        if self.check(&TokenKind::Lt) {
            self.advance(); // consume <
            if !self.check(&TokenKind::Gt) {
                loop {
                    let tp = self.expect_ident()?;
                    type_params.push(tp);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::Gt, ">")?;
        }

        let first_name = self.expect_ident()?;

        // Check for `impl Trait for Type` syntax
        if self.match_tok(TokenKind::For) {
            let trait_name = first_name;
            let type_name = self.expect_ident()?;
            self.expect(TokenKind::LBrace, "{")?;

            let mut methods = Vec::new();
            let mut associated_types = Vec::new();
            while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
                // Associated type binding: `ty Item = ConcreteType;` or GAT `ty Item<T> = ConcreteType;`
                if self.check(&TokenKind::Type) {
                    self.advance();
                    let assoc_name = self.expect_ident()?;
                    // Parse optional GAT type params: <T, U>
                    if self.check(&TokenKind::Lt) {
                        self.advance();
                        if !self.check(&TokenKind::Gt) {
                            loop {
                                self.expect_ident()?;
                                if !self.match_tok(TokenKind::Comma) {
                                    break;
                                }
                                if self.check(&TokenKind::Gt) {
                                    break;
                                }
                            }
                        }
                        self.expect(TokenKind::Gt, ">")?;
                    }
                    self.expect(TokenKind::Eq, "=")?;
                    let assoc_ty = self.parse_type()?;
                    self.match_tok(TokenKind::Semicolon);
                    associated_types.push((assoc_name, assoc_ty));
                    continue;
                }
                let is_pub = if self.check(&TokenKind::Pub) {
                    self.advance();
                    true
                } else {
                    false
                };
                let mut func = self.parse_func()?;
                func.is_pub = is_pub;
                methods.push(func);
            }
            self.expect(TokenKind::RBrace, "}")?;

            return Ok(ImplOrTrait::TraitImpl(TraitImpl {
                trait_name,
                type_name,
                type_params,
                methods,
                associated_types,
            }));
        }

        // Regular `impl Type { ... }`
        let struct_name = first_name;
        self.expect(TokenKind::LBrace, "{")?;

        let mut methods = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let is_pub = if self.check(&TokenKind::Pub) {
                self.advance();
                true
            } else {
                false
            };
            let mut func = self.parse_func()?;
            func.is_pub = is_pub;
            methods.push(func);
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(ImplOrTrait::StructImpl(ImplDef { struct_name, type_params, methods }))
    }

    fn parse_trait(&mut self) -> Result<TraitDef, String> {
        self.expect(TokenKind::Cap, "cap")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut methods = Vec::new();
        let mut associated_types = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            // Associated type declaration: `ty Item;` or GAT `ty Item<T, U>;`
            if self.check(&TokenKind::Type) {
                self.advance();
                let assoc_name = self.expect_ident()?;
                // Parse optional GAT type params: <T, U>
                let mut type_params = Vec::new();
                if self.check(&TokenKind::Lt) {
                    self.advance();
                    if !self.check(&TokenKind::Gt) {
                        loop {
                            let tp = self.expect_ident()?;
                            type_params.push(tp);
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                            // Allow trailing comma
                            if self.check(&TokenKind::Gt) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::Gt, ">")?;
                }
                self.match_tok(TokenKind::Semicolon);
                associated_types.push((assoc_name, type_params));
                continue;
            }
            let is_pub = if self.check(&TokenKind::Pub) {
                self.advance();
                true
            } else {
                false
            };
            // Parse method signature: pf name(params) -> ret_ty ;
            // or with default body: pf name(params) -> ret_ty { body }
            self.expect(TokenKind::Fn, "fn")?;
            let method_name = self.expect_ident_or_keyword()?;

            // Generic type params
            if self.check(&TokenKind::Lt) {
                self.advance();
                if !self.check(&TokenKind::Gt) {
                    loop {
                        self.expect_ident()?;
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::Gt, ">")?;
            }

            self.expect(TokenKind::LParen, "(")?;
            // Parse params, handling bare 'self' (without type annotation)
            let mut params = Vec::new();
            if !self.check(&TokenKind::RParen) {
                loop {
                    let pname = self.expect_ident()?;
                    if pname == "self" && !self.check(&TokenKind::Colon) {
                        // Bare 'self' — type will be filled in by the impl
                        params.push(Param { name: "self".to_string(), ty: Type::Named("Self".to_string()), is_pub: false, annotations: Vec::new() });
                    } else {
                        self.expect(TokenKind::Colon, ":")?;
                        let ty = self.parse_type()?;
                        params.push(Param { name: pname, ty, is_pub: false, annotations: Vec::new() });
                    }
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;

            let ret_ty = if self.match_tok(TokenKind::Arrow) {
                self.parse_type()?
            } else {
                Type::Void
            };

            let has_self = params.first().map_or(false, |p| p.name == "self");

            // Check for default body or just a signature
            let default_body = if self.check(&TokenKind::LBrace) {
                self.advance(); // consume {
                let body = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                Some(body)
            } else {
                self.match_tok(TokenKind::Semicolon);
                None
            };

            methods.push(TraitMethod {
                name: method_name,
                params,
                ret_ty,
                has_self,
                is_pub,
                default_body,
            });
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(TraitDef { name, methods, associated_types, is_pub: false })
    }

    /// Parse an extern function declaration (pf keyword already consumed)
    fn parse_extern_func_decl(&mut self, abi: &str, include: &Option<String>, link_lib: &Option<String>) -> Result<ExternDecl, String> {
        let c_name = self.expect_ident()?;
        // Optional `as pledge_name` to map C name to Fides name
        let pledge_name = if self.check(&TokenKind::As) {
            self.advance();
            self.expect_ident()?
        } else {
            c_name.clone()
        };
        self.expect(TokenKind::LParen, "(")?;
        let params = self.parse_params()?;
        self.expect(TokenKind::RParen, ")")?;
        let ret_ty = if self.match_tok(TokenKind::Arrow) {
            self.parse_type()?
        } else {
            Type::Void
        };
        self.match_tok(TokenKind::Semicolon);
        Ok(ExternDecl {
            abi: abi.to_string(),
            c_name,
            pledge_name,
            params,
            ret_ty,
            include: include.clone(),
            link_lib: link_lib.clone(),
        })
    }

    fn parse_func(&mut self) -> Result<Func, String> {
        self.expect(TokenKind::Fn, "fn")?;
        let name = self.expect_ident_or_keyword()?;

        // Generic type params: <T, U, ...> with optional trait bounds: <T: Display, U>
        // Also supports const generic params: <N: i64> where N is a comptime value
        let mut type_params = Vec::new();
        let mut type_param_bounds: Vec<(String, String)> = Vec::new();
        // Const generic params: (name, type_name) — e.g. ("N", "i64")
        let mut const_params: Vec<(String, String)> = Vec::new();
        if self.check(&TokenKind::Lt) {
            self.advance(); // consume <
            if !self.check(&TokenKind::Gt) {
                loop {
                    let tp = self.expect_ident()?;
                    // Parse bound: T: SomeCap or N: i64 (const generic)
                    if self.match_tok(TokenKind::Colon) {
                        let bound_tok = self.peek_token().clone();
                        // Check if it's a type (const generic) or a trait bound
                        let is_type = matches!(bound_tok.kind,
                            TokenKind::I8 | TokenKind::I16 | TokenKind::I32 | TokenKind::I64
                            | TokenKind::I128 | TokenKind::U8 | TokenKind::U16
                            | TokenKind::U32 | TokenKind::U64 | TokenKind::U128
                        );
                        if is_type {
                            let ty_name = match bound_tok.kind {
                                TokenKind::I8 => "i8",
                                TokenKind::I16 => "i16",
                                TokenKind::I32 => "i32",
                                TokenKind::I64 => "i64",
                                TokenKind::I128 => "i128",
                                TokenKind::U8 => "u8",
                                TokenKind::U16 => "u16",
                                TokenKind::U32 => "u32",
                                TokenKind::U64 => "u64",
                                TokenKind::U128 => "u128",
                                _ => unreachable!(),
                            };
                            self.advance();
                            const_params.push((tp.clone(), ty_name.to_string()));
                        } else {
                            let bound = self.expect_ident()?;
                            type_param_bounds.push((tp.clone(), bound));
                            // Skip additional bounds: T: Cap1 + Cap2
                            while self.match_tok(TokenKind::Plus) {
                                let _ = self.expect_ident()?;
                            }
                        }
                    }
                    type_params.push(tp);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::Gt, ">")?;
        }

        self.expect(TokenKind::LParen, "(")?;
        let params = self.parse_params()?;
        self.expect(TokenKind::RParen, ")")?;

        let ret_ty = if self.match_tok(TokenKind::Arrow) {
            self.parse_type()?
        } else {
            Type::Void
        };

        // Parse effect annotation: !IO, !{IO, Net}, !{} (pure)
        let mut has_effect_annotation = false;
        let declared_effects = if self.match_tok(TokenKind::Bang) {
            has_effect_annotation = true;
            if self.match_tok(TokenKind::LBrace) {
                // !{IO, Net, ...}
                let mut effects = Vec::new();
                if !self.check(&TokenKind::RBrace) {
                    loop {
                        let eff = self.expect_ident()?;
                        effects.push(eff);
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::RBrace, "}")?;
                effects
            } else {
                // !IO (single effect)
                vec![self.expect_ident()?]
            }
        } else {
            Vec::new()
        };

        // F10: Parse given/ensure contracts (Design by Contract)
        // Syntax: given <expr> ensure <expr> { ... }
        // Multiple conditions can be separated by commas or 'and'/'or'.
        // For simplicity, we parse one expression per given/ensure keyword.
        let mut preconditions: Vec<Expr> = Vec::new();
        let mut postconditions: Vec<Expr> = Vec::new();
        loop {
            if self.check(&TokenKind::Given) {
                self.advance();
                let expr = self.parse_expr()?;
                preconditions.push(expr);
                // Allow multiple given clauses
                continue;
            }
            if self.check(&TokenKind::Ensure) {
                self.advance();
                let expr = self.parse_expr()?;
                postconditions.push(expr);
                continue;
            }
            break;
        }

        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Func {
            name,
            type_params,
            type_param_bounds,
            const_params,
            params,
            ret_ty,
            body,
            is_pub: false,
            is_comptime: false,
            declared_effects,
            has_effect_annotation,
            is_interrupt: false,
            annotations: Vec::new(),
            preconditions,
            postconditions,
            is_async: false, // set by caller when parsing `async pf`
        })
    }

    fn parse_params(&mut self) -> Result<Vec<Param>, String> {
        let mut params = Vec::new();
        if self.check(&TokenKind::RParen) {
            return Ok(params);
        }

        loop {
            let name = self.expect_ident_or_keyword()?;
            // Handle bare 'self' (without type annotation) in impl methods
            if name == "self" && !self.check(&TokenKind::Colon) {
                params.push(Param { name: "self".to_string(), ty: Type::Named("Self".to_string()), is_pub: false, annotations: Vec::new() });
            } else {
                self.expect(TokenKind::Colon, ":")?;
                let ty = self.parse_type()?;
                params.push(Param { name, ty, is_pub: false, annotations: Vec::new() });
            }

            if !self.match_tok(TokenKind::Comma) {
                break;
            }
            // Allow trailing comma
            if self.check(&TokenKind::RParen) {
                break;
            }
        }

        Ok(params)
    }

    fn parse_type(&mut self) -> Result<Type, String> {
        let tok = self.peek_token().clone();
        let ty = match tok.kind {
            TokenKind::Fn => {
                // Function type: pf(params) -> ret
                self.advance();
                self.expect(TokenKind::LParen, "(")?;
                let mut params = Vec::new();
                if !self.check(&TokenKind::RParen) {
                    loop {
                        params.push(self.parse_type()?);
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::RParen, ")")?;
                self.expect(TokenKind::Arrow, "->")?;
                let ret = self.parse_type()?;
                return Ok(Type::Func {
                    params,
                    ret: Box::new(ret),
                });
            }
            TokenKind::I8 => Type::I8,
            TokenKind::I16 => Type::I16,
            TokenKind::I32 => Type::I32,
            TokenKind::I64 => Type::I64,
            TokenKind::I128 => Type::I128,
            TokenKind::U8 => Type::U8,
            TokenKind::U16 => Type::U16,
            TokenKind::U32 => Type::U32,
            TokenKind::U64 => Type::U64,
            TokenKind::U128 => Type::U128,
            TokenKind::F32 => Type::F32,
            TokenKind::F64 => Type::F64,
            TokenKind::Bool => Type::Bool,
            TokenKind::Str => Type::Str,
            TokenKind::Void => Type::Void,
            TokenKind::Dyn => {
                self.advance();
                let trait_name = self.expect_ident()?;
                return Ok(Type::Dyn(trait_name));
            }
            TokenKind::Ident(s) => {
                self.advance();
                // Check for inferred type: `_`
                if s == "_" {
                    return Ok(Type::Infer);
                }
                // Check for associated type: T::Item or GAT T::Item<X>
                if self.check(&TokenKind::DoubleColon) {
                    self.advance();
                    let assoc_name = self.expect_ident()?;
                    // Parse optional GAT type args: <X, Y>
                    let mut type_args = Vec::new();
                    if self.check(&TokenKind::Lt) {
                        self.advance();
                        if !self.check(&TokenKind::Gt) {
                            loop {
                                let ta = self.parse_type()?;
                                type_args.push(ta);
                                if !self.match_tok(TokenKind::Comma) {
                                    break;
                                }
                                if self.check(&TokenKind::Gt) {
                                    break;
                                }
                            }
                        }
                        self.expect(TokenKind::Gt, ">")?;
                    }
                    return Ok(Type::Assoc(s, String::new(), assoc_name, type_args));
                }
                // Check for generic type args: Name<T, U>
                if self.check(&TokenKind::Lt) {
                    self.advance(); // consume <

                    // Special case: Tensor<elem, [dims]> → ShapedTensor
                    if s == "Tensor" {
                        let elem = self.parse_type()?;
                        if self.match_tok(TokenKind::Comma) {
                            // Expect [dim, dim, ...]
                            if self.match_tok(TokenKind::LBracket) {
                                let mut shape = Vec::new();
                                if !self.check(&TokenKind::RBracket) {
                                    loop {
                                        if let TokenKind::IntLit(n) = self.peek().clone() {
                                            self.advance();
                                            shape.push(ShapeDim::Known(n));
                                        } else if let TokenKind::Ident(name) = self.peek().clone() {
                                            self.advance();
                                            shape.push(ShapeDim::Symbolic(name));
                                        } else {
                                            return Err(format!(
                                                "Expected dimension (int or identifier) in Tensor shape, got {:?}",
                                                self.peek()
                                            ));
                                        }
                                        if !self.match_tok(TokenKind::Comma) {
                                            break;
                                        }
                                    }
                                }
                                self.expect(TokenKind::RBracket, "]")?;
                                self.expect(TokenKind::Gt, ">")?;
                                return Ok(Type::ShapedTensor {
                                    elem: Box::new(elem),
                                    shape,
                                });
                            }
                        }
                        // Fallback: just Tensor<elem> without shape → Generic
                        self.expect(TokenKind::Gt, ">")?;
                        return Ok(Type::Generic {
                            name: s,
                            args: vec![elem],
                        });
                    }

                    let mut args = Vec::new();
                    if !self.check(&TokenKind::Gt) {
                        loop {
                            args.push(self.parse_type()?);
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::Gt, ">")?;
                    return Ok(Type::Generic { name: s, args });
                }
                return Ok(Type::Named(s));
            }
            TokenKind::LParen => {
                // Tuple type: (T, U, ...) or unit ()
                self.advance();
                let mut types = Vec::new();
                if !self.check(&TokenKind::RParen) {
                    loop {
                        types.push(self.parse_type()?);
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::RParen, ")")?;
                if types.len() == 1 {
                    return Ok(types.into_iter().next().unwrap());
                }
                return Ok(Type::Tuple(types));
            }
            TokenKind::Ampersand => {
                self.advance();
                let is_mut = self.match_tok(TokenKind::Mut);
                let inner = self.parse_type()?;
                return Ok(Type::Ref { inner: Box::new(inner), is_mut });
            }
            TokenKind::Star => {
                // Raw pointer type: *mut T or *const T
                self.advance();
                let is_mut = if self.check(&TokenKind::Mut) {
                    self.advance();
                    true
                } else if self.check(&TokenKind::Const) {
                    self.advance();
                    false
                } else {
                    // *T defaults to *const T
                    false
                };
                let inner = self.parse_type()?;
                return Ok(Type::RawPtr { inner: Box::new(inner), is_mut });
            }
            TokenKind::LBracket => {
                self.advance();
                let elem = self.parse_type()?;
                self.expect(TokenKind::Semicolon, ";")?;
                let size_tok = self.peek_token().clone();
                let ty = match &size_tok.kind {
                    TokenKind::IntLit(n) => {
                        self.advance();
                        Type::Array { elem: Box::new(elem), size: *n as usize }
                    }
                    TokenKind::Ident(name) => {
                        // Const generic size: [T; N] where N is a type param
                        self.advance();
                        Type::ArrayConst { elem: Box::new(elem), size_param: name.clone() }
                    }
                    _ => return Err(format!(
                        "Parse error at {}:{}: expected array size but got '{}'",
                        size_tok.line, size_tok.col, size_tok.kind
                    )),
                };
                self.expect(TokenKind::RBracket, "]")?;
                return Ok(ty);
            }
            _ => {
                return Err(format!(
                    "Parse error at {}:{}: expected type but got '{}'",
                    tok.line, tok.col, tok.kind
                ))
            }
        };
        self.advance();
        Ok(ty)
    }

    fn parse_block(&mut self) -> Result<Vec<Stmt>, String> {
        let mut stmts = Vec::new();

        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            match self.parse_stmt() {
                Ok(stmt) => stmts.push(stmt),
                Err(e) => {
                    // Statement-level recovery: record the error, skip to the
                    // next `;` or `}`, and continue parsing the block (task 2.4).
                    self.errors.push(e);
                    self.recover_to_sync_token();
                }
            }
        }

        Ok(stmts)
    }

    fn parse_stmt(&mut self) -> Result<Stmt, String> {
        match self.peek() {
            TokenKind::Persist => self.parse_persist_let(),
            TokenKind::Val => self.parse_let(),
            TokenKind::Var => self.parse_let(),
            TokenKind::Return => self.parse_return(),
            TokenKind::Query => self.parse_query_stmt(),
            TokenKind::If => {
                let line = self.peek_token().line;
                let expr = self.parse_if_expr()?;
                Ok(Stmt::If {
                    cond: *expr.cond,
                    then_block: expr.then_block,
                    else_block: expr.else_block,
                    line,
                })
            }
            TokenKind::While => self.parse_while(),
            TokenKind::For => self.parse_for(),
            TokenKind::Loop => self.parse_loop(),
            TokenKind::Break => {
                let line = self.peek_token().line;
                self.advance();
                if self.check(&TokenKind::Semicolon) || self.check(&TokenKind::RBrace) {
                    self.match_tok(TokenKind::Semicolon);
                    Ok(Stmt::Break(None, line))
                } else {
                    let expr = self.parse_expr()?;
                    self.match_tok(TokenKind::Semicolon);
                    Ok(Stmt::Break(Some(expr), line))
                }
            }
            TokenKind::Continue => {
                let line = self.peek_token().line;
                self.advance();
                self.match_tok(TokenKind::Semicolon);
                Ok(Stmt::Continue(line))
            }
            TokenKind::Defer => {
                let line = self.peek_token().line;
                self.advance();
                self.expect(TokenKind::LBrace, "{")?;
                let body = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                Ok(Stmt::Defer(body, line))
            }
            TokenKind::Match => self.parse_match_stmt(),
            TokenKind::Select => self.parse_select_stmt(),
            TokenKind::Use => self.parse_use_stmt(),
            TokenKind::Handle => self.parse_handle_stmt(),
            TokenKind::Do => self.parse_do_stmt(),
            TokenKind::Region => self.parse_region_block(),
            TokenKind::Sandbox => self.parse_sandbox_block(),
            TokenKind::Unsafe => self.parse_unsafe_block(),
            TokenKind::Txn => self.parse_txn_block(),
            TokenKind::Grad => self.parse_grad_block(),
            TokenKind::Backward => {
                let line = self.peek_token().line;
                self.advance(); // consume backward
                self.expect(TokenKind::LParen, "(")?;
                let loss = self.parse_expr()?;
                self.expect(TokenKind::RParen, ")")?;
                self.match_tok(TokenKind::Semicolon);
                Ok(Stmt::Backward { loss, line })
            }
            TokenKind::Comptime => {
                let line = self.peek_token().line;
                self.advance(); // consume comptime
                // comptime gen { block } — comptime code generation
                if let TokenKind::Ident(ref name) = self.peek().clone() {
                    if name == "gen" {
                        self.advance(); // consume gen
                        self.expect(TokenKind::LBrace, "{")?;
                        let block = self.parse_block()?;
                        self.expect(TokenKind::RBrace, "}")?;
                        return Ok(Stmt::ComptimeGen { block, line });
                    }
                }
                // Fall through to expression-level comptime parsing
                let expr = self.parse_comptime_expr(line)?;
                Ok(Stmt::Expr(expr, line))
            }
            TokenKind::Guard => self.parse_guard_stmt(),
            _ => self.parse_assign_or_expr(),
        }
    }

    fn parse_assign_or_expr(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        // Try to parse as expression first
        let expr = self.parse_expr()?;

        // Check if this is an assignment: ident op= expr
        let assign_op = match self.peek() {
            TokenKind::Eq => Some(AssignOp::Assign),
            TokenKind::PlusEq => Some(AssignOp::AddAssign),
            TokenKind::MinusEq => Some(AssignOp::SubAssign),
            TokenKind::StarEq => Some(AssignOp::MulAssign),
            TokenKind::SlashEq => Some(AssignOp::DivAssign),
            TokenKind::PercentEq => Some(AssignOp::ModAssign),
            TokenKind::AmpEq => Some(AssignOp::BitAndAssign),
            TokenKind::PipeEq => Some(AssignOp::BitOrAssign),
            TokenKind::CaretEq => Some(AssignOp::BitXorAssign),
            TokenKind::ShlEq => Some(AssignOp::ShlAssign),
            TokenKind::ShrEq => Some(AssignOp::ShrAssign),
            _ => None,
        };

        if let Some(op) = assign_op {
            self.advance(); // consume the compound assignment operator
            if let Expr::Ident(name) = expr {
                let rhs = self.parse_expr()?;
                self.match_tok(TokenKind::Semicolon);
                Ok(Stmt::Assign { target: name, op, expr: rhs, line })
            } else if let Expr::Index { array, index } = expr {
                if let Expr::Ident(name) = *array {
                    let rhs = self.parse_expr()?;
                    self.match_tok(TokenKind::Semicolon);
                    Ok(Stmt::IndexAssign { array: name, index: *index, op, expr: rhs, line })
                } else {
                    Err("Parse error: index assignment target must be an array variable".to_string())
                }
            } else if let Expr::Deref { expr: ptr_expr } = expr {
                let rhs = self.parse_expr()?;
                self.match_tok(TokenKind::Semicolon);
                Ok(Stmt::DerefAssign { ptr_expr: *ptr_expr, op, expr: rhs, line })
            } else if let Expr::FieldAccess { expr: obj_expr, field } = expr {
                let rhs = self.parse_expr()?;
                self.match_tok(TokenKind::Semicolon);
                Ok(Stmt::FieldAssign { object: *obj_expr, field, op, expr: rhs, line })
            } else {
                Err("Parse error: assignment target must be an identifier or array index".to_string())
            }
        } else {
            self.match_tok(TokenKind::Semicolon);
            Ok(Stmt::Expr(expr, line))
        }
    }

    fn parse_while(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::While, "while")?;
        let cond = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::While { cond, body, line })
    }

    /// Parse `guard cond => action` — early exit if condition is false.
    /// Desugars to `if !cond { action }`.
    /// The action can be a block `{ ... }` or a single statement.
    fn parse_guard_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Guard, "guard")?;
        let cond = self.parse_expr()?;
        self.expect(TokenKind::FatArrow, "=>")?;

        // The action can be a block or a single statement
        let then_block = if self.check(&TokenKind::LBrace) {
            self.advance(); // consume {
            let block = self.parse_block()?;
            self.expect(TokenKind::RBrace, "}")?;
            block
        } else {
            vec![self.parse_stmt()?]
        };

        // Desugar: guard cond => action  →  if !cond { action }
        Ok(Stmt::If {
            cond: Expr::Unary {
                op: UnaryOp::Not,
                expr: Box::new(cond),
            },
            then_block,
            else_block: None,
            line,
        })
    }

    fn parse_for(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::For, "for")?;
        let var = self.expect_ident()?;
        self.expect(TokenKind::In, "in")?;
        let iterator = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::For { var, iterator, body, line })
    }

    fn parse_loop(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Loop, "loop")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::Loop { body, line })
    }

    /// Parse `query(ComponentA, ComponentB, ...) { body }`
    fn parse_query_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Query, "query")?;
        self.expect(TokenKind::LParen, "(")?;
        let mut components = Vec::new();
        if !self.check(&TokenKind::RParen) {
            loop {
                let comp_name = self.expect_ident()?;
                components.push(comp_name);
                if !self.match_tok(TokenKind::Comma) {
                    break;
                }
            }
        }
        self.expect(TokenKind::RParen, ")")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::Query { components, body, line })
    }

    fn parse_match_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Match, "match")?;
        let expr = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut arms = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let pattern = self.parse_pattern()?;
            // Optional guard: `pattern if cond =>`
            let guard = if self.check(&TokenKind::If) {
                self.advance();
                Some(self.parse_expr()?)
            } else {
                None
            };
            self.expect(TokenKind::FatArrow, "=>")?;
            self.expect(TokenKind::LBrace, "{")?;
            let body = self.parse_block()?;
            self.expect(TokenKind::RBrace, "}")?;
            self.match_tok(TokenKind::Comma);
            arms.push(MatchArm { pattern, guard, body });
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Stmt::Match { expr, arms, line })
    }

    /// Parse a select statement: `select { case ch => x => { body } default => { body } }`
    fn parse_select_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Select, "select")?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut cases = Vec::new();
        let mut default = None;

        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            // Expect `case` or `default`
            let ident = self.expect_ident()?;
            if ident == "default" {
                self.expect(TokenKind::FatArrow, "=>")?;
                self.expect(TokenKind::LBrace, "{")?;
                let body = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                default = Some(body);
                self.match_tok(TokenKind::Comma);
            } else if ident == "case" {
                // case ch => var => { body }
                let channel = self.parse_expr()?;
                self.expect(TokenKind::FatArrow, "=>")?;
                let var_name = self.expect_ident()?;
                self.expect(TokenKind::FatArrow, "=>")?;
                self.expect(TokenKind::LBrace, "{")?;
                let body = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                self.match_tok(TokenKind::Comma);
                cases.push(SelectCase { channel, var_name, body });
            } else {
                return Err(format!("Expected 'case' or 'default' in select, got '{}'", ident));
            }
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Stmt::Select { cases, default, line })
    }

    fn parse_match_expr(&mut self) -> Result<Expr, String> {
        self.expect(TokenKind::Match, "match")?;
        let expr = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut arms = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let pattern = self.parse_pattern()?;
            // Optional guard: `pattern if cond =>`
            let guard = if self.check(&TokenKind::If) {
                self.advance();
                Some(self.parse_expr()?)
            } else {
                None
            };
            self.expect(TokenKind::FatArrow, "=>")?;
            // Arm body: either { block } or single expression
            let body = if self.check(&TokenKind::LBrace) {
                self.advance(); // consume {
                let block = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                block
            } else {
                let line = self.peek_token().line;
                let e = self.parse_expr()?;
                vec![Stmt::Expr(e, line)]
            };
            self.match_tok(TokenKind::Comma);
            arms.push(MatchArm { pattern, guard, body });
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Expr::Match { expr: Box::new(expr), arms })
    }

    fn parse_pattern(&mut self) -> Result<Pattern, String> {
        let tok = self.peek_token().clone();
        match &tok.kind {
            TokenKind::IntLit(n) => {
                self.advance();
                // Check for range pattern: 1..=3 or 1..5
                if self.check(&TokenKind::DotDotEq) {
                    self.advance();
                    let end_tok = self.peek_token().clone();
                    if let TokenKind::IntLit(m) = end_tok.kind {
                        self.advance();
                        // Inclusive range: 1..=3 means [1,3]. Lowering uses [lo,hi)
                        // so we store end+1 to make it exclusive.
                        return Ok(Pattern::Range {
                            start: Box::new(Pattern::IntLit(*n)),
                            end: Box::new(Pattern::IntLit(m + 1)),
                        });
                    }
                    return Err(format!("Parse error at {}:{}: expected int after ..=", end_tok.line, end_tok.col));
                }
                if self.check(&TokenKind::DotDot) {
                    self.advance();
                    let end_tok = self.peek_token().clone();
                    if let TokenKind::IntLit(m) = end_tok.kind {
                        self.advance();
                        // Exclusive range: 1..5 means [1,5). Lowering uses [lo,hi)
                        // so we keep end as-is.
                        return Ok(Pattern::Range {
                            start: Box::new(Pattern::IntLit(*n)),
                            end: Box::new(Pattern::IntLit(m)),
                        });
                    }
                    return Err(format!("Parse error at {}:{}: expected int after ..", end_tok.line, end_tok.col));
                }
                Ok(Pattern::IntLit(*n))
            }
            TokenKind::StringLit(s) => {
                self.advance();
                Ok(Pattern::StrLit(s.clone()))
            }
            TokenKind::True => {
                self.advance();
                Ok(Pattern::BoolLit(true))
            }
            TokenKind::False => {
                self.advance();
                Ok(Pattern::BoolLit(false))
            }
            TokenKind::Ident(name) => {
                self.advance();
                if name == "_" {
                    return Ok(Pattern::Wildcard);
                }
                if self.check(&TokenKind::DoubleColon) {
                    self.advance();
                    let variant = self.expect_ident()?;
                    let mut bindings = Vec::new();
                    if self.match_tok(TokenKind::LParen) {
                        loop {
                            let bname = self.expect_ident()?;
                            // Type annotation is optional: `x: Type` or just `x`
                            let bty = if self.match_tok(TokenKind::Colon) {
                                self.read_type_name()?
                            } else {
                                String::new() // empty = infer type
                            };
                            bindings.push((bname, bty));
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                        self.expect(TokenKind::RParen, ")")?;
                    }
                    Ok(Pattern::EnumVariant {
                        enum_name: name.clone(),
                        variant,
                        bindings,
                    })
                } else if self.check(&TokenKind::LParen) {
                    // Shorthand: Some(x) or Some(x: i64) without EnumName:: prefix
                    self.advance(); // consume (
                    let mut bindings = Vec::new();
                    loop {
                        let bname = self.expect_ident()?;
                        // Type annotation is optional: `x: Type` or just `x`
                        let bty = if self.match_tok(TokenKind::Colon) {
                            self.read_type_name()?
                        } else {
                            String::new() // empty = infer type
                        };
                        bindings.push((bname, bty));
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                    self.expect(TokenKind::RParen, ")")?;
                    Ok(Pattern::EnumVariant {
                        enum_name: String::new(), // empty = infer enum
                        variant: name.clone(),
                        bindings,
                    })
                } else {
                    Ok(Pattern::Ident(name.clone()))
                }
            }
            _ => Err(format!(
                "Parse error at {}:{}: invalid pattern '{}'",
                tok.line, tok.col, tok.kind
            )),
        }
    }

    fn parse_let(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        // Accept either `val` (immutable) or `var` (mutable)
        let kw = self.peek();
        let is_mut = match kw {
            TokenKind::Val => { self.advance(); false }
            TokenKind::Var => { self.advance(); true }
            _ => return Err(format!("expected 'val' or 'var' at line {}", line)),
        };

        // Check for tuple destructuring: `val (a, b) = expr`
        if self.check(&TokenKind::LParen) {
            // Peek ahead to see if this is a tuple pattern (ident, ident, ...)
            // vs a tuple type annotation (which would be `pv name: (i64, i64) = ...`)
            // Since we already consumed `pv` and there's no name yet, `(` means destructuring.
            self.advance(); // consume (
            let mut names = Vec::new();
            if !self.check(&TokenKind::RParen) {
                loop {
                    let name = self.expect_ident_or_keyword()?;
                    names.push(name);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;
            self.expect(TokenKind::Eq, "=")?;
            let expr = self.parse_expr()?;
            self.match_tok(TokenKind::Semicolon);
            return Ok(Stmt::LetTuple { names, expr, line });
        }

        let name = self.expect_ident_or_keyword()?;

        let ty = if self.match_tok(TokenKind::Colon) {
            Some(self.parse_type()?)
        } else {
            None
        };

        // `var x: i32;` without `=` → uninitialized mutable variable
        if is_mut && self.check(&TokenKind::Semicolon) {
            self.advance(); // consume ;
            // ty must be Some for uninit vars
            let ty = ty.ok_or_else(|| format!("var {} needs type annotation for uninitialized declaration at line {}", name, line))?;
            return Ok(Stmt::LetUninit { name, ty, line });
        }
        // Also handle `var x: i32` without semicolon (end of block etc.)
        if is_mut && !self.check(&TokenKind::Eq) {
            // No initializer — uninitialized variable
            let ty = ty.ok_or_else(|| format!("var {} needs type annotation for uninitialized declaration at line {}", name, line))?;
            self.match_tok(TokenKind::Semicolon);
            return Ok(Stmt::LetUninit { name, ty, line });
        }

        self.expect(TokenKind::Eq, "=")?;
        let expr = self.parse_expr()?;
        self.match_tok(TokenKind::Semicolon);

        // Check for `persist` modifier after the statement
        // Syntax: `val name = expr; persist` or `persist val name = expr;`
        // We use the prefix form: `persist val name = expr;`
        // This is handled in parse_stmt by checking for Persist token before Val
        Ok(Stmt::Let { name, ty, expr, is_mut, is_persist: false, line })
    }

    /// Parse `var name: Type;` — uninitialized mutable variable (F63: init guarantee).
    /// The variable must be assigned before it's read; the typechecker enforces this.
    /// Note: parse_let() now handles this case internally, but this function is kept
    /// for potential direct use.
    fn parse_let_uninit(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Var, "var")?;
        let name = self.expect_ident_or_keyword()?;
        self.expect(TokenKind::Colon, ":")?;
        let ty = self.parse_type()?;
        self.match_tok(TokenKind::Semicolon);
        Ok(Stmt::LetUninit { name, ty, line })
    }

    /// Parse `persist val name = expr;` or `persist var name = expr;` — variable that survives hot reloads.
    fn parse_persist_let(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Persist, "persist")?;
        // Accept either `val` (immutable) or `var` (mutable)
        let is_mut = if self.check(&TokenKind::Var) {
            self.advance();
            true
        } else {
            self.expect(TokenKind::Val, "val")?;
            false
        };
        let name = self.expect_ident_or_keyword()?;

        let ty = if self.match_tok(TokenKind::Colon) {
            Some(self.parse_type()?)
        } else {
            None
        };

        self.expect(TokenKind::Eq, "=")?;
        let expr = self.parse_expr()?;
        self.match_tok(TokenKind::Semicolon);

        Ok(Stmt::Let { name, ty, expr, is_mut, is_persist: true, line })
    }

    fn parse_return(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Return, "return")?;
        if self.check(&TokenKind::RBrace) || self.check(&TokenKind::Semicolon) {
            self.match_tok(TokenKind::Semicolon);
            Ok(Stmt::Return(None, line))
        } else {
            let expr = self.parse_expr()?;
            self.match_tok(TokenKind::Semicolon);
            Ok(Stmt::Return(Some(expr), line))
        }
    }

    fn parse_if_expr(&mut self) -> Result<IfExpr, String> {
        self.expect(TokenKind::If, "if")?;
        let cond = self.parse_expr()?;

        self.expect(TokenKind::LBrace, "{")?;
        let then_block = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;

        let else_block: Option<Vec<Stmt>> = if self.match_tok(TokenKind::Else) {
            if self.check(&TokenKind::If) {
                let line = self.peek_token().line;
                let nested = self.parse_if_expr()?;
                Some(vec![Stmt::If {
                    cond: *nested.cond,
                    then_block: nested.then_block,
                    else_block: nested.else_block,
                    line,
                }])
            } else {
                self.expect(TokenKind::LBrace, "{")?;
                let block = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                Some(block)
            }
        } else {
            None
        };

        Ok(IfExpr {
            cond: Box::new(cond),
            then_block,
            else_block,
        })
    }

    fn parse_expr(&mut self) -> Result<Expr, String> {
        self.parse_pipe_forward()
    }

    /// Parse pipe-forward operator `|>` — lowest precedence binary operator.
    /// `x |> f` desugars to `f(x)`, `x |> obj.method()` desugars to `obj.method(x)`.
    fn parse_pipe_forward(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_coalesce()?;
        while self.match_tok(TokenKind::PipeArrow) {
            let right = self.parse_coalesce()?;
            left = Expr::PipeForward {
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    /// Parse coalescing operator `??` — `expr ?? default`.
    /// If expr is None, return default; else unwrap and return the value.
    fn parse_coalesce(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_range()?;
        while self.match_tok(TokenKind::QuestionQuestion) {
            let right = self.parse_range()?;
            left = Expr::Coalesce {
                expr: Box::new(left),
                default: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_range(&mut self) -> Result<Expr, String> {
        let left = self.parse_or()?;
        if self.check(&TokenKind::DotDot) || self.check(&TokenKind::DotDotEq) {
            let inclusive = self.check(&TokenKind::DotDotEq);
            self.advance();
            let right = self.parse_or()?;
            Ok(Expr::Range {
                start: Box::new(left),
                end: Box::new(right),
                inclusive,
            })
        } else {
            Ok(left)
        }
    }

    fn parse_or(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_bit_or()?;
        while self.match_tok(TokenKind::Or) {
            let right = self.parse_bit_or()?;
            left = Expr::Binary {
                op: BinOp::Or,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_bit_or(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_bit_xor()?;
        while self.match_tok(TokenKind::Pipe) {
            let right = self.parse_bit_xor()?;
            left = Expr::Binary {
                op: BinOp::BitOr,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_bit_xor(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_bit_and()?;
        while self.match_tok(TokenKind::Caret) {
            let right = self.parse_bit_and()?;
            left = Expr::Binary {
                op: BinOp::BitXor,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_bit_and(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_shift()?;
        while self.match_tok(TokenKind::Ampersand) {
            let right = self.parse_shift()?;
            left = Expr::Binary {
                op: BinOp::BitAnd,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_shift(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_and()?;
        loop {
            let op = match self.peek() {
                TokenKind::Shl => BinOp::Shl,
                TokenKind::Shr => BinOp::Shr,
                _ => break,
            };
            self.advance();
            let right = self.parse_and()?;
            left = Expr::Binary {
                op,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_and(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_equality()?;
        while self.match_tok(TokenKind::And) {
            let right = self.parse_equality()?;
            left = Expr::Binary {
                op: BinOp::And,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }


    fn parse_equality(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_comparison()?;
        loop {
            let op = match self.peek() {
                TokenKind::EqEq => BinOp::Eq,
                TokenKind::BangEq => BinOp::Neq,
                _ => break,
            };
            self.advance();
            let right = self.parse_comparison()?;
            left = Expr::Binary {
                op,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_comparison(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_addition()?;
        // Check for `is` type/variant check
        if self.match_tok(TokenKind::Is) {
            // Accept both identifiers and type keyword tokens
            let peek_clone = self.peek().clone();
            let type_name = match &peek_clone {
                TokenKind::Ident(name) => {
                    self.advance();
                    name.clone()
                }
                TokenKind::I8 => { self.advance(); "i8".to_string() }
                TokenKind::I16 => { self.advance(); "i16".to_string() }
                TokenKind::I32 => { self.advance(); "i32".to_string() }
                TokenKind::I64 => { self.advance(); "i64".to_string() }
                TokenKind::I128 => { self.advance(); "i128".to_string() }
                TokenKind::U8 => { self.advance(); "u8".to_string() }
                TokenKind::U16 => { self.advance(); "u16".to_string() }
                TokenKind::U32 => { self.advance(); "u32".to_string() }
                TokenKind::U64 => { self.advance(); "u64".to_string() }
                TokenKind::U128 => { self.advance(); "u128".to_string() }
                TokenKind::F32 => { self.advance(); "f32".to_string() }
                TokenKind::F64 => { self.advance(); "f64".to_string() }
                TokenKind::Bool => { self.advance(); "bool".to_string() }
                TokenKind::Str => { self.advance(); "str".to_string() }
                TokenKind::Void => { self.advance(); "void".to_string() }
                _ => return Err(format!("expected type name after 'is' but got {:?}", peek_clone)),
            };
            return Ok(Expr::IsCheck {
                expr: Box::new(left),
                type_name,
            });
        }
        loop {
            let op = match self.peek() {
                TokenKind::Lt => BinOp::Lt,
                TokenKind::Gt => BinOp::Gt,
                TokenKind::LtEq => BinOp::LtEq,
                TokenKind::GtEq => BinOp::GtEq,
                _ => break,
            };
            self.advance();
            let right = self.parse_addition()?;
            left = Expr::Binary {
                op,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_addition(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_multiplication()?;
        loop {
            let op = match self.peek() {
                TokenKind::Plus => BinOp::Add,
                TokenKind::Minus => BinOp::Sub,
                _ => break,
            };
            self.advance();
            let right = self.parse_multiplication()?;
            left = Expr::Binary {
                op,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_multiplication(&mut self) -> Result<Expr, String> {
        let mut left = self.parse_unary()?;
        loop {
            let op = match self.peek() {
                TokenKind::Star => BinOp::Mul,
                TokenKind::Slash => BinOp::Div,
                TokenKind::Percent => BinOp::Mod,
                _ => break,
            };
            self.advance();
            let right = self.parse_unary()?;
            left = Expr::Binary {
                op,
                left: Box::new(left),
                right: Box::new(right),
            };
        }
        Ok(left)
    }

    fn parse_unary(&mut self) -> Result<Expr, String> {
        match self.peek() {
            TokenKind::Minus => {
                self.advance();
                let expr = self.parse_unary()?;
                Ok(Expr::Unary {
                    op: UnaryOp::Neg,
                    expr: Box::new(expr),
                })
            }
            TokenKind::Bang => {
                self.advance();
                let expr = self.parse_unary()?;
                Ok(Expr::Unary {
                    op: UnaryOp::Not,
                    expr: Box::new(expr),
                })
            }
            TokenKind::Tilde => {
                self.advance();
                let expr = self.parse_unary()?;
                Ok(Expr::Unary {
                    op: UnaryOp::BitNot,
                    expr: Box::new(expr),
                })
            }
            TokenKind::Ampersand => {
                self.advance();
                let is_mut = self.match_tok(TokenKind::Mut);
                let expr = self.parse_unary()?;
                Ok(Expr::Ref {
                    expr: Box::new(expr),
                    is_mut,
                })
            }
            TokenKind::Star => {
                // Deref: *expr
                self.advance();
                let expr = self.parse_unary()?;
                Ok(Expr::Deref {
                    expr: Box::new(expr),
                })
            }
            TokenKind::Comptime => {
                self.advance();
                return self.parse_comptime_expr(0);
            }
            _ => self.parse_postfix(),
        }
    }

    /// Parse comptime expression (comptime keyword already consumed)
    fn parse_comptime_expr(&mut self, _line: usize) -> Result<Expr, String> {
        // comptime { block } — comptime block expression
        if self.check(&TokenKind::LBrace) {
            self.advance();
            let mut stmts = Vec::new();
            while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
                stmts.push(self.parse_stmt()?);
            }
            self.expect(TokenKind::RBrace, "}")?;
            self.match_tok(TokenKind::Semicolon);
            return Ok(Expr::ComptimeBlock(stmts));
        }
        // comptime reflect_op(Type) — type reflection
        if let TokenKind::Ident(ref name) = self.peek().clone() {
            let reflect_op = match name.as_str() {
                "type_name" => Some(ReflectOp::TypeName),
                "is_struct" => Some(ReflectOp::IsStruct),
                "is_enum" => Some(ReflectOp::IsEnum),
                "field_count" => Some(ReflectOp::FieldCount),
                "field_names" => Some(ReflectOp::FieldNames),
                "field_types" => Some(ReflectOp::FieldTypes),
                "enum_variants" => Some(ReflectOp::EnumVariants),
                _ => None,
            };
            if let Some(op) = reflect_op {
                self.advance();
                self.expect(TokenKind::LParen, "(")?;
                let ty = self.parse_type()?;
                self.expect(TokenKind::RParen, ")")?;
                self.match_tok(TokenKind::Semicolon);
                return Ok(Expr::ComptimeReflect { op, ty });
            }
        }
        // comptime <expr> — comptime expression
        let inner = self.parse_unary()?;
        self.match_tok(TokenKind::Semicolon);
        Ok(Expr::Comptime(Box::new(inner)))
    }

    fn parse_postfix(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_primary()?;
        loop {
            if self.match_tok(TokenKind::Dot) {
                let field = self.expect_ident_or_keyword()?;
                if self.match_tok(TokenKind::LParen) {
                    // Method call: receiver.method(args)
                    let mut args = Vec::new();
                    if !self.check(&TokenKind::RParen) {
                        loop {
                            args.push(self.parse_expr()?);
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::RParen, ")")?;
                    expr = Expr::MethodCall {
                        receiver: Box::new(expr),
                        method: field,
                        args,
                    };
                } else {
                    expr = Expr::FieldAccess {
                        expr: Box::new(expr),
                        field,
                    };
                }
            } else if self.match_tok(TokenKind::QuestionDot) {
                // Optional field access: expr?.field
                let field = self.expect_ident()?;
                expr = Expr::OptionalFieldAccess {
                    expr: Box::new(expr),
                    field,
                };
            } else if self.match_tok(TokenKind::LBracket) {
                // Array index: arr[idx]
                let index = self.parse_expr()?;
                self.expect(TokenKind::RBracket, "]")?;
                expr = Expr::Index {
                    array: Box::new(expr),
                    index: Box::new(index),
                };
            } else if self.match_tok(TokenKind::Question) {
                // Try operator: expr?
                expr = Expr::Try {
                    expr: Box::new(expr),
                };
            } else if self.match_tok(TokenKind::As) {
                // Cast: expr as Type
                let target_ty = self.parse_type()?;
                expr = Expr::Cast {
                    expr: Box::new(expr),
                    target_ty,
                };
            } else if self.match_tok(TokenKind::TildeArrow) {
                // Quantization cast: expr ~> q8
                let target_ty = self.parse_type()?;
                expr = Expr::QuantCast {
                    expr: Box::new(expr),
                    target_ty,
                };
            } else {
                break;
            }
        }
        Ok(expr)
    }

    /// SH-1.4 / 0.3: Iterative expression parser using shunting-yard algorithm.
    ///
    /// This is used as a fallback when expression nesting depth exceeds
    /// `max_expr_depth` (default 256). It handles binary operators and
    /// nested parentheses without recursion, preventing stack overflow on
    /// deeply nested expressions like `((((1 + 2) + 3) + 4) ...)`.
    ///
    /// Handles: binary operators (all precedences), parenthesized grouping,
    /// tuples, and delegates non-paren primaries to `parse_primary`.
    /// Unary operators are handled by delegating to `parse_primary` (which
    /// calls `parse_unary` → `parse_postfix` → `parse_primary`).
    /// When `parse_primary` sees `(`, it checks depth and may call this
    /// function again — but only for the content inside the parens, which
    /// is handled iteratively by the shunting-yard loop.
    fn parse_expr_iterative(&mut self) -> Result<Expr, String> {
        // Operator stack entry: either a binary operator with its precedence,
        // or a parenthesis marker.
        enum OpEntry {
            Op(BinOp, u8),
            Paren,
        }

        let mut vals: Vec<Expr> = Vec::new();
        let mut ops: Vec<OpEntry> = Vec::new();

        // Helper: reduce the top of the operator stack by one binary op.
        // Pops one operator and two operands, pushes the result.
        macro_rules! reduce_one {
            () => {{
                let op_entry = ops.pop().unwrap();
                if let OpEntry::Op(bop, _) = op_entry {
                    let right = vals.pop().unwrap();
                    let left = vals.pop().unwrap();
                    vals.push(Expr::Binary {
                        op: bop,
                        left: Box::new(left),
                        right: Box::new(right),
                    });
                }
            }};
        }

        // Helper: get binary operator + precedence from current token.
        // Returns None if the current token is not a binary operator.
        // Precedence levels match the recursive descent chain:
        //   1 = |>, 2 = ??, 3 = range, 4 = ||, 5 = |, 6 = ^, 7 = &,
        //   8 = <<,>>, 9 = &&, 10 = ==,!=, 11 = <,>,<=,>=, 12 = +,-,
        //   13 = *,/,%
        macro_rules! peek_bin_op {
            () => {{
                match self.peek() {
                    TokenKind::PipeArrow => Some((BinOp::Or, 1u8)), // |> desugars, but treat as binary for parsing
                    TokenKind::QuestionQuestion => Some((BinOp::Or, 2u8)), // ?? — use Or as placeholder
                    TokenKind::Or => Some((BinOp::Or, 4u8)),
                    TokenKind::Pipe => Some((BinOp::BitOr, 5u8)),
                    TokenKind::Caret => Some((BinOp::BitXor, 6u8)),
                    TokenKind::Ampersand => Some((BinOp::BitAnd, 7u8)),
                    TokenKind::Shl => Some((BinOp::Shl, 8u8)),
                    TokenKind::Shr => Some((BinOp::Shr, 8u8)),
                    TokenKind::And => Some((BinOp::And, 9u8)),
                    TokenKind::EqEq => Some((BinOp::Eq, 10u8)),
                    TokenKind::BangEq => Some((BinOp::Neq, 10u8)),
                    TokenKind::Lt => Some((BinOp::Lt, 11u8)),
                    TokenKind::Gt => Some((BinOp::Gt, 11u8)),
                    TokenKind::LtEq => Some((BinOp::LtEq, 11u8)),
                    TokenKind::GtEq => Some((BinOp::GtEq, 11u8)),
                    TokenKind::Plus => Some((BinOp::Add, 12u8)),
                    TokenKind::Minus => Some((BinOp::Sub, 12u8)),
                    TokenKind::Star => Some((BinOp::Mul, 13u8)),
                    TokenKind::Slash => Some((BinOp::Div, 13u8)),
                    TokenKind::Percent => Some((BinOp::Mod, 13u8)),
                    _ => None,
                }
            }};
        }

        // Parse the first primary value.
        // If it's `(`, push a Paren marker and parse the next primary.
        // This is iterative — no recursion for nested parens.
        let mut need_primary = true;

        loop {
            if need_primary {
                if self.check(&TokenKind::LParen) {
                    // Open paren — push marker, consume `(`, still need a primary
                    self.advance();
                    ops.push(OpEntry::Paren);
                    // Check for empty tuple ()
                    if self.check(&TokenKind::RParen) {
                        self.advance();
                        ops.pop(); // remove the Paren marker we just pushed
                        vals.push(Expr::TupleLit(vec![]));
                        need_primary = false;
                    }
                    // else: still need_primary = true, loop will parse the inner primary
                } else {
                    // Delegate to parse_primary for everything except `(`.
                    // parse_primary won't see `(` here since we handled it above.
                    // It may see `(` for function call args (e.g., `foo(...)`),
                    // which is fine — those don't cause deep recursion.
                    vals.push(self.parse_primary()?);
                    need_primary = false;
                }
            } else if let Some((bop, prec)) = peek_bin_op!() {
                self.advance();
                // Pop higher-or-equal precedence operators (left-associative)
                while let Some(OpEntry::Op(_, tp)) = ops.last() {
                    if *tp >= prec {
                        reduce_one!();
                    } else {
                        break;
                    }
                }
                ops.push(OpEntry::Op(bop, prec));
                need_primary = true;
            } else if self.check(&TokenKind::RParen) {
                // Close paren — pop until Paren marker
                let mut found_paren = false;
                while let Some(top) = ops.last() {
                    match top {
                        OpEntry::Op(_, _) => {
                            reduce_one!();
                        }
                        OpEntry::Paren => {
                            ops.pop();
                            found_paren = true;
                            break;
                        }
                    }
                }
                if found_paren {
                    self.advance(); // consume `)`
                    // Check if this was a tuple: if next token is `,`, we're in tuple mode
                    // But we've already reduced the expression inside parens.
                    // The result is on vals. Check for comma to detect tuple.
                    if self.check(&TokenKind::Comma) {
                        // Tuple: collect remaining elements
                        let mut elems = vec![vals.pop().unwrap()];
                        while self.match_tok(TokenKind::Comma) {
                            // Parse each element iteratively
                            self.expr_depth += 1;
                            let elem = if self.expr_depth > self.max_expr_depth {
                                self.parse_expr_iterative()?
                            } else {
                                self.parse_expr()?
                            };
                            self.expr_depth -= 1;
                            elems.push(elem);
                        }
                        self.expect(TokenKind::RParen, ")")?;
                        vals.push(Expr::TupleLit(elems));
                    }
                    // Continue loop to check for more operators after the `)`
                } else {
                    // No matching paren — end of expression (unmatched `)`)
                    break;
                }
            } else {
                // Not a binary operator, not `)` — end of expression
                break;
            }
        }

        // Drain remaining operators
        while let Some(top) = ops.last() {
            match top {
                OpEntry::Op(_, _) => {
                    reduce_one!();
                }
                OpEntry::Paren => {
                    // Unmatched `(` — shouldn't happen if the input is well-formed
                    // The `)` handler should have consumed it. If we get here,
                    // it means the expression ended inside parens.
                    return Err("unmatched '(' in expression".to_string());
                }
            }
        }

        if vals.is_empty() {
            return Err("expected expression".to_string());
        }
        Ok(vals.pop().unwrap())
    }

    fn parse_primary(&mut self) -> Result<Expr, String> {
        let tok = self.peek_token().clone();
        match &tok.kind {
            TokenKind::IntLit(n) => {
                self.advance();
                Ok(Expr::IntLit(*n))
            }
            TokenKind::FloatLit(n) => {
                self.advance();
                Ok(Expr::FloatLit(*n))
            }
            TokenKind::StringLit(s) => {
                self.advance();
                Ok(Expr::StringLit(s.clone()))
            }
            TokenKind::FString(s) => {
                self.advance();
                self.parse_fstring_parts(s)
            }
            TokenKind::LBracket => {
                self.advance(); // consume [
                // Array literal: [val, val, ...]
                let mut elements = Vec::new();
                if !self.check(&TokenKind::RBracket) {
                    loop {
                        elements.push(self.parse_expr()?);
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                }
                self.expect(TokenKind::RBracket, "]")?;
                let elem_type = Box::new(Type::I64); // inferred, simplified
                Ok(Expr::ArrayLit { elem_type, elements })
            }
            TokenKind::True => {
                self.advance();
                Ok(Expr::BoolLit(true))
            }
            TokenKind::False => {
                self.advance();
                Ok(Expr::BoolLit(false))
            }
            TokenKind::Fn => {
                // Closure: pf(params) -> ret { body } or pf(params) => expr
                self.advance(); // consume pf
                self.expect(TokenKind::LParen, "(")?;
                let params = self.parse_params()?;
                self.expect(TokenKind::RParen, ")")?;

                let ret_ty = if self.match_tok(TokenKind::Arrow) {
                    // Could be -> type { body } or => expr
                    // Check if next is a type or a block
                    let peek = self.peek_token().clone();
                    match &peek.kind {
                        TokenKind::I8 | TokenKind::I16 | TokenKind::I32 | TokenKind::I64
                        | TokenKind::I128 | TokenKind::U8 | TokenKind::U16 | TokenKind::U32
                        | TokenKind::U64 | TokenKind::U128 | TokenKind::F32 | TokenKind::F64
                        | TokenKind::Bool | TokenKind::Str | TokenKind::Void
                        | TokenKind::Ident(_) | TokenKind::Ampersand | TokenKind::LBracket
                        | TokenKind::LParen => {
                            let ty = self.parse_type()?;
                            Some(ty)
                        }
                        _ => None,
                    }
                } else {
                    None
                };

                if self.match_tok(TokenKind::FatArrow) {
                    // Expression closure: pf(x) => expr
                    let line = self.peek_token().line;
                    let expr = self.parse_expr()?;
                    Ok(Expr::Closure {
                        params,
                        body: vec![Stmt::Expr(expr, line)],
                        ret_ty,
                    })
                } else {
                    // Block closure: pf(x) { body }
                    self.expect(TokenKind::LBrace, "{")?;
                    let body = self.parse_block()?;
                    self.expect(TokenKind::RBrace, "}")?;
                    Ok(Expr::Closure { params, body, ret_ty })
                }
            }
            TokenKind::LBrace => {
                // Block-as-expression: { stmt; stmt; expr }
                self.advance(); // consume {
                let block = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                Ok(Expr::Block(block))
            }
            TokenKind::Ident(name) => {
                self.advance();
                // Check for asm! macro
                if name == "asm" && self.match_tok(TokenKind::Bang) {
                    return self.parse_asm();
                }
                if self.match_tok(TokenKind::LParen) {
                    let mut args = Vec::new();
                    if !self.check(&TokenKind::RParen) {
                        loop {
                            args.push(self.parse_expr()?);
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::RParen, ")")?;
                    Ok(Expr::Call {
                        callee: name.clone(),
                        args,
                    })
                } else if self.check(&TokenKind::LBrace) && self.is_struct_name(name) {
                    // Struct literal: StructName { field: value, ... }
                    self.advance(); // consume {
                    let mut fields = Vec::new();
                    let mut spread: Option<Box<Expr>> = None;
                    if !self.check(&TokenKind::RBrace) {
                        loop {
                            // Check for spread: `..expr`
                            if self.check(&TokenKind::DotDot) {
                                self.advance(); // consume ..
                                let spread_expr = self.parse_expr()?;
                                spread = Some(Box::new(spread_expr));
                                if !self.match_tok(TokenKind::Comma) {
                                    break;
                                }
                                // Allow trailing comma after spread
                                if self.check(&TokenKind::RBrace) {
                                    break;
                                }
                                continue;
                            }
                            let fname = self.expect_ident_or_keyword()?;
                            self.expect(TokenKind::Colon, ":")?;
                            let fval = self.parse_expr()?;
                            fields.push((fname, fval));
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                            // Allow trailing comma
                            if self.check(&TokenKind::RBrace) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::RBrace, "}")?;
                    Ok(Expr::StructLit {
                        name: name.clone(),
                        fields,
                        spread,
                    })
                } else if self.check(&TokenKind::DoubleColon) {
                    // Could be: EnumName::Variant(args) or StructName::method(args)
                    self.advance(); // consume ::
                    let variant_or_method = self.expect_ident()?;
                    let mut args = Vec::new();
                    if self.match_tok(TokenKind::LParen) {
                        if !self.check(&TokenKind::RParen) {
                            loop {
                                args.push(self.parse_expr()?);
                                if !self.match_tok(TokenKind::Comma) {
                                    break;
                                }
                            }
                        }
                        self.expect(TokenKind::RParen, ")")?;
                    }
                    if self.is_enum_name(name) {
                        Ok(Expr::EnumLit {
                            enum_name: name.clone(),
                            variant: variant_or_method,
                            args,
                        })
                    } else {
                        // Struct associated function / static method call
                        Ok(Expr::MethodCall {
                            receiver: Box::new(Expr::Ident(name.clone())),
                            method: variant_or_method,
                            args,
                        })
                    }
                } else {
                    Ok(Expr::Ident(name.clone()))
                }
            }
            TokenKind::LParen => {
                self.advance();
                // SH-1.4 / 0.3: Track expression nesting depth. When it exceeds
                // max_expr_depth, switch to iterative parser to avoid stack overflow.
                self.expr_depth += 1;
                let first = if self.expr_depth > self.max_expr_depth {
                    self.parse_expr_iterative()?
                } else {
                    self.parse_expr()?
                };
                self.expr_depth -= 1;
                if self.match_tok(TokenKind::Comma) {
                    // Tuple literal: (expr, expr, ...)
                    let mut elems = vec![first];
                    loop {
                        self.expr_depth += 1;
                        let next = if self.expr_depth > self.max_expr_depth {
                            self.parse_expr_iterative()?
                        } else {
                            self.parse_expr()?
                        };
                        self.expr_depth -= 1;
                        elems.push(next);
                        if !self.match_tok(TokenKind::Comma) {
                            break;
                        }
                    }
                    self.expect(TokenKind::RParen, ")")?;
                    Ok(Expr::TupleLit(elems))
                } else {
                    self.expect(TokenKind::RParen, ")")?;
                    Ok(first)
                }
            }
            TokenKind::If => {
                let if_expr = self.parse_if_expr()?;
                Ok(Expr::If {
                    cond: if_expr.cond,
                    then_block: if_expr.then_block,
                    else_block: if_expr.else_block,
                })
            }
            TokenKind::Match => {
                self.parse_match_expr()
            }
            TokenKind::Try => {
                // try { body } — catches ? early returns, wraps result in Result
                self.advance(); // consume try
                self.expect(TokenKind::LBrace, "{")?;
                let body = self.parse_block()?;
                self.expect(TokenKind::RBrace, "}")?;
                Ok(Expr::TryBlock { body })
            }
            TokenKind::Await => {
                // await <expr> — suspend until Future is ready
                self.advance(); // consume await
                let expr = self.parse_unary()?;
                Ok(Expr::Await { expr: Box::new(expr) })
            }
            TokenKind::Spawn => {
                // spawn <expr> — create a concurrent task, returns Future<T>
                self.advance(); // consume spawn
                let expr = self.parse_unary()?;
                Ok(Expr::Spawn { expr: Box::new(expr) })
            }
            _ => {
                // Allow word-like keywords to be used as identifiers in expression
                // context (e.g. `query` as a variable name, `move` as a field name).
                // Punctuation tokens (e.g. `;`, `}`, `{`) must NOT be accepted —
                // they should produce a parse error so error recovery can activate.
                if tok.kind.is_word_keyword() {
                    let kw_str = format!("{}", tok.kind);
                    self.advance();
                    Ok(Expr::Ident(kw_str))
                } else {
                    // Not a valid primary expression token
                    Err(format!(
                        "Parse error at {}:{}: unexpected token '{}'",
                        tok.line, tok.col, tok.kind
                    ))
                }
            }
        }
    }

    /// Parse inline assembly: asm!("template", in("reg") expr, out("reg") ident, ...)
    fn parse_asm(&mut self) -> Result<Expr, String> {
        self.expect(TokenKind::LParen, "(")?;
        // First argument: template string
        let template = {
            let tok = self.peek_token().clone();
            match &tok.kind {
                TokenKind::StringLit(s) => {
                    self.advance();
                    s.clone()
                }
                _ => return Err(format!(
                    "Parse error: asm! first argument must be a string template, got '{}'",
                    tok.kind
                )),
            }
        };
        // Parse operands
        let mut operands = Vec::new();
        while self.match_tok(TokenKind::Comma) {
            // Direction: in() or out()
            // 'in' is a keyword token, 'out' is an identifier
            let direction = match &self.peek_token().kind {
                TokenKind::In => {
                    self.advance();
                    crate::ast::AsmDir::In
                }
                TokenKind::Ident(s) if s == "out" => {
                    self.advance();
                    crate::ast::AsmDir::Out
                }
                TokenKind::Ident(s) if s == "clobber" => {
                    self.advance();
                    crate::ast::AsmDir::Clobber
                }
                _ => return Err(format!(
                    "Parse error: asm! operand must start with 'in', 'out', or 'clobber', got '{}'",
                    self.peek_token().kind
                )),
            };
            self.expect(TokenKind::LParen, "(")?;
            let reg = {
                let tok = self.peek_token().clone();
                match &tok.kind {
                    TokenKind::StringLit(s) => {
                        self.advance();
                        s.clone()
                    }
                    _ => return Err(format!(
                        "Parse error: asm! register must be a string, got '{}'",
                        tok.kind
                    )),
                }
            };
            self.expect(TokenKind::RParen, ")")?;
            // Clobber operands don't have an expression — use IntLit(0) as placeholder
            let expr = if direction == crate::ast::AsmDir::Clobber {
                crate::ast::Expr::IntLit(0)
            } else {
                self.parse_expr()?
            };
            operands.push(crate::ast::AsmOperand {
                direction,
                reg,
                expr,
            });
        }
        self.expect(TokenKind::RParen, ")")?;
        Ok(Expr::Asm {
            template,
            operands,
        })
    }

    fn parse_use(&mut self) -> Result<ImportDef, String> {
        self.expect(TokenKind::Use, "use")?;
        // use path::item1, item2;
        // use path::{item1, item2};
        // use path::*;
        // use path as alias;
        // use path;
        let mut is_glob = false;
        let mut path = String::new();
        loop {
            // Break if we see { for selective imports: use core::{Option, max}
            if self.check(&TokenKind::LBrace) {
                break;
            }
            // Break if we see * for glob imports: use core::*
            if self.check(&TokenKind::Star) {
                // Consume the trailing :: if present
                if path.ends_with("::") {
                    path = path[..path.len() - 2].to_string();
                }
                self.advance(); // consume *
                is_glob = true;
                break;
            }
            let name = self.expect_ident()?;
            path.push_str(&name);
            if self.match_tok(TokenKind::DoubleColon) {
                path.push_str("::");
            } else {
                break;
            }
        }
        // Check for alias: use path as alias
        let alias = if self.match_tok(TokenKind::As) {
            Some(self.expect_ident()?)
        } else {
            None
        };
        // Check for selective imports: { item1, item2 } or { * }
        let mut items = Vec::new();
        if self.match_tok(TokenKind::LBrace) {
            if self.check(&TokenKind::Star) {
                self.advance();
                is_glob = true;
            } else if !self.check(&TokenKind::RBrace) {
                loop {
                    let item = self.expect_ident()?;
                    items.push(item);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RBrace, "}")?;
        }
        // Strip trailing :: from path if items were specified
        if (!items.is_empty() || is_glob) && path.ends_with("::") {
            path = path[..path.len() - 2].to_string();
        }
        // Check for capability grant: with { IO, FS }
        let mut capabilities = Vec::new();
        if self.match_tok(TokenKind::With) {
            self.expect(TokenKind::LBrace, "{")?;
            if !self.check(&TokenKind::RBrace) {
                loop {
                    let cap = self.expect_ident()?;
                    capabilities.push(cap);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RBrace, "}")?;
        }
        // Check for version pin: = "0.1.0"
        let version = if self.match_tok(TokenKind::Eq) {
            let tok = self.peek_token().clone();
            if let TokenKind::StringLit(s) = &tok.kind {
                self.advance();
                Some(s.clone())
            } else {
                return Err(format!(
                    "Parse error at {}:{}: expected version string but got '{}'",
                    tok.line, tok.col, tok.kind
                ));
            }
        } else {
            None
        };
        self.match_tok(TokenKind::Semicolon);
        Ok(ImportDef { path, items, capabilities, version, alias, is_pub: false, is_glob })
    }

    fn parse_use_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Use, "use")?;
        let mut path = String::new();
        loop {
            if self.check(&TokenKind::LBrace) {
                break;
            }
            let name = self.expect_ident()?;
            path.push_str(&name);
            if self.match_tok(TokenKind::DoubleColon) {
                path.push_str("::");
            } else {
                break;
            }
        }
        let mut items = Vec::new();
        if self.match_tok(TokenKind::LBrace) {
            if !self.check(&TokenKind::RBrace) {
                loop {
                    let item = self.expect_ident()?;
                    items.push(item);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RBrace, "}")?;
        }
        if !items.is_empty() && path.ends_with("::") {
            path = path[..path.len() - 2].to_string();
        }
        self.match_tok(TokenKind::Semicolon);
        Ok(Stmt::Use { path, items, line })
    }

    /// Parse `handle { body } with { Effect.op(params) => { handler }, ... }`
    fn parse_handle_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Handle, "handle")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        self.expect(TokenKind::With, "with")?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut handlers = Vec::new();
        while !self.check(&TokenKind::RBrace) {
            // Effect.op(params)
            let effect = self.expect_ident()?;
            self.expect(TokenKind::Dot, ".")?;
            let op = self.expect_ident()?;
            self.expect(TokenKind::LParen, "(")?;
            let mut params = Vec::new();
            if !self.check(&TokenKind::RParen) {
                loop {
                    params.push(self.expect_ident()?);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;
            self.expect(TokenKind::FatArrow, "=>")?;
            self.expect(TokenKind::LBrace, "{")?;
            let handler_body = self.parse_block()?;
            self.expect(TokenKind::RBrace, "}")?;

            handlers.push(EffectHandler {
                effect,
                op,
                params,
                body: handler_body,
            });

            if !self.match_tok(TokenKind::Comma) {
                break;
            }
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Stmt::Handle { body, handlers, line })
    }

    /// Parse `do Effect.op(args)` — perform an effect operation
    fn parse_do_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Do, "do")?;
        let effect = self.expect_ident()?;
        self.expect(TokenKind::Dot, ".")?;
        let op = self.expect_ident()?;
        self.expect(TokenKind::LParen, "(")?;
        let mut args = Vec::new();
        if !self.check(&TokenKind::RParen) {
            loop {
                args.push(self.parse_expr()?);
                if !self.match_tok(TokenKind::Comma) {
                    break;
                }
            }
        }
        self.expect(TokenKind::RParen, ")")?;
        self.match_tok(TokenKind::Semicolon);
        Ok(Stmt::Do { effect, op, args, line })
    }

    /// Parse `region r { body }`, `region r with budget(N) { body }`,
    /// `region gpu r { body }`, `region gpu r with budget(N) { body }`,
    /// or `region kv_cache r with budget(N) [window(W)] { body }` (ML9)
    fn parse_region_block(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Region, "region")?;

        // Optional modifier: `gpu` (ML1) or `kv_cache` (ML9)
        let is_gpu = if let TokenKind::Ident(s) = self.peek() {
            if s == "gpu" {
                self.advance();
                true
            } else {
                false
            }
        } else {
            false
        };
        let is_kv_cache = if !is_gpu {
            if let TokenKind::Ident(s) = self.peek() {
                if s == "kv_cache" {
                    self.advance();
                    true
                } else {
                    false
                }
            } else {
                false
            }
        } else {
            false
        };

        let name = self.expect_ident()?;

        // Optional: `with budget(N)`
        let budget = if self.match_tok(TokenKind::With) {
            let budget_name = self.expect_ident()?;
            if budget_name != "budget" {
                return Err(format!("Expected 'budget' after 'with' in region block, got '{}'", budget_name));
            }
            self.expect(TokenKind::LParen, "(")?;
            // Parse the budget value (must be a literal integer)
            let n = match self.peek().clone() {
                TokenKind::IntLit(n) => {
                    self.advance();
                    n as u64
                }
                _ => return Err("Expected integer literal in budget(N)".to_string()),
            };
            self.expect(TokenKind::RParen, ")")?;
            Some(n)
        } else {
            None
        };

        // Optional: `window(W)` — sliding window size for KV cache (ML9)
        let window_size = if is_kv_cache {
            if let TokenKind::Ident(s) = self.peek() {
                if s == "window" {
                    self.advance();
                    self.expect(TokenKind::LParen, "(")?;
                    let w = match self.peek().clone() {
                        TokenKind::IntLit(n) => {
                            self.advance();
                            n as u64
                        }
                        _ => return Err("Expected integer literal in window(W)".to_string()),
                    };
                    self.expect(TokenKind::RParen, ")")?;
                    Some(w)
                } else {
                    None
                }
            } else {
                None
            }
        } else {
            None
        };

        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::RegionBlock { name, budget, is_gpu, is_kv_cache, window_size, body, line })
    }

    /// Parse `sandbox { body }` — capability revocation block.
    /// All capabilities are stripped for the scope. Any IO/FS/Net/Spawn
    /// call inside is a compile error.
    fn parse_sandbox_block(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Sandbox, "sandbox")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::Sandbox { body, line })
    }

    /// Parse `unsafe { body }` — unsafe block.
    /// Permits operations that would otherwise be rejected by the safety
    /// type system (e.g. raw pointer dereference). The body is otherwise
    /// a normal block — unsafe is compile-time only.
    fn parse_unsafe_block(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Unsafe, "unsafe")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::Unsafe { body, line })
    }

    /// Parse a transaction block: `txn { body }`
    /// Atomic transaction with rollback on failure.
    fn parse_txn_block(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Txn, "txn")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::Txn { body, line })
    }

    /// Parse a grad block: `grad(var) { body; backward(loss); }`
    /// The compiler traces operations on `var` and generates gradient code.
    fn parse_grad_block(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Grad, "grad")?;
        self.expect(TokenKind::LParen, "(")?;
        let var = self.expect_ident()?;
        self.expect(TokenKind::RParen, ")")?;
        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;
        Ok(Stmt::GradBlock { var, body, line })
    }

    /// Parse an @-annotation: `@name` or `@name(args)`
    /// Returns (name, args) where args is a list of raw string tokens.
    fn parse_at_annotation(&mut self) -> Result<Annotation, String> {
        self.expect(TokenKind::At, "@")?;
        // Annotation name can be a keyword (like 'kernel') or an identifier.
        // Allow hyphens in annotation names (e.g., @safety-critical, @no-alloc, @dal-a)
        let name = match self.peek().clone() {
            TokenKind::Ident(s) => {
                self.advance();
                // Check for hyphenated name: ident - ident - ident ...
                let mut full_name = s;
                while self.check(&TokenKind::Minus) {
                    self.advance();
                    match self.peek().clone() {
                        TokenKind::Ident(part) => {
                            self.advance();
                            full_name.push('-');
                            full_name.push_str(&part);
                        }
                        _ => break,
                    }
                }
                full_name
            }
            TokenKind::Kernel => { self.advance(); "kernel".to_string() }
            TokenKind::Layer => { self.advance(); "layer".to_string() }
            TokenKind::Model => { self.advance(); "model".to_string() }
            TokenKind::Interrupt => { self.advance(); "interrupt".to_string() }
            _ => return Err(format!(
                "Parse error at {}:{}: expected annotation name after '@' but got '{:?}'",
                self.peek_token().line, self.peek_token().col, self.peek()
            )),
        };
        let mut args = Vec::new();
        if self.match_tok(TokenKind::LParen) {
            if !self.check(&TokenKind::RParen) {
                loop {
                    // Parse annotation arg — can be ident, int, string, or key=value
                    let arg = self.parse_annotation_arg()?;
                    args.push(arg);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;
        }
        Ok(Annotation { name, args })
    }

    fn parse_annotation_arg(&mut self) -> Result<String, String> {
        // key=value or just value
        let mut arg = String::new();
        match self.peek().clone() {
            TokenKind::Ident(s) => {
                self.advance();
                arg = s;
                if self.match_tok(TokenKind::Eq) {
                    arg.push_str("=");
                    arg.push_str(&self.parse_annotation_arg()?);
                }
            }
            TokenKind::IntLit(n) => {
                self.advance();
                arg = n.to_string();
            }
            TokenKind::FloatLit(f) => {
                self.advance();
                arg = format!("{}", f);
            }
            TokenKind::StringLit(s) => {
                self.advance();
                arg = format!("\"{}\"", s);
            }
            TokenKind::LBracket => {
                // [1, 2, 3] — workgroup size
                self.advance();
                arg.push_str("[");
                loop {
                    arg.push_str(&self.parse_annotation_arg()?);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                    arg.push_str(",");
                }
                self.expect(TokenKind::RBracket, "]")?;
                arg.push_str("]");
            }
            _ => return Err(format!("Expected annotation argument, got {:?}", self.peek())),
        }
        Ok(arg)
    }

    fn parse_attribute(&mut self) -> Result<Attribute, String> {
        self.expect(TokenKind::Hash, "#")?;
        self.expect(TokenKind::LBracket, "[")?;
        let name = self.expect_ident()?;
        let mut args = Vec::new();
        // Optional args: #[test] or #[name(arg1, arg2)]
        if self.match_tok(TokenKind::LParen) {
            if !self.check(&TokenKind::RParen) {
                loop {
                    let arg = self.expect_ident()?;
                    args.push(arg);
                    if !self.match_tok(TokenKind::Comma) {
                        break;
                    }
                }
            }
            self.expect(TokenKind::RParen, ")")?;
        }
        self.expect(TokenKind::RBracket, "]")?;
        Ok(Attribute { name, args })
    }

    fn parse_fstring_parts(&mut self, raw: &str) -> Result<Expr, String> {
        let mut parts = Vec::new();
        let mut current = String::new();
        let mut chars = raw.chars().peekable();

        while let Some(c) = chars.next() {
            if c == '{' {
                if !current.is_empty() {
                    parts.push(FStringPart::Literal(current.clone()));
                    current.clear();
                }
                // Read expression until matching }
                let mut expr_src = String::new();
                let mut depth = 1;
                while let Some(c2) = chars.next() {
                    if c2 == '{' {
                        depth += 1;
                        expr_src.push(c2);
                    } else if c2 == '}' {
                        depth -= 1;
                        if depth == 0 {
                            break;
                        }
                        expr_src.push(c2);
                    } else {
                        expr_src.push(c2);
                    }
                }
                // Re-lex and parse the expression
                let mut sub_lexer = crate::lexer::Lexer::new(&expr_src);
                let sub_tokens = sub_lexer.tokenize();
                let mut sub_parser = Parser::new(sub_tokens);
                let expr = sub_parser.parse_expr()?;
                parts.push(FStringPart::Expr(expr));
            } else {
                current.push(c);
            }
        }
        if !current.is_empty() {
            parts.push(FStringPart::Literal(current));
        }

        Ok(Expr::FString { parts })
    }
}
