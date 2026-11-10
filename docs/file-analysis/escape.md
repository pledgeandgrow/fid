# escape.fd — Reference escape check

> **First audit 2026-11-09** (session 29 module; it had a PROGRESS.md row but
> no analysis doc). Static read of the whole file plus live probes
> (`s18.exe run`). The check is precise and holds on every probe but one
> (a reference passed to a builtin outside the program).

- **Path:** `compiler/escape.fd`
- **LOC:** 1,589 (Rust `src/escape.rs`: 972)
- **Role:** References are second-class (Swift `inout` / Hylo style): they may be passed to calls, held in locals and returned when they come from a reference parameter, never stored anywhere they could outlive what they point to. Runs after type checking.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`.

## Callers

| Caller | Site | What |
|--------|------|------|
| `compiler.fd` main pipeline | :1682-1691 | `escape_check(program)` after type errors, before `secrecy_check`; errors fail the build |
| `compiler.fd` no-std path | :876-884 | same |
| `compiler.fd` `lint_source` | :1930-1933 | same |
| `typechecker.fd` | :8374 | `esc_idents_stmts` (identifiers a closure body reads, for linear captures) |
| `secrecy.fd` | :788 | `esc_idents_stmts` (closure captures) |

`esc_contains_ref` is `pub` but used only inside the module. Reached from
every command that builds a program, and from `lint`. Runs over the whole
merged program (core included, no `skip_bodies`); timed with secrecy as
`escape+secrecy` (compiler.fd:1702).

## What it does

- **Roots.** Each value has a sorted set of `EscRoot`s: a local at block depth `depth`, or the function's parameter `index` (:23-31, kept in Rust's `BTreeSet` order, :85).
- **Declarations** (`esc_check_decls` :440): no struct field, enum payload or global may have a type that holds a reference.
- **Value types** (`esc_check_value_type` :405): a reference may sit at the top of a parameter, local or return type, or inside `Option`/`Result`/tuples (`esc_is_transparent` :152); `&&T` and `Vec<&T>` are errors.
- **Summaries.** For each function whose result can hold a reference (`esc_ret_noref` :306, CS3), which parameters reach the result, found to a fixpoint (`escape_check` :1557-1580); then one reporting pass over every function (:1581-1587).
- **Statements** (`esc_stmt` :819): `let` takes the roots of its initializer; assignment requires the target to outlive every root (`esc_assign_to` :987); a reference cannot be stored in an element (:851), a field (:858, a tuple field passes through), or through another reference (:872); `return` and a function's tail value may carry only parameter roots (`esc_check_return` :668); a block's or branch's value must not point into the block (`esc_leave_blocks` :609); `break <value>` likewise for the loop.
- **Expressions** (`esc_expr` :1036): `&place` roots at the place (`esc_place` :1199; `&f()` is a temporary of the current block); struct literals and non-transparent enum payloads may not hold a reference; arrays neither; `spawn` may not receive one (:1101); calls apply the callee's summary and reject a reference passed to a non-reference parameter (`esc_call` :1242, which also rejects references as generic arguments: `ident(&x)` with `x: T`); method calls by name over every `Type_method` suffix (`esc_method_call` :1279); closures are checked as their own functions and carry their captures' roots (`esc_closure` :1325).
- `unsafe` blocks silence errors (`esc_error` :362).

## Findings

### Verified live

#### INFO — the rules hold on the probes

Rejected: `return &x` of a local; `r = pick(&y, &x)` when `pick` returns its
first parameter and `y` dies first; `keep = Option::Some(&y)` from an inner
block; a returned closure that captures a reference to a local; `ident(&x)`
into a type parameter. Accepted (correctly): the same `pick` returning its
second parameter (summaries are per parameter); a reference passed to a
`fn(&i64) -> i64` parameter.

#### LOW — a reference passed to a builtin outside the program is not checked

`esc_call` returns no roots and checks no argument when the callee is not a
function of the program (:1253-1254). Probe:
`p_vec_push(v, &y)` compiles and runs (the Vec holds the address of `y`).
The same holds for `extern "C"` functions. A list of builtins that keep
their arguments (as `esc_read_only_call` lists those that do not) would close
it.

#### LOW — odd wording for a temporary

`return &y as i64` parses as `&(y as i64)`; the error says "returns a
reference to local 'a temporary value'" (:1219 names the root "a temporary
value", :679 wraps it in "local '...'").

### Code inspection

#### LOW — statements not looked into

`esc_stmt` ends in `_ => {}` (:982): `ComptimeGen` bodies are not checked.
`esc_stmt_value` (:717) treats only Expr/If/Match/Unsafe/Sandbox/Txn/
RegionBlock/Handle as producing a value.

#### LOW — methods are found by name

`method_index` maps every `_`-suffix of every `Type_method` key (:240-255),
so `.get()` considers every type's `get`. A reference argument is accepted
if any candidate takes a reference there (:1289-1295), and results union
every candidate's summary (:1302-1321). Conservative for results; for
arguments it relies on the type checker having matched the real method.

#### INFO — cost

The fixpoint re-analyzes only functions whose result can hold a reference;
the reporting pass analyzes every function once, core included, on every
build.

## Fix order

1. Check arguments of calls to unknown callees against a list of builtins that keep their arguments (`p_vec_push`, `p_vec_set`, map inserts, channel sends, `extern "C"`).
2. Name a temporary as such in the error text.
3. Walk `ComptimeGen` bodies.

## Verification

Tests: `tests/bootstrap/test_escape_closure_fail.fd`, `_return_fail`,
`_scope_fail`, `_store_fail`. Whole-file read. Live: eight probes (above) with `s18.exe run`; seven
behave as the rules say, one (`p_vec_push(v, &y)`) shows the builtin hole.
