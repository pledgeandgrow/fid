[All modules](index.md)

# imu -- Fides Documentation

## Contents

- [imu_gravity_body](#imu_gravity_body) (function)
- [imu_gyro_step](#imu_gyro_step) (function)
- [Madgwick](#Madgwick) (struct)
- [madgwick_new](#madgwick_new) (function)
- [madgwick_update_imu](#madgwick_update_imu) (function)
- [madgwick_update](#madgwick_update) (function)
- [Mahony](#Mahony) (struct)
- [mahony_new](#mahony_new) (function)
- [mahony_update_imu](#mahony_update_imu) (function)
- [mahony_update](#mahony_update) (function)
- [GyroBias](#GyroBias) (struct)
- [gyro_bias_new](#gyro_bias_new) (function)
- [gyro_bias_feed](#gyro_bias_feed) (function)
- [gyro_bias_apply](#gyro_bias_apply) (function)
- [AxisCal](#AxisCal) (struct)
- [axiscal_identity](#axiscal_identity) (function)
- [axiscal_apply](#axiscal_apply) (function)
- [accel_cal_from_6](#accel_cal_from_6) (function)
- [MagMinMax](#MagMinMax) (struct)
- [mag_minmax_new](#mag_minmax_new) (function)
- [mag_minmax_feed](#mag_minmax_feed) (function)
- [mag_cal](#mag_cal) (function)
- [mag_heading](#mag_heading) (function)
- [imu_linear_accel_earth](#imu_linear_accel_earth) (function)
- [imu_linear_accel_body](#imu_linear_accel_body) (function)
- [imu_tilt_from_accel](#imu_tilt_from_accel) (function)

## imu_gravity_body

The gravity direction (unit, body frame) an orientation predicts.

```fides
pub fn imu_gravity_body(q: quat) -> vec3 {
```

**Parameters:**

- `q`: `quat`

**Returns:** `vec3`

*Defined at line 55*

---

## imu_gyro_step

q advanced by the gyro rates over dt (q += 0.5 q (0, w) dt, normalized).

```fides
pub fn imu_gyro_step(q: quat, g: vec3, dt: f64) -> quat {
```

**Parameters:**

- `q`: `quat`
- `g`: `vec3`
- `dt`: `f64`

**Returns:** `quat`

*Defined at line 60*

---

## Madgwick

```fides
pub struct Madgwick {
```

**Fields:**

| Name | Type |
|------|------|
| `q` | `quat` |
| `beta` | `f64` |

*Defined at line 70*

---

## madgwick_new

```fides
pub fn madgwick_new(beta: f64) -> Madgwick {
```

**Parameters:**

- `beta`: `f64`

**Returns:** `Madgwick`

*Defined at line 75*

---

## madgwick_update_imu

One 6-axis update (gyro rad/s, accel in any unit, dt seconds). A zero
accelerometer reading integrates the gyro alone.

```fides
pub fn madgwick_update_imu(f: &mut Madgwick, g: vec3, a: vec3, dt: f64) {
```

**Parameters:**

- `f`: `&mut Madgwick`
- `g`: `vec3`
- `a`: `vec3`
- `dt`: `f64`

*Defined at line 83*

---

## madgwick_update

One 9-axis update; a zero magnetometer reading falls back to 6 axes.

```fides
pub fn madgwick_update(f: &mut Madgwick, g: vec3, a: vec3, m: vec3, dt: f64) {
```

**Parameters:**

- `f`: `&mut Madgwick`
- `g`: `vec3`
- `a`: `vec3`
- `m`: `vec3`
- `dt`: `f64`

*Defined at line 88*

---

## Mahony

```fides
pub struct Mahony {
```

**Fields:**

| Name | Type |
|------|------|
| `q` | `quat` |
| `kp` | `f64` |
| `ki` | `f64` |
| `ix` | `f64` |
| `iy` | `f64` |
| `iz` | `f64` |

*Defined at line 136*

---

## mahony_new

```fides
pub fn mahony_new(kp: f64, ki: f64) -> Mahony {
```

**Parameters:**

- `kp`: `f64`
- `ki`: `f64`

**Returns:** `Mahony`

*Defined at line 146*

---

## mahony_update_imu

```fides
pub fn mahony_update_imu(f: &mut Mahony, g: vec3, a: vec3, dt: f64) {
```

**Parameters:**

- `f`: `&mut Mahony`
- `g`: `vec3`
- `a`: `vec3`
- `dt`: `f64`

*Defined at line 152*

---

## mahony_update

One update; zero accel skips the correction, zero mag uses 6 axes.

```fides
pub fn mahony_update(f: &mut Mahony, gin: vec3, a: vec3, m: vec3, dt: f64) {
```

**Parameters:**

- `f`: `&mut Mahony`
- `gin`: `vec3`
- `a`: `vec3`
- `m`: `vec3`
- `dt`: `f64`

*Defined at line 157*

---

## GyroBias

```fides
pub struct GyroBias {
```

**Fields:**

| Name | Type |
|------|------|
| `gyro_thresh` | `f64` |
| `acc_tol` | `f64` |
| `g` | `f64` |
| `needed` | `i64` |
| `n` | `i64` |
| `sx` | `f64` |
| `sy` | `f64` |
| `sz` | `f64` |
| `bias` | `vec3` |
| `valid` | `bool` |

*Defined at line 208*

---

## gyro_bias_new

```fides
pub fn gyro_bias_new(gyro_thresh: f64, acc_tol: f64, g: f64, needed: i64) -> GyroBias {
```

**Parameters:**

- `gyro_thresh`: `f64`
- `acc_tol`: `f64`
- `g`: `f64`
- `needed`: `i64`

**Returns:** `GyroBias`

*Defined at line 223*

---

## gyro_bias_feed

Feed a sample; motion restarts the run. Returns true when a new bias
estimate (the mean of the last `needed` resting samples) was taken.

```fides
pub fn gyro_bias_feed(b: &mut GyroBias, gyro: vec3, acc: vec3) -> bool {
```

**Parameters:**

- `b`: `&mut GyroBias`
- `gyro`: `vec3`
- `acc`: `vec3`

**Returns:** `bool`

*Defined at line 231*

---

## gyro_bias_apply

A gyro sample with the bias removed.

```fides
pub fn gyro_bias_apply(b: &GyroBias, gyro: vec3) -> vec3 {
```

**Parameters:**

- `b`: `&GyroBias`
- `gyro`: `vec3`

**Returns:** `vec3`

*Defined at line 258*

---

## AxisCal

Per-axis offset and scale: corrected = (raw - offset) * scale.

```fides
pub struct AxisCal {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `vec3` |
| `scale` | `vec3` |

*Defined at line 265*

---

## axiscal_identity

```fides
pub fn axiscal_identity() -> AxisCal {
```

**Returns:** `AxisCal`

*Defined at line 270*

---

## axiscal_apply

```fides
pub fn axiscal_apply(c: &AxisCal, raw: vec3) -> vec3 {
```

**Parameters:**

- `c`: `&AxisCal`
- `raw`: `vec3`

**Returns:** `vec3`

*Defined at line 276*

---

## accel_cal_from_6

Accelerometer calibration from the readings with each axis pointing up
(`up`) and down (`down`): up.x is the x reading with +x up, and so on.
offset = (up + down) / 2, scale = 2 g / (up - down).

```fides
pub fn accel_cal_from_6(up: vec3, down: vec3, g: f64) -> AxisCal {
```

**Parameters:**

- `up`: `vec3`
- `down`: `vec3`
- `g`: `f64`

**Returns:** `AxisCal`

*Defined at line 283*

---

## MagMinMax

Running min/max of magnetometer samples while the sensor is turned.

```fides
pub struct MagMinMax {
```

**Fields:**

| Name | Type |
|------|------|
| `lo` | `vec3` |
| `hi` | `vec3` |
| `n` | `i64` |

*Defined at line 290*

---

## mag_minmax_new

```fides
pub fn mag_minmax_new() -> MagMinMax {
```

**Returns:** `MagMinMax`

*Defined at line 296*

---

## mag_minmax_feed

```fides
pub fn mag_minmax_feed(mm: &mut MagMinMax, m: vec3) {
```

**Parameters:**

- `mm`: `&mut MagMinMax`
- `m`: `vec3`

*Defined at line 302*

---

## mag_cal

Hard iron offset = centre of the box; soft iron (axis aligned) scale =
mean radius / axis radius, so the corrected samples lie on a sphere of
the mean radius.

```fides
pub fn mag_cal(mm: &MagMinMax) -> AxisCal {
```

**Parameters:**

- `mm`: `&MagMinMax`

**Returns:** `AxisCal`

*Defined at line 311*

---

## mag_heading

Tilt-compensated heading (radians from magnetic north, positive
counter-clockwise about up) of a body-frame field at orientation q
whose roll and pitch are trusted.

```fides
pub fn mag_heading(q: quat, m: vec3) -> f64 {
```

**Parameters:**

- `q`: `quat`
- `m`: `vec3`

**Returns:** `f64`

*Defined at line 324*

---

## imu_linear_accel_earth

Linear acceleration in the earth frame: q a q* - (0, 0, g).

```fides
pub fn imu_linear_accel_earth(q: quat, acc: vec3, g: f64) -> vec3 {
```

**Parameters:**

- `q`: `quat`
- `acc`: `vec3`
- `g`: `f64`

**Returns:** `vec3`

*Defined at line 335*

---

## imu_linear_accel_body

Linear acceleration in the body frame: a - g * (gravity direction).

```fides
pub fn imu_linear_accel_body(q: quat, acc: vec3, g: f64) -> vec3 {
```

**Parameters:**

- `q`: `quat`
- `acc`: `vec3`
- `g`: `f64`

**Returns:** `vec3`

*Defined at line 341*

---

## imu_tilt_from_accel

Roll and pitch from one accelerometer sample (no gyro): a quaternion
with yaw 0.

```fides
pub fn imu_tilt_from_accel(a: vec3) -> quat {
```

**Parameters:**

- `a`: `vec3`

**Returns:** `quat`

*Defined at line 348*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
