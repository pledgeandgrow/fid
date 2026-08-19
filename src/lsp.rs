//! Minimal LSP (Language Server Protocol) server for Fides.
//!
//! Implements:
//! - initialize / shutdown
//! - textDocument/didOpen
//! - textDocument/didChange
//! - textDocument/publishDiagnostics
//!
//! The LSP server communicates over stdio using JSON-RPC 2.0.
//! No external dependencies — uses a minimal JSON parser/serializer.

use std::io::{self, BufRead, Read, Write};
use crate::compiler;

// ─── Minimal JSON ───────────────────────────────────────────────────────

#[derive(Debug, Clone)]
pub enum JsonValue {
    Null,
    Bool(bool),
    Int(i64),
    Float(f64),
    Str(String),
    Array(Vec<JsonValue>),
    Object(Vec<(String, JsonValue)>),
}

impl JsonValue {
    fn get(&self, key: &str) -> Option<&JsonValue> {
        if let JsonValue::Object(entries) = self {
            entries.iter().find(|(k, _)| k == key).map(|(_, v)| v)
        } else {
            None
        }
    }

    fn as_str(&self) -> Option<&str> {
        if let JsonValue::Str(s) = self { Some(s) } else { None }
    }

    fn as_int(&self) -> Option<i64> {
        match self {
            JsonValue::Int(n) => Some(*n),
            JsonValue::Float(f) => Some(*f as i64),
            _ => None,
        }
    }

    fn to_string(&self) -> String {
        match self {
            JsonValue::Null => "null".to_string(),
            JsonValue::Bool(b) => b.to_string(),
            JsonValue::Int(n) => n.to_string(),
            JsonValue::Float(f) => f.to_string(),
            JsonValue::Str(s) => json_escape_string(s),
            JsonValue::Array(arr) => {
                let items: Vec<String> = arr.iter().map(|v| v.to_string()).collect();
                format!("[{}]", items.join(","))
            }
            JsonValue::Object(entries) => {
                let pairs: Vec<String> = entries.iter()
                    .map(|(k, v)| format!("{}:{}", json_escape_string(k), v.to_string()))
                    .collect();
                format!("{{{}}}", pairs.join(","))
            }
        }
    }
}

fn json_escape_string(s: &str) -> String {
    let mut result = String::with_capacity(s.len() + 2);
    result.push('"');
    for c in s.chars() {
        match c {
            '"' => result.push_str("\\\""),
            '\\' => result.push_str("\\\\"),
            '\n' => result.push_str("\\n"),
            '\r' => result.push_str("\\r"),
            '\t' => result.push_str("\\t"),
            c if (c as u32) < 0x20 => {
                result.push_str(&format!("\\u{:04x}", c as u32));
            }
            c => result.push(c),
        }
    }
    result.push('"');
    result
}

// ─── Minimal JSON Parser ────────────────────────────────────────────────

struct JsonParser {
    chars: Vec<char>,
    pos: usize,
}

impl JsonParser {
    fn new(s: &str) -> Self {
        JsonParser { chars: s.chars().collect(), pos: 0 }
    }

    fn skip_ws(&mut self) {
        while self.pos < self.chars.len() && self.chars[self.pos].is_whitespace() {
            self.pos += 1;
        }
    }

    fn parse_value(&mut self) -> Result<JsonValue, String> {
        self.skip_ws();
        if self.pos >= self.chars.len() {
            return Err("Unexpected end of input".to_string());
        }
        match self.chars[self.pos] {
            '{' => self.parse_object(),
            '[' => self.parse_array(),
            '"' => self.parse_string().map(JsonValue::Str),
            't' | 'f' => self.parse_bool(),
            'n' => self.parse_null(),
            '-' | '0'..='9' => self.parse_number(),
            _ => Err(format!("Unexpected character: {}", self.chars[self.pos])),
        }
    }

