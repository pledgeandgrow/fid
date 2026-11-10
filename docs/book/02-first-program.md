# 2. Your first program

```fides
// hello.fd
fn main() -> i64 {
    println("Hello, Fides!");
    return 0;
}
```

## Run it three ways

```bash
fid run hello.fd          # interpret — fastest feedback
fid emit hello.fd         # native binary (PE on Windows; ELF via --target)
fid ccompile hello.fd     # via C — needs zig cc / cl / tcc
```

`fid emit hello.fd --target x86_64-linux` produces a static Linux ELF on
any host — no qemu, no cross toolchain. `aarch64-linux` and
`riscv64-linux` work the same way (run them with qemu-user or real
hardware).

## The anatomy

- `fn main() -> i64` — the entry point. The return value is the exit code.
- `println(...)` — prints a `str` plus newline. `print(...)` omits it.
- `//` line comments, `/* */` blocks.
- `use core;` — imports. Many basics (`println`, `str`, `Vec`, `i64`)
  work without an explicit import; stdlib modules need `use x;`.

## Values

```fides
val answer: i64 = 42;      // immutable, type inferred when obvious
var count: i64 = 0;        // mutable
count = count + 1;
var name: str = "fid";     // strings are immutable + length-tagged
var ok: bool = true;
var pi: f64 = 3.14159;
```

Arithmetic is **checked by default**: overflow panics. For deliberate
wrap-around (hashing, crypto), mark the function `@wrapping`:

```fides
@wrapping fn mix(x: i64) -> i64 {
    return x * 31 + 17;      // wraps on overflow instead of panicking
}
```

## Next

Chapter 3 tours the whole language. If you'd rather build first, jump to
chapter 9 (writing a service) and come back.
