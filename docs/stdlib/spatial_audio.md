[All modules](index.md)

# spatial_audio -- Fides Documentation

## Contents

- [SPATIAL_INVERSE:](#SPATIAL_INVERSE:) (const)
- [SPATIAL_LINEAR:](#SPATIAL_LINEAR:) (const)
- [SPATIAL_EXPONENTIAL:](#SPATIAL_EXPONENTIAL:) (const)
- [SPATIAL_NONE:](#SPATIAL_NONE:) (const)
- [SPATIAL_STEREO:](#SPATIAL_STEREO:) (const)
- [SPATIAL_BINAURAL:](#SPATIAL_BINAURAL:) (const)
- [SPATIAL_SPEED_OF_SOUND:](#SPATIAL_SPEED_OF_SOUND:) (const)
- [SPATIAL_HEAD_RADIUS:](#SPATIAL_HEAD_RADIUS:) (const)
- [SpatialListener](#SpatialListener) (struct)
- [SpatialEmitter](#SpatialEmitter) (struct)
- [spatial_listener](#spatial_listener) (function)
- [spatial_emitter](#spatial_emitter) (function)
- [spatial_attenuation](#spatial_attenuation) (function)
- [spatial_distance](#spatial_distance) (function)
- [spatial_gain](#spatial_gain) (function)
- [spatial_atan](#spatial_atan) (function)
- [spatial_atan2](#spatial_atan2) (function)
- [spatial_asin](#spatial_asin) (function)
- [spatial_right](#spatial_right) (function)
- [spatial_local_dir](#spatial_local_dir) (function)
- [spatial_azimuth](#spatial_azimuth) (function)
- [spatial_elevation](#spatial_elevation) (function)
- [spatial_pan](#spatial_pan) (function)
- [SpatialGains](#SpatialGains) (struct)
- [spatial_equal_power](#spatial_equal_power) (function)
- [spatial_doppler](#spatial_doppler) (function)
- [spatial_itd](#spatial_itd) (function)
- [spatial_lateral](#spatial_lateral) (function)
- [spatial_shadow_alpha](#spatial_shadow_alpha) (function)
- [SpatialShadow](#SpatialShadow) (struct)
- [spatial_shadow](#spatial_shadow) (function)
- [spatial_shadow_set](#spatial_shadow_set) (function)
- [spatial_shadow_run](#spatial_shadow_run) (function)
- [spatial_shadow_gain](#spatial_shadow_gain) (function)
- [SpatialChannel](#SpatialChannel) (struct)
- [spatial_channel_params](#spatial_channel_params) (function)
- [SpatialVoice](#SpatialVoice) (struct)
- [spatial_voice](#spatial_voice) (function)
- [spatial_render](#spatial_render) (function)

## SPATIAL_INVERSE:

```fides
pub const SPATIAL_INVERSE: i64 = 0;
```

*Defined at line 42*

---

## SPATIAL_LINEAR:

```fides
pub const SPATIAL_LINEAR: i64 = 1;
```

*Defined at line 43*

---

## SPATIAL_EXPONENTIAL:

```fides
pub const SPATIAL_EXPONENTIAL: i64 = 2;
```

*Defined at line 44*

---

## SPATIAL_NONE:

```fides
pub const SPATIAL_NONE: i64 = 3;
```

*Defined at line 45*

---

## SPATIAL_STEREO:

```fides
pub const SPATIAL_STEREO: i64 = 0;
```

*Defined at line 47*

---

## SPATIAL_BINAURAL:

```fides
pub const SPATIAL_BINAURAL: i64 = 1;
```

*Defined at line 48*

---

## SPATIAL_SPEED_OF_SOUND:

Speed of sound in air (m/s) used by OpenAL.

```fides
pub const SPATIAL_SPEED_OF_SOUND: f64 = 343.3;
```

*Defined at line 51*

---

## SPATIAL_HEAD_RADIUS:

Head radius (m) of the binaural model.

```fides
pub const SPATIAL_HEAD_RADIUS: f64 = 0.0875;
```

*Defined at line 53*

---

## SpatialListener

```fides
pub struct SpatialListener {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `vec3` |
| `vel` | `vec3` |
| `forward` | `vec3` |
| `up` | `vec3` |
| `gain` | `f64` |

*Defined at line 55*

---

## SpatialEmitter

```fides
pub struct SpatialEmitter {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `vec3` |
| `vel` | `vec3` |
| `gain` | `f64` |
| `ref_dist` | `f64` |
| `max_dist` | `f64` |
| `rolloff` | `f64` |
| `model` | `i64` |
| `pitch` | `f64` |
| `looping` | `bool` |

*Defined at line 63*

---

## spatial_listener

A listener at `pos` looking along -z with +y up.

```fides
pub fn spatial_listener(pos: vec3) -> SpatialListener {
```

**Parameters:**

- `pos`: `vec3`

**Returns:** `SpatialListener`

*Defined at line 76*

---

## spatial_emitter

An emitter at `pos`: inverse model, ref 1 m, max 100 m, rolloff 1.

```fides
pub fn spatial_emitter(pos: vec3) -> SpatialEmitter {
```

**Parameters:**

- `pos`: `vec3`

**Returns:** `SpatialEmitter`

*Defined at line 83*

---

## spatial_attenuation

Gain of `model` at distance `d` (OpenAL 1.1 "clamped" models).

```fides
pub fn spatial_attenuation(model: i64, d0: f64, ref_dist: f64, max_dist: f64, rolloff: f64) -> f64 {
```

**Parameters:**

- `model`: `i64`
- `d0`: `f64`
- `ref_dist`: `f64`
- `max_dist`: `f64`
- `rolloff`: `f64`

**Returns:** `f64`

*Defined at line 92*

---

## spatial_distance

Distance from the listener to the emitter.

```fides
pub fn spatial_distance(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 127*

---

## spatial_gain

The emitter's distance gain times both gains.

```fides
pub fn spatial_gain(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 132*

---

## spatial_atan

atan(x) to double precision (argument reduction + series).

```fides
pub fn spatial_atan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 139*

---

## spatial_atan2

atan2(y, x) in -pi..pi.

```fides
pub fn spatial_atan2(y: f64, x: f64) -> f64 {
```

**Parameters:**

- `y`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 164*

---

## spatial_asin

asin(x) for -1..1.

```fides
pub fn spatial_asin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 185*

---

## spatial_right

The listener's right vector (forward x up, normalized).

```fides
pub fn spatial_right(l: &SpatialListener) -> vec3 {
```

**Parameters:**

- `l`: `&SpatialListener`

**Returns:** `vec3`

*Defined at line 194*

---

## spatial_local_dir

The emitter's direction in the listener's frame: x right, y up, z ahead
(unit length; straight ahead when the emitter is on the listener).

```fides
pub fn spatial_local_dir(l: &SpatialListener, e: &SpatialEmitter) -> vec3 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `vec3`

*Defined at line 200*

---

## spatial_azimuth

Azimuth in radians: 0 ahead, +pi/2 right, -pi/2 left, +-pi behind.

```fides
pub fn spatial_azimuth(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 213*

---

## spatial_elevation

Elevation in radians: +pi/2 straight up.

```fides
pub fn spatial_elevation(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 219*

---

## spatial_pan

Pan -1 (left) .. 1 (right): the lateral component of the direction.

```fides
pub fn spatial_pan(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 225*

---

## SpatialGains

A pair of channel gains.

```fides
pub struct SpatialGains {
```

**Fields:**

| Name | Type |
|------|------|
| `left` | `f64` |
| `right` | `f64` |

*Defined at line 230*

---

## spatial_equal_power

Equal-power gains for `pan` -1..1: left^2 + right^2 = 1.

```fides
pub fn spatial_equal_power(pan0: f64) -> SpatialGains {
```

**Parameters:**

- `pan0`: `f64`

**Returns:** `SpatialGains`

*Defined at line 236*

---

## spatial_doppler

Pitch factor of the Doppler effect (OpenAL 1.1, section 3.5.2):
(SS - DF vls) / (SS - DF vss), the velocities projected on the
source-to-listener axis and clamped below SS / DF (> 1 approaching).

```fides
pub fn spatial_doppler(l: &SpatialListener, e: &SpatialEmitter, doppler_factor: f64, speed_of_sound: f64) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`
- `doppler_factor`: `f64`
- `speed_of_sound`: `f64`

**Returns:** `f64`

*Defined at line 255*

---

## spatial_itd

Woodworth's interaural time difference in seconds for lateral angle
`theta` (-pi/2 left .. pi/2 right); positive: the left ear hears later.

```fides
pub fn spatial_itd(theta0: f64) -> f64 {
```

**Parameters:**

- `theta0`: `f64`

**Returns:** `f64`

*Defined at line 290*

---

## spatial_lateral

Lateral angle of the emitter (asin of its rightward component): the
angle the ITD depends on (a source behind mirrors one in front).

```fides
pub fn spatial_lateral(l: &SpatialListener, e: &SpatialEmitter) -> f64 {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `f64`

*Defined at line 304*

---

## spatial_shadow_alpha

Brown-Duda alpha (high-frequency gain) for incidence angle `theta`
between the source and an ear's axis: (1 + amin/2) + (1 - amin/2)
cos(theta / 150 deg * 180 deg) with amin = 0.1, so 2 at 0 and 0.1 at
150 degrees (rising a little again behind: the "bright spot").

```fides
pub fn spatial_shadow_alpha(theta: f64) -> f64 {
```

**Parameters:**

- `theta`: `f64`

**Returns:** `f64`

*Defined at line 312*

---

## SpatialShadow

One head-shadow filter: y = b0 x + b1 x' - a1 y'.

```fides
pub struct SpatialShadow {
```

**Fields:**

| Name | Type |
|------|------|
| `b0` | `f64` |
| `b1` | `f64` |
| `a1` | `f64` |
| `x1` | `f64` |
| `y1` | `f64` |

*Defined at line 325*

---

## spatial_shadow

```fides
pub fn spatial_shadow() -> SpatialShadow {
```

**Returns:** `SpatialShadow`

*Defined at line 333*

---

## spatial_shadow_set

Set the filter for `alpha` at `sample_rate` (bilinear transform of
H(s) = (alpha s + beta) / (s + beta), beta = 2 c / a).

```fides
pub fn spatial_shadow_set(f: &mut SpatialShadow, alpha: f64, sample_rate: f64) {
```

**Parameters:**

- `f`: `&mut SpatialShadow`
- `alpha`: `f64`
- `sample_rate`: `f64`

*Defined at line 341*

---

## spatial_shadow_run

```fides
pub fn spatial_shadow_run(f: &mut SpatialShadow, x: f64) -> f64 {
```

**Parameters:**

- `f`: `&mut SpatialShadow`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 350*

---

## spatial_shadow_gain

Magnitude of the filter's response at `freq` Hz.

```fides
pub fn spatial_shadow_gain(f: &SpatialShadow, freq: f64, sample_rate: f64) -> f64 {
```

**Parameters:**

- `f`: `&SpatialShadow`
- `freq`: `f64`
- `sample_rate`: `f64`

**Returns:** `f64`

*Defined at line 358*

---

## SpatialChannel

What a channel-based mixer needs for one emitter.

```fides
pub struct SpatialChannel {
```

**Fields:**

| Name | Type |
|------|------|
| `volume` | `f64` |
| `pan` | `f64,  // -1..1 (audio_set_pan)` |

*Defined at line 373*

---

## spatial_channel_params

Volume and pan of an emitter for a channel-based mixer.

```fides
pub fn spatial_channel_params(l: &SpatialListener, e: &SpatialEmitter) -> SpatialChannel {
```

**Parameters:**

- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`

**Returns:** `SpatialChannel`

*Defined at line 380*

---

## SpatialVoice

Playback state of one emitter's clip.

```fides
pub struct SpatialVoice {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `cursor` | `f64` |
| `gain_l` | `f64` |
| `gain_r` | `f64` |
| `delay_l` | `f64` |
| `delay_r` | `f64` |
| `fresh` | `bool` |
| `done` | `bool` |
| `doppler_factor` | `f64` |
| `shadow_l` | `SpatialShadow` |
| `shadow_r` | `SpatialShadow` |

*Defined at line 393*

---

## spatial_voice

```fides
pub fn spatial_voice(mode: i64) -> SpatialVoice {
```

**Parameters:**

- `mode`: `i64`

**Returns:** `SpatialVoice`

*Defined at line 411*

---

## spatial_render

Render `frames` stereo frames of `clip` (mono, at `sample_rate`) played
by `e`, adding them into `out` (interleaved L R; grown when too short).
Returns false once a non-looping clip has played to its end.

```fides
pub fn spatial_render(v: &mut SpatialVoice, l: &SpatialListener, e: &SpatialEmitter, clip: &Vec<f64>, out: &mut Vec<f64>, frames: i64, sample_rate: f64) -> bool {
```

**Parameters:**

- `v`: `&mut SpatialVoice`
- `l`: `&SpatialListener`
- `e`: `&SpatialEmitter`
- `clip`: `&Vec<f64>`
- `out`: `&mut Vec<f64>`
- `frames`: `i64`
- `sample_rate`: `f64`

**Returns:** `bool`

*Defined at line 446*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
