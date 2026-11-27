[All modules](index.md)

# transform3d -- Fides Documentation

## Contents

- [Transform3](#Transform3) (struct)
- [tf3_identity](#tf3_identity) (function)
- [tf3_new](#tf3_new) (function)
- [tf3_translation](#tf3_translation) (function)
- [tf3_rotation](#tf3_rotation) (function)
- [tf3_from_xyz_rpy](#tf3_from_xyz_rpy) (function)
- [tf3_compose](#tf3_compose) (function)
- [tf3_inverse](#tf3_inverse) (function)
- [tf3_apply](#tf3_apply) (function)
- [tf3_apply_dir](#tf3_apply_dir) (function)
- [tf3_apply_all](#tf3_apply_all) (function)
- [tf3_interpolate](#tf3_interpolate) (function)
- [tf3_between](#tf3_between) (function)
- [tf3_distance](#tf3_distance) (function)
- [tf3_approx_eq](#tf3_approx_eq) (function)
- [tf3_to_mat4](#tf3_to_mat4) (function)
- [tf3_from_mat4](#tf3_from_mat4) (function)
- [tf3_rot_mat3](#tf3_rot_mat3) (function)
- [tf3_to_str](#tf3_to_str) (function)
- [FrameTree](#FrameTree) (struct)
- [ftree_new](#ftree_new) (function)
- [ftree_len](#ftree_len) (function)
- [ftree_id](#ftree_id) (function)
- [ftree_has](#ftree_has) (function)
- [ftree_parent](#ftree_parent) (function)
- [ftree_add](#ftree_add) (function)
- [ftree_set_transform](#ftree_set_transform) (function)
- [ftree_reparent](#ftree_reparent) (function)
- [ftree_lookup](#ftree_lookup) (function)
- [ftree_transform_point](#ftree_transform_point) (function)
- [ftree_path](#ftree_path) (function)

## Transform3

A rigid transform: p -> rot * p + trans.

```fides
pub struct Transform3 {
```

**Fields:**

| Name | Type |
|------|------|
| `rot` | `quat` |
| `trans` | `vec3` |

*Defined at line 39*

---

## tf3_identity

The identity transform.

```fides
pub fn tf3_identity() -> Transform3 {
```

**Returns:** `Transform3`

*Defined at line 45*

---

## tf3_new

A transform from a rotation (normalized here) and a translation.

```fides
pub fn tf3_new(rot: quat, trans: vec3) -> Transform3 {
```

**Parameters:**

- `rot`: `quat`
- `trans`: `vec3`

**Returns:** `Transform3`

*Defined at line 52*

---

## tf3_translation

A pure translation.

```fides
pub fn tf3_translation(t: vec3) -> Transform3 {
```

**Parameters:**

- `t`: `vec3`

**Returns:** `Transform3`

*Defined at line 59*

---

## tf3_rotation

A pure rotation.

```fides
pub fn tf3_rotation(q: quat) -> Transform3 {
```

**Parameters:**

- `q`: `quat`

**Returns:** `Transform3`

*Defined at line 66*

---

## tf3_from_xyz_rpy

A transform from a translation and fixed-axis roll, pitch, yaw
(R = Rz(yaw) Ry(pitch) Rx(roll), as in a URDF origin).

```fides
pub fn tf3_from_xyz_rpy(x: f64, y: f64, z: f64, roll: f64, pitch: f64, yaw: f64) -> Transform3 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`
- `roll`: `f64`
- `pitch`: `f64`
- `yaw`: `f64`

**Returns:** `Transform3`

*Defined at line 74*

---

## tf3_compose

a * b: apply b, then a.

```fides
pub fn tf3_compose(a: Transform3, b: Transform3) -> Transform3 {
```

**Parameters:**

- `a`: `Transform3`
- `b`: `Transform3`

**Returns:** `Transform3`

*Defined at line 81*

---

## tf3_inverse

The inverse: p -> R^T (p - t).

```fides
pub fn tf3_inverse(t: Transform3) -> Transform3 {
```

**Parameters:**

- `t`: `Transform3`

**Returns:** `Transform3`

*Defined at line 88*

---

## tf3_apply

The point p transformed: R p + t.

```fides
pub fn tf3_apply(t: Transform3, p: vec3) -> vec3 {
```

**Parameters:**

- `t`: `Transform3`
- `p`: `vec3`

**Returns:** `vec3`

*Defined at line 97*

---

## tf3_apply_dir

The direction v rotated (no translation).

```fides
pub fn tf3_apply_dir(t: Transform3, v: vec3) -> vec3 {
```

**Parameters:**

- `t`: `Transform3`
- `v`: `vec3`

**Returns:** `vec3`

*Defined at line 102*

---

## tf3_apply_all

Every point of `pts` transformed.

```fides
pub fn tf3_apply_all(t: Transform3, pts: &Vec<vec3>) -> Vec<vec3> {
```

**Parameters:**

- `t`: `Transform3`
- `pts`: `&Vec<vec3>`

**Returns:** `Vec<vec3>`

*Defined at line 107*

---

## tf3_interpolate

The transform at fraction s of the way from a (s = 0) to b (s = 1):
slerp of the rotations, lerp of the translations.

```fides
pub fn tf3_interpolate(a: Transform3, b: Transform3, s: f64) -> Transform3 {
```

**Parameters:**

- `a`: `Transform3`
- `b`: `Transform3`
- `s`: `f64`

**Returns:** `Transform3`

*Defined at line 119*

---

## tf3_between

The relative motion a^-1 * b (b seen from a).

```fides
pub fn tf3_between(a: Transform3, b: Transform3) -> Transform3 {
```

**Parameters:**

- `a`: `Transform3`
- `b`: `Transform3`

**Returns:** `Transform3`

*Defined at line 126*

---

## tf3_distance

Translation distance and rotation angle (radians) between a and b.

```fides
pub fn tf3_distance(a: Transform3, b: Transform3) -> vec2 {
```

**Parameters:**

- `a`: `Transform3`
- `b`: `Transform3`

**Returns:** `vec2`

*Defined at line 131*

---

## tf3_approx_eq

True when a and b differ by at most `tol_t` in translation and `tol_r`
radians in rotation.

```fides
pub fn tf3_approx_eq(a: Transform3, b: Transform3, tol_t: f64, tol_r: f64) -> bool {
```

**Parameters:**

- `a`: `Transform3`
- `b`: `Transform3`
- `tol_t`: `f64`
- `tol_r`: `f64`

**Returns:** `bool`

*Defined at line 137*

---

## tf3_to_mat4

The homogeneous 4x4 matrix (math's mat4, mRC = row R column C).

```fides
pub fn tf3_to_mat4(t: Transform3) -> mat4 {
```

**Parameters:**

- `t`: `Transform3`

**Returns:** `mat4`

*Defined at line 143*

---

## tf3_from_mat4

The transform of a homogeneous 4x4 matrix whose upper 3x3 is a rotation.

```fides
pub fn tf3_from_mat4(m: mat4) -> Transform3 {
```

**Parameters:**

- `m`: `mat4`

**Returns:** `Transform3`

*Defined at line 165*

---

## tf3_rot_mat3

The rotation matrix and translation of t.

```fides
pub fn tf3_rot_mat3(t: Transform3) -> Mat3 {
```

**Parameters:**

- `t`: `Transform3`

**Returns:** `Mat3`

*Defined at line 175*

---

## tf3_to_str

"t=(x, y, z) q=(w, x, y, z)" with `dp` decimals.

```fides
pub fn tf3_to_str(t: Transform3, dp: i64) -> str {
```

**Parameters:**

- `t`: `Transform3`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 180*

---

## FrameTree

Named frames with parent links; frame 0 is the root.

```fides
pub struct FrameTree {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `parents` | `Vec<i64>` |
| `to_parent` | `Vec<Transform3>` |
| `index` | `HashMap<str` |

*Defined at line 187*

---

## ftree_new

A tree with only the root frame.

```fides
pub fn ftree_new(root: str) -> FrameTree {
```

**Parameters:**

- `root`: `str`

**Returns:** `FrameTree`

*Defined at line 197*

---

## ftree_len

The number of frames.

```fides
pub fn ftree_len(t: &FrameTree) -> i64 {
```

**Parameters:**

- `t`: `&FrameTree`

**Returns:** `i64`

*Defined at line 209*

---

## ftree_id

The id of frame `name`, or -1.

```fides
pub fn ftree_id(t: &FrameTree, name: str) -> i64 {
```

**Parameters:**

- `t`: `&FrameTree`
- `name`: `str`

**Returns:** `i64`

*Defined at line 214*

---

## ftree_has

True when frame `name` exists.

```fides
pub fn ftree_has(t: &FrameTree, name: str) -> bool {
```

**Parameters:**

- `t`: `&FrameTree`
- `name`: `str`

**Returns:** `bool`

*Defined at line 222*

---

## ftree_parent

The parent's name of frame `name` ("" for the root or an unknown frame).

```fides
pub fn ftree_parent(t: &FrameTree, name: str) -> str {
```

**Parameters:**

- `t`: `&FrameTree`
- `name`: `str`

**Returns:** `str`

*Defined at line 227*

---

## ftree_add

Add frame `name` under `parent` with parent_T_child `tf`.
Err when the name exists or the parent does not.

```fides
pub fn ftree_add(t: &mut FrameTree, name: str, parent: str, tf: Transform3) -> Result<i64, str> {
```

**Parameters:**

- `t`: `&mut FrameTree`
- `name`: `str`
- `parent`: `str`
- `tf`: `Transform3`

**Returns:** `Result<i64, str>`

*Defined at line 237*

---

## ftree_set_transform

Replace parent_T_child of frame `name`. Err for the root or an unknown frame.

```fides
pub fn ftree_set_transform(t: &mut FrameTree, name: str, tf: Transform3) -> Result<i64, str> {
```

**Parameters:**

- `t`: `&mut FrameTree`
- `name`: `str`
- `tf`: `Transform3`

**Returns:** `Result<i64, str>`

*Defined at line 254*

---

## ftree_reparent

Move frame `name` under `parent` with a new parent_T_child. Err when it
would make a cycle (the new parent is the frame or below it).

```fides
pub fn ftree_reparent(t: &mut FrameTree, name: str, parent: str, tf: Transform3) -> Result<i64, str> {
```

**Parameters:**

- `t`: `&mut FrameTree`
- `name`: `str`
- `parent`: `str`
- `tf`: `Transform3`

**Returns:** `Result<i64, str>`

*Defined at line 280*

---

## ftree_lookup

target_T_source: maps coordinates in frame `source` to frame `target`.

```fides
pub fn ftree_lookup(t: &FrameTree, target: str, source: str) -> Result<Transform3, str> {
```

**Parameters:**

- `t`: `&FrameTree`
- `target`: `str`
- `source`: `str`

**Returns:** `Result<Transform3, str>`

*Defined at line 344*

---

## ftree_transform_point

The point p given in frame `source`, expressed in frame `target`.

```fides
pub fn ftree_transform_point(t: &FrameTree, target: str, source: str, p: vec3) -> Result<vec3, str> {
```

**Parameters:**

- `t`: `&FrameTree`
- `target`: `str`
- `source`: `str`
- `p`: `vec3`

**Returns:** `Result<vec3, str>`

*Defined at line 360*

---

## ftree_path

The frames from `source` up to the common ancestor and down to `target`,
joined with " -> ".

```fides
pub fn ftree_path(t: &FrameTree, target: str, source: str) -> str {
```

**Parameters:**

- `t`: `&FrameTree`
- `target`: `str`
- `source`: `str`

**Returns:** `str`

*Defined at line 373*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
