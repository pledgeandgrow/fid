# AGENTS.md — Fides Compiler Working Guide

> This file gives AI assistants (and humans) the context needed to work on the
> Fides compiler safely. Read this before making changes.

## Build & Test Commands

```bash
# Build (release — what verify.sh uses)
cargo build --release

# Build (debug — faster, used by cargo test)
cargo build

# Run all tests across all backends
./target/release/fidesc test

# Full verification (build + test + compare to baseline)
./verify.sh

# Save current test results as new baseline (only after intentional changes)
./verify.sh --save

# Quick build check (no tests)
./verify.sh --quick

# Generate golden files for intermediate compiler stages
./scripts/gen_golden.sh

# Bisect which layer broke a test
./scripts/bisect.sh <test_name>
# Example: ./scripts/bisect.sh strings

# Run a single test through the interpreter
./target/release/fidesc run tests/<name>.fd

# Show IR for a test
./target/release/fidesc ir tests/<name>.fd

# Show PIR (L1/L2/L3) for a test
./target/release/fidesc pir tests/<name>.fd 3

# Show assembly (x86-64)
./target/release/fidesc codegen tests/<name>.fd --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/fidesc codegen tests/<name>.fd --target x86-64 --peephole

# Run the LSP server (for editor integration)
./target/release/fidesc lsp
```

## Current Baseline (as of Aug 2026)

```
Unit tests:   136/136 passed
Harness:      8/8 passed       (golden_files_match now passes)
Interpreter:  99/99 normal     (+ 22/22 expected fail)
C Codegen:    99/99 matched    (FULL PARITY — no gaps) (+ 22/22 expected fail)
PE Emitter:   99/99 valid      (+ 22/22 expected fail: same as interpreter)
Total .fd:    143 test files   (121 normal + 22 expected-fail)
Skip list:    22 tests         (interpreter-only features: comptime, bare metal, WASM, SIMD, tensor, HTTP, JSON, dlopen, etc.)
```

### New Features (Phase 7.7)

- **Iterators**: `for x in vec`, `for k in map`, `for i in 0..N` — Vec, HashMap,
  and Range iteration with nested Vec support. HashMap iterates over keys.
- **Error chaining**: `?` operator on `Option<T>` and `Result<T, E>` propagates
  errors. `try { ... }` blocks catch errors. Option/Result methods: unwrap,
  unwrap_or, is_some, is_none, is_ok, is_err.
- **Conditional compilation**: `@cfg(platform = "linux")`, `@cfg(platform =
  "windows")`, `@cfg(platform = "macos")`, `@cfg(debug)`, `@cfg(release)`.
  Filters out functions and structs at compile time.
- **Derive macros**: `@derive(Debug)` generates `debug_str()` method.
  `@derive(Clone)` generates `clone()` method. Applied to structs.
- **Serde**: `@derive(Serialize)` generates `to_json()` method.
  `@derive(Deserialize)` generates `from_json()` method.
- **Dynamic loading**: `p_dl_open(path)`, `p_dl_sym(handle, symbol)`,
  `p_dl_close(handle)`, `p_dl_call(func_ptr, arg)`. Real dlopen/dlsym in C
  codegen, simulated in interpreter.

## New Features (Phase 11)

- **Generic Enums**: `Option<T>` and `Result<T, E>` are now declared with type
  parameters in `stdlib/core.fd`. The parser accepts `enum Foo<T, U> { ... }`
  and `struct Foo<T> { ... }`. The typechecker treats `Option` and `Option<T>`
  as compatible (uniform i64 representation).
- **LSP Server**: `fidesc lsp` runs a Language Server Protocol server over
  stdio. Supports `initialize`, `textDocument/didOpen`, `didChange`, `didClose`,
  and `publishDiagnostics`. No external dependencies — uses a built-in JSON
  parser/serializer.
- **New Stdlib Modules**:
  - `stdlib/math.fd`: Math functions (sqrt, sin, cos, floor, ceil, pow, fabs, constants)
  - `stdlib/env.fd`: Environment variable access (env_get, env_exists)
  - `stdlib/args.fd`: Command-line arguments (args_len, args_get, args_program_name)
  - `stdlib/process.fd`: Process control (process_exit, process_system)
  - `stdlib/regex.fd`: Simple wildcard pattern matching (regex_matches, regex_find, etc.)

