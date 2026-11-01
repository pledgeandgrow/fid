use crate::token::{Token, TokenKind};

pub struct Lexer {
    source: Vec<char>,
    pos: usize,
    line: usize,
    col: usize,
    /// Collected error messages (instead of printing directly).
    pub errors: Vec<String>,
}

impl Lexer {
    pub fn new(source: &str) -> Self {
        Lexer {
            source: source.chars().collect(),
            pos: 0,
            line: 1,
            col: 1,
            errors: Vec::new(),
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
            } else if c == 'r' && (self.peek_at(1) == Some('"') || self.peek_at(1) == Some('#')) {
                tokens.push(self.read_raw_string());
            } else if c.is_alphabetic() || c == '_' {
                tokens.push(self.read_identifier_or_keyword());
            } else if c.is_ascii_digit() {
                tokens.push(self.read_number());
            } else if c == '"' {
                tokens.push(self.read_string());
            } else if c == '\'' {
                tokens.push(self.read_char_lit());
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
            "fn" => TokenKind::Fn,
            "val" => TokenKind::Val,
            "var" => TokenKind::Var,
            "mut" => TokenKind::Mut,
            "const" => TokenKind::Const,
            "given" => TokenKind::Given,
            "ensure" => TokenKind::Ensure,
            "cap" => TokenKind::Cap,
            "type" => TokenKind::Type,
            "return" => TokenKind::Return,
            "if" => TokenKind::If,
            "else" => TokenKind::Else,
            "match" => TokenKind::Match,
            "select" => TokenKind::Select,
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
            "resume" => TokenKind::Resume,
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
            "component" => TokenKind::Component,
            "system" => TokenKind::System,
            "query" => TokenKind::Query,
            "state" => TokenKind::State,
            "on" => TokenKind::On,
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
                    let val = self.radix_literal(&s, 16, line, col);
                    return Token::new(TokenKind::IntLit(val), line, col);
                }
                Some('b') | Some('B') => {
                    s.push(self.advance()); // 0
                    s.push(self.advance()); // b
                    while self.pos < self.source.len() && (self.peek() == '0' || self.peek() == '1') {
                        s.push(self.advance());
                    }
                    let val = self.radix_literal(&s, 2, line, col);
                    return Token::new(TokenKind::IntLit(val), line, col);
                }
                Some('o') | Some('O') => {
                    s.push(self.advance()); // 0
                    s.push(self.advance()); // o
                    while self.pos < self.source.len() && (self.peek() >= '0' && self.peek() <= '7') {
                        s.push(self.advance());
                    }
                    let val = self.radix_literal(&s, 8, line, col);
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
            // Scientific notation: e[+/-]digits
            if self.peek() == 'e' || self.peek() == 'E' {
                s.push(self.advance());
                if self.peek() == '+' || self.peek() == '-' {
                    s.push(self.advance());
                }
                while self.pos < self.source.len() && (self.peek().is_ascii_digit() || self.peek() == '_') {
                    if self.peek() != '_' {
                        s.push(self.peek());
                    }
                    self.advance();
                }
            }
            let val: f64 = s.parse().unwrap();
            Token::new(TokenKind::FloatLit(val), line, col)
        } else {
            // Integer with optional scientific notation (e.g. 1e308)
            if self.peek() == 'e' || self.peek() == 'E' {
                s.push(self.advance());
                if self.peek() == '+' || self.peek() == '-' {
                    s.push(self.advance());
                }
                while self.pos < self.source.len() && (self.peek().is_ascii_digit() || self.peek() == '_') {
                    if self.peek() != '_' {
                        s.push(self.peek());
                    }
                    self.advance();
                }
                let val: f64 = s.parse().unwrap();
                return Token::new(TokenKind::FloatLit(val), line, col);
            }
            // A literal above i64::MAX is a u64: keep its bits.
            let val = s.parse::<i64>().or_else(|_| s.parse::<u64>().map(|v| v as i64));
            match val {
                Ok(v) => Token::new(TokenKind::IntLit(v), line, col),
                Err(_) => {
                    self.errors.push(format!("Lexer error at {}:{}: integer literal '{}' does not fit in 64 bits", line, col, s));
                    Token::new(TokenKind::IntLit(0), line, col)
                }
            }
        }
    }

