# typechecker.fd

> **Re-verified 2026-10-23:** confirmed — verify layer still intentionally permissive ("stubs — permissive" :1332, cap_verified whitelist at :1350-1363, verify_gas arg-only, verify_secret no taint-map, verify_dp no epsilon composition); match-exhaustiveness warnings still absent.

- **Path:** `compiler/typechecker.fd`
- **LOC:** 7,673 (Rust `src/typechecker.rs`: 8,684)
- **Role:** Type checking + region/effect/capability analysis. Runs on every compile path.
- **Call sites:** three identical 4-step sequences — no-std (`compiler.fd:826-846`), main pipeline (`:1317-1341`), lint-only (`:1540-1545`): `register_program` (stdlib→imports→user) → `register_trait_impls` → `collect_capabilities` → `check_bodies`.

## Findings

### HIGH — the annotation-verification layer is intentionally gutted

`check_bodies` invokes all 18 `verify_*` passes (:1331-1349) — every name exists — but the code comments say it outright:

```
// Additional verification passes (stubs -- permissive, don't reject)
// These are not needed for self-hosting since the compiler's own code
// doesn't use these annotations.
```

And :1350-1355 then pushes **every** function/method name into `self.cap_verified` — a blanket capability whitelist. The verify *shell* is wired; the verify *substance* is missing. Specifics confirmed against Rust:

- **`verify_gas` (:1764) is arg-validation only.** It checks `@gas(N)` has a positive-integer argument — full stop. Rust (`src/typechecker.rs:1986-2080`) builds a **call graph over all functions/methods/trait-impls**, counts per-function cost (`count_gas_cost_stmts`), computes transitive totals with memoization + cycle detection (`compute_total_gas`), and rejects `total > limit`. None of `count_gas_cost_*`, `compute_total_gas`, `scan_*_for_calls` exist in Fides — `@gas(5)` wrapping a million operations compiles clean. The compile-time gas guarantee is gone.
- **`verify_secret` (:3190) has real sink-checking but no taint propagation.** `check_secret_in_stmts` → `check_secret_in_expr` → `is_output_sink` (18 sinks: print/log/socket/file) → `expr_contains_secret` (Ident/Binary/Call/FieldAccess/Index coverage). **But `secret_names` is the fixed param-name list** — `let x = secret_param; print(x)` compiles clean; Rust's `propagate_secret_stmts` threads a `taint_map` through assignments (derived values stay secret). Only the direct-pass case is caught.
- **`verify_contracts` (:3370) is existence-only** — `preconditions.len() > 0` with a comment admitting "full evaluation would require comptime evaluation". Rust evaluates them (`eval_contract`, `eval_contract_expr`, `eval_expr_with_bindings` — all absent).
- **`verify_tainted` (:3161) is effects-list-only** — checks `IO` in `get_func_effects`; no `propagate_taint_*` value tracking.
- **`verify_dp` (:2931)** parses `epsilon=` values and collects `@dp` callees but `compute_total_epsilon` doesn't exist — no budget composition check.
- Missing helper families confirmed absent: `eval_*` (4), `scan_expr_for_calls/floats/division/while/unbounded` + stmt twins (10+), `count_gas_cost_*` (3), `propagate_taint/secret_*` (4), `compute_total_*` (2), `edit_distance`/`suggest_closest`/`suggestion_suffix` family (5 — "did you mean" hints), `shapes_compatible`/`get_tensor_shape`, `is_in_call_cycle` (though :1896 shows an indirect-recursion error does exist via different code), `register_generated_decls`.

### MEDIUM — match-exhaustiveness warnings absent (Issue 49)

Zero hits for `exhaustive`/`non-exhaustive` in Fides. Rust warns twice (stmt + expr contexts: `src/typechecker.rs:4869+`, `6418+`) on missing enum variants and missing bool arms. A non-exhaustive `match` compiles silently — downstream it's whatever codegen does on the missing arm.

### LOW — introspection surface dropped (corroborates run-31)

`get_funcs`, `get_methods`, `get_structs`, `get_enums`, `get_consts`, `get_traits`, `get_type_aliases`, `get_linear_structs`, `get_item_line`, `get_item_module` — all absent. This is the direct cause of the LSP limitation found in `lsp.md`: handlers can't query resolved types/methods from a TypeChecker that no longer exposes them.

### Verified real (the core is not gutted)

- **`check_expr`/`check_expr_with_hint`/`check_stmt`/`check_func` all present**, and Expr-arm coverage in `check_expr` is a **superset** of Rust — adds `NamedArg`, `None`, `Some`, `Yield` arms (4 extra, 0 missing).
- `register_program`/`register_trait_impls`/`collect_capabilities` ported with same 4-step call protocol at all 3 sites.
- `lint_unused_functions`/`lint_unused_imports` exist and are wired (:873-880 → `lint` command).
- Domain-preset expansion (`@crashfree`/`@medical`/`@safety-critical`/`@autosar` → primitive lists) present (:1790-1801) + the indirect-recursion safety error (:1896).
- 137 `TypeChecker_*` fns vs 149 Rust methods — the gap is concentrated in the verify/eval/scan/get families above, not the core checker.

## Suggested fix order

1. Port `scan_stmts_for_calls` + `count_gas_cost_stmts` + `compute_total_gas` into `verify_gas` — restores the `@gas` compile-time budget (self-contained; AST walkers already exist in this file's style).
2. Turn `secret_names` into a mutable taint set — push `let x = <secret-expr>` bindings (Rust's `propagate_secret_stmt` Let arm) — fixes the `let x = s; print(x)` hole in one place (`check_secret_in_stmt`'s `Stmt::Let` arm already calls `check_secret_in_expr`; extend it to add the bound name).
3. Port the Issue-49 exhaustiveness warnings (enum-variant set-diff + bool-arm check) — small, self-contained, restores a user-visible diagnostic.
4. Longer tail: `eval_contract`/`compute_total_epsilon`/`propagate_taint`, `get_*` accessors (needed if the LSP is ever to use real resolution), `suggest_closest` family (error-message quality).

## Verification

Inspection-only — no self-hosted binary. The gutting is explicit in-source (:1332-1334 comment + the `cap_verified` blanket loop). Each "gutted" claim was verified by reading the Fides verifier body and confirming the corresponding helper family is absent from the file's function table; the call-graph/eval/propagation machinery was located in Rust and found unimplemented rather than renamed.
