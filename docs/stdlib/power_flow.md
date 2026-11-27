[All modules](index.md)

# power_flow -- Fides Documentation

## Contents

- [PF_SLACK:](#PF_SLACK:) (const)
- [PF_PV:](#PF_PV:) (const)
- [PF_PQ:](#PF_PQ:) (const)
- [PfBus](#PfBus) (struct)
- [PfBranch](#PfBranch) (struct)
- [PfNetwork](#PfNetwork) (struct)
- [PfYbus](#PfYbus) (struct)
- [PfResult](#PfResult) (struct)
- [PfFlow](#PfFlow) (struct)
- [pf_network](#pf_network) (function)
- [pf_slack_bus](#pf_slack_bus) (function)
- [pf_pv_bus](#pf_pv_bus) (function)
- [pf_pq_bus](#pf_pq_bus) (function)
- [pf_add_bus](#pf_add_bus) (function)
- [pf_add_line](#pf_add_line) (function)
- [pf_add_transformer](#pf_add_transformer) (function)
- [pf_set_q_limits](#pf_set_q_limits) (function)
- [pf_set_shunt](#pf_set_shunt) (function)
- [pf_ybus_get](#pf_ybus_get) (function)
- [pf_ybus](#pf_ybus) (function)
- [PfLu](#PfLu) (struct)
- [pf_lu](#pf_lu) (function)
- [pf_lu_solve](#pf_lu_solve) (function)
- [pf_injections](#pf_injections) (function)
- [pf_newton](#pf_newton) (function)
- [pf_newton_qlim](#pf_newton_qlim) (function)
- [pf_gauss_seidel](#pf_gauss_seidel) (function)
- [pf_fast_decoupled](#pf_fast_decoupled) (function)
- [pf_dc](#pf_dc) (function)
- [pf_dc_flow](#pf_dc_flow) (function)
- [pf_branch_flow](#pf_branch_flow) (function)
- [pf_total_losses](#pf_total_losses) (function)
- [pf_generation](#pf_generation) (function)
- [pf_deg](#pf_deg) (function)

## PF_SLACK:

```fides
pub const PF_SLACK: i64 = 0;
```

*Defined at line 34*

---

## PF_PV:

```fides
pub const PF_PV: i64 = 1;
```

*Defined at line 35*

---

## PF_PQ:

```fides
pub const PF_PQ: i64 = 2;
```

*Defined at line 36*

---

## PfBus

A bus. `v`/`theta` are the specified (slack, PV) or starting values;
`pg`/`qg` generation, `pl`/`ql` load, `gsh`/`bsh` shunt admittance (pu).

```fides
pub struct PfBus {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `v` | `f64` |
| `theta` | `f64` |
| `pg` | `f64` |
| `qg` | `f64` |
| `pl` | `f64` |
| `ql` | `f64` |
| `qmin` | `f64` |
| `qmax` | `f64` |
| `gsh` | `f64` |
| `bsh` | `f64` |

*Defined at line 40*

---

## PfBranch

A line or transformer between bus indexes `from` and `to`.

```fides
pub struct PfBranch {
```

**Fields:**

| Name | Type |
|------|------|
| `from` | `i64` |
| `to` | `i64` |
| `r` | `f64` |
| `x` | `f64` |
| `b` | `f64` |
| `tap` | `f64` |
| `shift` | `f64` |

*Defined at line 55*

---

## PfNetwork

```fides
pub struct PfNetwork {
```

**Fields:**

| Name | Type |
|------|------|
| `base_mva` | `f64` |
| `buses` | `Vec<PfBus>` |
| `branches` | `Vec<PfBranch>` |

*Defined at line 65*

---

## PfYbus

Dense complex matrix as G + jB, row-major n x n.

```fides
pub struct PfYbus {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `g` | `Vec<f64>` |
| `b` | `Vec<f64>` |

*Defined at line 72*

---

## PfResult

```fides
pub struct PfResult {
```

**Fields:**

| Name | Type |
|------|------|
| `converged` | `bool` |
| `iterations` | `i64` |
| `max_mismatch` | `f64` |
| `v` | `Vec<f64>` |
| `theta` | `Vec<f64>` |
| `p` | `Vec<f64>` |
| `q` | `Vec<f64>` |

*Defined at line 78*

---

## PfFlow

```fides
pub struct PfFlow {
```

**Fields:**

| Name | Type |
|------|------|
| `p_from` | `f64` |
| `q_from` | `f64` |
| `p_to` | `f64` |
| `q_to` | `f64` |
| `p_loss` | `f64` |
| `q_loss` | `f64` |

*Defined at line 88*

---

## pf_network

```fides
pub fn pf_network(base_mva: f64) -> PfNetwork {
```

**Parameters:**

- `base_mva`: `f64`

**Returns:** `PfNetwork`

*Defined at line 99*

---

## pf_slack_bus

```fides
pub fn pf_slack_bus(v: f64, theta: f64) -> PfBus {
```

**Parameters:**

- `v`: `f64`
- `theta`: `f64`

**Returns:** `PfBus`

*Defined at line 105*

---

## pf_pv_bus

A generator bus holding |V| = v and generation pg (pu), with load pl/ql.

```fides
pub fn pf_pv_bus(v: f64, pg: f64, pl: f64, ql: f64) -> PfBus {
```

**Parameters:**

- `v`: `f64`
- `pg`: `f64`
- `pl`: `f64`
- `ql`: `f64`

**Returns:** `PfBus`

*Defined at line 112*

---

## pf_pq_bus

A load bus taking pl + j ql (pu).

```fides
pub fn pf_pq_bus(pl: f64, ql: f64) -> PfBus {
```

**Parameters:**

- `pl`: `f64`
- `ql`: `f64`

**Returns:** `PfBus`

*Defined at line 119*

---

## pf_add_bus

```fides
pub fn pf_add_bus(net: &mut PfNetwork, bus: PfBus) -> i64 {
```

**Parameters:**

- `net`: `&mut PfNetwork`
- `bus`: `PfBus`

**Returns:** `i64`

*Defined at line 125*

---

## pf_add_line

A line: series r + jx, total charging susceptance b.

```fides
pub fn pf_add_line(net: &mut PfNetwork, from: i64, to: i64, r: f64, x: f64, b: f64) -> i64 {
```

**Parameters:**

- `net`: `&mut PfNetwork`
- `from`: `i64`
- `to`: `i64`
- `r`: `f64`
- `x`: `f64`
- `b`: `f64`

**Returns:** `i64`

*Defined at line 131*

---

## pf_add_transformer

A transformer with off-nominal tap ratio `tap` and phase shift `shift`
(radians) at the from end.

```fides
pub fn pf_add_transformer(net: &mut PfNetwork, from: i64, to: i64, r: f64, x: f64, tap: f64, shift: f64) -> i64 {
```

**Parameters:**

- `net`: `&mut PfNetwork`
- `from`: `i64`
- `to`: `i64`
- `r`: `f64`
- `x`: `f64`
- `tap`: `f64`
- `shift`: `f64`

**Returns:** `i64`

*Defined at line 140*

---

## pf_set_q_limits

```fides
pub fn pf_set_q_limits(net: &mut PfNetwork, bus: i64, qmin: f64, qmax: f64) {
```

**Parameters:**

- `net`: `&mut PfNetwork`
- `bus`: `i64`
- `qmin`: `f64`
- `qmax`: `f64`

*Defined at line 147*

---

## pf_set_shunt

```fides
pub fn pf_set_shunt(net: &mut PfNetwork, bus: i64, g: f64, b: f64) {
```

**Parameters:**

- `net`: `&mut PfNetwork`
- `bus`: `i64`
- `g`: `f64`
- `b`: `f64`

*Defined at line 154*

---

## pf_ybus_get

```fides
pub fn pf_ybus_get(y: &PfYbus, i: i64, k: i64) -> Complex {
```

**Parameters:**

- `y`: `&PfYbus`
- `i`: `i64`
- `k`: `i64`

**Returns:** `Complex`

*Defined at line 179*

---

## pf_ybus

```fides
pub fn pf_ybus(net: &PfNetwork) -> PfYbus {
```

**Parameters:**

- `net`: `&PfNetwork`

**Returns:** `PfYbus`

*Defined at line 196*

---

## PfLu

LU factors with partial pivoting of an n x n row-major matrix.

```fides
pub struct PfLu {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `a` | `Vec<f64>` |
| `perm` | `Vec<i64>` |
| `singular` | `bool` |

*Defined at line 223*

---

## pf_lu

```fides
pub fn pf_lu(a: Vec<f64>, n: i64) -> PfLu {
```

**Parameters:**

- `a`: `Vec<f64>`
- `n`: `i64`

**Returns:** `PfLu`

*Defined at line 230*

---

## pf_lu_solve

```fides
pub fn pf_lu_solve(lu: &PfLu, rhs: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `lu`: `&PfLu`
- `rhs`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 290*

---

## pf_injections

P and Q injected at every bus for the given voltages: [P..., Q...].

```fides
pub fn pf_injections(y: &PfYbus, v: &Vec<f64>, th: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `y`: `&PfYbus`
- `v`: `&Vec<f64>`
- `th`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 326*

---

## pf_newton

Newton-Raphson load flow to a maximum power mismatch `tol` (pu).

```fides
pub fn pf_newton(net: &PfNetwork, tol: f64, max_iter: i64) -> PfResult {
```

**Parameters:**

- `net`: `&PfNetwork`
- `tol`: `f64`
- `max_iter`: `i64`

**Returns:** `PfResult`

*Defined at line 529*

---

## pf_newton_qlim

Newton-Raphson with PV reactive limits: a PV bus whose generation would
leave [qmin, qmax] is held at the limit as a PQ bus and the flow is
solved again (at most n times). `held` (one per bus) is set to 1 or -1
for a bus held at its upper or lower limit.

```fides
pub fn pf_newton_qlim(net: &PfNetwork, tol: f64, max_iter: i64, held: &mut Vec<i64>) -> PfResult {
```

**Parameters:**

- `net`: `&PfNetwork`
- `tol`: `f64`
- `max_iter`: `i64`
- `held`: `&mut Vec<i64>`

**Returns:** `PfResult`

*Defined at line 542*

---

## pf_gauss_seidel

Gauss-Seidel load flow; `accel` (typically 1.4..1.6) accelerates the
PQ-bus updates. Converged when no voltage changes by more than `tol`.

```fides
pub fn pf_gauss_seidel(net: &PfNetwork, tol: f64, max_iter: i64, accel: f64) -> PfResult {
```

**Parameters:**

- `net`: `&PfNetwork`
- `tol`: `f64`
- `max_iter`: `i64`
- `accel`: `f64`

**Returns:** `PfResult`

*Defined at line 598*

---

## pf_fast_decoupled

```fides
pub fn pf_fast_decoupled(net: &PfNetwork, tol: f64, max_iter: i64) -> PfResult {
```

**Parameters:**

- `net`: `&PfNetwork`
- `tol`: `f64`
- `max_iter`: `i64`

**Returns:** `PfResult`

*Defined at line 671*

---

## pf_dc

Bus angles of the DC approximation (slack bus angle as given, |V| = 1,
r and charging ignored): returns theta per bus.

```fides
pub fn pf_dc(net: &PfNetwork) -> Vec<f64> {
```

**Parameters:**

- `net`: `&PfNetwork`

**Returns:** `Vec<f64>`

*Defined at line 793*

---

## pf_dc_flow

DC flow on branch k from the angles of `pf_dc`.

```fides
pub fn pf_dc_flow(net: &PfNetwork, th: &Vec<f64>, k: i64) -> f64 {
```

**Parameters:**

- `net`: `&PfNetwork`
- `th`: `&Vec<f64>`
- `k`: `i64`

**Returns:** `f64`

*Defined at line 864*

---

## pf_branch_flow

Flows at both ends of branch k and its losses (pu).

```fides
pub fn pf_branch_flow(net: &PfNetwork, r: &PfResult, k: i64) -> PfFlow {
```

**Parameters:**

- `net`: `&PfNetwork`
- `r`: `&PfResult`
- `k`: `i64`

**Returns:** `PfFlow`

*Defined at line 872*

---

## pf_total_losses

Total losses: the sum of all bus injections, [P, Q].

```fides
pub fn pf_total_losses(r: &PfResult) -> Complex {
```

**Parameters:**

- `r`: `&PfResult`

**Returns:** `Complex`

*Defined at line 887*

---

## pf_generation

Generation at bus i: injection plus load.

```fides
pub fn pf_generation(net: &PfNetwork, r: &PfResult, i: i64) -> Complex {
```

**Parameters:**

- `net`: `&PfNetwork`
- `r`: `&PfResult`
- `i`: `i64`

**Returns:** `Complex`

*Defined at line 900*

---

## pf_deg

```fides
pub fn pf_deg(rad: f64) -> f64 {
```

**Parameters:**

- `rad`: `f64`

**Returns:** `f64`

*Defined at line 905*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
