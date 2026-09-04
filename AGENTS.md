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

# Self-host verification (tests/compiler/*.fd on 3 backends)
# Separate from verify.sh: verify.sh → tests/ (Rust compiler in src/)
# verify-selfhost.sh → tests/compiler/ (self-hosted Fides components in compiler/)
./verify-selfhost.sh
./verify-selfhost.sh --save
./verify-selfhost.sh --list

# Build the self-hosted compiler into release/ (Windows PowerShell)
.\build-selfhost.ps1              # Build release/fides-c.exe
.\build-selfhost.ps1 -Bootstrap   # Build + test self-compilation (fixpoint check)
.\build-selfhost.ps1 -Test <file> # Build + test on a .fd file
.\build-selfhost.ps1 -Clean       # Clean release/ and rebuild

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

# Dump intermediate representations (dump-* commands)
./target/release/fidesc dump-tokens tests/<name>.fd
./target/release/fidesc dump-ast tests/<name>.fd
./target/release/fidesc dump-ir tests/<name>.fd
./target/release/fidesc dump-pir tests/<name>.fd 3
./target/release/fidesc dump-c tests/<name>.fd

# Differential testing (compare against golden files)
./target/release/fidesc diff-test tests/<name>.fd --stage ir

# Run with per-stage timing
./target/release/fidesc run tests/<name>.fd --trace

# Run with recursion depth limit (0 = unlimited)
./target/release/fidesc run tests/<name>.fd --max-recursion 1000

# Show assembly (x86-64)
./target/release/fidesc codegen tests/<name>.fd --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/fidesc codegen tests/<name>.fd --target x86-64 --peephole

# Run the LSP server (for editor integration)
./target/release/fidesc lsp

# Run lint checks (unused functions, unused imports)
./target/release/fidesc lint <file.fd>

# Benchmark a file (interpreter or C native)
./target/release/fidesc bench <file.fd> [-n <count>] [--ccompile]

# Profile a file (per-function timing)
./target/release/fidesc profile <file.fd>

# Format a Fides source file
./target/release/fidesc fmt <file.fd>

# Parse a C header and generate Fides FFI bindings
./target/release/fidesc parse-header <header.h>

# Debug a Fides program (interactive debugger)
./target/release/fidesc debug <file.fd>

# Replay a recorded execution trace
./target/release/fidesc replay <trace.fdtrace>

# Debug a native binary (ptrace-based, Linux only)
./target/release/fidesc autopsy <file.fd>

# Run with post-mortem trace saving (auto-saves trace on panic)
./target/release/fidesc run <file.fd> --post-mortem

# Package manager (dependencies, install, lock files)
./target/release/pacte init [name]
./target/release/pacte add <dep> --path <path> | --git <url>
./target/release/pacte install
./target/release/pacte list

# Documentation generator (HTML or Markdown from .fd source)
./target/release/scribe <file.fd> [--format html|markdown] [--output <dir>]
./target/release/scribe <dir>   [--format html|markdown] [--output <dir>]

# Code coverage (function, line, and instruction coverage)
./target/release/veil <file.fd> [--format text|html] [--output <dir>]
```

## Current Baseline (as of Sep 4, 2026 — session 7)

```
Unit tests:   263/265 passed  (2 failing — recent lower.fd changes)
Interpreter:  191/194 normal
C Codegen:    74/194 matched   (tcc integration + Windows path fixes)
PE Emitter:   193/194 valid
Total .fd:    230 test files  (207 in tests/; 23 in tests/compiler/)
Self-host:    23/23 pass on interp + PE (tests/compiler/)
Source:       52 .rs files, 82,778 LOC
Stdlib:       69 .fd modules, 16,069 LOC
Self-host:    19 .fd modules, 40,762 LOC (token, diag, json, fmt, ast, lexer, parser, ir, typechecker, lower, mono, borrowck, pir, opt, codegen_c, compiler, comptime_eval, flat_ast, lsp, pe, peephole, regalloc, codegen_x64, main — all compile + pass tests)
  flat_ast.fd: FULL PORT (dump-ast-flat serialization, 1260 lines) — session 9
  comptime_eval.fd: FULL PORT (process_gen, substitute, I-5A, I-5D, reflection) — session 7
  pe.fd: 25+ runtime stubs (print, string, vec, map, float, bool) — session 7
