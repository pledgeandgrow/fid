# Borrow Checker Design Document

> Status: **Phases 1-6 complete — `on` by default, field-wise borrows, two-phase borrows, NLL**
> Created: Aug 2026
> Related: `src/borrowck.rs`, `src/typechecker.rs`, `src/compiler.rs`

## TL;DR

Add a compile-time borrow checker to Fides as a **separate pass** that runs
after typechecking succeeds. It catches aliasing bugs (use-after-free across
borrows, `&mut` aliasing, iterator invalidation) at zero runtime cost, fitting
alongside Fides' existing linear types and regions.

**Key design choice:** Use Fides' existing region system as the borrow
"lifetime." No new lifetime syntax (`'a`), no NLL solver, no variance rules.
A borrow is valid for the scope/region it was created in. Lexical, correct,
occasionally over-restrictive — upgradeable to NLL later.

---

## What Fides Already Has

| Feature | Where | What it catches |
|---------|-------|-----------------|
| `Type::Ref { inner, is_mut }` | `ast.rs:23` | Reference types exist |
| `Expr::Ref { expr, is_mut }` | `ast.rs:215` | `&` / `&mut` expressions |
| Regions (`region r { ... }`) | `typechecker.rs:41-50` | Use-after-free across region boundaries |
| `freed_regions` set | `typechecker.rs:50` | Catches access to popped-region vars |
| Linear types (`@linear`) | `typechecker.rs:70-77` | Double-free, leaks, double-consume |
| `unsafe {}` blocks | `typechecker.rs:81-83` | Escape hatch for raw pointers |

**The gap:** Fides tracks *whether a region is alive* but not *who is borrowing
what inside a live region*. This means `&mut x` aliasing and iterator
invalidation inside a single region are not caught.

---

## The Three Rules

1. **One `&mut` at a time** — creating `&mut x` while any borrow (`&` or `&mut`)
   on `x` is active → error.
2. **Many `&` at a time, but no `&mut`** — creating `&x` while `&mut x` is
   active → error.
3. **No mutation while borrowed** — mutating `x` while any borrow on `x` is
   active → error.

`unsafe { ... }` blocks skip all three rules (existing `unsafe_depth` tracking).

---

## Architecture

### Separate pass, not bolted into typechecker

```
parse → typecheck → [borrowck] → comptime → mono → lower → PIR → codegen
                          ↑ new
```

`src/borrowck.rs` is a standalone module with its own `BorrowChecker` struct.
It takes a `&TypeChecker` reference to reuse region info. The typechecker
stays untouched (except for the one-line `is_mut` fix in Step 1).

### Data structures (the whole state)

```rust
pub struct BorrowChecker<'tc> {
    tc: &'tc TypeChecker,

    // var → set of active shared borrowers
    shared_borrows: HashMap<String, HashSet<String>>,
    // var → active mutable borrower (None = none)
    mut_borrow: HashMap<String, Option<String>>,
    // borrower → (borrowed_var, is_mut)
    borrow_of: HashMap<String, (String, bool)>,

    scope_stack: Vec<HashSet<String>>,
    unsafe_depth: usize,
    errors: Vec<String>,
    warnings: Vec<String>,
}
```

Three maps + a scope stack. Compare to typechecker's ~25 fields.

### When borrows end (lexical, not NLL)

A borrow ends when its borrower:
- Goes out of scope (scope pop), OR
- Is reassigned

This is **lexical borrow scopes** — the same restriction Rust had before NLL.
Correct (never allows a real bug), occasionally over-restrictive. Upgradeable
to NLL later by computing last-use on SSA IR.

---

## Avoiding the Cons

| Con | Mitigation |
|-----|------------|
| Typechecker too big (4952 lines) | Separate `src/borrowck.rs` pass |
| Needs lifetime/NLL dataflow | Statement-level last-use analysis (Phase 6 NLL) |
| Breaks 330+ tests overnight | `--borrowck=off\|warn\|on` flag, default `on` |
| C codegen no back-end enforcement | Front-end-only check (like Rust); backends unchanged |
| User friction | Field-wise borrows + two-phase borrows + NLL + `unsafe {}` escape hatch |
| Redundant for Fides' niche | Only applies to `&T`/`&mut T`; linear types & raw pointers unaffected |

---

## Implementation Phases

### Phase 1: Skeleton (Steps 1-3) — COMPLETE

**Step 1:** Fix `is_mut` bug in `typechecker.rs` `Expr::Ref` (1 line).
Currently `is_mut: _` is discarded and always set to `false`.

**Step 2:** Create `src/borrowck.rs` with:
- `BorrowChecker` struct (three maps + scope stack)
- `check()` entry point that walks functions
- Scope push/pop tracking
- `Expr::Ref` handling that records borrows (Rules 1 & 2)
- Mutation detection (Rule 3) for `Assign`, `IndexAssign`, `DerefAssign`
- `unsafe_depth` tracking (skip checks when > 0)
- No errors emitted yet — just warnings in `warn` mode

