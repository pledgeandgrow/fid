# parser.fd — recursive-descent parser

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> The two Oct 23 findings (f-string stub, missing resume/spawn scope/cancel scope) are fixed.

- **Path:** `compiler/parser.fd`
- **LOC:** 5,708 (was 5,358; Rust `src/parser.rs`: 3,657)
- **Role:** `Vec<Token>` to `Program`. Each `parse_*` returns a `ParseResult*` enum (Ok/Err).
  Errors are collected per top-level item; `parse()` returns them joined with `"\n"` (:569-571).

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/compiler.fd` | :235 `cached_parse` | stdlib and imports (with stdlib type names) |
| `compiler/compiler.fd` | :653, :697, :789, :1178, :1226, :1475, :1507 | main file, `--no-std`, imports, runtime file, re-parses |
| `compiler/compiler.fd` | :1605 `turbofish_all()`, :3963 `take_qualified_calls()` | turbofish pass, `module::name` calls |
| `compiler/comptime_eval.fd` | :242 `Parser_new` | code from `comptime gen` |
| `compiler/secrecy.fd` | :933 `parser_saw_secret()` | skip the secrecy pass when no `@secret` was seen |
| `compiler/main.fd` | :3044 | function names for the native line table (`emit`) |

Reached from every command that compiles source. Pipeline stage: front end, after the lexer.

## What the module does now (features missing from the old doc)

- **f-strings parse their expressions** (`parse_fstring_parts`, :5518-5572): each `{expr}` is
  re-lexed and parsed into `FStringPart::Expr`. `f"v={x + 1}"` prints `v=5` for `x = 4` (verified).
- **`resume(cont, v)`** (:2634, :3561), **`spawn scope { }`** (contextual, :2637-2647, :3594),
  **`cancel scope;`** (contextual, :2649-2664, :3622).
- **Turbofish** `f::<T, U>(args)` (:4870-4896): the type list goes to the module global
  `g_turbofish_types` and the call is renamed `f::<#k>`; `mono.fd` instantiates it.
- **Module-qualified calls** `m::f()` for names imported with `use` (:4910-4913, :5361-5362),
  recorded in `g_qualified_calls`.
- `@secret` / `@sensitive` set `g_saw_secret` (:5422).
- Generic bounds on functions (`type_param_bounds`, :1654-1681), trait supertraits (:1440-1445),
  module-level `var` globals (`parse_global_var`, :1169), type aliases, `asm!` with
  `clobber(...)` (:5038-5086), if/match as expressions (`parse_if_expr`, `parse_match_expr`).
- `split_gt` (:439) splits `>>`, `>>=`, `>=` so nested generics close.
- Precedence chain (documented in docs/reference/reference.md § 4.1): `|>` < `??` < range <
  `||` < `|` < `^` < `&` < shifts < `&&` < `==` < `<` < `+` < `*` < unary < postfix.

## Findings

### Verified live

- **FIXED — f-string interpolation.** Sub-parse restored (:5554-5562).
- **FIXED — `resume`, `spawn scope`, `cancel scope`.** All three parse to their `Stmt` variants.
- **INFO — errors are joined into one string.** Two parse errors in one file come back as one
  `Err` text; `enrich_errors` then shows the source caret only for the first (seen in a probe:
  errors at 1:87 and 1:151, one caret).

### From code inspection

- **MEDIUM — `parse_expr_iterative` is dead and wrong.** Its only call is from itself (:5208), so
  the recursive parser never uses it. If it were used: `peek_bin_op` maps `|>` and `??` to
  `BinOp::Or` (:5103-5104), and it calls `parse_primary`, so unary, postfix, `as` and ranges are
  not handled. The old doc listed it as a working depth escape hatch; that is wrong. The
  `expr_depth` / `max_expr_depth` counters (:4694-4699, :4926-4932) only feed this dead path.
  Deep nesting relies on the recursive parser and the stack guard. Delete it or fix and wire it.
- **LOW — module globals are never reset.** `g_turbofish_types`, `g_import_names` (:209-216) and
  `g_saw_secret` (:5707) live for the whole process. In `fid lsp`, which parses on every
  request, they grow without bound and an `@secret` seen once stays set.
- **LOW — f-string sub-parser loses context.** It uses `Parser::new` (:5558), so the user's struct
  and enum names are not known inside `{...}` (struct literals there may parse as blocks), and
  error positions are relative to the snippet (line 1).
- **LOW — `Parser_new` and `Parser::new` are the same 50 lines twice** (:275-334, :336-385),
  including the hard-coded list of the compiler's own AST type names.
- **LOW — `where` is a keyword but has no grammar rule.** A `where` clause is a parse error.
  docs/reference/reference.md says this is intended (reserved).
- **INFO — `str_vec_has(self.struct_names, ...)` is a linear scan** (:4910) next to the
  `struct_name_set` hash map that exists for this purpose.

## Fix order

1. Delete `parse_expr_iterative`, `OpEntry`, `reduce_one_op`, `BinOpInfo`, `peek_bin_op` and the
   depth counters (or fix the operator table and the primary/unary call before wiring it).
2. Reset the parser globals at the start of each build (needed for the LSP).
3. Pass the caller's struct/enum names to the f-string sub-parser and offset its error lines.
4. Merge `Parser_new` and `Parser::new`.

## Verification state

- `fid check compiler/parser.fd`: no errors (14 s).
- Live probes with `fid run`: f-string interpolation, parse error reporting.
- `parse_expr_iterative` call sites found by grep over `compiler/*.fd` (one self-call only).
