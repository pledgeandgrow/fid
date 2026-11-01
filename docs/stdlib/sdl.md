# sdl — Fides Documentation

## Contents

- [SDL_INIT_VIDEO:](#sdl_init_video:) (const)
- [SDL_INIT_AUDIO:](#sdl_init_audio:) (const)
- [SDL_INIT_EVENTS:](#sdl_init_events:) (const)
- [SDL_INIT_GAMECONTROLLER:](#sdl_init_gamecontroller:) (const)
- [SDL_WINDOWPOS_CENTERED:](#sdl_windowpos_centered:) (const)
- [SDL_WINDOWPOS_UNDEFINED:](#sdl_windowpos_undefined:) (const)
- [SDL_WINDOW_FULLSCREEN:](#sdl_window_fullscreen:) (const)
- [SDL_WINDOW_OPENGL:](#sdl_window_opengl:) (const)
- [SDL_WINDOW_RESIZABLE:](#sdl_window_resizable:) (const)
- [SDL_WINDOW_SHOWN:](#sdl_window_shown:) (const)
- [SDL_RENDERER_ACCELERATED:](#sdl_renderer_accelerated:) (const)
- [SDL_RENDERER_VSYNC:](#sdl_renderer_vsync:) (const)
- [SDL_RENDERER_SOFTWARE:](#sdl_renderer_software:) (const)
- [SDL_QUIT:](#sdl_quit:) (const)
- [SDL_KEYDOWN:](#sdl_keydown:) (const)
- [SDL_KEYUP:](#sdl_keyup:) (const)
- [SDL_MOUSEMOTION:](#sdl_mousemotion:) (const)
- [SDL_MOUSEBUTTONDOWN:](#sdl_mousebuttondown:) (const)
- [SDL_MOUSEBUTTONUP:](#sdl_mousebuttonup:) (const)
- [SDL_SCANCODE_UNKNOWN:](#sdl_scancode_unknown:) (const)
- [SDL_SCANCODE_A:](#sdl_scancode_a:) (const)
- [SDL_SCANCODE_B:](#sdl_scancode_b:) (const)
- [SDL_SCANCODE_C:](#sdl_scancode_c:) (const)
- [SDL_SCANCODE_D:](#sdl_scancode_d:) (const)
- [SDL_SCANCODE_E:](#sdl_scancode_e:) (const)
- [SDL_SCANCODE_F:](#sdl_scancode_f:) (const)
- [SDL_SCANCODE_G:](#sdl_scancode_g:) (const)
- [SDL_SCANCODE_H:](#sdl_scancode_h:) (const)
- [SDL_SCANCODE_I:](#sdl_scancode_i:) (const)
- [SDL_SCANCODE_J:](#sdl_scancode_j:) (const)
- [SDL_SCANCODE_K:](#sdl_scancode_k:) (const)
- [SDL_SCANCODE_L:](#sdl_scancode_l:) (const)
- [SDL_SCANCODE_M:](#sdl_scancode_m:) (const)
- [SDL_SCANCODE_N:](#sdl_scancode_n:) (const)
- [SDL_SCANCODE_O:](#sdl_scancode_o:) (const)
- [SDL_SCANCODE_P:](#sdl_scancode_p:) (const)
- [SDL_SCANCODE_Q:](#sdl_scancode_q:) (const)
- [SDL_SCANCODE_R:](#sdl_scancode_r:) (const)
- [SDL_SCANCODE_S:](#sdl_scancode_s:) (const)
- [SDL_SCANCODE_T:](#sdl_scancode_t:) (const)
- [SDL_SCANCODE_U:](#sdl_scancode_u:) (const)
- [SDL_SCANCODE_V:](#sdl_scancode_v:) (const)
- [SDL_SCANCODE_W:](#sdl_scancode_w:) (const)
- [SDL_SCANCODE_X:](#sdl_scancode_x:) (const)
- [SDL_SCANCODE_Y:](#sdl_scancode_y:) (const)
- [SDL_SCANCODE_Z:](#sdl_scancode_z:) (const)
- [SDL_SCANCODE_1:](#sdl_scancode_1:) (const)
- [SDL_SCANCODE_2:](#sdl_scancode_2:) (const)
- [SDL_SCANCODE_3:](#sdl_scancode_3:) (const)
- [SDL_SCANCODE_4:](#sdl_scancode_4:) (const)
- [SDL_SCANCODE_5:](#sdl_scancode_5:) (const)
- [SDL_SCANCODE_6:](#sdl_scancode_6:) (const)
- [SDL_SCANCODE_7:](#sdl_scancode_7:) (const)
- [SDL_SCANCODE_8:](#sdl_scancode_8:) (const)
- [SDL_SCANCODE_9:](#sdl_scancode_9:) (const)
- [SDL_SCANCODE_0:](#sdl_scancode_0:) (const)
- [SDL_SCANCODE_RETURN:](#sdl_scancode_return:) (const)
- [SDL_SCANCODE_ESCAPE:](#sdl_scancode_escape:) (const)
- [SDL_SCANCODE_BACKSPACE:](#sdl_scancode_backspace:) (const)
- [SDL_SCANCODE_TAB:](#sdl_scancode_tab:) (const)
- [SDL_SCANCODE_SPACE:](#sdl_scancode_space:) (const)
- [SDL_SCANCODE_LEFT:](#sdl_scancode_left:) (const)
- [SDL_SCANCODE_RIGHT:](#sdl_scancode_right:) (const)
- [SDL_SCANCODE_UP:](#sdl_scancode_up:) (const)
- [SDL_SCANCODE_DOWN:](#sdl_scancode_down:) (const)
- [SDL_SCANCODE_LSHIFT:](#sdl_scancode_lshift:) (const)
- [SDL_SCANCODE_RSHIFT:](#sdl_scancode_rshift:) (const)
- [SDL_SCANCODE_LCTRL:](#sdl_scancode_lctrl:) (const)
- [SDL_SCANCODE_RCTRL:](#sdl_scancode_rctrl:) (const)
- [SDL_BUTTON_LEFT:](#sdl_button_left:) (const)
- [SDL_BUTTON_MIDDLE:](#sdl_button_middle:) (const)
- [SDL_BUTTON_RIGHT:](#sdl_button_right:) (const)
- [AUDIO_S16SYS:](#audio_s16sys:) (const)
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
- [SdlInputState](#sdlinputstate) (struct)
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

*Defined at line 86*

---

## SDL_INIT_AUDIO:

```fides
pub const SDL_INIT_AUDIO: i64 = 0x00000010;
```

*Defined at line 87*

---

## SDL_INIT_EVENTS:

```fides
pub const SDL_INIT_EVENTS: i64 = 0x00004000;
```

*Defined at line 88*

---

## SDL_INIT_GAMECONTROLLER:

```fides
pub const SDL_INIT_GAMECONTROLLER: i64 = 0x00002000;
```

*Defined at line 89*

---

## SDL_WINDOWPOS_CENTERED:

```fides
pub const SDL_WINDOWPOS_CENTERED: i64 = 0x2FFF0000;
```

*Defined at line 92*

---

## SDL_WINDOWPOS_UNDEFINED:

```fides
pub const SDL_WINDOWPOS_UNDEFINED: i64 = 0x1FFF0000;
```

*Defined at line 93*

---

## SDL_WINDOW_FULLSCREEN:

```fides
pub const SDL_WINDOW_FULLSCREEN: i64 = 0x00000001;
```

*Defined at line 96*

---

## SDL_WINDOW_OPENGL:

```fides
pub const SDL_WINDOW_OPENGL: i64 = 0x00000002;
```

*Defined at line 97*

---

## SDL_WINDOW_RESIZABLE:

```fides
pub const SDL_WINDOW_RESIZABLE: i64 = 0x00000020;
```

*Defined at line 98*

---

## SDL_WINDOW_SHOWN:

```fides
pub const SDL_WINDOW_SHOWN: i64 = 0x00000004;
```

*Defined at line 99*

---

## SDL_RENDERER_ACCELERATED:

```fides
pub const SDL_RENDERER_ACCELERATED: i64 = 0x00000002;
```

*Defined at line 102*

---

## SDL_RENDERER_VSYNC:

```fides
pub const SDL_RENDERER_VSYNC: i64 = 0x00000004;
```

*Defined at line 103*

---

## SDL_RENDERER_SOFTWARE:

```fides
pub const SDL_RENDERER_SOFTWARE: i64 = 0x00000001;
```

*Defined at line 104*

---

## SDL_QUIT:

```fides
pub const SDL_QUIT: i64 = 0x100;
```

*Defined at line 107*

---

## SDL_KEYDOWN:

```fides
pub const SDL_KEYDOWN: i64 = 0x300;
```

*Defined at line 108*

---

## SDL_KEYUP:

```fides
pub const SDL_KEYUP: i64 = 0x301;
```

*Defined at line 109*

---

## SDL_MOUSEMOTION:

```fides
pub const SDL_MOUSEMOTION: i64 = 0x400;
```

*Defined at line 110*

---

## SDL_MOUSEBUTTONDOWN:

```fides
pub const SDL_MOUSEBUTTONDOWN: i64 = 0x401;
```

*Defined at line 111*

---

## SDL_MOUSEBUTTONUP:

```fides
pub const SDL_MOUSEBUTTONUP: i64 = 0x402;
```

*Defined at line 112*

---

## SDL_SCANCODE_UNKNOWN:

```fides
pub const SDL_SCANCODE_UNKNOWN: i64 = 0;
```

*Defined at line 115*

---

## SDL_SCANCODE_A:

```fides
pub const SDL_SCANCODE_A: i64 = 4;
```

*Defined at line 116*

---

## SDL_SCANCODE_B:

```fides
pub const SDL_SCANCODE_B: i64 = 5;
```

*Defined at line 117*

---

## SDL_SCANCODE_C:

```fides
pub const SDL_SCANCODE_C: i64 = 6;
```

*Defined at line 118*

---

## SDL_SCANCODE_D:

```fides
pub const SDL_SCANCODE_D: i64 = 7;
```

*Defined at line 119*

---

## SDL_SCANCODE_E:

```fides
pub const SDL_SCANCODE_E: i64 = 8;
```

*Defined at line 120*

---

## SDL_SCANCODE_F:

```fides
pub const SDL_SCANCODE_F: i64 = 9;
```

*Defined at line 121*

---

## SDL_SCANCODE_G:

```fides
pub const SDL_SCANCODE_G: i64 = 10;
```

*Defined at line 122*

---

## SDL_SCANCODE_H:

```fides
pub const SDL_SCANCODE_H: i64 = 11;
```

*Defined at line 123*

---

## SDL_SCANCODE_I:

```fides
pub const SDL_SCANCODE_I: i64 = 12;
```

*Defined at line 124*

---

## SDL_SCANCODE_J:

```fides
pub const SDL_SCANCODE_J: i64 = 13;
```

*Defined at line 125*

---

## SDL_SCANCODE_K:

```fides
pub const SDL_SCANCODE_K: i64 = 14;
```

*Defined at line 126*

---

## SDL_SCANCODE_L:

```fides
pub const SDL_SCANCODE_L: i64 = 15;
```

*Defined at line 127*

---

## SDL_SCANCODE_M:

```fides
pub const SDL_SCANCODE_M: i64 = 16;
```

*Defined at line 128*

---

## SDL_SCANCODE_N:

```fides
pub const SDL_SCANCODE_N: i64 = 17;
```

*Defined at line 129*

---

## SDL_SCANCODE_O:

```fides
pub const SDL_SCANCODE_O: i64 = 18;
```

*Defined at line 130*

---

## SDL_SCANCODE_P:

```fides
pub const SDL_SCANCODE_P: i64 = 19;
```

*Defined at line 131*

---

## SDL_SCANCODE_Q:

```fides
pub const SDL_SCANCODE_Q: i64 = 20;
```

*Defined at line 132*

---

## SDL_SCANCODE_R:

```fides
pub const SDL_SCANCODE_R: i64 = 21;
```

*Defined at line 133*

---

## SDL_SCANCODE_S:

```fides
pub const SDL_SCANCODE_S: i64 = 22;
```

*Defined at line 134*

---

## SDL_SCANCODE_T:

```fides
pub const SDL_SCANCODE_T: i64 = 23;
```

*Defined at line 135*

---

## SDL_SCANCODE_U:

```fides
pub const SDL_SCANCODE_U: i64 = 24;
```

*Defined at line 136*

---

## SDL_SCANCODE_V:

```fides
pub const SDL_SCANCODE_V: i64 = 25;
```

*Defined at line 137*

---

## SDL_SCANCODE_W:

```fides
pub const SDL_SCANCODE_W: i64 = 26;
```

*Defined at line 138*

---

## SDL_SCANCODE_X:

```fides
pub const SDL_SCANCODE_X: i64 = 27;
```

*Defined at line 139*

---

## SDL_SCANCODE_Y:

```fides
pub const SDL_SCANCODE_Y: i64 = 28;
```

*Defined at line 140*

---

## SDL_SCANCODE_Z:

```fides
pub const SDL_SCANCODE_Z: i64 = 29;
```

*Defined at line 141*

---

## SDL_SCANCODE_1:

```fides
pub const SDL_SCANCODE_1: i64 = 30;
```

*Defined at line 142*

---

## SDL_SCANCODE_2:

```fides
pub const SDL_SCANCODE_2: i64 = 31;
```

*Defined at line 143*

---

## SDL_SCANCODE_3:

```fides
pub const SDL_SCANCODE_3: i64 = 32;
```

*Defined at line 144*

---

## SDL_SCANCODE_4:

```fides
pub const SDL_SCANCODE_4: i64 = 33;
```

*Defined at line 145*

---

## SDL_SCANCODE_5:

```fides
pub const SDL_SCANCODE_5: i64 = 34;
```

*Defined at line 146*

---

## SDL_SCANCODE_6:

```fides
pub const SDL_SCANCODE_6: i64 = 35;
```

*Defined at line 147*

---

## SDL_SCANCODE_7:

```fides
pub const SDL_SCANCODE_7: i64 = 36;
```

*Defined at line 148*

---

## SDL_SCANCODE_8:

```fides
pub const SDL_SCANCODE_8: i64 = 37;
```

*Defined at line 149*

---

## SDL_SCANCODE_9:

```fides
pub const SDL_SCANCODE_9: i64 = 38;
```

*Defined at line 150*

---

## SDL_SCANCODE_0:

```fides
pub const SDL_SCANCODE_0: i64 = 39;
```

*Defined at line 151*

---

## SDL_SCANCODE_RETURN:

```fides
pub const SDL_SCANCODE_RETURN: i64 = 40;
```

*Defined at line 152*

---

## SDL_SCANCODE_ESCAPE:

```fides
pub const SDL_SCANCODE_ESCAPE: i64 = 41;
```

*Defined at line 153*

---

## SDL_SCANCODE_BACKSPACE:

```fides
pub const SDL_SCANCODE_BACKSPACE: i64 = 42;
```

*Defined at line 154*

---

## SDL_SCANCODE_TAB:

```fides
pub const SDL_SCANCODE_TAB: i64 = 43;
```

*Defined at line 155*

---

## SDL_SCANCODE_SPACE:

```fides
pub const SDL_SCANCODE_SPACE: i64 = 44;
```

*Defined at line 156*

---

## SDL_SCANCODE_LEFT:

```fides
pub const SDL_SCANCODE_LEFT: i64 = 80;
```

*Defined at line 157*

---

## SDL_SCANCODE_RIGHT:

```fides
pub const SDL_SCANCODE_RIGHT: i64 = 79;
```

*Defined at line 158*

---

## SDL_SCANCODE_UP:

```fides
pub const SDL_SCANCODE_UP: i64 = 82;
```

*Defined at line 159*

---

## SDL_SCANCODE_DOWN:

```fides
pub const SDL_SCANCODE_DOWN: i64 = 81;
```

*Defined at line 160*

---

## SDL_SCANCODE_LSHIFT:

```fides
pub const SDL_SCANCODE_LSHIFT: i64 = 225;
```

*Defined at line 161*

---

## SDL_SCANCODE_RSHIFT:

```fides
pub const SDL_SCANCODE_RSHIFT: i64 = 229;
```

*Defined at line 162*

---

## SDL_SCANCODE_LCTRL:

```fides
pub const SDL_SCANCODE_LCTRL: i64 = 224;
```

*Defined at line 163*

---

## SDL_SCANCODE_RCTRL:

```fides
pub const SDL_SCANCODE_RCTRL: i64 = 228;
```

*Defined at line 164*

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

*Defined at line 172*

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

*Defined at line 178*

---

## sdl_quit

Shut down SDL and free all resources.

```fides
pub fn sdl_quit() -> void {
```

**Returns:** `void`

*Defined at line 183*

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

*Defined at line 189*

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

*Defined at line 195*

---

## sdl_destroy_renderer

Destroy a renderer.

```fides
pub fn sdl_destroy_renderer(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 200*

---

## sdl_destroy_window

Destroy a window.

```fides
pub fn sdl_destroy_window(win: i64) -> void {
```

**Parameters:**

- `win`: `i64`

**Returns:** `void`

*Defined at line 205*

---

## sdl_render_clear

Clear the screen with the current draw color.

```fides
pub fn sdl_render_clear(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 210*

---

## sdl_render_present

Present the rendered frame (swap buffers).

```fides
pub fn sdl_render_present(ren: i64) -> void {
```

**Parameters:**

- `ren`: `i64`

**Returns:** `void`

*Defined at line 215*

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

*Defined at line 220*

---

## sdl_fill_rect

Draw a filled rectangle (pass 0 for rect to fill entire screen).

```fides
pub fn sdl_fill_rect(ren: i64, rect: i64) -> void {
```

**Parameters:**

- `ren`: `i64`
- `rect`: `i64`

**Returns:** `void`

*Defined at line 225*

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

*Defined at line 230*

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

*Defined at line 235*

---

## sdl_delay

Delay (sleep) for the given milliseconds.

```fides
pub fn sdl_delay(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 240*

---

## sdl_get_ticks

Get ticks (milliseconds since SDL init).

```fides
pub fn sdl_get_ticks() -> i64 {
```

**Returns:** `i64`

*Defined at line 245*

---

## sdl_get_error

Get the last SDL error message.

```fides
pub fn sdl_get_error() -> str {
```

**Returns:** `str`

*Defined at line 250*

---

## SdlInputState

```fides
pub struct SdlInputState {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<i64>,        // 512 scancodes, 1 = held` |
| `prev_keys` | `Vec<i64>,   // previous frame state` |
| `mouse_x` | `i64` |
| `mouse_y` | `i64` |
| `mouse_buttons` | `i64,    // bitmask` |
| `prev_mouse_buttons` | `i64` |
| `quit_requested` | `bool` |

*Defined at line 257*

---

## sdl_input_create

Create a new input state tracker.

```fides
pub fn sdl_input_create() -> SdlInputState {
```

**Returns:** `SdlInputState`

*Defined at line 268*

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

*Defined at line 290*

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

*Defined at line 320*

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

*Defined at line 326*

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

*Defined at line 332*

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

*Defined at line 338*

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

*Defined at line 343*

---

## sdl_mouse_x

Get mouse X position.

```fides
pub fn sdl_mouse_x(state: &SdlInputState) -> i64 {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `i64`

*Defined at line 350*

---

## sdl_mouse_y

Get mouse Y position.

```fides
pub fn sdl_mouse_y(state: &SdlInputState) -> i64 {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `i64`

*Defined at line 355*

---

## sdl_quit_requested

Check if the user requested quit (close button, Alt+F4).

```fides
pub fn sdl_quit_requested(state: &SdlInputState) -> bool {
```

**Parameters:**

- `state`: `&SdlInputState`

**Returns:** `bool`

*Defined at line 360*

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

*Defined at line 368*

---

## sdl_destroy_texture

Destroy a texture.

```fides
pub fn sdl_destroy_texture(tex: i64) -> void {
```

**Parameters:**

- `tex`: `i64`

**Returns:** `void`

*Defined at line 377*

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

*Defined at line 383*

---

## sdl_audio_init

Initialize SDL_mixer audio (44100 Hz, 16-bit, stereo, 1024 chunk).
Returns 0 on success, -1 on failure.

```fides
pub fn sdl_audio_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 398*

---

## sdl_audio_shutdown

Shut down SDL_mixer.

```fides
pub fn sdl_audio_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 403*

---

## sdl_audio_load

Load a WAV file. Returns chunk handle or 0 on failure.

```fides
pub fn sdl_audio_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 408*

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

*Defined at line 415*

---

## sdl_audio_stop

Stop playback on a channel. Pass -1 for all channels.

```fides
pub fn sdl_audio_stop(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 420*

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

*Defined at line 425*

---

## sdl_audio_free

Free a loaded sound chunk.

```fides
pub fn sdl_audio_free(chunk: i64) -> void {
```

**Parameters:**

- `chunk`: `i64`

**Returns:** `void`

*Defined at line 430*

---

## sdl_audio_playing

Check if a channel is currently playing.

```fides
pub fn sdl_audio_playing(channel: i64) -> bool {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `bool`

*Defined at line 435*

---

## sdl_audio_pause

Pause a channel.

```fides
pub fn sdl_audio_pause(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 440*

---

## sdl_audio_resume

Resume a paused channel.

```fides
pub fn sdl_audio_resume(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 445*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
