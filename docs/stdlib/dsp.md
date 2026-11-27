[All modules](index.md)

# dsp -- Fides Documentation

## Contents

- [DSP_WIN_RECT:](#DSP_WIN_RECT:) (const)
- [DSP_WIN_HANN:](#DSP_WIN_HANN:) (const)
- [DSP_WIN_HAMMING:](#DSP_WIN_HAMMING:) (const)
- [DSP_WIN_BLACKMAN:](#DSP_WIN_BLACKMAN:) (const)
- [DSP_WIN_FLATTOP:](#DSP_WIN_FLATTOP:) (const)
- [CVec](#CVec) (struct)
- [dsp_pi](#dsp_pi) (function)
- [dsp_cvec_zeros](#dsp_cvec_zeros) (function)
- [dsp_cvec_from_real](#dsp_cvec_from_real) (function)
- [dsp_cvec_len](#dsp_cvec_len) (function)
- [dsp_cvec_get](#dsp_cvec_get) (function)
- [dsp_magnitude](#dsp_magnitude) (function)
- [dsp_is_pow2](#dsp_is_pow2) (function)
- [dsp_next_pow2](#dsp_next_pow2) (function)
- [dsp_fft](#dsp_fft) (function)
- [dsp_ifft](#dsp_ifft) (function)
- [dsp_fft_real](#dsp_fft_real) (function)
- [dsp_dft](#dsp_dft) (function)
- [dsp_convolve](#dsp_convolve) (function)
- [dsp_window](#dsp_window) (function)
- [dsp_apply_window](#dsp_apply_window) (function)
- [dsp_window_gain](#dsp_window_gain) (function)
- [dsp_fir_lowpass](#dsp_fir_lowpass) (function)
- [dsp_fir_highpass](#dsp_fir_highpass) (function)
- [dsp_fir_bandpass](#dsp_fir_bandpass) (function)
- [dsp_fir_filter](#dsp_fir_filter) (function)
- [dsp_freq_response](#dsp_freq_response) (function)
- [dsp_db](#dsp_db) (function)
- [Biquad](#Biquad) (struct)
- [dsp_biquad_lowpass](#dsp_biquad_lowpass) (function)
- [dsp_biquad_highpass](#dsp_biquad_highpass) (function)
- [dsp_biquad_bandpass](#dsp_biquad_bandpass) (function)
- [dsp_biquad_notch](#dsp_biquad_notch) (function)
- [dsp_biquad_allpass](#dsp_biquad_allpass) (function)
- [dsp_biquad_peaking](#dsp_biquad_peaking) (function)
- [dsp_biquad_lowshelf](#dsp_biquad_lowshelf) (function)
- [dsp_biquad_highshelf](#dsp_biquad_highshelf) (function)
- [Biquad_process](#Biquad_process) (function)
- [Biquad_reset](#Biquad_reset) (function)
- [Biquad_filter](#Biquad_filter) (function)
- [Biquad_magnitude](#Biquad_magnitude) (function)
- [dsp_rms](#dsp_rms) (function)
- [dsp_mean](#dsp_mean) (function)
- [dsp_goertzel](#dsp_goertzel) (function)
- [dsp_goertzel_power](#dsp_goertzel_power) (function)
- [dsp_harmonics](#dsp_harmonics) (function)
- [dsp_thd](#dsp_thd) (function)

## DSP_WIN_RECT:

```fides
pub const DSP_WIN_RECT: i64 = 0;
```

*Defined at line 35*

---

## DSP_WIN_HANN:

```fides
pub const DSP_WIN_HANN: i64 = 1;
```

*Defined at line 36*

---

## DSP_WIN_HAMMING:

```fides
pub const DSP_WIN_HAMMING: i64 = 2;
```

*Defined at line 37*

---

## DSP_WIN_BLACKMAN:

```fides
pub const DSP_WIN_BLACKMAN: i64 = 3;
```

*Defined at line 38*

---

## DSP_WIN_FLATTOP:

```fides
pub const DSP_WIN_FLATTOP: i64 = 4;
```

*Defined at line 39*

---

## CVec

A complex sequence as two parallel arrays.

```fides
pub struct CVec {
```

**Fields:**

| Name | Type |
|------|------|
| `re` | `Vec<f64>` |
| `im` | `Vec<f64>` |

*Defined at line 42*

---

## dsp_pi

```fides
pub fn dsp_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 47*

---

## dsp_cvec_zeros

```fides
pub fn dsp_cvec_zeros(n: i64) -> CVec {
```

**Parameters:**

- `n`: `i64`

**Returns:** `CVec`

*Defined at line 51*

---

## dsp_cvec_from_real

```fides
pub fn dsp_cvec_from_real(x: &Vec<f64>) -> CVec {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `CVec`

*Defined at line 65*

---

## dsp_cvec_len

```fides
pub fn dsp_cvec_len(x: &CVec) -> i64 {
```

**Parameters:**

- `x`: `&CVec`

**Returns:** `i64`

*Defined at line 79*

---

## dsp_cvec_get

```fides
pub fn dsp_cvec_get(x: &CVec, i: i64) -> Complex {
```

**Parameters:**

- `x`: `&CVec`
- `i`: `i64`

**Returns:** `Complex`

*Defined at line 83*

---

## dsp_magnitude

|X[k]| for every k.

```fides
pub fn dsp_magnitude(x: &CVec) -> Vec<f64> {
```

**Parameters:**

- `x`: `&CVec`

**Returns:** `Vec<f64>`

*Defined at line 88*

---

## dsp_is_pow2

```fides
pub fn dsp_is_pow2(n: i64) -> bool {
```

**Parameters:**

- `n`: `i64`

**Returns:** `bool`

*Defined at line 100*

---

## dsp_next_pow2

```fides
pub fn dsp_next_pow2(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 104*

---

## dsp_fft

The DFT of x, any length (radix-2 or Bluestein).

```fides
pub fn dsp_fft(x: &CVec) -> CVec {
```

**Parameters:**

- `x`: `&CVec`

**Returns:** `CVec`

*Defined at line 243*

---

## dsp_ifft

The inverse DFT (1/n scaling), any length.

```fides
pub fn dsp_ifft(x: &CVec) -> CVec {
```

**Parameters:**

- `x`: `&CVec`

**Returns:** `CVec`

*Defined at line 259*

---

## dsp_fft_real

The FFT of a real sequence.

```fides
pub fn dsp_fft_real(x: &Vec<f64>) -> CVec {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `CVec`

*Defined at line 280*

---

## dsp_dft

The direct O(n^2) DFT (a reference for checking).

```fides
pub fn dsp_dft(x: &CVec) -> CVec {
```

**Parameters:**

- `x`: `&CVec`

**Returns:** `CVec`

*Defined at line 286*

---

## dsp_convolve

Linear convolution of two real sequences through the FFT.

```fides
pub fn dsp_convolve(a: &Vec<f64>, b: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `&Vec<f64>`
- `b`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 310*

---

## dsp_window

Window `kind` of length n. Symmetric (w[0] == w[n-1], for FIR design)
unless `periodic` (the DFT-even form for spectral analysis).

```fides
pub fn dsp_window(kind: i64, n: i64, periodic: bool) -> Vec<f64> {
```

**Parameters:**

- `kind`: `i64`
- `n`: `i64`
- `periodic`: `bool`

**Returns:** `Vec<f64>`

*Defined at line 349*

---

## dsp_apply_window

x[i] * w[i].

```fides
pub fn dsp_apply_window(x: &Vec<f64>, w: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `w`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 379*

---

## dsp_window_gain

The coherent gain of a window: mean of its samples.

```fides
pub fn dsp_window_gain(w: &Vec<f64>) -> f64 {
```

**Parameters:**

- `w`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 390*

---

## dsp_fir_lowpass

Windowed-sinc low-pass with `ntaps` taps and cutoff `fc` as a fraction
of the sample rate (0 < fc < 0.5), scaled to unit gain at DC.

```fides
pub fn dsp_fir_lowpass(ntaps: i64, fc: f64, window: i64) -> Vec<f64> {
```

**Parameters:**

- `ntaps`: `i64`
- `fc`: `f64`
- `window`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 412*

---

## dsp_fir_highpass

High-pass by spectral inversion of the low-pass (ntaps must be odd).

```fides
pub fn dsp_fir_highpass(ntaps: i64, fc: f64, window: i64) -> Vec<f64> {
```

**Parameters:**

- `ntaps`: `i64`
- `fc`: `f64`
- `window`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 433*

---

## dsp_fir_bandpass

Band-pass f1..f2 (fractions of the sample rate): low-pass(f2) minus
low-pass(f1).

```fides
pub fn dsp_fir_bandpass(ntaps: i64, f1: f64, f2: f64, window: i64) -> Vec<f64> {
```

**Parameters:**

- `ntaps`: `i64`
- `f1`: `f64`
- `f2`: `f64`
- `window`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 450*

---

## dsp_fir_filter

y[n] = sum_k h[k] x[n - k], zero initial state, same length as x.

```fides
pub fn dsp_fir_filter(h: &Vec<f64>, x: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `h`: `&Vec<f64>`
- `x`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 463*

---

## dsp_freq_response

H(e^{iw}) of b/a at frequency f (fraction of the sample rate).

```fides
pub fn dsp_freq_response(b: &Vec<f64>, a: &Vec<f64>, f: f64) -> Complex {
```

**Parameters:**

- `b`: `&Vec<f64>`
- `a`: `&Vec<f64>`
- `f`: `f64`

**Returns:** `Complex`

*Defined at line 480*

---

## dsp_db

|H| in decibels (20 log10).

```fides
pub fn dsp_db(mag: f64) -> f64 {
```

**Parameters:**

- `mag`: `f64`

**Returns:** `f64`

*Defined at line 505*

---

## Biquad

Normalised biquad (a0 = 1) with its transposed-direct-form-II state.

```fides
pub struct Biquad {
```

**Fields:**

| Name | Type |
|------|------|
| `b0` | `f64` |
| `b1` | `f64` |
| `b2` | `f64` |
| `a1` | `f64` |
| `a2` | `f64` |
| `z1` | `f64` |
| `z2` | `f64` |

*Defined at line 512*

---

## dsp_biquad_lowpass

```fides
pub fn dsp_biquad_lowpass(fs: f64, f0: f64, q: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`

**Returns:** `Biquad`

*Defined at line 532*

---

## dsp_biquad_highpass

```fides
pub fn dsp_biquad_highpass(fs: f64, f0: f64, q: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`

**Returns:** `Biquad`

*Defined at line 539*

---

## dsp_biquad_bandpass

Band-pass with a constant 0 dB peak gain.

```fides
pub fn dsp_biquad_bandpass(fs: f64, f0: f64, q: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`

**Returns:** `Biquad`

*Defined at line 547*

---

## dsp_biquad_notch

```fides
pub fn dsp_biquad_notch(fs: f64, f0: f64, q: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`

**Returns:** `Biquad`

*Defined at line 554*

---

## dsp_biquad_allpass

```fides
pub fn dsp_biquad_allpass(fs: f64, f0: f64, q: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`

**Returns:** `Biquad`

*Defined at line 561*

---

## dsp_biquad_peaking

Peaking EQ with `gain_db` at f0.

```fides
pub fn dsp_biquad_peaking(fs: f64, f0: f64, q: f64, gain_db: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`
- `gain_db`: `f64`

**Returns:** `Biquad`

*Defined at line 569*

---

## dsp_biquad_lowshelf

```fides
pub fn dsp_biquad_lowshelf(fs: f64, f0: f64, q: f64, gain_db: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`
- `gain_db`: `f64`

**Returns:** `Biquad`

*Defined at line 577*

---

## dsp_biquad_highshelf

```fides
pub fn dsp_biquad_highshelf(fs: f64, f0: f64, q: f64, gain_db: f64) -> Biquad {
```

**Parameters:**

- `fs`: `f64`
- `f0`: `f64`
- `q`: `f64`
- `gain_db`: `f64`

**Returns:** `Biquad`

*Defined at line 586*

---

## Biquad_process

One sample through the biquad.

```fides
pub fn Biquad_process(bq: &mut Biquad, x: f64) -> f64 {
```

**Parameters:**

- `bq`: `&mut Biquad`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 596*

---

## Biquad_reset

```fides
pub fn Biquad_reset(bq: &mut Biquad) {
```

**Parameters:**

- `bq`: `&mut Biquad`

*Defined at line 603*

---

## Biquad_filter

A whole sequence through the biquad (state carries on).

```fides
pub fn Biquad_filter(bq: &mut Biquad, x: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `bq`: `&mut Biquad`
- `x`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 609*

---

## Biquad_magnitude

|H| of the biquad at frequency f (Hz) for sample rate fs.

```fides
pub fn Biquad_magnitude(bq: &Biquad, fs: f64, f: f64) -> f64 {
```

**Parameters:**

- `bq`: `&Biquad`
- `fs`: `f64`
- `f`: `f64`

**Returns:** `f64`

*Defined at line 620*

---

## dsp_rms

```fides
pub fn dsp_rms(x: &Vec<f64>) -> f64 {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 634*

---

## dsp_mean

```fides
pub fn dsp_mean(x: &Vec<f64>) -> f64 {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 644*

---

## dsp_goertzel

The DFT value sum x[n] e^{-2 pi i f n / fs} at any frequency f, by the
(generalised) Goertzel recurrence.

```fides
pub fn dsp_goertzel(x: &Vec<f64>, f: f64, fs: f64) -> Complex {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `f`: `f64`
- `fs`: `f64`

**Returns:** `Complex`

*Defined at line 656*

---

## dsp_goertzel_power

|X(f)|^2 by the classic Goertzel recurrence (no complex output needed).

```fides
pub fn dsp_goertzel_power(x: &Vec<f64>, f: f64, fs: f64) -> f64 {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `f`: `f64`
- `fs`: `f64`

**Returns:** `f64`

*Defined at line 677*

---

## dsp_harmonics

Peak amplitudes of harmonics 1..nh of f0 in a record of a whole number
of periods of f0 sampled at fs (one DFT bin per harmonic, so leakage is
zero). Index 0 of the result is the DC level.

```fides
pub fn dsp_harmonics(x: &Vec<f64>, fs: f64, f0: f64, nh: i64) -> Vec<f64> {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `fs`: `f64`
- `f0`: `f64`
- `nh`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 695*

---

## dsp_thd

THD_F: sqrt(sum of squares of harmonics 2..n) / fundamental, from the
amplitudes `dsp_harmonics` gives.

```fides
pub fn dsp_thd(harm: &Vec<f64>) -> f64 {
```

**Parameters:**

- `harm`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 710*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
