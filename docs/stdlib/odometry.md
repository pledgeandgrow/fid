[All modules](index.md)

# odometry -- Fides Documentation

## Contents

- [Pose2](#Pose2) (struct)
- [Twist2](#Twist2) (struct)
- [pose2](#pose2) (function)
- [pose2_to_str](#pose2_to_str) (function)
- [pose2_compose](#pose2_compose) (function)
- [odom_exp](#odom_exp) (function)
- [odom_integrate](#odom_integrate) (function)
- [odom_integrate_vw](#odom_integrate_vw) (function)
- [odom_integrate_euler](#odom_integrate_euler) (function)
- [DiffDrive](#DiffDrive) (struct)
- [diff_forward](#diff_forward) (function)
- [diff_inverse](#diff_inverse) (function)
- [DiffOdom](#DiffOdom) (struct)
- [odom_diff_new](#odom_diff_new) (function)
- [odom_diff_step](#odom_diff_step) (function)
- [odom_diff_update](#odom_diff_update) (function)
- [odom_ticks_to_m](#odom_ticks_to_m) (function)
- [Mecanum](#Mecanum) (struct)
- [mecanum_forward](#mecanum_forward) (function)
- [mecanum_inverse](#mecanum_inverse) (function)
- [mecanum_saturate](#mecanum_saturate) (function)
- [Ackermann](#Ackermann) (struct)
- [ackermann_forward](#ackermann_forward) (function)
- [ackermann_inverse](#ackermann_inverse) (function)
- [ackermann_radius](#ackermann_radius) (function)
- [ackermann_wheel_angles](#ackermann_wheel_angles) (function)
- [ackermann_rear_speeds](#ackermann_rear_speeds) (function)

## Pose2

A planar pose.

```fides
pub struct Pose2 {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `th` | `f64` |

*Defined at line 34*

---

## Twist2

A planar body velocity: vx forward, vy left, wz counter-clockwise.

```fides
pub struct Twist2 {
```

**Fields:**

| Name | Type |
|------|------|
| `vx` | `f64` |
| `vy` | `f64` |
| `wz` | `f64` |

*Defined at line 41*

---

## pose2

```fides
pub fn pose2(x: f64, y: f64, th: f64) -> Pose2 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `th`: `f64`

**Returns:** `Pose2`

*Defined at line 47*

---

## pose2_to_str

```fides
pub fn pose2_to_str(p: Pose2, dp: i64) -> str {
```

**Parameters:**

- `p`: `Pose2`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 53*

---

## pose2_compose

a then b expressed in a's frame (a (+) b).

```fides
pub fn pose2_compose(a: Pose2, b: Pose2) -> Pose2 {
```

**Parameters:**

- `a`: `Pose2`
- `b`: `Pose2`

**Returns:** `Pose2`

*Defined at line 58*

---

## odom_exp

The motion over dt at a constant body twist, in the body frame at the
start (the SE(2) exponential).

```fides
pub fn odom_exp(t: Twist2, dt: f64) -> Pose2 {
```

**Parameters:**

- `t`: `Twist2`
- `dt`: `f64`

**Returns:** `Pose2`

*Defined at line 68*

---

## odom_integrate

The pose after moving at a constant body twist for dt.

```fides
pub fn odom_integrate(p: Pose2, t: Twist2, dt: f64) -> Pose2 {
```

**Parameters:**

- `p`: `Pose2`
- `t`: `Twist2`
- `dt`: `f64`

**Returns:** `Pose2`

*Defined at line 86*

---

## odom_integrate_vw

The pose after dt at forward speed v and turn rate w (an exact arc).

```fides
pub fn odom_integrate_vw(p: Pose2, v: f64, w: f64, dt: f64) -> Pose2 {
```

**Parameters:**

- `p`: `Pose2`
- `v`: `f64`
- `w`: `f64`
- `dt`: `f64`

**Returns:** `Pose2`

*Defined at line 91*

---

## odom_integrate_euler

One explicit Euler step (for comparison; drifts on curves).

```fides
pub fn odom_integrate_euler(p: Pose2, v: f64, w: f64, dt: f64) -> Pose2 {
```

**Parameters:**

- `p`: `Pose2`
- `v`: `f64`
- `w`: `f64`
- `dt`: `f64`

**Returns:** `Pose2`

*Defined at line 98*

---

## DiffDrive

Wheel radius and track width (distance between the wheels).

```fides
pub struct DiffDrive {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `f64` |
| `track` | `f64` |

*Defined at line 107*

---

## diff_forward

(v, w) from left and right wheel speeds (rad/s).

```fides
pub fn diff_forward(d: &DiffDrive, wl: f64, wr: f64) -> Twist2 {
```

**Parameters:**

- `d`: `&DiffDrive`
- `wl`: `f64`
- `wr`: `f64`

**Returns:** `Twist2`

*Defined at line 113*

---

## diff_inverse

Left and right wheel speeds (rad/s) for (v, w): [left, right].

```fides
pub fn diff_inverse(d: &DiffDrive, v: f64, w: f64) -> Vec<f64> {
```

**Parameters:**

- `d`: `&DiffDrive`
- `v`: `f64`
- `w`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 120*

---

## DiffOdom

Odometry state of a differential drive: pose, covariance and the wheel
noise constants (variance per metre travelled by each wheel).

```fides
pub struct DiffOdom {
```

**Fields:**

| Name | Type |
|------|------|
| `track` | `f64` |
| `kl` | `f64` |
| `kr` | `f64` |
| `pose` | `Pose2` |
| `cov` | `DMat` |
| `dist` | `f64` |

*Defined at line 129*

---

## odom_diff_new

```fides
pub fn odom_diff_new(track: f64, kl: f64, kr: f64) -> DiffOdom {
```

**Parameters:**

- `track`: `f64`
- `kl`: `f64`
- `kr`: `f64`

**Returns:** `DiffOdom`

*Defined at line 138*

---

## odom_diff_step

The pose after the wheels travelled dl and dr metres (an exact arc).

```fides
pub fn odom_diff_step(p: Pose2, track: f64, dl: f64, dr: f64) -> Pose2 {
```

**Parameters:**

- `p`: `Pose2`
- `track`: `f64`
- `dl`: `f64`
- `dr`: `f64`

**Returns:** `Pose2`

*Defined at line 145*

---

## odom_diff_update

Advance the pose and its covariance by wheel distances dl, dr.

```fides
pub fn odom_diff_update(o: &mut DiffOdom, dl: f64, dr: f64) {
```

**Parameters:**

- `o`: `&mut DiffOdom`
- `dl`: `f64`
- `dr`: `f64`

*Defined at line 154*

---

## odom_ticks_to_m

Wheel distance (metres) from encoder ticks.

```fides
pub fn odom_ticks_to_m(ticks: i64, ticks_per_rev: i64, wheel_radius: f64) -> f64 {
```

**Parameters:**

- `ticks`: `i64`
- `ticks_per_rev`: `i64`
- `wheel_radius`: `f64`

**Returns:** `f64`

*Defined at line 187*

---

## Mecanum

Wheel radius, half wheelbase lx (front-rear) and half track ly.

```fides
pub struct Mecanum {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `f64` |
| `lx` | `f64` |
| `ly` | `f64` |

*Defined at line 194*

---

## mecanum_forward

Body twist from the wheel speeds [fl, fr, rl, rr] (rad/s).

```fides
pub fn mecanum_forward(m: &Mecanum, w: &Vec<f64>) -> Twist2 {
```

**Parameters:**

- `m`: `&Mecanum`
- `w`: `&Vec<f64>`

**Returns:** `Twist2`

*Defined at line 201*

---

## mecanum_inverse

Wheel speeds [fl, fr, rl, rr] (rad/s) for a body twist.

```fides
pub fn mecanum_inverse(m: &Mecanum, t: Twist2) -> Vec<f64> {
```

**Parameters:**

- `m`: `&Mecanum`
- `t`: `Twist2`

**Returns:** `Vec<f64>`

*Defined at line 213*

---

## mecanum_saturate

Scale wheel speeds so none exceeds wmax, keeping their ratios (the
direction of motion is kept when a command is too fast).

```fides
pub fn mecanum_saturate(w: &Vec<f64>, wmax: f64) -> Vec<f64> {
```

**Parameters:**

- `w`: `&Vec<f64>`
- `wmax`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 225*

---

## Ackermann

Wheelbase (front to rear axle) and track (between the front wheels).

```fides
pub struct Ackermann {
```

**Fields:**

| Name | Type |
|------|------|
| `wheelbase` | `f64` |
| `track` | `f64` |
| `max_steer` | `f64` |

*Defined at line 248*

---

## ackermann_forward

Body twist for speed v (at the rear axle) and steering angle delta.

```fides
pub fn ackermann_forward(a: &Ackermann, v: f64, delta: f64) -> Twist2 {
```

**Parameters:**

- `a`: `&Ackermann`
- `v`: `f64`
- `delta`: `f64`

**Returns:** `Twist2`

*Defined at line 255*

---

## ackermann_inverse

The steering angle for speed v and turn rate w, clamped to max_steer
(0 when v = 0).

```fides
pub fn ackermann_inverse(a: &Ackermann, v: f64, w: f64) -> f64 {
```

**Parameters:**

- `a`: `&Ackermann`
- `v`: `f64`
- `w`: `f64`

**Returns:** `f64`

*Defined at line 263*

---

## ackermann_radius

The turning radius at the rear axle centre (1e300 when straight).

```fides
pub fn ackermann_radius(a: &Ackermann, delta: f64) -> f64 {
```

**Parameters:**

- `a`: `&Ackermann`
- `delta`: `f64`

**Returns:** `f64`

*Defined at line 271*

---

## ackermann_wheel_angles

The front wheel angles [left, right] of true Ackermann geometry for a
bicycle steering angle delta (the inner wheel turns more).

```fides
pub fn ackermann_wheel_angles(a: &Ackermann, delta: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `&Ackermann`
- `delta`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 281*

---

## ackermann_rear_speeds

Rear wheel ground speeds [left, right] for speed v at the axle centre.

```fides
pub fn ackermann_rear_speeds(a: &Ackermann, v: f64, delta: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `&Ackermann`
- `v`: `f64`
- `delta`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 296*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
