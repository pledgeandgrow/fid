[All modules](index.md)

# compiler::elf64 -- Fides Documentation

## Contents

- [str_lines](#str_lines) (function)
- [ELFMAG0:](#ELFMAG0:) (const)
- [ELFMAG1:](#ELFMAG1:) (const)
- [ELFMAG2:](#ELFMAG2:) (const)
- [ELFMAG3:](#ELFMAG3:) (const)
- [ELFCLASS64:](#ELFCLASS64:) (const)
- [ELFDATA2LSB:](#ELFDATA2LSB:) (const)
- [EV_CURRENT:](#EV_CURRENT:) (const)
- [ELFOSABI_NONE:](#ELFOSABI_NONE:) (const)
- [ET_REL:](#ET_REL:) (const)
- [ET_EXEC:](#ET_EXEC:) (const)
- [EM_X86_64:](#EM_X86_64:) (const)
- [EM_AARCH64:](#EM_AARCH64:) (const)
- [EM_RISCV:](#EM_RISCV:) (const)
- [SHT_NULL:](#SHT_NULL:) (const)
- [SHT_PROGBITS:](#SHT_PROGBITS:) (const)
- [SHT_SYMTAB:](#SHT_SYMTAB:) (const)
- [SHT_STRTAB:](#SHT_STRTAB:) (const)
- [SHT_RELA:](#SHT_RELA:) (const)
- [SHT_NOBITS:](#SHT_NOBITS:) (const)
- [SHF_WRITE:](#SHF_WRITE:) (const)
- [SHF_ALLOC:](#SHF_ALLOC:) (const)
- [SHF_EXECINSTR:](#SHF_EXECINSTR:) (const)
- [SHF_INFO_LINK:](#SHF_INFO_LINK:) (const)
- [STB_LOCAL:](#STB_LOCAL:) (const)
- [STB_GLOBAL:](#STB_GLOBAL:) (const)
- [STT_NOTYPE:](#STT_NOTYPE:) (const)
- [STT_FUNC:](#STT_FUNC:) (const)
- [STT_SECTION:](#STT_SECTION:) (const)
- [STT_FILE:](#STT_FILE:) (const)
- [SHN_UNDEF:](#SHN_UNDEF:) (const)
- [SHN_ABS:](#SHN_ABS:) (const)
- [R_X86_64_64:](#R_X86_64_64:) (const)
- [R_X86_64_PC32:](#R_X86_64_PC32:) (const)
- [R_X86_64_PLT32:](#R_X86_64_PLT32:) (const)
- [Symbol](#Symbol) (struct)
- [Symbol_new](#Symbol_new) (function)
- [Relocation](#Relocation) (struct)
- [Relocation_new](#Relocation_new) (function)
- [RodataSym](#RodataSym) (struct)
- [RodataSym_new](#RodataSym_new) (function)
- [SymIndex](#SymIndex) (struct)
- [ElfEmitter](#ElfEmitter) (struct)
- [CustomSection](#CustomSection) (struct)
- [CustomSection_new](#CustomSection_new) (function)
- [ElfEmitter_new](#ElfEmitter_new) (function)
- [ElfEmitter_add_shstrtab](#ElfEmitter_add_shstrtab) (function)
- [ElfEmitter_add_strtab](#ElfEmitter_add_strtab) (function)
- [ElfEmitter_add_symbol](#ElfEmitter_add_symbol) (function)
- [ElfEmitter_add_custom_section](#ElfEmitter_add_custom_section) (function)
- [ElfEmitter_get_or_add_undef_symbol](#ElfEmitter_get_or_add_undef_symbol) (function)
- [pb2](#pb2) (function)
- [pb3](#pb3) (function)
- [pb4](#pb4) (function)
- [pb5](#pb5) (function)
- [pb6](#pb6) (function)
- [pb7](#pb7) (function)
- [pb8](#pb8) (function)
- [ResultVec](#ResultVec) (enum)
- [ResultVec_is_ok](#ResultVec_is_ok) (function)
- [ResultVec_is_err](#ResultVec_is_err) (function)
- [ResultVec_err_msg](#ResultVec_err_msg) (function)
- [ResultVec_bytes](#ResultVec_bytes) (function)
- [emit_object_file](#emit_object_file) (function)
- [emit_object_file_for_target](#emit_object_file_for_target) (function)
- [ResultUnit](#ResultUnit) (enum)
- [ElfEmitter_assemble](#ElfEmitter_assemble) (function)
- [ElfEmitter_handle_directive](#ElfEmitter_handle_directive) (function)
- [ElfEmitter_get_or_add_local_label](#ElfEmitter_get_or_add_local_label) (function)
- [OptI64](#OptI64) (enum)
- [MemInfo](#MemInfo) (struct)
- [MemInfoSib](#MemInfoSib) (struct)
- [ElfEmitter_encode_instruction](#ElfEmitter_encode_instruction) (function)
- [ElfEmitter_build_elf](#ElfEmitter_build_elf) (function)
- [ElfEmitter_build_elf_img](#ElfEmitter_build_elf_img) (function)
- [AsmBlockResult](#AsmBlockResult) (struct)
- [encode_asm_block](#encode_asm_block) (function)
- [dump_symbols](#dump_symbols) (function)
- [char_from_byte](#char_from_byte) (function)

## str_lines

```fides
pub fn str_lines(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 17*

---

## ELFMAG0:

```fides
pub const ELFMAG0: i64 = 0x7f;
```

*Defined at line 76*

---

## ELFMAG1:

```fides
pub const ELFMAG1: i64 = 0x45;  // 'E'
```

*Defined at line 77*

---

## ELFMAG2:

```fides
pub const ELFMAG2: i64 = 0x4c;  // 'L'
```

*Defined at line 78*

---

## ELFMAG3:

```fides
pub const ELFMAG3: i64 = 0x46;  // 'F'
```

*Defined at line 79*

---

## ELFCLASS64:

```fides
pub const ELFCLASS64: i64 = 2;
```

*Defined at line 80*

---

## ELFDATA2LSB:

```fides
pub const ELFDATA2LSB: i64 = 1;
```

*Defined at line 81*

---

## EV_CURRENT:

```fides
pub const EV_CURRENT: i64 = 1;
```

*Defined at line 82*

---

## ELFOSABI_NONE:

```fides
pub const ELFOSABI_NONE: i64 = 0;
```

*Defined at line 83*

---

## ET_REL:

```fides
pub const ET_REL: i64 = 1;
```

*Defined at line 84*

---

## ET_EXEC:

```fides
pub const ET_EXEC: i64 = 2;
```

*Defined at line 85*

---

## EM_X86_64:

```fides
pub const EM_X86_64: i64 = 62;
```

*Defined at line 86*

---

## EM_AARCH64:

```fides
pub const EM_AARCH64: i64 = 183;
```

*Defined at line 87*

---

## EM_RISCV:

```fides
pub const EM_RISCV: i64 = 243;
```

*Defined at line 88*

---

## SHT_NULL:

```fides
pub const SHT_NULL: i64 = 0;
```

*Defined at line 89*

---

## SHT_PROGBITS:

```fides
pub const SHT_PROGBITS: i64 = 1;
```

*Defined at line 90*

---

## SHT_SYMTAB:

```fides
pub const SHT_SYMTAB: i64 = 2;
```

*Defined at line 91*

---

## SHT_STRTAB:

```fides
pub const SHT_STRTAB: i64 = 3;
```

*Defined at line 92*

---

## SHT_RELA:

```fides
pub const SHT_RELA: i64 = 4;
```

*Defined at line 93*

---

## SHT_NOBITS:

```fides
pub const SHT_NOBITS: i64 = 8;
```

*Defined at line 94*

---

## SHF_WRITE:

```fides
pub const SHF_WRITE: i64 = 0x1;
```

*Defined at line 95*

---

## SHF_ALLOC:

```fides
pub const SHF_ALLOC: i64 = 0x2;
```

*Defined at line 96*

---

## SHF_EXECINSTR:

```fides
pub const SHF_EXECINSTR: i64 = 0x4;
```

*Defined at line 97*

---

## SHF_INFO_LINK:

```fides
pub const SHF_INFO_LINK: i64 = 0x40;
```

*Defined at line 98*

---

## STB_LOCAL:

```fides
pub const STB_LOCAL: i64 = 0;
```

*Defined at line 99*

---

## STB_GLOBAL:

```fides
pub const STB_GLOBAL: i64 = 1;
```

*Defined at line 100*

---

## STT_NOTYPE:

```fides
pub const STT_NOTYPE: i64 = 0;
```

*Defined at line 101*

---

## STT_FUNC:

```fides
pub const STT_FUNC: i64 = 2;
```

*Defined at line 102*

---

## STT_SECTION:

```fides
pub const STT_SECTION: i64 = 3;
```

*Defined at line 103*

---

## STT_FILE:

```fides
pub const STT_FILE: i64 = 4;
```

*Defined at line 104*

---

## SHN_UNDEF:

```fides
pub const SHN_UNDEF: i64 = 0;
```

*Defined at line 105*

---

## SHN_ABS:

```fides
pub const SHN_ABS: i64 = 0xfff1;
```

*Defined at line 106*

---

## R_X86_64_64:

```fides
pub const R_X86_64_64: i64 = 1;
```

*Defined at line 107*

---

## R_X86_64_PC32:

```fides
pub const R_X86_64_PC32: i64 = 2;
```

*Defined at line 108*

---

## R_X86_64_PLT32:

```fides
pub const R_X86_64_PLT32: i64 = 4;
```

*Defined at line 109*

---

## Symbol

```fides
pub struct Symbol {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `i64` |
| `size` | `i64` |
| `binding` | `i64` |
| `sym_type` | `i64` |
| `section` | `i64` |
| `is_defined` | `bool` |

*Defined at line 112*

---

## Symbol_new

```fides
pub fn Symbol_new(name: str, value: i64, size: i64, binding: i64, sym_type: i64, section: i64, is_defined: bool) -> Symbol {
```

**Parameters:**

- `name`: `str`
- `value`: `i64`
- `size`: `i64`
- `binding`: `i64`
- `sym_type`: `i64`
- `section`: `i64`
- `is_defined`: `bool`

**Returns:** `Symbol`

*Defined at line 122*

---

## Relocation

```fides
pub struct Relocation {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `symbol` | `i64` |
| `rtype` | `i64` |
| `addend` | `i64` |

*Defined at line 128*

---

## Relocation_new

```fides
pub fn Relocation_new(offset: i64, symbol: i64, rtype: i64, addend: i64) -> Relocation {
```

**Parameters:**

- `offset`: `i64`
- `symbol`: `i64`
- `rtype`: `i64`
- `addend`: `i64`

**Returns:** `Relocation`

*Defined at line 135*

---

## RodataSym

```fides
pub struct RodataSym {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |

*Defined at line 141*

---

## RodataSym_new

```fides
pub fn RodataSym_new(name: str, offset: i64) -> RodataSym {
```

**Parameters:**

- `name`: `str`
- `offset`: `i64`

**Returns:** `RodataSym`

*Defined at line 146*

---

## SymIndex

```fides
pub struct SymIndex {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `idx` | `i64` |

*Defined at line 152*

---

## ElfEmitter

```fides
pub struct ElfEmitter {
```

**Fields:**

| Name | Type |
|------|------|
| `text` | `Vec<i64>` |
| `machine` | `i64` |
| `comment` | `Vec<i64>` |
| `symbols` | `Vec<Symbol>` |
| `symbol_index` | `Vec<SymIndex>` |
| `relocations` | `Vec<Relocation>` |
| `strtab` | `Vec` |
| `strtab_offsets` | `Vec<SymIndex>` |
| `shstrtab` | `Vec` |
| `shstrtab_offsets` | `Vec<SymIndex>` |
| `rodata` | `Vec<i64>` |
| `rodata_symbols` | `Vec<RodataSym>` |
| `rodata_relocations` | `Vec<Relocation>` |
| `custom_sections` | `Vec<CustomSection>` |

*Defined at line 157*

---

## CustomSection

```fides
pub struct CustomSection {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `data` | `Vec<i64>` |
| `sh_type` | `i64` |
| `sh_flags` | `i64` |
| `align` | `i64,  // alignment in bytes` |

*Defined at line 176*

---

## CustomSection_new

```fides
pub fn CustomSection_new(name: str, data: Vec<i64>, sh_type: i64, sh_flags: i64, align: i64) -> CustomSection {
```

**Parameters:**

- `name`: `str`
- `data`: `Vec<i64>`
- `sh_type`: `i64`
- `sh_flags`: `i64`
- `align`: `i64`

**Returns:** `CustomSection`

*Defined at line 186*

---

## ElfEmitter_new

```fides
pub fn ElfEmitter_new() -> ElfEmitter {
```

**Returns:** `ElfEmitter`

*Defined at line 192*

---

## ElfEmitter_add_shstrtab

```fides
pub fn ElfEmitter_add_shstrtab(self: &mut ElfEmitter, s: str) -> i64 {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `s`: `str`

**Returns:** `i64`

*Defined at line 237*

---

## ElfEmitter_add_strtab

```fides
pub fn ElfEmitter_add_strtab(self: &mut ElfEmitter, s: str) -> i64 {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `s`: `str`

**Returns:** `i64`

*Defined at line 253*

---

## ElfEmitter_add_symbol

```fides
pub fn ElfEmitter_add_symbol(self: &mut ElfEmitter, name: str, value: i64, size: i64, binding: i64, sym_type: i64, section: i64, is_defined: bool) {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `name`: `str`
- `value`: `i64`
- `size`: `i64`
- `binding`: `i64`
- `sym_type`: `i64`
- `section`: `i64`
- `is_defined`: `bool`

*Defined at line 269*

---

## ElfEmitter_add_custom_section

```fides
pub fn ElfEmitter_add_custom_section(self: &mut ElfEmitter, name: str, data: Vec<i64>, section_type: i64, section_flags: i64, align: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `name`: `str`
- `data`: `Vec<i64>`
- `section_type`: `i64`
- `section_flags`: `i64`
- `align`: `i64`

**Returns:** `i64`

*Defined at line 289*

---

## ElfEmitter_get_or_add_undef_symbol

```fides
pub fn ElfEmitter_get_or_add_undef_symbol(self: &mut ElfEmitter, name: str) -> i64 {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `name`: `str`

**Returns:** `i64`

*Defined at line 298*

---

## pb2

```fides
pub fn pb2(t: &mut Vec, a: i64, b: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`

*Defined at line 310*

---

## pb3

```fides
pub fn pb3(t: &mut Vec, a: i64, b: i64, c: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`

*Defined at line 314*

---

## pb4

```fides
pub fn pb4(t: &mut Vec, a: i64, b: i64, c: i64, d: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`

*Defined at line 319*

---

## pb5

```fides
pub fn pb5(t: &mut Vec, a: i64, b: i64, c: i64, d: i64, e: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`
- `e`: `i64`

*Defined at line 325*

---

## pb6

```fides
pub fn pb6(t: &mut Vec, a: i64, b: i64, c: i64, d: i64, e: i64, f: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`
- `e`: `i64`
- `f`: `i64`

*Defined at line 332*

---

## pb7

```fides
pub fn pb7(t: &mut Vec, a: i64, b: i64, c: i64, d: i64, e: i64, f: i64, g: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`
- `e`: `i64`
- `f`: `i64`
- `g`: `i64`

*Defined at line 340*

---

## pb8

```fides
pub fn pb8(t: &mut Vec, a: i64, b: i64, c: i64, d: i64, e: i64, f: i64, g: i64, h: i64) {
```

**Parameters:**

- `t`: `&mut Vec`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`
- `e`: `i64`
- `f`: `i64`
- `g`: `i64`
- `h`: `i64`

*Defined at line 349*

---

## ResultVec

```fides
pub enum ResultVec {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 381*

---

## ResultVec_is_ok

```fides
pub fn ResultVec_is_ok(r: ResultVec) -> bool {
```

**Parameters:**

- `r`: `ResultVec`

**Returns:** `bool`

*Defined at line 388*

---

## ResultVec_is_err

```fides
pub fn ResultVec_is_err(r: ResultVec) -> bool {
```

**Parameters:**

- `r`: `ResultVec`

**Returns:** `bool`

*Defined at line 399*

---

## ResultVec_err_msg

```fides
pub fn ResultVec_err_msg(r: ResultVec) -> str {
```

**Parameters:**

- `r`: `ResultVec`

**Returns:** `str`

*Defined at line 410*

---

## ResultVec_bytes

```fides
pub fn ResultVec_bytes(r: ResultVec) -> Vec<i64> {
```

**Parameters:**

- `r`: `ResultVec`

**Returns:** `Vec<i64>`

*Defined at line 421*

---

## emit_object_file

```fides
pub fn emit_object_file(module: i64, asm: str) -> ResultVec {
```

**Parameters:**

- `module`: `i64`
- `asm`: `str`

**Returns:** `ResultVec`

*Defined at line 434*

---

## emit_object_file_for_target

```fides
pub fn emit_object_file_for_target(module: i64, asm: str, target: str) -> ResultVec {
```

**Parameters:**

- `module`: `i64`
- `asm`: `str`
- `target`: `str`

**Returns:** `ResultVec`

*Defined at line 446*

---

## ResultUnit

```fides
pub enum ResultUnit {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 478*

---

## ElfEmitter_assemble

```fides
pub fn ElfEmitter_assemble(self: &mut ElfEmitter, asm: str) -> ResultUnit {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `asm`: `str`

**Returns:** `ResultUnit`

*Defined at line 485*

---

## ElfEmitter_handle_directive

```fides
pub fn ElfEmitter_handle_directive(self: &mut ElfEmitter, line: str) {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `line`: `str`

*Defined at line 549*

---

## ElfEmitter_get_or_add_local_label

```fides
pub fn ElfEmitter_get_or_add_local_label(self: &mut ElfEmitter, label: str) -> i64 {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `label`: `str`

**Returns:** `i64`

*Defined at line 627*

---

## OptI64

```fides
pub enum OptI64 {
```

**Variants:**

- `Some`
- `None`

*Defined at line 643*

---

## MemInfo

Parse a memory operand like "-16(%rbp)" -> (offset, base_reg).
Returns OptI64::Some(packed) or None.
We use a MemInfo struct to return the result.

```fides
pub struct MemInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `base` | `i64` |

*Defined at line 900*

---

## MemInfoSib

```fides
pub struct MemInfoSib {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `base` | `i64` |
| `index` | `i64` |
| `scale` | `i64` |

*Defined at line 905*

---

## ElfEmitter_encode_instruction

```fides
pub fn ElfEmitter_encode_instruction(self: &mut ElfEmitter, line: str) {
```

**Parameters:**

- `self`: `&mut ElfEmitter`
- `line`: `str`

*Defined at line 1076*

---

## ElfEmitter_build_elf

```fides
pub fn ElfEmitter_build_elf(self: &mut ElfEmitter) -> ResultVec {
```

**Parameters:**

- `self`: `&mut ElfEmitter`

**Returns:** `ResultVec`

*Defined at line 3192*

---

## ElfEmitter_build_elf_img

TG13: the ELF64 relocatable object of the assembler path (`fid codegen
--elf`, build_executable) over image_writer. Unlike the old body above
it is well-formed (readelf -a, the linker in compiler/linker.fd and
ld read it), so the bytes differ (docs/reference/image-writer.md):
- 64-byte section headers (sh_addr 0 included; the old ones were 56)
- r_info = symbol << 32 | type, the symbol counted from the null
symbol (the old body swapped the halves and was off by one)
- the local symbols first, .symtab's sh_info the first global
- the .rodata labels (.str_N, .flt_N, .vtable_*) defined in section
2, so that references to them resolve; undefined symbols global
- .symtab, the relocations and the section headers 8-byte aligned,
.rodata 16-byte aligned
- an AArch64/RISC-V emitter's assembly text (`comment`) in .comment
Sections: 0 null, 1 .text, 2 .rodata, 3 .symtab, 4 .strtab,
5 .shstrtab, 6 .rela.text, 7 .rela.rodata, then the custom sections,
then .comment when there is one.

```fides
pub fn ElfEmitter_build_elf_img(self: &mut ElfEmitter) -> ResultVec {
```

**Parameters:**

- `self`: `&mut ElfEmitter`

**Returns:** `ResultVec`

*Defined at line 3236*

---

## AsmBlockResult

```fides
pub struct AsmBlockResult {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec` |
| `symbols` | `Vec<Symbol>` |
| `relocations` | `Vec<Relocation>` |

*Defined at line 3386*

---

## encode_asm_block

```fides
pub fn encode_asm_block(asm: str) -> AsmBlockResult {
```

**Parameters:**

- `asm`: `str`

**Returns:** `AsmBlockResult`

*Defined at line 3392*

---

## dump_symbols

```fides
pub fn dump_symbols(elf: Vec) {
```

**Parameters:**

- `elf`: `Vec`

*Defined at line 3412*

---

## char_from_byte

```fides
pub fn char_from_byte(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 3627*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