**Step 3:** Wire into `compiler.rs` after `tc.check_bodies()` succeeds:
```rust
if borrowck_mode != BorrowckMode::Off {
    let mut bc = BorrowChecker::new(&tc);
    bc.check(&program)?;
    if borrowck_mode == BorrowckMode::Warn {
        for w in &bc.warnings { eprintln!("borrow warning: {}", w); }
    } else {
        if !bc.errors.is_empty() { return Err(bc.errors); }
    }
}
```
Add `--borrowck=off|warn|on` CLI flag (default: `on` — borrow checking is
always active unless explicitly disabled).

### Phase 2: Rules 1-3 active in `warn`/`on` mode — COMPLETE

- Enable error/warning emission for all three rules
- Run `./verify.sh`, collect warnings, fix real bugs / mark intentional with `unsafe`
- Fixed false positive: reassignment of borrower variable (`mr = &mut z`) now
  releases the old borrow BEFORE checking the RHS, so sequential mutable borrows
  on the same variable work correctly
- Fixed `--borrowck=` flag parsing: flags are now filtered from args before
  command-specific parsing, so `fidesc run --borrowck=on file.fd` works
- Tests: `tests/test_borrowck.fd` (valid patterns),
  `tests/test_borrowck_fail.fd` (violations, in expected-fail list)
- All three rules verified:
  - Rule 1: `&mut x` while any borrow active → error
  - Rule 2: `&x` while `&mut x` active → error
  - Rule 3: mutating `x` while any borrow active → error
- `unsafe {}` blocks correctly skip all checks
- Temp borrows in function call arguments correctly released after call

### Phase 3: Field-wise borrows — COMPLETE

Borrow keys are field paths (e.g. `"s.a"`, `"s.inner.x"`), not just variable
names. This unlocks `let a = &s.x; let b = &s.y;` simultaneously. Conflicts
are checked hierarchically:
- `&mut s` conflicts with `&s.x` (whole-var borrow vs field borrow)
- `&mut s.x` conflicts with `&s.x` (same field)
- `&s.x` and `&s.y` coexist (different fields)

### Phase 4: Two-phase borrows — COMPLETE

During call argument evaluation, `&mut` borrows are "reserved" rather than
immediately "activated". A reserved borrow doesn't conflict with shared
borrows (`&x`) created in the same argument list. After all arguments are
evaluated, reserved borrows are activated. This allows patterns like
`copy_first(&mut v, &v)` where the shared borrow is consumed before the
mutable borrow takes effect.

Implementation: `in_reservation_phase` flag + `reserved_mut_borrows` vector
in `BorrowChecker`. `Call` and `MethodCall` handlers set the flag during
argument evaluation, then activate reserved borrows after shared temps are
released.

### Phase 5: Default to `on` — COMPLETE

Borrow checking is now ON by default (`BORROWCK_MODE = 2`). Use
`--borrowck=off` to disable. The `test_borrowck_fail` test was moved to the
expected-fail list since it's designed to fail with borrow checking on.
All 114 normal tests pass with borrow checking on.

### Phase 6: NLL (Non-Lexical Lifetimes) — COMPLETE

Borrows now end at the borrower's last use, not at scope end. This is
implemented via a two-pass approach:
1. **Pre-pass**: scan all statements in the function body, record the last
   statement index that references each variable (`last_use` map).
2. **Main pass**: before processing each statement, release borrows for
   borrowers whose last use was before the current statement
   (`release_dead_borrows`).

This allows patterns like:
```
var v = 10;
val r = &v;       // borrow v
print(*r);       // last use of r — borrow ends here (NLL)
v = 20;          // OK — r is dead, borrow released
```

While still correctly rejecting:
```
var n = 10;
val nr = &n;       // borrow n
n = 20;           // ERROR — nr is still live (used below)
print(*nr);       // nr used after mutation
```

---

## What This Deliberately Does NOT Do

- **No lifetime parameters** in type syntax (`'a`, `&'a T`). Lifetimes are
  implicit (region-scoped).
- **No variance / subtyping** rules for references.
- **No trait-based `Send`/`Sync`.** Fides' `!Spawn` effect handles cross-task
  safety.
- **No backend changes.** `codegen_c.rs`, `codegen_x64.rs`, `interp.rs` — all
  untouched. Borrow checking is purely front-end.

---

## Scope Discipline

The borrow checker only applies to `&T` / `&mut T`. It does **not** touch:
- Linear types (keep working exactly as today)
- Raw pointers (`*mut T` / `*const T` — already gated by `unsafe`)
- Value semantics (moves, copies — unaffected)

If a user writes purely linear-typed code with no references, they see zero
new errors.

---

## Residual Risks

1. **Breaking change when default flips to `on`.** Mitigated by phased rollout
   and `unsafe {}` escape hatch. Not fully avoidable.
2. **Lexical scopes will be annoying** until NLL is added. Resist pressure to
   add NLL too early — it's a real dataflow analysis and buggy checkers are
   worse than strict ones.
3. **Front-end-only means a borrowck bug becomes a silent miscompilation.**
   Same as Rust. Mitigated by thorough testing of the checker itself.
