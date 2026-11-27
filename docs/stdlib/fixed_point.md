[All modules](index.md)

# fixed_point -- Fides Documentation

## Contents

- [Q15_ONE:](#Q15_ONE:) (const)
- [Q15_MAX:](#Q15_MAX:) (const)
- [Q15_MIN:](#Q15_MIN:) (const)
- [Q31_MAX:](#Q31_MAX:) (const)
- [Q31_MIN:](#Q31_MIN:) (const)
- [Q16_ONE:](#Q16_ONE:) (const)
- [Q16_MAX:](#Q16_MAX:) (const)
- [Q16_MIN:](#Q16_MIN:) (const)
- [Q16_PI:](#Q16_PI:) (const)
- [Q16_HALF_PI:](#Q16_HALF_PI:) (const)
- [Q16_TWO_PI:](#Q16_TWO_PI:) (const)
- [fx_round_shift](#fx_round_shift) (function)
- [q15_sat](#q15_sat) (function)
- [q15_add](#q15_add) (function)
- [q15_sub](#q15_sub) (function)
- [q15_neg](#q15_neg) (function)
- [q15_abs](#q15_abs) (function)
- [q15_mul](#q15_mul) (function)
- [q15_div](#q15_div) (function)
- [q15_mac](#q15_mac) (function)
- [q15_from_acc](#q15_from_acc) (function)
- [q31_sat](#q31_sat) (function)
- [q31_add](#q31_add) (function)
- [q31_sub](#q31_sub) (function)
- [q31_mul](#q31_mul) (function)
- [q31_div](#q31_div) (function)
- [q15_to_q31](#q15_to_q31) (function)
- [q31_to_q15](#q31_to_q15) (function)
- [q16_sat](#q16_sat) (function)
- [q16_from_int](#q16_from_int) (function)
- [q16_add](#q16_add) (function)
- [q16_sub](#q16_sub) (function)
- [q16_mul](#q16_mul) (function)
- [q16_div](#q16_div) (function)
- [q16_floor](#q16_floor) (function)
- [q16_round](#q16_round) (function)
- [q15_from_f64](#q15_from_f64) (function)
- [q31_from_f64](#q31_from_f64) (function)
- [q16_from_f64](#q16_from_f64) (function)
- [q15_to_f64](#q15_to_f64) (function)
- [q31_to_f64](#q31_to_f64) (function)
- [q16_to_f64](#q16_to_f64) (function)
- [fx_to_str](#fx_to_str) (function)
- [q16_to_str](#q16_to_str) (function)
- [q15_to_str](#q15_to_str) (function)
- [fx_parse](#fx_parse) (function)
- [fx_isqrt](#fx_isqrt) (function)
- [q16_sqrt](#q16_sqrt) (function)
- [q15_sqrt](#q15_sqrt) (function)
- [CORDIC_K_Q30:](#CORDIC_K_Q30:) (const)
- [q16_sincos](#q16_sincos) (function)
- [q16_sin](#q16_sin) (function)
- [q16_cos](#q16_cos) (function)
- [q16_polar](#q16_polar) (function)
- [q16_atan2](#q16_atan2) (function)
- [q16_hypot](#q16_hypot) (function)

## Q15_ONE:

```fides
pub const Q15_ONE: i64 = 32768;
```

*Defined at line 23*

---

## Q15_MAX:

```fides
pub const Q15_MAX: i64 = 32767;
```

*Defined at line 24*

---

## Q15_MIN:

```fides
pub const Q15_MIN: i64 = -32768;
```

*Defined at line 25*

---

## Q31_MAX:

```fides
pub const Q31_MAX: i64 = 2147483647;
```

*Defined at line 26*

---

## Q31_MIN:

```fides
pub const Q31_MIN: i64 = -2147483648;
```

*Defined at line 27*

---

## Q16_ONE:

```fides
pub const Q16_ONE: i64 = 65536;
```

*Defined at line 28*

---

## Q16_MAX:

```fides
pub const Q16_MAX: i64 = 2147483647;
```

*Defined at line 29*

---

## Q16_MIN:

```fides
pub const Q16_MIN: i64 = -2147483648;
```

*Defined at line 30*

---

## Q16_PI:

```fides
pub const Q16_PI: i64 = 205887;
```

*Defined at line 31*

---

## Q16_HALF_PI:

```fides
pub const Q16_HALF_PI: i64 = 102944;
```

*Defined at line 32*

---

## Q16_TWO_PI:

```fides
pub const Q16_TWO_PI: i64 = 411775;
```

*Defined at line 33*

---

## fx_round_shift

Shift right by `n` rounding to nearest (ties away from zero).

```fides
pub fn fx_round_shift(v: i64, n: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 46*

---

## q15_sat

```fides
pub fn q15_sat(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 77*

---

## q15_add

```fides
pub fn q15_add(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 80*

---

## q15_sub

```fides
pub fn q15_sub(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 83*

---

## q15_neg

```fides
pub fn q15_neg(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 86*

---

## q15_abs

```fides
pub fn q15_abs(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 89*

---

## q15_mul

```fides
pub fn q15_mul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 96*

---

## q15_div

```fides
pub fn q15_div(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 100*

---

## q15_mac

Multiply-accumulate into a 64-bit accumulator (Q30), as DSP MAC units.

```fides
pub fn q15_mac(acc: i64, a: i64, b: i64) -> i64 {
```

**Parameters:**

- `acc`: `i64`
- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 111*

---

## q15_from_acc

The Q15 result of a Q30 accumulator.

```fides
pub fn q15_from_acc(acc: i64) -> i64 {
```

**Parameters:**

- `acc`: `i64`

**Returns:** `i64`

*Defined at line 116*

---

## q31_sat

```fides
pub fn q31_sat(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 122*

---

## q31_add

```fides
pub fn q31_add(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 125*

---

## q31_sub

```fides
pub fn q31_sub(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## q31_mul

```fides
pub fn q31_mul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 132*

---

## q31_div

```fides
pub fn q31_div(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 137*

---

## q15_to_q31

```fides
pub fn q15_to_q31(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 147*

---

## q31_to_q15

```fides
pub fn q31_to_q15(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 150*

---

## q16_sat

```fides
pub fn q16_sat(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 156*

---

## q16_from_int

```fides
pub fn q16_from_int(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 159*

---

## q16_add

```fides
pub fn q16_add(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 162*

---

## q16_sub

```fides
pub fn q16_sub(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 165*

---

## q16_mul

```fides
pub fn q16_mul(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 169*

---

## q16_div

```fides
pub fn q16_div(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 173*

---

## q16_floor

Integer part, rounding towards minus infinity.

```fides
pub fn q16_floor(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 184*

---

## q16_round

Nearest integer.

```fides
pub fn q16_round(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 189*

---

## q15_from_f64

```fides
pub fn q15_from_f64(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 212*

---

## q31_from_f64

```fides
pub fn q31_from_f64(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 215*

---

## q16_from_f64

```fides
pub fn q16_from_f64(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 218*

---

## q15_to_f64

```fides
pub fn q15_to_f64(a: i64) -> f64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `f64`

*Defined at line 221*

---

## q31_to_f64

```fides
pub fn q31_to_f64(a: i64) -> f64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `f64`

*Defined at line 224*

---

## q16_to_f64

```fides
pub fn q16_to_f64(a: i64) -> f64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `f64`

*Defined at line 227*

---

## fx_to_str

A value with `frac` fraction bits as decimal text with `places`
decimals, rounded to nearest: fx_to_str(205887, 16, 4) = "3.1416".

```fides
pub fn fx_to_str(a: i64, frac: i64, places: i64) -> str {
```

**Parameters:**

- `a`: `i64`
- `frac`: `i64`
- `places`: `i64`

**Returns:** `str`

*Defined at line 233*

---

## q16_to_str

```fides
pub fn q16_to_str(a: i64) -> str {
```

**Parameters:**

- `a`: `i64`

**Returns:** `str`

*Defined at line 272*

---

## q15_to_str

```fides
pub fn q15_to_str(a: i64) -> str {
```

**Parameters:**

- `a`: `i64`

**Returns:** `str`

*Defined at line 275*

---

## fx_parse

Parse "-12.375" into a value with `frac` fraction bits (rounded).

```fides
pub fn fx_parse(s: str, frac: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `frac`: `i64`

**Returns:** `i64`

*Defined at line 280*

---

## fx_isqrt

floor(sqrt(n)) for n >= 0, bit by bit.

```fides
pub fn fx_isqrt(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 312*

---

## q16_sqrt

Q16.16 square root (negative input: 0), rounded down to the last bit.

```fides
pub fn q16_sqrt(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 335*

---

## q15_sqrt

Q15 square root of a non-negative Q15 value.

```fides
pub fn q15_sqrt(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 343*

---

## CORDIC_K_Q30:

The CORDIC gain correction 1/K = prod 1/sqrt(1 + 2^-2i), Q30.

```fides
pub const CORDIC_K_Q30: i64 = 652032874;
```

*Defined at line 390*

---

## q16_sincos

sin and cos of a Q16.16 angle in radians, packed in a 2-element Vec
[sin, cos] (Q16.16).

```fides
pub fn q16_sincos(angle: i64) -> Vec<i64> {
```

**Parameters:**

- `angle`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 396*

---

## q16_sin

```fides
pub fn q16_sin(angle: i64) -> i64 {
```

**Parameters:**

- `angle`: `i64`

**Returns:** `i64`

*Defined at line 443*

---

## q16_cos

```fides
pub fn q16_cos(angle: i64) -> i64 {
```

**Parameters:**

- `angle`: `i64`

**Returns:** `i64`

*Defined at line 446*

---

## q16_polar

atan2(y, x) in Q16.16 radians, (-pi, pi]; [angle, magnitude] by
CORDIC vectoring (magnitude in the inputs' scale).

```fides
pub fn q16_polar(y0: i64, x0: i64) -> Vec<i64> {
```

**Parameters:**

- `y0`: `i64`
- `x0`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 452*

---

## q16_atan2

```fides
pub fn q16_atan2(y: i64, x: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 525*

---

## q16_hypot

```fides
pub fn q16_hypot(y: i64, x: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 528*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
