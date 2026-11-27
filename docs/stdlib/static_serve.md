[All modules](index.md)

# static_serve -- Fides Documentation

## Contents

- [FileStore](#FileStore) (struct)
- [ServeConfig](#ServeConfig) (struct)
- [ServeRequest](#ServeRequest) (struct)
- [ServeResponse](#ServeResponse) (struct)
- [serve_config](#serve_config) (function)
- [serve_request](#serve_request) (function)
- [serve_header](#serve_header) (function)
- [serve_resp_header](#serve_resp_header) (function)
- [serve_normalize](#serve_normalize) (function)
- [serve_http_date](#serve_http_date) (function)
- [serve_parse_date](#serve_parse_date) (function)
- [serve_etag](#serve_etag) (function)
- [serve_none_match](#serve_none_match) (function)
- [serve_accept_q](#serve_accept_q) (function)
- [serve_parse_range](#serve_parse_range) (function)
- [serve_static](#serve_static) (function)

## FileStore

```fides
pub struct FileStore {
```

**Fields:**

| Name | Type |
|------|------|
| `stat` | `fn(str) -> str` |
| `read` | `fn(str, i64, i64) -> Vec<i64>` |

*Defined at line 44*

---

## ServeConfig

```fides
pub struct ServeConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `root` | `str` |
| `index` | `str` |
| `precompressed` | `bool` |
| `allow_dotfiles` | `bool` |
| `cache_control` | `str` |
| `boundary` | `str,  // for multipart/byteranges` |

*Defined at line 49*

---

## ServeRequest

```fides
pub struct ServeRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str` |
| `path` | `str` |
| `header_names` | `Vec<str>` |
| `header_vals` | `Vec<str>` |

*Defined at line 58*

---

## ServeResponse

```fides
pub struct ServeResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `header_names` | `Vec<str>` |
| `header_vals` | `Vec<str>` |
| `body` | `Vec<i64>` |
| `file` | `str,  // the file served ("" when none)` |

*Defined at line 65*

---

## serve_config

```fides
pub fn serve_config(root: str) -> ServeConfig {
```

**Parameters:**

- `root`: `str`

**Returns:** `ServeConfig`

*Defined at line 73*

---

## serve_request

```fides
pub fn serve_request(method: str, path: str) -> ServeRequest {
```

**Parameters:**

- `method`: `str`
- `path`: `str`

**Returns:** `ServeRequest`

*Defined at line 93*

---

## serve_header

```fides
pub fn serve_header(r: &mut ServeRequest, name: str, value: str) {
```

**Parameters:**

- `r`: `&mut ServeRequest`
- `name`: `str`
- `value`: `str`

*Defined at line 99*

---

## serve_resp_header

A response header by name (case-insensitive), "" when absent.

```fides
pub fn serve_resp_header(r: &ServeResponse, name: str) -> str {
```

**Parameters:**

- `r`: `&ServeResponse`
- `name`: `str`

**Returns:** `str`

*Defined at line 127*

---

## serve_normalize

Normalize a request path to "/a/b.txt" form; "" when it must be
refused (bad escape, NUL, backslash, an encoded '/', or ".." above the
root). A trailing '/' is kept.

```fides
pub fn serve_normalize(raw: str) -> str {
```

**Parameters:**

- `raw`: `str`

**Returns:** `str`

*Defined at line 176*

---

## serve_http_date

IMF-fixdate of Unix seconds (1970 and later).

```fides
pub fn serve_http_date(ts: i64) -> str {
```

**Parameters:**

- `ts`: `i64`

**Returns:** `str`

*Defined at line 286*

---

## serve_parse_date

Parse an HTTP date (IMF-fixdate, RFC 850, asctime) to Unix seconds;
-1 when it is not one.

```fides
pub fn serve_parse_date(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 324*

---

## serve_etag

The strong ETag of a file version: "size-mtime[-enc]" in hex.

```fides
pub fn serve_etag(size: i64, mtime: i64, encoding: str) -> str {
```

**Parameters:**

- `size`: `i64`
- `mtime`: `i64`
- `encoding`: `str`

**Returns:** `str`

*Defined at line 394*

---

## serve_none_match

If-None-Match against an ETag (weak comparison): true when it matches.

```fides
pub fn serve_none_match(header: str, etag: str) -> bool {
```

**Parameters:**

- `header`: `str`
- `etag`: `str`

**Returns:** `bool`

*Defined at line 412*

---

## serve_accept_q

The quality (0..1000) Accept-Encoding gives `coding` ("*" counts when
the coding is not named); identity is acceptable unless refused.

```fides
pub fn serve_accept_q(header: str, coding: str) -> i64 {
```

**Parameters:**

- `header`: `str`
- `coding`: `str`

**Returns:** `i64`

*Defined at line 431*

---

## serve_parse_range

Parse a Range header for a body of `size` bytes: [start, end, start,
end, ...] (inclusive ends) of the satisfiable ranges; ["-1"] encoded as
[-1] when the header is malformed or has too many ranges (ignore it);
[] when none is satisfiable.

```fides
pub fn serve_parse_range(header: str, size: i64) -> Vec<i64> {
```

**Parameters:**

- `header`: `str`
- `size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 495*

---

## serve_static

Serve a request from a FileStore under the config's root.

```fides
pub fn serve_static(cfg: &ServeConfig, store: &FileStore, req: &ServeRequest) -> ServeResponse {
```

**Parameters:**

- `cfg`: `&ServeConfig`
- `store`: `&FileStore`
- `req`: `&ServeRequest`

**Returns:** `ServeResponse`

*Defined at line 598*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
