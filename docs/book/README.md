# The Fides Book (draft)

> A guide from your first program to running a service. Status: early
> draft — chapters marked ✓ are written, ◐ are stubs, ○ are planned.
> Reference material lives in `docs/reference/`; this book teaches rather
> than enumerates.

1. [✓ Introduction & install](01-intro.md)
2. [✓ Your first program](02-first-program.md)
3. [✓ A language tour](03-tour.md)
4. [✓ Ownership and memory](04-ownership.md)
5. [◐ Effects and annotations](05-effects.md)
6. [◐ Errors](06-errors.md)
7. [◐ The toolchain](07-toolchain.md)
8. [◐ Concurrency](08-concurrency.md)
9. [✓ Writing a service](09-service.md)
10. [◐ Shipping](10-shipping.md)

## Reading paths

- **Want the shortest route to running code?** Chapters 1 → 2 → 9.
- **Coming from another language?** Start with `docs/guides/from-{rust,go,zig,c}.md`,
  then chapter 4 for the ownership model.
- **Contributing to the compiler?** This book won't help — read
  `docs/compiler-internals.md` and `AGENTS.md`.

## Conventions

- Every example is a complete file — run it with `fid run x.fd`.
- Build a native binary with `fid emit x.fd` (Windows PE or Linux ELF via
  `--target x86_64-linux`, `aarch64-linux`, `riscv64-linux`), or through C
  with `fid ccompile x.fd -o x`.
- `i64` is the default integer; `str` is an immutable, length-tagged string;
  `val` binds immutably, `var` mutably; `fn main() -> i64` is the entry.
