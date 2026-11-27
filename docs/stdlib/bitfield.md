[All modules](index.md)

# bitfield -- Fides Documentation

## Contents

- [bit](#bit) (function)
- [set_bit](#set_bit) (function)
- [clear_bit](#clear_bit) (function)
- [toggle_bit](#toggle_bit) (function)
- [write_bit](#write_bit) (function)
- [get](#get) (function)
- [set](#set) (function)
- [CR0_PE:](#CR0_PE:) (const)
- [CR0_MP:](#CR0_MP:) (const)
- [CR0_EM:](#CR0_EM:) (const)
- [CR0_TS:](#CR0_TS:) (const)
- [CR0_ET:](#CR0_ET:) (const)
- [CR0_NE:](#CR0_NE:) (const)
- [CR0_WP:](#CR0_WP:) (const)
- [CR0_AM:](#CR0_AM:) (const)
- [CR0_NW:](#CR0_NW:) (const)
- [CR0_CD:](#CR0_CD:) (const)
- [CR0_PG:](#CR0_PG:) (const)
- [CR4_VME:](#CR4_VME:) (const)
- [CR4_PVI:](#CR4_PVI:) (const)
- [CR4_TSD:](#CR4_TSD:) (const)
- [CR4_DE:](#CR4_DE:) (const)
- [CR4_PSE:](#CR4_PSE:) (const)
- [CR4_PAE:](#CR4_PAE:) (const)
- [CR4_MCE:](#CR4_MCE:) (const)
- [CR4_PGE:](#CR4_PGE:) (const)
- [CR4_PCE:](#CR4_PCE:) (const)
- [CR4_OSFXSR:](#CR4_OSFXSR:) (const)
- [CR4_OSXMMEXCPT:](#CR4_OSXMMEXCPT:) (const)
- [CR4_SMEP:](#CR4_SMEP:) (const)
- [CR4_SMAP:](#CR4_SMAP:) (const)
- [EFER_SCE:](#EFER_SCE:) (const)
- [EFER_LME:](#EFER_LME:) (const)
- [EFER_LMA:](#EFER_LMA:) (const)
- [EFER_NXE:](#EFER_NXE:) (const)
- [EFER_SVME:](#EFER_SVME:) (const)
- [PTE_PRESENT:](#PTE_PRESENT:) (const)
- [PTE_WRITABLE:](#PTE_WRITABLE:) (const)
- [PTE_USER:](#PTE_USER:) (const)
- [PTE_WRITE_THROUGH:](#PTE_WRITE_THROUGH:) (const)
- [PTE_CACHE_DISABLE:](#PTE_CACHE_DISABLE:) (const)
- [PTE_ACCESSED:](#PTE_ACCESSED:) (const)
- [PTE_DIRTY:](#PTE_DIRTY:) (const)
- [PTE_HUGE:](#PTE_HUGE:) (const)
- [PTE_GLOBAL:](#PTE_GLOBAL:) (const)
- [PTE_NX:](#PTE_NX:) (const)
- [GDT_PRESENT:](#GDT_PRESENT:) (const)
- [GDT_DPL:](#GDT_DPL:) (const)
- [GDT_S:](#GDT_S:) (const)
- [GDT_EXECUTABLE:](#GDT_EXECUTABLE:) (const)
- [GDT_DC:](#GDT_DC:) (const)
- [GDT_RW:](#GDT_RW:) (const)
- [GDT_ACCESSED:](#GDT_ACCESSED:) (const)
- [GDT_GRANULARITY:](#GDT_GRANULARITY:) (const)
- [GDT_DB:](#GDT_DB:) (const)
- [GDT_L:](#GDT_L:) (const)
- [GDT_AVL:](#GDT_AVL:) (const)
- [IDT_TASK_GATE:](#IDT_TASK_GATE:) (const)
- [IDT_INT_GATE_16:](#IDT_INT_GATE_16:) (const)
- [IDT_TRAP_GATE_16:](#IDT_TRAP_GATE_16:) (const)
- [IDT_INT_GATE_32:](#IDT_INT_GATE_32:) (const)
- [IDT_TRAP_GATE_32:](#IDT_TRAP_GATE_32:) (const)
- [IDT_PRESENT:](#IDT_PRESENT:) (const)

## bit

```fides
pub fn bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 26*

---

## set_bit

```fides
pub fn set_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 31*

---

## clear_bit

```fides
pub fn clear_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 36*

---

## toggle_bit

```fides
pub fn toggle_bit(value: i64, pos: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 41*

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

*Defined at line 46*

---

## get


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(get(0x9A, 0, 8), 0x9A);
assert_eq(get(0x9A, 0, 4), 0xA);
assert_eq(get(0x9A, 4, 4), 0x9);
assert_eq(bit(5, 2), 1);
assert_eq(set_bit(0, 3), 8);
assert_eq(clear_bit(15, 0), 14);
assert_eq(toggle_bit(5, 1), 7);
assert_eq(write_bit(0, CR0_PG, 1), 1 << 31);
```

```fides
pub fn get(value: i64, offset: i64, width: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `offset`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 68*

---

## set


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(set(0x0000, 8, 8, 0x9A), 0x9A00);
assert_eq(set(0xFFFF, 4, 4, 0), 0xFF0F);
```

```fides
pub fn set(value: i64, offset: i64, width: i64, field_val: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`
- `offset`: `i64`
- `width`: `i64`
- `field_val`: `i64`

**Returns:** `i64`

*Defined at line 79*

---

## CR0_PE:

```fides
pub const CR0_PE: i64 = 0;  // Protection Enable
```

*Defined at line 90*

---

## CR0_MP:

```fides
pub const CR0_MP: i64 = 1;  // Monitor coProcessor
```

*Defined at line 91*

---

## CR0_EM:

```fides
pub const CR0_EM: i64 = 2;  // Emulation
```

*Defined at line 92*

---

## CR0_TS:

```fides
pub const CR0_TS: i64 = 3;  // Task Switched
```

*Defined at line 93*

---

## CR0_ET:

```fides
pub const CR0_ET: i64 = 4;  // Extension Type
```

*Defined at line 94*

---

## CR0_NE:

```fides
pub const CR0_NE: i64 = 5;  // Numeric Error
```

*Defined at line 95*

---

## CR0_WP:

```fides
pub const CR0_WP: i64 = 16;  // Write Protect
```

*Defined at line 96*

---

## CR0_AM:

```fides
pub const CR0_AM: i64 = 18;  // Alignment Mask
```

*Defined at line 97*

---

## CR0_NW:

```fides
pub const CR0_NW: i64 = 29;  // Not Write-through
```

*Defined at line 98*

---

## CR0_CD:

```fides
pub const CR0_CD: i64 = 30;  // Cache Disable
```

*Defined at line 99*

---

## CR0_PG:

```fides
pub const CR0_PG: i64 = 31;  // Paging Enable
```

*Defined at line 100*

---

## CR4_VME:

```fides
pub const CR4_VME: i64 = 0;  // Virtual 8086 Mode Extensions
```

*Defined at line 103*

---

## CR4_PVI:

```fides
pub const CR4_PVI: i64 = 1;  // Protected-Mode Virtual Interrupts
```

*Defined at line 104*

---

## CR4_TSD:

```fides
pub const CR4_TSD: i64 = 2;  // Time Stamp Disable
```

*Defined at line 105*

---

## CR4_DE:

```fides
pub const CR4_DE: i64 = 3;  // Debugging Extensions
```

*Defined at line 106*

---

## CR4_PSE:

```fides
pub const CR4_PSE: i64 = 4;  // Page Size Extension
```

*Defined at line 107*

---

## CR4_PAE:

```fides
pub const CR4_PAE: i64 = 5;  // Physical Address Extension
```

*Defined at line 108*

---

## CR4_MCE:

```fides
pub const CR4_MCE: i64 = 6;  // Machine Check Enable
```

*Defined at line 109*

---

## CR4_PGE:

```fides
pub const CR4_PGE: i64 = 7;  // Page Global Enable
```

*Defined at line 110*

---

## CR4_PCE:

```fides
pub const CR4_PCE: i64 = 8;  // Performance Counter Enable
```

*Defined at line 111*

---

## CR4_OSFXSR:

```fides
pub const CR4_OSFXSR: i64 = 9;  // OS FXSAVE/FXRSTOR Support
```

*Defined at line 112*

---

## CR4_OSXMMEXCPT:

```fides
pub const CR4_OSXMMEXCPT: i64 = 10;  // OS XMM Exception Support
```

*Defined at line 113*

---

## CR4_SMEP:

```fides
pub const CR4_SMEP: i64 = 20;  // Supervisor Mode Execution Prevention
```

*Defined at line 114*

---

## CR4_SMAP:

```fides
pub const CR4_SMAP: i64 = 21;  // Supervisor Mode Access Prevention
```

*Defined at line 115*

---

## EFER_SCE:

```fides
pub const EFER_SCE: i64 = 0;  // System Call Extensions
```

*Defined at line 118*

---

## EFER_LME:

```fides
pub const EFER_LME: i64 = 8;  // Long Mode Enable
```

*Defined at line 119*

---

## EFER_LMA:

```fides
pub const EFER_LMA: i64 = 10;  // Long Mode Active
```

*Defined at line 120*

---

## EFER_NXE:

```fides
pub const EFER_NXE: i64 = 11;  // No-Execute Enable
```

*Defined at line 121*

---

## EFER_SVME:

```fides
pub const EFER_SVME: i64 = 12;  // Secure Virtual Machine Enable
```

*Defined at line 122*

---

## PTE_PRESENT:

```fides
pub const PTE_PRESENT: i64 = 0;  // Present bit
```

*Defined at line 126*

---

## PTE_WRITABLE:

```fides
pub const PTE_WRITABLE: i64 = 1;  // Read/Write bit
```

*Defined at line 127*

---

## PTE_USER:

```fides
pub const PTE_USER: i64 = 2;  // User/Supervisor bit
```

*Defined at line 128*

---

## PTE_WRITE_THROUGH:

```fides
pub const PTE_WRITE_THROUGH: i64 = 3;  // Page-level Write-Through
```

*Defined at line 129*

---

## PTE_CACHE_DISABLE:

```fides
pub const PTE_CACHE_DISABLE: i64 = 4;  // Page-level Cache Disable
```

*Defined at line 130*

---

## PTE_ACCESSED:

```fides
pub const PTE_ACCESSED: i64 = 5;  // Accessed bit
```

*Defined at line 131*

---

## PTE_DIRTY:

```fides
pub const PTE_DIRTY: i64 = 6;  // Dirty bit
```

*Defined at line 132*

---

## PTE_HUGE:

```fides
pub const PTE_HUGE: i64 = 7;  // Huge page (PS bit)
```

*Defined at line 133*

---

## PTE_GLOBAL:

```fides
pub const PTE_GLOBAL: i64 = 8;  // Global page
```

*Defined at line 134*

---

## PTE_NX:

```fides
pub const PTE_NX: i64 = 63;  // No-Execute bit (in PAE mode)
```

*Defined at line 135*

---

## GDT_PRESENT:

```fides
pub const GDT_PRESENT: i64 = 7;  // Present bit (bit 7 of access byte)
```

*Defined at line 140*

---

## GDT_DPL:

```fides
pub const GDT_DPL: i64 = 5;  // Descriptor Privilege Level (bits 5-6)
```

*Defined at line 141*

---

## GDT_S:

```fides
pub const GDT_S: i64 = 4;  // Descriptor type (1 = code/data)
```

*Defined at line 142*

---

## GDT_EXECUTABLE:

```fides
pub const GDT_EXECUTABLE: i64 = 3;  // Executable (1 = code, 0 = data)
```

*Defined at line 143*

---

## GDT_DC:

```fides
pub const GDT_DC: i64 = 2;  // Direction/Conforming
```

*Defined at line 144*

---

## GDT_RW:

```fides
pub const GDT_RW: i64 = 1;  // Read/Write
```

*Defined at line 145*

---

## GDT_ACCESSED:

```fides
pub const GDT_ACCESSED: i64 = 0;  // Accessed bit
```

*Defined at line 146*

---

## GDT_GRANULARITY:

```fides
pub const GDT_GRANULARITY: i64 = 7;  // Granularity (1 = 4KB)
```

*Defined at line 149*

---

## GDT_DB:

```fides
pub const GDT_DB: i64 = 6;  // Default operation size (1 = 32-bit)
```

*Defined at line 150*

---

## GDT_L:

```fides
pub const GDT_L: i64 = 5;  // Long mode (1 = 64-bit)
```

*Defined at line 151*

---

## GDT_AVL:

```fides
pub const GDT_AVL: i64 = 4;  // Available for use by system
```

*Defined at line 152*

---

## IDT_TASK_GATE:

```fides
pub const IDT_TASK_GATE: i64 = 0x5;
```

*Defined at line 157*

---

## IDT_INT_GATE_16:

```fides
pub const IDT_INT_GATE_16: i64 = 0x6;
```

*Defined at line 158*

---

## IDT_TRAP_GATE_16:

```fides
pub const IDT_TRAP_GATE_16: i64 = 0x7;
```

*Defined at line 159*

---

## IDT_INT_GATE_32:

```fides
pub const IDT_INT_GATE_32: i64 = 0xE;
```

*Defined at line 160*

---

## IDT_TRAP_GATE_32:

```fides
pub const IDT_TRAP_GATE_32: i64 = 0xF;
```

*Defined at line 161*

---

## IDT_PRESENT:

```fides
pub const IDT_PRESENT: i64 = 7;
```

*Defined at line 163*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
