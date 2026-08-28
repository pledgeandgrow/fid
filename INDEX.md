# Fides Project Index

> Navigation hub for the Fides compiler. Every file in the project, grouped by
> directory, with a one-line description. Use this to find anything fast.
>
> **Stats:** 54 `.rs` source files (85,493 LOC) · 69 stdlib modules (17,567 LOC) · 218 `.fd`
> test files · 33 .md docs · 4 examples · 3 scripts · 3 benchmarks · 4 binaries (fidesc, pacte, scribe, veil).
> Cross-compilation: ARM64, RISC-V, Windows — zero external tools.

---

## Table of Contents

- [Documentation (docs/)](#documentation-docs)
- [Root Files](#root-files)
- [Source — Compiler Core (src/)](#source--compiler-core-src)
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
| [reference/architecture.md](reference/architecture.md) | Compiler pipeline, data flow, source file map. **Start here.** |
| [reference/syntax.md](reference/syntax.md) | Full language syntax specification (current `fn`/`val`/`var`/`const`/`return`/`type` syntax — migration from old `pf`/`pv`/`pc`/`ret`/`ty` is complete). |
| [reference/features.md](reference/features.md) | Feature status — verified against actual test runs. |
| [reference/tool.md](reference/tool.md) | Development tools (fidesc, pacte, scribe, veil). |
| [reference/testing.md](reference/testing.md) | Current test results across all backends + test batch specifications. |
| [reference/changelog.md](reference/changelog.md) | Per-phase change history (newest first). |
| [reference/vision.md](reference/vision.md) | The overarching vision — "the last language the world needs." |

### Guides (how things work)
| File | Description |
|------|-------------|
| [guides/best-practices.md](guides/best-practices.md) | Hard-won insights from building a custom toolchain. |
| [guides/architecture-overview.md](guides/architecture-overview.md) | Custom toolchain rationale (no LLVM/libc) + full-stack architecture. |
| [guides/borrowchecker.md](guides/borrowchecker.md) | Borrow checker design document (Phases 1-6). |

### Meta (big-picture thinking)
| File | Description |
|------|-------------|
| [meta/strategy.md](meta/strategy.md) | Design principles + direction analysis (are we going the right way?). |
| [meta/positioning.md](meta/positioning.md) | Competitive analysis (vs Rust/Zig/Go/Swift) + adoption strategy + industry conversion guide. |
| [meta/future.md](meta/future.md) | Innovation (AI, robotics, hardware, IoT) + nice-to-have community goals. |
| [meta/lessons.md](meta/lessons.md) | PL history lessons + IBM AIX/z/OS lessons learned. |

### Roadmaps
| File | Description |
|------|-------------|
| [roadmap/roadmap.md](roadmap/roadmap.md) | Milestones to "world domination" — the master roadmap. |
| [roadmap/features-roadmap.md](roadmap/features-roadmap.md) | New language features roadmap (track of additions). |
| [roadmap/llvm-roadmap.md](roadmap/llvm-roadmap.md) | LLVM parity & superset roadmap — compete, don't copy. |
| [roadmap/roadmap-limitations.md](roadmap/roadmap-limitations.md) | Limitations & pain points to address. |
| [roadmap/opportunities.md](roadmap/opportunities.md) | AI/ML feature opportunities + where Fides could be groundbreaking by industry. |

### Other
| File | Description |
|------|-------------|
| [fides-logo.svg](fides-logo.svg) | Project logo (SVG). |

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

## Source — Compiler Core (src/)

The front-end and middle-end pipeline: source → tokens → AST → types → IR.

| File | LOC | Description |
|------|----:|-------------|
| [main.rs](../src/main.rs) | 2221 | CLI entry point, command dispatch, external tool invocation. |
| [token.rs](../src/token.rs) | 263 | `TokenKind` enum — all Fides keywords and operators. |
| [lexer.rs](../src/lexer.rs) | 539 | Tokenizer: source text → `Vec<Token>`. |
| [ast.rs](../src/ast.rs) | 916 | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| [parser.rs](../src/parser.rs) | 3136 | Recursive descent parser: `Vec<Token>` → `Program`. Error recovery + multiple error collection. |
| [typechecker.rs](../src/typechecker.rs) | 6435 | Type validation, regions, effects, capabilities, traits, lints, @constant_time, Rc/Arc. ⚠️ MEDIUM risk. |
| [borrowck.rs](../src/borrowck.rs) | 1208 | Borrow checker (Phases 1-6): `--borrowck=off\|warn\|on`, field-wise borrows, NLL. |
| [ir.rs](../src/ir.rs) | 1696 | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`, `BorrowKind`. |
| [lower.rs](../src/lower.rs) | 5724 | AST → SSA IR lowering. ⚠️ MEDIUM risk — affects all backends. |
| [mono.rs](../src/mono.rs) | 1471 | Generic monomorphization — specializes generic functions per call site. |
| [comptime.rs](../src/comptime.rs) | 1316 | Compile-time eval: constant folding, reflection, f-string interpolation. |
| [compiler.rs](../src/compiler.rs) | 2084 | Pipeline orchestration, module resolution, stdlib auto-include. |
| [lsp.rs](../src/lsp.rs) | 892 | LSP server: JSON-RPC 2.0 over stdio, diagnostics, no external deps. |
| [json.rs](../src/json.rs) | 289 | Built-in JSON parser/serializer (used by LSP, no external deps). |
| [cparser.rs](../src/cparser.rs) | 315 | C header parser → generates Fides FFI bindings (`parse-header` command). |
| [diag.rs](../src/diag.rs) | 463 | Unified diagnostic system: error codes, source context, colored output, Levenshtein suggestions. |
| [fmt.rs](../src/fmt.rs) | 1165 | Source code formatter (`fmt` command). |

---

## Source — Backends & Codegen (src/)

IR → target output. Multiple backends: interpreter, native ELF64, PE, C, WASM, GPU, multi-arch asm.

| File | LOC | Description |
|------|----:|-------------|
| [interp.rs](../src/interp.rs) | 4903 | IR interpreter — the reference oracle. ⚠️ MEDIUM risk. Uses `vec_side_table` for p_vec_* ops. Call stack tracking for panic traces. Rc/Arc runtime via p_rc_incr/p_rc_decr. |
| [pir.rs](../src/pir.rs) | 2089 | PIR multi-level IR (L1/L2/L3) transforms. ⚠️ HIGH risk. |
| [opt.rs](../src/opt.rs) | 8109 | PIR optimizer — 23 passes (const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, etc.). Function-level parallelism. ⚠️ MEDIUM risk. |
| [peephole.rs](../src/peephole.rs) | 994 | 9-pass peephole optimizer, always on in `build`. ⚠️ CRITICAL. |
| [regalloc.rs](../src/regalloc.rs) | 1170 | Linear scan register allocator with constant rematerialization. ⚠️ CRITICAL. |
| [codegen_x64.rs](../src/codegen_x64.rs) | 1939 | PIR L3 → x86-64 assembly (used by `build` and `codegen`). LEA instruction selection, System V ABI. ⚠️ HIGH risk. |
| [codegen_c.rs](../src/codegen_c.rs) | 3622 | C codegen backend: IR → C99 source (`ccompile` command). Binary hardening + stack traces + Rc/Arc runtime. ⚠️ HIGH risk. |
| [codegen_arm64.rs](../src/codegen_arm64.rs) | 364 | PIR L3 → ARM64 assembly (`codegen --target arm64`). |
| [codegen_riscv.rs](../src/codegen_riscv.rs) | 360 | PIR L3 → RISC-V assembly (`codegen --target riscv`). |
| [codegen.rs](../src/codegen.rs) | 1134 | Legacy codegen dispatch (unused). |
| [x86.rs](../src/x86.rs) | 1461 | x86-64 MASM codegen: IR → assembly string (legacy `build` path). |
| [wasm.rs](../src/wasm.rs) | 1248 | WASM backend: PIR L3 → WebAssembly binary (`wasm` command). |
| [gpu.rs](../src/gpu.rs) | 601 | SPIR-V GPU backend: PIR L3 → SPIR-V binary (`gpu` command). |
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
| [elf64.rs](../src/elf64.rs) | 2609 | ELF64 emitter: PIR L3 → executable + relocatable objects (Linux). ⚠️ HIGH risk. |
| [pe.rs](../src/pe.rs) | 6392 | Direct PE emitter: IR → x86_64 machine code → PE `.exe` (no external tools). ⚠️ HIGH risk. |
| [macho.rs](../src/macho.rs) | 377 | Mach-O emitter: object files for macOS. ⚠️ HIGH risk. |
| [coff.rs](../src/coff.rs) | 396 | COFF emitter: object files for Windows. ⚠️ HIGH risk. |
| [linker.rs](../src/linker.rs) | 951 | Integrated linker: resolves relocations across multiple object files. Target-aware (ARM64, RISC-V, x86-64). ⚠️ MEDIUM risk. |

---

## Source — Advanced / Experimental (src/)

Specialized subsystems: embedded, hot reload, autograd, tensor, debugger, tools.

| File | LOC | Description |
|------|----:|-------------|
| [embedded.rs](../src/embedded.rs) | 622 | Embedded systems: `FixedBufferAllocator`, `RealTime` effect, HAL abstractions. |
| [hotreload.rs](../src/hotreload.rs) | 559 | Hot reload: native JIT (ELF64) + function-level change detection (`watch` command). |
| [autograd.rs](../src/autograd.rs) | 289 | Tape-based reverse-mode autograd for `grad` blocks. |
| [tensor.rs](../src/tensor.rs) | 462 | Tensor type and operations support. |
| [inc.rs](../src/inc.rs) | 171 | Incremental compilation cache (in-memory, hash-keyed by path+hash+flags). |
| [debugger.rs](../src/debugger.rs) | 965 | Interactive IR-level debugger (`debug` command): breakpoints, step, locals. |
| [native_debugger.rs](../src/native_debugger.rs) | 312 | Native ptrace-based debugger (`autopsy` command, Linux only). |
| [pacte.rs](../src/pacte.rs) | 707 | Package manager library: init, add, install, remove, list, lock, search, publish. |
| [scribe.rs](../src/scribe.rs) | 882 | Documentation generator library: HTML/Markdown from .fd source. |
| [veil.rs](../src/veil.rs) | 367 | Code coverage tool library: function/line/instruction coverage. |
| [lib.rs](../src/lib.rs) | 52 | Library crate root — shared by all 4 binaries. |
| [bin/pacte.rs](../src/bin/pacte.rs) | — | `pacte` binary entry point. |
| [bin/scribe.rs](../src/bin/scribe.rs) | — | `scribe` binary entry point. |
| [bin/veil.rs](../src/bin/veil.rs) | — | `veil` binary entry point. |

---

## Standard Library (stdlib/)

35 Fides source modules. `core.fd` is auto-included by the compiler; others are
imported via `use`. See `docs/reference/features.md` for which are feature-complete.

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

---

## Tests (tests/)

218 `.fd` test files (195 in root; 23 in subdirs: capability/effects/regions/safety) plus the Rust test harness.
Run all with `./target/release/fidesc test`; verify with `./verify.sh`.

### Test Infrastructure
| File | Description |
|------|-------------|
| [harness.rs](../tests/harness.rs) | Rust integration test harness — runs `.fd` tests across backends, holds the skip list (`is_skip_test()`). |
| [baseline/test_results.txt](../tests/baseline/test_results.txt) | Saved baseline output for `./verify.sh` comparison. |

### Test Subdirectories (categorized `.fd` tests)
| Directory | Contents |
|-----------|----------|
| [capability/](../tests/capability/) | Capability security tests (grant/deny/propagation, IO). |
| [effects/](../tests/effects/) | Effect system tests (annotations, handlers, purity, propagation). |
| [regions/](../tests/regions/) | Region inference tests. |
| [safety/](../tests/safety/) | Safety annotation tests. |
| [golden/](../tests/golden/) | Golden output files for intermediate compiler stages (IR, PIR L1/L2/L3, codegen, peephole) — used by `bisect.sh`. Contains 15 per-test subdirectories + `.txt` golden files. |

### Top-level `.fd` Tests (selected)
165 test files in root covering: arithmetic, control flow,
functions, strings, structs, pattern matching, enums, closures, generics,
traits, capabilities, collections (Vec/HashMap/String), Option/Result, FFI,
file I/O, async/await, borrow checking, linear types, contracts, ECS, state
machines, serde, iterators, error chaining, conditional compilation, derive
macros, dlopen, typed strings, raw pointers, select statements, Rc/Arc,
Box recursive types, vec/mat math, and more.

Expected-fail tests (24) verify that the compiler correctly *rejects* unsafe
code: `test_*_fail` variants for banking, money, borrowck, safety, contracts,
typed strings, init guarantees, constant_time, etc.

### Current Test Results (Aug 2026)
```
Interpreter:  149/149 normal
C Codegen:    149/149 matched
PE Emitter:   149/149 valid
Unit tests:   265/265 passed  (cargo test --lib)
Harness:      8/8 passed
```

> Full test inventory: see `tests/` directory listing, or run
> `./target/release/fidesc test` to see all test names and results.

---

## Scripts (scripts/)

| File | Description |
|------|-------------|
| [verify.sh](../verify.sh) | *(root)* Full verification: build + test + compare to baseline. `--save` updates baseline, `--quick` is build-only. |
| [bisect.sh](../scripts/bisect.sh) | Bisect which compiler layer (lowering, PIR, codegen, peephole) broke a specific test. |
| [gen_golden.sh](../scripts/gen_golden.sh) | Generate golden files for intermediate compiler stages. |
| [test_build.sh](../scripts/test_build.sh) | Run all non-skip tests through `fidesc build` and compare against the interpreter. |

---

## Examples (examples/)

| File | Description |
|------|-------------|
| [http_server.fd](../examples/http_server.fd) | HTTP server example (Phase 5D.3). |
| [neural_network.fd](../examples/neural_network.fd) | Neural network forward pass in pure Fides (Phase 5C.5). |
| [riscv_blink.fd](../examples/riscv_blink.fd) | Bare-metal LED blink for SiFive HiFive1 RISC-V (Phase 5B). |
| [rpi_led_blink.fd](../examples/rpi_led_blink.fd) | Bare-metal LED blinker for Raspberry Pi 3 ARM64 (Phase 5A). |

---

## Benchmarks (benches/ & benchmarks/)

| File | Description |
|------|-------------|
| [benches/bench_main.rs](../benches/bench_main.rs) | Rust performance benchmarks for the compiler (`cargo bench`). |
| [benchmarks/bench_vs_llvm.sh](../benchmarks/bench_vs_llvm.sh) | Compare Fides output vs equivalent C compiled with `gcc -O2` (Phase 4B). |
| [benchmarks/hot_reload_bench.sh](../benchmarks/hot_reload_bench.sh) | Measure hot reload latency — source change to updated result (Phase 4E). |

---

## Build Artifacts & Generated Files

These are outputs, not source. Safe to delete; regenerated by builds/tests.

| File | Description |
|------|-------------|
| `target/` | Cargo build output (debug + release binaries). |
| `tests/*.c`, `tests/*.exe`, `tests/test_async`, `tests/test_float`, `tests/test_generics` | Generated C / native executables from test runs. |
| `tests/hello.c`, `tests/strings.c`, `tests/strings.exe`, `tests/test_db.c`, `tests/test_db.exe` | Larger generated C test artifacts. |
| `tests/golden/*/` | Per-test golden output subdirectories (regenerated by `gen_golden.sh`). |
| `output.s`, `test_traits_build.asm` | Generated assembly output. |
| `_ast.txt`, `_ir.txt`, `_ir2.txt` | Dumped AST/IR snapshots (debugging artifacts). |
| `out.txt` | Empty output capture file. |
| `--opt` | Stray debug artifact in repo root (see AGENTS.md known bugs). |

---

## How to Use This Index

- **New to the codebase?** Read [README.md](../README.md) → [architecture.md](architecture.md) → [AGENTS.md](../AGENTS.md).
- **Looking for a specific feature?** Check [features.md](features.md) and [syntax-spec.md](syntax-spec.md).
- **About to edit the compiler?** Read [AGENTS.md](../AGENTS.md) — especially the high-risk file table. Run `./verify.sh` before and after.
- **A test broke?** Run `./scripts/bisect.sh <test_name>` to find the failing layer.
- **Adding a new file?** Add a row to the relevant table above so this index stays current.
