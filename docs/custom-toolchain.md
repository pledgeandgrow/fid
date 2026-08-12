# P Custom Toolchain — Why and How

> If we depend on it, we build it. No LLVM. No libc. No external linker.

*Last updated: Aug 2026 — verified against source tree (`src/*.rs`, `stdlib/*.p`, `features.md`).*

## Why Custom Everything?

### The LLVM Trap
Every modern language (Rust, Zig, Swift, Crystal, Julia) uses LLVM. This means:
- They all share the same optimization limitations
- They all share the same compile-time characteristics
- They all share the same binary format constraints
- None can leverage language-specific semantics in the optimizer
- None can fix LLVM bugs without going through LLVM's contribution process

**Using LLVM would make P "just another LLVM frontend."** We would gain nothing architecturally over Rust or Zig. Our semantic information (regions, effects, capabilities) would be lost when lowering to LLVM IR.

### The libc Trap
Most languages depend on libc for basic operations (malloc, printf, file I/O). This means:
- They inherit libc's security vulnerabilities
- They inherit libc's binary bloat
- They can't run on bare metal without a libc port
- They're tied to POSIX/C semantics

**P's custom runtime eliminates the libc dependency for P binaries.** On Linux, `runtime_linux.rs` implements `p_print_*`, `p_panic`, `p_alloc`, `p_malloc`, `p_free`, `p_str_concat`, `p_to_str` as hand-encoded raw x86-64 machine code using direct syscalls (System V AMD64 ABI, no libc calls). On Windows, `pe.rs` links against `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — the minimum Windows ABI surface, not the C runtime.

### The Linker Trap
External linkers (ld, lld, link.exe) are separate processes that:
- Don't understand language semantics
- Add 50%+ to build time for large projects
- Require object file formats that lose information
- Can't do cross-module optimization

**P's integrated linker** (`linker.rs`) is part of the compiler — no separate process, no information loss, incremental linking, link-time dead code elimination. Resolves `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64` relocations in-process.

---

## What We Built

### 1. Custom Lexer — ✅ Complete
- Hand-written tokenizer in `src/lexer.rs`
- No lexer generator (no flex, no nom)
- All P keywords (`pf`, `pv`, `pc`, `ret`, etc.), operators, literals (decimal/hex/binary/octal/underscored ints, f32/f64 floats, strings, f-strings with brace-depth tracking)
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
- `pledgec pir <file> [1|2|3|all]` to inspect each level
- Underlying single-level SSA IR still in `src/ir.rs` (used by interpreter and PE/ELF paths)

### 5. Custom Optimizer — ⚠️ Works, Not Enabled by Default
- Pass-based optimization in `src/opt.rs`: const fold, CSE, DCE, inline, unroll, TCO, effect-based elision
- Region-aware alias analysis (free, no pointer analysis)
- Effect-based reordering (pure functions can reorder)
- Capability elision (remove checks when provably safe)
- comptime partial evaluation (`src/comptime.rs`)
- **Caveat:** exposes a pre-existing register allocator bug (`regalloc.rs` — constants not loaded into registers before use can be clobbered by intervening instructions). Works for simple programs; complex ones miscompile. Not enabled by default.
- Peephole optimizer (`src/peephole.rs`) — 9 passes, **always on** in `build` command: redundant moves, zero idiom, nop arithmetic, branch threading, dead store, redundant test, store-to-load forwarding, LEA coalescing, cmp+setcc+jcc instruction selection

### 6. Custom Codegen — ✅ Multi-Arch
- **x86-64**: PE emitter (`src/pe.rs`, direct IR→machine code, Windows), x86 MASM (`src/x86.rs`), PIR→x86-64 (`src/codegen_x64.rs`)
- **ARM64**: `src/codegen_arm64.rs` (assembly output)
- **RISC-V**: `src/codegen_riscv.rs` (assembly output)
- **WASM**: `src/wasm.rs` (PIR → WebAssembly binary, `pledgec wasm`)
- **SPIR-V / GPU**: `src/gpu.rs` (PIR → SPIR-V binary, `pledgec gpu`)
- **C source**: `src/codegen_c.rs` (IR → C99, portable fallback compiled by gcc/clang/cl.exe)
- **Register allocator**: `src/regalloc.rs` — linear scan; has known bug (see Optimizer caveat above)
- `pledgec codegen <file> --target <arch>` for assembly; `--elf|--macho|--coff` for object files

