[All modules](index.md)

# timeline -- Fides Documentation

## Contents

- [TL_STEP:](#TL_STEP:) (const)
- [TL_LINEAR:](#TL_LINEAR:) (const)
- [TL_CUBIC:](#TL_CUBIC:) (const)
- [TlFloatTrack](#TlFloatTrack) (struct)
- [TlVecTrack](#TlVecTrack) (struct)
- [TlEventTrack](#TlEventTrack) (struct)
- [TlFired](#TlFired) (struct)
- [Timeline](#Timeline) (struct)
- [timeline_new](#timeline_new) (function)
- [timeline_interp](#timeline_interp) (function)
- [timeline_add_float](#timeline_add_float) (function)
- [timeline_add_vec](#timeline_add_vec) (function)
- [timeline_add_events](#timeline_add_events) (function)
- [timeline_key](#timeline_key) (function)
- [timeline_key_vec](#timeline_key_vec) (function)
- [timeline_event](#timeline_event) (function)
- [timeline_sample_float](#timeline_sample_float) (function)
- [timeline_sample_vec](#timeline_sample_vec) (function)
- [timeline_float](#timeline_float) (function)
- [timeline_vec](#timeline_vec) (function)
- [timeline_find](#timeline_find) (function)
- [timeline_seek](#timeline_seek) (function)
- [timeline_advance](#timeline_advance) (function)

## TL_STEP:

```fides
pub const TL_STEP: i64 = 0;
```

*Defined at line 35*

---

## TL_LINEAR:

```fides
pub const TL_LINEAR: i64 = 1;
```

*Defined at line 36*

---

## TL_CUBIC:

```fides
pub const TL_CUBIC: i64 = 2;
```

*Defined at line 37*

---

## TlFloatTrack

```fides
pub struct TlFloatTrack {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `times` | `Vec<f64>` |
| `vals` | `Vec<f64>` |
| `modes` | `Vec<i64>` |

*Defined at line 39*

---

## TlVecTrack

```fides
pub struct TlVecTrack {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `times` | `Vec<f64>` |
| `xs` | `Vec<f64>` |
| `ys` | `Vec<f64>` |
| `zs` | `Vec<f64>` |
| `modes` | `Vec<i64>` |

*Defined at line 46*

---

## TlEventTrack

```fides
pub struct TlEventTrack {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `times` | `Vec<f64>` |
| `names` | `Vec<str>` |
| `payloads` | `Vec<i64>` |

*Defined at line 55*

---

## TlFired

An event crossed by `timeline_advance`.

```fides
pub struct TlFired {
```

**Fields:**

| Name | Type |
|------|------|
| `track` | `str` |
| `name` | `str` |
| `time` | `f64` |
| `payload` | `i64` |

*Defined at line 63*

---

## Timeline

```fides
pub struct Timeline {
```

**Fields:**

| Name | Type |
|------|------|
| `length` | `f64` |
| `looping` | `bool` |
| `speed` | `f64` |
| `time` | `f64` |
| `started` | `bool` |
| `finished` | `bool` |
| `floats` | `Vec<TlFloatTrack>` |
| `vecs` | `Vec<TlVecTrack>` |
| `events` | `Vec<TlEventTrack>` |

*Defined at line 70*

---

## timeline_new

```fides
pub fn timeline_new(length: f64, looping: bool) -> Timeline {
```

**Parameters:**

- `length`: `f64`
- `looping`: `bool`

**Returns:** `Timeline`

*Defined at line 84*

---

## timeline_interp

Value of keys (`times`, `vals`, `modes`) at time `t`.

```fides
pub fn timeline_interp(times: &Vec<f64>, vals: &Vec<f64>, modes: &Vec<i64>, t: f64) -> f64 {
```

**Parameters:**

- `times`: `&Vec<f64>`
- `vals`: `&Vec<f64>`
- `modes`: `&Vec<i64>`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 126*

---

## timeline_add_float

Add a float track; returns its index.

```fides
pub fn timeline_add_float(tl: &mut Timeline, name: str) -> i64 {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `name`: `str`

**Returns:** `i64`

*Defined at line 170*

---

## timeline_add_vec

Add a vec3 track; returns its index.

```fides
pub fn timeline_add_vec(tl: &mut Timeline, name: str) -> i64 {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `name`: `str`

**Returns:** `i64`

*Defined at line 178*

---

## timeline_add_events

Add an event track; returns its index.

```fides
pub fn timeline_add_events(tl: &mut Timeline, name: str) -> i64 {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `name`: `str`

**Returns:** `i64`

*Defined at line 186*

---

## timeline_key

Key float track `track` at time `t`; `mode` shapes the segment after it.

```fides
pub fn timeline_key(tl: &mut Timeline, track: i64, t: f64, value: f64, mode: i64) {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `track`: `i64`
- `t`: `f64`
- `value`: `f64`
- `mode`: `i64`

*Defined at line 194*

---

## timeline_key_vec

Key vec3 track `track` at time `t`.

```fides
pub fn timeline_key_vec(tl: &mut Timeline, track: i64, t: f64, v: vec3, mode: i64) {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `track`: `i64`
- `t`: `f64`
- `v`: `vec3`
- `mode`: `i64`

*Defined at line 204*

---

## timeline_event

Put event `name` (with `payload`) on event track `track` at time `t`.

```fides
pub fn timeline_event(tl: &mut Timeline, track: i64, t: f64, name: str, payload: i64) {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `track`: `i64`
- `t`: `f64`
- `name`: `str`
- `payload`: `i64`

*Defined at line 216*

---

## timeline_sample_float

Float track `track` at time `t`.

```fides
pub fn timeline_sample_float(tl: &Timeline, track: i64, t: f64) -> f64 {
```

**Parameters:**

- `tl`: `&Timeline`
- `track`: `i64`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 226*

---

## timeline_sample_vec

Vec3 track `track` at time `t` (each component interpolated).

```fides
pub fn timeline_sample_vec(tl: &Timeline, track: i64, t: f64) -> vec3 {
```

**Parameters:**

- `tl`: `&Timeline`
- `track`: `i64`
- `t`: `f64`

**Returns:** `vec3`

*Defined at line 232*

---

## timeline_float

Float track `track` at the current time.

```fides
pub fn timeline_float(tl: &Timeline, track: i64) -> f64 {
```

**Parameters:**

- `tl`: `&Timeline`
- `track`: `i64`

**Returns:** `f64`

*Defined at line 238*

---

## timeline_vec

Vec3 track `track` at the current time.

```fides
pub fn timeline_vec(tl: &Timeline, track: i64) -> vec3 {
```

**Parameters:**

- `tl`: `&Timeline`
- `track`: `i64`

**Returns:** `vec3`

*Defined at line 243*

---

## timeline_find

Index of the track named `name` among float (kind 0), vec (1) or event
(2) tracks, -1 when there is none.

```fides
pub fn timeline_find(tl: &Timeline, kind: i64, name: str) -> i64 {
```

**Parameters:**

- `tl`: `&Timeline`
- `kind`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 249*

---

## timeline_seek

Jump to time `t` (clamped to 0..length) without firing events.

```fides
pub fn timeline_seek(tl: &mut Timeline, t: f64) {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `t`: `f64`

*Defined at line 328*

---

## timeline_advance

Advance by `dt` * speed seconds; the events crossed, in order.

```fides
pub fn timeline_advance(tl: &mut Timeline, dt: f64) -> Vec<TlFired> {
```

**Parameters:**

- `tl`: `&mut Timeline`
- `dt`: `f64`

**Returns:** `Vec<TlFired>`

*Defined at line 342*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
