//! Fides code formatter (`fidesc fmt`).
//!
//! A token-based pretty-printer (like early gofmt). It reads Fides source,
//! tokenizes it (preserving comments, which the main lexer drops), and
//! re-prints with consistent formatting:
//!
//!   - 4 spaces indentation
//!   - Normalized spacing around binary operators
//!   - Space after commas
//!   - No trailing whitespace
//!   - Single blank line between top-level definitions
//!   - Opening brace on same line; closing brace on its own line
//!   - Comments aligned with their code
//!
//! `--check` mode exits 1 if formatting would change the file, 0 otherwise.

use std::fmt::Write as _;

// ─── Token model ──────────────────────────────────────────────────────

/// A formatter token. Unlike the compiler's `Token`, this preserves comments
/// and string/number/identifier *text* so we can reprint faithfully.
#[derive(Debug, Clone, PartialEq)]
enum FTok {
    // Words — keywords and identifiers (we keep the literal text).
    Word(String),
    // Integer / float literal text.
    Number(String),
    // String literal, including surrounding quotes (and any escapes).
    Str(String),
    // f-string literal including the leading `f`.
    FStr(String),
    // Line comment including leading `//` (no trailing newline).
    LineComment(String),
    // Block comment including delimiters `/* ... */`.
    BlockComment(String),
    // Punctuation / operators — stored as canonical text.
    Punct(&'static str),
    // Blank-line separator (preserves blank lines between code).
    BlankLines(usize),
    // End of file sentinel.
    Eof,
}

impl FTok {
    fn is_word(&self) -> bool {
        matches!(self, FTok::Word(_))
    }
}

/// Multi-char operators/punctuation, longest first so the scanner is greedy.
const MULTI_PUNCT: &[&str] = &[
    "..=", "<<=", ">>=", "|>", "??", "?.", "~>", "::", "->", "=>", "==", "!=",
    "<=", ">=", "&&", "||", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "&=", "|=",
    "^=",
];

const SINGLE_PUNCT: &str = "+-*/%=<>!&|^~.,:;?@#()[]{}.";

/// Tokenize Fides source into formatter tokens (preserving comments & strings).
fn tokenize(src: &str) -> Vec<FTok> {
    let chars: Vec<char> = src.chars().collect();
    let mut i = 0;
    let n = chars.len();
    let mut out = Vec::new();
    // Track blank lines between significant tokens.
    let mut blank_count = 0usize;
    let mut saw_newline = false;

    while i < n {
        let c = chars[i];

        // Whitespace — track newlines for blank-line preservation.
        if c.is_whitespace() {
            if c == '\n' {
                if saw_newline {
                    blank_count += 1;
                }
                saw_newline = true;
            }
            i += 1;
            continue;
        }

        // If we accumulated blank lines before this token, emit a marker.
        if blank_count > 0 {
            out.push(FTok::BlankLines(blank_count));
            blank_count = 0;
        }
        saw_newline = false;

        // Comments
        if c == '/' && i + 1 < n && chars[i + 1] == '/' {
            let start = i;
            i += 2;
            while i < n && chars[i] != '\n' {
                i += 1;
            }
            let text: String = chars[start..i].iter().collect();
            // Trim trailing whitespace inside the comment.
            out.push(FTok::LineComment(text.trim_end().to_string()));
            continue;
        }
        if c == '/' && i + 1 < n && chars[i + 1] == '*' {
            let start = i;
            i += 2;
            while i + 1 < n && !(chars[i] == '*' && chars[i + 1] == '/') {
                i += 1;
            }
            if i + 1 < n {
                i += 2; // consume closing */
            } else {
                i = n;
            }
            let text: String = chars[start..i].iter().collect();
            out.push(FTok::BlockComment(text.trim_end().to_string()));
            continue;
        }

        // f-string: f"..." (and f'...' not supported by lexer, only f")
        if c == 'f' && i + 1 < n && chars[i + 1] == '"' {
            let start = i;
            i += 2; // consume f"
            while i < n && chars[i] != '"' {
                if chars[i] == '\\' && i + 1 < n {
                    i += 2;
                } else {
                    i += 1;
                }
            }
            if i < n {
                i += 1; // closing "
            }
            let text: String = chars[start..i].iter().collect();
            out.push(FTok::FStr(text));
            continue;
        }

        // String literal
        if c == '"' {
            let start = i;
            i += 1;
            while i < n && chars[i] != '"' {
                if chars[i] == '\\' && i + 1 < n {
                    i += 2;
                } else {
                    i += 1;
                }
            }
            if i < n {
                i += 1; // closing "
            }
            let text: String = chars[start..i].iter().collect();
            out.push(FTok::Str(text));
            continue;
        }

        // Number literal (int / float / hex / bin / oct with underscores)
        if c.is_ascii_digit()
            || (c == '0' && i + 1 < n && matches!(chars[i + 1], 'x' | 'X' | 'b' | 'B' | 'o' | 'O'))
        {
            let start = i;
            // Optional hex/bin/oct prefix
            if c == '0' && i + 1 < n && matches!(chars[i + 1], 'x' | 'X' | 'b' | 'B' | 'o' | 'O') {
                i += 2;
                while i < n && (chars[i].is_ascii_alphanumeric() || chars[i] == '_') {
                    i += 1;
                }
            } else {
                while i < n && (chars[i].is_ascii_digit() || chars[i] == '_') {
                    i += 1;
                }
                // fractional part
                if i < n && chars[i] == '.' && i + 1 < n && chars[i + 1].is_ascii_digit() {
                    i += 1;
                    while i < n && (chars[i].is_ascii_digit() || chars[i] == '_') {
                        i += 1;
                    }
                }
            }
            let text: String = chars[start..i].iter().collect();
            out.push(FTok::Number(text));
            continue;
        }

        // Identifier / keyword
        if c.is_alphabetic() || c == '_' {
            let start = i;
            while i < n && (chars[i].is_alphanumeric() || chars[i] == '_') {
                i += 1;
            }
            let text: String = chars[start..i].iter().collect();
            out.push(FTok::Word(text));
            continue;
        }

        // Multi-char punctuation (greedy, longest first)
        let mut matched = false;
        for &p in MULTI_PUNCT {
            let plen = p.chars().count();
            if i + plen <= n {
                let slice: String = chars[i..i + plen].iter().collect();
                if slice == p {
                    out.push(FTok::Punct(punct_canonical(p)));
                    i += plen;
                    matched = true;
                    break;
                }
            }
        }
        if matched {
            continue;
        }

        // Single-char punctuation
        if SINGLE_PUNCT.contains(c) {
            // Find the static form for this single char.
            if let Some(p) = single_punct(c) {
                out.push(FTok::Punct(p));
                i += 1;
                continue;
            }
        }

        // Unknown character — keep it as a single-char word so we don't lose it.
        out.push(FTok::Word(c.to_string()));
        i += 1;
    }

    out.push(FTok::Eof);
    out
}

/// Map a multi-char operator string to its canonical static form.
fn punct_canonical(s: &str) -> &'static str {
    match s {
        "..=" => "..=",
        "<<=" => "<<=",
        ">>=" => ">>=",
        "|>" => "|>",
        "??" => "??",
        "?." => "?.",
        "~>" => "~>",
        "::" => "::",
        "->" => "->",
        "=>" => "=>",
        "==" => "==",
        "!=" => "!=",
        "<=" => "<=",
        ">=" => ">=",
        "&&" => "&&",
        "||" => "||",
        "<<" => "<<",
        ">>" => ">>",
        "+=" => "+=",
        "-=" => "-=",
        "*=" => "*=",
        "/=" => "/=",
        "%=" => "%=",
        "&=" => "&=",
        "|=" => "|=",
        "^=" => "^=",
        _ => "",
    }
}

