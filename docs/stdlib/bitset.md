[All modules](index.md)

# bitset -- Fides Documentation

## Contents

- [BitSet](#BitSet) (struct)
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
| `words` | `Vec<i64>` |
| `nbits` | `i64,  // logical size in bits` |

*Defined at line 20*

---

## bitset_new

A BitSet of `nbits` bits, all clear.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new(10);
bitset_set(&mut b, 3);
bitset_set(&mut b, 9);
bitset_set(&mut b, 99);
assert_eq(bitset_to_string(b), "0001000001");
assert_eq(bitset_count(b), 2);
```

```fides
pub fn bitset_new(nbits: i64) -> BitSet {
```

**Parameters:**

- `nbits`: `i64`

**Returns:** `BitSet`

*Defined at line 45*

---

## bitset_new_full

A BitSet of `nbits` bits, all set.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new_full(5);
assert(bitset_all(b));
assert_eq(bitset_count(b), 5);
```

```fides
pub fn bitset_new_full(nbits: i64) -> BitSet {
```

**Parameters:**

- `nbits`: `i64`

**Returns:** `BitSet`

*Defined at line 68*

---

## bitset_from_string

A BitSet from a "0101" string (char index = bit index).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_from_string("0110");
assert(bitset_get(b, 1));
assert(!bitset_get(b, 0));
```

```fides
pub fn bitset_from_string(s: str) -> BitSet {
```

**Parameters:**

- `s`: `str`

**Returns:** `BitSet`

*Defined at line 80*

---

## bitset_set

Set bit `i`. Out-of-range indices are ignored.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new(8);
bitset_set(&mut b, 7);
assert(bitset_get(b, 7));
```

```fides
pub fn bitset_set(bs: &mut BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `&mut BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 105*

---

## bitset_clear

Clear bit `i`. Out-of-range indices are ignored.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new_full(4);
bitset_clear(&mut b, 2);
assert_eq(bitset_to_string(b), "1101");
```

```fides
pub fn bitset_clear(bs: &mut BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `&mut BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 120*

---

## bitset_flip

Flip bit `i`. Out-of-range indices are ignored.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new(4);
bitset_flip(&mut b, 0);
bitset_flip(&mut b, 0);
bitset_flip(&mut b, 1);
assert_eq(bitset_to_string(b), "0100");
```

```fides
pub fn bitset_flip(bs: &mut BitSet, i: i64) -> void {
```

**Parameters:**

- `bs`: `&mut BitSet`
- `i`: `i64`

**Returns:** `void`

*Defined at line 142*

---

## bitset_get

Is bit `i` set? Out-of-range reads return false.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_from_string("10");
assert(bitset_get(b, 0));
assert(!bitset_get(b, 50));
```

```fides
pub fn bitset_get(bs: BitSet, i: i64) -> bool {
```

**Parameters:**

- `bs`: `BitSet`
- `i`: `i64`

**Returns:** `bool`

*Defined at line 157*

---

## bitset_fill_all

Set every bit in range.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new(3);
bitset_fill_all(&mut b);
assert_eq(bitset_to_string(b), "111");
```

```fides
pub fn bitset_fill_all(bs: &mut BitSet) -> void {
```

**Parameters:**

- `bs`: `&mut BitSet`

**Returns:** `void`

*Defined at line 174*

---

## bitset_clear_all

Clear every bit.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_new_full(3);
bitset_clear_all(&mut b);
assert(bitset_none(b));
```

```fides
pub fn bitset_clear_all(bs: &mut BitSet) -> void {
```

**Parameters:**

- `bs`: `&mut BitSet`

**Returns:** `void`

*Defined at line 188*

---

## bitset_count

Number of set bits.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bitset_count(bitset_from_string("10110")), 3);
```

```fides
pub fn bitset_count(bs: BitSet) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `i64`

*Defined at line 200*

---

## bitset_any

Any bit set?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bitset_any(bitset_from_string("001")));
assert(!bitset_any(bitset_new(64)));
```

```fides
pub fn bitset_any(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 217*

---

## bitset_all

All bits set?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(!bitset_all(bitset_from_string("101")));
```

```fides
pub fn bitset_all(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 225*

---

## bitset_none

No bits set?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bitset_none(bitset_from_string("000")));
```

```fides
pub fn bitset_none(bs: BitSet) -> bool {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `bool`

*Defined at line 233*

---

## bitset_next_set

Index of the first set bit at or after `from`, or -1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = bitset_from_string("0110");
assert_eq(bitset_next_set(b, 0), 1);
assert_eq(bitset_next_set(b, 3), -1);
```

```fides
pub fn bitset_next_set(bs: BitSet, from: i64) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`
- `from`: `i64`

**Returns:** `i64`

*Defined at line 243*

---

## bitset_next_clear

Index of the first clear bit at or after `from`, or -1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bitset_next_clear(bitset_from_string("0110"), 1), 3);
```

```fides
pub fn bitset_next_clear(bs: BitSet, from: i64) -> i64 {
```

**Parameters:**

- `bs`: `BitSet`
- `from`: `i64`

**Returns:** `i64`

*Defined at line 261*

---

## bitset_and

a AND b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = bitset_and(bitset_from_string("1100"), bitset_from_string("1010"));
assert_eq(bitset_to_string(c), "1000");
```

```fides
pub fn bitset_and(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 288*

---

## bitset_or

a OR b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = bitset_or(bitset_from_string("1100"), bitset_from_string("1010"));
assert_eq(bitset_to_string(c), "1110");
```

```fides
pub fn bitset_or(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 304*

---

## bitset_xor

a XOR b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = bitset_xor(bitset_from_string("1100"), bitset_from_string("1010"));
assert_eq(bitset_to_string(c), "0110");
```

```fides
pub fn bitset_xor(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 320*

---

## bitset_andnot

a AND (NOT b) — the bits in a that are not in b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = bitset_andnot(bitset_from_string("1100"), bitset_from_string("1010"));
assert_eq(bitset_to_string(c), "0100");
```

```fides
pub fn bitset_andnot(a: BitSet, b: BitSet) -> BitSet {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `BitSet`

*Defined at line 336*

---

## bitset_intersects

Do a and b share at least one set bit?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bitset_intersects(bitset_from_string("1100"), bitset_from_string("0110")));
assert(!bitset_intersects(bitset_from_string("1000"), bitset_from_string("0001")));
```

```fides
pub fn bitset_intersects(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 355*

---

## bitset_contains_all

Every set bit of b is also set in a?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bitset_contains_all(bitset_from_string("1110"), bitset_from_string("0110")));
```

```fides
pub fn bitset_contains_all(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 371*

---

## bitset_equals

Same bits set in both?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bitset_equals(bitset_from_string("101"), bitset_from_string("101")));
```

```fides
pub fn bitset_equals(a: BitSet, b: BitSet) -> bool {
```

**Parameters:**

- `a`: `BitSet`
- `b`: `BitSet`

**Returns:** `bool`

*Defined at line 389*

---

## bitset_clone

A copy of the BitSet.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = bitset_from_string("10");
var b = bitset_clone(a);
bitset_set(&mut b, 1);
assert_eq(bitset_to_string(a), "10");
assert_eq(bitset_to_string(b), "11");
```

```fides
pub fn bitset_clone(bs: BitSet) -> BitSet {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `BitSet`

*Defined at line 411*

---

## bitset_not

Bitwise NOT within the set's size (bits past nbits stay clear).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bitset_to_string(bitset_not(bitset_from_string("0110"))), "1001");
```

```fides
pub fn bitset_not(bs: BitSet) -> BitSet {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `BitSet`

*Defined at line 425*

---

## bitset_to_string

"01011..." — one char per bit in index order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bitset_to_string(bitset_new(3)), "000");
```

```fides
pub fn bitset_to_string(bs: BitSet) -> str {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `str`

*Defined at line 443*

---

## bitset_to_vec

All set indices as a Vec<i64>, ascending.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = bitset_to_vec(bitset_from_string("0101"));
assert_eq(v.len(), 2);
assert_eq(v.get(1), 3);
```

```fides
pub fn bitset_to_vec(bs: BitSet) -> Vec<i64> {
```

**Parameters:**

- `bs`: `BitSet`

**Returns:** `Vec<i64>`

*Defined at line 463*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
