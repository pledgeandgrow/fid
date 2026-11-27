[All modules](index.md)

# compiler::image_writer -- Fides Documentation

## Contents

- [img_u8](#img_u8) (function)
- [img_u16](#img_u16) (function)
- [img_u32](#img_u32) (function)
- [img_u64](#img_u64) (function)
- [img_be_u32](#img_be_u32) (function)
- [img_be_u64](#img_be_u64) (function)
- [img_set_u16](#img_set_u16) (function)
- [img_set_u32](#img_set_u32) (function)
- [img_set_u64](#img_set_u64) (function)
- [img_get_u32](#img_get_u32) (function)
- [img_uleb](#img_uleb) (function)
- [img_bytes](#img_bytes) (function)
- [img_cstr](#img_cstr) (function)
- [img_name_field](#img_name_field) (function)
- [img_set_name_field](#img_set_name_field) (function)
- [img_pad_to](#img_pad_to) (function)
- [img_align](#img_align) (function)
- [img_append](#img_append) (function)
- [img_append_range](#img_append_range) (function)
- [img_place_parts](#img_place_parts) (function)
- [img_round](#img_round) (function)
- [IMG_SYM_NONE:](#IMG_SYM_NONE:) (const)
- [IMG_SYM_FUNC:](#IMG_SYM_FUNC:) (const)
- [IMG_SYM_SECTION:](#IMG_SYM_SECTION:) (const)
- [IMG_LOCAL:](#IMG_LOCAL:) (const)
- [IMG_GLOBAL:](#IMG_GLOBAL:) (const)
- [ImgSym](#ImgSym) (struct)
- [img_sym](#img_sym) (function)
- [img_func_syms](#img_func_syms) (function)
- [img_str_add](#img_str_add) (function)
- [img_elf_sym](#img_elf_sym) (function)
- [img_elf_symtab](#img_elf_symtab) (function)
- [img_elf_first_global](#img_elf_first_global) (function)
- [img_elf_rela](#img_elf_rela) (function)
- [IMG_ELF_SHOFF_AT:](#IMG_ELF_SHOFF_AT:) (const)
- [IMG_ELF_SHNUM_AT:](#IMG_ELF_SHNUM_AT:) (const)
- [IMG_ELF_SHSTRNDX_AT:](#IMG_ELF_SHSTRNDX_AT:) (const)
- [img_elf_header](#img_elf_header) (function)
- [img_elf_phdr](#img_elf_phdr) (function)
- [ImgSec](#ImgSec) (struct)
- [img_sec](#img_sec) (function)
- [img_elf_shstrtab](#img_elf_shstrtab) (function)
- [img_elf_shdr](#img_elf_shdr) (function)
- [img_elf_sections](#img_elf_sections) (function)
- [img_elf_sections_named](#img_elf_sections_named) (function)
- [img_sec_names](#img_sec_names) (function)
- [IMG_PE_NSECT_AT:](#IMG_PE_NSECT_AT:) (const)
- [IMG_PE_SYMPTR_AT:](#IMG_PE_SYMPTR_AT:) (const)
- [IMG_PE_NSYMS_AT:](#IMG_PE_NSYMS_AT:) (const)
- [IMG_PE_OPT_AT:](#IMG_PE_OPT_AT:) (const)
- [IMG_PE_DIRS_AT:](#IMG_PE_DIRS_AT:) (const)
- [img_pe_dos](#img_pe_dos) (function)
- [img_pe_signature](#img_pe_signature) (function)
- [img_coff_header](#img_coff_header) (function)
- [ImgPeOpt](#ImgPeOpt) (struct)
- [img_pe_optional](#img_pe_optional) (function)
- [img_pe_dirs](#img_pe_dirs) (function)
- [img_pe_dir](#img_pe_dir) (function)
- [img_pe_section](#img_pe_section) (function)
- [img_coff_section](#img_coff_section) (function)
- [img_coff_reloc](#img_coff_reloc) (function)
- [IMG_COFF_SYMPTR_AT:](#IMG_COFF_SYMPTR_AT:) (const)
- [IMG_COFF_NSYMS_AT:](#IMG_COFF_NSYMS_AT:) (const)
- [img_pe_section_at](#img_pe_section_at) (function)
- [img_coff_symbols](#img_coff_symbols) (function)
- [img_pe_base_relocs](#img_pe_base_relocs) (function)
- [img_macho_header](#img_macho_header) (function)
- [img_macho_segment](#img_macho_segment) (function)
- [img_macho_section](#img_macho_section) (function)
- [img_macho_section_rel](#img_macho_section_rel) (function)
- [img_macho_reloc](#img_macho_reloc) (function)
- [img_macho_nlist](#img_macho_nlist) (function)
- [img_macho_symtab](#img_macho_symtab) (function)
- [img_macho_symtab_cmd](#img_macho_symtab_cmd) (function)
- [img_macho_dysymtab_cmd](#img_macho_dysymtab_cmd) (function)
- [img_macho_dyld_info_cmd](#img_macho_dyld_info_cmd) (function)
- [img_macho_rebase](#img_macho_rebase) (function)
- [img_macho_bind](#img_macho_bind) (function)
- [img_macho_bind_ords](#img_macho_bind_ords) (function)
- [img_fat_arch](#img_fat_arch) (function)

## img_u8

```fides
pub fn img_u8(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 42*

---

## img_u16

```fides
pub fn img_u16(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 46*

---

## img_u32

```fides
pub fn img_u32(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 51*

---

## img_u64

```fides
pub fn img_u64(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 56*

---

## img_be_u32

```fides
pub fn img_be_u32(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 61*

---

## img_be_u64

```fides
pub fn img_be_u64(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 68*

---

## img_set_u16

```fides
pub fn img_set_u16(out: &mut Vec<i64>, at: i64, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `at`: `i64`
- `v`: `i64`

*Defined at line 76*

---

## img_set_u32

```fides
pub fn img_set_u32(out: &mut Vec<i64>, at: i64, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `at`: `i64`
- `v`: `i64`

*Defined at line 81*

---

## img_set_u64

```fides
pub fn img_set_u64(out: &mut Vec<i64>, at: i64, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `at`: `i64`
- `v`: `i64`

*Defined at line 89*

---

## img_get_u32

```fides
pub fn img_get_u32(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## img_uleb

```fides
pub fn img_uleb(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 98*

---

## img_bytes

The bytes of `s`, without a NUL.

```fides
pub fn img_bytes(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 112*

---

## img_cstr

The bytes of `s` and a NUL.

```fides
pub fn img_cstr(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 121*

---

## img_name_field

`s` in a field of `width` bytes: cut there, NUL-padded.

```fides
pub fn img_name_field(out: &mut Vec<i64>, s: str, width: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`
- `width`: `i64`

*Defined at line 127*

---

## img_set_name_field

The same, set at `at` (headers written in place).

```fides
pub fn img_set_name_field(out: &mut Vec<i64>, at: i64, s: str, width: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `at`: `i64`
- `s`: `str`
- `width`: `i64`

*Defined at line 140*

---

## img_pad_to

Zeros until `out` is `len` bytes long (nothing when it is longer).

```fides
pub fn img_pad_to(out: &mut Vec<i64>, len: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `len`: `i64`

*Defined at line 153*

---

## img_align

Zeros until the length of `out` is a multiple of `align`.

```fides
pub fn img_align(out: &mut Vec<i64>, align: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `align`: `i64`

*Defined at line 160*

---

## img_append

```fides
pub fn img_append(out: &mut Vec<i64>, src: &Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `src`: `&Vec<i64>`

*Defined at line 166*

---

## img_append_range

Bytes [from, to) of `src`.

```fides
pub fn img_append_range(out: &mut Vec<i64>, src: &Vec<i64>, from: i64, to: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `src`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`

*Defined at line 175*

---

## img_place_parts

Each of `parts` after `out`, each starting at a multiple of `align` and
`out` ending at one; the file offset of each part.

```fides
pub fn img_place_parts(out: &mut Vec<i64>, parts: &Vec<Vec<i64>>, align: i64) -> Vec<i64> {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `parts`: `&Vec<Vec<i64>>`
- `align`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 185*

---

## img_round

Round `v` up to a multiple of `n` (any n > 0).

```fides
pub fn img_round(v: i64, n: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 200*

---

## IMG_SYM_NONE:

```fides
pub const IMG_SYM_NONE: i64 = 0;
```

*Defined at line 208*

---

## IMG_SYM_FUNC:

```fides
pub const IMG_SYM_FUNC: i64 = 2;
```

*Defined at line 209*

---

## IMG_SYM_SECTION:

```fides
pub const IMG_SYM_SECTION: i64 = 3;  // = ELF STT_*
```

*Defined at line 210*

---

## IMG_LOCAL:

```fides
pub const IMG_LOCAL: i64 = 0;
```

*Defined at line 211*

---

## IMG_GLOBAL:

```fides
pub const IMG_GLOBAL: i64 = 1;  // = ELF STB_*
```

*Defined at line 212*

---

## ImgSym

A symbol: `value` is what the format records (an address, or an offset
in section `sect`, numbered from 1 in every format), `size` its length
(ELF only). A section symbol has no name.

```fides
pub struct ImgSym {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `i64` |
| `size` | `i64` |
| `sect` | `i64` |
| `kind` | `i64` |
| `bind` | `i64` |

*Defined at line 217*

---

## img_sym

```fides
pub fn img_sym(name: str, value: i64, size: i64, sect: i64, kind: i64, bind: i64) -> ImgSym {
```

**Parameters:**

- `name`: `str`
- `value`: `i64`
- `size`: `i64`
- `sect`: `i64`
- `kind`: `i64`
- `bind`: `i64`

**Returns:** `ImgSym`

*Defined at line 226*

---

## img_func_syms

The functions `names` at `starts[i] + base` (sizes ends[i] - starts[i];
no `ends`: 0), in section `sect`, all with binding `bind`.

```fides
pub fn img_func_syms(names: &Vec<str>, starts: &Vec<i64>, ends: &Vec<i64>, base: i64, sect: i64, bind: i64) -> Vec<ImgSym> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `&Vec<i64>`
- `base`: `i64`
- `sect`: `i64`
- `bind`: `i64`

**Returns:** `Vec<ImgSym>`

*Defined at line 234*

---

## img_str_add

Add `s` (and its NUL) to the string table `tab`; its offset.

```fides
pub fn img_str_add(tab: &mut Vec<i64>, s: str) -> i64 {
```

**Parameters:**

- `tab`: `&mut Vec<i64>`
- `s`: `str`

**Returns:** `i64`

*Defined at line 249*

---

## img_elf_sym

One Elf64_Sym (24 bytes).

```fides
pub fn img_elf_sym(out: &mut Vec<i64>, name: i64, info: i64, shndx: i64, value: i64, size: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `i64`
- `info`: `i64`
- `shndx`: `i64`
- `value`: `i64`
- `size`: `i64`

*Defined at line 260*

---

## img_elf_symtab

.symtab of `syms`: the null symbol, then each in order (ELF wants the
locals first: img_elf_first_global); the names go into `strtab` (which
starts with its empty name), a section symbol's name is 0.

```fides
pub fn img_elf_symtab(syms: &Vec<ImgSym>, strtab: &mut Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `syms`: `&Vec<ImgSym>`
- `strtab`: `&mut Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 272*

---

## img_elf_first_global

sh_info of .symtab: the index of the first global symbol (the null
symbol is 0), or one past the last when there is none.

```fides
pub fn img_elf_first_global(syms: &Vec<ImgSym>) -> i64 {
```

**Parameters:**

- `syms`: `&Vec<ImgSym>`

**Returns:** `i64`

*Defined at line 290*

---

## img_elf_rela

One Elf64_Rela (24 bytes).

```fides
pub fn img_elf_rela(out: &mut Vec<i64>, offset: i64, sym: i64, rtype: i64, addend: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `offset`: `i64`
- `sym`: `i64`
- `rtype`: `i64`
- `addend`: `i64`

*Defined at line 303*

---

## IMG_ELF_SHOFF_AT:

Where the ELF header keeps e_shoff, e_shnum and e_shstrndx.

```fides
pub const IMG_ELF_SHOFF_AT: i64 = 0x28;
```

*Defined at line 314*

---

## IMG_ELF_SHNUM_AT:

```fides
pub const IMG_ELF_SHNUM_AT: i64 = 0x3C;
```

*Defined at line 315*

---

## IMG_ELF_SHSTRNDX_AT:

```fides
pub const IMG_ELF_SHSTRNDX_AT: i64 = 0x3E;
```

*Defined at line 316*

---

## img_elf_header

The 64-byte ELF64 header (little-endian, version 1, System V ABI).

```fides
pub fn img_elf_header(out: &mut Vec<i64>, etype: i64, machine: i64, entry: i64, phoff: i64, shoff: i64, flags: i64, phentsize: i64, phnum: i64, shnum: i64, shstrndx: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `etype`: `i64`
- `machine`: `i64`
- `entry`: `i64`
- `phoff`: `i64`
- `shoff`: `i64`
- `flags`: `i64`
- `phentsize`: `i64`
- `phnum`: `i64`
- `shnum`: `i64`
- `shstrndx`: `i64`

*Defined at line 319*

---

## img_elf_phdr

One Elf64_Phdr (56 bytes); p_paddr = p_vaddr.

```fides
pub fn img_elf_phdr(out: &mut Vec<i64>, kind: i64, flags: i64, off: i64, vaddr: i64, filesz: i64, memsz: i64, align: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `kind`: `i64`
- `flags`: `i64`
- `off`: `i64`
- `vaddr`: `i64`
- `filesz`: `i64`
- `memsz`: `i64`
- `align`: `i64`

*Defined at line 345*

---

## ImgSec

A section of any of the formats: ELF's fields (the PE and Mach-O
writers use name, addr, off, size, flags and align).

```fides
pub struct ImgSec {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `typ` | `i64` |
| `flags` | `i64` |
| `addr` | `i64` |
| `off` | `i64` |
| `size` | `i64` |
| `link` | `i64` |
| `info` | `i64` |
| `align` | `i64` |
| `entsize` | `i64` |

*Defined at line 358*

---

## img_sec

```fides
pub fn img_sec(name: str, typ: i64, flags: i64, addr: i64, off: i64, size: i64, link: i64, info: i64, align: i64, entsize: i64) -> ImgSec {
```

**Parameters:**

- `name`: `str`
- `typ`: `i64`
- `flags`: `i64`
- `addr`: `i64`
- `off`: `i64`
- `size`: `i64`
- `link`: `i64`
- `info`: `i64`
- `align`: `i64`
- `entsize`: `i64`

**Returns:** `ImgSec`

*Defined at line 371*

---

## img_elf_shstrtab

The section header string table of sections named `names` (in section
order; "" is the null section's name, offset 0, not stored).

```fides
pub fn img_elf_shstrtab(names: &Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `names`: `&Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 379*

---

## img_elf_shdr

One Elf64_Shdr (64 bytes).

```fides
pub fn img_elf_shdr(out: &mut Vec<i64>, name: i64, typ: i64, flags: i64, addr: i64, off: i64, size: i64, link: i64, info: i64, align: i64, entsize: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `i64`
- `typ`: `i64`
- `flags`: `i64`
- `addr`: `i64`
- `off`: `i64`
- `size`: `i64`
- `link`: `i64`
- `info`: `i64`
- `align`: `i64`
- `entsize`: `i64`

*Defined at line 394*

---

## img_elf_sections

The section headers of `secs`, their names at the offsets
img_elf_shstrtab gives them; then e_shoff, e_shnum and e_shstrndx (the
section named ".shstrtab") of the ELF header at the start of `out`.

```fides
pub fn img_elf_sections(out: &mut Vec<i64>, secs: &Vec<ImgSec>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `secs`: `&Vec<ImgSec>`

*Defined at line 410*

---

## img_elf_sections_named

The section headers of `secs` with the names at `name_offs` of a
section header string table the caller made, section `shstrndx` being
that table; then e_shoff, e_shnum and e_shstrndx of the ELF header at
the start of `out`.

```fides
pub fn img_elf_sections_named(out: &mut Vec<i64>, secs: &Vec<ImgSec>, name_offs: &Vec<i64>, shstrndx: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `secs`: `&Vec<ImgSec>`
- `name_offs`: `&Vec<i64>`
- `shstrndx`: `i64`

*Defined at line 435*

---

## img_sec_names

The names of `secs`, in order (for img_elf_shstrtab).

```fides
pub fn img_sec_names(secs: &Vec<ImgSec>) -> Vec<str> {
```

**Parameters:**

- `secs`: `&Vec<ImgSec>`

**Returns:** `Vec<str>`

*Defined at line 453*

---

## IMG_PE_NSECT_AT:

File offsets in a PE image whose DOS header is 64 bytes (e_lfanew 0x40):
the COFF header's NumberOfSections, PointerToSymbolTable and
NumberOfSymbols, the optional header and its data directories.

```fides
pub const IMG_PE_NSECT_AT: i64 = 70;
```

*Defined at line 471*

---

## IMG_PE_SYMPTR_AT:

```fides
pub const IMG_PE_SYMPTR_AT: i64 = 76;
```

*Defined at line 472*

---

## IMG_PE_NSYMS_AT:

```fides
pub const IMG_PE_NSYMS_AT: i64 = 80;
```

*Defined at line 473*

---

## IMG_PE_OPT_AT:

```fides
pub const IMG_PE_OPT_AT: i64 = 88;
```

*Defined at line 474*

---

## IMG_PE_DIRS_AT:

```fides
pub const IMG_PE_DIRS_AT: i64 = 0xC8;
```

*Defined at line 475*

---

## img_pe_dos

The DOS header (64 bytes, e_lfanew = 64): `full` the fields of a real
MS-DOS header (what pe_build_file writes), else "MZ" and zeros.

```fides
pub fn img_pe_dos(out: &mut Vec<i64>, full: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `full`: `bool`

*Defined at line 479*

---

## img_pe_signature

"PE\0\0".

```fides
pub fn img_pe_signature(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 501*

---

## img_coff_header

The 20-byte COFF file header (TimeDateStamp 0: reproducible).

```fides
pub fn img_coff_header(out: &mut Vec<i64>, machine: i64, nsect: i64, symptr: i64, nsyms: i64, opt_size: i64, chars: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `machine`: `i64`
- `nsect`: `i64`
- `symptr`: `i64`
- `nsyms`: `i64`
- `opt_size`: `i64`
- `chars`: `i64`

*Defined at line 509*

---

## ImgPeOpt

The fields of a PE32+ optional header that differ between images
(versions: major only, minors 0; image version 0).

```fides
pub struct ImgPeOpt {
```

**Fields:**

| Name | Type |
|------|------|
| `linker_major` | `i64` |
| `code_size` | `i64` |
| `init_size` | `i64` |
| `entry` | `i64` |
| `base_of_code` | `i64` |
| `image_base` | `i64` |
| `sect_align` | `i64` |
| `file_align` | `i64` |
| `os_major` | `i64` |
| `subsys_major` | `i64` |
| `image_size` | `i64` |
| `header_size` | `i64` |
| `subsystem` | `i64` |
| `dll_chars` | `i64` |
| `stack_reserve` | `i64` |
| `stack_commit` | `i64` |
| `heap_reserve` | `i64` |
| `heap_commit` | `i64` |

*Defined at line 521*

---

## img_pe_optional

The 240-byte PE32+ optional header; `dirs` the 16 data directories as
[rva, size] pairs (missing ones 0).

```fides
pub fn img_pe_optional(out: &mut Vec<i64>, o: &ImgPeOpt, dirs: &Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `o`: `&ImgPeOpt`
- `dirs`: `&Vec<i64>`

*Defined at line 544*

---

## img_pe_dirs

16 empty data directories, for img_pe_dir.

```fides
pub fn img_pe_dirs() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 589*

---

## img_pe_dir

```fides
pub fn img_pe_dir(dirs: &mut Vec<i64>, idx: i64, rva: i64, size: i64) {
```

**Parameters:**

- `dirs`: `&mut Vec<i64>`
- `idx`: `i64`
- `rva`: `i64`
- `size`: `i64`

*Defined at line 597*

---

## img_pe_section

One 40-byte section header (a name of at most 8 bytes; longer names are
"/<offset>" into the COFF string table, which the caller makes).

```fides
pub fn img_pe_section(out: &mut Vec<i64>, name: str, vsize: i64, rva: i64, raw_size: i64, raw_off: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `str`
- `vsize`: `i64`
- `rva`: `i64`
- `raw_size`: `i64`
- `raw_off`: `i64`
- `flags`: `i64`

*Defined at line 604*

---

## img_coff_section

The same header with a relocation table (an object's section):
PointerToRelocations `reloc_off`, NumberOfRelocations `nreloc`; no line
numbers.

```fides
pub fn img_coff_section(out: &mut Vec<i64>, name: str, vsize: i64, rva: i64, raw_size: i64, raw_off: i64, reloc_off: i64, nreloc: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `str`
- `vsize`: `i64`
- `rva`: `i64`
- `raw_size`: `i64`
- `raw_off`: `i64`
- `reloc_off`: `i64`
- `nreloc`: `i64`
- `flags`: `i64`

*Defined at line 611*

---

## img_coff_reloc

One COFF relocation (10 bytes): VirtualAddress, SymbolTableIndex, Type.

```fides
pub fn img_coff_reloc(out: &mut Vec<i64>, va: i64, sym: i64, rtype: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `va`: `i64`
- `sym`: `i64`
- `rtype`: `i64`

*Defined at line 625*

---

## IMG_COFF_SYMPTR_AT:

Where a COFF object's header (no DOS header, no PE signature) keeps
PointerToSymbolTable and NumberOfSymbols, for img_coff_symbols.

```fides
pub const IMG_COFF_SYMPTR_AT: i64 = 8;
```

*Defined at line 633*

---

## IMG_COFF_NSYMS_AT:

```fides
pub const IMG_COFF_NSYMS_AT: i64 = 12;
```

*Defined at line 634*

---

## img_pe_section_at

The same header written over the 40 bytes at `at`.

```fides
pub fn img_pe_section_at(out: &mut Vec<i64>, at: i64, name: str, vsize: i64, rva: i64, raw_size: i64, raw_off: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `at`: `i64`
- `name`: `str`
- `vsize`: `i64`
- `rva`: `i64`
- `raw_size`: `i64`
- `raw_off`: `i64`
- `flags`: `i64`

*Defined at line 637*

---

## img_coff_symbols

A COFF symbol table and string table after `image`, one 18-byte record
per symbol (the name inline when at most 8 bytes, else 0 and its string
table offset; Value; SectionNumber; Type 0x20 for a function;
StorageClass EXTERNAL 2 for a global, STATIC 3 for a local; no aux
records), the string table starting with the names `pre` (long section
names). The COFF header's PointerToSymbolTable and NumberOfSymbols, at
`ptr_at` and `nsyms_at`, point at it.

```fides
pub fn img_coff_symbols(image: &mut Vec<i64>, pre: &Vec<str>, syms: &Vec<ImgSym>, ptr_at: i64, nsyms_at: i64) {
```

**Parameters:**

- `image`: `&mut Vec<i64>`
- `pre`: `&Vec<str>`
- `syms`: `&Vec<ImgSym>`
- `ptr_at`: `i64`
- `nsyms_at`: `i64`

*Defined at line 656*

---

## img_pe_base_relocs

A .reloc section: one block per 4 KB page of DIR64 entries (type 10)
for each RVA of `rvas` (any order), each block padded to 4 bytes with
an ABSOLUTE entry. Without any, one block (page `empty_page`) of
padding only: the image still says it can be relocated.

```fides
pub fn img_pe_base_relocs(rvas: &Vec<i64>, empty_page: i64) -> Vec<i64> {
```

**Parameters:**

- `rvas`: `&Vec<i64>`
- `empty_page`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 699*

---

## img_macho_header

mach_header_64 (32 bytes).

```fides
pub fn img_macho_header(out: &mut Vec<i64>, cputype: i64, cpusub: i64, filetype: i64, ncmds: i64, sizeofcmds: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `cputype`: `i64`
- `cpusub`: `i64`
- `filetype`: `i64`
- `ncmds`: `i64`
- `sizeofcmds`: `i64`
- `flags`: `i64`

*Defined at line 748*

---

## img_macho_segment

One LC_SEGMENT_64 (nsects section headers follow).

```fides
pub fn img_macho_segment(out: &mut Vec<i64>, name: str, vmaddr: i64, vmsize: i64, fileoff: i64, filesize: i64, maxprot: i64, initprot: i64, nsects: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `str`
- `vmaddr`: `i64`
- `vmsize`: `i64`
- `fileoff`: `i64`
- `filesize`: `i64`
- `maxprot`: `i64`
- `initprot`: `i64`
- `nsects`: `i64`

*Defined at line 760*

---

## img_macho_section

One section_64 (80 bytes), without relocations.

```fides
pub fn img_macho_section(out: &mut Vec<i64>, sect: str, seg: str, addr: i64, size: i64, offset: i64, align: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `sect`: `str`
- `seg`: `str`
- `addr`: `i64`
- `size`: `i64`
- `offset`: `i64`
- `align`: `i64`
- `flags`: `i64`

*Defined at line 775*

---

## img_macho_section_rel

The same with a relocation table (an object's section): `reloff`,
`nreloc` entries of img_macho_reloc.

```fides
pub fn img_macho_section_rel(out: &mut Vec<i64>, sect: str, seg: str, addr: i64, size: i64, offset: i64, align: i64, reloff: i64, nreloc: i64, flags: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `sect`: `str`
- `seg`: `str`
- `addr`: `i64`
- `size`: `i64`
- `offset`: `i64`
- `align`: `i64`
- `reloff`: `i64`
- `nreloc`: `i64`
- `flags`: `i64`

*Defined at line 781*

---

## img_macho_reloc

One relocation_info (8 bytes): the address, then r_symbolnum (24 bits),
r_pcrel, r_length (2 bits: 0..3 for 1..8 bytes), r_extern, r_type.

```fides
pub fn img_macho_reloc(out: &mut Vec<i64>, addr: i64, symnum: i64, pcrel: bool, length: i64, ext: bool, rtype: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `addr`: `i64`
- `symnum`: `i64`
- `pcrel`: `bool`
- `length`: `i64`
- `ext`: `bool`
- `rtype`: `i64`

*Defined at line 798*

---

## img_macho_nlist

One nlist_64 (16 bytes): n_strx, n_type, n_sect, n_desc, n_value.

```fides
pub fn img_macho_nlist(out: &mut Vec<i64>, strx: i64, ntype: i64, nsect: i64, desc: i64, value: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `strx`: `i64`
- `ntype`: `i64`
- `nsect`: `i64`
- `desc`: `i64`
- `value`: `i64`

*Defined at line 813*

---

## img_macho_symtab

The nlist_64 records of `syms` (C's leading underscore on each name, in
`strtab`, which starts with the empty name): N_SECT, N_EXT for a global.

```fides
pub fn img_macho_symtab(syms: &Vec<ImgSym>, strtab: &mut Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `syms`: `&Vec<ImgSym>`
- `strtab`: `&mut Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 823*

---

## img_macho_symtab_cmd

LC_SYMTAB.

```fides
pub fn img_macho_symtab_cmd(out: &mut Vec<i64>, symoff: i64, nsyms: i64, stroff: i64, strsize: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `symoff`: `i64`
- `nsyms`: `i64`
- `stroff`: `i64`
- `strsize`: `i64`

*Defined at line 842*

---

## img_macho_dysymtab_cmd

LC_DYSYMTAB with the local, external and undefined ranges (no tables).

```fides
pub fn img_macho_dysymtab_cmd(out: &mut Vec<i64>, ilocal: i64, nlocal: i64, iext: i64, next: i64, iundef: i64, nundef: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `ilocal`: `i64`
- `nlocal`: `i64`
- `iext`: `i64`
- `next`: `i64`
- `iundef`: `i64`
- `nundef`: `i64`

*Defined at line 852*

---

## img_macho_dyld_info_cmd

LC_DYLD_INFO_ONLY with rebase and bind opcodes (no weak, lazy or export).

```fides
pub fn img_macho_dyld_info_cmd(out: &mut Vec<i64>, rebase_off: i64, rebase_size: i64, bind_off: i64, bind_size: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `rebase_off`: `i64`
- `rebase_size`: `i64`
- `bind_off`: `i64`
- `bind_size`: `i64`

*Defined at line 869*

---

## img_macho_rebase

The relocations of a Mach-O executable, as dyld's rebase opcodes: one
pointer rebase per offset of `offs` (file positions in the image; `seg`
the segment index, `seg_start` where it starts in the image). Empty
without any.

```fides
pub fn img_macho_rebase(offs: &Vec<i64>, seg: i64, seg_start: i64) -> Vec<i64> {
```

**Parameters:**

- `offs`: `&Vec<i64>`
- `seg`: `i64`
- `seg_start`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 887*

---

## img_macho_bind

The imports of a Mach-O executable, as dyld's bind opcodes: `names[i]`
of dylib `ordinal` into the pointer at `offs[i]` (as img_macho_rebase).

```fides
pub fn img_macho_bind(names: &Vec<str>, offs: &Vec<i64>, seg: i64, seg_start: i64, ordinal: i64) -> Vec<i64> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `offs`: `&Vec<i64>`
- `seg`: `i64`
- `seg_start`: `i64`
- `ordinal`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 910*

---

## img_macho_bind_ords

TG15: img_macho_bind with a dylib ordinal per import (`ords[i]`, 1 for
the first LC_LOAD_DYLIB): SET_DYLIB_ORDINAL_IMM (ordinals up to 15) or
SET_DYLIB_ORDINAL_ULEB, written only when the ordinal changes.

```fides
pub fn img_macho_bind_ords(names: &Vec<str>, ords: &Vec<i64>, offs: &Vec<i64>, seg: i64, seg_start: i64) -> Vec<i64> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `ords`: `&Vec<i64>`
- `offs`: `&Vec<i64>`
- `seg`: `i64`
- `seg_start`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 938*

---

## img_fat_arch

One fat_arch (big-endian, 20 bytes) of a universal binary.

```fides
pub fn img_fat_arch(out: &mut Vec<i64>, cputype: i64, cpusub: i64, offset: i64, size: i64, align: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `cputype`: `i64`
- `cpusub`: `i64`
- `offset`: `i64`
- `size`: `i64`
- `align`: `i64`

*Defined at line 975*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
