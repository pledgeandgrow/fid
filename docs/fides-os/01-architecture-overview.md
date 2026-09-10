# Fides OS Architecture Overview

> Why Linux and Windows are unsafe, existing kernel types, and the Stratified
> Kernel concept that Fides OS introduces.

## Why Linux and Windows Are Unsafe

Both Linux and Windows are built on decades-old architectural decisions that
prioritized performance, compatibility, and flexibility over security.

### 1. Memory Safety — C/Assembly Foundation

Both kernels are written in C with inline assembly. No bounds checking, no
type safety at runtime, no ownership tracking.

- **Linux**: ~30M lines of C. Any wild pointer write corrupts kernel memory.
- **Windows**: NT kernel is C/C++. Same class of bugs.

70-80% of kernel CVEs in both Linux and Windows are memory safety bugs
(buffer overflows, use-after-free, double-free, null pointer dereference).

### 2. Monolithic Architecture — Everything in Ring 0

All kernel code shares one address space in ring 0: drivers, filesystems,
network stacks, GPU drivers. A bug in *any* of these compromises the entire
system.

- **Linux**: ~30,000 kernel-mode drivers. Each one is a privilege-escalation
  vector.
- **Windows**: Same. Third-party kernel drivers (antivirus, DRM, GPU) are
  notoriously buggy.

### 3. Ambient Authority — No Capabilities

Both use ambient authority — once a process has privileges, it has them for
everything those privileges grant.

- **Linux**: `root` (UID 0) bypasses all checks. Capabilities were bolted on
  but are coarse-grained and widely ignored.
- **Windows**: Administrator token is all-powerful. UAC is a speed bump.

### 4. Shared Address Space — No Internal Isolation

All kernel code shares one address space. A bug in subsystem A can corrupt
subsystem B's data. Use-after-free in one driver can corrupt another driver.

### 5. No Information Flow Control

Neither OS tracks where data goes. A secret (password, key) loaded into
memory can leak to any process that can read that memory, write it to disk,
or send it over the network.

### 6. Speculative Execution Side Channels

Both OSes assumed the CPU was a black box. Spectre/Meltdown/L1TF/MDS/Retbleed
showed that speculative execution leaks kernel memory to user space. Both
scrambled to apply performance-killing mitigations.

### 7. Compatibility Above All

