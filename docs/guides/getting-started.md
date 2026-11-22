# Getting Started with Fid

This guide takes you from zero to a running program in five minutes.

## Install

Download the installer for your platform from [fides-lang.org](https://fides-lang.org)
or build from source:

```
git clone https://github.com/pledgeandgrow/fid.git
cd fid
cargo build --release
```

The `fid` compiler is a single executable with the standard library
beside it. No runtime to install, no virtual machine, no toolchain
configuration.

## Hello World

Create `hello.fd`:

```fides
fn main() -> i64 {
    println("Hello, Fides!");
    return 0;
}
```

Run it:

```
fid run hello.fd
```

Build a native executable:

```
fid emit hello.fd -o hello        # Windows: hello.exe
fid emit hello.fd --nat -o hello  # cross-platform native
```

Or compile through C (needs a C compiler):

```
fid ccompile hello.fd -o hello
```

## Variables and Types

Every variable has one type, decided at compile time:

```fides
fn main() -> i64 {
    var count: i64 = 42;        // 64-bit integer
    var pi: f64 = 3.14159;      // double-precision float
    var name: str = "Fides";    // owned string
    var flag: bool = true;

    // Type inference works for local variables
    var total = count + 1;      // i64

    println(total);
    return 0;
}
```

No `null`. No undefined values. No type casts you didn't ask for.

## Functions

```fides
fn add(a: i64, b: i64) -> i64 {
    return a + b;
}

// Generics (checked at compile time)
fn largest<T>(items: &Vec<T>, less: fn(&T, &T) -> bool) -> T {
    var best: T = items.get(0);
    var i: i64 = 1;
    while i < items.len() {
        if less(&items.get(i), &best) {
            best = items.get(i);
        }
        i = i + 1;
    }
    return best;
}
```

## Control Flow

```fides
fn classify(n: i64) -> str {
    if n < 0 {
        return "negative";
    } else if n == 0 {
        return "zero";
    }
    return "positive";
}

// Match (no fallthrough, every case checked)
fn describe(x: i64) -> str {
    match x {
        0 => "zero",
        1..=9 => "single digit",
        _ => "large",
    }
}
```

## Collections

```fides
use vec;

fn main() -> i64 {
    var numbers: Vec<i64> = Vec::new();
    numbers.push(3);
    numbers.push(1);
    numbers.push(4);

    println(numbers.len());      // 3
    println(numbers.get(0));     // 3

    // Strings concatenate with +
    var s: str = "hello" + " " + "world";
    println(s);
    return 0;
}
```

## Effects and Safety

Every function declares what it can do. The compiler checks:

```fides
// This function cannot allocate, do I/O, or panic
@no-alloc @no-panic
fn safe_add(a: i64, b: i64) -> i64 {
    return a + b;
}

// This function can read and write files
fn process(path: str) -> str !read !write {
    return read_file(path);
}
```

## Building a Project

Create an `ark.toml`:

```toml
[package]
name = "myapp"
version = "0.1.0"

[dependencies]
textkit = { git = "https://github.com/fid-lang/textkit" }
```

Then:

```
fid ark install    # fetch dependencies
fid emit main.fd -o myapp
```

## Cross-Compiling

One compiler, every target:

```
fid emit hello.fd --target aarch64-linux -o hello_arm
fid emit hello.fd --target riscv64-linux -o hello_riscv
fid emit hello.fd --target wasm32-wasi -o hello.wasm   # via ccompile
```

No per-target toolchain. The compiler emits machine code directly.

## Next Steps

- [Language reference](../reference/reference.md)
- [Migration guides](#) for C, Rust, Go, Python developers
- [Standard library](../stdlib/) — 403 modules
- [Examples](../../examples/) — working programs
