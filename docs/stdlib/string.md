[All modules](index.md)

# string -- Fides Documentation

## Contents

- [str_join](#str_join) (function)
- [str_join_fast](#str_join_fast) (function)
- [str_join_ints](#str_join_ints) (function)
- [str_reverse](#str_reverse) (function)
- [str_capitalize](#str_capitalize) (function)
- [str_title](#str_title) (function)
- [str_pad_left](#str_pad_left) (function)
- [str_pad_right](#str_pad_right) (function)
- [str_center](#str_center) (function)
- [str_count](#str_count) (function)
- [str_count_char](#str_count_char) (function)
- [str_find_nth](#str_find_nth) (function)
- [str_is_blank](#str_is_blank) (function)
- [str_is_numeric](#str_is_numeric) (function)
- [str_is_alpha](#str_is_alpha) (function)
- [str_is_alphanumeric](#str_is_alphanumeric) (function)
- [str_strip_char](#str_strip_char) (function)
- [str_remove_char](#str_remove_char) (function)
- [bool_to_str](#bool_to_str) (function)
- [int_to_str_padded](#int_to_str_padded) (function)
- [int_to_hex](#int_to_hex) (function)
- [int_to_bin](#int_to_bin) (function)
- [smallstr_max_inline](#smallstr_max_inline) (function)
- [smallstr_from_str](#smallstr_from_str) (function)
- [smallstr_new](#smallstr_new) (function)
- [smallstr_len](#smallstr_len) (function)
- [smallstr_to_str](#smallstr_to_str) (function)
- [smallstr_is_inline](#smallstr_is_inline) (function)
- [smallstr_eq](#smallstr_eq) (function)
- [rope_empty](#rope_empty) (function)
- [rope_new](#rope_new) (function)
- [rope_push](#rope_push) (function)
- [rope_concat](#rope_concat) (function)
- [rope_append](#rope_append) (function)
- [rope_len](#rope_len) (function)
- [rope_fragments](#rope_fragments) (function)
- [rope_to_str](#rope_to_str) (function)
- [rope_byte_at](#rope_byte_at) (function)
- [rope_substring](#rope_substring) (function)
- [StrView](#StrView) (struct)
- [str_view](#str_view) (function)
- [str_view_all](#str_view_all) (function)
- [StrView](#StrView) (impl)
- [str_parse_int_opt](#str_parse_int_opt) (function)

## str_join

Join a Vec of strings with a separator.
str_join(["a", "b", "c"], "-") → "a-b-c"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var parts: Vec<str> = Vec::new();
parts.push("a"); parts.push("b"); parts.push("c");
var s = str_join(parts, "-");
println(s);
assert_eq(s, "a-b-c");
```

```fides
pub fn str_join(parts: Vec<str>, sep: str) -> str {
```

**Parameters:**

- `parts`: `Vec<str>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 25*

---

## str_join_fast

Efficient join that avoids O(n^2) memory by building in batches.
Concatenates groups of 1000 parts, then concatenates the groups.
This reduces the number of large intermediate strings.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var parts: Vec<str> = Vec::new();
parts.push("x"); parts.push("y");
assert_eq(str_join_fast(parts, ", "), "x, y");
```

```fides
pub fn str_join_fast(parts: Vec<str>, sep: str) -> str {
```

**Parameters:**

- `parts`: `Vec<str>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 63*

---

## str_join_ints

Join a Vec of i64 values with a separator.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var nums: Vec<i64> = Vec::new();
nums.push(1); nums.push(2); nums.push(3);
var s = str_join_ints(nums, "+");
println(s);
assert_eq(s, "1+2+3");
```

```fides
pub fn str_join_ints(nums: Vec<i64>, sep: str) -> str {
```

**Parameters:**

- `nums`: `Vec<i64>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 94*

---

## str_reverse

Reverse a string (byte-level).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_reverse("abc"), "cba");
assert_eq(str_reverse(""), "");
```

```fides
pub fn str_reverse(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 114*

---

## str_capitalize

Capitalize the first character of a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_capitalize("hello"), "Hello");
```

```fides
pub fn str_capitalize(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 132*

---

## str_title

Convert a string to title case (first char of each word uppercase).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = str_title("the quick fox");
println(t);
assert_eq(t, "The Quick Fox");
```

```fides
pub fn str_title(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 156*

---

## str_pad_left

Pad a string on the left with a character to reach the target length.
str_pad_left("42", 5, '0') → "00042"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_pad_left("42", 5, '0'), "00042");
assert_eq(str_pad_left("toolong", 3, ' '), "toolong");
```

```fides
pub fn str_pad_left(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 190*

---

## str_pad_right

Pad a string on the right with a character to reach the target length.
str_pad_right("42", 5, ' ') → "42   "


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_pad_right("42", 5, '.'), "42...");
```

```fides
pub fn str_pad_right(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 204*

---

## str_center

Center a string within a given width, padding with the given character.
str_center("hi", 6, ' ') → "  hi  "


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_center("hi", 6, '*'), "**hi**");
```

```fides
pub fn str_center(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 218*

---

## str_count

Count occurrences of a substring in a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_count("abcabc", "bc"), 2);
assert_eq(str_count("aaa", "x"), 0);
```

```fides
pub fn str_count(s: str, sub: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `i64`

*Defined at line 239*

---

## str_count_char

Count occurrences of a byte (character) in a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_count_char("banana", 'a'), 3);
```

```fides
pub fn str_count_char(s: str, ch: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `i64`

*Defined at line 262*

---

## str_find_nth

Find the nth occurrence of a substring. Returns index or -1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_find_nth("a-b-c", "-", 1), 1);
assert_eq(str_find_nth("a-b-c", "-", 2), 3);
assert_eq(str_find_nth("a-b-c", "-", 3), -1);
```

```fides
pub fn str_find_nth(s: str, sub: str, n: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 280*

---

## str_is_blank

Check if a string is empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(str_is_blank("   "));
assert(!str_is_blank(" x "));
```

```fides
pub fn str_is_blank(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 309*

---

## str_is_numeric

Check if a string consists only of digits.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(str_is_numeric("12345"));
assert(!str_is_numeric("12a"));
```

```fides
pub fn str_is_numeric(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 325*

---

## str_is_alpha

Check if a string consists only of alphabetic characters.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(str_is_alpha("abcXYZ"));
assert(!str_is_alpha("abc1"));
```

```fides
pub fn str_is_alpha(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 344*

---

## str_is_alphanumeric

Check if a string consists only of alphanumeric characters.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(str_is_alphanumeric("abc123"));
assert(!str_is_alphanumeric("a b"));
```

```fides
pub fn str_is_alphanumeric(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 363*

---

## str_strip_char

Strip a specific character from both ends of a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_strip_char("--x--", '-'), "x");
```

```fides
pub fn str_strip_char(s: str, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 384*

---

## str_remove_char

Remove all occurrences of a character from a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(str_remove_char("a,b,c", ','), "abc");
```

```fides
pub fn str_remove_char(s: str, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 401*

---

## bool_to_str

Convert a bool to a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(bool_to_str(true), "true");
assert_eq(bool_to_str(false), "false");
```

```fides
pub fn bool_to_str(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 421*

---

## int_to_str_padded

Convert an i64 to a string with a minimum width (zero-padded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(int_to_str_padded(7, 3), "007");
```

```fides
pub fn int_to_str_padded(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 432*

---

## int_to_hex

Convert an i64 to a hex string (lowercase, no prefix).
Negative values print as their 64-bit two's-complement bit pattern
(int_to_hex(-1) == "ffffffffffffffff"), so i64::MIN formats correctly.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(int_to_hex(255), "ff");
assert_eq(int_to_hex(0), "0");
assert_eq(int_to_hex(-1), "ffffffffffffffff");
assert_eq(int_to_hex(-255), "ffffffffffffff01");
```

```fides
pub fn int_to_hex(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 447*

---

## int_to_bin

Convert an i64 to a binary string (no prefix).
Negative values print as their 64-bit two's-complement bit pattern.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(int_to_bin(5), "101");
assert_eq(int_to_bin(0), "0");
assert_eq(str_len(int_to_bin(-1)), 64);
```

```fides
pub fn int_to_bin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 485*

---

## smallstr_max_inline

Maximum length for inline storage.

```fides
pub fn smallstr_max_inline() -> i64 {
```

**Returns:** `i64`

*Defined at line 545*

---

## smallstr_from_str

Create a SmallStr from a regular string.

```fides
pub fn smallstr_from_str(s: str) -> SmallStr {
```

**Parameters:**

- `s`: `str`

**Returns:** `SmallStr`

*Defined at line 550*

---

## smallstr_new

Alias for smallstr_from_str.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = smallstr_new("hello");
assert_eq(smallstr_len(s), 5);
assert(!smallstr_is_inline(s));
```

```fides
pub fn smallstr_new(s: str) -> SmallStr {
```

**Parameters:**

- `s`: `str`

**Returns:** `SmallStr`

*Defined at line 564*

---

## smallstr_len

Get the length of a SmallStr.

```fides
pub fn smallstr_len(s: SmallStr) -> i64 {
```

**Parameters:**

- `s`: `SmallStr`

**Returns:** `i64`

*Defined at line 569*

---

## smallstr_to_str

Convert a SmallStr to a regular heap string.

```fides
pub fn smallstr_to_str(s: SmallStr) -> str {
```

**Parameters:**

- `s`: `SmallStr`

**Returns:** `str`

*Defined at line 574*

---

## smallstr_is_inline

Check if a SmallStr is stored inline (no heap allocation).

```fides
pub fn smallstr_is_inline(_s: SmallStr) -> bool {
```

**Parameters:**

- `_s`: `SmallStr`

**Returns:** `bool`

*Defined at line 585*

---

## smallstr_eq

Compare two SmallStrs for equality.

```fides
pub fn smallstr_eq(a: SmallStr, b: SmallStr) -> bool {
```

**Parameters:**

- `a`: `SmallStr`
- `b`: `SmallStr`

**Returns:** `bool`

*Defined at line 590*

---

## rope_empty

Create an empty rope.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = rope_empty();
rope_push(&mut r, "a");
assert_eq(rope_to_str(r), "a");
```

```fides
pub fn rope_empty() -> Rope {
```

**Returns:** `Rope`

*Defined at line 630*

---

## rope_new

Create a rope holding one fragment.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = rope_new("Hello");
assert_eq(rope_len(r), 5);
assert_eq(rope_to_str(r), "Hello");
```

```fides
pub fn rope_new(s: str) -> Rope {
```

**Parameters:**

- `s`: `str`

**Returns:** `Rope`

*Defined at line 642*

---

## rope_push

Append a string fragment to the rope. Amortized O(1).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = rope_new("a");
rope_push(&mut r, "b");
rope_push(&mut r, "c");
assert_eq(rope_to_str(r), "abc");
```

```fides
pub fn rope_push(r: &mut Rope, s: str) -> void {
```

**Parameters:**

- `r`: `&mut Rope`
- `s`: `str`

**Returns:** `void`

*Defined at line 657*

---

## rope_concat

Concatenate two ropes: the result has left's fragments then right's.
O(#fragments) — fragment references are copied, never the text.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = rope_concat(rope_new("Hello, "), rope_new("world!"));
assert_eq(rope_to_str(r), "Hello, world!");
```

```fides
pub fn rope_concat(left: Rope, right: Rope) -> Rope {
```

**Parameters:**

- `left`: `Rope`
- `right`: `Rope`

**Returns:** `Rope`

*Defined at line 668*

---

## rope_append

Append all of `other`'s fragments to `r`. O(#fragments of other).

```fides
pub fn rope_append(r: &mut Rope, other: Rope) -> void {
```

**Parameters:**

- `r`: `&mut Rope`
- `other`: `Rope`

**Returns:** `void`

*Defined at line 686*

---

## rope_len

Get the total length of a rope. O(1) — stored in the node.

```fides
pub fn rope_len(r: Rope) -> i64 {
```

**Parameters:**

- `r`: `Rope`

**Returns:** `i64`

*Defined at line 696*

---

## rope_fragments

The number of fragments in the rope.

```fides
pub fn rope_fragments(r: Rope) -> i64 {
```

**Parameters:**

- `r`: `Rope`

**Returns:** `i64`

*Defined at line 701*

---

## rope_to_str

Convert a rope to a regular string. O(total bytes) — builds through a
byte buffer once, so embedded NULs in fragments are preserved.

```fides
pub fn rope_to_str(r: Rope) -> str {
```

**Parameters:**

- `r`: `Rope`

**Returns:** `str`

*Defined at line 707*

---

## rope_byte_at

The byte at position `idx` in the flattened rope (clamped to bounds).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = rope_new("hello");
assert_eq(rope_byte_at(r, 1), 101);
```

```fides
pub fn rope_byte_at(r: Rope, idx: i64) -> i64 {
```

**Parameters:**

- `r`: `Rope`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 728*

---

## rope_substring

Create a rope from a substring of another rope.
This is O(log n) for balanced ropes.

```fides
pub fn rope_substring(r: Rope, start: i64, end: i64) -> Rope {
```

**Parameters:**

- `r`: `Rope`
- `start`: `i64`
- `end`: `i64`

**Returns:** `Rope`

*Defined at line 748*

---

## StrView

```fides
pub struct StrView {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `str` |
| `start` | `i64` |
| `len` | `i64` |

*Defined at line 771*

---

## str_view

The bytes `start..end` of `s` (clamped to the string), not copied.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = str_view("key=value", 4, 9);
assert_eq(v.length(), 5);
assert(v.eq_str("value"));
assert_eq(v.to_str(), "value");
```

```fides
pub fn str_view(s: str, start: i64, end: i64) -> StrView {
```

**Parameters:**

- `s`: `str`
- `start`: `i64`
- `end`: `i64`

**Returns:** `StrView`

*Defined at line 784*

---

## str_view_all

A view of the whole string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = str_view_all("  a=1  ").trim();
var parts: Vec<StrView> = v.split("=");
assert_eq(parts.len(), 2);
assert(parts.get(0).eq_str("a"));
assert_eq(parts.get(1).parse_int(-1), 1);
```

```fides
pub fn str_view_all(s: str) -> StrView {
```

**Parameters:**

- `s`: `str`

**Returns:** `StrView`

*Defined at line 810*

---

## StrView

```fides
impl StrView {
```

**Methods:**

### length

```fides
pub fn length(self: &StrView) -> i64 {
```

### is_empty

```fides
pub fn is_empty(self: &StrView) -> bool {
```

### byte_at

The byte at `i`, or -1 outside the view.

```fides
pub fn byte_at(self: &StrView, i: i64) -> i64 {
```

### slice

The view's bytes `a..b` (clamped), not copied.

```fides
pub fn slice(self: &StrView, a: i64, b: i64) -> StrView {
```

### addr

Address of the first byte (for the comparisons below).

```fides
fn addr(self: &StrView) -> i64 {
```

### eq

```fides
pub fn eq(self: &StrView, other: &StrView) -> bool {
```

### eq_str

```fides
pub fn eq_str(self: &StrView, s: str) -> bool {
```

### starts_with

```fides
pub fn starts_with(self: &StrView, p: str) -> bool {
```

### ends_with

```fides
pub fn ends_with(self: &StrView, p: str) -> bool {
```

### find

Offset of the first `sub` in the view, or -1.

```fides
pub fn find(self: &StrView, sub: str) -> i64 {
```

### contains

```fides
pub fn contains(self: &StrView, sub: str) -> bool {
```

### split

The parts between each `delim`, as views of the same string.

```fides
pub fn split(self: &StrView, delim: str) -> Vec<StrView> {
```

### trim

Without leading and trailing spaces, tabs, CR and LF.

```fides
pub fn trim(self: &StrView) -> StrView {
```

### to_str

A new string with the view's bytes (the one copy).

```fides
pub fn to_str(self: &StrView) -> str {
```

### parse_int

The decimal integer the view holds (an optional `-`), or `fallback`.

```fides
pub fn parse_int(self: &StrView, fallback: i64) -> i64 {
```

*Defined at line 816*

---

## str_parse_int_opt

Parse an i64 with an optional +/- sign, or Option::None when the
string is not a well-formed integer — unlike str_parse_int, which
returns 0 both on failure and for the literal "0".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match str_parse_int_opt("-12") {
  Option::Some(v) => { assert_eq(v, -12); }
  Option::None => { assert(false); }
}
match str_parse_int_opt("x") {
  Option::Some(v) => { assert(false); }
  Option::None => { assert(true); }
}
```

```fides
pub fn str_parse_int_opt(s: str) -> Option<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Option<i64>`

*Defined at line 996*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
