# token.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> The Oct 22/23 findings are resolved; the file was rewritten to half its size.

- **Path:** `compiler/token.fd`
- **LOC:** 279 (was 560 on Oct 23; Rust `src/token.rs`: 263)
- **Role:** the `TokenKind` enum (138 variants), the `Token` struct (kind, line, col) and four helpers:
  `token_new`, `token_kind_to_str`, `is_word_keyword`, `token_kind_tag` / `token_kind_eq`.

## Callers

| Caller | What it uses |
|---|---|
| `compiler/lexer.fd` | builds every `Token` (`token_new`) |
| `compiler/parser.fd` | `token_kind_eq` on every token check, `token_kind_to_str` in error messages |
| `compiler/compiler.fd`, `compiler/comptime_eval.fd`, `compiler/main.fd` | import (token vectors pass through) |
| `compiler/main.fd` `cmd_tokens` (:365) | `fid tokens` / `fid dump-tokens` print `token_kind_to_str` |
| `tests/selfhost/test_token.fd`, `test_lexer_harness.fd`, `test_parser_harness.fd`, `test_annotations.fd`, `test_sl_debug.fd` | tests |

Reached from every command that reads source: `run`, `ccompile`, `build`/`emit`, `check`, `lsp`,
`tokens`, `dump-*`, `audit`, `qualify`. Pipeline stage: front end, before parsing.

## What the module does now

- `token_kind_tag` is a raw load of the enum's discriminant word (`token.fd:266-271`), not a
  138-arm match. The comment says the match compiled to about 1,900 zeroed locals per call (SP10).
  Every variant now has a distinct tag by construction.
- `is_word_keyword` is a range check `tag <= 62` (`token.fd:251-255`). It relies on the 63 word
  keywords (`Fn` .. `On`, including `True`, `False`, `Yield`, `Resume`) being declared first.
- `TokenKind::Resume` exists (`token.fd:24`); `lexer.fd:817` produces it and `parser.fd:2634` parses it.
- `DotDotDot` (`...`) and `Yield` are Fides-only variants; both are wired.

## Findings

### Verified live

- **FIXED — `Spawn`/`Yield` tag collision.** Tags are discriminants now, so no two variants can
  share one. `fid dump-tokens` prints `yield` and `resume` as distinct keywords.
- **FIXED — `Resume` missing.** Added; `resume(cont, v)` lexes and parses (`parser.fd:3561`).

### From code inspection

- **LOW — `is_word_keyword` depends on declaration order.** Adding a keyword after `On`, or a
  non-keyword before it, silently changes which tokens count as identifiers. The rule is only in
  a comment (`token.fd:252-253`). A test that checks `is_word_keyword` for each keyword would catch
  a mistake.
- **LOW — `token_kind_tag` reads the enum through a raw pointer** (`unsafe`, `token.fd:267-270`).
  It assumes the tag is the first word of the enum value. That holds for every backend today
  (the layout notes in AGENTS.md item 25 say an enum is `(1 + payload)` words, tag first).
- **INFO — `token_kind_to_str` mixes formats on purpose** (lowercase for keywords, variant names
  for types and punctuation). It matches Rust's `Display`; `dump-tokens` output depends on it.

## Fix order

1. Add a self-test that every keyword in `lexer.fd` maps to a tag `<= 62` and every other kind to
   a tag `> 62`.

## Verification state

- `fid check compiler/token.fd`: no errors. `fid lint`: no warnings.
- Live: `fid dump-tokens` on a scratch file with `yield resume` shows two keywords.
- Variant order counted by hand: 63 word keywords at discriminants 0..62.
