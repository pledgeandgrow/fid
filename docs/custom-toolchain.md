# Pledge Custom Toolchain — Why and How

> If we depend on it, we build it. No LLVM. No libc. No external linker.

## Why Custom Everything?

### The LLVM Trap
Every modern language (Rust, Zig, Swift, Crystal, Julia) uses LLVM. This means:
- They all share the same optimization limitations
- They all share the same compile-time characteristics
- They all share the same binary format constraints
- None can leverage language-specific semantics in the optimizer
- None can fix LLVM bugs without going through LLVM's contribution process

**Using LLVM would make Pledge "just another LLVM frontend."** We would gain nothing architecturally over Rust or Zig. Our semantic information (regions, effects, capabilities) would be lost when lowering to LLVM IR.

### The libc Trap
Most languages depend on libc for basic operations (malloc, printf, file I/O). This means:
- They inherit libc's security vulnerabilities
- They inherit libc's binary bloat
- They can't run on bare metal without a libc port
- They're tied to POSIX/C semantics

**Pledge's custom stdlib eliminates the libc dependency.** We implement I/O, memory, and strings ourselves — smaller binaries, no inherited vulnerabilities, bare metal ready.

### The Linker Trap
External linkers (ld, lld, link.exe) are separate processes that:
- Don't understand language semantics
- Add 50%+ to build time for large projects
- Require object file formats that lose information
- Can't do cross-module optimization

**Pledge's integrated linker** is part of the compiler — no separate process, no information loss, incremental linking, semantic-aware dead code elimination.

---

## What We Build

### 1. Custom Lexer (Done)
- Hand-written tokenizer in `src/lexer.rs`
- No lexer generator (no flex, no nom)
- Incremental lexing for hot reload (future)
- **Status: Complete**

### 2. Custom Parser (Done)
- Recursive descent parser in `src/parser.rs`
- No parser generator (no yacc, no ANTLR)
- Error recovery for IDE integration (future)
- **Status: Complete**

### 3. Custom Typechecker (In Progress)
- Region inference + effect system + capability checking
- No external SMT solver (everything built in)
- **Status: Basic typechecking works, regions/effects/capabilities not yet implemented**

### 4. Custom IR — PIR (Planned)
- Multi-level: semantic → mid-level → low-level
- Typed, SSA-based, human-readable
- Preserves region/effect/capability information through optimization
- **Status: Current IR (ir.rs) is a single-level SSA IR. PIR is the evolution.**

### 5. Custom Optimizer (Planned)
- Pass-based optimization at each PIR level
- Region-aware alias analysis (free, no pointer analysis)
- Effect-based reordering (pure functions can reorder)
- Capability elision (remove checks when provably safe)
- comptime partial evaluation
- **Status: Not started. Current pipeline has no optimizer.**

### 6. Custom Codegen (Partially Done)
- x86-64: PE emitter works (`src/pe.rs`)
- ARM64: planned
- RISC-V: planned
- WASM: planned (future)
- Register allocator: current PE emitter uses simple allocation, needs upgrade
- **Status: x86-64 PE emission works. Multi-architecture not started.**

### 7. Custom Linker (Partially Done)
- PE output: works (Windows)
- ELF output: planned (Linux)
- Mach-O output: planned (macOS)
- Incremental linking: planned
- **Status: Windows PE works. Other formats not started.**

### 8. Custom Runtime (Minimal)
- Region allocator: planned
- Panic handler: basic
- Effect runtime: planned (for when handlers can't be compiled away)
- Print/IO stubs: working (`p_print_*` functions)
- **Status: Minimal runtime works. Region allocator not started.**

### 9. Custom Stdlib (In Progress)
- Core types: Result, Option (working)
- Collections: Vec, HashMap (basic, working)
- Strings: basic
- Math: basic
- IO: basic (print works)
- **Status: Basic stdlib in `stdlib/core.p`. Needs expansion.**

### 10. Custom Allocator (Planned)
- Region allocator (arena-based, tied to region system)
- General purpose allocator (for when regions aren't enough)
- Fixed buffer allocator (for embedded/IoT)
- **Status: Not started. Currently uses simple malloc/free.**

---

## Dependencies We Accept (For Now)

| Dependency | Purpose | When We Remove It |
|---|---|---|
| Rust compiler | Build pledgec | Phase 6 (self-hosting) |
| Windows API | PE emission, file I/O | Phase 5 (custom runtime) |
| Standard library (Rust) | Build pledgec | Phase 6 (self-hosting) |

**Everything else is custom-built. No LLVM, no libc (for Pledge binaries), no external linker, no external assembler.**

---

## The Path to Zero Dependencies

```
Now:
  pledgec (Rust binary)
    → uses Rust stdlib
    → uses Windows API for file I/O
    → produces PE binaries (no external linker)

Phase 5:
  pledgec (Rust binary)
    → uses Rust stdlib
    → custom file I/O (no Windows API for Pledge binaries)
    → produces PE/ELF/Mach-O (no external linker)

Phase 6:
  pledgec (Pledge binary)
    → no Rust stdlib
    → no Windows API
    → produces PE/ELF/Mach-O
    → self-hosting complete
    → ZERO external dependencies
```

---

## Binary Output — Going Deeper

### Current: PE Emission (Windows)
- Direct byte emission of PE format
- No assembler, no linker
- Custom section layout (.text, .data, .rdata)
- Custom import table (for Windows API calls)
- **14848 bytes for a typical test binary**

### Future: ELF (Linux)
- Direct byte emission of ELF format
- Custom section layout
- Custom symbol table
- Custom relocation entries
- Target: < 4KB for simple programs (no libc bloat)

### Future: Mach-O (macOS)
- Direct byte emission of Mach-O format
- Custom segment layout
- Custom symbol table
- Code signing support (future)

### Future: Raw Binary (Bare Metal)
- No format — just raw machine code at a fixed address
- For bootloaders, OS kernels, firmware
- Entry point at address 0 (or configurable)
- Target: < 1KB for a simple LED blinker

---

## Why This Matters for Performance

### Compilation Speed
```
Rust pipeline:
  Source → AST → MIR → LLVM IR → LLVM opts (100+ passes) → Machine code → Object files → Linker
  Total: 10-30 minutes for large projects

Pledge pipeline (target):
  Source → AST → PIR L1 → PIR L2 → PIR L3 → Machine code → Binary
  Total: < 1 second for small projects, < 10 seconds for large projects
```

**Why Pledge is faster:**
- No LLVM (saves 80% of compilation time)
- No external linker (saves 50% of remaining time)
- Fewer optimization passes (only what's needed)
- Semantic optimization (regions/effects give us info LLVM would have to derive)
- Integrated pipeline (no IPC between compiler and linker)

### Binary Size
```
C "hello world": ~16KB (with libc)
Rust "hello world": ~4MB (with stdlib)
Pledge "hello world": ~4KB (no libc, no runtime, no stdlib bloat)
```

**Why Pledge is smaller:**
- No libc (we implement only what's needed)
- No runtime (no GC, no RTTI, no reflection)
- Dead code elimination at IR level (not link time)
- comptime inlines constants (no runtime initialization)
- Custom binary format (only sections we need)