Bootstrap:    release/fides-c.exe builds (8.5MB) and self-compiles (EXIT 0); fides-c2 crashes on stdlib (pre-existing)
Safety:       SH-12 DONE — C codegen recursion limit, p_malloc OOM abort, --max-steps/--max-memory/--timeout, test runner step limit, Windows path fixes, tcc auto-detection, crash handlers (VEH+SIGSEGV), all ported to Rust + Fides codegen
Docs:         24 docs/ + 4 root = 28 .md files
Binaries:     4 (fidesc, pacte, scribe, veil)
```

**Build directories:**
- `target/` — Rust compiler output (`fidesc.exe`) — gitignored
- `release/` — Self-hosted compiler output (`fides-c.c`, `fides-c.exe`) — gitignored
- Build script: `.\build-selfhost.ps1` (Windows) or `./bootstrap-fast.sh` (Linux)

**Changelog:** See `docs/reference/changelog.md` for the full history of
features and fixes. (Previously duplicated here — removed to avoid maintaining
the same information in two places.)

**Before making any change:** run `./verify.sh` to confirm you're starting from a known-good state.

## HIGH-RISK FILES — Always run `./verify.sh` after editing these

These files are where silent errors are most dangerous. Edits here can break
everything with no warning. Always run the full verification suite after
touching them.

| File | Risk | Why |
|------|------|-----|
| `src/regalloc.rs` | **CRITICAL** | Linear scan allocator (~1100 LOC). F-1A: constant rematerialization (`Location::Remat`). F-1D: effect-aware register allocation — calls to pure functions (in `L3Module.pure_functions`) only clobber RAX, not all caller-saved registers. I-2D: calls to `@no-alloc` functions (in `L3Module.no_alloc_functions`) also only clobber RAX. `@medical` functions included in no-alloc set. Any change here affects EVERY function. |
| `src/peephole.rs` | **CRITICAL** | 9 passes, always on in `build`. A wrong transform silently corrupts all output. |
| `src/elf64.rs` | **HIGH** | ELF64 header bytes, section layout, relocations (R_X86_64_PC32/PLT32/64). Off-by-one = corrupt binary. |
| `src/pe.rs` | **HIGH** | PE header bytes, import table, section layout. Off-by-one = corrupt .exe. |
| `src/coff.rs` | **HIGH** | COFF object file format. Same byte-level risk as PE/ELF. |
| `src/macho.rs` | **HIGH** | Mach-O object file format. Same byte-level risk. |
| `src/codegen_x64.rs` | **HIGH** | PIR L3 → x86-64 instruction encoding. ModRM/SIB bytes, REX prefixes, ABI rules. |
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 164/165 test parity (1 fail: test_compiler_sized, user-added). Phase 10B: Print instruction calls Fides Vec_to_str/HashMap_to_str for formatting. Channels: p_chan_* with pthread mutex+condvar. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic. Rc/Arc runtime via p_rc_incr/p_rc_decr. ML3: Autograd tape (ag_tape, ag_record, p_autograd_backward) with reverse-mode AD for add/sub/mul/matmul/scale/relu/sigmoid/sum/mean. ML11: Checkpoint mode stores input data for relu/sigmoid backward. ML12: DP noise/budget builtins. ML13: TP builtins (identity in single-rank). ML9: KV cache with sliding window eviction. ML10: Quantization (quantize/dequantize/qmatmul/find_quant_params). Tensor printing via p_tensor_to_str with %.4f for non-integers. Dlopen simulated to match interpreter (sequential handles). |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. `runtime_effects()` helper sets Write/Alloc/IO effects on `p_*` builtin calls — without this, the optimizer DCE's side-effecting calls. `p_vec_*` extern declarations (23 functions) ensure correct calling convention. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/borrowck.rs` | **LOW** | Borrow checker (Phases 1-6 complete). Separate pass after typecheck. `--borrowck=on` by default; `off`/`warn` modes available. Field-wise borrows, two-phase borrows, NLL (non-lexical lifetimes via last-use analysis). |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. 22 in default pass list (region-pool via `--region-pool`). `--opt` is enabled by default in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining (larger inline limits). PGO has been removed. S-1A pass skipping: `pass_relevant_for_module()` skips passes that can't help (no loops → skip unroll, no calls → skip inline, etc.). Inline cost model (F-3F): cost = blocks×5 + insts, with `is_pure` (50% bonus), `is_crashfree` (25%), `is_deterministic` (10%), capped at 75%. Sandbox-aware inlining (I-3C): sandbox call sites use 6 blocks / 40 insts vs normal 3/20. Sandbox-aware devirt (I-3C): devirtualize DynCalls in sandbox blocks to pure impls. Region bulk free (I-1C): `pass_region_pool` emits `p_region_free` before returns. @crashfree code hoisting (I-4B): `pass_effect_optimize` hoists @crashfree + effectively-pure calls out of loops. @gas-guided unrolling (I-4C): `pass_unroll` uses max_unroll=64 when `gas_limit.is_some()` (vs normal 8). Annotations propagated: `is_wrapping`, `is_deterministic`, `is_crashfree`, `is_no_alloc`, `is_medical`, `gas_limit`. I-6B linear type alias analysis: `linear_structs` propagated from AST → IrModule → L1Module → L2Module; `alias_analysis_function` tracks `linear_ptrs` and uses them to prove non-aliasing (stores through linear pointers can't invalidate loads from other pointers). |
| `src/comptime.rs` | **LOW** | Comptime evaluation engine. Evaluates `comptime <expr>`, `comptime { block }`, `comptime reflect_op(T)`, `comptime gen { block }` at compile time. I-5A: Aggressive comptime evaluation — auto-evaluates calls to `comptime fn` functions with all-literal args (no explicit `comptime` wrapper needed). I-5D: Comptime loop unrolling — unrolls `for` loops with literal bounds (≤16 iterations, ≤16 body stmts) at AST level. Also eliminates dead `if` branches with literal conditions. |
| `src/runtime_linux.rs` | **HIGH** | Hand-encoded x86-64 machine code (raw bytes). No assembler — every byte is manual. |
| `src/linker.rs` | **MEDIUM** | Relocation resolution. Wrong offset = crash or silent corruption. |
| `src/interp.rs` | **MEDIUM** | The reference oracle (~4400 LOC). If this is wrong, all backends "match" but produce wrong results. `p_vec_new` uses `vec_side_table` to separate raw struct fields from `Value::Vec`. `run_function` auto-detects missing `__env` param and inserts dummy. |

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
| `src/compiler.rs` | LOW | Pipeline orchestration — changes are visible. Phase 12: `enrich_errors()` post-processes errors with source context + error codes. |
| `src/diag.rs` | LOW | Diagnostic system — structured error/warning/note/help with error codes, source context, color. Zero external deps. |
| `src/lsp.rs` | LOW | LSP server — JSON-RPC over stdio, no external deps. |
| `stdlib/*.fd` | LOW | Fides source — errors surface as test failures. 69 stdlib modules: core, vec, math, string, fmt, bits, sort, hash, iter, stack, queue, buffer, collections, result, random, path, units, uuid, crypto, storage, clinical, medical, ecu, scada, swift, grid, ota, banking, kv_cache, quant, dp, shard, fs, io, os, sys, ecs, events, parallel, sync, channel, db, http, json, shape, simd, tensor, money, alloc, file, time, typed_strings, memo, privacy, env, args, process, regex, tilemap, sprite, flowfield, streaming, scene, physics, audio, input, arinc, can. |

