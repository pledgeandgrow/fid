[All modules](index.md)

# cortex_m -- Fides Documentation

## Contents

- [SYST_CSR:](#SYST_CSR:) (const)
- [SYST_RVR:](#SYST_RVR:) (const)
- [SYST_CVR:](#SYST_CVR:) (const)
- [SYST_CALIB:](#SYST_CALIB:) (const)
- [NVIC_ISER:](#NVIC_ISER:) (const)
- [NVIC_ICER:](#NVIC_ICER:) (const)
- [NVIC_ISPR:](#NVIC_ISPR:) (const)
- [NVIC_ICPR:](#NVIC_ICPR:) (const)
- [NVIC_IABR:](#NVIC_IABR:) (const)
- [NVIC_IPR:](#NVIC_IPR:) (const)
- [NVIC_STIR:](#NVIC_STIR:) (const)
- [SCB_CPUID:](#SCB_CPUID:) (const)
- [SCB_ICSR:](#SCB_ICSR:) (const)
- [SCB_VTOR:](#SCB_VTOR:) (const)
- [SCB_AIRCR:](#SCB_AIRCR:) (const)
- [SCB_SCR:](#SCB_SCR:) (const)
- [SCB_CCR:](#SCB_CCR:) (const)
- [SCB_SHPR1:](#SCB_SHPR1:) (const)
- [SCB_SHCSR:](#SCB_SHCSR:) (const)
- [SCB_CFSR:](#SCB_CFSR:) (const)
- [SCB_HFSR:](#SCB_HFSR:) (const)
- [SCB_DFSR:](#SCB_DFSR:) (const)
- [SCB_MMFAR:](#SCB_MMFAR:) (const)
- [SCB_BFAR:](#SCB_BFAR:) (const)
- [SCB_CPACR:](#SCB_CPACR:) (const)
- [MPU_TYPE:](#MPU_TYPE:) (const)
- [MPU_CTRL:](#MPU_CTRL:) (const)
- [MPU_RNR:](#MPU_RNR:) (const)
- [MPU_RBAR:](#MPU_RBAR:) (const)
- [MPU_RASR:](#MPU_RASR:) (const)
- [SYST_ENABLE:](#SYST_ENABLE:) (const)
- [SYST_TICKINT:](#SYST_TICKINT:) (const)
- [SYST_CLKSOURCE:](#SYST_CLKSOURCE:) (const)
- [SYST_COUNTFLAG:](#SYST_COUNTFLAG:) (const)
- [ICSR_NMIPENDSET:](#ICSR_NMIPENDSET:) (const)
- [ICSR_PENDSVSET:](#ICSR_PENDSVSET:) (const)
- [ICSR_PENDSVCLR:](#ICSR_PENDSVCLR:) (const)
- [ICSR_PENDSTSET:](#ICSR_PENDSTSET:) (const)
- [ICSR_PENDSTCLR:](#ICSR_PENDSTCLR:) (const)
- [AIRCR_VECTKEY:](#AIRCR_VECTKEY:) (const)
- [AIRCR_SYSRESETREQ:](#AIRCR_SYSRESETREQ:) (const)
- [EXC_RESET:](#EXC_RESET:) (const)
- [EXC_NMI:](#EXC_NMI:) (const)
- [EXC_HARDFAULT:](#EXC_HARDFAULT:) (const)
- [EXC_MEMMANAGE:](#EXC_MEMMANAGE:) (const)
- [EXC_BUSFAULT:](#EXC_BUSFAULT:) (const)
- [EXC_USAGEFAULT:](#EXC_USAGEFAULT:) (const)
- [EXC_SVCALL:](#EXC_SVCALL:) (const)
- [EXC_DEBUGMON:](#EXC_DEBUGMON:) (const)
- [EXC_PENDSV:](#EXC_PENDSV:) (const)
- [EXC_SYSTICK:](#EXC_SYSTICK:) (const)
- [cm_exception_name](#cm_exception_name) (function)
- [nvic_word](#nvic_word) (function)
- [nvic_bit](#nvic_bit) (function)
- [nvic_enable](#nvic_enable) (function)
- [nvic_disable](#nvic_disable) (function)
- [nvic_set_pending](#nvic_set_pending) (function)
- [nvic_clear_pending](#nvic_clear_pending) (function)
- [nvic_is_enabled](#nvic_is_enabled) (function)
- [nvic_is_pending](#nvic_is_pending) (function)
- [nvic_is_active](#nvic_is_active) (function)
- [nvic_trigger](#nvic_trigger) (function)
- [cm_prio_byte](#cm_prio_byte) (function)
- [cm_prio_addr](#cm_prio_addr) (function)
- [cm_set_priority](#cm_set_priority) (function)
- [cm_get_priority](#cm_get_priority) (function)
- [cm_encode_priority](#cm_encode_priority) (function)
- [cm_decode_priority](#cm_decode_priority) (function)
- [cm_set_priority_grouping](#cm_set_priority_grouping) (function)
- [cm_priority_grouping](#cm_priority_grouping) (function)
- [cm_system_reset](#cm_system_reset) (function)
- [cm_reset_value](#cm_reset_value) (function)
- [systick_reload](#systick_reload) (function)
- [systick_config](#systick_config) (function)
- [systick_stop](#systick_stop) (function)
- [systick_wrapped](#systick_wrapped) (function)
- [systick_calib_decode](#systick_calib_decode) (function)
- [systick_elapsed_ns](#systick_elapsed_ns) (function)
- [cm_vtor_align](#cm_vtor_align) (function)
- [cm_set_vtor](#cm_set_vtor) (function)
- [cm_pend_sv](#cm_pend_sv) (function)
- [cm_icsr_active](#cm_icsr_active) (function)
- [cm_icsr_pending](#cm_icsr_pending) (function)
- [cm_enable_faults](#cm_enable_faults) (function)
- [cm_enable_fpu](#cm_enable_fpu) (function)
- [cm_cpuid_decode](#cm_cpuid_decode) (function)
- [cm_cfsr_decode](#cm_cfsr_decode) (function)
- [cm_hfsr_decode](#cm_hfsr_decode) (function)
- [cm_fault_report](#cm_fault_report) (function)
- [cm_read_fault](#cm_read_fault) (function)
- [cm_clear_faults](#cm_clear_faults) (function)
- [cm_frame_decode](#cm_frame_decode) (function)
- [cm_exc_return_decode](#cm_exc_return_decode) (function)
- [MPU_AP_NONE:](#MPU_AP_NONE:) (const)
- [MPU_AP_PRIV_RW:](#MPU_AP_PRIV_RW:) (const)
- [MPU_AP_PRIV_RW_USER_RO:](#MPU_AP_PRIV_RW_USER_RO:) (const)
- [MPU_AP_FULL:](#MPU_AP_FULL:) (const)
- [MPU_AP_PRIV_RO:](#MPU_AP_PRIV_RO:) (const)
- [MPU_AP_RO:](#MPU_AP_RO:) (const)
- [MPU_STRONGLY_ORDERED:](#MPU_STRONGLY_ORDERED:) (const)
- [MPU_DEVICE:](#MPU_DEVICE:) (const)
- [MPU_NORMAL_WT:](#MPU_NORMAL_WT:) (const)
- [MPU_NORMAL_WB:](#MPU_NORMAL_WB:) (const)
- [MPU_NORMAL_NC:](#MPU_NORMAL_NC:) (const)
- [MPU_NORMAL_WBWA:](#MPU_NORMAL_WBWA:) (const)
- [MPU_DEVICE_NONSHARED:](#MPU_DEVICE_NONSHARED:) (const)
- [mpu_size_field](#mpu_size_field) (function)
- [mpu_rbar](#mpu_rbar) (function)
- [mpu_rasr](#mpu_rasr) (function)
- [mpu_region_check](#mpu_region_check) (function)
- [mpu_set_region](#mpu_set_region) (function)
- [mpu_enable](#mpu_enable) (function)
- [mpu_regions](#mpu_regions) (function)
- [mpu_region_decode](#mpu_region_decode) (function)

## SYST_CSR:

```fides
pub const SYST_CSR: i64 = 0xE000E010;
```

*Defined at line 25*

---

## SYST_RVR:

```fides
pub const SYST_RVR: i64 = 0xE000E014;
```

*Defined at line 26*

---

## SYST_CVR:

```fides
pub const SYST_CVR: i64 = 0xE000E018;
```

*Defined at line 27*

---

## SYST_CALIB:

```fides
pub const SYST_CALIB: i64 = 0xE000E01C;
```

*Defined at line 28*

---

## NVIC_ISER:

```fides
pub const NVIC_ISER: i64 = 0xE000E100;
```

*Defined at line 30*

---

## NVIC_ICER:

```fides
pub const NVIC_ICER: i64 = 0xE000E180;
```

*Defined at line 31*

---

## NVIC_ISPR:

```fides
pub const NVIC_ISPR: i64 = 0xE000E200;
```

*Defined at line 32*

---

## NVIC_ICPR:

```fides
pub const NVIC_ICPR: i64 = 0xE000E280;
```

*Defined at line 33*

---

## NVIC_IABR:

```fides
pub const NVIC_IABR: i64 = 0xE000E300;
```

*Defined at line 34*

---

## NVIC_IPR:

```fides
pub const NVIC_IPR: i64 = 0xE000E400;
```

*Defined at line 35*

---

## NVIC_STIR:

```fides
pub const NVIC_STIR: i64 = 0xE000EF00;
```

*Defined at line 36*

---

## SCB_CPUID:

```fides
pub const SCB_CPUID: i64 = 0xE000ED00;
```

*Defined at line 38*

---

## SCB_ICSR:

```fides
pub const SCB_ICSR: i64 = 0xE000ED04;
```

*Defined at line 39*

---

## SCB_VTOR:

```fides
pub const SCB_VTOR: i64 = 0xE000ED08;
```

*Defined at line 40*

---

## SCB_AIRCR:

```fides
pub const SCB_AIRCR: i64 = 0xE000ED0C;
```

*Defined at line 41*

---

## SCB_SCR:

```fides
pub const SCB_SCR: i64 = 0xE000ED10;
```

*Defined at line 42*

---

## SCB_CCR:

```fides
pub const SCB_CCR: i64 = 0xE000ED14;
```

*Defined at line 43*

---

## SCB_SHPR1:

```fides
pub const SCB_SHPR1: i64 = 0xE000ED18;
```

*Defined at line 44*

---

## SCB_SHCSR:

```fides
pub const SCB_SHCSR: i64 = 0xE000ED24;
```

*Defined at line 45*

---

## SCB_CFSR:

```fides
pub const SCB_CFSR: i64 = 0xE000ED28;
```

*Defined at line 46*

---

## SCB_HFSR:

```fides
pub const SCB_HFSR: i64 = 0xE000ED2C;
```

*Defined at line 47*

---

## SCB_DFSR:

```fides
pub const SCB_DFSR: i64 = 0xE000ED30;
```

*Defined at line 48*

---

## SCB_MMFAR:

```fides
pub const SCB_MMFAR: i64 = 0xE000ED34;
```

*Defined at line 49*

---

## SCB_BFAR:

```fides
pub const SCB_BFAR: i64 = 0xE000ED38;
```

*Defined at line 50*

---

## SCB_CPACR:

```fides
pub const SCB_CPACR: i64 = 0xE000ED88;
```

*Defined at line 51*

---

## MPU_TYPE:

```fides
pub const MPU_TYPE: i64 = 0xE000ED90;
```

*Defined at line 53*

---

## MPU_CTRL:

```fides
pub const MPU_CTRL: i64 = 0xE000ED94;
```

*Defined at line 54*

---

## MPU_RNR:

```fides
pub const MPU_RNR: i64 = 0xE000ED98;
```

*Defined at line 55*

---

## MPU_RBAR:

```fides
pub const MPU_RBAR: i64 = 0xE000ED9C;
```

*Defined at line 56*

---

## MPU_RASR:

```fides
pub const MPU_RASR: i64 = 0xE000EDA0;
```

*Defined at line 57*

---

## SYST_ENABLE:

```fides
pub const SYST_ENABLE: i64 = 1;
```

*Defined at line 60*

---

## SYST_TICKINT:

```fides
pub const SYST_TICKINT: i64 = 2;
```

*Defined at line 61*

---

## SYST_CLKSOURCE:

```fides
pub const SYST_CLKSOURCE: i64 = 4;
```

*Defined at line 62*

---

## SYST_COUNTFLAG:

```fides
pub const SYST_COUNTFLAG: i64 = 0x10000;
```

*Defined at line 63*

---

## ICSR_NMIPENDSET:

```fides
pub const ICSR_NMIPENDSET: i64 = 0x80000000;
```

*Defined at line 66*

---

## ICSR_PENDSVSET:

```fides
pub const ICSR_PENDSVSET: i64 = 0x10000000;
```

*Defined at line 67*

---

## ICSR_PENDSVCLR:

```fides
pub const ICSR_PENDSVCLR: i64 = 0x08000000;
```

*Defined at line 68*

---

## ICSR_PENDSTSET:

```fides
pub const ICSR_PENDSTSET: i64 = 0x04000000;
```

*Defined at line 69*

---

## ICSR_PENDSTCLR:

```fides
pub const ICSR_PENDSTCLR: i64 = 0x02000000;
```

*Defined at line 70*

---

## AIRCR_VECTKEY:

```fides
pub const AIRCR_VECTKEY: i64 = 0x05FA0000;
```

*Defined at line 73*

---

## AIRCR_SYSRESETREQ:

```fides
pub const AIRCR_SYSRESETREQ: i64 = 4;
```

*Defined at line 74*

---

## EXC_RESET:

```fides
pub const EXC_RESET: i64 = 1;
```

*Defined at line 77*

---

## EXC_NMI:

```fides
pub const EXC_NMI: i64 = 2;
```

*Defined at line 78*

---

## EXC_HARDFAULT:

```fides
pub const EXC_HARDFAULT: i64 = 3;
```

*Defined at line 79*

---

## EXC_MEMMANAGE:

```fides
pub const EXC_MEMMANAGE: i64 = 4;
```

*Defined at line 80*

---

## EXC_BUSFAULT:

```fides
pub const EXC_BUSFAULT: i64 = 5;
```

*Defined at line 81*

---

## EXC_USAGEFAULT:

```fides
pub const EXC_USAGEFAULT: i64 = 6;
```

*Defined at line 82*

---

## EXC_SVCALL:

```fides
pub const EXC_SVCALL: i64 = 11;
```

*Defined at line 83*

---

## EXC_DEBUGMON:

```fides
pub const EXC_DEBUGMON: i64 = 12;
```

*Defined at line 84*

---

## EXC_PENDSV:

```fides
pub const EXC_PENDSV: i64 = 14;
```

*Defined at line 85*

---

## EXC_SYSTICK:

```fides
pub const EXC_SYSTICK: i64 = 15;
```

*Defined at line 86*

---

## cm_exception_name

```fides
pub fn cm_exception_name(exc: i64) -> str {
```

**Parameters:**

- `exc`: `i64`

**Returns:** `str`

*Defined at line 88*

---

## nvic_word

Word address and bit of IRQ `irq` in one of the NVIC bit arrays.

```fides
pub fn nvic_word(base: i64, irq: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `irq`: `i64`

**Returns:** `i64`

*Defined at line 131*

---

## nvic_bit

```fides
pub fn nvic_bit(irq: i64) -> i64 {
```

**Parameters:**

- `irq`: `i64`

**Returns:** `i64`

*Defined at line 135*

---

## nvic_enable

Enable IRQ `irq`: write its bit to ISER (writing 0 bits has no effect,
so no read-modify-write).

```fides
pub fn nvic_enable(bus: &mut RegBus, irq: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

*Defined at line 147*

---

## nvic_disable

Disable IRQ `irq` (ICER). The ARM ARM wants a DSB+ISB after it when the
caller must be sure the IRQ can no longer fire.

```fides
pub fn nvic_disable(bus: &mut RegBus, irq: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

*Defined at line 154*

---

## nvic_set_pending

```fides
pub fn nvic_set_pending(bus: &mut RegBus, irq: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

*Defined at line 159*

---

## nvic_clear_pending

```fides
pub fn nvic_clear_pending(bus: &mut RegBus, irq: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

*Defined at line 164*

---

## nvic_is_enabled

Enabled? (ISER reads back the enable state.)

```fides
pub fn nvic_is_enabled(bus: &mut RegBus, irq: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

**Returns:** `bool`

*Defined at line 170*

---

## nvic_is_pending

```fides
pub fn nvic_is_pending(bus: &mut RegBus, irq: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

**Returns:** `bool`

*Defined at line 175*

---

## nvic_is_active

```fides
pub fn nvic_is_active(bus: &mut RegBus, irq: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

**Returns:** `bool`

*Defined at line 180*

---

## nvic_trigger

Trigger IRQ `irq` from software (STIR, needs USERSETMPEND for
unprivileged code).

```fides
pub fn nvic_trigger(bus: &mut RegBus, irq: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `irq`: `i64`

*Defined at line 187*

---

## cm_prio_byte

The byte written to an 8-bit priority register for priority `prio`
on a part implementing `bits` priority bits (they are the top bits).

```fides
pub fn cm_prio_byte(prio: i64, bits: i64) -> i64 {
```

**Parameters:**

- `prio`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 194*

---

## cm_prio_addr

Address of the priority byte of exception number `exc` (IRQs: IPR,
system handlers 4..15: SHPR1..3). 0 for fixed-priority exceptions.

```fides
pub fn cm_prio_addr(exc: i64) -> i64 {
```

**Parameters:**

- `exc`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## cm_set_priority

Set the priority of exception `exc` (byte access, as the ARM ARM
allows for IPR and SHPR).

```fides
pub fn cm_set_priority(bus: &mut RegBus, exc: i64, prio: i64, bits: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `exc`: `i64`
- `prio`: `i64`
- `bits`: `i64`

*Defined at line 218*

---

## cm_get_priority

```fides
pub fn cm_get_priority(bus: &mut RegBus, exc: i64, bits: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `exc`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 226*

---

## cm_encode_priority

NVIC_EncodePriority: with AIRCR.PRIGROUP = `group` (0..7), the
priority byte splits into preempt bits [7:group+1] and sub bits
[group:0]; on a part with `bits` implemented bits the result is the
`bits`-wide priority value to give cm_set_priority.

```fides
pub fn cm_encode_priority(group: i64, preempt: i64, sub: i64, bits: i64) -> i64 {
```

**Parameters:**

- `group`: `i64`
- `preempt`: `i64`
- `sub`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 244*

---

## cm_decode_priority

The inverse: (preempt, sub) packed as preempt * 256 + sub.

```fides
pub fn cm_decode_priority(prio: i64, group: i64, bits: i64) -> i64 {
```

**Parameters:**

- `prio`: `i64`
- `group`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 258*

---

## cm_set_priority_grouping

Set PRIGROUP: AIRCR must be written with VECTKEY; the other writable
bits (VECTCLRACTIVE, SYSRESETREQ) are written 0.

```fides
pub fn cm_set_priority_grouping(bus: &mut RegBus, group: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `group`: `i64`

*Defined at line 275*

---

## cm_priority_grouping

```fides
pub fn cm_priority_grouping(bus: &mut RegBus) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`

**Returns:** `i64`

*Defined at line 281*

---

## cm_system_reset

NVIC_SystemReset: request a reset keeping PRIGROUP. On the target the
caller issues DSB before and loops after (the reset is asynchronous).

```fides
pub fn cm_system_reset(bus: &mut RegBus) {
```

**Parameters:**

- `bus`: `&mut RegBus`

*Defined at line 287*

---

## cm_reset_value

The value written to AIRCR by cm_system_reset for a given AIRCR.

```fides
pub fn cm_reset_value(aircr: i64) -> i64 {
```

**Parameters:**

- `aircr`: `i64`

**Returns:** `i64`

*Defined at line 293*

---

## systick_reload

Reload value for `hz` interrupts per second from a `clk_hz` clock;
-1 when it does not fit the 24-bit counter.

```fides
pub fn systick_reload(clk_hz: i64, hz: i64) -> i64 {
```

**Parameters:**

- `clk_hz`: `i64`
- `hz`: `i64`

**Returns:** `i64`

*Defined at line 301*

---

## systick_config

SysTick_Config: reload, lowest priority for SysTick, clear the current
value, then processor clock + interrupt + enable. False (nothing
written) when the period does not fit.

```fides
pub fn systick_config(bus: &mut RegBus, ticks: i64, prio_bits: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `ticks`: `i64`
- `prio_bits`: `i64`

**Returns:** `bool`

*Defined at line 315*

---

## systick_stop

```fides
pub fn systick_stop(bus: &mut RegBus) {
```

**Parameters:**

- `bus`: `&mut RegBus`

*Defined at line 326*

---

## systick_wrapped

COUNTFLAG: set when the counter wrapped since the last CSR read (reading
clears it on the hardware).

```fides
pub fn systick_wrapped(bus: &mut RegBus) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`

**Returns:** `bool`

*Defined at line 332*

---

## systick_calib_decode

SYST_CALIB decode: "TENMS=<n> SKEW=<0/1> NOREF=<0/1>".

```fides
pub fn systick_calib_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 337*

---

## systick_elapsed_ns

Nanoseconds between two CVR samples (the counter counts down from
RVR to 0 and reloads) at `clk_hz`.

```fides
pub fn systick_elapsed_ns(start: i64, now: i64, reload: i64, clk_hz: i64) -> i64 {
```

**Parameters:**

- `start`: `i64`
- `now`: `i64`
- `reload`: `i64`
- `clk_hz`: `i64`

**Returns:** `i64`

*Defined at line 343*

---

## cm_vtor_align

Alignment the vector table needs for `n_vectors` entries: the size
rounded up to a power of two, at least 128 bytes (B3.2.5).

```fides
pub fn cm_vtor_align(n_vectors: i64) -> i64 {
```

**Parameters:**

- `n_vectors`: `i64`

**Returns:** `i64`

*Defined at line 355*

---

## cm_set_vtor

Move the vector table. False when `addr` is not aligned for it. The
caller issues DSB; ISB after.

```fides
pub fn cm_set_vtor(bus: &mut RegBus, addr: i64, n_vectors: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `addr`: `i64`
- `n_vectors`: `i64`

**Returns:** `bool`

*Defined at line 365*

---

## cm_pend_sv

Pend PendSV (the context-switch request of most RTOSes).

```fides
pub fn cm_pend_sv(bus: &mut RegBus) {
```

**Parameters:**

- `bus`: `&mut RegBus`

*Defined at line 374*

---

## cm_icsr_active

ICSR.VECTACTIVE: the exception number being handled (0: Thread mode).

```fides
pub fn cm_icsr_active(icsr: i64) -> i64 {
```

**Parameters:**

- `icsr`: `i64`

**Returns:** `i64`

*Defined at line 379*

---

## cm_icsr_pending

ICSR.VECTPENDING: the highest-priority pending exception.

```fides
pub fn cm_icsr_pending(icsr: i64) -> i64 {
```

**Parameters:**

- `icsr`: `i64`

**Returns:** `i64`

*Defined at line 384*

---

## cm_enable_faults

Enable the configurable fault handlers (SHCSR MEMFAULTENA, BUSFAULTENA,
USGFAULTENA) so faults do not escalate to HardFault.

```fides
pub fn cm_enable_faults(bus: &mut RegBus) {
```

**Parameters:**

- `bus`: `&mut RegBus`

*Defined at line 390*

---

## cm_enable_fpu

Full access to coprocessors CP10/CP11 (the FPU) in CPACR.

```fides
pub fn cm_enable_fpu(bus: &mut RegBus) {
```

**Parameters:**

- `bus`: `&mut RegBus`

*Defined at line 396*

---

## cm_cpuid_decode

CPUID decode (B3.2.3): "ARM Cortex-M4 r0p1".

```fides
pub fn cm_cpuid_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 402*

---

## cm_cfsr_decode

The fault flags set in CFSR (MMFSR bits 7:0, BFSR 15:8, UFSR 31:16).

```fides
pub fn cm_cfsr_decode(cfsr: i64) -> Vec<str> {
```

**Parameters:**

- `cfsr`: `i64`

**Returns:** `Vec<str>`

*Defined at line 440*

---

## cm_hfsr_decode

HFSR flags: VECTTBL (bit 1), FORCED (30, escalated), DEBUGEVT (31).

```fides
pub fn cm_hfsr_decode(hfsr: i64) -> Vec<str> {
```

**Parameters:**

- `hfsr`: `i64`

**Returns:** `Vec<str>`

*Defined at line 493*

---

## cm_fault_report

A one-line fault report from the fault registers: the flags, and the
fault address when its VALID bit says MMFAR/BFAR holds one.

```fides
pub fn cm_fault_report(cfsr: i64, hfsr: i64, mmfar: i64, bfar: i64) -> str {
```

**Parameters:**

- `cfsr`: `i64`
- `hfsr`: `i64`
- `mmfar`: `i64`
- `bfar`: `i64`

**Returns:** `str`

*Defined at line 509*

---

## cm_read_fault

Read the fault registers through the bus and report.

```fides
pub fn cm_read_fault(bus: &mut RegBus) -> str {
```

**Parameters:**

- `bus`: `&mut RegBus`

**Returns:** `str`

*Defined at line 526*

---

## cm_clear_faults

Clear the fault flags (CFSR and HFSR are write-1-to-clear).

```fides
pub fn cm_clear_faults(bus: &mut RegBus, cfsr: i64, hfsr: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `cfsr`: `i64`
- `hfsr`: `i64`

*Defined at line 535*

---

## cm_frame_decode

The exception frame a fault pushes on the stack (8 words: r0-r3, r12,
lr, pc, xPSR), as "pc=0x... lr=0x... xpsr=0x...".

```fides
pub fn cm_frame_decode(frame: Vec<i64>) -> str {
```

**Parameters:**

- `frame`: `Vec<i64>`

**Returns:** `str`

*Defined at line 542*

---

## cm_exc_return_decode

EXC_RETURN decode: which stack and mode the exception returns to.

```fides
pub fn cm_exc_return_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 550*

---

## MPU_AP_NONE:

```fides
pub const MPU_AP_NONE: i64 = 0;
```

*Defined at line 571*

---

## MPU_AP_PRIV_RW:

```fides
pub const MPU_AP_PRIV_RW: i64 = 1;
```

*Defined at line 572*

---

## MPU_AP_PRIV_RW_USER_RO:

```fides
pub const MPU_AP_PRIV_RW_USER_RO: i64 = 2;
```

*Defined at line 573*

---

## MPU_AP_FULL:

```fides
pub const MPU_AP_FULL: i64 = 3;
```

*Defined at line 574*

---

## MPU_AP_PRIV_RO:

```fides
pub const MPU_AP_PRIV_RO: i64 = 5;
```

*Defined at line 575*

---

## MPU_AP_RO:

```fides
pub const MPU_AP_RO: i64 = 6;
```

*Defined at line 576*

---

## MPU_STRONGLY_ORDERED:

```fides
pub const MPU_STRONGLY_ORDERED: i64 = 0;
```

*Defined at line 579*

---

## MPU_DEVICE:

```fides
pub const MPU_DEVICE: i64 = 1;
```

*Defined at line 580*

---

## MPU_NORMAL_WT:

```fides
pub const MPU_NORMAL_WT: i64 = 2;
```

*Defined at line 581*

---

## MPU_NORMAL_WB:

```fides
pub const MPU_NORMAL_WB: i64 = 3;
```

*Defined at line 582*

---

## MPU_NORMAL_NC:

```fides
pub const MPU_NORMAL_NC: i64 = 4;
```

*Defined at line 583*

---

## MPU_NORMAL_WBWA:

```fides
pub const MPU_NORMAL_WBWA: i64 = 7;
```

*Defined at line 584*

---

## MPU_DEVICE_NONSHARED:

```fides
pub const MPU_DEVICE_NONSHARED: i64 = 8;
```

*Defined at line 585*

---

## mpu_size_field

RASR.SIZE for a region of `bytes` (a power of two >= 32): log2 - 1.
-1 when invalid.

```fides
pub fn mpu_size_field(bytes: i64) -> i64 {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `i64`

*Defined at line 589*

---

## mpu_rbar

RBAR for region `region` at `base` with VALID set (so RNR is written
by the same store).

```fides
pub fn mpu_rbar(base: i64, region: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `region`: `i64`

**Returns:** `i64`

*Defined at line 607*

---

## mpu_rasr

RASR: enable, size, sub-region disable mask, TEX/C/B memory type
(`texcb` as TEX<<2 | C<<1 | B), shareable, access, execute-never.

```fides
pub fn mpu_rasr(bytes: i64, ap: i64, texcb: i64, shareable: bool, xn: bool, srd: i64) -> i64 {
```

**Parameters:**

- `bytes`: `i64`
- `ap`: `i64`
- `texcb`: `i64`
- `shareable`: `bool`
- `xn`: `bool`
- `srd`: `i64`

**Returns:** `i64`

*Defined at line 613*

---

## mpu_region_check

Check a region: base aligned to its size. Returns "" when fine.

```fides
pub fn mpu_region_check(base: i64, bytes: i64) -> str {
```

**Parameters:**

- `base`: `i64`
- `bytes`: `i64`

**Returns:** `str`

*Defined at line 631*

---

## mpu_set_region

Program region `region` (RBAR with VALID, then RASR). Enable the MPU
with mpu_enable afterwards.

```fides
pub fn mpu_set_region(bus: &mut RegBus, region: i64, base: i64, rasr: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `region`: `i64`
- `base`: `i64`
- `rasr`: `i64`

*Defined at line 643*

---

## mpu_enable

MPU_CTRL: ENABLE, plus PRIVDEFENA (privileged code sees the default map
outside regions). DSB+ISB after, on the target.

```fides
pub fn mpu_enable(bus: &mut RegBus, privdefena: bool) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `privdefena`: `bool`

*Defined at line 650*

---

## mpu_regions

MPU_TYPE.DREGION: how many regions the MPU has.

```fides
pub fn mpu_regions(type_: i64) -> i64 {
```

**Parameters:**

- `type_`: `i64`

**Returns:** `i64`

*Defined at line 659*

---

## mpu_region_decode

Decode a region: "base 0x20000000 size 64K full XN normal-WBWA S".

```fides
pub fn mpu_region_decode(rbar: i64, rasr: i64) -> str {
```

**Parameters:**

- `rbar`: `i64`
- `rasr`: `i64`

**Returns:** `str`

*Defined at line 664*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
