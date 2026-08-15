# AGENTS.md — P Compiler Working Guide

> This file gives AI assistants (and humans) the context needed to work on the
> P compiler safely. Read this before making changes.

## Build & Test Commands

```bash
# Build (release — what verify.sh uses)
cargo build --release

# Build (debug — faster, used by cargo test)
cargo build

# Run all tests across all backends
./target/release/pledgec test

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
./target/release/pledgec run tests/<name>.p

# Show IR for a test
./target/release/pledgec ir tests/<name>.p

# Show PIR (L1/L2/L3) for a test
./target/release/pledgec pir tests/<name>.p 3

# Show assembly (x86-64)
./target/release/pledgec codegen tests/<name>.p --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/pledgec codegen tests/<name>.p --target x86-64 --peephole
```

## Current Baseline (as of Aug 2026)

```
Unit tests:   136/136 passed
Harness:      8/8 passed       (golden_files_match now passes)
Interpreter:  86/86 passed     (+ 3/3 expected fail: test_trait_verify_fail, test_budget_fail, test_sandbox_fail)
C Codegen:    86/86 matched    (FULL PARITY — no gaps)
PE Emitter:   86/86 valid      (+ 3/3 expected fail: test_trait_verify_fail, test_budget_fail, test_sandbox_fail)
Skip list:    20 tests         (interpreter-only features: comptime, bare metal, WASM, SIMD, tensor, HTTP, JSON, etc.)
```

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
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 79/81 test parity. |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. Not enabled by default (regalloc bug). Changes can expose the bug. |
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
| `stdlib/*.p` | LOW | P source — errors surface as test failures. |

## Known Bugs (Do Not Re-Introduce)

1. **Register allocator clobbering** (`regalloc.rs`): Constants not loaded into
   registers before use can be clobbered by intervening instructions. Exposed
   by L2 optimizer's instruction reordering. The optimizer is disabled by
   default because of this. If you fix it, test with `--opt` enabled.

2. **Debug `eprintln!` statements** in `compiler.rs` and `elf64.rs`: There are
   temporary debug prints (DBG ASM, DBG ALU) added during debugging sessions.
   These should be removed before committing but are safe to leave during
   active debugging.

3. **PIR L2 GetFieldPtr offset** (`pir.rs` line ~1199): `offset: *field_index * 8`
   assumes all fields are 8 bytes. This is a workaround — the proper fix is to
   use real struct layouts from `StructLayout` (like `codegen_c.rs` does).

## Coding Conventions

1. **Codegen output**: use the `wln!` macro for formatted output in
   `codegen_c.rs` (defined at top of that file).

2. **Error handling**: return `Result<_, Vec<String>>` for compiler errors
   (multiple errors collected). Don't use `panic!` for user-facing errors.

3. **Test naming**: `test_<feature>.p` for feature tests, `test_pe_<feature>.p`
   for PE-emitter-specific tests. Keep the skip list in `tests/harness.rs`
   updated when adding tests for unimplemented features.

4. **Adding a new test**: create `tests/test_<name>.p`, run `./verify.sh --save`
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
| `src/cparser.rs` | C header parser → P FFI bindings |
| `src/compiler.rs` | Pipeline orchestration |
| `src/main.rs` | CLI entry point |
