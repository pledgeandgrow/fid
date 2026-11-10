# Coming from Rust

Fides will feel familiar: ownership, borrows, `struct`/`impl`, traits,
generics by monomorphization, no null, `Option`/`Result`, panics for
unrecoverable errors. The differences are mostly simplifications —
and a few genuinely different ideas (effects, `cap`, annotations).

## Syntax map

| Rust | Fides |
|---|---|
| `let x = 1;` / `let mut x` | `val x = 1;` / `var x = 1;` |
| `fn f(a: i32) -> i32` | `fn f(a: i64) -> i64` |
| `impl Foo { fn m(&self) }` | `impl Foo { pub fn m(self: Foo) }` — self type is explicit |
| `trait T { fn m(&self); }` | `cap T { fn m(self) -> i64; }` |
| `impl T for Foo` | same: `impl T for Foo` |
| `Box<dyn T>` / `&dyn T` | `dyn T` — cast with `x as dyn T` |
| `match x { Some(v) => .. }` | `match x { Option::Some(v: i64) => { .. } }` |
| `x.unwrap_or(0)` | `x ?? 0` |
| `println!("{}", v)` | `println(int_to_str(v))` — no format strings |
| `Vec<T>` methods | `Vec::new()`, `v.push`, `v.get`, `v.len` — same idea |
| `use crate::m` | `use m;` or `use compiler::m;` (path namespaced) |
| `#[derive]` | none — write the impl |
| `cargo build` | `fid emit file.fd` (native) / `fid run` (interp) |

## Ownership — what carried over

- Move semantics, single ownership, `&`/`&mut` borrows, borrow checker
  (`--borrowck=on|warn|off`, default on).
- `&mut` is exclusive; immutable borrows coexist. Violations are
  compile errors, same as Rust.

## What does NOT carry over

- **References are second-class.** You can't store a `&T` in a struct,
  return one from a function, send it to a task, or capture it in an
  escaping closure (`compiler/escape.fd` enforces this). Design with
  indices, copies, or owned handles instead — this is the biggest
  porting adjustment.
- **No explicit lifetimes.** Because references can't escape, the
  borrow checker doesn't need `<'a>` annotations anywhere.
- **No macros.** No `macro_rules!`, no proc macros. `println` is a
  builtin, not a macro — so no `{:?}` formatting.
- **`panic`, not `Result`-everywhere.** Functions that can fail
  recoverably return `Option`/`Result`, but the default error path is
  `panic(...)` with a trace — more like Go's model than Rust's `?`.
- **Annotation-driven codegen.** `@crashfree` (may not panic),
  `@no-alloc`, `@wrapping`, `@send`, `@secret`, `@repr(C)`,
  `@packed`, `@simd` — these are *trusted* contracts the compiler uses;
  some are verified, some are trusted (docs/reference/safety-rules.md).
- **Effects are types.** Every function has an `EffectSet`
  (alloc/read/write/io/diverge); the optimizer uses it, and `cap`
  bounds can require `pure`/`!read` callees (K7 rules).

## A Rust program and its Fides port

```rust
// Rust
trait Speak { fn speak(&self) -> i64; }
struct Cat;
impl Speak for Cat { fn speak(&self) -> i64 { 7 } }
fn main() {
    let c: Box<dyn Speak> = Box::new(Cat);
    println!("{}", c.speak());
}
```

```fides
// Fides
cap Speak { fn speak(self) -> i64; }
struct Cat { }
impl Speak for Cat {
    pub fn speak(self: Cat) -> i64 { return 7; }
}
fn main() -> i64 {
    val c = Cat { } as dyn Speak;
    println(int_to_str(c.speak()));
    return 0;
}
```

## Concurrency

| Rust | Fides |
|---|---|
| `thread::spawn(f)` | `task_go("f", arg)` (green threads) or `spawn` |
| `join_handle.join()` | `task_done(t)` / `task_result(t)` / `task_run()` |
| `rayon::join(a,b)` | `parallel { a | b }` |
| `mpsc::channel` | `use channel;` |
| `Mutex<T>` | `use sync;` |

Spawned-task arguments must be `@send`-safe — same idea as `Send`.

## Mindset differences

- Fides trades Rust's expressiveness-at-the-edges (lifetimes, macros,
  trait objects in generics) for a smaller, self-hosted compiler you can
  read end to end. If you find yourself fighting the escape check, the
  intended answer is usually an index or an owned `Vec`, not a lifetime.
- The compiler is younger — hit a rough edge, check
  `docs/reference/known-bugs.md` and `docs/compiler-internals.md`.
