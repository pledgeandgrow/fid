# Fides Custom Toolchain — Why and How

> If we depend on it, we build it. No LLVM. No libc. No external linker.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.fd`, `features.md`).*

## Why Custom Everything?

### The LLVM Trap
Every modern language (Rust, Zig, Swift, Crystal, Julia) uses LLVM. This means:
- They all share the same optimization limitations
- They all share the same compile-time characteristics
- They all share the same binary format constraints
- None can leverage language-specific semantics in the optimizer
- None can fix LLVM bugs without going through LLVM's contribution process

**Using LLVM would make Fides "just another LLVM frontend."** We would gain nothing architecturally over Rust or Zig. Our semantic information (regions, effects, capabilities) would be lost when lowering to LLVM IR.

### The libc Trap
Most languages depend on libc for basic operations (malloc, printf, file I/O). This means:
- They inherit libc's security vulnerabilities
- They inherit libc's binary bloat
- They can't run on bare metal without a libc port
- They're tied to POSIX/C semantics

**Fides's custom runtime eliminates the libc dependency for Fides binaries.** On Linux, `runtime_linux.rs` implements `p_print_*`, `p_panic`, `p_alloc`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str` as hand-encoded raw x86-64 machine code using direct syscalls (System V AMD64 ABI, no libc calls). On Windows, `pe.rs` links against `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — the minimum Windows ABI surface, not the C runtime.

### The Linker Trap
External linkers (ld, lld, link.exe) are separate processes that:
- Don't understand language semantics
- Add 50%+ to build time for large projects
- Require object file formats that lose information
- Can't do cross-module optimization

**Fides's integrated linker** (`linker.rs`) is part of the compiler — no separate process, no information loss, incremental linking, link-time dead code elimination. Resolves `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64` relocations in-process.

---

## What We Built

### 1. Custom Lexer — ✅ Complete
- Hand-written tokenizer in `src/lexer.rs`
- No lexer generator (no flex, no nom)
- All Fides keywords (`fn`, `val`, `const`, `return`, etc.), operators, literals (decimal/hex/binary/octal/underscored ints, f32/f64 floats, strings, f-strings with brace-depth tracking)
- Line/column tracking for error messages

### 2. Custom Parser — ✅ Complete
- Recursive descent parser in `src/parser.rs`
- No parser generator (no yacc, no ANTLR)
- Full AST: functions, structs, enums, impls, type aliases, consts, imports, attributes
- Block expressions, closures, match arms with guards and range patterns

### 3. Custom Typechecker — ✅ Complete
- Region inference + effect system + capability checking, all in `src/typechecker.rs`
- No external SMT solver (everything built in)
- **Regions**: compile-time use-after-free detection via `freed_regions` tracking; escape detection via `region_stack`; auto-inference
- **Effects**: `!IO`, `!Net`, `!Spawn`, `!Alloc`, `!{}` (pure); fixpoint iteration over call graph for inference; under-declaration is a compile error
- **Capabilities**: `cap` declarations, `use ... with { IO, FS, Net, Spawn }` grants, transitive propagation, compile-time denial, zero runtime overhead (capability elision)

### 4. Custom IR — PIR — ✅ Complete
- Multi-level IR in `src/pir.rs`: L1 (semantic) → L2 (mid-level) → L3 (low-level)
- Typed, SSA-based, human-readable
- Preserves region/effect/capability information through optimization
- `fidesc pir <file> [1|2|3|all]` to inspect each level
- Underlying single-level SSA IR still in `src/ir.rs` (used by interpreter and PE/ELF paths)

### 5. Custom Optimizer — ✅ Enabled by Default
- Pass-based optimization in `src/opt.rs` (10 passes): const fold, CSE, DCE, inline, unroll, TCO, alias analysis, effect reordering, partial eval, vectorize
- Region-aware alias analysis (free, no pointer analysis)
- Effect-based reordering (pure functions can reorder)
- Capability elision (remove checks when provably safe)
- comptime partial evaluation (`src/comptime.rs`)
- **Enabled by default** in `build` (use `--no-opt` to disable). `--lto` enables aggressive cross-module inlining. PGO has been removed.
- Peephole optimizer (`src/peephole.rs`) — 9 passes, **always on** in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection

### 6. Custom Codegen — ✅ Multi-Arch + Cross-Compilation
- **x86-64**: PE emitter (`src/pe.rs`, direct IR→machine code, Windows), x86 MASM (`src/x86.rs`), PIR→x86-64 (`src/codegen_x64.rs`)
- **ARM64**: `src/codegen_arm64.rs` (assembly output) + `src/arm64_enc.rs` (assembly→machine code encoder) + `src/runtime_arm64.rs` (ARM64 Linux runtime with SVC syscalls)
- **RISC-V**: `src/codegen_riscv.rs` (assembly output) + `src/riscv_enc.rs` (assembly→machine code encoder) + `src/runtime_riscv.rs` (RISC-V Linux runtime with ECALL syscalls)
- **WASM**: `src/wasm.rs` (PIR → WebAssembly binary, `fidesc wasm`)
- **SPIR-V / GPU**: `src/gpu.rs` (PIR → SPIR-V binary, `fidesc gpu`)
- **C source**: `src/codegen_c.rs` (IR → C99, portable fallback compiled by gcc/clang/cl.exe)
- **Register allocator**: `src/regalloc.rs` — linear scan; has known bug (see Optimizer caveat above)
- `fidesc codegen <file> --target <arch>` for assembly; `--elf|--macho|--coff` for object files
- **Cross-compilation**: `fidesc build --target arm64-linux|riscv64-linux|x86-64-windows` — zero external tools, built-in encoders + runtimes

### 7. Custom Linker / Binary Emitters — ✅ Multi-Format
- **PE**: `src/pe.rs` — Windows `.exe`, direct byte emission, no `link.exe`/`ml64.exe`
- **ELF64**: `src/elf64.rs` — Linux executables + relocatable object files, no external `ld`
- **Mach-O**: `src/macho.rs` — macOS object files
- **COFF**: `src/coff.rs` — Windows object files
- **Bare-metal flat binary**: `emit --bare` — no headers, no stdlib, raw machine code at fixed address
- **Integrated linker**: `src/linker.rs` — links ELF64 objects into executables, resolves relocations in-process, incremental linking, link-time DCE. Target-aware: handles ARM64 (BL, B, B.cond, ADRP, ADD:lo12) and RISC-V (JAL, branch) relocations. Emits EM_AARCH64 (183) and EM_RISCV (243) machine types.
- `fidesc link <files...>` to link multiple object files

### 8. Custom Runtime — ✅ No libc for Fides Binaries
- **Linux**: `src/runtime_linux.rs` — hand-encoded x86-64 machine code (raw bytes, not assembly text) implementing all runtime stubs via direct syscalls. System V AMD64 ABI. No libc.
- **Windows**: `src/pe.rs` runtime stubs link only `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — not the MSVC C runtime.
- **Bare metal**: no runtime — `--bare` mode emits flat binary with no stdlib
- Panic handler (`p_panic`), print stubs (`p_print_str/int/bool/float/newline`), allocator (`p_malloc`/`p_free`/`p_alloc`), string ops (`p_str_concat`/`p_to_str`), Vec/HashMap/String ops

