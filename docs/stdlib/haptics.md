[All modules](index.md)

# haptics -- Fides Documentation

## Contents

- [HAPTIC_TRANSIENT:](#HAPTIC_TRANSIENT:) (const)
- [HAPTIC_CONTINUOUS:](#HAPTIC_CONTINUOUS:) (const)
- [HAPTIC_MAX_CONTINUOUS_MS:](#HAPTIC_MAX_CONTINUOUS_MS:) (const)
- [ANDROID_EFFECT_CLICK:](#ANDROID_EFFECT_CLICK:) (const)
- [ANDROID_EFFECT_DOUBLE_CLICK:](#ANDROID_EFFECT_DOUBLE_CLICK:) (const)
- [ANDROID_EFFECT_TICK:](#ANDROID_EFFECT_TICK:) (const)
- [ANDROID_EFFECT_HEAVY_CLICK:](#ANDROID_EFFECT_HEAVY_CLICK:) (const)
- [ANDROID_PRIMITIVE_CLICK:](#ANDROID_PRIMITIVE_CLICK:) (const)
- [ANDROID_PRIMITIVE_THUD:](#ANDROID_PRIMITIVE_THUD:) (const)
- [ANDROID_PRIMITIVE_SPIN:](#ANDROID_PRIMITIVE_SPIN:) (const)
- [ANDROID_PRIMITIVE_QUICK_RISE:](#ANDROID_PRIMITIVE_QUICK_RISE:) (const)
- [ANDROID_PRIMITIVE_SLOW_RISE:](#ANDROID_PRIMITIVE_SLOW_RISE:) (const)
- [ANDROID_PRIMITIVE_QUICK_FALL:](#ANDROID_PRIMITIVE_QUICK_FALL:) (const)
- [ANDROID_PRIMITIVE_TICK:](#ANDROID_PRIMITIVE_TICK:) (const)
- [ANDROID_PRIMITIVE_LOW_TICK:](#ANDROID_PRIMITIVE_LOW_TICK:) (const)
- [IOS_IMPACT_LIGHT:](#IOS_IMPACT_LIGHT:) (const)
- [IOS_IMPACT_MEDIUM:](#IOS_IMPACT_MEDIUM:) (const)
- [IOS_IMPACT_HEAVY:](#IOS_IMPACT_HEAVY:) (const)
- [IOS_IMPACT_SOFT:](#IOS_IMPACT_SOFT:) (const)
- [IOS_IMPACT_RIGID:](#IOS_IMPACT_RIGID:) (const)
- [HapticEvent](#HapticEvent) (struct)
- [HapticCurve](#HapticCurve) (struct)
- [HapticPattern](#HapticPattern) (struct)
- [haptic_pattern_new](#haptic_pattern_new) (function)
- [haptic_add_transient](#haptic_add_transient) (function)
- [haptic_add_continuous](#haptic_add_continuous) (function)
- [haptic_add_curve](#haptic_add_curve) (function)
- [haptic_sorted](#haptic_sorted) (function)
- [haptic_validate](#haptic_validate) (function)
- [haptic_duration](#haptic_duration) (function)
- [haptic_amplitude](#haptic_amplitude) (function)
- [haptic_transient_ms](#haptic_transient_ms) (function)
- [Waveform](#Waveform) (struct)
- [haptic_android_waveform](#haptic_android_waveform) (function)
- [haptic_android_onoff](#haptic_android_onoff) (function)
- [haptic_impact_android_effect](#haptic_impact_android_effect) (function)
- [haptic_impact_pattern](#haptic_impact_pattern) (function)
- [haptic_num](#haptic_num) (function)
- [haptic_ahap](#haptic_ahap) (function)
- [AhapParsed](#AhapParsed) (struct)
- [haptic_parse_ahap](#haptic_parse_ahap) (function)
- [HapticCall](#HapticCall) (struct)
- [haptic_android_calls](#haptic_android_calls) (function)
- [haptic_ios_calls](#haptic_ios_calls) (function)

## HAPTIC_TRANSIENT:

```fides
pub const HAPTIC_TRANSIENT: i64 = 0;
```

*Defined at line 29*

---

## HAPTIC_CONTINUOUS:

```fides
pub const HAPTIC_CONTINUOUS: i64 = 1;
```

*Defined at line 30*

---

## HAPTIC_MAX_CONTINUOUS_MS:

```fides
pub const HAPTIC_MAX_CONTINUOUS_MS: i64 = 30000;
```

*Defined at line 31*

---

## ANDROID_EFFECT_CLICK:

```fides
pub const ANDROID_EFFECT_CLICK: i64 = 0;
```

*Defined at line 34*

---

## ANDROID_EFFECT_DOUBLE_CLICK:

```fides
pub const ANDROID_EFFECT_DOUBLE_CLICK: i64 = 1;
```

*Defined at line 35*

---

## ANDROID_EFFECT_TICK:

```fides
pub const ANDROID_EFFECT_TICK: i64 = 2;
```

*Defined at line 36*

---

## ANDROID_EFFECT_HEAVY_CLICK:

```fides
pub const ANDROID_EFFECT_HEAVY_CLICK: i64 = 5;
```

*Defined at line 37*

---

## ANDROID_PRIMITIVE_CLICK:

```fides
pub const ANDROID_PRIMITIVE_CLICK: i64 = 1;
```

*Defined at line 38*

---

## ANDROID_PRIMITIVE_THUD:

```fides
pub const ANDROID_PRIMITIVE_THUD: i64 = 2;
```

*Defined at line 39*

---

## ANDROID_PRIMITIVE_SPIN:

```fides
pub const ANDROID_PRIMITIVE_SPIN: i64 = 3;
```

*Defined at line 40*

---

## ANDROID_PRIMITIVE_QUICK_RISE:

```fides
pub const ANDROID_PRIMITIVE_QUICK_RISE: i64 = 4;
```

*Defined at line 41*

---

## ANDROID_PRIMITIVE_SLOW_RISE:

```fides
pub const ANDROID_PRIMITIVE_SLOW_RISE: i64 = 5;
```

*Defined at line 42*

---

## ANDROID_PRIMITIVE_QUICK_FALL:

```fides
pub const ANDROID_PRIMITIVE_QUICK_FALL: i64 = 6;
```

*Defined at line 43*

---

## ANDROID_PRIMITIVE_TICK:

```fides
pub const ANDROID_PRIMITIVE_TICK: i64 = 7;
```

*Defined at line 44*

---

## ANDROID_PRIMITIVE_LOW_TICK:

```fides
pub const ANDROID_PRIMITIVE_LOW_TICK: i64 = 8;
```

*Defined at line 45*

---

## IOS_IMPACT_LIGHT:

```fides
pub const IOS_IMPACT_LIGHT: i64 = 0;
```

*Defined at line 48*

---

## IOS_IMPACT_MEDIUM:

```fides
pub const IOS_IMPACT_MEDIUM: i64 = 1;
```

*Defined at line 49*

---

## IOS_IMPACT_HEAVY:

```fides
pub const IOS_IMPACT_HEAVY: i64 = 2;
```

*Defined at line 50*

---

## IOS_IMPACT_SOFT:

```fides
pub const IOS_IMPACT_SOFT: i64 = 3;
```

*Defined at line 51*

---

## IOS_IMPACT_RIGID:

```fides
pub const IOS_IMPACT_RIGID: i64 = 4;
```

*Defined at line 52*

---

## HapticEvent

```fides
pub struct HapticEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `time_ms` | `i64` |
| `duration_ms` | `i64,  // continuous only` |
| `intensity` | `f64,  // 0..1` |
| `sharpness` | `f64,  // 0..1` |

*Defined at line 54*

---

## HapticCurve

A control-point curve on the intensity of what plays (AHAP
HapticIntensityControl): times relative to `time_ms`.

```fides
pub struct HapticCurve {
```

**Fields:**

| Name | Type |
|------|------|
| `time_ms` | `i64` |
| `times` | `Vec<i64>` |
| `values` | `Vec<f64>` |

*Defined at line 64*

---

## HapticPattern

```fides
pub struct HapticPattern {
```

**Fields:**

| Name | Type |
|------|------|
| `events` | `Vec<HapticEvent>` |
| `curves` | `Vec<HapticCurve>` |

*Defined at line 70*

---

## haptic_pattern_new

```fides
pub fn haptic_pattern_new() -> HapticPattern {
```

**Returns:** `HapticPattern`

*Defined at line 75*

---

## haptic_add_transient

```fides
pub fn haptic_add_transient(p: &mut HapticPattern, time_ms: i64, intensity: f64, sharpness: f64) {
```

**Parameters:**

- `p`: `&mut HapticPattern`
- `time_ms`: `i64`
- `intensity`: `f64`
- `sharpness`: `f64`

*Defined at line 81*

---

## haptic_add_continuous

```fides
pub fn haptic_add_continuous(p: &mut HapticPattern, time_ms: i64, duration_ms: i64, intensity: f64, sharpness: f64) {
```

**Parameters:**

- `p`: `&mut HapticPattern`
- `time_ms`: `i64`
- `duration_ms`: `i64`
- `intensity`: `f64`
- `sharpness`: `f64`

*Defined at line 87*

---

## haptic_add_curve

```fides
pub fn haptic_add_curve(p: &mut HapticPattern, time_ms: i64, times: Vec<i64>, values: Vec<f64>) {
```

**Parameters:**

- `p`: `&mut HapticPattern`
- `time_ms`: `i64`
- `times`: `Vec<i64>`
- `values`: `Vec<f64>`

*Defined at line 93*

---

## haptic_sorted

Events sorted by start time (stable).

```fides
pub fn haptic_sorted(p: &HapticPattern) -> Vec<HapticEvent> {
```

**Parameters:**

- `p`: `&HapticPattern`

**Returns:** `Vec<HapticEvent>`

*Defined at line 100*

---

## haptic_validate

Problems that make the pattern unplayable; empty when it is fine.

```fides
pub fn haptic_validate(p: &HapticPattern) -> Vec<str> {
```

**Parameters:**

- `p`: `&HapticPattern`

**Returns:** `Vec<str>`

*Defined at line 116*

---

## haptic_duration

Total length in milliseconds (end of the last event).

```fides
pub fn haptic_duration(p: &HapticPattern) -> i64 {
```

**Parameters:**

- `p`: `&HapticPattern`

**Returns:** `i64`

*Defined at line 165*

---

## haptic_amplitude

Amplitude 1..255 of an intensity (0 stays 0).

```fides
pub fn haptic_amplitude(intensity: f64) -> i64 {
```

**Parameters:**

- `intensity`: `f64`

**Returns:** `i64`

*Defined at line 182*

---

## haptic_transient_ms

How long a transient pulse lasts on a vibration motor: 20 ms when
dull, 5 ms when sharp.

```fides
pub fn haptic_transient_ms(sharpness: f64) -> i64 {
```

**Parameters:**

- `sharpness`: `f64`

**Returns:** `i64`

*Defined at line 198*

---

## Waveform

The arguments of VibrationEffect.createWaveform.

```fides
pub struct Waveform {
```

**Fields:**

| Name | Type |
|------|------|
| `timings` | `Vec<i64>` |
| `amplitudes` | `Vec<i64>` |
| `repeat` | `i64,  // -1: play once` |

*Defined at line 210*

---

## haptic_android_waveform

The waveform of a pattern. `repeat_from_ms` >= 0 makes it loop from
the segment that starts at that time (-1: once).

```fides
pub fn haptic_android_waveform(p: &HapticPattern, repeat_from_ms: i64) -> Waveform {
```

**Parameters:**

- `p`: `&HapticPattern`
- `repeat_from_ms`: `i64`

**Returns:** `Waveform`

*Defined at line 231*

---

## haptic_android_onoff

The legacy createWaveform(long[] timings, int repeat) form for
vibrators without amplitude control: off, on, off, on... durations,
starting with an "off" (possibly 0); an amplitude below `min_amp`
counts as off.

```fides
pub fn haptic_android_onoff(w: &Waveform, min_amp: i64) -> Vec<i64> {
```

**Parameters:**

- `w`: `&Waveform`
- `min_amp`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 272*

---

## haptic_impact_android_effect

The predefined Android effect closest to an iOS impact style.

```fides
pub fn haptic_impact_android_effect(style: i64) -> i64 {
```

**Parameters:**

- `style`: `i64`

**Returns:** `i64`

*Defined at line 291*

---

## haptic_impact_pattern

An impact style as a one-tap pattern (the intensity and sharpness of
the system generators).

```fides
pub fn haptic_impact_pattern(style: i64) -> HapticPattern {
```

**Parameters:**

- `style`: `i64`

**Returns:** `HapticPattern`

*Defined at line 303*

---

## haptic_num

A number as AHAP writes it: up to 3 decimals, at least one.

```fides
pub fn haptic_num(x: f64) -> str {
```

**Parameters:**

- `x`: `f64`

**Returns:** `str`

*Defined at line 322*

---

## haptic_ahap

The AHAP document of a pattern.

```fides
pub fn haptic_ahap(p: &HapticPattern) -> str {
```

**Parameters:**

- `p`: `&HapticPattern`

**Returns:** `str`

*Defined at line 353*

---

## AhapParsed

Parse the events and curves of an AHAP document (ok false when it is
not one).

```fides
pub struct AhapParsed {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pattern` | `HapticPattern` |

*Defined at line 403*

---

## haptic_parse_ahap

```fides
pub fn haptic_parse_ahap(text: str) -> AhapParsed {
```

**Parameters:**

- `text`: `str`

**Returns:** `AhapParsed`

*Defined at line 408*

---

## HapticCall

```fides
pub struct HapticCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |

*Defined at line 474*

---

## haptic_android_calls

Android: build the effect, then vibrate (Vibrator from
Context.getSystemService(VIBRATOR_SERVICE), or VibratorManager on 31+).

```fides
pub fn haptic_android_calls() -> Vec<HapticCall> {
```

**Returns:** `Vec<HapticCall>`

*Defined at line 482*

---

## haptic_ios_calls

iOS: an engine, started, playing the AHAP bytes (NSData).

```fides
pub fn haptic_ios_calls() -> Vec<HapticCall> {
```

**Returns:** `Vec<HapticCall>`

*Defined at line 500*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
