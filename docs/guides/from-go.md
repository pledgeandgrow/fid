# Coming from Go

The obvious similarities end at "compiles to a native binary fast."
Fides has an ownership type system, generics, and algebraic-ish
`Option`/`Result` — more ceremony than Go, much less than Rust.

## Syntax map

| Go | Fides |
|---|---|
| `func f(a int) int` | `fn f(a: i64) -> i64` |
| `x := 1` | `val x = 1` (immutable) or `var x = 1` |
| `return v, err` | `return Option::Some(v)` / `Result`, or `panic` |
| `if err != nil` | `match r { Result::Ok(v) => .. }` or `r ?? default` |
| `interface { M() }` | `cap T { fn m(self) -> i64; }` — *explicit* `impl T for X` |
| `struct{}` | `struct X { field: i64 }` |
| `go f()` | `task_go("f", 0)` or `spawn` / `parallel { a | b }` |
| `chan int` | `use channel;` |
| `defer f()` | none — cleanup is explicit (or `@zeroize` for secrets) |
| `nil` | none — `Option::None` |
| `append(v, x)` | `v.push(x)` |
| `map[string]int` | `HashMap::new()` + `m.insert("k", v)` |
| `fmt.Println` | `println(...)` — takes a `str`, not `any` |
| `go build` | `fid emit file.fd` |

## The mental-model changes

- **Ownership, not GC.** Values have one owner; `&`/`&mut` borrows are
  checked at compile time. There is no garbage collector — frees are
  inserted by the compiler (and provably-finished locals free early).
- **Interfaces are explicit.** A type satisfies a `cap` only through an
  `impl Cap for Type` block — no structural/implicit satisfaction.
  Trait objects are `dyn T`, cast explicitly with `x as dyn T`.
- **Errors are typed or fatal.** `panic("msg")` for bugs/invariants
  (prints a trace, nonzero exit); `Option`/`Result` for recoverable
  paths; `??` to unwrap-or-default.
- **No implicit zero-cost abstraction over everything.** `dyn` is a
  real pair (function table + env); closures are `{fn, env}` pairs.
- **Functions are not first-class packages.** `task_go` takes a *name*
  string, not a func value — the runtime resolves it. Closures do
  exist: `fn(x) -> i64 { return x + n; }`.

## Goroutine → task

```go
// Go
go worker(ch)
v := <-ch
```

```fides
// Fides — green threads, see stdlib/task.fd
var t: i64 = task_go("worker", 0);
while !task_done(t) { task_yield(); }
var v: i64 = task_result(t);
```

## What you'll miss, and what replaces it

- `defer`: write the cleanup before `return`s; `@zeroize` covers the
  security-relevant case (wipe on scope exit).
- implicit interfaces: explicit `impl` is more boilerplate, but the
  compiler can devirtualize single-impl calls — the explicitness is a
  feature for the optimizer.
- `errors.Is/As` chains: `panic` traces + `Result` variants instead.

## What Go doesn't have that Fides does

- Compile-time effect tracking: a function marked `pure`/`!read`/`!alloc`
  is checked (and optimizers exploit it).
- `@crashfree`/`@no-alloc` contracts on functions — closer to MISRA-style
  embedded discipline than anything in Go.
- Borrow-checked mutation: two `&mut` aliases can't exist — data races
  on locals are a compile error before threads even enter the picture.
