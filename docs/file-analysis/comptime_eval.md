# comptime_eval.fd

> **Nov 21:** 4,989 lines (2,120 on Nov 9). Fixed Nov 15 (known-bugs
> 2g.19): an assignment inside an `if` in a `comptime fn` is kept (the
> body runs in `evaluate_block_rt`), with break/continue, compound
> operators and shadowing; a statement the evaluator cannot run is an
> error. The errors are no longer dropped: compiler.fd returns
> `comptime_gen_errors()` after `process_gen` and `comptime_subst_errors()`
> after `substitute` (`FID_CT_FALLBACK=1`: the old warn-and-run-at-run-time
> behaviour). `comptime gen` may now add enums, constants, impls and trait
> impls (LG9), and splices are expanded in impl methods too. Not re-checked:
> whether `substitute` reaches impl and trait methods.
>
> **Nov 21 (debcee9):** the comptime-hardening pass (+345), shipped with
> the conformance tests `s4_11_arith_overflow_rejected`,
> `s4_11_const_computes`, `s4_11_error_trace_names_fn`,
> `s4_11_fallback_env_runs`, `s4_11_fatal_not_runtime`,
> `s4_11_fuel_budget_error` and `s4_11_shift_amount_rejected`.
> Evaluation has a fuel budget — `ct_fuel_left` charges every
> `evaluate`/`evaluate_with_env`/`eval_binop`/`call_comptime_func` and
> every statement of `evaluate_block_rt`; over 10^8 steps (default;
> `FID_CT_FUEL` overrides) is an error, never a hang. Errors carry a
> trace: `g_ct_call_stack` gets a frame per `comptime fn` call (popped
> only on success, so a failure shows the whole chain), `ct_trace` prints
> the innermost 8 plus "while running line N" (`g_ct_line`/`g_ct_subst_line`
> from `ct_stmt_line`). Integer arithmetic is checked
> (`ct_add/sub/mul_checked`, `i64::MIN / -1` and `-i64::MIN`, `<<` that
> drops bits, shift amounts outside 0..63) — a wrapped constant table was
> silent before. `Expr::Ident` resolves a program `const`
> (`ct_find_const`/`ct_const_value`, nested consts bounded by the 400-deep
> call guard). `str_repeat` is capped at 16 MB. `struct_size` and
> `field_offset` now read a throwaway `Lowerer` with every struct and
> enum laid out to a fixpoint (`ce_layout_lowerer`; the evaluator's own
> copy of the layout rules — `ce_slot_size`, `ce_struct_layout`,
> `ce_has_annotation` — is deleted; `use compiler::ir`, `compiler::lower`
> are new). Splice errors reach the fatal list (`ct_fatal_splice`), and a
> `reflect` error is fatal even under `FID_CT_FALLBACK` (it used to lower
> to 0). In the Nov 21 comptime commits: generics are
> templates — `substitute`'s need-scan and the `ct_comptime_fns`
> registration both skip any function with `type_params` (:1577, :1628),
> so a generic's comptime nodes and a generic `comptime fn` are left for
> the second `substitute` pass that compiler.fd runs after
> `monomorphize`, where the `comptime T: type` parameters are concrete
> (§4.11). Line numbers below are from Nov 9.

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> `return comptime` and loop accumulation are fixed; impl methods, `if` assignments inside
> comptime functions, and dropped errors are still open.

- **Path:** `compiler/comptime_eval.fd`
- **LOC:** 4,989 (2026-11-21; 2,120 on Nov 9, 1,632 on Oct 22; Rust `src/comptime.rs`: 1,358)
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
