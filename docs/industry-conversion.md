# P Language — Industry Conversion Guide

> How projects from different sectors can migrate to P, what they gain,
> what they lose, and whether it's worth it.
>
> The core thesis: **P's safety systems (capabilities, effects, regions) are
> domain-agnostic. Learning P once unlocks every domain annotation. The
> switching cost is one language. The benefit is every industry.**

---

## The Shared Foundation

Before looking at individual industries, understand why P can serve all of
them. Every domain annotation is a *lens* on the same three core systems:

```
┌─────────────────────────────────────────────────────────────┐
│                    P CORE SAFETY SYSTEMS                      │
│                                                              │
│  Capabilities    Effects           Regions                   │
│  (authority)     (side effects)    (memory lifetime)         │
│      │               │                  │                    │
│      ▼               ▼                  ▼                    │
│  sandbox { }     !IO !Alloc        region r { }              │
│  @serverless     @deterministic    @firmware                 │
│  @gas            @realtime         @pool                     │
│  @audit          @crashfree        memory budgets             │
│                  @no_grad          @lowpower                 │
│                                                              │
│  Comptime (compile-time codegen + reflection)                │
│      │                                                       │
│      ▼                                                       │
│  @shader  @simd  @differentiable  @interrupt                 │
└─────────────────────────────────────────────────────────────┘
```

**This is why the scope isn't explosion — it's reuse.** Building
`@deterministic` for medical makes `@gas` for blockchain cheaper because
both use the effect system. Building `@pool` for gaming makes `@firmware`
for IoT cheaper because both use the region system.

---

