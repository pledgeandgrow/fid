# AGENTS.md — Fides Compiler Working Guide

> This file gives AI assistants (and humans) the context needed to work on the
> Fides compiler safely. Read this before making changes.
>
> **Fides is self-hosted and independent.** The primary compiler is in
> `compiler/*.fd` (61 modules, ~134K lines). The Rust source in `src/*.rs`
> (57 files, ~113K lines) is the bootstrap reference, frozen: it builds
> stage 1 when `compiler/*.fd` changes and runs the test harness. The
> language is what `fid` accepts: docs/reference/reference.md.
>
> **The self-hosted compiler is the release compiler:** `RELEASE=1
> ./scripts/bootstrap.sh` builds it in 3 stages (fixpoint checked) into
> `release/fid.exe`, which the installer ships as `fid.exe`. It checks
> more than the Rust compiler (generics, borrows, safety annotations,
> borrowed strings): see docs/reference/language-guide.md § 10.

## Build & Test Commands

### Self-hosted compiler

```bash
# Release build: 3 stages, fixpoint checked, installed as
# release/fid.exe (the installer ships it as fid.exe)
RELEASE=1 CC="zig cc" ./scripts/bootstrap.sh
.\release\fid.exe run      tests/bootstrap/hello.fd
.\release\fid.exe ccompile tests/bootstrap/hello.fd -o hello.exe  # needs a C compiler

# Single stage (development; same output name, not fixpoint-checked):
# release/fid-dev.c (generated C) + release/fid-dev(.exe)
./scripts/build-selfhost.sh          # fidesc dump-c + C compile → release/fid-dev.exe

.\release\fid.exe dump-c compiler/main.fd > c2.c   # self-compile (fixpoint)
.\release\fid.exe run tests/bootstrap/hello.fd    # interpreter (as fidesc run)
```

### Rust bootstrap compiler (builds stage 1; runs the test harness)

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

# Self-host verification (tests/selfhost/*.fd on 3 backends)
# Separate from verify.sh: verify.sh → tests/bootstrap/ (Rust compiler in src/)
# verify-selfhost.sh → tests/selfhost/ (self-hosted Fides components in compiler/)
./verify-selfhost.sh
./verify-selfhost.sh --save
./verify-selfhost.sh --list

# Build the self-hosted compiler into release/ (manual two-step)
#   1. fidesc ccompile compiler\main.fd -o ... generates C via the Rust bootstrap
#   2. link the generated C with a C compiler (zig cc / cl / tcc)
#   See docs/selfhost/selfhost.md § Build Pipeline for details.
# (build-selfhost.ps1 no longer exists — do not reference it.)

# NOTE: scripts/gen_golden.sh and scripts/bisect.sh do NOT exist —
# verify.sh --golden currently fails; bisect via dump-* commands instead.

# Run a single test through the interpreter
./target/release/fidesc run tests/bootstrap/<name>.fd

# Show IR for a test
./target/release/fidesc ir tests/bootstrap/<name>.fd

# Show PIR (L1/L2/L3) for a test
./target/release/fidesc pir tests/bootstrap/<name>.fd 3

# Dump intermediate representations (dump-* commands)
./target/release/fidesc dump-tokens tests/bootstrap/<name>.fd
./target/release/fidesc dump-ast tests/bootstrap/<name>.fd
./target/release/fidesc dump-ir tests/bootstrap/<name>.fd
./target/release/fidesc dump-pir tests/bootstrap/<name>.fd 3
./target/release/fidesc dump-c tests/bootstrap/<name>.fd

# Differential testing (compare against golden files)
./target/release/fidesc diff-test tests/bootstrap/<name>.fd --stage ir

# Run with per-stage timing
./target/release/fidesc run tests/bootstrap/<name>.fd --trace

# Run with recursion depth limit (0 = unlimited)
./target/release/fidesc run tests/bootstrap/<name>.fd --max-recursion 1000

# Show assembly (x86-64)
./target/release/fidesc codegen tests/bootstrap/<name>.fd --target x86-64

# Show assembly with peephole (what `build` actually uses)
./target/release/fidesc codegen tests/bootstrap/<name>.fd --target x86-64 --peephole

# Run the LSP server (for editor integration)
./target/release/fidesc lsp

# Run lint checks (unused functions, unused imports; fid also unwrap() outside tests)
./target/release/fidesc lint <file.fd>

# Self-hosted only: unsafe code a program reaches, leaks, use after free
.\release\fid.exe audit <file.fd> [--stdlib] [--strict]
.\release\fid.exe run <file.fd> --leaks
.\release\fid.exe run <file.fd> --checked-alloc

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
./target/release/ark init [name]
./target/release/ark add <dep> --path <path> | --git <url>
./target/release/ark install
./target/release/ark list

