# 7. The toolchain

Everything is one program, `fid`. The compiler, the interpreter, the test
runner, the linter, the formatter, the documentation generator, the
coverage tool, the package manager and the language server are all
subcommands of it. Run `fid` with no arguments for the list your build
has; `fid version` prints which build it is.

This chapter walks through the commands you will use every day, in the
order you meet them, and is honest about the ones with sharp edges. The
full table, with the state of every command, is
`docs/reference/tool.md`.

## Three ways to run a program

| Command | What it does | Needs |
|---|---|---|
| `fid run x.fd` | type-checks, then runs the program in the interpreter | nothing |
| `fid ccompile x.fd -o x.exe` | generates C, then runs a C compiler | `CC` (zig cc, cl, gcc, clang, tcc) |
| `fid emit x.fd -o x.exe` | writes a native executable itself (`build` is the same command) | nothing |

`fid run` is the quickest way to check that a program is right: no
toolchain, and the best panic traces (with line numbers). It is also slow
— an interpreter, many times slower than compiled code — so measure
speed in a build, not under `run`.

`fid ccompile` uses the C compiler in `CC`. **When `CC` is not set it
tries `tcc`**, and fails if you don't have it. With zig installed:

```bash
CC="zig cc" fid ccompile app.fd -o app.exe
```

It builds at `-O2` by default; `--debug` builds at `-O0`, and
`--cc-opt 0|1|2|3|s` (or `FIDES_CC_OPT`) picks the level. The generated
C is left beside the output (`app.c`), which is handy when you want to
see what the compiler made of something.

`fid emit` needs nothing at all: it writes a Windows PE, or with
`--target` a Linux or macOS executable, from any host (chapter 10).

All three print the same compile errors, because they share the front
end. Builds are incremental: a `.fides-stamp` file beside the output
records what went in, and an unchanged build says `app.exe is up to
date`. `--no-cache` (or `FIDES_NO_CACHE=1`) forces a rebuild.

## Checking without running

```bash
fid check app.fd
```

```
app.fd: no errors (2006 ms)
```

`check` runs the type, borrow, escape and effect checks and stops. It is
what to bind to a key in your editor if you don't use the language
server.

```bash
fid lint stats.fd
```

```
warning: unused function 'unused_helper' — defined but never called
warning: unused import: 'string' — module is imported but none of its functions are called
```

`lint` reports unused functions and imports and `unwrap()` outside tests.
Two notes: it reports `test_*` functions as unused too (nothing calls
them but the test runner), and it does not resolve `compiler::` paths.
`lint --perf` adds a check for quadratic patterns in loops.

`explain` shows what the optimizer did with the run-time checks of a
function:

```bash
fid explain stats.fd mean
```

```
fn mean: 2 check(s) removed, 2 kept
    removed bounds check (v28, counted loop)
    removed add overflow check (v33, range analysis)
```

Useful when a hot loop is slower than you expect: a check that is kept
is a branch in every iteration. `explain x.fd --alloc [name]` lists each
allocation site and why it is on the heap (`v4: Vec buffer (heap:
resizable)`); without a name it lists core's functions too.

## Testing

A test file holds functions named `test_*` (and `bench_*`) and **no
`main`**. Put the code under test in a module and `use` it:

```fides
// stats.fd
module stats;

/// The mean of the values, rounded down; 0 for an empty Vec.
pub fn mean(v: &Vec<i64>) -> i64 {
    if v.len() == 0 { return 0; }
    var s: i64 = 0;
    for i in 0..v.len() { s = s + v.get(i); }
    return s / v.len();
}

/// The largest value; 0 for an empty Vec.
pub fn largest(v: &Vec<i64>) -> i64 {
    var best: i64 = 0;
    for i in 0..v.len() {
        if i == 0 || v.get(i) > best { best = v.get(i); }
    }
    return best;
}
```

