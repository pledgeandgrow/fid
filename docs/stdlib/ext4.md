[All modules](index.md)

# ext4 -- Fides Documentation

## Contents

- [EXT4_MAGIC:](#EXT4_MAGIC:) (const)
- [EXT4_ROOT_INO:](#EXT4_ROOT_INO:) (const)
- [EXT4_FEATURE_INCOMPAT_FILETYPE:](#EXT4_FEATURE_INCOMPAT_FILETYPE:) (const)
- [EXT4_FEATURE_INCOMPAT_RECOVER:](#EXT4_FEATURE_INCOMPAT_RECOVER:) (const)
- [EXT4_FEATURE_INCOMPAT_META_BG:](#EXT4_FEATURE_INCOMPAT_META_BG:) (const)
- [EXT4_FEATURE_INCOMPAT_EXTENTS:](#EXT4_FEATURE_INCOMPAT_EXTENTS:) (const)
- [EXT4_FEATURE_INCOMPAT_64BIT:](#EXT4_FEATURE_INCOMPAT_64BIT:) (const)
- [EXT4_FEATURE_INCOMPAT_MMP:](#EXT4_FEATURE_INCOMPAT_MMP:) (const)
- [EXT4_FEATURE_INCOMPAT_FLEX_BG:](#EXT4_FEATURE_INCOMPAT_FLEX_BG:) (const)
- [EXT4_FEATURE_INCOMPAT_CSUM_SEED:](#EXT4_FEATURE_INCOMPAT_CSUM_SEED:) (const)
- [EXT4_FEATURE_INCOMPAT_LARGEDIR:](#EXT4_FEATURE_INCOMPAT_LARGEDIR:) (const)
- [EXT4_FEATURE_RO_COMPAT_METADATA_CSUM:](#EXT4_FEATURE_RO_COMPAT_METADATA_CSUM:) (const)
- [EXT4_FEATURE_COMPAT_HAS_JOURNAL:](#EXT4_FEATURE_COMPAT_HAS_JOURNAL:) (const)
- [EXT4_INDEX_FL:](#EXT4_INDEX_FL:) (const)
- [EXT4_EXTENTS_FL:](#EXT4_EXTENTS_FL:) (const)
- [EXT4_INLINE_DATA_FL:](#EXT4_INLINE_DATA_FL:) (const)
- [EXT4_FT_REG:](#EXT4_FT_REG:) (const)
- [EXT4_FT_DIR:](#EXT4_FT_DIR:) (const)
- [EXT4_FT_SYMLINK:](#EXT4_FT_SYMLINK:) (const)
- [Ext4Fs](#Ext4Fs) (struct)
- [Ext4Inode](#Ext4Inode) (struct)
- [Ext4Dirent](#Ext4Dirent) (struct)
- [ext4_crc32c](#ext4_crc32c) (function)
- [ext4_block](#ext4_block) (function)
- [ext4_supported_incompat](#ext4_supported_incompat) (function)
- [ext4_open](#ext4_open) (function)
- [ext4_open_opts](#ext4_open_opts) (function)
- [ext4_inode](#ext4_inode) (function)
- [ext4_is_dir](#ext4_is_dir) (function)
- [ext4_is_reg](#ext4_is_reg) (function)
- [ext4_is_symlink](#ext4_is_symlink) (function)
- [ext4_extents](#ext4_extents) (function)
- [ext4_block_map](#ext4_block_map) (function)
- [ext4_read](#ext4_read) (function)
- [ext4_readlink](#ext4_readlink) (function)
- [ext4_readdir](#ext4_readdir) (function)
- [ext4_dir_lookup](#ext4_dir_lookup) (function)
- [ext4_lookup](#ext4_lookup) (function)
- [ext4_read_file](#ext4_read_file) (function)
- [ext4_put_block](#ext4_put_block) (function)
- [ext4_writable_ro_compat](#ext4_writable_ro_compat) (function)
- [ext4_sync](#ext4_sync) (function)
- [ext4_set_clock](#ext4_set_clock) (function)
- [EXT4_BG_INODE_UNINIT:](#EXT4_BG_INODE_UNINIT:) (const)
- [EXT4_BG_BLOCK_UNINIT:](#EXT4_BG_BLOCK_UNINIT:) (const)
- [EXT4_BG_INODE_ZEROED:](#EXT4_BG_INODE_ZEROED:) (const)
- [Ext4Group](#Ext4Group) (struct)
- [ext4_group](#ext4_group) (function)
- [ext4_group_has_super](#ext4_group_has_super) (function)
- [ext4_alloc_block](#ext4_alloc_block) (function)
- [ext4_free_blocks](#ext4_free_blocks) (function)
- [ext4_alloc_inode](#ext4_alloc_inode) (function)
- [ext4_free_inode](#ext4_free_inode) (function)
- [ext4_write_inode](#ext4_write_inode) (function)
- [ext4_extent_insert](#ext4_extent_insert) (function)
- [ext4_extent_tree_blocks](#ext4_extent_tree_blocks) (function)
- [ext4_dir_unindex](#ext4_dir_unindex) (function)
- [ext4_dir_add](#ext4_dir_add) (function)
- [ext4_dir_remove](#ext4_dir_remove) (function)
- [ext4_create](#ext4_create) (function)
- [ext4_mkdir](#ext4_mkdir) (function)
- [ext4_symlink](#ext4_symlink) (function)
- [ext4_write](#ext4_write) (function)
- [ext4_truncate](#ext4_truncate) (function)
- [ext4_unlink](#ext4_unlink) (function)
- [ext4_rmdir](#ext4_rmdir) (function)
- [ext4_link](#ext4_link) (function)
- [ext4_rename](#ext4_rename) (function)
- [ext4_write_file](#ext4_write_file) (function)
- [ext4_mkdir_path](#ext4_mkdir_path) (function)
- [ext4_unlink_path](#ext4_unlink_path) (function)
- [ext4_rmdir_path](#ext4_rmdir_path) (function)
- [ext4_rename_path](#ext4_rename_path) (function)
- [ext4_symlink_path](#ext4_symlink_path) (function)
- [ext4_truncate_path](#ext4_truncate_path) (function)
- [JBD2_MAGIC:](#JBD2_MAGIC:) (const)
- [JBD2_FEATURE_INCOMPAT_REVOKE:](#JBD2_FEATURE_INCOMPAT_REVOKE:) (const)
- [JBD2_FEATURE_INCOMPAT_64BIT:](#JBD2_FEATURE_INCOMPAT_64BIT:) (const)
- [JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT:](#JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT:) (const)
- [JBD2_FEATURE_INCOMPAT_CSUM_V2:](#JBD2_FEATURE_INCOMPAT_CSUM_V2:) (const)
- [JBD2_FEATURE_INCOMPAT_CSUM_V3:](#JBD2_FEATURE_INCOMPAT_CSUM_V3:) (const)
- [JBD2_FEATURE_INCOMPAT_FAST_COMMIT:](#JBD2_FEATURE_INCOMPAT_FAST_COMMIT:) (const)
- [Ext4Recovery](#Ext4Recovery) (struct)
- [ext4_journal_recover](#ext4_journal_recover) (function)
- [ext4_ops](#ext4_ops) (function)
- [ext4_mounted](#ext4_mounted) (function)
- [ext4_mount](#ext4_mount) (function)

## EXT4_MAGIC:

```fides
pub const EXT4_MAGIC: i64 = 61267;
```

*Defined at line 69*

---

## EXT4_ROOT_INO:

```fides
pub const EXT4_ROOT_INO: i64 = 2;
```

*Defined at line 70*

---

## EXT4_FEATURE_INCOMPAT_FILETYPE:

```fides
pub const EXT4_FEATURE_INCOMPAT_FILETYPE: i64 = 2;
```

*Defined at line 73*

---

## EXT4_FEATURE_INCOMPAT_RECOVER:

```fides
pub const EXT4_FEATURE_INCOMPAT_RECOVER: i64 = 4;
```

*Defined at line 74*

---

## EXT4_FEATURE_INCOMPAT_META_BG:

```fides
pub const EXT4_FEATURE_INCOMPAT_META_BG: i64 = 16;
```

*Defined at line 75*

---

## EXT4_FEATURE_INCOMPAT_EXTENTS:

```fides
pub const EXT4_FEATURE_INCOMPAT_EXTENTS: i64 = 64;
```

*Defined at line 76*

---

## EXT4_FEATURE_INCOMPAT_64BIT:

```fides
pub const EXT4_FEATURE_INCOMPAT_64BIT: i64 = 128;
```

*Defined at line 77*

---

## EXT4_FEATURE_INCOMPAT_MMP:

```fides
pub const EXT4_FEATURE_INCOMPAT_MMP: i64 = 256;
```

*Defined at line 78*

---

## EXT4_FEATURE_INCOMPAT_FLEX_BG:

```fides
pub const EXT4_FEATURE_INCOMPAT_FLEX_BG: i64 = 512;
```

*Defined at line 79*

---

## EXT4_FEATURE_INCOMPAT_CSUM_SEED:

```fides
pub const EXT4_FEATURE_INCOMPAT_CSUM_SEED: i64 = 8192;
```

*Defined at line 80*

---

## EXT4_FEATURE_INCOMPAT_LARGEDIR:

```fides
pub const EXT4_FEATURE_INCOMPAT_LARGEDIR: i64 = 16384;
```

*Defined at line 81*

---

## EXT4_FEATURE_RO_COMPAT_METADATA_CSUM:

```fides
pub const EXT4_FEATURE_RO_COMPAT_METADATA_CSUM: i64 = 1024;
```

*Defined at line 83*

---

## EXT4_FEATURE_COMPAT_HAS_JOURNAL:

```fides
pub const EXT4_FEATURE_COMPAT_HAS_JOURNAL: i64 = 4;
```

*Defined at line 85*

---

## EXT4_INDEX_FL:

```fides
pub const EXT4_INDEX_FL: i64 = 4096;
```

*Defined at line 88*

---

## EXT4_EXTENTS_FL:

```fides
pub const EXT4_EXTENTS_FL: i64 = 524288;
```

*Defined at line 89*

---

## EXT4_INLINE_DATA_FL:

```fides
pub const EXT4_INLINE_DATA_FL: i64 = 268435456;
```

*Defined at line 90*

---

## EXT4_FT_REG:

```fides
pub const EXT4_FT_REG: i64 = 1;
```

*Defined at line 93*

---

## EXT4_FT_DIR:

```fides
pub const EXT4_FT_DIR: i64 = 2;
```

*Defined at line 94*

---

## EXT4_FT_SYMLINK:

```fides
pub const EXT4_FT_SYMLINK: i64 = 7;
```

*Defined at line 95*

---

## Ext4Fs

```fides
pub struct Ext4Fs {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `dev` | `BlockDev` |
| `bs` | `i64` |
| `blocks_count` | `i64` |
| `inodes_count` | `i64` |
| `free_blocks` | `i64` |
| `free_inodes` | `i64` |
| `first_data_block` | `i64` |
| `bpg` | `i64` |
| `ipg` | `i64` |
| `inode_size` | `i64` |
| `desc_size` | `i64` |
| `ngroups` | `i64` |
| `rev` | `i64` |
| `compat` | `i64` |
| `incompat` | `i64` |
| `ro_compat` | `i64` |
| `label` | `str` |
| `uuid` | `Vec<i64>` |
| `csum` | `bool` |
| `csum_seed` | `i64` |
| `sb_csum_ok` | `bool` |
| `sb` | `Vec<i64>` |
| `first_ino` | `i64` |
| `writable` | `bool` |
| `ro_reason` | `str` |
| `now` | `i64` |
| `next_gen` | `i64` |
| `recovered` | `bool` |
| `replayed_transactions` | `i64` |
| `replayed_blocks` | `i64` |
| `revoked_blocks` | `i64` |
| `journal_bad_checksums` | `i64` |

*Defined at line 97*

---

## Ext4Inode

```fides
pub struct Ext4Inode {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `i64` |
| `ino` | `i64` |
| `mode` | `i64` |
| `uid` | `i64` |
| `gid` | `i64` |
| `size` | `i64` |
| `links` | `i64` |
| `blocks` | `i64` |
| `flags` | `i64` |
| `atime` | `i64` |
| `ctime` | `i64` |
| `mtime` | `i64` |
| `generation` | `i64` |
| `iblock` | `Vec<i64>` |
| `csum_ok` | `bool` |
| `dtime` | `i64` |
| `file_acl` | `i64` |

*Defined at line 133*

---

## Ext4Dirent

```fides
pub struct Ext4Dirent {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ino` | `i64` |
| `ftype` | `i64` |

*Defined at line 153*

---

## ext4_crc32c

crc32c without the final inversion (the kernel's crc32c_le): ext4
starts it at 0xFFFFFFFF and stores the result as it is.

```fides
pub fn ext4_crc32c(crc: i64, data: &Vec<i64>, start: i64, len: i64) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `data`: `&Vec<i64>`
- `start`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## ext4_block

File system block `n` (empty when it cannot be read).

```fides
pub fn ext4_block(fs: &Ext4Fs, n: i64) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 229*

---

## ext4_supported_incompat

Incompatible features this reader handles.

```fides
pub fn ext4_supported_incompat() -> i64 {
```

**Returns:** `i64`

*Defined at line 246*

---

## ext4_open

Open the file system on `dev`. `err` is "" on success. A file system
whose journal needs recovery is recovered first (ext4_journal_recover),
as the kernel does on mount; `writable` says whether this driver may
change it (`ro_reason` says why not).

```fides
pub fn ext4_open(dev: BlockDev) -> Ext4Fs {
```

**Parameters:**

- `dev`: `BlockDev`

**Returns:** `Ext4Fs`

*Defined at line 336*

---

## ext4_open_opts

ext4_open, with journal recovery optional: `recover` false opens a
file system that needs recovery read-only without replaying it (as
mount -o ro,norecovery; what it reads may be out of date).

```fides
pub fn ext4_open_opts(dev: BlockDev, recover: bool) -> Ext4Fs {
```

**Parameters:**

- `dev`: `BlockDev`
- `recover`: `bool`

**Returns:** `Ext4Fs`

*Defined at line 343*

---

## ext4_inode

Read inode `ino` (1-based). `err` is 0 or -errno; `csum_ok` is false
when metadata_csum is on and the checksum does not match.

```fides
pub fn ext4_inode(fs: &Ext4Fs, ino: i64) -> Ext4Inode {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `ino`: `i64`

**Returns:** `Ext4Inode`

*Defined at line 401*

---

## ext4_is_dir

```fides
pub fn ext4_is_dir(n: &Ext4Inode) -> bool {
```

**Parameters:**

- `n`: `&Ext4Inode`

**Returns:** `bool`

*Defined at line 453*

---

## ext4_is_reg

```fides
pub fn ext4_is_reg(n: &Ext4Inode) -> bool {
```

**Parameters:**

- `n`: `&Ext4Inode`

**Returns:** `bool`

*Defined at line 457*

---

## ext4_is_symlink

```fides
pub fn ext4_is_symlink(n: &Ext4Inode) -> bool {
```

**Parameters:**

- `n`: `&Ext4Inode`

**Returns:** `bool`

*Defined at line 461*

---

## ext4_extents

Extents of an inode as flat words [logical, length, physical,
uninitialized] each, collected through an extent tree of any depth.

```fides
pub fn ext4_extents(fs: &Ext4Fs, n: &Ext4Inode) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `Vec<i64>`

*Defined at line 469*

---

## ext4_block_map

The physical block of every logical block of the file (0 = a hole or
an uninitialized extent).

```fides
pub fn ext4_block_map(fs: &Ext4Fs, n: &Ext4Inode) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `Vec<i64>`

*Defined at line 551*

---

## ext4_read

`len` bytes of the file from `off` (holes read as zeros).

```fides
pub fn ext4_read(fs: &Ext4Fs, n: &Ext4Inode, off: i64, len: i64) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`
- `off`: `i64`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 585*

---

## ext4_readlink

The target of a symlink inode.

```fides
pub fn ext4_readlink(fs: &Ext4Fs, n: &Ext4Inode) -> str {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `str`

*Defined at line 619*

---

## ext4_readdir

Every entry of a directory inode (including "." and ".."), read
linearly block by block.

```fides
pub fn ext4_readdir(fs: &Ext4Fs, n: &Ext4Inode) -> Vec<Ext4Dirent> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `Vec<Ext4Dirent>`

*Defined at line 633*

---

## ext4_dir_lookup

The inode of `name` in directory inode `dir`, or -ENOENT.

```fides
pub fn ext4_dir_lookup(fs: &Ext4Fs, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 672*

---

## ext4_lookup

Resolve an absolute path to an inode number (or -errno), following
symlinks (in the last component too when `follow`).

```fides
pub fn ext4_lookup(fs: &Ext4Fs, path: str, follow: bool) -> i64 {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `path`: `str`
- `follow`: `bool`

**Returns:** `i64`

*Defined at line 693*

---

## ext4_read_file

The contents of the file at `path`; sets fs.err ("" when ok).

```fides
pub fn ext4_read_file(fs: &mut Ext4Fs, path: str) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 760*

---

## ext4_put_block

Write file system block `n` (bs bytes). 0 or -errno.

```fides
pub fn ext4_put_block(fs: &Ext4Fs, n: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 816*

---

## ext4_writable_ro_compat

ro_compat features writing knows: sparse_super, large_file, btree_dir,
huge_file, gdt_csum (uninit_bg), dir_nlink, extra_isize,
metadata_csum.

```fides
pub fn ext4_writable_ro_compat() -> i64 {
```

**Returns:** `i64`

*Defined at line 867*

---

## ext4_sync

The superblock as it is now: counts, write time and checksum, written
to byte 1024 of the device.

```fides
pub fn ext4_sync(fs: &mut Ext4Fs) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`

**Returns:** `i64`

*Defined at line 907*

---

## ext4_set_clock

The clock new timestamps come from (seconds since 1970). It starts at
the superblock's write time.

```fides
pub fn ext4_set_clock(fs: &mut Ext4Fs, now: i64) {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `now`: `i64`

*Defined at line 926*

---

## EXT4_BG_INODE_UNINIT:

```fides
pub const EXT4_BG_INODE_UNINIT: i64 = 1;
```

*Defined at line 932*

---

## EXT4_BG_BLOCK_UNINIT:

```fides
pub const EXT4_BG_BLOCK_UNINIT: i64 = 2;
```

*Defined at line 933*

---

## EXT4_BG_INODE_ZEROED:

```fides
pub const EXT4_BG_INODE_ZEROED: i64 = 4;
```

*Defined at line 934*

---

## Ext4Group

One block group descriptor.

```fides
pub struct Ext4Group {
```

**Fields:**

| Name | Type |
|------|------|
| `block_bitmap` | `i64` |
| `inode_bitmap` | `i64` |
| `inode_table` | `i64` |
| `free_blocks` | `i64` |
| `free_inodes` | `i64` |
| `used_dirs` | `i64` |
| `flags` | `i64` |
| `itable_unused` | `i64` |

*Defined at line 937*

---

## ext4_group

Block group descriptor `g`.

```fides
pub fn ext4_group(fs: &Ext4Fs, g: i64) -> Ext4Group {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `g`: `i64`

**Returns:** `Ext4Group`

*Defined at line 958*

---

## ext4_group_has_super

Does group `g` hold a superblock backup (sparse_super: 0, 1 and powers
of 3, 5, 7)?

```fides
pub fn ext4_group_has_super(fs: &Ext4Fs, g: i64) -> bool {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `g`: `i64`

**Returns:** `bool`

*Defined at line 1082*

---

## ext4_alloc_block

Allocate one block, the first free one at or after `goal` in its group,
then in the following groups. The block number, or -ENOSPC.

```fides
pub fn ext4_alloc_block(fs: &mut Ext4Fs, goal: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `goal`: `i64`

**Returns:** `i64`

*Defined at line 1209*

---

## ext4_free_blocks

Free `count` blocks from `start`. Blocks already free are left alone.

```fides
pub fn ext4_free_blocks(fs: &mut Ext4Fs, start: i64, count: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `start`: `i64`
- `count`: `i64`

**Returns:** `i64`

*Defined at line 1253*

---

## ext4_alloc_inode

Allocate an inode, from group `g0` on (the parent's group for files;
for directories the group with the most free inodes is tried first).
The inode number, or -ENOSPC.

```fides
pub fn ext4_alloc_inode(fs: &mut Ext4Fs, g0: i64, is_dir: bool) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `g0`: `i64`
- `is_dir`: `bool`

**Returns:** `i64`

*Defined at line 1288*

---

## ext4_free_inode

Free inode `ino` in its bitmap and counts.

```fides
pub fn ext4_free_inode(fs: &mut Ext4Fs, ino: i64, is_dir: bool) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `ino`: `i64`
- `is_dir`: `bool`

**Returns:** `i64`

*Defined at line 1339*

---

## ext4_write_inode

Write inode `n` back: its fields over the on-disk bytes (extra fields
and in-inode attributes are kept), with the checksum.

```fides
pub fn ext4_write_inode(fs: &Ext4Fs, n: &Ext4Inode) -> i64 {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `i64`

*Defined at line 1399*

---

## ext4_extent_insert

Map logical blocks lblk.. lblk+len-1 to pblk.. in an extent tree: merged
into a neighbouring extent when contiguous, else inserted, splitting
full nodes and growing the tree as needed. The inode (root, i_blocks)
is changed in memory; the caller writes it.

```fides
pub fn ext4_extent_insert(fs: &mut Ext4Fs, n: &mut Ext4Inode, lblk: i64, len: i64, pblk: i64, uninit: bool) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `n`: `&mut Ext4Inode`
- `lblk`: `i64`
- `len`: `i64`
- `pblk`: `i64`
- `uninit`: `bool`

**Returns:** `i64`

*Defined at line 1762*

---

## ext4_extent_tree_blocks

The blocks of an extent tree's nodes below the root.

```fides
pub fn ext4_extent_tree_blocks(fs: &Ext4Fs, n: &Ext4Inode) -> Vec<i64> {
```

**Parameters:**

- `fs`: `&Ext4Fs`
- `n`: `&Ext4Inode`

**Returns:** `Vec<i64>`

*Defined at line 1842*

---

## ext4_dir_unindex

An htree directory becomes a linear one (as e2fsck accepts): the index
flag is cleared and the index blocks (the root after "." and "..",
and interior nodes) rewritten as empty directory blocks, so every
block reads linearly. Leaf blocks stay as they are.

```fides
pub fn ext4_dir_unindex(fs: &mut Ext4Fs, d: &mut Ext4Inode) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `d`: `&mut Ext4Inode`

**Returns:** `i64`

*Defined at line 2401*

---

## ext4_dir_add

Add the entry name -> ino to directory `d`: into the first block with
room (splitting an entry's slack), else a new block at the end. The
directory inode is written.

```fides
pub fn ext4_dir_add(fs: &mut Ext4Fs, d: &mut Ext4Inode, name: str, ino: i64, ftype: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `d`: `&mut Ext4Inode`
- `name`: `str`
- `ino`: `i64`
- `ftype`: `i64`

**Returns:** `i64`

*Defined at line 2441*

---

## ext4_dir_remove

Remove `name` from directory `d` (its space joins the entry before
it, or the entry is marked unused at the start of a block). The inode
it named, or -ENOENT.

```fides
pub fn ext4_dir_remove(fs: &mut Ext4Fs, d: &mut Ext4Inode, name: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `d`: `&mut Ext4Inode`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2503*

---

## ext4_create

Create an empty regular file `name` in directory inode `dir` with
permission bits `mode`. The new inode number or -errno.

```fides
pub fn ext4_create(fs: &mut Ext4Fs, dir: i64, name: str, mode: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `dir`: `i64`
- `name`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 2692*

---

## ext4_mkdir

mkdir in directory inode `dir`. The new inode number or -errno.

```fides
pub fn ext4_mkdir(fs: &mut Ext4Fs, dir: i64, name: str, mode: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `dir`: `i64`
- `name`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 2697*

---

## ext4_symlink

A symlink `name` -> `target` in directory inode `dir` (a fast symlink
in i_block below 60 bytes, else a data block).

```fides
pub fn ext4_symlink(fs: &mut Ext4Fs, dir: i64, name: str, target: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `dir`: `i64`
- `name`: `str`
- `target`: `str`

**Returns:** `i64`

*Defined at line 2703*

---

## ext4_write

Write `data` into file inode `ino` at byte `off`. Bytes written or
-errno (a short count when the disk filled up after some blocks).

```fides
pub fn ext4_write(fs: &mut Ext4Fs, ino: i64, off: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `ino`: `i64`
- `off`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 2712*

---

## ext4_truncate

Set a regular file's size: blocks past it are freed; growing leaves a
hole.

```fides
pub fn ext4_truncate(fs: &mut Ext4Fs, ino: i64, size: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `ino`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 2748*

---

## ext4_unlink

Remove the non-directory `name` from directory inode `dir`.

```fides
pub fn ext4_unlink(fs: &mut Ext4Fs, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2793*

---

## ext4_rmdir

Remove the empty directory `name` from directory inode `dir`.

```fides
pub fn ext4_rmdir(fs: &mut Ext4Fs, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2823*

---

## ext4_link

A hard link `name` in directory `dir` to the non-directory `ino`.

```fides
pub fn ext4_link(fs: &mut Ext4Fs, ino: i64, dir: i64, name: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `ino`: `i64`
- `dir`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 2862*

---

## ext4_rename

rename(2): `oname` in directory `odir` becomes `nname` in `ndir`,
replacing a file there (or an empty directory, for a directory).

```fides
pub fn ext4_rename(fs: &mut Ext4Fs, odir: i64, oname: str, ndir: i64, nname: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `odir`: `i64`
- `oname`: `str`
- `ndir`: `i64`
- `nname`: `str`

**Returns:** `i64`

*Defined at line 2922*

---

## ext4_write_file

Write a whole file at `path` (created when missing, truncated when it
exists). 0 or -errno.

```fides
pub fn ext4_write_file(fs: &mut Ext4Fs, path: str, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 3035*

---

## ext4_mkdir_path

```fides
pub fn ext4_mkdir_path(fs: &mut Ext4Fs, path: str, mode: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 3063*

---

## ext4_unlink_path

```fides
pub fn ext4_unlink_path(fs: &mut Ext4Fs, path: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 3072*

---

## ext4_rmdir_path

```fides
pub fn ext4_rmdir_path(fs: &mut Ext4Fs, path: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 3081*

---

## ext4_rename_path

```fides
pub fn ext4_rename_path(fs: &mut Ext4Fs, from: str, to: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `from`: `str`
- `to`: `str`

**Returns:** `i64`

*Defined at line 3090*

---

## ext4_symlink_path

```fides
pub fn ext4_symlink_path(fs: &mut Ext4Fs, target: str, path: str) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `target`: `str`
- `path`: `str`

**Returns:** `i64`

*Defined at line 3104*

---

## ext4_truncate_path

```fides
pub fn ext4_truncate_path(fs: &mut Ext4Fs, path: str, size: i64) -> i64 {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`
- `path`: `str`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 3113*

---

## JBD2_MAGIC:

```fides
pub const JBD2_MAGIC: i64 = 3225106840;
```

*Defined at line 3123*

---

## JBD2_FEATURE_INCOMPAT_REVOKE:

```fides
pub const JBD2_FEATURE_INCOMPAT_REVOKE: i64 = 1;
```

*Defined at line 3124*

---

## JBD2_FEATURE_INCOMPAT_64BIT:

```fides
pub const JBD2_FEATURE_INCOMPAT_64BIT: i64 = 2;
```

*Defined at line 3125*

---

## JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT:

```fides
pub const JBD2_FEATURE_INCOMPAT_ASYNC_COMMIT: i64 = 4;
```

*Defined at line 3126*

---

## JBD2_FEATURE_INCOMPAT_CSUM_V2:

```fides
pub const JBD2_FEATURE_INCOMPAT_CSUM_V2: i64 = 8;
```

*Defined at line 3127*

---

## JBD2_FEATURE_INCOMPAT_CSUM_V3:

```fides
pub const JBD2_FEATURE_INCOMPAT_CSUM_V3: i64 = 16;
```

*Defined at line 3128*

---

## JBD2_FEATURE_INCOMPAT_FAST_COMMIT:

```fides
pub const JBD2_FEATURE_INCOMPAT_FAST_COMMIT: i64 = 32;
```

*Defined at line 3129*

---

## Ext4Recovery

What recovery found and did.

```fides
pub struct Ext4Recovery {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `str` |
| `needed` | `bool` |
| `transactions` | `i64` |
| `replayed` | `i64` |
| `revoked` | `i64` |
| `bad_checksums` | `i64` |
| `next_sequence` | `i64` |

*Defined at line 3132*

---

## ext4_journal_recover

Replay the journal (when the file system needs recovery) the way jbd2
does: a scan for the last committed transaction (a commit block whose
checksum fails ends the log), the revoke records of those
transactions, then every logged block of every committed transaction
that no later revoke covers, written to its place (escaped blocks
restored). The journal is then marked empty and the needs_recovery
flag cleared. Data blocks whose tag checksum fails are not written
(counted in bad_checksums).

```fides
pub fn ext4_journal_recover(fs: &mut Ext4Fs) -> Ext4Recovery {
```

**Parameters:**

- `fs`: `&mut Ext4Fs`

**Returns:** `Ext4Recovery`

*Defined at line 3312*

---

## ext4_ops

The vfs operations: reads always, changes through ext4_write and the
rest (EROFS when the file system is not writable).

```fides
pub fn ext4_ops() -> FsOps {
```

**Returns:** `FsOps`

*Defined at line 3629*

---

## ext4_mounted

The mounted copy of file system handle `h` (as changed through the vfs).

```fides
pub fn ext4_mounted(h: i64) -> Ext4Fs {
```

**Parameters:**

- `h`: `i64`

**Returns:** `Ext4Fs`

*Defined at line 3643*

---

## ext4_mount

Mount an opened file system on `path` (read-write when `writable`,
else every change is EROFS); returns its handle or -errno. The mounted
copy is the one that changes: ext4_mounted(h) gives it back.

```fides
pub fn ext4_mount(v: &mut Vfs, path: str, fs: Ext4Fs) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `fs`: `Ext4Fs`

**Returns:** `i64`

*Defined at line 3650*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
