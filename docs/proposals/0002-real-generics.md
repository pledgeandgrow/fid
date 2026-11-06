# 0002: Real generics

- **Status:** accepted; implemented in `fid` (Nov 4–6) except the items
  under "Left" below. Not in the frozen `fidesc`.
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

## Status of the implementation (Nov 3, 2026)

In the self-hosted compiler (`compiler/*.fd`); the Rust compiler is frozen
(stability.md, ST1) and does not get this.

- Done: type arguments inferred per call, including from the expected
  type; "cannot infer the type parameter" when nothing gives one; bounds
  checked at the call, every bound of `T: A + B` kept and searched for
  methods; turbofish `f::<T>(..)` (the call is instantiated before type
  checking, so later passes see an ordinary call; a wrong count of type
  arguments is an error); generic methods instantiated per receiver type
  (`Option__unwrap__Pt`), with `map`/`and_then`/... keeping the receiver's
  type; an `Option<f64>`/`Option<str>` payload keeps its type with or without
  an annotation; a struct or enum payload of a type parameter is copied to
  the heap, so `Option<Pt>` returned from a function stays valid, and copied
  back out on a match. Tests: `test_generics_full.fd`, `test_generics_real.fd`,
  `test_generic_infer_fail.fd`, `test_generic_bound_fail.fd`,
  `test_turbofish_arity_fail.fd`.
- Left: generic structs and enums laid out per instantiation (a payload is
  still one word; wide values go through a heap copy, which is not freed);
  method-level type parameters (`map<U>`, core's `map` is `fn(T) -> T`);
  method arguments checked against the substituted parameter types; `?` and
  `??` on a struct payload; turbofish on methods, inside the body of a
  generic function, and in `--no-std` builds.

Nov 6: in `fid`, every type parameter is inferred or an error ("cannot infer
the type parameter 'T' of 'make'"), bounds `T: A + B` are checked at the call
("i64 does not implement Show (required by T: Show in 'twice')"), and a
turbofish `f::<T>()` must give as many types as `f` has parameters. Tests:
`test_generics_full.fd`, `test_generics_real.fd`,
`test_generic_infer_fail.fd`, `test_generic_bound_fail.fd`,
`safety/test_generic_unbound_fail.fd`, `test_turbofish_arity_fail.fd`
(checked with `fid run`, `fid ccompile` and `fid emit`; the full suite has
not been rerun since Nov 2). The rules: [reference.md §7](../reference/reference.md#7-generics).
The "Left" list above was written on Nov 3 and has not been rechecked.

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
