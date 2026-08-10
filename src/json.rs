// Minimal JSON module for the Pledge runtime.
// Provides parse, dump, and escape_string functions used by p_json_* builtins.

use std::collections::BTreeMap;

#[derive(Debug, Clone, PartialEq)]
pub enum Json {
    Null,
    Bool(bool),
    Number(f64),
    String(String),
    Array(Vec<Json>),
    Object(BTreeMap<String, Json>),
}

pub type ParseResult = Result<Json, String>;

pub fn parse(s: &str) -> ParseResult {
    let mut parser = JsonParser {
        chars: s.chars().collect(),
        pos: 0,
    };
    parser.skip_ws();
    let v = parser.parse_value()?;
    parser.skip_ws();
    if parser.pos < parser.chars.len() {
        return Err(format!("Unexpected trailing characters at position {}", parser.pos));
    }
    Ok(v)
}

pub fn dump(v: &Json, pretty: bool) -> String {
    let mut out = String::new();
    dump_inner(v, &mut out, pretty, 0);
    out
}

pub fn escape_string(s: &str) -> String {
    let mut out = String::from("\"");
    for c in s.chars() {
        match c {
            '"' => out.push_str("\\\""),
            '\\' => out.push_str("\\\\"),
            '\n' => out.push_str("\\n"),
            '\r' => out.push_str("\\r"),
            '\t' => out.push_str("\\t"),
            c if (c as u32) < 0x20 => out.push_str(&format!("\\u{:04x}", c as u32)),
            c => out.push(c),
        }
    }
    out.push('"');
    out
}

fn dump_inner(v: &Json, out: &mut String, pretty: bool, indent: usize) {
    match v {
        Json::Null => out.push_str("null"),
        Json::Bool(b) => out.push_str(if *b { "true" } else { "false" }),
        Json::Number(n) => {
            if n.fract() == 0.0 && n.abs() < 1e15 {
                out.push_str(&format!("{}", *n as i64));
            } else {
                out.push_str(&format!("{}", n));
            }
        }
        Json::String(s) => out.push_str(&escape_string(s)),
        Json::Array(arr) => {
            if arr.is_empty() {
                out.push_str("[]");
                return;
            }
            out.push('[');
            for (i, elem) in arr.iter().enumerate() {
                if i > 0 { out.push(','); }
                if pretty {
                    out.push('\n');
                    for _ in 0..(indent + 1) { out.push_str("  "); }
                }
                dump_inner(elem, out, pretty, indent + 1);
            }
            if pretty {
                out.push('\n');
                for _ in 0..indent { out.push_str("  "); }
            }
            out.push(']');
        }
        Json::Object(map) => {
            if map.is_empty() {
                out.push_str("{}");
                return;
            }
            out.push('{');
            for (i, (k, v)) in map.iter().enumerate() {
                if i > 0 { out.push(','); }
                if pretty {
                    out.push('\n');
                    for _ in 0..(indent + 1) { out.push_str("  "); }
                }
                out.push_str(&escape_string(k));
                out.push(':');
                if pretty { out.push(' '); }
                dump_inner(v, out, pretty, indent + 1);
            }
            if pretty {
                out.push('\n');
                for _ in 0..indent { out.push_str("  "); }
            }
            out.push('}');
        }
    }
}

struct JsonParser {
    chars: Vec<char>,
    pos: usize,
}

impl JsonParser {
    fn skip_ws(&mut self) {
        while self.pos < self.chars.len() && self.chars[self.pos].is_whitespace() {
            self.pos += 1;
        }
    }

    fn parse_value(&mut self) -> ParseResult {
        self.skip_ws();
        if self.pos >= self.chars.len() {
            return Err("Unexpected end of input".to_string());
        }
        match self.chars[self.pos] {
            'n' => self.parse_null(),
            't' | 'f' => self.parse_bool(),
            '"' => self.parse_string(),
            '[' => self.parse_array(),
            '{' => self.parse_object(),
            c if c == '-' || c.is_ascii_digit() => self.parse_number(),
            c => Err(format!("Unexpected character '{}' at position {}", c, self.pos)),
        }
    }

    fn parse_null(&mut self) -> ParseResult {
        if self.pos + 4 <= self.chars.len() && &self.chars[self.pos..self.pos + 4] == &['n', 'u', 'l', 'l'] {
            self.pos += 4;
            Ok(Json::Null)
        } else {
            Err("Invalid null literal".to_string())
        }
    }

    fn parse_bool(&mut self) -> ParseResult {
        if self.pos + 4 <= self.chars.len() && &self.chars[self.pos..self.pos + 4] == &['t', 'r', 'u', 'e'] {
            self.pos += 4;
            return Ok(Json::Bool(true));
        }
        if self.pos + 5 <= self.chars.len() && &self.chars[self.pos..self.pos + 5] == &['f', 'a', 'l', 's', 'e'] {
            self.pos += 5;
            return Ok(Json::Bool(false));
        }
        Err("Invalid boolean literal".to_string())
    }

