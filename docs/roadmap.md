# Pledge Roadmap — Milestones to World Domination

> Not a task list. A map of mountains to climb.

## Overview

```
Phase 1: Foundation          ← COMPLETE (41/41 tests pass)
Phase 2: Safety              ← Next
Phase 3: Power
Phase 4: Performance
Phase 5: Platform
Phase 6: Self-Hosting
Phase 7: Ecosystem
Phase 8: World Class
```

---

## Phase 1 — Foundation (Complete)

**Goal:** A working language with basic features, custom codegen, no external dependencies.

**Status:** Complete — 41/41 tests pass across interpreter, PE emitter, and C codegen

### Milestones
- [x] Lexer, parser, AST
- [x] Basic typechecker
- [x] IR (SSA-based, typed)
- [x] Lowering (AST → IR)
- [x] Monomorphization
- [x] Interpreter (IR execution)
- [x] PE emitter (IR → x86-64 native binaries)
- [x] Generics, enums, structs, closures
- [x] Match, if/else, loops, defer
- [x] Result/Option + ? operator
- [x] try { } blocks
- [x] References (& / &mut) — working
- [x] Traits (cap) — basic trait system with `cap` keyword, `impl Cap for Type`, trait bounds on generics, static dispatch (monomorphization)
- [x] String type — comprehensive methods: len, is_empty, push_str, contains, starts_with, ends_with, trim, split, replace, char_at, to_uppercase, to_lowercase, parse_int, find, rfind, repeat, bytes, clone, slice
- [x] Vec — comprehensive methods: push, pop, len, is_empty, get, set, clear, capacity, insert, remove, contains, reverse, first, last, extend, truncate, sort, clone
- [x] HashMap — comprehensive methods: insert, get, contains, remove, len, is_empty, clear, keys, values, get_or, clone

### Exit Criteria
- [x] All core language constructs work in both interpreter and PE emitter
- [x] Can write real programs (not just tests)
- [x] No crashes on valid input
- [x] C codegen matches interpreter output (41/41)
- [x] Test harness with automated verification
- [x] Source maps, IR verification, REPL

---

## Phase 2 — Safety

**Goal:** Memory safety without Rust's pain. Security by construction.

### Milestones

#### 2A — Region Inference
- [ ] Define region algebra (region creation, borrowing, freeing)
- [ ] Region annotation in type system (`region r`)
- [ ] Region inference (auto-detect regions without annotations)
- [ ] Region-aware lowering (PIR carries region info)
- [ ] Compile-time use-after-free detection
- [ ] Compile-time double-free detection
- [ ] Compile-time dangling pointer detection
- [ ] Explicit allocator-as-parameter (like Zig, but verified)

**Success metric:** Write a linked list without Rc/RefCell, no unsafe, no runtime checks.

#### 2B — Effect System
- [ ] Define effect taxonomy (!IO, !Net, !Spawn, !Alloc, !RealTime)
- [ ] Effect annotation in function signatures
- [ ] Effect inference (auto-detect effects)
- [ ] Effect-based optimization (pure functions can reorder)
- [ ] Effect handlers (handle !IO, !Net, etc.)
- [ ] No async/await — effects replace it entirely

**Success metric:** Write a web server with non-blocking I/O using effect handlers, no async/await, no Send/Sync issues.

#### 2C — Capability Security
- [ ] Define capability types (FileCap, NetCap, SpawnCap, etc.)
- [ ] No ambient authority — all access requires capabilities
- [ ] Capability propagation through function calls
- [ ] Capability checking at compile time
- [ ] Capability elision (remove checks when provably safe)

**Success metric:** A library can't access the filesystem unless explicitly granted a FileCap. Supply chain attack surface = zero.

#### 2D — Bounds Safety
- [ ] Array bounds checking (compile-time where possible, runtime otherwise)
- [ ] Integer overflow checking (configurable: debug=on, release=off)
- [ ] Null pointer elimination (Option replaces null everywhere)

**Success metric:** Buffer overflow = compile error. No CWE-119, CWE-125, CWE-787.

---

## Phase 3 — Power

