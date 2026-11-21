# input — Fides Documentation

## Contents

- [KEY_UNKNOWN:](#key_unknown:) (const)
- [KEY_SPACE:](#key_space:) (const)
- [KEY_APOSTROPHE:](#key_apostrophe:) (const)
- [KEY_COMMA:](#key_comma:) (const)
- [KEY_MINUS:](#key_minus:) (const)
- [KEY_PERIOD:](#key_period:) (const)
- [KEY_SLASH:](#key_slash:) (const)
- [KEY_0:](#key_0:) (const)
- [KEY_1:](#key_1:) (const)
- [KEY_2:](#key_2:) (const)
- [KEY_3:](#key_3:) (const)
- [KEY_4:](#key_4:) (const)
- [KEY_5:](#key_5:) (const)
- [KEY_6:](#key_6:) (const)
- [KEY_7:](#key_7:) (const)
- [KEY_8:](#key_8:) (const)
- [KEY_9:](#key_9:) (const)
- [KEY_SEMICOLON:](#key_semicolon:) (const)
- [KEY_EQUAL:](#key_equal:) (const)
- [KEY_A:](#key_a:) (const)
- [KEY_B:](#key_b:) (const)
- [KEY_C:](#key_c:) (const)
- [KEY_D:](#key_d:) (const)
- [KEY_E:](#key_e:) (const)
- [KEY_F:](#key_f:) (const)
- [KEY_G:](#key_g:) (const)
- [KEY_H:](#key_h:) (const)
- [KEY_I:](#key_i:) (const)
- [KEY_J:](#key_j:) (const)
- [KEY_K:](#key_k:) (const)
- [KEY_L:](#key_l:) (const)
- [KEY_M:](#key_m:) (const)
- [KEY_N:](#key_n:) (const)
- [KEY_O:](#key_o:) (const)
- [KEY_P:](#key_p:) (const)
- [KEY_Q:](#key_q:) (const)
- [KEY_R:](#key_r:) (const)
- [KEY_S:](#key_s:) (const)
- [KEY_T:](#key_t:) (const)
- [KEY_U:](#key_u:) (const)
- [KEY_V:](#key_v:) (const)
- [KEY_W:](#key_w:) (const)
- [KEY_X:](#key_x:) (const)
- [KEY_Y:](#key_y:) (const)
- [KEY_Z:](#key_z:) (const)
- [KEY_LEFT_BRACKET:](#key_left_bracket:) (const)
- [KEY_BACKSLASH:](#key_backslash:) (const)
- [KEY_RIGHT_BRACKET:](#key_right_bracket:) (const)
- [KEY_BACKTICK:](#key_backtick:) (const)
- [KEY_ESCAPE:](#key_escape:) (const)
- [KEY_ENTER:](#key_enter:) (const)
- [KEY_TAB:](#key_tab:) (const)
- [KEY_BACKSPACE:](#key_backspace:) (const)
- [KEY_INSERT:](#key_insert:) (const)
- [KEY_DELETE:](#key_delete:) (const)
- [KEY_RIGHT:](#key_right:) (const)
- [KEY_LEFT:](#key_left:) (const)
- [KEY_DOWN:](#key_down:) (const)
- [KEY_UP:](#key_up:) (const)
- [KEY_PAGE_UP:](#key_page_up:) (const)
- [KEY_PAGE_DOWN:](#key_page_down:) (const)
- [KEY_HOME:](#key_home:) (const)
- [KEY_END:](#key_end:) (const)
- [KEY_CAPS_LOCK:](#key_caps_lock:) (const)
- [KEY_SCROLL_LOCK:](#key_scroll_lock:) (const)
- [KEY_NUM_LOCK:](#key_num_lock:) (const)
- [KEY_PRINT_SCREEN:](#key_print_screen:) (const)
- [KEY_PAUSE:](#key_pause:) (const)
- [KEY_F1:](#key_f1:) (const)
- [KEY_F2:](#key_f2:) (const)
- [KEY_F3:](#key_f3:) (const)
- [KEY_F4:](#key_f4:) (const)
- [KEY_F5:](#key_f5:) (const)
- [KEY_F6:](#key_f6:) (const)
- [KEY_F7:](#key_f7:) (const)
- [KEY_F8:](#key_f8:) (const)
- [KEY_F9:](#key_f9:) (const)
- [KEY_F10:](#key_f10:) (const)
- [KEY_F11:](#key_f11:) (const)
- [KEY_F12:](#key_f12:) (const)
- [KEY_LEFT_SHIFT:](#key_left_shift:) (const)
- [KEY_LEFT_CONTROL:](#key_left_control:) (const)
- [KEY_LEFT_ALT:](#key_left_alt:) (const)
- [KEY_LEFT_SUPER:](#key_left_super:) (const)
- [KEY_RIGHT_SHIFT:](#key_right_shift:) (const)
- [KEY_RIGHT_CONTROL:](#key_right_control:) (const)
- [KEY_RIGHT_ALT:](#key_right_alt:) (const)
- [KEY_RIGHT_SUPER:](#key_right_super:) (const)
- [MOUSE_LEFT:](#mouse_left:) (const)
- [MOUSE_RIGHT:](#mouse_right:) (const)
- [MOUSE_MIDDLE:](#mouse_middle:) (const)
- [MOUSE_X1:](#mouse_x1:) (const)
- [MOUSE_X2:](#mouse_x2:) (const)
- [GAMEPAD_A:](#gamepad_a:) (const)
- [GAMEPAD_B:](#gamepad_b:) (const)
- [GAMEPAD_X:](#gamepad_x:) (const)
- [GAMEPAD_Y:](#gamepad_y:) (const)
- [GAMEPAD_LEFT_BUMPER:](#gamepad_left_bumper:) (const)
- [GAMEPAD_RIGHT_BUMPER:](#gamepad_right_bumper:) (const)
- [GAMEPAD_BACK:](#gamepad_back:) (const)
- [GAMEPAD_START:](#gamepad_start:) (const)
- [GAMEPAD_GUIDE:](#gamepad_guide:) (const)
- [GAMEPAD_LEFT_THUMB:](#gamepad_left_thumb:) (const)
- [GAMEPAD_RIGHT_THUMB:](#gamepad_right_thumb:) (const)
- [GAMEPAD_DPAD_UP:](#gamepad_dpad_up:) (const)
- [GAMEPAD_DPAD_RIGHT:](#gamepad_dpad_right:) (const)
- [GAMEPAD_DPAD_DOWN:](#gamepad_dpad_down:) (const)
- [GAMEPAD_DPAD_LEFT:](#gamepad_dpad_left:) (const)
- [InputState](#inputstate) (struct)
- [input_init](#input_init) (function)
- [input_create](#input_create) (function)
- [input_update](#input_update) (function)
- [input_key_held](#input_key_held) (function)
- [input_key_pressed](#input_key_pressed) (function)
- [input_key_released](#input_key_released) (function)
- [input_set_key](#input_set_key) (function)
- [input_mouse_x](#input_mouse_x) (function)
- [input_mouse_y](#input_mouse_y) (function)
- [input_mouse_delta](#input_mouse_delta) (function)
- [input_mouse_wheel](#input_mouse_wheel) (function)
- [input_mouse_held](#input_mouse_held) (function)
- [input_mouse_pressed](#input_mouse_pressed) (function)
- [input_mouse_released](#input_mouse_released) (function)
- [input_set_mouse_pos](#input_set_mouse_pos) (function)
- [input_set_mouse_button](#input_set_mouse_button) (function)
- [input_set_mouse_wheel](#input_set_mouse_wheel) (function)
- [input_gamepad_connected](#input_gamepad_connected) (function)
- [input_gamepad_held](#input_gamepad_held) (function)
- [input_gamepad_pressed](#input_gamepad_pressed) (function)
- [input_gamepad_left_x](#input_gamepad_left_x) (function)
- [input_gamepad_left_y](#input_gamepad_left_y) (function)
- [input_gamepad_right_x](#input_gamepad_right_x) (function)
- [input_gamepad_right_y](#input_gamepad_right_y) (function)
- [input_gamepad_left_trigger](#input_gamepad_left_trigger) (function)
- [input_gamepad_right_trigger](#input_gamepad_right_trigger) (function)
- [input_set_gamepad_button](#input_set_gamepad_button) (function)
- [input_set_gamepad_axes](#input_set_gamepad_axes) (function)
- [input_set_gamepad_connected](#input_set_gamepad_connected) (function)
- [input_shift_held](#input_shift_held) (function)
- [input_ctrl_held](#input_ctrl_held) (function)
- [input_alt_held](#input_alt_held) (function)
- [input_direction](#input_direction) (function)

## KEY_UNKNOWN:

```fides
pub const KEY_UNKNOWN: i64 = 0;
```

*Defined at line 25*

---

## KEY_SPACE:

```fides
pub const KEY_SPACE: i64 = 32;
```

*Defined at line 25*

---

## KEY_APOSTROPHE:

```fides
pub const KEY_APOSTROPHE: i64 = 39;
```

*Defined at line 25*

---

## KEY_COMMA:

```fides
pub const KEY_COMMA: i64 = 44;
```

*Defined at line 25*

---

## KEY_MINUS:

```fides
pub const KEY_MINUS: i64 = 45;
```

*Defined at line 25*

---

## KEY_PERIOD:

```fides
pub const KEY_PERIOD: i64 = 46;
```

*Defined at line 25*

---

## KEY_SLASH:

```fides
pub const KEY_SLASH: i64 = 47;
```

*Defined at line 25*

---

## KEY_0:

```fides
pub const KEY_0: i64 = 48;
```

*Defined at line 25*

---

## KEY_1:

```fides
pub const KEY_1: i64 = 49;
```

*Defined at line 25*

---

## KEY_2:

```fides
pub const KEY_2: i64 = 50;
```

*Defined at line 25*

---

## KEY_3:

```fides
pub const KEY_3: i64 = 51;
```

*Defined at line 25*

---

## KEY_4:

```fides
pub const KEY_4: i64 = 52;
```

*Defined at line 25*

---

## KEY_5:

```fides
pub const KEY_5: i64 = 53;
```

*Defined at line 25*

---

## KEY_6:

```fides
pub const KEY_6: i64 = 54;
```

*Defined at line 25*

---

## KEY_7:

```fides
pub const KEY_7: i64 = 55;
```

*Defined at line 25*

---

## KEY_8:

```fides
pub const KEY_8: i64 = 56;
```

*Defined at line 25*

---

## KEY_9:

```fides
pub const KEY_9: i64 = 57;
```

*Defined at line 25*

---

## KEY_SEMICOLON:

```fides
pub const KEY_SEMICOLON: i64 = 59;
```

*Defined at line 25*

---

## KEY_EQUAL:

```fides
pub const KEY_EQUAL: i64 = 61;
```

*Defined at line 25*

---

## KEY_A:

```fides
pub const KEY_A: i64 = 65;
```

*Defined at line 25*

---

## KEY_B:

```fides
pub const KEY_B: i64 = 66;
```

*Defined at line 25*

---

## KEY_C:

```fides
pub const KEY_C: i64 = 67;
```

*Defined at line 25*

---

## KEY_D:

```fides
pub const KEY_D: i64 = 68;
```

*Defined at line 25*

---

## KEY_E:

```fides
pub const KEY_E: i64 = 69;
```

*Defined at line 25*

---

## KEY_F:

```fides
pub const KEY_F: i64 = 70;
```

*Defined at line 25*

---

## KEY_G:

```fides
pub const KEY_G: i64 = 71;
```

*Defined at line 25*

---

## KEY_H:

```fides
pub const KEY_H: i64 = 72;
```

*Defined at line 25*

---

## KEY_I:

```fides
pub const KEY_I: i64 = 73;
```

*Defined at line 25*

---

## KEY_J:

```fides
pub const KEY_J: i64 = 74;
```

*Defined at line 25*

---

## KEY_K:

```fides
pub const KEY_K: i64 = 75;
```

*Defined at line 25*

---

## KEY_L:

```fides
pub const KEY_L: i64 = 76;
```

*Defined at line 25*

---

## KEY_M:

```fides
pub const KEY_M: i64 = 77;
```

*Defined at line 25*

---

## KEY_N:

```fides
pub const KEY_N: i64 = 78;
```

*Defined at line 25*

---

## KEY_O:

```fides
pub const KEY_O: i64 = 79;
```

*Defined at line 25*

---

## KEY_P:

```fides
pub const KEY_P: i64 = 80;
```

*Defined at line 25*

---

## KEY_Q:

```fides
pub const KEY_Q: i64 = 81;
```

*Defined at line 25*

---

## KEY_R:

```fides
pub const KEY_R: i64 = 82;
```

*Defined at line 25*

---

## KEY_S:

```fides
pub const KEY_S: i64 = 83;
```

*Defined at line 25*

---

## KEY_T:

```fides
pub const KEY_T: i64 = 84;
```

*Defined at line 25*

---

## KEY_U:

```fides
pub const KEY_U: i64 = 85;
```

*Defined at line 25*

---

## KEY_V:

```fides
pub const KEY_V: i64 = 86;
```

*Defined at line 25*

---

## KEY_W:

```fides
pub const KEY_W: i64 = 87;
```

*Defined at line 25*

---

## KEY_X:

```fides
pub const KEY_X: i64 = 88;
```

*Defined at line 25*

---

## KEY_Y:

```fides
pub const KEY_Y: i64 = 89;
```

*Defined at line 25*

---

## KEY_Z:

```fides
pub const KEY_Z: i64 = 90;
```

*Defined at line 25*

---

## KEY_LEFT_BRACKET:

```fides
pub const KEY_LEFT_BRACKET: i64 = 91;
```

*Defined at line 25*

---

## KEY_BACKSLASH:

```fides
pub const KEY_BACKSLASH: i64 = 92;
```

*Defined at line 25*

---

## KEY_RIGHT_BRACKET:

```fides
pub const KEY_RIGHT_BRACKET: i64 = 93;
```

*Defined at line 25*

---

## KEY_BACKTICK:

```fides
pub const KEY_BACKTICK: i64 = 96;
```

*Defined at line 25*

---

## KEY_ESCAPE:

```fides
pub const KEY_ESCAPE: i64 = 256;
```

*Defined at line 25*

---

## KEY_ENTER:

```fides
pub const KEY_ENTER: i64 = 257;
```

*Defined at line 25*

---

## KEY_TAB:

```fides
pub const KEY_TAB: i64 = 258;
```

*Defined at line 25*

---

## KEY_BACKSPACE:

```fides
pub const KEY_BACKSPACE: i64 = 259;
```

*Defined at line 25*

---

## KEY_INSERT:

```fides
pub const KEY_INSERT: i64 = 260;
```

*Defined at line 25*

---

## KEY_DELETE:

```fides
pub const KEY_DELETE: i64 = 261;
```

*Defined at line 25*

---

## KEY_RIGHT:

```fides
pub const KEY_RIGHT: i64 = 262;
```

*Defined at line 25*

---

## KEY_LEFT:

```fides
pub const KEY_LEFT: i64 = 263;
```

*Defined at line 25*

---

## KEY_DOWN:

```fides
pub const KEY_DOWN: i64 = 264;
```

*Defined at line 25*

---

## KEY_UP:

```fides
pub const KEY_UP: i64 = 265;
```

*Defined at line 25*

---

## KEY_PAGE_UP:

```fides
pub const KEY_PAGE_UP: i64 = 266;
```

*Defined at line 25*

---

## KEY_PAGE_DOWN:

```fides
pub const KEY_PAGE_DOWN: i64 = 267;
```

*Defined at line 25*

---

## KEY_HOME:

```fides
pub const KEY_HOME: i64 = 268;
```

*Defined at line 25*

---

## KEY_END:

```fides
pub const KEY_END: i64 = 269;
```

*Defined at line 25*

---

## KEY_CAPS_LOCK:

```fides
pub const KEY_CAPS_LOCK: i64 = 280;
```

*Defined at line 25*

---

## KEY_SCROLL_LOCK:

```fides
pub const KEY_SCROLL_LOCK: i64 = 281;
```

*Defined at line 25*

---

## KEY_NUM_LOCK:

```fides
pub const KEY_NUM_LOCK: i64 = 282;
```

*Defined at line 25*

---

## KEY_PRINT_SCREEN:

```fides
pub const KEY_PRINT_SCREEN: i64 = 283;
```

*Defined at line 25*

---

## KEY_PAUSE:

```fides
pub const KEY_PAUSE: i64 = 284;
```

*Defined at line 25*

---

## KEY_F1:

```fides
pub const KEY_F1: i64 = 290;
```

*Defined at line 25*

---

## KEY_F2:

```fides
pub const KEY_F2: i64 = 291;
```

*Defined at line 25*

---

## KEY_F3:

```fides
pub const KEY_F3: i64 = 292;
```

*Defined at line 25*

---

## KEY_F4:

```fides
pub const KEY_F4: i64 = 293;
```

*Defined at line 25*

---

## KEY_F5:

```fides
pub const KEY_F5: i64 = 294;
```

*Defined at line 25*

---

## KEY_F6:

```fides
pub const KEY_F6: i64 = 295;
```

*Defined at line 25*

---

## KEY_F7:

```fides
pub const KEY_F7: i64 = 296;
```

*Defined at line 25*

---

## KEY_F8:

```fides
pub const KEY_F8: i64 = 297;
```

*Defined at line 25*

---

## KEY_F9:

```fides
pub const KEY_F9: i64 = 298;
```

*Defined at line 25*

---

## KEY_F10:

```fides
pub const KEY_F10: i64 = 299;
```

*Defined at line 25*

---

## KEY_F11:

```fides
pub const KEY_F11: i64 = 300;
```

*Defined at line 25*

---

## KEY_F12:

```fides
pub const KEY_F12: i64 = 301;
```

*Defined at line 25*

---

## KEY_LEFT_SHIFT:

```fides
pub const KEY_LEFT_SHIFT: i64 = 340;
```

*Defined at line 25*

---

## KEY_LEFT_CONTROL:

```fides
pub const KEY_LEFT_CONTROL: i64 = 341;
```

*Defined at line 25*

---

## KEY_LEFT_ALT:

```fides
pub const KEY_LEFT_ALT: i64 = 342;
```

*Defined at line 25*

---

## KEY_LEFT_SUPER:

```fides
pub const KEY_LEFT_SUPER: i64 = 343;
```

*Defined at line 25*

---

## KEY_RIGHT_SHIFT:

```fides
pub const KEY_RIGHT_SHIFT: i64 = 344;
```

*Defined at line 25*

---

## KEY_RIGHT_CONTROL:

```fides
pub const KEY_RIGHT_CONTROL: i64 = 345;
```

*Defined at line 25*

---

## KEY_RIGHT_ALT:

```fides
pub const KEY_RIGHT_ALT: i64 = 346;
```

*Defined at line 25*

---

## KEY_RIGHT_SUPER:

```fides
pub const KEY_RIGHT_SUPER: i64 = 347;
```

*Defined at line 25*

---

## MOUSE_LEFT:

```fides
pub const MOUSE_LEFT: i64 = 0;
```

*Defined at line 28*

---

## MOUSE_RIGHT:

```fides
pub const MOUSE_RIGHT: i64 = 1;
```

*Defined at line 28*

---

## MOUSE_MIDDLE:

```fides
pub const MOUSE_MIDDLE: i64 = 2;
```

*Defined at line 28*

---

## MOUSE_X1:

```fides
pub const MOUSE_X1: i64 = 3;
```

*Defined at line 28*

---

## MOUSE_X2:

```fides
pub const MOUSE_X2: i64 = 4;
```

*Defined at line 28*

---

## GAMEPAD_A:

```fides
pub const GAMEPAD_A: i64 = 0;
```

*Defined at line 31*

---

## GAMEPAD_B:

```fides
pub const GAMEPAD_B: i64 = 1;
```

*Defined at line 31*

---

## GAMEPAD_X:

```fides
pub const GAMEPAD_X: i64 = 2;
```

*Defined at line 31*

---

## GAMEPAD_Y:

```fides
pub const GAMEPAD_Y: i64 = 3;
```

*Defined at line 31*

---

## GAMEPAD_LEFT_BUMPER:

```fides
pub const GAMEPAD_LEFT_BUMPER: i64 = 4;
```

*Defined at line 31*

---

## GAMEPAD_RIGHT_BUMPER:

```fides
pub const GAMEPAD_RIGHT_BUMPER: i64 = 5;
```

*Defined at line 31*

---

## GAMEPAD_BACK:

```fides
pub const GAMEPAD_BACK: i64 = 6;
```

*Defined at line 31*

---

## GAMEPAD_START:

```fides
pub const GAMEPAD_START: i64 = 7;
```

*Defined at line 31*

---

## GAMEPAD_GUIDE:

```fides
pub const GAMEPAD_GUIDE: i64 = 8;
```

*Defined at line 31*

---

## GAMEPAD_LEFT_THUMB:

```fides
pub const GAMEPAD_LEFT_THUMB: i64 = 9;
```

*Defined at line 31*

---

## GAMEPAD_RIGHT_THUMB:

```fides
pub const GAMEPAD_RIGHT_THUMB: i64 = 10;
```

*Defined at line 31*

---

## GAMEPAD_DPAD_UP:

```fides
pub const GAMEPAD_DPAD_UP: i64 = 11;
```

*Defined at line 31*

---

## GAMEPAD_DPAD_RIGHT:

```fides
pub const GAMEPAD_DPAD_RIGHT: i64 = 12;
```

*Defined at line 31*

---

## GAMEPAD_DPAD_DOWN:

```fides
pub const GAMEPAD_DPAD_DOWN: i64 = 13;
```

*Defined at line 31*

---

## GAMEPAD_DPAD_LEFT:

```fides
pub const GAMEPAD_DPAD_LEFT: i64 = 14;
```

*Defined at line 31*

---

## InputState

```fides
pub struct InputState {
```

**Fields:**

| Name | Type |
|------|------|
| `key_current` | `Vec<i64>,    // 512 keys` |
| `key_previous` | `Vec<i64>` |
| `mouse_x` | `i64` |
| `mouse_y` | `i64` |
| `mouse_delta_x` | `i64` |
| `mouse_delta_y` | `i64` |
| `mouse_current` | `Vec<i64>,  // 5 buttons` |
| `mouse_previous` | `Vec<i64>` |
| `mouse_wheel` | `i64,          // scroll wheel delta` |
| `gamepad_connected` | `bool` |
| `gamepad_current` | `Vec<i64>,  // 15 buttons` |
| `gamepad_previous` | `Vec<i64>` |
| `gamepad_left_x` | `f64,       // -1.0 to 1.0` |
| `gamepad_left_y` | `f64` |
| `gamepad_right_x` | `f64` |
| `gamepad_right_y` | `f64` |
| `gamepad_left_trigger` | `f64,  // 0.0 to 1.0` |
| `gamepad_right_trigger` | `f64` |

*Defined at line 40*

---

## input_init

Initialize the input system.

```fides
pub fn input_init() -> void {
```

**Returns:** `void`

*Defined at line 87*

---

## input_create

Create a new input state tracker.

```fides
pub fn input_create() -> InputState {
```

**Returns:** `InputState`

*Defined at line 100*

---

## input_update

Update the input state. Call this once per frame before querying input.
Copies current state to previous, then polls the platform for new state.

```fides
pub fn input_update(state: &mut InputState) -> void {
```

**Parameters:**

- `state`: `&mut InputState`

**Returns:** `void`

*Defined at line 137*

---

## input_key_held

Check if a key is currently held down.

```fides
pub fn input_key_held(state: InputState, key: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 173*

---

## input_key_pressed

Check if a key was just pressed this frame (not held from previous frame).

```fides
pub fn input_key_pressed(state: InputState, key: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 181*

---

## input_key_released

Check if a key was just released this frame.

```fides
pub fn input_key_released(state: InputState, key: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 189*

---

## input_set_key

Set a key state (for testing or platform integration).

```fides
pub fn input_set_key(state: &mut InputState, key: i64, held: bool) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `key`: `i64`
- `held`: `bool`

**Returns:** `void`

*Defined at line 197*

---

## input_mouse_x

Get mouse X position.

```fides
pub fn input_mouse_x(state: InputState) -> i64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `i64`

*Defined at line 211*

---

## input_mouse_y

Get mouse Y position.

```fides
pub fn input_mouse_y(state: InputState) -> i64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `i64`

*Defined at line 216*

---

## input_mouse_delta

Get mouse movement delta (change since last frame).

```fides
pub fn input_mouse_delta(state: InputState) -> (i64, i64) {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `(i64, i64)`

*Defined at line 221*

---

## input_mouse_wheel

Get mouse wheel scroll delta.

```fides
pub fn input_mouse_wheel(state: InputState) -> i64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `i64`

*Defined at line 226*

---

## input_mouse_held

Check if a mouse button is held.

```fides
pub fn input_mouse_held(state: InputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 231*

---

## input_mouse_pressed

Check if a mouse button was just pressed this frame.

```fides
pub fn input_mouse_pressed(state: InputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 239*

---

## input_mouse_released

Check if a mouse button was just released this frame.

```fides
pub fn input_mouse_released(state: InputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 247*

---

## input_set_mouse_pos

Set mouse position (for platform integration).

```fides
pub fn input_set_mouse_pos(state: &mut InputState, x: i64, y: i64) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 255*

---

## input_set_mouse_button

Set mouse button state (for platform integration).

```fides
pub fn input_set_mouse_button(state: &mut InputState, button: i64, held: bool) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `button`: `i64`
- `held`: `bool`

**Returns:** `void`

*Defined at line 263*

---

## input_set_mouse_wheel

Set mouse wheel delta (for platform integration).

```fides
pub fn input_set_mouse_wheel(state: &mut InputState, delta: i64) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `delta`: `i64`

**Returns:** `void`

*Defined at line 275*

---

## input_gamepad_connected

Check if a gamepad is connected.

```fides
pub fn input_gamepad_connected(state: InputState) -> bool {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `bool`

*Defined at line 282*

---

## input_gamepad_held

Check if a gamepad button is held.

```fides
pub fn input_gamepad_held(state: InputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 287*

---

## input_gamepad_pressed

Check if a gamepad button was just pressed.

```fides
pub fn input_gamepad_pressed(state: InputState, button: i64) -> bool {
```

**Parameters:**

- `state`: `InputState`
- `button`: `i64`

**Returns:** `bool`

*Defined at line 295*

---

## input_gamepad_left_x

Get left stick X axis (-1.0 to 1.0).

```fides
pub fn input_gamepad_left_x(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 303*

---

## input_gamepad_left_y

Get left stick Y axis (-1.0 to 1.0).

```fides
pub fn input_gamepad_left_y(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 308*

---

## input_gamepad_right_x

Get right stick X axis (-1.0 to 1.0).

```fides
pub fn input_gamepad_right_x(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 313*

---

## input_gamepad_right_y

Get right stick Y axis (-1.0 to 1.0).

```fides
pub fn input_gamepad_right_y(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 318*

---

## input_gamepad_left_trigger

Get left trigger (0.0 to 1.0).

```fides
pub fn input_gamepad_left_trigger(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 323*

---

## input_gamepad_right_trigger

Get right trigger (0.0 to 1.0).

```fides
pub fn input_gamepad_right_trigger(state: InputState) -> f64 {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `f64`

*Defined at line 328*

---

## input_set_gamepad_button

Set gamepad button state (for platform integration).

```fides
pub fn input_set_gamepad_button(state: &mut InputState, button: i64, held: bool) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `button`: `i64`
- `held`: `bool`

**Returns:** `void`

*Defined at line 333*

---

## input_set_gamepad_axes

Set gamepad axis values (for platform integration).

```fides
pub fn input_set_gamepad_axes(state: &mut InputState, lx: f64, ly: f64, rx: f64, ry: f64, lt: f64, rt: f64) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `lx`: `f64`
- `ly`: `f64`
- `rx`: `f64`
- `ry`: `f64`
- `lt`: `f64`
- `rt`: `f64`

**Returns:** `void`

*Defined at line 345*

---

## input_set_gamepad_connected

Set gamepad connected status.

```fides
pub fn input_set_gamepad_connected(state: &mut InputState, connected: bool) -> void {
```

**Parameters:**

- `state`: `&mut InputState`
- `connected`: `bool`

**Returns:** `void`

*Defined at line 355*

---

## input_shift_held

Check if either shift key is held.

```fides
pub fn input_shift_held(state: InputState) -> bool {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `bool`

*Defined at line 362*

---

## input_ctrl_held

Check if either control key is held.

```fides
pub fn input_ctrl_held(state: InputState) -> bool {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `bool`

*Defined at line 367*

---

## input_alt_held

Check if either alt key is held.

```fides
pub fn input_alt_held(state: InputState) -> bool {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `bool`

*Defined at line 372*

---

## input_direction

Get directional input as a normalized vector (dx, dy).
Combines keyboard arrows, WASD, and gamepad left stick.

```fides
pub fn input_direction(state: InputState) -> (f64, f64) {
```

**Parameters:**

- `state`: `InputState`

**Returns:** `(f64, f64)`

*Defined at line 380*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
