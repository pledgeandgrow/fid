# encoding — Fides Documentation

## Contents

- [enc_bytes_to_str](#enc_bytes_to_str) (function)
- [enc_str_to_bytes](#enc_str_to_bytes) (function)
- [b64_encode_bytes](#b64_encode_bytes) (function)
- [b64_encode](#b64_encode) (function)
- [b64_decode_bytes](#b64_decode_bytes) (function)
- [b64_decode](#b64_decode) (function)
- [b64_valid](#b64_valid) (function)
- [hex_encode_bytes](#hex_encode_bytes) (function)
- [hex_encode_bytes_upper](#hex_encode_bytes_upper) (function)
- [hex_encode](#hex_encode) (function)
- [hex_encode_upper](#hex_encode_upper) (function)
- [hex_decode_bytes](#hex_decode_bytes) (function)
- [hex_decode](#hex_decode) (function)
- [hex_valid](#hex_valid) (function)
- [url_encode_bytes](#url_encode_bytes) (function)
- [url_encode](#url_encode) (function)
- [url_encode_component](#url_encode_component) (function)
- [url_encode_form](#url_encode_form) (function)
- [url_decode_bytes](#url_decode_bytes) (function)
- [url_decode](#url_decode) (function)
- [url_decode_form](#url_decode_form) (function)
- [utf8_valid_bytes](#utf8_valid_bytes) (function)
- [utf8_valid](#utf8_valid) (function)
- [utf8_decode](#utf8_decode) (function)
- [utf8_len](#utf8_len) (function)
- [utf8_encode](#utf8_encode) (function)
- [utf8_encode_bytes](#utf8_encode_bytes) (function)
- [utf8_encode_all](#utf8_encode_all) (function)

## enc_bytes_to_str

Bytes -> a `str` holding exactly those bytes (NUL-terminated heap
buffer). This is the right way to turn decoded bytes into a string;
`str_from_bytes` re-encodes each element as a codepoint instead.

```fides
pub fn enc_bytes_to_str(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 36*

---

## enc_str_to_bytes

A `str` -> its raw bytes (same as str_bytes, kept for API symmetry).

```fides
pub fn enc_str_to_bytes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 51*

---

## b64_encode_bytes

Encode a byte Vec to base64 text (RFC 4648 with '=' padding).

```fides
pub fn b64_encode_bytes(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 62*

---

## b64_encode

Encode a string's UTF-8 bytes to base64 text.

```fides
pub fn b64_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 94*

---

## b64_decode_bytes

Decode base64 text to a byte Vec.
Whitespace is skipped; '=' ends the stream. Invalid characters make the
decode stop at the first bad group (lenient, like most libraries).

```fides
pub fn b64_decode_bytes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 115*

---

## b64_decode

Decode base64 text to a string. Returns "" when the decoded bytes are
empty or not valid UTF-8 output is not required — bytes are returned
verbatim.

```fides
pub fn b64_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 165*

---

## b64_valid

Check that a string is well-formed base64 (optional whitespace and
'=' padding allowed).

```fides
pub fn b64_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 171*

---

## hex_encode_bytes

Encode a byte Vec to lowercase hex text ("0a3f...").

```fides
pub fn hex_encode_bytes(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 205*

---

## hex_encode_bytes_upper

Encode a byte Vec to uppercase hex text ("0A3F...").

```fides
pub fn hex_encode_bytes_upper(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 219*

---

## hex_encode

Encode a string's bytes to lowercase hex text.

```fides
pub fn hex_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 233*

---

## hex_encode_upper

Encode a string's bytes to uppercase hex text.

```fides
pub fn hex_encode_upper(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 238*

---

## hex_decode_bytes

Decode hex text to a byte Vec. Whitespace is skipped. Stops at the
first invalid character (or an odd trailing half-byte).

```fides
pub fn hex_decode_bytes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 252*

---

## hex_decode

Decode hex text to a string.

```fides
pub fn hex_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 277*

---

## hex_valid

Check that a string is well-formed hex (even number of digits,
whitespace ignored).

```fides
pub fn hex_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 283*

---

## url_encode_bytes

Percent-encode a byte Vec: unreserved bytes pass through, everything
else becomes %XX (uppercase hex).

```fides
pub fn url_encode_bytes(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 314*

---

## url_encode

Percent-encode a string for use in a URL component
(space becomes %20).

```fides
pub fn url_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 334*

---

## url_encode_component

Percent-encode a string for use in a URL component (alias).

```fides
pub fn url_encode_component(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 339*

---

## url_encode_form

Percent-encode with HTML-form rules: like url_encode but a space
becomes '+'.

```fides
pub fn url_encode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 345*

---

## url_decode_bytes

Decode percent-encoded text to a byte Vec.
"%XX" -> byte XX, "+" stays "+", a malformed '%' is passed through.

```fides
pub fn url_decode_bytes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 368*

---

## url_decode

Decode percent-encoded text to a string. '+' is NOT translated
(use url_decode_form for query strings).

```fides
pub fn url_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 399*

---

## url_decode_form

Decode a form/query-encoded string: "%XX" -> byte, '+' -> space.

```fides
pub fn url_decode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 404*

---

## utf8_valid_bytes

Check that a byte Vec is valid UTF-8 (no overlongs, no surrogates,
no out-of-range codepoints, truncated sequences rejected).

```fides
pub fn utf8_valid_bytes(bytes: Vec) -> bool {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `bool`

*Defined at line 427*

---

## utf8_valid

Check that a string's bytes are valid UTF-8.

```fides
pub fn utf8_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 462*

---

## utf8_decode

Decode a string to a Vec of Unicode codepoints (i64).
Invalid sequences decode as U+FFFD (65533) and consume one byte.

```fides
pub fn utf8_decode(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 468*

---

## utf8_len

Count the number of UTF-8 codepoints (characters) in a string.
Invalid bytes each count as one (replacement) character.

```fides
pub fn utf8_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 519*

---

## utf8_encode

Encode one Unicode codepoint as its UTF-8 string.
Codepoints above U+10FFFF and surrogates encode as U+FFFD.

```fides
pub fn utf8_encode(cp: i64) -> str {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `str`

*Defined at line 540*

---

## utf8_encode_bytes

Raw UTF-8 bytes of one codepoint as a Vec<i64> (for byte pipelines).
Codepoints above U+10FFFF and surrogates encode as U+FFFD.

```fides
pub fn utf8_encode_bytes(cp: i64) -> Vec {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `Vec`

*Defined at line 553*

---

## utf8_encode_all

Encode a Vec of codepoints as a UTF-8 string.

```fides
pub fn utf8_encode_all(cps: Vec) -> str {
```

**Parameters:**

- `cps`: `Vec`

**Returns:** `str`

*Defined at line 578*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