Both prioritize backward compatibility over security. Linux can't break
userspace (Linus's rule) — 30 years of legacy syscalls and ABI guarantees.
Windows has Win32 ABI compatibility back to Windows 95.

### 8. No Formal Verification

Neither kernel is formally verified. They rely on testing and code review
on multi-million-line codebases.

### 9. Remote Kernel Access

Both allow remote access to kernel-level functionality: SSH to root, RDP to
Administrator, NFS/SMB in kernel, remote debugging, remote firmware flash.
A misconfigured firewall = remote kernel access.

---

## Existing Kernel Types

### Monolithic Kernel

**Examples:** Linux, Windows NT, FreeBSD

Everything runs in one address space in ring 0: scheduler, IPC, filesystem,
drivers, network stack, VM.

```
+-----------------------------------+
|         Ring 0 (kernel)           |
|  +------+ +-----+ +------+       |
|  | VFS  | | Net | | Drvr | ...   |
|  +------+ +-----+ +------+       |
|  +--------------------------+     |
|  |  Scheduler / IPC / VM     |     |
|  +--------------------------+     |
+-----------------------------------+
|      Ring 3 (user space)          |
|  +------+ +------+ +------+      |
|  | App1 | | App2 | | App3 |      |
|  +------+ +------+ +------+      |
```

**Pros:** Fast (no IPC overhead for internal calls), simple mental model.
**Cons:** One bug anywhere = full compromise. 30M lines of attack surface.

### Microkernel

**Examples:** seL4, L4/Fiasco, MINIX 3, QNX

Only the absolute minimum runs in ring 0: IPC, scheduling, address spaces.
Everything else runs as user-space processes.

```
+-----------------------------------+
|     Ring 0 (microkernel)          |
|  +--------------------------+     |
|  |  IPC / Scheduler / VM     |     |
|  +--------------------------+     |
+-----------------------------------+
|      Ring 3 (user space)          |
|  +------+ +------+ +------+      |
|  | FS   | | Net  | | Drvr |      |
|  | Srvr | | Srvr | | Srvr |      |
|  +------+ +------+ +------+      |
|  +------+ +------+ +------+      |
|  | App1 | | App2 | | App3 |      |
|  +------+ +------+ +------+      |
```

**Pros:** Isolation — a driver crash is a process crash, not a kernel panic.
seL4 is formally verified.
**Cons:** IPC overhead for every driver/syscall interaction (~100-300 ns per
round-trip).

### Hybrid Kernel

**Examples:** Windows NT (in practice), XNU (macOS)

Marketed as "best of both worlds." Microkernel in theory, but critical
services run in kernel mode for performance.

**Pros:** Faster than pure microkernel, more isolated than monolithic.
**Cons:** Complexity of both, safety of neither.

### Exokernel

**Examples:** MIT Aegis, XOS

The kernel provides almost nothing — just secure multiplexing of hardware
resources. All abstractions are implemented in user-space libraries.

**Pros:** Maximum flexibility — each app gets exactly the abstraction it
needs.
**Cons:** No shared services. Each app reinvents the wheel.

### Unikernel

**Examples:** MirageOS, IncludeOS, OSv

The kernel and application are compiled into a single binary. No
multi-process, no userspace/kernel boundary.

**Pros:** Tiny attack surface, fast boot, perfect for VMs/cloud.
**Cons:** No isolation between app and OS. One bug = full compromise.

### Library OS (LibOS)

**Examples:** Graphene (SGX), Browsix

Similar to exokernel, but the library provides a full OS API (POSIX) in
user space. Often used inside enclaves.

### Capability-Based Kernel

**Examples:** seL4, Fuchsia/Zircon, CHERI (hardware caps)

Every resource is referenced by an unforgeable capability. No ambient
authority — a process can only access what it holds a capability for.

**Pros:** Principle of least privilege enforced by the kernel.
**Cons:** Capability management is complex. Existing software assumes
ambient authority.

### Multikernel

**Examples:** Barrelfish (ETH Zurich)

Treats the machine as a distributed system. Each CPU core runs its own
kernel instance. Cores communicate via message passing, not shared memory.

**Pros:** No shared-state synchronization overhead. Scales perfectly with
cores.
**Cons:** Message passing overhead. Complex to program.

### Nanokernel

**Examples:** KeyKOS, Coyotos

Even smaller than a microkernel. Only provides hardware abstraction and
capability dispatch. Everything else, including scheduling policy, is in
user space.

**Pros:** Minimal TCB. Most verifiable.
**Cons:** Very hard to build. Almost no real-world deployments.

---

## The Stratified Kernel — What Fides OS Introduces

The Stratified Kernel combines the best properties of existing kernel types
and adds capabilities unique to Fides (effects, regions, comptime, memory
safety).

### Core Concept

The kernel is a set of independent **strata** (layers), each with:
1. **Effect profile** — what effects it's allowed to have
2. **Capability requirements** — what hardware/resources it needs
3. **Region requirements** — what memory pools it needs
4. **Dependencies** — which lower strata it builds on
5. **Verification proof** — formal or effect-based proof of safety

You assemble a **profile** — a comptime-selected set of strata. The compiler
compiles only the selected strata, producing a binary with minimal attack
surface.

### What Makes It Unique

| Feature | seL4 | Fuchsia | Linux | Fides OS |
|---------|------|---------|-------|----------|
| Microkernel isolation | Yes | Yes | No | Yes |
| Capability-based | Yes | Yes | No | Yes |
| Info-flow control | No | No | No | Yes |
| Effect-driven dispatch | No | No | No | Yes |
| Compile-time effect verification | No | No | No | Yes |
| Region-based memory pools | No | No | No | Yes |
| Comptime kernel config | No | No | No | Yes |
| Memory-safe language | No (C) | No (C++) | No (C) | Yes (Fides) |
| Formal verification | Yes (core) | No | No | Possible (small core) |
| Attack surface scales with need | No | No | No | Yes |
| Same binary scales terminal to desktop | No | No | No | Yes |

### The Lego Concept

Each stratum is a Lego brick. You snap together only what you need:

- **Banking terminal**: 5 strata, 200 KB binary, no network code exists
- **Crypto mining**: 3 strata, 500 KB, +15-40% hashrate vs Linux
- **AI training**: 5 strata, 2 MB, +30-40% throughput vs Linux+CUDA
- **General purpose**: 20+ strata, 5 MB, each app isolated

The same kernel core (stratum 0) is shared across all editions. Bug fixes
in stratum 0 fix all editions. Each edition is independently tested. Each
edition has a known, bounded attack surface.

See [02-stratified-kernel.md](02-stratified-kernel.md) for the full design.
