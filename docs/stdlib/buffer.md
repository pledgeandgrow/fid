[All modules](index.md)

# buffer -- Fides Documentation

## Contents

- [ByteCursor](#ByteCursor) (struct)
- [ByteCursor](#ByteCursor) (impl)
- [buffer_new](#buffer_new) (function)
- [buffer_from_bytes](#buffer_from_bytes) (function)
- [buffer_from_string](#buffer_from_string) (function)

## ByteCursor

A byte buffer with separate read and write cursors.

```fides
pub struct ByteCursor {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `pos` | `i64,  // read cursor` |

*Defined at line 16*

---

## ByteCursor

```fides
impl ByteCursor {
```

**Methods:**

### new

Create a new empty buffer.

Example:
var b = ByteCursor::new();
b.write_u8(1);
b.write_u16_le(0x0302);
b.write_u32_be(0x04050607);
assert_eq(b.len(), 7);
assert_eq(b.read_u8(), 1);
assert_eq(b.read_u16_le(), 0x0302);
assert_eq(b.read_u32_be(), 0x04050607);
assert_eq(b.remaining(), 0);
b.rewind();
assert_eq(b.position(), 0);

```fides
pub fn new() -> ByteCursor {
```

### from_bytes

Create a buffer from an existing Vec of bytes.

```fides
pub fn from_bytes(bytes: Vec<i64>) -> ByteCursor {
```

### from_string

Create a buffer from a string (UTF-8 bytes).

Example:
var b = ByteCursor::from_string("hi");
assert_eq(b.len(), 2);
assert_eq(b.read_u8(), 104);
assert_eq(b.to_string(), "hi");

```fides
pub fn from_string(s: str) -> ByteCursor {
```

### len

Get the total length of the buffer.

```fides
pub fn len(self: ByteCursor) -> i64 {
```

### is_empty

Check if the buffer is empty.

```fides
pub fn is_empty(self: ByteCursor) -> bool {
```

### remaining

Get the number of remaining readable bytes.

```fides
pub fn remaining(self: ByteCursor) -> i64 {
```

### rewind

Reset the read cursor to the beginning.

```fides
pub fn rewind(self: &mut ByteCursor) -> void {
```

### clear

Clear the buffer and reset the cursor.

```fides
pub fn clear(self: &mut ByteCursor) -> void {
```

### to_bytes

Get the raw bytes as a Vec.

```fides
pub fn to_bytes(self: ByteCursor) -> Vec<i64> {
```

### to_string

Convert the buffer to a string (UTF-8).

```fides
pub fn to_string(self: ByteCursor) -> str {
```

### write_u8

Write a single byte.

```fides
pub fn write_u8(self: &mut ByteCursor, value: i64) -> void {
```

### write_u16_le

Write a 16-bit unsigned integer (little-endian).

```fides
pub fn write_u16_le(self: &mut ByteCursor, value: i64) -> void {
```

### write_u16_be

Write a 16-bit unsigned integer (big-endian).

```fides
pub fn write_u16_be(self: &mut ByteCursor, value: i64) -> void {
```

### write_u32_le

Write a 32-bit unsigned integer (little-endian).

```fides
pub fn write_u32_le(self: &mut ByteCursor, value: i64) -> void {
```

### write_u32_be

Write a 32-bit unsigned integer (big-endian).

```fides
pub fn write_u32_be(self: &mut ByteCursor, value: i64) -> void {
```

### write_u64_le

Write a 64-bit integer (little-endian).

```fides
pub fn write_u64_le(self: &mut ByteCursor, value: i64) -> void {
```

### write_u64_be

Write a 64-bit integer (big-endian).

```fides
pub fn write_u64_be(self: &mut ByteCursor, value: i64) -> void {
```

### write_bytes

Write a raw byte array.

```fides
pub fn write_bytes(self: &mut ByteCursor, bytes: Vec<i64>) -> void {
```

### write_string

Write a string as UTF-8 bytes.

```fides
pub fn write_string(self: &mut ByteCursor, s: str) -> void {
```

### write_string_lp

Write a length-prefixed string (u32 LE length + bytes).

```fides
pub fn write_string_lp(self: &mut ByteCursor, s: str) -> void {
```

### read_u8

Read a single byte. Returns -1 if at end.

```fides
pub fn read_u8(self: &mut ByteCursor) -> i64 {
```

### read_u16_le

Read a 16-bit unsigned integer (little-endian).

```fides
pub fn read_u16_le(self: &mut ByteCursor) -> i64 {
```

### read_u16_be

Read a 16-bit unsigned integer (big-endian).

```fides
pub fn read_u16_be(self: &mut ByteCursor) -> i64 {
```

### read_u32_le

Read a 32-bit unsigned integer (little-endian).

```fides
pub fn read_u32_le(self: &mut ByteCursor) -> i64 {
```

### read_u32_be

Read a 32-bit unsigned integer (big-endian).

```fides
pub fn read_u32_be(self: &mut ByteCursor) -> i64 {
```

### read_u64_le

Read a 64-bit integer (little-endian).

```fides
pub fn read_u64_le(self: &mut ByteCursor) -> i64 {
```

### read_u64_be

Read a 64-bit integer (big-endian).

```fides
pub fn read_u64_be(self: &mut ByteCursor) -> i64 {
```

### read_bytes

Read `n` bytes into a Vec.

```fides
pub fn read_bytes(self: &mut ByteCursor, n: i64) -> Vec<i64> {
```

### read_string_lp

Read a length-prefixed string (u32 LE length + bytes).

```fides
pub fn read_string_lp(self: ByteCursor) -> str {
```

### read_string_null

Read a null-terminated string.

```fides
pub fn read_string_null(self: &mut ByteCursor) -> str {
```

### read_remaining_string

Read the remaining bytes as a string.

```fides
pub fn read_remaining_string(self: ByteCursor) -> str {
```

### seek

Seek to an absolute position.

```fides
pub fn seek(self: &mut ByteCursor, pos: i64) -> void {
```

### skip

Skip `n` bytes.

```fides
pub fn skip(self: &mut ByteCursor, n: i64) -> void {
```

### position

Get the current read position.

```fides
pub fn position(self: ByteCursor) -> i64 {
```

*Defined at line 21*

---

## buffer_new

Create a new empty buffer (free function wrapper).

```fides
pub fn buffer_new() -> ByteCursor {
```

**Returns:** `ByteCursor`

*Defined at line 345*

---

## buffer_from_bytes

Create a buffer from an existing Vec of bytes.

```fides
pub fn buffer_from_bytes(bytes: Vec<i64>) -> ByteCursor {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `ByteCursor`

*Defined at line 352*

---

## buffer_from_string

Create a buffer from a string.

```fides
pub fn buffer_from_string(s: str) -> ByteCursor {
```

**Parameters:**

- `s`: `str`

**Returns:** `ByteCursor`

*Defined at line 359*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