```fides
// stats_test.fd
use stats;

fn test_mean() {
    var v: Vec<i64> = Vec::new();
    v.push(2);
    v.push(4);
    assert_eq(mean(&v), 3);
}

fn test_mean_empty() {
    var v: Vec<i64> = Vec::new();
    assert_eq(mean(&v), 0);
}

fn test_largest() {
    var v: Vec<i64> = Vec::new();
    v.push(-5);
    v.push(-2);
    assert_eq(largest(&v), -1);      // wrong on purpose
}
```

```
$ CC="zig cc" fid test stats_test.fd
ok    test_mean
ok    test_mean_empty
Panic: Assertion failed: values not equal
stack trace:
    - test_largest
    -| called from main
FAIL  test_largest
2 passed, 1 failed
```

Each test runs in its own process, so one panicking test doesn't stop
the others, and the exit status is nonzero when any fails. `fid test`
builds through C, so it needs `CC` like `ccompile`. (`--filter <text>`
and `--jobs <n>` are in the current source; the 0.8.0-unstable build
ignores them and runs every test.) A program that uses the file as a
module:

```fides
// app.fd
use stats;

fn main() -> i64 {
    var v: Vec<i64> = Vec::new();
    for i in 1..=10 { v.push(i * i); }
    println("mean " + int_to_str(mean(&v)));
    println("max  " + int_to_str(largest(&v)));
    return 0;
}
```

```
$ fid run app.fd
mean 38
max  100
```

`use stats;` finds `stats.fd` next to the file that imports it; then
`stats/mod.fd`, the standard library, and the project root (the first
directory up that holds `stdlib/core.fd`).

## Memory checks

```bash
fid run app.fd --leaks            # every block never freed, by allocation site
fid run app.fd --checked-alloc    # fault on use after free or past a block's end
```

```
mean 38
max  100
fid --leaks: every block was freed
```

