[All modules](index.md)

# ecu -- Fides Documentation

## Contents

- [ECU_STATE_INIT:](#ECU_STATE_INIT:) (const)
- [ECU_STATE_RUN:](#ECU_STATE_RUN:) (const)
- [ECU_STATE_SHUTDOWN:](#ECU_STATE_SHUTDOWN:) (const)
- [ECU_STATE_FAULT:](#ECU_STATE_FAULT:) (const)
- [ECU_STATE_LIMP_HOME:](#ECU_STATE_LIMP_HOME:) (const)
- [ecu_state_name](#ecu_state_name) (function)
- [SensorReading](#SensorReading) (struct)
- [sensor_ok](#sensor_ok) (function)
- [sensor_fault](#sensor_fault) (function)
- [sensor_validate](#sensor_validate) (function)
- [sensor_raw_to_value](#sensor_raw_to_value) (function)
- [ActuatorCommand](#ActuatorCommand) (struct)
- [actuator_command](#actuator_command) (function)
- [actuator_rate_limit](#actuator_rate_limit) (function)
- [PIDController](#PIDController) (struct)
- [pid_new](#pid_new) (function)
- [pid_set_limits](#pid_set_limits) (function)
- [pid_set_integral_limits](#pid_set_integral_limits) (function)
- [pid_compute](#pid_compute) (function)
- [pid_reset](#pid_reset) (function)
- [CalParam](#CalParam) (struct)
- [cal_param_new](#cal_param_new) (function)
- [cal_param_set](#cal_param_set) (function)
- [cal_param_get](#cal_param_get) (function)
- [cal_param_is_valid](#cal_param_is_valid) (function)
- [Watchdog](#Watchdog) (struct)
- [watchdog_new](#watchdog_new) (function)
- [watchdog_kick](#watchdog_kick) (function)
- [watchdog_check](#watchdog_check) (function)
- [watchdog_is_expired](#watchdog_is_expired) (function)

## ECU_STATE_INIT:

```fides
pub const ECU_STATE_INIT: i64 = 0;
```

*Defined at line 23*

---

## ECU_STATE_RUN:

```fides
pub const ECU_STATE_RUN: i64 = 1;
```

*Defined at line 24*

---

## ECU_STATE_SHUTDOWN:

```fides
pub const ECU_STATE_SHUTDOWN: i64 = 2;
```

*Defined at line 25*

---

## ECU_STATE_FAULT:

```fides
pub const ECU_STATE_FAULT: i64 = 3;
```

*Defined at line 26*

---

## ECU_STATE_LIMP_HOME:

```fides
pub const ECU_STATE_LIMP_HOME: i64 = 4;
```

*Defined at line 27*

---

## ecu_state_name

Get ECU state name.

```fides
pub fn ecu_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 29*

---

## SensorReading

A sensor reading with validity and timestamp.

```fides
pub struct SensorReading {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |
| `raw_value` | `i64` |
| `is_valid` | `bool` |
| `timestamp` | `i64` |
| `error_code` | `i64` |

*Defined at line 51*

---

## sensor_ok

Create a valid sensor reading.

```fides
pub fn sensor_ok(value: f64, raw: i64, timestamp: i64) -> SensorReading {
```

**Parameters:**

- `value`: `f64`
- `raw`: `i64`
- `timestamp`: `i64`

**Returns:** `SensorReading`

*Defined at line 60*

---

## sensor_fault

Create an invalid sensor reading (sensor fault).

```fides
pub fn sensor_fault(error_code: i64, timestamp: i64) -> SensorReading {
```

**Parameters:**

- `error_code`: `i64`
- `timestamp`: `i64`

**Returns:** `SensorReading`

*Defined at line 67*

---

## sensor_validate

Validate a sensor reading against min/max range.

```fides
pub fn sensor_validate(reading: SensorReading, min_val: f64, max_val: f64) -> SensorReading {
```

**Parameters:**

- `reading`: `SensorReading`
- `min_val`: `f64`
- `max_val`: `f64`

**Returns:** `SensorReading`

*Defined at line 74*

---

## sensor_raw_to_value

Convert raw ADC value to engineering value (linear conversion).

```fides
pub fn sensor_raw_to_value(raw: i64, raw_min: i64, raw_max: i64, eng_min: f64, eng_max: f64) -> f64 {
```

**Parameters:**

- `raw`: `i64`
- `raw_min`: `i64`
- `raw_max`: `i64`
- `eng_min`: `f64`
- `eng_max`: `f64`

**Returns:** `f64`

*Defined at line 87*

---

## ActuatorCommand

An actuator command with safety limits.

```fides
pub struct ActuatorCommand {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `f64` |
| `clamped` | `bool` |
| `limited` | `bool` |

*Defined at line 99*

---

## actuator_command

Create an actuator command with safety clamping.

```fides
pub fn actuator_command(value: f64, min_val: f64, max_val: f64) -> ActuatorCommand {
```

**Parameters:**

- `value`: `f64`
- `min_val`: `f64`
- `max_val`: `f64`

**Returns:** `ActuatorCommand`

*Defined at line 106*

---

## actuator_rate_limit

Apply a rate limit to an actuator command.
Limits how fast the command can change per time step.

```fides
pub fn actuator_rate_limit(target: f64, current: f64, max_rate: f64) -> f64 {
```

**Parameters:**

- `target`: `f64`
- `current`: `f64`
- `max_rate`: `f64`

**Returns:** `f64`

*Defined at line 124*

---

## PIDController

A PID controller state.

```fides
pub struct PIDController {
```

**Fields:**

| Name | Type |
|------|------|
| `kp` | `f64` |
| `ki` | `f64` |
| `kd` | `f64` |
| `integral` | `f64` |
| `prev_error` | `f64` |
| `output_min` | `f64` |
| `output_max` | `f64` |
| `integral_min` | `f64` |
| `integral_max` | `f64` |

*Defined at line 141*

---

## pid_new

Create a new PID controller.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = pid_new(2.0, 0.0, 0.0);
assert(pid_compute(&mut p, 10.0, 4.0, 1.0) == 12.0);
pid_set_limits(&mut p, -5.0, 5.0);
assert(pid_compute(&mut p, 10.0, 4.0, 1.0) == 5.0);
assert(sensor_raw_to_value(512, 0, 1024, 0.0, 100.0) == 50.0);
assert(sensor_ok(1.5, 7, 0).is_valid);
assert(!sensor_fault(3, 0).is_valid);
```

```fides
pub fn pid_new(kp: f64, ki: f64, kd: f64) -> PIDController {
```

**Parameters:**

- `kp`: `f64`
- `ki`: `f64`
- `kd`: `f64`

**Returns:** `PIDController`

*Defined at line 163*

---

## pid_set_limits

Set output limits.

```fides
pub fn pid_set_limits(pid: &mut PIDController, out_min: f64, out_max: f64) -> void {
```

**Parameters:**

- `pid`: `&mut PIDController`
- `out_min`: `f64`
- `out_max`: `f64`

**Returns:** `void`

*Defined at line 170*

---

## pid_set_integral_limits

Set integral anti-windup limits.

```fides
pub fn pid_set_integral_limits(pid: &mut PIDController, int_min: f64, int_max: f64) -> void {
```

**Parameters:**

- `pid`: `&mut PIDController`
- `int_min`: `f64`
- `int_max`: `f64`

**Returns:** `void`

*Defined at line 176*

---

## pid_compute

Compute PID output for one time step.
setpoint: desired value, measured: current value, dt: time step in seconds

```fides
pub fn pid_compute(pid: &mut PIDController, setpoint: f64, measured: f64, dt: f64) -> f64 {
```

**Parameters:**

- `pid`: `&mut PIDController`
- `setpoint`: `f64`
- `measured`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 184*

---

## pid_reset

Reset PID controller state.

```fides
pub fn pid_reset(pid: &mut PIDController) -> void {
```

**Parameters:**

- `pid`: `&mut PIDController`

**Returns:** `void`

*Defined at line 213*

---

## CalParam

A calibration parameter.

```fides
pub struct CalParam {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `f64` |
| `min_val` | `f64` |
| `max_val` | `f64` |
| `unit` | `str` |

*Defined at line 224*

---

## cal_param_new

Create a calibration parameter.

```fides
pub fn cal_param_new(name: str, value: f64, min_val: f64, max_val: f64, unit: str) -> CalParam {
```

**Parameters:**

- `name`: `str`
- `value`: `f64`
- `min_val`: `f64`
- `max_val`: `f64`
- `unit`: `str`

**Returns:** `CalParam`

*Defined at line 233*

---

## cal_param_set

Set a calibration parameter value (clamped to range).

```fides
pub fn cal_param_set(param: &mut CalParam, value: f64) -> void {
```

**Parameters:**

- `param`: `&mut CalParam`
- `value`: `f64`

**Returns:** `void`

*Defined at line 240*

---

## cal_param_get

Get a calibration parameter value.

```fides
pub fn cal_param_get(param: CalParam) -> f64 {
```

**Parameters:**

- `param`: `CalParam`

**Returns:** `f64`

*Defined at line 252*

---

## cal_param_is_valid

Validate a calibration parameter is within range.

```fides
pub fn cal_param_is_valid(param: CalParam) -> bool {
```

**Parameters:**

- `param`: `CalParam`

**Returns:** `bool`

*Defined at line 257*

---

## Watchdog

A watchdog timer.

```fides
pub struct Watchdog {
```

**Fields:**

| Name | Type |
|------|------|
| `timeout_ms` | `i64` |
| `last_kick` | `i64` |
| `expired` | `bool` |

*Defined at line 267*

---

## watchdog_new

Create a new watchdog timer.

```fides
pub fn watchdog_new(timeout_ms: i64) -> Watchdog {
```

**Parameters:**

- `timeout_ms`: `i64`

**Returns:** `Watchdog`

*Defined at line 274*

---

## watchdog_kick

Kick the watchdog (reset the timer).

```fides
pub fn watchdog_kick(wd: &mut Watchdog, current_time: i64) -> void {
```

**Parameters:**

- `wd`: `&mut Watchdog`
- `current_time`: `i64`

**Returns:** `void`

*Defined at line 281*

---

## watchdog_check

Check if the watchdog has expired.

```fides
pub fn watchdog_check(wd: &mut Watchdog, current_time: i64) -> bool {
```

**Parameters:**

- `wd`: `&mut Watchdog`
- `current_time`: `i64`

**Returns:** `bool`

*Defined at line 287*

---

## watchdog_is_expired

Check if watchdog is expired without updating state.

```fides
pub fn watchdog_is_expired(wd: Watchdog) -> bool {
```

**Parameters:**

- `wd`: `Watchdog`

**Returns:** `bool`

*Defined at line 296*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
