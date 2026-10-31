# Fides Compiler Architecture

**Version:** 0.6.0
**Targets:** x86_64 (Windows PE + Linux ELF64), ARM64 (Linux ELF64 cross-compilation), RISC-V (Linux ELF64 cross-compilation), Mach-O (macOS), COFF (Windows), WebAssembly, SPIR-V (GPU), bare-metal flat binary, C codegen, IR interpreter

---

## 0. Why a Custom Toolchain

*From `architecture-overview.md`.*

> If we depend on it, we build it. No LLVM. No libc. No external linker.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.fd`, `features.md`).*

### Why Custom Everything?

#### The LLVM Trap
Every modern language (Rust, Zig, Swift, Crystal, Julia) uses LLVM. This means:
- They all share the same optimization limitations
- They all share the same compile-time characteristics
- They all share the same binary format constraints
- None can leverage language-specific semantics in the optimizer
- None can fix LLVM bugs without going through LLVM's contribution process

**Using LLVM would make Fides "just another LLVM frontend."** We would gain nothing architecturally over Rust or Zig. Our semantic information (regions, effects, capabilities) would be lost when lowering to LLVM IR.

#### The libc Trap
Most languages depend on libc for basic operations (malloc, printf, file I/O). This means:
- They inherit libc's security vulnerabilities
- They inherit libc's binary bloat
- They can't run on bare metal without a libc port
- They're tied to POSIX/C semantics

**Fides's custom runtime eliminates the libc dependency for Fides binaries.** On Linux, `runtime_linux.rs` implements `p_print_*`, `p_panic`, `p_alloc`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str` as hand-encoded raw x86-64 machine code using direct syscalls (System V AMD64 ABI, no libc calls). On Windows, `pe.rs` links against `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — the minimum Windows ABI surface, not the C runtime.

#### The Linker Trap
External linkers (ld, lld, link.exe) are separate processes that:
- Don't understand language semantics
- Add 50%+ to build time for large projects
- Require object file formats that lose information
- Can't do cross-module optimization

**Fides's integrated linker** (`linker.rs`) is part of the compiler — no separate process, no information loss, incremental linking, link-time dead code elimination. Resolves `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64` relocations in-process.

---

### What We Built

#### 1. Custom Lexer — ✅ Complete
- Hand-written tokenizer in `src/lexer.rs`
- No lexer generator (no flex, no nom)
- All Fides keywords (`fn`, `val`, `const`, `return`, etc.), operators, literals (decimal/hex/binary/octal/underscored ints, f32/f64 floats, strings, f-strings with brace-depth tracking)
- Line/column tracking for error messages

#### 2. Custom Parser — ✅ Complete
- Recursive descent parser in `src/parser.rs`
- No parser generator (no yacc, no ANTLR)
- Full AST: functions, structs, enums, impls, type aliases, consts, imports, attributes
- Block expressions, closures, match arms with guards and range patterns

#### 3. Custom Typechecker — ✅ Complete
- Region inference + effect system + capability checking, all in `src/typechecker.rs`
- No external SMT solver (everything built in)
- **Regions**: compile-time use-after-free detection via `freed_regions` tracking; escape detection via `region_stack`; auto-inference
- **Effects**: `!IO`, `!Net`, `!Spawn`, `!Alloc`, `!{}` (pure); fixpoint iteration over call graph for inference; under-declaration is a compile error
- **Capabilities**: `cap` declarations, `use ... with { IO, FS, Net, Spawn }` grants, transitive propagation, compile-time denial, zero runtime overhead (capability elision)

#### 4. Custom IR — PIR — ✅ Complete
- Multi-level IR in `src/pir.rs`: L1 (semantic) → L2 (mid-level) → L3 (low-level)
- Typed, SSA-based, human-readable
- Preserves region/effect/capability information through optimization
- `fidesc pir <file> [1|2|3|all]` to inspect each level
- Underlying single-level SSA IR still in `src/ir.rs` (used by interpreter and PE/ELF paths)

#### 5. Custom Optimizer — ✅ Enabled by Default
- Pass-based optimization in `src/opt.rs` (10 passes): const fold, CSE, DCE, inline, unroll, TCO, alias analysis, effect reordering, partial eval, vectorize
- Region-aware alias analysis (free, no pointer analysis)
- Effect-based reordering (pure functions can reorder)
- Capability elision (remove checks when provably safe)
- comptime partial evaluation (`src/comptime.rs`)
- **Enabled by default** in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining. PGO has been removed.
- Peephole optimizer (`src/peephole.rs`) — 9 passes, **always on** in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection

#### 6. Custom Codegen — ✅ Multi-Arch + Cross-Compilation
- **x86-64**: PE emitter (`src/pe.rs`, direct IR→machine code, Windows), x86 MASM (`src/x86.rs`), PIR→x86-64 (`src/codegen_x64.rs`)
- **ARM64**: `src/codegen_arm64.rs` (assembly output) + `src/arm64_enc.rs` (assembly→machine code encoder) + `src/runtime_arm64.rs` (ARM64 Linux runtime with SVC syscalls)
- **RISC-V**: `src/codegen_riscv.rs` (assembly output) + `src/riscv_enc.rs` (assembly→machine code encoder) + `src/runtime_riscv.rs` (RISC-V Linux runtime with ECALL syscalls)
- **WASM**: `src/wasm.rs` (PIR → WebAssembly binary, `fidesc wasm`)
- **SPIR-V / GPU**: `src/gpu.rs` (PIR → SPIR-V binary, `fidesc gpu`)
- **C source**: `src/codegen_c.rs` (IR → C99, portable fallback compiled by gcc/clang/cl.exe)
- **Register allocator**: `src/regalloc.rs` — linear scan; has known bug (see Optimizer caveat above)
- `fidesc codegen <file> --target <arch>` for assembly; `--elf|--macho|--coff` for object files
- **Cross-compilation**: `fidesc build --target arm64-linux|riscv64-linux|x86-64-windows` — zero external tools, built-in encoders + runtimes

#### 7. Custom Linker / Binary Emitters — ✅ Multi-Format
- **PE**: `src/pe.rs` — Windows `.exe`, direct byte emission, no `link.exe`/`ml64.exe`
- **ELF64**: `src/elf64.rs` — Linux executables + relocatable object files, no external `ld`
- **Mach-O**: `src/macho.rs` — macOS object files
- **COFF**: `src/coff.rs` — Windows object files
- **Bare-metal flat binary**: `emit --bare` — no headers, no stdlib, raw machine code at fixed address
- **Integrated linker**: `src/linker.rs` — links ELF64 objects into executables, resolves relocations in-process, incremental linking, link-time DCE. Target-aware: handles ARM64 (BL, B, B.cond, ADRP, ADD:lo12) and RISC-V (JAL, branch) relocations. Emits EM_AARCH64 (183) and EM_RISCV (243) machine types.
- `fidesc link <files...>` to link multiple object files

#### 8. Custom Runtime — ✅ No libc for Fides Binaries
- **Linux**: `src/runtime_linux.rs` — hand-encoded x86-64 machine code (raw bytes, not assembly text) implementing all runtime stubs via direct syscalls. System V AMD64 ABI. No libc.
- **Windows**: `src/pe.rs` runtime stubs link only `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — not the MSVC C runtime.
- **Bare metal**: no runtime — `--bare` mode emits flat binary with no stdlib
- Panic handler (`p_panic`), print stubs (`p_print_str/int/bool/float/newline`), allocator (`p_malloc`/`p_free`/`p_alloc`), string ops (`p_str_concat`/`p_to_str`), Vec/HashMap/String ops

#### 9. Custom Stdlib — ✅ 35 Modules
- `stdlib/core.fd` (auto-included): `Option`, `Result`, `println`, `assert`, `assert_eq`, `max`, `min`, `abs`, `clamp`
- `stdlib/vec.fd`: dynamic array (Vec<T> with push/pop/get/len)
- `stdlib/math.fd`: sqrt, sin, cos, floor, ceil, pow, fabs, constants
- `stdlib/ecs.fd`: ECS runtime helpers
- `stdlib/events.fd`: event dispatch helpers
- `stdlib/parallel.fd`: par_map, par_sum, par_dot (sequential framework)
- `stdlib/sync.fd`: thread/atomic/mutex primitives
- `stdlib/json.fd`: parse, stringify, get, at
- `stdlib/tensor.fd`: n-d arrays, matmul, relu, softmax
- `stdlib/db.fd`: `cap Database` trait, in-memory KV store
- `stdlib/http.fd`: socket + HTTP parsing
- `stdlib/simd.fd`: elementwise ops, dot, sum, scale (f64x2, f64x4, etc.)
- `stdlib/shape.fd`: shape system for tensor ops
- Plus: env, args, process, regex, quant, money, banking, alloc, file, time, typed_strings, memo, privacy
- Plus 8 game stdlib modules: tilemap, sprite, flowfield, streaming, scene, physics, audio, input

#### 10. Custom Allocator — ✅ Partial
- `src/embedded.rs` — `FixedBufferAllocator`: pre-allocated buffer, no heap fragmentation, deterministic allocation patterns for embedded/IoT
- General path: `p_malloc`/`p_free` wrap the platform allocator (HeapAlloc on Windows, hand-coded mmap/brk on Linux runtime)
- Region allocator (arena-based, tied to the region system): planned, not yet implemented
- `!RealTime` effect for bounded execution time guarantees: framework in `embedded.rs`