## Known Bugs (Do Not Re-Introduce)

1. **Register allocator clobbering** (`regalloc.rs`): Previously, constants not
   loaded into registers before use could be clobbered by intervening
   instructions. **Fixed by F-1A (Constant Rematerialization):** constants are
   now rematerialized at each use site via `Location::Remat(i64)` instead of
   being spilled and reloaded. The L2 optimizer's `effect_reorder` pass was also
   fixed to respect memory dependencies and keep terminators last. Test with
   `--no-opt` when changing regalloc to isolate issues.

2. **Debug `eprintln!` statements** in `compiler.rs` and `elf64.rs`: There are
   temporary debug prints (DBG ASM, DBG ALU) added during debugging sessions.
   These should be removed before committing but are safe to leave during
   active debugging.

3. **PIR L2 GetFieldPtr offset** (`pir.rs`): Previously used `field_index * 8`
   as the byte offset, which was correct when all struct fields were 8 bytes.
   **Fixed (SH-6.1):** Now looks up real offsets from `struct_layouts` using a
   `value_types` map built during L2→L3 lowering. This was necessary because
   enum-typed struct fields need `(1 + max_payload) * 8` bytes, not 8.
   `compute_struct_layout()` in `lower.rs` now uses enum sizes for enum-typed
   fields, and the PIR L2→L3 lowering looks up the real offset.

4. **Native codegen bugs** (not optimizer-related): All previously failing
   native ELF64 tests (`test_collections`, `test_db`, `test_match_range`,
   `test_matrix_mul`) are now FIXED. The root cause was a single wrong REX
   prefix byte in `p_json_at` (`0x49` instead of `0x4C` — see changelog for
   details). The `--opt` flag produces identical output to non-opt for all
   tests except 3 closure tests that are broken in both paths.

## NEVER DO THESE (Hard Rules)

1. **NEVER run `git stash`, `git checkout -- .`, `git reset --hard`, or any
   command that discards/hides uncommitted work.** This destroys the user's
   in-progress changes. To compare against a baseline, use `git diff` or a
   separate worktree (`git worktree add`). If you need to test "before" state,
   ask the user — do not touch the working tree.

2. **NEVER force-push, rewrite history, or delete branches** without explicit
   user confirmation for that specific action.

3. **NEVER run destructive shell commands** (`rm -rf`, `truncate`, bulk
   deletes) without describing exactly what will happen and waiting for
   approval.

These rules apply to ALL sessions and agents. No exceptions.

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

## CRITICAL RULES — NEVER DO THESE

1. **NEVER run `git stash`.** This can lose the user's work. Ever. No exceptions.
   If you need to compare against original code, use `git diff` or `git show`
   on specific files — never stash the working tree.

2. **NEVER run `git checkout` on files the user has modified.** This overwrites
   their changes. Use `git diff` to see what changed instead.

3. **NEVER run `git reset --hard` or `git clean`.** These are destructive and
   will wipe uncommitted work.

4. **NEVER force-push or rewrite git history.** No `git push --force`,
   no `git rebase` that rewrites shared commits.

5. **NEVER delete files you did not create.** If a file is in the way, ask the
   user — don't `rm` it.

6. **NEVER use `rm -rf` on project directories.** Ever.

If you need to see the original state of a file, use `git show HEAD:<path>`
or `git diff <path>` — these are read-only and safe.

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
| `src/opt.rs` | PIR optimizer (enabled by default in `build`, `--lto` for aggressive inlining) |
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