    fn parse_object(&mut self) -> Result<JsonValue, String> {
        self.pos += 1; // {
        let mut entries = Vec::new();
        self.skip_ws();
        if self.pos < self.chars.len() && self.chars[self.pos] == '}' {
            self.pos += 1;
            return Ok(JsonValue::Object(entries));
        }
        loop {
            self.skip_ws();
            let key = self.parse_string()?;
            self.skip_ws();
            if self.pos >= self.chars.len() || self.chars[self.pos] != ':' {
                return Err("Expected ':' in object".to_string());
            }
            self.pos += 1;
            let value = self.parse_value()?;
            entries.push((key, value));
            self.skip_ws();
            if self.pos >= self.chars.len() {
                return Err("Unexpected end of object".to_string());
            }
            match self.chars[self.pos] {
                ',' => { self.pos += 1; }
                '}' => { self.pos += 1; break; }
                _ => return Err("Expected ',' or '}' in object".to_string()),
            }
        }
        Ok(JsonValue::Object(entries))
    }

    fn parse_array(&mut self) -> Result<JsonValue, String> {
        self.pos += 1; // [
        let mut items = Vec::new();
        self.skip_ws();
        if self.pos < self.chars.len() && self.chars[self.pos] == ']' {
            self.pos += 1;
            return Ok(JsonValue::Array(items));
        }
        loop {
            let value = self.parse_value()?;
            items.push(value);
            self.skip_ws();
            if self.pos >= self.chars.len() {
                return Err("Unexpected end of array".to_string());
            }
            match self.chars[self.pos] {
                ',' => { self.pos += 1; }
                ']' => { self.pos += 1; break; }
                _ => return Err("Expected ',' or ']' in array".to_string()),
            }
        }
        Ok(JsonValue::Array(items))
    }

    fn parse_string(&mut self) -> Result<String, String> {
        if self.pos >= self.chars.len() || self.chars[self.pos] != '"' {
            return Err("Expected string".to_string());
        }
        self.pos += 1;
        let mut result = String::new();
        while self.pos < self.chars.len() {
            match self.chars[self.pos] {
                '"' => { self.pos += 1; return Ok(result); }
                '\\' => {
                    self.pos += 1;
                    if self.pos >= self.chars.len() {
                        return Err("Unexpected end of string escape".to_string());
                    }
                    match self.chars[self.pos] {
                        '"' => result.push('"'),
                        '\\' => result.push('\\'),
                        '/' => result.push('/'),
                        'n' => result.push('\n'),
                        'r' => result.push('\r'),
                        't' => result.push('\t'),
                        'u' => {
                            if self.pos + 4 >= self.chars.len() {
                                return Err("Invalid unicode escape".to_string());
                            }
                            let hex: String = self.chars[self.pos+1..self.pos+5].iter().collect();
                            if let Ok(code) = u32::from_str_radix(&hex, 16) {
                                if let Some(c) = char::from_u32(code) {
                                    result.push(c);
                                }
                            }
                            self.pos += 4;
                        }
                        c => result.push(c),
                    }
                    self.pos += 1;
                }
                c => { result.push(c); self.pos += 1; }
            }
        }
        Err("Unterminated string".to_string())
    }

    fn parse_bool(&mut self) -> Result<JsonValue, String> {
        if self.pos + 4 <= self.chars.len() && &self.chars[self.pos..self.pos+4] == &['t', 'r', 'u', 'e'] {
            self.pos += 4;
            Ok(JsonValue::Bool(true))
        } else if self.pos + 5 <= self.chars.len() && &self.chars[self.pos..self.pos+5] == &['f', 'a', 'l', 's', 'e'] {
            self.pos += 5;
            Ok(JsonValue::Bool(false))
        } else {
            Err("Invalid boolean".to_string())
        }
    }

    fn parse_null(&mut self) -> Result<JsonValue, String> {
        if self.pos + 4 <= self.chars.len() && &self.chars[self.pos..self.pos+4] == &['n', 'u', 'l', 'l'] {
            self.pos += 4;
            Ok(JsonValue::Null)
        } else {
            Err("Invalid null".to_string())
        }
    }

