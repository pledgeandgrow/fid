[All modules](index.md)

# vfs -- Fides Documentation

## Contents

- [VFS_EPERM:](#VFS_EPERM:) (const)
- [VFS_ENOENT:](#VFS_ENOENT:) (const)
- [VFS_EIO:](#VFS_EIO:) (const)
- [VFS_EBADF:](#VFS_EBADF:) (const)
- [VFS_EACCES:](#VFS_EACCES:) (const)
- [VFS_EBUSY:](#VFS_EBUSY:) (const)
- [VFS_EEXIST:](#VFS_EEXIST:) (const)
- [VFS_EXDEV:](#VFS_EXDEV:) (const)
- [VFS_ENOTDIR:](#VFS_ENOTDIR:) (const)
- [VFS_EISDIR:](#VFS_EISDIR:) (const)
- [VFS_EINVAL:](#VFS_EINVAL:) (const)
- [VFS_EMFILE:](#VFS_EMFILE:) (const)
- [VFS_EFBIG:](#VFS_EFBIG:) (const)
- [VFS_ENOSPC:](#VFS_ENOSPC:) (const)
- [VFS_EROFS:](#VFS_EROFS:) (const)
- [VFS_ENAMETOOLONG:](#VFS_ENAMETOOLONG:) (const)
- [VFS_ENOSYS:](#VFS_ENOSYS:) (const)
- [VFS_ENOTEMPTY:](#VFS_ENOTEMPTY:) (const)
- [VFS_ELOOP:](#VFS_ELOOP:) (const)
- [VFS_REG:](#VFS_REG:) (const)
- [VFS_DIR:](#VFS_DIR:) (const)
- [VFS_LNK:](#VFS_LNK:) (const)
- [VFS_S_IFMT:](#VFS_S_IFMT:) (const)
- [VFS_S_IFREG:](#VFS_S_IFREG:) (const)
- [VFS_S_IFDIR:](#VFS_S_IFDIR:) (const)
- [VFS_S_IFLNK:](#VFS_S_IFLNK:) (const)
- [VFS_O_RDONLY:](#VFS_O_RDONLY:) (const)
- [VFS_O_WRONLY:](#VFS_O_WRONLY:) (const)
- [VFS_O_RDWR:](#VFS_O_RDWR:) (const)
- [VFS_O_ACCMODE:](#VFS_O_ACCMODE:) (const)
- [VFS_O_CREAT:](#VFS_O_CREAT:) (const)
- [VFS_O_EXCL:](#VFS_O_EXCL:) (const)
- [VFS_O_TRUNC:](#VFS_O_TRUNC:) (const)
- [VFS_O_APPEND:](#VFS_O_APPEND:) (const)
- [VFS_O_DIRECTORY:](#VFS_O_DIRECTORY:) (const)
- [VFS_O_NOFOLLOW:](#VFS_O_NOFOLLOW:) (const)
- [VFS_SEEK_SET:](#VFS_SEEK_SET:) (const)
- [VFS_SEEK_CUR:](#VFS_SEEK_CUR:) (const)
- [VFS_SEEK_END:](#VFS_SEEK_END:) (const)
- [VFS_MAX_FDS:](#VFS_MAX_FDS:) (const)
- [VFS_MAX_LINKS:](#VFS_MAX_LINKS:) (const)
- [VFS_NAME_MAX:](#VFS_NAME_MAX:) (const)
- [VFS_ST_ERR:](#VFS_ST_ERR:) (const)
- [VFS_ST_KIND:](#VFS_ST_KIND:) (const)
- [VFS_ST_SIZE:](#VFS_ST_SIZE:) (const)
- [VFS_ST_NLINK:](#VFS_ST_NLINK:) (const)
- [VFS_ST_MODE:](#VFS_ST_MODE:) (const)
- [VFS_ST_MTIME:](#VFS_ST_MTIME:) (const)
- [FsOps](#FsOps) (struct)
- [vfs_ops_readonly](#vfs_ops_readonly) (function)
- [VStat](#VStat) (struct)
- [VDirent](#VDirent) (struct)
- [VMount](#VMount) (struct)
- [VFile](#VFile) (struct)
- [Vfs](#Vfs) (struct)
- [VWalk](#VWalk) (struct)
- [vfs_new](#vfs_new) (function)
- [vfs_getattr](#vfs_getattr) (function)
- [vfs_walk](#vfs_walk) (function)
- [vfs_mount](#vfs_mount) (function)
- [vfs_umount](#vfs_umount) (function)
- [vfs_open](#vfs_open) (function)
- [vfs_open_mode](#vfs_open_mode) (function)
- [vfs_close](#vfs_close) (function)
- [vfs_dup](#vfs_dup) (function)
- [vfs_pread](#vfs_pread) (function)
- [vfs_read](#vfs_read) (function)
- [vfs_pwrite](#vfs_pwrite) (function)
- [vfs_write](#vfs_write) (function)
- [vfs_lseek](#vfs_lseek) (function)
- [vfs_fstat](#vfs_fstat) (function)
- [vfs_readdir_fd](#vfs_readdir_fd) (function)
- [vfs_stat](#vfs_stat) (function)
- [vfs_lstat](#vfs_lstat) (function)
- [vfs_readdir](#vfs_readdir) (function)
- [vfs_list](#vfs_list) (function)
- [vfs_mkdir](#vfs_mkdir) (function)
- [vfs_mkdir_p](#vfs_mkdir_p) (function)
- [vfs_unlink](#vfs_unlink) (function)
- [vfs_rmdir](#vfs_rmdir) (function)
- [vfs_rename](#vfs_rename) (function)
- [vfs_symlink](#vfs_symlink) (function)
- [vfs_readlink](#vfs_readlink) (function)
- [vfs_truncate](#vfs_truncate) (function)
- [vfs_chdir](#vfs_chdir) (function)
- [vfs_getcwd](#vfs_getcwd) (function)
- [vfs_read_file](#vfs_read_file) (function)
- [vfs_write_file](#vfs_write_file) (function)
- [vfs_errno_name](#vfs_errno_name) (function)
- [BlockDev](#BlockDev) (struct)
- [blockdev_new](#blockdev_new) (function)
- [blk_read](#blk_read) (function)
- [blk_write](#blk_write) (function)
- [blk_zero](#blk_zero) (function)
- [blk_read_bytes](#blk_read_bytes) (function)
- [blk_write_bytes](#blk_write_bytes) (function)
- [memdisk_read](#memdisk_read) (function)
- [memdisk_write](#memdisk_write) (function)
- [memdisk_zero](#memdisk_zero) (function)
- [memdisk_new](#memdisk_new) (function)
- [memdisk_from_bytes](#memdisk_from_bytes) (function)
- [memdisk_used_blocks](#memdisk_used_blocks) (function)

## VFS_EPERM:

```fides
pub const VFS_EPERM: i64 = 1;
```

*Defined at line 36*

---

## VFS_ENOENT:

```fides
pub const VFS_ENOENT: i64 = 2;
```

*Defined at line 37*

---

## VFS_EIO:

```fides
pub const VFS_EIO: i64 = 5;
```

*Defined at line 38*

---

## VFS_EBADF:

```fides
pub const VFS_EBADF: i64 = 9;
```

*Defined at line 39*

---

## VFS_EACCES:

```fides
pub const VFS_EACCES: i64 = 13;
```

*Defined at line 40*

---

## VFS_EBUSY:

```fides
pub const VFS_EBUSY: i64 = 16;
```

*Defined at line 41*

---

## VFS_EEXIST:

```fides
pub const VFS_EEXIST: i64 = 17;
```

*Defined at line 42*

---

## VFS_EXDEV:

```fides
pub const VFS_EXDEV: i64 = 18;
```

*Defined at line 43*

---

## VFS_ENOTDIR:

```fides
pub const VFS_ENOTDIR: i64 = 20;
```

*Defined at line 44*

---

## VFS_EISDIR:

```fides
pub const VFS_EISDIR: i64 = 21;
```

*Defined at line 45*

---

## VFS_EINVAL:

```fides
pub const VFS_EINVAL: i64 = 22;
```

*Defined at line 46*

---

## VFS_EMFILE:

```fides
pub const VFS_EMFILE: i64 = 24;
```

*Defined at line 47*

---

## VFS_EFBIG:

```fides
pub const VFS_EFBIG: i64 = 27;
```

*Defined at line 48*

---

## VFS_ENOSPC:

```fides
pub const VFS_ENOSPC: i64 = 28;
```

*Defined at line 49*

---

## VFS_EROFS:

```fides
pub const VFS_EROFS: i64 = 30;
```

*Defined at line 50*

---

## VFS_ENAMETOOLONG:

```fides
pub const VFS_ENAMETOOLONG: i64 = 36;
```

*Defined at line 51*

---

## VFS_ENOSYS:

```fides
pub const VFS_ENOSYS: i64 = 38;
```

*Defined at line 52*

---

## VFS_ENOTEMPTY:

```fides
pub const VFS_ENOTEMPTY: i64 = 39;
```

*Defined at line 53*

---

## VFS_ELOOP:

```fides
pub const VFS_ELOOP: i64 = 40;
```

*Defined at line 54*

---

## VFS_REG:

```fides
pub const VFS_REG: i64 = 1;
```

*Defined at line 58*

---

## VFS_DIR:

```fides
pub const VFS_DIR: i64 = 2;
```

*Defined at line 59*

---

## VFS_LNK:

```fides
pub const VFS_LNK: i64 = 3;
```

*Defined at line 60*

---

## VFS_S_IFMT:

```fides
pub const VFS_S_IFMT: i64 = 61440;
```

*Defined at line 62*

---

## VFS_S_IFREG:

```fides
pub const VFS_S_IFREG: i64 = 32768;
```

*Defined at line 63*

---

## VFS_S_IFDIR:

```fides
pub const VFS_S_IFDIR: i64 = 16384;
```

*Defined at line 64*

---

## VFS_S_IFLNK:

```fides
pub const VFS_S_IFLNK: i64 = 40960;
```

*Defined at line 65*

---

## VFS_O_RDONLY:

```fides
pub const VFS_O_RDONLY: i64 = 0;
```

*Defined at line 69*

---

## VFS_O_WRONLY:

```fides
pub const VFS_O_WRONLY: i64 = 1;
```

*Defined at line 70*

---

## VFS_O_RDWR:

```fides
pub const VFS_O_RDWR: i64 = 2;
```

*Defined at line 71*

---

## VFS_O_ACCMODE:

```fides
pub const VFS_O_ACCMODE: i64 = 3;
```

*Defined at line 72*

---

## VFS_O_CREAT:

```fides
pub const VFS_O_CREAT: i64 = 64;
```

*Defined at line 73*

---

## VFS_O_EXCL:

```fides
pub const VFS_O_EXCL: i64 = 128;
```

*Defined at line 74*

---

## VFS_O_TRUNC:

```fides
pub const VFS_O_TRUNC: i64 = 512;
```

*Defined at line 75*

---

## VFS_O_APPEND:

```fides
pub const VFS_O_APPEND: i64 = 1024;
```

*Defined at line 76*

---

## VFS_O_DIRECTORY:

```fides
pub const VFS_O_DIRECTORY: i64 = 65536;
```

*Defined at line 77*

---

## VFS_O_NOFOLLOW:

```fides
pub const VFS_O_NOFOLLOW: i64 = 131072;
```

*Defined at line 78*

---

## VFS_SEEK_SET:

```fides
pub const VFS_SEEK_SET: i64 = 0;
```

*Defined at line 80*

---

## VFS_SEEK_CUR:

```fides
pub const VFS_SEEK_CUR: i64 = 1;
```

*Defined at line 81*

---

## VFS_SEEK_END:

```fides
pub const VFS_SEEK_END: i64 = 2;
```

*Defined at line 82*

---

## VFS_MAX_FDS:

```fides
pub const VFS_MAX_FDS: i64 = 256;
```

*Defined at line 84*

---

## VFS_MAX_LINKS:

```fides
pub const VFS_MAX_LINKS: i64 = 40;
```

*Defined at line 85*

---

## VFS_NAME_MAX:

```fides
pub const VFS_NAME_MAX: i64 = 255;
```

*Defined at line 86*

---

## VFS_ST_ERR:

```fides
pub const VFS_ST_ERR: i64 = 0;
```

*Defined at line 89*

---

## VFS_ST_KIND:

```fides
pub const VFS_ST_KIND: i64 = 1;
```

*Defined at line 90*

---

## VFS_ST_SIZE:

```fides
pub const VFS_ST_SIZE: i64 = 2;
```

*Defined at line 91*

---

## VFS_ST_NLINK:

```fides
pub const VFS_ST_NLINK: i64 = 3;
```

*Defined at line 92*

---

## VFS_ST_MODE:

```fides
pub const VFS_ST_MODE: i64 = 4;
```

*Defined at line 93*

---

## VFS_ST_MTIME:

```fides
pub const VFS_ST_MTIME: i64 = 5;
```

*Defined at line 94*

---

## FsOps

One file system type's operations. `fs` is the instance handle the
file system gave out; inode numbers are the file system's own.
Results are >= 0 on success, -errno on failure.

```fides
pub struct FsOps {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `root` | `fn(i64) -> i64` |
| `lookup` | `fn(i64, i64, str) -> i64` |
| `getattr` | `fn(i64, i64) -> Vec<i64>` |
| `read` | `fn(i64, i64, i64, i64) -> Vec<i64>` |
| `write` | `fn(i64, i64, i64, Vec<i64>) -> i64` |
| `truncate` | `fn(i64, i64, i64) -> i64` |
| `create` | `fn(i64, i64, str, i64) -> i64` |
| `mkdir` | `fn(i64, i64, str, i64) -> i64` |
| `unlink` | `fn(i64, i64, str) -> i64` |
| `rmdir` | `fn(i64, i64, str) -> i64` |
| `rename` | `fn(i64, i64, str, i64, str) -> i64` |
| `readdir` | `fn(i64, i64, i64) -> str` |
| `symlink` | `fn(i64, i64, str, str) -> i64` |
| `readlink` | `fn(i64, i64) -> str` |

*Defined at line 101*

---

## vfs_ops_readonly

Operations of a read-only file system: the given read side, EROFS for
everything that would change it.

```fides
pub fn vfs_ops_readonly(name: str, root: fn(i64) -> i64, lookup: fn(i64, i64, str) -> i64, getattr: fn(i64, i64) -> Vec<i64>, read: fn(i64, i64, i64, i64) -> Vec<i64>, readdir: fn(i64, i64, i64) -> str, readlink: fn(i64, i64) -> str) -> FsOps {
```

**Parameters:**

- `name`: `str`
- `root`: `fn(i64) -> i64`
- `lookup`: `fn(i64`
- `getattr`: `fn(i64`
- `read`: `fn(i64`
- `readdir`: `fn(i64`
- `readlink`: `fn(i64`

**Returns:** `FsOps`

*Defined at line 145*

---

## VStat

```fides
pub struct VStat {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `i64` |
| `dev` | `i64` |
| `ino` | `i64` |
| `kind` | `i64` |
| `size` | `i64` |
| `nlink` | `i64` |
| `mode` | `i64` |
| `mtime` | `i64` |

*Defined at line 153*

---

## VDirent

```fides
pub struct VDirent {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ino` | `i64` |
| `kind` | `i64` |

*Defined at line 164*

---

## VMount

```fides
pub struct VMount {
```

**Fields:**

| Name | Type |
|------|------|
| `used` | `bool` |
| `path` | `str` |
| `ops` | `FsOps` |
| `fs` | `i64` |
| `root` | `i64` |
| `parent` | `i64` |
| `covered` | `i64` |

*Defined at line 170*

---

## VFile

```fides
pub struct VFile {
```

**Fields:**

| Name | Type |
|------|------|
| `used` | `bool` |
| `m` | `i64` |
| `ino` | `i64` |
| `off` | `i64` |
| `flags` | `i64` |
| `kind` | `i64` |

*Defined at line 180*

---

## Vfs

```fides
pub struct Vfs {
```

**Fields:**

| Name | Type |
|------|------|
| `mounts` | `Vec<VMount>` |
| `files` | `Vec<VFile>` |
| `cwd_m` | `Vec<i64>` |
| `cwd_i` | `Vec<i64>` |
| `cwd_n` | `Vec<str>` |
| `dcache` | `HashMap<str` |
| `dc_hits` | `i64` |
| `dc_misses` | `i64` |
| `err` | `i64` |

*Defined at line 189*

---

## VWalk

The result of a path walk: the inode reached (or, for a parent walk,
the parent directory and the last name), and the dentry stack.

```fides
pub struct VWalk {
```

**Fields:**

| Name | Type |
|------|------|
| `err` | `i64` |
| `m` | `i64` |
| `ino` | `i64` |
| `name` | `str` |
| `sm` | `Vec<i64>` |
| `si` | `Vec<i64>` |
| `sn` | `Vec<str>` |

*Defined at line 204*

---

## vfs_new

```fides
pub fn vfs_new() -> Vfs {
```

**Returns:** `Vfs`

*Defined at line 214*

---

## vfs_getattr

getattr through the mount's operations, as a VStat.

```fides
pub fn vfs_getattr(v: &Vfs, m: i64, ino: i64) -> VStat {
```

**Parameters:**

- `v`: `&Vfs`
- `m`: `i64`
- `ino`: `i64`

**Returns:** `VStat`

*Defined at line 229*

---

## vfs_walk

Walk `path`. With `parent` the last component is not looked up: the
result is its directory and `name`. `follow` follows a symlink in the
last component.

```fides
pub fn vfs_walk(v: &mut Vfs, path: str, follow: bool, parent: bool) -> VWalk {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `follow`: `bool`
- `parent`: `bool`

**Returns:** `VWalk`

*Defined at line 308*

---

## vfs_mount

Mount file system `fs` (operations `ops`) on `path`. The first mount
must be "/". Returns the mount index or -errno.

```fides
pub fn vfs_mount(v: &mut Vfs, path: str, ops: FsOps, fs: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `ops`: `FsOps`
- `fs`: `i64`

**Returns:** `i64`

*Defined at line 432*

---

## vfs_umount

Unmount the file system mounted at `path`: EBUSY while a file of it is
open or another mount sits on it.

```fides
pub fn vfs_umount(v: &mut Vfs, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 469*

---

## vfs_open

open(2), creating with mode 0644. Returns a descriptor or -errno.

```fides
pub fn vfs_open(v: &mut Vfs, path: str, flags: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 537*

---

## vfs_open_mode

open(2) with the permission bits a new file gets.

```fides
pub fn vfs_open_mode(v: &mut Vfs, path: str, flags: i64, mode: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `flags`: `i64`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 542*

---

## vfs_close

```fides
pub fn vfs_close(v: &mut Vfs, fd: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`

**Returns:** `i64`

*Defined at line 608*

---

## vfs_dup

Duplicate a descriptor onto the lowest free one (dup(2)); the two share
nothing afterwards except the inode (this table has no shared offsets).

```fides
pub fn vfs_dup(v: &mut Vfs, fd: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`

**Returns:** `i64`

*Defined at line 620*

---

## vfs_pread

pread(2): `n` bytes at `off`, offset unchanged. Sets v.err.

```fides
pub fn vfs_pread(v: &mut Vfs, fd: i64, off: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`
- `off`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 635*

---

## vfs_read

read(2): up to `n` bytes at the descriptor's offset, which advances.

```fides
pub fn vfs_read(v: &mut Vfs, fd: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 661*

---

## vfs_pwrite

pwrite(2). Returns bytes written or -errno.

```fides
pub fn vfs_pwrite(v: &mut Vfs, fd: i64, off: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`
- `off`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 677*

---

## vfs_write

write(2): at the offset (at the end with O_APPEND); the offset advances.

```fides
pub fn vfs_write(v: &mut Vfs, fd: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 694*

---

## vfs_lseek

lseek(2). Seeking past the end is allowed (a later write leaves a hole
of zeros). Returns the new offset or -errno.

```fides
pub fn vfs_lseek(v: &mut Vfs, fd: i64, off: i64, whence: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`
- `off`: `i64`
- `whence`: `i64`

**Returns:** `i64`

*Defined at line 714*

---

## vfs_fstat

```fides
pub fn vfs_fstat(v: &mut Vfs, fd: i64) -> VStat {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`

**Returns:** `VStat`

*Defined at line 736*

---

## vfs_readdir_fd

The next entry of an open directory (getdents): ino -1 at the end.
The descriptor's offset is the entry index.

```fides
pub fn vfs_readdir_fd(v: &mut Vfs, fd: i64) -> VDirent {
```

**Parameters:**

- `v`: `&mut Vfs`
- `fd`: `i64`

**Returns:** `VDirent`

*Defined at line 748*

---

## vfs_stat

```fides
pub fn vfs_stat(v: &mut Vfs, path: str) -> VStat {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `VStat`

*Defined at line 788*

---

## vfs_lstat

stat without following a symlink in the last component.

```fides
pub fn vfs_lstat(v: &mut Vfs, path: str) -> VStat {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `VStat`

*Defined at line 797*

---

## vfs_readdir

The entries of a directory (no "." or ".."), in the file system's
order. Sets v.err.

```fides
pub fn vfs_readdir(v: &mut Vfs, path: str) -> Vec<VDirent> {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `Vec<VDirent>`

*Defined at line 807*

---

## vfs_list

The names of a directory's entries.

```fides
pub fn vfs_list(v: &mut Vfs, path: str) -> Vec<str> {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 828*

---

## vfs_mkdir

```fides
pub fn vfs_mkdir(v: &mut Vfs, path: str, mode: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 839*

---

## vfs_mkdir_p

mkdir -p: every missing directory of `path`. Returns 0 or -errno.

```fides
pub fn vfs_mkdir_p(v: &mut Vfs, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 860*

---

## vfs_unlink

```fides
pub fn vfs_unlink(v: &mut Vfs, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 889*

---

## vfs_rmdir

```fides
pub fn vfs_rmdir(v: &mut Vfs, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 908*

---

## vfs_rename

rename(2): within one mount (EXDEV across mounts); replaces a file, or
an empty directory with a directory.

```fides
pub fn vfs_rename(v: &mut Vfs, from: str, to: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `from`: `str`
- `to`: `str`

**Returns:** `i64`

*Defined at line 932*

---

## vfs_symlink

symlink(2): a link at `path` whose target is `target` (not checked).

```fides
pub fn vfs_symlink(v: &mut Vfs, target: str, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `target`: `str`
- `path`: `str`

**Returns:** `i64`

*Defined at line 966*

---

## vfs_readlink

readlink(2): the target, "" with v.err set when `path` is no symlink.

```fides
pub fn vfs_readlink(v: &mut Vfs, path: str) -> str {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `str`

*Defined at line 984*

---

## vfs_truncate

```fides
pub fn vfs_truncate(v: &mut Vfs, path: str, size: i64) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 1000*

---

## vfs_chdir

```fides
pub fn vfs_chdir(v: &mut Vfs, path: str) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `i64`

*Defined at line 1017*

---

## vfs_getcwd

The current directory as the path walked to reach it (symlinks
resolved, as getcwd(3) reports it).

```fides
pub fn vfs_getcwd(v: &Vfs) -> str {
```

**Parameters:**

- `v`: `&Vfs`

**Returns:** `str`

*Defined at line 1033*

---

## vfs_read_file

Read a whole file. Sets v.err.

```fides
pub fn vfs_read_file(v: &mut Vfs, path: str) -> Vec<i64> {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1047*

---

## vfs_write_file

Create or replace a whole file. Returns bytes written or -errno.

```fides
pub fn vfs_write_file(v: &mut Vfs, path: str, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `&mut Vfs`
- `path`: `str`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1062*

---

## vfs_errno_name

```fides
pub fn vfs_errno_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 1072*

---

## BlockDev

A block device: `read(handle, lba)` gives one block (block_size bytes,
empty on error), `write(handle, lba, bytes)` stores one (0 or -errno),
`zero(handle, lba, count)` zeroes a range (WRITE ZEROES / discard).

```fides
pub struct BlockDev {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `block_size` | `i64` |
| `nblocks` | `i64` |
| `read` | `fn(i64, i64) -> Vec<i64>` |
| `write` | `fn(i64, i64, Vec<i64>) -> i64` |
| `zero` | `fn(i64, i64, i64) -> i64` |

*Defined at line 1145*

---

## blockdev_new

A BlockDev from a driver's functions (for modules that import vfs).

```fides
pub fn blockdev_new(handle: i64, block_size: i64, nblocks: i64, read: fn(i64, i64) -> Vec<i64>, write: fn(i64, i64, Vec<i64>) -> i64, zero: fn(i64, i64, i64) -> i64) -> BlockDev {
```

**Parameters:**

- `handle`: `i64`
- `block_size`: `i64`
- `nblocks`: `i64`
- `read`: `fn(i64`
- `write`: `fn(i64`
- `zero`: `fn(i64`

**Returns:** `BlockDev`

*Defined at line 1155*

---

## blk_read

```fides
pub fn blk_read(d: &BlockDev, lba: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&BlockDev`
- `lba`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1161*

---

## blk_write

```fides
pub fn blk_write(d: &BlockDev, lba: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `d`: `&BlockDev`
- `lba`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1169*

---

## blk_zero

```fides
pub fn blk_zero(d: &BlockDev, lba: i64, count: i64) -> i64 {
```

**Parameters:**

- `d`: `&BlockDev`
- `lba`: `i64`
- `count`: `i64`

**Returns:** `i64`

*Defined at line 1177*

---

## blk_read_bytes

`len` bytes from byte offset `off` (any alignment).

```fides
pub fn blk_read_bytes(d: &BlockDev, off: i64, len: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&BlockDev`
- `off`: `i64`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1186*

---

## blk_write_bytes

Write `data` at byte offset `off` (read-modify-write of partial blocks).

```fides
pub fn blk_write_bytes(d: &BlockDev, off: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `d`: `&BlockDev`
- `off`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1207*

---

## memdisk_read

```fides
pub fn memdisk_read(h: i64, lba: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `lba`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1255*

---

## memdisk_write

```fides
pub fn memdisk_write(h: i64, lba: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `lba`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1267*

---

## memdisk_zero

```fides
pub fn memdisk_zero(h: i64, lba: i64, count: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `lba`: `i64`
- `count`: `i64`

**Returns:** `i64`

*Defined at line 1282*

---

## memdisk_new

A new zero-filled RAM disk of `nblocks` blocks of `bsize` bytes.

```fides
pub fn memdisk_new(bsize: i64, nblocks: i64) -> BlockDev {
```

**Parameters:**

- `bsize`: `i64`
- `nblocks`: `i64`

**Returns:** `BlockDev`

*Defined at line 1297*

---

## memdisk_from_bytes

A RAM disk holding `bytes` (padded with zeros to whole blocks); blocks
that are all zero are not stored.

```fides
pub fn memdisk_from_bytes(bytes: Vec<i64>, bsize: i64) -> BlockDev {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `bsize`: `i64`

**Returns:** `BlockDev`

*Defined at line 1310*

---

## memdisk_used_blocks

How many blocks of the disk are stored (written and not zeroed).

```fides
pub fn memdisk_used_blocks(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1339*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
