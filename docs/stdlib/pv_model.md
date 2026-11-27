[All modules](index.md)

# pv_model -- Fides Documentation

## Contents

- [pv_pi](#pv_pi) (function)
- [pv_julian_day](#pv_julian_day) (function)
- [pv_day_of_year](#pv_day_of_year) (function)
- [SolarPosition](#SolarPosition) (struct)
- [pv_solar_position](#pv_solar_position) (function)
- [pv_extraterrestrial](#pv_extraterrestrial) (function)
- [pv_airmass](#pv_airmass) (function)
- [pv_aoi](#pv_aoi) (function)
- [PoaIrradiance](#PoaIrradiance) (struct)
- [pv_poa_isotropic](#pv_poa_isotropic) (function)
- [pv_poa_hay_davies](#pv_poa_hay_davies) (function)
- [pv_cell_temp_noct](#pv_cell_temp_noct) (function)
- [pv_cell_temp_faiman](#pv_cell_temp_faiman) (function)
- [DiodeParams](#DiodeParams) (struct)
- [pv_thermal_voltage](#pv_thermal_voltage) (function)
- [pv_diode_params](#pv_diode_params) (function)
- [pv_desoto](#pv_desoto) (function)
- [pv_current](#pv_current) (function)
- [pv_isc](#pv_isc) (function)
- [pv_voc](#pv_voc) (function)
- [MppPoint](#MppPoint) (struct)
- [pv_mpp](#pv_mpp) (function)
- [pv_fill_factor](#pv_fill_factor) (function)
- [SandiaInverter](#SandiaInverter) (struct)
- [pv_sandia_inverter](#pv_sandia_inverter) (function)
- [pv_efficiency_curve](#pv_efficiency_curve) (function)
- [pv_cec_efficiency](#pv_cec_efficiency) (function)
- [pv_euro_efficiency](#pv_euro_efficiency) (function)

## pv_pi

```fides
pub fn pv_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 34*

---

## pv_julian_day

Julian day of a Gregorian date at `hour_ut` hours (UT).

```fides
pub fn pv_julian_day(year: i64, month: i64, day: i64, hour_ut: f64) -> f64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `hour_ut`: `f64`

**Returns:** `f64`

*Defined at line 67*

---

## pv_day_of_year

Day of the year (1 = 1 January).

```fides
pub fn pv_day_of_year(year: i64, month: i64, day: i64) -> i64 {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`

**Returns:** `i64`

*Defined at line 82*

---

## SolarPosition

```fides
pub struct SolarPosition {
```

**Fields:**

| Name | Type |
|------|------|
| `zenith` | `f64` |
| `true_zenith` | `f64` |
| `azimuth` | `f64` |
| `declination` | `f64` |
| `hour_angle` | `f64` |
| `elevation` | `f64` |

*Defined at line 86*

---

## pv_solar_position

Sun position for a UT instant at latitude/longitude (degrees, east
positive), with refraction for pressure (hPa) and temperature (C).

```fides
pub fn pv_solar_position(year: i64, month: i64, day: i64, hour_ut: f64, lat: f64, lon: f64, pressure: f64, temp: f64) -> SolarPosition {
```

**Parameters:**

- `year`: `i64`
- `month`: `i64`
- `day`: `i64`
- `hour_ut`: `f64`
- `lat`: `f64`
- `lon`: `f64`
- `pressure`: `f64`
- `temp`: `f64`

**Returns:** `SolarPosition`

*Defined at line 97*

---

## pv_extraterrestrial

Extraterrestrial normal irradiance on day-of-year `doy` (Spencer 1971).

```fides
pub fn pv_extraterrestrial(doy: i64) -> f64 {
```

**Parameters:**

- `doy`: `i64`

**Returns:** `f64`

*Defined at line 125*

---

## pv_airmass

Relative (not pressure-corrected) air mass, Kasten & Young 1989.

```fides
pub fn pv_airmass(zenith: f64) -> f64 {
```

**Parameters:**

- `zenith`: `f64`

**Returns:** `f64`

*Defined at line 132*

---

## pv_aoi

Angle of incidence (degrees) on a surface of `tilt` facing `surf_az`.

```fides
pub fn pv_aoi(zenith: f64, azimuth: f64, tilt: f64, surf_az: f64) -> f64 {
```

**Parameters:**

- `zenith`: `f64`
- `azimuth`: `f64`
- `tilt`: `f64`
- `surf_az`: `f64`

**Returns:** `f64`

*Defined at line 142*

---

## PoaIrradiance

```fides
pub struct PoaIrradiance {
```

**Fields:**

| Name | Type |
|------|------|
| `total` | `f64` |
| `beam` | `f64` |
| `sky_diffuse` | `f64` |
| `ground` | `f64` |
| `aoi` | `f64` |

*Defined at line 149*

---

## pv_poa_isotropic

Isotropic sky (Liu & Jordan): diffuse DHI (1 + cos tilt)/2.

```fides
pub fn pv_poa_isotropic(zenith: f64, azimuth: f64, tilt: f64, surf_az: f64, dni: f64, ghi: f64, dhi: f64, albedo: f64) -> PoaIrradiance {
```

**Parameters:**

- `zenith`: `f64`
- `azimuth`: `f64`
- `tilt`: `f64`
- `surf_az`: `f64`
- `dni`: `f64`
- `ghi`: `f64`
- `dhi`: `f64`
- `albedo`: `f64`

**Returns:** `PoaIrradiance`

*Defined at line 167*

---

## pv_poa_hay_davies

Hay-Davies sky: anisotropy index Ai = DNI / DNI_extra; circumsolar part
DHI Ai Rb with Rb = cos(aoi) / cos(zenith) (cos zenith floored at
cos 89 deg), the rest isotropic.

```fides
pub fn pv_poa_hay_davies(zenith: f64, azimuth: f64, tilt: f64, surf_az: f64, dni: f64, ghi: f64, dhi: f64, albedo: f64, dni_extra: f64) -> PoaIrradiance {
```

**Parameters:**

- `zenith`: `f64`
- `azimuth`: `f64`
- `tilt`: `f64`
- `surf_az`: `f64`
- `dni`: `f64`
- `ghi`: `f64`
- `dhi`: `f64`
- `albedo`: `f64`
- `dni_extra`: `f64`

**Returns:** `PoaIrradiance`

*Defined at line 177*

---

## pv_cell_temp_noct

NOCT model: Tc = Ta + (NOCT - 20) / 800 * G.

```fides
pub fn pv_cell_temp_noct(t_air: f64, poa: f64, noct: f64) -> f64 {
```

**Parameters:**

- `t_air`: `f64`
- `poa`: `f64`
- `noct`: `f64`

**Returns:** `f64`

*Defined at line 190*

---

## pv_cell_temp_faiman

Faiman model: Tc = Ta + G / (U0 + U1 * wind); typical U0 = 25,
U1 = 6.84 W/m^2/K per m/s.

```fides
pub fn pv_cell_temp_faiman(t_air: f64, poa: f64, wind: f64, u0: f64, u1: f64) -> f64 {
```

**Parameters:**

- `t_air`: `f64`
- `poa`: `f64`
- `wind`: `f64`
- `u0`: `f64`
- `u1`: `f64`

**Returns:** `f64`

*Defined at line 196*

---

## DiodeParams

Parameters at one operating condition: photocurrent, saturation current,
series and shunt resistance, and n * Ns * Vt (the modified ideality
factor, volts).

```fides
pub struct DiodeParams {
```

**Fields:**

| Name | Type |
|------|------|
| `il` | `f64` |
| `i0` | `f64` |
| `rs` | `f64` |
| `rsh` | `f64` |
| `nnsvth` | `f64` |

*Defined at line 205*

---

## pv_thermal_voltage

Thermal voltage k T / q at cell temperature tc (C).

```fides
pub fn pv_thermal_voltage(tc: f64) -> f64 {
```

**Parameters:**

- `tc`: `f64`

**Returns:** `f64`

*Defined at line 214*

---

## pv_diode_params

Parameters from a datasheet-style set: ideality n, Ns cells in series,
cell temperature tc.

```fides
pub fn pv_diode_params(il: f64, i0: f64, rs: f64, rsh: f64, n: f64, ns: i64, tc: f64) -> DiodeParams {
```

**Parameters:**

- `il`: `f64`
- `i0`: `f64`
- `rs`: `f64`
- `rsh`: `f64`
- `n`: `f64`
- `ns`: `i64`
- `tc`: `f64`

**Returns:** `DiodeParams`

*Defined at line 220*

---

## pv_desoto

De Soto et al. (2006): parameters at irradiance g (W/m^2) and cell
temperature tc from reference ones at 1000 W/m^2, 25 C; alpha_isc in
A/K, band gap eg_ref (eV, 1.121 for silicon) with dEg/dT = -0.0002677.

```fides
pub fn pv_desoto(reference: &DiodeParams, alpha_isc: f64, eg_ref: f64, g: f64, tc: f64) -> DiodeParams {
```

**Parameters:**

- `reference`: `&DiodeParams`
- `alpha_isc`: `f64`
- `eg_ref`: `f64`
- `g`: `f64`
- `tc`: `f64`

**Returns:** `DiodeParams`

*Defined at line 229*

---

## pv_current

Current at terminal voltage v (Newton on the implicit equation,
started at the photocurrent).

```fides
pub fn pv_current(p: &DiodeParams, v: f64) -> f64 {
```

**Parameters:**

- `p`: `&DiodeParams`
- `v`: `f64`

**Returns:** `f64`

*Defined at line 243*

---

## pv_isc

```fides
pub fn pv_isc(p: &DiodeParams) -> f64 {
```

**Parameters:**

- `p`: `&DiodeParams`

**Returns:** `f64`

*Defined at line 267*

---

## pv_voc

Open-circuit voltage: Newton on I(V) = 0 with I = 0.

```fides
pub fn pv_voc(p: &DiodeParams) -> f64 {
```

**Parameters:**

- `p`: `&DiodeParams`

**Returns:** `f64`

*Defined at line 272*

---

## MppPoint

```fides
pub struct MppPoint {
```

**Fields:**

| Name | Type |
|------|------|
| `v` | `f64` |
| `i` | `f64` |
| `p` | `f64` |

*Defined at line 290*

---

## pv_mpp

Maximum power point by golden-section search on P(V) over [0, Voc].

```fides
pub fn pv_mpp(p: &DiodeParams) -> MppPoint {
```

**Parameters:**

- `p`: `&DiodeParams`

**Returns:** `MppPoint`

*Defined at line 297*

---

## pv_fill_factor

Fill factor Pmp / (Voc Isc).

```fides
pub fn pv_fill_factor(p: &DiodeParams) -> f64 {
```

**Parameters:**

- `p`: `&DiodeParams`

**Returns:** `f64`

*Defined at line 330*

---

## SandiaInverter

Sandia inverter model parameters (King et al. 2007).

```fides
pub struct SandiaInverter {
```

**Fields:**

| Name | Type |
|------|------|
| `paco` | `f64` |
| `pdco` | `f64` |
| `vdco` | `f64` |
| `pso` | `f64` |
| `c0` | `f64` |
| `c1` | `f64` |
| `c2` | `f64` |
| `c3` | `f64` |
| `pnt` | `f64` |

*Defined at line 337*

---

## pv_sandia_inverter

AC output for DC input power pdc at DC voltage vdc; clipped at paco,
-pnt (night tare) below the start-up power pso.

```fides
pub fn pv_sandia_inverter(inv: &SandiaInverter, pdc: f64, vdc: f64) -> f64 {
```

**Parameters:**

- `inv`: `&SandiaInverter`
- `pdc`: `f64`
- `vdc`: `f64`

**Returns:** `f64`

*Defined at line 351*

---

## pv_efficiency_curve

Efficiency from a curve of (load fraction, efficiency) points, linear
between them and flat beyond the ends.

```fides
pub fn pv_efficiency_curve(load: &Vec<f64>, eff: &Vec<f64>, fraction: f64) -> f64 {
```

**Parameters:**

- `load`: `&Vec<f64>`
- `eff`: `&Vec<f64>`
- `fraction`: `f64`

**Returns:** `f64`

*Defined at line 365*

---

## pv_cec_efficiency

CEC weighted efficiency: weights 0.04 0.05 0.12 0.21 0.53 0.05 at 10,
20, 30, 50, 75 and 100 % load.

```fides
pub fn pv_cec_efficiency(load: &Vec<f64>, eff: &Vec<f64>) -> f64 {
```

**Parameters:**

- `load`: `&Vec<f64>`
- `eff`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 383*

---

## pv_euro_efficiency

European weighted efficiency: weights 0.03 0.06 0.13 0.10 0.48 0.20 at
5, 10, 20, 30, 50 and 100 % load.

```fides
pub fn pv_euro_efficiency(load: &Vec<f64>, eff: &Vec<f64>) -> f64 {
```

**Parameters:**

- `load`: `&Vec<f64>`
- `eff`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 389*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
