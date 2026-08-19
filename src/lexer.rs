use crate::token::{Token, TokenKind};

pub struct Lexer {
    source: Vec<char>,
    pos: usize,
    line: usize,
    col: usize,
}

impl Lexer {
    pub fn new(source: &str) -> Self {
        Lexer {
            source: source.chars().collect(),
            pos: 0,
            line: 1,
            col: 1,
        }
    }

    pub fn tokenize(&mut self) -> Vec<Token> {
        let mut tokens = Vec::new();

        while self.pos < self.source.len() {
            let c = self.peek();

            if c.is_whitespace() {
                self.advance();
            } else if c == '/' && self.peek_at(1) == Some('/') {
                self.skip_line_comment();
            } else if c == '/' && self.peek_at(1) == Some('*') {
                self.skip_block_comment();
            } else if c == 'f' && self.peek_at(1) == Some('"') {
                tokens.push(self.read_fstring());
            } else if c.is_alphabetic() || c == '_' {
                tokens.push(self.read_identifier_or_keyword());
            } else if c.is_ascii_digit() {
                tokens.push(self.read_number());
            } else if c == '"' {
                tokens.push(self.read_string());
            } else {
                tokens.push(self.read_operator());
            }
        }

        tokens.push(Token::new(TokenKind::Eof, self.line, self.col));
        tokens
    }

    fn peek(&self) -> char {
        *self.source.get(self.pos).unwrap_or(&'\0')
    }

    fn peek_at(&self, offset: usize) -> Option<char> {
        self.source.get(self.pos + offset).copied()
    }

    fn advance(&mut self) -> char {
        let c = self.peek();
        self.pos += 1;
        if c == '\n' {
            self.line += 1;
            self.col = 1;
        } else {
            self.col += 1;
        }
        c
    }

    fn skip_line_comment(&mut self) {
        while self.pos < self.source.len() && self.peek() != '\n' {
            self.advance();
        }
    }

    fn read_identifier_or_keyword(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        let mut s = String::new();

        while self.pos < self.source.len() {
            let c = self.peek();
            if c.is_alphanumeric() || c == '_' {
                s.push(self.advance());
            } else {
                break;
            }
        }

        let kind = match s.as_str() {
            "pf" => TokenKind::Pf,
            "pv" => TokenKind::Pv,
            "pm" => TokenKind::Pm,
            "mut" => TokenKind::Mut,
            "const" => TokenKind::Const,
            "given" => TokenKind::Given,
            "ensure" => TokenKind::Ensure,
            "pc" => TokenKind::Pc,
            "cap" => TokenKind::Cap,
            "ty" => TokenKind::Ty,
            "ret" => TokenKind::Ret,
            "if" => TokenKind::If,
            "else" => TokenKind::Else,
            "match" => TokenKind::Match,
            "struct" => TokenKind::Struct,
            "enum" => TokenKind::Enum,
            "impl" => TokenKind::Impl,
            "pub" => TokenKind::Pub,
            "use" => TokenKind::Use,
            "module" => TokenKind::Module,
            "with" => TokenKind::With,
            "async" => TokenKind::Async,
            "await" => TokenKind::Await,
            "comptime" => TokenKind::Comptime,
            "defer" => TokenKind::Defer,
            "spawn" => TokenKind::Spawn,
            "move" => TokenKind::Move,
            "static" => TokenKind::Static,
            "try" => TokenKind::Try,
            "in" => TokenKind::In,
            "as" => TokenKind::As,
            "dyn" => TokenKind::Dyn,
            "where" => TokenKind::Where,
            "region" => TokenKind::Region,
            "sandbox" => TokenKind::Sandbox,
            "unsafe" => TokenKind::Unsafe,
            "txn" => TokenKind::Txn,
            "persist" => TokenKind::Persist,
            "for" => TokenKind::For,
            "while" => TokenKind::While,
            "loop" => TokenKind::Loop,
            "break" => TokenKind::Break,
            "continue" => TokenKind::Continue,
            "handle" => TokenKind::Handle,
            "do" => TokenKind::Do,
            "interrupt" => TokenKind::Interrupt,
            "extern" => TokenKind::Extern,
            "guard" => TokenKind::Guard,
            "is" => TokenKind::Is,
            "true" => TokenKind::True,
            "false" => TokenKind::False,
            "layer" => TokenKind::Layer,
            "model" => TokenKind::Model,
            "grad" => TokenKind::Grad,
            "backward" => TokenKind::Backward,
            "kernel" => TokenKind::Kernel,
            "q8" => TokenKind::I8,    // q8 is an alias for i8 (quantized 8-bit)
            "q4" => TokenKind::I8,    // q4 is stored in i8 (quantized 4-bit)
            "i8" => TokenKind::I8,
            "i16" => TokenKind::I16,
            "i32" => TokenKind::I32,
            "i64" => TokenKind::I64,
            "i128" => TokenKind::I128,
            "u8" => TokenKind::U8,
            "u16" => TokenKind::U16,
            "u32" => TokenKind::U32,
            "u64" => TokenKind::U64,
            "u128" => TokenKind::U128,
            "f32" => TokenKind::F32,
            "f64" => TokenKind::F64,
            "bool" => TokenKind::Bool,
            "str" => TokenKind::Str,
            "void" => TokenKind::Void,
            _ => TokenKind::Ident(s),
        };

        Token::new(kind, line, col)
    }

