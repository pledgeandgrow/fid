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

## Current Status (Sep 5, 2026 — session 14)

- **🎉 Self-compilation works!** — `fides-c.exe` compiles `compiler/main.fd` → `fides-c2.exe`
- **🎉 PE emitter ported to Fides!** — `compiler/pe.fd` (4,845 lines) generates native Windows PE executables without tcc
- **🎉 comptime_eval.fd fully ported!** — process_gen, substitute, I-5A aggressive comptime, I-5D loop unrolling, reflection
- **🎉 Linux native codegen ported!** — `peephole.fd` + `regalloc.fd` + `codegen_x64.fd` (session 8), builds OK, not yet integrated
- **🎉 LSP server ported to Fides!** — `compiler/lsp.fd` (1,082 lines) — JSON-RPC over stdio, diagnostics, completion, hover, definition, rename (session 9)
- **🎉 flat_ast.fd ported!** — `compiler/flat_ast.fd` (1,259 lines) — flat AST serialization for `dump-ast-flat` command (session 9)
- **🎉 Module-level `var`/`pub var` implemented!** — Rust compiler now supports mutable globals across all backends (interp, C, PE, WASM, ARM64, RISC-V). `test_globals.fd` passes on interp, C, and PE. (session 11)
- **🎉 Category B codegen parity gaps resolved!** (session 14):
  - B5 (float bit-pattern) — **FIXED**: `p_f64_to_bits`/`p_f32_to_bits` runtime + union compound literals in ConstFloat codegen
  - B6 (print/println dispatch) — **AT PARITY**: all type cases (Vec/HashMap/Tensor/F64/Bool/Ptr) match Rust
  - B7 (runtime sigs) — **AT PARITY**: `p_malloc_persistent`/`p_arena_save`/`p_arena_restore` registered
  - B8 (Vec word_repr) — **AT PARITY**: `lower_method_call` uses `word_repr(elem_ty)`; `infer_expr_type` matches Rust by design
- **Box<T> unbox helpers centralized** — `box_expr_unbox`, `box_type_unbox`, `box_pattern_unbox` now in `ast.fd` (session 9)
- **`.exe` extension auto-appended** — `ccompile` and `build` now produce `.exe` output without manual rename (session 8)
- **`build` command** — `fides-c.exe build tests/hello_pe.fd -o hello_pe.exe` → working .exe
- **`lsp` command** — `fides-c.exe lsp` starts the LSP server (session 9)
- **PE runtime stubs** — 25+ stubs (print, string, vec, map, float, bool) for native PE executables
- **All pipeline stages pass** during self-compilation (lex, parse, typecheck, borrowck, mono, lower, C codegen, tcc link)
- **SRET fixed** — struct returns via out-parameter convention now work
- **Enum `==` fixed** — all enum comparisons use `*_eq()` helper functions
- **String builder** — `p_sb_*` eliminates O(n²) memory in C codegen
- **23/23 self-host tests pass** on interp + PE (tests/compiler/)
- **Bootstrap fixpoint not yet verified** — fides-c2.exe crashes during type checking (TypeChecker_lookup null deref); need to fix Category A crash blockers and verify stage2.c == stage3.c

### Codebase size (session 14)
| Component | Files | LOC |
|-----------|-------|-----|
| Self-hosted compiler (`compiler/*.fd`) | 25 | 57,021 |
| Rust compiler (`src/*.rs`) | 52 | 84,447 |
| Stdlib (`stdlib/*.fd`) | 69 | 16,090 |
| Test files (`tests/*.fd` + `tests/compiler/*.fd`) | 225 + 23 | — |

### Test results (session 14)
| Backend | Result |
|---------|--------|
| Interpreter | 209/211 passed |
| C Codegen | 198/211 matched |
| PE Emitter | 209/211 valid |
| Self-host (tests/compiler/) | 23/23 pass on interp + PE |

Pre-existing failures (documented in known-bugs.md §2b.8): test_codegen_c, test_lower, test_banking, test_deep_nesting, test_new_features, test_parallel, test_parallel_concurrent, test_phase8b, test_rc_arc, test_regression_collections, test_vec_math, test_token_full, test_fmt_real — all related to the `var` feature implementation (session 11).

### Remaining steps to fixpoint:
1. ~~Fix `.exe` extension in self-hosted compiler output~~ — DONE (session 8)
2. ~~Port LSP server to Fides~~ — DONE (session 9)
3. ~~Category B codegen parity gaps~~ — DONE (session 14: B5 fixed, B6/B7/B8 at parity)
4. **Fix Category A crash blockers** (current: TypeChecker_lookup null deref):
   - A1: TypeChecker scope lookup null safety — add null checks after Vec_get
   - A2: TypeChecker_register_program missing fields
   - A3: TypeChecker `check` return type — collect errors like Rust
   - A4: Statement-dropping bug (root cause or workaround)
   - A5: Type alias cross-module visibility
5. **Fix remaining Category B gaps** (B1-B4, B9-B10 — low priority, not used by compiler source):
   - B1: `generate_c` entry point
   - B2: DynCall/TryUnwrap stubs
   - B3: Allocator builtins stubs
   - B4: Bare-metal builtins stubs
   - B9: Async/await lowering
   - B10: Closure lowering
6. **Mirror `var` feature to `compiler/*.fd` files** — Rust compiler has it, Fides self-hosted compiler does not yet
7. **Fix `inc.fd` global_cache_* using `var` feature** — now possible since Rust compiler supports `var`
8. **Fix Category E fixpoint stability risks**:
   - E1: Sort HashMap keys before output
   - E2: Zero-initialize all allocas
   - E6: Gate 38 debug `p_eprint_str` calls behind flag
9. Verify `fides-c2.c == fides-c3.c` (the fixpoint check)
10. Extend PE emitter to support full compiler source (struct/enum, Vec/HashMap, floats)
11. Integrate Linux native codegen (port `elf64.rs`, `linker.rs`, `runtime_linux.rs`, wire into pipeline)
12. ~~Assess debugger.rs porting feasibility~~ — **ASSESSED (session 9): NOT feasible without porting interp.rs (5,834 LOC, 251 runtime handlers, closures for debug hooks). Skip — debugger is a dev tool, not needed for self-hosting.**

See `docs/roadmap/selfhost-roadmap.md` for the comprehensive gap analysis (Categories A–F) and `docs/selfhost/known-bugs.md` for the full bug history.

## For Parallel Sessions

If you run multiple agent sessions simultaneously:

1. **Every session reads `COORDINATION.md` first.**
2. **Every session adds itself to "Active Sessions"** with the files it's editing.
3. **No two sessions edit the same file.**
4. **Architectural decisions are recorded immediately** in `COORDINATION.md`.
5. **`src/lower.rs` and `compiler/lower.fd` must stay in sync** — if one changes, the other needs the same change.
6. **`src/codegen_c.rs` and `compiler/codegen_c.fd` must stay in sync.**