Both rebuild the program through C with a debug allocator, so they need
`CC`. Run them in your test setup; chapter 4 explains what they catch.
`fid audit app.fd` lists the `unsafe` code your program reaches and
whether each site has a `// SAFETY:` comment (`--stdlib` includes the
standard library's).

## Formatting

```bash
fid fmt app.fd --check     # exit 1 and "would reformat" if it would change
fid fmt app.fd             # rewrites the file in place
```

The formatter is young. It expands one-line blocks, but it also writes
`&v` as `& v` and has left a stray space before `fn` where it removed a
blank line. The result compiles and runs the same, but commit first and
read the diff; don't run it over a tree you can't revert.

## Documentation

`///` comments before an item are its documentation:

```bash
fid doc stats.fd           # Generated docs/stats.html (2 items)
```

The page goes to `docs/` under the current directory. `fid scribe` is
the same tool with `--format html|markdown` and `--output <dir>`. Newer
builds also take a directory (`fid doc src/`) and write a site with a
page per module and a searchable index; 0.8.0-unstable answers
`Cannot read src`.

## Coverage

```bash
fid veil app.fd
```

```
Functions:     1/1 (100.0%)
Lines:         3/3 (100.0%)
Instructions:  162/173 (93.6%)
...
Coverage report: coverage/coverage.txt
```

`veil` runs the program in the interpreter and marks each source line it
executed (`+` in the listing). It counts the functions of the file you
name; to cover a module, point it at a file whose `main` exercises that
module's functions directly, or at the module's tests. `--format html`
writes an HTML report.

## Profiling and benchmarks

```bash
CC="zig cc" fid profile app.fd     # builds with counters, runs, prints the top 20 functions
```

A program that finishes in under a millisecond has nothing to show
(`profile: no samples`). `docs/reference/tool.md` also documents a
sampling profiler built into Linux programs made by `fid emit`: run them
with `FID_PROF=1` for a table of functions by self and total samples on
stderr at exit.

`fid bench app.fd -n 10` builds the program and runs it `n` times. In
current builds it always uses `tcc`, whatever `CC` says; without `tcc`,
build the executable yourself and time it.

## Talking to C

```bash
fid parse-header geo.h
```

```fides
// Generated by `fid parse-header` from geo.h.
// Regenerate rather than edit. Pointers other than char* are addresses
// (i64), char* is str, C `long` is 32 bits (Windows x64).

extern "C" link "geo" {
    fn geo_add(a: i64, b: i64) -> i32;
    fn geo_scale(x: f64, k: f64) -> f64;
}
```

Check the result against the header before you rely on it: above, C
`int` parameters came out as `i64` and the `int` result as `i32`. The
`extern "C"` syntax is in the reference (§2.10); calling a C function
whose name does not start with `p_` needs `unsafe`. Only `ccompile`
links C libraries.

## Packages: ark

Packages are called **pacts**, the manifest is `ark.toml`, and installed
pacts go to `ark_modules/`, where `use` looks for them:

```bash
fid ark init demo                      # writes ark.toml
fid ark add textkit                    # from the pacts index, or --path / --git
fid ark install                        # into ark_modules/
fid ark list
fid ark lock                           # ark.lock: git commits and file hashes
```

```
$ fid ark list
demo v0.1.0 (2026)

NAME                  TYPE     SOURCE
------------------------------------------------------------
lefty  path  ../lefty
```

**Verified on Windows, Linux and macOS.** The 0.8.0-unstable build had
real Windows bugs (a directory pact reported missing, a file pact
installed without its `.fd` extension, `ark lock` writing 0 pacts);
they are fixed, and the four ark test suites (paths, features,
projects, registry — 283 checks) run green on Windows and in CI on
Linux and macOS.

Pacts also come from an index, like crates: [pledgeandgrow/pacts](https://github.com/pledgeandgrow/pacts).

```bash
$ fid ark search textkit
textkit 1.0.0  https://github.com/pledgeandgrow/textkit.git
$ fid ark add textkit            # or --version ^1
$ fid ark install
Installing textkit... OK (registry 1.0.0 92c4ae284fba)
```

Then `use textkit;` and `textkit_title("hello pacts")` prints
`Hello Pacts` — through the interpreter and a C build alike, on every
OS. Publishing your own pact is `git tag v1.0.0 && fid ark publish`,
then a pull request the index's check verifies (`ARK_INDEX` points ark
at another index if you need your own). The index is young: few pacts
are published yet. Until yours is there, `--path` and `--git` work the
same as ever, and so does the plain module search: `lefty.fd` next to
the program, `use lefty;`.

## Looking inside

`fid tokens`, `fid ast`, `fid ir` and `fid dump-c` print each stage of the
compiler for a file. You won't need them to write programs; they are the
first thing to attach to a bug report.

## Editors

`fid lsp` is a language server over stdio. `docs/reference/tool.md`
lists what it answers — diagnostics, completion, hover, go to definition,
rename and references (by whole word, within one file), formatting,
signature help, parameter-name hints — and its known gaps. Point your
editor's generic LSP client at `fid lsp` for `.fd` files; if it
misbehaves, `fid check` on save gives you the same diagnostics.

## Newer commands

Builds from mid-November 2026 on add `fid fix` (apply the compiler's
suggested fixes: `val` to `var`, a narrowing `as`, a misspelt name, an
unused import) and `fid fuzz` (run a test file's `fuzz_*` targets on
generated inputs). If `fid` without arguments doesn't list them, your
build predates them.

## Common mistakes

- **No `CC`.** `ccompile`, `test`, `profile`, `--leaks` and
  `--checked-alloc` all go through C. Set `CC="zig cc"` (or `cl`, `gcc`)
  once in your shell profile.
- **Naming a file after a standard module.** The importing file's
  directory is searched before `stdlib/`, so with a `channel.fd` of your
  own in that folder — including the program itself — `use channel;`
  loads yours, and every `channel_*` call is an `undefined function`.
  Don't reuse standard module names for your files.
- **A `main` in the test file.** `fid test` refuses it:
  `a test file holds only fn test_*() and fn bench_*()`.
- **Benchmarking `fid run`.** It is an interpreter. Use `emit` or
  `ccompile` for anything timed.
- **Running `fmt` on uncommitted work.** It rewrites in place; check the
  diff.
- **Lint noise about `test_*` functions.** Expected; lint test files
  separately or ignore those lines.
- **Two `fid` builds at once on a small machine.** A self-hosted
  compiler is a big program; parallel builds slow each other down far more
  than linearly.