    fn read_number(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        let mut s = String::new();

        // Check for hex, binary, octal prefixes
        if self.peek() == '0' {
            let next = self.peek_at(1);
            match next {
                Some('x') | Some('X') => {
                    s.push(self.advance()); // 0
                    s.push(self.advance()); // x
                    while self.pos < self.source.len() && self.peek().is_ascii_hexdigit() {
                        s.push(self.advance());
                    }
                    let val = i64::from_str_radix(&s[2..], 16).unwrap_or(0);
                    return Token::new(TokenKind::IntLit(val), line, col);
                }
                Some('b') | Some('B') => {
                    s.push(self.advance()); // 0
                    s.push(self.advance()); // b
                    while self.pos < self.source.len() && (self.peek() == '0' || self.peek() == '1') {
                        s.push(self.advance());
                    }
                    let val = i64::from_str_radix(&s[2..], 2).unwrap_or(0);
                    return Token::new(TokenKind::IntLit(val), line, col);
                }
                Some('o') | Some('O') => {
                    s.push(self.advance()); // 0
                    s.push(self.advance()); // o
                    while self.pos < self.source.len() && (self.peek() >= '0' && self.peek() <= '7') {
                        s.push(self.advance());
                    }
                    let val = i64::from_str_radix(&s[2..], 8).unwrap_or(0);
                    return Token::new(TokenKind::IntLit(val), line, col);
                }
                _ => {}
            }
        }

        while self.pos < self.source.len() && (self.peek().is_ascii_digit() || self.peek() == '_') {
            if self.peek() != '_' {
                s.push(self.peek());
            }
            self.advance();
        }

        if self.peek() == '.' && self.peek_at(1).map_or(false, |c| c.is_ascii_digit()) {
            s.push(self.advance());
            while self.pos < self.source.len() && (self.peek().is_ascii_digit() || self.peek() == '_') {
                if self.peek() != '_' {
                    s.push(self.peek());
                }
                self.advance();
            }
            let val: f64 = s.parse().unwrap();
            Token::new(TokenKind::FloatLit(val), line, col)
        } else {
            let val: i64 = s.parse().unwrap();
            Token::new(TokenKind::IntLit(val), line, col)
        }
    }

    fn skip_block_comment(&mut self) {
        self.advance(); // consume '/'
        self.advance(); // consume '*'
        while self.pos < self.source.len() {
            if self.peek() == '*' && self.peek_at(1) == Some('/') {
                self.advance();
                self.advance();
                return;
            }
            self.advance();
        }
    }

    fn read_fstring(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        self.advance(); // consume 'f'
        self.advance(); // consume opening "

        let mut s = String::new();
        while self.pos < self.source.len() && self.peek() != '"' {
            let c = self.peek();
            if c == '\\' {
                self.advance();
                let esc = self.advance();
                match esc {
                    'n' => s.push('\n'),
                    't' => s.push('\t'),
                    'r' => s.push('\r'),
                    '\\' => s.push('\\'),
                    '"' => s.push('"'),
                    '0' => s.push('\0'),
                    _ => {
                        eprintln!("Error:{}:{}: unknown escape sequence \\{}", line, col, esc);
                        s.push(esc);
                    }
                }
            } else if c == '{' {
                // Expression interpolation: {expr}
                self.advance(); // consume {
                s.push('{');
                let mut depth = 1;
                while self.pos < self.source.len() && depth > 0 {
                    let c2 = self.peek();
                    if c2 == '{' {
                        depth += 1;
                    } else if c2 == '}' {
                        depth -= 1;
                        if depth == 0 {
                            self.advance(); // consume }
                            s.push('}');
                            break;
                        }
                    }
                    s.push(self.advance());
                }
            } else {
                s.push(self.advance());
            }
        }

        if self.pos < self.source.len() {
            self.advance(); // consume closing "
        } else {
            eprintln!("Error:{}:{}: unterminated f-string", line, col);
        }

        Token::new(TokenKind::FString(s), line, col)
    }

