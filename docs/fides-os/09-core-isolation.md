# Fides OS — Core Isolation: Local-Only, Architectural Air-Gap

> The core (stratum 0) is architecturally unreachable from the network.
> Not "firewalled off" — the code that would create a network path doesn't
> exist in the binary.

## The Problem with Linux and Windows

Both allow remote access to kernel-level functionality:

| Remote path to kernel | Linux | Windows |
|----------------------|------|---------|
| SSH -> root -> kernel | Yes | N/A |
| RDP -> Administrator -> kernel | N/A | Yes |
| Network filesystem (NFS, SMB) | Yes (in kernel) | Yes (in kernel) |
| Remote debugging (kgdb, WinDbg) | Yes | Yes |
| Remote management (SNMP, WinRM) | Yes | Yes |
| Kernel module loading (remote) | Yes (insmod over SSH) | Yes (driver over RDP) |
| Firmware update (remote) | Yes | Yes |

A remote attacker who gets root on Linux can load kernel modules, modify
kernel memory, change parameters, patch the running kernel, and access all
kernel data structures.

**The core is remotely reachable.** Firewall rules are a policy, not an
architecture. A misconfigured firewall = remote kernel access.

---

## The Fides Solution: Architectural Air-Gap

The core is physically and logically incapable of network access. Not "not
allowed" — **incapable**. The code that handles network packets doesn't
exist in the core. The capability to access the NIC is never granted to the
core.

```
+-----------------------------------------------------+
|                    Fides OS                          |
|                                                     |
|  +---------------------------------------------+    |
|  |  Stratum 0: Core                             |    |
|  |                                              |    |
|  |  @no-io  <- CANNOT access any device         |    |
|  |  caps: []  <- NO capabilities at all         |    |
|  |                                              |    |
|  |  Scheduler, IPC, VM, capabilities            |    |
|  |  NO network code exists here                 |    |
|  |  NO NIC access exists here                   |    |
|  |  NO remote management exists here            |    |
|  |  NO remote debugging exists here             |    |
|  |                                              |    |
|  |  Administration: LOCAL ONLY                  |    |
|  |  Via: serial console or physical keyboard    |    |
|  +---------------------------------------------+    |
|          |                                          |
|          | IPC (local only — no network path)       |
|          |                                          |
|  +-------+----------+  +---------------------+     |
|  |  Network Stratum  |  |  Other Strata        |     |
|  |  caps: [nic:0]   |  |  caps: [whatever]    |     |
|  |  @tainted         |  |                      |     |
|  |                   |  |                      |     |
|  |  Handles packets  |  |                      |     |
|  |  CANNOT access    |  |                      |     |
|  |  core memory      |  |                      |     |
|  |  CANNOT modify    |  |                      |     |
|  |  core data        |  |                      |     |
|  |  CANNOT load      |  |                      |     |
|  |  strata           |  |                      |     |
|  +-------------------+  +----------------------+     |
+-----------------------------------------------------+
```

---

## How the Air-Gap Works — 7 Layers

### Layer 1: No Network Code in the Core

The core is compiled with `@no-io`. No NIC driver, no packet handling, no
socket code, no network stack, no remote management, no remote debugging.

**The code doesn't exist. You can't exploit what doesn't exist.**

```fides
@no-io @no-alloc @deterministic
fn core_main() -> void {
    // CANNOT: send/receive packets, open sockets, listen on ports,
    //          accept connections, receive remote commands
    // CAN: schedule strata, manage page tables, dispatch IPC,
    //      enforce capabilities, handle local interrupts
}
```

### Layer 2: No Network Capability for the Core

```fides
comptime {
    core_capabilities = []  // EMPTY — core has no capabilities
    network_stratum_capabilities = [Capability::IoPort(0x3000)]  // NIC
}
```

The capability table is enforced by hardware (IOMMU, MMU). Even if the core
is compromised, it can't access the NIC.

### Layer 3: IPC Is Local-Only

IPC uses shared memory + local interrupts, not network sockets. There is no
network path for IPC. A remote attacker can't send an IPC message.

### Layer 4: Administration Is Physical-Only

```
LOCAL (allowed):
  Serial console (UART, physical)
  Keyboard + VGA (physical)
  Debug port (JTAG, physical)

REMOTE (impossible):
  SSH -> BLOCKED (no SSH in core)
  RDP -> BLOCKED (no RDP in core)
  WinRM -> BLOCKED (no WinRM in core)
  SNMP -> BLOCKED (no SNMP in core)
  Web admin -> BLOCKED (no HTTP in core)
  Remote debug -> BLOCKED (no gdb in core)
  NFS/SMB -> BLOCKED (no NFS/SMB in core)
```

