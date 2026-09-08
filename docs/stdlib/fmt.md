# fmt — Fides Documentation

## Contents

- [fmt_int](#fmt_int) (function)
- [fmt_int_width](#fmt_int_width) (function)
- [fmt_int_zero_pad](#fmt_int_zero_pad) (function)
- [fmt_int_signed](#fmt_int_signed) (function)
- [fmt_hex](#fmt_hex) (function)
- [fmt_hex_prefix](#fmt_hex_prefix) (function)
- [fmt_hex_width](#fmt_hex_width) (function)
- [fmt_bin](#fmt_bin) (function)
- [fmt_bin_prefix](#fmt_bin_prefix) (function)
- [fmt_octal](#fmt_octal) (function)
- [fmt_float](#fmt_float) (function)
- [fmt_float_default](#fmt_float_default) (function)
- [fmt_percent](#fmt_percent) (function)
- [fmt_bool](#fmt_bool) (function)
- [fmt_yes_no](#fmt_yes_no) (function)
- [fmt_pad_left](#fmt_pad_left) (function)
- [fmt_pad_right](#fmt_pad_right) (function)
- [fmt_pad_center](#fmt_pad_center) (function)
- [fmt_commas](#fmt_commas) (function)
- [fmt_bytes](#fmt_bytes) (function)
- [fmt_row](#fmt_row) (function)
- [fmt_separator](#fmt_separator) (function)
- [fmt_row_widths](#fmt_row_widths) (function)

## fmt_int

Format an i64 as a decimal string.

```fides
pub fn fmt_int(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 21*

---

## fmt_int_width

Format an i64 with a minimum width, padded with spaces on the left.

```fides
pub fn fmt_int_width(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 26*

---

## fmt_int_zero_pad

Format an i64 with a minimum width, zero-padded on the left.

```fides
pub fn fmt_int_zero_pad(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 31*

---

## fmt_int_signed

Format an i64 as a signed string with a '+' for positive numbers.

```fides
pub fn fmt_int_signed(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 36*

---

## fmt_hex

Format an i64 as a hex string (lowercase).

```fides
pub fn fmt_hex(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 44*

---

## fmt_hex_prefix

Format an i64 as a hex string with "0x" prefix.

```fides
pub fn fmt_hex_prefix(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 49*

---

## fmt_hex_width

Format an i64 as a hex string with minimum width (zero-padded).

```fides
pub fn fmt_hex_width(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 54*

---

## fmt_bin

Format an i64 as a binary string.

```fides
pub fn fmt_bin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 59*

---

## fmt_bin_prefix

Format an i64 as a binary string with "0b" prefix.

```fides
pub fn fmt_bin_prefix(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 64*

---

## fmt_octal

Format an i64 as an octal string.

```fides
pub fn fmt_octal(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 69*

---

## fmt_float

Format an f64 with a specified number of decimal places.
fmt_float(3.14159, 2) → "3.14"

```fides
pub fn fmt_float(f: f64, decimals: i64) -> str {
```

**Parameters:**

- `f`: `f64`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 90*

---

## fmt_float_default

Format an f64 with default 2 decimal places.

```fides
pub fn fmt_float_default(f: f64) -> str {
```

**Parameters:**

- `f`: `f64`

**Returns:** `str`

*Defined at line 112*

---

## fmt_percent

Format an f64 as a percentage with specified decimals.
fmt_percent(0.875, 1) → "87.5%"

```fides
pub fn fmt_percent(f: f64, decimals: i64) -> str {
```

**Parameters:**

- `f`: `f64`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 118*

---

## fmt_bool

Format a bool as "true" or "false".

```fides
pub fn fmt_bool(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 125*

---

## fmt_yes_no

Format a bool as "yes" or "no".

```fides
pub fn fmt_yes_no(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 130*

---

## fmt_pad_left

Pad a string to a minimum width, left-aligned (pad right with spaces).

```fides
pub fn fmt_pad_left(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 138*

---

## fmt_pad_right

Pad a string to a minimum width, left-aligned (pad right with spaces).

```fides
pub fn fmt_pad_right(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 143*

---

## fmt_pad_center

Pad a string to a minimum width, centered.

```fides
pub fn fmt_pad_center(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 148*

---

## fmt_commas

Format an i64 with thousands separators (commas).
fmt_commas(1234567) → "1,234,567"

```fides
pub fn fmt_commas(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 156*

---

## fmt_bytes

Format a byte count as a human-readable size.
fmt_bytes(1536) → "1.5 KB"
fmt_bytes(1048576) → "1.0 MB"

```fides
pub fn fmt_bytes(bytes: i64) -> str {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `str`

*Defined at line 190*

---

## fmt_row

Format a Vec of strings as a row in a table (pipe-separated).
fmt_row(["Name", "Age", "City"]) → "| Name | Age | City |"

```fides
pub fn fmt_row(cols: Vec) -> str {
```

**Parameters:**

- `cols`: `Vec`

**Returns:** `str`

*Defined at line 211*

---

## fmt_separator

Format a separator line for a table with N columns of given widths.
fmt_separator([5, 3, 5]) → "+-----+---+-----+"

```fides
pub fn fmt_separator(widths: Vec) -> str {
```

**Parameters:**

- `widths`: `Vec`

**Returns:** `str`

*Defined at line 225*

---

## fmt_row_widths

Format a table row with specific column widths (left-aligned).

```fides
pub fn fmt_row_widths(cols: Vec, widths: Vec) -> str {
```

**Parameters:**

- `cols`: `Vec`
- `widths`: `Vec`

**Returns:** `str`

*Defined at line 237*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