## 1. Gaming — Switching from C++ / C# / Unity / Unreal

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Crashes in game loop | Try/catch, hope for the best | One panic = lost player |
| Shader strings | GLSL/HLSL as string literals | No type checking, no IDE support, runtime crashes |
| Memory allocation in hot loop | Custom allocators, manual pools | Boilerplate, error-prone, GC stalls (C#) |
| ECS frameworks | Unity DOTS, Entitas, custom | Each is different, none are native, all are libraries |
| State machine bugs | Enum + switch, no compile-time check | Invalid transitions crash at runtime |
| SIMD optimization | Intrinsics, manual vectorization | Platform-specific, unreadable, unmaintainable |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@crashfree` | Compiler proves a function cannot panic — zero crashes in game loop | C++, C#, Rust (no proof) |
| `@shader` | Write GPU code in P, compiler emits SPIR-V — same language, same types | C++ (needs HLSL strings), C# (needs shader strings) |
| `@simd` | Annotate a loop, compiler auto-vectorizes to SSE/AVX/NEON | C++ (manual intrinsics), C# (limited) |
| `@pool` | Zero-allocation object pools, compiler manages pool memory | C++ (manual), C# (GC pressure) |
| `vec2/3/4` + `mat4` + swizzle | First-class math types with operator overloading + SIMD codegen | C# (no SIMD native), Python (no native) |
| ECS (`component`/`system`/`query`) | Native ECS with SoA memory layout, compiler-controlled | C++ (library only), C# (library only) |
| `state` machines | Compile-time verified state transitions — invalid transition = compile error | C++, C#, Rust (all use enum+switch) |
| `on` event handlers | Unified input/collision/tick/lifecycle events | C++ (each framework different), C# (Unity events are magic) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Unity/Unreal editor | 🔴 Critical for AAA | P targets indie/2D/RTS first. Editor can be built later in P |
| Physics engine (PhysX, Havok) | 🔴 Critical | `stdlib/physics.p` planned. Or FFI to existing engines |
| Asset pipeline | 🟡 Significant | Build tooling in Phase 11. P's comptime can process assets |
| Audio engine (Wwise, FMOD) | 🟡 Significant | `stdlib/audio.p` via FFI |
| Community size | 🔴 Critical | This is the real barrier. Solved by shipping killer games |
| Existing codebase | 🔴 Varies | FFI allows calling C++ libraries. Migration is incremental |

### Migration Path

```
Step 1: Write game logic in P, call existing C++ engine via FFI
Step 2: Replace subsystems one at a time (physics, audio, rendering)
Step 3: Use @shader to move GPU code into P
Step 4: Use @crashfree to prove the game loop is panic-free
Step 5: Full P codebase, zero C++ dependency
```

### Verdict

🟢 **Worth it for indie/2D/RTS/web games.** The `@crashfree` guarantee alone
is a game-changer (pun intended). No other language can prove your game loop
won't crash. For AAA studios locked into Unreal/Unity, the migration cost is
too high — but they're not the target.

**Killer demo:** A 2D game in P with `@crashfree` game loop, `@shader` for
GPU effects, and ECS for entities. Under 5000 lines. Zero crashes. Prove it
runs at 60fps. That gets attention.

---

## 2. Blockchain — Switching from Solidity / Rust / Move / Vyper

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Gas limit exceeded at runtime | Runtime check, transaction reverts | User pays gas, tx fails, bad UX |
| Non-deterministic execution | Manual auditing | One non-deterministic op = chain fork |
| Reentrancy attacks | Checks-effects-interactions pattern, reentrancy guards | Easy to forget, $1.7B stolen to date |
| Unsafe math (overflow) | Solidity 0.8+ checked arithmetic, but still runtime | Runtime checks cost gas |
| No capability isolation | Anyone can call any public function | Hacks exploit unexpected call paths |
| Auditor bottleneck | Manual review by firms (Trail of Bits, OpenZeppelin) | $50K+ per audit, weeks of delay |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@gas` | Compiler counts IR ops, verifies gas < limit at compile time | Solidity (runtime only), Rust (no gas concept) |
| `@deterministic` | Compiler rejects FP, random, IO, time, mutable globals — provably deterministic | Solidity (runtime check), Rust (no enforcement) |
| `sandbox { }` | Capability revocation — untrusted contract code can't call IO/FS/Net | Solidity (no isolation), Rust (no compile-time caps) |
| `@emit` | Event emission with signature verification, compiler tracks all emitted events | Solidity (manual emit), Rust (no native events) |
| Effect system | Know at compile time what a function does (reads? writes? allocates?) | Solidity (no effects), Rust (no effect tracking) |
| Comptime | Compile-time code generation for serializers, ABI encoders | Solidity (limited), Rust (macros are complex) |
| Native performance | P compiles to native code — fast execution for L2/off-chain | Solidity (EVM interpreted), Rust (yes but complex) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| EVM compatibility | 🔴 Critical for L1s | P targets new L2s/sidechains/app-chains, not EVM |
| Solidity ecosystem (OpenZeppelin, etc.) | 🔴 Critical | Rebuild core libraries in P stdlib. Comptime helps |
| Existing DeFi composability | 🔴 Critical | New chain = new composability. This is a fresh start |
| Auditing firm familiarity | 🟡 Significant | Auditors will learn P if chains adopt it. `@deterministic` makes auditing easier |
| Wallet/tooling support | 🟡 Significant | Build tooling in Phase 11. P's comptime can generate ABIs |

### Migration Path

```
Step 1: Build a P VM (interpreter or JIT) as the chain's execution layer
Step 2: Write core contracts (token, vault, governance) in P with @gas + @deterministic
Step 3: Use sandbox { } to isolate untrusted contract calls
Step 4: Use @emit for event logging with signature verification
Step 5: Deploy chain with P as native contract language
```

### Verdict

🟢 **Worth it for new L1/L2 chains and app-chains.** Compile-time gas
verification is a genuine improvement over Solidity's runtime check.
`@deterministic` eliminates an entire class of consensus bugs. The barrier
is ecosystem (no OpenZeppelin), but new chains start fresh anyway.

**Killer demo:** A simple DEX contract in P with `@gas` (compile-time gas
proof), `@deterministic` (provably no IO/random), and `sandbox { }` (isolate
user-provided callbacks). Show the compiler rejecting a contract that
exceeds gas. That's something Solidity cannot do.

---

## 3. Robotics & IoT — Switching from C / C++ / Rust / MicroPython

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Missed deadlines (WCET) | Manual timing analysis, oscilloscope | One missed deadline = crashed robot |
| Memory leaks on long runs | Manual free, valgrind, hope | Robot runs for months, leak = crash |
| Heap allocation in control loop | Ban malloc (convention), code review | One malloc = unpredictable latency |
| Interrupt handler bugs | Manual ISR code, register save/restore | One wrong register = corrupted state |
| Watchdog timer management | Manual kicks, hope | Forgot to kick = robot resets mid-operation |
| Battery life | Manual sleep, clock gating | No compiler help for power optimization |
| Safety certification | Manual code review, DO-178C / IEC 61508 | Expensive, slow, error-prone |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@realtime` | Compiler analyzes WCET, rejects malloc, rejects unbounded loops | C (manual), Rust (no WCET), Zig (no WCET) |
| `@interrupt` | Compiler generates ISR code, vector table, register save/restore, RETI | C (manual asm), Rust (manual) |
| `@firmware` | Compiler strips OS deps, bare metal startup, rejects heap | C (manual linker scripts), Rust (no_std but manual) |
| `@watchdog` | Compiler auto-inserts watchdog kicks at safe points (loop boundaries) | C (manual), Rust (manual) |
| `@lowpower` | Compiler inserts WFI/WFE sleep, gates clocks, estimates battery life | C (manual), Rust (no support) |
| Region system | Memory safety without GC, without borrow checker complexity | Rust (borrow checker is hard), C (unsafe) |
| `@deterministic` | Provably no FP/random/IO in control loop | C (manual review), Rust (no enforcement) |
| `@pool` | Zero-allocation object pools for sensor data | C (manual), Rust (arena libraries) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| ROS/ROS2 ecosystem | 🔴 Critical for research robots | FFI to ROS. Or build P-native robotics stdlib |
| Vendor SDKs (STM32, NXP, ESP32) | 🔴 Critical | FFI to vendor C headers. `parse-header` already works |
| MicroPython ease of use | 🟡 For prototyping | P is more verbose but safer. Use MicroPython for prototypes, P for production |
| Existing C codebase | 🟡 Varies | FFI allows calling C. Migration is incremental |
| Arduino ecosystem | 🟡 For hobbyists | P targets professional robotics, not Arduino |

### Migration Path

```
Step 1: Write control loop in P with @realtime (compiler rejects malloc)
Step 2: Use @interrupt for ISR code (compiler generates vector table)
Step 3: Use @firmware for bare metal startup (no OS dependency)
Step 4: Use @watchdog for automatic watchdog kicks
Step 5: Call vendor SDK via FFI for hardware-specific operations
Step 6: Full P firmware, zero C dependency
```

### Verdict

🟢 **Worth it for professional robotics and IoT firmware.** `@realtime`
rejecting malloc at compile time is a genuine safety improvement. The region
system is simpler than Rust's borrow checker for embedded developers who
are used to C. The barrier is vendor SDKs, but FFI solves that.

**Killer demo:** A motor control loop in P with `@realtime` (prove no
malloc, bounded loop), `@interrupt` (auto-generated ISR for encoder input),
and `@watchdog` (auto-kick). Show the compiler rejecting a version that
accidentally calls malloc. That's something C and Rust cannot do.

---

## 4. Medical Devices — Switching from C++ / Ada / SPARK / Java

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Non-deterministic computation | Manual code review, ban FP | One FP rounding error = wrong dosage |
| Audit trails | Manual logging, hope devs remember | FDA requires audit trail, missing log = non-compliance |
| Untrusted code access | Process isolation, OS-level sandboxing | Heavy, slow, not always available on medical devices |
| Safety certification (IEC 62304, FDA 510(k)) | Expensive external audits | $1M+ per device, 12-18 months |
| Side effects in pure functions | Manual review, code conventions | One hidden side effect = wrong result |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@deterministic` | Compiler rejects FP, random, IO, time, mutable globals — provably deterministic | C++ (manual), Java (no enforcement), Ada (limited) |
| `@audit` | Compiler auto-instruments every function entry/exit — FDA-compliant audit trail for free | C++ (manual logging), Java (AOP but runtime) |
| `sandbox { }` | Compile-time capability revocation — untrusted code can't access patient data | C++ (OS-level only), Java (SecurityManager deprecated) |
| Effect system | Know at compile time that dosage calculation is pure (!{}) | C++ (no effects), Java (no effects), Ada (limited) |
| Region system | Memory safety without GC — critical for medical device reliability | Java (GC pauses), C++ (unsafe), Rust (borrow checker complexity) |
| `@crashfree` | Prove the device controller cannot panic | C++ (no proof), Java (exceptions everywhere) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Ada/SPARK formal verification | 🔴 For safety-critical | P's `@deterministic` + `@crashfree` cover most needs. Full formal verification in Phase 12 |
| Existing certified codebase | 🔴 Critical | Recertification required. But P's compile-time guarantees may make certification *easier* |
| Java ecosystem (for hospital IT) | 🟡 For non-device code | P targets device firmware, not hospital backend |
| Auditing firm familiarity | 🟡 Significant | Auditors will learn P if medical companies adopt it. `@audit` makes their job easier |

### Migration Path

```
Step 1: Write dosage calculation in P with @deterministic (no FP, no IO)
Step 2: Use @audit for automatic FDA-compliant audit trail
Step 3: Use sandbox { } to isolate patient data access
Step 4: Use @crashfree to prove the device controller cannot panic
Step 5: Use effect system to prove pure functions are actually pure
Step 6: Submit for FDA certification — P's compile-time proofs strengthen the case
```

### Verdict

🟢 **Worth it for new medical devices.** `@deterministic` eliminating FP/IO/
random at compile time is exactly what medical device developers need. The
`@audit` auto-instrumentation saves thousands of lines of manual logging.
The barrier is regulatory certification, but P's compile-time guarantees
should make certification *easier*, not harder.

**Killer demo:** A drug dosage calculator in P with `@deterministic` (no FP,
integer-only math), `@audit` (auto-logged every call), and `@crashfree`
(proven no panic). Show the compiler rejecting a version that accidentally
uses floating point. That's something C++ and Java cannot do.

---

## 5. Serverless & Edge — Switching from Go / TypeScript / Python / Rust

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Cold start latency | Provisioned concurrency, keep-alive | 100ms+ cold starts = bad UX |
| Binary size | Docker images, layers | 100MB+ images for a simple function |
| Unnecessary code in binary | Tree-shaking (JS), manual trimming | Ship entire runtime for one function |
| Security isolation | Containers, V8 isolates | Heavy, slow, attack surface |
| Paying for unused features | Runtime includes everything | Pay for GC, reflection, unused stdlib |
| WASM complexity | Manual WASM compilation, ABI issues | Hard to target, limited language support |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@serverless` | Compiler tree-shakes, pre-initializes, outputs WASM | Go (manual), TS (bundler-dependent), Python (impossible) |
| Capability-aware linking | `--caps IO` strips all non-IO functions from binary | Go (no caps), TS (no caps), Rust (no caps) |
| Tiny binaries | No runtime, no GC, no VM — KB not MB | Go (MB), Java (100MB+), Python (container) |
| `sandbox { }` | Compile-time capability isolation per function | Go (OS-level), TS (none), Python (none) |
| Native + WASM | Same code compiles to native (edge) or WASM (browser) | Go (WASM but large), Python (no native) |
| Effect system | Know exactly what a function does — bill for actual work | Go (no effects), TS (no effects) |
| Hot reload | `pledgec watch` — instant function-level reload | Go (rebuild), TS (HMR but complex) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| AWS Lambda/GCP Cloud Run native support | 🔴 Critical | P compiles to WASM — works with any WASM-compatible platform |
| npm/pip/crates ecosystem | 🔴 Critical | P stdlib + package manager (Phase 11). Start with core libraries |
| Node.js event loop | 🟡 For I/O-heavy code | P's effect system + native threads. Different model but capable |
| Existing DevOps tooling | 🟡 Significant | P's build system (Phase 11) handles compilation. CI/CD via CLI |

### Migration Path

```
Step 1: Write serverless function in P with @serverless (tree-shake + WASM output)
Step 2: Use capability-aware linking: --caps "IO,Net" strips everything else
Step 3: Deploy as WASM module to Cloudflare Workers / Fastly / AWS Lambda (WASM)
Step 4: Use sandbox { } to isolate untrusted request handlers
Step 5: Use effect system to verify functions only do what they declare
```

### Verdict

🟢 **Worth it for edge computing and WASM-based serverless.** P's tiny
binaries (KB not MB) and capability-aware linking are genuine advantages
for edge deployment where size and security matter. The barrier is platform
support, but WASM compatibility means P works anywhere WASM works.

**Killer demo:** A serverless API in P that compiles to a 50KB WASM module
with `@serverless` (tree-shaken), `--caps "IO,Net"` (only IO+Net functions
included), and `sandbox { }` for request isolation. Show it cold-starting
in <1ms. That's something Go and Node.js cannot match.

---

## 6. AI/ML — Switching from Python / C++ / CUDA / Mojo

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Two-language problem | Python for research, C++/CUDA for production | Rewrite everything, bugs in translation |
| CUDA strings | Write GPU kernels as string literals | No type checking, no IDE, runtime crashes |
| Shape errors | Runtime dimension mismatch | Crash in production, not at compile time |
| Gradient bugs | Manual autograd, hope it's right | Silent wrong gradients = wrong model |
| Deployment bloat | Ship Python runtime + PyTorch + CUDA | GB-sized containers for a small model |
| Inference latency | Python GIL, interpreter overhead | Slow inference, need ONNX/TensorRT workaround |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| `@differentiable` | Compiler traces IR, auto-generates backward pass | Python (PyTorch runtime), C++ (manual), Mojo (yes but new) |
| `@no_grad` | Disable gradient tracking for inference — zero overhead | Python (torch.no_grad() but runtime), C++ (manual) |
| Shape types | Compile-time dimension checking — shape mismatch = compile error | Python (runtime), C++ (no), Mojo (partial) |
| `@shader` / GPU | Write GPU kernels in P, compiler emits SPIR-V | Python (CUDA strings), C++ (CUDA strings) |
| `@simd` | Auto-vectorize inference loops to SSE/AVX/NEON | Python (NumPy runtime), C++ (manual intrinsics) |
| Native performance | No interpreter, no GIL — fast inference | Python (GIL + interpreter), Java (GC) |
| Tiny deployment | Compile model + inference to native binary (KB) | Python (GB container), PyTorch (GB) |
| Layer/model syntax | `layer Dense { ... }` — neural network syntax is native | Python (class boilerplate), C++ (class boilerplate) |
| Quantization | `w ~> q8` — native quantization syntax | Python (manual), C++ (manual) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| PyTorch/TensorFlow ecosystem | 🔴 Critical for training | P targets *inference deployment*, not training. Use PyTorch for training, P for deployment |
| Jupyter notebooks | 🔴 Critical for research | P REPL exists. Jupyter kernel possible in Phase 11 |
| Python ML libraries (sklearn, pandas, etc.) | 🔴 Critical | Rebuild core libraries in P. Start with tensor + autograd |
| Hugging Face model hub | 🟡 Significant | P can load exported models (ONNX format). FFI to existing runtimes |
| CUDA ecosystem | 🟡 For training | P's SPIR-V backend targets GPU. For training, PyTorch is still better |
| Mojo (direct competitor) | 🟡 Mojo has Python interop | P differentiates with safety systems (effects, capabilities, regions) that Mojo lacks |

### Migration Path

```
Step 1: Train model in PyTorch (keep existing workflow)
Step 2: Export model to P using comptime (generate P code from model graph)
Step 3: Write inference loop in P with @no_grad (zero tracing overhead)
Step 4: Use @simd for auto-vectorized inference
Step 5: Use @shader for GPU inference (SPIR-V output)
Step 6: Deploy as tiny native binary (KB not GB)
```

### Verdict

🟡 **Worth it for inference deployment and edge AI, not for training.**
Python's training ecosystem (PyTorch, Jupyter, Hugging Face) is too entrenched
to displace. But *deploying* models is a different story — P's tiny binaries,
native performance, `@no_grad`, `@simd`, and `@shader` make it compelling for
production inference. Mojo is the direct competitor, but P differentiates with
safety systems Mojo doesn't have.

**Killer demo:** A quantized transformer inference engine in P with `@no_grad`
(no tracing), `@simd` (auto-vectorized matmul), shape types (compile-time
dimension checking), and `~> q8` quantization. Deploy as a 100KB binary.
Show it running faster than PyTorch with 1000x smaller deployment. That's
the inference deployment pitch.

---

## 7. Systems Programming — Switching from C / Rust / Zig / Go

### The Pain Today

| Problem | Current "Solution" | Why It Hurts |
|---------|-------------------|--------------|
| Memory safety without GC | Rust (borrow checker) or C (unsafe) | Rust: learning curve. C: segfaults |
| Borrow checker complexity | Fight it, use Rc/RefCell, give up | #1 Rust complaint, 27.9% call it a "big problem" |
| No effect tracking | Manual reasoning, code review | Hidden side effects = bugs |
| No capability isolation | OS-level sandboxing, containers | Heavy, not compile-time |
| Slow compile times (Rust) | sccache, incremental, pray | 45% of Rust quitters cite compile times |
| C's undefined behavior | Sanitizers, valgrind, hope | One UAF = security vulnerability |
| Go's GC | Tune GC, accept pauses | Not suitable for realtime/low-latency |
| Zig's lack of safety | Manual discipline | No compile-time safety guarantees |

### What P Gives Them

| P Feature | Benefit | Impossible In |
|-----------|---------|---------------|
| Region-based safety | Memory safety without borrow checker complexity | Rust (borrow checker), C (unsafe), Zig (no guarantee) |
| Effect system | Know at compile time what code does (IO? Alloc? Pure?) | C (no), Rust (no), Zig (no), Go (no) |
| Capability system | Compile-time authority revocation — `sandbox { }` | C (no), Rust (no), Zig (no), Go (no) |
| Comptime | Compile-time codegen, reflection, no macros | C (macros), Rust (complex macros), Go (no generics until 1.18) |
| Native codegen | Zero external tools — PE, ELF64, Mach-O, COFF, WASM, SPIR-V | Rust (LLVM), Go (own but limited targets), Zig (LLVM) |
| No GC | Region-based memory management | Go (GC), Java (GC), Python (GC) |
| Memory budgets | Compile-time allocation limit | C (no), Rust (no), Zig (no), Go (no) |
| Fast compilation | No LLVM dependency, direct codegen | Rust (LLVM, slow), C++ (slow), Swift (slow) |

### What They Lose

| Loss | Severity | Mitigation |
|------|----------|------------|
| Rust crate ecosystem | 🔴 Critical | Build core libraries in P. FFI for existing C/Rust libraries |
| C ABI compatibility | 🟡 P has FFI | FFI works. P can call C libraries |
| Zig's C interop story | 🟡 Zig is better at this | P's FFI is functional. Improve over time |
| Go's goroutines | 🟡 For concurrent servers | P's `parallel { | }` fork-join (Phase 7). Effect system for concurrency safety |
| Rust's borrow checker guarantees | 🟡 Different safety model | P's region system provides different but comparable safety. Formal proof in Phase 12 |
| Maturity | 🔴 Critical | This is the real barrier. Solved by time + users + real projects |

### Migration Path

```
Step 1: Write new module in P, call existing C/Rust via FFI
Step 2: Use region system for memory safety (simpler than borrow checker)
Step 3: Use effect system to track side effects across the codebase
Step 4: Use sandbox { } to isolate untrusted/plugin code
Step 5: Use comptime for code generation (replace macros/templates)
Step 6: Gradually migrate modules, one at a time
```

### Verdict

🟢 **This is P's strongest sector.** The combination of region-based safety
(simpler than Rust), effect tracking (no one has this), capability revocation
(no one has this), and comptime (like Zig) is genuinely unique. The barrier
is ecosystem maturity, not technology.

**Killer demo:** A web server in P with sandbox blocks for plugin isolation,
effect-tracked request handlers (prove a handler is pure), region-based
memory management (no GC, no borrow checker), and comptime-generated
serialization. Show it handling 100K requests/sec with zero crashes. That's
the systems programming pitch.

---

## Cross-Industry Synergy: The Real Moat

The killer insight: **switching to P for one industry makes switching to
P for another industry free.**

```
Gaming company adopts P for @crashfree
    → Already has the effect system
    → @deterministic for medical is just another annotation on the same system
    → @gas for blockchain is just another annotation on the same system
    → @realtime for robotics is just another annotation on the same system

Robotics company adopts P for @realtime
    → Already has the region system
    → @pool for gaming is just another annotation on the same system
    → @firmware for IoT is just another annotation on the same system
    → memory budgets for safety are just another annotation on the same system
```

**No other language offers this.** If you learn Rust for systems programming,
you can't use it for blockchain gas verification (no effect system). If you
learn Solidity for blockchain, you can't use it for robotics (no realtime
guarantees). If you learn C++ for gaming, you can't use it for medical
devices (no deterministic enforcement).

P is the only language where **the safety systems are the product, and the
domain annotations are just configurations of those systems.**

---

## Summary: Migration Difficulty by Industry

| Industry | From | Migration Difficulty | P's Value | Verdict |
|----------|------|---------------------|-----------|---------|
| **Systems** | C/Rust/Zig | 🟡 Medium | 🟢 High — unique safety combo | **Primary target** |
| **Robotics/IoT** | C/C++ | 🟡 Medium | 🟢 High — @realtime + @interrupt + @firmware | **Strong target** |
| **Blockchain** | Solidity/Rust | 🟡 Medium | 🟢 High — @gas + @deterministic + sandbox | **Strong target** |
| **Medical** | C++/Ada | 🔴 Hard (regulatory) | 🟢 High — @deterministic + @audit + @crashfree | **Strong target, slow adoption** |
| **Gaming** | C++/C# | 🟡 Medium | 🟢 High — @crashfree + @shader + ECS + @pool | **Good target (indie first)** |
| **Serverless/Edge** | Go/TS | 🟡 Medium | 🟢 High — tiny binaries + WASM + capability linking | **Good target** |
| **AI/ML** | Python/C++ | 🔴 Hard (ecosystem) | 🟡 Medium — inference only, not training | **Niche target (inference)** |

## The Pitch

> **Learn P once. Use it for your game server (`@crashfree`), your smart
> contract (`@gas`), your robot firmware (`@realtime`), your medical device
> (`@deterministic`), your edge function (`@serverless`), and your inference
> engine (`@no_grad`). Same language, same safety systems, different
> annotations.**
>
> **No other language does this.**

---

*This document should be updated as domain annotations are implemented and
real migration experiences are gathered.*
