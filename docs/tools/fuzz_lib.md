[All modules](index.md)

# fuzz_lib -- Fides Documentation

## Contents

- [fl_args](#fl_args) (function)
- [fl_env](#fl_env) (function)
- [fl_env_has](#fl_env_has) (function)
- [fl_opt](#fl_opt) (function)
- [fl_flag](#fl_flag) (function)
- [fl_positionals](#fl_positionals) (function)
- [fl_int](#fl_int) (function)
- [fl_is_int](#fl_is_int) (function)
- [fl_win](#fl_win) (function)
- [fl_sep](#fl_sep) (function)
- [fl_isabs](#fl_isabs) (function)
- [fl_join](#fl_join) (function)
- [fl_cwd](#fl_cwd) (function)
- [fl_normpath](#fl_normpath) (function)
- [fl_abspath](#fl_abspath) (function)
- [fl_stem](#fl_stem) (function)
- [fl_basename](#fl_basename) (function)
- [fl_exists](#fl_exists) (function)
- [fl_remove](#fl_remove) (function)
- [fl_mkdirs](#fl_mkdirs) (function)
- [fl_replace](#fl_replace) (function)
- [fl_write](#fl_write) (function)
- [fl_write_textmode](#fl_write_textmode) (function)
- [fl_append_textmode](#fl_append_textmode) (function)
- [fl_read](#fl_read) (function)
- [fl_bytes_str](#fl_bytes_str) (function)
- [fl_decode](#fl_decode) (function)
- [fl_encode](#fl_encode) (function)
- [fl_encode_all](#fl_encode_all) (function)
- [fl_cps](#fl_cps) (function)
- [fl_decode_str](#fl_decode_str) (function)
- [fl_text](#fl_text) (function)
- [fl_no_cr](#fl_no_cr) (function)
- [fl_lines](#fl_lines) (function)
- [fl_join_lines](#fl_join_lines) (function)
- [fl_count_nl](#fl_count_nl) (function)
- [fl_tail](#fl_tail) (function)
- [fl_tail_cps](#fl_tail_cps) (function)
- [fl_head_cps](#fl_head_cps) (function)
- [fl_hex](#fl_hex) (function)
- [fl_hex0x](#fl_hex0x) (function)
- [fl_repr](#fl_repr) (function)
- [fl_lt](#fl_lt) (function)
- [fl_sort](#fl_sort) (function)
- [fl_contains](#fl_contains) (function)
- [fl_index](#fl_index) (function)
- [fl_print](#fl_print) (function)
- [fl_prog](#fl_prog) (function)
- [fl_code](#fl_code) (function)
- [FlRun](#FlRun) (struct)
- [fl_setdefault_env](#fl_setdefault_env) (function)
- [fl_run_env](#fl_run_env) (function)
- [fl_run_env_ms](#fl_run_env_ms) (function)
- [fl_run](#fl_run) (function)
- [fl_as_text](#fl_as_text) (function)
- [fl_v1](#fl_v1) (function)
- [fl_v2](#fl_v2) (function)
- [fl_v3](#fl_v3) (function)
- [fl_v5](#fl_v5) (function)
- [fl_word](#fl_word) (function)
- [fl_digit](#fl_digit) (function)

## fl_args

The arguments after the program (`fid run tools/x.fd a b` passes "run"
and the path first: those are left out).

```fides
pub fn fl_args() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 31*

---

## fl_env

os.environ.get(name, "")

```fides
pub fn fl_env(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 45*

---

## fl_env_has

```fides
pub fn fl_env_has(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 53*

---

## fl_opt

argparse's value of option `name` ("--name v", "--name=v", and for a
one-letter option "-nV"); `def` when it is not given. The last one
given wins, as in argparse.

```fides
pub fn fl_opt(av: &Vec<str>, name: str, def: str) -> str {
```

**Parameters:**

- `av`: `&Vec<str>`
- `name`: `str`
- `def`: `str`

**Returns:** `str`

*Defined at line 64*

---

## fl_flag

Is flag `name` on the command line?

```fides
pub fn fl_flag(av: &Vec<str>, name: str) -> bool {
```

**Parameters:**

- `av`: `&Vec<str>`
- `name`: `str`

**Returns:** `bool`

*Defined at line 85*

---

## fl_positionals

The arguments that are not options: `with_value` lists the options
that take a value (their value is skipped too).

```fides
pub fn fl_positionals(av: &Vec<str>, with_value: &Vec<str>) -> Vec<str> {
```

**Parameters:**

- `av`: `&Vec<str>`
- `with_value`: `&Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 96*

---

## fl_int

int(s) for the decimal (optionally signed, blank-padded) text argparse
takes; `def` when it is not a number.

```fides
pub fn fl_int(s: str, def: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 119*

---

## fl_is_int

```fides
pub fn fl_is_int(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 141*

---

## fl_win

```fides
pub fn fl_win() -> bool {
```

**Returns:** `bool`

*Defined at line 147*

---

## fl_sep

```fides
pub fn fl_sep() -> str {
```

**Returns:** `str`

*Defined at line 151*

---

## fl_isabs

```fides
pub fn fl_isabs(p: str) -> bool {
```

**Parameters:**

- `p`: `str`

**Returns:** `bool`

*Defined at line 165*

---

## fl_join

os.path.join(a, b)

```fides
pub fn fl_join(a: str, b: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `str`

*Defined at line 172*

---

## fl_cwd

os.getcwd()

```fides
pub fn fl_cwd() -> str {
```

**Returns:** `str`

*Defined at line 182*

---

## fl_normpath

os.path.normpath

```fides
pub fn fl_normpath(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 220*

---

## fl_abspath

os.path.abspath

```fides
pub fn fl_abspath(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 255*

---

## fl_stem

os.path.splitext(p)[0]

```fides
pub fn fl_stem(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 261*

---

## fl_basename

os.path.basename

```fides
pub fn fl_basename(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 279*

---

## fl_exists

```fides
pub fn fl_exists(p: str) -> bool {
```

**Parameters:**

- `p`: `str`

**Returns:** `bool`

*Defined at line 288*

---

## fl_remove

os.remove (no error when it is not there)

```fides
pub fn fl_remove(p: str) {
```

**Parameters:**

- `p`: `str`

*Defined at line 293*

---

## fl_mkdirs

os.makedirs(p, exist_ok=True). Not fs_mkdir_all: it tries to make
"C:" of "C:/x/y" and gives up (fs_is_dir("C:") is false).

```fides
pub fn fl_mkdirs(p: str) {
```

**Parameters:**

- `p`: `str`

*Defined at line 299*

---

## fl_replace

os.replace(a, b)

```fides
pub fn fl_replace(a: str, b: str) {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

*Defined at line 319*

---

## fl_write

Write `text` (UTF-8, "\n" kept: Python's newline="\n").

```fides
pub fn fl_write(path: str, text: str) {
```

**Parameters:**

- `path`: `str`
- `text`: `str`

*Defined at line 327*

---

## fl_write_textmode

Python's open(path, "w") with text-mode newlines: "\n" is "\r\n" on
Windows.

```fides
pub fn fl_write_textmode(path: str, text: str) {
```

**Parameters:**

- `path`: `str`
- `text`: `str`

*Defined at line 349*

---

## fl_append_textmode

Append `text` in text mode.

```fides
pub fn fl_append_textmode(path: str, text: str) {
```

**Parameters:**

- `path`: `str`
- `text`: `str`

*Defined at line 358*

---

## fl_read

The raw bytes of a file as a str (C0 80 for a 0 byte); "" when none.

```fides
pub fn fl_read(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 373*

---

## fl_bytes_str

Bytes as a str, each 0 as C0 80.

```fides
pub fn fl_bytes_str(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 379*

---

## fl_decode

bytes.decode("utf-8", "replace"): code points, each invalid sequence
(its maximal valid prefix) as U+FFFD.

```fides
pub fn fl_decode(b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 399*

---

## fl_encode

Code points [from, to) as UTF-8 text (U+0000 as C0 80; a lone
surrogate as its three bytes, which "surrogatepass" writes too).

```fides
pub fn fl_encode(cps: &Vec<i64>, from: i64, to: i64) -> str {
```

**Parameters:**

- `cps`: `&Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `str`

*Defined at line 466*

---

## fl_encode_all

```fides
pub fn fl_encode_all(cps: &Vec<i64>) -> str {
```

**Parameters:**

- `cps`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 494*

---

## fl_cps

The code points of text made by fl_encode (or of valid UTF-8).

```fides
pub fn fl_cps(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 499*

---

## fl_decode_str

Bytes decoded as Python's errors="replace" does, as a str.

```fides
pub fn fl_decode_str(raw: str) -> str {
```

**Parameters:**

- `raw`: `str`

**Returns:** `str`

*Defined at line 523*

---

## fl_text

Text mode's newlines: "\r\n" and "\r" are "\n".

```fides
pub fn fl_text(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 529*

---

## fl_no_cr

s.replace("\r", "")

```fides
pub fn fl_no_cr(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 534*

---

## fl_lines

s.split("\n") (keeps empty pieces).

```fides
pub fn fl_lines(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 539*

---

## fl_join_lines

"\n".join(v)

```fides
pub fn fl_join_lines(v: &Vec<str>, sep: str) -> str {
```

**Parameters:**

- `v`: `&Vec<str>`
- `sep`: `str`

**Returns:** `str`

*Defined at line 544*

---

## fl_count_nl

The number of times "\n" is in s.

```fides
pub fn fl_count_nl(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 556*

---

## fl_tail

The last `n` bytes of s (Python's s[-n:] on ASCII text).

```fides
pub fn fl_tail(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 567*

---

## fl_tail_cps

s[-n:] by code point.

```fides
pub fn fl_tail_cps(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 573*

---

## fl_head_cps

s[:n] by code point.

```fides
pub fn fl_head_cps(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 580*

---

## fl_hex

```fides
pub fn fl_hex(c: i64, width: i64) -> str {
```

**Parameters:**

- `c`: `i64`
- `width`: `i64`

**Returns:** `str`

*Defined at line 597*

---

## fl_hex0x

Python's hex(n) for n >= 0.

```fides
pub fn fl_hex0x(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 609*

---

## fl_repr

Python's repr() of a str.

```fides
pub fn fl_repr(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 621*

---

## fl_lt

Byte order (Python's code point order for UTF-8 text).

```fides
pub fn fl_lt(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 667*

---

## fl_sort

sorted() of strings.

```fides
pub fn fl_sort(v: &mut Vec<str>) {
```

**Parameters:**

- `v`: `&mut Vec<str>`

*Defined at line 681*

---

## fl_contains

```fides
pub fn fl_contains(v: &Vec<str>, s: str) -> bool {
```

**Parameters:**

- `v`: `&Vec<str>`
- `s`: `str`

**Returns:** `bool`

*Defined at line 695*

---

## fl_index

```fides
pub fn fl_index(v: &Vec<str>, s: str) -> i64 {
```

**Parameters:**

- `v`: `&Vec<str>`
- `s`: `str`

**Returns:** `i64`

*Defined at line 704*

---

## fl_print

```fides
pub fn fl_print(s: str) {
```

**Parameters:**

- `s`: `str`

*Defined at line 713*

---

## fl_prog

The program's slashes as backslashes on Windows (CreateProcess does
not take "release/fid.exe" as Python's subprocess does), a bare name
that names a file here as ./name.

```fides
pub fn fl_prog(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 722*

---

## fl_code

What subprocess's returncode is for exit code `c` (unsigned on
Windows, -n for a signal elsewhere; process_wait gives 128 + n).

```fides
pub fn fl_code(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 730*

---

## FlRun

```fides
pub struct FlRun {
```

**Fields:**

| Name | Type |
|------|------|
| `started` | `bool` |
| `timed_out` | `bool` |
| `code` | `i64` |
| `out` | `str` |
| `err` | `str` |

*Defined at line 738*

---

## fl_setdefault_env

os.environ.setdefault(name, value): children get name=value unless
it is set already.

```fides
pub fn fl_setdefault_env(name: str, value: str) {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

*Defined at line 753*

---

## fl_run_env

Run `argv` (no shell) in `dir` ("" for ours) with `env` ("K=V") added,
for at most `secs` seconds (0: no limit). On a timeout the child is
killed, on Windows with its whole tree (taskkill /F /T, as the scripts'
_run does). Raw bytes in out and err (0 as C0 80).

```fides
pub fn fl_run_env(argv: &Vec<str>, env: &Vec<str>, dir: str, secs: i64) -> FlRun {
```

**Parameters:**

- `argv`: `&Vec<str>`
- `env`: `&Vec<str>`
- `dir`: `str`
- `secs`: `i64`

**Returns:** `FlRun`

*Defined at line 787*

---

## fl_run_env_ms

fl_run_env with the limit in milliseconds (negative: none; 0: the
child is stopped at once unless it has ended, as subprocess's
timeout=0).

```fides
pub fn fl_run_env_ms(argv: &Vec<str>, env: &Vec<str>, dir: str, limit_ms: i64) -> FlRun {
```

**Parameters:**

- `argv`: `&Vec<str>`
- `env`: `&Vec<str>`
- `dir`: `str`
- `limit_ms`: `i64`

**Returns:** `FlRun`

*Defined at line 796*

---

## fl_run

```fides
pub fn fl_run(argv: &Vec<str>, dir: str, secs: i64) -> FlRun {
```

**Parameters:**

- `argv`: `&Vec<str>`
- `dir`: `str`
- `secs`: `i64`

**Returns:** `FlRun`

*Defined at line 849*

---

## fl_as_text

A child's output as `text=True, errors="replace"` reads it: decoded
(UTF-8 here; Python uses the locale's code page on Windows, which the
ASCII of these programs' output does not tell apart), newlines
translated.

```fides
pub fn fl_as_text(raw: str) -> str {
```

**Parameters:**

- `raw`: `str`

**Returns:** `str`

*Defined at line 858*

---

## fl_v1

```fides
pub fn fl_v1(a: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`

**Returns:** `Vec<str>`

*Defined at line 864*

---

## fl_v2

```fides
pub fn fl_v2(a: str, b: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `Vec<str>`

*Defined at line 870*

---

## fl_v3

```fides
pub fn fl_v3(a: str, b: str, c: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`

**Returns:** `Vec<str>`

*Defined at line 876*

---

## fl_v5

```fides
pub fn fl_v5(a: str, b: str, c: str, d: str, e: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`
- `d`: `str`
- `e`: `str`

**Returns:** `Vec<str>`

*Defined at line 882*

---

## fl_word

Is c a word character of Python's re (ASCII part)?

```fides
pub fn fl_word(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 890*

---

## fl_digit

```fides
pub fn fl_digit(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 894*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
