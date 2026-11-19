# A semantics of core Fides, part 2 (VF9)

Status (Nov 17 2026): this extends [semantics.md](semantics.md) (the integer
core: §1–4 there) to floats, strings, enums with `Option`/`Result`,
generics, closures, references, effects, concurrency and memory (§5–9,
Nov 16), then to `f32`, `i128`, arrays and tuples, traits and `dyn`,
`defer`, regions, `@linear`, effect handlers, channels and `select`, and
async green tasks (§10–19, Nov 17); §20 records three points settled in
the reference (float to integer casts, HashMap order, the output of
threads). The
notation is the same: big-step, `F, ρ ⊢ ⟨e, σ, H⟩ ⇓ ⟨v, σ', H'⟩` or `⇓ panic`,
premises evaluated in order, panic propagating. Each rule names its
reference section ([reference.md](../reference/reference.md)) and the line
of `tests/bootstrap/test_verify_semantics_ext.fd` that tests it (F1, S3, …):
that test prints, rule by rule, what the rule says, so a backend that
disagrees with a rule fails the suite. Memory safety has its own model,
checked by machine: [memory-safety-model.md](memory-safety-model.md).

What part 2 does not do is listed in [§22](#22-what-is-left); the main gap
is the same as part 1's: the rules are written, tested and partly executed,
but not proved in a proof assistant.

## 5. Floats (§3.1, §4.2, §4.6)

Values of `f64` are the IEEE 754 binary64 values (finite, ±0, ±∞, NaN);
`f32` the binary32 ones. Write ⟦x⟧ for the real a finite float denotes and
round(r) for round-to-nearest-even into the type.

- **F-arith.** For ⊕ ∈ {+, −, ×, ÷} on two floats: the IEEE result,
  round(⟦a⟧ ⊕ ⟦b⟧) on finite operands, with the IEEE rules for ±0, ±∞ and
  NaN. **No float operation panics**; in particular a ÷ 0.0 is ±∞ or NaN
  (F1: `0.1 + 0.2 == 0.3` is false; F2: `1.0 / 0.0` > 1.0e308 and
  `-1.0 / 0.0` < −1.0e308).
- **F-mixed.** An integer operand of a float operation is first converted
  as by `as f64` (rule F-itof); the result type is `f64` unless neither
  operand is `f64` (§4.2) (F6).
- **F-cmp.** `==` `!=` `<` `<=` `>` `>=` are IEEE comparisons: every one is
  false when an operand is NaN except `!=`, which is true; −0.0 == 0.0
  (F3, F7).
- **F-ftoi.** `x as iN` / `x as uN` for finite x whose truncation toward
  zero is in range(τ): trunc(⟦x⟧) (F4: 2.7 → 2, −2.7 → −2, −0.5 → 0).
  Out of range, ±∞ or NaN: was unspecified on Nov 16; settled on Nov 17
  (§20.1, reference §4.6): the nearest end of range(τ), and 0 for NaN.
  The fuzzer (`gen_vs_c.py --ext`) generates only in-range casts.
- **F-itof.** `n as f64`: round(n) (F5: 2^53 + 1 → 2^53).
- **F-lit.** A literal `d.d[e±d]` denotes round(its decimal value); `1e5`
  is not a float (§1.6).

Executable form: `scripts/fuzz/gen_vs_c.py --ext` computes its float
operations in C `double` (IEEE on every host it runs on), so VF2 checks
these rules on random programs.

## 6. Strings (§1.8, §4.2, §8.1, §13.4)

A `str` value is a finite sequence of bytes s = s₀…sₙ₋₁. Its length is n,
**including zero bytes** (§1.8, RT2).

- **S-lit.** A literal denotes the bytes of its text after escapes; `\0`,
  `\x00` and `\u{0}` are a zero byte of the string and the literal goes on;
  `\u{…}` is its UTF-8 encoding (S1: `str_len("ab\0cd")` = 5,
  `str_len("\u{e9}")` = 2).
- **S-concat.** `s + t` = the sequence s·t (S2).
- **S-eq.** `s == t` iff the sequences are equal (length and bytes) (S2).
- **S-cmp.** `str_compare(s, t)` ∈ {−1, 0, 1} by lexicographic order on
  bytes as unsigned values, a proper prefix being smaller (S4).
- **S-slice.** `str_slice(s, a, b)` for 0 ≤ a ≤ b ≤ n is sₐ…s_{b−1};
  `str_byte_at(s, i)` for 0 ≤ i < n is sᵢ as an integer 0–255 (S3).
- **S-value.** Strings are values: binding, passing and assigning give the
  same sequence, and nothing can change a sequence in place, so sharing is
  not observable (§8.1) (S5).
- **S-fmt.** `f"…{e}…"` is the concatenation of the literal parts and the
  text form of each `e`, left to right; the text form of an integer is its
  decimal digits with a leading `-` when negative, of a str itself, of a
  bool `true`/`false` (S6).

## 7. Enums, `Option`, `Result`, match (§2.3, §4.5, §6, §13.2)

A value of `enum E` is a pair ⟨V, (v₁, …, vₖ)⟩ of a variant of E and a
payload whose arity and types are V's.

- **E-ctor.** `E::V(e₁, …, eₖ)`: evaluate e₁…eₖ left to right; the value
  is ⟨V, (v₁…vₖ)⟩ (E1).
- **E-match.** `match e { p₁ [if g₁] => b₁, … }`: evaluate e to v; take
  the first i such that pᵢ matches v (binding its names) and gᵢ, evaluated
  with those bindings, is true; evaluate bᵢ. `_` and a bare name match
  anything; `a..=b` matches integers in [a, b], `a..b` in [a, b). A match
  on an enum or bool is exhaustive by the type checker, so no arm matching
  is impossible there; on integers or strings with no arm matching, a
  `match` statement does nothing (E1, E2).
- **E-eq.** For an enum without type parameters, `==` is true iff the
  variants are equal and the payloads are equal component-wise (E3).
- **E-value.** Enums are values (§8.1): assigning the variable afterwards
  does not change a copy (E5).
- **E-try.** `e?` where e ⇓ `Option::None` (or `Result::Err(x)`): the
  enclosing function returns `Option::None` (`Result::Err(x)`); where
  e ⇓ `Some(v)` (`Ok(v)`), the value is v (E4).
- **E-default.** `e ?? d`: e ⇓ `Some(v)` gives v; e ⇓ `None` gives the
  value of d (evaluated only then). `o.unwrap_or(d)` evaluates d first (it
  is a call) (E4).
- **E-unwrap.** `o.unwrap()` on `None` (`Err`) panics.

## 8. Generics, closures, references, effects (§4.10, §7, §8.4, §9.1)

- **G-mono.** A call of a generic function f⟨T̄⟩ with the type arguments ā
  that inference or a turbofish gives (§7.2) means a call of f with ā
  substituted for T̄ in its body: the meaning of a generic program is the
  meaning of its monomorphised program (`compiler/mono.fd`). No rule
  depends on a type argument except through the instance's body (G1, G2).
- **C-make.** `fn(x̄) => e` evaluated in ρ, σ gives the closure value
  ⟨x̄, e, ρ_c⟩ where ρ_c maps each free variable y of e to a **copy** of
  σ(ρ(y)) taken now (capture by value, §4.10) (C1: changing n after the
  closure is made does not change what it computes).
- **C-call.** Calling ⟨x̄, e, ρ_c⟩ with values v̄ evaluates e in ρ_c
  extended with x̄ ↦ v̄. A named function is the closure with empty ρ_c
  (C2, C3).
- **R-ref.** `&x` / `&mut x` evaluate to the location ρ(x) (a place, not a
  copy); `*r` reads σ(r's location); `*r = v` writes it. The borrow and
  escape rules (§8.4) are static: they make every location a reference
  holds alive while the reference is used — that is the content of
  [memory-safety-model.md](memory-safety-model.md) — and they reject
  programs; they add no run-time step (R1, R2).
- **Effects.** Effects (§9.1) are a static judgement Γ ⊢ f : !ε over the
  call graph; they reject programs and **change no rule above**. The
  dynamic semantics of an accepted program is the same with every effect
  annotation erased. (Tested by the rest of the suite's effect tests, not
  here: an effect annotation cannot change a printed value.)

## 9. Memory, collections and concurrency (§8.1, §10, §13.3)

- **M-value.** `Vec`, `HashMap`, `str`, `Box` and structs holding them are
  values: after `var b = a`, no operation on b is observable through a and
  conversely. The lowering implements this with shared buffers and copy
  on the first write (proposal 0004); that the implementation is the value
  semantics is what [memory-safety-model.md](memory-safety-model.md)
  checks (M1, M2, M3).
- **M-vec.** A `Vec<T>` value is a finite sequence. `push(x)` appends;
  `get(i)` for 0 ≤ i < len is the i-th element, otherwise **panic**
  (§8.2); `set(i, x)` likewise. (`pop` of an empty Vec: not settled
  here; the reference does not say.)
- **M-map.** A `HashMap<K, V>` value is a finite map; `insert(k, v)`
  updates it and `get(k)` of a present key gives its value (M3); an absent
  key is not settled here (`get_or` names the default). Iteration order
  (`keys`, `values`): ascending keys, §20.2.
- **T-spawn.** `spawn f(ē)` evaluates ē in the current thread (copying each
  argument whole, §10.1), then starts f(v̄) on a thread of its own; the
  value is a future. `await t` blocks until that thread has returned v
  and gives v. When f and what it calls neither print nor assign a global
  (the race check of §10.6 guarantees the latter for globals; data shared
  through raw pointers is outside the rule), the result is the same as the
  sequential call f(v̄) at the `spawn` (T1). Output of concurrent threads:
  §20.3 (each `print` call's text is whole; calls are ordered only by
  happens-before edges).
- **Panic and exit.** As in part 1 §2: a panic prints `Panic: <message>` on
  stderr and the process exits with status 1; in a spawned thread the
  same (the whole process ends).

## 10. `f32` (§3.1, §4.2, §4.6)

Write round32(r) for round-to-nearest-even into binary32.

- **F32-arith.** For ⊕ ∈ {+, −, ×, ÷} on two `f32` values:
  round32(⟦a⟧ ⊕ ⟦b⟧), with the IEEE rules for ±0, ±∞ and NaN; the result
  is an `f32`. Each operation rounds once, to binary32. (Computing in
  binary64 and rounding the result to binary32 gives the same value for
  these four operations, since 53 ≥ 2·24 + 2; keeping a binary64
  intermediate across two operations does not.)
- **F32-cmp.** As F-cmp, on the binary32 values.
- **F32-cast.** `x as f32` for an `f64` x is round32(x); `y as f64` for an
  `f32` y is exact (every binary32 value is a binary64 value);
  `n as f32` for an integer n is round32(n), rounded once.
- **F32-lit.** A float literal given to an `f32` is the `f64` the literal
  denotes (F-lit) converted by F32-cast.
- **F32-ftoi.** As F-ftoi and §20.1.

Tested by `tests/verify/bugs/m22_f32_rounding.fd` (F8–F10: `0.1f + 0.2f ==
0.3f` holds in binary32, `16777216f + 1f` is 16777216, a third is not the
binary64 third), not by the suite test: two backends break it (§21, M22).

## 11. `i128` and `u128` (§3.1, §3.5)

- **I-core.** The rules of part 1 §1–§3 with w(i128) = w(u128) = 128:
  range(i128) = [−2^127, 2^127 − 1], range(u128) = [0, 2^128 − 1], the same
  panics on overflow, division by zero and min/−1, the same wrap_τ under
  `@wrapping` and in casts (I1: 3·10^9 squared, divided by 7, the
  remainder; all values fit in i64).
- **I-wide.** In particular a value outside range(i64) is a value of
  `i128` (i64::MAX + 1 does not overflow). The compiler breaks this (§21,
  D20): `tests/verify/bugs/d20_i128_wide.fd`.

## 12. Arrays and tuples (§3.2, §8.1)

An array value of type `[T; n]` is a sequence of n values of T; a tuple
value of type `(T₁, …, Tₙ)` is an n-tuple.

- **A-lit.** `[e₁, …, eₙ]`: evaluate e₁ … eₙ left to right; the value is
  ⟨v₁, …, vₙ⟩.
- **A-index.** `a[i]`: evaluate a, then i; **panic** (`Array index out of
  bounds`) unless 0 ≤ i < n; else vᵢ (§3.2, §8.2).
- **A-set.** `a[i] = e` replaces element i of the array a names (same
  check) (A1: `[4, 5, 6]` read at 0 and 2, `[1, 2, 3]` written at 1, the
  array passed whole to a function).
- **A-val.** Arrays are values: `var b = a` and passing `a` by value copy
  the sequence, so `b[0] = 9` does not change `a` (§8.1). The compiler
  breaks this (§21, M21): `tests/verify/bugs/m21_array_assign_aliases.fd`.
- **U-lit.** `(e₁, …, eₙ)`, n ≥ 2: evaluate left to right; the value is the
  n-tuple. `(e)` is e.
- **U-let.** `val (x₁, …, xₙ) = e`: evaluate e once to ⟨v₁, …, vₙ⟩; bind xᵢ
  to vᵢ (U1). A function returning a tuple returns the whole tuple (U1:
  `qr(17, 5)` = (3, 2)).
- **U-val.** Tuples are values (§8.1).

## 13. Traits and `dyn` (§2.5)

- **D-static.** `v.m(ē)` where v : S and S implements the `cap` C that
  declares m: if S's impl of C defines m, call it with self = v; else call
  C's default body of m with self = v. A default body calls methods on
  `self` by the same rule, so it sees the type's own methods (D1: `Pt`'s
  `twice_show` is the default, 2 · `show()`; `Sq` overrides it).
- **D-make.** `e as dyn C`: evaluate e to a value u of type S (S
  implements C); the value is the object ⟨S, u⟩, which holds **a copy** of
  u (structs are values, §8.1). An object stays valid wherever it is
  passed or returned, and assigning the variable e named afterwards does
  not change it. The compiler breaks this (§21, M20):
  `tests/verify/bugs/m20_dyn_from_local_dangles.fd`.
- **D-call.** `d.m(ē)` where d ⇓ ⟨S, u⟩: as D-static with v = u (D2: the
  same function `show_dyn` gives 3 for a `Pt` and 16 for a `Sq`).
- **D-super.** `cap Sub: A + B` adds A's and B's methods to Sub; calls go
  by D-static and D-call.

## 14. `defer` (§5.5)

Write D(r) for the deferred blocks attached to a `return` r: the blocks of
the `defer` statements that come before r in the function's text (§5.5:
attached by position, also when the `defer` was in a branch that did not
run).

