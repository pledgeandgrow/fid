[All modules](index.md)

# deep_link -- Fides Documentation

## Contents

- [DlUrl](#DlUrl) (struct)
- [dl_segments](#dl_segments) (function)
- [dl_parse](#dl_parse) (function)
- [dl_query](#dl_query) (function)
- [DlRoute](#DlRoute) (struct)
- [DlRouter](#DlRouter) (struct)
- [dl_router_new](#dl_router_new) (function)
- [dl_add](#dl_add) (function)
- [dl_exclude](#dl_exclude) (function)
- [DlMatch](#DlMatch) (struct)
- [dl_route](#dl_route) (function)
- [dl_param](#dl_param) (function)
- [dl_build](#dl_build) (function)
- [dl_aasa_path](#dl_aasa_path) (function)
- [dl_aasa](#dl_aasa) (function)
- [dl_fingerprint](#dl_fingerprint) (function)
- [dl_assetlinks](#dl_assetlinks) (function)
- [dl_android_path_attr](#dl_android_path_attr) (function)
- [dl_intent_filters](#dl_intent_filters) (function)

## DlUrl

```fides
pub struct DlUrl {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `scheme` | `str,  // lower case` |
| `host` | `str,  // lower case` |
| `port` | `i64,  // -1 when absent` |
| `segments` | `Vec<str>,  // decoded path segments` |
| `query_keys` | `Vec<str>` |
| `query_vals` | `Vec<str>` |
| `fragment` | `str` |

*Defined at line 29*

---

## dl_segments

Split a path into decoded, non-empty segments.

```fides
pub fn dl_segments(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 47*

---

## dl_parse

Parse "scheme://host[:port]/path?query#fragment" (also "scheme:/path"
and "scheme:path" with an empty host).

```fides
pub fn dl_parse(url: str) -> DlUrl {
```

**Parameters:**

- `url`: `str`

**Returns:** `DlUrl`

*Defined at line 62*

---

## dl_query

The value of a query parameter ("" when absent).

```fides
pub fn dl_query(u: &DlUrl, key: str) -> str {
```

**Parameters:**

- `u`: `&DlUrl`
- `key`: `str`

**Returns:** `str`

*Defined at line 142*

---

## DlRoute

```fides
pub struct DlRoute {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `scheme` | `str` |
| `host` | `str,  // "" any for custom schemes; "*.x.com" subdomains` |
| `parts` | `Vec<str>,  // literal, ":name" or "*name" (last only)` |
| `exclude` | `bool,  // a path the app must not open (AASA exclude)` |

*Defined at line 155*

---

## DlRouter

```fides
pub struct DlRouter {
```

**Fields:**

| Name | Type |
|------|------|
| `routes` | `Vec<DlRoute>` |
| `errors` | `Vec<str>` |

*Defined at line 163*

---

## dl_router_new

```fides
pub fn dl_router_new() -> DlRouter {
```

**Returns:** `DlRouter`

*Defined at line 168*

---

## dl_add

Add a route from a pattern URL; false (and an error recorded) when the
pattern is malformed.

```fides
pub fn dl_add(r: &mut DlRouter, name: str, pattern: str) -> bool {
```

**Parameters:**

- `r`: `&mut DlRouter`
- `name`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 176*

---

## dl_exclude

Add a path the app must not handle (listed as excluded in the AASA).

```fides
pub fn dl_exclude(r: &mut DlRouter, pattern: str) -> bool {
```

**Parameters:**

- `r`: `&mut DlRouter`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 181*

---

## DlMatch

A match: the route's name and its parameters (path, then query).

```fides
pub struct DlMatch {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `name` | `str` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `score` | `i64` |

*Defined at line 222*

---

## dl_route

Route a URL: the best match (excluded paths never match), with the
query parameters appended after the path parameters.

```fides
pub fn dl_route(r: &DlRouter, url: str) -> DlMatch {
```

**Parameters:**

- `r`: `&DlRouter`
- `url`: `str`

**Returns:** `DlMatch`

*Defined at line 294*

---

## dl_param

A parameter of a match ("" when absent).

```fides
pub fn dl_param(m: &DlMatch, key: str) -> str {
```

**Parameters:**

- `m`: `&DlMatch`
- `key`: `str`

**Returns:** `str`

*Defined at line 324*

---

## dl_build

Build the URL of a route from parameters (keys / vals); parameters the
path does not use become the query. "" when the route is unknown or a
path parameter is missing.

```fides
pub fn dl_build(r: &DlRouter, name: str, keys: Vec<str>, vals: Vec<str>) -> str {
```

**Parameters:**

- `r`: `&DlRouter`
- `name`: `str`
- `keys`: `Vec<str>`
- `vals`: `Vec<str>`

**Returns:** `str`

*Defined at line 338*

---

## dl_aasa_path

The AASA path pattern of a route: parameters become "*" (one
segment: "?" patterns cannot say that, "*" is what Apple documents).

```fides
pub fn dl_aasa_path(rt: &DlRoute) -> str {
```

**Parameters:**

- `rt`: `&DlRoute`

**Returns:** `str`

*Defined at line 409*

---

## dl_aasa

The apple-app-site-association file for the https routes on `host`:
excluded paths first (the first matching component wins).

```fides
pub fn dl_aasa(r: &DlRouter, app_ids: Vec<str>, host: str) -> str {
```

**Parameters:**

- `r`: `&DlRouter`
- `app_ids`: `Vec<str>`
- `host`: `str`

**Returns:** `str`

*Defined at line 429*

---

## dl_fingerprint

A SHA-256 certificate fingerprint as assetlinks wants it: 32 bytes,
upper-case hex, colon separated ("" when not 32 bytes of hex).

```fides
pub fn dl_fingerprint(fp: str) -> str {
```

**Parameters:**

- `fp`: `str`

**Returns:** `str`

*Defined at line 475*

---

## dl_assetlinks

The assetlinks.json statement list for one package; "" when a
fingerprint is malformed.

```fides
pub fn dl_assetlinks(package: str, fingerprints: Vec<str>) -> str {
```

**Parameters:**

- `package`: `str`
- `fingerprints`: `Vec<str>`

**Returns:** `str`

*Defined at line 494*

---

## dl_android_path_attr

The <data> path attribute of a route: pathPrefix when only the tail is
open, pathPattern (".*" per parameter, literal dots escaped) when a
parameter is in the middle, path when it is all literal.

```fides
pub fn dl_android_path_attr(rt: &DlRoute) -> str {
```

**Parameters:**

- `rt`: `&DlRoute`

**Returns:** `str`

*Defined at line 520*

---

## dl_intent_filters

One <intent-filter> per route (excluded routes are left out).

```fides
pub fn dl_intent_filters(r: &DlRouter) -> str {
```

**Parameters:**

- `r`: `&DlRouter`

**Returns:** `str`

*Defined at line 555*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
