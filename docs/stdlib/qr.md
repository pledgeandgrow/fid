[All modules](index.md)

# qr -- Fides Documentation

## Contents

- [QR_L:](#QR_L:) (const)
- [QR_M:](#QR_M:) (const)
- [QR_Q:](#QR_Q:) (const)
- [QR_H:](#QR_H:) (const)
- [QR_MODE_NUMERIC:](#QR_MODE_NUMERIC:) (const)
- [QR_MODE_ALNUM:](#QR_MODE_ALNUM:) (const)
- [QR_MODE_BYTE:](#QR_MODE_BYTE:) (const)
- [QR_MODE_ECI:](#QR_MODE_ECI:) (const)
- [qr_ecc_per_block](#qr_ecc_per_block) (function)
- [qr_num_blocks](#qr_num_blocks) (function)
- [qr_size](#qr_size) (function)
- [qr_raw_modules](#qr_raw_modules) (function)
- [qr_data_codewords](#qr_data_codewords) (function)
- [qr_alignment_positions](#qr_alignment_positions) (function)
- [qr_gf_mul](#qr_gf_mul) (function)
- [qr_rs_divisor](#qr_rs_divisor) (function)
- [qr_rs_remainder](#qr_rs_remainder) (function)
- [QrSegment](#QrSegment) (struct)
- [qr_is_numeric](#qr_is_numeric) (function)
- [qr_is_alnum](#qr_is_alnum) (function)
- [qr_segment_numeric](#qr_segment_numeric) (function)
- [qr_segment_alnum](#qr_segment_alnum) (function)
- [qr_segment_bytes](#qr_segment_bytes) (function)
- [qr_segment_eci](#qr_segment_eci) (function)
- [qr_segment_auto](#qr_segment_auto) (function)
- [qr_count_bits](#qr_count_bits) (function)
- [qr_total_bits](#qr_total_bits) (function)
- [QrCode](#QrCode) (struct)
- [qr_get](#qr_get) (function)
- [qr_format_bits](#qr_format_bits) (function)
- [qr_version_bits](#qr_version_bits) (function)
- [qr_add_ecc](#qr_add_ecc) (function)
- [qr_mask_bit](#qr_mask_bit) (function)
- [qr_penalty](#qr_penalty) (function)
- [qr_encode_segments](#qr_encode_segments) (function)
- [qr_make_data](#qr_make_data) (function)
- [qr_encode_codewords](#qr_encode_codewords) (function)
- [qr_encode_text](#qr_encode_text) (function)
- [qr_to_text](#qr_to_text) (function)
- [QrDecoded](#QrDecoded) (struct)
- [qr_decode](#qr_decode) (function)
- [qr_parse_data](#qr_parse_data) (function)

## QR_L:

```fides
pub const QR_L: i64 = 0;
```

*Defined at line 30*

---

## QR_M:

```fides
pub const QR_M: i64 = 1;
```

*Defined at line 31*

---

## QR_Q:

```fides
pub const QR_Q: i64 = 2;
```

*Defined at line 32*

---

## QR_H:

```fides
pub const QR_H: i64 = 3;
```

*Defined at line 33*

---

## QR_MODE_NUMERIC:

```fides
pub const QR_MODE_NUMERIC: i64 = 1;
```

*Defined at line 35*

---

## QR_MODE_ALNUM:

```fides
pub const QR_MODE_ALNUM: i64 = 2;
```

*Defined at line 36*

---

## QR_MODE_BYTE:

```fides
pub const QR_MODE_BYTE: i64 = 4;
```

*Defined at line 37*

---

## QR_MODE_ECI:

```fides
pub const QR_MODE_ECI: i64 = 7;
```

*Defined at line 38*

---

## qr_ecc_per_block

Error correction codewords per block, by level and version (index 0
unused).

```fides
pub fn qr_ecc_per_block(ecl: i64, ver: i64) -> i64 {
```

**Parameters:**

- `ecl`: `i64`
- `ver`: `i64`

**Returns:** `i64`

*Defined at line 55*

---

## qr_num_blocks

Error correction blocks by level and version.

```fides
pub fn qr_num_blocks(ecl: i64, ver: i64) -> i64 {
```

**Parameters:**

- `ecl`: `i64`
- `ver`: `i64`

**Returns:** `i64`

*Defined at line 70*

---

## qr_size

```fides
pub fn qr_size(ver: i64) -> i64 {
```

**Parameters:**

- `ver`: `i64`

**Returns:** `i64`

*Defined at line 84*

---

## qr_raw_modules

Data modules of a version (all modules minus function patterns and
format / version information).

```fides
pub fn qr_raw_modules(ver: i64) -> i64 {
```

**Parameters:**

- `ver`: `i64`

**Returns:** `i64`

*Defined at line 90*

---

## qr_data_codewords

Data codewords (after error correction is taken out).

```fides
pub fn qr_data_codewords(ver: i64, ecl: i64) -> i64 {
```

**Parameters:**

- `ver`: `i64`
- `ecl`: `i64`

**Returns:** `i64`

*Defined at line 103*

---

## qr_alignment_positions

Centers of the alignment patterns along one axis.

```fides
pub fn qr_alignment_positions(ver: i64) -> Vec<i64> {
```

**Parameters:**

- `ver`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 108*

---

## qr_gf_mul

```fides
pub fn qr_gf_mul(x: i64, y: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 137*

---

## qr_rs_divisor

The generator polynomial of `degree` (coefficients, highest first,
the leading 1 left out).

```fides
pub fn qr_rs_divisor(degree: i64) -> Vec<i64> {
```

**Parameters:**

- `degree`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 150*

---

## qr_rs_remainder

The error correction codewords of `data`.

```fides
pub fn qr_rs_remainder(data: &Vec<i64>, div: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `&Vec<i64>`
- `div`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 176*

---

## QrSegment

```fides
pub struct QrSegment {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `count` | `i64,  // characters (bytes for byte mode)` |
| `bits` | `Vec<i64>,  // one bit per element` |

*Defined at line 204*

---

## qr_is_numeric

```fides
pub fn qr_is_numeric(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 218*

---

## qr_is_alnum

```fides
pub fn qr_is_alnum(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 229*

---

## qr_segment_numeric

```fides
pub fn qr_segment_numeric(s: str) -> QrSegment {
```

**Parameters:**

- `s`: `str`

**Returns:** `QrSegment`

*Defined at line 240*

---

## qr_segment_alnum

```fides
pub fn qr_segment_alnum(s: str) -> QrSegment {
```

**Parameters:**

- `s`: `str`

**Returns:** `QrSegment`

*Defined at line 257*

---

## qr_segment_bytes

```fides
pub fn qr_segment_bytes(data: Vec<i64>) -> QrSegment {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `QrSegment`

*Defined at line 273*

---

## qr_segment_eci

An ECI designator segment (e.g. 26 for UTF-8).

```fides
pub fn qr_segment_eci(assign: i64) -> QrSegment {
```

**Parameters:**

- `assign`: `i64`

**Returns:** `QrSegment`

*Defined at line 286*

---

## qr_segment_auto

One segment in the most compact single mode for the text.

```fides
pub fn qr_segment_auto(s: str) -> QrSegment {
```

**Parameters:**

- `s`: `str`

**Returns:** `QrSegment`

*Defined at line 303*

---

## qr_count_bits

Width of the character count field.

```fides
pub fn qr_count_bits(mode: i64, ver: i64) -> i64 {
```

**Parameters:**

- `mode`: `i64`
- `ver`: `i64`

**Returns:** `i64`

*Defined at line 314*

---

## qr_total_bits

Bits the segments take at version `ver` (-1 when a count overflows).

```fides
pub fn qr_total_bits(segs: &Vec<QrSegment>, ver: i64) -> i64 {
```

**Parameters:**

- `segs`: `&Vec<QrSegment>`
- `ver`: `i64`

**Returns:** `i64`

*Defined at line 339*

---

## QrCode

```fides
pub struct QrCode {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `version` | `i64` |
| `ecl` | `i64` |
| `mask` | `i64` |
| `size` | `i64` |
| `modules` | `Vec<bool>` |
| `func` | `Vec<bool>` |

*Defined at line 356*

---

## qr_get

```fides
pub fn qr_get(q: &QrCode, x: i64, y: i64) -> bool {
```

**Parameters:**

- `q`: `&QrCode`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 382*

---

## qr_format_bits

The 15 format bits of a level and mask (with the 0x5412 mask).

```fides
pub fn qr_format_bits(ecl: i64, mask: i64) -> i64 {
```

**Parameters:**

- `ecl`: `i64`
- `mask`: `i64`

**Returns:** `i64`

*Defined at line 395*

---

## qr_version_bits

The 18 version bits (versions 7 and up).

```fides
pub fn qr_version_bits(ver: i64) -> i64 {
```

**Parameters:**

- `ver`: `i64`

**Returns:** `i64`

*Defined at line 416*

---

## qr_add_ecc

Error correction added and blocks interleaved: the final codewords.

```fides
pub fn qr_add_ecc(data: &Vec<i64>, ver: i64, ecl: i64) -> Vec<i64> {
```

**Parameters:**

- `data`: `&Vec<i64>`
- `ver`: `i64`
- `ecl`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 551*

---

## qr_mask_bit

Does mask `m` invert module (x, y)?

```fides
pub fn qr_mask_bit(m: i64, x: i64, y: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 635*

---

## qr_penalty

The penalty score of the symbol (N1 runs, N2 2x2 blocks, N3 finder-like
patterns, N4 dark balance).

```fides
pub fn qr_penalty(q: &QrCode) -> i64 {
```

**Parameters:**

- `q`: `&QrCode`

**Returns:** `i64`

*Defined at line 740*

---

## qr_encode_segments

Encode segments: the smallest version in [min_ver, max_ver] that fits
at `ecl`; `mask` -1 picks the lowest penalty.

```fides
pub fn qr_encode_segments(segs: Vec<QrSegment>, ecl: i64, min_ver: i64, max_ver: i64, mask: i64) -> QrCode {
```

**Parameters:**

- `segs`: `Vec<QrSegment>`
- `ecl`: `i64`
- `min_ver`: `i64`
- `max_ver`: `i64`
- `mask`: `i64`

**Returns:** `QrCode`

*Defined at line 780*

---

## qr_make_data

The data codewords of segments at a version and level: mode and count
headers, the terminator, bit padding and the 0xEC 0x11 pad bytes.

```fides
pub fn qr_make_data(segs: &Vec<QrSegment>, ver: i64, ecl: i64) -> Vec<i64> {
```

**Parameters:**

- `segs`: `&Vec<QrSegment>`
- `ver`: `i64`
- `ecl`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 802*

---

## qr_encode_codewords

Encode a symbol from its data codewords (padded to capacity).

```fides
pub fn qr_encode_codewords(data: Vec<i64>, ver: i64, ecl: i64, mask: i64) -> QrCode {
```

**Parameters:**

- `data`: `Vec<i64>`
- `ver`: `i64`
- `ecl`: `i64`
- `mask`: `i64`

**Returns:** `QrCode`

*Defined at line 853*

---

## qr_encode_text

Encode text with automatic mode and version.

```fides
pub fn qr_encode_text(text: str, ecl: i64) -> QrCode {
```

**Parameters:**

- `text`: `str`
- `ecl`: `i64`

**Returns:** `QrCode`

*Defined at line 888*

---

## qr_to_text

The matrix as text rows: '#' dark, '.' light.

```fides
pub fn qr_to_text(q: &QrCode) -> str {
```

**Parameters:**

- `q`: `&QrCode`

**Returns:** `str`

*Defined at line 895*

---

## QrDecoded

```fides
pub struct QrDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `version` | `i64` |
| `ecl` | `i64` |
| `mask` | `i64` |
| `text` | `str` |
| `bytes` | `Vec<i64>,  // the concatenated segment data` |
| `modes` | `Vec<i64>` |
| `bad_blocks` | `i64,  // blocks whose Reed-Solomon syndromes are not 0` |
| `eci` | `i64,  // last ECI assignment seen, -1 none` |

*Defined at line 916*

---

## qr_decode

Decode a clean module matrix (size * size, row-major, true = dark).

```fides
pub fn qr_decode(m: Vec<bool>, size: i64) -> QrDecoded {
```

**Parameters:**

- `m`: `Vec<bool>`
- `size`: `i64`

**Returns:** `QrDecoded`

*Defined at line 954*

---

## qr_parse_data

Parse the segments of data codewords at version `ver`.

```fides
pub fn qr_parse_data(data: Vec<i64>, ver: i64) -> QrDecoded {
```

**Parameters:**

- `data`: `Vec<i64>`
- `ver`: `i64`

**Returns:** `QrDecoded`

*Defined at line 1126*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
