//! Unified diagnostic system for the Fides compiler.
//!
//! Provides structured error/warning/note output with:
//! - Source context display (shows the offending line with a caret `^^^`)
//! - Color output (red errors, yellow warnings, cyan notes/help)
//! - Error codes (E001, E002, ...)
//! - Help/suggestion text
//! - File path in error location
//!
//! Design goals:
//! - Zero external dependencies (no `annotate-snippets` crate)
//! - Works with or without color (auto-detects TTY)
//! - Backwards compatible: `Diag` can be converted to/from `String`

use std::fmt;
use std::sync::atomic::{AtomicBool, Ordering};

/// Global flag: should output use color? Auto-detected from TTY, overridable.
static USE_COLOR: AtomicBool = AtomicBool::new(true);

/// Disable color output (e.g. when piping to a file or in tests).
pub fn set_color(enabled: bool) {
    USE_COLOR.store(enabled, Ordering::Relaxed);
}

pub fn color_enabled() -> bool {
    USE_COLOR.load(Ordering::Relaxed)
}

/// Severity of a diagnostic.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Severity {
    Error,
    Warning,
    Note,
    Help,
}

impl Severity {
    fn label(&self) -> &'static str {
        match self {
            Severity::Error => "error",
            Severity::Warning => "warning",
            Severity::Note => "note",
            Severity::Help => "help",
        }
    }

    fn color_code(&self) -> &'static str {
        match self {
            Severity::Error => "\x1b[31m",   // red
            Severity::Warning => "\x1b[33m",  // yellow
            Severity::Note => "\x1b[36m",     // cyan
            Severity::Help => "\x1b[36m",     // cyan
        }
    }
}

/// Error codes for common diagnostic categories.
/// Format: E### (zero-padded to 3 digits).
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ErrCode {
    // Lexer errors (E001-E019)
    UnterminatedString,
    UnterminatedFString,
    UnknownEscape,
    UnexpectedChar,

    // Parser errors (E020-E039)
    ExpectedToken,
    ExpectedIdent,
    ExpectedType,
    InvalidAssignmentTarget,

    // Type errors (E040-E079)
    UndefinedVariable,
    UndefinedFunction,
    UndefinedStruct,
    UndefinedField,
    TypeMismatch,
    IfConditionNotBool,
    ArgCountMismatch,
    MissingReturn,
    InitBeforeAssign,
    NotLinear,
    LinearConsumed,
    LinearInLoop,

    // Borrow checker (E080-E089)
    BorrowConflict,
    MutableBorrowConflict,

    // Safety/contract errors (E090-E109)
    CapabilityDenied,
    SandboxViolation,
    ConservationViolation,
    GasExceeded,
    NotCrashfree,
    ContractViolation,
    MedicalViolation,

    // Effect errors (E110-E119)
    EffectMismatch,

    // Misc (E120+)
    NoMain,
    Internal,
    Custom,
}

impl ErrCode {
    pub fn code(&self) -> &'static str {
        match self {
            // Lexer
            ErrCode::UnterminatedString => "E001",
            ErrCode::UnterminatedFString => "E002",
            ErrCode::UnknownEscape => "E003",
            ErrCode::UnexpectedChar => "E004",
            // Parser
            ErrCode::ExpectedToken => "E020",
            ErrCode::ExpectedIdent => "E021",
            ErrCode::ExpectedType => "E022",
            ErrCode::InvalidAssignmentTarget => "E023",
            // Type
            ErrCode::UndefinedVariable => "E040",
            ErrCode::UndefinedFunction => "E041",
            ErrCode::UndefinedStruct => "E042",
            ErrCode::UndefinedField => "E043",
            ErrCode::TypeMismatch => "E044",
            ErrCode::IfConditionNotBool => "E045",
            ErrCode::ArgCountMismatch => "E046",
            ErrCode::MissingReturn => "E047",
            ErrCode::InitBeforeAssign => "E048",
            ErrCode::NotLinear => "E049",
            ErrCode::LinearConsumed => "E050",
            ErrCode::LinearInLoop => "E051",
            // Borrow
            ErrCode::BorrowConflict => "E080",
            ErrCode::MutableBorrowConflict => "E081",
            // Safety
            ErrCode::CapabilityDenied => "E090",
            ErrCode::SandboxViolation => "E091",
            ErrCode::ConservationViolation => "E092",
            ErrCode::GasExceeded => "E093",
            ErrCode::NotCrashfree => "E094",
            ErrCode::ContractViolation => "E095",
            ErrCode::MedicalViolation => "E096",
            // Effect
            ErrCode::EffectMismatch => "E110",
            // Misc
            ErrCode::NoMain => "E120",
            ErrCode::Internal => "E999",
            ErrCode::Custom => "E000",
        }
    }
}

