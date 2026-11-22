# 6. Errors

Fid has two kinds of failure, and the first design decision in any
function is which one it uses:

- **A panic** is a bug or a broken invariant. It prints a message and a
  stack trace and ends the *whole program* with exit status 1. It is never
  caught.
- **An `Option` or `Result`** is an outcome the caller is expected to
  handle: a missing file, a bad config value, a user who doesn't exist.

There are no exceptions, no `try`/`catch` that intercepts a panic, and no
error codes hidden in globals. The rule from the reference (§8.2) is
short: code that can fail and must go on returns `Result` or `Option`.

## Panics

`panic(msg)` stops the program:

```fides
// panic.fd
fn checked_age(n: i64) -> i64 {
    if n < 0 {
        panic("age cannot be negative: " + int_to_str(n));
    }
    return n;
}

fn main() -> i64 {
    println("start");
    println(int_to_str(checked_age(30)));
    println(int_to_str(checked_age(-1)));
    println("not reached");
    return 0;
}
```

```
$ fid run panic.fd
start
30
Panic: age cannot be negative: -1 at line ...
stack trace:
    → panic at line 4
    ↳ called from checked_age...
    ↳ called from main
$ echo $?
1
```

What a panic guarantees, on every path (`fid run`, C, native):

- `Panic: <message>` and a trace go to **stderr**; what the program already
  printed to stdout is flushed, not lost.
- The exit status is 1.
- **Nothing unwinds.** No `defer` block, no `drop` method runs after the
  panic. The operating system takes back memory, files and sockets when
  the process ends.
- A panic in a task or thread ends the whole program too (chapter 8).

The trace's detail depends on how the program was built. The interpreter
shows lines (the `at line` on the `Panic:` line itself is, today, the line
of the `p_panic` call inside `core.fd`'s `panic` function, 71 with the
Nov 21 tree; the trace below it, `panic at line 4`, is your line); a C
build (`fid ccompile`) shows the function names it can
find on the machine stack (`- main`); a native Windows build from
`fid emit` prints the message without a trace. When you need a trace from
a crash, reproduce it under `fid run` or a C build.

### The checks that panic for you

You don't write most panics; the runtime does. These are checked on every
backend:

| Operation | Message |
|---|---|
| `v.get(i)` / `v[i]` out of range | `Panic: Vec index out of bounds` |
| `+ - *` overflow at the declared width | `Panic: Integer overflow on addition: 9223372036854775807 + 1 at line 5` |
| `/` or `%` by zero | `Panic: Division by zero` (even in `@wrapping` code) |
| `unwrap()` of `None` | `Panic: called unwrap on None` |
| `first()`/`last()` of an empty `Vec` | `Panic: Vec is empty` |
| stack overflow | `Panic: Stack overflow` (`fid run` stops at 1000 nested calls instead; `--max-recursion N`) |

Overflow checking is the one that surprises people coming from C: it is
on in release builds too. For arithmetic meant to wrap (hashes,
checksums), mark the function `@wrapping` (chapter 5).

### Assertions

Core has `assert(c)`, `assert_msg(c, msg)`, `assert_eq(a, b)`,
`assert_ne(a, b)` and the `_msg` forms. They are ordinary functions that
panic, and they are never compiled out:

```fides
// assert.fd
fn main() -> i64 {
    val total: i64 = 2 + 2;
    assert_eq(total, 4);
    assert_msg(total < 4, "total too big: " + int_to_str(total));
    return 0;
}
```

```
$ fid run assert.fd
Panic: total too big: 4 at line ...
stack trace:
    → assert_msg...
    ↳ called from main
```

(The line in the interpreter's first line is the `p_panic` call inside
`core.fd`'s `assert_msg`, not your line; the trace below it is what
points at your code.)

## Option: a value that may be missing

```fides
pub enum Option<T> { Some(T), None }
```

You construct and match it with the enum's name: `Option::Some(x)`,
`Option::None`. The tools for taking it apart:

