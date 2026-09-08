# audio — Fides Documentation

## Contents

- [AUDIO_NO_CHANNEL:](#audio_no_channel:) (const)
- [AUDIO_MAX_CHANNELS:](#audio_max_channels:) (const)
- [audio_init](#audio_init) (function)
- [audio_shutdown](#audio_shutdown) (function)
- [audio_load](#audio_load) (function)
- [audio_play](#audio_play) (function)
- [audio_play_full](#audio_play_full) (function)
- [audio_stop](#audio_stop) (function)
- [audio_stop_all](#audio_stop_all) (function)
- [audio_pause](#audio_pause) (function)
- [audio_resume](#audio_resume) (function)
- [audio_set_volume](#audio_set_volume) (function)
- [audio_set_loop](#audio_set_loop) (function)
- [audio_set_pan](#audio_set_pan) (function)
- [audio_is_playing](#audio_is_playing) (function)
- [audio_master_volume](#audio_master_volume) (function)
- [AudioState](#audiostate) (struct)
- [audio_state_create](#audio_state_create) (function)
- [audio_state_set_volume](#audio_state_set_volume) (function)
- [audio_state_toggle_mute](#audio_state_toggle_mute) (function)
- [audio_state_play_music](#audio_state_play_music) (function)
- [audio_state_stop_music](#audio_state_stop_music) (function)
- [audio_state_play_sfx](#audio_state_play_sfx) (function)
- [audio_state_cleanup_sfx](#audio_state_cleanup_sfx) (function)

## AUDIO_NO_CHANNEL:

```fides
pub const AUDIO_NO_CHANNEL: i64 = -1;
```

*Defined at line 25*

---

## AUDIO_MAX_CHANNELS:

```fides
pub const AUDIO_MAX_CHANNELS: i64 = 32;
```

*Defined at line 26*

---

## audio_init

Initialize the audio system. Returns 0 on success, -1 on failure.

```fides
pub fn audio_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 54*

---

## audio_shutdown

Shut down the audio system and free all resources.

```fides
pub fn audio_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 59*

---

## audio_load

Load a sound file. Returns a sound handle (>0) on success, -1 on failure.

```fides
pub fn audio_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 66*

---

## audio_play

Play a sound at the given volume (0.0 to 1.0). Returns a channel handle.

```fides
pub fn audio_play(sound: i64, volume: f64) -> i64 {
```

**Parameters:**

- `sound`: `i64`
- `volume`: `f64`

**Returns:** `i64`

*Defined at line 73*

---

## audio_play_full

Play a sound at full volume. Returns a channel handle.

```fides
pub fn audio_play_full(sound: i64) -> i64 {
```

**Parameters:**

- `sound`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## audio_stop

Stop a playing sound channel.

```fides
pub fn audio_stop(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 83*

---

## audio_stop_all

Stop all playing sounds.

```fides
pub fn audio_stop_all() -> void {
```

**Returns:** `void`

*Defined at line 88*

---

## audio_pause

Pause a channel (can be resumed with audio_resume).

```fides
pub fn audio_pause(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 93*

---

## audio_resume

Resume a paused channel.

```fides
pub fn audio_resume(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 98*

---

## audio_set_volume

Set the volume of a channel (0.0 to 1.0).

```fides
pub fn audio_set_volume(channel: i64, volume: f64) -> void {
```

**Parameters:**

- `channel`: `i64`
- `volume`: `f64`

**Returns:** `void`

*Defined at line 105*

---

## audio_set_loop

Set whether a channel loops (restarts when finished).

```fides
pub fn audio_set_loop(channel: i64, looping: bool) -> void {
```

**Parameters:**

- `channel`: `i64`
- `looping`: `bool`

**Returns:** `void`

*Defined at line 110*

---

## audio_set_pan

Set the stereo pan of a channel (-1.0 = left, 0.0 = center, 1.0 = right).

```fides
pub fn audio_set_pan(channel: i64, pan: f64) -> void {
```

**Parameters:**

- `channel`: `i64`
- `pan`: `f64`

**Returns:** `void`

*Defined at line 119*

---

## audio_is_playing

Check if a channel is still playing. Returns true if playing.

```fides
pub fn audio_is_playing(channel: i64) -> bool {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `bool`

*Defined at line 124*

---

## audio_master_volume

Set the master volume (affects all channels). 0.0 to 1.0.

```fides
pub fn audio_master_volume(volume: f64) -> void {
```

**Parameters:**

- `volume`: `f64`

**Returns:** `void`

*Defined at line 131*

---

## AudioState

```fides
pub struct AudioState {
```

**Fields:**

| Name | Type |
|------|------|
| `master_vol` | `f64` |
| `muted` | `bool` |
| `music_channel` | `i64,  // dedicated music channel` |
| `sfx_channels` | `Vec<i64>, // active SFX channels` |

*Defined at line 141*

---

## audio_state_create

Create an audio state tracker.

```fides
pub fn audio_state_create() -> AudioState {
```

**Returns:** `AudioState`

*Defined at line 149*

---

## audio_state_set_volume

Set master volume through the state tracker (respects mute).

```fides
pub fn audio_state_set_volume(state: &mut AudioState, vol: f64) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`
- `vol`: `f64`

**Returns:** `void`

*Defined at line 159*

---

## audio_state_toggle_mute

Toggle mute. Returns the new mute state.

```fides
pub fn audio_state_toggle_mute(state: &mut AudioState) -> bool {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `bool`

*Defined at line 167*

---

## audio_state_play_music

Play music (loops). Stops any previous music.

```fides
pub fn audio_state_play_music(state: &mut AudioState, sound: i64) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`
- `sound`: `i64`

**Returns:** `void`

*Defined at line 178*

---

## audio_state_stop_music

Stop music.

```fides
pub fn audio_state_stop_music(state: &mut AudioState) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `void`

*Defined at line 187*

---

## audio_state_play_sfx

Play a sound effect and track the channel.

```fides
pub fn audio_state_play_sfx(state: &mut AudioState, sound: i64, volume: f64) -> i64 {
```

**Parameters:**

- `state`: `&mut AudioState`
- `sound`: `i64`
- `volume`: `f64`

**Returns:** `i64`

*Defined at line 195*

---

## audio_state_cleanup_sfx

Clean up finished SFX channels from the tracking list.

```fides
pub fn audio_state_cleanup_sfx(state: &mut AudioState) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `void`

*Defined at line 204*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
