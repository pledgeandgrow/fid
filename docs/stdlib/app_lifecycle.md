[All modules](index.md)

# app_lifecycle -- Fides Documentation

## Contents

- [LC_NOT_RUNNING:](#LC_NOT_RUNNING:) (const)
- [LC_ACTIVE:](#LC_ACTIVE:) (const)
- [LC_INACTIVE:](#LC_INACTIVE:) (const)
- [LC_BACKGROUND:](#LC_BACKGROUND:) (const)
- [LC_SUSPENDED:](#LC_SUSPENDED:) (const)
- [LC_TERMINATED:](#LC_TERMINATED:) (const)
- [LC_STATE_COUNT:](#LC_STATE_COUNT:) (const)
- [LC_EV_LAUNCH:](#LC_EV_LAUNCH:) (const)
- [LC_EV_LAUNCH_BACKGROUND:](#LC_EV_LAUNCH_BACKGROUND:) (const)
- [LC_EV_BECOME_ACTIVE:](#LC_EV_BECOME_ACTIVE:) (const)
- [LC_EV_RESIGN_ACTIVE:](#LC_EV_RESIGN_ACTIVE:) (const)
- [LC_EV_ENTER_BACKGROUND:](#LC_EV_ENTER_BACKGROUND:) (const)
- [LC_EV_ENTER_FOREGROUND:](#LC_EV_ENTER_FOREGROUND:) (const)
- [LC_EV_SUSPEND:](#LC_EV_SUSPEND:) (const)
- [LC_EV_WAKE:](#LC_EV_WAKE:) (const)
- [LC_EV_TERMINATE:](#LC_EV_TERMINATE:) (const)
- [LC_EV_MEMORY_WARNING:](#LC_EV_MEMORY_WARNING:) (const)
- [LC_EV_NONE:](#LC_EV_NONE:) (const)
- [LC_MASK_ALL:](#LC_MASK_ALL:) (const)
- [lc_mask](#lc_mask) (function)
- [LC_LAUNCH_COLD:](#LC_LAUNCH_COLD:) (const)
- [LC_LAUNCH_WARM:](#LC_LAUNCH_WARM:) (const)
- [LC_LAUNCH_HOT:](#LC_LAUNCH_HOT:) (const)
- [lc_state_name](#lc_state_name) (function)
- [lc_event_name](#lc_event_name) (function)
- [lc_next](#lc_next) (function)
- [lc_path](#lc_path) (function)
- [LcObserver](#LcObserver) (struct)
- [Lifecycle](#Lifecycle) (struct)
- [lifecycle_new](#lifecycle_new) (function)
- [lifecycle_observe](#lifecycle_observe) (function)
- [lifecycle_unobserve](#lifecycle_unobserve) (function)
- [lifecycle_apply](#lifecycle_apply) (function)
- [lifecycle_dispatch](#lifecycle_dispatch) (function)
- [lifecycle_tick](#lifecycle_tick) (function)
- [lifecycle_is_foreground](#lifecycle_is_foreground) (function)
- [lifecycle_is_running](#lifecycle_is_running) (function)
- [ANDROID_TRIM_MEMORY_RUNNING_MODERATE:](#ANDROID_TRIM_MEMORY_RUNNING_MODERATE:) (const)
- [ANDROID_TRIM_MEMORY_RUNNING_LOW:](#ANDROID_TRIM_MEMORY_RUNNING_LOW:) (const)
- [ANDROID_TRIM_MEMORY_RUNNING_CRITICAL:](#ANDROID_TRIM_MEMORY_RUNNING_CRITICAL:) (const)
- [ANDROID_TRIM_MEMORY_UI_HIDDEN:](#ANDROID_TRIM_MEMORY_UI_HIDDEN:) (const)
- [ANDROID_TRIM_MEMORY_BACKGROUND:](#ANDROID_TRIM_MEMORY_BACKGROUND:) (const)
- [ANDROID_TRIM_MEMORY_MODERATE:](#ANDROID_TRIM_MEMORY_MODERATE:) (const)
- [ANDROID_TRIM_MEMORY_COMPLETE:](#ANDROID_TRIM_MEMORY_COMPLETE:) (const)
- [lc_android_event](#lc_android_event) (function)
- [lc_ios_event](#lc_ios_event) (function)
- [lifecycle_android](#lifecycle_android) (function)
- [lifecycle_ios](#lifecycle_ios) (function)
- [lifecycle_summary](#lifecycle_summary) (function)

## LC_NOT_RUNNING:

```fides
pub const LC_NOT_RUNNING: i64 = 0;
```

*Defined at line 34*

---

## LC_ACTIVE:

```fides
pub const LC_ACTIVE: i64 = 1;  // foreground, receiving events
```

*Defined at line 35*

---

## LC_INACTIVE:

```fides
pub const LC_INACTIVE: i64 = 2;  // foreground, not receiving events
```

*Defined at line 36*

---

## LC_BACKGROUND:

```fides
pub const LC_BACKGROUND: i64 = 3;  // running, not visible
```

*Defined at line 37*

---

## LC_SUSPENDED:

```fides
pub const LC_SUSPENDED: i64 = 4;  // in memory, not running
```

*Defined at line 38*

---

## LC_TERMINATED:

```fides
pub const LC_TERMINATED: i64 = 5;
```

*Defined at line 39*

---

## LC_STATE_COUNT:

```fides
pub const LC_STATE_COUNT: i64 = 6;
```

*Defined at line 40*

---

## LC_EV_LAUNCH:

```fides
pub const LC_EV_LAUNCH: i64 = 0;  // launched into the foreground
```

*Defined at line 43*

---

## LC_EV_LAUNCH_BACKGROUND:

```fides
pub const LC_EV_LAUNCH_BACKGROUND: i64 = 1;  // launched for background work
```

*Defined at line 44*

---

## LC_EV_BECOME_ACTIVE:

```fides
pub const LC_EV_BECOME_ACTIVE: i64 = 2;
```

*Defined at line 45*

---

## LC_EV_RESIGN_ACTIVE:

```fides
pub const LC_EV_RESIGN_ACTIVE: i64 = 3;
```

*Defined at line 46*

---

## LC_EV_ENTER_BACKGROUND:

```fides
pub const LC_EV_ENTER_BACKGROUND: i64 = 4;
```

*Defined at line 47*

---

## LC_EV_ENTER_FOREGROUND:

```fides
pub const LC_EV_ENTER_FOREGROUND: i64 = 5;
```

*Defined at line 48*

---

## LC_EV_SUSPEND:

```fides
pub const LC_EV_SUSPEND: i64 = 6;
```

*Defined at line 49*

---

## LC_EV_WAKE:

```fides
pub const LC_EV_WAKE: i64 = 7;  // suspended app run in background
```

*Defined at line 50*

---

## LC_EV_TERMINATE:

```fides
pub const LC_EV_TERMINATE: i64 = 8;
```

*Defined at line 51*

---

## LC_EV_MEMORY_WARNING:

```fides
pub const LC_EV_MEMORY_WARNING: i64 = 9;  // no state change
```

*Defined at line 52*

---

## LC_EV_NONE:

```fides
pub const LC_EV_NONE: i64 = -1;
```

*Defined at line 53*

---

## LC_MASK_ALL:

```fides
pub const LC_MASK_ALL: i64 = 63;
```

*Defined at line 56*

---

## lc_mask

```fides
pub fn lc_mask(state: i64) -> i64 {
```

**Parameters:**

- `state`: `i64`

**Returns:** `i64`

*Defined at line 58*

---

## LC_LAUNCH_COLD:

```fides
pub const LC_LAUNCH_COLD: i64 = 0;  // process started
```

*Defined at line 63*

---

## LC_LAUNCH_WARM:

```fides
pub const LC_LAUNCH_WARM: i64 = 1;  // process alive, UI rebuilt from background
```

*Defined at line 64*

---

## LC_LAUNCH_HOT:

```fides
pub const LC_LAUNCH_HOT: i64 = 2;  // resumed from suspension
```

*Defined at line 65*

---

## lc_state_name

```fides
pub fn lc_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 67*

---

## lc_event_name

```fides
pub fn lc_event_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 89*

---

## lc_next

The transition table: the state `event` moves `from` to, or -1 when
the event is not valid there. A memory warning keeps any running state.

```fides
pub fn lc_next(from: i64, event: i64) -> i64 {
```

**Parameters:**

- `from`: `i64`
- `event`: `i64`

**Returns:** `i64`

*Defined at line 99*

---

## lc_path

The events (shortest sequence of the table) that take `from` to `to`;
empty when from == to or `to` cannot be reached. Termination and
memory warnings are never inserted.

```fides
pub fn lc_path(from: i64, to: i64) -> Vec<i64> {
```

**Parameters:**

- `from`: `i64`
- `to`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 160*

---

## LcObserver

```fides
pub struct LcObserver {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `mask` | `i64` |
| `f` | `fn(i64, i64, i64) -> void,  // from, to, event` |

*Defined at line 222*

---

## Lifecycle

```fides
pub struct Lifecycle {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `observers` | `Vec<LcObserver>` |
| `next_id` | `i64` |
| `history` | `Vec<i64>,  // events applied, in order` |
| `rejected` | `i64,  // invalid events seen` |
| `memory_warnings` | `i64` |
| `launches` | `Vec<i64>,  // LC_LAUNCH_* of each return to the foreground` |
| `time_in` | `Vec<i64>,  // ms spent in each state` |
| `since` | `i64,  // timestamp of the last transition (-1: none yet)` |
| `was_suspended` | `bool` |
| `cold_pending` | `bool` |

*Defined at line 228*

---

## lifecycle_new

```fides
pub fn lifecycle_new() -> Lifecycle {
```

**Returns:** `Lifecycle`

*Defined at line 242*

---

## lifecycle_observe

Register an observer for transitions into the states of `mask`;
returns its id.

```fides
pub fn lifecycle_observe(lc: &mut Lifecycle, mask: i64, f: fn(i64, i64, i64) -> void) -> i64 {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `mask`: `i64`
- `f`: `fn(i64`

**Returns:** `i64`

*Defined at line 256*

---

## lifecycle_unobserve

Remove an observer; false when there is none with that id.

```fides
pub fn lifecycle_unobserve(lc: &mut Lifecycle, id: i64) -> bool {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 266*

---

## lifecycle_apply

Apply one event strictly: false (and nothing changes) when the table
has no such transition from the current state.

```fides
pub fn lifecycle_apply(lc: &mut Lifecycle, event: i64, now: i64) -> bool {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `event`: `i64`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 299*

---

## lifecycle_dispatch

Apply an event tolerantly: when it is not valid in the current state,
first take the shortest path (without launching) to a state where it
is, then apply it.
Returns the number of events applied (0: impossible, nothing done).

```fides
pub fn lifecycle_dispatch(lc: &mut Lifecycle, event: i64, now: i64) -> i64 {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `event`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 347*

---

## lifecycle_tick

Close the time accounting at `now` (e.g. before reporting).

```fides
pub fn lifecycle_tick(lc: &mut Lifecycle, now: i64) {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `now`: `i64`

*Defined at line 383*

---

## lifecycle_is_foreground

```fides
pub fn lifecycle_is_foreground(lc: &Lifecycle) -> bool {
```

**Parameters:**

- `lc`: `&Lifecycle`

**Returns:** `bool`

*Defined at line 387*

---

## lifecycle_is_running

May the app run code now (not suspended, not terminated, launched)?

```fides
pub fn lifecycle_is_running(lc: &Lifecycle) -> bool {
```

**Parameters:**

- `lc`: `&Lifecycle`

**Returns:** `bool`

*Defined at line 392*

---

## ANDROID_TRIM_MEMORY_RUNNING_MODERATE:

Android TRIM_MEMORY_* levels (ComponentCallbacks2).

```fides
pub const ANDROID_TRIM_MEMORY_RUNNING_MODERATE: i64 = 5;
```

*Defined at line 399*

---

## ANDROID_TRIM_MEMORY_RUNNING_LOW:

```fides
pub const ANDROID_TRIM_MEMORY_RUNNING_LOW: i64 = 10;
```

*Defined at line 400*

---

## ANDROID_TRIM_MEMORY_RUNNING_CRITICAL:

```fides
pub const ANDROID_TRIM_MEMORY_RUNNING_CRITICAL: i64 = 15;
```

*Defined at line 401*

---

## ANDROID_TRIM_MEMORY_UI_HIDDEN:

```fides
pub const ANDROID_TRIM_MEMORY_UI_HIDDEN: i64 = 20;
```

*Defined at line 402*

---

## ANDROID_TRIM_MEMORY_BACKGROUND:

```fides
pub const ANDROID_TRIM_MEMORY_BACKGROUND: i64 = 40;
```

*Defined at line 403*

---

## ANDROID_TRIM_MEMORY_MODERATE:

```fides
pub const ANDROID_TRIM_MEMORY_MODERATE: i64 = 60;
```

*Defined at line 404*

---

## ANDROID_TRIM_MEMORY_COMPLETE:

```fides
pub const ANDROID_TRIM_MEMORY_COMPLETE: i64 = 80;
```

*Defined at line 405*

---

## lc_android_event

The event of an Android Activity callback (LC_EV_NONE when it changes
nothing: onRestart, onSaveInstanceState, a mild onTrimMemory). `arg`
is the trim level for onTrimMemory.

```fides
pub fn lc_android_event(callback: str, arg: i64) -> i64 {
```

**Parameters:**

- `callback`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 410*

---

## lc_ios_event

The event of a UIApplicationDelegate or UISceneDelegate method.
`arg` for application:didFinishLaunchingWithOptions: is 1 when the
app was launched into the background (applicationState background).

```fides
pub fn lc_ios_event(method: str, arg: i64) -> i64 {
```

**Parameters:**

- `method`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 451*

---

## lifecycle_android

Feed an Android callback; returns the events applied.

```fides
pub fn lifecycle_android(lc: &mut Lifecycle, callback: str, arg: i64, now: i64) -> i64 {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `callback`: `str`
- `arg`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 487*

---

## lifecycle_ios

Feed an iOS delegate method; returns the events applied.

```fides
pub fn lifecycle_ios(lc: &mut Lifecycle, method: str, arg: i64, now: i64) -> i64 {
```

**Parameters:**

- `lc`: `&mut Lifecycle`
- `method`: `str`
- `arg`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 492*

---

## lifecycle_summary

One line: state, launches (c/w/h), ms per state, warnings, rejections.

```fides
pub fn lifecycle_summary(lc: &Lifecycle) -> str {
```

**Parameters:**

- `lc`: `&Lifecycle`

**Returns:** `str`

*Defined at line 497*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
