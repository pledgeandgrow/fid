# bitfield — Fides Documentation

## Contents

- [bit](#bit) (function)
- [set_bit](#set_bit) (function)
- [clear_bit](#clear_bit) (function)
- [toggle_bit](#toggle_bit) (function)
- [write_bit](#write_bit) (function)
- [get](#get) (function)
- [set](#set) (function)
- [CR0_PE:](#cr0_pe:) (const)
- [CR0_MP:](#cr0_mp:) (const)
- [CR0_EM:](#cr0_em:) (const)
- [CR0_TS:](#cr0_ts:) (const)
- [CR0_ET:](#cr0_et:) (const)
- [CR0_NE:](#cr0_ne:) (const)
- [CR0_WP:](#cr0_wp:) (const)
- [CR0_AM:](#cr0_am:) (const)
- [CR0_NW:](#cr0_nw:) (const)
- [CR0_CD:](#cr0_cd:) (const)
- [CR0_PG:](#cr0_pg:) (const)
- [CR4_VME:](#cr4_vme:) (const)
- [CR4_PVI:](#cr4_pvi:) (const)
- [CR4_TSD:](#cr4_tsd:) (const)
- [CR4_DE:](#cr4_de:) (const)
- [CR4_PSE:](#cr4_pse:) (const)
- [CR4_PAE:](#cr4_pae:) (const)
- [CR4_MCE:](#cr4_mce:) (const)
- [CR4_PGE:](#cr4_pge:) (const)
- [CR4_PCE:](#cr4_pce:) (const)
- [CR4_OSFXSR:](#cr4_osfxsr:) (const)
- [CR4_OSXMMEXCPT:](#cr4_osxmmexcpt:) (const)
- [CR4_SMEP:](#cr4_smep:) (const)
- [CR4_SMAP:](#cr4_smap:) (const)
- [EFER_SCE:](#efer_sce:) (const)
- [EFER_LME:](#efer_lme:) (const)
- [EFER_LMA:](#efer_lma:) (const)
- [EFER_NXE:](#efer_nxe:) (const)
- [EFER_SVME:](#efer_svme:) (const)
- [PTE_PRESENT:](#pte_present:) (const)
- [PTE_WRITABLE:](#pte_writable:) (const)
- [PTE_USER:](#pte_user:) (const)
- [PTE_WRITE_THROUGH:](#pte_write_through:) (const)
- [PTE_CACHE_DISABLE:](#pte_cache_disable:) (const)
- [PTE_ACCESSED:](#pte_accessed:) (const)
- [PTE_DIRTY:](#pte_dirty:) (const)
- [PTE_HUGE:](#pte_huge:) (const)
- [PTE_GLOBAL:](#pte_global:) (const)
- [PTE_NX:](#pte_nx:) (const)
- [GDT_PRESENT:](#gdt_present:) (const)
- [GDT_DPL:](#gdt_dpl:) (const)
- [GDT_S:](#gdt_s:) (const)
- [GDT_EXECUTABLE:](#gdt_executable:) (const)
- [GDT_DC:](#gdt_dc:) (const)
- [GDT_RW:](#gdt_rw:) (const)
- [GDT_ACCESSED:](#gdt_accessed:) (const)
- [GDT_GRANULARITY:](#gdt_granularity:) (const)
- [GDT_DB:](#gdt_db:) (const)
- [GDT_L:](#gdt_l:) (const)
- [GDT_AVL:](#gdt_avl:) (const)
- [IDT_TASK_GATE:](#idt_task_gate:) (const)
- [IDT_INT_GATE_16:](#idt_int_gate_16:) (const)
- [IDT_TRAP_GATE_16:](#idt_trap_gate_16:) (const)
- [IDT_INT_GATE_32:](#idt_int_gate_32:) (const)
- [IDT_TRAP_GATE_32:](#idt_trap_gate_32:) (const)
- [IDT_PRESENT:](#idt_present:) (const)

## bit

```fides
pub fn bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 27*

---

## set_bit

```fides
pub fn set_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 32*

---

## clear_bit

```fides
pub fn clear_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 37*

---

## toggle_bit

```fides
pub fn toggle_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 42*

---

## write_bit

```fides
pub fn write_bit(value: i64, pos: i64, bit_val: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`
- `bit_val`: `i64`

**Returns:** `i64`

*Defined at line 47*

---

## get

```fides
pub fn get(value: i64, offset: i64, width: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `offset`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 60*

---

## set

```fides
pub fn set(value: i64, offset: i64, width: i64, field_val: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `offset`: `i64`
- `width`: `i64`
- `field_val`: `i64`

**Returns:** `i64`

*Defined at line 68*

---

## CR0_PE:

```fides
pub const CR0_PE: i64 = 0;    // Protection Enable
```

*Defined at line 79*

---

## CR0_MP:

```fides
pub const CR0_MP: i64 = 1;    // Monitor coProcessor
```

*Defined at line 80*

---

## CR0_EM:

```fides
pub const CR0_EM: i64 = 2;    // Emulation
```

*Defined at line 81*

---

## CR0_TS:

```fides
pub const CR0_TS: i64 = 3;    // Task Switched
```

*Defined at line 82*

---

## CR0_ET:

```fides
pub const CR0_ET: i64 = 4;    // Extension Type
```

*Defined at line 83*

---

## CR0_NE:

```fides
pub const CR0_NE: i64 = 5;    // Numeric Error
```

*Defined at line 84*

---

## CR0_WP:

```fides
pub const CR0_WP: i64 = 16;   // Write Protect
```

*Defined at line 85*

---

## CR0_AM:

```fides
pub const CR0_AM: i64 = 18;   // Alignment Mask
```

*Defined at line 86*

---

## CR0_NW:

```fides
pub const CR0_NW: i64 = 29;   // Not Write-through
```

*Defined at line 87*

---

## CR0_CD:

```fides
pub const CR0_CD: i64 = 30;   // Cache Disable
```

*Defined at line 88*

---

## CR0_PG:

```fides
pub const CR0_PG: i64 = 31;   // Paging Enable
```

*Defined at line 89*

---

## CR4_VME:

```fides
pub const CR4_VME: i64 = 0;       // Virtual 8086 Mode Extensions
```

*Defined at line 92*

---

## CR4_PVI:

```fides
pub const CR4_PVI: i64 = 1;       // Protected-Mode Virtual Interrupts
```

*Defined at line 93*

---

## CR4_TSD:

```fides
pub const CR4_TSD: i64 = 2;       // Time Stamp Disable
```

*Defined at line 94*

---

## CR4_DE:

```fides
pub const CR4_DE: i64 = 3;        // Debugging Extensions
```

*Defined at line 95*

---

## CR4_PSE:

```fides
pub const CR4_PSE: i64 = 4;       // Page Size Extension
```

*Defined at line 96*

---

## CR4_PAE:

```fides
pub const CR4_PAE: i64 = 5;       // Physical Address Extension
```

*Defined at line 97*

---

## CR4_MCE:

```fides
pub const CR4_MCE: i64 = 6;       // Machine Check Enable
```

*Defined at line 98*

---

## CR4_PGE:

```fides
pub const CR4_PGE: i64 = 7;       // Page Global Enable
```

*Defined at line 99*

---

## CR4_PCE:

```fides
pub const CR4_PCE: i64 = 8;       // Performance Counter Enable
```

*Defined at line 100*

---

## CR4_OSFXSR:

```fides
pub const CR4_OSFXSR: i64 = 9;    // OS FXSAVE/FXRSTOR Support
```

*Defined at line 101*

---

## CR4_OSXMMEXCPT:

```fides
pub const CR4_OSXMMEXCPT: i64 = 10; // OS XMM Exception Support
```

*Defined at line 102*

---

## CR4_SMEP:

```fides
pub const CR4_SMEP: i64 = 20;     // Supervisor Mode Execution Prevention
```

*Defined at line 103*

---

## CR4_SMAP:

```fides
pub const CR4_SMAP: i64 = 21;     // Supervisor Mode Access Prevention
```

*Defined at line 104*

---

## EFER_SCE:

```fides
pub const EFER_SCE: i64 = 0;      // System Call Extensions
```

*Defined at line 107*

---

## EFER_LME:

```fides
pub const EFER_LME: i64 = 8;      // Long Mode Enable
```

*Defined at line 108*

---

## EFER_LMA:

```fides
pub const EFER_LMA: i64 = 10;     // Long Mode Active
```

*Defined at line 109*

---

## EFER_NXE:

```fides
pub const EFER_NXE: i64 = 11;     // No-Execute Enable
```

*Defined at line 110*

---

## EFER_SVME:

```fides
pub const EFER_SVME: i64 = 12;    // Secure Virtual Machine Enable
```

*Defined at line 111*

---

## PTE_PRESENT:

```fides
pub const PTE_PRESENT: i64 = 0;   // Present bit
```

*Defined at line 115*

---

## PTE_WRITABLE:

```fides
pub const PTE_WRITABLE: i64 = 1;  // Read/Write bit
```

*Defined at line 116*

---

## PTE_USER:

```fides
pub const PTE_USER: i64 = 2;      // User/Supervisor bit
```

*Defined at line 117*

---

## PTE_WRITE_THROUGH:

```fides
pub const PTE_WRITE_THROUGH: i64 = 3; // Page-level Write-Through
```

*Defined at line 118*

---

## PTE_CACHE_DISABLE:

```fides
pub const PTE_CACHE_DISABLE: i64 = 4; // Page-level Cache Disable
```

*Defined at line 119*

---

## PTE_ACCESSED:

```fides
pub const PTE_ACCESSED: i64 = 5;  // Accessed bit
```

*Defined at line 120*

---

## PTE_DIRTY:

```fides
pub const PTE_DIRTY: i64 = 6;     // Dirty bit
```

*Defined at line 121*

---

## PTE_HUGE:

```fides
pub const PTE_HUGE: i64 = 7;      // Huge page (PS bit)
```

*Defined at line 122*

---

## PTE_GLOBAL:

```fides
pub const PTE_GLOBAL: i64 = 8;    // Global page
```

*Defined at line 123*

---

## PTE_NX:

```fides
pub const PTE_NX: i64 = 63;       // No-Execute bit (in PAE mode)
```

*Defined at line 124*

---

## GDT_PRESENT:

```fides
pub const GDT_PRESENT: i64 = 7;   // Present bit (bit 7 of access byte)
```

*Defined at line 129*

---

## GDT_DPL:

```fides
pub const GDT_DPL: i64 = 5;       // Descriptor Privilege Level (bits 5-6)
```

*Defined at line 130*

---

## GDT_S:

```fides
pub const GDT_S: i64 = 4;         // Descriptor type (1 = code/data)
```

*Defined at line 131*

---

## GDT_EXECUTABLE:

```fides
pub const GDT_EXECUTABLE: i64 = 3; // Executable (1 = code, 0 = data)
```

*Defined at line 132*

---

## GDT_DC:

```fides
pub const GDT_DC: i64 = 2;        // Direction/Conforming
```

*Defined at line 133*

---

## GDT_RW:

```fides
pub const GDT_RW: i64 = 1;        // Read/Write
```

*Defined at line 134*

---

## GDT_ACCESSED:

```fides
pub const GDT_ACCESSED: i64 = 0;  // Accessed bit
```

*Defined at line 135*

---

## GDT_GRANULARITY:

```fides
pub const GDT_GRANULARITY: i64 = 7; // Granularity (1 = 4KB)
```

*Defined at line 138*

---

## GDT_DB:

```fides
pub const GDT_DB: i64 = 6;        // Default operation size (1 = 32-bit)
```

*Defined at line 139*

---

## GDT_L:

```fides
pub const GDT_L: i64 = 5;         // Long mode (1 = 64-bit)
```

*Defined at line 140*

---

## GDT_AVL:

```fides
pub const GDT_AVL: i64 = 4;       // Available for use by system
```

*Defined at line 141*

---

## IDT_TASK_GATE:

```fides
pub const IDT_TASK_GATE: i64 = 0x5;
```

*Defined at line 146*

---

## IDT_INT_GATE_16:

```fides
pub const IDT_INT_GATE_16: i64 = 0x6;
```

*Defined at line 147*

---

## IDT_TRAP_GATE_16:

```fides
pub const IDT_TRAP_GATE_16: i64 = 0x7;
```

*Defined at line 148*

---

## IDT_INT_GATE_32:

```fides
pub const IDT_INT_GATE_32: i64 = 0xE;
```

*Defined at line 149*

---

## IDT_TRAP_GATE_32:

```fides
pub const IDT_TRAP_GATE_32: i64 = 0xF;
```

*Defined at line 150*

---

## IDT_PRESENT:

```fides
pub const IDT_PRESENT: i64 = 7;
```

*Defined at line 153*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
