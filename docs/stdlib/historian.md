[All modules](index.md)

# historian -- Fides Documentation

## Contents

- [HistTag](#HistTag) (struct)
- [Historian](#Historian) (struct)
- [historian_new](#historian_new) (function)
- [historian_add_tag](#historian_add_tag) (function)
- [historian_tag_id](#historian_tag_id) (function)
- [HistTag_compress](#HistTag_compress) (function)
- [HistTag_add](#HistTag_add) (function)
- [HistTag_flush](#HistTag_flush) (function)
- [historian_add](#historian_add) (function)
- [historian_flush](#historian_flush) (function)
- [HistTag_compression_ratio](#HistTag_compression_ratio) (function)
- [HistTag_value_at](#HistTag_value_at) (function)
- [HistTag_interpolated](#HistTag_interpolated) (function)
- [HistTag_recorded](#HistTag_recorded) (function)
- [HistAggregate](#HistAggregate) (struct)
- [HistTag_aggregate](#HistTag_aggregate) (function)

## HistTag

```fides
pub struct HistTag {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `units` | `str` |
| `description` | `str` |
| `exc_dev` | `f64` |
| `exc_max` | `f64` |
| `comp_dev` | `f64` |
| `comp_max` | `f64` |
| `times` | `Vec<f64>` |
| `values` | `Vec<f64>` |
| `exc_has` | `bool` |
| `exc_t` | `f64` |
| `exc_v` | `f64` |
| `held_has` | `bool` |
| `held_t` | `f64` |
| `held_v` | `f64` |
| `snap_has` | `bool` |
| `snap_t` | `f64` |
| `snap_v` | `f64` |
| `slope_hi` | `f64` |
| `slope_lo` | `f64` |
| `received` | `i64` |
| `passed` | `i64` |

*Defined at line 36*

---

## Historian

```fides
pub struct Historian {
```

**Fields:**

| Name | Type |
|------|------|
| `tags` | `Vec<HistTag>` |
| `index` | `HashMap<str` |

*Defined at line 65*

---

## historian_new

```fides
pub fn historian_new() -> Historian {
```

**Returns:** `Historian`

*Defined at line 71*

---

## historian_add_tag

Register a tag; returns its id (the existing id if the name is known).
A deviation of 0 passes every change; a max time of 0 means none.

```fides
pub fn historian_add_tag(h: &mut Historian, name: str, units: str, exc_dev: f64, exc_max: f64, comp_dev: f64, comp_max: f64) -> i64 {
```

**Parameters:**

- `h`: `&mut Historian`
- `name`: `str`
- `units`: `str`
- `exc_dev`: `f64`
- `exc_max`: `f64`
- `comp_dev`: `f64`
- `comp_max`: `f64`

**Returns:** `i64`

*Defined at line 79*

---

## historian_tag_id

Tag id by name, -1 when unknown.

```fides
pub fn historian_tag_id(h: &Historian, name: str) -> i64 {
```

**Parameters:**

- `h`: `&Historian`
- `name`: `str`

**Returns:** `i64`

*Defined at line 92*

---

## HistTag_compress

Feed one exception-passed point to the compressor.

```fides
pub fn HistTag_compress(tag: &mut HistTag, t: f64, v: f64) {
```

**Parameters:**

- `tag`: `&mut HistTag`
- `t`: `f64`
- `v`: `f64`

*Defined at line 107*

---

## HistTag_add

Feed one raw sample: exception test, then compression.

```fides
pub fn HistTag_add(tag: &mut HistTag, t: f64, v: f64) {
```

**Parameters:**

- `tag`: `&mut HistTag`
- `t`: `f64`
- `v`: `f64`

*Defined at line 152*

---

## HistTag_flush

Archive what is pending (the held sample and the snapshot), e.g. at
shutdown or before reading the newest data.

```fides
pub fn HistTag_flush(tag: &mut HistTag) {
```

**Parameters:**

- `tag`: `&mut HistTag`

*Defined at line 183*

---

## historian_add

```fides
pub fn historian_add(h: &mut Historian, id: i64, t: f64, v: f64) {
```

**Parameters:**

- `h`: `&mut Historian`
- `id`: `i64`
- `t`: `f64`
- `v`: `f64`

*Defined at line 200*

---

## historian_flush

```fides
pub fn historian_flush(h: &mut Historian, id: i64) {
```

**Parameters:**

- `h`: `&mut Historian`
- `id`: `i64`

*Defined at line 206*

---

## HistTag_compression_ratio

Raw samples per archived value.

```fides
pub fn HistTag_compression_ratio(tag: &HistTag) -> f64 {
```

**Parameters:**

- `tag`: `&HistTag`

**Returns:** `f64`

*Defined at line 213*

---

## HistTag_value_at

The trend's value at t: linear between archived points, the first or
last value outside them.

```fides
pub fn HistTag_value_at(tag: &HistTag, t: f64) -> f64 {
```

**Parameters:**

- `tag`: `&HistTag`
- `t`: `f64`

**Returns:** `f64`

*Defined at line 242*

---

## HistTag_interpolated

`count` evenly spaced interpolated values from t0 to t1 inclusive.

```fides
pub fn HistTag_interpolated(tag: &HistTag, t0: f64, t1: f64, count: i64) -> Vec<f64> {
```

**Parameters:**

- `tag`: `&HistTag`
- `t0`: `f64`
- `t1`: `f64`
- `count`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 261*

---

## HistTag_recorded

Archived points with t0 <= t <= t1 as [t, v, t, v, ...].

```fides
pub fn HistTag_recorded(tag: &HistTag, t0: f64, t1: f64) -> Vec<f64> {
```

**Parameters:**

- `tag`: `&HistTag`
- `t0`: `f64`
- `t1`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 276*

---

## HistAggregate

```fides
pub struct HistAggregate {
```

**Fields:**

| Name | Type |
|------|------|
| `time_avg` | `f64` |
| `min` | `f64` |
| `max` | `f64` |
| `sample_avg` | `f64` |
| `count` | `i64` |

*Defined at line 290*

---

## HistTag_aggregate

Aggregates of the interpolated trend over [t0, t1] (t1 > t0). Min and
max cover the window's end values and every archived point inside;
count and sample average the archived points inside.

```fides
pub fn HistTag_aggregate(tag: &HistTag, t0: f64, t1: f64) -> HistAggregate {
```

**Parameters:**

- `tag`: `&HistTag`
- `t0`: `f64`
- `t1`: `f64`

**Returns:** `HistAggregate`

*Defined at line 301*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
