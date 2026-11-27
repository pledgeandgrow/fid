[All modules](index.md)

# aarch64_sys -- Fides Documentation

## Contents

- [a64_current_el](#a64_current_el) (function)
- [a64_spsr_mode](#a64_spsr_mode) (function)
- [PSTATE_D:](#PSTATE_D:) (const)
- [PSTATE_A:](#PSTATE_A:) (const)
- [PSTATE_I:](#PSTATE_I:) (const)
- [PSTATE_F:](#PSTATE_F:) (const)
- [a64_spsr](#a64_spsr) (function)
- [a64_spsr_decode](#a64_spsr_decode) (function)
- [a64_esr_ec](#a64_esr_ec) (function)
- [a64_esr_iss](#a64_esr_iss) (function)
- [a64_ec_name](#a64_ec_name) (function)
- [a64_fsc_name](#a64_fsc_name) (function)
- [a64_esr_decode](#a64_esr_decode) (function)
- [a64_vector_offset](#a64_vector_offset) (function)
- [a64_sysreg](#a64_sysreg) (function)
- [A64_CURRENTEL](#A64_CURRENTEL) (function)
- [A64_DAIF](#A64_DAIF) (function)
- [A64_SPSR_EL1](#A64_SPSR_EL1) (function)
- [A64_ELR_EL1](#A64_ELR_EL1) (function)
- [A64_SCTLR_EL1](#A64_SCTLR_EL1) (function)
- [A64_TTBR0_EL1](#A64_TTBR0_EL1) (function)
- [A64_TTBR1_EL1](#A64_TTBR1_EL1) (function)
- [A64_TCR_EL1](#A64_TCR_EL1) (function)
- [A64_ESR_EL1](#A64_ESR_EL1) (function)
- [A64_FAR_EL1](#A64_FAR_EL1) (function)
- [A64_MAIR_EL1](#A64_MAIR_EL1) (function)
- [A64_VBAR_EL1](#A64_VBAR_EL1) (function)
- [A64_MPIDR_EL1](#A64_MPIDR_EL1) (function)
- [A64_CNTFRQ_EL0](#A64_CNTFRQ_EL0) (function)
- [A64_CNTPCT_EL0](#A64_CNTPCT_EL0) (function)
- [A64_CNTVCT_EL0](#A64_CNTVCT_EL0) (function)
- [A64_CNTP_TVAL_EL0](#A64_CNTP_TVAL_EL0) (function)
- [A64_CNTP_CTL_EL0](#A64_CNTP_CTL_EL0) (function)
- [A64_CNTP_CVAL_EL0](#A64_CNTP_CVAL_EL0) (function)
- [A64_CNTV_CTL_EL0](#A64_CNTV_CTL_EL0) (function)
- [A64_CNTV_CVAL_EL0](#A64_CNTV_CVAL_EL0) (function)
- [A64_ICC_PMR_EL1](#A64_ICC_PMR_EL1) (function)
- [A64_ICC_IAR1_EL1](#A64_ICC_IAR1_EL1) (function)
- [A64_ICC_EOIR1_EL1](#A64_ICC_EOIR1_EL1) (function)
- [A64_ICC_SGI1R_EL1](#A64_ICC_SGI1R_EL1) (function)
- [A64_ICC_SRE_EL1](#A64_ICC_SRE_EL1) (function)
- [A64_ICC_IGRPEN1_EL1](#A64_ICC_IGRPEN1_EL1) (function)
- [a64_mrs](#a64_mrs) (function)
- [a64_msr](#a64_msr) (function)
- [A64_ERET:](#A64_ERET:) (const)
- [A64_WFI:](#A64_WFI:) (const)
- [A64_ISB:](#A64_ISB:) (const)
- [A64_DSB_SY:](#A64_DSB_SY:) (const)
- [a64_sysreg_name](#a64_sysreg_name) (function)
- [a64_sysreg_disasm](#a64_sysreg_disasm) (function)
- [a64_mpidr_decode](#a64_mpidr_decode) (function)
- [CNT_CTL_ENABLE:](#CNT_CTL_ENABLE:) (const)
- [CNT_CTL_IMASK:](#CNT_CTL_IMASK:) (const)
- [CNT_CTL_ISTATUS:](#CNT_CTL_ISTATUS:) (const)
- [a64_timer_tval](#a64_timer_tval) (function)
- [a64_timer_cval](#a64_timer_cval) (function)
- [a64_timer_tval_read](#a64_timer_tval_read) (function)
- [a64_timer_ctl](#a64_timer_ctl) (function)
- [a64_timer_fires](#a64_timer_fires) (function)
- [DESC_INVALID:](#DESC_INVALID:) (const)
- [DESC_BLOCK:](#DESC_BLOCK:) (const)
- [DESC_TABLE:](#DESC_TABLE:) (const)
- [DESC_PAGE:](#DESC_PAGE:) (const)
- [DESC_NS:](#DESC_NS:) (const)
- [DESC_AP_RW_EL1:](#DESC_AP_RW_EL1:) (const)
- [DESC_AP_RW_ALL:](#DESC_AP_RW_ALL:) (const)
- [DESC_AP_RO_EL1:](#DESC_AP_RO_EL1:) (const)
- [DESC_AP_RO_ALL:](#DESC_AP_RO_ALL:) (const)
- [DESC_SH_NONE:](#DESC_SH_NONE:) (const)
- [DESC_SH_OUTER:](#DESC_SH_OUTER:) (const)
- [DESC_SH_INNER:](#DESC_SH_INNER:) (const)
- [DESC_AF:](#DESC_AF:) (const)
- [DESC_NG:](#DESC_NG:) (const)
- [DESC_CONT:](#DESC_CONT:) (const)
- [DESC_PXN:](#DESC_PXN:) (const)
- [DESC_UXN:](#DESC_UXN:) (const)
- [MAIR_DEVICE_NGNRNE:](#MAIR_DEVICE_NGNRNE:) (const)
- [MAIR_DEVICE_NGNRE:](#MAIR_DEVICE_NGNRE:) (const)
- [MAIR_NORMAL_NC:](#MAIR_NORMAL_NC:) (const)
- [MAIR_NORMAL_WB:](#MAIR_NORMAL_WB:) (const)
- [a64_mair](#a64_mair) (function)
- [a64_tcr](#a64_tcr) (function)
- [a64_va_index](#a64_va_index) (function)
- [a64_level_size](#a64_level_size) (function)
- [a64_table_desc](#a64_table_desc) (function)
- [a64_leaf_desc](#a64_leaf_desc) (function)
- [a64_desc_decode](#a64_desc_decode) (function)
- [A64Mem](#A64Mem) (struct)
- [a64mem_new](#a64mem_new) (function)
- [a64mem_map](#a64mem_map) (function)
- [A64Walk](#A64Walk) (struct)
- [a64mem_walk](#a64mem_walk) (function)
- [a64_check_access](#a64_check_access) (function)
- [GICD_CTLR:](#GICD_CTLR:) (const)
- [GICD_TYPER:](#GICD_TYPER:) (const)
- [GICD_IIDR:](#GICD_IIDR:) (const)
- [GICD_IGROUPR:](#GICD_IGROUPR:) (const)
- [GICD_ISENABLER:](#GICD_ISENABLER:) (const)
- [GICD_ICENABLER:](#GICD_ICENABLER:) (const)
- [GICD_ISPENDR:](#GICD_ISPENDR:) (const)
- [GICD_ICPENDR:](#GICD_ICPENDR:) (const)
- [GICD_ISACTIVER:](#GICD_ISACTIVER:) (const)
- [GICD_ICACTIVER:](#GICD_ICACTIVER:) (const)
- [GICD_IPRIORITYR:](#GICD_IPRIORITYR:) (const)
- [GICD_ITARGETSR:](#GICD_ITARGETSR:) (const)
- [GICD_ICFGR:](#GICD_ICFGR:) (const)
- [GICD_SGIR:](#GICD_SGIR:) (const)
- [GICD_IROUTER:](#GICD_IROUTER:) (const)
- [GICC_CTLR:](#GICC_CTLR:) (const)
- [GICC_PMR:](#GICC_PMR:) (const)
- [GICC_BPR:](#GICC_BPR:) (const)
- [GICC_IAR:](#GICC_IAR:) (const)
- [GICC_EOIR:](#GICC_EOIR:) (const)
- [GICC_RPR:](#GICC_RPR:) (const)
- [GICC_HPPIR:](#GICC_HPPIR:) (const)
- [GIC_SPURIOUS:](#GIC_SPURIOUS:) (const)
- [gic_lines](#gic_lines) (function)
- [gic_kind](#gic_kind) (function)
- [gic_enable](#gic_enable) (function)
- [gic_disable](#gic_disable) (function)
- [gic_set_priority](#gic_set_priority) (function)
- [gic_set_targets](#gic_set_targets) (function)
- [gic_set_edge](#gic_set_edge) (function)
- [gicv2_sgir](#gicv2_sgir) (function)
- [gicv2_send_sgi](#gicv2_send_sgi) (function)
- [gicv2_init_dist](#gicv2_init_dist) (function)
- [gicv2_init_cpu](#gicv2_init_cpu) (function)
- [gicv2_ack](#gicv2_ack) (function)
- [gicv2_eoi](#gicv2_eoi) (function)
- [GICR_CTLR:](#GICR_CTLR:) (const)
- [GICR_TYPER:](#GICR_TYPER:) (const)
- [GICR_WAKER:](#GICR_WAKER:) (const)
- [GICR_SGI_BASE:](#GICR_SGI_BASE:) (const)
- [GICR_STRIDE:](#GICR_STRIDE:) (const)
- [gicr_base](#gicr_base) (function)
- [gicv3_irouter](#gicv3_irouter) (function)
- [gicv3_sgi1r](#gicv3_sgi1r) (function)
- [gicv3_wake](#gicv3_wake) (function)
- [gicv3_enable_ppi](#gicv3_enable_ppi) (function)
- [gicv3_route_spi](#gicv3_route_spi) (function)

## a64_current_el

CurrentEL register value -> EL number (bits 3:2).

```fides
pub fn a64_current_el(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 36*

---

## a64_spsr_mode

SPSR M[3:0] for returning to `el` with SP_ELx (h) or SP_EL0 (t).

```fides
pub fn a64_spsr_mode(el: i64, use_spx: bool) -> i64 {
```

**Parameters:**

- `el`: `i64`
- `use_spx`: `bool`

**Returns:** `i64`

*Defined at line 41*

---

## PSTATE_D:

```fides
pub const PSTATE_D: i64 = 0x200;
```

*Defined at line 49*

---

## PSTATE_A:

```fides
pub const PSTATE_A: i64 = 0x100;
```

*Defined at line 50*

---

## PSTATE_I:

```fides
pub const PSTATE_I: i64 = 0x80;
```

*Defined at line 51*

---

## PSTATE_F:

```fides
pub const PSTATE_F: i64 = 0x40;
```

*Defined at line 52*

---

## a64_spsr

SPSR value for an ERET to `el` (h variant) with the given DAIF masks.

```fides
pub fn a64_spsr(el: i64, daif: i64) -> i64 {
```

**Parameters:**

- `el`: `i64`
- `daif`: `i64`

**Returns:** `i64`

*Defined at line 55*

---

## a64_spsr_decode

Decode an SPSR_ELx: "EL1h DAIF=0101 NZCV=0000".

```fides
pub fn a64_spsr_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 60*

---

## a64_esr_ec

```fides
pub fn a64_esr_ec(esr: i64) -> i64 {
```

**Parameters:**

- `esr`: `i64`

**Returns:** `i64`

*Defined at line 102*

---

## a64_esr_iss

```fides
pub fn a64_esr_iss(esr: i64) -> i64 {
```

**Parameters:**

- `esr`: `i64`

**Returns:** `i64`

*Defined at line 106*

---

## a64_ec_name

```fides
pub fn a64_ec_name(ec: i64) -> str {
```

**Parameters:**

- `ec`: `i64`

**Returns:** `str`

*Defined at line 110*

---

## a64_fsc_name

Fault status code (DFSC/IFSC) name, with the translation level.

```fides
pub fn a64_fsc_name(fsc: i64) -> str {
```

**Parameters:**

- `fsc`: `i64`

**Returns:** `str`

*Defined at line 190*

---

## a64_esr_decode

One line for an ESR (and FAR when the syndrome says it is valid).

```fides
pub fn a64_esr_decode(esr: i64, far: i64) -> str {
```

**Parameters:**

- `esr`: `i64`
- `far`: `i64`

**Returns:** `str`

*Defined at line 224*

---

## a64_vector_offset

The vector-table offset VBAR_ELx + offset for an exception: `kind` 0
synchronous, 1 IRQ, 2 FIQ, 3 SError; from the current EL with SP_EL0
(0), with SP_ELx (1), from a lower EL in AArch64 (2) or AArch32 (3).

```fides
pub fn a64_vector_offset(source: i64, kind: i64) -> i64 {
```

**Parameters:**

- `source`: `i64`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 271*

---

## a64_sysreg

A system register as op0:op1:CRn:CRm:op2 packed (op0 << 14 | op1 << 11
| CRn << 7 | CRm << 3 | op2), the order of the MRS/MSR fields.

```fides
pub fn a64_sysreg(op0: i64, op1: i64, crn: i64, crm: i64, op2: i64) -> i64 {
```

**Parameters:**

- `op0`: `i64`
- `op1`: `i64`
- `crn`: `i64`
- `crm`: `i64`
- `op2`: `i64`

**Returns:** `i64`

*Defined at line 279*

---

## A64_CURRENTEL

```fides
pub fn A64_CURRENTEL() -> i64 {
```

**Returns:** `i64`

*Defined at line 283*

---

## A64_DAIF

```fides
pub fn A64_DAIF() -> i64 {
```

**Returns:** `i64`

*Defined at line 286*

---

## A64_SPSR_EL1

```fides
pub fn A64_SPSR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 289*

---

## A64_ELR_EL1

```fides
pub fn A64_ELR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 292*

---

## A64_SCTLR_EL1

```fides
pub fn A64_SCTLR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 295*

---

## A64_TTBR0_EL1

```fides
pub fn A64_TTBR0_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 298*

---

## A64_TTBR1_EL1

```fides
pub fn A64_TTBR1_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 301*

---

## A64_TCR_EL1

```fides
pub fn A64_TCR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 304*

---

## A64_ESR_EL1

```fides
pub fn A64_ESR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 307*

---

## A64_FAR_EL1

```fides
pub fn A64_FAR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 310*

---

## A64_MAIR_EL1

```fides
pub fn A64_MAIR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 313*

---

## A64_VBAR_EL1

```fides
pub fn A64_VBAR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 316*

---

## A64_MPIDR_EL1

```fides
pub fn A64_MPIDR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 319*

---

## A64_CNTFRQ_EL0

```fides
pub fn A64_CNTFRQ_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 322*

---

## A64_CNTPCT_EL0

```fides
pub fn A64_CNTPCT_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 325*

---

## A64_CNTVCT_EL0

```fides
pub fn A64_CNTVCT_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 328*

---

## A64_CNTP_TVAL_EL0

```fides
pub fn A64_CNTP_TVAL_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 331*

---

## A64_CNTP_CTL_EL0

```fides
pub fn A64_CNTP_CTL_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 334*

---

## A64_CNTP_CVAL_EL0

```fides
pub fn A64_CNTP_CVAL_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 337*

---

## A64_CNTV_CTL_EL0

```fides
pub fn A64_CNTV_CTL_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 340*

---

## A64_CNTV_CVAL_EL0

```fides
pub fn A64_CNTV_CVAL_EL0() -> i64 {
```

**Returns:** `i64`

*Defined at line 343*

---

## A64_ICC_PMR_EL1

```fides
pub fn A64_ICC_PMR_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 346*

---

## A64_ICC_IAR1_EL1

```fides
pub fn A64_ICC_IAR1_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 349*

---

## A64_ICC_EOIR1_EL1

```fides
pub fn A64_ICC_EOIR1_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 352*

---

## A64_ICC_SGI1R_EL1

```fides
pub fn A64_ICC_SGI1R_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 355*

---

## A64_ICC_SRE_EL1

```fides
pub fn A64_ICC_SRE_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 358*

---

## A64_ICC_IGRPEN1_EL1

```fides
pub fn A64_ICC_IGRPEN1_EL1() -> i64 {
```

**Returns:** `i64`

*Defined at line 361*

---

## a64_mrs

MRS Xt, <sysreg>.

```fides
pub fn a64_mrs(rt: i64, sysreg: i64) -> i64 {
```

**Parameters:**

- `rt`: `i64`
- `sysreg`: `i64`

**Returns:** `i64`

*Defined at line 366*

---

## a64_msr

MSR <sysreg>, Xt.

```fides
pub fn a64_msr(sysreg: i64, rt: i64) -> i64 {
```

**Parameters:**

- `sysreg`: `i64`
- `rt`: `i64`

**Returns:** `i64`

*Defined at line 371*

---

## A64_ERET:

```fides
pub const A64_ERET: i64 = 0xD69F03E0;
```

*Defined at line 375*

---

## A64_WFI:

```fides
pub const A64_WFI: i64 = 0xD503207F;
```

*Defined at line 376*

---

## A64_ISB:

```fides
pub const A64_ISB: i64 = 0xD5033FDF;
```

*Defined at line 377*

---

## A64_DSB_SY:

```fides
pub const A64_DSB_SY: i64 = 0xD5033F9F;
```

*Defined at line 378*

---

## a64_sysreg_name

The name of a packed system register (those above), else S<op0>_...

```fides
pub fn a64_sysreg_name(r: i64) -> str {
```

**Parameters:**

- `r`: `i64`

**Returns:** `str`

*Defined at line 381*

---

## a64_sysreg_disasm

Disassemble an MRS/MSR: "mrs x0, CurrentEL"; "" otherwise.

```fides
pub fn a64_sysreg_disasm(insn: i64) -> str {
```

**Parameters:**

- `insn`: `i64`

**Returns:** `str`

*Defined at line 440*

---

## a64_mpidr_decode

MPIDR_EL1 affinity as "Aff3.Aff2.Aff1.Aff0".

```fides
pub fn a64_mpidr_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 455*

---

## CNT_CTL_ENABLE:

```fides
pub const CNT_CTL_ENABLE: i64 = 1;
```

*Defined at line 461*

---

## CNT_CTL_IMASK:

```fides
pub const CNT_CTL_IMASK: i64 = 2;
```

*Defined at line 462*

---

## CNT_CTL_ISTATUS:

```fides
pub const CNT_CTL_ISTATUS: i64 = 4;
```

*Defined at line 463*

---

## a64_timer_tval

TVAL for an interrupt `us` microseconds from now.

```fides
pub fn a64_timer_tval(freq: i64, us: i64) -> i64 {
```

**Parameters:**

- `freq`: `i64`
- `us`: `i64`

**Returns:** `i64`

*Defined at line 466*

---

## a64_timer_cval

CVAL for a deadline `us` after the counter value `now`.

```fides
pub fn a64_timer_cval(now: i64, freq: i64, us: i64) -> i64 {
```

**Parameters:**

- `now`: `i64`
- `freq`: `i64`
- `us`: `i64`

**Returns:** `i64`

*Defined at line 471*

---

## a64_timer_tval_read

TVAL as the hardware reads it: CVAL - counter, as a signed 32-bit
value (negative once the deadline has passed).

```fides
pub fn a64_timer_tval_read(cval: i64, now: i64) -> i64 {
```

**Parameters:**

- `cval`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 477*

---

## a64_timer_ctl

The timer condition: ISTATUS is set when the counter has reached CVAL
(and the timer is enabled); the interrupt fires unless IMASK.

```fides
pub fn a64_timer_ctl(enabled: bool, masked: bool, cval: i64, now: i64) -> i64 {
```

**Parameters:**

- `enabled`: `bool`
- `masked`: `bool`
- `cval`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 487*

---

## a64_timer_fires

```fides
pub fn a64_timer_fires(ctl: i64) -> bool {
```

**Parameters:**

- `ctl`: `i64`

**Returns:** `bool`

*Defined at line 501*

---

## DESC_INVALID:

```fides
pub const DESC_INVALID: i64 = 0;
```

*Defined at line 507*

---

## DESC_BLOCK:

```fides
pub const DESC_BLOCK: i64 = 1;
```

*Defined at line 508*

---

## DESC_TABLE:

```fides
pub const DESC_TABLE: i64 = 3;
```

*Defined at line 509*

---

## DESC_PAGE:

```fides
pub const DESC_PAGE: i64 = 3;
```

*Defined at line 510*

---

## DESC_NS:

```fides
pub const DESC_NS: i64 = 0x20;
```

*Defined at line 513*

---

## DESC_AP_RW_EL1:

```fides
pub const DESC_AP_RW_EL1: i64 = 0x00;
```

*Defined at line 514*

---

## DESC_AP_RW_ALL:

```fides
pub const DESC_AP_RW_ALL: i64 = 0x40;
```

*Defined at line 515*

---

## DESC_AP_RO_EL1:

```fides
pub const DESC_AP_RO_EL1: i64 = 0x80;
```

*Defined at line 516*

---

## DESC_AP_RO_ALL:

```fides
pub const DESC_AP_RO_ALL: i64 = 0xC0;
```

*Defined at line 517*

---

## DESC_SH_NONE:

```fides
pub const DESC_SH_NONE: i64 = 0x000;
```

*Defined at line 518*

---

## DESC_SH_OUTER:

```fides
pub const DESC_SH_OUTER: i64 = 0x200;
```

*Defined at line 519*

---

## DESC_SH_INNER:

```fides
pub const DESC_SH_INNER: i64 = 0x300;
```

*Defined at line 520*

---

## DESC_AF:

```fides
pub const DESC_AF: i64 = 0x400;
```

*Defined at line 521*

---

## DESC_NG:

```fides
pub const DESC_NG: i64 = 0x800;
```

*Defined at line 522*

---

## DESC_CONT:

```fides
pub const DESC_CONT: i64 = 0x10000000000000;
```

*Defined at line 523*

---

## DESC_PXN:

```fides
pub const DESC_PXN: i64 = 0x20000000000000;
```

*Defined at line 524*

---

## DESC_UXN:

```fides
pub const DESC_UXN: i64 = 0x40000000000000;
```

*Defined at line 525*

---

## MAIR_DEVICE_NGNRNE:

MAIR attribute bytes.

```fides
pub const MAIR_DEVICE_NGNRNE: i64 = 0x00;
```

*Defined at line 528*

---

## MAIR_DEVICE_NGNRE:

```fides
pub const MAIR_DEVICE_NGNRE: i64 = 0x04;
```

*Defined at line 529*

---

## MAIR_NORMAL_NC:

```fides
pub const MAIR_NORMAL_NC: i64 = 0x44;
```

*Defined at line 530*

---

## MAIR_NORMAL_WB:

```fides
pub const MAIR_NORMAL_WB: i64 = 0xFF;
```

*Defined at line 531*

---

## a64_mair

MAIR_EL1 from up to 8 attribute bytes (index i in byte i).

```fides
pub fn a64_mair(attrs: Vec<i64>) -> i64 {
```

**Parameters:**

- `attrs`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 534*

---

## a64_tcr

TCR_EL1 for 4 KiB granules on both halves, `va_bits` of VA, inner
shareable write-back walks, physical address size code `ips`
(0 32-bit, 1 36, 2 40, 3 42, 4 44, 5 48, 6 52).

```fides
pub fn a64_tcr(va_bits: i64, ips: i64) -> i64 {
```

**Parameters:**

- `va_bits`: `i64`
- `ips`: `i64`

**Returns:** `i64`

*Defined at line 547*

---

## a64_va_index

Index of `va` in the table of level `lvl` (0..3).

```fides
pub fn a64_va_index(va: i64, lvl: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`
- `lvl`: `i64`

**Returns:** `i64`

*Defined at line 556*

---

## a64_level_size

Bytes mapped by one entry of level `lvl`: 512G, 1G, 2M, 4K.

```fides
pub fn a64_level_size(lvl: i64) -> i64 {
```

**Parameters:**

- `lvl`: `i64`

**Returns:** `i64`

*Defined at line 561*

---

## a64_table_desc

A table descriptor pointing at the next-level table at `pa`.

```fides
pub fn a64_table_desc(pa: i64) -> i64 {
```

**Parameters:**

- `pa`: `i64`

**Returns:** `i64`

*Defined at line 566*

---

## a64_leaf_desc

A block (lvl 1 or 2) or page (lvl 3) descriptor: output address `pa`,
MAIR index `attr_idx`, and the other attribute bits in `attrs` (AP, SH,
AF, nG, PXN, UXN ...).

```fides
pub fn a64_leaf_desc(lvl: i64, pa: i64, attr_idx: i64, attrs: i64) -> i64 {
```

**Parameters:**

- `lvl`: `i64`
- `pa`: `i64`
- `attr_idx`: `i64`
- `attrs`: `i64`

**Returns:** `i64`

*Defined at line 576*

---

## a64_desc_decode

Decode a descriptor at level `lvl`.

```fides
pub fn a64_desc_decode(d: i64, lvl: i64) -> str {
```

**Parameters:**

- `d`: `i64`
- `lvl`: `i64`

**Returns:** `str`

*Defined at line 592*

---

## A64Mem

Page tables in a model of physical memory: each 4 KiB table is 512
descriptors, allocated from `next_pa` upwards.

```fides
pub struct A64Mem {
```

**Fields:**

| Name | Type |
|------|------|
| `tables` | `HashMap<i64` |
| `root` | `i64` |
| `next_pa` | `i64` |

*Defined at line 645*

---

## a64mem_new

```fides
pub fn a64mem_new(table_pool_pa: i64) -> A64Mem {
```

**Parameters:**

- `table_pool_pa`: `i64`

**Returns:** `A64Mem`

*Defined at line 652*

---

## a64mem_map

Map [va, va+size) to [pa, pa+size) with `attr_idx`/`attrs`, using
1 GiB and 2 MiB blocks where both addresses are aligned and enough
remains, 4 KiB pages otherwise. Returns the number of descriptors
written.

```fides
pub fn a64mem_map(m: &mut A64Mem, va: i64, pa: i64, size: i64, attr_idx: i64, attrs: i64) -> i64 {
```

**Parameters:**

- `m`: `&mut A64Mem`
- `va`: `i64`
- `pa`: `i64`
- `size`: `i64`
- `attr_idx`: `i64`
- `attrs`: `i64`

**Returns:** `i64`

*Defined at line 686*

---

## A64Walk

Result of a translation-table walk.

```fides
pub struct A64Walk {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pa` | `i64` |
| `level` | `i64` |
| `desc` | `i64` |
| `fault` | `str` |

*Defined at line 718*

---

## a64mem_walk

Walk the tables for `va` as the MMU does (stage 1, TTBR0 region): the
output address, the level of the leaf, or the translation fault.

```fides
pub fn a64mem_walk(m: &A64Mem, va: i64) -> A64Walk {
```

**Parameters:**

- `m`: `&A64Mem`
- `va`: `i64`

**Returns:** `A64Walk`

*Defined at line 728*

---

## a64_check_access

Would an access be permitted by the leaf descriptor? `el` 0 or 1,
`write`, `exec`. Returns "" or the reason (a permission fault).

```fides
pub fn a64_check_access(d: i64, el: i64, write: bool, exec: bool) -> str {
```

**Parameters:**

- `d`: `i64`
- `el`: `i64`
- `write`: `bool`
- `exec`: `bool`

**Returns:** `str`

*Defined at line 767*

---

## GICD_CTLR:

```fides
pub const GICD_CTLR: i64 = 0x000;
```

*Defined at line 789*

---

## GICD_TYPER:

```fides
pub const GICD_TYPER: i64 = 0x004;
```

*Defined at line 790*

---

## GICD_IIDR:

```fides
pub const GICD_IIDR: i64 = 0x008;
```

*Defined at line 791*

---

## GICD_IGROUPR:

```fides
pub const GICD_IGROUPR: i64 = 0x080;
```

*Defined at line 792*

---

## GICD_ISENABLER:

```fides
pub const GICD_ISENABLER: i64 = 0x100;
```

*Defined at line 793*

---

## GICD_ICENABLER:

```fides
pub const GICD_ICENABLER: i64 = 0x180;
```

*Defined at line 794*

---

## GICD_ISPENDR:

```fides
pub const GICD_ISPENDR: i64 = 0x200;
```

*Defined at line 795*

---

## GICD_ICPENDR:

```fides
pub const GICD_ICPENDR: i64 = 0x280;
```

*Defined at line 796*

---

## GICD_ISACTIVER:

```fides
pub const GICD_ISACTIVER: i64 = 0x300;
```

*Defined at line 797*

---

## GICD_ICACTIVER:

```fides
pub const GICD_ICACTIVER: i64 = 0x380;
```

*Defined at line 798*

---

## GICD_IPRIORITYR:

```fides
pub const GICD_IPRIORITYR: i64 = 0x400;
```

*Defined at line 799*

---

## GICD_ITARGETSR:

```fides
pub const GICD_ITARGETSR: i64 = 0x800;
```

*Defined at line 800*

---

## GICD_ICFGR:

```fides
pub const GICD_ICFGR: i64 = 0xC00;
```

*Defined at line 801*

---

## GICD_SGIR:

```fides
pub const GICD_SGIR: i64 = 0xF00;
```

*Defined at line 802*

---

## GICD_IROUTER:

```fides
pub const GICD_IROUTER: i64 = 0x6100;
```

*Defined at line 803*

---

## GICC_CTLR:

```fides
pub const GICC_CTLR: i64 = 0x00;
```

*Defined at line 805*

---

## GICC_PMR:

```fides
pub const GICC_PMR: i64 = 0x04;
```

*Defined at line 806*

---

## GICC_BPR:

```fides
pub const GICC_BPR: i64 = 0x08;
```

*Defined at line 807*

---

## GICC_IAR:

```fides
pub const GICC_IAR: i64 = 0x0C;
```

*Defined at line 808*

---

## GICC_EOIR:

```fides
pub const GICC_EOIR: i64 = 0x10;
```

*Defined at line 809*

---

## GICC_RPR:

```fides
pub const GICC_RPR: i64 = 0x14;
```

*Defined at line 810*

---

## GICC_HPPIR:

```fides
pub const GICC_HPPIR: i64 = 0x18;
```

*Defined at line 811*

---

## GIC_SPURIOUS:

```fides
pub const GIC_SPURIOUS: i64 = 1023;
```

*Defined at line 813*

---

## gic_lines

Number of interrupt lines from GICD_TYPER.ITLinesNumber.

```fides
pub fn gic_lines(typer: i64) -> i64 {
```

**Parameters:**

- `typer`: `i64`

**Returns:** `i64`

*Defined at line 816*

---

## gic_kind

```fides
pub fn gic_kind(intid: i64) -> str {
```

**Parameters:**

- `intid`: `i64`

**Returns:** `str`

*Defined at line 824*

---

## gic_enable

```fides
pub fn gic_enable(bus: &mut RegBus, gicd: i64, intid: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`

*Defined at line 843*

---

## gic_disable

```fides
pub fn gic_disable(bus: &mut RegBus, gicd: i64, intid: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`

*Defined at line 847*

---

## gic_set_priority

Priority byte (byte-accessible register).

```fides
pub fn gic_set_priority(bus: &mut RegBus, gicd: i64, intid: i64, prio: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`
- `prio`: `i64`

*Defined at line 852*

---

## gic_set_targets

GICv2 target CPU mask byte of an SPI.

```fides
pub fn gic_set_targets(bus: &mut RegBus, gicd: i64, intid: i64, cpu_mask: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`
- `cpu_mask`: `i64`

*Defined at line 857*

---

## gic_set_edge

Edge (true) or level (false) triggering: bit 2n+1 of ICFGR.

```fides
pub fn gic_set_edge(bus: &mut RegBus, gicd: i64, intid: i64, edge: bool) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`
- `edge`: `bool`

*Defined at line 862*

---

## gicv2_sgir

GICD_SGIR value: filter 0 (the CPU list), 1 (all but self), 2 (self).

```fides
pub fn gicv2_sgir(intid: i64, filter: i64, cpu_list: i64) -> i64 {
```

**Parameters:**

- `intid`: `i64`
- `filter`: `i64`
- `cpu_list`: `i64`

**Returns:** `i64`

*Defined at line 875*

---

## gicv2_send_sgi

```fides
pub fn gicv2_send_sgi(bus: &mut RegBus, gicd: i64, intid: i64, cpu_list: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`
- `cpu_list`: `i64`

*Defined at line 879*

---

## gicv2_init_dist

GICv2 distributor init: off, every SPI level-triggered at priority
0xA0 targeting CPU 0 and disabled, then on.

```fides
pub fn gicv2_init_dist(bus: &mut RegBus, gicd: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`

*Defined at line 885*

---

## gicv2_init_cpu

GICv2 CPU interface init: priority mask (let everything above `pmr`
through), binary point 0, enable.

```fides
pub fn gicv2_init_cpu(bus: &mut RegBus, gicc: i64, pmr: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicc`: `i64`
- `pmr`: `i64`

*Defined at line 909*

---

## gicv2_ack

Acknowledge: the IAR value (INTID in bits 9:0, source CPU 12:10 for
SGIs). Pass the whole value to gicv2_eoi.

```fides
pub fn gicv2_ack(bus: &mut RegBus, gicc: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicc`: `i64`

**Returns:** `i64`

*Defined at line 917*

---

## gicv2_eoi

```fides
pub fn gicv2_eoi(bus: &mut RegBus, gicc: i64, iar: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicc`: `i64`
- `iar`: `i64`

*Defined at line 921*

---

## GICR_CTLR:

```fides
pub const GICR_CTLR: i64 = 0x0000;
```

*Defined at line 927*

---

## GICR_TYPER:

```fides
pub const GICR_TYPER: i64 = 0x0008;
```

*Defined at line 928*

---

## GICR_WAKER:

```fides
pub const GICR_WAKER: i64 = 0x0014;
```

*Defined at line 929*

---

## GICR_SGI_BASE:

```fides
pub const GICR_SGI_BASE: i64 = 0x10000;
```

*Defined at line 930*

---

## GICR_STRIDE:

```fides
pub const GICR_STRIDE: i64 = 0x20000;
```

*Defined at line 931*

---

## gicr_base

RD_base of the redistributor of CPU number `cpu`.

```fides
pub fn gicr_base(gicr0: i64, cpu: i64) -> i64 {
```

**Parameters:**

- `gicr0`: `i64`
- `cpu`: `i64`

**Returns:** `i64`

*Defined at line 934*

---

## gicv3_irouter

GICD_IROUTER<n> value for an SPI routed to affinity a3.a2.a1.a0
(irm: any participating PE).

```fides
pub fn gicv3_irouter(a3: i64, a2: i64, a1: i64, a0: i64, irm: bool) -> i64 {
```

**Parameters:**

- `a3`: `i64`
- `a2`: `i64`
- `a1`: `i64`
- `a0`: `i64`
- `irm`: `bool`

**Returns:** `i64`

*Defined at line 940*

---

## gicv3_sgi1r

ICC_SGI1R_EL1 for SGI `intid` to the PEs in `target_list` (bit n =
Aff0 n) of cluster aff3.aff2.aff1, or to all but self (irm).

```fides
pub fn gicv3_sgi1r(intid: i64, aff3: i64, aff2: i64, aff1: i64, target_list: i64, irm: bool) -> i64 {
```

**Parameters:**

- `intid`: `i64`
- `aff3`: `i64`
- `aff2`: `i64`
- `aff1`: `i64`
- `target_list`: `i64`
- `irm`: `bool`

**Returns:** `i64`

*Defined at line 950*

---

## gicv3_wake

Wake a redistributor: clear GICR_WAKER.ProcessorSleep (bit 1) and wait
for ChildrenAsleep (bit 2) to clear. False when it never did.

```fides
pub fn gicv3_wake(bus: &mut RegBus, rd: i64, tries: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `rd`: `i64`
- `tries`: `i64`

**Returns:** `bool`

*Defined at line 960*

---

## gicv3_enable_ppi

Enable an SGI/PPI (0..31) in a redistributor's SGI frame, in Group 1.

```fides
pub fn gicv3_enable_ppi(bus: &mut RegBus, rd: i64, intid: i64, prio: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `rd`: `i64`
- `intid`: `i64`
- `prio`: `i64`

*Defined at line 974*

---

## gicv3_route_spi

Route an SPI with GICv3 affinity routing (GICD_IROUTER<n>, 64-bit).

```fides
pub fn gicv3_route_spi(bus: &mut RegBus, gicd: i64, intid: i64, route: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `gicd`: `i64`
- `intid`: `i64`
- `route`: `i64`

*Defined at line 983*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
