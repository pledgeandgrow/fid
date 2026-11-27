[All modules](index.md)

# x86_cpu -- Fides Documentation

## Contents

- [CpuidRegs](#CpuidRegs) (struct)
- [cpuid_regs](#cpuid_regs) (function)
- [cpuid_vendor](#cpuid_vendor) (function)
- [cpuid_family](#cpuid_family) (function)
- [cpuid_model](#cpuid_model) (function)
- [cpuid_stepping](#cpuid_stepping) (function)
- [cpuid_apic_id](#cpuid_apic_id) (function)
- [cpuid_logical_count](#cpuid_logical_count) (function)
- [cpuid_clflush_size](#cpuid_clflush_size) (function)
- [cpuid_features_edx](#cpuid_features_edx) (function)
- [cpuid_features_ecx](#cpuid_features_ecx) (function)
- [cpuid_features_7ebx](#cpuid_features_7ebx) (function)
- [cpuid_features_7ecx](#cpuid_features_7ecx) (function)
- [cpuid_features_ext_edx](#cpuid_features_ext_edx) (function)
- [cpuid_features_ext_ecx](#cpuid_features_ext_ecx) (function)
- [cpuid_has](#cpuid_has) (function)
- [cpuid_brand](#cpuid_brand) (function)
- [cpuid_signature](#cpuid_signature) (function)
- [MSR_IA32_TSC:](#MSR_IA32_TSC:) (const)
- [MSR_IA32_APIC_BASE:](#MSR_IA32_APIC_BASE:) (const)
- [MSR_IA32_FEATURE_CONTROL:](#MSR_IA32_FEATURE_CONTROL:) (const)
- [MSR_IA32_MTRRCAP:](#MSR_IA32_MTRRCAP:) (const)
- [MSR_IA32_SYSENTER_CS:](#MSR_IA32_SYSENTER_CS:) (const)
- [MSR_IA32_SYSENTER_ESP:](#MSR_IA32_SYSENTER_ESP:) (const)
- [MSR_IA32_SYSENTER_EIP:](#MSR_IA32_SYSENTER_EIP:) (const)
- [MSR_IA32_MISC_ENABLE:](#MSR_IA32_MISC_ENABLE:) (const)
- [MSR_IA32_PAT:](#MSR_IA32_PAT:) (const)
- [MSR_IA32_TSC_DEADLINE:](#MSR_IA32_TSC_DEADLINE:) (const)
- [MSR_X2APIC_BASE:](#MSR_X2APIC_BASE:) (const)
- [MSR_EFER:](#MSR_EFER:) (const)
- [MSR_STAR:](#MSR_STAR:) (const)
- [MSR_LSTAR:](#MSR_LSTAR:) (const)
- [MSR_CSTAR:](#MSR_CSTAR:) (const)
- [MSR_FMASK:](#MSR_FMASK:) (const)
- [MSR_FS_BASE:](#MSR_FS_BASE:) (const)
- [MSR_GS_BASE:](#MSR_GS_BASE:) (const)
- [MSR_KERNEL_GS_BASE:](#MSR_KERNEL_GS_BASE:) (const)
- [MSR_TSC_AUX:](#MSR_TSC_AUX:) (const)
- [EFER_SCE:](#EFER_SCE:) (const)
- [EFER_LME:](#EFER_LME:) (const)
- [EFER_LMA:](#EFER_LMA:) (const)
- [EFER_NXE:](#EFER_NXE:) (const)
- [x86_msr_name](#x86_msr_name) (function)
- [x86_efer_decode](#x86_efer_decode) (function)
- [x86_star](#x86_star) (function)
- [x86_star_decode](#x86_star_decode) (function)
- [x86_apic_base_decode](#x86_apic_base_decode) (function)
- [x86_memtype_name](#x86_memtype_name) (function)
- [x86_pat](#x86_pat) (function)
- [x86_pat_decode](#x86_pat_decode) (function)
- [SEG_KERNEL_CODE:](#SEG_KERNEL_CODE:) (const)
- [SEG_KERNEL_DATA:](#SEG_KERNEL_DATA:) (const)
- [SEG_USER_CODE:](#SEG_USER_CODE:) (const)
- [SEG_USER_DATA:](#SEG_USER_DATA:) (const)
- [SEG_TSS64:](#SEG_TSS64:) (const)
- [SEG_FLAGS_LONG:](#SEG_FLAGS_LONG:) (const)
- [SEG_FLAGS_32:](#SEG_FLAGS_32:) (const)
- [gdt_entry](#gdt_entry) (function)
- [gdt_decode](#gdt_decode) (function)
- [tss_desc_low](#tss_desc_low) (function)
- [tss_desc_high](#tss_desc_high) (function)
- [x86_selector](#x86_selector) (function)
- [GATE_INTERRUPT:](#GATE_INTERRUPT:) (const)
- [GATE_TRAP:](#GATE_TRAP:) (const)
- [idt_gate_low](#idt_gate_low) (function)
- [idt_gate_high](#idt_gate_high) (function)
- [idt_gate_target](#idt_gate_target) (function)
- [x86_vector_name](#x86_vector_name) (function)
- [x86_has_error_code](#x86_has_error_code) (function)
- [x86_pf_error_decode](#x86_pf_error_decode) (function)
- [x86_put_le](#x86_put_le) (function)
- [x86_table_bytes](#x86_table_bytes) (function)
- [x86_pseudo_desc](#x86_pseudo_desc) (function)
- [x86_tss_bytes](#x86_tss_bytes) (function)
- [x86_standard_gdt](#x86_standard_gdt) (function)
- [PortIo](#PortIo) (struct)
- [portio_fake](#portio_fake) (function)
- [portio_real](#portio_real) (function)
- [portio_in](#portio_in) (function)
- [portio_out](#portio_out) (function)
- [x86_inb](#x86_inb) (function)
- [x86_outb](#x86_outb) (function)
- [PIC1_CMD:](#PIC1_CMD:) (const)
- [PIC1_DATA:](#PIC1_DATA:) (const)
- [PIC2_CMD:](#PIC2_CMD:) (const)
- [PIC2_DATA:](#PIC2_DATA:) (const)
- [PIC_EOI:](#PIC_EOI:) (const)
- [pic_remap](#pic_remap) (function)
- [pic_set_mask](#pic_set_mask) (function)
- [pic_eoi](#pic_eoi) (function)
- [PIT_HZ:](#PIT_HZ:) (const)
- [pit_divisor](#pit_divisor) (function)
- [pit_set_rate](#pit_set_rate) (function)
- [cmos_read](#cmos_read) (function)
- [cmos_write](#cmos_write) (function)

## CpuidRegs

The registers one CPUID leaf returned.

```fides
pub struct CpuidRegs {
```

**Fields:**

| Name | Type |
|------|------|
| `eax` | `i64` |
| `ebx` | `i64` |
| `ecx` | `i64` |
| `edx` | `i64` |

*Defined at line 34*

---

## cpuid_regs

```fides
pub fn cpuid_regs(eax: i64, ebx: i64, ecx: i64, edx: i64) -> CpuidRegs {
```

**Parameters:**

- `eax`: `i64`
- `ebx`: `i64`
- `ecx`: `i64`
- `edx`: `i64`

**Returns:** `CpuidRegs`

*Defined at line 41*

---

## cpuid_vendor

Leaf 0: the vendor string, EBX EDX ECX ("GenuineIntel", "AuthenticAMD").

```fides
pub fn cpuid_vendor(l0: CpuidRegs) -> str {
```

**Parameters:**

- `l0`: `CpuidRegs`

**Returns:** `str`

*Defined at line 61*

---

## cpuid_family

Leaf 1 EAX: display family (base + extended when base is 0xF).

```fides
pub fn cpuid_family(eax: i64) -> i64 {
```

**Parameters:**

- `eax`: `i64`

**Returns:** `i64`

*Defined at line 66*

---

## cpuid_model

Display model: extended model << 4 added for families 6 and 0xF.

```fides
pub fn cpuid_model(eax: i64) -> i64 {
```

**Parameters:**

- `eax`: `i64`

**Returns:** `i64`

*Defined at line 75*

---

## cpuid_stepping

```fides
pub fn cpuid_stepping(eax: i64) -> i64 {
```

**Parameters:**

- `eax`: `i64`

**Returns:** `i64`

*Defined at line 84*

---

## cpuid_apic_id

Leaf 1 EBX: initial APIC ID (31:24) and logical processor count (23:16).

```fides
pub fn cpuid_apic_id(ebx: i64) -> i64 {
```

**Parameters:**

- `ebx`: `i64`

**Returns:** `i64`

*Defined at line 89*

---

## cpuid_logical_count

```fides
pub fn cpuid_logical_count(ebx: i64) -> i64 {
```

**Parameters:**

- `ebx`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## cpuid_clflush_size

CLFLUSH line size in bytes (EBX 15:8 * 8).

```fides
pub fn cpuid_clflush_size(ebx: i64) -> i64 {
```

**Parameters:**

- `ebx`: `i64`

**Returns:** `i64`

*Defined at line 98*

---

## cpuid_features_edx

Leaf 1 EDX feature names.

```fides
pub fn cpuid_features_edx(edx: i64) -> Vec<str> {
```

**Parameters:**

- `edx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 117*

---

## cpuid_features_ecx

Leaf 1 ECX feature names.

```fides
pub fn cpuid_features_ecx(ecx: i64) -> Vec<str> {
```

**Parameters:**

- `ecx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 122*

---

## cpuid_features_7ebx

Leaf 7 subleaf 0 EBX feature names.

```fides
pub fn cpuid_features_7ebx(ebx: i64) -> Vec<str> {
```

**Parameters:**

- `ebx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 127*

---

## cpuid_features_7ecx

Leaf 7 subleaf 0 ECX feature names (the commonly checked ones).

```fides
pub fn cpuid_features_7ecx(ecx: i64) -> Vec<str> {
```

**Parameters:**

- `ecx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 132*

---

## cpuid_features_ext_edx

Leaf 0x80000001 EDX (AMD-defined, Intel implements syscall/nx/pdpe1gb/rdtscp/lm).

```fides
pub fn cpuid_features_ext_edx(edx: i64) -> Vec<str> {
```

**Parameters:**

- `edx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 137*

---

## cpuid_features_ext_ecx

Leaf 0x80000001 ECX.

```fides
pub fn cpuid_features_ext_ecx(ecx: i64) -> Vec<str> {
```

**Parameters:**

- `ecx`: `i64`

**Returns:** `Vec<str>`

*Defined at line 142*

---

## cpuid_has

True when `name` is among the flags of a decoded feature list.

```fides
pub fn cpuid_has(flags: Vec<str>, name: str) -> bool {
```

**Parameters:**

- `flags`: `Vec<str>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 147*

---

## cpuid_brand

The brand string from leaves 0x80000002, 0x80000003, 0x80000004,
leading spaces trimmed.

```fides
pub fn cpuid_brand(a: CpuidRegs, b: CpuidRegs, c: CpuidRegs) -> str {
```

**Parameters:**

- `a`: `CpuidRegs`
- `b`: `CpuidRegs`
- `c`: `CpuidRegs`

**Returns:** `str`

*Defined at line 160*

---

## cpuid_signature

"family 6 model 0x9E stepping 10".

```fides
pub fn cpuid_signature(eax: i64) -> str {
```

**Parameters:**

- `eax`: `i64`

**Returns:** `str`

*Defined at line 169*

---

## MSR_IA32_TSC:

```fides
pub const MSR_IA32_TSC: i64 = 0x10;
```

*Defined at line 175*

---

## MSR_IA32_APIC_BASE:

```fides
pub const MSR_IA32_APIC_BASE: i64 = 0x1B;
```

*Defined at line 176*

---

## MSR_IA32_FEATURE_CONTROL:

```fides
pub const MSR_IA32_FEATURE_CONTROL: i64 = 0x3A;
```

*Defined at line 177*

---

## MSR_IA32_MTRRCAP:

```fides
pub const MSR_IA32_MTRRCAP: i64 = 0xFE;
```

*Defined at line 178*

---

## MSR_IA32_SYSENTER_CS:

```fides
pub const MSR_IA32_SYSENTER_CS: i64 = 0x174;
```

*Defined at line 179*

---

## MSR_IA32_SYSENTER_ESP:

```fides
pub const MSR_IA32_SYSENTER_ESP: i64 = 0x175;
```

*Defined at line 180*

---

## MSR_IA32_SYSENTER_EIP:

```fides
pub const MSR_IA32_SYSENTER_EIP: i64 = 0x176;
```

*Defined at line 181*

---

## MSR_IA32_MISC_ENABLE:

```fides
pub const MSR_IA32_MISC_ENABLE: i64 = 0x1A0;
```

*Defined at line 182*

---

## MSR_IA32_PAT:

```fides
pub const MSR_IA32_PAT: i64 = 0x277;
```

*Defined at line 183*

---

## MSR_IA32_TSC_DEADLINE:

```fides
pub const MSR_IA32_TSC_DEADLINE: i64 = 0x6E0;
```

*Defined at line 184*

---

## MSR_X2APIC_BASE:

```fides
pub const MSR_X2APIC_BASE: i64 = 0x800;
```

*Defined at line 185*

---

## MSR_EFER:

```fides
pub const MSR_EFER: i64 = 0xC0000080;
```

*Defined at line 186*

---

## MSR_STAR:

```fides
pub const MSR_STAR: i64 = 0xC0000081;
```

*Defined at line 187*

---

## MSR_LSTAR:

```fides
pub const MSR_LSTAR: i64 = 0xC0000082;
```

*Defined at line 188*

---

## MSR_CSTAR:

```fides
pub const MSR_CSTAR: i64 = 0xC0000083;
```

*Defined at line 189*

---

## MSR_FMASK:

```fides
pub const MSR_FMASK: i64 = 0xC0000084;
```

*Defined at line 190*

---

## MSR_FS_BASE:

```fides
pub const MSR_FS_BASE: i64 = 0xC0000100;
```

*Defined at line 191*

---

## MSR_GS_BASE:

```fides
pub const MSR_GS_BASE: i64 = 0xC0000101;
```

*Defined at line 192*

---

## MSR_KERNEL_GS_BASE:

```fides
pub const MSR_KERNEL_GS_BASE: i64 = 0xC0000102;
```

*Defined at line 193*

---

## MSR_TSC_AUX:

```fides
pub const MSR_TSC_AUX: i64 = 0xC0000103;
```

*Defined at line 194*

---

## EFER_SCE:

```fides
pub const EFER_SCE: i64 = 0x1;
```

*Defined at line 196*

---

## EFER_LME:

```fides
pub const EFER_LME: i64 = 0x100;
```

*Defined at line 197*

---

## EFER_LMA:

```fides
pub const EFER_LMA: i64 = 0x400;
```

*Defined at line 198*

---

## EFER_NXE:

```fides
pub const EFER_NXE: i64 = 0x800;
```

*Defined at line 199*

---

## x86_msr_name

```fides
pub fn x86_msr_name(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 201*

---

## x86_efer_decode

```fides
pub fn x86_efer_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 265*

---

## x86_star

IA32_STAR: SYSCALL loads CS = kernel_cs, SS = kernel_cs + 8; SYSRET to
64-bit loads CS = user_base + 16, SS = user_base + 8 (both | 3).

```fides
pub fn x86_star(kernel_cs: i64, user_base: i64) -> i64 {
```

**Parameters:**

- `kernel_cs`: `i64`
- `user_base`: `i64`

**Returns:** `i64`

*Defined at line 287*

---

## x86_star_decode

The selectors SYSCALL/SYSRET use with a STAR value:
"syscall CS=0x10 SS=0x18 sysret CS=0x33 SS=0x2B".

```fides
pub fn x86_star_decode(star: i64) -> str {
```

**Parameters:**

- `star`: `i64`

**Returns:** `str`

*Defined at line 293*

---

## x86_apic_base_decode

IA32_APIC_BASE: "base 0xFEE00000 BSP enabled" (+ "x2APIC").

```fides
pub fn x86_apic_base_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 300*

---

## x86_memtype_name

PAT memory type names.

```fides
pub fn x86_memtype_name(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 317*

---

## x86_pat

IA32_PAT from 8 entry types (PA0..PA7).

```fides
pub fn x86_pat(types: Vec<i64>) -> i64 {
```

**Parameters:**

- `types`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 340*

---

## x86_pat_decode

```fides
pub fn x86_pat_decode(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 350*

---

## SEG_KERNEL_CODE:

Access byte values.

```fides
pub const SEG_KERNEL_CODE: i64 = 0x9A;
```

*Defined at line 363*

---

## SEG_KERNEL_DATA:

```fides
pub const SEG_KERNEL_DATA: i64 = 0x92;
```

*Defined at line 364*

---

## SEG_USER_CODE:

```fides
pub const SEG_USER_CODE: i64 = 0xFA;
```

*Defined at line 365*

---

## SEG_USER_DATA:

```fides
pub const SEG_USER_DATA: i64 = 0xF2;
```

*Defined at line 366*

---

## SEG_TSS64:

```fides
pub const SEG_TSS64: i64 = 0x89;
```

*Defined at line 367*

---

## SEG_FLAGS_LONG:

Flags nibble: G (8), D/B (4), L (2).

```fides
pub const SEG_FLAGS_LONG: i64 = 0xA;
```

*Defined at line 369*

---

## SEG_FLAGS_32:

```fides
pub const SEG_FLAGS_32: i64 = 0xC;
```

*Defined at line 370*

---

## gdt_entry

An 8-byte code/data descriptor.

```fides
pub fn gdt_entry(base: i64, limit: i64, access: i64, flags: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `limit`: `i64`
- `access`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 373*

---

## gdt_decode

Decode an 8-byte descriptor: "code base=0x0 limit=0xFFFFF DPL0 P L G".

```fides
pub fn gdt_decode(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 384*

---

## tss_desc_low

The two words of a 64-bit TSS descriptor (limit 103 for a TSS without
an I/O bitmap).

```fides
pub fn tss_desc_low(base: i64, limit: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `limit`: `i64`

**Returns:** `i64`

*Defined at line 426*

---

## tss_desc_high

```fides
pub fn tss_desc_high(base: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`

**Returns:** `i64`

*Defined at line 430*

---

## x86_selector

A segment selector.

```fides
pub fn x86_selector(index: i64, ldt: bool, rpl: i64) -> i64 {
```

**Parameters:**

- `index`: `i64`
- `ldt`: `bool`
- `rpl`: `i64`

**Returns:** `i64`

*Defined at line 435*

---

## GATE_INTERRUPT:

Gate types.

```fides
pub const GATE_INTERRUPT: i64 = 0xE;
```

*Defined at line 444*

---

## GATE_TRAP:

```fides
pub const GATE_TRAP: i64 = 0xF;
```

*Defined at line 445*

---

## idt_gate_low

Low and high words of a 16-byte IDT gate for `handler`.

```fides
pub fn idt_gate_low(handler: i64, selector: i64, ist: i64, gate_type: i64, dpl: i64) -> i64 {
```

**Parameters:**

- `handler`: `i64`
- `selector`: `i64`
- `ist`: `i64`
- `gate_type`: `i64`
- `dpl`: `i64`

**Returns:** `i64`

*Defined at line 448*

---

## idt_gate_high

```fides
pub fn idt_gate_high(handler: i64) -> i64 {
```

**Parameters:**

- `handler`: `i64`

**Returns:** `i64`

*Defined at line 457*

---

## idt_gate_target

The handler address a gate points to.

```fides
pub fn idt_gate_target(low: i64, high: i64) -> i64 {
```

**Parameters:**

- `low`: `i64`
- `high`: `i64`

**Returns:** `i64`

*Defined at line 462*

---

## x86_vector_name

Exception vectors 0..31 (SDM 3A table 6-1).

```fides
pub fn x86_vector_name(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 467*

---

## x86_has_error_code

Vectors that push an error code.

```fides
pub fn x86_has_error_code(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 479*

---

## x86_pf_error_decode

#PF error code decode: "present write user" style.

```fides
pub fn x86_pf_error_decode(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 484*

---

## x86_put_le

Append `v` as `n` little-endian bytes.

```fides
pub fn x86_put_le(out: &mut Vec<i64>, v: i64, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`
- `n`: `i64`

*Defined at line 519*

---

## x86_table_bytes

Words as little-endian bytes (a GDT or IDT image).

```fides
pub fn x86_table_bytes(words: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `words`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 528*

---

## x86_pseudo_desc

The 10-byte GDTR/IDTR operand of LGDT/LIDT: limit (size - 1), base.

```fides
pub fn x86_pseudo_desc(base: i64, size_bytes: i64) -> Vec<i64> {
```

**Parameters:**

- `base`: `i64`
- `size_bytes`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 539*

---

## x86_tss_bytes

The 104-byte 64-bit TSS: RSP0..2 at 4, 12, 20; IST1..7 at 36..84; the
I/O map base at 102 (104 = no bitmap).

```fides
pub fn x86_tss_bytes(rsp0: i64, ist: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `rsp0`: `i64`
- `ist`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 548*

---

## x86_standard_gdt

A standard long-mode GDT: null, kernel code, kernel data, user data,
user code (the order SYSRET needs: STAR user base = 0x18), then the
TSS descriptor (two slots).

```fides
pub fn x86_standard_gdt(tss_base: i64) -> Vec<i64> {
```

**Parameters:**

- `tss_base`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 572*

---

## PortIo

Port I/O: mode 0 fake (ports in a map, every access logged), mode 1
real (in/out instructions through the port builtins).

```fides
pub struct PortIo {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `ports` | `HashMap<i64` |
| `log` | `Vec<str>` |

*Defined at line 588*

---

## portio_fake

```fides
pub fn portio_fake() -> PortIo {
```

**Returns:** `PortIo`

*Defined at line 595*

---

## portio_real

```fides
pub fn portio_real() -> PortIo {
```

**Returns:** `PortIo`

*Defined at line 601*

---

## portio_in

```fides
pub fn portio_in(io: &mut PortIo, port: i64, width: i64) -> i64 {
```

**Parameters:**

- `io`: `&mut PortIo`
- `port`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 631*

---

## portio_out

```fides
pub fn portio_out(io: &mut PortIo, port: i64, width: i64, v: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `port`: `i64`
- `width`: `i64`
- `v`: `i64`

*Defined at line 646*

---

## x86_inb

```fides
pub fn x86_inb(io: &mut PortIo, port: i64) -> i64 {
```

**Parameters:**

- `io`: `&mut PortIo`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 659*

---

## x86_outb

```fides
pub fn x86_outb(io: &mut PortIo, port: i64, v: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `port`: `i64`
- `v`: `i64`

*Defined at line 662*

---

## PIC1_CMD:

```fides
pub const PIC1_CMD: i64 = 0x20;
```

*Defined at line 668*

---

## PIC1_DATA:

```fides
pub const PIC1_DATA: i64 = 0x21;
```

*Defined at line 669*

---

## PIC2_CMD:

```fides
pub const PIC2_CMD: i64 = 0xA0;
```

*Defined at line 670*

---

## PIC2_DATA:

```fides
pub const PIC2_DATA: i64 = 0xA1;
```

*Defined at line 671*

---

## PIC_EOI:

```fides
pub const PIC_EOI: i64 = 0x20;
```

*Defined at line 672*

---

## pic_remap

Remap the two 8259s to vectors off1..off1+7 and off2..off2+7 (ICW1
with ICW4, ICW2 offsets, ICW3 cascade on IRQ2, ICW4 8086 mode), then
restore the masks that were set.

```fides
pub fn pic_remap(io: &mut PortIo, off1: i64, off2: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `off1`: `i64`
- `off2`: `i64`

*Defined at line 677*

---

## pic_set_mask

Mask (true) or unmask one IRQ line 0..15.

```fides
pub fn pic_set_mask(io: &mut PortIo, irq: i64, masked: bool) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `irq`: `i64`
- `masked`: `bool`

*Defined at line 693*

---

## pic_eoi

End of interrupt: the slave too for IRQs 8..15.

```fides
pub fn pic_eoi(io: &mut PortIo, irq: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `irq`: `i64`

*Defined at line 710*

---

## PIT_HZ:

```fides
pub const PIT_HZ: i64 = 1193182;
```

*Defined at line 717*

---

## pit_divisor

PIT channel 0 divisor for `hz` (1..65536; 0 is written for 65536).

```fides
pub fn pit_divisor(hz: i64) -> i64 {
```

**Parameters:**

- `hz`: `i64`

**Returns:** `i64`

*Defined at line 720*

---

## pit_set_rate

Program channel 0 as a rate generator (mode 2) at `hz`: command 0x34
(channel 0, lobyte/hibyte, mode 2, binary), then the divisor.

```fides
pub fn pit_set_rate(io: &mut PortIo, hz: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `hz`: `i64`

*Defined at line 733*

---

## cmos_read

Read CMOS register `reg` (NMI stays disabled while selecting: bit 7).

```fides
pub fn cmos_read(io: &mut PortIo, reg: i64) -> i64 {
```

**Parameters:**

- `io`: `&mut PortIo`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 741*

---

## cmos_write

```fides
pub fn cmos_write(io: &mut PortIo, reg: i64, v: i64) {
```

**Parameters:**

- `io`: `&mut PortIo`
- `reg`: `i64`
- `v`: `i64`

*Defined at line 746*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
