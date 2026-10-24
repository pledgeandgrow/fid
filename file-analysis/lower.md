# lower.fd — AST → SSA IR Lowering

> **Re-verified 2026-10-23:** all findings confirmed — `await` → `p_future_poll` still unimplemented (Int(0) under `run`), spawn → `p_thread_spawn` still fake-handled, borrow_kind tags still never emitted (compounds pir/opt dead metadata).

**6,930 LOC** vs `src/lower.rs` 7,629 LOC (~91%). The largest compiler module — lowers every
`Stmt`/`Expr` into the 42-variant `Inst` vocabulary in ir.fd, builds `IrModule`, registers
externs, resolves named/default args, mangles impl/trait methods, computes function effects.

## Where it's called (live — the mandatory middle of every path)

- `compiler/compiler.fd:1455,1486,1568,1581,1666` — `lower(program)` in `lower_to_ir_module`,
  `lower_to_ir_module_with_path`, `ccompile_source_with_path` (post-cache-miss), interp path,
  `build_executable`.
- `compiler/main.fd:1709,1740,1768` — `dump-ir`, `ir`, `pir` commands.
- Imported by `compiler/debugger.fd:46` (dead module).

## Structure — verified faithful port

- `lower_program` driver is 1:1: consts → globals → aliases → structs (incl. `@linear` tracking)
  → enums → externs → impl/trait method **pre-registration with mangled names** → trait
  method-name table → per-function lowering. The pre-registration correctly handles the
  impl-method visibility gap found in comptime_eval.
- `Expr` coverage: all 38 Rust arms present + Fides-added `NamedArg`/`Some`/`None`/`Yield`.
- `Stmt` coverage: all 30 Fides-AST variants. `Pattern` coverage 7/7.
- `defer_stack`/`emit_defers`/`emit_persist_saves` at the same 3 sites (Return, fn end, try).
- `__env` dummy insertion (`:3963`), `CallPtr` for locals w/ `has_env:true` (`:3949`),
  `p_future_ready` wrap on async calls (`:3972`), sret alloca for struct returns (`:3979`),
  `Inst::Print` desugar, `thread_id`/`workgroup_*` → `ConstInt(0)` — all match Rust.
- `runtime_effects` (:352) — the Write/Alloc/IO table AGENTS.md flags as DCE-load-bearing —
  is substantially ported (vec/map/string/atomic/chan/mutex/io families).

## Findings

### MEDIUM — `borrow_kind` provenance tags dropped (I-7A port gap)

Rust's `Expr::Ref` lowering tags the alloca `ValueMeta.borrow_kind` with `Mut`/`Shared` at
**three** sites (src/lower.rs:5671,5702,5714). The Fides `Expr::Ref` arm (:4824) ports all
the pointer-shape logic (SRET double-indirection avoidance etc.) but **drops every
`borrow_kind` write** — `compiler/lower.fd` contains zero `borrow_kind` references.

Downstream: `pir.fd:1183` harvests `meta.borrow_kind` into `f.borrow_kinds`, which opt's
`borrow_non_aliasing` (src/opt.rs:5031) consults for dead-store/load elimination. With all
metadata `None`, that alias-analysis family is **silently dead** — missed optimizations, not
miscompiles (conservative direction).

### MEDIUM — async lowering emits calls the interpreter can't run (cross-file)

- `Expr::Spawn` (:5278) and `Stmt::Parallel` (:3219) correctly emit
  `p_thread_spawn`/`p_thread_join` — but interp.fd's `p_thread_spawn` (:3414) returns a **fake
  negative handle and never runs the function**. Rust's version (`src/interp.rs:5093`) runs
  the callee eagerly and pushes `FutureState::Ready` — so `spawn f()` under `fidesc run` is a
  semantic no-op in the port. (interp-side defect; noted again here because lower produces the
  call.)
- `Expr::Await` (:5271) emits `p_future_poll` — **not implemented in interp.fd** (only claimed
  by `is_runtime` at :1868) → falls to the `Int(0)` default. `p_future_ready` (wrapped around
  every async call, :3976) is likewise unimplemented → async calls return `0` under `run`.
- Rust's L3B `scope_executor_stack` path (`p_executor_spawn` when inside `spawn scope`) is
  absent — consistent with `Stmt::SpawnScope` not existing in the Fides AST (see below).

### LOW (latent) — block-param merge values are unreachable & unbindable

- `add_block_param` is called at three merge points (:4108 if-expr, :4282, :5051 Coalesce)
  where only *some* predecessors deliver a value. `Expr::Coalesce`'s then-block at :5039
  **never binds the param** — `Jump`/`Branch` carry no args and the interp's block loop never
  reads `block.params` (verified: `values` Vec is pre-filled `Void` at interp.fd:744-748 and
  only `set` on `ExecOutcome::Value`). Inherited design gap — Rust's interp ignores params too.
- Reachability: the typechecker rejects `if`/`match` as expressions (verified: "cannot assign
  void to variable"), and `Option ?? default` is desugared to `Match` in mono.fd:981 →
  **`Expr::Coalesce` is unreachable end-to-end** (typechecker restricts `??` to `Option<T>`,
  verified live). If any path ever reaches these arms, the merged value is `Void`→0 under
  `fidesc run`.

### LOW — `Expr::Index` treats any `I64` receiver as a Vec

`Expr::Index` (:4696): `ir_type_eq(arr_ty, IrType::I64)` → `p_vec_get`. Rust only routes
`Opaque("Vec")`. The `I64` clause is a Fides-added catch for type-decayed Vecs, but a genuine
`i64` receiver indexing (e.g. an FFI handle) silently becomes a vec access instead of a GEP.
Normally unreachable (typechecker rejects `cannot index type str` — verified), so latent.

### Noted / inherited

- `String[i..j]` slicing (Rust → `p_string_slice`, src/lower.rs:5267) is absent; typecheck
  blocks `str` indexing outright — latent.
- `Expr::ComptimeReflect` → `ConstInt(0)` — inherited verbatim (Rust :6320, "should not reach
  here after comptime substitution" — but see comptime_eval.md: substitution drops mean it
  *can* reach here).
- `Expr::Yield` — Fides-only variant (absent from Rust's AST); lowers to eval-inner + no-op.
- `Vec` indexing calls runtime `p_vec_get` directly (:4737) — Rust routes through stdlib
  `Vec_get` with `__env`. Deliberate port adaptation; skips the stdlib bounds-check layer.
- `Stmt::Handle`/`Stmt::Do` implement one-way effects only: handlers bind args + inline their
  body, with **no resume** — `Stmt::Resume`/`SpawnScope`/`CancelScope` don't exist in the
  Fides `Stmt` enum at all (feature dropped at AST level, not just lowering).

## Assessment

The single most structurally-faithful large port yet — the driver, SSA plumbing, sret, `__env`
convention, defer/persist emission, and effect folding all match. The real losses are
metadata-level: `borrow_kind` tags (alias analysis silently dead) and the interp-side
spawn/await gap (async is decorative under `fidesc run`). The block-param machinery is
half-built in both implementations — lower emits params, nothing binds them; only pir.fd's
phi resolution makes them meaningful on compiled paths.
