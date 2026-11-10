# borrowck.fd — Borrow checker

> **Re-audited 2026-11-09** against the current source, with live probes
> (`s18.exe run`). Fixed since 10-22: field assignment on a variable,
> trait impl bodies, read-only methods, `for`-loop invalidation, the dead
> no-std checker. Still open: `--borrowck=warn` prints nothing, catch-alls
> hide casts and tuples, nested field writes, closures.

- **Path:** `compiler/borrowck.fd`
- **LOC:** 2,230 (was 2,067 on 10-22; Rust `src/borrowck.rs`: 1,221)
- **Role:** Lexical/NLL borrow checker, a separate pass after type checking, escape and secrecy, before comptime `substitute` and mono.
- **Imports:** `core`, `vec`, `string`, `compiler::ast`.

Rules (header :7-10): (1) `&mut x` while any borrow of x is active; (2) `&x`
while `&mut x` is active; (3) changing x while any borrow of x is active.
Also: field-path borrows (`&s.x` and `&s.y` coexist), `&v[i]` borrows `v`
(:982, release K5), two-phase `&mut` in call arguments, NLL release at the
borrower's last read, closure captures, and changing a Vec a `for` loop
walks (`iterating`, :1318 and :1226, release K4). Errors name the borrow that
holds the place and how to free it (`holder_note` :1187, CM2).

## Callers

| Caller | Site | What |
|--------|------|------|
| `main.fd:95` | `borrowck_mode_parse(bval)` | `--borrowck=` flag (`off/0/no`, `warn/warning/w`, `on/1/yes/err/error`) |
| `compiler.fd:1722-1755` | `BorrowChecker_new(mode)`, `bc.skip_bodies = core_skip`, `check`, `BorrowChecker_free_maps` | main pipeline, after lints |
| `compiler.fd:898-926` | same, no core skip | no-std path (the dead `BorrowChecker_new(Off)` is gone) |
| `tests/selfhost/test_borrowck.fd` | | self-host test |

Not run by `lint` (`lint_source`). `pattern_bindings`, `str_to_lower` and
`bc_box_expr_unbox` are `pub` with no outside callers; `str_to_lower`
(:2051) duplicates core's (`stdlib/core.fd:3911`).

## Findings

### Verified live

#### HIGH — `--borrowck=warn` prints nothing (caller reads the wrong list)

`violation` (:1355-1359) puts Warn-mode messages in `bc.warnings`. Both
callers print `bc.errors` in their warn branch (compiler.fd:1735-1750 and
:907-922), which is empty in Warn mode. Probe: `val r = &x; x = 9;` is an
error with the default mode and prints no warning with `--borrowck=warn`;
warn behaves as `off`. (`README.md:61` advertises `--borrowck=warn`.)

#### HIGH — catch-alls hide casts, tuples, pipes

`check_expr` ends in `_ => {}` (:1986), `collect_var_uses_expr` too (:791).
Not walked: `Cast`, `QuantCast`, `IsCheck`, `PipeForward`, `TupleLit`, `Asm`,
`Comptime`, `ComptimeBlock`, `ComptimeReflect`. Probe:
`g(&mut x, &mut x) as i64` compiles and prints 2 (both writes through one
variable); without `as i64` it is rejected.

#### MEDIUM — a nested field write is not checked

`FieldAssign` checks only an `Expr::Ident` object (:1422-1433). Probe:
`val r: &In = &s.a; s.a.z = 99; print(r.z)` compiles and prints 99. One-level
`s.x = 99` while `&s.x` is live is rejected (fixed). `DerefAssign` still has
no mutation check (:1413-1416).

#### MEDIUM — a closure that changes a captured Vec conflicts with itself

`v.push(1)` inside a closure is not seen as a write by
`collect_assigned_stmt_fd` (Assign/FieldAssign only, :2180), so the capture
is a shared borrow (:1970-1972); checking the body then sees `v.push` as a
change of `v` while that borrow is held. Probe: `val f = fn() { v.push(1); };`
is rejected: "cannot mutate v: shared borrow(s) are active ... Held by: a
closure that captures 'v' (&v, line 0)". Capture borrows also use line 0
(:1972).

#### MEDIUM — a borrow that is never read lives to the end of the scope

`collect_var_uses_stmt` does not record a definition as a use (:399-415), so
`release_dead_borrows` (:832) never frees a borrower that is never read.
Probe: `val r = &mut x; x = 1;` is rejected (Rust's checker and rustc accept
it). Stricter than the reference; intended per the comment, untested.