- **X-return.** `return e` (or the end of the body): evaluate e to v; then
  run the blocks of D(r), **last registered first**; then the call returns
  v. So a call in e runs before the deferred blocks (X1: `c`, then `r` from
  the return value, then `b`, `a`).
- **X-panic.** A panic ends the program (part 1 §2); deferred blocks do not
  run.
- A deferred block runs at function exit, not at the end of the block
  that holds it.

## 15. Regions (§8.6)

- **RG-scope.** `region r { S }` (with or without `with budget(N)`)
  evaluates as the block `{ S }`: the names it declares are not visible
  after it, and the outcome is the block's outcome.
- **RG-keep.** Freeing at a region's end, by the static drops of release
  C6 or by an arena (`FID_REGION_ARENA=1`), is not observable in an
  accepted program: a value that something outside the region keeps (a
  variable declared before it, a field or element of one, a return value,
  a global, a channel, another task) is unchanged after the region (RG1:
  a string made in the region and pushed on an outer `Vec` prints after
  it).
- **RG-budget.** The budget check is static (it rejects programs) and adds
  no run-time step.

## 16. `@linear` (§8.7)

- **L-erase.** The linearity rules (used exactly once, not in a loop, not
  captured) are static. A `@linear` struct value is a struct value, and an
  accepted program means what it means with `@linear` erased (L1). Since
  the value is used once, whether it is moved or copied is not observable.

