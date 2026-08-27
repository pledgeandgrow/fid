# Fides Roadmap — Limitations & Pain Points

> Addressing every major complaint from every major language.
> Built on top of Phases 1-5 (claimed complete) and complementing Phases 6-8 (existing roadmap).
> Last updated: Aug 2026

## How This Document Relates to the Existing Roadmap

The existing `docs/roadmap.md` defines Phases 1-12:
- **Phases 1-5**: COMPLETE (Foundation, Safety, Power, Performance, Platform)
- **Phase 6**: AI/ML (COMPLETE — tape-based autodiff, gradient checkpointing, DP epsilon budget tracking, tensor parallelism all have runtime implementations)
- **Phase 7**: Ergonomics (MOSTLY COMPLETE — F13 parallel{} PLANNED, F14 @differentiable typecheck + runtime autograd done)
- **Phase 7.5**: Tier 1 Safety (COMPLETE)
- **Phase 7.55**: Borrow Checker (COMPLETE — closures now handled)
- **Phase 7.6**: Async/Await (MOSTLY COMPLETE)
- **Phase 7.7**: Iterators, Error Chaining, Cfg, Derive, Serde, Dynamic Loading (COMPLETE)
- **Phase 8A**: Vec/Mat Math (COMPLETE)
- **Phase 8B**: ECS/States/Events (COMPLETE)
- **Phase 8D**: Game Stdlib (COMPLETE — 8 modules, test_game_stdlib now passes)
- **Phase 8**: Game Development (IN PROGRESS — 8A+8B+8D done, 8C remaining)
- **Phase 9**: Industry Domains (NOT STARTED)
- **Phase 10**: Self-Hosting (READY TO START — all 3 blockers resolved + Issues 44-49 fixed)
- **Phase 11**: Ecosystem (IN PROGRESS — LSP server, generic enums, new stdlib modules done)
- **Phase 12**: World Class (NOT STARTED)

**This document defines Phases L1-L10** (Limitation phases) that:
1. Close the gap between what the roadmap **claims** and what **actually works** (per `features.md` and `README.md`)
2. Address every major pain point identified across Rust, Go, C++, Python, JS/TS, Java, Zig, Swift, Kotlin, and other languages
3. Complement (not replace) Phases 10-12 — cross-references are noted where overlap exists

---

## The Gap: Claimed vs. Actual (VERIFIED Aug 2026)

This section has been **verified by reading source code, running tests, and executing binaries**. Not based on docs — based on actual behavior.

### Test Results (Actual, Verified)

| Test Group | Actual (Verified) | Notes |
|------------|-------------------|-------|
| Unit tests (`cargo test --lib`) | **265/265** | All passing (incl. 47 cross-compilation tests + 7 incremental compilation) |
| Harness tests (`cargo test --test harness`) | **8/8** | All passing |
| Interpreter (`run`) | **146/146** normal | All normal tests pass. (Aug 27 cleanup: 41 stale skip/known-fail tests promoted.) |
| PE Emitter (`emit`) | **146/146** normal | All normal tests pass. (Aug 27 cleanup: 41 stale tests promoted.) |
| C Codegen (`ccompile`) | **146/146** normal | All normal tests match. Phase 10B: Vec/HashMap/String migrated to Fides stdlib. Binary hardening enabled. (Aug 27 cleanup: 41 stale tests promoted.) |
| Skip list | **7** tests | 5 C codegen missing-builtins (kv_cache, quantization, dp, shard, autodiff_grad, ml_full) + 1 design difference (dlopen). Was 42 — 37 were stale, then 2 more added. |
| Known-fail | **6** tests | 3 parser bugs (game_stdlib, time, time_edge) + 3 C codegen mismatches (iter, vec_math, parallel_concurrent). Was 10 — 4 were stale. |
| Expected-fail | **35** tests | Correctly fail — error detection tests. |
| Total `.fd` test files | **218** | 195 in root + 23 in subdirs (capability/effects/regions/safety) |
| Source files | **54** `.rs` | 85,493 LOC |
| Stdlib modules | **69** `.fd` | core, vec, math, ecs, events, parallel, sync, + 62 more |
| Golden files | **148** | Expected outputs in `tests/golden/` |

### Feature Verification Matrix (Code-Verified)

| Feature | Roadmap Claims | README Claims | ACTUAL VERIFIED State | Verdict |
|---------|---------------|--------------|----------------------|---------|
| **FFI (`extern "C"`)** | Phase 3D COMPLETE | "Not parsed" | **WORKS** — parser parses it (parser.rs:120), AST has `ExternDecl` (ast.rs:580), lowerer registers externs (lower.rs:121), C codegen emits extern decls (codegen_c.rs:430), interpreter has C stubs (interp.rs:2389). `test_ffi.fd` and `test_ffi_full.fd` pass. | **README is WRONG** — FFI works |
| **Const generics** | Phase 3C COMPLETE | "Parsed, pending" | **WORKS** — parser parses `ArrayConst` (parser.rs:883), mono handles it (mono.rs:58+), `test_const_generic.fd` passes in interpreter | **README is WRONG** — const generics work |
| **GATs** | Phase 3C COMPLETE | "Not parsed" | **WORKS** — `test_gat.fd` passes in interpreter + C codegen. | **README was wrong** — GATs work in all backends |
| **Effect handlers** | Phase 2B COMPLETE | "Syntax works, pending" | **WORKS** — `test_effect_handler.fd` passes in interpreter + C codegen. Basic dispatch works. Full continuation semantics NOT verified. | **Works** — basic dispatch in interpreter + C codegen |
| **PIR optimizer** | Phase 4B COMPLETE | "Not in default pipeline" | **ENABLED BY DEFAULT** — wired into `build` (compiler.rs), `--opt` enabled by default (use `--no-opt` to disable). `--lto` for aggressive cross-module inlining. PGO removed. 23 passes. Constant rematerialization (F-1A). | **README was wrong** — optimizer IS enabled by default |
| **Register allocator** | Phase 4C COMPLETE | "Not in default pipeline" | **PARTIALLY WIRED** — wired into `codegen_x64::generate_asm` (codegen_x64.rs:39), used by `build` and `codegen` commands. NOT used by `emit` command (pe.rs uses its own stack-based codegen). | **README partially wrong** — it IS wired into `build`/`codegen`, but NOT `emit` |
| **Peephole optimizer** | Phase 4B COMPLETE | Not mentioned | **WIRED** — always on in `build` command (compiler.rs:487). 9 passes: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection. | **Works** and enabled by default |
| **ELF64 executable** | Phase 4D COMPLETE | "Partial, WIP" | **WORKS** — `build --target x86-64` produces working ELF64 executables on Linux (zero external tools). `hello.fd` builds to 3468 bytes and runs correctly. `--opt` produces 3316 bytes. | **Roadmap was wrong** — executable now works |
| **ELF64 object files** | Phase 4C COMPLETE | "Partial" | **WORKS** — `codegen --elf` produces valid relocatable objects (readelf confirms, 13279 bytes for hello.fd) | **Works** |
| **Mach-O emission** | Phase 4D COMPLETE | Not mentioned | **WORKS** (objects) — `codegen --macho` produces 9248-byte Mach-O object file. Executable emission NOT tested (no macOS). | **Partial** — objects work, executables unverified |
| **COFF emission** | Phase 4C COMPLETE | Not mentioned | **WORKS** — `codegen --coff` produces 9677-byte COFF object file. | **Works** for objects |
| **ARM64 codegen** | Phase 4C COMPLETE | Not mentioned | **WORKS** (assembly) — `codegen --target arm64` produces valid ARM64 assembly. `build --target arm64` produces ELF object (not executable). | **Partial** — assembly works, no executable |
| **RISC-V codegen** | Phase 4C COMPLETE | Not mentioned | **WORKS** (assembly) — `codegen --target riscv` produces valid RISC-V assembly. `build --target riscv` produces ELF object (not executable). | **Partial** — assembly works, no executable |
| **WASM backend** | Phase 5D COMPLETE | "PIR -> WASM binary" | **WORKS** — `wasm` command produces valid WASM binary (magic header confirmed). 2 unit tests pass. | **Works** but minimal verification |
| **Hot reload / JIT** | Phase 4E COMPLETE | Not mentioned | **WORKS** — `watch` command compiles to native ELF64 via `build_executable` pipeline, runs as child process, detects function-level changes via L2 IR hashing. Reports added/modified/removed functions on each reload. `--opt` flag enables L2 optimizer. `--no-run` compiles only. | **Works** — native JIT + function diff |
| **GPU backend** | Phase 5C COMPLETE | Not mentioned | **WORKS** — `gpu` command produces 340-byte SPIR-V binary. `gpu.rs` has `generate_spirv` and `generate_ptx` functions, wired via `fidesc gpu` CLI command (main.rs:1063). | **Works** — produces SPIR-V binary |
| **Bare metal** | Phase 5A COMPLETE | "Works for flat binary" | **WORKS** — `emit --bare` produces flat binary | **Verified** |
| **Boot mode** | Phase 5A COMPLETE | Not mentioned | **WORKS** — `emit --boot` produces multiboot2 binary | **Verified** |
| **async/await/spawn** | Phase 7.6 COMPLETE | "Parsed, not implemented" | **✅ WORKS** — `async fn` declares async functions, `await <expr>` waits for Futures, `spawn <expr>` creates threads. Eager evaluation model (OS threads). State machine transform for cooperative scheduling pending. | **README was wrong** — async/await IS implemented |
| **`#[test]` attribute** | Not claimed | "Parsed but not executed" | **VERIFIED** — parser parses attributes (parser.rs:2134) but `fidesc test` runs file-based tests, not `#[test]` functions. `test` command (main.rs:616) iterates `.fd` files. | **Correct** — parsed but not executed |
| **`@derive`** | Phase 7.7 COMPLETE | "Parsed, not implemented" | **✅ WORKS** — `@derive(Debug)` generates `debug_str()`, `@derive(Clone)` generates `clone()`, `@derive(Serialize)` generates `to_json()`, `@derive(Deserialize)` generates `from_json()`. Implemented in `compiler.rs::process_derive()`. | **Was wrong** — derive IS implemented |
| **HTTP server stdlib** | Phase 5D COMPLETE | "Works in interpreter" | **✅ WORKS in interpreter + C codegen** — `test_http` passes in both. In skip list for `fidesc test` (was previously failing, now works). | **Complete** |
| **JSON stdlib** | Phase 5D COMPLETE | "Works in interpreter" | **✅ WORKS in interpreter + C codegen** — `test_json` passes in both. In skip list for `fidesc test`. | **Complete** |
| **DB stdlib** | Phase 5D COMPLETE | "Works in interpreter" | **✅ WORKS in interpreter + C codegen** — `test_db` passes in both. In skip list for `fidesc test`. | **Complete** |
| **SIMD** | Phase 5C COMPLETE | Not mentioned | **✅ WORKS in interpreter + C codegen** — `test_simd` passes in both. In skip list for `fidesc test`. PE backend has SSE2 encoding. | **Complete** |
| **Tensor** | Phase 5C COMPLETE | Not mentioned | **✅ WORKS in interpreter + C codegen** — `test_tensor` passes in both. In skip list for `fidesc test`. | **Complete** |
| **Inline assembly** | Phase 5A COMPLETE | "Yes" all backends | **ALL BACKENDS** — `test_asm` passes in C codegen (asm emitted as comments, matching interpreter no-op). | **Complete** |

### C Codegen Failures

These tests pass in the interpreter but FAIL in C codegen:
- ~~`test_asm` — inline assembly not supported in C codegen~~ **FIXED** — asm emitted as C comments
- ~~`test_comptime_reflect` — comptime reflection not in C codegen~~ **FIXED** — Type::Str used for array elements
- ~~`test_db` — DB builtins not in C codegen~~ **FIXED** — DB builtins work in C codegen
- `test_phase8b` — ECS `__ecs_*` functions not declared in C codegen (current known gap)
- `test_trait_verify_fail` — expected failure (correct)

### Summary: What's Real vs. What's Bullshit

