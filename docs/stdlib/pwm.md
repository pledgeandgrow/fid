[All modules](index.md)

# pwm -- Fides Documentation

## Contents

- [PWM_POLARITY_NORMAL:](#PWM_POLARITY_NORMAL:) (const)
- [PWM_POLARITY_INVERSED:](#PWM_POLARITY_INVERSED:) (const)
- [PWM_NS_PER_S:](#PWM_NS_PER_S:) (const)
- [PwmState](#PwmState) (struct)
- [pwm_state](#pwm_state) (function)
- [pwm_off](#pwm_off) (function)
- [pwm_period_for_hz](#pwm_period_for_hz) (function)
- [pwm_hz_for_period](#pwm_hz_for_period) (function)
- [pwm_duty_permille](#pwm_duty_permille) (function)
- [pwm_duty_percent](#pwm_duty_percent) (function)
- [pwm_percent](#pwm_percent) (function)
- [pwm_check](#pwm_check) (function)
- [pwm_level_at](#pwm_level_at) (function)
- [pwm_measured_duty](#pwm_measured_duty) (function)
- [PwmWrite](#PwmWrite) (struct)
- [pwm_sysfs_channel](#pwm_sysfs_channel) (function)
- [pwm_sysfs_plan](#pwm_sysfs_plan) (function)
- [pwm_sysfs_apply](#pwm_sysfs_apply) (function)
- [pwm_sysfs_read](#pwm_sysfs_read) (function)
- [PwmFakeChip](#PwmFakeChip) (struct)
- [pwm_fake_chip](#pwm_fake_chip) (function)
- [pwm_fake_write](#pwm_fake_write) (function)
- [pwm_fake_apply](#pwm_fake_apply) (function)
- [Servo](#Servo) (struct)
- [servo_standard](#servo_standard) (function)
- [servo_new](#servo_new) (function)
- [servo_pulse_ns](#servo_pulse_ns) (function)
- [servo_angle](#servo_angle) (function)
- [servo_state](#servo_state) (function)
- [servo_speed_pulse_ns](#servo_speed_pulse_ns) (function)
- [PwmTimer](#PwmTimer) (struct)
- [pwm_timer_setup](#pwm_timer_setup) (function)

## PWM_POLARITY_NORMAL:

```fides
pub const PWM_POLARITY_NORMAL: i64 = 0;
```

*Defined at line 25*

---

## PWM_POLARITY_INVERSED:

```fides
pub const PWM_POLARITY_INVERSED: i64 = 1;
```

*Defined at line 26*

---

## PWM_NS_PER_S:

```fides
pub const PWM_NS_PER_S: i64 = 1000000000;
```

*Defined at line 27*

---

## PwmState

```fides
pub struct PwmState {
```

**Fields:**

| Name | Type |
|------|------|
| `period_ns` | `i64` |
| `duty_ns` | `i64` |
| `polarity` | `i64` |
| `enabled` | `bool` |

*Defined at line 29*

---

## pwm_state

```fides
pub fn pwm_state(period_ns: i64, duty_ns: i64) -> PwmState {
```

**Parameters:**

- `period_ns`: `i64`
- `duty_ns`: `i64`

**Returns:** `PwmState`

*Defined at line 36*

---

## pwm_off

```fides
pub fn pwm_off() -> PwmState {
```

**Returns:** `PwmState`

*Defined at line 42*

---

## pwm_period_for_hz

Period for a frequency (rounded to the nearest ns).

```fides
pub fn pwm_period_for_hz(hz: i64) -> i64 {
```

**Parameters:**

- `hz`: `i64`

**Returns:** `i64`

*Defined at line 49*

---

## pwm_hz_for_period

```fides
pub fn pwm_hz_for_period(period_ns: i64) -> f64 {
```

**Parameters:**

- `period_ns`: `i64`

**Returns:** `f64`

*Defined at line 56*

---

## pwm_duty_permille

Duty for a fraction in per-mille (0..1000) of the period.

```fides
pub fn pwm_duty_permille(period_ns: i64, permille: i64) -> i64 {
```

**Parameters:**

- `period_ns`: `i64`
- `permille`: `i64`

**Returns:** `i64`

*Defined at line 64*

---

## pwm_duty_percent

```fides
pub fn pwm_duty_percent(period_ns: i64, percent: f64) -> i64 {
```

**Parameters:**

- `period_ns`: `i64`
- `percent`: `f64`

**Returns:** `i64`

*Defined at line 75*

---

## pwm_percent

The duty cycle as a percentage of the period.

```fides
pub fn pwm_percent(s: &PwmState) -> f64 {
```

**Parameters:**

- `s`: `&PwmState`

**Returns:** `f64`

*Defined at line 87*

---

## pwm_check

Why a state cannot be applied, or "" when it can.

```fides
pub fn pwm_check(s: &PwmState) -> str {
```

**Parameters:**

- `s`: `&PwmState`

**Returns:** `str`

*Defined at line 95*

---

## pwm_level_at

The output level at time t (0 = start of a period): high for the
first duty_ns of each period (low for inversed polarity); a disabled
output sits at its inactive level.

```fides
pub fn pwm_level_at(s: &PwmState, t_ns: i64) -> i64 {
```

**Parameters:**

- `s`: `&PwmState`
- `t_ns`: `i64`

**Returns:** `i64`

*Defined at line 114*

---

## pwm_measured_duty

Average output (0..1) over n samples of one period: checks a state or
a software PWM against its intended duty.

```fides
pub fn pwm_measured_duty(s: &PwmState, samples: i64) -> f64 {
```

**Parameters:**

- `s`: `&PwmState`
- `samples`: `i64`

**Returns:** `f64`

*Defined at line 131*

---

## PwmWrite

```fides
pub struct PwmWrite {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `value` | `str` |

*Defined at line 146*

---

## pwm_sysfs_channel

```fides
pub fn pwm_sysfs_channel(root: str, chip: i64, ch: i64) -> str {
```

**Parameters:**

- `root`: `str`
- `chip`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 151*

---

## pwm_sysfs_plan

The writes that take a channel from `old` to `new`. Export when the
channel is new (old = pwm_off() and exported=false). Polarity changes
with the output disabled. Period and duty are ordered so that duty <=
period holds after each write.

```fides
pub fn pwm_sysfs_plan(root: str, chip: i64, ch: i64, exported: bool, old: &PwmState, new: &PwmState) -> Vec<PwmWrite> {
```

**Parameters:**

- `root`: `str`
- `chip`: `i64`
- `ch`: `i64`
- `exported`: `bool`
- `old`: `&PwmState`
- `new`: `&PwmState`

**Returns:** `Vec<PwmWrite>`

*Defined at line 159*

---

## pwm_sysfs_apply

```fides
pub fn pwm_sysfs_apply(writes: &Vec<PwmWrite>) -> i64 {
```

**Parameters:**

- `writes`: `&Vec<PwmWrite>`

**Returns:** `i64`

*Defined at line 221*

---

## pwm_sysfs_read

Read a channel's state back from its sysfs files.

```fides
pub fn pwm_sysfs_read(root: str, chip: i64, ch: i64) -> Result<PwmState, str> {
```

**Parameters:**

- `root`: `str`
- `chip`: `i64`
- `ch`: `i64`

**Returns:** `Result<PwmState, str>`

*Defined at line 255*

---

## PwmFakeChip

```fides
pub struct PwmFakeChip {
```

**Fields:**

| Name | Type |
|------|------|
| `exported` | `Vec<bool>` |
| `states` | `Vec<PwmState>` |
| `writes` | `i64` |

*Defined at line 295*

---

## pwm_fake_chip

```fides
pub fn pwm_fake_chip(nch: i64) -> PwmFakeChip {
```

**Parameters:**

- `nch`: `i64`

**Returns:** `PwmFakeChip`

*Defined at line 301*

---

## pwm_fake_write

Apply one sysfs write to the fake chip ("<file>" is the last path
part, ch the channel). Returns "" or the errno-like reason the kernel
would refuse it with.

```fides
pub fn pwm_fake_write(c: &mut PwmFakeChip, ch: i64, file: str, value: str) -> str {
```

**Parameters:**

- `c`: `&mut PwmFakeChip`
- `ch`: `i64`
- `file`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 318*

---

## pwm_fake_apply

Run a plan against the fake chip; the first refusal or "".

```fides
pub fn pwm_fake_apply(c: &mut PwmFakeChip, ch: i64, writes: &Vec<PwmWrite>) -> str {
```

**Parameters:**

- `c`: `&mut PwmFakeChip`
- `ch`: `i64`
- `writes`: `&Vec<PwmWrite>`

**Returns:** `str`

*Defined at line 371*

---

## Servo

```fides
pub struct Servo {
```

**Fields:**

| Name | Type |
|------|------|
| `min_ns` | `i64` |
| `max_ns` | `i64` |
| `travel_deg` | `f64` |
| `period_ns` | `i64` |

*Defined at line 388*

---

## servo_standard

A hobby servo: 1.0-2.0 ms over 180 degrees at 50 Hz.

```fides
pub fn servo_standard() -> Servo {
```

**Returns:** `Servo`

*Defined at line 396*

---

## servo_new

```fides
pub fn servo_new(min_us: i64, max_us: i64, travel_deg: f64, hz: i64) -> Servo {
```

**Parameters:**

- `min_us`: `i64`
- `max_us`: `i64`
- `travel_deg`: `f64`
- `hz`: `i64`

**Returns:** `Servo`

*Defined at line 402*

---

## servo_pulse_ns

Pulse width for an angle (clamped to the travel), rounded to the ns.

```fides
pub fn servo_pulse_ns(s: &Servo, angle_deg: f64) -> i64 {
```

**Parameters:**

- `s`: `&Servo`
- `angle_deg`: `f64`

**Returns:** `i64`

*Defined at line 409*

---

## servo_angle

```fides
pub fn servo_angle(s: &Servo, pulse_ns: i64) -> f64 {
```

**Parameters:**

- `s`: `&Servo`
- `pulse_ns`: `i64`

**Returns:** `f64`

*Defined at line 421*

---

## servo_state

```fides
pub fn servo_state(s: &Servo, angle_deg: f64) -> PwmState {
```

**Parameters:**

- `s`: `&Servo`
- `angle_deg`: `f64`

**Returns:** `PwmState`

*Defined at line 436*

---

## servo_speed_pulse_ns

Continuous-rotation servo: speed -1.0 (full reverse) .. 1.0 (full
forward) around the middle pulse (stop).

```fides
pub fn servo_speed_pulse_ns(s: &Servo, speed: f64) -> i64 {
```

**Parameters:**

- `s`: `&Servo`
- `speed`: `f64`

**Returns:** `i64`

*Defined at line 442*

---

## PwmTimer

Register values of a timer: f = clk / ((psc + 1) * (arr + 1)),
duty = ccr / (arr + 1).

```fides
pub struct PwmTimer {
```

**Fields:**

| Name | Type |
|------|------|
| `psc` | `i64` |
| `arr` | `i64` |
| `ccr` | `i64` |
| `actual_hz` | `f64` |
| `resolution_bits` | `i64` |

*Defined at line 463*

---

## pwm_timer_setup

The smallest prescaler that lets ARR fit in max_count (65535 for a
16-bit timer): the most duty resolution. psc is limited to 0..65535;
psc = -1 when the frequency cannot be reached.

```fides
pub fn pwm_timer_setup(clk_hz: i64, hz: i64, duty_permille: i64, max_count: i64) -> PwmTimer {
```

**Parameters:**

- `clk_hz`: `i64`
- `hz`: `i64`
- `duty_permille`: `i64`
- `max_count`: `i64`

**Returns:** `PwmTimer`

*Defined at line 474*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