    fn parse_number(&mut self) -> Result<JsonValue, String> {
        let start = self.pos;
        if self.chars[self.pos] == '-' { self.pos += 1; }
        while self.pos < self.chars.len() && (self.chars[self.pos].is_ascii_digit() || self.chars[self.pos] == '.' || self.chars[self.pos] == 'e' || self.chars[self.pos] == 'E' || self.chars[self.pos] == '+' || self.chars[self.pos] == '-') {
            self.pos += 1;
        }
        let s: String = self.chars[start..self.pos].iter().collect();
        if s.contains('.') || s.contains('e') || s.contains('E') {
            s.parse::<f64>().map(JsonValue::Float).map_err(|e| e.to_string())
        } else {
            s.parse::<i64>().map(JsonValue::Int).map_err(|e| e.to_string())
        }
    }
}

pub fn parse_json(s: &str) -> Result<JsonValue, String> {
    let mut parser = JsonParser::new(s);
    parser.parse_value()
}

// ─── LSP Server ─────────────────────────────────────────────────────────

/// Run the LSP server over stdio.
pub fn run() -> i32 {
    let stdin = io::stdin();
    let mut stdin = stdin.lock();
    let stdout = io::stdout();
    let mut stdout = stdout.lock();

    // Track open documents: URI -> source text
    let mut documents: std::collections::HashMap<String, String> = std::collections::HashMap::new();

    loop {
        // Read Content-Length header
        let mut content_length = 0usize;
        loop {
            let mut line = String::new();
            match stdin.read_line(&mut line) {
                Ok(0) => return 0, // EOF
                Ok(_) => {}
                Err(_) => return 1,
            }
            let trimmed = line.trim();
            if trimmed.is_empty() {
                break; // End of headers
            }
            if let Some(len_str) = trimmed.strip_prefix("Content-Length: ") {
                content_length = len_str.parse().unwrap_or(0);
            }
        }

        if content_length == 0 {
            continue;
        }

        // Read body
        let mut body = vec![0u8; content_length];
        if stdin.read_exact(&mut body).is_err() {
            continue;
        }
        let body_str = String::from_utf8_lossy(&body);

        // Parse JSON-RPC message
        let msg = match parse_json(&body_str) {
            Ok(m) => m,
            Err(_) => continue,
        };

        let method = msg.get("method").and_then(|v| v.as_str()).unwrap_or("");
        let id = msg.get("id").cloned();
        let params = msg.get("params").cloned().unwrap_or(JsonValue::Null);

        match method {
            "initialize" => {
                let result = JsonValue::Object(vec![
                    ("capabilities".to_string(), JsonValue::Object(vec![
                        ("textDocumentSync".to_string(), JsonValue::Int(1)), // Full sync
                        ("diagnosticProvider".to_string(), JsonValue::Object(vec![])),
                    ])),
                    ("serverInfo".to_string(), JsonValue::Object(vec![
                        ("name".to_string(), JsonValue::Str("fides-lsp".to_string())),
                        ("version".to_string(), JsonValue::Str("0.1.0".to_string())),
                    ])),
                ]);
                send_response(&mut stdout, id.unwrap(), result);
            }
            "initialized" => {
                // No response needed for notification
            }
            "shutdown" => {
                send_response(&mut stdout, id.unwrap(), JsonValue::Null);
            }
            "exit" => {
                return 0;
            }
            "textDocument/didOpen" => {
                if let Some(text_doc) = params.get("textDocument") {
                    let uri = text_doc.get("uri").and_then(|v| v.as_str()).unwrap_or("").to_string();
                    let text = text_doc.get("text").and_then(|v| v.as_str()).unwrap_or("").to_string();
                    documents.insert(uri.clone(), text.clone());
                    publish_diagnostics(&mut stdout, &uri, &text);
                }
            }
            "textDocument/didChange" => {
                if let Some(text_doc) = params.get("textDocument") {
                    let uri = text_doc.get("uri").and_then(|v| v.as_str()).unwrap_or("").to_string();
                    if let Some(changes) = params.get("contentChanges") {
                        if let JsonValue::Array(arr) = changes {
                            if let Some(first) = arr.first() {
                                if let Some(text) = first.get("text").and_then(|v| v.as_str()) {
                                    documents.insert(uri.clone(), text.to_string());
                                    publish_diagnostics(&mut stdout, &uri, text);
                                }
                            }
                        }
                    }
                }
            }
            "textDocument/didClose" => {
                if let Some(text_doc) = params.get("textDocument") {
                    let uri = text_doc.get("uri").and_then(|v| v.as_str()).unwrap_or("").to_string();
                    documents.remove(&uri);
                }
            }
            _ => {
                // Unknown method — if it has an id, return an error
                if let Some(id) = id {
                    let error = JsonValue::Object(vec![
                        ("code".to_string(), JsonValue::Int(-32601)),
                        ("message".to_string(), JsonValue::Str(format!("Method not found: {}", method))),
                    ]);
                    send_error(&mut stdout, id, error);
                }
            }
        }
    }
}

