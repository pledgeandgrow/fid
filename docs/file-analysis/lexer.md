# File Analysis: compiler/lexer.fd

> **Re-verified 2026-10-23:** confirmed — integer scientific notation still broken (`5e3` → IntLit+Ident; exponent handling at :353-364 lives inside the `peek()=='.'` branch). `\xHH`/`\u{...}` still absent from read_string.

**Run:** 28/52 | **LOC:** 923 (src/lexer.rs: 774) | **Status:** LIVE — tokenizer for the whole pipeline

## Where it's called

`compiler.fd:212,634` (`Lexer::new().tokenize()` + the token cache), `parser.fd`, `main.fd`, `comptime_eval.fd`. Every front-end path — `run`, `ccompile`, `lsp`, `fmt` — starts here.

## Verified live (fidesc run, importing compiler::lexer itself)

```
Lexer::new("var a: f64 = 5e3;").tokenize()
→ var  a  Colon  F64  Eq  5  e3  Semicolon  EOF
                          ^^  ^^^ IntLit(5) then Ident("e3") — BROKEN
```

## Findings

### HIGH (verified) — integer scientific notation dropped: `5e3` → `5` + `e3`

`read_number` :346-376 — the exponent branch lives **only inside the `if peek()=='.' && digit` fraction block** (:354-364). Rust has a second exponent check in the else branch — "Integer with optional scientific notation (e.g. 1e308)" (lexer.rs:256-269) → `FloatLit`. The port dropped it. `var x: f64 = 5e3` lexes `IntLit(5)` + `Ident("e3")` → downstream parse/type errors blaming the user. `1e308`, `2E-3`, `1e+9` all broken — only `1.5e3` (fraction first) works.

### MEDIUM — `\xHH` and `\u{...}` escapes dropped from `read_string`

Fides `read_string` (:452-465) handles only `\n \t \r \\ \" \0`. Rust's handles `\xHH` and `\u{...}` (Issue 29, lexer.rs:~370-385). So `"\x41"` → "unknown escape sequence \x" + literal `x`. Worse: `read_char_lit` **does** implement both escapes (:235-238) — `'\x41'` works but `"\x41"` errors — internally inconsistent port coverage.

### MEDIUM — malformed floats become `0.0` silently

`1.5e`, `1.5e+`, `1.` (followed by non-digit → int+dot, fine) — `1.5e` consumes `e` then finds no digits → `str_parse_float("1.5e")` fails → `FloatLit(0.0)`. Rust's `s.parse().unwrap()` **panics** — neither errors cleanly, but the port converts a crash into a silent wrong value.

### Inherited / noted

- Unknown char → `TokenKind::Eof` **mid-stream** (:652-655) — parser sees premature EOF after the error. Identical in Rust (:766-768) — arguably should be a distinct `Error` token; neither has one.
- Unterminated `/* …` block comment → silent consume-to-EOF in both; `0b102` → `IntLit(2)` then `IntLit(2)` — no invalid-digit error in either.
- `0x`/`0b`/`0o` accept only valid digits then stop — `0x1_000` → `IntLit(1)` + `Ident(_000)` in both.
- Dead `fn str_filter_underscores` stub **inside `impl Lexer`** (:380-382) — body is a single `var result = "";`, no return, no `self` — dead stub shadowing nothing; the real free fn at :662 is what `read_number` calls.
- `str_filter_underscores` char-by-char `str_concat` — O(n²) per literal, trivial sizes.
- Keyword table: full parity incl. `q8`/`q4`→I8, ML (layer/model/grad/backward/kernel), game (component/system/query/state/on), `given`/`ensure`/`cap`/`select`/`guard`/`is`. Split into 8 sub-fns (a/b/c/d/e/ml/game/type) — comment says it keeps generated C small enough for **TCC** — a codegen_c workaround baked into lexer structure. `slen>10` fast path safe (longest keyword = 9).
- UTF-8 handling is careful: continuation bytes counted as one column (`advance` :109-134), `read_utf8`/`read_codepoint` proper multi-byte decode, `peek_at` returns −1 (not 0) avoiding false EOF/NUL matches — all better than a naive byte port.

## Verdict

Solid lexer with one live break: **integer-exponent floats are unlexable** under the self-hosted compiler (`5e3` verified → `5`+`e3`). Fix = copy Rust's else-branch exponent check (~12 lines). Secondary: `\x`/`\u` in strings (port already has the helpers in `read_char_lit`/`read_hex_value` — reuse them). Remove the dead `impl Lexer` stub while there.
