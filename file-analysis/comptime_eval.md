# comptime_eval.fd — comptime evaluator + substitution (1,632 LOC)

> **Re-verified 2026-10-23:** all headline findings confirmed — env cloned per nested block (:1172), return-comptime never substituted, impl/trait methods skipped, errors dropped. Still accurate.

## Role

Two public entry points used by `compiler.fd`:

| Export | Caller | Purpose |
|--------|--------|---------|
| `process_gen` | `compiler.fd:804,1266` | eval `comptime gen {…}` blocks → parse emitted source → append funcs/structs |
| `substitute` | `compiler.fd:1358` | replace every `comptime expr` with its literal value, then aggressive lit-args eval (I-5A) + loop unroll (I-5D) + dead-if elimination |

Internally: `evaluate`/`evaluate_with_env`/`evaluate_block_with_env`
(tree-walk eval with `ComptimeEnv`), `eval_binop`/`eval_unaryop`,
`evaluate_reflect` (all 7 ReflectOps implemented), `call_comptime_func`,
`replace_ident_*`, `stmt_has_break_or_continue`. Port of `src/comptime.rs`
(1,358 LOC).

## Where it's called

- `compiler/compiler.fd:36` import; only the two sites above. Leaf consumer
  of lexer+parser for `parse_generated_code` (:239).

## Findings

### F1 — `return comptime expr` is never substituted — HIGH (port regression)

`substitute_stmt` :332-340 — unboxes the return expr, substitutes into the
copy, then *"Can't re-wrap into Box easily; skip for now"* — the substituted
expr is **dropped**. Same drop in `aggressive_comptime_stmt` :568-576.
`return comptime f()` — the most common comptime usage — reaches
typecheck/mono unevaluated. Rust substitutes in place (`opt`'s `&mut`,
src:505-508). Fix: `*opt = OptBoxExpr::Some(box_expr_new(inner))`.

### F2 — Comptime in impl/trait methods never substituted — HIGH (port regression)

All three phases iterate `prog.funcs` only (:138, :263, :272, :281).
Rust also iterates `program.impls[].funcs` and `trait_impls[].methods`
(src:80-106 `for method in …`). Any `comptime` inside a method body or a
`comptime gen` block inside a method is invisible to every pass.

### F3 — `substitute` drops all errors (Rust returns `Result`) — HIGH (port regression)

Rust: `fn substitute(&Program) -> Result<Program, Vec<String>>` — eval
failures propagate to the caller as diagnostics. Fides: `-> Program`,
`errs` accumulated then **discarded** (:257). Every comptime failure —
missing comptime fn, non-bool if, div-by-zero, bad gen parse — produces a
raw `Expr::Comptime` that sails into typecheck/mono with zero diagnostic.
`process_gen` also drops errs (gen block is removed regardless — on failure
the `ComptimeGen` stmt silently vanishes :230).

### F4 — Loop/if-body assignments die with the env clone — HIGH (port regression)

`evaluate_block_with_env` clones its input env (:1103); for-loop bodies
and if-arms call it again → `Stmt::Assign`/`Let` inside a `for` or `if`
body writes to a **private clone** → `comptime { s=0; for i in 0..5 { s+=i } s }`
evaluates `s` → **0**. Rust's for-body writes to the *same* `env`
(src:855-870 `env.insert(target,…)`). String accumulation in `comptime gen`
loops — the feature's main use case — is broken.
(Rust also loses `Assign` inside `if` bodies via `_ => {}` — the Fides
version *executes* them then discards the clone — same net result, more
confusing mechanism.)

### F5 — `evaluate`'s env-less fallthrough returns the input — MEDIUM

`evaluate` (:1092) `_ => { return ResultExpr::Ok(*expr); }` — any
non-evaluable variant (Ident, StructLit, closures, MethodCall…) is
returned **as-is**. `substitute_expr_comptime` then writes it back —
silent non-substitution, indistinguishable from success.

### F6 — Thin walker coverage — MEDIUM

- `substitute_expr` handles ~18 Expr variants; `Await`/`Try`/`Yield`/
  `PipeForward`/`TupleLit`/`Asm`/`IsCheck`/`QuantCast`/`Closure`/
  `ComptimeAssign`-adjacent cases hit `_ => {}` → nested `comptime` in
  those positions unsubstituted.
- `aggressive_comptime_expr` recurses into only 4 shapes
  (Binary/Unary/Call/MethodCall) — lit-arg calls inside `If`, `Match`,
  `Index`, `ArrayLit`, `Block`, casts… never aggressively evaluated.
- `aggressive_comptime_stmt` misses `Match`/`Defer`/`Txn`/`Sandbox`/
  `Unsafe`/`Handle`/`Query`/`Parallel`/`Backward` that `substitute_stmt`
  does handle — inconsistent pass coverage.
- `Stmt::While` inside comptime eval → `_ => {}` skipped (inherited —
  Rust omits it too).

### F7 — Recursion/iteration hazards — MEDIUM (inherited)

- `call_comptime_func` has no depth cap — recursive `comptime fn`
  → unbounded recursion → compiler stack overflow (same in Rust).
- Phase-1 runs 8 iterations, Phase-3 4 iterations — fixed caps, no
  fixpoint check; deeply nested comptime can remain partially substituted
  silently.
- Fides Phase-1 substitutes **all** functions; Rust restricts it to
  `is_comptime` fns — extra work + mild drift.

### F8 — Smaller notes — LOW

- `eval_unaryop` `!` on IntLit → `IntLit(0/1)` — type-inconsistent vs
  BoolLit the rest of the pipeline expects.
- `BitNot` → `-1 ^ n` — correct two's-complement trick ✓.
- `FloatLit` lacks `Mod`/bit ops — errors, fine.
- Loop unroll (I-5D) caps `trip ≤ 16 && body ≤ 16` + break/continue scan —
  `stmt_has_break_or_continue` only checks top-level stmts, so a break
  nested in an inner `if` inside the loop body won't block unrolling →
  `break` semantics silently change (check nesting — likely inherited).
- No `Stmt::While`/`Match` in the *evaluator* (`_ => {}` skip).

## Verification state

- Every public fn traced; the four HIGHs confirmed by direct Rust-vs-Fides
  line comparison (Return arm, impl iteration, Result signature,
  env-sharing).
- `>>` parse blocker still blocks E2E; findings from inspection + Rust
  oracle.

## Next

`cparser.fd`.
