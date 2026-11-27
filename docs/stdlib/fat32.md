[All modules](index.md)

# fat32 -- Fides Documentation

## Contents

- [FAT_ATTR_READ_ONLY:](#FAT_ATTR_READ_ONLY:) (const)
- [FAT_ATTR_HIDDEN:](#FAT_ATTR_HIDDEN:) (const)
- [FAT_ATTR_SYSTEM:](#FAT_ATTR_SYSTEM:) (const)
- [FAT_ATTR_VOLUME_ID:](#FAT_ATTR_VOLUME_ID:) (const)
- [FAT_ATTR_DIRECTORY:](#FAT_ATTR_DIRECTORY:) (const)
- [FAT_ATTR_ARCHIVE:](#FAT_ATTR_ARCHIVE:) (const)
- [FAT_ATTR_LFN:](#FAT_ATTR_LFN:) (const)
- [FatVol](#FatVol) (struct)
- [FatDirEnt](#FatDirEnt) (struct)
- [fat_date](#fat_date) (function)
- [fat_time](#fat_time) (function)
- [fat_datetime_str](#fat_datetime_str) (function)
- [fat_set_clock](#fat_set_clock) (function)
- [fat_open](#fat_open) (function)
- [fat_type_name](#fat_type_name) (function)
- [fat_eoc](#fat_eoc) (function)
- [fat_is_eoc](#fat_is_eoc) (function)
- [fat_get](#fat_get) (function)
- [fat_set](#fat_set) (function)
- [fat_chain](#fat_chain) (function)
- [fat_alloc](#fat_alloc) (function)
- [fat_free_chain](#fat_free_chain) (function)
- [fat_count_free](#fat_count_free) (function)
- [fat_sync](#fat_sync) (function)
- [fat_root](#fat_root) (function)
- [fat_lfn_checksum](#fat_lfn_checksum) (function)
- [fat_str_to_utf16](#fat_str_to_utf16) (function)
- [fat_dir_entries](#fat_dir_entries) (function)
- [fat_find](#fat_find) (function)
- [fat_lookup](#fat_lookup) (function)
- [fat_list](#fat_list) (function)
- [fat_short_name](#fat_short_name) (function)
- [fat_read_file](#fat_read_file) (function)
- [fat_read_ent](#fat_read_ent) (function)
- [fat_write_file](#fat_write_file) (function)
- [fat_mkdir](#fat_mkdir) (function)
- [fat_delete](#fat_delete) (function)
- [fat_rename](#fat_rename) (function)
- [fat_format](#fat_format) (function)
- [fat_fs_root](#fat_fs_root) (function)
- [fat_fs_lookup](#fat_fs_lookup) (function)
- [fat_fs_getattr](#fat_fs_getattr) (function)
- [fat_fs_read](#fat_fs_read) (function)
- [fat_fs_write](#fat_fs_write) (function)
- [fat_fs_truncate](#fat_fs_truncate) (function)
- [fat_fs_create](#fat_fs_create) (function)
- [fat_fs_mkdir](#fat_fs_mkdir) (function)
- [fat_fs_unlink](#fat_fs_unlink) (function)
- [fat_fs_rmdir](#fat_fs_rmdir) (function)
- [fat_fs_rename](#fat_fs_rename) (function)
- [fat_fs_readdir](#fat_fs_readdir) (function)
- [fat_ops](#fat_ops) (function)
- [fat_mount](#fat_mount) (function)
- [fat_fs_volume](#fat_fs_volume) (function)

## FAT_ATTR_READ_ONLY:

```fides
pub const FAT_ATTR_READ_ONLY: i64 = 1;
```

*Defined at line 29*

---

## FAT_ATTR_HIDDEN:

```fides
pub const FAT_ATTR_HIDDEN: i64 = 2;
```

*Defined at line 30*

---

## FAT_ATTR_SYSTEM:

```fides
pub const FAT_ATTR_SYSTEM: i64 = 4;
```

*Defined at line 31*

---

## FAT_ATTR_VOLUME_ID:

```fides
pub const FAT_ATTR_VOLUME_ID: i64 = 8;
```

*Defined at line 32*

---

## FAT_ATTR_DIRECTORY:

```fides
pub const FAT_ATTR_DIRECTORY: i64 = 16;
```

*Defined at line 33*

---

## FAT_ATTR_ARCHIVE:

```fides
pub const FAT_ATTR_ARCHIVE: i64 = 32;
```

*Defined at line 34*

---

## FAT_ATTR_LFN:

```fides
pub const FAT_ATTR_LFN: i64 = 15;
```

*Defined at line 35*

---

## FatVol

```fides
pub struct FatVol {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `dev` | `BlockDev` |
| `fat_type` | `i64` |
| `bps` | `i64` |
| `spc` | `i64` |
| `rsvd` | `i64` |
| `nfats` | `i64` |
| `root_ent` | `i64` |
| `tot_sec` | `i64` |
| `fatsz` | `i64` |
| `root_clus` | `i64` |
| `fsinfo` | `i64` |
| `root_start` | `i64` |
| `root_secs` | `i64` |
| `data_start` | `i64` |
| `nclusters` | `i64` |
| `media` | `i64` |
| `volid` | `i64` |
| `label` | `str` |
| `spt` | `i64` |
| `heads` | `i64` |
| `free_count` | `i64` |
| `next_free` | `i64` |
| `date` | `i64` |
| `time` | `i64` |

*Defined at line 37*

---

## FatDirEnt

```fides
pub struct FatDirEnt {
```

**Fields:**

| Name | Type |
|------|------|
| `found` | `bool` |
| `name` | `str` |
| `short` | `str` |
| `attr` | `i64` |
| `clus` | `i64` |
| `size` | `i64` |
| `wdate` | `i64` |
| `wtime` | `i64` |
| `cdate` | `i64` |
| `ctime` | `i64` |
| `dir` | `i64` |
| `slot` | `i64` |
| `first` | `i64` |

*Defined at line 65*

---

## fat_date

```fides
pub fn fat_date(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 126*

---

## fat_time

```fides
pub fn fat_time(hour: i64, minute: i64, second: i64) -> i64 {
```

**Parameters:**

- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

**Returns:** `i64`

*Defined at line 130*

---

## fat_datetime_str

"YYYY-MM-DD hh:mm:ss" of a FAT date and time.

```fides
pub fn fat_datetime_str(d: i64, t: i64) -> str {
```

**Parameters:**

- `d`: `i64`
- `t`: `i64`

**Returns:** `str`

*Defined at line 135*

---

## fat_set_clock

```fides
pub fn fat_set_clock(vol: &mut FatVol, year: i64, month: i64, day: i64, hour: i64, minute: i64, second: i64) {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `hour`: `i64`
- `minute`: `i64`
- `second`: `i64`

*Defined at line 140*

---

## fat_open

Read the boot sector of `dev` and work out the layout. `err` is "" on
success.

```fides
pub fn fat_open(dev: BlockDev) -> FatVol {
```

**Parameters:**

- `dev`: `BlockDev`

**Returns:** `FatVol`

*Defined at line 155*

---

## fat_type_name

```fides
pub fn fat_type_name(vol: &FatVol) -> str {
```

**Parameters:**

- `vol`: `&FatVol`

**Returns:** `str`

*Defined at line 224*

---

## fat_eoc

```fides
pub fn fat_eoc(vol: &FatVol) -> i64 {
```

**Parameters:**

- `vol`: `&FatVol`

**Returns:** `i64`

*Defined at line 230*

---

## fat_is_eoc

```fides
pub fn fat_is_eoc(vol: &FatVol, v: i64) -> bool {
```

**Parameters:**

- `vol`: `&FatVol`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 240*

---

## fat_get

FAT entry `n` (copy 0).

```fides
pub fn fat_get(vol: &FatVol, n: i64) -> i64 {
```

**Parameters:**

- `vol`: `&FatVol`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 261*

---

## fat_set

Set FAT entry `n` in every FAT copy.

```fides
pub fn fat_set(vol: &FatVol, n: i64, value: i64) {
```

**Parameters:**

- `vol`: `&FatVol`
- `n`: `i64`
- `value`: `i64`

*Defined at line 288*

---

## fat_chain

The clusters of the chain starting at `c` (stops at a loop or a bad
entry).

```fides
pub fn fat_chain(vol: &FatVol, start: i64) -> Vec<i64> {
```

**Parameters:**

- `vol`: `&FatVol`
- `start`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 325*

---

## fat_alloc

Allocate a free cluster, mark it end of chain and link it after `prev`
(0: a new chain). Returns the cluster or -ENOSPC.

```fides
pub fn fat_alloc(vol: &mut FatVol, prev: i64) -> i64 {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `prev`: `i64`

**Returns:** `i64`

*Defined at line 341*

---

## fat_free_chain

Free the chain starting at `c`.

```fides
pub fn fat_free_chain(vol: &mut FatVol, start: i64) {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `start`: `i64`

*Defined at line 372*

---

## fat_count_free

Count free clusters by reading the whole FAT, each sector once.

```fides
pub fn fat_count_free(vol: &FatVol) -> i64 {
```

**Parameters:**

- `vol`: `&FatVol`

**Returns:** `i64`

*Defined at line 385*

---

## fat_sync

Write the FSInfo sector (FAT32) with the current free count and hint.

```fides
pub fn fat_sync(vol: &FatVol) {
```

**Parameters:**

- `vol`: `&FatVol`

*Defined at line 432*

---

## fat_root

The root directory's cluster: 0 for the fixed FAT12/16 root region.

```fides
pub fn fat_root(vol: &FatVol) -> i64 {
```

**Parameters:**

- `vol`: `&FatVol`

**Returns:** `i64`

*Defined at line 449*

---

## fat_lfn_checksum

The 8.3 checksum the long-name pieces carry.

```fides
pub fn fat_lfn_checksum(short11: &Vec<i64>) -> i64 {
```

**Parameters:**

- `short11`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 485*

---

## fat_str_to_utf16

UTF-8 -> UTF-16 code units (invalid bytes become U+FFFD).

```fides
pub fn fat_str_to_utf16(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 535*

---

## fat_dir_entries

Every live entry of directory `dclus` ("." and ".." included, the
volume label left out), with long names put together.

```fides
pub fn fat_dir_entries(vol: &FatVol, dclus: i64) -> Vec<FatDirEnt> {
```

**Parameters:**

- `vol`: `&FatVol`
- `dclus`: `i64`

**Returns:** `Vec<FatDirEnt>`

*Defined at line 622*

---

## fat_find

The entry called `name` (long or short name, any ASCII case) in `dclus`.

```fides
pub fn fat_find(vol: &FatVol, dclus: i64, name: str) -> FatDirEnt {
```

**Parameters:**

- `vol`: `&FatVol`
- `dclus`: `i64`
- `name`: `str`

**Returns:** `FatDirEnt`

*Defined at line 709*

---

## fat_lookup

Look `path` up from the root ("/" separated, "." and ".." allowed).

```fides
pub fn fat_lookup(vol: &FatVol, path: str) -> FatDirEnt {
```

**Parameters:**

- `vol`: `&FatVol`
- `path`: `str`

**Returns:** `FatDirEnt`

*Defined at line 733*

---

## fat_list

List a directory (no "." and "..").

```fides
pub fn fat_list(vol: &FatVol, path: str) -> Vec<FatDirEnt> {
```

**Parameters:**

- `vol`: `&FatVol`
- `path`: `str`

**Returns:** `Vec<FatDirEnt>`

*Defined at line 797*

---

## fat_short_name

The 8.3 form of `name` in directory entries `es`: 11 bytes, then the
NT case byte, then 1 when a long name must be written as well.

```fides
pub fn fat_short_name(name: str, es: &Vec<FatDirEnt>) -> Vec<i64> {
```

**Parameters:**

- `name`: `str`
- `es`: `&Vec<FatDirEnt>`

**Returns:** `Vec<i64>`

*Defined at line 878*

---

## fat_read_file

The contents of a file. Sets vol.err ("" when ok).

```fides
pub fn fat_read_file(vol: &mut FatVol, path: str) -> Vec<i64> {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1245*

---

## fat_read_ent

`len` bytes from `off` of the file of entry `e`.

```fides
pub fn fat_read_ent(vol: &FatVol, e: &FatDirEnt, off: i64, len: i64) -> Vec<i64> {
```

**Parameters:**

- `vol`: `&FatVol`
- `e`: `&FatDirEnt`
- `off`: `i64`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1260*

---

## fat_write_file

Create or replace the file at `path` with `data`. Returns 0 or -errno.

```fides
pub fn fat_write_file(vol: &mut FatVol, path: str, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `path`: `str`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1339*

---

## fat_mkdir

Create a directory. Returns 0 or -errno.

```fides
pub fn fat_mkdir(vol: &mut FatVol, path: str) -> i64 {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `path`: `str`

**Returns:** `i64`

*Defined at line 1436*

---

## fat_delete

Delete a file or an empty directory. Returns 0 or -errno.

```fides
pub fn fat_delete(vol: &mut FatVol, path: str) -> i64 {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `path`: `str`

**Returns:** `i64`

*Defined at line 1466*

---

## fat_rename

Rename or move an entry (the target must not exist). A moved
directory's ".." is updated.

```fides
pub fn fat_rename(vol: &mut FatVol, from: str, to: str) -> i64 {
```

**Parameters:**

- `vol`: `&mut FatVol`
- `from`: `str`
- `to`: `str`

**Returns:** `i64`

*Defined at line 1487*

---

## fat_format

Format `dev` as FAT12, FAT16 or FAT32 (`kind` 12/16/32, 0 to choose by
size). `volid` is the volume serial number. The result is the opened
volume (err "" on success).

```fides
pub fn fat_format(dev: BlockDev, kind: i64, label: str, volid: i64) -> FatVol {
```

**Parameters:**

- `dev`: `BlockDev`
- `kind`: `i64`
- `label`: `str`
- `volid`: `i64`

**Returns:** `FatVol`

*Defined at line 1580*

---

## fat_fs_root

```fides
pub fn fat_fs_root(fs: i64) -> i64 {
```

**Parameters:**

- `fs`: `i64`

**Returns:** `i64`

*Defined at line 1829*

---

## fat_fs_lookup

```fides
pub fn fat_fs_lookup(fs: i64, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 1836*

---

## fat_fs_getattr

```fides
pub fn fat_fs_getattr(fs: i64, ino: i64) -> Vec<i64> {
```

**Parameters:**

- `fs`: `i64`
- `ino`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1852*

---

## fat_fs_read

```fides
pub fn fat_fs_read(fs: i64, ino: i64, off: i64, len: i64) -> Vec<i64> {
```

**Parameters:**

- `fs`: `i64`
- `ino`: `i64`
- `off`: `i64`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1884*

---

## fat_fs_write

```fides
pub fn fat_fs_write(fs: i64, ino: i64, off: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `ino`: `i64`
- `off`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1912*

---

## fat_fs_truncate

```fides
pub fn fat_fs_truncate(fs: i64, ino: i64, size: i64) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `ino`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 1941*

---

## fat_fs_create

```fides
pub fn fat_fs_create(fs: i64, dir: i64, name: str, mode: i64) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `name`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 1974*

---

## fat_fs_mkdir

```fides
pub fn fat_fs_mkdir(fs: i64, dir: i64, name: str, mode: i64) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `name`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 1982*

---

## fat_fs_unlink

```fides
pub fn fat_fs_unlink(fs: i64, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2015*

---

## fat_fs_rmdir

```fides
pub fn fat_fs_rmdir(fs: i64, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2019*

---

## fat_fs_rename

```fides
pub fn fat_fs_rename(fs: i64, odir: i64, oname: str, ndir: i64, nname: str) -> i64 {
```

**Parameters:**

- `fs`: `i64`
- `odir`: `i64`
- `oname`: `str`
- `ndir`: `i64`
- `nname`: `str`

**Returns:** `i64`

*Defined at line 2023*

---

## fat_fs_readdir

```fides
pub fn fat_fs_readdir(fs: i64, dir: i64, index: i64) -> str {
```

**Parameters:**

- `fs`: `i64`
- `dir`: `i64`
- `index`: `i64`

**Returns:** `str`

*Defined at line 2060*

---

## fat_ops

```fides
pub fn fat_ops() -> FsOps {
```

**Returns:** `FsOps`

*Defined at line 2090*

---

## fat_mount

Register an opened volume as a vfs file system and mount it on `path`.
Returns the file system handle or -errno.

```fides
pub fn fat_mount(v: &mut Vfs, path: str, vol: FatVol) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `vol`: `FatVol`

**Returns:** `i64`

*Defined at line 2098*

---

## fat_fs_volume

The volume behind a mounted handle (its free count and hint are kept
up to date by the vfs operations).

```fides
pub fn fat_fs_volume(fs: i64) -> FatVol {
```

**Parameters:**

- `fs`: `i64`

**Returns:** `FatVol`

*Defined at line 2113*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
