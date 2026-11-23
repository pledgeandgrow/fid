# Week 12 — Systems programming II: C, machine code and the back ends

**Reading:** reference §2.10 (`extern "C"`) and §8.5; book chapters 7
("Talking to C") and 10 (`docs/book/10-shipping.md`);
`docs/reference/language-guide.md` §8 (back ends); `docs/guides/cpp-interop.md`;
`docs/reference/compiler-internals.md`.

**After this week you can:** call a C library from Fid and say which
safety guarantees stop at the boundary; build the same program for the
interpreter, C, native Windows and Linux, and WebAssembly; explain how
the project checks that all back ends agree; read what the optimizer did
to a loop.

## 1. Calling C

An `extern "C"` block declares C functions: `include` adds a header,
`link` a library, `as` gives the function a Fid name. A call to one needs
`unsafe`, because the compiler cannot check what the C code does:

```fides
// cmath.fd
extern "C" include "<math.h>" link "m" {
    fn sqrt as c_sqrt(x: f64) -> f64;
    fn floor as c_floor(x: f64) -> f64;
}

extern "C" include "<string.h>" {
    fn strlen as c_strlen(s: str) -> u64;
}

fn hypot(a: f64, b: f64) -> f64 {
    // SAFETY: sqrt is defined for every non-negative double
    unsafe {
        return c_sqrt(a * a + b * b);
    }
}

fn isqrt(n: i64) -> i64 {
    // SAFETY: floor and sqrt of a non-negative double
    unsafe {
        return c_floor(c_sqrt(n as f64)) as i64;
    }
}

fn main() -> i64 {
    println(float_to_str(hypot(3.0, 4.0)));
    println(int_to_str(isqrt(99)) + " " + int_to_str(isqrt(100)));
    var n: u64 = 0;
    // SAFETY: a Fid string is NUL-terminated
    unsafe { n = c_strlen("systems"); }
    println(int_to_str(n as i64));
    return 0;
}
```

```
$ fid run cmath.fd
5
9 10
7
```

`hypot` and `isqrt` are safe wrappers: their callers need no `unsafe`.
Without the block the call is rejected:

```fides
// c_unsafe_fail.fd
extern "C" include "<math.h>" link "m" {
    fn sqrt as c_sqrt(x: f64) -> f64;
}

fn main() -> i64 {
    println(float_to_str(c_sqrt(2.0)));
    return 0;
}
```

```
$ fid check c_unsafe_fail.fd
error[E097]: Safety error (line 7): calling the C function c_sqrt() needs an `unsafe` block.
...
```

What crosses the boundary (proposal 0004 and reference §2.10):

| Fid | C | Notes |
|---|---|---|
| `i64`, `u8`…`u64`, `f64`, `bool` | the same-width scalar | by value |
| `str` | `const char *` | NUL-terminated; a `str` returned by C is copied in (`""` for null) |
| `*mut T`, `*u8` | `T *`, `void *` | raw: `unsafe` to use |
| `@repr(C)` struct, by pointer | `struct T *` | the layouts of week 11 |
| a named function | a function pointer | a closure is refused (below) |

A C library that takes a callback gets a plain function pointer, which
cannot carry a closure's captured variables. So only a named function may
be passed, and state travels through the library's `void *user_data`
argument (`test_c_callback_user_data.fd`). This file is rejected for
passing a closure:

```fides
// callback_fail.fd
extern "C" include "<stdlib.h>" {
    fn qsort as c_qsort(base: *u8, n: i64, size: i64, cmp: fn(*u8, *u8) -> i32);
}

fn main() -> i64 {
    var descending: bool = true;
    var data: i64 = 0;
    // SAFETY: data points to 4 i64 slots
    unsafe {
        data = p_malloc(32);
        c_qsort(data as *u8, 4, 8, fn(a: *u8, b: *u8) -> i32 {
            if descending { return 1; }
            return -1;
        });
        p_free(data);
    }
    return 0;
}
```

(`fid check callback_fail.fd` reports that parameter 4 of `c_qsort` needs
a named function.) Inside a sandbox (week 10) no C function may be called
at all: its effects are unknown.

