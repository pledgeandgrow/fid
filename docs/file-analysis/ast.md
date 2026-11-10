# ast.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> The three headline Oct 22/23 findings (no `AsmDir::Clobber`, undeclared variants became tag 0,
> no `Stmt::Resume`) are fixed.

- **Path:** `compiler/ast.fd`
- **LOC:** 1,289 (was 1,153; Rust `src/ast.rs`: 936)
- **Role:** the AST vocabulary: `Type` (28 variants), `Expr` (41), `Stmt` (33), `Pattern` (7),
  operator enums, declaration structs, `Program`, constructors, unbox helpers, `type_to_str`,
  and a few shared helpers used across the compiler.

## Callers

37 compiler modules import it (every pipeline stage plus `lsp`, `gpu`, `wasm`, `egraph`,
`rules`, `iropt`, `nat_ir`, `cowcheck`, `checks`, `flat_ast`) and 12 selfhost tests.

| Helper | Uses outside ast.fd | Files |
|---|---|---|
| `box_expr_unbox` | 420 | 4 |
| `binop_eq` | 265 | 10 |
| `box_expr_deref` | 193 | 4 |
| `box_type_unbox` | 95 | 4 |
| `type_to_str` | 85 | 5 |
| `str_vec_contains` | 37 | 6 |
| `box_expr_new` | 32 | 2 |
| `unaryop_eq` | 12 | 5 |
| `stmt_line` | 9 | 5 |
| `vec_contains_str` | 7 | 1 |
| `escape_str` | 6 | 1 |
| `box_pattern_unbox`, `box_type_to_str`, `shape_dim_to_str` | 0 | 0 |

Pipeline stage: data definitions for parser through lowering.

## What the module does now

- `AsmDir` is `In, Out, Clobber` (`ast.fd:240-244`).
- `Stmt` gained `Resume(cont, value, line)`, `SpawnScope(body, line)`, `CancelScope(line)`
  (:353-355); `stmt_line` covers all 33 (:597-635).
- `Program.globals: Vec<GlobalVar>` holds module-level `var`s (:422-427, :552).
- `Func` carries `type_params`, `type_param_bounds`, `const_params`, `declared_effects`,
  `annotations`, pre/postconditions, `is_async` (:375-392). `TraitDef.supertraits` (:450-456).
- `ReflectOp` has 12 ops, including `FieldOffset`, `EnumDiscriminant`, `FunctionEffects`,
  `FunctionCapabilities` (:223-238).
- Shared helpers at the end of the file: `str_vec_contains`, `vec_contains_str`, `escape_str`,
  `vec_contains_i64`, `vec_find_i64`, `box_expr_deref` (:1179-1289).

## Findings

### Verified live

- **FIXED — `AsmDir::Clobber` missing.** Added (:243). `flat_ast.fd` was not updated and now fails
  to type-check because of it (see flat_ast.md).
- **FIXED — undeclared enum variants became tag 0.** `Two::C` on `enum Two { A, B }` is now
  `Type error: enum 'Two' has no variant 'C'` (the check lives in the typechecker). The error is
  printed twice for one use (typechecker detail, not ast.fd).
- **FIXED — `Stmt::Resume` missing.**
- **LOW — STILL TRUE — `use string` is unused.** `fid lint compiler/ast.fd`:
  `warning: unused import: 'string'`.

### From code inspection

- **LOW — STILL TRUE — silent fallbacks after exhaustive matches.** `box_type_unbox` returns
  `Type::Never` (:1003), `box_pattern_unbox` `Pattern::Wildcard` (:1016), `box_expr_unbox` and
  `box_expr_deref` `Expr::IntLit(0)` (:1067, :1288). Unreachable today. Since the typechecker now
  rejects non-exhaustive matches (it caught `flat_ast.fd`), a new variant would be reported there
  first; the tails could become `panic`.
- **LOW — duplicate helpers.** `box_expr_deref` is a copy of `box_expr_unbox`;
  `vec_contains_str` is a copy of `str_vec_contains` (and of `parser.fd`'s `str_vec_has`).
- **LOW — dead tails** `return false` after exhaustive matches in `binop_eq`/`unaryop_eq`
  (:178, :187). `box_pattern_unbox`, `box_type_to_str`, `shape_dim_to_str` are `pub` with no
  outside callers.
- **INFO — `type_to_str` boxes its argument** to reuse `box_type_to_str` (:768-772): one
  allocation per call.
- **INFO — `binop_eq` is a nested match per operator** (:157-179); with 265 call sites a tag
  compare (as `token_kind_tag` does) would be smaller.

## Fix order

1. Drop `use string`.
2. Remove `box_expr_deref` and `vec_contains_str` (rename call sites).
3. Make the unreachable fallbacks panic; make internal helpers non-`pub`.

## Verification state

- `fid check compiler/ast.fd`: no errors.
- Live probe: `Two::C` rejected at type check (scratch file).
- Variant counts taken from the enum bodies (:103-131, :252-294, :297-305, :322-356).
- Call counts by grep over `compiler/*.fd`.