    /// The value of a 0x/0b/0o literal: 64 bits, kept as they are (it may be
    /// a u64 above i64::MAX). More than 64 bits is an error.
    fn radix_literal(&mut self, s: &str, radix: u32, line: usize, col: usize) -> i64 {
        match u64::from_str_radix(&s[2..], radix) {
            Ok(v) => v as i64,
            Err(_) => {
                self.errors.push(format!("Lexer error at {}:{}: integer literal '{}' does not fit in 64 bits", line, col, s));
                0
            }
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
                    // Issue 29: hex escape \xHH
                    'x' => {
                        let (ch, ok) = self.read_hex_escape(line, col);
                        if ok { s.push(ch); }
                    }
                    // Issue 29: unicode escape \u{...}
                    'u' => {
                        if self.peek() == '{' {
                            self.advance(); // consume {
                            let (ch, ok) = self.read_unicode_escape(line, col);
                            if ok { s.push(ch); }
                        } else {
                            self.errors.push(format!("Error:{}:{}: expected '{{' after \\u", line, col));
                        }
                    }
                    _ => {
                        self.errors.push(format!("Error:{}:{}: unknown escape sequence \\{}", line, col, esc));
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
            self.errors.push(format!("Error:{}:{}: unterminated f-string", line, col));
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
                    // Issue 29: hex escape \xHH
                    'x' => {
                        let (ch, ok) = self.read_hex_escape(line, col);
                        if ok { s.push(ch); }
                    }
                    // Issue 29: unicode escape \u{...}
                    'u' => {
                        if self.peek() == '{' {
                            self.advance(); // consume {
                            let (ch, ok) = self.read_unicode_escape(line, col);
                            if ok { s.push(ch); }
                        } else {
                            self.errors.push(format!("Error:{}:{}: expected '{{' after \\u", line, col));
                        }
                    }
                    _ => {
                        self.errors.push(format!("Error:{}:{}: unknown escape sequence \\{}", line, col, esc));
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
            self.errors.push(format!("Error:{}:{}: unterminated string", line, col));
        }

        Token::new(TokenKind::StringLit(s), line, col)
    }

    /// R1.7: char literal `'c'` → IntLit holding the Unicode scalar value, so
    /// it compares directly against `str_byte_at` results. Supports the same
    /// escapes as strings plus `\'`.
    fn read_char_lit(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        self.advance(); // consume opening '

        let ch = if self.pos >= self.source.len() || self.peek() == '\'' || self.peek() == '\n' {
            self.errors.push(format!("Error:{}:{}: empty char literal", line, col));
            '\0'
        } else if self.peek() == '\\' {
            self.advance();
            let esc = self.advance();
            match esc {
                'n' => '\n',
                't' => '\t',
                'r' => '\r',
                '\\' => '\\',
                '\'' => '\'',
                '"' => '"',
                '0' => '\0',
                'x' => self.read_hex_escape(line, col).0,
                'u' if self.peek() == '{' => {
                    self.advance(); // consume {
                    self.read_unicode_escape(line, col).0
                }
                _ => {
                    self.errors.push(format!("Error:{}:{}: unknown escape sequence \\{}", line, col, esc));
                    esc
                }
            }
        } else {
            self.advance()
        };

        if self.peek() == '\'' {
            self.advance(); // consume closing '
        } else {
            self.errors.push(format!("Error:{}:{}: unterminated char literal", line, col));
        }

        Token::new(TokenKind::IntLit(ch as i64), line, col)
    }

    /// Issue 29: Read a hex escape \xHH — reads exactly 2 hex digits.
    /// Returns (char, true) on success, (replacement char, false) on error.
    fn read_hex_escape(&mut self, line: usize, col: usize) -> (char, bool) {
        let mut val: u32 = 0;
        for _ in 0..2 {
            if self.pos < self.source.len() {
                let c = self.peek();
                if let Some(d) = c.to_digit(16) {
                    val = val * 16 + d;
                    self.advance();
                } else {
                    self.errors.push(format!("Error:{}:{}: \\x expects 2 hex digits", line, col));
                    return ('\0', false);
                }
            } else {
                self.errors.push(format!("Error:{}:{}: \\x expects 2 hex digits", line, col));
                return ('\0', false);
            }
        }
        if let Some(ch) = char::from_u32(val) {
            (ch, true)
        } else {
            self.errors.push(format!("Error:{}:{}: \\x{:02X} is not a valid char", line, col, val));
            ('\0', false)
        }
    }

    /// Issue 29: Read a unicode escape \u{XXXX} — reads hex digits until }.
    /// The opening { has already been consumed.
    /// Returns (char, true) on success, (replacement char, false) on error.
    fn read_unicode_escape(&mut self, line: usize, col: usize) -> (char, bool) {
        let mut val: u32 = 0;
        let mut count = 0;
        while self.pos < self.source.len() && self.peek() != '}' {
            let c = self.peek();
            if let Some(d) = c.to_digit(16) {
                val = val * 16 + d;
                count += 1;
                self.advance();
                if count > 6 {
                    self.errors.push(format!("Error:{}:{}: \\u{{...}} value too large", line, col));
                    // Skip to closing }
                    while self.pos < self.source.len() && self.peek() != '}' {
                        self.advance();
                    }
                    if self.pos < self.source.len() {
                        self.advance(); // consume }
                    }
                    return ('\0', false);
                }
            } else {
                self.errors.push(format!("Error:{}:{}: \\u{{...}} expects hex digits", line, col));
                while self.pos < self.source.len() && self.peek() != '}' {
                    self.advance();
                }
                if self.pos < self.source.len() {
                    self.advance(); // consume }
                }
                return ('\0', false);
            }
        }
        if self.pos < self.source.len() {
            self.advance(); // consume }
        } else {
            self.errors.push(format!("Error:{}:{}: \\u{{...}} missing closing }}", line, col));
            return ('\0', false);
        }
        if count == 0 {
            self.errors.push(format!("Error:{}:{}: \\u{{}} needs at least 1 hex digit", line, col));
            return ('\0', false);
        }
        if let Some(ch) = char::from_u32(val) {
            (ch, true)
        } else {
            self.errors.push(format!("Error:{}:{}: \\u{{{:X}}} is not a valid char", line, col, val));
            ('\0', false)
        }
    }

    /// Read a raw string: r"..." or r#"..."# (with N hash signs).
    /// No escape sequences are processed — the content is taken literally.
    /// The closing delimiter is " followed by the same number of # signs.
    fn read_raw_string(&mut self) -> Token {
        let line = self.line;
        let col = self.col;
        self.advance(); // consume 'r'

        // Count the number of # signs (0 or more)
        let mut hash_count = 0;
        while self.peek() == '#' {
            self.advance();
            hash_count += 1;
        }

        // Expect opening "
        if self.peek() != '"' {
            self.errors.push(format!("Error:{}:{}: expected '\"' after r{}", line, col, "#".repeat(hash_count)));
            return Token::new(TokenKind::StringLit(String::new()), line, col);
        }
        self.advance(); // consume opening "

        let mut s = String::new();
        // Read until we find " followed by exactly hash_count # signs
        while self.pos < self.source.len() {
            if self.peek() == '"' {
                // Check if this is the closing delimiter: " followed by hash_count # signs
                let mut matched = true;
                for i in 0..hash_count {
                    if self.peek_at(1 + i) != Some('#') {
                        matched = false;
                        break;
                    }
                }
                if matched {
                    self.advance(); // consume "
                    for _ in 0..hash_count {
                        self.advance(); // consume each #
                    }
                    return Token::new(TokenKind::StringLit(s), line, col);
                }
            }
            s.push(self.advance());
        }

        self.errors.push(format!("Error:{}:{}: unterminated raw string", line, col));
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
                self.errors.push(format!("Error:{}:{}: unexpected character '{}'", line, col, c));
                TokenKind::Eof
            }
        };

        Token::new(kind, line, col)
    }
}
