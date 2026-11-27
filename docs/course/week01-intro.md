# Week 1 — A language with a checked contract

**Reading before the lecture:** book chapters 1 and 2
(`docs/book/01-intro.md`, `docs/book/02-first-program.md`); reference §1,
§3.1 and §3.4 (`docs/reference/reference.md`).

**After this week you can:** install and run `fid`; explain the stages a
Fid program passes through; read the compiler's error messages; tell a
compile-time rejection from a run-time panic, and say which the language
promises for a given mistake.

## 1. Why a course on Fid

Fid is a small systems language whose compiler, Fides, is written in Fid
itself (`compiler/*.fd`, 65 modules, about 361,000 lines on Nov 27, 2026;
the release compiler is the
one that compiled its own source). It is a good teaching vehicle for four
ideas this course follows through the term:

| Theme | Weeks | The question it answers |
|---|---|---|
| Memory safety | 5–7 | Who frees a value, and how does the compiler know nobody still uses it? |
| Effect systems | 8–9 | What may a function *do* besides compute its result? |
| Capability security | 10 | What may a *program* (or a part of it) do at all? |
| Systems programming | 11–12 | What does a value look like in memory, and how does it meet C and the machine? |

Each of these is a contract between the programmer and the compiler. The
recurring exercise of the course is to find out exactly what the contract
says, where the compiler enforces it, and where it stops. The reference
(`docs/reference/reference.md`) is written that way: every rule names the
function of the compiler that checks it and the test that shows it.

## 2. The pipeline

`fid run x.fd` and `fid emit x.fd` share a front end and differ in the
back end:

```
source ─ lexer ─ parser ─ imports ─ type checker ─ borrow checker ─ monomorphizer
                                        │ effects, capabilities, safety annotations
                                        ▼
                          lowering to SSA IR (ownership: drops, copies)
                                        │
             ┌──────────────────┬───────┴──────────┬──────────────────────┐
         interpreter         C code           native machine code      WebAssembly
         (`fid run`)      (`fid ccompile`)   (`fid emit`: PE, ELF,    (`fid wasm`)
                                              Mach-O)
```

Everything the course calls "the compiler rejects it" happens in the top
row, before any code exists, so it holds on every back end alike.
`fid check x.fd` stops after it. Everything called "it panics" happens at
run time, and the language promises the same panic on every back end
(reference §8.2).

## 3. A first program

`fn main() -> i64` is the entry point; its result is the process's exit
status. `val` binds once, `var` may be assigned again, and every variable
has a type, written or inferred:

```fides
// collatz.fd
fn steps(start: i64) -> i64 {
    var n: i64 = start;
    var count: i64 = 0;
    while n != 1 {
        if n % 2 == 0 { n = n / 2; } else { n = 3 * n + 1; }
        count = count + 1;
    }
    return count;
}

fn main() -> i64 {
    var best: i64 = 0;
    var arg: i64 = 0;
    for i in 1..30 {
        val s: i64 = steps(i);
        if s > best { best = s; arg = i; }
    }
    println(f"longest below 30: {arg} takes {best} steps");
    return best % 7;
}
```

```
$ fid run collatz.fd
longest below 30: 27 takes 111 steps
$ echo $?
6
```

`1..30` counts from 1 to 29. `f"…{name}…"` is a format string. The exit
status is `111 % 7`.

## 4. Rejected, or stopped

The same mistake can be caught at two moments. A value that cannot fit is
a compile-time error when the compiler can see it, and the error says how
to fix it:

```fides
// narrow_fail.fd
fn main() -> i64 {
    var a: u8 = 300;
    val big: i64 = 5000000000;
    var b: i32 = big;
    return 0;
}
```

```
$ fid check narrow_fail.fd
error[E079]: Type error (line 3): the literal does not fit in u8.
...
error[E079]: Type error (line 5): a i64 may not fit in i32.
    Fix: convert it with `as i32`.
...
```

When only the run time knows, the program stops instead. Integer
arithmetic is checked at the declared width: `200 + 100` does not fit in
a `u8`, and the program panics rather than wrapping silently:

```fides
// overflow.fd
fn add(x: u8, y: u8) -> u8 {
    return x + y;
}

fn main() -> i64 {
    println(int_to_str(add(100, 100) as i64));
    println(int_to_str(add(200, 100) as i64));
    return 0;
}
```

```
$ fid run overflow.fd
200
Panic: Integer overflow on addition: 200 + 100 ...
...
$ echo $?
1
```

A panic prints `Panic:` and a stack trace on stderr, keeps what was
already printed, runs no cleanup and exits with status 1. It is never
caught. Week 3 is about writing code that does not need it; week 9 about
proving that a function cannot reach it (`@crashfree`, `@wrapping`).

## 5. The tools of the course

| Command | Use |
|---|---|
| `fid run x.fd` | interpret; the reference behaviour |
| `fid check x.fd` | type, borrow, effect and capability checks only |
| `CC="zig cc" fid ccompile x.fd -o x` | build through C |
| `fid emit x.fd -o x.exe` | native Windows; `--target x86_64-linux` (or `aarch64-linux`, `riscv64-linux`) for Linux |
| `fid audit x.fd` | the `unsafe` code a program reaches (week 10) |
| `fid explain x.fd` | which run-time checks the optimizer removed, and why |

Every complete example in these notes starts with a `// name.fd` comment
and is followed by the session it produced. `tests/course/check_course.py`
runs all of them; run it yourself after changing an example.

## Exercises

The lab for this week is [Lab 1 — The longest Collatz chain](labs/lab01-collatz/README.md).

1. Change `collatz.fd` to search below 10,000. Which `i64` value does the
   longest chain reach on its way? Could it overflow, and what would
   happen if it did?
2. Write `sum_u8(v: Vec<u8>) -> u8` and find the shortest input that
   panics. Then change the result type so that it cannot panic for any
   input of up to a million elements, and explain why.
3. Run `fid check` on a program that calls a function that does not
   exist, on one that passes a `str` where an `i64` is expected, and on
   one with a missing `}`. Which stage of the pipeline reports each?

## Discussion

The language reports overflow instead of wrapping. C wraps unsigned
arithmetic and makes signed overflow undefined; Rust panics in debug
builds and wraps in release builds. What does each choice cost, and who
pays: the programmer, the compiler writer, or the user of the program?
