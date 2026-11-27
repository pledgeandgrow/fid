[All modules](index.md)

# compiler::runtime -- Fides Documentation

## Contents

- [p_malloc](#p_malloc) (function)
- [p_free](#p_free) (function)
- [p_alloc](#p_alloc) (function)
- [p_memset](#p_memset) (function)
- [p_memcpy](#p_memcpy) (function)
- [rt_str_length](#rt_str_length) (function)
- [p_str_concat](#p_str_concat) (function)
- [p_str_equal](#p_str_equal) (function)
- [p_str_compare](#p_str_compare) (function)
- [p_to_str](#p_to_str) (function)
- [p_char_to_str](#p_char_to_str) (function)
- [p_bool_to_str](#p_bool_to_str) (function)
- [p_write_str](#p_write_str) (function)
- [p_print_str](#p_print_str) (function)
- [p_eprint_str](#p_eprint_str) (function)
- [p_print_int](#p_print_int) (function)
- [p_print_bool](#p_print_bool) (function)
- [p_print_newline](#p_print_newline) (function)
- [p_print](#p_print) (function)
- [p_eprint](#p_eprint) (function)
- [p_panic](#p_panic) (function)
- [p_exit](#p_exit) (function)
- [p_file_write](#p_file_write) (function)
- [p_file_append](#p_file_append) (function)
- [p_file_exists](#p_file_exists) (function)
- [p_file_read](#p_file_read) (function)
- [p_argc](#p_argc) (function)
- [p_str_wrap](#p_str_wrap) (function)
- [p_argv](#p_argv) (function)
- [p_getenv](#p_getenv) (function)
- [p_clock_now](#p_clock_now) (function)
- [p_clock_now_ms](#p_clock_now_ms) (function)
- [p_clock_now_us](#p_clock_now_us) (function)

## p_malloc

```fides
pub fn p_malloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 133*

---

## p_free

```fides
pub fn p_free(ptr: i64) {
```

**Parameters:**

- `ptr`: `i64`

*Defined at line 207*

---

## p_alloc

```fides
pub fn p_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 224*

---

## p_memset

```fides
pub fn p_memset(dst: i64, byte: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `byte`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 233*

---

## p_memcpy

```fides
pub fn p_memcpy(dst: i64, src: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 245*

---

## rt_str_length

```fides
pub fn rt_str_length(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 264*

---

## p_str_concat

```fides
pub fn p_str_concat(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 278*

---

## p_str_equal

```fides
pub fn p_str_equal(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 294*

---

## p_str_compare

```fides
pub fn p_str_compare(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 316*

---

## p_to_str

```fides
pub fn p_to_str(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 342*

---

## p_char_to_str

```fides
pub fn p_char_to_str(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 407*

---

## p_bool_to_str

```fides
pub fn p_bool_to_str(b: bool) -> i64 {
```

**Parameters:**

- `b`: `bool`

**Returns:** `i64`

*Defined at line 420*

---

## p_write_str

```fides
pub fn p_write_str(fd: i64, s: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `s`: `i64`

**Returns:** `i64`

*Defined at line 441*

---

## p_print_str

```fides
pub fn p_print_str(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 445*

---

## p_eprint_str

```fides
pub fn p_eprint_str(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 449*

---

## p_print_int

```fides
pub fn p_print_int(n: i64) {
```

**Parameters:**

- `n`: `i64`

*Defined at line 453*

---

## p_print_bool

```fides
pub fn p_print_bool(b: bool) {
```

**Parameters:**

- `b`: `bool`

*Defined at line 459*

---

## p_print_newline

```fides
pub fn p_print_newline() {
```

*Defined at line 472*

---

## p_print

```fides
pub fn p_print(s: str) {
```

**Parameters:**

- `s`: `str`

*Defined at line 476*

---

## p_eprint

```fides
pub fn p_eprint(s: str) {
```

**Parameters:**

- `s`: `str`

*Defined at line 480*

---

## p_panic

```fides
pub fn p_panic(msg: i64) {
```

**Parameters:**

- `msg`: `i64`

*Defined at line 485*

---

## p_exit

```fides
pub fn p_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 491*

---

## p_file_write

```fides
pub fn p_file_write(path: i64, content: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`
- `content`: `i64`

**Returns:** `i64`

*Defined at line 508*

---

## p_file_append

```fides
pub fn p_file_append(path: i64, content: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`
- `content`: `i64`

**Returns:** `i64`

*Defined at line 519*

---

## p_file_exists

```fides
pub fn p_file_exists(path: i64) -> bool {
```

**Parameters:**

- `path`: `i64`

**Returns:** `bool`

*Defined at line 530*

---

## p_file_read

```fides
pub fn p_file_read(path: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`

**Returns:** `i64`

*Defined at line 540*

---

## p_argc

```fides
pub fn p_argc() -> i64 {
```

**Returns:** `i64`

*Defined at line 579*

---

## p_str_wrap

```fides
pub fn p_str_wrap(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 584*

---

## p_argv

```fides
pub fn p_argv(i: i64) -> i64 {
```

**Parameters:**

- `i`: `i64`

**Returns:** `i64`

*Defined at line 594*

---

## p_getenv

```fides
pub fn p_getenv(name: i64) -> i64 {
```

**Parameters:**

- `name`: `i64`

**Returns:** `i64`

*Defined at line 598*

---

## p_clock_now

```fides
pub fn p_clock_now() -> i64 {
```

**Returns:** `i64`

*Defined at line 602*

---

## p_clock_now_ms

```fides
pub fn p_clock_now_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 606*

---

## p_clock_now_us

```fides
pub fn p_clock_now_us() -> i64 {
```

**Returns:** `i64`

*Defined at line 610*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
