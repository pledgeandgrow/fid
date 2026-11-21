# 1. Introduction & install

Fides is a self-hosted systems language: the compiler ships as a native
binary (`fid.exe`) written in Fides itself. It aims at the space between
Go and Rust — native binaries, predictable memory, an ownership checker —
with a compiler small enough to read.

## What it gives you

- **Native binaries with no toolchain**: `fid emit` writes a PE exe or a
  Linux ELF directly (x86-64, AArch64, RISC-V 64). A C path
  (`fid ccompile` + zig cc/cl/tcc) covers the rest.
- **Ownership without a GC**: one owner per value, borrows checked at
  compile time, frees inserted for you. References can't escape, so most
  code writes no lifetimes (a struct that holds a reference takes one).
- **Effects in the type**: the compiler works out what each function does
  (`IO`, `Net`, `Spawn`, `Diverge`, `Alloc`); write `!{}`, `!IO` or
  `!{IO, Net}` after the result type and the body is held to it
  (chapter 5).
- **Annotation-driven contracts**: `@crashfree`, `@no-alloc`,
  `@wrapping`, `@send`, `@secret`, `@repr(C)`, `@packed`, `@simd`,
  `@interrupt`, `@firmware` — for services, kernels, and crypto alike.
- **Self-hosted and fixpoint-verified**: the release bootstrap checks
  that the compiler compiles itself byte-for-byte identically (stage 2 ==
  stage 3); last verified on Nov 8, 2026, on a smaller tree.

## Install

- **Windows installer**: each release has `Fid-Setup-<version>.exe`
  (built from `installer/` with NSIS). It installs `fid.exe`, the frozen
  Rust compiler `fidesc.exe`, `ark.exe`, `scribe.exe`, `veil.exe`, the
  standard library and the examples, and puts them on `PATH`.
- **Linux**: `fid-x86_64-linux.tar.gz` holds `bin/fid`, `stdlib/` and
  `runtime/`; unpack it anywhere and put `bin/` on `PATH`.
- **From source**: `RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh` builds
  the compiler in three stages and checks that stage 2 and stage 3 are
  the same (`release/fid.exe`). Stage 0 is a self-hosted `fid` (the
  pinned `release/stage0/fid.exe`, or `STAGE0=<fid>`): a `fid` built by
  the Rust compiler runs out of memory compiling today's tree
  (known-bugs 2g.79). For development, `./scripts/build-selfhost.sh`
  builds one stage, `release/fid-dev.exe` (check that it built at -O2:
  it can fall back to -O0 without saying so, 2g.86).

`fid run` and `fid emit` need nothing else. `fid ccompile` (and
`fid run --leaks`) also need a C compiler: `zig cc` works on every host
(`CC="zig cc"`), and `cl` or `tcc` on Windows.

Check the install:

```
$ fid version
fid ...
...
```

The first line is `fid` and the version (`fid 1.0.0+dev (self-hosted
Fides compiler)` for the development build this book was last checked
with, Nov 21, 2026); the next lines say how the binary was built.

then run a first program: `fid run examples\hello.fd` from the install
directory (the installer puts `hello.fd` in `examples\`), or
`fid run tests/bootstrap/hello.fd` from a checkout, or save chapter 2's
`hello.fd` anywhere and run it.

## Where things are

In a checkout of the repository:

```
compiler/    the self-hosted compiler (the product)
stdlib/      the standard modules (docs/stdlib/ has a page for each)
tools/       standalone tool entry files (ark/scribe/veil)
src/         frozen Rust bootstrap compiler (builds stage 1)
tests/       bootstrap + selfhost suites
docs/        this book, reference, guides, roadmaps
examples/    small programs
```

## What it is not (yet)

- Not stable: the language still moves; `docs/reference/stability.md`
  tracks what is promised.
- Not a cargo-style ecosystem yet: `ark` (chapter 7) installs pacts from
  a path, git or the pacts index (pledgeandgrow/pacts), which has few
  packages yet.
- Not proven-safe for kernels today: bare-metal/`@firmware`/`@interrupt`
  exist and `fides-os` exercises them, but the runtime is young.

## Common mistakes

- **A `fid` that can't find its standard library.** `fid` looks for
  `stdlib/core.fd` in `./stdlib`, then `$FID_HOME/stdlib`, then
  `stdlib/` beside the executable or one directory up (where the
  installer and the Linux archive put it). A `fid` you copied somewhere
  else fails every program with `Stdlib not found: stdlib/core.fd`: set
  `FID_HOME` to the directory that holds `stdlib/`. And because
  `./stdlib` comes first, running from a directory with a `stdlib/` of
  its own (an old checkout) uses that one.
- **`fid ccompile` without a C compiler.** With no `CC` set, `fid` looks
  for `gcc`, `clang`, `tcc` and `cl.exe` (`cc` too on Linux); with none
  of them the build ends with `C compilation failed`. Set
  `CC="zig cc"`, or use `fid emit`, which needs no C compiler.
- **Using `fidesc` for your programs.** `fidesc.exe` is the frozen Rust
  bootstrap compiler. It lacks generics checks, safety annotations,
  borrowed strings and much else this book uses; `fid` is the compiler.
- **Reading the numbers in this book as promises.** The language is not
  stable yet (`docs/reference/stability.md`); when the book and `fid`
  disagree, `fid` and `docs/reference/reference.md` win, and the book is
  wrong: please report it.