/// Map a single punctuation char to its canonical static form.
fn single_punct(c: char) -> Option<&'static str> {
    Some(match c {
        '+' => "+",
        '-' => "-",
        '*' => "*",
        '/' => "/",
        '%' => "%",
        '=' => "=",
        '<' => "<",
        '>' => ">",
        '!' => "!",
        '&' => "&",
        '|' => "|",
        '^' => "^",
        '~' => "~",
        '.' => ".",
        ',' => ",",
        ':' => ":",
        ';' => ";",
        '?' => "?",
        '@' => "@",
        '#' => "#",
        '(' => "(",
        ')' => ")",
        '[' => "[",
        ']' => "]",
        '{' => "{",
        '}' => "}",
        _ => return None,
    })
}

// ─── Token text ───────────────────────────────────────────────────────

/// Render a formatter token to its source text.
fn tok_text(t: &FTok) -> String {
    match t {
        FTok::Word(s) => s.clone(),
        FTok::Number(s) => s.clone(),
        FTok::Str(s) => s.clone(),
        FTok::FStr(s) => s.clone(),
        FTok::LineComment(s) => s.clone(),
        FTok::BlockComment(s) => s.clone(),
        FTok::Punct(s) => (*s).to_string(),
        FTok::BlankLines(_) => String::new(),
        FTok::Eof => String::new(),
    }
}

// ─── Operator classification ──────────────────────────────────────────

