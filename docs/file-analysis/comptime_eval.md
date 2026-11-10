# comptime_eval.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> `return comptime` and loop accumulation are fixed; impl methods, `if` assignments inside
> comptime functions, and dropped errors are still open.

- **Path:** `compiler/comptime_eval.fd`
- **LOC:** 2,120 (was 1,632; Rust `src/comptime.rs`: 1,358)
- **Role:** compile-time evaluation. `process_gen` runs `comptime gen { }` blocks, parses the
  string they produce and appends its functions and structs. `substitute` replaces each
  `comptime expr` / `comptime { }` / `comptime reflect(T)` with a literal, then evaluates calls to
  `comptime fn` whose arguments are all literals (I-5A), unrolls small literal loops (I-5D) and
  drops dead `if` branches.

## Callers

| Caller | Site | Use |
|---|---|---|
| `compiler/compiler.fd` | :831 | `process_gen` on imported modules |
| `compiler/compiler.fd` | :1598 | `process_gen` in the main build (before type check) |
| `compiler/compiler.fd` | :1776 | `substitute` (after type check and borrow check, before mono) |

Reached from every command that builds a program (`run`, `ccompile`, `build`/`emit`, `check`,
`lsp`, ...). Because `substitute` runs after the type checker, comptime calls are type-checked as
ordinary calls (an unknown function in `comptime f()` is reported by the type checker).

## What the module does now

- Evaluator `evaluate_block_rt` (:1142-1294) shares the caller's env Vecs, so assignments to
  outer variables inside `if` / `for` bodies in a `comptime { }` block persist; names a block
  declares are truncated at its end.
- `call_comptime_func` (:1468-1660) is a separate, one-level evaluator for `comptime fn` bodies.
- Reflection: all 12 `ReflectOp`s including struct layout (`ce_struct_layout`, :1859), field
  offsets, enum discriminants, function effects and capabilities (:1917-1975).
- `stmt_has_break_or_continue` is now recursive (:804-850).

## Findings

### Verified live (probe file, `fid run` + `fid dump-ir`)

| Source | Result | Status |
|---|---|---|
| `var a = comptime sq(4)` | 16, a constant | ok |
| `fn ret() { return comptime sq(5); }` | constant + return | **FIXED** (`substitute_stmt` rebuilds the return, :357-363) |
| `comptime tri(5)` (for-loop sum) | 10, a constant | **FIXED** (loop body assigns into `env`) |
| method `return comptime sq(6)` in `impl P` | `Call sq` at run time | **STILL TRUE** |
| `comptime pick(true)` with `if b { r = 7; }` | **1** (should be 7) | **STILL TRUE**, wrong value |
| `comptime (10 / 0)` | no diagnostic; panics at run time | **STILL TRUE** |
| `(comptime sq(2), 1)` (tuple) | `Call sq` at run time | **STILL TRUE** |

- **HIGH — wrong value from `if` in a `comptime fn`.** In `call_comptime_func` the `if` arm runs
  its body in a clone (`sub_env`, :1539) and handles only `Let`, `Return`, `Expr` (:1543-1565).
  An `Assign` there is skipped, so the value is silently the old one. The for-loop body (:1610-1650)
  handles `Assign` but not nested `if`/`for`.
- **HIGH — comptime in impl / trait methods is not substituted.** All phases loop over
  `prog.funcs` only (:136-140, :146-150, :262-293). The value is computed at run time
  instead (correct result, not compile time; `@comptime`-only code would not work).
- **HIGH — errors are dropped.** `substitute` and `process_gen` collect `errs` and never return
  them (:131-166, :255-297). A failed evaluation leaves the expression in place; `10 / 0` becomes
  a run-time panic. A `comptime gen` block that fails is removed with no message (:212-222).

### From code inspection

- **MEDIUM — STILL TRUE — `evaluate` returns non-evaluable expressions as they are** (:1124),
  indistinguishable from success.
- **MEDIUM — walker coverage.** `substitute_expr` (:434-552) handles 20 `Expr` kinds; `TupleLit`,
  `Await`, `Try`, `PipeForward`, `Closure`, `IsCheck`, `QuantCast`, `Spawn`, `Yield` fall to
  `_ => {}`. `aggressive_comptime_expr` (:730-791) only walks `Binary`, `Unary`, `Call`,
  `MethodCall`. `aggressive_comptime_stmt` (:601-728) covers 7 statement kinds and drops its
  rewritten `Return` value (:606-612, the copy is never written back).
- **MEDIUM — STILL TRUE — no recursion cap** in `call_comptime_func`; fixed 8 + 1 + 4 passes
  (:262-293) with no fixpoint check.
- **LOW — `!` on an integer gives `IntLit(0/1)`** (:1773-1777), not a bool.
- **INFO — `ct_comptime_fns` is a module global hash map** (:587), rebuilt per `substitute`.

## Fix order

1. Replace `call_comptime_func`'s own statement loop with `evaluate_block_rt` (one evaluator;
   fixes `if` assignments and nesting).
2. Return `errs` from `substitute` / `process_gen` and report them as build errors.
3. Walk `impls[].methods` and `trait_impls[].methods` in every phase.
4. Extend `substitute_expr` to the missing `Expr` kinds; write back the `Return` in the
   aggressive pass.

## Verification state

- `fid check compiler/comptime_eval.fd`: no errors (16 s).
- Live probes as in the table (scratch files `ct.fd`, `ct2.fd`). IR read with `fid dump-ir`
  (which prints instruction kinds, not constant values).
