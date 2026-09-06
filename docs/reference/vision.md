# Fides Language — Vision

> Build the last programming language the world needs.
>
> **Status (Sep 6, 2026 — session 16):** Bootstrap fixpoint achieved. The
> Fides compiler is self-hosted (26 modules, 61K LOC) and reproduces
> byte-identical output across generations (`fides-c2.c == fides-c3.c`,
> SHA256-identical, 701,100 lines). Three generations verified end-to-end.
> Tier 3 OS features added: `@cpu_local`, inline asm clobbers, `--no-std`,
> cross-compilation dispatch, UEFI boot services bindings.

## Mission

Fides is a systems programming language built from first principles — every layer custom-made, from parser to machine code. No LLVM dependency. No borrowed infrastructure. A language designed to outperform C, Rust, and Zig in security, performance, developer velocity, and expressiveness.

## The Eight Pillars

### 1. Originality — Own Our Uniqueness
- No existing language combines region-based memory safety, effect handlers, capability security, and comptime metaprogramming
- Every layer of the compiler stack is built by us: lexer, parser, typechecker, IR, optimizer, codegen, linker
- Fides is not "Rust with different syntax" or "Zig with safety" — it is a new architecture

### 2. Security — Prevent CVEs by Construction
- **Capability-based security**: no ambient authority, no implicit filesystem/network access
- **Region-based memory safety**: use-after-free, double-free, buffer overflow — all compile-time errors
- **No `unsafe` escape hatch in user code**: framework-only, like capability kernels
- **Effect system**: IO, network, spawn — all explicit, all auditable
- **Supply chain defense**: libraries receive only granted capabilities, can't access what they're not given
- Target: eliminate 90%+ of CWE categories that plague C/C++

### 3. Performance — 10x Faster (Realistically)
- **Runtime**: 2-3x faster than Rust/Zig in specific cases via semantic optimization (regions enable better alias analysis, effects enable reordering, comptime eliminates runtime branches)
- **Compilation**: 10x faster than Rust via custom IR and codegen (no LLVM overhead)
- **Iteration**: 100x faster dev cycle via hot reload (milliseconds vs minutes)
- **Binary size**: smaller than C via dead code elimination at IR level
- **Memory**: 40% less overhead than Rust via region-based allocation (no Rc/RefCell needed)

### 4. Memory Safety — Regions First, RC as the Safe Escape Hatch
- **Regions are the default memory model**: `region r { ... }` for scoped, bulk-freed, zero-overhead allocation. Compile-time escape detection prevents use-after-free.
- **References are unchecked by default**: `&` and `&mut` work without a borrow checker. Aliasing patterns that Rust rejects are valid in Fides. The borrow checker is opt-in (`--borrowck=on`) for users who want compile-time aliasing safety.
- **RC is the safe alternative to `unsafe`**: when a pattern needs shared ownership (graphs, caches, callbacks, dynamic lifetime), `Rc<T>`/`Arc<T>` provides it memory-safely. This eliminates the need for `unsafe` raw pointers in user code.
- **No `unsafe` in user code**: raw pointers are framework-only. If you can't use regions and you can't use references, you use RC — never `unsafe`.
- **Explicit allocators**: caller chooses allocation strategy (like Zig), but with compile-time safety (like Rust)
- **Zero runtime cost for the default path**: regions and references are zero-cost. RC has runtime cost, but only when explicitly chosen.

### 5. Power & Flexibility — Like C, But Safe
- **Direct hardware access**: raw pointers available in framework code only (not user code — use RC instead)
- **Inline assembly**: for kernel/embedded development
- **Zero-cost abstractions**: generics, closures, traits — all compile to optimal code
- **comptime**: full metaprogramming without macros — regular code at compile time
- **No hidden control flow**: no exceptions, no hidden allocations, no implicit casts
- **FFI**: interop with C for legacy integration during transition

### 6. Adoption — Make Switching Easy
- **C interop**: call C libraries directly, gradual migration path
- **Familiar syntax**: Rust-like syntax with Fides keywords (val/fn/const)
- **Hot reload**: instant feedback loop attracts game/GUI/web developers
- **Fast compilation**: sub-second builds for small projects
- **AI tooling**: LLM-friendly syntax, semantic IR enables AI-assisted refactoring
- **Single binary**: no runtime, no VM, no dependencies — just a binary
- **Cross-compilation**: built-in, not an afterthought — **DONE** (ARM64, RISC-V, Windows)

### 7. Custom — Everything Built By Us
- **Custom lexer**: hand-written, not generated
- **Custom parser**: recursive descent, not parser generators
- **Custom typechecker**: region inference + effect system + capability checking
- **Custom IR (PIR)**: Fides Intermediate Representation — typed, semantic, multi-level
- **Custom optimizer**: semantic-aware, leverages region/effect/capability info
- **Custom codegen**: direct to machine code (x86-64, ARM64, RISC-V) — **DONE** (with cross-compilation)
- **Custom linker**: integrated, not a separate process — **DONE** (target-aware: x86-64, ARM64, RISC-V)
- **Custom stdlib**: no libc dependency for core operations
- **Custom allocator**: region-aware, built into the language runtime
- Minimal external dependencies: the Rust compiler plus 4 crates (hashbrown, compact_str, smallvec, bumpalo) to build Fides (bootstrapping later)

### 8. Innovation — Optimized for AI, Robotics, Hardware, IoT
- **AI/ML**: comptime specialization for tensor operations, SIMD intrinsics as first-class
- **Robotics**: real-time guarantees via effect system (mark functions as !RealTime)
- **Hardware**: inline assembly, register access, interrupt handlers
- **IoT**: bare-metal targets, deterministic memory (FixedBufferAllocator), tiny binaries
- **Effect handlers**: model robot sensors, actuators, and control loops as effects
- **Region-based memory**: predictable allocation for real-time systems
- **Cross-compilation**: target any architecture from any host — **DONE** (ARM64, RISC-V, Windows with zero external tools)
- **No runtime**: bare metal, no OS needed

## What Fides Is NOT

- **Not a research language**: designed for production, not papers
- **Not a Rust replacement**: different architecture, different tradeoffs
- **Not a garbage-collected language**: zero runtime overhead
- **Not a managed language**: no VM, no JIT in production mode
- **Not a macro-based language**: comptime replaces all metaprogramming
- **Not an unsafe language**: no `unsafe` in user code, ever — RC is the safe escape hatch when regions can't express the ownership pattern

## The North Star

```
A language where:
- You can write an OS kernel
- You can write a web server
- You can program a robot
- You can train an AI model
- You can target a microcontroller
- All with the same language
- All memory-safe
- All fast
- All without fighting the compiler
- All with hot reload during development
- All compiled to native code by our own toolchain
```