| You want | Write |
|---|---|
| the value, or a default | `o ?? default` or `o.unwrap_or(default)` |
| to branch | `match o { Option::Some(v) => { … } Option::None => { … } }` |
| a test | `o.is_some()`, `o.is_none()`, `o is Some` |
| a `Result` with a message | `o.context("what was missing")` |
| to crash if it is missing | `o.unwrap()` or `o.expect(msg)` |

```fides
// option.fd
fn find_user(id: i64) -> Option<str> {
    if id == 1 { return Option::Some("ada"); }
    if id == 2 { return Option::Some("grace"); }
    return Option::None;
}

fn main() -> i64 {
    val first: str = find_user(1) ?? "nobody";
    val other: str = find_user(7) ?? "nobody";
    println(first);
    println(other);

    match find_user(2) {
        Option::Some(name) => { println("found " + name); }
        Option::None => { println("missing"); }
    }

    if find_user(9).is_none() {
        println("9 is not a user");
    }

    val r: Result<str, str> = find_user(5).context("user 5 not found");
    match r {
        Result::Ok(n) => { println(n); }
        Result::Err(e) => { println("error: " + e); }
    }
    return 0;
}
```

```
$ fid run option.fd
ada
nobody
found grace
9 is not a user
error: user 5 not found
```

Give the result of `??` a type when it is a `str` (`val first: str = …`):
see the first common mistake at the end of the chapter.

The standard library uses `Option` where something is commonly absent:
`read_file(path)`, `env_var(name)`, `arg_at(i)` and `read_line()` all
return `Option<str>`:

```fides
// readfile.fd
fn main() -> i64 {
    match read_file("missing.txt") {
        Option::Some(text) => { println("read " + int_to_str(str_len(text)) + " bytes"); }
        Option::None => { println("missing.txt: no such file"); }
    }
    val cfg: str = read_file("app.toml") ?? "";
    println("app.toml has " + int_to_str(str_len(cfg)) + " bytes");
    return 0;
}
```

```
$ printf 'port = 8080\n' > app.toml
$ fid run readfile.fd
missing.txt: no such file
app.toml has 12 bytes
```

## Result: success or a reason

```fides
pub enum Result<T, E> { Ok(T), Err(E) }
```

`E` can be anything. A `str` message is the quick choice; an enum is the
choice when callers need to tell errors apart. Here is a parser that
reports why it refused:

```fides
// port.fd
fn parse_port(s: str) -> Result<i64, str> {
    if str_len(s) == 0 {
        return Result::Err("empty port");
    }
    var n: i64 = 0;
    var i: i64 = 0;
    while i < str_len(s) {
        val b: i64 = str_byte_at(s, i);
        if b < '0' || b > '9' {
            return Result::Err("not a number: " + s);
        }
        n = n * 10 + (b - '0');
        if n > 65535 {
            return Result::Err("port out of range: " + s);
        }
        i = i + 1;
    }
    return Result::Ok(n);
}

fn main() -> i64 {
    val inputs: Vec<str> = str_split("8080,http,99999,", ",");
    for s in inputs {
        match parse_port(s) {
            Result::Ok(p) => { println("port " + int_to_str(p)); }
            Result::Err(e) => { println("error: " + e); }
        }
    }
    return 0;
}
```

```
$ fid run port.fd
port 8080
error: not a number: http
error: port out of range: 99999
error: empty port
```

Why write it by hand? Because core's `str_parse_int` returns **0 on
failure** — `str_parse_int("forty-two")` is `0`. When 0 is a valid
input, validate first, or use `str_parse_int_opt` (`use string;`), which
returns `Option::None` for text that is not an integer.

`Result` has `is_ok`, `is_err`, `unwrap`, `unwrap_or`, `ok` (to an
`Option`), `err`, `map`, `map_err`, `and_then`, `expect`,
`context(msg)` and `with_context(f)`. `map` and `map_err` take a
function from a type to *the same* type:

