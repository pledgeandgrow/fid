# bits — Fides Documentation

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

```fides
pub fn bit_get(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 16*

---

## bit_set

Set bit at position `idx` to 1. Returns the new value.

```fides
pub fn bit_set(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 21*

---

## bit_clear

Set bit at position `idx` to 0 (clear). Returns the new value.

```fides
pub fn bit_clear(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 26*

---

## bit_toggle

Toggle bit at position `idx`. Returns the new value.

```fides
pub fn bit_toggle(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 31*

---

## bit_popcount

Count the number of 1-bits in n (population count / Hamming weight).

```fides
pub fn bit_popcount(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 38*

---

## bit_leading_zeros

Count leading zeros (number of 0-bits before the most significant 1).

```fides
pub fn bit_leading_zeros(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 49*

---

## bit_trailing_zeros

Count trailing zeros (number of 0-bits after the least significant 1).

```fides
pub fn bit_trailing_zeros(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 62*

---

## bit_highest_set

Find the position of the highest set bit. Returns -1 if n == 0.

```fides
pub fn bit_highest_set(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 74*

---

## bit_lowest_set

Find the position of the lowest set bit. Returns -1 if n == 0.

```fides
pub fn bit_lowest_set(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 87*

---

## bit_rotate_left

Rotate bits left by k positions (64-bit).

```fides
pub fn bit_rotate_left(n: i64, k: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 95*

---

## bit_rotate_right

Rotate bits right by k positions (64-bit).

```fides
pub fn bit_rotate_right(n: i64, k: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 102*

---

## bit_byte_at

Extract byte at position `idx` (0 = least significant byte).

```fides
pub fn bit_byte_at(n: i64, idx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 111*

---

## bit_high_nibble

Extract the high nibble (upper 4 bits of the lowest byte).

```fides
pub fn bit_high_nibble(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 116*

---

## bit_low_nibble

Extract the low nibble (lower 4 bits).

```fides
pub fn bit_low_nibble(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 121*

---

## bit_is_power_of_two

Check if n is a power of two.

```fides
pub fn bit_is_power_of_two(n: i64) -> bool {
```

**Parameters:**

- `n`: `i64`

**Returns:** `bool`

*Defined at line 128*

---

## bit_next_power_of_two

Round up to the next power of two. Returns n if already a power of two.
Returns 0 if n == 0. Returns -1 if n > 2^62.

```fides
pub fn bit_next_power_of_two(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 135*

---

## bit_ceil

Round up to the next power of two (alias).

```fides
pub fn bit_ceil(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 147*

---

## bit_log2

Floor log2 of n. Returns -1 if n <= 0.

```fides
pub fn bit_log2(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 152*

---

## bit_reverse_byte

Reverse the bits of a byte (8-bit reversal).

```fides
pub fn bit_reverse_byte(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 160*

---

## bit_reverse

Reverse all 64 bits.

```fides
pub fn bit_reverse(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 181*

---

## bit_mask

Create a mask of `count` least-significant 1-bits.
bit_mask(4) → 0b1111

```fides
pub fn bit_mask(count: i64) -> i64 {
```

**Parameters:**

- `count`: `i64`

**Returns:** `i64`

*Defined at line 195*

---

## bit_mask_high

Create a mask of `count` most-significant 1-bits (in 64-bit space).

```fides
pub fn bit_mask_high(count: i64) -> i64 {
```

**Parameters:**

- `count`: `i64`

**Returns:** `i64`

*Defined at line 202*

---

## bit_extract

Extract `bits` bits starting from position `offset`.

```fides
pub fn bit_extract(n: i64, offset: i64, bits: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `offset`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 209*

---

## bit_insert

Insert `bits` bits of `value` into `n` starting at position `offset`.

```fides
pub fn bit_insert(n: i64, value: i64, offset: i64, bits: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `value`: `i64`
- `offset`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 214*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
