[All modules](index.md)

# wind_model -- Fides Documentation

## Contents

- [wind_pi](#wind_pi) (function)
- [PowerCurve](#PowerCurve) (struct)
- [wind_power_curve](#wind_power_curve) (function)
- [wind_power](#wind_power) (function)
- [wind_power_cubic](#wind_power_cubic) (function)
- [wind_density_normalise](#wind_density_normalise) (function)
- [wind_air_density](#wind_air_density) (function)
- [wind_power_available](#wind_power_available) (function)
- [wind_gamma](#wind_gamma) (function)
- [wind_weibull_pdf](#wind_weibull_pdf) (function)
- [wind_weibull_cdf](#wind_weibull_cdf) (function)
- [wind_weibull_mean](#wind_weibull_mean) (function)
- [wind_rayleigh_scale](#wind_rayleigh_scale) (function)
- [wind_weibull_fit](#wind_weibull_fit) (function)
- [wind_weibull_fit_samples](#wind_weibull_fit_samples) (function)
- [wind_aep](#wind_aep) (function)
- [wind_aep_histogram](#wind_aep_histogram) (function)
- [wind_capacity_factor](#wind_capacity_factor) (function)
- [wind_shear_power](#wind_shear_power) (function)
- [wind_shear_log](#wind_shear_log) (function)
- [wind_shear_alpha](#wind_shear_alpha) (function)
- [wind_jensen_deficit](#wind_jensen_deficit) (function)
- [wind_circle_overlap](#wind_circle_overlap) (function)
- [wind_jensen_rotor_deficit](#wind_jensen_rotor_deficit) (function)
- [wind_farm_speeds](#wind_farm_speeds) (function)

## wind_pi

```fides
pub fn wind_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 29*

---

## PowerCurve

```fides
pub struct PowerCurve {
```

**Fields:**

| Name | Type |
|------|------|
| `speeds` | `Vec<f64>` |
| `powers` | `Vec<f64>` |
| `cut_in` | `f64` |
| `cut_out` | `f64` |

*Defined at line 35*

---

## wind_power_curve

A tabulated curve; speeds increasing. Below `cut_in` and above
`cut_out` the output is 0.

```fides
pub fn wind_power_curve(speeds: Vec<f64>, powers: Vec<f64>, cut_in: f64, cut_out: f64) -> PowerCurve {
```

**Parameters:**

- `speeds`: `Vec<f64>`
- `powers`: `Vec<f64>`
- `cut_in`: `f64`
- `cut_out`: `f64`

**Returns:** `PowerCurve`

*Defined at line 44*

---

## wind_power

```fides
pub fn wind_power(pc: &PowerCurve, v: f64) -> f64 {
```

**Parameters:**

- `pc`: `&PowerCurve`
- `v`: `f64`

**Returns:** `f64`

*Defined at line 53*

---

## wind_power_cubic

Idealised curve: rated * (v^3 - vi^3) / (vr^3 - vi^3) between cut-in
and rated speed, rated up to cut-out.

```fides
pub fn wind_power_cubic(v: f64, cut_in: f64, rated_speed: f64, rated: f64, cut_out: f64) -> f64 {
```

**Parameters:**

- `v`: `f64`
- `cut_in`: `f64`
- `rated_speed`: `f64`
- `rated`: `f64`
- `cut_out`: `f64`

**Returns:** `f64`

*Defined at line 74*

---

## wind_density_normalise

IEC 61400-12-1 normalised speed for a site air density rho (reference
rho0, 1.225 kg/m^3 at sea level).

```fides
pub fn wind_density_normalise(v: f64, rho: f64, rho0: f64) -> f64 {
```

**Parameters:**

- `v`: `f64`
- `rho`: `f64`
- `rho0`: `f64`

**Returns:** `f64`

*Defined at line 87*

---

## wind_air_density

Air density from pressure (Pa) and temperature (C), dry air.

```fides
pub fn wind_air_density(pressure_pa: f64, temp_c: f64) -> f64 {
```

**Parameters:**

- `pressure_pa`: `f64`
- `temp_c`: `f64`

**Returns:** `f64`

*Defined at line 92*

---

## wind_power_available

Power in the wind through a rotor of diameter d: 0.5 rho A v^3 (W).

```fides
pub fn wind_power_available(rho: f64, d: f64, v: f64) -> f64 {
```

**Parameters:**

- `rho`: `f64`
- `d`: `f64`
- `v`: `f64`

**Returns:** `f64`

*Defined at line 97*

---

## wind_gamma

Gamma function (Lanczos, g = 7, 9 terms; reflection below 0.5).

```fides
pub fn wind_gamma(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 104*

---

## wind_weibull_pdf

```fides
pub fn wind_weibull_pdf(v: f64, k: f64, c: f64) -> f64 {
```

**Parameters:**

- `v`: `f64`
- `k`: `f64`
- `c`: `f64`

**Returns:** `f64`

*Defined at line 129*

---

## wind_weibull_cdf

```fides
pub fn wind_weibull_cdf(v: f64, k: f64, c: f64) -> f64 {
```

**Parameters:**

- `v`: `f64`
- `k`: `f64`
- `c`: `f64`

**Returns:** `f64`

*Defined at line 146*

---

## wind_weibull_mean

```fides
pub fn wind_weibull_mean(k: f64, c: f64) -> f64 {
```

**Parameters:**

- `k`: `f64`
- `c`: `f64`

**Returns:** `f64`

*Defined at line 153*

---

## wind_rayleigh_scale

Rayleigh (k = 2) scale for a mean speed: c = 2 mean / sqrt(pi).

```fides
pub fn wind_rayleigh_scale(mean: f64) -> f64 {
```

**Parameters:**

- `mean`: `f64`

**Returns:** `f64`

*Defined at line 158*

---

## wind_weibull_fit

Justus' empirical fit: k = (sigma / mean)^-1.086, c = mean / G(1+1/k).
Returns [k, c].

```fides
pub fn wind_weibull_fit(mean: f64, sigma: f64) -> Vec<f64> {
```

**Parameters:**

- `mean`: `f64`
- `sigma`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 164*

---

## wind_weibull_fit_samples

Mean and standard deviation of a sample of speeds, then Justus' fit.

```fides
pub fn wind_weibull_fit_samples(v: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `v`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 173*

---

## wind_aep

Annual energy (curve units x hours) for Weibull(k, c): 8760 x the
Simpson integral of P(v) f(v) over [0, vmax] with `steps` (even)
intervals, times `availability` (0..1).

```fides
pub fn wind_aep(pc: &PowerCurve, k: f64, c: f64, availability: f64) -> f64 {
```

**Parameters:**

- `pc`: `&PowerCurve`
- `k`: `f64`
- `c`: `f64`
- `availability`: `f64`

**Returns:** `f64`

*Defined at line 195*

---

## wind_aep_histogram

Annual energy from a histogram: hours[i] at speed bin centre speeds[i].

```fides
pub fn wind_aep_histogram(pc: &PowerCurve, speeds: &Vec<f64>, hours: &Vec<f64>) -> f64 {
```

**Parameters:**

- `pc`: `&PowerCurve`
- `speeds`: `&Vec<f64>`
- `hours`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 216*

---

## wind_capacity_factor

```fides
pub fn wind_capacity_factor(aep: f64, rated: f64) -> f64 {
```

**Parameters:**

- `aep`: `f64`
- `rated`: `f64`

**Returns:** `f64`

*Defined at line 226*

---

## wind_shear_power

```fides
pub fn wind_shear_power(v_ref: f64, h_ref: f64, h: f64, alpha: f64) -> f64 {
```

**Parameters:**

- `v_ref`: `f64`
- `h_ref`: `f64`
- `h`: `f64`
- `alpha`: `f64`

**Returns:** `f64`

*Defined at line 232*

---

## wind_shear_log

```fides
pub fn wind_shear_log(v_ref: f64, h_ref: f64, h: f64, z0: f64) -> f64 {
```

**Parameters:**

- `v_ref`: `f64`
- `h_ref`: `f64`
- `h`: `f64`
- `z0`: `f64`

**Returns:** `f64`

*Defined at line 236*

---

## wind_shear_alpha

Power-law exponent from speeds measured at two heights.

```fides
pub fn wind_shear_alpha(v1: f64, h1: f64, v2: f64, h2: f64) -> f64 {
```

**Parameters:**

- `v1`: `f64`
- `h1`: `f64`
- `v2`: `f64`
- `h2`: `f64`

**Returns:** `f64`

*Defined at line 241*

---

## wind_jensen_deficit

Fractional velocity deficit on the wake axis at distance x behind a
rotor of diameter d with thrust coefficient ct and decay constant k
(about 0.075 onshore, 0.04-0.05 offshore).

```fides
pub fn wind_jensen_deficit(ct: f64, k: f64, d: f64, x: f64) -> f64 {
```

**Parameters:**

- `ct`: `f64`
- `k`: `f64`
- `d`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 250*

---

## wind_circle_overlap

Area of the intersection of circles of radii r1, r2 with centres d apart.

```fides
pub fn wind_circle_overlap(r1: f64, r2: f64, d: f64) -> f64 {
```

**Parameters:**

- `r1`: `f64`
- `r2`: `f64`
- `d`: `f64`

**Returns:** `f64`

*Defined at line 260*

---

## wind_jensen_rotor_deficit

Deficit seen by a rotor (same diameter) at distance x downstream and
lateral offset y: the axial deficit times the shaded fraction of its
swept area.

```fides
pub fn wind_jensen_rotor_deficit(ct: f64, k: f64, d: f64, x: f64, y: f64) -> f64 {
```

**Parameters:**

- `ct`: `f64`
- `k`: `f64`
- `d`: `f64`
- `x`: `f64`
- `y`: `f64`

**Returns:** `f64`

*Defined at line 279*

---

## wind_farm_speeds

Speeds at turbines (xs[i], ys[i]) for free stream u0 along +x, every
turbine with diameter d and thrust coefficient ct; wakes combined by
root-sum-square of deficits.

```fides
pub fn wind_farm_speeds(xs: &Vec<f64>, ys: &Vec<f64>, u0: f64, ct: f64, k: f64, d: f64) -> Vec<f64> {
```

**Parameters:**

- `xs`: `&Vec<f64>`
- `ys`: `&Vec<f64>`
- `u0`: `f64`
- `ct`: `f64`
- `k`: `f64`
- `d`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 292*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
