[All modules](index.md)

# router -- Fides Documentation

## Contents

- [RT_STATIC:](#RT_STATIC:) (const)
- [RT_PARAM:](#RT_PARAM:) (const)
- [RT_WILD:](#RT_WILD:) (const)
- [RouteParams](#RouteParams) (struct)
- [route_params_new](#route_params_new) (function)
- [route_param](#route_param) (function)
- [route_has_param](#route_has_param) (function)
- [HttpRouter](#HttpRouter) (struct)
- [RouteMatch](#RouteMatch) (struct)
- [http_router_new](#http_router_new) (function)
- [http_router_set_not_found](#http_router_set_not_found) (function)
- [http_router_set_not_allowed](#http_router_set_not_allowed) (function)
- [http_router_strict_slash](#http_router_strict_slash) (function)
- [route_pct_decode](#route_pct_decode) (function)
- [http_router_add](#http_router_add) (function)
- [http_router_get](#http_router_get) (function)
- [http_router_post](#http_router_post) (function)
- [http_router_put](#http_router_put) (function)
- [http_router_delete](#http_router_delete) (function)
- [http_router_patch](#http_router_patch) (function)
- [http_router_len](#http_router_len) (function)
- [RouteGroup](#RouteGroup) (struct)
- [route_group](#route_group) (function)
- [route_group_sub](#route_group_sub) (function)
- [route_group_add](#route_group_add) (function)
- [http_router_match](#http_router_match) (function)
- [http_router_handle](#http_router_handle) (function)
- [http_router_serve_fn](#http_router_serve_fn) (function)
- [route_query](#route_query) (function)
- [http_router_describe](#http_router_describe) (function)

## RT_STATIC:

```fides
pub const RT_STATIC: i64 = 3;
```

*Defined at line 47*

---

## RT_PARAM:

```fides
pub const RT_PARAM: i64 = 2;
```

*Defined at line 48*

---

## RT_WILD:

```fides
pub const RT_WILD: i64 = 1;
```

*Defined at line 49*

---

## RouteParams

The parameters a match captured: names[i] = vals[i]; a wildcard's
value is also in `wildcard` (segments joined by '/', decoded).

```fides
pub struct RouteParams {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `wildcard` | `str` |

*Defined at line 53*

---

## route_params_new

```fides
pub fn route_params_new() -> RouteParams {
```

**Returns:** `RouteParams`

*Defined at line 59*

---

## route_param

The value of parameter `name`, or "".

```fides
pub fn route_param(p: &RouteParams, name: str) -> str {
```

**Parameters:**

- `p`: `&RouteParams`
- `name`: `str`

**Returns:** `str`

*Defined at line 66*

---

## route_has_param

```fides
pub fn route_has_param(p: &RouteParams, name: str) -> bool {
```

**Parameters:**

- `p`: `&RouteParams`
- `name`: `str`

**Returns:** `bool`

*Defined at line 77*

---

## HttpRouter

```fides
pub struct HttpRouter {
```

**Fields:**

| Name | Type |
|------|------|
| `routes` | `Vec<RouteEntry>` |
| `not_found` | `fn(HttpRequest, RouteParams) -> str` |
| `not_allowed` | `fn(HttpRequest, RouteParams) -> str` |
| `strict_slash` | `bool` |
| `auto_options` | `bool` |
| `error` | `str` |

*Defined at line 96*

---

## RouteMatch

The outcome of matching: status 200 (index = the route), 404, 405
(allow = the methods the path takes, "GET, HEAD, OPTIONS"), 400 (bad
escape), or 204 for an automatic OPTIONS answer.

```fides
pub struct RouteMatch {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `index` | `i64` |
| `params` | `RouteParams` |
| `allow` | `str` |
| `pattern` | `str` |

*Defined at line 108*

---

## http_router_new

```fides
pub fn http_router_new() -> HttpRouter {
```

**Returns:** `HttpRouter`

*Defined at line 124*

---

## http_router_set_not_found

The handler for paths no route matches (default: plain 404).

```fides
pub fn http_router_set_not_found(r: &mut HttpRouter, h: fn(HttpRequest, RouteParams) -> str) {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `h`: `fn(HttpRequest`

*Defined at line 131*

---

## http_router_set_not_allowed

The handler for a known path with the wrong method (default: plain
405). The router adds the Allow header to whatever it returns.

```fides
pub fn http_router_set_not_allowed(r: &mut HttpRouter, h: fn(HttpRequest, RouteParams) -> str) {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `h`: `fn(HttpRequest`

*Defined at line 137*

---

## http_router_strict_slash

Treat "/a/" and "/a" as different paths.

```fides
pub fn http_router_strict_slash(r: &mut HttpRouter, on: bool) {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `on`: `bool`

*Defined at line 142*

---

## route_pct_decode

Percent-decode one path segment; ok false on a bad escape. '+' stays
'+' (it means a space only in a query).

```fides
pub fn route_pct_decode(s: str, ok: &mut bool) -> str {
```

**Parameters:**

- `s`: `str`
- `ok`: `&mut bool`

**Returns:** `str`

*Defined at line 163*

---

## http_router_add

Add a route. false (and r.error set) when the pattern is bad: a
wildcard not last, an empty parameter name, a parameter name used
twice, or the same method and pattern added before.

```fides
pub fn http_router_add(r: &mut HttpRouter, method: str, pattern: str, handler: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `method`: `str`
- `pattern`: `str`
- `handler`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 246*

---

## http_router_get

```fides
pub fn http_router_get(r: &mut HttpRouter, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 322*

---

## http_router_post

```fides
pub fn http_router_post(r: &mut HttpRouter, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 326*

---

## http_router_put

```fides
pub fn http_router_put(r: &mut HttpRouter, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 330*

---

## http_router_delete

```fides
pub fn http_router_delete(r: &mut HttpRouter, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 334*

---

## http_router_patch

```fides
pub fn http_router_patch(r: &mut HttpRouter, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 338*

---

## http_router_len

```fides
pub fn http_router_len(r: &HttpRouter) -> i64 {
```

**Parameters:**

- `r`: `&HttpRouter`

**Returns:** `i64`

*Defined at line 342*

---

## RouteGroup

A prefix shared by several routes.

```fides
pub struct RouteGroup {
```

**Fields:**

| Name | Type |
|------|------|
| `prefix` | `str` |

*Defined at line 349*

---

## route_group

```fides
pub fn route_group(prefix: str) -> RouteGroup {
```

**Parameters:**

- `prefix`: `str`

**Returns:** `RouteGroup`

*Defined at line 361*

---

## route_group_sub

A group nested in another: route_group_sub(route_group("/api"), "/v1")
has the prefix "/api/v1".

```fides
pub fn route_group_sub(g: RouteGroup, prefix: str) -> RouteGroup {
```

**Parameters:**

- `g`: `RouteGroup`
- `prefix`: `str`

**Returns:** `RouteGroup`

*Defined at line 369*

---

## route_group_add

```fides
pub fn route_group_add(r: &mut HttpRouter, g: RouteGroup, method: str, pattern: str, h: fn(HttpRequest, RouteParams) -> str) -> bool {
```

**Parameters:**

- `r`: `&mut HttpRouter`
- `g`: `RouteGroup`
- `method`: `str`
- `pattern`: `str`
- `h`: `fn(HttpRequest`

**Returns:** `bool`

*Defined at line 375*

---

## http_router_match

Match a method and path (path without the query).

```fides
pub fn http_router_match(r: &HttpRouter, method: str, path: str) -> RouteMatch {
```

**Parameters:**

- `r`: `&HttpRouter`
- `method`: `str`
- `path`: `str`

**Returns:** `RouteMatch`

*Defined at line 454*

---

## http_router_handle

Route one request and return the response.

```fides
pub fn http_router_handle(r: &HttpRouter, req: HttpRequest) -> str {
```

**Parameters:**

- `r`: `&HttpRouter`
- `req`: `HttpRequest`

**Returns:** `str`

*Defined at line 592*

---

## http_router_serve_fn

A handler for http_serve that routes with (a copy of) `r`.

```fides
pub fn http_router_serve_fn(r: HttpRouter) -> fn(HttpRequest) -> str {
```

**Parameters:**

- `r`: `HttpRouter`

**Returns:** `fn(HttpRequest) -> str`

*Defined at line 629*

---

## route_query

The value of `name` in a query string (form-decoded: '+' is a space),
or "" when absent. route_query(req.query, "page").

```fides
pub fn route_query(query: str, name: str) -> str {
```

**Parameters:**

- `query`: `str`
- `name`: `str`

**Returns:** `str`

*Defined at line 639*

---

## http_router_describe

The route table, one "METHOD pattern" per line (for logs).

```fides
pub fn http_router_describe(r: &HttpRouter) -> str {
```

**Parameters:**

- `r`: `&HttpRouter`

**Returns:** `str`

*Defined at line 666*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
