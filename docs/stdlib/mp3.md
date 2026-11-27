[All modules](index.md)

# mp3 -- Fides Documentation

## Contents

- [Mp3Header](#Mp3Header) (struct)
- [mp3_parse_header](#mp3_parse_header) (function)
- [Mp3Bits](#Mp3Bits) (struct)
- [mp3_bits](#mp3_bits) (function)
- [mp3_read](#mp3_read) (function)
- [Mp3HuffCheck](#Mp3HuffCheck) (struct)
- [mp3_huff_check](#mp3_huff_check) (function)
- [mp3_prefix_code_check](#mp3_prefix_code_check) (function)
- [mp3_huff_pair](#mp3_huff_pair) (function)
- [mp3_linbits](#mp3_linbits) (function)
- [Mp3Granule](#Mp3Granule) (struct)
- [Mp3Side](#Mp3Side) (struct)
- [mp3_parse_side](#mp3_parse_side) (function)
- [Mp3Decoder](#Mp3Decoder) (struct)
- [mp3_decoder](#mp3_decoder) (function)
- [mp3_imdct36](#mp3_imdct36) (function)
- [mp3_synth_window](#mp3_synth_window) (function)
- [mp3_synthesize](#mp3_synthesize) (function)
- [Mp3Frame](#Mp3Frame) (struct)
- [mp3_decode_frame](#mp3_decode_frame) (function)
- [mp3_l2_table](#mp3_l2_table) (function)
- [mp3_l2_sblimit](#mp3_l2_sblimit) (function)
- [mp3_free_frame_len](#mp3_free_frame_len) (function)
- [mp3_id3_size](#mp3_id3_size) (function)
- [Mp3File](#Mp3File) (struct)
- [mp3_decode_stream](#mp3_decode_stream) (function)
- [mp3_decode](#mp3_decode) (function)

## Mp3Header

```fides
pub struct Mp3Header {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `version` | `i64` |
| `layer` | `i64` |
| `has_crc` | `bool` |
| `bitrate` | `i64` |
| `rate` | `i64` |
| `padding` | `i64` |
| `mode` | `i64` |
| `mode_ext` | `i64` |
| `channels` | `i64` |
| `frame_len` | `i64` |
| `samples` | `i64` |
| `side_len` | `i64` |
| `free` | `bool,  // free format (bitrate index 0): bitrate and frame_len are 0` |

*Defined at line 68*

---

## mp3_parse_header

Parse the 4-byte header at `at`.

```fides
pub fn mp3_parse_header(b: &Vec<i64>, at: i64) -> Mp3Header {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `Mp3Header`

*Defined at line 126*

---

## Mp3Bits

```fides
pub struct Mp3Bits {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `pos` | `i64` |

*Defined at line 216*

---

## mp3_bits

```fides
pub fn mp3_bits(data: Vec<i64>) -> Mp3Bits {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Mp3Bits`

*Defined at line 221*

---

## mp3_read

Read `n` (0..32) bits, most significant first; zeros past the end.

```fides
pub fn mp3_read(r: &mut Mp3Bits, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Mp3Bits`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 228*

---

## Mp3HuffCheck

```fides
pub struct Mp3HuffCheck {
```

**Fields:**

| Name | Type |
|------|------|
| `codes` | `i64` |
| `max_len` | `i64` |
| `prefix_free` | `bool` |
| `complete` | `bool,  // sum of 2^-len over the codewords is exactly 1` |

*Defined at line 674*

---

## mp3_huff_check

Check Huffman table `t` (1..32) as a code: count, longest codeword,
prefix-freeness and completeness.

```fides
pub fn mp3_huff_check(t: i64) -> Mp3HuffCheck {
```

**Parameters:**

- `t`: `i64`

**Returns:** `Mp3HuffCheck`

*Defined at line 684*

---

## mp3_prefix_code_check

Check a code given as codewords (MSB first) and their lengths.

```fides
pub fn mp3_prefix_code_check(cs: &Vec<i64>, ls: &Vec<i64>) -> Mp3HuffCheck {
```

**Parameters:**

- `cs`: `&Vec<i64>`
- `ls`: `&Vec<i64>`

**Returns:** `Mp3HuffCheck`

*Defined at line 706*

---

## mp3_huff_pair

Decode one big-values pair with table `t` (1..31); x * 16 + y, or -1
(count1 table A is t = 32: its value is v w x y as 4 bits).

```fides
pub fn mp3_huff_pair(t: i64, r: &mut Mp3Bits) -> i64 {
```

**Parameters:**

- `t`: `i64`
- `r`: `&mut Mp3Bits`

**Returns:** `i64`

*Defined at line 783*

---

## mp3_linbits

The linbits of table `t`.

```fides
pub fn mp3_linbits(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 816*

---

## Mp3Granule

```fides
pub struct Mp3Granule {
```

**Fields:**

| Name | Type |
|------|------|
| `part2_3_length` | `i64` |
| `big_values` | `i64` |
| `global_gain` | `i64` |
| `scalefac_compress` | `i64` |
| `window_switching` | `bool` |
| `block_type` | `i64` |
| `mixed` | `bool` |
| `table_select` | `Vec<i64>` |
| `subblock_gain` | `Vec<i64>` |
| `region0_count` | `i64` |
| `region1_count` | `i64` |
| `preflag` | `i64` |
| `scalefac_scale` | `i64` |
| `count1_table` | `i64` |

*Defined at line 890*

---

## Mp3Side

```fides
pub struct Mp3Side {
```

**Fields:**

| Name | Type |
|------|------|
| `main_data_begin` | `i64` |
| `scfsi` | `Vec<i64>` |
| `granules` | `Vec<Mp3Granule>,  // [gr * channels + ch]` |

*Defined at line 907*

---

## mp3_parse_side

Parse Layer III side information (the bytes after the header and CRC).

```fides
pub fn mp3_parse_side(hd: &Mp3Header, side: Vec<i64>) -> Mp3Side {
```

**Parameters:**

- `hd`: `&Mp3Header`
- `side`: `Vec<i64>`

**Returns:** `Mp3Side`

*Defined at line 915*

---

## Mp3Decoder

```fides
pub struct Mp3Decoder {
```

**Fields:**

| Name | Type |
|------|------|
| `reservoir` | `Vec<i64>` |
| `overlap` | `Vec<Vec<f64>>` |
| `vbuf` | `Vec<Vec<f64>>` |
| `voff` | `Vec<i64>` |
| `scf_prev` | `Vec<Vec<i64>>` |
| `frames` | `i64` |
| `error` | `str` |
| `free_len` | `i64,  // free format: frame bytes without the padding slot, 0 until known` |

*Defined at line 1007*

---

## mp3_decoder

```fides
pub fn mp3_decoder() -> Mp3Decoder {
```

**Returns:** `Mp3Decoder`

*Defined at line 1022*

---

## mp3_imdct36

The 36-point IMDCT of 18 values: x_i = sum_k X_k cos(pi/72 (2i + 19)(2k + 1)).

```fides
pub fn mp3_imdct36(x: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1648*

---

## mp3_synth_window

D[i] of the synthesis window, i in 0..511.

```fides
pub fn mp3_synth_window(i: i64) -> f64 {
```

**Parameters:**

- `i`: `i64`

**Returns:** `f64`

*Defined at line 1773*

---

## mp3_synthesize

Run `n` time samples of the 32 subbands (sub[k * n + t]) through
channel `ch`'s synthesis filterbank of `d`: 32 * n PCM samples.

```fides
pub fn mp3_synthesize(d: &mut Mp3Decoder, ch: i64, sub: &Vec<f64>, n: i64) -> Vec<f64> {
```

**Parameters:**

- `d`: `&mut Mp3Decoder`
- `ch`: `i64`
- `sub`: `&Vec<f64>`
- `n`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 1780*

---

## Mp3Frame

```fides
pub struct Mp3Frame {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `header` | `Mp3Header` |
| `pcm` | `Vec<Vec<f64>>,  // per channel; empty while the reservoir fills` |

*Defined at line 1871*

---

## mp3_decode_frame

Decode the frame (Layer I, II or III) at `at`.

```fides
pub fn mp3_decode_frame(d: &mut Mp3Decoder, b: &Vec<i64>, at: i64) -> Mp3Frame {
```

**Parameters:**

- `d`: `&mut Mp3Decoder`
- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `Mp3Frame`

*Defined at line 1879*

---

## mp3_l2_table

The Layer II allocation table of a frame: 0..3 are ISO 11172-3 Tables
3-B.2a..d (chosen by sample rate and bitrate per channel; a
free-format stream uses B.2a at 48 kHz, B.2b otherwise), 4 is ISO
13818-3 Table B.1 (the MPEG-2 rates).

```fides
pub fn mp3_l2_table(hd: &Mp3Header) -> i64 {
```

**Parameters:**

- `hd`: `&Mp3Header`

**Returns:** `i64`

*Defined at line 2041*

---

## mp3_l2_sblimit

Subbands with an allocation (sblimit) in Layer II table `tbl`.

```fides
pub fn mp3_l2_sblimit(tbl: i64) -> i64 {
```

**Parameters:**

- `tbl`: `i64`

**Returns:** `i64`

*Defined at line 2072*

---

## mp3_free_frame_len

Bytes of the free-format frame at `at` (header included): the distance
to the next header of the same stream (same version, layer, sample
rate, CRC flag and channel count, bitrate index 0) whose own frame ends
at a further such header or at the end of `b`. 0 when there is none.

```fides
pub fn mp3_free_frame_len(b: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 2391*

---

## mp3_id3_size

Bytes of an ID3v2 tag at the start (0 when there is none).

```fides
pub fn mp3_id3_size(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 2458*

---

## Mp3File

```fides
pub struct Mp3File {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `version` | `i64` |
| `layer` | `i64` |
| `rate` | `i64` |
| `channels` | `i64` |
| `free_format` | `bool` |
| `frames` | `i64` |
| `info_frame` | `bool` |
| `delay` | `i64` |
| `padding` | `i64` |
| `pcm` | `Vec<Vec<f64>>` |

*Defined at line 2470*

---

## mp3_decode_stream

Decode every frame. With `gapless`, the LAME tag's encoder delay (+ the
529 samples of decoder delay) is cut from the start and its padding from
the end, as ffmpeg and LAME's own decoder do.

```fides
pub fn mp3_decode_stream(b: &Vec<i64>, gapless: bool) -> Mp3File {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `gapless`: `bool`

**Returns:** `Mp3File`

*Defined at line 2511*

---

## mp3_decode

Decode a whole MP3 file without gapless trimming.

```fides
pub fn mp3_decode(b: &Vec<i64>) -> Mp3File {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Mp3File`

*Defined at line 2610*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
