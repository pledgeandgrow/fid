[All modules](index.md)

# encoding -- Fides Documentation

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

Bytes -> a `str` holding exactly those bytes (a counted, NUL-terminated
heap string). This is the right way to turn decoded bytes into a string;
`str_from_bytes` re-encodes each element as a codepoint instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(104); b.push(105);
assert_eq(enc_bytes_to_str(b), "hi");
```

```fides
pub fn enc_bytes_to_str(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 43*

---

## enc_str_to_bytes

A `str` -> its raw bytes (same as str_bytes, kept for API symmetry).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = enc_str_to_bytes("AB");
assert_eq(b.len(), 2);
assert_eq(b.get(1), 66);
```

```fides
pub fn enc_str_to_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 79*

---

## b64_encode_bytes

Encode a byte Vec to base64 text (RFC 4648 with '=' padding).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(0); b.push(255);
assert_eq(b64_encode_bytes(b), "AP8=");
```

```fides
pub fn b64_encode_bytes(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 95*

---

## b64_encode

Encode a string's UTF-8 bytes to base64 text.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var e = b64_encode("hello");
println(e);
assert_eq(e, "aGVsbG8=");
```

```fides
pub fn b64_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 132*

---

## b64_decode_bytes

Decode base64 text to a byte Vec.
Whitespace is skipped; '=' ends the stream. Invalid characters make the
decode stop at the first bad group (lenient, like most libraries).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = b64_decode_bytes("AP8=");
assert_eq(b.len(), 2);
assert_eq(b.get(1), 255);
```

```fides
pub fn b64_decode_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 173*

---

## b64_decode

Decode base64 text to a string. Returns "" when the decoded bytes are
empty or not valid UTF-8 output is not required — bytes are returned
verbatim.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(b64_decode("aGVsbG8="), "hello");
assert_eq(b64_decode(b64_encode("round trip")), "round trip");
```

```fides
pub fn b64_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 226*

---

## b64_valid

Check that a string is well-formed base64 (optional whitespace and
'=' padding allowed).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(b64_valid("aGVsbG8="));
assert(!b64_valid("a*b"));
```

```fides
pub fn b64_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 236*

---

## hex_encode_bytes

Encode a byte Vec to lowercase hex text ("0a3f...").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(10); b.push(255);
assert_eq(hex_encode_bytes(b), "0aff");
```

```fides
pub fn hex_encode_bytes(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 276*

---

## hex_encode_bytes_upper

Encode a byte Vec to uppercase hex text ("0A3F...").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(10); b.push(255);
assert_eq(hex_encode_bytes_upper(b), "0AFF");
```

```fides
pub fn hex_encode_bytes_upper(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 297*

---

## hex_encode

Encode a string's bytes to lowercase hex text.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hex_encode("Hi"), "4869");
```

```fides
pub fn hex_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 316*

---

## hex_encode_upper

Encode a string's bytes to uppercase hex text.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hex_encode_upper("\n"), "0A");
```

```fides
pub fn hex_encode_upper(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 324*

---

## hex_decode_bytes

Decode hex text to a byte Vec. Whitespace is skipped. Stops at the
first invalid character (or an odd trailing half-byte).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = hex_decode_bytes("de ad");
assert_eq(b.len(), 2);
assert_eq(b.get(0), 222);
```

```fides
pub fn hex_decode_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 352*

---

## hex_decode

Decode hex text to a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(hex_decode("4869"), "Hi");
```

```fides
pub fn hex_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 382*

---

## hex_valid

Check that a string is well-formed hex (even number of digits,
whitespace ignored).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(hex_valid("00ff"));
assert(!hex_valid("0f0"));
assert(!hex_valid("zz"));
```

```fides
pub fn hex_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 393*

---

## url_encode_bytes

Percent-encode a byte Vec: unreserved bytes pass through, everything
else becomes %XX (uppercase hex).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
b.push(97); b.push(32); b.push(47);
assert_eq(url_encode_bytes(b), "a%20%2F");
```

```fides
pub fn url_encode_bytes(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 439*

---

## url_encode

Percent-encode a string for use in a URL component
(space becomes %20).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var e = url_encode("a b&c=d");
println(e);
assert_eq(e, "a%20b%26c%3Dd");
```

```fides
pub fn url_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 466*

---

## url_encode_component

Percent-encode a string for use in a URL component (alias).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_encode_component("x/y"), "x%2Fy");
```

```fides
pub fn url_encode_component(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 474*

---

## url_encode_form

Percent-encode with HTML-form rules: like url_encode but a space
becomes '+'.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_encode_form("a b"), "a+b");
```

```fides
pub fn url_encode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 483*

---

## url_decode_bytes

Decode percent-encoded text to a byte Vec.
"%XX" -> byte XX, "+" stays "+", a malformed '%' is passed through.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = url_decode_bytes("%41+");
assert_eq(b.len(), 2);
assert_eq(b.get(0), 65);
assert_eq(b.get(1), 43);
```

```fides
pub fn url_decode_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 514*

---

## url_decode

Decode percent-encoded text to a string. '+' is NOT translated
(use url_decode_form for query strings).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_decode("a%20b+c"), "a b+c");
```

```fides
pub fn url_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 549*

---

## url_decode_form

Decode a form/query-encoded string: "%XX" -> byte, '+' -> space.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(url_decode_form("a+b%21"), "a b!");
```

```fides
pub fn url_decode_form(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 557*

---

## utf8_valid_bytes

Check that a byte Vec is valid UTF-8 (no overlongs, no surrogates,
no out-of-range codepoints, truncated sequences rejected).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var ok: Vec<i64> = Vec::new();
ok.push(195); ok.push(169);
assert(utf8_valid_bytes(ok));
var bad: Vec<i64> = Vec::new();
bad.push(195);
assert(!utf8_valid_bytes(bad));
```

```fides
pub fn utf8_valid_bytes(bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 637*

---

## utf8_valid

Check that a string's bytes are valid UTF-8.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(utf8_valid("caf" + utf8_encode(233)));
assert(utf8_valid("plain ascii"));
```

```fides
pub fn utf8_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 666*

---

## utf8_decode

Decode a string to a Vec of Unicode codepoints (i64).
Invalid sequences decode as U+FFFD (65533) and consume one byte.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var cps = utf8_decode(utf8_encode(233) + "!");
assert_eq(cps.len(), 2);
assert_eq(cps.get(0), 233);
assert_eq(cps.get(1), 33);
```

```fides
pub fn utf8_decode(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 678*

---

## utf8_len

Count the number of UTF-8 codepoints (characters) in a string.
Invalid bytes each count as one (replacement) character.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = "a" + utf8_encode(8364) + "b";
assert_eq(str_len(s), 5);
assert_eq(utf8_len(s), 3);
```

```fides
pub fn utf8_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 721*

---

## utf8_encode

Encode one Unicode codepoint as its UTF-8 string.
Codepoints above U+10FFFF and surrogates encode as U+FFFD.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_len(utf8_encode(8364)), 3);
assert_eq(utf8_encode(65), "A");
```

```fides
pub fn utf8_encode(cp: i64) -> str {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `str`

*Defined at line 748*

---

## utf8_encode_bytes

Raw UTF-8 bytes of one codepoint as a Vec<i64> (for byte pipelines).
Codepoints above U+10FFFF and surrogates encode as U+FFFD.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b = utf8_encode_bytes(233);
assert_eq(b.len(), 2);
assert_eq(b.get(0), 195);
assert_eq(b.get(1), 169);
```

```fides
pub fn utf8_encode_bytes(cp: i64) -> Vec<i64> {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 767*

---

## utf8_encode_all

Encode a Vec of codepoints as a UTF-8 string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var cps: Vec<i64> = Vec::new();
cps.push(72); cps.push(105);
assert_eq(utf8_encode_all(cps), "Hi");
```

```fides
pub fn utf8_encode_all(cps: Vec<i64>) -> str {
```

**Parameters:**

- `cps`: `Vec<i64>`

**Returns:** `str`

*Defined at line 797*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
