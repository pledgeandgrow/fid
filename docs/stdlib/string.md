# string — Fides Documentation

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

## str_join

Join a Vec of strings with a separator.
str_join(["a", "b", "c"], "-") → "a-b-c"

```fides
pub fn str_join(parts: Vec, sep: str) -> str {
```

**Parameters:**

- `parts`: `Vec`
- `sep`: `str`

**Returns:** `str`

*Defined at line 20*

---

## str_join_fast

Efficient join that avoids O(n^2) memory by building in batches.
Concatenates groups of 1000 parts, then concatenates the groups.
This reduces the number of large intermediate strings.

```fides
pub fn str_join_fast(parts: Vec, sep: str) -> str {
```

**Parameters:**

- `parts`: `Vec`
- `sep`: `str`

**Returns:** `str`

*Defined at line 36*

---

## str_join_ints

Join a Vec of i64 values with a separator.

```fides
pub fn str_join_ints(nums: Vec, sep: str) -> str {
```

**Parameters:**

- `nums`: `Vec`
- `sep`: `str`

**Returns:** `str`

*Defined at line 56*

---

## str_reverse

Reverse a string (byte-level).

```fides
pub fn str_reverse(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 71*

---

## str_capitalize

Capitalize the first character of a string.

```fides
pub fn str_capitalize(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 84*

---

## str_title

Convert a string to title case (first char of each word uppercase).

```fides
pub fn str_title(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 101*

---

## str_pad_left

Pad a string on the left with a character to reach the target length.
str_pad_left("42", 5, '0') → "00042"

```fides
pub fn str_pad_left(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 129*

---

## str_pad_right

Pad a string on the right with a character to reach the target length.
str_pad_right("42", 5, ' ') → "42   "

```fides
pub fn str_pad_right(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 138*

---

## str_center

Center a string within a given width, padding with the given character.
str_center("hi", 6, ' ') → "  hi  "

```fides
pub fn str_center(s: str, width: i64, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 147*

---

## str_count

Count occurrences of a substring in a string.

```fides
pub fn str_count(s: str, sub: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `i64`

*Defined at line 162*

---

## str_count_char

Count occurrences of a byte (character) in a string.

```fides
pub fn str_count_char(s: str, ch: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `i64`

*Defined at line 180*

---

## str_find_nth

Find the nth occurrence of a substring. Returns index or -1.

```fides
pub fn str_find_nth(s: str, sub: str, n: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 193*

---

## str_is_blank

Check if a string is empty.

```fides
pub fn str_is_blank(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 214*

---

## str_is_numeric

Check if a string consists only of digits.

```fides
pub fn str_is_numeric(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 226*

---

## str_is_alpha

Check if a string consists only of alphabetic characters.

```fides
pub fn str_is_alpha(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 239*

---

## str_is_alphanumeric

Check if a string consists only of alphanumeric characters.

```fides
pub fn str_is_alphanumeric(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 252*

---

## str_strip_char

Strip a specific character from both ends of a string.

```fides
pub fn str_strip_char(s: str, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 268*

---

## str_remove_char

Remove all occurrences of a character from a string.

```fides
pub fn str_remove_char(s: str, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 282*

---

## bool_to_str

Convert a bool to a string.

```fides
pub fn bool_to_str(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 298*

---

## int_to_str_padded

Convert an i64 to a string with a minimum width (zero-padded).

```fides
pub fn int_to_str_padded(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 304*

---

## int_to_hex

Convert an i64 to a hex string (lowercase, no prefix).

```fides
pub fn int_to_hex(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 310*

---

## int_to_bin

Convert an i64 to a binary string (no prefix).

```fides
pub fn int_to_bin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 330*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
