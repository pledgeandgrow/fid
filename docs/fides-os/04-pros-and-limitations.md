# Fides OS — Pros and Limitations

> An honest assessment of the Stratified Kernel architecture. No design has
> zero trade-offs — this documents what we gain and what we choose between.

## Pros

### 1. Attack Surface Scales With Need

| Profile | Attack surface | Linux comparison |
|---------|---------------|-----------------|
| Banking terminal | Keyboard + VGA only | 30M lines, all exposed |
| Crypto mining | Network (tainted) + serial | Same 30M lines |
| AI training | Network + disk + GPU | Same 30M lines |
| General purpose | Full but isolated | Full, NOT isolated |

A banking terminal running Fides OS has **zero network code in the binary**.
Not "network code that's disabled" — it's not compiled in. You can't attack
what doesn't exist.

### 2. Memory Safety by Language Design

Eliminates the entire class of bugs that cause 70-80% of kernel CVEs in
Linux and Windows:
- No buffer overflows (bounds checking)
- No use-after-free (ownership tracking, regions)
- No double-free (region-scoped allocation)
- No null pointer dereference (option types)
- No integer overflow leading to buffer issues (wrapping is explicit)

### 3. Compile-Time Security Proofs

The effect system proves properties before the kernel runs:
- `@no-alloc` -> compiler proves it never allocates -> kernel won't map pages
- `@deterministic` -> compiler proves no IO -> no timing side channel
- `@medical` -> compiler proves no panic, no divergence, no recursion
- `@realtime` -> compiler proves bounded execution time
- `@secret` -> compiler proves it can't reach a network sink

### 4. Composable Without Forking

Linux has 30,000+ kernel config options but they all produce the same
architecture. Fides strata are independent compilation units — remove a
stratum and there's no code, no data structures, no capability checks, no
attack surface for it.

### 5. Per-Stratum Isolation Choice

You choose the isolation boundary per stratum, per profile:
- Terminal: inline serial driver (fast, no IPC)
- Server: isolate filesystem (safe, IPC overhead acceptable)
- AI: inline GPU driver (performance critical) but isolate network

### 6. Capability-Based Least Privilege

Every stratum gets exactly the capabilities it needs. No ambient authority.
A compromised filesystem stratum can't access the network because it doesn't
hold a network capability.

### 7. Info-Flow Control

Data carries security labels through the system:
- `@secret` (banking data) -> can't reach network
- `@tainted` (network input) -> can't reach `@deterministic` computation
- `@quantized` (model weights) -> can't be dequantized in `@quantized` stratum

### 8. Region-Aware Memory

Memory pools map to hardware properties:
- `region dma` -> uncached, physically contiguous
- `region realtime` -> wired (no swap, no page faults)
- `region gpu` -> GPU memory pool
- `region kv_cache` -> sliding window eviction

Budget enforced at compile time (typechecker) and runtime (kernel).

### 9. Formal Verification Is Tractable

Stratum 0 is ~3,000 lines. seL4 verified ~8,700 lines in 20 person-years.
Fides stratum 0 could be verified in ~5 person-years. Higher strata use
effect verification + capability isolation.

### 10. Fast Boot

A terminal profile with 5 strata boots in milliseconds. Linux takes 1-5
seconds because it initializes every subsystem "just in case."

### 11. Deterministic Builds

Comptime configuration means the same profile always produces the same
binary. No runtime config files, no `/etc` parsing, no registry.

### 12. Graceful Degradation

- Inlined stratum crash -> kernel panic (rare — effect-verified)
- Isolated stratum crash -> process crash, kernel restarts it

---

## Limitations and Honest Trade-offs

### 1. IPC Overhead for Isolated Strata

Every cross-stratum call for isolated strata requires IPC.

**Numbers (from L4/seL4 benchmarks):**
- Direct function call: ~5 ns
- IPC round-trip: ~100-300 ns
- Linux syscall: ~50-100 ns

**Impact:** I/O-heavy workloads (database servers, web servers) will be
slower than Linux for isolated strata.

**Mitigation:** Batch IPC, shared memory regions, inline critical strata.
The overhead never reaches zero for isolated strata, but comptime
specialization, zero page faults, and zero copies often exceed the IPC cost.

### 2. No Existing Software Ecosystem

Fides OS can't run existing Linux binaries, Windows applications, or
existing drivers. Everything starts from zero.

**Mitigation:** A compatibility stratum could provide POSIX/Linux ABI
compatibility, but it reintroduces attack surface. The LLVM connector (see
[08-llvm-connector.md](08-llvm-connector.md)) provides a path for porting
C/C++/Rust software.

### 3. Driver Availability

Fides OS needs drivers written in Fides. The world has millions of drivers
written in C.