    fn read_string(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        self.advance(); // consume opening "

        let mut s = String::new();
        while self.pos < self.source.len() && self.peek() != '"' {
            let c = self.peek();
            if c == '\\' {
                self.advance();
                let esc = self.advance();
                match esc {
                    'n' => s.push('\n'),
                    't' => s.push('\t'),
                    'r' => s.push('\r'),
                    '\\' => s.push('\\'),
                    '"' => s.push('"'),
                    '0' => s.push('\0'),
                    _ => {
                        eprintln!("Error:{}:{}: unknown escape sequence \\{}", line, col, esc);
                        s.push(esc);
                    }
                }
            } else {
                s.push(self.advance());
            }
        }

        if self.pos < self.source.len() {
            self.advance(); // consume closing "
        } else {
            eprintln!("Error:{}:{}: unterminated string", line, col);
        }

        Token::new(TokenKind::StringLit(s), line, col)
    }

    fn read_operator(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        let c = self.advance();

        let kind = match c {
            '+' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::PlusEq
                } else {
                    TokenKind::Plus
                }
            }
            '-' => {
                if self.peek() == '>' {
                    self.advance();
                    TokenKind::Arrow
                } else if self.peek() == '=' {
                    self.advance();
                    TokenKind::MinusEq
                } else {
                    TokenKind::Minus
                }
            }
            '*' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::StarEq
                } else {
                    TokenKind::Star
                }
            }
            '/' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::SlashEq
                } else {
                    TokenKind::Slash
                }
            }
            '%' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::PercentEq
                } else {
                    TokenKind::Percent
                }
            }
            '=' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::EqEq
                } else if self.peek() == '>' {
                    self.advance();
                    TokenKind::FatArrow
                } else {
                    TokenKind::Eq
                }
            }
            '!' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::BangEq
                } else {
                    TokenKind::Bang
                }
            }
            '<' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::LtEq
                } else if self.peek() == '<' {
                    self.advance();
                    if self.peek() == '=' {
                        self.advance();
                        TokenKind::ShlEq
                    } else {
                        TokenKind::Shl
                    }
                } else {
                    TokenKind::Lt
                }
            }
            '>' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::GtEq
                } else if self.peek() == '>' {
                    self.advance();
                    if self.peek() == '=' {
                        self.advance();
                        TokenKind::ShrEq
                    } else {
                        TokenKind::Shr
                    }
                } else {
                    TokenKind::Gt
                }
            }
            '&' => {
                if self.peek() == '&' {
                    self.advance();
                    TokenKind::And
                } else if self.peek() == '=' {
                    self.advance();
                    TokenKind::AmpEq
                } else {
                    TokenKind::Ampersand
                }
            }
            '|' => {
                if self.peek() == '|' {
                    self.advance();
                    TokenKind::Or
                } else if self.peek() == '>' {
                    self.advance();
                    TokenKind::PipeArrow
                } else if self.peek() == '=' {
                    self.advance();
                    TokenKind::PipeEq
                } else {
                    TokenKind::Pipe
                }
            }
            '^' => {
                if self.peek() == '=' {
                    self.advance();
                    TokenKind::CaretEq
                } else {
                    TokenKind::Caret
                }
            }
            '~' => {
                if self.peek() == '>' {
                    self.advance();
                    TokenKind::TildeArrow
                } else {
                    TokenKind::Tilde
                }
            }
            '.' => {
                if self.peek() == '.' {
                    self.advance();
                    if self.peek() == '=' {
                        self.advance();
                        TokenKind::DotDotEq
                    } else {
                        TokenKind::DotDot
                    }
                } else {
                    TokenKind::Dot
                }
            }
            '#' => TokenKind::Hash,
            '@' => TokenKind::At,
            ',' => TokenKind::Comma,
            ':' => {
                if self.peek() == ':' {
                    self.advance();
                    TokenKind::DoubleColon
                } else {
                    TokenKind::Colon
                }
            }
            ';' => TokenKind::Semicolon,
            '?' => {
                if self.peek() == '?' {
                    self.advance();
                    TokenKind::QuestionQuestion
                } else if self.peek() == '.' {
                    self.advance();
                    TokenKind::QuestionDot
                } else {
                    TokenKind::Question
                }
            }
            '(' => TokenKind::LParen,
            ')' => TokenKind::RParen,
            '{' => TokenKind::LBrace,
            '}' => TokenKind::RBrace,
            '[' => TokenKind::LBracket,
            ']' => TokenKind::RBracket,
            _ => {
                eprintln!("Error:{}:{}: unexpected character '{}'", line, col, c);
                TokenKind::Eof
            }
        };

        Token::new(kind, line, col)
    }
}