    fn parse_string(&mut self) -> ParseResult {
        self.pos += 1; // skip opening quote
        let mut s = String::new();
        while self.pos < self.chars.len() {
            match self.chars[self.pos] {
                '"' => { self.pos += 1; return Ok(Json::String(s)); }
                '\\' => {
                    self.pos += 1;
                    if self.pos >= self.chars.len() {
                        return Err("Unterminated escape sequence".to_string());
                    }
                    match self.chars[self.pos] {
                        '"' => s.push('"'),
                        '\\' => s.push('\\'),
                        '/' => s.push('/'),
                        'n' => s.push('\n'),
                        'r' => s.push('\r'),
                        't' => s.push('\t'),
                        'u' => {
                            if self.pos + 4 >= self.chars.len() {
                                return Err("Invalid unicode escape".to_string());
                            }
                            let hex: String = self.chars[self.pos + 1..self.pos + 5].iter().collect();
                            match u32::from_str_radix(&hex, 16) {
                                Ok(code) => {
                                    if let Some(c) = char::from_u32(code) {
                                        s.push(c);
                                    }
                                }
                                Err(_) => return Err("Invalid unicode escape".to_string()),
                            }
                            self.pos += 4;
                        }
                        c => return Err(format!("Invalid escape character '{}'", c)),
                    }
                    self.pos += 1;
                }
                c => { s.push(c); self.pos += 1; }
            }
        }
        Err("Unterminated string".to_string())
    }

    fn parse_number(&mut self) -> ParseResult {
        let start = self.pos;
        if self.pos < self.chars.len() && self.chars[self.pos] == '-' {
            self.pos += 1;
        }
        while self.pos < self.chars.len() && self.chars[self.pos].is_ascii_digit() {
            self.pos += 1;
        }
        if self.pos < self.chars.len() && self.chars[self.pos] == '.' {
            self.pos += 1;
            while self.pos < self.chars.len() && self.chars[self.pos].is_ascii_digit() {
                self.pos += 1;
            }
        }
        if self.pos < self.chars.len() && (self.chars[self.pos] == 'e' || self.chars[self.pos] == 'E') {
            self.pos += 1;
            if self.pos < self.chars.len() && (self.chars[self.pos] == '+' || self.chars[self.pos] == '-') {
                self.pos += 1;
            }
            while self.pos < self.chars.len() && self.chars[self.pos].is_ascii_digit() {
                self.pos += 1;
            }
        }
        let s: String = self.chars[start..self.pos].iter().collect();
        s.parse::<f64>()
            .map(Json::Number)
            .map_err(|_| format!("Invalid number: {}", s))
    }

    fn parse_array(&mut self) -> ParseResult {
        self.pos += 1; // skip [
        self.skip_ws();
        let mut arr = Vec::new();
        if self.pos < self.chars.len() && self.chars[self.pos] == ']' {
            self.pos += 1;
            return Ok(Json::Array(arr));
        }
        loop {
            let v = self.parse_value()?;
            arr.push(v);
            self.skip_ws();
            if self.pos >= self.chars.len() {
                return Err("Unterminated array".to_string());
            }
            match self.chars[self.pos] {
                ',' => { self.pos += 1; self.skip_ws(); }
                ']' => { self.pos += 1; return Ok(Json::Array(arr)); }
                c => return Err(format!("Expected ',' or ']' but got '{}' at position {}", c, self.pos)),
            }
        }
    }

    fn parse_object(&mut self) -> ParseResult {
        self.pos += 1; // skip {
        self.skip_ws();
        let mut map = BTreeMap::new();
        if self.pos < self.chars.len() && self.chars[self.pos] == '}' {
            self.pos += 1;
            return Ok(Json::Object(map));
        }
        loop {
            self.skip_ws();
            let key = match self.parse_string()? {
                Json::String(s) => s,
                _ => return Err("Expected string key in object".to_string()),
            };
            self.skip_ws();
            if self.pos >= self.chars.len() || self.chars[self.pos] != ':' {
                return Err("Expected ':' after key in object".to_string());
            }
            self.pos += 1; // skip :
            let v = self.parse_value()?;
            map.insert(key, v);
            self.skip_ws();
            if self.pos >= self.chars.len() {
                return Err("Unterminated object".to_string());
            }
            match self.chars[self.pos] {
                ',' => { self.pos += 1; }
                '}' => { self.pos += 1; return Ok(Json::Object(map)); }
                c => return Err(format!("Expected ',' or '}}' but got '{}' at position {}", c, self.pos)),
            }
        }
    }
}
