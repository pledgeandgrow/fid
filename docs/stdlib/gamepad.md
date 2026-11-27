[All modules](index.md)

# gamepad -- Fides Documentation

## Contents

- [gp_sdl_poll](#gp_sdl_poll) (function)
- [GP_BUTTON_A:](#GP_BUTTON_A:) (const)
- [GP_BUTTON_B:](#GP_BUTTON_B:) (const)
- [GP_BUTTON_X:](#GP_BUTTON_X:) (const)
- [GP_BUTTON_Y:](#GP_BUTTON_Y:) (const)
- [GP_BUTTON_BACK:](#GP_BUTTON_BACK:) (const)
- [GP_BUTTON_GUIDE:](#GP_BUTTON_GUIDE:) (const)
- [GP_BUTTON_START:](#GP_BUTTON_START:) (const)
- [GP_BUTTON_LEFTSTICK:](#GP_BUTTON_LEFTSTICK:) (const)
- [GP_BUTTON_RIGHTSTICK:](#GP_BUTTON_RIGHTSTICK:) (const)
- [GP_BUTTON_LEFTSHOULDER:](#GP_BUTTON_LEFTSHOULDER:) (const)
- [GP_BUTTON_RIGHTSHOULDER:](#GP_BUTTON_RIGHTSHOULDER:) (const)
- [GP_BUTTON_DPAD_UP:](#GP_BUTTON_DPAD_UP:) (const)
- [GP_BUTTON_DPAD_DOWN:](#GP_BUTTON_DPAD_DOWN:) (const)
- [GP_BUTTON_DPAD_LEFT:](#GP_BUTTON_DPAD_LEFT:) (const)
- [GP_BUTTON_DPAD_RIGHT:](#GP_BUTTON_DPAD_RIGHT:) (const)
- [GP_BUTTON_MISC1:](#GP_BUTTON_MISC1:) (const)
- [GP_BUTTON_TOUCHPAD:](#GP_BUTTON_TOUCHPAD:) (const)
- [GP_BUTTON_COUNT:](#GP_BUTTON_COUNT:) (const)
- [GP_AXIS_LEFTX:](#GP_AXIS_LEFTX:) (const)
- [GP_AXIS_LEFTY:](#GP_AXIS_LEFTY:) (const)
- [GP_AXIS_RIGHTX:](#GP_AXIS_RIGHTX:) (const)
- [GP_AXIS_RIGHTY:](#GP_AXIS_RIGHTY:) (const)
- [GP_AXIS_TRIGGERLEFT:](#GP_AXIS_TRIGGERLEFT:) (const)
- [GP_AXIS_TRIGGERRIGHT:](#GP_AXIS_TRIGGERRIGHT:) (const)
- [GP_AXIS_COUNT:](#GP_AXIS_COUNT:) (const)
- [gp_button_name](#gp_button_name) (function)
- [gp_axis_name](#gp_axis_name) (function)
- [GP_SRC_BUTTON:](#GP_SRC_BUTTON:) (const)
- [GP_SRC_AXIS:](#GP_SRC_AXIS:) (const)
- [GP_SRC_HAT:](#GP_SRC_HAT:) (const)
- [GpBinding](#GpBinding) (struct)
- [GpMapping](#GpMapping) (struct)
- [GpGuid](#GpGuid) (struct)
- [gp_guid_decode](#gp_guid_decode) (function)
- [gp_mapping_parse](#gp_mapping_parse) (function)
- [gp_mapping_to_str](#gp_mapping_to_str) (function)
- [GpRaw](#GpRaw) (struct)
- [GpState](#GpState) (struct)
- [gp_state_new](#gp_state_new) (function)
- [gp_apply](#gp_apply) (function)
- [gp_axis_norm](#gp_axis_norm) (function)
- [gp_trigger_norm](#gp_trigger_norm) (function)
- [gp_deadzone_axial](#gp_deadzone_axial) (function)
- [GpStick](#GpStick) (struct)
- [gp_deadzone_radial](#gp_deadzone_radial) (function)
- [gp_deadzone_axial2](#gp_deadzone_axial2) (function)
- [gp_curve](#gp_curve) (function)
- [GpRumble](#GpRumble) (struct)
- [GpRumbleMixer](#GpRumbleMixer) (struct)
- [gp_rumble_mixer_new](#gp_rumble_mixer_new) (function)
- [gp_rumble_play](#gp_rumble_play) (function)
- [gp_rumble_envelope](#gp_rumble_envelope) (function)
- [gp_rumble_mix](#gp_rumble_mix) (function)
- [gp_rumble_send](#gp_rumble_send) (function)
- [GP_EVENT_CONNECTED:](#GP_EVENT_CONNECTED:) (const)
- [GP_EVENT_DISCONNECTED:](#GP_EVENT_DISCONNECTED:) (const)
- [GP_EVENT_RECONNECTED:](#GP_EVENT_RECONNECTED:) (const)
- [GpEvent](#GpEvent) (struct)
- [GpHub](#GpHub) (struct)
- [gp_hub_new](#gp_hub_new) (function)
- [gp_hub_player_of](#gp_hub_player_of) (function)
- [gp_hub_added](#gp_hub_added) (function)
- [gp_hub_removed](#gp_hub_removed) (function)
- [gp_hub_take](#gp_hub_take) (function)
- [gp_hub_connected](#gp_hub_connected) (function)

## gp_sdl_poll

Read an open SDL controller into a GpState.

```fides
pub fn gp_sdl_poll(controller: i64) -> GpState {
```

**Parameters:**

- `controller`: `i64`

**Returns:** `GpState`

*Defined at line 58*

---

## GP_BUTTON_A:

```fides
pub const GP_BUTTON_A: i64 = 0;
```

*Defined at line 88*

---

## GP_BUTTON_B:

```fides
pub const GP_BUTTON_B: i64 = 1;
```

*Defined at line 89*

---

## GP_BUTTON_X:

```fides
pub const GP_BUTTON_X: i64 = 2;
```

*Defined at line 90*

---

## GP_BUTTON_Y:

```fides
pub const GP_BUTTON_Y: i64 = 3;
```

*Defined at line 91*

---

## GP_BUTTON_BACK:

```fides
pub const GP_BUTTON_BACK: i64 = 4;
```

*Defined at line 92*

---

## GP_BUTTON_GUIDE:

```fides
pub const GP_BUTTON_GUIDE: i64 = 5;
```

*Defined at line 93*

---

## GP_BUTTON_START:

```fides
pub const GP_BUTTON_START: i64 = 6;
```

*Defined at line 94*

---

## GP_BUTTON_LEFTSTICK:

```fides
pub const GP_BUTTON_LEFTSTICK: i64 = 7;
```

*Defined at line 95*

---

## GP_BUTTON_RIGHTSTICK:

```fides
pub const GP_BUTTON_RIGHTSTICK: i64 = 8;
```

*Defined at line 96*

---

## GP_BUTTON_LEFTSHOULDER:

```fides
pub const GP_BUTTON_LEFTSHOULDER: i64 = 9;
```

*Defined at line 97*

---

## GP_BUTTON_RIGHTSHOULDER:

```fides
pub const GP_BUTTON_RIGHTSHOULDER: i64 = 10;
```

*Defined at line 98*

---

## GP_BUTTON_DPAD_UP:

```fides
pub const GP_BUTTON_DPAD_UP: i64 = 11;
```

*Defined at line 99*

---

## GP_BUTTON_DPAD_DOWN:

```fides
pub const GP_BUTTON_DPAD_DOWN: i64 = 12;
```

*Defined at line 100*

---

## GP_BUTTON_DPAD_LEFT:

```fides
pub const GP_BUTTON_DPAD_LEFT: i64 = 13;
```

*Defined at line 101*

---

## GP_BUTTON_DPAD_RIGHT:

```fides
pub const GP_BUTTON_DPAD_RIGHT: i64 = 14;
```

*Defined at line 102*

---

## GP_BUTTON_MISC1:

```fides
pub const GP_BUTTON_MISC1: i64 = 15;
```

*Defined at line 103*

---

## GP_BUTTON_TOUCHPAD:

```fides
pub const GP_BUTTON_TOUCHPAD: i64 = 20;
```

*Defined at line 104*

---

## GP_BUTTON_COUNT:

```fides
pub const GP_BUTTON_COUNT: i64 = 21;
```

*Defined at line 105*

---

## GP_AXIS_LEFTX:

```fides
pub const GP_AXIS_LEFTX: i64 = 0;
```

*Defined at line 107*

---

## GP_AXIS_LEFTY:

```fides
pub const GP_AXIS_LEFTY: i64 = 1;
```

*Defined at line 108*

---

## GP_AXIS_RIGHTX:

```fides
pub const GP_AXIS_RIGHTX: i64 = 2;
```

*Defined at line 109*

---

## GP_AXIS_RIGHTY:

```fides
pub const GP_AXIS_RIGHTY: i64 = 3;
```

*Defined at line 110*

---

## GP_AXIS_TRIGGERLEFT:

```fides
pub const GP_AXIS_TRIGGERLEFT: i64 = 4;
```

*Defined at line 111*

---

## GP_AXIS_TRIGGERRIGHT:

```fides
pub const GP_AXIS_TRIGGERRIGHT: i64 = 5;
```

*Defined at line 112*

---

## GP_AXIS_COUNT:

```fides
pub const GP_AXIS_COUNT: i64 = 6;
```

*Defined at line 113*

---

## gp_button_name

```fides
pub fn gp_button_name(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 152*

---

## gp_axis_name

```fides
pub fn gp_axis_name(a: i64) -> str {
```

**Parameters:**

- `a`: `i64`

**Returns:** `str`

*Defined at line 160*

---

## GP_SRC_BUTTON:

```fides
pub const GP_SRC_BUTTON: i64 = 0;
```

*Defined at line 181*

---

## GP_SRC_AXIS:

```fides
pub const GP_SRC_AXIS: i64 = 1;
```

*Defined at line 182*

---

## GP_SRC_HAT:

```fides
pub const GP_SRC_HAT: i64 = 2;
```

*Defined at line 183*

---

## GpBinding

One binding: a raw input to a standard output.

```fides
pub struct GpBinding {
```

**Fields:**

| Name | Type |
|------|------|
| `to_axis` | `bool` |
| `target` | `i64` |
| `out_half` | `i64` |
| `src` | `i64` |
| `index` | `i64` |
| `hat_mask` | `i64` |
| `in_half` | `i64` |
| `invert` | `bool` |

*Defined at line 186*

---

## GpMapping

```fides
pub struct GpMapping {
```

**Fields:**

| Name | Type |
|------|------|
| `guid` | `str` |
| `name` | `str` |
| `platform` | `str` |
| `bindings` | `Vec<GpBinding>` |

*Defined at line 201*

---

## GpGuid

```fides
pub struct GpGuid {
```

**Fields:**

| Name | Type |
|------|------|
| `bus` | `i64` |
| `crc` | `i64` |
| `vendor` | `i64` |
| `product` | `i64` |
| `version` | `i64` |
| `driver` | `i64` |

*Defined at line 208*

---

## gp_guid_decode

Decode a 32-hex-digit SDL 2 joystick GUID.

```fides
pub fn gp_guid_decode(s: str) -> Result<GpGuid, str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Result<GpGuid, str>`

*Defined at line 233*

---

## gp_mapping_parse

Parse a mapping line. Unknown target names are skipped (newer SDL
versions add some); malformed sources are errors.

```fides
pub fn gp_mapping_parse(line: str) -> Result<GpMapping, str> {
```

**Parameters:**

- `line`: `str`

**Returns:** `Result<GpMapping, str>`

*Defined at line 305*

---

## gp_mapping_to_str

The mapping back as a line (fields in binding order).

```fides
pub fn gp_mapping_to_str(m: &GpMapping) -> str {
```

**Parameters:**

- `m`: `&GpMapping`

**Returns:** `str`

*Defined at line 369*

---

## GpRaw

A raw joystick: buttons, axes (-32768..32767), hats (direction masks).

```fides
pub struct GpRaw {
```

**Fields:**

| Name | Type |
|------|------|
| `buttons` | `Vec<bool>` |
| `axes` | `Vec<i64>` |
| `hats` | `Vec<i64>` |

*Defined at line 413*

---

## GpState

```fides
pub struct GpState {
```

**Fields:**

| Name | Type |
|------|------|
| `buttons` | `Vec<bool>` |
| `axes` | `Vec<i64>` |

*Defined at line 419*

---

## gp_state_new

```fides
pub fn gp_state_new() -> GpState {
```

**Returns:** `GpState`

*Defined at line 425*

---

## gp_apply

The standard state of a raw joystick under a mapping (SDL semantics:
a half input uses only its side; a full axis feeding a trigger maps
-32768..32767 to 0..32767; a button or hat feeding an axis gives the
output's maximum; an axis feeding a button presses it past half way).

```fides
pub fn gp_apply(m: &GpMapping, raw: &GpRaw) -> GpState {
```

**Parameters:**

- `m`: `&GpMapping`
- `raw`: `&GpRaw`

**Returns:** `GpState`

*Defined at line 470*

---

## gp_axis_norm

A stick axis as -1..1 (negative side divided by 32768).

```fides
pub fn gp_axis_norm(v: i64) -> f64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `f64`

*Defined at line 550*

---

## gp_trigger_norm

A trigger as 0..1.

```fides
pub fn gp_trigger_norm(v: i64) -> f64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `f64`

*Defined at line 558*

---

## gp_deadzone_axial

One axis with a dead zone `dz` and saturation at `outer` (both 0..1),
rescaled so the output runs 0..1 between them.

```fides
pub fn gp_deadzone_axial(x: f64, dz: f64, outer: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`
- `dz`: `f64`
- `outer`: `f64`

**Returns:** `f64`

*Defined at line 567*

---

## GpStick

```fides
pub struct GpStick {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |

*Defined at line 582*

---

## gp_deadzone_radial

Radial dead zone: inside `dz` of the centre the stick reads (0, 0);
beyond, the direction is kept and the magnitude rescaled from dz..outer
to 0..1 (no axis-aligned snapping, no corner square).

```fides
pub fn gp_deadzone_radial(x: f64, y: f64, dz: f64, outer: f64) -> GpStick {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `dz`: `f64`
- `outer`: `f64`

**Returns:** `GpStick`

*Defined at line 590*

---

## gp_deadzone_axial2

Per-axis dead zone on both axes (snaps towards the axes).

```fides
pub fn gp_deadzone_axial2(x: f64, y: f64, dz: f64, outer: f64) -> GpStick {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `dz`: `f64`
- `outer`: `f64`

**Returns:** `GpStick`

*Defined at line 607*

---

## gp_curve

A response curve: sign(x) * |x|^exponent (2 gives fine aim near the
centre).

```fides
pub fn gp_curve(x: f64, exponent: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`
- `exponent`: `f64`

**Returns:** `f64`

*Defined at line 615*

---

## GpRumble

```fides
pub struct GpRumble {
```

**Fields:**

| Name | Type |
|------|------|
| `low` | `f64` |
| `high` | `f64` |
| `start_ms` | `i64` |
| `duration_ms` | `i64` |
| `attack_ms` | `i64` |
| `fade_ms` | `i64` |

*Defined at line 628*

---

## GpRumbleMixer

```fides
pub struct GpRumbleMixer {
```

**Fields:**

| Name | Type |
|------|------|
| `effects` | `Vec<GpRumble>` |

*Defined at line 638*

---

## gp_rumble_mixer_new

```fides
pub fn gp_rumble_mixer_new() -> GpRumbleMixer {
```

**Returns:** `GpRumbleMixer`

*Defined at line 642*

---

## gp_rumble_play

```fides
pub fn gp_rumble_play(m: &mut GpRumbleMixer, low: f64, high: f64, now_ms: i64, duration_ms: i64, attack_ms: i64, fade_ms: i64) {
```

**Parameters:**

- `m`: `&mut GpRumbleMixer`
- `low`: `f64`
- `high`: `f64`
- `now_ms`: `i64`
- `duration_ms`: `i64`
- `attack_ms`: `i64`
- `fade_ms`: `i64`

*Defined at line 648*

---

## gp_rumble_envelope

Envelope of an effect at a time: ramps up over attack, down over fade.

```fides
pub fn gp_rumble_envelope(e: &GpRumble, now_ms: i64) -> f64 {
```

**Parameters:**

- `e`: `&GpRumble`
- `now_ms`: `i64`

**Returns:** `f64`

*Defined at line 655*

---

## gp_rumble_mix

The two motor values (0..65535) at a time: the strongest active effect
per motor; finished effects are dropped. Returns [low, high].

```fides
pub fn gp_rumble_mix(m: &mut GpRumbleMixer, now_ms: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&mut GpRumbleMixer`
- `now_ms`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 676*

---

## gp_rumble_send

Send the mix to a controller for the next `ms` milliseconds (SDL).

```fides
pub fn gp_rumble_send(controller: i64, motors: &Vec<i64>, ms: i64) -> i64 {
```

**Parameters:**

- `controller`: `i64`
- `motors`: `&Vec<i64>`
- `ms`: `i64`

**Returns:** `i64`

*Defined at line 713*

---

## GP_EVENT_CONNECTED:

```fides
pub const GP_EVENT_CONNECTED: i64 = 1;
```

*Defined at line 726*

---

## GP_EVENT_DISCONNECTED:

```fides
pub const GP_EVENT_DISCONNECTED: i64 = 2;
```

*Defined at line 727*

---

## GP_EVENT_RECONNECTED:

```fides
pub const GP_EVENT_RECONNECTED: i64 = 3;
```

*Defined at line 728*

---

## GpEvent

```fides
pub struct GpEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `player` | `i64` |
| `instance` | `i64` |
| `guid` | `str` |

*Defined at line 730*

---

## GpHub

```fides
pub struct GpHub {
```

**Fields:**

| Name | Type |
|------|------|
| `max_players` | `i64` |
| `instance` | `Vec<i64>` |
| `guid` | `Vec<str>` |
| `events` | `Vec<GpEvent>` |

*Defined at line 737*

---

## gp_hub_new

```fides
pub fn gp_hub_new(max_players: i64) -> GpHub {
```

**Parameters:**

- `max_players`: `i64`

**Returns:** `GpHub`

*Defined at line 746*

---

## gp_hub_player_of

The player slot of a connected instance, or -1.

```fides
pub fn gp_hub_player_of(h: &GpHub, instance: i64) -> i64 {
```

**Parameters:**

- `h`: `&GpHub`
- `instance`: `i64`

**Returns:** `i64`

*Defined at line 759*

---

## gp_hub_added

A device was added (SDL_CONTROLLERDEVICEADDED). Returns its player slot
or -1 when all slots are taken (or it is already connected).

```fides
pub fn gp_hub_added(h: &mut GpHub, instance: i64, guid: str) -> i64 {
```

**Parameters:**

- `h`: `&mut GpHub`
- `instance`: `i64`
- `guid`: `str`

**Returns:** `i64`

*Defined at line 772*

---

## gp_hub_removed

A device was removed (SDL_CONTROLLERDEVICEREMOVED). Returns the slot it
had, or -1 for an unknown instance. The slot remembers the GUID.

```fides
pub fn gp_hub_removed(h: &mut GpHub, instance: i64) -> i64 {
```

**Parameters:**

- `h`: `&mut GpHub`
- `instance`: `i64`

**Returns:** `i64`

*Defined at line 818*

---

## gp_hub_take

Take the queued events.

```fides
pub fn gp_hub_take(h: &mut GpHub) -> Vec<GpEvent> {
```

**Parameters:**

- `h`: `&mut GpHub`

**Returns:** `Vec<GpEvent>`

*Defined at line 831*

---

## gp_hub_connected

```fides
pub fn gp_hub_connected(h: &GpHub) -> i64 {
```

**Parameters:**

- `h`: `&GpHub`

**Returns:** `i64`

*Defined at line 837*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
