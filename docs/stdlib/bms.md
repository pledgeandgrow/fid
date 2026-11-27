[All modules](index.md)

# bms -- Fides Documentation

## Contents

- [OcvTable](#OcvTable) (struct)
- [CellModel](#CellModel) (struct)
- [CellState](#CellState) (struct)
- [bms_ocv_table](#bms_ocv_table) (function)
- [bms_ocv](#bms_ocv) (function)
- [bms_ocv_slope](#bms_ocv_slope) (function)
- [bms_soc_from_ocv](#bms_soc_from_ocv) (function)
- [bms_rint](#bms_rint) (function)
- [bms_thevenin](#bms_thevenin) (function)
- [bms_thevenin2](#bms_thevenin2) (function)
- [bms_cell_state](#bms_cell_state) (function)
- [bms_terminal_voltage](#bms_terminal_voltage) (function)
- [bms_cell_step](#bms_cell_step) (function)
- [CoulombCounter](#CoulombCounter) (struct)
- [bms_coulomb_counter](#bms_coulomb_counter) (function)
- [CoulombCounter_step](#CoulombCounter_step) (function)
- [BmsEkf](#BmsEkf) (struct)
- [bms_ekf](#bms_ekf) (function)
- [BmsEkf_step](#BmsEkf_step) (function)
- [bms_soh_capacity](#bms_soh_capacity) (function)
- [bms_soh_resistance](#bms_soh_resistance) (function)
- [bms_capacity_from_ocv](#bms_capacity_from_ocv) (function)
- [bms_cycles_to_eol](#bms_cycles_to_eol) (function)
- [bms_balance_decisions](#bms_balance_decisions) (function)
- [bms_bleed_ah](#bms_bleed_ah) (function)
- [bms_bleed_time](#bms_bleed_time) (function)
- [BmsPack](#BmsPack) (struct)
- [PackSummary](#PackSummary) (struct)
- [bms_pack](#bms_pack) (function)
- [BmsPack_add_cell](#BmsPack_add_cell) (function)
- [BmsPack_summary](#BmsPack_summary) (function)

## OcvTable

OCV against SoC; `soc` strictly increasing from 0 to 1.

```fides
pub struct OcvTable {
```

**Fields:**

| Name | Type |
|------|------|
| `soc` | `Vec<f64>` |
| `ocv` | `Vec<f64>` |

*Defined at line 32*

---

## CellModel

Equivalent-circuit parameters. r1 = 0 removes the first RC branch
(Rint), r2 = 0 the second (1RC).

```fides
pub struct CellModel {
```

**Fields:**

| Name | Type |
|------|------|
| `capacity_ah` | `f64` |
| `r0` | `f64` |
| `r1` | `f64` |
| `c1` | `f64` |
| `r2` | `f64` |
| `c2` | `f64` |
| `eta_charge` | `f64` |
| `ocv` | `OcvTable` |

*Defined at line 39*

---

## CellState

```fides
pub struct CellState {
```

**Fields:**

| Name | Type |
|------|------|
| `soc` | `f64` |
| `v1` | `f64` |
| `v2` | `f64` |

*Defined at line 50*

---

## bms_ocv_table

```fides
pub fn bms_ocv_table(soc: Vec<f64>, ocv: Vec<f64>) -> OcvTable {
```

**Parameters:**

- `soc`: `Vec<f64>`
- `ocv`: `Vec<f64>`

**Returns:** `OcvTable`

*Defined at line 58*

---

## bms_ocv

OCV at SoC s (clamped to the table).

```fides
pub fn bms_ocv(t: &OcvTable, s: f64) -> f64 {
```

**Parameters:**

- `t`: `&OcvTable`
- `s`: `f64`

**Returns:** `f64`

*Defined at line 77*

---

## bms_ocv_slope

dOCV/dSoC at s (the slope of the segment holding s).

```fides
pub fn bms_ocv_slope(t: &OcvTable, s: f64) -> f64 {
```

**Parameters:**

- `t`: `&OcvTable`
- `s`: `f64`

**Returns:** `f64`

*Defined at line 91*

---

## bms_soc_from_ocv

SoC of a rested cell at voltage v (inverse of the table, clamped).

```fides
pub fn bms_soc_from_ocv(t: &OcvTable, v: f64) -> f64 {
```

**Parameters:**

- `t`: `&OcvTable`
- `v`: `f64`

**Returns:** `f64`

*Defined at line 98*

---

## bms_rint

```fides
pub fn bms_rint(capacity_ah: f64, r0: f64, ocv: OcvTable) -> CellModel {
```

**Parameters:**

- `capacity_ah`: `f64`
- `r0`: `f64`
- `ocv`: `OcvTable`

**Returns:** `CellModel`

*Defined at line 116*

---

## bms_thevenin

```fides
pub fn bms_thevenin(capacity_ah: f64, r0: f64, r1: f64, c1: f64, ocv: OcvTable) -> CellModel {
```

**Parameters:**

- `capacity_ah`: `f64`
- `r0`: `f64`
- `r1`: `f64`
- `c1`: `f64`
- `ocv`: `OcvTable`

**Returns:** `CellModel`

*Defined at line 122*

---

## bms_thevenin2

```fides
pub fn bms_thevenin2(capacity_ah: f64, r0: f64, r1: f64, c1: f64, r2: f64, c2: f64, ocv: OcvTable) -> CellModel {
```

**Parameters:**

- `capacity_ah`: `f64`
- `r0`: `f64`
- `r1`: `f64`
- `c1`: `f64`
- `r2`: `f64`
- `c2`: `f64`
- `ocv`: `OcvTable`

**Returns:** `CellModel`

*Defined at line 128*

---

## bms_cell_state

```fides
pub fn bms_cell_state(soc: f64) -> CellState {
```

**Parameters:**

- `soc`: `f64`

**Returns:** `CellState`

*Defined at line 134*

---

## bms_terminal_voltage

Terminal voltage for the present state and current.

```fides
pub fn bms_terminal_voltage(m: &CellModel, s: &CellState, current: f64) -> f64 {
```

**Parameters:**

- `m`: `&CellModel`
- `s`: `&CellState`
- `current`: `f64`

**Returns:** `f64`

*Defined at line 141*

---

## bms_cell_step

Advance the cell by dt seconds at a constant current; returns the
terminal voltage at the end of the step.

```fides
pub fn bms_cell_step(m: &CellModel, s: &mut CellState, current: f64, dt: f64) -> f64 {
```

**Parameters:**

- `m`: `&CellModel`
- `s`: `&mut CellState`
- `current`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 155*

---

## CoulombCounter

```fides
pub struct CoulombCounter {
```

**Fields:**

| Name | Type |
|------|------|
| `soc` | `f64` |
| `capacity_ah` | `f64` |
| `eta_charge` | `f64` |
| `ah_out` | `f64` |
| `ah_in` | `f64` |

*Defined at line 170*

---

## bms_coulomb_counter

```fides
pub fn bms_coulomb_counter(soc: f64, capacity_ah: f64, eta_charge: f64) -> CoulombCounter {
```

**Parameters:**

- `soc`: `f64`
- `capacity_ah`: `f64`
- `eta_charge`: `f64`

**Returns:** `CoulombCounter`

*Defined at line 178*

---

## CoulombCounter_step

```fides
pub fn CoulombCounter_step(c: &mut CoulombCounter, current: f64, dt: f64) -> f64 {
```

**Parameters:**

- `c`: `&mut CoulombCounter`
- `current`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 184*

---

## BmsEkf

```fides
pub struct BmsEkf {
```

**Fields:**

| Name | Type |
|------|------|
| `soc` | `f64` |
| `v1` | `f64` |
| `p00` | `f64` |
| `p01` | `f64` |
| `p11` | `f64` |
| `q_soc` | `f64` |
| `q_v1` | `f64` |
| `r_v` | `f64` |
| `model` | `CellModel` |

*Defined at line 198*

---

## bms_ekf

An EKF starting at soc0 with variance p_soc; q_* process noise
variances, r_v the voltage measurement variance (V^2).

```fides
pub fn bms_ekf(model: CellModel, soc0: f64, p_soc: f64, q_soc: f64, q_v1: f64, r_v: f64) -> BmsEkf {
```

**Parameters:**

- `model`: `CellModel`
- `soc0`: `f64`
- `p_soc`: `f64`
- `q_soc`: `f64`
- `q_v1`: `f64`
- `r_v`: `f64`

**Returns:** `BmsEkf`

*Defined at line 212*

---

## BmsEkf_step

One predict/correct cycle with the current over the last dt seconds and
the terminal voltage measured at its end. Returns the SoC estimate.

```fides
pub fn BmsEkf_step(e: &mut BmsEkf, current: f64, v_meas: f64, dt: f64) -> f64 {
```

**Parameters:**

- `e`: `&mut BmsEkf`
- `current`: `f64`
- `v_meas`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 220*

---

## bms_soh_capacity

Capacity SoH: measured / rated capacity.

```fides
pub fn bms_soh_capacity(measured_ah: f64, rated_ah: f64) -> f64 {
```

**Parameters:**

- `measured_ah`: `f64`
- `rated_ah`: `f64`

**Returns:** `f64`

*Defined at line 255*

---

## bms_soh_resistance

Resistance SoH: 1 when r == r_new, 0 when r reaches r_eol.

```fides
pub fn bms_soh_resistance(r: f64, r_new: f64, r_eol: f64) -> f64 {
```

**Parameters:**

- `r`: `f64`
- `r_new`: `f64`
- `r_eol`: `f64`

**Returns:** `f64`

*Defined at line 260*

---

## bms_capacity_from_ocv

Capacity from two rested voltages and the charge moved between them
(Ah, positive when discharged).

```fides
pub fn bms_capacity_from_ocv(t: &OcvTable, v_start: f64, v_end: f64, ah_moved: f64) -> f64 {
```

**Parameters:**

- `t`: `&OcvTable`
- `v_start`: `f64`
- `v_end`: `f64`
- `ah_moved`: `f64`

**Returns:** `f64`

*Defined at line 266*

---

## bms_cycles_to_eol

Remaining equivalent full cycles to end of life, from a linear fade
rate (fraction of rated capacity lost per cycle) and the EOL threshold.

```fides
pub fn bms_cycles_to_eol(soh: f64, fade_per_cycle: f64, eol_soh: f64) -> f64 {
```

**Parameters:**

- `soh`: `f64`
- `fade_per_cycle`: `f64`
- `eol_soh`: `f64`

**Returns:** `f64`

*Defined at line 273*

---

## bms_balance_decisions

Which cells to bleed: those more than `threshold` volts above the
lowest cell, when they are at least `min_voltage`.

```fides
pub fn bms_balance_decisions(voltages: &Vec<f64>, threshold: f64, min_voltage: f64) -> Vec<bool> {
```

**Parameters:**

- `voltages`: `&Vec<f64>`
- `threshold`: `f64`
- `min_voltage`: `f64`

**Returns:** `Vec<bool>`

*Defined at line 284*

---

## bms_bleed_ah

Charge (Ah) a bleed resistor of r_bleed ohms removes from a cell at
voltage v in `seconds`.

```fides
pub fn bms_bleed_ah(v: f64, r_bleed: f64, seconds: f64) -> f64 {
```

**Parameters:**

- `v`: `f64`
- `r_bleed`: `f64`
- `seconds`: `f64`

**Returns:** `f64`

*Defined at line 303*

---

## bms_bleed_time

Seconds of bleeding needed to remove the charge between two SoCs.

```fides
pub fn bms_bleed_time(soc_high: f64, soc_target: f64, capacity_ah: f64, v: f64, r_bleed: f64) -> f64 {
```

**Parameters:**

- `soc_high`: `f64`
- `soc_target`: `f64`
- `capacity_ah`: `f64`
- `v`: `f64`
- `r_bleed`: `f64`

**Returns:** `f64`

*Defined at line 308*

---

## BmsPack

s series groups of p cells in parallel; cell k of group g is at
index g * p + k of `cells` and `capacity_ah`.

```fides
pub struct BmsPack {
```

**Fields:**

| Name | Type |
|------|------|
| `series` | `i64` |
| `parallel` | `i64` |
| `capacity_ah` | `Vec<f64>` |
| `cells` | `Vec<CellState>` |

*Defined at line 316*

---

## PackSummary

```fides
pub struct PackSummary {
```

**Fields:**

| Name | Type |
|------|------|
| `voltage` | `f64` |
| `capacity_ah` | `f64` |
| `available_ah` | `f64` |
| `soc` | `f64` |
| `energy_wh` | `f64` |
| `v_min` | `f64` |
| `v_max` | `f64` |
| `weakest_group` | `i64` |

*Defined at line 323*

---

## bms_pack

```fides
pub fn bms_pack(series: i64, parallel: i64) -> BmsPack {
```

**Parameters:**

- `series`: `i64`
- `parallel`: `i64`

**Returns:** `BmsPack`

*Defined at line 334*

---

## BmsPack_add_cell

```fides
pub fn BmsPack_add_cell(p: &mut BmsPack, capacity_ah: f64, soc: f64) {
```

**Parameters:**

- `p`: `&mut BmsPack`
- `capacity_ah`: `f64`
- `soc`: `f64`

*Defined at line 340*

---

## BmsPack_summary

Summary at rest (OCV of every cell from the table `t`). Each parallel
group sits at the capacity-weighted mean OCV; the pack's capacity and
available charge are those of its weakest group. `energy_wh` is the
stored charge of every cell times its present OCV (an estimate).

```fides
pub fn BmsPack_summary(p: &BmsPack, t: &OcvTable) -> PackSummary {
```

**Parameters:**

- `p`: `&BmsPack`
- `t`: `&OcvTable`

**Returns:** `PackSummary`

*Defined at line 349*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
