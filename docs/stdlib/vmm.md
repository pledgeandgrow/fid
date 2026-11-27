[All modules](index.md)

# vmm -- Fides Documentation

## Contents

- [VMM_X86_64:](#VMM_X86_64:) (const)
- [VMM_AARCH64:](#VMM_AARCH64:) (const)
- [VMM_SV39:](#VMM_SV39:) (const)
- [VMM_SV48:](#VMM_SV48:) (const)
- [VM_READ:](#VM_READ:) (const)
- [VM_WRITE:](#VM_WRITE:) (const)
- [VM_EXEC:](#VM_EXEC:) (const)
- [VM_USER:](#VM_USER:) (const)
- [VM_GLOBAL:](#VM_GLOBAL:) (const)
- [VM_NOCACHE:](#VM_NOCACHE:) (const)
- [VM_ACCESSED:](#VM_ACCESSED:) (const)
- [VM_DIRTY:](#VM_DIRTY:) (const)
- [VMM_OK:](#VMM_OK:) (const)
- [VMM_ERR_NOMEM:](#VMM_ERR_NOMEM:) (const)
- [VMM_ERR_EXISTS:](#VMM_ERR_EXISTS:) (const)
- [VMM_ERR_ALIGN:](#VMM_ERR_ALIGN:) (const)
- [VMM_ERR_CANON:](#VMM_ERR_CANON:) (const)
- [VMM_ERR_NOTMAPPED:](#VMM_ERR_NOTMAPPED:) (const)
- [VMM_ERR_SIZE:](#VMM_ERR_SIZE:) (const)
- [VMM_PAGE:](#VMM_PAGE:) (const)
- [VMM_ADDR_MASK:](#VMM_ADDR_MASK:) (const)
- [PhysMem](#PhysMem) (struct)
- [physmem_fake](#physmem_fake) (function)
- [physmem_direct](#physmem_direct) (function)
- [phys_read64](#phys_read64) (function)
- [phys_write64](#phys_write64) (function)
- [phys_read8](#phys_read8) (function)
- [phys_write8](#phys_write8) (function)
- [phys_read_le](#phys_read_le) (function)
- [phys_write_le](#phys_write_le) (function)
- [phys_write_bytes](#phys_write_bytes) (function)
- [phys_read_bytes](#phys_read_bytes) (function)
- [phys_zero_frame](#phys_zero_frame) (function)
- [FrameAlloc](#FrameAlloc) (struct)
- [framealloc_new](#framealloc_new) (function)
- [framealloc_alloc](#framealloc_alloc) (function)
- [framealloc_free](#framealloc_free) (function)
- [framealloc_used](#framealloc_used) (function)
- [vmm_levels](#vmm_levels) (function)
- [vmm_va_bits](#vmm_va_bits) (function)
- [vmm_index](#vmm_index) (function)
- [vmm_level_size](#vmm_level_size) (function)
- [vmm_leaf_allowed](#vmm_leaf_allowed) (function)
- [vmm_canonical](#vmm_canonical) (function)
- [VmEntry](#VmEntry) (struct)
- [vmm_entry_encode](#vmm_entry_encode) (function)
- [vmm_entry_decode](#vmm_entry_decode) (function)
- [vmm_flags_str](#vmm_flags_str) (function)
- [AddrSpace](#AddrSpace) (struct)
- [Translation](#Translation) (struct)
- [vmm_new](#vmm_new) (function)
- [vmm_map_page](#vmm_map_page) (function)
- [vmm_best_size](#vmm_best_size) (function)
- [vmm_map](#vmm_map) (function)
- [vmm_translate](#vmm_translate) (function)
- [vmm_unmap](#vmm_unmap) (function)
- [vmm_protect](#vmm_protect) (function)
- [vmm_cr3](#vmm_cr3) (function)
- [vmm_satp](#vmm_satp) (function)
- [vmm_ttbr](#vmm_ttbr) (function)
- [vmm_mair_el1](#vmm_mair_el1) (function)
- [vmm_tcr_el1](#vmm_tcr_el1) (function)

## VMM_X86_64:

```fides
pub const VMM_X86_64: i64 = 0;
```

*Defined at line 32*

---

## VMM_AARCH64:

```fides
pub const VMM_AARCH64: i64 = 1;
```

*Defined at line 33*

---

## VMM_SV39:

```fides
pub const VMM_SV39: i64 = 2;
```

*Defined at line 34*

---

## VMM_SV48:

```fides
pub const VMM_SV48: i64 = 3;
```

*Defined at line 35*

---

## VM_READ:

```fides
pub const VM_READ: i64 = 1;
```

*Defined at line 39*

---

## VM_WRITE:

```fides
pub const VM_WRITE: i64 = 2;
```

*Defined at line 40*

---

## VM_EXEC:

```fides
pub const VM_EXEC: i64 = 4;
```

*Defined at line 41*

---

## VM_USER:

```fides
pub const VM_USER: i64 = 8;
```

*Defined at line 42*

---

## VM_GLOBAL:

```fides
pub const VM_GLOBAL: i64 = 16;
```

*Defined at line 43*

---

## VM_NOCACHE:

Device / uncached memory (x86 PCD|PWT, AArch64 MAIR index 1, RISC-V Svpbmt IO).

```fides
pub const VM_NOCACHE: i64 = 32;
```

*Defined at line 45*

---

## VM_ACCESSED:

```fides
pub const VM_ACCESSED: i64 = 64;
```

*Defined at line 46*

---

## VM_DIRTY:

```fides
pub const VM_DIRTY: i64 = 128;
```

*Defined at line 47*

---

## VMM_OK:

```fides
pub const VMM_OK: i64 = 0;
```

*Defined at line 50*

---

## VMM_ERR_NOMEM:

```fides
pub const VMM_ERR_NOMEM: i64 = -1;
```

*Defined at line 51*

---

## VMM_ERR_EXISTS:

```fides
pub const VMM_ERR_EXISTS: i64 = -2;
```

*Defined at line 52*

---

## VMM_ERR_ALIGN:

```fides
pub const VMM_ERR_ALIGN: i64 = -3;
```

*Defined at line 53*

---

## VMM_ERR_CANON:

```fides
pub const VMM_ERR_CANON: i64 = -4;
```

*Defined at line 54*

---

## VMM_ERR_NOTMAPPED:

```fides
pub const VMM_ERR_NOTMAPPED: i64 = -5;
```

*Defined at line 55*

---

## VMM_ERR_SIZE:

```fides
pub const VMM_ERR_SIZE: i64 = -6;
```

*Defined at line 56*

---

## VMM_PAGE:

```fides
pub const VMM_PAGE: i64 = 4096;
```

*Defined at line 58*

---

## VMM_ADDR_MASK:

```fides
pub const VMM_ADDR_MASK: i64 = 0x000FFFFFFFFFF000;
```

*Defined at line 59*

---

## PhysMem

```fides
pub struct PhysMem {
```

**Fields:**

| Name | Type |
|------|------|
| `fake` | `bool` |
| `words` | `Vec<i64>` |
| `base` | `i64` |
| `size` | `i64` |
| `dmap` | `i64,  // direct-map offset (real mode)` |

*Defined at line 63*

---

## physmem_fake

`frames` 4-KiB frames of zeroed fake RAM starting at physical `base`.

```fides
pub fn physmem_fake(base: i64, frames: i64) -> PhysMem {
```

**Parameters:**

- `base`: `i64`
- `frames`: `i64`

**Returns:** `PhysMem`

*Defined at line 75*

---

## physmem_direct

Real physical memory reached as virtual address pa + offset.

```fides
pub fn physmem_direct(offset: i64) -> PhysMem {
```

**Parameters:**

- `offset`: `i64`

**Returns:** `PhysMem`

*Defined at line 82*

---

## phys_read64

64-bit read at an 8-byte aligned physical address.

```fides
pub fn phys_read64(m: &PhysMem, pa: i64) -> i64 {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`

**Returns:** `i64`

*Defined at line 107*

---

## phys_write64

64-bit write at an 8-byte aligned physical address.

```fides
pub fn phys_write64(m: &mut PhysMem, pa: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `v`: `i64`

*Defined at line 118*

---

## phys_read8

```fides
pub fn phys_read8(m: &PhysMem, pa: i64) -> i64 {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`

**Returns:** `i64`

*Defined at line 132*

---

## phys_write8

```fides
pub fn phys_write8(m: &mut PhysMem, pa: i64, b: i64) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `b`: `i64`

*Defined at line 145*

---

## phys_read_le

```fides
pub fn phys_read_le(m: &PhysMem, pa: i64, n: i64) -> i64 {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 162*

---

## phys_write_le

```fides
pub fn phys_write_le(m: &mut PhysMem, pa: i64, n: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `n`: `i64`
- `v`: `i64`

*Defined at line 185*

---

## phys_write_bytes

Copy bytes (one per element) into physical memory.

```fides
pub fn phys_write_bytes(m: &mut PhysMem, pa: i64, bytes: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`
- `bytes`: `Vec<i64>`

*Defined at line 207*

---

## phys_read_bytes

Read n bytes from physical memory.

```fides
pub fn phys_read_bytes(m: &PhysMem, pa: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&PhysMem`
- `pa`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 216*

---

## phys_zero_frame

Zero one 4-KiB frame.

```fides
pub fn phys_zero_frame(m: &mut PhysMem, pa: i64) {
```

**Parameters:**

- `m`: `&mut PhysMem`
- `pa`: `i64`

*Defined at line 227*

---

## FrameAlloc

```fides
pub struct FrameAlloc {
```

**Fields:**

| Name | Type |
|------|------|
| `pmm` | `Pmm` |
| `base` | `i64` |

*Defined at line 237*

---

## framealloc_new

Allocator for `frames` frames starting at physical `base`.

```fides
pub fn framealloc_new(base: i64, frames: i64) -> FrameAlloc {
```

**Parameters:**

- `base`: `i64`
- `frames`: `i64`

**Returns:** `FrameAlloc`

*Defined at line 243*

---

## framealloc_alloc

Physical address of a free frame, -1 when none is left.

```fides
pub fn framealloc_alloc(fa: &mut FrameAlloc) -> i64 {
```

**Parameters:**

- `fa`: `&mut FrameAlloc`

**Returns:** `i64`

*Defined at line 250*

---

## framealloc_free

```fides
pub fn framealloc_free(fa: &mut FrameAlloc, pa: i64) {
```

**Parameters:**

- `fa`: `&mut FrameAlloc`
- `pa`: `i64`

*Defined at line 258*

---

## framealloc_used

Frames in use.

```fides
pub fn framealloc_used(fa: &FrameAlloc) -> i64 {
```

**Parameters:**

- `fa`: `&FrameAlloc`

**Returns:** `i64`

*Defined at line 263*

---

## vmm_levels

Number of translation levels (the root is level levels-1, leaves of
4-KiB pages are level 0).

```fides
pub fn vmm_levels(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 271*

---

## vmm_va_bits

Bits of virtual address the architecture translates.

```fides
pub fn vmm_va_bits(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 279*

---

## vmm_index

Table index of va at `level`.

```fides
pub fn vmm_index(va: i64, level: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`
- `level`: `i64`

**Returns:** `i64`

*Defined at line 287*

---

## vmm_level_size

Bytes mapped by one leaf entry at `level`.

```fides
pub fn vmm_level_size(level: i64) -> i64 {
```

**Parameters:**

- `level`: `i64`

**Returns:** `i64`

*Defined at line 292*

---

## vmm_leaf_allowed

Whether a leaf (large page / block) may sit at `level`.

```fides
pub fn vmm_leaf_allowed(arch: i64, level: i64) -> bool {
```

**Parameters:**

- `arch`: `i64`
- `level`: `i64`

**Returns:** `bool`

*Defined at line 297*

---

## vmm_canonical

```fides
pub fn vmm_canonical(arch: i64, va: i64) -> bool {
```

**Parameters:**

- `arch`: `i64`
- `va`: `i64`

**Returns:** `bool`

*Defined at line 310*

---

## VmEntry

```fides
pub struct VmEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `leaf` | `bool` |
| `pa` | `i64` |
| `flags` | `i64` |

*Defined at line 321*

---

## vmm_entry_encode

Hardware entry for a table pointer (leaf=false) or a mapping of pa
with VM_* flags at `level`.

```fides
pub fn vmm_entry_encode(arch: i64, level: i64, pa: i64, flags: i64, leaf: bool) -> i64 {
```

**Parameters:**

- `arch`: `i64`
- `level`: `i64`
- `pa`: `i64`
- `flags`: `i64`
- `leaf`: `bool`

**Returns:** `i64`

*Defined at line 516*

---

## vmm_entry_decode

Decode an entry read from a table at `level`.

```fides
pub fn vmm_entry_decode(arch: i64, level: i64, e: i64) -> VmEntry {
```

**Parameters:**

- `arch`: `i64`
- `level`: `i64`
- `e`: `i64`

**Returns:** `VmEntry`

*Defined at line 527*

---

## vmm_flags_str

Flags as text, one letter per flag or '-': r w x u g c(nocache) a d.

```fides
pub fn vmm_flags_str(flags: i64) -> str {
```

**Parameters:**

- `flags`: `i64`

**Returns:** `str`

*Defined at line 538*

---

## AddrSpace

```fides
pub struct AddrSpace {
```

**Fields:**

| Name | Type |
|------|------|
| `arch` | `i64` |
| `root` | `i64` |
| `root_hi` | `i64` |
| `asid` | `i64` |
| `tables` | `i64` |
| `mapped` | `i64,  // bytes mapped` |

*Defined at line 555*

---

## Translation

```fides
pub struct Translation {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pa` | `i64` |
| `flags` | `i64` |
| `level` | `i64` |
| `page_size` | `i64` |
| `entry_pa` | `i64,  // where the leaf entry lives` |

*Defined at line 567*

---

## vmm_new

A fresh, empty address space. root is -1 when no frame is free.

```fides
pub fn vmm_new(arch: i64, mem: &mut PhysMem, fa: &mut FrameAlloc) -> AddrSpace {
```

**Parameters:**

- `arch`: `i64`
- `mem`: `&mut PhysMem`
- `fa`: `&mut FrameAlloc`

**Returns:** `AddrSpace`

*Defined at line 586*

---

## vmm_map_page

Map [va, va+size) to [pa, pa+size) with one leaf entry; size is 4 KiB
or a large page size the architecture allows.

```fides
pub fn vmm_map_page(a: &mut AddrSpace, mem: &mut PhysMem, fa: &mut FrameAlloc, va: i64, pa: i64, size: i64, flags: i64) -> i64 {
```

**Parameters:**

- `a`: `&mut AddrSpace`
- `mem`: `&mut PhysMem`
- `fa`: `&mut FrameAlloc`
- `va`: `i64`
- `pa`: `i64`
- `size`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 616*

---

## vmm_best_size

Largest leaf size usable at va/pa with `left` bytes still to map.

```fides
pub fn vmm_best_size(arch: i64, va: i64, pa: i64, left: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`
- `va`: `i64`
- `pa`: `i64`
- `left`: `i64`

**Returns:** `i64`

*Defined at line 665*

---

## vmm_map

Map a range (4-KiB aligned) using large pages where alignment allows.
Returns the number of leaf entries written, or an error.

```fides
pub fn vmm_map(a: &mut AddrSpace, mem: &mut PhysMem, fa: &mut FrameAlloc, va: i64, pa: i64, len: i64, flags: i64) -> i64 {
```

**Parameters:**

- `a`: `&mut AddrSpace`
- `mem`: `&mut PhysMem`
- `fa`: `&mut FrameAlloc`
- `va`: `i64`
- `pa`: `i64`
- `len`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 679*

---

## vmm_translate

Walk the tables for va.

```fides
pub fn vmm_translate(a: &AddrSpace, mem: &PhysMem, va: i64) -> Translation {
```

**Parameters:**

- `a`: `&AddrSpace`
- `mem`: `&PhysMem`
- `va`: `i64`

**Returns:** `Translation`

*Defined at line 698*

---

## vmm_unmap

Remove the mapping holding va (a whole large page when va is in one)
and free page tables left empty. Returns the size unmapped or an error.
The caller then invalidates the TLB entry for va.

```fides
pub fn vmm_unmap(a: &mut AddrSpace, mem: &mut PhysMem, fa: &mut FrameAlloc, va: i64) -> i64 {
```

**Parameters:**

- `a`: `&mut AddrSpace`
- `mem`: `&mut PhysMem`
- `fa`: `&mut FrameAlloc`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 746*

---

## vmm_protect

Replace the flags of the mapping holding va.

```fides
pub fn vmm_protect(a: &mut AddrSpace, mem: &mut PhysMem, va: i64, flags: i64) -> i64 {
```

**Parameters:**

- `a`: `&mut AddrSpace`
- `mem`: `&mut PhysMem`
- `va`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 789*

---

## vmm_cr3

CR3: root table address with a PCID (CR4.PCIDE) in bits 11:0.

```fides
pub fn vmm_cr3(a: &AddrSpace, pcid: i64) -> i64 {
```

**Parameters:**

- `a`: `&AddrSpace`
- `pcid`: `i64`

**Returns:** `i64`

*Defined at line 802*

---

## vmm_satp

```fides
pub fn vmm_satp(a: &AddrSpace) -> i64 {
```

**Parameters:**

- `a`: `&AddrSpace`

**Returns:** `i64`

*Defined at line 808*

---

## vmm_ttbr

```fides
pub fn vmm_ttbr(table: i64, asid: i64) -> i64 {
```

**Parameters:**

- `table`: `i64`
- `asid`: `i64`

**Returns:** `i64`

*Defined at line 818*

---

## vmm_mair_el1

MAIR_EL1 matching the encodings above: Attr0 = normal write-back
(0xFF), Attr1 = Device-nGnRE (0x04).

```fides
pub fn vmm_mair_el1() -> i64 {
```

**Returns:** `i64`

*Defined at line 824*

---

## vmm_tcr_el1

TCR_EL1 for 48-bit VAs in both halves with 4-KiB granules, write-back
inner-shareable walks, and IPS from the physical address width.

```fides
pub fn vmm_tcr_el1(pa_bits: i64) -> i64 {
```

**Parameters:**

- `pa_bits`: `i64`

**Returns:** `i64`

*Defined at line 830*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
