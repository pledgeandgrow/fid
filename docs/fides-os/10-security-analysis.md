# Fides OS — Security Analysis

> What Linux and Windows got wrong at the architectural level, and how Fides
> OS fixes each flaw.

## Why Linux and Windows Are Unsafe — Root Causes

### 1. Memory Safety — C/Assembly Foundation

Both kernels are written in C with inline assembly. No bounds checking, no
type safety at runtime, no ownership tracking.

- **Linux**: ~30M lines of C. Any wild pointer write corrupts kernel memory.
- **Windows**: NT kernel is C/C++. Same class of bugs.

70-80% of kernel CVEs in both Linux and Windows are memory safety bugs:
buffer overflows, use-after-free, double-free, null pointer dereference,
integer overflow leading to buffer issues.

**Fides fix:** Fides is memory-safe by language design. No buffer overflows
(bounds checking), no use-after-free (ownership tracking, regions), no
double-free (region-scoped allocation), no null pointer dereference (option
types), no integer overflow (wrapping is explicit). This eliminates the
entire class of bugs that cause 70-80% of kernel CVEs.

### 2. Monolithic Architecture — Everything in Ring 0

All kernel code shares one address space in ring 0: drivers, filesystems,
network stacks, GPU drivers. A bug in any of these compromises the entire
system.

- **Linux**: ~30,000 kernel-mode drivers. Each is a privilege-escalation
  vector. A flawed USB driver can root the machine.
- **Windows**: Same. Third-party kernel drivers (antivirus, DRM, GPU) are
  notoriously buggy.

**Fides fix:** Stratified architecture. Strata are isolated processes. A
bug in stratum 2 (filesystem) can't access stratum 0 (core) memory. Each
stratum has its own address space. A driver crash is a process crash, not a
kernel compromise.

### 3. Ambient Authority — No Capabilities

Both use ambient authority — once a process has privileges, it has them for
everything.

- **Linux**: `root` (UID 0) bypasses all checks. Capabilities were bolted
  on but are coarse-grained and widely ignored. `sudo` gives full root.
- **Windows**: Administrator token is all-powerful. UAC is a speed bump.

**Fides fix:** Capability-based security. Every resource access requires an
unforgeable capability. No ambient authority. A process can only do what it
holds capabilities for. Serial driver: `caps: [io:0x3F8]` — can only access
COM1 port. Filesystem: `caps: [disk:0]` — can only access disk 0.

### 4. Shared Address Space — No Internal Isolation

All kernel code shares one address space. A bug in subsystem A can corrupt
subsystem B's data. Use-after-free in one driver can corrupt another driver.

**Fides fix:** Per-stratum address space isolation. Each stratum has its own
memory. The MMU blocks cross-stratum access. A buffer overflow in the
network stratum can't corrupt the filesystem stratum.

### 5. No Information Flow Control

Neither OS tracks where data goes. A secret (password, key) loaded into
memory can leak to any process that can read that memory, write it to disk,
or send it over the network.

**Fides fix:** Info-flow control via `@secret` and `@tainted` annotations.
`@secret` data can't reach output sinks (network, display). `@tainted` data
(from network) can't reach `@deterministic` computation. This is
Bell-LaPadula (confidentiality) + Biba (integrity) enforced by the compiler
and kernel together.

### 6. Trusting the Firmware Chain

Both OSes boot through a long chain of unverified firmware: UEFI ->
bootloader -> kernel. Each link is an attack surface.

**Fides fix:** Measured boot with hardware root of trust (TPM). Each stage
verifies the next before executing it. No unsigned code at any stage.

### 7. Speculative Execution Side Channels

Both OSes assumed the CPU was a black box. Spectre/Meltdown/L1TF/MDS/Retbleed
showed that speculative execution leaks kernel memory to user space. Both
scrambled to apply performance-killing mitigations.

**Fides fix:** KPTI-style isolation from day 1 (separate address spaces for
user/kernel). The effect system's `@constant_time` annotation prevents
timing side channels in crypto code.

### 8. The "Compatibility Above All" Trap

Both prioritize backward compatibility over security. Linux can't break
userspace (Linus's rule) — 30 years of legacy. Windows has Win32 ABI back to
Windows 95.

**Fides fix:** Clean slate. No 30-year ABI baggage. Unsafe APIs can be
deprecated and removed. A compatibility stratum provides POSIX/Linux ABI
compatibility as an optional, isolated, capability-scoped layer.

### 9. No Formal Verification

Neither kernel is formally verified. They rely on testing and code review
on multi-million-line codebases.

**Fides fix:** Stratum 0 is ~3,000 lines — small enough for formal
verification (seL4 verified ~8,700 lines). Higher strata use effect
verification (compile-time proof of properties) + capability isolation.

### 10. Privilege Escalation via IPC

Both have IPC mechanisms that don't enforce least privilege. Any process can
call `kill()`, `ptrace()`, `mmap()` into other processes if it has the right
UID. D-Bus is system-wide with no per-message authorization.

**Fides fix:** Capability-based IPC. Each message carries only the authority
the sender actually has. No ambient authority. The receiver can only act on
what the message grants.

### 11. Remote Kernel Access

Both allow remote access to kernel-level functionality: SSH to root, RDP to
Administrator, NFS/SMB in kernel, remote debugging, remote firmware flash.

**Fides fix:** Architectural air-gap. The core is `@no-io` — no network code
exists. No SSH, no RDP, no remote management in the core. Administration
requires physical serial console or BMC. See
[09-core-isolation.md](09-core-isolation.md) for full details.

---

## What Fides OS Does Differently — Summary

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
| No formal verification | Yes | No (core verified) | No (core + effects) |
| Compatibility above security | Yes | No | No (clean slate, optional compat) |

---

## The Fundamental Insight

Most kernel vulnerabilities are memory safety bugs (70-80% per Microsoft's
and Google's own data). Fides eliminates that entire class by design — the
language is memory-safe.

The remaining 20-30% are logic bugs and side channels. Those require
architectural decisions:
- Microkernel/stratified isolation (a driver bug is a process crash)
- Capabilities (no ambient authority, no privilege escalation)
- Info-flow control (secrets can't leak, tainted data can't corrupt)
- Effect verification (compile-time proof of safety properties)
- Architectural air-gap (core unreachable from network)
- Formal verification (provable correctness for the core)

Fides OS makes all of these architectural decisions from the start, rather
than bolting them on after 30 years of legacy accumulation.

## What "Perfect" Means Here

No OS is perfect. Remaining risks that no software can eliminate:
- **Hardware vulnerabilities:** Spectre, Meltdown, Rowhammer (below the OS)
- **Physical side channels:** Power, EM emanation (below the OS)
- **Supply chain:** A malicious compiler could insert backdoors (Trusting Trust)
- **Hardware bugs:** CPU errata, firmware bugs, DMA attacks
- **Human error in annotations:** Marking something @no-io when it does IO

**What Fides OS guarantees:**
- No memory safety bugs (language design)
- No remote kernel access (architectural air-gap)
- No privilege escalation (capabilities)
- No secret leakage (info-flow control)
- No panic in safety-critical code (@medical, @no-panic)
- No unbounded recursion (@no-recursion)
- No dynamic allocation in realtime code (@no-alloc)
- No IO in deterministic code (@deterministic)
- Minimal attack surface (only needed strata compiled in)
- Formally verifiable core (~3,000 lines)

**Perfect for a given threat model.** If your threat model is "remote
attacker over network," a banking terminal profile with no network stratum
is perfect — there's no attack surface. If your threat model is "malicious
hardware," no software can fully protect you, but Fides gets closer than any
existing OS.
