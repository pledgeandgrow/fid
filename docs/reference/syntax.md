# Fides Language Syntax Specification

**Version:** 0.5.0
**Status:** Phase 4D — Direct PE/ELF64 emitters (x86_64) + MASM backend + C codegen backend + WASM/SPIR-V backends + multi-arch assembly (x86-64/ARM64/RISC-V) + IR interpreter + PIR pipeline (L1/L2/L3)

> **Implementation Status:** This document is the full language specification. Not all features are implemented yet — sections marked with implementation status (✅ working / ⚠️ partial / ❌ parsed-only or planned). See `features.md` for the authoritative feature matrix and `docs/architecture.md` for the compiler pipeline.

## 1. Design Philosophy

Fides is a systems programming language designed for safety, clarity, and modern AI-assisted development. It borrows proven concepts from Rust while introducing several innovations that address Rust's pain points.

### 1.1 Core Principles

1. **Safe by default** — Bounds checks, ownership, and capabilities are enforced by default. The `unsafe { }` block exists for opting out of bounds checks (e.g. inside stdlib `core.fd`), but ordinary user code is safe without explicit annotation.
2. **No hidden control flow** — No exceptions. All effects (IO, async, errors) are explicit in function signatures.
3. **No macros** — Compile-time code execution via `comptime` blocks. String interpolation via `f"..."`. No macro system at all.
4. **Region inference** — The compiler deduces memory regions (lifetimes) automatically. Explicit annotations are rare.
5. **Capability-based security** — No ambient authority. File, network, and hardware access require explicit capabilities.
6. **AI-friendly syntax** — Syntax choices prioritize AI model comprehension. Familiar patterns from Rust, Python, Swift.
7. **One keyword per concept** — `fn` for all functions (named, anonymous, async, comptime). `val` for all variables. `const` for constants. `cap` for traits/capabilities.

### 1.2 What Fides Takes From Rust

- Ownership and borrowing (`&` / `&mut`)
- Algebraic enums and pattern matching (`match` / `=>`)
- Traits and impl blocks
- `Result<T, E>` + `?` operator
- `@attribute` syntax (Fides-native; `#[...]` also parsed for `#[test]`)
- `pub` visibility
- `<T>` generics with trait bounds
- `for x in iter` loops
- `..` and `..=` ranges
- `&str` / `String` string model
- `[T; N]` / `Vec<T>` / `&[T]` collection model

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
| Lifetimes | `'a`, `'b` explicit | Region inference (rarely explicit) |
| Macros | `macro_rules!`, proc macros | `comptime` blocks (no macros) |
| Unsafe code | `unsafe { }` blocks | `unsafe { }` blocks (skip bounds checks — used in stdlib `core.fd`) |
| Error handling | `Result<T, E>` only | `Result<T, E>` + effect handlers (`!Effect`) |
| Async | `async fn` / `.await` | `async fn` / `.await` (sugar over effects) |
| Compile-time code | Procedural macros | `comptime { }` blocks |
| Hot reload | Not supported | Built-in (`fidesc watch`) |
| Capability security | Not built-in | Core language feature |

---

## 2. Lexical Structure

### 2.1 Keywords

```
fn          val          const          mut         pub         struct
enum        cap         impl        use         match       type
if          else        for         while       loop
return         break       continue    async       await
comptime    in          as          where       region
spawn       defer       move        static      try
handle      do          interrupt   extern      dyn         module
with
true        false
```

**Note:** `none`, `some`, `ok`, `err` are NOT keywords — they are enum variant names from the standard library (`Option` and `Result`). They are listed here for reference only and are not in the keyword block above.

**Keyword reference** (from `src/token.rs`):

