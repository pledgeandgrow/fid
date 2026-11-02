# 0002: Real generics

- **Status:** accepted
- **Decision:** accepted on 2026-11-02, part of removing the technical
  weaknesses listed in the competitive roadmap.

## Problem

Generics are generic in syntax only:

```fides
cap Show { fn show(self) -> str; }
fn first<T>(xs: Vec<T>) -> T { return xs.get(0); }
fn twice<T: Show>(v: T) -> str { return v.show() + v.show(); }

var fs: Vec<f64> = Vec::new(); fs.push(1.5);
println(first(fs));          // fidesc: "Unknown function 'first__T'"
val o: Option<f64> = Option::Some(2.5);
match o { Option::Some(v) => { println(v); } ... }   // fid: 4612811918334230528
println(twice(5));           // accepted; panics at run time: no 'show'
```

`Option` and `Result` are written once over `i64` (`unwrap` returns `i64`), a
type parameter with no binding defaults to `i32`, and bounds are not checked.
So the type checker accepts what it cannot type: an `f64` returned from
`-> i64`, a `str` compared with `0`.

## Change

1. **Every type parameter is inferred or an error.** No default type: "cannot
   infer T for the call to `first`; write `first::<f64>(xs)`".
2. **Bounds are checked at the call:** `twice(5)` is "i64 does not implement
   Show (required by T: Show in twice)", reported at the caller's line.
3. **`Option<T>`, `Result<T, E>` and their methods are generic:**
   `unwrap(self) -> T`, `map<U>(self, f: fn(T) -> U) -> Option<U>`, `context`
   needs `E = str`. A payload keeps its type on every backend (an `f64` is an
   `f64`).
4. **Monomorphization of every generic function, method and impl** in both
   compilers, including generic methods on generic types and calls between
   generic functions (instantiations are created on demand from the call's
   types, as Rust does).
5. **No implicit conversions between unrelated types:** `str` and integers
   only through `as` in the runtime layer; `f64` to `i64` only through `as`.

## What it breaks

Code that relied on the `i64` erasure: an `Option` read as `i64` where it holds
something else, `unwrap()` results used as integers. Code with an unbound type
parameter. Both are bugs today; the compiler names each one.

## Alternatives

- Type erasure with boxing (Java): one copy of each function, but every value
  boxed and slow, and no `f64` without a box.
- Keep `i64` erasure and add checks: still wrong for `f64` and structs wider
  than a word.

## Implementation

`compiler/mono.fd` and `src/mono.rs` already instantiate generic functions
called with concrete types; they are extended to generic enums (`Option<T>`
is a family of enums, laid out by payload size), generic impl blocks, and
instantiation on demand during lowering. The type checker gets a unifier for
type parameters per call (it now treats a `Named` parameter as compatible with
anything). Core's `Option` and `Result` impls become generic. Tests: the probe
above on three backends, a bound-failure and an inference-failure test, and
`Option<f64>`, `Option<str>`, `Option<Struct>`, `Result<Vec<i64>, str>`.