### Code inspection

#### MEDIUM — closure capture checking is partial

- `collect_captures_stmt_fd` / `_expr_fd` (:2071-2167) end in `_ => {}`:
  captures inside struct/array/enum literals, `if`/`match` expressions,
  f-strings, `match` statements, loops, etc. are missed.
- Capture borrows are pushed into the closure's own scope (:1936, :1973) and
  released at its end (:1982): a closure that escapes (stored, returned)
  holds no borrow afterwards.

#### LOW — the header says `unsafe` skips all checks

Header :12 and :1606. `unsafe_depth` only stops borrow creation (:1891,
:1999) and capture borrows (:1969); `check_mutation` still runs inside
`unsafe`.

#### LOW — a borrow nested in an initializer is not promoted

`maybe_record_borrow_from_expr` (:1996) promotes only a bare `Expr::Ref`
initializer. `val a = (&x, 1)` or `val a = if c { &x } else { &y }` leaves a
`__tmp_ref_N` borrow to the end of the scope and does not tie it to `a`.

#### LOW — method calls are matched by name

`bc_read_method` (:1365) lists read-only methods; any other method on an
`Ident` receiver is a change (:1805, :1827), whatever the receiver's type.
`BorrowChecker_new` takes no typechecker, so it cannot ask. A user method
named `get` that changes its receiver is treated as a read.

#### INFO — cost

All maps are Vecs searched linearly; `last_use` has an index
(`last_use_index`, release D8). Off mode returns at once (:296). Core bodies
are skipped once a core was checked clean (`skip_bodies`, D9). Borrow check
of the compiler itself took 16 s on Nov 6 (language-guide § 10).

## Status of the 2026-10-22 findings

| Finding | Status |
|---------|--------|
| F1 `--borrowck=warn` silent | **STILL TRUE** — compiler.fd:1740 and :914 read `bc.errors`. |
| F2 FieldAssign / DerefAssign skip rule 3 | **CHANGED** — `s.x = ..` on a variable is checked (:1417-1434, release K5); nested objects and `DerefAssign` are not (above). |
| F3 `_ => {}` hides Cast/PipeForward/TupleLit/... | **STILL TRUE** — :1986, :791; `as i64` probe accepted. |
| F4 never-read borrows not released | **STILL TRUE** — :399-415. |
| F5 closure capture checking | **STILL TRUE**, plus the self-conflict on `v.push` (above). |
| F6 every method call is a mutation; `line=0` | **CHANGED** — read-only methods are exempt (`bc_read_method` :1365); method-call errors use `stmt_line`. Still name-based; captures still line 0. |
| F7 `unsafe` comment vs behavior | **STILL TRUE** (:12). |
| F8 temp-borrow leaks in non-trivial initializers | **STILL TRUE** (:1996). |
| F9 dead `BorrowChecker_new(Off)` in no-std path | **FIXED** — the no-std path runs the checker with the requested mode (compiler.fd:898). |
| F9 statement-level NLL granularity | **STILL TRUE** (:367, `stmt_index = i` on top-level statements). |
| F9 local copies (`str_to_lower`, `pattern_bindings`, `box_expr_unbox`) | **CHANGED** — `box_expr_unbox` renamed `bc_box_expr_unbox` (:109); `str_to_lower` duplicates core's. |
| Trait impl bodies never checked | **FIXED** (:317-328, release K5). |

## Fix order

1. compiler.fd warn branches: print `bc.warnings` (two places).
2. Explicit arms for every Expr in `check_expr` and `collect_var_uses_expr`.
3. `FieldAssign` on any place: `extract_field_path(object)` → `check_mutation`.
4. Closures: count changing method calls as mutable captures; keep capture borrows for an escaping closure.
5. Decide F4 (release never-read borrows or document and test it).
6. Fix the header comment; promote nested refs in initializers.

## Test coverage

`tests/bootstrap/test_borrowck.fd`, `test_borrowck_fail.fd`,
`test_iter_invalidation_fail.fd`, `test_field_borrow_fail.fd` (SR-17).
Nothing covers warn mode, casts/tuples around a call, nested field writes,
or a closure that pushes to a captured Vec.

## Verification

Static read of the whole statement and expression walkers, the NLL
pre-pass, the borrow maps and the closure helpers. Live probes: field write
under a borrow (rejected), nested field write (accepted, prints 99),
`as i64` double `&mut` (accepted), never-read `&mut` (rejected), closure
`v.push` (rejected, false positive), warn mode (silent).