### 9. Custom Stdlib — ✅ 35 Modules
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

### 10. Custom Allocator — ✅ Partial
- `src/embedded.rs` — `FixedBufferAllocator`: pre-allocated buffer, no heap fragmentation, deterministic allocation patterns for embedded/IoT
- General path: `p_malloc`/`p_free` wrap the platform allocator (HeapAlloc on Windows, hand-coded mmap/brk on Linux runtime)
- Region allocator (arena-based, tied to the region system): planned, not yet implemented
- `!RealTime` effect for bounded execution time guarantees: framework in `embedded.rs`

### 11. Embedded & IoT Support — ✅ Phase 5B
- RISC-V target via `codegen_riscv.rs`
- Deterministic memory mode (`FixedBufferAllocator`)
- Binary size optimization (< 4KB for simple programs)
- Real-time guarantees (`!RealTime` effect — bounded execution time)
- HAL framework
- Examples: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`

### 12. Hot Reload — ✅
- `src/hotreload.rs` + `fidesc watch`
- Native JIT via ELF64 emission
- Function-level change detection via L2 IR hashing
- Recompiles and swaps only changed functions

### 13. C Header FFI — ✅
- `src/cparser.rs` + `fidesc parse-header <file.h>`
- Parses C headers and generates Fides FFI bindings automatically
- `extern "C"` functions: parsed, lowered, C codegen, interpreter stubs

---

## Dependencies We Accept (For Now)

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

## The Path to Zero Dependencies

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

## Binary Output — Current State

### PE Emission (Windows) — ✅
- Direct byte emission of PE32+ format in `src/pe.rs`
- No assembler, no linker, no `ml64.exe`, no `link.exe`
- Custom section layout (`.text`, `.data`, `.rdata`)
- Custom import table (only `kernel32.dll`: ExitProcess, GetStdHandle, WriteFile, HeapAlloc, HeapFree, GetProcessHeap)
- RIP-relative addressing, Rel32 relocations, Win64 ABI (RCX/RDX/R8/R9 + shadow space)
- `fidesc emit <file> -o <exe>` produces a working `.exe` in one step

### ELF64 Emission (Linux) — ✅
- Direct byte emission in `src/elf64.rs`
- Primary native backend on Linux via `fidesc build`
- Full PIR pipeline: L1 → L2 → L3 → codegen → peephole → ELF64
- Relocations: `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64`
- Links against hand-encoded runtime (`runtime_linux.rs`) — raw machine code, no libc
- Also emits relocatable object files (`.o`)
- Zero external tools: no `as`, no `ld`, no `gcc`

### Mach-O (macOS) — ✅
- `src/macho.rs` — object file emission
- `fidesc codegen <file> --macho <out.o>`

### COFF (Windows) — ✅
- `src/coff.rs` — object file emission
- `fidesc codegen <file> --coff <out.o>`

### Bare-Metal Flat Binary — ✅
- `fidesc emit <file> --bare -o <bin>`
- No file format headers — raw machine code at a fixed address
- No stdlib, no runtime
- For bootloaders, OS kernels, firmware, microcontrollers
- Examples: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`, `tests/test_boot.fd`, `tests/test_bare.fd`

