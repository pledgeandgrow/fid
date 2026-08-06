# Pledge Language Syntax Specification

**Version:** 0.4.0
**Status:** Phase 1 Bootstrap — Direct PE emitter (x86_64) + MASM backend + C codegen backend + IR interpreter

> **Implementation Status:** This document is the full language specification. Not all features are implemented yet. See `README.md` for a breakdown of what is working end-to-end (interpreter + native x86_64 + C codegen) vs. parsed-only vs. planned.

## 1. Design Philosophy

Pledge is a systems programming language designed for safety, clarity, and modern AI-assisted development. It borrows proven concepts from Rust while introducing several innovations that address Rust's pain points.

### 1.1 Core Principles

1. **Safe by default** — No `unsafe` keyword. Safety is enforced by the ty system, ownership, and capabilities.
2. **No hidden control flow** — No exceptions. All effects (IO, async, errors) are explicit in function signatures.
3. **No macros** — Compile-time code execution via `comptime` blocks. String interpolation via `f"..."`. No macro system at all.
4. **Region inference** — The compiler deduces memory regions (lifetimes) automatically. Explicit annotations are rare.
5. **Capability-based security** — No ambient authority. File, network, and hardware access require explicit capabilities.
6. **AI-friendly syntax** — Syntax choices prioritize AI model comprehension. Familiar patterns from Rust, Python, Swift.
7. **One keyword per concept** — `pf` for all functions (named, anonymous, async, comptime). `pv` for all variables. `pc` for constants. `cap` for traits/capabilities.

### 1.2 What Pledge Takes From Rust

- Ownership and borrowing (`&` / `&mut`)
- Algebraic enums and pattern matching (`match` / `=>`)
- Traits and impl blocks
- `Result<T, E>` + `?` operator
- `#[attribute]` syntax
- `pub` visibility
- `<T>` generics with trait bounds
- `for x in iter` loops
- `..` and `..=` ranges
- `&str` / `String` string model
- `[T; N]` / `Vec<T>` / `&[T]` collection model

### 1.3 What Pledge Does Differently

| Feature | Rust | Pledge |
|---------|------|--------|
| Function keyword | `fn` | `pf` |
| Variable keyword | `let` / `let mut` | `pv` / `pv mut` |
| Constant keyword | `const` | `pc` |
| Trait keyword | `trait` | `cap` |
| Return keyword | `return` | `ret` |
| Type alias keyword | `type` | `ty` |
| Closures | `\|x\| x * 2` | `pf(x) { x * 2 }` or `pf(x) => x * 2` |
| String formatting | `println!("{} + {}", a, b)` | `println(f"{a} + {b}")` |
| Lifetimes | `'a`, `'b` explicit | Region inference (rarely explicit) |
| Macros | `macro_rules!`, proc macros | `comptime` blocks (no macros) |
| Unsafe code | `unsafe { }` blocks | Not in user code (Framework only) |
| Error handling | `Result<T, E>` only | `Result<T, E>` + effect handlers (`!Effect`) |
| Async | `async fn` / `.await` | `async pf` / `.await` (sugar over effects) |
| Compile-time code | Procedural macros | `comptime { }` blocks |
| Hot reload | Not supported | Built-in (for PledgeOS) |
| Capability security | Not built-in | Core language feature |

---

## 2. Lexical Structure

### 2.1 Keywords

```
pf          pv          pc          mut         pub         struct
enum        cap         impl        use         match       ty
if          else        for         while       loop
ret         break       continue    async       await
comptime    in          as          where       region
spawn       defer       move        static
true        false       none        some        ok          err
```

**Note:** `none`, `some`, `ok`, `err` are not keywords — they are enum variant names from the standard library (`Option` and `Result`). They are listed here for reference.

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

```pledge
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

```pledge
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

```pledge
pv name = "World";
pv x = 5;
pv y = 10;

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

```pledge
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
pf(x: i32) -> i32   // type signature
```

### 3.3 Custom Types

```pledge
// Struct — product type
struct Point {
    x: f64,
    y: f64,
}

