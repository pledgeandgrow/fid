[All modules](index.md)

# url -- Fides Documentation

## Contents

- [Url](#Url) (struct)
- [url_encode](#url_encode) (function)
- [url_encode_path](#url_encode_path) (function)
- [url_path_escape](#url_path_escape) (function)
- [url_encode_form](#url_encode_form) (function)
- [url_decode](#url_decode) (function)
- [url_decode_form](#url_decode_form) (function)
- [url_parse](#url_parse) (function)
- [url_to_str](#url_to_str) (function)
- [url_query_get](#url_query_get) (function)
- [url_query_has](#url_query_has) (function)
- [url_query_keys](#url_query_keys) (function)
- [url_query_build](#url_query_build) (function)
- [url_resolve](#url_resolve) (function)
- [url_normalize_path](#url_normalize_path) (function)

## Url

A parsed absolute or relative URL. Missing parts are "" (port is -1
when not present; userinfo holds "user" or "user:pass"). `path` is the
path as written, percent-encoded: a `%2F` in it is data in a segment,
not a separator (RFC 3986 section 3.3). `url_decode` gives it decoded.

```fides
pub struct Url {
```

**Fields:**

| Name | Type |
|------|------|
| `scheme` | `str` |
| `userinfo` | `str` |
| `host` | `str` |
| `port` | `i64` |
| `path` | `str` |
| `query` | `str` |
| `fragment` | `str` |

*Defined at line 21*

---

## url_encode

Percent-encode `s` — only unreserved bytes pass through.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_encode("a b/c"), "a%20b%2Fc");
```

```fides
pub fn url_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 60*

---

## url_encode_path

Percent-encode a path, keeping "/" and the other characters a path may hold.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_encode_path("/docs/my file.txt"), "/docs/my%20file.txt");
```

```fides
pub fn url_encode_path(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 98*

---

## url_path_escape

Percent-encode a path that is already percent-encoded, as a parsed URL
holds it: valid `%XX` escapes stay, as do "/" and the characters a path
may hold; every other byte is encoded (a lone "%" too).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_path_escape("/a%2Fb/c d"), "/a%2Fb/c%20d");
assert_eq(url_path_escape("/100%"), "/100%25");
```

```fides
pub fn url_path_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 109*

---

## url_encode_form

Encode for query/form values — like url_encode but space → '+'.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_encode_form("a b&c"), "a+b%26c");
```

```fides
pub fn url_encode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 141*

---

## url_decode

Percent-decode `s`; '+' stays literal (use url_decode_form for query).
Invalid % sequences are passed through unchanged.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_decode("a%20b%2Fc"), "a b/c");
```

```fides
pub fn url_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 164*

---

## url_decode_form

Percent-decode with '+' → ' ' (application/x-www-form-urlencoded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_decode_form("a+b%26c"), "a b&c");
```

```fides
pub fn url_decode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 208*

---

## url_parse

Parse a URL string into a Url struct. Lenient: missing components
are left empty; port is -1 when absent or unparseable.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = url_parse("https://user@example.com:8443/a/b?x=1&y=2#top");
assert_eq(u.scheme, "https");
assert_eq(u.userinfo, "user");
assert_eq(u.host, "example.com");
assert_eq(u.port, 8443);
assert_eq(u.path, "/a/b");
assert_eq(u.query, "x=1&y=2");
assert_eq(u.fragment, "top");
assert_eq(url_parse("http://example.com").port, -1);
```

```fides
pub fn url_parse(s: str) -> Url {
```

**Parameters:**

- `s`: `str`

**Returns:** `Url`

*Defined at line 241*

---

## url_to_str

Serialize a Url struct back to a string (components re-encoded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = url_parse("http://example.com/x?q=1");
u.path = "/y";
assert_eq(url_to_str(u), "http://example.com/y?q=1");
```

```fides
pub fn url_to_str(u: Url) -> str {
```

**Parameters:**

- `u`: `Url`

**Returns:** `str`

*Defined at line 364*

---

## url_query_get

Value of `key` in a query string ("a=1&b=2"); "" when absent.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_query_get("name=Ada%20L&age=36", "name"), "Ada L");
assert_eq(url_query_get("a=1", "b"), "");
```

```fides
pub fn url_query_get(query: str, key: str) -> str {
```

**Parameters:**

- `query`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 389*

---

## url_query_has

True if `key` appears in the query string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(url_query_has("debug&x=1", "debug"));
assert(!url_query_has("x=1", "y"));
```

```fides
pub fn url_query_has(query: str, key: str) -> bool {
```

**Parameters:**

- `query`: `str`
- `key`: `str`

**Returns:** `bool`

*Defined at line 414*

---

## url_query_keys

All keys of the query string in order (decoded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var k = url_query_keys("a=1&b=2&c");
assert_eq(k.len(), 3);
assert_eq(k.get(2), "c");
```

```fides
pub fn url_query_keys(query: str) -> Vec<str> {
```

**Parameters:**

- `query`: `str`

**Returns:** `Vec<str>`

*Defined at line 438*

---

## url_query_build

Build a query string from parallel key/value vecs (form-encoded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var k: Vec<str> = Vec::new();
var v: Vec<str> = Vec::new();
k.push("q"); v.push("fides lang");
k.push("page"); v.push("2");
assert_eq(url_query_build(k, v), "q=fides+lang&page=2");
```

```fides
pub fn url_query_build(keys: Vec<str>, values: Vec<str>) -> str {
```

**Parameters:**

- `keys`: `Vec<str>`
- `values`: `Vec<str>`

**Returns:** `str`

*Defined at line 467*

---

## url_resolve

Resolve `rel` against `base` (RFC 3986 §5.2, no scheme changes).
Handles absolute paths, relative paths, and dot segments.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var base = url_parse("http://a.com/b/c/d");
assert_eq(url_resolve(base, "../e"), "http://a.com/b/e");
assert_eq(url_resolve(base, "/x"), "http://a.com/x");
```

```fides
pub fn url_resolve(base: Url, rel: str) -> str {
```

**Parameters:**

- `base`: `Url`
- `rel`: `str`

**Returns:** `str`

*Defined at line 513*

---

## url_normalize_path

Collapse "." and ".." segments in a path.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_normalize_path("/a/./b/../c"), "/a/c");
```

```fides
pub fn url_normalize_path(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 569*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
