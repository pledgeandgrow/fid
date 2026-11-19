# Memory safety on a core calculus: the machine-checked part (VF11)

Status (Nov 16 2026; extensions and the differential check against the
compiler Nov 18, §1b and §5). [memory-safety-core.md](memory-safety-core.md) states
the claim and a pen-and-paper proof sketch for the calculus λ-own.
`scripts/verify/own_model.py` checks it by machine, **up to a bound**: it
enumerates every program of an extended λ-own up to N statements, applies
the compiler's rules, and runs each accepted program two ways. This is
exhaustive bounded model checking, not a proof: a bug that needs more than
N statements is not excluded. A Coq/Lean proof is still the goal (§4).

## 1. What is modelled

Source statements over owners `a`, `b`, a reference `r`, and in callees a
`&mut` parameter `p` and a local `t`:

    let x = new | let x = y | x = new | x = y | write x | read x
    x.push(y)   | let y = x.pop()      | let r = &x | let r = &mut x
    r = &x      | r = &mut x           | { … }      | f_k(&mut x)

`x` may be `r` (or `p`) wherever the source allows a write through a
`&mut`. Boxes hold a number and a list of boxes, so a `Vec<Vec<…>>`
is covered, and `push`/`pop` move boxes in and out of boxes. The callees
f₀…f₄ write, reassign, push into themselves, copy-then-write and pop
through their parameter.

**Checks** (the front end): names declared before use; the borrow rules
of reference §8.4 with non-lexical lifetimes (a borrow is live until the
reference's last use; changing the borrowed variable — assignment, write,
push, pop, `&mut` — while it is live is an error; `&x` while `&mut x` is
live is an error); the escape rule (a reference may not be pointed at a
variable of a block deeper than its own).

**Lowered run** (proposal 0004): counted cells; `let x = y` duplicates;
`x = y` duplicates y, then drops x's old value; a write, push or pop on a
cell whose count is above 1 copies it first and counts the elements of the
copy; a block's owners are dropped at its end, last declared first;
parameters are borrowed (places), never dropped by the callee. After every
statement Inv 1 of the core document is checked: each live cell's count is
the number of owners (variables and elements) holding it, and every owner
holds a live cell. A use of a freed cell or a drop of one is a failure; so
is any cell left at the end (a leak).

**Value run:** the same program on plain trees (no heap, no counts); a
reference is the place it names. The lowered run must read exactly what
the value run reads: the lowering is the value semantics (reference §8.1,
semantics-2.md M-value).

**Two-phase borrows** (Nov 18; borrowchecker.md Phase 4): `x.push(*r)` is
accepted when that push is r's last use, also when r borrows x: the push's
`&mut x` is reserved while its argument reads r. The lowered run reads r
(and dups the value) before it changes x, so this is sound, and `fid`
accepts it; without it the model rejected 582 programs of size 4 that
`fid` accepts (found by own_diff.py, §5).

**Moves at last use** (Nov 18; compiler/lower.fd `Lowerer_last_use`):
`let x = y` hands y's value over without a `dup` when no later statement
names y, and y is then "moved" (a read of it is a failure). The compiler
decides by name; the model also requires that no reference pointing to y
is used later. The mutant `move_ignores_refs` drops that second condition
and is caught by `let a = new; let r = &a; { let b = a }; let b = r` (use
after free) — the shape of OW7 (§5), which the compiler gets wrong.

## 1b. Extensions (Nov 18)

`--ext` (default `all`) explores, beside the base calculus, each of these
with a smaller base alphabet (`ext_base`: new, copy, assignment, write,
read, push, one borrow of each kind, the call f₂) up to `--ext-size`
statements (default `--size`):

| Extension | Statements | Lowered run |
|---|---|---|
| `refs2` | a second reference `q`: `let q = &x`, `let q = &mut x`, `q = &x`, writes, reads and pushes through q | each reference is checked on its own (the borrow rules hold for every borrow of x) |
| `struct` | `let s = Pr { f: a, g: a }`, `let u = Pr { f: a, g: *r }`, `let u = s`, `s = u`, `let b = s.f`, `s.f = y`, `write s.f`, `read s` | a struct is two owner slots; a copy dups both fields, an assignment dups the new fields then drops the old, a field write is copy-on-write on the field |
| `enum` | `let o = Sb(x)`, `let o = Nb`, `let w = o`, `o = Sb(x)`, `o = Nb`, `let y = match o { Sb(v) => v, Nb => new }`, `read o` | the payload is one owner; the match dups it |
| `closure` | `let c = \|\| read x`, `let d = c`, `c()` | the closure owns a dup of x made at the capture; a copy dups it; scope end drops it |
| `rc` | `let h = Rc::new(x)`, `let k = h.clone()`, `h = k.clone()`, `let y = h.get()`, `read h.count()`, `read h.get()` | a counted cell owning a dup of x; clone counts the cell, get dups the content; the last handle frees the cell and drops the content; `count` is read as a value and must equal the value run's number of live handles |