/// A single diagnostic message.
///
/// A `Diag` represents one error/warning/note with structured information.
/// It can be rendered with or without color, with or without source context.
#[derive(Debug, Clone)]
pub struct Diag {
    pub severity: Severity,
    pub code: ErrCode,
    /// Primary message (without "error:" prefix).
    pub message: String,
    /// Source file path (e.g. "tests/hello.fd" or "<stdin>").
    pub file: String,
    /// 1-based line number.
    pub line: usize,
    /// 1-based column number.
    pub col: usize,
    /// The source line text (for context display). Empty if unavailable.
    pub source_line: String,
    /// Length of the span (in characters). 0 if unknown. 1 = single char.
    pub span_len: usize,
    /// Optional help/suggestion text.
    pub help: Option<String>,
    /// Optional secondary notes (related context).
    pub notes: Vec<String>,
}

impl Diag {
    /// Create a new error diagnostic.
    pub fn error(code: ErrCode, msg: impl Into<String>, file: &str, line: usize, col: usize) -> Self {
        Diag {
            severity: Severity::Error,
            code,
            message: msg.into(),
            file: file.to_string(),
            line,
            col,
            source_line: String::new(),
            span_len: 1,
            help: None,
            notes: Vec::new(),
        }
    }

    /// Create a new warning diagnostic.
    pub fn warning(code: ErrCode, msg: impl Into<String>, file: &str, line: usize, col: usize) -> Self {
        Diag {
            severity: Severity::Warning,
            code,
            message: msg.into(),
            file: file.to_string(),
            line,
            col,
            source_line: String::new(),
            span_len: 1,
            help: None,
            notes: Vec::new(),
        }
    }

    /// Set the source line text for context display.
    pub fn with_source(mut self, line: impl Into<String>) -> Self {
        self.source_line = line.into();
        self
    }

    /// Set the span length (number of characters underlined).
    pub fn with_span(mut self, len: usize) -> Self {
        self.span_len = len.max(1);
        self
    }

    /// Add a help suggestion.
    pub fn with_help(mut self, help: impl Into<String>) -> Self {
        self.help = Some(help.into());
        self
    }

    /// Add a secondary note.
    pub fn with_note(mut self, note: impl Into<String>) -> Self {
        self.notes.push(note.into());
        self
    }

    /// Render the diagnostic as a formatted string.
    ///
    /// If `color` is enabled, uses ANSI escape codes.
    /// If `source_line` is non-empty, shows the source context with a caret.
    pub fn render(&self) -> String {
        let use_color = color_enabled();
        let bold = if use_color { "\x1b[1m" } else { "" };
        let dim = if use_color { "\x1b[2m" } else { "" };
        let reset = if use_color { "\x1b[0m" } else { "" };
        let sev_color = if use_color { self.severity.color_code() } else { "" };

        let mut out = String::new();

        // Header: error[E044]: type mismatch
        out.push_str(&format!(
            "{}{}{}{}[{}]: {}{}\n",
            bold, sev_color, self.severity.label(), reset,
            self.code.code(),
            self.message, reset,
        ));

        // Location: --> tests/hello.fd:2:15
        if self.line > 0 {
            out.push_str(&format!(
                "{}  -->{} {}:{}:{}\n",
                dim, reset, self.file, self.line, self.col,
            ));
        }

        // Source context with caret
        if !self.source_line.is_empty() {
            let line_str = format!("{}", self.line);
            let line_width = line_str.len();

            // Source line: "  2 | pv x: str = 42;"
            out.push_str(&format!(
                "{}{} |{} {}\n",
                dim, " ".repeat(line_width), reset, self.source_line,
            ));

            // Caret line:  "  |        ^^^"
            let pad = " ".repeat(line_width);
            let col_pad = " ".repeat(self.col.saturating_sub(1));
            let carets = "^".repeat(self.span_len.max(1));
            out.push_str(&format!(
                "{}{} |{} {}{}{}{}{}\n",
                dim, pad, reset,
                col_pad, sev_color, bold, carets, reset,
            ));
        }

        // Notes
        for note in &self.notes {
            let note_color = if use_color { "\x1b[36m" } else { "" };
            out.push_str(&format!(
                "{}  = note:{} {}\n",
                note_color, reset, note,
            ));
        }

        // Help
        if let Some(help) = &self.help {
            let help_color = if use_color { "\x1b[36m" } else { "" };
            out.push_str(&format!(
                "{}  = help:{} {}\n",
                help_color, reset, help,
            ));
        }

        out
    }
}

impl fmt::Display for Diag {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.render())
    }
}

