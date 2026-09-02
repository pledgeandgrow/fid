# Current Focus

> Single source of truth for "what are we doing right now."
> Read this first every session. Updated after every milestone change.
> Last updated: Feb 10, 2026.

## Where We Stopped (Feb 10 — pick up here)

**Crash fixed. Self-hosted compiler runs to completion but has a lowerer bug
in enum variant extraction that causes parse errors.**

### What works:
1. **Build pipeline complete** — `compiler/*.fd` (18 modules, 39.5K LOC) compiles to C via the Rust compiler, then links to `release/fides-c.exe` via tcc
2. **All 18 self-host modules compile + pass tests** (23/23 on tests/compiler/ via interpreter + PE)
3. **Arena allocator** (chunk-based, never invalidates pointers) in both `src/codegen_c.rs` and `compiler/codegen_c.fd`
4. **Windows compatibility** — tcc builds work with ws2_32.def linking, PE stack patched to 256MB, 16MB thread stack wrapper
5. **Safety hardening (SH-12 DONE)** — recursion limit, OOM abort, crash handlers, all ported to both Rust and Fides codegen
6. **Progressive stage reporting** — `p_eprint_str` calls in `compiler.fd` and `main.fd` print each stage to stderr
7. **SRET calling convention** — Fides lowerer now correctly handles struct returns via out-parameter convention for both regular calls and method calls
8. **Enum layout computation** — Fides lowerer now computes enum payload sizes from actual field sizes, with iterative fixpoint for nested types
9. **Lexer optimization** — Cached `src_len` to avoid O(n) `str_len` on every peek; `str_slice` instead of char-by-char `str_concat` for identifiers/numbers

### Current blocker:
The self-hosted compiler runs to completion but reports parse errors when
parsing `stdlib/core.fd`:
```
Parse error: assignment target must be an identifier or array index at line 298 col 15
```
This occurs at `*p0 = 0;` — a dereference assignment. The `Expr::Deref`
variant is not matched because the `Expr` value is truncated to 8 bytes
during enum variant extraction.

**Root cause:** When matching `ParseResultExpr::Ok(e) => { left = e; }`,
the Fides lowerer emits `Inst::Load` which only reads 8 bytes for
struct-typed fields (because `c_type(Struct(_))` is a pointer type in C).
The full `Expr` (80 bytes) is truncated to just the tag field.

**Fix already done in Rust lowerer:** `load_enum_field()` helper in
`src/lower.rs` (line 246) allocates a buffer and uses `Inst::Store`
(which emits `memcpy` for structs) to copy the full inline struct data.

### Build commands:
```
cargo build --release                                    # → target/release/fidesc.exe
.\target\release\fidesc.exe ccompile compiler\main.fd -o release\fides-c   # → release/fides-c.c
tcc -O2 -w -o release\fides-c.exe release\fides-c.c C:\tcc\lib\ws2_32.def  # → release/fides-c.exe
# Patch PE stack to 256MB (see build-selfhost.ps1)
```
Or just: `.\build-selfhost.ps1`

### Next steps:
1. **Port `load_enum_field()` to `compiler/lower.fd`** — allocate buffer + use `Store` (memcpy) for struct-typed enum variant fields instead of `Load`
2. **Apply at both enum match extraction sites** in `compiler/lower.fd`
3. **Ensure `Inst::Store` in `compiler/codegen_c.fd` emits `memcpy` for struct types** (like the Rust codegen does)
4. **Once parse errors are fixed**: verify the bootstrap fixpoint (stage2.c == stage3.c)
5. **Run full test suite** through the self-hosted compiler

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