| Keyword | Purpose |
|---------|---------|
| `fn` | Function declaration (named, anonymous, async, comptime) |
| `val` | Variable declaration (`var` for mutable) |
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
| `async` / `await` | Async functions (✅ implemented — eager evaluation, OS thread model; state machine transform pending) |
| `try` | Try blocks for error handling |

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
42          // i32 (inferred)
42i64       // explicit i64
3.14        // f64
3.14f32     // explicit f32
true        // bool
false       // bool
"hello"     // &str
f"hello"    // interpolated string (f-string)
b"bytes"    // &[u8]
'c'         // char
[1, 2, 3]   // array literal
```

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
f"Pi = {3.14159:.2}"                // format specifier → "Pi = 3.14"
f"Hex: {x:#x}"                      // hex format → "Hex: 0x5"
f"{{escaped}}"                      // literal braces → "{escaped}"
```

---

## 3. Types

### 3.1 Primitive Types

```
Integers:   i8  i16  i32  i64  i128  isize
            u8  u16  u32  u64  u128  usize
Floats:     f32  f64
Boolean:    bool
Character:  char
String:     str (used as &str)  String (owned)
Byte:       u8  &[u8]
Unit:       () (void/empty)
Never:      ! (diverges / never returns)
```

### 3.2 Compound Types

```fides
// Array — fixed size, stack allocated
[T; N]          // type
[1, 2, 3]       // literal

// Slice — borrowed view into array/vec
&[T]            // borrowed slice

// Vector — growable, heap allocated
Vec<T>          // type
vec![1, 2, 3]   // literal (function, not macro)

// Tuple
(T1, T2, T3)    // type
(a, b, c)       // literal

// Function type
fn(x: i32) -> i32   // type signature
```

### 3.3 Custom Types

```fides
// Struct — product type
struct Point {
    x: f64,
    y: f64,
}

// Tuple struct
struct Color(u8, u8, u8);

// Unit struct
struct Empty;

// Enum — sum type (algebraic)
enum Shape {
    Circle { radius: f64 },
    Rectangle { width: f64, height: f64 },
    Triangle(f64, f64, f64),    // tuple variant
    Point,                       // unit variant
}

// Generic type
struct HashMap<K, V> {
    buckets: Vec<Option<(K, V)>>,
}

// type alias
type Score = i32;
```

### 3.4 Standard Library Types

```fides
Option<T>        // enum Option<T> { Some(i64), None }  — uniform i64 representation
Result<T, E>     // enum Result<T, E> { Ok(i64), Err(i64) }  — uniform i64 representation
Vec<T>           // growable array
String           // owned string
HashMap<K, V>    // hash map
Box<T>           // heap indirection (enables recursive types)
Rc<T>            // reference-counted smart pointer (manual refcount)
Arc<T>           // atomic reference-counted smart pointer (manual refcount)
```

**Note:** `Option<T>` and `Result<T, E>` are declared with type parameters in
`stdlib/core.fd`. The type parameters are syntactic — the internal representation
uses uniform `i64` for all variant data. This means `Option` and `Option<T>` are
compatible types, and methods like `.unwrap()` work on both.

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

#### String Methods

```fides
val s: String = "Hello";

s.len() -> i64              // length in bytes
s.is_empty() -> bool        // true if length == 0
s.push_str(other: String) -> String  // concatenation
s.to_string() -> String     // identity conversion
s.slice(start: i64, end: i64) -> String  // substring
s.contains(needle: String) -> bool       // substring search
s.starts_with(prefix: String) -> bool
s.ends_with(suffix: String) -> bool
s.trim() -> String          // strip whitespace
s.split(delim: String) -> String         // split on delimiter
s.replace(old: String, new: String) -> String
s.char_at(idx: i64) -> String           // single char as string
s.to_uppercase() -> String
s.to_lowercase() -> String
s.parse_int() -> i64        // parse string as integer
s.find(needle: String) -> i64           // index or -1
s.rfind(needle: String) -> i64          // last index or -1
s.repeat(n: i64) -> String  // repeated string
s.bytes() -> i64            // byte count
s.clone() -> String         // deep copy
```

#### Vec Methods