### WASM — ✅
- `src/wasm.rs` — PIR → WebAssembly binary
- `fidesc wasm <file> -o <out.wasm>`

### SPIR-V / GPU — ✅
- `src/gpu.rs` — PIR → SPIR-V binary
- `fidesc gpu <file>`

### C Source (portability fallback) — ✅
- `src/codegen_c.rs` — IR → C99 source
- `fidesc ccompile <file> -o <exe>` generates `.c`, invokes `gcc`/`clang`/`cl.exe`
- Used when targeting platforms the native backends don't yet cover
- Note: 146/146 test parity with interpreter. Binary hardening enabled.

---

## Why This Matters for Performance

### Compilation Speed
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

### Binary Size
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

## Source File Map (Toolchain-Relevant)

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

# Fides Architecture — The Full Stack

> Every layer built by us. No LLVM. No borrowed infrastructure.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.fd`, `features.md`).*

## Current Architecture (Phase 8 — What We Have Now)

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
- 69 stdlib modules: core, vec, math, string, fmt, bits, sort, hash, iter, stack, queue, buffer, collections, result, random, path, units, uuid, crypto, storage, clinical, medical, ecu, scada, swift, grid, ota, banking, kv_cache, quant, dp, shard, fs, io, os, sys, ecs, events, parallel, sync, channel, db, http, json, shape, simd, tensor, money, alloc, file, time, typed_strings, memo, privacy, env, args, process, regex, tilemap, sprite, flowfield, streaming, scene, physics, audio, input, arinc, can
- C header FFI parsing (`fidesc parse-header`)
- Embedded/IoT: FixedBufferAllocator, `!RealTime` effect, RISC-V target, HAL framework
- Smart pointers: `Box<T>` (recursive types), `Rc<T>` / `Arc<T>` (manual refcount management)
- Vec/Mat math types: vec2/3/4, mat4, quat with operator overloading + swizzle access
- Incremental compilation: in-memory cache (`src/inc.rs`), shared across LSP/watch/test harness
- Profiler (`fidesc profile`), linter (`fidesc lint`), benchmark (`fidesc bench`), formatter (`fidesc fmt`)
- Package manager (`pacte`), doc generator (`scribe`), coverage tool (`veil`)
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

## PIR — Fides Intermediate Representation

### Design Principles

1. **Typed**: every value has a type, types carry semantic information
2. **Multi-level**: high-level → mid-level → low-level, each preserves different info
3. **Semantic-aware**: regions, effects, capabilities are first-class in PIR
4. **SSA-based**: static single assignment for optimization passes
5. **Target-independent at high level**: same PIR for all architectures
6. **Human-readable**: debuggable, inspectable (`fidesc pir <file> [1|2|3|all]`)

### PIR Level 1 — High-Level (Semantic)

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

### PIR Level 2 — Mid-Level (Control Flow)

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

### PIR Level 3 — Low-Level (Machine-Near)

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

## Custom Codegen — No LLVM

### Why Not LLVM?
- LLVM is a 20-year-old C++ codebase designed for C/C++
- Its IR loses high-level semantics
- Its optimization passes are generic, not language-specific
- Its compile times are slow (100+ passes)
- Its linker (lld) is separate and expensive
- We can't control its behavior precisely
- Using it would make Fides "just another LLVM frontend" — no architectural advantage over Rust or Zig

### What We Built Instead
- **x86-64 backend**: PE emitter (`src/pe.rs`, direct IR→machine code), PIR codegen (`src/codegen_x64.rs`), MASM (`src/x86.rs`)
- **ARM64 backend**: `src/codegen_arm64.rs` — for Apple Silicon, embedded, servers
- **RISC-V backend**: `src/codegen_riscv.rs` — for IoT, custom hardware, research
- **WASM backend**: `src/wasm.rs` — PIR → WebAssembly binary (`fidesc wasm`)
- **SPIR-V / GPU backend**: `src/gpu.rs` — PIR → SPIR-V binary (`fidesc gpu`)
- **C source backend**: `src/codegen_c.rs` — IR → C99, portability fallback compiled by gcc/clang/cl.exe

### Codegen Strategy
```
PIR Level 3 → Register Allocator → Instruction Selector → Peephole → Binary Output
```

- **Register allocator**: linear scan (`src/regalloc.rs`) — ⚠️ has known bug (constants not loaded into registers before use can be clobbered by intervening instructions)
- **Instruction selector**: pattern matching on PIR → machine instructions per architecture
- **Peephole**: 9 local optimization passes (always on in `build`): redundant moves, zero idioms, nop arithmetic, branch threading, dead stores, redundant tests, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection
- **Binary output**: direct to ELF/PE/Mach-O/COFF/WASM/SPIR-V (no external assembler/linker for native targets)

---

## Custom Linker — Integrated, Not Separate

### Why Not lld/ld?
- Linking is 50%+ of build time for large projects
- External linker doesn't understand PIR semantics
- Separate linker = separate process = IPC overhead

### What We Built
- **Integrated linker** (`src/linker.rs`): part of the compiler, not a separate process
- **ELF64 object linking**: resolves `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64` relocations in-process
- **Incremental linking**: cache object files, only re-link changed modules
- **Link-time dead code elimination**: remove unused functions (smaller binaries)
- **Multi-file linking**: `fidesc link <files...>`
- **Runtime symbol resolution**: links against hand-encoded Fides runtime (`src/runtime_linux.rs`)

---

## Custom Runtime — Minimal, No libc

### What We Have
- **Linux runtime** (`src/runtime_linux.rs`): hand-encoded x86-64 machine code (raw bytes, not assembly text) implementing all `p_print_*`, `p_panic`, `p_alloc`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str` via direct syscalls. System V AMD64 ABI. **No libc dependency.**
- **Windows runtime** (in `src/pe.rs`): links only `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — not the MSVC C runtime.
- **Bare metal**: no runtime — `--bare` mode emits flat binary with no stdlib
- **Panic handler** (`p_panic`): for unrecoverable errors (no exceptions)

### What We DON'T Need
- No garbage collector
- No runtime type information (RTTI)
- No virtual machine
- No JIT in production mode (hot reload JIT is dev-only)
- No reflection (comptime replaces it)
- No libc (raw syscalls on Linux, kernel32.dll only on Windows)

### What's Planned
- Region allocator: arena-based, tied to the region system (not yet started)
- Effect runtime: for effect handlers that can't be compiled away (full continuation semantics pending)

---

## Custom Stdlib — 34 Modules

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

## Embedded & IoT Support (Phase 5B)

- **RISC-V target**: `src/codegen_riscv.rs` — assembly output for IoT/custom hardware
- **Deterministic memory**: `FixedBufferAllocator` in `src/embedded.rs` — pre-allocated buffer, no heap fragmentation
- **Binary size optimization**: < 4KB for simple programs
- **Real-time guarantees**: `!RealTime` effect — bounded execution time
- **HAL framework**: hardware abstraction layer in `src/embedded.rs`
- **Bare-metal mode**: `fidesc emit <file> --bare -o <bin>` — no headers, no stdlib, raw machine code
- **Examples**: `examples/rpi_led_blink.fd`, `examples/riscv_blink.fd`, `tests/test_boot.fd`, `tests/test_bare.fd`

---

## Hot Reload (Phase 4D)

- `src/hotreload.rs` + `fidesc watch <file>`
- Native JIT: compiles to ELF64 in memory, runs immediately
- Function-level change detection via L2 IR hashing
- Recompiles and swaps only changed functions
- Sub-second iteration for development

---

## C FFI

- `src/cparser.rs` + `fidesc parse-header <file.h>` — parses C headers, generates Fides FFI bindings
- `extern "C"` declarations: parsed, lowered, C codegen, interpreter stubs
- Tests: `test_ffi.fd`, `test_ffi_full.fd`, `test_ffi_header.fd`

---

## Bootstrapping Strategy

### Phase 1–4D: Fides in Rust (Current)
- Compiler written in Rust (`fidesc` binary)
- Compiles Fides source to native code via custom pipeline
- Self-hosting IR, PIR, codegen, linker, runtime — all custom-built
- 4 Rust crates used to build `fidesc`: `hashbrown`, `compact_str`, `smallvec`, `bumpalo`

### Phase 5: Custom Everything for Fides Binaries
- Custom file I/O via raw syscalls on Windows too (drop kernel32.dll for Fides binaries)
- Region allocator tied to region system
- Full effect handler continuation semantics
- `async` / `await` / `spawn` implementation

### Phase 6: Fides in Fides (Self-Hosting)
- Rewrite compiler in Fides
- Fides compiler compiles itself
- No Rust dependency for end users
- Compiler binary is a Fides binary
- PIR optimizer runs on Fides compiler code
- comptime specializes the compiler for the target
- Compiler becomes faster by optimizing itself
- **ZERO external dependencies**