/// A collection of diagnostics. This is what compiler stages return on error.
#[derive(Debug, Clone, Default)]
pub struct DiagList {
    diags: Vec<Diag>,
}

impl DiagList {
    pub fn new() -> Self {
        DiagList { diags: Vec::new() }
    }

    pub fn push(&mut self, diag: Diag) {
        self.diags.push(diag);
    }

    pub fn is_empty(&self) -> bool {
        self.diags.is_empty()
    }

    pub fn len(&self) -> usize {
        self.diags.len()
    }

    pub fn iter(&self) -> impl Iterator<Item = &Diag> {
        self.diags.iter()
    }

    pub fn has_errors(&self) -> bool {
        self.diags.iter().any(|d| d.severity == Severity::Error)
    }

    /// Render all diagnostics as a single string.
    pub fn render(&self) -> String {
        let mut out = String::new();
        for d in &self.diags {
            out.push_str(&d.render());
        }
        out
    }

    /// Convert to a Vec<String> for backwards compatibility with existing
    /// code that expects `Vec<String>` errors.
    pub fn to_string_vec(&self) -> Vec<String> {
        self.diags.iter().map(|d| d.render()).collect()
    }
}

impl From<Diag> for DiagList {
    fn from(d: Diag) -> Self {
        DiagList { diags: vec![d] }
    }
}

/// Helper: create an error Diag with source context from a source string.
///
/// Extracts the source line at the given line number and sets the span.
pub fn error_with_source(
    code: ErrCode,
    msg: impl Into<String>,
    file: &str,
    line: usize,
    col: usize,
    source: &str,
    span_len: usize,
) -> Diag {
    let source_line = source.lines().nth(line.saturating_sub(1)).unwrap_or("");
    Diag::error(code, msg, file, line, col)
        .with_source(source_line)
        .with_span(span_len)
}

/// Helper: suggest the closest match from a list of candidates.
/// Returns a help string like "did you mean 'foo'?" if a close match is found.
pub fn suggest_did_you_mean(input: &str, candidates: &[&str]) -> Option<String> {
    let mut best: Option<(usize, &str)> = None;
    for &cand in candidates {
        let dist = levenshtein(input, cand);
        // Use threshold of 2 — close enough to be a useful suggestion,
        // but not so loose that unrelated words match.
        if dist <= 2 && (best.is_none() || dist < best.unwrap().0) {
            best = Some((dist, cand));
        }
    }
    best.map(|(_, cand)| format!("did you mean '{}'?", cand))
}

/// Simple Levenshtein distance for "did you mean" suggestions.
pub fn levenshtein(a: &str, b: &str) -> usize {
    let a: Vec<char> = a.chars().collect();
    let b: Vec<char> = b.chars().collect();
    let mut prev: Vec<usize> = (0..=b.len()).collect();
    let mut curr: Vec<usize> = vec![0; b.len() + 1];

    for i in 1..=a.len() {
        curr[0] = i;
        for j in 1..=b.len() {
            let cost = if a[i - 1] == b[j - 1] { 0 } else { 1 };
            curr[j] = (prev[j] + 1)
                .min(curr[j - 1] + 1)
                .min(prev[j - 1] + cost);
        }
        std::mem::swap(&mut prev, &mut curr);
    }
    prev[b.len()]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_diag_render_no_color() {
        set_color(false);
        let d = Diag::error(ErrCode::TypeMismatch, "cannot assign i64 to str", "test.fd", 2, 15)
            .with_source("    pv x: str = 42;")
            .with_span(2)
            .with_help("change the type annotation or the expression");
        let rendered = d.render();
        assert!(rendered.contains("error[E044]"));
        assert!(rendered.contains("cannot assign i64 to str"));
        assert!(rendered.contains("--> test.fd:2:15"));
        assert!(rendered.contains("pv x: str = 42;"));
        assert!(rendered.contains("^^"));
        assert!(rendered.contains("help:"));
    }

    #[test]
    fn test_suggest_did_you_mean() {
        assert_eq!(
            suggest_did_you_mean("bar", &["foo", "baz", "qux"]),
            Some("did you mean 'baz'?".to_string())
        );
        assert_eq!(suggest_did_you_mean("xyz", &["foo", "bar"]), None);
        // Exact match should not be suggested (distance 0 is fine but not useful)
        // — actually distance 0 means it's the same word, which is a valid suggestion
    }

    #[test]
    fn test_levenshtein() {
        assert_eq!(levenshtein("bar", "baz"), 1);
        assert_eq!(levenshtein("hello", "hello"), 0);
        assert_eq!(levenshtein("abc", "xyz"), 3);
    }
}
