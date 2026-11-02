# Fides Language Guide

> **A comprehensive guide to how the Fides language works — from source code
> to executable binary, covering the type system, memory model, effect system,
> and compilation pipeline.**

*Last updated: Sep 6, 2026 (session 16 — Tier 3 OS features: @cpu_local, inline asm clobbers, --no-std, UEFI bindings)*

## Table of Contents

1. [Introduction](#1-introduction)
2. [Compilation Pipeline](#2-compilation-pipeline)
3. [Type System](#3-type-system)
4. [Memory & Ownership](#4-memory--ownership)
5. [Effects & Capabilities](#5-effects--capabilities)
6. [Error Handling & Control Flow](#6-error-handling--control-flow)
7. [Intermediate Representations](#7-intermediate-representations)
8. [Backends & Code Generation](#8-backends--code-generation)
9. [Standard Library](#9-standard-library)
10. [Self-Hosting](#10-self-hosting)

---

## 1. Introduction

Fides is a systems programming language designed around three principles:
**safety by construction, verifiable correctness, and zero external
dependencies.** It compiles to native code (x86-64, ARM64, RISC-V), C99,
WebAssembly, and includes a built-in interpreter.

### What Makes Fides Different

| Feature | Fides | Rust | C |
|---------|-------|------|---|
| Memory safety | Compile-time (ownership + regions) | Compile-time (ownership + lifetimes) | Manual |
| Effect tracking | First-class (effects in type system) | None | None |
| Capability security | Compile-time capability grants | None | None |
| Dependency model | Self-contained (no libc, no LLVM) | External (crates, LLVM) | External (libc) |
| Codegen | Direct machine code emission | Via LLVM | Via gcc/clang |
| Self-hosting | ✅ Achieved (25 modules, 61K LOC, bootstrap fixpoint verified) | Yes | Yes |

### Design Philosophy

1. **Safety by construction, not by convention** — The compiler enforces
   memory safety, capability access, and effect annotations at compile time.
   No runtime checks needed for the core guarantees.

2. **If we depend on it, we build it** — No LLVM, no libc, no external
   linker. Every byte of machine code is emitted by Fides itself.

3. **Explicit over implicit** — Effects, capabilities, and ownership
   transfers are visible in the source. The compiler infers types and
   regions, but never silently changes semantics.

4. **Compile-time computation** — `comptime` blocks and `comptime gen`
   enable metaprogramming without a separate macro language.

---

## 2. Compilation Pipeline

Fides source code (`.fd` files) passes through these stages to become an
executable:

```
Source (.fd)
    │
    ▼
┌─────────┐     ┌──────────┐     ┌──────────────┐     ┌───────────┐
│  Lexer   │────▶│  Parser  │────▶│ Import Merge │────▶│ Type Check │
│ (tokens) │     │  (AST)   │     │ + Comptime   │     │ + Borrowck│
└─────────┘     └──────────┘     └──────────────┘     └───────────┘
                                                              │
                                                              ▼
┌──────────┐     ┌──────────┐     ┌───────────┐     ┌───────────────┐
│ Codegen  │◀────│ Optimize │◀────│ PIR Lower │◀────│ IR Lower      │
│ (x86/PE/ │     │ (L2 opt) │     │ (L1→L2→L3)│     │ (AST→SSA IR)  │
│  C/WASM) │     └──────────┘     └───────────┘     └───────────────┘
└──────────┘
    │
    ▼
Executable / C source / WASM
```

### Stage Details

#### 2.1 Lexing (`lexer.rs` / `lexer.fd`)

The lexer is a hand-written character scanner that produces tokens:
- **Keywords:** `fn`, `val`, `var`, `const`, `if`, `else`, `match`, `struct`,
  `enum`, `impl`, `pub`, `use`, `module`, `defer`, `comptime`, `region`,
  `sandbox`, `unsafe`, `txn`, `spawn`, `async`, `await`, etc.
- **Type keywords:** `i8`–`i128`, `u8`–`u128`, `f32`, `f64`, `bool`, `str`, `void`
- **Literals:** integers (dec/hex/bin/oct with `_` separators), floats,
  strings, f-strings (`f"..."`), raw strings (`r"..."`, `r#"..."#`)
- **Operators:** longest-match greedy (e.g. `<<=` before `<<` before `<`)

#### 2.2 Parsing (`parser.rs` / `parser.fd`)

Recursive-descent parser with operator-precedence climbing. Produces an AST
(`Program` containing `Func`, `StructDef`, `EnumDef`, `ImplDef`, `TraitDef`,
etc.).

**Expression precedence** (lowest to highest):

| Level | Operators | Associativity |
|-------|-----------|---------------|
| 1 | `\|>` | left |
| 2 | `??` | left |
| 3 | `..` `..=` | — |
| 4 | `\|\|` | left |
| 5 | `\|` | left |
| 6 | `^` | left |
| 7 | `&` | left |
| 8 | `<<` `>>` | left |
| 9 | `&&` | left |
| 10 | `==` `!=` | left |
| 11 | `<` `>` `<=` `>=` `is` | left |
| 12 | `+` `-` | left |
| 13 | `*` `/` `%` | left |
| 14 | unary `-` `!` `~` `&` `*` | prefix |
| 15 | `.` `?.` `[]` `?` `as` `~>` | postfix |
| 16 | literals, idents, closures | — |

#### 2.3 Import Resolution & Merging

1. Parse stdlib (`core.fd` embedded in compiler)
2. Parse user source
3. Resolve `use` imports from disk (searches `<base>/<path>.fd`,
   `<base>/<path>/mod.fd`, `stdlib/`, project root)
4. Pre-scan imports for transitive dependencies
5. Re-parse with full struct/enum name set (for `EnumName::Variant` parsing)
6. Merge: stdlib → imported modules → user program (user wins on conflicts)
   A user function with the same name as a function of one imported
   module does not replace it: the module's copy is renamed
   `module__name`, so code in the module keeps calling its own function.
   A name defined by two imported modules is an error.

#### 2.4 Type Checking (`typechecker.rs` / `typechecker.fd`)

- Bidirectional type inference with integer literal narrowing
- Region inference (lifetimes deduced automatically)
- Effect checking (function body effects vs. declared effects)
- Capability checking (sandbox restrictions, strict mode)
- Trait impl verification
- Linear type checking (`@linear` structs)
- Borrow checking (separate pass, NLL-based)

#### 2.5 IR Lowering (`lower.rs` / `lower.fd`)

AST → typed SSA IR. Key transformations:
- Mutable variables become `alloca` + `Load`/`Store`
- Closures are lambda-lifted to top-level functions with `__env` struct
- `match` becomes compare-and-branch chains
- `?` operator becomes tag-check + conditional error propagation
- `defer` blocks are pushed onto a stack and emitted at function exit (LIFO)
- Enums become structs with a tag field (field 0) + payload fields

#### 2.6 PIR Lowering (`pir.rs` / `pir.fd`)

SSA IR → three-level PIR (Portable IR):
- **L1:** Adds named regions, capability sets, effect sets to IR instructions
- **L2:** Drops region handles, converts to memory regions, keeps SSA form
- **L3:** Lowers to virtual registers (`VReg`) and stack slots — the final
  representation consumed by native code generators

#### 2.7 Optimization (`opt.rs` / `opt.fd`)

23 optimization passes over L2 PIR:
- **Constant folding** — evaluate constant expressions at compile time
- **CSE/GVN** — common subexpression elimination / global value numbering
- **DCE/DSE** — dead code/store elimination
- **Inlining** — with cost model (blocks×5 + insts, purity bonus)
- **Loop unrolling** — with `@gas`-guided limits
- **TCO** — tail-call optimization
- **Alias analysis** — linear type-based non-aliasing proofs
- **Devirtualization** — DynCall → direct Call
- **Partial evaluation** — evaluate calls with literal args
- **Vectorization, strength reduction, reassociation, memoization**

#### 2.8 Code Generation

| Backend | Output | Use Case |
|---------|--------|----------|
| x86-64 (ELF64) | Native Linux binary | Default `build` |
| x86-64 (PE) | Native Windows binary | Cross-compilation |
| ARM64 (ELF64) | Native ARM binary | Cross-compilation |
| RISC-V (ELF64) | Native RISC-V binary | Cross-compilation |
| C codegen | C99 source | Portability, bootstrap |
| Interpreter | Direct execution | Testing, debugging |
| WebAssembly | `.wasm` | Web deployment |

---

## 3. Type System

### 3.1 Primitive Types

| Type | Size | Description |
|------|------|-------------|
| `i8`–`i128` | 1–16 bytes | Signed integers |
| `u8`–`u128` | 1–16 bytes | Unsigned integers |
| `f32` | 4 bytes | Single-precision float |
| `f64` | 8 bytes | Double-precision float |
| `bool` | 1 byte | Boolean (`true`/`false`) |
| `str` | 8 bytes | String (pointer to runtime string) |
| `void` | 0 bytes | Unit type |
| `Never` | — | Bottom type (unreachable) |

**Note:** In the IR, all values are stored as 8 bytes (i64/pointer-sized).
Smaller types are widened on load and truncated on store. `i128`/`u128` are
lowered to `i64` in the interpreter but supported as `__int128` in C codegen.

### 3.2 Compound Types

```fides
// References
&str           // shared reference (immutable borrow)
&mut str       // mutable reference (exclusive borrow)
*mut T         // raw pointer (unsafe)

// Arrays
[i64; 10]      // fixed-size array
[T; N]         // generic-size array (ArrayConst)

// Tuples
(i64, str, bool)

// Function types
fn(i64, i64) -> i64

// Generic types
Vec<T>
HashMap<K, V>
Option<T>      // generic enum
```

### 3.3 Type Inference

Fides uses **bidirectional inference**:
- **Forward:** literal expressions infer the smallest fitting type
- **Backward:** context (let type, function param, return type) propagates
  hints downward, narrowing integer/float literals

```fides
val x = 42;          // infers i32 (smallest fitting)
val y: i64 = 42;     // hint forces i64
val z = 3.14;        // infers f64
val w = if cond { 1 } else { 2 };  // infers from both branches
```

### 3.4 Generics

```fides
fn identity<T>(x: T) -> T { x }

fn max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

struct Vec<T> {
    data: *T,
    len: i64,
    cap: i64,
}
```

Generic functions are **monomorphized** — each concrete instantiation gets
its own compiled version. Type parameters can have trait bounds (`T: Ord`).

### 3.5 Traits (caps)

Fides calls traits "caps" (capabilities, not to be confused with security
capabilities):

```fides
cap Display {
    fn fmt(&self) -> str;
}

cap Iterator {
    type Item;  // associated type
    fn next(&mut self) -> Option<Self::Item>;
}

impl Display for Point {
    fn fmt(&self) -> str {
        f"({self.x}, {self.y})"
    }
}
```

- Associated types are supported (`type Item;`)
- Default method bodies are supported
- Trait objects via `dyn Trait` (dispatch by type name string at runtime)
- Supertraits: `cap NamedGreet: Greet + Show { ... }`. A subtrait inherits
  every method of its supertraits, so `impl NamedGreet for T` must provide
  them too, and a `dyn NamedGreet` value can call them.

A function that never returns (it always panics or loops) declares `-> !`;
a call to it fits wherever a value of any type is expected.

#### Compile-time reflection

`comptime op(...)` is replaced by a literal while compiling:

| Expression | Result |
|------------|--------|
| `type_name(T)`, `is_struct(T)`, `is_enum(T)` | `str`, `bool` |
| `field_count(T)`, `field_names(T)`, `field_types(T)` | `i64`, `[str]` |
| `enum_variants(E)`, `enum_discriminant(E.Variant)` | `[str]`, `i64` |
| `struct_size(T)`, `field_offset(T.field)` | bytes, as laid out by the compiler |
| `function_effects(f)` | the effects `f` declares |
| `function_capabilities(m)` | the traits implemented by the type defining method `m` |

### 3.6 Type Compatibility Rules

- Exact match (after alias resolution)
- `Never` is compatible with anything (bottom type)
- `Infer` (`_`) is compatible with anything
- Integer widening (i32 → i64) is implicit
- Int → Float conversion is implicit in comparisons and arithmetic with a
  float operand (`f < 0`, `2 * f`)
- `&&` and `||` short-circuit: the right operand is evaluated only when it
  decides the result, so `i < v.len() && v.get(i) == x` never reads past
  the end
- `str` ↔ `i64` is allowed (pointer representation)
- Type parameters are compatible with anything in their scope
- `dyn Trait` is compatible with any concrete impl
- Tuples are checked element-wise

---

## 4. Memory & Ownership

### Integer Arithmetic

The same on the interpreter, the C backend and native Windows executables:

- `+`, `-`, `*` and negation are checked at the declared width: overflow panics.
- In a `@wrapping` function they wrap at that width.
- `/` and `%` by zero always panic, even in `@wrapping` code. `MIN / -1` panics,
  unless `@wrapping`.
- A bare literal takes the other operand's type when it fits; otherwise the
  wider type wins, and the unsigned one at equal width.
- `u64` is unsigned for division, modulo, comparison, `>>` and `print`.
- A panic prints `Panic: <message>` on stderr and exits with status 1. On native
  Windows executables a stack overflow or an invalid memory access does too.

The direct ARM64, RISC-V and WASM backends do not implement this yet; a
cross-build through `ccompile` does.

### 4.1 Memory Model

Fides has no garbage collector. What happens today:

1. `p_malloc` hands out power-of-two blocks from per-class free lists; `p_free`
   returns one (a double free is ignored).
2. A `Vec` that provably never leaves its local variable is freed by the
   compiler: the previous one when the variable is written again, the last one
   at each return. Its elements are not freed.
3. `Vec` growth and `HashMap` rehashing give their old buffers back.
4. A type with a `drop` method has it called at scope exit, for annotated locals.
5. Everything else stays allocated until the process exits. **Strings and
   escaping values are not reclaimed yet**, so a long-running program that
   builds many strings grows. There is no general ownership or RAII.

Structs are copied by default; only `@linear` types move. `Vec` and `HashMap`
values are handles: assigning one aliases it, and nothing tracks that.

You can borrow immutably (`&`) with unlimited readers and mutably (`&mut`) with
one writer, and references cannot outlive what they point to.

### 4.2 Move vs Copy

- **Copy types** (`i32`, `bool`, `f64`, etc.) — copied by default, original
  still valid
- **Move types** (`String`, `Vec`, structs without `Copy`) — moved by
  default, original is invalidated

```fides
val a = 5;
val b = a;       // i32 is Copy — a still valid

val s1 = String::from("hello");
val s2 = s1;     // String is Move — s1 is moved
// println(s1);  // ERROR: s1 was moved
```

### Module Names

Two imported modules may define the same function. Name the one you mean:

```fides
use file;
use fs;
use json as j;

println(file::file_exists("a.txt"));
println(fs::file_exists("b.txt"));
println(j::json_string("x"));
```

A name only one module defines needs no prefix.

### What Safe Code Promises

The goal: **a program with no `unsafe` block cannot corrupt memory.** A memory bug
is rejected at compile time or stops with a panic. Here is exactly how far that
holds today. Safe code means no `unsafe` block and no `@unsafe` function in your
program; the modules it imports are trusted, as is the compiler.

**Holds today:**

- A `Vec` or array index out of bounds panics.
- Integer overflow and division by zero panic, unless the function is `@wrapping`.
- Reading through a raw pointer needs `unsafe`, as do `p_malloc`, `p_free`, making
  a pointer from an integer, and calling `extern "C"` functions.
- A reference cannot be stored, returned from a local, sent to a task, or captured
  by an escaping closure.
- What the compiler frees for you never dangles: when it cannot prove a value is
  finished with, it leaves it allocated. The worst case is a leak.
- A thread cannot write a global of your program without holding a lock.

**Does not hold yet:**

- `var b = a` on a `Vec`, `str` or `HashMap` gives two names for one value, with no
  move check. One name can change what the other sees.
- `push` during a `for` over the same `Vec` compiles. It is bounds-checked, so it
  cannot read outside the `Vec`, but it can skip or repeat elements.
- Race checks cover globals only: a `Vec` shared between threads is not checked.
- The rules above about `unsafe` apply to your program, not yet to imported modules.
  The standard library has not been audited.

**What `unsafe` gives up:** the compiler stops checking raw pointers. Memory freed
and then read gives stale data, with nothing to detect it. A double free panics
unless the block has been reused, in which case it frees the new owner's memory. A
C library can do anything at all. Inside `unsafe`, those are yours to prevent.

`http_serve_scoped` is an `@unsafe` function for this reason: it frees a request's
strings after the handler returns, on your promise that the handler kept none.

### Unsafe Code

A program works with raw memory only inside `unsafe { }`, or in a function
marked `@unsafe`:

```fides
@unsafe
fn first_word(addr: i64) -> i64 {
    var p: *mut i64 = addr;
    return *p;
}
```

Outside them, each of these is a compile error:

- reading or writing through a raw pointer (`*p`, `*p = v`)
- `p_malloc`, `p_alloc`, `p_free`, `p_realloc`, `p_memcpy`, `p_memset`
- making a pointer from an integer
- calling an `extern "C"` function

Inside them the compiler no longer protects you, though a bad free still stops
the program: freeing a block twice panics with `Double free`, and freeing
anything that is not a live block panics with `Invalid free`. A second free
after the block has been reused cannot be detected.

These rules apply to the program itself. Imported modules are not held to them
yet, and Fides does not yet promise full memory safety: see
`docs/roadmap/release-roadmap.md`, track M.

**Strings and Vecs are freed for you.** A temporary string, and a local string
or `Vec` that never leaves its variable, is freed by the compiler. Anything it
cannot prove stays allocated: the worst case is a leak.

### Values That Must Fit

A value given to an integer type must fit it:

```fides
var a: u8 = 300;           // error: the literal does not fit in u8
var big: i64 = 5000000000;
var b: i32 = big;          // error: a i64 may not fit in i32
var c: i32 = big as i32;   // the conversion is explicit
var i = 0;                 // an i64, whatever the size of the literal
```

A float going into an integer needs `as` too.

### Tasks

Tasks are green threads: many of them share one OS thread, each with its own
stack, and one runs until it calls `task_yield`.

```fides
use task;

fn worker(n: i64) -> i64 {
    task_yield();
    return n * 2;
}

fn main() -> i64 {
    val t = task_go("worker", 21);
    task_run();
    println(task_result(t));     // 42
    return 0;
}
```

The interpreter has one call stack, so there a task runs to its end when it
starts; `task_yield` does nothing.

`task_sleep(ms)` and `task_wait_read(fd, timeout_ms)` wait while the other tasks
run. An `async fn` is a task that starts at once and runs until it first waits,
then its caller goes on; `await` gives its result, and meanwhile other tasks run:

```fides
use task;

async fn slow(id: i64) -> Future<i64> {
    task_sleep(10);
    return id * 2;
}

fn main() -> i64 {
    val a = slow(1);
    val b = slow(2);
    println((await a) + (await b));     // 6, after 10 ms
    return 0;
}
```

An `async fn` whose arguments or result are floats, structs or references runs to
its end at the call: a task cannot hold those across a suspension yet.

### Threads and Globals

A function that runs on another thread, or anything it calls, may not write a
global of the program without taking a lock:

```fides
var counter: i64 = 0;
fn bump() { counter = counter + 1; }   // error when `spawn bump()` exists
```

Hold a mutex around the write, or use `p_shared_alloc` and the `p_atomic_*`
functions. Shared data reached through pointers is not checked yet.

### 4.3 Region-Based Memory

Fides supports region-based memory management via `region` blocks:

```fides
fn process(data: &str) -> &str {
    region r {
        val cached = data.to_string();
        &cached    // borrow valid within region r
    }
}
```

- Allocations inside a region get `Region::Named(name)`
- Regions are checked at compile time (scope, budget). They do not free memory yet: `p_region_free` is a no-op
- Region budgets can limit memory usage
- GPU regions (`region gpu { ... }`) for GPU-scoped allocation
- KV-cache regions with sliding window eviction

### 4.4 Borrow Checking

The borrow checker is a separate pass after type checking:
- **NLL (Non-Lexical Lifetimes):** borrows end at last use, not scope end
- **Field-wise borrows:** `&s.x` and `&s.y` can coexist
- **Two-phase borrows:** `&mut` and `&` in same call expression
- Modes: `on` (default), `warn`, `off`

### 4.5 Linear Types

Structs annotated `@linear` are tracked as linear types:
- Must be consumed (moved or explicitly dropped) — cannot be forgotten
- Used for resource handles, database connections, financial transactions
- Alias analysis uses linear pointers to prove non-aliasing

### 4.6 Second-Class References

Fides references (`&T`, `&mut T`) are **second-class**: they let a function
read or update a value its caller owns, but they can never be kept anywhere
that could outlive that value. The compiler enforces this with no lifetime
annotations. Swift's `inout` and Hylo use the same approach.

The borrow checker (`docs/reference/borrowchecker.md`) makes sure a value is not
mutated while it is borrowed. The escape check described here makes sure a
reference never outlives what it points to. Together they rule out dangling
references.

### What you can do with a reference

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

### What the compiler rejects

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

### How it works

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

### Limits

Being annotation-free, the check is deliberately conservative: a function
returning a reference is assumed to point into every reference parameter that
can reach its result, and references cannot be stored even where a lifetime
system could prove it safe. None of the existing compiler, standard library,
tests or examples needed either.

---

## 5. Effects & Capabilities

### 5.1 Effect System

Every function has an inferred **effect set** — the set of side effects it
may perform. Effects are tracked through the IR and checked against declared
annotations.

| Effect | Description |
|--------|-------------|
| `Alloc` | Allocates memory (heap or stack) |
| `IO` | Console/file I/O |
| `Net` | Network I/O |
| `Spawn` | Spawns threads or subprocesses |
| `Diverge` | May not return (infinite loop, panic) |
| `Read` | Reads from memory |
| `Write` | Writes to memory |
| `Diff` | Differentiable operation (autodiff) |
| `Noise` | Adds noise (differential privacy) |
| `Comm` | Cross-device communication |

Functions can declare their effects:

```fides
fn read_config(path: &str) -> Config !FileIO {
    val data = read_file(path)?;
    parse_config(data)
}
```

The compiler verifies that the function body only performs declared effects.

### 5.2 Capability Security

Capabilities control what resources a module can access:

```fides
use fs::file with { Read, Write };
use net::http with { Net };
```

- **Strict mode:** When any `with` clause is present, only listed
  capabilities are granted
- **Sandbox:** `sandbox { ... }` blocks deny all capabilities not explicitly
  granted
- **Transitive:** Imported modules inherit the importer's capabilities
- **Compile-time:** All checks happen at compile time — no runtime overhead

### 5.3 Safety Annotations

| Annotation | Meaning |
|------------|---------|
| `@deterministic` | Output is purely a function of inputs |
| `@crashfree` | Function cannot panic or abort |
| `@no-alloc` | Function performs no heap allocation |
| `@gas(n)` | Function has a gas limit of n operations |
| `@conserve` | Function conserves a resource (no leaks) |
| `@audit` | Function is audited for compliance |
| `@medical` | Function meets medical device safety standards |
| `@linear` | Struct type is linear (must be consumed) |
| `@send` | Struct can be sent between threads |
| `@sync` | Struct can be shared between threads |

### 5.4 Secrets (`@secret`)

`@secret` marks data that must never be revealed: keys, passwords, tokens,
patient records. The compiler proves that no secret, and nothing computed
from one, reaches an output.

### Marking secrets

```fides
// A parameter: this function receives a secret and promises not to leak it.
fn sign(@secret key: str, msg: str) -> i64 { ... }

// A local.
@secret val key = load_from_vault();

// A function whose result is secret wherever it is called.
@secret
fn load_key() -> str { ... }
```

### What counts as a leak

| Leak | Example |
|------|---------|
| Output, logging, network, files | `println(key)`, `log_info(key)`, `p_socket_send(s, key)` |
| Through any helper | `fn show(x: str) { println(x); }` then `show(key)` |
| Through containers and copies | `v.push(key); println(v.get(0))`, `S { k: key }`, `val y = key + "!"` |
| Through control flow | `if key == "x" { println("yes"); }` (which line prints reveals the key) |
| Globals | `g_last = key;` |
| Other tasks | `spawn work(key)` |
| Calls through function values | `callback(key)` (the callee is unknown) |
| Return values | `return key;` from a function not marked `@secret` or `@declassify` |
| Writes through `&mut` parameters | `*out = key;` |

Code inside nested blocks, loops, closures and `unsafe { }` is checked like
any other code.

### Releasing a value on purpose

Some results derived from a secret are meant to be public, such as whether a
password matched. Write the release as a function marked `@declassify`;
callers treat its result as public:

```fides
@declassify
fn password_matches(@secret stored: str, attempt: str) -> bool {
    return stored == attempt;
}
```

Every release point is then easy to find and review (`grep @declassify`).

### How it works

Every value carries a label: whether it may hold a secret, and which of the
function's parameters it may come from. A parameter label that reaches an
output means the function outputs that parameter; that becomes part of the
function's summary, together with which parameters reach its result and
whether it produces output at all. Summaries are inferred for every function
to a fixpoint, so a secret passed to a helper is caught at the call, however
deep the helper chain. Inside a branch or loop whose condition depends on a
secret, everything carries that secret too.

The pass is `src/secrecy.rs` (Rust bootstrap) and `compiler/secrecy.fd`
(self-hosted). It runs after type checking and the reference escape check.

### Wiping secrets from memory (`@zeroize`)

`@zeroize` on a local wipes it when the function returns (after the return
value is computed, together with `defer` blocks and drops):

```fides
fn sign(msg: str) -> i64 {
    @zeroize val key: str = load_key();
    @zeroize var scratch: Vec<i64> = Vec::new();
    ...
    return mac(key, msg);   // computed first, then key and scratch are wiped
}
```

For a `str` the string's bytes are overwritten (string literals are part of
the program and are left alone); for a `Vec` its whole element buffer is.
Every other handle to the same string or Vec sees the zeros afterwards, so do
not return a `@zeroize` string or Vec, or keep another handle to it, if the
data must outlive the function.

The C backend wipes heap data; the interpreters keep strings and Vecs as
managed values and wipe nothing, and the PE backend wipes the variable itself
but not yet the heap bytes.

### Limits

The analysis is conservative: a variable's label is the union of everything
it is ever assigned, so a variable reused for a secret and then for public
data stays secret. Lengths are not treated specially (`key.len()` is
secret). Timing is a separate concern: use `@constant_time` for code whose
running time must not depend on a secret.

---

## 6. Error Handling & Control Flow

### 6.1 Result Type

```fides
// Result is modeled as a struct with a tag field
// tag 0 = Ok, tag 1 = Err
fn parse_int(s: &str) -> Result<i64, ParseError> {
    // ...
    Ok(42)
}
```

### 6.2 The `?` Operator

The `?` operator propagates errors:

```fides
fn read_file(path: &str) -> Result<str, FileError> {
    val file = open(path)?;        // if Err, return Err immediately
    val data = read(file)?;
    Ok(data)
}
```

**How it works (IR level):**
1. Lower the inner expression → Result struct value
2. Load the tag field (field 0)
3. If tag == 0 (Ok): continue with the value field (field 1)
4. If tag == 1 (Err):
   - If inside a `try` block: copy error to the try context's error slot,
     jump to the error handler block
   - Otherwise: emit deferred blocks, return the error from the current
     function

### 6.3 Try Blocks

```fides
val result = try {
    val a = parse_int(s1)?;
    val b = parse_int(s2)?;
    a + b
};
```

A `try` block pushes an error target onto the `try_stack`. Any `?` inside
jumps to the error handler on failure. On success, the last expression is
wrapped as `Ok`.

### 6.4 Defer

```fides
fn process_file(path: &str) -> Result<(), FileError> {
    val file = open(path)?;
    defer close(file);          // runs when function exits (even on error)
    val data = read(file)?;
    Ok(())
    // close(file) runs here automatically
}
```

`defer` blocks are pushed onto a stack and emitted in LIFO order at:
- Function return (explicit or implicit)
- Error propagation from `?`
- Function epilogue

### 6.5 Match

```fides
match option {
    Some(value) => println(f"Got {value}"),
    None => println("Nothing"),
    _ => println("Other"),
}
```

**Match lowering:**
- Scrutinee is lowered once to a value
- Each arm generates a compare block and a body block
- `IntLit`/`BoolLit` → `BinOp::Eq` compare
- `StrLit` → `p_str_equal` call
- `Range` → `<=` and `<` compares
- `EnumVariant` → load tag field 0, compare to variant index
- Bindings → `GetFieldPtr`/`Load` from payload fields
- Guards → extra `Branch` after pattern match
- All arms merge to a single merge block

### 6.6 Loops

```fides
// For-in (iterator based)
for x in nums { ... }
for i in 0..10 { ... }
for i in 0..=10 { ... }

// While
while condition { ... }

// Infinite loop (with break-value)
val result = loop {
    if done { break value; }
};
```

`break` can carry a value (the loop expression evaluates to the break value).

---

## 7. Intermediate Representations

Fides uses three levels of intermediate representation between the AST and
native code:

### 7.1 AST (Abstract Syntax Tree)

The parser produces a `Program` containing:
- `funcs: Vec<Func>` — function definitions
- `structs: Vec<StructDef>` — struct types
- `enums: Vec<EnumDef>` — enum types
- `impls: Vec<ImplDef>` — impl blocks
- `traits: Vec<TraitDef>` — trait definitions
- `trait_impls: Vec<TraitImpl>` — trait implementations
- `consts: Vec<ConstDef>` — constants
- `type_aliases: Vec<TypeAlias>` — type aliases
- `imports: Vec<ImportDef>` — imports
- `extern_decls: Vec<ExternDecl>` — FFI declarations

**Key AST types:**
- `Type` — all type expressions (primitives, generics, refs, arrays, etc.)
- `Expr` — all expression forms (literals, calls, match, closures, etc.)
- `Stmt` — all statement forms (let, assign, if, while, for, defer, etc.)
- `Pattern` — match patterns (literals, bindings, enum variants, ranges)

### 7.2 IR (SSA Intermediate Representation)

The lowerer converts AST to typed SSA IR:

**IrType:**
```
I8/I16/I32/I64, U8/U16/U32/U64, F32/F64, Bool
Ptr(Box<IrType>), Struct(str), Array(Box<IrType>, i64)
Void, Never, Opaque(str), Generic(str, Vec<IrType>)
```

**Key instructions:**
- Constants: `ConstInt`, `ConstFloat(f64)`, `ConstBool`, `ConstStr`, `ConstNull`
- Arithmetic: `BinOp`, `UnaryOp`
- Memory: `Alloca(IrType, Region)`, `Load`, `Store`, `GetFieldPtr`, `GetElemPtr`
- Control flow: `Jump`, `Branch`, `Return`, `Unreachable`
- Calls: `Call`, `CallPtr` (closures/fn pointers), `DynCall` (trait objects), `MakeDyn`
- Conversions: `BitCast`, `IntExtend`, `IntTrunc`, `IntToFloat`, `FloatToInt`
- Fides-specific: `TryUnwrap`, `Print`, `Panic`, `Asm`
- Regions: `RegionBegin`, `RegionEnd`
- Persistence: `PersistLoad`, `PersistSave`
- Transactions: `TxnBegin`, `TxnEnd`
- Unsafe: `UnsafeBegin`, `UnsafeEnd`
- Autograd: `GradBegin`, `GradEnd`, `Backward`

**IR structure:**
- `IrModule` — collection of functions, struct layouts, extern declarations
- `IrFunction` — SSA function with basic blocks, value table, instruction map
- `BasicBlock` — list of instructions + predecessors + phi params
- `ValueId` / `BlockId` — typed handles for values and blocks
- `ValueMeta` — type, region, effects, source line, borrow kind per value

### 7.3 PIR (Portable IR — L1/L2/L3)

PIR is a three-level lowering pipeline:

**L1 (IR + effects/caps/regions):**
- Adds `EffectSet` and `CapSet` to each instruction
- Adds `RegionRef` for named region tracking
- `NewRegion`/`FreeRegion` for region lifecycle

**L2 (SSA + memory regions):**
- Drops region handles, uses `MemRegion` (Stack/Heap/Static/Named)
- Keeps SSA form with virtual value IDs
- This is the level the optimizer works on

**L3 (Virtual registers + stack slots):**
- Lowers SSA values to `VReg` (virtual registers) and `Slot` (stack slots)
- `Mov`, `Bin`, `Unary`, `Load`, `Store`, `FieldAddr`, `ElemAddr`
- `Jmp`, `Br`, `Ret`, `Call`, `CallPtr`, `DynCall`
- `Cast` with `CastKind` (BitCast/IntExtend/IntTrunc/IntToFloat/FloatToInt)
- This is the final representation consumed by native code generators

### 7.4 Closures

Closures use **lambda lifting** with environment capture:

1. **Find captures:** Free variables in the closure body (excluding params)
2. **Lift:** Create a top-level function `__closure_N` with `__env` as first
   parameter
3. **Environment:** At creation site, `p_malloc` an env struct:
   - Field 0: function name string (for dispatch)
   - Fields 1..N: captured values
4. **Call:** `CallPtr` with `has_env=true` — the closure pointer is passed
   as the first argument

In C codegen, `CallPtr` generates an inline `if/else` chain with `strcmp`
over candidate function names to dispatch to the right closure function.

### 7.5 Enum Representation

Enums are compiled to structs with a tag + payload:

```
Enum Foo { Bar(i64), Baz(str), Qux }

→ struct Foo_s {
    i64 tag;        // 0=Bar, 1=Baz, 2=Qux
    // payload fields (union of all variant data)
    i64 field_1;    // i64 for Bar, unused for Baz/Qux
    // str field_2;  // str for Baz (same offset, 8 bytes)
}
```

- Tag is always field 0
- Payload fields start at field 1
- Enum size = (1 + max_payload_fields) * 8 bytes
- `match` loads tag and compares to variant index

---

## 8. Backends & Code Generation

### 8.1 Interpreter

The interpreter is the reference oracle (~4400 LOC). It directly executes
the SSA IR:
- Walks basic blocks, evaluates each instruction
- Maintains a value table per function
- `Call`/`CallPtr` dispatch to `p_*` builtins or user functions
- Supports recursion depth limiting
- Auto-detects missing `__env` param and inserts dummy

**Value representation:**
```
Value::Int(i64), Float(f64), Bool, Str, Ptr(u64), Void
Vec(Rc<RefCell<SmallVec<[Value; 8]>>>)
Map(Rc<RefCell<HashMap<String, Value>>>)
Dyn(String, u64)     // trait object (type_name, data_ptr)
Tensor, Json, Future, ThreadHandle
```

### 8.2 C Codegen

Generates C99 source code from the IR:
- Each IR function → C function
- Each SSA value → `vN` temporary variable
- Basic blocks → labeled sections with `goto`
- Structs → `struct Name_s { ... }`
- Strings → static `char[]` constants
- Runtime functions (`p_*`) linked to bundled C runtime
- Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack
- Stack traces on panic

**C type mapping:**
```
IrType::I8  → int8_t      IrType::U8  → uint8_t
IrType::I64 → int64_t     IrType::U64 → uint64_t
IrType::F32 → float       IrType::F64 → double
IrType::Bool → bool       IrType::Ptr(_) → void*
IrType::Struct(n) → struct n_s*
IrType::Array(e, n) → {ctype}[{n}]
```

### 8.3 Native Codegen (x86-64/ARM64/RISC-V)

Direct machine code emission — no assembler, no LLVM:
- PIR L3 → machine code (manual byte encoding)
- ModRM/SIB bytes, REX prefixes, ABI rules
- Linear scan register allocation with constant rematerialization
- Peephole optimizer (9 passes, always on)
- ELF64 (Linux), PE (Windows), Mach-O (macOS) object file formats
- Custom linker for raw linking with runtime

### 8.4 Runtime Library

The runtime (`runtime_linux.rs`, etc.) is hand-encoded x86-64 machine code:
- Memory: `p_malloc`, `p_free`, `p_region_free`
- Strings: `p_str_concat`, `p_str_equal`, `p_to_str`, `p_float_to_str`
- Collections: `p_vec_new/push/pop/len/get/set`, `p_map_*`
- I/O: `p_print_int/float/bool/str/newline`, `p_panic`
- Threads: `p_thread_create/join`, `p_mutex_*`, `p_atomic_*`
- Networking: `p_socket_*`, `p_http_*`
- ML: `p_tensor_*`, `p_autograd_backward`, `p_quantize`
- Privacy: `p_dp_budget_*`, `p_tp_*`

All builtins are prefixed with `p_` to avoid C standard library collisions.

### Inline Assembly

Fides supports inline assembly with input/output operands and clobber lists:

```
// Basic: zero-operand instructions
asm!("nop");
asm!("cli; hlt");

// Input operands: load value into register
val x = 42;
asm!("mov rax, rcx", in("rax") x);

// Output operands: store register to variable
val result = asm!("nop", out("rax") x);

// Clobber lists: tell compiler which registers are destroyed
asm!("call rax", in("rax") fn_ptr, in("rcx") arg, clobber("rdx"), clobber("r8"));

// Port I/O (kernel/UEFI)
asm!("out dx, al", in("dx") port, in("al") val);
```

**Syntax:** `asm!(template, operand1, operand2, ...)`
- **template**: string of x86 instructions separated by `;` or newlines
- **in("reg") expr**: load `expr` into register `reg` before executing
- **out("reg") expr**: store register `reg` to `expr` after executing
- **clobber("reg")**: save/restore register `reg` around the asm block

The PE backend encodes 50+ x86 instructions directly to machine code,
including: nop, hlt, cli, sti, mov, xor, add, sub, and, or, push, pop,
inc, dec, not, neg, shl, shr, sar, in, out, syscall, sysret, swapgs,
rdmsr, wrmsr, rdtsc, cpuid, iret, iretq, lgdt, lidt, invlpg, call, jmp,
ret, mfence, sfence, lfence, pause, wbinvd, int, leave, cld, std, ud2.

### Bare-Metal / No-Std Mode

```
# Compile with zero stdlib dependencies
fides-c build kernel.fd --bare
fides-c build kernel.fd --no-std
fides-c build kernel.fd --no-stdlib
```

In `--bare`/`--no-std` mode, the compiler:
1. Skips loading `stdlib/core.fd` entirely
2. Typechecks the user program alone (no stdlib merge)
3. Skips import resolution (no stdlib to import from)
4. Produces output with zero hidden runtime dependencies

This is for kernel/UEFI/bare-metal development where the program must
not depend on any stdlib functions.

### `@cpu_local` Variables (Per-CPU Data)

For SMP kernel development, `@cpu_local` declares per-CPU storage
accessed via the GS segment register:

```
@cpu_local var current_task: i64 = 0;
@cpu_local var cpu_stack: *mut u8 = 0;
@cpu_local var cpu_id: i32 = 0;

fn schedule() {
    // Access uses GS-relative addressing: lea rax, [gs:offset]
    current_task = pick_next_task();
}
```

The GS segment base is set per-CPU via `WRMSR(0xC0000101, base)`.
Each cpu_local variable gets a unique offset starting at 0x1000
(0x0000-0x0FFF reserved for per-CPU control block).

### UEFI Boot Services

`stdlib/uefi.fd` provides FFI bindings for UEFI boot services:

```
use uefi;

fn efi_main(image_handle: i64, system_table: i64) -> i64 {
    uefi_init(image_handle, system_table);
    uefi_print("Hello from UEFI!\n");
    var buf = uefi_allocate_pool(uefi_pool_data, 4096);
    uefi_clear_screen();
    uefi_stall(1000000);  // 1 second
    return EFI_SUCCESS;
}
```

Available functions: `uefi_init`, `uefi_get_boot_services`,
`uefi_get_runtime_services`, `uefi_allocate_pool`, `uefi_free_pool`,
`uefi_allocate_pages`, `uefi_get_memory_map`, `uefi_exit_boot_services`,
`uefi_stall`, `uefi_print`, `uefi_clear_screen`.

### Cross-Compilation Targets

```
fides-c build app.fd --target x86-64        # Native (default)
fides-c build app.fd --target arm64         # ARM64 (C codegen fallback)
fides-c build app.fd --target aarch64       # AArch64 (C codegen fallback)
fides-c build app.fd --target riscv64       # RISC-V 64 (C codegen fallback)
```

The self-hosted PE emitter currently supports x86-64 only. ARM64 and
RISC-V targets fall back to C codegen (the IR is target-independent).
Native ARM64/RISC-V codegen modules for the self-hosted compiler are
planned but not yet implemented.

---

## 9. Standard Library

Fides has 99 stdlib modules (~28,370 LOC). Key modules:

### Core
| Module | Description |
|--------|-------------|
| `core` | Built-in types, Option, Result, range, Box |
| `vec` | Dynamic array (Vec<T>) |
| `string` | String operations |
| `hash` | Hashing (fnv1a, siphash) |
| `collections` | HashMap, HashSet, BTreeMap, BTreeSet, Deque |
| `sort` | Sorting algorithms |
| `iter` | Iterator adapters (eager Vec-based) |
| `stack` / `queue` | Stack and queue data structures |
| `buffer` | Byte buffer |
| `result` | Result type helpers |
| `fmt` | String formatting |
| `math` | Math functions |
| `bits` | Bit manipulation |
| `random` | Random number generation |

### I/O & System
| Module | Description |
|--------|-------------|
| `fs` / `file` / `io` | File system, file I/O, stream I/O |
| `os` / `sys` / `env` / `args` / `process` | OS interface, environment, CLI args |
| `path` | Path manipulation |
| `time` | Time and timers |
| `channel` | Channel-based messaging |
| `sync` | Mutex, atomics, thread-local storage |
| `parallel` | Parallel computation |

### Networking & Data
| Module | Description |
|--------|-------------|
| `http` | HTTP client/server |
| `db` | Database interface |
| `json` | JSON parsing/serialization |
| `regex` | Regular expressions |
| `uuid` | UUID generation |
| `crypto` | Cryptographic primitives |

### Domain-Specific
| Module | Description |
|--------|-------------|
| `money` / `banking` | Financial types and operations |
| `clinical` / `medical` | Healthcare data standards |
| `ecu` / `scada` / `swift` / `arinc` / `can` | Industrial/automotive/aviation protocols |
| `grid` / `ota` | Grid computing, over-the-air updates |

### OS / Bare-Metal
| Module | Description |
|--------|-------------|
| `kernel` | OS kernel primitives (scheduler, memory, interrupts) |
| `volatile` | Volatile memory access (p_volatile_load/store) |
| `uefi` | UEFI boot services bindings (AllocatePool, GetMemoryMap, ExitBootServices, console) |

### ML & AI
| Module | Description |
|--------|-------------|
| `tensor` | Tensor operations |
| `simd` | SIMD intrinsics |
| `shape` | Shape-level type safety |
| `kv_cache` | KV cache for LLM inference |
| `quant` | Quantization |
| `dp` | Differential privacy |
| `shard` | Tensor sharding/parallelism |
| `privacy` | Privacy primitives |

### Game & Graphics
| Module | Description |
|--------|-------------|
| `ecs` / `events` | Entity-component-system |
| `tilemap` / `sprite` | 2D game primitives |
| `flowfield` / `scene` / `physics` | Game physics and scenes |
| `audio` / `input` | Audio and input handling |
| `streaming` | Streaming content |

### Utilities
| Module | Description |
|--------|-------------|
| `memo` | Memoization |
| `units` | Unit conversions |
| `typed_strings` | Type-safe string newtypes |
| `alloc` | Custom allocators |
| `test` | Testing framework |

---

## 10. Self-Hosting

**The Fides compiler is self-hosted.** The compiler has been rewritten from
Rust (`src/*.rs`, ~91K LOC) into Fides (`compiler/*.fd`, ~61K LOC) and
achieves a **bootstrap fixpoint**: the self-hosted compiler reproduces
byte-identical output across generations.

### Current State (session 15)

- **25 modules** in `compiler/`:
  `token`, `diag`, `json`, `fmt`, `ast`, `lexer`, `parser`, `ir`,
  `typechecker`, `lower`, `mono`, `borrowck`, `pir`, `opt`, `codegen_c`,
  `compiler`, `comptime_eval`, `flat_ast`, `lsp`, `pe`, `peephole`,
  `regalloc`, `codegen_x64`, `main`, `inc`
- **23/23 self-host tests pass** on all 3 backends (interpreter, C codegen,
  PE emitter)
- **Bootstrap fixpoint achieved** (session 15):
  - Rust → c1 (706K lines) → c2 (701K lines) → c3 (701K lines)
  - **fides-c2.c == fides-c3.c** (SHA256-identical, 0 diffs / 701,100 lines)
  - Three generations verified, all run test_simple.fd correctly

### Bootstrap Strategy

The bootstrap uses the C codegen path with TCC:
1. Rust compiler (`fidesc`) compiles `compiler/*.fd` → C source (706K lines)
2. C source compiled with TCC → `fides-c.exe` (first-generation)
3. `fides-c.exe` compiles `compiler/*.fd` → C source (701K lines)
4. C source compiled with TCC → `fides-c2.exe` (second-generation)
5. `fides-c2.exe` compiles `compiler/*.fd` → C source (701K lines)
6. **Step 5 output == Step 3 output** → bootstrap fixpoint verified

A 256MB stack wrapper is required because TCC's default 4KB PE stack
reserve is insufficient for the compiler's deep recursion.

### Key Adaptations (Rust → Fides)

| Rust Feature | Fides Adaptation |
|--------------|------------------|
| `enum` with struct variants | Tuple-style variants (`Type::Array(Box<Type>, i64)`) |
| `Option<T>` | Custom `Opt*` enums (`OptBoxExpr`, `OptStmts`, etc.) |
| `Result<T, E>` | Custom `ParseResult*` enums |
| `String` / `usize` / `u64` | `str` / `i64` |
| `format!` macro | String concatenation with `+` and `int_to_str` |
| `HashMap<K,V>` | `HashMap` via `collections.fd` |
| `HashSet<T>` | `HashSet` in `collections.fd` |
| Lazy iterators | Eager Vec-based adapters |
| `mem::replace`/`swap` | Manual temp variables |
| `derive(Debug, Clone)` | `@derive(Debug, Clone)` |
| `?` operator | Explicit `match` propagation |
| `==` on enums | Manual `*_eq()` helper functions (tag-based comparison) |
| `==` on strings | Lowered to `p_str_equal` by the compiler automatically |

### Remaining Work (post-fixpoint)

- **Rust vs self-hosted parity gap**: c1=706K vs c2=701K lines (~5K difference)
- **Debug prints**: 287 `p_eprint_str` calls (go to stderr, don't affect fixpoint)
- **Mirror `var` feature to `compiler/*.fd`**: Rust supports module-level `var`/`pub var`
- **PE emitter full compiler support**: struct/enum, floats, many-arg calls
- **Linux native codegen integration**: port `elf64.rs`, `linker.rs`, `runtime_linux.rs`

---

*For syntax details, see [syntax.md](syntax.md). For compiler architecture,
see [architecture.md](architecture.md). For self-hosting details, see
[selfhost.md](selfhost.md). For feature status, see
[features.md](features.md).*
