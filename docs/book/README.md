# The Fid Book (draft)

> A guide from your first program to running a service. **Fid** is the
> language, **Fides** the compiler, `fid` the command. Status: draft —
> every chapter is written and was checked example by example against
> `fid` 0.8.0-unstable (chapters 5–8 and 10 on Nov 15, 1–4 and 9 on
> Nov 17, 2026); each says where that build falls short.
> Reference material lives in `docs/reference/`; this book teaches rather
> than enumerates.

1. [Introduction & install](01-intro.md)
2. [Your first program](02-first-program.md)
3. [A language tour](03-tour.md)
4. [Ownership and memory](04-ownership.md)
5. [Effects and annotations](05-effects.md)
6. [Errors](06-errors.md)
7. [The toolchain](07-toolchain.md)
8. [Concurrency](08-concurrency.md)
9. [Writing a service](09-service.md)
10. [Shipping](10-shipping.md)

Every chapter follows one pattern: the concepts, then complete programs
you can run, then the common mistakes.

## Checking the book

`tests/book/check_book.py` runs the book: it writes every complete
example of a chapter into a directory, runs each terminal session the
book shows (`$ fid run x.fd` and the output after it, `$ echo $?`), and
checks that the remaining files type-check, or are rejected when their
name ends in `_fail.fd`. `--native` also builds each program with
`fid ccompile` and `fid emit` and compares the output.

```bash
python3 tests/book/check_book.py --fid release/fid.exe            # every chapter
python3 tests/book/check_book.py docs/book/04-ownership.md --fid release/fid.exe --native -v
```

Run it at each release, and fix the text, the example, or the compiler
when it fails.

## Reading paths

- **Want the shortest route to running code?** Chapters 1 → 2 → 9 → 10.
- **Coming from another language?** Start with
  `docs/guides/from-{rust,go,zig,c,python}.md`, then chapter 4 for the
  ownership model and chapter 6 for errors.
- **Writing code that must not fail?** Chapters 5 and 6.
- **Contributing to the compiler?** This book won't help — read
  `docs/compiler-internals.md` and `AGENTS.md`.

## Conventions

- An example that starts with a `// name.fd` comment is a complete file:
  run it with `fid run name.fd`. The output shown after it is what `fid`
  printed; `...` stands for lines (or the rest of a line) that depend on
  the build, such as the frames of a stack trace. A file whose name ends
  in `_fail.fd` shows an error: the compiler rejects it. Shorter snippets
  show one point and need surrounding code.
- Build a native binary with `fid emit x.fd` (Windows PE, or Linux ELF via
  `--target x86_64-linux`, `aarch64-linux`, `riscv64-linux`), or through C
  with `CC="zig cc" fid ccompile x.fd -o x`.
- `i64` is the default integer; `str` is an immutable, length-tagged string;
  `val` binds immutably, `var` mutably; `fn main() -> i64` is the entry,
  and its result is the exit status.
