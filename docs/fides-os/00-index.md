# Fides OS Documentation

> The Stratified Kernel — a memory-safe, effect-verified, capability-based,
> region-aware, comptime-configured operating system built in Fides.

## Table of Contents

| Document | Description |
|----------|-------------|
| [01-architecture-overview.md](01-architecture-overview.md) | Why Linux and Windows are unsafe, existing kernel types, and the Stratified Kernel concept |
| [02-stratified-kernel.md](02-stratified-kernel.md) | Full design of the Stratified Kernel — strata, effects, capabilities, regions, comptime config |
| [03-editions.md](03-editions.md) | Pre-built editions (banking, mining, AI, research, medical, industrial, embedded, general, custom) |
| [04-pros-and-limitations.md](04-pros-and-limitations.md) | Honest assessment of advantages and remaining trade-offs |
| [05-performance.md](05-performance.md) | Why Fides OS is faster than Linux/Windows — comptime specialization, zero-overhead kernel launches, dedicated cores |
| [06-ai-gpu-training.md](06-ai-gpu-training.md) | AI/ML GPU training optimization — kernel fusion, persistent kernels, direct NVLink, KV cache |
| [07-general-purpose.md](07-general-purpose.md) | Programming, gaming, and general-purpose desktop use on Fides OS |
| [08-llvm-connector.md](08-llvm-connector.md) | LLVM integration strategy — connector, not dependency; compatibility stratum |
| [09-core-isolation.md](09-core-isolation.md) | Local-only core, architectural air-gap, BMC remote management |
| [10-security-analysis.md](10-security-analysis.md) | What Linux and Windows got wrong, and how Fides OS fixes each flaw |
| [11-novel-combinations.md](11-novel-combinations.md) | What exists in OS research, what's genuinely novel, and 12 additional approaches to combine (CHERI, temporal, forensic, Z3, AI-native, etc.) |

## Quick Summary

Fides OS is a **Stratified Kernel** — an operating system built from independent
strata (layers), each with its own effect profile, capability set, and memory
regions. You assemble only the strata your use case requires, producing a binary
with minimal attack surface.

### Key Properties

- **Memory-safe by language design** — no buffer overflows, no use-after-free
- **Effect-verified** — `@no-alloc`, `@realtime`, `@medical`, `@deterministic` enforced at compile time
- **Capability-based** — no ambient authority, every resource access requires a capability
- **Region-aware memory** — `region gpu`, `region dma`, `region realtime` map to hardware pools
- **Comptime-configured** — only needed code compiled in, no runtime config parsing
- **Architecturally air-gapped core** — the kernel core is unreachable from the network
- **Lego-style editions** — pick a pre-built edition or assemble your own

### Editions at a Glance

| Edition | Binary | Boot | Use Case |
|---------|--------|------|----------|
| Banking | 200 KB | 50 ms | ATMs, payment terminals, POS |
| Mining | 500 KB | 100 ms | Crypto mining rigs |
| AI Training | 2 MB | 500 ms | LLM/ML training clusters |
| Research | 2 MB | 500 ms | HPC, scientific computing |
| Medical | 150 KB | 10 ms | Pacemakers, medical devices |
| Industrial | 600 KB | 100 ms | Factory automation, SCADA |
| Embedded | 50 KB | 5 ms | IoT, sensors, edge devices |
| General | 5 MB | 1 sec | Desktop, workstation, server |
| Custom | varies | varies | Build your own |
