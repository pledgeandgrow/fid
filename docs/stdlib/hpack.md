[All modules](index.md)

# hpack -- Fides Documentation

## Contents

- [HPACK_INDEX:](#HPACK_INDEX:) (const)
- [HPACK_NO_INDEX:](#HPACK_NO_INDEX:) (const)
- [HPACK_NEVER_INDEX:](#HPACK_NEVER_INDEX:) (const)
- [HPACK_STATIC_LEN:](#HPACK_STATIC_LEN:) (const)
- [Hpack](#Hpack) (struct)
- [HpackHeaders](#HpackHeaders) (struct)
- [hpack_new](#hpack_new) (function)
- [huffman_code](#huffman_code) (function)
- [huffman_code_len](#huffman_code_len) (function)
- [hpack_static_name](#hpack_static_name) (function)
- [hpack_static_value](#hpack_static_value) (function)
- [huffman_encoded_len](#huffman_encoded_len) (function)
- [huffman_encode](#huffman_encode) (function)
- [huffman_decode_into](#huffman_decode_into) (function)
- [huffman_decode](#huffman_decode) (function)
- [hpack_put_int](#hpack_put_int) (function)
- [hpack_get_int](#hpack_get_int) (function)
- [hpack_put_str](#hpack_put_str) (function)
- [hpack_get_str](#hpack_get_str) (function)
- [hpack_entry_size](#hpack_entry_size) (function)
- [hpack_table_len](#hpack_table_len) (function)
- [hpack_table_add](#hpack_table_add) (function)
- [hpack_table_resize](#hpack_table_resize) (function)
- [hpack_index_valid](#hpack_index_valid) (function)
- [hpack_name_at](#hpack_name_at) (function)
- [hpack_value_at](#hpack_value_at) (function)
- [hpack_table_dump](#hpack_table_dump) (function)
- [hpack_decode](#hpack_decode) (function)
- [hpack_set_max_size](#hpack_set_max_size) (function)
- [hpack_put_size_updates](#hpack_put_size_updates) (function)
- [hpack_encode_field](#hpack_encode_field) (function)
- [hpack_default_mode](#hpack_default_mode) (function)
- [hpack_encode](#hpack_encode) (function)

## HPACK_INDEX:

How a literal field is represented (RFC 7541 6.2).

```fides
pub const HPACK_INDEX: i64 = 0;  // literal with incremental indexing
```

*Defined at line 27*

---

## HPACK_NO_INDEX:

```fides
pub const HPACK_NO_INDEX: i64 = 1;  // literal without indexing
```

*Defined at line 28*

---

## HPACK_NEVER_INDEX:

```fides
pub const HPACK_NEVER_INDEX: i64 = 2;  // literal never indexed
```

*Defined at line 29*

---

## HPACK_STATIC_LEN:

The static table has 61 entries (Appendix A); dynamic entries follow.

```fides
pub const HPACK_STATIC_LEN: i64 = 61;
```

*Defined at line 32*

---

## Hpack

Encoder or decoder state of one direction: the dynamic table.

```fides
pub struct Hpack {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `size` | `i64` |
| `max_size` | `i64` |
| `limit` | `i64` |
| `update` | `i64` |
| `update_min` | `i64` |
| `huffman` | `bool,  // encoder: Huffman-code a string when it is shorter` |

*Defined at line 37*

---

## HpackHeaders

A decoded header block. `ok` false: `error` says why (a COMPRESSION_ERROR).

```fides
pub struct HpackHeaders {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `never` | `Vec<bool>,  // field was "never indexed" (6.2.3)` |

*Defined at line 56*

---

## hpack_new

Encoder and decoder state with a dynamic table of `max_size` bytes
(4096 is the HTTP/2 default). The encoder Huffman-codes strings.

```fides
pub fn hpack_new(max_size: i64) -> Hpack {
```

**Parameters:**

- `max_size`: `i64`

**Returns:** `Hpack`

*Defined at line 66*

---

## huffman_code

The Huffman code of a symbol (0..255, 256 = EOS), right-aligned.

```fides
pub fn huffman_code(sym: i64) -> i64 {
```

**Parameters:**

- `sym`: `i64`

**Returns:** `i64`

*Defined at line 782*

---

## huffman_code_len

The length in bits of a symbol's Huffman code.

```fides
pub fn huffman_code_len(sym: i64) -> i64 {
```

**Parameters:**

- `sym`: `i64`

**Returns:** `i64`

*Defined at line 788*

---

## hpack_static_name

Name of static entry `i` (1..61).

```fides
pub fn hpack_static_name(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 794*

---

## hpack_static_value

Value of static entry `i` (1..61).

```fides
pub fn hpack_static_value(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 800*

---

## huffman_encoded_len

Bytes the Huffman coding of `bytes` takes.

```fides
pub fn huffman_encoded_len(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 808*

---

## huffman_encode

Huffman-code `bytes`; the last byte is padded with the high bits of EOS
(ones).

```fides
pub fn huffman_encode(bytes: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 821*

---

## huffman_decode_into

Decode `n` Huffman-coded bytes of `b` from `from` into `out`. False when
the input holds EOS, is padded with more than 7 bits, or with bits that
are not the start of EOS (RFC 7541 5.2): a decoding error.

```fides
pub fn huffman_decode_into(b: Vec<i64>, from: i64, n: i64, out: &mut Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from`: `i64`
- `n`: `i64`
- `out`: `&mut Vec<i64>`

**Returns:** `bool`

*Defined at line 850*

---

## huffman_decode

Decode a whole Huffman-coded string; Option::None on a decoding error.

```fides
pub fn huffman_decode(b: Vec<i64>) -> Option<Vec<i64>> {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `Option<Vec<i64>>`

*Defined at line 888*

---

## hpack_put_int

Append `value` as an integer with an N-bit prefix (`prefix` 1..8); `first`
holds the bits above the prefix in the first byte (the representation's
pattern, such as 0x80 for an indexed field).

```fides
pub fn hpack_put_int(out: &mut Vec<i64>, first: i64, prefix: i64, value: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `first`: `i64`
- `prefix`: `i64`
- `value`: `i64`

*Defined at line 901*

---

## hpack_get_int

Read an integer with an N-bit prefix at `*pos`, moving `*pos` past it.
-1 (and `*pos` -1) when the input ends first or the value is too large.

```fides
pub fn hpack_get_int(b: Vec<i64>, pos: &mut i64, prefix: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `pos`: `&mut i64`
- `prefix`: `i64`

**Returns:** `i64`

*Defined at line 918*

---

## hpack_put_str

Append a string literal: Huffman-coded when `huffman` and that is not
longer, raw otherwise.

```fides
pub fn hpack_put_str(out: &mut Vec<i64>, s: str, huffman: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`
- `huffman`: `bool`

*Defined at line 952*

---

## hpack_get_str

Read a string literal at `*pos`. On an error `*pos` becomes -1.

```fides
pub fn hpack_get_str(b: Vec<i64>, pos: &mut i64) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`
- `pos`: `&mut i64`

**Returns:** `str`

*Defined at line 973*

---

## hpack_entry_size

The size an entry takes in the table (4.1).

```fides
pub fn hpack_entry_size(name: str, value: str) -> i64 {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `i64`

*Defined at line 1008*

---

## hpack_table_len

Number of entries in the dynamic table.

```fides
pub fn hpack_table_len(h: &Hpack) -> i64 {
```

**Parameters:**

- `h`: `&Hpack`

**Returns:** `i64`

*Defined at line 1013*

---

## hpack_table_add

Add an entry, evicting as needed. One bigger than the whole table
leaves it empty (4.4).

```fides
pub fn hpack_table_add(h: &mut Hpack, name: str, value: str) {
```

**Parameters:**

- `h`: `&mut Hpack`
- `name`: `str`
- `value`: `str`

*Defined at line 1028*

---

## hpack_table_resize

Change the table's maximum size, evicting what no longer fits (4.3).

```fides
pub fn hpack_table_resize(h: &mut Hpack, max_size: i64) {
```

**Parameters:**

- `h`: `&mut Hpack`
- `max_size`: `i64`

*Defined at line 1043*

---

## hpack_index_valid

Is `i` an index of the static or dynamic table?

```fides
pub fn hpack_index_valid(h: &Hpack, i: i64) -> bool {
```

**Parameters:**

- `h`: `&Hpack`
- `i`: `i64`

**Returns:** `bool`

*Defined at line 1049*

---

## hpack_name_at

Name at index `i` of the combined index space (2.3.3).

```fides
pub fn hpack_name_at(h: &Hpack, i: i64) -> str {
```

**Parameters:**

- `h`: `&Hpack`
- `i`: `i64`

**Returns:** `str`

*Defined at line 1054*

---

## hpack_value_at

Value at index `i` of the combined index space.

```fides
pub fn hpack_value_at(h: &Hpack, i: i64) -> str {
```

**Parameters:**

- `h`: `&Hpack`
- `i`: `i64`

**Returns:** `str`

*Defined at line 1062*

---

## hpack_table_dump

The dynamic table as RFC 7541 Appendix C prints it.

```fides
pub fn hpack_table_dump(h: &Hpack) -> str {
```

**Parameters:**

- `h`: `&Hpack`

**Returns:** `str`

*Defined at line 1070*

---

## hpack_decode

Decode a header block, updating the dynamic table.

```fides
pub fn hpack_decode(h: &mut Hpack, block: Vec<i64>) -> HpackHeaders {
```

**Parameters:**

- `h`: `&mut Hpack`
- `block`: `Vec<i64>`

**Returns:** `HpackHeaders`

*Defined at line 1094*

---

## hpack_set_max_size

Encoder: change the table's maximum size (to at most the peer's
SETTINGS_HEADER_TABLE_SIZE). The change is announced at the start of the
next header block; when it went down and then up again, both (4.2).

```fides
pub fn hpack_set_max_size(h: &mut Hpack, max_size: i64) {
```

**Parameters:**

- `h`: `&mut Hpack`
- `max_size`: `i64`

*Defined at line 1174*

---

## hpack_put_size_updates

Append the pending size updates, if any. hpack_encode calls it; a caller
that writes fields one by one calls it before the first.

```fides
pub fn hpack_put_size_updates(h: &mut Hpack, out: &mut Vec<i64>) {
```

**Parameters:**

- `h`: `&mut Hpack`
- `out`: `&mut Vec<i64>`

*Defined at line 1184*

---

## hpack_encode_field

Append one field. With HPACK_INDEX a field already in a table is sent
as its index; otherwise as a literal of that kind, its name indexed when
a table has it. HPACK_NEVER_INDEX is for values such as passwords that
no intermediary may index.

```fides
pub fn hpack_encode_field(h: &mut Hpack, out: &mut Vec<i64>, name: str, value: str, mode: i64) {
```

**Parameters:**

- `h`: `&mut Hpack`
- `out`: `&mut Vec<i64>`
- `name`: `str`
- `value`: `str`
- `mode`: `i64`

*Defined at line 1239*

---

## hpack_default_mode

The representation hpack_encode picks for a field: never indexed for
credentials, without indexing for entries too big for the table, with
incremental indexing otherwise.

```fides
pub fn hpack_default_mode(h: &Hpack, name: str, value: str) -> i64 {
```

**Parameters:**

- `h`: `&Hpack`
- `name`: `str`
- `value`: `str`

**Returns:** `i64`

*Defined at line 1268*

---

## hpack_encode

Encode a header list (names must be lowercase in HTTP/2).

```fides
pub fn hpack_encode(h: &mut Hpack, names: Vec<str>, vals: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `h`: `&mut Hpack`
- `names`: `Vec<str>`
- `vals`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 1279*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
