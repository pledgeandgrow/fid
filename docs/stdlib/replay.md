# replay — Fides Documentation

## Contents

- [ReplayFrame](#replayframe) (struct)
- [ReplayHeader](#replayheader) (struct)
- [Replay](#replay) (struct)
- [REPLAY_IDLE:](#replay_idle:) (const)
- [REPLAY_RECORDING:](#replay_recording:) (const)
- [REPLAY_PLAYING:](#replay_playing:) (const)
- [REPLAY_MAGIC:](#replay_magic:) (const)
- [REPLAY_VERSION:](#replay_version:) (const)
- [replay_create](#replay_create) (function)
- [replay_start_recording](#replay_start_recording) (function)
- [replay_record_frame](#replay_record_frame) (function)
- [replay_stop_recording](#replay_stop_recording) (function)
- [replay_start_playback](#replay_start_playback) (function)
- [replay_make_frame](#replay_make_frame) (function)
- [replay_get_frame_into](#replay_get_frame_into) (function)
- [replay_get_frame](#replay_get_frame) (function)
- [replay_next_frame](#replay_next_frame) (function)
- [replay_is_finished](#replay_is_finished) (function)
- [replay_stop_playback](#replay_stop_playback) (function)
- [replay_mode](#replay_mode) (function)
- [replay_frame_count](#replay_frame_count) (function)
- [replay_current_frame_idx](#replay_current_frame_idx) (function)
- [replay_start_seed](#replay_start_seed) (function)
- [replay_progress](#replay_progress) (function)
- [replay_seek](#replay_seek) (function)
- [replay_set_max_frames](#replay_set_max_frames) (function)
- [replay_save](#replay_save) (function)
- [replay_load](#replay_load) (function)
- [replay_clear](#replay_clear) (function)

## ReplayFrame

```fides
pub struct ReplayFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_number` | `i64` |
| `key_count` | `i64` |
| `key0` | `i64` |
| `key1` | `i64` |
| `key2` | `i64` |
| `key3` | `i64` |
| `mouse_x` | `i64` |
| `mouse_y` | `i64` |
| `mouse_buttons` | `i64` |
| `dt_ms` | `i64` |
| `seed` | `i64` |

*Defined at line 23*

---

## ReplayHeader

```fides
pub struct ReplayHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `magic` | `i64` |
| `version` | `i64` |
| `frame_count` | `i64` |
| `start_seed` | `i64` |
| `game_name` | `str` |
| `game_version` | `str` |
| `recorded_at` | `i64` |

*Defined at line 37*

---

## Replay

```fides
pub struct Replay {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `ReplayHeader` |
| `frame_numbers` | `Vec<i64>` |
| `key_counts` | `Vec<i64>` |
| `key0s` | `Vec<i64>` |
| `key1s` | `Vec<i64>` |
| `key2s` | `Vec<i64>` |
| `key3s` | `Vec<i64>` |
| `mouse_xs` | `Vec<i64>` |
| `mouse_ys` | `Vec<i64>` |
| `mouse_buttons` | `Vec<i64>` |
| `dt_ms` | `Vec<i64>` |
| `seeds` | `Vec<i64>` |
| `current_frame` | `i64` |
| `mode` | `i64` |
| `max_frames` | `i64` |
| `read_pos` | `i64` |
| `read_data` | `Vec<i64>` |

*Defined at line 47*

---

## REPLAY_IDLE:

```fides
pub const REPLAY_IDLE: i64 = 0;
```

*Defined at line 67*

---

## REPLAY_RECORDING:

```fides
pub const REPLAY_RECORDING: i64 = 1;
```

*Defined at line 67*

---

## REPLAY_PLAYING:

```fides
pub const REPLAY_PLAYING: i64 = 2;
```

*Defined at line 67*

---

## REPLAY_MAGIC:

```fides
pub const REPLAY_MAGIC: i64 = 0x46505244;
```

*Defined at line 67*

---

## REPLAY_VERSION:

```fides
pub const REPLAY_VERSION: i64 = 1;
```

*Defined at line 67*

---

## replay_create

Create a new replay state.

```fides
pub fn replay_create() -> Replay {
```

**Returns:** `Replay`

*Defined at line 87*

---

## replay_start_recording

Start recording a replay.

```fides
pub fn replay_start_recording(rp: &mut Replay, seed: i64, game_name: str, game_version: str) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`
- `seed`: `i64`
- `game_name`: `str`
- `game_version`: `str`

**Returns:** `void`

*Defined at line 96*

---

## replay_record_frame

Record a frame of input (up to 4 keys per frame).

```fides
pub fn replay_record_frame(rp: &mut Replay, keys: Vec<i64>, mouse_x: i64, mouse_y: i64, mouse_buttons: i64, dt_ms: i64, seed: i64) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`
- `keys`: `Vec<i64>`
- `mouse_x`: `i64`
- `mouse_y`: `i64`
- `mouse_buttons`: `i64`
- `dt_ms`: `i64`
- `seed`: `i64`

**Returns:** `void`

*Defined at line 116*

---

## replay_stop_recording

Stop recording.

```fides
pub fn replay_stop_recording(rp: &mut Replay) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`

**Returns:** `void`

*Defined at line 156*

---

## replay_start_playback

Start playback of a loaded replay.

```fides
pub fn replay_start_playback(rp: &mut Replay) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`

**Returns:** `void`

*Defined at line 161*

---

## replay_make_frame

Create a default replay frame (for pre-allocation).

```fides
pub fn replay_make_frame() -> ReplayFrame {
```

**Returns:** `ReplayFrame`

*Defined at line 170*

---

## replay_get_frame_into

Get the current replay frame (during playback). Writes into the provided frame.

```fides
pub fn replay_get_frame_into(rp: &Replay, out: &mut ReplayFrame) -> void {
```

**Parameters:**

- `rp`: `&Replay`
- `out`: `&mut ReplayFrame`

**Returns:** `void`

*Defined at line 177*

---

## replay_get_frame

Get the current replay frame (during playback).

```fides
pub fn replay_get_frame(rp: &Replay) -> ReplayFrame {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `ReplayFrame`

*Defined at line 220*

---

## replay_next_frame

Advance to the next frame during playback.

```fides
pub fn replay_next_frame(rp: &mut Replay) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`

**Returns:** `void`

*Defined at line 229*

---

## replay_is_finished

Check if playback is finished.

```fides
pub fn replay_is_finished(rp: &Replay) -> bool {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `bool`

*Defined at line 240*

---

## replay_stop_playback

Stop playback.

```fides
pub fn replay_stop_playback(rp: &mut Replay) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`

**Returns:** `void`

*Defined at line 245*

---

## replay_mode

Get the current replay mode.

```fides
pub fn replay_mode(rp: &Replay) -> i64 {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `i64`

*Defined at line 250*

---

## replay_frame_count

Get the total number of frames.

```fides
pub fn replay_frame_count(rp: &Replay) -> i64 {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `i64`

*Defined at line 255*

---

## replay_current_frame_idx

Get the current frame index (during playback).

```fides
pub fn replay_current_frame_idx(rp: &Replay) -> i64 {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `i64`

*Defined at line 260*

---

## replay_start_seed

Get the start seed from the replay.

```fides
pub fn replay_start_seed(rp: &Replay) -> i64 {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `i64`

*Defined at line 265*

---

## replay_progress

Get the progress (0..1) of playback.

```fides
pub fn replay_progress(rp: &Replay) -> f64 {
```

**Parameters:**

- `rp`: `&Replay`

**Returns:** `f64`

*Defined at line 270*

---

## replay_seek

Seek to a specific frame (for scrubbing).

```fides
pub fn replay_seek(rp: &mut Replay, frame: i64) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`
- `frame`: `i64`

**Returns:** `void`

*Defined at line 278*

---

## replay_set_max_frames

Set the maximum recording length (in frames).

```fides
pub fn replay_set_max_frames(rp: &mut Replay, max: i64) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`
- `max`: `i64`

**Returns:** `void`

*Defined at line 291*

---

## replay_save

Save the replay to a file as a binary string.

```fides
pub fn replay_save(rp: &Replay, path: str) -> bool {
```

**Parameters:**

- `rp`: `&Replay`
- `path`: `str`

**Returns:** `bool`

*Defined at line 296*

---

## replay_load

Load a replay from a file.

```fides
pub fn replay_load(rp: &mut Replay, path: str) -> bool {
```

**Parameters:**

- `rp`: `&mut Replay`
- `path`: `str`

**Returns:** `bool`

*Defined at line 323*

---

## replay_clear

Clear the replay buffer.

```fides
pub fn replay_clear(rp: &mut Replay) -> void {
```

**Parameters:**

- `rp`: `&mut Replay`

**Returns:** `void`

*Defined at line 397*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
