[All modules](index.md)

# nmea -- Fides Documentation

## Contents

- [nmea_checksum](#nmea_checksum) (function)
- [nmea_sentence](#nmea_sentence) (function)
- [NmeaSentence](#NmeaSentence) (struct)
- [nmea_parse](#nmea_parse) (function)
- [nmea_field](#nmea_field) (function)
- [NmeaReader](#NmeaReader) (struct)
- [nmea_reader_new](#nmea_reader_new) (function)
- [nmea_reader_feed](#nmea_reader_feed) (function)
- [nmea_is_number](#nmea_is_number) (function)
- [nmea_coord](#nmea_coord) (function)
- [nmea_fixed](#nmea_fixed) (function)
- [nmea_ddmm](#nmea_ddmm) (function)
- [nmea_time](#nmea_time) (function)
- [NmeaGga](#NmeaGga) (struct)
- [nmea_gga](#nmea_gga) (function)
- [NmeaRmc](#NmeaRmc) (struct)
- [nmea_rmc](#nmea_rmc) (function)
- [nmea_rmc_unix](#nmea_rmc_unix) (function)
- [NmeaGsa](#NmeaGsa) (struct)
- [nmea_gsa](#nmea_gsa) (function)
- [NmeaGsv](#NmeaGsv) (struct)
- [nmea_gsv](#nmea_gsv) (function)
- [NmeaSky](#NmeaSky) (struct)
- [nmea_sky_new](#nmea_sky_new) (function)
- [nmea_sky_add](#nmea_sky_add) (function)
- [NmeaVtg](#NmeaVtg) (struct)
- [nmea_vtg](#nmea_vtg) (function)
- [nmea_build_gga](#nmea_build_gga) (function)

## nmea_checksum

XOR of the bytes of `body` (the text between '$' and '*').

```fides
pub fn nmea_checksum(body: str) -> i64 {
```

**Parameters:**

- `body`: `str`

**Returns:** `i64`

*Defined at line 26*

---

## nmea_sentence

"$" + body + "*" + checksum, e.g. nmea_sentence("GPVTG,...").

```fides
pub fn nmea_sentence(body: str) -> str {
```

**Parameters:**

- `body`: `str`

**Returns:** `str`

*Defined at line 42*

---

## NmeaSentence

A sentence split into fields. `fields` are the comma-separated fields
after the address field; `talker` is "GP" and `kind` "GGA" for $GPGGA
(a proprietary $P... sentence has talker "P" and the rest as kind).

```fides
pub struct NmeaSentence {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `talker` | `str` |
| `kind` | `str` |
| `fields` | `Vec<str>` |
| `has_checksum` | `bool` |

*Defined at line 49*

---

## nmea_parse

Parse one line (trailing CR/LF allowed). A checksum, when present, must
match; `require_checksum` refuses lines without one.

```fides
pub fn nmea_parse(line: str, require_checksum: bool) -> NmeaSentence {
```

**Parameters:**

- `line`: `str`
- `require_checksum`: `bool`

**Returns:** `NmeaSentence`

*Defined at line 73*

---

## nmea_field

Field i, or "" when the sentence has fewer fields.

```fides
pub fn nmea_field(s: &NmeaSentence, i: i64) -> str {
```

**Parameters:**

- `s`: `&NmeaSentence`
- `i`: `i64`

**Returns:** `str`

*Defined at line 140*

---

## NmeaReader

Splits a byte stream into lines.

```fides
pub struct NmeaReader {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `str` |

*Defined at line 148*

---

## nmea_reader_new

```fides
pub fn nmea_reader_new() -> NmeaReader {
```

**Returns:** `NmeaReader`

*Defined at line 152*

---

## nmea_reader_feed

Add received text; the complete lines in it (without CR/LF, empty
lines dropped). A partial line waits for the rest.

```fides
pub fn nmea_reader_feed(r: &mut NmeaReader, chunk: str) -> Vec<str> {
```

**Parameters:**

- `r`: `&mut NmeaReader`
- `chunk`: `str`

**Returns:** `Vec<str>`

*Defined at line 160*

---

## nmea_is_number

Whether a field holds a decimal number (sign, digits, one point).

```fides
pub fn nmea_is_number(f: str) -> bool {
```

**Parameters:**

- `f`: `str`

**Returns:** `bool`

*Defined at line 185*

---

## nmea_coord

ddmm.mmmm (or dddmm.mmmm) and a hemisphere letter to signed degrees.
`ok` false when the field or the letter is not valid.

```fides
pub fn nmea_coord(field: str, hemi: str, ok: &mut bool) -> f64 {
```

**Parameters:**

- `field`: `str`
- `hemi`: `str`
- `ok`: `&mut bool`

**Returns:** `f64`

*Defined at line 217*

---

## nmea_fixed

x rounded to `decimals` places, as text ("48.1173").

```fides
pub fn nmea_fixed(x: f64, decimals: i64) -> str {
```

**Parameters:**

- `x`: `f64`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 239*

---

## nmea_ddmm

Degrees back to the NMEA form: "4807.038" for 48.1173 (lat, 3 minute
decimals) and the hemisphere; longitude has three degree digits.

```fides
pub fn nmea_ddmm(deg: f64, is_lon: bool, decimals: i64) -> str {
```

**Parameters:**

- `deg`: `f64`
- `is_lon`: `bool`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 271*

---

## nmea_time

"hhmmss.ss" to seconds since midnight; -1 when not a time.

```fides
pub fn nmea_time(f: str) -> f64 {
```

**Parameters:**

- `f`: `str`

**Returns:** `f64`

*Defined at line 298*

---

## NmeaGga

GGA: time, position, fix quality (0 none, 1 GPS, 2 DGPS, 4 RTK fixed,
5 RTK float, 6 estimated), satellites, HDOP, altitude (m above mean sea
level), geoid separation.

```fides
pub struct NmeaGga {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `time` | `f64` |
| `has_position` | `bool` |
| `lat` | `f64` |
| `lon` | `f64` |
| `quality` | `i64` |
| `sats` | `i64` |
| `has_hdop` | `bool` |
| `hdop` | `f64` |
| `has_alt` | `bool` |
| `alt` | `f64` |
| `has_geoid` | `bool` |
| `geoid` | `f64` |

*Defined at line 333*

---

## nmea_gga

```fides
pub fn nmea_gga(s: &NmeaSentence) -> NmeaGga {
```

**Parameters:**

- `s`: `&NmeaSentence`

**Returns:** `NmeaGga`

*Defined at line 349*

---

## NmeaRmc

RMC: time, status (A valid, V warning), position, speed over ground
(knots), course (degrees true), date, magnetic variation, mode.

```fides
pub struct NmeaRmc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `time` | `f64` |
| `valid` | `bool` |
| `has_position` | `bool` |
| `lat` | `f64` |
| `lon` | `f64` |
| `has_speed` | `bool` |
| `speed_knots` | `f64` |
| `has_course` | `bool` |
| `course` | `f64` |
| `year` | `i64` |
| `month` | `i64` |
| `day` | `i64` |
| `has_magvar` | `bool` |
| `magvar` | `f64,  // east positive` |
| `mode` | `str` |

*Defined at line 381*

---

## nmea_rmc

```fides
pub fn nmea_rmc(s: &NmeaSentence) -> NmeaRmc {
```

**Parameters:**

- `s`: `&NmeaSentence`

**Returns:** `NmeaRmc`

*Defined at line 400*

---

## nmea_rmc_unix

The RMC's date and time as seconds since 1970 (UTC); -1 without both.

```fides
pub fn nmea_rmc_unix(r: &NmeaRmc) -> f64 {
```

**Parameters:**

- `r`: `&NmeaRmc`

**Returns:** `f64`

*Defined at line 444*

---

## NmeaGsa

GSA: selection mode (A/M), fix type (1 none, 2 2D, 3 3D), the PRNs of
up to 12 satellites used, PDOP, HDOP, VDOP.

```fides
pub struct NmeaGsa {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `mode` | `str` |
| `fix` | `i64` |
| `prns` | `Vec<i64>` |
| `pdop` | `f64` |
| `hdop` | `f64` |
| `vdop` | `f64` |

*Defined at line 453*

---

## nmea_gsa

```fides
pub fn nmea_gsa(s: &NmeaSentence) -> NmeaGsa {
```

**Parameters:**

- `s`: `&NmeaSentence`

**Returns:** `NmeaGsa`

*Defined at line 463*

---

## NmeaGsv

GSV: sentence `num` of `total`, satellites in view, and up to four
satellites (PRN, elevation, azimuth, SNR; -1 for an empty field).

```fides
pub struct NmeaGsv {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `total` | `i64` |
| `num` | `i64` |
| `in_view` | `i64` |
| `prns` | `Vec<i64>` |
| `elevs` | `Vec<i64>` |
| `azims` | `Vec<i64>` |
| `snrs` | `Vec<i64>` |

*Defined at line 489*

---

## nmea_gsv

```fides
pub fn nmea_gsv(s: &NmeaSentence) -> NmeaGsv {
```

**Parameters:**

- `s`: `&NmeaSentence`

**Returns:** `NmeaGsv`

*Defined at line 507*

---

## NmeaSky

The satellites of a whole GSV group (sentences 1..total in order).

```fides
pub struct NmeaSky {
```

**Fields:**

| Name | Type |
|------|------|
| `talker` | `str` |
| `expect` | `i64,  // next sentence number wanted` |
| `total` | `i64` |
| `in_view` | `i64` |
| `complete` | `bool` |
| `prns` | `Vec<i64>` |
| `elevs` | `Vec<i64>` |
| `azims` | `Vec<i64>` |
| `snrs` | `Vec<i64>` |

*Defined at line 533*

---

## nmea_sky_new

```fides
pub fn nmea_sky_new() -> NmeaSky {
```

**Returns:** `NmeaSky`

*Defined at line 545*

---

## nmea_sky_add

Add a GSV sentence. A sentence out of order starts over (from its
group's first sentence). True when the group became complete.

```fides
pub fn nmea_sky_add(k: &mut NmeaSky, s: &NmeaSentence) -> bool {
```

**Parameters:**

- `k`: `&mut NmeaSky`
- `s`: `&NmeaSentence`

**Returns:** `bool`

*Defined at line 553*

---

## NmeaVtg

VTG: course (true and magnetic), speed in knots and km/h, mode.

```fides
pub struct NmeaVtg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `has_course` | `bool` |
| `course_true` | `f64` |
| `has_course_mag` | `bool` |
| `course_mag` | `f64` |
| `knots` | `f64` |
| `kmh` | `f64` |
| `mode` | `str` |

*Defined at line 586*

---

## nmea_vtg

```fides
pub fn nmea_vtg(s: &NmeaSentence) -> NmeaVtg {
```

**Parameters:**

- `s`: `&NmeaSentence`

**Returns:** `NmeaVtg`

*Defined at line 597*

---

## nmea_build_gga

Build a GGA sentence (talker "GP") from a fix: time "hhmmss", position
with `decimals` minute digits, quality, satellites, HDOP, altitude and
geoid separation with one decimal.

```fides
pub fn nmea_build_gga(time: str, lat: f64, lon: f64, decimals: i64, quality: i64, sats: i64, hdop: f64, alt: f64, geoid: f64) -> str {
```

**Parameters:**

- `time`: `str`
- `lat`: `f64`
- `lon`: `f64`
- `decimals`: `i64`
- `quality`: `i64`
- `sats`: `i64`
- `hdop`: `f64`
- `alt`: `f64`
- `geoid`: `f64`

**Returns:** `str`

*Defined at line 622*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
