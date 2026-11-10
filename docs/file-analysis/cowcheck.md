# cowcheck.fd — Copy-on-write migration report and use-count helpers

> **First audit 2026-11-09** (no earlier doc). Whole-file read plus live
> probes (`s18.exe run` / `emit`).

- **Path:** `compiler/cowcheck.fd`
- **LOC:** 603 (no Rust counterpart)
- **Role:** Two jobs. (1) The migration report of proposal 0001 / release M5 (`FIDES_COW_CHECK=1`): a local copied from a place, changed through a Vec, and never written back. (2) AST walkers that `lower.fd` uses for ownership decisions (proposal 0004): `cow_first_change`, `cow_written_back`, `cow_root`, `cow_uses_stmt(s)`/`cow_uses_expr`.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`. The file has no `module compiler::cowcheck;` line (every other compiler module has one); it is imported as `compiler::cowcheck` all the same.

## Callers

| Caller | Site | What |
|--------|------|------|
| `compiler.fd` main pipeline | :1758-1770 | `cow_check_program(program)` when `FIDES_COW_CHECK` is set; prints `cow: ...` lines to stderr, never fails the build |
| `lower.fd` param entry | :4232 | `cow_first_change(body, param)`: a by-value str/Vec/map/struct parameter the body changes is copied at entry (live, `LOWER_RC`) |
| `lower.fd` param entry | :4215 | same under `LOWER_COW` (off) |
| `lower.fd` `Lowerer_last_use` | :7545, :7549 | `cow_uses_stmt` — move a variable on its last use (live) |
| `lower.fd` `lw_rc_append_parts` | :4936 | `cow_uses_expr` — in-place `s = s + a + b` (live) |
| `lower.fd` `Lowerer_readonly_copy_in` | :7579, :7612-7615 | `cow_root`, `cow_first_change`, `cow_written_back` — `var x = place` may borrow instead of copy; reached from `Lowerer_rc_borrow_copy_ok` (:5340, live when ownership is on) and `Lowerer_readonly_copy` (:5336, `LOWER_COW` only) |
| `lower.fd` `Lowerer_rc_borrow_copy_ok` | :12827 | `cow_root` |

The report runs on every command that builds a program when the variable is
set; the helpers run inside lowering on every build.

## What it does

- `cow_check_program` (:26) → `cow_check_body` (:56): collect `let x = <place>` of non-scalar type (`cow_collect_copies` :131, `cow_place` :106 — `a`, `a.f`, `a[i]`, `a.get(i)`/`first`/`last`), then report the first change through `x` (`cow_first_change` :191: a Vec-changing method on a path from `x`, `&mut` of one, an element or field write through `x.get(i)`/`x[i]`) when `x` is never used whole (`cow_written_back` :319).
- `cow_uses_stmts`/`cow_uses_expr` (:427-603) count uses of a name, returning `COW_MANY` (1000) for a use in a loop, an assignment, `&`, a closure capture, shadowing, or any construct not looked into. Conservative by design: unknown means "many".

## Findings

### Verified live

#### MEDIUM — the report's premise is out of date

The header (:3-7) and every report say a change through a copy "reaches
`<src>` only while Vecs are shared handles. Under copy-on-write it is lost".
Under the ownership lowering (0004) a copy is already a copy. Probes (`run`
and native `emit`): `var b = a; b.set(0, 99); b.push(2)` leaves `a` as `[1]`;
`var w = h.v; w.push(5)` leaves `h.v` empty — and the report still prints
`cow: main (line 5): 'w' is a copy of 'h.v' ...`. Every report is now a
change that is already lost today (a real bug in the reported code, or an
intended copy). The message and the header should say so.

### Code inspection

#### LOW — `cow_first_change` / `cow_written_back` are not conservative

Unlike `cow_uses_*`, these return "no change" / "not used whole" for what
they do not look into: `cow_first_change` skips `Defer`, `Sandbox`, `Txn`,
`Handle`, `Select`, `Parallel`, `SpawnScope`, `LetTuple`, `Break` (:262
`_ => {}`); `cow_expr_changes` skips `Match`, struct/enum/array/tuple
literals, closures, `Try`, `Coalesce`, f-strings (:309); `cow_inner_use`
skips `EnumLit`, `Match`, `Coalesce`, `Try`, f-strings (:409). For the report
this means missed places. In lowering, `Lowerer_readonly_copy_in` uses them
to decide that `var x = place` can borrow the place's handles; a change or a
whole use it does not see could let the borrowed copy outlive or diverge from
its root. Not reproduced: `var x = s;` followed by `s = ...` inside a `match`
expression, or by a `defer` that prints `x`, printed the right values under
`run` and native `emit` (other checks in `Lowerer_rc_borrow_copy_ok` and
`lw_has_defer` stop those cases). Making the two walkers return "changed" /
"used" on anything unknown would make them safe by construction.

#### LOW — copies are collected from a few statement kinds

`cow_collect_copies` (:131) descends into If, While, For, Loop, Match,
RegionBlock, Unsafe, Sandbox; a `let` inside `defer`, `txn`, `select`,
`parallel` or an expression block is not reported.

## Fix order

1. Reword the report and the header: the change is lost now, not "under copy-on-write".
2. Make `cow_first_change`, `cow_expr_changes`, `cow_written_back` and `cow_inner_use` answer "yes" for constructs they do not look into (as `cow_uses_*` do with `COW_MANY`).
3. Add the `module compiler::cowcheck;` line.

## Verification

Whole-file read; lower.fd call sites read. Live: `FIDES_COW_CHECK=1` report
on a probe (one place reported); value-semantics probes under `run` and
`emit`; two borrow-copy probes (no wrong output).