```fides
// maperr.fd
use file;

fn tag(e: str) -> str { return "config: " + e; }

fn load(path: str) -> Result<str, str> {
    return file_read_result(path).map_err(tag);
}

fn main() -> i64 {
    match load("nope.toml") {
        Result::Ok(t) => { println(t); }
        Result::Err(e) => { println(e); }
    }
    return 0;
}
```

```
$ fid run maperr.fd
config: cannot read nope.toml
```

### Propagating with `?`

`e?` takes an `Option` or a `Result`. On `Some`/`Ok` it is the value; on
`None`/`Err` it returns that from the enclosing function at once:

```fides
// digits.fd
fn parse_digit(s: str) -> Result<i64, str> {
    if str_len(s) != 1 {
        return Result::Err("want one digit, got '" + s + "'");
    }
    val b: i64 = str_byte_at(s, 0);
    if b < '0' || b > '9' {
        return Result::Err("not a digit: '" + s + "'");
    }
    return Result::Ok(b - '0');
}

fn add_digits(a: str, b: str) -> Result<i64, str> {
    val x = parse_digit(a)?;
    val y = parse_digit(b)?;
    return Result::Ok(x + y);
}

fn show(r: Result<i64, str>) -> void {
    match r {
        Result::Ok(v) => { println("ok " + int_to_str(v)); }
        Result::Err(e) => { println("err " + e); }
    }
}

fn main() -> i64 {
    show(add_digits("3", "4"));
    show(add_digits("3", "x"));
    show(add_digits("12", "4"));
    return 0;
}
```

```
$ fid run digits.fd
ok 7
err not a digit: 'x'
err want one digit, got '12'
```

The function using `?` must itself return an `Option` or `Result` of a
matching kind. There is no conversion between error types: if `parse_digit`
returned `Result<i64, ConfigError>` and `add_digits` returned
`Result<i64, str>`, convert explicitly with a `match` or `map_err`.

### Error enums

When callers need to act on *which* error happened, make `E` an enum:

```fides
// config.fd
enum ConfigError {
    Missing(key: str),
    Bad(key: str, value: str),
}

fn describe(e: ConfigError) -> str {
    match e {
        ConfigError::Missing(key) => { return "missing key " + key; }
        ConfigError::Bad(key, value) => { return "bad value '" + value + "' for " + key; }
    }
}

fn lookup(text: str, key: str) -> Result<str, ConfigError> {
    for line in str_lines(text) {
        val eq: i64 = str_find(line, "=");
        if eq > 0 {
            val k: str = str_trim(str_slice(line, 0, eq));
            if k == key {
                return Result::Ok(str_trim(str_slice(line, eq + 1, str_len(line))));
            }
        }
    }
    return Result::Err(ConfigError::Missing(key));
}

fn workers(text: str) -> Result<i64, ConfigError> {
    val raw: str = lookup(text, "workers")?;
    val n: i64 = str_parse_int(raw);
    if n <= 0 {
        return Result::Err(ConfigError::Bad("workers", raw));
    }
    return Result::Ok(n);
}

fn report(text: str) -> void {
    match workers(text) {
        Result::Ok(n) => { println("workers = " + int_to_str(n)); }
        Result::Err(e) => { println("config error: " + describe(e)); }
    }
}

fn main() -> i64 {
    report("port = 80\nworkers = 4\n");
    report("port = 80\n");
    report("workers = many\n");
    return 0;
}
```

```
$ fid run config.fd
workers = 4
config error: missing key workers
config error: bad value 'many' for workers
```

The `match` in `describe` must cover every variant; leaving one out is a
compile error. Core also declares `cap Error { fn message(self: Self) ->
str; }` if you prefer a trait-shaped error, but nothing in the language
requires it.

### try blocks

`try { …; expr }` collects `?` inside a single expression. Its type is
`Result<T, i64>`, so it suits code whose errors are integers:

