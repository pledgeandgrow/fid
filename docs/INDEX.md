# Fides Project Index

> Navigation hub for the Fides compiler. Every file in the project, grouped by
> directory, with a one-line description. Use this to find anything fast.
>
> **Stats:** 38 `.rs` source files (~65,700 LOC) · 34 stdlib modules · 149 `.fd`
> test files · 27 docs · 4 examples · 3 scripts · 2 benchmarks.

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
| [architecture.md](architecture.md) | Compiler pipeline, data flow, source file map. **Start here.** |
| [syntax-spec.md](syntax-spec.md) | Full language syntax specification (v0.5.0). |
| [features.md](features.md) | Feature status — verified against actual test runs. |
| [test.md](test.md) | Current test results across all backends. |
| [test-batches.md](test-batches.md) | Specification of all test batches. |
| [changelog.md](changelog.md) | Per-phase change history (newest first). |
| [history.md](history.md) | Lessons learned from other programming languages. |

### Design & Principles
| File | Description |
|------|-------------|
| [vision.md](vision.md) | The overarching vision — "the last language the world needs." |
| [design-principles.md](design-principles.md) | Non-negotiable rules guiding every design decision. |
| [best-practices.md](best-practices.md) | Hard-won insights from building a custom toolchain. |
| [borrowchecker.md](borrowchecker.md) | Borrow checker design document (Phases 1-6). |
| [custom-toolchain.md](custom-toolchain.md) | Why and how the toolchain is built from scratch (no LLVM/libc). |
| [full-stack-architecture.md](full-stack-architecture.md) | Every layer of the stack, built in-house. |

### Roadmaps
| File | Description |
|------|-------------|
| [roadmap.md](roadmap.md) | Milestones to "world domination" — the master roadmap. |
| [features-roadmap.md](features-roadmap.md) | New language features roadmap (track of additions). |
| [llvm-roadmap.md](llvm-roadmap.md) | LLVM parity & superset roadmap — compete, don't copy. |
| [roadmap-limitations.md](roadmap-limitations.md) | Limitations & pain points to address. |

