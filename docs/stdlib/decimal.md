[All modules](index.md)

# decimal -- Fides Documentation

## Contents

- [DEC_HALF_EVEN:](#DEC_HALF_EVEN:) (const)
- [DEC_HALF_UP:](#DEC_HALF_UP:) (const)
- [DEC_DOWN:](#DEC_DOWN:) (const)
- [DEC_UP:](#DEC_UP:) (const)
- [DEC_FLOOR:](#DEC_FLOOR:) (const)
- [DEC_CEILING:](#DEC_CEILING:) (const)
- [Decimal](#Decimal) (struct)
- [dec_zero](#dec_zero) (function)
- [dec_from_i64](#dec_from_i64) (function)
- [dec_from_units](#dec_from_units) (function)
- [dec_parse](#dec_parse) (function)
- [dec_to_str](#dec_to_str) (function)
- [dec_add](#dec_add) (function)
- [dec_sub](#dec_sub) (function)
- [dec_mul](#dec_mul) (function)
- [dec_neg](#dec_neg) (function)
- [dec_abs](#dec_abs) (function)
- [dec_sign](#dec_sign) (function)
- [dec_cmp](#dec_cmp) (function)
- [dec_eq](#dec_eq) (function)
- [dec_lt](#dec_lt) (function)
- [dec_gt](#dec_gt) (function)
- [dec_round](#dec_round) (function)
- [dec_div](#dec_div) (function)
- [dec_normalize](#dec_normalize) (function)
- [dec_to_i64](#dec_to_i64) (function)

## DEC_HALF_EVEN:

```fides
pub const DEC_HALF_EVEN: i64 = 0;
```

*Defined at line 23*

---

## DEC_HALF_UP:

```fides
pub const DEC_HALF_UP: i64 = 1;
```

*Defined at line 24*

---

## DEC_DOWN:

```fides
pub const DEC_DOWN: i64 = 2;
```

*Defined at line 25*

---

## DEC_UP:

```fides
pub const DEC_UP: i64 = 3;
```

*Defined at line 26*

---

## DEC_FLOOR:

```fides
pub const DEC_FLOOR: i64 = 4;
```

*Defined at line 27*

---

## DEC_CEILING:

```fides
pub const DEC_CEILING: i64 = 5;
```

*Defined at line 28*

---

## Decimal

mantissa / 10^scale


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var price = dec_parse("19.99");
var total = dec_mul(price, dec_from_i64(3));
println(dec_to_str(total));
assert_eq(dec_to_str(total), "59.97");
assert_eq(dec_to_str(dec_add(dec_parse("0.1"), dec_parse("0.2"))), "0.3");
```

```fides
pub struct Decimal {
```

**Fields:**

| Name | Type |
|------|------|
| `m` | `BigInt` |
| `scale` | `i64` |

*Defined at line 37*

---

## dec_zero

The decimal 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_sign(dec_zero()), 0);
```

```fides
pub fn dec_zero() -> Decimal {
```

**Returns:** `Decimal`

*Defined at line 53*

---

## dec_from_i64

The integer n as a decimal (scale 0).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_from_i64(-7)), "-7");
```

```fides
pub fn dec_from_i64(n: i64) -> Decimal {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Decimal`

*Defined at line 63*

---

## dec_from_units

units / 10^scale: dec_from_units(1250, 2) is 12.50. A negative scale
multiplies: dec_from_units(12, -2) is 1200.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_from_units(1250, 2)), "12.50");
```

```fides
pub fn dec_from_units(units: i64, scale: i64) -> Decimal {
```

**Parameters:**

- `units`: `i64`
- `scale`: `i64`

**Returns:** `Decimal`

*Defined at line 74*

---

## dec_parse

Parse "[-+]digits[.digits][e[-+]digits]"; anything else is 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_parse("-3.140")), "-3.140");
assert_eq(dec_to_str(dec_parse("1.5e2")), "150");
```

```fides
pub fn dec_parse(text: str) -> Decimal {
```

**Parameters:**

- `text`: `str`

**Returns:** `Decimal`

*Defined at line 91*

---

## dec_to_str

The decimal as text, with exactly `scale` digits after the point.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_from_units(5, 3)), "0.005");
```

```fides
pub fn dec_to_str(d: Decimal) -> str {
```

**Parameters:**

- `d`: `Decimal`

**Returns:** `str`

*Defined at line 158*

---

## dec_add

a + b, exact (the scale of the result is the larger scale).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_add(dec_parse("1.25"), dec_parse("0.5"))), "1.75");
```

```fides
pub fn dec_add(a: Decimal, b: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `Decimal`

*Defined at line 200*

---

## dec_sub

a - b, exact.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_sub(dec_parse("1"), dec_parse("0.01"))), "0.99");
```

```fides
pub fn dec_sub(a: Decimal, b: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `Decimal`

*Defined at line 216*

---

## dec_mul

a * b, exact (the scales add up).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_mul(dec_parse("1.5"), dec_parse("1.5"))), "2.25");
```

```fides
pub fn dec_mul(a: Decimal, b: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `Decimal`

*Defined at line 224*

---

## dec_neg

The value with its sign flipped.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_neg(dec_parse("2.5"))), "-2.5");
```

```fides
pub fn dec_neg(a: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`

**Returns:** `Decimal`

*Defined at line 234*

---

## dec_abs

The value without its sign.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_abs(dec_parse("-2.5"))), "2.5");
```

```fides
pub fn dec_abs(a: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`

**Returns:** `Decimal`

*Defined at line 244*

---

## dec_sign

-1, 0 or 1


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_sign(dec_parse("-0.1")), -1);
```

```fides
pub fn dec_sign(a: Decimal) -> i64 {
```

**Parameters:**

- `a`: `Decimal`

**Returns:** `i64`

*Defined at line 254*

---

## dec_cmp

-1, 0 or 1 by value (2.50 equals 2.5)


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_cmp(dec_parse("2.50"), dec_parse("2.5")), 0);
assert_eq(dec_cmp(dec_parse("1"), dec_parse("1.01")), -1);
```

```fides
pub fn dec_cmp(a: Decimal, b: Decimal) -> i64 {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `i64`

*Defined at line 269*

---

## dec_eq

Whether a == b in value (2.50 equals 2.5).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(dec_eq(dec_parse("2.50"), dec_parse("2.5")));
```

```fides
pub fn dec_eq(a: Decimal, b: Decimal) -> bool {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `bool`

*Defined at line 277*

---

## dec_lt

Whether a < b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(dec_lt(dec_parse("0.09"), dec_parse("0.1")));
```

```fides
pub fn dec_lt(a: Decimal, b: Decimal) -> bool {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `bool`

*Defined at line 284*

---

## dec_gt

Whether a > b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(dec_gt(dec_parse("0"), dec_parse("-0.5")));
```

```fides
pub fn dec_gt(a: Decimal, b: Decimal) -> bool {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`

**Returns:** `bool`

*Defined at line 291*

---

## dec_round

a rounded to `scale` digits after the point.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_round(dec_parse("2.345"), 2, DEC_HALF_UP)), "2.35");
assert_eq(dec_to_str(dec_round(dec_parse("2.345"), 2, DEC_HALF_EVEN)), "2.34");
assert_eq(dec_to_str(dec_round(dec_parse("2.349"), 2, DEC_DOWN)), "2.34");
```

```fides
pub fn dec_round(a: Decimal, scale: i64, mode: i64) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`
- `scale`: `i64`
- `mode`: `i64`

**Returns:** `Decimal`

*Defined at line 336*

---

## dec_div

a / b with `scale` digits after the point, rounded by `mode`. Division
by zero panics.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var third = dec_div(dec_from_i64(1), dec_from_i64(3), 4, DEC_HALF_EVEN);
assert_eq(dec_to_str(third), "0.3333");
```

```fides
pub fn dec_div(a: Decimal, b: Decimal, scale: i64, mode: i64) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`
- `b`: `Decimal`
- `scale`: `i64`
- `mode`: `i64`

**Returns:** `Decimal`

*Defined at line 356*

---

## dec_normalize

The same value with no trailing zeros after the point (2.500 -> 2.5).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_str(dec_normalize(dec_parse("2.500"))), "2.5");
```

```fides
pub fn dec_normalize(a: Decimal) -> Decimal {
```

**Parameters:**

- `a`: `Decimal`

**Returns:** `Decimal`

*Defined at line 383*

---

## dec_to_i64

The integer part, rounded toward zero, if it fits in an i64.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(dec_to_i64(dec_parse("-12.9")), -12);
```

```fides
pub fn dec_to_i64(a: Decimal) -> i64 {
```

**Parameters:**

- `a`: `Decimal`

**Returns:** `i64`

*Defined at line 408*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
