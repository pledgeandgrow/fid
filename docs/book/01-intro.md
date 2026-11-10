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
  compile time, frees inserted for you. No lifetimes — references can't
  escape, so they don't need them.
- **Effects as types**: functions declare what they do (`pure`, `!read`,
  `!alloc`, diverge). The optimizer uses it; wrong declarations fail to
  compile or fail honestly.
- **Annotation-driven contracts**: `@crashfree`, `@no-alloc`,
  `@wrapping`, `@send`, `@secret`, `@repr(C)`, `@packed`, `@simd`,
  `@interrupt`, `@firmware` — for services, kernels, and crypto alike.
- **Self-hosted and fixpoint-verified**: the compiler compiles itself
  byte-for-byte identically (stage 2 == stage 3 on every release build).

## Install

- **Windows installer**: `installer/` builds `Fides-Setup-<v>.exe`
  (NSIS). It installs `fid.exe`, `fidesc.exe`, `ark.exe`, `scribe.exe`,
  `veil.exe` and the stdlib.
- **From source**: `RELEASE=1 ./scripts/bootstrap.sh` (3 stages,
  fixpoint checked → `release/fid.exe`). For development,
  `./scripts/build-selfhost.sh` builds a single-stage `release/fid-dev.exe`.

Check:

```bash
fid version
fid run examples/hello.fd    # if your checkout has it, else tests/bootstrap/hello.fd
```

## Where things are

```
compiler/    the self-hosted compiler (the product)
stdlib/      137 standard modules
tools/       standalone tool entry files (ark/scribe/veil)
src/         frozen Rust bootstrap compiler (builds stage 1)
tests/       bootstrap + selfhost suites
docs/        this book, reference, guides, roadmaps
examples/    small programs
```

## What it is not (yet)

- Not stable: the language still moves; `docs/reference/stability.md`
  tracks what is promised.
- Not a cargo-style ecosystem: `ark` manages local/git pacts, no registry.
- Not proven-safe for kernels today: bare-metal/`@firmware`/`@interrupt`
  exist and `fides-os` exercises them, but the runtime is young.
