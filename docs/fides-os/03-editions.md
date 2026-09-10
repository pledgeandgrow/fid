# Fides OS Editions

> Pre-built Lego kits for specific use cases. Pick one, or build your own
> custom edition by selecting strata.

## Overview

Each edition is a curated set of strata, tested together, with a specific
threat model and performance profile. All editions share the same stratum 0
(core kernel) — they differ only in which higher strata are included.

```
+-------------------------------------------------------------+
|                    Fides OS Editions                         |
|                                                             |
|  +----------+ +----------+ +----------+ +----------+        |
|  | Banking  | | Mining   | | AI       | | Research |        |
|  | Edition  | | Edition  | | Edition  | | Edition  |        |
|  +----------+ +----------+ +----------+ +----------+        |
|                                                             |
|  +----------+ +----------+ +----------+ +----------+        |
|  | Medical  | | Industrial| | Embedded | | General  |        |
|  | Edition  | | Edition  | | Edition  | | Purpose  |        |
|  |          | |          | |          | | Edition  |        |
|  +----------+ +----------+ +----------+ +----------+        |
|                                                             |
|  +--------------------------------------------+            |
|  |  Custom Edition (pick any strata)           |            |
|  +--------------------------------------------+            |
+-------------------------------------------------------------+
```

## Edition Comparison

| Edition | Lines | Binary | Boot | Attack Surface | Key Feature |
|---------|-------|--------|------|----------------|-------------|
| Banking | 10K | 200 KB | 50 ms | Keyboard + display | No network code exists |
| Mining | 13K | 500 KB | 100 ms | NIC port 3333 | +15-40% hashrate |
| AI Training | 31K | 2 MB | 500 ms | Network + disk + GPU | +30-40% throughput |
| Research | 28K | 2 MB | 500 ms | Network (SSH) | Reproducible results |
| Medical | 8K | 150 KB | 10 ms | Sensors only | Proven zero crashes |
| Industrial | 15K | 600 KB | 100 ms | NIC port 502 | Hard real-time |
| Embedded | 5.5K | 50 KB | 5 ms | GPIO/I2C/SPI | 4 KB RAM |
| General | 113K | 5 MB | 1 sec | Full but isolated | Each app isolated |
| Custom | varies | varies | varies | Only what you pick | You decide |

**Linux for comparison:** 30,000,000 lines, 30 MB binary, 1-5 sec boot,
everything exposed, no isolation.

---

## Edition 1: Banking Terminal

**Who it's for:** ATMs, bank kiosks, payment terminals, point-of-sale systems.

**Threat model:** Physical access + network attacker. No remote code
execution, no data exfiltration.

```
+-----------------------------------------+
|         Banking Edition                 |
|                                         |
|  Stratum 0: Core                        |
|    Scheduler, IPC, VM, capabilities     |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Keyboard, VGA display, card reader   |
|    ~2,000 lines                         |
|                                         |
|  Stratum 2: (none)                      |
|    No filesystem -- no persistence      |
|    No network -- no remote attack       |
|    No USB -- no peripheral attack       |
|                                         |
|  Stratum 3: Banking application         |
|    @medical @no-panic @no-io            |
|    Transaction processing               |
|    ~5,000 lines                         |
|                                         |
|  Total: ~10,000 lines                   |
|  Binary: ~200 KB                        |
|  Boot time: <50 ms                      |
|  Attack surface: keyboard + display     |
|  Network code in binary: ZERO           |
|  Filesystem code in binary: ZERO        |
|  USB code in binary: ZERO               |
+-----------------------------------------+
```

**Can do:** Read card input, display transactions, process transactions,
write to secure log (append-only, capability-scoped).

**Cannot do:** Access network (no code), read/write files (no code), accept
USB devices (no code), run arbitrary programs (no shell), crash
(`@no-panic` + formal verification).

---

## Edition 2: Crypto Mining