## 2. One front end, several back ends

```
                     ┌─ interpreter  (`fid run`; compiler/interp.fd)
                     ├─ C            (`fid ccompile`; codegen_c.fd, then zig cc / cl / tcc)
typed SSA IR ────────┼─ PE x64       (`fid emit`, Windows; pe_ir*.fd)
 (lower.fd, iropt.fd)├─ machine IR   (`fid emit --target x86_64-linux|aarch64-linux|riscv64-linux`,
                     │                 `--nat` on Windows; nat_ir.fd, nat_enc.fd)
                     └─ WebAssembly  (`fid wasm`)
```

The same program, five ways:

```bash
# from the directory of sum.fd below
fid run sum.fd                                  # interpreter
CC="zig cc" fid ccompile sum.fd -o sum_c.exe    # through C
fid emit sum.fd -o sum.exe                      # native Windows PE
fid emit sum.fd --target x86_64-linux -o sum    # Linux ELF: run on Linux or in WSL
fid wasm sum.fd -o sum.wasm                     # WebAssembly (WASI)
node scripts/wasm-run.mjs sum.wasm
```

The native back ends write machine code and object files themselves: no
assembler, no system linker for PE and the static Linux targets. That is
why the project's riskiest files are encoders and file-format writers
(AGENTS.md, "HIGH-RISK FILES").

## 3. Agreeing back ends

Five back ends are five chances to disagree. The project's answer is
differential testing: the interpreter is the reference, and every other
back end must print the same output and exit with the same status on the
same program. `tests/course/check_course.py --native` does this for the
examples of these notes (each `fid run` session is also built with
`ccompile` and `emit` and compared); `scripts/cross-test.sh` does it for
the whole test suite on each target, with qemu-user for the Linux
architectures; `FID_VERIFY=1` runs the IR verifier and a checker of the
register allocator's output during a build.

## 4. What the optimizer did

The run-time checks of weeks 1 and 4 (bounds, overflow) cost something,
and the optimizer removes the ones it can prove never fire. `fid explain`
says which, and why:

```fides
// sum.fd
fn sum(v: &Vec<i64>) -> i64 {
    var s: i64 = 0;
    for i in 0..v.len() { s = s + v.get(i); }
    return s;
}

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    for i in 0..10 { v.push(i * i); }
    println(int_to_str(sum(&v)));
    return 0;
}
```

```
$ fid run sum.fd
285
$ fid explain sum.fd sum
fn sum: ...
...
    removed bounds check (v...
...
```

`v.get(i)` for `i` in `0..v.len()` cannot be out of bounds: a *counted
loop* proves it, so the check is gone. `s + v.get(i)` can overflow for
some inputs, so that check stays (exercise 2). `fid explain x.fd --alloc`
does the same for heap allocations: where they are and why each could not
live on the stack.

## Exercises

The lab for this week is [Lab 12 — A safe wrapper around C](labs/lab12-ffi/README.md).

1. Declare `getenv` and `atoi` from `<stdlib.h>` and write a safe
   `env_int(name: str, default: i64) -> i64`. What must your wrapper do
   when C returns a null pointer? (Compare with core's `env_var`.)
2. Make `sum` free of all checks: what must you know about the elements?
   Express that knowledge in the code (types, `@wrapping`, a different
   loop) and confirm with `fid explain`.
3. Build `bits.fd` from week 11 with `ccompile` and `emit` and compare
   the executables' sizes. Read `docs/benchmarks.md`: which back end is
   fastest on which program, and why might that be?
4. Write a program that prints something different under `fid run` and
   a native build, using only `unsafe` code. Then explain why no *safe*
   program should be able to (and what the project does when one does:
   `docs/verification/miscompile-log.md`).
5. Pick one row of the boundary table and find the code in
   `compiler/codegen_c.fd` that implements it.

## Discussion

Rust, Zig and Go can all call C, and each draws the trust boundary in a
different place (Go's cgo even changes how goroutines are scheduled).
What is the smallest trusted computing base a Fid program can have if it
calls one C function? List every component.
