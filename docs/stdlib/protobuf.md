[All modules](index.md)

# protobuf -- Fides Documentation

## Contents

- [PB_VARINT:](#PB_VARINT:) (const)
- [PB_FIXED64:](#PB_FIXED64:) (const)
- [PB_LEN:](#PB_LEN:) (const)
- [PB_FIXED32:](#PB_FIXED32:) (const)
- [PbWriter](#PbWriter) (struct)
- [pb_writer_new](#pb_writer_new) (function)
- [pb_put_varint](#pb_put_varint) (function)
- [pb_zigzag](#pb_zigzag) (function)
- [pb_unzigzag](#pb_unzigzag) (function)
- [pb_write_varint](#pb_write_varint) (function)
- [pb_write_sint](#pb_write_sint) (function)
- [pb_write_bool](#pb_write_bool) (function)
- [pb_write_fixed64](#pb_write_fixed64) (function)
- [pb_write_fixed32](#pb_write_fixed32) (function)
- [pb_write_double](#pb_write_double) (function)
- [pb_write_bytes](#pb_write_bytes) (function)
- [pb_write_string](#pb_write_string) (function)
- [pb_write_message](#pb_write_message) (function)
- [pb_write_packed](#pb_write_packed) (function)
- [PbReader](#PbReader) (struct)
- [pb_reader_new](#pb_reader_new) (function)
- [pb_next](#pb_next) (function)
- [pb_read_varint](#pb_read_varint) (function)
- [pb_read_sint](#pb_read_sint) (function)
- [pb_read_bool](#pb_read_bool) (function)
- [pb_read_fixed64](#pb_read_fixed64) (function)
- [pb_read_fixed32](#pb_read_fixed32) (function)
- [pb_read_double](#pb_read_double) (function)
- [pb_read_bytes](#pb_read_bytes) (function)
- [pb_read_string](#pb_read_string) (function)
- [pb_read_packed](#pb_read_packed) (function)
- [pb_skip](#pb_skip) (function)
- [pb_hex](#pb_hex) (function)

## PB_VARINT:

```fides
pub const PB_VARINT: i64 = 0;
```

*Defined at line 26*

---

## PB_FIXED64:

```fides
pub const PB_FIXED64: i64 = 1;
```

*Defined at line 27*

---

## PB_LEN:

```fides
pub const PB_LEN: i64 = 2;
```

*Defined at line 28*

---

## PB_FIXED32:

```fides
pub const PB_FIXED32: i64 = 5;
```

*Defined at line 29*

---

## PbWriter

```fides
pub struct PbWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |

*Defined at line 32*

---

## pb_writer_new

An empty message writer.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var w = pb_writer_new();
pb_write_varint(&mut w, 1, 150);
assert_eq(pb_hex(w.bytes), "08 96 01");
pb_write_string(&mut w, 2, "hi");
pb_write_sint(&mut w, 3, -2);
var r = pb_reader_new(w.bytes);
assert(pb_next(&mut r));
assert_eq(r.field, 1);
assert_eq(pb_read_varint(&mut r), 150);
assert(pb_next(&mut r));
assert_eq(pb_read_string(&mut r), "hi");
assert(pb_next(&mut r));
assert_eq(pb_read_sint(&mut r), -2);
assert(!pb_next(&mut r));
```

```fides
pub fn pb_writer_new() -> PbWriter {
```

**Returns:** `PbWriter`

*Defined at line 53*

---

## pb_put_varint

```fides
pub fn pb_put_varint(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 62*

---

## pb_zigzag

```fides
pub fn pb_zigzag(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## pb_unzigzag

```fides
pub fn pb_unzigzag(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## pb_write_varint

int32, int64, uint32, uint64, bool or enum field

```fides
pub fn pb_write_varint(w: &mut PbWriter, field: i64, v: i64) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `v`: `i64`

*Defined at line 88*

---

## pb_write_sint

sint32 or sint64 field (zigzag: small negatives stay short)

```fides
pub fn pb_write_sint(w: &mut PbWriter, field: i64, v: i64) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `v`: `i64`

*Defined at line 94*

---

## pb_write_bool

```fides
pub fn pb_write_bool(w: &mut PbWriter, field: i64, b: bool) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `b`: `bool`

*Defined at line 98*

---

## pb_write_fixed64

```fides
pub fn pb_write_fixed64(w: &mut PbWriter, field: i64, v: i64) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `v`: `i64`

*Defined at line 108*

---

## pb_write_fixed32

fixed32 or sfixed32 field, little-endian

```fides
pub fn pb_write_fixed32(w: &mut PbWriter, field: i64, v: i64) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `v`: `i64`

*Defined at line 118*

---

## pb_write_double

double field

```fides
pub fn pb_write_double(w: &mut PbWriter, field: i64, x: f64) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `x`: `f64`

*Defined at line 128*

---

## pb_write_bytes

bytes field (or an embedded message's encoding)

```fides
pub fn pb_write_bytes(w: &mut PbWriter, field: i64, data: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `data`: `Vec<i64>`

*Defined at line 133*

---

## pb_write_string

string field (UTF-8 bytes as they are)

```fides
pub fn pb_write_string(w: &mut PbWriter, field: i64, s: str) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `s`: `str`

*Defined at line 144*

---

## pb_write_message

An embedded message: the bytes another writer made

```fides
pub fn pb_write_message(w: &mut PbWriter, field: i64, inner: PbWriter) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `inner`: `PbWriter`

*Defined at line 155*

---

## pb_write_packed

A packed repeated varint field

```fides
pub fn pb_write_packed(w: &mut PbWriter, field: i64, values: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut PbWriter`
- `field`: `i64`
- `values`: `Vec<i64>`

*Defined at line 160*

---

## PbReader

A cursor over encoded bytes. After pb_next: `field` and `wire` of the
field at hand; `ok` turns false on malformed input (and stays false).

```fides
pub struct PbReader {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `pos` | `i64` |
| `end` | `i64` |
| `field` | `i64` |
| `wire` | `i64` |
| `ok` | `bool` |

*Defined at line 174*

---

## pb_reader_new

```fides
pub fn pb_reader_new(bytes: Vec<i64>) -> PbReader {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `PbReader`

*Defined at line 183*

---

## pb_next

Move to the next field; false at the end or on malformed input.

```fides
pub fn pb_next(r: &mut PbReader) -> bool {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `bool`

*Defined at line 208*

---

## pb_read_varint

```fides
pub fn pb_read_varint(r: &mut PbReader) -> i64 {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `i64`

*Defined at line 225*

---

## pb_read_sint

```fides
pub fn pb_read_sint(r: &mut PbReader) -> i64 {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `i64`

*Defined at line 229*

---

## pb_read_bool

```fides
pub fn pb_read_bool(r: &mut PbReader) -> bool {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `bool`

*Defined at line 233*

---

## pb_read_fixed64

```fides
pub fn pb_read_fixed64(r: &mut PbReader) -> i64 {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `i64`

*Defined at line 253*

---

## pb_read_fixed32

```fides
pub fn pb_read_fixed32(r: &mut PbReader) -> i64 {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `i64`

*Defined at line 257*

---

## pb_read_double

```fides
pub fn pb_read_double(r: &mut PbReader) -> f64 {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `f64`

*Defined at line 261*

---

## pb_read_bytes

A length-delimited field's bytes (bytes, or an embedded message to
read with its own pb_reader_new)

```fides
pub fn pb_read_bytes(r: &mut PbReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `Vec<i64>`

*Defined at line 267*

---

## pb_read_string

```fides
pub fn pb_read_string(r: &mut PbReader) -> str {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `str`

*Defined at line 284*

---

## pb_read_packed

A packed repeated varint field's values

```fides
pub fn pb_read_packed(r: &mut PbReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut PbReader`

**Returns:** `Vec<i64>`

*Defined at line 296*

---

## pb_skip

Skip the field at hand (one not wanted, as readers must allow)

```fides
pub fn pb_skip(r: &mut PbReader) {
```

**Parameters:**

- `r`: `&mut PbReader`

*Defined at line 310*

---

## pb_hex

The bytes as lowercase hex, two digits each (for tests and logs)

```fides
pub fn pb_hex(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 330*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
