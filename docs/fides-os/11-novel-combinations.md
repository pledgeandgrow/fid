# Fides OS — Novel Combinations and Future Approaches

> What already exists in OS research, what's genuinely novel about the
> Stratified Kernel, and what additional approaches could be combined to
> make Fides OS even more unique.

## What Already Exists

### Components that exist individually

| Component | Exists in | What they have | What they lack |
|-----------|----------|----------------|----------------|
| Formally verified microkernel | seL4 | Correctness proof, capabilities, info-flow proof | C code (not memory-safe), no effects, no regions, no comptime, fixed architecture |
| Memory-safe OS in safe language | Theseus (Rust) | Intralingual design, state spill elimination, runtime composability | No effects, no capabilities, no regions, no editions, no comptime config |
| Component-based kernel assembly | THINK | Assemble kernel from components, flexible binding | No effects, no capabilities, no memory safety, no comptime verification |
| Composable abstractions | null-Kernel | Capability-based, multiple abstraction levels | Research paper only, no effects, no memory safety |
| Certified concurrent kernel | CertiKOS | Compositional verification, Coq proofs | C code, no effects, no capabilities, no regions |
| Compile-time effect tracking | KernRift | Context safety, lock ordering, MMIO types, capability gating, effect tracking | Language only, not a full stratified OS architecture |
| Effect dispatcher in kernel | arkhe-kernel | Effect dispatch, capabilities, principals | Experimental, not stratified, no editions |
| Z3-verified kernel | KeuOS | SMT-verified safety invariants | No effects, no regions, no editions |
| Forensic computing substrate | Phosphoric | Affine capabilities, effect alphabet, per-function effects, manifest-sealed | Single-task edge devices only, no stratified architecture |
| Hardware capability enforcement | CHERI/CheriBSD | Hardware-enforced spatial+temporal safety, capability pointers | C code, no effects, no regions, no comptime, FreeBSD-based |
| Kernel debloating | Tracie/Dice (Linux) | Config-based code removal, 21-35% reduction | Still monolithic, no isolation, post-hoc not architectural |
| AI-native kernel | Composable OS paper | LKMs as AI computation units | Still Linux, no effects, no capabilities |
| Driver-managed GPU memory | NVIDIA CDMM | Driver controls GPU memory, not OS | Driver-level, not kernel-architectural |

### Key references

- **seL4**: First formally verified microkernel. 8,830 lines of C. Proves
  functional correctness, access control, info-flow noninterference, WCET.
  Capabilities for authority. No effects, no regions, no comptime, no
  memory-safe language, no editions.
  Source: https://www.sel4.systems/Research/

- **Theseus**: OS in Rust, intralingual design, eliminates state spill.
  Runtime composability — any component replaceable at runtime. Single
  address space, language-enforced isolation. No effects, no capabilities,
  no regions, no editions, no comptime config.
  Source: https://www.theseus-os.com/

- **KernRift**: Kernel-first systems language with compile-time effect
  tracking, interrupt context safety, lock ordering detection, MMIO
  correctness, capability gating. Closest to Fides's effect-driven approach.
  But: language, not a full stratified OS with editions and regions.
  Source: https://github.com/Pantelis23/KernRift-bootstrap

- **Phosphoric**: Forensic-primacy deterministic computing substrate.
  Affine capabilities, six-element effect alphabet, per-function effect
  declarations, manifest-sealed authority graph. Very close in spirit to
  Fides. But: single-task edge devices only, no stratified architecture.
  Source: https://doi.org/10.5281/zenodo.20024282

- **CHERI/CheriBSD**: Hardware capability pointers. CPU enforces bounds
  checks in silicon. Spatial + temporal memory safety for C. ARM Morello
  and CHERI-RISC-V implementations. No effects, no regions, no comptime.
  Source: https://www.cl.cam.ac.uk/research/security/ctsrd/cheri/

- **Barrelfish**: Multikernel — each core runs its own kernel instance.
  Message passing between cores, no shared kernel state.
  Source: ETH Zurich

- **Oreulius**: Temporal kernel with snapshot/rollback/replay. Capability
  kernel with centralized authority transitions for verification.
  Source: https://github.com/reeveskeefe/Oreulius-Kernel

---

## What NOBODY Has (The Fides OS Novelty)

The individual pieces exist in various research projects. But nobody has
combined them, and several pieces are entirely new:

### 1. Effect-Driven Kernel Dispatch (NEW)

