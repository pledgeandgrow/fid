# camera — Fides Documentation

## Contents

- [Camera2D](#camera2d) (struct)
- [camera2d_create](#camera2d_create) (function)
- [camera2d_follow](#camera2d_follow) (function)
- [camera2d_set_bounds](#camera2d_set_bounds) (function)
- [camera2d_set_zoom](#camera2d_set_zoom) (function)
- [camera2d_zoom_at](#camera2d_zoom_at) (function)
- [camera2d_shake](#camera2d_shake) (function)
- [camera2d_update](#camera2d_update) (function)
- [camera2d_screen_to_world](#camera2d_screen_to_world) (function)
- [camera2d_world_to_screen](#camera2d_world_to_screen) (function)
- [camera2d_view_offset](#camera2d_view_offset) (function)
- [camera2d_is_visible](#camera2d_is_visible) (function)
- [Camera3D](#camera3d) (struct)
- [camera3d_create](#camera3d_create) (function)
- [camera3d_update](#camera3d_update) (function)
- [camera3d_view_matrix](#camera3d_view_matrix) (function)
- [camera3d_projection](#camera3d_projection) (function)
- [camera3d_view_proj](#camera3d_view_proj) (function)
- [camera3d_forward](#camera3d_forward) (function)
- [camera3d_right](#camera3d_right) (function)
- [camera3d_orbit](#camera3d_orbit) (function)
- [camera3d_move_forward](#camera3d_move_forward) (function)
- [camera3d_move_right](#camera3d_move_right) (function)
- [camera3d_set_aspect](#camera3d_set_aspect) (function)
- [Ray3D](#ray3d) (struct)
- [camera3d_screen_to_ray](#camera3d_screen_to_ray) (function)
- [camera3d_fps_update](#camera3d_fps_update) (function)

## Camera2D

```fides
pub struct Camera2D {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64,              // center of camera in world space` |
| `y` | `f64` |
| `zoom` | `f64,           // scale factor (1.0 = normal)` |
| `rotation` | `f64,       // rotation in radians` |
| `viewport_w` | `i64,     // screen width in pixels` |
| `viewport_h` | `i64,     // screen height in pixels` |
| `target_x` | `f64,       // follow target` |
| `target_y` | `f64` |
| `follow_speed` | `f64,   // lerp speed (0 = instant, 1 = slow)` |
| `shake_x` | `f64,         // current shake offset` |
| `shake_y` | `f64` |
| `shake_amount` | `f64,   // remaining shake intensity` |
| `shake_decay` | `f64,    // shake decay per second` |
| `min_x` | `f64,          // bounds (0 = no limit)` |
| `min_y` | `f64` |
| `max_x` | `f64` |
| `max_y` | `f64` |
| `has_bounds` | `bool` |

*Defined at line 28*

---

## camera2d_create

Create a 2D camera centered at (x, y).

```fides
pub fn camera2d_create(viewport_w: i64, viewport_h: i64) -> Camera2D {
```

**Parameters:**

- `viewport_w`: `i64`
- `viewport_h`: `i64`

**Returns:** `Camera2D`

*Defined at line 50*

---

## camera2d_follow

Set the camera follow target.

```fides
pub fn camera2d_follow(cam: &mut Camera2D, tx: f64, ty: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `tx`: `f64`
- `ty`: `f64`

**Returns:** `void`

*Defined at line 65*

---

## camera2d_set_bounds

Set camera bounds (clamps position).

```fides
pub fn camera2d_set_bounds(cam: &mut Camera2D, min_x: f64, min_y: f64, max_x: f64, max_y: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `min_x`: `f64`
- `min_y`: `f64`
- `max_x`: `f64`
- `max_y`: `f64`

**Returns:** `void`

*Defined at line 71*

---

## camera2d_set_zoom

Set zoom level.

```fides
pub fn camera2d_set_zoom(cam: &mut Camera2D, zoom: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `zoom`: `f64`

**Returns:** `void`

*Defined at line 80*

---

## camera2d_zoom_at

Zoom toward a world point (keeps the point under the cursor).

```fides
pub fn camera2d_zoom_at(cam: &mut Camera2D, world_x: f64, world_y: f64, factor: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `world_x`: `f64`
- `world_y`: `f64`
- `factor`: `f64`

**Returns:** `void`

*Defined at line 86*

---

## camera2d_shake

Trigger screen shake.

```fides
pub fn camera2d_shake(cam: &mut Camera2D, amount: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `amount`: `f64`

**Returns:** `void`

*Defined at line 96*

---

## camera2d_update

Update camera (smooth follow, shake decay).

```fides
pub fn camera2d_update(cam: &mut Camera2D, dt: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera2D`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 101*

---

## camera2d_screen_to_world

Convert screen coordinates to world coordinates.

```fides
pub fn camera2d_screen_to_world(cam: &Camera2D, sx: i64, sy: i64) -> vec2 {
```

**Parameters:**

- `cam`: `&Camera2D`
- `sx`: `i64`
- `sy`: `i64`

**Returns:** `vec2`

*Defined at line 133*

---

## camera2d_world_to_screen

Convert world coordinates to screen coordinates.

```fides
pub fn camera2d_world_to_screen(cam: &Camera2D, wx: f64, wy: f64) -> vec2 {
```

**Parameters:**

- `cam`: `&Camera2D`
- `wx`: `f64`
- `wy`: `f64`

**Returns:** `vec2`

*Defined at line 140*

---

## camera2d_view_offset

Get the camera's view offset for rendering (top-left corner in world space).

```fides
pub fn camera2d_view_offset(cam: &Camera2D) -> vec2 {
```

**Parameters:**

- `cam`: `&Camera2D`

**Returns:** `vec2`

*Defined at line 147*

---

## camera2d_is_visible

Check if a world-space rectangle is visible on screen.

```fides
pub fn camera2d_is_visible(cam: &Camera2D, wx: f64, wy: f64, ww: f64, wh: f64) -> bool {
```

**Parameters:**

- `cam`: `&Camera2D`
- `wx`: `f64`
- `wy`: `f64`
- `ww`: `f64`
- `wh`: `f64`

**Returns:** `bool`

*Defined at line 154*

---

## Camera3D

```fides
pub struct Camera3D {
```

**Fields:**

| Name | Type |
|------|------|
| `position` | `vec3` |
| `target` | `vec3` |
| `up` | `vec3` |
| `fov` | `f64,            // field of view in radians` |
| `aspect` | `f64,         // width / height` |
| `near` | `f64` |
| `far` | `f64` |
| `yaw` | `f64,            // horizontal angle` |
| `pitch` | `f64,          // vertical angle` |
| `view_matrix` | `mat4` |
| `proj_matrix` | `mat4` |
| `dirty` | `bool` |

*Defined at line 164*

---

## camera3d_create

Create a 3D camera.

```fides
pub fn camera3d_create(pos: vec3, target: vec3, fov_deg: f64, aspect: f64, near: f64, far: f64) -> Camera3D {
```

**Parameters:**

- `pos`: `vec3`
- `target`: `vec3`
- `fov_deg`: `f64`
- `aspect`: `f64`
- `near`: `f64`
- `far`: `f64`

**Returns:** `Camera3D`

*Defined at line 180*

---

## camera3d_update

Update the camera's view and projection matrices.

```fides
pub fn camera3d_update(cam: &mut Camera3D) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`

**Returns:** `void`

*Defined at line 198*

---

## camera3d_view_matrix

Get the view matrix.

```fides
pub fn camera3d_view_matrix(cam: &Camera3D) -> mat4 {
```

**Parameters:**

- `cam`: `&Camera3D`

**Returns:** `mat4`

*Defined at line 205*

---

## camera3d_projection

Get the projection matrix.

```fides
pub fn camera3d_projection(cam: &Camera3D) -> mat4 {
```

**Parameters:**

- `cam`: `&Camera3D`

**Returns:** `mat4`

*Defined at line 210*

---

## camera3d_view_proj

Get the combined view-projection matrix.

```fides
pub fn camera3d_view_proj(cam: &Camera3D) -> mat4 {
```

**Parameters:**

- `cam`: `&Camera3D`

**Returns:** `mat4`

*Defined at line 215*

---

## camera3d_forward

Get the camera's forward direction.

```fides
pub fn camera3d_forward(cam: &Camera3D) -> vec3 {
```

**Parameters:**

- `cam`: `&Camera3D`

**Returns:** `vec3`

*Defined at line 220*

---

## camera3d_right

Get the camera's right direction.

```fides
pub fn camera3d_right(cam: &Camera3D) -> vec3 {
```

**Parameters:**

- `cam`: `&Camera3D`

**Returns:** `vec3`

*Defined at line 225*

---

## camera3d_orbit

Orbit the camera around the target by (delta_yaw, delta_pitch).

```fides
pub fn camera3d_orbit(cam: &mut Camera3D, dyaw: f64, dpitch: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`
- `dyaw`: `f64`
- `dpitch`: `f64`

**Returns:** `void`

*Defined at line 231*

---

## camera3d_move_forward

Move the camera forward/backward by distance.

```fides
pub fn camera3d_move_forward(cam: &mut Camera3D, dist: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`
- `dist`: `f64`

**Returns:** `void`

*Defined at line 246*

---

## camera3d_move_right

Move the camera right/left by distance.

```fides
pub fn camera3d_move_right(cam: &mut Camera3D, dist: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`
- `dist`: `f64`

**Returns:** `void`

*Defined at line 254*

---

## camera3d_set_aspect

Set aspect ratio (call on window resize).

```fides
pub fn camera3d_set_aspect(cam: &mut Camera3D, aspect: f64) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`
- `aspect`: `f64`

**Returns:** `void`

*Defined at line 262*

---

## Ray3D

```fides
pub struct Ray3D {
```

**Fields:**

| Name | Type |
|------|------|
| `origin` | `vec3` |
| `direction` | `vec3` |

*Defined at line 267*

---

## camera3d_screen_to_ray

Convert a screen point (0..1) to a 3D ray (origin + direction).

```fides
pub fn camera3d_screen_to_ray(cam: &Camera3D, ndc_x: f64, ndc_y: f64) -> Ray3D {
```

**Parameters:**

- `cam`: `&Camera3D`
- `ndc_x`: `f64`
- `ndc_y`: `f64`

**Returns:** `Ray3D`

*Defined at line 273*

---

## camera3d_fps_update

Update camera position based on yaw/pitch (FPS-style).

```fides
pub fn camera3d_fps_update(cam: &mut Camera3D) -> void {
```

**Parameters:**

- `cam`: `&mut Camera3D`

**Returns:** `void`

*Defined at line 295*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
