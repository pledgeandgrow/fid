[All modules](index.md)

# quat -- Fides Documentation

## Contents

- [rmath_pi](#rmath_pi) (function)
- [rmath_atan](#rmath_atan) (function)
- [rmath_atan2](#rmath_atan2) (function)
- [rmath_asin](#rmath_asin) (function)
- [rmath_acos](#rmath_acos) (function)
- [rmath_tan](#rmath_tan) (function)
- [rmath_hypot](#rmath_hypot) (function)
- [rmath_wrap_angle](#rmath_wrap_angle) (function)
- [rmath_max](#rmath_max) (function)
- [rmath_min](#rmath_min) (function)
- [rmath_clamp](#rmath_clamp) (function)
- [rmath_fmt](#rmath_fmt) (function)
- [rmath_vec3_str](#rmath_vec3_str) (function)
- [Mat3](#Mat3) (struct)
- [mat3_identity](#mat3_identity) (function)
- [mat3_from_rows](#mat3_from_rows) (function)
- [mat3_col](#mat3_col) (function)
- [mat3_mul](#mat3_mul) (function)
- [mat3_mul_vec](#mat3_mul_vec) (function)
- [mat3_transpose](#mat3_transpose) (function)
- [mat3_det](#mat3_det) (function)
- [mat3_max_diff](#mat3_max_diff) (function)
- [mat3_rot_x](#mat3_rot_x) (function)
- [mat3_rot_y](#mat3_rot_y) (function)
- [mat3_rot_z](#mat3_rot_z) (function)
- [mat3_to_str](#mat3_to_str) (function)
- [AxisAngle](#AxisAngle) (struct)
- [EulerZYX](#EulerZYX) (struct)
- [qt_identity](#qt_identity) (function)
- [qt_mul](#qt_mul) (function)
- [qt_conj](#qt_conj) (function)
- [qt_norm_sq](#qt_norm_sq) (function)
- [qt_norm](#qt_norm) (function)
- [qt_normalize](#qt_normalize) (function)
- [qt_inverse](#qt_inverse) (function)
- [qt_dot](#qt_dot) (function)
- [qt_canonical](#qt_canonical) (function)
- [qt_rotate](#qt_rotate) (function)
- [qt_from_axis_angle](#qt_from_axis_angle) (function)
- [qt_to_axis_angle](#qt_to_axis_angle) (function)
- [qt_to_rotvec](#qt_to_rotvec) (function)
- [qt_from_rotvec](#qt_from_rotvec) (function)
- [qt_from_euler_zyx](#qt_from_euler_zyx) (function)
- [qt_to_euler_zyx](#qt_to_euler_zyx) (function)
- [qt_to_mat3](#qt_to_mat3) (function)
- [qt_from_mat3](#qt_from_mat3) (function)
- [qt_slerp](#qt_slerp) (function)
- [qt_angle_between](#qt_angle_between) (function)
- [qt_from_two_vectors](#qt_from_two_vectors) (function)
- [qt_integrate](#qt_integrate) (function)
- [qt_approx_eq](#qt_approx_eq) (function)
- [qt_to_str](#qt_to_str) (function)

## rmath_pi

pi.

```fides
pub fn rmath_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 42*

---

## rmath_atan

atan(x) in (-pi/2, pi/2).

```fides
pub fn rmath_atan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 77*

---

## rmath_atan2

atan2(y, x) in (-pi, pi]; 0 for (0, 0).

```fides
pub fn rmath_atan2(y: f64, x: f64) -> f64 {
```

**Parameters:**

- `y`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 88*

---

## rmath_asin

asin(x), x clamped to [-1, 1].

```fides
pub fn rmath_asin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 119*

---

## rmath_acos

acos(x), x clamped to [-1, 1].

```fides
pub fn rmath_acos(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 131*

---

## rmath_tan

tan(x).

```fides
pub fn rmath_tan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 143*

---

## rmath_hypot

sqrt(x*x + y*y) without overflow for large components.

```fides
pub fn rmath_hypot(x: f64, y: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`

**Returns:** `f64`

*Defined at line 148*

---

## rmath_wrap_angle

An angle wrapped to (-pi, pi].

```fides
pub fn rmath_wrap_angle(a: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`

**Returns:** `f64`

*Defined at line 164*

---

## rmath_max

The larger of two floats.

```fides
pub fn rmath_max(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 175*

---

## rmath_min

The smaller of two floats.

```fides
pub fn rmath_min(a: f64, b: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `f64`

*Defined at line 183*

---

## rmath_clamp

x clamped to [lo, hi].

```fides
pub fn rmath_clamp(x: f64, lo: f64, hi: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`
- `lo`: `f64`
- `hi`: `f64`

**Returns:** `f64`

*Defined at line 191*

---

## rmath_fmt

x with `dp` decimals (0..12), rounded half away from zero; "-0.00" never
appears ("0.00" instead). NaN prints "nan", infinities "inf"/"-inf".

```fides
pub fn rmath_fmt(x: f64, dp: i64) -> str {
```

**Parameters:**

- `x`: `f64`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 203*

---

## rmath_vec3_str

A vec3 as "(x, y, z)" with `dp` decimals.

```fides
pub fn rmath_vec3_str(v: vec3, dp: i64) -> str {
```

**Parameters:**

- `v`: `vec3`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 246*

---

## Mat3

A 3x3 matrix, row-major: mRC is row R, column C.

```fides
pub struct Mat3 {
```

**Fields:**

| Name | Type |
|------|------|
| `m00` | `f64` |
| `m01` | `f64` |
| `m02` | `f64` |
| `m10` | `f64` |
| `m11` | `f64` |
| `m12` | `f64` |
| `m20` | `f64` |
| `m21` | `f64` |
| `m22` | `f64` |

*Defined at line 253*

---

## mat3_identity

The identity matrix.

```fides
pub fn mat3_identity() -> Mat3 {
```

**Returns:** `Mat3`

*Defined at line 266*

---

## mat3_from_rows

A matrix from its rows.

```fides
pub fn mat3_from_rows(r0: vec3, r1: vec3, r2: vec3) -> Mat3 {
```

**Parameters:**

- `r0`: `vec3`
- `r1`: `vec3`
- `r2`: `vec3`

**Returns:** `Mat3`

*Defined at line 273*

---

## mat3_col

Column c (0..2) of m.

```fides
pub fn mat3_col(m: Mat3, c: i64) -> vec3 {
```

**Parameters:**

- `m`: `Mat3`
- `c`: `i64`

**Returns:** `vec3`

*Defined at line 280*

---

## mat3_mul

a * b.

```fides
pub fn mat3_mul(a: Mat3, b: Mat3) -> Mat3 {
```

**Parameters:**

- `a`: `Mat3`
- `b`: `Mat3`

**Returns:** `Mat3`

*Defined at line 291*

---

## mat3_mul_vec

m * v.

```fides
pub fn mat3_mul_vec(m: Mat3, v: vec3) -> vec3 {
```

**Parameters:**

- `m`: `Mat3`
- `v`: `vec3`

**Returns:** `vec3`

*Defined at line 298*

---

## mat3_transpose

The transpose (the inverse, for a rotation).

```fides
pub fn mat3_transpose(m: Mat3) -> Mat3 {
```

**Parameters:**

- `m`: `Mat3`

**Returns:** `Mat3`

*Defined at line 303*

---

## mat3_det

The determinant.

```fides
pub fn mat3_det(m: Mat3) -> f64 {
```

**Parameters:**

- `m`: `Mat3`

**Returns:** `f64`

*Defined at line 310*

---

## mat3_max_diff

Largest absolute difference between the entries of a and b.

```fides
pub fn mat3_max_diff(a: Mat3, b: Mat3) -> f64 {
```

**Parameters:**

- `a`: `Mat3`
- `b`: `Mat3`

**Returns:** `f64`

*Defined at line 315*

---

## mat3_rot_x

Rotation about x by `a` radians.

```fides
pub fn mat3_rot_x(a: f64) -> Mat3 {
```

**Parameters:**

- `a`: `f64`

**Returns:** `Mat3`

*Defined at line 330*

---

## mat3_rot_y

Rotation about y by `a` radians.

```fides
pub fn mat3_rot_y(a: f64) -> Mat3 {
```

**Parameters:**

- `a`: `f64`

**Returns:** `Mat3`

*Defined at line 339*

---

## mat3_rot_z

Rotation about z by `a` radians.

```fides
pub fn mat3_rot_z(a: f64) -> Mat3 {
```

**Parameters:**

- `a`: `f64`

**Returns:** `Mat3`

*Defined at line 348*

---

## mat3_to_str

The matrix as three bracketed rows with `dp` decimals.

```fides
pub fn mat3_to_str(m: Mat3, dp: i64) -> str {
```

**Parameters:**

- `m`: `Mat3`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 357*

---

## AxisAngle

Axis (unit) and angle (radians, in [0, pi]) of a rotation.

```fides
pub struct AxisAngle {
```

**Fields:**

| Name | Type |
|------|------|
| `axis` | `vec3` |
| `angle` | `f64` |

*Defined at line 364*

---

## EulerZYX

Euler angles of the ZYX convention: R = Rz(yaw) Ry(pitch) Rx(roll).

```fides
pub struct EulerZYX {
```

**Fields:**

| Name | Type |
|------|------|
| `roll` | `f64` |
| `pitch` | `f64` |
| `yaw` | `f64` |

*Defined at line 370*

---

## qt_identity

The identity rotation.

```fides
pub fn qt_identity() -> quat {
```

**Returns:** `quat`

*Defined at line 377*

---

## qt_mul

a * b (Hamilton product): rotating by the result is rotating by b, then a.

```fides
pub fn qt_mul(a: quat, b: quat) -> quat {
```

**Parameters:**

- `a`: `quat`
- `b`: `quat`

**Returns:** `quat`

*Defined at line 382*

---

## qt_conj

The conjugate w - xi - yj - zk.

```fides
pub fn qt_conj(q: quat) -> quat {
```

**Parameters:**

- `q`: `quat`

**Returns:** `quat`

*Defined at line 387*

---

## qt_norm_sq

w*w + x*x + y*y + z*z.

```fides
pub fn qt_norm_sq(q: quat) -> f64 {
```

**Parameters:**

- `q`: `quat`

**Returns:** `f64`

*Defined at line 392*

---

## qt_norm

The norm |q|.

```fides
pub fn qt_norm(q: quat) -> f64 {
```

**Parameters:**

- `q`: `quat`

**Returns:** `f64`

*Defined at line 397*

---

## qt_normalize

q / |q|; the identity for a zero quaternion.

```fides
pub fn qt_normalize(q: quat) -> quat {
```

**Parameters:**

- `q`: `quat`

**Returns:** `quat`

*Defined at line 402*

---

## qt_inverse

The inverse conj(q) / |q|^2 (the conjugate for a unit quaternion).

```fides
pub fn qt_inverse(q: quat) -> quat {
```

**Parameters:**

- `q`: `quat`

**Returns:** `quat`

*Defined at line 411*

---

## qt_dot

The 4D dot product.

```fides
pub fn qt_dot(a: quat, b: quat) -> f64 {
```

**Parameters:**

- `a`: `quat`
- `b`: `quat`

**Returns:** `f64`

*Defined at line 420*

---

## qt_canonical

q with w >= 0 (q and -q are the same rotation).

```fides
pub fn qt_canonical(q: quat) -> quat {
```

**Parameters:**

- `q`: `quat`

**Returns:** `quat`

*Defined at line 425*

---

## qt_rotate

v rotated by the unit quaternion q (q v q*).

```fides
pub fn qt_rotate(q: quat, v: vec3) -> vec3 {
```

**Parameters:**

- `q`: `quat`
- `v`: `vec3`

**Returns:** `vec3`

*Defined at line 433*

---

## qt_from_axis_angle

The rotation by `angle` radians about `axis` (normalized here; a zero
axis gives the identity).

```fides
pub fn qt_from_axis_angle(axis: vec3, angle: f64) -> quat {
```

**Parameters:**

- `axis`: `vec3`
- `angle`: `f64`

**Returns:** `quat`

*Defined at line 443*

---

## qt_to_axis_angle

Axis and angle of q; angle in [0, pi]. The identity gives axis (1, 0, 0)
and angle 0.

```fides
pub fn qt_to_axis_angle(qin: quat) -> AxisAngle {
```

**Parameters:**

- `qin`: `quat`

**Returns:** `AxisAngle`

*Defined at line 454*

---

## qt_to_rotvec

The rotation vector (axis * angle) of q.

```fides
pub fn qt_to_rotvec(q: quat) -> vec3 {
```

**Parameters:**

- `q`: `quat`

**Returns:** `vec3`

*Defined at line 468*

---

## qt_from_rotvec

The rotation of a rotation vector (axis * angle).

```fides
pub fn qt_from_rotvec(r: vec3) -> quat {
```

**Parameters:**

- `r`: `vec3`

**Returns:** `quat`

*Defined at line 474*

---

## qt_from_euler_zyx

The ZYX Euler rotation Rz(yaw) Ry(pitch) Rx(roll).

```fides
pub fn qt_from_euler_zyx(roll: f64, pitch: f64, yaw: f64) -> quat {
```

**Parameters:**

- `roll`: `f64`
- `pitch`: `f64`
- `yaw`: `f64`

**Returns:** `quat`

*Defined at line 484*

---

## qt_to_euler_zyx

ZYX Euler angles of q: roll and yaw in (-pi, pi], pitch in [-pi/2, pi/2].
At pitch = +-pi/2 (gimbal lock) roll is 0 and yaw takes the whole turn.

```fides
pub fn qt_to_euler_zyx(qin: quat) -> EulerZYX {
```

**Parameters:**

- `qin`: `quat`

**Returns:** `EulerZYX`

*Defined at line 496*

---

## qt_to_mat3

The rotation matrix of q (normalized first).

```fides
pub fn qt_to_mat3(qin: quat) -> Mat3 {
```

**Parameters:**

- `qin`: `quat`

**Returns:** `Mat3`

*Defined at line 519*

---

## qt_from_mat3

The quaternion (w >= 0) of a rotation matrix, by Shepperd's method (the
largest of the four candidates, so no division by a small number).

```fides
pub fn qt_from_mat3(m: Mat3) -> quat {
```

**Parameters:**

- `m`: `Mat3`

**Returns:** `quat`

*Defined at line 537*

---

## qt_slerp

Spherical linear interpolation along the shorter arc: a at t = 0, b at
t = 1, constant angular speed in between. Nearly equal inputs use a
normalized lerp (sin(theta) would be ~0).

```fides
pub fn qt_slerp(a: quat, bin: quat, t: f64) -> quat {
```

**Parameters:**

- `a`: `quat`
- `bin`: `quat`
- `t`: `f64`

**Returns:** `quat`

*Defined at line 563*

---

## qt_angle_between

The angle (radians, [0, pi]) of the rotation taking a to b.

```fides
pub fn qt_angle_between(a: quat, b: quat) -> f64 {
```

**Parameters:**

- `a`: `quat`
- `b`: `quat`

**Returns:** `f64`

*Defined at line 581*

---

## qt_from_two_vectors

The shortest rotation taking direction a to direction b.

```fides
pub fn qt_from_two_vectors(ain: vec3, bin: vec3) -> quat {
```

**Parameters:**

- `ain`: `vec3`
- `bin`: `vec3`

**Returns:** `quat`

*Defined at line 590*

---

## qt_integrate

q advanced by the body angular velocity `omega` (rad/s, in the rotated
frame) over dt seconds: q * exp(omega dt / 2). Exact for constant omega.

```fides
pub fn qt_integrate(q: quat, omega: vec3, dt: f64) -> quat {
```

**Parameters:**

- `q`: `quat`
- `omega`: `vec3`
- `dt`: `f64`

**Returns:** `quat`

*Defined at line 608*

---

## qt_approx_eq

True when a and b are the same rotation within `tol` (q and -q equal).

```fides
pub fn qt_approx_eq(a: quat, b: quat, tol: f64) -> bool {
```

**Parameters:**

- `a`: `quat`
- `b`: `quat`
- `tol`: `f64`

**Returns:** `bool`

*Defined at line 614*

---

## qt_to_str

q as "w + xi + yj + zk" style "(w, x, y, z)" with `dp` decimals.

```fides
pub fn qt_to_str(q: quat, dp: i64) -> str {
```

**Parameters:**

- `q`: `quat`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 620*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
