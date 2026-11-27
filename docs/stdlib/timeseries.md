[All modules](index.md)

# timeseries -- Fides Documentation

## Contents

- [TsRing](#TsRing) (struct)
- [ts_ring_new](#ts_ring_new) (function)
- [ts_ring_len](#ts_ring_len) (function)
- [ts_ring_push](#ts_ring_push) (function)
- [ts_ring_time](#ts_ring_time) (function)
- [ts_ring_value](#ts_ring_value) (function)
- [ts_ring_clear](#ts_ring_clear) (function)
- [ts_ring_lower_bound](#ts_ring_lower_bound) (function)
- [TsSeries](#TsSeries) (struct)
- [ts_series_new](#ts_series_new) (function)
- [ts_series_push](#ts_series_push) (function)
- [ts_ring_range](#ts_ring_range) (function)
- [ts_ring_all](#ts_ring_all) (function)
- [TsBuckets](#TsBuckets) (struct)
- [ts_downsample](#ts_downsample) (function)
- [ts_bucket_avg](#ts_bucket_avg) (function)
- [TsBits](#TsBits) (struct)
- [ts_bits_new](#ts_bits_new) (function)
- [ts_bits_put](#ts_bits_put) (function)
- [TsBitReader](#TsBitReader) (struct)
- [ts_bits_get](#ts_bits_get) (function)
- [TsGorilla](#TsGorilla) (struct)
- [ts_gorilla_new](#ts_gorilla_new) (function)
- [ts_gorilla_append](#ts_gorilla_append) (function)
- [ts_gorilla_bytes](#ts_gorilla_bytes) (function)
- [ts_gorilla_encode](#ts_gorilla_encode) (function)
- [TsDecoded](#TsDecoded) (struct)
- [ts_gorilla_decode](#ts_gorilla_decode) (function)

## TsRing

```fides
pub struct TsRing {
```

**Fields:**

| Name | Type |
|------|------|
| `cap` | `i64` |
| `times` | `Vec<i64>` |
| `values` | `Vec<f64>` |
| `head` | `i64,  // index of the oldest point` |
| `count` | `i64` |
| `dropped` | `i64,  // points overwritten so far` |

*Defined at line 28*

---

## ts_ring_new

```fides
pub fn ts_ring_new(cap: i64) -> TsRing {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `TsRing`

*Defined at line 37*

---

## ts_ring_len

```fides
pub fn ts_ring_len(r: &TsRing) -> i64 {
```

**Parameters:**

- `r`: `&TsRing`

**Returns:** `i64`

*Defined at line 53*

---

## ts_ring_push

Add a point; false (and nothing stored) when `t` is before the latest.

```fides
pub fn ts_ring_push(r: &mut TsRing, t: i64, v: f64) -> bool {
```

**Parameters:**

- `r`: `&mut TsRing`
- `t`: `i64`
- `v`: `f64`

**Returns:** `bool`

*Defined at line 58*

---

## ts_ring_time

Time of point i (0 is the oldest).

```fides
pub fn ts_ring_time(r: &TsRing, i: i64) -> i64 {
```

**Parameters:**

- `r`: `&TsRing`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 77*

---

## ts_ring_value

```fides
pub fn ts_ring_value(r: &TsRing, i: i64) -> f64 {
```

**Parameters:**

- `r`: `&TsRing`
- `i`: `i64`

**Returns:** `f64`

*Defined at line 81*

---

## ts_ring_clear

```fides
pub fn ts_ring_clear(r: &mut TsRing) {
```

**Parameters:**

- `r`: `&mut TsRing`

*Defined at line 85*

---

## ts_ring_lower_bound

Index of the first point with time >= t (len when none).

```fides
pub fn ts_ring_lower_bound(r: &TsRing, t: i64) -> i64 {
```

**Parameters:**

- `r`: `&TsRing`
- `t`: `i64`

**Returns:** `i64`

*Defined at line 91*

---

## TsSeries

Times and values of the points with t0 <= time < t1.

```fides
pub struct TsSeries {
```

**Fields:**

| Name | Type |
|------|------|
| `times` | `Vec<i64>` |
| `values` | `Vec<f64>` |

*Defined at line 106*

---

## ts_series_new

```fides
pub fn ts_series_new() -> TsSeries {
```

**Returns:** `TsSeries`

*Defined at line 111*

---

## ts_series_push

```fides
pub fn ts_series_push(s: &mut TsSeries, t: i64, v: f64) {
```

**Parameters:**

- `s`: `&mut TsSeries`
- `t`: `i64`
- `v`: `f64`

*Defined at line 117*

---

## ts_ring_range

```fides
pub fn ts_ring_range(r: &TsRing, t0: i64, t1: i64) -> TsSeries {
```

**Parameters:**

- `r`: `&TsRing`
- `t0`: `i64`
- `t1`: `i64`

**Returns:** `TsSeries`

*Defined at line 122*

---

## ts_ring_all

Every point, oldest first.

```fides
pub fn ts_ring_all(r: &TsRing) -> TsSeries {
```

**Parameters:**

- `r`: `&TsRing`

**Returns:** `TsSeries`

*Defined at line 133*

---

## TsBuckets

Bucket b covers [start + b*width, start + (b+1)*width).

```fides
pub struct TsBuckets {
```

**Fields:**

| Name | Type |
|------|------|
| `start` | `i64` |
| `width` | `i64` |
| `counts` | `Vec<i64>` |
| `sums` | `Vec<f64>` |
| `mins` | `Vec<f64>` |
| `maxs` | `Vec<f64>` |
| `firsts` | `Vec<f64>` |
| `lasts` | `Vec<f64>` |

*Defined at line 146*

---

## ts_downsample

Downsample points (times ascending) into `n` buckets of `width` from
`start`; points outside are ignored.

```fides
pub fn ts_downsample(s: &TsSeries, start: i64, width: i64, n: i64) -> TsBuckets {
```

**Parameters:**

- `s`: `&TsSeries`
- `start`: `i64`
- `width`: `i64`
- `n`: `i64`

**Returns:** `TsBuckets`

*Defined at line 159*

---

## ts_bucket_avg

Average of bucket k (0.0 for an empty bucket: check the count).

```fides
pub fn ts_bucket_avg(b: &TsBuckets, k: i64) -> f64 {
```

**Parameters:**

- `b`: `&TsBuckets`
- `k`: `i64`

**Returns:** `f64`

*Defined at line 205*

---

## TsBits

```fides
pub struct TsBits {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `nbits` | `i64` |

*Defined at line 214*

---

## ts_bits_new

```fides
pub fn ts_bits_new() -> TsBits {
```

**Returns:** `TsBits`

*Defined at line 219*

---

## ts_bits_put

```fides
pub fn ts_bits_put(w: &mut TsBits, v: i64, n: i64) {
```

**Parameters:**

- `w`: `&mut TsBits`
- `v`: `i64`
- `n`: `i64`

*Defined at line 227*

---

## TsBitReader

```fides
pub struct TsBitReader {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `pos` | `i64` |
| `nbits` | `i64` |
| `ok` | `bool` |

*Defined at line 243*

---

## ts_bits_get

```fides
pub fn ts_bits_get(r: &mut TsBitReader, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut TsBitReader`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 252*

---

## TsGorilla

A compressed block that points can be appended to.

```fides
pub struct TsGorilla {
```

**Fields:**

| Name | Type |
|------|------|
| `bits` | `TsBits` |
| `count` | `i64` |
| `prev_t` | `i64` |
| `prev_delta` | `i64` |
| `prev_v` | `i64,  // bits of the previous value` |
| `lead` | `i64,  // previous XOR window` |
| `trail` | `i64` |

*Defined at line 311*

---

## ts_gorilla_new

```fides
pub fn ts_gorilla_new() -> TsGorilla {
```

**Returns:** `TsGorilla`

*Defined at line 321*

---

## ts_gorilla_append

```fides
pub fn ts_gorilla_append(g: &mut TsGorilla, t: i64, v: f64) {
```

**Parameters:**

- `g`: `&mut TsGorilla`
- `t`: `i64`
- `v`: `f64`

*Defined at line 330*

---

## ts_gorilla_bytes

The block as bytes: count (4 bytes, big-endian), then the bits.

```fides
pub fn ts_gorilla_bytes(g: &TsGorilla) -> Vec<i64> {
```

**Parameters:**

- `g`: `&TsGorilla`

**Returns:** `Vec<i64>`

*Defined at line 387*

---

## ts_gorilla_encode

Compress a whole series.

```fides
pub fn ts_gorilla_encode(s: &TsSeries) -> Vec<i64> {
```

**Parameters:**

- `s`: `&TsSeries`

**Returns:** `Vec<i64>`

*Defined at line 398*

---

## TsDecoded

```fides
pub struct TsDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `series` | `TsSeries` |

*Defined at line 408*

---

## ts_gorilla_decode

```fides
pub fn ts_gorilla_decode(b: Vec<i64>) -> TsDecoded {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `TsDecoded`

*Defined at line 415*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
