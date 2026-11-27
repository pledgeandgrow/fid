[All modules](index.md)

# smp -- Fides Documentation

## Contents

- [SMP_OFFLINE:](#SMP_OFFLINE:) (const)
- [SMP_STARTING:](#SMP_STARTING:) (const)
- [SMP_ONLINE:](#SMP_ONLINE:) (const)
- [SMP_FAILED:](#SMP_FAILED:) (const)
- [SMP_INIT_DELAY_US:](#SMP_INIT_DELAY_US:) (const)
- [SMP_SIPI_DELAY_US:](#SMP_SIPI_DELAY_US:) (const)
- [SMP_CALLIN_TIMEOUT_US:](#SMP_CALLIN_TIMEOUT_US:) (const)
- [SMP_CALLIN_POLL_US:](#SMP_CALLIN_POLL_US:) (const)
- [SMP_CALLIN_MAGIC:](#SMP_CALLIN_MAGIC:) (const)
- [smp_trampoline_ok](#smp_trampoline_ok) (function)
- [smp_sipi_vector](#smp_sipi_vector) (function)
- [ApParams](#ApParams) (struct)
- [SMP_PARAMS_OFF:](#SMP_PARAMS_OFF:) (const)
- [SMP_PARAMS_SIZE:](#SMP_PARAMS_SIZE:) (const)
- [smp_params_encode](#smp_params_encode) (function)
- [smp_params_decode](#smp_params_decode) (function)
- [SMP_STEP_IPI:](#SMP_STEP_IPI:) (const)
- [SMP_STEP_DELAY:](#SMP_STEP_DELAY:) (const)
- [SMP_STEP_WAIT:](#SMP_STEP_WAIT:) (const)
- [SMP_STEP_SIPI_IF_ABSENT:](#SMP_STEP_SIPI_IF_ABSENT:) (const)
- [SmpStep](#SmpStep) (struct)
- [smp_boot_plan](#smp_boot_plan) (function)
- [SmpOps](#SmpOps) (struct)
- [ApResult](#ApResult) (struct)
- [smp_boot_ap](#smp_boot_ap) (function)
- [CpuMap](#CpuMap) (struct)
- [smp_cpu_map](#smp_cpu_map) (function)
- [smp_cpu_of_apic](#smp_cpu_of_apic) (function)
- [smp_present_count](#smp_present_count) (function)
- [smp_set_state](#smp_set_state) (function)
- [smp_online_count](#smp_online_count) (function)
- [CpuTopo](#CpuTopo) (struct)
- [smp_topology](#smp_topology) (function)
- [smp_shift_for](#smp_shift_for) (function)
- [PCPU_SELF:](#PCPU_SELF:) (const)
- [PCPU_CPU:](#PCPU_CPU:) (const)
- [PCPU_APIC_ID:](#PCPU_APIC_ID:) (const)
- [PCPU_CURRENT:](#PCPU_CURRENT:) (const)
- [PCPU_KSTACK:](#PCPU_KSTACK:) (const)
- [PCPU_TSS:](#PCPU_TSS:) (const)
- [PCPU_PREEMPT:](#PCPU_PREEMPT:) (const)
- [PCPU_IRQ_DEPTH:](#PCPU_IRQ_DEPTH:) (const)
- [PCPU_FIRST_FREE:](#PCPU_FIRST_FREE:) (const)
- [MSR_GS_BASE:](#MSR_GS_BASE:) (const)
- [MSR_KERNEL_GS_BASE:](#MSR_KERNEL_GS_BASE:) (const)
- [PerCpu](#PerCpu) (struct)
- [percpu_new](#percpu_new) (function)
- [percpu_addr](#percpu_addr) (function)
- [percpu_base](#percpu_base) (function)
- [percpu_get](#percpu_get) (function)
- [percpu_set](#percpu_set) (function)
- [percpu_init](#percpu_init) (function)
- [percpu_owner](#percpu_owner) (function)
- [smp_ap_params](#smp_ap_params) (function)

## SMP_OFFLINE:

```fides
pub const SMP_OFFLINE: i64 = 0;
```

*Defined at line 27*

---

## SMP_STARTING:

```fides
pub const SMP_STARTING: i64 = 1;
```

*Defined at line 28*

---

## SMP_ONLINE:

```fides
pub const SMP_ONLINE: i64 = 2;
```

*Defined at line 29*

---

## SMP_FAILED:

```fides
pub const SMP_FAILED: i64 = 3;
```

*Defined at line 30*

---

## SMP_INIT_DELAY_US:

```fides
pub const SMP_INIT_DELAY_US: i64 = 10000;
```

*Defined at line 33*

---

## SMP_SIPI_DELAY_US:

```fides
pub const SMP_SIPI_DELAY_US: i64 = 200;
```

*Defined at line 34*

---

## SMP_CALLIN_TIMEOUT_US:

```fides
pub const SMP_CALLIN_TIMEOUT_US: i64 = 100000;
```

*Defined at line 35*

---

## SMP_CALLIN_POLL_US:

```fides
pub const SMP_CALLIN_POLL_US: i64 = 100;
```

*Defined at line 36*

---

## SMP_CALLIN_MAGIC:

```fides
pub const SMP_CALLIN_MAGIC: i64 = 0x41505550;
```

*Defined at line 39*

---

## smp_trampoline_ok

Whether a trampoline page can be started by a SIPI: 4 KiB aligned,
below 1 MiB, not page 0 (real-mode IVT/BDA) and below the EBDA/VGA
hole at 0x9F000.

```fides
pub fn smp_trampoline_ok(addr: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `bool`

*Defined at line 46*

---

## smp_sipi_vector

SIPI vector of a trampoline page (CS = vector << 8, IP = 0).

```fides
pub fn smp_sipi_vector(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 51*

---

## ApParams

```fides
pub struct ApParams {
```

**Fields:**

| Name | Type |
|------|------|
| `cr3` | `i64` |
| `stack_top` | `i64` |
| `entry` | `i64` |
| `gdt_base` | `i64` |
| `gdt_limit` | `i64` |
| `cpu` | `i64` |
| `apic_id` | `i64` |
| `callin` | `i64` |

*Defined at line 55*

---

## SMP_PARAMS_OFF:

```fides
pub const SMP_PARAMS_OFF: i64 = 0xF00;
```

*Defined at line 71*

---

## SMP_PARAMS_SIZE:

```fides
pub const SMP_PARAMS_SIZE: i64 = 52;
```

*Defined at line 72*

---

## smp_params_encode

```fides
pub fn smp_params_encode(p: ApParams) -> Vec<i64> {
```

**Parameters:**

- `p`: `ApParams`

**Returns:** `Vec<i64>`

*Defined at line 94*

---

## smp_params_decode

```fides
pub fn smp_params_decode(b: Vec<i64>) -> ApParams {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `ApParams`

*Defined at line 109*

---

## SMP_STEP_IPI:

```fides
pub const SMP_STEP_IPI: i64 = 0;
```

*Defined at line 117*

---

## SMP_STEP_DELAY:

```fides
pub const SMP_STEP_DELAY: i64 = 1;
```

*Defined at line 118*

---

## SMP_STEP_WAIT:

```fides
pub const SMP_STEP_WAIT: i64 = 2;
```

*Defined at line 119*

---

## SMP_STEP_SIPI_IF_ABSENT:

```fides
pub const SMP_STEP_SIPI_IF_ABSENT: i64 = 3;  // second SIPI, skipped when callin is seen
```

*Defined at line 121*

---

## SmpStep

```fides
pub struct SmpStep {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `value` | `i64` |

*Defined at line 123*

---

## smp_boot_plan

The INIT-SIPI-SIPI plan for one AP. icr values are for shorthand none
(destination = the AP's APIC id).

```fides
pub fn smp_boot_plan(trampoline: i64) -> Vec<SmpStep> {
```

**Parameters:**

- `trampoline`: `i64`

**Returns:** `Vec<SmpStep>`

*Defined at line 130*

---

## SmpOps

```fides
pub struct SmpOps {
```

**Fields:**

| Name | Type |
|------|------|
| `send_ipi` | `fn(i64, i64) -> bool` |
| `delay_us` | `fn(i64) -> void` |
| `read_flag` | `fn(i64) -> i64` |

*Defined at line 156*

---

## ApResult

```fides
pub struct ApResult {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `ipis` | `i64` |
| `waited_us` | `i64` |

*Defined at line 162*

---

## smp_boot_ap

Run the plan for CPU `cpu` at APIC id `apic_id`. ONLINE when the
callin word holds SMP_CALLIN_MAGIC in time, FAILED otherwise (an IPI
that was not delivered fails at once).

```fides
pub fn smp_boot_ap(ops: &SmpOps, cpu: i64, apic_id: i64, trampoline: i64) -> ApResult {
```

**Parameters:**

- `ops`: `&SmpOps`
- `cpu`: `i64`
- `apic_id`: `i64`
- `trampoline`: `i64`

**Returns:** `ApResult`

*Defined at line 172*

---

## CpuMap

```fides
pub struct CpuMap {
```

**Fields:**

| Name | Type |
|------|------|
| `apic_ids` | `Vec<i64>` |
| `uids` | `Vec<i64>` |
| `present` | `Vec<bool>` |
| `state` | `Vec<i64>` |

*Defined at line 214*

---

## smp_cpu_map

Logical numbering from the MADT: the BSP (bsp_apic_id) is CPU 0, then
every enabled CPU, then online-capable ones (possible, not present).

```fides
pub fn smp_cpu_map(info: &MadtInfo, bsp_apic_id: i64) -> CpuMap {
```

**Parameters:**

- `info`: `&MadtInfo`
- `bsp_apic_id`: `i64`

**Returns:** `CpuMap`

*Defined at line 225*

---

## smp_cpu_of_apic

Logical CPU of an APIC id, -1 when unknown.

```fides
pub fn smp_cpu_of_apic(m: &CpuMap, apic_id: i64) -> i64 {
```

**Parameters:**

- `m`: `&CpuMap`
- `apic_id`: `i64`

**Returns:** `i64`

*Defined at line 262*

---

## smp_present_count

```fides
pub fn smp_present_count(m: &CpuMap) -> i64 {
```

**Parameters:**

- `m`: `&CpuMap`

**Returns:** `i64`

*Defined at line 273*

---

## smp_set_state

```fides
pub fn smp_set_state(m: &mut CpuMap, cpu: i64, st: i64) {
```

**Parameters:**

- `m`: `&mut CpuMap`
- `cpu`: `i64`
- `st`: `i64`

*Defined at line 285*

---

## smp_online_count

```fides
pub fn smp_online_count(m: &CpuMap) -> i64 {
```

**Parameters:**

- `m`: `&CpuMap`

**Returns:** `i64`

*Defined at line 289*

---

## CpuTopo

```fides
pub struct CpuTopo {
```

**Fields:**

| Name | Type |
|------|------|
| `package` | `i64` |
| `core` | `i64` |
| `thread` | `i64` |

*Defined at line 303*

---

## smp_topology

Split an x2APIC id with the CPUID leaf 0xB / 0x1F shifts: smt_shift
(EAX of the SMT level) and core_shift (EAX of the core level).

```fides
pub fn smp_topology(apic_id: i64, smt_shift: i64, core_shift: i64) -> CpuTopo {
```

**Parameters:**

- `apic_id`: `i64`
- `smt_shift`: `i64`
- `core_shift`: `i64`

**Returns:** `CpuTopo`

*Defined at line 311*

---

## smp_shift_for

Bits needed for n items (the shift width CPUID reports for a level).

```fides
pub fn smp_shift_for(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 318*

---

## PCPU_SELF:

```fides
pub const PCPU_SELF: i64 = 0;
```

*Defined at line 329*

---

## PCPU_CPU:

```fides
pub const PCPU_CPU: i64 = 8;
```

*Defined at line 331*

---

## PCPU_APIC_ID:

```fides
pub const PCPU_APIC_ID: i64 = 16;
```

*Defined at line 332*

---

## PCPU_CURRENT:

```fides
pub const PCPU_CURRENT: i64 = 24;
```

*Defined at line 333*

---

## PCPU_KSTACK:

```fides
pub const PCPU_KSTACK: i64 = 32;
```

*Defined at line 334*

---

## PCPU_TSS:

```fides
pub const PCPU_TSS: i64 = 40;
```

*Defined at line 335*

---

## PCPU_PREEMPT:

```fides
pub const PCPU_PREEMPT: i64 = 48;
```

*Defined at line 336*

---

## PCPU_IRQ_DEPTH:

```fides
pub const PCPU_IRQ_DEPTH: i64 = 56;
```

*Defined at line 337*

---

## PCPU_FIRST_FREE:

```fides
pub const PCPU_FIRST_FREE: i64 = 64;
```

*Defined at line 338*

---

## MSR_GS_BASE:

```fides
pub const MSR_GS_BASE: i64 = 0xC0000101;
```

*Defined at line 340*

---

## MSR_KERNEL_GS_BASE:

```fides
pub const MSR_KERNEL_GS_BASE: i64 = 0xC0000102;
```

*Defined at line 341*

---

## PerCpu

```fides
pub struct PerCpu {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `stride` | `i64` |
| `count` | `i64` |
| `words` | `Vec<i64>,  // the area's memory, one element per 8 bytes` |

*Defined at line 343*

---

## percpu_new

Area for `count` CPUs at virtual address base with `size` bytes of
per-CPU data (rounded up to a multiple of 64).

```fides
pub fn percpu_new(base: i64, count: i64, size: i64) -> PerCpu {
```

**Parameters:**

- `base`: `i64`
- `count`: `i64`
- `size`: `i64`

**Returns:** `PerCpu`

*Defined at line 352*

---

## percpu_addr

Address of `field` in cpu's block.

```fides
pub fn percpu_addr(p: &PerCpu, cpu: i64, field: i64) -> i64 {
```

**Parameters:**

- `p`: `&PerCpu`
- `cpu`: `i64`
- `field`: `i64`

**Returns:** `i64`

*Defined at line 363*

---

## percpu_base

The per-CPU base register value (IA32_GS_BASE, TPIDR_EL1, tp).

```fides
pub fn percpu_base(p: &PerCpu, cpu: i64) -> i64 {
```

**Parameters:**

- `p`: `&PerCpu`
- `cpu`: `i64`

**Returns:** `i64`

*Defined at line 368*

---

## percpu_get

```fides
pub fn percpu_get(p: &PerCpu, cpu: i64, field: i64) -> i64 {
```

**Parameters:**

- `p`: `&PerCpu`
- `cpu`: `i64`
- `field`: `i64`

**Returns:** `i64`

*Defined at line 372*

---

## percpu_set

```fides
pub fn percpu_set(p: &mut PerCpu, cpu: i64, field: i64, v: i64) {
```

**Parameters:**

- `p`: `&mut PerCpu`
- `cpu`: `i64`
- `field`: `i64`
- `v`: `i64`

*Defined at line 376*

---

## percpu_init

Fill the fixed fields of every block from a CPU map.

```fides
pub fn percpu_init(p: &mut PerCpu, m: &CpuMap, kstacks_top: i64, kstack_size: i64) {
```

**Parameters:**

- `p`: `&mut PerCpu`
- `m`: `&CpuMap`
- `kstacks_top`: `i64`
- `kstack_size`: `i64`

*Defined at line 381*

---

## percpu_owner

Which CPU owns a per-CPU address, -1 outside the area.

```fides
pub fn percpu_owner(p: &PerCpu, addr: i64) -> i64 {
```

**Parameters:**

- `p`: `&PerCpu`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 393*

---

## smp_ap_params

Parameters for AP `cpu` (its stack from the per-CPU area).

```fides
pub fn smp_ap_params(p: &PerCpu, cpu: i64, cr3: i64, entry: i64, gdt_base: i64, gdt_limit: i64) -> ApParams {
```

**Parameters:**

- `p`: `&PerCpu`
- `cpu`: `i64`
- `cr3`: `i64`
- `entry`: `i64`
- `gdt_base`: `i64`
- `gdt_limit`: `i64`

**Returns:** `ApParams`

*Defined at line 402*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
