[All modules](index.md)

# fs -- Fides Documentation

## Contents

- [fs_exists](#fs_exists) (function)
- [fs_file_size](#fs_file_size) (function)
- [fs_read_text](#fs_read_text) (function)
- [fs_read_bytes](#fs_read_bytes) (function)
- [fs_read_lines](#fs_read_lines) (function)
- [fs_write_text](#fs_write_text) (function)
- [fs_write_bytes](#fs_write_bytes) (function)
- [fs_append_text](#fs_append_text) (function)
- [fs_copy_file](#fs_copy_file) (function)
- [p_fs_copy_file](#p_fs_copy_file) (function)
- [fs_delete](#fs_delete) (function)
- [fs_rename](#fs_rename) (function)
- [fs_list_dir](#fs_list_dir) (function)
- [fs_is_dir](#fs_is_dir) (function)
- [fs_is_file](#fs_is_file) (function)
- [fs_mkdir](#fs_mkdir) (function)
- [fs_mkdir_all](#fs_mkdir_all) (function)
- [fs_rmdir](#fs_rmdir) (function)
- [fs_rmdir_all](#fs_rmdir_all) (function)
- [fs_walk](#fs_walk) (function)
- [fs_walk_files](#fs_walk_files) (function)
- [fs_glob_match](#fs_glob_match) (function)
- [fs_glob](#fs_glob) (function)
- [fs_temp_path](#fs_temp_path) (function)
- [fs_temp_file](#fs_temp_file) (function)
- [fs_temp_dir](#fs_temp_dir) (function)
- [fs_join](#fs_join) (function)
- [fs_basename](#fs_basename) (function)
- [fs_dirname](#fs_dirname) (function)
- [fs_ext](#fs_ext) (function)
- [fs_stem](#fs_stem) (function)
- [fs_path_in](#fs_path_in) (function)
- [file_read](#file_read) (function)
- [file_write](#file_write) (function)
- [file_append](#file_append) (function)
- [file_exists](#file_exists) (function)
- [file_delete](#file_delete) (function)
- [file_size](#file_size) (function)
- [FILE_BUF_LIMIT:](#FILE_BUF_LIMIT:) (const)
- [FILE_READ_CHUNK:](#FILE_READ_CHUNK:) (const)
- [FILE_CLOSED:](#FILE_CLOSED:) (const)
- [FILE_READ:](#FILE_READ:) (const)
- [FILE_WRITE:](#FILE_WRITE:) (const)
- [File](#File) (struct)
- [File](#File) (impl)
- [fs_modified_ms](#fs_modified_ms) (function)
- [FileView](#FileView) (struct)
- [p_fs_map](#p_fs_map) (function)
- [p_fs_unmap](#p_fs_unmap) (function)
- [FileView](#FileView) (impl)

## fs_exists

Check if a file exists.

```fides
pub fn fs_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 24*

---

## fs_file_size

Get file size in bytes. Returns -1 if file doesn't exist.

```fides
pub fn fs_file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 32*

---

## fs_read_text

Read a file as text. Returns "" if file doesn't exist.

```fides
pub fn fs_read_text(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 45*

---

## fs_read_bytes

Read a file as a Vec of bytes. Returns empty Vec if file doesn't exist.

```fides
pub fn fs_read_bytes(path: str) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 73*

---

## fs_read_lines

Read a file as lines. Returns empty Vec if file doesn't exist.

```fides
pub fn fs_read_lines(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 93*

---

## fs_write_text

Write text to a file. Overwrites if exists. Returns true on success.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val p = "fid_doctest_fs.tmp";
assert(fs_write_text(p, "a\nb"));
assert(fs_exists(p));
assert(fs_is_file(p));
assert_eq(fs_read_lines(p).len(), 2);
assert(fs_append_text(p, "\nc"));
assert_eq(fs_read_text(p), "a\nb\nc");
assert_eq(fs_file_size(p), 5);
assert(fs_delete(p));
assert(!fs_exists(p));
```

```fides
pub fn fs_write_text(path: str, content: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `bool`

*Defined at line 116*

---

## fs_write_bytes

Write bytes to a file. Overwrites if exists. Returns true on success.

```fides
pub fn fs_write_bytes(path: str, bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `path`: `str`
- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 128*

---

## fs_append_text

Append text to a file. Creates if doesn't exist. Returns true on success.

```fides
pub fn fs_append_text(path: str, content: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `bool`

*Defined at line 140*

---

## fs_copy_file

Copy the file `src` to `dst` (created, or emptied first): the number of
bytes copied, or -1 when `src` cannot be read or `dst` written. A file
copied onto itself is left as it is.

Zero-copy where the system allows it: the kernel moves the bytes
without passing them through the program -- copy_file_range, then
sendfile, on Linux (C builds and native builds), fcopyfile on macOS and
CopyFileA on Windows (C builds). Where it cannot (`fid run`, the Windows
native runtime, a file system without those calls) the bytes are read
and written instead, with the same result.

```fides
pub fn fs_copy_file(src: str, dst: str) -> i64 {
```

**Parameters:**

- `src`: `str`
- `dst`: `str`

**Returns:** `i64`

*Defined at line 163*

---

## p_fs_copy_file

fs_copy_file's portable body: what `fid run` and a back end without a
runtime version run (read every byte, write them). The C back end runs
its runtime's __p_zc_copy_file instead (compiler/codegen_c.fd
fast_zero_copy_body) and native Linux pert_fs_copy_file
(stdlib/linux_runtime/sys.fd).

```fides
pub fn p_fs_copy_file(src: str, dst: str) -> i64 {
```

**Parameters:**

- `src`: `str`
- `dst`: `str`

**Returns:** `i64`

*Defined at line 191*

---

## fs_delete

Delete a file. Returns true on success.

```fides
pub fn fs_delete(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 208*

---

## fs_rename

Rename/move a file. Returns true on success.

```fides
pub fn fs_rename(old_path: str, new_path: str) -> bool {
```

**Parameters:**

- `old_path`: `str`
- `new_path`: `str`

**Returns:** `bool`

*Defined at line 216*

---

## fs_list_dir

List entries in a directory (names only, no "." or "..").
Returns empty Vec if directory doesn't exist or can't be opened.
(Under a fake file system the names are sorted.)

```fides
pub fn fs_list_dir(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 228*

---

## fs_is_dir

Check if a path is a directory.

```fides
pub fn fs_is_dir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 240*

---

## fs_is_file

Check if a path is a file (not a directory).

```fides
pub fn fs_is_file(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 248*

---

## fs_mkdir

Create a single directory. Returns true on success (false if it already
exists or the parent is missing — use fs_mkdir_all for the recursive
form).

```fides
pub fn fs_mkdir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 257*

---

## fs_mkdir_all

Create a directory and all missing parents ("mkdir -p").
Returns true if the directory exists afterwards.

```fides
pub fn fs_mkdir_all(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 266*

---

## fs_rmdir

Remove an empty directory. Returns true on success.

```fides
pub fn fs_rmdir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 303*

---

## fs_rmdir_all

Remove a directory and everything inside it ("rm -rf").
Returns true when the path no longer exists afterwards.

```fides
pub fn fs_rmdir_all(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 312*

---

## fs_walk

Recursively list every file and directory under `dir` (depth-first,
full paths relative to `dir` as given). Empty Vec when `dir` is not a
readable directory.

```fides
pub fn fs_walk(dir: str) -> Vec<str> {
```

**Parameters:**

- `dir`: `str`

**Returns:** `Vec<str>`

*Defined at line 357*

---

## fs_walk_files

Like fs_walk but reports only files, not directories.

```fides
pub fn fs_walk_files(dir: str) -> Vec<str> {
```

**Parameters:**

- `dir`: `str`

**Returns:** `Vec<str>`

*Defined at line 367*

---

## fs_glob_match

Does `name` match a single-segment glob pattern (`*`, `?`)?
This is the same wildcard matcher as regex_match.

```fides
pub fn fs_glob_match(name: str, pattern: str) -> bool {
```

**Parameters:**

- `name`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 387*

---

## fs_glob

Match `pattern` against the directory tree under `dir`.
Returns matching paths as `dir`-relative full paths.
fs_glob("src", "*.fd")        — direct children ending in .fd
fs_glob("src", "**/*.fd")     — .fd files at any depth
fs_glob("src", "a/?/b.txt")   — single-char middle component

```fides
pub fn fs_glob(dir: str, pattern: str) -> Vec<str> {
```

**Parameters:**

- `dir`: `str`
- `pattern`: `str`

**Returns:** `Vec<str>`

*Defined at line 433*

---

## fs_temp_path

Return a fresh temp-file path in `dir` (created on first write).
The path does not exist yet — nothing is created.

```fides
pub fn fs_temp_path(dir: str, prefix: str) -> str {
```

**Parameters:**

- `dir`: `str`
- `prefix`: `str`

**Returns:** `str`

*Defined at line 461*

---

## fs_temp_file

Create an empty temp file in `dir` and return its path, or "" on
failure. `prefix` becomes the first characters of the file name.

```fides
pub fn fs_temp_file(dir: str, prefix: str) -> str {
```

**Parameters:**

- `dir`: `str`
- `prefix`: `str`

**Returns:** `str`

*Defined at line 467*

---

## fs_temp_dir

Create a temp directory in `dir` and return its path, or "" on failure.

```fides
pub fn fs_temp_dir(dir: str, prefix: str) -> str {
```

**Parameters:**

- `dir`: `str`
- `prefix`: `str`

**Returns:** `str`

*Defined at line 483*

---

## fs_join

Join two path components (handles "/", also treats "\" as separator on
the right-hand side for Windows callers).

```fides
pub fn fs_join(a: str, b: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `str`

*Defined at line 507*

---

## fs_basename

The last path component (after '/' or '\'); "" for a trailing sep.

```fides
pub fn fs_basename(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 528*

---

## fs_dirname

Everything before the last '/' or '\'; "" when there is none.

```fides
pub fn fs_dirname(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 542*

---

## fs_ext

The extension including the dot ("a.tar.gz" -> ".gz"); "" when none.

```fides
pub fn fs_ext(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 555*

---

## fs_stem

Path without its final extension.

```fides
pub fn fs_stem(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 572*

---

## fs_path_in

Get the full path of a file in a directory.

```fides
pub fn fs_path_in(dir: str, filename: str) -> str {
```

**Parameters:**

- `dir`: `str`
- `filename`: `str`

**Returns:** `str`

*Defined at line 578*

---

## file_read

Alias for file_read — read file contents as a string.

```fides
pub fn file_read(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 586*

---

## file_write

Alias for file_write — write string to a file. Returns 0 on success, -1 on error.

```fides
pub fn file_write(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 591*

---

## file_append

Alias for file_append — append string to a file. Returns 0 on success, -1 on error.

```fides
pub fn file_append(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 599*

---

## file_exists

Alias for file_exists — check if a file exists.

```fides
pub fn file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 607*

---

## file_delete

Alias for file_delete — delete a file. Returns 0 on success, -1 on error.

```fides
pub fn file_delete(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 612*

---

## file_size

Alias for file_size — get file size in bytes, or -1 if it doesn't exist.

```fides
pub fn file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 620*

---

## FILE_BUF_LIMIT:

Pending write bytes kept before a File writes them out.

```fides
pub const FILE_BUF_LIMIT: i64 = 65536;
```

*Defined at line 650*

---

## FILE_READ_CHUNK:

Bytes a read handle reads from the file at a time.

```fides
pub const FILE_READ_CHUNK: i64 = 65536;
```

*Defined at line 652*

---

## FILE_CLOSED:

File modes.

```fides
pub const FILE_CLOSED: i64 = 0;
```

*Defined at line 654*

---

## FILE_READ:

```fides
pub const FILE_READ: i64 = 1;
```

*Defined at line 655*

---

## FILE_WRITE:

```fides
pub const FILE_WRITE: i64 = 2;
```

*Defined at line 656*

---

## File

An open file: a read handle (`File::open`) or a write handle
(`File::create`, `File::append`).

```fides
pub struct File {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `mode` | `i64` |
| `data` | `Vec<i64>,  // read: the window (file bytes from buf_at); write: bytes not yet written` |
| `pos` | `i64,  // read position (an offset in the file)` |
| `written` | `i64,  // write: bytes handed to write() so far` |
| `failed` | `bool,  // a write to disk failed` |
| `limit` | `i64,  // write: pending bytes kept before a write-out` |
| `buf_at` | `i64,  // read: the file offset of data's first byte` |
| `fsize` | `i64,  // read: the file's size when it was opened` |
| `whole` | `bool,  // read: data is the whole file (a fake one)` |

*Defined at line 660*

---

## File

```fides
impl File {
```

**Methods:**

### open

Open an existing file for reading. None if it does not exist or
is a directory. Nothing is read yet.

```fides
pub fn open(path: str) -> Option<File> {
```

### create

Create (or truncate) a file for writing. None if it cannot be
created.

```fides
pub fn create(path: str) -> Option<File> {
```

### append

Open a file for writing at its end, creating it if missing.

```fides
pub fn append(path: str) -> Option<File> {
```

### is_open

True while the handle is open.

```fides
pub fn is_open(self: &File) -> bool {
```

### set_buffer_limit

Keep at most `n` pending write bytes (at least 1) before writing
them out; the default is FILE_BUF_LIMIT.

```fides
pub fn set_buffer_limit(self: &mut File, n: i64) -> void {
```

### pending

Pending write bytes not yet on disk.

```fides
pub fn pending(self: &File) -> i64 {
```

### fill

The window holds file byte `at`, read from the file when it did
not: false past the end of the file.

```fides
fn fill(self: &mut File, at: i64) -> bool {
```

### read

Up to `n` bytes from the current position; empty at the end of the
file (or on a write handle).

```fides
pub fn read(self: &mut File, n: i64) -> Vec<i64> {
```

### read_str

Up to `n` bytes as text.

```fides
pub fn read_str(self: &mut File, n: i64) -> str {
```

### read_byte

The next byte (0..255), or -1 at the end of the file.

```fides
pub fn read_byte(self: &mut File) -> i64 {
```

### read_line

The next line without its newline (and a carriage return before
it); None at the end of the file.

```fides
pub fn read_line(self: &mut File) -> Option<str> {
```

### read_to_end

Everything from the current position to the end.

```fides
pub fn read_to_end(self: &mut File) -> Vec<i64> {
```

### at_eof

True when a read handle has nothing left to read (by the size the
file had when it was opened).

```fides
pub fn at_eof(self: &File) -> bool {
```

### tell

The read position; on a write handle, the bytes written so far.

```fides
pub fn tell(self: &File) -> i64 {
```

### seek

Move the read position to `offset` (clamped to 0..size). False on a
write or closed handle. Nothing is read until the next read.

```fides
pub fn seek(self: &mut File, offset: i64) -> bool {
```

### size

The size of a read handle's file (when it was opened); on a write
handle, the bytes written so far.

```fides
pub fn size(self: &File) -> i64 {
```

### write

Write bytes (each 0..255). False on a read or closed handle, or once
a write to disk has failed.

```fides
pub fn write(self: &mut File, bytes: Vec<i64>) -> bool {
```

### write_str

Write the bytes of a string.

```fides
pub fn write_str(self: &mut File, s: str) -> bool {
```

### write_line

Write a string and a newline.

```fides
pub fn write_line(self: &mut File, s: str) -> bool {
```

### flush

Write the pending bytes to disk.

```fides
pub fn flush(self: &mut File) -> bool {
```

### close

Flush and close. True when every write reached the file. A closed
handle reads and writes nothing.

```fides
pub fn close(self: &mut File) -> bool {
```

*Defined at line 741*

---

## fs_modified_ms

The time `path` was last written, in ms since 1970 (UTC); -1 when it
does not exist. (A fake file under stdlib/world: 0.)

```fides
pub fn fs_modified_ms(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1013*

---

## FileView

A file opened for reading in place (`FileView::open`).

```fides
pub struct FileView {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `len` | `i64` |
| `addr` | `i64,  // where the file is mapped; 0 when its bytes are in `data`` |
| `data` | `Vec<i64>,  // the file's bytes when it is not mapped` |

*Defined at line 1042*

---

## p_fs_map

Map `size` bytes of the file `path` read-only: the address, or 0 when
it cannot be mapped (then FileView reads it). This portable body maps
nothing; the C back end runs its runtime's __p_zc_map instead
(compiler/codegen_c.fd fast_zero_copy_body) and native Linux
pert_fs_map (stdlib/linux_runtime/sys.fd). It makes a call, so that the
IR inliner keeps the call for the native runtime to take over.

```fides
pub fn p_fs_map(path: str, size: i64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 1055*

---

## p_fs_unmap

Undo p_fs_map: 0, or -1. Nothing is mapped by the portable body, so
reaching it with a mapping is a bug.

```fides
pub fn p_fs_unmap(addr: i64, _size: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `_size`: `i64`

**Returns:** `i64`

*Defined at line 1064*

---

## FileView

```fides
impl FileView {
```

**Methods:**

### open

Open the file `path` for reading in place: None when it does not
exist, is a folder or cannot be read.

```fides
pub fn open(path: str) -> Option<FileView> {
```

### is_mapped

Whether the file's pages are mapped (false: it was read instead).

```fides
pub fn is_mapped(self: &FileView) -> bool {
```

### byte

The byte at `i` (0..255), or -1 outside the file.

```fides
pub fn byte(self: &FileView, i: i64) -> i64 {
```

### slice

The bytes from `from` (inclusive) to `to` (exclusive) as a string,
both clamped to the file.

```fides
pub fn slice(self: &FileView, from: i64, to: i64) -> str {
```

### to_str

The whole file as a string.

```fides
pub fn to_str(self: &FileView) -> str {
```

### close

Unmap the file (or drop its bytes). A closed view reads nothing.

```fides
pub fn close(self: &mut FileView) -> void {
```

*Defined at line 1071*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
