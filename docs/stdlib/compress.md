[All modules](index.md)

# compress -- Fides Documentation

## Contents

- [crc32_bytes](#crc32_bytes) (function)
- [crc32](#crc32) (function)
- [adler32_bytes](#adler32_bytes) (function)
- [adler32](#adler32) (function)
- [deflate_stored](#deflate_stored) (function)
- [zlib_compress_bytes](#zlib_compress_bytes) (function)
- [zlib_compress](#zlib_compress) (function)
- [gzip_compress_bytes](#gzip_compress_bytes) (function)
- [gzip_compress](#gzip_compress) (function)
- [inflate_from](#inflate_from) (function)
- [inflate](#inflate) (function)
- [zlib_decompress](#zlib_decompress) (function)
- [gzip_decompress](#gzip_decompress) (function)
- [zlib_decompress_str](#zlib_decompress_str) (function)
- [gzip_decompress_str](#gzip_decompress_str) (function)
- [deflate_compress](#deflate_compress) (function)
- [zlib_compress_level](#zlib_compress_level) (function)
- [gzip_compress_level](#gzip_compress_level) (function)

## crc32_bytes

CRC-32 of a byte Vec.

```fides
pub fn crc32_bytes(bytes: Vec) -> i64 {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `i64`

*Defined at line 47*

---

## crc32

CRC-32 of a string's bytes.

```fides
pub fn crc32(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 59*

---

## adler32_bytes

Adler-32 of a byte Vec.

```fides
pub fn adler32_bytes(bytes: Vec) -> i64 {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `i64`

*Defined at line 70*

---

## adler32

Adler-32 of a string's bytes.

```fides
pub fn adler32(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 83*

---

## deflate_stored

DEFLATE bitstream (stored blocks only) for `bytes`.
Chunked into ≤65535-byte blocks; BTYPE=00.

```fides
pub fn deflate_stored(bytes: Vec) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `Vec`

*Defined at line 109*

---

## zlib_compress_bytes

zlib-compress a byte Vec (deflate stored + zlib header + adler32).

```fides
pub fn zlib_compress_bytes(bytes: Vec) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `Vec`

*Defined at line 144*

---

## zlib_compress

zlib-compress a string's bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(crc32("hello"), 907060870);
assert_eq(adler32("abc"), 38600999);
assert_eq(zlib_decompress_str(zlib_compress("hello, zlib")), "hello, zlib");
assert_eq(gzip_decompress_str(gzip_compress("hello, gzip")), "hello, gzip");
```

```fides
pub fn zlib_compress(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 173*

---

## gzip_compress_bytes

gzip-compress a byte Vec (deflate stored + gzip header + crc32/isize).

```fides
pub fn gzip_compress_bytes(bytes: Vec) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `Vec`

*Defined at line 186*

---

## gzip_compress

gzip-compress a string's bytes.

```fides
pub fn gzip_compress(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 208*

---

## inflate_from

Inflate a raw DEFLATE stream starting at byte `start` of `data`.
`max_out` caps the output size (decompression-bomb guard); pass a
generous limit like 268435456 (256MB). Returns Vec<i64> of bytes
(empty on error: a truncated or corrupt stream, or more than
`max_out` bytes of output).

```fides
pub fn inflate_from(data: Vec, start: i64, max_out: i64) -> Vec {
```

**Parameters:**

- `data`: `Vec`
- `start`: `i64`
- `max_out`: `i64`

**Returns:** `Vec`

*Defined at line 751*

---

## inflate

Inflate a raw DEFLATE stream (whole Vec).

```fides
pub fn inflate(data: Vec) -> Vec {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `Vec`

*Defined at line 756*

---

## zlib_decompress

Decompress a zlib stream (validates header, skips optional dict id,
checks the Adler-32). Empty on any error.

```fides
pub fn zlib_decompress(data: Vec) -> Vec {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `Vec`

*Defined at line 769*

---

## gzip_decompress

Decompress a gzip member (parses header incl. FNAME/FCOMMENT/FEXTRA,
checks the CRC-32 and ISIZE trailer). Empty on any error.

```fides
pub fn gzip_decompress(data: Vec) -> Vec {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `Vec`

*Defined at line 801*

---

## zlib_decompress_str

Decompress and interpret the result as a string.

```fides
pub fn zlib_decompress_str(data: Vec) -> str {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `str`

*Defined at line 854*

---

## gzip_decompress_str

```fides
pub fn gzip_decompress_str(data: Vec) -> str {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `str`

*Defined at line 858*

---

## deflate_compress

Compress `bytes` into a raw DEFLATE stream (RFC 1951). `level` 0 is
stored blocks only (`deflate_stored`); 1-9 trade speed for size as in
zlib (1-3 greedy matching, 4-9 lazy, longer hash chains as the level
rises). Each block is written as dynamic Huffman, fixed Huffman or
stored, whichever is smallest.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var text: Vec = Vec::new();
var i: i64 = 0;
while i < 600 { text.push(97 + (i % 3)); i = i + 1; }
var z: Vec = deflate_compress(text, 6);
assert(z.len() < 30);
assert_eq(inflate(z).len(), 600);
```

```fides
pub fn deflate_compress(bytes: Vec, level: i64) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`
- `level`: `i64`

**Returns:** `Vec`

*Defined at line 1494*

---

## zlib_compress_level

zlib stream (RFC 1950) of `bytes` compressed at `level` (0-9).

```fides
pub fn zlib_compress_level(bytes: Vec, level: i64) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`
- `level`: `i64`

**Returns:** `Vec`

*Defined at line 1537*

---

## gzip_compress_level

gzip member (RFC 1952) of `bytes` compressed at `level` (0-9).

```fides
pub fn gzip_compress_level(bytes: Vec, level: i64) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`
- `level`: `i64`

**Returns:** `Vec`

*Defined at line 1561*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
