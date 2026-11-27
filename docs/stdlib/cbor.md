[All modules](index.md)

# cbor -- Fides Documentation

## Contents

- [CBOR_UINT:](#CBOR_UINT:) (const)
- [CBOR_NEGINT:](#CBOR_NEGINT:) (const)
- [CBOR_BYTES:](#CBOR_BYTES:) (const)
- [CBOR_TEXT:](#CBOR_TEXT:) (const)
- [CBOR_ARRAY:](#CBOR_ARRAY:) (const)
- [CBOR_MAP:](#CBOR_MAP:) (const)
- [CBOR_TAG:](#CBOR_TAG:) (const)
- [CBOR_SIMPLE:](#CBOR_SIMPLE:) (const)
- [CBOR_FLOAT:](#CBOR_FLOAT:) (const)
- [CBOR_FALSE:](#CBOR_FALSE:) (const)
- [CBOR_TRUE:](#CBOR_TRUE:) (const)
- [CBOR_NULL:](#CBOR_NULL:) (const)
- [CBOR_UNDEFINED:](#CBOR_UNDEFINED:) (const)
- [CBOR_MAX_DEPTH:](#CBOR_MAX_DEPTH:) (const)
- [cbor_put_head](#cbor_put_head) (function)
- [cbor_put_uint](#cbor_put_uint) (function)
- [cbor_put_int](#cbor_put_int) (function)
- [cbor_put_negint_raw](#cbor_put_negint_raw) (function)
- [cbor_put_bytes](#cbor_put_bytes) (function)
- [cbor_put_text](#cbor_put_text) (function)
- [cbor_put_array_head](#cbor_put_array_head) (function)
- [cbor_put_map_head](#cbor_put_map_head) (function)
- [cbor_put_tag](#cbor_put_tag) (function)
- [cbor_put_indef](#cbor_put_indef) (function)
- [cbor_put_break](#cbor_put_break) (function)
- [cbor_put_bool](#cbor_put_bool) (function)
- [cbor_put_null](#cbor_put_null) (function)
- [cbor_put_undefined](#cbor_put_undefined) (function)
- [cbor_put_simple](#cbor_put_simple) (function)
- [cbor_put_float](#cbor_put_float) (function)
- [cbor_put_double](#cbor_put_double) (function)
- [cbor_pow2](#cbor_pow2) (function)
- [cbor_half_to_f64](#cbor_half_to_f64) (function)
- [cbor_single_to_f64](#cbor_single_to_f64) (function)
- [cbor_negate](#cbor_negate) (function)
- [cbor_f64_to_half](#cbor_f64_to_half) (function)
- [cbor_f64_to_single](#cbor_f64_to_single) (function)
- [CborDoc](#CborDoc) (struct)
- [cbor_decode](#cbor_decode) (function)
- [cbor_decode_all](#cbor_decode_all) (function)
- [cbor_kind](#cbor_kind) (function)
- [cbor_uint](#cbor_uint) (function)
- [cbor_int_fits](#cbor_int_fits) (function)
- [cbor_int](#cbor_int) (function)
- [cbor_bytes](#cbor_bytes) (function)
- [cbor_text](#cbor_text) (function)
- [cbor_len](#cbor_len) (function)
- [cbor_at](#cbor_at) (function)
- [cbor_map_key](#cbor_map_key) (function)
- [cbor_map_val](#cbor_map_val) (function)
- [cbor_map_get](#cbor_map_get) (function)
- [cbor_map_get_int](#cbor_map_get_int) (function)
- [cbor_tag](#cbor_tag) (function)
- [cbor_tag_content](#cbor_tag_content) (function)
- [cbor_float](#cbor_float) (function)
- [cbor_is_number](#cbor_is_number) (function)
- [cbor_is_bool](#cbor_is_bool) (function)
- [cbor_bool](#cbor_bool) (function)
- [cbor_is_null](#cbor_is_null) (function)
- [cbor_encode](#cbor_encode) (function)
- [cbor_hex](#cbor_hex) (function)
- [cbor_float_diag](#cbor_float_diag) (function)
- [cbor_diag](#cbor_diag) (function)

## CBOR_UINT:

```fides
pub const CBOR_UINT: i64 = 0;
```

*Defined at line 45*

---

## CBOR_NEGINT:

```fides
pub const CBOR_NEGINT: i64 = 1;
```

*Defined at line 46*

---

## CBOR_BYTES:

```fides
pub const CBOR_BYTES: i64 = 2;
```

*Defined at line 47*

---

## CBOR_TEXT:

```fides
pub const CBOR_TEXT: i64 = 3;
```

*Defined at line 48*

---

## CBOR_ARRAY:

```fides
pub const CBOR_ARRAY: i64 = 4;
```

*Defined at line 49*

---

## CBOR_MAP:

```fides
pub const CBOR_MAP: i64 = 5;
```

*Defined at line 50*

---

## CBOR_TAG:

```fides
pub const CBOR_TAG: i64 = 6;
```

*Defined at line 51*

---

## CBOR_SIMPLE:

```fides
pub const CBOR_SIMPLE: i64 = 7;
```

*Defined at line 52*

---

## CBOR_FLOAT:

```fides
pub const CBOR_FLOAT: i64 = 8;
```

*Defined at line 53*

---

## CBOR_FALSE:

```fides
pub const CBOR_FALSE: i64 = 20;
```

*Defined at line 56*

---

## CBOR_TRUE:

```fides
pub const CBOR_TRUE: i64 = 21;
```

*Defined at line 57*

---

## CBOR_NULL:

```fides
pub const CBOR_NULL: i64 = 22;
```

*Defined at line 58*

---

## CBOR_UNDEFINED:

```fides
pub const CBOR_UNDEFINED: i64 = 23;
```

*Defined at line 59*

---

## CBOR_MAX_DEPTH:

Deepest nesting the decoder accepts.

```fides
pub const CBOR_MAX_DEPTH: i64 = 64;
```

*Defined at line 62*

---

## cbor_put_head

```fides
pub fn cbor_put_head(out: &mut Vec<i64>, major: i64, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `major`: `i64`
- `n`: `i64`

*Defined at line 69*

---

## cbor_put_uint

An unsigned integer; `n` is read as 64 unsigned bits (so -1 is
18446744073709551615).

```fides
pub fn cbor_put_uint(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 100*

---

## cbor_put_int

A signed integer: major type 0 for v >= 0, 1 (as -1-n) below.

```fides
pub fn cbor_put_int(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 105*

---

## cbor_put_negint_raw

A negative integer given by n: the value is -1-n (n as 64 unsigned
bits, so every value down to -2^64 can be written).

```fides
pub fn cbor_put_negint_raw(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 115*

---

## cbor_put_bytes

```fides
pub fn cbor_put_bytes(out: &mut Vec<i64>, data: Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `data`: `Vec<i64>`

*Defined at line 119*

---

## cbor_put_text

A text string (the bytes of `s`, which should be UTF-8).

```fides
pub fn cbor_put_text(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 129*

---

## cbor_put_array_head

The head of an array of `n` items (the items follow).

```fides
pub fn cbor_put_array_head(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 140*

---

## cbor_put_map_head

The head of a map of `n` pairs (key, value, key, value ... follow).

```fides
pub fn cbor_put_map_head(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 145*

---

## cbor_put_tag

A tag; the tagged item follows.

```fides
pub fn cbor_put_tag(out: &mut Vec<i64>, tag: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `tag`: `i64`

*Defined at line 150*

---

## cbor_put_indef

Start an indefinite-length item of major type 2, 3, 4 or 5; end it
with cbor_put_break.

```fides
pub fn cbor_put_indef(out: &mut Vec<i64>, major: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `major`: `i64`

*Defined at line 156*

---

## cbor_put_break

```fides
pub fn cbor_put_break(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 160*

---

## cbor_put_bool

```fides
pub fn cbor_put_bool(out: &mut Vec<i64>, b: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `b`: `bool`

*Defined at line 164*

---

## cbor_put_null

```fides
pub fn cbor_put_null(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 172*

---

## cbor_put_undefined

```fides
pub fn cbor_put_undefined(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 176*

---

## cbor_put_simple

simple(n), n in 0..19 or 32..255 (20..23 are false/true/null/undefined,
24..31 are reserved).

```fides
pub fn cbor_put_simple(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 182*

---

## cbor_put_float

A float in its preferred serialization: half precision if that keeps
the value, else single, else double. Every NaN is written as f97e00.

```fides
pub fn cbor_put_float(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 193*

---

## cbor_put_double

A float always as 8 bytes (major 7, additional information 27).

```fides
pub fn cbor_put_double(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 210*

---

## cbor_pow2

2^e as an f64, exactly (e in -1074..1023).

```fides
pub fn cbor_pow2(e: i64) -> f64 {
```

**Parameters:**

- `e`: `i64`

**Returns:** `f64`

*Defined at line 218*

---

## cbor_half_to_f64

The value of a half-precision float (16 bits).

```fides
pub fn cbor_half_to_f64(h: i64) -> f64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `f64`

*Defined at line 236*

---

## cbor_single_to_f64

The value of a single-precision float (32 bits).

```fides
pub fn cbor_single_to_f64(s: i64) -> f64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `f64`

*Defined at line 259*

---

## cbor_negate

```fides
pub fn cbor_negate(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 283*

---

## cbor_f64_to_half

```fides
pub fn cbor_f64_to_half(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 296*

---

## cbor_f64_to_single

```fides
pub fn cbor_f64_to_single(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 334*

---

## CborDoc

A decoded CBOR item as a flat tree. Node i has kind[i] (CBOR_*) and:
UINT: val = the value (64 bits); NEGINT: val = n, the value is -1-n;
BYTES/TEXT: data = the bytes (an indefinite string has its chunks as
children and data = all of them joined);
ARRAY: children = the items; MAP: children = key, value, key, ...;
TAG: val = the tag number, one child; SIMPLE: val = the simple value;
FLOAT: fval = the value, val = its width on the wire (2, 4 or 8).
`indef` marks indefinite length. `ok` false: `error` says why.

```fides
pub struct CborDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>` |
| `val` | `Vec<i64>` |
| `fval` | `Vec<f64>` |
| `data` | `Vec<Vec<i64>>` |
| `indef` | `Vec<bool>` |
| `first` | `Vec<i64>` |
| `nkids` | `Vec<i64>` |
| `kids` | `Vec<i64>` |
| `root` | `i64` |
| `ok` | `bool` |
| `error` | `str` |
| `used` | `i64,  // bytes the root item took` |

*Defined at line 379*

---

## cbor_decode

Decode the first item of `bytes`. Bytes after it are allowed (`used`
says how many the item took); cbor_decode_all requires none.

```fides
pub fn cbor_decode(bytes: Vec<i64>) -> CborDoc {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `CborDoc`

*Defined at line 676*

---

## cbor_decode_all

Decode a whole buffer holding exactly one item.

```fides
pub fn cbor_decode_all(bytes: Vec<i64>) -> CborDoc {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `CborDoc`

*Defined at line 690*

---

## cbor_kind

```fides
pub fn cbor_kind(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 702*

---

## cbor_uint

UINT: the value's 64 bits; NEGINT: n (value -1-n); TAG: the tag number;
SIMPLE: the simple value; BYTES/TEXT: the length; ARRAY: items; MAP: pairs.

```fides
pub fn cbor_uint(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 708*

---

## cbor_int_fits

Whether cbor_int gives the exact value (an integer in the i64 range).

```fides
pub fn cbor_int_fits(d: &CborDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 713*

---

## cbor_int

An integer node's value (exact when cbor_int_fits).

```fides
pub fn cbor_int(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 719*

---

## cbor_bytes

Bytes of a byte or text string (all chunks joined).

```fides
pub fn cbor_bytes(d: &CborDoc, node: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 727*

---

## cbor_text

```fides
pub fn cbor_text(d: &CborDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 731*

---

## cbor_len

Items of an array, pairs of a map, chunks of an indefinite string.

```fides
pub fn cbor_len(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 736*

---

## cbor_at

Item i of an array (or chunk i of an indefinite string).

```fides
pub fn cbor_at(d: &CborDoc, node: i64, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 748*

---

## cbor_map_key

```fides
pub fn cbor_map_key(d: &CborDoc, node: i64, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 752*

---

## cbor_map_val

```fides
pub fn cbor_map_val(d: &CborDoc, node: i64, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 756*

---

## cbor_map_get

The value under text key `key` in a map, or -1.

```fides
pub fn cbor_map_get(d: &CborDoc, node: i64, key: str) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`
- `key`: `str`

**Returns:** `i64`

*Defined at line 761*

---

## cbor_map_get_int

The value under integer key `key` in a map, or -1.

```fides
pub fn cbor_map_get_int(d: &CborDoc, node: i64, key: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`
- `key`: `i64`

**Returns:** `i64`

*Defined at line 777*

---

## cbor_tag

```fides
pub fn cbor_tag(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 793*

---

## cbor_tag_content

```fides
pub fn cbor_tag_content(d: &CborDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 797*

---

## cbor_float

A float node's value; an integer node's value as f64.

```fides
pub fn cbor_float(d: &CborDoc, node: i64) -> f64 {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `f64`

*Defined at line 802*

---

## cbor_is_number

```fides
pub fn cbor_is_number(d: &CborDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 813*

---

## cbor_is_bool

```fides
pub fn cbor_is_bool(d: &CborDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 818*

---

## cbor_bool

```fides
pub fn cbor_bool(d: &CborDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 822*

---

## cbor_is_null

```fides
pub fn cbor_is_null(d: &CborDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 826*

---

## cbor_encode

Encode node `node` of `d` again: definite and indefinite lengths and
float widths as decoded, every head in its shortest form.

```fides
pub fn cbor_encode(d: &CborDoc, node: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 834*

---

## cbor_hex

Lowercase hex of the bytes, no separators.

```fides
pub fn cbor_hex(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 894*

---

## cbor_float_diag

A float as diagnostic notation: Infinity, -Infinity, NaN, "N.0" for
integral values below 2^53, else float_to_str's digits.

```fides
pub fn cbor_float_diag(x: f64) -> str {
```

**Parameters:**

- `x`: `f64`

**Returns:** `str`

*Defined at line 979*

---

## cbor_diag

Diagnostic notation of node `node`.

```fides
pub fn cbor_diag(d: &CborDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&CborDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1009*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
