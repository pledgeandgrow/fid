[All modules](index.md)

# lqr -- Fides Documentation

## Contents

- [DMat](#DMat) (struct)
- [dm_zeros](#dm_zeros) (function)
- [dm_identity](#dm_identity) (function)
- [dm_from](#dm_from) (function)
- [dm_parse](#dm_parse) (function)
- [dm_col_vec](#dm_col_vec) (function)
- [dm_diag](#dm_diag) (function)
- [dm_get](#dm_get) (function)
- [dm_set](#dm_set) (function)
- [dm_to_vec](#dm_to_vec) (function)
- [dm_col](#dm_col) (function)
- [dm_add](#dm_add) (function)
- [dm_sub](#dm_sub) (function)
- [dm_scale](#dm_scale) (function)
- [dm_mul](#dm_mul) (function)
- [dm_transpose](#dm_transpose) (function)
- [dm_symmetrize](#dm_symmetrize) (function)
- [dm_block](#dm_block) (function)
- [dm_set_block](#dm_set_block) (function)
- [dm_trace](#dm_trace) (function)
- [dm_max_abs](#dm_max_abs) (function)
- [dm_max_diff](#dm_max_diff) (function)
- [dm_norm](#dm_norm) (function)
- [dm_solve](#dm_solve) (function)
- [dm_inverse](#dm_inverse) (function)
- [dm_det](#dm_det) (function)
- [dm_cholesky](#dm_cholesky) (function)
- [dm_expm](#dm_expm) (function)
- [dm_to_str](#dm_to_str) (function)
- [DiscreteSys](#DiscreteSys) (struct)
- [lqr_c2d](#lqr_c2d) (function)
- [LqrResult](#LqrResult) (struct)
- [lqr_dare](#lqr_dare) (function)
- [lqr_dlqr](#lqr_dlqr) (function)
- [lqr_closed_loop](#lqr_closed_loop) (function)
- [lqr_simulate](#lqr_simulate) (function)
- [lqr_cost](#lqr_cost) (function)

## DMat

A rows x cols matrix, row-major.

```fides
pub struct DMat {
```

**Fields:**

| Name | Type |
|------|------|
| `rows` | `i64` |
| `cols` | `i64` |
| `data` | `Vec<f64>` |

*Defined at line 40*

---

## dm_zeros

A rows x cols matrix of zeros.

```fides
pub fn dm_zeros(rows: i64, cols: i64) -> DMat {
```

**Parameters:**

- `rows`: `i64`
- `cols`: `i64`

**Returns:** `DMat`

*Defined at line 47*

---

## dm_identity

The n x n identity.

```fides
pub fn dm_identity(n: i64) -> DMat {
```

**Parameters:**

- `n`: `i64`

**Returns:** `DMat`

*Defined at line 60*

---

## dm_from

A matrix from row-major values (missing values are 0, extra ignored).

```fides
pub fn dm_from(rows: i64, cols: i64, vals: Vec<f64>) -> DMat {
```

**Parameters:**

- `rows`: `i64`
- `cols`: `i64`
- `vals`: `Vec<f64>`

**Returns:** `DMat`

*Defined at line 71*

---

## dm_parse

A matrix from text: rows separated by ';', entries by spaces or commas
("1 0.1; 0 1"). Short rows are padded with zeros.

```fides
pub fn dm_parse(text: str) -> DMat {
```

**Parameters:**

- `text`: `str`

**Returns:** `DMat`

*Defined at line 83*

---

## dm_col_vec

A column vector (n x 1) of `vals`.

```fides
pub fn dm_col_vec(vals: Vec<f64>) -> DMat {
```

**Parameters:**

- `vals`: `Vec<f64>`

**Returns:** `DMat`

*Defined at line 126*

---

## dm_diag

A square diagonal matrix of `vals`.

```fides
pub fn dm_diag(vals: Vec<f64>) -> DMat {
```

**Parameters:**

- `vals`: `Vec<f64>`

**Returns:** `DMat`

*Defined at line 131*

---

## dm_get

Entry (i, j).

```fides
pub fn dm_get(m: &DMat, i: i64, j: i64) -> f64 {
```

**Parameters:**

- `m`: `&DMat`
- `i`: `i64`
- `j`: `i64`

**Returns:** `f64`

*Defined at line 143*

---

## dm_set

Set entry (i, j).

```fides
pub fn dm_set(m: &mut DMat, i: i64, j: i64, v: f64) {
```

**Parameters:**

- `m`: `&mut DMat`
- `i`: `i64`
- `j`: `i64`
- `v`: `f64`

*Defined at line 148*

---

## dm_to_vec

All entries, row-major (for a column vector: its elements).

```fides
pub fn dm_to_vec(m: &DMat) -> Vec<f64> {
```

**Parameters:**

- `m`: `&DMat`

**Returns:** `Vec<f64>`

*Defined at line 153*

---

## dm_col

Column j as a Vec.

```fides
pub fn dm_col(m: &DMat, j: i64) -> Vec<f64> {
```

**Parameters:**

- `m`: `&DMat`
- `j`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 164*

---

## dm_add

a + b (same shape).

```fides
pub fn dm_add(a: &DMat, b: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`

**Returns:** `DMat`

*Defined at line 175*

---

## dm_sub

a - b (same shape).

```fides
pub fn dm_sub(a: &DMat, b: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`

**Returns:** `DMat`

*Defined at line 186*

---

## dm_scale

s * a.

```fides
pub fn dm_scale(a: &DMat, s: f64) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `s`: `f64`

**Returns:** `DMat`

*Defined at line 197*

---

## dm_mul

a * b (a.cols == b.rows).

```fides
pub fn dm_mul(a: &DMat, b: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`

**Returns:** `DMat`

*Defined at line 208*

---

## dm_transpose

The transpose.

```fides
pub fn dm_transpose(a: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `DMat`

*Defined at line 232*

---

## dm_symmetrize

(m + m') / 2.

```fides
pub fn dm_symmetrize(a: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `DMat`

*Defined at line 247*

---

## dm_block

The nr x nc block of a at (r0, c0).

```fides
pub fn dm_block(a: &DMat, r0: i64, c0: i64, nr: i64, nc: i64) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `r0`: `i64`
- `c0`: `i64`
- `nr`: `i64`
- `nc`: `i64`

**Returns:** `DMat`

*Defined at line 253*

---

## dm_set_block

Copy b into a at (r0, c0).

```fides
pub fn dm_set_block(a: &mut DMat, r0: i64, c0: i64, b: &DMat) {
```

**Parameters:**

- `a`: `&mut DMat`
- `r0`: `i64`
- `c0`: `i64`
- `b`: `&DMat`

*Defined at line 268*

---

## dm_trace

The trace.

```fides
pub fn dm_trace(a: &DMat) -> f64 {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `f64`

*Defined at line 281*

---

## dm_max_abs

The largest absolute entry.

```fides
pub fn dm_max_abs(a: &DMat) -> f64 {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `f64`

*Defined at line 292*

---

## dm_max_diff

The largest absolute difference between entries of a and b.

```fides
pub fn dm_max_diff(a: &DMat, b: &DMat) -> f64 {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`

**Returns:** `f64`

*Defined at line 303*

---

## dm_norm

The Euclidean norm of all entries (Frobenius).

```fides
pub fn dm_norm(a: &DMat) -> f64 {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `f64`

*Defined at line 309*

---

## dm_solve

The solution X of A X = B (A square), by Gaussian elimination with
partial pivoting. Err when A is singular (a pivot below 1e-14 times the
largest entry).

```fides
pub fn dm_solve(a: &DMat, b: &DMat) -> Result<DMat, str> {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`

**Returns:** `Result<DMat, str>`

*Defined at line 322*

---

## dm_inverse

The inverse of a square matrix; Err when singular.

```fides
pub fn dm_inverse(a: &DMat) -> Result<DMat, str> {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `Result<DMat, str>`

*Defined at line 404*

---

## dm_det

The determinant (Gaussian elimination with partial pivoting).

```fides
pub fn dm_det(a: &DMat) -> f64 {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `f64`

*Defined at line 410*

---

## dm_cholesky

The lower-triangular L with L L' = a, for a symmetric positive definite
a. Err when a is not positive definite.

```fides
pub fn dm_cholesky(a: &DMat) -> Result<DMat, str> {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `Result<DMat, str>`

*Defined at line 458*

---

## dm_expm

exp(a) for a square matrix: a is scaled by 2^-s until its norm is below
0.5, the Taylor series is summed to 20 terms (or until a term is
negligible), then the result is squared s times.

```fides
pub fn dm_expm(a: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`

**Returns:** `DMat`

*Defined at line 493*

---

## dm_to_str

The matrix as "[a b; c d]" with `dp` decimals.

```fides
pub fn dm_to_str(a: &DMat, dp: i64) -> str {
```

**Parameters:**

- `a`: `&DMat`
- `dp`: `i64`

**Returns:** `str`

*Defined at line 524*

---

## DiscreteSys

A discrete-time model x[k+1] = ad x[k] + bd u[k].

```fides
pub struct DiscreteSys {
```

**Fields:**

| Name | Type |
|------|------|
| `ad` | `DMat` |
| `bd` | `DMat` |
| `dt` | `f64` |

*Defined at line 547*

---

## lqr_c2d

Zero-order-hold discretization of x' = A x + B u with step dt:
Ad = exp(A dt), Bd = (integral_0^dt exp(A s) ds) B, both read off
exp([[A, B], [0, 0]] dt).

```fides
pub fn lqr_c2d(a: &DMat, b: &DMat, dt: f64) -> DiscreteSys {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`
- `dt`: `f64`

**Returns:** `DiscreteSys`

*Defined at line 556*

---

## LqrResult

The result of a discrete LQR design.

```fides
pub struct LqrResult {
```

**Fields:**

| Name | Type |
|------|------|
| `k` | `DMat` |
| `p` | `DMat` |
| `iterations` | `i64` |

*Defined at line 569*

---

## lqr_dare

The stabilizing solution P of the discrete algebraic Riccati equation
P = Q + A'PA - A'PB (R + B'PB)^-1 B'PA, by fixed-point iteration from
P = Q. Stops when no entry changes by more than tol (relative to the
largest entry); Err after max_iter iterations or on a singular R + B'PB.

```fides
pub fn lqr_dare(a: &DMat, b: &DMat, q: &DMat, r: &DMat, max_iter: i64, tol: f64) -> Result<LqrResult, str> {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`
- `q`: `&DMat`
- `r`: `&DMat`
- `max_iter`: `i64`
- `tol`: `f64`

**Returns:** `Result<LqrResult, str>`

*Defined at line 581*

---

## lqr_dlqr

The discrete LQR gain K (u = -K x) minimizing sum x'Qx + u'Ru for
x[k+1] = A x[k] + B u[k] (10000 iterations, tolerance 1e-12).

```fides
pub fn lqr_dlqr(a: &DMat, b: &DMat, q: &DMat, r: &DMat) -> Result<LqrResult, str> {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`
- `q`: `&DMat`
- `r`: `&DMat`

**Returns:** `Result<LqrResult, str>`

*Defined at line 629*

---

## lqr_closed_loop

The closed-loop matrix A - B K.

```fides
pub fn lqr_closed_loop(a: &DMat, b: &DMat, k: &DMat) -> DMat {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`
- `k`: `&DMat`

**Returns:** `DMat`

*Defined at line 634*

---

## lqr_simulate

The states x[0..=steps] of x[k+1] = (A - BK) x[k] from x0 (a column).

```fides
pub fn lqr_simulate(a: &DMat, b: &DMat, k: &DMat, x0: &DMat, steps: i64) -> Vec<DMat> {
```

**Parameters:**

- `a`: `&DMat`
- `b`: `&DMat`
- `k`: `&DMat`
- `x0`: `&DMat`
- `steps`: `i64`

**Returns:** `Vec<DMat>`

*Defined at line 640*

---

## lqr_cost

The cost sum_k x'Qx + u'Ru (u = -Kx) over a simulated trajectory.

```fides
pub fn lqr_cost(traj: &Vec<DMat>, k: &DMat, q: &DMat, r: &DMat) -> f64 {
```

**Parameters:**

- `traj`: `&Vec<DMat>`
- `k`: `&DMat`
- `q`: `&DMat`
- `r`: `&DMat`

**Returns:** `f64`

*Defined at line 655*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
