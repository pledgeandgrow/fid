[All modules](index.md)

# zstd -- Fides Documentation

## Contents

- [ZstdResult](#ZstdResult) (struct)
- [zstd_xxh64_range](#zstd_xxh64_range) (function)
- [zstd_xxh64](#zstd_xxh64) (function)
- [zstd_decode_limit](#zstd_decode_limit) (function)
- [zstd_decode](#zstd_decode) (function)
- [zstd_decompress](#zstd_decompress) (function)
- [zstd_decompress_limit](#zstd_decompress_limit) (function)
- [zstd_ok](#zstd_ok) (function)
- [zstd_error](#zstd_error) (function)
- [zstd_decompress_str](#zstd_decompress_str) (function)
- [zstd_compress_level](#zstd_compress_level) (function)
- [zstd_compress](#zstd_compress) (function)
- [zstd_compress_str](#zstd_compress_str) (function)

## ZstdResult

The outcome of decoding a Zstandard stream. `ok` is false on malformed
input; `bytes` is then empty and `error` says what was wrong. A valid
stream whose content is empty gives ok == true and empty `bytes`.

```fides
pub struct ZstdResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `bytes` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 47*

---

## zstd_xxh64_range

```fides
pub fn zstd_xxh64_range(d: Vec<i64>, from: i64, to: i64, seed: i64) -> i64 {
```

**Parameters:**

- `d`: `Vec<i64>`
- `from`: `i64`
- `to`: `i64`
- `seed`: `i64`

**Returns:** `i64`

*Defined at line 1111*

---

## zstd_xxh64

XXH64 of a whole byte Vec with `seed` (see zstd_xxh64_range).

```fides
pub fn zstd_xxh64(bytes: Vec<i64>, seed: i64) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `seed`: `i64`

**Returns:** `i64`

*Defined at line 1161*

---

## zstd_decode_limit

Decode a Zstandard stream (one or more frames, skippable frames
skipped) into at most `max_out` bytes. On malformed input, or output
over `max_out`, the result has ok == false, empty bytes and a message.

```fides
pub fn zstd_decode_limit(data: Vec<i64>, max_out: i64) -> ZstdResult {
```

**Parameters:**

- `data`: `Vec<i64>`
- `max_out`: `i64`

**Returns:** `ZstdResult`

*Defined at line 1379*

---

## zstd_decode

Decode a Zstandard stream (output capped at 256 MB). See ZstdResult.

```fides
pub fn zstd_decode(data: Vec<i64>) -> ZstdResult {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `ZstdResult`

*Defined at line 1438*

---

## zstd_decompress

Decompress a Zstandard stream. Returns the decoded bytes, or an empty
Vec on malformed input; use zstd_ok or zstd_decode to tell an error
from a stream whose content is empty.

```fides
pub fn zstd_decompress(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1445*

---

## zstd_decompress_limit

Decompress with the output capped at `max_out` bytes (a
decompression-bomb guard). Empty Vec on error or over the cap.

```fides
pub fn zstd_decompress_limit(data: Vec<i64>, max_out: i64) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`
- `max_out`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1452*

---

## zstd_ok

Whether `data` is a valid Zstandard stream (it is decoded in full to
find out; to keep the bytes as well, call zstd_decode once).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
// one frame: single segment, content size 2, one raw last block "hi"
var f: Vec<i64> = Vec::new();
f.push(0x28); f.push(0xB5); f.push(0x2F); f.push(0xFD);
f.push(0x20); f.push(0x02);
f.push(0x11); f.push(0x00); f.push(0x00);
f.push(0x68); f.push(0x69);
assert(zstd_ok(f));
assert_eq(zstd_error(f), "");
assert_eq(zstd_decompress_str(f), "hi");
assert_eq(zstd_decompress(f).len(), 2);
var bad: Vec<i64> = Vec::new();
bad.push(1); bad.push(2);
assert(!zstd_ok(bad));
assert_eq(zstd_xxh64(Vec::new(), 0), -1205034819632174695);
```

```fides
pub fn zstd_ok(data: Vec<i64>) -> bool {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1475*

---

## zstd_error

Why `data` does not decode, or "" when it does.

```fides
pub fn zstd_error(data: Vec<i64>) -> str {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1481*

---

## zstd_decompress_str

Decompress and interpret the result as a string ("" on error). The
string ends at the first NUL byte, as every Fides str does.

```fides
pub fn zstd_decompress_str(data: Vec<i64>) -> str {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1488*

---

## zstd_compress_level

Compress `bytes` (values 0..255; higher bits are dropped) into one
Zstandard frame with the content size and checksum. `level` 0 only
stores (raw and RLE blocks); 1 to 22 search for matches, harder at
higher levels (3 is zstd_compress's). Any Zstandard decoder reads the
result; the ratio is below the zstd tool's at the same level.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
var i: i64 = 0;
while i < 200 { b.push(97 + i % 3); i = i + 1; }
val z: Vec<i64> = zstd_compress_level(b, 3);
assert(z.len() < 40);
assert(zstd_ok(z));
assert_eq(zstd_decompress(z).len(), 200);
assert_eq(str_len(zstd_decompress_str(zstd_compress_level(b, 0))), 200);
```

```fides
pub fn zstd_compress_level(bytes: Vec<i64>, level: i64) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `level`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2366*

---

## zstd_compress

Compress `bytes` into one Zstandard frame at level 3 (see
zstd_compress_level).

```fides
pub fn zstd_compress(bytes: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2485*

---

## zstd_compress_str

Compress the bytes of a string at level 3; zstd_decompress_str gives
the string back.

```fides
pub fn zstd_compress_str(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 2491*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