## 17. Effect handlers (§9.4)

- **H-handle.** `handle { S } with { E.op(x̄) => { B }, … }`: run S. If S
  completes without reaching a `do`, the statement completes as S does.
- **H-do.** `do E.op(ē)` reached while running S: evaluate ē left to right
  to v̄ (each as `i64`), then run the handler B of E.op with x̄ bound to v̄
  and `cont` bound to k, the rest of S after the `do`. When B completes,
  the `handle` statement completes, and the program continues after it.
- **H-resume.** `resume(cont, v)` in B runs k to its end, then B continues
  after the `resume` (H1: `a`, the handler prints 5, `c`).
- **H-drop.** If B completes without `resume`, k never runs (H2: `c` is
  not printed; `d`, after the statement, is).
- Not covered: the value v given to `resume` (a `do` is a statement in
  the tested programs), a `do` outside any matching handler (lowered as
  the evaluation of its arguments only), nested handlers, `resume` twice.

## 18. Channels and `select` (§10.5)

A channel is a handle c (as `Vec` was in part 1) to a FIFO queue Q(c) of
`i64` values with a capacity.

- **K-send.** `channel_send(c, v)`: wait while Q(c) is full; append v.
- **K-recv.** `channel_recv(c)`: wait while Q(c) is empty and c is open;
  remove and give the first value (K1).