# Documentation generator (HTML or Markdown from .fd source)
./target/release/scribe <file.fd> [--format html|markdown] [--output <dir>]
./target/release/scribe <dir>   [--format html|markdown] [--output <dir>]

# Code coverage (function, line, and instruction coverage)
./target/release/veil <file.fd> [--format text|html] [--output <dir>]

# Build the Windows installer (requires NSIS)
# On Windows: cd installer && makensis fides.nsi
# On CI: triggered automatically by GitHub Actions on release tags
# Manual trigger: gh workflow run build-installer.yml -f version=1.0.0
```

## Current Baseline (Nov 6 2026; full suite last run Nov 2)

> Project-wide numbers live in `CURRENT.md` → "Numbers"; keep the two in step.

```
SELF-HOSTED COMPILER (primary source):
  compiler/*.fd:  61 modules, 133,688 lines — THE COMPILER
  Fixpoint:       VERIFIED byte-for-byte (stage2 == stage3, 49 MB of C,
                  Nov 6; checked by every RELEASE=1 bootstrap)
  Bootstrap:      5 generations verified (Rust → c1 → c2 → c3 → c4 → c5)
  Binary:         release/fid.exe (RELEASE=1 scripts/bootstrap.sh):
                  fixpoint. Shipped as fid.exe. The Nov 4–6 compiler was
                  checked on the new and key tests on all 3 backends; the
                  full suite has not been rerun with it.
  Self-compile:   `dump-c compiler/main.fd` ~133 s (Nov 6: type check 59 s),
                  fidesc 21 s. The compiler is built at -O0 (bootstrap.sh).

RUST BOOTSTRAP (reference + test harness):
  src/*.rs:       57 files, 112,511 lines — bootstrap reference (frozen)
  Used when:      compiler/*.fd changes (builds stage 1); runs `fidesc test`
  Suite (Nov 2):  interp 339/339, C 343/343, PE 345/345, expected-fail 89/89
                  (saved baseline: an earlier run, 334/338/341)

WINDOWS INSTALLER (v1.0.0):
  installer/:     NSIS script + build helper
  CI:            .github/workflows/build-installer.yml is not in this
                 checkout (no .github/ directory); the user runs CI
  Output:        Fides-Setup-1.0.0.exe (2.5 MB, tested on Windows)
  Ships:         fid.exe = self-hosted (release/fid.exe), fidesc.exe =
                 Rust compiler, stdlib/ and examples/ beside them

TESTS:
  tests/bootstrap/: 452 .fd at top level (91 expected-fail), 493 with
                    capability/, effects/, regions/, safety/, diagnostics/
  tests/selfhost/:  51 .fd files (verify-selfhost.sh; baseline saved 36/47×3)
  Baseline:         tests/bootstrap/baseline/test_results.txt

STDLIB:
  stdlib/*.fd:    137 modules, 43,576 lines (+ 4 in stdlib/pe_runtime/;
                  kernel/uefi moved to the fides-os project)

RECENT IMPROVEMENTS:
  Nov 4–6:        fid: generics (0002), if/match values, @cfg, freeing of
                  provably finished locals (C5–C8), K4/K5 borrow checks, K7
                  effects, @send on spawn args (H4), parallel branches (H7),
                  async f64/struct (H9), native per-thread free lists (H8),
                  safety annotations enforced + `fid qualify` + safety-rules.md
                  (L1–L4), owned/borrowed strings (M11), integer-key HashMap,
                  per-call-site higher-order effects, iropt on the native path,
                  core checked once (D9). Copy-on-write written, LOWER_COW off.
  Session 38:     C path: no per-call call-stack push/pop; panic traces from
                  the machine stack (__p_frames / __p_fn_name over the
                  __p_fns table), stack overflow from the OS guard page
                  (SetThreadStackGuarantee, sigaltstack); fib through C 3.0 s
                  -> 0.81 s. Counted-loop check removal shared with C
                  (compiler/checks.fd, pe::unchecked_ops). Vec_push inlines.
                  Native: shadow space once, flags-only compares, fall-through
                  blocks, params in registers, short immediates. One Vec
                  implementation: every backend's p_vec_* runs core's Vec_*.
                  Crypto vectors (NIST, RFC 4231), p_os_random_u64.
  Session 37:     Self-compile 4.4x faster (601 s -> 136 s, fixpoint kept),
                  found with a stack sampler on the stage-3 C. C runtime prints
                  with one fwrite per print (printf on unbuffered stdout wrote a
                  byte at a time on Windows: every printing program was slow);
                  IrFunction.inst_at (id -> instruction, was a linear scan for
                  any function with parameters); IrModule.fn_index/str_index,
                  Lowerer.func_info_at and the comptime pass's name set replace
                  scans by name. Linux CI green: suite, bootstrap, clean install.
                  Tasks wake by deadline; StringBuilder usable after to_str on C.
  Session 30:     Post-codegen constant-time verifier on the x86-64 ELF path:
                  after peephole, `verify_constant_time_asm` scans final asm for
                  conditional/indirect jumps, memory cmov, indexed memory ops,
                  variable-latency ops, and calls to non-CT functions; violations
                  become build errors (test_ct_asm.fd / test_ct_asm_fail.fd).
                  cast_arg in both C codegens now dereferences Ptr(inner)→Generic
                  when inner == the generic type (heals &mut Vec → Vec forwards).
                  cl.exe invocation fixed (cmd /C quote-stripping).
  Session 29:     Reference escape check (src/escape.rs, compiler/escape.fd):
                  references are second-class — cannot be stored, returned from
                  locals, sent to tasks, or captured by escaping closures.
                  Vec<f64> element reads + f64 return conversion fixed on all
                  backends. Fixpoint re-verified (stage2==stage3).
  Session 26-28:  Branch consolidation (release-infra → main), fixpoint
                  re-verified (c4==c5 byte-for-byte), file-analysis/ audit of
                  all compiler/*.fd modules, B/C/D remediation batches.
  Session 23:     6 high-priority language features implemented:
                  @zeroize (F88) — compiler-inserted memset(0) at scope exit
                  @secret (F74) — info-flow control: secrets can't reach output sinks
                  @interrupt (F29) — @interrupt(N) annotation + automatic IDT setup
                  @firmware (F30) — rejects OS-dependent calls in bare-metal code
                  parallel { | } (F13) — fork-join syntax (contextual keyword)
                  @simd (F23) — aggressive loop vectorization (wider vectors, relaxed cost)
                  Test baseline improved: +2 interp, +1 C codegen, 0 regressions
  Session 21:     26 game/media stdlib modules added (sdl, gl, image, font, mesh,
                  gameloop, physics3d, skel, ui, net, camera, particles, animation2d,
                  audio_mixer, asset, serialize, collision_broad, raycast, navmesh,
                  terrain, profiler, replay, shader, editor, chunk, touch)
                  — 9,140 LOC added, 99 stdlib modules total
                  4 runtime bugs documented (global Vec crash, Vec::set overflow,
                  struct return reassignment overflow, Vec<f64> bit patterns)
  Session 20:     12 Rust modules ported (coff, macho, cparser, embedded, tensor,
                  wasm, scribe, veil, hotreload, debugger, native_debugger, gpu)
                  + 7 cross-comp backends (codegen_arm64/riscv, runtime_arm64/riscv,
                  arm64_enc, riscv_enc) — 14,577 LOC added, 50 modules total
  PE stubs:       p_map_keys/values/get_or/clone + p_string_trim/split implemented
  ELF dump:       dump_symbols implemented (symbol table parser)
  @packed:        Packed struct layout (field_align=1, no padding)
  Opaque fields:  GetFieldPtr resolves IrType::Opaque when struct exists
  ELF64 encoder:  Full x86-64 instruction encoder ported (compiler/elf64.fd, 2,423 LOC)
  Interp port:    75 missing interpreter functions ported (compiler/interp.fd, 2,839 LOC)
  Linux runtime:  Native Linux syscall runtime (compiler/runtime_linux.fd, 4,362 LOC)
  Linker:         ELF relocation/linker ported (compiler/linker.fd, 1,051 LOC)
  Sandbox:        Alloc capability enforced (p_malloc/p_free blocked in sandbox)
  Rc/Arc:         stdlib/rc.fd — reference counting (rc_new, rc_clone, rc_drop, etc.)
  @repr(C):       C-compatible struct layout for hardware/FFI/kernel
  Kernel:         stdlib/kernel.fd — MOVED to the fides-os project (Oct 24)
  @cpu_local:     Per-CPU variables via GS segment (compiler/pe.fd)
  Inline asm:     Clobber lists, 50+ x86 instructions encoded (compiler/pe.fd)
  --no-std:       Bare-metal mode skips stdlib/core.fd (compiler/compiler.fd)
  UEFI:           stdlib/uefi.fd — MOVED to the fides-os project (Oct 24)
  Cross-comp:     --target arm64/riscv64 validation with C codegen fallback

Binaries:     4 (fidesc, ark, scribe, veil) + the self-hosted fid
```

**Build directories:**
- `target/` — Rust bootstrap compiler output (`fidesc.exe`) — gitignored
- Never commit a build directory. `target3/` (an agent's build folder) went
  into commits f9c55d8/087609a by mistake; it is now in `.gitignore`. Agents
  build in their scratch folder (`CARGO_TARGET_DIR`), not in the repository.
- `release/` — Self-hosted compiler output: `fid.exe` (from the release
  bootstrap, or a single stage with `fid.c` from build-selfhost.sh) — gitignored
- `compiler/` — Self-hosted compiler source (54 `.fd` modules) — THE PRIMARY COMPILER

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
| `src/regalloc.rs` | **CRITICAL** | Linear scan allocator (~1100 LOC). F-1A: constant rematerialization (`Location::Remat`). F-1D: effect-aware register allocation — calls to pure functions (in `L3Module.pure_functions`) only clobber RAX, not all caller-saved registers. I-2D: calls to `@no-alloc` functions (in `L3Module.no_alloc_functions`) also only clobber RAX. `@medical` functions included in no-alloc set. S-17: deterministic interval sorting (secondary key: vreg ID) to prevent non-deterministic codegen from HashMap iteration order. Any change here affects EVERY function. |
| `src/peephole.rs` | **CRITICAL** | 9 passes, always on in `build`. A wrong transform silently corrupts all output. |
| `src/elf64.rs` | **HIGH** | ELF64 header bytes, section layout, relocations (R_X86_64_PC32/PLT32/64). Off-by-one = corrupt binary. Self-hosted port in `compiler/elf64.fd` (2,423 LOC, 85% parity) includes full x86-64 instruction encoder, assembly parser, and ELF builder. |
| `src/pe.rs` | **HIGH** | PE header bytes, import table (kernel32.dll + msvcrt.dll: every name in `IMPORTS`/`MSVCRT_IMPORTS` must be a real export, or no program starts), section layout. Off-by-one = corrupt .exe. Machine-code stubs must not keep a value in a volatile register (rcx, rdx, r8–r11) across a call (HeapFree clobbers them). |
| `stdlib/pe_runtime/*.fd` | **MEDIUM** | The Fides runtime of native Windows executables: builtin `p_X` runs `pert_X` (base, conc, tensor, data). Merged into every `emit`: a type error here breaks every PE build. C functions via `extern "C"` thunks; helpers prefixed `pe_`/`pet_`/`ped_` (unused ones are left out of the .exe). |
| `src/coff.rs` | **HIGH** | COFF object file format. Same byte-level risk as PE/ELF. |
| `src/macho.rs` | **HIGH** | Mach-O object file format. Same byte-level risk. |
| `src/codegen_x64.rs` | **HIGH** | PIR L3 → x86-64 instruction encoding. ModRM/SIB bytes, REX prefixes, ABI rules. |
| `src/codegen_c.rs` | **HIGH** | IR → C99. ABI casts, struct offsets, function renaming. 343/343 test parity (Nov 2). Phase 10B: Print instruction calls Fides Vec_to_str/HashMap_to_str for formatting. Channels: p_chan_* with pthread mutex+condvar. Binary hardening: stack canaries, PIE/ASLR, RELRO, NX stack. Stack traces on panic. Rc/Arc runtime via p_rc_incr/p_rc_decr. ML3: Autograd tape (ag_tape, ag_record, p_autograd_backward) with reverse-mode AD for add/sub/mul/matmul/scale/relu/sigmoid/sum/mean. ML11: Checkpoint mode stores input data for relu/sigmoid backward. ML12: DP noise/budget builtins. ML13: TP builtins (identity in single-rank). ML9: KV cache with sliding window eviction. ML10: Quantization (quantize/dequantize/qmatmul/find_quant_params). Tensor printing via p_tensor_to_str with %.4f for non-integers. Dlopen simulated to match interpreter (sequential handles). |
| `src/pir.rs` | **HIGH** | PIR L1/L2/L3 transforms. A bad lowering propagates to all backends. |
| `src/lower.rs` | **MEDIUM** | AST → SSA IR. Changes here affect interpreter + all backends. `runtime_effects()` helper sets Write/Alloc/IO effects on `p_*` builtin calls — without this, the optimizer DCE's side-effecting calls. `p_vec_*` extern declarations (23 functions) ensure correct calling convention. |
| `src/typechecker.rs` | **MEDIUM** | Types + regions + effects + capabilities. Wrong rejection = broken code; wrong acceptance = silent miscompilation. |
| `src/borrowck.rs` | **LOW** | Borrow checker (Phases 1-6 complete). Separate pass after typecheck. `--borrowck=on` by default; `off`/`warn` modes available. Field-wise borrows, two-phase borrows, NLL (non-lexical lifetimes via last-use analysis). |
| `src/opt.rs` | **MEDIUM** | PIR optimizer. 23 passes: const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize. 22 in default pass list (region-pool via `--region-pool`). `--opt` is enabled by default in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining (larger inline limits). PGO has been removed. S-1A pass skipping: `pass_relevant_for_module()` skips passes that can't help (no loops → skip unroll, no calls → skip inline, etc.). S-17: all instruction traversal functions now handle `L2Inst::Copy` (was missing, causing SROA+CSE to produce dangling references). Inline cost model (F-3F): cost = blocks×5 + insts, with `is_pure` (50% bonus), `is_crashfree` (25%), `is_deterministic` (10%), capped at 75%. Sandbox-aware inlining (I-3C): sandbox call sites use 6 blocks / 40 insts vs normal 3/20. Sandbox-aware devirt (I-3C): devirtualize DynCalls in sandbox blocks to pure impls. Region bulk free (I-1C): `pass_region_pool` emits `p_region_free` before returns. @crashfree code hoisting (I-4B): `pass_effect_optimize` hoists @crashfree + effectively-pure calls out of loops. @gas-guided unrolling (I-4C): `pass_unroll` uses max_unroll=64 when `gas_limit.is_some()` (vs normal 8). Annotations propagated: `is_wrapping`, `is_deterministic`, `is_crashfree`, `is_no_alloc`, `is_medical`, `gas_limit`. I-6B linear type alias analysis: `linear_structs` propagated from AST → IrModule → L1Module → L2Module; `alias_analysis_function` tracks `linear_ptrs` and uses them to prove non-aliasing (stores through linear pointers can't invalidate loads from other pointers). |
| `src/comptime.rs` | **LOW** | Comptime evaluation engine. Evaluates `comptime <expr>`, `comptime { block }`, `comptime reflect_op(T)`, `comptime gen { block }` at compile time. I-5A: Aggressive comptime evaluation — auto-evaluates calls to `comptime fn` functions with all-literal args (no explicit `comptime` wrapper needed). I-5D: Comptime loop unrolling — unrolls `for` loops with literal bounds (≤16 iterations, ≤16 body stmts) at AST level. Also eliminates dead `if` branches with literal conditions. |
| `src/runtime_linux.rs` | **HIGH** | Hand-encoded x86-64 machine code (raw bytes). No assembler — every byte is manual. |
| `src/linker.rs` | **MEDIUM** | Relocation resolution. Wrong offset = crash or silent corruption. |
| `src/interp.rs` | **MEDIUM** | The reference oracle (~6060 LOC). If this is wrong, all backends "match" but produce wrong results. `p_vec_new` uses `vec_side_table` to separate raw struct fields from `Value::Vec`. `run_function` auto-detects missing `__env` param and inserts dummy. Self-hosted port in `compiler/interp.fd` (~7,200 lines) runs every test program `fidesc run` does. |

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
| `stdlib/*.fd` | LOW | Fides source — errors surface as test failures. 137 stdlib modules, among them: alloc, animation2d, arena, args, arinc, asset, audio, audio_mixer, banking, bitfield, bits, buffer, camera, can, channel, chunk, clinical, collections, collision_broad, core, crypto, data, db, dp, ecs, ecu, editor, env, events, file, flowfield, fmt, font, fs, game, gameloop, gl, grid, hash, http, image, input, io, iter, json, kv_cache, math, medical, memo, mesh, money, navmesh, net, os, ota, parallel, particles, path, physics, physics3d, plot, privacy, process, profiler, quant, queue, random, raycast, rc, regex, replay, result, scada, scene, sdl, serialize, shader, shape, shard, simd, skel, sort, sprite, sqlite, stack, storage, streaming, string, swift, sync, sys, template, tensor, terrain, test, tilemap, time, touch, typed_strings, ui, units, uuid, vec, volatile. (kernel.fd + uefi.fd moved to the fides-os project.) |

## Known Bugs (Do Not Re-Introduce)

> **Critical Blockers:** See `docs/roadmap/roadmap.md` § Issues & Debates → "Critical
> Blockers" section for the highest-priority issues that must be fixed
> before production use:
> 1. CB1: Safety annotations — ✅ in `fid` (Nov 4): every rule enforced, each with a negative test (docs/reference/safety-rules.md)
> 2. CB2: Generics — ✅ in `fid` (Nov 3–4, proposal 0002): inference or an error, bounds checked, turbofish; `fidesc` is frozen without it
> 3. CB3: Backend test parity — ✅ resolved (Nov 2: 339/339 interp, 343/343 C, 345/345 PE)
> 4. CB4: Parser debug prints — ✅ resolved (Session 19)
> 5. CB5: Self-hosted compiler build fails — ✅ resolved (Session 20, all 50 modules type-check)
> 6. CB6: PE emitter stubs — partially resolved (Session 20, 6/15 fixed, 9 p_string_* remain)
> 7. CB7: ELF64 symbol table dump — ✅ resolved (Session 20)

1. **Self-hosted compiler: what still differs from `fidesc`** (Oct 27).
   `run` matches on all 302 test programs and all 10 examples; `ccompile`
   on 242 of 243 test programs and 9 of 10 examples; all 59 expected-fail
   tests are rejected. The rest:
   - `test_sqlite` needs libsqlite3 and `examples/minimal_game` needs SDL2
     (the Rust compiler fails the same way without them).
   - `build --native` (compiler/pe.fd via PIR) still crashes (optimizer
     `pass_dse`, register allocation); `fid emit` (compiler/pe_ir*.fd) is the
     working native path.
   Fixed Oct 25–27 (docs/reference/known-bugs.md §2e): block jumps ended
   functions, byte loads read side-table strings, builtins shadowed core
   functions, the comptime loop unroller, String/HashMap result types,
   stdlib lookup from an installed location, `ccompile` without `CC`, a
   second `impl` block in an imported module was dropped (`merge_programs`
   aliasing), @conserve checked only effects, `extern "C" link` libraries
   not linked.

2. **Nondeterministic self-hosted typecheck/borrowck**: regenerating
   `release/fid-dev.c` from identical `compiler/*.fd` input can fail with
   *different* errors per run (e.g. `Vec<i64>` vs `Vec<Vec<EscVar>>`
   assignment, or a reference-escape error) or succeed. HashMap iteration
   order in the self-hosted typechecker/inference is the suspected source.
   Retry the build before assuming a regression. (Not reproduced on
   Oct 25: five 3-stage bootstraps all type-checked and reached the
   fixpoint.)

3. **`cmd /C` quote stripping** (`src/main.rs`): the `vcvarsall.bat && cl.exe`
   link path failed on Windows because `cmd /C` strips the first quote when
   the command string begins with `"` and contains `&&`/`>` — `cl.exe` then
   ran as `'C:\Program'` and produced empty stderr. **Fixed:** `cmd_str` is
   wrapped in an outer quote pair.

4. **`Vec` argument ABI at `cast_arg`** (`src/codegen_c.rs`,
   `compiler/codegen_c.fd`): when a `Ptr(inner)` value (e.g. an
   `&mut Vec<T>` parameter slot) is passed to a `Generic`/`Opaque` parameter
   of exactly that inner type, the caller must emit `*(void**)` to load the
   handle. Without the deref, `Vec_len`/`Vec_get` read the slot pointer as a
   header → garbage lengths and crashes (seen in `record_def` →
   `ra_vec_find_i64`). The deref fires **only** when the pointee type equals
   the destination type — a broader version dereferenced `str` (which is
   `*u8`) passed to generic params and corrupted `Parser_new`.

5. **Post-codegen constant-time verification** (`src/codegen_x64.rs`):
   `verify_constant_time_asm` runs on the x86-64 ELF path after peephole. It
   rejects `jcc`/indirect jumps, `cmov` with memory operands, indexed
   memory, variable-latency ops, and calls to non-`@constant_time` functions
   in annotated code. `@wrapping` exempts generated overflow branches.
   The recursion-guard and stack-canary sequences are exempt. **x86-64 ELF
   only** — the PE path emits machine code directly and is not yet covered;
   ARM64/RISC-V are not covered.

6. **Session-16 regression — bare runtime stubs**: the `p_hlt`/`p_cli`/
   `p_sti`/`p_inb`/`p_outb`/`p_serial_*` stubs lost in the
   `git checkout -- compiler/pe.fd` incident (known-bugs.md §1.37) were
   re-added; verify `emit --bare` still emits them before touching pe.fd.

7. **Register allocator clobbering** (`regalloc.rs`): Previously, constants not
   loaded into registers before use could be clobbered by intervening
   instructions. **Fixed by F-1A (Constant Rematerialization):** constants are
   now rematerialized at each use site via `Location::Remat(i64)` instead of
   being spilled and reloaded. The L2 optimizer's `effect_reorder` pass was also
   fixed to respect memory dependencies and keep terminators last. Test with
   `--no-opt` when changing regalloc to isolate issues.

8. **Debug `eprintln!` statements**: Temporary debug prints (DBG ASM, DBG ALU,
   DEBUG gen_store, DEBUG struct_layouts) were added during debugging sessions
   and have been removed. Do not re-add them.

9. **PIR L2 GetFieldPtr offset** (`pir.rs`): Previously used `field_index * 8`
   as the byte offset, which was correct when all struct fields were 8 bytes.
   **Fixed (SH-6.1):** Now looks up real offsets from `struct_layouts` using a
   `value_types` map built during L2→L3 lowering. This was necessary because
   enum-typed struct fields need `(1 + max_payload) * 8` bytes, not 8.
   `compute_struct_layout()` in `lower.rs` now uses enum sizes for enum-typed
   fields, and the PIR L2→L3 lowering looks up the real offset.

10. **Native codegen bugs** (not optimizer-related): All previously failing
   native ELF64 tests (`test_collections`, `test_db`, `test_match_range`,
   `test_matrix_mul`) are now FIXED. The root cause was a single wrong REX
   prefix byte in `p_json_at` (`0x49` instead of `0x4C` — see changelog for
   details). The `--opt` flag produces identical output to non-opt for all
   tests except 3 closure tests that are broken in both paths.

11. **Non-deterministic register allocation** (`src/regalloc.rs`): The linear
   scan allocator sorted live intervals by start point only. When multiple
   intervals shared the same start, HashMap iteration order (non-deterministic
   due to random seeding) determined their relative order, producing different
   register/stack-slot assignments on each run. **Fixed:** Added `i.vreg.0` as
   a secondary sort key: `sorted.sort_by_key(|i| (i.start, i.vreg.0))`. The
   self-hosted compiler (`compiler/regalloc.fd`) was already deterministic
   (uses Vec, not HashMap).

12. **SROA + CSE dangling reference** (`src/opt.rs`): SROA replaces struct
   field loads with `Copy` instructions. CSE then eliminates duplicate
   constants and calls `replace_all_refs` to update references — but
   `replace_refs_in_inst` did not handle `L2Inst::Copy`, leaving dangling
   references after DCE removed the original constant. This caused
   zero-valued struct fields to print garbage in optimized native builds.
   **Fixed:** Added `L2Inst::Copy { value, .. }` to all instruction traversal
   functions in `src/opt.rs` (`replace_refs_in_inst`, `apply_replacements`,
   `remap_inst`, `inst_references`, `is_pure`, `is_pure_inst`, `cse_key`,
   `gvn_key`). The self-hosted compiler (`compiler/opt.fd`) already handled
   `Copy` correctly — the bug was only in the Rust bootstrap.

13. **Stack canary / float scratch overlap** (`compiler/pir.fd`,
   `compiler/codegen_x64.fd`): PIR frame allocation started at offset 0,
   causing the first alloca slot to overlap with the canary at `-8(%rbp)`.
   Float conversion also used `-8(%rbp)`, overlapping the canary.
   **Fixed:** Frame size starts at 16 bytes (8 canary + 8 float scratch) in
   `compiler/pir.fd`. Float scratch moved to `-16(%rbp)` in
   `compiler/codegen_x64.fd`.

14. **Slow paths that look harmless** (`compiler/ir.fd`, both C runtimes):
   do not print through `printf` in the C runtime (stdout is unbuffered: one
   write per byte on Windows; use `fwrite`), and do not look up instructions,
   functions or strings by scanning a list: use `inst_at`, `fn_index`,
   `str_index`. A function's parameters take value ids without instructions,
   so `id == index` does not hold. Any code that replaces `module.functions`
   wholesale must `HashMap_clear(module.fn_index)`.

15. **Per-call bookkeeping in generated C** (both C generators): do not
   bring back `p_call_stack_push`/`pop` or a recursion counter. Traces come
   from the machine stack (`__p_print_trace_ctx`); overflow from the guard
   page. `zig cc` off Windows needs `-lunwind` for `_Unwind_Backtrace`
   (main.rs, main.fd, bootstrap.sh, build-selfhost.sh add it).

16. **One `Vec` implementation** (`pe_runtime_alias`, `pe_ir_stubs.fd`,
   `interp.rs core_vec_fn`): every `p_vec_*` builtin runs core's `Vec_*`
   function. Do not add a backend stub with its own semantics; the old ones
   disagreed (`clone` shared data, `first` returned garbage). The C runtime's
   `PVec` functions share core's layout and must keep doing so.
   `test_vec_builtins.fd` checks every backend agrees.

17. **The counted-loop analysis lives in `compiler/checks.fd`**, not in
   `pe_ir.fd`: `codegen_c.fd` importing `pe_ir` doubled the C of any program
   that uses the C generator (`test_codegen_c` 5 MB -> 11 MB).

18. **Testing a runtime `str` for null**: write `(s as i64) == 0`. `s == 0`
   on a `str` is a text comparison in `fid`, so it never detects the null
   that `p_file_read`, `p_getenv` and `p_argv` return on failure.

19. **fidesc bakes `stdlib/core.fd` in** (`include_str!` in
   `src/compiler.rs`): after editing core, rebuild `fidesc` before testing
   with it, or it runs the old core. `fid` reads core at run time.

20. **The debug allocator** (`P_LEAKS`, `P_CHECKED_ALLOC` in
   `compiler/codegen_c.fd`) renames the block allocator to
   `__p_block_malloc`/`__p_block_free` with `#define` and puts recording
   `p_malloc`/`p_free` after it. Keep code that must call the real block
   allocator between those defines, and nothing else.

21. **A plain string is not a headered one** (`stdlib/core.fd`
   `p_str_free`, `p_str_length`): a length-headered string has
   `P_STR_MAGIC` at `s - 16` and its length at `s - 8`. A plain `p_malloc`
   string can sit right after a block whose last word happens to be the
   magic (the compiler's own IR of core.fd holds it as a constant), so the
   magic alone is not enough: `p_str_len_word` also requires the word at
   `s - 8` to be a plausible length (a block header is negative). Without
   it, natively built `fid` freed `s - 16` ("Invalid free").

22. **The C runtime frees what it allocated with the same allocator**:
   strings from `p_malloc` (`p_to_str_float`, `p_file_read`, `p_dir_list`,
   `p_string_split`, `p_to_str_vec`, `p_to_str_map`) go back through
   `p_free`, never `free()` (`p_print_float` did: a crash after the second
   float printed).

23. **Freeing a by-value `Vec` parameter needs copy-on-write**
   (`compiler/lower.fd`, `lw_drop_function_vecs`): a function may free a
   `Vec` parameter at its return only when callers pass copy-on-write
   shares, i.e. when `LOWER_COW` is on. With it off the caller's own header
   arrives, and freeing it is a double free in the caller (seen as
   "Double free" in `cc_inherit_supertraits` when the compiler built itself).

24. **Field addresses come from a pointer typed as the struct**
   (`Lowerer_emit_struct_share`): a pointer typed as what holds the struct
   (an enum's payload) resolves field indexes against the holder's layout.
   Bit-cast to `Ptr(Struct)` first. Likewise word offsets of fields come
   from the layout's `offset`, not from adding up field sizes: an enum
   field is `(1 + payload)` words.

25. **Copy-on-write** (`LOWER_COW`, on since Nov 6): shares must stay lazy
   (`Vec_share_deep` records the element kind, `vec_own` copies at the first
   write) and counted, a struct store must not share fresh values or last
   uses (`Lowerer_fresh_struct`), and read-only copies are judged on the rest
   of their block (`Lowerer_readonly_copy_in`). The first eager version ran
   the self-compile out of memory. With it on the fixpoint holds (Nov 6,
   4.7 GB peak), and the suite gives the same output on and off. While
   `LOWER_ARG_SHARE` is off a by-value parameter is the caller's handle:
   its last use is not a move (`Lowerer_last_use`, test_spawn_vec_copy).

26. **Another agent may be editing `core.fd`/`lower.fd`**: build from a
   snapshot of `compiler/`, `stdlib/` and `scripts/` when files change
   during a build, or the stage-3 C differs only because a file changed
   between the stages.

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

4. **Adding a new test**: create `tests/bootstrap/test_<name>.fd`, run `./verify.sh --save`
   to update the baseline. If the test exercises a not-yet-implemented feature,
   add it to `is_skip_test()` in `tests/harness.rs`.

5. **Adding a new backend stage**: `scripts/gen_golden.sh` and
   `scripts/bisect.sh` do not exist yet — when they are written, add a
   golden-file capture and a bisect check there. Until then use the
   `dump-*`/`codegen`/`pir` commands to inspect stages manually.

## Debugging Workflow (When Tests Break)

1. **Don't panic.** Run `./verify.sh` to see exactly which tests regressed.

2. **Bisect the layer.** There is no `scripts/bisect.sh` — bisect manually:
   ```bash
   ./target/release/fidesc dump-ir  tests/bootstrap/<name>.fd   # AST→IR
   ./target/release/fidesc dump-pir tests/bootstrap/<name>.fd 3 # L1→L3
   ./target/release/fidesc codegen  tests/bootstrap/<name>.fd --target x86-64 --peephole
   ```
   This tells you whether the regression is in lowering, PIR, codegen, or
   peephole.

3. **Check if it's a real regression or a "was passing for the wrong reason".**
   Compare the failing test's golden output (`tests/bootstrap/golden/`) to the
   new output. If the new output is *correct* and the golden was *wrong*, the
   test was passing by accident — update the golden file.

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

# 2. If regressions: bisect the layer manually with dump-ir / dump-pir /
#    codegen --peephole (scripts/bisect.sh does not exist)

# 3. If no regressions and tests improved: update baseline
./verify.sh --save

# 4. If you changed codegen/regalloc/peephole/elf64/pe: regenerate golden
#    files (scripts/gen_golden.sh does not exist yet — capture dumps by hand
#    into tests/bootstrap/golden/)
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
| `stdlib/pe_runtime/*.fd` | Fides runtime of the PE backend |
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
