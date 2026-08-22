# IBM Lessons — What Fides Can Learn from AIX and z/OS

> IBM built the most vertically integrated computing stacks in history. They're
> also niche, closed, and locked to their own hardware. This document maps what
> Fides should steal from their playbook, what to avoid, and the concrete
> language-level features that get us z/OS-class guarantees on mainstream
> hardware — without the mainframe tax.
>
> **Important framing:** Fides is a **language**, not an OS. The OS comes later,
> once Fides is self-hosting. Everything in this document is about making the
> *language* the perfect foundation so that the future OS is built well,
> maximally performant, and tightly optimized for hardware. The goal: Windows
> convenience, Linux flexibility and openness, and benchmarks that beat both.

---

## Table of Contents

- [How IBM Built Their Stacks](#how-ibm-built-their-stacks)
- [Fides Today: What We Built vs. What We Depend On](#fides-today-what-we-built-vs-what-we-depend-on)
- [The Strategic Position](#the-strategic-position)
- [Four Features to Get z/OS-Class Guarantees on Mainstream Hardware](#four-features-to-get-zos-class-guarantees-on-mainstream-hardware)
  - [1. io_uring Runtime](#1-io_uring-runtime)
  - [2. Capability-Based Process Isolation](#2-capability-based-process-isolation)
  - [3. Transactional Memory at the Codegen Level](#3-transactional-memory-at-the-codegen-level)
  - [4. Deterministic Scheduling Mode](#4-deterministic-scheduling-mode)
- [The Road to a Fides OS](#the-road-to-a-fides-os)
- [What IBM Got Wrong — And How Fides Avoids It](#what-ibm-got-wrong--and-how-fides-avoids-it)

---

## How IBM Built Their Stacks

Neither AIX nor z/OS depends on LLVM, ARM, or any modern cross-platform
toolchain layer. Both predate LLVM by decades. They were built the hard way:
own compiler, own assembler, own object format, own linker, own kernel, own
runtime, own hardware.

### AIX (1986 — present)

| Layer | What AIX uses |
|-------|---------------|
| Hardware | IBM Power processors (POWER ISA — POWER9/POWER10) |
| Firmware | PowerVM (PHYP) hypervisor via PAPR interface |
| Kernel | IBM's own AIX kernel — monolithic UNIX (System V R3 + BSD) |
| Implementation | Mostly C + POWER assembly for low-level routines |
| Compiler | IBM XL C/C++ (native, not LLVM). Modern "Open XL" is LLVM-based but the OS wasn't built with it. |
| Hardware I/O | POWER privileged instructions + PAPR hypervisor calls |

### z/OS (1964 — present, lineage: OS/360 → MVS → OS/390 → z/OS)

| Layer | What z/OS uses |
|-------|---------------|
| Hardware | IBM Z mainframes (z/Architecture, 64-bit) |
| Firmware | LIC (Licensed Internal Code) — IBM-proprietary microcode below the OS |
| Kernel | z/OS's own kernel — NOT UNIX. Batch + transaction processing. USS (UNIX System Services) is a bolt-on subsystem. |
| Implementation | HLASM (assembler) + PL/S (IBM-internal systems language, derived from PL/I) + Assembler + C |
| Compiler | IBM Enterprise COBOL, PL/I, z/OS XL C/C++, HLASM — none are LLVM |
| Hardware I/O | z/Architecture privileged instructions + **channel I/O** — dedicated I/O processors handle device communication; the CPU issues one "start I/O" instruction and gets an interrupt when done. Fundamentally different from x86 MMIO/DMA. |

**The key insight:** z/OS is the most "from scratch" OS still in serious
production. Its lineage goes back to 1964. It uses its own ISA, its own
assembler, its own internal systems language (PL/S), its own I/O model
(channels), and its own compilers. Zero dependency on LLVM, ARM, x86, or any
modern cross-platform toolchain.

```
Modern Linux/Windows stack:          AIX stack:                    z/OS stack:
┌──────────────┐                     ┌──────────────┐              ┌──────────────┐
│  User apps   │                     │  User apps   │              │  User apps   │
├──────────────┤                     ├──────────────┤              ├──────────────┤
│  libc / CRT  │                     │  libc (AIX)  │              │  Language    │
│              │                     │              │              │  RTLs (IBM)  │
├──────────────┤                     ├──────────────┤              ├──────────────┤
│  Kernel      │                     │  AIX kernel  │              │  z/OS kernel │
│  (Linux/NT)  │                     │  (IBM UNIX)  │              │  (OS/360 →)  │
├──────────────┤                     ├──────────────┤              ├──────────────┤
│  UEFI/ACPI   │                     │  PAPR/PHYP   │              │  LIC         │
├──────────────┤                     ├──────────────┤              │  (microcode) │
│  x86/ARM HW  │                     │  POWER HW    │              ├──────────────┤
│              │                     │              │              │  z/Arch HW   │
└──────────────┘                     └──────────────┘              └──────────────┘

Compiler chain:                      Compiler chain:               Compiler chain:
  GCC / Clang(LLVM)                   IBM XL C/C++                  IBM HLASM / PL/S
  → LLVM IR → x86/ARM                 → POWER machine code          → z/Arch machine code
```

---

## Fides Today: What We Built vs. What We Depend On

Verified against the actual source tree (`Cargo.toml`, `Cargo.lock`,
`runtime_linux.rs`, all `process::Command` calls).

### What Fides Built Itself (zero external dependency)

| Layer | Fides implementation | File(s) |
|-------|---------------------|---------|
| Lexer | Own tokenizer | `lexer.rs` |
| Parser | Own recursive descent | `parser.rs` |
| Typechecker | Own (types, regions, effects, capabilities, borrow checker) | `typechecker.rs`, `borrowck.rs` |
| IR | Own SSA IR | `ir.rs` |
| Optimizer | Own PIR optimizer (23 passes) | `opt.rs`, `pir.rs` |
| Register allocator | Own linear scan with constant rematerialization | `regalloc.rs` |
| Peephole optimizer | Own (9 passes) | `peephole.rs` |
| x86-64 codegen | Own instruction encoding (manual ModRM/SIB/REX) | `codegen_x64.rs` |
| ARM64 codegen | Own | `codegen_arm64.rs` |
| RISC-V codegen | Own | `codegen_riscv.rs` |
| ELF64 emitter | Own (header bytes, sections, relocations — no `ld`) | `elf64.rs` |
| PE emitter | Own (Windows .exe directly — no linker) | `pe.rs` |
| Mach-O emitter | Own (macOS object files) | `macho.rs` |
| COFF emitter | Own (Windows object files) | `coff.rs` |
| Linker | Own (resolves relocations — no external `ld`) | `linker.rs` |
| Linux runtime | Own — **raw syscall bytes** (`0x0f 0x05`), no libc | `runtime_linux.rs` |
| WASM backend | Own (PIR → WASM binary) | `wasm.rs` |
| GPU backend | Own (PIR → SPIR-V binary) | `gpu.rs` |
| Interpreter | Own (reference oracle) | `interp.rs` |
| LSP server | Own (JSON-RPC, no external deps) | `lsp.rs` |
| JSON parser | Own (no serde) | `json.rs` |
| C header parser | Own (for FFI bindings) | `cparser.rs` |
| Bare-metal / kernel | Own (multiboot2 headers, flat binary) | `embedded.rs`, `main.rs` |
| Hot reload | Own (native JIT via ELF64) | `hotreload.rs` |
| Autograd | Own (reverse-mode AD) | `autograd.rs` |

### What Fides Depends On (external)

| Dependency | Type | Why | Avoidable? |
|-----------|------|-----|-----------|
| `hashbrown` | Rust crate | HashMap (faster than std) | Yes — could use std HashMap |
| `compact_str` | Rust crate | Memory-efficient strings | Yes — could use std String |
| `smallvec` | Rust crate | Stack-allocated small vectors | Yes — could use Vec |
| `bumpalo` | Rust crate | Bump allocator (compiler internals) | Yes — could write own |
| Rust std library | Language runtime | The compiler is written in Rust | Bootstrap problem — need *some* language to write the compiler in. Self-hosting removes this. |
| `gcc` / `clang` / `cl.exe` | External compiler | **Only for `ccompile` command** — C codegen path | **Yes for native path** — `build` and `emit` don't use it |
| Linux kernel | OS | Runtime makes raw syscalls, needs a kernel to receive them | No — unless we write our own OS (future goal) |
| x86-64 / ARM / RISC-V hardware | Hardware | Codegen targets these ISAs | No — physical reality |

### The Key Distinction

```
Fides "build" path (native ELF64):     Fides "ccompile" path:        Typical language (Rust):
┌────────────┐                         ┌────────────┐                ┌────────────┐
│  .fd source│                         │  .fd source│                │  .rs source│
└──────┬─────┘                         └──────┬─────┘                └──────┬─────┘
       ▼                                     ▼                            ▼
┌────────────┐                         ┌────────────┐                ┌────────────┐
│  Fides     │                         │  Fides     │                │  rustc     │
│  compiler  │                         │  compiler  │                │  (uses     │
│  (own IR,  │                         │  → C code  │                │   LLVM)    │
│   own      │                         └──────┬─────┘                └──────┬─────┘
│   codegen, │                                ▼                            ▼
│   own      │                         ┌────────────┐                ┌────────────┐
│   ELF64)   │                         │  gcc/clang │                │  LLVM      │
└──────┬─────┘                         │  (external)│                │  backend   │
       ▼                               └──────┬─────┘                └──────┬─────┘
┌────────────┐                                ▼                            ▼
│  ELF64     │                         ┌────────────┐                ┌────────────┐
│  executable│                         │  executable│                │  executable│
│  (DIRECT)  │                         │  (via gcc) │                │  (via LLVM)│
└────────────┘                         └────────────┘                └────────────┘

External tools: NONE (0)               External tools: gcc (1)       External tools: LLVM (huge)
```

**The `build` path uses ZERO external tools.** No assembler, no linker, no C
compiler, no libc. The runtime is hand-encoded syscall bytes. This is genuinely
comparable to what IBM did — owning the whole stack — but on mainstream
hardware.

---

## The Strategic Position

```
         Niche / proprietary                    Mainstream / open
         ┌──────────────────┐                   ┌──────────────────┐
         │      z/OS        │                   │                  │
         │  (own stack,     │                   │     Fides        │
         │   own hardware,  │     ──────────►   │  (own stack,     │
         │   closed,        │     the gap       │   mainstream HW, │
         │   banking only)  │     Fides fills   │   open,          │
         │                  │                   │   multi-domain)  │
         ├──────────────────┤                   ├──────────────────┤
         │      AIX         │                   │   Rust/Go/Zig    │
         │  (own stack,     │                   │  (borrowed LLVM/ │
         │   POWER only,    │                   │   GCC backend,   │
         │   closed)        │                   │   mainstream)    │
         └──────────────────┘                   └──────────────────┘
         Own whole stack                       Depend on others' stack
```

Fides's unique position is **top-right**: own the whole stack *and* run on
mainstream hardware *and* open source. Nobody else is there. z/OS is top-left.
Rust/Go/Zig are bottom-right. AIX is bottom-left. That gap is the opportunity.

**Can we match z/OS performance on mainstream hardware?** Yes, for the
workloads where z/OS's advantage comes from software architecture rather than
hardware:

| z/OS advantage | Source | Can Fides match on x86/ARM? |
|---------------|--------|-------------------------------|
| Transaction throughput | Channel I/O (hardware) + transaction monitor (software) | **Partially.** Can't replicate channel I/O. But io_uring is the closest mainstream equivalent. Software side: fully replicable. |
| Reliability / zero downtime | Software architecture (recovery, logging, rollback) | **Yes.** Fides has `txn {}` with rollback, `@crashfree` proofs. Extend to process supervision. |
| Deterministic execution | Software (no rdtsc, no random scheduling) | **Yes.** Fides has `@deterministic` and LogicalTime. Compiler/OS choice, not hardware. |
| Security isolation | Hardware (logical partitions) + software (RACF) | **Partially.** Hardware isolation needs hardware. But `sandbox {}` + capabilities + seccomp/cgroups give strong software isolation. |
| Batch processing efficiency | Software (job scheduler, batch I/O) | **Yes.** Purely software. |
| Memory-mapped I/O throughput | Hardware (channel I/O) | **No** — hardware feature. io_uring gets ~70-80% of the way. |

**Bottom line:** We can match z/OS's *software* advantages on mainstream
hardware. We cannot fully match its *hardware* advantages — but io_uring +
Linux namespaces/cgroups get us 70-80% there, and modern x86/ARM raw CPU speed
is so much higher than mainframe CPUs that total throughput often comes out
ahead anyway.

---

## Four Features to Get z/OS-Class Guarantees on Mainstream Hardware

These are **language and runtime features**, not OS features. They make Fides
the perfect foundation for a future OS built in Fides. Each one is evaluated
with honest benefits and cons.

### 1. io_uring Runtime

**What:** Replace the current raw-syscall runtime with an io_uring-based async
I/O layer. io_uring is Linux's modern async I/O interface (kernel 5.1+, stable
since 5.10). You submit many I/O operations in one syscall via a shared ring
buffer; the kernel processes them asynchronously; you get completions back via
another ring buffer. This is the x86/ARM equivalent of z/OS channel I/O.

**How it connects to what we have:** Fides already has `async`/`await`/`spawn`
(Phase 7.6) and `Future<T>`. The current implementation is eager-evaluation
with a state-machine transform pending. io_uring would be the *runtime* that
backs async I/O operations — the natural completion of the async story.

**Implementation sketch:**
- New runtime functions in `runtime_linux.rs`: `p_uring_init`, `p_uring_submit`,
  `p_uring_complete`, `p_uring_wait` — all hand-encoded syscall bytes
  (`io_uring_setup`, `io_uring_enter`).
- New stdlib module `stdlib/io_uring.fd` with a high-level API.
- Wire `async` functions that do I/O to submit completions to the ring instead
  of blocking syscalls.
- The `Future<T>` poll mechanism becomes "check the completion ring."

**Benefits:**
- **Mainframe-class I/O throughput on commodity Linux.** A single thread can
  handle tens of thousands of concurrent I/O operations with one syscall
  submission batch. Current blocking-syscall runtime does 1 operation = 1
  syscall = 1 context switch.
- **Completes the async story.** `async`/`await`/`spawn` already exist but are
  eager. io_uring gives them a real async runtime that's faster than any
  userspace scheduler (no epoll, no thread pool).
- **Language-specific optimization.** Because we own the runtime, we can batch
  I/O submissions across multiple `await` points in the same scheduler tick —
  something that requires complex framework-level work in Rust (tokio) or Go
  (netpoller). We can do it at the codegen level.
- **No dependency.** io_uring is a Linux kernel feature, not an external
  library. We call it via raw syscalls, same as we do now. Zero new
  dependencies.
- **Future OS foundation.** When we build the Fides OS, the io_uring model
  maps directly to a kernel async I/O subsystem. The language-level API
  becomes the OS API.

**Cons:**
- **Linux-only.** io_uring is Linux-specific. Windows has IOCP (I/O Completion
  Ports) which is similar but different. macOS has kqueue. We'd need per-OS
  runtime implementations. The current raw-syscall runtime is also
  Linux-only, so this doesn't make things worse — but it doesn't help Windows.
- **Kernel version dependency.** io_uring requires Linux 5.1+ (stable 5.10+).
  Some older enterprise Linux distros (RHEL 7, Ubuntu 16.04) don't have it.
  This matters for enterprise adoption — exactly the z/OS market we're
  targeting.
- **Complexity.** The ring buffer setup, completion polling, and cancellation
  logic is significantly more complex than blocking syscalls. More code =
  more bugs = more testing.
- **Security surface.** io_uring has had CVEs. Some security-conscious
  environments (Google, Cloudflare) disable it. We'd need a fallback to
  epoll/poll for those environments.
- **Debugging difficulty.** Async I/O failures are harder to trace than
  blocking I/O failures. The operation is submitted, you get a completion
  later — the stack trace at completion time doesn't show the submission
  context.

**Verdict:** High value, medium effort. This is the single biggest performance
win available. The async infrastructure is already there. The main risk is
portability (Linux-only) and the kernel version requirement. Recommendation:
implement as an optional runtime mode (`--io=uring` vs `--io=blocking`), with
epoll fallback for older kernels and restricted environments.

---

### 2. Capability-Based Process Isolation

**What:** Extend Fides's existing `sandbox {}` blocks and capability system
from type-system-only isolation to *physical* OS-level isolation at runtime.
When a `sandbox {}` block runs, the generated code (or runtime) sets up Linux
seccomp filters, cgroup resource limits, and namespace isolation so the
sandboxed code is physically constrained — not just type-checked.

**How it connects to what we have:** Fides already has:
- Capability system with compile-time grants/denial (`src/typechecker.rs`)
- `sandbox {}` blocks with `in_sandbox` flag propagated through AST → IR → L1 → L2
- Capability-based dead-code elimination: ungranted capability stubs emit `ud2`
  (trap) instead of real code (`runtime_linux.rs`)
- Sandbox-aware optimization: devirtualize and inline more aggressively in
  sandbox blocks (`opt.rs`, I-3C)

The missing piece: at runtime, a sandboxed Fides process should be *physically*
unable to do things it wasn't granted, even if there's a compiler bug.

**Implementation sketch:**
- At the start of a `sandbox {}` block, generated code calls
  `p_sandbox_enter(capability_mask)` which:
  - Installs a seccomp-BPF filter allowing only syscalls in the granted
    capabilities (e.g. `read`/`write` for FS, `socket` for Net)
  - Sets cgroup memory/CPU limits
  - Optionally enters a new mount/PID namespace for full isolation
- At the end of the block, `p_sandbox_exit()` restores the original filter
- New runtime functions in `runtime_linux.rs` (raw syscalls: `seccomp`,
  `prctl`, `unshare`, `cgroup` writes via `write` syscall)
- The type-system capability check is the *fast path* (compile-time, zero
  runtime cost); seccomp is the *defense-in-depth* (catches compiler bugs,
  FFI escapes, raw pointer abuse)

**Benefits:**
- **z/OS-class isolation on commodity Linux.** z/OS uses hardware logical
  partitions (LPARs). We can't do that on x86, but seccomp + namespaces +
  cgroups give software-level isolation that's stronger than typical Linux
  processes and approaches LPAR strength for most threat models.
- **Defense in depth.** The type system catches capability violations at
  compile time (zero cost). seccomp catches them at runtime (catches compiler
  bugs, FFI escapes, `unsafe {}` abuse, raw pointer dereferences to sensitive
  memory). Two layers, complementary.
- **Auditable.** Security auditors can see the seccomp filter and verify
  exactly which syscalls are allowed. This is concrete evidence for compliance
  (SOC 2, PCI DSS, HIPAA) — the same audits z/OS passes because of LPAR.
- **Future OS foundation.** When we build the Fides OS, the capability system
  becomes the kernel's permission model. The language-level `sandbox {}`
  maps directly to a hardware-isolated process. We're designing the OS's
  security model now, at the language level.
- **No external dependency.** seccomp, cgroups, namespaces are Linux kernel
  features accessed via raw syscalls. Same approach as current runtime.

**Cons:**
- **Linux-only (mostly).** seccomp is Linux. cgroups are Linux. Namespaces are
  Linux. Windows has job objects and AppContainers (similar but different API).
  macOS has sandbox profiles (different again). Full cross-platform
  isolation requires per-OS implementations.
- **Performance overhead.** seccomp adds ~1-3% overhead on syscall-heavy
  workloads. Namespace setup is expensive (fork/clone). This should be opt-in
  per sandbox block, not always-on.
- **Privilege requirements.** Creating new namespaces requires `CAP_SYS_ADMIN`
  or user namespaces (which some distros disable). cgroup management requires
  root or delegation. This limits where sandbox isolation can run —
  containers, CI, and servers are fine; unprivileged user machines may not
  support full isolation.
- **Complexity and debugging.** seccomp denials produce `SIGSYS` crashes with
  minimal context. Namespace issues are notoriously hard to debug. This adds
  significant operational complexity.
- **Doesn't help the common case.** Most Fides programs don't use `sandbox {}`.
  This feature benefits the security-critical niche (financial, medical,
  embedded) but adds no value for general-purpose programs.

**Verdict:** High value for the security niche, medium-high effort. This is
what makes Fides credible for banking/medical/embedded use cases — the same
markets z/OS dominates. Recommendation: implement as a runtime option
(`--isolate=sandbox`), with the type-system check always on and seccomp as
opt-in defense-in-depth. Prioritize Linux first; Windows/macOS later.

---

### 3. Transactional Memory at the Codegen Level

**What:** Push the `txn {}` rollback semantics from interpreter-only into the
native codegen. When a `txn {}` block is compiled to native code, the codegen
generates a *write journal*: every memory write inside the txn is recorded
(before/after). If the txn fails (panic, explicit rollback), the journal is
reversed, restoring memory to the pre-txn state. If it succeeds, the journal
is discarded.

**How it connects to what we have:** Fides already has:
- `txn { ... }` blocks with rollback semantics in the interpreter (`interp.rs`)
- `test_txn_rollback.fd` — nested txn, txn with loops
- The architecture doc notes: "Future: interpreter-level memory snapshot for
  rollback" — this extends that to native code

**Implementation sketch:**
- New IR instruction: `JournalWrite(addr, size)` — emitted before every
  `Store` inside a `txn {}` block during lowering
- The codegen translates `JournalWrite` to: save old value to a journal buffer
  (stack-allocated, grows via `p_alloc` if needed), then do the write
- New runtime functions: `p_txn_begin()` (returns journal pointer),
  `p_txn_commit(journal)` (discard), `p_txn_rollback(journal)` (replay
  backwards, restore old values)
- On panic inside a txn: the panic handler calls `p_txn_rollback` before
  unwinding
- Nested txns: stack of journals, rollback the top one only

**Benefits:**
- **z/OS-class transaction semantics at the language level.** z/OS's
  transaction monitor (CICS) does this at the OS/middleware level. Fides does
  it at the *language* level — every program gets it for free, no middleware
  needed. This is strictly better: the compiler knows exactly which memory
  locations are written (no conservative whole-memory snapshot).
- **Compile-time optimization.** Because we own the codegen, we can:
  - Skip journaling for writes to stack-local variables that don't escape the
    txn (they'll be discarded on rollback anyway)
  - Batch journal entries (one journal slot for contiguous writes)
  - Elide the journal entirely for `@crashfree` txn blocks (can't panic =
    can't need rollback)
  - This is impossible in C/C++ — they have no txn concept, so any
    transactional library must conservatively snapshot everything.
- **Financial correctness.** This is the foundation for the banking/money
  stdlib. A failed transfer must not leave money in an inconsistent state.
  Currently this is only guaranteed in the interpreter. Native codegen with
  journaling makes it guaranteed everywhere.
- **Future OS foundation.** When we build the Fides OS, the txn journal
  becomes the kernel's transaction log — same mechanism, just at the OS
  level. The language-level txn maps directly to an OS-level transaction.

**Cons:**
- **Performance overhead.** Every write inside a txn becomes two writes (save
  old + write new) plus journal management. For write-heavy txns, this is
  2-3x slower than non-txn code. This is inherent to any software
  transactional memory — hardware TM (Intel TSX) would help but is unreliable
  and being deprecated.
- **Memory overhead.** The journal grows with the number of writes. For large
  txns, this can be significant. Need careful sizing and potential spill to
  heap.
- **Doesn't handle external side effects.** If a txn writes to a file or
  socket and then rolls back, the file/socket write already happened. The
  journal only covers memory. True transactional I/O requires OS-level
  support (which we'd have in a Fides OS, but not on Linux). This is a
  fundamental limitation — same as z/OS CICS, which only transactions
  in-memory state and uses logs for I/O.
- **Concurrency complexity.** Software transactional memory gets very hard
  with concurrent txns (conflict detection, retry). The current design is
  single-threaded txn. Making it concurrent is a major research-level effort.
  (z/OS solves this with locking and CICS's own scheduler — not STM.)
- **Codegen complexity.** Every backend (x86-64, ARM64, RISC-V, C codegen)
  needs journal support. This is a significant codegen feature, not a small
  addition.

**Verdict:** High value for financial/safety niche, high effort. This is the
feature that makes Fides *correct* for banking, not just *fast*. The
interpreter already has it; native codegen needs it for parity.
Recommendation: implement for the native ELF64 path first (highest value),
C codegen second. Start with single-threaded txn (concurrent STM is a
research project). Use `@crashfree` to elide journals where safe.

---

### 4. Deterministic Scheduling Mode

**What:** A `--deterministic` codegen flag that produces binaries guaranteed
to produce identical output on every run, given identical input. This means:
- No `rdtsc` / `rdtscp` instructions (no timestamp-based randomness)
- No `getrandom` / `read(/dev/urandom)` (no OS randomness)
- No thread spawning, or deterministic thread scheduling (fixed order)
- HashMap/HashSet iteration always sorted (already done in interpreter)
- No floating-point operations that depend on rounding mode (already enforced
  by `@deterministic` annotation, but this makes it global)
- Memory allocation addresses are deterministic (bump allocator with fixed
  base, no ASLR)

**How it connects to what we have:** Fides already has:
- `@deterministic` annotation (F26) — enforced in typechecker, rejects IO,
  randomness, floating-point
- `LogicalTime` stdlib (`stdlib/time.fd`) — clock-free, deterministic date math
- Sorted HashMap output in interpreter (for deterministic test output)
- `@conserve` + `@deterministic` used together in money/banking stdlib

The `--deterministic` flag makes these guarantees *global* (whole program, not
just annotated functions) and *enforced at the codegen level* (not just the
typechecker).

**Implementation sketch:**
- Codegen flag `--deterministic` that:
  - Replaces `p_getrandom` calls with a fixed-seed PRNG (or compile error)
  - Disables ASLR in the ELF64 header (`ET_EXEC` instead of `ET_DYN`)
  - Uses a fixed-base bump allocator (no `mmap` with kernel-chosen address)
  - Forces sorted iteration for all maps/sets (codegen inserts sort before
    iteration if not already sorted)
  - Rejects `spawn` (or schedules spawned tasks in a fixed order)
  - Rejects `rdtsc` intrinsics
- The typechecker in `--deterministic` mode treats the whole program as
  `@deterministic` (every function must be deterministic)

**Benefits:**
- **Reproducible builds and runs.** Given the same input, the same binary
  produces the same output, bit-for-bit, every time. This is gold for:
  - **Testing** — flaky tests become impossible. If a test passes once, it
    passes always.
  - **Auditing** — auditors can replay a financial calculation and verify the
    exact result. This is why z/OS is trusted in banking.
  - **Debugging** — bugs reproduce 100% of the time. No "works on my machine."
  - **Compliance** — regulatory bodies (SEC, FDA, FAA) increasingly require
    reproducible computation. `--deterministic` is proof.
- **z/OS-class determinism on commodity hardware.** z/OS achieves this
  through OS-level controls (no rdtsc, fixed scheduling). Fides achieves it
  at the *compiler* level — no OS cooperation needed. This is strictly more
  portable.
- **Free for `@deterministic` code.** If the whole program is already
  `@deterministic`, the flag is a no-op (just enforcement). Zero overhead.
- **Future OS foundation.** A Fides OS built with `--deterministic` as the
  default would be the first general-purpose OS with guaranteed
  reproducibility. This is a unique selling point.

**Cons:**
- **Performance cost for non-deterministic programs.** Sorted iteration is
  O(n log n) vs O(n) for hash iteration. Fixed-base allocation prevents
  ASLR (security cost — ASLR is a major exploit mitigation). Deterministic
  thread scheduling prevents parallelism. This flag is not free for programs
  that would otherwise be non-deterministic.
- **Security tradeoff.** Disabling ASLR makes buffer overflows easier to
  exploit (though Fides is memory-safe, FFI/raw pointers can still overflow).
  This is a real concern for production deployment.
- **Limits expressiveness.** No randomness, no timestamps, no parallelism.
  Many programs legitimately need these. `--deterministic` is not a
  general-purpose mode — it's for the audit/compliance/testing niche.
- **Floating-point challenges.** Even with `@deterministic` rejecting
  non-deterministic FP, different CPUs (Intel vs AMD vs ARM) can produce
  different FP results due to FMA contraction and differing precision.
  True cross-platform FP determinism requires software FP or strict IEEE 754
  mode (which is slow). This is a known hard problem.
- **Doesn't help most users.** Like sandbox isolation, this is a niche
  feature. General-purpose programs don't need global determinism.

**Verdict:** Medium value, low-medium effort. The infrastructure is mostly
there (`@deterministic` annotation, LogicalTime, sorted maps). The flag just
makes it global and enforces it at codegen. The main cost is the security
tradeoff (ASLR) and FP determinism challenges. Recommendation: implement as
an opt-in flag. Don't disable ASLR by default — use a fixed-base allocator
within the process's ASLR-assigned region (deterministic *relative* addresses,
still ASLR-protected externally). This gets 90% of the benefit with minimal
security cost.

---

## The Road to a Fides OS

**Fides is a language, not an OS.** The OS comes later. But every feature
above is designed to make the language the perfect foundation for that OS.

The sequence:

```
Phase 1 (now): Language foundation
  ✅ Own compiler, own IR, own codegen, own linker, own runtime
  ✅ Safety features (bounds, overflow, linear types, borrow checker)
  ✅ Financial features (money, banking, txn, conservation)
  ✅ Capability system + sandbox
  ✅ Deterministic annotations + LogicalTime
  ✅ Bare-metal + multiboot2 kernel mode (--bare, --boot)

Phase 2 (near): Language completion
  ○ Self-hosting (Fides compiler written in Fides)
  ○ io_uring runtime (feature 1 above)
  ○ Capability-based process isolation (feature 2)
  ○ Transactional memory in native codegen (feature 3)
  ○ --deterministic codegen mode (feature 4)

Phase 3 (future): Fides OS
  ○ Kernel written in Fides (using --bare/--boot infrastructure)
  ○ Memory management (page allocator, virtual memory)
  ○ Scheduler (deterministic mode as default)
  ○ I/O subsystem (io_uring model, designed in Phase 2)
  ○ Capability-based security (designed in Phase 2)
  ○ Transactional filesystem (txn journaling, designed in Phase 2)
  ○ Drivers (written in Fides, capability-restricted)
```

**Why this order matters:** If we build the OS before the language is
self-hosting, we'd write the OS in Rust (or C) and then port it — ending up
like every other OS, with a language mismatch between the OS and its
applications. If we self-host first, the OS is written in the same language it
runs, with the same safety guarantees, same capability model, same
determinism. The OS and the language are one system. This is what IBM did with
z/OS (PL/S + HLASM) and AIX (C + XL compiler) — the OS and its tooling are
co-designed.

**The goal:** A Fides OS that has:
- **Windows convenience** — GUI, drivers work out of the box, applications are
  easy to install and run
- **Linux flexibility and openness** — open source, scriptable, configurable,
  no vendor lock-in
- **z/OS-class reliability** — transactions, determinism, isolation, zero
  downtime
- **Beats both on benchmarks** — because the compiler, runtime, and OS are
  co-designed, we optimize across layers that Windows and Linux can't (they
  have separate compiler teams, kernel teams, and runtime teams with
  incompatible assumptions)

The four features above are the language-level groundwork that makes this OS
possible. Each one maps directly to an OS subsystem:

| Language feature | OS subsystem it becomes |
|-----------------|------------------------|
| io_uring runtime | Kernel async I/O subsystem |
| Capability isolation | Kernel permission/security model |
| Transactional memory | Kernel transaction log / journaling filesystem |
| Deterministic mode | Kernel scheduler + memory allocator design |

---

## What IBM Got Wrong — And How Fides Avoids It

| IBM's mistake | Why it happened | How Fides avoids it |
|--------------|----------------|---------------------|
| **Locked to own hardware** | z/OS only runs on z/Architecture. AIX only runs on POWER. | Fides targets x86-64, ARM64, RISC-V, WASM. Mainstream hardware, no lock-in. |
| **Closed source** | IBM's business model is selling hardware + support contracts. | Fides is MIT-licensed. Open. Anyone can contribute, audit, fork. |
| **Can't run on cloud commodity hardware** | Mainframes are expensive, physical, scarce. | Fides produces native ELF64 on standard Linux. Runs on AWS, GCP, Azure, any x86/ARM box. |
| **No modern language ergonomics** | z/OS devs write COBOL/PL/I. Hard to hire. | Fides has pattern matching, closures, traits, async, borrow checking. Modern. Easy to hire for. |
| **No cross-platform story** | AIX and z/OS don't run on each other's hardware. | Fides: one IR → multiple backends. Same source, any target. |
| **Slow evolution** | Mainframe timescales. Decades between major changes. | Fides is one codebase, iterates in days. |
| **Niche focus became a trap** | z/OS owns banking but can't expand beyond it. | Fides targets multiple niches (financial, embedded, AI/ML, systems) with the same language. |

**The lesson:** Own the stack, but don't own the hardware. Be open, not
closed. Be modern, not legacy. Be multi-target, not single-target. IBM proved
that vertical integration produces superior systems. Fides proves that you can
get the same benefits without the mainframe tax.
