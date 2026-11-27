[All modules](index.md)

# cobol -- Fides Documentation

## Contents

- [COBOL_ALPHA:](#COBOL_ALPHA:) (const)
- [COBOL_ZONED:](#COBOL_ZONED:) (const)
- [COBOL_PACKED:](#COBOL_PACKED:) (const)
- [COBOL_BINARY:](#COBOL_BINARY:) (const)
- [CobolField](#CobolField) (struct)
- [Copybook](#Copybook) (struct)
- [cobol_packed_size](#cobol_packed_size) (function)
- [cobol_pack](#cobol_pack) (function)
- [cobol_packed_valid](#cobol_packed_valid) (function)
- [cobol_unpack](#cobol_unpack) (function)
- [cobol_packed_hex](#cobol_packed_hex) (function)
- [cobol_fits](#cobol_fits) (function)
- [cobol_zone](#cobol_zone) (function)
- [cobol_zoned_valid](#cobol_zoned_valid) (function)
- [cobol_unzone](#cobol_unzone) (function)
- [cobol_binary_size](#cobol_binary_size) (function)
- [cobol_copybook](#cobol_copybook) (function)
- [cobol_field_index](#cobol_field_index) (function)
- [cobol_layout](#cobol_layout) (function)
- [cobol_record_new](#cobol_record_new) (function)
- [cobol_set_num](#cobol_set_num) (function)
- [cobol_get_num](#cobol_get_num) (function)
- [cobol_set_str](#cobol_set_str) (function)
- [cobol_get_str](#cobol_get_str) (function)
- [cobol_field_valid](#cobol_field_valid) (function)
- [cobol_format](#cobol_format) (function)
- [cobol_split_records](#cobol_split_records) (function)
- [cobol_join_records](#cobol_join_records) (function)

## COBOL_ALPHA:

```fides
pub const COBOL_ALPHA: i64 = 1;
```

*Defined at line 33*

---

## COBOL_ZONED:

```fides
pub const COBOL_ZONED: i64 = 2;
```

*Defined at line 34*

---

## COBOL_PACKED:

```fides
pub const COBOL_PACKED: i64 = 3;
```

*Defined at line 35*

---

## COBOL_BINARY:

```fides
pub const COBOL_BINARY: i64 = 4;
```

*Defined at line 36*

---

## CobolField

One elementary field of a copybook.

```fides
pub struct CobolField {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `kind` | `i64,  // COBOL_ALPHA / ZONED / PACKED / BINARY` |
| `digits` | `i64,  // total digits (numeric) or characters (alpha)` |
| `scale` | `i64,  // digits after the implied point (V)` |
| `signed` | `bool,  // PIC S...` |
| `offset` | `i64,  // byte offset in the record` |
| `size` | `i64,  // bytes in the record` |

*Defined at line 39*

---

## Copybook

A parsed copybook: its elementary fields in record order.

```fides
pub struct Copybook {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str,  // the 01-level record name ("" if none)` |
| `fields` | `Vec<CobolField>` |
| `size` | `i64,  // record length in bytes` |
| `error` | `str,  // "" when the copybook parsed cleanly` |

*Defined at line 50*

---

## cobol_packed_size

Bytes a packed field of `digits` digits takes: digits + sign nibble,
rounded up to whole bytes.

```fides
pub fn cobol_packed_size(digits: i64) -> i64 {
```

**Parameters:**

- `digits`: `i64`

**Returns:** `i64`

*Defined at line 61*

---

## cobol_pack

Encode `value` as packed decimal of `digits` digits. Sign nibble C/D
when `signed`, F otherwise (a negative value into an unsigned field
keeps its magnitude, as COBOL MOVE does). High digits beyond `digits`
are cut, as COBOL truncation does; use cobol_fits first to reject.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(cobol_packed_hex(cobol_pack(12345, 5, true)), "12345C");
assert_eq(cobol_packed_hex(cobol_pack(-12345, 5, true)), "12345D");
assert_eq(cobol_packed_size(5), 3);
val p = cobol_pack(-42, 5, true);
assert(cobol_packed_valid(p, 0, 3));
assert_eq(cobol_unpack(p, 0, 3), -42);
assert(cobol_fits(99999, 5, false));
assert(!cobol_fits(100000, 5, false));
assert_eq(str_from_bytes(cobol_zone(123, 3, true)), "12C");
assert_eq(cobol_unzone(str_bytes("12}"), 0, 3), -120);
```

```fides
pub fn cobol_pack(value: i64, digits: i64, signed: bool) -> Vec<i64> {
```

**Parameters:**

- `value`: `i64`
- `digits`: `i64`
- `signed`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 81*

---

## cobol_packed_valid

True when bytes[start..start+size] are valid packed decimal: every
digit nibble 0-9 and the sign nibble A-F.

```fides
pub fn cobol_packed_valid(bytes: Vec<i64>, start: i64, size: i64) -> bool {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `start`: `i64`
- `size`: `i64`

**Returns:** `bool`

*Defined at line 121*

---

## cobol_unpack

Decode packed decimal at bytes[start..start+size]. Sign nibble B or D
is negative, any other valid sign positive. Invalid data gives 0
(check with cobol_packed_valid).

```fides
pub fn cobol_unpack(bytes: Vec<i64>, start: i64, size: i64) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `start`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 153*

---

## cobol_packed_hex

The bytes as upper-case hex, two characters each ("12345C").

```fides
pub fn cobol_packed_hex(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 177*

---

## cobol_fits

True when |value| has at most `digits` digits (and, for an unsigned
field, value >= 0): it can be stored without truncation.

```fides
pub fn cobol_fits(value: i64, digits: i64, signed: bool) -> bool {
```

**Parameters:**

- `value`: `i64`
- `digits`: `i64`
- `signed`: `bool`

**Returns:** `bool`

*Defined at line 191*

---

## cobol_zone

Encode `value` as `digits` ASCII digits; when `signed`, the last digit
carries the sign as an overpunch ('{'/'A'..'I' positive,
'}'/'J'..'R' negative).

```fides
pub fn cobol_zone(value: i64, digits: i64, signed: bool) -> Vec<i64> {
```

**Parameters:**

- `value`: `i64`
- `digits`: `i64`
- `signed`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 212*

---

## cobol_zoned_valid

True when bytes[start..start+size] are valid zoned decimal (spaces are
not digits: an unset numeric field is invalid, as in COBOL).

```fides
pub fn cobol_zoned_valid(bytes: Vec<i64>, start: i64, size: i64) -> bool {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `start`: `i64`
- `size`: `i64`

**Returns:** `bool`

*Defined at line 274*

---

## cobol_unzone

Decode zoned decimal at bytes[start..start+size] (0 when invalid).

```fides
pub fn cobol_unzone(bytes: Vec<i64>, start: i64, size: i64) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `start`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 289*

---

## cobol_binary_size

Bytes a COMP field of `digits` digits takes (2, 4 or 8).

```fides
pub fn cobol_binary_size(digits: i64) -> i64 {
```

**Parameters:**

- `digits`: `i64`

**Returns:** `i64`

*Defined at line 314*

---

## cobol_copybook

Parse copybook text (free format: level, name, PIC, USAGE, OCCURS,
VALUE; each entry ends with a period; '*' starts a comment line).
Check `.error` (empty when it parsed).

```fides
pub fn cobol_copybook(text: str) -> Copybook {
```

**Parameters:**

- `text`: `str`

**Returns:** `Copybook`

*Defined at line 467*

---

## cobol_field_index

Index of the field named `name` (upper-cased), or -1.

```fides
pub fn cobol_field_index(cb: &Copybook, name: str) -> i64 {
```

**Parameters:**

- `cb`: `&Copybook`
- `name`: `str`

**Returns:** `i64`

*Defined at line 580*

---

## cobol_layout

The copybook as a layout listing, one field a line:
"NAME kind=packed digits=9 scale=2 signed offset=8 size=5".

```fides
pub fn cobol_layout(cb: &Copybook) -> str {
```

**Parameters:**

- `cb`: `&Copybook`

**Returns:** `str`

*Defined at line 594*

---

## cobol_record_new

A fresh record: alphanumeric fields spaces, numeric fields zero.

```fides
pub fn cobol_record_new(cb: &Copybook) -> Vec<i64> {
```

**Parameters:**

- `cb`: `&Copybook`

**Returns:** `Vec<i64>`

*Defined at line 622*

---

## cobol_set_num

Set numeric field `name` to the scaled integer `value` (cents for
V99). False, and the record unchanged, when there is no such numeric
field, the record is too short, or the value does not fit.

```fides
pub fn cobol_set_num(cb: &Copybook, rec: &mut Vec<i64>, name: str, value: i64) -> bool {
```

**Parameters:**

- `cb`: `&Copybook`
- `rec`: `&mut Vec<i64>`
- `name`: `str`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 655*

---

## cobol_get_num

The scaled integer in numeric field `name` (0 when absent or invalid:
check with cobol_field_valid).

```fides
pub fn cobol_get_num(cb: &Copybook, rec: Vec<i64>, name: str) -> i64 {
```

**Parameters:**

- `cb`: `&Copybook`
- `rec`: `Vec<i64>`
- `name`: `str`

**Returns:** `i64`

*Defined at line 676*

---

## cobol_set_str

Set alphanumeric field `name`: truncated or padded with spaces.

```fides
pub fn cobol_set_str(cb: &Copybook, rec: &mut Vec<i64>, name: str, value: str) -> bool {
```

**Parameters:**

- `cb`: `&Copybook`
- `rec`: `&mut Vec<i64>`
- `name`: `str`
- `value`: `str`

**Returns:** `bool`

*Defined at line 698*

---

## cobol_get_str

Alphanumeric field `name`, trailing spaces removed ("" when absent).

```fides
pub fn cobol_get_str(cb: &Copybook, rec: Vec<i64>, name: str) -> str {
```

**Parameters:**

- `cb`: `&Copybook`
- `rec`: `Vec<i64>`
- `name`: `str`

**Returns:** `str`

*Defined at line 723*

---

## cobol_field_valid

True when field `name` holds valid data for its kind (an alphanumeric
field is always valid).

```fides
pub fn cobol_field_valid(cb: &Copybook, rec: Vec<i64>, name: str) -> bool {
```

**Parameters:**

- `cb`: `&Copybook`
- `rec`: `Vec<i64>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 747*

---

## cobol_format

Format a scaled integer with its implied decimal point:
cobol_format(-123456, 2) = "-1234.56".

```fides
pub fn cobol_format(value: i64, scale: i64) -> str {
```

**Parameters:**

- `value`: `i64`
- `scale`: `i64`

**Returns:** `str`

*Defined at line 767*

---

## cobol_split_records

Split a fixed-width file's bytes into records of `size` bytes (a short
tail is dropped; a trailing LF or CRLF after each record, as text
transfers add, is skipped when `line_sequential`).

```fides
pub fn cobol_split_records(data: Vec<i64>, size: i64, line_sequential: bool) -> Vec<Vec<i64>> {
```

**Parameters:**

- `data`: `Vec<i64>`
- `size`: `i64`
- `line_sequential`: `bool`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 791*

---

## cobol_join_records

Join records into one byte buffer (with "\n" after each when
`line_sequential`), ready for fs_write_bytes.

```fides
pub fn cobol_join_records(recs: Vec<Vec<i64>>, line_sequential: bool) -> Vec<i64> {
```

**Parameters:**

- `recs`: `Vec<Vec<i64>>`
- `line_sequential`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 820*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
