[All modules](index.md)

# base -- Fides Documentation

## Contents

- [pe_heap_thread_end](#pe_heap_thread_end) (function)
- [pert_malloc](#pert_malloc) (function)
- [pert_malloc_uninit](#pert_malloc_uninit) (function)
- [pert_alloc](#pert_alloc) (function)
- [pert_free](#pert_free) (function)
- [pert_mem_resident](#pert_mem_resident) (function)
- [pert_print_str](#pert_print_str) (function)
- [pert_eprint_str](#pert_eprint_str) (function)
- [pert_print_newline](#pert_print_newline) (function)
- [pert_print_int](#pert_print_int) (function)
- [pert_print_bool](#pert_print_bool) (function)
- [pert_print_float](#pert_print_float) (function)
- [pert_flush_stdout](#pert_flush_stdout) (function)
- [pert_panic](#pert_panic) (function)
- [pert_inb](#pert_inb) (function)
- [pert_inw](#pert_inw) (function)
- [pert_inl](#pert_inl) (function)
- [pert_outb](#pert_outb) (function)
- [pert_outw](#pert_outw) (function)
- [pert_outl](#pert_outl) (function)
- [pert_cli](#pert_cli) (function)
- [pert_sti](#pert_sti) (function)
- [pert_hlt](#pert_hlt) (function)
- [pert_serial_init](#pert_serial_init) (function)
- [pert_serial_write](#pert_serial_write) (function)
- [pert_serial_read](#pert_serial_read) (function)
- [pert_serial_ready](#pert_serial_ready) (function)
- [pert_exit](#pert_exit) (function)
- [pert_exit_now](#pert_exit_now) (function)
- [pe_rt_exit](#pe_rt_exit) (function)
- [pe_rt_init](#pe_rt_init) (function)
- [pe_on_signal](#pe_on_signal) (function)
- [pe_on_signal_uc](#pe_on_signal_uc) (function)
- [pe_dbg_frame_print](#pe_dbg_frame_print) (function)
- [pert_vblk_copy](#pert_vblk_copy) (function)
- [pert_vblk_diff](#pert_vblk_diff) (function)
- [pert_vblk_cmpz](#pert_vblk_cmpz) (function)
- [pert_vblk_find](#pert_vblk_find) (function)
- [pert_mem_copy](#pert_mem_copy) (function)
- [pert_str_copy_n](#pert_str_copy_n) (function)
- [pert_bytes_eq](#pert_bytes_eq) (function)
- [pert_mem_find_byte](#pert_mem_find_byte) (function)
- [pert_str_concat](#pert_str_concat) (function)
- [pert_str_join_n](#pert_str_join_n) (function)
- [pert_to_str](#pert_to_str) (function)
- [pert_to_str_bool](#pert_to_str_bool) (function)
- [pert_bool_to_str](#pert_bool_to_str) (function)
- [pert_to_str_str](#pert_to_str_str) (function)
- [pert_char_to_str](#pert_char_to_str) (function)
- [pert_to_str_vec](#pert_to_str_vec) (function)
- [pert_print_vec](#pert_print_vec) (function)
- [pert_print_map](#pert_print_map) (function)
- [pert_str_equal](#pert_str_equal) (function)
- [pert_str_compare](#pert_str_compare) (function)
- [pert_str_hash](#pert_str_hash) (function)
- [pert_str_to_int](#pert_str_to_int) (function)
- [pert_str_to_float](#pert_str_to_float) (function)
- [pert_to_str_float](#pert_to_str_float) (function)
- [pert_float_to_str](#pert_float_to_str) (function)
- [pert_sb_new](#pert_sb_new) (function)
- [pert_sb_append](#pert_sb_append) (function)
- [pert_sb_to_str](#pert_sb_to_str) (function)
- [pert_allocator_create](#pert_allocator_create) (function)
- [pert_allocator_alloc](#pert_allocator_alloc) (function)
- [pert_allocator_free](#pert_allocator_free) (function)
- [pert_allocator_stats](#pert_allocator_stats) (function)
- [pert_string_len](#pert_string_len) (function)
- [pert_string_find](#pert_string_find) (function)
- [pert_string_contains](#pert_string_contains) (function)
- [pert_string_starts_with](#pert_string_starts_with) (function)
- [pert_string_ends_with](#pert_string_ends_with) (function)
- [pert_string_replace](#pert_string_replace) (function)
- [pert_string_trim](#pert_string_trim) (function)
- [pert_string_to_uppercase](#pert_string_to_uppercase) (function)
- [pert_string_to_lowercase](#pert_string_to_lowercase) (function)
- [pert_string_from](#pert_string_from) (function)
- [pert_string_substring](#pert_string_substring) (function)
- [pert_stdin_eof](#pert_stdin_eof) (function)
- [pert_read_line](#pert_read_line) (function)
- [pert_read_stdin](#pert_read_stdin) (function)
- [pert_string_split](#pert_string_split) (function)
- [pert_argc](#pert_argc) (function)
- [pert_argv](#pert_argv) (function)
- [pert_getenv](#pert_getenv) (function)
- [pert_getpid](#pert_getpid) (function)
- [pert_clock_now](#pert_clock_now) (function)
- [pert_clock_now_us](#pert_clock_now_us) (function)
- [pert_clock_now_ms](#pert_clock_now_ms) (function)
- [pert_clock_mono_ms](#pert_clock_mono_ms) (function)
- [pert_region_word](#pert_region_word) (function)
- [pert_os_random_u64](#pert_os_random_u64) (function)
- [pert_file_read](#pert_file_read) (function)
- [pert_file_write](#pert_file_write) (function)
- [pert_file_write_bin](#pert_file_write_bin) (function)
- [pert_file_append](#pert_file_append) (function)
- [pert_file_exists](#pert_file_exists) (function)
- [pert_file_delete](#pert_file_delete) (function)
- [pert_memset](#pert_memset) (function)
- [pert_str_zeroize](#pert_str_zeroize) (function)
- [pert_vec_zeroize](#pert_vec_zeroize) (function)
- [pert_sensitive_mark](#pert_sensitive_mark) (function)
- [pert_sensitive_remark](#pert_sensitive_remark) (function)
- [pert_sensitive_check](#pert_sensitive_check) (function)
- [pert_sensitive_unmark](#pert_sensitive_unmark) (function)
- [pert_sensitive_free](#pert_sensitive_free) (function)
- [pert_sensitive_wipes](#pert_sensitive_wipes) (function)

## pe_heap_thread_end

The OS thread running this ends (conc.fd pe_thread_main on Linux,
win.fd / darwin.fd after their thread's last free): its cached spans
are unmapped, the blocks other threads freed to it come onto its lists,
its chunk is retired (the rest cut into its lists), its empty chunks
unmapped, its lists go to the pool and its heap is abandoned (RT8:
kept, with its id, for the frees of its blocks still in use and for the
next thread that starts). An allocation on this thread afterwards takes
a heap again (pe_heap_attach).

```fides
pub fn pe_heap_thread_end() {
```

*Defined at line 622*

---

## pert_malloc

p_malloc: `size` bytes that read zero. RT8 fast path (one function, no
call but nat_tls_get once threads run): a block of up to PE_FAST_MAX
bytes off the head of this thread's list of its class (PE_H_CLS),
zeroed in the bytes asked for (to 16); anything else is pe_alloc_slow.
Written with raw loads and stores, nested ifs and shifts, not pe_ld /
pe_st / pe_magic(): the mid-end inlines only what main reaches, which
the runtime functions a builtin runs are not, so each of those would
stay a call here.

```fides
pub fn pert_malloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 1164*

---

## pert_malloc_uninit

p_malloc_uninit (RT8): a block exactly as pert_malloc's (the same kind,
freed by pert_free, the same --leaks / --checked-alloc / FID_MEM_LIMIT
treatment) whose bytes are unspecified: for core's producers that write
every byte they hand out (strings, Vec buffers). The fast path is
pert_malloc's without the zeroing; a block known zero (fresh pages, a
debug block) is handed out as it is.

```fides
pub fn pert_malloc_uninit(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 1272*

---

## pert_alloc

```fides
pub fn pert_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 2042*

---

## pert_free

p_free. RT8 fast path (one function, as pert_malloc's, raw loads and
stores): a block this thread's heap handed out (its header is the
heap's PE_H_LIVE plus its class: one compare) of a class not decommitted
on free goes on the head of the heap's list; anything else is
pe_free_raw.

```fides
pub fn pert_free(p: i64) {
```

**Parameters:**

- `p`: `i64`

*Defined at line 2054*

---

## pert_mem_resident

RT7: the bytes of the process in physical memory: the second field of
/proc/self/statm (resident pages, 4096 bytes each here), -1 where there
is no such file (the macOS and WASI layers). On --nat Windows, the
working set (win.fd pe_win_mem_resident).

```fides
pub fn pert_mem_resident() -> i64 {
```

**Returns:** `i64`

*Defined at line 2215*

---

## pert_print_str

```fides
pub fn pert_print_str(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 2291*

---

## pert_eprint_str

```fides
pub fn pert_eprint_str(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 2298*

---

## pert_print_newline

```fides
pub fn pert_print_newline() {
```

*Defined at line 2305*

---

## pert_print_int

```fides
pub fn pert_print_int(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2309*

---

## pert_print_bool

```fides
pub fn pert_print_bool(b: bool) {
```

**Parameters:**

- `b`: `bool`

*Defined at line 2316*

---

## pert_print_float

```fides
pub fn pert_print_float(val: f64) {
```

**Parameters:**

- `val`: `f64`

*Defined at line 2324*

---

## pert_flush_stdout

```fides
pub fn pert_flush_stdout() {
```

*Defined at line 2330*

---

## pert_panic

```fides
pub fn pert_panic(msg: i64) {
```

**Parameters:**

- `msg`: `i64`

*Defined at line 2333*

---

## pert_inb

```fides
pub fn pert_inb(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 2358*

---

## pert_inw

```fides
pub fn pert_inw(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 2363*

---

## pert_inl

```fides
pub fn pert_inl(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 2368*

---

## pert_outb

```fides
pub fn pert_outb(port: i64, v: i64) {
```

**Parameters:**

- `port`: `i64`
- `v`: `i64`

*Defined at line 2373*

---

## pert_outw

```fides
pub fn pert_outw(port: i64, v: i64) {
```

**Parameters:**

- `port`: `i64`
- `v`: `i64`

*Defined at line 2377*

---

## pert_outl

```fides
pub fn pert_outl(port: i64, v: i64) {
```

**Parameters:**

- `port`: `i64`
- `v`: `i64`

*Defined at line 2381*

---

## pert_cli

```fides
pub fn pert_cli() {
```

*Defined at line 2385*

---

## pert_sti

```fides
pub fn pert_sti() {
```

*Defined at line 2389*

---

## pert_hlt

```fides
pub fn pert_hlt() {
```

*Defined at line 2393*

---

## pert_serial_init

```fides
pub fn pert_serial_init() {
```

*Defined at line 2397*

---

## pert_serial_write

```fides
pub fn pert_serial_write(c: i64) {
```

**Parameters:**

- `c`: `i64`

*Defined at line 2400*

---

## pert_serial_read

```fides
pub fn pert_serial_read() -> i64 {
```

**Returns:** `i64`

*Defined at line 2403*

---

## pert_serial_ready

```fides
pub fn pert_serial_ready() -> i64 {
```

**Returns:** `i64`

*Defined at line 2407*

---

## pert_exit

```fides
pub fn pert_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 2411*

---

## pert_exit_now

Exit immediately without running the entry-point cleanup a second time.

```fides
pub fn pert_exit_now(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 2421*

---

## pe_rt_exit

main's return (nat_ir.fd's Linux `_start` calls it with main's
result): the profiler's report (sys.fd, DB9), --leaks' report (RL16),
then exit_group.

```fides
pub fn pe_rt_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 2428*

---

## pe_rt_init

```fides
pub fn pe_rt_init() {
```

*Defined at line 2556*

---

## pe_on_signal

The signal handler (nat_sig_handler): a registered signal is noted
(p_signal_check); anything else is a panic like any other (reference
§8.2, the same text on every backend): "Panic: Stack overflow" when the
address is in a guard region, "Panic: Invalid memory access (null or
dangling pointer) at 0x..." for another SIGSEGV or SIGBUS, else
"Panic: signal N (NAME)"; then the trace and exit status 1. Allocates
nothing.

```fides
pub fn pe_on_signal(sig: i64, info: i64) {
```

**Parameters:**

- `sig`: `i64`
- `info`: `i64`

*Defined at line 2624*

---

## pe_on_signal_uc

The Linux signal handlers' entry (nat_ir.fd ng_sig_fn passes the
ucontext too): a SIGPROF of the profiler is one sample (sys.fd
pe_prof_tick, DB9; one arriving after the report is dropped), anything
else goes to pe_on_signal.

```fides
pub fn pe_on_signal_uc(sig: i64, info: i64, uc: i64) {
```

**Parameters:**

- `sig`: `i64`
- `info`: `i64`
- `uc`: `i64`

*Defined at line 2724*

---

## pe_dbg_frame_print

CE8: the values of the failing frame, from frame fp0 up: the first
program function on the stack (runtime functions skipped, as in the
trace) whose code holds the return address and which has a record.
Nothing without a frame table (no -g) or before the runtime is set up.

```fides
pub fn pe_dbg_frame_print(fp0: i64) {
```

**Parameters:**

- `fp0`: `i64`

*Defined at line 3265*

---

## pert_vblk_copy

Copies the first n & ~15 bytes at src to dst (the areas do not
overlap); returns that count.

```fides
pub fn pert_vblk_copy(dst: i64, src: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3360*

---

## pert_vblk_diff

The offset of the first 16-byte block, among the first n & ~15 bytes,
in which a and b differ; n & ~15 when none does.

```fides
pub fn pert_vblk_diff(a: i64, b: i64, n: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3381*

---

## pert_vblk_cmpz

```fides
pub fn pert_vblk_cmpz(a: i64, b: i64, n: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3413*

---

## pert_vblk_find

```fides
pub fn pert_vblk_find(p: i64, n: i64, b: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `n`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 3448*

---

## pert_mem_copy

```fides
pub fn pert_mem_copy(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 3478*

---

## pert_str_copy_n

```fides
pub fn pert_str_copy_n(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 3513*

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

*Defined at line 3517*

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

*Defined at line 3574*

---

## pert_str_concat

```fides
pub fn pert_str_concat(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 3638*

---

## pert_str_join_n

BG251: p_str_join_n(parts): the strings of a Vec<str> one after the
other, as one counted string (pe_ir's PeirGen_gen_str_join_n, the C
runtime's): each part counted by its header (pe_len), a null part as "",
an empty Vec "".

```fides
pub fn pert_str_join_n(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 3655*

---

## pert_to_str

```fides
pub fn pert_to_str(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 3714*

---

## pert_to_str_bool

```fides
pub fn pert_to_str_bool(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 3722*

---

## pert_bool_to_str

```fides
pub fn pert_bool_to_str(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 3729*

---

## pert_to_str_str

```fides
pub fn pert_to_str_str(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 3733*

---

## pert_char_to_str

```fides
pub fn pert_char_to_str(c: i64) -> str {
```

**Parameters:**

- `c`: `i64`

**Returns:** `str`

*Defined at line 3747*

---

## pert_to_str_vec

```fides
pub fn pert_to_str_vec(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 3764*

---

## pert_print_vec

```fides
pub fn pert_print_vec(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3768*

---

## pert_print_map

```fides
pub fn pert_print_map(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 3774*

---

## pert_str_equal

```fides
pub fn pert_str_equal(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 3816*

---

## pert_str_compare

```fides
pub fn pert_str_compare(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 3848*

---

## pert_str_hash

```fides
pub fn pert_str_hash(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 3918*

---

## pert_str_to_int

```fides
pub fn pert_str_to_int(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 3986*

---

## pert_str_to_float

atof: sign, digits, fraction, exponent; correctly rounded, as strtod
(BG206). The digits read make an integer D and a power of ten E (the
value is D * 10^E); when E is negative the value is D / 10^-E, and the
leading bits of that quotient come from a division bit by bit. At most
800 significant digits are read exactly (a midpoint of two doubles has
at most 767); a non-zero digit beyond them only says "a little more",
which cannot turn a rounding around: such an input has over 490 fraction
digits, so the dropped part is under 10^-489, and the smallest gap that
could matter is a unit of the last digit read. inf on overflow, 0 on
underflow, the sign of a zero kept.

```fides
pub fn pert_str_to_float(s: i64) -> f64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `f64`

*Defined at line 4161*

---

## pert_to_str_float

```fides
pub fn pert_to_str_float(val: f64) -> str {
```

**Parameters:**

- `val`: `f64`

**Returns:** `str`

*Defined at line 4482*

---

## pert_float_to_str

```fides
pub fn pert_float_to_str(val: f64) -> str {
```

**Parameters:**

- `val`: `f64`

**Returns:** `str`

*Defined at line 4813*

---

## pert_sb_new

```fides
pub fn pert_sb_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 4819*

---

## pert_sb_append

```fides
pub fn pert_sb_append(sb: i64, s: i64) {
```

**Parameters:**

- `sb`: `i64`
- `s`: `i64`

*Defined at line 4827*

---

## pert_sb_to_str

```fides
pub fn pert_sb_to_str(sb: i64) -> str {
```

**Parameters:**

- `sb`: `i64`

**Returns:** `str`

*Defined at line 4845*

---

## pert_allocator_create

```fides
pub fn pert_allocator_create(arg: i64) -> i64 {
```

**Parameters:**

- `arg`: `i64`

**Returns:** `i64`

*Defined at line 4851*

---

## pert_allocator_alloc

```fides
pub fn pert_allocator_alloc(alloc: i64, size: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 4855*

---

## pert_allocator_free

```fides
pub fn pert_allocator_free(alloc: i64, ptr: i64) {
```

**Parameters:**

- `alloc`: `i64`
- `ptr`: `i64`

*Defined at line 4862*

---

## pert_allocator_stats

```fides
pub fn pert_allocator_stats(alloc: i64) -> i64 {
```

**Parameters:**

- `alloc`: `i64`

**Returns:** `i64`

*Defined at line 4866*

---

## pert_string_len

```fides
pub fn pert_string_len(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 4872*

---

## pert_string_find

```fides
pub fn pert_string_find(s: i64, sub: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `i64`

*Defined at line 4894*

---

## pert_string_contains

```fides
pub fn pert_string_contains(s: i64, sub: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `bool`

*Defined at line 4901*

---

## pert_string_starts_with

```fides
pub fn pert_string_starts_with(s: i64, prefix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `prefix`: `i64`

**Returns:** `bool`

*Defined at line 4908*

---

## pert_string_ends_with

```fides
pub fn pert_string_ends_with(s: i64, suffix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `suffix`: `i64`

**Returns:** `bool`

*Defined at line 4927*

---

## pert_string_replace

```fides
pub fn pert_string_replace(s: i64, from: i64, to: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `from`: `i64`
- `to`: `i64`

**Returns:** `str`

*Defined at line 4946*

---

## pert_string_trim

```fides
pub fn pert_string_trim(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 4977*

---

## pert_string_to_uppercase

```fides
pub fn pert_string_to_uppercase(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 4993*

---

## pert_string_to_lowercase

```fides
pub fn pert_string_to_lowercase(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 5007*

---

## pert_string_from

p_string_from(s): a new string with the bytes of s ("" for null).

```fides
pub fn pert_string_from(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 5022*

---

## pert_string_substring

p_string_substring(s, start, end): the C runtime's p_string_slice
(start clamped to 0, end to the length, empty when start > end).

```fides
pub fn pert_string_substring(s: i64, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 5031*

---

## pert_stdin_eof

```fides
pub fn pert_stdin_eof() -> i64 {
```

**Returns:** `i64`

*Defined at line 5059*

---

## pert_read_line

```fides
pub fn pert_read_line() -> str {
```

**Returns:** `str`

*Defined at line 5063*

---

## pert_read_stdin

```fides
pub fn pert_read_stdin(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 5103*

---

## pert_string_split

The pieces of s between the delimiters (a trailing empty piece is not
kept, as in the C runtime).

```fides
pub fn pert_string_split(s: i64, delim: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `delim`: `i64`

**Returns:** `i64`

*Defined at line 5129*

---

## pert_argc

```fides
pub fn pert_argc() -> i64 {
```

**Returns:** `i64`

*Defined at line 5155*

---

## pert_argv

```fides
pub fn pert_argv(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 5159*

---

## pert_getenv

```fides
pub fn pert_getenv(name: i64) -> str {
```

**Parameters:**

- `name`: `i64`

**Returns:** `str`

*Defined at line 5172*

---

## pert_getpid

```fides
pub fn pert_getpid() -> i64 {
```

**Returns:** `i64`

*Defined at line 5193*

---

## pert_clock_now

```fides
pub fn pert_clock_now() -> i64 {
```

**Returns:** `i64`

*Defined at line 5206*

---

## pert_clock_now_us

```fides
pub fn pert_clock_now_us() -> i64 {
```

**Returns:** `i64`

*Defined at line 5210*

---

## pert_clock_now_ms

```fides
pub fn pert_clock_now_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 5214*

---

## pert_clock_mono_ms

C6: milliseconds on CLOCK_MONOTONIC (1), the clock of deadlines
(timeout blocks, tokens): a change of the wall clock does not move it.
On --nat Windows win.fd answers clock 1 from QueryPerformanceCounter.

```fides
pub fn pert_clock_mono_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 5221*

---

## pert_region_word

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

*Defined at line 5232*

---

## pert_os_random_u64

```fides
pub fn pert_os_random_u64() -> i64 {
```

**Returns:** `i64`

*Defined at line 5236*

---

## pert_file_read

```fides
pub fn pert_file_read(path: i64) -> str {
```

**Parameters:**

- `path`: `i64`

**Returns:** `str`

*Defined at line 5250*

---

## pert_file_write

```fides
pub fn pert_file_write(path: i64, content: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`
- `content`: `i64`

**Returns:** `i64`

*Defined at line 5310*

---

## pert_file_write_bin

```fides
pub fn pert_file_write_bin(path: i64, data: i64, n: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`
- `data`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 5314*

---

## pert_file_append

```fides
pub fn pert_file_append(path: i64, content: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`
- `content`: `i64`

**Returns:** `i64`

*Defined at line 5321*

---

## pert_file_exists

```fides
pub fn pert_file_exists(path: i64) -> bool {
```

**Parameters:**

- `path`: `i64`

**Returns:** `bool`

*Defined at line 5325*

---

## pert_file_delete

```fides
pub fn pert_file_delete(path: i64) -> i64 {
```

**Parameters:**

- `path`: `i64`

**Returns:** `i64`

*Defined at line 5337*

---

## pert_memset

```fides
pub fn pert_memset(dst: i64, v: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `v`: `i64`
- `n`: `i64`

*Defined at line 5354*

---

## pert_str_zeroize

```fides
pub fn pert_str_zeroize(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 5375*

---

## pert_vec_zeroize

```fides
pub fn pert_vec_zeroize(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 5386*

---

## pert_sensitive_mark

```fides
pub fn pert_sensitive_mark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 5434*

---

## pert_sensitive_remark

```fides
pub fn pert_sensitive_remark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 5445*

---

## pert_sensitive_check

```fides
pub fn pert_sensitive_check(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 5455*

---

## pert_sensitive_unmark

```fides
pub fn pert_sensitive_unmark(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 5462*

---

## pert_sensitive_free

```fides
pub fn pert_sensitive_free(v: i64, buf: i64, cap: i64) {
```

**Parameters:**

- `v`: `i64`
- `buf`: `i64`
- `cap`: `i64`

*Defined at line 5476*

---

## pert_sensitive_wipes

```fides
pub fn pert_sensitive_wipes() -> i64 {
```

**Returns:** `i64`

*Defined at line 5493*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
