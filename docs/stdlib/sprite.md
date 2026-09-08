# sprite — Fides Documentation

## Contents

- [Animation](#animation) (struct)
- [sprite_anim_create](#sprite_anim_create) (function)
- [sprite_anim_play](#sprite_anim_play) (function)
- [sprite_anim_stop](#sprite_anim_stop) (function)
- [sprite_anim_set_looping](#sprite_anim_set_looping) (function)
- [sprite_anim_update](#sprite_anim_update) (function)
- [sprite_anim_reset](#sprite_anim_reset) (function)
- [sprite_anim_is_finished](#sprite_anim_is_finished) (function)
- [sprite_anim_get_frame](#sprite_anim_get_frame) (function)
- [sprite_anim_total_duration](#sprite_anim_total_duration) (function)
- [AtlasRegion](#atlasregion) (struct)
- [sprite_region_new](#sprite_region_new) (function)
- [sprite_region_with_origin](#sprite_region_with_origin) (function)
- [Sprite](#sprite) (struct)
- [sprite_create](#sprite_create) (function)
- [sprite_move](#sprite_move) (function)
- [sprite_set_pos](#sprite_set_pos) (function)
- [sprite_set_scale](#sprite_set_scale) (function)
- [sprite_set_rotation](#sprite_set_rotation) (function)
- [sprite_set_visible](#sprite_set_visible) (function)
- [sprite_set_alpha](#sprite_set_alpha) (function)
- [sprite_set_flip_x](#sprite_set_flip_x) (function)
- [sprite_set_flip_y](#sprite_set_flip_y) (function)
- [sprite_set_region](#sprite_set_region) (function)
- [sprite_bounds](#sprite_bounds) (function)
- [PaletteSwap](#paletteswap) (struct)
- [sprite_palette_create](#sprite_palette_create) (function)
- [sprite_palette_add](#sprite_palette_add) (function)
- [sprite_palette_apply](#sprite_palette_apply) (function)
- [sprite_palette_apply_buffer](#sprite_palette_apply_buffer) (function)
- [AnimController](#animcontroller) (struct)
- [sprite_controller_create](#sprite_controller_create) (function)
- [sprite_controller_add](#sprite_controller_add) (function)
- [sprite_controller_play](#sprite_controller_play) (function)
- [sprite_controller_update](#sprite_controller_update) (function)
- [sprite_controller_current_track](#sprite_controller_current_track) (function)

## Animation

A single animation track (a sequence of frames with a frame duration).

```fides
pub struct Animation {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_count` | `i64,      // total number of frames` |
| `frame_duration` | `f64,   // seconds per frame` |
| `current_frame` | `i64,    // current frame index` |
| `elapsed` | `f64,          // time accumulated in current frame` |
| `playing` | `bool,         // is the animation playing?` |
| `looping` | `bool,         // does it loop?` |
| `finished` | `bool,        // has a non-looping animation finished?` |

*Defined at line 17*

---

## sprite_anim_create

Create a new animation with the given frame count and duration per frame.

```fides
pub fn sprite_anim_create(frame_count: i64, frame_duration: f64) -> Animation {
```

**Parameters:**

- `frame_count`: `i64`
- `frame_duration`: `f64`

**Returns:** `Animation`

*Defined at line 28*

---

## sprite_anim_play

Start playing the animation from a specific frame.

```fides
pub fn sprite_anim_play(anim: &mut Animation, start_frame: i64) -> void {
```

**Parameters:**

- `anim`: `&mut Animation`
- `start_frame`: `i64`

**Returns:** `void`

*Defined at line 41*

---

## sprite_anim_stop

Stop the animation.

```fides
pub fn sprite_anim_stop(anim: &mut Animation) -> void {
```

**Parameters:**

- `anim`: `&mut Animation`

**Returns:** `void`

*Defined at line 49*

---

## sprite_anim_set_looping

Set whether the animation loops.

```fides
pub fn sprite_anim_set_looping(anim: &mut Animation, looping: bool) -> void {
```

**Parameters:**

- `anim`: `&mut Animation`
- `looping`: `bool`

**Returns:** `void`

*Defined at line 54*

---

## sprite_anim_update

Advance the animation by dt seconds. Returns the current frame index.

```fides
pub fn sprite_anim_update(anim: &mut Animation, dt: f64) -> i64 {
```

**Parameters:**

- `anim`: `&mut Animation`
- `dt`: `f64`

**Returns:** `i64`

*Defined at line 59*

---

## sprite_anim_reset

Reset the animation to frame 0.

```fides
pub fn sprite_anim_reset(anim: &mut Animation) -> void {
```

**Parameters:**

- `anim`: `&mut Animation`

**Returns:** `void`

*Defined at line 86*

---

## sprite_anim_is_finished

Check if a non-looping animation has finished.

```fides
pub fn sprite_anim_is_finished(anim: Animation) -> bool {
```

**Parameters:**

- `anim`: `Animation`

**Returns:** `bool`

*Defined at line 93*

---

## sprite_anim_get_frame

Get the current frame without advancing.

```fides
pub fn sprite_anim_get_frame(anim: Animation) -> i64 {
```

**Parameters:**

- `anim`: `Animation`

**Returns:** `i64`

*Defined at line 98*

---

## sprite_anim_total_duration

Get the total duration of the animation in seconds.

```fides
pub fn sprite_anim_total_duration(anim: Animation) -> f64 {
```

**Parameters:**

- `anim`: `Animation`

**Returns:** `f64`

*Defined at line 103*

---

## AtlasRegion

```fides
pub struct AtlasRegion {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `i64,        // source x in atlas (pixels)` |
| `y` | `i64,        // source y in atlas (pixels)` |
| `width` | `i64,    // sprite width (pixels)` |
| `height` | `i64,   // sprite height (pixels)` |
| `origin_x` | `i64, // center x for rotation/scaling (relative to top-left)` |
| `origin_y` | `i64, // center y for rotation/scaling` |

*Defined at line 112*

---

## sprite_region_new

Create a default atlas region (top-left origin).

```fides
pub fn sprite_region_new(x: i64, y: i64, width: i64, height: i64) -> AtlasRegion {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`
- `width`: `i64`
- `height`: `i64`

**Returns:** `AtlasRegion`

*Defined at line 122*

---

## sprite_region_with_origin

Create an atlas region with a custom origin point.

```fides
pub fn sprite_region_with_origin(x: i64, y: i64, width: i64, height: i64, origin_x: i64, origin_y: i64) -> AtlasRegion {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`
- `width`: `i64`
- `height`: `i64`
- `origin_x`: `i64`
- `origin_y`: `i64`

**Returns:** `AtlasRegion`

*Defined at line 134*

---

## Sprite

```fides
pub struct Sprite {
```

**Fields:**

| Name | Type |
|------|------|
| `src_rect` | `AtlasRegion,  // source rect in atlas` |
| `pos_x` | `f64,           // world position x` |
| `pos_y` | `f64,           // world position y` |
| `scale_x` | `f64,         // horizontal scale (1.0 = normal)` |
| `scale_y` | `f64,         // vertical scale` |
| `rotation` | `f64,        // rotation in radians` |
| `visible` | `bool,        // is the sprite drawn?` |
| `flip_x` | `bool,         // flip horizontally?` |
| `flip_y` | `bool,         // flip vertically?` |
| `alpha` | `f64,           // opacity (0.0 = transparent, 1.0 = opaque)` |

*Defined at line 147*

---

## sprite_create

Create a sprite from an atlas region at a world position.

```fides
pub fn sprite_create(src_rect: AtlasRegion, x: f64, y: f64) -> Sprite {
```

**Parameters:**

- `src_rect`: `AtlasRegion`
- `x`: `f64`
- `y`: `f64`

**Returns:** `Sprite`

*Defined at line 161*

---

## sprite_move

Move a sprite by (dx, dy).

```fides
pub fn sprite_move(s: &mut Sprite, dx: f64, dy: f64) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `dx`: `f64`
- `dy`: `f64`

**Returns:** `void`

*Defined at line 177*

---

## sprite_set_pos

Set the sprite's position.

```fides
pub fn sprite_set_pos(s: &mut Sprite, x: f64, y: f64) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `x`: `f64`
- `y`: `f64`

**Returns:** `void`

*Defined at line 183*

---

## sprite_set_scale

Set the sprite's scale.

```fides
pub fn sprite_set_scale(s: &mut Sprite, sx: f64, sy: f64) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `sx`: `f64`
- `sy`: `f64`

**Returns:** `void`

*Defined at line 189*

---

## sprite_set_rotation

Set the sprite's rotation (radians).

```fides
pub fn sprite_set_rotation(s: &mut Sprite, rot: f64) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `rot`: `f64`

**Returns:** `void`

*Defined at line 195*

---

## sprite_set_visible

Set the sprite's visibility.

```fides
pub fn sprite_set_visible(s: &mut Sprite, visible: bool) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `visible`: `bool`

**Returns:** `void`

*Defined at line 200*

---

## sprite_set_alpha

Set the sprite's alpha (opacity).

```fides
pub fn sprite_set_alpha(s: &mut Sprite, alpha: f64) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `alpha`: `f64`

**Returns:** `void`

*Defined at line 205*

---

## sprite_set_flip_x

Flip the sprite horizontally.

```fides
pub fn sprite_set_flip_x(s: &mut Sprite, flip: bool) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `flip`: `bool`

**Returns:** `void`

*Defined at line 210*

---

## sprite_set_flip_y

Flip the sprite vertically.

```fides
pub fn sprite_set_flip_y(s: &mut Sprite, flip: bool) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `flip`: `bool`

**Returns:** `void`

*Defined at line 215*

---

## sprite_set_region

Change the atlas region (e.g., for animation frame swapping).

```fides
pub fn sprite_set_region(s: &mut Sprite, src_rect: AtlasRegion) -> void {
```

**Parameters:**

- `s`: `&mut Sprite`
- `src_rect`: `AtlasRegion`

**Returns:** `void`

*Defined at line 220*

---

## sprite_bounds

Get the sprite's bounding box (for collision).

```fides
pub fn sprite_bounds(s: Sprite) -> (f64, f64, f64, f64) {
```

**Parameters:**

- `s`: `Sprite`

**Returns:** `(f64, f64, f64, f64)`

*Defined at line 225*

---

## PaletteSwap

```fides
pub struct PaletteSwap {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<i64>,  // pairs: [old0, new0, old1, new1, ...]` |

*Defined at line 243*

---

## sprite_palette_create

Create an empty palette swap.

```fides
pub fn sprite_palette_create() -> PaletteSwap {
```

**Returns:** `PaletteSwap`

*Defined at line 248*

---

## sprite_palette_add

Add a color mapping: old_color → new_color.

```fides
pub fn sprite_palette_add(palette: &mut PaletteSwap, old_color: i64, new_color: i64) -> void {
```

**Parameters:**

- `palette`: `&mut PaletteSwap`
- `old_color`: `i64`
- `new_color`: `i64`

**Returns:** `void`

*Defined at line 253*

---

## sprite_palette_apply

Apply the palette swap to a single pixel color.
Returns the new color if found, otherwise the original.

```fides
pub fn sprite_palette_apply(palette: PaletteSwap, color: i64) -> i64 {
```

**Parameters:**

- `palette`: `PaletteSwap`
- `color`: `i64`

**Returns:** `i64`

*Defined at line 260*

---

## sprite_palette_apply_buffer

Apply the palette swap to a buffer of pixels (in-place).

```fides
pub fn sprite_palette_apply_buffer(palette: PaletteSwap, pixels: &mut Vec<i64>) -> void {
```

**Parameters:**

- `palette`: `PaletteSwap`
- `pixels`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 272*

---

## AnimController

```fides
pub struct AnimController {
```

**Fields:**

| Name | Type |
|------|------|
| `animations` | `Vec<Animation>` |
| `current` | `i64,  // index of current animation` |

*Defined at line 287*

---

## sprite_controller_create

Create an empty animation controller.

```fides
pub fn sprite_controller_create() -> AnimController {
```

**Returns:** `AnimController`

*Defined at line 293*

---

## sprite_controller_add

Add an animation track to the controller. Returns the track index.

```fides
pub fn sprite_controller_add(controller: &mut AnimController, anim: Animation) -> i64 {
```

**Parameters:**

- `controller`: `&mut AnimController`
- `anim`: `Animation`

**Returns:** `i64`

*Defined at line 301*

---

## sprite_controller_play

Switch to a specific animation track by index.

```fides
pub fn sprite_controller_play(controller: &mut AnimController, track: i64) -> void {
```

**Parameters:**

- `controller`: `&mut AnimController`
- `track`: `i64`

**Returns:** `void`

*Defined at line 308*

---

## sprite_controller_update

Update the current animation. Returns the current frame.

```fides
pub fn sprite_controller_update(controller: &mut AnimController, dt: f64) -> i64 {
```

**Parameters:**

- `controller`: `&mut AnimController`
- `dt`: `f64`

**Returns:** `i64`

*Defined at line 319*

---

## sprite_controller_current_track

Get the current animation track index.

```fides
pub fn sprite_controller_current_track(controller: AnimController) -> i64 {
```

**Parameters:**

- `controller`: `AnimController`

**Returns:** `i64`

*Defined at line 332*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