### 7. Custom Linker / Binary Emitters — ✅ Multi-Format
- **PE**: `src/pe.rs` — Windows `.exe`, direct byte emission, no `link.exe`/`ml64.exe`
- **ELF64**: `src/elf64.rs` — Linux executables + relocatable object files, no external `ld`
- **Mach-O**: `src/macho.rs` — macOS object files
- **COFF**: `src/coff.rs` — Windows object files
- **Bare-metal flat binary**: `emit --bare` — no headers, no stdlib, raw machine code at fixed address
- **Integrated linker**: `src/linker.rs` — links ELF64 objects into executables, resolves relocations in-process, incremental linking, link-time DCE
- `pledgec link <files...>` to link multiple object files

### 8. Custom Runtime — ✅ No libc for P Binaries
- **Linux**: `src/runtime_linux.rs` — hand-encoded x86-64 machine code (raw bytes, not assembly text) implementing all runtime stubs via direct syscalls. System V AMD64 ABI. No libc.
- **Windows**: `src/pe.rs` runtime stubs link only `kernel32.dll` (HeapAlloc/HeapFree/WriteFile/GetStdHandle/ExitProcess) — not the MSVC C runtime.
- **Bare metal**: no runtime — `--bare` mode emits flat binary with no stdlib
- Panic handler (`p_panic`), print stubs (`p_print_str/int/bool/float/newline`), allocator (`p_malloc`/`p_free`/`p_alloc`), string ops (`p_str_concat`/`p_to_str`), Vec/HashMap/String ops

### 9. Custom Stdlib — ✅ 8 Modules
- `stdlib/core.p` (auto-included): `Option`, `Result`, `println`, `assert`, `assert_eq`, `max`, `min`, `abs`, `clamp`
- `stdlib/json.p`: parse, stringify, get, at
- `stdlib/tensor.p`: n-d arrays, matmul, relu, softmax
- `stdlib/parallel.p`: par_map, par_sum, par_dot
- `stdlib/db.p`: `cap Database` trait, in-memory KV store
- `stdlib/http.p`: socket + HTTP parsing
- `stdlib/simd.p`: elementwise ops, dot, sum, scale (f64x2, f64x4, etc.)
- `stdlib/shape.p`: shape system for tensor ops

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
- Examples: `examples/rpi_led_blink.p`, `examples/riscv_blink.p`

### 12. Hot Reload — ✅
- `src/hotreload.rs` + `pledgec watch`
- Native JIT via ELF64 emission
- Function-level change detection via L2 IR hashing
- Recompiles and swaps only changed functions

### 13. C Header FFI — ✅
- `src/cparser.rs` + `pledgec parse-header <file.h>`
- Parses C headers and generates P FFI bindings automatically
- `extern "C"` functions: parsed, lowered, C codegen, interpreter stubs

---

## Dependencies We Accept (For Now)

| Dependency | Purpose | When We Remove It |
|---|---|---|
| Rust compiler | Build `pledgec` itself | Phase 6 (self-hosting) |
| Rust stdlib | Build `pledgec` itself | Phase 6 (self-hosting) |
| `hashbrown`, `compact_str`, `smallvec`, `bumpalo` | Build `pledgec` itself | Phase 6 (self-hosting) |
| Windows `kernel32.dll` | P binary I/O + heap on Windows | Phase 5 (custom syscall runtime) |
| `gcc`/`clang`/`cl.exe` | Only for `ccompile` command (C backend) | Never — it's an optional fallback backend |

**Everything else is custom-built. No LLVM, no libc (for P binaries), no external linker, no external assembler.**

Note: the `ccompile` command intentionally invokes an external C compiler — that's the *portability fallback* backend (Zig-style), not the primary path. The primary paths (`emit`, `build`, `run`) use zero external tools.

---

## The Path to Zero Dependencies