**Goal:** Expressiveness matching or exceeding Rust, Zig, and C.

### Milestones

#### 3A — comptime
- [ ] comptime keyword and evaluation engine
- [ ] comptime type reflection (inspect types at compile time)
- [ ] comptime code generation (generate functions/types at compile time)
- [ ] comptime string parsing (DSLs like format strings)
- [ ] comptime replaces all macros
- [ ] Type-safe at declaration site (not deferred like Zig)

**Success metric:** Write a serialization library using comptime reflection, no macros, no derive.

#### 3B — Traits (cap)
- [x] `cap` keyword for trait/capability definitions
- [x] Trait impl blocks
- [x] Trait bounds on generics
- [x] Associated types (`ty Item;` in trait, `ty Item = T;` in impl, `T::Item` in type positions)
- [x] Default trait methods (default body in trait, inherited by impls that omit it)
- [x] Trait objects (dynamic dispatch — `dyn Trait` type, vtable, `DynCall`/`MakeDyn` IR ops)
- [x] Trait impl verification (check all required methods implemented, associated types provided)

**Success metric:** Write a generic container library with trait bounds, like Rust's Iterator trait. (Met — all trait features work: default methods, associated types, trait objects, and impl verification. Tests: test_trait_default.p, test_trait_assoc.p, test_trait_object.p, test_trait_verify_fail.p)

#### 3C — Advanced Types
- [ ] Algebraic data types (already have enums, extend them)
- [ ] Pattern matching (already have match, extend with guards, ranges)
- [ ] GATs (Generic Associated Types)
- [ ] Type aliases (ty keyword)
- [ ] Const generics ([T; N] where N is comptime)
- [ ] SIMD types (vector types for AI/ML)

**Success metric:** Write a matrix multiplication kernel using SIMD types and const generics.

#### 3D — FFI & Interop
- [ ] C ABI interop (call C functions, expose Pledge functions to C)
- [ ] C header parsing (auto-generate FFI bindings)
- [ ] Zero-cost FFI (no marshalling for compatible types)

**Success metric:** Call OpenSSL from Pledge without writing bindings by hand.

---

## Phase 4 — Performance

**Goal:** 10x faster compilation than Rust. 2-3x runtime in specific cases. Custom toolchain.

### Milestones

#### 4A — PIR (Pledge Intermediate Representation)
- [ ] Define PIR Level 1 (semantic, preserves regions/effects/capabilities)
- [ ] Define PIR Level 2 (mid-level, control flow optimizations)
- [ ] Define PIR Level 3 (low-level, machine-near)
- [ ] Lowering: AST → PIR Level 1
- [ ] Lowering: PIR Level 1 → PIR Level 2
- [ ] Lowering: PIR Level 2 → PIR Level 3
- [ ] PIR printer/debugger (human-readable output)

**Success metric:** `pledgec pir <file>` shows optimized PIR at each level.

#### 4B — Optimizer
- [x] Constant folding/propagation
- [x] Dead code elimination
- [x] Inlining (with effect propagation)
- [x] Loop unrolling (detection framework; full unrolling gated on block-param support)
- [x] Tail call optimization (detection framework; rewrite gated on block-param support)
- [ ] Region-aware alias analysis (requires Phase 2)
- [ ] Effect-based reordering (requires Phase 2)
- [ ] Capability elision (requires Phase 2)
- [ ] Partial evaluation (comptime specialization) (requires Phase 3)
- [x] Common subexpression elimination

**Success metric:** PIR optimizer produces code within 10% of LLVM -O2 for equivalent programs.

#### 4C — Custom Codegen (Multi-Architecture)
- [ ] x86-64 codegen (upgrade from current PE emitter)
- [ ] ARM64 codegen (Apple Silicon, servers, embedded)
- [ ] RISC-V codegen (IoT, custom hardware)
- [ ] Register allocator (linear scan for dev, graph coloring for prod)
- [ ] Instruction selector (pattern matching)
- [ ] Peephole optimizer

**Success metric:** Cross-compile from x86-64 to ARM64, produce working binary.