KernRift and Phosphoric have effect tracking in the language, but neither
uses effects to drive kernel-level resource allocation:
- `@no-alloc` -> kernel won't map new pages for the stratum
- `@realtime` -> dedicated CPU core with deadline scheduling
- `@deterministic` -> kernel blocks all IO access
- `@medical` -> kernel enforces no-panic, no-recursion, no-divergence

**This is new.** No existing OS uses compile-time effect analysis to drive
kernel-level resource allocation and isolation.

### 2. Stratified Editions with Compile-Time Selection (NEW)

THINK has component assembly, Theseus has runtime composability, but neither
has pre-built editions (banking, mining, AI, medical) with compile-time
effect verification across stratum boundaries and automatic capability
table generation.

**This is new.**

### 3. Region-Aware Memory Mapping to Hardware Pools (NEW)

Linux has HMM/GPU SVM (runtime), NVIDIA has CDMM (driver-level). Nobody has
compile-time region-to-hardware-pool mapping as an architectural property:
- `region dma` -> uncached, physically contiguous
- `region realtime` -> wired, no swap, no page faults
- `region gpu` -> GPU memory pool
- `region kv_cache` -> sliding window eviction

**This is new.**

### 4. Comptime Kernel Configuration with Effect Verification (NEW)

Linux has kconfig (15,000 options), but it's config flags, not
effect-verified stratum selection with automatic capability table generation
and cross-stratum effect verification.

**This is new.**

### 5. Capability-Flow with Info-Flow Labels (NEW COMBINATION)

seL4 has capabilities + info-flow proof, but they're separate mechanisms.
Fides combines them via `@secret`/`@tainted` annotations enforced by the
compiler and kernel together. Capabilities carry data-flow labels.

**This combination is new.**

### 6. Architectural Air-Gap of the Core (NEW)

seL4 can be configured as a separation kernel, but the core still has IPC
paths. Fides's `@no-io` on the core makes network access architecturally
impossible — the code doesn't exist in the binary.

**This is new.**

### 7. The Full Combination (NEW)

No single system has all of:
1. Memory-safe language
2. Effect verification
3. Capabilities
4. Info-flow control
5. Region-aware memory
6. Comptime configuration
7. Stratified editions
8. Architectural air-gap

**This combination is the novel contribution.**

---

## 12 Approaches That Could Be Combined

### 1. CHERI Hardware Capabilities (Hardware-Enforced)

**What:** CHERI adds capability pointers to the CPU hardware. Every pointer
carries bounds, permissions, and a validity tag. The CPU enforces bounds
checks in silicon.

**How to combine:** Fides OS targets CHERI hardware (ARM Morello,
CHERI-RISC-V). Software capabilities (Fides's capability table) are backed
by hardware capabilities (CHERI pointers). Two layers of enforcement:
- Software: Fides compiler verifies at compile time
- Hardware: CHERI CPU enforces at runtime

**Why valuable:** If the compiler has a bug, hardware catches it. If
hardware has a bug, compiler catches it. Defense in depth. CHERI blocks
35-61% of kernel vulnerabilities in Linux/FreeBSD per research.

**What Fides adds:** Effects, regions, comptime, editions, info-flow labels.
CHERI only does spatial/temporal memory safety — it doesn't know what kind
of computation you're doing.

**Effort:** Medium (requires CHERI hardware or QEMU-CHERI emulator)
**Priority:** Year 3 (when CHERI hardware is more available)

### 2. Temporal Kernel — Snapshot/Rollback/Replay

**What:** The kernel can snapshot state, roll back, and replay execution.
Enables time-travel debugging and forensic analysis.

**How to combine:**

```fides
@temporal
fn critical_operation() -> Result {
    @snapshot("before_transfer")
    transfer_funds(from, to, amount)
    if !verify_transfer() {
        @rollback("before_transfer")
        return Error("Transfer failed")
    }
}
```

**Why valuable:** Banking and medical editions get automatic rollback on
failure. Debugging becomes time-travel. Forensic analysis can replay attacks.

**What Fides adds:** The effect system knows which strata are `@temporal`
(can roll back) vs `@no-rollback` (permanent operations like network sends
or medical actuations). Prevents rolling back irreversible operations.

**Effort:** Medium (~5,000 lines for snapshot/rollback infrastructure)
**Priority:** Year 1-2 (high value for banking/medical editions)

### 3. Forensic Computing (Phosphoric-style)

**What:** The kernel emits typed residual records at every authority
transition. Records form a deterministic chain, independently verifiable
off-device.

**How to combine:**

```fides
// At every capability invocation, kernel emits:
ForensicRecord {
    timestamp: u64,
    stratum: "filesystem",
    capability: "disk:0",
    operation: "read",
    args_hash: Hash([offset, length]),
    result_hash: Hash([data]),
    prev_record_hash: Hash(prev),
}
// Tamper-evident chain, independently verifiable
```

**Why valuable:** Banking terminals prove every transaction. Medical devices
prove every dose. Industrial systems prove every control decision. Provable
accountability — not just logging, but cryptographic proof.

**What Fides adds:** Effect system marks which operations need forensic
records (`@audited`) vs which don't (`@no-audit` for performance-critical
paths). Phosphoric does all operations; Fides can be selective.