// Tuple struct
struct Color(u8, u8, u8);

// Unit struct
struct Empty;

// Enum — sum ty (algebraic)
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
ty Score = i32;
```

### 3.4 Standard Library Types

```pledge
Option<T>        // enum Option<T> { Some(T), None }
Result<T, E>     // enum Result<T, E> { Ok(T), Err(E) }
Vec<T>           // growable array
String           // owned string
HashMap<K, V>    // hash map
HashSet<T>       // hash set
Box<T>           // heap allocation
Rc<T>            // reference counted
Arc<T>           // atomically reference counted
```

### 3.5 Built-in Type Methods

#### String Methods

```pledge
pv s: String = "Hello";

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

```pledge
pv v: Vec<i64> = vec![1, 2, 3];

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

```pledge
pv m: HashMap<String, i64> = hashmap![];

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

```pledge
// Basic function
pf add(x: i32, y: i32) -> i32 {
    x + y
}

// Public function
pub pf main() {
    println("Hello, Pledge!");
}

// Function with no ret value (unit)
pf greet(name: &str) {
    println(f"Hello, {name}!");
}

// Generic function
pf identity<T>(x: T) -> T {
    x
}

// Generic with cap bound
pf max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

// Generic with where clause
pf merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone, U: Clone
{
    // ...
}

// Async function
async pf fetch(url: &str) -> Result<str, NetError> {
    pv response = http_get(url).await?;
    Ok(response.body)
}

// Function with effect handlers (advanced)
pf read_config(path: &str) -> Config !FileIO + LogIO {
    pv data = read_file(path)?;
    handle LogIO::Info(f"Loaded {path}");
    parse_config(data)
}
```

### 4.2 Closures

```pledge
// Full closure
pv double = pf(x: i32) -> i32 {
    x * 2
};

// Short-form closure (single expression, no braces)
pv square = pf(x: i32) -> i32 => x * x;

// Closure capturing environment
pv multiplier = 3;
pv multiply = pf(x: i32) -> i32 => x * multiplier;

// Closure as argument
pv nums = vec![1, 2, 3];
pv doubled = nums.map(pf(x: i32) -> i32 => x * 2);

// Closure with no parameters
pv greet = pf() => println("Hello!");

// Async closure
pv task = async pf() {
    fetch("https://example.com").await
};
```

### 4.3 Function Pointers

```pledge
// Function type
ty BinOp = pf(i32, i32) -> i32;

pv add: BinOp = pf(a: i32, b: i32) -> i32 => a + b;
pv result = add(5, 3);
```

---

## 5. Variables and Bindings

### 5.1 Variable Declaration

```pledge
// Immutable (default)
pv x = 5;
pv name = "Pledge";
pv point = Point { x: 3.0, y: 4.0 };

// Mutable
pv mut counter = 0;
counter += 1;

// With explicit type
pv x: i64 = 42;
pv mut buffer: Vec<u8> = Vec::new();

// Destructuring
pv Point { x, y } = point;
pv (a, b, c) = tuple;
pv Some(value) = option;  // pattern match in let

// Shadowing (allowed, ty can change)
pv x = 5;
pv x = "now a string";  // shadows previous x
```

### 5.2 Constants

```pledge
// Compile-time constant
pc MAX_PLAYERS: i32 = 64;
pc PI: f64 = 3.14159265358979;
pc GREETING: &str = "Hello, Pledge!";

// Static (single memory location)
static COUNTER: AtomicI32 = AtomicI32::new(0);
```

### 5.3 Move Semantics

```pledge
// Copy types (i32, bool, f64, etc.) — copied by default
pv a = 5;
pv b = a;      // a is still valid (i32 is Copy)
println(a);   // works

// Move types (String, Vec, etc.) — moved by default
pv s1 = String::from("hello");
pv s2 = s1;        // s1 is moved
// println(s1);   // ERROR: s1 was moved

// Borrow instead of move
pv s3 = String::from("world");
pv s4 = &s3;       // borrow (s3 still valid)
println(s3);      // works
println(s4);      // works
```

