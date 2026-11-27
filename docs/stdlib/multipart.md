[All modules](index.md)

# multipart -- Fides Documentation

## Contents

- [MpPart](#MpPart) (struct)
- [MpEvent](#MpEvent) (struct)
- [MP_PART_START:](#MP_PART_START:) (const)
- [MP_DATA:](#MP_DATA:) (const)
- [MP_PART_END:](#MP_PART_END:) (const)
- [MP_END:](#MP_END:) (const)
- [MP_ERROR:](#MP_ERROR:) (const)
- [MpBody](#MpBody) (struct)
- [MpStream](#MpStream) (struct)
- [mp_stream_new](#mp_stream_new) (function)
- [mp_header_param](#mp_header_param) (function)
- [mp_boundary_of](#mp_boundary_of) (function)
- [mp_header](#mp_header) (function)
- [mp_feed](#mp_feed) (function)
- [mp_finish](#mp_finish) (function)
- [mp_parse](#mp_parse) (function)
- [mp_find_part](#mp_find_part) (function)
- [mp_field](#mp_field) (function)
- [mp_file](#mp_file) (function)
- [mp_build](#mp_build) (function)
- [mp_boundary_ok](#mp_boundary_ok) (function)
- [mp_content_type](#mp_content_type) (function)
- [mp_new_boundary](#mp_new_boundary) (function)

## MpPart

```fides
pub struct MpPart {
```

**Fields:**

| Name | Type |
|------|------|
| `header_names` | `Vec<str>,  // lowercase` |
| `header_vals` | `Vec<str>` |
| `name` | `str` |
| `filename` | `str` |
| `content_type` | `str,  // "text/plain" when absent (RFC 2046)` |
| `data` | `Vec<i64>` |

*Defined at line 29*

---

## MpEvent

kind: 0 part start (`part` has the headers), 1 data (`data`), 2 part
end, 3 body end, 4 error (`error`).

```fides
pub struct MpEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `part` | `MpPart` |
| `data` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 40*

---

## MP_PART_START:

```fides
pub const MP_PART_START: i64 = 0;
```

*Defined at line 47*

---

## MP_DATA:

```fides
pub const MP_DATA: i64 = 1;
```

*Defined at line 48*

---

## MP_PART_END:

```fides
pub const MP_PART_END: i64 = 2;
```

*Defined at line 49*

---

## MP_END:

```fides
pub const MP_END: i64 = 3;
```

*Defined at line 50*

---

## MP_ERROR:

```fides
pub const MP_ERROR: i64 = 4;
```

*Defined at line 51*

---

## MpBody

```fides
pub struct MpBody {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `parts` | `Vec<MpPart>` |

*Defined at line 53*

---

## MpStream

```fides
pub struct MpStream {
```

**Fields:**

| Name | Type |
|------|------|
| `boundary` | `str` |
| `buf` | `Vec<i64>` |
| `state` | `i64` |
| `cur` | `MpPart` |
| `parts_seen` | `i64` |

*Defined at line 61*

---

## mp_stream_new

```fides
pub fn mp_stream_new(boundary: str) -> MpStream {
```

**Parameters:**

- `boundary`: `str`

**Returns:** `MpStream`

*Defined at line 81*

---

## mp_header_param

A parameter of a header value like `form-data; name="a"; filename=x`
(quoted strings with \-escapes or tokens; names compare without case).
"" when absent.

```fides
pub fn mp_header_param(value: str, param: str) -> str {
```

**Parameters:**

- `value`: `str`
- `param`: `str`

**Returns:** `str`

*Defined at line 135*

---

## mp_boundary_of

The boundary parameter of a multipart Content-Type value.

```fides
pub fn mp_boundary_of(content_type: str) -> str {
```

**Parameters:**

- `content_type`: `str`

**Returns:** `str`

*Defined at line 188*

---

## mp_header

A part's header value by name (case-insensitive), "" when absent.

```fides
pub fn mp_header(p: &MpPart, name: str) -> str {
```

**Parameters:**

- `p`: `&MpPart`
- `name`: `str`

**Returns:** `str`

*Defined at line 193*

---

## mp_feed

Feed the next chunk of the body; the events it completes.

```fides
pub fn mp_feed(s: &mut MpStream, chunk: Vec<i64>) -> Vec<MpEvent> {
```

**Parameters:**

- `s`: `&mut MpStream`
- `chunk`: `Vec<i64>`

**Returns:** `Vec<MpEvent>`

*Defined at line 400*

---

## mp_finish

The end of the input: an error event when the body was not closed by
its final "--boundary--".

```fides
pub fn mp_finish(s: &mut MpStream) -> Vec<MpEvent> {
```

**Parameters:**

- `s`: `&mut MpStream`

**Returns:** `Vec<MpEvent>`

*Defined at line 417*

---

## mp_parse

Parse a whole body.

```fides
pub fn mp_parse(body: Vec<i64>, boundary: str) -> MpBody {
```

**Parameters:**

- `body`: `Vec<i64>`
- `boundary`: `str`

**Returns:** `MpBody`

*Defined at line 431*

---

## mp_find_part

The first part named `name`, its index or -1.

```fides
pub fn mp_find_part(b: &MpBody, name: str) -> i64 {
```

**Parameters:**

- `b`: `&MpBody`
- `name`: `str`

**Returns:** `i64`

*Defined at line 472*

---

## mp_field

A form field part.

```fides
pub fn mp_field(name: str, value: str) -> MpPart {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `MpPart`

*Defined at line 486*

---

## mp_file

A file part.

```fides
pub fn mp_file(name: str, filename: str, content_type: str, data: Vec<i64>) -> MpPart {
```

**Parameters:**

- `name`: `str`
- `filename`: `str`
- `content_type`: `str`
- `data`: `Vec<i64>`

**Returns:** `MpPart`

*Defined at line 494*

---

## mp_build

A multipart/form-data body. Each part gets Content-Disposition (name,
filename when set) and, for files, its Content-Type; extra headers in
a part's header lists are written after them.

```fides
pub fn mp_build(parts: &Vec<MpPart>, boundary: str) -> Vec<i64> {
```

**Parameters:**

- `parts`: `&Vec<MpPart>`
- `boundary`: `str`

**Returns:** `Vec<i64>`

*Defined at line 535*

---

## mp_boundary_ok

Whether `boundary` is usable for these parts: RFC 2046 §5.1.1 forbids
the delimiter (CRLF "--" boundary) inside any part's data or at its
start.

```fides
pub fn mp_boundary_ok(parts: &Vec<MpPart>, boundary: str) -> bool {
```

**Parameters:**

- `parts`: `&Vec<MpPart>`
- `boundary`: `str`

**Returns:** `bool`

*Defined at line 573*

---

## mp_content_type

The Content-Type header value for a form-data body with `boundary`.

```fides
pub fn mp_content_type(boundary: str) -> str {
```

**Parameters:**

- `boundary`: `str`

**Returns:** `str`

*Defined at line 592*

---

## mp_new_boundary

A boundary from 64 random bits (hex), e.g. for mp_build.

```fides
pub fn mp_new_boundary(r: i64) -> str {
```

**Parameters:**

- `r`: `i64`

**Returns:** `str`

*Defined at line 597*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