**Mitigation:** Start with narrow hardware (QEMU, specific SBCs, specific
GPUs). Comptime-configured approach means you only need drivers for the
hardware in your profile.

### 4. Single-Language Constraint

All strata must be Fides for effect verification, capability tracking, and
memory safety. C code via FFI bypasses the security model.

**Mitigation:** FFI exists for porting, but it's an escape hatch. The
recommendation is: write new code in Fides, use FFI only for porting.

### 5. Compile-Time Configuration Means No Runtime Flexibility

If you compile a terminal profile (no network), you can't add networking at
runtime. You need to recompile.

**Mitigation:** For fixed-purpose systems (banking, pacemaker, mining), this
is fine. For general-purpose, a "dynamic stratum" could be added but
reintroduces runtime attack surface.

### 6. Formal Verification Only Covers Stratum 0

The effect system proves properties (no-alloc, no-io, no-panic) but doesn't
prove correctness. A `@medical` function is proven to not panic, but not
proven to correctly pace the heart.

**Mitigation:** Critical strata can be formally verified individually.
Full-system verification is only possible for stratum 0.

### 7. Complexity of Effect System

Developers need to understand effects, capabilities, regions, and comptime.

**Mitigation:** Good error messages, IDE support (LSP), documentation. But
the complexity is inherent — reasoning about security at compile time is new
for most developers.

### 8. Multi-Core Scaling Requires Careful Design

Isolated strata need message passing between cores. Shared memory is
possible via regions but must be carefully managed.

**Mitigation:** The multikernel approach (Barrelfish) shows this can work.
Region-aware shared memory can handle many workloads.

### 9. Debugging Is Harder

Cross-stratum debugging requires tracing across process boundaries.

**Mitigation:** Fides has a debugger, post-mortem traces, and hot-reload.
Cross-stratum debugging tools need to be built.

### 10. Performance for General-Purpose Use

Estimated overhead vs Linux for general-purpose workloads:
- CPU-bound (compute): ~0-5% (minimal cross-stratum calls)
- I/O-bound (database, web server): ~10-30% (IPC overhead per I/O)
- Mixed (desktop): ~5-15%

**Mitigation:** Inline performance-critical strata, optimize IPC (batching,
zero-copy), use shared memory regions. The gains from comptime
specialization often exceed the IPC overhead.

### 11. The "Perfect" Claim Is Asymptotic

Remaining risks that no OS can eliminate:
- **Hardware vulnerabilities:** Spectre, Meltdown, Rowhammer
- **Side channels:** Timing, power, EM emanation
- **Supply chain:** A malicious compiler could insert backdoors
  (Ken Thompson's "Trusting Trust")
- **Human error in annotations:** Marking something `@no-io` when it
  actually does IO
- **Hardware bugs:** CPU errata, firmware bugs, DMA attacks

**What "perfect" means here:** Perfect for a given threat model. If your
threat model is "remote attacker over network," a terminal profile with no
network stratum is perfect — there's no attack surface.

---

## Summary Scorecard

| Dimension | Linux | seL4 | Fuchsia | Fides OS |
|-----------|-------|------|---------|----------|
| Memory safety | No (C) | Yes (verified C) | No (C++) | Yes (language) |
| Attack surface scaling | No (fixed) | No (fixed) | No (fixed) | Yes (compile-time) |
| Isolation choice | No | Yes (all isolated) | No | Yes (per-stratum) |
| Capability-based | No | Yes | Yes | Yes |
| Effect verification | No | No | No | Yes |
| Info-flow control | No | No | No | Yes |
| Region-aware memory | No | No | No | Yes |
| Comptime config | No | No | No | Yes |
| Formal verification | No | Yes (core) | No | Yes (core) + effects |
| Hardware support | Yes (millions) | No (minimal) | No (limited) | No (from scratch) |
| App ecosystem | Yes (millions) | No | No | No |
| General-purpose perf | Yes (fast) | No (IPC) | Yes (hybrid) | Trade-off (IPC) |
| Boot time | 1-5 sec | ms | 1-2 sec | ms |
| Debugging | Yes (mature) | Limited | Limited | Needs tooling |

## Bottom Line

**Best architecture for security-critical, fixed-purpose systems** (banking
terminals, pacemakers, crypto rigs, AI training clusters, industrial
controllers). For these use cases, it's strictly better than Linux, Windows,
or seL4.

**For general-purpose desktop use**, it's a trade-off: more secure but
slower, with no existing app ecosystem. The general-purpose profile is
achievable but takes years of ecosystem building.

**Recommendation:** Start with fixed-purpose profiles where the architecture
has zero compromises. Build the ecosystem from there. The Lego concept means
you don't need to build everything before shipping something useful.
