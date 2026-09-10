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
                        ("completionProvider".to_string(), JsonValue::Object(vec![
                            ("triggerCharacters".to_string(), JsonValue::Array(vec![
                                JsonValue::Str(".".to_string()),
                            ])),
                            ("resolveProvider".to_string(), JsonValue::Bool(false)),
                        ])),
                        ("hoverProvider".to_string(), JsonValue::Bool(true)),
                        ("definitionProvider".to_string(), JsonValue::Bool(true)),
                        ("renameProvider".to_string(), JsonValue::Bool(true)),
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
            "textDocument/completion" => {
                let uri = params.get("textDocument")
                    .and_then(|v| v.get("uri"))
                    .and_then(|v| v.as_str())
                    .unwrap_or("")
                    .to_string();
                let line = params.get("position")
                    .and_then(|v| v.get("line"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let character = params.get("position")
                    .and_then(|v| v.get("character"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let text = documents.get(&uri).map(|s| s.as_str()).unwrap_or("");
                let result = handle_completion(text, line, character);
                send_response(&mut stdout, id.unwrap(), result);
            }
            "textDocument/hover" => {
                let uri = params.get("textDocument")
                    .and_then(|v| v.get("uri"))
                    .and_then(|v| v.as_str())
                    .unwrap_or("")
                    .to_string();
                let line = params.get("position")
                    .and_then(|v| v.get("line"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let character = params.get("position")
                    .and_then(|v| v.get("character"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let text = documents.get(&uri).map(|s| s.as_str()).unwrap_or("");
                let result = handle_hover(text, line, character);
                send_response(&mut stdout, id.unwrap(), result);
            }
            "textDocument/definition" => {
                let uri = params.get("textDocument")
                    .and_then(|v| v.get("uri"))
                    .and_then(|v| v.as_str())
                    .unwrap_or("")
                    .to_string();
                let line = params.get("position")
                    .and_then(|v| v.get("line"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let character = params.get("position")
                    .and_then(|v| v.get("character"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let text = documents.get(&uri).map(|s| s.as_str()).unwrap_or("");
                let result = handle_definition(text, &uri, line, character);
                send_response(&mut stdout, id.unwrap(), result);
            }
            "textDocument/rename" => {
                let uri = params.get("textDocument")
                    .and_then(|v| v.get("uri"))
                    .and_then(|v| v.as_str())
                    .unwrap_or("")
                    .to_string();
                let line = params.get("position")
                    .and_then(|v| v.get("line"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let character = params.get("position")
                    .and_then(|v| v.get("character"))
                    .and_then(|v| v.as_int())
                    .unwrap_or(0) as usize;
                let new_name = params.get("newName")
                    .and_then(|v| v.as_str())
                    .unwrap_or("new_name");
                let text = documents.get(&uri).map(|s| s.as_str()).unwrap_or("");
                let result = handle_rename(text, &uri, line, character, new_name);
                send_response(&mut stdout, id.unwrap(), result);
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

// ─── Completion / Hover / Goto-Definition ─────────────────────────────────

/// Fides keywords for completion.
const KEYWORDS: &[&str] = &[
    "fn", "val", "var", "const", "type", "pub", "struct", "enum", "impl", "cap",
    "if", "elif", "else", "while", "for", "in", "loop", "break", "continue",
    "return", "match", "use", "region", "sandbox", "unsafe", "txn", "do",
    "handle", "with", "await", "async", "comptime",
];

/// Check if a character is a valid identifier character.
fn is_ident_char(c: char) -> bool {
    c.is_alphanumeric() || c == '_'
}

/// Extract the word at a given (0-based line, 0-based character) position.
/// The character position may be on the word or just after it.
fn word_at_position(text: &str, line: usize, character: usize) -> String {
    let lines: Vec<&str> = text.lines().collect();
    if line >= lines.len() {
        return String::new();
    }
    let line_str = lines[line];
    let chars: Vec<char> = line_str.chars().collect();
    if chars.is_empty() {
        return String::new();
    }
    // Clamp character to valid range
    let pos = character.min(chars.len());
    // If the cursor is right after the word (pos is past the last ident char),
    // step back to find the word.
    let mut end = pos;
    let mut start = pos;
    // If at end of line or current char is not an ident char, step back
    if pos == chars.len() || !is_ident_char(chars[pos]) {
        if pos > 0 && is_ident_char(chars[pos - 1]) {
            end = pos;
            start = pos;
            while start > 0 && is_ident_char(chars[start - 1]) {
                start -= 1;
            }
            return chars[start..end].iter().collect();
        }
        return String::new();
    }
    // Cursor is on an ident char — expand both directions
    while start > 0 && is_ident_char(chars[start - 1]) {
        start -= 1;
    }
    while end < chars.len() && is_ident_char(chars[end]) {
        end += 1;
    }
    chars[start..end].iter().collect()
}

/// Check if the cursor is immediately after a `.` (dot-access for methods/fields).
/// Returns the identifier before the dot if so.
fn ident_before_dot(text: &str, line: usize, character: usize) -> Option<String> {
    let lines: Vec<&str> = text.lines().collect();
    if line >= lines.len() {
        return None;
    }
    let line_str = lines[line];
    let chars: Vec<char> = line_str.chars().collect();
    if chars.is_empty() {
        return None;
    }
    let pos = character.min(chars.len());
    if pos == 0 {
        return None;
    }
    // The character right before the cursor should be '.'
    if chars[pos - 1] != '.' {
        return None;
    }
    // Find the identifier before the dot
    let mut end = pos - 1; // position of '.'
    if end == 0 {
        return None;
    }
    let mut start = end - 1;
    // Skip whitespace before the dot
    while start > 0 && chars[start].is_whitespace() {
        start -= 1;
    }
    if !is_ident_char(chars[start]) {
        return None;
    }
    end = start + 1;
    while start > 0 && is_ident_char(chars[start - 1]) {
        start -= 1;
    }
    Some(chars[start..end].iter().collect())
}

/// Build a CompletionItem JSON object.
fn completion_item(label: &str, kind: i64, detail: &str) -> JsonValue {
    JsonValue::Object(vec![
        ("label".to_string(), JsonValue::Str(label.to_string())),
        ("kind".to_string(), JsonValue::Int(kind)),
        ("detail".to_string(), JsonValue::Str(detail.to_string())),
    ])
}

/// Handle textDocument/completion.
fn handle_completion(text: &str, line: usize, character: usize) -> JsonValue {
    let (_program, tc) = compiler::build_program_for_lsp(text);

    let mut items: Vec<JsonValue> = Vec::new();

    // Check if we're after a dot (method/field completion)
    if let Some(receiver_name) = ident_before_dot(text, line, character) {
        // Try to find the type of the receiver from scopes
        // For now, return all methods from all types that have methods
        // Also try to match the receiver to a known type
        let receiver_lower = receiver_name.to_lowercase();

        // If the receiver looks like a known type name (e.g. "Vec", "HashMap"),
        // return its methods directly
        let methods = tc.get_methods();
        for (struct_name, method_map) in methods {
            if struct_name.to_lowercase() == receiver_lower
                || *struct_name == receiver_name
                || receiver_name == "vec" && struct_name.contains("Vec")
                || receiver_name == "map" && struct_name.contains("HashMap")
                || receiver_name == "str" && (struct_name.contains("String") || struct_name.contains("Str"))
            {
                for (method_name, (params, ret, _has_self, _is_pub)) in method_map {
                    let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
                    let detail = format!("fn {}({}) -> {}", method_name, param_strs.join(", "), ret);
                    items.push(completion_item(method_name, 2, &detail)); // 2 = Method
                }
            }
        }

        // Also return struct fields if the receiver is a known struct
        let structs = tc.get_structs();
        if let Some(fields) = structs.get(&receiver_name) {
            for (field_name, field_ty) in fields {
                items.push(completion_item(field_name, 5, &format!("{}: {}", field_name, field_ty))); // 5 = Field
            }
        }

        // If we found any items, return them; otherwise return all methods
        // (useful when the receiver type can't be determined)
        if items.is_empty() {
            for (struct_name, method_map) in methods {
                let _ = struct_name; // unused but needed for iteration
                for (method_name, (params, ret, _has_self, _is_pub)) in method_map {
                    let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
                    let detail = format!("fn {}({}) -> {}", method_name, param_strs.join(", "), ret);
                    items.push(completion_item(method_name, 2, &detail));
                }
            }
        }

        return JsonValue::Array(items);
    }

    // Global completions: keywords, functions, structs, enums, consts

    // Keywords
    for kw in KEYWORDS {
        items.push(completion_item(kw, 14, "keyword")); // 14 = Keyword
    }

    // Functions
    for (name, (params, ret)) in tc.get_funcs() {
        let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
        let detail = format!("fn {}({}) -> {}", name, param_strs.join(", "), ret);
        items.push(completion_item(name, 3, &detail)); // 3 = Function
    }

    // Structs
    for (name, fields) in tc.get_structs() {
        let field_strs: Vec<String> = fields.iter().map(|(f, t)| format!("{}: {}", f, t)).collect();
        let detail = format!("struct {} {{ {} }}", name, field_strs.join(", "));
        items.push(completion_item(name, 22, &detail)); // 22 = Struct
    }

    // Enums
    for name in tc.get_enums() {
        items.push(completion_item(name, 20, "enum")); // 20 = Enum
    }

    // Constants
    for (name, ty) in tc.get_consts() {
        items.push(completion_item(name, 21, &format!("const {}: {}", name, ty))); // 21 = Constant
    }

    // Type aliases
    for (name, ty) in tc.get_type_aliases() {
        items.push(completion_item(name, 9, &format!("type {} = {}", name, ty))); // 9 = Module? use Class
    }

    // Traits
    for name in tc.get_traits().keys() {
        items.push(completion_item(name, 8, "trait")); // 8 = Interface
    }

    JsonValue::Array(items)
}

/// Handle textDocument/hover.
fn handle_hover(text: &str, line: usize, character: usize) -> JsonValue {
    let word = word_at_position(text, line, character);
    if word.is_empty() {
        return JsonValue::Null;
    }

    let (_program, tc) = compiler::build_program_for_lsp(text);

    let content = if let Some((params, ret)) = tc.get_funcs().get(&word) {
        let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
        format!("**fn {}({}) → {}**", word, param_strs.join(", "), ret)
    } else if let Some(fields) = tc.get_structs().get(&word) {
        let field_strs: Vec<String> = fields.iter().map(|(f, t)| format!("{}: {}", f, t)).collect();
        format!("**struct {}**\n\n{{ {} }}", word, field_strs.join(", "))
    } else if tc.get_enums().contains(&word) {
        format!("**enum {}**", word)
    } else if let Some(ty) = tc.get_consts().get(&word) {
        format!("**const {}**: {}", word, ty)
    } else if let Some(ty) = tc.get_type_aliases().get(&word) {
        format!("**type {}** = {}", word, ty)
    } else if tc.get_traits().contains_key(&word) {
        format!("**trait {}**", word)
    } else {
        // Check if it's a struct field type or method
        let mut found = None;
        for (struct_name, method_map) in tc.get_methods() {
            if let Some((params, ret, has_self, _is_pub)) = method_map.get(&word) {
                let param_strs: Vec<String> = params.iter().map(|t| t.to_string()).collect();
                let self_note = if *has_self { "method" } else { "function" };
                found = Some(format!("**{} {}::{}({}) → {}**", self_note, struct_name, word, param_strs.join(", "), ret));
                break;
            }
        }
        if let Some(s) = found {
            s
        } else {
            return JsonValue::Null;
        }
    };

    let hover = JsonValue::Object(vec![
        ("contents".to_string(), JsonValue::Object(vec![
            ("kind".to_string(), JsonValue::Str("markdown".to_string())),
            ("value".to_string(), JsonValue::Str(content)),
        ])),
    ]);
    hover
}

/// Handle textDocument/definition.
/// Uses the typechecker's symbol table (AST-based) to resolve definitions.
/// Falls back to text search only for methods (which don't have line info yet).
fn handle_definition(text: &str, uri: &str, line: usize, character: usize) -> JsonValue {
    let word = word_at_position(text, line, character);
    if word.is_empty() {
        return JsonValue::Null;
    }

    let (_program, tc) = compiler::build_program_for_lsp(text);

    // Determine what kind of symbol we're looking for
    let is_func = tc.get_funcs().contains_key(&word);
    let is_struct = tc.get_structs().contains_key(&word);
    let is_enum = tc.get_enums().contains(&word);
    let is_const = tc.get_consts().contains_key(&word);
    let is_trait = tc.get_traits().contains_key(&word);

    if !is_func && !is_struct && !is_enum && !is_const && !is_trait {
        // Check if it's a method name
        for (_struct_name, method_map) in tc.get_methods() {
            if method_map.contains_key(&word) {
                if let Some((def_line, def_col)) = find_definition(text, &word, true) {
                    return location_json(uri, def_line, def_col, &word);
                }
                return JsonValue::Null;
            }
        }
        return JsonValue::Null;
    }

    // Issue 37: Use AST-based line info from the typechecker when available.
    // The typechecker records the source line of each top-level item.
    let item_line = tc.get_item_line(&word);
    if item_line > 0 {
        // Find the column of the identifier on that line.
        // We look for the word as a whole word on the definition line.
        let line_text = text.lines().nth(item_line.saturating_sub(1)).unwrap_or("");
        if let Some(col) = find_ident_column(line_text, &word) {
            return location_json(uri, item_line.saturating_sub(1), col, &word);
        }
        // Fallback: column 0 if we can't find the identifier on the line
        return location_json(uri, item_line.saturating_sub(1), 0, &word);
    }

    // Fallback: text search for items without line info (e.g. synthetic items)
    if let Some((def_line, def_col)) = find_definition(text, &word, is_func) {
        return location_json(uri, def_line, def_col, &word);
    }

    JsonValue::Null
}

/// Find the 0-based column of `name` as a whole word in `line`.
/// Used by handle_definition to pinpoint the identifier on a known definition line.
fn find_ident_column(line: &str, name: &str) -> Option<usize> {
    let mut search_from = 0;
    while let Some(pos) = line[search_from..].find(name) {
        let abs = search_from + pos;
        // Check word boundary before
        let ok_before = abs == 0 || {
            let prev = line.chars().nth(abs.saturating_sub(1)).unwrap_or(' ');
            !is_ident_char(prev)
        };
        // Check word boundary after
        let after = abs + name.len();
        let ok_after = after >= line.len() || {
            let next = line.chars().nth(after).unwrap_or(' ');
            !is_ident_char(next)
        };
        if ok_before && ok_after {
            return Some(abs);
        }
        search_from = abs + 1;
        if search_from >= line.len() {
            break;
        }
    }
    None
}

/// Handle textDocument/rename — find all occurrences of the symbol at the given
/// position and return a WorkspaceEdit with TextEdits to rename them.
fn handle_rename(text: &str, uri: &str, line: usize, character: usize, new_name: &str) -> JsonValue {
    let word = word_at_position(text, line, character);
    if word.is_empty() {
        return JsonValue::Null;
    }

    let (_program, tc) = compiler::build_program_for_lsp(text);

    // Determine what kind of symbol we're looking for
    let is_func = tc.get_funcs().contains_key(&word);
    let is_struct = tc.get_structs().contains_key(&word);
    let is_enum = tc.get_enums().contains(&word);
    let is_const = tc.get_consts().contains_key(&word);
    let is_trait = tc.get_traits().contains_key(&word);
    let is_method = tc.get_methods().values().any(|m| m.contains_key(&word));

    if !is_func && !is_struct && !is_enum && !is_const && !is_trait && !is_method {
        return JsonValue::Null;
    }

    // Find all occurrences of the word as whole-word matches in the source
    let edits = find_all_occurrences(text, &word);
    if edits.is_empty() {
        return JsonValue::Null;
    }

    // Build the LSP WorkspaceEdit response
    let edit_objects: Vec<JsonValue> = edits.iter().map(|(l, c, len)| {
        JsonValue::Object(vec![
            ("range".to_string(), JsonValue::Object(vec![
                ("start".to_string(), JsonValue::Object(vec![
                    ("line".to_string(), JsonValue::Int(*l as i64)),
                    ("character".to_string(), JsonValue::Int(*c as i64)),
                ])),
                ("end".to_string(), JsonValue::Object(vec![
                    ("line".to_string(), JsonValue::Int(*l as i64)),
                    ("character".to_string(), JsonValue::Int((*c + *len) as i64)),
                ])),
            ])),
            ("newText".to_string(), JsonValue::Str(new_name.to_string())),
        ])
    }).collect();

    JsonValue::Object(vec![
        ("changes".to_string(), JsonValue::Object(vec![
            (uri.to_string(), JsonValue::Array(edit_objects)),
        ])),
    ])
}

/// Find all whole-word occurrences of `name` in the source text.
/// Returns (line, column, length) for each occurrence.
fn find_all_occurrences(text: &str, name: &str) -> Vec<(usize, usize, usize)> {
    let mut results = Vec::new();
    let name_len = name.len();
    if name_len == 0 {
        return results;
    }
    for (line_idx, line_str) in text.lines().enumerate() {
        let _col = 0;
        let bytes = line_str.as_bytes();
        let mut i = 0;
        while i + name_len <= bytes.len() {
            // Check if we have a whole-word match at position i
            let slice = &line_str[i..i + name_len];
            if slice == name {
                // Check word boundary before
                let ok_before = i == 0 || !is_ident_char(bytes[i - 1] as char);
                // Check word boundary after
                let ok_after = i + name_len == bytes.len() || !is_ident_char(bytes[i + name_len] as char);
                if ok_before && ok_after {
                    results.push((line_idx, i, name_len));
                }
            }
            i += 1;
        }
    }
    results
}

/// Search the source text for a definition of the given symbol name.
/// Returns (0-based line, 0-based column) if found.
/// For functions, looks for patterns like `pf name`, `pv name`, `pub pf name`.
/// For structs, looks for `struct Name`.
/// For enums, looks for `enum Name`.
fn find_definition(text: &str, name: &str, is_func: bool) -> Option<(usize, usize)> {
    for (line_idx, line_str) in text.lines().enumerate() {
        if is_func {
            // Look for function definition patterns:
            //   pf name(   |   pv name(   |   pub pf name(   |   pub pv name(
            // Also: pm name( is not a function, but cap methods use pf/pv
            for kw in &["fn ", "val "] {
                if let Some(pos) = find_word_after(line_str, kw, name) {
                    return Some((line_idx, pos));
                }
            }
            // Also check "pub fn" / "pub val"
            for kw in &["pub fn ", "pub val "] {
                if let Some(pos) = find_word_after(line_str, kw, name) {
                    return Some((line_idx, pos));
                }
            }
        } else {
            // Look for struct/enum/const/trait/type definitions
            for kw in &["struct ", "enum ", "const ", "type ", "cap ", "pub struct ", "pub enum ", "pub const ", "pub type ", "pub cap "] {
                if let Some(pos) = find_word_after(line_str, kw, name) {
                    return Some((line_idx, pos));
                }
            }
        }
    }
    None
}

/// Check if `line` contains `keyword` followed by `name` (as a whole word).
/// Returns the 0-based column of `name` if found.
fn find_word_after(line: &str, keyword: &str, name: &str) -> Option<usize> {
    let line_lower = line.to_lowercase();
    let kw_lower = keyword.to_lowercase();
    let mut search_from = 0;
    while let Some(kw_pos) = line_lower[search_from..].find(&kw_lower) {
        let abs_pos = search_from + kw_pos;
        // Check that the keyword starts at a word boundary
        if abs_pos > 0 {
            let prev_char = line.chars().nth(abs_pos - 1);
            if let Some(c) = prev_char {
                if is_ident_char(c) {
                    search_from = abs_pos + 1;
                    continue;
                }
            }
        }
        // Find the name after the keyword
        let after_kw = abs_pos + keyword.len();
        let chars: Vec<char> = line.chars().collect();
        let mut idx = after_kw;
        // Skip whitespace
        while idx < chars.len() && chars[idx].is_whitespace() {
            idx += 1;
        }
        // Check if the next word matches `name`
        let name_chars: Vec<char> = name.chars().collect();
        if idx + name_chars.len() <= chars.len() {
            let candidate: String = chars[idx..idx + name_chars.len()].iter().collect();
            if candidate == name {
                // Ensure it's a word boundary after
                let after = idx + name_chars.len();
                if after >= chars.len() || !is_ident_char(chars[after]) {
                    return Some(idx);
                }
            }
        }
        search_from = abs_pos + 1;
    }
    None
}

/// Build a Location JSON object for goto-definition.
fn location_json(uri: &str, line: usize, col: usize, word: &str) -> JsonValue {
    let word_len = word.chars().count();
    JsonValue::Object(vec![
        ("uri".to_string(), JsonValue::Str(uri.to_string())),
        ("range".to_string(), JsonValue::Object(vec![
            ("start".to_string(), JsonValue::Object(vec![
                ("line".to_string(), JsonValue::Int(line as i64)),
                ("character".to_string(), JsonValue::Int(col as i64)),
            ])),
            ("end".to_string(), JsonValue::Object(vec![
                ("line".to_string(), JsonValue::Int(line as i64)),
                ("character".to_string(), JsonValue::Int((col + word_len) as i64)),
            ])),
        ])),
    ])
}
