[All modules](index.md)

# file -- Fides Documentation

## Contents

- [file_read](#file_read) (function)
- [file_read_result](#file_read_result) (function)
- [file_write](#file_write) (function)
- [file_append](#file_append) (function)
- [file_exists](#file_exists) (function)
- [file_write_bytes](#file_write_bytes) (function)
- [file_read_bytes](#file_read_bytes) (function)
- [file_write_raw](#file_write_raw) (function)
- [file_delete](#file_delete) (function)
- [file_size](#file_size) (function)
- [fs_read_text](#fs_read_text) (function)
- [fs_write_text](#fs_write_text) (function)
- [fs_append_text](#fs_append_text) (function)
- [fs_exists](#fs_exists) (function)
- [fs_delete](#fs_delete) (function)
- [fs_size](#fs_size) (function)

## file_read

Read the entire contents of a file as a string.
Returns empty string if the file doesn't exist or can't be read.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val p = "fid_doctest_file.tmp";
file_write(p, "one\n");
file_append(p, "two\n");
assert(file_exists(p));
assert_eq(file_read(p), "one\ntwo\n");
assert_eq(file_size(p), 8);
file_delete(p);
assert(!file_exists(p));
assert_eq(file_read(p), "");
```

```fides
pub fn file_read(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 24*

---

## file_read_result

Read the entire contents of a file: Err("cannot read <path>") when it
does not exist or cannot be read.

```fides
pub fn file_read_result(path: str) -> Result<str, str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Result<str, str>`

*Defined at line 41*

---

## file_write

Write content to a file (overwrites existing content).
Returns 0 on success, -1 on error.

```fides
pub fn file_write(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 57*

---

## file_append

Append content to a file (creates if it doesn't exist).
Returns 0 on success, -1 on error.

```fides
pub fn file_append(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 67*

---

## file_exists

Check if a file exists.

```fides
pub fn file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 76*

---

## file_write_bytes

Write raw bytes to a file (binary mode). The bytes are provided as a Vec
of byte values (0-255). Returns 0 on success, -1 on error.
Binary-safe: p_file_write_vec uses fwrite with an explicit length, so
bytes containing 0x00 are written correctly (known-bug 3.14 fix — routing
through str_from_bytes + p_file_write truncated at the first NUL).

```fides
pub fn file_write_bytes(path: str, bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `path`: `str`
- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 89*

---

## file_read_bytes

Read a file as a Vec of byte values. Returns an empty Vec if the file
doesn't exist or can't be read.
Binary-safe: p_file_read_bytes preserves bytes past a 0x00 (unlike
p_file_read + str_bytes, which truncate at the first NUL).

```fides
pub fn file_read_bytes(path: str) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 97*

---

## file_write_raw

Write a raw string to a file in binary mode (no newline translation).
On the C backend this uses fopen("wb"). On the interpreter it uses
std::fs::write which is already binary-safe.

```fides
pub fn file_write_raw(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 108*

---

## file_delete

Delete a file. Returns 0 on success, -1 on error.

```fides
pub fn file_delete(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 113*

---

## file_size

Get the size of a file in bytes, or -1 if it doesn't exist.

```fides
pub fn file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 118*

---

## fs_read_text

Alias for fs_read_text — read file contents as a string.

```fides
pub fn fs_read_text(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 126*

---

## fs_write_text

Alias for fs_write_text — write string to a file.

```fides
pub fn fs_write_text(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 131*

---

## fs_append_text

Alias for fs_append_text — append string to a file.

```fides
pub fn fs_append_text(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 136*

---

## fs_exists

Alias for fs_exists — check if a file exists.

```fides
pub fn fs_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 141*

---

## fs_delete

Alias for fs_delete — delete a file.

```fides
pub fn fs_delete(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 146*

---

## fs_size

Alias for fs_size — get file size in bytes.

```fides
pub fn fs_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 151*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
