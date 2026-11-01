# audio_mixer — Fides Documentation

## Contents

- [GROUP_MASTER:](#group_master:) (const)
- [GROUP_SFX:](#group_sfx:) (const)
- [GROUP_MUSIC:](#group_music:) (const)
- [GROUP_VOICE:](#group_voice:) (const)
- [GROUP_AMBIENT:](#group_ambient:) (const)
- [GROUP_COUNT:](#group_count:) (const)
- [SoundEntry](#soundentry) (struct)
- [AudioMixer](#audiomixer) (struct)
- [audio_mixer_init](#audio_mixer_init) (function)
- [audio_mixer_load](#audio_mixer_load) (function)
- [audio_mixer_load_group](#audio_mixer_load_group) (function)
- [audio_mixer_play](#audio_mixer_play) (function)
- [audio_mixer_play_volume](#audio_mixer_play_volume) (function)
- [audio_mixer_play_3d](#audio_mixer_play_3d) (function)
- [audio_mixer_play_2d](#audio_mixer_play_2d) (function)
- [audio_mixer_distance_attenuation](#audio_mixer_distance_attenuation) (function)
- [audio_mixer_set_master_volume](#audio_mixer_set_master_volume) (function)
- [audio_mixer_set_group_volume](#audio_mixer_set_group_volume) (function)
- [audio_mixer_master_volume](#audio_mixer_master_volume) (function)
- [audio_mixer_group_volume](#audio_mixer_group_volume) (function)
- [audio_mixer_set_listener](#audio_mixer_set_listener) (function)
- [audio_mixer_set_3d_params](#audio_mixer_set_3d_params) (function)
- [audio_mixer_stop_group](#audio_mixer_stop_group) (function)
- [audio_mixer_stop_all](#audio_mixer_stop_all) (function)
- [audio_mixer_pause_all](#audio_mixer_pause_all) (function)
- [audio_mixer_resume_all](#audio_mixer_resume_all) (function)
- [audio_mixer_unload](#audio_mixer_unload) (function)
- [audio_mixer_unload_all](#audio_mixer_unload_all) (function)
- [audio_mixer_sound_count](#audio_mixer_sound_count) (function)
- [audio_mixer_is_loaded](#audio_mixer_is_loaded) (function)
- [audio_mixer_shutdown](#audio_mixer_shutdown) (function)

## GROUP_MASTER:

```fides
pub const GROUP_MASTER: i64 = 0;
```

*Defined at line 35*

---

## GROUP_SFX:

```fides
pub const GROUP_SFX: i64 = 1;
```

*Defined at line 36*

---

## GROUP_MUSIC:

```fides
pub const GROUP_MUSIC: i64 = 2;
```

*Defined at line 37*

---

## GROUP_VOICE:

```fides
pub const GROUP_VOICE: i64 = 3;
```

*Defined at line 38*

---

## GROUP_AMBIENT:

```fides
pub const GROUP_AMBIENT: i64 = 4;
```

*Defined at line 39*

---

## GROUP_COUNT:

```fides
pub const GROUP_COUNT: i64 = 5;
```

*Defined at line 40*

---

## SoundEntry

```fides
pub struct SoundEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `handle` | `i64,        // SDL_mixer chunk handle` |
| `group` | `i64` |
| `default_volume` | `f64` |
| `loaded` | `bool` |

*Defined at line 44*

---

## AudioMixer

```fides
pub struct AudioMixer {
```

**Fields:**

| Name | Type |
|------|------|
| `sounds` | `Vec<SoundEntry>` |
| `group_volumes` | `Vec<f64>,   // per-group volume (0..1)` |
| `master_volume` | `f64` |
| `listener_x` | `f64` |
| `listener_y` | `f64` |
| `listener_z` | `f64` |
| `max_distance` | `f64,        // distance at which sound is inaudible` |
| `ref_distance` | `f64,        // distance at which sound is at full volume` |
| `rolloff` | `f64,             // attenuation curve (1.0 = linear)` |
| `initialized` | `bool` |

*Defined at line 52*

---

## audio_mixer_init

Initialize the audio mixer.

```fides
pub fn audio_mixer_init() -> void {
```

**Returns:** `void`

*Defined at line 77*

---

## audio_mixer_load

Load a sound file into the cache. Returns the sound index or -1 on failure.

```fides
pub fn audio_mixer_load(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 89*

---

## audio_mixer_load_group

Load a sound into a specific group.

```fides
pub fn audio_mixer_load_group(name: str, group: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `group`: `i64`

**Returns:** `i64`

*Defined at line 117*

---

## audio_mixer_play

Play a sound by index.

```fides
pub fn audio_mixer_play(sound_idx: i64) -> i64 {
```

**Parameters:**

- `sound_idx`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## audio_mixer_play_volume

Play a sound with a specific volume override.

```fides
pub fn audio_mixer_play_volume(sound_idx: i64, volume: f64) -> i64 {
```

**Parameters:**

- `sound_idx`: `i64`
- `volume`: `f64`

**Returns:** `i64`

*Defined at line 139*

---

## audio_mixer_play_3d

Play a sound with 3D positional attenuation.

```fides
pub fn audio_mixer_play_3d(sound_idx: i64, sx: f64, sy: f64, sz: f64) -> i64 {
```

**Parameters:**

- `sound_idx`: `i64`
- `sx`: `f64`
- `sy`: `f64`
- `sz`: `f64`

**Returns:** `i64`

*Defined at line 150*

---

## audio_mixer_play_2d

Play a sound with 2D positional audio (x, y only).

```fides
pub fn audio_mixer_play_2d(sound_idx: i64, sx: f64, sy: f64) -> i64 {
```

**Parameters:**

- `sound_idx`: `i64`
- `sx`: `f64`
- `sy`: `f64`

**Returns:** `i64`

*Defined at line 169*

---

## audio_mixer_distance_attenuation

Compute distance attenuation factor (0..1).

```fides
pub fn audio_mixer_distance_attenuation(distance: f64) -> f64 {
```

**Parameters:**

- `distance`: `f64`

**Returns:** `f64`

*Defined at line 174*

---

## audio_mixer_set_master_volume

Set the master volume (0..1).

```fides
pub fn audio_mixer_set_master_volume(vol: f64) -> void {
```

**Parameters:**

- `vol`: `f64`

**Returns:** `void`

*Defined at line 195*

---

## audio_mixer_set_group_volume

Set the volume for a specific group (0..1).

```fides
pub fn audio_mixer_set_group_volume(group: i64, vol: f64) -> void {
```

**Parameters:**

- `group`: `i64`
- `vol`: `f64`

**Returns:** `void`

*Defined at line 202*

---

## audio_mixer_master_volume

Get the master volume.

```fides
pub fn audio_mixer_master_volume() -> f64 {
```

**Returns:** `f64`

*Defined at line 210*

---

## audio_mixer_group_volume

Get the volume for a specific group.

```fides
pub fn audio_mixer_group_volume(group: i64) -> f64 {
```

**Parameters:**

- `group`: `i64`

**Returns:** `f64`

*Defined at line 215*

---

## audio_mixer_set_listener

Set the listener position for 3D audio.

```fides
pub fn audio_mixer_set_listener(x: f64, y: f64, z: f64) -> void {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `void`

*Defined at line 221*

---

## audio_mixer_set_3d_params

Set the 3D audio parameters.

```fides
pub fn audio_mixer_set_3d_params(ref_dist: f64, max_dist: f64, rolloff: f64) -> void {
```

**Parameters:**

- `ref_dist`: `f64`
- `max_dist`: `f64`
- `rolloff`: `f64`

**Returns:** `void`

*Defined at line 228*

---

## audio_mixer_stop_group

Stop all sounds in a group.

```fides
pub fn audio_mixer_stop_group(group: i64) -> void {
```

**Parameters:**

- `group`: `i64`

**Returns:** `void`

*Defined at line 235*

---

## audio_mixer_stop_all

Stop all sounds.

```fides
pub fn audio_mixer_stop_all() -> void {
```

**Returns:** `void`

*Defined at line 242*

---

## audio_mixer_pause_all

Pause all sounds.

```fides
pub fn audio_mixer_pause_all() -> void {
```

**Returns:** `void`

*Defined at line 247*

---

## audio_mixer_resume_all

Resume all sounds.

```fides
pub fn audio_mixer_resume_all() -> void {
```

**Returns:** `void`

*Defined at line 252*

---

## audio_mixer_unload

Unload a sound from the cache.

```fides
pub fn audio_mixer_unload(sound_idx: i64) -> void {
```

**Parameters:**

- `sound_idx`: `i64`

**Returns:** `void`

*Defined at line 257*

---

## audio_mixer_unload_all

Unload all sounds.

```fides
pub fn audio_mixer_unload_all() -> void {
```

**Returns:** `void`

*Defined at line 268*

---

## audio_mixer_sound_count

Get the number of loaded sounds.

```fides
pub fn audio_mixer_sound_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 278*

---

## audio_mixer_is_loaded

Check if a sound is loaded.

```fides
pub fn audio_mixer_is_loaded(sound_idx: i64) -> bool {
```

**Parameters:**

- `sound_idx`: `i64`

**Returns:** `bool`

*Defined at line 283*

---

## audio_mixer_shutdown

Shutdown the audio mixer.

```fides
pub fn audio_mixer_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 289*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