/// Binary operators that get spaces around them: `a + b`, `a == b`, etc.
fn is_binop(p: &str) -> bool {
    matches!(p,
        "+" | "-" | "*" | "/" | "%"
            | "==" | "!=" | "<=" | ">=" | "<" | ">"
            | "&&" | "||" | "<<" | ">>" | "&" | "|" | "^"
    )
}

/// Assignment operators that get spaces around them: `x = 5`, `x += 1`.
fn is_assignop(p: &str) -> bool {
    matches!(p,
        "=" | "+=" | "-=" | "*=" | "/=" | "%="
            | "&=" | "|=" | "^=" | "<<=" | ">>="
    )
}

/// Prefix unary operators (no space after them for their operand): `-x`, `!b`, `&x`.
fn is_prefix_unary(p: &str) -> bool {
    matches!(p, "-" | "!" | "&" | "*" | "~")
}

/// `->` and `=>` get spaces around them.
fn is_arrow(p: &str) -> bool {
    matches!(p, "->" | "=>" | "~>")
}

/// Keywords that open a brace block on the same line.
fn is_block_kw(w: &str) -> bool {
    matches!(w,
        "if" | "else" | "while" | "for" | "match" | "loop"
            | "struct" | "enum" | "impl" | "cap" | "component"
            | "system" | "state_machine" | "on" | "try"
            | "sandbox" | "unsafe" | "txn" | "region" | "handle"
            | "query" | "do" | "given" | "ensure"
    )
}

/// Keywords that begin a top-level definition (for blank-line spacing).
fn is_def_kw(w: &str) -> bool {
    matches!(w,
        "fn" | "val" | "var" | "const" | "struct" | "enum" | "impl"
            | "cap" | "type" | "use" | "module" | "extern"
            | "component" | "system" | "state_machine" | "on"
            | "layer" | "model"
    )
}

/// Keywords that are followed by a condition/expression then `{`.
fn is_cond_kw(w: &str) -> bool {
    matches!(w, "if" | "while" | "for" | "match" | "query" | "guard")
}

/// Is this word a type keyword (so `:` before it in params shouldn't get extra spacing)?
fn is_type_word(w: &str) -> bool {
    matches!(w,
        "i8" | "i16" | "i32" | "i64" | "i128"
            | "u8" | "u16" | "u32" | "u64" | "u128"
            | "f32" | "f64" | "bool" | "str" | "void"
            | "q8" | "q4"
    )
}

/// Determine if a `{` at position `idx` opens a definition body (struct/enum/
/// impl/cap/etc.) where `,` should produce newlines between fields.
/// We look at the significant token before the `{` and, if it's a word (the
/// type name), the token before that (the keyword like `struct`/`enum`).
fn is_def_body_brace_at(tokens: &[FTok], idx: usize) -> bool {
    // Get the token immediately before `{`.
    let prev1 = prev_significant_at(tokens, idx);
    match prev1 {
        Some(FTok::Word(w)) => {
            // If the word itself is a definition keyword, it's a def body.
            if matches!(w.as_str(), "struct" | "enum" | "extern") {
                return true;
            }
            // Otherwise, check the token before the word (e.g. `struct Point {`
            // → prev1 is "Point", prev2 is "struct").
            // Find prev1's index, then look before it.
            let prev1_idx = prev_significant_index(tokens, idx);
            if let Some(p1i) = prev1_idx {
                let prev2 = prev_significant_at(tokens, p1i);
                match prev2 {
                    Some(FTok::Word(w2)) => {
                        matches!(w2.as_str(),
                            "struct" | "enum" | "impl" | "cap" | "component"
                            | "state_machine" | "state" | "extern"
                        )
                    }
                    _ => false,
                }
            } else {
                false
            }
        }
        Some(FTok::Punct("}")) => {
            // `} {` — could be `impl Foo for Bar {` where `}` closes a previous
            // impl. Check further back. For simplicity, return false.
            false
        }
        _ => false,
    }
}

/// Find the index of the significant token immediately before `idx`.
fn prev_significant_index(tokens: &[FTok], idx: usize) -> Option<usize> {
    if idx == 0 {
        return None;
    }
    let mut i = idx;
    while i > 0 {
        i -= 1;
        if is_significant(&tokens[i]) {
            return Some(i);
        }
    }
    None
}

/// Check if `<` at position `idx` is a generic bracket (type parameters) vs
/// a comparison operator. Heuristic: if the previous significant token is a
/// word (function/type name) and the next significant token is a word or
/// `>` (type parameter list), treat as generic.
fn is_generic_open(tokens: &[FTok], idx: usize) -> bool {
    // idx points to the `<` token. Check the previous significant token.
    let prev = prev_significant_at(tokens, idx);
    let next = next_significant(tokens, idx + 1);
    match (prev, next) {
        (Some(FTok::Word(p)), Some(FTok::Word(_))) if !is_cond_or_match_kw(p) && !is_type_word(p) => true,
        (Some(FTok::Word(p)), Some(FTok::Punct(">"))) if !is_cond_or_match_kw(p) => true,
        _ => false,
    }
}

