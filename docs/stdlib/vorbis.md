[All modules](index.md)

# vorbis -- Fides Documentation

## Contents

- [ogg_crc_update](#ogg_crc_update) (function)
- [ogg_crc](#ogg_crc) (function)
- [OGG_CONTINUED:](#OGG_CONTINUED:) (const)
- [OGG_BOS:](#OGG_BOS:) (const)
- [OGG_EOS:](#OGG_EOS:) (const)
- [OggPage](#OggPage) (struct)
- [ogg_parse_page](#ogg_parse_page) (function)
- [ogg_page_encode](#ogg_page_encode) (function)
- [OggStream](#OggStream) (struct)
- [ogg_read](#ogg_read) (function)
- [ogg_paginate](#ogg_paginate) (function)
- [VbBits](#VbBits) (struct)
- [vorbis_bits](#vorbis_bits) (function)
- [vorbis_read](#vorbis_read) (function)
- [vorbis_ilog](#vorbis_ilog) (function)
- [vorbis_float32_unpack](#vorbis_float32_unpack) (function)
- [vorbis_lookup1_values](#vorbis_lookup1_values) (function)
- [VbBook](#VbBook) (struct)
- [vorbis_book_from_lengths](#vorbis_book_from_lengths) (function)
- [vorbis_book_decode](#vorbis_book_decode) (function)
- [VbFloor](#VbFloor) (struct)
- [VbResidue](#VbResidue) (struct)
- [VbMapping](#VbMapping) (struct)
- [VbMode](#VbMode) (struct)
- [VorbisDecoder](#VorbisDecoder) (struct)
- [vorbis_decoder](#vorbis_decoder) (function)
- [vorbis_header](#vorbis_header) (function)
- [vorbis_floor0_map](#vorbis_floor0_map) (function)
- [vorbis_floor0_curve](#vorbis_floor0_curve) (function)
- [vorbis_imdct](#vorbis_imdct) (function)
- [vorbis_decode_packet](#vorbis_decode_packet) (function)
- [VorbisFile](#VorbisFile) (struct)
- [vorbis_decode_file](#vorbis_decode_file) (function)

## ogg_crc_update

Ogg CRC-32 of bytes[from..to) (continue from `crc`; start at 0).

```fides
pub fn ogg_crc_update(crc: i64, b: &Vec<i64>, from: i64, to: i64) -> i64 {
```

**Parameters:**

- `crc`: `i64`
- `b`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## ogg_crc

```fides
pub fn ogg_crc(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 89*

---

## OGG_CONTINUED:

```fides
pub const OGG_CONTINUED: i64 = 1;
```

*Defined at line 93*

---

## OGG_BOS:

```fides
pub const OGG_BOS: i64 = 2;
```

*Defined at line 94*

---

## OGG_EOS:

```fides
pub const OGG_EOS: i64 = 4;
```

*Defined at line 95*

---

## OggPage

One Ogg page.

```fides
pub struct OggPage {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `header_type` | `i64` |
| `granule` | `i64` |
| `serial` | `i64` |
| `seq` | `i64` |
| `crc` | `i64` |
| `crc_ok` | `bool` |
| `lacing` | `Vec<i64>` |
| `body` | `Vec<i64>` |
| `size` | `i64,  // bytes of the whole page` |

*Defined at line 98*

---

## ogg_parse_page

```fides
pub fn ogg_parse_page(b: &Vec<i64>, at: i64) -> OggPage {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `OggPage`

*Defined at line 127*

---

## ogg_page_encode

```fides
pub fn ogg_page_encode(p: &OggPage) -> Vec<i64> {
```

**Parameters:**

- `p`: `&OggPage`

**Returns:** `Vec<i64>`

*Defined at line 183*

---

## OggStream

A logical stream split into packets.

```fides
pub struct OggStream {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `serial` | `i64` |
| `pages` | `i64` |
| `crc_errors` | `i64` |
| `packets` | `Vec<Vec<i64>>` |
| `granules` | `Vec<i64>` |
| `last_granule` | `i64` |

*Defined at line 231*

---

## ogg_read

Read every page of the first logical stream in `b` and assemble its
packets. Pages with a bad CRC are an error.

```fides
pub fn ogg_read(b: &Vec<i64>) -> OggStream {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `OggStream`

*Defined at line 245*

---

## ogg_paginate

Pack `packets` into pages of stream `serial` (pages of at most 255
lacing values; one page per packet unless a packet needs more).
`granules[i]` is the granule position after packet i. The first page is
marked BOS, the last EOS.

```fides
pub fn ogg_paginate(packets: &Vec<Vec<i64>>, granules: &Vec<i64>, serial: i64) -> Vec<i64> {
```

**Parameters:**

- `packets`: `&Vec<Vec<i64>>`
- `granules`: `&Vec<i64>`
- `serial`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 319*

---

## VbBits

```fides
pub struct VbBits {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `pos` | `i64` |
| `nbits` | `i64` |
| `eop` | `bool,  // a read went past the end of the packet` |

*Defined at line 379*

---

## vorbis_bits

```fides
pub fn vorbis_bits(data: Vec<i64>) -> VbBits {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `VbBits`

*Defined at line 387*

---

## vorbis_read

Read `n` (0..32) bits as an unsigned integer; past the end: 0 and eop.

```fides
pub fn vorbis_read(r: &mut VbBits, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut VbBits`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 395*

---

## vorbis_ilog

ilog: the position of the highest set bit (ilog(0) = 0, ilog(7) = 3).

```fides
pub fn vorbis_ilog(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 431*

---

## vorbis_float32_unpack

float32_unpack of the spec: 21-bit mantissa, sign, 10-bit exponent
biased by 788.

```fides
pub fn vorbis_float32_unpack(x: i64) -> f64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `f64`

*Defined at line 443*

---

## vorbis_lookup1_values

lookup1_values: the largest r with r^dims <= entries.

```fides
pub fn vorbis_lookup1_values(entries: i64, dims: i64) -> i64 {
```

**Parameters:**

- `entries`: `i64`
- `dims`: `i64`

**Returns:** `i64`

*Defined at line 453*

---

## VbBook

```fides
pub struct VbBook {
```

**Fields:**

| Name | Type |
|------|------|
| `dims` | `i64` |
| `entries` | `i64` |
| `lengths` | `Vec<i64>` |
| `left` | `Vec<i64>` |
| `right` | `Vec<i64>` |
| `lookup` | `i64` |
| `vq` | `Vec<f64>` |
| `single` | `i64,  // the entry of a book with one used entry, else -1` |

*Defined at line 483*

---

## vorbis_book_from_lengths

A scalar codebook (no VQ) with these codeword lengths (0: unused
entry), codewords assigned as the spec does. `entries` is -1 when the
lengths are over-specified.

```fides
pub fn vorbis_book_from_lengths(lengths: Vec<i64>) -> VbBook {
```

**Parameters:**

- `lengths`: `Vec<i64>`

**Returns:** `VbBook`

*Defined at line 620*

---

## vorbis_book_decode

Decode one entry number with `bk` (-1 at the end of the packet or on a
codeword the book does not have).

```fides
pub fn vorbis_book_decode(bk: &VbBook, r: &mut VbBits) -> i64 {
```

**Parameters:**

- `bk`: `&VbBook`
- `r`: `&mut VbBits`

**Returns:** `i64`

*Defined at line 633*

---

## VbFloor

```fides
pub struct VbFloor {
```

**Fields:**

| Name | Type |
|------|------|
| `partitions` | `i64` |
| `part_class` | `Vec<i64>` |
| `class_dims` | `Vec<i64>` |
| `class_subs` | `Vec<i64>` |
| `class_master` | `Vec<i64>` |
| `sub_books` | `Vec<i64>` |
| `multiplier` | `i64` |
| `xlist` | `Vec<i64>` |
| `sorted` | `Vec<i64>` |
| `low_n` | `Vec<i64>` |
| `high_n` | `Vec<i64>` |
| `ftype` | `i64` |
| `order` | `i64` |
| `f0_rate` | `i64` |
| `bark_size` | `i64` |
| `amp_bits` | `i64` |
| `amp_offset` | `i64` |
| `f0_books` | `Vec<i64>` |
| `map0` | `Vec<i64>` |
| `map1` | `Vec<i64>,  // and of the long blocks` |

*Defined at line 763*

---

## VbResidue

```fides
pub struct VbResidue {
```

**Fields:**

| Name | Type |
|------|------|
| `rtype` | `i64` |
| `begin` | `i64` |
| `end` | `i64` |
| `psize` | `i64` |
| `nclass` | `i64` |
| `classbook` | `i64` |
| `books` | `Vec<i64>,  // [class * 8 + pass], -1: none` |

*Defined at line 790*

---

## VbMapping

```fides
pub struct VbMapping {
```

**Fields:**

| Name | Type |
|------|------|
| `submaps` | `i64` |
| `mag` | `Vec<i64>` |
| `ang` | `Vec<i64>` |
| `mux` | `Vec<i64>` |
| `sub_floor` | `Vec<i64>` |
| `sub_residue` | `Vec<i64>` |

*Defined at line 800*

---

## VbMode

```fides
pub struct VbMode {
```

**Fields:**

| Name | Type |
|------|------|
| `blockflag` | `i64` |
| `mapping` | `i64` |

*Defined at line 809*

---

## VorbisDecoder

Decoder state: the three headers, then the overlap of the last block.

```fides
pub struct VorbisDecoder {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `channels` | `i64` |
| `rate` | `i64` |
| `bitrate_max` | `i64` |
| `bitrate_nominal` | `i64` |
| `bitrate_min` | `i64` |
| `bs0` | `i64` |
| `bs1` | `i64` |
| `vendor` | `str` |
| `comments` | `Vec<str>` |
| `books` | `Vec<VbBook>` |
| `floors` | `Vec<VbFloor>` |
| `residues` | `Vec<VbResidue>` |
| `mappings` | `Vec<VbMapping>` |
| `modes` | `Vec<VbMode>` |
| `headers` | `i64` |
| `prev` | `Vec<Vec<f64>>` |
| `prev_n` | `i64` |
| `prev_long` | `bool` |

*Defined at line 815*

---

## vorbis_decoder

```fides
pub fn vorbis_decoder() -> VorbisDecoder {
```

**Returns:** `VorbisDecoder`

*Defined at line 841*

---

## vorbis_header

Feed one of the three header packets (in order). False on an error
(d.error says why).

```fides
pub fn vorbis_header(d: &mut VorbisDecoder, packet: Vec<i64>) -> bool {
```

**Parameters:**

- `d`: `&mut VorbisDecoder`
- `packet`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 888*

---

## vorbis_floor0_map

The floor 0 bark map of a block of `n` (half the blocksize) values
(Vorbis I spec 6.2.3): map[i] = min(bark_size - 1, floor(bark(rate i /
2n) * bark_size / bark(rate / 2))).

```fides
pub fn vorbis_floor0_map(n: i64, rate: i64, bark_size: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`
- `rate`: `i64`
- `bark_size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1373*

---

## vorbis_floor0_curve

The floor 0 curve (spec 6.2.3) over a bark map: for each run of equal
map values, omega = pi map[i] / bark_size, w = cos(omega), and with the
LSP coefficients c (angles),
odd order:  p = (1 - w^2) prod_j 4 (cos c[2j+1] - w)^2,  q = 1/4 prod_j 4 (cos c[2j] - w)^2
even order: p = (1 - w)/2 prod_j 4 (cos c[2j+1] - w)^2,  q = (1 + w)/2 prod_j 4 (cos c[2j] - w)^2
and the value exp(0.11512925 (amplitude amp_offset / ((2^amp_bits - 1)
sqrt(p + q)) - amp_offset)).

```fides
pub fn vorbis_floor0_curve(map: &Vec<i64>, bark_size: i64, amp_bits: i64, amp_offset: i64, amplitude: i64, coefs: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `map`: `&Vec<i64>`
- `bark_size`: `i64`
- `amp_bits`: `i64`
- `amp_offset`: `i64`
- `amplitude`: `i64`
- `coefs`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1395*

---

## vorbis_imdct

The inverse MDCT of the spec: n/2 coefficients `x` to n samples
y[i] = sum_k x[k] cos(2 pi / n (i + 1/2 + n/4)(k + 1/2)).

```fides
pub fn vorbis_imdct(x: &Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `x`: `&Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1848*

---

## vorbis_decode_packet

Decode one audio packet; the PCM it completes, one Vec per channel
(empty for the first packet, which only fills the overlap). On an
invalid packet the result is empty and nothing changes.

```fides
pub fn vorbis_decode_packet(d: &mut VorbisDecoder, packet: Vec<i64>) -> Vec<Vec<f64>> {
```

**Parameters:**

- `d`: `&mut VorbisDecoder`
- `packet`: `Vec<i64>`

**Returns:** `Vec<Vec<f64>>`

*Defined at line 1943*

---

## VorbisFile

```fides
pub struct VorbisFile {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `channels` | `i64` |
| `rate` | `i64` |
| `vendor` | `str` |
| `comments` | `Vec<str>` |
| `packets` | `i64` |
| `pcm` | `Vec<Vec<f64>>,  // per channel` |

*Defined at line 2145*

---

## vorbis_decode_file

Decode an Ogg Vorbis file. PCM is trimmed to the last page's granule
position (the stream's length in samples) when that is shorter.

```fides
pub fn vorbis_decode_file(b: &Vec<i64>) -> VorbisFile {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `VorbisFile`

*Defined at line 2159*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
