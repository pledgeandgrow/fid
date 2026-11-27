[All modules](index.md)

# elf_loader -- Fides Documentation

## Contents

- [PT_NULL:](#PT_NULL:) (const)
- [PT_LOAD:](#PT_LOAD:) (const)
- [PT_DYNAMIC:](#PT_DYNAMIC:) (const)
- [PT_INTERP:](#PT_INTERP:) (const)
- [PT_PHDR:](#PT_PHDR:) (const)
- [PT_TLS:](#PT_TLS:) (const)
- [PT_GNU_STACK:](#PT_GNU_STACK:) (const)
- [PT_GNU_RELRO:](#PT_GNU_RELRO:) (const)
- [PF_X:](#PF_X:) (const)
- [PF_W:](#PF_W:) (const)
- [PF_R:](#PF_R:) (const)
- [ET_EXEC:](#ET_EXEC:) (const)
- [ET_DYN:](#ET_DYN:) (const)
- [EM_X86_64:](#EM_X86_64:) (const)
- [EM_AARCH64:](#EM_AARCH64:) (const)
- [EM_RISCV:](#EM_RISCV:) (const)
- [DT_NULL:](#DT_NULL:) (const)
- [DT_NEEDED:](#DT_NEEDED:) (const)
- [DT_PLTRELSZ:](#DT_PLTRELSZ:) (const)
- [DT_PLTGOT:](#DT_PLTGOT:) (const)
- [DT_HASH:](#DT_HASH:) (const)
- [DT_STRTAB:](#DT_STRTAB:) (const)
- [DT_SYMTAB:](#DT_SYMTAB:) (const)
- [DT_RELA:](#DT_RELA:) (const)
- [DT_RELASZ:](#DT_RELASZ:) (const)
- [DT_RELAENT:](#DT_RELAENT:) (const)
- [DT_STRSZ:](#DT_STRSZ:) (const)
- [DT_SYMENT:](#DT_SYMENT:) (const)
- [DT_INIT:](#DT_INIT:) (const)
- [DT_FINI:](#DT_FINI:) (const)
- [DT_SONAME:](#DT_SONAME:) (const)
- [DT_REL:](#DT_REL:) (const)
- [DT_RELSZ:](#DT_RELSZ:) (const)
- [DT_RELENT:](#DT_RELENT:) (const)
- [DT_PLTREL:](#DT_PLTREL:) (const)
- [DT_JMPREL:](#DT_JMPREL:) (const)
- [DT_INIT_ARRAY:](#DT_INIT_ARRAY:) (const)
- [DT_FINI_ARRAY:](#DT_FINI_ARRAY:) (const)
- [DT_INIT_ARRAYSZ:](#DT_INIT_ARRAYSZ:) (const)
- [DT_FINI_ARRAYSZ:](#DT_FINI_ARRAYSZ:) (const)
- [DT_GNU_HASH:](#DT_GNU_HASH:) (const)
- [R_X86_64_NONE:](#R_X86_64_NONE:) (const)
- [R_X86_64_64:](#R_X86_64_64:) (const)
- [R_X86_64_PC32:](#R_X86_64_PC32:) (const)
- [R_X86_64_GLOB_DAT:](#R_X86_64_GLOB_DAT:) (const)
- [R_X86_64_JUMP_SLOT:](#R_X86_64_JUMP_SLOT:) (const)
- [R_X86_64_RELATIVE:](#R_X86_64_RELATIVE:) (const)
- [R_AARCH64_NONE:](#R_AARCH64_NONE:) (const)
- [R_AARCH64_ABS64:](#R_AARCH64_ABS64:) (const)
- [R_AARCH64_GLOB_DAT:](#R_AARCH64_GLOB_DAT:) (const)
- [R_AARCH64_JUMP_SLOT:](#R_AARCH64_JUMP_SLOT:) (const)
- [R_AARCH64_RELATIVE:](#R_AARCH64_RELATIVE:) (const)
- [R_RISCV_NONE:](#R_RISCV_NONE:) (const)
- [R_RISCV_64:](#R_RISCV_64:) (const)
- [R_RISCV_RELATIVE:](#R_RISCV_RELATIVE:) (const)
- [R_RISCV_JUMP_SLOT:](#R_RISCV_JUMP_SLOT:) (const)
- [ELF_PAGE:](#ELF_PAGE:) (const)
- [ElfImage](#ElfImage) (struct)
- [ElfSym](#ElfSym) (struct)
- [elf_img_in](#elf_img_in) (function)
- [elf_img_u8](#elf_img_u8) (function)
- [elf_img_u16](#elf_img_u16) (function)
- [elf_img_u32](#elf_img_u32) (function)
- [elf_img_u64](#elf_img_u64) (function)
- [elf_img_put64](#elf_img_put64) (function)
- [elf_img_put32](#elf_img_put32) (function)
- [elf_img_str](#elf_img_str) (function)
- [elf_addr](#elf_addr) (function)
- [elf_machine_name](#elf_machine_name) (function)
- [elf_load](#elf_load) (function)
- [elf_sysv_hash](#elf_sysv_hash) (function)
- [elf_gnu_hash](#elf_gnu_hash) (function)
- [elf_sym](#elf_sym) (function)
- [elf_lookup_sysv](#elf_lookup_sysv) (function)
- [elf_lookup_gnu](#elf_lookup_gnu) (function)
- [elf_lookup](#elf_lookup) (function)
- [elf_sym_count](#elf_sym_count) (function)
- [elf_relocate](#elf_relocate) (function)
- [elf_init_functions](#elf_init_functions) (function)
- [elf_perms_at](#elf_perms_at) (function)

## PT_NULL:

```fides
pub const PT_NULL: i64 = 0;
```

*Defined at line 33*

---

## PT_LOAD:

```fides
pub const PT_LOAD: i64 = 1;
```

*Defined at line 34*

---

## PT_DYNAMIC:

```fides
pub const PT_DYNAMIC: i64 = 2;
```

*Defined at line 35*

---

## PT_INTERP:

```fides
pub const PT_INTERP: i64 = 3;
```

*Defined at line 36*

---

## PT_PHDR:

```fides
pub const PT_PHDR: i64 = 6;
```

*Defined at line 37*

---

## PT_TLS:

```fides
pub const PT_TLS: i64 = 7;
```

*Defined at line 38*

---

## PT_GNU_STACK:

```fides
pub const PT_GNU_STACK: i64 = 1685382481;
```

*Defined at line 39*

---

## PT_GNU_RELRO:

```fides
pub const PT_GNU_RELRO: i64 = 1685382482;
```

*Defined at line 40*

---

## PF_X:

```fides
pub const PF_X: i64 = 1;
```

*Defined at line 42*

---

## PF_W:

```fides
pub const PF_W: i64 = 2;
```

*Defined at line 43*

---

## PF_R:

```fides
pub const PF_R: i64 = 4;
```

*Defined at line 44*

---

## ET_EXEC:

```fides
pub const ET_EXEC: i64 = 2;
```

*Defined at line 46*

---

## ET_DYN:

```fides
pub const ET_DYN: i64 = 3;
```

*Defined at line 47*

---

## EM_X86_64:

```fides
pub const EM_X86_64: i64 = 62;
```

*Defined at line 49*

---

## EM_AARCH64:

```fides
pub const EM_AARCH64: i64 = 183;
```

*Defined at line 50*

---

## EM_RISCV:

```fides
pub const EM_RISCV: i64 = 243;
```

*Defined at line 51*

---

## DT_NULL:

```fides
pub const DT_NULL: i64 = 0;
```

*Defined at line 53*

---

## DT_NEEDED:

```fides
pub const DT_NEEDED: i64 = 1;
```

*Defined at line 54*

---

## DT_PLTRELSZ:

```fides
pub const DT_PLTRELSZ: i64 = 2;
```

*Defined at line 55*

---

## DT_PLTGOT:

```fides
pub const DT_PLTGOT: i64 = 3;
```

*Defined at line 56*

---

## DT_HASH:

```fides
pub const DT_HASH: i64 = 4;
```

*Defined at line 57*

---

## DT_STRTAB:

```fides
pub const DT_STRTAB: i64 = 5;
```

*Defined at line 58*

---

## DT_SYMTAB:

```fides
pub const DT_SYMTAB: i64 = 6;
```

*Defined at line 59*

---

## DT_RELA:

```fides
pub const DT_RELA: i64 = 7;
```

*Defined at line 60*

---

## DT_RELASZ:

```fides
pub const DT_RELASZ: i64 = 8;
```

*Defined at line 61*

---

## DT_RELAENT:

```fides
pub const DT_RELAENT: i64 = 9;
```

*Defined at line 62*

---

## DT_STRSZ:

```fides
pub const DT_STRSZ: i64 = 10;
```

*Defined at line 63*

---

## DT_SYMENT:

```fides
pub const DT_SYMENT: i64 = 11;
```

*Defined at line 64*

---

## DT_INIT:

```fides
pub const DT_INIT: i64 = 12;
```

*Defined at line 65*

---

## DT_FINI:

```fides
pub const DT_FINI: i64 = 13;
```

*Defined at line 66*

---

## DT_SONAME:

```fides
pub const DT_SONAME: i64 = 14;
```

*Defined at line 67*

---

## DT_REL:

```fides
pub const DT_REL: i64 = 17;
```

*Defined at line 68*

---

## DT_RELSZ:

```fides
pub const DT_RELSZ: i64 = 18;
```

*Defined at line 69*

---

## DT_RELENT:

```fides
pub const DT_RELENT: i64 = 19;
```

*Defined at line 70*

---

## DT_PLTREL:

```fides
pub const DT_PLTREL: i64 = 20;
```

*Defined at line 71*

---

## DT_JMPREL:

```fides
pub const DT_JMPREL: i64 = 23;
```

*Defined at line 72*

---

## DT_INIT_ARRAY:

```fides
pub const DT_INIT_ARRAY: i64 = 25;
```

*Defined at line 73*

---

## DT_FINI_ARRAY:

```fides
pub const DT_FINI_ARRAY: i64 = 26;
```

*Defined at line 74*

---

## DT_INIT_ARRAYSZ:

```fides
pub const DT_INIT_ARRAYSZ: i64 = 27;
```

*Defined at line 75*

---

## DT_FINI_ARRAYSZ:

```fides
pub const DT_FINI_ARRAYSZ: i64 = 28;
```

*Defined at line 76*

---

## DT_GNU_HASH:

```fides
pub const DT_GNU_HASH: i64 = 1879047925;
```

*Defined at line 77*

---

## R_X86_64_NONE:

```fides
pub const R_X86_64_NONE: i64 = 0;
```

*Defined at line 79*

---

## R_X86_64_64:

```fides
pub const R_X86_64_64: i64 = 1;
```

*Defined at line 80*

---

## R_X86_64_PC32:

```fides
pub const R_X86_64_PC32: i64 = 2;
```

*Defined at line 81*

---

## R_X86_64_GLOB_DAT:

```fides
pub const R_X86_64_GLOB_DAT: i64 = 6;
```

*Defined at line 82*

---

## R_X86_64_JUMP_SLOT:

```fides
pub const R_X86_64_JUMP_SLOT: i64 = 7;
```

*Defined at line 83*

---

## R_X86_64_RELATIVE:

```fides
pub const R_X86_64_RELATIVE: i64 = 8;
```

*Defined at line 84*

---

## R_AARCH64_NONE:

```fides
pub const R_AARCH64_NONE: i64 = 0;
```

*Defined at line 85*

---

## R_AARCH64_ABS64:

```fides
pub const R_AARCH64_ABS64: i64 = 257;
```

*Defined at line 86*

---

## R_AARCH64_GLOB_DAT:

```fides
pub const R_AARCH64_GLOB_DAT: i64 = 1025;
```

*Defined at line 87*

---

## R_AARCH64_JUMP_SLOT:

```fides
pub const R_AARCH64_JUMP_SLOT: i64 = 1026;
```

*Defined at line 88*

---

## R_AARCH64_RELATIVE:

```fides
pub const R_AARCH64_RELATIVE: i64 = 1027;
```

*Defined at line 89*

---

## R_RISCV_NONE:

```fides
pub const R_RISCV_NONE: i64 = 0;
```

*Defined at line 90*

---

## R_RISCV_64:

```fides
pub const R_RISCV_64: i64 = 2;
```

*Defined at line 91*

---

## R_RISCV_RELATIVE:

```fides
pub const R_RISCV_RELATIVE: i64 = 3;
```

*Defined at line 92*

---

## R_RISCV_JUMP_SLOT:

```fides
pub const R_RISCV_JUMP_SLOT: i64 = 5;
```

*Defined at line 93*

---

## ELF_PAGE:

```fides
pub const ELF_PAGE: i64 = 4096;
```

*Defined at line 95*

---

## ElfImage

```fides
pub struct ElfImage {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `etype` | `i64` |
| `machine` | `i64` |
| `base` | `i64` |
| `vmin` | `i64` |
| `mem` | `Vec<i64>` |
| `entry` | `i64` |
| `segs` | `Vec<i64>` |
| `interp` | `str` |
| `dynamic` | `i64` |
| `needed` | `Vec<str>` |
| `soname` | `str` |
| `strtab` | `i64` |
| `strsz` | `i64` |
| `symtab` | `i64` |
| `syment` | `i64` |
| `hash` | `i64` |
| `gnu_hash` | `i64` |
| `rela` | `i64` |
| `relasz` | `i64` |
| `relaent` | `i64` |
| `rel` | `i64` |
| `relsz` | `i64` |
| `relent` | `i64` |
| `jmprel` | `i64` |
| `pltrelsz` | `i64` |
| `pltrel` | `i64` |
| `init` | `i64` |
| `fini` | `i64` |
| `init_array` | `i64` |
| `init_arraysz` | `i64` |
| `unresolved` | `Vec<str>` |

*Defined at line 97*

---

## ElfSym

```fides
pub struct ElfSym {
```

**Fields:**

| Name | Type |
|------|------|
| `found` | `bool` |
| `index` | `i64` |
| `name` | `str` |
| `value` | `i64` |
| `size` | `i64` |
| `info` | `i64` |
| `shndx` | `i64` |

*Defined at line 132*

---

## elf_img_in

```fides
pub fn elf_img_in(img: &ElfImage, va: i64, n: i64) -> bool {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`
- `n`: `i64`

**Returns:** `bool`

*Defined at line 154*

---

## elf_img_u8

```fides
pub fn elf_img_u8(img: &ElfImage, va: i64) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 159*

---

## elf_img_u16

```fides
pub fn elf_img_u16(img: &ElfImage, va: i64) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 163*

---

## elf_img_u32

```fides
pub fn elf_img_u32(img: &ElfImage, va: i64) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 167*

---

## elf_img_u64

```fides
pub fn elf_img_u64(img: &ElfImage, va: i64) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 172*

---

## elf_img_put64

```fides
pub fn elf_img_put64(img: &mut ElfImage, va: i64, v: i64) {
```

**Parameters:**

- `img`: `&mut ElfImage`
- `va`: `i64`
- `v`: `i64`

*Defined at line 176*

---

## elf_img_put32

```fides
pub fn elf_img_put32(img: &mut ElfImage, va: i64, v: i64) {
```

**Parameters:**

- `img`: `&mut ElfImage`
- `va`: `i64`
- `v`: `i64`

*Defined at line 185*

---

## elf_img_str

A NUL-terminated string at link-time address `va`.

```fides
pub fn elf_img_str(img: &ElfImage, va: i64) -> str {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `str`

*Defined at line 195*

---

## elf_addr

The run-time address of link-time address `va`.

```fides
pub fn elf_addr(img: &ElfImage, va: i64) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `i64`

*Defined at line 210*

---

## elf_machine_name

```fides
pub fn elf_machine_name(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 216*

---

## elf_load

Load `b` (the file's bytes). `base` is the load bias for ET_DYN (page
aligned); ET_EXEC objects load where they were linked.

```fides
pub fn elf_load(b: Vec<i64>, base: i64) -> ElfImage {
```

**Parameters:**

- `b`: `Vec<i64>`
- `base`: `i64`

**Returns:** `ElfImage`

*Defined at line 231*

---

## elf_sysv_hash

The SysV ELF hash (DT_HASH).

```fides
pub fn elf_sysv_hash(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 405*

---

## elf_gnu_hash

The GNU hash (DT_GNU_HASH): djb2, h * 33 + c, 32 bits.

```fides
pub fn elf_gnu_hash(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 421*

---

## elf_sym

Dynamic symbol `idx`.

```fides
pub fn elf_sym(img: &ElfImage, idx: i64) -> ElfSym {
```

**Parameters:**

- `img`: `&ElfImage`
- `idx`: `i64`

**Returns:** `ElfSym`

*Defined at line 432*

---

## elf_lookup_sysv

Look a name up through DT_HASH.

```fides
pub fn elf_lookup_sysv(img: &ElfImage, name: str) -> ElfSym {
```

**Parameters:**

- `img`: `&ElfImage`
- `name`: `str`

**Returns:** `ElfSym`

*Defined at line 451*

---

## elf_lookup_gnu

Look a name up through DT_GNU_HASH (bloom filter first).

```fides
pub fn elf_lookup_gnu(img: &ElfImage, name: str) -> ElfSym {
```

**Parameters:**

- `img`: `&ElfImage`
- `name`: `str`

**Returns:** `ElfSym`

*Defined at line 475*

---

## elf_lookup

A symbol the object defines (GNU hash preferred, SysV otherwise):
its run-time address, or -1.

```fides
pub fn elf_lookup(img: &ElfImage, name: str) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`
- `name`: `str`

**Returns:** `i64`

*Defined at line 520*

---

## elf_sym_count

How many dynamic symbols there are (DT_HASH's nchain, or the end of
the last GNU hash chain).

```fides
pub fn elf_sym_count(img: &ElfImage) -> i64 {
```

**Parameters:**

- `img`: `&ElfImage`

**Returns:** `i64`

*Defined at line 533*

---

## elf_relocate

Apply every relocation (DT_RELA, DT_REL and the PLT ones) now.
Returns how many were applied, or -1 (see img.err and img.unresolved).

```fides
pub fn elf_relocate(img: &mut ElfImage, resolve: fn(str) -> i64) -> i64 {
```

**Parameters:**

- `img`: `&mut ElfImage`
- `resolve`: `fn(str) -> i64`

**Returns:** `i64`

*Defined at line 687*

---

## elf_init_functions

The run-time addresses of the init array (after relocation), to be
called in order by whoever runs the object.

```fides
pub fn elf_init_functions(img: &ElfImage) -> Vec<i64> {
```

**Parameters:**

- `img`: `&ElfImage`

**Returns:** `Vec<i64>`

*Defined at line 714*

---

## elf_perms_at

"R-X"-style permissions of the segment containing `va`, "" outside.

```fides
pub fn elf_perms_at(img: &ElfImage, va: i64) -> str {
```

**Parameters:**

- `img`: `&ElfImage`
- `va`: `i64`

**Returns:** `str`

*Defined at line 728*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
