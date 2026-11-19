# Memory safety on a core calculus (VF11, first piece)

Status (Nov 7 2026): a small calculus for the ownership rule of
[proposal 0004](../proposals/0004-ownership-and-layout.md), the claim stated
for it, and a proof **sketch** (pen and paper, not mechanised). It shows
what the claim rests on, which is the useful part today: three checks the
compiler must get right, each named below with the code that does it. What
it does not cover is listed at the end.

Nov 16–18: the calculus is checked by machine up to a bound
(`scripts/verify/own_model.py`, [memory-safety-model.md](memory-safety-model.md)),
with copy-on-write, boxes in boxes, `&mut` calls and, since Nov 18,
structs and enums with managed fields, closures, `Rc` and two references
at once (§ Extensions below). `scripts/verify/own_diff.py` runs the
model's programs through the compiler and compares (§ The lowering against
the model).

## The claim

> A program accepted by `fid`, with no `unsafe`, `extern` or raw pointer,
> never reads or writes a freed heap block and never frees one twice.

Leaks are not excluded (a leak is safe; `scripts/verify/soak.py` measures
them, and VF16 found one, see [miscompile-log.md](miscompile-log.md) L1).

## The calculus λ-own

Types and values:

    τ ::= int | box             (box: a counted heap value — str, Vec, HashMap)
         | &τ                   (a borrow; second-class: a parameter only)
    v ::= n | ℓ                 (ℓ: a heap address)

A heap H maps addresses to cells ⟨count, payload⟩. A frame maps variables to
values and marks each variable *owner* or *borrow*. Expressions are in
A-normal form (every intermediate value is named), which is what lowering
produces:

    e ::= x | n | new                     (allocate: count 1)
        | dup x                           (count + 1, same ℓ)
        | read x | write x n              (use the payload)
        | f(x₁, …, xₖ)                    (parameters borrow)
    s ::= let x = e                       (x owns the result)
        | x := e                          (drop the old value, own the new)
        | drop x                          (count − 1; free at 0; x cleared)
        | move y ← x                      (y owns, x is cleared: last use)
        | if x { s* } else { s* } | while x { s* } | return x

The lowering of proposal 0004 is a translation from source Fides into this
form: rule 2 gives `dup`/`move` at each place a value is kept, rule 3 a
`drop` at each scope exit and before each assignment, rule 4 a `drop` of
each unkept temporary at the end of its statement, rule 5 borrowed
parameters (with a `dup` at entry when the callee changes one).

Reduction is the usual small-step relation on ⟨frames, H⟩, with one rule of
interest: `read x` / `write x` when x's value ℓ has no cell in H is **stuck**
(that is a use after free), and `drop x` when H(ℓ) has count 0 or no cell is
stuck (a double free).

## The invariant

For a configuration ⟨frames, H⟩ define, for each address ℓ:

- owners(ℓ) = the number of owner variables, in all frames, holding ℓ, plus
  the number of times ℓ is held inside the payload of a live cell (an
  element of a Vec);
- borrows(ℓ) = the borrow variables holding ℓ.

**Inv.** For every ℓ:

1. (counts) ℓ ∈ dom(H) ⟺ owners(ℓ) > 0, and then H(ℓ).count = owners(ℓ);
2. (borrows are covered) if a borrow variable in frame k holds ℓ, some owner
   of ℓ is a variable of a frame j < k (an older frame), or a cell reachable
   from one, and **that owner is not dropped, assigned or moved while frame k
   is live**.

## Proof sketch

*Initial state:* empty heap, no variables: Inv holds.

*Preservation,* by cases on the step:

- `let x = new`: a fresh ℓ with count 1 and one owner (x). (1) holds.
- `let y = dup x`: count + 1, owners + 1. (1) holds.
- `move y ← x`: one owner leaves, one arrives: count unchanged. (1) holds.
- `drop x`: owners − 1 and count − 1; at 0, ℓ leaves H and has no owners.
  Could a borrow of ℓ survive? Only if its covering owner was x; (2) says x
  is not dropped while the borrowing frame is live, and frames newer than
  x's are the only ones that can hold a borrow covered by x. So no borrow
  survives. (2) holds.
- `x := e`: a `drop` of the old value then an own of the new: as above.
- call `f(x₁, …)`: the new frame's parameters are borrows of values the
  caller owns (or borrows, covered further down): (2) holds for the new
  frame with j = the caller or older. Counts do not change. (1) holds.
