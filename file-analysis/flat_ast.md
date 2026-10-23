# flat_ast.fd — flat/tree AST printer (1,287 LOC)

> **Re-verified 2026-10-23:** still dead — `dump-ast-flat` still aliases to `cmd_ast` (main.fd:146). The one-line wiring gap stands.

## Role

Complete `Program` → indented-tree formatter: `type_flat`, `expr_flat`,
`stmt_flat`, `func_flat`, `program_flat`, plus per-node helpers. Port of
`src/flat_ast.rs` (780 LOC; Fides grew due to variant-count and the
`*_flat` decomposition needed for the self-hosted type system).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| — | — | **Nothing.** No importer; `program_flat`/`expr_flat`/`stmt_flat` referenced nowhere outside the file. |

Rust wires it: `compiler.rs:1273` `parse_to_ast_flat_raw` →
`program_flat`, and `main.rs:718` maps `fidesc dump-ast-flat` to it —
`main.rs:833` notes the format is consumed by
`tests/selfhost/test_parser_harness.fd`. **The port dropped the call
site**: `main.fd` aliases `dump-ast-flat` to `cmd_ast` (:137), which
prints only a decl-count summary with the telling comment *"no full AST
formatter yet"* — the formatter exists, unwired, in this file.
Eighth dead module.

## Findings

### F1 — `dump-ast-flat` silently degraded — HIGH (wiring gap)

`fidesc dump-ast-flat file.fd` (Rust) → full tree. `fides-c3
dump-ast-flat` → 5-line decl summary. Any golden test or harness
comparing flat-AST output against the self-hosted CLI gets a
summary instead of the tree — `test_parser_harness.fd`-style
verification is unportable until `cmd_ast` calls `program_flat`
(it's a one-line fix: the build path already returns `Program`).

### F2 — Coverage is complete — INFO (positive)

Verified: all **40 `Expr` variants** and all **29 `Stmt` variants**
have explicit arms in `expr_flat`/`stmt_flat` — including the
Fides-only additions (`Yield`, `NamedArg`, `Parallel`) and the
full comptime/region/sandbox/select/txn vocabulary. **Zero `_ =>`
fallthroughs** in the file — the same coverage audit that found
missing-arms bugs in borrowck/comptime_eval comes back clean here.
`program_flat` covers impls, traits, trait_impls, externs, imports,
attributes; ML/game decls degrade to counts (Layers/Models/… — fine).

### F3 — Structural notes — LOW

- Trailing-newline trimming repeats `str_ends_with(s,"\n") → slice`
  ~15 times — a `rstrip_nl(s)` helper would collapse it.
- `esc(s)` escapes string literals; check whether `\n` inside a
  source string could still break single-line golden compares
  (it escapes to `\\n` — verified by reading `esc`, correct).
- `param_flat`/`annotation_flat`/`binop_flat`/`type_flat` mirrors
  Rust's label choices — output is byte-compatible with the Rust
  printer for the common constructs (the point of the format).

## Verification state

- Dead status verified: zero importers; `cmd_ast` confirmed to print
  the summary instead.
- Coverage verified mechanically: per-variant presence checked for
  Expr (40/40) and Stmt (29/29), no fallthroughs.
- No port regressions inside the file — the only regression is the
  dropped wiring (`compiler.rs:1273` equivalent missing).

## Next

`fmt.fd`.
