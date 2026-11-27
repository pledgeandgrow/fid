[All modules](index.md)

# animation2d -- Fides Documentation

## Contents

- [AnimState2D](#AnimState2D) (struct)
- [Animation2D](#Animation2D) (struct)
- [anim2d_create](#anim2d_create) (function)
- [anim2d_add_state](#anim2d_add_state) (function)
- [anim2d_add_state_row](#anim2d_add_state_row) (function)
- [anim2d_find_state](#anim2d_find_state) (function)
- [anim2d_play](#anim2d_play) (function)
- [anim2d_play_idx](#anim2d_play_idx) (function)
- [anim2d_update](#anim2d_update) (function)
- [anim2d_current_frame](#anim2d_current_frame) (function)
- [anim2d_current_row](#anim2d_current_row) (function)
- [anim2d_source_rect](#anim2d_source_rect) (function)
- [anim2d_current_state_name](#anim2d_current_state_name) (function)
- [anim2d_is_finished](#anim2d_is_finished) (function)
- [anim2d_set_speed](#anim2d_set_speed) (function)
- [anim2d_reset](#anim2d_reset) (function)
- [anim2d_state_count](#anim2d_state_count) (function)
- [anim2d_duration](#anim2d_duration) (function)
- [anim2d_progress](#anim2d_progress) (function)

## AnimState2D

```fides
pub struct AnimState2D {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `start_frame` | `i64` |
| `frame_count` | `i64` |
| `frame_time` | `f64` |
| `looping` | `bool` |
| `row` | `i64,  // row in sprite sheet (for grid layout)` |

*Defined at line 22*

---

## Animation2D

```fides
pub struct Animation2D {
```

**Fields:**

| Name | Type |
|------|------|
| `states` | `Vec<AnimState2D>` |
| `current_state` | `i64` |
| `current_frame` | `i64` |
| `elapsed` | `f64` |
| `finished` | `bool` |
| `speed` | `f64` |
| `frame_w` | `i64` |
| `frame_h` | `i64,  // frame height in pixels` |

*Defined at line 34*

---

## anim2d_create

Create an animation system with the given frame dimensions.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = anim2d_create(16, 32);
var idle = anim2d_add_state(&mut a, "idle", 0, 4, 0.25, true);
assert_eq(idle, 0);
assert_eq(anim2d_find_state(&a, "idle"), 0);
assert_eq(anim2d_find_state(&a, "nope"), -1);
assert(anim2d_play(&mut a, "idle"));
assert_eq(anim2d_current_state_name(&a), "idle");
anim2d_update(&mut a, 0.25);
assert_eq(anim2d_current_frame(&a), 1);
assert(anim2d_duration(&a) == 1.0);
assert(!anim2d_is_finished(&a));
```

```fides
pub fn anim2d_create(frame_w: i64, frame_h: i64) -> Animation2D {
```

**Parameters:**

- `frame_w`: `i64`
- `frame_h`: `i64`

**Returns:** `Animation2D`

*Defined at line 65*

---

## anim2d_add_state

Add an animation state.
Returns the state index.

```fides
pub fn anim2d_add_state(anim: &mut Animation2D, name: str, start_frame: i64, frame_count: i64, frame_time: f64, looping: bool) -> i64 {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `name`: `str`
- `start_frame`: `i64`
- `frame_count`: `i64`
- `frame_time`: `f64`
- `looping`: `bool`

**Returns:** `i64`

*Defined at line 73*

---

## anim2d_add_state_row

Add an animation state with a specific row in the sprite sheet.

```fides
pub fn anim2d_add_state_row(anim: &mut Animation2D, name: str, start_frame: i64, frame_count: i64, frame_time: f64, looping: bool, row: i64) -> i64 {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `name`: `str`
- `start_frame`: `i64`
- `frame_count`: `i64`
- `frame_time`: `f64`
- `looping`: `bool`
- `row`: `i64`

**Returns:** `i64`

*Defined at line 82*

---

## anim2d_find_state

Find a state by name. Returns index or -1.

```fides
pub fn anim2d_find_state(anim: &Animation2D, name: str) -> i64 {
```

**Parameters:**

- `anim`: `&Animation2D`
- `name`: `str`

**Returns:** `i64`

*Defined at line 91*

---

## anim2d_play

Play an animation state by name. Returns true if found.

```fides
pub fn anim2d_play(anim: &mut Animation2D, name: str) -> bool {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `name`: `str`

**Returns:** `bool`

*Defined at line 103*

---

## anim2d_play_idx

Play an animation state by index.

```fides
pub fn anim2d_play_idx(anim: &mut Animation2D, idx: i64) -> void {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `idx`: `i64`

**Returns:** `void`

*Defined at line 120*

---

## anim2d_update

Update the animation.

```fides
pub fn anim2d_update(anim: &mut Animation2D, dt: f64) -> void {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 134*

---

## anim2d_current_frame

Get the current frame index (absolute, in the sprite sheet).

```fides
pub fn anim2d_current_frame(anim: &Animation2D) -> i64 {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `i64`

*Defined at line 158*

---

## anim2d_current_row

Get the current row in the sprite sheet.

```fides
pub fn anim2d_current_row(anim: &Animation2D) -> i64 {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `i64`

*Defined at line 167*

---

## anim2d_source_rect

Get the source rectangle (x, y, w, h) in the sprite sheet.

```fides
pub fn anim2d_source_rect(anim: &Animation2D, sheet_cols: i64) -> vec4 {
```

**Parameters:**

- `anim`: `&Animation2D`
- `sheet_cols`: `i64`

**Returns:** `vec4`

*Defined at line 175*

---

## anim2d_current_state_name

Get the name of the current state.

```fides
pub fn anim2d_current_state_name(anim: &Animation2D) -> str {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `str`

*Defined at line 184*

---

## anim2d_is_finished

Check if the current animation has finished.

```fides
pub fn anim2d_is_finished(anim: &Animation2D) -> bool {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `bool`

*Defined at line 192*

---

## anim2d_set_speed

Set playback speed.

```fides
pub fn anim2d_set_speed(anim: &mut Animation2D, speed: f64) -> void {
```

**Parameters:**

- `anim`: `&mut Animation2D`
- `speed`: `f64`

**Returns:** `void`

*Defined at line 197*

---

## anim2d_reset

Reset the current animation to frame 0.

```fides
pub fn anim2d_reset(anim: &mut Animation2D) -> void {
```

**Parameters:**

- `anim`: `&mut Animation2D`

**Returns:** `void`

*Defined at line 205*

---

## anim2d_state_count

Get the number of states.

```fides
pub fn anim2d_state_count(anim: &Animation2D) -> i64 {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `i64`

*Defined at line 212*

---

## anim2d_duration

Get the total duration of the current animation in seconds.

```fides
pub fn anim2d_duration(anim: &Animation2D) -> f64 {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `f64`

*Defined at line 217*

---

## anim2d_progress

Get the progress (0..1) of the current animation.

```fides
pub fn anim2d_progress(anim: &Animation2D) -> f64 {
```

**Parameters:**

- `anim`: `&Animation2D`

**Returns:** `f64`

*Defined at line 226*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
