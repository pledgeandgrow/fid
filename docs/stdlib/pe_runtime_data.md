[All modules](index.md)

# data -- Fides Documentation

## Contents

- [pert_json_parse](#pert_json_parse) (function)
- [pert_json_dump](#pert_json_dump) (function)
- [pert_json_dump_pretty](#pert_json_dump_pretty) (function)
- [pert_json_pretty](#pert_json_pretty) (function)
- [pert_json_string](#pert_json_string) (function)
- [pert_json_type](#pert_json_type) (function)
- [pert_json_get](#pert_json_get) (function)
- [pert_json_get_int](#pert_json_get_int) (function)
- [pert_json_get_float](#pert_json_get_float) (function)
- [pert_json_get_bool](#pert_json_get_bool) (function)
- [pert_json_object_get](#pert_json_object_get) (function)
- [pert_json_at](#pert_json_at) (function)
- [pert_json_len](#pert_json_len) (function)
- [pert_http_build_response](#pert_http_build_response) (function)
- [pert_http_parse_request](#pert_http_parse_request) (function)
- [pert_db_mem_open](#pert_db_mem_open) (function)
- [pert_db_mem_exec](#pert_db_mem_exec) (function)
- [pert_db_mem_query](#pert_db_mem_query) (function)
- [pert_db_mem_close](#pert_db_mem_close) (function)

## pert_json_parse

```fides
pub fn pert_json_parse(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 798*

---

## pert_json_dump

```fides
pub fn pert_json_dump(j: str) -> str {
```

**Parameters:**

- `j`: `str`

**Returns:** `str`

*Defined at line 802*

---

## pert_json_dump_pretty

```fides
pub fn pert_json_dump_pretty(j: str) -> str {
```

**Parameters:**

- `j`: `str`

**Returns:** `str`

*Defined at line 806*

---

## pert_json_pretty

The canonical form, one member per line, 2-space indentation.

```fides
pub fn pert_json_pretty(j: str) -> str {
```

**Parameters:**

- `j`: `str`

**Returns:** `str`

*Defined at line 811*

---

## pert_json_string

A string as a JSON string literal: quotes, \" \\ and \n escaped.

```fides
pub fn pert_json_string(j: str) -> str {
```

**Parameters:**

- `j`: `str`

**Returns:** `str`

*Defined at line 816*

---

## pert_json_type

```fides
pub fn pert_json_type(j: str) -> str {
```

**Parameters:**

- `j`: `str`

**Returns:** `str`

*Defined at line 842*

---

## pert_json_get

The value of `key` in the top-level object, as `fid run` gives it (a
string quoted and escaped, nested values canonical, a later duplicate
winning), "null" if there is none; panics when j is not a JSON object.

```fides
pub fn pert_json_get(j: str, key: str) -> str {
```

**Parameters:**

- `j`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 873*

---

## pert_json_get_int

```fides
pub fn pert_json_get_int(j: str, key: str) -> i64 {
```

**Parameters:**

- `j`: `str`
- `key`: `str`

**Returns:** `i64`

*Defined at line 924*

---

## pert_json_get_float

```fides
pub fn pert_json_get_float(j: str, key: str) -> f64 {
```

**Parameters:**

- `j`: `str`
- `key`: `str`

**Returns:** `f64`

*Defined at line 928*

---

## pert_json_get_bool

```fides
pub fn pert_json_get_bool(j: str, key: str) -> i64 {
```

**Parameters:**

- `j`: `str`
- `key`: `str`

**Returns:** `i64`

*Defined at line 932*

---

## pert_json_object_get

```fides
pub fn pert_json_object_get(j: str, key: str) -> str {
```

**Parameters:**

- `j`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 939*

---

## pert_json_at

Element idx of the first array in the text (strings keep their quotes),
"null" past the end.

```fides
pub fn pert_json_at(j: str, idx: i64) -> str {
```

**Parameters:**

- `j`: `str`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 945*

---

## pert_json_len

Top-level commas + 1 (outside strings).

```fides
pub fn pert_json_len(j: str) -> i64 {
```

**Parameters:**

- `j`: `str`

**Returns:** `i64`

*Defined at line 1001*

---

## pert_http_build_response

"HTTP/1.1 <status> <reason>\r\nContent-Length: <n>\r\n\r\n<body>"
(the content type is not used).

```fides
pub fn pert_http_build_response(status: i64, content_type: str, body: str) -> str {
```

**Parameters:**

- `status`: `i64`
- `content_type`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 1094*

---

## pert_http_parse_request

The request as JSON text:
{"method":..,"path":..,"version":..,"headers":{..},"body":".."}
read as the interpreter (compiler/interp.fd p_http_parse_request) and
the C runtime (p_http_parse_request__raw, BG160) read it: a request line
of one word is the method (path "/", version "1.1"), a version not
"HTTP/x" is "1.1", header names and values are trimmed, the body runs
from the first empty line to the next one (or the end), and every text
is escaped (a `"` in the path gave invalid JSON).

```fides
pub fn pert_http_parse_request(request: str) -> str {
```

**Parameters:**

- `request`: `str`

**Returns:** `str`

*Defined at line 1133*

---

## pert_db_mem_open

```fides
pub fn pert_db_mem_open() -> i64 {
```

**Returns:** `i64`

*Defined at line 1250*

---

## pert_db_mem_exec

"INSERT key value" / "SET key value" (any case): 1 when stored.

```fides
pub fn pert_db_mem_exec(db: i64, query: str) -> i64 {
```

**Parameters:**

- `db`: `i64`
- `query`: `str`

**Returns:** `i64`

*Defined at line 1273*

---

## pert_db_mem_query

"... key=K ..." -> K's value ("" if absent); otherwise every key, in
insertion order, as a JSON array of strings.

```fides
pub fn pert_db_mem_query(db: i64, query: str) -> str {
```

**Parameters:**

- `db`: `i64`
- `query`: `str`

**Returns:** `str`

*Defined at line 1324*

---

## pert_db_mem_close

The store's memory is not reclaimed (the program's heap is).

```fides
pub fn pert_db_mem_close(db: i64) {
```

**Parameters:**

- `db`: `i64`

*Defined at line 1370*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
