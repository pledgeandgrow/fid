[All modules](index.md)

# gcode -- Fides Documentation

## Contents

- [GLine](#GLine) (struct)
- [gcode_checksum](#gcode_checksum) (function)
- [gcode_with_checksum](#gcode_with_checksum) (function)
- [gcode_parse_line](#gcode_parse_line) (function)
- [gline_has](#gline_has) (function)
- [gline_get](#gline_get) (function)
- [gline_has_code](#gline_has_code) (function)
- [GSeq](#GSeq) (struct)
- [gcode_seq_new](#gcode_seq_new) (function)
- [gcode_seq_check](#gcode_seq_check) (function)
- [GMove](#GMove) (struct)
- [GState](#GState) (struct)
- [gcode_state_new](#gcode_state_new) (function)
- [gcode_run_line](#gcode_run_line) (function)
- [gcode_exec](#gcode_exec) (function)
- [gcode_run](#gcode_run) (function)
- [gcode_num](#gcode_num) (function)
- [gcode_move_str](#gcode_move_str) (function)
- [gcode_polyline](#gcode_polyline) (function)
- [gcode_rounded_rect](#gcode_rounded_rect) (function)
- [gcode_number_lines](#gcode_number_lines) (function)
- [gcode_path_length](#gcode_path_length) (function)

## GLine

One parsed line. `letters` holds the upper-case letter codes ('G' = 71)
in order and `values` their numbers.

```fides
pub struct GLine {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `line_no` | `i64` |
| `has_checksum` | `bool` |
| `checksum` | `i64` |
| `letters` | `Vec<i64>` |
| `values` | `Vec<f64>` |
| `comment` | `str` |

*Defined at line 44*

---

## gcode_checksum

XOR of the bytes of s (the RepRap checksum of what precedes the `*`).

```fides
pub fn gcode_checksum(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 56*

---

## gcode_with_checksum

"N<n> <cmd>*<checksum>", the form a host sends to Marlin/RepRap firmware.

```fides
pub fn gcode_with_checksum(n: i64, cmd: str) -> str {
```

**Parameters:**

- `n`: `i64`
- `cmd`: `str`

**Returns:** `str`

*Defined at line 67*

---

## gcode_parse_line

Parse one line of G-code.

```fides
pub fn gcode_parse_line(text: str) -> GLine {
```

**Parameters:**

- `text`: `str`

**Returns:** `GLine`

*Defined at line 87*

---

## gline_has

Whether the line has a word with this letter ("G" etc.).

```fides
pub fn gline_has(l: &GLine, letter: str) -> bool {
```

**Parameters:**

- `l`: `&GLine`
- `letter`: `str`

**Returns:** `bool`

*Defined at line 192*

---

## gline_get

The value of the first word with this letter, or `def`.

```fides
pub fn gline_get(l: &GLine, letter: str, def: f64) -> f64 {
```

**Parameters:**

- `l`: `&GLine`
- `letter`: `str`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 209*

---

## gline_has_code

Whether the line holds the code letter+number (e.g. "G", 91; "M", 83).

```fides
pub fn gline_has_code(l: &GLine, letter: str, num: f64) -> bool {
```

**Parameters:**

- `l`: `&GLine`
- `letter`: `str`
- `num`: `f64`

**Returns:** `bool`

*Defined at line 218*

---

## GSeq

The host-side line-number rule: tracks the expected next N.

```fides
pub struct GSeq {
```

**Fields:**

| Name | Type |
|------|------|
| `expected` | `i64` |

*Defined at line 231*

---

## gcode_seq_new

```fides
pub fn gcode_seq_new() -> GSeq {
```

**Returns:** `GSeq`

*Defined at line 235*

---

## gcode_seq_check

Check a parsed line's number: "" when it is the next one (or the line
has none and no checksum), else the error (Marlin answers "Resend: n").
M110 resets the counter to its N (the next line is N + 1).

```fides
pub fn gcode_seq_check(s: &mut GSeq, l: &GLine) -> str {
```

**Parameters:**

- `s`: `&mut GSeq`
- `l`: `&GLine`

**Returns:** `str`

*Defined at line 244*

---

## GMove

One linear move to an absolute machine position (mm), with its feed
(mm/min) and whether it is a rapid (G0).

```fides
pub struct GMove {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |
| `e` | `f64` |
| `f` | `f64` |
| `rapid` | `bool` |

*Defined at line 263*

---

## GState

```fides
pub struct GState {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |
| `e` | `f64` |
| `feed` | `f64` |
| `absolute` | `bool` |
| `e_absolute` | `bool` |
| `unit` | `f64` |
| `motion` | `i64` |
| `plane` | `i64` |
| `seg_mm` | `f64` |
| `error` | `str` |

*Defined at line 272*

---

## gcode_state_new

```fides
pub fn gcode_state_new() -> GState {
```

**Returns:** `GState`

*Defined at line 290*

---

## gcode_run_line

Parse and execute one line; the moves it makes (empty on error, with
st.error set).

```fides
pub fn gcode_run_line(st: &mut GState, text: str) -> Vec<GMove> {
```

**Parameters:**

- `st`: `&mut GState`
- `text`: `str`

**Returns:** `Vec<GMove>`

*Defined at line 298*

---

## gcode_exec

Execute a parsed line.

```fides
pub fn gcode_exec(st: &mut GState, l: &GLine) -> Vec<GMove> {
```

**Parameters:**

- `st`: `&mut GState`
- `l`: `&GLine`

**Returns:** `Vec<GMove>`

*Defined at line 319*

---

## gcode_run

Run a whole program (lines separated by newlines); stops at the first
error, which is left in st.error with the line number (1-based) before it.

```fides
pub fn gcode_run(st: &mut GState, program: str) -> Vec<GMove> {
```

**Parameters:**

- `st`: `&mut GState`
- `program`: `str`

**Returns:** `Vec<GMove>`

*Defined at line 580*

---

## gcode_num

A number in G-code style: rounded to 4 decimals, trailing zeros and a
trailing point removed ("10", "-2.5", "0.125").

```fides
pub fn gcode_num(x: f64) -> str {
```

**Parameters:**

- `x`: `f64`

**Returns:** `str`

*Defined at line 604*

---

## gcode_move_str

"G1 X.. Y.. F.." for a move (F only when feed > 0).

```fides
pub fn gcode_move_str(rapid: bool, x: f64, y: f64, feed: f64) -> str {
```

**Parameters:**

- `rapid`: `bool`
- `x`: `f64`
- `y`: `f64`
- `feed`: `f64`

**Returns:** `str`

*Defined at line 623*

---

## gcode_polyline

A program that follows a polyline (xs, ys) at depth z: a rapid to the
first point at the safe height, plunge, cut along the points, retract.

```fides
pub fn gcode_polyline(xs: &Vec<f64>, ys: &Vec<f64>, z: f64, safe_z: f64, feed: f64) -> Vec<str> {
```

**Parameters:**

- `xs`: `&Vec<f64>`
- `ys`: `&Vec<f64>`
- `z`: `f64`
- `safe_z`: `f64`
- `feed`: `f64`

**Returns:** `Vec<str>`

*Defined at line 637*

---

## gcode_rounded_rect

A rectangle with rounded corners of radius r (r > 0: G3 arcs, counter-
clockwise) from (x0, y0) to (x1, y1) at depth z.

```fides
pub fn gcode_rounded_rect(x0: f64, y0: f64, x1: f64, y1: f64, r: f64, z: f64, safe_z: f64, feed: f64) -> Vec<str> {
```

**Parameters:**

- `x0`: `f64`
- `y0`: `f64`
- `x1`: `f64`
- `y1`: `f64`
- `r`: `f64`
- `z`: `f64`
- `safe_z`: `f64`
- `feed`: `f64`

**Returns:** `Vec<str>`

*Defined at line 658*

---

## gcode_number_lines

Number and checksum every line, starting at N1 after an "M110 N0".

```fides
pub fn gcode_number_lines(lines: &Vec<str>) -> Vec<str> {
```

**Parameters:**

- `lines`: `&Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 679*

---

## gcode_path_length

The total path length of moves starting from (x, y, z).

```fides
pub fn gcode_path_length(moves: &Vec<GMove>, x: f64, y: f64, z: f64) -> f64 {
```

**Parameters:**

- `moves`: `&Vec<GMove>`
- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `f64`

*Defined at line 691*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
