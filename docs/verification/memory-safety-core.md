# Memory safety on a core calculus (VF11, first piece)

Status (Nov 7 2026): a small calculus for the ownership rule of
[proposal 0004](../proposals/0004-ownership-and-layout.md), the claim stated
for it, and a proof **sketch** (pen and paper, not mechanised). It shows
what the claim rests on, which is the useful part today: three checks the
compiler must get right, each named below with the code that does it. What
it does not cover is listed at the end.

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

## What the proof leans on

Each lemma above is a check in the compiler. If one is wrong, the claim is.

| Lemma | Check | Where | Tested by |
|---|---|---|---|
| A borrow is never stored, returned, sent or captured by an escaping closure (borrows are second-class) | reference escape check | `compiler/escape.fd` | `test_escape_*_fail.fd` |
| While a borrow of x is live, x is not assigned, dropped or moved (the "not dropped while frame k is live" of Inv 2) | borrow checker, K4/K5 | `compiler/borrowck.fd` | `test_borrow*_fail.fd`, `test_iter_invalidation_fail.fd` |
| Lowering inserts exactly one drop per owner on every path, a `dup` or a move wherever a value is kept, and clears a moved variable | the ownership pass (`LOWER_RC`) | `compiler/lower.fd`, section "Ownership" | `tests/leaks` (`scripts/leak-check.sh`), `--checked-alloc` runs |
| Counts are not shared between threads | values sent to a thread are copied (rule 8) | `lower.fd` spawn lowering | `test_spawn_send_fail.fd` |
| Collections drop their elements once | Vec/HashMap element ownership by type id | `stdlib/core.fd` `Vec_drop`, `HashMap_drop` | `test_vec_builtins.fd` |

The third row is the large one: it is an informal claim about a few
thousand lines of lowering, and it is where bugs have been (AGENTS.md known
bugs 20–25). The fuzzers of VF1/VF2 under `--checked-alloc` (the `checked`
backend of `scripts/fuzz/gen_programs.py`) test it from outside.

## What is left

- Mechanising the calculus and the proof (Coq/Lean/Iris).
- A proof that `lower.fd`'s ownership pass is the translation assumed above
  (today: tests only).
- Copy-on-write (rule 7): a write through a shared buffer copies it first;
  the calculus treats boxes as immutable payload + count, so a write to a
  shared box should be modelled as `dup`-then-write. Not done.
- Structs and enums with managed fields, `Option`/`Result` payloads, the `?`
  error path, `defer`, closures' environments, regions (§8.6), `@linear`
  (§8.7), `Rc`/`Arc`, threads beyond rule 8.
- The C and native runtimes themselves (the allocator, the counted-string
  header of known bug 21) are trusted.
