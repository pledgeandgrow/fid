# The Stratified Kernel — Full Design

> The Stratified Kernel is a memory-safe, effect-verified, capability-based,
> region-aware, comptime-configured operating system architecture built in
> Fides.

## Overview

```
+---------------------------------------------------+
|              Stratum 0: Core                      |  <- Always present
|  +---------------------------------------------+  |     (irreducible core)
|  |  Scheduler / IPC / VM / Capabilities         |  |
|  |  ~3,000 lines, formally verifiable           |  |
|  |  @no-alloc @no-io @deterministic             |  |
|  +---------------------------------------------+  |
+---------------------------------------------------+
          |              |              |
          v              v              v
+--------------+ +--------------+ +--------------+
|  Stratum 1   | |  Stratum 1   | |  Stratum 1   |  <- Pick what you need
|  Serial I/O  | |  VGA Console | |  Keyboard    |
|  @no-alloc   | |  @no-alloc   | |  @no-alloc   |
|  caps: [io]  | |  caps: [mem] | |  caps: [io]  |
+--------------+ +--------------+ +--------------+
          |              |              |
          v              v              v
+--------------+ +--------------+ +--------------+
|  Stratum 2   | |  Stratum 2   | |  Stratum 2   |  <- Higher-level services
|  Filesystem  | |  Network     | |  GPU Driver  |
|  @no-io      | |  @tainted    | |  @quantized  |
|  caps: [disk]| |  caps: [nic] | |  caps: [gpu] |
|  region:disk | |  region:net  | |  region:gpu  |
+--------------+ +--------------+ +--------------+
          |              |              |
          v              v              v
+--------------+ +--------------+ +--------------+
|  Stratum 3   | |  Stratum 3   | |  Stratum 3   |  <- Application frameworks
|  Window Mgr  | |  TCP Stack   | |  AI Runtime  |
|  @no-panic   | |  @tainted    | |  @differenti |
|  caps: [fb]  | |  caps: [nic] | |  caps: [gpu] |
+--------------+ +--------------+ +--------------+
```

## Stratum 0 — The Irreducible Core (~3,000 lines)

**Responsibilities:**
- Page table management
- Capability table
- IPC (message passing between strata)
- Scheduler (preemptive, deadline-aware for @realtime)
- Interrupt dispatch (IDT setup, vector routing)

**Properties:**
- `@no-alloc` (static memory only)
- `@no-io` (no device access — delegates to stratum 1)
- `@deterministic` (no timing leaks)
- Formally verified (seL4-style proof)