- `return x`: the callee's owners are dropped (as `drop`); the returned
  value is moved or `dup`ed into the caller's temporary (rule 2: returning
  keeps a value). Its borrows end with the frame. A borrow cannot be
  returned (escape check), so no borrow outlives its frame.
- `read`/`write x`: x holds ℓ; if x is an owner, owners(ℓ) > 0 so ℓ ∈ H by
  (1); if x is a borrow, its covering owner is live by (2), so again ℓ ∈ H.
  The step is not stuck.

*Progress:* by the last case, no `read`/`write` gets stuck, and `drop x` is
only inserted for an owner x, whose count is ≥ 1 by (1). So a well-formed
configuration never reaches the stuck states, which are exactly the use
after free and the double free.

## Extensions (Nov 18)

The calculus grows by four kinds of owner and a second reference. Each is
in the bounded model (`own_model.py --ext ...`); the cases below extend the
proof sketch the same way.

    τ ::= … | Pr                 (a struct { f: box, g: box })
            | Ob                 (an enum { Nb, Sb(box) })
            | fn() -> τ          (a closure; captures by value)
            | Rc<box>            (a counted, shared, unchangeable cell)
    s ::= … | let s = Pr { f: x, g: y } | s.f := y | write s.f | s := t
            | let o = Sb(x) | o := Nb | let y = match o { Sb(v) => v, Nb => new }
            | let c = || e[x] | c()
            | let h = Rc::new(x) | let k = h.clone() | let y = h.get() | h.count()

**Owners.** owners(ℓ) of the invariant now also counts each managed field
of a struct variable, the payload of an enum variable, the box a closure
captured (its environment), and the content of an Rc cell (the cell is one
owner of it). An Rc cell ρ is owned by its handle variables:
H(ρ).count = handles(ρ), so `h.count()` is the number of live handles —
an observable count, which the differential check compares.

**Preservation, the new cases:**

- `let s = Pr { f: x, g: y }`: one `dup` per field (two for x = y): each
  field is one more owner. `let t = s` dups every managed field (the
  lowering's `__own_dup_N`); `s := t` dups t's fields *before* it drops
  s's old ones, which is what keeps `s := s` from freeing (the mutant that
  drops first is caught).
- `s.f := y`: `dup y`, then drop the old field. `write s.f`: copy-on-write
  on the field's cell, as on a variable's.
- An enum's payload is one owner; a `match` that keeps the payload dups it
  (the enum still holds it: it is not a move); `o := Nb` drops it.
- A closure owns a `dup` of what it captures, made when the closure is;
  copying the closure dups the environment, its scope's end drops it. The
  capture is a snapshot: a later write to x finds x's cell shared (count 2)
  and copies it, so the closure still reads the old value — the value
  semantics of "closures capture by value" (reference §4.6). A closure that
  read the variable instead is caught by its first such program.
- `Rc::new(x)`: a fresh cell (count 1) owning a `dup` of x. `clone` counts
  the cell, `get` dups the content, the last handle's drop frees the cell
  and drops the content. Nothing writes through an Rc, so it needs no
  copy-on-write; it holds boxes only, so no cycle can be built (a leak by
  cycle is outside the claim, as in Rust).
- Two references: Inv 2 holds for each separately. Rule 1 (no `&mut x`
  while any borrow of x is live) and rule 2 (no `&x` while `&mut x` is
  live) keep at most one changing path to a cell, so the snapshot a shared
  borrow takes cannot be changed behind it.

The proof of these cases leans on two more checks, added to the table
below: a write needs a `&mut` (a write through `&x` would change a cell
another owner shares without copying it: miscompile log OW1 shows `fid`
accepts one), and the closure's and the Rc's drops (OW3, OW4).

## The lowering against the model

`scripts/verify/own_diff.py` (Nov 18) translates the model's accepted
programs into Fides — a box is `struct Bx { n: i64, kids: Vec<Bx> }`, so
every `dup`, `drop` and copy-on-write of the model is one of
compiler/lower.fd's `LOWER_RC` insertions (`Vec_share`, `__own_dup_N`,
the drops at scope end and before an assignment) — and runs them with
`fid run`, `fid run --leaks` and `--checked-alloc`. Each program must print
what the model's value run reads (values, and `count()` for Rc), leave no
block allocated on its lines, and not fault; programs the model rejects for
a borrow or escape reason must be rejected by `fid check`. This is the
missing link of the third row below, tested rather than proved: a
disagreement is a bug in the lowering, the checker or the model. Of the
Nov 18 runs (with the Nov 16 compiler), one was the model's (it rejected
the two-phase push `x.push(*r)` that `fid` rightly accepts; fixed), the
rest the compiler's (miscompile log OW1–OW7; runs in
[memory-safety-model.md](memory-safety-model.md) §5). OW7 breaks the
claim itself on the C path: `var r = &a; var b = a;` followed by a read
through r faults, because the copy is a move at a's last use by name
while r still points to a.