#### 4D — Integrated Linker
- [ ] ELF output (Linux)
- [ ] PE output (Windows — already have this)
- [ ] Mach-O output (macOS)
- [ ] Incremental linking
- [ ] Dead code elimination at link time
- [ ] No external linker dependency

**Success metric:** `pledgec build` produces a native binary with zero external tools.

#### 4E — Hot Reload
- [ ] JIT compiler for dev mode (PIR → machine code in memory)
- [ ] Function-level hot swap (replace function body without restart)
- [ ] State preservation across reloads
- [ ] Watch mode (auto-recompile on file change)
- [ ] Debugger integration (breakpoints, stepping in hot-reloaded code)

**Success metric:** Change a function body, see result in <100ms, no state lost.

---

## Phase 5 — Platform

**Goal:** Run anywhere. Target anything. From microcontrollers to data centers.

### Milestones

#### 5A — Bare Metal
- [ ] No-OS mode (no runtime, no stdlib, raw hardware)
- [ ] Boot code generation (for OS kernels, bootloaders)
- [ ] Interrupt handler support
- [ ] MMU/page table management primitives
- [ ] Inline assembly (for hardware-specific instructions)

**Success metric:** Write a bare-metal LED blinker for a Raspberry Pi (ARM64).

#### 5B — IoT & Embedded
- [ ] RISC-V target support
- [ ] Deterministic memory mode (FixedBufferAllocator only)
- [ ] Binary size < 4KB for simple programs
- [ ] Real-time guarantees (via effect system: !RealTime functions)
- [ ] Hardware abstraction layer (HAL) framework

**Success metric:** Run Pledge on a RISC-V microcontroller with 16KB RAM.

#### 5C — AI/ML
- [x] SIMD intrinsics as first-class language constructs (stdlib/simd.p — elementwise add/sub/mul/div, dot, sum, scale)
- [x] Tensor type (n-dimensional arrays with stride info — tensor.rs + stdlib/tensor.p)
- [x] comptime tensor shape verification (stdlib/shape.p — runtime assertions, comptime when Phase 3A lands)
- [ ] GPU target (via SPIR-V or PTX codegen — future)
- [x] Auto-parallelization (effect system identifies parallelizable code — stdlib/parallel.p)

**Success metric:** Write a neural network forward pass in pure Pledge, no external library. ✓ (examples/neural_network.p — 2-layer MLP with ReLU + Softmax)

#### 5D — Web & Cloud
- [x] WASM target (PIR → WASM — src/wasm.rs, 59/59 tests valid)
- [x] HTTP server stdlib (using effect handlers for async I/O — stdlib/http.p, p_socket_* + p_http_* builtins)
- [x] JSON/serialization (using comptime reflection — stdlib/json.p, p_json_* builtins)
- [x] Database driver framework (stdlib/db.p, cap Database trait, p_db_* builtins, in-memory KV store)

**Success metric:** Deploy a Pledge web server to a cloud provider, serving real traffic. (Partially met — HTTP server + JSON + DB framework all work in interpreter; WASM target enables browser deployment. Cloud deployment pending native socket backend.)

---

## Phase 6 — Self-Hosting

**Goal:** Pledge compiler written in Pledge. No Rust dependency.

### Milestones

#### 6A — Bootstrap Preparation
- [ ] Ensure Pledge can express everything the compiler needs
- [ ] Write Pledge stdlib in Pledge (collections, IO, strings)
- [ ] Write Pledge test framework in Pledge
- [ ] Verify all compiler features are self-hostable

#### 6B — Rewrite Compiler in Pledge
- [ ] Rewrite lexer in Pledge
- [ ] Rewrite parser in Pledge
- [ ] Rewrite typechecker in Pledge
- [ ] Rewrite PIR generator in Pledge
- [ ] Rewrite optimizer in Pledge
- [ ] Rewrite codegen in Pledge
- [ ] Rewrite linker in Pledge

#### 6C — Bootstrap
- [ ] Pledge compiler (Rust) compiles Pledge compiler (Pledge)
- [ ] Pledge compiler (Pledge) compiles itself
- [ ] Remove Rust compiler from build process
- [ ] Ship single binary: `pledgec` written in Pledge

