[All modules](index.md)

# goap -- Fides Documentation

## Contents

- [GOAP_EQ:](#GOAP_EQ:) (const)
- [GOAP_NE:](#GOAP_NE:) (const)
- [GOAP_LT:](#GOAP_LT:) (const)
- [GOAP_LE:](#GOAP_LE:) (const)
- [GOAP_GT:](#GOAP_GT:) (const)
- [GOAP_GE:](#GOAP_GE:) (const)
- [GOAP_SET:](#GOAP_SET:) (const)
- [GOAP_ADD:](#GOAP_ADD:) (const)
- [GoapState](#GoapState) (struct)
- [goap_state](#goap_state) (function)
- [goap_get](#goap_get) (function)
- [goap_set](#goap_set) (function)
- [goap_state_key](#goap_state_key) (function)
- [GoapCond](#GoapCond) (struct)
- [GoapEffect](#GoapEffect) (struct)
- [GoapAction](#GoapAction) (struct)
- [goap_action](#goap_action) (function)
- [goap_pre](#goap_pre) (function)
- [goap_effect_set](#goap_effect_set) (function)
- [goap_add](#goap_add) (function)
- [GoapGoal](#GoapGoal) (struct)
- [goap_goal](#goap_goal) (function)
- [goap_goal_add](#goap_goal_add) (function)
- [goap_cond_holds](#goap_cond_holds) (function)
- [goap_unmet](#goap_unmet) (function)
- [goap_usable](#goap_usable) (function)
- [goap_apply](#goap_apply) (function)
- [GoapPlan](#GoapPlan) (struct)
- [goap_plan](#goap_plan) (function)
- [GoapRun](#GoapRun) (struct)
- [goap_simulate](#goap_simulate) (function)

## GOAP_EQ:

```fides
pub const GOAP_EQ: i64 = 0;
```

*Defined at line 30*

---

## GOAP_NE:

```fides
pub const GOAP_NE: i64 = 1;
```

*Defined at line 31*

---

## GOAP_LT:

```fides
pub const GOAP_LT: i64 = 2;
```

*Defined at line 32*

---

## GOAP_LE:

```fides
pub const GOAP_LE: i64 = 3;
```

*Defined at line 33*

---

## GOAP_GT:

```fides
pub const GOAP_GT: i64 = 4;
```

*Defined at line 34*

---

## GOAP_GE:

```fides
pub const GOAP_GE: i64 = 5;
```

*Defined at line 35*

---

## GOAP_SET:

```fides
pub const GOAP_SET: i64 = 0;
```

*Defined at line 37*

---

## GOAP_ADD:

```fides
pub const GOAP_ADD: i64 = 1;
```

*Defined at line 38*

---

## GoapState

Facts, sorted by name.

```fides
pub struct GoapState {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `vals` | `Vec<i64>` |

*Defined at line 43*

---

## goap_state

```fides
pub fn goap_state() -> GoapState {
```

**Returns:** `GoapState`

*Defined at line 48*

---

## goap_get

The value of fact `key` (0 when it is not set).

```fides
pub fn goap_get(s: &GoapState, key: str) -> i64 {
```

**Parameters:**

- `s`: `&GoapState`
- `key`: `str`

**Returns:** `i64`

*Defined at line 74*

---

## goap_set

Set fact `key` to `value`.

```fides
pub fn goap_set(s: &mut GoapState, key: str, value: i64) {
```

**Parameters:**

- `s`: `&mut GoapState`
- `key`: `str`
- `value`: `i64`

*Defined at line 83*

---

## goap_state_key

The state as text, "k=v;" per fact in name order (facts equal to 0 are
left out, as a missing fact reads 0).

```fides
pub fn goap_state_key(s: &GoapState) -> str {
```

**Parameters:**

- `s`: `&GoapState`

**Returns:** `str`

*Defined at line 96*

---

## GoapCond

```fides
pub struct GoapCond {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `op` | `i64` |
| `value` | `i64` |

*Defined at line 121*

---

## GoapEffect

```fides
pub struct GoapEffect {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `kind` | `i64` |
| `value` | `i64` |

*Defined at line 127*

---

## GoapAction

```fides
pub struct GoapAction {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `cost` | `f64` |
| `pre` | `Vec<GoapCond>` |
| `effects` | `Vec<GoapEffect>` |

*Defined at line 134*

---

## goap_action

```fides
pub fn goap_action(name: str, cost: f64) -> GoapAction {
```

**Parameters:**

- `name`: `str`
- `cost`: `f64`

**Returns:** `GoapAction`

*Defined at line 141*

---

## goap_pre

Add a precondition: fact `key` OP `value`.

```fides
pub fn goap_pre(a: &mut GoapAction, key: str, op: i64, value: i64) {
```

**Parameters:**

- `a`: `&mut GoapAction`
- `key`: `str`
- `op`: `i64`
- `value`: `i64`

*Defined at line 148*

---

## goap_effect_set

Add an effect: fact `key` becomes `value`.

```fides
pub fn goap_effect_set(a: &mut GoapAction, key: str, value: i64) {
```

**Parameters:**

- `a`: `&mut GoapAction`
- `key`: `str`
- `value`: `i64`

*Defined at line 155*

---

## goap_add

Add an effect: `delta` is added to fact `key`.

```fides
pub fn goap_add(a: &mut GoapAction, key: str, delta: i64) {
```

**Parameters:**

- `a`: `&mut GoapAction`
- `key`: `str`
- `delta`: `i64`

*Defined at line 162*

---

## GoapGoal

A goal: every condition must hold.

```fides
pub struct GoapGoal {
```

**Fields:**

| Name | Type |
|------|------|
| `conds` | `Vec<GoapCond>` |

*Defined at line 169*

---

## goap_goal

```fides
pub fn goap_goal() -> GoapGoal {
```

**Returns:** `GoapGoal`

*Defined at line 173*

---

## goap_goal_add

```fides
pub fn goap_goal_add(g: &mut GoapGoal, key: str, op: i64, value: i64) {
```

**Parameters:**

- `g`: `&mut GoapGoal`
- `key`: `str`
- `op`: `i64`
- `value`: `i64`

*Defined at line 179*

---

## goap_cond_holds

Does `c` hold in `s`?

```fides
pub fn goap_cond_holds(s: &GoapState, c: &GoapCond) -> bool {
```

**Parameters:**

- `s`: `&GoapState`
- `c`: `&GoapCond`

**Returns:** `bool`

*Defined at line 186*

---

## goap_unmet

Number of goal conditions not met in `s`.

```fides
pub fn goap_unmet(s: &GoapState, g: &GoapGoal) -> i64 {
```

**Parameters:**

- `s`: `&GoapState`
- `g`: `&GoapGoal`

**Returns:** `i64`

*Defined at line 219*

---

## goap_usable

Can `a` run in `s`?

```fides
pub fn goap_usable(s: &GoapState, a: &GoapAction) -> bool {
```

**Parameters:**

- `s`: `&GoapState`
- `a`: `&GoapAction`

**Returns:** `bool`

*Defined at line 233*

---

## goap_apply

The state after running `a` in `s`.

```fides
pub fn goap_apply(s: &GoapState, a: &GoapAction) -> GoapState {
```

**Parameters:**

- `s`: `&GoapState`
- `a`: `&GoapAction`

**Returns:** `GoapState`

*Defined at line 238*

---

## GoapPlan

```fides
pub struct GoapPlan {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `actions` | `Vec<str>` |
| `cost` | `f64` |
| `expanded` | `i64,  // nodes taken off the open list` |

*Defined at line 255*

---

## goap_plan

The cheapest sequence of `actions` that takes `start` to a state where
`goal` holds. Gives up (ok false) after `max_nodes` expansions or when
every reachable state has been tried.

```fides
pub fn goap_plan(start: &GoapState, goal: &GoapGoal, actions: &Vec<GoapAction>, max_nodes: i64) -> GoapPlan {
```

**Parameters:**

- `start`: `&GoapState`
- `goal`: `&GoapGoal`
- `actions`: `&Vec<GoapAction>`
- `max_nodes`: `i64`

**Returns:** `GoapPlan`

*Defined at line 275*

---

## GoapRun

Run a plan's actions in order from `start` (a check that the plan works:
each action's preconditions must hold when it runs). The final state,
or the state where an action could not run when `ok` is false.

```fides
pub struct GoapRun {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `state` | `GoapState` |
| `failed_at` | `i64` |

*Defined at line 365*

---

## goap_simulate

```fides
pub fn goap_simulate(start: &GoapState, plan: &GoapPlan, actions: &Vec<GoapAction>) -> GoapRun {
```

**Parameters:**

- `start`: `&GoapState`
- `plan`: `&GoapPlan`
- `actions`: `&Vec<GoapAction>`

**Returns:** `GoapRun`

*Defined at line 371*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