```fides
val v: Vec<i64> = vec![1, 2, 3];

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
val m: HashMap<String, i64> = hashmap![];

m.insert(key: String, val: i64)  // insert/update
m.get(key: String) -> i64        // lookup (0 if missing)
m.get_or(key: String, default: i64) -> i64  // lookup with default
m.contains(key: String) -> bool  // key exists
m.remove(key: String)            // delete by key
m.len() -> i64                   // entry count
m.is_empty() -> bool             // true if empty
m.clear()                        // remove all
m.keys() -> Vec<String>          // all keys
m.values() -> Vec<i64>           // all values
m.clone() -> HashMap<String, i64>  // deep copy
```

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

// Generic with cap bound
fn max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

// Generic with where clause
fn merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone, U: Clone
{
    // ...
}

// Async function
async fn fetch(url: &str) -> Result<str, NetError> {
    val response = http_get(url).await?;
    Ok(response.body)
}

// Function with effect handlers (advanced)
fn read_config(path: &str) -> Config !FileIO + LogIO {
    val data = read_file(path)?;
    handle LogIO::Info(f"Loaded {path}");
    parse_config(data)
}
```

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
val nums = vec![1, 2, 3];
val doubled = nums.map(fn(x: i32) -> i32 => x * 2);

// Closure with no parameters
val greet = fn() => println("Hello!");

// Async closure
val task = async fn() {
    fetch("https://example.com").await
};
```

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

// Destructuring
val Point { x, y } = point;
val (a, b, c) = tuple;
val Some(value) = option;  // pattern match in let

// Shadowing (allowed, type can change)
val x = 5;
val x = "now a string";  // shadows previous x
```

### 5.2 Constants

```fides
// Compile-time constant
const MAX_PLAYERS: i32 = 64;
const PI: f64 = 3.14159265358979;
const GREETING: &str = "Hello, Fides!";

// Static (single memory location)
static COUNTER: AtomicI32 = AtomicI32::new(0);
```

### 5.3 Move Semantics

```fides
// Copy types (i32, bool, f64, etc.) — copied by default
val a = 5;
val b = a;      // a is still valid (i32 is Copy)
println(a);   // works

// Move types (String, Vec, etc.) — moved by default
val s1 = String::from("hello");
val s2 = s1;        // s1 is moved
// println(s1);   // ERROR: s1 was moved