/// Get the significant token immediately before index `idx`.
fn prev_significant_at(tokens: &[FTok], idx: usize) -> Option<&FTok> {
    if idx == 0 {
        return None;
    }
    let mut i = idx;
    while i > 0 {
        i -= 1;
        if is_significant(&tokens[i]) {
            return Some(&tokens[i]);
        }
    }
    None
}

// ─── Pretty printer ───────────────────────────────────────────────────

struct Printer {
    out: String,
    indent: usize,
    /// True if the current line has content (we're mid-line).
    at_line_start: bool,
    /// Pending blank lines to emit before the next token (0 = none).
    pending_blanks: usize,
    /// The previous non-comment token's text (for spacing decisions).
    prev_significant: Option<String>,
    /// True if previous significant token was a keyword that starts a def.
    prev_was_def_kw: bool,
}

impl Printer {
    fn new() -> Self {
        Printer {
            out: String::new(),
            indent: 0,
            at_line_start: true,
            pending_blanks: 0,
            prev_significant: None,
            prev_was_def_kw: false,
        }
    }

    fn indent_str(&self) -> String {
        " ".repeat(self.indent * 4)
    }

    /// Begin a new line (apply indentation). Called when we need to write
    /// after a newline was emitted.
    fn start_line(&mut self) {
        if self.at_line_start {
            let _ = self.out.write_str(&self.indent_str());
            self.at_line_start = false;
        }
    }

    /// Emit a newline (and mark we're at line start).
    fn newline(&mut self) {
        // Trim trailing whitespace on the current line before newline.
        while self.out.ends_with(' ') || self.out.ends_with('\t') {
            self.out.pop();
        }
        self.out.push('\n');
        self.at_line_start = true;
    }

    /// Emit pending blank lines (collapsed to at most 1 between defs, but
    /// we allow up to 1 blank line).
    fn flush_blanks(&mut self) {
        let blanks = self.pending_blanks.min(1);
        self.pending_blanks = 0;
        for _ in 0..blanks {
            // Ensure we don't leave trailing whitespace on the blank line.
            self.newline();
        }
    }

    /// Write a token's text, ensuring proper leading spacing.
    fn write_tok(&mut self, text: &str) {
        self.flush_blanks();
        self.start_line();
        self.out.push_str(text);
        self.at_line_start = false;
    }
}

/// Determine if a token is a "significant" token (not a comment, not EOF).
fn is_significant(t: &FTok) -> bool {
    !matches!(t, FTok::LineComment(_) | FTok::BlockComment(_) | FTok::BlankLines(_) | FTok::Eof)
}

