[All modules](index.md)

# kalman -- Fides Documentation

## Contents

- [Kf](#Kf) (struct)
- [kf_new](#kf_new) (function)
- [kf_set_control](#kf_set_control) (function)
- [kf_predict](#kf_predict) (function)
- [kf_predict_u](#kf_predict_u) (function)
- [kf_update](#kf_update) (function)
- [kf_cv_model](#kf_cv_model) (function)
- [Ekf](#Ekf) (struct)
- [ekf_new](#ekf_new) (function)
- [ekf_predict](#ekf_predict) (function)
- [ekf_update](#ekf_update) (function)
- [Ukf](#Ukf) (struct)
- [ukf_new](#ukf_new) (function)
- [ukf_sigma_points](#ukf_sigma_points) (function)
- [ukf_predict](#ukf_predict) (function)
- [ukf_update](#ukf_update) (function)

## Kf

A linear Kalman filter.

```fides
pub struct Kf {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `DMat` |
| `p` | `DMat` |
| `f` | `DMat` |
| `b` | `DMat` |
| `h` | `DMat` |
| `q` | `DMat` |
| `r` | `DMat` |
| `y` | `DMat` |
| `s` | `DMat` |
| `nis` | `f64` |

*Defined at line 41*

---

## kf_new

A filter with transition f, measurement h, noises q and r, initial
state x0 (n x 1) and covariance p0.

```fides
pub fn kf_new(f: DMat, h: DMat, q: DMat, r: DMat, x0: DMat, p0: DMat) -> Kf {
```

**Parameters:**

- `f`: `DMat`
- `h`: `DMat`
- `q`: `DMat`
- `r`: `DMat`
- `x0`: `DMat`
- `p0`: `DMat`

**Returns:** `Kf`

*Defined at line 58*

---

## kf_set_control

Set the control input matrix B (n x k).

```fides
pub fn kf_set_control(kf: &mut Kf, b: DMat) {
```

**Parameters:**

- `kf`: `&mut Kf`
- `b`: `DMat`

*Defined at line 67*

---

## kf_predict

x = F x, P = F P F' + Q.

```fides
pub fn kf_predict(kf: &mut Kf) {
```

**Parameters:**

- `kf`: `&mut Kf`

*Defined at line 72*

---

## kf_predict_u

x = F x + B u, P = F P F' + Q.

```fides
pub fn kf_predict_u(kf: &mut Kf, u: &DMat) {
```

**Parameters:**

- `kf`: `&mut Kf`
- `u`: `&DMat`

*Defined at line 82*

---

## kf_update

Fold in measurement z (m x 1). Returns the NIS of the innovation.

```fides
pub fn kf_update(kf: &mut Kf, z: &DMat) -> Result<f64, str> {
```

**Parameters:**

- `kf`: `&mut Kf`
- `z`: `&DMat`

**Returns:** `Result<f64, str>`

*Defined at line 141*

---

## kf_cv_model

The constant-velocity model of one axis with step dt and white
acceleration noise of spectral density qc: F = [1 dt; 0 1],
Q = qc [dt^3/3 dt^2/2; dt^2/2 dt].

```fides
pub fn kf_cv_model(dt: f64, qc: f64) -> Vec<DMat> {
```

**Parameters:**

- `dt`: `f64`
- `qc`: `f64`

**Returns:** `Vec<DMat>`

*Defined at line 162*

---

## Ekf

An extended Kalman filter: state, covariance and the noises.

```fides
pub struct Ekf {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `DMat` |
| `p` | `DMat` |
| `q` | `DMat` |
| `r` | `DMat` |
| `y` | `DMat` |
| `nis` | `f64` |

*Defined at line 190*

---

## ekf_new

A filter at x0 with covariance p0, process noise q, measurement noise r.

```fides
pub fn ekf_new(x0: DMat, p0: DMat, q: DMat, r: DMat) -> Ekf {
```

**Parameters:**

- `x0`: `DMat`
- `p0`: `DMat`
- `q`: `DMat`
- `r`: `DMat`

**Returns:** `Ekf`

*Defined at line 200*

---

## ekf_predict

x = f(x, u), P = F P F' + Q with F = fjac(x, u) at the old x.

```fides
pub fn ekf_predict(e: &mut Ekf, f: fn(Vec<f64>, Vec<f64>) -> Vec<f64>, fjac: fn(Vec<f64>, Vec<f64>) -> Vec<f64>, u: &Vec<f64>) {
```

**Parameters:**

- `e`: `&mut Ekf`
- `f`: `fn(Vec<f64>`
- `fjac`: `fn(Vec<f64>`
- `u`: `&Vec<f64>`

*Defined at line 207*

---

## ekf_update

Fold in z with the measurement model h(x) and its Jacobian hjac(x).
Returns the NIS.

```fides
pub fn ekf_update(e: &mut Ekf, z: &DMat, h: fn(Vec<f64>) -> Vec<f64>, hjac: fn(Vec<f64>) -> Vec<f64>) -> Result<f64, str> {
```

**Parameters:**

- `e`: `&mut Ekf`
- `z`: `&DMat`
- `h`: `fn(Vec<f64>) -> Vec<f64>`
- `hjac`: `fn(Vec<f64>) -> Vec<f64>`

**Returns:** `Result<f64, str>`

*Defined at line 221*

---

## Ukf

An unscented Kalman filter with Merwe's scaled sigma points.

```fides
pub struct Ukf {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `DMat` |
| `p` | `DMat` |
| `q` | `DMat` |
| `r` | `DMat` |
| `alpha` | `f64` |
| `beta` | `f64` |
| `kappa` | `f64` |
| `wm` | `Vec<f64>` |
| `wc` | `Vec<f64>` |
| `lambda` | `f64` |
| `y` | `DMat` |
| `nis` | `f64` |

*Defined at line 244*

---

## ukf_new

A filter at x0 / p0 with noises q and r and sigma-point parameters
alpha (spread, e.g. 1e-3 .. 1), beta (2 for Gaussian priors), kappa
(usually 0 or 3 - n).

```fides
pub fn ukf_new(x0: DMat, p0: DMat, q: DMat, r: DMat, alpha: f64, beta: f64, kappa: f64) -> Ukf {
```

**Parameters:**

- `x0`: `DMat`
- `p0`: `DMat`
- `q`: `DMat`
- `r`: `DMat`
- `alpha`: `f64`
- `beta`: `f64`
- `kappa`: `f64`

**Returns:** `Ukf`

*Defined at line 263*

---

## ukf_sigma_points

The 2n + 1 sigma points of (x, p): x, then x + c_i and x - c_i for the
columns c_i of the Cholesky factor of (n + lambda) p.

```fides
pub fn ukf_sigma_points(x: &DMat, p: &DMat, lambda: f64) -> Result<Vec<DMat>, str> {
```

**Parameters:**

- `x`: `&DMat`
- `p`: `&DMat`
- `lambda`: `f64`

**Returns:** `Result<Vec<DMat>, str>`

*Defined at line 284*

---

## ukf_predict

Propagate the sigma points through f(x, u): x = mean, P = cov + Q.

```fides
pub fn ukf_predict(k: &mut Ukf, f: fn(Vec<f64>, Vec<f64>) -> Vec<f64>, u: &Vec<f64>) -> Result<i64, str> {
```

**Parameters:**

- `k`: `&mut Ukf`
- `f`: `fn(Vec<f64>`
- `u`: `&Vec<f64>`

**Returns:** `Result<i64, str>`

*Defined at line 340*

---

## ukf_update

Fold in z with the measurement model h(x). Returns the NIS.

```fides
pub fn ukf_update(k: &mut Ukf, z: &DMat, h: fn(Vec<f64>) -> Vec<f64>) -> Result<f64, str> {
```

**Parameters:**

- `k`: `&mut Ukf`
- `z`: `&DMat`
- `h`: `fn(Vec<f64>) -> Vec<f64>`

**Returns:** `Result<f64, str>`

*Defined at line 362*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
