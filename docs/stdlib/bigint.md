[All modules](index.md)

# bigint -- Fides Documentation

## Contents

- [BigInt](#BigInt) (struct)
- [BigDiv](#BigDiv) (struct)
- [bigint_zero](#bigint_zero) (function)
- [bigint_one](#bigint_one) (function)
- [bigint_from_i64](#bigint_from_i64) (function)
- [bigint_is_zero](#bigint_is_zero) (function)
- [bigint_from_str](#bigint_from_str) (function)
- [bigint_to_str](#bigint_to_str) (function)
- [bigint_to_hex](#bigint_to_hex) (function)
- [bigint_to_i64](#bigint_to_i64) (function)
- [bigint_cmp](#bigint_cmp) (function)
- [bigint_eq](#bigint_eq) (function)
- [bigint_lt](#bigint_lt) (function)
- [bigint_gt](#bigint_gt) (function)
- [bigint_add](#bigint_add) (function)
- [bigint_sub](#bigint_sub) (function)
- [bigint_neg](#bigint_neg) (function)
- [bigint_abs](#bigint_abs) (function)
- [bigint_mul](#bigint_mul) (function)
- [bigint_mul_i64](#bigint_mul_i64) (function)
- [bigint_add_i64](#bigint_add_i64) (function)
- [bigint_divmod](#bigint_divmod) (function)
- [bigint_div](#bigint_div) (function)
- [bigint_mod](#bigint_mod) (function)
- [bigint_pow](#bigint_pow) (function)
- [bigint_modpow](#bigint_modpow) (function)

## BigInt

A big integer: little-endian base-1e9 limbs + sign.
Invariant: no trailing zero limbs (except the value 0 → limbs [0]).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = bigint_from_str("123456789012345678901234567890");
var b = bigint_mul(a, a);
println(bigint_to_str(b));
assert_eq(bigint_to_str(b), "15241578753238836750495351562536198787501905199875019052100");
```

```fides
pub struct BigInt {
```

**Fields:**

| Name | Type |
|------|------|
| `limbs` | `Vec` |
| `neg` | `bool` |

*Defined at line 28*

---

## BigDiv

Result of bigint_divmod: quotient and remainder.

```fides
pub struct BigDiv {
```

**Fields:**

| Name | Type |
|------|------|
| `q` | `BigInt` |
| `r` | `BigInt` |

*Defined at line 35*

---

## bigint_zero

The BigInt 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bigint_is_zero(bigint_zero()));
```

```fides
pub fn bigint_zero() -> BigInt {
```

**Returns:** `BigInt`

*Defined at line 54*

---

## bigint_one

The BigInt 1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_i64(bigint_one()), 1);
```

```fides
pub fn bigint_one() -> BigInt {
```

**Returns:** `BigInt`

*Defined at line 66*

---

## bigint_from_i64

The BigInt holding `x`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_from_i64(-42)), "-42");
```

```fides
pub fn bigint_from_i64(x: i64) -> BigInt {
```

**Parameters:**

- `x`: `i64`

**Returns:** `BigInt`

*Defined at line 78*

---

## bigint_is_zero

Whether the value is 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bigint_is_zero(bigint_sub(bigint_from_i64(5), bigint_from_i64(5))));
```

```fides
pub fn bigint_is_zero(a: BigInt) -> bool {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `bool`

*Defined at line 110*

---

## bigint_from_str

Parse a decimal string (optional leading '-'). Invalid input → 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var n = bigint_from_str("-1000000000000");
assert_eq(bigint_to_str(n), "-1000000000000");
```

```fides
pub fn bigint_from_str(s: str) -> BigInt {
```

**Parameters:**

- `s`: `str`

**Returns:** `BigInt`

*Defined at line 121*

---

## bigint_to_str

Decimal string form.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_pow(bigint_from_i64(2), 64)), "18446744073709551616");
```

```fides
pub fn bigint_to_str(a: BigInt) -> str {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `str`

*Defined at line 150*

---

## bigint_to_hex

Hex string form ("0x..." or "-0x...").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_hex(bigint_from_i64(255)), "0xff");
assert_eq(bigint_to_hex(bigint_from_i64(-16)), "-0x10");
```

```fides
pub fn bigint_to_hex(a: BigInt) -> str {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `str`

*Defined at line 171*

---

## bigint_to_i64

Lossy small-int conversion (clamps into i64 range naturally via mod).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_i64(bigint_from_str("9000000000")), 9000000000);
```

```fides
pub fn bigint_to_i64(a: BigInt) -> i64 {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `i64`

*Defined at line 196*

---

## bigint_cmp

Compare a and b: -1, 0, 1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_cmp(bigint_from_i64(-5), bigint_from_i64(3)), -1);
assert_eq(bigint_cmp(bigint_from_i64(7), bigint_from_i64(7)), 0);
```

```fides
pub fn bigint_cmp(a: BigInt, b: BigInt) -> i64 {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `i64`

*Defined at line 240*

---

## bigint_eq

Whether a == b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bigint_eq(bigint_from_str("10"), bigint_from_i64(10)));
```

```fides
pub fn bigint_eq(a: BigInt, b: BigInt) -> bool {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `bool`

*Defined at line 264*

---

## bigint_lt

Whether a < b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bigint_lt(bigint_from_i64(-1), bigint_zero()));
```

```fides
pub fn bigint_lt(a: BigInt, b: BigInt) -> bool {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `bool`

*Defined at line 271*

---

## bigint_gt

Whether a > b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(bigint_gt(bigint_from_str("100000000000000000000"), bigint_from_i64(1)));
```

```fides
pub fn bigint_gt(a: BigInt, b: BigInt) -> bool {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `bool`

*Defined at line 278*

---

## bigint_add

a + b


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = bigint_add(bigint_from_str("999999999999999999"), bigint_one());
assert_eq(bigint_to_str(s), "1000000000000000000");
```

```fides
pub fn bigint_add(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 351*

---

## bigint_sub

a - b


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_sub(bigint_from_i64(3), bigint_from_i64(10))), "-7");
```

```fides
pub fn bigint_sub(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 375*

---

## bigint_neg

-a


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_neg(bigint_from_i64(4))), "-4");
```

```fides
pub fn bigint_neg(a: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `BigInt`

*Defined at line 389*

---

## bigint_abs

|a|


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_abs(bigint_from_i64(-4))), "4");
```

```fides
pub fn bigint_abs(a: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`

**Returns:** `BigInt`

*Defined at line 402*

---

## bigint_mul

a * b (schoolbook — products stay < 10^18, safely in i64).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_mul(bigint_from_i64(-3), bigint_from_i64(4))), "-12");
```

```fides
pub fn bigint_mul(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 412*

---

## bigint_mul_i64

a * small (|small| < 2^62 or so; products < 10^18 for |small| < 10^9
guaranteed, larger smalls work but multiply limb-wise).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_mul_i64(bigint_from_str("500000000000"), 4)), "2000000000000");
```

```fides
pub fn bigint_mul_i64(a: BigInt, small: i64) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `small`: `i64`

**Returns:** `BigInt`

*Defined at line 456*

---

## bigint_add_i64

a + small


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_add_i64(bigint_from_i64(1), 2)), "3");
```

```fides
pub fn bigint_add_i64(a: BigInt, small: i64) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `small`: `i64`

**Returns:** `BigInt`

*Defined at line 489*

---

## bigint_divmod

divmod: a = q*b + r, 0 <= |r| < |b| (r takes a's sign).
Division by zero → {0, 0}.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d = bigint_divmod(bigint_from_i64(17), bigint_from_i64(5));
assert_eq(bigint_to_i64(d.q), 3);
assert_eq(bigint_to_i64(d.r), 2);
```

```fides
pub fn bigint_divmod(a: BigInt, b: BigInt) -> BigDiv {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigDiv`

*Defined at line 525*

---

## bigint_div

a / b (truncated toward zero).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var q = bigint_div(bigint_from_str("100000000000000000000"), bigint_from_i64(7));
assert_eq(bigint_to_str(q), "14285714285714285714");
```

```fides
pub fn bigint_div(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 681*

---

## bigint_mod

a mod b — result takes a's sign (truncated division).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_i64(bigint_mod(bigint_from_str("100000000000000000000"), bigint_from_i64(7))), 2);
```

```fides
pub fn bigint_mod(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 689*

---

## bigint_pow

a^e for e >= 0 (binary exponentiation).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bigint_to_str(bigint_pow(bigint_from_i64(10), 20)), "100000000000000000000");
```

```fides
pub fn bigint_pow(a: BigInt, e: i64) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `e`: `i64`

**Returns:** `BigInt`

*Defined at line 699*

---

## bigint_modpow

(a^e) mod m — the workhorse for RSA-style crypto.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = bigint_modpow(bigint_from_i64(4), bigint_from_i64(13), bigint_from_i64(497));
assert_eq(bigint_to_i64(r), 445);
```

```fides
pub fn bigint_modpow(base_in: BigInt, e_in: BigInt, m: BigInt) -> BigInt {
```

**Parameters:**

- `base_in`: `BigInt`
- `e_in`: `BigInt`
- `m`: `BigInt`

**Returns:** `BigInt`

*Defined at line 718*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