### Strategy & Analysis
| File | Description |
|------|-------------|
| [ibm-lessons.md](ibm-lessons.md) | What Fides can learn from IBM's AIX/z/OS — vertical integration on mainstream hardware. 4 concrete features (io_uring, capability isolation, transactional memory, deterministic mode) with benefits/cons. Road to a Fides OS. |
| [direction-analysis.md](direction-analysis.md) | Are we going in the right direction? |
| [competitive-analysis.md](competitive-analysis.md) | Fides vs the world — honest competitive assessment. |
| [adoption-strategy.md](adoption-strategy.md) | How to make switching to Fides easy. |
| [industry-opportunities.md](industry-opportunities.md) | Where Fides could be groundbreaking, by industry. |
| [industry-conversion.md](industry-conversion.md) | Migration guide for projects from different sectors. |
| [innovation.md](innovation.md) | AI, robotics, hardware, IoT applications. |
| [ai-ml-features.md](ai-ml-features.md) | Mapping Fides capabilities to AI/ML problems. |
| [tool.md](tool.md) | Names and roles of Fides development tools. |
| [extra.md](extra.md) | Nice-to-have community goals (not on critical path). |

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
| [main.rs](../src/main.rs) | 1746 | CLI entry point, command dispatch, external tool invocation. |
| [token.rs](../src/token.rs) | 234 | `TokenKind` enum — all Fides keywords and operators. |
| [lexer.rs](../src/lexer.rs) | 535 | Tokenizer: source text → `Vec<Token>`. |
| [ast.rs](../src/ast.rs) | 882 | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| [parser.rs](../src/parser.rs) | 3032 | Recursive descent parser: `Vec<Token>` → `Program`. |
| [typechecker.rs](../src/typechecker.rs) | 5053 | Type validation, regions, effects, capabilities, traits. ⚠️ MEDIUM risk. |
| [borrowck.rs](../src/borrowck.rs) | 1176 | Borrow checker (Phases 1-6): `--borrowck=off\|warn\|on`, field-wise borrows, NLL. |
| [ir.rs](../src/ir.rs) | 1640 | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`. |
| [lower.rs](../src/lower.rs) | 4700 | AST → SSA IR lowering. ⚠️ MEDIUM risk — affects all backends. |
| [mono.rs](../src/mono.rs) | 1437 | Generic monomorphization — specializes generic functions per call site. |
| [comptime.rs](../src/comptime.rs) | 1007 | Compile-time eval: constant folding, reflection, f-string interpolation. |
| [compiler.rs](../src/compiler.rs) | 1623 | Pipeline orchestration, module resolution, stdlib auto-include. |
| [lsp.rs](../src/lsp.rs) | 444 | LSP server: JSON-RPC 2.0 over stdio, diagnostics, no external deps. |
| [json.rs](../src/json.rs) | 289 | Built-in JSON parser/serializer (used by LSP, no external deps). |
| [cparser.rs](../src/cparser.rs) | 313 | C header parser → generates Fides FFI bindings (`parse-header` command). |

---

## Source — Backends & Codegen (src/)

IR → target output. Multiple backends: interpreter, native ELF64, PE, C, WASM, GPU, multi-arch asm.

| File | LOC | Description |
|------|----:|-------------|
| [interp.rs](../src/interp.rs) | 3450 | IR interpreter — the reference oracle. ⚠️ LOW risk but if wrong, all backends "match" wrongly. |
| [pir.rs](../src/pir.rs) | 2024 | PIR multi-level IR (L1/L2/L3) transforms. ⚠️ HIGH risk. |
| [opt.rs](../src/opt.rs) | 7177 | PIR optimizer — 23 passes (const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, etc.). ⚠️ MEDIUM risk. |
| [peephole.rs](../src/peephole.rs) | 994 | 9-pass peephole optimizer, always on in `build`. ⚠️ CRITICAL. |
| [regalloc.rs](../src/regalloc.rs) | 1135 | Linear scan register allocator with constant rematerialization. ⚠️ CRITICAL. |
| [codegen_x64.rs](../src/codegen_x64.rs) | 1690 | PIR L3 → x86-64 assembly (used by `build` and `codegen`). ⚠️ HIGH risk. |
| [codegen_c.rs](../src/codegen_c.rs) | 3117 | C codegen backend: IR → C99 source (`ccompile` command). ⚠️ HIGH risk. |
| [codegen_arm64.rs](../src/codegen_arm64.rs) | 364 | PIR L3 → ARM64 assembly (`codegen --target arm64`). |
| [codegen_riscv.rs](../src/codegen_riscv.rs) | 360 | PIR L3 → RISC-V assembly (`codegen --target riscv`). |
| [codegen.rs](../src/codegen.rs) | 1121 | Legacy codegen dispatch (unused). |
| [x86.rs](../src/x86.rs) | 1461 | x86-64 MASM codegen: IR → assembly string (legacy `build` path). |
| [wasm.rs](../src/wasm.rs) | 1248 | WASM backend: PIR L3 → WebAssembly binary (`wasm` command). |
| [gpu.rs](../src/gpu.rs) | 601 | SPIR-V GPU backend: PIR L3 → SPIR-V binary (`gpu` command). |
| [runtime_linux.rs](../src/runtime_linux.rs) | 4383 | Hand-encoded x86-64 Linux runtime (raw syscall bytes, no libc). ⚠️ HIGH risk. |

---

## Source — Object Formats & Linking (src/)

Binary emitters and the integrated linker. Byte-level correctness is critical.

| File | LOC | Description |
|------|----:|-------------|
| [elf64.rs](../src/elf64.rs) | 2609 | ELF64 emitter: PIR L3 → executable + relocatable objects (Linux). ⚠️ HIGH risk. |
| [pe.rs](../src/pe.rs) | 6392 | Direct PE emitter: IR → x86_64 machine code → PE `.exe` (no external tools). ⚠️ HIGH risk. |
| [macho.rs](../src/macho.rs) | 377 | Mach-O emitter: object files for macOS. ⚠️ HIGH risk. |
| [coff.rs](../src/coff.rs) | 396 | COFF emitter: object files for Windows. ⚠️ HIGH risk. |
| [linker.rs](../src/linker.rs) | 737 | Integrated linker: resolves relocations across multiple object files. ⚠️ MEDIUM risk. |

---

## Source — Advanced / Experimental (src/)

Specialized subsystems: embedded, hot reload, autograd, tensor.

| File | LOC | Description |
|------|----:|-------------|
| [embedded.rs](../src/embedded.rs) | 622 | Embedded systems: `FixedBufferAllocator`, `RealTime` effect, HAL abstractions. |
| [hotreload.rs](../src/hotreload.rs) | 559 | Hot reload: native JIT (ELF64) + function-level change detection (`watch` command). |
| [autograd.rs](../src/autograd.rs) | 289 | Tape-based reverse-mode autograd for `grad` blocks. |
| [tensor.rs](../src/tensor.rs) | 462 | Tensor type and operations support. |

---

## Standard Library (stdlib/)

34 Fides source modules. `core.fd` is auto-included by the compiler; others are
imported via `use`. See `docs/features.md` for which are feature-complete.

| File | Description |
|------|-------------|
| [core.fd](../stdlib/core.fd) | Core stdlib (auto-included): `Option<T>`, `Result<T,E>`, `println`, `assert`, `assert_eq`. |
| [vec.fd](../stdlib/vec.fd) | `Vec<T>` container and iteration helpers. |
| [math.fd](../stdlib/math.fd) | Math functions: sqrt, sin, cos, floor, ceil, pow, fabs, constants. |
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

149 `.fd` test files (127 normal + 22 expected-fail) plus the Rust test harness.
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
127 normal + 22 expected-fail tests covering: arithmetic, control flow,
functions, strings, structs, pattern matching, enums, closures, generics,
traits, capabilities, collections (Vec/HashMap), Option/Result, FFI, file I/O,
async/await, borrow checking, linear types, contracts, ECS, state machines,
serde, iterators, error chaining, conditional compilation, derive macros,
dlopen, typed strings, raw pointers, and more.

Expected-fail tests (22) verify that the compiler correctly *rejects* unsafe
code: `test_*_fail` variants for banking, money, borrowck, safety, contracts,
typed strings, init guarantees, etc.

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