```
Now (Phase 4D, complete):
  pledgec (Rust binary)
    → uses Rust stdlib + 4 crates (hashbrown, compact_str, smallvec, bumpalo)
    → produces ELF64 executables on Linux (no external ld/as)
    → produces PE executables on Windows (no external link.exe/ml64.exe)
    → produces Mach-O / COFF object files
    → produces WASM, SPIR-V, bare-metal flat binaries
    → P binaries use raw syscalls (Linux) or kernel32.dll (Windows) — no libc

Phase 5:
  pledgec (Rust binary)
    → custom file I/O via raw syscalls on Windows too (drop kernel32.dll for P binaries)
    → region allocator tied to region system
    → full effect handler continuation semantics

Phase 6:
  pledgec (P binary, self-hosting)
    → no Rust stdlib
    → no Rust compiler needed to build pledgec
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
- `pledgec emit <file> -o <exe>` produces a working `.exe` in one step

### ELF64 Emission (Linux) — ✅
- Direct byte emission in `src/elf64.rs`
- Primary native backend on Linux via `pledgec build`
- Full PIR pipeline: L1 → L2 → L3 → codegen → peephole → ELF64
- Relocations: `R_X86_64_PC32`, `R_X86_64_PLT32`, `R_X86_64_64`
- Links against hand-encoded runtime (`runtime_linux.rs`) — raw machine code, no libc
- Also emits relocatable object files (`.o`)
- Zero external tools: no `as`, no `ld`, no `gcc`

### Mach-O (macOS) — ✅
- `src/macho.rs` — object file emission
- `pledgec codegen <file> --macho <out.o>`

### COFF (Windows) — ✅
- `src/coff.rs` — object file emission
- `pledgec codegen <file> --coff <out.o>`

### Bare-Metal Flat Binary — ✅
- `pledgec emit <file> --bare -o <bin>`
- No file format headers — raw machine code at a fixed address
- No stdlib, no runtime
- For bootloaders, OS kernels, firmware, microcontrollers
- Examples: `examples/rpi_led_blink.p`, `examples/riscv_blink.p`, `tests/test_boot.p`, `tests/test_bare.p`

### WASM — ✅
- `src/wasm.rs` — PIR → WebAssembly binary
- `pledgec wasm <file> -o <out.wasm>`

### SPIR-V / GPU — ✅
- `src/gpu.rs` — PIR → SPIR-V binary
- `pledgec gpu <file>`

### C Source (portability fallback) — ✅
- `src/codegen_c.rs` — IR → C99 source
- `pledgec ccompile <file> -o <exe>` generates `.c`, invokes `gcc`/`clang`/`cl.exe`
- Used when targeting platforms the native backends don't yet cover
- Note: 76/78 test parity with interpreter — secondary backend, not primary (2 real mismatches: test_asm by design, test_comptime_reflect interpreter-only)

---

## Why This Matters for Performance

### Compilation Speed
```
Rust pipeline:
  Source → AST → MIR → LLVM IR → LLVM opts (100+ passes) → Machine code → Object files → Linker
  Total: 10-30 minutes for large projects

P pipeline (current):
  Source → AST → TypeCheck → Mono → IR → PIR L1 → L2 → L3 → Codegen → Peephole → ELF64
  Total: < 1 second for small projects, < 10 seconds for large projects
```

**Why P is faster:**
- No LLVM (saves ~80% of compilation time)
- No external linker (saves ~50% of remaining time)
- Fewer optimization passes (only what's needed — peephole is always on, opt.rs is opt-in)
- Semantic optimization (regions/effects give us info LLVM would have to derive)
- Integrated pipeline (no IPC between compiler and linker)

### Binary Size
```
C "hello world": ~16KB (with libc)
Rust "hello world": ~4MB (with stdlib)
P "hello world": ~4KB target (no libc, no runtime, no stdlib bloat)
```

**Why P is smaller:**
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
| `src/opt.rs` | PIR optimizer (opt-in — has regalloc bug) |
| `src/peephole.rs` | 9-pass peephole (always on in `build`) |
| `src/regalloc.rs` | Linear scan register allocator (⚠️ known bug) |
| `src/mono.rs` | Generic monomorphization |
| `src/comptime.rs` | Compile-time eval + reflection |
| `src/pe.rs` | PE emitter (Windows, direct IR→machine code) |
| `src/elf64.rs` | ELF64 emitter (Linux, PIR pipeline) |
| `src/macho.rs` | Mach-O object emitter (macOS) |
| `src/coff.rs` | COFF object emitter (Windows) |
| `src/linker.rs` | Integrated linker (ELF64 objects → executable) |
| `src/runtime_linux.rs` | Hand-encoded Linux x86-64 runtime (raw syscalls, no libc) |
| `src/codegen_x64.rs` | PIR L3 → x86-64 assembly |
| `src/codegen_arm64.rs` | PIR L3 → ARM64 assembly |
| `src/codegen_riscv.rs` | PIR L3 → RISC-V assembly |
| `src/codegen_c.rs` | IR → C99 source (portability fallback) |
| `src/wasm.rs` | PIR → WebAssembly |
| `src/gpu.rs` | PIR → SPIR-V |
| `src/x86.rs` | IR → MASM assembly (legacy `build` path) |
| `src/embedded.rs` | FixedBufferAllocator, RealTime effect, HAL framework |
| `src/hotreload.rs` | Native JIT + function-level change detection |
| `src/cparser.rs` | C header parser → P FFI bindings |
| `src/compiler.rs` | Pipeline orchestration, module resolution, stdlib auto-include |
| `src/main.rs` | CLI entry point, command dispatch |
