[All modules](index.md)

# simd -- Fides Documentation

## Contents

- [add](#add) (function)
- [sub](#sub) (function)
- [mul](#mul) (function)
- [div](#div) (function)
- [dot](#dot) (function)
- [sum](#sum) (function)
- [scale](#scale) (function)
- [from_vec](#from_vec) (function)
- [to_str](#to_str) (function)
- [add_i64](#add_i64) (function)
- [sub_i64](#sub_i64) (function)
- [and_i64](#and_i64) (function)
- [or_i64](#or_i64) (function)
- [xor_i64](#xor_i64) (function)
- [simd_lanes_add_i64](#simd_lanes_add_i64) (function)
- [simd_lanes_sub_i64](#simd_lanes_sub_i64) (function)
- [simd_lanes_and_i64](#simd_lanes_and_i64) (function)
- [simd_lanes_or_i64](#simd_lanes_or_i64) (function)
- [simd_lanes_xor_i64](#simd_lanes_xor_i64) (function)

## add

Elementwise addition of two SIMD vectors.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a: Vec<f64> = Vec::new();
a.push(1.0); a.push(2.0); a.push(3.0);
var b: Vec<f64> = Vec::new();
b.push(4.0); b.push(5.0); b.push(6.0);
assert(dot(a, b) == 32.0);
assert(sum(a) == 6.0);
var x: Vec<i64> = Vec::new();
x.push(12); x.push(10);
assert_eq(and_i64(x, x).get(0), 12);
assert_eq(add_i64(x, x).get(1), 20);
```


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
var a: Vec<f64> = Vec::new();
a.push(1.0); a.push(2.0); a.push(3.0);
var b: Vec<f64> = Vec::new();
b.push(4.0); b.push(5.0); b.push(6.0);
assert(add(a, b).get(2) == 9.0);
assert(mul(a, b).get(1) == 10.0);
assert(sum(scale(a, 2.0)) == 12.0);
```

```fides
pub fn add(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 45*

---

## sub

Elementwise subtraction.

```fides
pub fn sub(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 50*

---

## mul

Elementwise multiplication.

```fides
pub fn mul(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 55*

---

## div

Elementwise division.

```fides
pub fn div(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 60*

---

## dot

Dot product of two SIMD vectors -> scalar.

```fides
pub fn dot(a: Vec<f64>, b: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 65*

---

## sum

Sum all elements of a SIMD vector -> scalar.

```fides
pub fn sum(a: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 70*

---

## scale

Scale a SIMD vector by a scalar.

```fides
pub fn scale(a: Vec<f64>, s: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `s`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 75*

---

## from_vec

Convert a Vec to a SIMD vector (identity — same representation).

```fides
pub fn from_vec(a: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 80*

---

## to_str

Convert SIMD vector to string.

```fides
pub fn to_str(a: Vec<f64>) -> str {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `str`

*Defined at line 85*

---

## add_i64

Elementwise wrapping addition of two Vec<i64>.

```fides
pub fn add_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 103*

---

## sub_i64

Elementwise wrapping subtraction of two Vec<i64>.

```fides
pub fn sub_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 108*

---

## and_i64

Elementwise bitwise and of two Vec<i64>.

```fides
pub fn and_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 113*

---

## or_i64

Elementwise bitwise or of two Vec<i64>.

```fides
pub fn or_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 118*

---

## xor_i64

Elementwise bitwise exclusive or of two Vec<i64>.

```fides
pub fn xor_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 123*

---

## simd_lanes_add_i64

```fides
pub fn simd_lanes_add_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 130*

---

## simd_lanes_sub_i64

```fides
pub fn simd_lanes_sub_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 143*

---

## simd_lanes_and_i64

The lanes of and_i64.

```fides
pub fn simd_lanes_and_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 155*

---

## simd_lanes_or_i64

The lanes of or_i64.

```fides
pub fn simd_lanes_or_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 167*

---

## simd_lanes_xor_i64

The lanes of xor_i64.

```fides
pub fn simd_lanes_xor_i64(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 179*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
