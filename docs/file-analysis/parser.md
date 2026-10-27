# parser.fd — recursive-descent parser

> **Re-verified 2026-10-23:** confirmed — f-string sub-parse still commented out (:5172-5175, Literal(expr_src) instead), resume/spawn-scope/cancel-scope still unparseable (Resume still not a token — see token.md).

- **Path:** `compiler/parser.fd` — **5,358 LOC** (Rust `src/parser.rs` = 3,657, ~147%)
- **Role:** token stream → `Program` AST. Produces `ParseResult*` tagged enums (Fides has no `Result<T,E>` — the +1,700 LOC delta is almost entirely this boilerplate, not features).

## Where it is called

| Caller | Site | Mechanism |
|---|---|---|
| `compiler.fd` | :231-232 | `Parser_with_names(toks, struct_names, enum_names).parse()` — main pipeline (pre-seeded with stdlib type names for struct-literal disambiguation) |
| `compiler.fd` | :636 | `Parser_new` on a sub-path |
| `compiler.fd` | :1275-1276 | re-parse after incremental changes |
| `comptime_eval.fd` | :242-243 | `Parser_new(...).parse()` for comptime source |

Importers: `compiler.fd`, `comptime_eval.fd`. Both entry points (`Parser_new`, `Parser_with_names`) are live.

## Verified faithful (code inspection)

- **Precedence parity confirmed**: recursive chain `or → bit_or → bit_xor → bit_and → shift → and → equality → comparison → addition → mult → unary` is *identical* in both trees, and `peek_bin_op`'s numeric table (1–13) matches the chain order — `&&` is deliberately tighter than `|` in this grammar (unconventional vs C, but internally consistent).
- `recover_to_sync_token` sync set 1:1 (`;` consume, `}`/`fn`/`pub`/`struct`/`enum`/`impl`/`use` stop); error accumulation + `Err(errors.join("\n"))` — same shape.
- `split_gt` ported: `>>`→`Gt`, `>>=`→`GtEq`, `>=`→`Eq` — nested generics `Vec<Vec<T>>` close correctly.
- `alias_to_struct` tracked + consulted (:450/:1182 vs Rust :428/:843).
- `@annotation` on local `val`/`var` ported 1:1 (:2406+); all 15 exotic constructs exist both sides (layer/model/component/system/state-machine/event-handler/query/select/parallel/persist/handle/do/asm/fstring/guard).
- `parse_type` variant coverage identical; stmt dispatch 1:1 after accounting for `parse_assign_or_expr` split; `?` → `Expr::Try`, `select`, `try {}`, `await`, `spawn <expr>` all present.
- `parse_block` statement-level recovery: push error + `recover_to_sync_token` + continue — same as Rust.

## Findings

### HIGH — f-string interpolation is dead end-to-end *(verified by inspection)*

`parse_fstring_parts` (:5129-5184) extracts `{expr}` correctly (brace-depth tracking at :5146-5165) — then the sub-lex/sub-parse is **commented out** behind a TODO ("Re-enable when cross-module Lexer usage is fixed") and the raw source is pushed as `FStringPart::Literal(expr_src)` (:5175).

Rust does the real thing at src/parser.rs:3642-3646 — `Lexer::new` + `Parser::new` + `parse_expr` → `FStringPart::Expr`.

**Effect:** `f"val={x + 1}"` parses to `[Lit("val="), Lit("x + 1")]` — braces stripped, expression never evaluated, renders `val=x + 1`. Silently wrong output. `FStringPart::Expr` is fully wired downstream (borrowck :684/:1684, codegen :1322) but the parser never produces it — dead variant in practice.

*Not verified live — no `release/fides-c3.exe` in this workspace; `fidesc.exe` uses the Rust parser which works.*

### MEDIUM — `resume`, `spawn scope {}`, `cancel scope;` unparseable *(code inspection)*

Rust's parser handles all three (`parse_resume_stmt` :1578, `Stmt::SpawnScope` :1858, `Stmt::CancelScope` :1867). Fides has **zero** references — `resume` isn't even a `TokenKind`, `spawn` only parses the expression form (`Expr::Spawn` at :4610), `cancel` falls through to ident. Source-level drop matching run-30's AST-level absence — effect handlers can bind but never resume; structured concurrency scopes don't exist.

### Positive — `parse_expr_iterative` depth escape hatch *(Fides-only)*

Explicit-stack shunting-yard (:4747) invoked when `expr_depth > 256` inside tuple-element parsing (:4828-4838) — a self-hosting adaptation the Rust side doesn't need. Precedence table verified identical to the recursive chain — deep expressions don't flip semantics.

## Fix order

1. **Restore f-string sub-parse** — highest user-facing impact; the infrastructure (`Lexer` in parser.fd scope, `FStringPart::Expr` consumers) already exists. Needs the "cross-module Lexer" issue resolved or worked around (parser.fd already imports lexer indirectly? check what's blocking).
2. Port `resume`/`spawn scope`/`cancel scope` only if effects/structured-concurrency are a real target — they need AST variants first (ast.fd), so this is a feature decision, not a parser fix.

## Verification state

- No live runs (self-hosted binary absent). All findings from side-by-side source inspection: dispatch coverage diffed token-by-token, precedence tables compared numerically, recovery sets compared literally, construct lists counted 15/15.
