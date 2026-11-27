[All modules](index.md)

# bits -- Fides Documentation

## Contents

- [bit_get](#bit_get) (function)
- [bit_set](#bit_set) (function)
- [bit_clear](#bit_clear) (function)
- [bit_toggle](#bit_toggle) (function)
- [bit_popcount](#bit_popcount) (function)
- [bit_leading_zeros](#bit_leading_zeros) (function)
- [bit_trailing_zeros](#bit_trailing_zeros) (function)
- [bit_highest_set](#bit_highest_set) (function)
- [bit_lowest_set](#bit_lowest_set) (function)
- [bit_rotate_left](#bit_rotate_left) (function)
- [bit_rotate_right](#bit_rotate_right) (function)
- [bit_byte_at](#bit_byte_at) (function)
- [bit_high_nibble](#bit_high_nibble) (function)
- [bit_low_nibble](#bit_low_nibble) (function)
- [bit_is_power_of_two](#bit_is_power_of_two) (function)
- [bit_next_power_of_two](#bit_next_power_of_two) (function)
- [bit_ceil](#bit_ceil) (function)
- [bit_log2](#bit_log2) (function)
- [bit_reverse_byte](#bit_reverse_byte) (function)
- [bit_reverse](#bit_reverse) (function)
- [bit_mask](#bit_mask) (function)
- [bit_mask_high](#bit_mask_high) (function)
- [bit_extract](#bit_extract) (function)
- [bit_insert](#bit_insert) (function)

## bit_get

Get the value of bit at position `idx` (0 or 1).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_get(5, 0), 1);
assert_eq(bit_get(5, 1), 0);
```

```fides
pub fn bit_get(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 19*

---

## bit_set

Set bit at position `idx` to 1. Returns the new value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_set(0, 3), 8);
```

```fides
pub fn bit_set(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 27*

---

## bit_clear

Set bit at position `idx` to 0 (clear). Returns the new value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_clear(15, 0), 14);
```

```fides
pub fn bit_clear(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 35*

---

## bit_toggle

Toggle bit at position `idx`. Returns the new value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_toggle(5, 1), 7);
assert_eq(bit_toggle(7, 1), 5);
```

```fides
pub fn bit_toggle(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 44*

---

## bit_popcount

```fides
pub fn bit_popcount(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 59*

---

## bit_leading_zeros

```fides
pub fn bit_leading_zeros(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## bit_trailing_zeros

```fides
pub fn bit_trailing_zeros(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 114*

---

## bit_highest_set

Find the position of the highest set bit. Returns -1 if n == 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_highest_set(10), 3);
assert_eq(bit_highest_set(0), -1);
```

```fides
pub fn bit_highest_set(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 126*

---

## bit_lowest_set

Find the position of the lowest set bit. Returns -1 if n == 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_lowest_set(12), 2);
```

```fides
pub fn bit_lowest_set(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## bit_rotate_left

Rotate bits left by k positions (64-bit).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_rotate_left(1, 4), 16);
assert_eq(bit_rotate_left(bit_rotate_right(12345, 7), 7), 12345);
```

```fides
pub fn bit_rotate_left(n: i64, k: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 158*

---

## bit_rotate_right

Rotate bits right by k positions (64-bit).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_rotate_right(16, 4), 1);
assert_eq(bit_rotate_right(1, 1), bit_set(0, 63));
```

```fides
pub fn bit_rotate_right(n: i64, k: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 171*

---

## bit_byte_at

Extract byte at position `idx` (0 = least significant byte).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_byte_at(0x1234, 1), 0x12);
```

```fides
pub fn bit_byte_at(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 185*

---

## bit_high_nibble

Extract the high nibble (upper 4 bits of the lowest byte).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_high_nibble(0xAB), 0xA);
```

```fides
pub fn bit_high_nibble(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 193*

---

## bit_low_nibble

Extract the low nibble (lower 4 bits).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_low_nibble(0xAB), 0xB);
```

```fides
pub fn bit_low_nibble(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 201*

---

## bit_is_power_of_two

Check if n is a power of two.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bit_is_power_of_two(64));
assert(!bit_is_power_of_two(65));
```

```fides
pub fn bit_is_power_of_two(n: i64) -> bool {
```

**Parameters:**

- `n`: `i64`

**Returns:** `bool`

*Defined at line 212*

---

## bit_next_power_of_two

Round up to the next power of two. Returns n if already a power of two.
Returns 0 if n == 0. Returns -1 if n > 2^62.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_next_power_of_two(100), 128);
assert_eq(bit_next_power_of_two(64), 64);
```

```fides
pub fn bit_next_power_of_two(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 225*

---

## bit_ceil

Round up to the next power of two (alias).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_ceil(5), 8);
```

```fides
pub fn bit_ceil(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 246*

---

## bit_log2

Floor log2 of n. Returns -1 if n <= 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_log2(1024), 10);
assert_eq(bit_log2(1000), 9);
```

```fides
pub fn bit_log2(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 255*

---

## bit_reverse_byte

Reverse the bits of a byte (8-bit reversal).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_reverse_byte(1), 128);
assert_eq(bit_reverse_byte(0xF0), 0x0F);
```

```fides
pub fn bit_reverse_byte(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 269*

---

## bit_reverse

Reverse all 64 bits.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_reverse(1), bit_set(0, 63));
```

```fides
pub fn bit_reverse(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 293*

---

## bit_mask

Create a mask of `count` least-significant 1-bits.
bit_mask(4) → 0b1111


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_mask(4), 15);
assert_eq(bit_mask(63), 9223372036854775807);
```

```fides
pub fn bit_mask(count: i64) -> i64 {
```

**Parameters:**

- `count`: `i64`

**Returns:** `i64`

*Defined at line 311*

---

## bit_mask_high

Create a mask of `count` most-significant 1-bits (in 64-bit space).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_mask_high(1), bit_set(0, 63));
assert_eq(bit_mask_high(63), -2);
```

```fides
pub fn bit_mask_high(count: i64) -> i64 {
```

**Parameters:**

- `count`: `i64`

**Returns:** `i64`

*Defined at line 326*

---

## bit_extract

Extract `bits` bits starting from position `offset`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_extract(0xABCD, 4, 8), 0xBC);
```

```fides
pub fn bit_extract(n: i64, offset: i64, bits: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `offset`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 340*

---

## bit_insert

Insert `bits` bits of `value` into `n` starting at position `offset`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bit_insert(0, 5, 4, 3), 80);
assert_eq(bit_insert(0xFF, 0, 0, 4), 0xF0);
```

```fides
pub fn bit_insert(n: i64, value: i64, offset: i64, bits: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `value`: `i64`
- `offset`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 349*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