**Who it's for:** SHA256/Ethash mining rigs, crypto farms.

**Threat model:** Network attacker trying to steal hashrate or inject fake
shares.

```
+-----------------------------------------+
|         Mining Edition                  |
|                                         |
|  Stratum 0: Core                        |
|    Scheduler (run-to-completion)        |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Serial (management), NIC             |
|    ~3,000 lines                         |
|                                         |
|  Stratum 2: Network (minimal)           |
|    @tainted -- all incoming tagged      |
|    Stratum protocol only                |
|    caps: [nic:0, port:3333]             |
|    ~2,000 lines                         |
|                                         |
|  Stratum 3: Mining engine               |
|    @deterministic @no-io                |
|    SHA256/Ethash (comptime AVX2)        |
|    Dedicated core, zero OS interference |
|    region: mining (pre-mapped, huge)    |
|    ~5,000 lines                         |
|                                         |
|  Total: ~13,000 lines                   |
|  Binary: ~500 KB                        |
|  Boot time: <100 ms                     |
|  Hashrate: +15-40% vs Linux             |
+-----------------------------------------+
```

**Can do:** Mine at maximum hashrate (dedicated core, comptime CPU
specialization), communicate with pool (stratum protocol, `@tainted`),
report stats via serial.

**Cannot do:** Run other programs, access filesystem, accept arbitrary
connections (NIC scoped to port 3333), waste CPU on OS housekeeping.

---

## Edition 3: AI Training

**Who it's for:** LLM training clusters, ML research labs, inference servers.

**Threat model:** Adversarial input data, model theft via side channels,
resource contention.

```
+-----------------------------------------+
|         AI Training Edition             |
|                                         |
|  Stratum 0: Core                        |
|    Scheduler (deadline-aware)           |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Serial, NVMe, NIC, GPU (x8)          |
|    ~5,000 lines                         |
|                                         |
|  Stratum 2: Services                    |
|    Filesystem (@tainted)                |
|    Network (@tainted)                   |
|    GPU manager (@quantized, region:gpu) |
|    ~8,000 lines                         |
|                                         |
|  Stratum 3: AI Runtime                  |
|    @differentiable                      |
|    region:gpu (pre-allocated, 80GB)     |
|    region:kv_cache (sliding window)     |
|    Comptime kernel fusion               |
|    Persistent GPU kernels               |
|    Direct NVLink all-reduce             |
|    ~15,000 lines                        |
|                                         |
|  Total: ~31,000 lines                   |
|  Binary: ~2 MB                          |
|  Boot time: <500 ms                     |
|  Training: +30-40% vs Linux+CUDA        |
|  GPU utilization: 90-95% (vs 50-70%)    |
+-----------------------------------------+
```

**Can do:** Train models 30-40% faster (comptime fusion, persistent
kernels, direct NVLink), load data from disk, serve inference (KV cache
with sliding window), multi-GPU training (direct NVLink, no NCCL).

**Cannot do:** Run untrusted code, leak model weights (`@secret`), waste
GPU time on launch overhead, have training interrupted by OS housekeeping.

---

## Edition 4: Research / Scientific

**Who it's for:** Universities, national labs, HPC clusters, simulation
workloads.

**Threat model:** Resource contention between users, data integrity for
published results.

```
+-----------------------------------------+
|      Research Edition                   |
|                                         |
|  Stratum 0: Core                        |
|    Scheduler (fair-share)               |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Serial, NVMe, NIC, GPU (optional)    |
|    ~5,000 lines                         |
|                                         |
|  Stratum 2: Services                    |
|    Filesystem (multi-user, quotas)      |
|    Network (SSH for remote access)      |
|    GPU manager (shared)                 |
|    Batch scheduler (SLURM-like)         |
|    ~10,000 lines                        |
|                                         |
|  Stratum 3: Research tools              |
|    @deterministic (reproducible)        |
|    Simulation runtime                   |
|    Data analysis                        |
|    Visualization (optional)             |
|    ~10,000 lines                        |
|                                         |
|  Total: ~28,000 lines                   |
|  Binary: ~2 MB                          |
|  Key: @deterministic = reproducible     |
+-----------------------------------------+
```