**Cannot do:**
- File I/O (no filesystem — that's stratum 2)
- Network (no NIC access — that's stratum 2)
- Display (no VGA — that's stratum 1)
- Dynamic allocation (no malloc — static memory only)

## Stratum 1 — Hardware Drivers (~500 lines each)

Each driver is a stratum:
- Serial I/O (`@no-alloc`, `caps: [io:0x3F8]`)
- VGA Console (`@no-alloc`, `caps: [mem:0xB8000]`)
- Keyboard (`@no-alloc`, `caps: [io:0x60]`)
- Timer (`@no-alloc`, `caps: [io:0x40]`)
- Disk Driver (`@no-alloc`, `caps: [io:0x1F0]`)

**Properties:**
- `@no-alloc` (drivers use static buffers)
- Capability-scoped (can only access their assigned hardware)
- Effect-verified (no IO outside their port range)

**Can be:**
- Inlined into stratum 0 (for performance-critical profiles)
- Isolated as user-space processes (for security-critical profiles)

## Stratum 2 — Services (~2,000-5,000 lines each)

- Filesystem (`caps: [disk]`, `region:disk`, `@no-io`)
- Network Stack (`caps: [nic]`, `region:net`, `@tainted`)
- GPU Driver (`caps: [gpu]`, `region:gpu`, `@quantized`)
- Audio (`caps: [audio]`, `region:realtime`, `@realtime`)

**Properties:**
- Each service has its own address space
- Capabilities restrict what hardware they can access
- Regions restrict what memory pools they can allocate from
- Effects restrict what kind of computation they can do

## Stratum 3 — Application Framework (~10,000-50,000 lines)

- Window Manager (`caps: [fb]`, `@no-panic`)
- Shell (`caps: [fs:root, exec]`, `@tainted`)
- Package Manager (`caps: [fs:pkg, net:pkg]`, `@tainted`)
- AI Runtime (`caps: [gpu]`, `region:kv_cache`, `@differentiable`)
- Banking App (`caps: [fs:bank]`, `@medical`, `@no-panic`, `@no-io`)

**Properties:**
- Least privilege — only capabilities explicitly granted
- Info-flow labeled — `@secret` data can't reach `@tainted` sinks
- Effect-verified — `@medical` apps can't panic, `@realtime` apps meet deadlines

## The Compilation Flow

```fides
// kernel.fd — the kernel configuration file
comptime {
    profile = "terminal"  // or "ai_training", "general_purpose", etc.

    match profile {
        "terminal" => {
            strata = [Core, Serial, Keyboard, VGA, BankingApp]
            inline = [Serial, Keyboard, VGA]  // in-kernel for speed
            isolated = [BankingApp]            // user-space for safety
            verify = Formal                     // formally verify core
        }
        "ai_training" => {
            strata = [Core, Serial, Network, Filesystem, GPU, AIRuntime]
            inline = [Serial]
            isolated = [Network, Filesystem, GPU, AIRuntime]
            verify = Effects
        }
        "general_purpose" => {
            strata = [Core, Serial, Keyboard, VGA, Mouse, Audio, USB,
                      Filesystem, Network, GPU, WindowMgr, Shell, PackageManager]
            inline = [Serial, Keyboard, VGA, Mouse]
            isolated = [Filesystem, Network, GPU, WindowMgr, Shell]
            verify = Effects
        }
    }
}
```

The compiler then:
1. Reads the profile configuration
2. Compiles only the selected strata
3. Inlines or isolates each stratum as specified
4. Generates the capability table from the strata's requirements
5. Verifies effects across stratum boundaries
6. Verifies info-flow labels (no `@secret` -> `@tainted` leaks)
7. Produces a single binary with only the code needed

## Effect-Driven Kernel (Fides-Unique)

The kernel dispatches based on effect annotations, not just privilege rings:

- `@no-alloc` driver -> kernel won't map new pages for it
- `@realtime` driver -> guaranteed CPU time slices, deadline tracking
- `@medical` driver -> medical-safety enforcement (no panics, no divergence)
- `@deterministic` driver -> no IO, no information leakage
- `@quantized` driver -> dedicated INT8 memory pool

No existing OS uses compile-time effect analysis to drive kernel-level
resource allocation and isolation.

## Region-Aware Memory (Fides-Unique)

Fides's region system maps to hardware memory pools:

- `region dma_pool with budget(4096)` -> uncached, physically contiguous
- `region kv_cache with budget(1024)` -> sliding-window memory pool
- `region realtime with budget(8192)` -> wired pages (no swap, no page faults)
- `region gpu_pool with budget(65536)` -> GPU memory pool

Budget is enforced at compile time (typechecker) AND runtime (kernel).

## Comptime-Configured Kernel (Fides-Unique)

The kernel is configured at compile time using Fides's comptime system:
- No runtime configuration files
- No `/etc`, no registry
- Only needed drivers compiled in
- Configuration is verified at compile time (types, effects, capabilities)

## Capability-Flow Kernel (Fides-Unique)

Capabilities carry data-flow labels:
- `cap: [fs:read] label: Secret` -> can read, but result is tagged Secret
- Secret data can't reach network sink (no cap for net)
- `@tainted` data (from network) can't reach `@deterministic` computation

This is Bell-LaPadula + Biba integrity model, enforced by the language
and kernel together.

## Inlining vs. Isolation

Each stratum can be:
- **Inlined** into stratum 0 (monolithic-style for that service) — fast, no
  IPC overhead, but a bug affects the core
- **Isolated** as a user-space process (microkernel-style) — safe, IPC
  overhead, but a bug is contained

The choice is **per-stratum, per-profile, at compile time**. This eliminates
the fundamental monolithic-vs-microkernel trade-off.

```fides
comptime {
    // Terminal profile: serial is in-kernel (fast, no IPC)
    strata_inline = [Serial, Keyboard, VGA]

    // General profile: serial is isolated (safe, IPC)
    strata_inline = []
}
```

## Comparison with Existing OSes

| Mistake | Linux/Windows | seL4 | Fides OS |
|---------|---------------|------|----------|
| C/asm kernel | Yes | Yes (verified C) | No (Fides is memory-safe) |
| Monolithic | Yes | No (microkernel) | No (stratified) |
| Ambient authority | Yes | No (capabilities) | No (capabilities) |
| Shared address space | Yes | No (isolated) | No (per-stratum) |
| No info-flow control | Yes | Yes | No (@secret/@tainted) |
| No effect verification | Yes | Yes | No (effect system) |
| No region-aware memory | Yes | Yes | No (region system) |
| No comptime config | Yes | Yes | No (comptime) |
| Remote kernel access | Yes | No | No (architectural air-gap) |
| Attack surface doesn't scale | Yes (fixed) | Yes (fixed) | No (scales with strata) |

## Future Combinations

The Stratified Kernel is the foundation. Additional approaches can be
combined to make it even more powerful:

- **CHERI hardware capabilities** — hardware-enforced capability pointers
  (defense in depth with software capabilities)
- **Temporal kernel** — snapshot/rollback/replay for debugging and forensics
- **Forensic computing** — typed residual records at every authority
  transition (provable accountability)
- **Z3/SMT verification** — solver-verified invariants beyond the effect
  system
- **Intralingual design** — lock ordering, IRQ context safety, MMIO types
  in the type system
- **Runtime evolution** — verified, signed live updates for non-core strata
- **Multikernel mode** — per-core kernel instances for many-core scaling
- **AI-native kernel** — `@differentiable` kernel participates in autograd
- **Homomorphic computation** — `@secret` triggers encrypted computation
- **Post-quantum crypto** — quantum-resistant algorithms by default
- **Energy-aware scheduling** — effect-driven core type assignment
- **Self-healing strata** — `@crashfree` automatic restart with rollback

See [11-novel-combinations.md](11-novel-combinations.md) for full details on
each approach, what exists in research, what's genuinely novel, and
implementation priority.
