# borrowck.fd — Borrow checker (2,067 LOC)

> **Re-verified 2026-10-23:** headline findings confirmed — `--borrowck=warn` still silent (caller reads `bc.errors`), 9× `_ => {}` catch-alls still hide casts/pipes/tuples. Rule-3 field/deref skip mechanism consistent with source; exact arm not re-traced.

## Role

Lexical/NLL borrow checker — a separate pass that runs **after typecheck, before
comptime substitution and monomorphization**. Port of `src/borrowck.rs` (1,221 LOC).

Three rules (header :7-10):
1. `&mut x` while any borrow on x is active → error
2. `&x` while `&mut x` is active → error
3. mutating `x` while any borrow on x is active → error

Field-path borrows (`&s.x` vs `&s.y` coexist), two-phase borrows (Phase 4:
reserved `&mut` in call args), NLL release at borrower's last use (Phase 6),
closure-capture borrow tracking. Modes: `--borrowck=off|warn|on` (default on).

## Where it's called

| Caller | Site | What |
|--------|------|------|
| `compiler/main.fd:76` | `borrowck_mode_parse(bval)` | parses `--borrowck=` CLI flag |
| `compiler/main.fd:127-131,643,1442,1754` | mode threaded through all cmd_* | string `off/warn/on` → pipeline |
| `compiler/compiler.fd:1332-1333` | `BorrowChecker_new(mode)` + `check(&mut bc, program)` | **the only real invocation** — after typecheck+lints, before `substitute()`/`monomorphize` |
| `compiler/compiler.fd:848` | `BorrowChecker_new(BorrowckMode::Off)` | **dead code** — created in no-std path, `check()` never called, value dropped |

`pattern_bindings`/`str_to_lower`/`box_expr_unbox` are `pub` but have no callers
outside the module (`box_expr_unbox` duplicates `ast.fd:1001` — 45-line copy).

## Live verification (fidesc = Rust oracle; Fides ports logic verbatim)

| Test | Result |
|------|--------|
| `val a = &x; x = 9;` | REJECTED — rule 3 works for whole vars |
| `val r = &s.x; s.x = 99;` | **ACCEPTED** — field-write hole (see F2) |
| `g(&mut x, &mut x)` | REJECTED — "mutable borrow already reserved" (two-phase works) |
| `g(&mut x, &mut x) as i64` | **ACCEPTED** — Cast hides the call (see F3) |
| `val r = &mut x; x = 1;` | ACCEPTED — Rust releases never-read borrow (Fides diverges, see F4) |
| lint on the file | **blocked** — transitive `compiler/ast.fd:344` `Vec<Vec<Stmt>>` hits the in-progress split_gt parser work (`expected > but got 'Shr'`); same for autograd. Not a borrowck issue. |

## Findings

### F1 — `--borrowck=warn` prints nothing (caller regression) — HIGH

`violation()` (:1248-1251) routes Warn-mode messages to `bc.warnings`.
But `compiler.fd:1340-1348` iterates **`bc.errors`** in the warn branch — always
empty in Warn mode. Result: warn mode produces zero output and never fails —
functionally identical to `off` (minus the wasted analysis time).

Rust got it right: `src/compiler.rs:543-546` iterates `bc.warnings`.
**Fix:** in `compiler.fd`, loop `bc.warnings` (and drop the `bc.errors.len() > 0`
gate for warn). 3-line fix in the caller; borrowck itself is correct.

### F2 — FieldAssign / DerefAssign skip Rule 3 — HIGH (inherited from Rust)

`check_stmt` (:1289-1302): `IndexAssign` calls `check_mutation(array)` ✓ but
`FieldAssign` and `DerefAssign` only `check_expr` the operands — **no mutation
check**. `s.x = 99` while `&s.x` (or `&s`) is live is silently accepted —
verified live (prints `1`, no error).

Identical in `src/borrowck.rs:853-866` → inherited design gap, not port drift.
**Fix:** `FieldAssign`: `extract_field_path(object)` → `check_mutation(path)`.
`DerefAssign` needs points-to tracking the checker lacks — at minimum document.

### F3 — `_ => {}` fallthroughs hide whole Expr families — HIGH (inherited)

Both `check_expr` (:1823) and `collect_var_uses_expr` (:742) end in catch-alls.
Unhandled variants with expr children: **`Cast`, `QuantCast`, `IsCheck`,
`PipeForward`, `Asm`, `Comptime`, `ComptimeBlock`, `ComptimeReflect`, `TupleLit`**.

Consequences, verified live:
- `g(&mut x, &mut x) as i64` — the call is never walked → **double-`&mut`
  accepted**. `val t = (&mut x, &mut x)` same via TupleLit.
