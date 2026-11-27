[All modules](index.md)

# middleware -- Fides Documentation

## Contents

- [MW_CORS:](#MW_CORS:) (const)
- [MW_COMPRESS:](#MW_COMPRESS:) (const)
- [MW_LOG:](#MW_LOG:) (const)
- [MW_REQUEST_ID:](#MW_REQUEST_ID:) (const)
- [MW_RATE_LIMIT:](#MW_RATE_LIMIT:) (const)
- [MW_BASIC_AUTH:](#MW_BASIC_AUTH:) (const)
- [MW_BEARER_AUTH:](#MW_BEARER_AUTH:) (const)
- [MW_TIMEOUT:](#MW_TIMEOUT:) (const)
- [MW_BEFORE:](#MW_BEFORE:) (const)
- [MW_AFTER:](#MW_AFTER:) (const)
- [MwResponse](#MwResponse) (struct)
- [mw_response_new](#mw_response_new) (function)
- [mw_response_parse](#mw_response_parse) (function)
- [mw_response_build](#mw_response_build) (function)
- [mw_response_header](#mw_response_header) (function)
- [mw_response_set_header](#mw_response_set_header) (function)
- [mw_deflate](#mw_deflate) (function)
- [mw_gzip](#mw_gzip) (function)
- [mw_zlib](#mw_zlib) (function)
- [mw_negotiate_encoding](#mw_negotiate_encoding) (function)
- [MwCors](#MwCors) (struct)
- [mw_cors_default](#mw_cors_default) (function)
- [MwRateLimiter](#MwRateLimiter) (struct)
- [mw_rate_limiter_new](#mw_rate_limiter_new) (function)
- [mw_rate_take](#mw_rate_take) (function)
- [mw_rate_remaining](#mw_rate_remaining) (function)
- [mw_rate_retry_ms](#mw_rate_retry_ms) (function)
- [mw_basic_credentials](#mw_basic_credentials) (function)
- [mw_bearer_token](#mw_bearer_token) (function)
- [MwChain](#MwChain) (struct)
- [mw_chain_new](#mw_chain_new) (function)
- [mw_chain_set_clock](#mw_chain_set_clock) (function)
- [mw_use_cors](#mw_use_cors) (function)
- [mw_use_compress](#mw_use_compress) (function)
- [mw_use_log](#mw_use_log) (function)
- [mw_use_request_id](#mw_use_request_id) (function)
- [mw_use_rate_limit](#mw_use_rate_limit) (function)
- [mw_use_basic_auth](#mw_use_basic_auth) (function)
- [mw_use_bearer_auth](#mw_use_bearer_auth) (function)
- [mw_use_timeout](#mw_use_timeout) (function)
- [mw_use_before](#mw_use_before) (function)
- [mw_use_after](#mw_use_after) (function)
- [mw_chain_handle](#mw_chain_handle) (function)
- [mw_chain_take_log](#mw_chain_take_log) (function)
- [mw_chain_serve_fn](#mw_chain_serve_fn) (function)

## MW_CORS:

```fides
pub const MW_CORS: i64 = 1;
```

*Defined at line 60*

---

## MW_COMPRESS:

```fides
pub const MW_COMPRESS: i64 = 2;
```

*Defined at line 61*

---

## MW_LOG:

```fides
pub const MW_LOG: i64 = 3;
```

*Defined at line 62*

---

## MW_REQUEST_ID:

```fides
pub const MW_REQUEST_ID: i64 = 4;
```

*Defined at line 63*

---

## MW_RATE_LIMIT:

```fides
pub const MW_RATE_LIMIT: i64 = 5;
```

*Defined at line 64*

---

## MW_BASIC_AUTH:

```fides
pub const MW_BASIC_AUTH: i64 = 6;
```

*Defined at line 65*

---

## MW_BEARER_AUTH:

```fides
pub const MW_BEARER_AUTH: i64 = 7;
```

*Defined at line 66*

---

## MW_TIMEOUT:

```fides
pub const MW_TIMEOUT: i64 = 8;
```

*Defined at line 67*

---

## MW_BEFORE:

```fides
pub const MW_BEFORE: i64 = 9;
```

*Defined at line 68*

---

## MW_AFTER:

```fides
pub const MW_AFTER: i64 = 10;
```

*Defined at line 69*

---

## MwResponse

```fides
pub struct MwResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `reason` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `body` | `str` |

*Defined at line 73*

---

## mw_response_new

```fides
pub fn mw_response_new(status: i64, content_type: str, body: str) -> MwResponse {
```

**Parameters:**

- `status`: `i64`
- `content_type`: `str`
- `body`: `str`

**Returns:** `MwResponse`

*Defined at line 81*

---

## mw_response_parse

Parse a response as stdlib http builds it ("HTTP/1.1 200 OK\r\n...").

```fides
pub fn mw_response_parse(raw: str) -> MwResponse {
```

**Parameters:**

- `raw`: `str`

**Returns:** `MwResponse`

*Defined at line 93*

---

## mw_response_build

The response text, Content-Length from the body (none for 204/304).

```fides
pub fn mw_response_build(r: &MwResponse) -> str {
```

**Parameters:**

- `r`: `&MwResponse`

**Returns:** `str`

*Defined at line 119*

---

## mw_response_header

```fides
pub fn mw_response_header(r: &MwResponse, name: str) -> str {
```

**Parameters:**

- `r`: `&MwResponse`
- `name`: `str`

**Returns:** `str`

*Defined at line 166*

---

## mw_response_set_header

Set a header, replacing one of the same name.

```fides
pub fn mw_response_set_header(r: &mut MwResponse, name: str, value: str) {
```

**Parameters:**

- `r`: `&mut MwResponse`
- `name`: `str`
- `value`: `str`

*Defined at line 178*

---

## mw_deflate

Raw DEFLATE (one fixed-Huffman block) of `data`.

```fides
pub fn mw_deflate(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 349*

---

## mw_gzip

gzip member (RFC 1952) holding mw_deflate(data).

```fides
pub fn mw_gzip(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 422*

---

## mw_zlib

zlib stream (RFC 1950) holding mw_deflate(data): HTTP's "deflate".

```fides
pub fn mw_zlib(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 456*

---

## mw_negotiate_encoding

The coding to answer with: "gzip", "deflate" or "" (identity).
gzip wins a tie.

```fides
pub fn mw_negotiate_encoding(accept: str) -> str {
```

**Parameters:**

- `accept`: `str`

**Returns:** `str`

*Defined at line 533*

---

## MwCors

```fides
pub struct MwCors {
```

**Fields:**

| Name | Type |
|------|------|
| `origins` | `Vec<str>` |
| `methods` | `str` |
| `headers` | `str` |
| `expose` | `str` |
| `credentials` | `bool` |
| `max_age` | `i64` |

*Defined at line 555*

---

## mw_cors_default

Any origin, the common methods, Content-Type and Authorization.

```fides
pub fn mw_cors_default() -> MwCors {
```

**Returns:** `MwCors`

*Defined at line 565*

---

## MwRateLimiter

Token buckets, one per key: `capacity` tokens, `per_second` refill.

```fides
pub struct MwRateLimiter {
```

**Fields:**

| Name | Type |
|------|------|
| `capacity` | `f64` |
| `per_second` | `f64` |
| `key_header` | `str` |
| `keys` | `Vec<str>` |
| `tokens` | `Vec<f64>` |
| `last_ms` | `Vec<i64>` |

*Defined at line 610*

---

## mw_rate_limiter_new

```fides
pub fn mw_rate_limiter_new(capacity: f64, per_second: f64, key_header: str) -> MwRateLimiter {
```

**Parameters:**

- `capacity`: `f64`
- `per_second`: `f64`
- `key_header`: `str`

**Returns:** `MwRateLimiter`

*Defined at line 619*

---

## mw_rate_take

Take a token for `key` at `now_ms`: true when allowed.

```fides
pub fn mw_rate_take(rl: &mut MwRateLimiter, key: str, now_ms: i64) -> bool {
```

**Parameters:**

- `rl`: `&mut MwRateLimiter`
- `key`: `str`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 649*

---

## mw_rate_remaining

Whole tokens left for `key`.

```fides
pub fn mw_rate_remaining(rl: &mut MwRateLimiter, key: str, now_ms: i64) -> i64 {
```

**Parameters:**

- `rl`: `&mut MwRateLimiter`
- `key`: `str`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 660*

---

## mw_rate_retry_ms

Milliseconds until `key` has a token again (0 when it has one).

```fides
pub fn mw_rate_retry_ms(rl: &mut MwRateLimiter, key: str, now_ms: i64) -> i64 {
```

**Parameters:**

- `rl`: `&mut MwRateLimiter`
- `key`: `str`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 666*

---

## mw_basic_credentials

Basic credentials from an Authorization value: [user, password], or an
empty Vec when it is not well-formed Basic.

```fides
pub fn mw_basic_credentials(auth: str) -> Vec<str> {
```

**Parameters:**

- `auth`: `str`

**Returns:** `Vec<str>`

*Defined at line 684*

---

## mw_bearer_token

The token of "Bearer <token>", or "".

```fides
pub fn mw_bearer_token(auth: str) -> str {
```

**Parameters:**

- `auth`: `str`

**Returns:** `str`

*Defined at line 705*

---

## MwChain

```fides
pub struct MwChain {
```

**Fields:**

| Name | Type |
|------|------|
| `layers` | `Vec<MwLayer>` |
| `cors` | `Vec<MwCors>` |
| `limiters` | `Vec<MwRateLimiter>` |
| `basic` | `Vec<MwBasicFn>` |
| `bearer` | `Vec<MwBearerFn>` |
| `befores` | `Vec<MwBeforeFn>` |
| `afters` | `Vec<MwAfterFn>` |
| `handler` | `fn(HttpRequest) -> str` |
| `clock` | `fn() -> i64` |
| `rng` | `i64` |
| `log` | `Vec<str>` |
| `served` | `i64` |

*Defined at line 743*

---

## mw_chain_new

```fides
pub fn mw_chain_new(handler: fn(HttpRequest) -> str) -> MwChain {
```

**Parameters:**

- `handler`: `fn(HttpRequest) -> str`

**Returns:** `MwChain`

*Defined at line 762*

---

## mw_chain_set_clock

Use `clock` (milliseconds) instead of the system clock.

```fides
pub fn mw_chain_set_clock(c: &mut MwChain, clock: fn() -> i64) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `clock`: `fn() -> i64`

*Defined at line 769*

---

## mw_use_cors

```fides
pub fn mw_use_cors(c: &mut MwChain, cfg: MwCors) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `cfg`: `MwCors`

*Defined at line 779*

---

## mw_use_compress

Compress bodies of at least `min_bytes` when the client accepts it.

```fides
pub fn mw_use_compress(c: &mut MwChain, min_bytes: i64) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `min_bytes`: `i64`

*Defined at line 785*

---

## mw_use_log

One line per request into c.log (and printed when `print` is true).

```fides
pub fn mw_use_log(c: &mut MwChain, print: bool) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `print`: `bool`

*Defined at line 790*

---

## mw_use_request_id

X-Request-Id: kept or made (from a generator seeded with `seed`).

```fides
pub fn mw_use_request_id(c: &mut MwChain, seed: i64) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `seed`: `i64`

*Defined at line 795*

---

## mw_use_rate_limit

A token bucket per value of header `key_header` ("" = one bucket).

```fides
pub fn mw_use_rate_limit(c: &mut MwChain, capacity: f64, per_second: f64, key_header: str) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `capacity`: `f64`
- `per_second`: `f64`
- `key_header`: `str`

*Defined at line 803*

---

## mw_use_basic_auth

```fides
pub fn mw_use_basic_auth(c: &mut MwChain, realm: str, check: fn(str, str) -> bool) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `realm`: `str`
- `check`: `fn(str`

*Defined at line 808*

---

## mw_use_bearer_auth

```fides
pub fn mw_use_bearer_auth(c: &mut MwChain, realm: str, check: fn(str) -> bool) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `realm`: `str`
- `check`: `fn(str) -> bool`

*Defined at line 815*

---

## mw_use_timeout

Replace a response that took longer than `ms` with 503.

```fides
pub fn mw_use_timeout(c: &mut MwChain, ms: i64) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `ms`: `i64`

*Defined at line 823*

---

## mw_use_before

A hook run on the way in: it may change the request and returns true
to pass it on, or fills the response (a 500 with no body until it does)
and returns false to answer at once.

```fides
pub fn mw_use_before(c: &mut MwChain, hook: fn(&mut HttpRequest, &mut MwResponse) -> bool) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `hook`: `fn(&mut HttpRequest`

*Defined at line 830*

---

## mw_use_after

A hook run on the way out: it may change the response (its result is
not used).

```fides
pub fn mw_use_after(c: &mut MwChain, hook: fn(HttpRequest, &mut MwResponse) -> bool) {
```

**Parameters:**

- `c`: `&mut MwChain`
- `hook`: `fn(HttpRequest`

*Defined at line 839*

---

## mw_chain_handle

Run one request through the layers and the handler.

```fides
pub fn mw_chain_handle(c: &mut MwChain, request: HttpRequest) -> str {
```

**Parameters:**

- `c`: `&mut MwChain`
- `request`: `HttpRequest`

**Returns:** `str`

*Defined at line 1040*

---

## mw_chain_take_log

The log lines so far, and forget them.

```fides
pub fn mw_chain_take_log(c: &mut MwChain) -> Vec<str> {
```

**Parameters:**

- `c`: `&mut MwChain`

**Returns:** `Vec<str>`

*Defined at line 1073*

---

## mw_chain_serve_fn

A handler for http_serve running the chain (its state kept between
requests).

```fides
pub fn mw_chain_serve_fn(c: MwChain) -> fn(HttpRequest) -> str {
```

**Parameters:**

- `c`: `MwChain`

**Returns:** `fn(HttpRequest) -> str`

*Defined at line 1092*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
