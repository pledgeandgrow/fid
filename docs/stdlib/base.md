# base — Fides Documentation

## Contents

- [pert_int_to_float](#pert_int_to_float) (function)
- [pert_float_to_int](#pert_float_to_int) (function)
- [pert_f64_to_bits](#pert_f64_to_bits) (function)
- [pert_f64_from_bits](#pert_f64_from_bits) (function)
- [pert_f32_to_bits](#pert_f32_to_bits) (function)
- [pert_f32_from_bits](#pert_f32_from_bits) (function)
- [pert_str_to_float](#pert_str_to_float) (function)
- [pert_str_to_int](#pert_str_to_int) (function)
- [pert_str_equal](#pert_str_equal) (function)
- [pert_to_str_float](#pert_to_str_float) (function)
- [pert_float_to_str](#pert_float_to_str) (function)
- [pert_print_float](#pert_print_float) (function)
- [pert_file_exists](#pert_file_exists) (function)
- [pert_file_append](#pert_file_append) (function)
- [pert_file_delete](#pert_file_delete) (function)
- [pert_file_size](#pert_file_size) (function)
- [pert_file_read_bytes](#pert_file_read_bytes) (function)
- [pert_file_write_vec](#pert_file_write_vec) (function)
- [pert_file_hash](#pert_file_hash) (function)
- [pert_c_putchar](#pert_c_putchar) (function)
- [pert_c_puts](#pert_c_puts) (function)
- [pert_clock_now](#pert_clock_now) (function)
- [pert_clock_now_us](#pert_clock_now_us) (function)
- [pert_clock_now_ms](#pert_clock_now_ms) (function)
- [pert_exit](#pert_exit) (function)
- [pert_getenv](#pert_getenv) (function)
- [pert_is_dir](#pert_is_dir) (function)
- [pert_rename](#pert_rename) (function)
- [pert_mkdir](#pert_mkdir) (function)
- [pert_rmdir](#pert_rmdir) (function)
- [pert_getpid](#pert_getpid) (function)
- [pert_dir_list](#pert_dir_list) (function)
- [pert_process_run](#pert_process_run) (function)
- [pert_process_stdout](#pert_process_stdout) (function)
- [pert_process_stderr](#pert_process_stderr) (function)
- [pert_process_code](#pert_process_code) (function)
- [pert_process_close](#pert_process_close) (function)
- [pert_regex_match](#pert_regex_match) (function)
- [pert_alloc](#pert_alloc) (function)
- [pert_volatile_load](#pert_volatile_load) (function)
- [pert_volatile_store](#pert_volatile_store) (function)
- [pert_volatile_load8](#pert_volatile_load8) (function)
- [pert_volatile_store8](#pert_volatile_store8) (function)
- [pert_volatile_load32](#pert_volatile_load32) (function)
- [pert_volatile_store32](#pert_volatile_store32) (function)
- [pert_simd_add](#pert_simd_add) (function)
- [pert_simd_sub](#pert_simd_sub) (function)
- [pert_simd_mul](#pert_simd_mul) (function)
- [pert_simd_div](#pert_simd_div) (function)
- [pert_simd_dot](#pert_simd_dot) (function)
- [pert_simd_sum](#pert_simd_sum) (function)
- [pert_simd_scale](#pert_simd_scale) (function)
- [pert_simd_from_vec](#pert_simd_from_vec) (function)
- [pert_simd_to_str](#pert_simd_to_str) (function)
- [pert_dl_open](#pert_dl_open) (function)
- [pert_dl_sym](#pert_dl_sym) (function)
- [pert_dl_close](#pert_dl_close) (function)
- [pert_dl_call](#pert_dl_call) (function)
- [pert_allocator_create](#pert_allocator_create) (function)
- [pert_allocator_alloc](#pert_allocator_alloc) (function)
- [pert_allocator_free](#pert_allocator_free) (function)
- [pert_allocator_stats](#pert_allocator_stats) (function)
- [pert_ecs_create_entity](#pert_ecs_create_entity) (function)
- [pert_ecs_entity_count](#pert_ecs_entity_count) (function)
- [pert_ecs_has](#pert_ecs_has) (function)
- [pert_ecs_get](#pert_ecs_get) (function)
- [pert_ecs_add](#pert_ecs_add) (function)
- [pert_ecs_remove](#pert_ecs_remove) (function)
- [pert_flush_stdout](#pert_flush_stdout) (function)
- [pert_stdin_eof](#pert_stdin_eof) (function)
- [pert_read_stdin](#pert_read_stdin) (function)
- [pert_read_line](#pert_read_line) (function)

## pert_int_to_float

```fides
pub fn pert_int_to_float(x: i64) -> f64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `f64`

*Defined at line 60*

---

## pert_float_to_int

```fides
pub fn pert_float_to_int(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 64*

---

## pert_f64_to_bits

```fides
pub fn pert_f64_to_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 68*

---

## pert_f64_from_bits

```fides
pub fn pert_f64_from_bits(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 72*

---

## pert_f32_to_bits

The bits of an f32, sign-extended (C: (int64_t) of an int32_t).

```fides
pub fn pert_f32_to_bits(x: f32) -> i64 {
```

**Parameters:**

- `x`: `f32`

**Returns:** `i64`

*Defined at line 77*

---

## pert_f32_from_bits

```fides
pub fn pert_f32_from_bits(bits: i64) -> f32 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f32`

*Defined at line 85*

---

## pert_str_to_float

atof: 0.0 when the text is not a number.

```fides
pub fn pert_str_to_float(s: str) -> f64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `f64`

*Defined at line 90*

---

## pert_str_to_int

atoll: base 10, 0 when the text is not a number.

```fides
pub fn pert_str_to_int(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 98*

---

## pert_str_equal

p_str_equal for this backend: compare the lengths first, then the
memory in one msvcrt memcmp — not byte by byte (pe-roadmap item 74).

```fides
pub fn pert_str_equal(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 107*

---

## pert_to_str_float

A float as the interpreter prints it (Rust's Display): the shortest
fixed-point text that reads back as the same number, no exponent.

```fides
pub fn pert_to_str_float(val: f64) -> str {
```

**Parameters:**

- `val`: `f64`

**Returns:** `str`

*Defined at line 179*

---

## pert_float_to_str

```fides
pub fn pert_float_to_str(val: f64) -> str {
```

**Parameters:**

- `val`: `f64`

**Returns:** `str`

*Defined at line 235*

---

## pert_print_float

```fides
pub fn pert_print_float(val: f64) {
```

**Parameters:**

- `val`: `f64`

*Defined at line 239*

---

## pert_file_exists

```fides
pub fn pert_file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 290*

---

## pert_file_append

```fides
pub fn pert_file_append(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 299*

---

## pert_file_delete

```fides
pub fn pert_file_delete(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 309*

---

## pert_file_size

```fides
pub fn pert_file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 316*

---

## pert_file_read_bytes

Every byte of the file as one element (binary-safe); empty if unreadable.

```fides
pub fn pert_file_read_bytes(path: str) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 331*

---

## pert_file_write_vec

Each element's low byte, in order.

```fides
pub fn pert_file_write_vec(path: str, v: Vec<i64>) -> i64 {
```

**Parameters:**

- `path`: `str`
- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 352*

---

## pert_file_hash

```fides
pub fn pert_file_hash(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 373*

---

## pert_c_putchar

```fides
pub fn pert_c_putchar(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 398*

---

## pert_c_puts

```fides
pub fn pert_c_puts(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 403*

---

## pert_clock_now

```fides
pub fn pert_clock_now() -> i64 {
```

**Returns:** `i64`

*Defined at line 421*

---

## pert_clock_now_us

```fides
pub fn pert_clock_now_us() -> i64 {
```

**Returns:** `i64`

*Defined at line 425*

---

## pert_clock_now_ms

```fides
pub fn pert_clock_now_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 429*

---

## pert_exit

End the process with this exit code (output is written unbuffered, so
nothing is lost).

```fides
pub fn pert_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 435*

---

## pert_getenv

The variable's value; null when it is not set.

```fides
pub fn pert_getenv(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 440*

---

## pert_is_dir

```fides
pub fn pert_is_dir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 446*

---

## pert_rename

```fides
pub fn pert_rename(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 456*

---

## pert_mkdir

```fides
pub fn pert_mkdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 463*

---

## pert_rmdir

Remove an EMPTY directory (fs_rmdir_all does the walk-delete).

```fides
pub fn pert_rmdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 471*

---

## pert_getpid

```fides
pub fn pert_getpid() -> i64 {
```

**Returns:** `i64`

*Defined at line 478*

---

## pert_dir_list

Entry names of a directory joined by '\n' (no "./.."); "" on failure.

```fides
pub fn pert_dir_list(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 508*

---

## pert_process_run

Run a shell command capturing stdout/stderr/exit code. The handle is a
heap record [out ptr, err ptr, code]; 0 on failure to start.

```fides
pub fn pert_process_run(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 570*

---

## pert_process_stdout

```fides
pub fn pert_process_stdout(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 600*

---

## pert_process_stderr

```fides
pub fn pert_process_stderr(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 604*

---

## pert_process_code

```fides
pub fn pert_process_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 608*

---

## pert_process_close

```fides
pub fn pert_process_close(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 615*

---

## pert_regex_match

```fides
pub fn pert_regex_match(s: str, pattern: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 625*

---

## pert_alloc

```fides
pub fn pert_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 661*

---

## pert_volatile_load

```fides
pub fn pert_volatile_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 665*

---

## pert_volatile_store

```fides
pub fn pert_volatile_store(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 672*

---

## pert_volatile_load8

```fides
pub fn pert_volatile_load8(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 677*

---

## pert_volatile_store8

```fides
pub fn pert_volatile_store8(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 681*

---

## pert_volatile_load32

```fides
pub fn pert_volatile_load32(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 685*

---

## pert_volatile_store32

```fides
pub fn pert_volatile_store32(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 692*

---

## pert_simd_add

```fides
pub fn pert_simd_add(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 699*

---

## pert_simd_sub

```fides
pub fn pert_simd_sub(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 709*

---

## pert_simd_mul

```fides
pub fn pert_simd_mul(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 719*

---

## pert_simd_div

x / 0 is 0.

```fides
pub fn pert_simd_div(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 730*

---

## pert_simd_dot

```fides
pub fn pert_simd_dot(a: Vec<f64>, b: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 745*

---

## pert_simd_sum

```fides
pub fn pert_simd_sum(a: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 755*

---

## pert_simd_scale

```fides
pub fn pert_simd_scale(a: Vec<f64>, k: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `k`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 765*

---

## pert_simd_from_vec

```fides
pub fn pert_simd_from_vec(v: Vec<i64>) -> Vec<f64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<f64>`

*Defined at line 775*

---

## pert_simd_to_str

"[1, 2.5]": whole numbers as integers, others as %g.

```fides
pub fn pert_simd_to_str(a: Vec<f64>) -> str {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `str`

*Defined at line 786*

---

## pert_dl_open

Sequential handles 1, 2, ...; 0 for an empty path.

```fides
pub fn pert_dl_open(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 812*

---

## pert_dl_sym

```fides
pub fn pert_dl_sym(handle: i64, symbol: str) -> i64 {
```

**Parameters:**

- `handle`: `i64`
- `symbol`: `str`

**Returns:** `i64`

*Defined at line 820*

---

## pert_dl_close

```fides
pub fn pert_dl_close(handle: i64) {
```

**Parameters:**

- `handle`: `i64`

*Defined at line 827*

---

## pert_dl_call

Simulated: the argument back.

```fides
pub fn pert_dl_call(func: i64, arg: i64) -> i64 {
```

**Parameters:**

- `func`: `i64`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 831*

---

## pert_allocator_create

```fides
pub fn pert_allocator_create(arg: i64) -> i64 {
```

**Parameters:**

- `arg`: `i64`

**Returns:** `i64`

*Defined at line 837*

---

## pert_allocator_alloc

```fides
pub fn pert_allocator_alloc(alloc: i64, size: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 841*

---

## pert_allocator_free

```fides
pub fn pert_allocator_free(alloc: i64, ptr: i64) {
```

**Parameters:**

- `alloc`: `i64`
- `ptr`: `i64`

*Defined at line 848*

---

## pert_allocator_stats

```fides
pub fn pert_allocator_stats(alloc: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`

**Returns:** `i64`

*Defined at line 852*

---

## pert_ecs_create_entity

```fides
pub fn pert_ecs_create_entity() -> i64 {
```

**Returns:** `i64`

*Defined at line 885*

---

## pert_ecs_entity_count

```fides
pub fn pert_ecs_entity_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 891*

---

## pert_ecs_has

```fides
pub fn pert_ecs_has(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 895*

---

## pert_ecs_get

```fides
pub fn pert_ecs_get(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 910*

---

## pert_ecs_add

Set the entity's value (a new entry, or the existing one updated).

```fides
pub fn pert_ecs_add(name: str, entity: i64, value: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 926*

---

## pert_ecs_remove

Remove the entity's entry: the last entry takes its place.

```fides
pub fn pert_ecs_remove(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 942*

---

## pert_flush_stdout

Our prints go straight to WriteFile — unbuffered — so there is nothing
pending; flushing all C streams mirrors `fflush(stdout)` anyway.

```fides
pub fn pert_flush_stdout() {
```

*Defined at line 977*

---

## pert_stdin_eof

```fides
pub fn pert_stdin_eof() -> i64 {
```

**Returns:** `i64`

*Defined at line 981*

---

## pert_read_stdin

p_read_stdin(n): up to n raw bytes from stdin as a NUL-terminated str.
Sets the eof flag when the stream ends before n bytes.

```fides
pub fn pert_read_stdin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 987*

---

## pert_read_line

p_read_line(): one line without the trailing '\n'; "" on EOF or error
(eof flag set on end of stream — callers test it like feof()).
Byte reads are enough: this serves the REPL/debugger, not bulk input.

```fides
pub fn pert_read_line() -> str {
```

**Returns:** `str`

*Defined at line 1018*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
