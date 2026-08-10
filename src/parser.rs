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
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Self {
        Parser { tokens, pos: 0, struct_names: Vec::new(), enum_names: Vec::new() }
    }

    /// Create a parser with pre-populated struct/enum names (for stdlib auto-include)
    pub fn with_names(tokens: Vec<Token>, struct_names: Vec<String>, enum_names: Vec<String>) -> Self {
        Parser { tokens, pos: 0, struct_names, enum_names }
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
                TokenKind::Pc => {
                    let mut c = self.parse_const()?;
                    c.is_pub = is_pub;
                    program.consts.push(c);
                }
                TokenKind::Ty => {
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
                _ => {
                    let mut func = self.parse_func()?;
                    func.is_pub = is_pub;
                    program.funcs.push(func);
                }
            }
        }

        Ok(program)
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

    fn is_struct_name(&self, name: &str) -> bool {
        self.struct_names.iter().any(|s| s == name)
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
        self.expect(TokenKind::LBrace, "{")?;

        let mut fields = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let fname = self.expect_ident()?;
            self.expect(TokenKind::Colon, ":")?;
            let fty = self.parse_type()?;
            fields.push(Param { name: fname, ty: fty });
            self.match_tok(TokenKind::Comma);
        }
        self.expect(TokenKind::RBrace, "}")?;

        self.struct_names.push(name.clone());
        Ok(StructDef { name, fields, is_pub: false })
    }

    fn parse_enum(&mut self) -> Result<EnumDef, String> {
        self.expect(TokenKind::Enum, "enum")?;
        let name = self.expect_ident()?;
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
                            params.push(Param { name: pname, ty: pty });
                        } else {
                            // Anonymous: just a type
                            let pty = self.parse_type()?;
                            let pname = format!("_{}", params.len());
                            params.push(Param { name: pname, ty: pty });
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
        Ok(EnumDef { name, variants, is_pub: false })
    }

    fn parse_const(&mut self) -> Result<ConstDef, String> {
        self.expect(TokenKind::Pc, "pc")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::Colon, ":")?;
        let ty = self.parse_type()?;
        self.expect(TokenKind::Eq, "=")?;
        let expr = self.parse_expr()?;
        self.match_tok(TokenKind::Semicolon);
        Ok(ConstDef { name, ty, expr, is_pub: false })
    }

    fn parse_type_alias(&mut self) -> Result<TypeAlias, String> {
        self.expect(TokenKind::Ty, "ty")?;
        let name = self.expect_ident()?;
        self.expect(TokenKind::Eq, "=")?;
        let alias_for = self.parse_type()?;
        self.match_tok(TokenKind::Semicolon);
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
                // Associated type binding: `ty Item = ConcreteType;`
                if self.check(&TokenKind::Ty) {
                    self.advance();
                    let assoc_name = self.expect_ident()?;
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
            // Associated type declaration: `ty Item;`
            if self.check(&TokenKind::Ty) {
                self.advance();
                let assoc_name = self.expect_ident()?;
                self.match_tok(TokenKind::Semicolon);
                associated_types.push(assoc_name);
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
            self.expect(TokenKind::Pf, "pf")?;
            let method_name = self.expect_ident()?;

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
                        params.push(Param { name: "self".to_string(), ty: Type::Named("Self".to_string()) });
                    } else {
                        self.expect(TokenKind::Colon, ":")?;
                        let ty = self.parse_type()?;
                        params.push(Param { name: pname, ty });
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

    fn parse_func(&mut self) -> Result<Func, String> {
        self.expect(TokenKind::Pf, "fn")?;
        let name = self.expect_ident()?;

        // Generic type params: <T, U, ...> with optional trait bounds: <T: Display, U>
        let mut type_params = Vec::new();
        let mut type_param_bounds: Vec<(String, String)> = Vec::new();
        if self.check(&TokenKind::Lt) {
            self.advance(); // consume <
            if !self.check(&TokenKind::Gt) {
                loop {
                    let tp = self.expect_ident()?;
                    type_params.push(tp.clone());
                    // Parse trait bound: T: SomeCap
                    if self.match_tok(TokenKind::Colon) {
                        let bound = self.expect_ident()?;
                        type_param_bounds.push((tp, bound));
                        // Skip additional bounds: T: Cap1 + Cap2
                        while self.match_tok(TokenKind::Plus) {
                            let _ = self.expect_ident()?;
                        }
                    }
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

        self.expect(TokenKind::LBrace, "{")?;
        let body = self.parse_block()?;
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Func {
            name,
            type_params,
            type_param_bounds,
            params,
            ret_ty,
            body,
            is_pub: false,
            is_comptime: false,
        })
    }

    fn parse_params(&mut self) -> Result<Vec<Param>, String> {
        let mut params = Vec::new();
        if self.check(&TokenKind::RParen) {
            return Ok(params);
        }

        loop {
            let name = self.expect_ident()?;
            self.expect(TokenKind::Colon, ":")?;
            let ty = self.parse_type()?;
            params.push(Param { name, ty });

            if !self.match_tok(TokenKind::Comma) {
                break;
            }
        }

        Ok(params)
    }

    fn parse_type(&mut self) -> Result<Type, String> {
        let tok = self.peek_token().clone();
        let ty = match tok.kind {
            TokenKind::Pf => {
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
                // Check for associated type: T::Item
                if self.check(&TokenKind::DoubleColon) {
                    self.advance();
                    let assoc_name = self.expect_ident()?;
                    return Ok(Type::Assoc(s, String::new(), assoc_name));
                }
                // Check for generic type args: Name<T, U>
                if self.check(&TokenKind::Lt) {
                    self.advance(); // consume <
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
            TokenKind::LBracket => {
                self.advance();
                let elem = self.parse_type()?;
                self.expect(TokenKind::Semicolon, ";")?;
                let size_tok = self.peek_token().clone();
                let size = match &size_tok.kind {
                    TokenKind::IntLit(n) => *n as usize,
                    _ => return Err(format!(
                        "Parse error at {}:{}: expected array size but got '{}'",
                        size_tok.line, size_tok.col, size_tok.kind
                    )),
                };
                self.advance();
                self.expect(TokenKind::RBracket, "]")?;
                return Ok(Type::Array { elem: Box::new(elem), size });
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
            stmts.push(self.parse_stmt()?);
        }

        Ok(stmts)
    }

    fn parse_stmt(&mut self) -> Result<Stmt, String> {
        match self.peek() {
            TokenKind::Pv => self.parse_let(),
            TokenKind::Ret => self.parse_return(),
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
            TokenKind::Use => self.parse_use_stmt(),
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

    fn parse_match_stmt(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Match, "match")?;
        let expr = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut arms = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let pattern = self.parse_pattern()?;
            self.expect(TokenKind::FatArrow, "=>")?;
            self.expect(TokenKind::LBrace, "{")?;
            let body = self.parse_block()?;
            self.expect(TokenKind::RBrace, "}")?;
            self.match_tok(TokenKind::Comma);
            arms.push(MatchArm { pattern, body });
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Stmt::Match { expr, arms, line })
    }

    fn parse_match_expr(&mut self) -> Result<Expr, String> {
        self.expect(TokenKind::Match, "match")?;
        let expr = self.parse_expr()?;
        self.expect(TokenKind::LBrace, "{")?;

        let mut arms = Vec::new();
        while !self.check(&TokenKind::RBrace) && !self.is_at_end() {
            let pattern = self.parse_pattern()?;
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
            arms.push(MatchArm { pattern, body });
        }
        self.expect(TokenKind::RBrace, "}")?;

        Ok(Expr::Match { expr: Box::new(expr), arms })
    }

    fn parse_pattern(&mut self) -> Result<Pattern, String> {
        let tok = self.peek_token().clone();
        match &tok.kind {
            TokenKind::IntLit(n) => {
                self.advance();
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
                            self.expect(TokenKind::Colon, ":")?;
                            // Accept any type name (ident or type keyword)
                            let bty = self.read_type_name()?;
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
                    // Shorthand: Some(x: i64) without EnumName:: prefix
                    self.advance(); // consume (
                    let mut bindings = Vec::new();
                    loop {
                        let bname = self.expect_ident()?;
                        self.expect(TokenKind::Colon, ":")?;
                        let bty = self.read_type_name()?;
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
        self.expect(TokenKind::Pv, "pv")?;
        let is_mut = self.match_tok(TokenKind::Mut);
        let name = self.expect_ident()?;

        let ty = if self.match_tok(TokenKind::Colon) {
            Some(self.parse_type()?)
        } else {
            None
        };

        self.expect(TokenKind::Eq, "=")?;
        let expr = self.parse_expr()?;
        self.match_tok(TokenKind::Semicolon);

        Ok(Stmt::Let { name, ty, expr, is_mut, line })
    }

    fn parse_return(&mut self) -> Result<Stmt, String> {
        let line = self.peek_token().line;
        self.expect(TokenKind::Ret, "ret")?;
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
        self.parse_range()
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
            _ => self.parse_postfix(),
        }
    }

    fn parse_postfix(&mut self) -> Result<Expr, String> {
        let mut expr = self.parse_primary()?;
        loop {
            if self.match_tok(TokenKind::Dot) {
                let field = self.expect_ident()?;
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
            } else {
                break;
            }
        }
        Ok(expr)
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
            TokenKind::Pf => {
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
                    if !self.check(&TokenKind::RBrace) {
                        loop {
                            let fname = self.expect_ident()?;
                            self.expect(TokenKind::Colon, ":")?;
                            let fval = self.parse_expr()?;
                            fields.push((fname, fval));
                            if !self.match_tok(TokenKind::Comma) {
                                break;
                            }
                        }
                    }
                    self.expect(TokenKind::RBrace, "}")?;
                    Ok(Expr::StructLit {
                        name: name.clone(),
                        fields,
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
                let first = self.parse_expr()?;
                if self.match_tok(TokenKind::Comma) {
                    // Tuple literal: (expr, expr, ...)
                    let mut elems = vec![first];
                    loop {
                        elems.push(self.parse_expr()?);
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
            _ => {
                // Try postfix (field access) on primary expressions
                // If it's not a primary token, error
                Err(format!(
                    "Parse error at {}:{}: unexpected token '{}'",
                    tok.line, tok.col, tok.kind
                ))
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
                _ => return Err(format!(
                    "Parse error: asm! operand must start with 'in' or 'out', got '{}'",
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
            let expr = self.parse_expr()?;
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
