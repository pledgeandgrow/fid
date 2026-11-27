[All modules](index.md)

# microgrid -- Fides Documentation

## Contents

- [MgGenerator](#MgGenerator) (struct)
- [MgStorage](#MgStorage) (struct)
- [Microgrid](#Microgrid) (struct)
- [MgDispatch](#MgDispatch) (struct)
- [mg_new](#mg_new) (function)
- [mg_add_generator](#mg_add_generator) (function)
- [mg_add_step](#mg_add_step) (function)
- [mg_cost](#mg_cost) (function)
- [mg_balance_error](#mg_balance_error) (function)
- [mg_dispatch_rules](#mg_dispatch_rules) (function)
- [mg_dispatch_lp](#mg_dispatch_lp) (function)
- [MG_CONNECTED:](#MG_CONNECTED:) (const)
- [MG_ISLANDED:](#MG_ISLANDED:) (const)
- [MG_RESYNC:](#MG_RESYNC:) (const)
- [IslandLimits](#IslandLimits) (struct)
- [mg_island_limits_1547](#mg_island_limits_1547) (function)
- [IslandController](#IslandController) (struct)
- [mg_island_controller](#mg_island_controller) (function)
- [mg_state_name](#mg_state_name) (function)
- [IslandController_command](#IslandController_command) (function)
- [IslandController_step](#IslandController_step) (function)

## MgGenerator

```fides
pub struct MgGenerator {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `pmin` | `f64` |
| `pmax` | `f64` |
| `cost` | `f64` |

*Defined at line 37*

---

## MgStorage

```fides
pub struct MgStorage {
```

**Fields:**

| Name | Type |
|------|------|
| `capacity_kwh` | `f64` |
| `p_charge_max` | `f64` |
| `p_discharge_max` | `f64` |
| `eta_charge` | `f64` |
| `eta_discharge` | `f64` |
| `soc_min` | `f64` |
| `soc_max` | `f64` |
| `soc_init` | `f64` |

*Defined at line 44*

---

## Microgrid

```fides
pub struct Microgrid {
```

**Fields:**

| Name | Type |
|------|------|
| `gens` | `Vec<MgGenerator>` |
| `storage` | `MgStorage` |
| `load` | `Vec<f64>` |
| `pv` | `Vec<f64>` |
| `import_price` | `Vec<f64>` |
| `export_price` | `Vec<f64>` |
| `grid_up` | `Vec<bool>` |
| `import_limit` | `f64` |
| `export_limit` | `f64` |
| `voll` | `f64` |
| `dt` | `f64` |

*Defined at line 55*

---

## MgDispatch

A schedule: per step values (gen is step-major: gen[t * G + i]); `soc`
is the stored energy (kWh) at the end of each step.

```fides
pub struct MgDispatch {
```

**Fields:**

| Name | Type |
|------|------|
| `feasible` | `bool` |
| `gen` | `Vec<f64>` |
| `charge` | `Vec<f64>` |
| `discharge` | `Vec<f64>` |
| `import` | `Vec<f64>` |
| `export` | `Vec<f64>` |
| `curtail` | `Vec<f64>` |
| `shed` | `Vec<f64>` |
| `soc` | `Vec<f64>` |
| `cost` | `f64` |

*Defined at line 71*

---

## mg_new

```fides
pub fn mg_new(storage: MgStorage, dt: f64, import_limit: f64, export_limit: f64, voll: f64) -> Microgrid {
```

**Parameters:**

- `storage`: `MgStorage`
- `dt`: `f64`
- `import_limit`: `f64`
- `export_limit`: `f64`
- `voll`: `f64`

**Returns:** `Microgrid`

*Defined at line 84*

---

## mg_add_generator

```fides
pub fn mg_add_generator(m: &mut Microgrid, name: str, pmin: f64, pmax: f64, cost: f64) {
```

**Parameters:**

- `m`: `&mut Microgrid`
- `name`: `str`
- `pmin`: `f64`
- `pmax`: `f64`
- `cost`: `f64`

*Defined at line 90*

---

## mg_add_step

Append one step of forecasts.

```fides
pub fn mg_add_step(m: &mut Microgrid, load: f64, pv: f64, import_price: f64, export_price: f64, grid_up: bool) {
```

**Parameters:**

- `m`: `&mut Microgrid`
- `load`: `f64`
- `pv`: `f64`
- `import_price`: `f64`
- `export_price`: `f64`
- `grid_up`: `bool`

*Defined at line 97*

---

## mg_cost

Cost of a schedule: generation + import + VOLL x unserved - export.

```fides
pub fn mg_cost(m: &Microgrid, d: &MgDispatch) -> f64 {
```

**Parameters:**

- `m`: `&Microgrid`
- `d`: `&MgDispatch`

**Returns:** `f64`

*Defined at line 112*

---

## mg_balance_error

Largest power-balance error of a schedule (should be ~0).

```fides
pub fn mg_balance_error(m: &Microgrid, d: &MgDispatch) -> f64 {
```

**Parameters:**

- `m`: `&Microgrid`
- `d`: `&MgDispatch`

**Returns:** `f64`

*Defined at line 129*

---

## mg_dispatch_rules

```fides
pub fn mg_dispatch_rules(m: &Microgrid) -> MgDispatch {
```

**Parameters:**

- `m`: `&Microgrid`

**Returns:** `MgDispatch`

*Defined at line 148*

---

## mg_dispatch_lp

Optimal schedule by linear programming. `keep_energy` requires the
stored energy at the end to be at least the initial energy.

```fides
pub fn mg_dispatch_lp(m: &Microgrid, keep_energy: bool) -> MgDispatch {
```

**Parameters:**

- `m`: `&Microgrid`
- `keep_energy`: `bool`

**Returns:** `MgDispatch`

*Defined at line 257*

---

## MG_CONNECTED:

```fides
pub const MG_CONNECTED: i64 = 0;
```

*Defined at line 379*

---

## MG_ISLANDED:

```fides
pub const MG_ISLANDED: i64 = 1;
```

*Defined at line 380*

---

## MG_RESYNC:

```fides
pub const MG_RESYNC: i64 = 2;
```

*Defined at line 381*

---

## IslandLimits

```fides
pub struct IslandLimits {
```

**Fields:**

| Name | Type |
|------|------|
| `v_min` | `f64` |
| `v_max` | `f64` |
| `f_min` | `f64` |
| `f_max` | `f64` |
| `trip_delay` | `f64` |
| `reconnect_delay` | `f64` |
| `sync_dv` | `f64` |
| `sync_df` | `f64` |
| `sync_dphase` | `f64` |

*Defined at line 383*

---

## mg_island_limits_1547

Limits of IEEE 1547: 0.88..1.10 pu, 59.3..60.5 Hz (60 Hz systems),
a 0.16 s trip delay, 300 s in band before reconnecting, and the
synchronisation limits for units up to 500 kVA.

```fides
pub fn mg_island_limits_1547() -> IslandLimits {
```

**Returns:** `IslandLimits`

*Defined at line 398*

---

## IslandController

```fides
pub struct IslandController {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `limits` | `IslandLimits` |
| `abnormal_for` | `f64` |
| `normal_for` | `f64` |
| `breaker_closed` | `bool` |
| `hold_island` | `bool` |
| `events` | `Vec<str>` |
| `time` | `f64` |

*Defined at line 404*

---

## mg_island_controller

```fides
pub fn mg_island_controller(limits: IslandLimits) -> IslandController {
```

**Parameters:**

- `limits`: `IslandLimits`

**Returns:** `IslandController`

*Defined at line 415*

---

## mg_state_name

```fides
pub fn mg_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 421*

---

## IslandController_command

Intentional islanding (true) or release of the hold (false).

```fides
pub fn IslandController_command(c: &mut IslandController, island: bool) {
```

**Parameters:**

- `c`: `&mut IslandController`
- `island`: `bool`

*Defined at line 436*

---

## IslandController_step

Advance by dt seconds with the grid-side voltage (pu) and frequency
(Hz), and the voltage difference (pu), frequency difference (Hz) and
phase difference (degrees) across the open breaker. Returns the state.

```fides
pub fn IslandController_step(c: &mut IslandController, dt: f64, grid_v: f64, grid_f: f64, dv: f64, df: f64, dphase: f64) -> i64 {
```

**Parameters:**

- `c`: `&mut IslandController`
- `dt`: `f64`
- `grid_v`: `f64`
- `grid_f`: `f64`
- `dv`: `f64`
- `df`: `f64`
- `dphase`: `f64`

**Returns:** `i64`

*Defined at line 449*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
