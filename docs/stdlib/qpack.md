[All modules](index.md)

# qpack -- Fides Documentation

## Contents

- [QPACK_DECOMPRESSION_FAILED:](#QPACK_DECOMPRESSION_FAILED:) (const)
- [QPACK_ENCODER_STREAM_ERROR:](#QPACK_ENCODER_STREAM_ERROR:) (const)
- [QPACK_DECODER_STREAM_ERROR:](#QPACK_DECODER_STREAM_ERROR:) (const)
- [QPACK_STATIC_LEN:](#QPACK_STATIC_LEN:) (const)
- [QPACK_SCAN_OK:](#QPACK_SCAN_OK:) (const)
- [QPACK_SCAN_MORE:](#QPACK_SCAN_MORE:) (const)
- [QPACK_SCAN_ERROR:](#QPACK_SCAN_ERROR:) (const)
- [QpackFields](#QpackFields) (struct)
- [QpackScan](#QpackScan) (struct)
- [qpack_static_name](#qpack_static_name) (function)
- [qpack_static_value](#qpack_static_value) (function)
- [qpack_static_find](#qpack_static_find) (function)
- [qpack_static_find_name](#qpack_static_find_name) (function)
- [qpack_section_size](#qpack_section_size) (function)
- [qpack_huffman_code](#qpack_huffman_code) (function)
- [qpack_encode_line](#qpack_encode_line) (function)
- [qpack_encode](#qpack_encode) (function)
- [qpack_decode](#qpack_decode) (function)
- [qpack_encoder_stream_scan](#qpack_encoder_stream_scan) (function)
- [qpack_decoder_stream_scan](#qpack_decoder_stream_scan) (function)

## QPACK_DECOMPRESSION_FAILED:

QPACK error codes (RFC 9204 §6), sent as HTTP/3 application errors.

```fides
pub const QPACK_DECOMPRESSION_FAILED: i64 = 0x200;
```

*Defined at line 40*

---

## QPACK_ENCODER_STREAM_ERROR:

```fides
pub const QPACK_ENCODER_STREAM_ERROR: i64 = 0x201;
```

*Defined at line 41*

---

## QPACK_DECODER_STREAM_ERROR:

```fides
pub const QPACK_DECODER_STREAM_ERROR: i64 = 0x202;
```

*Defined at line 42*

---

## QPACK_STATIC_LEN:

The static table has 99 entries, 0..98 (Appendix A).

```fides
pub const QPACK_STATIC_LEN: i64 = 99;
```

*Defined at line 45*

---

## QPACK_SCAN_OK:

Results of scanning an encoder or decoder stream.

```fides
pub const QPACK_SCAN_OK: i64 = 0;
```

*Defined at line 48*

---

## QPACK_SCAN_MORE:

```fides
pub const QPACK_SCAN_MORE: i64 = 1;
```

*Defined at line 49*

---

## QPACK_SCAN_ERROR:

```fides
pub const QPACK_SCAN_ERROR: i64 = 2;
```

*Defined at line 50*

---

## QpackFields

A decoded field section. `ok` false: `error` is the QPACK error code
(QPACK_DECOMPRESSION_FAILED), or -1 with `too_large` when the section
is over the size limit (an HTTP/3 message error, not a QPACK one);
`why` says what was wrong. `size` is the section's size by RFC 9114
§4.2.2.

```fides
pub struct QpackFields {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `i64` |
| `too_large` | `bool` |
| `why` | `str` |
| `names` | `Vec<str>` |
| `values` | `Vec<str>` |
| `size` | `i64` |

*Defined at line 57*

---

## QpackScan

What an encoder- or decoder-stream scan found: QPACK_SCAN_OK when
every instruction from `from` was complete and allowed (`used` bytes),
QPACK_SCAN_MORE when the last one is cut short (`used` covers the
complete ones before it), QPACK_SCAN_ERROR with the code in `error`.

```fides
pub struct QpackScan {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `used` | `i64` |
| `error` | `i64` |
| `why` | `str` |

*Defined at line 71*

---

## qpack_static_name

Name of static entry `i` (0..98); "" out of range.

```fides
pub fn qpack_static_name(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 296*

---

## qpack_static_value

Value of static entry `i` (0..98); "" out of range.

```fides
pub fn qpack_static_value(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 305*

---

## qpack_static_find

The static entry holding exactly `name` and `value`, -1 if none.

```fides
pub fn qpack_static_find(name: str, value: str) -> i64 {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `i64`

*Defined at line 314*

---

## qpack_static_find_name

The first static entry named `name`, -1 if none.

```fides
pub fn qpack_static_find_name(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 327*

---

## qpack_section_size

The size of a field section by RFC 9114 §4.2.2 (and RFC 9204 §3.2.1):
each line's name and value lengths plus 32.

```fides
pub fn qpack_section_size(names: Vec<str>, values: Vec<str>) -> i64 {
```

**Parameters:**

- `names`: `Vec<str>`
- `values`: `Vec<str>`

**Returns:** `i64`

*Defined at line 341*

---

## qpack_huffman_code

The Huffman code of a symbol (0..255, 256 = EOS), right-aligned.

```fides
pub fn qpack_huffman_code(sym: i64) -> i64 {
```

**Parameters:**

- `sym`: `i64`

**Returns:** `i64`

*Defined at line 421*

---

## qpack_encode_line

Append one field line: an indexed line when the static table holds the
whole field, a literal with a static name reference when it holds the
name, a literal with a literal name otherwise. Strings are
Huffman-coded when `huffman` and that is shorter.

```fides
pub fn qpack_encode_line(out: &mut Vec<i64>, name: str, value: str, huffman: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `name`: `str`
- `value`: `str`
- `huffman`: `bool`

*Defined at line 586*

---

## qpack_encode

Encode a field section with the static table only: the prefix
(Required Insert Count 0, Base 0), then one line per field.

```fides
pub fn qpack_encode(names: Vec<str>, values: Vec<str>, huffman: bool) -> Vec<i64> {
```

**Parameters:**

- `names`: `Vec<str>`
- `values`: `Vec<str>`
- `huffman`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 616*

---

## qpack_decode

Decode a field section of a HEADERS frame, with no dynamic table (our
SETTINGS_QPACK_MAX_TABLE_CAPACITY is 0). `max_size` is the largest
section accepted (RFC 9114 §4.2.2), -1 for no limit.

```fides
pub fn qpack_decode(block: Vec<i64>, max_size: i64) -> QpackFields {
```

**Parameters:**

- `block`: `Vec<i64>`
- `max_size`: `i64`

**Returns:** `QpackFields`

*Defined at line 676*

---

## qpack_encoder_stream_scan

Scan what the peer sent on its QPACK encoder stream from `from`,
against our table capacity `max_capacity` (0 for stdlib/http3.fd).
With a capacity of 0 nothing can be inserted: only Set Dynamic Table
Capacity 0 is allowed.

```fides
pub fn qpack_encoder_stream_scan(b: Vec<i64>, from: i64, max_capacity: i64) -> QpackScan {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from`: `i64`
- `max_capacity`: `i64`

**Returns:** `QpackScan`

*Defined at line 789*

---

## qpack_decoder_stream_scan

Scan what the peer sent on its QPACK decoder stream from `from`. We
never reference the dynamic table, so the peer has nothing to
acknowledge: Section Acknowledgment and Insert Count Increment are
errors; Stream Cancellation is fine.

```fides
pub fn qpack_decoder_stream_scan(b: Vec<i64>, from: i64) -> QpackScan {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from`: `i64`

**Returns:** `QpackScan`

*Defined at line 818*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
