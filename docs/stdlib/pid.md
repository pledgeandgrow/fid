[All modules](index.md)

# pid -- Fides Documentation

## Contents

- [PidCtl](#PidCtl) (struct)
- [pidc_new](#pidc_new) (function)
- [pidc_set_output_limits](#pidc_set_output_limits) (function)
- [pidc_set_integral_limits](#pidc_set_integral_limits) (function)
- [pidc_set_derivative_filter](#pidc_set_derivative_filter) (function)
- [pidc_set_anti_windup](#pidc_set_anti_windup) (function)
- [pidc_set_gains](#pidc_set_gains) (function)
- [pidc_reset](#pidc_reset) (function)
- [pidc_update](#pidc_update) (function)
- [PidGains](#PidGains) (struct)
- [GainSchedule](#GainSchedule) (struct)
- [gsched_new](#gsched_new) (function)
- [gsched_add](#gsched_add) (function)
- [gsched_len](#gsched_len) (function)
- [gsched_lookup](#gsched_lookup) (function)
- [pidc_apply_schedule](#pidc_apply_schedule) (function)
- [FirstOrderPlant](#FirstOrderPlant) (struct)
- [fop_new](#fop_new) (function)
- [fop_step](#fop_step) (function)

## PidCtl

PID state and settings.

```fides
pub struct PidCtl {
```

**Fields:**

| Name | Type |
|------|------|
| `kp` | `f64` |
| `ki` | `f64` |
| `kd` | `f64` |
| `tau_d` | `f64` |
| `out_min` | `f64` |
| `out_max` | `f64` |
| `int_min` | `f64` |
| `int_max` | `f64` |
| `anti_windup` | `bool` |
| `i_term` | `f64` |
| `d_filt` | `f64` |
| `prev_meas` | `f64` |
| `started` | `bool` |
| `last_p` | `f64` |
| `last_i` | `f64` |
| `last_d` | `f64` |
| `last_out` | `f64` |
| `saturated` | `bool` |

*Defined at line 34*

---

## pidc_new

A controller with gains kp, ki, kd, no limits (+-1e300), no derivative
filter, anti-windup on.

```fides
pub fn pidc_new(kp: f64, ki: f64, kd: f64) -> PidCtl {
```

**Parameters:**

- `kp`: `f64`
- `ki`: `f64`
- `kd`: `f64`

**Returns:** `PidCtl`

*Defined at line 62*

---

## pidc_set_output_limits

Output limits.

```fides
pub fn pidc_set_output_limits(c: &mut PidCtl, lo: f64, hi: f64) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `lo`: `f64`
- `hi`: `f64`

*Defined at line 69*

---

## pidc_set_integral_limits

Limits of the I term.

```fides
pub fn pidc_set_integral_limits(c: &mut PidCtl, lo: f64, hi: f64) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `lo`: `f64`
- `hi`: `f64`

*Defined at line 75*

---

## pidc_set_derivative_filter

Time constant of the derivative low-pass filter (seconds; 0 = none).

```fides
pub fn pidc_set_derivative_filter(c: &mut PidCtl, tau: f64) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `tau`: `f64`

*Defined at line 82*

---

## pidc_set_anti_windup

Turn anti-windup (conditional integration) on or off.

```fides
pub fn pidc_set_anti_windup(c: &mut PidCtl, on: bool) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `on`: `bool`

*Defined at line 87*

---

## pidc_set_gains

New gains; the I term is kept, so the output does not jump.

```fides
pub fn pidc_set_gains(c: &mut PidCtl, kp: f64, ki: f64, kd: f64) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `kp`: `f64`
- `ki`: `f64`
- `kd`: `f64`

*Defined at line 92*

---

## pidc_reset

Forget the integral, the derivative and the previous measurement.

```fides
pub fn pidc_reset(c: &mut PidCtl) {
```

**Parameters:**

- `c`: `&mut PidCtl`

*Defined at line 99*

---

## pidc_update

One control step: the output for `setpoint` given `measurement`, dt
seconds after the previous step. dt <= 0 holds the integral and the
derivative and returns the proportional action on the old terms.

```fides
pub fn pidc_update(c: &mut PidCtl, setpoint: f64, measurement: f64, dt: f64) -> f64 {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `setpoint`: `f64`
- `measurement`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 124*

---

## PidGains

Gains at one operating point.

```fides
pub struct PidGains {
```

**Fields:**

| Name | Type |
|------|------|
| `kp` | `f64` |
| `ki` | `f64` |
| `kd` | `f64` |

*Defined at line 167*

---

## GainSchedule

Rows (x, kp, ki, kd), sorted by x.

```fides
pub struct GainSchedule {
```

**Fields:**

| Name | Type |
|------|------|
| `xs` | `Vec<f64>` |
| `kps` | `Vec<f64>` |
| `kis` | `Vec<f64>` |
| `kds` | `Vec<f64>` |

*Defined at line 174*

---

## gsched_new

An empty schedule.

```fides
pub fn gsched_new() -> GainSchedule {
```

**Returns:** `GainSchedule`

*Defined at line 182*

---

## gsched_add

Add (or replace) the row at x, keeping the rows sorted.

```fides
pub fn gsched_add(s: &mut GainSchedule, x: f64, kp: f64, ki: f64, kd: f64) {
```

**Parameters:**

- `s`: `&mut GainSchedule`
- `x`: `f64`
- `kp`: `f64`
- `ki`: `f64`
- `kd`: `f64`

*Defined at line 189*

---

## gsched_len

The number of rows.

```fides
pub fn gsched_len(s: &GainSchedule) -> i64 {
```

**Parameters:**

- `s`: `&GainSchedule`

**Returns:** `i64`

*Defined at line 207*

---

## gsched_lookup

The gains at x: linear between the two rows around x, the first or
last row outside the table; zero gains for an empty table.

```fides
pub fn gsched_lookup(s: &GainSchedule, x: f64) -> PidGains {
```

**Parameters:**

- `s`: `&GainSchedule`
- `x`: `f64`

**Returns:** `PidGains`

*Defined at line 213*

---

## pidc_apply_schedule

Set the controller's gains from the schedule at x (bumpless: the I term
is kept).

```fides
pub fn pidc_apply_schedule(c: &mut PidCtl, s: &GainSchedule, x: f64) {
```

**Parameters:**

- `c`: `&mut PidCtl`
- `s`: `&GainSchedule`
- `x`: `f64`

*Defined at line 244*

---

## FirstOrderPlant

y' = (gain * u - y) / tau.

```fides
pub struct FirstOrderPlant {
```

**Fields:**

| Name | Type |
|------|------|
| `gain` | `f64` |
| `tau` | `f64` |
| `y` | `f64` |

*Defined at line 252*

---

## fop_new

A plant at rest at y0.

```fides
pub fn fop_new(gain: f64, tau: f64, y0: f64) -> FirstOrderPlant {
```

**Parameters:**

- `gain`: `f64`
- `tau`: `f64`
- `y0`: `f64`

**Returns:** `FirstOrderPlant`

*Defined at line 259*

---

## fop_step

Advance dt seconds with the input held at u (exact for a held input);
returns the new output.

```fides
pub fn fop_step(p: &mut FirstOrderPlant, u: f64, dt: f64) -> f64 {
```

**Parameters:**

- `p`: `&mut FirstOrderPlant`
- `u`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 267*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