**Effort:** Low (~2,000 lines for record emission + chain hashing)
**Priority:** Year 1 (low effort, high value for banking/medical)

### 4. Z3/SMT Verification (KeuOS-style)

**What:** Use Z3 (SMT solver) to verify kernel invariants beyond what the
type system checks.

**How to combine:** Z3 verifies:
- Capability table consistency (no stratum can escalate privileges)
- Region budget arithmetic (allocations never exceed budget)
- Scheduler deadline proofs (all `@realtime` strata meet deadlines)
- IPC protocol safety (no deadlock in capability channels)

**Why valuable:** Effect system proves per-function properties. Z3 proves
system-wide properties (e.g., "scheduler will never miss a deadline"
requires reasoning about all possible interleavings).

**Effort:** Medium (~3,000 lines for Z3 binding + verification conditions)
**Priority:** Year 2

### 5. Intralingual Design — Go Further (Theseus/KernRift-style)

**What:** Shift more OS responsibilities into the compiler. The compiler
enforces OS invariants, not just language invariants.

**How to combine:**

```fides
@irq  // Runs in interrupt context
fn timer_interrupt() -> void {
    // Compiler: can only call @irq-safe functions
    // Compiler: cannot allocate (no malloc in IRQ context)
    // Compiler: cannot block (no waiting in IRQ context)
    scheduler.tick()
}

// Lock ordering: compiler detects deadlock cycles at compile time
@locks_order([MutexA, MutexB])
fn safe_function() -> void {
    lock(MutexA)
    lock(MutexB)  // OK — matches declared order
    // ...
}

// MMIO correctness: hardware register access is typed
@mmio(0x3F8)
val UART_DATA: volatile u8  // Type-safe hardware register
```

**Why valuable:** KernRift already does lock ordering and context safety.
Combining with Fides's effects, regions, and capabilities gives the most
comprehensive compile-time OS verification possible.

**Effort:** Medium (~4,000 lines for IRQ context, lock ordering, MMIO types)
**Priority:** Year 1-2

### 6. Runtime Evolution — Selective Live Updates (Theseus-style)

**What:** Theseus allows any component to be replaced at runtime without
rebooting.

**How to combine:** Fides OS currently requires reflash. But for non-core
strata, verified runtime updates:

```fides
@runtime_updatable
fn web_server_v2() -> void {
    // Same capabilities as v1: [fs:webroot, net:http]
    // Same effects: [@tainted, @no-panic]
    // Signed by trusted developer
    // Hot-swapped via serial console (not network)
}
```

**Rules:**
- Core stratum: NEVER updatable at runtime (requires reflash)
- Isolated strata: updatable IF same/fewer capabilities, passes effect
  verification, signed with trusted key, done via local serial

**Why valuable:** Security patches for non-core strata without rebooting.
A web server can be updated without taking down AI training on the same
machine.

**What Fides adds that Theseus lacks:** Effect system verifies the new
stratum is safe before swap. Theseus allows any replacement; Fides only
allows replacements that pass effect verification with same/fewer
capabilities.

**Effort:** High (~8,000 lines for live linking + verification)
**Priority:** Year 2-3

### 7. Distributed/Multikernel with Regions (Barrelfish-style)

**What:** Each CPU core runs its own kernel instance. Cores communicate via
message passing, not shared kernel state.

**How to combine:**

```fides
comptime {
    match profile {
        "ai_training_64core" => {
            kernel_model = KernelModel::Multikernel
            core_kernels = {
                0: Core,
                1: DataLoader,
                2-9: GPUManager,  // One kernel per GPU
            }
            // No shared kernel state = no lock contention
            // Regions provide shared data without shared kernel state
        }
    }
}
```

**Why valuable:** Eliminates lock contention on many-core systems. Each
core's kernel is independent. Regions provide shared data path without
shared kernel state.

**Effort:** High (~10,000 lines for per-core kernel + message passing)
**Priority:** Year 3-4

### 8. AI-Native Kernel — The Kernel Is Differentiable

**What:** The `@differentiable` effect means the kernel can run autograd
through strata. The kernel becomes part of the ML training pipeline.