/// Format Fides source code. Returns the formatted string.
pub fn format_source(src: &str) -> String {
    let tokens = tokenize(src);
    let mut p = Printer::new();

    let mut idx = 0;
    // Track whether we just closed a top-level brace (for blank line before next def).
    let mut just_closed_top_level = false;
    // Track paren/bracket/brace depth to know if we're at top level.
    let mut brace_depth: i32 = 0;
    // Track paren/bracket depth (inside () or [] but not {}).
    let mut paren_depth: i32 = 0;
    // Stack tracking whether each `{` opens a "definition body" (struct/enum/
    // impl/cap/etc.) where `,` should newline, vs an expression block (struct
    // literal, if/while/for/match body) where `,` is just a separator.
    let mut brace_is_def_body: Vec<bool> = Vec::new();
    // Track generic bracket depth (`<` ... `>` for type parameters).
    let mut generic_depth: i32 = 0;

    while idx < tokens.len() {
        let tok = &tokens[idx];
        if let FTok::Eof = tok {
            break;
        }

        // ── Blank-line markers ───────────────────────────────────────
        if let FTok::BlankLines(count) = tok {
            // Preserve blank lines (capped at 1 for consistency).
            p.pending_blanks = p.pending_blanks.max(*count).min(1);
            idx += 1;
            continue;
        }

        // ── Comments ────────────────────────────────────────────────
        if let FTok::LineComment(text) = tok {
            // A comment resets the "just closed top-level" flag so we don't
            // insert a blank line between a comment and the following def.
            just_closed_top_level = false;
            // A line comment: if it's on its own line (we're at line start),
            // emit it at current indent. Otherwise it's a trailing comment.
            p.flush_blanks();
            if p.at_line_start {
                p.start_line();
                p.out.push_str(text);
                p.at_line_start = false;
            } else {
                // trailing comment — two spaces before it.
                p.out.push_str("  ");
                p.out.push_str(text);
            }
            p.newline();
            // Comments don't update prev_significant.
            idx += 1;
            continue;
        }
        if let FTok::BlockComment(text) = tok {
            p.flush_blanks();
            if p.at_line_start {
                p.start_line();
            }
            p.out.push_str(text);
            p.at_line_start = false;
            // Block comments can be inline; leave spacing to next-token logic.
            idx += 1;
            continue;
        }

        // ── Significant tokens ──────────────────────────────────────
        let text = tok_text(tok);
        let prev = p.prev_significant.clone();

        // For closing braces, de-indent before writing the token.
        if let FTok::Punct("}") = tok {
            if brace_depth > 0 {
                brace_depth -= 1;
            }
            p.indent = brace_depth.max(0) as usize;
            // Ensure `}` is on its own line (unless we're already at line start).
            if !p.at_line_start {
                p.newline();
            }
        }

        // Track generic brackets: `<` after a word (function/type name) followed
        // by a type parameter is a generic open, not a comparison.
        let cur_is_generic_open = matches!(tok, FTok::Punct("<")) && is_generic_open(&tokens, idx);
        let cur_is_generic_close = matches!(tok, FTok::Punct(">")) && generic_depth > 0;
        if cur_is_generic_open {
            generic_depth += 1;
        }

        // Decide whether we need a space before this token.
        let mut need_space_before = !p.at_line_start && need_space(&prev, tok, &tokens, idx, brace_depth);

        // Inside generic brackets, only space after `:` and `,`.
        if generic_depth > 0 {
            need_space_before = matches!(prev.as_deref(), Some(":") | Some(","));
        }
        // After a generic open `<`, no space.
        if cur_is_generic_open {
            need_space_before = false;
        }
        // Before a generic close `>`, no space.
        if cur_is_generic_close {
            need_space_before = false;
            if generic_depth > 0 {
                generic_depth -= 1;
            }
        }
        // After a generic close `>`, no space before `(` (function params).
        if prev.as_deref() == Some(">") && matches!(tok, FTok::Punct("(")) {
            need_space_before = false;
        }

        // Blank line handling: if this is a top-level def keyword and we
        // already have content, ensure a blank line before it — but only if
        // the previous definition ended with a closing brace (not after a
        // comment or other non-brace content).
        if brace_depth == 0 && is_def_word(tok) {
            if !p.out.is_empty() && p.pending_blanks == 0 {
                if just_closed_top_level && p.at_line_start {
                    p.pending_blanks = 1;
                }
            }
            just_closed_top_level = false;
        }

        p.flush_blanks();

        if need_space_before {
            p.out.push(' ');
        }

        if p.at_line_start {
            p.start_line();
        }

        // Write the token text.
        p.out.push_str(&text);
        p.at_line_start = false;

        // Update brace tracking and newline decisions.
        match tok {
            FTok::Punct("{") => {
                brace_depth += 1;
                // Determine if this `{` opens a definition body (struct/enum/
                // impl/cap/etc.) where `,` should newline, vs an expression
                // block (struct literal, if/while/for/match body).
                let is_def_body = is_def_body_brace_at(&tokens, idx);
                brace_is_def_body.push(is_def_body);
                // Opening brace: newline after (unless next token is closing brace).
                if next_significant_is(&tokens, idx + 1, "}") {
                    // empty block — keep on same line, next token handles close.
                } else {
                    p.indent = brace_depth as usize;
                    p.newline();
                }
            }
            FTok::Punct("}") => {
                brace_is_def_body.pop();
                // Closing brace goes on its own line at the reduced indent.
                // indent was already reduced before writing `}`.
                let nxt = next_significant(&tokens, idx + 1);
                match nxt {
                    Some(FTok::Punct(",")) => {
                        // struct field close in a struct literal? keep comma.
                    }
                    Some(FTok::Punct(";")) | Some(FTok::Punct(")")) | Some(FTok::Punct("]")) => {
                        // no newline
                    }
                    Some(FTok::Word(w)) if w == "else" || w == "while" || w == "catch" => {
                        // `} else {` or `} while` — stay on same line.
                        // The space will be added by need_space on the next token.
                        p.at_line_start = false;
                    }
                    Some(FTok::Punct("=>")) => {
                        // match arm `} => {` — but actually `}` here is closing
                        // a previous arm's block. Put on same line with ` => `.
                        p.out.push(' ');
                    }
                    _ => {
                        if brace_depth <= 0 {
                            just_closed_top_level = true;
                        }
                        p.newline();
                    }
                }
            }
            FTok::Punct(";") => {
                // Semicolon: newline after (statement boundary), unless inside
                // for-loop header parens.
                if brace_depth > 0 && !inside_for_header(&tokens, idx) {
                    p.newline();
                }
            }
            FTok::Punct(",") => {
                // Comma: newline after if inside a definition body (struct/enum
                // fields), but just space if inside parens/brackets or an
                // expression block (struct literal, function args).
                let in_def_body = brace_is_def_body.last().copied().unwrap_or(false);
                if brace_depth > 0 && paren_depth == 0 && in_def_body {
                    let nxt = next_significant(&tokens, idx + 1);
                    if !matches!(nxt, Some(FTok::Punct("}"))) {
                        p.newline();
                    }
                }
            }
            FTok::Punct("(") => {
                paren_depth += 1;
            }
            FTok::Punct(")") => {
                if paren_depth > 0 {
                    paren_depth -= 1;
                }
            }
            FTok::Punct("[") => {
                paren_depth += 1;
            }
            FTok::Punct("]") => {
                if paren_depth > 0 {
                    paren_depth -= 1;
                }
            }
            _ => {}
        }

        // Update prev_significant for spacing decisions.
        if is_significant(tok) {
            p.prev_significant = Some(text.clone());
            p.prev_was_def_kw = is_def_word(tok);
        }

        idx += 1;
    }

    // Final cleanup: ensure file ends with exactly one newline, no trailing ws.
    while p.out.ends_with(' ') || p.out.ends_with('\t') {
        p.out.pop();
    }
    // Collapse trailing newlines to exactly one.
    while p.out.ends_with("\n\n") {
        p.out.pop();
    }
    if !p.out.ends_with('\n') && !p.out.is_empty() {
        p.out.push('\n');
    }
    // Collapse any remaining 3+ consecutive newlines to max 2 (1 blank line).
    while p.out.contains("\n\n\n") {
        p.out = p.out.replace("\n\n\n", "\n\n");
    }

    p.out
}