- NLL `last_use` under-recorded for uses inside those exprs → borrows released
  early (wrong-accept) or, combined with F4, wrongly reported.

With the ast.fd finding (typechecker can't catch a missing match arm), the only
safe pattern is **explicit arms for every variant** — delete the catch-alls.

### F4 — Port drift: never-read borrowers never released — MEDIUM

Rust `collect_var_uses_stmt` records the **defined** name (`src/borrowck.rs:230`
`self.last_use.insert(name, idx)` for Let/LetTuple/LetUninit/Assign targets).
The Fides port (:360-378) deliberately omits this — comment: "a definition is
not a 'use' in NLL… the borrow persists until scope end."

Net effect: `val r = &mut x; x = 1;` — Rust releases r's borrow at stmt 1 →
**accepts** (verified). Fides: `last_use(r)` unset → `release_dead_borrows`
guard `last >= 0` fails → borrow held to scope end → **rejects** (false positive
vs the reference oracle and rustc NLL). Intentional-looking but divergent —
either restore Rust behavior or document + test the stricter semantics.

### F5 — Closure-capture checking is multiply broken — MEDIUM

`Expr::Closure` (:1769-1820):
- `collect_captures_expr_fd` (:2002) has its own `_ => {}` — misses captures in
  StructLit/ArrayLit/If/Match/Deref/StrConcat/etc → under-collected.
- Capture borrows are pushed to the closure's scope → released at `pop_scope`
  → an **escaping** closure keeps no borrow → wrong-accept.
- `is_mut` inferred only from `Assign`/`FieldAssign` — `v.push(x)` in a closure
  counts as *shared* capture → misclassified.
- Compounded by the proven interp `&mut Vec` no-op (arm64_enc report):
  `collect_captures_stmt_fd(..., &mut captured)` writes nothing under
  `fidesc run` → capture checking is a complete no-op in the interpreter;
  only the C backend sees it.

### F6 — All method calls treated as mutations — MEDIUM (design cap)

`Expr::MethodCall` (:1664) `check_mutation(name, 0)` for **every** call on an
Ident receiver: `v.len()` while `&v` is live → spurious error. Rust has the same
approximation — but Rust's `BorrowChecker::new(&tc, mode)` holds the
typechecker; Fides `BorrowChecker_new(mode)` **dropped the tc param**, so
type-aware fixes aren't even possible here. Also: `line=0` → errors report
"line 0" (visible in T5 output).

### F7 — `unsafe` doc vs behavior — LOW

Header (:12): "unsafe { ... } blocks skip all checks". Actually `unsafe_depth`
only gates Ref-borrow creation (:1728) and capture borrows (:1806) —
`check_mutation` still fires inside unsafe (`test_borrowck.fd:60-65` relies on
this: `*r1 = 1` under unsafe). Fix the comment.

### F8 — Temp-borrow leaks in non-trivial initializers — LOW-MEDIUM

`maybe_record_borrow_from_expr` (:1833) only promotes a bare `Expr::Ref`
initializer. A `Ref` nested deeper (`val a = &x + f(0)`, tuples, match arms)
leaves its `__tmp_ref_N` borrow live to scope end — over-conservative false
positives — and `a` is never registered as a borrower.

### F9 — Misc — LOW

- `compiler.fd:848` dead `BorrowChecker_new(Off)` — remove.
- NLL granularity is top-level-statement-only (:330 `stmt_index = i`); nested
  uses collapse to the enclosing index — documented-but-coarse.
- Everything is `Vec`-linear-scan (O(vars²) per function); header admits
  "extremely slow on stdlib" — the Off early-out (:273) exists because of this.
- `str_to_lower`/`pattern_bindings`/`box_expr_unbox` local copies — drift risk
  with the typechecker/ast versions; share or dedupe.
- `check` skips entirely in Off mode (:273) — good; but the `errors`/`warnings`
  vecs persist across `check_func` calls by design (accumulation).

## Fix order

1. `compiler.fd` warn-mode → `bc.warnings` (F1) — one-line caller fix.
2. `check_mutation` on `FieldAssign` via `extract_field_path` (F2).
3. Explicit Expr arms replacing `_ => {}` in `check_expr`/`collect_var_uses_expr`
   (F3) — biggest soundness win.
4. Decide F4: match Rust (release never-read borrows) or document deviation.
5. F5/F6 need design work (capture lifetime, tc access) — document for now.
6. Doc fixes (F7), dead code (F9), tests for each hole.

## Test coverage

`tests/bootstrap/test_borrowck.fd` + `test_borrowck_fail.fd` cover rules 1-3 on
whole vars, field-wise shared borrows, two-phase borrows, basic NLL, unsafe
escape. Nothing exercises F2/F3/F4/F5 holes — which is why they survive.