**How to combine:**

```fides
@differentiable
fn train_step(batch: region gpu, model: region gpu) -> Loss {
    // Kernel automatically:
    // 1. Records computation graph
    // 2. Allocates gradient memory (region:gradients)
    // 3. Runs backward pass after forward
    // 4. Applies optimizer step
    // All without user writing backward() or step()
}

// The kernel itself is part of the autograd graph
// Scheduler decisions, memory allocation, data movement
// can all be differentiated and optimized
```

**Why valuable:** The kernel becomes part of the ML pipeline, not just a
resource manager. Meta-learning at the OS level — the OS learns optimal
scheduling/allocation for each model.

**Effort:** Very high (research-heavy, ~15,000+ lines)
**Priority:** Year 4-5

### 9. Homomorphic Computation Stratum

**What:** Compute on encrypted data without decrypting it.

**How to combine:** The `@secret` annotation triggers homomorphic computation:

```fides
@secret
fn process_medical_data(data: PatientData) -> Diagnosis {
    // @secret: data must not be exposed
    // Kernel routes to homomorphic computation stratum
    // Computes on encrypted data
    // Result is also @secret — only authorized party can decrypt
}
```

**Why valuable:** Medical and banking data processed without ever being
decrypted in memory. Even if kernel is compromised, attacker only sees
ciphertext.

**Effort:** Very high (requires FHE library, ~20,000+ lines, very slow)
**Priority:** Year 4-5

### 10. Post-Quantum Cryptography by Default

**What:** Use quantum-resistant algorithms (lattice-based, hash-based) as
default for all crypto.

**How to combine:**

```fides
comptime {
    crypto_policy = CryptoPolicy::PostQuantum
    // Kyber for key exchange (NIST-selected)
    // Dilithium for signatures (NIST-selected)
    // AES-256-GCM for symmetric (quantum-resistant with 256-bit key)
    // No RSA, no ECDSA, no DH — all broken by Shor's algorithm
}
```

**Why valuable:** Future-proof against quantum computers. Banking and
medical editions get quantum-resistant security by default.

**Effort:** Low-Medium (~3,000 lines for PQ crypto primitives)
**Priority:** Year 1-2 (NIST standards are finalized)

### 11. Energy-Aware Effect Scheduling

**What:** Use the effect system to drive energy-efficient scheduling on
heterogeneous CPUs (big.LITTLE, P+E cores).

**How to combine:**

```fides
comptime {
    energy_policy = {
        @realtime: MaxPerformance,     // Big cores, max frequency
        @no-alloc: Efficiency,         // Little cores, low frequency
        @deterministic: Balanced,      // Any core, adaptive frequency
        @no-io: LowPower,              // Any core, min frequency, NIC low-power
    }
}
```

**Why valuable:** 30-50% energy savings on heterogeneous CPUs. Effect system
tells scheduler exactly what each stratum needs, so it assigns the most
energy-efficient core that meets the requirement.

**Effort:** Low (~1,500 lines for effect-to-core-type mapping + DVFS)
**Priority:** Year 1 (low effort, high value for embedded/mobile)

### 12. Self-Healing Strata with Effect Proofs

**What:** If a stratum crashes, kernel automatically restarts it with same
capabilities.

**How to combine:**

```fides
@crashfree @no-panic
fn banking_transaction(...) -> Result {
    // If this crashes despite @no-panic:
    // 1. Kernel detects crash
    // 2. Restarts stratum with same capabilities
    // 3. Rolls back to last @snapshot
    // 4. Retries transaction
    // @crashfree proves restart is safe (no irreversible side effects)
}
```

**Why valuable:** Self-healing systems. Crashed web server restarts in
milliseconds. Crashed filesystem stratum restarts with journal. Effect
system proves restart is safe.

**Effort:** Low (~2,000 lines for crash detection + restart + snapshot)
**Priority:** Year 1 (low effort, high value for all editions)

---

## The Combined Architecture

If all 12 approaches are combined, Fides OS becomes:

