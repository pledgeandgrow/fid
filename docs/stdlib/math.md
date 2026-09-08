# math — Fides Documentation

## Contents

- [math_pi](#math_pi) (function)
- [math_e](#math_e) (function)
- [math_sqrt](#math_sqrt) (function)
- [math_sin](#math_sin) (function)
- [math_cos](#math_cos) (function)
- [math_floor](#math_floor) (function)
- [math_ceil](#math_ceil) (function)
- [math_pow](#math_pow) (function)
- [math_fabs](#math_fabs) (function)
- [math_log](#math_log) (function)
- [math_exp](#math_exp) (function)
- [math_abs_i64](#math_abs_i64) (function)
- [math_max_f64](#math_max_f64) (function)
- [math_min_f64](#math_min_f64) (function)
- [math_clamp_f64](#math_clamp_f64) (function)
- [vec2](#vec2) (struct)
- [vec3](#vec3) (struct)
- [vec4](#vec4) (struct)
- [mat4](#mat4) (struct)
- [quat](#quat) (struct)
- [vec2](#vec2) (function)
- [vec3](#vec3) (function)
- [vec4](#vec4) (function)
- [quat](#quat) (function)
- [radians](#radians) (function)
- [degrees](#degrees) (function)
- [lerp](#lerp) (function)
- [distance](#distance) (function)
- [vec2](#vec2) (impl)
- [vec3](#vec3) (impl)
- [vec4](#vec4) (impl)
- [mat4](#mat4) (impl)
- [translate](#translate) (function)
- [mat_scale](#mat_scale) (function)
- [quat](#quat) (impl)

## math_pi

```fides
pub fn math_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 32*

---

## math_e

```fides
pub fn math_e() -> f64 {
```

**Returns:** `f64`

*Defined at line 36*

---

## math_sqrt

Square root of x.

```fides
pub fn math_sqrt(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 43*

---

## math_sin

Sine of x (x in radians).

```fides
pub fn math_sin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 48*

---

## math_cos

Cosine of x (x in radians).

```fides
pub fn math_cos(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 53*

---

## math_floor

Floor of x (largest integer <= x).

```fides
pub fn math_floor(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 58*

---

## math_ceil

Ceiling of x (smallest integer >= x).

```fides
pub fn math_ceil(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 63*

---

## math_pow

base raised to the power of exp.

```fides
pub fn math_pow(base: f64, exp: f64) -> f64 {
```

**Parameters:**

- `base`: `f64`
- `exp`: `f64`

**Returns:** `f64`

*Defined at line 68*

---

## math_fabs

Absolute value of a float.

```fides
pub fn math_fabs(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 73*

---

## math_log

Natural logarithm (base e) of x.

```fides
pub fn math_log(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 78*

---

## math_exp

Exponential function (e^x).

```fides
pub fn math_exp(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 83*

---

## math_abs_i64

Absolute value of an integer.

```fides
pub fn math_abs_i64(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 88*

---

## math_max_f64

Maximum of two floats.

```fides
pub fn math_max_f64(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 94*

---

## math_min_f64

Minimum of two floats.

```fides
pub fn math_min_f64(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 100*

---

## math_clamp_f64

Clamp a float to a range.

```fides
pub fn math_clamp_f64(n: f64, lo: f64, hi: f64) -> f64 {
```

**Parameters:**

- `n`: `f64`
- `lo`: `f64`
- `hi`: `f64`

**Returns:** `f64`

*Defined at line 106*

---

## vec2

```fides
pub struct vec2 { pub x: f64, pub y: f64 }
```

*Defined at line 115*

---

## vec3

```fides
pub struct vec3 { pub x: f64, pub y: f64, pub z: f64 }
```

*Defined at line 116*

---

## vec4

```fides
pub struct vec4 { pub x: f64, pub y: f64, pub z: f64, pub w: f64 }
```

*Defined at line 117*

---

## mat4

```fides
pub struct mat4 {
```

**Fields:**

| Name | Type |
|------|------|
| `m00` | `f64, pub m01: f64, pub m02: f64, pub m03: f64` |
| `m10` | `f64, pub m11: f64, pub m12: f64, pub m13: f64` |
| `m20` | `f64, pub m21: f64, pub m22: f64, pub m23: f64` |
| `m30` | `f64, pub m31: f64, pub m32: f64, pub m33: f64` |

*Defined at line 120*

---

## quat

```fides
pub struct quat { pub w: f64, pub x: f64, pub y: f64, pub z: f64 }
```

*Defined at line 128*

---

## vec2

```fides
pub fn vec2(x: f64, y: f64) -> vec2 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`

**Returns:** `vec2`

*Defined at line 132*

---

## vec3

```fides
pub fn vec3(x: f64, y: f64, z: f64) -> vec3 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `vec3`

*Defined at line 136*

---

## vec4

```fides
pub fn vec4(x: f64, y: f64, z: f64, w: f64) -> vec4 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`
- `w`: `f64`

**Returns:** `vec4`

*Defined at line 140*

---

## quat

```fides
pub fn quat(w: f64, x: f64, y: f64, z: f64) -> quat {
```

**Parameters:**

- `w`: `f64`
- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `quat`

*Defined at line 144*

---

## radians

```fides
pub fn radians(degrees: f64) -> f64 {
```

**Parameters:**

- `degrees`: `f64`

**Returns:** `f64`

*Defined at line 150*

---

## degrees

```fides
pub fn degrees(radians: f64) -> f64 {
```

**Parameters:**

- `radians`: `f64`

**Returns:** `f64`

*Defined at line 154*

---

## lerp

```fides
pub fn lerp(a: f64, b: f64, t: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 158*

---

## distance

```fides
pub fn distance(a: vec3, b: vec3) -> f64 {
```

**Parameters:**

- `a`: `vec3`
- `b`: `vec3`

**Returns:** `f64`

*Defined at line 162*

---

## vec2

```fides
impl vec2 {
```

*Defined at line 171*

---

## vec3

```fides
impl vec3 {
```

*Defined at line 205*

---

## vec4

```fides
impl vec4 {
```

*Defined at line 251*

---

## mat4

```fides
impl mat4 {
```

*Defined at line 294*

---

## translate

```fides
pub fn translate(t: vec3) -> mat4 {
```

**Parameters:**

- `t`: `vec3`

**Returns:** `mat4`

*Defined at line 347*

---

## mat_scale

```fides
pub fn mat_scale(s: f64) -> mat4 {
```

**Parameters:**

- `s`: `f64`

**Returns:** `mat4`

*Defined at line 356*

---

## quat

```fides
impl quat {
```

*Defined at line 367*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
