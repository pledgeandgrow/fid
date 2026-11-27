[All modules](index.md)

# compiler::linker -- Fides Documentation

## Contents

- [LK_R_X86_64_64:](#LK_R_X86_64_64:) (const)
- [LK_R_X86_64_PC32:](#LK_R_X86_64_PC32:) (const)
- [LK_R_X86_64_PLT32:](#LK_R_X86_64_PLT32:) (const)
- [R_ARM64_BL:](#R_ARM64_BL:) (const)
- [R_ARM64_B:](#R_ARM64_B:) (const)
- [R_ARM64_BCOND:](#R_ARM64_BCOND:) (const)
- [R_ARM64_ADRP:](#R_ARM64_ADRP:) (const)
- [R_ARM64_ADD_LO12:](#R_ARM64_ADD_LO12:) (const)
- [R_RISCV_JAL:](#R_RISCV_JAL:) (const)
- [R_RISCV_BRANCH:](#R_RISCV_BRANCH:) (const)
- [LinkerSymbol](#LinkerSymbol) (struct)
- [LinkerSymbol_new](#LinkerSymbol_new) (function)
- [LinkerReloc](#LinkerReloc) (struct)
- [LinkerReloc_new](#LinkerReloc_new) (function)
- [LinkerRodataSym](#LinkerRodataSym) (struct)
- [LinkerRodataSym_new](#LinkerRodataSym_new) (function)
- [SectionInfo](#SectionInfo) (struct)
- [Linker](#Linker) (struct)
- [Linker_new](#Linker_new) (function)
- [Linker_new_for_target](#Linker_new_for_target) (function)
- [LinkerResult](#LinkerResult) (enum)
- [LinkerResultUnit](#LinkerResultUnit) (enum)
- [LinkerResult_is_ok](#LinkerResult_is_ok) (function)
- [LinkerResult_err_msg](#LinkerResult_err_msg) (function)
- [LinkerResult_bytes](#LinkerResult_bytes) (function)
- [link_objects](#link_objects) (function)
- [link_with_runtime](#link_with_runtime) (function)
- [link_with_runtime_for_target](#link_with_runtime_for_target) (function)
- [link_raw_with_runtime_for_target](#link_raw_with_runtime_for_target) (function)
- [dead_code_elimination](#dead_code_elimination) (function)
- [Linker_add_object](#Linker_add_object) (function)
- [Linker_link](#Linker_link) (function)
- [Linker_build_executable](#Linker_build_executable) (function)
- [Linker_build_executable_img](#Linker_build_executable_img) (function)

## LK_R_X86_64_64:

```fides
pub const LK_R_X86_64_64: i64 = 1;
```

*Defined at line 19*

---

## LK_R_X86_64_PC32:

```fides
pub const LK_R_X86_64_PC32: i64 = 2;
```

*Defined at line 20*

---

## LK_R_X86_64_PLT32:

```fides
pub const LK_R_X86_64_PLT32: i64 = 4;  // ARM64 internal relocation codes
```

*Defined at line 21*

---

## R_ARM64_BL:

```fides
pub const R_ARM64_BL: i64 = 10;
```

*Defined at line 22*

---

## R_ARM64_B:

```fides
pub const R_ARM64_B: i64 = 11;
```

*Defined at line 23*

---

## R_ARM64_BCOND:

```fides
pub const R_ARM64_BCOND: i64 = 12;
```

*Defined at line 24*

---

## R_ARM64_ADRP:

```fides
pub const R_ARM64_ADRP: i64 = 14;
```

*Defined at line 25*

---

## R_ARM64_ADD_LO12:

```fides
pub const R_ARM64_ADD_LO12: i64 = 15;  // RISC-V internal relocation codes
```

*Defined at line 26*

---

## R_RISCV_JAL:

```fides
pub const R_RISCV_JAL: i64 = 20;
```

*Defined at line 27*

---

## R_RISCV_BRANCH:

```fides
pub const R_RISCV_BRANCH: i64 = 21;
```

*Defined at line 28*

---

## LinkerSymbol

```fides
pub struct LinkerSymbol {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |
| `is_defined` | `bool` |

*Defined at line 31*

---

## LinkerSymbol_new

```fides
pub fn LinkerSymbol_new(name: str, offset: i64, is_defined: bool) -> LinkerSymbol {
```

**Parameters:**

- `name`: `str`
- `offset`: `i64`
- `is_defined`: `bool`

**Returns:** `LinkerSymbol`

*Defined at line 37*

---

## LinkerReloc

```fides
pub struct LinkerReloc {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `symbol` | `str` |
| `rtype` | `i64` |

*Defined at line 43*

---

## LinkerReloc_new

```fides
pub fn LinkerReloc_new(offset: i64, symbol: str, rtype: i64) -> LinkerReloc {
```

**Parameters:**

- `offset`: `i64`
- `symbol`: `str`
- `rtype`: `i64`

**Returns:** `LinkerReloc`

*Defined at line 49*

---

## LinkerRodataSym

```fides
pub struct LinkerRodataSym {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |

*Defined at line 55*

---

## LinkerRodataSym_new

```fides
pub fn LinkerRodataSym_new(name: str, offset: i64) -> LinkerRodataSym {
```

**Parameters:**

- `name`: `str`
- `offset`: `i64`

**Returns:** `LinkerRodataSym`

*Defined at line 60*

---

## SectionInfo

```fides
pub struct SectionInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `name_offset` | `i64` |
| `sh_type` | `i64` |
| `offset` | `i64` |
| `size` | `i64` |
| `link` | `i64` |
| `entsize` | `i64` |
| `name` | `str` |

*Defined at line 66*

---

## Linker

```fides
pub struct Linker {
```

**Fields:**

| Name | Type |
|------|------|
| `text_bytes` | `Vec` |
| `symbols` | `Vec<LinkerSymbol>` |
| `relocations` | `Vec<LinkerReloc>` |
| `rodata` | `Vec` |
| `rodata_symbols` | `Vec<LinkerRodataSym>` |
| `rodata_relocations` | `Vec<LinkerReloc>` |
| `target` | `str` |

*Defined at line 76*

---

## Linker_new

```fides
pub fn Linker_new() -> Linker {
```

**Returns:** `Linker`

*Defined at line 86*

---

## Linker_new_for_target

```fides
pub fn Linker_new_for_target(target: str) -> Linker {
```

**Parameters:**

- `target`: `str`

**Returns:** `Linker`

*Defined at line 92*

---

## LinkerResult

```fides
pub enum LinkerResult {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 100*

---

## LinkerResultUnit

```fides
pub enum LinkerResultUnit {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 104*

---

## LinkerResult_is_ok

```fides
pub fn LinkerResult_is_ok(r: LinkerResult) -> bool {
```

**Parameters:**

- `r`: `LinkerResult`

**Returns:** `bool`

*Defined at line 111*

---

## LinkerResult_err_msg

```fides
pub fn LinkerResult_err_msg(r: LinkerResult) -> str {
```

**Parameters:**

- `r`: `LinkerResult`

**Returns:** `str`

*Defined at line 122*

---

## LinkerResult_bytes

```fides
pub fn LinkerResult_bytes(r: LinkerResult) -> Vec {
```

**Parameters:**

- `r`: `LinkerResult`

**Returns:** `Vec`

*Defined at line 133*

---

## link_objects

```fides
pub fn link_objects(objects: Vec<Vec>, entry: str) -> LinkerResult {
```

**Parameters:**

- `objects`: `Vec<Vec>`
- `entry`: `str`

**Returns:** `LinkerResult`

*Defined at line 300*

---

## link_with_runtime

```fides
pub fn link_with_runtime(objects: Vec<Vec>, entry: str, runtime_text: Vec, runtime_symbols: Vec<LinkerSymbol>, runtime_rodata: Vec, runtime_rodata_syms: Vec<LinkerRodataSym>, runtime_relocs: Vec<LinkerReloc>) -> LinkerResult {
```

**Parameters:**

- `objects`: `Vec<Vec>`
- `entry`: `str`
- `runtime_text`: `Vec`
- `runtime_symbols`: `Vec<LinkerSymbol>`
- `runtime_rodata`: `Vec`
- `runtime_rodata_syms`: `Vec<LinkerRodataSym>`
- `runtime_relocs`: `Vec<LinkerReloc>`

**Returns:** `LinkerResult`

*Defined at line 318*

---

## link_with_runtime_for_target

```fides
pub fn link_with_runtime_for_target(objects: Vec<Vec>, entry: str, runtime_text: Vec, runtime_symbols: Vec<LinkerSymbol>, runtime_rodata: Vec, runtime_rodata_syms: Vec<LinkerRodataSym>, runtime_relocs: Vec<LinkerReloc>, target: str) -> LinkerResult {
```

**Parameters:**

- `objects`: `Vec<Vec>`
- `entry`: `str`
- `runtime_text`: `Vec`
- `runtime_symbols`: `Vec<LinkerSymbol>`
- `runtime_rodata`: `Vec`
- `runtime_rodata_syms`: `Vec<LinkerRodataSym>`
- `runtime_relocs`: `Vec<LinkerReloc>`
- `target`: `str`

**Returns:** `LinkerResult`

*Defined at line 324*

---

## link_raw_with_runtime_for_target

```fides
pub fn link_raw_with_runtime_for_target(user_text: Vec, user_symbols: Vec<LinkerSymbol>, user_relocs: Vec<LinkerReloc>, user_rodata: Vec, user_rodata_syms: Vec<LinkerRodataSym>, entry: str, runtime_text: Vec, runtime_symbols: Vec<LinkerSymbol>, runtime_rodata: Vec, runtime_rodata_syms: Vec<LinkerRodataSym>, runtime_relocs: Vec<LinkerReloc>, target: str) -> LinkerResult {
```

**Parameters:**

- `user_text`: `Vec`
- `user_symbols`: `Vec<LinkerSymbol>`
- `user_relocs`: `Vec<LinkerReloc>`
- `user_rodata`: `Vec`
- `user_rodata_syms`: `Vec<LinkerRodataSym>`
- `entry`: `str`
- `runtime_text`: `Vec`
- `runtime_symbols`: `Vec<LinkerSymbol>`
- `runtime_rodata`: `Vec`
- `runtime_rodata_syms`: `Vec<LinkerRodataSym>`
- `runtime_relocs`: `Vec<LinkerReloc>`
- `target`: `str`

**Returns:** `LinkerResult`

*Defined at line 374*

---

## dead_code_elimination

```fides
pub fn dead_code_elimination(symbols: Vec<LinkerSymbol>, relocations: Vec<LinkerReloc>, entry: str) -> Vec<i64> {
```

**Parameters:**

- `symbols`: `Vec<LinkerSymbol>`
- `relocations`: `Vec<LinkerReloc>`
- `entry`: `str`

**Returns:** `Vec<i64>`

*Defined at line 445*

---

## Linker_add_object

```fides
pub fn Linker_add_object(self: &mut Linker, obj: Vec) -> LinkerResultUnit {
```

**Parameters:**

- `self`: `&mut Linker`
- `obj`: `Vec`

**Returns:** `LinkerResultUnit`

*Defined at line 550*

---

## Linker_link

```fides
pub fn Linker_link(self: &mut Linker, entry: str) -> LinkerResult {
```

**Parameters:**

- `self`: `&mut Linker`
- `entry`: `str`

**Returns:** `LinkerResult`

*Defined at line 796*

---

## Linker_build_executable

```fides
pub fn Linker_build_executable(self: &Linker, entry_offset: i64) -> LinkerResult {
```

**Parameters:**

- `self`: `&Linker`
- `entry_offset`: `i64`

**Returns:** `LinkerResult`

*Defined at line 1038*

---

## Linker_build_executable_img

TG13: Linker_build_executable as a layout over image_writer (the same
bytes): the ELF header (ET_EXEC, no section headers: e_shentsize 0 as
well), one PT_LOAD (read/execute) of .text and .rodata from file
offset 120 at 0x400000 + 120, then .text and .rodata.

```fides
pub fn Linker_build_executable_img(self: &Linker, entry_offset: i64) -> LinkerResult {
```

**Parameters:**

- `self`: `&Linker`
- `entry_offset`: `i64`

**Returns:** `LinkerResult`

*Defined at line 1063*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
