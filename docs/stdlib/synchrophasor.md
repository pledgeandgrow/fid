[All modules](index.md)

# synchrophasor -- Fides Documentation

## Contents

- [C37_TYPE_DATA:](#C37_TYPE_DATA:) (const)
- [C37_TYPE_HEADER:](#C37_TYPE_HEADER:) (const)
- [C37_TYPE_CFG1:](#C37_TYPE_CFG1:) (const)
- [C37_TYPE_CFG2:](#C37_TYPE_CFG2:) (const)
- [C37_TYPE_CMD:](#C37_TYPE_CMD:) (const)
- [C37_TYPE_CFG3:](#C37_TYPE_CFG3:) (const)
- [C37_CMD_OFF:](#C37_CMD_OFF:) (const)
- [C37_CMD_ON:](#C37_CMD_ON:) (const)
- [C37_CMD_SEND_HDR:](#C37_CMD_SEND_HDR:) (const)
- [C37_CMD_SEND_CFG1:](#C37_CMD_SEND_CFG1:) (const)
- [C37_CMD_SEND_CFG2:](#C37_CMD_SEND_CFG2:) (const)
- [C37_CMD_SEND_CFG3:](#C37_CMD_SEND_CFG3:) (const)
- [C37_FMT_POLAR:](#C37_FMT_POLAR:) (const)
- [C37_FMT_PH_FLOAT:](#C37_FMT_PH_FLOAT:) (const)
- [C37_FMT_AN_FLOAT:](#C37_FMT_AN_FLOAT:) (const)
- [C37_FMT_FREQ_FLOAT:](#C37_FMT_FREQ_FLOAT:) (const)
- [C37Header](#C37Header) (struct)
- [C37PmuConfig](#C37PmuConfig) (struct)
- [C37Config](#C37Config) (struct)
- [C37PmuData](#C37PmuData) (struct)
- [C37Data](#C37Data) (struct)
- [c37_crc_range](#c37_crc_range) (function)
- [c37_crc](#c37_crc) (function)
- [c37_check_crc](#c37_check_crc) (function)
- [c37_u16](#c37_u16) (function)
- [c37_s16](#c37_s16) (function)
- [c37_u32](#c37_u32) (function)
- [c37_f32_bits](#c37_f32_bits) (function)
- [c37_f32_from_bits](#c37_f32_from_bits) (function)
- [c37_fracsec](#c37_fracsec) (function)
- [c37_parse_header](#c37_parse_header) (function)
- [c37_valid](#c37_valid) (function)
- [c37_encode_command](#c37_encode_command) (function)
- [c37_decode_command](#c37_decode_command) (function)
- [c37_encode_header_frame](#c37_encode_header_frame) (function)
- [c37_decode_header_frame](#c37_decode_header_frame) (function)
- [c37_pmu_config](#c37_pmu_config) (function)
- [c37_add_phasor](#c37_add_phasor) (function)
- [c37_add_analog](#c37_add_analog) (function)
- [c37_add_digital](#c37_add_digital) (function)
- [c37_encode_config](#c37_encode_config) (function)
- [c37_decode_config](#c37_decode_config) (function)
- [c37_nominal_hz](#c37_nominal_hz) (function)
- [c37_pmu_data](#c37_pmu_data) (function)
- [c37_encode_data](#c37_encode_data) (function)
- [c37_decode_data](#c37_decode_data) (function)
- [syn_pi](#syn_pi) (function)
- [syn_phasor](#syn_phasor) (function)
- [syn_positive_sequence](#syn_positive_sequence) (function)
- [syn_wrap](#syn_wrap) (function)
- [syn_frequency](#syn_frequency) (function)
- [syn_rocof](#syn_rocof) (function)
- [syn_tve](#syn_tve) (function)

## C37_TYPE_DATA:

```fides
pub const C37_TYPE_DATA: i64 = 0;
```

*Defined at line 44*

---

## C37_TYPE_HEADER:

```fides
pub const C37_TYPE_HEADER: i64 = 1;
```

*Defined at line 45*

---

## C37_TYPE_CFG1:

```fides
pub const C37_TYPE_CFG1: i64 = 2;
```

*Defined at line 46*

---

## C37_TYPE_CFG2:

```fides
pub const C37_TYPE_CFG2: i64 = 3;
```

*Defined at line 47*

---

## C37_TYPE_CMD:

```fides
pub const C37_TYPE_CMD: i64 = 4;
```

*Defined at line 48*

---

## C37_TYPE_CFG3:

```fides
pub const C37_TYPE_CFG3: i64 = 5;
```

*Defined at line 49*

---

## C37_CMD_OFF:

```fides
pub const C37_CMD_OFF: i64 = 1;
```

*Defined at line 51*

---

## C37_CMD_ON:

```fides
pub const C37_CMD_ON: i64 = 2;
```

*Defined at line 52*

---

## C37_CMD_SEND_HDR:

```fides
pub const C37_CMD_SEND_HDR: i64 = 3;
```

*Defined at line 53*

---

## C37_CMD_SEND_CFG1:

```fides
pub const C37_CMD_SEND_CFG1: i64 = 4;
```

*Defined at line 54*

---

## C37_CMD_SEND_CFG2:

```fides
pub const C37_CMD_SEND_CFG2: i64 = 5;
```

*Defined at line 55*

---

## C37_CMD_SEND_CFG3:

```fides
pub const C37_CMD_SEND_CFG3: i64 = 6;
```

*Defined at line 56*

---

## C37_FMT_POLAR:

```fides
pub const C37_FMT_POLAR: i64 = 1;
```

*Defined at line 59*

---

## C37_FMT_PH_FLOAT:

```fides
pub const C37_FMT_PH_FLOAT: i64 = 2;
```

*Defined at line 60*

---

## C37_FMT_AN_FLOAT:

```fides
pub const C37_FMT_AN_FLOAT: i64 = 4;
```

*Defined at line 61*

---

## C37_FMT_FREQ_FLOAT:

```fides
pub const C37_FMT_FREQ_FLOAT: i64 = 8;
```

*Defined at line 62*

---

## C37Header

The common frame header.

```fides
pub struct C37Header {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_type` | `i64` |
| `version` | `i64` |
| `framesize` | `i64` |
| `idcode` | `i64` |
| `soc` | `i64` |
| `time_quality` | `i64` |
| `fracsec` | `i64` |

*Defined at line 65*

---

## C37PmuConfig

One PMU of a configuration frame.

```fides
pub struct C37PmuConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `stn` | `str` |
| `idcode` | `i64` |
| `format` | `i64` |
| `phnmr` | `i64` |
| `annmr` | `i64` |
| `dgnmr` | `i64` |
| `chnam` | `Vec<str>` |
| `phunit` | `Vec<i64>` |
| `anunit` | `Vec<i64>` |
| `digunit` | `Vec<i64>` |
| `fnom` | `i64` |
| `cfgcnt` | `i64` |

*Defined at line 76*

---

## C37Config

```fides
pub struct C37Config {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_type` | `i64` |
| `idcode` | `i64` |
| `soc` | `i64` |
| `fracsec` | `i64` |
| `time_base` | `i64` |
| `pmus` | `Vec<C37PmuConfig>` |
| `data_rate` | `i64` |

*Defined at line 91*

---

## C37PmuData

Measurements of one PMU in a data frame. Phasors are rectangular
(re, im) in volts or amperes whatever the wire format.

```fides
pub struct C37PmuData {
```

**Fields:**

| Name | Type |
|------|------|
| `stat` | `i64` |
| `ph_re` | `Vec<f64>` |
| `ph_im` | `Vec<f64>` |
| `freq` | `f64` |
| `dfreq` | `f64` |
| `analog` | `Vec<f64>` |
| `digital` | `Vec<i64>` |

*Defined at line 103*

---

## C37Data

```fides
pub struct C37Data {
```

**Fields:**

| Name | Type |
|------|------|
| `idcode` | `i64` |
| `soc` | `i64` |
| `time_quality` | `i64` |
| `fracsec` | `i64` |
| `pmus` | `Vec<C37PmuData>` |

*Defined at line 113*

---

## c37_crc_range

CRC-CCITT (0x1021, init 0xFFFF) of bytes[start..end).

```fides
pub fn c37_crc_range(b: &Vec<i64>, start: i64, end: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `start`: `i64`
- `end`: `i64`

**Returns:** `i64`

*Defined at line 124*

---

## c37_crc

```fides
pub fn c37_crc(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 143*

---

## c37_check_crc

True when the last two bytes are the CRC of the rest and FRAMESIZE
matches the length.

```fides
pub fn c37_check_crc(b: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 149*

---

## c37_u16

```fides
pub fn c37_u16(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 172*

---

## c37_s16

```fides
pub fn c37_s16(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 176*

---

## c37_u32

```fides
pub fn c37_u32(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 184*

---

## c37_f32_bits

The 32-bit pattern of x as an IEEE 754 single.

```fides
pub fn c37_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 223*

---

## c37_f32_from_bits

The value of a 32-bit IEEE 754 single pattern.

```fides
pub fn c37_f32_from_bits(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 266*

---

## c37_fracsec

FRACSEC word from the time-quality byte and the 24-bit fraction count.

```fides
pub fn c37_fracsec(time_quality: i64, fraction: i64) -> i64 {
```

**Parameters:**

- `time_quality`: `i64`
- `fraction`: `i64`

**Returns:** `i64`

*Defined at line 318*

---

## c37_parse_header

```fides
pub fn c37_parse_header(b: &Vec<i64>) -> C37Header {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `C37Header`

*Defined at line 322*

---

## c37_valid

A frame is well formed: SYNC byte, FRAMESIZE and CRC.

```fides
pub fn c37_valid(b: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 329*

---

## c37_encode_command

```fides
pub fn c37_encode_command(idcode: i64, soc: i64, fracsec: i64, cmd: i64) -> Vec<i64> {
```

**Parameters:**

- `idcode`: `i64`
- `soc`: `i64`
- `fracsec`: `i64`
- `cmd`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 338*

---

## c37_decode_command

The CMD word of a command frame (-1 when the frame is not one).

```fides
pub fn c37_decode_command(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 346*

---

## c37_encode_header_frame

```fides
pub fn c37_encode_header_frame(idcode: i64, soc: i64, fracsec: i64, text: str) -> Vec<i64> {
```

**Parameters:**

- `idcode`: `i64`
- `soc`: `i64`
- `fracsec`: `i64`
- `text`: `str`

**Returns:** `Vec<i64>`

*Defined at line 353*

---

## c37_decode_header_frame

```fides
pub fn c37_decode_header_frame(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 365*

---

## c37_pmu_config

A PMU configuration with no channels yet.

```fides
pub fn c37_pmu_config(stn: str, idcode: i64, format: i64, nominal_50hz: bool) -> C37PmuConfig {
```

**Parameters:**

- `stn`: `str`
- `idcode`: `i64`
- `format`: `i64`
- `nominal_50hz`: `bool`

**Returns:** `C37PmuConfig`

*Defined at line 378*

---

## c37_add_phasor

Add a phasor channel; `is_current` selects amperes, `scale` is the
integer-format scale in 10^-5 V (or A) per bit.

```fides
pub fn c37_add_phasor(p: &mut C37PmuConfig, name: str, is_current: bool, scale: i64) {
```

**Parameters:**

- `p`: `&mut C37PmuConfig`
- `name`: `str`
- `is_current`: `bool`
- `scale`: `i64`

*Defined at line 390*

---

## c37_add_analog

```fides
pub fn c37_add_analog(p: &mut C37PmuConfig, name: str, anunit: i64) {
```

**Parameters:**

- `p`: `&mut C37PmuConfig`
- `name`: `str`
- `anunit`: `i64`

*Defined at line 412*

---

## c37_add_digital

A digital status word: 16 bit names and its DIGUNIT (normal mask << 16 |
valid mask).

```fides
pub fn c37_add_digital(p: &mut C37PmuConfig, bit_names: Vec<str>, normal_mask: i64, valid_mask: i64) {
```

**Parameters:**

- `p`: `&mut C37PmuConfig`
- `bit_names`: `Vec<str>`
- `normal_mask`: `i64`
- `valid_mask`: `i64`

*Defined at line 432*

---

## c37_encode_config

```fides
pub fn c37_encode_config(cfg: &C37Config) -> Vec<i64> {
```

**Parameters:**

- `cfg`: `&C37Config`

**Returns:** `Vec<i64>`

*Defined at line 473*

---

## c37_decode_config

Decode a CFG-1 or CFG-2 frame. Check `c37_valid` first.

```fides
pub fn c37_decode_config(b: &Vec<i64>) -> C37Config {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `C37Config`

*Defined at line 516*

---

## c37_nominal_hz

Nominal frequency of a PMU configuration in Hz.

```fides
pub fn c37_nominal_hz(p: &C37PmuConfig) -> f64 {
```

**Parameters:**

- `p`: `&C37PmuConfig`

**Returns:** `f64`

*Defined at line 570*

---

## c37_pmu_data

```fides
pub fn c37_pmu_data(stat: i64, freq: f64, dfreq: f64) -> C37PmuData {
```

**Parameters:**

- `stat`: `i64`
- `freq`: `f64`
- `dfreq`: `f64`

**Returns:** `C37PmuData`

*Defined at line 579*

---

## c37_encode_data

Encode a data frame for the configuration `cfg` (one C37PmuData per
PMU, in order).

```fides
pub fn c37_encode_data(cfg: &C37Config, d: &C37Data) -> Vec<i64> {
```

**Parameters:**

- `cfg`: `&C37Config`
- `d`: `&C37Data`

**Returns:** `Vec<i64>`

*Defined at line 591*

---

## c37_decode_data

Decode a data frame with the stream's configuration.

```fides
pub fn c37_decode_data(cfg: &C37Config, b: &Vec<i64>) -> C37Data {
```

**Parameters:**

- `cfg`: `&C37Config`
- `b`: `&Vec<i64>`

**Returns:** `C37Data`

*Defined at line 652*

---

## syn_pi

```fides
pub fn syn_pi() -> f64 {
```

**Returns:** `f64`

*Defined at line 727*

---

## syn_phasor

One-cycle DFT phasor (RMS magnitude) of x[start .. start + n), with n
samples per nominal cycle. The angle is referenced to a nominal-frequency
cosine at sample 0 of `x` (absolute time), so a steady signal at
nominal frequency gives the same phasor in every window.

```fides
pub fn syn_phasor(x: &Vec<f64>, start: i64, n: i64) -> Complex {
```

**Parameters:**

- `x`: `&Vec<f64>`
- `start`: `i64`
- `n`: `i64`

**Returns:** `Complex`

*Defined at line 735*

---

## syn_positive_sequence

Positive-sequence phasor (Va + a Vb + a^2 Vc) / 3, a = 1 at 120 deg.

```fides
pub fn syn_positive_sequence(va: Complex, vb: Complex, vc: Complex) -> Complex {
```

**Parameters:**

- `va`: `Complex`
- `vb`: `Complex`
- `vc`: `Complex`

**Returns:** `Complex`

*Defined at line 751*

---

## syn_wrap

Wrap an angle to (-pi, pi].

```fides
pub fn syn_wrap(a: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`

**Returns:** `f64`

*Defined at line 759*

---

## syn_frequency

Frequency from two phasors `dt` seconds apart: the phasor of a signal
at f0 + df turns at 2 pi df rad/s.

```fides
pub fn syn_frequency(prev: Complex, cur: Complex, dt: f64, f0: f64) -> f64 {
```

**Parameters:**

- `prev`: `Complex`
- `cur`: `Complex`
- `dt`: `f64`
- `f0`: `f64`

**Returns:** `f64`

*Defined at line 772*

---

## syn_rocof

ROCOF from two frequency estimates `dt` seconds apart (Hz/s).

```fides
pub fn syn_rocof(f_prev: f64, f_cur: f64, dt: f64) -> f64 {
```

**Parameters:**

- `f_prev`: `f64`
- `f_cur`: `f64`
- `dt`: `f64`

**Returns:** `f64`

*Defined at line 778*

---

## syn_tve

Total vector error |est - ref| / |ref| (C37.118.1).

```fides
pub fn syn_tve(est: Complex, reference: Complex) -> f64 {
```

**Parameters:**

- `est`: `Complex`
- `reference`: `Complex`

**Returns:** `f64`

*Defined at line 783*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