---

## 6. Ownership and Borrowing

### 6.1 References

```pledge
// Shared reference (immutable borrow)
pf len(s: &str) -> usize {
    s.len()
}

// Mutable reference (exclusive borrow)
pf push(buf: &mut Vec<i32>, val: i32) {
    buf.push(val);
}

// Taking a reference
pv x = 5;
pv r = &x;          // &i32
pv val = *r;        // dereference → 5

// Mutable reference
pv mut y = 10;
pv r = &mut y;      // &mut i32
*r += 5;           // y is now 15
```

### 6.2 Region Inference

Pledge deduces regions (lifetimes) automatically. You rarely need to annotate them.

```pledge
// Rust requires explicit lifetime:
//   fn longest<'a>(x: &'a str, y: &'a str) -> &'a str
// Pledge: compiler infers the region
pf longest(x: &str, y: &str) -> &str {
    if x.len() > y.len() { x } else { y }
}

// When explicit (rare), use region blocks:
pf store(data: &str) -> &str {
    region r {
        pv cached = data.to_string();
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

```pledge
// Statement form
if x > 0 {
    println("positive");
} else if x < 0 {
    println("negative");
} else {
    println("zero");
}

// Expression form (returns value)
pv sign = if x > 0 { 1 } else if x < 0 { -1 } else { 0 };

// With let
pv result = if condition {
    compute_value()
} else {
    default_value()
};
```

### 7.2 Match

```pledge
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

```pledge
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
pv mut i = 0;
while i < 10 {
    println(i);
    i += 1;
}

// Infinite loop
loop {
    pv input = read_line();
    if input == "quit" {
        break;
    }
    if input == "skip" {
        continue;
    }
    process(input);
}

// Break with value
pv result = loop {
    pv input = read_line();
    if input == "done" {
        break input;  // returns "done"
    }
};
```

### 7.4 Return

```pledge
pf foo(x: i32) -> i32 {
    if x < 0 {
        ret 0;     // early return
    }
    x * 2             // implicit return (last expression)
}
```

### 7.5 Defer

Pledge introduces `defer` for cleanup (inspired by Go, but with RAII):

```pledge
pf process_file(path: &str) -> Result<(), FileError> {
    pv file = open(path)?;
    defer close(file);          // runs when function exits (even on error)

    pv data = read(file)?;
    process(data)?;
    Ok(())
    // close(file) runs here automatically
}
```

---

## 8. Structs and Impl

### 8.1 Struct Definitions

```pledge
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

```pledge
// Full literal
pv p = Point { x: 3.0, y: 4.0 };

// Shorthand (when variable name matches field name)
pv x = 3.0;
pv y = 4.0;
pv p = Point { x, y };     // shorthand for Point { x: x, y: y }

// Update syntax (copy remaining fields from another)
pv p2 = Point { x: 5.0, ..p };   // x=5.0, y=4.0 (from p)
```

### 8.3 Impl Blocks

```pledge
impl Point {
    // Associated function (constructor)
    pf new(x: f64, y: f64) -> Point {
        Point { x, y }
    }

    // Method (takes &self)
    pf distance(&self, other: &Point) -> f64 {
        pv dx = self.x - other.x;
        pv dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }

    // Mutable method
    pf translate(&mut self, dx: f64, dy: f64) {
        self.x += dx;
        self.y += dy;
    }

    // Associated constant
    pc ORIGIN: Point = Point { x: 0.0, y: 0.0 };
}
```

### 8.4 Tuple Structs

```pledge
struct Color(u8, u8, u8);

