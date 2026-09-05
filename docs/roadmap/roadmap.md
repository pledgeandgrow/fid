# Fides Roadmap — Milestones to World Domination

> Not a task list. A map of mountains to climb.
> Last updated: August 2026. Based on full codebase analysis.

## Overview

```
Phase 1: Foundation          ← COMPLETE — 137/137 all backends, zero regressions
Phase 2: Safety              ← COMPLETE — compile-time safety + binary hardening
Phase 3: Power               ← COMPLETE — all features work in all backends
Phase 4: Performance         ← MOSTLY COMPLETE — optimizer + LTO + loop-vec + SLP + parallel passes + incremental; AVX2/SSE2 vector codegen + graph-coloring regalloc remaining
Phase 5: Platform            ← MOSTLY COMPLETE — cross-compilation to ARM64/RISC-V/Windows done; DWARF debug info remaining
Phase 6: AI/ML               ← COMPLETE — pipeline, layers, shapes, GPU, quantization, KV cache, DP (epsilon budget tracking), sharding (simulated multi-rank), tape-based autodiff (runtime gradient computation), gradient checkpointing (activation recompute)
Phase 7: Ergonomics          ← MOSTLY COMPLETE — guard, spread, ?., ??, is, contracts, @memo; @differentiable typecheck + runtime autograd done
Phase 7.5: Tier 1 Safety     ← COMPLETE — all safety annotations + @constant_time + @tainted
Phase 7.55: Borrow Checker   ← COMPLETE — Rules 1-3, field-wise, two-phase, NLL, default on, @send/@sync
Phase 7.6: Async/Await       ← MOSTLY COMPLETE — executor, non-blocking poll, async IO, channels, select; state machine transform (stackless coroutines) + PE stack traces remaining
Phase 8A: Vec/Mat Math        ← COMPLETE — vec2/vec3/vec4, mat4, quat, swizzle, operator overloading, shape checking
Phase 8B: ECS/States/Events   ← COMPLETE — component/system/query, state_machine, event handlers
Phase 8C: Shaders/SIMD       ← PARTIAL — SIMD stdlib + SPIR-V GPU codegen; @shader/@simd annotations not implemented
Phase 8D: Game Stdlib         ← COMPLETE — 8 game stdlib modules (test_game_stdlib now passes — &mut T auto-deref fix)
Phase 9: Industry Domains    ← NOT STARTED — only stdlib/banking.fd exists; medical/blockchain/robotics/automotive not built
Phase 10: Self-Hosting       ← IN PROGRESS — SH-6 through SH-9.4 DONE (all 15 modules compile, 37,584 LOC Fides). Bootstrap (SH-9.5) IN PROGRESS: `fides-bootstrap` (Rust-compiled Fides compiler) no longer segfaults on hello.fd — 4 codegen/porting bugs fixed: (1) string padding, (2) auto-deref &mut Vec in infer_expr_type, (3) box_type_unbox Deref returning inner Box directly, (4) box_expr_unbox(receiver) porting bug in check_method_call — receiver was Expr not Box<Expr>, causing double-deref segfault. Now produces false typechecker errors — 3 remaining bugs: (1) unsafe block init tracking, (2) Type::Named("Vec") method dispatch, (3) undefined function lookups. 265/265 Rust unit tests pass, 164/165 normal tests pass on interp+C codegen, 165/165 PE.
Phase 10B: PE Emitter Rewrite ← NOT STARTED — rewrite pe.rs in Fides after C codegen bootstrap. Fix Issues 67-69 first, then rewrite PE emitter in Fides, then switch default backend to PE for zero-dependency deployment. C backend stays as fallback.
Phase 11: Ecosystem          ← IN PROGRESS — LSP (diag+completion+hover+def), formatter, linter, bench, profile
Phase 12: Security Hardening ← MOSTLY COMPLETE — binary hardening + all safety annotations; CFI + full taint tracking + runtime sandbox remaining
Phase 13: World Class        ← IN PROGRESS — type inference done, SLP+loop-vec done, @send/@sync done, debugger done, ark package manager done, scribe doc generator done, veil coverage tool done, green threads remaining
Phase 14: Debugging Tools    ← COMPLETE — interpreter debugger, record/replay, hot-reload, post-mortem, native debugger
Phase 15: Compete Where We Lose ← NOT STARTED — win in web/data-science/prototyping/games without adding a runtime (arena allocators, comptime frameworks, ergonomic modes)
```

### Verified Test Baseline (Aug 2026)

| Backend | Result | Notes |
|---------|--------|-------|
| Unit tests (`cargo test --lib`) | **265/265** | All passing (incl. 47 cross-compilation tests + 7 incremental compilation) |
| Harness tests | **8/8** | All passing |
| Interpreter | **164/165** normal | 1 fail: test_compiler_sized (user-added, unrelated). |
| C Codegen | **164/165** matched | 1 fail: test_compiler_sized. Binary hardening enabled. |
| PE Emitter | **165/165** valid | All normal tests pass. |
| Skip list | **4** tests | 3 C codegen missing-builtins (kv_cache, quantization, dp) + 1 design difference (dlopen). Was 42 — 37 stale + 3 more after runtime effects fix. |
| Known-fail | **0** tests | All previously known-fail tests now pass on all 3 backends. |
| Expected-fail | **35** tests | Correctly fail — error detection tests. |
| Total `.fd` test files | **218** | 195 in root + 23 in subdirs (capability/effects/regions/safety) |
| Source files | **51** `.rs` | 86,742 LOC Rust |
| Stdlib files | **69** `.fd` | 18,242 LOC Fides |

### Security Hardening (New — Aug 2026)

