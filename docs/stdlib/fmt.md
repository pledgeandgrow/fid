[All modules](index.md)

# fmt -- Fides Documentation

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_int(-42), "-42");
```

```fides
pub fn fmt_int(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 22*

---

## fmt_int_width

Format an i64 with a minimum width, padded with spaces on the left.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_int_width(42, 5), "   42");
```

```fides
pub fn fmt_int_width(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 30*

---

## fmt_int_zero_pad

Format an i64 with a minimum width, zero-padded on the left.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_int_zero_pad(42, 5), "00042");
```

```fides
pub fn fmt_int_zero_pad(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 39*

---

## fmt_int_signed

Format an i64 as a signed string with a '+' for positive numbers.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_int_signed(5), "+5");
assert_eq(fmt_int_signed(-5), "-5");
```

```fides
pub fn fmt_int_signed(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 49*

---

## fmt_hex

Format an i64 as a hex string (lowercase).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_hex(48879), "beef");
```

```fides
pub fn fmt_hex(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 60*

---

## fmt_hex_prefix

Format an i64 as a hex string with "0x" prefix.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_hex_prefix(255), "0xff");
```

```fides
pub fn fmt_hex_prefix(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 68*

---

## fmt_hex_width

Format an i64 as a hex string with minimum width (zero-padded).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_hex_width(10, 4), "000a");
```

```fides
pub fn fmt_hex_width(n: i64, width: i64) -> str {
```

**Parameters:**

- `n`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 76*

---

## fmt_bin

Format an i64 as a binary string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_bin(10), "1010");
```

```fides
pub fn fmt_bin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 84*

---

## fmt_bin_prefix

Format an i64 as a binary string with "0b" prefix.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_bin_prefix(3), "0b11");
```

```fides
pub fn fmt_bin_prefix(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 92*

---

## fmt_octal

Format an i64 as an octal string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_octal(8), "10");
assert_eq(fmt_octal(511), "777");
```

```fides
pub fn fmt_octal(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 101*

---

## fmt_float

Format an f64 with a specified number of decimal places.
fmt_float(3.14159, 2) → "3.14"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = fmt_float(3.14159, 2);
println(s);
assert_eq(s, "3.14");
assert_eq(fmt_float(-1.5, 1), "-1.5");
```

```fides
pub fn fmt_float(f: f64, decimals: i64) -> str {
```

**Parameters:**

- `f`: `f64`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 130*

---

## fmt_float_default

Format an f64 with default 2 decimal places.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_float_default(2.5), "2.50");
```

```fides
pub fn fmt_float_default(f: f64) -> str {
```

**Parameters:**

- `f`: `f64`

**Returns:** `str`

*Defined at line 155*

---

## fmt_percent

Format an f64 as a percentage with specified decimals.
fmt_percent(0.875, 1) → "87.5%"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_percent(0.875, 1), "87.5%");
```

```fides
pub fn fmt_percent(f: f64, decimals: i64) -> str {
```

**Parameters:**

- `f`: `f64`
- `decimals`: `i64`

**Returns:** `str`

*Defined at line 164*

---

## fmt_bool

Format a bool as "true" or "false".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_bool(true), "true");
```

```fides
pub fn fmt_bool(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 174*

---

## fmt_yes_no

Format a bool as "yes" or "no".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_yes_no(false), "no");
```

```fides
pub fn fmt_yes_no(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 182*

---

## fmt_pad_left

Pad a string to a minimum width with spaces on the left (right-aligned).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_pad_left("ab", 4), "  ab");
```

```fides
pub fn fmt_pad_left(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 195*

---

## fmt_pad_right

Pad a string to a minimum width, left-aligned (pad right with spaces).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_pad_right("ab", 4), "ab  ");
```

```fides
pub fn fmt_pad_right(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 203*

---

## fmt_pad_center

Pad a string to a minimum width, centered.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_pad_center("ab", 6), "  ab  ");
```

```fides
pub fn fmt_pad_center(s: str, width: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `width`: `i64`

**Returns:** `str`

*Defined at line 211*

---

## fmt_commas

Format an i64 with thousands separators (commas).
fmt_commas(1234567) → "1,234,567"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_commas(1234567), "1,234,567");
assert_eq(fmt_commas(-1000), "-1,000");
assert_eq(fmt_commas(999), "999");
```

```fides
pub fn fmt_commas(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 224*

---

## fmt_bytes

Format a byte count as a human-readable size.
fmt_bytes(1536) → "1.5 KB"
fmt_bytes(1048576) → "1.0 MB"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(fmt_bytes(1536), "1.5 KB");
assert_eq(fmt_bytes(1048576), "1.0 MB");
assert_eq(fmt_bytes(500), "500.0 B");
```

```fides
pub fn fmt_bytes(bytes: i64) -> str {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `str`

*Defined at line 267*

---

## fmt_row

Format a Vec of strings as a row in a table (pipe-separated).
fmt_row(["Name", "Age", "City"]) → "| Name | Age | City |"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var cols: Vec<str> = Vec::new();
cols.push("Name"); cols.push("Age");
assert_eq(fmt_row(cols), "| Name | Age |");
```

```fides
pub fn fmt_row(cols: Vec<str>) -> str {
```

**Parameters:**

- `cols`: `Vec<str>`

**Returns:** `str`

*Defined at line 293*

---

## fmt_separator

Format a separator line for a table with N columns of given widths.
Each column gets its width plus 2 dashes (the cell padding of fmt_row):
fmt_separator([3, 1]) → "+-----+---+"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var w: Vec<i64> = Vec::new();
w.push(3); w.push(1);
assert_eq(fmt_separator(w), "+-----+---+");
```

```fides
pub fn fmt_separator(widths: Vec<i64>) -> str {
```

**Parameters:**

- `widths`: `Vec<i64>`

**Returns:** `str`

*Defined at line 313*

---

## fmt_row_widths

Format a table row with specific column widths (cells right-aligned,
padded on the left by fmt_pad_left).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var cols: Vec<str> = Vec::new();
cols.push("a"); cols.push("bb");
var w: Vec<i64> = Vec::new();
w.push(3); w.push(3);
var row = fmt_row_widths(cols, w);
println(row);
assert_eq(row, "|   a |  bb |");
```

```fides
pub fn fmt_row_widths(cols: Vec<str>, widths: Vec<i64>) -> str {
```

**Parameters:**

- `cols`: `Vec<str>`
- `widths`: `Vec<i64>`

**Returns:** `str`

*Defined at line 335*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
