[All modules](index.md)

# thermal -- Fides Documentation

## Contents

- [WallLayer](#WallLayer) (struct)
- [Wall](#Wall) (struct)
- [thermal_wall](#thermal_wall) (function)
- [thermal_add_layer](#thermal_add_layer) (function)
- [thermal_add_resistance](#thermal_add_resistance) (function)
- [thermal_wall_r](#thermal_wall_r) (function)
- [thermal_u_value](#thermal_u_value) (function)
- [thermal_heat_loss](#thermal_heat_loss) (function)
- [thermal_interface_temps](#thermal_interface_temps) (function)
- [Rc1](#Rc1) (struct)
- [Rc1_step](#Rc1_step) (function)
- [Rc2](#Rc2) (struct)
- [Rc2_step](#Rc2_step) (function)
- [Rc2_steady](#Rc2_steady) (function)
- [thermal_steady_power](#thermal_steady_power) (function)
- [thermal_hdd](#thermal_hdd) (function)
- [thermal_cdd](#thermal_cdd) (function)
- [thermal_hdd_hourly](#thermal_hdd_hourly) (function)
- [thermal_cdd_hourly](#thermal_cdd_hourly) (function)
- [thermal_degree_day_energy](#thermal_degree_day_energy) (function)
- [thermal_cop_carnot_heating](#thermal_cop_carnot_heating) (function)
- [thermal_cop_carnot_cooling](#thermal_cop_carnot_cooling) (function)
- [thermal_cop_quadratic](#thermal_cop_quadratic) (function)
- [thermal_scop](#thermal_scop) (function)
- [thermal_pws](#thermal_pws) (function)
- [thermal_humidity_ratio](#thermal_humidity_ratio) (function)
- [thermal_pw_from_w](#thermal_pw_from_w) (function)
- [thermal_w_from_rh](#thermal_w_from_rh) (function)
- [thermal_rh](#thermal_rh) (function)
- [thermal_dew_point](#thermal_dew_point) (function)
- [thermal_enthalpy](#thermal_enthalpy) (function)
- [thermal_specific_volume](#thermal_specific_volume) (function)
- [thermal_w_from_wet_bulb](#thermal_w_from_wet_bulb) (function)
- [thermal_wet_bulb](#thermal_wet_bulb) (function)

## WallLayer

```fides
pub struct WallLayer {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `thickness` | `f64` |
| `conductivity` | `f64` |
| `resistance` | `f64` |

*Defined at line 33*

---

## Wall

```fides
pub struct Wall {
```

**Fields:**

| Name | Type |
|------|------|
| `layers` | `Vec<WallLayer>` |
| `rsi` | `f64` |
| `rse` | `f64` |

*Defined at line 40*

---

## thermal_wall

A wall with ISO 6946 surface resistances for horizontal heat flow.

```fides
pub fn thermal_wall() -> Wall {
```

**Returns:** `Wall`

*Defined at line 47*

---

## thermal_add_layer

A solid layer (outside first): thickness m, conductivity W/mK.

```fides
pub fn thermal_add_layer(w: &mut Wall, name: str, thickness: f64, k: f64) {
```

**Parameters:**

- `w`: `&mut Wall`
- `name`: `str`
- `thickness`: `f64`
- `k`: `f64`

*Defined at line 54*

---

## thermal_add_resistance

A layer given by its thermal resistance (air cavity), m2K/W.

```fides
pub fn thermal_add_resistance(w: &mut Wall, name: str, r: f64) {
```

**Parameters:**

- `w`: `&mut Wall`
- `name`: `str`
- `r`: `f64`

*Defined at line 61*

---

## thermal_wall_r

```fides
pub fn thermal_wall_r(w: &Wall) -> f64 {
```

**Parameters:**

- `w`: `&Wall`

**Returns:** `f64`

*Defined at line 67*

---

## thermal_u_value

```fides
pub fn thermal_u_value(w: &Wall) -> f64 {
```

**Parameters:**

- `w`: `&Wall`

**Returns:** `f64`

*Defined at line 77*

---

## thermal_heat_loss

Steady heat flow (W) through area a for inside/outside temperatures.

```fides
pub fn thermal_heat_loss(w: &Wall, area: f64, t_in: f64, t_out: f64) -> f64 {
```

**Parameters:**

- `w`: `&Wall`
- `area`: `f64`
- `t_in`: `f64`
- `t_out`: `f64`

**Returns:** `f64`

*Defined at line 82*

---

## thermal_interface_temps

Temperatures from the outside air inwards: outside surface, every
interface between layers, inside surface (layers.len() + 1 values).

```fides
pub fn thermal_interface_temps(w: &Wall, t_in: f64, t_out: f64) -> Vec<f64> {
```

**Parameters:**

- `w`: `&Wall`
- `t_in`: `f64`
- `t_out`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 88*

---

## Rc1

1R1C: resistance R (K/W) to the outside, capacitance C (J/K).

```fides
pub struct Rc1 {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `f64` |
| `c` | `f64` |
| `t` | `f64` |

*Defined at line 105*

---

## Rc1_step

Advance dt seconds with outside temperature ta and heat input q (W)
held constant over the step (exact solution).

```fides
pub fn Rc1_step(m: &mut Rc1, dt: f64, ta: f64, q: f64) -> f64 {
```

**Parameters:**

- `m`: `&mut Rc1`
- `dt`: `f64`
- `ta`: `f64`
- `q`: `f64`

**Returns:** `f64`

*Defined at line 113*

---

## Rc2

2R2C: indoor air (ti, ci) coupled to the envelope mass (te, ce) by ri,
the envelope to outside by re. Heating goes to the air, solar gains to
the envelope mass.

```fides
pub struct Rc2 {
```

**Fields:**

| Name | Type |
|------|------|
| `ri` | `f64` |
| `re` | `f64` |
| `ci` | `f64` |
| `ce` | `f64` |
| `ti` | `f64` |
| `te` | `f64` |

*Defined at line 122*

---

## Rc2_step

Backward-Euler step: solves the 2x2 implicit system exactly.

```fides
pub fn Rc2_step(m: &mut Rc2, dt: f64, ta: f64, q_heat: f64, q_solar: f64) {
```

**Parameters:**

- `m`: `&mut Rc2`
- `dt`: `f64`
- `ta`: `f64`
- `q_heat`: `f64`
- `q_solar`: `f64`

*Defined at line 132*

---

## Rc2_steady

Steady indoor temperature of the 2R2C model.

```fides
pub fn Rc2_steady(m: &Rc2, ta: f64, q_heat: f64, q_solar: f64) -> f64 {
```

**Parameters:**

- `m`: `&Rc2`
- `ta`: `f64`
- `q_heat`: `f64`
- `q_solar`: `f64`

**Returns:** `f64`

*Defined at line 147*

---

## thermal_steady_power

Heating power that holds `t_set` against `ta` in a 1R1C building.

```fides
pub fn thermal_steady_power(r: f64, t_set: f64, ta: f64) -> f64 {
```

**Parameters:**

- `r`: `f64`
- `t_set`: `f64`
- `ta`: `f64`

**Returns:** `f64`

*Defined at line 154*

---

## thermal_hdd

Heating degree-days from daily mean temperatures.

```fides
pub fn thermal_hdd(daily_mean: &Vec<f64>, base: f64) -> f64 {
```

**Parameters:**

- `daily_mean`: `&Vec<f64>`
- `base`: `f64`

**Returns:** `f64`

*Defined at line 161*

---

## thermal_cdd

```fides
pub fn thermal_cdd(daily_mean: &Vec<f64>, base: f64) -> f64 {
```

**Parameters:**

- `daily_mean`: `&Vec<f64>`
- `base`: `f64`

**Returns:** `f64`

*Defined at line 171*

---

## thermal_hdd_hourly

Heating degree-days integrated from hourly temperatures (degree-hours
/ 24).

```fides
pub fn thermal_hdd_hourly(hourly: &Vec<f64>, base: f64) -> f64 {
```

**Parameters:**

- `hourly`: `&Vec<f64>`
- `base`: `f64`

**Returns:** `f64`

*Defined at line 183*

---

## thermal_cdd_hourly

```fides
pub fn thermal_cdd_hourly(hourly: &Vec<f64>, base: f64) -> f64 {
```

**Parameters:**

- `hourly`: `&Vec<f64>`
- `base`: `f64`

**Returns:** `f64`

*Defined at line 187*

---

## thermal_degree_day_energy

Seasonal heating energy (kWh) of a building with heat loss coefficient
h (W/K): h x HDD x 24 / 1000.

```fides
pub fn thermal_degree_day_energy(h_w_per_k: f64, hdd: f64) -> f64 {
```

**Parameters:**

- `h_w_per_k`: `f64`
- `hdd`: `f64`

**Returns:** `f64`

*Defined at line 193*

---

## thermal_cop_carnot_heating

Carnot heating COP between sink t_hot and source t_cold (C).

```fides
pub fn thermal_cop_carnot_heating(t_hot: f64, t_cold: f64) -> f64 {
```

**Parameters:**

- `t_hot`: `f64`
- `t_cold`: `f64`

**Returns:** `f64`

*Defined at line 200*

---

## thermal_cop_carnot_cooling

```fides
pub fn thermal_cop_carnot_cooling(t_hot: f64, t_cold: f64) -> f64 {
```

**Parameters:**

- `t_hot`: `f64`
- `t_cold`: `f64`

**Returns:** `f64`

*Defined at line 205*

---

## thermal_cop_quadratic

COP = a + b lift + c lift^2, lift = t_hot - t_cold (a regression of
manufacturer data).

```fides
pub fn thermal_cop_quadratic(a: f64, b: f64, c: f64, t_hot: f64, t_cold: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `c`: `f64`
- `t_hot`: `f64`
- `t_cold`: `f64`

**Returns:** `f64`

*Defined at line 212*

---

## thermal_scop

Bin method: heat demand (kWh) and COP per temperature bin; seasonal
COP = sum heat / sum (heat / COP).

```fides
pub fn thermal_scop(heat: &Vec<f64>, cop: &Vec<f64>) -> f64 {
```

**Parameters:**

- `heat`: `&Vec<f64>`
- `cop`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 219*

---

## thermal_pws

Saturation vapour pressure (kPa), Hyland-Wexler (ASHRAE eq. 5 and 6).

```fides
pub fn thermal_pws(t: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`

**Returns:** `f64`

*Defined at line 234*

---

## thermal_humidity_ratio

Humidity ratio from vapour partial pressure pw at total pressure p.

```fides
pub fn thermal_humidity_ratio(pw: f64, p: f64) -> f64 {
```

**Parameters:**

- `pw`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 245*

---

## thermal_pw_from_w

Vapour partial pressure from humidity ratio.

```fides
pub fn thermal_pw_from_w(w: f64, p: f64) -> f64 {
```

**Parameters:**

- `w`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 250*

---

## thermal_w_from_rh

Humidity ratio of air at t with relative humidity rh (0..1).

```fides
pub fn thermal_w_from_rh(t: f64, rh: f64, p: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `rh`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 255*

---

## thermal_rh

```fides
pub fn thermal_rh(t: f64, w: f64, p: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `w`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 259*

---

## thermal_dew_point

Dew point (C) for vapour pressure pw: the temperature whose saturation
pressure is pw (bisection on -100..200 C).

```fides
pub fn thermal_dew_point(pw: f64) -> f64 {
```

**Parameters:**

- `pw`: `f64`

**Returns:** `f64`

*Defined at line 265*

---

## thermal_enthalpy

Specific enthalpy of moist air, kJ/kg dry air.

```fides
pub fn thermal_enthalpy(t: f64, w: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `w`: `f64`

**Returns:** `f64`

*Defined at line 282*

---

## thermal_specific_volume

Specific volume, m3/kg dry air.

```fides
pub fn thermal_specific_volume(t: f64, w: f64, p: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `w`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 287*

---

## thermal_w_from_wet_bulb

Humidity ratio from dry bulb t and thermodynamic wet bulb tw (ASHRAE
eq. 33, above freezing).

```fides
pub fn thermal_w_from_wet_bulb(t: f64, tw: f64, p: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `tw`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 293*

---

## thermal_wet_bulb

Thermodynamic wet bulb for dry bulb t and humidity ratio w (bisection
between the dew point and t).

```fides
pub fn thermal_wet_bulb(t: f64, w: f64, p: f64) -> f64 {
```

**Parameters:**

- `t`: `f64`
- `w`: `f64`
- `p`: `f64`

**Returns:** `f64`

*Defined at line 300*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