// Borrow instead of move
val s3 = String::from("world");
val s4 = &s3;       // borrow (s3 still valid)
println(s3);      // works
println(s4);      // works
```

---

## 6. Ownership and Borrowing

### 6.1 References

```fides
// Shared reference (immutable borrow)
fn len(s: &str) -> usize {
    s.len()
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
fn longest(x: &str, y: &str) -> &str {
    if x.len() > y.len() { x } else { y }
}

// When explicit (rare), use region blocks:
fn store(data: &str) -> &str {
    region r {
        val cached = data.to_string();
        &cached
    }
}
```

### 6.3 Ownership Rules

1. Each value has exactly one **owner**
2. When the owner goes out of scope, the value is dropped (RAII)
3. You can borrow immutably (`&`) with unlimited readers
4. You can borrow mutably (`&mut`) with exactly one writer
5. Borrows must not outlive the owner (enforced by region inference)

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
// Basic match
match color {
    Color::Red => 0xFF0000,
    Color::Green => 0x00FF00,
    Color::Blue => 0x0000FF,
}

// With bindings
match option {
    Some(value) => println(f"Got {value}"),
    None => println("Nothing"),
}

// With guards
match score {
    s if s >= 90 => "A",
    s if s >= 80 => "B",
    s if s >= 70 => "C",
    _ => "F",
}

// With struct destructuring
match shape {
    Shape::Circle { radius } => f"Circle(r={radius})",
    Shape::Rectangle { width, height } => f"Rect({width}x{height})",
    Shape::Triangle(a, b, c) => f"Tri({a},{b},{c})",
    Shape::Point => "Point",
}

// Match on ranges
match x {
    0..=9 => "single digit",
    10..=99 => "double digit",
    100..=999 => "triple digit",
    _ => "large",
}

// Match must be exhaustive (compiler enforces)
```

### 7.3 Loops

```fides
// For-in loop (iterator based)
for x in nums {
    println(x);
}

// For with index
for (i, x) in nums.iter().enumerate() {
    println(f"{i}: {x}");
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

// Break with value
val result = loop {
    val input = read_line();
    if input == "done" {
        break input;  // returns "done"
    }
};
```

### 7.4 Return

```fides
fn foo(x: i32) -> i32 {
    if x < 0 {
        return 0;     // early return
    }
    x * 2             // implicit return (last expression)
}
```

### 7.5 Defer

Fides introduces `defer` for cleanup (inspired by Go, but with RAII):

```fides
fn process_file(path: &str) -> Result<(), FileError> {
    val file = open(path)?;
    defer close(file);          // runs when function exits (even on error)

    val data = read(file)?;
    process(data)?;
    Ok(())
    // close(file) runs here automatically
}
```

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

// Shorthand (when variable name matches field name)
val x = 3.0;
val y = 4.0;
val p = Point { x, y };     // shorthand for Point { x: x, y: y }

// Update syntax (copy remaining fields from another)
val p2 = Point { x: 5.0, ..fd };   // x=5.0, y=4.0 (from p)
```

### 8.3 Impl Blocks

```fides
impl Point {
    // Associated function (constructor)
    fn new(x: f64, y: f64) -> Point {
        Point { x, y }
    }

    // Method (takes &self)
    fn distance(&self, other: &Point) -> f64 {
        val dx = self.x - other.x;
        val dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }

    // Mutable method
    fn translate(&mut self, dx: f64, dy: f64) {
        self.x += dx;
        self.y += dy;
    }

    // Associated constant
    const ORIGIN: Point = Point { x: 0.0, y: 0.0 };
}
```

### 8.4 Tuple Structs

```fides
struct Color(u8, u8, u8);

val red = Color(255, 0, 0);
val r = red.0;    // 255
val g = red.1;    // 0
val b = red.2;    // 0
```

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
    Move { x: i32, y: i32 },       // struct variant
    Write(str),                     // tuple variant
    ChangeColor(u8, u8, u8),       // tuple variant
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
    fn process(&self) {
        match self {
            Message::Quit => println("Quitting"),
            Message::Move { x, y } => println(f"Move to ({x}, {y})"),
            Message::Write(text) => println(text),
            Message::ChangeColor(r, g, b) => println(f"Color: {r},{g},{b}"),
        }
    }
}
```

### 9.2 Pattern Matching

```fides
// Exhaustive match (compiler enforces all cases)
match msg {
    Message::Quit => 0,
    Message::Move { x, y } => x + y,
    Message::Write(text) => text.len(),
    Message::ChangeColor(r, g, b) => (r as i32 + g as i32 + b as i32),
}

// Wildcard
match x {
    0 => "zero",
    _ => "nonzero",
}

// Or-patterns
match key {
    "q" | "quit" => exit(),
    "h" | "help" => show_help(),
    _ => handle_key(key),
}

// Nested patterns
match point {
    Point { x: 0.0, y: 0.0 } => "origin",
    Point { x: 0.0, y } => f"on y-axis at {y}",
    Point { x, y: 0.0 } => f"on x-axis at {x}",
    Point { x, y } => f"at ({x}, {y})",
}

// Binding with @
match age {
    n @ 0..=12 => f"Child ({n})",
    n @ 13..=19 => f"Teen ({n})",
    n @ 20..=150 => f"Adult ({n})",
    _ => "Invalid age",
}
```

---

## 10. Traits (cap)

### 10.1 Trait (cap) Definitions

```fides
// Basic cap
cap Drawable {
    fn draw(&self);
    fn area(&self) -> f64;
}

// cap with default implementation
cap Greet {
    fn greet(&self) {
        println(f"Hello from {self.name()}");
    }

    fn name(&self) -> &str;
}

// Generic cap
cap Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;
}

// cap bounds
cap Ord: Eq + PartialOrd {
    fn cmp(&self, other: &Self) -> Ordering;
}
```

### 10.2 Trait (cap) Implementations

```fides
impl Drawable for Point {
    fn draw(&self) {
        render_pixel(self.x, self.y);
    }

    fn area(&self) -> f64 {
        0.0  // points have no area
    }
}

impl Iterator for Counter {
    type Item = i32;

    fn next(&mut self) -> Option<i32> {
        if self.count < self.max {
            self.count += 1;
            Some(self.count)
        } else {
            None
        }
    }
}
```

### 10.3 Trait (cap) Bounds

```fides
// In function signatures
fn sort<T: Ord>(arr: &mut [T]) {
    // ...
}

// With where clause (for complex bounds)
fn merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone + Ord,
          U: Clone + Hash
{
    // ...
}

// impl cap (abstract types)
fn make_iter() -> impl Iterator<Item = i32> {
    0..10
}
```

---

## 11. Error Handling

### 11.1 Result Type

```fides
// Returning Result
fn parse_int(s: &str) -> Result<i32, ParseError> {
    // ...
    Ok(42)
}

// Handling with match
match parse_int("42") {
    Ok(n) => println(f"Got {n}"),
    Err(e) => println(f"Error: {e}"),
}

// Propagating with ? operator
fn read_config(path: &str) -> Result<Config, ConfigError> {
    val data = read_file(path)?;        // ? propagates Err
    val config = parse_config(data)?;   // ? propagates Err
    Ok(config)
}

// ? with different error types (auto-conversion)
fn load() -> Result<Data, AppError> {
    val file = open("data.txt")?;       // FileError → AppError (via From)
    val content = read(file)?;          // FileError → AppError
    Ok(parse(content))
}
```

### 11.2 Effect Handlers (⚠️ Partial — Phase 4D)

> **Implementation status:** Effect annotations (`!IO`, `!Net`, `!Spawn`, `!Alloc`, `!{}`) and effect inference via fixpoint iteration over the call graph are ✅ working. The `handle { do } with` syntax parses and runs, but **full continuation semantics are pending** — effect handlers currently execute their handler bodies but do not support resumable continuations.

```fides
// Effect type in signature
fn read_file(path: &str) -> str !FileIO {
    handle FileIO::Read(path)
}

// Multiple effects
fn process() -> Result<Data, Error> !FileIO + LogIO + TimeIO {
    val start = handle TimeIO::Now;
    handle LogIO::Info("Starting process");

    val data = handle FileIO::Read("data.txt");
    val parsed = parse(data)?;

    val elapsed = handle TimeIO::Now - start;
    handle LogIO::Info(f"Done in {elapsed}ms");

    Ok(parsed)
}

// Handling effects
fn main() {
    handle {
        process()
    } with {
        FileIO::Read(path) => std::fs::read_to_string(path),
        LogIO::Info(msg) => println(msg),
        TimeIO::Now => std::time::Instant::now(),
    }
}
```

### 11.3 Async as Effect Sugar

```fides
// This:
async fn fetch(url: &str) -> Result<str, NetError> {
    val resp = http_get(url).await?;
    Ok(resp.body)
}

// Desugars to:
fn fetch(url: &str) -> Result<str, NetError> !Async {
    val resp = handle Async::Wait(http_get(url))?;
    Ok(resp.body)
}
```

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
// Import specific items
use std::io::println;
use std::fs::{read, write};

// Import module (access via path)
use std::collections;

// Rename
use std::io::println as print;

// Glob import (discouraged but allowed)
use std::prelude::*;
```

### 12.3 Visibility

```fides
// Private by default
fn helper() { ... }           // private to module

// Public
pub fn api() { ... }          // accessible from outside

// Restricted visibility
pub(crate) fn internal() { ... }    // crate-internal only
pub(super) fn child_api() { ... }   // parent module only
```

---

## 13. Generics

### 13.1 Generic Functions

```fides
fn identity<T>(x: T) -> T {
    x
}

fn max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

fn merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone + Ord,
          U: Clone + Hash
{
    // ...
}
```

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

struct HashMap<K: Hash + Eq, V> {
    buckets: Vec<Option<(K, V)>>,
}
```

### 13.3 Associated Types

```fides
cap Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;
}

impl Iterator for Range {
    type Item = i32;

    fn next(&mut self) -> Option<i32> {
        // ...
    }
}
```

---

## 14. Compile-Time Code (comptime) — ✅ Working

Fides replaces macros with `comptime` blocks — real code that runs at compile time. Implemented in `src/comptime.rs`.

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
comptime {
    val name = type_name<T>;           // type name as string
    val is_struct = is_struct<T>;      // true if T is a struct
    val is_enum = is_enum<T>;          // true if T is an enum
    val fields = field_names<T>;       // Vec of field name strings
    val field_types = field_types<T>;  // Vec of type descriptors
    val variants = enum_variants<T>;   // Vec of variant name strings
}
```

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

> **Implementation status:** Fides uses `@attribute` syntax (the Fides-native form). The parser also accepts `#[...]` for `#[test]` compatibility, but `@` is the canonical syntax. `#[test]` is parsed but **not executed** by `fidesc test` (❌ — `fidesc test` runs file-based tests, not `#[test]` functions). `@derive(Debug/Clone/Serialize/Deserialize)` is ✅ implemented (Phase 7.7). `@cfg(platform=...)` is ✅ implemented (conditional compilation). Other attributes shown below are the planned syntax.

```fides
// Standard attributes
#[test]
fn test_addition() { assert(1 + 1 == 2); }        // ❌ parsed, not executed by fidesc test

#[test]
#[should_panic]
fn test_oob() { val _ = [1, 2, 3][10]; }            // ❌ parsed, not executed by fidesc test

@derive(Debug, Clone)                               // ✅ implemented (Phase 7.7)
struct Point { x: f64, y: f64 }

@derive(Serialize, Deserialize)                     // ✅ implemented (Phase 7.7)
struct Config { /* ... */ }

@cfg(platform = "linux")                            // ✅ implemented
fn linux_only() { /* ... */ }

@inline
fn fast_add(a: i32, b: i32) -> i32 { a + b }

@deprecated("use new_function() instead")
fn old_function() { /* ... */ }
```

---

## 16. Concurrency (✅ Implemented — Phase 7.6)

> **Implementation status:** `async`, `await`, and `spawn` are ✅ implemented (eager evaluation, OS thread model). `async fn` declares an async function, `await <expr>` waits for a Future, `spawn <expr>` creates a new thread. State machine transform for true cooperative scheduling is pending. Effect annotations (`!Spawn`, `!Async`) and effect inference are ✅ working.

### 16.1 Async / Await (✅ Implemented)

```fides
// Async function
async fn fetch_data(url: &str) -> Result<str, NetError> {
    val resp = http_get(url).await?;
    Ok(resp.body)
}

// Await on any async expression
async fn main() {
    val data = fetch_data("https://api.example.com").await?;
    println(data);
}

// Concurrent fetches
async fn fetch_all(urls: Vec<&str>) -> Vec<Result<str, NetError>> {
    val tasks = urls.map(|url| spawn async { fetch_data(url).await });
    tasks.await_all()
}
```

### 16.2 Spawning Tasks

```fides
// Spawn a task (runs concurrently)
val task = spawn async {
    fetch_data("https://example.com").await
};

// Do other work...
process_local();

// Wait for result
val result = task.await;
```

### 16.3 Synchronization (❌ Planned)

> **Note:** `Rc<T>` and `Arc<T>` are implemented (compiler builtin types with manual refcount management: `new`, `clone`, `drop`, `get`, `count`). Channels are available via `stdlib/channel.fd`. `Mutex<T>` is not yet implemented. The syntax below is the planned design for `Mutex<T>`.

```fides
// Mutex (planned)
use std::sync::Mutex;

val data = Mutex::new(0);
{
    var guard = data.lock();
    *guard += 1;
}  // lock released here

// Channels (for message passing, planned)
use std::sync::channel;

val (tx, rx) = channel();
spawn fn() {
    tx.send(42);
};
val received = rx.recv();
```

---

## 17. Capability-Based Security — ✅ Working

Fides has no ambient authority. File, network, and hardware access require explicit capabilities, enforced at compile time with zero runtime overhead (capability elision).

> **Implementation status:** ✅ `cap` declarations, `use ... with { IO, FS, Net, Spawn }` capability grants, transitive propagation through the call graph, compile-time denial, and zero-runtime-overhead capability elision all work.

### 17.1 Capability Grants in Imports

```fides
// Grant capabilities to an imported module
use network_lib with { Net, IO };       // network_lib can use Net + IO
use file_lib with { FS };               // file_lib can use FS only
use pure_lib;                           // pure_lib gets no capabilities
```

### 17.2 Capability Declarations

```fides
// Declare a capability/trait
cap Database {
    fn query(&self, sql: &str) -> Result<Vec<Row>, DbError>;
    fn execute(&self, sql: &str) -> Result<(), DbError>;
}

// Implement it
impl Database for InMemoryDB {
    fn query(&self, sql: &str) -> Result<Vec<Row>, DbError> {
        // ...
    }
    fn execute(&self, sql: &str) -> Result<(), DbError> {
        // ...
    }
}
```

### 17.3 Compile-Time Enforcement

```fides
// Without capability grant — this won't compile
fn bad() {
    // ERROR: no Net capability in scope
    val response = http_get("https://example.com");
}

// With capability — compiles fine
fn good() !Net {
    val response = http_get("https://example.com");
    // ...
}
```

### 17.4 Transitive Capabilities

Capabilities propagate through the call graph. If function A calls function B which requires `Net`, then A must also have `Net` (or declare it). The compiler verifies this at compile time — no runtime checks needed.

---

## 18. Hot Reload — ✅ Working

Fides supports hot-swapping functions at runtime via `fidesc watch`. Implemented in `src/hotreload.rs` — native JIT (ELF64) with function-level change detection via L2 IR hashing.

```fides
// Any function can be hot-reloaded when using `fidesc watch`
pub fn handle_request(req: &Request) -> Response {
    // This function can be replaced at runtime
    // without restarting the process
    process(req)
}
```

```bash
# Start watching — compiles to native ELF64, runs, and watches for changes
fidesc watch main.fd

# When you edit and save a function, only that function is recompiled
# and swapped in the running process — sub-second iteration
```

> **Note:** Hot reload for FidesOS (the OS project) uses the same mechanism with OS-specific integration. The `fidesc watch` command works on any Linux system.

---

## 19. Standard Library Prelude

Items available without explicit import (from `stdlib/core.fd`, auto-included):

```fides
// Types
Option<T>     Some(T)     None
Result<T, E>  Ok(T)       Err(E)
Vec<T>        String      &str

// Functions
println       print       eprintln
vec!          format      dbg
spawn         handle      // spawn: parsed only, handle: effect handlers

// Note: `print` is polymorphic (accepts int, float, bool, str, vec, map)
// and always appends a trailing newline (like Rust's `println!`).
// `println` is an alias for `print` with explicit newline semantics.

// Traits (caps)
Clone         Copy        Debug
Default       Display     Eq + Ord
Hash          Iterator    IntoIterator
From          Into        TryFrom + TryInto

// Macros (none — Fides has no macros)
// All of the above are functions or types
```

> **Note:** `Box<T>`, `Rc<T>`, and `Arc<T>` are implemented as compiler builtin types (not stdlib modules). `Box<T>` enables recursive types. `Rc<T>` / `Arc<T>` provide manual reference counting. Channels are available via `stdlib/channel.fd`. `Mutex<T>` is not yet implemented. See section 3.4a for the full list of stdlib modules.

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
use std::io::println;
use std::fs::read_to_string;
use std::collections::HashMap;

@derive(Debug, Clone)
struct Config {
    name: str,
    version: str,
    settings: HashMap<str, str>,
}

impl Config {
    fn new(name: &str) -> Config {
        Config {
            name: name.to_string(),
            version: "0.1.0".to_string(),
            settings: HashMap::new(),
        }
    }

    fn set(&mut self, key: &str, value: &str) {
        self.settings.insert(key.to_string(), value.to_string());
    }

    fn get(&self, key: &str) -> Option<&str> {
        self.settings.get(key).map(|v| v.as_str())
    }
}

enum LoadError {
    FileNotFound,
    ParseError(str),
}

fn load_config(path: &str) -> Result<Config, LoadError> {
    val content = read_to_string(path)
        .map_err(|_| LoadError::FileNotFound)?;

    var config = Config::new("app");
    for line in content.lines() {
        val parts: Vec<&str> = line.splitn(2, '=').collect();
        if parts.len() == 2 {
            config.set(parts[0].trim(), parts[1].trim());
        }
    }

    Ok(config)
}

#[test]
fn test_config() {
    var config = Config::new("test");
    config.set("key", "value");
    assert_eq(config.get("key"), Some("value"));
    assert_eq(config.get("missing"), None);
}

#[test]
fn test_config_debug() {
    val config = Config::new("app");
    val debug = format(f"{config:?}");
    assert(debug.contains("Config"));
}

pub fn main() -> Result<(), LoadError> {
    val config = load_config("pacte.toml")?;

    println(f"Loaded: {config.name} v{config.version}");
    println(f"Settings: {config.settings.len()} entries");

    for (key, value) in &config.settings {
        println(f"  {key} = {value}");
    }

    Ok(())
}
```

---

## 22. Syntax Summary Cheat Sheet

```
// Functions
fn name(params) -> ReturnType { body }
pub fn name(params) -> ReturnType { body }
async fn name(params) -> ReturnType { body }
fn<T: cap> name(x: T) -> T { body }

// Variables
val x = value;
var x = value;
val x: Type = value;

// Control flow
if cond { } else if cond { } else { }
match value { Pattern => expr, _ => default }
for x in iter { }
while cond { }
loop { break value; }

// Types
struct Name { field: Type }
enum Name { Variant, Variant(T), Variant { field: T } }
cap Name { fn method(&self); }
impl Name for Type { fn method(&self) { } }

// Generics
fn name<T: cap>(x: T) -> T { }
struct Name<T, U> { }

// Error handling
Result<T, E>    Ok(value)    Err(error)
expr?           // propagate error

// Strings
"literal"       f"interpolated {expr}"

// Collections
[T; N]          &[T]         Vec<T>
[a, b, c]       0..10        0..=10

// Modules
use module::item;
pub fn name() { }
pub(crate) fn name() { }

// Other
@attribute
comptime { }
defer expr;
spawn async { }
expr.await
```

---

## 22. LSP Server (Editor Integration)

Fides includes a built-in Language Server Protocol server — no external tools
required.

### Starting the LSP Server

```bash
fidesc lsp
```

This starts a JSON-RPC 2.0 server over stdio. Configure your editor to use
`fidesc lsp` as the language server command.

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
- 8 harness tests pass
- 153/153 normal tests pass on interpreter + PE emitter, 153/153 on C codegen

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
| `defer expr;`            | Deferred cleanup            |
| `spawn expr;`            | Spawn task                  |
| `async`/`await`          | Async/await                 |
| `region { }`             | Region block                |
| `handle/with`            | Effect handlers             |
| `move x`                 | Move semantics              |
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
| `where T: Bound`         | Trait bounds                |
| `try { }`                | Try block                   |
| `;` (semicolons)         | Required                    |
| `//` and `/* */`         | Comments                    |
| `@attribute`             | Attribute syntax (canonical)|
| `#[test]`                | Test attribute (also parsed, not executed) |
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