- **K-fifo.** The values one thread sends on c are received in the order
  it sent them, whichever thread receives them (K1, K3: a producer thread
  sends 10, 20, 30; the main thread receives them in that order and then
  awaits the producer's result).
- **K-select.** `select { case c₁ => x => B₁ … [default => B₀] }`:
  evaluate c₁ … cₙ left to right; then, in that order, take the first cᵢ
  whose queue is not empty **now**, remove its first value, bind it to x
  and run Bᵢ; if none, run B₀, or nothing when there is no `default`.
  `select` never waits (K2: 208, then −1 from the default).
- Not settled here: `channel_recv` on a closed, empty channel (the runtimes
  give a sentinel, and do not agree on which), and sending the value
  −9223372036854775807, which `select` takes for "empty".

## 19. Async green tasks (§10.2)

- **Y-call.** Calling an `async fn` f(ē) that the task can carry (§10.2):
  evaluate ē, then start a task running f's body **at once**, on its own
  stack, until the task first waits (`task_yield`, `task_sleep`, an `await`
  of an unfinished future, a channel operation that waits); then the
  caller continues. The value is a future of the task.
- **Y-await.** `await t`: run other tasks until the task of t has
  returned v; the value is v (Y1: 10 and 20). Awaiting a finished task
  gives its value at once.
- **Y-order.** Which ready task runs next is unspecified; a program's
  output may depend on it only through §20.3.
- An `async fn` the task cannot carry runs to its end at the call (§10.2),
  which is then a plain call.
- `fid run` breaks Y-call (§21, D23): it runs every task to its end at the
  call. `tests/verify/bugs/d23_run_async_runs_to_end.fd`.

## 20. Points settled with the reference (Nov 17)

### 20.1 Float to integer casts (reference §4.6)

**F-ftoi-total.** `x as τ` for a float x and an integer type τ never
panics:

- x finite: t = trunc(⟦x⟧) (toward zero); the result is t if
  t ∈ range(τ), min(τ) if t < min(τ), max(τ) if t > max(τ);
- x = +∞: max(τ); x = −∞: min(τ);
- x NaN: 0.

This is Rust's and Java's rule (for the types Java has). It replaces the
"unspecified" of F-ftoi. Repro: `tests/verify/bugs/d21_float_to_int_saturates.fd`
(no backend does it yet: §21, D21).

### 20.2 HashMap iteration order (reference §13.3)

**M-order.** `m.keys()` gives the keys of m in **ascending** order:
integer keys by value, `str` keys by bytes as `str_compare` (a proper
prefix first). `m.values()` gives the values in the order of `m.keys()`.
The order does not depend on the insertion order, the capacity or the
addresses of the strings (O1: keys 30, 10, 20 inserted in that order come
out 10, 20, 30, values 1, 2, 3). String keys: §21, D22,
`tests/verify/bugs/d22_hashmap_str_keys_order.fd`.

### 20.3 Output of several threads and tasks (reference §10.7)

**O-atomic.** The text of one `print` or `println` call is written whole:
another thread's output never appears inside it.

**O-order.** Output is ordered when a happens-before edge orders the calls:
program order inside one thread or task; a thread's (task's) calls before
it returns come before the calls of the thread that awaits its future
after `await` returns (O2: `t` before ` 1`); the joins at the end of
`spawn scope` and `parallel` likewise; a thread's calls before
`channel_send` come before the receiver's calls after the `channel_recv`
that takes that value. Any two calls that no such edge orders may come
in either order.

