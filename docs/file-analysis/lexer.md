# lexer.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> One Oct 23 bug is still open (`5e3`); string escapes are fixed.

- **Path:** `compiler/lexer.fd`
- **LOC:** 967 (Rust `src/lexer.rs`: 774)
- **Role:** source text to `Vec<Token>`. Byte scanner with UTF-8-aware columns. Errors are
  collected in `Lexer.errors` as `"Error:line:col: msg"` strings, not printed.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/compiler.fd` | :216 `cached_lex` | imported modules and stdlib (token cache) |
| `compiler/compiler.fd` | :651, :685, :787, :1176, :1206 | main file, `--no-std` build, imports, runtime file, main build |
| `compiler/comptime_eval.fd` | :240 | parse source made by `comptime gen` |
| `compiler/parser.fd` | :5556 | sub-lex of an f-string `{expr}` |
| `compiler/main.fd` | :378 `cmd_tokens`, :3043 line-table reparse in `cmd_build` | `fid tokens`, native line table |

Reached from every command that reads source. Pipeline stage: first step of the front end.

## What the module does now

- Numbers: decimal with `_`, `0x` / `0b` / `0o`, floats with fraction and optional exponent.
  `lex_parse_int` is `@wrapping` (`lexer.fd:957-967`): one above `i64::MAX` keeps its bits (u64 literals).
- Strings: `\n \t \r \\ \" \0`, plus `\xHH` and `\u{...}` (new since Oct 23, `lexer.fd:469-475`),
  encoded as UTF-8 by `lex_utf8_encode` (:887). Same escapes in f-strings (:401-407).
- `\0` (or `\x00`, `\u{0}`) ends the string: the text after it is dropped (`nul_at`, :468, :490).
  The comment says "the C string ends here". `"ab\0cd"` has length 2 (verified).
- Char literals `'c'` become `IntLit(code point)` with the same escapes plus `\'` (:216-254).
- Raw strings `r"..."` and `r#"..."#` (:498-546), f-strings with nested `{}` (:381-448).
- Keyword table split into 8 functions so TCC can compile the generated C (:695-861).
  Identifiers longer than 10 bytes skip it (:700).

## Findings

### Verified live

- **HIGH — STILL TRUE — integer exponent literals split in two.** `5e3` lexes as `IntLit(5)` then
  `Ident("e3")`. The exponent check is only inside the fraction branch (`lexer.fd:347-370`); the
  integer path (:372-376) never looks for `e`. `var y = 5e3;` fails with
  `undefined variable 'e3'`, which blames the user. `1.5e3` works.
- **MEDIUM — NEW — lex errors in imported modules are ignored.** `compiler.fd` checks
  `lex.errors` only for the main file (:687, :1215). `cached_lex` (:206-219) and the import path
  (:787-789) never look. An unknown character ends the token stream with `Eof` (:667-670), so the
  parser drops the rest of the module without a word. Probe: module with `$` between `fn a` and
  `fn b`; the user file gets `undefined function 'b'` and no lex error.
- **LOW — CHANGED — malformed floats.** `1.5e` now lexes as `FloatLit(1.5)` (was `0.0`):
  `str_parse_float` parses the longest prefix. Still no error for the dangling `e`.
- **LOW — STILL TRUE — bad digits are not errors.** `0b102` gives `IntLit(2)` then `IntLit(2)`;
  `0x1_000` gives `IntLit(1)` then `Ident("_000")` (hex/bin/oct loops do not accept `_`, :310-333).
- **FIXED — `\xHH` / `\u{...}` in strings.** `"\x41\u{e9}"` gives `"Aé"`.

### From code inspection

- **LOW — quadratic string building.** `read_string`, `read_fstring` and `read_raw_string` append
  one code point at a time with `str_concat` (:436, :481, :541); `str_filter_underscores` (:677)
  and `repeat_char` (:938) do the same. Each append copies the string so far. Fine for normal
  literals; slow for very long ones (embedded data).
- **LOW — unknown character becomes `Eof` mid-stream** (:667-670), as in Rust. With the import
  bug above this truncates modules silently. A distinct error token would let the parser go on.
- **LOW — unterminated `/* ...` consumes to end of file** with no error (:262-274). Same in Rust.
- **INFO — leftover doc comment.** `/// Remove underscore separators...` at :379 sits above
  `read_fstring`; the dead stub it described was removed (FIXED).
- **INFO — `lex_is_alpha` is ASCII only** (:871). A non-ASCII letter outside strings and comments
  is an unknown character.

## Fix order

1. Add the integer exponent branch: after the integer digits, if `e`/`E` follows (with optional
   sign and at least one digit), lex a `FloatLit`. About 12 lines, mirrors `src/lexer.rs:256-269`.
2. Check `lex.errors` in `cached_lex` and the import path of `compiler.fd` (fix lives in compiler.fd).
3. Report an error for `e` without exponent digits and for invalid digits after `0b`/`0o`/`0x`.
4. Build strings with `StringBuilder` or slices.

## Verification state

- `fid check compiler/lexer.fd`: no errors.
- Live probes (scratch files, `fid dump-tokens` and `fid run`): `5e3`, `1.5e`, `0x1_000`,
  `0b102`, `"\x41\u{e9}"`, `"ab\0cd"`, `$`, imported module with `$`.
- `fid lint compiler/lexer.fd` is not usable: lint does not resolve `compiler::` imports
  (it reports `undefined function 'token_new'`).
