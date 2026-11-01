# touch — Fides Documentation

## Contents

- [TOUCH_NONE:](#touch_none:) (const)
- [TOUCH_BEGAN:](#touch_began:) (const)
- [TOUCH_MOVED:](#touch_moved:) (const)
- [TOUCH_ENDED:](#touch_ended:) (const)
- [TOUCH_CANCELLED:](#touch_cancelled:) (const)
- [MAX_TOUCHES:](#max_touches:) (const)
- [Touch](#touch) (struct)
- [TouchState](#touchstate) (struct)
- [touch_create](#touch_create) (function)
- [touch_init](#touch_init) (function)
- [touch_set_screen_size](#touch_set_screen_size) (function)
- [touch_begin](#touch_begin) (function)
- [touch_move](#touch_move) (function)
- [touch_end](#touch_end) (function)
- [touch_cancel](#touch_cancel) (function)
- [touch_update](#touch_update) (function)
- [touch_count](#touch_count) (function)
- [touch_get](#touch_get) (function)
- [touch_is_tap](#touch_is_tap) (function)
- [touch_tap_position](#touch_tap_position) (function)
- [touch_pinch_active](#touch_pinch_active) (function)
- [touch_pinch_scale](#touch_pinch_scale) (function)
- [touch_pan_active](#touch_pan_active) (function)
- [touch_pan_delta](#touch_pan_delta) (function)
- [touch_swipe_active](#touch_swipe_active) (function)
- [touch_swipe_direction](#touch_swipe_direction) (function)
- [touch_swipe_delta](#touch_swipe_delta) (function)
- [touch_to_pixels](#touch_to_pixels) (function)
- [touch_set_tap_threshold](#touch_set_tap_threshold) (function)
- [touch_set_swipe_threshold](#touch_set_swipe_threshold) (function)
- [touch_clear](#touch_clear) (function)

## TOUCH_NONE:

```fides
pub const TOUCH_NONE: i64 = 0;
```

*Defined at line 21*

---

## TOUCH_BEGAN:

```fides
pub const TOUCH_BEGAN: i64 = 1;
```

*Defined at line 22*

---

## TOUCH_MOVED:

```fides
pub const TOUCH_MOVED: i64 = 2;
```

*Defined at line 23*

---

## TOUCH_ENDED:

```fides
pub const TOUCH_ENDED: i64 = 3;
```

*Defined at line 24*

---

## TOUCH_CANCELLED:

```fides
pub const TOUCH_CANCELLED: i64 = 4;
```

*Defined at line 25*

---

## MAX_TOUCHES:

```fides
pub const MAX_TOUCHES: i64 = 10;
```

*Defined at line 27*

---

## Touch

```fides
pub struct Touch {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `x` | `f64` |
| `y` | `f64` |
| `prev_x` | `f64` |
| `prev_y` | `f64` |
| `start_x` | `f64` |
| `start_y` | `f64` |
| `phase` | `i64` |
| `pressure` | `f64` |
| `timestamp` | `f64` |
| `active` | `bool` |

*Defined at line 31*

---

## TouchState

```fides
pub struct TouchState {
```

**Fields:**

| Name | Type |
|------|------|
| `touches` | `Vec<Touch>` |
| `screen_width` | `f64` |
| `screen_height` | `f64` |
| `tap_max_distance` | `f64` |
| `tap_max_duration` | `f64` |
| `swipe_min_distance` | `f64` |
| `pinch_active` | `bool` |
| `pinch_initial_distance` | `f64` |
| `pinch_current_distance` | `f64` |
| `pan_active` | `bool` |
| `swipe_active` | `bool` |
| `swipe_direction` | `i64` |

*Defined at line 45*

---

## touch_create

Create a new touch state.

```fides
pub fn touch_create() -> TouchState {
```

**Returns:** `TouchState`

*Defined at line 61*

---

## touch_init

Initialize/reset the touch state.

```fides
pub fn touch_init(ts: &mut TouchState) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`

**Returns:** `void`

*Defined at line 94*

---

## touch_set_screen_size

Set the screen dimensions.

```fides
pub fn touch_set_screen_size(ts: &mut TouchState, w: f64, h: f64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `w`: `f64`
- `h`: `f64`

**Returns:** `void`

*Defined at line 116*

---

## touch_begin

Handle a touch begin event.

```fides
pub fn touch_begin(ts: &mut TouchState, id: i64, x: f64, y: f64, pressure: f64, timestamp: f64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `id`: `i64`
- `x`: `f64`
- `y`: `f64`
- `pressure`: `f64`
- `timestamp`: `f64`

**Returns:** `void`

*Defined at line 144*

---

## touch_move

Handle a touch move event.

```fides
pub fn touch_move(ts: &mut TouchState, id: i64, x: f64, y: f64, pressure: f64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `id`: `i64`
- `x`: `f64`
- `y`: `f64`
- `pressure`: `f64`

**Returns:** `void`

*Defined at line 163*

---

## touch_end

Handle a touch end event.

```fides
pub fn touch_end(ts: &mut TouchState, id: i64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `id`: `i64`

**Returns:** `void`

*Defined at line 177*

---

## touch_cancel

Handle a touch cancel event.

```fides
pub fn touch_cancel(ts: &mut TouchState, id: i64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `id`: `i64`

**Returns:** `void`

*Defined at line 186*

---

## touch_update

Update gesture detection. Call after processing all touch events.

```fides
pub fn touch_update(ts: &mut TouchState) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`

**Returns:** `void`

*Defined at line 195*

---

## touch_count

Get the number of active touches.

```fides
pub fn touch_count(ts: &TouchState) -> i64 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `i64`

*Defined at line 269*

---

## touch_get

Get a touch by index (0..count-1).

```fides
pub fn touch_get(ts: &TouchState, idx: i64) -> Touch {
```

**Parameters:**

- `ts`: `&TouchState`
- `idx`: `i64`

**Returns:** `Touch`

*Defined at line 280*

---

## touch_is_tap

Check if a tap occurred.

```fides
pub fn touch_is_tap(ts: &TouchState) -> bool {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `bool`

*Defined at line 298*

---

## touch_tap_position

Get the tap position.

```fides
pub fn touch_tap_position(ts: &TouchState) -> vec2 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `vec2`

*Defined at line 314*

---

## touch_pinch_active

Check if a pinch gesture is active.

```fides
pub fn touch_pinch_active(ts: &TouchState) -> bool {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `bool`

*Defined at line 332*

---

## touch_pinch_scale

Get the pinch scale factor.

```fides
pub fn touch_pinch_scale(ts: &TouchState) -> f64 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `f64`

*Defined at line 337*

---

## touch_pan_active

Check if a pan gesture is active.

```fides
pub fn touch_pan_active(ts: &TouchState) -> bool {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `bool`

*Defined at line 344*

---

## touch_pan_delta

Get the pan delta.

```fides
pub fn touch_pan_delta(ts: &TouchState) -> vec2 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `vec2`

*Defined at line 349*

---

## touch_swipe_active

Check if a swipe just occurred.

```fides
pub fn touch_swipe_active(ts: &TouchState) -> bool {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `bool`

*Defined at line 362*

---

## touch_swipe_direction

Get the swipe direction (0=right, 1=left, 2=down, 3=up).

```fides
pub fn touch_swipe_direction(ts: &TouchState) -> i64 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `i64`

*Defined at line 367*

---

## touch_swipe_delta

Get the swipe delta.

```fides
pub fn touch_swipe_delta(ts: &TouchState) -> vec2 {
```

**Parameters:**

- `ts`: `&TouchState`

**Returns:** `vec2`

*Defined at line 372*

---

## touch_to_pixels

Convert normalized touch coordinates to screen pixels.

```fides
pub fn touch_to_pixels(ts: &TouchState, t: &Touch) -> vec2 {
```

**Parameters:**

- `ts`: `&TouchState`
- `t`: `&Touch`

**Returns:** `vec2`

*Defined at line 385*

---

## touch_set_tap_threshold

Set the tap detection threshold.

```fides
pub fn touch_set_tap_threshold(ts: &mut TouchState, max_dist: f64, max_duration: f64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `max_dist`: `f64`
- `max_duration`: `f64`

**Returns:** `void`

*Defined at line 390*

---

## touch_set_swipe_threshold

Set the swipe detection threshold.

```fides
pub fn touch_set_swipe_threshold(ts: &mut TouchState, min_dist: f64) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`
- `min_dist`: `f64`

**Returns:** `void`

*Defined at line 396*

---

## touch_clear

Clear all touch state.

```fides
pub fn touch_clear(ts: &mut TouchState) -> void {
```

**Parameters:**

- `ts`: `&mut TouchState`

**Returns:** `void`

*Defined at line 401*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
