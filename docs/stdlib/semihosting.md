[All modules](index.md)

# semihosting -- Fides Documentation

## Contents

- [SYS_OPEN:](#SYS_OPEN:) (const)
- [SYS_CLOSE:](#SYS_CLOSE:) (const)
- [SYS_WRITEC:](#SYS_WRITEC:) (const)
- [SYS_WRITE0:](#SYS_WRITE0:) (const)
- [SYS_WRITE:](#SYS_WRITE:) (const)
- [SYS_READ:](#SYS_READ:) (const)
- [SYS_READC:](#SYS_READC:) (const)
- [SYS_ISERROR:](#SYS_ISERROR:) (const)
- [SYS_ISTTY:](#SYS_ISTTY:) (const)
- [SYS_SEEK:](#SYS_SEEK:) (const)
- [SYS_FLEN:](#SYS_FLEN:) (const)
- [SYS_TMPNAM:](#SYS_TMPNAM:) (const)
- [SYS_REMOVE:](#SYS_REMOVE:) (const)
- [SYS_RENAME:](#SYS_RENAME:) (const)
- [SYS_CLOCK:](#SYS_CLOCK:) (const)
- [SYS_TIME:](#SYS_TIME:) (const)
- [SYS_SYSTEM:](#SYS_SYSTEM:) (const)
- [SYS_ERRNO:](#SYS_ERRNO:) (const)
- [SYS_GET_CMDLINE:](#SYS_GET_CMDLINE:) (const)
- [SYS_HEAPINFO:](#SYS_HEAPINFO:) (const)
- [SYS_EXIT:](#SYS_EXIT:) (const)
- [SYS_EXIT_EXTENDED:](#SYS_EXIT_EXTENDED:) (const)
- [SYS_ELAPSED:](#SYS_ELAPSED:) (const)
- [SYS_TICKFREQ:](#SYS_TICKFREQ:) (const)
- [ADP_STOPPED_BRANCH_THROUGH_ZERO:](#ADP_STOPPED_BRANCH_THROUGH_ZERO:) (const)
- [ADP_STOPPED_UNDEFINED_INSTR:](#ADP_STOPPED_UNDEFINED_INSTR:) (const)
- [ADP_STOPPED_SOFTWARE_INTERRUPT:](#ADP_STOPPED_SOFTWARE_INTERRUPT:) (const)
- [ADP_STOPPED_PREFETCH_ABORT:](#ADP_STOPPED_PREFETCH_ABORT:) (const)
- [ADP_STOPPED_DATA_ABORT:](#ADP_STOPPED_DATA_ABORT:) (const)
- [ADP_STOPPED_INTERNAL_ERROR:](#ADP_STOPPED_INTERNAL_ERROR:) (const)
- [ADP_STOPPED_RUNTIME_ERROR_UNKNOWN:](#ADP_STOPPED_RUNTIME_ERROR_UNKNOWN:) (const)
- [ADP_STOPPED_APPLICATION_EXIT:](#ADP_STOPPED_APPLICATION_EXIT:) (const)
- [SEMI_BKPT_THUMB:](#SEMI_BKPT_THUMB:) (const)
- [SEMI_HLT_T32:](#SEMI_HLT_T32:) (const)
- [SEMI_HLT_A32:](#SEMI_HLT_A32:) (const)
- [SEMI_HLT_A64:](#SEMI_HLT_A64:) (const)
- [SEMI_RISCV_SLLI:](#SEMI_RISCV_SLLI:) (const)
- [SEMI_RISCV_EBREAK:](#SEMI_RISCV_EBREAK:) (const)
- [SEMI_RISCV_SRAI:](#SEMI_RISCV_SRAI:) (const)
- [semi_op_name](#semi_op_name) (function)
- [semi_open_mode](#semi_open_mode) (function)
- [SemiMem](#SemiMem) (struct)
- [semimem_new](#semimem_new) (function)
- [semimem_rd8](#semimem_rd8) (function)
- [semimem_wr8](#semimem_wr8) (function)
- [semimem_rdw](#semimem_rdw) (function)
- [semimem_wrw](#semimem_wrw) (function)
- [semimem_write_bytes](#semimem_write_bytes) (function)
- [semimem_read_bytes](#semimem_read_bytes) (function)
- [semimem_cstr](#semimem_cstr) (function)
- [SemiFile](#SemiFile) (struct)
- [SemiHost](#SemiHost) (struct)
- [semihost_new](#semihost_new) (function)
- [semihost_call](#semihost_call) (function)
- [SemiTarget](#SemiTarget) (struct)
- [semitarget_new](#semitarget_new) (function)
- [semi_trap](#semi_trap) (function)
- [semi_block](#semi_block) (function)
- [semi_putc](#semi_putc) (function)
- [semi_puts](#semi_puts) (function)
- [semi_open](#semi_open) (function)
- [semi_write](#semi_write) (function)
- [semi_read](#semi_read) (function)
- [semi_close](#semi_close) (function)
- [semi_seek](#semi_seek) (function)
- [semi_flen](#semi_flen) (function)
- [semi_exit](#semi_exit) (function)
- [semi_elapsed](#semi_elapsed) (function)
- [semi_cmdline](#semi_cmdline) (function)

## SYS_OPEN:

```fides
pub const SYS_OPEN: i64 = 0x01;
```

*Defined at line 33*

---

## SYS_CLOSE:

```fides
pub const SYS_CLOSE: i64 = 0x02;
```

*Defined at line 34*

---

## SYS_WRITEC:

```fides
pub const SYS_WRITEC: i64 = 0x03;
```

*Defined at line 35*

---

## SYS_WRITE0:

```fides
pub const SYS_WRITE0: i64 = 0x04;
```

*Defined at line 36*

---

## SYS_WRITE:

```fides
pub const SYS_WRITE: i64 = 0x05;
```

*Defined at line 37*

---

## SYS_READ:

```fides
pub const SYS_READ: i64 = 0x06;
```

*Defined at line 38*

---

## SYS_READC:

```fides
pub const SYS_READC: i64 = 0x07;
```

*Defined at line 39*

---

## SYS_ISERROR:

```fides
pub const SYS_ISERROR: i64 = 0x08;
```

*Defined at line 40*

---

## SYS_ISTTY:

```fides
pub const SYS_ISTTY: i64 = 0x09;
```

*Defined at line 41*

---

## SYS_SEEK:

```fides
pub const SYS_SEEK: i64 = 0x0A;
```

*Defined at line 42*

---

## SYS_FLEN:

```fides
pub const SYS_FLEN: i64 = 0x0C;
```

*Defined at line 43*

---

## SYS_TMPNAM:

```fides
pub const SYS_TMPNAM: i64 = 0x0D;
```

*Defined at line 44*

---

## SYS_REMOVE:

```fides
pub const SYS_REMOVE: i64 = 0x0E;
```

*Defined at line 45*

---

## SYS_RENAME:

```fides
pub const SYS_RENAME: i64 = 0x0F;
```

*Defined at line 46*

---

## SYS_CLOCK:

```fides
pub const SYS_CLOCK: i64 = 0x10;
```

*Defined at line 47*

---

## SYS_TIME:

```fides
pub const SYS_TIME: i64 = 0x11;
```

*Defined at line 48*

---

## SYS_SYSTEM:

```fides
pub const SYS_SYSTEM: i64 = 0x12;
```

*Defined at line 49*

---

## SYS_ERRNO:

```fides
pub const SYS_ERRNO: i64 = 0x13;
```

*Defined at line 50*

---

## SYS_GET_CMDLINE:

```fides
pub const SYS_GET_CMDLINE: i64 = 0x15;
```

*Defined at line 51*

---

## SYS_HEAPINFO:

```fides
pub const SYS_HEAPINFO: i64 = 0x16;
```

*Defined at line 52*

---

## SYS_EXIT:

```fides
pub const SYS_EXIT: i64 = 0x18;
```

*Defined at line 53*

---

## SYS_EXIT_EXTENDED:

```fides
pub const SYS_EXIT_EXTENDED: i64 = 0x20;
```

*Defined at line 54*

---

## SYS_ELAPSED:

```fides
pub const SYS_ELAPSED: i64 = 0x30;
```

*Defined at line 55*

---

## SYS_TICKFREQ:

```fides
pub const SYS_TICKFREQ: i64 = 0x31;
```

*Defined at line 56*

---

## ADP_STOPPED_BRANCH_THROUGH_ZERO:

```fides
pub const ADP_STOPPED_BRANCH_THROUGH_ZERO: i64 = 0x20000;
```

*Defined at line 58*

---

## ADP_STOPPED_UNDEFINED_INSTR:

```fides
pub const ADP_STOPPED_UNDEFINED_INSTR: i64 = 0x20001;
```

*Defined at line 59*

---

## ADP_STOPPED_SOFTWARE_INTERRUPT:

```fides
pub const ADP_STOPPED_SOFTWARE_INTERRUPT: i64 = 0x20002;
```

*Defined at line 60*

---

## ADP_STOPPED_PREFETCH_ABORT:

```fides
pub const ADP_STOPPED_PREFETCH_ABORT: i64 = 0x20003;
```

*Defined at line 61*

---

## ADP_STOPPED_DATA_ABORT:

```fides
pub const ADP_STOPPED_DATA_ABORT: i64 = 0x20004;
```

*Defined at line 62*

---

## ADP_STOPPED_INTERNAL_ERROR:

```fides
pub const ADP_STOPPED_INTERNAL_ERROR: i64 = 0x20024;
```

*Defined at line 63*

---

## ADP_STOPPED_RUNTIME_ERROR_UNKNOWN:

```fides
pub const ADP_STOPPED_RUNTIME_ERROR_UNKNOWN: i64 = 0x20023;
```

*Defined at line 64*

---

## ADP_STOPPED_APPLICATION_EXIT:

```fides
pub const ADP_STOPPED_APPLICATION_EXIT: i64 = 0x20026;
```

*Defined at line 65*

---

## SEMI_BKPT_THUMB:

```fides
pub const SEMI_BKPT_THUMB: i64 = 0xBEAB;
```

*Defined at line 68*

---

## SEMI_HLT_T32:

```fides
pub const SEMI_HLT_T32: i64 = 0xBABC;
```

*Defined at line 69*

---

## SEMI_HLT_A32:

```fides
pub const SEMI_HLT_A32: i64 = 0xE10F0070;
```

*Defined at line 70*

---

## SEMI_HLT_A64:

```fides
pub const SEMI_HLT_A64: i64 = 0xD45E0000;
```

*Defined at line 71*

---

## SEMI_RISCV_SLLI:

```fides
pub const SEMI_RISCV_SLLI: i64 = 0x01F01013;
```

*Defined at line 72*

---

## SEMI_RISCV_EBREAK:

```fides
pub const SEMI_RISCV_EBREAK: i64 = 0x00100073;
```

*Defined at line 73*

---

## SEMI_RISCV_SRAI:

```fides
pub const SEMI_RISCV_SRAI: i64 = 0x40705013;
```

*Defined at line 74*

---

## semi_op_name

```fides
pub fn semi_op_name(op: i64) -> str {
```

**Parameters:**

- `op`: `i64`

**Returns:** `str`

*Defined at line 76*

---

## semi_open_mode

fopen() mode string to the SYS_OPEN mode number; -1 when unknown.

```fides
pub fn semi_open_mode(m: str) -> i64 {
```

**Parameters:**

- `m`: `str`

**Returns:** `i64`

*Defined at line 153*

---

## SemiMem

```fides
pub struct SemiMem {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `bytes` | `Vec<i64>` |
| `word` | `i64` |

*Defined at line 167*

---

## semimem_new

`size` bytes of target RAM at `base`; `word` 4 (AArch32) or 8 (AArch64).

```fides
pub fn semimem_new(base: i64, size: i64, word: i64) -> SemiMem {
```

**Parameters:**

- `base`: `i64`
- `size`: `i64`
- `word`: `i64`

**Returns:** `SemiMem`

*Defined at line 174*

---

## semimem_rd8

```fides
pub fn semimem_rd8(m: &SemiMem, a: i64) -> i64 {
```

**Parameters:**

- `m`: `&SemiMem`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 180*

---

## semimem_wr8

```fides
pub fn semimem_wr8(m: &mut SemiMem, a: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut SemiMem`
- `a`: `i64`
- `v`: `i64`

*Defined at line 184*

---

## semimem_rdw

A target word (little-endian; sign-extended so -1 reads as -1).

```fides
pub fn semimem_rdw(m: &SemiMem, a: i64) -> i64 {
```

**Parameters:**

- `m`: `&SemiMem`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 189*

---

## semimem_wrw

```fides
pub fn semimem_wrw(m: &mut SemiMem, a: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut SemiMem`
- `a`: `i64`
- `v`: `i64`

*Defined at line 202*

---

## semimem_write_bytes

```fides
pub fn semimem_write_bytes(m: &mut SemiMem, a: i64, b: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut SemiMem`
- `a`: `i64`
- `b`: `Vec<i64>`

*Defined at line 210*

---

## semimem_read_bytes

```fides
pub fn semimem_read_bytes(m: &SemiMem, a: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&SemiMem`
- `a`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 218*

---

## semimem_cstr

The NUL-terminated string at `a`.

```fides
pub fn semimem_cstr(m: &SemiMem, a: i64) -> str {
```

**Parameters:**

- `m`: `&SemiMem`
- `a`: `i64`

**Returns:** `str`

*Defined at line 229*

---

## SemiFile

```fides
pub struct SemiFile {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `pos` | `i64` |
| `mode` | `i64` |
| `open` | `bool` |
| `tty` | `bool` |

*Defined at line 251*

---

## SemiHost

```fides
pub struct SemiHost {
```

**Fields:**

| Name | Type |
|------|------|
| `files` | `HashMap<str` |
| `handles` | `Vec<SemiFile>` |
| `console` | `str` |
| `input` | `Vec<i64>` |
| `errno` | `i64` |
| `exited` | `bool` |
| `exit_reason` | `i64` |
| `exit_code` | `i64` |
| `clock_cs` | `i64` |
| `time` | `i64` |
| `ticks` | `i64` |
| `tick_freq` | `i64` |
| `cmdline` | `str` |
| `calls` | `Vec<str>` |

*Defined at line 259*

---

## semihost_new

```fides
pub fn semihost_new() -> SemiHost {
```

**Returns:** `SemiHost`

*Defined at line 277*

---

## semihost_call

Perform one semihosting call as the debugger does: r0 = op, r1 = the
parameter block address (or value). Returns the new r0.

```fides
pub fn semihost_call(h: &mut SemiHost, m: &mut SemiMem, op: i64, r1: i64) -> i64 {
```

**Parameters:**

- `h`: `&mut SemiHost`
- `m`: `&mut SemiMem`
- `op`: `i64`
- `r1`: `i64`

**Returns:** `i64`

*Defined at line 293*

---

## SemiTarget

Firmware view: target memory with a scratch area for parameter blocks
and strings, and the trap (here: the host model).

```fides
pub struct SemiTarget {
```

**Fields:**

| Name | Type |
|------|------|
| `mem` | `SemiMem` |
| `scratch` | `i64` |
| `host` | `SemiHost` |

*Defined at line 522*

---

## semitarget_new

```fides
pub fn semitarget_new(base: i64, size: i64, word: i64) -> SemiTarget {
```

**Parameters:**

- `base`: `i64`
- `size`: `i64`
- `word`: `i64`

**Returns:** `SemiTarget`

*Defined at line 528*

---

## semi_trap

The seam: on hardware r0 = op, r1 = arg, then the trap instruction
(SEMI_BKPT_THUMB / SEMI_HLT_A64 ...), result in r0.

```fides
pub fn semi_trap(t: &mut SemiTarget, op: i64, r1: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `op`: `i64`
- `r1`: `i64`

**Returns:** `i64`

*Defined at line 536*

---

## semi_block

Lay out a parameter block of words at the scratch area; its address.

```fides
pub fn semi_block(t: &mut SemiTarget, words: Vec<i64>) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `words`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 541*

---

## semi_putc

```fides
pub fn semi_putc(t: &mut SemiTarget, c: i64) {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `c`: `i64`

*Defined at line 559*

---

## semi_puts

```fides
pub fn semi_puts(t: &mut SemiTarget, s: str) {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `s`: `str`

*Defined at line 565*

---

## semi_open

SYS_OPEN: a handle (>= 1) or -1.

```fides
pub fn semi_open(t: &mut SemiTarget, name: str, mode: str) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `name`: `str`
- `mode`: `str`

**Returns:** `i64`

*Defined at line 572*

---

## semi_write

SYS_WRITE: bytes not written (0 = all).

```fides
pub fn semi_write(t: &mut SemiTarget, fd: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `fd`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 584*

---

## semi_read

SYS_READ up to n bytes: what was read.

```fides
pub fn semi_read(t: &mut SemiTarget, fd: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `fd`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 594*

---

## semi_close

```fides
pub fn semi_close(t: &mut SemiTarget, fd: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `fd`: `i64`

**Returns:** `i64`

*Defined at line 607*

---

## semi_seek

```fides
pub fn semi_seek(t: &mut SemiTarget, fd: i64, pos: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `fd`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 613*

---

## semi_flen

```fides
pub fn semi_flen(t: &mut SemiTarget, fd: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `fd`: `i64`

**Returns:** `i64`

*Defined at line 620*

---

## semi_exit

SYS_EXIT with ApplicationExit and a status: on AArch64 the block
{reason, status}; on AArch32 r1 is the reason (status 0 only), so a
non-zero status uses SYS_EXIT_EXTENDED.

```fides
pub fn semi_exit(t: &mut SemiTarget, status: i64) {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `status`: `i64`

*Defined at line 629*

---

## semi_elapsed

The 64-bit tick counter (SYS_ELAPSED) and its frequency.

```fides
pub fn semi_elapsed(t: &mut SemiTarget) -> i64 {
```

**Parameters:**

- `t`: `&mut SemiTarget`

**Returns:** `i64`

*Defined at line 643*

---

## semi_cmdline

```fides
pub fn semi_cmdline(t: &mut SemiTarget, cap: i64) -> str {
```

**Parameters:**

- `t`: `&mut SemiTarget`
- `cap`: `i64`

**Returns:** `str`

*Defined at line 655*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