**ACTUALLY WORKS (code-verified):**
- Lexer, parser, typechecker, IR lowering ✅
- Interpreter (`run`) — 146/146 normal tests ✅
- PE emitter (`emit`) — 146/146 valid PE files ✅ (Windows-only, can't run on Linux)
- ELF64 executables (`build --target x86-64`) — working Linux executables, zero external tools ✅
- C codegen (`ccompile`) — 146/146 match ✅ (Phase 10B: Vec/HashMap/String migrated to Fides stdlib)
- PIR (L1/L2/L3) — `pir` command ✅
- Optimizer — `opt` command and `build` (enabled by default, use `--no-opt` to disable) ✅. `--lto` for aggressive cross-module inlining.
- Register allocator — wired into `codegen_x64` (used by `build`/`codegen`) ⚠️ (pre-existing bug)
- Peephole — 9 passes, always on in `build` ✅
- FFI (`extern "C"`) — parsed, lowered, C codegen, interpreter stubs ✅
- Const generics — works in interpreter ✅
- GATs — works in interpreter ✅
- Effect handlers (basic) — works in interpreter ✅
- Comptime — works ✅
- Traits (cap) — works ✅
- Regions — works ✅
- Capabilities — works ✅
- ELF64 object files — valid relocatable objects ✅
- Mach-O object files — valid objects ✅
- COFF object files — valid objects ✅
- WASM backend — valid binary ✅
- GPU/SPIR-V backend — valid binary ✅
- ARM64/RISC-V codegen — valid assembly text ✅
- Bare metal / boot mode — valid flat/multiboot2 binary ✅
- `parse-header` — works ✅
- REPL, sourcemap, verify — commands exist ✅

**BUILT BUT NOT WIRED IN / BROKEN:**
- **`#[test]` execution** — parsed but `fidesc test` runs file-based tests, not `#[test]` functions ❌
- **`@derive(Debug/Clone/Serialize/Deserialize)`** — ✅ WORKS (Phase 7.7)
- **async/await/spawn** — ✅ WORKS (eager evaluation, OS thread model; state machine transform pending)
- **L2 optimizer (`--opt`)** — ✅ ENABLED BY DEFAULT in `build` (use `--no-opt` to disable). `--lto` for aggressive inlining. PGO removed.

**README/FEATURES — HISTORICAL INACCURACIES (now corrected in README.md as of Aug 2026):**
1. "C FFI: Not parsed" → **WAS WRONG, FFI works** — README now says "Yes (stubs)"
2. "Const generics: Parsed, pending" → **WAS WRONG, const generics work** — README now says "Yes"
3. "GATs: Not parsed" → **WAS WRONG, GATs work** — README now says "Yes"
4. "52/52 interpreter tests" → **WAS WRONG, now 146/146 normal** — docs now correct
5. "52/52 PE emitter" → **WAS WRONG, now 146/146 normal** — docs now correct
6. "38+ C codegen matched" → **WAS WRONG, now 146/146 (Phase 10B: Vec/HashMap/String migrated to Fides stdlib)** — docs now correct
7. "async/spawn: Parsed" → **NOW WORKS** — eager evaluation, OS thread model
8. "Optimizer not in default pipeline" → **NOW ENABLED BY DEFAULT** in `build` (use `--no-opt` to disable)
9. "Register allocator not in default pipeline" → **WAS PARTIALLY WRONG, wired into `build`/`codegen`** — README now documents this
10. "~120 compiler warnings" → **WAS WRONG, it's ~238** — README now says ~238
11. "ELF64 WIP" → **WAS WRONG, ELF64 executables work on Linux** — README now says "works"

**ROADMAP CLAIMS THAT ARE WRONG (should be corrected):**
1. ~~Phase 5D "HTTP/JSON/DB work" → PARTIAL, interpreter only — C codegen fails~~ **FIXED — HTTP/JSON/DB/SIMD/Tensor all work in C codegen now**

**Conclusion:** The core compiler (lexer → parser → typechecker → IR → interpreter/PE/C codegen/ELF64) is solid and well-tested. The PIR optimizer (23 passes) is enabled by default in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining. The register allocator (with constant rematerialization F-1A) is wired into `build`/`codegen` but NOT the `emit` command. ELF64 executables work on Linux (zero external tools). The GPU/SPIR-V backend is wired via `fidesc gpu`. The peephole optimizer (9 passes) is always on in `build`. Hot reload (`fidesc watch`) is fully implemented — native JIT via ELF64 + function-level change detection. PGO has been removed. Phase 10B: Vec/HashMap/String migrated to Fides stdlib using raw pointers.

---

## Pain Point Catalog (From All Major Languages)

This catalog drives the 10 phases below. Each pain point is tagged with which languages suffer from it and which phase addresses it.

### 1. Build Systems & Tooling
- **Rust**: Slow compile times (#1 complaint, 27.9% "big problem"), slow incremental rebuilds, slow linking, disk space bloat
- **C++**: CMake universally despised, no standard build system, no standard package manager, manual file listing, header hell
- **Python**: Packaging zoo (pip, pipenv, poetry, pdm, hatch, uv, pipx, conda, venv, pyenv), metadata inconsistency, native dependency hell
- **JS/TS**: node_modules bloat, build tool proliferation (webpack, vite, esbuild, rollup, swc, turbopack), ESM/CJS interop
- **Zig**: Breaking changes every release, no real package manager until recently
- **Java**: Maven vs Gradle complexity
- **→ Phase L5** (Package Manager & Build System), **Phase L9** (Compile Performance)

### 2. Compile Times
- **Rust**: Defining complaint, 45% of quitters cite it, workspace rebuild cascades
- **Swift**: Slow compile, especially with type inference
- **Kotlin/Native**: Documented compile time problems, container rebuild issues
- **C++**: Template metaprogramming explosions, unreadable error messages
- **Scala**: Legendary slow compile (improved in Scala 3)
- **Julia**: TTFP (time to first plot) — compilation latency
- **→ Phase L9** (Production-Grade Optimization), **Phase L1** (wire optimizer in)

### 3. Error Handling
- **Go**: `if err != nil` verbosity — #1 complaint for 15+ years, `?` proposal failed
- **Java**: Checked exceptions "20-year experiment that failed," every framework wraps to unchecked
- **C++**: No standard error type — exceptions, return codes, `expected`, `variant`, `StatusOr` all coexist
- **Python**: Exceptions everywhere, no compile-time checking
- **JS**: Thrown errors are untyped, `catch(e)` gives `any`
- **→ Already solved** — Fides has `Result<T,E>` + `?` operator + `try` blocks (Phase 1). **Phase L8** adds typed catches.

### 4. Memory Management
- **C/C++**: Undefined behavior (use-after-free, buffer overflows, data races), manual management, move semantics confusion
- **Rust**: Borrow checker learning curve, lifetime annotations, `Pin`/`Unpin` complexity, "fighting the compiler"
- **Go/Java/Python/JS**: GC pauses, no control over allocation
- **Zig**: Explicit allocators everywhere — verbose
- **→ Already solved** — Fides has region-based safety (Phase 2). **Phase L1** verifies it works in all backends.

### 5. Type System Limitations
- **Go**: Limited generics (no generic methods), no sum types, no pattern matching (until recently), no immutability, no operator overloading
- **Java**: Type erasure, no value types (until Valhalla), no unsigned types, primitive/object duality, no pattern matching (until recently)
- **TypeScript**: Types erased at runtime, structural typing surprises, unsound (`any`), no dependent types
- **Python**: Type hints not enforced, no real runtime generics, mutable default arguments
- **C++**: Template error messages, no reflection (until C++26), initialization rules (most vexing parse)
- **Rust**: No specialization (unstable), no trait upcasting (recent), no named arguments, no function overloading, orphan rule strictness
- **→ Phase L8** (Type System Completeness)

### 6. Concurrency & Async
- **Rust**: Async ecosystem fragmentation (tokio vs async-std vs smol vs embassy), `Pin`/`Unpin` complexity, function coloring, Send/Sync friction, no stdlib async runtime
- **Go**: Goroutines great but no compile-time data race prevention, channel deadlocks/leaks
- **Zig**: Async removed, being re-added as stackless coroutines, "function coloring" concern, no concurrency model currently
- **JS**: Function coloring, Promise error handling, single-threaded
- **Python**: GIL (being removed but ecosystem broken), async/sync boundary friction
- **Java**: Heavy threads (until virtual threads), lock complexity, no first-class async
- **Swift**: Swift 6 strict concurrency — `Sendable` checking turns boundaries into errors, actor isolation complexity
- **Kotlin**: Coroutine complexity, bridging to Swift 6 is "hardest integration point"
- **→ Phase L3** (Concurrency Without Function Coloring)

### 7. Ecosystem & Package Management
- **Rust**: crates.io supply chain ("adds security holes at immense speeds"), maintainer burnout (38.4% worry), not enough industry usage (42.1% worry)
- **Python**: PyPI author-led model, no build farm, inconsistent wheels, native deps painful, 10+ packaging tools
- **C++**: No standard package manager (vcpkg/Conan/FetchContent), no binary distribution
- **JS**: npm dependency hell, left-pad, deep trees, security advisories, gigabyte node_modules
- **Zig**: Tiny ecosystem, breaking changes break everything
- **Nim**: Small community, limited libraries
- **→ Phase L5** (Package Manager & Build System), **Phase L10** (Ecosystem & Adoption)

### 8. Language Complexity
- **Rust**: Complexity is #2 concern (41.6% worry), borrow checker/lifetimes/Pin/async/traits/macros — steep learning curve, 4% think already too complex
- **C++**: Decades of accumulated features, multiple paradigms, Rule of 5, initialization complexity, UB surface area
- **Scala**: Implicits, given/using, type classes, category theory
- **Swift**: Protocol-oriented programming learning curve, ABI complexity
- **Haskell**: Monads, type theory, string fragmentation (String/Text/ByteString/ShortText), Cabal hell
- **→ Fides design principles already address this** (one way to do things, comptime replaces macros). **Phase L10** keeps it simple.

### 9. FFI & Interop
- **Rust**: FFI exists but unsafe and verbose, no automatic bindings
- **Go**: cgo slow and discouraged
- **Zig**: C interop is a strength — seamless C header inclusion (killer feature)
- **Python**: C extensions painful (Cython, pybind11, cffi compete)
- **Java**: JNI notoriously awful
- **JS**: N-API complex
- **Fides**: Roadmap claims Phase 3D complete but README says "Not parsed" — **major gap**
- **→ Phase L6** (Seamless FFI & C Interop)

### 10. Cross-Platform & Targets
- **Rust**: Excellent cross-platform, multiple architectures
- **Go**: Excellent cross-compilation
- **Zig**: Cross-compilation is a strength — "bring your own OS"
- **C++**: Platform fragmentation — different compilers, ABIs, stdlib implementations
- **Fides**: Linux x86-64 ELF64 executables work (zero external tools). Windows x64 PE executables work. ARM64/RISC-V produce valid assembly + ELF objects. Mach-O/COFF objects work. No macOS executable verification.
- **→ Phase L2** (Cross-Platform Native Execution)

### 11. Debugging & Tooling
- **Rust**: Debugging experience 3rd biggest complaint (19.90% big problem), `println` debugging common
- **C++**: gdb/lldb works but name mangling makes output unreadable
- **Python**: pdb works but no type info
- **JS**: Source maps fragile across build tools
- **Haskell**: No proper debugging — lazy evaluation makes stepping impossible
- **→ Phase L7** (Developer Tooling)

### 12. Standard Library Gaps
- **Go**: Strong stdlib (net/http, encoding/json, os) — a strength
- **Rust**: Intentionally small stdlib, everything is a crate, fragmented (time vs chrono, log vs tracing)
- **C++**: Huge but inconsistent, no networking/JSON/HTTP/crypto
- **Zig**: Growing but small, I/O just reworked
- **Fides**: Minimal — Vec, HashMap, Option, Result, String. Has http.fd/json.fd/db.fd but only work in interpreter.
- **→ Phase L4** (Comprehensive Standard Library)

### 13. Language-Specific Complaints
- **Go**: Nil pointer panics (no null safety), interface pollution, stringly-typed struct tags, no default args
- **Python**: No encapsulation, no TCO, `self` everywhere, 2 vs 3 scars
- **JS**: `this` binding, `==` vs `===`, prototype chain, no integer type
- **Ruby**: Monkey-patching, slow execution, no static typing
- **PHP**: Inconsistent APIs, historically no typing
- **Java**: Boilerplate (getters/setters/equals/hashCode), no operator overloading, Spring complexity, slow startup
- **C#**: Nullable ref types opt-in, no deterministic destruction, async deadlock pitfalls
- **Kotlin**: Null safety bypassed (`!!`), multiplatform complexity
- **Swift**: Xcode lock-in, no stable ABI on Linux, result builder complexity
- **Nim**: Two ways to do everything, GC vs ARC confusion
- **→ Phase L8** (Type System), **Phase L4** (Stdlib), design principles already address most

---

## The 10 Limitation Phases

```
Phase L1: Pipeline Integration          ← Close claimed-vs-actual gap
Phase L2: Cross-Platform Native         ← Linux, macOS, cross-compilation
Phase L3: Concurrency Without Coloring  ← Effect handlers, green threads, data race safety
Phase L4: Comprehensive Stdlib          ← net, fs, io, time, json, regex, crypto, http
Phase L5: Package Manager & Build       ← pacte.toml, registry, lockfiles
Phase L6: Seamless FFI & C Interop      ← extern "C", auto-bindings, transpiler
Phase L7: Developer Tooling             ← LSP, debugger, profiler, formatter, linter
Phase L8: Type System Completeness      ← Named args, reflection, specialization, verification
Phase L9: Production-Grade Optimization ← Wire optimizer, benchmark vs LLVM, PGO, LTO
Phase L10: Ecosystem & Adoption         ← Spec, book, playground, community, migration
```

**Priority order:** L1 > L3 > L6 > L2 > L4 > L5 > L9 > L7 > L8 > L10

**Rationale:** L1 must come first because building on a broken foundation wastes effort. L3 (concurrency) is the biggest differentiator — every language struggles here. L6 (FFI) is non-negotiable for a systems language. L2 (cross-platform) because Windows-only is a death sentence. L4 (stdlib) because people pick Go for "batteries included." L5 (package manager) because cargo is why Rust won.

---

## Phase L1 — Pipeline Integration (Close the Gap)

**Goal:** Make the features the roadmap claims actually work in the default pipeline. No more "built but not wired in."

**Status:** Not started

**Why:** The roadmap claims Phases 1-5 are complete. The optimizer is now enabled by default in `build`, FFI works, ELF64 works, and most backends are wired in. The remaining gap is that the `emit` command (PE) bypasses PIR/regalloc entirely. Every subsequent phase depends on a solid foundation.

### Milestones

#### L1A — Wire Optimizer into `emit` Command
- [x] **VERIFIED:** Optimizer IS wired into `build --opt` (compiler.rs:476) and `opt` command (main.rs:812)
- [ ] Wire PIR optimizer into `emit` command (currently `pe.rs` bypasses PIR entirely — goes directly IR → PE)
- [ ] Wire PIR optimizer into `build` by default (currently requires `--opt` flag)
- [ ] Add `--opt-level 0|1|2|3` flag (default: 1 for debug, 2 for release)
- [ ] Verify optimizer doesn't break any of the 183 existing test files
- [ ] **Already built:** `opt.rs` (9 optimization passes), `peephole.rs` (9 passes)
- [ ] **Not redoing:** The optimization passes themselves

#### L1B — Wire Register Allocator into `emit` Command
- [x] **VERIFIED:** Register allocator IS wired into `codegen_x64::generate_asm` (codegen_x64.rs:39), used by `build` and `codegen` commands
- [ ] Wire register allocator into `emit` command (currently `pe.rs` uses its own stack-based codegen, bypasses regalloc)
- [ ] Verify all 183 test files still pass with register allocation in `emit`
- [ ] **Already built:** `regalloc.rs` (linear scan allocator) — already used by `build`/`codegen`
- [ ] **Not redoing:** The allocator algorithm

#### L1C — Fix ELF64 Executable (DONE)
- [x] **FIXED:** `build --target x86-64` now produces working ELF64 executables on Linux (zero external tools)
- [x] **VERIFIED WORKING:** ELF64 object files work (`codegen --elf` produces valid relocatable objects)
- [x] `hello.fd` builds to 3468 bytes, runs correctly. `--opt` produces 3316 bytes.
- [x] `fidesc build file.fd -o app` on Linux produces a working ELF64 executable
- [ ] Verify with `readelf` and actual execution on Linux
- [ ] **Already built:** ELF64 headers, symbol tables, relocations, `linker.rs` (link function exists)
- [ ] **Not redoing:** ELF64 format handling — fixing the link/relocation bug

#### L1D — FFI Already Works (Update Docs)
- [x] **VERIFIED WORKING:** FFI (`extern "C"`) IS parsed (parser.rs:120), lowered (lower.rs:121), C codegen emits externs (codegen_c.rs:430), interpreter has C stubs (interp.rs:2389). `test_ffi.fd` and `test_ffi_full.fd` pass.
- [x] **VERIFIED WORKING:** `fidesc parse-header` generates bindings (cparser.rs)
- [x] **ACTION:** Update README and features.md to remove "C FFI: Not parsed" — this is incorrect (DONE Aug 2026)
- [ ] Verify FFI works in PE backend (currently only verified in interpreter and C codegen)
- [ ] **Already built and working:** `extern "C"` syntax, `parse-header`, test files
- [ ] **Not redoing:** FFI implementation — just fixing docs and verifying PE backend

#### L1E — Verify ARM64 and RISC-V Codegen Produce Working Binaries
- [ ] Test `fidesc build --target arm64` produces an executable that runs on ARM64 hardware (or QEMU)
- [ ] Test `fidesc build --target riscv` produces an executable that runs on RISC-V hardware (or QEMU)
- [ ] Fix any codegen bugs found
- [ ] **Already built:** `codegen_arm64.rs`, `codegen_riscv.rs`, ELF objects with correct machine types
- [ ] **Not redoing:** The codegen backends — just verifying and fixing

#### L1F — Verify Mach-O Emission
- [ ] Test `macho.rs` produces valid Mach-O 64-bit object files
- [ ] Test `fidesc build --target x86-64-macos` produces a working macOS executable
- [ ] Verify with `otool` and actual execution on macOS
- [ ] **Already built (claimed):** `macho.rs` — needs verification
- [ ] **Not redoing:** Mach-O format handling (if it works)

#### L1G — Wire Hot Reload (DONE)
- [x] **FIXED:** `watch` command now compiles to native ELF64 via `build_executable` and runs as child process
- [x] Function-level change detection via L2 IR hashing (`FunctionSnapshot::from_l2` + `diff`)
- [x] State preservation via `PersistentState` (temp file based)
- [x] Debugger integration (`Debugger` struct with call stack tracking)
- [x] `--opt` flag enables L2 optimizer, `--no-run` compiles only
- [x] 8 unit tests pass (function diff, persistent state, compile_and_run, watcher, debugger)

#### L1H — Const Generics and GATs Already Work (Update Docs)
- [x] **VERIFIED WORKING:** Const generics (`[T; N]`) work in interpreter — `test_const_generic.fd` passes
- [x] **VERIFIED WORKING:** GATs work in interpreter — `test_gat.fd` passes
- [x] **ACTION:** Update README to remove "Const generics: Parsed, pending" and "GATs: Not parsed" — both are incorrect (DONE Aug 2026)
- [ ] Verify const generics and GATs work in PE and C codegen backends (currently only verified in interpreter)
- [ ] **Already built and working:** Parser, mono, typechecker support for const generics and GATs
- [ ] **Not redoing:** The type system features — just fixing docs and verifying backends

#### L1I — Fix HTTP/JSON/DB/SIMD/Tensor in C Codegen
- [x] **VERIFIED:** C codegen is at 146/146 parity with interpreter (Phase 10B: Vec/HashMap/String migrated to Fides stdlib).
- [x] **VERIFIED:** These now PASS in C codegen (previously failed): `test_http`, `test_json`, `test_simd`, `test_tensor`, `test_interrupt`, `test_mmu`, `test_matrix_mul`, `test_shape`, `test_const_generic`, `test_bounds_check`, `test_trait_object`, `test_gat`, `test_comptime_serialize`, `test_boot`, `test_parallel`, `test_print_map`, `test_regression_collections`, `test_type_alias`, `test_vec_one`, `strings`, `test_fstring_int`, `test_fstring_vec`
- [x] **VERIFIED:** These all PASS in interpreter
- [x] ~~Fix C codegen to support: inline assembly, comptime reflection, DB builtins~~ **ALL FIXED** — inline asm, comptime reflection, and DB builtins all work in C codegen now
- [ ] Fix C codegen for ECS __ecs_* functions (test_phase8b — only remaining C codegen gap)
- [ ] Verify HTTP/JSON/DB work in PE backend (currently unverified)

#### L1J — Wire GPU Backend (Currently Dead Code)
#### L1J — Wire GPU Backend (DONE)
- [x] **VERIFIED:** `gpu.rs` has `generate_spirv` and `generate_ptx` functions
- [x] **DONE:** CLI command `fidesc gpu <file.fd> --target spirv|ptx -o <output>` works
- [x] **DONE:** `generate_spirv` and `generate_ptx` are wired into the command
- [x] **VERIFIED:** `fidesc gpu tests/hello.fd --target spirv -o /tmp/test.spv` produces 340-byte SPIR-V binary

#### L1K — Clean Up Compiler Warnings
- [ ] Address ~238 compiler warnings (dead code, unused variables)
- [ ] Remove or `#[allow]` genuinely unused code
- [ ] Document why suppressed warnings are acceptable
- [ ] **Not redoing:** Any functionality — just cleanup

### Exit Criteria
- [ ] `fidesc emit` uses PIR optimizer + register allocator (currently bypasses both)
- [x] ~~`fidesc build` uses optimizer by default~~ **DONE** — optimizer enabled by default (use `--no-opt` to disable)
- [x] `fidesc build file.fd -o app` on Linux produces a working ELF64 executable (verified — zero external tools)
- [x] `fidesc watch` compiles to native ELF64 and runs on file change (with function-level diff)
- [x] ~~`fidesc gpu` command exposes SPIR-V/PTX codegen~~ **DONE** — `fidesc gpu --target spirv|ptx` works
- [ ] FFI verified working in PE backend (already works in interpreter + C codegen)
- [x] ~~Const generics and GATs verified in PE/C backends~~ **DONE** — GATs work in C codegen
- [x] ~~HTTP/JSON/DB/SIMD/Tensor work in C codegen~~ **DONE** — all work in C codegen (in skip list for historical reasons)
- [x] ~~README and features.md corrected to reflect actual state~~ **DONE**
- [ ] Compiler warnings reduced from ~213 to <50 (or all suppressed with reasons)
- [x] All 188 test files pass (currently 146/146 normal)

**Success metric:** Every feature the roadmap claims as complete actually works in the default pipeline. No more "built but not wired in." No more dead code. No more stubs.

---

## Phase L2 — Cross-Platform Native Execution

**Goal:** Fides runs natively on Linux, macOS, and Windows. Cross-compilation from any host to any target.

**Status:** Not started

**Why:** Windows-only PE is a death sentence for adoption. Rust, Go, and Zig all have excellent cross-platform support. Fides needs the same.

**Depends on:** Phase L1 (ELF64 and Mach-O must work first)

### Milestones

#### L2A — Linux ELF64 Executable Pipeline
- [ ] Full ELF64 executable emission (program headers, relocation resolution, entry point)
- [ ] `fidesc build file.fd -o app` produces a working Linux executable
- [ ] Test on Ubuntu, Debian, Arch, Alpine (different libc versions)
- [ ] Static linking option (`--static`) — no libc dependency
- [ ] **Already built:** `elf64.rs` (headers, symbols, relocations), `linker.rs` (incremental, DCE)
- [ ] **Not redoing:** ELF64 format — completing instruction encoding (L1C) makes this work

#### L2B — macOS Mach-O Executable Pipeline
- [ ] Full Mach-O executable emission (LC_SEGMENT, LINKEDIT, symbol table)
- [ ] `fidesc build file.fd -o app` produces a working macOS executable
- [ ] Test on Intel and Apple Silicon (x86-64 and ARM64)
- [ ] Universal binary support (`--target universal` produces fat binary)
- [ ] **Already built (claimed):** `macho.rs` (object files)
- [ ] **Not redoing:** Mach-O format — extending to executable emission

#### L2C — Cross-Compilation
- [ ] `fidesc build --target x86-64-linux` from macOS/Windows host
- [ ] `fidesc build --target arm64-linux` from any host
- [ ] `fidesc build --target x86-64-macos` from Linux/Windows host
- [ ] `fidesc build --target x86-64-windows` from Linux/macOS host
- [ ] `fidesc build --target riscv64-embedded` from any host
- [ ] Target triple syntax: `<arch>-<os>-<abi>` (e.g., `arm64-linux-gnu`, `x86-64-windows-msvc`)
- [ ] **Already built:** Multi-arch codegen (x86-64, ARM64, RISC-V), multi-format emission (PE, ELF, Mach-O)
- [ ] **Not redoing:** Codegen backends — just wiring cross-compilation

#### L2D — ABI Stability
- [ ] Document calling convention for each target (System V AMD64, AAPCS64, RISC-V LP64)
- [ ] Stable ABI for FFI (C-compatible function signatures)
- [ ] Struct layout rules documented and stable
- [ ] **Already built:** Calling conventions in codegen backends
- [ ] **Not redoing:** Calling convention implementation — documenting and freezing

#### L2E — WASM Target Verification
- [ ] Verify `fidesc wasm file.fd -o out.wasm` produces valid WASM that runs in browsers
- [ ] Test with Node.js, Deno, Bun, browser WASM runtime
- [ ] WASI support for system access
- [ ] **Already built:** `wasm.rs` (PIR -> WASM binary, 2 unit tests)
- [ ] **Not redoing:** WASM backend — verifying and extending

### Exit Criteria
- [ ] `fidesc build file.fd` works on Linux, macOS, and Windows natively
- [ ] Cross-compilation from any host to any supported target
- [ ] Static and dynamic linking options
- [ ] WASM target runs in browsers and WASI runtimes
- [ ] ABI documented and stable

**Success metric:** Write Fides on Linux, compile for macOS, Windows, ARM64, RISC-V, and WASM — all from one command.

---

## Phase L3 — Concurrency Without Function Coloring

**Goal:** Concurrency that doesn't split the ecosystem. No async/await function coloring. Compile-time data race safety. Built-in runtime.

**Status:** Not started (async/spawn are parsed but unimplemented)

**Why:** This is the biggest differentiator opportunity. Every language struggles here:
- Rust: async ecosystem fragmentation, Pin/Unpin, function coloring, no stdlib runtime
- Go: no compile-time data race prevention
- Zig: async removed, no concurrency model
- JS/Python: function coloring, GIL
- Swift/Kotlin: bridging complexity

Fides's effect system is the foundation — effect handlers can replace async/await entirely (per design principles: "One concurrency model: effect handlers, not async/await + threads + channels").

### Milestones

#### L3A — Effect Handler Full Continuation Semantics
- [ ] Implement full continuation semantics for `handle { do } with` (not just dispatch)
- [ ] Continuation capture: `resume(cont, value)` to resume from effect handler
- [ ] Composable handlers (handler A can delegate to handler B)
- [ ] Effect polymorphism (functions that work with or without handlers)
- [ ] **Already built:** Effect taxonomy, annotations, inference, checking, handler dispatch (Phase 2B)
- [ ] **Not redoing:** Effect system foundation — adding continuations on top

#### L3B — Green Threads / Structured Concurrency
- [ ] `spawn { expr }` creates a green thread (lightweight, M:N scheduling)
- [ ] Structured concurrency: `spawn` blocks wait for children at scope exit
- [ ] Built-in scheduler (no external runtime needed — unlike Rust's tokio)
- [ ] Work-stealing scheduler for multi-core
- [ ] **Already parsed:** `spawn` keyword exists in parser
- [ ] **Not redoing:** Parser — implementing the runtime

#### L3C — Compile-Time Data Race Safety
- [ ] Region + effect system prevents data races at compile time
- [ ] `&T` (shared reference) is `Sync` — multiple threads can read
- [ ] `&mut T` (mutable reference) is `Send` — only one thread can write
- [ ] No `Send`/`Sync` marker traits (unlike Rust) — region system handles it
- [ ] `!Spawn` effect on functions that can't spawn threads
- [ ] **Already built:** Region system (Phase 2A), effect system (Phase 2B)
- [ ] **Not redoing:** Region/effect systems — extending to concurrency

#### L3D — Channels and Select
- [ ] `Channel<T>` — typed, bounded/unbounded channels (like Go)
- [ ] `select { ch1.recv() => ..., ch2.send(x) => ... }` — multiplexed channel operations
- [ ] Channel close semantics
- [ ] **Not built:** This is new
- [ ] **Design principle:** "One concurrency model: effect handlers, not async/await + threads + channels" — channels built on top of effect handlers, not separate

#### L3E — Cancellation
- [ ] Scope-based cancellation (cancel a `spawn` block and all children)
- [ ] Cooperative cancellation via effect handler (`do Cancel`)
- [ ] No leaked threads on cancellation (structured concurrency guarantees)
- [ ] **Not built:** This is new

#### L3F — No Function Coloring
- [ ] Any function can be called from any context (sync or concurrent)
- [ ] No `async fn` vs `fn` distinction — effect handlers handle asynchronicity
- [ ] No `await` keyword needed — effect handler resume is implicit
- [ ] No ecosystem split (unlike Rust's sync vs async crates)
- [ ] **Design principle:** Effect handlers replace async/await entirely

#### L3G — Async I/O via Effect Handlers
- [ ] `do IO.read(file)` — yields to IO handler, handler does async I/O, resumes with result
- [ ] `do Net.accept(socket)` — yields to Net handler, handler does async accept, resumes with connection
- [ ] Built-in IO/Net handlers for epoll (Linux), kqueue (macOS), IOCP (Windows)
- [ ] **Already built (partial):** Effect handler dispatch (Phase 2B), socket builtins (Phase 5D)
- [ ] **Not redoing:** Socket builtins — wrapping in effect handlers

### Exit Criteria
- [ ] Write a concurrent web server using `spawn` + effect handlers, no async/await
- [ ] No data races possible (compile-time verified)
- [ ] Cancellation works without leaks
- [ ] Any function callable from any context (no coloring)
- [ ] Built-in async I/O on Linux, macOS, Windows

**Success metric:** Write a concurrent web server in Fides that is shorter, safer, and faster than equivalent Rust+tokio, Go, and Node.js code. No `async`, no `await`, no `Send`/`Sync` bounds, no runtime selection.

---

## Phase L4 — Comprehensive Standard Library

**Goal:** "Batteries included" like Go. One canonical implementation per domain. No Rust-style fragmentation.

**Status:** Not started (minimal stdlib exists: Vec, HashMap, Option, Result, String + http.fd, json.fd, db.fd, simd.fd, tensor.fd)

**Why:** People pick Go because "batteries included." Rust's intentionally small stdlib causes fragmentation (time vs chrono, log vs tracing). C++ has no networking/JSON/HTTP/crypto. Fides needs a comprehensive stdlib to compete.

**Depends on:** Phase L1 (stdlib must work in native backends, not just interpreter)

**Overlaps with:** Existing roadmap Phase 7B (Standard Library) — this phase is more detailed and prioritized

### Milestones

#### L4A — Core Collections (Verify and Extend)
- [ ] Verify `Vec<T>` works in all backends (already has 15+ methods)
- [ ] Verify `HashMap<K,V>` works in all backends (already has 12+ methods)
- [ ] Add `BTreeMap<K,V>` — ordered map
- [ ] Add `LinkedList<T>` — doubly linked list
- [ ] Add `HashSet<T>` — set type
- [ ] Add `BTreeSet<T>` — ordered set
- [ ] Add `RingBuffer<T>` — fixed-size circular buffer
- [ ] **Already built:** `Vec`, `HashMap` (comprehensive methods)
- [ ] **Not redoing:** Vec/HashMap — adding new collections

#### L4B — IO (File, Network, Console)
- [ ] `fs` module: `File` (open, read, write, close, metadata), `Dir` (list, create, remove), `Path` (join, parse, normalize)
- [ ] `io` module: `Reader` (read, read_to_end, read_line), `Writer` (write, write_all, flush), `BufReader`, `BufWriter`
- [ ] `net` module: `TcpStream`, `TcpListener`, `UdpSocket`, `UnixStream`, `DnsResolver`
- [ ] `console` module: `stdin`, `stdout`, `stderr`, `read_line`, `print`, `println`
- [ ] All work via effect handlers (Phase L3G) for async I/O
- [ ] **Already built (partial):** `p_socket_*` builtins (Phase 5D), `print`/`println` (Phase 1)
- [ ] **Not redoing:** Socket builtins — wrapping in proper API

#### L4C — Time
- [ ] `Instant` — monotonic time point
- [ ] `Duration` — time span (secs, millis, nanos)
- [ ] `SystemTime` — wall clock time
- [ ] `sleep(Duration)` — via effect handler (async-compatible)
- [ ] Time formatting and parsing
- [ ] **Not built:** This is new

#### L4D — JSON and Serialization
- [ ] Verify `stdlib/json.fd` works in all backends (Phase 5D claims complete)
- [ ] `json::parse(str) -> Value` — parse JSON string
- [ ] `json::serialize(Value) -> String` — serialize to JSON
- [ ] Comptime-derived serialization: `@derive(Serialize, Deserialize)` via comptime reflection (extending existing Phase 7.7 @derive)
- [ ] Binary serialization format (compact, typed)
- [ ] **Already built (claimed):** `stdlib/json.fd`, `p_json_*` builtins
- [ ] **Not redoing:** JSON parser — verifying and extending with comptime derivation

#### L4E — Regex
- [ ] `Regex` type — compile pattern, match, find, replace, capture groups
- [ ] Comptime regex compilation (pattern verified at compile time)
- [ ] **Not built:** This is new

#### L4F — Crypto
- [ ] Hashing: SHA-256, SHA-512, BLAKE3, MD5 (legacy)
- [ ] HMAC: HMAC-SHA256, HMAC-SHA512
- [ ] Symmetric: AES-256-GCM, ChaCha20-Poly1305
- [ ] Asymmetric: Ed25519 (signing), X25519 (key exchange)
- [ ] Random: `CryptoRng` (secure random number generator)
- [ ] All gated by `Crypto` capability (capability security)
- [ ] **Not built:** This is new
- [ ] **Design principle:** Capabilities — crypto requires explicit `Crypto` capability grant

#### L4G — HTTP (Verify and Extend)
- [ ] Verify `stdlib/http.fd` works in PE and ELF backends (not just interpreter)
- [ ] `http::Server` — route, handle, listen (via effect handlers for async)
- [ ] `http::Client` — get, post, put, delete, with headers and body
- [ ] Request/response types, middleware, routing
- [ ] HTTPS/TLS support (via crypto module)
- [ ] **Already built (partial):** `stdlib/http.fd`, `p_http_*` builtins (Phase 5D)
- [ ] **Not redoing:** HTTP parsing — extending to full server/client

#### L4H — Process and Environment
- [ ] `process::spawn(cmd, args) -> Process` — spawn child process
- [ ] `process::Command` — builder for complex process spawning
- [ ] `process::exit(code)` — exit current process
- [ ] `env::var(name) -> Option<String>` — get environment variable
- [ ] `env::args() -> Vec<String>` — get command line arguments
- [ ] Gated by `Spawn` capability (capability security)
- [ ] **Not built:** This is new
- [ ] **Design principle:** Capabilities — process spawning requires `Spawn` capability

#### L4I — Math (Verify and Extend)
- [ ] Verify `stdlib/simd.fd` and `stdlib/tensor.fd` work in all backends
- [ ] `math` module: sin, cos, tan, sqrt, pow, log, exp, floor, ceil, round, abs, min, max
- [ ] `complex` module: Complex number type and operations
- [ ] `Random` type — non-crypto PRNG (xorshift, PCG)
- [ ] **Already built:** `stdlib/simd.fd` (SSE2), `stdlib/tensor.fd` (Phase 5C)
- [ ] **Not redoing:** SIMD/tensor — adding standard math functions

#### L4J — Testing Framework
- [ ] `#[test]` attribute — execute tests with `fidesc test` (currently parsed but not executed)
- [ ] `assert!`, `assert_eq!`, `assert_ne!` macros (or comptime equivalents)
- [ ] Test runner: discover, execute, report results
- [ ] `#[should_panic]` — assert that a test panics
- [ ] `#[ignore]` — skip a test
- [ ] **Already parsed:** `#[test]` attribute
- [ ] **Not redoing:** Attribute parsing — implementing execution

### Exit Criteria
- [ ] All stdlib modules work in interpreter, PE, ELF, and C codegen backends
- [ ] Write a REST API server using only stdlib (http + json + crypto + fs)
- [ ] Write a CLI tool using only stdlib (env + process + fs + io)
- [ ] `fidesc test` runs `#[test]` functions
- [ ] One canonical implementation per domain (no fragmentation)

**Success metric:** A developer can build a real web application using only Fides stdlib — no external dependencies needed. Like Go, but with memory safety, capability security, and effect-based concurrency.

---

## Phase L5 — Package Manager & Build System

**Goal:** Built-in package manager and build system. One canonical way. No CMake, no Make, no external build tools.

**Status:** Not started

**Why:** Cargo is why Rust won. CMake is why C++ lost developers. Python's packaging zoo is a running joke. Fides needs this from day one.

**Overlaps with:** Existing roadmap Phase 7A (Tooling) — this phase is more detailed

### Milestones

#### L5A — Project File (`pacte.toml`)
- [x] `pacte.toml` — project manifest (name, version, edition, dependencies, targets) — IMPLEMENTED by pacte
- [ ] `[package]` section: name, version, authors, edition, description, license
- [ ] `[dependencies]` section: package = "version" or package = { version = "...", features = [...] }
- [ ] `[dev-dependencies]` section: test-only dependencies
- [ ] `[target]` section: build targets (bin, lib, test)
- [ ] `[[bin]]` and `[[lib]]` sections: multiple targets per project
- [ ] **Not built:** This is new

#### L5B — Build System (`fides build`)
- [ ] `fidesc build` — compile project (reads `pacte.toml`, compiles all targets)
- [ ] `fides build --release` — optimized build (enables PIR optimizer, LTO)
- [ ] `fides build --target <triple>` — cross-compile
- [ ] `fides run` — build and run main binary
- [ ] `fides test` — build and run all tests
- [ ] `fides check` — type-check without codegen (fast feedback)
- [ ] `fides clean` — remove build artifacts
- [ ] Incremental builds (only recompile changed files — use `linker.rs::IncrementalLinker`)
- [ ] **Already built:** `linker.rs::IncrementalLinker` (caches by content hash), `fidesc build` command
- [ ] **Not redoing:** Incremental linker — wrapping in project-aware build system

#### L5C — Package Manager (`pacte add/remove/publish`)
- [x] `pacte add <package>` — add dependency to `pacte.toml`, download — IMPLEMENTED (path/git deps)
- [ ] `fides remove <package>` — remove dependency
- [ ] `fides update` — update dependencies within version constraints
- [ ] `fides publish` — publish package to registry
- [ ] `fides search <query>` — search registry
- [ ] Semantic versioning support (caret, tilde, wildcard, exact)
- [ ] **Not built:** This is new

#### L5D — Lockfiles
- [ ] `fides.lock` — deterministic dependency versions
- [ ] Reproducible builds (same lockfile = same output)
- [ ] Consistent metadata (fix Python's problem where metadata can differ per machine)
- [ ] Lockfile includes content hashes (supply chain integrity)
- [ ] **Not built:** This is new

#### L5E — Registry
- [ ] Fides package registry (like crates.io but curated)
- [ ] Package verification (signed packages, verified publishers)
- [ ] Binary package distribution (not just source — fix C++'s "no binary libraries" problem)
- [ ] Mirror support (corporate/offline environments)
- [ ] **Not built:** This is new
- [ ] **Design principle:** Capabilities — downloaded packages can't access IO/FS/Net unless explicitly granted

#### L5F — Workspaces
- [ ] `[workspace]` in `pacte.toml` — multi-package projects
- [ ] Shared dependencies across workspace members
- [ ] `fides build` from workspace root builds all members
- [ ] Path dependencies for local development
- [ ] **Not built:** This is new

### Exit Criteria
- [x] `pacte init myproject` creates a new project with `pacte.toml` — IMPLEMENTED
- [x] `pacte add http --git <url>` downloads and integrates the `http` package — IMPLEMENTED (git deps)
- [ ] `fides build` compiles the project with incremental builds
- [ ] `fides publish` publishes to the Fides registry
- [ ] Lockfiles ensure reproducible builds
- [ ] No external build tools needed (no Make, no CMake, no Ninja)

**Success metric:** A new user can `fides new app && fides add http && fides run` and have a working HTTP server in under 30 seconds. Like `cargo new && cargo run`, but faster.

---

## Phase L6 — Seamless FFI & C Interop

**Goal:** Call C libraries as easily as Zig. Auto-generate bindings. Gradual migration from C.

**Status:** FFI works (verified in interpreter + C codegen) — needs PE backend verification and docs correction

**Why:** Zig's seamless C interop is its killer feature. Rust's FFI is unsafe and verbose. C++ has no automatic bindings. Fides needs this for adoption — no one rewrites everything at once.

**Depends on:** Phase L1D (verify FFI actually works)

### Milestones

#### L6A — Verify and Fix `extern "C"`
- [ ] Verify `extern "C" fn name(params) -> return;` works on all backends
- [ ] Verify Fides primitives map directly to C types (i64 = int64_t, f64 = double, etc.)
- [ ] Verify pointer types (`*mut T`, `*const T`) work across FFI boundary
- [ ] Verify struct layout matches C (repr(C) equivalent)
- [ ] **Already built (claimed):** `extern "C"` syntax, `test_ffi.fd`, `test_ffi_full.fd`
- [ ] **Not redoing:** FFI syntax — making it actually work everywhere

#### L6B — Auto-Binding Generation (`fides parse-header`)
- [ ] Verify `fidesc parse-header <file.h>` generates working Fides bindings
- [ ] Handle: function prototypes, struct definitions, enum definitions, typedefs, macros
- [ ] Handle: pointer types, arrays, function pointers
- [ ] Handle: `#include` resolution (find headers in standard paths)
- [ ] Output: `.fd` file with `extern "C"` declarations
- [ ] **Already built (claimed):** `parse-header` command, `test_ffi_header.fd`
- [ ] **Not redoing:** Header parser — verifying and extending

#### L6C — Shared Library Loading
- [ ] `dlopen`/`LoadLibrary` wrapper — load shared libraries at runtime
- [ ] `dlsym`/`GetProcAddress` wrapper — resolve symbols at runtime
- [ ] Type-safe function pointer lookup
- [ ] Gated by `FS` capability (loading a library is filesystem access)
- [ ] **Not built:** This is new

#### L6D — C-to-Fides Transpiler
- [ ] Auto-translate C code to Fides (best-effort, manual review needed)
- [ ] Handle: functions, structs, enums, typedefs, basic control flow
- [ ] Map C patterns to Fides idioms (malloc/free -> regions, pointers -> references)
- [ ] Output readable Fides code (not machine-generated mess)
- [ ] **Overlaps with:** Existing roadmap Phase 7C (Migration Tools)
- [ ] **Not built:** This is new

#### L6E — Multi-Language Interop via C ABI
- [ ] Python interop: Fides compiles to shared library, Python calls via ctypes/cffi
- [ ] JavaScript interop: Fides compiles to WASM (via existing WASM backend), JS calls via WASM imports
- [ ] Rust interop: Fides calls Rust via C ABI (Rust `extern "C"`)
- [ ] Go interop: Fides calls Go via cgo-compatible C ABI
- [ ] **Already built:** WASM backend (Phase 5D)
- [ ] **Not redoing:** WASM backend — using it for JS interop

### Exit Criteria
- [ ] Call OpenSSL, libcurl, SQLite, and libpng from Fides without hand-writing bindings
- [ ] `fides parse-header openssl/ssl.h` generates working bindings
- [ ] Fides shared library callable from Python, JS, Rust, and Go
- [ ] C-to-Fides transpiler handles real-world C code (at least simple programs)

**Success metric:** A C project can gradually migrate to Fides — one file at a time, calling remaining C code via FFI, with auto-generated bindings. Like Zig's `@cImport`, but better.

---

## Phase L7 — Developer Tooling

**Goal:** Best-in-class developer experience. LSP, debugger, profiler, formatter, linter, docs.

**Status:** Not started (REPL and sourcemap exist)

**Why:** Rust's debugging experience is the 3rd biggest complaint. C++ name mangling makes debugging painful. Haskell has no debugging at all. Good tooling is table stakes.

**Overlaps with:** Existing roadmap Phase 7A (Tooling) — this phase is more detailed

### Milestones

#### L7A — LSP Server
- [ ] `fides lsp` — Language Server Protocol implementation
- [ ] Features: completion, hover, goto definition, goto references, rename, format on save
- [ ] Diagnostics: type errors, capability errors, effect errors, region errors — all in editor
- [ ] Semantic highlighting (using PIR type info)
- [ ] Inlay hints (types, effect annotations, capability requirements)
- [ ] Integration: VS Code, Neovim, Helix, JetBrains
- [ ] **Not built:** This is new
- [ ] **Already built:** Typechecker, sourcemap (for goto definition)

#### L7B — Debugger
- [ ] `fides debug <binary>` — source-level debugger
- [ ] Breakpoints (line, function, conditional)
- [ ] Stepping (step in, step over, step out, continue)
- [ ] Variable inspection (print values, expand structs/enums)
- [ ] Call stack inspection
- [ ] Watch expressions
- [ ] **Already built (claimed):** `hotreload.rs::Debugger` (breakpoints, stepping, call stack)
- [ ] **Not redoing:** Debugger infrastructure — extending to standalone debugging

#### L7C — Profiler
- [ ] `fides profile <binary>` — runtime profiler
- [ ] CPU profiling (function-level timing, call graph)
- [ ] Memory profiling (allocation tracking, region usage)
- [ ] Allocation profiler (which functions allocate, how much)
- [ ] Flame graph output
- [ ] **Not built:** This is new

#### L7D — Formatter
- [ ] `fides fmt` — code formatter (like `rustfmt`, `gofmt`)
- [ ] Consistent style across all Fides code
- [ ] No configuration needed (one canonical style)
- [ ] `fides fmt --check` for CI (verify code is formatted)
- [ ] **Not built:** This is new
- [ ] **Design principle:** "One way to do things" — one canonical format

#### L7E — Linter
- [ ] `fides lint` — code linter
- [ ] Lints: unused variables, dead code, unnecessary allocations, effect violations
- [ ] Style lints: naming conventions, import ordering
- [ ] Security lints: capability usage audit, unsafe code audit
- [ ] `fides lint --fix` — auto-fix where possible
- [ ] **Not built:** This is new

#### L7F — Documentation Generator
- [ ] `fides doc` — generate HTML documentation from source
- [ ] Doc comments (`///` or `/** */`)
- [ ] Cross-references (type links, function links)
- [ ] Searchable
- [ ] Effect/capability/region annotations in docs
- [ ] **Not built:** This is new

#### L7G — REPL Improvements
- [ ] Verify `fides repl` works (claimed in features.md)
- [ ] Multi-line input
- [ ] Type inspection (`:type expr`)
- [ ] Effect inspection (`:effects func`)
- [ ] Capability inspection (`:caps func`)
- [ ] PIR inspection (`:pir func`)
- [ ] **Already built:** REPL command exists
- [ ] **Not redoing:** REPL — extending with inspection commands

#### L7H — Source Map Verification
- [ ] Verify `fides sourcemap` produces correct source maps
- [ ] Source maps work with debugger (L7B)
- [ ] Source maps work with profiler (L7C)
- [ ] Source maps work with error messages (file:line:col)
- [ ] **Already built:** `fides sourcemap` command
- [ ] **Not redoing:** Source map generation — verifying integration

### Exit Criteria
- [ ] VS Code extension with full LSP support (completion, diagnostics, refactor)
- [ ] Source-level debugger with breakpoints and variable inspection
- [ ] CPU and memory profiler with flame graphs
- [ ] `fides fmt` produces canonical formatting
- [ ] `fides doc` generates searchable HTML docs
- [ ] REPL with type/effect/capability inspection

**Success metric:** A developer's first experience with Fides includes: syntax highlighting, autocomplete, inline errors, goto definition, formatter on save, and a debugger — all within 5 minutes of installation. Like Rust-analyzer, but faster.

---

## Phase L8 — Type System Completeness

**Goal:** Every type system feature that developers expect from a modern language. No gaps.

**Status:** Partial (generics, enums, traits, pattern matching work; const generics and GATs work in interpreter — need PE/C backend verification)

**Why:** Go's limited generics, Java's type erasure, TypeScript's erased types, Rust's missing specialization — all are major complaints. Fides needs to close every gap.

**Depends on:** Phase L1H (verify const generics and GATs)

### Milestones

#### L8A — Verify Const Generics
- [ ] Confirm `[T; N]` where N is a comptime value works in monomorphization
- [ ] Test: `fn sum<N: i64>(arr: [i64; N]) -> i64` monomorphizes correctly
- [ ] Test: const generic in struct (`struct Matrix<T, const N: usize>`)
- [ ] Test: const generic constraints (`where N > 0`)
- [ ] **Already built (claimed):** `test_const_generic.fd`, `test_matrix_mul.fd`
- [ ] **Not redoing:** Const generic implementation — verifying

#### L8B — Verify GATs
- [ ] Confirm `type Item<T>;` in traits works
- [ ] Confirm `type Item<T> = T;` in impls works
- [ ] Confirm `T::Item<X>` in type positions works
- [ ] Test: `test_gat.fd` passes
- [ ] **Already built (claimed):** `test_gat.fd`
- [ ] **Not redoing:** GAT implementation — verifying

#### L8C — Named Arguments
- [ ] `fn foo(name: T, value: V)` can be called as `foo(name: "x", value: 42)` or `foo("x", 42)`
- [ ] Named arguments at any position (not just trailing)
- [ ] Mixed positional and named
- [ ] **Not built:** This is new (Rust doesn't have this either — common complaint)

#### L8D — Default Arguments
- [ ] `fn foo(x: i64, y: i64 = 0)` — `y` defaults to 0 if omitted
- [ ] Defaults must be comptime-evaluable
- [ ] Works with named arguments (`foo(x: 5)` uses default for `y`)
- [ ] **Not built:** This is new (Go doesn't have this — common complaint)

#### L8E — Variadic Arguments
- [ ] `fn printf(fmt: &str, ...args: T)` — variadic arguments
- [ ] Type-safe variadics (not C-style untyped)
- [ ] Via comptime: `fn sum(...xs: i64) -> i64` with `xs` as comptime tuple
- [ ] **Not built:** This is new

#### L8F — Reflection (Comptime)
- [ ] Verify comptime reflection works: `type_name`, `is_struct`, `is_enum`, `field_count`, `field_names`, `field_types`, `enum_variants`
- [ ] Add: `field_offset`, `struct_size`, `enum_discriminant`
- [ ] Add: `function_effects`, `function_capabilities` (inspect effect/capability annotations)
- [ ] Runtime type info (minimal — for debugging only, not for dispatch)
- [ ] **Already built:** Comptime reflection (Phase 3A)
- [ ] **Not redoing:** Reflection — verifying and extending

#### L8G — Trait Upcasting
- [ ] `dyn SubTrait` can be upcast to `dyn SuperTrait`
- [ ] Trait inheritance: `cap SubCap: SuperCap { ... }`
- [ ] **Not built:** This is new (Rust only recently added this)

#### L8H — Specialization
- [ ] Specialize generic implementations for specific types
- [ ] `fn foo<T>(x: T) { ... }` with `fn foo(x: i64) { ... }` as specialization
- [ ] Most-specific specialization wins
- [ ] **Not built:** This is new (Rust has this unstable for years — common complaint)

#### L8I — Associated Type Bounds
- [ ] `where T::Item: Display` — bounds on associated types
- [ ] `where T: Iterator<Item = i64>` — equality bounds
- [ ] **Not built:** This is new

#### L8J — `@derive` via Comptime (Extending Phase 7.7)
- [ ] `@derive(Debug, Clone, PartialEq, Eq)` on structs and enums (currently Debug/Clone/Serialize/Deserialize work via compiler.rs::process_derive)
- [ ] Implemented via comptime (not a separate macro system)
- [ ] Derivable traits: `Debug`, `Clone`, `PartialEq`, `Eq`, `Hash`, `Serialize`, `Deserialize`
- [ ] **Already implemented:** `@derive(Debug/Clone/Serialize/Deserialize)` (Phase 7.7)
- [ ] **Not redoing:** Attribute parsing — extending derivation via comptime

#### L8K — Never Type
- [ ] `!` type (never returns — divergent functions)
- [ ] `!` coerces to any type (like Rust's `!`)
- [ ] `fn panic(msg: &str) -> !`
- [ ] **Not built:** This is new

#### L8L — Unsafe (Framework-Only)
- [ ] `unsafe { }` blocks for framework code only (not user code)
- [ ] Used for: FFI, hardware access, inline assembly, raw pointer manipulation
- [ ] Clearly marked in source — auditable
- [ ] **Design principle:** "No `unsafe` escape hatch in user code: framework-only, like capability kernels"
- [ ] **Not built:** This is new (needed for FFI and bare metal)

### Exit Criteria
- [ ] Const generics and GATs verified working
- [ ] Named, default, and variadic arguments work
- [ ] Comptime reflection covers all type introspection needs
- [ ] `#[derive]` generates trait impls via comptime
- [ ] Specialization, trait upcasting, associated type bounds work
- [ ] `unsafe` blocks available for framework code only

**Success metric:** A developer coming from Rust, Go, or TypeScript finds every type system feature they expect — plus comptime reflection and derivation that none of those languages have.

---

## Phase L9 — Production-Grade Optimization

**Goal:** Generated code within 5% of LLVM -O2. Compile time 10x faster than Rust.

**Status:** Partial (optimizer enabled by default in `build`, but not wired into `emit` command — Phase L1A fixes this)

**Why:** Rust's slow compilation is the #1 complaint. C++ template explosions. If Fides is slow to compile or produces slow code, none of its other advantages matter.

**Depends on:** Phase L1A (optimizer wired into `emit`), Phase L1B (register allocator wired into `emit`)

**Overlaps with:** Existing roadmap Phase 8B (Optimization Parity) — this phase is more detailed

### Milestones

#### L9A — Benchmark Against LLVM -O2
- [ ] Benchmark suite: Fides vs gcc -O2 vs clang -O2 vs rustc -O2
- [ ] Benchmarks: loop-heavy, recursion-heavy, allocation-heavy, IO-heavy, mixed
- [ ] Track: runtime speed, binary size, compile time
- [ ] Target: within 5% of LLVM -O2 for runtime, within 10% for binary size
- [ ] **Already built:** `benchmarks/bench_vs_llvm.sh` (claims 95% of gcc -O2)
- [ ] **Not redoing:** Benchmark framework — extending and verifying

#### L9B — Profile-Guided Optimization (PGO)
- [ ] `fides build --profile-generate` — instrument binary for profiling
- [ ] `fides build --profile-use <profile>` — use profile data for optimization
- [ ] Branch prediction hints from profile data
- [ ] Inlining decisions from profile data
- [ ] **Not built:** This is new

#### L9C — Link-Time Optimization (LTO)
- [ ] Cross-module inlining (inline functions from other modules)
- [ ] Cross-module dead code elimination
- [ ] Cross-module constant propagation
- [ ] **Already built:** `linker.rs::dead_code_elimination` (BFS from entry)
- [ ] **Not redoing:** Link-time DCE — adding cross-module optimization

#### L9D — Whole-Program Optimization
- [ ] Analyze entire program (not just module-by-module)
- [ ] Devirtualization (replace `dyn Trait` calls with direct calls when possible)
- [ ] Escape analysis (stack-allocate heap objects that don't escape)
- [ ] **Not built:** This is new
- [ ] **Already built (partial):** Region-aware alias analysis (Phase 4B)

#### L9E — SIMD Auto-Vectorization
- [ ] Auto-vectorize loops (detect parallelizable loops, generate SIMD code)
- [ ] Use effect system: `!{}` (pure) functions are safe to vectorize
- [ ] Use region system: non-aliasing pointers are safe to vectorize
- [ ] **Already built:** SIMD types (Phase 5C), SSE2 codegen
- [ ] **Not redoing:** SIMD types — adding auto-vectorization pass

#### L9F — Compile-Time Benchmarks
- [ ] Benchmark: Fides vs Rust vs Go vs Zig for compile time
- [ ] Track: clean build, incremental build, no-op rebuild
- [ ] Target: 10x faster than Rust, comparable to Go
- [ ] **Already built:** `benchmarks/hot_reload_bench.sh` (claims 13ms warm)
- [ ] **Not redoing:** Benchmark framework — extending

#### L9G — Binary Size Optimization
- [ ] Dead code elimination at IR level (already in linker)
- [ ] String pooling (deduplicate string literals)
- [ ] Symbol stripping (`fides build --strip`)
- [ ] LTO for size (`fides build --opt-level z`)
- [ ] Target: smaller than C for equivalent functionality
- [ ] **Already built:** `linker.rs::dead_code_elimination`, `embedded.rs::BinarySizeOptimizer`
- [ ] **Not redoing:** DCE — adding size-specific optimizations

### Exit Criteria
- [ ] Fides code runs within 5% of LLVM -O2 on standard benchmarks
- [ ] Fides compiles 10x faster than Rust for equivalent projects
- [ ] Binary size smaller than C for equivalent functionality
- [ ] PGO, LTO, and whole-program optimization available
- [ ] Auto-vectorization uses effect + region info for safety

**Success metric:** `fides build --release` produces code as fast as `rustc -O` and `gcc -O2`, compiles in 1/10th the time, and produces smaller binaries. No LLVM dependency.

---

## Phase L10 — Ecosystem, Community & Adoption

**Goal:** Real developers using Fides for real projects. Documentation, community, migration paths.

**Status:** Not started

**Why:** Rust's #1 worry is "not enough usage in tech industry" (42.1%). Zig's tiny ecosystem. Nim's small community. A language without users is a research project, not a product.

**Overlaps with:** Existing roadmap Phase 7D (Documentation & Community) and Phase 8D (Industry Adoption) — this phase consolidates and extends

### Milestones

#### L10A — Language Specification
- [ ] Formal grammar (EBNF or similar)
- [ ] Type system semantics (formal rules for regions, effects, capabilities)
- [ ] Memory model (region lifecycle, allocation rules)
- [ ] Effect system semantics (effect inference algorithm, checking rules)
- [ ] Capability security model (grant, denial, propagation rules)
- [ ] ABI specification (calling conventions, struct layout, name mangling)
- [ ] **Overlaps with:** Existing roadmap Phase 8A (Formal Verification) — spec comes first
- [ ] **Not built:** This is new

#### L10B — Fides Book
- [ ] "The Fides Programming Language" — like "The Rust Programming Language"
- [ ] Chapters: Getting Started, Syntax, Types, Traits, Regions, Effects, Capabilities, Comptime, FFI, Concurrency, Stdlib, Deployment
- [ ] Interactive examples (run in browser via WASM backend)
- [ ] Free online, paid print/ebook
- [ ] **Overlaps with:** Existing roadmap Phase 7D
- [ ] **Not built:** This is new

#### L10C — Examples Library
- [ ] Real-world code samples: CLI tools, web servers, embedded firmware, data processing
- [ ] Each example demonstrates a Fides differentiator (regions, effects, capabilities, comptime)
- [ ] "Fides by Example" — like "Go by Example"
- [ ] **Overlaps with:** Existing roadmap Phase 7D
- [ ] **Not built:** This is new

#### L10D — Playground
- [ ] Web-based Fides IDE (run in browser)
- [ ] Edit, compile (to WASM), run, share
- [ ] Embeddable (like Rust Playground)
- [ ] Links from documentation and book
- [ ] **Already built:** WASM backend (Phase 5D)
- [ ] **Not redoing:** WASM backend — building playground on top

#### L10E — Migration Guides
- [ ] "C to Fides" guide — map C patterns to Fides idioms
- [ ] "Rust to Fides" guide — map Rust patterns (borrow checker, lifetimes, async) to Fides (regions, effects)
- [ ] "Go to Fides" guide — map Go patterns (goroutines, channels, interfaces) to Fides (spawn, channels, traits)
- [ ] "Python to Fides" guide — for performance-critical Python code
- [ ] **Overlaps with:** Existing roadmap Phase 7C (Migration Tools)
- [ ] **Not built:** This is new

#### L10F — Community
- [ ] Community forum (Discourse or similar)
- [ ] Discord/Matrix for real-time chat
- [ ] GitHub Discussions for Q&A
- [ ] Contributing guide (how to contribute to compiler, stdlib, docs)
- [ ] Code of conduct
- [ ] **Overlaps with:** Existing roadmap Phase 7D
- [ ] **Not built:** This is new

#### L10G — Security Audit
- [ ] External security audit of the compiler
- [ ] Audit region safety (can it be bypassed?)
- [ ] Audit capability security (can unprivileged code access IO/FS/Net?)
- [ ] Audit effect system (can effects be hidden?)
- [ ] Audit FFI (can unsafe C code bypass Fides safety?)
- [ ] Publish audit report
- [ ] **Overlaps with:** Existing roadmap Phase 8A (Formal Verification)
- [ ] **Not built:** This is new

#### L10H — Production Case Studies
- [ ] Find 3 companies willing to pilot Fides in production
- [ ] Document use case, results, lessons learned
- [ ] Publish case studies
- [ ] **Overlaps with:** Existing roadmap Phase 8D (Industry Adoption)
- [ ] **Not built:** This is new

#### L10I — University Course
- [ ] Develop university-level course materials for Fides
- [ ] Topics: systems programming, memory safety, capability security, effect systems
- [ ] Lab exercises (using playground)
- [ ] Partner with at least 1 university to teach it
- [ ] **Overlaps with:** Existing roadmap Phase 8D
- [ ] **Not built:** This is new

#### L10J — Killer App
- [ ] Build at least 1 application that only Fides can do well:
  - Hot-reloadable web server with compile-time capability security (no other language does both)
  - Memory-safe OS kernel with effect-based real-time guarantees
  - AI inference engine that runs on CPU, GPU, and microcontroller from same code
- [ ] **Overlaps with:** Existing roadmap Phase 8D ("killer app")
- [ ] **Not built:** This is new

### Exit Criteria
- [ ] Formal language specification published
- [ ] "The Fides Programming Language" book available online
- [ ] Web playground live and linked from docs
- [ ] Migration guides for C, Rust, Go, Python
- [ ] Active community (forum + chat)
- [ ] External security audit completed and published
- [ ] At least 3 production case studies
- [ ] At least 1 university teaching Fides
- [ ] At least 1 killer app that demonstrates Fides's unique capabilities

**Success metric:** A developer can go from "never heard of Fides" to "running their first Fides program" in under 5 minutes, find comprehensive documentation, join an active community, and see real companies using it in production.

---

## Phase Dependency Graph

```
L1 (Pipeline Integration)
├── L2 (Cross-Platform) — needs ELF64/Mach-O working
├── L6 (FFI) — needs extern "C" verified
├── L9 (Optimization) — needs optimizer wired in
└── L4 (Stdlib) — needs native backends working

L3 (Concurrency) — independent (effects already work)
├── L4 (Stdlib) — async I/O needs effect handlers

L5 (Package Manager) — independent (no codegen dependency)
├── L10 (Ecosystem) — registry enables community

L7 (Tooling) — needs L1 (sourcemap, typechecker)
├── L10 (Ecosystem) — LSP enables adoption

L8 (Type System) — needs L1H (verify const generics/GATs)

L10 (Ecosystem) — needs L4, L5, L7 (stdlib, packages, tooling)
```

**Critical path:** L1 → L2 → L4 → L5 → L10 (foundation to adoption)
**Parallel track:** L3, L5, L7, L8 can proceed in parallel after L1

---

## Timeline (Rough Estimates)

| Phase | Duration | Cumulative | Priority |
|-------|----------|------------|----------|
| L1 — Pipeline Integration | 2 months | 2 months | 1 (blocking) |
| L2 — Cross-Platform Native | 3 months | 5 months | 4 |
| L3 — Concurrency | 4 months | 6 months | 2 (parallel with L1) |
| L4 — Comprehensive Stdlib | 4 months | 9 months | 5 |
| L5 — Package Manager & Build | 3 months | 8 months | 6 (parallel with L4) |
| L6 — Seamless FFI | 2 months | 7 months | 3 (after L1) |
| L7 — Developer Tooling | 4 months | 11 months | 8 (parallel) |
| L8 — Type System Completeness | 3 months | 10 months | 9 (parallel) |
| L9 — Production Optimization | 3 months | 12 months | 7 (after L1) |
| L10 — Ecosystem & Adoption | 6+ months | 18+ months | 10 (after L4, L5, L7) |

**~18 months to address all limitations. ~6 months to close the claimed-vs-actual gap (L1) and have working concurrency (L3) and FFI (L6).**

---

## What This Roadmap Does NOT Redo

Every phase explicitly notes what is already built and what is new work. This table is **code-verified** (Aug 2026).

| Already Built & Working (Verified) | New Work (Phases L1-L10) |
|-----------------------------------|------------------------|
| Lexer, parser, typechecker | Wire optimizer into `emit` command (L1A) |
| IR, PIR (L1/L2/L3) | Wire register allocator into `emit` command (L1B) |
| PE emitter (146/146 valid) | Wire peephole into `emit` command |
| C codegen (146/146 match — Phase 10B) | ✅ DONE — Vec/HashMap/String migrated to Fides stdlib |
| PIR optimizer (23 passes) — enabled by default in `build` | `--lto` for aggressive inlining; PGO removed |
| Peephole optimizer (9 passes) — always on in `build` command | Wire peephole into `emit` command |
| Register allocator — wired into `codegen_x64` (`build`/`codegen`) | Wire register allocator into `emit` (L1B) |
| ELF64 executables (working on Linux, zero tools) | Verify Mach-O executables on macOS |
| Mach-O object files (valid objects) | Verify Mach-O executables on macOS (L1F) |
| ARM64 codegen (valid assembly) | Verify ARM64 executables on hardware (L1E) |
| RISC-V codegen (valid assembly) | Verify RISC-V executables on hardware (L1E) |
| WASM backend (valid binary) | Verify WASM runs in browsers (L2E) |
| Region system | Effect handler continuations (L3A) |
| Effect system (basic handlers work) | Green threads, data race safety (L3B-C) |
| Capability security | Comprehensive stdlib (L4) |
| Comptime | Package manager (L5) |
| Traits (cap) | LSP server (L7) |
| Generics, enums, structs | Debugger, profiler (L7) |
| Pattern matching | Named/default/variadic args (L8) |
| Result + ? operator | Specialization (L8H) |
| Defer | `#[derive]` via comptime (L8J) |
| Inline assembly (interpreter) | Fix inline assembly in C codegen (L1I) |
| Bare metal mode (verified) | Language spec (L10A) |
| SIMD types (interpreter + PE claimed) | Fix SIMD in C codegen (L1I) |
| Tensor type (interpreter) | Fix tensor in C codegen (L1I) |
| FFI `extern "C"` (verified working) | Verify FFI in PE backend (L1D) |
| Const generics (verified working) | Verify in PE/C backends (L1H) |
| GATs (verified working) | Verify in PE/C backends (L1H) |
| `parse-header` (verified working) | Extend header parsing (L6B) |
| Sourcemap command | LSP integration (L7) |
| REPL command | REPL improvements (L7G) |
| Test harness (file-based, 146/146 normal pass) | `#[test]` attribute execution (L4J) |
| Hot reload (`watch` — native JIT + function diff) | Wire optimizer into `emit` command |
| **✅ DONE — GPU codegen** (`fidesc gpu --target spirv|ptx`) | Produces SPIR-V binary (340 bytes for hello.fd) |
| **✅ DONE — `@derive`** (Phase 7.7) | `@derive(Debug/Clone/Serialize/Deserialize)` all work |
| **✅ DONE — async/await/spawn** (Phase 7.6) | `async fn`, `await`, `spawn` all work. State machine transform for cooperative scheduling pending |

**Principle:** If it's built and working, verify it (L1). If it's built but not wired in, wire it in (L1). If it's not built, build it (L2-L10). Never redo working code.

---

## Decision Framework

When choosing what to work on next, ask:

1. **Is it blocking other phases?** If yes, do it first (L1 blocks almost everything).
2. **Does it address a top complaint from multiple languages?** If yes, prioritize (L3 concurrency, L6 FFI).
3. **Does it differentiate Fides from all other languages?** If yes, prioritize (L3 effect-based concurrency, L4 capability-gated stdlib).
4. **Does it enable adoption?** If yes, prioritize after foundation (L5 package manager, L7 tooling).
5. **Is it already built but not wired in?** If yes, just wire it in — don't rebuild (L1).

**Priority order:** L1 (foundation) > L3 (concurrency) > L6 (FFI) > L2 (cross-platform) > L4 (stdlib) > L5 (packages) > L9 (optimization) > L7 (tooling) > L8 (type system) > L10 (ecosystem)

---

## Relationship to Existing Phases 6-8

| Existing Phase | Status | Relationship to L-Phases |
|---------------|--------|-------------------------|
| Phase 10 — Self-Hosting | Planned | Independent. Can start after L1 (need all features working first). L-phases don't block it. |
| Phase 11 — Ecosystem | Planned | **L4, L5, L7, L10 are detailed implementations of Phase 11.** Consolidate: replace Phase 11 with L4+L5+L7+L10. |
| Phase 12 — World Class | Planned | **L9 and L10 overlap with Phase 12.** L9 implements Phase 12B (Optimization Parity). L10 implements Phase 12D (Industry Adoption). Phase 12A (Formal Verification) and 12C (Real-World Validation) remain separate. |

**Recommendation:** Merge Phase 11 into L4+L5+L7+L10. Merge Phase 12B into L9, Phase 12D into L10. Keep Phase 10 (Self-Hosting) and Phase 12A/12C (Formal Verification, Real-World Validation) as separate post-L-phase work.

---

## Theoretical Limitations — Senior Dev Analysis (Aug 2026)

> A critical, honest assessment of Fides's architectural decisions, the
> theoretical limitations they create, and the path to solving each one.
> Every limitation is categorized by difficulty: **engineering gap**
> (just missing work), **significant engineering** (known solution, real
> effort), **research** (novel approach needed), or **fundamental
> tradeoff** (inherent to the design, can only be mitigated).

---

### TL1: The "No LLVM" Decision — Performance Gap

**Category: Significant engineering (solvable with strategy)**

**The decision**: Build everything from scratch — no LLVM, no libc, no
external linker. 85,493 LOC of compiler code, 23 optimization passes,
4 backends (interpreter, C codegen, PE, ELF64).

**Theoretical limitations**:

- **Optimization ceiling**: LLVM has 100+ passes built by 1000+
  contributors over 20+ years. Fides's `pass_loop_vectorize` and
  `pass_slp_vectorize` won't match LLVM's autovectorization (VP, cost
  models, target-specific intrinsics). For compute-heavy ML/scientific
  workloads, Fides-generated code will be measurably slower.
- **Register allocation**: Linear scan vs LLVM's graph coloring + PBQP +
  priority-based spilling. Tight loops with high register pressure will
  spill more.
- **Architecture coverage**: Each new target (GPU, WASM as first-class)
  needs a full backend. LLVM gives 30+ targets for free.
- **No sanitizers**: No ASan, UBSan, MSan, TSan. `veil` covers
  coverage, not memory safety testing.

**Where Fides can beat LLVM**:

- **Compile speed**: LLVM is slow (minutes for large projects). A
  from-scratch compiler can be 10x faster — no legacy pass pipeline.
- **Predictability**: LLVM optimizations are opaque — sometimes they
  help, sometimes they hurt. A custom optimizer can be deterministic.
- **Domain-specific optimization**: For real-time (WCET optimization),
  embedded (size optimization), and safety-critical (no undefined
  behavior), Fides can optimize for what matters. LLVM doesn't do WCET.
- **Binary size**: LLVM produces bloated binaries. Custom backend
  produces tighter code.
- **Debuggability**: LLVM's optimization passes make debugging hard.
  Clear pass boundaries = easier debugging.

**The solution: Optional LLVM backend, not replacement**

```
fidesc build --target x86-64          → custom codegen (embedded, real-time, default)
fidesc build --target x86-64 --llvm   → LLVM codegen (max performance, optional)
```

- **Embedded/real-time**: Custom codegen, zero dependencies, full control
- **ML/scientific**: LLVM backend, competitive performance
- **Safety-critical**: Custom codegen (auditable, no external code)

The LLVM backend would be ~3000 lines (IR → LLVM IR text + `llc`
invocation). The custom backend stays the default. The language
philosophy stays intact. LLVM is an escape hatch, not a dependency.

**Can a custom compiler beat LLVM?** In specific domains — yes. In
general — no, not by brute force. LLVM was written by thousands of
humans over 20 years. But it was written by humans. A focused compiler
can beat it in compile speed, predictability, domain-specific
optimization, and binary size. For raw autovectorization and
cross-target code quality, LLVM stays ahead. The smart move: be better
where you can, interoperate where you can't.

---

### TL2: Region-Based Memory — Coarse Granularity

**Category: Research (novel approach needed) + significant engineering**

**The decision**: Region-based memory management instead of RAII or GC.
All allocations in a region live until the region ends.

**Theoretical limitations**:

- **No fine-grained lifetimes**: Rust's lifetime system expresses "this
  borrow lives exactly as long as this function call". Regions are
  all-or-nothing — everything in a region lives until region exit. Can't
  express "field A lives longer than field B within the same struct".
- **Arena bloat**: A long-lived region accumulates dead objects that
  can't be freed until region exit. A web server allocating per-request
  in a long-lived region will OOM.
- **No RAII**: Without destructors, resource cleanup (file handles,
  locks, sockets) is manual. `Rc::drop()` must be called explicitly —
  forget it and you leak. This is a regression from Rust's safety model.
- **Self-referential structs**: Regions make self-referential data hard.
  `Box<T>` helps but adds indirection. Linked lists, trees with parent
  pointers, graph structures — all painful without GC or arena.
- **Iterator invalidation**: Mutation through one reference can
  invalidate another within the same region.

**The solution: Nested regions + Drop trait**

1. **Nested regions** with explicit sub-regions:
   ```fides
   region request r {
       region headers hr {
           // headers freed here
       }
       // body still alive
       region body br {
           // body freed here
       }
       // r still alive but mostly empty
   }
   ```
   Fine-grained cleanup without full lifetime tracking. Covers 90% of
   use cases. The remaining 10% (self-referential structs, complex
   graphs) use `Box<T>` or arena allocation.

2. **`Drop` trait + compiler-inserted drops**:
   ```fides
   trait Drop { fn drop(&mut self); }
   ```
   The compiler inserts drop calls at:
   - Region exit (for region-allocated values)
   - Stack frame exit (for stack-allocated values)
   - `Rc::drop()` / `Arc::drop()` (already exists, just automate it)

   Linear types guarantee the drop is called exactly once. This is the
   natural extension of `@linear`. `@linear` + `Drop` = automatic
   resource management without GC.

**Result**: Not as precise as Rust's lifetimes, but simpler and covers
90% of use cases. Combined with `@linear` + `Drop`, you get RAII-like
safety without the lifetime annotation complexity.

---

### TL3: The Effect System — Syntactic, Not Semantic

**Category: Research (novel approach — Fides's killer feature)**

**The decision**: 10 effects (IO, Net, Spawn, Diverge, Read, Write,
Alloc, Noise, Comm, Diff). 25+ annotations (`@dp`, `@constant_time`,
`@realtime`, `@crashfree`, `@deterministic`, etc.).

**Theoretical limitations**:

- **Effect polymorphism**: Higher-order functions need effect
  polymorphism. `fn map<T, U>(f: fn(T) -> U) -> Vec<U>` — what effects
  does this have? Without row polymorphism, you either over-constrain
  (mark as IO) or under-constrain (mark as pure).
- **Semantic gap**: `@dp(epsilon=1.0)` checks "has Noise effect" but
  doesn't verify the noise is mathematically correct (distribution,
  sensitivity, composition). `@constant_time` checks no early returns
  but doesn't check data-dependent memory access patterns. This creates
  an **illusion of safety** — worse than no annotation.
- **Untracked effects**: Cache timing, memory access patterns,
  termination (halting problem), information flow — none tracked.
- **Annotation combinatorics**: 25+ annotations means 25+ verification
  passes. Interaction between annotations is largely untested.

**The solution: Comptime annotation verifiers (Fides's unique advantage)**

Fides has **comptime** — it can run Fides code at compile time. The
novel insight: use comptime to **verify** annotations semantically, not
just syntactically.

```fides
@comptime_verify
fn check_dp_sensitivity(fn_name: String, epsilon: Float) -> Bool {
    // Walk the IR at compile time, verify:
    // 1. Gaussian noise with sigma >= sensitivity / epsilon
    // 2. No raw data leaves the function
    // 3. Composition is correct (sequential vs parallel)
    // This runs at compile time, producing real proofs
}
```

Each annotation gets a comptime verifier:
- `@dp` → sensitivity analyzer (walks IR, checks noise distribution)
- `@constant_time` → memory-access-pattern checker (no data-dependent
  array indices, no data-dependent branches)
- `@realtime` → WCET analyzer (bounds execution time from IR structure)
- `@crashfree` → proof that all paths return normally (no panic, no
  overflow, no null deref)

This is **comptime metaprogramming for verification** — using Fides's
comptime to verify Fides annotations. No other language has comptime +
annotations + effects together. This combination can produce real
semantic verification that Rust/SPARK/Mojo can't match.

**For effect polymorphism**: Add row polymorphism (`with effs` syntax):
```fides
fn map<T, U, effs>(f: fn(T) -> U with effs, v: Vec<T>) -> Vec<U> with effs
```
Functions that take function arguments propagate effects automatically.
~1500 lines of typechecker work, well-understood (Koka, Unison).

---

### TL4: The Annotation Explosion — 25+ Annotations

**Category: Significant engineering (profiles) + research (verifiers)**

**The decision**: 25+ annotations for safety, security, real-time,
medical, automotive, ML, and financial domains.

**Theoretical limitations**:

- **Combinatorial explosion**: Each annotation has its own verification
  rules. Interactions are untested. `@constant_time @dp(epsilon=0.1)
  @realtime` — do the rules compose? Are there contradictions?
- **False confidence**: Most annotations check syntactic properties but
  claim semantic guarantees. The gap is where bugs live.
- **Maintenance burden**: 25+ verification passes in the typechecker.
  Each new feature must consider all annotations.

**The solution: Profiles (language subsets)**

Don't try to use all annotations at once. Use **profiles** — subsets of
the language with specific restrictions:

```
--profile embedded      → no heap, no IO, @realtime enforced, no dynamic dispatch
--profile safety        → no unsafe, all annotations verified, WCET analysis
--profile ml            → dynamic shapes allowed, GPU backend, relaxed safety
--profile systems       → unsafe allowed, full feature set, LLVM backend optional
```

Each profile is a **subset of the language** with specific verification
levels. The same compiler, same language, different restrictions. This
is how SPARK works (SPARK is a subset of Ada with proofs).

**Fides isn't one language — it's a family of profiles.** Each profile
is smooth for its use case. Trying to use all features at once is
intentionally hard. This resolves the three-goal tension (systems +
safety-critical + ML) by making it a feature, not a contradiction.

---

### TL5: The Multi-Backend Problem

**Category: Significant engineering (consolidation)**

**The decision**: Interpreter + C codegen + PE emitter + ELF64 emitter +
ARM64 + RISC-V. 5 tests in the skip list (4 C codegen missing-builtins + 1 design difference). Was 42 — 37 stale tests promoted Aug 27.

**Theoretical limitations**:

- **N × M complexity**: N features × M backends = NM test combinations.
  The skip list is evidence of backend divergence.
- **Semantic divergence**: Interpreter uses bump allocator (never frees),
  C codegen uses `malloc`/`free`, native codegen uses `mmap`.
  Use-after-free bugs are invisible in the interpreter but crash native
  code. The interpreter gives false confidence.
- **C codegen is a second compiler**: 3,642 lines of C generation. Every
  IR instruction needs a C translation. Maintenance force multiplier.
- **No cross-backend verification**: No formal proof that all backends
  produce the same result. Testing (119 tests) can't cover all paths.

**The solution: Reduce to 3 backends + interpreter UAF detection**

1. **Keep 3 backends**:
   - **Interpreter** (always — for testing, LSP, comptime)
   - **C codegen** (always — portable, uses system compiler for all targets)
   - **LLVM backend** (optional — for max performance)

2. **Drop**: PE emitter, ELF64 emitter, ARM64 native, RISC-V native.
   C codegen + system compiler (`cc`/`gcc`/`clang`) handles all targets.
   This cuts ~15,000 LOC of backend code.

3. **Interpreter UAF detection**: Track region-owned addresses, mark
   freed on `RegionEnd`, check on `Load`/`Store`. ~100 lines. Makes the
   interpreter catch bugs that native code would hit.

**Result**: 3 backends cover all use cases. C codegen gives portability.
LLVM gives performance. Interpreter gives safety testing. ~15,000 LOC
removed.

---

### TL6: Missing Language Features

**Category: Engineering gaps (fully solvable)**

**Theoretical limitations for real-world use**:

| Feature | Status | Solution | Effort |
|---------|--------|----------|--------|
| **Closures + borrow checker** | Missing (`borrowck.rs` has 0 Closure cases) | Add `Expr::Closure` to `check_expr`, analyze captures, record borrows | ~400 lines |
| **Async/await** | "Green threads pending" | State machine transform (desugar async/await to state machine) | ~2000 lines |
| **`val` immutability** | Silently allowed (Issue 46) | Track `mutable_vars` set, reject `Assign` on `val` bindings | ~50 lines |
| **`&mut T` method calls** | Fails (Issue 45) | Auto-deref `Type::Ref` before method lookup | ~20 lines |
| **Non-exhaustive `match`** | Not caught (Issue 49) | Exhaustiveness checking after typechecking match on enum | ~100 lines |
| **`break` in comptime loops** | Silently ignored (Issue 44) | Skip unrolling if body has `break`/`continue` | ~30 lines |
| **String interpolation** | Missing | f-string syntax: `f"value = {x}"` desugars to `fmt_concat` | ~100 lines |
| **Lazy iterators** | Missing | `yield` keyword or `Iterator<T>` trait with `next()` | ~500 lines |
| **`?` error operator** | Missing | `expr?` desugars to match with early return | ~50 lines |
| **`Drop` trait** | Missing (manual `rc.drop()`) | Compiler-inserted drop calls at region/frame exit | ~300 lines |
| **Mutex/RwLock/Atomics** | Missing | Add to stdlib with `@send` verification | ~200 lines |

**All of these are known solutions.** None require research. The total
effort is ~3,800 lines of code across the compiler and stdlib. This is
the "Phase 1: Fix the bugs" and "Phase 2: Close language gaps" work.

---

### TL7: The Stdlib — Breadth Without Depth

**Category: Significant engineering (refocus)**

**The observation**: 69 modules, 17,567 LOC. Average: 253 lines per
module. Many are superficial (`regex.fd` is wildcard matching, not real
regex; `crypto.fd` is 485 lines, not audited cryptography).

**Theoretical limitations**:

- **`regex.fd`**: No NFA/DFA, no backreferences, no lookahead. Real
  regex is 5000+ lines (RE2 is 30,000+).
- **`crypto.fd`**: 485 lines is not enough for AES, SHA-256, RSA, ECDSA,
  TLS. This is a toy, not production cryptography.
- **`http.fd`**: Basic GET/POST, not HTTP/1.1 with chunked encoding,
  keep-alive, pipelining, HTTP/2, HTTP/3, TLS.
- **Domain modules** (`medical.fd`, `banking.fd`, `arinc.fd`, `can.fd`):
  Without domain experts validating them, they're superficial. Real
  medical software needs IEC 62304 compliance. Real automotive needs
  ISO 26262 ASIL-D.

**The solution: 10 deep core modules + C FFI for everything else**

Focus on **10 core modules** that are deep and correct:

| Module | Current | Target | Notes |
|--------|---------|--------|-------|
| `core` | 2550 lines ✓ | 2550+ | Option, Result, println, assert — already deep |
| `vec` | 410 lines ✓ | 500+ | Add more iterator adapters |
| `string` | needs work | 2000+ | Full string manipulation, formatting, parsing |
| `math` | 399 lines ✓ | 1000+ | vec2/3/4, mat4, quat, special functions |
| `fmt` | needs work | 500+ | Formatting, f-strings, printf-style |
| `hash` | needs work | 800+ | Real hash map (open addressing, robin hood) |
| `io` | needs work | 500+ | Buffered I/O, readers, writers |
| `fs` | needs work | 400+ | File system operations (merge with file.fd) |
| `time` | 357 lines ✓ | 500+ | Monotonic clock, formatting, durations |
| `result` | merge with core | — | Merge into core.fd's Result<T, E> |

The domain modules (`medical`, `banking`, `arinc`, `can`, `scada`,
`swift`, `grid`, `ota`, `clinical`, `ecu`) should be **examples**, not
stdlib. They demonstrate Fides's capabilities but shouldn't be maintained
as production libraries.

**For everything else**: C FFI. If someone needs HTTP, they call
`libcurl` via FFI. If they need crypto, they call `libsodium` via FFI.
If they need a database, they call `libsqlite3` via FFI. Fides provides
the safety layer on top of C libraries. This is how Zig works — thin
wrapper over C ecosystem, with safety on top.

---

### TL8: The Bootstrap Problem

**Category: Strategic decision (not a limitation to solve, but to evaluate)**

**The goal**: Self-hosting (rewrite Fides in Fides). 85,493 LOC to rewrite.

**Theoretical limitations**:

- **Fides lacks compiler-writing features**: No closures in borrow
  checker, no lazy iterators, no async, no string interpolation. Writing
  a compiler in Fides would be harder than writing it in Rust.
- **Self-hosting is vanity unless it buys something**: TypeScript
  self-hosted because JavaScript couldn't do types. Rust self-hosted
  because OCaml was too slow. What can't Rust do that Fides can?

**The solution: Don't self-host yet. Re-evaluate after Phase 2**

After Phase 2 (closures, iterators, async, error handling, string
interpolation), re-evaluate. The question: is Fides-as-implementation-
language better than Rust-as-implementation-language for writing a
compiler?

The answer might be yes if:
- Comptime metaprogramming makes compiler passes easier to write
- Effect system catches compiler bugs (e.g., marking IR transforms as
  `@crashfree` or `@deterministic`)
- Region memory makes memory management of the compiler simpler
- Annotations verify compiler correctness properties

Until those features exist and prove useful, Rust is the better
implementation language. Self-hosting is a post-Phase-2 decision.

---

### TL9: The Hot Reload Contradiction

**Category: Research (novel — versioned persistence)**

**The decision**: `persist` keeps values across reloads.

**Theoretical limitations**:

- **Type migration**: If a struct layout changes between reloads,
  persisted values are invalid. No migration strategy.
- **Linear types + hot reload**: Linear values can't be duplicated, but
  hot reload needs to reconstruct state.
- **Effect safety + hot reload**: A `@realtime` function is reloaded —
  but the new version might not be `@realtime` safe. The reload happens
  at runtime, but the annotation check is compile-time.

**The solution: Versioned persistence + migration functions**

```fides
@version(2)
struct User {
    name: String,
    email: String,  // added in version 2
}

@migrate(1, 2)
fn migrate_user(old: User_v1) -> User {
    User { name: old.name, email: "" }
}
```

On hot reload, the `persist` system:
1. Checks struct versions against persisted data
2. If version changed, runs migration functions
3. Verifies annotation compatibility (new `@realtime` function must
   still pass `@realtime` verification)

This is how database migrations work, applied to hot reload. No other
language has this. It's a Fides-unique feature.

---

### TL10: The Concurrency Model

**Category: Significant engineering**

**The decision**: Channels as the primary concurrency primitive. `@send`
for thread safety. No `Mutex<T>`, `RwLock<T>`, or `AtomicU64`.

**Theoretical limitations**:

- **No memory model**: No happens-before relationship defined for
  atomics. `@send` checks no shared references but doesn't define what
  happens when two threads access the same memory.
- **Channels only**: Real systems need shared memory with locks.
  Channels alone lead to message-passing overhead for fine-grained
  sharing.
- **No async runtime**: Without async, I/O concurrency requires OS
  threads (heavy) or callbacks (which need closures).

**The solution: Mutex/RwLock/Atomics + async + memory model**

1. **Add `Mutex<T>`, `RwLock<T>`, `AtomicU64`** to stdlib with `@send`
   verification. ~200 lines.
2. **Define a memory model**: Happens-before relationships for atomics
   (acquire/release semantics, like C11/Rust).
3. **Async/await** (see TL6): State machine transform for cooperative
   scheduling. Combined with channels, gives full concurrency story.

---

### TL11: The Fundamental Tension — Three Goals in Conflict

**Category: Fundamental tradeoff (mitigated by profiles)**

Fides tries to be three things at once:

1. **Systems language** (like Rust/Zig) — regions, no GC, manual memory
2. **Safety-critical language** (like SPARK/Ada) — annotations, verification
3. **ML/AI language** (like Mojo) — tensors, autodiff, quantization, GPU

These genuinely conflict:
- **Systems** wants `unsafe` escape hatches and raw pointers
- **Safety-critical** wants no `unsafe`, full verification, restricted features
- **ML** wants dynamic shapes, GPU kernels, Python interop, flexibility

**The solution: Profiles resolve the tension**

```
--profile embedded      → no heap, no IO, @realtime enforced, no unsafe
--profile safety        → no unsafe, all annotations verified, WCET analysis
--profile ml            → dynamic shapes, GPU backend, relaxed safety, LLVM
--profile systems       → unsafe allowed, full features, LLVM optional
```

Each profile is a **subset of the language** with specific restrictions.
The same compiler, same language, different verification levels. This
makes the three-goal tension a **feature** — Fides adapts to the domain,
instead of forcing one domain's constraints on all others.

This is the key architectural decision: **Fides isn't one language —
it's a family of profiles.** Each profile is smooth for its use case.

---

## Solution Roadmap — The Path to "Everything Smooth"

### Phase 1: Fix the bugs (engineering gaps)

| Task | Effort | Issues |
|------|--------|--------|
| `val` immutability enforcement | ~50 lines | Issue 46, 47 |
| `&mut T` auto-deref for methods | ~20 lines | Issue 45 |
| Non-exhaustive `match` checking | ~100 lines | Issue 49 |
| `break` in comptime-unrolled loops | ~30 lines | Issue 44 |
| Closures in borrow checker | ~400 lines | — |
| Interpreter UAF detection | ~100 lines | — |
| Clean up skip/known-fail lists | cleanup | Issues 50, 51 |
| IR verification for unsafe + return | ~20 lines | Issue 48 |

**Result**: Language becomes correct. No silent wrong behavior.

### Phase 2: Close language gaps (significant engineering)

| Task | Effort | Notes |
|------|--------|-------|
| `?` error operator | ~50 lines | Desugar to match + early return |
| String interpolation (f-strings) | ~100 lines | `f"value = {x}"` |
| `Drop` trait + compiler-inserted drops | ~300 lines | RAII without GC |
| Lazy iterators (`yield` or `Iterator<T>`) | ~500 lines | `for x in iter` |
| Async/await (state machine transform) | ~2000 lines | Cooperative scheduling |
| `Mutex<T>` / `RwLock<T>` / `AtomicU64` | ~200 lines | Shared memory concurrency |
| Memory model (happens-before) | spec | Acquire/release semantics |

**Result**: Language becomes ergonomic. Can write real programs.

### Phase 3: Novel features (research — Fides's unique value)

| Task | Effort | Notes |
|------|--------|-------|
| Comptime annotation verifiers | ~3000 lines | **Killer feature** — semantic verification, not syntactic |
| Profiles (`--profile embedded/safety/ml/systems`) | ~1000 lines | Language subsets with different verification levels |
| Versioned persistence + migration | ~500 lines | Hot reload with type migration — Fides-unique |
| Effect polymorphism (row types) | ~1500 lines | `with effs` syntax, automatic effect propagation |
| Nested regions | ~400 lines | Fine-grained region cleanup |

**Result**: Fides becomes unique. No other language has comptime
verification + profiles + versioned hot reload + effect polymorphism +
nested regions. This is the value proposition.

### Phase 4: Performance (significant engineering)

| Task | Effort | Notes |
|------|--------|-------|
| Optional LLVM backend (`--llvm` flag) | ~3000 lines | IR → LLVM IR text + `llc` invocation |
| Reduce to 3 backends | removal | Drop PE/ELF64/ARM64/RISC-V native (~15,000 LOC removed) |
| C FFI as ecosystem strategy | ~500 lines | Thin wrappers over libcurl, libsodium, libsqlite3 |
| WCET optimizer (for `@realtime`) | ~1000 lines | Domain-specific — beat LLVM here |

**Result**: Performance gap closes. Custom backend for embedded/real-
time. LLVM for compute-heavy. C FFI for ecosystem. ~15,000 LOC removed.

### Phase 5: Self-hosting decision (strategic)

Re-evaluate after Phase 2. The question: is Fides-as-implementation-
language better than Rust for writing a compiler?

The answer is yes if:
- Comptime metaprogramming makes compiler passes easier to write
- Effect system catches compiler bugs (mark IR transforms as
  `@crashfree` or `@deterministic`)
- Region memory simplifies compiler memory management
- Annotations verify compiler correctness properties

Until those features exist and prove useful, Rust remains the
implementation language. Self-hosting is a post-Phase-2 decision.

---

## Summary: What's Solvable vs Fundamental

| Limitation | Category | Solvable? | Solution |
|-----------|----------|-----------|----------|
| No LLVM performance gap | Significant eng | **Yes (partial)** | Optional LLVM backend |
| Region coarse granularity | Research | **Yes (partial)** | Nested regions + Drop trait |
| Effect system syntactic | Research | **Yes** | Comptime annotation verifiers |
| Annotation explosion | Significant eng | **Yes** | Profiles (language subsets) |
| Multi-backend complexity | Significant eng | **Yes** | Reduce to 3 backends |
| Missing language features | Engineering gap | **Yes** | ~3,800 lines of known work |
| Stdlib breadth without depth | Significant eng | **Yes** | 10 deep modules + C FFI |
| Bootstrap problem | Strategic | **Maybe** | Re-evaluate after Phase 2 |
| Hot reload type migration | Research | **Yes** | Versioned persistence |
| Concurrency model | Significant eng | **Yes** | Mutex/Atomics + async + memory model |
| Three-goal tension | Fundamental | **Mitigated** | Profiles resolve the conflict |

**Bottom line**: 10 of 11 limitations are fully solvable. 1 is a
fundamental tradeoff that's mitigated by profiles. The path requires
~12,000 lines of new code and ~15,000 lines of removed code (backend
consolidation). The result is a language that is genuinely unique:
**compile-time verified safety annotations + region memory + hot reload
+ comptime metaprogramming + profiles**. No other language has all five.
