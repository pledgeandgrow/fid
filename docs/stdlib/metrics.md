[All modules](index.md)

# metrics -- Fides Documentation

## Contents

- [METRIC_COUNTER:](#METRIC_COUNTER:) (const)
- [METRIC_GAUGE:](#METRIC_GAUGE:) (const)
- [METRIC_HISTOGRAM:](#METRIC_HISTOGRAM:) (const)
- [Metrics](#Metrics) (struct)
- [metrics_new](#metrics_new) (function)
- [metrics_valid_name](#metrics_valid_name) (function)
- [metrics_counter](#metrics_counter) (function)
- [metrics_gauge](#metrics_gauge) (function)
- [metrics_histogram](#metrics_histogram) (function)
- [metrics_inc](#metrics_inc) (function)
- [metrics_set](#metrics_set) (function)
- [metrics_observe](#metrics_observe) (function)
- [metrics_get](#metrics_get) (function)
- [metrics_count](#metrics_count) (function)
- [metrics_text](#metrics_text) (function)
- [metrics_http_handler](#metrics_http_handler) (function)
- [metrics_serve_set](#metrics_serve_set) (function)
- [metrics_serve](#metrics_serve) (function)

## METRIC_COUNTER:

```fides
pub const METRIC_COUNTER: i64 = 0;
```

*Defined at line 17*

---

## METRIC_GAUGE:

```fides
pub const METRIC_GAUGE: i64 = 1;
```

*Defined at line 18*

---

## METRIC_HISTOGRAM:

```fides
pub const METRIC_HISTOGRAM: i64 = 2;
```

*Defined at line 19*

---

## Metrics

A set of metrics. Ids are indexes; a histogram's buckets live in `bounds`
and `buckets`, from `first[id]`, `nbounds[id]` of them.

```fides
pub struct Metrics {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `helps` | `Vec<str>` |
| `kinds` | `Vec<i64>` |
| `values` | `Vec<i64>` |
| `counts` | `Vec<i64>` |
| `first` | `Vec<i64>` |
| `nbounds` | `Vec<i64>` |
| `bounds` | `Vec<i64>` |
| `buckets` | `Vec<i64>` |

*Defined at line 22*

---

## metrics_new

An empty set of metrics.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = metrics_new();
var reqs = metrics_counter(&mut m, "http_requests_total", "Requests served.");
metrics_inc(&mut m, reqs, 1);
metrics_inc(&mut m, reqs, 2);
assert_eq(metrics_get(m, reqs), 3);
var text = metrics_text(m);
println(text);
assert(str_contains(text, "http_requests_total 3"));
```

```fides
pub fn metrics_new() -> Metrics {
```

**Returns:** `Metrics`

*Defined at line 47*

---

## metrics_valid_name

Is `name` a valid metric name: [a-zA-Z_:][a-zA-Z0-9_:]*?


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(metrics_valid_name("http_requests_total"));
assert(!metrics_valid_name("9lives"));
```

```fides
pub fn metrics_valid_name(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 58*

---

## metrics_counter

A counter: only goes up.

```fides
pub fn metrics_counter(m: &mut Metrics, name: str, help: str) -> i64 {
```

**Parameters:**

- `m`: `&mut Metrics`
- `name`: `str`
- `help`: `str`

**Returns:** `i64`

*Defined at line 97*

---

## metrics_gauge

A gauge: set to any value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = metrics_new();
var g = metrics_gauge(&mut m, "queue_depth", "Jobs waiting.");
metrics_set(&mut m, g, 12);
metrics_set(&mut m, g, 4);
assert_eq(metrics_get(m, g), 4);
```

```fides
pub fn metrics_gauge(m: &mut Metrics, name: str, help: str) -> i64 {
```

**Parameters:**

- `m`: `&mut Metrics`
- `name`: `str`
- `help`: `str`

**Returns:** `i64`

*Defined at line 109*

---

## metrics_histogram

A histogram with the given upper bounds, in increasing order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = metrics_new();
var up: Vec<i64> = Vec::new();
up.push(10); up.push(100);
var h = metrics_histogram(&mut m, "latency_ms", "Request latency.", up);
metrics_observe(&mut m, h, 5);
metrics_observe(&mut m, h, 50);
assert_eq(metrics_count(m, h), 2);
assert_eq(metrics_get(m, h), 55);
```

```fides
pub fn metrics_histogram(m: &mut Metrics, name: str, help: str, upper: Vec<i64>) -> i64 {
```

**Parameters:**

- `m`: `&mut Metrics`
- `name`: `str`
- `help`: `str`
- `upper`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 124*

---

## metrics_inc

Add `by` (>= 0) to a counter, or to a gauge.

```fides
pub fn metrics_inc(m: &mut Metrics, id: i64, by: i64) {
```

**Parameters:**

- `m`: `&mut Metrics`
- `id`: `i64`
- `by`: `i64`

*Defined at line 140*

---

## metrics_set

Set a gauge.

```fides
pub fn metrics_set(m: &mut Metrics, id: i64, value: i64) {
```

**Parameters:**

- `m`: `&mut Metrics`
- `id`: `i64`
- `value`: `i64`

*Defined at line 148*

---

## metrics_observe

Record one value in a histogram.

```fides
pub fn metrics_observe(m: &mut Metrics, id: i64, value: i64) {
```

**Parameters:**

- `m`: `&mut Metrics`
- `id`: `i64`
- `value`: `i64`

*Defined at line 156*

---

## metrics_get

A counter's or gauge's value; a histogram's sum.

```fides
pub fn metrics_get(m: Metrics, id: i64) -> i64 {
```

**Parameters:**

- `m`: `Metrics`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 174*

---

## metrics_count

A histogram's number of observations.

```fides
pub fn metrics_count(m: Metrics, id: i64) -> i64 {
```

**Parameters:**

- `m`: `Metrics`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 179*

---

## metrics_text

Every metric in the Prometheus text format, version 0.0.4.

```fides
pub fn metrics_text(m: Metrics) -> str {
```

**Parameters:**

- `m`: `Metrics`

**Returns:** `str`

*Defined at line 184*

---

## metrics_http_handler

The endpoint body: Prometheus text on /metrics, 404 elsewhere.

```fides
pub fn metrics_http_handler(req: HttpRequest) -> str {
```

**Parameters:**

- `req`: `HttpRequest`

**Returns:** `str`

*Defined at line 221*

---

## metrics_serve_set

Point the served endpoint at `m` without starting a server.

```fides
pub fn metrics_serve_set(m: Metrics) {
```

**Parameters:**

- `m`: `Metrics`

*Defined at line 229*

---

## metrics_serve

Serve `m` at GET /metrics on `port` — one call (CO3). Returns false when
the port cannot be opened.

```fides
pub fn metrics_serve(m: Metrics, port: i64) -> bool {
```

**Parameters:**

- `m`: `Metrics`
- `port`: `i64`

**Returns:** `bool`

*Defined at line 235*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
