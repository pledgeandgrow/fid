[All modules](index.md)

# state_machine -- Fides Documentation

## Contents

- [HSM_NO_HISTORY:](#HSM_NO_HISTORY:) (const)
- [HSM_SHALLOW:](#HSM_SHALLOW:) (const)
- [HSM_DEEP:](#HSM_DEEP:) (const)
- [HsmState](#HsmState) (struct)
- [HsmTrans](#HsmTrans) (struct)
- [Hsm](#Hsm) (struct)
- [hsm_new](#hsm_new) (function)
- [hsm_add_state](#hsm_add_state) (function)
- [hsm_set_initial](#hsm_set_initial) (function)
- [hsm_set_history](#hsm_set_history) (function)
- [hsm_on_entry](#hsm_on_entry) (function)
- [hsm_on_exit](#hsm_on_exit) (function)
- [hsm_on_do](#hsm_on_do) (function)
- [hsm_add_transition](#hsm_add_transition) (function)
- [hsm_add_timed](#hsm_add_timed) (function)
- [hsm_set_guard](#hsm_set_guard) (function)
- [hsm_set_action](#hsm_set_action) (function)
- [hsm_state_name](#hsm_state_name) (function)
- [hsm_find](#hsm_find) (function)
- [hsm_in](#hsm_in) (function)
- [hsm_path](#hsm_path) (function)
- [hsm_emit](#hsm_emit) (function)
- [hsm_start](#hsm_start) (function)
- [hsm_post](#hsm_post) (function)
- [hsm_dispatch](#hsm_dispatch) (function)
- [hsm_run_limit](#hsm_run_limit) (function)
- [hsm_run](#hsm_run) (function)
- [hsm_update](#hsm_update) (function)
- [hsm_take_log](#hsm_take_log) (function)

## HSM_NO_HISTORY:

```fides
pub const HSM_NO_HISTORY: i64 = 0;
```

*Defined at line 43*

---

## HSM_SHALLOW:

```fides
pub const HSM_SHALLOW: i64 = 1;
```

*Defined at line 44*

---

## HSM_DEEP:

```fides
pub const HSM_DEEP: i64 = 2;
```

*Defined at line 45*

---

## HsmState

```fides
pub struct HsmState {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `parent` | `i64` |
| `initial` | `i64` |
| `history` | `i64` |
| `hist` | `i64` |
| `entered_at` | `f64` |
| `has_entry` | `bool` |
| `has_exit` | `bool` |
| `has_do` | `bool` |
| `on_entry` | `fn(i64) -> i64` |
| `on_exit` | `fn(i64) -> i64` |
| `on_do` | `fn(i64) -> i64` |

*Defined at line 47*

---

## HsmTrans

```fides
pub struct HsmTrans {
```

**Fields:**

| Name | Type |
|------|------|
| `source` | `i64` |
| `target` | `i64` |
| `event` | `str` |
| `after` | `f64` |
| `has_guard` | `bool` |
| `has_action` | `bool` |
| `guard` | `fn(i64) -> i64` |
| `action` | `fn(i64) -> i64` |

*Defined at line 63*

---

## Hsm

```fides
pub struct Hsm {
```

**Fields:**

| Name | Type |
|------|------|
| `states` | `Vec<HsmState>` |
| `trans` | `Vec<HsmTrans>` |
| `initial` | `i64` |
| `cur` | `i64` |
| `queue` | `Vec<str>` |
| `now` | `f64` |
| `ctx` | `i64` |
| `log` | `Vec<str>` |
| `trace_do` | `bool` |

*Defined at line 77*

---

## hsm_new

A machine whose actions get `ctx`.

```fides
pub fn hsm_new(ctx: i64) -> Hsm {
```

**Parameters:**

- `ctx`: `i64`

**Returns:** `Hsm`

*Defined at line 96*

---

## hsm_add_state

Add a state under `parent` (-1: top level). The first child added to a
state becomes its initial child, and the first top-level state the
machine's initial state; hsm_set_initial changes either.

```fides
pub fn hsm_add_state(m: &mut Hsm, name: str, parent: i64) -> i64 {
```

**Parameters:**

- `m`: `&mut Hsm`
- `name`: `str`
- `parent`: `i64`

**Returns:** `i64`

*Defined at line 105*

---

## hsm_set_initial

The initial child of a state (state -1: the machine's initial state).

```fides
pub fn hsm_set_initial(m: &mut Hsm, state: i64, child: i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `state`: `i64`
- `child`: `i64`

*Defined at line 123*

---

## hsm_set_history

```fides
pub fn hsm_set_history(m: &mut Hsm, state: i64, kind: i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `state`: `i64`
- `kind`: `i64`

*Defined at line 133*

---

## hsm_on_entry

```fides
pub fn hsm_on_entry(m: &mut Hsm, state: i64, f: fn(i64) -> i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `state`: `i64`
- `f`: `fn(i64) -> i64`

*Defined at line 139*

---

## hsm_on_exit

```fides
pub fn hsm_on_exit(m: &mut Hsm, state: i64, f: fn(i64) -> i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `state`: `i64`
- `f`: `fn(i64) -> i64`

*Defined at line 146*

---

## hsm_on_do

```fides
pub fn hsm_on_do(m: &mut Hsm, state: i64, f: fn(i64) -> i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `state`: `i64`
- `f`: `fn(i64) -> i64`

*Defined at line 153*

---

## hsm_add_transition

A transition on `event` from source to target (-1: internal); its index.

```fides
pub fn hsm_add_transition(m: &mut Hsm, source: i64, target: i64, event: str) -> i64 {
```

**Parameters:**

- `m`: `&mut Hsm`
- `source`: `i64`
- `target`: `i64`
- `event`: `str`

**Returns:** `i64`

*Defined at line 161*

---

## hsm_add_timed

A transition taken after `ms` milliseconds in `source`.

```fides
pub fn hsm_add_timed(m: &mut Hsm, source: i64, target: i64, ms: f64) -> i64 {
```

**Parameters:**

- `m`: `&mut Hsm`
- `source`: `i64`
- `target`: `i64`
- `ms`: `f64`

**Returns:** `i64`

*Defined at line 169*

---

## hsm_set_guard

```fides
pub fn hsm_set_guard(m: &mut Hsm, t: i64, g: fn(i64) -> i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `t`: `i64`
- `g`: `fn(i64) -> i64`

*Defined at line 176*

---

## hsm_set_action

```fides
pub fn hsm_set_action(m: &mut Hsm, t: i64, a: fn(i64) -> i64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `t`: `i64`
- `a`: `fn(i64) -> i64`

*Defined at line 183*

---

## hsm_state_name

```fides
pub fn hsm_state_name(m: &Hsm, id: i64) -> str {
```

**Parameters:**

- `m`: `&Hsm`
- `id`: `i64`

**Returns:** `str`

*Defined at line 190*

---

## hsm_find

The id of the state with this name (-1 when none).

```fides
pub fn hsm_find(m: &Hsm, name: str) -> i64 {
```

**Parameters:**

- `m`: `&Hsm`
- `name`: `str`

**Returns:** `i64`

*Defined at line 198*

---

## hsm_in

Whether `s` is the active leaf or one of its ancestors.

```fides
pub fn hsm_in(m: &Hsm, s: i64) -> bool {
```

**Parameters:**

- `m`: `&Hsm`
- `s`: `i64`

**Returns:** `bool`

*Defined at line 210*

---

## hsm_path

The active configuration, outermost first: "On/Moving/Fast".

```fides
pub fn hsm_path(m: &Hsm) -> str {
```

**Parameters:**

- `m`: `&Hsm`

**Returns:** `str`

*Defined at line 222*

---

## hsm_emit

Post an event from inside an action; it is queued after the current
one (run to completion).

```fides
pub fn hsm_emit(ev: str) {
```

**Parameters:**

- `ev`: `str`

*Defined at line 242*

---

## hsm_start

Enter the initial configuration.

```fides
pub fn hsm_start(m: &mut Hsm, now: f64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `now`: `f64`

*Defined at line 367*

---

## hsm_post

Queue an event.

```fides
pub fn hsm_post(m: &mut Hsm, ev: str) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `ev`: `str`

*Defined at line 435*

---

## hsm_dispatch

Process one event now (run to completion); false when nothing took it.

```fides
pub fn hsm_dispatch(m: &mut Hsm, ev: str) -> bool {
```

**Parameters:**

- `m`: `&mut Hsm`
- `ev`: `str`

**Returns:** `bool`

*Defined at line 440*

---

## hsm_run_limit

Process queued events until the queue is empty (events emitted by
actions are processed too, in order). The number processed; stops after
`limit` events to break event loops.

```fides
pub fn hsm_run_limit(m: &mut Hsm, limit: i64) -> i64 {
```

**Parameters:**

- `m`: `&mut Hsm`
- `limit`: `i64`

**Returns:** `i64`

*Defined at line 454*

---

## hsm_run

```fides
pub fn hsm_run(m: &mut Hsm) -> i64 {
```

**Parameters:**

- `m`: `&mut Hsm`

**Returns:** `i64`

*Defined at line 465*

---

## hsm_update

Advance the clock: run the do actions of the active states (outermost
first), take due timed transitions (innermost first, one per call
level, then again from the new state), then the queued events.

```fides
pub fn hsm_update(m: &mut Hsm, now: f64) {
```

**Parameters:**

- `m`: `&mut Hsm`
- `now`: `f64`

*Defined at line 472*

---

## hsm_take_log

The trace joined with ", " (and cleared).

```fides
pub fn hsm_take_log(m: &mut Hsm) -> str {
```

**Parameters:**

- `m`: `&mut Hsm`

**Returns:** `str`

*Defined at line 518*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
