# bitset — Fides Documentation

## Contents

- [BitSet](#bitset) (struct)
- [bitset_new](#bitset_new) (function)
- [bitset_new_full](#bitset_new_full) (function)
- [bitset_from_string](#bitset_from_string) (function)
- [bitset_set](#bitset_set) (function)
- [bitset_clear](#bitset_clear) (function)
- [bitset_flip](#bitset_flip) (function)
- [bitset_get](#bitset_get) (function)
- [bitset_fill_all](#bitset_fill_all) (function)
- [bitset_clear_all](#bitset_clear_all) (function)
- [bitset_count](#bitset_count) (function)
- [bitset_any](#bitset_any) (function)
- [bitset_all](#bitset_all) (function)
- [bitset_none](#bitset_none) (function)
- [bitset_next_set](#bitset_next_set) (function)
- [bitset_next_clear](#bitset_next_clear) (function)
- [bitset_and](#bitset_and) (function)
- [bitset_or](#bitset_or) (function)
- [bitset_xor](#bitset_xor) (function)
- [bitset_andnot](#bitset_andnot) (function)
- [bitset_intersects](#bitset_intersects) (function)
- [bitset_contains_all](#bitset_contains_all) (function)
- [bitset_equals](#bitset_equals) (function)
- [bitset_clone](#bitset_clone) (function)
- [bitset_not](#bitset_not) (function)
- [bitset_to_string](#bitset_to_string) (function)
- [bitset_to_vec](#bitset_to_vec) (function)

## BitSet

A set of bits. `words` holds BITS_PER_WORD bits per element.

```fides
pub struct BitSet {
```

**Fields:**

| Name | Type |
|------|------|
| `words` | `Vec,    // Vec<i64>, each element holds 63 bits` |
| `nbits` | `i64,    // logical size in bits` |

*Defined at line 23*

---

## bitset_new

A BitSet of `nbits` bits, all clear.

```fides
pub fn bitset_new(nbits: i64) -> BitSet {
```

**Parameters:**

- `nbits`: `i64`

**Returns:** `BitSet`

*Defined at line 37*

---

## bitset_new_full

A BitSet of `nbits` bits, all set.

```fides
pub fn bitset_new_full(nbits: i64) -> BitSet {
```

**Parameters:**

- `nbits`: `i64`

**Returns:** `BitSet`

*Defined at line 51*

---

## bitset_from_string

A BitSet from a "0101" string (char index = bit index).

```fides
pub fn bitset_from_string(s: str) -> BitSet {
```

**Parameters:**

- `s`: `str`

**Returns:** `BitSet`

*Defined at line 58*

---

## bitset_set

Set bit `i`. Out-of-range indices are ignored.

```fides
pub fn bitset_set(bs: BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 77*

---

## bitset_clear

Clear bit `i`. Out-of-range indices are ignored.

```fides
pub fn bitset_clear(bs: BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 85*

---

## bitset_flip

Flip bit `i`. Out-of-range indices are ignored.

```fides
pub fn bitset_flip(bs: BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 97*

---

## bitset_get

Is bit `i` set? Out-of-range reads return false.

```fides
pub fn bitset_get(bs: BitSet, i: i64) -> bool {
```

**Parameters:**

- `bs`: `BitSet`
- `i`: `i64`

**Returns:** `bool`

*Defined at line 105*

---

## bitset_fill_all

Set every bit in range.

```fides
pub fn bitset_fill_all(bs: BitSet) -> void {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `void`

*Defined at line 115*

---

## bitset_clear_all

Clear every bit.

```fides
pub fn bitset_clear_all(bs: BitSet) -> void {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `void`

*Defined at line 124*

---

## bitset_count

Number of set bits.

```fides
pub fn bitset_count(bs: BitSet) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `i64`

*Defined at line 133*

---

## bitset_any

Any bit set?

```fides
pub fn bitset_any(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 144*

---

## bitset_all

All bits set?

```fides
pub fn bitset_all(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 149*

---

## bitset_none

No bits set?

```fides
pub fn bitset_none(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 154*

---

## bitset_next_set

Index of the first set bit at or after `from`, or -1.

```fides
pub fn bitset_next_set(bs: BitSet, from: i64) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`
- `from`: `i64`

**Returns:** `i64`

*Defined at line 159*

---

## bitset_next_clear

Index of the first clear bit at or after `from`, or -1.

```fides
pub fn bitset_next_clear(bs: BitSet, from: i64) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`
- `from`: `i64`

**Returns:** `i64`

*Defined at line 170*

---

## bitset_and

a AND b.

```fides
pub fn bitset_and(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 189*

---

## bitset_or

a OR b.

```fides
pub fn bitset_or(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 201*

---

## bitset_xor

a XOR b.

```fides
pub fn bitset_xor(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 213*

---

## bitset_andnot

a AND (NOT b) — the bits in a that are not in b.

```fides
pub fn bitset_andnot(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 225*

---

## bitset_intersects

Do a and b share at least one set bit?

```fides
pub fn bitset_intersects(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 239*

---

## bitset_contains_all

Every set bit of b is also set in a?

```fides
pub fn bitset_contains_all(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 250*

---

## bitset_equals

Same bits set in both?

```fides
pub fn bitset_equals(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 263*

---

## bitset_clone

A copy of the BitSet.

```fides
pub fn bitset_clone(bs: BitSet) -> BitSet {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `BitSet`

*Defined at line 274*

---

## bitset_not

Bitwise NOT within the set's size (bits past nbits stay clear).

```fides
pub fn bitset_not(bs: BitSet) -> BitSet {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `BitSet`

*Defined at line 285*

---

## bitset_to_string

"01011..." — one char per bit in index order.

```fides
pub fn bitset_to_string(bs: BitSet) -> str {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `str`

*Defined at line 298*

---

## bitset_to_vec

All set indices as a Vec<i64>, ascending.

```fides
pub fn bitset_to_vec(bs: BitSet) -> Vec {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `Vec`

*Defined at line 313*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
