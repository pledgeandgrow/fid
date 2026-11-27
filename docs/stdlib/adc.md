[All modules](index.md)

# adc -- Fides Documentation

## Contents

- [AdcSpec](#AdcSpec) (struct)
- [adc_spec](#adc_spec) (function)
- [adc_spec_full](#adc_spec_full) (function)
- [adc_spec_bipolar](#adc_spec_bipolar) (function)
- [adc_lsb](#adc_lsb) (function)
- [adc_sign_extend](#adc_sign_extend) (function)
- [adc_to_volts](#adc_to_volts) (function)
- [adc_from_volts](#adc_from_volts) (function)
- [adc_to_mv_int](#adc_to_mv_int) (function)
- [adc_oversample](#adc_oversample) (function)
- [adc_average](#adc_average) (function)
- [AdcMovingAvg](#AdcMovingAvg) (struct)
- [adc_mavg_new](#adc_mavg_new) (function)
- [adc_mavg_push](#adc_mavg_push) (function)
- [AdcEma](#AdcEma) (struct)
- [adc_ema_new](#adc_ema_new) (function)
- [adc_ema_push](#adc_ema_push) (function)
- [adc_median](#adc_median) (function)
- [AdcMedian](#AdcMedian) (struct)
- [adc_median_new](#adc_median_new) (function)
- [adc_median_push](#adc_median_push) (function)
- [AdcCal](#AdcCal) (struct)
- [adc_cal_two_point](#adc_cal_two_point) (function)
- [adc_cal_apply](#adc_cal_apply) (function)
- [adc_cal_inverse](#adc_cal_inverse) (function)
- [adc_divider_vin](#adc_divider_vin) (function)
- [adc_ln](#adc_ln) (function)
- [adc_ntc_resistance](#adc_ntc_resistance) (function)
- [adc_ntc_celsius](#adc_ntc_celsius) (function)
- [adc_iio_dir](#adc_iio_dir) (function)
- [adc_iio_millivolts](#adc_iio_millivolts) (function)
- [adc_iio_read_mv](#adc_iio_read_mv) (function)
- [AdcScanType](#AdcScanType) (struct)
- [adc_parse_scan_type](#adc_parse_scan_type) (function)
- [adc_scan_decode](#adc_scan_decode) (function)

## AdcSpec

```fides
pub struct AdcSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `bits` | `i64` |
| `vref` | `f64` |
| `bipolar` | `bool` |
| `minus_one` | `bool` |

*Defined at line 28*

---

## adc_spec

A unipolar converter: codes 0..2^bits-1 over 0..vref (LSB = vref/2^bits).

```fides
pub fn adc_spec(bits: i64, vref: f64) -> AdcSpec {
```

**Parameters:**

- `bits`: `i64`
- `vref`: `f64`

**Returns:** `AdcSpec`

*Defined at line 36*

---

## adc_spec_full

A converter whose full-scale code means exactly vref (LSB = vref/(2^bits-1)).

```fides
pub fn adc_spec_full(bits: i64, vref: f64) -> AdcSpec {
```

**Parameters:**

- `bits`: `i64`
- `vref`: `f64`

**Returns:** `AdcSpec`

*Defined at line 43*

---

## adc_spec_bipolar

A bipolar converter: two's complement codes over -fs..+fs (LSB = fs / 2^(bits-1)).

```fides
pub fn adc_spec_bipolar(bits: i64, full_scale: f64) -> AdcSpec {
```

**Parameters:**

- `bits`: `i64`
- `full_scale`: `f64`

**Returns:** `AdcSpec`

*Defined at line 50*

---

## adc_lsb

Volts per code.

```fides
pub fn adc_lsb(s: &AdcSpec) -> f64 {
```

**Parameters:**

- `s`: `&AdcSpec`

**Returns:** `f64`

*Defined at line 57*

---

## adc_sign_extend

The low `bits` bits of raw as a signed number.

```fides
pub fn adc_sign_extend(raw: i64, bits: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 68*

---

## adc_to_volts

```fides
pub fn adc_to_volts(s: &AdcSpec, raw: i64) -> f64 {
```

**Parameters:**

- `s`: `&AdcSpec`
- `raw`: `i64`

**Returns:** `f64`

*Defined at line 76*

---

## adc_from_volts

The code for a voltage (nearest, clamped to the range).

```fides
pub fn adc_from_volts(s: &AdcSpec, v: f64) -> i64 {
```

**Parameters:**

- `s`: `&AdcSpec`
- `v`: `f64`

**Returns:** `i64`

*Defined at line 84*

---

## adc_to_mv_int

Integer path: millivolts for a unipolar code, rounded (no floats, for
small MCUs): raw * vref_mv / 2^bits.

```fides
pub fn adc_to_mv_int(raw: i64, bits: i64, vref_mv: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`
- `bits`: `i64`
- `vref_mv`: `i64`

**Returns:** `i64`

*Defined at line 109*

---

## adc_oversample

Decimate 4^extra_bits samples into one code with extra_bits more
resolution. Uses the first 4^extra_bits samples; -1 if too few.

```fides
pub fn adc_oversample(samples: &Vec<i64>, extra_bits: i64) -> i64 {
```

**Parameters:**

- `samples`: `&Vec<i64>`
- `extra_bits`: `i64`

**Returns:** `i64`

*Defined at line 117*

---

## adc_average

Plain average of n samples, rounded.

```fides
pub fn adc_average(samples: &Vec<i64>) -> i64 {
```

**Parameters:**

- `samples`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 132*

---

## AdcMovingAvg

Moving average over the last `size` samples (ring buffer, running sum).

```fides
pub struct AdcMovingAvg {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `Vec<i64>` |
| `pos` | `i64` |
| `count` | `i64` |
| `sum` | `i64` |

*Defined at line 148*

---

## adc_mavg_new

```fides
pub fn adc_mavg_new(size: i64) -> AdcMovingAvg {
```

**Parameters:**

- `size`: `i64`

**Returns:** `AdcMovingAvg`

*Defined at line 155*

---

## adc_mavg_push

Add a sample; the average of what the window holds (rounded down).

```fides
pub fn adc_mavg_push(m: &mut AdcMovingAvg, x: i64) -> i64 {
```

**Parameters:**

- `m`: `&mut AdcMovingAvg`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 162*

---

## AdcEma

Exponential moving average in fixed point: y += (x - y) >> shift, y
held with `shift` fractional bits so no precision is lost.

```fides
pub struct AdcEma {
```

**Fields:**

| Name | Type |
|------|------|
| `acc` | `i64` |
| `shift` | `i64` |
| `primed` | `bool` |

*Defined at line 176*

---

## adc_ema_new

```fides
pub fn adc_ema_new(shift: i64) -> AdcEma {
```

**Parameters:**

- `shift`: `i64`

**Returns:** `AdcEma`

*Defined at line 182*

---

## adc_ema_push

```fides
pub fn adc_ema_push(e: &mut AdcEma, x: i64) -> i64 {
```

**Parameters:**

- `e`: `&mut AdcEma`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 188*

---

## adc_median

Median of a set of samples (the lower middle one for an even count).

```fides
pub fn adc_median(v: &Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 216*

---

## AdcMedian

Median over the last `size` samples: removes spikes.

```fides
pub struct AdcMedian {
```

**Fields:**

| Name | Type |
|------|------|
| `win` | `Vec<i64>` |
| `size` | `i64` |

*Defined at line 225*

---

## adc_median_new

```fides
pub fn adc_median_new(size: i64) -> AdcMedian {
```

**Parameters:**

- `size`: `i64`

**Returns:** `AdcMedian`

*Defined at line 230*

---

## adc_median_push

```fides
pub fn adc_median_push(m: &mut AdcMedian, x: i64) -> i64 {
```

**Parameters:**

- `m`: `&mut AdcMedian`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 236*

---

## AdcCal

value = gain * raw + offset.

```fides
pub struct AdcCal {
```

**Fields:**

| Name | Type |
|------|------|
| `gain` | `f64` |
| `offset` | `f64` |

*Defined at line 247*

---

## adc_cal_two_point

From two points: raw1 reads ref1, raw2 reads ref2 (raw1 != raw2).

```fides
pub fn adc_cal_two_point(raw1: i64, ref1: f64, raw2: i64, ref2: f64) -> AdcCal {
```

**Parameters:**

- `raw1`: `i64`
- `ref1`: `f64`
- `raw2`: `i64`
- `ref2`: `f64`

**Returns:** `AdcCal`

*Defined at line 253*

---

## adc_cal_apply

```fides
pub fn adc_cal_apply(c: &AdcCal, raw: i64) -> f64 {
```

**Parameters:**

- `c`: `&AdcCal`
- `raw`: `i64`

**Returns:** `f64`

*Defined at line 265*

---

## adc_cal_inverse

The raw code that reads `value` after calibration.

```fides
pub fn adc_cal_inverse(c: &AdcCal, value: f64) -> f64 {
```

**Parameters:**

- `c`: `&AdcCal`
- `value`: `f64`

**Returns:** `f64`

*Defined at line 270*

---

## adc_divider_vin

Input voltage of a divider: vin -> r1 -> (vout) -> r2 -> ground.

```fides
pub fn adc_divider_vin(vout: f64, r1: f64, r2: f64) -> f64 {
```

**Parameters:**

- `vout`: `f64`
- `r1`: `f64`
- `r2`: `f64`

**Returns:** `f64`

*Defined at line 280*

---

## adc_ln

Natural logarithm (x > 0): x = m * 2^e with m in [sqrt(1/2), sqrt(2)),
ln m by the atanh series.

```fides
pub fn adc_ln(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 286*

---

## adc_ntc_resistance

NTC resistance from a divider reading: vref -> r_fixed -> (vout) -> NTC
-> ground.

```fides
pub fn adc_ntc_resistance(vout: f64, vref: f64, r_fixed: f64) -> f64 {
```

**Parameters:**

- `vout`: `f64`
- `vref`: `f64`
- `r_fixed`: `f64`

**Returns:** `f64`

*Defined at line 312*

---

## adc_ntc_celsius

Thermistor temperature (Celsius) by the Beta equation:
1/T = 1/T0 + ln(R/R0) / B, temperatures in kelvin.

```fides
pub fn adc_ntc_celsius(r: f64, r0: f64, t0_c: f64, beta: f64) -> f64 {
```

**Parameters:**

- `r`: `f64`
- `r0`: `f64`
- `t0_c`: `f64`
- `beta`: `f64`

**Returns:** `f64`

*Defined at line 321*

---

## adc_iio_dir

```fides
pub fn adc_iio_dir(root: str, dev: i64) -> str {
```

**Parameters:**

- `root`: `str`
- `dev`: `i64`

**Returns:** `str`

*Defined at line 329*

---

## adc_iio_millivolts

The channel's processed value in millivolts: (raw + offset) * scale,
as the IIO ABI defines it.

```fides
pub fn adc_iio_millivolts(raw: i64, offset: f64, scale: f64) -> f64 {
```

**Parameters:**

- `raw`: `i64`
- `offset`: `f64`
- `scale`: `f64`

**Returns:** `f64`

*Defined at line 335*

---

## adc_iio_read_mv

Read in_voltage<ch>_raw and its scale/offset (per channel, else the
shared in_voltage_scale / in_voltage_offset) and give millivolts.

```fides
pub fn adc_iio_read_mv(root: str, dev: i64, ch: i64) -> Result<f64, str> {
```

**Parameters:**

- `root`: `str`
- `dev`: `i64`
- `ch`: `i64`

**Returns:** `Result<f64, str>`

*Defined at line 352*

---

## AdcScanType

A buffered channel's format from scan_elements/<ch>_type:
"[be|le]:[s|u]<realbits>/<storagebits>[X<repeat>]>><shift>".

```fides
pub struct AdcScanType {
```

**Fields:**

| Name | Type |
|------|------|
| `big_endian` | `bool` |
| `signed` | `bool` |
| `realbits` | `i64` |
| `storagebits` | `i64` |
| `repeat` | `i64` |
| `shift` | `i64` |
| `ok` | `bool` |

*Defined at line 380*

---

## adc_parse_scan_type

```fides
pub fn adc_parse_scan_type(t: str) -> AdcScanType {
```

**Parameters:**

- `t`: `str`

**Returns:** `AdcScanType`

*Defined at line 408*

---

## adc_scan_decode

Decode one sample of a scan type at byte offset off in a buffer read.

```fides
pub fn adc_scan_decode(st: &AdcScanType, bytes: &Vec<i64>, off: i64) -> i64 {
```

**Parameters:**

- `st`: `&AdcScanType`
- `bytes`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 447*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