fn publish_diagnostics(stdout: &mut impl Write, uri: &str, text: &str) {
    // Get diagnostics from the compiler
    let source_path = if uri.starts_with("file://") {
        Some(&uri[7..])
    } else {
        None
    };

    let diags = compiler::get_diagnostics(text, source_path);

    let diag_array: Vec<JsonValue> = diags.iter().map(|(line, col, msg)| {
        JsonValue::Object(vec![
            ("range".to_string(), JsonValue::Object(vec![
                ("start".to_string(), JsonValue::Object(vec![
                    ("line".to_string(), JsonValue::Int(*line as i64 - 1)),
                    ("character".to_string(), JsonValue::Int(*col as i64 - 1)),
                ])),
                ("end".to_string(), JsonValue::Object(vec![
                    ("line".to_string(), JsonValue::Int(*line as i64 - 1)),
                    ("character".to_string(), JsonValue::Int(*col as i64)),
                ])),
            ])),
            ("severity".to_string(), JsonValue::Int(1)), // Error
            ("message".to_string(), JsonValue::Str(msg.clone())),
            ("source".to_string(), JsonValue::Str("fides".to_string())),
        ])
    }).collect();

    let notification = JsonValue::Object(vec![
        ("jsonrpc".to_string(), JsonValue::Str("2.0".to_string())),
        ("method".to_string(), JsonValue::Str("textDocument/publishDiagnostics".to_string())),
        ("params".to_string(), JsonValue::Object(vec![
            ("uri".to_string(), JsonValue::Str(uri.to_string())),
            ("diagnostics".to_string(), JsonValue::Array(diag_array)),
        ])),
    ]);

    send_notification(stdout, notification);
}

fn send_response(stdout: &mut impl Write, id: JsonValue, result: JsonValue) {
    let response = JsonValue::Object(vec![
        ("jsonrpc".to_string(), JsonValue::Str("2.0".to_string())),
        ("id".to_string(), id),
        ("result".to_string(), result),
    ]);
    send_notification(stdout, response);
}

fn send_error(stdout: &mut impl Write, id: JsonValue, error: JsonValue) {
    let response = JsonValue::Object(vec![
        ("jsonrpc".to_string(), JsonValue::Str("2.0".to_string())),
        ("id".to_string(), id),
        ("error".to_string(), error),
    ]);
    send_notification(stdout, response);
}

fn send_notification(stdout: &mut impl Write, msg: JsonValue) {
    let json = msg.to_string();
    let output = format!("Content-Length: {}\r\n\r\n{}", json.len(), json);
    let _ = stdout.write_all(output.as_bytes());
    let _ = stdout.flush();
}
