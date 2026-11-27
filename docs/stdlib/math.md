[All modules](index.md)

# math -- Fides Documentation

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
- [perspective](#perspective) (function)
- [orthographic](#orthographic) (function)
- [look_at](#look_at) (function)
- [rotation_x](#rotation_x) (function)
- [rotation_y](#rotation_y) (function)
- [rotation_z](#rotation_z) (function)
- [vec3_sub](#vec3_sub) (function)

## math_pi

The constant pi (3.14159...).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_pi() > 3.14159 && math_pi() < 3.1416);
```

```fides
pub fn math_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 37*

---

## math_e

Euler's number e (2.71828...).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(math_log(math_e()) - 1.0) < 0.000001);
```

```fides
pub fn math_e() -> f64 {
```

**Returns:** `f64`

*Defined at line 45*

---

## math_sqrt

Square root of x.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
println(float_to_str(math_sqrt(16.0)));
assert(math_sqrt(16.0) == 4.0);
```

```fides
pub fn math_sqrt(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 56*

---

## math_sin

Sine of x (x in radians).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(math_sin(math_pi() / 2.0) - 1.0) < 0.000001);
```

```fides
pub fn math_sin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 68*

---

## math_cos

Cosine of x (x in radians).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(math_cos(0.0) - 1.0) < 0.000001);
```

```fides
pub fn math_cos(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 80*

---

## math_floor

Floor of x (largest integer <= x).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_floor(2.7) == 2.0);
assert(math_floor(-2.5) == -3.0);
```

```fides
pub fn math_floor(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 93*

---

## math_ceil

Ceiling of x (smallest integer >= x).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_ceil(2.1) == 3.0);
```

```fides
pub fn math_ceil(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 105*

---

## math_pow

base raised to the power of exp.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_pow(2.0, 10.0) == 1024.0);
```

```fides
pub fn math_pow(base: f64, exp: f64) -> f64 {
```

**Parameters:**

- `base`: `f64`
- `exp`: `f64`

**Returns:** `f64`

*Defined at line 117*

---

## math_fabs

Absolute value of a float.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(-3.5) == 3.5);
```

```fides
pub fn math_fabs(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 129*

---

## math_log

Natural logarithm (base e) of x.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(math_log(math_exp(2.0)) - 2.0) < 0.000001);
```

```fides
pub fn math_log(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 141*

---

## math_exp

Exponential function (e^x).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_exp(0.0) == 1.0);
```

```fides
pub fn math_exp(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 153*

---

## math_abs_i64

Absolute value of an integer.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(math_abs_i64(-7), 7);
```

```fides
pub fn math_abs_i64(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 165*

---

## math_max_f64

Maximum of two floats.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_max_f64(1.5, 2.5) == 2.5);
```

```fides
pub fn math_max_f64(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 176*

---

## math_min_f64

Minimum of two floats.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_min_f64(1.5, 2.5) == 1.5);
```

```fides
pub fn math_min_f64(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 187*

---

## math_clamp_f64

Clamp a float to a range.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_clamp_f64(5.0, 0.0, 1.0) == 1.0);
assert(math_clamp_f64(-5.0, 0.0, 1.0) == 0.0);
```

```fides
pub fn math_clamp_f64(n: f64, lo: f64, hi: f64) -> f64 {
```

**Parameters:**

- `n`: `f64`
- `lo`: `f64`
- `hi`: `f64`

**Returns:** `f64`

*Defined at line 199*

---

## vec2

```fides
pub struct vec2 {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |

*Defined at line 212*

---

## vec3

```fides
pub struct vec3 {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |

*Defined at line 216*

---

## vec4

```fides
pub struct vec4 {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |
| `w` | `f64` |

*Defined at line 221*

---

## mat4

```fides
pub struct mat4 {
```

**Fields:**

| Name | Type |
|------|------|
| `m00` | `f64` |
| `m01` | `f64` |
| `m02` | `f64` |
| `m03` | `f64` |
| `m10` | `f64` |
| `m11` | `f64` |
| `m12` | `f64` |
| `m13` | `f64` |
| `m20` | `f64` |
| `m21` | `f64` |
| `m22` | `f64` |
| `m23` | `f64` |
| `m30` | `f64` |
| `m31` | `f64` |
| `m32` | `f64` |
| `m33` | `f64` |

*Defined at line 229*

---

## quat

```fides
pub struct quat {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `f64` |
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |

*Defined at line 249*

---

## vec2

A 2D vector with components x and y.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = vec2(3.0, 4.0);
assert(a.length() == 5.0);
assert(a.add(vec2(1.0, 1.0)).x == 4.0);
assert(a.dot(vec2(1.0, 0.0)) == 3.0);
```

```fides
pub fn vec2(x: f64, y: f64) -> vec2 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`

**Returns:** `vec2`

*Defined at line 265*

---

## vec3

A 3D vector with components x, y and z.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var x = vec3(1.0, 0.0, 0.0);
var y = vec3(0.0, 1.0, 0.0);
var z = x.cross(y);
assert(z.z == 1.0);
assert(x.add(y).scale(2.0).y == 2.0);
```

```fides
pub fn vec3(x: f64, y: f64, z: f64) -> vec3 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `vec3`

*Defined at line 279*

---

## vec4

A 4D vector with components x, y, z and w.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = vec4(1.0, 2.0, 3.0, 1.0);
assert(v.xyz().z == 3.0);
assert(v.dot(vec4(1.0, 1.0, 1.0, 0.0)) == 6.0);
```

```fides
pub fn vec4(x: f64, y: f64, z: f64, w: f64) -> vec4 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`
- `w`: `f64`

**Returns:** `vec4`

*Defined at line 291*

---

## quat

A quaternion (w, x, y, z); quat(1, 0, 0, 0) is no rotation.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var q = quat::from_axis_angle(vec3(0.0, 0.0, 1.0), math_pi() / 2.0);
var r = q.rotate_vec3(vec3(1.0, 0.0, 0.0));
assert(math_fabs(r.y - 1.0) < 0.000001);
```

```fides
pub fn quat(w: f64, x: f64, y: f64, z: f64) -> quat {
```

**Parameters:**

- `w`: `f64`
- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `quat`

*Defined at line 303*

---

## radians

Degrees converted to radians.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(radians(180.0) - math_pi()) < 0.000001);
```

```fides
pub fn radians(degrees: f64) -> f64 {
```

**Parameters:**

- `degrees`: `f64`

**Returns:** `f64`

*Defined at line 315*

---

## degrees

Radians converted to degrees.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(math_fabs(degrees(math_pi()) - 180.0) < 0.000001);
```

```fides
pub fn degrees(radians: f64) -> f64 {
```

**Parameters:**

- `radians`: `f64`

**Returns:** `f64`

*Defined at line 323*

---

## lerp

Linear interpolation: `a` at t = 0, `b` at t = 1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(lerp(10.0, 20.0, 0.25) == 12.5);
```

```fides
pub fn lerp(a: f64, b: f64, t: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 331*

---

## distance

Euclidean distance between two points.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(distance(vec3(0.0, 0.0, 0.0), vec3(1.0, 2.0, 2.0)) == 3.0);
```

```fides
pub fn distance(a: vec3, b: vec3) -> f64 {
```

**Parameters:**

- `a`: `vec3`
- `b`: `vec3`

**Returns:** `f64`

*Defined at line 339*

---

## vec2

```fides
impl vec2 {
```

**Methods:**

### add

```fides
pub fn add(self: vec2, other: vec2) -> vec2 {
```

### sub

```fides
pub fn sub(self: vec2, other: vec2) -> vec2 {
```

### scale

```fides
pub fn scale(self: vec2, s: f64) -> vec2 {
```

### dot

```fides
pub fn dot(self: vec2, other: vec2) -> f64 {
```

### length

```fides
pub fn length(self: vec2) -> f64 {
```

### normalize

```fides
pub fn normalize(self: vec2) -> vec2 {
```

### lerp

```fides
pub fn lerp(self: vec2, other: vec2, t: f64) -> vec2 {
```

*Defined at line 348*

---

## vec3

```fides
impl vec3 {
```

**Methods:**

### add

```fides
pub fn add(self: vec3, other: vec3) -> vec3 {
```

### sub

```fides
pub fn sub(self: vec3, other: vec3) -> vec3 {
```

### scale

```fides
pub fn scale(self: vec3, s: f64) -> vec3 {
```

### dot

```fides
pub fn dot(self: vec3, other: vec3) -> f64 {
```

### cross

```fides
pub fn cross(self: vec3, other: vec3) -> vec3 {
```

### length

```fides
pub fn length(self: vec3) -> f64 {
```

### normalize

```fides
pub fn normalize(self: vec3) -> vec3 {
```

### lerp

```fides
pub fn lerp(self: vec3, other: vec3, t: f64) -> vec3 {
```

*Defined at line 396*

---

## vec4

```fides
impl vec4 {
```

**Methods:**

### add

```fides
pub fn add(self: vec4, other: vec4) -> vec4 {
```

### sub

```fides
pub fn sub(self: vec4, other: vec4) -> vec4 {
```

### scale

```fides
pub fn scale(self: vec4, s: f64) -> vec4 {
```

### dot

```fides
pub fn dot(self: vec4, other: vec4) -> f64 {
```

### length

```fides
pub fn length(self: vec4) -> f64 {
```

### normalize

```fides
pub fn normalize(self: vec4) -> vec4 {
```

### xyz

```fides
pub fn xyz(self: vec4) -> vec3 {
```

### lerp

```fides
pub fn lerp(self: vec4, other: vec4, t: f64) -> vec4 {
```

*Defined at line 450*

---

## mat4

```fides
impl mat4 {
```

**Methods:**

### identity

```fides
pub fn identity() -> mat4 {
```

### mul

```fides
pub fn mul(self: mat4, other: mat4) -> mat4 {
```

### transform_vec4

```fides
pub fn transform_vec4(self: mat4, v: vec4) -> vec4 {
```

### transform_vec3

```fides
pub fn transform_vec3(self: mat4, v: vec3) -> vec3 {
```

*Defined at line 504*

---

## translate

A translation matrix that moves points by `t`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = translate(vec3(1.0, 2.0, 3.0)).transform_vec3(vec3(1.0, 1.0, 1.0));
assert(p.x == 2.0 && p.y == 3.0 && p.z == 4.0);
```

```fides
pub fn translate(t: vec3) -> mat4 {
```

**Parameters:**

- `t`: `vec3`

**Returns:** `mat4`

*Defined at line 541*

---

## mat_scale

A uniform scaling matrix.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = mat_scale(2.0).transform_vec3(vec3(1.0, 2.0, 3.0));
assert(p.z == 6.0);
```

```fides
pub fn mat_scale(s: f64) -> mat4 {
```

**Parameters:**

- `s`: `f64`

**Returns:** `mat4`

*Defined at line 552*

---

## quat

```fides
impl quat {
```

**Methods:**

### from_axis_angle

```fides
pub fn from_axis_angle(axis: vec3, angle: f64) -> quat {
```

### rotate_vec3

```fides
pub fn rotate_vec3(self: quat, v: vec3) -> vec3 {
```

### normalize

Normalize a quaternion to unit length.

```fides
pub fn normalize(self: quat) -> quat {
```

### conjugate

Conjugate of a quaternion (negate xyz, keep w).

```fides
pub fn conjugate(self: quat) -> quat {
```

### mul

Hamilton product of two quaternions (combines rotations).

```fides
pub fn mul(self: quat, other: quat) -> quat {
```

### slerp

Spherical linear interpolation between two quaternions.

```fides
pub fn slerp(self: quat, other: quat, t: f64) -> quat {
```

### to_mat4

Convert quaternion to a 4x4 rotation matrix.

```fides
pub fn to_mat4(self: quat) -> mat4 {
```

*Defined at line 560*

---

## perspective

Perspective projection matrix (field of view in radians).

```fides
pub fn perspective(fov: f64, aspect: f64, near: f64, far: f64) -> mat4 {
```

**Parameters:**

- `fov`: `f64`
- `aspect`: `f64`
- `near`: `f64`
- `far`: `f64`

**Returns:** `mat4`

*Defined at line 672*

---

## orthographic

Orthographic projection matrix.

```fides
pub fn orthographic(left: f64, right: f64, bottom: f64, top: f64, near: f64, far: f64) -> mat4 {
```

**Parameters:**

- `left`: `f64`
- `right`: `f64`
- `bottom`: `f64`
- `top`: `f64`
- `near`: `f64`
- `far`: `f64`

**Returns:** `mat4`

*Defined at line 681*

---

## look_at

Look-at view matrix (camera at eye, looking at target, up direction).

```fides
pub fn look_at(eye: vec3, target: vec3, up: vec3) -> mat4 {
```

**Parameters:**

- `eye`: `vec3`
- `target`: `vec3`
- `up`: `vec3`

**Returns:** `mat4`

*Defined at line 691*

---

## rotation_x

Rotation matrix around X axis (angle in radians).

```fides
pub fn rotation_x(angle: f64) -> mat4 {
```

**Parameters:**

- `angle`: `f64`

**Returns:** `mat4`

*Defined at line 705*

---

## rotation_y

Rotation matrix around Y axis (angle in radians).

```fides
pub fn rotation_y(angle: f64) -> mat4 {
```

**Parameters:**

- `angle`: `f64`

**Returns:** `mat4`

*Defined at line 714*

---

## rotation_z

Rotation matrix around Z axis (angle in radians).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = rotation_z(math_pi() / 2.0).transform_vec3(vec3(1.0, 0.0, 0.0));
assert(math_fabs(p.y - 1.0) < 0.000001);
```

```fides
pub fn rotation_z(angle: f64) -> mat4 {
```

**Parameters:**

- `angle`: `f64`

**Returns:** `mat4`

*Defined at line 727*

---

## vec3_sub

Helper: vec3 subtraction (for look_at).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d = vec3_sub(vec3(5.0, 5.0, 5.0), vec3(1.0, 2.0, 3.0));
assert(d.x == 4.0 && d.z == 2.0);
```

```fides
pub fn vec3_sub(a: vec3, b: vec3) -> vec3 {
```

**Parameters:**

- `a`: `vec3`
- `b`: `vec3`

**Returns:** `vec3`

*Defined at line 740*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
