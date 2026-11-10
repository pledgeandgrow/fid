# flat_ast.fd

> **Re-audited 2026-11-09** against the current source and the latest `fid` build (s18).
> Still dead, and now it no longer type-checks.

- **Path:** `compiler/flat_ast.fd`
- **LOC:** 1,316 (was 1,287; Rust `src/flat_ast.rs`: 780)
- **Role:** `Program` to an indented line-per-node text (`type_flat`, `expr_flat`, `stmt_flat`,
  `func_flat`, `program_flat`). Meant for `dump-ast-flat` and parser differential tests.

## Callers

| Caller | Site | What |
|---|---|---|
| — | — | Nothing. No `use compiler::flat_ast` anywhere in `compiler/`, `stdlib/` or `tests/`. |

`fid dump-ast-flat` is an alias of `cmd_ast` (`main.fd:157`), which prints a short summary
(`main.fd:402-446`, comment "no full AST formatter yet"). The Rust side wires it
(`src/compiler.rs:1676` → `flat_ast::program_flat`). AGENTS.md lists the module among the 8
that nothing reachable from `main.fd` imports.

## What the module does now

- Covers all 41 `Expr` and all 33 `Stmt` variants, including `Resume`, `SpawnScope`,
  `CancelScope` (:1067-1090, added Oct 24-25). Every `Type` variant is handled.
- No `_ =>` fallthroughs.

## Findings

### Verified live

- **HIGH — NEW — the module does not compile.** `fid check compiler/flat_ast.fd`:
  `non-exhaustive match at line 580 in compiler::flat_ast — missing variant(s): AsmDir::Clobber`.
  The `Expr::Asm` arm matches only `In` and `Out` (:580-583). `AsmDir::Clobber` was added to
  ast.fd after this file was last touched. Nothing notices because nothing imports it.
- **HIGH — STILL TRUE — `dump-ast-flat` is not wired.** It prints the summary, not the tree.

### From code inspection

- **LOW — STILL TRUE — repeated trailing-newline trim** (`str_ends_with(s, "\n")` then slice)
  about 15 times; a helper would do.
- **LOW — quadratic `esc`.** `esc` appends one character at a time with `str_concat` (:21-49).

## Fix order

1. Add `AsmDir::Clobber => { dir = "Clobber"; }` at :582.
2. Either wire it (`use compiler::flat_ast` in main.fd, `dump-ast-flat` → `program_flat` on the
   parsed user program) or delete the module.

## Verification state

- `fid check compiler/flat_ast.fd`: 1 error (above).
- Importers: grep over `compiler/`, `stdlib/`, `tests/` found none.
- `fid lint` cannot check it (lint does not resolve `compiler::` imports: it reports
  `undefined function 'box_type_unbox'`).