#### 11. Embedded & IoT Support — ✅ Phase 5B
- RISC-V target via `codegen_riscv.rs`
- Deterministic memory mode (`FixedBufferAllocator`)
- Binary size optimization (< 4KB for simple programs)
- Real-time guarantees (`!RealTime` effect — bounded execution time)
- HAL framework
- Examples: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`

#### 12. Hot Reload — ✅
- `src/hotreload.rs` + `fidesc watch`
- Native JIT via ELF64 emission
- Function-level change detection via L2 IR hashing
- Recompiles and swaps only changed functions

#### 13. C Header FFI — ✅
- `src/cparser.rs` + `fidesc parse-header <file.h>`
- Parses C headers and generates Fides FFI bindings automatically
- `extern "C"` functions: parsed, lowered, C codegen, interpreter stubs

---

### Dependencies We Accept (For Now)

| Dependency | Purpose | When We Remove It |
|---|---|---|
| Rust compiler | Build `fidesc` itself | Phase 6 (self-hosting) |
| Rust stdlib | Build `fidesc` itself | Phase 6 (self-hosting) |
| `hashbrown`, `compact_str`, `smallvec`, `bumpalo` | Build `fidesc` itself | Phase 6 (self-hosting) |
| Windows `kernel32.dll` | Fides binary I/O + heap on Windows | Phase 5 (custom syscall runtime) |
| `gcc`/`clang`/`cl.exe` | Only for `ccompile` command (C backend) | Never — it's an optional fallback backend |

**Everything else is custom-built. No LLVM, no libc (for Fides binaries), no external linker, no external assembler.**

Note: the `ccompile` command intentionally invokes an external C compiler — that's the *portability fallback* backend (Zig-style), not the primary path. The primary paths (`emit`, `build`, `run`) use zero external tools.

---

### The Path to Zero Dependencies

```
Now (Phase 4D, complete):
  fidesc (Rust binary)
    → uses Rust stdlib + 4 crates (hashbrown, compact_str, smallvec, bumpalo)
    → produces ELF64 executables on Linux (no external ld/as)
    → produces PE executables on Windows (no external link.exe/ml64.exe)
    → produces Mach-O / COFF object files
    → produces WASM, SPIR-V, bare-metal flat binaries
    → Fides binaries use raw syscalls (Linux) or kernel32.dll (Windows) — no libc

Phase 5:
  fidesc (Rust binary)
    → custom file I/O via raw syscalls on Windows too (drop kernel32.dll for Fides binaries)
    → region allocator tied to region system
    → full effect handler continuation semantics

Phase 6:
  fidesc (Fides binary, self-hosting)
    → no Rust stdlib
    → no Rust compiler needed to build fidesc
    → produces PE/ELF/Mach-O/WASM/SPIR-V
    → ZERO external dependencies
```

---

### Binary Output — Current State

#### PE Emission (Windows) — ✅
- Direct byte emission of PE32+ format in `src/pe.rs` (bootstrap) and `compiler/pe_ir.fd` + `compiler/pe_ir_enc.fd` (self-hosted, full parity)
- No assembler, no linker, no `ml64.exe`, no `link.exe`
- Custom section layout (`.text`, `.data`, `.rdata`)
- Custom import table (only `kernel32.dll` + `msvcrt.dll` for strlen/memcpy/memcmp/etc.)
- RIP-relative addressing, Rel32 relocations, Win64 ABI (RCX/RDX/R8/R9 + shadow space)
- `fidesc emit <file> -o <exe>` and `fid emit <file> -o <exe>` produce working `.exe`s in one step
- **Self-hosted fixpoint verified** (Oct 30): `fid-n5 emit compiler/main.fd` produces a byte-identical compiler to `fid-n5` itself (12,304,896 B); the same source emitted by the C-compiled compiler is also byte-identical
- **Faster than the C build on real work:** `dump-c compiler/main.fd` 3m21s native vs 4m21s C release (0.77×), byte-identical 40 MB output
- Cheap-codegen wins: block-local slot reuse (~140 KB → ~1.3 KB frames), store→load forwarding, immediates, `cmp`+`jcc` fusion, call-arg immediates, inlined `p_vec_len`/`p_map_len`/`p_*_is_empty`, `rep movsq` struct copies, `strlen`/`memcpy`-based string stubs, `pert_str_hash` word hashing
- Correctness: Windows guard-page **stack probes** on frames > 4 KB; deterministic output (no HashMap-order leaks in slot assignment)

#### ELF64 Emission (Linux) — ✅
- Direct byte emission in `src/elf64.rs`
- Primary native backend on Linux via `fidesc build`
- Full PIR pipeline: L1 → L2 → L3 → codegen → peephole → ELF64
- Relocations: `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64`
- Links against hand-encoded runtime (`runtime_linux.rs`) — raw machine code, no libc
- Also emits relocatable object files (`.o`)
- Zero external tools: no `as`, no `ld`, no `gcc`

#### Mach-O (macOS) — ✅
- `src/macho.rs` — object file emission
- `fidesc codegen <file> --macho <out.o>`

#### COFF (Windows) — ✅
- `src/coff.rs` — object file emission
- `fidesc codegen <file> --coff <out.o>`

#### Bare-Metal Flat Binary — ✅
- `fidesc emit <file> --bare -o <bin>`
- No file format headers — raw machine code at a fixed address
- No stdlib, no runtime
- For bootloaders, OS kernels, firmware, microcontrollers
- Examples: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`, `tests/bootstrap/test_boot.fd`, `tests/bootstrap/test_bare.fd`

#### WASM — ✅
- `src/wasm.rs` — PIR → WebAssembly binary
- `fidesc wasm <file> -o <out.wasm>`

#### SPIR-V / GPU — ✅
- `src/gpu.rs` — PIR → SPIR-V binary
- `fidesc gpu <file>`

#### C Source (portability fallback) — ✅
- `src/codegen_c.rs` — IR → C99 source
- `fidesc ccompile <file> -o <exe>` generates `.c`, invokes `gcc`/`clang`/`cl.exe`
- Used when targeting platforms the native backends don't yet cover
- Note: 164/165 test parity with interpreter. Binary hardening enabled.

---

### Why This Matters for Performance

#### Compilation Speed
```
Rust pipeline:
  Source → AST → MIR → LLVM IR → LLVM opts (100+ passes) → Machine code → Object files → Linker
  Total: 10-30 minutes for large projects

Fides pipeline (current):
  Source → AST → TypeCheck → Mono → IR → PIR L1 → L2 → L3 → Codegen → Peephole → ELF64
  Total: < 1 second for small projects, < 10 seconds for large projects
```