```fides
// try.fd
fn half(x: i64) -> Result<i64, i64> {
    if x % 2 != 0 { return Result::Err(x); }
    return Result::Ok(x / 2);
}

fn main() -> i64 {
    val r = try {
        val a = half(20)?;
        val b = half(a)?;
        a + b
    };
    match r {
        Result::Ok(v) => { println("sum " + int_to_str(v)); }
        Result::Err(bad) => { println("odd: " + int_to_str(bad)); }
    }
    return 0;
}
```

```
$ fid run try.fd
sum 15
```

(`half(20)` is 10, `half(10)` is 5. Start from 6 and the second `half`
gets 3, so the block is `Err(3)`.) For `str` errors, put the steps in a
small function returning `Result<T, str>` and use `?` there.

## Cleanup: defer

`defer { … }` runs a block when the function returns — on every `return`
and every `?` that leaves the function — last registered first:

```fides
// defer.fd
fn work(fail: bool) -> Result<i64, str> {
    println("open");
    defer { println("close"); }
    if fail {
        return Result::Err("disk full");
    }
    println("write");
    return Result::Ok(3);
}

fn main() -> i64 {
    val a = work(false);
    println(f"ok? {a.is_ok()}");
    val b = work(true);
    println(f"ok? {b.is_ok()}");
    return 0;
}
```

```
$ fid run defer.fd
open
write
close
ok? true
open
close
ok? false
```

Two things to know: `defer` runs at *function* exit, not at the end of
the block it is written in; and it does **not** run on a panic.

## Functions that must not fail

`@crashfree` (= `@no-panic` + `@no-unbounded`) makes the compiler prove a
function has no panic path: no `unwrap`, no division by a non-literal, no
checked arithmetic, no `v.get(i)`, no `while`. The details and the rules
it forces are in chapter 5. It is the right tool for small leaf functions
on paths that must keep running — a health check, a packet classifier —
not something to sprinkle over a codebase.

## Debugging a crash

- Rerun under `fid run` for a trace with line numbers.
- `fid run x.fd --checked-alloc` rebuilds the program through C with a
  debug allocator (it needs `CC`), so a use after free in `unsafe` code
  faults at the bad access instead of reading stale data (chapter 4).
- Print. `eprint`-style output goes through `p_eprint_str(s)`, which
  writes to stderr and so stays out of the program's real output.

The Rust bootstrap compiler `fidesc` has `--post-mortem`, `replay` and
an interactive `debug`; `fid` has none of those. There is no debugger for
`fid` builds today beyond what your C toolchain gives a `ccompile --debug`
binary.

## Common mistakes

- **`??` on a `str` without a type.** Today the result of `??` on an
  `Option<str>` is only a `str` when you say so. `println(find_user(1) ??
  "nobody")` prints `ada` under `fid run` but a number in C and native
  builds, and `"user: " + (find_user(1) ?? "nobody")` prints an empty line
  under `fid run` and crashes a C build. Bind it to a typed variable first:
  `val name: str = find_user(1) ?? "nobody";`.
- **A bare `None` in a pattern.** `match o { None => … }` binds a new
  variable called `None` and matches everything — the `Some` arm after it
  never runs, and nothing warns you. Write `Option::None`.
- **`??` on a `Result`.** It is an error (`?? operator requires
  Option<T>`). Use `r.unwrap_or(d)`, or `r.ok() ?? d`.
- **`?` in a function that returns a plain value.** Today the compiler
  accepts `val x = f()?;` inside `fn main() -> i64` and the program quits
  with a meaningless status (0 under `fid run`, 192 from a C build).
  Use `?` only in functions that return `Option`/`Result`; in `main`,
  `match` and return an exit code yourself.
- **Trusting `str_parse_int`.** It returns 0 for text that is not a
  number. Validate, or write a parser that returns `Result`.
- **Expecting `defer` to run on a panic.** It doesn't; nothing does.
  Keep state on disk consistent by writing to a temporary file and
  renaming, not by cleanup code.
- **Catching a panic in a task.** You can't. A task that may fail returns
  a value the caller checks.
