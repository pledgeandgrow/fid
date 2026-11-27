[All modules](index.md)

# apic -- Fides Documentation

## Contents

- [LAPIC_ID:](#LAPIC_ID:) (const)
- [LAPIC_VERSION:](#LAPIC_VERSION:) (const)
- [LAPIC_TPR:](#LAPIC_TPR:) (const)
- [LAPIC_PPR:](#LAPIC_PPR:) (const)
- [LAPIC_EOI:](#LAPIC_EOI:) (const)
- [LAPIC_LDR:](#LAPIC_LDR:) (const)
- [LAPIC_DFR:](#LAPIC_DFR:) (const)
- [LAPIC_SVR:](#LAPIC_SVR:) (const)
- [LAPIC_ISR:](#LAPIC_ISR:) (const)
- [LAPIC_TMR:](#LAPIC_TMR:) (const)
- [LAPIC_IRR:](#LAPIC_IRR:) (const)
- [LAPIC_ESR:](#LAPIC_ESR:) (const)
- [LAPIC_LVT_CMCI:](#LAPIC_LVT_CMCI:) (const)
- [LAPIC_ICR_LO:](#LAPIC_ICR_LO:) (const)
- [LAPIC_ICR_HI:](#LAPIC_ICR_HI:) (const)
- [LAPIC_LVT_TIMER:](#LAPIC_LVT_TIMER:) (const)
- [LAPIC_LVT_THERMAL:](#LAPIC_LVT_THERMAL:) (const)
- [LAPIC_LVT_PERF:](#LAPIC_LVT_PERF:) (const)
- [LAPIC_LVT_LINT0:](#LAPIC_LVT_LINT0:) (const)
- [LAPIC_LVT_LINT1:](#LAPIC_LVT_LINT1:) (const)
- [LAPIC_LVT_ERROR:](#LAPIC_LVT_ERROR:) (const)
- [LAPIC_TIMER_INIT:](#LAPIC_TIMER_INIT:) (const)
- [LAPIC_TIMER_CUR:](#LAPIC_TIMER_CUR:) (const)
- [LAPIC_TIMER_DIV:](#LAPIC_TIMER_DIV:) (const)
- [MSR_APIC_BASE:](#MSR_APIC_BASE:) (const)
- [MSR_TSC_DEADLINE:](#MSR_TSC_DEADLINE:) (const)
- [MSR_X2APIC_ICR:](#MSR_X2APIC_ICR:) (const)
- [MSR_X2APIC_SELF_IPI:](#MSR_X2APIC_SELF_IPI:) (const)
- [LAPIC_DEFAULT_BASE:](#LAPIC_DEFAULT_BASE:) (const)
- [IOAPIC_DEFAULT_BASE:](#IOAPIC_DEFAULT_BASE:) (const)
- [APIC_DM_FIXED:](#APIC_DM_FIXED:) (const)
- [APIC_DM_LOWEST:](#APIC_DM_LOWEST:) (const)
- [APIC_DM_SMI:](#APIC_DM_SMI:) (const)
- [APIC_DM_NMI:](#APIC_DM_NMI:) (const)
- [APIC_DM_INIT:](#APIC_DM_INIT:) (const)
- [APIC_DM_STARTUP:](#APIC_DM_STARTUP:) (const)
- [APIC_DM_EXTINT:](#APIC_DM_EXTINT:) (const)
- [APIC_DEST_NONE:](#APIC_DEST_NONE:) (const)
- [APIC_DEST_SELF:](#APIC_DEST_SELF:) (const)
- [APIC_DEST_ALL:](#APIC_DEST_ALL:) (const)
- [APIC_DEST_OTHERS:](#APIC_DEST_OTHERS:) (const)
- [APIC_DELIVERY_PENDING:](#APIC_DELIVERY_PENDING:) (const)
- [APIC_ACTIVE_LOW:](#APIC_ACTIVE_LOW:) (const)
- [APIC_REMOTE_IRR:](#APIC_REMOTE_IRR:) (const)
- [APIC_LEVEL:](#APIC_LEVEL:) (const)
- [APIC_MASKED:](#APIC_MASKED:) (const)
- [APIC_TIMER_ONESHOT:](#APIC_TIMER_ONESHOT:) (const)
- [APIC_TIMER_PERIODIC:](#APIC_TIMER_PERIODIC:) (const)
- [APIC_TIMER_TSC_DEADLINE:](#APIC_TIMER_TSC_DEADLINE:) (const)
- [apic_x2_msr](#apic_x2_msr) (function)
- [ApicBaseMsr](#ApicBaseMsr) (struct)
- [apic_base_decode](#apic_base_decode) (function)
- [apic_base_encode](#apic_base_encode) (function)
- [apic_lvt](#apic_lvt) (function)
- [apic_lvt_timer](#apic_lvt_timer) (function)
- [LvtInfo](#LvtInfo) (struct)
- [apic_lvt_decode](#apic_lvt_decode) (function)
- [apic_timer_div_encode](#apic_timer_div_encode) (function)
- [apic_timer_div_decode](#apic_timer_div_decode) (function)
- [apic_timer_count](#apic_timer_count) (function)
- [apic_timer_calibrate](#apic_timer_calibrate) (function)
- [apic_icr](#apic_icr) (function)
- [apic_icr_init](#apic_icr_init) (function)
- [apic_icr_init_deassert](#apic_icr_init_deassert) (function)
- [apic_icr_sipi](#apic_icr_sipi) (function)
- [apic_icr64](#apic_icr64) (function)
- [ApicBus](#ApicBus) (struct)
- [lapic_read](#lapic_read) (function)
- [lapic_write](#lapic_write) (function)
- [lapic_id](#lapic_id) (function)
- [lapic_max_lvt](#lapic_max_lvt) (function)
- [lapic_eoi](#lapic_eoi) (function)
- [lapic_init](#lapic_init) (function)
- [lapic_timer_start](#lapic_timer_start) (function)
- [lapic_timer_stop](#lapic_timer_stop) (function)
- [lapic_send_ipi](#lapic_send_ipi) (function)
- [lapic_vector_bit](#lapic_vector_bit) (function)
- [IOAPIC_REG_ID:](#IOAPIC_REG_ID:) (const)
- [IOAPIC_REG_VER:](#IOAPIC_REG_VER:) (const)
- [IOAPIC_REG_ARB:](#IOAPIC_REG_ARB:) (const)
- [ioapic_read](#ioapic_read) (function)
- [ioapic_write](#ioapic_write) (function)
- [ioapic_entries](#ioapic_entries) (function)
- [ioapic_version](#ioapic_version) (function)
- [ioapic_redir](#ioapic_redir) (function)
- [ioapic_get_redir](#ioapic_get_redir) (function)
- [ioapic_set_redir](#ioapic_set_redir) (function)
- [ioapic_mask](#ioapic_mask) (function)
- [MadtCpu](#MadtCpu) (struct)
- [MadtIoApic](#MadtIoApic) (struct)
- [MadtOverride](#MadtOverride) (struct)
- [MadtInfo](#MadtInfo) (struct)
- [madt_parse](#madt_parse) (function)
- [IrqRoute](#IrqRoute) (struct)
- [madt_isa_route](#madt_isa_route) (function)
- [madt_ioapic_for](#madt_ioapic_for) (function)
- [apic_isa_redir](#apic_isa_redir) (function)

## LAPIC_ID:

```fides
pub const LAPIC_ID: i64 = 0x20;
```

*Defined at line 28*

---

## LAPIC_VERSION:

```fides
pub const LAPIC_VERSION: i64 = 0x30;
```

*Defined at line 29*

---

## LAPIC_TPR:

```fides
pub const LAPIC_TPR: i64 = 0x80;
```

*Defined at line 30*

---

## LAPIC_PPR:

```fides
pub const LAPIC_PPR: i64 = 0xA0;
```

*Defined at line 31*

---

## LAPIC_EOI:

```fides
pub const LAPIC_EOI: i64 = 0xB0;
```

*Defined at line 32*

---

## LAPIC_LDR:

```fides
pub const LAPIC_LDR: i64 = 0xD0;
```

*Defined at line 33*

---

## LAPIC_DFR:

```fides
pub const LAPIC_DFR: i64 = 0xE0;
```

*Defined at line 34*

---

## LAPIC_SVR:

```fides
pub const LAPIC_SVR: i64 = 0xF0;
```

*Defined at line 35*

---

## LAPIC_ISR:

```fides
pub const LAPIC_ISR: i64 = 0x100;
```

*Defined at line 36*

---

## LAPIC_TMR:

```fides
pub const LAPIC_TMR: i64 = 0x180;
```

*Defined at line 37*

---

## LAPIC_IRR:

```fides
pub const LAPIC_IRR: i64 = 0x200;
```

*Defined at line 38*

---

## LAPIC_ESR:

```fides
pub const LAPIC_ESR: i64 = 0x280;
```

*Defined at line 39*

---

## LAPIC_LVT_CMCI:

```fides
pub const LAPIC_LVT_CMCI: i64 = 0x2F0;
```

*Defined at line 40*

---

## LAPIC_ICR_LO:

```fides
pub const LAPIC_ICR_LO: i64 = 0x300;
```

*Defined at line 41*

---

## LAPIC_ICR_HI:

```fides
pub const LAPIC_ICR_HI: i64 = 0x310;
```

*Defined at line 42*

---

## LAPIC_LVT_TIMER:

```fides
pub const LAPIC_LVT_TIMER: i64 = 0x320;
```

*Defined at line 43*

---

## LAPIC_LVT_THERMAL:

```fides
pub const LAPIC_LVT_THERMAL: i64 = 0x330;
```

*Defined at line 44*

---

## LAPIC_LVT_PERF:

```fides
pub const LAPIC_LVT_PERF: i64 = 0x340;
```

*Defined at line 45*

---

## LAPIC_LVT_LINT0:

```fides
pub const LAPIC_LVT_LINT0: i64 = 0x350;
```

*Defined at line 46*

---

## LAPIC_LVT_LINT1:

```fides
pub const LAPIC_LVT_LINT1: i64 = 0x360;
```

*Defined at line 47*

---

## LAPIC_LVT_ERROR:

```fides
pub const LAPIC_LVT_ERROR: i64 = 0x370;
```

*Defined at line 48*

---

## LAPIC_TIMER_INIT:

```fides
pub const LAPIC_TIMER_INIT: i64 = 0x380;
```

*Defined at line 49*

---

## LAPIC_TIMER_CUR:

```fides
pub const LAPIC_TIMER_CUR: i64 = 0x390;
```

*Defined at line 50*

---

## LAPIC_TIMER_DIV:

```fides
pub const LAPIC_TIMER_DIV: i64 = 0x3E0;
```

*Defined at line 51*

---

## MSR_APIC_BASE:

```fides
pub const MSR_APIC_BASE: i64 = 0x1B;
```

*Defined at line 53*

---

## MSR_TSC_DEADLINE:

```fides
pub const MSR_TSC_DEADLINE: i64 = 0x6E0;
```

*Defined at line 54*

---

## MSR_X2APIC_ICR:

```fides
pub const MSR_X2APIC_ICR: i64 = 0x830;
```

*Defined at line 55*

---

## MSR_X2APIC_SELF_IPI:

```fides
pub const MSR_X2APIC_SELF_IPI: i64 = 0x83F;
```

*Defined at line 56*

---

## LAPIC_DEFAULT_BASE:

```fides
pub const LAPIC_DEFAULT_BASE: i64 = 0xFEE00000;
```

*Defined at line 58*

---

## IOAPIC_DEFAULT_BASE:

```fides
pub const IOAPIC_DEFAULT_BASE: i64 = 0xFEC00000;
```

*Defined at line 59*

---

## APIC_DM_FIXED:

```fides
pub const APIC_DM_FIXED: i64 = 0;
```

*Defined at line 62*

---

## APIC_DM_LOWEST:

```fides
pub const APIC_DM_LOWEST: i64 = 1;
```

*Defined at line 63*

---

## APIC_DM_SMI:

```fides
pub const APIC_DM_SMI: i64 = 2;
```

*Defined at line 64*

---

## APIC_DM_NMI:

```fides
pub const APIC_DM_NMI: i64 = 4;
```

*Defined at line 65*

---

## APIC_DM_INIT:

```fides
pub const APIC_DM_INIT: i64 = 5;
```

*Defined at line 66*

---

## APIC_DM_STARTUP:

```fides
pub const APIC_DM_STARTUP: i64 = 6;
```

*Defined at line 67*

---

## APIC_DM_EXTINT:

```fides
pub const APIC_DM_EXTINT: i64 = 7;
```

*Defined at line 68*

---

## APIC_DEST_NONE:

```fides
pub const APIC_DEST_NONE: i64 = 0;
```

*Defined at line 71*

---

## APIC_DEST_SELF:

```fides
pub const APIC_DEST_SELF: i64 = 1;
```

*Defined at line 72*

---

## APIC_DEST_ALL:

```fides
pub const APIC_DEST_ALL: i64 = 2;
```

*Defined at line 73*

---

## APIC_DEST_OTHERS:

```fides
pub const APIC_DEST_OTHERS: i64 = 3;
```

*Defined at line 74*

---

## APIC_DELIVERY_PENDING:

```fides
pub const APIC_DELIVERY_PENDING: i64 = 0x1000;
```

*Defined at line 77*

---

## APIC_ACTIVE_LOW:

```fides
pub const APIC_ACTIVE_LOW: i64 = 0x2000;
```

*Defined at line 78*

---

## APIC_REMOTE_IRR:

```fides
pub const APIC_REMOTE_IRR: i64 = 0x4000;
```

*Defined at line 79*

---

## APIC_LEVEL:

```fides
pub const APIC_LEVEL: i64 = 0x8000;
```

*Defined at line 80*

---

## APIC_MASKED:

```fides
pub const APIC_MASKED: i64 = 0x10000;
```

*Defined at line 81*

---

## APIC_TIMER_ONESHOT:

```fides
pub const APIC_TIMER_ONESHOT: i64 = 0;
```

*Defined at line 83*

---

## APIC_TIMER_PERIODIC:

```fides
pub const APIC_TIMER_PERIODIC: i64 = 1;
```

*Defined at line 84*

---

## APIC_TIMER_TSC_DEADLINE:

```fides
pub const APIC_TIMER_TSC_DEADLINE: i64 = 2;
```

*Defined at line 85*

---

## apic_x2_msr

x2APIC MSR of an xAPIC register offset.

```fides
pub fn apic_x2_msr(offset: i64) -> i64 {
```

**Parameters:**

- `offset`: `i64`

**Returns:** `i64`

*Defined at line 88*

---

## ApicBaseMsr

```fides
pub struct ApicBaseMsr {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `bsp` | `bool` |
| `x2apic` | `bool` |
| `enabled` | `bool` |

*Defined at line 94*

---

## apic_base_decode

```fides
pub fn apic_base_decode(v: i64) -> ApicBaseMsr {
```

**Parameters:**

- `v`: `i64`

**Returns:** `ApicBaseMsr`

*Defined at line 101*

---

## apic_base_encode

```fides
pub fn apic_base_encode(m: ApicBaseMsr) -> i64 {
```

**Parameters:**

- `m`: `ApicBaseMsr`

**Returns:** `i64`

*Defined at line 107*

---

## apic_lvt

LVT entry: vector, delivery mode, active-low, level, masked.

```fides
pub fn apic_lvt(vector: i64, mode: i64, active_low: bool, level: bool, masked: bool) -> i64 {
```

**Parameters:**

- `vector`: `i64`
- `mode`: `i64`
- `active_low`: `bool`
- `level`: `bool`
- `masked`: `bool`

**Returns:** `i64`

*Defined at line 124*

---

## apic_lvt_timer

LVT timer entry: vector and timer mode (one-shot, periodic, TSC deadline).

```fides
pub fn apic_lvt_timer(vector: i64, timer_mode: i64, masked: bool) -> i64 {
```

**Parameters:**

- `vector`: `i64`
- `timer_mode`: `i64`
- `masked`: `bool`

**Returns:** `i64`

*Defined at line 139*

---

## LvtInfo

```fides
pub struct LvtInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `vector` | `i64` |
| `mode` | `i64` |
| `pending` | `bool` |
| `active_low` | `bool` |
| `level` | `bool` |
| `masked` | `bool` |
| `timer_mode` | `i64` |

*Defined at line 147*

---

## apic_lvt_decode

```fides
pub fn apic_lvt_decode(v: i64) -> LvtInfo {
```

**Parameters:**

- `v`: `i64`

**Returns:** `LvtInfo`

*Defined at line 157*

---

## apic_timer_div_encode

Divide configuration register value for divisor 1, 2, 4, ... 128
(bits 3,1,0); -1 for other divisors.

```fides
pub fn apic_timer_div_encode(div: i64) -> i64 {
```

**Parameters:**

- `div`: `i64`

**Returns:** `i64`

*Defined at line 167*

---

## apic_timer_div_decode

Divisor of a divide configuration value.

```fides
pub fn apic_timer_div_decode(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 188*

---

## apic_timer_count

Initial count for a period of ns nanoseconds at timer input frequency
hz (the bus / core crystal clock) and a divisor.

```fides
pub fn apic_timer_count(hz: i64, div: i64, ns: i64) -> i64 {
```

**Parameters:**

- `hz`: `i64`
- `div`: `i64`
- `ns`: `i64`

**Returns:** `i64`

*Defined at line 198*

---

## apic_timer_calibrate

Timer input frequency from a calibration: the counter fell by `ticks`
with divisor div during ref_ns nanoseconds of a reference timer.

```fides
pub fn apic_timer_calibrate(ticks: i64, div: i64, ref_ns: i64) -> i64 {
```

**Parameters:**

- `ticks`: `i64`
- `div`: `i64`
- `ref_ns`: `i64`

**Returns:** `i64`

*Defined at line 204*

---

## apic_icr

Low 32 bits of an ICR command (edge triggered, level asserted).

```fides
pub fn apic_icr(vector: i64, mode: i64, logical: bool, shorthand: i64) -> i64 {
```

**Parameters:**

- `vector`: `i64`
- `mode`: `i64`
- `logical`: `bool`
- `shorthand`: `i64`

**Returns:** `i64`

*Defined at line 211*

---

## apic_icr_init

INIT IPI (to one APIC with shorthand none, or to all others).

```fides
pub fn apic_icr_init(shorthand: i64) -> i64 {
```

**Parameters:**

- `shorthand`: `i64`

**Returns:** `i64`

*Defined at line 221*

---

## apic_icr_init_deassert

INIT level de-assert (needed by 82489DX-era APICs, ignored by later ones).

```fides
pub fn apic_icr_init_deassert() -> i64 {
```

**Returns:** `i64`

*Defined at line 226*

---

## apic_icr_sipi

STARTUP IPI: the AP starts in real mode at trampoline (4 KiB aligned,
below 1 MiB): vector = trampoline >> 12.

```fides
pub fn apic_icr_sipi(trampoline: i64, shorthand: i64) -> i64 {
```

**Parameters:**

- `trampoline`: `i64`
- `shorthand`: `i64`

**Returns:** `i64`

*Defined at line 232*

---

## apic_icr64

```fides
pub fn apic_icr64(lo: i64, dest: i64, x2: bool) -> i64 {
```

**Parameters:**

- `lo`: `i64`
- `dest`: `i64`
- `x2`: `bool`

**Returns:** `i64`

*Defined at line 239*

---

## ApicBus

```fides
pub struct ApicBus {
```

**Fields:**

| Name | Type |
|------|------|
| `x2` | `bool` |
| `rd` | `fn(i64) -> i64` |
| `wr` | `fn(i64, i64) -> void` |

*Defined at line 248*

---

## lapic_read

```fides
pub fn lapic_read(b: &ApicBus, offset: i64) -> i64 {
```

**Parameters:**

- `b`: `&ApicBus`
- `offset`: `i64`

**Returns:** `i64`

*Defined at line 254*

---

## lapic_write

```fides
pub fn lapic_write(b: &ApicBus, offset: i64, v: i64) {
```

**Parameters:**

- `b`: `&ApicBus`
- `offset`: `i64`
- `v`: `i64`

*Defined at line 262*

---

## lapic_id

APIC id of this CPU: bits 31:24 in xAPIC mode, the whole register in x2APIC.

```fides
pub fn lapic_id(b: &ApicBus) -> i64 {
```

**Parameters:**

- `b`: `&ApicBus`

**Returns:** `i64`

*Defined at line 272*

---

## lapic_max_lvt

Version register: version (bits 7:0) and number of LVT entries.

```fides
pub fn lapic_max_lvt(b: &ApicBus) -> i64 {
```

**Parameters:**

- `b`: `&ApicBus`

**Returns:** `i64`

*Defined at line 281*

---

## lapic_eoi

```fides
pub fn lapic_eoi(b: &ApicBus) {
```

**Parameters:**

- `b`: `&ApicBus`

*Defined at line 285*

---

## lapic_init

Bring up the Local APIC: flat logical mode (xAPIC), every LVT masked,
errors cleared, TPR 0, then software-enable with the spurious vector.

```fides
pub fn lapic_init(b: &ApicBus, spurious: i64, error_vector: i64) {
```

**Parameters:**

- `b`: `&ApicBus`
- `spurious`: `i64`
- `error_vector`: `i64`

*Defined at line 291*

---

## lapic_timer_start

Start the timer: divisor, LVT (vector, mode), then the initial count
(writing the count starts it).

```fides
pub fn lapic_timer_start(b: &ApicBus, vector: i64, mode: i64, div: i64, count: i64) {
```

**Parameters:**

- `b`: `&ApicBus`
- `vector`: `i64`
- `mode`: `i64`
- `div`: `i64`
- `count`: `i64`

*Defined at line 309*

---

## lapic_timer_stop

```fides
pub fn lapic_timer_stop(b: &ApicBus) {
```

**Parameters:**

- `b`: `&ApicBus`

*Defined at line 315*

---

## lapic_send_ipi

Send an IPI. xAPIC: destination into ICR_HI, command into ICR_LO,
then wait (at most `spins` reads) for the delivery status to clear.
x2APIC: one write of the 64-bit ICR MSR. Returns false on a timeout.

```fides
pub fn lapic_send_ipi(b: &ApicBus, dest: i64, lo: i64, spins: i64) -> bool {
```

**Parameters:**

- `b`: `&ApicBus`
- `dest`: `i64`
- `lo`: `i64`
- `spins`: `i64`

**Returns:** `bool`

*Defined at line 323*

---

## lapic_vector_bit

Bit `vector` of the 256-bit ISR / IRR / TMR array at `base`.

```fides
pub fn lapic_vector_bit(b: &ApicBus, base: i64, vector: i64) -> bool {
```

**Parameters:**

- `b`: `&ApicBus`
- `base`: `i64`
- `vector`: `i64`

**Returns:** `bool`

*Defined at line 342*

---

## IOAPIC_REG_ID:

```fides
pub const IOAPIC_REG_ID: i64 = 0x00;
```

*Defined at line 349*

---

## IOAPIC_REG_VER:

```fides
pub const IOAPIC_REG_VER: i64 = 0x01;
```

*Defined at line 350*

---

## IOAPIC_REG_ARB:

```fides
pub const IOAPIC_REG_ARB: i64 = 0x02;
```

*Defined at line 351*

---

## ioapic_read

```fides
pub fn ioapic_read(io: &ApicBus, reg: i64) -> i64 {
```

**Parameters:**

- `io`: `&ApicBus`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 353*

---

## ioapic_write

```fides
pub fn ioapic_write(io: &ApicBus, reg: i64, v: i64) {
```

**Parameters:**

- `io`: `&ApicBus`
- `reg`: `i64`
- `v`: `i64`

*Defined at line 360*

---

## ioapic_entries

Number of redirection entries (max redirection entry + 1).

```fides
pub fn ioapic_entries(io: &ApicBus) -> i64 {
```

**Parameters:**

- `io`: `&ApicBus`

**Returns:** `i64`

*Defined at line 367*

---

## ioapic_version

```fides
pub fn ioapic_version(io: &ApicBus) -> i64 {
```

**Parameters:**

- `io`: `&ApicBus`

**Returns:** `i64`

*Defined at line 371*

---

## ioapic_redir

```fides
pub fn ioapic_redir(vector: i64, mode: i64, logical: bool, active_low: bool, level: bool, masked: bool, dest: i64) -> i64 {
```

**Parameters:**

- `vector`: `i64`
- `mode`: `i64`
- `logical`: `bool`
- `active_low`: `bool`
- `level`: `bool`
- `masked`: `bool`
- `dest`: `i64`

**Returns:** `i64`

*Defined at line 378*

---

## ioapic_get_redir

```fides
pub fn ioapic_get_redir(io: &ApicBus, n: i64) -> i64 {
```

**Parameters:**

- `io`: `&ApicBus`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 387*

---

## ioapic_set_redir

```fides
pub fn ioapic_set_redir(io: &ApicBus, n: i64, e: i64) {
```

**Parameters:**

- `io`: `&ApicBus`
- `n`: `i64`
- `e`: `i64`

*Defined at line 396*

---

## ioapic_mask

```fides
pub fn ioapic_mask(io: &ApicBus, n: i64, masked: bool) {
```

**Parameters:**

- `io`: `&ApicBus`
- `n`: `i64`
- `masked`: `bool`

*Defined at line 402*

---

## MadtCpu

```fides
pub struct MadtCpu {
```

**Fields:**

| Name | Type |
|------|------|
| `uid` | `i64` |
| `apic_id` | `i64` |
| `enabled` | `bool` |
| `online_capable` | `bool` |
| `x2` | `bool` |

*Defined at line 414*

---

## MadtIoApic

```fides
pub struct MadtIoApic {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `addr` | `i64` |
| `gsi_base` | `i64` |

*Defined at line 422*

---

## MadtOverride

```fides
pub struct MadtOverride {
```

**Fields:**

| Name | Type |
|------|------|
| `source` | `i64` |
| `gsi` | `i64` |
| `flags` | `i64` |

*Defined at line 428*

---

## MadtInfo

```fides
pub struct MadtInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `lapic_addr` | `i64` |
| `pcat_compat` | `bool` |
| `cpus` | `Vec<MadtCpu>` |
| `ioapics` | `Vec<MadtIoApic>` |
| `overrides` | `Vec<MadtOverride>` |
| `nmi_lint` | `i64,  // LINT pin of the NMI entry for all CPUs, -1 none` |

*Defined at line 434*

---

## madt_parse

```fides
pub fn madt_parse(m: Vec<i64>) -> MadtInfo {
```

**Parameters:**

- `m`: `Vec<i64>`

**Returns:** `MadtInfo`

*Defined at line 446*

---

## IrqRoute

```fides
pub struct IrqRoute {
```

**Fields:**

| Name | Type |
|------|------|
| `gsi` | `i64` |
| `active_low` | `bool` |
| `level` | `bool` |

*Defined at line 492*

---

## madt_isa_route

Where ISA IRQ irq arrives: its override, or identity, edge, active high.

```fides
pub fn madt_isa_route(info: &MadtInfo, irq: i64) -> IrqRoute {
```

**Parameters:**

- `info`: `&MadtInfo`
- `irq`: `i64`

**Returns:** `IrqRoute`

*Defined at line 499*

---

## madt_ioapic_for

Index of the IO APIC serving gsi, given each one's entry count, or -1.

```fides
pub fn madt_ioapic_for(info: &MadtInfo, gsi: i64, counts: Vec<i64>) -> i64 {
```

**Parameters:**

- `info`: `&MadtInfo`
- `gsi`: `i64`
- `counts`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 518*

---

## apic_isa_redir

Redirection entry for an ISA IRQ routed through the MADT.

```fides
pub fn apic_isa_redir(info: &MadtInfo, irq: i64, vector: i64, dest: i64) -> i64 {
```

**Parameters:**

- `info`: `&MadtInfo`
- `irq`: `i64`
- `vector`: `i64`
- `dest`: `i64`

**Returns:** `i64`

*Defined at line 531*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
