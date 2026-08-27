# Fides Innovation — AI, Robotics, Hardware, IoT

> Fides is designed for the next 50 years of computing, not the last 50.

## Why Existing Languages Fail at the Future

### C/Rust/Zig were designed for CPUs
- No native SIMD support (C/Rust have intrinsics but they're not first-class)
- No tensor types (you need libraries like NumPy or PyTorch)
- No GPU targeting (you need CUDA/HIP/SYCL — separate languages)
- No real-time guarantees (C has volatile, Rust has no formal real-time story)

### Python/Java/Go were designed for servers
- Garbage collection pauses break real-time
- Runtime overhead breaks embedded constraints
- No hardware access (can't write a device driver in Python)
- No memory predictability (GC can pause at any time)

### The gap
No language can:
1. Write a robot control loop with real-time guarantees
2. Run an AI inference on a microcontroller
3. Target a GPU without a separate language
4. Write a device driver with memory safety
5. Hot-reload code on a running robot

**Fides targets all of these.**

---

## AI/ML — First-Class Support

### SIMD Types (WORKING — see `stdlib/simd.fd`)
```
// SIMD is a first-class type, not an intrinsic
pv a: simd[i64; 4] = [1, 2, 3, 4];
pv b: simd[i64; 4] = [5, 6, 7, 8];
pv c: simd[i64; 4] = a + b;  // vectorized addition, one instruction
```

### Tensor Type (WORKING — see `stdlib/tensor.fd`)
```
// Tensors are n-dimensional arrays with stride info
pv weights: Tensor[f32; 784, 128] = load_weights("model.bin");
pv input: Tensor[f32; 1, 784] = load_input("image.raw");
pv output: Tensor[f32; 1, 128] = matmul(input, weights);
```

### comptime Shape Verification
```
pf comptime matmul[M: comptime, N: comptime, K: comptime](
    a: Tensor[f32; M, K],
    b: Tensor[f32; K, N]
) -> Tensor[f32; M, N] {
    // Shape mismatch = compile error, not runtime error
    pv result: Tensor[f32; M, N] = Tensor::zeros();
    // ... implementation
    ret result;
}
```

### GPU Targeting (Implemented — SPIR-V codegen in `src/gpu.rs`)
```
// Same Fides code, different target
#[target(gpu)]
pf train_step(batch: Tensor[f32; 32, 784]) -> Tensor[f32; 32, 10] {
    // PIR lowers to SPIR-V or PTX for GPU
    // Same language, same types, different codegen
}
```

### Auto-Parallelization
```
// Effect system knows this is pure → can auto-parallelize
pf sum(xs: &[i64]) -> i64 ![] {
    pv total = 0;
    for x in xs {
        total += x;
    }
    ret total;
}
// Compiler can split xs into chunks and run on multiple cores
// No explicit threading needed
```

---

## Robotics — Real-Time + Safety

### Real-Time Effects
```
// !RealTime means: this function must complete within a deadline
pf control_loop(sensor: SensorCap) -> MotorCommand !RealTime !IO {
    pv reading = sensor.read()?;
    pv command = compute_pid(reading);
    ret command;
}

// The compiler verifies:
// 1. No allocations in !RealTime functions (no GC pauses)
// 2. No blocking IO (must use effect handlers)
// 3. No unbounded loops (must have iteration limit)
// 4. Region memory only (deterministic deallocation)
```

### Hardware Abstraction
```
// Robot sensors and actuators as capabilities
pf robot_main(motor: MotorCap, sensor: SensorCap) !IO !RealTime {
    loop {
        pv reading = sensor.read()?;
        pv command = control_loop(reading);
        motor.write(command)?;
    }
}

// The caller decides what hardware to use:
pf main() {
    pv motor = MotorCap::new("/dev/ttyUSB0");
    pv sensor = SensorCap::new("/dev/i2c-1");
    robot_main(motor, sensor)?;
}
```

### Hot Reload on Running Robots
```
// Change the control_loop function while the robot is running
// No restart, no state loss, no safety violation
// The new code is type-checked and safety-checked before patching
```

---

## IoT & Embedded — Tiny + Deterministic

### Bare Metal Targeting
```
// No OS, no runtime, no stdlib — just raw hardware
#[target(bare_metal)]
#[no_std]
pf main() {
    // Direct register access (framework code, not user code)
    pv gpio = 0x3F200000 as *mut u32;
    unsafe_ptr_write(gpio, 1);  // turn on LED
}
```

### Deterministic Memory
```
// FixedBufferAllocator — no heap, no fragmentation, no surprises
pf firmware_main() {
    pv buf: [u8; 4096] = [0; 4096];
    pv alloc = FixedBufferAllocator::new(&buf);
    
    // All allocations come from buf — deterministic, no surprises
    pv data = alloc.alloc(256)?;
    process(data)?;
    // No free needed — allocator resets at end of scope (region)
}
```

### Tiny Binaries
```
// Fides produces smaller binaries than C because:
// 1. Dead code elimination at IR level (not link time)
// 2. No libc dependency
// 3. No runtime
// 4. comptime inlines constants (no runtime initialization)
// Target: < 4KB for a simple LED blinker
```

### Cross-Compilation
```
// Build for any target from any host
fidesc build --target riscv32-embedded    // RISC-V microcontroller
fidesc build --target arm64-bare-metal    // Raspberry Pi
fidesc build --target x86_64-linux        // Server
fidesc build --target wasm                // Web
```

---

## Hardware — Direct Access

### Inline Assembly
```
// Inline assembly for hardware-specific instructions (framework code only)
pf read_cycle_counter() -> u64 {
    asm!("rdtsc", out("rax") _, out("rdx") _);
    // Returns the CPU cycle counter
}
```

### Interrupt Handlers
```
#[interrupt(timer0)]
pf timer_interrupt() !RealTime {
    // Handle timer interrupt
    // No allocations, no blocking, no exceptions
    pv state = get_global_state();
    state.tick += 1;
}
```

### MMU Management
```
// OS kernel primitives (framework code)
pf map_page(virt: usize, phys: usize, flags: PageFlags) {
    pv pt = get_page_table();
    pt[virt >> 12] = phys | flags.bits();
    asm!("invlpg ({})", in(reg) virt);
}
```

---

## The Unified Vision

```
┌─────────────────────────────────────────────────────┐
│                  Fides Language                     │
│                                                     │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌──────────┐  │
│  │ AI/ML   │ │ Robotics│ │  IoT    │ │ Servers  │  │
│  │ SIMD    │ │ RT Effects│ │ Bare   │ │ Effects  │  │
│  │ Tensors │ │ Caps    │ │ Metal   │ │ Hot Reload│  │
│  │ GPU     │ │ Hot     │ │ Tiny    │ │ Fast     │  │
│  │         │ │ Reload  │ │ Bins    │ │ Compile  │  │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘  │
│                                                     │
│  ┌─────────────────────────────────────────────┐    │
│  │              PIR (Fides IR)                 │    │
│  │  Regions + Effects + Capabilities + comptime │    │
│  └─────────────────────────────────────────────┘    │
│                                                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────┐ │
│  │ x86-64   │ │ ARM64    │ │ RISC-V   │ │ WASM   │ │
│  │ Codegen  │ │ Codegen  │ │ Codegen  │ │ Codegen│ │
│  └──────────┘ └──────────┘ └──────────┘ └────────┘ │
│                                                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐            │
│  │ ELF      │ │ PE       │ │ Mach-O   │            │
│  │ Linux    │ │ Windows  │ │ macOS    │            │
│  └──────────┘ └──────────┘ └──────────┘            │
└─────────────────────────────────────────────────────┘
```

**One language. Every platform. Every domain. All safe. All fast. All custom-built.**

---

# Extra — Nice-to-Have Community Goals

> Not on the critical path. Not required for Fides to be useful.
> These are ideas that would help adoption and community growth,
> but the language and compiler are complete without them.

## Migration Tools

### C-to-Fides Transpiler (Feasible — moderate effort)

```
// Input: C code
int add(int a, int b) { return a + b; }

// Output: Fides code
pf add(a: i32, b: i32) -> i32 {
    ret a + b;
}
```

- Auto-translate C syntax to Fides syntax
- Add memory safety annotations (regions, capabilities)
- Flag unsafe patterns for manual review
- Gradual migration: translate one file at a time

**Honest assessment:** The `add` example is the easy 5%. Real C has
preprocessor macros, pointer arithmetic, `goto`, union punning, function
pointers, switch fallthrough, and implicit conversions. Existing precedent
(`c2rust`, `corrode`) shows this is a multi-year effort even with mature
ecosystems. Tractable but not trivial.

**What already works:** Fides has `cparser.rs` (C header parser) and
`extern "C"` FFI — calling C from Fides is already covered. A transpiler would
help porting C *logic* into Fides, but FFI covers the interop case.

### Rust-to-Fides Transpiler (Aspirational — very high effort)

```
// Input: Rust code
fn add(a: i64, b: i64) -> i64 { a + b }

// Output: Fides code
pf add(a: i64, b: i64) -> i64 {
    ret a + b;
}
```

- Convert Rust syntax to Fides syntax
- Replace lifetimes with regions (auto-inferred)
- Replace async/await with effect handlers
- Replace macros with comptime

**Honest assessment:** This is the hardest item in this file. Rust's borrow
checker, lifetimes, macros (`macro_rules!`, proc macros), complex trait
system, async, and `unsafe` don't map cleanly to Fides.

- "Replace lifetimes with regions" is optimistic — Fides's regions and Rust's
  lifetimes are semantically different systems. Mechanical translation is
  not straightforward.
- "Replace macros with comptime" only works for simple declarative macros.
  Proc macros generate arbitrary token streams that can't be transpiled
  without re-implementing the macro system.

**Practical alternative for Rust migration:** Use FFI to call Rust from Fides
(via C ABI), port files manually over time. A full Rust-to-Fides transpiler
that produces *correct, idiomatic* Fides code is a research project, not a
tooling task.

### FFI Generator (Already working)

```
// Input: C header (openssl.h)
SSL_CTX* SSL_CTX_new(const SSL_METHOD* method);

// Output: Fides FFI binding
extern "C" pf SSL_CTX_new(method: *const SSL_METHOD) -> *mut SSL_CTX;
```

- Parse C headers automatically
- Generate Fides FFI bindings
- No hand-written bindings needed
- **Working now** — `fidesc parse-header <file.h>` parses C headers and
  emits Fides FFI bindings (`src/cparser.rs`)

This is the only migration tool that exists today. It covers the most
common need (calling C from Fides) without requiring a full transpiler.

---

## Community & Documentation

### Language Specification
- Formal grammar + semantics document
- Not required for using Fides, but needed for third-party implementations
- and academic credibility

### Fides Book
- Like "The Rust Programming Language"
- Tutorial-style introduction for new users
- Would significantly help adoption

### Examples Library
- Real-world code samples (web server, CLI tool, embedded firmware)
- Shows Fides's strengths in practice

### Playground
- Web-based Fides IDE (compile + run in browser)
- Uses WASM backend (already implemented)
- Low effort since WASM target exists

### Community Forum / Discord
- For support, announcements, package sharing
- Pure community effort, no compiler work needed

---

## Medical Device Compliance Tooling

These items address gaps identified during analysis of the medical device industry's
language limitations. They are ecosystem/tooling features, not language features —
the language-level safety (regions, bounds, @crashfree, @medical, @audit) is already
in the roadmap (Phases 7.5 and 9B). These tools support the *process* side of medical
device compliance (IEC 62304, FDA cybersecurity guidance).

### SBOM Generator (`pacte sbom`) — Depends on package manager

- Scan Fides import graph, generate Software Bill of Materials
- List all dependencies: stdlib modules, FFI libraries, transitive imports
- Include versions and content hashes
- Required for FDA 2025 cybersecurity compliance and IEC 62304 SOUP management
- Output format: SPDX or CycloneDX (industry-standard SBOM formats)

**Why this is extra, not roadmap:** An SBOM is only meaningful with versioned,
registered packages. Fides has pacte package manager (Phase 13.4). SBOM generation is now a natural companion feature.

### CVE Checker (`pacte audit`) — Depends on SBOM + package registry

- Check dependencies against known vulnerability databases (NVD, OSV)
- Flag CVEs in SOUP (Software of Unknown Provenance) components
- Requires SBOM generator + package registry to be useful
- Ongoing maintenance: CVE databases update constantly

**Why this is extra, not roadmap:** Same dependency as SBOM — needs a package
registry. Also requires ongoing infrastructure (CVE database updates). Consider
integrating with existing services (NVD API, OSV.dev) rather than hosting a
database. This is a CI/CD tool, not a compiler feature.

### IEC 62304 Process Integration — Tooling, not compiler

IEC 62304 is a *process* standard, not a coding standard. It requires:
- **Requirements traceability:** each requirement → design → code → test
- **Risk management:** each hazard → risk control → verification
- **Problem resolution:** each bug → root cause → fix → regression test

Fides's language-level features (@crashfree, @medical, @audit, @bounded) serve
as *risk control measures* in the IEC 62304 risk management process. But the
*process tooling* — linking hazards to code, tracing requirements through
implementation, tracking problem resolution — is a separate concern.

**What this tooling would do:**
- Link Fides functions and tests to requirement IDs (via annotations or external mapping)
- Export traceability matrices for auditor review
- Map @crashfree/@medical/@bounded annotations to hazard IDs as risk controls
- Integrate with issue trackers for problem resolution tracking

**Why this is extra, not roadmap:** Full IEC 62304 process tooling is a product
category on its own (companies like Polarion, Jama, Visure sell this). Building
it into Fides is scope creep. The right approach: Fides provides the language-level
safety guarantees, and third-party tooling or integrations handle the process-level
requirements. Consider an export format that integrates with existing ALM tools
rather than building a full requirements management system.

### CWE Mapping Documentation (`docs/fides-cwe-mapping.md`) — Low effort, high value

Map Fides safety features to CWE (Common Weakness Enumeration) categories for
medical device auditor review:

| CWE ID | CWE Name | Fides Prevention |
|--------|----------|-----------------|
| CWE-787 | Out-of-bounds Write | Bounds checking + region escape detection |
| CWE-416 | Use-After-Free | Region system (compile-time lifetime tracking) |
| CWE-476 | NULL Pointer Dereference | Option<T> (no null in the language) |
| CWE-190 | Integer Overflow | Checked arithmetic in debug mode |
| CWE-119 | Improper Bounds Restriction | Bounds checking (compile-time + runtime) |
| CWE-20 | Improper Input Validation | @crashfree (reject unhandled errors) |
| CWE-125 | Out-of-bounds Read | Bounds checking + region escape detection |
| CWE-788 | Access of Memory Location of Wrong Size | Type system (fixed-width types) |
| CWE-479 | Use of a Pointer Derived from an Untrusted Source | Capability system (untrusted code can't access pointers) |

**Why this is extra, not roadmap:** This is documentation, not code. It doesn't
change the compiler. It can be written now — the safety features it maps (regions,
bounds, Option) already exist. The only risk is accuracy: if the mapping claims a
CWE is prevented and there's an edge case, that's a false claim in a safety-critical
context. The mapping must be verified, not just written. Low effort, high value
for medical device adoption — auditors expect CWE coverage documentation.

---

## Why These Are Separate From the Roadmap

The main roadmap (`docs/roadmap.md`) tracks what's needed to make Fides a
complete, correct, fast language. The items here are about making Fides
*popular* — which depends on community, marketing, and adoption dynamics
that are orthogonal to compiler correctness.

**Priority:** Finish Phases 4–5 (fix regalloc, C codegen coverage) first.
Then Phase 6 (self-hosting). Then Phase 7 ecosystem tooling (LSP, package
manager, formatter). The items in this file are "if there's time and
interest" — not blockers for anything.

---

## What Actually Drives Adoption (Not Transpilers)

Based on how languages actually grow communities:

1. **A killer app** — something Fides can do that others can't (hot reload +
   safety + bare metal in one language)
2. **Good docs** — people can learn Fides in an afternoon
3. **LSP support** — IDE autocomplete, go-to-definition, error highlighting
4. **Package manager** — `pacte add <dep>` just works
5. **Real projects** — at least one notable project written in Fides

Transpilers are nice marketing but historically don't drive adoption.
TypeScript succeeded because of types + JS ecosystem, not because of
transpilers *to* TypeScript. Rust succeeded because of safety + cargo,
not because of C-to-Rust tools.
