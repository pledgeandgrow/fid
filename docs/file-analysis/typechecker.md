# typechecker.fd

> **Re-audited 2026-11-09** against the current source and with live probes
> (`s18.exe run`, test files in a scratch folder). The verify layer the
> 2026-10-22 audit called "gutted" is now real; the open problems are holes at
> the edges (guards, generic types, methods), listed below.

- **Path:** `compiler/typechecker.fd`
- **LOC:** 12,526 (was 7,673 on 10-22; Rust `src/typechecker.rs`: 8,684). 132 `TypeChecker_*` functions.
- **Role:** Types, effects, capabilities, safety annotations, generics (bounds, inference), borrowed strings and `@scoped` structs, linear types, `@send`/`@sync`, sandbox checks. Runs on every compile path.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`, `compiler::escape` (`esc_idents_stmts`, for linear captures at :8374).

## Callers

| Caller | Site | What |
|--------|------|------|
| `compiler.fd` main pipeline | :1620-1716 | `TypeChecker::new` → `register_program` (core, imports, user) → `register_trait_impls` → `register_generated_decls` → `verify_trait_impls` → `collect_capabilities` → `check_bodies` → `run_lints` → `free_maps` |
| `compiler.fd` no-std path | :843-865 | same sequence without core; then `collect_capabilities` + `check_bodies` per imported module |
| `compiler.fd` `lint_source` | :1917-1940 | same sequence + `run_lints` (no borrow check) |
| `compiler.fd` `lint_unwraps` | :1953-1979 | `TypeChecker_collect_called_funcs` |
| `compiler.fd` core skip | :1662 | `tc_core_skip_set` → `tc.skip_bodies` (release D9: core bodies are checked once per core hash) |
| `main.fd` `qualify` | :2455 | `expand_safety_primitives` |
| `tests/selfhost/test_annotations.fd` | | self-host test |

Reached from every command that builds a program (`run`, `compile`,
`ccompile`, `build`/`emit`, `dump-ir`/`dump-c`/`split-c`/`pir`/`opt`, `check`,
`test`, `bench`, `profile`, `codegen`, `wasm`, `gpu`, `verify`, `audit`,
`qualify`, `lsp`) and from `lint`.

## What it does now

`check_bodies` (:1809) runs, in order:

1. `check_annotation_validity` (:1770) — an unknown or misplaced annotation is an error (`annotation_target_mask` :1630: fn / struct / param / local / global bits).
2. `build_caches`, `compute_func_effects`, `verify_declared_effects`, `verify_deterministic`.
3. The verify passes: `verify_conserve`, `verify_gas` (call graph, memoized totals, cycles, literal loop trips: :2897-3157), `verify_safety_annotations` (domain presets, `@no-alloc`/`@no-recursion`/`@no-unbounded`/`@no-panic`/`@no-indirect`/`@requires-audit`/`@firmware`/`@bounded-stack`, transitive; :3159-3922), `verify_constant_time` (secret propagation, early returns, indexes; :4821-5359), `verify_realtime`, `verify_differentiable`, `verify_quantized`, `verify_checkpoint`, `verify_dp` (epsilon composition across `@dp` callees: :5637-5748), `verify_shard`, `verify_adapter`, `verify_tainted` (function effects, `@tainted` params), `verify_scoped` (M11), `verify_contracts` (constant arguments evaluated against `given`: :5911-6148), `verify_allocator_params`. `@secret` moved out to `compiler::secrecy` (:1837).
4. Body checks for functions, impl methods (with the impl's type params and `Self`), trait impl methods; bodies in `skip_bodies` are left out.
5. `verify_method_calls` (:2186: rules for calls to user methods, found once receiver types are known) and `verify_no_races` (:2044: a thread entry may not write a global without a lock).

Inside bodies: generic calls are checked against bounds, inferred, or rejected
(`check_generic_call` :7242, proposal 0002); match exhaustiveness on enums and
bool is an error (`check_exhaustive` :2133); a by-value Vec/HashMap/struct
parameter cannot be changed (`check_param_change` :7392); `&str` may go only to
read-only callees (`tc_borrow_safe_callee` :7330); sandbox calls are checked
transitively, including higher-order callees and function values (:7996);
higher-order effects are instantiated per call site (`register_ho` :2314,
`passed_fn_effects` :2346).

## Findings

### Verified live

#### MEDIUM — a guarded arm counts as covering its case

`check_exhaustive` (:2138-2149) reads only `arm.pattern`; `arm.guard` is
ignored. `Color::Blue if k > 5 => ...` covers `Blue`, and `_ if c => ...`
makes any match exhaustive (:2140 returns at once). Probe: a match on
`Color::Blue` with arms `Red`, `Green`, `Blue if k > 5` (k = 1) compiles; no
arm runs and execution falls through silently.

#### MEDIUM — exhaustiveness is checked only for non-generic enums and bool

`check_exhaustive` matches `Type::Named` and `Type::Bool` (:2150-2180).
`Option<T>`, `Result<T, E>` and user generic enums are `Type::Generic`: never
checked. Probes: `match o { Option::Some(x) => ... }` on `Option::None`, and
`enum Res2<T> { Good(T), Bad }` matched on `Good` only, both compile and run.

#### MEDIUM — generic structs cannot be used

`struct Box2<T> { v: T }` with `Box2 { v: 9 }` fails: "field 'v' expected T,
got i8" (:9796, field types are not substituted with the struct's type
arguments) and "cannot access field 'v' on type Box2<i64>" (field access
handles only `Type::Named`, :9698 / :9735). No test or stdlib module declares
a generic struct. `docs/reference/features.md:180` marks them done.

#### MEDIUM — contracts: only direct calls with constant arguments, outside a few expression kinds

`verify_contracts` (:5911) finds calls with `cs_expr` (:6048), which records
`Expr::Call` only and descends into Call/MethodCall args, Binary, Unary,
FieldAccess, Index, If, Block (`_ => {}` :6069). Not seen: method calls
(preconditions of `Type_m` match only a plain `Type_m(..)` call), calls inside
`match` expressions, struct/enum/array/tuple literals, closures, casts.
Probes: `m.half(-4)` and `val k = match 1 { _ => half(-4) }` with
`given x >= 0` both compile and print -2. Nothing checks a precondition at
run time (non-constant `half(v)` with v = -4 prints -2), and `ensure`
(`Func.postconditions`) is read by no pass in typechecker, lower, interp or
codegen_c.

#### MEDIUM — `@tainted`, `@dp`, `@shard`, `@adapter`, `@allocator` skip impl methods

`verify_tainted` (:5876), `verify_dp` (:5577), `verify_shard` (:5754),
`verify_adapter` (:5784), `verify_allocator_params` (:6154) loop over
`program.funcs` only. Probe: `impl L { fn log(self: L, @tainted s: str) !IO
{ println(s); } }` compiles and prints. (Gas, safety, constant-time and
contracts do include methods.)

#### LOW — function-level `@tainted` is rejected

`annotation_target_mask` gives `tainted` mask 4 (param only, :1650), so
`@tainted fn src()` fails with "Misplaced annotation '@tainted' ... valid on
param, not fn". The function-level branch of `verify_tainted` (:5878-5883) is
dead. `docs/reference/safety-rules.md` SR-12 says "a tainted function or
parameter".

#### LOW — unknown trait in a bound is accepted

`check_generic_call` skips a bound whose trait is not declared (:7300
`!TypeChecker_is_trait_name(self, tr) ... continue`). Probe:
`fn pr<T: Shwo>(x: T) -> T` called with 5 compiles and prints 5. A typo in a
bound is a silent no-op, unlike a typo in an annotation.

#### LOW — `@gas` counts a recursive call once

`gas_total` (:2983) counts a callee already on the path at its own cost.
Probe: `@gas(100) fn rec(n)` that recurses n times compiles (it then hits the
interpreter's recursion limit at 1000). Also: `gas_cost_expr` has no arm for
closures, spawn, await (`_ => {}` :3154), and `gas_trips_while` (:3106)
takes `while i < N` as N trips whatever `i` starts at.

### Code inspection

#### LOW — `cap_verified` is write-only

Filled with every function and method name (:1841-1855) and pruned in
`check_capability` (:8036, :8054), but never read. It is not a whitelist (the
10-22 audit called it one): capability errors are pushed regardless. Dead
state.

#### LOW — stale comments in `check_bodies`

:1819 "simplified -- just check @deterministic" and :1821-1823 "stubs --
permissive, don't reject" describe the October code; the passes below them
reject.

#### LOW — no introspection, no "did you mean"

`get_funcs`/`get_methods`/`get_structs`/... and the `edit_distance` /
`suggest_closest` family are still absent; `shapes_compatible` /
`get_tensor_shape` too. The LSP cannot ask the typechecker for resolved
types (see lsp.md).

#### INFO — method calls are matched to user methods by name

`verify_method_calls` keys targets as `owner.method` from the receiver type
recorded during checking; `safety-rules.md` "Known limits" describes the
remaining name-based matching. Not re-checked further.

## Status of the 2026-10-22 findings

| Finding | Status |
|---------|--------|
| HIGH: verify layer "stubs — permissive", `cap_verified` blanket whitelist | **CHANGED.** The passes reject now. `cap_verified` is filled but never read (dead, not a whitelist). The "stubs" comment at :1821 is stale. |
| `verify_gas` argument-only | **FIXED** — call graph, memoized totals, cycle handling, literal loop trips (:2897-3157). Recursion counts once (LOW above). |
| `verify_secret` no taint propagation | **FIXED / MOVED** — `@secret` is checked by `compiler/secrecy.fd` (labels propagate through assignments, calls, branches). See secrecy.md. |
| `verify_contracts` existence-only | **CHANGED** — constant arguments are evaluated (`ct_eval_int`/`ct_eval_bool` :6074-6148); gaps above. |
| `verify_tainted` effects-only | **STILL TRUE** (effects only, no value tracking), now also reads `@tainted` params (:5886). Methods skipped (above). |
| `verify_dp` no epsilon composition | **FIXED** — `dp_compute_total_epsilon` (:5691). Requires `!Noise`. |
| Missing helper families (`scan_*`, `is_in_call_cycle`, `register_generated_decls`, `eval_*`) | **FIXED** — `scan_stmts_for_calls` :3934, `scan_for_division` :4504, `scan_for_while_loops` :4705, `scan_for_floats` :12154, `is_in_call_cycle_fd` :3450, `register_generated_decls` :1484, `ct_eval_*` :6074. Still absent: `edit_distance`/`suggest_closest`, `shapes_compatible`/`get_tensor_shape`. |
| MEDIUM: match exhaustiveness absent | **FIXED** — an error now (`check_exhaustive` :2133, called :8831 and :9933). Gaps: guards, generic enums (above). |
| LOW: introspection surface dropped | **STILL TRUE.** |
| Verified real: `check_expr` superset, 4-step protocol, lints, domain presets | **STILL TRUE**; the protocol is now 6 steps (`register_generated_decls`, `verify_trait_impls` added). |

## Suggested fix order

1. `check_exhaustive`: skip arms with a guard when collecting covered cases; resolve `Type::Generic(name, _)` to the enum `name`.
2. Generic structs: substitute the struct's type arguments in StructLit field checks and in field access on `Type::Generic`.
3. Unknown trait in a bound: an error, as for unknown annotations.
4. Loop impl methods (and trait impl methods) in `verify_tainted`/`dp`/`shard`/`adapter`/`allocator_params`; decide whether `@tainted` is valid on a function and align the mask or SR-12.
5. `cs_expr`: method calls (resolved as `verify_method_calls` does), `match`, literals, closures. Decide on a runtime check for `given` and on `ensure`.
6. `@gas` on a recursive function: an error, or document it under "Known limits".
7. Remove `cap_verified` and the stale comments.

## Verification

Static read of `check_bodies`, every `verify_*`, `check_exhaustive`,
`check_generic_call`, the bound/inference helpers and the struct field paths;
live probes with `s18.exe run` (guarded and generic non-exhaustive matches,
generic struct, unknown bound, method/match contracts, runtime precondition,
`@tainted` method and function, recursive `@gas`, `@no-alloc`,
`@deterministic` through a closure, declared effects). Each MEDIUM/LOW marked
"verified live" was reproduced.
