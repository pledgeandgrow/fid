[All modules](index.md)

# utility_ai -- Fides Documentation

## Contents

- [UTIL_LINEAR:](#UTIL_LINEAR:) (const)
- [UTIL_QUADRATIC:](#UTIL_QUADRATIC:) (const)
- [UTIL_LOGISTIC:](#UTIL_LOGISTIC:) (const)
- [UTIL_STEP:](#UTIL_STEP:) (const)
- [UtilCurve](#UtilCurve) (struct)
- [util_curve](#util_curve) (function)
- [util_curve_eval](#util_curve_eval) (function)
- [UtilContext](#UtilContext) (struct)
- [util_context](#util_context) (function)
- [util_ctx_set](#util_ctx_set) (function)
- [util_ctx_get](#util_ctx_get) (function)
- [UtilConsideration](#UtilConsideration) (struct)
- [util_consideration](#util_consideration) (function)
- [util_normalize](#util_normalize) (function)
- [util_consideration_score](#util_consideration_score) (function)
- [UtilAction](#UtilAction) (struct)
- [util_action](#util_action) (function)
- [util_consider](#util_consider) (function)
- [util_action_score](#util_action_score) (function)
- [util_on_cooldown](#util_on_cooldown) (function)
- [UtilReasoner](#UtilReasoner) (struct)
- [util_reasoner](#util_reasoner) (function)
- [util_add_action](#util_add_action) (function)
- [util_select](#util_select) (function)
- [util_action_name](#util_action_name) (function)

## UTIL_LINEAR:

```fides
pub const UTIL_LINEAR: i64 = 0;
```

*Defined at line 33*

---

## UTIL_QUADRATIC:

```fides
pub const UTIL_QUADRATIC: i64 = 1;
```

*Defined at line 34*

---

## UTIL_LOGISTIC:

```fides
pub const UTIL_LOGISTIC: i64 = 2;
```

*Defined at line 35*

---

## UTIL_STEP:

```fides
pub const UTIL_STEP: i64 = 3;
```

*Defined at line 36*

---

## UtilCurve

A response curve. linear: m*(x-c)+b; quadratic: m*(x-c)^k+b;
logistic: k/(1+e^(-m*(x-c)))+b; step: k when x >= c, else b.

```fides
pub struct UtilCurve {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `m` | `f64` |
| `k` | `f64` |
| `c` | `f64` |
| `b` | `f64` |

*Defined at line 40*

---

## util_curve

```fides
pub fn util_curve(kind: i64, m: f64, k: f64, c: f64, b: f64) -> UtilCurve {
```

**Parameters:**

- `kind`: `i64`
- `m`: `f64`
- `k`: `f64`
- `c`: `f64`
- `b`: `f64`

**Returns:** `UtilCurve`

*Defined at line 48*

---

## util_curve_eval

The curve's utility at normalized input `x` (0..1), clamped to 0..1.

```fides
pub fn util_curve_eval(cv: &UtilCurve, x0: f64) -> f64 {
```

**Parameters:**

- `cv`: `&UtilCurve`
- `x0`: `f64`

**Returns:** `f64`

*Defined at line 87*

---

## UtilContext

The named inputs the considerations read (missing inputs read 0).

```fides
pub struct UtilContext {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `vals` | `Vec<f64>` |

*Defined at line 109*

---

## util_context

```fides
pub fn util_context() -> UtilContext {
```

**Returns:** `UtilContext`

*Defined at line 114*

---

## util_ctx_set

```fides
pub fn util_ctx_set(ctx: &mut UtilContext, key: str, value: f64) {
```

**Parameters:**

- `ctx`: `&mut UtilContext`
- `key`: `str`
- `value`: `f64`

*Defined at line 120*

---

## util_ctx_get

```fides
pub fn util_ctx_get(ctx: &UtilContext, key: str) -> f64 {
```

**Parameters:**

- `ctx`: `&UtilContext`
- `key`: `str`

**Returns:** `f64`

*Defined at line 133*

---

## UtilConsideration

```fides
pub struct UtilConsideration {
```

**Fields:**

| Name | Type |
|------|------|
| `input` | `str` |
| `lo` | `f64` |
| `hi` | `f64` |
| `curve` | `UtilCurve` |

*Defined at line 146*

---

## util_consideration

Read `input`, map lo..hi to 0..1 (clamped), apply `curve`.

```fides
pub fn util_consideration(input: str, lo: f64, hi: f64, curve: UtilCurve) -> UtilConsideration {
```

**Parameters:**

- `input`: `str`
- `lo`: `f64`
- `hi`: `f64`
- `curve`: `UtilCurve`

**Returns:** `UtilConsideration`

*Defined at line 154*

---

## util_normalize

The normalized input of `c` in `ctx`.

```fides
pub fn util_normalize(c: &UtilConsideration, ctx: &UtilContext) -> f64 {
```

**Parameters:**

- `c`: `&UtilConsideration`
- `ctx`: `&UtilContext`

**Returns:** `f64`

*Defined at line 161*

---

## util_consideration_score

```fides
pub fn util_consideration_score(c: &UtilConsideration, ctx: &UtilContext) -> f64 {
```

**Parameters:**

- `c`: `&UtilConsideration`
- `ctx`: `&UtilContext`

**Returns:** `f64`

*Defined at line 172*

---

## UtilAction

```fides
pub struct UtilAction {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `weight` | `f64` |
| `cooldown` | `f64` |
| `last_used` | `f64` |
| `considerations` | `Vec<UtilConsideration>` |

*Defined at line 176*

---

## util_action

```fides
pub fn util_action(name: str, weight: f64, cooldown: f64) -> UtilAction {
```

**Parameters:**

- `name`: `str`
- `weight`: `f64`
- `cooldown`: `f64`

**Returns:** `UtilAction`

*Defined at line 185*

---

## util_consider

```fides
pub fn util_consider(a: &mut UtilAction, c: UtilConsideration) {
```

**Parameters:**

- `a`: `&mut UtilAction`
- `c`: `UtilConsideration`

*Defined at line 191*

---

## util_action_score

Product of the consideration scores with the compensation factor
(each score s becomes s + (1 - s) * (1 - 1/n) * s), times the weight.
An action without considerations scores its weight.

```fides
pub fn util_action_score(a: &UtilAction, ctx: &UtilContext) -> f64 {
```

**Parameters:**

- `a`: `&UtilAction`
- `ctx`: `&UtilContext`

**Returns:** `f64`

*Defined at line 198*

---

## util_on_cooldown

Is `a` cooling down at time `now`?

```fides
pub fn util_on_cooldown(a: &UtilAction, now: f64) -> bool {
```

**Parameters:**

- `a`: `&UtilAction`
- `now`: `f64`

**Returns:** `bool`

*Defined at line 220*

---

## UtilReasoner

```fides
pub struct UtilReasoner {
```

**Fields:**

| Name | Type |
|------|------|
| `actions` | `Vec<UtilAction>` |
| `current` | `i64` |
| `inertia` | `f64` |
| `scores` | `Vec<f64>,  // scores of the last selection, one per action` |

*Defined at line 226*

---

## util_reasoner

```fides
pub fn util_reasoner(inertia: f64) -> UtilReasoner {
```

**Parameters:**

- `inertia`: `f64`

**Returns:** `UtilReasoner`

*Defined at line 235*

---

## util_add_action

```fides
pub fn util_add_action(r: &mut UtilReasoner, a: UtilAction) -> i64 {
```

**Parameters:**

- `r`: `&mut UtilReasoner`
- `a`: `UtilAction`

**Returns:** `i64`

*Defined at line 241*

---

## util_select

Score every action at time `now` and pick the best (first on ties);
-1 when every action scores 0. A newly picked action starts its
cooldown; the running one keeps going without restarting it.

```fides
pub fn util_select(r: &mut UtilReasoner, ctx: &UtilContext, now: f64) -> i64 {
```

**Parameters:**

- `r`: `&mut UtilReasoner`
- `ctx`: `&UtilContext`
- `now`: `f64`

**Returns:** `i64`

*Defined at line 249*

---

## util_action_name

The name of action `i`, "" for -1.

```fides
pub fn util_action_name(r: &UtilReasoner, i: i64) -> str {
```

**Parameters:**

- `r`: `&UtilReasoner`
- `i`: `i64`

**Returns:** `str`

*Defined at line 280*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
