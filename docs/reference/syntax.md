# Fides Language Syntax Specification

**Version:** 0.5.0
**Status (Nov 6, 2026):** self-hosted compiler `fid` with bootstrap fixpoint. Backends: interpreter (`fid run`), C (`fid ccompile`), native Windows x64 (`fid emit`); Linux x64 through C. ARM64, RISC-V, WASM, macOS, native ELF and bare metal are experimental / incomplete.

> **Implementation Status:** This document describes the language design, with implementation status marked in places (✅ working / ⚠️ partial / ❌ parsed-only or planned). The language is what the self-hosted compiler `fid` accepts; [reference.md](reference.md) describes it rule by rule from the compiler source and is right where this document disagrees. See `features.md` for the feature matrix and `architecture.md` for the compiler pipeline.
>
> **Self-hosting status (Nov 6, 2026):** The Fides compiler is self-hosted: 61 modules (133,688 lines), bootstrap fixpoint (stage 2 C == stage 3 C, 49 MB) checked on every release build. (First reached in session 15 with 25 modules.) Counts: [CURRENT.md → Numbers](../../CURRENT.md#numbers).
>
> **Tier 3 OS features (session 16):** inline asm clobber lists, `--no-std` bare-metal mode, cross-compilation target dispatch. `stdlib/uefi.fd` and `stdlib/kernel.fd` moved to the fides-os project; bare metal is incomplete (`emit --bare` panics).

## 1. Design Philosophy

Fides is a systems programming language designed for safety, clarity, and modern AI-assisted development. It borrows proven concepts from Rust while introducing several innovations that address Rust's pain points.

### 1.1 Core Principles

1. **Safe by default** — Bounds checks, ownership, and capabilities are enforced by default. The `unsafe { }` block exists for opting out of bounds checks (e.g. inside stdlib `core.fd`), but ordinary user code is safe without explicit annotation.
2. **No hidden control flow** — No exceptions. All effects (IO, async, errors) are explicit in function signatures.
3. **No macros** — Compile-time code execution via `comptime` blocks. String interpolation via `f"..."`. No macro system at all.
4. **No lifetime annotations** — References are second-class (they cannot be stored or returned from locals), so no lifetimes are written. `region r { }` is a scope with an optional size budget.
5. **Capabilities and effects** — `use m with { IO, FS }` grants capabilities, checked per call by name; inferred effects (`!IO`, `!{}`) are checked through the call graph.
6. **AI-friendly syntax** — Syntax choices prioritize AI model comprehension. Familiar patterns from Rust, Python, Swift.
7. **One keyword per concept** — `fn` for all functions (named, anonymous, async, comptime). `val` for all variables. `const` for constants. `cap` for traits/capabilities.

### 1.2 What Fides Takes From Rust

- Ownership and borrowing (`&` / `&mut`)
- Algebraic enums and pattern matching (`match` / `=>`)
- Traits and impl blocks
- `Result<T, E>` + `?` operator
- `@annotation` syntax (`#[...]` is parsed and ignored)
- `pub` visibility
- `<T>` generics with trait bounds
- `for x in iter` loops
- `..` and `..=` ranges
- `str` (owned) / `&str` (borrowed) string model
- `[T; N]` / `Vec<T>` collection model

### 1.3 What Fides Does Differently

| Feature | Rust | Fides |
|---------|------|--------|
| Function keyword | `fn` | `fn` |
| Variable keyword | `let` / `let mut` | `val` / `var` |
| Constant keyword | `const` | `const` |
| Trait keyword | `trait` | `cap` |
| Return keyword | `return` | `return` |
| Type alias keyword | `type` | `type` |
| Closures | `\|x\| x * 2` | `fn(x) { x * 2 }` or `fn(x) => x * 2` |
| String formatting | `println!("{} + {}", a, b)` | `println(f"{a} + {b}")` |
| Lifetimes | `'a`, `'b` explicit | None: references cannot be stored or escape |
| Macros | `macro_rules!`, proc macros | `comptime` blocks (no macros) |
| Unsafe code | `unsafe { }` blocks | `unsafe { }` blocks (skip bounds checks — used in stdlib `core.fd`) |
| Error handling | `Result<T, E>` only | `Result<T, E>` + effect handlers (`!Effect`) |
| Async | `async fn` / `.await` | `async fn` / prefix `await e` |
| Compile-time code | Procedural macros | `comptime { }` blocks |
| Hot reload | Not supported | Rebuild and rerun on save (`fidesc watch`) |
| Capability security | Not built-in | `use … with { … }`, `sandbox { }` |

---

## 2. Lexical Structure

### 2.1 Keywords

```
fn          val          var          const        mut         pub
struct      enum         cap          impl         use         match
type        if           else         for          while       loop
return      break        continue     async        await
comptime    in           as           where        region
spawn       defer        move         static       try
handle      do           interrupt    extern       dyn         module
with        unsafe       sandbox      txn          persist
given       ensure       guard        is           select
layer       model        grad         backward     kernel
component   system       query        state        on
true        false
```

**Note:** `none`, `some`, `ok`, `err` are NOT keywords — they are enum variant names from the standard library (`Option` and `Result`). They are listed here for reference only and are not in the keyword block above.

**Keyword reference** (from `compiler/lexer.fd`; the full reserved list, including `yield`, `resume` and the type names, is in reference.md §1.4):

| Keyword | Purpose |
|---------|---------|
| `fn` | Function declaration (named, anonymous, async, comptime) |
| `val` | Immutable variable declaration |
| `var` | Mutable variable declaration |
| `const` | Compile-time constant |
| `cap` | Capability / trait declaration |
| `type` | Type alias |
| `return` | Return statement |
| `region` | Region block for memory lifetime tracking |
| `handle` / `do` / `with` | Effect handlers |
| `extern` | FFI declarations (`extern "C"`) |
| `dyn` | Trait object (dynamic dispatch) |
| `module` | Module declaration |
| `with` | Capability grant in imports (`use ... with { IO, FS, Net, Spawn }`) |
| `interrupt` | Interrupt handler (bare-metal) |
| `comptime` | Compile-time code execution |
| `defer` | Deferred cleanup (Go-style) |
| `spawn` | Spawn concurrent task (✅ implemented — OS thread model) |
| `async` / `await` | Async functions (✅ green tasks on their own stacks; `await e` is prefix; native async crashes) |
| `try` | Try blocks for error handling |
| `unsafe` | Unsafe block (skip bounds checks — framework/stdlib use only) |
| `sandbox` | Sandbox block (restricted execution context) |
| `txn` | Transaction block: `fid run` snapshots memory at entry; C and native run the body with no rollback |
| `persist` | Keeps a variable across hot reloads; a plain `val`/`var` otherwise |
| `given` / `ensure` | Contracts: `given` is checked at calls with constant arguments; `ensure` is parsed and not checked |
| `guard` | Guard clause |
| `is` | Type check operator |
| `select` | Channel select statement |
| `layer` / `model` / `grad` / `backward` | ML/deep learning primitives |
| `kernel` / `component` / `system` / `query` / `state` / `on` | Domain-specific declarations |
| `asm!` | Inline assembly with operands and clobbers (✅ implemented — PE backend, 50+ x86 instructions) |
| `@cpu_local` | Accepted by the name check; no pass reads it in `fid` |

### 2.2 Operators

```
Arithmetic:  +  -  *  /  %
Comparison:  ==  !=  <  >  <=  >=
Logical:     &&  ||  !
Bitwise:     &  |  ^  ~  <<  >>
Assignment:  =  +=  -=  *=  /=  %=  &=  |=  ^=  <<=  >>=
Range:       ..  ..=
Arrow:       ->  =>
Reference:   &  &mut
Dereference: *
Question:    ?
Namespace:   ::
```

### 2.3 Comments

```fides
// Line comment

/* Block comment */

/// Documentation comment (for items)
//! Module-level documentation comment
```

### 2.4 Identifiers

- Functions, variables, modules: `snake_case`
- Types, structs, enums, caps (traits): `PascalCase`
- Constants: `SCREAMING_SNAKE_CASE`

### 2.5 Literals

```fides
42          // integer; `val x = 42` is an i64
3.14        // f64
true        // bool
false       // bool
"hello"     // str
r"C:\dir"   // raw string
f"hello"    // interpolated string (f-string)
'c'         // integer: the character's Unicode scalar value (99)
[1, 2, 3]   // array literal (elements are i64)
0xFF 0b1010 0o17 1_000_000
```

There are no literal suffixes (`42i64` is `42` followed by the keyword `i64`,
a parse error; write `42 as i64` or annotate the variable), no byte-string
literals (`b"…"`), and no `char` type. A float literal needs digits on both
sides of the `.`; `1e5` is not a float.

### 2.6 F-String Interpolation

F-strings are a first-class language feature (not a macro). Any expression can be embedded in `{}`:

```fides
val name = "World";
val x = 5;
val y = 10;

f"Hello, {name}!"                    // "Hello, World!"
f"{x} + {y} = {x + y}"              // "5 + 10 = 15"
f"Point: ({p.x}, {p.y})"            // field access
f"Length: {nums.len()}"             // method call
```

There are no format specifiers: the parser reads one expression from the
braces and ignores the tokens after it (`{x:#x}` is `{x}`). There is no escape
for a literal brace (`{{` is not one).

---

## 3. Types

### 3.1 Primitive Types

```
Integers:   i8  i16  i32  i64  i128
            u8  u16  u32  u64  u128
Floats:     f32  f64
Boolean:    bool
String:     str (owned)  &str (borrowed)
Unit:       void, () (empty tuple)
Never:      ! (diverges / never returns)
```

There is no `char` (a character literal is an integer), `isize`, `usize` or
slice type. `str` is owned text; `&str` is a borrowed string that can be read,
compared, printed and joined with `+` but not returned as `str` or stored
(`test_borrowed_str.fd`). `str_clone(s)` makes an owned copy.

### 3.2 Compound Types

```fides
// Array — fixed size, stack allocated
[T; N]          // type
[1, 2, 3]       // literal

// Vector — growable, heap allocated
Vec<T>          // type
Vec::new()      // empty; add elements with push (there is no vec![…])

// Tuple
(T1, T2, T3)    // type
(a, b, c)       // literal

// Function type
fn(i32) -> i32      // type signature (the -> is required)
```

### 3.3 Custom Types

```fides
// Struct — product type
struct Point {
    x: f64,
    y: f64,
}

// Enum — sum type (algebraic)
enum Shape {
    Circle(f64),
    Rectangle(width: f64, height: f64),   // named payload
    Triangle(f64, f64, f64),
    Point,                                // unit variant
}

// Generic type
struct Pair<K, V> {
    key: K,
    value: V,
}

// type alias
type Score = i32;
```

There are no tuple structs (`struct Color(u8, u8, u8)`), no unit structs
(`struct Empty;`), no struct-like enum variants (`Circle { radius: f64 }`) and
no explicit discriminants.

### 3.4 Standard Library Types

```fides
Option<T>        // enum Option<T> { Some(T), None }
Result<T, E>     // enum Result<T, E> { Ok(T), Err(E) }
Vec<T>           // growable array
HashMap<K, V>    // hash map (str or integer keys)
Box<T>           // heap indirection (enables recursive types)
Rc<T>            // reference-counted smart pointer (manual refcount)
Arc<T>           // atomic reference-counted smart pointer (manual refcount)
```

**Note:** `Option<T>` and `Result<T, E>` are generic enums in
`stdlib/core.fd`. `fid` instantiates their methods for each `T` a program uses
(`Option<f64>`'s `unwrap` returns an `f64`). On an `Option` written without
its type argument, `T` is `i64`.

**Box<T>** enables recursive types via heap indirection. `enum Expr { Add(Box<Expr>, Box<Expr>) }`
compiles and runs correctly. `Box::new(x)` allocates on the heap and returns a pointer.

**Rc<T>** and **Arc<T>** are reference-counted smart pointers with manual refcount
management (Fides lacks automatic destructors). Memory layout: `[refcount: i64][value: T]`.
The API is explicit:
- `Rc::new(x)` / `Arc::new(x)` — allocate, set refcount=1, return pointer to value
- `rc.clone()` / `arc.clone()` — increment refcount, return same pointer
- `rc.drop()` / `arc.drop()` — decrement refcount, free if zero
- `rc.get()` / `arc.get()` — load the contained value
- `rc.count()` / `arc.count()` — read the current refcount

### 3.4a Standard Library Modules

Fides's stdlib lives in `stdlib/` and is imported explicitly via `use` (except `core.fd`, which is auto-included):

| Module | File | Contents |
|--------|------|----------|
| `core` | `stdlib/core.fd` | Auto-included: `Option<T>`, `Result<T, E>`, `println`, `print`, `assert`, `assert_eq`, `max`, `min`, `abs`, `clamp` |
| `math` | `stdlib/math.fd` | `math_sqrt`, `math_pow`, `math_floor`, `math_ceil`, `math_sin`, `math_cos`, `math_fabs`, `math_pi`, `math_e`. Vec2/3/4, mat4, quat types with operator overloading + swizzle access |
| `env` | `stdlib/env.fd` | `env_get`, `env_exists` |
| `args` | `stdlib/args.fd` | `args_len`, `args_get`, `args_program_name` |
| `process` | `stdlib/process.fd` | `process_exit`, `process_system` |
| `regex` | `stdlib/regex.fd` | `regex_matches` (?/* wildcards), `regex_find`, `regex_contains`, `regex_starts_with`, `regex_ends_with` |
| `json` | `stdlib/json.fd` | `parse`, `stringify`, `get`, `at` |
| `tensor` | `stdlib/tensor.fd` | n-d arrays, `matmul`, `relu`, `softmax` |
| `parallel` | `stdlib/parallel.fd` | `par_map`, `par_sum`, `par_dot` |
| `db` | `stdlib/db.fd` | `cap Database` trait, in-memory KV store |
| `http` | `stdlib/http.fd` | socket + HTTP parsing |
| `simd` | `stdlib/simd.fd` | elementwise ops, `dot`, `sum`, `scale` (f64x2, f64x4, etc.) |
| `shape` | `stdlib/shape.fd` | shape system for tensor ops |
| `quant` | `stdlib/quant.fd` | quantization: `q8`, `q4`, `quantize_q8`, `dequantize_q8`, `qmatmul` |
| `money` | `stdlib/money.fd` | fixed-point i64 cents, `@linear` struct, arithmetic, formatting |
| `banking` | `stdlib/banking.fd` | account, deposit, withdraw, transfer, interest |
| `alloc` | `stdlib/alloc.fd` | heap allocator builtins (`malloc`, `realloc`, `free`) |
| `file` | `stdlib/file.fd` | file I/O: read, write, append, exists |
| `time` | `stdlib/time.fd` | `LogicalTime`: timezone-free, `@deterministic` date math |
| `typed_strings` | `stdlib/typed_strings.fd` | `SQLString`, `ShellString`, `HTMLString`, `JSONString`, `PathString` |
| `memo` | `stdlib/memo.fd` | `Memo::new`, `get_or_compute` |
| `privacy` | `stdlib/privacy.fd` | field-level privacy test module |

> **Note:** `Box<T>`, `Rc<T>`, and `Arc<T>` are implemented in the compiler (not as stdlib modules — they are builtin types with compiler support). `Box<T>` enables recursive types via heap indirection. `Rc<T>` / `Arc<T>` provide manual reference counting (`new`, `clone`, `drop`, `get`, `count`). `HashSet<T>` and `Mutex<T>` are not yet implemented. Channels are available via `stdlib/channel.fd`. Heap allocation uses `p_malloc`/`p_free` directly or via `Vec`/`String`/`HashMap`.

### 3.5 Built-in Type Methods

#### String Functions

`str` has no methods; core provides functions (reference.md §13.4):

```fides
val s: str = "Hello";

str_len(s) -> i64                       // length in bytes
str_slice(s, start, end) -> str         // substring
str_contains(s, needle) -> bool
str_starts_with(s, prefix) -> bool
str_ends_with(s, suffix) -> bool
str_trim(s) -> str
str_split(s, delim) -> Vec<str>
str_replace(s, old, new) -> str
str_to_upper(s) -> str
str_to_lower(s) -> str
str_parse_int(s) -> i64
str_find(s, needle) -> i64              // index or -1
str_rfind(s, needle) -> i64
str_repeat(s, n) -> str
str_byte_at(s, i) -> i64
str_clone(s) -> str                     // owned copy
```

`StringBuilder` (`StringBuilder::new()`, `append_str`, `to_str`) builds strings.

#### Vec Methods

```fides
val v: Vec<i64> = Vec::new();

v.push(val: i64)            // append element
v.pop() -> i64              // remove and return last
v.len() -> i64              // element count
v.is_empty() -> bool        // true if len == 0
v.get(idx: i64) -> i64      // element at index
v.set(idx: i64, val: i64)   // set element at index
v.clear()                   // remove all elements
v.capacity() -> i64         // allocated capacity
v.insert(idx: i64, val: i64)  // insert at index
v.remove(idx: i64) -> i64     // remove and return at index
v.contains(val: i64) -> bool  // linear search
v.reverse()                 // reverse in place
v.first() -> i64            // first element
v.last() -> i64             // last element
v.extend(other: Vec<i64>)   // append all from other
v.truncate(len: i64)        // shorten to len
v.sort()                    // sort in place
v.clone() -> Vec<i64>       // deep copy
```

#### HashMap Methods

```fides
val m: HashMap<str, i64> = HashMap::new();

m.insert(key: str, val: i64)     // insert/update
m.get(key: str) -> i64           // lookup (0 if missing)
m.get_or(key: str, default: i64) -> i64  // lookup with default
m.contains(key: str) -> bool     // key exists
m.remove(key: str)               // delete by key
m.len() -> i64                   // entry count
m.is_empty() -> bool             // true if empty
m.clear()                        // remove all
m.keys() -> Vec<str>             // all keys
m.values() -> Vec<i64>           // all values
m.clone() -> HashMap<str, i64>   // deep copy
```

Keys may also be integers (`HashMap<i64, V>`, `test_hashmap_int_keys.fd`).
0 and -1 are ordinary keys; the two integers `0x5DEECE66D1234567` and its
complement are reserved and panic.

---

## 4. Functions

### 4.1 Named Functions

```fides
// Basic function
fn add(x: i32, y: i32) -> i32 {
    x + y
}

// Public function
pub fn main() {
    println("Hello, Fides!");
}

// Function with no return value (unit)
fn greet(name: &str) {
    println(f"Hello, {name}!");
}

// Generic function
fn identity<T>(x: T) -> T {
    x
}

// Generic with cap bounds (each bound is checked at the call)
fn show_twice<T: Show + Clone>(a: T) -> str {
    return a.show() + a.show();
}

// Default and named arguments
fn add(a: i64, b: i64 = 10) -> i64 { return a + b; }
val s = add(b: 2, a: 1);

// Async function: declared Future<T>, body returns T
async fn compute(x: i64) -> Future<i64> {
    return x * 2;
}
val r = await compute(1);       // await is a prefix operator

// Declared effects
fn read_config(path: str) -> str !{IO} {
    return read_file(path);
}
```

There are no `where` clauses; write bounds in the parameter list
(`<T: A + B>`). There is no postfix `.await`.

### 4.2 Closures

```fides
// Full closure
val double = fn(x: i32) -> i32 {
    x * 2
};

// Short-form closure (single expression, no braces)
val square = fn(x: i32) -> i32 => x * x;

// Closure capturing environment
val multiplier = 3;
val multiply = fn(x: i32) -> i32 => x * multiplier;

// Closure as argument
val doubled = nums.map(fn(x: i64) -> i64 => x * 2);

// Closure with no parameters
val greet = fn() => println("Hello!");
```

A closure copies the variables it names when it is created. There are no
async closures.

### 4.3 Function Pointers

```fides
// Function type
type BinOp = fn(i32, i32) -> i32;

val add: BinOp = fn(a: i32, b: i32) -> i32 => a + b;
val result = add(5, 3);
```

---

## 5. Variables and Bindings

### 5.1 Variable Declaration

```fides
// Immutable (default)
val x = 5;
val name = "Fides";
val point = Point { x: 3.0, y: 4.0 };

// Mutable
var counter = 0;
counter += 1;

// With explicit type
val x: i64 = 42;
var buffer: Vec<u8> = Vec::new();

// Destructuring (tuples only; `val Point { x, y } = p` and
// `val Some(v) = o` do not parse)
val (a, b, c) = tuple;

// Shadowing (allowed, type can change)
val x = 5;
val x = "now a string";  // shadows previous x
```

### 5.2 Constants

```fides
// Compile-time constant
const MAX_PLAYERS: i32 = 64;
const PI: f64 = 3.14159265358979;
const GREETING: str = "Hello, Fides!";

// Global (static is another spelling of var)
var COUNTER: i64 = 0;
```

### 5.3 Copies, Handles and Moves

```fides
// Numbers, bool, structs, enums, tuples, arrays: copied
val a = 5;
val b = a;      // a is still valid

// Vec, HashMap, str: handles. Assignment gives a second name for the same data
val v: Vec<i64> = Vec::new();
var w = v;
w.push(1);      // v.len() is now 1 as well
```

There is no move check for `str` or `Vec`: only `@linear` struct types move
(used exactly once; reference.md §8.7). Copy-on-write for `Vec`, `str` and
`HashMap` is decided (proposal 0001) and written behind a switch that is off,
so today `var b = a` on a `Vec` aliases.

---

## 6. Ownership and Borrowing

### 6.1 References

```fides
// Shared reference (immutable borrow)
fn len(s: &str) -> i64 {
    return str_len(s);
}

// Mutable reference (exclusive borrow)
fn push(buf: &mut Vec<i32>, val: i32) {
    buf.push(val);
}

// Taking a reference
val x = 5;
val r = &x;          // &i32
val val = *r;        // dereference → 5

// Mutable reference
var y = 10;
val r = &mut y;      // &mut i32
*r += 5;           // y is now 15
```

### 6.2 Region Inference

Fides deduces regions (lifetimes) automatically. You rarely need to annotate them.

```fides
// Rust requires explicit lifetime:
//   fn longest<'a>(x: &'a str, y: &'a str) -> &'a str
// Fides: compiler infers the region
fn first(x: &i64, y: &i64) -> &i64 {
    return x;      // allowed: x comes from a reference parameter
}
```

A function may not return a reference to a local or a by-value parameter, and
a reference may not be stored (field, `Vec`, global) or sent to another task
(`compiler/escape.fd`; reference.md §8.4). A `region r { }` block is a scope
with an optional size budget; a reference to a value inside it cannot leave it.

### 6.3 Ownership Rules

1. You can borrow immutably (`&`) with unlimited readers
2. You can borrow mutably (`&mut`) with exactly one writer; changing a value
   while it is borrowed is an error, also a `push` to a `Vec` a `for` loop is
   walking (`test_iter_invalidation_fail.fd`)
3. References do not outlive what they point to (escape check)
4. There is no garbage collector and no general destructor. The compiler frees
   `Vec`/`HashMap` locals, a `Vec<str>`/`Vec<Vec>` that owns its elements,
   collections declared in a `region` at its end, and string temporaries when
   it can prove they are finished (`test_mem_owned_drop.fd`). Everything else
   lives until the process exits: it leaks, it does not dangle.

---

## 7. Control Flow

### 7.1 If / Else

```fides
// Statement form
if x > 0 {
    println("positive");
} else if x < 0 {
    println("negative");
} else {
    println("zero");
}

// Expression form (returns value)
val sign = if x > 0 { 1 } else if x < 0 { -1 } else { 0 };

// With let
val result = if condition {
    compute_value()
} else {
    default_value()
};
```

### 7.2 Match

```fides
// Statement match: every arm body is a block
match color {
    Color::Red => { println("red"); }
    Color::Green => { println("green"); }
    Color::Blue => { println("blue"); }
}

// Match as a value: an arm body may be an expression
val rgb = match color {
    Color::Red => 0xFF0000,
    Color::Green => 0x00FF00,
    Color::Blue => 0x0000FF,
};

// With bindings (write Option::None: a bare None is a new variable)
match option {
    Option::Some(value) => { println(f"Got {value}"); }
    Option::None => { println("Nothing"); }
}

// With guards
val grade = match score {
    s if s >= 90 => "A",
    s if s >= 80 => "B",
    s if s >= 70 => "C",
    _ => "F",
};

// Payload bindings
val desc = match shape {
    Shape::Circle(r) => f"Circle(r={r})",
    Shape::Rectangle(w, h) => f"Rect({w}x{h})",
    Shape::Triangle(a, b, c) => f"Tri({a},{b},{c})",
    Shape::Point => "Point",
};

// Match on ranges
val size = match x {
    0..=9 => "single digit",
    10..=99 => "double digit",
    100..=999 => "triple digit",
    _ => "large",
};
```

A statement `match x { 1 => f(), _ => g(), }` (expression arms) is a parse
error. Struct-like patterns (`Shape::Circle { radius }`), nested, tuple and `|`
patterns do not parse. A `match` on an enum must cover every variant and one
on `bool` both values, unless an arm is `_` or a name; a `match` on integers or
strings is not checked for exhaustiveness.

### 7.3 Loops

```fides
// For-in loop (iterator based)
for x in nums {
    println(x);
}

// With an index (`for (i, x) in …` does not parse)
for i in 0..nums.len() {
    println(f"{i}: {nums[i]}");
}

// Range
for i in 0..10 {
    println(i);
}

// Inclusive range
for i in 0..=10 {
    println(i);
}

// While loop
var i = 0;
while i < 10 {
    println(i);
    i += 1;
}

// Infinite loop
loop {
    val input = read_line();
    if input == "quit" {
        break;
    }
    if input == "skip" {
        continue;
    }
    process(input);
}
```

`loop` is a statement, not an expression: `val r = loop { … }` does not parse,
and `break expr` evaluates `expr` and discards it. There are no loop labels.

### 7.4 Return

```fides
fn foo(x: i32) -> i32 {
    if x < 0 {
        return 0;     // early return
    }
    x * 2             // implicit return (last expression)
}

fn max(a: i64, b: i64) -> i64 {
    if a > b { a } else { b }   // a trailing if/match is the value
}
```

### 7.5 Defer

`defer` takes a block (braces are required; `defer close(file);` does not
parse). Deferred blocks run when the function returns, last registered first:

```fides
fn work() -> i64 {
    println("start");
    defer { println("cleanup"); }   // runs at every return after this point
    return 1;                       // prints "cleanup", then returns
}
```

Deferred blocks run at function exit, not block exit. Every `return` lowered
after a `defer` statement runs it, also when the `defer` was in a branch that
did not execute (reference.md §5.5).

---

## 8. Structs and Impl

### 8.1 Struct Definitions

```fides
// Named fields
struct Point {
    x: f64,
    y: f64,
}

// Generic struct
struct Vec3<T: Copy> {
    x: T,
    y: T,
    z: T,
}

// Public struct with private fields
pub struct Config {
    pub name: str,
    pub version: str,
    internal_hash: u64,   // private — not accessible outside module
}
```

### 8.2 Struct Literals

```fides
// Full literal
val p = Point { x: 3.0, y: 4.0 };

// Update syntax (copy remaining fields from another)
val p2 = Point { x: 5.0, ..p };   // x=5.0, y=4.0 (from p)
```

There is no field shorthand (`Point { x, y }`). A field left out of a literal
is not reported.

### 8.3 Impl Blocks

```fides
impl Point {
    // Associated function (constructor), called as Point::new(1.0, 2.0)
    fn new(x: f64, y: f64) -> Point {
        return Point { x: x, y: y };
    }

    // Method: `self` (type Self), or `self: &Point`
    fn distance(self, other: &Point) -> f64 {
        val dx = self.x - other.x;
        val dy = self.y - other.y;
        return math_sqrt(dx * dx + dy * dy);
    }

    // Changing method
    fn translate(self: &mut Point, dx: f64, dy: f64) {
        self.x += dx;
        self.y += dy;
    }
}
```

`&self` and `&mut self` are not valid syntax. Annotations cannot be written on
methods. There are no associated constants.

### 8.4 Tuple Structs

Not supported: `struct Color(u8, u8, u8)` does not parse. Use a struct with
named fields.

---

## 9. Enums and Pattern Matching

### 9.1 Enum Definitions

```fides
// Simple enum (unit variants)
enum Color {
    Red,
    Green,
    Blue,
}

// Enum with data
enum Message {
    Quit,                           // unit variant
    Move(x: i32, y: i32),           // named payload (no struct variants)
    Write(str),
    ChangeColor(u8, u8, u8),
}

// Generic enum
enum Option<T> {
    Some(T),
    None,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

// Enum with methods
impl Message {
    fn process(self) {
        match self {
            Message::Quit => { println("Quitting"); }
            Message::Move(x, y) => { println(f"Move to ({x}, {y})"); }
            Message::Write(text) => { println(text); }
            Message::ChangeColor(r, g, b) => { println(f"Color: {r},{g},{b}"); }
        }
    }
}
```

### 9.2 Pattern Matching

```fides
// Exhaustive match on an enum (every variant, or `_`)
val n = match msg {
    Message::Quit => 0,
    Message::Move(x, y) => x + y,
    Message::Write(text) => str_len(text),
    Message::ChangeColor(r, g, b) => (r as i32 + g as i32 + b as i32),
};

// Wildcard
val s = match x {
    0 => "zero",
    _ => "nonzero",
};
```

Or-patterns (`"q" | "quit"`), nested patterns and struct patterns
(`Point { x: 0.0, y }`) and `@` bindings (`n @ 0..=12`) do not parse.
Integer patterns are non-negative literals and ranges (`0..=9`); `-1 =>` is a
parse error.

---

## 10. Traits (cap)

### 10.1 Trait (cap) Definitions

```fides
// Basic cap
cap Drawable {
    fn draw(self);
    fn area(self) -> f64;
}

// cap with default implementation
cap Greet {
    fn greet(self) {
        println("Hello from " + self.name());
    }

    fn name(self) -> str;
}

// Associated type
cap Iterator {
    type Item;

    fn next(self: &mut Self) -> Option<Self::Item>;
}

// Supertraits: an impl of Ord must provide Eq's and PartialOrd's methods
cap Ord: Eq + PartialOrd {
    fn cmp(self, other: &Self) -> i64;
}
```

### 10.2 Trait (cap) Implementations

```fides
impl Drawable for Point {
    pub fn draw(self: Point) {
        render_pixel(self.x, self.y);
    }

    pub fn area(self: Point) -> f64 {
        return 0.0;  // points have no area
    }
}
```

An impl must provide every method without a default and every associated
type. The type after `for` is a single name: `impl<T> Show for Slot<T>` is a
parse error. Parameter and return types of impl methods are not compared with
the trait's.

### 10.3 Trait (cap) Bounds

```fides
// In function signatures; every bound is checked at each call
fn largest<T: Ord + Clone>(v: Vec<T>) -> T {
    // ...
}
```

There are no `where` clauses and no `impl Trait` return types. A call whose
argument does not implement a bound is an error
(`i64 does not implement Show (required by T: Show in 'twice')`,
`test_generic_bound_fail.fd`).

---

## 11. Error Handling

### 11.1 Result Type

```fides
// Returning Result (variants are written with the enum name)
fn parse_num(s: str) -> Result<i64, str> {
    // ...
    return Result::Ok(42);
}

// Handling with match
match parse_num("42") {
    Result::Ok(n) => { println(f"Got {n}"); }
    Result::Err(e) => { println(f"Error: {e}"); }
}

// Propagating with ? operator
fn load(path: str) -> Result<i64, str> {
    val n = parse_num(path)?;           // ? returns the Err
    return Result::Ok(n + 1);
}
```

`?` does not convert error types (there is no `From`).

### 11.2 Effects and Effect Handlers

Every function has an inferred set of effects (`IO`, `Net`, `Spawn`,
`Diverge`, `Alloc`, `Read`, `Write`, `Diff`, `Noise`, `Comm`), propagated
through calls. A function may declare its effects after the return type
(`!IO`, `!{IO, Net}`, `!{}`); an inferred effect that is not declared is an
error. A function that takes a function value gets the effects of the function
passed at each call site (`effects/test_effect_ho_instantiation.fd`).

Effect handlers (reference.md §9.4):

```fides
handle {
    val x = 10;
    do IO.print(x);
    println(x + 1);          // runs after resume
} with {
    IO.print(v) => {
        println(v);
        resume(cont, 0);
    }
}
```

`do E.op(args)` runs the matching handler with the arguments bound as `i64`;
`cont` is the rest of the `handle` block, and `resume(cont, v)` runs it
(`test_effect_handler.fd`, `test_effect_resume.fd`).

---

## 12. Modules and Imports

### 12.1 Module Declaration

```fides
// File: src/net/http.fd
pub module net::http;

pub struct Request { ... }
pub fn get(url: &str) -> Response { ... }

// Submodule
pub module net::http::client;
```

### 12.2 Imports

```fides
// Import a module: all its public items become visible
use collections;
use net::http;              // <dir>/net/http.fd, <dir>/net/http/mod.fd, stdlib, project root

// Qualified calls: http::get(…), or with a rename j::f()
use json as j;

// Grant capabilities
use file with { IO, FS };
```

`use m::{a, b}` and `use m::*` are accepted, but the list does not select:
they import the whole module. `core` is always loaded. A version pin
`use m = "1.0";` is a parse error.

### 12.3 Visibility

```fides
// Private by default
fn helper() { ... }           // private to module

// Public
pub fn api() { ... }          // accessible from outside
```

A struct field without `pub` is private to its module. There is no
`pub(crate)` or `pub(super)`.

---

## 13. Generics

### 13.1 Generic Functions

```fides
fn identity<T>(x: T) -> T {
    x
}

fn show_all<T: Show + Clone>(v: Vec<T>) -> str {
    // ...
}

// Explicit type arguments (turbofish); the count must match
val z = zero::<i64>();

// Const parameter: [T; N] takes the array's length
fn sum<N: i64>(a: [i64; N]) -> i64 { /* ... */ }
```

Generic functions are monomorphised (`compiler/mono.fd`). `fid` infers type
arguments from the arguments, and from the expected type when only the result
mentions a parameter (`val z: i64 = zero();`). A parameter nothing gives is an
error (`cannot infer the type parameter 'T' of 'make'`). Bounds `T: A + B` are
checked at each call. Tests: `test_generics_full.fd`,
`test_generic_infer_fail.fd`, `test_generic_bound_fail.fd`,
`test_turbofish_arity_fail.fd`; design: proposal 0002. There are no `where`
clauses.

### 13.2 Generic Types

```fides
struct Pair<A, B> {
    first: A,
    second: B,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

impl<T> Slot<T> {
    fn get(self) -> T { return self.value; }
}
```

Methods of a generic `impl` are typed per receiver: on a `Slot<f64>`, `T` is
`f64`. Bounds on struct parameters are not part of the grammar.

### 13.3 Associated Types

```fides
cap Iterator {
    type Item;

    fn next(self) -> Option<Self::Item>;
}

impl Iterator for Range {
    type Item = i64;

    pub fn next(self: Range) -> Option<i64> {
        // ...
    }
}
```

The type checker treats an associated type as compatible with every type.

---

## 14. Compile-Time Code (comptime) — ✅ Working

Fides replaces macros with `comptime` blocks — real code that runs at compile time. Implemented in `compiler/comptime_eval.fd`.

### 14.1 Comptime Blocks

```fides
// Compile-time evaluation
const MAX_SIZE: i32 = comptime {
    val config = parse_env("P_MAX_SIZE");
    config.unwrap_or(1024)
};

// Compile-time code generation
comptime {
    // Generate code based on build configuration
    for item in build_config().items {
        generate_handler(item);
    }
}

// Comptime function — note: `comptime` goes before `fn`
comptime fn fibonacci(n: i32) -> i32 {
    if n <= 1 { n } else { fibonacci(n - 1) + fibonacci(n - 2) }
}

const FIB_10: i32 = comptime fibonacci(10);  // 55
```

### 14.2 Comptime Reflection — ✅ Working

```fides
val name = comptime type_name(Point);        // type name as string
val s = comptime is_struct(Point);           // true if a struct
val e = comptime is_enum(Shape);             // true if an enum
val n = comptime field_count(Point);
val fields = comptime field_names(Point);    // field names
val types = comptime field_types(Point);
val variants = comptime enum_variants(Shape);
val size = comptime struct_size(Point);
```

Also `field_offset(T.f)`, `enum_discriminant(E.V)`, `function_effects(f)` and
`function_capabilities(m)` (`test_comptime_reflect.fd`).

### 14.3 Comptime vs Macros

| Feature | Rust Macros | Fides comptime |
|---------|------------|-----------------|
| Syntax | `macro_rules!` or proc macros | `comptime { }` blocks |
| Language | Macro DSL or Rust | Fides itself |
| Debugging | Difficult | Standard debugger |
| Error messages | Often cryptic | Compiler-generated, clear |
| AI generation | Hard (macro DSL) | Easy (it's just Fides) |

---

## 15. Attributes

> **Implementation status:** Fides uses `@annotation` syntax, written before a top-level `fn` or `struct`, a function parameter, or a `val`/`var`. Every annotation name is checked: an unknown name is an error (`Unknown annotation`), and one on the wrong kind of item is an error (`Misplaced annotation`). Annotations cannot be written on methods, struct fields, enums or globals. `#[...]` at the top level is parsed and ignored. `fid test file.fd` runs the functions whose names start with `test_` and `bench_`. The full table is in [reference.md §11](reference.md#11-annotations); the safety rules and their tests are in [safety-rules.md](safety-rules.md).

```fides
@derive(Debug, Clone)                 // generates Point_debug_str, Point_clone
struct Point { x: f64, y: f64 }

@derive(Serialize, Deserialize)       // Config_to_json, Config_from_json
struct Config { /* ... */ }

@cfg(linux)                           // kept only when fid runs on Linux
fn linux_only() { /* ... */ }

@cfg(platform = "windows")
fn windows_only() { /* ... */ }

@no-alloc                             // safety rule, checked
fn fast_add(a: i32, b: i32) -> i32 { a + b }
```

`@cfg(…)` filters functions and structs: `linux`, `windows`, `macos`,
`platform = "…"`, `os = "…"` hold when `fid` itself runs on that system (the
host, not `--target`); `arch = "x86_64"` and `release` always hold; `debug`
and the ARM64/RISC-V arches never do (`test_cfg.fd`, all three backends).
`@inline` and `@deprecated` do not exist.

---

## 16. Concurrency (✅ Implemented — Phase 7.6)

> **Implementation status:** `spawn f(args)` starts `f` on an OS thread and gives a `Future<T>`. An `async fn` runs as a green task on its own stack; `await e` (prefix) waits for a `Future<T>` and gives `T`. `async fn` with `f64` or struct arguments and results runs as a task (interpreter and C). Native (`fid emit`) programs that use async or spawn+await crash (`test_async`, `test_async_float_struct`). `yield e` evaluates `e`; it is not a suspension. Details: [reference.md §10](reference.md#10-concurrency).

### 16.1 Async / Await

```fides
// Declared Future<T>; the body returns T
async fn compute_value(x: i64) -> Future<i64> {
    return x * 2;
}

fn main() {
    val r = await compute_value(1);
    val s = await compute_value(2) + await compute_value(3);
    println(r + s);
}
```

### 16.2 Spawning Threads

```fides
fn work(n: i64) -> i64 { return n * n; }

val f = spawn work(7);     // runs on another thread; f: Future<i64>
process_local();
val result = await f;
```

Every argument of a spawned call must be `@send`: numbers, `bool`, `str`,
enums, `@send` structs, structs whose fields are all `@send`, and `Vec`,
`HashMap`, `Option`, `Result`, `Box`, `Future`, `Mutex`, `Channel` of such
types. References and raw pointers are not (`test_spawn_send_fail.fd`,
`test_race_ptr_fail.fd`). A function that runs on another thread may not
assign a global unless it takes a lock (`test_race_global_fail.fd`).

### 16.3 parallel, spawn scope, channels

```fides
parallel {
    | a = fib(n);
    | b = fib(n + 1);
}
```

Branches are separated by `|`. A `x = f(…)` branch is spawned and `x` gets the
result when the threads are joined; a multi-statement branch that only reads
word-sized locals runs on its own thread; any other branch runs in order on
the current thread (`test_parallel_branches.fd`, all three backends).
`spawn scope { … }` joins every spawned thread at its end; `cancel scope;`
cancels pending tasks. The `channel` module provides `channel_create`,
`channel_send`, `channel_recv`, `channel_try_recv`, `channel_close`; `select`
takes the first case whose channel has a value (`test_channels_select.fd`).
`Rc<T>` and `Arc<T>` are reference-counted with manual `clone`/`drop`.

---

## 17. Capability-Based Security — ✅ Working

Capabilities are checked at compile time, with no run-time cost.

> **Implementation status:** `use ... with { IO, FS, Net, Spawn, Alloc }` grants capabilities. When any import of the program has a `with` clause, the program runs in strict mode: a call that needs a capability not granted is an error. The check looks at each call by name (`print` needs `IO`, `read_file` needs `FS`, `http_get` needs `Net`, `spawn` needs `Spawn`, `p_malloc` needs `Alloc`), in every function except those of `core`. It does not follow calls through the call graph. Inside `sandbox { }` every such call is an error, whatever was granted (reference.md §9.2–9.3).

### 17.1 Capability Grants in Imports

```fides
use network_lib with { Net, IO };       // grants Net and IO
use file_lib with { FS };               // grants FS
use pure_lib;                           // grants nothing
```

### 17.2 Capability Declarations

```fides
// `cap` declares a trait (§10); it is not a run-time capability
cap Database {
    fn query(self, sql: str) -> Vec<str>;
}
```

### 17.3 Compile-Time Enforcement

```fides
use util with { FS };      // strict mode: only FS granted

fn bad() {
    print("hi");           // ERROR: print() requires the IO capability
}

fn ok() {
    sandbox {
        val n = 1 + 2;     // allowed; print() here would be an error
    }
}
```

### 17.4 What Is Not Checked

Capabilities do not propagate through the call graph: a function that calls
another function which prints is not itself checked for `IO`. Effects (§11.2)
do propagate; declare `!{}` or `!IO` on a function to have the compiler check
everything it reaches.

---

## 18. Hot Reload — ⚠️ Rebuild and rerun only

`fidesc watch` (the frozen Rust compiler, `src/hotreload.rs`) rebuilds and
reruns the program when the file is saved and reports which functions changed;
functions are not swapped in the running process. In `fid`, the `watch`
command currently runs `bench` (`compiler/main.fd`).

```fides
// With `fidesc watch`, saving the file rebuilds and reruns the program
pub fn handle_request(req: &Request) -> Response {
    // Planned: replace this function at runtime without restarting.
    // Today the whole program is rebuilt and restarted.
    process(req)
}
```

```bash
# Start watching — compiles to native ELF64, runs, and watches for changes
fidesc watch main.fd

# When you edit and save, the program is rebuilt and run again; watch
# reports which functions changed (in-process swapping is planned)
```

> **Note:** Hot reload for FidesOS (the OS project) uses the same mechanism with OS-specific integration. The `fidesc watch` command works on any Linux system.

---

## 19. Standard Library Prelude

Items available without explicit import (from `stdlib/core.fd`, auto-included):

```fides
// Types (variants are written Option::Some(x), Result::Ok(x))
Option<T>     Result<T, E>   Vec<T>   HashMap<K, V>   Box<T>   Rc<T>   Arc<T>
StringBuilder

// Functions
print         println       assert        assert_msg     assert_eq     assert_ne
panic         max           min           abs            clamp
int_to_str    float_to_str  byte_to_str   str_*          (string functions)

// Note: `print` is built in and accepts any value (int, float, bool, str,
// Vec, HashMap); `println(s)` prints a str followed by "\n".

// Traits (caps) declared in core
Iterator      Error

// Macros (none — Fides has no macros)
```

> **Note:** `Box<T>`, `Rc<T>`, and `Arc<T>` are built-in types. `Box<T>` enables recursive types. `Rc<T>` / `Arc<T>` provide manual reference counting. Channels are in `stdlib/channel.fd`. See section 3.4a for stdlib modules and [reference.md §13](reference.md#13-the-prelude) for the prelude.

---

## 20. File Extension

Fides source files use the `.fd` extension:

```
main.fd
parser.fd
net/http.fd
```

---

## 21. Complete Example

```fides
use file;

@derive(Debug, Clone)
struct Config {
    name: str,
    lines: i64,
}

impl Config {
    fn new(name: str) -> Config {
        return Config { name: name, lines: 0 };
    }
}

enum LoadError {
    FileNotFound,
    ParseError(str),
}

fn load_config(path: str) -> Result<Config, LoadError> {
    val content = file_read(path);          // "" when the file cannot be read
    if str_len(content) == 0 {
        return Result::Err(LoadError::FileNotFound);
    }
    var config = Config::new("app");
    for line in str_lines(content) {
        if str_contains(line, "=") {
            config.lines += 1;
        }
    }
    return Result::Ok(config);
}

// `fid test file.fd` runs functions whose names start with test_
fn test_config() {
    val config = Config::new("test");
    assert_eq(config.lines, 0);
}

fn main() -> i64 {
    match load_config("ark.toml") {
        Result::Ok(c) => { println(f"Loaded: {c.name}, {c.lines} settings"); }
        Result::Err(e) => { println("no config"); }
    }
    return 0;
}
```

---

## 22. Syntax Summary Cheat Sheet

```
// Functions
fn name(params) -> ReturnType { body }
pub fn name(params) -> ReturnType { body }
async fn name(params) -> Future<T> { body }

// Variables
val x = value;
var x = value;
val x: Type = value;

// Control flow
if cond { } else if cond { } else { }
val v = if cond { a } else { b };
match value { Pattern => { }, _ => { } }          // statement: block arms
val v = match value { Pattern => expr, _ => d };  // value: expression arms
for x in iter { }
while cond { }
loop { break; }

// Types
struct Name { field: Type }
enum Name { Variant, Variant(T), Variant(name: T) }
cap Name { fn method(self); }
impl Name for Type { fn method(self: Type) { } }

// Generics
fn name<T: A + B>(x: T) -> T { }
name::<i64>(x)
struct Name<T, U> { }

// Error handling
Result<T, E>    Result::Ok(value)    Result::Err(error)
expr?           // propagate error

// Strings
"literal"       f"interpolated {expr}"    r"raw"

// Collections
[T; N]          Vec<T>       HashMap<K, V>
[a, b, c]       0..10        0..=10

// Modules
use module;
use module with { IO, FS };
pub fn name() { }

// Other
@annotation
comptime { }
defer { stmts }
spawn f(args)
await expr
```

---

## 22. LSP Server (Editor Integration)

Fides includes a built-in Language Server Protocol server — no external tools
required.

### Starting the LSP Server

```bash
fid lsp        # or fidesc lsp
```

This starts a JSON-RPC 2.0 server over stdio. Configure your editor to use
`fid lsp` as the language server command.

### Supported LSP Methods

| Method | Description |
|--------|-------------|
| `initialize` | Returns server capabilities (textDocumentSync, diagnosticProvider) |
| `initialized` | Notification — no response needed |
| `shutdown` | Prepares for shutdown |
| `exit` | Terminates the server |
| `textDocument/didOpen` | Publishes diagnostics when a file is opened |
| `textDocument/didChange` | Full sync — re-publishes diagnostics on every change |
| `textDocument/didClose` | Clears document state |
| `textDocument/publishDiagnostics` | Sends error diagnostics with line/column positions |

### Editor Configuration

**VS Code** (`.vscode/settings.json`):
```json
{
    "fides.languageServerPath": "/path/to/fidesc",
    "fides.languageServerArgs": ["lsp"]
}
```

**Neovim** (`init.lua`):
```lua
require'lspconfig'.fides.setup{
    cmd = {'/path/to/fidesc', 'lsp'},
    filetypes = {'fides'},
}
```

### Notes

- The LSP server uses a built-in JSON parser/serializer — no external Rust
  dependencies (no serde, no serde_json).
- Diagnostics are sourced from the compiler's type checker and parser.
- Text document sync mode is `1` (full document sync).

---

# Fides Syntax Migration — Completed

**Version:** 1.0
**Date:** 2026-08-26
**Status:** COMPLETED — migration is done, all .fd files use the new syntax

> This document records the syntax migration from Fides's original keywords
> (`pf`/`pv`/`pc`/`ret`/`ty`/`pm`) to standard, universally understood keywords
> (`fn`/`val`/`var`/`const`/`return`/`type`). The `cap` and `use` keywords were
> kept — they are short, distinct, and already part of Fides's identity.
> The migration is complete: the lexer, parser, all test files, and all stdlib
> modules use the new syntax.
>
> The before/after examples below show the keyword rename only. Some of them
> use syntax that `fid` does not accept: `&self` (write `self` or
> `self: &T`), `where` clauses, postfix `.await` (write `await e`), and
> `defer expr;` (write `defer { … }`). See [reference.md](reference.md).

---

## 1. Design Rationale

### 1.1 Why change the syntax?

The original Fides keywords (`fn`, `val`, `const`, `return`, `type`, `pm`) were
unique to Fides. While they gave the language a distinct identity, they created
unnecessary friction:

- **Learning curve:** Every new user had to learn 6+ unfamiliar keywords
- **AI familiarity:** AI models are trained on billions of lines of code using
  `fn`, `let`, `return`, `type` — not `fn`, `val`, `return`
- **No performance benefit:** Shorter keywords have zero effect on compile
  speed, binary size, or runtime performance (keywords become token enum
  variants after lexing)
- **Self-hosting:** The self-hosted compiler will be written in Fides. Using
  standard keywords makes the compiler source code more readable and
  accessible to contributors

### 1.2 Design principles for the new syntax

1. **Familiar over unique** — Use keywords that programmers already know
2. **Systems language conventions** — Follow Rust/Zig/Carbon, not Python/Ruby
3. **Clean pairs** — `val`/`var` for immutable/mutable, not `let`/`let mut`
4. **One keyword per concept** — No overloading (except `const` which is
   clearly compile-time only)
5. **Minimal keyword count** — Remove unnecessary keywords (`pm` eliminated)

### 1.3 Analysis of other languages

| Language    | Function | Immutable | Mutable    | Constant  | Return   | Trait      | Type alias |
|-------------|----------|-----------|------------|-----------|----------|------------|------------|
| Rust        | `fn`     | `let`     | `let mut`  | `const`   | `return` | `trait`    | `type`     |
| Zig         | `fn`     | `const`   | `var`      | `const`   | `return` | —          | —          |
| Carbon      | `fn`     | `let`     | `var`      | `let`     | `return` | `interface`| `alias`    |
| Go          | `func`   | `const`   | `var`      | `const`   | `return` | `interface`| `type`     |
| Swift       | `func`   | `let`     | `var`      | `let`     | `return` | `protocol` | `typealias`|
| Kotlin      | `fun`    | `val`     | `var`      | `const`   | `return` | `trait`    | `typealias`|
| Scala       | `def`    | `val`     | `var`      | `val`     | `return` | `trait`    | `type`     |
| Python      | `def`    | —         | —          | —         | `return` | —          | —          |
| Mojo        | `def`    | —         | `var`      | `comptime`| `return` | `trait`    | —          |
| Nim         | `proc`   | `let`     | `var`      | `const`   | `return` | `concept`  | `type`     |
| Odin        | `proc`   | `let`     | `var`      | `::`      | `return` | —          | `type`     |
| C/C++       | —        | `const`   | —          | `const`   | `return` | —          | `typedef`  |
| Java        | —        | `final`   | —          | `final`   | `return` | `interface`| `import`   |
| JavaScript  | `function`| `const`  | `let`      | `const`   | `return` | —          | —          |
| Lua         | `function`| `local`  | `local`    | —         | `return` | —          | —          |
| Crystal     | `def`    | —         | —          | —         | `return` | —          | —          |

**Fides's choice** takes the best from each:
- `fn` from Rust/Zig/Carbon (systems language standard)
- `val`/`var` from Kotlin/Scala/Swift (clean 3-letter pair)
- `const` from Zig/Go/C (compile-time only, universal)
- `return` from every language (universal)
- `cap` kept — short, distinct, part of Fides identity (capability/trait)
- `type` from Rust/Go/Scala (universal)
- `use` kept — short, universal enough, already part of Fides identity

---

## 2. Keyword Changes Summary

### 2.1 Changed keywords

| Old (Fides) | New        | Meaning              | Used by (examples)        |
|-------------|------------|----------------------|---------------------------|
| `pf`        | `fn`       | Function declaration | Rust, Zig, Carbon         |
| `pv`        | `val`      | Immutable variable   | Kotlin, Scala, Swift      |
| `pv mut`    | `var`      | Mutable variable     | Swift, Kotlin, Scala, Zig |
| `pc`        | `const`    | Compile-time constant| Zig, Go, C, Rust          |
| `ret`       | `return`   | Return statement     | Every language            |
| `ty`        | `type`     | Type alias           | Rust, Go, Scala, Nim      |
| `pm`        | *(removed)*| Use `var x: T;`     | Zig, C, Carbon            |

### 2.2 Kept keywords (already correct)

| Keyword     | Meaning              | Used by (examples)        |
|-------------|----------------------|---------------------------|
| `match`     | Pattern matching     | Rust, Swift               |
| `struct`    | Struct declaration   | Rust, C, C++, Go, Zig     |
| `enum`      | Enum declaration     | Rust, C, C++, Java, Zig   |
| `pub`       | Public visibility    | Rust                      |
| `impl`      | Implementation block | Rust                      |
| `cap`       | Capability/trait     | Fides-unique (kept)       |
| `use`       | Module import        | Rust, Zig (kept)          |
| `comptime`  | Compile-time code    | Zig                       |
| `defer`     | Deferred cleanup     | Go, Zig, Odin             |
| `async`     | Async function       | Rust, Python, JS, Swift   |
| `await`     | Await async result   | Rust, Python, JS, Swift   |
| `spawn`     | Spawn task           | Go, Zig                   |
| `region`    | Region block         | Fides-unique              |
| `handle`    | Effect handler       | Fides-unique              |
| `do`        | Effect handler block | Koka, Helium              |
| `with`      | Capability grant     | Fides-unique              |
| `move`      | Move semantics       | Rust                      |
| `as`        | Type cast            | Rust, Python, TypeScript  |
| `extern`    | FFI declaration      | Rust, C, C++, Zig         |
| `for`       | For loop             | Every language            |
| `in`        | For-in iteration     | Rust, Python, Swift, JS   |
| `while`     | While loop           | Every language            |
| `loop`      | Infinite loop        | Rust                      |
| `if`        | Conditional          | Every language            |
| `else`      | Else branch          | Every language            |
| `break`     | Break loop           | Every language            |
| `continue`  | Continue loop        | Every language            |
| `dyn`       | Dynamic dispatch     | Rust                      |
| `module`    | Module declaration   | Swift, Zig, Odin          |
| `static`    | Static variable      | Rust, C, C++, Java        |
| `where`     | Trait bounds         | Rust, Haskell             |
| `try`       | Try block            | C++, Java, JS, Rust       |
| `true`      | Boolean true         | Every language            |
| `false`     | Boolean false        | Every language            |
| `mut`       | Mutable modifier     | Rust, Zig                 |
| `ref`       | Reference modifier   | Rust, C++                 |

### 2.3 Removed keywords

| Keyword  | Reason                                                      |
|----------|-------------------------------------------------------------|
| `pm`     | Replaced by `var x: T;` (no initializer). Definite          |
|          | assignment analysis still catches read-before-write.        |

---

## 3. Complete Keyword List (New Syntax)

```
fn          val         var         const       mut         pub         struct
enum        cap         impl        use         match       type
if          else        for         while       loop
return      break       continue    async       await
comptime    in          as          where       region
spawn       defer       move        static      try
handle      do          with        extern      dyn         module
interrupt

true        false
```

**Note:** `none`, `some`, `ok`, `err` are NOT keywords — they are enum variant
names from the standard library (`Option` and `Result`).

---

## 4. Variable Declaration Model

The new model uses a clean three-keyword system:

### 4.1 Immutable variable (`val`)

```fides
val x = 10;              // type inferred (i32)
val y: i64 = 20;         // explicit type
val name = "Fides";      // type inferred (&str)
val z = compute_value(); // runtime value, immutable after binding
```

`val` means: "this binding cannot be reassigned, but the value is computed
at runtime." This is the default — prefer `val` over `var` whenever possible.

### 4.2 Mutable variable (`var`)

```fides
var x = 10;              // type inferred (i32), mutable
var y: i64 = 20;         // explicit type, mutable
var count = 0;
count += 1;              // OK — var is mutable
```

### 4.3 Uninitialized mutable variable (`var` without initializer)

```fides
var x: i32;              // declared, no initial value
if condition {
    x = 1;
} else {
    x = 2;
}
println(f"x = {x}");     // OK — definite assignment analysis proves x is set
```

```fides
var x: i32;
println(f"x = {x}");     // ERROR — x may be uninitialized (definite assignment)
```

This replaces the old `pm` keyword. The definite assignment analysis that was
built for `pm` now runs on `var` declarations without initializers.

### 4.4 Compile-time constant (`const`)

```fides
const MAX_SIZE: i32 = 1024;    // compile-time constant, explicit type
const PI: f64 = 3.14159;       // compile-time constant
const GREETING = "Hello";      // compile-time constant, type inferred
```

`const` means: "this value is known at compile time and is embedded directly
into the binary." Unlike `val`, `const` values cannot be computed at runtime.

### 4.5 Summary table

| Keyword  | Mutability  | Init required | When known    | Use case                    |
|----------|-------------|---------------|---------------|-----------------------------|
| `val`    | Immutable   | Yes           | Runtime       | Default for all bindings     |
| `var`    | Mutable     | No            | Runtime       | When reassignment is needed |
| `const`  | Immutable   | Yes           | Compile-time  | Compile-time constants       |

---

## 5. Syntax Examples (Old vs New)

### 5.1 Hello World

**Old:**
```fides
use core;

pf main() -> i64 {
    println("Hello, World!");
    ret 0;
}
```

**New:**
```fides
use core;

fn main() -> i64 {
    println("Hello, World!");
    return 0;
}
```

### 5.2 Variables

**Old:**
```fides
pv x = 10;
pv mut y = 20;
pc MAX: i32 = 100;
pm z: i32;
z = 42;
```

**New:**
```fides
val x = 10;
var y = 20;
const MAX: i32 = 100;
var z: i32;
z = 42;
```

### 5.3 Function with capabilities

**Old:**
```fides
cap Drawable {
    pf draw(&self);
}

impl Drawable for Circle {
    pf draw(&self) {
        println("Drawing circle");
    }
}
```

**New:**
```fides
cap Drawable {
    fn draw(&self);
}

impl Drawable for Circle {
    fn draw(&self) {
        println("Drawing circle");
    }
}
```

### 5.4 Type alias

**Old:**
```fides
ty Score = i32;
```

**New:**
```fides
type Score = i32;
```

### 5.5 Pattern matching

**Old:**
```fides
pv result = match x {
    1 => "one",
    2 => "two",
    _ => "other",
};
```

**New:**
```fides
val result = match x {
    1 => "one",
    2 => "two",
    _ => "other",
};
```

### 5.6 Closures

**Old:**
```fides
pv add = pf(x: i32, y: i32) { x + y };
pv double = pf(x: i32) => x * 2;
```

**New:**
```fides
val add = fn(x: i32, y: i32) { x + y };
val double = fn(x: i32) => x * 2;
```

### 5.7 Generic function with bounds

**Old:**
```fides
pf max<T>(a: T, b: T) -> T where T: Ord {
    if a > b { ret a; } else { ret b; }
}
```

**New:**
```fides
fn max<T>(a: T, b: T) -> T where T: Ord {
    if a > b { return a; } else { return b; }
}
```

### 5.8 Error handling

**Old:**
```fides
pf read_file(path: &str) -> Result<String, Error> {
    pv content = file_read(path)?;
    ret content;
}
```

**New:**
```fides
fn read_file(path: &str) -> Result<String, Error> {
    val content = file_read(path)?;
    return content;
}
```

### 5.9 Struct with impl

**Old:**
```fides
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    pf new(x: f64, y: f64) -> Point {
        ret Point { x, y };
    }

    pf distance(&self, other: &Point) -> f64 {
        pv dx = self.x - other.x;
        pv dy = self.y - other.y;
        ret (dx * dx + dy * dy).sqrt();
    }
}
```

**New:**
```fides
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    fn new(x: f64, y: f64) -> Point {
        return Point { x, y };
    }

    fn distance(&self, other: &Point) -> f64 {
        val dx = self.x - other.x;
        val dy = self.y - other.y;
        return (dx * dx + dy * dy).sqrt();
    }
}
```

### 5.10 Import with capabilities

**Old:**
```fides
use core with { IO, FS };
use core::string;
```

**New:**
```fides
use core with { IO, FS };
use core::string;
```

### 5.11 Async/spawn

**Old:**
```fides
async pf fetch_data(url: &str) -> String {
    ret http_get(url).await;
}

pf main() -> i64 {
    spawn fetch_data("https://example.com");
    ret 0;
}
```

**New:**
```fides
async fn fetch_data(url: &str) -> String {
    return http_get(url).await;
}

fn main() -> i64 {
    spawn fetch_data("https://example.com");
    return 0;
}
```

### 5.12 Effect handlers

**Old:**
```fides
handle !IO {
    pv data = read_input();
    process(data);
} with {
    !IO(e) => {
        println(f"IO error: {e}");
    }
}
```

**New:**
```fides
handle !IO {
    val data = read_input();
    process(data);
} with {
    !IO(e) => {
        println(f"IO error: {e}");
    }
}
```

### 5.13 Comptime

**Old:**
```fides
comptime {
    pv types = @reflect(Widget);
    @generate(types);
}
```

**New:**
```fides
comptime {
    val types = @reflect(Widget);
    @generate(types);
}
```

### 5.14 Region block

**Old:**
```fides
region {
    pv buf = Buffer::new();
    process(&buf);
}  // buf is freed here
```

**New:**
```fides
region {
    val buf = Buffer::new();
    process(&buf);
}  // buf is freed here
```

### 5.15 Defer

**Old:**
```fides
pf write_file(path: &str) -> Result<(), Error> {
    pv f = file_open(path)?;
    defer file_close(f);
    ret write_data(f);
}
```

**New:**
```fides
fn write_file(path: &str) -> Result<(), Error> {
    val f = file_open(path)?;
    defer file_close(f);
    return write_data(f);
}
```

---

## 6. Migration Record (Completed)

### 6.1 Approach: Update Rust compiler first, then migrate all .fd files

**Step 1: Update the Rust compiler** (`src/lexer.rs`, `src/token.rs`, `src/parser.rs`) — DONE
- New keywords: `fn`, `val`, `var`, `const`, `return`, `type`
- Old keywords removed: `pf`, `pv`, `pc`, `ret`, `ty`, `pm`
- Kept existing keywords: `cap`, `use` (unchanged)
- The compiler now accepts ONLY the new syntax

**Step 2: Migration script** — DONE
- `sed` script converted all `.fd` files
- Context-sensitive replacements:
  - `pv mut` → `var` (mutable variable — must handle before `pv` → `val`)
  - `pv` → `val` (immutable variable)
  - `pf` → `fn` (function)
  - `pc` → `const` (constant)
  - `ret` → `return` (return)
  - `ty` → `type` (type alias)
  - `pm` → `var` (uninitialized mutable — remove keyword, keep declaration)
  - `cap` and `use` were unchanged — no replacement needed

**Step 3: Migration script used**
```bash
# Order matters! pv mut → var must happen before pv → val
find . -name "*.fd" -exec sed -i \
    -e 's/\bpv mut\b/var/g' \
    -e 's/\bpv\b/val/g' \
    -e 's/\bpf\b/fn/g' \
    -e 's/\bpc\b/const/g' \
    -e 's/\bret\b/return/g' \
    -e 's/\bty\b/type/g' \
    -e 's/\bpm\b/var/g' \
    {} \;
```

**Step 4: All tests pass** — VERIFIED
- 265 unit tests pass
- 209/211 interpreter tests pass, 198/211 C codegen matched, 209/211 PE valid
- 23/23 self-host tests pass
- Bootstrap fixpoint achieved (session 15)

**Step 5: Documentation updated** — DONE
- `docs/reference/syntax.md` — all examples updated
- `docs/reference/features.md` — keyword references updated
- `README.md` — keyword list updated
- `AGENTS.md` — baseline updated

### 6.2 Migration order (was critical)

The migration script handled replacements in this order:

1. `pv mut` → `var` (BEFORE `pv` → `val`, otherwise `pv mut` becomes `val mut`)
2. `pv` → `val`
3. `pf` → `fn`
4. `pc` → `const`
5. `ret` → `return`
6. `ty` → `type`
7. `pm` → `var`

### 6.3 Edge cases

| Old syntax                    | New syntax                     | Notes                              |
|-------------------------------|--------------------------------|------------------------------------|
| `pv mut x = 10;`             | `var x = 10;`                 | `pv mut` → `var` (drop `mut`)     |
| `pv x = 10;`                 | `val x = 10;`                 | Simple replacement                 |
| `pm x: i32;`                 | `var x: i32;`                 | `pm` → `var`, no initializer       |
| `use core with { IO }`       | `use core with { IO }`        | `use` unchanged                    |
| `cap Drawable { ... }`       | `cap Drawable { ... }`        | `cap` unchanged                    |
| `pf(x) { x * 2 }`            | `fn(x) { x * 2 }`             | Closure syntax                     |
| `pf(x) => x * 2`             | `fn(x) => x * 2`              | Arrow closure                      |
| `async pf foo()`             | `async fn foo()`              | Async function                     |
| `pub pf foo()`               | `pub fn foo()`                | Public function                    |
| `extern "C" pf foo()`        | `extern "C" fn foo()`         | FFI function                       |

---

## 7. What Does NOT Change

These syntax elements remain exactly the same:

| Syntax                    | Meaning                     |
|---------------------------|-----------------------------|
| `&x`, `&mut x`           | References                  |
| `*const T`, `*mut T`     | Raw pointers                |
| `?`                      | Error propagation operator  |
| `@attribute`             | Attribute syntax            |
| `f"..."`                 | String interpolation        |
| `match x { ... }`        | Pattern matching            |
| `struct Foo { }`         | Struct declaration          |
| `enum Foo { }`           | Enum declaration            |
| `impl Foo { }`           | Implementation block        |
| `cap Drawable { }`       | Capability/trait declaration (kept) |
| `use core with { IO }`   | Module import (kept)        |
| `pub`                    | Public visibility           |
| `comptime { }`           | Compile-time block          |
| `defer { }`              | Deferred cleanup            |
| `spawn expr;`            | Spawn task                  |
| `async`/`await`          | Async/await                 |
| `region { }`             | Region block                |
| `handle/with`            | Effect handlers             |
| `x as T`                 | Type cast                   |
| `extern "C"`             | FFI                         |
| `for x in iter`          | For loop                    |
| `while cond`             | While loop                  |
| `loop { }`               | Infinite loop               |
| `if`/`else`              | Conditional                 |
| `break`/`continue`       | Loop control                |
| `dyn Trait`              | Dynamic dispatch            |
| `module foo`             | Module declaration          |
| `static x: T = v;`       | Static variable             |
| `try { }`                | Try block                   |
| `;` (semicolons)         | Optional after statements   |
| `//` and `/* */`         | Comments                    |
| `@attribute`             | Attribute syntax (canonical)|
| `#[test]`                | Parsed and ignored          |
| `..` and `..=`           | Ranges                      |
| `=>`                     | Match arm / closure arrow   |
| `->`                     | Return type arrow           |
| `::`                     | Path separator              |

---

## 8. Impact Assessment

### 8.1 Files affected by migration

| Category              | Count  | Impact                              |
|-----------------------|--------|-------------------------------------|
| `tests/*.fd`          | ~164   | All need keyword replacement        |
| `stdlib/*.fd`         | ~35    | All need keyword replacement        |
| `src/*.rs`            | ~54    | Lexer, parser, typechecker updates  |
| `docs/*.md`           | ~32    | All examples need updating          |
| `roadmap/*.md`        | ~7     | Update references                   |
| `README.md`           | 1      | Update keyword list                 |
| `AGENTS.md`           | 1      | Update baseline                     |

### 8.2 No semantic changes

This is purely a syntax change. No semantics change:
- Types work the same
- Ownership/borrowing works the same
- Effects work the same
- Capabilities work the same
- Codegen is identical
- IR is identical
- All optimizations are identical

### 8.3 No performance impact

Keyword text is discarded after lexing. `fn` and `fn` produce the same token
type (an enum variant). The compiler never looks at the keyword text again.
Binary output is identical.

---

## 9. Comparison: Before and After

### 9.1 Complete example — Before (old syntax)

```fides
use core;
use core::io;

cap Shape {
    fn area(&self) -> f64;
    fn name(&self) -> &str;
}

struct Circle {
    radius: f64,
}

impl Shape for Circle {
    fn area(&self) -> f64 {
        return 3.14159 * self.radius * self.radius;
    }

    fn name(&self) -> &str {
        return "circle";
    }
}

struct Rectangle {
    width: f64,
    height: f64,
}

impl Shape for Rectangle {
    fn area(&self) -> f64 {
        return self.width * self.height;
    }

    fn name(&self) -> &str {
        return "rectangle";
    }
}

fn print_shape(shape: &dyn Shape) {
    println(f"{shape.name()}: {shape.area()}");
}

fn main() -> i64 {
    val circle = Circle { radius: 5.0 };
    val rect = Rectangle { width: 3.0, height: 4.0 };

    print_shape(&circle);
    print_shape(&rect);

    return 0;
}
```

### 9.2 Complete example — After (new syntax)

```fides
use core;
use core::io;

cap Shape {
    fn area(&self) -> f64;
    fn name(&self) -> &str;
}

struct Circle {
    radius: f64,
}

impl Shape for Circle {
    fn area(&self) -> f64 {
        return 3.14159 * self.radius * self.radius;
    }

    fn name(&self) -> &str {
        return "circle";
    }
}

struct Rectangle {
    width: f64,
    height: f64,
}

impl Shape for Rectangle {
    fn area(&self) -> f64 {
        return self.width * self.height;
    }

    fn name(&self) -> &str {
        return "rectangle";
    }
}

fn print_shape(shape: &dyn Shape) {
    println(f"{shape.name()}: {shape.area()}");
}

fn main() -> i64 {
    val circle = Circle { radius: 5.0 };
    val rect = Rectangle { width: 3.0, height: 4.0 };

    print_shape(&circle);
    print_shape(&rect);

    return 0;
}
```

The new version is instantly readable to any Rust, Swift, Kotlin, or Zig
programmer. Zero learning curve for the keywords.

---

## 10. Decision Log

| Decision          | Choice       | Rationale                                              |
|-------------------|--------------|--------------------------------------------------------|
| Function keyword  | `fn`         | Rust/Zig/Carbon standard. 2 letters. AI-familiar.     |
| Immutable var     | `val`        | Kotlin/Scala. Clean 3-letter pair with `var`.          |
| Mutable var       | `var`        | Swift/Kotlin/Scala/Zig. Pairs with `val`.              |
| Compile-time const| `const`      | Zig/Go/C. Universal. Separates runtime from compile.   |
| Return            | `return`     | Universal. Every language uses this.                   |
| Capability/trait  | `cap`        | Kept. Short, distinct, part of Fides identity.         |
| Type alias        | `type`       | Rust/Go/Scala. Universal.                              |
| Import            | `use`        | Kept. Short, Rust/Zig convention. Already familiar.    |
| Uninit var        | `var x: T;`  | Zig/C/Carbon. No extra keyword. Definite assignment.   |
| Semicolons        | Required     | Rust/C/Go. Explicit, unambiguous.                      |
| Closures          | `fn(x) { }`  | Rust-style. Consistent with function keyword.          |
| Attributes        | `@...`       | Current Fides. Short, distinct from comments.          |
| String interp     | `f"..."`     | Python f-string. AI-familiar, explicit.                |
| References        | `&`/`&mut`   | Rust. Universal, clear.                                |
| Pointers          | `*const`/`*mut`| Rust. Explicit mutability in type.                   |
| Pattern matching  | `match`      | Rust/Swift. Already correct.                           |
| Struct/Enum       | `struct`/`enum`| Universal. Already correct.                         |
| Visibility        | `pub`        | Rust. Short, clear.                                    |
| Impl blocks       | `impl`       | Rust. Already correct.                                 |
| Comptime          | `comptime`   | Zig. Already correct.                                  |
| Defer             | `defer`      | Go/Zig. Already correct.                               |
| Async/Await       | `async`/`await`| Universal. Already correct.                          |
| Spawn             | `spawn`      | Go/Zig. Already correct.                               |
| Region            | `region`     | Fides-unique. Fits the concept.                        |
| Effect handlers   | `handle`/`with`| Fides-unique. Fits the concept.                      |
| Move              | `move`       | Rust. Already correct.                                 |
| Cast              | `as`         | Rust/Python/TS. Already correct.                       |
| FFI               | `extern`     | Rust/C/Zig. Already correct.                           |
| Dynamic dispatch  | `dyn`        | Rust. Already correct.                                 |
| Module            | `module`     | Swift/Zig/Odin. Already correct.                       |
| Static            | `static`     | Rust/C. Already correct.                               |
| Where             | `where`      | Rust/Haskell. Already correct.                         |
| Try               | `try`        | C++/Java/JS. Already correct.                          |
| Loop              | `loop`       | Rust. Already correct.                                 |
| Error operator    | `?`          | Rust. Already correct.                                 |
