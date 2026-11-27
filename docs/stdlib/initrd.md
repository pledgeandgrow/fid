[All modules](index.md)

# initrd -- Fides Documentation

## Contents

- [CPIO_S_IFMT:](#CPIO_S_IFMT:) (const)
- [CPIO_S_IFSOCK:](#CPIO_S_IFSOCK:) (const)
- [CPIO_S_IFLNK:](#CPIO_S_IFLNK:) (const)
- [CPIO_S_IFREG:](#CPIO_S_IFREG:) (const)
- [CPIO_S_IFBLK:](#CPIO_S_IFBLK:) (const)
- [CPIO_S_IFDIR:](#CPIO_S_IFDIR:) (const)
- [CPIO_S_IFCHR:](#CPIO_S_IFCHR:) (const)
- [CPIO_S_IFIFO:](#CPIO_S_IFIFO:) (const)
- [CPIO_HDR:](#CPIO_HDR:) (const)
- [CpioEntry](#CpioEntry) (struct)
- [CpioArchive](#CpioArchive) (struct)
- [cpio_entry](#cpio_entry) (function)
- [cpio_file](#cpio_file) (function)
- [cpio_dir](#cpio_dir) (function)
- [cpio_symlink](#cpio_symlink) (function)
- [cpio_kind](#cpio_kind) (function)
- [cpio_checksum](#cpio_checksum) (function)
- [cpio_parse](#cpio_parse) (function)
- [cpio_write](#cpio_write) (function)
- [cpio_find](#cpio_find) (function)
- [initrd_clean](#initrd_clean) (function)
- [InitrdStats](#InitrdStats) (struct)
- [initrd_unpack](#initrd_unpack) (function)
- [initrd_mount](#initrd_mount) (function)

## CPIO_S_IFMT:

```fides
pub const CPIO_S_IFMT: i64 = 61440;
```

*Defined at line 27*

---

## CPIO_S_IFSOCK:

```fides
pub const CPIO_S_IFSOCK: i64 = 49152;
```

*Defined at line 28*

---

## CPIO_S_IFLNK:

```fides
pub const CPIO_S_IFLNK: i64 = 40960;
```

*Defined at line 29*

---

## CPIO_S_IFREG:

```fides
pub const CPIO_S_IFREG: i64 = 32768;
```

*Defined at line 30*

---

## CPIO_S_IFBLK:

```fides
pub const CPIO_S_IFBLK: i64 = 24576;
```

*Defined at line 31*

---

## CPIO_S_IFDIR:

```fides
pub const CPIO_S_IFDIR: i64 = 16384;
```

*Defined at line 32*

---

## CPIO_S_IFCHR:

```fides
pub const CPIO_S_IFCHR: i64 = 8192;
```

*Defined at line 33*

---

## CPIO_S_IFIFO:

```fides
pub const CPIO_S_IFIFO: i64 = 4096;
```

*Defined at line 34*

---

## CPIO_HDR:

```fides
pub const CPIO_HDR: i64 = 110;
```

*Defined at line 36*

---

## CpioEntry

```fides
pub struct CpioEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ino` | `i64` |
| `mode` | `i64` |
| `uid` | `i64` |
| `gid` | `i64` |
| `nlink` | `i64` |
| `mtime` | `i64` |
| `devmajor` | `i64` |
| `devminor` | `i64` |
| `rdevmajor` | `i64` |
| `rdevminor` | `i64` |
| `check` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 38*

---

## CpioArchive

```fides
pub struct CpioArchive {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `crc` | `bool` |
| `entries` | `Vec<CpioEntry>` |

*Defined at line 54*

---

## cpio_entry

```fides
pub fn cpio_entry(name: str, mode: i64, data: Vec<i64>) -> CpioEntry {
```

**Parameters:**

- `name`: `str`
- `mode`: `i64`
- `data`: `Vec<i64>`

**Returns:** `CpioEntry`

*Defined at line 60*

---

## cpio_file

```fides
pub fn cpio_file(name: str, perm: i64, data: Vec<i64>) -> CpioEntry {
```

**Parameters:**

- `name`: `str`
- `perm`: `i64`
- `data`: `Vec<i64>`

**Returns:** `CpioEntry`

*Defined at line 66*

---

## cpio_dir

```fides
pub fn cpio_dir(name: str, perm: i64) -> CpioEntry {
```

**Parameters:**

- `name`: `str`
- `perm`: `i64`

**Returns:** `CpioEntry`

*Defined at line 70*

---

## cpio_symlink

```fides
pub fn cpio_symlink(name: str, target: str) -> CpioEntry {
```

**Parameters:**

- `name`: `str`
- `target`: `str`

**Returns:** `CpioEntry`

*Defined at line 76*

---

## cpio_kind

```fides
pub fn cpio_kind(e: &CpioEntry) -> i64 {
```

**Parameters:**

- `e`: `&CpioEntry`

**Returns:** `i64`

*Defined at line 80*

---

## cpio_checksum

The newc checksum: the sum of the data bytes, modulo 2^32.

```fides
pub fn cpio_checksum(data: &Vec<i64>) -> i64 {
```

**Parameters:**

- `data`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 85*

---

## cpio_parse

Parse an archive (or several concatenated ones). `err` is "" on
success; the trailers are not returned as entries.

```fides
pub fn cpio_parse(b: Vec<i64>) -> CpioArchive {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `CpioArchive`

*Defined at line 129*

---

## cpio_write

Write an archive (newc, or with `crc` the 070702 form), then the
trailer, padded to a multiple of `block` bytes (512 for GNU cpio's
default, 4 for none).

```fides
pub fn cpio_write(entries: Vec<CpioEntry>, crc: bool, block: i64) -> Vec<i64> {
```

**Parameters:**

- `entries`: `Vec<CpioEntry>`
- `crc`: `bool`
- `block`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 281*

---

## cpio_find

The index of the entry named `name` ("./" prefixes ignored), or -1.

```fides
pub fn cpio_find(a: &CpioArchive, name: str) -> i64 {
```

**Parameters:**

- `a`: `&CpioArchive`
- `name`: `str`

**Returns:** `i64`

*Defined at line 300*

---

## initrd_clean

Archive names as initramfs reads them: no leading "/" or "./".

```fides
pub fn initrd_clean(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 315*

---

## InitrdStats

```fides
pub struct InitrdStats {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `i64` |
| `dirs` | `i64` |
| `files` | `i64` |
| `symlinks` | `i64` |
| `hardlinks` | `i64` |
| `skipped` | `i64` |
| `bytes` | `i64` |

*Defined at line 340*

---

## initrd_unpack

Unpack the entries of `a` under directory `dest` of `v` (which must
exist). Existing files are replaced, existing directories kept.

```fides
pub fn initrd_unpack(v: &mut Vfs, dest: str, a: &CpioArchive) -> InitrdStats {
```

**Parameters:**

- `v`: `&mut Vfs`
- `dest`: `str`
- `a`: `&CpioArchive`

**Returns:** `InitrdStats`

*Defined at line 356*

---

## initrd_mount

Parse `bytes`, make a ramfs, mount it on `path` (as "/" when `v` has
nothing mounted yet) and unpack into it. `err` is -errno on failure
(VFS_EINVAL for a bad archive).

```fides
pub fn initrd_mount(v: &mut Vfs, path: str, bytes: Vec<i64>) -> InitrdStats {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `bytes`: `Vec<i64>`

**Returns:** `InitrdStats`

*Defined at line 446*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
