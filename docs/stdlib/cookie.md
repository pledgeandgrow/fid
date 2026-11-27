[All modules](index.md)

# cookie -- Fides Documentation

## Contents

- [Cookie](#Cookie) (struct)
- [SetCookie](#SetCookie) (struct)
- [set_cookie_new](#set_cookie_new) (function)
- [cookie_name_ok](#cookie_name_ok) (function)
- [cookie_value_ok](#cookie_value_ok) (function)
- [cookie_parse_header](#cookie_parse_header) (function)
- [cookie_get](#cookie_get) (function)
- [cookie_header](#cookie_header) (function)
- [set_cookie_serialize](#set_cookie_serialize) (function)
- [set_cookie_parse](#set_cookie_parse) (function)
- [cookie_default_path](#cookie_default_path) (function)
- [cookie_path_match](#cookie_path_match) (function)
- [cookie_domain_match](#cookie_domain_match) (function)
- [cookie_http_date](#cookie_http_date) (function)
- [cookie_hmac_sha512](#cookie_hmac_sha512) (function)
- [cookie_ct_eq](#cookie_ct_eq) (function)
- [cookie_sign](#cookie_sign) (function)
- [cookie_unsign](#cookie_unsign) (function)
- [Session](#Session) (struct)
- [SessionStore](#SessionStore) (struct)
- [session_store_new](#session_store_new) (function)
- [session_id_from_words](#session_id_from_words) (function)
- [session_create](#session_create) (function)
- [session_touch](#session_touch) (function)
- [session_set](#session_set) (function)
- [session_get](#session_get) (function)
- [session_destroy](#session_destroy) (function)
- [session_sweep](#session_sweep) (function)

## Cookie

A name/value pair of a Cookie request header.

```fides
pub struct Cookie {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `str` |

*Defined at line 40*

---

## SetCookie

A Set-Cookie header. `max_age` is used only when `has_max_age`;
`expires` is an HTTP date or ""; `same_site` is "Strict", "Lax",
"None" or "".

```fides
pub struct SetCookie {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `value` | `str` |
| `expires` | `str` |
| `has_max_age` | `bool` |
| `max_age` | `i64` |
| `domain` | `str` |
| `path` | `str` |
| `secure` | `bool` |
| `http_only` | `bool` |
| `same_site` | `str` |

*Defined at line 48*

---

## set_cookie_new

```fides
pub fn set_cookie_new(name: str, value: str) -> SetCookie {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `SetCookie`

*Defined at line 61*

---

## cookie_name_ok

Whether `s` is a non-empty token (a valid cookie name).

```fides
pub fn cookie_name_ok(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 95*

---

## cookie_value_ok

Whether `s` is a valid cookie value: cookie-octets, optionally inside
one pair of double quotes.

```fides
pub fn cookie_value_ok(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 112*

---

## cookie_parse_header

The pairs of a Cookie header, in order.

```fides
pub fn cookie_parse_header(h: str) -> Vec<Cookie> {
```

**Parameters:**

- `h`: `str`

**Returns:** `Vec<Cookie>`

*Defined at line 144*

---

## cookie_get

The value of the first cookie called `name`.

```fides
pub fn cookie_get(cs: Vec<Cookie>, name: str) -> Option<str> {
```

**Parameters:**

- `cs`: `Vec<Cookie>`
- `name`: `str`

**Returns:** `Option<str>`

*Defined at line 166*

---

## cookie_header

A Cookie request header from pairs ("" when a pair is invalid).

```fides
pub fn cookie_header(cs: Vec<Cookie>) -> str {
```

**Parameters:**

- `cs`: `Vec<Cookie>`

**Returns:** `str`

*Defined at line 178*

---

## set_cookie_serialize

The Set-Cookie header value, or "" when the cookie breaks a rule (see
the header comment).

```fides
pub fn set_cookie_serialize(c: SetCookie) -> str {
```

**Parameters:**

- `c`: `SetCookie`

**Returns:** `str`

*Defined at line 199*

---

## set_cookie_parse

A Set-Cookie header read as a user agent does (RFC 6265 §5.2). The
name is "" when the header holds no "=" before its first ";" or the
name is empty (the header is then to be ignored).

```fides
pub fn set_cookie_parse(h: str) -> SetCookie {
```

**Parameters:**

- `h`: `str`

**Returns:** `SetCookie`

*Defined at line 272*

---

## cookie_default_path

The default path of a cookie set by a response to `request_path`
(RFC 6265 §5.1.4).

```fides
pub fn cookie_default_path(request_path: str) -> str {
```

**Parameters:**

- `request_path`: `str`

**Returns:** `str`

*Defined at line 346*

---

## cookie_path_match

Whether a cookie with `cookie_path` is sent with `request_path`
(RFC 6265 §5.1.4 path-match).

```fides
pub fn cookie_path_match(cookie_path: str, request_path: str) -> bool {
```

**Parameters:**

- `cookie_path`: `str`
- `request_path`: `str`

**Returns:** `bool`

*Defined at line 366*

---

## cookie_domain_match

Whether a cookie with `domain` (no leading dot, lower case) is sent to
`host` (RFC 6265 §5.1.3 domain-match).

```fides
pub fn cookie_domain_match(domain: str, host: str) -> bool {
```

**Parameters:**

- `domain`: `str`
- `host`: `str`

**Returns:** `bool`

*Defined at line 381*

---

## cookie_http_date

Unix seconds as an IMF-fixdate ("Sun, 06 Nov 1994 08:49:37 GMT").

```fides
pub fn cookie_http_date(ts: i64) -> str {
```

**Parameters:**

- `ts`: `i64`

**Returns:** `str`

*Defined at line 403*

---

## cookie_hmac_sha512

HMAC-SHA512 (RFC 2104) of `msg` under `key`, both as bytes.

```fides
pub fn cookie_hmac_sha512(key: Vec<i64>, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `Vec<i64>`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 445*

---

## cookie_ct_eq

Whether two strings are equal, in time that depends only on their
lengths.

```fides
pub fn cookie_ct_eq(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 480*

---

## cookie_sign

`value` signed for the cookie `name`: value + "." + 128 hex digits. The
value must be a valid cookie value without quotes ("" otherwise).

```fides
pub fn cookie_sign(secret: str, name: str, value: str) -> str {
```

**Parameters:**

- `secret`: `str`
- `name`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 499*

---

## cookie_unsign

The value inside a signed cookie when its signature is right for
`name` under `secret`.

```fides
pub fn cookie_unsign(secret: str, name: str, signed: str) -> Option<str> {
```

**Parameters:**

- `secret`: `str`
- `name`: `str`
- `signed`: `str`

**Returns:** `Option<str>`

*Defined at line 508*

---

## Session

One session: its id, data and expiry (seconds).

```fides
pub struct Session {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `expires` | `i64` |

*Defined at line 527*

---

## SessionStore

Sessions living `ttl` seconds after their last use.

```fides
pub struct SessionStore {
```

**Fields:**

| Name | Type |
|------|------|
| `sessions` | `Vec<Session>` |
| `ttl` | `i64` |

*Defined at line 535*

---

## session_store_new

```fides
pub fn session_store_new(ttl: i64) -> SessionStore {
```

**Parameters:**

- `ttl`: `i64`

**Returns:** `SessionStore`

*Defined at line 540*

---

## session_id_from_words

A session id from caller-supplied random words (take them from the
OS: four 64-bit words give 256 bits); lower-case hex.

```fides
pub fn session_id_from_words(words: Vec<i64>) -> str {
```

**Parameters:**

- `words`: `Vec<i64>`

**Returns:** `str`

*Defined at line 548*

---

## session_create

Start a session with `id` (false when one with that id is alive).

```fides
pub fn session_create(st: &mut SessionStore, id: str, now: i64) -> bool {
```

**Parameters:**

- `st`: `&mut SessionStore`
- `id`: `str`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 581*

---

## session_touch

Whether session `id` is alive; renews it (sliding expiry).

```fides
pub fn session_touch(st: &mut SessionStore, id: str, now: i64) -> bool {
```

**Parameters:**

- `st`: `&mut SessionStore`
- `id`: `str`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 593*

---

## session_set

Set `key` in session `id` (false when the session is gone); renews it.

```fides
pub fn session_set(st: &mut SessionStore, id: str, key: str, value: str, now: i64) -> bool {
```

**Parameters:**

- `st`: `&mut SessionStore`
- `id`: `str`
- `key`: `str`
- `value`: `str`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 605*

---

## session_get

The value of `key` in session `id`, when both exist.

```fides
pub fn session_get(st: &SessionStore, id: str, key: str, now: i64) -> Option<str> {
```

**Parameters:**

- `st`: `&SessionStore`
- `id`: `str`
- `key`: `str`
- `now`: `i64`

**Returns:** `Option<str>`

*Defined at line 630*

---

## session_destroy

End session `id` (logout).

```fides
pub fn session_destroy(st: &mut SessionStore, id: str) {
```

**Parameters:**

- `st`: `&mut SessionStore`
- `id`: `str`

*Defined at line 647*

---

## session_sweep

Drop expired sessions; how many are left.

```fides
pub fn session_sweep(st: &mut SessionStore, now: i64) -> i64 {
```

**Parameters:**

- `st`: `&mut SessionStore`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 660*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