## What the proof leans on

Each lemma above is a check in the compiler. If one is wrong, the claim is.

| Lemma | Check | Where | Tested by |
|---|---|---|---|
| A borrow is never stored, returned, sent or captured by an escaping closure (borrows are second-class) | reference escape check | `compiler/escape.fd` | `test_escape_*_fail.fd` |
| While a borrow of x is live, x is not assigned, dropped or moved (the "not dropped while frame k is live" of Inv 2) | borrow checker, K4/K5 | `compiler/borrowck.fd` | `test_borrow*_fail.fd`, `test_iter_invalidation_fail.fd` |
| Lowering inserts exactly one drop per owner on every path, a `dup` or a move wherever a value is kept, and clears a moved variable; it moves only a value nothing reads again, through its name or a reference | the ownership pass (`LOWER_RC`; `Lowerer_last_use` for moves) | `compiler/lower.fd`, section "Ownership" | `tests/leaks` (`scripts/leak-check.sh`), `--checked-alloc` runs, `scripts/verify/own_diff.py` (since Nov 18); open: OW2, OW6 (leaks), OW7 (a move while a reference reads the variable: `ow7_copy_moves_while_borrowed.fd`) |
| Counts are not shared between threads | values sent to a thread are copied (rule 8) | `lower.fd` spawn lowering | `test_spawn_send_fail.fd` |
| Collections drop their elements once | Vec/HashMap element ownership by type id | `stdlib/core.fd` `Vec_drop`, `HashMap_drop` | `test_vec_builtins.fd` |
| A change goes through an owner or a `&mut`, never a `&` (so a shared borrow's cell is never written behind it) | type checker / borrow checker | `compiler/typechecker.fd`, `compiler/borrowck.fd` | `test_mut_ref_fail.fd`; `tests/verify/bugs/ow1_write_through_shared_ref.fd` (open: a field write through `&T` is accepted) |
| A struct copy dups every managed field; an assignment dups before it drops | the ownership pass (`__own_dup_N`) | `compiler/lower.fd` | `own_diff.py --ext struct`, `tests/leaks/value_semantics.fd` |
| A closure reads and owns its captures and drops them; an Rc drops what it holds with its last handle | closure and Rc lowering | `compiler/lower.fd` | `own_diff.py --ext closure,rc`; `ow3_closure_capture_struct.fd`, `ow4_rc_of_struct.fd` (open) |

The third row is the large one: it is an informal claim about a few
thousand lines of lowering, and it is where bugs have been (AGENTS.md known
bugs 20–25). The fuzzers of VF1/VF2 under `--checked-alloc` (the `checked`
backend of `scripts/fuzz/gen_programs.py`) test it from outside.

## What is left

- Mechanising the calculus and the proof for all sizes (Coq/Lean/Iris; none
  is installed here). The bounded model checks every program up to 5
  statements, and the extensions up to the same bound.
- A proof that `lower.fd`'s ownership pass is the translation assumed above:
  since Nov 18 a differential test (`own_diff.py`, above), not a proof.
- ~~Copy-on-write (rule 7)~~: in the model since Nov 16 (a write to a shared
  cell copies it and counts its elements).
- ~~Structs and enums with managed fields, closures' environments, `Rc`~~:
  in the model since Nov 18 (fields of a fixed struct `Pr`, a one-payload
  enum, closures capturing one box, `Rc` of a box). Still out: generic
  `Option`/`Result` payloads (the user enum stands for them), the `?` error
  path, `defer`, closures that change their captures, `Box<T>` (a counted
  shared box, LOWER_BOX_RC), `HashMap` and `str`, regions (§8.6), `@linear`
  (§8.7), `Arc` and threads beyond rule 8 (the model is one thread: `Arc`
  would be `Rc` with atomic counts).
- `stdlib/rc.fd`'s `rc_new`/`rc_drop` over `i64` are manual counting (a
  second `rc_drop` frees twice): they are outside the claim, like `unsafe`.
- The C and native runtimes themselves (the allocator, the counted-string
  header of known bug 21) are trusted.
