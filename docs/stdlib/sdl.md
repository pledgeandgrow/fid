[All modules](index.md)

# sdl -- Fides Documentation

## Contents

- [SDL_INIT_VIDEO:](#SDL_INIT_VIDEO:) (const)
- [SDL_INIT_AUDIO:](#SDL_INIT_AUDIO:) (const)
- [SDL_INIT_EVENTS:](#SDL_INIT_EVENTS:) (const)
- [SDL_INIT_GAMECONTROLLER:](#SDL_INIT_GAMECONTROLLER:) (const)
- [SDL_WINDOWPOS_CENTERED:](#SDL_WINDOWPOS_CENTERED:) (const)
- [SDL_WINDOWPOS_UNDEFINED:](#SDL_WINDOWPOS_UNDEFINED:) (const)
- [SDL_WINDOW_FULLSCREEN:](#SDL_WINDOW_FULLSCREEN:) (const)
- [SDL_WINDOW_OPENGL:](#SDL_WINDOW_OPENGL:) (const)
- [SDL_WINDOW_RESIZABLE:](#SDL_WINDOW_RESIZABLE:) (const)
- [SDL_WINDOW_SHOWN:](#SDL_WINDOW_SHOWN:) (const)
- [SDL_RENDERER_ACCELERATED:](#SDL_RENDERER_ACCELERATED:) (const)
- [SDL_RENDERER_VSYNC:](#SDL_RENDERER_VSYNC:) (const)
- [SDL_RENDERER_SOFTWARE:](#SDL_RENDERER_SOFTWARE:) (const)
- [SDL_QUIT:](#SDL_QUIT:) (const)
- [SDL_KEYDOWN:](#SDL_KEYDOWN:) (const)
- [SDL_KEYUP:](#SDL_KEYUP:) (const)
- [SDL_MOUSEMOTION:](#SDL_MOUSEMOTION:) (const)
- [SDL_MOUSEBUTTONDOWN:](#SDL_MOUSEBUTTONDOWN:) (const)
- [SDL_MOUSEBUTTONUP:](#SDL_MOUSEBUTTONUP:) (const)
- [SDL_SCANCODE_UNKNOWN:](#SDL_SCANCODE_UNKNOWN:) (const)
- [SDL_SCANCODE_A:](#SDL_SCANCODE_A:) (const)
- [SDL_SCANCODE_B:](#SDL_SCANCODE_B:) (const)
- [SDL_SCANCODE_C:](#SDL_SCANCODE_C:) (const)
- [SDL_SCANCODE_D:](#SDL_SCANCODE_D:) (const)
- [SDL_SCANCODE_E:](#SDL_SCANCODE_E:) (const)
- [SDL_SCANCODE_F:](#SDL_SCANCODE_F:) (const)
- [SDL_SCANCODE_G:](#SDL_SCANCODE_G:) (const)
- [SDL_SCANCODE_H:](#SDL_SCANCODE_H:) (const)
- [SDL_SCANCODE_I:](#SDL_SCANCODE_I:) (const)
- [SDL_SCANCODE_J:](#SDL_SCANCODE_J:) (const)
- [SDL_SCANCODE_K:](#SDL_SCANCODE_K:) (const)
- [SDL_SCANCODE_L:](#SDL_SCANCODE_L:) (const)
- [SDL_SCANCODE_M:](#SDL_SCANCODE_M:) (const)
- [SDL_SCANCODE_N:](#SDL_SCANCODE_N:) (const)
- [SDL_SCANCODE_O:](#SDL_SCANCODE_O:) (const)
- [SDL_SCANCODE_P:](#SDL_SCANCODE_P:) (const)
- [SDL_SCANCODE_Q:](#SDL_SCANCODE_Q:) (const)
- [SDL_SCANCODE_R:](#SDL_SCANCODE_R:) (const)
- [SDL_SCANCODE_S:](#SDL_SCANCODE_S:) (const)
- [SDL_SCANCODE_T:](#SDL_SCANCODE_T:) (const)
- [SDL_SCANCODE_U:](#SDL_SCANCODE_U:) (const)
- [SDL_SCANCODE_V:](#SDL_SCANCODE_V:) (const)
- [SDL_SCANCODE_W:](#SDL_SCANCODE_W:) (const)
- [SDL_SCANCODE_X:](#SDL_SCANCODE_X:) (const)
- [SDL_SCANCODE_Y:](#SDL_SCANCODE_Y:) (const)
- [SDL_SCANCODE_Z:](#SDL_SCANCODE_Z:) (const)
- [SDL_SCANCODE_1:](#SDL_SCANCODE_1:) (const)
- [SDL_SCANCODE_2:](#SDL_SCANCODE_2:) (const)
- [SDL_SCANCODE_3:](#SDL_SCANCODE_3:) (const)
- [SDL_SCANCODE_4:](#SDL_SCANCODE_4:) (const)
- [SDL_SCANCODE_5:](#SDL_SCANCODE_5:) (const)
- [SDL_SCANCODE_6:](#SDL_SCANCODE_6:) (const)
- [SDL_SCANCODE_7:](#SDL_SCANCODE_7:) (const)
- [SDL_SCANCODE_8:](#SDL_SCANCODE_8:) (const)
- [SDL_SCANCODE_9:](#SDL_SCANCODE_9:) (const)
- [SDL_SCANCODE_0:](#SDL_SCANCODE_0:) (const)
- [SDL_SCANCODE_RETURN:](#SDL_SCANCODE_RETURN:) (const)
- [SDL_SCANCODE_ESCAPE:](#SDL_SCANCODE_ESCAPE:) (const)
- [SDL_SCANCODE_BACKSPACE:](#SDL_SCANCODE_BACKSPACE:) (const)
- [SDL_SCANCODE_TAB:](#SDL_SCANCODE_TAB:) (const)
- [SDL_SCANCODE_SPACE:](#SDL_SCANCODE_SPACE:) (const)
- [SDL_SCANCODE_LEFT:](#SDL_SCANCODE_LEFT:) (const)
- [SDL_SCANCODE_RIGHT:](#SDL_SCANCODE_RIGHT:) (const)
- [SDL_SCANCODE_UP:](#SDL_SCANCODE_UP:) (const)
- [SDL_SCANCODE_DOWN:](#SDL_SCANCODE_DOWN:) (const)
- [SDL_SCANCODE_LSHIFT:](#SDL_SCANCODE_LSHIFT:) (const)
- [SDL_SCANCODE_RSHIFT:](#SDL_SCANCODE_RSHIFT:) (const)
- [SDL_SCANCODE_LCTRL:](#SDL_SCANCODE_LCTRL:) (const)
- [SDL_SCANCODE_RCTRL:](#SDL_SCANCODE_RCTRL:) (const)
- [SDL_BUTTON_LEFT:](#SDL_BUTTON_LEFT:) (const)
- [SDL_BUTTON_MIDDLE:](#SDL_BUTTON_MIDDLE:) (const)
- [SDL_BUTTON_RIGHT:](#SDL_BUTTON_RIGHT:) (const)
- [AUDIO_S16SYS:](#AUDIO_S16SYS:) (const)
- [sdl_init](#sdl_init) (function)
- [sdl_quit](#sdl_quit) (function)
- [sdl_create_window](#sdl_create_window) (function)
- [sdl_create_renderer](#sdl_create_renderer) (function)
- [sdl_destroy_renderer](#sdl_destroy_renderer) (function)
- [sdl_destroy_window](#sdl_destroy_window) (function)
- [sdl_render_clear](#sdl_render_clear) (function)
- [sdl_render_present](#sdl_render_present) (function)
- [sdl_set_draw_color](#sdl_set_draw_color) (function)
- [sdl_fill_rect](#sdl_fill_rect) (function)
- [sdl_draw_line](#sdl_draw_line) (function)
- [sdl_draw_point](#sdl_draw_point) (function)
- [sdl_delay](#sdl_delay) (function)
- [sdl_get_ticks](#sdl_get_ticks) (function)
- [sdl_get_error](#sdl_get_error) (function)
- [SdlInputState](#SdlInputState) (struct)
- [sdl_input_create](#sdl_input_create) (function)
- [sdl_poll_events](#sdl_poll_events) (function)
- [sdl_key_held](#sdl_key_held) (function)
- [sdl_key_pressed](#sdl_key_pressed) (function)
- [sdl_key_released](#sdl_key_released) (function)
- [sdl_mouse_held](#sdl_mouse_held) (function)
- [sdl_mouse_pressed](#sdl_mouse_pressed) (function)
- [sdl_mouse_x](#sdl_mouse_x) (function)
- [sdl_mouse_y](#sdl_mouse_y) (function)
- [sdl_quit_requested](#sdl_quit_requested) (function)
- [sdl_load_texture](#sdl_load_texture) (function)
- [sdl_destroy_texture](#sdl_destroy_texture) (function)
- [sdl_draw_texture](#sdl_draw_texture) (function)
- [sdl_audio_init](#sdl_audio_init) (function)
- [sdl_audio_shutdown](#sdl_audio_shutdown) (function)
- [sdl_audio_load](#sdl_audio_load) (function)
- [sdl_audio_play](#sdl_audio_play) (function)
- [sdl_audio_stop](#sdl_audio_stop) (function)
- [sdl_audio_volume](#sdl_audio_volume) (function)
- [sdl_audio_free](#sdl_audio_free) (function)
- [sdl_audio_playing](#sdl_audio_playing) (function)
- [sdl_audio_pause](#sdl_audio_pause) (function)
- [sdl_audio_resume](#sdl_audio_resume) (function)

## SDL_INIT_VIDEO:

```fides
pub const SDL_INIT_VIDEO: i64 = 0x00000020;
```

*Defined at line 92*

---

## SDL_INIT_AUDIO:

```fides
pub const SDL_INIT_AUDIO: i64 = 0x00000010;
```

*Defined at line 93*

---

## SDL_INIT_EVENTS:

```fides
pub const SDL_INIT_EVENTS: i64 = 0x00004000;
```

*Defined at line 94*

---

## SDL_INIT_GAMECONTROLLER:

```fides
pub const SDL_INIT_GAMECONTROLLER: i64 = 0x00002000;
```

*Defined at line 95*

---

## SDL_WINDOWPOS_CENTERED:

```fides
pub const SDL_WINDOWPOS_CENTERED: i64 = 0x2FFF0000;
```

*Defined at line 97*

---

## SDL_WINDOWPOS_UNDEFINED:

```fides
pub const SDL_WINDOWPOS_UNDEFINED: i64 = 0x1FFF0000;
```

*Defined at line 98*

---

## SDL_WINDOW_FULLSCREEN:

```fides
pub const SDL_WINDOW_FULLSCREEN: i64 = 0x00000001;
```

*Defined at line 100*

---

## SDL_WINDOW_OPENGL:

```fides
pub const SDL_WINDOW_OPENGL: i64 = 0x00000002;
```

*Defined at line 101*

---

## SDL_WINDOW_RESIZABLE:

```fides
pub const SDL_WINDOW_RESIZABLE: i64 = 0x00000020;
```

*Defined at line 102*

---

## SDL_WINDOW_SHOWN:

```fides
pub const SDL_WINDOW_SHOWN: i64 = 0x00000004;
```

*Defined at line 103*

---

## SDL_RENDERER_ACCELERATED:

```fides
pub const SDL_RENDERER_ACCELERATED: i64 = 0x00000002;
```

*Defined at line 105*

---

## SDL_RENDERER_VSYNC:

```fides
pub const SDL_RENDERER_VSYNC: i64 = 0x00000004;
```

*Defined at line 106*

---

## SDL_RENDERER_SOFTWARE:

```fides
pub const SDL_RENDERER_SOFTWARE: i64 = 0x00000001;
```

*Defined at line 107*

---

## SDL_QUIT:

```fides
pub const SDL_QUIT: i64 = 0x100;
```

*Defined at line 109*

---

## SDL_KEYDOWN:

```fides
pub const SDL_KEYDOWN: i64 = 0x300;
```

*Defined at line 110*

---

## SDL_KEYUP:

```fides
pub const SDL_KEYUP: i64 = 0x301;
```

*Defined at line 111*

---

## SDL_MOUSEMOTION:

```fides
pub const SDL_MOUSEMOTION: i64 = 0x400;
```

*Defined at line 112*

---

## SDL_MOUSEBUTTONDOWN:

```fides
pub const SDL_MOUSEBUTTONDOWN: i64 = 0x401;
```

*Defined at line 113*

---

## SDL_MOUSEBUTTONUP:

```fides
pub const SDL_MOUSEBUTTONUP: i64 = 0x402;
```

*Defined at line 114*

---

## SDL_SCANCODE_UNKNOWN:

```fides
pub const SDL_SCANCODE_UNKNOWN: i64 = 0;
```

*Defined at line 116*

---

## SDL_SCANCODE_A:

```fides
pub const SDL_SCANCODE_A: i64 = 4;
```

*Defined at line 117*

---

## SDL_SCANCODE_B:

```fides
pub const SDL_SCANCODE_B: i64 = 5;
```

*Defined at line 118*

---

## SDL_SCANCODE_C:

```fides
pub const SDL_SCANCODE_C: i64 = 6;
```

*Defined at line 119*

---

## SDL_SCANCODE_D:

```fides
pub const SDL_SCANCODE_D: i64 = 7;
```

*Defined at line 120*

---

## SDL_SCANCODE_E:

```fides
pub const SDL_SCANCODE_E: i64 = 8;
```

*Defined at line 121*

---

## SDL_SCANCODE_F:

```fides
pub const SDL_SCANCODE_F: i64 = 9;
```

*Defined at line 122*

---

## SDL_SCANCODE_G:

```fides
pub const SDL_SCANCODE_G: i64 = 10;
```

*Defined at line 123*

---

## SDL_SCANCODE_H:

```fides
pub const SDL_SCANCODE_H: i64 = 11;
```

*Defined at line 124*

---

## SDL_SCANCODE_I:

```fides
pub const SDL_SCANCODE_I: i64 = 12;
```

*Defined at line 125*

---

## SDL_SCANCODE_J:

```fides
pub const SDL_SCANCODE_J: i64 = 13;
```

*Defined at line 126*

---

## SDL_SCANCODE_K:

```fides
pub const SDL_SCANCODE_K: i64 = 14;
```

*Defined at line 127*

---

## SDL_SCANCODE_L:

```fides
pub const SDL_SCANCODE_L: i64 = 15;
```

*Defined at line 128*

---

## SDL_SCANCODE_M:

```fides
pub const SDL_SCANCODE_M: i64 = 16;
```

*Defined at line 129*

---

## SDL_SCANCODE_N:

```fides
pub const SDL_SCANCODE_N: i64 = 17;
```

*Defined at line 130*

---

## SDL_SCANCODE_O:

```fides
pub const SDL_SCANCODE_O: i64 = 18;
```

*Defined at line 131*

---

## SDL_SCANCODE_P:

```fides
pub const SDL_SCANCODE_P: i64 = 19;
```

*Defined at line 132*

---

## SDL_SCANCODE_Q:

```fides
pub const SDL_SCANCODE_Q: i64 = 20;
```

*Defined at line 133*

---

## SDL_SCANCODE_R:

```fides
pub const SDL_SCANCODE_R: i64 = 21;
```

*Defined at line 134*

---

## SDL_SCANCODE_S:

```fides
pub const SDL_SCANCODE_S: i64 = 22;
```

*Defined at line 135*

---

## SDL_SCANCODE_T:

```fides
pub const SDL_SCANCODE_T: i64 = 23;
```

*Defined at line 136*

---

## SDL_SCANCODE_U:

```fides
pub const SDL_SCANCODE_U: i64 = 24;
```

*Defined at line 137*

---

## SDL_SCANCODE_V:

```fides
pub const SDL_SCANCODE_V: i64 = 25;
```

*Defined at line 138*

---

## SDL_SCANCODE_W:

```fides
pub const SDL_SCANCODE_W: i64 = 26;
```

*Defined at line 139*

---

## SDL_SCANCODE_X:

```fides
pub const SDL_SCANCODE_X: i64 = 27;
```

*Defined at line 140*

---

## SDL_SCANCODE_Y:

```fides
pub const SDL_SCANCODE_Y: i64 = 28;
```

*Defined at line 141*

---

## SDL_SCANCODE_Z:

```fides
pub const SDL_SCANCODE_Z: i64 = 29;
```

*Defined at line 142*

---

## SDL_SCANCODE_1:

```fides
pub const SDL_SCANCODE_1: i64 = 30;
```

*Defined at line 143*

---

## SDL_SCANCODE_2:

```fides
pub const SDL_SCANCODE_2: i64 = 31;
```

*Defined at line 144*

---

## SDL_SCANCODE_3:

```fides
pub const SDL_SCANCODE_3: i64 = 32;
```

*Defined at line 145*

---

## SDL_SCANCODE_4:

```fides
pub const SDL_SCANCODE_4: i64 = 33;
```

*Defined at line 146*

---

## SDL_SCANCODE_5:

```fides
pub const SDL_SCANCODE_5: i64 = 34;
```

*Defined at line 147*

---

## SDL_SCANCODE_6:

```fides
pub const SDL_SCANCODE_6: i64 = 35;
```

*Defined at line 148*

---

## SDL_SCANCODE_7:

```fides
pub const SDL_SCANCODE_7: i64 = 36;
```

*Defined at line 149*

---

## SDL_SCANCODE_8:

```fides
pub const SDL_SCANCODE_8: i64 = 37;
```

*Defined at line 150*

---

## SDL_SCANCODE_9:

```fides
pub const SDL_SCANCODE_9: i64 = 38;
```

*Defined at line 151*

---

## SDL_SCANCODE_0:

```fides
pub const SDL_SCANCODE_0: i64 = 39;
```

*Defined at line 152*

---

## SDL_SCANCODE_RETURN:

```fides
pub const SDL_SCANCODE_RETURN: i64 = 40;
```

*Defined at line 153*

---

## SDL_SCANCODE_ESCAPE:

```fides
pub const SDL_SCANCODE_ESCAPE: i64 = 41;
```

*Defined at line 154*

---

## SDL_SCANCODE_BACKSPACE:

```fides
pub const SDL_SCANCODE_BACKSPACE: i64 = 42;
```

*Defined at line 155*

---

## SDL_SCANCODE_TAB:

```fides
pub const SDL_SCANCODE_TAB: i64 = 43;
```

*Defined at line 156*

---

## SDL_SCANCODE_SPACE:

```fides
pub const SDL_SCANCODE_SPACE: i64 = 44;
```

*Defined at line 157*

---

## SDL_SCANCODE_LEFT:

```fides
pub const SDL_SCANCODE_LEFT: i64 = 80;
```

*Defined at line 158*

---

## SDL_SCANCODE_RIGHT:

```fides
pub const SDL_SCANCODE_RIGHT: i64 = 79;
```

*Defined at line 159*

---

## SDL_SCANCODE_UP:

```fides
pub const SDL_SCANCODE_UP: i64 = 82;
```

*Defined at line 160*

---

## SDL_SCANCODE_DOWN:

```fides
pub const SDL_SCANCODE_DOWN: i64 = 81;
```

*Defined at line 161*

---

## SDL_SCANCODE_LSHIFT:

```fides
pub const SDL_SCANCODE_LSHIFT: i64 = 225;
```

*Defined at line 162*

---

## SDL_SCANCODE_RSHIFT:

```fides
pub const SDL_SCANCODE_RSHIFT: i64 = 229;
```

*Defined at line 163*

---

## SDL_SCANCODE_LCTRL:

```fides
pub const SDL_SCANCODE_LCTRL: i64 = 224;
```

*Defined at line 164*

---

## SDL_SCANCODE_RCTRL:

```fides
pub const SDL_SCANCODE_RCTRL: i64 = 228;
```

*Defined at line 165*

---

## SDL_BUTTON_LEFT:

```fides
pub const SDL_BUTTON_LEFT: i64 = 1;
```

*Defined at line 167*

---

## SDL_BUTTON_MIDDLE:

```fides
pub const SDL_BUTTON_MIDDLE: i64 = 2;
```

*Defined at line 168*

---

## SDL_BUTTON_RIGHT:

```fides
pub const SDL_BUTTON_RIGHT: i64 = 3;
```

*Defined at line 169*

---

## AUDIO_S16SYS:

```fides
pub const AUDIO_S16SYS: i64 = 0x8010;
```

*Defined at line 171*

---

## sdl_init

Initialize SDL with the given subsystem flags.
Returns 0 on success, -1 on failure.

```fides
pub fn sdl_init(flags: i64) -> i64 {
```

**Parameters:**

- `flags`: `i64`

**Returns:** `i64`

*Defined at line 176*

---

## sdl_quit

Shut down SDL and free all resources.

```fides
pub fn sdl_quit() -> void {
```

**Returns:** `void`

*Defined at line 185*

---

## sdl_create_window

Create a window with centered position.
Returns window handle (i64 pointer) or 0 on failure.

```fides
pub fn sdl_create_window(title: str, width: i64, height: i64) -> i64 {
```

**Parameters:**

- `title`: `str`
- `width`: `i64`
- `height`: `i64`

**Returns:** `i64`

*Defined at line 196*

---

## sdl_create_renderer

Create a hardware-accelerated renderer with vsync.
Returns renderer handle or 0 on failure.

```fides
pub fn sdl_create_renderer(win: i64) -> i64 {
```

**Parameters:**

- `win`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## sdl_destroy_renderer

Destroy a renderer.

```fides
pub fn sdl_destroy_renderer(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 216*

---

## sdl_destroy_window

Destroy a window.

```fides
pub fn sdl_destroy_window(win: i64) -> void {
```

**Parameters:**

- `win`: `i64`

**Returns:** `void`

*Defined at line 225*

---

## sdl_render_clear

Clear the screen with the current draw color.

```fides
pub fn sdl_render_clear(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 234*

---

## sdl_render_present

Present the rendered frame (swap buffers).

```fides
pub fn sdl_render_present(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 243*

---

## sdl_set_draw_color

Set the draw color (r, g, b, a — each 0-255).

```fides
pub fn sdl_set_draw_color(ren: i64, r: i64, g: i64, b: i64, a: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`
- `a`: `i64`

**Returns:** `void`

*Defined at line 252*

---

## sdl_fill_rect

Draw a filled rectangle (pass 0 for rect to fill entire screen).
A nonzero `rect` is the address of an SDL_Rect (four 32-bit ints).

```fides
pub fn sdl_fill_rect(ren: i64, rect: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `rect`: `i64`

**Returns:** `void`

*Defined at line 262*

---

## sdl_draw_line

Draw a line from (x1,y1) to (x2,y2).

```fides
pub fn sdl_draw_line(ren: i64, x1: i64, y1: i64, x2: i64, y2: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `x1`: `i64`
- `y1`: `i64`
- `x2`: `i64`
- `y2`: `i64`

**Returns:** `void`

*Defined at line 272*

---

## sdl_draw_point

Draw a single point at (x, y).

```fides
pub fn sdl_draw_point(ren: i64, x: i64, y: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 280*

---

## sdl_delay

Delay (sleep) for the given milliseconds.

```fides
pub fn sdl_delay(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 288*

---

## sdl_get_ticks

Get ticks (milliseconds since SDL init).

```fides
pub fn sdl_get_ticks() -> i64 {
```

**Returns:** `i64`

*Defined at line 296*

---

## sdl_get_error

Get the last SDL error message.

```fides
pub fn sdl_get_error() -> str {
```

**Returns:** `str`

*Defined at line 304*

---

## SdlInputState

```fides
pub struct SdlInputState {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<i64>` |
| `prev_keys` | `Vec<i64>` |
| `mouse_x` | `i64` |
| `mouse_y` | `i64` |
| `mouse_buttons` | `i64` |
| `prev_mouse_buttons` | `i64` |
| `quit_requested` | `bool` |

*Defined at line 315*

---

## sdl_input_create

Create a new input state tracker.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var st = sdl_input_create();
assert(!sdl_quit_requested(&st));
assert(!sdl_key_held(&st, SDL_SCANCODE_A));
st.keys.set(SDL_SCANCODE_A, 1);
assert(sdl_key_held(&st, SDL_SCANCODE_A));
assert(sdl_key_pressed(&st, SDL_SCANCODE_A));
assert(!sdl_key_released(&st, SDL_SCANCODE_A));
```

```fides
pub fn sdl_input_create() -> SdlInputState {
```

**Returns:** `SdlInputState`

*Defined at line 338*

---

## sdl_poll_events

Poll SDL events and update input state.
Call this once per frame before checking input.

```fides
pub fn sdl_poll_events(state: &mut SdlInputState) -> void {
```

**Parameters:**

- `state`: `&mut SdlInputState`

**Returns:** `void`

*Defined at line 354*

---

## sdl_key_held

Check if a key is currently held down.

```fides
pub fn sdl_key_held(state: &SdlInputState, scancode: i64) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`
- `scancode`: `i64`

**Returns:** `bool`

*Defined at line 473*

---

## sdl_key_pressed

Check if a key was just pressed this frame (held now, not held last frame).

```fides
pub fn sdl_key_pressed(state: &SdlInputState, scancode: i64) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`
- `scancode`: `i64`

**Returns:** `bool`

*Defined at line 481*

---

## sdl_key_released

Check if a key was just released this frame (not held now, held last frame).

```fides
pub fn sdl_key_released(state: &SdlInputState, scancode: i64) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`
- `scancode`: `i64`

**Returns:** `bool`

*Defined at line 489*

---

## sdl_mouse_held

Check if a mouse button is held (button: SDL_BUTTON_LEFT/MIDDLE/RIGHT).

```fides
pub fn sdl_mouse_held(state: &SdlInputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 497*

---

## sdl_mouse_pressed

Check if a mouse button was just pressed.

```fides
pub fn sdl_mouse_pressed(state: &SdlInputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 502*

---

## sdl_mouse_x

Get mouse X position.

```fides
pub fn sdl_mouse_x(state: &SdlInputState) -> i64 {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `i64`

*Defined at line 509*

---

## sdl_mouse_y

Get mouse Y position.

```fides
pub fn sdl_mouse_y(state: &SdlInputState) -> i64 {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `i64`

*Defined at line 514*

---

## sdl_quit_requested

Check if the user requested quit (close button, Alt+F4).

```fides
pub fn sdl_quit_requested(state: &SdlInputState) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `bool`

*Defined at line 519*

---

## sdl_load_texture

Load an image file (PNG, JPG, BMP) as a texture.
Returns texture handle or 0 on failure.

```fides
pub fn sdl_load_texture(ren: i64, path: str) -> i64 {
```

**Parameters:**

- `ren`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 527*

---

## sdl_destroy_texture

Destroy a texture.

```fides
pub fn sdl_destroy_texture(tex: i64) -> void {
```

**Parameters:**

- `tex`: `i64`

**Returns:** `void`

*Defined at line 549*

---

## sdl_draw_texture

Draw a texture at position (x, y) with width w and height h.
Pass 0 for src to draw the whole texture.

```fides
pub fn sdl_draw_texture(ren: i64, tex: i64, x: i64, y: i64, w: i64, h: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `tex`: `i64`
- `x`: `i64`
- `y`: `i64`
- `w`: `i64`
- `h`: `i64`

**Returns:** `void`

*Defined at line 559*

---

## sdl_audio_init

Initialize SDL_mixer audio (44100 Hz, 16-bit, stereo, 1024 chunk).
Returns 0 on success, -1 on failure.

```fides
pub fn sdl_audio_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 581*

---

## sdl_audio_shutdown

Shut down SDL_mixer.

```fides
pub fn sdl_audio_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 590*

---

## sdl_audio_load

Load a WAV file. Returns chunk handle or 0 on failure.

```fides
pub fn sdl_audio_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 599*

---

## sdl_audio_play

Play a sound on a channel. Returns the channel used, or -1 on failure.
Pass -1 for channel to use any free channel.
Pass 0 for loops to play once, -1 for infinite looping.

```fides
pub fn sdl_audio_play(channel: i64, chunk: i64, loops: i64) -> i64 {
```

**Parameters:**

- `channel`: `i64`
- `chunk`: `i64`
- `loops`: `i64`

**Returns:** `i64`

*Defined at line 610*

---

## sdl_audio_stop

Stop playback on a channel. Pass -1 for all channels.

```fides
pub fn sdl_audio_stop(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 620*

---

## sdl_audio_volume

Set volume for a channel (0-128). Pass -1 for all channels.

```fides
pub fn sdl_audio_volume(channel: i64, volume: i64) -> void {
```

**Parameters:**

- `channel`: `i64`
- `volume`: `i64`

**Returns:** `void`

*Defined at line 628*

---

## sdl_audio_free

Free a loaded sound chunk.

```fides
pub fn sdl_audio_free(chunk: i64) -> void {
```

**Parameters:**

- `chunk`: `i64`

**Returns:** `void`

*Defined at line 637*

---

## sdl_audio_playing

Check if a channel is currently playing.

```fides
pub fn sdl_audio_playing(channel: i64) -> bool {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `bool`

*Defined at line 647*

---

## sdl_audio_pause

Pause a channel.

```fides
pub fn sdl_audio_pause(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 657*

---

## sdl_audio_resume

Resume a paused channel.

```fides
pub fn sdl_audio_resume(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 665*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
