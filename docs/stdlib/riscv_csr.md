[All modules](index.md)

# riscv_csr -- Fides Documentation

## Contents

- [CSR_SSTATUS:](#CSR_SSTATUS:) (const)
- [CSR_SIE:](#CSR_SIE:) (const)
- [CSR_STVEC:](#CSR_STVEC:) (const)
- [CSR_SSCRATCH:](#CSR_SSCRATCH:) (const)
- [CSR_SEPC:](#CSR_SEPC:) (const)
- [CSR_SCAUSE:](#CSR_SCAUSE:) (const)
- [CSR_STVAL:](#CSR_STVAL:) (const)
- [CSR_SIP:](#CSR_SIP:) (const)
- [CSR_SATP:](#CSR_SATP:) (const)
- [CSR_MSTATUS:](#CSR_MSTATUS:) (const)
- [CSR_MISA:](#CSR_MISA:) (const)
- [CSR_MEDELEG:](#CSR_MEDELEG:) (const)
- [CSR_MIDELEG:](#CSR_MIDELEG:) (const)
- [CSR_MIE:](#CSR_MIE:) (const)
- [CSR_MTVEC:](#CSR_MTVEC:) (const)
- [CSR_MCOUNTEREN:](#CSR_MCOUNTEREN:) (const)
- [CSR_MSCRATCH:](#CSR_MSCRATCH:) (const)
- [CSR_MEPC:](#CSR_MEPC:) (const)
- [CSR_MCAUSE:](#CSR_MCAUSE:) (const)
- [CSR_MTVAL:](#CSR_MTVAL:) (const)
- [CSR_MIP:](#CSR_MIP:) (const)
- [CSR_PMPCFG0:](#CSR_PMPCFG0:) (const)
- [CSR_PMPADDR0:](#CSR_PMPADDR0:) (const)
- [CSR_MCYCLE:](#CSR_MCYCLE:) (const)
- [CSR_MINSTRET:](#CSR_MINSTRET:) (const)
- [CSR_CYCLE:](#CSR_CYCLE:) (const)
- [CSR_TIME:](#CSR_TIME:) (const)
- [CSR_INSTRET:](#CSR_INSTRET:) (const)
- [CSR_MVENDORID:](#CSR_MVENDORID:) (const)
- [CSR_MARCHID:](#CSR_MARCHID:) (const)
- [CSR_MIMPID:](#CSR_MIMPID:) (const)
- [CSR_MHARTID:](#CSR_MHARTID:) (const)
- [rv_csr_name](#rv_csr_name) (function)
- [rv_csr_readonly](#rv_csr_readonly) (function)
- [rv_csr_priv](#rv_csr_priv) (function)
- [RV_PRIV_U:](#RV_PRIV_U:) (const)
- [RV_PRIV_S:](#RV_PRIV_S:) (const)
- [RV_PRIV_M:](#RV_PRIV_M:) (const)
- [rv_priv_name](#rv_priv_name) (function)
- [MSTATUS_SIE:](#MSTATUS_SIE:) (const)
- [MSTATUS_MIE:](#MSTATUS_MIE:) (const)
- [MSTATUS_SPIE:](#MSTATUS_SPIE:) (const)
- [MSTATUS_MPIE:](#MSTATUS_MPIE:) (const)
- [MSTATUS_SPP:](#MSTATUS_SPP:) (const)
- [MSTATUS_MPP:](#MSTATUS_MPP:) (const)
- [MSTATUS_FS:](#MSTATUS_FS:) (const)
- [MSTATUS_XS:](#MSTATUS_XS:) (const)
- [MSTATUS_MPRV:](#MSTATUS_MPRV:) (const)
- [MSTATUS_SUM:](#MSTATUS_SUM:) (const)
- [MSTATUS_MXR:](#MSTATUS_MXR:) (const)
- [MSTATUS_TVM:](#MSTATUS_TVM:) (const)
- [MSTATUS_TW:](#MSTATUS_TW:) (const)
- [MSTATUS_TSR:](#MSTATUS_TSR:) (const)
- [rv_mstatus_mpp](#rv_mstatus_mpp) (function)
- [rv_mstatus_with_mpp](#rv_mstatus_with_mpp) (function)
- [rv_ext_state_name](#rv_ext_state_name) (function)
- [rv_mstatus_sd](#rv_mstatus_sd) (function)
- [rv_mstatus_decode](#rv_mstatus_decode) (function)
- [RV_IRQ_SSI:](#RV_IRQ_SSI:) (const)
- [RV_IRQ_MSI:](#RV_IRQ_MSI:) (const)
- [RV_IRQ_STI:](#RV_IRQ_STI:) (const)
- [RV_IRQ_MTI:](#RV_IRQ_MTI:) (const)
- [RV_IRQ_SEI:](#RV_IRQ_SEI:) (const)
- [RV_IRQ_MEI:](#RV_IRQ_MEI:) (const)
- [RV_IRQ_LCOFI:](#RV_IRQ_LCOFI:) (const)
- [rv_irq_name](#rv_irq_name) (function)
- [rv_ip_decode](#rv_ip_decode) (function)
- [rv_irq_select](#rv_irq_select) (function)
- [MTVEC_DIRECT:](#MTVEC_DIRECT:) (const)
- [MTVEC_VECTORED:](#MTVEC_VECTORED:) (const)
- [rv_mtvec](#rv_mtvec) (function)
- [rv_mtvec_target](#rv_mtvec_target) (function)
- [rv_mcause_is_interrupt](#rv_mcause_is_interrupt) (function)
- [rv_mcause_code](#rv_mcause_code) (function)
- [rv_mcause_interrupt](#rv_mcause_interrupt) (function)
- [rv_exception_name](#rv_exception_name) (function)
- [rv_cause_name](#rv_cause_name) (function)
- [rv_mtval_is_address](#rv_mtval_is_address) (function)
- [rv_misa_decode](#rv_misa_decode) (function)
- [rv_misa](#rv_misa) (function)
- [SATP_BARE:](#SATP_BARE:) (const)
- [SATP_SV39:](#SATP_SV39:) (const)
- [SATP_SV48:](#SATP_SV48:) (const)
- [SATP_SV57:](#SATP_SV57:) (const)
- [rv_satp](#rv_satp) (function)
- [rv_satp_decode](#rv_satp_decode) (function)
- [PMP_R:](#PMP_R:) (const)
- [PMP_W:](#PMP_W:) (const)
- [PMP_X:](#PMP_X:) (const)
- [PMP_OFF:](#PMP_OFF:) (const)
- [PMP_TOR:](#PMP_TOR:) (const)
- [PMP_NA4:](#PMP_NA4:) (const)
- [PMP_NAPOT:](#PMP_NAPOT:) (const)
- [PMP_L:](#PMP_L:) (const)
- [rv_pmpcfg](#rv_pmpcfg) (function)
- [rv_pmp_napot](#rv_pmp_napot) (function)
- [rv_pmp_napot_decode](#rv_pmp_napot_decode) (function)
- [rv_pmpcfg_decode](#rv_pmpcfg_decode) (function)
- [CSR_OP_WRITE:](#CSR_OP_WRITE:) (const)
- [CSR_OP_SET:](#CSR_OP_SET:) (const)
- [CSR_OP_CLEAR:](#CSR_OP_CLEAR:) (const)
- [CsrOp](#CsrOp) (struct)
- [csr_op](#csr_op) (function)
- [rv_csr_insn](#rv_csr_insn) (function)
- [rv_csrr](#rv_csrr) (function)
- [rv_csrw](#rv_csrw) (function)
- [RV_MRET:](#RV_MRET:) (const)
- [RV_SRET:](#RV_SRET:) (const)
- [RV_WFI:](#RV_WFI:) (const)
- [RV_ECALL:](#RV_ECALL:) (const)
- [RV_EBREAK:](#RV_EBREAK:) (const)
- [rv_csr_disasm](#rv_csr_disasm) (function)
- [rv_reg_name](#rv_reg_name) (function)
- [CsrFile](#CsrFile) (struct)
- [csrfile_new](#csrfile_new) (function)
- [csrfile_get](#csrfile_get) (function)
- [csrfile_poke](#csrfile_poke) (function)
- [csrfile_apply](#csrfile_apply) (function)
- [csrfile_run](#csrfile_run) (function)
- [csrfile_trap](#csrfile_trap) (function)
- [csrfile_check_irq](#csrfile_check_irq) (function)
- [csrfile_mret](#csrfile_mret) (function)
- [rv_trap_setup_ops](#rv_trap_setup_ops) (function)
- [plic_priority_addr](#plic_priority_addr) (function)
- [plic_pending_addr](#plic_pending_addr) (function)
- [plic_enable_addr](#plic_enable_addr) (function)
- [plic_threshold_addr](#plic_threshold_addr) (function)
- [plic_claim_addr](#plic_claim_addr) (function)
- [plic_set_priority](#plic_set_priority) (function)
- [plic_enable](#plic_enable) (function)
- [plic_disable](#plic_disable) (function)
- [plic_set_threshold](#plic_set_threshold) (function)
- [plic_claim](#plic_claim) (function)
- [plic_complete](#plic_complete) (function)
- [PlicSim](#PlicSim) (struct)
- [plicsim_new](#plicsim_new) (function)
- [plicsim_set_priority](#plicsim_set_priority) (function)
- [plicsim_enable](#plicsim_enable) (function)
- [plicsim_set_threshold](#plicsim_set_threshold) (function)
- [plicsim_raise](#plicsim_raise) (function)
- [plicsim_best](#plicsim_best) (function)
- [plicsim_eip](#plicsim_eip) (function)
- [plicsim_claim](#plicsim_claim) (function)
- [plicsim_complete](#plicsim_complete) (function)
- [clint_msip_addr](#clint_msip_addr) (function)
- [clint_mtimecmp_addr](#clint_mtimecmp_addr) (function)
- [clint_mtime_addr](#clint_mtime_addr) (function)
- [clint_send_ipi](#clint_send_ipi) (function)
- [clint_clear_ipi](#clint_clear_ipi) (function)
- [clint_set_timer64](#clint_set_timer64) (function)
- [clint_set_timer32](#clint_set_timer32) (function)
- [clint_read_mtime32](#clint_read_mtime32) (function)
- [clint_deadline](#clint_deadline) (function)

## CSR_SSTATUS:

```fides
pub const CSR_SSTATUS: i64 = 0x100;
```

*Defined at line 32*

---

## CSR_SIE:

```fides
pub const CSR_SIE: i64 = 0x104;
```

*Defined at line 33*

---

## CSR_STVEC:

```fides
pub const CSR_STVEC: i64 = 0x105;
```

*Defined at line 34*

---

## CSR_SSCRATCH:

```fides
pub const CSR_SSCRATCH: i64 = 0x140;
```

*Defined at line 35*

---

## CSR_SEPC:

```fides
pub const CSR_SEPC: i64 = 0x141;
```

*Defined at line 36*

---

## CSR_SCAUSE:

```fides
pub const CSR_SCAUSE: i64 = 0x142;
```

*Defined at line 37*

---

## CSR_STVAL:

```fides
pub const CSR_STVAL: i64 = 0x143;
```

*Defined at line 38*

---

## CSR_SIP:

```fides
pub const CSR_SIP: i64 = 0x144;
```

*Defined at line 39*

---

## CSR_SATP:

```fides
pub const CSR_SATP: i64 = 0x180;
```

*Defined at line 40*

---

## CSR_MSTATUS:

```fides
pub const CSR_MSTATUS: i64 = 0x300;
```

*Defined at line 41*

---

## CSR_MISA:

```fides
pub const CSR_MISA: i64 = 0x301;
```

*Defined at line 42*

---

## CSR_MEDELEG:

```fides
pub const CSR_MEDELEG: i64 = 0x302;
```

*Defined at line 43*

---

## CSR_MIDELEG:

```fides
pub const CSR_MIDELEG: i64 = 0x303;
```

*Defined at line 44*

---

## CSR_MIE:

```fides
pub const CSR_MIE: i64 = 0x304;
```

*Defined at line 45*

---

## CSR_MTVEC:

```fides
pub const CSR_MTVEC: i64 = 0x305;
```

*Defined at line 46*

---

## CSR_MCOUNTEREN:

```fides
pub const CSR_MCOUNTEREN: i64 = 0x306;
```

*Defined at line 47*

---

## CSR_MSCRATCH:

```fides
pub const CSR_MSCRATCH: i64 = 0x340;
```

*Defined at line 48*

---

## CSR_MEPC:

```fides
pub const CSR_MEPC: i64 = 0x341;
```

*Defined at line 49*

---

## CSR_MCAUSE:

```fides
pub const CSR_MCAUSE: i64 = 0x342;
```

*Defined at line 50*

---

## CSR_MTVAL:

```fides
pub const CSR_MTVAL: i64 = 0x343;
```

*Defined at line 51*

---

## CSR_MIP:

```fides
pub const CSR_MIP: i64 = 0x344;
```

*Defined at line 52*

---

## CSR_PMPCFG0:

```fides
pub const CSR_PMPCFG0: i64 = 0x3A0;
```

*Defined at line 53*

---

## CSR_PMPADDR0:

```fides
pub const CSR_PMPADDR0: i64 = 0x3B0;
```

*Defined at line 54*

---

## CSR_MCYCLE:

```fides
pub const CSR_MCYCLE: i64 = 0xB00;
```

*Defined at line 55*

---

## CSR_MINSTRET:

```fides
pub const CSR_MINSTRET: i64 = 0xB02;
```

*Defined at line 56*

---

## CSR_CYCLE:

```fides
pub const CSR_CYCLE: i64 = 0xC00;
```

*Defined at line 57*

---

## CSR_TIME:

```fides
pub const CSR_TIME: i64 = 0xC01;
```

*Defined at line 58*

---

## CSR_INSTRET:

```fides
pub const CSR_INSTRET: i64 = 0xC02;
```

*Defined at line 59*

---

## CSR_MVENDORID:

```fides
pub const CSR_MVENDORID: i64 = 0xF11;
```

*Defined at line 60*

---

## CSR_MARCHID:

```fides
pub const CSR_MARCHID: i64 = 0xF12;
```

*Defined at line 61*

---

## CSR_MIMPID:

```fides
pub const CSR_MIMPID: i64 = 0xF13;
```

*Defined at line 62*

---

## CSR_MHARTID:

```fides
pub const CSR_MHARTID: i64 = 0xF14;
```

*Defined at line 63*

---

## rv_csr_name

```fides
pub fn rv_csr_name(c: i64) -> str {
```

**Parameters:**

- `c`: `i64`

**Returns:** `str`

*Defined at line 65*

---

## rv_csr_readonly

CSR address bits 11:10 == 0b11: read-only (2.1).

```fides
pub fn rv_csr_readonly(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 166*

---

## rv_csr_priv

Lowest privilege level that may access the CSR: bits 9:8 (0 U, 1 S,
2 hypervisor, 3 M).

```fides
pub fn rv_csr_priv(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 172*

---

## RV_PRIV_U:

```fides
pub const RV_PRIV_U: i64 = 0;
```

*Defined at line 178*

---

## RV_PRIV_S:

```fides
pub const RV_PRIV_S: i64 = 1;
```

*Defined at line 179*

---

## RV_PRIV_M:

```fides
pub const RV_PRIV_M: i64 = 3;
```

*Defined at line 180*

---

## rv_priv_name

```fides
pub fn rv_priv_name(p: i64) -> str {
```

**Parameters:**

- `p`: `i64`

**Returns:** `str`

*Defined at line 182*

---

## MSTATUS_SIE:

```fides
pub const MSTATUS_SIE: i64 = 0x2;
```

*Defined at line 197*

---

## MSTATUS_MIE:

```fides
pub const MSTATUS_MIE: i64 = 0x8;
```

*Defined at line 198*

---

## MSTATUS_SPIE:

```fides
pub const MSTATUS_SPIE: i64 = 0x20;
```

*Defined at line 199*

---

## MSTATUS_MPIE:

```fides
pub const MSTATUS_MPIE: i64 = 0x80;
```

*Defined at line 200*

---

## MSTATUS_SPP:

```fides
pub const MSTATUS_SPP: i64 = 0x100;
```

*Defined at line 201*

---

## MSTATUS_MPP:

```fides
pub const MSTATUS_MPP: i64 = 0x1800;
```

*Defined at line 202*

---

## MSTATUS_FS:

```fides
pub const MSTATUS_FS: i64 = 0x6000;
```

*Defined at line 203*

---

## MSTATUS_XS:

```fides
pub const MSTATUS_XS: i64 = 0x18000;
```

*Defined at line 204*

---

## MSTATUS_MPRV:

```fides
pub const MSTATUS_MPRV: i64 = 0x20000;
```

*Defined at line 205*

---

## MSTATUS_SUM:

```fides
pub const MSTATUS_SUM: i64 = 0x40000;
```

*Defined at line 206*

---

## MSTATUS_MXR:

```fides
pub const MSTATUS_MXR: i64 = 0x80000;
```

*Defined at line 207*

---

## MSTATUS_TVM:

```fides
pub const MSTATUS_TVM: i64 = 0x100000;
```

*Defined at line 208*

---

## MSTATUS_TW:

```fides
pub const MSTATUS_TW: i64 = 0x200000;
```

*Defined at line 209*

---

## MSTATUS_TSR:

```fides
pub const MSTATUS_TSR: i64 = 0x400000;
```

*Defined at line 210*

---

## rv_mstatus_mpp

```fides
pub fn rv_mstatus_mpp(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 212*

---

## rv_mstatus_with_mpp

```fides
pub fn rv_mstatus_with_mpp(v: i64, p: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `p`: `i64`

**Returns:** `i64`

*Defined at line 216*

---

## rv_ext_state_name

FS/VS/XS states: 0 Off, 1 Initial, 2 Clean, 3 Dirty.

```fides
pub fn rv_ext_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 221*

---

## rv_mstatus_sd

SD for RV64 (bit 63): set when FS, VS or XS is Dirty.

```fides
pub fn rv_mstatus_sd(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 235*

---

## rv_mstatus_decode

Decode mstatus: "MIE=1 MPIE=0 MPP=M SPP=U FS=Dirty SD=1".

```fides
pub fn rv_mstatus_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 240*

---

## RV_IRQ_SSI:

```fides
pub const RV_IRQ_SSI: i64 = 1;
```

*Defined at line 271*

---

## RV_IRQ_MSI:

```fides
pub const RV_IRQ_MSI: i64 = 3;
```

*Defined at line 272*

---

## RV_IRQ_STI:

```fides
pub const RV_IRQ_STI: i64 = 5;
```

*Defined at line 273*

---

## RV_IRQ_MTI:

```fides
pub const RV_IRQ_MTI: i64 = 7;
```

*Defined at line 274*

---

## RV_IRQ_SEI:

```fides
pub const RV_IRQ_SEI: i64 = 9;
```

*Defined at line 275*

---

## RV_IRQ_MEI:

```fides
pub const RV_IRQ_MEI: i64 = 11;
```

*Defined at line 276*

---

## RV_IRQ_LCOFI:

```fides
pub const RV_IRQ_LCOFI: i64 = 13;
```

*Defined at line 277*

---

## rv_irq_name

```fides
pub fn rv_irq_name(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 279*

---

## rv_ip_decode

The short names of the bits set in mie/mip: "MTIE MEIE" style.

```fides
pub fn rv_ip_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 308*

---

## rv_irq_select

The interrupt taken among pending & enabled ones, in the priority order
of 3.1.9 (MEI, MSI, MTI, SEI, SSI, STI, LCOFI); -1 when none.

```fides
pub fn rv_irq_select(mip: i64, mie: i64) -> i64 {
```

**Parameters:**

- `mip`: `i64`
- `mie`: `i64`

**Returns:** `i64`

*Defined at line 336*

---

## MTVEC_DIRECT:

```fides
pub const MTVEC_DIRECT: i64 = 0;
```

*Defined at line 368*

---

## MTVEC_VECTORED:

```fides
pub const MTVEC_VECTORED: i64 = 1;
```

*Defined at line 369*

---

## rv_mtvec

mtvec value: BASE must be 4-byte aligned.

```fides
pub fn rv_mtvec(base: i64, mode: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 372*

---

## rv_mtvec_target

Where a trap with `mcause` goes: BASE for exceptions and in direct
mode, BASE + 4 * cause for interrupts in vectored mode.

```fides
pub fn rv_mtvec_target(mtvec: i64, mcause: i64, xlen: i64) -> i64 {
```

**Parameters:**

- `mtvec`: `i64`
- `mcause`: `i64`
- `xlen`: `i64`

**Returns:** `i64`

*Defined at line 384*

---

## rv_mcause_is_interrupt

```fides
pub fn rv_mcause_is_interrupt(c: i64, xlen: i64) -> bool {
```

**Parameters:**

- `c`: `i64`
- `xlen`: `i64`

**Returns:** `bool`

*Defined at line 394*

---

## rv_mcause_code

```fides
pub fn rv_mcause_code(c: i64, xlen: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`
- `xlen`: `i64`

**Returns:** `i64`

*Defined at line 398*

---

## rv_mcause_interrupt

mcause for an interrupt `code` on an `xlen` hart.

```fides
pub fn rv_mcause_interrupt(code: i64, xlen: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`
- `xlen`: `i64`

**Returns:** `i64`

*Defined at line 406*

---

## rv_exception_name

```fides
pub fn rv_exception_name(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 410*

---

## rv_cause_name

The trap named by mcause.

```fides
pub fn rv_cause_name(c: i64, xlen: i64) -> str {
```

**Parameters:**

- `c`: `i64`
- `xlen`: `i64`

**Returns:** `str`

*Defined at line 466*

---

## rv_mtval_is_address

Exceptions whose mtval holds the faulting address (misaligned, access
and page faults; breakpoints may too).

```fides
pub fn rv_mtval_is_address(code: i64) -> bool {
```

**Parameters:**

- `code`: `i64`

**Returns:** `bool`

*Defined at line 475*

---

## rv_misa_decode

"RV64IMAFDCSU": MXL and the extension letters, in alphabet order except
that the ISA string puts I/E first as the spec writes it.

```fides
pub fn rv_misa_decode(v: i64, xlen: i64) -> str {
```

**Parameters:**

- `v`: `i64`
- `xlen`: `i64`

**Returns:** `str`

*Defined at line 483*

---

## rv_misa

Build misa from MXL and a letter string ("IMAC").

```fides
pub fn rv_misa(mxl: i64, exts: str, xlen: i64) -> i64 {
```

**Parameters:**

- `mxl`: `i64`
- `exts`: `str`
- `xlen`: `i64`

**Returns:** `i64`

*Defined at line 515*

---

## SATP_BARE:

```fides
pub const SATP_BARE: i64 = 0;
```

*Defined at line 530*

---

## SATP_SV39:

```fides
pub const SATP_SV39: i64 = 8;
```

*Defined at line 531*

---

## SATP_SV48:

```fides
pub const SATP_SV48: i64 = 9;
```

*Defined at line 532*

---

## SATP_SV57:

```fides
pub const SATP_SV57: i64 = 10;
```

*Defined at line 533*

---

## rv_satp

RV64 satp: MODE[63:60] ASID[59:44] PPN[43:0], from the root page
table's physical address.

```fides
pub fn rv_satp(mode: i64, asid: i64, root_pa: i64) -> i64 {
```

**Parameters:**

- `mode`: `i64`
- `asid`: `i64`
- `root_pa`: `i64`

**Returns:** `i64`

*Defined at line 537*

---

## rv_satp_decode

```fides
pub fn rv_satp_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 544*

---

## PMP_R:

```fides
pub const PMP_R: i64 = 1;
```

*Defined at line 564*

---

## PMP_W:

```fides
pub const PMP_W: i64 = 2;
```

*Defined at line 565*

---

## PMP_X:

```fides
pub const PMP_X: i64 = 4;
```

*Defined at line 566*

---

## PMP_OFF:

```fides
pub const PMP_OFF: i64 = 0;
```

*Defined at line 567*

---

## PMP_TOR:

```fides
pub const PMP_TOR: i64 = 1;
```

*Defined at line 568*

---

## PMP_NA4:

```fides
pub const PMP_NA4: i64 = 2;
```

*Defined at line 569*

---

## PMP_NAPOT:

```fides
pub const PMP_NAPOT: i64 = 3;
```

*Defined at line 570*

---

## PMP_L:

```fides
pub const PMP_L: i64 = 0x80;
```

*Defined at line 571*

---

## rv_pmpcfg

One pmpNcfg byte.

```fides
pub fn rv_pmpcfg(perms: i64, amode: i64, locked: bool) -> i64 {
```

**Parameters:**

- `perms`: `i64`
- `amode`: `i64`
- `locked`: `bool`

**Returns:** `i64`

*Defined at line 574*

---

## rv_pmp_napot

pmpaddr for a NAPOT region (size a power of two >= 8, base aligned):
(base >> 2) with size/8 - 1 in the low bits.

```fides
pub fn rv_pmp_napot(base: i64, size: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 584*

---

## rv_pmp_napot_decode

Decode a NAPOT pmpaddr: base * 2^32-free form "base 0x... size N".

```fides
pub fn rv_pmp_napot_decode(addr: i64) -> str {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `str`

*Defined at line 595*

---

## rv_pmpcfg_decode

A pmpcfg byte as text: "NAPOT RW- L".

```fides
pub fn rv_pmpcfg_decode(c: i64) -> str {
```

**Parameters:**

- `c`: `i64`

**Returns:** `str`

*Defined at line 606*

---

## CSR_OP_WRITE:

```fides
pub const CSR_OP_WRITE: i64 = 1;
```

*Defined at line 642*

---

## CSR_OP_SET:

```fides
pub const CSR_OP_SET: i64 = 2;
```

*Defined at line 643*

---

## CSR_OP_CLEAR:

```fides
pub const CSR_OP_CLEAR: i64 = 3;
```

*Defined at line 644*

---

## CsrOp

One CSR access: write `value`, set its bits or clear its bits.

```fides
pub struct CsrOp {
```

**Fields:**

| Name | Type |
|------|------|
| `op` | `i64` |
| `csr` | `i64` |
| `value` | `i64` |

*Defined at line 647*

---

## csr_op

```fides
pub fn csr_op(op: i64, csr: i64, value: i64) -> CsrOp {
```

**Parameters:**

- `op`: `i64`
- `csr`: `i64`
- `value`: `i64`

**Returns:** `CsrOp`

*Defined at line 653*

---

## rv_csr_insn

Encode csrrw/csrrs/csrrc rd, csr, rs1 (funct3 1/2/3) or, with `imm`,
csrrwi/csrrsi/csrrci rd, csr, uimm (funct3 5/6/7, rs1 holds uimm[4:0]).

```fides
pub fn rv_csr_insn(op: i64, rd: i64, csr: i64, rs1: i64, imm: bool) -> i64 {
```

**Parameters:**

- `op`: `i64`
- `rd`: `i64`
- `csr`: `i64`
- `rs1`: `i64`
- `imm`: `bool`

**Returns:** `i64`

*Defined at line 661*

---

## rv_csrr

csrr rd, csr (csrrs rd, csr, x0).

```fides
pub fn rv_csrr(rd: i64, csr: i64) -> i64 {
```

**Parameters:**

- `rd`: `i64`
- `csr`: `i64`

**Returns:** `i64`

*Defined at line 670*

---

## rv_csrw

csrw csr, rs (csrrw x0, csr, rs).

```fides
pub fn rv_csrw(csr: i64, rs: i64) -> i64 {
```

**Parameters:**

- `csr`: `i64`
- `rs`: `i64`

**Returns:** `i64`

*Defined at line 675*

---

## RV_MRET:

```fides
pub const RV_MRET: i64 = 0x30200073;
```

*Defined at line 679*

---

## RV_SRET:

```fides
pub const RV_SRET: i64 = 0x10200073;
```

*Defined at line 680*

---

## RV_WFI:

```fides
pub const RV_WFI: i64 = 0x10500073;
```

*Defined at line 681*

---

## RV_ECALL:

```fides
pub const RV_ECALL: i64 = 0x00000073;
```

*Defined at line 682*

---

## RV_EBREAK:

```fides
pub const RV_EBREAK: i64 = 0x00100073;
```

*Defined at line 683*

---

## rv_csr_disasm

Decode a Zicsr instruction: "csrrs a0, mhartid, zero". "" when it is
not one.

```fides
pub fn rv_csr_disasm(insn: i64) -> str {
```

**Parameters:**

- `insn`: `i64`

**Returns:** `str`

*Defined at line 687*

---

## rv_reg_name

ABI register names x0..x31.

```fides
pub fn rv_reg_name(r: i64) -> str {
```

**Parameters:**

- `r`: `i64`

**Returns:** `str`

*Defined at line 709*

---

## CsrFile

CSR values with the write masks (WARL) of the M-mode registers, the
current privilege level and the pc for trap modelling.

```fides
pub struct CsrFile {
```

**Fields:**

| Name | Type |
|------|------|
| `xlen` | `i64` |
| `vals` | `HashMap<i64` |
| `wmask` | `HashMap<i64` |
| `priv_` | `i64` |
| `pc` | `i64` |

*Defined at line 718*

---

## csrfile_new

A hart in M-mode after reset: misa as given, mstatus.MPP = M,
everything else 0. Writable bits: mstatus (SIE MIE SPIE MPIE SPP MPP
FS MPRV SUM MXR TVM TW TSR), mie/mip's standard bits (mip: only the
S-level pending bits are writable from software), mtvec with mode 0/1,
mepc with bit 0 clear (IALIGN 16, C extension), mscratch, mcause, mtval.

```fides
pub fn csrfile_new(xlen: i64, misa: i64, hartid: i64) -> CsrFile {
```

**Parameters:**

- `xlen`: `i64`
- `misa`: `i64`
- `hartid`: `i64`

**Returns:** `CsrFile`

*Defined at line 733*

---

## csrfile_get

```fides
pub fn csrfile_get(f: &CsrFile, csr: i64) -> i64 {
```

**Parameters:**

- `f`: `&CsrFile`
- `csr`: `i64`

**Returns:** `i64`

*Defined at line 762*

---

## csrfile_poke

Hardware-side set (a timer or the PLIC raising mip bits).

```fides
pub fn csrfile_poke(f: &mut CsrFile, csr: i64, v: i64) {
```

**Parameters:**

- `f`: `&mut CsrFile`
- `csr`: `i64`
- `v`: `i64`

*Defined at line 770*

---

## csrfile_apply

Apply one CsrOp as the hart would: an access below the CSR's privilege
or a write to a read-only CSR is an illegal-instruction trap (result
-1, nothing changed; csrrs/csrrc with value 0 do not write and are
allowed on read-only CSRs). Otherwise the old value is returned and
the new one stored through the WARL mask (mtvec bit 1 is not
writable: modes 2 and 3 are reserved).

```fides
pub fn csrfile_apply(f: &mut CsrFile, o: CsrOp) -> i64 {
```

**Parameters:**

- `f`: `&mut CsrFile`
- `o`: `CsrOp`

**Returns:** `i64`

*Defined at line 780*

---

## csrfile_run

Run a sequence of ops; the number that trapped.

```fides
pub fn csrfile_run(f: &mut CsrFile, ops: Vec<CsrOp>) -> i64 {
```

**Parameters:**

- `f`: `&mut CsrFile`
- `ops`: `Vec<CsrOp>`

**Returns:** `i64`

*Defined at line 809*

---

## csrfile_trap

Trap into M-mode (3.1.6.1): mepc = pc, mcause, mtval, MPIE = MIE,
MIE = 0, MPP = the privilege trapped from, privilege = M, pc = the
mtvec target. Returns the new pc.

```fides
pub fn csrfile_trap(f: &mut CsrFile, mcause: i64, mtval: i64) -> i64 {
```

**Parameters:**

- `f`: `&mut CsrFile`
- `mcause`: `i64`
- `mtval`: `i64`

**Returns:** `i64`

*Defined at line 824*

---

## csrfile_check_irq

Take the pending interrupt if mstatus.MIE (or a lower privilege) lets
it through: returns the interrupt code taken, -1 when none.

```fides
pub fn csrfile_check_irq(f: &mut CsrFile) -> i64 {
```

**Parameters:**

- `f`: `&mut CsrFile`

**Returns:** `i64`

*Defined at line 844*

---

## csrfile_mret

MRET (3.3.2): privilege = MPP, MIE = MPIE, MPIE = 1, MPP = U (the
least privileged mode, U supported), MPRV cleared when leaving M;
pc = mepc. Returns the new pc, -1 (illegal) outside M-mode.

```fides
pub fn csrfile_mret(f: &mut CsrFile) -> i64 {
```

**Parameters:**

- `f`: `&mut CsrFile`

**Returns:** `i64`

*Defined at line 861*

---

## rv_trap_setup_ops

The standard M-mode trap setup a boot stub runs: mtvec, mscratch, mie
bits, global MIE, as CsrOps (to encode with rv_csr_insn or apply).

```fides
pub fn rv_trap_setup_ops(handler: i64, vectored: bool, scratch: i64, mie_bits: i64) -> Vec<CsrOp> {
```

**Parameters:**

- `handler`: `i64`
- `vectored`: `bool`
- `scratch`: `i64`
- `mie_bits`: `i64`

**Returns:** `Vec<CsrOp>`

*Defined at line 885*

---

## plic_priority_addr

```fides
pub fn plic_priority_addr(base: i64, src: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `src`: `i64`

**Returns:** `i64`

*Defined at line 900*

---

## plic_pending_addr

```fides
pub fn plic_pending_addr(base: i64, src: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `src`: `i64`

**Returns:** `i64`

*Defined at line 904*

---

## plic_enable_addr

```fides
pub fn plic_enable_addr(base: i64, ctx: i64, src: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `ctx`: `i64`
- `src`: `i64`

**Returns:** `i64`

*Defined at line 908*

---

## plic_threshold_addr

```fides
pub fn plic_threshold_addr(base: i64, ctx: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `ctx`: `i64`

**Returns:** `i64`

*Defined at line 912*

---

## plic_claim_addr

```fides
pub fn plic_claim_addr(base: i64, ctx: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `ctx`: `i64`

**Returns:** `i64`

*Defined at line 916*

---

## plic_set_priority

```fides
pub fn plic_set_priority(bus: &mut RegBus, base: i64, src: i64, prio: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `src`: `i64`
- `prio`: `i64`

*Defined at line 926*

---

## plic_enable

Enable a source for a context (read-modify-write of its enable word).

```fides
pub fn plic_enable(bus: &mut RegBus, base: i64, ctx: i64, src: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `ctx`: `i64`
- `src`: `i64`

*Defined at line 932*

---

## plic_disable

```fides
pub fn plic_disable(bus: &mut RegBus, base: i64, ctx: i64, src: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `ctx`: `i64`
- `src`: `i64`

*Defined at line 938*

---

## plic_set_threshold

```fides
pub fn plic_set_threshold(bus: &mut RegBus, base: i64, ctx: i64, t: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `ctx`: `i64`
- `t`: `i64`

*Defined at line 944*

---

## plic_claim

Claim: read the claim register (0 when nothing is pending).

```fides
pub fn plic_claim(bus: &mut RegBus, base: i64, ctx: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `ctx`: `i64`

**Returns:** `i64`

*Defined at line 949*

---

## plic_complete

Complete: write the claimed ID back.

```fides
pub fn plic_complete(bus: &mut RegBus, base: i64, ctx: i64, src: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `ctx`: `i64`
- `src`: `i64`

*Defined at line 954*

---

## PlicSim

The PLIC's arbitration, as the hardware does it.

```fides
pub struct PlicSim {
```

**Fields:**

| Name | Type |
|------|------|
| `nsrc` | `i64` |
| `nctx` | `i64` |
| `prio` | `Vec<i64>` |
| `pending` | `Vec<bool>` |
| `claimed` | `Vec<bool>` |
| `enable` | `Vec<bool>` |
| `threshold` | `Vec<i64>` |

*Defined at line 959*

---

## plicsim_new

```fides
pub fn plicsim_new(nsrc: i64, nctx: i64) -> PlicSim {
```

**Parameters:**

- `nsrc`: `i64`
- `nctx`: `i64`

**Returns:** `PlicSim`

*Defined at line 969*

---

## plicsim_set_priority

```fides
pub fn plicsim_set_priority(p: &mut PlicSim, src: i64, prio: i64) {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `src`: `i64`
- `prio`: `i64`

*Defined at line 975*

---

## plicsim_enable

```fides
pub fn plicsim_enable(p: &mut PlicSim, ctx: i64, src: i64, on: bool) {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `ctx`: `i64`
- `src`: `i64`
- `on`: `bool`

*Defined at line 978*

---

## plicsim_set_threshold

```fides
pub fn plicsim_set_threshold(p: &mut PlicSim, ctx: i64, t: i64) {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `ctx`: `i64`
- `t`: `i64`

*Defined at line 981*

---

## plicsim_raise

A device raises source `src` (the gateway holds one request until it
has been claimed and completed).

```fides
pub fn plicsim_raise(p: &mut PlicSim, src: i64) {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `src`: `i64`

*Defined at line 987*

---

## plicsim_best

The source the context's claim would return: the pending, enabled
source with the highest priority strictly above the threshold, the
lowest ID first on a tie; 0 when none. Priority 0 never interrupts.

```fides
pub fn plicsim_best(p: &PlicSim, ctx: i64) -> i64 {
```

**Parameters:**

- `p`: `&PlicSim`
- `ctx`: `i64`

**Returns:** `i64`

*Defined at line 996*

---

## plicsim_eip

The context's external interrupt line (MEIP/SEIP).

```fides
pub fn plicsim_eip(p: &PlicSim, ctx: i64) -> bool {
```

**Parameters:**

- `p`: `&PlicSim`
- `ctx`: `i64`

**Returns:** `bool`

*Defined at line 1012*

---

## plicsim_claim

Claim: returns the ID and clears its pending bit.

```fides
pub fn plicsim_claim(p: &mut PlicSim, ctx: i64) -> i64 {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `ctx`: `i64`

**Returns:** `i64`

*Defined at line 1017*

---

## plicsim_complete

Complete: the gateway may forward the next request of that source.

```fides
pub fn plicsim_complete(p: &mut PlicSim, src: i64) {
```

**Parameters:**

- `p`: `&mut PlicSim`
- `src`: `i64`

*Defined at line 1027*

---

## clint_msip_addr

```fides
pub fn clint_msip_addr(base: i64, hart: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `hart`: `i64`

**Returns:** `i64`

*Defined at line 1035*

---

## clint_mtimecmp_addr

```fides
pub fn clint_mtimecmp_addr(base: i64, hart: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `hart`: `i64`

**Returns:** `i64`

*Defined at line 1039*

---

## clint_mtime_addr

```fides
pub fn clint_mtime_addr(base: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`

**Returns:** `i64`

*Defined at line 1043*

---

## clint_send_ipi

Raise / clear a hart's machine software interrupt (MSIP).

```fides
pub fn clint_send_ipi(bus: &mut RegBus, base: i64, hart: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `hart`: `i64`

*Defined at line 1048*

---

## clint_clear_ipi

```fides
pub fn clint_clear_ipi(bus: &mut RegBus, base: i64, hart: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `hart`: `i64`

*Defined at line 1052*

---

## clint_set_timer64

Set mtimecmp on RV64: one 64-bit store.

```fides
pub fn clint_set_timer64(bus: &mut RegBus, base: i64, hart: i64, when: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `hart`: `i64`
- `when`: `i64`

*Defined at line 1057*

---

## clint_set_timer32

Set mtimecmp with 32-bit stores (RV32, Privileged spec 3.2.1): low
word to all ones first so no spurious interrupt fires between the two
halves, then the high word, then the low word.

```fides
pub fn clint_set_timer32(bus: &mut RegBus, base: i64, hart: i64, when: i64) {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`
- `hart`: `i64`
- `when`: `i64`

*Defined at line 1064*

---

## clint_read_mtime32

Read mtime with 32-bit loads: high, low, high again, retried until the
high word did not change (a carry between the reads).

```fides
pub fn clint_read_mtime32(bus: &mut RegBus, base: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut RegBus`
- `base`: `i64`

**Returns:** `i64`

*Defined at line 1073*

---

## clint_deadline

mtimecmp for a tick `ms` milliseconds after `now` on a `freq` Hz
timebase.

```fides
pub fn clint_deadline(now: i64, ms: i64, freq: i64) -> i64 {
```

**Parameters:**

- `now`: `i64`
- `ms`: `i64`
- `freq`: `i64`

**Returns:** `i64`

*Defined at line 1090*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