## 21. Where the compiler disagrees (Nov 17)

Checked with the stage-0 `fid` of Nov 16 (built before this run's
changes), Windows x64, on `run`, `ccompile` (zig cc) and `emit` (pe_ir).
Each repro is in `tests/verify/bugs/` and logged in
[miscompile-log.md](miscompile-log.md).

| Rule | Repro | What the backends do |
|---|---|---|
| S-lit (S1) | `test_frontend_str_nul.fd` | Nov 16: `str_len("ab\0cd")` gave 2 everywhere (a fid built before RT2). Nov 17, stage 0: 5 on `run`, `ccompile` and `emit`: agrees |
| E-value (§7, line E5) | `m23_enum_copy_dse.fd` (M23) | a regression since Nov 16: `emit` prints −100 for the copy `eb` (no variant matches); `FID_IROPT_OFF=dse` makes it right, so iropt's dead store elimination drops the copy's store. `run` and `ccompile` are right |
| F32-arith, F32-cast (§10) | `m22_f32_rounding.fd` (M22) | `ccompile` right; `run` does not round `f64 as f32`; `emit` adds two `f32` wrong and stops with an illegal instruction (known-bugs 2g.13) |
| I-wide (§11) | `d20_i128_wide.fd` (D20) | `i128`/`u128` are lowered to 64 bits (known-bugs 2c.10): i64::MAX + 1 panics on every backend |
| A-val (§12) | `m21_array_assign_aliases.fd` (M21) | `var b = a; b[0] = 9` changes `a` on every backend |
| D-make (§13) | `m20_dyn_from_local_dangles.fd` (M20) | the object holds the address of the local, not a copy: `emit` and `ccompile` read a dead frame; a later assignment to the variable shows through the object |
| Y-call (§19) | `d23_run_async_runs_to_end.fd` (D23) | `run` runs the task to its end at the call (compiler/interp.fd cannot interleave tasks); `ccompile` and `emit` are right |
| F-ftoi-total (§20.1) | `d21_float_to_int_saturates.fd` (D21) | `run` and `emit` give i64::MIN for out of range and NaN, and do not bring narrow results into range; `ccompile` gives what C's undefined cast gives |
| M-order (§20.2) | `d22_hashmap_str_keys_order.fd` (D22) | `HashMap_keys` (stdlib/core.fd) sorts string keys as words (their addresses): `run` and `ccompile` print them in address order; `emit` happened to print them sorted |