The value run gives structs and enums as tuples, a closure the value it
captured, an Rc an id into a table of values that never change.

## 2. Result

| Bound | Programs | Accepted | Failures | Time |
|---|---|---|---|---|
| 3 statements | 1,763 | 1,351 | 0 | < 1 s |
| 4 statements | 70,771 | 46,089 | 0 | 2 s |
| 5 statements | 3,399,403 | 1,859,415 | 0 | 91 s |

(Windows x64, Python 3.14, Nov 16.) Nov 18, with two-phase pushes
accepted: size 4 gives 46,671 accepted, size 5 1,896,043 (50 s), 0
failures. The extensions, Nov 18:

| Extension | Size 4: programs / accepted | Size 5: programs / accepted | Failures | Time (size 5) |
|---|---|---|---|---|
| refs2 | 25,830 / 12,887 | 833,504 / 326,991 | 0 | 11 s |
| struct | 9,303 / 7,857 | 222,960 / 180,453 | 0 | 5 s |
| enum | 48,524 / 44,464 | 1,456,878 / 1,276,583 | 0 | 34 s |
| closure | 13,970 / 11,820 | 349,056 / 278,067 | 0 | 7 s |
| rc | 16,278 / 13,958 | 459,032 / 373,053 | 0 | 10 s |

