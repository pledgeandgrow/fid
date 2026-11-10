# mono.fd — Monomorphization (generic instantiation)

> **Re-audited 2026-11-09** against the current source, with live probes
> (`s18.exe run` / `dump-c`). Generic impls now parse and are instantiated;
> turbofish is new. New open problem: call sites inside several statement and
> expression kinds are never renamed (verified live).

- **Path:** `compiler/mono.fd`
- **LOC:** 3,054 (was 2,629 on 10-22; Rust `src/mono.rs`: 1,549)
- **Role:** Clones each generic function and generic impl method per concrete type substitution, renames call sites to the mangled instance (`ident__i64`), resolves const generics and trait associated types, and resolves turbofish calls (`f::<T>(..)`) before type checking.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`.

## Callers

| Caller | Site | What |
|--------|------|------|
| `compiler.fd` main pipeline | :1605-1606 | `mono_resolve_turbofish(program, turbofish_all())` + `mono_turbofish_errors()` — before type checking |
| `compiler.fd` main pipeline | :1785 | `monomorphize(program)` — after borrow check and comptime `substitute`, before lowering |
| `compiler.fd` no-std path | :931 | `monomorphize(program)` (no turbofish resolution on this path) |
| `tests/selfhost/test_mono.fd` | | self-host test |

Reached from every command that builds a program (the same set as
typechecker.md); not from `lint`.

## What it does now

- `monomorphize` (:1716): records non-generic return types (`g_mono_ret_index`, for receiver types of calls), separates concrete and generic functions (an existing `__` instance from a turbofish is marked seen), registers trait associated-type bindings, registers generic impl and generic trait impl methods (`ctx.generic_methods`), runs `process_func` on every concrete function and method, rebuilds impls, appends `ctx.instantiated`.
- Generic impls (`impl<T> Option` in core, `stdlib/core.fd:77`): their methods are instantiated on demand; each also gets an `i64` instance under the plain name, for a call whose receiver type is unknown here (:1807-1817).
- `process_func` (:2240): collects calls (`collect_generic_calls_stmt_with_hint` :2407 / `_expr_with_hint` :2640, which cover every Stmt and Expr variant), instantiates each fully bound substitution once (`MonoCtx_seen_insert`), then renames call sites with `fixup_generic_calls` (:1880) per call from its own argument types.
- Inference: `infer_subst`/`infer_from_pair` (:1420/:1438, including `RawPtr` and `Generic` recursion); `subst_with_result` (:1988) also binds from the expected type; `fixup_hinted_call` (:2019) uses a `let`'s declared type or the function's return type; `method_subst` (:1954) binds from arguments after `self`, then from the receiver's type arguments, then `i64`.
- Turbofish (proposal 0002, :1557-1707): the parser records `name::<#k>`; `mono_resolve_turbofish` renames it to `name__T_U`, checks arity (:1593) and that the callee is generic (:1614), and instantiates.
- `Coalesce` is kept as `Expr::Coalesce` (:972); the 10-22 note about a Coalesce→Match desugar no longer applies.

## Findings

### Verified live

#### HIGH — generic calls inside closures, f-strings, pipes, `sandbox` and `parallel` are not renamed

Collection walks every variant, so the instance is made, but the renaming
walkers stop early: `fixup_generic_calls_stmt` handles Expr, Let, LetTuple,
Return, If, While, For, Assign, FieldAssign, IndexAssign, Loop, Defer, Unsafe,
Match and returns anything else unchanged (`other => { return other; }`
:1946); `fixup_generic_calls_expr` returns unchanged (:2233) for Closure,
FString, PipeForward, Spawn, Await, NamedArg, IsCheck, OptionalFieldAccess,
Range, QuantCast, Yield, Comptime*, Asm. Statements left alone include
Sandbox, RegionBlock, Txn, Handle, Do, Select, Parallel, SpawnScope, Query,
GradBlock, Backward, Resume, DerefAssign and `break <value>`. The call keeps
the generic name, which is not in the program after mono.

Probes with `fn ident<T>(x: T) -> T`: `fn(a: i64) { ident(a) + 1 }`,
`f"v={ident(5)}"`, `x |> ident`, `sandbox { r = ident(7); }` and
`parallel { print(ident(3)); | ... }` all type-check and then panic under
`run` with "Unknown function 'ident'". `dump-c` of the sandbox case emits
`ident(v4)` next to a declared `ident__i64`: the C does not link.

Fix: give both fixup walkers an arm for every variant (as the collectors
have), and drop the catch-alls.

#### INFO — core generic programs work

`test_generics.fd` prints `421002050`; `test_generics_full.fd` (payload
types, `map`, turbofish, several bounds) and `test_generic_enums.fd` run.

### Code inspection

#### MEDIUM — no turbofish resolution on the no-std path

`build_program_no_std_borrowck` (compiler.fd:682-935) calls `monomorphize`
but never `mono_resolve_turbofish`, so `name::<#k>` calls stay unresolved in
`--no-std` builds. Not probed.

#### LOW — the renaming and the collection infer differently

For a plain call, collection uses `subst_with_result` (arguments, then the
expected type) but `fixup_generic_calls_expr` uses `infer_subst` on the
arguments only (:2103). Where only the expected type binds a parameter and
the call is not the whole of a `let`/`return` (handled by
`fixup_hinted_call`), the instance made and the name used can differ. The
typechecker rejects most such calls ("cannot infer"), so this is latent.

#### LOW — dead stores `local_renames` / `method_renames`

Filled at :2290 and :2317, never read: `subst_stmt` gets `rename_new()`
(:2331). Same as Rust. Harmless; remove.

#### LOW — process-wide state

`g_mono_tf_*` (:1565-1570) and `g_mono_ret_*` (:1713-1714) are module
globals reset at each entry. Fine for one build per process; the LSP builds
many in one process (each entry resets them first).

## Status of the 2026-10-22 findings

| Finding | Status |
|---------|--------|
| Port improvement: `infer_from_pair` has `RawPtr` + `Generic` arms | **STILL TRUE** (:1438). |
| Dead path: `impl<T>` does not parse, `generic_methods` never filled | **FIXED** — `impl<T> Option` / `impl<T, E> Result` in `stdlib/core.fd:77,156` parse and are instantiated (:1760-1786, :2121-2157). |
| Latent self-param shift in generic method instantiation | **FIXED** — `method_subst` starts at parameter 1 (:1956). |
| Dead store `local_renames` | **STILL TRUE** (:2258/:2290), plus `method_renames` (:2298/:2317). |
| `Coalesce`→`Match` desugar at :981 | **CHANGED** — `Coalesce` is kept (:972); lowering handles it. |
| Method unbound params → `Type::I32` | **CHANGED** — unbound type params become `i64` (:1977-1981). |
| `process_func` clears `is_comptime` | **STILL TRUE** (:2339). |
| Verified live: `test_generics.fd` → 421002050 | **STILL TRUE.** |

## Fix order

1. Complete `fixup_generic_calls_stmt` / `_expr` (HIGH above) and add a test with a generic call in a closure, f-string, pipe, `sandbox` and `parallel`.
2. Call `mono_resolve_turbofish` on the no-std path.
3. Make the fixup use `subst_with_result` like the collector.
4. Remove `local_renames` / `method_renames`.

## Verification

Static read of `monomorphize`, `process_func`, `instantiate`, both fixup
walkers, both collectors, the turbofish block and `method_subst`. Live:
`test_generics`, `test_generics_full`, `test_generic_enums`,
`test_const_generic` run; five probes reproduce the HIGH (interpreter panic),
`dump-c` shows the unrenamed C call.
