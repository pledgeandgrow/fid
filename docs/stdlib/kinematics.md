[All modules](index.md)

# kinematics -- Fides Documentation

## Contents

- [DhJoint](#DhJoint) (struct)
- [DhChain](#DhChain) (struct)
- [dh_revolute](#dh_revolute) (function)
- [dh_prismatic](#dh_prismatic) (function)
- [dh_with_limits](#dh_with_limits) (function)
- [dh_chain_new](#dh_chain_new) (function)
- [dh_chain_add](#dh_chain_add) (function)
- [dh_dof](#dh_dof) (function)
- [dh_link_transform](#dh_link_transform) (function)
- [dh_fk_frames](#dh_fk_frames) (function)
- [dh_fk](#dh_fk) (function)
- [dh_jacobian](#dh_jacobian) (function)
- [dh_pose_error](#dh_pose_error) (function)
- [IkOptions](#IkOptions) (struct)
- [dh_ik_defaults](#dh_ik_defaults) (function)
- [IkResult](#IkResult) (struct)
- [dh_clamp](#dh_clamp) (function)
- [dh_ik](#dh_ik) (function)
- [planar2_fk](#planar2_fk) (function)
- [planar2_ik](#planar2_ik) (function)
- [planar2_jacobian](#planar2_jacobian) (function)

## DhJoint

One DH link: a, alpha, d, theta; q adds to theta (revolute) or d
(prismatic). Limits apply when qmin < qmax.

```fides
pub struct DhJoint {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `f64` |
| `alpha` | `f64` |
| `d` | `f64` |
| `theta` | `f64` |
| `revolute` | `bool` |
| `qmin` | `f64` |
| `qmax` | `f64` |

*Defined at line 39*

---

## DhChain

A serial chain: world_T_tool = base * links * tool.

```fides
pub struct DhChain {
```

**Fields:**

| Name | Type |
|------|------|
| `joints` | `Vec<DhJoint>` |
| `base` | `Transform3` |
| `tool` | `Transform3` |

*Defined at line 50*

---

## dh_revolute

A revolute joint (theta = q + theta_offset), no limits.

```fides
pub fn dh_revolute(a: f64, alpha: f64, d: f64, theta_offset: f64) -> DhJoint {
```

**Parameters:**

- `a`: `f64`
- `alpha`: `f64`
- `d`: `f64`
- `theta_offset`: `f64`

**Returns:** `DhJoint`

*Defined at line 57*

---

## dh_prismatic

A prismatic joint (d = q + d_offset), no limits.

```fides
pub fn dh_prismatic(a: f64, alpha: f64, theta: f64, d_offset: f64) -> DhJoint {
```

**Parameters:**

- `a`: `f64`
- `alpha`: `f64`
- `theta`: `f64`
- `d_offset`: `f64`

**Returns:** `DhJoint`

*Defined at line 64*

---

## dh_with_limits

j with limits [qmin, qmax].

```fides
pub fn dh_with_limits(j: DhJoint, qmin: f64, qmax: f64) -> DhJoint {
```

**Parameters:**

- `j`: `DhJoint`
- `qmin`: `f64`
- `qmax`: `f64`

**Returns:** `DhJoint`

*Defined at line 71*

---

## dh_chain_new

An empty chain with identity base and tool.

```fides
pub fn dh_chain_new() -> DhChain {
```

**Returns:** `DhChain`

*Defined at line 78*

---

## dh_chain_add

Append a joint.

```fides
pub fn dh_chain_add(c: &mut DhChain, j: DhJoint) {
```

**Parameters:**

- `c`: `&mut DhChain`
- `j`: `DhJoint`

*Defined at line 85*

---

## dh_dof

The number of joints.

```fides
pub fn dh_dof(c: &DhChain) -> i64 {
```

**Parameters:**

- `c`: `&DhChain`

**Returns:** `i64`

*Defined at line 90*

---

## dh_link_transform

The link transform of joint j at joint value q:
Rz(theta) Tz(d) Tx(a) Rx(alpha).

```fides
pub fn dh_link_transform(j: DhJoint, q: f64) -> Transform3 {
```

**Parameters:**

- `j`: `DhJoint`
- `q`: `f64`

**Returns:** `Transform3`

*Defined at line 96*

---

## dh_fk_frames

The frames base, base*T1, ..., base*T1..Tn (n + 1 transforms, the tool
not applied).

```fides
pub fn dh_fk_frames(c: &DhChain, q: &Vec<f64>) -> Vec<Transform3> {
```

**Parameters:**

- `c`: `&DhChain`
- `q`: `&Vec<f64>`

**Returns:** `Vec<Transform3>`

*Defined at line 113*

---

## dh_fk

The tool pose base * T1(q1) ... Tn(qn) * tool.

```fides
pub fn dh_fk(c: &DhChain, q: &Vec<f64>) -> Transform3 {
```

**Parameters:**

- `c`: `&DhChain`
- `q`: `&Vec<f64>`

**Returns:** `Transform3`

*Defined at line 127*

---

## dh_jacobian

The 6 x n geometric Jacobian at q: rows 0..2 the linear velocity of the
tool point, rows 3..5 the angular velocity, both in the base (world)
frame.

```fides
pub fn dh_jacobian(c: &DhChain, q: &Vec<f64>) -> DMat {
```

**Parameters:**

- `c`: `&DhChain`
- `q`: `&Vec<f64>`

**Returns:** `DMat`

*Defined at line 135*

---

## dh_pose_error

The pose error target - current as a 6-vector: position difference and
the rotation vector of target * current^-1 (both in the base frame).

```fides
pub fn dh_pose_error(target: Transform3, current: Transform3) -> Vec<f64> {
```

**Parameters:**

- `target`: `Transform3`
- `current`: `Transform3`

**Returns:** `Vec<f64>`

*Defined at line 164*

---

## IkOptions

Settings of `dh_ik`.

```fides
pub struct IkOptions {
```

**Fields:**

| Name | Type |
|------|------|
| `max_iter` | `i64` |
| `tol_pos` | `f64` |
| `tol_rot` | `f64` |
| `lambda` | `f64` |
| `max_step` | `f64` |
| `position_only` | `bool` |

*Defined at line 181*

---

## dh_ik_defaults

200 iterations, 1e-9 m / 1e-9 rad, lambda 0.05, steps of at most 0.5.

```fides
pub fn dh_ik_defaults() -> IkOptions {
```

**Returns:** `IkOptions`

*Defined at line 195*

---

## IkResult

The result of `dh_ik`.

```fides
pub struct IkResult {
```

**Fields:**

| Name | Type |
|------|------|
| `q` | `Vec<f64>` |
| `iterations` | `i64` |
| `pos_err` | `f64` |
| `rot_err` | `f64` |
| `converged` | `bool` |

*Defined at line 202*

---

## dh_clamp

q with each joint clamped to its limits (joints without limits unchanged).

```fides
pub fn dh_clamp(c: &DhChain, q: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `c`: `&DhChain`
- `q`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 211*

---

## dh_ik

Joint values bringing the tool to `target`, from the guess q0, by damped
least squares. `converged` says whether the tolerances were met.

```fides
pub fn dh_ik(c: &DhChain, target: Transform3, q0: &Vec<f64>, opt: IkOptions) -> IkResult {
```

**Parameters:**

- `c`: `&DhChain`
- `target`: `Transform3`
- `q0`: `&Vec<f64>`
- `opt`: `IkOptions`

**Returns:** `IkResult`

*Defined at line 228*

---

## planar2_fk

The tip (x, y) of a planar arm with links l1, l2 at angles q1, q2
(q2 relative to link 1).

```fides
pub fn planar2_fk(l1: f64, l2: f64, q1: f64, q2: f64) -> vec2 {
```

**Parameters:**

- `l1`: `f64`
- `l2`: `f64`
- `q1`: `f64`
- `q2`: `f64`

**Returns:** `vec2`

*Defined at line 300*

---

## planar2_ik

Joint angles (q1, q2) putting the tip at (x, y): elbow_up picks q2 <= 0,
otherwise q2 >= 0. Err
when the point is out of reach.

```fides
pub fn planar2_ik(l1: f64, l2: f64, x: f64, y: f64, elbow_up: bool) -> Result<vec2, str> {
```

**Parameters:**

- `l1`: `f64`
- `l2`: `f64`
- `x`: `f64`
- `y`: `f64`
- `elbow_up`: `bool`

**Returns:** `Result<vec2, str>`

*Defined at line 307*

---

## planar2_jacobian

The 2 x 2 Jacobian d(x, y)/d(q1, q2) of the planar arm.

```fides
pub fn planar2_jacobian(l1: f64, l2: f64, q1: f64, q2: f64) -> DMat {
```

**Parameters:**

- `l1`: `f64`
- `l2`: `f64`
- `q1`: `f64`
- `q2`: `f64`

**Returns:** `DMat`

*Defined at line 324*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