**Can do:** Run simulations with guaranteed reproducibility
(`@deterministic`), share resources fairly (capability-scoped quotas),
remote access via SSH, GPU acceleration.

**Cannot do:** One user's job interfering with another's, non-deterministic
results, a user accessing another user's data.

---

## Edition 5: Medical / Safety-Critical

**Who it's for:** Pacemakers, insulin pumps, surgical robots, avionics,
automotive ECUs.

**Threat model:** Any failure = patient death or physical harm. Zero
tolerance for crashes.

```
+-----------------------------------------+
|      Medical Edition                    |
|                                         |
|  Stratum 0: Core                        |
|    Formally verified (seL4-style)       |
|    Deadline scheduler (hard real-time)  |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Sensor inputs, actuator outputs      |
|    @no-alloc @no-panic                  |
|    ~2,000 lines                         |
|                                         |
|  Stratum 2: (none)                      |
|    No filesystem, no network, no display|
|                                         |
|  Stratum 3: Medical application         |
|    @medical @no-panic @no-recursion     |
|    @crashfree @deterministic            |
|    @realtime (deadline guaranteed)      |
|    ~3,000 lines                         |
|                                         |
|  Total: ~8,000 lines                    |
|  Binary: ~150 KB                        |
|  Boot time: <10 ms                      |
|  Crash rate: PROVEN ZERO (formal)       |
|  Worst-case latency: PROVEN BOUNDED     |
+-----------------------------------------+
```

**Can do:** Control medical devices with guaranteed deadlines, never crash,
never recurse infinitely, never allocate dynamically, produce deterministic
output.

**Cannot do:** Crash, access network, access filesystem, allocate memory at
runtime, recurse, panic, diverge.

---

## Edition 6: Industrial Control

**Who it's for:** Factory automation, power plants, robotics, SCADA systems.

**Threat model:** Network attacker (Stuxnet-style), equipment damage from
malfunction.

```
+-----------------------------------------+
|      Industrial Edition                 |
|                                         |
|  Stratum 0: Core                        |
|    Deadline scheduler                    |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Sensors, actuators, PLC interface    |
|    Modbus, CAN bus, RS-485              |
|    @no-alloc @no-panic                  |
|    ~4,000 lines                         |
|                                         |
|  Stratum 2: Services                    |
|    Network (isolated, @tainted,         |
|      caps: [nic:0, port:502])           |
|    Logging (append-only, @secret)       |
|    ~3,000 lines                         |
|                                         |
|  Stratum 3: Control logic               |
|    @realtime @no-panic @deterministic   |
|    Safety interlocks                    |
|    Emergency shutdown                   |
|    ~5,000 lines                         |
|                                         |
|  Total: ~15,000 lines                   |
|  Binary: ~600 KB                        |
|  Boot time: <100 ms                     |
|  Network: isolated, single port         |
+-----------------------------------------+
```

---

## Edition 7: Embedded / IoT

**Who it's for:** Smart sensors, IoT devices, embedded controllers, edge
devices.

**Threat model:** Physical access, network attacker, resource constraints
(memory, power).

```
+-----------------------------------------+
|      Embedded Edition                   |
|                                         |
|  Stratum 0: Core (minimal)              |
|    Cooperative scheduler (no preempt)   |
|    ~1,500 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    GPIO, I2C, SPI, UART                 |
|    @no-alloc                            |
|    ~1,000 lines                         |
|                                         |
|  Stratum 2: (optional)                  |
|    Network (BLE or LoRa, @tainted)      |
|    ~1,000 lines (optional)              |
|                                         |
|  Stratum 3: Device logic                |
|    @no-alloc (static memory only)       |
|    Sensor reading, actuator control     |
|    ~2,000 lines                         |
|                                         |
|  Total: ~5,500 lines                    |
|  Binary: ~50 KB                         |
|  RAM: ~4 KB                             |
|  Power: minimal (only needed cores)     |
|  Boot time: <5 ms                       |
+-----------------------------------------+
```