All C codegen output is now compiled with binary hardening flags by default:
- `-fstack-protector-strong` (stack canaries)
- `-fPIE -pie` (ASLR / position-independent executable)
- `-Wl,-z,relro -Wl,-z,now` (full RELRO — prevents GOT overwrites)
- `-Wl,-z,noexecstack` (NX stack — prevents shellcode injection)
- `-fno-strict-overflow` (compiler doesn't assume no overflow)

New security annotations:
- `@constant_time` — prevents timing side-channel attacks (Spectre, Meltdown)
- `@tainted` — prevents injection attacks (SQL, shell, HTML, path traversal)

See `docs/security.md` for the full security hardening guide.

---

## Phase 1 — Foundation (COMPLETE)

**Goal:** A working language with basic features, custom codegen, no external dependencies.

**Status:** Complete — 164/165 on interp+C codegen, 165/165 PE. Zero regressions (1 fail: test_compiler_sized, user-added).

### Milestones
- [x] Lexer, parser, AST
- [x] Basic typechecker
- [x] IR (SSA-based, typed)
- [x] Lowering (AST → IR)
- [x] Monomorphization
- [x] Interpreter (IR execution)
- [x] PE emitter (IR → x86-64 native binaries)
- [x] C codegen (IR → C → gcc/clang)
- [x] Generics, enums, structs, closures
- [x] Match, if/else, loops, defer
- [x] Result/Option + ? operator
- [x] try { } blocks
- [x] References (& / &mut)
- [x] Traits (cap) — basic trait system with static dispatch
- [x] String, Vec, HashMap — comprehensive methods
- [x] Drop trait — compiler-inserted drops at function exit (Issue 59)
- [x] Auto-deref for `&T`/`&mut T` method calls (Issue 45)
- [x] `val` immutability enforcement (Issues 46-47)
- [x] Non-exhaustive `match` warnings (Issue 49)

### What's Left
**Nothing.** Phase 1 is fully complete.

---

## Phase 2 — Safety (COMPLETE)

**Goal:** Compile-time safety guarantees that eliminate entire exploit classes.

**Status:** Complete — all safety features work in all backends. Binary hardening
added in Aug 2026 update.

### Milestones
- [x] Bounds checking (F62) — always on, skippable in `unsafe {}`
- [x] Checked arithmetic (F64) — overflow = panic, always on
- [x] `@wrapping` annotation (F65) — escape hatch for wrap-around arithmetic
- [x] Init guarantee (F63) — `var` keyword (without initializer) + definite assignment analysis
- [x] Region-based memory management — freed_regions tracking, escape detection
- [x] `unsafe {}` blocks — raw pointer ops, FFI, skip bounds checks
- [x] Raw pointer types — `*mut T` / `*const T`
- [x] Sandbox blocks — `sandbox { }` strips all capabilities
- [x] Capability system — `with` clauses, grant/deny/propagation
- [x] Typed strings (F67) — SQLString, ShellString, HTMLString, JSONString, PathString
- [x] Parameterized queries (F68) — no string concat on typed strings
- [x] Binary hardening — stack canaries, PIE, RELRO, NX stack (Aug 2026)
- [x] `@constant_time` — side-channel defense (Aug 2026)
- [x] `@tainted` — injection defense (Aug 2026)
- [x] Interpreter UAF detection — `freed_addresses` + `region_addresses` tracking
  in `src/interp.rs`. `RegionEnd` marks all addresses in a region as freed.
  `Load`/`Store` check against `freed_addresses` and panic on use-after-free.
  Re-entrant region blocks handled with unique depth-keyed region keys.

### What's Left
**Nothing for Phase 2.** All safety features are implemented and tested.

---

## Phase 3 — Power (COMPLETE)

**Goal:** Advanced language features that make Fides expressive.

**Status:** Complete — all power features work in all backends.

### Milestones
- [x] Comptime evaluation — `@comptime` functions, `comptime gen { }`
- [x] Comptime reflection — `@derive(Debug)`, `@derive(Clone)`
- [x] Serde — `@derive(Serialize)`, `@derive(Deserialize)` → JSON
- [x] Conditional compilation — `@cfg(platform = "linux")`, `@cfg(debug)`
- [x] Dynamic loading — `p_dl_open`, `p_dl_sym`, `p_dl_call`
- [x] Pipeline operator — `|>` chaining
- [x] Guard clauses — `guard =>` early return
- [x] Spread operator — `..` for array/struct spreading
- [x] Optional chaining — `?.`
- [x] Null coalescing — `??`
- [x] Type checking — `is` operator
- [x] Design by Contract — `given`/`ensure` preconditions/postconditions
- [x] Memoization — `@memo` annotation
- [x] Iterator adapters — `for x in vec`, `for k in map`, `for i in 0..N`
- [x] Error chaining — `?` on Option/Result, `try { }` blocks
- [x] Generic enums/structs — `Option<T>`, `Result<T, E>`, `struct Foo<T>`

### What's Left
**Nothing for Phase 3.** All power features are implemented.

---

## Phase 4 — Performance (MOSTLY COMPLETE)

**Goal:** Optimized codegen competitive with gcc -O2.

**Status:** Mostly complete. Optimizer enabled by default with 23 passes. Loop
vectorization for both constant and variable trip counts. SLP vectorizer.
Parallel function-level optimization. Incremental compilation cache. Borrow-
based alias analysis. Cost model for vectorization profitability.

### Milestones
- [x] Optimizer enabled by default (`--no-opt` to disable)
- [x] `--lto` for aggressive inlining
- [x] CSE/GVN — common subexpression elimination
- [x] Constant folding/propagation
- [x] Dead code elimination
- [x] Strength reduction
- [x] Loop unrolling
- [x] Tail call optimization
- [x] `@deterministic` CSE/GVN — eliminates duplicate deterministic calls
- [x] Loop vectorization (constant + variable trip counts)
- [x] SLP vectorizer
- [x] Peephole optimizer (9 passes)
- [x] Linear scan register allocator
- [x] Codegen benchmark suite — `benchmarks/bench_codegen.sh`
- [x] `fidesc bench` command — timing statistics
- [x] Function-level parallelism (S-1C) — 19/22 passes parallelized
- [x] Incremental compilation cache (S-1B) — skip optimization on cache hit
- [x] Borrow-based alias analysis (I-7A) — eliminate redundant loads
- [x] Cost model for vectorization profitability

### What's Left
- [ ] SSE2/AVX2 instruction emission in codegen (loop-vec produces scalar unrolled code, not SIMD instructions)
- [ ] Graph coloring register allocator (linear scan is "good enough")

---

## Phase 5 — Platform (MOSTLY COMPLETE)

**Goal:** Run on multiple platforms and architectures.

**Status:** Mostly complete. Interpreter and C codegen work on Linux/Windows/macOS.
Native codegen (PE/ELF64/Mach-O) works for x86-64. Cross-compilation to ARM64
and RISC-V is complete — `build --target arm64-linux` and `--target riscv64-linux`
produce valid ELF64 executables with zero external tools. Windows cross-compilation
via `--target x86-64-windows` also works. WASM, bare metal, boot sectors,
interrupt handlers, and MMU all supported.

### Milestones
- [x] Linux support — ELF64 emission, runtime_linux.rs
- [x] Windows support — PE emission, Windows API calls through IAT
- [x] macOS support — Mach-O emission
- [x] C codegen — portable C output, compiled with gcc/clang
- [x] Cross-compilation — `--target <triple>` for C codegen
- [x] **Cross-compilation (native) — `--target arm64-linux|riscv64-linux|x86-64-windows`** with zero external tools
- [x] WASM codegen — `fidesc build --target wasm`
- [x] Bare metal — `fidesc build --bare` (flat binary, no OS)
- [x] Boot sector — `fidesc build --boot` (512-byte boot sector)
- [x] Interrupt handlers — `@interrupt` annotation
- [x] MMU/page tables — `@mmu` annotation
- [x] ARM64 codegen — `src/codegen_arm64.rs` + `src/arm64_enc.rs` + `src/runtime_arm64.rs`
- [x] RISC-V codegen — `src/codegen_riscv.rs` + `src/riscv_enc.rs` + `src/runtime_riscv.rs`
- [x] Hot reload — `src/hotreload.rs` (file watcher + recompile)
- [x] LSP server — `fidesc lsp` (JSON-RPC over stdio)

### What's Left
- [ ] DWARF debug info emission (no debug info in ELF64/Mach-O output)
- [ ] ARM64/RISC-V runtime testing on real hardware (QEMU)

---

## Phase 6 — AI/ML (MOSTLY COMPLETE)

**Goal:** First-class AI/ML support.

**Status:** Complete. Tensor operations, quantization, KV cache, GPU
codegen, shape verification, tape-based autodiff (runtime gradient
computation), gradient checkpointing (activation recompute), DP epsilon
budget tracking, and tensor parallelism (simulated multi-rank) all work.

### Milestones
- [x] Pipeline operator — `|>` chaining
- [x] Layer/model syntax — `layer`, `model` declarations
- [x] Shape types — `[N, M]` dimensional types
- [x] Tape-based autodiff — `src/autograd.rs` + `src/interp.rs` (typechecker verifies `@differentiable`, interpreter computes gradients via tape-based reverse-mode AD)
- [x] GPU kernels — SPIR-V codegen (`src/gpu.rs`)
- [x] Quantization — `stdlib/quant.fd` (real INT8 quantize/dequantize/qmatmul in interpreter)
- [x] Tensor operations — `src/tensor.rs`
- [x] SIMD — `stdlib/simd.fd` (basic)
- [x] KV cache — `region kv_cache r with budget(N)` (ML9)
- [x] Differential privacy — `@dp(epsilon=N)` annotation + noise builtins + epsilon budget tracking (ML12)
- [x] Tensor parallelism — `@shard` annotation + comm builtins + simulated multi-rank runtime (ML13)
- [x] Gradient checkpointing — `@checkpoint` annotation + runtime activation recompute (ML11)
- [x] LoRA/adapter — `@adapter` annotation (ML14)

### What's Left
- [ ] **C codegen autograd** — `GradBlock`/`Backward` instructions are handled in interpreter but C codegen does not implement the autograd tape runtime.
- [ ] **Multi-rank TP** — `p_all_reduce` etc. are identity in single-rank mode. Real multi-rank distributed execution not implemented.

---

## Phase 7 — Ergonomics (MOSTLY COMPLETE)

**Goal:** Developer-friendly language features.

**Status:** Mostly complete. Most ergonomics features done.

### Milestones
- [x] Guard clauses (F6) — `guard =>`
- [x] Spread operator (F7) — `..`
- [x] Optional chaining (F8) — `?.`
- [x] Null coalescing (F9) — `??`
- [x] Type checking (F11) — `is`
- [x] Design by Contract (F10) — `given`/`ensure`
- [x] Memoization (F12) — `@memo`
- [x] Tuple destructuring — `val (a, b) = expr`
- [x] File I/O — `stdlib/file.fd`
- [x] `txn {}` blocks — with interpreter rollback
- [x] LogicalTime — `stdlib/time.fd`
- [x] Parser error recovery — sync tokens, multi-error reporting (Aug 2026)
- [x] More Result/Option methods — map, and_then, or_else, expect, unwrap_or_else (stdlib/core.fd)

### What's Left
- [x] `@differentiable` (F14) — annotation verified by typechecker (ML3: compile-time proof of differentiability)
- [x] Autograd transform — tape-based reverse-mode AD in interpreter. `grad(var) { ... backward(loss) }` computes gradients. `p_grad_get` retrieves results.

---

## Phase 7.5 — Tier 1 Safety (COMPLETE)

**Goal:** Domain-specific safety annotations for regulated industries.

**Status:** Complete — all safety annotations implemented and tested.

### Milestones
- [x] `@deterministic` — no IO/Net/floating-point, transitive
- [x] `@conserve` — no money creation/destruction, transitive
- [x] `@linear` — resource single-use (no duplication)
- [x] `@audit` — full param trail logging
- [x] `@gas(N)` — compile-time gas budget
- [x] `@crashfree` — no panic (no unwrap, no div/mod, no while)
- [x] `@medical` — IEC 62304 (no-alloc + no-recursion + crashfree + audit)
- [x] `@safety-critical` — ISO 26262 / DO-178C (no-alloc + no-recursion + crashfree + no-indirect)
- [x] `@autosar` — automotive safety (no-alloc + no-recursion + crashfree)
- [x] `@no-alloc` — no dynamic allocation
- [x] `@no-recursion` — no direct/indirect self-calls
- [x] `@no-unbounded` — no while loops
- [x] `@no-indirect` — no function pointers / dynamic dispatch
- [x] `@constant_time` — side-channel defense (Aug 2026)
- [x] `@tainted` — injection defense (Aug 2026)
- [x] Composable safety primitives — `@no-alloc @no-recursion` etc.
- [x] Transitive enforcement — safety-annotated functions can only call safety-annotated functions

### What's Left
**Nothing for Phase 7.5.** All safety annotations are implemented.

---

## Phase 7.55 — Borrow Checker (COMPLETE)

**Goal:** Prevent aliasing and data races at compile time.

**Status:** Complete — Phases 1-6 done, plus @send/@sync for cross-thread safety.

### Milestones
- [x] Rule 1: No `&mut x` while any borrow active
- [x] Rule 2: No `&x` while `&mut x` active
- [x] Rule 3: No mutation while any borrow active
- [x] Field-wise borrows — `&a.x` and `&mut a.y` can coexist
- [x] Two-phase borrows — temporary shared borrows during mutation
- [x] NLL (Non-Lexical Lifetimes) — borrows released at last use, not scope end
- [x] Default on — `--borrowck=off|warn|on` (on by default)
- [x] `unsafe {}` blocks skip borrow checking
- [x] Multi-threaded data race prevention (@send/@sync) — IMPLEMENTED
  `@send` annotation on structs marks types safe to send across threads.
  `@sync` annotation marks types safe to share across threads. Compile-time
  checks: `spawn` captures must be `@send`, `p_chan_send` values must be `@send`,
  `p_thread_spawn` args must be `@send`. References (&T) and raw pointers are
  never `@send`. Primitives are always `@send` and `@sync`.
- [x] Closures in borrow checker — `Expr::Closure` handled in `borrowck.rs`.
  Captured variables are correctly tracked. Closure body is checked in a new
  scope. Closure params are local to the closure scope (no borrow tracking
  needed for params).

---

## Phase 7.6 — Async/Await (MOSTLY COMPLETE)

**Goal:** Concurrent programming with async/await, channels, and select.

**Status:** Mostly complete. Real OS thread executor with non-blocking poll.
Channels with select statement. Async IO, bounded MPSC channels, and
stack traces on panic all working. State machine transform (stackless
coroutines) deferred — it's an optimization, not a feature.

### Milestones
- [x] `async fn` — async function syntax
- [x] `await <expr>` — await Future values
- [x] `spawn <expr>` — spawn tasks
- [x] Executor runtime (A1) — `p_executor_new/spawn/block_on/run_all/poll/is_ready`
- [x] Non-blocking poll (A2) — `p_executor_poll_all`, `p_task_poll_nonblocking`
- [x] Round-robin scheduling — concurrent task interleaving
- [x] Async IO (A3) — `p_net_connect/read/write/close`, `p_fs_read_async`
- [x] Bounded MPSC channels — `p_chan_create/send/recv/try_recv/close/is_closed`
- [x] Channel wrapper — `stdlib/channel.fd` + `stdlib/sync.fd`
- [x] `select` statement — `select { case ch => x => { ... } default => { ... } }` (Aug 2026)
- [x] Stack traces on panic — interpreter call_stack tracking

### What's Left
- [ ] State machine transform (stackless coroutines) — deferred to post-self-host
- [ ] PE emitter stack traces — minimal stack walker (C codegen + interpreter have stack traces)

---

## Phase 8 — Game Development (MOSTLY COMPLETE)

**Goal:** Game development features (ECS, math, rendering, audio).

**Status:** ECS/states/events done. Game stdlib done. Vec/mat math and
shaders remaining.

### Phase 8A: Vec/Mat Math (COMPLETE)
- [x] vec2/vec3/vec4 types — structs with add/sub/scale/dot/cross/length/normalize/lerp methods
- [x] mat4 type — 4x4 matrix with identity/mul/transform_vec4/transform_vec3
- [x] Quaternion type — from_axis_angle, rotate_vec3
- [x] Swizzle operators (xyzw, rgba, stpq) — 2-4 components, reordering, repetition
- [x] Operator overloading (+, -, *, / on vec types) — compiler rewrites to method calls
- [x] Free functions: radians, degrees, lerp, distance, translate, mat_scale
- [x] Shape checking (vec3 + vec2 → compile error) — enforced in typechecker.rs
- [ ] SIMD codegen (SSE/AVX/NEON) — currently uses scalar ops

### Phase 8B: ECS/States/Events (COMPLETE)
- [x] `component` declaration — desugars to struct
- [x] `system` declaration — desugars to function
- [x] `query(CompA, CompB)` — iterates entities with all components
- [x] `state_machine` — desugars to enum + handle function
- [x] `on event_handler` — desugars to `__on_<event>` function
- [x] ECS runtime — `__ecs_create_entity/has/get/add/remove`
- [x] Test: `test_phase8b.fd` (passes on interp+PE, fails on C codegen)

### Phase 8C: Shaders/SIMD (PARTIAL)
- [x] SIMD stdlib — `stdlib/simd.fd` (basic operations)
- [ ] `@shader` annotation — GPU shader generation
- [ ] `@simd` annotation — auto-vectorization hints
- [ ] Compute shaders — GPU compute kernel generation

### Phase 8D: Game Stdlib (COMPLETE)
- [x] `stdlib/tilemap.fd` — tile-based maps
- [x] `stdlib/sprite.fd` — sprite rendering
- [x] `stdlib/flowfield.fd` — flow field pathfinding
- [x] `stdlib/streaming.fd` — chunk streaming
- [x] `stdlib/scene.fd` — scene management
- [x] `stdlib/physics.fd` — physics simulation
- [x] `stdlib/audio.fd` — audio playback
- [x] `stdlib/input.fd` — input handling

### What's Left
- [ ] SIMD codegen for vec/mat (8A extension) — SSE/AVX/NEON instruction emission
- [ ] Shader generation (8C) — @shader/@simd annotations, compute shaders (not implemented)
- [ ] Fix test_game_stdlib — type error on &mut Vec method calls (in skip list)

---

## Phase 9 — Industry Domains (NOT STARTED)

**Goal:** Domain-specific features for regulated industries.

**Status:** Not started (partial). Safety annotations exist (`@medical`, `@autosar`,
`@safety-critical`) and `stdlib/banking.fd` provides a minimal banking layer
with `@conserve`/`@linear` Money. But the remaining industry-specific stdlib
modules (medical, blockchain, robotics, automotive, aerospace, energy,
serverless) are not built.

### Planned
- [x] Banking — `stdlib/banking.fd` with linear Money + @conserve (partial: accounts, deposit, withdraw, interest)
- [ ] Banking — batch processing, COBOL interop, SWIFT messaging
- [ ] Medical — IEC 62304 compliance tooling, audit trail verification
- [ ] Blockchain — `@emit` event logging, smart contract verification
- [ ] Robotics — `@realtime`, `@interrupt`, `@firmware` annotations
- [ ] Automotive — `@can-bus`, CAN frame types, ECU abstraction
- [ ] Aerospace — `@dal-a`, `@redundant`, `@partitioned` annotations
- [ ] Energy — `@grid-safe`, grid protocol types
- [ ] Serverless — `@serverless`, cold-start optimization

### What's Left
**Most of Phase 9.** The safety annotations and a partial banking stdlib are
in place but the industry-specific tooling and remaining stdlib modules need
to be built.

---

## Phase 10 — Self-Hosting (IN PROGRESS — session 7)

**Goal:** Rewrite the Fides compiler in Fides.

**Detailed plan:** See `selfhost-roadmap.md` for the full self-hosting
roadmap. All 18 modules have been rewritten in Fides (39.5K LOC). The
self-hosted compiler successfully compiles itself (EXIT 0). The PE emitter
has been ported with 25+ runtime stubs. Comptime evaluation has been fully
ported.

**Status:** All 3 language-level blockers resolved. All 25 self-host
modules rewritten. Self-compilation works (EXIT 0). PE emitter works for
simple programs. Comptime eval fully ported. **Bootstrap fixpoint achieved
(session 15): fides-c2.c == fides-c3.c (SHA256-identical, 701,100 lines).**

### Milestones
- [x] `unsafe {}` blocks — with unsafe_depth tracking
- [x] Raw pointer types — `*mut T` / `*const T` with unsafe enforcement
- [x] Heap allocation builtins — `stdlib/alloc.fd`
- [x] Field-level privacy — `is_pub` on Param
- [x] Vec/HashMap/String stdlib migration — implemented in Fides stdlib
- [x] Arena allocator — `p_arena_new`, `p_arena_alloc`
- [x] Rc/Arc — reference counting (partial)
- [x] **Typed enum payloads** — match bindings keep declared payload type
- [x] **Real generics at value level** — `Vec<T>`/`HashMap<K,V>` carry actual
      element types from `let` annotations; `Vec::new()` honors type hints
- [x] **Recursive types via `Box<T>`** — `enum Expr { Add(Box<Expr>, Box<Expr>) }`
      compiles and runs; match lowerer dereferences Box pointers
- [x] **All 3 blockers pass on all 3 backends** (interpreter, C codegen, PE emitter)

### Self-Hosting Blockers (all resolved)

Three blockers were identified that prevented the Fides compiler from being
rewritten in Fides. All are now resolved:

1. **[DONE] Typed enum payloads.** Enum variant payloads lost their type on
   match — bindings were always `i32`. Fixed by adding `enum_variants` /
   `enum_type_params` maps to the typechecker, `pattern_binding_types()`
   resolving payload types with type-param substitution, and
   `pattern_binding_ir_types()` in the lowerer using the real IR type
   (aggregates as `Ptr(Struct(..))` not bare `I64`).
   Test: `tests/test_enum_payload_types.fd`.

2. **[DONE] Real generics at the value level.** `Vec<T>`, `Option<T>`,
   `Result<T,E>` stored `i64` regardless of `T`. `Vec::new()` always returned
   `Vec<i64>`. Fixed by threading a `let_hint` through the lowerer so
   `val v: Vec<Token> = Vec::new()` picks up the element type, making
   `Vec::get()` return `Ptr(Struct(..))` for struct elements via `word_repr()`,
   and adding `Vec<X>`/`HashMap<X,Y>` compatibility in `types_compatible()`.
   Test: `tests/test_generic_vec_typed.fd`.

3. **[DONE] Recursive types.** No true `Box<T>` indirection existed, making
   recursive data structures (ASTs) impossible. Fixed by adding
   `maybe_deref_box()` to the match lowerer — when the scrutinee is a
   `Box<T>` (either `Ptr(Ptr(Struct(..)))` from `Box::new` or
   `Generic("Box", [Struct(..)])` from a typed parameter), it loads through
   the Box once to get the inner struct pointer before pattern matching.
   Test: `tests/test_recursive_box.fd` — recursive `eval` and `count_nodes`
   on `(1+2)*(3+4)` = 21 with 7 nodes.

### What's Left Before Self-Host Can Start

- [x] **Stdlib gaps filled** — str_bytes, str_from_bytes, str_split, str_split_char,
      str_trim_start, str_trim_end, str_slice, str_parse_int, str_parse_hex,
      str_parse_int_range, str_find, str_rfind, str_replace, str_contains,
      str_starts_with, str_ends_with, str_to_upper, str_to_lower, str_equal,
      str_compare, str_concat, str_len, str_is_empty, int_to_str, byte_to_str
- [x] **Binary/byte IO** — file_write_bytes, file_read_bytes, file_write_raw,
      file_delete, file_size (with runtime support: p_file_delete, p_file_size)
- [ ] **Native runtime stubs** — argv, file I/O, process execution in native
      backends (`runtime_linux.rs`, ARM64/RISC-V) — not needed for C codegen path

### Self-Host Rewrite Plan

| Phase | Description | LOC | Effort |
|-------|-------------|-----|--------|
| **1: Stdlib gaps** | String builder, char iteration, parsing | ~500 | 1-2 sessions |
| **2: Lexer** | Rewrite lexer.rs in Fides | ~800 | 1 session |
| **3: Parser** | Rewrite parser.rs in Fides | ~2000 | 2-3 sessions |
| **4: Typechecker** | Rewrite typechecker.rs in Fides | ~3000 | 3-4 sessions |
| **5: Lowerer** | Rewrite lower.rs in Fides | ~1500 | 2 sessions |
| **6: IR/PIR** | Rewrite ir.rs + pir.rs in Fides | ~2000 | 2 sessions |
| **7: Optimizer** | Rewrite opt.rs in Fides | ~3000 | 3-4 sessions |
| **8: Codegen** | Rewrite codegen_x64.rs in Fides | ~1500 | 2 sessions |
| **9: Bootstrap** | Fides compiler compiles itself | — | 1 session |
| **Total** | | ~14,300 | ~15-20 sessions |

---

## Phase 10B — PE Emitter Rewrite in Fides (IN PROGRESS — session 7)

**Goal:** Rewrite the PE emitter (`src/pe.rs`) in Fides, making the
self-hosted compiler fully self-contained with zero external dependencies.

**Strategy:** C codegen bootstrap first, PE emitter rewrite second.

This follows the same path Zig took: bootstrap with C, self-host the
compiler, then make the self-hosted compiler the default. The C backend
stays as a fallback.

### Why C Codegen Bootstrap First

The PE emitter is a **nice-to-have for the end state**, not the bootstrap
path. During self-hosting development, you want:

1. **Fast iteration** — C codegen compiles fast, the C compiler gives errors
2. **Debuggability** — gdb, valgrind, ASan all work on C codegen output
3. **Correctness** — C codegen has 164/165 test parity; PE has 3 critical bugs

The PE emitter's value proposition is "zero external dependencies." But that
only matters for **deployment**, not for **development**.

### Prerequisites (Must Fix Before PE Rewrite)

Fix the 3 critical PE emitter bugs (Issues 67-69) that are self-hosting
blockers for the PE path:

- [ ] **Issue 67: `val` inside loop corrupts loop variable** — PE emitter
      allocates `val`'s stack slot at the same offset as the loop variable.
      A self-hosted lexer/parser is full of `val` declarations inside
      `while`/`for` loops. Fix: stack slot allocation in PE lowering pass.
- [ ] **Issue 68: `str_split` returns empty Vec in PE** — A self-hosted
      compiler needs `str_split` for line-by-line tokenization. Fix: PE
      emitter's `str_split` runtime implementation.
- [ ] **Issue 69: Recursive `Box<T>` segfaults in PE** — A self-hosted
      compiler's AST is inherently recursive (`Box<Expr>`, `Box<Stmt>`).
      Fix: stack frame sizing or Box pointer handling in PE emitter.
- [ ] **Native runtime stubs** — argv, file I/O, process execution in
      `runtime_linux.rs` for PE emitter path. Not needed for C codegen
      (libc provides these).

### Rewrite Plan

| Step | Description | LOC | Effort |
|------|-------------|-----|--------|
| **1: Fix Issues 67-69** | Fix PE emitter bugs in Rust (`src/pe.rs`) | — | 2-3 sessions |
| **2: Native runtime stubs** | argv, file I/O, process exec in `runtime_linux.rs` | ~500 | 1-2 sessions |
| **3: Rewrite pe.rs in Fides** | PE emitter in Fides, using self-hosted compiler | ~2000 | 3-4 sessions |
| **4: Switch default backend** | Self-hosted compiler defaults to PE emitter | — | 1 session |
| **5: Keep C backend as fallback** | `--backend c` flag for platforms without PE support | — | trivial |
| **Total** | | ~2500 | ~7-10 sessions |

### End State

After Phase 10B:

- `fidesc build` produces native executables with **zero external tools**
  (PE emitter, no gcc/clang dependency)
- `fidesc build --backend c` falls back to C codegen (for platforms where
  PE emission is unsupported or for debugging with gdb/valgrind)
- The self-hosted compiler is fully self-contained: Fides source → Fides
  compiler → native binary, no C compiler in the toolchain

The C backend stays as a permanent fallback, exactly like Zig's C backend.

---

## Phase 11 — Ecosystem (IN PROGRESS)

**Goal:** Developer tooling that matches Rust/Go/Zig.

**Status:** In progress. LSP server with diagnostics, completion, hover, and
goto-definition. Code formatter, linter, and benchmark command all work.

### Milestones
- [x] LSP server — `fidesc lsp` (JSON-RPC over stdio)
- [x] LSP diagnostics — `publishDiagnostics` on didOpen/didChange
- [x] LSP completion — `textDocument/completion` (keywords + symbols)
- [x] LSP hover — `textDocument/hover` (type signatures)
- [x] LSP goto-definition — `textDocument/definition`
- [x] LSP symbol accessors — `get_funcs`, `get_structs`, `get_methods`, etc.
- [x] Code formatter — `fidesc fmt <file.fd> [--check]` (`src/fmt.rs`, 972 LOC)
- [x] Linter — `fidesc lint <file.fd>` (unused functions, unused imports)
- [x] Benchmark — `fidesc bench <file.fd> [-n N] [--ccompile]`
- [x] Diagnostic system — error codes (E001-E120), source context, colors
- [x] "Did you mean?" suggestions — Levenshtein distance for typos
- [x] Parser error recovery — multi-error reporting with sync tokens
- [x] Generic enums/structs — `Option<T>`, `Result<T, E>`
- [x] New stdlib modules — math, env, args, process, regex, channel, sync

### What's Left
- [ ] LSP rename — `textDocument/rename` (rename symbol across file)
- [ ] LSP code actions — quick fixes, refactorings
- [x] Package manager — `ark` binary, `ark.toml` manifest, path + git deps, lock file
- [ ] Profiler — `fidesc profile <file.fd>` (timing per function)
- [x] Documentation generator — `scribe` binary (HTML/Markdown from `///` comments)
- [x] Coverage tool — `veil` binary (function/line/instruction coverage, text + HTML reports)

---

## Phase 12 — Security Hardening (MOSTLY COMPLETE)

**Goal:** Eliminate entire classes of binary-level and language-level exploits.

**Status:** Mostly complete. Binary hardening on all C codegen output,
`@constant_time` and `@tainted` annotations implemented and tested. See
`roadmap/security.md` for the full exploit class coverage matrix and
annotation reference.

### Milestones
- [x] Binary hardening — stack canaries, PIE, RELRO, NX stack (all by default)
- [x] `@constant_time` — prevents timing side-channel attacks
- [x] `@tainted` — prevents injection attacks (SQL, shell, HTML, path)
- [x] `@crashfree` — no panic guarantee (no unwrap, div, mod)
- [x] `@safety-critical` — ISO 26262 / DO-178C (combines @no-alloc + @no-recursion + @crashfree + @no-indirect)
- [x] `@medical` — IEC 62304 (combines @no-alloc + @no-recursion + @crashfree + @requires-audit)
- [x] `@deterministic` — reproducible output (no IO/Net/floating-point)
- [x] `@conserve` — money conservation (linear types)
- [x] `@gas(N)` — compile-time computation budget
- [x] `@linear` — resource single-use (prevents duplication)
- [x] `@wrapping` — overflow escape hatch
- [x] `@no-alloc` / `@no-recursion` / `@no-unbounded` / `@no-indirect` — composable safety primitives
- [x] `unsafe {}` blocks — escape hatch for raw pointers, bounds skip, FFI
- [x] `docs/security.md` — comprehensive security hardening guide
- [x] Test coverage — `test_constant_time.fd` (pass), 2 expected-fail tests

### What's Left (from security.md "Not Yet Prevented")
- [ ] CFI (Control Flow Integrity) — forward-edge protection for indirect calls
- [ ] Full taint propagation — data-flow tracking (currently just sink detection)
- [ ] Runtime sandbox — capability revocation in interpreter
- [ ] Runtime resource limits — memory/time enforcement
- [x] ~~Data race prevention~~ — `@sync`/`@send` done in Phase 7.55

---

## Phase 13 — World Class (IN PROGRESS)

**Goal:** Transformative features that make Fides a credible alternative to
Rust, Go, and C for real-world projects.

### Planned

#### 13.1 Green Threads / Goroutine-Style Concurrency — ❌ NOT DONE
- Stackful coroutines using `makecontext`/`swapcontext` or assembly stack switching
- Work-stealing scheduler (like Go's runtime)
- `go fn()` syntax (or reuse `spawn`)
- Integration with channels for CSP-style concurrency

**Effort:** 1-2 months
**Files:** `src/runtime_linux.rs`, `src/interp.rs`, `src/codegen_c.rs`, `src/ast.rs`, `src/parser.rs`

#### 13.2 Type Inference — ✅ DONE
- Bidirectional type inference in typechecker — DONE
- Infer variable types from initializer expressions — DONE (contextual literal hints)
- Infer function return types from body — DONE (`-> _` syntax, `Type::Infer`)
- Keep explicit annotations optional (for documentation) — DONE

**Effort:** 2-4 weeks (completed)
**Files:** `src/typechecker.rs`, `src/ast.rs`, `src/parser.rs`, `src/ir.rs`, `src/mono.rs`

#### 13.3 Auto-Vectorization — ✅ DONE (partial)
- Loop vectorization for variable trip counts — NOT DONE (constant trips only)
- Proper SLP vectorization (correct semantics) — DONE (`pass_slp_vectorize`)
- SSE2/AVX2 instruction emission in codegen_x64.rs — DONE (SSE2: movupd, addpd, subpd, mulpd, divpd)
- Cost model to decide when vectorization is profitable — NOT DONE

**Effort:** 1-2 months (SLP + loop-vec complete; variable trips + cost model remaining)
**Files:** `src/opt.rs`, `src/codegen_x64.rs`

#### 13.4 Package Manager — ✅ DONE (basic, registry pending)
- `ark.toml` manifest format — DONE (custom TOML parser, zero deps)
- Path dependencies (files and directories) — DONE
- Git dependencies (clone with `--depth 1`, branch/tag support) — DONE
- Lock file (`ark.lock` with resolved commit hashes) — DONE
- Commands: init, add, install, remove, list, lock, update — DONE
- Registry (ark.dev) — NOT DONE (search, publish, version resolution)
- Semver dependency resolution — NOT DONE
- Integration with `fidesc build` (auto-install deps) — NOT DONE
- Workspace support (multi-package repos) — NOT DONE

**Effort:** Basic version done. Registry: 1-2 months more.
**Files:** `src/ark.rs`, `src/bin/ark.rs`

#### 13.5 Data Race Prevention (@sync / @send) — ✅ DONE
- `@sync` annotation on types (like Rust's `Sync` trait) — DONE
- `@send` annotation on types (like Rust's `Send` trait) — DONE
- Compile-time check: no sharing of non-`@sync` types across threads — DONE
- Compile-time check: no sending of non-`@send` types to other threads — DONE

**Effort:** 2-3 weeks
**Files:** `src/typechecker.rs`, `src/ast.rs`

---

## Phase 14 — Debugging Tools (COMPLETE)

**Goal:** World-class debugging support — interactive debugger, record/replay,
hot-reload, post-mortem, and native code debugging.

**Status:** Complete — all debugging tools implemented and tested.

### Milestones
- [x] Interpreter debugger — `fidesc debug <file.fd>` (`src/debugger.rs`)
  - Breakpoints: `break <func>[:<line>]`, `delete <n>`, list breakpoints
  - Stepping: `step` (into calls), `next` (over calls), `continue`
  - Variable inspection: `print <var>`, `print` (all locals), `info locals`
  - Source display: `list` (source with `→` marker at current line)
  - IR display: `ir` (full IR with `→` at current instruction), `ir current`
  - Call stack: `backtrace` / `bt`
  - Effects panel: `effects` (function effects + annotations)
  - Regions panel: `regions` (memory regions: Heap/Stack, stack allocation count)
  - Capabilities panel: `capabilities` (annotations + required capabilities from effects)
  - Watch variables: `watch <var>` (print on change)
  - Function list: `info functions`
  - Help: `help`
- [x] Record/replay — `fidesc replay <trace.fdtrace>`
  - Trace format: `func_name block_id inst_id source_line [value]`
  - `save_trace` / `load_trace` / `replay_trace` in `src/debugger.rs`
  - `TraceEntry` struct in `src/interp.rs`
  - `record` command in debugger starts recording
- [x] Hot-reload — `reload` command in debugger
  - Re-reads source file, recompiles, restarts execution
  - Preserves breakpoints and watches across reloads
  - Uses `__debugger_reload__` sentinel to signal outer loop
  - Detects if source actually changed before reloading
- [x] Post-mortem debugging — `fidesc run <file.fd> --post-mortem`
  - Enables trace recording during execution
  - On panic, auto-saves trace to `<file>.fdtrace`
  - Prints replay command for the user
  - Uses `interpret_source_with_trace` in `src/compiler.rs`
- [x] Native code debugger — `fidesc autopsy <file.fd>` (`src/native_debugger.rs`, also `native-debug`)
  - Compiles Fides to native binary (C codegen + gcc -g -O0)
  - Runs under ptrace (Linux)
  - Commands: `run`, `step`, `continue`, `regs`, `quit`
  - Shows CPU registers (RAX-R15, RIP, RSP, RBP)
- [x] Debug hook in interpreter — `debug_hook` field in `Interpreter` struct
  - Closure called before each instruction execution
  - Can block (e.g. waiting for user input) and returns `DebugAction::Continue`,
    `InjectReturn`, or `InjectPanic`
  - `RunResult::Paused` variant added for future pause/resume support

### What's Left
- [ ] Breakpoint injection in native debugger (currently single-step + continue only)
- [ ] Symbol table parsing for native debugger (variable inspection in native code)
- [ ] DWARF debug info emission for external debugger support (gdb/lldb)

---

## Phase 15 — Compete Where We Lose (NOT STARTED)

**Goal:** Win in domains where no-GC / no-RTTI / no-reflection designs
traditionally lose to GC languages (Go, Java, Python). Win *without* adding
a runtime — by moving the work to compile time and arena allocation.

**Core thesis:** Fides doesn't need a GC to compete with GC languages. It
needs (1) arena/region allocators that give GC-like ergonomics with zero
pause, (2) comptime frameworks that replace runtime reflection, and (3)
ergonomic modes that lower the safety barrier during prototyping. Every
"loss" can be turned into a win by moving work from runtime to compile time.

**Prerequisite:** Phase 10 (self-hosting) must be complete — these frameworks
are built in Fides, using Fides, proving the language is productive enough
to build its own ecosystem.

### 15.1 Arena & Region Allocator — GC Ergonomics Without GC

**Problem:** Manual memory management is the #1 reason developers choose Go
over Rust/Fides. "Just allocate and forget" is powerful ergonomics.

**Solution:** Region-based arena allocators. Each scope (HTTP request, game
frame, transaction, test case) gets a region. All allocations go into the
arena. One `p_region_free` call frees everything. Zero pause, zero tracing,
perfect cache locality.

- [ ] **Region allocator stdlib** (`stdlib/arena.fd`)
  - `Arena.new()` → arena handle
  - `Arena.alloc(size)` → pointer (bump allocator, no per-allocation free)
  - `Arena.free()` → free entire arena in one call
  - `Arena.reset()` → mark free but keep backing memory (reuse for next frame)
  - Backed by `p_arena_new` / `p_arena_alloc` (already in runtime)
- [ ] **Region-scoped allocations** — `region "request" { ... }` block
  syntax. All `alloca` inside the region go to the arena. Compiler inserts
  `p_region_free` at block exit (including panic paths via `defer`).
  Already partially implemented (RegionBegin/RegionEnd IR instructions exist).
- [ ] **Region inference** — compiler auto-assigns local allocations to the
  enclosing region when no explicit annotation is given. During prototyping,
  you write `var x = Vec::new()` and the compiler puts it in the function's
  arena. No annotation needed.
- [ ] **Frame allocator** — specialized arena for game loops / event loops.
  `frame_arena { ... }` resets at the end of each iteration. Zero allocation
  cost after warmup (backing memory is reused).
- [ ] **HTTP request arena** — each HTTP request gets its own arena. All
  parsing, routing, handler allocations go into it. Freed at response time.
  This is how nginx and Tempeh (Rust web framework) achieve GC-like
  ergonomics without GC.

**Effort:** 2-3 weeks (stdlib + compiler region inference)
**Files:** `stdlib/arena.fd`, `src/lower.rs` (region inference), `src/interp.rs` (arena runtime)

---

### 15.2 Web Framework — Comptime Replace Reflection

**Problem:** GC languages have ORMs, serializers, middleware, routing — all
built on runtime reflection. Fides has no reflection.

**Solution:** `comptime` + `@derive` replaces reflection entirely. Types are
known at compile time; code is generated at compile time; zero runtime
overhead.

- [ ] **HTTP server** (`stdlib/http.fd` — stub exists, needs implementation)
  - Request/response types with `@derive(Serialize, Deserialize)`
  - Router with compile-time route matching (`comptime match` on path patterns)
  - Middleware chain via effect system (`!IO`, `!Db`, `!Net` enforce ordering)
  - Async I/O compiled to epoll/kqueue state machine (no runtime — each binary
    contains its own event loop, hand-coded via `p_epoll_*` syscalls)
- [ ] **ORM via comptime** (`stdlib/db.fd` — stub exists)
  - `comptime schema("users")` reads table schema at compile time, generates
    typed struct + query functions
  - `@derive(Serialize, Deserialize)` for JSON ↔ struct (already implemented)
  - No runtime reflection — all types and queries are baked into the binary
  - Parameterized queries via typed strings (SQLString already exists)
- [ ] **Template engine via comptime gen**
  - `comptime template("index.html")` parses template at compile time,
    generates typed rendering function
  - No runtime parsing, no runtime type checking — just function calls
- [ ] **Effect-based middleware** — `fn handler(req: Request) !IO !Db { ... }`
  - Compiler enforces that `!Db` middleware runs before `!IO` middleware
  - GC languages can't do this — they have no effect system

**Effort:** 1-2 months (HTTP server + ORM + templates)
**Files:** `stdlib/http.fd`, `stdlib/db.fd`, `stdlib/template.fd` (new), `src/codegen_c.rs` (epoll runtime)

---

### 15.3 Data Science & Scripting — Comptime Replace eval()

**Problem:** Python wins with `eval()`, runtime reflection, REPL, numpy,
pandas, matplotlib. Fides has none of these at runtime.

**Solution:** `comptime` is more powerful than `eval()` — it runs code at
compile time and bakes results into the binary. A REPL uses incremental
compilation (already exists in `src/inc.rs`). Tensor ops use comptime shape
inference.

- [ ] **REPL mode** — `fidesc repl`
  - Uses incremental compilation cache (`src/inc.rs` already exists)
  - Each line compiles to native code and runs immediately
  - No VM — direct native execution with persistent state
  - `:type` command shows inferred types, `:ir` shows generated IR
- [ ] **Comptime CSV/JSON schema inference**
  - `comptime load_csv("data.csv")` parses header at compile time, generates
    typed struct matching columns
  - `comptime load_json("config.json")` infers types from JSON structure
  - No runtime parsing of schema — just typed data loading
- [ ] **Comptime shape inference for tensors**
  - `comptime infer_shapes(model)` traces the computation graph at compile
    time, inserts shape checks, generates specialized kernels
  - This is what JAX and `torch.compile` do, but natively — no Python overhead
  - Fides already has autograd, quantization, KV cache, DP noise
- [ ] **Plotting library** (`stdlib/plot.fd` — new)
  - `comptime gen { ... }` generates SVG/HTML plots at compile time
  - `@derive(Debug)` gives structured data printing
  - Notebook mode: render comptime-evaluated values as HTML
- [ ] **Notebook mode** — `fidesc notebook <file.fd>`
  - Each `///` comment block becomes a markdown cell
  - Each code block is evaluated, output rendered inline
  - Uses incremental compilation — only re-evaluates changed cells
  - No VM, no kernel — just the Fides compiler + HTML renderer

**Effort:** 2-3 months (REPL + comptime data tools + plotting + notebook)
**Files:** `src/main.rs` (REPL mode), `stdlib/plot.fd` (new), `stdlib/data.fd` (new), `src/inc.rs` (REPL integration)

---

### 15.4 Rapid Prototyping — Lower the Safety Barrier

**Problem:** Borrow checker + manual memory management slows iteration.
Python/Go let you write code fast and fix safety later.

**Solution:** Ergonomic modes that lower safety barriers during prototyping,
then tighten them for production. No language changes — just compiler flags.

- [ ] **`--borrowck=warn` prototyping mode** — already exists
  - Borrow check violations become warnings, not errors
  - Code compiles and runs; you get warnings instead of hard stops
  - Switch to `--borrowck=on` (default) when hardening for production
- [ ] **`--no-opt` fast compile** — already exists
  - Skip 23 optimizer passes during prototyping for instant compile
  - Enable `--opt` (default in `build`) for production
- [ ] **Hot-reload JIT (dev-only)** — in roadmap
  - JIT-compile changed functions, hot-swap into running process
  - No VM in production — the JIT is a dev tool only
  - Already partially implemented in debugger (`reload` command)
- [ ] **Region-inferred mode** — `--region-infer`
  - All local allocations auto-assigned to function-level arena
  - No explicit region annotations needed during prototyping
  - Switch to explicit regions when optimizing for zero-alloc
- [ ] **`--unsafe-all` escape hatch** — wrap entire file in `unsafe {}`
  - For quick experiments where safety checks get in the way
  - Produces a warning on every build: "unsafe mode — not for production"
  - Never default, never silent

**Effort:** 1-2 weeks (most infrastructure already exists)
**Files:** `src/main.rs` (flag wiring), `src/borrowck.rs` (warn mode), `src/lower.rs` (region inference)

---

### 15.5 Game Engine Framework — ECS via Comptime

**Problem:** Game engines need entity systems, scripting, asset loading,
hot-reload. GC languages handle this with runtime reflection and embedded
scripting VMs (Lua).

**Solution:** Fides is its own scripting language. ECS layouts are generated
at compile time. Frame arenas handle per-frame allocations. No VM, no
reflection, no embedded scripting language.

- [ ] **Comptime ECS** (`stdlib/ecs.fd` — stub exists)
  - `comptime register_components(Position, Velocity, Health)` generates
    storage layout + query code at compile time
  - `comptime query(Position, Velocity)` generates specialized iteration code
  - No RTTI — the vtable is generated, not queried at runtime
  - Cache-perfect iteration (struct-of-arrays generated at compile time)
- [ ] **Frame-based arena** — from 15.1
  - All per-frame allocations (particles, transforms, debug draws) go into
    frame arena; freed in one shot at frame end
  - This is how Doom Eternal and Zig game engines work
- [ ] **Asset serialization** — `@derive(Serialize, Deserialize)` (done)
  - Load JSON/binary assets into typed structs at runtime
  - No reflection — deserialization code generated at compile time
- [ ] **Scripting via comptime gen**
  - Game scripts are Fides code compiled with `comptime gen { ... }`
  - The "scripting language" is Fides itself — no Lua, no separate VM
  - Scripts can be hot-reloaded via dev JIT (15.4)
- [ ] **Hot-reload dev mode** — reload changed systems without restarting
  - Dev JIT hot-swaps changed functions into running game
  - Production builds are fully AOT-compiled, no JIT

**Effort:** 1-2 months (ECS framework + game loop + asset pipeline)
**Files:** `stdlib/ecs.fd`, `stdlib/game.fd` (new), `src/codegen_c.rs` (frame arena runtime)

---

### 15.6 The Common Thread

Every "loss" is turned into a win by **moving work to compile time**:

| Problem | GC language solution | Fides solution | Phase |
|---------|---------------------|----------------|-------|
| "Don't think about freeing" | Garbage collector | Region/arena allocator (one free call) | 15.1 |
| "Inspect types at runtime" | Reflection | `comptime` + `@derive` | 15.2-15.3 |
| "Run generated code" | `eval()` / VM | `comptime gen { ... }` | 15.3 |
| "Fast iteration" | Dynamic typing | `--borrowck=warn` + `--no-opt` + hot reload | 15.4 |
| "Serialize anything" | Runtime reflection | Compile-time derive (done) | 15.2 |
| "Scripting in games" | Embedded Lua VM | Fides is its own scripting language | 15.5 |
| "Per-frame allocation" | GC collects between frames | Frame arena (one free per frame) | 15.1/15.5 |

**Key insight:** Fides doesn't need to add a runtime to compete with GC
languages. It needs to make its compile-time machinery powerful enough that
you don't miss the runtime. The building blocks already exist (comptime,
effects, regions, derive, incremental compilation). What's missing is the
*framework layer* that makes them ergonomic.

**Risk:** Building all these frameworks is significant work, and ecosystem
size is a moat that GC languages have spent decades building. But the
technical approach is sound — Zig is proving the same thesis (no GC, no
reflection, comptime), and Rust is proving that borrow checking doesn't
prevent productivity once the ecosystem matures.

---

## What NOT to Focus On (Low ROI)

| Feature | Why Skip |
|---------|----------|
| HKT (Higher-Kinded Types) | Rust doesn't have them either. Niche academic feature. |
| Lifetime parameters | Fides uses regions instead. Different approach, don't mix. |
| PGO (Profile-Guided Optimization) | Complex infrastructure, low payoff for small programs. |
| Full macro system | comptime replaces macros. Don't add a second system. |
| C++ interop | Massive complexity. C interop is sufficient. |
| DWARF debug info | Huge effort. Interpreter debugger + native debugger (ptrace) + stack traces are sufficient. |
| Graph coloring register allocator | Linear scan is good enough. |

---

## Recommended Execution Order

### Immediate (Next 1-2 weeks)
1. ✅ Fix remaining C codegen tests — DONE (test_iter, test_lang_features now pass; test_phase8b in skip list)
2. ✅ More Result/Option methods — DONE (map, and_then, or_else, unwrap_or_else, expect)
3. ✅ Iterator adapters in stdlib — DONE (take, skip, zip, enumerate, flat_map, chunk, window, dedup, sort_by)
4. LSP rename support (3-5 days)

### Short-term (1-2 months)
5. ✅ `@sync`/`@send` data race prevention — DONE
6. ✅ Vec/mat math (8A) — vec2/vec3/vec4, mat4, quat, swizzle, operator overloading — DONE
7. ✅ Type inference — DONE (bidirectional, contextual literals, `-> _` return type inference)
8. ✅ C codegen stack traces — DONE (p_call_stack[] with push/pop in prologues/epilogues)

### Medium-term (2-4 months)
9. Green threads / goroutine-style concurrency (1-2 months)
10. Auto-vectorization with variable trip counts (1-2 months)
11. ✅ Package manager — `ark` binary with init/add/install/remove/list/lock commands, path + git deps — DONE

### Long-term (4+ months)
12. Industry domain modules (Phase 9)
13. Self-hosting compiler rewrite (Phase 10)
14. Shader generation (8C)

### Completed (Aug 2026)
- ✅ Debugging tools (Phase 14) — interpreter debugger, record/replay, hot-reload, post-mortem, native debugger
- ✅ Diagnostic system (Phase 12) — error codes, source context, colors, "did you mean?"
- ✅ Security hardening — binary hardening, @constant_time, @tainted
- ✅ Borrow checker + @send/@sync — Rules 1-3, NLL, field-wise, data race prevention

---

## Competitive Positioning

### Current State (Aug 2026)

| Dimension | Fides | vs Rust | vs Go | vs Zig | vs C |
|-----------|-------|---------|-------|--------|------|
| Compilation speed | **10/10** | +8 | +1 | +2 | +2 |
| Safety | **10/10** | +1 | +6 | +6 | +8 |
| Concurrency | 6/10 | -2 | -3 | +1 | +3 |
| Tooling | 8/10 | -1 | = | +1 | +4 |
| Debugging | **9/10** | = | +1 | +2 | +5 |
| Stdlib | 7/10 | -1 | -2 | +3 | -1 |
| Binary hardening | **10/10** | +2 | +3 | +3 | +5 |
| Unique features | **10/10** | +2 | +4 | +3 | +6 |

### After Phase 13 (World Class)

| Dimension | Fides | vs Rust | vs Go | vs Zig | vs C |
|-----------|-------|---------|-------|--------|------|
| Compilation speed | **10/10** | +8 | +1 | +2 | +2 |
| Safety | **10/10** | +1 | +6 | +6 | +8 |
| Concurrency | **9/10** | = | = | +5 | +6 |
| Tooling | 8/10 | -1 | = | +1 | +4 |
| Debugging | **9/10** | = | +1 | +2 | +5 |
| Stdlib | 8/10 | = | -1 | +4 | -2 |
| Binary hardening | **10/10** | +2 | +3 | +3 | +5 |
| Unique features | **10/10** | +2 | +4 | +3 | +6 |

**Bottom line:** Fides already beats C and Zig on safety, matches Go on
concurrency (channels + select), beats Rust on compilation speed, and has
unique features (sandbox, capabilities, hot reload, @conserve, @constant_time,
binary hardening by default, interactive debugger with hot-reload) that no
competitor has. After Phase 13, Fides becomes a credible alternative for
concurrent server programming, systems programming, and embedded development.
