[All modules](index.md)

# complex -- Fides Documentation

## Contents

- [Complex](#Complex) (struct)
- [complex](#complex) (function)
- [complex_from_real](#complex_from_real) (function)
- [complex_from_polar](#complex_from_polar) (function)
- [complex_zero](#complex_zero) (function)
- [complex_one](#complex_one) (function)
- [complex_i](#complex_i) (function)
- [complex_add](#complex_add) (function)
- [complex_sub](#complex_sub) (function)
- [complex_mul](#complex_mul) (function)
- [complex_scale](#complex_scale) (function)
- [complex_div](#complex_div) (function)
- [complex_recip](#complex_recip) (function)
- [complex_neg](#complex_neg) (function)
- [complex_conj](#complex_conj) (function)
- [complex_abs](#complex_abs) (function)
- [complex_norm](#complex_norm) (function)
- [complex_arg](#complex_arg) (function)
- [complex_exp](#complex_exp) (function)
- [complex_ln](#complex_ln) (function)
- [complex_pow](#complex_pow) (function)
- [complex_sqrt](#complex_sqrt) (function)
- [complex_sin](#complex_sin) (function)
- [complex_cos](#complex_cos) (function)
- [complex_eq](#complex_eq) (function)
- [complex_approx_eq](#complex_approx_eq) (function)
- [complex_is_finite](#complex_is_finite) (function)
- [complex_to_str](#complex_to_str) (function)
- [complex_to_str_prec](#complex_to_str_prec) (function)

## Complex

A complex number re + im*i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val z = Complex { re: 1.0, im: -2.0 };
assert(z.re == 1.0 && z.im == -2.0);
```

```fides
pub struct Complex {
```

**Fields:**

| Name | Type |
|------|------|
| `re` | `f64` |
| `im` | `f64` |

*Defined at line 41*

---

## complex

The complex number re + im*i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val z = complex(3.0, 4.0);
assert(z.re == 3.0);
assert(z.im == 4.0);
```

```fides
pub fn complex(re: f64, im: f64) -> Complex {
```

**Parameters:**

- `re`: `f64`
- `im`: `f64`

**Returns:** `Complex`

*Defined at line 54*

---

## complex_from_real

The real number x as a complex number (x + 0i).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val z = complex_from_real(2.5);
assert(z.re == 2.5 && z.im == 0.0);
```

```fides
pub fn complex_from_real(x: f64) -> Complex {
```

**Parameters:**

- `x`: `f64`

**Returns:** `Complex`

*Defined at line 65*

---

## complex_from_polar

The complex number with modulus r and argument theta (radians):
r*cos(theta) + r*sin(theta)*i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val z = complex_from_polar(2.0, math_pi() / 2.0);
assert(complex_approx_eq(z, complex(0.0, 2.0), 0.000000000001));
```

```fides
pub fn complex_from_polar(r: f64, theta: f64) -> Complex {
```

**Parameters:**

- `r`: `f64`
- `theta`: `f64`

**Returns:** `Complex`

*Defined at line 75*

---

## complex_zero

0 + 0i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_zero(), complex(0.0, 0.0)));
```

```fides
pub fn complex_zero() -> Complex {
```

**Returns:** `Complex`

*Defined at line 83*

---

## complex_one

1 + 0i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_mul(complex_one(), complex(2.0, 3.0)), complex(2.0, 3.0)));
```

```fides
pub fn complex_one() -> Complex {
```

**Returns:** `Complex`

*Defined at line 91*

---

## complex_i

The imaginary unit i (0 + 1i).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val m = complex_mul(complex_i(), complex_i());
assert(complex_eq(m, complex(-1.0, 0.0)));
```

```fides
pub fn complex_i() -> Complex {
```

**Returns:** `Complex`

*Defined at line 100*

---

## complex_add

a + b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val s = complex_add(complex(1.0, 2.0), complex(3.0, -5.0));
assert(complex_eq(s, complex(4.0, -3.0)));
```

```fides
pub fn complex_add(a: Complex, b: Complex) -> Complex {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `Complex`

*Defined at line 111*

---

## complex_sub

a - b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val d = complex_sub(complex(1.0, 2.0), complex(3.0, -5.0));
assert(complex_eq(d, complex(-2.0, 7.0)));
```

```fides
pub fn complex_sub(a: Complex, b: Complex) -> Complex {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `Complex`

*Defined at line 120*

---

## complex_mul

a * b = (ac - bd) + (ad + bc)i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val p = complex_mul(complex(1.0, 2.0), complex(3.0, 4.0));
assert(complex_eq(p, complex(-5.0, 10.0)));
```

```fides
pub fn complex_mul(a: Complex, b: Complex) -> Complex {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `Complex`

*Defined at line 129*

---

## complex_scale

z * k for a real k.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_scale(complex(1.0, -2.0), 3.0), complex(3.0, -6.0)));
```

```fides
pub fn complex_scale(z: Complex, k: f64) -> Complex {
```

**Parameters:**

- `z`: `Complex`
- `k`: `f64`

**Returns:** `Complex`

*Defined at line 137*

---

## complex_div

a / b by Smith's algorithm on operands scaled by powers of two (exact
scalings), so neither a huge nor a tiny component of `b` overflows or
underflows an intermediate value when the quotient is representable.
Dividing by 0 gives infinite or NaN parts, as f64 division does.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val q = complex_div(complex(-5.0, 10.0), complex(3.0, 4.0));
assert(complex_approx_eq(q, complex(1.0, 2.0), 0.000000000000001));
val h = complex_div(complex(1.0e300, 1.0e300), complex(1.0e308, 1.0e308));
assert(complex_approx_eq(h, complex(0.00000001, 0.0), 0.000000000000001));
```

```fides
pub fn complex_div(a: Complex, b: Complex) -> Complex {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `Complex`

*Defined at line 151*

---

## complex_recip

1 / z (`complex_div(complex_one(), z)`).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_approx_eq(complex_recip(complex(0.0, 2.0)), complex(0.0, -0.5), 0.000000000000001));
```

```fides
pub fn complex_recip(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 171*

---

## complex_neg

-z.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_neg(complex(1.0, -2.0)), complex(-1.0, 2.0)));
```

```fides
pub fn complex_neg(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 179*

---

## complex_conj

The conjugate re - im*i.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_conj(complex(1.0, 2.0)), complex(1.0, -2.0)));
```

```fides
pub fn complex_conj(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 187*

---

## complex_abs

The modulus |z| = sqrt(re^2 + im^2), computed without overflow or
underflow of the squares (a hypot): |3e300 + 4e300i| is 5e300.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_abs(complex(3.0, 4.0)) == 5.0);
val big = complex_abs(complex(3.0e300, 4.0e300));
assert(math_fabs(big / 5.0e300 - 1.0) < 0.000000000000001);
```

```fides
pub fn complex_abs(z: Complex) -> f64 {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `f64`

*Defined at line 200*

---

## complex_norm

re^2 + im^2 (the squared modulus; can overflow where `complex_abs`
does not).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_norm(complex(3.0, 4.0)) == 25.0);
```

```fides
pub fn complex_norm(z: Complex) -> f64 {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `f64`

*Defined at line 209*

---

## complex_arg

The argument of z in radians, in (-pi, pi] (atan2(im, re)); 0 for 0.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(complex_arg(complex(0.0, 1.0)) - math_pi() / 2.0) < 0.000000000000001);
assert(complex_arg(complex(-1.0, 0.0)) == math_pi());
```

```fides
pub fn complex_arg(z: Complex) -> f64 {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `f64`

*Defined at line 218*

---

## complex_exp

e^z = e^re * (cos(im) + i*sin(im)).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val e = complex_exp(complex(0.0, math_pi()));
assert(complex_approx_eq(e, complex(-1.0, 0.0), 0.000000000000001));
```

```fides
pub fn complex_exp(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 229*

---

## complex_ln

The principal natural logarithm ln|z| + i*arg(z); the imaginary part is
in (-pi, pi]. ln(0) has a real part of -infinity.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val l = complex_ln(complex(-1.0, 0.0));
assert(complex_approx_eq(l, complex(0.0, math_pi()), 0.000000000000001));
```

```fides
pub fn complex_ln(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 243*

---

## complex_pow

The principal value of a^b = e^(b * ln a). 0^0 is 1; 0^b is 0 when the
real part of b is positive, NaN otherwise.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val p = complex_pow(complex(0.0, 1.0), complex(2.0, 0.0));
assert(complex_approx_eq(p, complex(-1.0, 0.0), 0.000000000000001));
val ii = complex_pow(complex_i(), complex_i());
assert(complex_approx_eq(ii, complex(math_exp(0.0 - math_pi() / 2.0), 0.0), 0.000000000000001));
```

```fides
pub fn complex_pow(a: Complex, b: Complex) -> Complex {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `Complex`

*Defined at line 255*

---

## complex_sqrt

The principal square root: real part >= 0, and the imaginary part has
the sign of z's (a zero counts as positive), so sqrt(-1) is i. Stable
for negative real parts and scaled for huge inputs.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex_sqrt(complex(-1.0, 0.0)), complex(0.0, 1.0)));
assert(complex_eq(complex_sqrt(complex(-4.0, 0.0)), complex(0.0, 2.0)));
val r = complex_sqrt(complex(3.0, -4.0));
assert(complex_approx_eq(r, complex(2.0, -1.0), 0.000000000000001));
```

```fides
pub fn complex_sqrt(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 278*

---

## complex_sin

sin z = sin(re)*cosh(im) + i*cos(re)*sinh(im).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_approx_eq(complex_sin(complex(math_pi() / 2.0, 0.0)), complex_one(), 0.000000000000001));
val s = complex_sin(complex(0.0, 1.0));
assert(complex_approx_eq(s, complex(0.0, 1.1752011936438014), 0.000000000000001));
```

```fides
pub fn complex_sin(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 311*

---

## complex_cos

cos z = cos(re)*cosh(im) - i*sin(re)*sinh(im).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_approx_eq(complex_cos(complex_zero()), complex_one(), 0.0));
val c = complex_cos(complex(0.0, 1.0));
assert(complex_approx_eq(c, complex(1.5430806348152437, 0.0), 0.000000000000001));
```

```fides
pub fn complex_cos(z: Complex) -> Complex {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `Complex`

*Defined at line 321*

---

## complex_eq

Exact equality of both parts (0.0 equals -0.0; NaN equals nothing).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_eq(complex(1.0, 2.0), complex(1.0, 2.0)));
assert(!complex_eq(complex(1.0, 2.0), complex(1.0, 2.5)));
```

```fides
pub fn complex_eq(a: Complex, b: Complex) -> bool {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`

**Returns:** `bool`

*Defined at line 332*

---

## complex_approx_eq

True when |a - b| <= tol * max(1, |a|, |b|): an absolute tolerance near
zero, a relative one for large values. NaN is near nothing.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_approx_eq(complex(1.0, 1.0), complex(1.0, 1.0000000001), 0.000000001));
assert(!complex_approx_eq(complex(1.0, 1.0), complex(1.0, 1.001), 0.000000001));
assert(complex_approx_eq(complex(1.0e20, 0.0), complex(1.0e20 + 1.0e10, 0.0), 0.000000001));
```

```fides
pub fn complex_approx_eq(a: Complex, b: Complex, tol: f64) -> bool {
```

**Parameters:**

- `a`: `Complex`
- `b`: `Complex`
- `tol`: `f64`

**Returns:** `bool`

*Defined at line 343*

---

## complex_is_finite

True when both parts are finite (not infinite, not NaN).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(complex_is_finite(complex(1.0, -1.0)));
val zero: f64 = 0.0;
assert(!complex_is_finite(complex(1.0 / zero, 0.0)));
```

```fides
pub fn complex_is_finite(z: Complex) -> bool {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `bool`

*Defined at line 358*

---

## complex_to_str

"re+imi" or "re-imi" with each part as `float_to_str` writes it:
complex(3.0, -4.0) is "3-4i". Infinite parts are "inf"/"-inf", NaN is
"nan".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(complex_to_str(complex(3.0, -4.0)), "3-4i");
assert_eq(complex_to_str(complex(0.5, 2.0)), "0.5+2i");
```

```fides
pub fn complex_to_str(z: Complex) -> str {
```

**Parameters:**

- `z`: `Complex`

**Returns:** `str`

*Defined at line 369*

---

## complex_to_str_prec

"re+imi" with each part rounded to `decimals` places (half away from
zero), the same text on every backend; a part that rounds to zero has no
sign. Parts of 1e15 or more (or that are not finite) are written as
`complex_to_str` writes them.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(complex_to_str_prec(complex(1.0 / 3.0, -2.0 / 3.0), 4), "0.3333-0.6667i");
assert_eq(complex_to_str_prec(complex(-0.00001, 2.5), 2), "0.00+2.50i");
```

```fides
pub fn complex_to_str_prec(z: Complex, decimals: i64) -> str {
```

**Parameters:**

- `z`: `Complex`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 387*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
