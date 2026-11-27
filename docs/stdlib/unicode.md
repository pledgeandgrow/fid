[All modules](index.md)

# unicode -- Fides Documentation

## Contents

- [uni_to_codepoints](#uni_to_codepoints) (function)
- [uni_encode_cp](#uni_encode_cp) (function)
- [uni_from_codepoints](#uni_from_codepoints) (function)
- [uni_len](#uni_len) (function)
- [uni_lower_cp](#uni_lower_cp) (function)
- [uni_upper_cp](#uni_upper_cp) (function)
- [uni_lower](#uni_lower) (function)
- [uni_upper](#uni_upper) (function)
- [uni_equal_fold](#uni_equal_fold) (function)
- [uni_is_combining](#uni_is_combining) (function)
- [uni_is_alpha](#uni_is_alpha) (function)
- [uni_is_digit](#uni_is_digit) (function)
- [uni_is_space](#uni_is_space) (function)
- [uni_graphemes](#uni_graphemes) (function)
- [uni_grapheme_len](#uni_grapheme_len) (function)
- [uni_normalize_lite](#uni_normalize_lite) (function)
- [uni_nfd](#uni_nfd) (function)
- [uni_nfkd](#uni_nfkd) (function)
- [uni_nfc](#uni_nfc) (function)
- [uni_nfkc](#uni_nfkc) (function)

## uni_to_codepoints

Decode a UTF-8 string into a Vec of codepoints (invalid bytes → U+FFFD).

```fides
pub fn uni_to_codepoints(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 57*

---

## uni_encode_cp

Encode one codepoint as UTF-8.

```fides
pub fn uni_encode_cp(cp: i64) -> str {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `str`

*Defined at line 91*

---

## uni_from_codepoints

Encode a Vec of codepoints into a UTF-8 string.

```fides
pub fn uni_from_codepoints(cps: Vec) -> str {
```

**Parameters:**

- `cps`: `Vec`

**Returns:** `str`

*Defined at line 116*

---

## uni_len

Number of codepoints (not bytes) in a string.

```fides
pub fn uni_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 127*

---

## uni_lower_cp

Lowercase one codepoint (ASCII, Latin-1, Latin Ext-A, Greek, Cyrillic).

```fides
pub fn uni_lower_cp(cp: i64) -> i64 {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `i64`

*Defined at line 150*

---

## uni_upper_cp

Uppercase one codepoint (mirrors uni_lower_cp coverage).

```fides
pub fn uni_upper_cp(cp: i64) -> i64 {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `i64`

*Defined at line 211*

---

## uni_lower

Unicode-aware lowercase.

```fides
pub fn uni_lower(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 271*

---

## uni_upper

Unicode-aware uppercase.

```fides
pub fn uni_upper(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 282*

---

## uni_equal_fold

Case-folded comparison (lowercase both sides first).

```fides
pub fn uni_equal_fold(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 293*

---

## uni_is_combining

True for a codepoint in a combining-mark range (Mn/Mc approx.).

```fides
pub fn uni_is_combining(cp: i64) -> bool {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `bool`

*Defined at line 304*

---

## uni_is_alpha

True for a Unicode letter (major scripts; not exhaustive).

```fides
pub fn uni_is_alpha(cp: i64) -> bool {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `bool`

*Defined at line 317*

---

## uni_is_digit

True for a decimal digit (ASCII + common digit ranges).

```fides
pub fn uni_is_digit(cp: i64) -> bool {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `bool`

*Defined at line 332*

---

## uni_is_space

True for whitespace per Unicode White_Space (common members).

```fides
pub fn uni_is_space(cp: i64) -> bool {
```

**Parameters:**

- `cp`: `i64`

**Returns:** `bool`

*Defined at line 341*

---

## uni_graphemes

Split a string into grapheme clusters (Vec of str).
Approximation: handles combining marks, VS16 emoji, ZWJ chains,
and flag pairs. Does not handle all GB9/extended-pictographic rules.

```fides
pub fn uni_graphemes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 369*

---

## uni_grapheme_len

Number of grapheme clusters (user-perceived characters).

```fides
pub fn uni_grapheme_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 415*

---

## uni_normalize_lite

Compose base+combining-mark pairs for Latin (subset of NFC).

```fides
pub fn uni_normalize_lite(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 614*

---

## uni_nfd

Canonical decomposition (NFD).

*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
var v = uni_to_codepoints(uni_nfd("\u{00E9}"));
// [101, 769] — e followed by combining acute
println(int_to_str(v.len()));
```

```fides
pub fn uni_nfd(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 750*

---

## uni_nfkd

Compatibility decomposition (NFKD).

*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
var v = uni_to_codepoints(uni_nfkd("\u{00BD}"));
// [49, 8260, 50] — "1⁄2" (one, fraction slash, two)
println(int_to_str(v.len()));
```

```fides
pub fn uni_nfkd(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 766*

---

## uni_nfc

Canonical composition (NFC) — NFD then canonical recomposition.

*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
if str_equal(uni_nfc("e\u{0301}"), "\u{00E9}") {
  println("composed");
}
```

```fides
pub fn uni_nfc(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 782*

---

## uni_nfkc

Compatibility composition (NFKC) — NFKD then canonical recomposition.

*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
if str_equal(uni_nfkc("\u{339F}"), "mm") {
  println("squared mm folded");
}
```

```fides
pub fn uni_nfkc(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 798*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