/// Is this token a definition-starting keyword?
fn is_def_word(t: &FTok) -> bool {
    if let FTok::Word(w) = t {
        is_def_kw(w)
    } else {
        false
    }
}

/// Get the next significant (non-comment, non-EOF) token after `start`.
fn next_significant(tokens: &[FTok], start: usize) -> Option<&FTok> {
    let mut i = start;
    while i < tokens.len() {
        match &tokens[i] {
            FTok::LineComment(_) | FTok::BlockComment(_) => i += 1,
            FTok::Eof => return None,
            other => return Some(other),
        }
    }
    None
}

/// Check if the next significant token is a specific punctuation.
fn next_significant_is(tokens: &[FTok], start: usize, punct: &str) -> bool {
    matches!(next_significant(tokens, start), Some(FTok::Punct(p)) if *p == punct)
}

/// Heuristic: are we inside a `for (...)` header? (so `;` shouldn't newline)
fn inside_for_header(tokens: &[FTok], semi_idx: usize) -> bool {
    // Walk backwards to find an unmatched `(` that follows `for` or `while`.
    let mut depth = 0;
    let mut i = semi_idx;
    while i > 0 {
        i -= 1;
        match &tokens[i] {
            FTok::Punct(")") => depth += 1,
            FTok::Punct("]") => depth += 1,
            FTok::Punct("(") => {
                if depth == 0 {
                    // Found the enclosing `(`. Check if preceded by `for`/`while`.
                    // Walk back past whitespace/comments (already skipped in tokens).
                    let mut j = i;
                    while j > 0 {
                        j -= 1;
                        if let FTok::Word(w) = &tokens[j] {
                            if w == "for" || w == "while" {
                                return true;
                            }
                            // Some other word before `(` — not a for/while header.
                            // But could be a function call; only for/while counts.
                            break;
                        }
                        if is_significant(&tokens[j]) {
                            break;
                        }
                    }
                    return false;
                }
                depth -= 1;
            }
            FTok::Punct("[") => {
                if depth == 0 {
                    return false;
                }
                depth -= 1;
            }
            _ => {}
        }
    }
    false
}

