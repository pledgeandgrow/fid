[All modules](index.md)

# sys -- Fides Documentation

## Contents

- [pert_file_size](#pert_file_size) (function)
- [pert_file_read_bytes](#pert_file_read_bytes) (function)
- [pert_file_write_vec](#pert_file_write_vec) (function)
- [pert_file_hash](#pert_file_hash) (function)
- [pert_mmap_anon](#pert_mmap_anon) (function)
- [pert_mmap_len](#pert_mmap_len) (function)
- [pert_munmap](#pert_munmap) (function)
- [pert_file_read_at](#pert_file_read_at) (function)
- [pert_file_append_vec](#pert_file_append_vec) (function)
- [pert_file_mtime_ms](#pert_file_mtime_ms) (function)
- [pert_is_dir](#pert_is_dir) (function)
- [pert_rename](#pert_rename) (function)
- [pert_mkdir](#pert_mkdir) (function)
- [pert_rmdir](#pert_rmdir) (function)
- [pert_dir_list](#pert_dir_list) (function)
- [pert_regex_match](#pert_regex_match) (function)
- [pert_volatile_load](#pert_volatile_load) (function)
- [pert_volatile_store](#pert_volatile_store) (function)
- [pert_volatile_load8](#pert_volatile_load8) (function)
- [pert_volatile_store8](#pert_volatile_store8) (function)
- [pert_volatile_load32](#pert_volatile_load32) (function)
- [pert_volatile_store32](#pert_volatile_store32) (function)
- [pert_c_exit](#pert_c_exit) (function)
- [pert_c_abort](#pert_c_abort) (function)
- [pert_c_getenv](#pert_c_getenv) (function)
- [pert_c_getpid](#pert_c_getpid) (function)
- [pert_c_getppid](#pert_c_getppid) (function)
- [pert_c_getuid](#pert_c_getuid) (function)
- [pert_c_getgid](#pert_c_getgid) (function)
- [pert_c_clock_gettime](#pert_c_clock_gettime) (function)
- [pert_c_getpagesize](#pert_c_getpagesize) (function)
- [pert_c_putchar](#pert_c_putchar) (function)
- [pert_c_puts](#pert_c_puts) (function)
- [pert_c_printf](#pert_c_printf) (function)
- [pert_c_getchar](#pert_c_getchar) (function)
- [pert_c_strlen](#pert_c_strlen) (function)
- [pert_c_strcmp](#pert_c_strcmp) (function)
- [pert_c_abs](#pert_c_abs) (function)
- [pert_c_labs](#pert_c_labs) (function)
- [pert_c_sin](#pert_c_sin) (function)
- [pert_c_cos](#pert_c_cos) (function)
- [pert_c_tan](#pert_c_tan) (function)
- [pert_socket_listen](#pert_socket_listen) (function)
- [pert_unix_listen](#pert_unix_listen) (function)
- [pert_unix_connect](#pert_unix_connect) (function)
- [pert_socket_accept](#pert_socket_accept) (function)
- [pert_socket_read](#pert_socket_read) (function)
- [pert_socket_write](#pert_socket_write) (function)
- [pert_socket_write_str](#pert_socket_write_str) (function)
- [pert_socket_close](#pert_socket_close) (function)
- [pert_net_close](#pert_net_close) (function)
- [pert_socket_recv_str](#pert_socket_recv_str) (function)
- [pert_c_pledge_net_socket](#pert_c_pledge_net_socket) (function)
- [pert_c_pledge_net_close](#pert_c_pledge_net_close) (function)
- [pert_c_pledge_net_inet_addr](#pert_c_pledge_net_inet_addr) (function)
- [pert_c_pledge_net_bind](#pert_c_pledge_net_bind) (function)
- [pert_c_pledge_net_set_nonblocking](#pert_c_pledge_net_set_nonblocking) (function)
- [pert_c_pledge_net_set_recv_timeout](#pert_c_pledge_net_set_recv_timeout) (function)
- [pert_c_pledge_net_sendto](#pert_c_pledge_net_sendto) (function)
- [pert_c_pledge_net_recvfrom](#pert_c_pledge_net_recvfrom) (function)
- [pert_c_pledge_net_connect](#pert_c_pledge_net_connect) (function)
- [pert_c_pledge_net_send](#pert_c_pledge_net_send) (function)
- [pert_c_pledge_net_recv](#pert_c_pledge_net_recv) (function)
- [pert_net_connect](#pert_net_connect) (function)
- [pert_co_accept](#pert_co_accept) (function)
- [pert_co_connect](#pert_co_connect) (function)
- [pert_co_recv](#pert_co_recv) (function)
- [pert_co_send](#pert_co_send) (function)
- [pert_fs_copy_file](#pert_fs_copy_file) (function)
- [pert_net_send_file](#pert_net_send_file) (function)
- [pert_fs_map](#pert_fs_map) (function)
- [pert_fs_unmap](#pert_fs_unmap) (function)
- [pert_system](#pert_system) (function)
- [pert_c_system](#pert_c_system) (function)
- [pert_process_run](#pert_process_run) (function)
- [pert_process_stdout](#pert_process_stdout) (function)
- [pert_process_stderr](#pert_process_stderr) (function)
- [pert_process_code](#pert_process_code) (function)
- [pert_process_close](#pert_process_close) (function)
- [pert_c_fid_proc_spawn](#pert_c_fid_proc_spawn) (function)
- [pert_c_fid_proc_spawn_io](#pert_c_fid_proc_spawn_io) (function)
- [pert_c_fid_proc_wait](#pert_c_fid_proc_wait) (function)
- [pert_c_fid_proc_code](#pert_c_fid_proc_code) (function)
- [pert_c_fid_proc_kill](#pert_c_fid_proc_kill) (function)
- [pert_c_fid_proc_pid](#pert_c_fid_proc_pid) (function)
- [pert_c_fid_proc_free](#pert_c_fid_proc_free) (function)
- [pert_c_fid_proc_write](#pert_c_fid_proc_write) (function)
- [pert_c_fid_proc_read](#pert_c_fid_proc_read) (function)
- [pert_c_fid_proc_close_in](#pert_c_fid_proc_close_in) (function)
- [pert_c_sysconf](#pert_c_sysconf) (function)
- [pert_signal_register](#pert_signal_register) (function)
- [pert_signal_check](#pert_signal_check) (function)
- [pert_signal_raise](#pert_signal_raise) (function)
- [pert_c_raise](#pert_c_raise) (function)
- [pert_c_pledge_asset_load_texture](#pert_c_pledge_asset_load_texture) (function)
- [pert_c_pledge_asset_load_sound](#pert_c_pledge_asset_load_sound) (function)
- [pert_c_pledge_asset_load_font](#pert_c_pledge_asset_load_font) (function)
- [pert_c_pledge_asset_load_mesh](#pert_c_pledge_asset_load_mesh) (function)
- [pert_c_pledge_asset_unload](#pert_c_pledge_asset_unload) (function)
- [pert_c_pledge_asset_mtime](#pert_c_pledge_asset_mtime) (function)
- [pert_c_qsort](#pert_c_qsort) (function)
- [pert_c_bsearch](#pert_c_bsearch) (function)
- [pert_c_malloc](#pert_c_malloc) (function)
- [pert_c_free](#pert_c_free) (function)
- [pert_c_calloc](#pert_c_calloc) (function)
- [pert_c_memcpy](#pert_c_memcpy) (function)
- [pert_c_memmove](#pert_c_memmove) (function)
- [pert_c_memset](#pert_c_memset) (function)
- [pert_c_memcmp](#pert_c_memcmp) (function)
- [pert_c__snprintf](#pert_c__snprintf) (function)
- [pert_arena_save](#pert_arena_save) (function)
- [pert_arena_restore](#pert_arena_restore) (function)
- [pert_region_free](#pert_region_free) (function)
- [pert_dl_open](#pert_dl_open) (function)
- [pert_dl_sym](#pert_dl_sym) (function)
- [pert_dl_close](#pert_dl_close) (function)
- [pert_dl_call](#pert_dl_call) (function)
- [pert_dl_load](#pert_dl_load) (function)
- [pert_dl_find](#pert_dl_find) (function)
- [pert_dl_unload](#pert_dl_unload) (function)
- [pert_simd_add](#pert_simd_add) (function)
- [pert_simd_sub](#pert_simd_sub) (function)
- [pert_simd_mul](#pert_simd_mul) (function)
- [pert_simd_div](#pert_simd_div) (function)
- [pert_simd_dot](#pert_simd_dot) (function)
- [pert_simd_sum](#pert_simd_sum) (function)
- [pert_simd_scale](#pert_simd_scale) (function)
- [pert_simd_from_vec](#pert_simd_from_vec) (function)
- [pert_simd_to_str](#pert_simd_to_str) (function)
- [pert_ecs_create_entity](#pert_ecs_create_entity) (function)
- [pert_ecs_entity_count](#pert_ecs_entity_count) (function)
- [pert_ecs_has](#pert_ecs_has) (function)
- [pert_ecs_get](#pert_ecs_get) (function)
- [pert_ecs_add](#pert_ecs_add) (function)
- [pert_ecs_remove](#pert_ecs_remove) (function)
- [pe_prof_watch_main](#pe_prof_watch_main) (function)
- [pert_prof_start](#pert_prof_start) (function)
- [pert_prof_stop](#pert_prof_stop) (function)
- [pert_prof_report](#pert_prof_report) (function)
- [pert_prof_samples](#pert_prof_samples) (function)

## pert_file_size

```fides
pub fn pert_file_size(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 26*

---

## pert_file_read_bytes

Every byte of the file as one element; empty if unreadable.

```fides
pub fn pert_file_read_bytes(path: str) -> Vec<i64> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<i64>`

*Defined at line 49*

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

*Defined at line 74*

---

## pert_file_hash

64-bit FNV-1a of the file's bytes; 0 if it cannot be read.

```fides
pub fn pert_file_hash(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 88*

---

## pert_mmap_anon

`len` zeroed read/write bytes of their own pages; 0 on failure.

```fides
pub fn pert_mmap_anon(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 140*

---

## pert_mmap_len

The length of mapping `addr`, 0 when it is not one.

```fides
pub fn pert_mmap_len(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 163*

---

## pert_munmap

Unmap `addr`: 0, or -1 when it is not a mapping.

```fides
pub fn pert_munmap(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 175*

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

*Defined at line 196*

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

*Defined at line 242*

---

## pert_file_mtime_ms

The time the file was last written, in ms since 1970; -1 when it does
not exist (as the C runtime's p_file_mtime_ms). Every OS layer's
newfstatat fills st_mtime at 88 and its nanoseconds at 96 (win.fd,
darwin.fd and wasi.fd copy them there).

```fides
pub fn pert_file_mtime_ms(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 281*

---

## pert_is_dir

```fides
pub fn pert_is_dir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 319*

---

## pert_rename

```fides
pub fn pert_rename(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 324*

---

## pert_mkdir

```fides
pub fn pert_mkdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 334*

---

## pert_rmdir

Remove an empty directory.

```fides
pub fn pert_rmdir(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 345*

---

## pert_dir_list

Entry names of a directory in byte order joined by '\n' (no "." or
".."); "" on failure.

```fides
pub fn pert_dir_list(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 374*

---

## pert_regex_match

```fides
pub fn pert_regex_match(s: str, pattern: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 445*

---

## pert_volatile_load

```fides
pub fn pert_volatile_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 481*

---

## pert_volatile_store

```fides
pub fn pert_volatile_store(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 485*

---

## pert_volatile_load8

```fides
pub fn pert_volatile_load8(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 489*

---

## pert_volatile_store8

```fides
pub fn pert_volatile_store8(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 493*

---

## pert_volatile_load32

```fides
pub fn pert_volatile_load32(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 497*

---

## pert_volatile_store32

```fides
pub fn pert_volatile_store32(addr: i64, val: i64) {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

*Defined at line 508*

---

## pert_c_exit

```fides
pub fn pert_c_exit(code: i64) {
```

**Parameters:**

- `code`: `i64`

*Defined at line 519*

---

## pert_c_abort

```fides
pub fn pert_c_abort() {
```

*Defined at line 523*

---

## pert_c_getenv

```fides
pub fn pert_c_getenv(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 527*

---

## pert_c_getpid

```fides
pub fn pert_c_getpid() -> i64 {
```

**Returns:** `i64`

*Defined at line 531*

---

## pert_c_getppid

```fides
pub fn pert_c_getppid() -> i64 {
```

**Returns:** `i64`

*Defined at line 535*

---

## pert_c_getuid

```fides
pub fn pert_c_getuid() -> i64 {
```

**Returns:** `i64`

*Defined at line 539*

---

## pert_c_getgid

```fides
pub fn pert_c_getgid() -> i64 {
```

**Returns:** `i64`

*Defined at line 543*

---

## pert_c_clock_gettime

clock_gettime(clock, ts): the system call (0, or -errno where C gives
-1). The compiler's interpreter reads its process CPU-time clock (2)
through it for FID_CPU_LIMIT under `fid run` (interp.fd interp_cpu_ms),
so a natively built fid needs no C library for it.

```fides
pub fn pert_c_clock_gettime(clock: i64, ts: i64) -> i64 {
```

**Parameters:**

- `clock`: `i64`
- `ts`: `i64`

**Returns:** `i64`

*Defined at line 551*

---

## pert_c_getpagesize

```fides
pub fn pert_c_getpagesize() -> i64 {
```

**Returns:** `i64`

*Defined at line 555*

---

## pert_c_putchar

```fides
pub fn pert_c_putchar(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 559*

---

## pert_c_puts

```fides
pub fn pert_c_puts(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 567*

---

## pert_c_printf

The format alone: nothing is substituted ("%%" prints "%").

```fides
pub fn pert_c_printf(fmt: str) -> i64 {
```

**Parameters:**

- `fmt`: `str`

**Returns:** `i64`

*Defined at line 574*

---

## pert_c_getchar

```fides
pub fn pert_c_getchar() -> i64 {
```

**Returns:** `i64`

*Defined at line 591*

---

## pert_c_strlen

```fides
pub fn pert_c_strlen(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 602*

---

## pert_c_strcmp

```fides
pub fn pert_c_strcmp(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 614*

---

## pert_c_abs

```fides
pub fn pert_c_abs(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 618*

---

## pert_c_labs

```fides
pub fn pert_c_labs(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 625*

---

## pert_c_sin

```fides
pub fn pert_c_sin(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 726*

---

## pert_c_cos

```fides
pub fn pert_c_cos(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 730*

---

## pert_c_tan

```fides
pub fn pert_c_tan(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 734*

---

## pert_socket_listen

A TCP socket on all addresses, listening, or -1: IPv6 and IPv4 clients
on one socket (IPV6_V6ONLY off), IPv4 alone without IPv6.

```fides
pub fn pert_socket_listen(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 1384*

---

## pert_unix_listen

A socket listening at `path`, or -1.

```fides
pub fn pert_unix_listen(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1429*

---

## pert_unix_connect

A socket connected to the listener at `path`, or -1.

```fides
pub fn pert_unix_connect(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 1434*

---

## pert_socket_accept

```fides
pub fn pert_socket_accept(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 1470*

---

## pert_socket_read

```fides
pub fn pert_socket_read(fd: i64, buf: i64, n: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 1481*

---

## pert_socket_write

```fides
pub fn pert_socket_write(fd: i64, data: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `data`: `str`

**Returns:** `i64`

*Defined at line 1493*

---

## pert_socket_write_str

```fides
pub fn pert_socket_write_str(fd: i64, data: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `data`: `str`

**Returns:** `i64`

*Defined at line 1510*

---

## pert_socket_close

```fides
pub fn pert_socket_close(fd: i64) {
```

**Parameters:**

- `fd`: `i64`

*Defined at line 1514*

---

## pert_net_close

```fides
pub fn pert_net_close(fd: i64) {
```

**Parameters:**

- `fd`: `i64`

*Defined at line 1519*

---

## pert_socket_recv_str

Up to 4095 bytes received, as a string ("" when none).

```fides
pub fn pert_socket_recv_str(fd: i64) -> str {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `str`

*Defined at line 1525*

---

## pert_c_pledge_net_socket

```fides
pub fn pert_c_pledge_net_socket(domain: i64, ty: i64, protocol: i64) -> i64 {
```

**Parameters:**

- `domain`: `i64`
- `ty`: `i64`
- `protocol`: `i64`

**Returns:** `i64`

*Defined at line 1551*

---

## pert_c_pledge_net_close

```fides
pub fn pert_c_pledge_net_close(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 1555*

---

## pert_c_pledge_net_inet_addr

inet_addr: "a.b.c.d" in network order, INADDR_NONE (0xFFFFFFFF) when it
is not one.

```fides
pub fn pert_c_pledge_net_inet_addr(ip_str: str) -> i64 {
```

**Parameters:**

- `ip_str`: `str`

**Returns:** `i64`

*Defined at line 1562*

---

## pert_c_pledge_net_bind

```fides
pub fn pert_c_pledge_net_bind(sock: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1573*

---

## pert_c_pledge_net_set_nonblocking

```fides
pub fn pert_c_pledge_net_set_nonblocking(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 1584*

---

## pert_c_pledge_net_set_recv_timeout

```fides
pub fn pert_c_pledge_net_set_recv_timeout(sock: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 1592*

---

## pert_c_pledge_net_sendto

```fides
pub fn pert_c_pledge_net_sendto(sock: i64, data: i64, len: i64, ip: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`
- `ip`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1601*

---

## pert_c_pledge_net_recvfrom

A datagram into buf; when from_addr is given (a NetAddr: ip, port, raw)
the sender's IPv4 address and port go there, as pledge_net.c does.

```fides
pub fn pert_c_pledge_net_recvfrom(sock: i64, buf: i64, len: i64, from_addr: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`
- `from_addr`: `i64`

**Returns:** `i64`

*Defined at line 1610*

---

## pert_c_pledge_net_connect

```fides
pub fn pert_c_pledge_net_connect(sock: i64, ip: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `ip`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1628*

---

## pert_c_pledge_net_send

```fides
pub fn pert_c_pledge_net_send(sock: i64, data: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 1635*

---

## pert_c_pledge_net_recv

```fides
pub fn pert_c_pledge_net_recv(sock: i64, buf: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 1639*

---

## pert_net_connect

A TCP connection to host:port, or -1. Every address of the host is
tried in turn until one connects (competitive SL5): the IPv4 and then
the IPv6 address a literal or /etc/hosts gives; without one, every A
answer of DNS and then every AAAA answer (at most 8 of each). On
`--nat` Windows, every address getaddrinfo gives (pe_win_connect_all).

```fides
pub fn pert_net_connect(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1648*

---

## pert_co_accept

```fides
pub fn pert_co_accept(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 1794*

---

## pert_co_connect

```fides
pub fn pert_co_connect(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1810*

---

## pert_co_recv

Bytes received (0 at the end), -1 on error, -2 when timeout_ms passed.

```fides
pub fn pert_co_recv(fd: i64, buf: i64, n: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 1819*

---

## pert_co_send

```fides
pub fn pert_co_send(fd: i64, buf: i64, n: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 1835*

---

## pert_fs_copy_file

p_fs_copy_file: `src` to `dst` (created or emptied first; a file copied
onto itself is left as it is): the bytes copied, or -1.

```fides
pub fn pert_fs_copy_file(src: str, dst: str) -> i64 {
```

**Parameters:**

- `src`: `str`
- `dst`: `str`

**Returns:** `i64`

*Defined at line 1973*

---

## pert_net_send_file

p_net_send_file: the file at `path` on the connected socket `sock`: the
bytes sent, or -1. In a task the socket is made non-blocking and the
task waits while it is full, as pert_co_send does.

```fides
pub fn pert_net_send_file(sock: i64, path: str) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 2031*

---

## pert_fs_map

p_fs_map (fs.fd FileView): `size` bytes of the file `path` mapped
read-only, or 0 (the file is not that size now, or cannot be mapped).
Linux only: win.fd and darwin.fd give anonymous memory for every mmap,
whatever the descriptor, so there FileView reads the file instead.

```fides
pub fn pert_fs_map(path: str, size: i64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 2081*

---

## pert_fs_unmap

p_fs_unmap: undo pert_fs_map: 0, or -1.

```fides
pub fn pert_fs_unmap(addr: i64, size: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 2105*

---

## pert_system

```fides
pub fn pert_system(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 2155*

---

## pert_c_system

```fides
pub fn pert_c_system(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 2162*

---

## pert_process_run

Run a shell command capturing stdout, stderr and the exit status: the
handle is [out, err, code]; 0 if it could not start (as Windows does,
through temporary files).

```fides
pub fn pert_process_run(cmd: str) -> i64 {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `i64`

*Defined at line 2169*

---

## pert_process_stdout

```fides
pub fn pert_process_stdout(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 2225*

---

## pert_process_stderr

```fides
pub fn pert_process_stderr(h: i64) -> str {
```

**Parameters:**

- `h`: `i64`

**Returns:** `str`

*Defined at line 2232*

---

## pert_process_code

```fides
pub fn pert_process_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2239*

---

## pert_process_close

```fides
pub fn pert_process_close(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 2246*

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

*Defined at line 2357*

---

## pert_c_fid_proc_spawn_io

roadmap L4H (stdlib/process.fd process_spawn_io, Command::spawn): io bits
1 / 2 / 4 give the child a pipe as its stdin / stdout / stderr
(pert_c_fid_proc_write / _read / _close_in), 8 sends its stderr where
its stdout goes; a piped stream takes no file. Linux and macOS: each
"pipe" is a connected AF_UNIX stream pair (pe_px_pairs: nat_ir's x86-64
system call map has no pipe2 or dup3), placed at the child's descriptor
0 / 1 / 2 with fcntl(F_DUPFD) between fork and exec. --nat Windows:
CreatePipe pairs whose child ends are its standard handles (win.fd
pe_win_spawn, pe_win_pipes); this process's ends are HANDLEs.

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

*Defined at line 2370*

---

## pert_c_fid_proc_wait

1 once the child has ended (its code kept), 0 while it is still running
after timeout_ms (< 0: no limit), -1 for a bad handle or an error.

```fides
pub fn pert_c_fid_proc_wait(h: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 2680*

---

## pert_c_fid_proc_code

Its exit code once pert_c_fid_proc_wait said it ended (killed by signal
n: 128 + n; Windows: 137), else -1.

```fides
pub fn pert_c_fid_proc_code(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2743*

---

## pert_c_fid_proc_kill

SIGKILL (Windows: TerminateProcess with 137): 1 when asked, 0 when it
had ended or could not be stopped.

```fides
pub fn pert_c_fid_proc_kill(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2755*

---

## pert_c_fid_proc_pid

```fides
pub fn pert_c_fid_proc_pid(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2771*

---

## pert_c_fid_proc_free

Forget the handle (a child still running keeps running; POSIX: one not
waited for stays a zombie until this process ends).

```fides
pub fn pert_c_fid_proc_free(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2780*

---

## pert_c_fid_proc_write

roadmap L4H: all n bytes at data to the child's stdin pipe: n, or -1
when it has none or the child closed it (MSG_NOSIGNAL: no SIGPIPE;
macOS: SO_NOSIGPIPE, darwin.fd).

```fides
pub fn pert_c_fid_proc_write(h: i64, data: i64, n: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `data`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 2795*

---

## pert_c_fid_proc_read

Up to max bytes the child wrote to its stdout (which 1) or stderr (2)
pipe, into buf, waiting for at least one: 0 once it is closed and empty
(the child ended), -1 when there is no such pipe.

```fides
pub fn pert_c_fid_proc_read(h: i64, which: i64, buf: i64, max: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`
- `which`: `i64`
- `buf`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 2826*

---

## pert_c_fid_proc_close_in

Close our end of the child's stdin pipe: it reads the end of its input.

```fides
pub fn pert_c_fid_proc_close_in(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 2848*

---

## pert_c_sysconf

sysconf: CPUs, the page size, clock ticks, memory pages.

```fides
pub fn pert_c_sysconf(name: i64) -> i64 {
```

**Parameters:**

- `name`: `i64`

**Returns:** `i64`

*Defined at line 2857*

---

## pert_signal_register

```fides
pub fn pert_signal_register(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 2886*

---

## pert_signal_check

```fides
pub fn pert_signal_check(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 2897*

---

## pert_signal_raise

```fides
pub fn pert_signal_raise(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 2906*

---

## pert_c_raise

```fides
pub fn pert_c_raise(sig: i64) -> i64 {
```

**Parameters:**

- `sig`: `i64`

**Returns:** `i64`

*Defined at line 2914*

---

## pert_c_pledge_asset_load_texture

```fides
pub fn pert_c_pledge_asset_load_texture(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2934*

---

## pert_c_pledge_asset_load_sound

```fides
pub fn pert_c_pledge_asset_load_sound(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2942*

---

## pert_c_pledge_asset_load_font

```fides
pub fn pert_c_pledge_asset_load_font(path: str, size: f64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `size`: `f64`

**Returns:** `i64`

*Defined at line 2950*

---

## pert_c_pledge_asset_load_mesh

```fides
pub fn pert_c_pledge_asset_load_mesh(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2958*

---

## pert_c_pledge_asset_unload

```fides
pub fn pert_c_pledge_asset_unload(handle: i64, asset_type: i64) {
```

**Parameters:**

- `handle`: `i64`
- `asset_type`: `i64`

*Defined at line 2969*

---

## pert_c_pledge_asset_mtime

The time the file was last written, in seconds since 1970; 0 when it
cannot be read (pledge_asset.c: stat's st_mtime).

```fides
pub fn pert_c_pledge_asset_mtime(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2974*

---

## pert_c_qsort

Sort n elements of `size` bytes at base: a stable merge sort.

```fides
pub fn pert_c_qsort(base: i64, n: i64, size: i64, cmp: i64) {
```

**Parameters:**

- `base`: `i64`
- `n`: `i64`
- `size`: `i64`
- `cmp`: `i64`

*Defined at line 2994*

---

## pert_c_bsearch

```fides
pub fn pert_c_bsearch(key: i64, base: i64, n: i64, size: i64, cmp: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `base`: `i64`
- `n`: `i64`
- `size`: `i64`
- `cmp`: `i64`

**Returns:** `i64`

*Defined at line 3041*

---

## pert_c_malloc

```fides
pub fn pert_c_malloc(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 3063*

---

## pert_c_free

```fides
pub fn pert_c_free(p: i64) {
```

**Parameters:**

- `p`: `i64`

*Defined at line 3066*

---

## pert_c_calloc

```fides
pub fn pert_c_calloc(count: i64, size: i64) -> i64 {
```

**Parameters:**

- `count`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 3069*

---

## pert_c_memcpy

```fides
pub fn pert_c_memcpy(dst: i64, src: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3073*

---

## pert_c_memmove

```fides
pub fn pert_c_memmove(dst: i64, src: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3082*

---

## pert_c_memset

```fides
pub fn pert_c_memset(dst: i64, c: i64, n: i64) -> i64 {
```

**Parameters:**

- `dst`: `i64`
- `c`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3107*

---

## pert_c_memcmp

```fides
pub fn pert_c_memcmp(a: i64, b: i64, n: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 3112*

---

## pert_c__snprintf

_snprintf(buf, size, fmt, x) for the formats the shared runtime uses:
"%I64d" / "%lld" / "%d" (an integer) and "%.*f" (prec, then the float).

```fides
pub fn pert_c__snprintf(buf: i64, size: i64, fmt: str, a: i64, b: i64) -> i64 {
```

**Parameters:**

- `buf`: `i64`
- `size`: `i64`
- `fmt`: `str`
- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 3195*

---

## pert_arena_save

Heap arenas need no marks here: save gives 0, restore does nothing.

```fides
pub fn pert_arena_save() -> i64 {
```

**Returns:** `i64`

*Defined at line 3350*

---

## pert_arena_restore

```fides
pub fn pert_arena_restore(mark: i64) {
```

**Parameters:**

- `mark`: `i64`

*Defined at line 3353*

---

## pert_region_free

```fides
pub fn pert_region_free(r: i64) {
```

**Parameters:**

- `r`: `i64`

*Defined at line 3355*

---

## pert_dl_open

Simulated, as in the C backend and the interpreter: handles 1, 2, ...

```fides
pub fn pert_dl_open(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 3361*

---

## pert_dl_sym

```fides
pub fn pert_dl_sym(handle: i64, symbol: str) -> i64 {
```

**Parameters:**

- `handle`: `i64`
- `symbol`: `str`

**Returns:** `i64`

*Defined at line 3369*

---

## pert_dl_close

```fides
pub fn pert_dl_close(handle: i64) {
```

**Parameters:**

- `handle`: `i64`

*Defined at line 3376*

---

## pert_dl_call

A simulated symbol (pert_dl_sym: below 64 KiB, where no code is) gives
the argument back; on `--nat` Windows a real one (p_dl_find's) is
called as i64 -> i64 in the C convention (win.fd pe_win_dl_call), as
the C backend and pe_ir do. No other target of this runtime loads a
library, so it has no real symbols.

```fides
pub fn pert_dl_call(func: i64, arg: i64) -> i64 {
```

**Parameters:**

- `func`: `i64`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 3384*

---

## pert_dl_load

```fides
pub fn pert_dl_load(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 3402*

---

## pert_dl_find

```fides
pub fn pert_dl_find(handle: i64, name: str) -> i64 {
```

**Parameters:**

- `handle`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 3409*

---

## pert_dl_unload

```fides
pub fn pert_dl_unload(handle: i64) -> i64 {
```

**Parameters:**

- `handle`: `i64`

**Returns:** `i64`

*Defined at line 3416*

---

## pert_simd_add

```fides
pub fn pert_simd_add(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 3423*

---

## pert_simd_sub

```fides
pub fn pert_simd_sub(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 3433*

---

## pert_simd_mul

```fides
pub fn pert_simd_mul(a: Vec<f64>, b: Vec<f64>) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `Vec<f64>`

*Defined at line 3443*

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

*Defined at line 3454*

---

## pert_simd_dot

```fides
pub fn pert_simd_dot(a: Vec<f64>, b: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`
- `b`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 3469*

---

## pert_simd_sum

```fides
pub fn pert_simd_sum(a: Vec<f64>) -> f64 {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `f64`

*Defined at line 3479*

---

## pert_simd_scale

```fides
pub fn pert_simd_scale(a: Vec<f64>, k: f64) -> Vec<f64> {
```

**Parameters:**

- `a`: `Vec<f64>`
- `k`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 3489*

---

## pert_simd_from_vec

```fides
pub fn pert_simd_from_vec(v: Vec<i64>) -> Vec<f64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<f64>`

*Defined at line 3499*

---

## pert_simd_to_str

"[1, 2.5]": whole numbers as integers, others as %g.

```fides
pub fn pert_simd_to_str(a: Vec<f64>) -> str {
```

**Parameters:**

- `a`: `Vec<f64>`

**Returns:** `str`

*Defined at line 3510*

---

## pert_ecs_create_entity

```fides
pub fn pert_ecs_create_entity() -> i64 {
```

**Returns:** `i64`

*Defined at line 3555*

---

## pert_ecs_entity_count

```fides
pub fn pert_ecs_entity_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 3561*

---

## pert_ecs_has

```fides
pub fn pert_ecs_has(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 3565*

---

## pert_ecs_get

```fides
pub fn pert_ecs_get(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 3580*

---

## pert_ecs_add

```fides
pub fn pert_ecs_add(name: str, entity: i64, value: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 3595*

---

## pert_ecs_remove

```fides
pub fn pert_ecs_remove(name: str, entity: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 3610*

---

## pe_prof_watch_main

The watcher thread: a report on request or every FID_PROF_EVERY s.

```fides
pub fn pe_prof_watch_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 4421*

---

## pert_prof_start

p_prof_start(): count samples from now on (earlier ones are dropped):
0, or -1 when this program cannot be sampled.

```fides
pub fn pert_prof_start() -> i64 {
```

**Returns:** `i64`

*Defined at line 4563*

---

## pert_prof_stop

p_prof_stop(): stop counting; the samples taken.

```fides
pub fn pert_prof_stop() -> i64 {
```

**Returns:** `i64`

*Defined at line 4621*

---

## pert_prof_report

p_prof_report(): the profile so far on stderr; the samples taken.

```fides
pub fn pert_prof_report() -> i64 {
```

**Returns:** `i64`

*Defined at line 4641*

---

## pert_prof_samples

p_prof_samples(name): the samples with function `name` on the stack.

```fides
pub fn pert_prof_samples(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 4856*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
