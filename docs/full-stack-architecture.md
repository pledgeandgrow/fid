# Fides Architecture — The Full Stack

> Every layer built by us. No LLVM. No borrowed infrastructure.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.fd`, `features.md`).*

## Current Architecture (Phase 4D — What We Have Now)

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
│  Optimizer (src/opt.rs)             — opt-in (has regalloc bug)      │
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
- PIR optimizer: const fold, CSE, DCE, inline, unroll, TCO, effect elision (opt-in — has regalloc bug)
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
- 8 stdlib modules: core, json, tensor, parallel, db, http, simd, shape
- C header FFI parsing (`fidesc parse-header`)
- Embedded/IoT: FixedBufferAllocator, `!RealTime` effect, RISC-V target, HAL framework
- No external dependencies for primary paths: no LLVM, no libc (in Fides binaries), no external assembler, no external linker

**What's partial / pending:**
- ⚠️ PIR optimizer: works for simple programs, exposes pre-existing register allocator bug on complex ones (not enabled by default)
- ⚠️ Effect handlers: `handle { do } with` syntax parses and runs; full continuation semantics pending
- ❌ `async` / `await` / `spawn`: parsed, not yet implemented
- ❌ `#[test]` attribute: parsed but not executed by `fidesc test`
- ❌ `#[derive]`: not yet implemented
- ❌ Default arguments, variadic arguments: not implemented
- ❌ Region allocator (arena-based, tied to region system): not started
- ❌ Self-hosting (Phase 6): compiler is still written in Rust

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

**Optimizations at this level** (in `src/opt.rs`, opt-in):
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
    ret
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

## Custom Stdlib — 8 Modules

| Module | File | Contents |
|--------|------|----------|
| core | `stdlib/core.fd` | Auto-included: `Option`, `Result`, `println`, `assert`, `assert_eq`, `max`, `min`, `abs`, `clamp` |
| json | `stdlib/json.fd` | parse, stringify, get, at |
| tensor | `stdlib/tensor.fd` | n-d arrays, matmul, relu, softmax |
| parallel | `stdlib/parallel.fd` | par_map, par_sum, par_dot |
| db | `stdlib/db.fd` | `cap Database` trait, in-memory KV store |
| http | `stdlib/http.fd` | socket + HTTP parsing |
| simd | `stdlib/simd.fd` | elementwise ops, dot, sum, scale (f64x2, f64x4, etc.) |
| shape | `stdlib/shape.fd` | shape system for tensor ops |

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
