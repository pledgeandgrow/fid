[All modules](index.md)

# motion -- Fides Documentation

## Contents

- [ASENSOR_TYPE_ACCELEROMETER:](#ASENSOR_TYPE_ACCELEROMETER:) (const)
- [ASENSOR_TYPE_MAGNETIC_FIELD:](#ASENSOR_TYPE_MAGNETIC_FIELD:) (const)
- [ASENSOR_TYPE_GYROSCOPE:](#ASENSOR_TYPE_GYROSCOPE:) (const)
- [ASENSOR_TYPE_GRAVITY:](#ASENSOR_TYPE_GRAVITY:) (const)
- [ASENSOR_TYPE_LINEAR_ACCELERATION:](#ASENSOR_TYPE_LINEAR_ACCELERATION:) (const)
- [ASENSOR_TYPE_ROTATION_VECTOR:](#ASENSOR_TYPE_ROTATION_VECTOR:) (const)
- [ASENSOR_TYPE_STEP_DETECTOR:](#ASENSOR_TYPE_STEP_DETECTOR:) (const)
- [ASENSOR_TYPE_STEP_COUNTER:](#ASENSOR_TYPE_STEP_COUNTER:) (const)
- [ASENSOR_EVENT_SIZE:](#ASENSOR_EVENT_SIZE:) (const)
- [MOTION_G:](#MOTION_G:) (const)
- [SensorEvent](#SensorEvent) (struct)
- [motion_read_sensor_event](#motion_read_sensor_event) (function)
- [motion_event_from_bits](#motion_event_from_bits) (function)
- [MVec](#MVec) (struct)
- [mvec](#mvec) (function)
- [mvec_norm](#mvec_norm) (function)
- [mvec_cross](#mvec_cross) (function)
- [mvec_scale](#mvec_scale) (function)
- [motion_from_ios_accel](#motion_from_ios_accel) (function)
- [motion_pi](#motion_pi) (function)
- [motion_deg](#motion_deg) (function)
- [motion_rad](#motion_rad) (function)
- [motion_atan](#motion_atan) (function)
- [motion_atan2](#motion_atan2) (function)
- [GravityFilter](#GravityFilter) (struct)
- [gravity_filter_new](#gravity_filter_new) (function)
- [gravity_filter_update](#gravity_filter_update) (function)
- [motion_tilt_x](#motion_tilt_x) (function)
- [motion_tilt_y](#motion_tilt_y) (function)
- [Complementary](#Complementary) (struct)
- [complementary_new](#complementary_new) (function)
- [complementary_update](#complementary_update) (function)
- [Orientation](#Orientation) (struct)
- [motion_orientation](#motion_orientation) (function)
- [motion_true_heading](#motion_true_heading) (function)
- [motion_compass_point](#motion_compass_point) (function)
- [StepDetector](#StepDetector) (struct)
- [step_detector_new](#step_detector_new) (function)
- [step_detector_update](#step_detector_update) (function)
- [step_cadence](#step_cadence) (function)
- [ShakeDetector](#ShakeDetector) (struct)
- [shake_detector_new](#shake_detector_new) (function)
- [shake_detector_update](#shake_detector_update) (function)

## ASENSOR_TYPE_ACCELEROMETER:

```fides
pub const ASENSOR_TYPE_ACCELEROMETER: i64 = 1;
```

*Defined at line 47*

---

## ASENSOR_TYPE_MAGNETIC_FIELD:

```fides
pub const ASENSOR_TYPE_MAGNETIC_FIELD: i64 = 2;
```

*Defined at line 48*

---

## ASENSOR_TYPE_GYROSCOPE:

```fides
pub const ASENSOR_TYPE_GYROSCOPE: i64 = 4;
```

*Defined at line 49*

---

## ASENSOR_TYPE_GRAVITY:

```fides
pub const ASENSOR_TYPE_GRAVITY: i64 = 9;
```

*Defined at line 50*

---

## ASENSOR_TYPE_LINEAR_ACCELERATION:

```fides
pub const ASENSOR_TYPE_LINEAR_ACCELERATION: i64 = 10;
```

*Defined at line 51*

---

## ASENSOR_TYPE_ROTATION_VECTOR:

```fides
pub const ASENSOR_TYPE_ROTATION_VECTOR: i64 = 11;
```

*Defined at line 52*

---

## ASENSOR_TYPE_STEP_DETECTOR:

```fides
pub const ASENSOR_TYPE_STEP_DETECTOR: i64 = 18;
```

*Defined at line 53*

---

## ASENSOR_TYPE_STEP_COUNTER:

```fides
pub const ASENSOR_TYPE_STEP_COUNTER: i64 = 19;
```

*Defined at line 54*

---

## ASENSOR_EVENT_SIZE:

```fides
pub const ASENSOR_EVENT_SIZE: i64 = 104;
```

*Defined at line 55*

---

## MOTION_G:

```fides
pub const MOTION_G: f64 = 9.80665;
```

*Defined at line 56*

---

## SensorEvent

One decoded ASensorEvent.

```fides
pub struct SensorEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `sensor_type` | `i64` |
| `timestamp_ns` | `i64` |
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |

*Defined at line 59*

---

## motion_read_sensor_event

Decode an ASensorEvent at `addr`: version, sensor, type (int32 each),
reserved, int64 timestamp at 16, then float data[16] at 24.

```fides
pub fn motion_read_sensor_event(addr: i64) -> SensorEvent {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `SensorEvent`

*Defined at line 97*

---

## motion_event_from_bits

The sensor event of three float bit patterns (for tests and replays).

```fides
pub fn motion_event_from_bits(kind: i64, ts: i64, xb: i64, yb: i64, zb: i64) -> SensorEvent {
```

**Parameters:**

- `kind`: `i64`
- `ts`: `i64`
- `xb`: `i64`
- `yb`: `i64`
- `zb`: `i64`

**Returns:** `SensorEvent`

*Defined at line 122*

---

## MVec

```fides
pub struct MVec {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |

*Defined at line 130*

---

## mvec

```fides
pub fn mvec(x: f64, y: f64, z: f64) -> MVec {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `MVec`

*Defined at line 136*

---

## mvec_norm

```fides
pub fn mvec_norm(v: MVec) -> f64 {
```

**Parameters:**

- `v`: `MVec`

**Returns:** `f64`

*Defined at line 141*

---

## mvec_cross

```fides
pub fn mvec_cross(a: MVec, b: MVec) -> MVec {
```

**Parameters:**

- `a`: `MVec`
- `b`: `MVec`

**Returns:** `MVec`

*Defined at line 144*

---

## mvec_scale

```fides
pub fn mvec_scale(a: MVec, k: f64) -> MVec {
```

**Parameters:**

- `a`: `MVec`
- `k`: `f64`

**Returns:** `MVec`

*Defined at line 149*

---

## motion_from_ios_accel

iOS CMAcceleration (g, gravity negative z when flat) -> m/s^2 here.

```fides
pub fn motion_from_ios_accel(x: f64, y: f64, z: f64) -> MVec {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `MVec`

*Defined at line 156*

---

## motion_pi

```fides
pub fn motion_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 160*

---

## motion_deg

```fides
pub fn motion_deg(r: f64) -> f64 {
```

**Parameters:**

- `r`: `f64`

**Returns:** `f64`

*Defined at line 163*

---

## motion_rad

```fides
pub fn motion_rad(d: f64) -> f64 {
```

**Parameters:**

- `d`: `f64`

**Returns:** `f64`

*Defined at line 166*

---

## motion_atan

atan(x) by two argument halvings and a Taylor series.

```fides
pub fn motion_atan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 171*

---

## motion_atan2

```fides
pub fn motion_atan2(y: f64, x: f64) -> f64 {
```

**Parameters:**

- `y`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 197*

---

## GravityFilter

```fides
pub struct GravityFilter {
```

**Fields:**

| Name | Type |
|------|------|
| `alpha` | `f64,  // 0.8 in the Android documentation` |
| `gravity` | `MVec` |
| `started` | `bool` |

*Defined at line 218*

---

## gravity_filter_new

```fides
pub fn gravity_filter_new(alpha: f64) -> GravityFilter {
```

**Parameters:**

- `alpha`: `f64`

**Returns:** `GravityFilter`

*Defined at line 224*

---

## gravity_filter_update

Update with an accelerometer sample; returns the linear acceleration
(the sample minus the low-passed gravity).

```fides
pub fn gravity_filter_update(f: &mut GravityFilter, a: MVec) -> MVec {
```

**Parameters:**

- `f`: `&mut GravityFilter`
- `a`: `MVec`

**Returns:** `MVec`

*Defined at line 232*

---

## motion_tilt_x

Rotation about the device x axis implied by gravity (radians):
positive when the top edge rises.

```fides
pub fn motion_tilt_x(a: MVec) -> f64 {
```

**Parameters:**

- `a`: `MVec`

**Returns:** `f64`

*Defined at line 247*

---

## motion_tilt_y

Rotation about the device y axis implied by gravity (radians).

```fides
pub fn motion_tilt_y(a: MVec) -> f64 {
```

**Parameters:**

- `a`: `MVec`

**Returns:** `f64`

*Defined at line 252*

---

## Complementary

```fides
pub struct Complementary {
```

**Fields:**

| Name | Type |
|------|------|
| `alpha` | `f64,  // weight of the gyro path, e.g. 0.98` |
| `angle_x` | `f64,  // radians` |
| `angle_y` | `f64` |
| `started` | `bool` |

*Defined at line 256*

---

## complementary_new

```fides
pub fn complementary_new(alpha: f64) -> Complementary {
```

**Parameters:**

- `alpha`: `f64`

**Returns:** `Complementary`

*Defined at line 263*

---

## complementary_update

Fuse one accelerometer + gyroscope sample taken `dt` seconds after the
previous one.

```fides
pub fn complementary_update(c: &mut Complementary, a: MVec, gyro: MVec, dt: f64) {
```

**Parameters:**

- `c`: `&mut Complementary`
- `a`: `MVec`
- `gyro`: `MVec`
- `dt`: `f64`

*Defined at line 271*

---

## Orientation

```fides
pub struct Orientation {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool,  // false in free fall or near a magnetic pole` |
| `azimuth` | `f64,  // degrees clockwise from magnetic north, [0, 360)` |
| `pitch` | `f64,  // degrees (SensorManager.getOrientation)` |
| `roll` | `f64` |

*Defined at line 286*

---

## motion_orientation

SensorManager.getRotationMatrix + getOrientation: gravity `a` and the
geomagnetic field `e` in the device frame.

```fides
pub fn motion_orientation(a: MVec, e: MVec) -> Orientation {
```

**Parameters:**

- `a`: `MVec`
- `e`: `MVec`

**Returns:** `Orientation`

*Defined at line 295*

---

## motion_true_heading

True heading from a magnetic one and the local declination (east +).

```fides
pub fn motion_true_heading(magnetic: f64, declination: f64) -> f64 {
```

**Parameters:**

- `magnetic`: `f64`
- `declination`: `f64`

**Returns:** `f64`

*Defined at line 327*

---

## motion_compass_point

The 16-point compass name of a heading ("N", "NNE", ...).

```fides
pub fn motion_compass_point(deg: f64) -> str {
```

**Parameters:**

- `deg`: `f64`

**Returns:** `str`

*Defined at line 339*

---

## StepDetector

```fides
pub struct StepDetector {
```

**Fields:**

| Name | Type |
|------|------|
| `threshold` | `f64,  // m/s^2 above g` |
| `min_interval_ms` | `i64,  // refractory period` |
| `smooth` | `f64,  // EMA weight of the new sample` |
| `level` | `f64` |
| `prev` | `f64` |
| `prev2` | `f64` |
| `last_step_ms` | `i64` |
| `steps` | `i64` |
| `intervals_sum` | `i64` |
| `samples` | `i64` |

*Defined at line 347*

---

## step_detector_new

```fides
pub fn step_detector_new(threshold: f64, min_interval_ms: i64) -> StepDetector {
```

**Parameters:**

- `threshold`: `f64`
- `min_interval_ms`: `i64`

**Returns:** `StepDetector`

*Defined at line 360*

---

## step_detector_update

Feed one accelerometer sample; true when it completes a step (the
previous smoothed value was a local peak above g + threshold).

```fides
pub fn step_detector_update(s: &mut StepDetector, a: MVec, t_ms: i64) -> bool {
```

**Parameters:**

- `s`: `&mut StepDetector`
- `a`: `MVec`
- `t_ms`: `i64`

**Returns:** `bool`

*Defined at line 368*

---

## step_cadence

Steps per minute from the mean interval (0 before two steps).

```fides
pub fn step_cadence(s: &StepDetector) -> f64 {
```

**Parameters:**

- `s`: `&StepDetector`

**Returns:** `f64`

*Defined at line 393*

---

## ShakeDetector

```fides
pub struct ShakeDetector {
```

**Fields:**

| Name | Type |
|------|------|
| `threshold_g` | `f64,  // spike when |a| > threshold * g (2.7 is common)` |
| `window_ms` | `i64` |
| `min_spikes` | `i64` |
| `cooldown_ms` | `i64` |
| `spikes` | `Vec<i64>,  // times of recent spikes` |
| `last_shake_ms` | `i64` |
| `shakes` | `i64` |

*Defined at line 402*

---

## shake_detector_new

```fides
pub fn shake_detector_new(threshold_g: f64, window_ms: i64, min_spikes: i64, cooldown_ms: i64) -> ShakeDetector {
```

**Parameters:**

- `threshold_g`: `f64`
- `window_ms`: `i64`
- `min_spikes`: `i64`
- `cooldown_ms`: `i64`

**Returns:** `ShakeDetector`

*Defined at line 412*

---

## shake_detector_update

Feed a sample; true when it completes a shake gesture.

```fides
pub fn shake_detector_update(s: &mut ShakeDetector, a: MVec, t_ms: i64) -> bool {
```

**Parameters:**

- `s`: `&mut ShakeDetector`
- `a`: `MVec`
- `t_ms`: `i64`

**Returns:** `bool`

*Defined at line 419*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