Expected-fail tests (21):
test_trait_verify_fail, test_budget_fail, test_sandbox_fail,
test_deterministic_fail, test_field_privacy_fail, test_money_linear_fail,
test_money_conserve_fail, test_banking_double_credit_fail,
test_banking_lost_money_fail, test_loop_linear_fail, test_gas_fail,
test_crashfree_fail, test_txn_rollback_fail, test_typed_strings_fail,
test_init_guarantee_fail, test_init_guarantee_cond_fail,
test_param_queries_fail, test_medical_fail, test_contracts_fail,
test_safety_annotations_fail, test_safety_no_audit_fail,
test_borrowck_fail

Features now present in the baseline: tuple destructuring, file I/O
(`stdlib/file.fd`), `txn {}` blocks (with interpreter rollback), loop-aware linear types, `@gas`
annotation, `@crashfree` annotation, `@audit` annotation (with full param trail), banking stdlib,
money stdlib, LogicalTime stdlib (`stdlib/time.fd`), typed strings
(`stdlib/typed_strings.fd` — F67: SQLString/ShellString/HTMLString/JSONString/PathString),
init guarantee (F63: `pm` keyword + definite assignment analysis),
raw pointer types (`*mut T` / `*const T` with `unsafe {}` blocks),
memoization stdlib (`stdlib/memo.fd`),
checked arithmetic lock (F64: overflow = panic, always on),
`@wrapping` annotation (F65: escape hatch for wrap-around arithmetic),
bounds checking lock (F62: always on, skippable in `unsafe {}`),
parameterized queries (F68: no string concat on typed strings),
`@medical` annotation (F48: IEC 62304 — no alloc, no recursion, no while, must be @audit),
given/ensure contracts (F10: Design by Contract — preconditions verified at call sites),
composable safety primitives (F44/F52/F56: @no-alloc, @no-recursion, @no-unbounded, @no-panic,
@no-indirect, @requires-audit — @safety-critical, @autosar, @dal-a are aliases that expand to these),
async/await (Phase 7.6: `async pf`, `await <expr>`, `spawn <expr>` — Future<T> type with eager evaluation, typechecker enforces Future<T> for await, `p_future_ready`/`p_future_poll` runtime functions in interpreter + C codegen; state machine transform pending for true cooperative scheduling),
borrow checker (Phases 1-6 complete: `--borrowck=off|warn|on` — Rules 1-3 active, field-wise borrows, two-phase borrows, NLL, `on` by default),
generic enums/structs (`enum Option<T> { ... }`, `struct Foo<T> { ... }` — type_params in AST, parser, typechecker, lowerer),
LSP server (`fidesc lsp` — JSON-RPC over stdio, initialize/didOpen/didChange/diagnostics, no external deps),
new stdlib modules (`stdlib/math.fd`, `stdlib/env.fd`, `stdlib/args.fd`, `stdlib/process.fd`, `stdlib/regex.fd`).

**Note:** `test_db` is no longer flaky — HashMap keys are now sorted in the interpreter for deterministic output.

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state.

## HIGH-RISK FILES — Always run `./verify.sh` after editing these

These files are where silent errors are most dangerous. Edits here can break
everything with no warning. Always run the full verification suite after
touching them.

