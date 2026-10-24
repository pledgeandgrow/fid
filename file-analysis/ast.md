# ast.fd — file analysis

> **Re-verified 2026-10-23:** all headline findings re-checked against current source — still accurate (AsmDir Clobber missing, undeclared-variant tag-0, no Resume).

- **Path:** `compiler/ast.fd` — 1,153 LOC
- **Role:** The AST vocabulary — `Type`, `Expr` (41 variants), `Stmt` (30),
  `Pattern`, decl structs, `Program`, plus display/unbox/constructor helpers.
  Rewritten from `src/ast.rs` (936 LOC Rust).
- **Analyzed:** 2026-10-22 — run 3 of 52 (alphabetical order)

## Where it is called

**26 importers — the most-depended-on module in the compiler.** Every pipeline
stage imports it: parser, typechecker, borrowck, mono, comptime_eval, flat_ast,
lower, ir, interp, opt, pir, regalloc, all codegens, pe, peephole, x86, wasm,
gpu, lsp, compiler, main. Consumption is implicit everywhere (variant
construction + matching) rather than through a narrow API.

Helper usage: `box_expr_unbox` 515 sites, `binop_eq` 97, `box_type_unbox` 69,
`type_to_str` 44, `unaryop_eq` 12, `OptFunc` 12, `stmt_line` 3,
`box_pattern_unbox` 2, `program_new` 2. `box_type_to_str` and
`shape_dim_to_str` have **no external callers** (internal to `type_to_str`).

Imports: `core`, `vec`, `string`. Leaf module.

## Findings

### Verified live

1. **HIGH — `AsmDir` is missing `Clobber`; every clobber operand silently
   becomes `in("reg") 0`.**
   `ast.fd:233-236` declares only `In, Out` — Rust has `In, Out, Clobber`
   (`src/ast.rs:328`). But `parser.fd:4657` constructs `AsmDir::Clobber`,
   `lower.fd:5360`/`mono.fd`/`typechecker.fd:5989`/`flat_ast.fd:581` all match
   or compare it. Live probe: `var y: Two = Two::C` (undeclared variant)
   **resolves to tag 0** — verified in the interpreter (`y is A`) and in
   generated C (`v33 = 0LL` stored for `y`). So `clobber("rcx")` parses →
   stored as direction `In` with `expr=IntLit(0)` → lowered as a regular input
   loading **0 into rcx**, and the `CL:`-prefix clobber path in lower.fd:5360
   plus all of `pe.fd`'s clobber save/restore (is_clobber_reg, push/pop) is
   **unreachable dead code**. Fix is one line: add `Clobber` to the enum.
   *(Was masked because parser accidentally does the right thing for the
   no-expr case: `direction != AsmDir::Clobber` compares tag0==tag0 → false →
   skips expr parse.)*

2. **HIGH — undeclared enum variants silently resolve to tag 0, in every
   backend.** `Two::C` on `enum Two {A,B}` constructs tag 0 (interp **and**
   C codegen verified above); a `Two::C` match arm is accepted and dead.
   The typechecker never validates variant names — so this isn't just the
   `AsmDir` bug, it's a general hole: any typo'd variant (`Some::Thign`)
   compiles and aliases variant 0. Finding lives in ast.fd's report because
   this file is where the missing-variant drift manifested; the fix belongs
   to typechecker/lowering validation (watch for it in the typechecker.fd
   run).

### From code inspection

3. **MEDIUM — `Stmt::Resume` missing vs Rust.** `src/ast.rs:489` has
   `Resume{cont, value}` for effect-handler continuations (lowered in
   `src/lower.rs:2507`, continuation built at :3715+). The Fides AST has no
   Resume, no token, no lowering — `resume(cont, v)` in user code parses as a
   plain `Call` and dies at typecheck as "unknown function" instead of
   getting continuation semantics. Feature gap (L3A) vs the bootstrap.

4. **LOW — Fides-only AST divergence (deliberate but worth tracking):**
   `Expr::Yield`, `Expr::NamedArg`, `Stmt::Parallel` don't exist in
   `src/ast.rs`. Yield is produced by `parser.fd:4608`; NamedArg is consumed
   by lower/typechecker; Parallel is F13 (Session 23). If the two ASTs must
   stay serializable-compatible (flat_ast round-trips), `flat_ast.fd`'s
   tagging order drifts from `src/flat_ast.rs` accordingly — worth
   confirming tag indices match for shared variants.

5. **LOW — silent default fallbacks in unbox helpers.** `box_expr_unbox`
   falls through to `Expr::IntLit(0)` (:1044), `box_type_unbox` →
   `Type::Never` (:985), `box_pattern_unbox` → `Wildcard` (:998) if a variant
   is ever unhandled. Today all 41/28/7 arms are exhaustive so they're dead —
   but combined with finding #2 (the typechecker can't verify), a future
   variant added without updating these helpers produces **IntLit(0)/Never/
   Wildcard corruption with zero diagnostics**. Safer: panic on fallthrough,
   or eliminate helpers once non-copy unbox exists.

6. **LOW — `use string` unused.** `fidesc lint` →
   `warning: unused import: 'string'`. (`vec`, `core` are used.)

7. **LOW — dead tails after exhaustive matches.** `binop_eq`/`unaryop_eq`
   trailing `return false` (:178,187) unreachable — same codebase pattern,
   harmless but noisy. `shape_dim_to_str`/`box_type_to_str` pub yet
   internal-only.

8. **INFO — `type_to_str` boxes every Type to display it** (:752-755) —
   one Box alloc per call in hot diagnostic paths. Cheap enough today
   (documented in the comment); a `match t` duplicate would avoid it but
   doubles the 28-arm table.

9. **INFO — type surface notes:** `Expr::FloatLit(f64)` is the only non-i64
   scalar payload in the big enums — works, but it's the layout that hit the
   Session-21 `Vec<f64>` bit-pattern bug class; worth remembering if enum
   payloads ever grow. `Param.default_expr` uses `has_default`+`IntLit(0)`
   sentinel (Option can't hold Expr — documented); `Attribute` vs
   `Annotation` duplication mirrors Rust.

## Suggested fix order

1. Add `Clobber` to `AsmDir` (ast.fd:233) — restores the whole clobber chain.
2. Typechecker/lowering: reject undeclared enum variants at construction and
   in match arms (the real fix for the tag-0 hole; flag for typechecker.fd).
3. Decide: implement `Stmt::Resume` (continuation lowering exists in Rust to
   port) or document as intentionally unsupported.
4. Make unbox fallbacks panic (`p_panic`/`unreachable`) instead of returning
   sentinel values.
5. Drop `use string`.
6. Non-pub `box_type_to_str`/`shape_dim_to_str` if no external use is planned.

## Verification state

- `fidesc lint`: 1 warning (unused `string` import).
- Live probe `enum_probe.fd`: `Two::C` → tag 0 in interpreter output and in
  `dump-c` generated code (`*(int64_t*)v = 0LL`) — tag-collapse confirmed in
  both backends. Test file removed.
- `src/ast.rs` diffed enum-by-enum: Type 28/28, Expr 41 (Fides superset),
  Pattern 7/7, Stmt — missing `Resume`, extra `Parallel`; `AsmDir` missing
  `Clobber`.
- Consumption map: all decl structs used (`compiler.fd:2237+` desugars the
  ML/game decls then clears the vecs — not dead fields).
