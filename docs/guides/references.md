# References

Fides references (`&T`, `&mut T`) are **second-class**: they let a function
read or update a value its caller owns, but they can never be kept anywhere
that could outlive that value. The compiler enforces this with no lifetime
annotations. Swift's `inout` and Hylo use the same approach.

The borrow checker (`docs/guides/borrowchecker.md`) makes sure a value is not
mutated while it is borrowed. The escape check described here makes sure a
reference never outlives what it points to. Together they rule out dangling
references.

## What you can do with a reference

```fides
struct Point { x: i64, y: i64 }

// Borrow for the duration of a call.
fn norm1(p: &Point) -> i64 { return p.x + p.y; }

// Hold one in a local.
var a: i64 = 1;
var r: &i64 = &a;

// Return one that comes from a reference parameter.
fn x_of(p: &Point) -> &i64 { return &p.x; }

// Option, Result and tuples pass a reference through.
fn pick(a: &i64, b: &i64, first: bool) -> Option<&i64> {
    if first { return Option::Some(a); }
    return Option::Some(b);
}
```

The compiler works out which parameters can reach a function's result, so a
caller of `x_of(&p)` knows the result points into `p`. No annotation says so.

## What the compiler rejects

| Rule | Rejected example | Why |
|------|------------------|-----|
| A returned reference must come from a reference parameter | `fn f() -> &i64 { var x = 1; return &x; }` | `x` is freed when `f` returns |
| | `fn f(p: Point) -> &i64 { return &p.x; }` | A by-value parameter is a local of `f` |
| References are not stored | `struct S { r: &i64 }`, `Vec<&i64>`, `Box<&T>`, a global of type `&T` | Storage can outlive the referent |
| | `v.push(&x)`, `S { r: &x }`, `arr[i] = &x` | Same, at the point of storing |
| No reference to a reference | `&&T`, `&mut &T` | A callee could store a reference into the caller's variable |
| A reference must not outlive its block | `var r = &a; { var b = 2; r = &b; } use(r)` | `b` is freed at the end of the block |
| | `val r = { var t = 5; &t };` | The block's value points at its own local |
| References are not generic arguments | `id(&x)` where `fn id<T>(v: T) -> T` | Generic code may store its arguments |
| A closure holding a reference is rooted like that reference | `fn mk() -> fn() -> i64 { var x = 1; val r = &x; return fn() { *r }; }` | Closures capture by value, so the closure carries `r` |
| References are not sent to other tasks | `spawn work(&x)` | The task may outlive `x` |

Code inside `unsafe { }` is not checked.

## How it works

For every expression the compiler computes its **roots**: the variables a
reference in it may point into.

- `&x` is rooted at the local `x`; `&p.field` where `p` is a reference
  parameter is rooted at that parameter.
- A call's result is rooted at the arguments of the parameters that reach the
  callee's result (inferred per function, iterated to a fixpoint).
- `if`, `match`, blocks, `Option`/`Result` constructors and tuples combine the
  roots of their parts; a closure takes the roots of the references it
  captures.
- Arithmetic, field reads of non-tuple values, element reads and dereferences
  produce plain values with no roots.

The check then looks at the places a value can escape: `return`, assignment
to a variable in an outer block, a block's value, stores into fields,
elements or globals, generic parameters and `spawn`. A root that would be
freed before the destination is a compile error naming both.

The pass is `src/escape.rs` (Rust bootstrap) and `compiler/escape.fd`
(self-hosted). It runs right after type checking.

## Limits

Being annotation-free, the check is deliberately conservative: a function
returning a reference is assumed to point into every reference parameter that
can reach its result, and references cannot be stored even where a lifetime
system could prove it safe. None of the existing compiler, standard library,
tests or examples needed either.