**Why Fides is faster:**
- No LLVM (saves ~80% of compilation time)
- No external linker (saves ~50% of remaining time)
- Fewer optimization passes (only what's needed — peephole is always on, opt.rs is enabled by default)
- Semantic optimization (regions/effects give us info LLVM would have to derive)
- Integrated pipeline (no IPC between compiler and linker)

#### Binary Size
```
C "hello world": ~16KB (with libc)
Rust "hello world": ~4MB (with stdlib)
Fides "hello world": ~4KB target (no libc, no runtime, no stdlib bloat)
```

**Why Fides is smaller:**
- No libc (we implement only what's needed, via raw syscalls)
- No runtime (no GC, no RTTI, no reflection)
- Dead code elimination at IR level (not link time)
- comptime inlines constants (no runtime initialization)
- Custom binary format (only sections we need)
- Bare-metal mode strips even the file format headers

---

### Source File Map (Toolchain-Relevant)

| File | Role |
|------|------|
| `src/lexer.rs` | Tokenizer |
| `src/parser.rs` | Recursive descent parser |
| `src/typechecker.rs` | Types + regions + effects + capabilities |
| `src/ir.rs` | Single-level SSA IR (interpreter + PE/ELF path) |
| `src/lower.rs` | AST → SSA IR |
| `src/pir.rs` | Multi-level PIR (L1/L2/L3) |
| `src/opt.rs` | PIR optimizer (enabled by default, `--lto` for aggressive inlining) |
| `src/peephole.rs` | 9-pass peephole (always on in `build`) |
| `src/regalloc.rs` | Linear scan register allocator (⚠️ known bug) |
| `src/mono.rs` | Generic monomorphization |
| `src/comptime.rs` | Compile-time eval + reflection |
| `src/pe.rs` | PE emitter (Windows, direct IR→machine code) |
| `src/elf64.rs` | ELF64 emitter (Linux, PIR pipeline) |
| `src/macho.rs` | Mach-O object emitter (macOS) |
| `src/coff.rs` | COFF object emitter (Windows) |
| `src/linker.rs` | Integrated linker (ELF64 objects → executable), target-aware (ARM64, RISC-V) |
| `src/runtime_linux.rs` | Hand-encoded Linux x86-64 runtime (raw syscalls, no libc) |
| `src/arm64_enc.rs` | ARM64 assembly text → 4-byte machine code encoder |
| `src/runtime_arm64.rs` | ARM64 Linux runtime (SVC syscalls, no libc) |
| `src/riscv_enc.rs` | RISC-V (RV64) assembly text → 4-byte machine code encoder |
| `src/runtime_riscv.rs` | RISC-V Linux runtime (ECALL syscalls, no libc) |
| `src/codegen_x64.rs` | PIR L3 → x86-64 assembly |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly |
| `src/codegen_c.rs` | IR → C99 source (portability fallback) |
| `src/wasm.rs` | PIR → WebAssembly |
| `src/gpu.rs` | PIR → SPIR-V |
| `src/x86.rs` | IR → MASM assembly (legacy `build` path) |
| `src/embedded.rs` | FixedBufferAllocator, RealTime effect, HAL framework |
| `src/hotreload.rs` | Native JIT + function-level change detection |
| `src/cparser.rs` | C header parser → Fides FFI bindings |
| `src/compiler.rs` | Pipeline orchestration, module resolution, stdlib auto-include |
| `src/main.rs` | CLI entry point, command dispatch |

---

## Fides Architecture — The Full Stack

> Every layer built by us. No LLVM. No borrowed infrastructure.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.fd`, `features.md`).*

### Current Architecture (Phase 8 — What We Have Now)

```
Source (.fd files)
    ↓
Lexer (src/lexer.rs)              — hand-written tokenizer
    ↓
Parser (src/parser.rs)            — recursive descent, produces AST
    ↓
Typechecker (src/typechecker.rs)  — types + regions + effects + capabilities + traits
    ↓
Monomorphization (src/mono.rs)    — generic specialization
    ↓
Lowering (src/lower.rs)           — AST → SSA IR (src/ir.rs)
    ↓
Comptime (src/comptime.rs)        — compile-time eval + reflection
    ↓
┌──────────────────────────────────────────────────────────────────────┐
│                                                                      │
│  Interpreter (src/interp.rs)        — runs IR directly (fidesc run) │
│  PIR Pipeline (src/pir.rs)          — IR → PIR L1 → L2 → L3          │
│  Peephole (src/peephole.rs)         — 9 passes, always on in build   │
│  Optimizer (src/opt.rs)             — enabled by default in build    │
│  Register Allocator (src/regalloc.rs) — linear scan                  │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
    ↓
┌──────────────────────────────────────────────────────────────────────┐
│                                                                      │
│  Native Backends                                                     │
│  • PE Emitter (src/pe.rs)            — IR → x86-64 .exe (Windows)    │
│  • ELF64 Emitter (src/elf64.rs)      — PIR → ELF64 executable (Linux)│
│  • Mach-O Emitter (src/macho.rs)     — object files (macOS)          │
│  • COFF Emitter (src/coff.rs)        — object files (Windows)        │
│  • Bare-metal flat binary            — emit --bare                   │
│                                                                      │
│  Multi-Arch Codegen (assembly output)                                │
│  • x86-64 (src/codegen_x64.rs)       • ARM64 (src/codegen_arm64.rs)  │
│  • RISC-V (src/codegen_riscv.rs)     • MASM (src/x86.rs)             │
│                                                                      │
│  Portable / Web / GPU Backends                                       │
│  • C source (src/codegen_c.rs)       — IR → C99 (gcc/clang/cl.exe)   │
│  • WASM (src/wasm.rs)                — PIR → WebAssembly binary      │
│  • SPIR-V (src/gpu.rs)               — PIR → SPIR-V GPU binary       │
│                                                                      │
│  Integrated Linker (src/linker.rs)   — ELF64 objects → executable    │
│  Hot Reload (src/hotreload.rs)       — native JIT + function swap    │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
    ↓
Native Binary / WASM / SPIR-V / C Source / Running Code
```

**What works today:**
- Lexer, parser, AST, typechecker (types + regions + effects + capabilities + traits)
- Lowering, monomorphization, comptime (eval + reflection + f-string interpolation)
- IR interpreter (`fidesc run`)
- PIR pipeline: L1 → L2 → L3 (`fidesc pir`)
- Peephole optimizer: 9 passes, always on in `build`
- PIR optimizer: 23 passes (const fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias analysis, effect reorder, partial eval, vectorize, strength reduce, devirt, jump thread, loop rotate, loop vec, effect opt, region pool, reassociate, memoize) — enabled by default in `build` (use `--no-opt` to disable), `--lto` for aggressive inlining. Function-level parallelism (19/22 passes parallelized).
- PE emitter: native x86-64 Windows executables, no external tools
- ELF64 emitter: native Linux executables + relocatable objects, no external tools
- Mach-O / COFF object file emission
- Bare-metal flat binary mode (`--bare`)
- Multi-arch assembly codegen: x86-64, ARM64, RISC-V
- WASM backend, SPIR-V/GPU backend
- C source backend (portability fallback via gcc/clang/cl.exe)
- Integrated linker (ELF64 objects → executable, in-process)
- Hot reload (`fidesc watch`) — native JIT + function-level change detection
- Custom runtime: hand-encoded x86-64 machine code via raw syscalls (Linux, no libc) / kernel32.dll (Windows)
- 99 stdlib modules: core, vec, math, string, fmt, bits, sort, hash, iter, stack, queue, buffer, collections, result, random, path, units, uuid, crypto, storage, clinical, medical, ecu, scada, swift, grid, ota, banking, kv_cache, quant, dp, shard, fs, io, os, sys, ecs, events, parallel, sync, channel, db, http, json, shape, simd, tensor, money, alloc, file, time, typed_strings, memo, privacy, env, args, process, regex, tilemap, sprite, flowfield, streaming, scene, physics, audio, input, arinc, can, uefi
- C header FFI parsing (`fidesc parse-header`)
- Embedded/IoT: FixedBufferAllocator, `!RealTime` effect, RISC-V target, HAL framework
- Smart pointers: `Box<T>` (recursive types), `Rc<T>` / `Arc<T>` (manual refcount management)
- Vec/Mat math types: vec2/3/4, mat4, quat with operator overloading + swizzle access
- Incremental compilation: in-memory cache (`src/inc.rs`), shared across LSP/watch/test harness
- Profiler (`fidesc profile`), linter (`fidesc lint`), benchmark (`fidesc bench`), formatter (`fidesc fmt`)
- Package manager (`ark`), doc generator (`scribe`), coverage tool (`veil`)
- Interactive debugger (`fidesc debug`), native debugger (`fidesc autopsy`)
- No external dependencies for primary paths: no LLVM, no libc (in Fides binaries), no external assembler, no external linker

**What's partial / pending:**
- ✅ PIR optimizer: enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive cross-module inlining. PGO removed.
- ⚠️ Effect handlers: `handle { do } with` syntax parses and runs; full continuation semantics pending
- ✅ `async` / `await` / `spawn`: implemented (eager evaluation, OS thread model). State machine transform for cooperative scheduling pending.
- ❌ `#[test]` attribute: parsed but not executed by `fidesc test`
- ✅ `@derive(Debug/Clone/Serialize/Deserialize)`: implemented (Phase 7.7)
- ✅ Borrow checker: Phases 1-6 complete, `on` by default, field-wise borrows, two-phase borrows, NLL
- ✅ `Box<T>` recursive types: implemented (self-hosting blocker 3 resolved)
- ✅ `Rc<T>` / `Arc<T>` smart pointers: implemented (manual refcount management)
- ✅ Vec/Mat math types: vec2/3/4, mat4, quat with operator overloading + swizzle
- ✅ Incremental compilation: in-memory cache (`src/inc.rs`)
- ❌ Default arguments, variadic arguments: not implemented
- ❌ Region allocator (arena-based, tied to region system): not started
- ⚠️ Self-hosting (Phase 6): 3 of 5 blockers resolved (enum payloads, generic Vec, recursive Box). Compiler is still written in Rust.

---

### PIR — Fides Intermediate Representation

#### Design Principles

1. **Typed**: every value has a type, types carry semantic information
2. **Multi-level**: high-level → mid-level → low-level, each preserves different info
3. **Semantic-aware**: regions, effects, capabilities are first-class in PIR
4. **SSA-based**: static single assignment for optimization passes
5. **Target-independent at high level**: same PIR for all architectures
6. **Human-readable**: debuggable, inspectable (`fidesc pir <file> [1|2|3|all]`)

#### PIR Level 1 — High-Level (Semantic)

Preserves all Fides language semantics:

```
function main() -> i64 !IO {
    region r1 = new_region();
    %1 = alloca_in(r1, [u8; 4096]);          ; tied to region r1
    %2 = call read_file("input.txt") 
         effects=[!IO] region=r1;             ; borrows r1
    %3 = call parse(%2) effects=[];           ; pure call
    free_region(r1);                          ; r1 freed → %1, %2 invalidated
    ; Any use of %1 or %2 after this = compile error
    return %3;
}
```

**Optimizations at this level** (in `src/opt.rs`, enabled by default in `build`):
- Region-aware alias analysis (free, no pointer analysis needed)
- Effect-based reordering (pure calls can move past each other)
- Capability elision (if capability is provably unused, remove checks)
- comptime evaluation (run code at compile time, inline results)
- Inlining (function bodies inlined with region/effect propagation)

#### PIR Level 2 — Mid-Level (Control Flow)

Loses high-level semantics, gains optimization opportunities:

```
function main() -> i64 {
bb0:
    %1 = alloca [u8; 4096]
    %2 = call read_file("input.txt")
    %3 = call parse(%2)
    %4 = free %1
    return %3
}
```

**Optimizations at this level:**
- Loop unrolling, loop fusion, loop hoisting
- Tail call optimization
- Closure conversion (closures become functions + env structs)
- Constant folding, strength reduction
- Dead code elimination
- Common subexpression elimination

#### PIR Level 3 — Low-Level (Machine-Near)

Close to machine code, architecture-specific:

```
function main() -> i64 {
bb0:
    %1 = sub rsp, 4096          ; stack alloc
    %2 = lea rdi, [rsp]         ; pointer to buffer
    %3 = call read_file         ; call
    %4 = call parse             ; call
    %5 = add rsp, 4096          ; stack free
    mov rax, %4                 ; return value
    return
}
```

**Optimizations at this level:**
- Register allocation (linear scan in `src/regalloc.rs`)
- Instruction selection (per architecture in `codegen_x64.rs` / `codegen_arm64.rs` / `codegen_riscv.rs`)
- Peephole optimization (9 passes in `src/peephole.rs`, always on in `build`)
- Stack frame layout
- Calling convention lowering

---

### Custom Codegen — No LLVM

#### Why Not LLVM?
- LLVM is a 20-year-old C++ codebase designed for C/C++
- Its IR loses high-level semantics
- Its optimization passes are generic, not language-specific
- Its compile times are slow (100+ passes)
- Its linker (lld) is separate and expensive
- We can't control its behavior precisely
- Using it would make Fides "just another LLVM frontend" — no architectural advantage over Rust or Zig

#### What We Built Instead
- **x86-64 backend**: PE emitter (`src/pe.rs`, direct IR→machine code), PIR codegen (`src/codegen_x64.rs`), MASM (`src/x86.rs`)
- **ARM64 backend**: `src/codegen_arm64.rs` — for Apple Silicon, embedded, servers
- **RISC-V backend**: `src/codegen_riscv.rs` — for IoT, custom hardware, research
- **WASM backend**: `src/wasm.rs` — PIR → WebAssembly binary (`fidesc wasm`)
- **SPIR-V / GPU backend**: `src/gpu.rs` — PIR → SPIR-V binary (`fidesc gpu`)
- **C source backend**: `src/codegen_c.rs` — IR → C99, portability fallback compiled by gcc/clang/cl.exe

#### Codegen Strategy
```
PIR Level 3 → Register Allocator → Instruction Selector → Peephole → Binary Output
```

- **Register allocator**: linear scan (`src/regalloc.rs`) — ⚠️ has known bug (constants not loaded into registers before use can be clobbered by intervening instructions)
- **Instruction selector**: pattern matching on PIR → machine instructions per architecture
- **Peephole**: 9 local optimization passes (always on in `build`): redundant moves, zero idioms, nop arithmetic, branch threading, dead stores, redundant tests, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **Binary output**: direct to ELF/PE/Mach-O/COFF/WASM/SPIR-V (no external assembler/linker for native targets)

---

### Custom Linker — Integrated, Not Separate

#### Why Not lld/ld?
- Linking is 50%+ of build time for large projects
- External linker doesn't understand PIR semantics
- Separate linker = separate process = IPC overhead

#### What We Built
- **Integrated linker** (`src/linker.rs`): part of the compiler, not a separate process
- **ELF64 object linking**: resolves `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64` relocations in-process
- **Incremental linking**: cache object files, only re-link changed modules
- **Link-time dead code elimination**: remove unused functions (smaller binaries)
- **Multi-file linking**: `fidesc link <files...>`
- **Runtime symbol resolution**: links against hand-encoded Fides runtime (`src/runtime_linux.rs`)

---

### Custom Runtime — Minimal, No libc

#### What We Have
- **Linux runtime** (`src/runtime_linux.rs`): hand-encoded x86-64 machine code (raw bytes, not assembly text) implementing all `p_print_*`, `p_panic`, `p_alloc`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str` via direct syscalls. System V AMD64 ABI. **No libc dependency.**
- **Windows runtime** (in `src/pe.rs`): links only `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — not the MSVC C runtime.
- **Bare metal**: no runtime — `--bare` mode emits flat binary with no stdlib
- **Panic handler** (`p_panic`): for unrecoverable errors (no exceptions)

#### What We DON'T Need
- No garbage collector
- No runtime type information (RTTI)
- No virtual machine
- No JIT in production mode (hot reload JIT is dev-only)
- No reflection (comptime replaces it)
- No libc (raw syscalls on Linux, kernel32.dll only on Windows)

#### What's Planned
- Region allocator: arena-based, tied to the region system (not yet started)
- Effect runtime: for effect handlers that can't be compiled away (full continuation semantics pending)

---

### Custom Stdlib — 34 Modules

| Module | File | Contents |
|--------|------|----------|
| core | `stdlib/core.fd` | Auto-included: `Option`, `Result`, `println`, `assert`, `assert_eq`, `max`, `min`, `abs`, `clamp` |
| vec | `stdlib/vec.fd` | Dynamic array (Vec<T> with push/pop/get/len) |
| math | `stdlib/math.fd` | sqrt, sin, cos, floor, ceil, pow, fabs, constants |
| ecs | `stdlib/ecs.fd` | ECS runtime helpers |
| events | `stdlib/events.fd` | Event dispatch helpers |
| parallel | `stdlib/parallel.fd` | par_map, par_sum, par_dot (sequential framework) |
| sync | `stdlib/sync.fd` | Thread/atomic/mutex primitives |
| json | `stdlib/json.fd` | parse, stringify, get, at |
| tensor | `stdlib/tensor.fd` | n-d arrays, matmul, relu, softmax |
| db | `stdlib/db.fd` | `cap Database` trait, in-memory KV store |
| http | `stdlib/http.fd` | socket + HTTP parsing |
| simd | `stdlib/simd.fd` | elementwise ops, dot, sum, scale (f64x2, f64x4, etc.) |
| shape | `stdlib/shape.fd` | shape system for tensor ops |
| Plus 21 more | | env, args, process, regex, quant, money, banking, alloc, file, time, typed_strings, memo, privacy, tilemap, sprite, flowfield, streaming, scene, physics, audio, input |

---

### Embedded & IoT Support (Phase 5B)

- **RISC-V target**: `src/codegen_riscv.rs` — assembly output for IoT/custom hardware
- **Deterministic memory**: `FixedBufferAllocator` in `src/embedded.rs` — pre-allocated buffer, no heap fragmentation
- **Binary size optimization**: < 4KB for simple programs
- **Real-time guarantees**: `!RealTime` effect — bounded execution time
- **HAL framework**: hardware abstraction layer in `src/embedded.rs`
- **Bare-metal mode**: `fidesc emit <file> --bare -o <bin>` — no headers, no stdlib, raw machine code
- **Examples**: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`, `tests/bootstrap/test_boot.fd`, `tests/bootstrap/test_bare.fd`

---

### Hot Reload (Phase 4D)

- `src/hotreload.rs` + `fidesc watch <file>`
- Native JIT: compiles to ELF64 in memory, runs immediately
- Function-level change detection via L2 IR hashing
- Recompiles and swaps only changed functions
- Sub-second iteration for development

---

### C FFI

- `src/cparser.rs` + `fidesc parse-header <file.h>` — parses C headers, generates Fides FFI bindings
- `extern "C"` declarations: parsed, lowered, C codegen, interpreter stubs
- Tests: `test_ffi.fd`, `test_ffi_full.fd`, `test_ffi_header.fd`

---

### Bootstrapping Strategy

#### Phase 1–4D: Fides in Rust (Current)
- Compiler written in Rust (`fidesc` binary)
- Compiles Fides source to native code via custom pipeline
- Self-hosting IR, PIR, codegen, linker, runtime — all custom-built
- 4 Rust crates used to build `fidesc`: `hashbrown`, `compact_str`, `smallvec`, `bumpalo`

#### Phase 5: Custom Everything for Fides Binaries
- Custom file I/O via raw syscalls on Windows too (drop kernel32.dll for Fides binaries)
- Region allocator tied to region system
- Full effect handler continuation semantics
- `async` / `await` / `spawn` implementation

#### Phase 6: Fides in Fides (Self-Hosting)
- Rewrite compiler in Fides
- Fides compiler compiles itself
- No Rust dependency for end users
- Compiler binary is a Fides binary
- PIR optimizer runs on Fides compiler code
- comptime specializes the compiler for the target
- Compiler becomes faster by optimizing itself
- **ZERO external dependencies**

---

## 1. Pipeline Overview

```
Source (.fd)
    │
    ▼
┌──────────┐     ┌──────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────┐     ┌───────────────┐
│  Lexer   │────▶│  Parser  │────▶│  TypeChecker │────▶│  Monomorph   │────▶│  Lowerer │────▶│   SSA IR      │
│lexer.rs  │     │parser.rs │     │typechecker.rs│     │  mono.rs     │     │lower.rs  │     │   ir.rs       │
└──────────┘     └──────────┘     └──────────────┘     └──────────────┘     └──────────┘     └───────────────┘
                                         │              (types, caps,        (generic          (typed SSA,
                                         │               effects, regions,   specialization)   effects, regions)
                                         │               traits)
                                         ▼
                                    ┌──────────────┐
                                    │  Comptime    │
                                    │  comptime.rs │
                                    └──────────────┘
                                                                    │
                                        ┌───────────────────────────┼───────────────────────────┐
                                        │                           │                           │
                                        ▼                           ▼                           ▼
                                ┌──────────────┐           ┌──────────────┐           ┌──────────────┐
                                │  PIR Optimizer│          │  Backends    │           │  Interpreter │
                                │  pir.rs      │           │              │           │  interp.rs   │
                                │  opt.rs      │           │              │           └──────────────┘
                                │  peephole.rs │           │              │
                                │  regalloc.rs │           │              │
                                └──────────────┘           │              │
                                        │                  │              │
                                        ▼                  ▼              │
                                ┌──────────────┐    ┌──────────────┐      │
                                │  PIR L1/L2/L3│    │  PE Emitter  │      │
                                │  (optimized) │    │  pe.rs       │      │
                                └──────────────┘    └──────────────┘      │
                                                    │              │      │
                                                    ▼              ▼      │
                                            ┌──────────────┐ ┌──────────────┐
                                            │  ELF64       │ │  x86_64 Code │
                                            │  elf64.rs    │ │  x86.rs      │
                                            └──────────────┘ └──────────────┘
                                                    │              │
                                                    ▼              ▼
                                            ELF64 .o       MASM (.asm)
                                                    │              │
                                                    ▼              ▼
                                            ┌──────────┐    ┌──────────┐
                                            │ linker   │    │ ml64.exe │
                                            └──────────┘    └──────────┘
                                                    │              │
                                                    ▼              ▼
                                            Native binary   Native .exe

                         ┌──────────────┐           ┌──────────────┐
                         │  WASM        │           │  C Codegen   │
                         │  wasm.rs     │           │  codegen_c.rs│
                         └──────────────┘           └──────────────┘
                                │                          │
                                ▼                          ▼
                           .wasm binary              C source (.c)
                                                           │
                                                           ▼
                                                     ┌──────────┐
                                                     │ gcc/clang│
                                                     └──────────┘
                                                           │
                                                           ▼
                                                     Native .exe
```

The **`build` command** uses the full PIR pipeline (L1→L2→L3 → codegen → peephole → ELF64) and is the primary native backend on Linux — it produces working ELF64 executables directly with zero external tool dependencies. **Cross-compilation** to ARM64 (`--target arm64-linux`) and RISC-V (`--target riscv64-linux`) uses built-in instruction encoders and runtime libraries with raw Linux syscalls, also producing valid ELF64 executables. Cross-compilation to Windows (`--target x86-64-windows`) uses the PE emitter. The **`emit` command** bypasses PIR entirely: it operates directly on IR to produce Windows `.exe` files via the PE emitter (or bare-metal flat binaries with `--bare`). The **WASM backend** (`wasm.rs`) and **SPIR-V GPU backend** (`gpu.rs`) both consume PIR L3. The **C codegen** (`codegen_c.rs`) consumes IR directly. The **interpreter** provides rapid testing without any external tools. Object file formats (ELF64, Mach-O, COFF) are available via `codegen --elf|--macho|--coff`, and multiple object files can be linked with the integrated `linker.rs`.

### Entry Points (`compiler.rs`)

| Function | Pipeline | CLI Command |
|----------|----------|-------------|
| `emit_source_with_path(source, path)` | lex → parse → typecheck → monomorphize → lower → PE emission (direct IR→machine code, bypasses PIR) | `emit` |
| `emit_bare(source, path)` | lex → parse → typecheck → lower → flat binary (no stdlib) | `emit --bare` |
| `build_executable(source, path)` | lex → parse → typecheck → monomorphize → lower → PIR (L1→L2→L3) → codegen → peephole → ELF64/PE executable. Supports cross-compilation: `--target arm64`, `--target riscv`, `--target x86-64-windows`. | `build` |
| `ccompile_source_with_path(source, path)` | lex → parse → typecheck → monomorphize → lower → C codegen | `ccompile` |
| `interpret_source_with_path(source, path, verbose)` | lex → parse → typecheck → monomorphize → lower → interpret | `run` |
| `lower_to_ir(source)` | lex → parse → typecheck → monomorphize → lower → format IR | `ir` |
| `lex(source)` | lex only | `tokens` |
| `parse_to_ast(source)` | lex → parse | `ast` |
| PIR lowering (`pir.rs`) | IR → PIR L1 → L2 → L3 | `pir` |
| PIR optimization (`opt.rs`) | PIR + const fold + CSE + DCE + inline + unroll + TCO | `opt`, `pir --opt` |
| Codegen (`codegen_x64.rs`, `codegen_arm64.rs`, `codegen_riscv.rs`) | PIR L3 → x86-64 / ARM64 / RISC-V assembly | `codegen` |
| WASM backend (`wasm.rs`) | PIR L3 → WebAssembly binary | `wasm` |
| GPU backend (`gpu.rs`) | PIR L3 → SPIR-V binary | `gpu` |
| C header parser (`cparser.rs`) | C header → Fides FFI bindings | `parse-header` |
| Hot reload (`hotreload.rs`) | Native JIT (ELF64) + function-level change detection | `watch` |
| Linker (`linker.rs`) | Multiple object files → linked binary | `link` |
| ARM64 Encoder (`arm64_enc.rs`) | ARM64 assembly text → 4-byte machine code | (internal) |
| ARM64 Runtime (`runtime_arm64.rs`) | ARM64 Linux runtime (SVC syscalls) | (internal) |
| RISC-V Encoder (`riscv_enc.rs`) | RV64 assembly text → 4-byte machine code | (internal) |
| RISC-V Runtime (`runtime_riscv.rs`) | RISC-V Linux runtime (ECALL syscalls) | (internal) |
| LSP server (`lsp.rs`) | JSON-RPC 2.0 over stdio, diagnostics from compiler | `lsp` |
| Incremental cache (`inc.rs`) | In-memory cache of optimized L2 modules keyed by (path, hash, flags) | (internal) |
| Debugger (`debugger.rs`) | Interactive IR-level debugger with breakpoints, step, locals | `debug` |
| Native debugger (`native_debugger.rs`) | Ptrace-based native binary debugger (Linux) | `autopsy` |
| Profiler (`interp.rs`) | Per-function timing in interpreter mode | `profile` |
| Formatter (`fmt.rs`) | Source code formatter | `fmt` |
| Linter (`compiler.rs`) | Unused function/import detection | `lint` |
| Benchmark (`main.rs`) | Timing statistics over N runs (interpreter or C native) | `bench` |
| Package manager (`ark.rs`) | Dependency management, lock files, install | `ark` |
| Doc generator (`scribe.rs`) | HTML/Markdown docs from .fd source | `scribe` |
| Coverage tool (`veil.rs`) | Function/line/instruction coverage reporting | `veil` |

### Module Resolution (`compiler.rs`)

Before type-checking, the compiler:
1. Parses `stdlib/core.fd` (auto-included) — Option<T>, Result<T, E>, println, assert, max, min, abs, clamp
2. Parses user source with stdlib names pre-populated
3. Resolves `use` imports by searching for `<path>.fd` or `<path>/mod.fd` relative to source. **Transitive imports** are resolved automatically: if `clinical.fd` has `use math`, importing `clinical` will also load `math`.
4. Merges stdlib + imported modules + user code (user wins on conflicts, `extern_decls` merged)
5. Type-checks the merged program with module-level visibility enforcement

**Standard library modules** (99 modules in `stdlib/`, 28,370 LOC): `core.fd` (auto-included), `vec.fd`, `math.fd`, `string.fd`, `fmt.fd`, `bits.fd`, `sort.fd`, `hash.fd`, `iter.fd`, `stack.fd`, `queue.fd`, `buffer.fd`, `collections.fd`, `result.fd`, `random.fd`, `path.fd`, `units.fd`, `uuid.fd`, `crypto.fd`, `storage.fd`, `clinical.fd`, `medical.fd`, `ecu.fd`, `scada.fd`, `swift.fd`, `grid.fd`, `ota.fd`, `banking.fd`, `kv_cache.fd`, `quant.fd`, `dp.fd`, `shard.fd`, `channel.fd`, `sync.fd`, `parallel.fd`, `ecs.fd`, `events.fd`, `db.fd`, `http.fd`, `json.fd`, `simd.fd`, `shape.fd`, `tensor.fd`, `money.fd`, `alloc.fd`, `file.fd`, `time.fd`, `typed_strings.fd`, `memo.fd`, `privacy.fd`, `env.fd`, `args.fd`, `process.fd`, `regex.fd`, `tilemap.fd`, `sprite.fd`, `flowfield.fd`, `streaming.fd`, `scene.fd`, `physics.fd`, `audio.fd`, `input.fd`, `arinc.fd`, `can.fd`, `fs.fd`, `io.fd`, `os.fd`, `sys.fd`, `uefi.fd` — imported explicitly via `use`.

**String concatenation**: `str + str` is lowered to a `p_str_concat` runtime call. Works with literals, variables, and inline function call results. `str + i64` is pointer arithmetic (not concatenation).

### Monomorphization (`mono.rs`)

After type-checking, generic functions are monomorphized:
- Scans all call sites for concrete type arguments
- Generates specialized copies (e.g., `identity__i64`, `identity__bool`)
- Replaces generic call sites with calls to specialized versions
- Handles generic methods on structs (mangled as `StructName__method__T1_T2`)

---

## 2. Stage Details

### 2.1 Lexer (`lexer.rs`, `token.rs`)

Reads source text character-by-character and produces a `Vec<Token>`.

**Key responsibilities:**
- Tokenize all Fides keywords (`fn`, `val`, `const`, `return`, `if`, `else`, `for`, `while`, `match`, etc.)
- Parse integer literals (decimal, hex `0xFF`, binary `0b1010`, octal `0o777`, underscores `1_000`)
- Parse float literals (`3.14`, `3.14f32`)
- Parse string literals (`"hello"`)
- Parse f-string literals (`f"hello {name}!"`) via `read_fstring()` with brace-depth tracking
- Tokenize operators (`+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<=`, `>=`, `&&`, `||`, `&`, `|`, `^`, `~`, `<<`, `>>`, `..`, `..=`, `->`, `=>`, `::`, `?`, `#`)
- Skip comments (`//` line, `/* */` block)
- Track line/column for error messages

**Output:** `Vec<Token>` where `Token { kind: TokenKind, line: usize, col: usize }`

### 2.2 Parser (`parser.rs`, `ast.rs`)

Recursive descent parser. Consumes tokens and builds an AST (`Program`).

**AST structure:**
```
Program
├── functions: Vec<Func>
├── structs: Vec<StructDef>
├── enums: Vec<EnumDef>
├── impls: Vec<ImplDef>
├── type_aliases: Vec<TypeAlias>
├── consts: Vec<ConstDef>
├── imports: Vec<ImportDef>
└── attributes: Vec<Attribute>

Func
├── name, params, ret_type, body: Vec<Stmt>, type_params

Stmt
├── Let { name, type, init, is_mut }
├── Assign { target, op, value }
├── Ret { expr }
├── If { cond, then_body, else_body }
├── While { cond, body }
├── For { var, iterator, body }
├── Match { expr, arms }
├── Expr(expr)
├── Defer { body }
├── Break, Continue
└── Use { path, items }

Expr
├── IntLit, FloatLit, StrLit, BoolLit
├── FString { parts: Vec<FStringPart> }
├── Ident, BinOp, UnaryOp
├── Call { func, args }
├── MethodCall { receiver, method, args }
├── StructLit { name, fields }
├── FieldAccess { expr, field }
├── ArrayLit, Index
├── Ref { expr, is_mut }, Deref { expr }
├── Closure { params, body, ret_ty }
├── TupleLit
├── Range { start, end, inclusive }
├── Block(Vec<Stmt>)
├── EnumLit { enum_name, variant, args }
└── Try { expr }
```

### 2.3 Type Checker (`typechecker.rs`)

Validates the AST before lowering. Collects all errors and reports them at once.

**Key responsibilities:**
- Type inference for `val` bindings without explicit type
- Function call argument type matching
- Binary operation type compatibility (arithmetic, comparison, logical, bitwise)
- Struct literal field validation
- Field access on valid struct types
- Return statement type matching
- Pattern match arm type consistency
- Constant resolution (`const` constants)
- Type alias resolution (`type`)
- Generic type parameter registration (simplified)

**Output:** Either `Ok(())` or `Err(Vec<String>)` with all error messages.

### 2.4 IR Lowering (`lower.rs`)

Lowers the type-checked AST to typed SSA IR.

**SSA IR structure (`ir.rs`):**
```
IrModule
├── functions: Vec<IrFunction>
└── externs: Vec<ExternFunc>

IrFunction
├── name, params, ret_ty
├── blocks: Vec<BasicBlock>
├── values: HashMap<ValueId, ValueMeta>
└── allocas: Vec<AllocaInst>

BasicBlock
├── id: BlockId
├── insts: Vec<Inst>
└── terminator: Terminator

Inst
├── Const { value, type }
├── ConstStr { value }
├── BinOp { op, lhs, rhs }
├── UnaryOp { op, operand }
├── Call { func, args, ret_ty }
├── Load { ptr, type }
├── Store { ptr, value }
├── Alloca { type }
├── GetElemPtr { array_ptr, index, elem_ty }
├── GetFieldPtr { struct_ptr, field_index, field_ty }
├── StructConstruct { fields }
├── StructField { struct_val, field_idx }
├── TryUnwrap { value, ok_block, err_block }
├── Jump { target }
├── Branch { cond, then, else_ }
└── ...

Terminator
├── Br { target }
├── CondBr { cond, then_block, else_block }
├── Ret { value }
└── Panic { msg }
```

**Key lowering decisions:**
- **Mutable variables** → `alloca` + `load`/`store` (not SSA renaming)
- **`alloca_types` tracking** → `HashMap<ValueId, IrType>` maps each alloca to its IR type, enabling correct `Load` types (not hardcoded `I64`)
- **For loops (range)** → `alloca` for loop variable, comparison block, body block, increment, back-edge
- **For loops (array iterator)** → index-based while loop: `alloca` for index counter, `GetElemPtr` + `Load` for element access, handles both direct array and pointer-to-array (from mutable variable loads)
- **While loops** → condition block, body block, back-edge
- **If/else** → `CondBr` to then/else blocks, merge block
- **Defer blocks** → `defer_stack: Vec<Vec<Stmt>>` tracks deferred statements; `emit_defers` drains and emits them before `Ret` instructions
- **F-strings** → `ConstStr` for literals, `__fides_to_str` for non-string expressions, `__fides_str_concat` to join all parts
- **Constants** → `Inst::Const` with value inlined
- **Array literals** → `Alloca` for array, `GetElemPtr` + `Store` per element, returns array pointer
- **Array indexing** → `GetElemPtr` with element type from array type, `Load` with correct element type
- **Array index assignment** → `GetElemPtr` + `Store` (with compound assignment via `Load` + `BinOp` + `Store`)
- **Method calls** → Name mangling (`__fides_StructName_method`), receiver as first arg, SRET for struct returns
- **Match expressions** → `lower_match_expr` with result slot `alloca`, pattern matching branches, merge block
- **Match patterns** → `IntLit` (equality), `StrLit` (string comparison), `Range` (bounds check), `EnumVariant` (discriminant + bindings), `Wildcard`, `Ident` (binding)
- **Tuple literals** → `Alloca` for tuple struct, `GetFieldPtr` + `Store` per element
- **Enum literals** → `Alloca` for enum struct, stores discriminant + payload
- **Try operator (`?`)** → `TryUnwrap` with ok/err blocks, early return on error
- **Closures** → Lambda lifting: creates new `IrFunction` with mangled name (no environment capture yet)
- **Compound assignment** → Uses `get_value_type()` / `alloca_types` for correct `BinOp` type instead of hardcoded `I64`
- **Loop stack** → 3-tuple `(cond_block, exit_block, Option<ValueId>)` for loop-as-expression break values
- **Runtime functions** registered as externs: `__fides_print_int`, `__fides_print_str`, `__fides_print_bool`, `__fides_to_str`, `__fides_str_concat`, `__fides_panic`

### 2.5 IR Interpreter (`interp.rs`)

Executes SSA IR directly. Used by the `run` command for fast iteration.

**Value types:**
```
Value
├── Int(i64)
├── Float(f64)
├── Bool(bool)
├── Str(String)
├── Ptr(usize)
└── Unit
```

**Execution model:**
- Walks basic blocks following terminators
- Evaluates instructions and maintains a `HashMap<ValueId, Value>` per function
- Handles `Alloca` by allocating stack slots in a `HashMap<usize, Value>`
- Runtime functions are intercepted in `call_runtime()`:
  - `print` → dispatches based on value type to `__fides_print_int/str/bool`
  - `__fides_to_str` → converts `Int`/`Bool`/`Float` to `Str`; `Str` passes through
  - `__fides_str_concat` → concatenates two string values
- Returns `RunResult::Return(Value)` or `RunResult::Panic(String)`

### 2.6 PE Emitter (`pe.rs`) — Direct Native Code Generation

The **PE emitter** is the primary native backend. It encodes x86_64 instructions directly to machine code bytes and constructs a valid PE file — no assembler, linker, or C compiler required.

**Pipeline:** `IrModule` → machine code bytes → PE file → `.exe`

**Code generation:**

1. **Instruction encoding**: Full x86_64 encoder with REX prefixes, ModR/M + SIB bytes, displacement (8-bit/32-bit), and immediates
2. **Register allocation**: Stack-based alloca model — every IR value gets a stack slot at a fixed `rbp` offset; registers used as scratch within instructions
3. **Windows x64 ABI**: RCX/RDX/R8/R9 for first 4 args, 32-byte shadow space, 16-byte stack alignment before calls, RAX for return values
4. **PE file construction**: DOS header, PE header, section headers (.text + .rdata), import directory, IAT for `kernel32.dll` functions
5. **Import resolution**: `GetStdHandle`, `WriteFile`, `ExitProcess` imported from `kernel32.dll` via IAT with RIP-relative addressing
6. **Runtime stubs** (generated inline as machine code):
   - `p_print_int`: Integer to string (div/mod loop, reverse fill), `WriteFile` to stdout, newline
   - `p_print_str`: String + newline to stdout via `WriteFile`
   - `p_print_bool`: "true"/"false" to stdout
   - `p_print_float`: (stub, prints truncated int)
   - `p_panic`: Prints message + `ExitProcess`
   - `p_malloc` / `p_free`: Heap allocation (stubs)
7. **Control flow**: Conditional jumps (`jcc`), unconditional jumps (`jmp`), label-based relocation with deferred resolution
8. **Arithmetic**: Integer add/sub/mul/div/mod, float add/sub/mul/div, comparisons, bitwise and/or/xor/shl/shr
9. **Stack frame management**: `push rbp` / `mov rbp, rsp` / `sub rsp, N` prologues, pre-allocated alloca slots
10. **Non-volatile registers**: `r12`/`r13` saved/restored in runtime stubs
11. **Entry point**: Calls `main`, passes exit code to `ExitProcess`
12. **Read-only data**: String literals in `.rdata` section, referenced via RIP-relative addressing

**Instruction encoders** (selected):
- `mov_rr`, `mov_mem_disp_reg`, `mov_mem_disp_reg32` (32-bit store), `mov_r_imm`, `mov_r8_imm`
- `add_rr`, `sub_rr`, `imul_rr`, `xor_rr`, `and_rr`, `or_rr`
- `cmp_rr`, `test_rr`, `jcc_label`, `call_label`, `call_reg`, `call_rip`
- `return`, `push`, `pop`, `lea_rbp`, `sub_rsp_i32`, `add_rsp_i32`
- `mov_mem_reg8` (byte store for digit conversion)

### 2.7 x86_64 MASM Code Generator (`x86.rs`)

Generates MASM-compatible x86_64 assembly from SSA IR.

**Output format:** MASM (Microsoft Macro Assembler) syntax for `ml64.exe`

**Code generation:**

1. **Data section** (`.data`):
   - String literals as null-terminated byte arrays
   - `str_concat_buf` and `str_concat_tmp` — 4096-byte static buffers for string concatenation
   - `BYTE 4096 DUP(?)` for uninitialized data

2. **Code section** (`.code`):
   - User functions: prologue (`push rbp` / `mov rbp, rsp` / `sub rsp, N`), body, epilogue
   - Stack frame size: pre-computed from alloca count + shadow space + alignment
   - Alloca slots: pre-allocated at fixed offsets from `rbp`

3. **Calling convention** (Windows x64):
   - First 4 args: RCX, RDX, R8, R9
   - Additional args: stack
   - 32-byte shadow space allocated for each call
   - 16-byte stack alignment maintained before calls
   - Return value in RAX

4. **Runtime stubs** (generated inline):
   - `__fides_print_int`: Converts integer to string in stack buffer, calls `WriteFile`
   - `__fides_print_str`: Calls `WriteFile` with string pointer
   - `__fides_print_bool`: Prints "true" or "false"
   - `__fides_to_str`: Converts integer to null-terminated string in stack buffer (reverse fill)
   - `__fides_str_concat`: Copies left string to temp buffer, then both to output buffer (avoids aliasing)
   - `__fides_panic`: Prints message, calls `ExitProcess`

5. **Non-volatile registers**: `r12`, `r13` saved/restored in runtime stubs (Windows x64 ABI)

6. **External imports**: `GetStdHandle`, `WriteFile`, `ExitProcess` from `kernel32.lib`

7. **Entry point**: Both `main` and `_main` exported

**MASM-specific notes:**
- `mov reg, OFFSET label` for static data addressing (not `lea reg, [rip + label]` — MASM doesn't use `rip` keyword)
- `BYTE N DUP(?)` for uninitialized data (not `DUP(0)`)
- `EXTERN` declarations for kernel32 functions

### 2.8 C Code Generator (`codegen_c.rs`)

Translates SSA IR to portable C99 code that compiles with MSVC (`cl.exe`), GCC, or Clang.

**Output format:** C99 source code (`.c` file)

**Code generation:**

1. **Runtime header** (`#include` directives, `#undef` for `max`/`min`/`abs` to avoid MSVC macro conflicts, struct typedefs for `PVec`/`PMap`, runtime function declarations)
2. **String constants**: Emitted as `static const char str_N[] = "...";`
3. **Struct definitions**: Emitted as C `struct` typedefs with field layout
4. **Function prototypes**: All IR functions declared with renamed C names (`main` → `main_entry`, `abs` → `p_abs`, etc.)
5. **Function bodies**: SSA values declared as `int64_t vN;` at function start; instructions emitted as assignments; basic blocks as C labels with `goto` for control flow
6. **Polymorphic `print` dispatch**: `print` calls intercepted and dispatched to `p_print_str`/`p_print_int`/`p_print_bool`/`p_print_float` based on argument IR type, followed by `p_print_newline()`
7. **Runtime implementations**: `p_print*`, `p_panic`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str`, Vec/HashMap/String operations
8. **Main wrapper**: C `int main() { return (int)main_entry(); }`

**Function renaming:**
- `main` → `main_entry` (avoids conflict with C `main` wrapper)
- `abs` → `p_abs`, `max` → `p_max`, `min` → `p_min` (avoids MSVC intrinsic/macro conflicts)

**MSVC integration (`main.rs`):**
- `find_cl()`: Searches Visual Studio paths for `cl.exe`
- `find_vcvarsall()`: Searches for `vcvarsall.bat` to set up MSVC environment
- `ccompile` command: Generates `.c` file, invokes `cl.exe` via `vcvarsall.bat`, produces `.exe`

### 2.9 Build Pipeline (`main.rs`)

The `build` command is the primary native backend on Linux — it uses the full PIR pipeline with zero external tools:

1. `compiler::build_executable(source, path)` → PIR (L1→L2→L3) → codegen → peephole → ELF64 bytes
2. Write bytes directly to executable file
3. Print "Built native executable: <exe> (<size> bytes)"

The `emit` command produces Windows PE executables directly (bypasses PIR):

1. `compiler::emit_source_with_path(source, path)` → PE bytes (`Vec<u8>`)
2. Write bytes directly to `.exe` file
3. Print "Emitted native executable: <exe> (<size> bytes, no ml64.exe/link.exe)"

The legacy `build` MASM pipeline (Windows, external tools) is also available:

1. `compiler::build_source(source)` → assembly string
2. Write `.asm` file
3. Find `ml64.exe` (searches Visual Studio paths)
4. Run `ml64.exe /nologo /c /Fo<obj> <asm>`
5. Find `link.exe` and `kernel32.lib`
6. Run `link.exe /SUBSYSTEM:CONSOLE /ENTRY:_main /LIBPATH:<path> <obj> kernel32.lib /OUT:<exe>`
7. Print "Built native executable: <exe>"

**Tool discovery:**
- `find_ml64()`: Searches `C:\Program Files\Microsoft Visual Studio\` and `C:\Program Files (x86)\Microsoft Visual Studio\`
- `find_link()`: Same search paths
- `find_kernel32_lib()`: Searches for `kernel32.lib` in Windows SDK directories

### 2.10 Backends Overview

The Fides compiler supports multiple output backends, each consuming either IR or PIR L3:

| Backend | Source File | Input | Output | CLI Command |
|---------|-----------|-------|--------|-------------|
| **PE** (Windows) | `pe.rs` | IR (bypasses PIR) | x86_64 machine code → `.exe` | `emit` |
| **ELF64** (Linux) | `elf64.rs` | PIR L3 → codegen → peephole | ELF64 executable / relocatable `.o` | `build`, `codegen --elf` |
| **Mach-O** (macOS) | `macho.rs` | PIR L3 → codegen | Mach-O object file | `codegen --macho` |
| **COFF** (Windows) | `coff.rs` | PIR L3 → codegen | COFF object file | `codegen --coff` |
| **WASM** | `wasm.rs` | PIR L3 | WebAssembly binary (`.wasm`) | `wasm` |
| **SPIR-V** (GPU) | `gpu.rs` | PIR L3 | SPIR-V binary | `gpu` |
| **C codegen** | `codegen_c.rs` | IR (bypasses PIR) | C99 source (`.c`) → gcc/clang → executable | `ccompile` |
| **Bare-metal** | `pe.rs` (`--bare` mode) | IR | Flat binary (no stdlib, no PE headers) | `emit --bare` |
| **MASM** (legacy) | `x86.rs` | IR | MASM assembly (`.asm`) → `ml64.exe` → `link.exe` | legacy `build` (Windows) |
| **Interpreter** | `interp.rs` | IR | Direct execution (in-memory) | `run` |

**Multi-architecture codegen:** `codegen_x64.rs` (x86-64), `codegen_arm64.rs` (ARM64), and `codegen_riscv.rs` (RISC-V) all consume PIR L3 and produce assembly. Selected via `codegen --target <arch>`.

**Integrated linker:** `linker.rs` resolves relocations across multiple object files (ELF64, Mach-O, COFF) — invoked via the `link` command.

**Linux runtime:** `runtime_linux.rs` provides hand-encoded x86_64 machine code for raw Linux syscalls (write, exit, mmap) — no libc dependency. Used by the ELF64 backend.

### 2.11 AI/ML Extensions

The Fides compiler includes six AI/ML-oriented features that extend the language for neural network programming, automatic differentiation, GPU acceleration, and quantization. These features are integrated into the existing pipeline through new AST nodes, compiler passes, and a dedicated autograd module.

**Pipeline Operator (`|>`):**
- New token `PipeArrow` (`|>`) in `token.rs`, lexed by `lexer.rs`
- New AST node `Expr::PipeForward` in `ast.rs`, parsed by `parser.rs`
- Desugars `x |> f |> g` to `g(f(x))` during parsing — works with bare identifiers, method calls, and field access
- No new IR instructions — pipeline expressions are lowered to standard `Call` / `MethodCall` / `FieldAccess` IR nodes

**Layer/Model Syntax:**
- New keywords `layer` and `model` in `token.rs`, parsed into `LayerDef` and `ModelDef` AST nodes
- **Desugaring pass** in `compiler.rs::desugar_layers_models()` runs after parsing and before type-checking:
  - `layer Dense { w: Tensor, b: Tensor, forward(self, x) -> Tensor { ... } }` desugars to a `struct Dense` + `impl Dense { forward() }` + a constructor function `Dense(args)`
  - `model MLP { l1: Dense(784, 128), l2: Dense(128, 10), forward(self, x) -> Tensor { ... } }` desugars to a `struct MLP` containing layer fields + `impl MLP { forward() }`
- After desugaring, layers and models are standard structs and impls — no special handling needed in typechecker, lowerer, or backends

**Shape-Level Type Safety:**
- New AST nodes `Type::ShapedTensor`, `ShapeDim::Known`, `ShapeDim::Symbolic` in `ast.rs`
- Shape checking implemented in `typechecker.rs` — verifies shape compatibility for `matmul`, `add`, `sub`, `mul`, `transpose` at compile time
- Shape dimensions can be concrete (`784`) or symbolic (`batch`) — symbolic dimensions are tracked but not resolved
- Shape mismatches produce compile errors, not runtime crashes
- Shapes are erased during IR lowering — the IR `Tensor` type is unshaped (runtime tensors carry shape metadata)

**Autograd (`autograd.rs`):**
- New module `src/autograd.rs` implements a tape-based gradient computation framework
- New AST nodes `Stmt::GradBlock` and `Stmt::Backward` in `ast.rs`
- `grad(var) { ... backward(loss); }` syntax: the compiler traces operations within the grad block and builds a backward computation graph
- The autograd pass is hooked into the compilation pipeline after type-checking and before IR lowering:
  1. Identifies `GradBlock` statements and the variable to differentiate
  2. Traces all operations on the target variable within the block
  3. Builds a backward computation graph using reverse-mode AD
  4. Emits gradient computation code as additional IR instructions
- Supports standard tensor operations: `matmul`, `add`, `sub`, `mul`, `relu`, `softmax`, etc.

**GPU Kernel Annotation:**
- New token `At` (`@`) in `token.rs`, lexed by `lexer.rs`
- New `Annotation` struct on `Func` in `ast.rs` — parsed by `parser.rs`
- `@kernel` annotation marks functions as GPU kernels
- Builtins: `thread_id(dim)`, `workgroup_size(dim)`, `workgroup_id(dim)`, `num_workgroups()` — available within `@kernel` functions
- On CPU: `thread_id()` returns 0 (single-threaded fallback) — enables testing GPU kernels in the interpreter
- On GPU: maps to SPIR-V `GlobalInvocationId` / PTX `threadIdx`
- Enhanced SPIR-V codegen in `gpu.rs` with configurable workgroup sizes

**Quantized Types:**
- New token `TildeArrow` (`~>`) in `token.rs`, lexed by `lexer.rs`
- New AST node `Expr::QuantCast` in `ast.rs` — parsed by `parser.rs`
- `q8` and `q4` type aliases map to `i8` storage — registered in the typechecker
- `expr ~> q8` quantization cast operator — lowered to calls to `quantize_q8` / `quantize_q4` from `stdlib/quant.fd`
- New stdlib module `stdlib/quant.fd` with `quantize_q8`, `dequantize_q8`, `qmatmul`, `find_quant_params`

---

## 3. Verification Infrastructure

The compiler includes a suite of scripts and conventions for maintaining correctness across all backends:

| Tool | Purpose |
|------|---------|
| `verify.sh` | Full verification: build (release) + run all tests + compare to baseline. Use `--save` to update baseline, `--quick` for build-only check. |
| `scripts/bisect.sh <test>` | Bisect which compiler layer (lowering, PIR, codegen, peephole) broke a specific test. |
| `scripts/gen_golden.sh` | Generate golden files for intermediate compiler stages (IR, PIR L1/L2/L3, codegen, peephole) for future bisect accuracy. |
| `AGENTS.md` | Working guide for AI assistants and humans: build commands, high-risk file list, known bugs, coding conventions, debugging workflow. **Read before making changes.** |

**Workflow:** Run `./verify.sh` before and after every change. If tests regress, use `./scripts/bisect.sh <test_name>` to isolate the layer. If codegen/regalloc/peephole/elf64/pe changed, also run `./scripts/gen_golden.sh` to keep golden files current.

---

## 4. Source File Map

| File | Responsibility |
|------|---------------|
| `src/main.rs` | CLI entry point, command dispatch, external tool invocation |
| `src/token.rs` | `TokenKind` enum (all Fides keywords and operators) |
| `src/lexer.rs` | Tokenizer: source text → `Vec<Token>` |
| `src/ast.rs` | AST node definitions: `Program`, `Func`, `Stmt`, `Expr`, `StructDef`, `EnumDef`, etc. |
| `src/parser.rs` | Recursive descent parser: `Vec<Token>` → `Program` |
| `src/typechecker.rs` | Type validation: `Program` → `Ok` or `Err(Vec<String>)` |
| `src/ir.rs` | SSA IR definitions: `IrModule`, `IrFunction`, `BasicBlock`, `Inst`, `IrType`, `ValueId` |
| `src/lower.rs` | AST → SSA IR lowering: `Program` → `IrModule`. Auto-deref for &T/&mut T method calls. Drop trait: compiler-inserted `T_drop` calls at function exit (LIFO). `emit_drops()` at return, implicit end, `?` propagation. |
| `src/interp.rs` | IR interpreter: `IrModule` → execution result. UAF detection: `freed_addresses` + `region_addresses` tracking, Load/Store panic on use-after-free. Re-entrant region blocks with depth-keyed keys. |
| `src/pe.rs` | **Direct PE emitter**: `IrModule` → x86_64 machine code → PE `.exe` (no external tools) |
| `src/elf64.rs` | **ELF64 emitter**: PIR L3 → codegen → peephole → ELF64 executable (Linux, zero external tools) + relocatable objects |
| `src/macho.rs` | **Mach-O emitter**: object files for macOS |
| `src/coff.rs` | **COFF emitter**: object files for Windows |
| `src/linker.rs` | **Integrated linker**: resolves relocations across multiple object files |
| `src/runtime_linux.rs` | Hand-encoded x86_64 Linux runtime (raw syscall bytes, no libc) |
| `src/x86.rs` | x86_64 MASM codegen: `IrModule` → assembly string (legacy `build` path) |
| `src/codegen_x64.rs` | PIR L3 → x86_64 assembly (used by `build` and `codegen --target x86-64`) |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly (`codegen --target arm64`) |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly (`codegen --target riscv`) |
| `src/codegen_c.rs` | C codegen backend: `IrModule` → C99 source code (for `ccompile` command) |
| `src/codegen.rs` | Legacy codegen dispatch (unused) |
| `src/wasm.rs` | **WASM backend**: PIR L3 → WebAssembly binary (`wasm` command) |
| `src/gpu.rs` | **SPIR-V GPU backend**: PIR L3 → SPIR-V binary (`gpu` command) |
| `src/embedded.rs` | Embedded systems support: `FixedBufferAllocator`, `RealTime` effect, HAL abstractions |
| `src/hotreload.rs` | **Hot reload**: native JIT (ELF64) + function-level change detection (`watch` command) |
| `src/cparser.rs` | **C header parser**: parses C `.h` files → generates Fides FFI bindings (`parse-header` command) |
| `src/pir.rs` | **PIR**: multi-level IR (L1/L2/L3) — `IrModule` → PIR |
| `src/comptime.rs` | **Compile-time eval**: constant folding, reflection, f-string interpolation |
| `src/opt.rs` | **PIR optimizer**: 23 passes (const-fold, CSE, GVN, SROA, DSE, DCE, inline, unroll, TCO, alias-analysis, effect-reorder, partial-eval, vectorize, strength-reduce, devirt, jump-thread, loop-rotate, loop-vec, effect-opt, region-pool, reassociate, memoize). 22 in default pass list. Enabled by default in `build` (use `--no-opt` to disable). `--lto` for aggressive cross-module inlining. |
| `src/peephole.rs` | **9-pass peephole optimizer**: always on in `build` (redundant moves, zero idiom, branch threading, store-to-load, LEA coalescing, instruction selection) |
| `src/regalloc.rs` | **Linear scan register allocator** (⚠️ known bug: constants clobbered by intervening instructions) |
| `src/mono.rs` | Generic monomorphization: specializes generic functions per call site |
| `src/compiler.rs` | Pipeline orchestration, module resolution, stdlib auto-include, layer/model desugaring |
| `src/lsp.rs` | **LSP server**: JSON-RPC 2.0 over stdio, diagnostics, no external deps |
| `src/autograd.rs` | **Autograd**: tape-based gradient computation framework — reverse-mode AD for `grad` blocks |
| `src/borrowck.rs` | **Borrow checker**: Rules 1-3, field-wise borrows, NLL, @send/@sync, closures (Expr::Closure handled in new scope) |
| `stdlib/core.fd` | Core standard library (Option<T>, Result<T, E>, println, assert, assert_eq) |
| `tests/*.fd` | Test programs (hello, struct, struct_return, features2-6, control_flow, functions, strings) |

---

## 5. Data Flow Example

For `tests/bootstrap/strings.fd`:

```p
fn main() {
    print("Hello, World!");
    val name = "Fides";
    print(name);
    val greeting = f"Hello, {name}!";
    print(greeting);
    val x = 42;
    print(f"x = {x}");
}
```

1. **Lexer** produces tokens: `Keyword(Pf)`, `Ident("main")`, `LParen`, `RParen`, `LBrace`, `Ident("print")`, `LParen`, `StrLit("Hello, World!")`, `RParen`, `Semicolon`, `Keyword(Pv)`, `Ident("name")`, `Assign`, `StrLit("Fides")`, `Semicolon`, ...

2. **Parser** builds AST with `Func { name: "main", body: [Expr(Call(print, "Hello, World!")), Let(name, "Fides"), Expr(Call(print, Ident(name))), Let(greeting, FString([Literal("Hello, "), Expr(Ident(name)), Literal("!")])), ...] }`

3. **TypeChecker** validates: `print` takes `str`, `name` is `str`, f-string parts are valid, etc.

4. **Lowerer** produces IR:
   - `ConstStr("Hello, World!")` → `Call(__fides_print_str, [str])`
   - `ConstStr("Fides")` → `Store(alloca_name, str)`
   - `Load(alloca_name)` → `Call(__fides_print_str, [str])`
   - `ConstStr("Hello, ")` + `Load(alloca_name)` (already Ptr, skip `__fides_to_str`) + `Call(__fides_str_concat, [prev, name])` + `ConstStr("!")` + `Call(__fides_str_concat, [prev, "!"])` → `Call(__fides_print_str, [result])`
   - `Const(42)` → `Call(__fides_to_str, [42])` → concat with `ConstStr("x = ")` → `Call(__fides_print_str, [result])`

5. **Interpreter** executes: prints "Hello, World!", "Fides", "Hello, Fides!", "x = 42"

6. **x86 Codegen** generates MASM assembly with:
   - String literals in `.data`
   - `main` function with prologue, calls to runtime stubs, epilogue
   - Runtime stubs for `__fides_print_str`, `__fides_to_str`, `__fides_str_concat`
   - `EXTERN GetStdHandle`, `WriteFile`, `ExitProcess`

7. **ml64.exe** assembles `.asm` → `.obj`

8. **link.exe** links `.obj` + `kernel32.lib` → `.exe`

9. **.exe** runs and produces the same output as the interpreter

> **PE emitter path:** Steps 6-8 are replaced by `pe.rs` which encodes x86_64 instructions directly to machine code bytes and constructs a PE file — no `ml64.exe` or `link.exe` needed.
>
> **Alternative path (C codegen):** Step 6 can be replaced by `codegen_c.rs` which generates a `.c` file, then `cl.exe`/`gcc`/`clang` compiles it directly to a `.exe`. The output is identical.

---

## Phase 7.5: Financial Safety & Production Primitives

This phase introduces compile-time guarantees for financial correctness and production-grade language primitives for I/O, transactions, resource accounting, and auditability.

### Linear Types

- `@linear` annotation on structs prevents copying (move semantics)
- Typechecker tracks consumed variables, rejects double-use
- Loop-aware: linear variables can't be consumed inside loop bodies
- Files: `src/typechecker.rs` (`linear_structs`, `consumed_vars`, `loop_depth`)

### @conserve Annotation

- Functions marked `@conserve` can't: mint money, do IO/Net, use floats, call non-`@conserve` functions
- Compile-time proof of financial conservation
- Files: `src/typechecker.rs` (`verify_conserve`)

### Money & Banking Stdlib

- `stdlib/money.fd`: `@linear` struct `Money`, `@conserve` operations (add, sub, split, merge)
- `stdlib/banking.fd`: `@linear` struct `Account`, deposit/withdraw/transfer/interest
- All operations `@conserve` except mint (`Money_new`, `Account_open`) and IO (`print`)

### Tuple Destructuring

- `val (a, b) = expr` — unpack multi-return values
- AST: `Stmt::LetTuple` variant
- Lowerer: extracts fields with correct types (`Ptr` for structs, `I64` for scalars)

### File I/O

- `stdlib/file.fd`: `file_read`, `file_write`, `file_append`, `file_exists`
- Interpreter: uses host OS filesystem
- C codegen: uses `fopen`/`fread`/`fwrite`
- Native: will use Linux syscalls

### txn {} Blocks

- Atomic transaction blocks with rollback semantics
- AST: `Stmt::Txn` variant
- Currently transparent (body lowered as normal statements)
- Future: interpreter-level memory snapshot for rollback

### @gas Annotation

- Compile-time gas budget enforcement
- Cost model: AST node count
- Transitive: total gas = own cost + sum of callee costs
- Files: `src/typechecker.rs` (`verify_gas`, `compute_total_gas`)

### @audit Annotation

- Auto-instruments function entry/exit with logging
- IR injection in `lower.rs`
- Full audit trail (timestamps, params, crypto signing) pending

### @crashfree Annotation

- Compile-time proof that a function cannot panic at runtime
- Rejects: `unwrap()`, `p_panic()`, non-const division/modulo, `while` loops, non-`@crashfree` calls
- Allows: `for` loops over finite ranges, `match` on Option/Result, `unwrap_or()`, const division
- SPARK Ada "absence of runtime errors" model, pattern-based static check
- Files: `src/typechecker.rs` (`verify_crashfree`, `scan_for_division`, `scan_for_while_loops`)

### LogicalTime Stdlib (stdlib/time.fd)

- Timezone-free, clock-free time type for deterministic date math
- `LogicalTime` struct: stores Unix timestamp as `i64` seconds
- All operations `@deterministic` (pure integer math, no IO, no randomness)
- Construction: `time_new(year, month, day, hour, min, sec)`, `time_from_timestamp(ts)`
- Extraction: `time_get_year/month/day/hour/minute/second/weekday/day_of_year`
- Arithmetic: `time_add_seconds/minutes/hours/days`, `time_diff_seconds/days`
- Comparison: `time_before/after/eq/before_eq`
- Formatting: `time_to_iso(t)` → "YYYY-MM-DDTHH:MM:SSZ", `time_to_date_string(t)`
- Calendar: `is_leap_year(year)`, `days_in_month(year, month)`
- Uses Howard Hinnant's `civil_from_days` algorithm for exact date math
- Prevents timezone bugs and clock skew bugs in financial systems
- NOT `@linear` — value type, freely copyable
