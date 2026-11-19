# 2. Your first program

```fides
// hello.fd
fn main() -> i64 {
    println("Hello, Fides!");
    return 0;
}
```

```
$ fid run hello.fd
Hello, Fides!
```

## Run it three ways

```bash
fid run hello.fd                      # interpret: the fastest feedback
fid emit hello.fd -o hello.exe        # a native executable, no C compiler
CC="zig cc" fid ccompile hello.fd -o hello.exe   # through C (zig cc, cl or tcc)
./hello.exe
```

All three print the same thing and exit with the same status; when they
don't, it is a compiler bug (docs/reference/known-bugs.md lists the known
ones). Without `-o`, `emit` and `ccompile` write `a.out.exe` on Windows
and `a.out` elsewhere.

`fid emit hello.fd --target x86_64-linux -o hello` writes a Linux
executable from any host: no cross toolchain. `aarch64-linux` and
`riscv64-linux` work the same way (run them with qemu-user or on the
hardware). Chapter 10 covers every target.

## The anatomy

- `fn main() -> i64` is the entry point. Its result is the program's exit
  status.
- `println(x)` prints `x` and a newline; `print(x)` leaves the newline
  out. Both take a `str` or a number.
- `//` starts a line comment, `/* ... */` a block comment. `///` before an
  item is a doc comment (`fid doc` turns them into pages, chapter 7).
- Statements end with `;`. Blocks are `{ ... }`, and `if`/`while` need
  no parentheses.
- `core` (strings, `Vec`, `HashMap`, `Option`, `Result`, printing, files)
  is always there. Any other standard module needs `use name;` at the top
  of the file: `use string;` for `str_join`, `use json;`, `use http;`.

## Values

```fides
// values.fd
@wrapping fn mix(x: i64) -> i64 {
    return x * 31 + 17;      // wraps on overflow instead of panicking
}

fn main() -> i64 {
    val answer: i64 = 42;    // val: bound once
    var count: i64 = 0;      // var: can change
    count = count + 1;
    var name: str = "fid";   // strings are immutable and know their length
    var ok: bool = true;
    var pi: f64 = 3.14159;
    val inferred = 7;        // an integer literal alone is an i64
    println(int_to_str(answer) + " " + int_to_str(count) + " " + name);
    if ok { println("ok"); }
    println(float_to_str(pi));
    println(f"{inferred} doubled is {inferred * 2}");
    println(int_to_str(mix(9223372036854775807)));
    return 0;
}
```

```
$ fid run values.fd
42 1 fid
ok
3.14159
7 doubled is 14
9223372036854775794
```

`int_to_str` and `float_to_str` turn numbers into text; `+` joins two
strings. An `f"..."` string puts the value of each `{expression}` in
place.

The integer types are `i8` to `i128` and `u8` to `u128`; `i64` is the
default. Floats are `f64` and `f32`. Conversions are explicit:
`x as i64`, `n as f64`.

## Arithmetic is checked

`+`, `-` and `*` panic on overflow at the declared width, and `/` and `%`
panic on a zero divisor, on every backend:

```fides
// overflow.fd
fn main() -> i64 {
    var big: i64 = 9223372036854775807;
    println("before");
    big = big + 1;
    println(int_to_str(big));
    return 0;
}
```

```
$ fid run overflow.fd
before
Panic: Integer overflow on addition: 9223372036854775807 + 1 at line 5
...
$ echo $?
1
```

A panic prints `Panic: <message>` and a stack trace on stderr and ends
the program with status 1 (chapter 6). For deliberate wrap-around
(hashing, checksums, crypto), mark the function `@wrapping`, as `mix`
above: inside it, `+ - *` wrap at the type's width. Integer division
rounds toward zero: `7 / 2` is `3`.

## The exit status

`main`'s result is what the shell sees:

```fides
// exitcode.fd
fn main() -> i64 {
    println("failing on purpose");
    return 3;
}
```

```
$ fid run exitcode.fd
failing on purpose
$ echo $?
3
```

## Common mistakes

- **Joining a number to a string with `+`.** Write
  `"n = " + int_to_str(n)`, or `f"n = {n}"`. Today `"n = " + 5` is
  accepted and gives an empty string on every backend instead of being a
  type error.
- **Changing a `val`.** It is an error (`cannot assign to 'x': it is
  declared with val`); declare it with `var`.
- **Leaving out `return 0;` in `main`.** The compiler accepts a
  `fn main() -> i64` that ends without `return`, and the program exits
  with status 0, whatever happened. End `main` with an explicit
  `return`.
- **Forgetting `use`.** `str_join`, `json_get`, `http_serve` and the
  other non-core functions are an "undefined function" error until the
  file says `use string;`, `use json;`, `use http;`.
- **Expecting `fid emit x.fd` to write `x.exe`.** It writes `a.out.exe`
  (`a.out` off Windows); pass `-o`.

## Next

Chapter 3 tours the whole language. If you'd rather build first, jump to
chapter 9 (writing a service) and come back.
