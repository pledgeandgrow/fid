[All modules](index.md)

# audio -- Fides Documentation

## Contents

- [AUDIO_NO_CHANNEL:](#AUDIO_NO_CHANNEL:) (const)
- [AUDIO_MAX_CHANNELS:](#AUDIO_MAX_CHANNELS:) (const)
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
- [AudioBuffer](#AudioBuffer) (struct)
- [audio_wav_decode](#audio_wav_decode) (function)
- [audio_load_wav](#audio_load_wav) (function)
- [audio_wav_encode](#audio_wav_encode) (function)
- [audio_save_wav](#audio_save_wav) (function)
- [AudioState](#AudioState) (struct)
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

*Defined at line 56*

---

## audio_shutdown

Shut down the audio system and free all resources.

```fides
pub fn audio_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 65*

---

## audio_load

Load a sound file. Returns a sound handle (>0) on success, -1 on failure.

```fides
pub fn audio_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 76*

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

*Defined at line 88*

---

## audio_play_full

Play a sound at full volume. Returns a channel handle.

```fides
pub fn audio_play_full(sound: i64) -> i64 {
```

**Parameters:**

- `sound`: `i64`

**Returns:** `i64`

*Defined at line 97*

---

## audio_stop

Stop a playing sound channel.

```fides
pub fn audio_stop(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 106*

---

## audio_stop_all

Stop all playing sounds.

```fides
pub fn audio_stop_all() -> void {
```

**Returns:** `void`

*Defined at line 116*

---

## audio_pause

Pause a channel (can be resumed with audio_resume).

```fides
pub fn audio_pause(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 125*

---

## audio_resume

Resume a paused channel.

```fides
pub fn audio_resume(channel: i64) -> void {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `void`

*Defined at line 135*

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

*Defined at line 147*

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

*Defined at line 157*

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

*Defined at line 171*

---

## audio_is_playing

Check if a channel is still playing. Returns true if playing.

```fides
pub fn audio_is_playing(channel: i64) -> bool {
```

**Parameters:**

- `channel`: `i64`

**Returns:** `bool`

*Defined at line 181*

---

## audio_master_volume

Set the master volume (affects all channels). 0.0 to 1.0.

```fides
pub fn audio_master_volume(volume: f64) -> void {
```

**Parameters:**

- `volume`: `f64`

**Returns:** `void`

*Defined at line 195*

---

## AudioBuffer

```fides
pub struct AudioBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `sample_rate` | `i64` |
| `channels` | `i64` |
| `bits` | `i64` |
| `data` | `Vec<i64>,  // interleaved samples (i16 range for 16-bit, 0-255 for 8-bit)` |

*Defined at line 210*

---

## audio_wav_decode

Decode a WAV byte stream. `sample_rate` is 0 on failure.

```fides
pub fn audio_wav_decode(d: Vec<i64>) -> AudioBuffer {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `AudioBuffer`

*Defined at line 231*

---

## audio_load_wav

Load and decode a WAV file. `sample_rate` is 0 on failure.

```fides
pub fn audio_load_wav(path: str) -> AudioBuffer {
```

**Parameters:**

- `path`: `str`

**Returns:** `AudioBuffer`

*Defined at line 302*

---

## audio_wav_encode

Encode an AudioBuffer as a 16-bit PCM WAV byte stream.
8-bit buffers are widened to 16 bits.

```fides
pub fn audio_wav_encode(buf: &AudioBuffer) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&AudioBuffer`

**Returns:** `Vec<i64>`

*Defined at line 308*

---

## audio_save_wav

Save an AudioBuffer as a 16-bit PCM WAV file.

```fides
pub fn audio_save_wav(buf: &AudioBuffer, path: str) -> i64 {
```

**Parameters:**

- `buf`: `&AudioBuffer`
- `path`: `str`

**Returns:** `i64`

*Defined at line 369*

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
| `music_channel` | `i64` |
| `sfx_channels` | `Vec<i64>,  // active SFX channels` |

*Defined at line 379*

---

## audio_state_create

Create an audio state tracker.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val st = audio_state_create();
assert(!st.muted);
assert(st.master_vol == 1.0);
assert_eq(st.music_channel, AUDIO_NO_CHANNEL);
assert_eq(st.sfx_channels.len(), 0);
```

```fides
pub fn audio_state_create() -> AudioState {
```

**Returns:** `AudioState`

*Defined at line 395*

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

*Defined at line 402*

---

## audio_state_toggle_mute

Toggle mute. Returns the new mute state.

```fides
pub fn audio_state_toggle_mute(state: &mut AudioState) -> bool {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `bool`

*Defined at line 410*

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

*Defined at line 421*

---

## audio_state_stop_music

Stop music.

```fides
pub fn audio_state_stop_music(state: &mut AudioState) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `void`

*Defined at line 430*

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

*Defined at line 438*

---

## audio_state_cleanup_sfx

Clean up finished SFX channels from the tracking list.

```fides
pub fn audio_state_cleanup_sfx(state: &mut AudioState) -> void {
```

**Parameters:**

- `state`: `&mut AudioState`

**Returns:** `void`

*Defined at line 447*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
