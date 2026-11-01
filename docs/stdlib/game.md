# game — Fides Documentation

## Contents

- [game_config_new](#game_config_new) (function)
- [game_config_with_fps](#game_config_with_fps) (function)
- [game_loop](#game_loop) (function)
- [game_frame](#game_frame) (function)
- [load_asset](#load_asset) (function)
- [load_texture](#load_texture) (function)
- [load_sound](#load_sound) (function)
- [load_model](#load_model) (function)
- [input_key_down](#input_key_down) (function)
- [input_key_up](#input_key_up) (function)
- [input_mouse_move](#input_mouse_move) (function)
- [render_clear](#render_clear) (function)
- [render_rect](#render_rect) (function)
- [render_circle](#render_circle) (function)
- [render_text](#render_text) (function)
- [render_present](#render_present) (function)
- [compile_script](#compile_script) (function)
- [hot_reload](#hot_reload) (function)
- [physics_move](#physics_move) (function)
- [physics_gravity](#physics_gravity) (function)
- [physics_aabb_overlap](#physics_aabb_overlap) (function)

## game_config_new

Create a default game configuration.

```fides
pub fn game_config_new() -> GameConfig {
```

**Returns:** `GameConfig`

*Defined at line 44*

---

## game_config_with_fps

Create a game configuration with a custom FPS.

```fides
pub fn game_config_with_fps(fps: i64) -> GameConfig {
```

**Parameters:**

- `fps`: `i64`

**Returns:** `GameConfig`

*Defined at line 54*

---

## game_loop

Run the game loop. Calls update and render functions each frame.
Uses a frame allocator for per-frame allocations.

```fides
pub fn game_loop(config: GameConfig, update_fn: fn(f64) -> void, render_fn: fn() -> void) -> void {
```

**Parameters:**

- `config`: `GameConfig`
- `update_fn`: `fn(f64) -> void`
- `render_fn`: `fn() -> void`

**Returns:** `void`

*Defined at line 63*

---

## game_frame

Run one frame of the game. Useful for testing.

```fides
pub fn game_frame(config: GameConfig, update_fn: fn(f64) -> void, render_fn: fn() -> void) -> void {
```

**Parameters:**

- `config`: `GameConfig`
- `update_fn`: `fn(f64) -> void`
- `render_fn`: `fn() -> void`

**Returns:** `void`

*Defined at line 75*

---

## load_asset

Load an asset from a JSON file. The asset type is determined by the
caller (via @derive(Deserialize)). Returns an asset handle.

```fides
pub fn load_asset(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 94*

---

## load_texture

Load a texture asset.

```fides
pub fn load_texture(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 103*

---

## load_sound

Load a sound asset.

```fides
pub fn load_sound(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 108*

---

## load_model

Load a 3D model asset.

```fides
pub fn load_model(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 113*

---

## input_key_down

Dispatch a key down event.

```fides
pub fn input_key_down(key: i64) -> void {
```

**Parameters:**

- `key`: `i64`

**Returns:** `void`

*Defined at line 130*

---

## input_key_up

Dispatch a key up event.

```fides
pub fn input_key_up(key: i64) -> void {
```

**Parameters:**

- `key`: `i64`

**Returns:** `void`

*Defined at line 135*

---

## input_mouse_move

Dispatch a mouse move event.

```fides
pub fn input_mouse_move(x: i64, y: i64) -> void {
```

**Parameters:**

- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 140*

---

## render_clear

Clear the screen with a color.

```fides
pub fn render_clear(r: f64, g: f64, b: f64) -> void {
```

**Parameters:**

- `r`: `f64`
- `g`: `f64`
- `b`: `f64`

**Returns:** `void`

*Defined at line 150*

---

## render_rect

Draw a rectangle.

```fides
pub fn render_rect(x: f64, y: f64, w: f64, h: f64, r: f64, g: f64, b: f64) -> void {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `w`: `f64`
- `h`: `f64`
- `r`: `f64`
- `g`: `f64`
- `b`: `f64`

**Returns:** `void`

*Defined at line 156*

---

## render_circle

Draw a circle.

```fides
pub fn render_circle(x: f64, y: f64, radius: f64, r: f64, g: f64, b: f64) -> void {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `radius`: `f64`
- `r`: `f64`
- `g`: `f64`
- `b`: `f64`

**Returns:** `void`

*Defined at line 161*

---

## render_text

Draw text.

```fides
pub fn render_text(x: f64, y: f64, text: str) -> void {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `text`: `str`

**Returns:** `void`

*Defined at line 166*

---

## render_present

Present the rendered frame to the screen.

```fides
pub fn render_present() -> void {
```

**Returns:** `void`

*Defined at line 171*

---

## compile_script

Compile a game script. In comptime mode, this generates native code.
For now, it's a no-op (scripts are compiled normally).

```fides
pub fn compile_script(source: str) -> void {
```

**Parameters:**

- `source`: `str`

**Returns:** `void`

*Defined at line 195*

---

## hot_reload

Reload changed systems. Called when a file changes during development.
In production, this is a no-op.

```fides
pub fn hot_reload() -> void {
```

**Returns:** `void`

*Defined at line 208*

---

## physics_move

Apply velocity to a position over a time step.

```fides
pub fn physics_move(pos_x: f64, pos_y: f64, vel_x: f64, vel_y: f64, dt: f64) -> (f64, f64) {
```

**Parameters:**

- `pos_x`: `f64`
- `pos_y`: `f64`
- `vel_x`: `f64`
- `vel_y`: `f64`
- `dt`: `f64`

**Returns:** `(f64, f64)`

*Defined at line 218*

---

## physics_gravity

Apply gravity to a velocity.

```fides
pub fn physics_gravity(vel_y: f64, gravity: f64, dt: f64) -> f64 {
```

**Parameters:**

- `vel_y`: `f64`
- `gravity`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 225*

---

## physics_aabb_overlap

Check if two axis-aligned bounding boxes overlap.

```fides
pub fn physics_aabb_overlap(x1: f64, y1: f64, w1: f64, h1: f64,
```

**Parameters:**

- `x1`: `f64`
- `y1`: `f64`
- `w1`: `f64`
- `h1`: `f64`

*Defined at line 230*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
