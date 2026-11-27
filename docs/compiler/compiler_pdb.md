[All modules](index.md)

# compiler::pdb -- Fides Documentation

## Contents

- [PDB_BLOCK:](#PDB_BLOCK:) (const)
- [pdb_get_u16](#pdb_get_u16) (function)
- [pdb_get_u32](#pdb_get_u32) (function)
- [pdb_hash_v1](#pdb_hash_v1) (function)
- [pdb_guid](#pdb_guid) (function)
- [pdb_guid_text](#pdb_guid_text) (function)
- [pdb_path_for](#pdb_path_for) (function)
- [pdb_setup](#pdb_setup) (function)
- [pdb_out_path](#pdb_out_path) (function)
- [pdb_attach](#pdb_attach) (function)
- [pdb_add_debug_directory](#pdb_add_debug_directory) (function)
- [pdb_coff_functions](#pdb_coff_functions) (function)
- [pdb_build](#pdb_build) (function)
- [PdbVarItem](#PdbVarItem) (struct)
- [PdbLocals](#PdbLocals) (struct)
- [PdbDies](#PdbDies) (struct)
- [pdb_dwarf_locals](#pdb_dwarf_locals) (function)
- [pdb_msf](#pdb_msf) (function)

## PDB_BLOCK:

```fides
pub const PDB_BLOCK: i64 = 4096;
```

*Defined at line 61*

---

## pdb_get_u16

```fides
pub fn pdb_get_u16(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 104*

---

## pdb_get_u32

```fides
pub fn pdb_get_u32(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 108*

---

## pdb_hash_v1

hashStringV1 of the PDB reference implementation (LHashPbCb): the names
map of the info stream, the string tables and the GSI buckets use it.

```fides
pub fn pdb_hash_v1(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 160*

---

## pdb_guid

The PDB's GUID (16 bytes, a version-4 layout): four FNV-1a lanes over
the image, so the same image always names the same PDB.

```fides
pub fn pdb_guid(pe: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pe`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 182*

---

## pdb_guid_text

The GUID as Windows prints it ({XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
without braces, upper case): what symbol servers and dumpbin show.

```fides
pub fn pdb_guid_text(g: &Vec<i64>) -> str {
```

**Parameters:**

- `g`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 210*

---

## pdb_path_for

The PDB beside `exe`: its name with ".exe" replaced by ".pdb" (or
".pdb" added).

```fides
pub fn pdb_path_for(exe: str) -> str {
```

**Parameters:**

- `exe`: `str`

**Returns:** `str`

*Defined at line 243*

---

## pdb_setup

A debug build's driver (main.fd) calls this with the output path before
the backend runs: pdb_attach then writes the PDB beside it. FID_PDB=0
writes none.

```fides
pub fn pdb_setup(output: str) {
```

**Parameters:**

- `output`: `str`

*Defined at line 254*

---

## pdb_out_path

Where pdb_attach writes: g_pdb_out, else $FID_PDB_OUT, else "".

```fides
pub fn pdb_out_path() -> str {
```

**Returns:** `str`

*Defined at line 269*

---

## pdb_attach

The backends' call (pe_ir.fd after the COFF symbols, nat_enc.fd
nat_pe_debug): `pe` is the finished image; the functions `names` span
[starts, ends) of .text; `rows` are their line rows (see the top of the
file) against `files` (empty: just `file`). Writes the PDB to
pdb_out_path() and returns the image with its debug directory; with no
path, no functions, or an image pdb_add_debug_directory cannot extend,
returns `pe` as it is (and writes nothing).

```fides
pub fn pdb_attach(pe: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, rows: Vec<i64>, files: Vec<str>, file: str) -> Vec<i64> {
```

**Parameters:**

- `pe`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `rows`: `Vec<i64>`
- `files`: `Vec<str>`
- `file`: `str`

**Returns:** `Vec<i64>`

*Defined at line 290*

---

## pdb_add_debug_directory

`pe` (a PE32+ image) with a `.buildid` section after its last one that
holds an IMAGE_DEBUG_DIRECTORY entry (type 2, CODEVIEW) and the RSDS
record it points at: "RSDS", `guid`, `age`, `path` NUL-ended. Data
directory 6 (Debug) points at the entry. The section gets the next
aligned address and file space (INITIALIZED_DATA | MEM_READ);
NumberOfSections, SizeOfImage and SizeOfInitializedData cover it. A COFF
symbol table after the sections moves after the new one (its pointer
follows). Empty when `pe` is not a PE32+ image, has fewer than 7 data
directories, or has no room for another section header.

```fides
pub fn pdb_add_debug_directory(pe: &Vec<i64>, guid: &Vec<i64>, age: i64, path: str) -> Vec<i64> {
```

**Parameters:**

- `pe`: `&Vec<i64>`
- `guid`: `&Vec<i64>`
- `age`: `i64`
- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 339*

---

## pdb_coff_functions

The function symbols of `pe`'s COFF symbol table (section 1, .text):
their names in `names`, their offsets in .text in `offs`.

```fides
pub fn pdb_coff_functions(pe: &Vec<i64>, names: &mut Vec<str>, offs: &mut Vec<i64>) {
```

**Parameters:**

- `pe`: `&Vec<i64>`
- `names`: `&mut Vec<str>`
- `offs`: `&mut Vec<i64>`

*Defined at line 453*

---

## pdb_build

The PDB of the image `pe` (with its debug directory: pdb_add_debug_
directory): the functions `names` at [starts, ends) of .text, their
line rows `rows` against `files` (empty: `file`), `guid` and `age` as
in the image's RSDS record. See the top of the file for the streams.

```fides
pub fn pdb_build(pe: &Vec<i64>, names: &Vec<str>, starts: &Vec<i64>, ends: &Vec<i64>, rows: &Vec<i64>, files: &Vec<str>, file: str, guid: &Vec<i64>, age: i64) -> Vec<i64> {
```

**Parameters:**

- `pe`: `&Vec<i64>`
- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `&Vec<i64>`
- `rows`: `&Vec<i64>`
- `files`: `&Vec<str>`
- `file`: `str`
- `guid`: `&Vec<i64>`
- `age`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 805*

---

## PdbVarItem

One item of a function's variables: kind 0 S_REGREL32 (reg, off), 1
S_REGISTER (reg), 2 S_LOCAL with `ranges` (5 words each: start offset in
.text, length, 0 register / 1 register + offset, register, offset), 10 a
lexical block [lo, lo + len) begins, 11 it ends.

```fides
pub struct PdbVarItem {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `name` | `str` |
| `ty` | `i64` |
| `reg` | `i64` |
| `off` | `i64` |
| `lo` | `i64` |
| `len` | `i64` |
| `is_param` | `bool` |
| `ranges` | `Vec<i64>` |

*Defined at line 1365*

---

## PdbLocals

The variables of the image: per function (by its offset in .text) its
items, and the TPI type records they use (`ntypes` of them, from 0x1000).

```fides
pub struct PdbLocals {
```

**Fields:**

| Name | Type |
|------|------|
| `fn_starts` | `Vec<i64>` |
| `fn_items` | `Vec<Vec<PdbVarItem>>` |
| `types` | `Vec<i64>` |
| `ntypes` | `i64` |
| `hashes` | `Vec<i64>` |

*Defined at line 1379*

---

## PdbDies

The entries of .debug_info: parallel lists (one element per entry).

```fides
pub struct PdbDies {
```

**Fields:**

| Name | Type |
|------|------|
| `off` | `Vec<i64>` |
| `tag` | `Vec<i64>` |
| `parent` | `Vec<i64>` |
| `name` | `Vec<str>` |
| `ty` | `Vec<i64>` |
| `lo` | `Vec<i64>` |
| `hi` | `Vec<i64>` |
| `enc` | `Vec<i64>` |
| `size` | `Vec<i64>` |
| `memloc` | `Vec<i64>` |
| `loc_kind` | `Vec<i64>` |
| `loc_at` | `Vec<i64>` |
| `loc_len` | `Vec<i64>` |
| `cu_base` | `Vec<i64>` |

*Defined at line 1539*

---

## pdb_dwarf_locals

The variables of the image `pe` (empty when it has no .debug_info).

```fides
pub fn pdb_dwarf_locals(pe: &Vec<i64>) -> PdbLocals {
```

**Parameters:**

- `pe`: `&Vec<i64>`

**Returns:** `PdbLocals`

*Defined at line 2139*

---

## pdb_msf

The MSF 7.00 container ("big MSF", blocks of 4096 bytes) of `streams`:
block 0 the superblock, 1 and 2 (and 1, 2 of every 4096) the free block
maps, then each stream's blocks, the stream directory (number of
streams, their sizes, their blocks) and the block that lists the
directory's blocks. Every block is used: the free block map has a 0 bit
for each block of the file, 1 after.

```fides
pub fn pdb_msf(streams: &Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `streams`: `&Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 2471*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