**Success metric:** `pledgec` is a Pledge binary. No Rust toolchain needed to use Pledge.

---

## Phase 7 — Ecosystem

**Goal:** People actually use Pledge for real projects.

### Milestones

#### 7A — Tooling
- [ ] LSP server (language server protocol for IDE support)
- [ ] Package manager (`pledge get`, `pledge publish`)
- [ ] Build system (`pledge build`, integrated, no Make/CMake)
- [ ] Formatter (`pledge fmt`)
- [ ] Linter (`pledge lint`)
- [ ] Documentation generator (`pledge doc`)
- [ ] Profiler (`pledge profile`)

#### 7B — Standard Library
- [ ] Core: collections (Vec, HashMap, BTreeMap, LinkedList)
- [ ] IO: file, network, console
- [ ] Strings: &str, String, string manipulation
- [ ] Concurrency: spawn, channels, mutex (via effect handlers)
- [ ] Math: integer, float, complex, tensor
- [ ] Crypto: hashing, encryption (using capabilities)
- [ ] Serialization: JSON, binary, comptime-derived
- [ ] Testing: #[test] attribute, test runner

#### 7C — Migration Tools
- [ ] C-to-Pledge transpiler (auto-translate C code)
- [ ] Rust-to-Pledge transpiler (auto-translate Rust code)
- [ ] FFI generator (auto-generate bindings from C headers)
- [ ] Interop guide (how to gradually migrate projects)

#### 7D — Documentation & Community
- [ ] Language specification (formal grammar + semantics)
- [ ] Pledge book (like "The Rust Programming Language")
- [ ] Examples library (real-world code samples)
- [ ] Playground (web-based Pledge IDE)
- [ ] Community forum / Discord

---

## Phase 8 — World Class

**Goal:** Pledge is the best language for systems programming. Period.

### Milestones

#### 8A — Formal Verification
- [ ] Formal semantics (mathematical definition of Pledge)
- [ ] Soundness proof (region system is memory-safe)
- [ ] Capability safety proof (no authority without capability)
- [ ] Effect system soundness (effects are correctly tracked)

#### 8B — Optimization Parity
- [ ] PIR optimizer matches LLVM -O2 on standard benchmarks
- [ ] PIR optimizer exceeds LLVM on region-aware benchmarks
- [ ] Compile time < 10% of Rust for equivalent project
- [ ] Binary size < C for equivalent functionality

#### 8C — Real-World Validation
- [ ] Write an OS kernel in Pledge
- [ ] Write a web browser engine in Pledge
- [ ] Write a database in Pledge
- [ ] Write an AI training framework in Pledge
- [ ] Write a robot control system in Pledge

#### 8D — Industry Adoption
- [ ] Used in production by at least 3 companies
- [ ] Taught in at least 1 university course
- [ ] Has at least 1 killer app that only Pledge can do (hot reload + safety + bare metal)
- [ ] Community of 1000+ active developers

---

## Timeline (Rough Estimates)

| Phase | Duration | Cumulative |
|---|---|---|
| Phase 1 — Foundation | 1 month remaining | 1 month |
| Phase 2 — Safety | 6 months | 7 months |
| Phase 3 — Power | 6 months | 13 months |
| Phase 4 — Performance | 12 months | 25 months |
| Phase 5 — Platform | 6 months | 31 months |
| Phase 6 — Self-Hosting | 6 months | 37 months |
| Phase 7 — Ecosystem | 12 months | 49 months |
| Phase 8 — World Class | 12+ months | 61+ months |

**~5 years to world-class. ~2 years to something people can use.**

---

## Decision Framework

When choosing what to work on next, ask:

1. **Does it differentiate Pledge from Rust/Zig/C?** If no, deprioritize.
2. **Does it require custom infrastructure?** If yes, it's core to our mission.
3. **Does it enable a use case no other language handles?** If yes, prioritize.
4. **Does it reduce external dependencies?** If yes, prioritize.
5. **Does it improve developer velocity?** If yes, prioritize.

**Priority order:** Safety > Performance > Power > Platform > Ecosystem
