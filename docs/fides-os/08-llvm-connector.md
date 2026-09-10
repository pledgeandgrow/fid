# Fides OS — LLVM Integration Strategy

> Should LLVM be added to the Fides compiler? No. Should Fides OS support
> running C/C++/Rust programs? Yes, via a connector, not a dependency.

## The Three Options

### Option A: Add LLVM to the Fides Compiler (REJECTED)

This means LLVM becomes the backend of `fidesc`, like Rust uses LLVM.

**Do NOT do this.** Reasons:

| Problem | Impact |
|---------|--------|
| LLVM is 60M+ lines of C++ | Breaks self-hosting, adds massive dependency |
| LLVM doesn't understand effects | @no-alloc, @realtime, @medical lost at IR boundary |
| LLVM doesn't understand capabilities | Capability table, info-flow labels lost |
| LLVM doesn't understand regions | Flat memory model, no memory pools |
| LLVM's optimizer can break Fides invariants | May optimize away security checks |
| Build time | 30-60 min (LLVM) vs 2 min (Fides) |
| Attack surface | 200+ CVEs in LLVM |

Fides already has its own backends (x86-64, ARM64, RISC-V, PE, ELF, WASM, C
codegen). They're smaller, faster to build, and understand Fides's semantics.

### Option B: Build a Connector — LLVM Targets Fides OS (APPROVED)

LLVM stays a separate tool. You write an LLVM backend that emits
Fides-compatible binaries. C/C++/Rust programs compiled with this LLVM can
run on Fides OS.

```
Two separate paths:

Path 1 (Fides programs — full security):
  Fides source -> fidesc -> Fides IR -> Fides backends -> Fides binary
  (effects, capabilities, regions, memory safety — all enforced)

Path 2 (C/C++/Rust programs — compatibility):
  C/C++/Rust source -> clang/rustc -> LLVM IR -> Fides OS ABI target -> Fides binary
  (no effects, no capabilities, no memory safety — isolated in compatibility stratum)
```

### Option C: Don't Use LLVM at All (Phase 1 Only)

No C/C++/Rust programs. Everything is Fides. Maximum security, but no
existing software.

**Good for fixed-purpose systems (banking, mining, AI), bad for
general-purpose desktop.**

---

## The Recommended Approach: Option B, Phased

### Phase 1: Pure Fides (Now — Year 1)

Don't touch LLVM. Build fixed-purpose profiles:
- Banking terminal (pure Fides, 5 strata)
- Crypto mining rig (pure Fides, 3 strata)
- AI training cluster (pure Fides, 5 strata)
- Developer workstation (pure Fides, compiler + tools)

### Phase 2: Fides OS ABI + Loader (Year 1-2)

Define the Fides OS ABI and build the binary loader. This lets Fides-compiled
programs run as native processes on Fides OS.

```
Fides source -> fidesc build --target fides-os -> Fides OS binary
The binary declares:
  capabilities: [fs:root, net:loopback, gpu:0]
  effects: [@no-panic, @deterministic]
  regions: [gpu:8GB, kv_cache:2GB]
The loader reads this, sets up the process accordingly.
```

### Phase 3: LLVM Connector (Year 2-3)

Build the LLVM Fides backend and port musl libc. C/C++/Rust programs can be
compiled to run on Fides OS:

```bash
# On a Linux dev machine:
clang --target=fides-os -o myprogram myprogram.c

# Copy to Fides OS:
ark add myprogram --bin
ark install

# Run on Fides OS (in compatibility stratum):
$ myprogram
```

### Phase 4: Port Key Software (Year 3-5)

| Software | Why | Approach |
|----------|-----|----------|
| git | Every developer needs it | Port (C, LLVM connector) |
| Servo (browser) | Web browsing | Port (Rust, LLVM connector) |
| Godot (game engine) | Gaming | Port (C++, LLVM connector) |
| SQLite | Database | Port (C, LLVM connector) |
| ffmpeg | Media | Port (C, LLVM connector) |
| Python | Data science | Port CPython (C, LLVM connector) |

Each runs in an isolated compatibility stratum with inferred capabilities.

---

## The Clean Architecture