Each mutant is caught, with the first counterexample the search finds
(the base ones at size 4, Nov 16; the extensions' Nov 18):

| Mutant | Caught by |
|---|---|
| borrow checker off | `let a = new; let r = &a; f1(&mut a); let b = r` → use after free |
| escape check off | `let a = new; let r = &a; { let b = new; r = &b }; let b = r` → use after free |
| `x = y` drops before it duplicates | `…; a = a` → use after free |
| no copy on a shared write | `let a = new; …; f2(&mut a)` (push a into itself) → a cycle, leak |
| the copy does not count its elements | `let a = new; f2(&mut a); f2(&mut a)` → count 1, owners 2 |
| no drop at a block's end | `…{ …; let b = new }` → a cell without owner |
| a callee drops its parameter | `let a = new; …; f0(&mut a)` → an owner holds a freed cell |
| the borrow checker follows only `r` (refs2) | `let a = new; let q = &a; f2(&mut a); read q` → reads the old value |
| a struct copy does not count its fields | `let a = new; write a; let s = Pr { f: a, g: a }; let u = s` → count 3, owners 5 |
| `s.f = y` does not drop the old field | `…; let s = Pr { f: a, g: a }; s.f = a` → count 4, owners 3 |
| `s = t` drops the old fields first | `let a = new; let s = Pr { f: a, g: a }; write a; s = s` → use after free |
| a match keeping the payload does not count it | `…; let o = Sb(a); let b = match o { … }` → count 2, owners 3 |
| a closure captures without counting | `let a = new; write a; write a; let c = \|\| read a` → count 1, owners 2 |
| a closure reads the variable, not the capture | `let a = new; let c = \|\| read a; write a; c()` → reads 1, the value semantics 0 |
| Rc clone does not count | `…; let h = Rc::new(a); let k = h.clone()` → count 1, owners 2 |
| Rc get does not count | `…; let h = Rc::new(a); let b = h.get()` → count 2, owners 3 |
| a move at the last use by name, a reference to the variable read later (OW7) | `let a = new; let r = &a; { let b = a }; let b = r` → use after free |

`python3 scripts/verify/own_model.py --size 4 --self-test` runs on every
push (ci.yml, gates job; since Nov 18 it also explores the five extensions
at size 4, their nine mutants and the move mutant: 17 in all, 5 s); `--size 5` every week
(verify-weekly.yml, own-model job; about 2 minutes with the extensions).
`tests/tooling/vf11-own-model-check.sh` checks all of this and, given a
compiler, runs §5's differential check.

## 3. What it rests on

The model's checks are a transcription of the reference's rules, not of
`compiler/borrowck.fd` and `compiler/escape.fd`; its lowering is a
transcription of proposal 0004, not `compiler/lower.fd`. So it shows the
**rules** are sound (to the bound) and pins which mistakes break them.
That the compiler implements them is tested by the suite
(`test_borrow*_fail.fd`, `test_escape_*_fail.fd`, tests/leaks,
`--checked-alloc`) and, since Nov 18, by running the model's own programs
through the compiler (§5).

## 4. What is left

- A proof in a proof assistant for all sizes (none is installed here).
- ~~Structs with managed fields, enum payloads, closures' environments,
  `Rc`~~ (Nov 18, §1b). Still not in the model: regions, `Arc` and threads,
  `Box<T>` (LOWER_BOX_RC), `HashMap`/`str` payloads, generic
  `Option`/`Result`, closures that change their captures, field borrows
  (`&mut s.f`), a struct or closure behind a reference.
- ~~More than one reference variable~~ (`refs2`, Nov 18: r and q, on one
  owner or two).
- ~~Linking the model to the compiler~~ (§5, Nov 18): a differential test,
  sampled (every statement of each feature, then every k-th program), not
  every program; the full run (`--per -1`) takes hours at one fid process.

## 5. The differential check against the compiler (Nov 18)

`scripts/verify/own_diff.py --fid <fid> [--size N] [--per K] [--rejected M]`
takes K accepted programs of each feature (base and the five extensions):
first one for every statement kind, statement and pair of consecutive
kinds no chosen program has yet, then every k-th of those that read
something. Each becomes a Fides function (a box is
`struct Bx { n: i64, kids: Vec<Bx> }`, `Pr`, `Ob`, closures
`fn() -> str`, `Rc<Bx>`; the callees f₀…f₄ are translated from the model
too); a batch file calls them in turn, each followed by a marker line. It
runs `fid run`, `fid run --leaks` and `fid run --checked-alloc` on each
batch and requires, per program: the printed lines are the model's value
reads, no block allocated on its lines is left, no fault. A batch that does
not build is split until each program `fid check` rejects is alone; those
are listed (`--strict` fails on them). With `--rejected M`, M programs the
model rejects for a borrow or escape reason are each checked alone:
`fid check` must reject them too. The report is `<out>/own_diff.json`.

**Runs, Nov 18** (stage 0 of Nov 16, not the tree's compiler, with the
stdlib of HEAD; Windows, `zig cc`):

| Run | Programs checked | Disagreements |
|---|---|---|
| size 3, 40 per feature, first selection | 228 | 12 rejected by fid; leaks in 19 (base 3, closure 16); rc does not build |
| size 3, 40 per feature, covering selection | 235 | 5 rejected by fid; 6 value mismatches (both closure calls, in all three modes); leaks in 12 (base 1, refs2 1, closure 10); rc does not build; rejected-but-accepted 31 of 52 (27 OW1, 4 two-phase pushes `x.push(*r)`, which the model now accepts) |

| size 4, 60 per feature, two-phase pushes in the model | 344 | 16 rejected by fid (all OW5); 12 value mismatches (4 closure calls × 3 modes, OW3); leaks in 24 (base 7, refs2 1, closure 16); a fault on C in 1 struct program (OW7); rc does not build (OW4); rejected-but-accepted 24 of 60, all OW1 |

What they found (miscompile log OW1–OW7, repros in `tests/verify/bugs/`):

- **OW1** — a write through a shared reference is accepted: `p.n = …`,
  `r.kids.push(…)`, `r.kids.pop()`, `*r = v` where `r: &Bx` (every
  rejected-but-accepted program of the runs, after the two-phase fix of
  the model). The proof needs this check: a shared borrow is a snapshot of
  a cell another owner may share, and a write through it skips the copy.
- **OW2** — `*p = v` through a `&mut` (parameter or local) does not drop
  the old value: a leak.
- **OW3** — a closure capturing a struct with a Vec reads garbage when
  called (every mode) and its environment (16 bytes) is never freed.
- **OW4** — `Rc<Bx>` (an Rc of a struct holding a Vec) does not work:
  `Rc_clone` unknown to the interpreter, the C does not compile.
- **OW5** (stricter, not unsafe) — a borrow with no later use stays active
  to the end of its block, against §8.4's "a borrow ends at the borrower's
  last use": all the "fid rejects" lines.
- **OW6** — popping a struct element (with a Vec field) out of a
  `Vec<Bx>` leaks its blocks.
- **OW7** — a use after move on the C path: `var r = &a; var b = a;
  r.kids.len()` faults ("Invalid memory access at 0x8"): the copy of `a`
  at its last use by name is a move, though `r` still reads `a`. Found as
  `let a = new; let r = &mut a; { let u = Pr { f: a, g: *r }; read u }`,
  in the `--leaks` and `--checked-alloc` runs (`fid run` is right). This is
  the one memory-safety failure of the runs: the claim's third lemma (the
  lowering moves only what nothing reads again) does not hold for a
  variable that a live reference points to.

The value mismatches are OW3's alone; the faults are OW7's and OW4's
(Rc not lowered).

The validation of this goal reruns §5 with the tree's compiler:

    python3 scripts/verify/own_diff.py --fid $FID --size 4 --per 60 --rejected 10 --out build/own-diff
    bash tests/tooling/vf11-own-model-check.sh     # FID=$FID
