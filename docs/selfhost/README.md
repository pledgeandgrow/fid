# Fides Self-Hosted Compiler — Documentation

> **Read `COORDINATION.md` FIRST, before any other file in this folder.**
> It tracks active sessions, architectural decisions, and file ownership.
> Without it, parallel agents will conflict and undo each other's work.

## Files in this folder

| File | Purpose | Read order |
|------|---------|------------|
| `COORDINATION.md` | **READ FIRST.** Active sessions, architectural decisions, file ownership, pre-flight checklist | 1 |
| `vision-analysis.md` | Are we on the right path? What the language can do now, what's missing | 2 |
| `capability-comparison.md` | What Fides can do today + honest comparison vs C/Rust/Zig/Go/Python | 3 |
| `path-to-fixpoint.md` | **The steps from current state to self-reproduction** | 4 |
| `CURRENT.md` (repo root) | What are we doing right now (not in this folder) | 5 |
| `architecture.md` | File-by-file breakdown of every `compiler/*.fd` module (including `pe.fd`) | 4 |
| `language-constraints.md` | What the Fides language must respect: types, effects, regions, patterns, closures, FFI, defer, try | 5 |
| `calling-conventions.md` | SRET, enum layout, struct copy, Box semantics, Vec handling — the low-level machinery | 6 |
| `build-pipeline.md` | How to build, test, bootstrap, and verify the self-hosted compiler (including PE emitter `build` command) | 7 |
| `known-bugs.md` | Known bugs, gotchas, and things that have been fixed (do not re-introduce) | 8 |
| `port-gap-analysis.md` | Rust vs Fides module comparison — what's missing in each port | 9 |
| `agent-guide.md` | Best practices and rules for agents working on the compiler | 10 |

> **Note:** `calling-conventions.md` and `known-bugs.md` contain some outdated
> content about the inline + memcpy approach for enum payloads. The current
> direction is the **pointer-based approach** — see `COORDINATION.md` [D1].
> Those docs will be updated when the approach is finalized.

## Quick Start

```bash
# 1. Build the Rust host compiler
cargo build --release

# 2. Use it to compile the self-hosted compiler (Fides → C → .exe)
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c
tcc -O2 -w -o release\fides-c.exe release\fides-c.c -lws2_32
# (patch PE stack to 256MB — see build-pipeline.md)

# 3. Test the self-hosted compiler
./verify-selfhost.sh

# 4. Verify the Rust compiler's test suite still passes
./verify.sh

# 5. NEW: Generate a native PE executable directly (no tcc!)
.\release\fides-c.exe build tests\hello_pe.fd -o release\hello_pe.exe
.\release\hello_pe.exe
# → Hello from PE!
```

## Current Status (Sep 5, 2026 — session 11)

- **🎉 Self-compilation works!** — `fides-c.exe` compiles `compiler/main.fd` → `fides-c2.exe`
- **🎉 PE emitter ported to Fides!** — `compiler/pe.fd` generates native Windows PE executables without tcc
- **🎉 comptime_eval.fd fully ported!** — process_gen, substitute, I-5A aggressive comptime, I-5D loop unrolling, reflection
- **🎉 Linux native codegen ported!** — `peephole.fd` + `regalloc.fd` + `codegen_x64.fd` (session 8), builds OK, not yet integrated
- **🎉 LSP server ported to Fides!** — `compiler/lsp.fd` (1084 lines) — JSON-RPC over stdio, diagnostics, completion, hover, definition, rename (session 9)
- **🎉 flat_ast.fd ported!** — `compiler/flat_ast.fd` (1260 lines) — flat AST serialization for `dump-ast-flat` command (session 9)
- **🎉 Module-level `var`/`pub var` implemented!** — Rust compiler now supports mutable globals across all backends (interp, C, PE, WASM, ARM64, RISC-V). `test_globals.fd` passes on interp, C, and PE. (session 11)
- **🎉 Full static analysis of all 24 `compiler/*.fd` files completed!** — Every module analyzed for stubs, bugs, and gaps. 5 P0 and 17 P1 issues identified. (session 11)
- **Box<T> unbox helpers centralized** — `box_expr_unbox`, `box_type_unbox`, `box_pattern_unbox` now in `ast.fd` (session 9)
- **`.exe` extension auto-appended** — `ccompile` and `build` now produce `.exe` output without manual rename (session 8)
- **`build` command** — `fides-c.exe build tests/hello_pe.fd -o hello_pe.exe` → working .exe
- **`lsp` command** — `fides-c.exe lsp` starts the LSP server (session 9)
- **PE runtime stubs** — 25+ stubs (print, string, vec, map, float, bool) for native PE executables
- **All pipeline stages pass** during self-compilation (lex, parse, typecheck, borrowck, mono, lower, C codegen, tcc link)
- **SRET fixed** — struct returns via out-parameter convention now work
- **Enum `==` fixed** — all enum comparisons use `*_eq()` helper functions
- **String builder** — `p_sb_*` eliminates O(n²) memory in C codegen
- **22/23 self-host tests pass** on all 3 backends (1 pre-existing failure: test_codegen_c.fd)
- **Bootstrap fixpoint not yet verified** — fides-c2.exe crashes on stdlib (statement-dropping bug); need to fix and verify stage2.c == stage3.c

### Remaining steps to fixpoint:
1. ~~Fix `.exe` extension in self-hosted compiler output~~ — DONE (session 8)
2. ~~Port LSP server to Fides~~ — DONE (session 9)
3. **Fix pre-existing parse error at codegen_c.fd:973** — blocks rebuilding fides-c.exe with current code
4. Fix the statement-dropping bug in the Fides compiler (root cause of fides-c2.exe crash on stdlib/core.fd)
5. **Fix 5 P0 bugs in parser.fd and lexer.fd** (session 11 analysis):
   - `lexer.fd:291` — remove duplicate empty `str_filter_underscores` stub
   - `parser.fd` — rewrite `box_expr_deref` to match actual `ast.fd` Expr enum
   - `parser.fd` — add missing `return` in 4 parse functions
   - `parser.fd` — remove duplicate `var has_forward` in `parse_layer`/`parse_model`
   - `parser.fd:4684` — fix `parse_use` version-pin to only error on missing string
6. **Mirror `var` feature to `compiler/*.fd` files** — Rust compiler has it, Fides self-hosted compiler does not yet
7. **Fix `inc.fd` global_cache_* using `var` feature** — now possible since Rust compiler supports `var`
8. Remove/gate 38 debug `p_eprint_str` calls
9. Verify `fides-c2.c == fides-c3.c` (the fixpoint check)
10. Extend PE emitter to support full compiler source (struct/enum, Vec/HashMap, floats)
11. Integrate Linux native codegen (port `elf64.rs`, `linker.rs`, `runtime_linux.rs`, wire into pipeline)
12. ~~Assess debugger.rs porting feasibility~~ — **ASSESSED (session 9): NOT feasible without porting interp.rs (5782 LOC, 251 runtime handlers, closures for debug hooks). Skip — debugger is a dev tool, not needed for self-hosting.**

See `CURRENT.md` in the repo root for the latest state.

## For Parallel Sessions

If you run multiple agent sessions simultaneously:

1. **Every session reads `COORDINATION.md` first.**
2. **Every session adds itself to "Active Sessions"** with the files it's editing.
3. **No two sessions edit the same file.**
4. **Architectural decisions are recorded immediately** in `COORDINATION.md`.
5. **`src/lower.rs` and `compiler/lower.fd` must stay in sync** — if one changes, the other needs the same change.
6. **`src/codegen_c.rs` and `compiler/codegen_c.fd` must stay in sync.**