/// Decide whether a space is needed between `prev` and `cur`.
fn need_space(
    prev: &Option<String>,
    cur: &FTok,
    tokens: &[FTok],
    idx: usize,
    _brace_depth: i32,
) -> bool {
    let prev = match prev {
        Some(p) => p.as_str(),
        None => return false, // beginning of file/line — no leading space.
    };

    let cur_text = tok_text(cur);

    // No space right after an opening bracket/paren.
    if prev == "(" || prev == "[" || prev == "{" {
        return false;
    }
    // No space right before a closing bracket/paren/brace or comma/semicolon.
    if cur_text == ")" || cur_text == "]" || cur_text == "}" || cur_text == "," || cur_text == ";" {
        return false;
    }
    // No space before `.` `::` `?.` `..` `..=` (member access / ranges).
    if cur_text == "." || cur_text == "::" || cur_text == "?." || cur_text == ".." || cur_text == "..=" {
        return false;
    }
    // No space after `.` `::` `?.` `@` `#` `..` `..=`.
    if prev == "." || prev == "::" || prev == "?." || prev == "@" || prev == "#" || prev == ".." || prev == "..=" {
        return false;
    }
    // No space between a word and `(` (function call) or `[` (index).
    if is_word_text(prev) && (cur_text == "(" || cur_text == "[") {
        // But `if (` / `while (` / `for (` / `match (` should have a space.
        if is_cond_or_match_kw(prev) {
            return true;
        }
        return false;
    }
    // No space between `)` and `(` (chained call).
    if prev == ")" && cur_text == "(" {
        return false;
    }

    // `:` — space after, but not before (for type annotations & struct fields).
    // Exception: `::` is handled above. For `:` in params/fields: `name: type`.
    if prev == ":" {
        // space after colon
        return true;
    }
    if cur_text == ":" {
        // No space before colon (e.g. `x: i32`, not `x : i32`).
        // But `::` is a different token already handled.
        return false;
    }

    // `->` and `=>` and `~>` — spaces around.
    if is_arrow(prev) || is_arrow(&cur_text) {
        return true;
    }

    // Binary operators — spaces around.
    if is_binop(&cur_text) || is_binop(prev) {
        // Special: `..` range in `for i in 0..5` — space around? We'll space it.
        return true;
    }
    // Assignment operators — spaces around.
    if is_assignop(&cur_text) || is_assignop(prev) {
        return true;
    }

    // `@` annotation prefix — no space after `@` (handled above), but space
    // before `@` if it starts a line (we handle via word check).
    if cur_text == "@" {
        return true;
    }

    // Comma — space after (handled by general "word after punct" rule below).
    if prev == "," {
        return true;
    }
    if cur_text == "," {
        return false;
    }

    // `;` inside for-header — space after (handled by newline logic normally,
    // but in for-header we don't newline, so ensure space).
    if prev == ";" {
        return true;
    }

    // `!` as prefix unary — no space after.
    if prev == "!" && is_word_text(&cur_text) {
        return false;
    }
    // `~` as prefix unary (bitwise NOT) — no space after.
    if prev == "~" && is_word_text(&cur_text) {
        return false;
    }
    // `&` as prefix (reference) — no space after when followed by word.
    if prev == "&" && (is_word_text(&cur_text) || cur_text == "&") {
        // `&&` is a binop (handled above). Single `&x` — no space.
        if prev == "&" {
            // Check: is this `&` a prefix (reference) or binary AND?
            // Heuristic: if the token before `&` is a word/closing-bracket,
            // it's binary AND (space already added by binop rule above).
            // If before `&` is `(` `,` `=` `return` etc., it's prefix.
            // We already returned true for binop above, so if we're here,
            // it's prefix.
            return false;
        }
    }
    // `*` as prefix (deref) — no space after when followed by word.
    if prev == "*" && is_word_text(&cur_text) {
        // Could be binary `*` (handled by binop above → space). If we're here,
        // the binop rule didn't fire, meaning `*` is prefix deref.
        return false;
    }
    // `-` as prefix unary (negative number) — no space after when followed by
    // a number, or when at start of expression.
    if prev == "-" && (is_number_text(&cur_text)) {
        // Check if `-` is prefix: look at token before `-`.
        if idx >= 1 {
            if let Some(before_minus) = prev_significant_before(tokens, idx) {
                let bt = tok_text(before_minus);
                if bt == "(" || bt == "," || bt == "=" || bt == "==" || bt == "!="
                    || bt == "<" || bt == ">" || bt == "<=" || bt == ">="
                    || bt == "+" || bt == "-" || bt == "*" || bt == "/"
                    || bt == "%" || bt == "&&" || bt == "||" || bt == "!"
                    || bt == "=>" || bt == "->" || bt == "[" || bt == "{"
                    || bt == ":" || bt == ";"
                {
                    return false; // prefix unary minus
                }
                if is_word_text(&bt) && bt == "return" {
                    return false;
                }
            } else {
                return false; // beginning → prefix
            }
        }
    }

    // Two words — space between.
    if is_word_text(prev) && is_word_text(&cur_text) {
        return true;
    }
    // Word followed by `{` — space (e.g. `if x {`, `struct Point {`).
    if is_word_text(prev) && cur_text == "{" {
        return true;
    }
    // Number/string followed by `{` — space (e.g. `1 {` in `if 1 {`).
    if (is_number_text(prev) || prev.starts_with('"') || prev.starts_with('f')) && cur_text == "{" {
        return true;
    }
    // `)` followed by `{` — space (e.g. `if (x) {`, `fn() {`).
    if prev == ")" && cur_text == "{" {
        return true;
    }
    // `]` followed by `{` — space (e.g. `[T; N] {`).
    if prev == "]" && cur_text == "{" {
        return true;
    }
    // Word followed by `#` (rare) — space.
    if is_word_text(prev) && cur_text == "#" {
        return true;
    }
    // `>` from generics followed by `{` — handled by `)`/word rules; default true.
    // Closing brace `}` followed by a word — space (e.g. struct literal in expr).
    if prev == "}" && is_word_text(&cur_text) {
        return true;
    }

    // Default: space between most things to be safe, but not for punct-adjacent.
    // If both are punctuation with no special rule, no space.
    if !is_word_text(prev) && !is_word_text(&cur_text) {
        // e.g. `(` followed by `@` — space? Usually no. Default no space.
        return false;
    }

    // Word after punctuation (other than the no-space cases above) — space.
    if !is_word_text(prev) && is_word_text(&cur_text) {
        return true;
    }
    // Punctuation after word (other than handled cases) — space.
    if is_word_text(prev) && !is_word_text(&cur_text) {
        // e.g. `x +` — but `+` is binop handled above. `x ;` → `;` handled above.
        // `x {` handled. `x (` handled (no space for calls). `x [` handled.
        // `x @` → space. `x #` → space.
        // Default: space (e.g. `x ?`).
        return true;
    }

    true
}

