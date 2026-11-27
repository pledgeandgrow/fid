[All modules](index.md)

# tar -- Fides Documentation

## Contents

- [TAR_BLOCK:](#TAR_BLOCK:) (const)
- [TAR_RECORD:](#TAR_RECORD:) (const)
- [TAR_REG:](#TAR_REG:) (const)
- [TAR_AREG:](#TAR_AREG:) (const)
- [TAR_LNK:](#TAR_LNK:) (const)
- [TAR_SYM:](#TAR_SYM:) (const)
- [TAR_CHR:](#TAR_CHR:) (const)
- [TAR_BLK:](#TAR_BLK:) (const)
- [TAR_DIR:](#TAR_DIR:) (const)
- [TAR_FIFO:](#TAR_FIFO:) (const)
- [TAR_CONT:](#TAR_CONT:) (const)
- [TAR_PAX:](#TAR_PAX:) (const)
- [TAR_PAX_GLOBAL:](#TAR_PAX_GLOBAL:) (const)
- [TAR_GNU_LONGNAME:](#TAR_GNU_LONGNAME:) (const)
- [TAR_GNU_LONGLINK:](#TAR_GNU_LONGLINK:) (const)
- [TarEntry](#TarEntry) (struct)
- [TarArchive](#TarArchive) (struct)
- [tar_entry](#tar_entry) (function)
- [tar_file](#tar_file) (function)
- [tar_dir](#tar_dir) (function)
- [tar_symlink](#tar_symlink) (function)
- [tar_is_dir](#tar_is_dir) (function)
- [tar_is_file](#tar_is_file) (function)
- [tar_field_num](#tar_field_num) (function)
- [tar_checksums](#tar_checksums) (function)
- [tar_pax_record](#tar_pax_record) (function)
- [tar_parse](#tar_parse) (function)
- [tar_find](#tar_find) (function)
- [tar_split_point](#tar_split_point) (function)
- [tar_fix_checksum](#tar_fix_checksum) (function)
- [tar_header](#tar_header) (function)
- [tar_write](#tar_write) (function)
- [tar_mode_str](#tar_mode_str) (function)

## TAR_BLOCK:

```fides
pub const TAR_BLOCK: i64 = 512;
```

*Defined at line 22*

---

## TAR_RECORD:

```fides
pub const TAR_RECORD: i64 = 10240;
```

*Defined at line 23*

---

## TAR_REG:

```fides
pub const TAR_REG: i64 = 48;
```

*Defined at line 26*

---

## TAR_AREG:

```fides
pub const TAR_AREG: i64 = 0;
```

*Defined at line 27*

---

## TAR_LNK:

```fides
pub const TAR_LNK: i64 = 49;
```

*Defined at line 28*

---

## TAR_SYM:

```fides
pub const TAR_SYM: i64 = 50;
```

*Defined at line 29*

---

## TAR_CHR:

```fides
pub const TAR_CHR: i64 = 51;
```

*Defined at line 30*

---

## TAR_BLK:

```fides
pub const TAR_BLK: i64 = 52;
```

*Defined at line 31*

---

## TAR_DIR:

```fides
pub const TAR_DIR: i64 = 53;
```

*Defined at line 32*

---

## TAR_FIFO:

```fides
pub const TAR_FIFO: i64 = 54;
```

*Defined at line 33*

---

## TAR_CONT:

```fides
pub const TAR_CONT: i64 = 55;
```

*Defined at line 34*

---

## TAR_PAX:

```fides
pub const TAR_PAX: i64 = 120;
```

*Defined at line 35*

---

## TAR_PAX_GLOBAL:

```fides
pub const TAR_PAX_GLOBAL: i64 = 103;
```

*Defined at line 36*

---

## TAR_GNU_LONGNAME:

```fides
pub const TAR_GNU_LONGNAME: i64 = 76;
```

*Defined at line 37*

---

## TAR_GNU_LONGLINK:

```fides
pub const TAR_GNU_LONGLINK: i64 = 75;
```

*Defined at line 38*

---

## TarEntry

```fides
pub struct TarEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `mode` | `i64` |
| `uid` | `i64` |
| `gid` | `i64` |
| `size` | `i64` |
| `mtime` | `i64` |
| `typeflag` | `i64` |
| `linkname` | `str` |
| `uname` | `str` |
| `gname` | `str` |
| `devmajor` | `i64` |
| `devminor` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 40*

---

## TarArchive

```fides
pub struct TarArchive {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `entries` | `Vec<TarEntry>` |

*Defined at line 56*

---

## tar_entry

```fides
pub fn tar_entry(name: str, typeflag: i64, mode: i64, data: Vec<i64>) -> TarEntry {
```

**Parameters:**

- `name`: `str`
- `typeflag`: `i64`
- `mode`: `i64`
- `data`: `Vec<i64>`

**Returns:** `TarEntry`

*Defined at line 61*

---

## tar_file

A regular file entry.

```fides
pub fn tar_file(name: str, data: Vec<i64>, mode: i64) -> TarEntry {
```

**Parameters:**

- `name`: `str`
- `data`: `Vec<i64>`
- `mode`: `i64`

**Returns:** `TarEntry`

*Defined at line 68*

---

## tar_dir

A directory entry (name conventionally ends in "/").

```fides
pub fn tar_dir(name: str, mode: i64) -> TarEntry {
```

**Parameters:**

- `name`: `str`
- `mode`: `i64`

**Returns:** `TarEntry`

*Defined at line 73*

---

## tar_symlink

A symbolic link entry.

```fides
pub fn tar_symlink(name: str, target: str) -> TarEntry {
```

**Parameters:**

- `name`: `str`
- `target`: `str`

**Returns:** `TarEntry`

*Defined at line 78*

---

## tar_is_dir

```fides
pub fn tar_is_dir(e: &TarEntry) -> bool {
```

**Parameters:**

- `e`: `&TarEntry`

**Returns:** `bool`

*Defined at line 84*

---

## tar_is_file

```fides
pub fn tar_is_file(e: &TarEntry) -> bool {
```

**Parameters:**

- `e`: `&TarEntry`

**Returns:** `bool`

*Defined at line 88*

---

## tar_field_num

A numeric field: octal digits (leading spaces or NULs skipped, ended by
a space or NUL) or, with the top bit of the first byte set, the GNU
base-256 big-endian form.

```fides
pub fn tar_field_num(b: &Vec<i64>, off: i64, len: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 123*

---

## tar_checksums

The checksum of the header at `off`: every byte summed with the
checksum field (148..155) counted as spaces. [unsigned, signed].

```fides
pub fn tar_checksums(b: &Vec<i64>, off: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 152*

---

## tar_pax_record

One pax record "LEN key=value\n", LEN counting the whole record.

```fides
pub fn tar_pax_record(key: str, value: str) -> str {
```

**Parameters:**

- `key`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 228*

---

## tar_parse

Parse a whole archive. `err` is "" on success; entries read before an
error are kept.

```fides
pub fn tar_parse(b: Vec<i64>) -> TarArchive {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `TarArchive`

*Defined at line 326*

---

## tar_find

The index of the entry named `name`, or -1 (the last one wins, as an
extracting tar would leave it).

```fides
pub fn tar_find(a: &TarArchive, name: str) -> i64 {
```

**Parameters:**

- `a`: `&TarArchive`
- `name`: `str`

**Returns:** `i64`

*Defined at line 441*

---

## tar_split_point

Where to split `name` into prefix "/" name (ustar): the index of the
'/', or -1 when no split gives prefix <= 155 and name <= 100 bytes.

```fides
pub fn tar_split_point(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 457*

---

## tar_fix_checksum

Recompute the checksum field of the header block at the start of `h`.

```fides
pub fn tar_fix_checksum(h: &mut Vec<i64>) {
```

**Parameters:**

- `h`: `&mut Vec<i64>`

*Defined at line 476*

---

## tar_header

The header block(s) of one entry: a pax header first when a value does
not fit the ustar fields.

```fides
pub fn tar_header(e: &TarEntry) -> Vec<i64> {
```

**Parameters:**

- `e`: `&TarEntry`

**Returns:** `Vec<i64>`

*Defined at line 530*

---

## tar_write

A whole archive: every entry, two zero blocks, padded to `TAR_RECORD`.

```fides
pub fn tar_write(entries: Vec<TarEntry>) -> Vec<i64> {
```

**Parameters:**

- `entries`: `Vec<TarEntry>`

**Returns:** `Vec<i64>`

*Defined at line 581*

---

## tar_mode_str

"-rw-r--r--"-style mode string of an entry (as `tar tv` prints it).

```fides
pub fn tar_mode_str(e: &TarEntry) -> str {
```

**Parameters:**

- `e`: `&TarEntry`

**Returns:** `str`

*Defined at line 609*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
