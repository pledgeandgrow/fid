# Fides Project Index

> Navigation hub for the Fides compiler. Every file in the project, grouped by
> directory, with a one-line description. Use this to find anything fast.
>
> **Fides is self-hosted and released (v1.0.0).** The primary compiler is in `compiler/*.fd`
> (54 modules, 104,439 LOC). The Rust source in `src/*.rs` (55 files, 100,935 LOC) is the
> bootstrap reference implementation, used when changing compiler internals.
> The self-hosted compiler is the release compiler (`release/fid.exe`,
> shipped as `fid.exe`); its `run` and `ccompile` match `fidesc` on the
> test programs (see `AGENTS.md` → Known Bugs #1 for the exceptions).
>
> **Stats:** 54 self-hosted `.fd` compiler modules (~105K LOC) · 55 `.rs` bootstrap files (~101K LOC) ·
> 104 stdlib modules (30,446 LOC) · 348 `.fd` test files (302 bootstrap + 46 selfhost) · 10 examples · 4 binaries.
> Fixpoint: VERIFIED byte-for-byte (stage 2 == stage 3, ~38 MB of C; checked by `RELEASE=1 scripts/bootstrap.sh`).
> v1.0.0 released Sep 9, 2026 with Windows installer (Fides-Setup-1.0.0.exe, 2.5 MB).

---

## Table of Contents

- [Documentation (docs/)](#documentation-docs)
- [Root Files](#root-files)
- [Self-Hosted Compiler (compiler/) — PRIMARY](#self-hosted-compiler-compiler--primary)
- [Source — Compiler Core (src/) — BOOTSTRAP REFERENCE](#source--compiler-core-src--bootstrap-reference)
- [Source — Backends & Codegen (src/)](#source--backends--codegen-src)
- [Source — Object Formats & Linking (src/)](#source--object-formats--linking-src)
- [Source — Advanced / Experimental (src/)](#source--advanced--experimental-src)
- [Standard Library (stdlib/)](#standard-library-stdlib)
- [Tests (tests/)](#tests-tests)
- [Scripts (scripts/)](#scripts-scripts)
- [Examples (examples/)](#examples-examples)
- [Benchmarks (benches/ & benchmarks/)](#benchmarks-benches--benchmarks)
- [Build Artifacts & Generated Files](#build-artifacts--generated-files)

---

## Documentation (docs/)

Design, architecture, roadmap, and analysis documents. Read `architecture.md`
and `AGENTS.md` first when working on the compiler.

### Core Reference
| File | Description |
|------|-------------|
| [reference/architecture.md](reference/architecture.md) | Custom-toolchain rationale (§0) + compiler pipeline, data flow, source file map. **Start here.** |
| [reference/syntax.md](reference/syntax.md) | Full language syntax specification (current `fn`/`val`/`var`/`const`/`return`/`type` syntax — migration from old `pf`/`pv`/`pc`/`ret`/`ty` is complete). |
| [reference/features.md](reference/features.md) | Feature status — verified against actual test runs. |
| [reference/tool.md](reference/tool.md) | Development tools (fidesc, ark, scribe, veil). |
| [reference/testing.md](reference/testing.md) | Current test results across all backends + test batch specifications. |
| [reference/changelog.md](reference/changelog.md) | Per-phase change history (newest first). |
| [reference/language-guide.md](reference/language-guide.md) | Comprehensive language guide — type system, ownership, effects; includes references §4.6 and @secret §5.4. |
| [reference/selfhost.md](reference/selfhost.md) | Self-hosted compiler — coordination, architecture, build pipeline, porting guide (consolidated). |
| [reference/quickstart.md](reference/quickstart.md) | Install + first program. |

### Guides (how things work)
| File | Description |
|------|-------------|
| [reference/best-practices.md](reference/best-practices.md) | Hard-won insights from building a custom toolchain. |
| [reference/calling-conventions.md](reference/calling-conventions.md) | SRET, enum layout, struct copy, Box semantics — low-level codegen machinery. |
| [reference/known-bugs.md](reference/known-bugs.md) | Self-host known bugs & history (+ port-gap-analysis appendix). |
| [reference/borrowchecker.md](reference/borrowchecker.md) | Borrow checker design document (Phases 1-6). |

### Meta (big-picture thinking)
| File | Description |
|------|-------------|
| [reference/vision.md](reference/vision.md) | Vision, design principles, decisions, future direction (consolidated). |
| [reference/positioning.md](reference/positioning.md) | Competitive analysis (vs Rust/Zig/Go/Swift) + adoption strategy + industry conversion guide. |
| [reference/lessons.md](reference/lessons.md) | PL history lessons + IBM AIX/z/OS lessons learned. |


### Roadmaps
| File | Description |
|------|-------------|
| [roadmap/roadmap.md](roadmap/roadmap.md) | Master roadmap — milestones, features, release/self-host, limitations, issues, opportunities (consolidated 2026-10-24). |
| [roadmap/release-roadmap.md](roadmap/release-roadmap.md) | Road to public beta and stable 1.0 — every known gap, by track, with acceptance criteria. |
| [roadmap/llvm-roadmap.md](roadmap/llvm-roadmap.md) | LLVM parity & superset roadmap — compete, don't copy. |
| [roadmap/safety-critical.md](roadmap/safety-critical.md) | Safety-critical roadmap (IEC 62304 / DO-178C class work). |
| [roadmap/security-roadmap.md](roadmap/security-roadmap.md) | Security hardening guide and exploit-class coverage. |
| [roadmap/bug-audit-2026-10-22.md](roadmap/bug-audit-2026-10-22.md) | Dated bug-audit snapshot. |

### Other
| File | Description |
|------|-------------|
| [fid-logo.svg](fid-logo.svg) | Project logo (SVG). |

---

## Root Files

| File | Description |
|------|-------------|
| [README.md](../README.md) | Project overview, status, feature matrix, quick start. |
| [AGENTS.md](../AGENTS.md) | **Working guide for AI assistants & humans** — build commands, high-risk files, known bugs, conventions. Read before editing. |
| [Cargo.toml](../Cargo.toml) | Rust package manifest (`fidesc` v0.1.0). |
| [Cargo.lock](../Cargo.lock) | Pinned dependency versions. |
| [verify.sh](../verify.sh) | Full verification: build + test + compare to baseline. `--save` updates baseline, `--quick` is build-only. |
| [.gitignore](../.gitignore) | Git ignore rules. |

---

## Self-Hosted Compiler (compiler/) — PRIMARY

> **This is the primary compiler.** 54 Fides modules (~105K LOC) that compile
> themselves to byte-identical C across generations (bootstrap fixpoint
> verified, stage 2 == stage 3). The release build
> (`RELEASE=1 scripts/bootstrap.sh` → `release/fid.exe`) is what the
> installer ships as `fid.exe`.
> Per-module findings are tracked in `file-analysis/PROGRESS.md`.

| File | LOC | Description |
|------|----:|-------------|
| [main.fd](../compiler/main.fd) | 2,267 | CLI entry point (command dispatch: run, dump-c, ccompile, fmt, lsp, codegen, build-x86, gpu, wasm, parse-header, …) |
| [token.fd](../compiler/token.fd) | 564 | Token kind definitions (Yield tag = 136) |
| [lexer.fd](../compiler/lexer.fd) | 921 | Tokenizer: source text → tokens |
| [parser.fd](../compiler/parser.fd) | 5,472 | Recursive descent parser: tokens → AST (f-string sub-parse still stubbed) |
| [ast.fd](../compiler/ast.fd) | 1,278 | AST node definitions: Expr, Stmt, Func, StructDef, EnumDef, etc. |
| [typechecker.fd](../compiler/typechecker.fd) | 8,300 | Type checking + capability + effect + region + sandbox enforcement (verify layer is permissive) |
| [borrowck.fd](../compiler/borrowck.fd) | 2,093 | Borrow checker (NLL, field-wise, two-phase, off/warn/on modes) |
| [escape.fd](../compiler/escape.fd) | 1,502 | Reference escape analysis — second-class references (session 29) |
| [mono.fd](../compiler/mono.fd) | 2,643 | Generic monomorphization |
| [ir.fd](../compiler/ir.fd) | 2,640 | SSA IR definitions + verify + dead-function elimination |
| [lower.fd](../compiler/lower.fd) | 7,059 | AST → SSA IR lowering |
| [pir.fd](../compiler/pir.fd) | 2,704 | PIR (L1/L2/L3) transforms |
| [opt.fd](../compiler/opt.fd) | 7,763 | PIR optimizer (~19 passes; Licm/EscapeAnalysis absent) |
| [peephole.fd](../compiler/peephole.fd) | 1,150 | 9-pass peephole optimizer (always on in `build`) |
| [regalloc.fd](../compiler/regalloc.fd) | 1,354 | Linear scan register allocator (remat, effect-aware clobbers) |
| [codegen_c.fd](../compiler/codegen_c.fd) | 5,922 | C codegen backend (IR → .c); cast_arg Ptr→Generic deref added session 30 |
| [codegen_x64.fd](../compiler/codegen_x64.fd) | 2,000 | x86-64 native codegen (L3 → asm; float-ABI regression documented) |
| [codegen_arm64.fd](../compiler/codegen_arm64.fd) | 553 | ARM64 codegen (--target arm64; slot/immediate regressions documented) |
| [codegen_riscv.fd](../compiler/codegen_riscv.fd) | 554 | RISC-V codegen (--target riscv; same class of issues) |
| [codegen.fd](../compiler/codegen.fd) | 1,497 | Legacy AST→ARM64 codegen — dead module |
| [x86.fd](../compiler/x86.fd) | 1,779 | x86-64 MASM-style backend (build-x86) |
| [elf64.fd](../compiler/elf64.fd) | 2,780 | ELF64 emitter + full x86-64 instruction encoder |
| [pe.fd](../compiler/pe.fd) | 6,852 | PE format writer — consumes codegen_x64 asm (D1 refactor) |
| [coff.fd](../compiler/coff.fd) | 426 | COFF object writer (format-only, shares x86 encoders) |
| [macho.fd](../compiler/macho.fd) | 445 | Mach-O object writer (format-only, shares x86 encoders) |
| [linker.fd](../compiler/linker.fd) | 1,077 | ELF relocator/linker |
| [runtime.fd](../compiler/runtime.fd) | 531 | Fides-authored runtime scaffold (rt_sys_* ABI; not yet wired) |
| [runtime_linux.fd](../compiler/runtime_linux.fd) | 4,399 | x86-64 Linux runtime — hand-encoded syscalls |
| [runtime_arm64.fd](../compiler/runtime_arm64.fd) | 799 | ARM64 Linux runtime |
| [runtime_riscv.fd](../compiler/runtime_riscv.fd) | 827 | RISC-V Linux runtime |
| [arm64_enc.fd](../compiler/arm64_enc.fd) | 1,851 | ARM64 instruction encoder (asm text → machine code) |
| [riscv_enc.fd](../compiler/riscv_enc.fd) | 1,191 | RISC-V (RV64) instruction encoder |
| [comptime_eval.fd](../compiler/comptime_eval.fd) | 1,633 | Compile-time evaluation engine |
| [compiler.fd](../compiler/compiler.fd) | 3,078 | Pipeline orchestration |
| [flat_ast.fd](../compiler/flat_ast.fd) | 1,311 | Flat AST serialization (wired: dump-ast-flat) |
| [diag.fd](../compiler/diag.fd) | 659 | Diagnostics with error codes (dead — call sites dropped) |
| [fmt.fd](../compiler/fmt.fd) | 1,179 | Source code formatter (known corruption bugs — see file-analysis) |
| [json.fd](../compiler/json.fd) | 666 | JSON parser/serializer |
| [lsp.fd](../compiler/lsp.fd) | 1,082 | Language Server Protocol |
| [inc.fd](../compiler/inc.fd) | 166 | Incremental compilation cache (in-memory) |
| [ark.fd](../compiler/ark.fd) | 761 | Self-hosted Ark package manager |
| [scribe.fd](../compiler/scribe.fd) | 986 | Documentation generator |
| [veil.fd](../compiler/veil.fd) | 516 | Coverage tool (debug-hook dependent — 0% known issue) |
| [wasm.fd](../compiler/wasm.fd) | 1,656 | WASM backend |
| [gpu.fd](../compiler/gpu.fd) | 826 | SPIR-V/PTX GPU backend (stubs live but broken) |
| [tensor.fd](../compiler/tensor.fd) | 757 | Tensor/KV-cache/quantize builtins wiring |
| [autograd.fd](../compiler/autograd.fd) | 41 | Autograd shim (interp stubs; real impl in codegen_c) |
| [embedded.fd](../compiler/embedded.fd) | 511 | Embedded/HAL scaffolding (dead in Rust too) |
| [green_thread.fd](../compiler/green_thread.fd) | 207 | Green-thread scheduler (interp only; C backend lacks p_gt_*) |
| [debugger.fd](../compiler/debugger.fd) | 977 | IR debugger (dead — `debug` not dispatched) |
| [native_debugger.fd](../compiler/native_debugger.fd) | 544 | ptrace debugger skeleton (dead — ptrace missing) |
| [hotreload.fd](../compiler/hotreload.fd) | 635 | Hot reload watcher (decoy — `watch`→bench alias) |
| [cparser.fd](../compiler/cparser.fd) | 492 | C header parser → FFI bindings (parse-header) |
| **Total** | **104,439** | **54/54 modules — all type-check; fixpoint verified** |

---

## Source — Compiler Core (src/) — BOOTSTRAP REFERENCE

> **Note:** The Rust source is the bootstrap reference implementation. It is used
> to generate the first-generation C (`fides-c.c`) from the self-hosted compiler
> source (`compiler/*.fd`), and it holds the test harness (`fidesc test`).
> It is needed when `compiler/*.fd` changes and the bootstrap chain must be
> regenerated; the installer also ships it as `fidesc.exe` (its `emit`
> makes native Windows executables without a C compiler).

The front-end and middle-end pipeline: source → tokens → AST → types → IR.

| File | LOC | Description |
|------|----:|-------------|
| [main.rs](../src/main.rs) | 3442 | CLI entry point, command dispatch, external tool invocation (cl.exe/zig link). |
| [token.rs](../src/token.rs) | 263 | `TokenKind` enum — all Fides keywords and operators. |
| [lexer.rs](../src/lexer.rs) | 774 | Tokenizer: source text → `Vec<Token>`. |
| [ast.rs](../src/ast.rs) | 990 | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| [parser.rs](../src/parser.rs) | 3657 | Recursive descent parser: `Vec<Token>` → `Program`. Error recovery + multiple error collection. |
| [typechecker.rs](../src/typechecker.rs) | 9093 | Type validation, regions, effects, capabilities, traits, lints, @constant_time, Rc/Arc. ⚠️ MEDIUM risk. |
| [borrowck.rs](../src/borrowck.rs) | 1392 | Borrow checker (Phases 1-6): `--borrowck=off\|warn\|on`, field-wise borrows, NLL. |
| [escape.rs](../src/escape.rs) | 949 | Reference escape analysis — second-class references (session 29). |
| [ir.rs](../src/ir.rs) | 1811 | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`, `BorrowKind`. |
| [lower.rs](../src/lower.rs) | 7688 | AST → SSA IR lowering. ⚠️ MEDIUM risk — affects all backends. |
| [mono.rs](../src/mono.rs) | 1549 | Generic monomorphization — specializes generic functions per call site. |
| [comptime.rs](../src/comptime.rs) | 1358 | Compile-time eval: constant folding, reflection, f-string interpolation. |
| [compiler.rs](../src/compiler.rs) | 2843 | Pipeline orchestration, module resolution, stdlib auto-include, CT verify wiring. |
| [lsp.rs](../src/lsp.rs) | 1039 | LSP server: JSON-RPC 2.0 over stdio, diagnostics, no external deps. |
| [json.rs](../src/json.rs) | 289 | Built-in JSON parser/serializer (used by LSP, no external deps). |
| [cparser.rs](../src/cparser.rs) | 315 | C header parser → generates Fides FFI bindings (`parse-header` command). |
| [diag.rs](../src/diag.rs) | 463 | Unified diagnostic system: error codes, source context, colored output, Levenshtein suggestions. |
| [fmt.rs](../src/fmt.rs) | 1236 | Source code formatter (`fmt` command). |

---

## Source — Backends & Codegen (src/)

IR → target output. Multiple backends: interpreter, native ELF64, PE, C, WASM, GPU, multi-arch asm.

| File | LOC | Description |
|------|----:|-------------|
| [interp.rs](../src/interp.rs) | 6370 | IR interpreter — the reference oracle. ⚠️ MEDIUM risk. Uses `vec_side_table` for p_vec_* ops. Call stack tracking for panic traces. Rc/Arc runtime via p_rc_incr/p_rc_decr. |
| [pir.rs](../src/pir.rs) | 2262 | PIR multi-level IR (L1/L2/L3) transforms. ⚠️ HIGH risk. |
| [opt.rs](../src/opt.rs) | 8574 | PIR optimizer — 23 passes (const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, etc.). Function-level parallelism. ⚠️ MEDIUM risk. |
| [peephole.rs](../src/peephole.rs) | 994 | 9-pass peephole optimizer, always on in `build`. ⚠️ CRITICAL. |
| [regalloc.rs](../src/regalloc.rs) | 1512 | Linear scan register allocator with constant rematerialization. ⚠️ CRITICAL. |
| [codegen_x64.rs](../src/codegen_x64.rs) | 2629 | PIR L3 → x86-64 assembly (used by `build` and `codegen`) + post-codegen `verify_constant_time_asm`. LEA instruction selection, System V ABI. ⚠️ HIGH risk. |
| [codegen_c.rs](../src/codegen_c.rs) | 5020 | C codegen backend: IR → C99 source (`ccompile` command). Binary hardening + stack traces + Rc/Arc runtime. `cast_arg` Ptr→Generic deref (session 30). ⚠️ HIGH risk. |
| [codegen_arm64.rs](../src/codegen_arm64.rs) | 394 | PIR L3 → ARM64 assembly (`codegen --target arm64`). |
| [codegen_riscv.rs](../src/codegen_riscv.rs) | 388 | PIR L3 → RISC-V assembly (`codegen --target riscv`). |
| [codegen.rs](../src/codegen.rs) | 1150 | Legacy codegen dispatch (unused). |
| [x86.rs](../src/x86.rs) | 1420 | x86-64 MASM codegen: IR → assembly string (legacy `build` path). |
| [wasm.rs](../src/wasm.rs) | 1277 | WASM backend: PIR L3 → WebAssembly binary (`wasm` command). |
| [gpu.rs](../src/gpu.rs) | 590 | SPIR-V GPU backend: PIR L3 → SPIR-V binary (`gpu` command). |
| [runtime_linux.rs](../src/runtime_linux.rs) | 4383 | Hand-encoded x86-64 Linux runtime (raw syscall bytes, no libc). ⚠️ HIGH risk. |
| [arm64_enc.rs](../src/arm64_enc.rs) | 1328 | ARM64 (AArch64) instruction encoder: assembly text → 4-byte machine code. 17 tests. |
| [runtime_arm64.rs](../src/runtime_arm64.rs) | 961 | ARM64 Linux runtime using raw SVC syscalls (write, exit, mmap). 9 tests. |
| [riscv_enc.rs](../src/riscv_enc.rs) | 933 | RISC-V (RV64) instruction encoder: assembly text → 4-byte machine code. 14 tests. |
| [runtime_riscv.rs](../src/runtime_riscv.rs) | 868 | RISC-V Linux runtime using raw ECALL syscalls (write, exit, mmap). 7 tests. |

---

## Source — Object Formats & Linking (src/)

Binary emitters and the integrated linker. Byte-level correctness is critical.

| File | LOC | Description |
|------|----:|-------------|
| [elf64.rs](../src/elf64.rs) | 2852 | ELF64 emitter: PIR L3 → executable + relocatable objects (Linux). ⚠️ HIGH risk. |
| [pe.rs](../src/pe.rs) | 8966 | Direct PE emitter: IR → x86_64 machine code → PE `.exe` (no external tools). ⚠️ HIGH risk. |
| [macho.rs](../src/macho.rs) | 374 | Mach-O emitter: object files for macOS. ⚠️ HIGH risk. |
| [coff.rs](../src/coff.rs) | 385 | COFF emitter: object files for Windows. ⚠️ HIGH risk. |
| [linker.rs](../src/linker.rs) | 968 | Integrated linker: resolves relocations across multiple object files. Target-aware (ARM64, RISC-V, x86-64). ⚠️ MEDIUM risk. |
| [dwarf.rs](../src/dwarf.rs) | 516 | DWARF debug info emission (started — see roadmap Phase 5). |

---

## Source — Advanced / Experimental (src/)

Specialized subsystems: embedded, hot reload, autograd, tensor, debugger, tools.

| File | LOC | Description |
|------|----:|-------------|
| [embedded.rs](../src/embedded.rs) | 622 | Embedded systems: `FixedBufferAllocator`, `RealTime` effect, HAL abstractions. |
| [hotreload.rs](../src/hotreload.rs) | 559 | Hot reload: native JIT (ELF64) + function-level change detection (`watch` command). |
| [autograd.rs](../src/autograd.rs) | 32 | Tape-based reverse-mode autograd for `grad` blocks (mostly stubs — real impl in codegen_c). |
| [tensor.rs](../src/tensor.rs) | 609 | Tensor type and operations support. |
| [inc.rs](../src/inc.rs) | 171 | Incremental compilation cache (in-memory, hash-keyed by path+hash+flags). |
| [debugger.rs](../src/debugger.rs) | 965 | Interactive IR-level debugger (`debug` command): breakpoints, step, locals. |
| [native_debugger.rs](../src/native_debugger.rs) | 311 | Native ptrace-based debugger (`autopsy` command, Linux only). |
| [ark.rs](../src/ark.rs) | 707 | Package manager library: init, add, install, remove, list, lock, search, publish. |
| [scribe.rs](../src/scribe.rs) | 881 | Documentation generator library: HTML/Markdown from .fd source. |
| [veil.rs](../src/veil.rs) | 367 | Code coverage tool library: function/line/instruction coverage. |
| [flat_ast.rs](../src/flat_ast.rs) | 780 | Flat AST serialization (`dump-ast-flat` / `diff-test`). |
| [green_thread.rs](../src/green_thread.rs) | 600 | Green-thread scheduler (interp-backed). |
| [lib.rs](../src/lib.rs) | 66 | Library crate root — shared by all 4 binaries. |
| [bin/ark.rs](../src/bin/ark.rs) | — | `ark` binary entry point. |
| [bin/scribe.rs](../src/bin/scribe.rs) | — | `scribe` binary entry point. |
| [bin/veil.rs](../src/bin/veil.rs) | — | `veil` binary entry point. |

---

## Standard Library (stdlib/)

104 Fides source modules (30,446 LOC). `core.fd` is auto-included by the
compiler; others are imported via `use`. The table below lists the
foundational modules — the full set (including the session-21 game/media
batch and the industry modules) is in `stdlib/`; per-module docs live in
`docs/stdlib/` (coverage is partial — 30 modules lack a page).
`kernel.fd`/`uefi.fd` moved to the `fides-os` project.
See `docs/reference/features.md` for which are feature-complete.

| File | Description |
|------|-------------|
| [core.fd](../stdlib/core.fd) | Core stdlib (auto-included): `Option<T>`, `Result<T,E>`, `println`, `assert`, `assert_eq`. Option/Result combinators: map, and_then, or_else, unwrap_or_else, expect. |
| [vec.fd](../stdlib/vec.fd) | `Vec<T>` container, higher-order methods (map, filter, fold), iterator adapters (take, skip, zip, enumerate, flat_map, chunk, window, dedup, sort_by, contains_val). |
| [channel.fd](../stdlib/channel.fd) | Bounded MPSC channels: `channel_create`, `channel_send`, `channel_recv`, `channel_try_recv`, `channel_close`, `channel_is_closed`. |
| [math.fd](../stdlib/math.fd) | Math functions: sqrt, sin, cos, floor, ceil, pow, fabs, constants. Vec2/3/4, mat4, quat types with operator overloading + swizzle. |
| [ecs.fd](../stdlib/ecs.fd) | Entity-Component-System runtime (Phase 8B: F18). |
| [events.fd](../stdlib/events.fd) | Event system / handlers (Phase 8B: F20). |
| [parallel.fd](../stdlib/parallel.fd) | Auto-parallelization framework (Phase 5C.4). |
| [sync.fd](../stdlib/sync.fd) | Concurrency primitives. |
| [db.fd](../stdlib/db.fd) | Database driver framework (Phase 5D.4). |
| [http.fd](../stdlib/http.fd) | HTTP server (Phase 5D.3). |
| [json.fd](../stdlib/json.fd) | JSON parsing/serialization (Phase 5D.2). |
| [shape.fd](../stdlib/shape.fd) | Comptime tensor shape verification (Phase 5C.3). |
| [simd.fd](../stdlib/simd.fd) | SIMD primitives (Phase 5C.2). |
| [tensor.fd](../stdlib/tensor.fd) | Tensor type and ops (Phase 5C.1). |
| [quant.fd](../stdlib/quant.fd) | Quantization (Phase 5E.1). |
| [money.fd](../stdlib/money.fd) | Fixed-point money type, arithmetic, formatting (Phase 7.5B). |
| [banking.fd](../stdlib/banking.fd) | Accounts, transfers, double-credit/lost-money detection (Phase 7.5B). |
| [alloc.fd](../stdlib/alloc.fd) | Allocator abstractions (Phase 10A). |
| [file.fd](../stdlib/file.fd) | File I/O via file descriptors. |
| [time.fd](../stdlib/time.fd) | LogicalTime — deterministic, clock-free date math (Phase 7.5A, F43). |
| [typed_strings.fd](../stdlib/typed_strings.fd) | SQLString/ShellString/HTMLString/JSONString/PathString — injection prevention (F67). |
| [memo.fd](../stdlib/memo.fd) | Memoization helper: `Memo::new()`, `get_or_compute()`. |
| [privacy.fd](../stdlib/privacy.fd) | Private field access test module. |
| [env.fd](../stdlib/env.fd) | Environment variable access: `env_get`, `env_exists`. |
| [args.fd](../stdlib/args.fd) | Command-line arguments: `args_len`, `args_get`, `args_program_name`. |
| [process.fd](../stdlib/process.fd) | Process control: `process_exit`, `process_system`. |
| [regex.fd](../stdlib/regex.fd) | Basic wildcard pattern matching. |
| [tilemap.fd](../stdlib/tilemap.fd) | Tilemap game stdlib (Phase 8D S1). |
| [sprite.fd](../stdlib/sprite.fd) | Sprite game stdlib (Phase 8D S2). |
| [flowfield.fd](../stdlib/flowfield.fd) | Flowfield game stdlib (Phase 8D S3). |
| [streaming.fd](../stdlib/streaming.fd) | Streaming game stdlib (Phase 8D S4). |
| [scene.fd](../stdlib/scene.fd) | Scene graph game stdlib (Phase 8D S5). |
| [physics.fd](../stdlib/physics.fd) | Physics game stdlib (Phase 8D S6). |
| [audio.fd](../stdlib/audio.fd) | Audio game stdlib (Phase 8D S7). |
| [input.fd](../stdlib/input.fd) | Input handling game stdlib (Phase 8D S8). |
| [rc.fd](../stdlib/rc.fd) | Reference counting: `rc_new`, `rc_clone`, `rc_drop`, `rc_get`, `rc_count` (and Arc equivalents). |
| `kernel.fd` | *(moved)* — OS kernel primitives live in the `fides-os` project. |
| `uefi.fd` | *(moved)* — UEFI boot services bindings live in the `fides-os` project. |

---

## Tests (tests/)

348 `.fd` test files: 302 in `tests/bootstrap/` (+ categorized subdirs:
capability/effects/regions/safety) and 46 in `tests/selfhost/`, plus the Rust
test harness. Run all with `./target/release/fidesc test`; verify with
`./verify.sh` (bootstrap) and `./verify-selfhost.sh` (selfhost).

### Test Infrastructure
| File | Description |
|------|-------------|
| [harness.rs](../tests/harness.rs) | Rust integration test harness — runs `.fd` tests across backends, holds the skip list (`is_skip_test()`). |
| [baseline/test_results.txt](../tests/bootstrap/baseline/test_results.txt) | Saved baseline output for `./verify.sh` comparison. |

### Test Subdirectories (categorized `.fd` tests)
| Directory | Contents |
|-----------|----------|
| [capability/](../tests/bootstrap/capability/) | Capability security tests (grant/deny/propagation, IO). |
| [effects/](../tests/bootstrap/effects/) | Effect system tests (annotations, handlers, purity, propagation). |
| [regions/](../tests/bootstrap/regions/) | Region inference tests. |
| [safety/](../tests/bootstrap/safety/) | Safety annotation tests. |
| [golden/](../tests/bootstrap/golden/) | Golden output files for intermediate compiler stages (IR, PIR L1/L2/L3, codegen, peephole). Contains per-test subdirectories + `.txt` golden files. |

### Top-level `.fd` Tests (selected)
302 `.fd` test files in tests/bootstrap/ covering: arithmetic, control flow,
functions, strings, structs, pattern matching, enums, closures, generics,
traits, capabilities, collections (Vec/HashMap/String), Option/Result, FFI,
file I/O, async/await, borrow checking, linear types, contracts, ECS, state
machines, serde, iterators, error chaining, conditional compilation, derive
macros, dlopen, typed strings, raw pointers, select statements, Rc/Arc,
Box recursive types, vec/mat math, and more.

Expected-fail tests (24) verify that the compiler correctly *rejects* unsafe
code: `test_*_fail` variants for banking, money, borrowck, safety, contracts,
typed strings, init guarantees, constant_time, etc.

### Current Test Results

**See `AGENTS.md` → "Current Baseline"** for authoritative test counts.
Baseline file: `tests/bootstrap/baseline/test_results.txt`.

> Full test inventory: see `tests/` directory listing, or run
> `./target/release/fidesc test` to see all test names and results.

---

## Scripts (scripts/)

| File | Description |
|------|-------------|
| [verify.sh](../verify.sh) | *(root)* Full verification: build + test + compare to baseline. `--save` updates baseline, `--quick` is build-only. `--golden` is currently broken (the helper script it calls is missing). |
| [verify-selfhost.sh](../verify-selfhost.sh) | *(root)* tests/selfhost/ on 3 backends (interp, in-process C, in-process PE). |
| [bootstrap.sh](../scripts/bootstrap.sh) | Multi-stage bootstrap: builds c1…cN and checks the C fixpoint. `RELEASE=1` builds 3 stages, installs `release/fid.exe` and smoke-tests it from an installed layout. |
| [build-selfhost.sh](../scripts/build-selfhost.sh) | Build the self-hosted compiler into `release/` — produces `release/fid-dev.c` + `release/fid-dev(.exe)`. |
| `bisect.sh`, `gen_golden.sh`, `test_build.sh` | **Do not exist** — older docs still reference them; bisect manually via `dump-*`/`codegen --peephole` and regenerate golden files by hand. |

---

## Examples (examples/)

| File | Description |
|------|-------------|
| [http_server.fd](../examples/http_server.fd) | HTTP server example (Phase 5D.3). |
| [neural_network.fd](../examples/neural_network.fd) | Neural network forward pass in pure Fides (Phase 5C.5). |
| [riscv_blink.fd](../examples/riscv_blink.fd) | Bare-metal LED blink for SiFive HiFive1 RISC-V (Phase 5B). |
| [rpi_led_blink.fd](../examples/rpi_led_blink.fd) | Bare-metal LED blinker for Raspberry Pi 3 ARM64 (Phase 5A). |

---

## Build Artifacts & Generated Files

These are outputs, not source. Safe to delete; regenerated by builds/tests.
All are gitignored (see `.gitignore`).

| File | Description |
|------|-------------|
| `target/` | Cargo build output (debug + release binaries). |
| `release/fid(.exe)` | The shipped self-hosted compiler (stage-3 fixpoint build; `RELEASE=1 scripts/bootstrap.sh`). |
| `release/fid-dev.c`, `release/fid-dev(.exe)`, `release/fid-dev.log`, `release/fid-dev.cc.log` | Dev self-compile artifact + build logs (`./scripts/build-selfhost.sh`). `release/archive/fid-old.c` is a stale previous generation, kept for diffing. |
| `tests/*.c` | Generated C output from test runs (auto-generated from `.fd` files). |
| `tests/bootstrap/test_async`, `tests/bootstrap/test_float`, `tests/bootstrap/test_generics` | Compiled native test binaries. |
| `tests/selfhost/*` (extensionless) | Compiled self-host probe/harness binaries (built from `.c` sources). |
| `tests/bootstrap/golden/*/` | Per-test golden output subdirectories (regenerate manually — `gen_golden.sh` does not exist). |
| `output.s` | Generated assembly output. |
| `_ast.txt`, `_ir.txt`, `_ir2.txt` | Dumped AST/IR snapshots (debugging artifacts). |
| `out.txt` | Empty output capture file. |

---

## How to Use This Index

- **New to the codebase?** Read [README.md](../README.md) → [reference/architecture.md](reference/architecture.md) → [AGENTS.md](../AGENTS.md).
- **Looking for a specific feature?** Check [reference/features.md](reference/features.md) and [reference/syntax.md](reference/syntax.md).
- **About to edit the compiler?** Read [AGENTS.md](../AGENTS.md) — especially the high-risk file table. Run `./verify.sh` before and after.
- **A test broke?** Bisect manually with `dump-ir` / `dump-pir` / `codegen --peephole` (no bisect script exists).
- **Adding a new file?** Add a row to the relevant table above so this index stays current.
