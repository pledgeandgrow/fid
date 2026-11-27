[All modules](index.md)

# hash -- Fides Documentation

## Contents

- [hash_str](#hash_str) (function)
- [hash_bytes](#hash_bytes) (function)
- [hash_i64](#hash_i64) (function)
- [hash_combine](#hash_combine) (function)
- [hash_vec](#hash_vec) (function)
- [hash_djb2](#hash_djb2) (function)
- [hash_djb2_bytes](#hash_djb2_bytes) (function)
- [hash_checksum](#hash_checksum) (function)
- [hash_xor_checksum](#hash_xor_checksum) (function)
- [hash_crc32](#hash_crc32) (function)
- [hash_crc32_str](#hash_crc32_str) (function)
- [hash64_str](#hash64_str) (function)
- [hash64_bytes](#hash64_bytes) (function)
- [hash64_i64](#hash64_i64) (function)
- [hash_mix64](#hash_mix64) (function)
- [hash64_combine](#hash64_combine) (function)
- [Hasher](#Hasher) (struct)
- [Hasher](#Hasher) (impl)

## hash_str

FNV-1a hash of a string. 32-bit (avoids i64 overflow).
Fast, good distribution, widely used for hash tables.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hash_str(""), 2166136261);
assert_eq(hash_str("a"), 3826002220);
assert(hash_str("abc") != hash_str("abd"));
```

```fides
pub fn hash_str(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 22*

---

## hash_bytes

FNV-1a hash of a Vec of bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(97);
assert_eq(hash_bytes(b), hash_str("a"));
```

```fides
pub fn hash_bytes(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 41*

---

## hash_i64

FNV-1a hash of an i64.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hash_i64(42), hash_i64(42));
assert(hash_i64(1) != hash_i64(2));
```

```fides
pub fn hash_i64(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 57*

---

## hash_combine

Combine two hash values (for hashing tuples/structs).
Boost-style combine: order-sensitive, and equal pairs do not cancel.
a and b are treated as 32-bit; the result stays 32-bit.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var h = hash_combine(hash_str("x"), hash_i64(7));
assert_eq(h, hash_combine(hash_str("x"), hash_i64(7)));
assert(hash_combine(1, 2) != hash_combine(2, 1));
assert(hash_combine(7, 7) != 0);
```

```fides
pub fn hash_combine(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 79*

---

## hash_vec

Hash a Vec of i64 values by combining their hashes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a: Vec<i64> = Vec::new();
a.push(1); a.push(2);
var b: Vec<i64> = Vec::new();
b.push(1); b.push(2);
assert_eq(hash_vec(a), hash_vec(b));
```

```fides
pub fn hash_vec(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 93*

---

## hash_djb2

DJB2 hash of a string. Simple, fast, good for small hash tables.
Kept modulo 2^32 so it never overflows i64 arithmetic.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hash_djb2(""), 5381);
assert_eq(hash_djb2("a"), 177670);
```

```fides
pub fn hash_djb2(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 111*

---

## hash_djb2_bytes

DJB2 hash of a Vec of bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(97);
assert_eq(hash_djb2_bytes(b), hash_djb2("a"));
```

```fides
pub fn hash_djb2_bytes(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 127*

---

## hash_checksum

Simple sum checksum of a Vec of bytes. NOT a real checksum.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(1); b.push(2); b.push(3);
assert_eq(hash_checksum(b), 6);
```

```fides
pub fn hash_checksum(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 145*

---

## hash_xor_checksum

XOR checksum of a Vec of bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(15); b.push(3);
assert_eq(hash_xor_checksum(b), 12);
```

```fides
pub fn hash_xor_checksum(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 161*

---

## hash_crc32

CRC32 checksum of a Vec of bytes.
Uses the standard CRC-32 polynomial (0xEDB88320).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(97);
assert_eq(hash_crc32(b), 3904355907);
```

```fides
pub fn hash_crc32(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 180*

---

## hash_crc32_str

CRC32 checksum of a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = hash_crc32_str("123456789");
println(int_to_str(c));
assert_eq(c, 3421780262);
```

```fides
pub fn hash_crc32_str(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 205*

---

## hash64_str

64-bit FNV-1a hash of a string (wraps i64; result can be negative).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hash64_str(""), -3750763034362895579);
assert(hash64_str("abc") != hash64_str("abd"));
```

```fides
pub fn hash64_str(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 224*

---

## hash64_bytes

64-bit FNV-1a hash of a Vec of bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(97);
assert_eq(hash64_bytes(b), hash64_str("a"));
```

```fides
pub fn hash64_bytes(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 240*

---

## hash64_i64

64-bit FNV-1a hash of an i64 (all 8 bytes).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hash64_i64(42), hash64_i64(42));
assert(hash64_i64(1) != hash64_i64(2));
```

```fides
pub fn hash64_i64(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 255*

---

## hash_mix64

```fides
pub fn hash_mix64(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 272*

---

## hash64_combine

```fides
pub fn hash64_combine(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 284*

---

## Hasher

Incremental 64-bit FNV-1a hasher: feed bytes in any chunk sizes and the
result equals hashing the concatenation.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var h = Hasher::new();
h.update_str("he");
h.update_str("llo");
assert_eq(h.digest(), hash64_str("hello"));
```

```fides
pub struct Hasher {
```

**Fields:**

| Name | Type |
|------|------|
| `h` | `i64` |

*Defined at line 298*

---

## Hasher

```fides
impl Hasher {
```

**Methods:**

### new

An empty 64-bit hasher.

```fides
pub fn new() -> Hasher {
```

### update

Feed a Vec of bytes.

```fides
pub fn update(self: &mut Hasher, bytes: Vec<i64>) -> void {
```

### update_byte

Feed one byte.

```fides
pub fn update_byte(self: &mut Hasher, b: i64) -> void {
```

### update_i64

Feed all 8 bytes of an i64.

```fides
pub fn update_i64(self: &mut Hasher, n: i64) -> void {
```

### update_str

Feed a string's bytes.

```fides
pub fn update_str(self: &mut Hasher, s: str) -> void {
```

### digest

The current 64-bit hash value.

```fides
pub fn digest(self: &Hasher) -> i64 {
```

### reset

Reset to the empty state.

```fides
pub fn reset(self: &mut Hasher) -> void {
```

*Defined at line 302*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
