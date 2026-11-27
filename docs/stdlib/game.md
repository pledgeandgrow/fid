[All modules](index.md)

# game -- Fides Documentation

## Contents

- [GameConfig](#GameConfig) (struct)
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
- [render_init](#render_init) (function)
- [game_framebuffer](#game_framebuffer) (function)
- [render_clear](#render_clear) (function)
- [render_rect](#render_rect) (function)
- [render_circle](#render_circle) (function)
- [render_text](#render_text) (function)
- [render_present](#render_present) (function)
- [render_present_to](#render_present_to) (function)
- [render_set_output](#render_set_output) (function)
- [compile_script](#compile_script) (function)
- [hot_reload](#hot_reload) (function)
- [physics_move](#physics_move) (function)
- [physics_gravity](#physics_gravity) (function)
- [physics_aabb_overlap](#physics_aabb_overlap) (function)

## GameConfig

Game configuration — timing, window, etc.

```fides
pub struct GameConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `target_fps` | `i64` |
| `frame_time` | `f64` |
| `max_entities` | `i64` |
| `running` | `bool,  // whether the game is running` |

*Defined at line 36*

---

## game_config_new

Create a default game configuration.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val c = game_config_new();
assert_eq(c.target_fps, 60);
val c4 = game_config_with_fps(4);
assert(c4.frame_time == 0.25);
val (x, y) = physics_move(1.0, 2.0, 4.0, -8.0, 0.5);
assert(x == 3.0);
assert(y == -2.0);
assert(physics_aabb_overlap(0.0, 0.0, 2.0, 2.0, 1.0, 1.0, 2.0, 2.0));
assert(!physics_aabb_overlap(0.0, 0.0, 1.0, 1.0, 5.0, 5.0, 1.0, 1.0));
```

```fides
pub fn game_config_new() -> GameConfig {
```

**Returns:** `GameConfig`

*Defined at line 58*

---

## game_config_with_fps

Create a game configuration with a custom FPS.

```fides
pub fn game_config_with_fps(fps: i64) -> GameConfig {
```

**Parameters:**

- `fps`: `i64`

**Returns:** `GameConfig`

*Defined at line 65*

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

*Defined at line 74*

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

*Defined at line 86*

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

*Defined at line 104*

---

## load_texture

Load a texture asset.

```fides
pub fn load_texture(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 113*

---

## load_sound

Load a sound asset.

```fides
pub fn load_sound(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 118*

---

## load_model

Load a 3D model asset.

```fides
pub fn load_model(path: str) -> AssetHandle {
```

**Parameters:**

- `path`: `str`

**Returns:** `AssetHandle`

*Defined at line 123*

---

## input_key_down

Dispatch a key down event.

```fides
pub fn input_key_down(_key: i64) -> void {
```

**Parameters:**

- `_key`: `i64`

**Returns:** `void`

*Defined at line 139*

---

## input_key_up

Dispatch a key up event.

```fides
pub fn input_key_up(_key: i64) -> void {
```

**Parameters:**

- `_key`: `i64`

**Returns:** `void`

*Defined at line 144*

---

## input_mouse_move

Dispatch a mouse move event.

```fides
pub fn input_mouse_move(_x: i64, _y: i64) -> void {
```

**Parameters:**

- `_x`: `i64`
- `_y`: `i64`

**Returns:** `void`

*Defined at line 149*

---

## render_init

Resize the software framebuffer (default 320x240, allocated lazily).

```fides
pub fn render_init(w: i64, h: i64) -> void {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`

**Returns:** `void`

*Defined at line 172*

---

## game_framebuffer

Borrow the framebuffer Image (e.g. to save it yourself).

```fides
pub fn game_framebuffer() -> &Image {
```

**Returns:** `&Image`

*Defined at line 179*

---

## render_clear

Clear the screen with a color (channels are 0.0-1.0 floats).

```fides
pub fn render_clear(r: f64, g: f64, b: f64) -> void {
```

**Parameters:**

- `r`: `f64`
- `g`: `f64`
- `b`: `f64`

**Returns:** `void`

*Defined at line 195*

---

## render_rect

Draw a filled rectangle.

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

*Defined at line 201*

---

## render_circle

Draw a filled circle.

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

*Defined at line 217*

---

## render_text

Draw text using the built-in 8x8 bitmap font.

```fides
pub fn render_text(x: f64, y: f64, text: str) -> void {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `text`: `str`

**Returns:** `void`

*Defined at line 236*

---

## render_present

Present the frame: saves the framebuffer to "frame.png" (or the path
set by render_set_output). Returns file_write_bytes' status.

```fides
pub fn render_present() -> void {
```

**Returns:** `void`

*Defined at line 260*

---

## render_present_to

Present the frame to a specific file (.png or .bmp by extension).

```fides
pub fn render_present_to(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 266*

---

## render_set_output

Set the file render_present() writes to.

```fides
pub fn render_set_output(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 276*

---

## compile_script

Compile a game script. In comptime mode, this generates native code.
For now, it's a no-op (scripts are compiled normally).

```fides
pub fn compile_script(_source: str) -> void {  // In comptime mode, this would generate and compile the script.
```

**Parameters:**

- `_source`: `str`

**Returns:** `void`

*Defined at line 300*

---

## hot_reload

Reload changed systems. Called when a file changes during development.
In production, this is a no-op.

```fides
pub fn hot_reload() -> void {  // Hot-reload is handled by `fidesc watch` at the compiler level.
```

**Returns:** `void`

*Defined at line 312*

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

*Defined at line 321*

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

*Defined at line 328*

---

## physics_aabb_overlap

Check if two axis-aligned bounding boxes overlap.

```fides
pub fn physics_aabb_overlap(x1: f64, y1: f64, w1: f64, h1: f64, x2: f64, y2: f64, w2: f64, h2: f64) -> bool {
```

**Parameters:**

- `x1`: `f64`
- `y1`: `f64`
- `w1`: `f64`
- `h1`: `f64`
- `x2`: `f64`
- `y2`: `f64`
- `w2`: `f64`
- `h2`: `f64`

**Returns:** `bool`

*Defined at line 333*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