| File | Risk | Why |
|------|------|-----|
| `src/regalloc.rs` | **CRITICAL** | Linear scan allocator. Has known pre-existing bug: constants not loaded into registers before use can be clobbered by intervening instructions. Any change here affects EVERY function. |
| `src/peephole.rs` | **CRITICAL** | 9 passes, always on in `build`. A wrong transform silently corrupts all output. |
| `src/elf64.rs` | **HIGH** | ELF64 header bytes, section layout, relocations (R_X86_64_PC32/PLT32/64). Off-by-one = corrupt binary. |
| `src/pe.rs` | **HIGH** | PE header bytes, import table, section layout. Off-by-one = corrupt .exe. |
| `src/coff.rs` | **HIGH** | COFF object file format. Same byte-level risk as PE/ELF. |
| `src/macho.rs` | **HIGH** | Mach-O object file format. Same byte-level risk. |
| `src/codegen_x64.rs` | **HIGH** | PIR L3 → x86-64 instruction encoding. ModRM/SIB bytes, REX prefixes, ABI rules. |
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 96/96 test parity (FULL PARITY). |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/borrowck.rs` | **LOW** | Borrow checker (Phases 1-6 complete). Separate pass after typecheck. `--borrowck=on` by default; `off`/`warn` modes available. Field-wise borrows, two-phase borrows, NLL (non-lexical lifetimes via last-use analysis). |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. Optimizer bugs fixed (effect_reorder, inline, TCO, imul). `--opt` is now enabled by default in `build` (use `--no-opt` to disable). Produces matching output on all tests. |
| `src/runtime_linux.rs` | **HIGH** | Hand-encoded x86-64 machine code (raw bytes). No assembler — every byte is manual. |
| `src/linker.rs` | **MEDIUM** | Relocation resolution. Wrong offset = crash or silent corruption. |
| `src/interp.rs` | **LOW** | The reference oracle. If this is wrong, all backends "match" but produce wrong results. |

## LOWER-RISK FILES

| File | Risk | Why |
|------|------|-----|
| `src/lexer.rs` | LOW | Tokenizer — errors are usually loud (parse failures). |
| `src/parser.rs` | LOW | Recursive descent — errors are loud. |
| `src/ast.rs` | LOW | Data definitions — changes propagate visibly. |
| `src/token.rs` | LOW | Token enum — adding tokens is safe. |
| `src/mono.rs` | LOW | Monomorphization — errors surface as missing functions. |
| `src/comptime.rs` | LOW | Compile-time eval — errors are loud. |
| `src/main.rs` | LOW | CLI dispatch — adding commands is safe. |
| `src/compiler.rs` | LOW | Pipeline orchestration — changes are visible. |
| `src/lsp.rs` | LOW | LSP server — JSON-RPC over stdio, no external deps. |
| `stdlib/*.fd` | LOW | Fides source — errors surface as test failures. Stdlib modules: core, db, http, json, parallel, shape, simd, tensor, quant, money, banking, alloc, file, time, typed_strings, memo, privacy, math, env, args, process, regex. |

## Known Bugs (Do Not Re-Introduce)

1. **Register allocator clobbering** (`regalloc.rs`): Constants not loaded into
   registers before use can be clobbered by intervening instructions. The L2
   optimizer's `effect_reorder` pass was fixed to respect memory dependencies
   and keep terminators last, so `--opt` (now enabled by default) no longer
   exposes this bug on the current test suite. However, the underlying regalloc
   weakness may still surface with new code. Test with `--no-opt` when changing
   regalloc to isolate issues.

2. **Debug `eprintln!` statements** in `compiler.rs` and `elf64.rs`: There are
   temporary debug prints (DBG ASM, DBG ALU) added during debugging sessions.
   These should be removed before committing but are safe to leave during
   active debugging.

3. **PIR L2 GetFieldPtr offset** (`pir.rs` line ~1240): `offset: *field_index * 8`
   uses `field_index * 8` as the byte offset. This is **correct by design** —
   `compute_struct_layout()` in `lower.rs` deliberately assigns every field an
   8-byte slot (uniform representation), so `field_index * 8` always matches the
   real offset in `StructLayout`. This is not a bug; it's a design choice. If
   struct layouts are ever changed to use packed/sized fields, this code must be
   updated to look up real offsets from `struct_layouts` (as `codegen_c.rs` does).

4. **Native codegen bugs** (not optimizer-related): All previously failing
   native ELF64 tests (`test_collections`, `test_db`, `test_match_range`,
   `test_matrix_mul`) are now FIXED. The root cause was a single wrong REX
   prefix byte in `p_json_at` (`0x49` instead of `0x4C` — see changelog for
   details). The `--opt` flag produces identical output to non-opt for all
   tests except 3 closure tests that are broken in both paths.

## Coding Conventions

1. **Codegen output**: use the `wln!` macro for formatted output in
   `codegen_c.rs` (defined at top of that file).

2. **Error handling**: return `Result<_, Vec<String>>` for compiler errors
   (multiple errors collected). Don't use `panic!` for user-facing errors.

3. **Test naming**: `test_<feature>.fd` for feature tests, `test_pe_<feature>.fd`
   for PE-emitter-specific tests. Keep the skip list in `tests/harness.rs`
   updated when adding tests for unimplemented features.

4. **Adding a new test**: create `tests/test_<name>.fd`, run `./verify.sh --save`
   to update the baseline. If the test exercises a not-yet-implemented feature,
   add it to `is_skip_test()` in `tests/harness.rs`.

5. **Adding a new backend stage**: add a golden file capture in
   `scripts/gen_golden.sh` and a check in `scripts/bisect.sh`.

## Debugging Workflow (When Tests Break)

1. **Don't panic.** Run `./verify.sh` to see exactly which tests regressed.

2. **Bisect the layer.** For a failing test, run:
   ```bash
   ./scripts/bisect.sh <test_name>
   ```
   This tells you whether the regression is in lowering, PIR, codegen, or
   peephole.

3. **Check if it's a real regression or a "was passing for the wrong reason".**
   Compare the failing test's golden output to the new output. If the new
   output is *correct* and the golden was *wrong*, the test was passing by
   accident — update the golden file.

4. **Make the smallest possible fix.** Don't refactor while fixing. One change,
   one commit, one `./verify.sh` run.

5. **If you must make a large change**, gate it behind a flag:
   ```rust
   const NEW_BEHAVIOR: bool = true;  // flip to false to test old path
   ```
   Then test both ways before removing the flag.

## What To Do Before Every Session

```bash
# 1. Confirm baseline
./verify.sh

# 2. If baseline doesn't match, someone changed something — investigate before
#    building on top of a broken state.

# 3. If you're starting fresh and there's no baseline:
./verify.sh --save
```

## What To Do After Every Change

```bash
# 1. Run verification
./verify.sh

# 2. If regressions: bisect to find the layer
./scripts/bisect.sh <failing_test>

# 3. If no regressions and tests improved: update baseline
./verify.sh --save

# 4. If you changed codegen/regalloc/peephole/elf64/pe: also regenerate golden
#    files so future bisects are accurate
./scripts/gen_golden.sh
```

## File Map (Quick Reference)

| File | Role |
|------|------|
| `src/lexer.rs` | Tokenizer |
| `src/parser.rs` | Recursive descent parser |
| `src/typechecker.rs` | Types + regions + effects + capabilities |
| `src/borrowck.rs` | Borrow checker (alias analysis, `--borrowck=off\|warn\|on`) |
| `src/ir.rs` | Single-level SSA IR |
| `src/lower.rs` | AST → SSA IR |
| `src/pir.rs` | Multi-level PIR (L1/L2/L3) |
| `src/opt.rs` | PIR optimizer (opt-in — has regalloc bug) |
| `src/peephole.rs` | 9-pass peephole (always on in `build`) |
| `src/regalloc.rs` | Linear scan register allocator (⚠️ known bug) |
| `src/mono.rs` | Generic monomorphization |
| `src/comptime.rs` | Compile-time eval + reflection |
| `src/pe.rs` | PE emitter (Windows) |
| `src/elf64.rs` | ELF64 emitter (Linux) |
| `src/macho.rs` | Mach-O emitter (macOS) |
| `src/coff.rs` | COFF emitter (Windows) |
| `src/linker.rs` | Integrated linker |
| `src/runtime_linux.rs` | Hand-encoded Linux runtime (raw syscalls) |
| `src/codegen_x64.rs` | PIR L3 → x86-64 assembly |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly |
| `src/codegen_c.rs` | IR → C99 source |
| `src/wasm.rs` | PIR → WebAssembly |
| `src/gpu.rs` | PIR → SPIR-V |
| `src/embedded.rs` | FixedBufferAllocator, RealTime effect, HAL |
| `src/hotreload.rs` | Native JIT + function-level change detection |
| `src/cparser.rs` | C header parser → Fides FFI bindings |
| `src/compiler.rs` | Pipeline orchestration |
| `src/lsp.rs` | LSP server (JSON-RPC over stdio) |
| `src/main.rs` | CLI entry point |