Every line of `tests/bootstrap/test_verify_semantics_ext.fd` (F1 to O2)
is meant to print what its rule says on every backend; a rule broken on
some backend has no line there, only the repro above, except E5, which
held on Nov 16 and stays as a regression test. Nov 17, stage 0: through C
all 44 lines then in the file are right; through `emit` all but E5 (M23);
line A1, added after, was checked alone on both; `fid run` stopped
at S3 with `Unknown function 'p_region_word'` (the tree's `stdlib/core.fd`
calls a runtime function this older interpreter does not have: the tree
was in flux, not a rule broken), so lines S4 to O2 are to be checked on
`run` with the rebuilt compiler.

## 22. What is left

- Mechanisation of part 1 and part 2 in a proof assistant (Coq, Lean, K):
  none is installed where this was written, so nothing here is checked by
  one. The executable pieces are `gen_vs_c.py` (part 1, and §5–7 with
  `--ext`), `own_model.py` (§8 references and §9 memory) and the suite
  test with its repros.
- The compiler fixes of §21 (each is a needed change in the compiler or
  in `stdlib/core.fd`, not in these rules).
- Not covered: comptime (§4.11), `parallel` branches (§10.3) beyond the
  join of §20.3, the value passed by `resume`, nested handlers, a closed
  channel, `Rc`/`Arc` counts, `extern "C"` calls, `unsafe` and raw
  pointers (§8.5), tensors.