To administer a Fides OS machine:
1. Physically walk to the machine
2. Plug in a serial cable (or use attached keyboard + display)
3. Authenticate locally
4. Perform administration

### Layer 5: No Remote Code Loading

```fides
@no-io @no-alloc
fn core_main() -> void {
    // Strata are compiled in at build time
    // No runtime stratum loading
    // No runtime module loading
    // No dynamic linking
    // No plugin system
    //
    // To add/remove strata:
    // 1. Physically access the machine
    // 2. Connect serial console
    // 3. Rebuild the edition
    // 4. Reflash the kernel binary
    // 5. Reboot
}
```

The only way to change the kernel is to reflash it physically.

### Layer 6: Network Stratum Is Isolated from Core

Even if the network stratum is fully compromised:
- It can send/receive arbitrary packets (bad, but contained)
- It **cannot** access the core (MMU blocks)
- It **cannot** modify the kernel (no capability)
- It **cannot** load new strata (no capability)
- It **cannot** change capabilities (no capability)
- It **cannot** access other strata's memory (MMU blocks)

**Compromised network stack on Linux = full kernel compromise.**
**Compromised network stratum on Fides = network stratum is compromised, nothing else.**

### Layer 7: Hardware Enforcement (IOMMU + MMU)

```
MMU:
  Core page tables: core code + core data only
  Network stratum: network code + NIC registers, NOT core
  Other strata: their own memory, NOT core

IOMMU:
  NIC DMA: can write to network stratum memory only
  CANNOT write to core memory
  CANNOT write to other strata memory

I/O Port Protection:
  Core: no I/O port access
  Network stratum: NIC I/O ports only
  Other strata: their assigned ports only
```

Even a zero-day in the core can't reach the network (no NIC access). Even a
zero-day in the network stratum can't reach the core (MMU blocks it).

---

## What Remote Attackers Can and Cannot Do

### CANNOT Do

| Attack | Linux | Fides OS |
|--------|-------|---------|
| SSH to kernel | Yes | Impossible (no SSH in core) |
| Load kernel module remotely | Yes | Impossible (no runtime loading) |
| Modify kernel config remotely | Yes | Impossible (no remote config) |
| Remote debug kernel | Yes | Impossible (no remote debug) |
| Access kernel memory remotely | Yes | Impossible (no remote access) |
| Flash firmware remotely | Yes | Impossible (physical only) |
| Install backdoor in kernel | Yes | Impossible (no runtime code loading) |
| Pivot from network to kernel | Yes | Impossible (network stratum isolated) |
| Use NFS/SMB to access kernel | Yes | Impossible (no NFS/SMB in core) |

### CAN Do (and how we limit damage)

| Attack | What they reach | How Fides limits damage |
|--------|----------------|----------------------|
| Exploit network stratum | Network stratum only | Isolated — can't reach core |
| Exploit web server stratum | Web server only | caps: [fs:webroot] |
| Exploit database stratum | Database only | caps: [fs:db] |
| Send malicious packets | Network stratum | @tainted — data tagged |
| DoS the network | Network stratum | Other strata unaffected |
| Exploit a ported C app | That app only | Isolated sandbox |

**Worst case:** Attacker fully compromises the network stratum. They can
send/receive packets, read/modify network stratum memory, crash the network
stratum. They **cannot** access the core, other strata, load code, change
capabilities, or persist across reboots.

---

## Local Administration Console

```fides
@no-io  // Can't access NIC
fn admin_console(serial: SerialPort) -> void {
    val auth = serial.read_password()
    if !verify_password(auth) {
        serial.write("Access denied\n")
        return
    }

    loop {
        serial.write("\nFides OS Admin Console\n")
        serial.write("1. Stratum status\n")
        serial.write("2. Capability table\n")
        serial.write("3. View logs\n")
        serial.write("4. Restart stratum\n")
        serial.write("5. Diagnostics\n")
        serial.write("6. Rebuild edition (offline)\n")
        serial.write("7. Reflash kernel\n")
        serial.write("0. Exit\n")

        match serial.read_char() {
            '1' => show_stratum_status(serial),
            '2' => show_capability_table(serial),
            '3' => show_logs(serial),  // @secret data redacted
            '4' => restart_stratum(serial),
            '5' => run_diagnostics(serial),
            '6' => rebuild_edition(serial),
            '7' => reflash_kernel(serial),  // Physical confirmation required
            '0' => break,
            _ => serial.write("Invalid\n"),
        }
    }
}
```

---

## Remote Management via BMC

