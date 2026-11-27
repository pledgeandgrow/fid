[All modules](index.md)

# forecast -- Fides Documentation

## Contents

- [FC_SES:](#FC_SES:) (const)
- [FC_HOLT:](#FC_HOLT:) (const)
- [FC_HW_ADD:](#FC_HW_ADD:) (const)
- [FC_HW_MUL:](#FC_HW_MUL:) (const)
- [FC_ARIMA:](#FC_ARIMA:) (const)
- [FcModel](#FcModel) (struct)
- [fc_model](#fc_model) (function)
- [fc_arima_model](#fc_arima_model) (function)
- [fc_ses_smooth](#fc_ses_smooth) (function)
- [fc_ses](#fc_ses) (function)
- [fc_one_step](#fc_one_step) (function)
- [fc_holt](#fc_holt) (function)
- [fc_holt_winters](#fc_holt_winters) (function)
- [fc_sse](#fc_sse) (function)
- [fc_fit](#fc_fit) (function)
- [fc_least_squares](#fc_least_squares) (function)
- [ArimaFit](#ArimaFit) (struct)
- [fc_diff](#fc_diff) (function)
- [fc_arima_fit](#fc_arima_fit) (function)
- [fc_arima_forecast](#fc_arima_forecast) (function)
- [fc_mae](#fc_mae) (function)
- [fc_rmse](#fc_rmse) (function)
- [fc_mape](#fc_mape) (function)
- [fc_smape](#fc_smape) (function)
- [fc_forecast](#fc_forecast) (function)
- [Backtest](#Backtest) (struct)
- [fc_backtest](#fc_backtest) (function)

## FC_SES:

```fides
pub const FC_SES: i64 = 0;
```

*Defined at line 29*

---

## FC_HOLT:

```fides
pub const FC_HOLT: i64 = 1;
```

*Defined at line 30*

---

## FC_HW_ADD:

```fides
pub const FC_HW_ADD: i64 = 2;
```

*Defined at line 31*

---

## FC_HW_MUL:

```fides
pub const FC_HW_MUL: i64 = 3;
```

*Defined at line 32*

---

## FC_ARIMA:

```fides
pub const FC_ARIMA: i64 = 4;
```

*Defined at line 33*

---

## FcModel

A model choice for backtesting: kind, smoothing parameters, season
length and ARIMA orders.

```fides
pub struct FcModel {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `alpha` | `f64` |
| `beta` | `f64` |
| `gamma` | `f64` |
| `season` | `i64` |
| `p` | `i64` |
| `d` | `i64` |
| `q` | `i64` |

*Defined at line 37*

---

## fc_model

```fides
pub fn fc_model(kind: i64, alpha: f64, beta: f64, gamma: f64, season: i64) -> FcModel {
```

**Parameters:**

- `kind`: `i64`
- `alpha`: `f64`
- `beta`: `f64`
- `gamma`: `f64`
- `season`: `i64`

**Returns:** `FcModel`

*Defined at line 48*

---

## fc_arima_model

```fides
pub fn fc_arima_model(p: i64, d: i64, q: i64) -> FcModel {
```

**Parameters:**

- `p`: `i64`
- `d`: `i64`
- `q`: `i64`

**Returns:** `FcModel`

*Defined at line 54*

---

## fc_ses_smooth

S_t for t = 0..n (one more than y: the last is the next forecast);
S_0 is undefined and set to y_0, S_1 = y_0.

```fides
pub fn fc_ses_smooth(y: &Vec<f64>, alpha: f64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `alpha`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 74*

---

## fc_ses

```fides
pub fn fc_ses(y: &Vec<f64>, alpha: f64, h: i64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `alpha`: `f64`
- `h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 86*

---

## fc_one_step

One-step-ahead forecasts f[t] of y[t] for t = 1..n-1 (f[0] = y[0]) of
a model; the in-sample errors are y - f.

```fides
pub fn fc_one_step(y: &Vec<f64>, m: &FcModel) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `m`: `&FcModel`

**Returns:** `Vec<f64>`

*Defined at line 93*

---

## fc_holt

```fides
pub fn fc_holt(y: &Vec<f64>, alpha: f64, beta: f64, h: i64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `alpha`: `f64`
- `beta`: `f64`
- `h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 142*

---

## fc_holt_winters

Holt-Winters forecasts (season length m, multiplicative when `mul`).

```fides
pub fn fc_holt_winters(y: &Vec<f64>, m: i64, alpha: f64, beta: f64, gamma: f64, mul: bool, h: i64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `m`: `i64`
- `alpha`: `f64`
- `beta`: `f64`
- `gamma`: `f64`
- `mul`: `bool`
- `h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 234*

---

## fc_sse

Sum of squared one-step errors after a warm-up of `skip` points.

```fides
pub fn fc_sse(y: &Vec<f64>, m: &FcModel, skip: i64) -> f64 {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `m`: `&FcModel`
- `skip`: `i64`

**Returns:** `f64`

*Defined at line 253*

---

## fc_fit

Fit alpha (and beta, gamma where the kind uses them) on a 0.2 grid, then
a 0.05 grid within 0.1 of the best and a 0.01 grid within 0.02 of that;
returns the fitted model (the starting guess 0.5/0.1/0.1 when nothing
beats it).

```fides
pub fn fc_fit(y: &Vec<f64>, kind: i64, season: i64) -> FcModel {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `kind`: `i64`
- `season`: `i64`

**Returns:** `FcModel`

*Defined at line 273*

---

## fc_least_squares

Solve min |X b - y| (X row-major, n rows, k columns) by the normal
equations with partially pivoted elimination.

```fides
pub fn fc_least_squares(x: &Vec<f64>, y: &Vec<f64>, n: i64, k: i64) -> Vec<f64> {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `y`: `&Vec<f64>`
- `n`: `i64`
- `k`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 345*

---

## ArimaFit

A fitted ARIMA(p, d, q): intercept c, ar[0..p), ma[0..q) of the
d-times differenced series, its residuals, and the series itself.

```fides
pub struct ArimaFit {
```

**Fields:**

| Name | Type |
|------|------|
| `p` | `i64` |
| `d` | `i64` |
| `q` | `i64` |
| `c` | `f64` |
| `ar` | `Vec<f64>` |
| `ma` | `Vec<f64>` |
| `resid` | `Vec<f64>` |
| `w` | `Vec<f64>` |
| `y` | `Vec<f64>` |
| `sigma2` | `f64` |

*Defined at line 428*

---

## fc_diff

First difference applied d times.

```fides
pub fn fc_diff(y: &Vec<f64>, d: i64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `d`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 442*

---

## fc_arima_fit

Fit ARIMA(p, d, q) by least squares (Hannan-Rissanen when q > 0).

```fides
pub fn fc_arima_fit(y: &Vec<f64>, p: i64, d: i64, q: i64) -> ArimaFit {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `p`: `i64`
- `d`: `i64`
- `q`: `i64`

**Returns:** `ArimaFit`

*Defined at line 521*

---

## fc_arima_forecast

h-step forecasts of the original series.

```fides
pub fn fc_arima_forecast(f: &ArimaFit, h: i64) -> Vec<f64> {
```

**Parameters:**

- `f`: `&ArimaFit`
- `h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 582*

---

## fc_mae

```fides
pub fn fc_mae(actual: &Vec<f64>, pred: &Vec<f64>) -> f64 {
```

**Parameters:**

- `actual`: `&Vec<f64>`
- `pred`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 638*

---

## fc_rmse

```fides
pub fn fc_rmse(actual: &Vec<f64>, pred: &Vec<f64>) -> f64 {
```

**Parameters:**

- `actual`: `&Vec<f64>`
- `pred`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 648*

---

## fc_mape

Mean absolute percentage error in percent (points with actual 0 skipped).

```fides
pub fn fc_mape(actual: &Vec<f64>, pred: &Vec<f64>) -> f64 {
```

**Parameters:**

- `actual`: `&Vec<f64>`
- `pred`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 660*

---

## fc_smape

Symmetric MAPE in percent: mean of 2|a - f| / (|a| + |f|).

```fides
pub fn fc_smape(actual: &Vec<f64>, pred: &Vec<f64>) -> f64 {
```

**Parameters:**

- `actual`: `&Vec<f64>`
- `pred`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 678*

---

## fc_forecast

Forecast h steps of a model fitted to y (parameters as given).

```fides
pub fn fc_forecast(y: &Vec<f64>, m: &FcModel, h: i64) -> Vec<f64> {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `m`: `&FcModel`
- `h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 694*

---

## Backtest

```fides
pub struct Backtest {
```

**Fields:**

| Name | Type |
|------|------|
| `actual` | `Vec<f64>` |
| `predicted` | `Vec<f64>` |
| `mae` | `f64` |
| `rmse` | `f64` |
| `mape` | `f64` |

*Defined at line 711*

---

## fc_backtest

Rolling-origin evaluation: for every origin t from `first` to n - h,
fit on y[0..t) and compare the h-th step ahead with y[t + h - 1].

```fides
pub fn fc_backtest(y: &Vec<f64>, m: &FcModel, first: i64, h: i64) -> Backtest {
```

**Parameters:**

- `y`: `&Vec<f64>`
- `m`: `&FcModel`
- `first`: `i64`
- `h`: `i64`

**Returns:** `Backtest`

*Defined at line 721*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
