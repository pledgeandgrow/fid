[All modules](index.md)

# lp_solver -- Fides Documentation

## Contents

- [LP_LE:](#LP_LE:) (const)
- [LP_GE:](#LP_GE:) (const)
- [LP_EQ:](#LP_EQ:) (const)
- [LP_OPTIMAL:](#LP_OPTIMAL:) (const)
- [LP_INFEASIBLE:](#LP_INFEASIBLE:) (const)
- [LP_UNBOUNDED:](#LP_UNBOUNDED:) (const)
- [LP_ITER_LIMIT:](#LP_ITER_LIMIT:) (const)
- [LP_EPS:](#LP_EPS:) (const)
- [LP_INT_TOL:](#LP_INT_TOL:) (const)
- [LP_DEGENERATE_RUN:](#LP_DEGENERATE_RUN:) (const)
- [LpProblem](#LpProblem) (struct)
- [LpResult](#LpResult) (struct)
- [lp_new](#lp_new) (function)
- [lp_set_objective](#lp_set_objective) (function)
- [lp_set_integer](#lp_set_integer) (function)
- [lp_add_constraint](#lp_add_constraint) (function)
- [lp_add_sparse](#lp_add_sparse) (function)
- [lp_add_upper_bound](#lp_add_upper_bound) (function)
- [lp_row2](#lp_row2) (function)
- [lp_row3](#lp_row3) (function)
- [lp_row4](#lp_row4) (function)
- [lp_status_name](#lp_status_name) (function)
- [lp_solve](#lp_solve) (function)
- [lp_solve_mip](#lp_solve_mip) (function)

## LP_LE:

```fides
pub const LP_LE: i64 = 0;
```

*Defined at line 43*

---

## LP_GE:

```fides
pub const LP_GE: i64 = 1;
```

*Defined at line 44*

---

## LP_EQ:

```fides
pub const LP_EQ: i64 = 2;
```

*Defined at line 45*

---

## LP_OPTIMAL:

```fides
pub const LP_OPTIMAL: i64 = 0;
```

*Defined at line 47*

---

## LP_INFEASIBLE:

```fides
pub const LP_INFEASIBLE: i64 = 1;
```

*Defined at line 48*

---

## LP_UNBOUNDED:

```fides
pub const LP_UNBOUNDED: i64 = 2;
```

*Defined at line 49*

---

## LP_ITER_LIMIT:

```fides
pub const LP_ITER_LIMIT: i64 = 3;
```

*Defined at line 50*

---

## LP_EPS:

```fides
pub const LP_EPS: f64 = 0.000000001;
```

*Defined at line 52*

---

## LP_INT_TOL:

```fides
pub const LP_INT_TOL: f64 = 0.000001;
```

*Defined at line 53*

---

## LP_DEGENERATE_RUN:

Degenerate pivots in a row after which pricing switches to Bland's rule.

```fides
pub const LP_DEGENERATE_RUN: i64 = 30;
```

*Defined at line 55*

---

## LpProblem

A linear program. `rows[i]` holds the coefficients of constraint i (one
per variable), `rel[i]` its relation (LP_LE, LP_GE, LP_EQ) and `rhs[i]`
its right-hand side.

```fides
pub struct LpProblem {
```

**Fields:**

| Name | Type |
|------|------|
| `nvars` | `i64` |
| `maximize` | `bool` |
| `c` | `Vec<f64>` |
| `rows` | `Vec<Vec<f64>>` |
| `rel` | `Vec<i64>` |
| `rhs` | `Vec<f64>` |
| `is_int` | `Vec<bool>` |
| `max_iter` | `i64` |
| `bland_only` | `bool` |

*Defined at line 60*

---

## LpResult

The solution. `x` and `duals` are filled when `status` is LP_OPTIMAL;
`nodes` counts the branch-and-bound nodes solved (1 for a plain LP).

```fides
pub struct LpResult {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `x` | `Vec<f64>` |
| `objective` | `f64` |
| `duals` | `Vec<f64>` |
| `iterations` | `i64` |
| `nodes` | `i64` |

*Defined at line 74*

---

## lp_new

A problem with `nvars` variables, all objective coefficients 0.

```fides
pub fn lp_new(nvars: i64, maximize: bool) -> LpProblem {
```

**Parameters:**

- `nvars`: `i64`
- `maximize`: `bool`

**Returns:** `LpProblem`

*Defined at line 84*

---

## lp_set_objective

```fides
pub fn lp_set_objective(p: &mut LpProblem, j: i64, coef: f64) {
```

**Parameters:**

- `p`: `&mut LpProblem`
- `j`: `i64`
- `coef`: `f64`

*Defined at line 98*

---

## lp_set_integer

Mark variable j as integer (used by `lp_solve_mip`).

```fides
pub fn lp_set_integer(p: &mut LpProblem, j: i64) {
```

**Parameters:**

- `p`: `&mut LpProblem`
- `j`: `i64`

*Defined at line 103*

---

## lp_add_constraint

Add a dense constraint (one coefficient per variable); returns its index.

```fides
pub fn lp_add_constraint(p: &mut LpProblem, coeffs: Vec<f64>, rel: i64, rhs: f64) -> i64 {
```

**Parameters:**

- `p`: `&mut LpProblem`
- `coeffs`: `Vec<f64>`
- `rel`: `i64`
- `rhs`: `f64`

**Returns:** `i64`

*Defined at line 108*

---

## lp_add_sparse

Add a sparse constraint: `vals[k]` is the coefficient of variable
`idx[k]`, every other coefficient is 0. Returns the constraint index.

```fides
pub fn lp_add_sparse(p: &mut LpProblem, idx: Vec<i64>, vals: Vec<f64>, rel: i64, rhs: f64) -> i64 {
```

**Parameters:**

- `p`: `&mut LpProblem`
- `idx`: `Vec<i64>`
- `vals`: `Vec<f64>`
- `rel`: `i64`
- `rhs`: `f64`

**Returns:** `i64`

*Defined at line 120*

---

## lp_add_upper_bound

x_j <= ub as a constraint row.

```fides
pub fn lp_add_upper_bound(p: &mut LpProblem, j: i64, ub: f64) -> i64 {
```

**Parameters:**

- `p`: `&mut LpProblem`
- `j`: `i64`
- `ub`: `f64`

**Returns:** `i64`

*Defined at line 136*

---

## lp_row2

Convenience rows for small problems.

```fides
pub fn lp_row2(a: f64, b: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 145*

---

## lp_row3

```fides
pub fn lp_row3(a: f64, b: f64, c: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `c`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 152*

---

## lp_row4

```fides
pub fn lp_row4(a: f64, b: f64, c: f64, d: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `c`: `f64`
- `d`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 158*

---

## lp_status_name

```fides
pub fn lp_status_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 164*

---

## lp_solve

Solve the LP relaxation (integrality is ignored).

```fides
pub fn lp_solve(p: &LpProblem) -> LpResult {
```

**Parameters:**

- `p`: `&LpProblem`

**Returns:** `LpResult`

*Defined at line 333*

---

## lp_solve_mip

Solve with the integrality constraints of `lp_set_integer`. Without any
integer variable this is `lp_solve`. `duals` are those of the LP at the
best node (with its branching bounds), objective and `x` the best integer
solution; status LP_INFEASIBLE when no integer point is feasible.

```fides
pub fn lp_solve_mip(p: &LpProblem) -> LpResult {
```

**Parameters:**

- `p`: `&LpProblem`

**Returns:** `LpResult`

*Defined at line 551*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
