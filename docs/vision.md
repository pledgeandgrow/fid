# Pledge Language — Vision

> Build the last programming language the world needs.

## Mission

Pledge is a systems programming language built from first principles — every layer custom-made, from parser to machine code. No LLVM dependency. No borrowed infrastructure. A language designed to outperform C, Rust, and Zig in security, performance, developer velocity, and expressiveness.

## The Eight Pillars

### 1. Originality — Own Our Uniqueness
- No existing language combines region-based memory safety, effect handlers, capability security, and comptime metaprogramming
- Every layer of the compiler stack is built by us: lexer, parser, typechecker, IR, optimizer, codegen, linker
- Pledge is not "Rust with different syntax" or "Zig with safety" — it is a new architecture

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

### 4. Memory Safety — Like Rust, Without the Pain
- **Region inference**: compiler tracks memory regions, no lifetime annotations needed
- **No borrow checker fights**: regions are more flexible than ownership+borrowing
- **No "mutation xor sharing" rule**: aliasing patterns that Rust rejects are valid in Pledge
- **Explicit allocators**: caller chooses allocation strategy (like Zig), but with compile-time safety (like Rust)
- **Zero runtime cost**: all safety checks are compile-time

### 5. Power & Flexibility — Like C, But Safe
- **Direct hardware access**: raw pointers available in framework code (not user code)
- **Inline assembly**: for kernel/embedded development
- **Zero-cost abstractions**: generics, closures, traits — all compile to optimal code
- **comptime**: full metaprogramming without macros — regular code at compile time
- **No hidden control flow**: no exceptions, no hidden allocations, no implicit casts
- **FFI**: interop with C for legacy integration during transition

### 6. Adoption — Make Switching Easy
- **C interop**: call C libraries directly, gradual migration path
- **Familiar syntax**: Rust-like syntax with Pledge keywords (pv/pf/pc)
- **Hot reload**: instant feedback loop attracts game/GUI/web developers
- **Fast compilation**: sub-second builds for small projects
- **AI tooling**: LLM-friendly syntax, semantic IR enables AI-assisted refactoring
- **Single binary**: no runtime, no VM, no dependencies — just a binary
- **Cross-compilation**: built-in, not an afterthought

### 7. Custom — Everything Built By Us
- **Custom lexer**: hand-written, not generated
- **Custom parser**: recursive descent, not parser generators
- **Custom typechecker**: region inference + effect system + capability checking
- **Custom IR (PIR)**: Pledge Intermediate Representation — typed, semantic, multi-level
- **Custom optimizer**: semantic-aware, leverages region/effect/capability info
- **Custom codegen**: direct to machine code (x86-64, ARM64, RISC-V)
- **Custom linker**: integrated, not a separate process
- **Custom stdlib**: no libc dependency for core operations
- **Custom allocator**: region-aware, built into the language runtime
- Minimal external dependency: only the Rust compiler to build Pledge (bootstrapping later)

### 8. Innovation — Optimized for AI, Robotics, Hardware, IoT
- **AI/ML**: comptime specialization for tensor operations, SIMD intrinsics as first-class
- **Robotics**: real-time guarantees via effect system (mark functions as !RealTime)
- **Hardware**: inline assembly, register access, interrupt handlers
- **IoT**: bare-metal targets, deterministic memory (FixedBufferAllocator), tiny binaries
- **Effect handlers**: model robot sensors, actuators, and control loops as effects
- **Region-based memory**: predictable allocation for real-time systems
- **Cross-compilation**: target any architecture from any host
- **No runtime**: bare metal, no OS needed

## What Pledge Is NOT

- **Not a research language**: designed for production, not papers
- **Not a Rust replacement**: different architecture, different tradeoffs
- **Not a garbage-collected language**: zero runtime overhead
- **Not a managed language**: no VM, no JIT in production mode
- **Not a macro-based language**: comptime replaces all metaprogramming
- **Not an unsafe language**: no `unsafe` in user code, ever

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
