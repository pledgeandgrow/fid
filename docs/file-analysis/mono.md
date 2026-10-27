# mono.fd — Monomorphization (Generic Instantiation)

> **Re-verified 2026-10-23:** consistent with current source — `impl<T>` parse failure still leaves `generic_methods` unpopulated (dead path). Not exhaustively re-traced but no contradicting evidence found.

**2,629 LOC** vs `src/mono.rs` 1,549 LOC (~170% — Vec-entry structs + `box_*` shims +
`subst_expr`/`subst_stmt` split for TCC size limits). Live in the main pipeline.

## Where it's called

- `compiler/compiler.fd:894` — `monomorphize(program)` in the no-std path.
- `compiler/compiler.fd:1409` — `monomorphize(program)` in `build_program_with_path_borrowck`
  (post-typecheck, pre-lower — runs on every real build/run).

## Verified faithful

- `monomorphize` driver 1:1: separate concrete/generic fns → register trait-assoc bindings →
  register generic impl/trait-impl methods → `process_func` per concrete fn → rebuild
  impls/trait_impls → append `ctx.instantiated`.
- `mangle`/`mangle_method`/`mangle_type`, `infer_subst` pairing, `seen` dedup,
  `all_bound` skip semantics, `__const_N`→`IntLit` const-generic substitution
  (`Expr::Ident` :809-826), trait assoc-type resolution (`resolve_assoc_types` :695,
  `subst_gat_type_args` :622).
- `fixup_generic_calls` (:1753) re-infers arg types **per call site** and rewrites each
  `Expr::Call` to its own mangled name — `identity(42)`→`identity__i32` and
  `identity(100)`→`identity__i64` coexist. **Verified live**: `test_generics.fd` prints
  `421002050` — all four instantiations correct.
- `subst_type`/`subst_expr`/`subst_stmt` walkers — 42 Expr arms; `Generic` substitution
  recurses args (`Vec<T>`→`Vec<i64>` works); `MethodCall`→`Call` rewrite pushes receiver as
  arg 0 (:885-891) — same shape as Rust.

## Findings

### Port improvement — `infer_from_pair` gains `RawPtr` + `Generic` arms

Fides's `infer_from_pair` (:1434) adds `Type::RawPtr` (:1468) and `Type::Generic` (:1505)
recursion — Rust has **neither** (`_ => {}` at src/mono.rs:677). Consequence: Rust **cannot
infer `T` from `param=Vec<T>, arg=Vec<i64>`** → `all_bound=false` → the generic call is
silently skipped and left unmonomorphized. The Fides port can monomorphize generic
`Vec<T>`/`HashMap<K,V>`/`Option<T>` parameters that the reference compiler drops. One of the
few places the port is strictly ahead.

### Dead path — generic *methods* can't be written (`impl<T>` doesn't parse)

`ctx.generic_methods` is populated only when `imp.type_params.len() > 0` (:1591/:1605).
But `impl<T> Box2<T> { ... }` is rejected by the parser — verified live: `fidesc run`
reports "expected { but got 'Lt'" (the Rust bootstrap parser rejects it too — so it's
dead on **both** sides). The entire generic-method instantiation path is unreachable.

### Latent defect inside the dead path — self-param shift

Even if `impl<T>` parsed, Fides's method instantiation pushes **all** params incl. `self`
into `param_types` (:1908-1911) while `arg_types` (:2312-2317) collects only call args —
a **shift-by-one**: `self`'s type pairs with the first real arg. Rust does `.skip(1)`
(src/mono.rs:978-981). Inherited dead-code defect.

### Inherited dead store — `local_renames` populated then ignored

:1888 fills `local_renames` but `subst_stmt` at :1943 is called with `rename_new()` — the
map is never consulted. Same in Rust (src/mono.rs:967 + `HashMap::new()` at :1009) —
deliberate vestige; `fixup_generic_calls` already rewrote call sites.

### Noted

- `Expr::Coalesce`→`Expr::Match` desugar at :981 — where `Option ?? default` becomes the
  tag-check + alloca IR seen in lower.fd's dump.
- Method unbound-params → `Type::I32` placeholder (:1918) — same as Rust.
- `process_func` clears `is_comptime` on processed fns (:1951) — faithful.

## Assessment

Core generic-function monomorphization is verified working end-to-end (`test_generics.fd`
passes on all four instantiations) and the inference is *better* than the reference. The
losses are confined to dead space: generic methods can't parse anywhere, so the self-shift
bug inside that path is harmless. The standout is that the port *fixed* a real Rust gap —
nested-generic inference — which makes stdlib `Vec<T>`-style generics actually
monomorphizable under `fides-c`.
