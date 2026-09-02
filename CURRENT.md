# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Feb 9, 2026.

## Where We Stopped (Feb 9 — pick up here)

**Self-hosted compiler builds into `release/` but crashes on use. Safety hardening done.**

### What works:
1. **Build pipeline complete** — `compiler/*.fd` (18 modules, 39.5K LOC) compiles to C via the Rust compiler, then links to `release/fides-c.exe` (7.9MB) via tcc
2. **All 18 self-host modules compile + pass tests** (23/23 on tests/compiler/ via interpreter + PE)
3. **Arena allocator** (chunk-based, never invalidates pointers) in both `src/codegen_c.rs` and `compiler/codegen_c.fd`
4. **Windows compatibility** — tcc builds work with ws2_32.def linking, PE stack patched to 256MB
5. **Test reorganization** — `compiler/tests/` moved to `tests/compiler/`, root stray files cleaned up
6. **Build dirs clean**: `target/` (Rust), `release/` (self-hosted), no more `target2/`
7. **Safety hardening (SH-12 DONE)** — C codegen recursion limit (p_max_recursion=10000), p_malloc OOM detection (abort on alloc failure), interpreter --max-steps flag (infinite loop prevention), test runner step limit (50M), Windows path fixes (temp dir), tcc auto-detection
8. **Interpreter opaque_values bug fixed** — stale Value::Str entries cleared on non-opaque Store, fixing string loss during SRET operations

### Current blocker:
`release/fides-c.exe` crashes with `STATUS_ACCESS_VIOLATION` (0xC0000005) when compiling any `.fd` file. The crash occurs in `Lexer_tokenize` at stdlib position ~69845 (around `byte_to_str` in `stdlib/core.fd`). This is a pre-existing bug — it crashes even with `p_malloc` reverted to plain `malloc`.

### Build commands:
```
cargo build --release                                    # → target/release/fidesc.exe
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c   # → release/fides-c.c
tcc -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def  # → release/fides-c.exe
# Patch PE stack to 256MB (see build-selfhost.ps1)
```
Or just: `.\build-selfhost.ps1`

### Next steps:
1. **Fix the Lexer_tokenize crash** at stdlib position ~69845
   - Investigate if it's a tcc codegen bug (try gcc/clang)
   - Add targeted debug prints to narrow down the exact failing operation
   - Check if `alloca` in tcc is causing stack exhaustion in deep recursion
2. **Once crash is fixed**: verify the bootstrap fixpoint (stage2.c == stage1.c)
3. **Run full test suite** through the self-hosted compiler

## Self-Host Module Status

All 18 modules rewritten in Fides. 23/23 self-host tests pass on interpreter + PE emitter.

| Module | Rust LOC | Fides LOC | Status |
|--------|----------|-----------|--------|
| `token.fd` | 261 | 517 | DONE |
| `diag.fd` | 463 | 594 | DONE |
| `json.fd` | 289 | 628 | DONE |
| `fmt.fd` | 1165 | 1007 | DONE |
| `ast.fd` | 936 | 920 | DONE |
| `lexer.fd` | 589 | 673 | DONE |
| `parser.fd` | 3433 | 4800 | DONE |
| `ir.fd` | 1761 | 2139 | DONE |
| `typechecker.fd` | 7431 | 4848 | DONE |
| `lower.fd` | 6198 | 4444 | DONE |
| `mono.fd` | 1475 | 2422 | DONE |
| `borrowck.fd` | 1221 | 1731 | DONE |
| `pir.fd` | — | 2432 | DONE |
| `opt.fd` | — | 6022 | DONE |
| `codegen_c.fd` | — | 4529 | DONE |
| `compiler.fd` | — | 1641 | DONE |
| `comptime_eval.fd` | — | 35 | DONE |
| `main.fd` | — | 120 | DONE (entry point) |
| **Total** | | **39,502** | **18/18 modules** |

## Test Baseline (Feb 9, 2026)

```
Unit tests:   263/265 passed  (2 failing — recent lower.fd changes)
Interpreter:  191/194 normal
C Codegen:    74/194 matched   (tcc integration, Windows path fixes)
PE Emitter:   193/194 valid
Self-host:    23/23 pass on interp + PE (tests/compiler/)
Total .fd:    207 in tests/ + 23 in tests/compiler/ = 230
Stdlib:       69 modules, 16,069 LOC
Source:       52 .rs files, 82,778 LOC
```

## Build Directory Layout

```
target/    ← Rust compiler output (fidesc.exe) — gitignored
release/   ← Self-hosted compiler output (fides-c.c, fides-c.exe) — gitignored
tests/     ← Rust compiler tests (207 .fd files)
tests/compiler/  ← Self-host module tests (23 .fd files)
tests/selfhost/  ← Harness/probe files for differential testing
compiler/  ← Self-hosted compiler source (18 .fd modules)
stdlib/    ← Fides standard library (69 .fd modules)
```

## Deferred (Do Not Start)

- PE emitter rewrite (after C codegen self-host lands)
- Playground website (post-self-host)
- Package registry (post-self-host)
- New language features (add on Rust until self-host lands)
- Industry domain stdlibs (medical, blockchain, robotics)

## Gate

`verify.sh` must pass clean before self-host rewrite begins.
Current: 191/194 interp, 74/194 C codegen (tcc + path fixes), 193/194 PE.
