[All modules](index.md)

# tween -- Fides Documentation

## Contents

- [EASE_LINEAR:](#EASE_LINEAR:) (const)
- [EASE_QUAD_IN:](#EASE_QUAD_IN:) (const)
- [EASE_QUAD_OUT:](#EASE_QUAD_OUT:) (const)
- [EASE_QUAD_INOUT:](#EASE_QUAD_INOUT:) (const)
- [EASE_CUBIC_IN:](#EASE_CUBIC_IN:) (const)
- [EASE_CUBIC_OUT:](#EASE_CUBIC_OUT:) (const)
- [EASE_CUBIC_INOUT:](#EASE_CUBIC_INOUT:) (const)
- [EASE_QUART_IN:](#EASE_QUART_IN:) (const)
- [EASE_QUART_OUT:](#EASE_QUART_OUT:) (const)
- [EASE_QUART_INOUT:](#EASE_QUART_INOUT:) (const)
- [EASE_QUINT_IN:](#EASE_QUINT_IN:) (const)
- [EASE_QUINT_OUT:](#EASE_QUINT_OUT:) (const)
- [EASE_QUINT_INOUT:](#EASE_QUINT_INOUT:) (const)
- [EASE_SINE_IN:](#EASE_SINE_IN:) (const)
- [EASE_SINE_OUT:](#EASE_SINE_OUT:) (const)
- [EASE_SINE_INOUT:](#EASE_SINE_INOUT:) (const)
- [EASE_EXPO_IN:](#EASE_EXPO_IN:) (const)
- [EASE_EXPO_OUT:](#EASE_EXPO_OUT:) (const)
- [EASE_EXPO_INOUT:](#EASE_EXPO_INOUT:) (const)
- [EASE_CIRC_IN:](#EASE_CIRC_IN:) (const)
- [EASE_CIRC_OUT:](#EASE_CIRC_OUT:) (const)
- [EASE_CIRC_INOUT:](#EASE_CIRC_INOUT:) (const)
- [EASE_BACK_IN:](#EASE_BACK_IN:) (const)
- [EASE_BACK_OUT:](#EASE_BACK_OUT:) (const)
- [EASE_BACK_INOUT:](#EASE_BACK_INOUT:) (const)
- [EASE_ELASTIC_IN:](#EASE_ELASTIC_IN:) (const)
- [EASE_ELASTIC_OUT:](#EASE_ELASTIC_OUT:) (const)
- [EASE_ELASTIC_INOUT:](#EASE_ELASTIC_INOUT:) (const)
- [EASE_BOUNCE_IN:](#EASE_BOUNCE_IN:) (const)
- [EASE_BOUNCE_OUT:](#EASE_BOUNCE_OUT:) (const)
- [EASE_BOUNCE_INOUT:](#EASE_BOUNCE_INOUT:) (const)
- [EASE_COUNT:](#EASE_COUNT:) (const)
- [tween_ease_name](#tween_ease_name) (function)
- [tween_ease](#tween_ease) (function)
- [tween_lerp](#tween_lerp) (function)
- [Tween](#Tween) (struct)
- [tween_new](#tween_new) (function)
- [tween_f64](#tween_f64) (function)
- [tween_vec2](#tween_vec2) (function)
- [tween_vec3](#tween_vec3) (function)
- [tween_set_delay](#tween_set_delay) (function)
- [tween_set_repeat](#tween_set_repeat) (function)
- [tween_total](#tween_total) (function)
- [tween_progress_at](#tween_progress_at) (function)
- [tween_sample_at](#tween_sample_at) (function)
- [tween_seek](#tween_seek) (function)
- [tween_update](#tween_update) (function)
- [tween_restart](#tween_restart) (function)
- [tween_value](#tween_value) (function)
- [tween_value2](#tween_value2) (function)
- [tween_value3](#tween_value3) (function)
- [TWEEN_SEQUENCE:](#TWEEN_SEQUENCE:) (const)
- [TWEEN_PARALLEL:](#TWEEN_PARALLEL:) (const)
- [TweenGroup](#TweenGroup) (struct)
- [tween_group_new](#tween_group_new) (function)
- [tween_group_add](#tween_group_add) (function)
- [tween_group_set_repeat](#tween_group_set_repeat) (function)
- [tween_group_total](#tween_group_total) (function)
- [tween_group_local](#tween_group_local) (function)
- [tween_group_seek](#tween_group_seek) (function)
- [tween_group_update](#tween_group_update) (function)
- [tween_group_value](#tween_group_value) (function)

## EASE_LINEAR:

```fides
pub const EASE_LINEAR: i64 = 0;
```

*Defined at line 29*

---

## EASE_QUAD_IN:

```fides
pub const EASE_QUAD_IN: i64 = 1;
```

*Defined at line 30*

---

## EASE_QUAD_OUT:

```fides
pub const EASE_QUAD_OUT: i64 = 2;
```

*Defined at line 31*

---

## EASE_QUAD_INOUT:

```fides
pub const EASE_QUAD_INOUT: i64 = 3;
```

*Defined at line 32*

---

## EASE_CUBIC_IN:

```fides
pub const EASE_CUBIC_IN: i64 = 4;
```

*Defined at line 33*

---

## EASE_CUBIC_OUT:

```fides
pub const EASE_CUBIC_OUT: i64 = 5;
```

*Defined at line 34*

---

## EASE_CUBIC_INOUT:

```fides
pub const EASE_CUBIC_INOUT: i64 = 6;
```

*Defined at line 35*

---

## EASE_QUART_IN:

```fides
pub const EASE_QUART_IN: i64 = 7;
```

*Defined at line 36*

---

## EASE_QUART_OUT:

```fides
pub const EASE_QUART_OUT: i64 = 8;
```

*Defined at line 37*

---

## EASE_QUART_INOUT:

```fides
pub const EASE_QUART_INOUT: i64 = 9;
```

*Defined at line 38*

---

## EASE_QUINT_IN:

```fides
pub const EASE_QUINT_IN: i64 = 10;
```

*Defined at line 39*

---

## EASE_QUINT_OUT:

```fides
pub const EASE_QUINT_OUT: i64 = 11;
```

*Defined at line 40*

---

## EASE_QUINT_INOUT:

```fides
pub const EASE_QUINT_INOUT: i64 = 12;
```

*Defined at line 41*

---

## EASE_SINE_IN:

```fides
pub const EASE_SINE_IN: i64 = 13;
```

*Defined at line 42*

---

## EASE_SINE_OUT:

```fides
pub const EASE_SINE_OUT: i64 = 14;
```

*Defined at line 43*

---

## EASE_SINE_INOUT:

```fides
pub const EASE_SINE_INOUT: i64 = 15;
```

*Defined at line 44*

---

## EASE_EXPO_IN:

```fides
pub const EASE_EXPO_IN: i64 = 16;
```

*Defined at line 45*

---

## EASE_EXPO_OUT:

```fides
pub const EASE_EXPO_OUT: i64 = 17;
```

*Defined at line 46*

---

## EASE_EXPO_INOUT:

```fides
pub const EASE_EXPO_INOUT: i64 = 18;
```

*Defined at line 47*

---

## EASE_CIRC_IN:

```fides
pub const EASE_CIRC_IN: i64 = 19;
```

*Defined at line 48*

---

## EASE_CIRC_OUT:

```fides
pub const EASE_CIRC_OUT: i64 = 20;
```

*Defined at line 49*

---

## EASE_CIRC_INOUT:

```fides
pub const EASE_CIRC_INOUT: i64 = 21;
```

*Defined at line 50*

---

## EASE_BACK_IN:

```fides
pub const EASE_BACK_IN: i64 = 22;
```

*Defined at line 51*

---

## EASE_BACK_OUT:

```fides
pub const EASE_BACK_OUT: i64 = 23;
```

*Defined at line 52*

---

## EASE_BACK_INOUT:

```fides
pub const EASE_BACK_INOUT: i64 = 24;
```

*Defined at line 53*

---

## EASE_ELASTIC_IN:

```fides
pub const EASE_ELASTIC_IN: i64 = 25;
```

*Defined at line 54*

---

## EASE_ELASTIC_OUT:

```fides
pub const EASE_ELASTIC_OUT: i64 = 26;
```

*Defined at line 55*

---

## EASE_ELASTIC_INOUT:

```fides
pub const EASE_ELASTIC_INOUT: i64 = 27;
```

*Defined at line 56*

---

## EASE_BOUNCE_IN:

```fides
pub const EASE_BOUNCE_IN: i64 = 28;
```

*Defined at line 57*

---

## EASE_BOUNCE_OUT:

```fides
pub const EASE_BOUNCE_OUT: i64 = 29;
```

*Defined at line 58*

---

## EASE_BOUNCE_INOUT:

```fides
pub const EASE_BOUNCE_INOUT: i64 = 30;
```

*Defined at line 59*

---

## EASE_COUNT:

Number of easing kinds (0 .. EASE_COUNT - 1).

```fides
pub const EASE_COUNT: i64 = 31;
```

*Defined at line 61*

---

## tween_ease_name

The name of an easing kind ("quad_in", ...), "linear" for 0.

```fides
pub fn tween_ease_name(kind: i64) -> str {
```

**Parameters:**

- `kind`: `i64`

**Returns:** `str`

*Defined at line 64*

---

## tween_ease

Ease `t` (0..1; clamped) with easing `kind`. The result may leave 0..1
for back and elastic, which overshoot by design.

```fides
pub fn tween_ease(kind: i64, t0: f64) -> f64 {
```

**Parameters:**

- `kind`: `i64`
- `t0`: `f64`

**Returns:** `f64`

*Defined at line 156*

---

## tween_lerp

Linear interpolation a + (b - a) * t.

```fides
pub fn tween_lerp(a: f64, b: f64, t: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 266*

---

## Tween

A tween of `from.len()` components from `from` to `to`.

```fides
pub struct Tween {
```

**Fields:**

| Name | Type |
|------|------|
| `from` | `Vec<f64>` |
| `to` | `Vec<f64>` |
| `value` | `Vec<f64>` |
| `duration` | `f64` |
| `delay` | `f64` |
| `ease` | `i64` |
| `repeat` | `i64` |
| `yoyo` | `bool` |
| `time` | `f64` |
| `done` | `bool` |
| `progress` | `f64,  // eased progress of the current play, 0..1` |

*Defined at line 273*

---

## tween_new

A tween of `from` to `to` (same length) over `duration` seconds.

```fides
pub fn tween_new(from: Vec<f64>, to: Vec<f64>, duration: f64, ease: i64) -> Tween {
```

**Parameters:**

- `from`: `Vec<f64>`
- `to`: `Vec<f64>`
- `duration`: `f64`
- `ease`: `i64`

**Returns:** `Tween`

*Defined at line 292*

---

## tween_f64

A one-component tween.

```fides
pub fn tween_f64(from: f64, to: f64, duration: f64, ease: i64) -> Tween {
```

**Parameters:**

- `from`: `f64`
- `to`: `f64`
- `duration`: `f64`
- `ease`: `i64`

**Returns:** `Tween`

*Defined at line 305*

---

## tween_vec2

A tween of a 2D vector.

```fides
pub fn tween_vec2(from: vec2, to: vec2, duration: f64, ease: i64) -> Tween {
```

**Parameters:**

- `from`: `vec2`
- `to`: `vec2`
- `duration`: `f64`
- `ease`: `i64`

**Returns:** `Tween`

*Defined at line 314*

---

## tween_vec3

A tween of a 3D vector.

```fides
pub fn tween_vec3(from: vec3, to: vec3, duration: f64, ease: i64) -> Tween {
```

**Parameters:**

- `from`: `vec3`
- `to`: `vec3`
- `duration`: `f64`
- `ease`: `i64`

**Returns:** `Tween`

*Defined at line 325*

---

## tween_set_delay

Wait `delay` seconds before the first play.

```fides
pub fn tween_set_delay(tw: &mut Tween, delay: f64) {
```

**Parameters:**

- `tw`: `&mut Tween`
- `delay`: `f64`

*Defined at line 338*

---

## tween_set_repeat

Play `repeat` more times after the first (-1: forever); with `yoyo`
every second play runs from `to` back to `from`.

```fides
pub fn tween_set_repeat(tw: &mut Tween, repeat: i64, yoyo: bool) {
```

**Parameters:**

- `tw`: `&mut Tween`
- `repeat`: `i64`
- `yoyo`: `bool`

*Defined at line 344*

---

## tween_total

Total length in seconds, delay included; -1.0 when it repeats forever.

```fides
pub fn tween_total(tw: &Tween) -> f64 {
```

**Parameters:**

- `tw`: `&Tween`

**Returns:** `f64`

*Defined at line 350*

---

## tween_progress_at

The eased progress (0..1, beyond for back/elastic) at local time `t`.

```fides
pub fn tween_progress_at(tw: &Tween, t: f64) -> f64 {
```

**Parameters:**

- `tw`: `&Tween`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 358*

---

## tween_sample_at

Component `c` of the value at local time `t` (scrubbing: no state).

```fides
pub fn tween_sample_at(tw: &Tween, c: i64, t: f64) -> f64 {
```

**Parameters:**

- `tw`: `&Tween`
- `c`: `i64`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 386*

---

## tween_seek

Move the tween to local time `t` and recompute its value.

```fides
pub fn tween_seek(tw: &mut Tween, t: f64) {
```

**Parameters:**

- `tw`: `&mut Tween`
- `t`: `f64`

*Defined at line 392*

---

## tween_update

Advance by `dt` seconds. True on the update that finishes the tween
(only once; later updates return false).

```fides
pub fn tween_update(tw: &mut Tween, dt: f64) -> bool {
```

**Parameters:**

- `tw`: `&mut Tween`
- `dt`: `f64`

**Returns:** `bool`

*Defined at line 406*

---

## tween_restart

Start again from time 0.

```fides
pub fn tween_restart(tw: &mut Tween) {
```

**Parameters:**

- `tw`: `&mut Tween`

*Defined at line 415*

---

## tween_value

The first component of the current value.

```fides
pub fn tween_value(tw: &Tween) -> f64 {
```

**Parameters:**

- `tw`: `&Tween`

**Returns:** `f64`

*Defined at line 420*

---

## tween_value2

The current value as a vec2 (components 0 and 1).

```fides
pub fn tween_value2(tw: &Tween) -> vec2 {
```

**Parameters:**

- `tw`: `&Tween`

**Returns:** `vec2`

*Defined at line 425*

---

## tween_value3

The current value as a vec3 (components 0, 1 and 2).

```fides
pub fn tween_value3(tw: &Tween) -> vec3 {
```

**Parameters:**

- `tw`: `&Tween`

**Returns:** `vec3`

*Defined at line 430*

---

## TWEEN_SEQUENCE:

```fides
pub const TWEEN_SEQUENCE: i64 = 0;
```

*Defined at line 436*

---

## TWEEN_PARALLEL:

```fides
pub const TWEEN_PARALLEL: i64 = 1;
```

*Defined at line 437*

---

## TweenGroup

Tweens played one after the other (TWEEN_SEQUENCE) or all at once
(TWEEN_PARALLEL). Each member tween's own delay/repeat/yoyo applies
inside its slot; the group repeats and yoyos as a whole.

```fides
pub struct TweenGroup {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `items` | `Vec<Tween>` |
| `starts` | `Vec<f64>` |
| `length` | `f64` |
| `repeat` | `i64` |
| `yoyo` | `bool` |
| `time` | `f64` |
| `done` | `bool` |

*Defined at line 442*

---

## tween_group_new

```fides
pub fn tween_group_new(mode: i64) -> TweenGroup {
```

**Parameters:**

- `mode`: `i64`

**Returns:** `TweenGroup`

*Defined at line 455*

---

## tween_group_add

Add a tween (it must not repeat forever). Returns its index.

```fides
pub fn tween_group_add(g: &mut TweenGroup, tw: Tween) -> i64 {
```

**Parameters:**

- `g`: `&mut TweenGroup`
- `tw`: `Tween`

**Returns:** `i64`

*Defined at line 462*

---

## tween_group_set_repeat

```fides
pub fn tween_group_set_repeat(g: &mut TweenGroup, repeat: i64, yoyo: bool) {
```

**Parameters:**

- `g`: `&mut TweenGroup`
- `repeat`: `i64`
- `yoyo`: `bool`

*Defined at line 480*

---

## tween_group_total

Total length of the group; -1.0 when it repeats forever.

```fides
pub fn tween_group_total(g: &TweenGroup) -> f64 {
```

**Parameters:**

- `g`: `&TweenGroup`

**Returns:** `f64`

*Defined at line 486*

---

## tween_group_local

Time within one play of the group at group time `t` (repeat and yoyo).

```fides
pub fn tween_group_local(g: &TweenGroup, t: f64) -> f64 {
```

**Parameters:**

- `g`: `&TweenGroup`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 494*

---

## tween_group_seek

Move the group to time `t`: every item is set from its own local time.

```fides
pub fn tween_group_seek(g: &mut TweenGroup, t: f64) {
```

**Parameters:**

- `g`: `&mut TweenGroup`
- `t`: `f64`

*Defined at line 513*

---

## tween_group_update

Advance by dt; true on the update that finishes the group (once).

```fides
pub fn tween_group_update(g: &mut TweenGroup, dt: f64) -> bool {
```

**Parameters:**

- `g`: `&mut TweenGroup`
- `dt`: `f64`

**Returns:** `bool`

*Defined at line 528*

---

## tween_group_value

Component `c` of item `i`'s current value.

```fides
pub fn tween_group_value(g: &TweenGroup, i: i64, c: i64) -> f64 {
```

**Parameters:**

- `g`: `&TweenGroup`
- `i`: `i64`
- `c`: `i64`

**Returns:** `f64`

*Defined at line 537*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