Some use cases need remote management (data centers, mining farms, AI
clusters). Solution: a separate physical management chip (BMC).

```
+-----------------------------------------------------+
|              Machine Chassis                         |
|                                                     |
|  +--------------+    +----------------------+       |
|  |  BMC Chip    |    |  Main CPU            |       |
|  |  (separate)  |    |                      |       |
|  |  caps:       |    |  +----------------+  |       |
|  |   [serial]   |    |  |  Fides OS Core |  |       |
|  |   [reset]    |    |  |  (no network)  |  |       |
|  |   [power]    |    |  +----------------+  |       |
|  |   [flash]    |    |                      |       |
|  |              |    |  Network stratum     |       |
|  |  Connected:  |    |  (isolated)          |       |
|  |   Serial <-> |<-->| Serial console port  |       |
|  |   Reset  ->  |    |                      |       |
|  |   Power  ->  |    |                      |       |
|  |   Flash  ->  |    |                      |       |
|  +--------------+    +----------------------+       |
|         ^                                           |
|         |                                           |
|    Management network                               |
|    (SEPARATE physical network)                      |
+-----------------------------------------------------+
```

The BMC:
- Is a separate chip with its own CPU and network connection
- Has a separate network cable (management network, not production)
- Can only access the main CPU via serial console (not main network)
- Can reset, power cycle, and reflash the main CPU
- Cannot access the main CPU's memory at runtime (serial only)
- Is itself running Fides OS (embedded edition, hardened)

**How remote management works:**
1. Admin connects to BMC over management network (separate VLAN/cable)
2. Admin authenticates to BMC (password + hardware key)
3. BMC opens serial console to main CPU
4. Admin sees the local admin console (same as being physically present)
5. Admin can view status, restart strata, reflash, etc.

**What the admin CANNOT do:**
- Access the main CPU over the production network
- Access the main CPU's memory directly (BMC only has serial access)
- Inject code without reflashing (serial console is text-only)
- Access without BMC authentication

### Per-Edition Remote Management

| Edition | Remote management | How |
|---------|------------------|-----|
| Banking | None | Physical access only |
| Mining | BMC | Management network -> BMC -> serial -> core |
| AI Training | BMC | Management network -> BMC -> serial -> core |
| Research | BMC + SSH to user stratum | SSH reaches user stratum, NOT core |
| Medical | None | Physical access only |
| Industrial | BMC (isolated management VLAN) | Management -> BMC -> serial -> core |
| Embedded | None | Physical access only |
| General | Local for core, SSH for user stratum | SSH reaches user shell, NOT core |

### The Critical Distinction: User Stratum vs Core

For editions with remote access (research, general purpose), SSH reaches the
**user stratum**, not the core:

```
SSH connection (remote)
    |
Network stratum (receives connection)
    |
SSH stratum (handles SSH, @tainted)
    |
User shell stratum (user's session)

Can do: run user programs, read/write user files, compile, access user caps
CANNOT do: access core, modify kernel config, load strata, change caps
```

SSH gives you a user shell, not kernel access. Administration still requires
physical access (or BMC).

---

## Comparison with Linux and Windows

| Feature | Linux | Windows | Fides OS |
|---------|-------|---------|---------|
| Core reachable over network | Yes (SSH, NFS, SNMP) | Yes (RDP, WinRM, SMB) | Never |
| Remote kernel module loading | Yes (insmod over SSH) | Yes (driver over RDP) | Never |
| Remote kernel config change | Yes (sysctl over SSH) | Yes (registry over RDP) | Never |
| Remote kernel debugging | Yes (kgdb) | Yes (WinDbg) | Never |
| Remote firmware flash | Yes (if enabled) | Yes (if enabled) | Never (physical only) |
| Network stack in kernel | Yes | Yes | No (isolated stratum) |
| Compromised network = kernel | Yes | Yes | No (network stratum isolated) |
| Admin requires physical access | No | No | Yes (or via BMC serial) |

## Summary

The core is architecturally unreachable from the network:

1. **No network code in the core** — `@no-io` proven at compile time
2. **No network capability** — capability table empty, enforced by MMU
3. **No remote management** — no SSH, RDP, WinRM, SNMP in the core
4. **No remote code loading** — strata compiled at build time
5. **No remote kernel access** — administration requires physical serial or BMC
6. **Network stratum isolated** — even if compromised, can't reach core
7. **Hardware enforcement** — MMU and IOMMU block access at silicon level

**This is the fundamental security boundary:** the core is local, the
network is remote, and there is no path between them. Not "no allowed path"
— **no existing path**. The code that would create that path doesn't exist
in the binary.
