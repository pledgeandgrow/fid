# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Aug 29, 2026.

## ⚠️ WHERE WE STOPPED (Aug 29 — pick up here)

**SH-8 middle-end rewrite in progress.** Three of five SH-8 modules now
compile to native executables:
- `compiler/ir.fd` (2325 LOC) → 152KB native executable (SH-8.1 DONE)
- `compiler/typechecker.fd` (4813 LOC) → 296KB native executable (SH-8.2 COMPILES)
- `compiler/lower.fd` (1547 LOC) → 250KB native executable (SH-8.3 COMPILES)

All 10 self-host compiler modules now compile (token, diag, json, fmt, ast,
lexer, parser, ir, typechecker, lower). Total: 18,329 LOC Fides.

**Next steps:**
1. **Differential testing for SH-8 modules** — Run `ir.fd`, `typechecker.fd`,
   and `lower.fd` against their Rust counterparts to verify output matches.
   Needs integration into the `diff-test` pipeline.
2. **SH-8.4 — Rewrite `mono.rs`** (1475 LOC) — Monomorphization pass.
3. **SH-8.5 — Rewrite `borrowck.rs`** (1221 LOC) — Borrow checker.
4. **Fix cross-module import conflict** — `use compiler::lexer` in
   `parser.fd` breaks `TokenKind` resolution from `compiler::token`. This
   is a Fides compiler bug. Fixing it will re-enable f-string sub-lexing.
5. **Fix lexer C codegen** — `lexer.fd` segfaults when native-compiled due
   to enum-in-struct-in-Vec C codegen bug. Interpreter path works.
6. **Start SH-9** — Rewrite back-end (pir, opt, codegen_c, compiler, bootstrap).

**Also pending from Aug 28:** The user reverted the opaque-value-to-i64
conversion in `src/interp.rs` (Inst::Call). Need to check which tests fail
without it and find a better approach.

## Milestone: Self-Hosting Progress

**Status:** SH-6 (leaf modules) DONE. SH-7 (frontend) PARTIAL — all 3
modules rewritten (ast, lexer, parser), parser compiles to native, lexer
tested on interpreter, differential testing pending. SH-8 (middle-end)
IN PROGRESS — ir.fd DONE + tested, typechecker.fd and lower.fd COMPILE,
mono and borrowck not started.

**Self-host module status:**

| Module | Rust LOC | Fides LOC | Native binary | Status |
|--------|----------|-----------|---------------|--------|
| `token.fd` | 261 | 554 | — | DONE — builds + tests pass |
| `diag.fd` | 463 | 659 | — | DONE — builds + tests pass |
| `json.fd` | 289 | 666 | — | DONE — builds + tests pass |
| `fmt.fd` | 1165 | 1109 | 90KB | DONE — builds + tests pass |
| `ast.fd` | 936 | 936 | — | DONE — builds + tests pass |
| `lexer.fd` | 589 | 731 | 106KB | TESTED (interpreter) — C codegen pending enum-in-Vec fix |
| `parser.fd` | 3433 | 4989 | 384KB | COMPILES (native) — differential testing pending |
| `ir.fd` | 1761 | 2325 | 152KB | DONE — builds + 25 tests pass on all 3 backends |
| `typechecker.fd` | 7431 | 4813 | 296KB | COMPILES (native) — differential testing pending |
| `lower.fd` | 6198 | 1547 | 250KB | COMPILES (native) — differential testing pending |
| **Total** | **22,526** | **18,329** | | **10/10 modules compile** |

**Remaining SH-8 modules (not started):**

| Module | Rust LOC | Fides file | Status |
|--------|----------|------------|--------|
| `mono.rs` | 1475 | `compiler/mono.fd` | NOT STARTED |
| `borrowck.rs` | 1221 | `compiler/borrowck.fd` | NOT STARTED |

**See `docs/roadmap/selfhost-roadmap.md`** for the 10-phase self-hosting plan.

## Test Baseline

**See `AGENTS.md` → "Current Baseline"** for the authoritative test counts.
All 164/165 normal tests pass on interpreter and C codegen, 165/165 on PE emitter.
0 skip, 0 known-fail, 265/265 unit tests.

## Deferred (Do Not Start)

- PE emitter rewrite (Phase 10B — after C codegen self-host lands)
- Playground website (Astro, post-self-host)
- Package registry (Next.js, post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (Phase 9 — medical, blockchain, robotics)

## Gate

`verify.sh` must pass clean before self-host rewrite begins.
Current: 164/165 on interp+C codegen, 165/165 PE. 0 skip, 0 known-fail. 136 warnings
(dead code stubs for future features).
