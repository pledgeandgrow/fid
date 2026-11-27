[All modules](index.md)

# grid -- Fides Documentation

## Contents

- [LN_XCBR:](#LN_XCBR:) (const)
- [LN_XSWI:](#LN_XSWI:) (const)
- [LN_MMXU:](#LN_MMXU:) (const)
- [LN_PTUV:](#LN_PTUV:) (const)
- [LN_PTOC:](#LN_PTOC:) (const)
- [LN_PDIS:](#LN_PDIS:) (const)
- [LN_RREC:](#LN_RREC:) (const)
- [LN_RBRF:](#LN_RBRF:) (const)
- [LN_CSWI:](#LN_CSWI:) (const)
- [LN_CILO:](#LN_CILO:) (const)
- [LN_CSWI2:](#LN_CSWI2:) (const)
- [iec61850_ln_name](#iec61850_ln_name) (function)
- [CDC_SPS:](#CDC_SPS:) (const)
- [CDC_DPS:](#CDC_DPS:) (const)
- [CDC_MV:](#CDC_MV:) (const)
- [CDC_ASG:](#CDC_ASG:) (const)
- [CDC_ING:](#CDC_ING:) (const)
- [CDC_ENG:](#CDC_ENG:) (const)
- [CDC_SPC:](#CDC_SPC:) (const)
- [CDC_DPC:](#CDC_DPC:) (const)
- [CDC_INC:](#CDC_INC:) (const)
- [iec61850_cdc_name](#iec61850_cdc_name) (function)
- [XCBR](#XCBR) (struct)
- [iec61850_xcbr_new](#iec61850_xcbr_new) (function)
- [iec61850_xcbr_set_pos](#iec61850_xcbr_set_pos) (function)
- [iec61850_xcbr_pos_name](#iec61850_xcbr_pos_name) (function)
- [iec61850_xcbr_is_closed](#iec61850_xcbr_is_closed) (function)
- [iec61850_xcbr_is_open](#iec61850_xcbr_is_open) (function)
- [iec61850_xcbr_set_health](#iec61850_xcbr_set_health) (function)
- [iec61850_xcbr_health_name](#iec61850_xcbr_health_name) (function)
- [MMXU](#MMXU) (struct)
- [iec61850_mmxu_new](#iec61850_mmxu_new) (function)
- [iec61850_mmxu_set_voltage](#iec61850_mmxu_set_voltage) (function)
- [iec61850_mmxu_set_current](#iec61850_mmxu_set_current) (function)
- [iec61850_mmxu_set_power](#iec61850_mmxu_set_power) (function)
- [iec61850_mmxu_apparent_power](#iec61850_mmxu_apparent_power) (function)
- [iec61850_mmxu_total_power](#iec61850_mmxu_total_power) (function)
- [GOOSEMessage](#GOOSEMessage) (struct)
- [iec61850_goose_new](#iec61850_goose_new) (function)
- [iec61850_goose_add_data](#iec61850_goose_add_data) (function)
- [iec61850_goose_update_data](#iec61850_goose_update_data) (function)
- [iec61850_goose_get_data](#iec61850_goose_get_data) (function)
- [iec61850_goose_data_count](#iec61850_goose_data_count) (function)
- [iec61850_goose_serialize](#iec61850_goose_serialize) (function)
- [iec61850_ref](#iec61850_ref) (function)
- [iec61850_dataset_ref](#iec61850_dataset_ref) (function)
- [iec61850_control_ref](#iec61850_control_ref) (function)
- [PTOC](#PTOC) (struct)
- [iec61850_ptoc_new](#iec61850_ptoc_new) (function)
- [iec61850_ptoc_check](#iec61850_ptoc_check) (function)
- [iec61850_ptoc_set_inverse](#iec61850_ptoc_set_inverse) (function)
- [iec61850_ptoc_trip_time](#iec61850_ptoc_trip_time) (function)

## LN_XCBR:

```fides
pub const LN_XCBR: i64 = 0;  // Circuit Breaker
```

*Defined at line 25*

---

## LN_XSWI:

```fides
pub const LN_XSWI: i64 = 1;  // Switch / Disconnector
```

*Defined at line 26*

---

## LN_MMXU:

```fides
pub const LN_MMXU: i64 = 2;  // Measurement (voltage, current, power)
```

*Defined at line 27*

---

## LN_PTUV:

```fides
pub const LN_PTUV: i64 = 3;  // Voltage Time Protection
```

*Defined at line 28*

---

## LN_PTOC:

```fides
pub const LN_PTOC: i64 = 4;  // Overcurrent Protection
```

*Defined at line 29*

---

## LN_PDIS:

```fides
pub const LN_PDIS: i64 = 5;  // Distance Protection
```

*Defined at line 30*

---

## LN_RREC:

```fides
pub const LN_RREC: i64 = 6;  // Disturbance Recorder
```

*Defined at line 31*

---

## LN_RBRF:

```fides
pub const LN_RBRF: i64 = 7;  // Breaker Failure
```

*Defined at line 32*

---

## LN_CSWI:

```fides
pub const LN_CSWI: i64 = 8;  // Control Switch
```

*Defined at line 33*

---

## LN_CILO:

```fides
pub const LN_CILO: i64 = 9;  // Interlocking
```

*Defined at line 34*

---

## LN_CSWI2:

```fides
pub const LN_CSWI2: i64 = 10;  // Control Switch (alternate)
```

*Defined at line 35*

---

## iec61850_ln_name

Get logical node name.

```fides
pub fn iec61850_ln_name(ln_type: i64) -> str {
```

**Parameters:**

- `ln_type`: `i64`

**Returns:** `str`

*Defined at line 38*

---

## CDC_SPS:

```fides
pub const CDC_SPS: i64 = 0;  // Single Point Status
```

*Defined at line 77*

---

## CDC_DPS:

```fides
pub const CDC_DPS: i64 = 1;  // Double Point Status
```

*Defined at line 78*

---

## CDC_MV:

```fides
pub const CDC_MV: i64 = 2;  // Measured Value
```

*Defined at line 79*

---

## CDC_ASG:

```fides
pub const CDC_ASG: i64 = 3;  // Analog Setting
```

*Defined at line 80*

---

## CDC_ING:

```fides
pub const CDC_ING: i64 = 4;  // Integer Setting
```

*Defined at line 81*

---

## CDC_ENG:

```fides
pub const CDC_ENG: i64 = 5;  // Enumerated Setting
```

*Defined at line 82*

---

## CDC_SPC:

```fides
pub const CDC_SPC: i64 = 6;  // Single Point Controllable
```

*Defined at line 83*

---

## CDC_DPC:

```fides
pub const CDC_DPC: i64 = 7;  // Double Point Controllable
```

*Defined at line 84*

---

## CDC_INC:

```fides
pub const CDC_INC: i64 = 8;  // Integer Controllable
```

*Defined at line 85*

---

## iec61850_cdc_name

Get CDC name.

```fides
pub fn iec61850_cdc_name(cdc: i64) -> str {
```

**Parameters:**

- `cdc`: `i64`

**Returns:** `str`

*Defined at line 88*

---

## XCBR

A circuit breaker logical node.

```fides
pub struct XCBR {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ln_type` | `i64` |
| `position` | `i64` |
| `health` | `i64` |
| `capable_of_op` | `bool` |
| `op_cnt` | `i64` |
| `rated_current` | `i64,  // amperes` |

*Defined at line 124*

---

## iec61850_xcbr_new

Create a new circuit breaker.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(iec61850_ln_name(LN_XCBR), "XCBR");
var cb = iec61850_xcbr_new("CB1", 630);
assert_eq(iec61850_xcbr_pos_name(cb), "Open");
assert(iec61850_xcbr_is_open(cb));
iec61850_xcbr_set_pos(&mut cb, 1);
assert(iec61850_xcbr_is_closed(cb));
assert_eq(iec61850_xcbr_pos_name(cb), "Closed");
iec61850_xcbr_set_pos(&mut cb, 0);
assert(cb.op_cnt >= 1);
```

```fides
pub fn iec61850_xcbr_new(name: str, rated_current: i64) -> XCBR {
```

**Parameters:**

- `name`: `str`
- `rated_current`: `i64`

**Returns:** `XCBR`

*Defined at line 150*

---

## iec61850_xcbr_set_pos

Set breaker position (0=open, 1=closed).

```fides
pub fn iec61850_xcbr_set_pos(breaker: &mut XCBR, pos: i64) -> void {
```

**Parameters:**

- `breaker`: `&mut XCBR`
- `pos`: `i64`

**Returns:** `void`

*Defined at line 159*

---

## iec61850_xcbr_pos_name

Get breaker position name.

```fides
pub fn iec61850_xcbr_pos_name(breaker: XCBR) -> str {
```

**Parameters:**

- `breaker`: `XCBR`

**Returns:** `str`

*Defined at line 170*

---

## iec61850_xcbr_is_closed

Check if breaker is closed.

```fides
pub fn iec61850_xcbr_is_closed(breaker: XCBR) -> bool {
```

**Parameters:**

- `breaker`: `XCBR`

**Returns:** `bool`

*Defined at line 181*

---

## iec61850_xcbr_is_open

Check if breaker is open.

```fides
pub fn iec61850_xcbr_is_open(breaker: XCBR) -> bool {
```

**Parameters:**

- `breaker`: `XCBR`

**Returns:** `bool`

*Defined at line 186*

---

## iec61850_xcbr_set_health

Set breaker health.

```fides
pub fn iec61850_xcbr_set_health(breaker: &mut XCBR, health: i64) -> void {
```

**Parameters:**

- `breaker`: `&mut XCBR`
- `health`: `i64`

**Returns:** `void`

*Defined at line 191*

---

## iec61850_xcbr_health_name

Get breaker health name.

```fides
pub fn iec61850_xcbr_health_name(breaker: XCBR) -> str {
```

**Parameters:**

- `breaker`: `XCBR`

**Returns:** `str`

*Defined at line 199*

---

## MMXU

A measurement logical node.

```fides
pub struct MMXU {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ln_type` | `i64` |
| `voltage_a` | `f64` |
| `voltage_b` | `f64` |
| `voltage_c` | `f64` |
| `current_a` | `f64` |
| `current_b` | `f64` |
| `current_c` | `f64` |
| `active_power` | `f64` |
| `reactive_power` | `f64` |
| `power_factor` | `f64` |
| `frequency` | `f64,  // Hz` |

*Defined at line 214*

---

## iec61850_mmxu_new

Create a new measurement node.

```fides
pub fn iec61850_mmxu_new(name: str) -> MMXU {
```

**Parameters:**

- `name`: `str`

**Returns:** `MMXU`

*Defined at line 239*

---

## iec61850_mmxu_set_voltage

Set three-phase voltage measurements.

```fides
pub fn iec61850_mmxu_set_voltage(mmxu: &mut MMXU, va: f64, vb: f64, vc: f64) -> void {
```

**Parameters:**

- `mmxu`: `&mut MMXU`
- `va`: `f64`
- `vb`: `f64`
- `vc`: `f64`

**Returns:** `void`

*Defined at line 246*

---

## iec61850_mmxu_set_current

Set three-phase current measurements.

```fides
pub fn iec61850_mmxu_set_current(mmxu: &mut MMXU, ia: f64, ib: f64, ic: f64) -> void {
```

**Parameters:**

- `mmxu`: `&mut MMXU`
- `ia`: `f64`
- `ib`: `f64`
- `ic`: `f64`

**Returns:** `void`

*Defined at line 253*

---

## iec61850_mmxu_set_power

Set power measurements.

```fides
pub fn iec61850_mmxu_set_power(mmxu: &mut MMXU, active: f64, reactive: f64, pf: f64) -> void {
```

**Parameters:**

- `mmxu`: `&mut MMXU`
- `active`: `f64`
- `reactive`: `f64`
- `pf`: `f64`

**Returns:** `void`

*Defined at line 260*

---

## iec61850_mmxu_apparent_power

Calculate apparent power: sqrt(P² + Q²).

```fides
pub fn iec61850_mmxu_apparent_power(mmxu: MMXU) -> f64 {
```

**Parameters:**

- `mmxu`: `MMXU`

**Returns:** `f64`

*Defined at line 267*

---

## iec61850_mmxu_total_power

Calculate three-phase total active power: P = Va*Ia + Vb*Ib + Vc*Ic (approx).

```fides
pub fn iec61850_mmxu_total_power(mmxu: MMXU) -> f64 {
```

**Parameters:**

- `mmxu`: `MMXU`

**Returns:** `f64`

*Defined at line 272*

---

## GOOSEMessage

A GOOSE message.

```fides
pub struct GOOSEMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `app_id` | `i64` |
| `goose_ref` | `str` |
| `seq_num` | `i64` |
| `time` | `i64` |
| `st_num` | `i64` |
| `data` | `Vec<i64>,  // dataset values` |

*Defined at line 283*

---

## iec61850_goose_new

Create a new GOOSE message.

```fides
pub fn iec61850_goose_new(app_id: i64, ref: str) -> GOOSEMessage {
```

**Parameters:**

- `app_id`: `i64`
- `ref`: `str`

**Returns:** `GOOSEMessage`

*Defined at line 297*

---

## iec61850_goose_add_data

Add a data value to the GOOSE message.

```fides
pub fn iec61850_goose_add_data(msg: &mut GOOSEMessage, value: i64) -> void {
```

**Parameters:**

- `msg`: `&mut GOOSEMessage`
- `value`: `i64`

**Returns:** `void`

*Defined at line 304*

---

## iec61850_goose_update_data

Update a data value in the GOOSE message (increments seq_num).

```fides
pub fn iec61850_goose_update_data(msg: &mut GOOSEMessage, index: i64, value: i64) -> void {
```

**Parameters:**

- `msg`: `&mut GOOSEMessage`
- `index`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 311*

---

## iec61850_goose_get_data

Get a data value from the GOOSE message.

```fides
pub fn iec61850_goose_get_data(msg: GOOSEMessage, index: i64) -> i64 {
```

**Parameters:**

- `msg`: `GOOSEMessage`
- `index`: `i64`

**Returns:** `i64`

*Defined at line 320*

---

## iec61850_goose_data_count

Get the number of data values.

```fides
pub fn iec61850_goose_data_count(msg: GOOSEMessage) -> i64 {
```

**Parameters:**

- `msg`: `GOOSEMessage`

**Returns:** `i64`

*Defined at line 328*

---

## iec61850_goose_serialize

Serialize GOOSE message to a Vec of bytes (simplified format).

```fides
pub fn iec61850_goose_serialize(msg: GOOSEMessage) -> Vec<i64> {
```

**Parameters:**

- `msg`: `GOOSEMessage`

**Returns:** `Vec<i64>`

*Defined at line 333*

---

## iec61850_ref

Build an IEC 61850 object reference.

```fides
pub fn iec61850_ref(ld: str, ln: str, field: str) -> str {
```

**Parameters:**

- `ld`: `str`
- `ln`: `str`
- `field`: `str`

**Returns:** `str`

*Defined at line 371*

---

## iec61850_dataset_ref

Build a dataset reference.

```fides
pub fn iec61850_dataset_ref(ld: str, ds_name: str) -> str {
```

**Parameters:**

- `ld`: `str`
- `ds_name`: `str`

**Returns:** `str`

*Defined at line 376*

---

## iec61850_control_ref

Build a control object reference.

```fides
pub fn iec61850_control_ref(ld: str, ln: str, co_name: str) -> str {
```

**Parameters:**

- `ld`: `str`
- `ln`: `str`
- `co_name`: `str`

**Returns:** `str`

*Defined at line 381*

---

## PTOC

Overcurrent protection settings.

```fides
pub struct PTOC {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `ln_type` | `i64` |
| `pickup_current` | `f64` |
| `time_delay` | `f64` |
| `inverse_curve` | `bool` |
| `tds` | `f64` |
| `tripped` | `bool` |

*Defined at line 388*

---

## iec61850_ptoc_new

Create a new overcurrent protection element.

```fides
pub fn iec61850_ptoc_new(name: str, pickup: f64, delay: f64) -> PTOC {
```

**Parameters:**

- `name`: `str`
- `pickup`: `f64`
- `delay`: `f64`

**Returns:** `PTOC`

*Defined at line 403*

---

## iec61850_ptoc_check

Check if overcurrent condition is met.

```fides
pub fn iec61850_ptoc_check(ptoc: &mut PTOC, measured_current: f64) -> bool {
```

**Parameters:**

- `ptoc`: `&mut PTOC`
- `measured_current`: `f64`

**Returns:** `bool`

*Defined at line 410*

---

## iec61850_ptoc_set_inverse

Set inverse time curve mode.

```fides
pub fn iec61850_ptoc_set_inverse(ptoc: &mut PTOC, tds: f64) -> void {
```

**Parameters:**

- `ptoc`: `&mut PTOC`
- `tds`: `f64`

**Returns:** `void`

*Defined at line 419*

---

## iec61850_ptoc_trip_time

Calculate trip time for inverse time curve (IEC 60255 standard).
Trip time = TDS * (A / ((I/Is)^p - 1) + B)
For standard inverse: A=0.14, p=0.02, B=0

```fides
pub fn iec61850_ptoc_trip_time(ptoc: PTOC, measured_current: f64) -> f64 {
```

**Parameters:**

- `ptoc`: `PTOC`
- `measured_current`: `f64`

**Returns:** `f64`

*Defined at line 427*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
