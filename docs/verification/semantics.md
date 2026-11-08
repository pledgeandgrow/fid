# A semantics of core Fides (VF9, first piece)

Status (Nov 7 2026): this covers the **integer core**: the eight integer
types, `bool`, locals, arithmetic with its panics, casts, comparisons,
short-circuit operators, `if` as a value, blocks, `while`/`for` with
`break`/`continue`, `match` on integers, calls of first-order functions,
structs of integers, and `Vec<i64>` as a handle. It is written as a big-step
operational semantics. It is not mechanised, and it leaves out floats,
strings, enums, generics, closures, references, effects, concurrency and
memory release (see [What is left](#what-is-left)).

It is also executable: `scripts/fuzz/gen_vs_c.py` writes, for each random
program, a C program whose helper functions (`add_i8`, `div_u32`, `shl_i16`,
`vget`, …) are exactly the rules of §3 below, and checks every backend
against it (VF2). A rule changed here must be changed there.

The reference ([reference.md](../reference/reference.md)) is the source of
each rule; its section is given with the rule.

## 1. Values

For an integer type τ ∈ {i8, i16, i32, i64, u8, u16, u32, u64} with width
w(τ) ∈ {8, 16, 32, 64}:

- range(iN) = [−2^(N−1), 2^(N−1) − 1], range(uN) = [0, 2^N − 1];
- a value of type τ is a mathematical integer n ∈ range(τ), written n:τ.

`bool` has the values true and false. A struct value is a finite map from
its field names to values (§2.2: structs are values). A `Vec<i64>` value is
a **handle** h, an address of a heap cell holding a sequence of i64 values
(§8.1: Vec is a handle).

Two functions on mathematical integers are used below:

- wrap_τ(n): the unique m ∈ range(τ) with m ≡ n (mod 2^w(τ));
- trunc(a / b): the quotient rounded toward zero; a rem b = a − b·trunc(a/b)
  (the sign of the remainder is the sign of a).

## 2. Configurations and outcomes

- An environment ρ maps names to locations; a store σ maps locations to
  values; a heap H maps handles to sequences of i64.
- A function table F maps names to (parameters, body, attributes); the one
  attribute here is `@wrapping`.

Evaluating an expression gives either a value or **panic**:

    F, ρ ⊢ ⟨e, σ, H⟩ ⇓ ⟨v, σ', H'⟩        or        F, ρ ⊢ ⟨e, σ, H⟩ ⇓ panic

Executing a statement gives one of: normal completion ⟨σ', H'⟩, `break`,
`continue`, `return v`, or panic. Panic propagates through every rule: if a
premise panics, the conclusion panics, and **no later premise is evaluated**.
The order of premises is the order of evaluation; it is observable only
through panics and output, both of which are part of the outcome.

The observable result of a program is the text it printed and its exit
status: 0 when `main` returns, 1 on panic (§8.2: "prints `Panic: <message>`
and a stack trace on stderr and exits with status 1"). Standard error is not
part of the result.

## 3. Expressions

**Literals.** An integer literal n in a context that expects τ evaluates to
n:τ; the program is rejected at compile time unless n ∈ range(τ) (§3.4). A
literal with no expected type has the smallest of i8, i16, i32, i64 that
holds it; a `val`/`var` with no annotation initialised from literals only is
i64 (§3.4).

**Variables.** ρ(x) = ℓ, σ(ℓ) = v ⟹ x ⇓ v.

**Binary arithmetic.** Both operands have the same type τ (the type checker
inserts no conversion; `as` is required otherwise, §3.4). Evaluate the left
operand, then the right: a:τ, b:τ. Then, in a function **without**
`@wrapping` (§3.5):

| e | result |
|---|---|
| a + b, a − b, a · b | the exact result r if r ∈ range(τ), else **panic** |
| a / b | **panic** if b = 0; **panic** if τ signed, a = min(τ), b = −1; else trunc(a/b):τ |
| a % b | **panic** if b = 0; **panic** if τ signed, a = min(τ), b = −1; else (a rem b):τ |
| −a (τ signed) | **panic** if a = min(τ), else (−a):τ |

In a `@wrapping` function, `+ − ·` and `−a` give wrap_τ(r) instead of
panicking. Division and remainder by zero still panic ("`/` and `%` by zero
always panic", §3.5); min/−1 in a `@wrapping` function is not specified by
the reference and is left out of this core.

**Bitwise.** a & b, a | b, a ^ b and ~a act on the two's-complement bit
patterns of width w(τ) and read the result back as τ. For shifts, the shift
amount s is the value of the right operand, required here to satisfy
0 ≤ s < w(τ) (the reference does not say what a larger amount does; the
fuzzer masks it):

- a << s = wrap_τ(a · 2^s): the low w(τ) bits are kept, never a panic;
- a >> s = floor(a / 2^s): arithmetic for signed τ, logical for unsigned
  (both are floor division of the value, since an unsigned value is ≥ 0).

**Comparisons.** `== != < > <= >=` on two values of type τ compare the
integers (so unsigned types compare unsigned, §3.5) and give a bool. On two
structs of the same type, `==` is true iff every field is equal (§2.2).

**Short circuit.** a && b: evaluate a; if false, the result is false and b
is not evaluated; else the result is b. a || b: dually (§4.2).

**Casts.** For integer types σ → τ: (n:σ) as τ = wrap_τ(n):τ (§4.6). This
covers widening (the value is kept), narrowing (the low bits are kept) and a
change of signedness at the same width (the bit pattern is kept and read in
τ: `200u8 as i8` = −56).

**if as a value.** `if c { e₁ } else { e₂ }`: evaluate c; then exactly one
of e₁, e₂ (§4.7). Its type is the type of the branches; a branch that is
itself an `if` value has that `if`'s type.

**Calls.** f(e₁, …, eₖ): evaluate the arguments left to right to v₁ … vₖ;
bind fresh locations to the parameters (integers and structs are copied:
§2.2 "copied ... when passed by value"; a Vec argument copies the handle);
execute the body under F with f's `@wrapping` attribute; the result is the
value of its `return` (or of its trailing expression, §2.1). A `return`
inside nested loops ends the call.

**Vec.** `Vec::new()` allocates a fresh handle h with H(h) = ⟨⟩.
`v.push(x)` appends; `v.len()` is the length as i64; `v.get(i)` and `v[i]`
are the i-th element (0-based), and **panic** unless 0 ≤ i < len (§8.2);
`v.set(i, x)` likewise. Since v evaluates to a handle, every name bound to
the same handle sees the change (§8.1: `var b = a; b.push(2)` changes `a`).

## 4. Statements

- `val x: τ = e` / `var x: τ = e`: evaluate e to v; bind x to a fresh
  location holding v. `val` names cannot be assigned (§5.1, compile time).
- `x = e`: evaluate e; store it at ρ(x). `x op= e` is `x = x op e` with x
  read once (§5.3). `s.f = e` updates one field of the struct in s.
- `if c { S₁ } else { S₂ }`: evaluate c, run one block. A block's
  declarations are not visible after it.
- `while c { S }`: evaluate c; if false, complete normally; else run S:
  normal or `continue` ⟹ repeat; `break` ⟹ complete normally; `return v` or
  panic ⟹ propagate.
- `for i in a..b { S }`: a and b are evaluated once; i takes a, a+1, …, b−1
  (`..=` includes b) (§5.4); `break`/`continue` as for `while`.
- `match e { p₁ => { S₁ } … _ => { Sₙ } }` on an integer: evaluate e once;
  run the first arm whose pattern holds (a literal k: equality; a range
  `k..=m`: k ≤ v ≤ m; `k..m`: k ≤ v < m; `_` or a name: always) (§6).
- `println(e)` for an integer e of type τ appends the decimal text of the
  integer (unsigned types print unsigned, §3.5) and a newline to the output.

## 5. Where the compiler disagrees (Nov 7)

These are the rules above that some backend breaks; each has a repro in
`tests/verify/bugs/` and an entry in [miscompile-log.md](miscompile-log.md):

- casts at the same width (M1): `fid run` and `fid emit` keep the source
  bits zero-extended, so `200u8 as i8` acts as 200;
- `if` value whose branch is an `if` (M2): rejected as `void`;
- `println` of a u64 ≥ 2^63 (M3): printed signed.

## What is left

- Floats (IEEE-754 binary64, the text `println` gives them), `str` (C
  strings: a NUL ends them, §1.8), enums and `Option`/`Result`, `?` and
  `??`, generics (by monomorphisation, §7), closures (capture by value at
  creation, §4.10), references and the escape rules (§8.4), `defer`
  (function exit, last first, §5.5), effects and capabilities (§9),
  concurrency (§10).
- Memory release (proposal 0004: owners, borrows, drops at scope end), which
  belongs with the memory-safety calculus of [memory-safety-core.md](memory-safety-core.md).
- Evaluation order inside struct literals and of the two operands of a
  comparison chain is taken as left to right; the reference does not say.
- A mechanised version (Coq, Lean or K) and a proof that the C prelude of
  gen_vs_c.py implements §3.