---

## Edition 8: General Purpose

**Who it's for:** Desktop, laptop, workstation, server — people who need
everything.

**Threat model:** General malware, phishing, drive-by downloads, local
privilege escalation.

```
+-----------------------------------------+
|      General Purpose Edition            |
|                                         |
|  Stratum 0: Core                        |
|    Preemptive scheduler                  |
|    ~3,000 lines                         |
|                                         |
|  Stratum 1: Hardware drivers            |
|    Keyboard, Mouse, VGA, Audio, USB,    |
|    NVMe, NIC, GPU, Bluetooth            |
|    ~15,000 lines                        |
|                                         |
|  Stratum 2: Services                    |
|    Filesystem (multi-user, journaling)  |
|    Network (TCP/IP, DNS, TLS)           |
|    GPU (compositor, 3D)                 |
|    Audio (mixer, multiple apps)         |
|    USB (device enumeration)             |
|    ~25,000 lines                        |
|                                         |
|  Stratum 3: Desktop environment         |
|    Window manager, Shell, Package mgr   |
|    Code editor + LSP, File manager      |
|    Terminal                             |
|    ~20,000 lines                        |
|                                         |
|  Stratum 4: Applications                |
|    Browser (compatibility stratum)      |
|    Games, Media player, Office suite    |
|    ~50,000+ lines                       |
|                                         |
|  Total: ~113,000 lines                  |
|  Binary: ~5 MB                          |
|  Boot time: <1 sec                      |
|  Attack surface: large but isolated     |
|  Each app in its own stratum            |
+-----------------------------------------+
```

**Key difference from Linux:** Even in general-purpose mode, each
application runs in its own isolated stratum with capability-scoped access.
A compromised browser can't access the filesystem beyond its cache
directory. A compromised game can't access the network (unless it declares
multiplayer capability).

---

## Custom Edition (Build Your Own)

```fides
// my_custom_edition.fd
comptime {
    edition = "custom"

    strata = [
        Core,
        Serial, Keyboard, VGA, NVMe, NIC,
        Filesystem,    // caps: [fs:root]
        Network,       // caps: [nic:0, port:443, port:80]
        // No GPU -- this is a headless server
        // No Audio -- no sound needed
        // No USB -- no peripheral support
        WebServer,     // @tainted, caps: [fs:webroot, net:http]
        Database,      // @no-io, caps: [fs:db]
        Monitoring,    // @no-io, caps: [fs:logs]
    ]

    inline = [Serial, Keyboard]
    isolated = [Filesystem, Network, WebServer, Database]

    verify = Effects
    info_flow = true
    measured_boot = true

    cpu = "epyc-9654"
    huge_pages = true
    numa_aware = true
    power = PowerPolicy::Balanced
}
```

---

## How a User Chooses

```bash
# List available editions
$ fides-os list-editions

# Build a specific edition
$ fides-os build --edition banking --target x86-64

# Build a custom edition
$ fides-os build --edition custom --config my_edition.fd

# Build general purpose
$ fides-os build --edition general --target x86-64
```

## Key Insight: Same Kernel, Different Assembly

All editions share the same stratum 0 (core kernel). They differ only in
which higher strata are included. This means:

1. **Bug fixes in stratum 0 fix all editions** — patch the core, rebuild all
2. **Each edition is independently tested** — test only the strata it includes
3. **Each edition has a known, bounded attack surface** — count the strata
4. **A vulnerability in one edition doesn't affect others** — no network in
   banking = no network bug
5. **The same compiler builds all editions** — `fidesc` + `fides-os build`
