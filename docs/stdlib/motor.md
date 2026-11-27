[All modules](index.md)

# motor -- Fides Documentation

## Contents

- [MOTOR_COAST:](#MOTOR_COAST:) (const)
- [MOTOR_FORWARD:](#MOTOR_FORWARD:) (const)
- [MOTOR_REVERSE:](#MOTOR_REVERSE:) (const)
- [MOTOR_BRAKE:](#MOTOR_BRAKE:) (const)
- [DcMotor](#DcMotor) (struct)
- [HBridge](#HBridge) (struct)
- [dcm_new](#dcm_new) (function)
- [dcm_set_deadband](#dcm_set_deadband) (function)
- [dcm_set_brake_at_zero](#dcm_set_brake_at_zero) (function)
- [dcm_command](#dcm_command) (function)
- [dcm_brake](#dcm_brake) (function)
- [motor_pwm_compare](#motor_pwm_compare) (function)
- [DcModel](#DcModel) (struct)
- [dcmodel_new](#dcmodel_new) (function)
- [dcmodel_step](#dcmodel_step) (function)
- [dcmodel_steady_speed](#dcmodel_steady_speed) (function)
- [STEP_WAVE:](#STEP_WAVE:) (const)
- [STEP_FULL:](#STEP_FULL:) (const)
- [STEP_HALF:](#STEP_HALF:) (const)
- [STEP_MICRO:](#STEP_MICRO:) (const)
- [StepCoils](#StepCoils) (struct)
- [stp_cycle_len](#stp_cycle_len) (function)
- [stp_coils](#stp_coils) (function)
- [stp_dac](#stp_dac) (function)
- [motor_round](#motor_round) (function)
- [stp_unipolar_half_bits](#stp_unipolar_half_bits) (function)
- [stp_plan_exact](#stp_plan_exact) (function)
- [StepGen](#StepGen) (struct)
- [stepgen_new](#stepgen_new) (function)
- [stepgen_move_to](#stepgen_move_to) (function)
- [stepgen_next](#stepgen_next) (function)
- [Servo](#Servo) (struct)
- [servo_new](#servo_new) (function)
- [servo_pulse_us](#servo_pulse_us) (function)
- [servo_angle](#servo_angle) (function)
- [servo_ticks](#servo_ticks) (function)
- [PhaseDrive](#PhaseDrive) (struct)
- [bldc_sector_from_hall](#bldc_sector_from_hall) (function)
- [bldc_drive](#bldc_drive) (function)
- [bldc_commutate](#bldc_commutate) (function)
- [bldc_drive_str](#bldc_drive_str) (function)
- [Ab](#Ab) (struct)
- [Abc](#Abc) (struct)
- [foc_clarke](#foc_clarke) (function)
- [foc_clarke3](#foc_clarke3) (function)
- [foc_inv_clarke](#foc_inv_clarke) (function)
- [foc_park](#foc_park) (function)
- [foc_inv_park](#foc_inv_park) (function)
- [foc_svpwm](#foc_svpwm) (function)
- [foc_sector](#foc_sector) (function)
- [qenc_delta](#qenc_delta) (function)
- [QuadEnc](#QuadEnc) (struct)
- [qenc_new](#qenc_new) (function)
- [qenc_update](#qenc_update) (function)
- [qenc_angle](#qenc_angle) (function)
- [qenc_speed](#qenc_speed) (function)
- [qenc_pattern](#qenc_pattern) (function)

## MOTOR_COAST:

```fides
pub const MOTOR_COAST: i64 = 0;
```

*Defined at line 45*

---

## MOTOR_FORWARD:

```fides
pub const MOTOR_FORWARD: i64 = 1;
```

*Defined at line 46*

---

## MOTOR_REVERSE:

```fides
pub const MOTOR_REVERSE: i64 = 2;
```

*Defined at line 47*

---

## MOTOR_BRAKE:

```fides
pub const MOTOR_BRAKE: i64 = 3;
```

*Defined at line 48*

---

## DcMotor

Settings and current-limit state of one DC motor channel.

```fides
pub struct DcMotor {
```

**Fields:**

| Name | Type |
|------|------|
| `max_duty` | `f64` |
| `deadband` | `f64` |
| `current_limit` | `f64` |
| `recover` | `f64` |
| `scale` | `f64` |
| `brake_at_zero` | `bool` |
| `limited` | `bool` |

*Defined at line 51*

---

## HBridge

What to put on the bridge: the logic level of each input while the PWM
is on, the PWM duty in [0, 1] and the mode (MOTOR_*).

```fides
pub struct HBridge {
```

**Fields:**

| Name | Type |
|------|------|
| `in1` | `bool` |
| `in2` | `bool` |
| `duty` | `f64` |
| `mode` | `i64` |

*Defined at line 66*

---

## dcm_new

A motor with a maximum duty and a current limit (amps; <= 0: none).

```fides
pub fn dcm_new(max_duty: f64, current_limit: f64) -> DcMotor {
```

**Parameters:**

- `max_duty`: `f64`
- `current_limit`: `f64`

**Returns:** `DcMotor`

*Defined at line 74*

---

## dcm_set_deadband

Commands with |cmd| <= db give zero output.

```fides
pub fn dcm_set_deadband(m: &mut DcMotor, db: f64) {
```

**Parameters:**

- `m`: `&mut DcMotor`
- `db`: `f64`

*Defined at line 81*

---

## dcm_set_brake_at_zero

Brake (true) or coast (false) when the command is zero.

```fides
pub fn dcm_set_brake_at_zero(m: &mut DcMotor, on: bool) {
```

**Parameters:**

- `m`: `&mut DcMotor`
- `on`: `bool`

*Defined at line 86*

---

## dcm_command

Bridge levels for a command in [-1, 1] given the measured current.
Over the limit the duty is scaled by limit / current (cumulatively);
under it the scale recovers by `recover` per call up to 1.

```fides
pub fn dcm_command(m: &mut DcMotor, cmd: f64, current: f64) -> HBridge {
```

**Parameters:**

- `m`: `&mut DcMotor`
- `cmd`: `f64`
- `current`: `f64`

**Returns:** `HBridge`

*Defined at line 93*

---

## dcm_brake

An explicit brake (both inputs high, full duty).

```fides
pub fn dcm_brake() -> HBridge {
```

**Returns:** `HBridge`

*Defined at line 125*

---

## motor_pwm_compare

The timer compare value for a duty, rounded, in [0, period].

```fides
pub fn motor_pwm_compare(duty: f64, period: i64) -> i64 {
```

**Parameters:**

- `duty`: `f64`
- `period`: `i64`

**Returns:** `i64`

*Defined at line 132*

---

## DcModel

A brushed DC motor: V = R i + L di/dt + Ke w; J dw/dt = Kt i - b w - load.

```fides
pub struct DcModel {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `f64` |
| `l` | `f64` |
| `ke` | `f64` |
| `kt` | `f64` |
| `j` | `f64` |
| `b` | `f64` |
| `i` | `f64` |
| `w` | `f64` |
| `theta` | `f64` |

*Defined at line 138*

---

## dcmodel_new

```fides
pub fn dcmodel_new(r: f64, l: f64, ke: f64, kt: f64, j: f64, b: f64) -> DcModel {
```

**Parameters:**

- `r`: `f64`
- `l`: `f64`
- `ke`: `f64`
- `kt`: `f64`
- `j`: `f64`
- `b`: `f64`

**Returns:** `DcModel`

*Defined at line 150*

---

## dcmodel_step

Advance the model by dt with voltage v and load torque, in `sub` equal
semi-implicit Euler substeps.

```fides
pub fn dcmodel_step(m: &mut DcModel, v: f64, load: f64, dt: f64, sub: i64) {
```

**Parameters:**

- `m`: `&mut DcModel`
- `v`: `f64`
- `load`: `f64`
- `dt`: `f64`
- `sub`: `i64`

*Defined at line 158*

---

## dcmodel_steady_speed

The steady-state speed at voltage v with no load.

```fides
pub fn dcmodel_steady_speed(m: &DcModel, v: f64) -> f64 {
```

**Parameters:**

- `m`: `&DcModel`
- `v`: `f64`

**Returns:** `f64`

*Defined at line 174*

---

## STEP_WAVE:

```fides
pub const STEP_WAVE: i64 = 0;
```

*Defined at line 180*

---

## STEP_FULL:

```fides
pub const STEP_FULL: i64 = 1;
```

*Defined at line 181*

---

## STEP_HALF:

```fides
pub const STEP_HALF: i64 = 2;
```

*Defined at line 182*

---

## STEP_MICRO:

```fides
pub const STEP_MICRO: i64 = 3;
```

*Defined at line 183*

---

## StepCoils

Coil currents of a two-phase stepper, each in [-1, 1].

```fides
pub struct StepCoils {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `f64` |
| `b` | `f64` |

*Defined at line 186*

---

## stp_cycle_len

Positions per electrical cycle (4 full steps) in a mode.

```fides
pub fn stp_cycle_len(mode: i64, micro: i64) -> i64 {
```

**Parameters:**

- `mode`: `i64`
- `micro`: `i64`

**Returns:** `i64`

*Defined at line 192*

---

## stp_coils

The coil currents at position `pos` (any integer, taken modulo the
cycle) in a mode; `micro` microsteps per full step for STEP_MICRO.
Wave and micro start with phase A alone; full drives both phases.

```fides
pub fn stp_coils(mode: i64, pos: i64, micro: i64) -> StepCoils {
```

**Parameters:**

- `mode`: `i64`
- `pos`: `i64`
- `micro`: `i64`

**Returns:** `StepCoils`

*Defined at line 212*

---

## stp_dac

DAC values (-imax..imax, rounded) of the coil currents.

```fides
pub fn stp_dac(c: StepCoils, imax: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `StepCoils`
- `imax`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 283*

---

## motor_round

Round half away from zero.

```fides
pub fn motor_round(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 292*

---

## stp_unipolar_half_bits

The 4 coil bits (bit 0 = IN1 .. bit 3 = IN4) of a unipolar motor
(28BYJ-48 on a ULN2003) in half-step mode at position pos.

```fides
pub fn stp_unipolar_half_bits(pos: i64) -> i64 {
```

**Parameters:**

- `pos`: `i64`

**Returns:** `i64`

*Defined at line 301*

---

## stp_plan_exact

Exact step delays (seconds) of a move of n steps that accelerates at
`accel` steps/s^2 up to `vmax` steps/s, cruises, and decelerates at the
same rate (a triangle when vmax is not reached). Delay k is the time
between step k-1 and step k (step 0 at t = 0 is the start).

```fides
pub fn stp_plan_exact(n: i64, accel: f64, vmax: f64) -> Vec<f64> {
```

**Parameters:**

- `n`: `i64`
- `accel`: `f64`
- `vmax`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 334*

---

## StepGen

The real-time step generator (Austin's recurrence, AccelStepper's
algorithm). Position and target are in steps; call `stepgen_next`
after each step to get the delay before the next one.

```fides
pub struct StepGen {
```

**Fields:**

| Name | Type |
|------|------|
| `accel` | `f64` |
| `vmax` | `f64` |
| `pos` | `i64` |
| `target` | `i64` |
| `n` | `i64` |
| `c0` | `f64` |
| `cn` | `f64` |
| `cmin` | `f64` |
| `speed` | `f64` |
| `dir` | `i64` |

*Defined at line 370*

---

## stepgen_new

```fides
pub fn stepgen_new(accel: f64, vmax: f64) -> StepGen {
```

**Parameters:**

- `accel`: `f64`
- `vmax`: `f64`

**Returns:** `StepGen`

*Defined at line 386*

---

## stepgen_move_to

```fides
pub fn stepgen_move_to(g: &mut StepGen, target: i64) {
```

**Parameters:**

- `g`: `&mut StepGen`
- `target`: `i64`

*Defined at line 392*

---

## stepgen_next

Compute the delay before the next step and take it: returns the delay
in seconds, or 0.0 when the motor is at its target and stopped.

```fides
pub fn stepgen_next(g: &mut StepGen) -> f64 {
```

**Parameters:**

- `g`: `&mut StepGen`

**Returns:** `f64`

*Defined at line 398*

---

## Servo

Pulse range for an angle range (standard: 1000..2000 us for 0..180 deg).

```fides
pub struct Servo {
```

**Fields:**

| Name | Type |
|------|------|
| `min_us` | `f64` |
| `max_us` | `f64` |
| `min_deg` | `f64` |
| `max_deg` | `f64` |

*Defined at line 449*

---

## servo_new

```fides
pub fn servo_new(min_us: f64, max_us: f64, min_deg: f64, max_deg: f64) -> Servo {
```

**Parameters:**

- `min_us`: `f64`
- `max_us`: `f64`
- `min_deg`: `f64`
- `max_deg`: `f64`

**Returns:** `Servo`

*Defined at line 456*

---

## servo_pulse_us

The pulse width (us) for an angle, clamped to the range.

```fides
pub fn servo_pulse_us(s: &Servo, deg: f64) -> f64 {
```

**Parameters:**

- `s`: `&Servo`
- `deg`: `f64`

**Returns:** `f64`

*Defined at line 463*

---

## servo_angle

The angle a pulse width commands, clamped to the range.

```fides
pub fn servo_angle(s: &Servo, us: f64) -> f64 {
```

**Parameters:**

- `s`: `&Servo`
- `us`: `f64`

**Returns:** `f64`

*Defined at line 469*

---

## servo_ticks

Timer ticks of a pulse for a timer counting at `timer_hz`.

```fides
pub fn servo_ticks(us: f64, timer_hz: f64) -> i64 {
```

**Parameters:**

- `us`: `f64`
- `timer_hz`: `f64`

**Returns:** `i64`

*Defined at line 475*

---

## PhaseDrive

Which way each phase is driven: 1 high side on, -1 low side on, 0 floating.

```fides
pub struct PhaseDrive {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `i64` |
| `b` | `i64` |
| `c` | `i64` |

*Defined at line 482*

---

## bldc_sector_from_hall

The sector (0..5) of a Hall code (bit 0 = Hall A, bit 1 = B, bit 2 = C)
for sensors 120 degrees apart; -1 for the illegal codes 0 and 7.
Forward rotation sees the codes 1, 3, 2, 6, 4, 5 (sectors 0..5).

```fides
pub fn bldc_sector_from_hall(hall: i64) -> i64 {
```

**Parameters:**

- `hall`: `i64`

**Returns:** `i64`

*Defined at line 491*

---

## bldc_drive

The drive of sector s (0..5): A+B-, A+C-, B+C-, B+A-, C+A-, C+B-.
Reverse uses the opposite currents (the sector three steps on).
An illegal sector (< 0) floats every phase.

```fides
pub fn bldc_drive(sector: i64, forward: bool) -> PhaseDrive {
```

**Parameters:**

- `sector`: `i64`
- `forward`: `bool`

**Returns:** `PhaseDrive`

*Defined at line 516*

---

## bldc_commutate

The drive for a Hall code.

```fides
pub fn bldc_commutate(hall: i64, forward: bool) -> PhaseDrive {
```

**Parameters:**

- `hall`: `i64`
- `forward`: `bool`

**Returns:** `PhaseDrive`

*Defined at line 557*

---

## bldc_drive_str

"A+B-" style text of a drive ("off" when all float).

```fides
pub fn bldc_drive_str(d: PhaseDrive) -> str {
```

**Parameters:**

- `d`: `PhaseDrive`

**Returns:** `str`

*Defined at line 562*

---

## Ab

A two-axis value (alpha/beta or d/q).

```fides
pub struct Ab {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |

*Defined at line 592*

---

## Abc

Three phase values.

```fides
pub struct Abc {
```

**Fields:**

| Name | Type |
|------|------|
| `a` | `f64` |
| `b` | `f64` |
| `c` | `f64` |

*Defined at line 598*

---

## foc_clarke

Clarke transform, amplitude invariant, from two phase currents
(a + b + c = 0): alpha = a, beta = (a + 2 b) / sqrt 3.

```fides
pub fn foc_clarke(a: f64, b: f64) -> Ab {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `Ab`

*Defined at line 606*

---

## foc_clarke3

Clarke transform from all three phases: alpha = (2a - b - c) / 3,
beta = (b - c) / sqrt 3.

```fides
pub fn foc_clarke3(v: Abc) -> Ab {
```

**Parameters:**

- `v`: `Abc`

**Returns:** `Ab`

*Defined at line 614*

---

## foc_inv_clarke

Inverse Clarke transform.

```fides
pub fn foc_inv_clarke(ab: Ab) -> Abc {
```

**Parameters:**

- `ab`: `Ab`

**Returns:** `Abc`

*Defined at line 621*

---

## foc_park

Park transform at electrical angle theta: alpha/beta -> d/q.

```fides
pub fn foc_park(ab: Ab, theta: f64) -> Ab {
```

**Parameters:**

- `ab`: `Ab`
- `theta`: `f64`

**Returns:** `Ab`

*Defined at line 629*

---

## foc_inv_park

Inverse Park transform: d/q -> alpha/beta.

```fides
pub fn foc_inv_park(dq: Ab, theta: f64) -> Ab {
```

**Parameters:**

- `dq`: `Ab`
- `theta`: `f64`

**Returns:** `Ab`

*Defined at line 638*

---

## foc_svpwm

Space-vector PWM: the phase duties (0..1, centre-aligned) for a voltage
vector (alpha, beta) on a bus of vdc. A vector longer than vdc / sqrt 3
(the linear range) is scaled down onto that circle.

```fides
pub fn foc_svpwm(vab: Ab, vdc: f64) -> Abc {
```

**Parameters:**

- `vab`: `Ab`
- `vdc`: `f64`

**Returns:** `Abc`

*Defined at line 649*

---

## foc_sector

The SVPWM sector (1..6, 60 degrees each, sector 1 from 0 to 60 degrees).

```fides
pub fn foc_sector(vab: Ab) -> i64 {
```

**Parameters:**

- `vab`: `Ab`

**Returns:** `i64`

*Defined at line 670*

---

## qenc_delta

The count change of a transition prev -> cur of the 2-bit state
(A << 1) | B; forward is 00 -> 01 -> 11 -> 10. 2 marks an illegal
transition (both lines changed).

```fides
pub fn qenc_delta(prev: i64, cur: i64) -> i64 {
```

**Parameters:**

- `prev`: `i64`
- `cur`: `i64`

**Returns:** `i64`

*Defined at line 687*

---

## QuadEnc

A x4 quadrature decoder.

```fides
pub struct QuadEnc {
```

**Fields:**

| Name | Type |
|------|------|
| `cpr` | `i64` |
| `state` | `i64` |
| `count` | `i64` |
| `errors` | `i64` |
| `last_count` | `i64` |
| `speed` | `f64` |

*Defined at line 703*

---

## qenc_new

A decoder whose lines start at levels a, b.

```fides
pub fn qenc_new(cpr: i64, a: bool, b: bool) -> QuadEnc {
```

**Parameters:**

- `cpr`: `i64`
- `a`: `bool`
- `b`: `bool`

**Returns:** `QuadEnc`

*Defined at line 714*

---

## qenc_update

Feed the current A/B levels; returns the count change (0, 1, -1; an
illegal transition counts an error and changes nothing).

```fides
pub fn qenc_update(e: &mut QuadEnc, a: bool, b: bool) -> i64 {
```

**Parameters:**

- `e`: `&mut QuadEnc`
- `a`: `bool`
- `b`: `bool`

**Returns:** `i64`

*Defined at line 733*

---

## qenc_angle

The shaft angle in radians (count / (4 cpr) turns).

```fides
pub fn qenc_angle(e: &QuadEnc) -> f64 {
```

**Parameters:**

- `e`: `&QuadEnc`

**Returns:** `f64`

*Defined at line 746*

---

## qenc_speed

The speed (rad/s) from the counts since the previous call, dt seconds ago.

```fides
pub fn qenc_speed(e: &mut QuadEnc, dt: f64) -> f64 {
```

**Parameters:**

- `e`: `&mut QuadEnc`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 751*

---

## qenc_pattern

The A/B levels at quadrature position k (forward sequence 00 01 11 10),
to drive a decoder in tests or simulations: bit 1 = A, bit 0 = B.

```fides
pub fn qenc_pattern(k: i64) -> i64 {
```

**Parameters:**

- `k`: `i64`

**Returns:** `i64`

*Defined at line 760*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