pv red = Color(255, 0, 0);
pv r = red.0;    // 255
pv g = red.1;    // 0
pv b = red.2;    // 0
```

---

## 9. Enums and Pattern Matching

### 9.1 Enum Definitions

```pledge
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
    pf process(&self) {
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

```pledge
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

```pledge
// Basic cap
cap Drawable {
    pf draw(&self);
    pf area(&self) -> f64;
}

// cap with default implementation
cap Greet {
    pf greet(&self) {
        println(f"Hello from {self.name()}");
    }

    pf name(&self) -> &str;
}

// Generic cap
cap Iterator {
    ty Item;

    pf next(&mut self) -> Option<Self::Item>;
}

// cap bounds
cap Ord: Eq + PartialOrd {
    pf cmp(&self, other: &Self) -> Ordering;
}
```

### 10.2 Trait (cap) Implementations

```pledge
impl Drawable for Point {
    pf draw(&self) {
        render_pixel(self.x, self.y);
    }

    pf area(&self) -> f64 {
        0.0  // points have no area
    }
}

impl Iterator for Counter {
    ty Item = i32;

    pf next(&mut self) -> Option<i32> {
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

```pledge
// In function signatures
pf sort<T: Ord>(arr: &mut [T]) {
    // ...
}

// With where clause (for complex bounds)
pf merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone + Ord,
          U: Clone + Hash
{
    // ...
}

// impl cap (abstract types)
pf make_iter() -> impl Iterator<Item = i32> {
    0..10
}
```

---

## 11. Error Handling

### 11.1 Result Type

```pledge
// Returning Result
pf parse_int(s: &str) -> Result<i32, ParseError> {
    // ...
    Ok(42)
}

// Handling with match
match parse_int("42") {
    Ok(n) => println(f"Got {n}"),
    Err(e) => println(f"Error: {e}"),
}

// Propagating with ? operator
pf read_config(path: &str) -> Result<Config, ConfigError> {
    pv data = read_file(path)?;        // ? propagates Err
    pv config = parse_config(data)?;   // ? propagates Err
    Ok(config)
}

// ? with different error types (auto-conversion)
pf load() -> Result<Data, AppError> {
    pv file = open("data.txt")?;       // FileError → AppError (via From)
    pv content = read(file)?;          // FileError → AppError
    Ok(parse(content))
}
```

### 11.2 Effect Handlers (Advanced — Phase 4+)

```pledge
// Effect ty in signature
pf read_file(path: &str) -> str !FileIO {
    handle FileIO::Read(path)
}

// Multiple effects
pf process() -> Result<Data, Error> !FileIO + LogIO + TimeIO {
    pv start = handle TimeIO::Now;
    handle LogIO::Info("Starting process");

    pv data = handle FileIO::Read("data.txt");
    pv parsed = parse(data)?;

    pv elapsed = handle TimeIO::Now - start;
    handle LogIO::Info(f"Done in {elapsed}ms");

    Ok(parsed)
}

// Handling effects
pf main() {
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

```pledge
// This:
async pf fetch(url: &str) -> Result<str, NetError> {
    pv resp = http_get(url).await?;
    Ok(resp.body)
}

// Desugars to:
pf fetch(url: &str) -> Result<str, NetError> !Async {
    pv resp = handle Async::Wait(http_get(url))?;
    Ok(resp.body)
}
```

---

## 12. Modules and Imports

### 12.1 Module Declaration

```pledge
// File: src/net/http.p
pub module net::http;

pub struct Request { ... }
pub pf get(url: &str) -> Response { ... }

// Submodule
pub module net::http::client;
```

### 12.2 Imports

```pledge
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

```pledge
// Private by default
pf helper() { ... }           // private to module

// Public
pub pf api() { ... }          // accessible from outside

// Restricted visibility
pub(crate) pf internal() { ... }    // crate-internal only
pub(super) pf child_api() { ... }   // parent module only
```

---

## 13. Generics

### 13.1 Generic Functions

```pledge
pf identity<T>(x: T) -> T {
    x
}

pf max<T: Ord>(a: T, b: T) -> T {
    if a >= b { a } else { b }
}

pf merge<T, U>(left: Vec<T>, right: Vec<U>) -> Vec<(T, U)>
    where T: Clone + Ord,
          U: Clone + Hash
{
    // ...
}
```

### 13.2 Generic Types

```pledge
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

```pledge
cap Iterator {
    ty Item;

    pf next(&mut self) -> Option<Self::Item>;
}

impl Iterator for Range {
    ty Item = i32;

    pf next(&mut self) -> Option<i32> {
        // ...
    }
}
```

---

## 14. Compile-Time Code (comptime)

Pledge replaces macros with `comptime` blocks — real code that runs at compile time.

### 14.1 Comptime Blocks

```pledge
// Compile-time evaluation
pc MAX_SIZE: i32 = comptime {
    pv config = parse_env("PLEDGE_MAX_SIZE");
    config.unwrap_or(1024)
};

// Compile-time code generation
comptime {
    // Generate code based on build configuration
    for item in build_config().items {
        generate_handler(item);
    }
}

// Comptime function
pf comptime fibonacci(n: i32) -> i32 {
    if n <= 1 { n } else { fibonacci(n - 1) + fibonacci(n - 2) }
}

pc FIB_10: i32 = comptime fibonacci(10);  // 55
```

### 14.2 Comptime vs Macros

| Feature | Rust Macros | Pledge comptime |
|---------|------------|-----------------|
| Syntax | `macro_rules!` or proc macros | `comptime { }` blocks |
| Language | Macro DSL or Rust | Pledge itself |
| Debugging | Difficult | Standard debugger |
| Error messages | Often cryptic | Compiler-generated, clear |
| AI generation | Hard (macro DSL) | Easy (it's just Pledge) |

---

## 15. Attributes

```pledge
// Standard attributes
#[test]
pf test_addition() { assert(1 + 1 == 2); }

#[test]
#[should_panic]
pf test_oob() { pv _ = [1, 2, 3][10]; }

#[test]
#[ignore]
pf slow_test() { /* ... */ }

#[derive(Debug, Clone, PartialEq)]
struct Point { x: f64, y: f64 }

#[inline]
pf fast_add(a: i32, b: i32) -> i32 { a + b }

#[deprecated("use new_function() instead")]
pf old_function() { /* ... */ }

// Custom attributes (for tools and frameworks)
#[serde(rename_all = "camelCase")]
struct Config { /* ... */ }
```

---

## 16. Concurrency

### 16.1 Async / Await

```pledge
// Async function
async pf fetch_data(url: &str) -> Result<str, NetError> {
    pv resp = http_get(url).await?;
    Ok(resp.body)
}

// Await on any async expression
async pf main() {
    pv data = fetch_data("https://api.example.com").await?;
    println(data);
}

// Concurrent fetches
async pf fetch_all(urls: Vec<&str>) -> Vec<Result<str, NetError>> {
    pv tasks = urls.map(|url| spawn async { fetch_data(url).await });
    tasks.await_all()
}
```

### 16.2 Spawning Tasks

```pledge
// Spawn a task (runs concurrently)
pv task = spawn async {
    fetch_data("https://example.com").await
};

// Do other work...
process_local();

// Wait for result
pv result = task.await;
```

### 16.3 Synchronization

```pledge
// Mutex
use std::sync::Mutex;

pv data = Mutex::new(0);
{
    pv mut guard = data.lock();
    *guard += 1;
}  // lock released here

// Channels (for message passing)
use std::sync::channel;

pv (tx, rx) = channel();
spawn pf() {
    tx.send(42);
};
pv received = rx.recv();
```

---

## 17. Capability-Based Security

Pledge has no ambient authority. File, network, and hardware access require explicit capabilities.

```pledge
// No ambient authority — this won't compile
pf bad() {
    // ERROR: no File capability in scope
    pv file = open("secret.txt");
}

// Explicit capability required
pf good(cap: &FileCap) {
    pv file = cap.open("data.txt");
    pv content = file.read();
    println(content);
}

// Capabilities are passed, not global
pf main() {
    pv cap = FileCap::new("/allowed/path/");  // restricted to this path
    good(&cap);
}
```

---

## 18. Hot Reload

Pledge supports hot-swapping functions at runtime (critical for PledgeOS):

```pledge
#[hot_reloadable]
pub pf handle_request(req: &Request) -> Response {
    // This function can be replaced at runtime
    // without restarting the process
    process(req)
}

// Reload trigger (PledgeOS-specific)
pf reload_module(name: &str) -> Result<(), ReloadError> {
    pv new_code = compile_file(name)?;
    swap_function(name, new_code)?;
    Ok(())
}
```

---

## 19. Standard Library Prelude

Items available without explicit import:

```pledge
// Types
Option<T>     Some(T)     None
Result<T, E>  Ok(T)       Err(E)
Vec<T>        String      &str
Box<T>        Rc<T>       Arc<T>

// Functions
println       print       eprintln
vec!          format      dbg
spawn         handle

// Note: `print` is polymorphic (accepts int, float, bool, str, vec, map)
// and always appends a trailing newline (like Rust's `println!`).
// `println` is an alias for `print` with explicit newline semantics.

// Traits
Clone         Copy        Debug
Default       Display     Eq + Ord
Hash          Iterator    IntoIterator
From          Into        TryFrom + TryInto

// Macros (none — Pledge has no macros)
// All of the above are functions or types
```

---

## 20. File Extension

Pledge source files use the `.p` extension:

```
main.p
parser.p
net/http.p
```

---

## 21. Complete Example

```pledge
use std::io::println;
use std::fs::read_to_string;
use std::collections::HashMap;

#[derive(Debug, Clone, PartialEq)]
struct Config {
    name: str,
    version: str,
    settings: HashMap<str, str>,
}

impl Config {
    pf new(name: &str) -> Config {
        Config {
            name: name.to_string(),
            version: "0.1.0".to_string(),
            settings: HashMap::new(),
        }
    }

    pf set(&mut self, key: &str, value: &str) {
        self.settings.insert(key.to_string(), value.to_string());
    }

    pf get(&self, key: &str) -> Option<&str> {
        self.settings.get(key).map(|v| v.as_str())
    }
}

enum LoadError {
    FileNotFound,
    ParseError(str),
}

pf load_config(path: &str) -> Result<Config, LoadError> {
    pv content = read_to_string(path)
        .map_err(|_| LoadError::FileNotFound)?;

    pv mut config = Config::new("app");
    for line in content.lines() {
        pv parts: Vec<&str> = line.splitn(2, '=').collect();
        if parts.len() == 2 {
            config.set(parts[0].trim(), parts[1].trim());
        }
    }

    Ok(config)
}

#[test]
pf test_config() {
    pv mut config = Config::new("test");
    config.set("key", "value");
    assert_eq(config.get("key"), Some("value"));
    assert_eq(config.get("missing"), None);
}

#[test]
pf test_config_debug() {
    pv config = Config::new("app");
    pv debug = format(f"{config:?}");
    assert(debug.contains("Config"));
}

pub pf main() -> Result<(), LoadError> {
    pv config = load_config("pledge.toml")?;

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
pf name(params) -> ReturnType { body }
pub pf name(params) -> ReturnType { body }
async pf name(params) -> ReturnType { body }
pf<T: cap> name(x: T) -> T { body }

// Variables
pv x = value;
pv mut x = value;
pv x: Type = value;

// Control flow
if cond { } else if cond { } else { }
match value { Pattern => expr, _ => default }
for x in iter { }
while cond { }
loop { break value; }

// Types
struct Name { field: Type }
enum Name { Variant, Variant(T), Variant { field: T } }
cap Name { pf method(&self); }
impl Name for Type { pf method(&self) { } }

// Generics
pf name<T: cap>(x: T) -> T { }
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
pub pf name() { }
pub(crate) pf name() { }

// Other
#[attribute]
comptime { }
defer expr;
spawn async { }
expr.await
```
