# Pledge Architecture — The Full Stack

> Every layer built by us. No LLVM. No borrowed infrastructure.

## Current Architecture (Phase 1 — What We Have Now)

```
Source (.p files)
    ↓
Lexer (src/lexer.rs)          — hand-written tokenizer
    ↓
Parser (src/parser.rs)        — recursive descent, produces AST
    ↓
Typechecker (src/typechecker.rs) — basic type inference
    ↓
Lowering (src/lower.rs)        — AST → IR (our own SSA-based IR)
    ↓
Monomorphization (src/mono.rs) — generic specialization
    ↓
┌─────────────────────────────────────────────┐
│                                             │
│  Interpreter (src/interp.rs)                │ — runs IR directly
│  PE Emitter (src/pe.rs)                     │ — IR → x86-64 machine code
│                                             │
└─────────────────────────────────────────────┘
```

**What works today:**
- Lexer, parser, AST, typechecker, lowering, monomorphization
- IR interpreter (for `pledgec run`)
- PE emitter (for `pledgec emit` — produces native x86-64 executables)
- Generics, enums, structs, closures, match, try blocks, defer, references
- No external dependencies (no LLVM, no libc, no assembler, no linker)

**What's missing:**
- No optimizer (IR is directly translated to machine code)
- No region inference
- No effect system
- No capability checking
- No comptime
- No hot reload
- Only x86-64 target

---

## Target Architecture (End State)

```
Source (.p files)
    ↓
Lexer (src/lexer.rs)               — hand-written, incremental
    ↓
Parser (src/parser.rs)             — recursive descent, error recovery
    ↓
Typechecker (src/typechecker.rs)   — regions + effects + capabilities + traits
    ↓
PIR Generator (src/pir.rs)         — AST → PIR (Pledge Intermediate Representation)
    ↓
┌──────────────────────────────────────────────────────────┐
│                                                          │
│  PIR Level 1 — High-level (semantic)                     │
│  • Preserves regions, effects, capabilities              │
│  • Inlining, specialization, comptime evaluation         │
│  • Effect-based reordering                               │
│  • Region-based alias analysis                           │
│  • Capability elision                                    │
│                                                          │
│  PIR Level 2 — Mid-level (control flow)                  │
│  • Loop transforms, tail call optimization               │
│  • Closure conversion, lambda lifting                    │
│  • Partial evaluation, constant folding                  │
│  • Dead code elimination                                 │
│                                                          │
│  PIR Level 3 — Low-level (machine-near)                  │
│  • SSA form, register allocation                         │
│  • Instruction selection per architecture                │
│  • Peephole optimization                                 │
│  • Stack frame layout                                    │
│                                                          │
└──────────────────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│                                             │
│  Dev Mode: JIT Compiler                     │ — hot reload, fast compilation
│  • Compile function bodies on demand        │
│  • Patch running code in memory             │
│  • Sub-millisecond iteration               │
│                                             │
│  Prod Mode: AOT Compiler                    │ — full optimization, small binaries
│  • All PIR optimization passes              │
│  • Custom codegen (x86-64, ARM64, RISC-V)  │
│  • Integrated linker (no external linker)   │
│  • Binary output (ELF, PE, Mach-O)          │
│                                             │
└─────────────────────────────────────────────┘
    ↓
Native Binary / Running Code
```

---

## PIR — Pledge Intermediate Representation

### Design Principles

1. **Typed**: every value has a type, types carry semantic information
2. **Multi-level**: high-level → mid-level → low-level, each preserves different info
3. **Semantic-aware**: regions, effects, capabilities are first-class in PIR
4. **SSA-based**: static single assignment for optimization passes
5. **Target-independent at high level**: same PIR for all architectures
6. **Human-readable**: debuggable, inspectable (like LLVM IR but cleaner)

### PIR Level 1 — High-Level (Semantic)

Preserves all Pledge language semantics:

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

**Optimizations at this level:**
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
- Register allocation (graph coloring or linear scan)
- Instruction selection (choose optimal instructions per architecture)
- Peephole optimization (local instruction improvements)
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

### What We Build Instead
- **x86-64 backend**: direct PIR → machine code (already have this in pe.rs!)
- **ARM64 backend**: for Apple Silicon, embedded, servers
- **RISC-V backend**: for IoT, custom hardware, research
- **WASM backend**: for web deployment (future)

### Codegen Strategy
```
PIR Level 3 → Register Allocator → Instruction Selector → Peephole → Binary Output
```

- **Register allocator**: linear scan for dev (fast), graph coloring for prod (optimal)
- **Instruction selector**: pattern matching on PIR → machine instructions
- **Peephole**: local optimizations (e.g., `mov rax, 0` → `xor rax, rax`)
- **Binary output**: direct to ELF/PE/Mach-O (no external assembler/linker)

---

## Custom Linker — Integrated, Not Separate

### Why Not lld/ld?
- Linking is 50%+ of build time for large projects
- External linker doesn't understand PIR semantics
- Separate linker = separate process = IPC overhead

### What We Build Instead
- **Integrated linker**: part of the compiler, not a separate process
- **Incremental linking**: only re-link changed modules
- **Semantic linking**: leverage PIR for cross-module optimization
- **Dead code elimination**: at link time, remove unused functions (smaller binaries)

---

## Custom Runtime — Minimal, Region-Aware

### What We Need
- **Region allocator**: arena/pool/stack allocation backed by regions
- **Panic handler**: for unrecoverable errors (no exceptions)
- **Effect runtime**: for effect handlers (when not compiled away)
- **Print/IO stubs**: minimal stdlib (already have p_print_*)

### What We DON'T Need
- No garbage collector
- No runtime type information (RTTI)
- No virtual machine
- No JIT in production mode
- No reflection (comptime replaces it)

---

## Bootstrapping Strategy

### Phase 1: Pledge in Rust (Now)
- Compiler written in Rust
- Compiles Pledge source to native code
- Self-hosting IR and codegen

### Phase 2: Pledge in Pledge (Self-Hosting)
- Rewrite compiler in Pledge
- Pledge compiler compiles itself
- No Rust dependency for end users
- Compiler binary is a Pledge binary

### Phase 3: Pledge Optimizes Itself
- PIR optimizer runs on Pledge compiler code
- comptime specializes the compiler for the target
- Compiler becomes faster by optimizing itself
