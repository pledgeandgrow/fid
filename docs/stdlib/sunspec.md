[All modules](index.md)

# sunspec -- Fides Documentation

## Contents

- [SUNSPEC_MARKER_HI:](#SUNSPEC_MARKER_HI:) (const)
- [SUNSPEC_MARKER_LO:](#SUNSPEC_MARKER_LO:) (const)
- [SUNSPEC_END:](#SUNSPEC_END:) (const)
- [SUNSPEC_MODEL_COMMON:](#SUNSPEC_MODEL_COMMON:) (const)
- [SUNSPEC_MODEL_INVERTER_1PH:](#SUNSPEC_MODEL_INVERTER_1PH:) (const)
- [SUNSPEC_MODEL_INVERTER_SPLIT:](#SUNSPEC_MODEL_INVERTER_SPLIT:) (const)
- [SUNSPEC_MODEL_INVERTER_3PH:](#SUNSPEC_MODEL_INVERTER_3PH:) (const)
- [SUNSPEC_MODEL_METER_WYE:](#SUNSPEC_MODEL_METER_WYE:) (const)
- [SUNSPEC_MODEL_DER_AC:](#SUNSPEC_MODEL_DER_AC:) (const)
- [sunspec_i16](#sunspec_i16) (function)
- [sunspec_u32](#sunspec_u32) (function)
- [sunspec_i32](#sunspec_i32) (function)
- [sunspec_apply_sf](#sunspec_apply_sf) (function)
- [sunspec_string](#sunspec_string) (function)
- [sunspec_pack_string](#sunspec_pack_string) (function)
- [sunspec_find_marker](#sunspec_find_marker) (function)
- [SunSpecModel](#SunSpecModel) (struct)
- [sunspec_walk_models](#sunspec_walk_models) (function)
- [sunspec_model_offset](#sunspec_model_offset) (function)
- [SunSpecCommon](#SunSpecCommon) (struct)
- [sunspec_common](#sunspec_common) (function)
- [SunSpecInverter](#SunSpecInverter) (struct)
- [sunspec_inverter](#sunspec_inverter) (function)
- [SunSpecMeter](#SunSpecMeter) (struct)
- [sunspec_meter](#sunspec_meter) (function)
- [sunspec_scan](#sunspec_scan) (function)
- [sunspec_read_inverter](#sunspec_read_inverter) (function)
- [sunspec_read_meter](#sunspec_read_meter) (function)
- [sunspec_read_common](#sunspec_read_common) (function)

## SUNSPEC_MARKER_HI:

```fides
pub const SUNSPEC_MARKER_HI: i64 = 0x5375;  // "Su"
```

*Defined at line 42*

---

## SUNSPEC_MARKER_LO:

```fides
pub const SUNSPEC_MARKER_LO: i64 = 0x6e53;  // "nS"
```

*Defined at line 43*

---

## SUNSPEC_END:

```fides
pub const SUNSPEC_END: i64 = 0xFFFF;
```

*Defined at line 44*

---

## SUNSPEC_MODEL_COMMON:

```fides
pub const SUNSPEC_MODEL_COMMON: i64 = 1;
```

*Defined at line 46*

---

## SUNSPEC_MODEL_INVERTER_1PH:

```fides
pub const SUNSPEC_MODEL_INVERTER_1PH: i64 = 101;
```

*Defined at line 47*

---

## SUNSPEC_MODEL_INVERTER_SPLIT:

```fides
pub const SUNSPEC_MODEL_INVERTER_SPLIT: i64 = 102;
```

*Defined at line 48*

---

## SUNSPEC_MODEL_INVERTER_3PH:

```fides
pub const SUNSPEC_MODEL_INVERTER_3PH: i64 = 103;
```

*Defined at line 49*

---

## SUNSPEC_MODEL_METER_WYE:

```fides
pub const SUNSPEC_MODEL_METER_WYE: i64 = 203;
```

*Defined at line 50*

---

## SUNSPEC_MODEL_DER_AC:

```fides
pub const SUNSPEC_MODEL_DER_AC: i64 = 701;
```

*Defined at line 51*

---

## sunspec_i16

Sign-extend a 16-bit register to a signed i64.

```fides
pub fn sunspec_i16(reg: i64) -> i64 {
```

**Parameters:**

- `reg`: `i64`

**Returns:** `i64`

*Defined at line 56*

---

## sunspec_u32

Read an unsigned 32-bit value from two big-endian registers at `off`.

```fides
pub fn sunspec_u32(regs: &Vec<i64>, off: i64) -> i64 {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 65*

---

## sunspec_i32

Read a signed 32-bit value from two big-endian registers at `off`.

```fides
pub fn sunspec_i32(regs: &Vec<i64>, off: i64) -> i64 {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 70*

---

## sunspec_apply_sf

Apply a SunSpec scale factor (a signed power of ten) to an integer value.

```fides
pub fn sunspec_apply_sf(value: i64, sf: i64) -> f64 {
```

**Parameters:**

- `value`: `i64`
- `sf`: `i64`

**Returns:** `f64`

*Defined at line 79*

---

## sunspec_string

Read a fixed-length string of `nregs` registers (two chars each,
big-endian) at `off`, stopping at a NUL.

```fides
pub fn sunspec_string(regs: &Vec<i64>, off: i64, nregs: i64) -> str {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`
- `nregs`: `i64`

**Returns:** `str`

*Defined at line 98*

---

## sunspec_pack_string

Pack a string into `nregs` registers (two chars each), NUL-padded. Used
to build a device image for testing.

```fides
pub fn sunspec_pack_string(s: str, nregs: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`
- `nregs`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 120*

---

## sunspec_find_marker

Find the "SunS" marker from `start` onward; returns its register offset
or -1. The two registers after the marker begin the first model.

```fides
pub fn sunspec_find_marker(regs: &Vec<i64>, start: i64) -> i64 {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `start`: `i64`

**Returns:** `i64`

*Defined at line 143*

---

## SunSpecModel

A model entry: id, the register offset of its first data point, and the
data length in registers.

```fides
pub struct SunSpecModel {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `data_off` | `i64` |
| `length` | `i64` |

*Defined at line 156*

---

## sunspec_walk_models

Walk the model list beginning just after the marker at `marker_off`.

```fides
pub fn sunspec_walk_models(regs: &Vec<i64>, marker_off: i64) -> Vec<SunSpecModel> {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `marker_off`: `i64`

**Returns:** `Vec<SunSpecModel>`

*Defined at line 163*

---

## sunspec_model_offset

Find a model's data offset by id in a walked list; -1 if absent.

```fides
pub fn sunspec_model_offset(models: &Vec<SunSpecModel>, id: i64) -> i64 {
```

**Parameters:**

- `models`: `&Vec<SunSpecModel>`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 181*

---

## SunSpecCommon

```fides
pub struct SunSpecCommon {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `manufacturer` | `str` |
| `model` | `str` |
| `options` | `str` |
| `version` | `str` |
| `serial` | `str` |
| `device_address` | `i64` |

*Defined at line 194*

---

## sunspec_common

Parse the common model's data at `off` (its length is 66 registers).
Manufacturer/model/options/version/serial are 16/16/8/8/16 registers;
device address is the last register.

```fides
pub fn sunspec_common(regs: &Vec<i64>, off: i64) -> SunSpecCommon {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `SunSpecCommon`

*Defined at line 207*

---

## SunSpecInverter

Scaled inverter readings.

```fides
pub struct SunSpecInverter {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ac_current` | `f64,  // A` |
| `ac_voltage` | `f64,  // V (phase A to N)` |
| `ac_power` | `f64,  // W` |
| `frequency` | `f64,  // Hz` |
| `apparent_power` | `f64,  // VA` |
| `reactive_power` | `f64,  // VAr` |
| `power_factor` | `f64,  // %` |
| `energy_wh` | `f64,  // Wh (accumulated)` |

*Defined at line 216*

---

## sunspec_inverter

Parse an inverter model (101/102/103) at data offset `off`. The point
layout is the standard integer+SF model: A(0) AphA..C(1..3) A_SF(4),
PPV/PhV(5..10) V_SF(11), W(12) W_SF(13), Hz(14) Hz_SF(15), VA(16)
VA_SF(17), VAr(18) VAr_SF(19), PF(20) PF_SF(21), WH(22:23) WH_SF(24).

```fides
pub fn sunspec_inverter(regs: &Vec<i64>, off: i64) -> SunSpecInverter {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `SunSpecInverter`

*Defined at line 232*

---

## SunSpecMeter

```fides
pub struct SunSpecMeter {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ac_current` | `f64,  // A total` |
| `voltage_ln` | `f64,  // V line-to-neutral average` |
| `real_power` | `f64,  // W total` |
| `frequency` | `f64,  // Hz` |
| `exported_wh` | `f64,  // Wh exported (total)` |
| `imported_wh` | `f64,  // Wh imported (total)` |

*Defined at line 248*

---

## sunspec_meter

Parse a wye meter (203) at data offset `off`. Standard layout: A(0)
AphA..C(1..3) A_SF(4); PhV avg(8) ... V_SF(15); Hz(16) Hz_SF(17);
W(18) ... W_SF(22); then energy: TotWhExp(a 32-bit at 38), TotWhImp
(32-bit at 42), TotWh_SF(54). Offsets follow the SunSpec 203 map.

```fides
pub fn sunspec_meter(regs: &Vec<i64>, off: i64) -> SunSpecMeter {
```

**Parameters:**

- `regs`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `SunSpecMeter`

*Defined at line 262*

---

## sunspec_scan

Scan a device from a ModbusMap's holding registers starting at `start`:
returns the walked model list. The marker is found first.

```fides
pub fn sunspec_scan(m: &ModbusMap, start: i64) -> Vec<SunSpecModel> {
```

**Parameters:**

- `m`: `&ModbusMap`
- `start`: `i64`

**Returns:** `Vec<SunSpecModel>`

*Defined at line 277*

---

## sunspec_read_inverter

Read the first inverter model present (101/102/103) from a ModbusMap.

```fides
pub fn sunspec_read_inverter(m: &ModbusMap, models: &Vec<SunSpecModel>) -> SunSpecInverter {
```

**Parameters:**

- `m`: `&ModbusMap`
- `models`: `&Vec<SunSpecModel>`

**Returns:** `SunSpecInverter`

*Defined at line 286*

---

## sunspec_read_meter

Read the wye meter model (203) from a ModbusMap.

```fides
pub fn sunspec_read_meter(m: &ModbusMap, models: &Vec<SunSpecModel>) -> SunSpecMeter {
```

**Parameters:**

- `m`: `&ModbusMap`
- `models`: `&Vec<SunSpecModel>`

**Returns:** `SunSpecMeter`

*Defined at line 303*

---

## sunspec_read_common

Read the common model (1) from a ModbusMap.

```fides
pub fn sunspec_read_common(m: &ModbusMap, models: &Vec<SunSpecModel>) -> SunSpecCommon {
```

**Parameters:**

- `m`: `&ModbusMap`
- `models`: `&Vec<SunSpecModel>`

**Returns:** `SunSpecCommon`

*Defined at line 314*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
