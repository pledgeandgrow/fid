[All modules](index.md)

# term -- Fides Documentation

## Contents

- [TERM_ESC:](#TERM_ESC:) (const)
- [term_reset](#term_reset) (function)
- [term_bold](#term_bold) (function)
- [term_dim](#term_dim) (function)
- [term_italic](#term_italic) (function)
- [term_underline](#term_underline) (function)
- [term_blink](#term_blink) (function)
- [term_inverse](#term_inverse) (function)
- [term_strikethrough](#term_strikethrough) (function)
- [term_black](#term_black) (function)
- [term_red](#term_red) (function)
- [term_green](#term_green) (function)
- [term_yellow](#term_yellow) (function)
- [term_blue](#term_blue) (function)
- [term_magenta](#term_magenta) (function)
- [term_cyan](#term_cyan) (function)
- [term_white](#term_white) (function)
- [term_gray](#term_gray) (function)
- [term_bright_red](#term_bright_red) (function)
- [term_bright_green](#term_bright_green) (function)
- [term_bright_yellow](#term_bright_yellow) (function)
- [term_bright_blue](#term_bright_blue) (function)
- [term_bright_magenta](#term_bright_magenta) (function)
- [term_bright_cyan](#term_bright_cyan) (function)
- [term_bright_white](#term_bright_white) (function)
- [term_bg_red](#term_bg_red) (function)
- [term_bg_green](#term_bg_green) (function)
- [term_bg_yellow](#term_bg_yellow) (function)
- [term_bg_blue](#term_bg_blue) (function)
- [term_bg_magenta](#term_bg_magenta) (function)
- [term_bg_cyan](#term_bg_cyan) (function)
- [term_bg_white](#term_bg_white) (function)
- [term_fg256](#term_fg256) (function)
- [term_bg256](#term_bg256) (function)
- [term_rgb](#term_rgb) (function)
- [term_bg_rgb](#term_bg_rgb) (function)
- [term_up](#term_up) (function)
- [term_down](#term_down) (function)
- [term_forward](#term_forward) (function)
- [term_back](#term_back) (function)
- [term_col](#term_col) (function)
- [term_goto](#term_goto) (function)
- [term_home](#term_home) (function)
- [term_save_cursor](#term_save_cursor) (function)
- [term_restore_cursor](#term_restore_cursor) (function)
- [term_hide_cursor](#term_hide_cursor) (function)
- [term_show_cursor](#term_show_cursor) (function)
- [term_clear_code](#term_clear_code) (function)
- [term_clear_line_code](#term_clear_line_code) (function)
- [term_clear_to_end_code](#term_clear_to_end_code) (function)
- [term_clear_line_to_end_code](#term_clear_line_to_end_code) (function)
- [term_clear](#term_clear) (function)
- [term_clear_line](#term_clear_line) (function)
- [term_progress_bar](#term_progress_bar) (function)
- [term_progress](#term_progress) (function)
- [term_progress_done](#term_progress_done) (function)
- [term_strip](#term_strip) (function)
- [term_width](#term_width) (function)
- [term_supports_ansi](#term_supports_ansi) (function)

## TERM_ESC:

```fides
pub const TERM_ESC: str = "\x1b";
```

*Defined at line 19*

---

## term_reset

```fides
pub fn term_reset() -> str {
```

**Returns:** `str`

*Defined at line 26*

---

## term_bold

```fides
pub fn term_bold(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 29*

---

## term_dim

```fides
pub fn term_dim(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 32*

---

## term_italic

```fides
pub fn term_italic(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 35*

---

## term_underline

```fides
pub fn term_underline(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 38*

---

## term_blink

```fides
pub fn term_blink(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 41*

---

## term_inverse

```fides
pub fn term_inverse(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 44*

---

## term_strikethrough

```fides
pub fn term_strikethrough(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 47*

---

## term_black

```fides
pub fn term_black(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 52*

---

## term_red

```fides
pub fn term_red(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 55*

---

## term_green

```fides
pub fn term_green(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 58*

---

## term_yellow

```fides
pub fn term_yellow(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 61*

---

## term_blue

```fides
pub fn term_blue(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 64*

---

## term_magenta

```fides
pub fn term_magenta(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 67*

---

## term_cyan

```fides
pub fn term_cyan(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 70*

---

## term_white

```fides
pub fn term_white(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 73*

---

## term_gray

```fides
pub fn term_gray(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 76*

---

## term_bright_red

```fides
pub fn term_bright_red(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 79*

---

## term_bright_green

```fides
pub fn term_bright_green(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 82*

---

## term_bright_yellow

```fides
pub fn term_bright_yellow(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 85*

---

## term_bright_blue

```fides
pub fn term_bright_blue(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 88*

---

## term_bright_magenta

```fides
pub fn term_bright_magenta(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 91*

---

## term_bright_cyan

```fides
pub fn term_bright_cyan(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 94*

---

## term_bright_white

```fides
pub fn term_bright_white(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 97*

---

## term_bg_red

```fides
pub fn term_bg_red(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 102*

---

## term_bg_green

```fides
pub fn term_bg_green(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 105*

---

## term_bg_yellow

```fides
pub fn term_bg_yellow(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 108*

---

## term_bg_blue

```fides
pub fn term_bg_blue(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 111*

---

## term_bg_magenta

```fides
pub fn term_bg_magenta(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 114*

---

## term_bg_cyan

```fides
pub fn term_bg_cyan(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 117*

---

## term_bg_white

```fides
pub fn term_bg_white(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 120*

---

## term_fg256

256-color foreground (0-255).

```fides
pub fn term_fg256(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 125*

---

## term_bg256

256-color background (0-255).

```fides
pub fn term_bg256(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 130*

---

## term_rgb

True-color foreground.

```fides
pub fn term_rgb(s: str, r: i64, g: i64, b: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`

**Returns:** `str`

*Defined at line 135*

---

## term_bg_rgb

True-color background.

```fides
pub fn term_bg_rgb(s: str, r: i64, g: i64, b: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`

**Returns:** `str`

*Defined at line 140*

---

## term_up

```fides
pub fn term_up(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 146*

---

## term_down

```fides
pub fn term_down(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 149*

---

## term_forward

```fides
pub fn term_forward(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 152*

---

## term_back

```fides
pub fn term_back(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 155*

---

## term_col

```fides
pub fn term_col(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 158*

---

## term_goto

```fides
pub fn term_goto(row: i64, col: i64) -> str {
```

**Parameters:**

- `row`: `i64`
- `col`: `i64`

**Returns:** `str`

*Defined at line 161*

---

## term_home

```fides
pub fn term_home() -> str {
```

**Returns:** `str`

*Defined at line 164*

---

## term_save_cursor

```fides
pub fn term_save_cursor() -> str {
```

**Returns:** `str`

*Defined at line 167*

---

## term_restore_cursor

```fides
pub fn term_restore_cursor() -> str {
```

**Returns:** `str`

*Defined at line 170*

---

## term_hide_cursor

```fides
pub fn term_hide_cursor() -> str {
```

**Returns:** `str`

*Defined at line 173*

---

## term_show_cursor

```fides
pub fn term_show_cursor() -> str {
```

**Returns:** `str`

*Defined at line 176*

---

## term_clear_code

```fides
pub fn term_clear_code() -> str {
```

**Returns:** `str`

*Defined at line 182*

---

## term_clear_line_code

```fides
pub fn term_clear_line_code() -> str {
```

**Returns:** `str`

*Defined at line 185*

---

## term_clear_to_end_code

```fides
pub fn term_clear_to_end_code() -> str {
```

**Returns:** `str`

*Defined at line 188*

---

## term_clear_line_to_end_code

```fides
pub fn term_clear_line_to_end_code() -> str {
```

**Returns:** `str`

*Defined at line 191*

---

## term_clear

```fides
pub fn term_clear() -> void {
```

**Returns:** `void`

*Defined at line 195*

---

## term_clear_line

```fides
pub fn term_clear_line() -> void {
```

**Returns:** `void`

*Defined at line 198*

---

## term_progress_bar

Render a progress bar string of `width` cells: [████░░░░] 40%.

```fides
pub fn term_progress_bar(done: i64, total: i64, width: i64) -> str {
```

**Parameters:**

- `done`: `i64`
- `total`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 205*

---

## term_progress

Print a carriage-returned progress bar (overwrites the current line).

```fides
pub fn term_progress(done: i64, total: i64) -> void {
```

**Parameters:**

- `done`: `i64`
- `total`: `i64`

**Returns:** `void`

*Defined at line 239*

---

## term_progress_done

End a progress line (newline).

```fides
pub fn term_progress_done() -> void {
```

**Returns:** `void`

*Defined at line 244*

---

## term_strip

Strip ANSI escape sequences from `s` (for width calculations/logs).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val s = term_bold(term_red("hi"));
assert(str_len(s) > 2);
assert_eq(term_strip(s), "hi");
assert_eq(term_width(term_green("abc")), 3);
assert_eq(term_strip(term_goto(3, 4) + "x"), "x");
assert(str_contains(term_progress_bar(1, 2, 10), "50%"));
```

```fides
pub fn term_strip(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 257*

---

## term_width

Visible width of a possibly-colored string.

```fides
pub fn term_width(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 288*

---

## term_supports_ansi

True when stdout is likely a terminal with ANSI support:
TERM is set (and not "dumb"), or we're on modern Windows
(WT_SESSION / ANSICON / NO_COLOR absent heuristics).

```fides
pub fn term_supports_ansi() -> bool {
```

**Returns:** `bool`

*Defined at line 295*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