```
+-----------------------------------------------------+
|                    Fides OS                          |
|                                                     |
|  +---------------------------------------------+    |
|  |  Stratum 0: Core (Fides)                     |    |
|  |  Effects + capabilities + regions + safety   |    |
|  +---------------------------------------------+    |
|                                                     |
|  +----------------+    +---------------------+     |
|  | Fides-native   |    | Compatibility       |     |
|  | applications   |    | stratum             |     |
|  |                |    |                     |     |
|  | fidesc build   |    | LLVM Fides target   |     |
|  | -> Fides binary|    | -> Fides binary     |     |
|  |                |    |                     |     |
|  | Full effects   |    | Inferred caps       |     |
|  | Full caps      |    | No effects          |     |
|  | Full regions   |    | No regions          |     |
|  | Full safety    |    | Isolated sandbox    |     |
|  +----------------+    +---------------------+     |
|         |                        |                 |
|  +------+------------------------+----------+      |
|  |  Fides OS ABI (common binary format)     |      |
|  |  - Calling convention                   |      |
|  |  - Symbol table                         |      |
|  |  - Capability declarations               |      |
|  |  - Region declarations                   |      |
|  +------------------------------------------+      |
|         |                                          |
|  +------+-----------------------------------+      |
|  |  Binary Loader (Fides)                   |      |
|  |  Reads ABI, sets up process with caps    |      |
|  +------------------------------------------+      |
+-----------------------------------------------------+
```

## What the LLVM Fides Target Does

```cpp
// llvm/lib/Target/FidesOS/FidesOSTargetMachine.cpp
// ~8,000 lines total (similar to WebAssembly backend)

class FidesOSTargetMachine : public LLVMTargetMachine {
    // Emit Fides OS ABI binary format
    // System V calling convention (compatible with existing Fides backends)
    // Flat memory model (no regions — compatibility stratum)
    // Capability inference from libc calls:
    //   fopen -> cap:fs
    //   socket -> cap:net
    //   mmap -> cap:mem
    //   printf -> cap:stdout
};
```

The target is simple because:
- Uses System V ABI (already supported by clang)
- Emits a flat binary (no regions, no effects)
- Fides OS loader handles capability setup
- libc (musl) provides the syscall interface

## What the LLVM Target Does NOT Do

- No effect verification (C has no effect annotations)
- No memory safety (C has buffer overflows)
- No region awareness (C uses malloc, not regions)
- No capability tracking (C has ambient authority)

The compatibility stratum compensates:
- Capabilities inferred from static analysis of libc calls
- Memory safety enforced at runtime (isolated address space)
- A buffer overflow crashes the app, not the OS

## Fides OS ABI Format

```
Fides OS Binary Format (FEB):
  Header:
    magic: "FEB\x01"
    version: 1
    entry_point: u64
    capabilities: [CapabilityDecl]
    regions: [RegionDecl]
    effects: [EffectDecl]           // Fides-only
    symbols: [SymbolTable]
  Code:
    .text section
    .data section
    .rodata section
  Manifest:
    origin: "fides" | "llvm"
    memory_safe: bool               // Fides = true, LLVM = false
    effect_verified: bool           // Fides = true, LLVM = false
```

The loader reads the manifest and sets up the process accordingly:
- Fides binary -> full effect enforcement, region mapping, capability table
- LLVM binary -> isolated sandbox, inferred capabilities, no regions

## Effort Estimate

| Component | Lines of code | Time |
|-----------|-------------|------|
| Fides OS ABI spec | Document | 1 week |
| LLVM Fides backend | ~8,000 C++ | 2-3 months |
| Fides OS binary loader | ~1,000 Fides | 2 weeks |
| libc (port musl) | ~15,000 C (port) | 1 month |
| **Total** | ~24,000 | 3-4 months |

## Why Not Add LLVM to Fides Itself — Detailed

1. **Destroys self-hosting:** Fides compiler depends on 60M-line C++ project
2. **Breaks effect system:** LLVM has no concept of @no-alloc, @medical, @secret
3. **Breaks region-aware memory:** LLVM's memory model is flat
4. **Fides already has better backends:** They understand effects, regions, capabilities
5. **LLVM's optimizer isn't better for Fides:** It doesn't know which functions are pure
6. **Fides's own optimizer is designed for Fides:** 23 passes, effect-aware

## Summary

| Question | Answer |
|----------|--------|
| Add LLVM to Fides compiler? | No. Destroys self-hosting, breaks effects/regions/capabilities. |
| Build LLVM connector? | Yes, in Phase 3. LLVM stays separate, targets Fides OS ABI. |
| Use Fides's own backends? | Yes, always for Fides programs. |
| Can C/C++/Rust run on Fides OS? | Yes, via LLVM connector. Isolated, capability-scoped. |
| Do we need LLVM at all? | No for Fides programs. Yes for compatibility. |

**The principle: Fides is the primary path. LLVM is the compatibility path.
Fides programs get full security. C/C++/Rust programs get isolation +
capabilities but not effects. Both run on the same OS, through the same
loader, with the same ABI.**
