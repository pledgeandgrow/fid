[All modules](index.md)

# base -- Fides Documentation

## Contents

- [pert_int_to_float](#pert_int_to_float) (function)
- [pert_float_to_int](#pert_float_to_int) (function)
- [pert_f64_to_bits](#pert_f64_to_bits) (function)
- [pert_f64_from_bits](#pert_f64_from_bits) (function)
- [pert_f32_to_bits](#pert_f32_to_bits) (function)
- [pert_f32_from_bits](#pert_f32_from_bits) (function)
- [pert_str_to_float](#pert_str_to_float) (function)
- [pert_str_to_int](#pert_str_to_int) (function)
- [pert_mem_copy](#pert_mem_copy) (function)
- [pert_str_copy_n](#pert_str_copy_n) (function)
- [pert_bytes_eq](#pert_bytes_eq) (function)
- [pert_mem_find_byte](#pert_mem_find_byte) (function)
- [pert_str_compare](#pert_str_compare) (function)
- [pert_str_equal](#pert_str_equal) (function)
- [pert_str_hash](#pert_str_hash) (function)
- [pert_to_str_float](#pert_to_str_float) (function)
- [pert_float_to_str](#pert_float_to_str) (function)
- [pert_print_float](#pert_print_float) (function)
- [pert_to_str](#pert_to_str) (function)
- [pert_char_to_str](#pert_char_to_str) (function)
- [pert_file_exists](#pert_file_exists) (function)
- [pert_file_append](#pert_file_append) (function)
- [pert_file_delete](#pert_file_delete) (function)
- [pert_file_size](#pert_file_size) (function)
- [pert_file_read_bytes](#pert_file_read_bytes) (function)
- [pert_file_write_vec](#pert_file_write_vec) (function)
- [pert_file_write_bin](#pert_file_write_bin) (function)
- [pert_file_read_at](#pert_file_read_at) (function)
- [pert_file_append_vec](#pert_file_append_vec) (function)
- [pert_file_mtime_ms](#pert_file_mtime_ms) (function)
- [pert_file_hash](#pert_file_hash) (function)
- [pert_c_putchar](#pert_c_putchar) (function)
- [pert_c_puts](#pert_c_puts) (function)
- [pert_region_word](#pert_region_word) (function)
- [pert_os_random_u64](#pert_os_random_u64) (function)
- [pert_mem_resident](#pert_mem_resident) (function)
- [pert_clock_now](#pert_clock_now) (function)
- [pert_clock_now_us](#pert_clock_now_us) (function)
- [pert_clock_now_ms](#pert_clock_now_ms) (function)
- [pert_exit](#pert_exit) (function)
- [pert_exit_now](#pert_exit_now) (function)
- [pert_getenv](#pert_getenv) (function)
- [pert_argc](#pert_argc) (function)
- [pert_argv](#pert_argv) (function)
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
- [pert_sb_new](#pert_sb_new) (function)
- [pert_sb_append](#pert_sb_append) (function)
- [pert_sb_to_str](#pert_sb_to_str) (function)
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
- [pert_dl_load](#pert_dl_load) (function)
- [pert_dl_find](#pert_dl_find) (function)
- [pert_dl_unload](#pert_dl_unload) (function)
- [pert_signal_register](#pert_signal_register) (function)
- [pert_signal_check](#pert_signal_check) (function)
- [pert_signal_raise](#pert_signal_raise) (function)
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
- [pert_str_zeroize](#pert_str_zeroize) (function)
- [pert_vec_zeroize](#pert_vec_zeroize) (function)
- [pert_sensitive_mark](#pert_sensitive_mark) (function)
- [pert_sensitive_remark](#pert_sensitive_remark) (function)
- [pert_sensitive_check](#pert_sensitive_check) (function)
- [pert_sensitive_unmark](#pert_sensitive_unmark) (function)
- [pert_sensitive_free](#pert_sensitive_free) (function)
- [pert_sensitive_wipes](#pert_sensitive_wipes) (function)
- [pert_crash_report](#pert_crash_report) (function)
- [pert_c_fid_proc_spawn](#pert_c_fid_proc_spawn) (function)
- [pert_c_fid_proc_spawn_io](#pert_c_fid_proc_spawn_io) (function)
- [pert_c_fid_proc_write](#pert_c_fid_proc_write) (function)
- [pert_c_fid_proc_read](#pert_c_fid_proc_read) (function)
- [pert_c_fid_proc_close_in](#pert_c_fid_proc_close_in) (function)
- [pert_c_fid_proc_wait](#pert_c_fid_proc_wait) (function)
- [pert_c_fid_proc_code](#pert_c_fid_proc_code) (function)
- [pert_c_fid_proc_kill](#pert_c_fid_proc_kill) (function)
- [pert_c_fid_proc_pid](#pert_c_fid_proc_pid) (function)
- [pert_c_fid_proc_free](#pert_c_fid_proc_free) (function)
- [pert_leak_malloc](#pert_leak_malloc) (function)
- [pert_leak_free](#pert_leak_free) (function)
- [pert_leak_exit](#pert_leak_exit) (function)

## pert_int_to_float

```fides
pub fn pert_int_to_float(x: i64) -> f64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `f64`

*Defined at line 76*

---

## pert_float_to_int

```fides
pub fn pert_float_to_int(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 80*

---

## pert_f64_to_bits

```fides
pub fn pert_f64_to_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 84*

---

## pert_f64_from_bits

```fides
pub fn pert_f64_from_bits(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 88*

---

## pert_f32_to_bits

The bits of an f32, sign-extended (C: (int64_t) of an int32_t).

```fides
pub fn pert_f32_to_bits(x: f32) -> i64 {
```

**Parameters:**

- `x`: `f32`

**Returns:** `i64`

*Defined at line 93*

---

## pert_f32_from_bits

```fides
pub fn pert_f32_from_bits(bits: i64) -> f32 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f32`

*Defined at line 101*

---

## pert_str_to_float

atof: 0.0 when the text is not a number.

```fides
pub fn pert_str_to_float(s: str) -> f64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `f64`

*Defined at line 106*

---

## pert_str_to_int

atoll: base 10, 0 when the text is not a number.

```fides
pub fn pert_str_to_int(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 114*

---

## pert_mem_copy

Core's copy, compare and byte search run msvcrt's vectorized routines
(competitive roadmap OP23): string building, Vec growth and copies,
`==` on byte runs and substring search no longer go a byte at a time.

```fides
pub fn pert_mem_copy(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 124*

---

## pert_str_copy_n

```fides
pub fn pert_str_copy_n(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 130*

---

## pert_bytes_eq

```fides
pub fn pert_bytes_eq(a: i64, b: i64, n: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

**Returns:** `bool`

*Defined at line 136*

---

## pert_mem_find_byte

```fides
pub fn pert_mem_find_byte(p: i64, n: i64, b: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `n`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 143*

---

## pert_str_compare

OP23: core's p_str_compare (bytes up to the first NUL of either; the
first difference as a - b, 0 when equal). strlen finds the NULs, msvcrt's
memcmp skips equal 64-byte runs (it gives only the sign, so the
difference itself is found byte by byte inside the run that differs).

```fides
pub fn pert_str_compare(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 159*

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

*Defined at line 190*

---

## pert_str_hash

```fides
pub fn pert_str_hash(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 268*

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

*Defined at line 342*

---

## pert_float_to_str

```fides
pub fn pert_float_to_str(val: f64) -> str {
```

**Parameters:**

- `val`: `f64`

**Returns:** `str`

*Defined at line 459*

---

## pert_print_float

```fides
pub fn pert_print_float(val: f64) {
```

**Parameters:**

- `val`: `f64`

*Defined at line 463*

---

## pert_to_str

p_to_str (int_to_str): v in decimal, a counted string (proposal 0004)
as the C runtime's and the machine layer's (linux_runtime pert_to_str).
RL16 (known-bugs 2g.81): the machine stub p_to_str made a plain
p_malloc buffer, which no drop frees (Str_drop leaves a string without
a count alone): every int_to_str leaked, as `--leaks` showed. Digits
are taken on the negative side, where i64::MIN has its value.

```fides
pub fn pert_to_str(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 473*

---

## pert_char_to_str

```fides
pub fn pert_char_to_str(c: i64) -> str {
```

**Parameters:**

- `c`: `i64`

**Returns:** `str`

*Defined at line 567*

---

## pert_file_exists

```fides
pub fn pert_file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 594*

---

## pert_file_append

```fides
pub fn pert_file_append(path: str, content: str) -> i64 {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `i64`

*Defined at line 603*

---

## pert_file_delete

```fides
pub fn pert_file_delete(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 628*

---

## pert_file_size

```fides
pub fn pert_file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 635*

---

## pert_file_read_bytes

Every byte of the file as one element (binary-safe); empty if unreadable.

```fides
pub fn pert_file_read_bytes(path: str) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 650*

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

*Defined at line 671*

---

## pert_file_write_bin

RT2: the `n` bytes at `data`, zero bytes included (p_file_write stops
at the first, as fputs does); 0, or -1 when the file cannot be written.

```fides
pub fn pert_file_write_bin(path: str, data: str, n: i64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `data`: `str`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 697*

---

## pert_file_read_at

Up to n bytes from byte `offset`, one element each; empty past the end
or when the file cannot be read. Only those bytes are read.

```fides
pub fn pert_file_read_at(path: str, offset: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`
- `offset`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 728*

---

## pert_file_append_vec

Each element's low byte after what the file holds (created if missing),
zero bytes included: 0, or -1 when it cannot be written.

```fides
pub fn pert_file_append_vec(path: str, v: Vec<i64>) -> i64 {
```

**Parameters:**

- `path`: `str`
- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 773*

---

## pert_file_mtime_ms

The time the file was last written, in ms since 1970 (UTC); -1 when it
does not exist (as the C runtime's p_file_mtime_ms).

```fides
pub fn pert_file_mtime_ms(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 802*

---

## pert_file_hash

```fides
pub fn pert_file_hash(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 822*

---

## pert_c_putchar

```fides
pub fn pert_c_putchar(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 847*

---

## pert_c_puts

```fides
pub fn pert_c_puts(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 852*

---

## pert_region_word

64 bits from the operating system's secure generator (msvcrt's rand_s,
over RtlGenRandom).
Regions freed in one step (proposal 0005) need the C runtime's arenas:
here no region is ever open, so regions stay scopes and core's region
checks fall through.

```fides
pub fn pert_region_word(op: i64, x: i64) -> i64 {
```

**Parameters:**

- `op`: `i64`
- `x`: `i64`

**Returns:** `i64`

*Defined at line 879*

---

## pert_os_random_u64

```fides
pub fn pert_os_random_u64() -> i64 {
```

**Returns:** `i64`

*Defined at line 883*

---

## pert_mem_resident

RT7: the bytes of the process in physical memory: the working set,
WorkingSetSize at offset 16 of a 72-byte PROCESS_MEMORY_COUNTERS
(the current process is the handle -1); -1 when Windows does not say.

```fides
pub fn pert_mem_resident() -> i64 {
```

**Returns:** `i64`

*Defined at line 914*

---

## pert_clock_now

```fides
pub fn pert_clock_now() -> i64 {
```

**Returns:** `i64`

*Defined at line 924*

---

## pert_clock_now_us

```fides
pub fn pert_clock_now_us() -> i64 {
```

**Returns:** `i64`

*Defined at line 928*

---

## pert_clock_now_ms

```fides
pub fn pert_clock_now_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 932*

---

## pert_exit

End the process with this exit code (output is written unbuffered, so
nothing is lost).

```fides
pub fn pert_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 938*

---

## pert_exit_now

Exit immediately without running the entry-point cleanup a second time.

```fides
pub fn pert_exit_now(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 949*

---

## pert_getenv

The variable's value, a counted copy (proposal 0004); null when it is
not set (p_getenv is a raw word: core's env_var gives the Option).

```fides
pub fn pert_getenv(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 955*

---

## pert_argc

The number of command-line arguments, the program's name included, as
a C program's argc (the machine stub split at every space).

```fides
pub fn pert_argc() -> i64 {
```

**Returns:** `i64`

*Defined at line 1089*

---

## pert_argv

Command-line argument i, by the C runtime's rules (pe_argv_scan: `"a b"`
is one argument without its quotes); null when there is none.

```fides
pub fn pert_argv(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 1095*

---

## pert_is_dir

```fides
pub fn pert_is_dir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 1116*

---

## pert_rename

```fides
pub fn pert_rename(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 1126*

---

## pert_mkdir

```fides
pub fn pert_mkdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1133*

---

## pert_rmdir

Remove an EMPTY directory (fs_rmdir_all does the walk-delete).

```fides
pub fn pert_rmdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1141*

---

## pert_getpid

```fides
pub fn pert_getpid() -> i64 {
```

**Returns:** `i64`

*Defined at line 1148*

---

## pert_dir_list

Entry names of a directory joined by '\n' (no "./.."); "" on failure.

```fides
pub fn pert_dir_list(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 1178*

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

*Defined at line 1240*

---

## pert_process_stdout

```fides
pub fn pert_process_stdout(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 1300*

---

## pert_process_stderr

```fides
pub fn pert_process_stderr(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 1308*

---

## pert_process_code

```fides
pub fn pert_process_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1315*

---

## pert_process_close

```fides
pub fn pert_process_close(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 1322*

---

## pert_regex_match

```fides
pub fn pert_regex_match(s: str, pattern: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 1332*

---

## pert_sb_new

```fides
pub fn pert_sb_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 1388*

---

## pert_sb_append

```fides
pub fn pert_sb_append(sb: i64, s: str) {
```

**Parameters:**

- `sb`: `i64`
- `s`: `str`

*Defined at line 1396*

---

## pert_sb_to_str

```fides
pub fn pert_sb_to_str(sb: i64) -> str {
```

**Parameters:**

- `sb`: `i64`

**Returns:** `str`

*Defined at line 1414*

---

## pert_alloc

```fides
pub fn pert_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 1422*

---

## pert_volatile_load

```fides
pub fn pert_volatile_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 1426*

---

## pert_volatile_store

```fides
pub fn pert_volatile_store(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 1437*

---

## pert_volatile_load8

```fides
pub fn pert_volatile_load8(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 1445*

---

## pert_volatile_store8

```fides
pub fn pert_volatile_store8(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 1449*

---

## pert_volatile_load32

```fides
pub fn pert_volatile_load32(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 1453*

---

## pert_volatile_store32

```fides
pub fn pert_volatile_store32(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 1463*

---

## pert_simd_add

```fides
pub fn pert_simd_add(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1473*

---

## pert_simd_sub

```fides
pub fn pert_simd_sub(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1483*

---

## pert_simd_mul

```fides
pub fn pert_simd_mul(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 1493*

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

*Defined at line 1504*

---

## pert_simd_dot

```fides
pub fn pert_simd_dot(a: Vec<f64>, b: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 1519*

---

## pert_simd_sum

```fides
pub fn pert_simd_sum(a: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 1529*

---

## pert_simd_scale

```fides
pub fn pert_simd_scale(a: Vec<f64>, k: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `k`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 1539*

---

## pert_simd_from_vec

```fides
pub fn pert_simd_from_vec(v: Vec<i64>) -> Vec<f64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<f64>`

*Defined at line 1549*

---

## pert_simd_to_str

"[1, 2.5]": whole numbers as integers, others as %g.

```fides
pub fn pert_simd_to_str(a: Vec<f64>) -> str {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `str`

*Defined at line 1560*

---

## pert_dl_open

Sequential handles 1, 2, ...; 0 for an empty path.

```fides
pub fn pert_dl_open(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1586*

---

## pert_dl_sym

```fides
pub fn pert_dl_sym(handle: i64, symbol: str) -> i64 {
```

**Parameters:**

- `handle`: `i64`
- `symbol`: `str`

**Returns:** `i64`

*Defined at line 1594*

---

## pert_dl_close

```fides
pub fn pert_dl_close(handle: i64) {
```

**Parameters:**

- `handle`: `i64`

*Defined at line 1601*

---

## pert_dl_call

A simulated symbol (p_dl_sym: below 64 KiB, where no code is) gives the
argument back; a real one (p_dl_find) is called as i64 -> i64, the C
convention (C backend: the same test).

```fides
pub fn pert_dl_call(func: i64, arg: i64) -> i64 {
```

**Parameters:**

- `func`: `i64`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 1607*

---

## pert_dl_load

```fides
pub fn pert_dl_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1618*

---

## pert_dl_find

```fides
pub fn pert_dl_find(handle: i64, name: str) -> i64 {
```

**Parameters:**

- `handle`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 1625*

---

## pert_dl_unload

```fides
pub fn pert_dl_unload(handle: i64) -> i64 {
```

**Parameters:**

- `handle`: `i64`

**Returns:** `i64`

*Defined at line 1632*

---

## pert_signal_register

SIGINT (2) and SIGTERM (15) can be registered; -1 for any other.

```fides
pub fn pert_signal_register(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 1682*

---

## pert_signal_check

1 when `sig` arrived since the last check (and clears it), else 0.

```fides
pub fn pert_signal_check(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 1701*

---

## pert_signal_raise

msvcrt's raise for these two: a registered signal is noted (0); an
unregistered one ends the process with status 3 (the default action).

```fides
pub fn pert_signal_raise(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 1715*

---

## pert_allocator_create

```fides
pub fn pert_allocator_create(arg: i64) -> i64 {
```

**Parameters:**

- `arg`: `i64`

**Returns:** `i64`

*Defined at line 1735*

---

## pert_allocator_alloc

```fides
pub fn pert_allocator_alloc(alloc: i64, size: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 1739*

---

## pert_allocator_free

```fides
pub fn pert_allocator_free(alloc: i64, ptr: i64) {
```

**Parameters:**

- `alloc`: `i64`
- `ptr`: `i64`

*Defined at line 1746*

---

## pert_allocator_stats

```fides
pub fn pert_allocator_stats(alloc: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`

**Returns:** `i64`

*Defined at line 1750*

---

## pert_ecs_create_entity

```fides
pub fn pert_ecs_create_entity() -> i64 {
```

**Returns:** `i64`

*Defined at line 1783*

---

## pert_ecs_entity_count

```fides
pub fn pert_ecs_entity_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 1789*

---

## pert_ecs_has

```fides
pub fn pert_ecs_has(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 1793*

---

## pert_ecs_get

```fides
pub fn pert_ecs_get(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 1808*

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

*Defined at line 1824*

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

*Defined at line 1840*

---

## pert_flush_stdout

Our prints go straight to WriteFile — unbuffered — so there is nothing
pending; flushing all C streams mirrors `fflush(stdout)` anyway.

```fides
pub fn pert_flush_stdout() {
```

*Defined at line 1872*

---

## pert_stdin_eof

```fides
pub fn pert_stdin_eof() -> i64 {
```

**Returns:** `i64`

*Defined at line 1876*

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

*Defined at line 1882*

---

## pert_read_line

p_read_line(): one line without its trailing '\n' (and '\r'), the eof
flag set at the end of the stream (callers test it like feof()); 0, the
null core's read_line takes for the end of the input, when the stream
ended before a byte was read, as the C runtime's and the machine
layer's (BG250: it gave "" there, so `while read_line()` never ended:
test_read_line_long's child hung). The buffer grows to the whole line
(it cut lines at 65,536 bytes). Byte reads are enough: this serves the
REPL/debugger, not bulk input.

```fides
pub fn pert_read_line() -> str {
```

**Returns:** `str`

*Defined at line 1918*

---

## pert_str_zeroize

```fides
pub fn pert_str_zeroize(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 1973*

---

## pert_vec_zeroize

```fides
pub fn pert_vec_zeroize(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 1982*

---

## pert_sensitive_mark

```fides
pub fn pert_sensitive_mark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2049*

---

## pert_sensitive_remark

```fides
pub fn pert_sensitive_remark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2060*

---

## pert_sensitive_check

```fides
pub fn pert_sensitive_check(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2070*

---

## pert_sensitive_unmark

```fides
pub fn pert_sensitive_unmark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2077*

---

## pert_sensitive_free

```fides
pub fn pert_sensitive_free(v: i64, buf: i64, cap: i64) {
```

**Parameters:**

- `v`: `i64`
- `buf`: `i64`
- `cap`: `i64`

*Defined at line 2091*

---

## pert_sensitive_wipes

```fides
pub fn pert_sensitive_wipes() -> i64 {
```

**Returns:** `i64`

*Defined at line 2104*

---

## pert_crash_report

CO9: called by pe_ir's p_panic stub with the panic's message (a
NUL-ended text) after it printed it: the crash report, when
FID_CRASH_REPORT names a file; stderr says where it is.

```fides
pub fn pert_crash_report(msg: i64) {
```

**Parameters:**

- `msg`: `i64`

*Defined at line 2326*

---

## pert_c_fid_proc_spawn

```fides
pub fn pert_c_fid_proc_spawn(argv_list: i64, env_list: i64, dir: i64, out: i64) -> i64 {
```

**Parameters:**

- `argv_list`: `i64`
- `env_list`: `i64`
- `dir`: `i64`
- `out`: `i64`

**Returns:** `i64`

*Defined at line 2667*

---

## pert_c_fid_proc_spawn_io

roadmap L4H (stdlib/process.fd process_spawn_io, Command::spawn): io bits
1 / 2 / 4 give the child a pipe as its stdin / stdout / stderr
(pert_c_fid_proc_write / _read / _close_in), 8 sends its stderr where
its stdout goes; a piped stream takes no file.

```fides
pub fn pert_c_fid_proc_spawn_io(argv_list: i64, env_list: i64, dir: i64, out: i64, io: i64) -> i64 {
```

**Parameters:**

- `argv_list`: `i64`
- `env_list`: `i64`
- `dir`: `i64`
- `out`: `i64`
- `io`: `i64`

**Returns:** `i64`

*Defined at line 2675*

---

## pert_c_fid_proc_write

All n bytes at data to the child's stdin pipe: n, or -1 when it has
none or the child closed it.

```fides
pub fn pert_c_fid_proc_write(h: i64, data: i64, n: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `data`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 2915*

---

## pert_c_fid_proc_read

Up to max bytes the child wrote to its stdout (which 1) or stderr (2)
pipe, into buf, waiting for at least one: 0 once the pipe is closed and
empty (the child ended), -1 when there is no such pipe.

```fides
pub fn pert_c_fid_proc_read(h: i64, which: i64, buf: i64, max: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `which`: `i64`
- `buf`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 2950*

---

## pert_c_fid_proc_close_in

Close our end of the child's stdin pipe: it reads the end of its input.

```fides
pub fn pert_c_fid_proc_close_in(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2989*

---

## pert_c_fid_proc_wait

1 once the child has ended (its code kept), 0 while it is still running
after timeout_ms (< 0: no limit), -1 for a bad handle.

```fides
pub fn pert_c_fid_proc_wait(h: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 3003*

---

## pert_c_fid_proc_code

Its exit code once pert_c_fid_proc_wait said it ended, else -1.

```fides
pub fn pert_c_fid_proc_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 3040*

---

## pert_c_fid_proc_kill

TerminateProcess with code 137 (128 + SIGKILL, as POSIX reports a
killed child): 1 when asked, 0 when it had ended or could not be.

```fides
pub fn pert_c_fid_proc_kill(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 3052*

---

## pert_c_fid_proc_pid

```fides
pub fn pert_c_fid_proc_pid(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 3065*

---

## pert_c_fid_proc_free

Forget the handle (a child still running keeps running).

```fides
pub fn pert_c_fid_proc_free(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 3073*

---

## pert_leak_malloc

p_malloc under --leaks (the backend's thunk calls it).

```fides
pub fn pert_leak_malloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 3318*

---

## pert_leak_free

p_free under --leaks: a tracked block leaves the list and goes back
whole; any other block straight to the allocator (which panics on a
pointer that is no block, or one freed twice).

```fides
pub fn pert_leak_free(p: i64) {
```

**Parameters:**

- `p`: `i64`

*Defined at line 3361*

---

## pert_leak_exit

After main returns (the backend calls it under --leaks, main's result
in): the report; main's result back.

```fides
pub fn pert_leak_exit(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 3514*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