/// Get the significant token immediately before index `idx` (the token at idx
/// is the current token; we want the one before the prev_significant which
/// is already passed in). Actually we need the token before the *previous*
/// token. Since `prev` is already the previous significant token, we look
/// for the token before `idx-1`'s significant predecessor.
fn prev_significant_before(tokens: &[FTok], idx: usize) -> Option<&FTok> {
    // idx is the current token. The previous significant token is at some
    // position < idx. We want the one before that.
    let mut found_first = false;
    let mut i = idx;
    while i > 0 {
        i -= 1;
        if is_significant(&tokens[i]) {
            if found_first {
                return Some(&tokens[i]);
            }
            found_first = true;
        }
    }
    None
}

fn is_word_text(s: &str) -> bool {
    !s.is_empty() && (s.chars().next().unwrap().is_alphabetic() || s.chars().next().unwrap() == '_')
}

fn is_number_text(s: &str) -> bool {
    !s.is_empty() && s.chars().next().unwrap().is_ascii_digit()
}

fn is_cond_or_match_kw(s: &str) -> bool {
    matches!(s, "if" | "while" | "for" | "match" | "query" | "guard" | "return")
}

// ─── Public API ───────────────────────────────────────────────────────

/// Format source and return the formatted string.
pub fn format(src: &str) -> String {
    format_source(src)
}

/// Check if source is already formatted. Returns true if formatting would
/// NOT change it (i.e. it's already well-formatted).
pub fn is_formatted(src: &str) -> bool {
    format_source(src) == src
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_format() {
        let input = "pf main(){pv x=1+2;print(x);}";
        let out = format(input);
        assert!(out.contains("pf main() {"));
        assert!(out.contains("    pv x = 1 + 2;"));
        assert!(out.contains("    print(x);"));
        assert!(out.ends_with("}\n"));
    }

    #[test]
    fn test_idempotent() {
        let input = "pf fib(n: i64) -> i64 {\n    if n <= 1 { ret n; }\n    pv a = fib(n - 1);\n    ret a;\n}\n";
        let once = format(input);
        let twice = format(&once);
        assert_eq!(once, twice, "formatting is not idempotent");
    }

    #[test]
    fn test_check_mode() {
        let unformatted = "pf main(){pv x=1;}";
        assert!(!is_formatted(unformatted));
        let formatted = format(unformatted);
        assert!(is_formatted(&formatted));
    }

    #[test]
    fn test_preserves_comments() {
        let input = "// hello\npf main() {\n    pv x = 1; // inline\n}\n";
        let out = format(input);
        assert!(out.contains("// hello"));
        assert!(out.contains("// inline"));
    }

    #[test]
    fn test_struct_spacing() {
        let input = "struct Point{x:i32,y:i32,}";
        let out = format(input);
        assert!(out.contains("struct Point {"));
        assert!(out.contains("    x: i32,"));
        assert!(out.contains("    y: i32,"));
    }

    #[test]
    fn test_blank_line_between_defs() {
        let input = "fn a() {\n    return 1;\n}\nfn b() {\n    return 2;\n}\n";
        let out = format(input);
        assert!(out.contains("}\n\nfn b()"));
    }
}