```
+-------------------------------------------------------------------+
|              Fides OS — Combined Architecture                      |
|                                                                   |
|  Layer 1: Hardware (CHERI capabilities)                           |
|    Hardware-enforced capability pointers                          |
|    Spatial + temporal memory safety in silicon                    |
|                                                                   |
|  Layer 2: Core Kernel (Stratum 0)                                 |
|    @no-io @no-alloc @deterministic                                |
|    Formally verified (Coq) + Z3-verified invariants               |
|    Architectural air-gap (no network code)                        |
|    Temporal snapshots (rollback/replay)                           |
|    Forensic record emission at authority transitions              |
|                                                                   |
|  Layer 3: Strata (1-N)                                            |
|    Effect-driven dispatch (@realtime -> dedicated core)           |
|    Region-aware memory (region:gpu -> GPU memory pool)            |
|    Capability-flow with info-flow labels (@secret -> no network)  |
|    Comptime-configured per edition                                 |
|    Runtime-updatable (non-core, verified, signed)                 |
|    Self-healing (@crashfree -> automatic restart)                 |
|    Energy-aware scheduling (@no-alloc -> efficiency core)         |
|                                                                   |
|  Layer 4: Editions                                                |
|    Banking, Mining, AI, Research, Medical, Industrial,            |
|    Embedded, General, Custom                                       |
|    Each with compile-time effect verification                     |
|    Each with minimal attack surface                                |
|                                                                   |
|  Layer 5: Advanced Features                                       |
|    AI-native (@differentiable kernel in autograd graph)           |
|    Homomorphic computation (@secret -> encrypted computation)     |
|    Post-quantum cryptography (default)                             |
|    Multikernel mode (per-core kernel for many-core)               |
|    Intralingual design (lock ordering, IRQ context in type system)|
+-------------------------------------------------------------------+
```

### Property Matrix

| Property | Achieved by |
|----------|------------|
| Memory safety | Fides language + CHERI hardware (double enforcement) |
| Effect verification | Fides effect system + Z3 (compiler + solver) |
| Capability enforcement | Fides capability table + CHERI pointers (software + hardware) |
| Info-flow control | @secret/@tainted annotations (compiler + kernel) |
| Region-aware memory | Fides region system (compile-time + runtime) |
| Comptime configuration | Fides comptime (editions, profiles, hardware specialization) |
| Architectural air-gap | @no-io on core (compile-time proof) |
| Formal verification | Coq (core correctness) + Z3 (invariants) |
| Temporal rollback | @temporal + @snapshot (kernel primitive) |
| Forensic accountability | Forensic records at authority transitions |
| Self-healing | @crashfree + automatic restart |
| Energy efficiency | Effect-aware scheduling (big.LITTLE assignment) |
| AI-native | @differentiable (kernel in autograd graph) |
| Quantum-resistant | Post-quantum crypto by default |
| Runtime evolution | Verified, signed, capability-constrained live updates |
| Multikernel scaling | Per-core kernel instances for many-core |

**No existing OS or research project has even half of these properties
combined.**

---

## Research Contribution

If built and published, the contribution would be:

> "We present the Stratified Kernel, a novel operating system architecture
> that combines compile-time effect verification, capability-based isolation,
> region-aware memory management, and comptime-configured editions in a
> memory-safe language. Unlike seL4 (which lacks effects, regions, and
> comptime), Theseus (which lacks capabilities, regions, and editions), and
> CHERI (which lacks effects and comptime), the Stratified Kernel is the
> first OS architecture to combine all of these properties in a single
> system. We demonstrate 8 pre-built editions ranging from 150 KB (medical)
> to 5 MB (general purpose), each with formally verified core,
> effect-verified strata, and compile-time attack surface minimization."

This would be a legitimate OSDI/SOSP paper. The combination is novel, the
individual techniques are proven, and the result is an OS that is both more
secure and faster than existing systems for specific workloads.

---

## Implementation Priority

**Don't build all 12 at once.** Build the core Stratified Kernel first,
then add combinations incrementally:

| Priority | Feature | Effort | Value |
|----------|---------|--------|-------|
| Now | Core Stratified Kernel | High | Foundation |
| Year 1 | Forensic records | Low | High (banking/medical) |
| Year 1 | Self-healing strata | Low | High (all editions) |
| Year 1 | Energy-aware scheduling | Low | High (embedded/mobile) |
| Year 1-2 | Post-quantum crypto | Low-Med | High (banking/medical) |
| Year 1-2 | Temporal snapshots | Medium | High (banking/medical) |
| Year 1-2 | Intralingual design (IRQ, locks, MMIO) | Medium | Medium |
| Year 2 | Z3 verification | Medium | Medium |
| Year 2-3 | Runtime evolution | High | Medium |
| Year 3 | CHERI targeting | Medium | High (defense in depth) |
| Year 3-4 | Multikernel mode | High | Medium (many-core) |
| Year 4-5 | AI-native kernel | Very high | High (AI workloads) |
| Year 4-5 | Homomorphic computation | Very high | Medium (niche) |

Each combination is independently valuable. The core Stratified Kernel
alone is already more comprehensive than any existing OS. The combinations
make it even more unique, but they're optional enhancements, not
requirements.
