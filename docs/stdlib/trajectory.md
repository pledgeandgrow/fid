[All modules](index.md)

# trajectory -- Fides Documentation

## Contents

- [TrajState](#TrajState) (struct)
- [TrajPoly](#TrajPoly) (struct)
- [traj_cubic](#traj_cubic) (function)
- [traj_quintic](#traj_quintic) (function)
- [traj_poly_eval](#traj_poly_eval) (function)
- [traj_poly_jerk](#traj_poly_jerk) (function)
- [TrajSpline](#TrajSpline) (struct)
- [traj_tridiag](#traj_tridiag) (function)
- [traj_spline_natural](#traj_spline_natural) (function)
- [traj_spline_clamped](#traj_spline_clamped) (function)
- [traj_spline_eval](#traj_spline_eval) (function)
- [TrajTrapezoid](#TrajTrapezoid) (struct)
- [traj_trapezoid](#traj_trapezoid) (function)
- [traj_trapezoid_eval](#traj_trapezoid_eval) (function)
- [TrajSCurve](#TrajSCurve) (struct)
- [traj_scurve](#traj_scurve) (function)
- [traj_scurve_eval](#traj_scurve_eval) (function)
- [traj_times](#traj_times) (function)
- [traj_sample_poly](#traj_sample_poly) (function)
- [traj_sample_spline](#traj_sample_spline) (function)
- [traj_sample_trapezoid](#traj_sample_trapezoid) (function)
- [traj_sample_scurve](#traj_sample_scurve) (function)

## TrajState

Position, velocity and acceleration at a time.

```fides
pub struct TrajState {
```

**Fields:**

| Name | Type |
|------|------|
| `t` | `f64` |
| `p` | `f64` |
| `v` | `f64` |
| `a` | `f64` |

*Defined at line 38*

---

## TrajPoly

p(t) = sum c[k] (t - t0)^k for t in [0, duration] (t measured from the
start of the segment).

```fides
pub struct TrajPoly {
```

**Fields:**

| Name | Type |
|------|------|
| `c` | `Vec<f64>` |
| `duration` | `f64` |

*Defined at line 49*

---

## traj_cubic

The cubic with p(0) = p0, p'(0) = v0, p(T) = p1, p'(T) = v1.

```fides
pub fn traj_cubic(p0: f64, v0: f64, p1: f64, v1: f64, t: f64) -> TrajPoly {
```

**Parameters:**

- `p0`: `f64`
- `v0`: `f64`
- `p1`: `f64`
- `v1`: `f64`
- `t`: `f64`

**Returns:** `TrajPoly`

*Defined at line 55*

---

## traj_quintic

The quintic with position, velocity and acceleration (p0, v0, a0) at
t = 0 and (p1, v1, a1) at t = T.

```fides
pub fn traj_quintic(p0: f64, v0: f64, a0: f64, p1: f64, v1: f64, a1: f64, t: f64) -> TrajPoly {
```

**Parameters:**

- `p0`: `f64`
- `v0`: `f64`
- `a0`: `f64`
- `p1`: `f64`
- `v1`: `f64`
- `a1`: `f64`
- `t`: `f64`

**Returns:** `TrajPoly`

*Defined at line 68*

---

## traj_poly_eval

The state of a polynomial at time t (clamped to [0, duration]; outside
it the velocity and acceleration are those of the nearest end).

```fides
pub fn traj_poly_eval(p: &TrajPoly, tin: f64) -> TrajState {
```

**Parameters:**

- `p`: `&TrajPoly`
- `tin`: `f64`

**Returns:** `TrajState`

*Defined at line 88*

---

## traj_poly_jerk

The jerk (third derivative) of a polynomial at time t.

```fides
pub fn traj_poly_jerk(p: &TrajPoly, t: f64) -> f64 {
```

**Parameters:**

- `p`: `&TrajPoly`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 113*

---

## TrajSpline

A cubic spline: knots t (increasing), values y, second derivatives m.

```fides
pub struct TrajSpline {
```

**Fields:**

| Name | Type |
|------|------|
| `t` | `Vec<f64>` |
| `y` | `Vec<f64>` |
| `m` | `Vec<f64>` |

*Defined at line 129*

---

## traj_tridiag

Solve the tridiagonal system sub[i] x[i-1] + diag[i] x[i] + sup[i]
x[i+1] = rhs[i] (Thomas algorithm).

```fides
pub fn traj_tridiag(sub: &Vec<f64>, diag: &Vec<f64>, sup: &Vec<f64>, rhs: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `sub`: `&Vec<f64>`
- `diag`: `&Vec<f64>`
- `sup`: `&Vec<f64>`
- `rhs`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 137*

---

## traj_spline_natural

The natural cubic spline through (t[i], y[i]).

```fides
pub fn traj_spline_natural(t: &Vec<f64>, y: &Vec<f64>) -> TrajSpline {
```

**Parameters:**

- `t`: `&Vec<f64>`
- `y`: `&Vec<f64>`

**Returns:** `TrajSpline`

*Defined at line 228*

---

## traj_spline_clamped

The cubic spline through (t[i], y[i]) with velocity v0 at the first
knot and vn at the last.

```fides
pub fn traj_spline_clamped(t: &Vec<f64>, y: &Vec<f64>, v0: f64, vn: f64) -> TrajSpline {
```

**Parameters:**

- `t`: `&Vec<f64>`
- `y`: `&Vec<f64>`
- `v0`: `f64`
- `vn`: `f64`

**Returns:** `TrajSpline`

*Defined at line 234*

---

## traj_spline_eval

The spline's state at time t (clamped to the knot range).

```fides
pub fn traj_spline_eval(s: &TrajSpline, tin: f64) -> TrajState {
```

**Parameters:**

- `s`: `&TrajSpline`
- `tin`: `f64`

**Returns:** `TrajState`

*Defined at line 239*

---

## TrajTrapezoid

A rest-to-rest move of `dist` (signed) from p0 with peak speed vpeak:
accelerate for ta, cruise for tv, decelerate for ta.

```fides
pub struct TrajTrapezoid {
```

**Fields:**

| Name | Type |
|------|------|
| `p0` | `f64` |
| `dist` | `f64` |
| `vpeak` | `f64` |
| `amax` | `f64` |
| `ta` | `f64` |
| `tv` | `f64` |
| `total` | `f64` |

*Defined at line 278*

---

## traj_trapezoid

The fastest move from p0 to p1 with |v| <= vmax and |a| <= amax.

```fides
pub fn traj_trapezoid(p0: f64, p1: f64, vmax: f64, amax: f64) -> TrajTrapezoid {
```

**Parameters:**

- `p0`: `f64`
- `p1`: `f64`
- `vmax`: `f64`
- `amax`: `f64`

**Returns:** `TrajTrapezoid`

*Defined at line 289*

---

## traj_trapezoid_eval

The state of a trapezoidal move at time t.

```fides
pub fn traj_trapezoid_eval(tr: &TrajTrapezoid, t: f64) -> TrajState {
```

**Parameters:**

- `tr`: `&TrajTrapezoid`
- `t`: `f64`

**Returns:** `TrajState`

*Defined at line 307*

---

## TrajSCurve

A rest-to-rest jerk-limited move: seven segments of constant jerk.

```fides
pub struct TrajSCurve {
```

**Fields:**

| Name | Type |
|------|------|
| `p0` | `f64` |
| `dist` | `f64` |
| `jmax` | `f64` |
| `tj` | `f64` |
| `ta` | `f64` |
| `tv` | `f64` |
| `vpeak` | `f64` |
| `apeak` | `f64` |
| `total` | `f64` |
| `durs` | `Vec<f64>` |
| `jerks` | `Vec<f64>` |

*Defined at line 341*

---

## traj_scurve

The fastest move from p0 to p1 with |v| <= vmax, |a| <= amax and
|jerk| <= jmax, starting and ending at rest.

```fides
pub fn traj_scurve(p0: f64, p1: f64, vmax: f64, amax: f64, jmax: f64) -> TrajSCurve {
```

**Parameters:**

- `p0`: `f64`
- `p1`: `f64`
- `vmax`: `f64`
- `amax`: `f64`
- `jmax`: `f64`

**Returns:** `TrajSCurve`

*Defined at line 374*

---

## traj_scurve_eval

The state of an S-curve move at time t (the segments integrated in
closed form).

```fides
pub fn traj_scurve_eval(s: &TrajSCurve, tin: f64) -> TrajState {
```

**Parameters:**

- `s`: `&TrajSCurve`
- `tin`: `f64`

**Returns:** `TrajState`

*Defined at line 434*

---

## traj_times

The times 0, dt, 2 dt, ... and the end time `total`.

```fides
pub fn traj_times(total: f64, dt: f64) -> Vec<f64> {
```

**Parameters:**

- `total`: `f64`
- `dt`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 473*

---

## traj_sample_poly

A polynomial sampled every dt seconds.

```fides
pub fn traj_sample_poly(p: &TrajPoly, dt: f64) -> Vec<TrajState> {
```

**Parameters:**

- `p`: `&TrajPoly`
- `dt`: `f64`

**Returns:** `Vec<TrajState>`

*Defined at line 489*

---

## traj_sample_spline

A spline sampled every dt seconds from its first knot to its last.

```fides
pub fn traj_sample_spline(s: &TrajSpline, dt: f64) -> Vec<TrajState> {
```

**Parameters:**

- `s`: `&TrajSpline`
- `dt`: `f64`

**Returns:** `Vec<TrajState>`

*Defined at line 501*

---

## traj_sample_trapezoid

A trapezoidal move sampled every dt seconds.

```fides
pub fn traj_sample_trapezoid(tr: &TrajTrapezoid, dt: f64) -> Vec<TrajState> {
```

**Parameters:**

- `tr`: `&TrajTrapezoid`
- `dt`: `f64`

**Returns:** `Vec<TrajState>`

*Defined at line 514*

---

## traj_sample_scurve

An S-curve move sampled every dt seconds.

```fides
pub fn traj_sample_scurve(s: &TrajSCurve, dt: f64) -> Vec<TrajState> {
```

**Parameters:**

- `s`: `&TrajSCurve`
- `dt`: `f64`

**Returns:** `Vec<TrajState>`

*Defined at line 526*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
