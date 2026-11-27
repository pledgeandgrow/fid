[All modules](index.md)

# sys -- Fides Documentation

## Contents

- [_SC_PAGESIZE:](#_SC_PAGESIZE:) (const)
- [_SC_CLK_TCK:](#_SC_CLK_TCK:) (const)
- [_SC_NPROCESSORS_ONLN:](#_SC_NPROCESSORS_ONLN:) (const)
- [_SC_NPROCESSORS_CONF:](#_SC_NPROCESSORS_CONF:) (const)
- [_SC_PHYS_PAGES:](#_SC_PHYS_PAGES:) (const)
- [_SC_AVPHYS_PAGES:](#_SC_AVPHYS_PAGES:) (const)
- [sys_arch](#sys_arch) (function)
- [sys_cpu_count](#sys_cpu_count) (function)
- [sys_cpu_count_conf](#sys_cpu_count_conf) (function)
- [sys_page_size](#sys_page_size) (function)
- [sys_total_memory](#sys_total_memory) (function)
- [sys_available_memory](#sys_available_memory) (function)
- [sys_total_memory_kb](#sys_total_memory_kb) (function)
- [sys_total_memory_mb](#sys_total_memory_mb) (function)
- [sys_total_memory_gb](#sys_total_memory_gb) (function)
- [sys_pid](#sys_pid) (function)
- [sys_ppid](#sys_ppid) (function)
- [sys_uid](#sys_uid) (function)
- [sys_gid](#sys_gid) (function)
- [sys_clk_tck](#sys_clk_tck) (function)
- [sys_is_linux](#sys_is_linux) (function)
- [sys_is_macos](#sys_is_macos) (function)
- [sys_is_windows](#sys_is_windows) (function)
- [sys_is_64bit](#sys_is_64bit) (function)
- [sys_is_32bit](#sys_is_32bit) (function)
- [sys_is_little_endian](#sys_is_little_endian) (function)
- [sys_is_big_endian](#sys_is_big_endian) (function)
- [SystemInfo](#SystemInfo) (struct)
- [sys_info](#sys_info) (function)
- [sys_info_string](#sys_info_string) (function)

## _SC_PAGESIZE:

```fides
pub const _SC_PAGESIZE: i64 = 30;
```

*Defined at line 44*

---

## _SC_CLK_TCK:

```fides
pub const _SC_CLK_TCK: i64 = 2;
```

*Defined at line 45*

---

## _SC_NPROCESSORS_ONLN:

```fides
pub const _SC_NPROCESSORS_ONLN: i64 = 84;
```

*Defined at line 46*

---

## _SC_NPROCESSORS_CONF:

```fides
pub const _SC_NPROCESSORS_CONF: i64 = 83;
```

*Defined at line 47*

---

## _SC_PHYS_PAGES:

```fides
pub const _SC_PHYS_PAGES: i64 = 85;
```

*Defined at line 48*

---

## _SC_AVPHYS_PAGES:

```fides
pub const _SC_AVPHYS_PAGES: i64 = 86;
```

*Defined at line 49*

---

## sys_arch

Get the CPU architecture. @cfg resolves it for the build target
(x86_64 on the host; aarch64/riscv64/wasm32 on cross targets);
PLEDGE_ARCH overrides.

```fides
pub fn sys_arch() -> str {
```

**Returns:** `str`

*Defined at line 321*

---

## sys_cpu_count

Get the number of processors currently online.


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
assert(sys_cpu_count() >= 1);
assert(sys_page_size() >= 4096);
assert(sys_pid() > 0);
assert(str_len(sys_arch()) > 0);
```

```fides
pub fn sys_cpu_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 358*

---

## sys_cpu_count_conf

Get the number of configured processors.

```fides
pub fn sys_cpu_count_conf() -> i64 {
```

**Returns:** `i64`

*Defined at line 367*

---

## sys_page_size

Get the system page size in bytes.

```fides
pub fn sys_page_size() -> i64 {
```

**Returns:** `i64`

*Defined at line 378*

---

## sys_total_memory

Get total physical memory in bytes.

```fides
pub fn sys_total_memory() -> i64 {
```

**Returns:** `i64`

*Defined at line 387*

---

## sys_available_memory

Get available (free) physical memory in bytes.

```fides
pub fn sys_available_memory() -> i64 {
```

**Returns:** `i64`

*Defined at line 397*

---

## sys_total_memory_kb

Get total memory in KB (rounded).

```fides
pub fn sys_total_memory_kb() -> i64 {
```

**Returns:** `i64`

*Defined at line 407*

---

## sys_total_memory_mb

Get total memory in MB (rounded).

```fides
pub fn sys_total_memory_mb() -> i64 {
```

**Returns:** `i64`

*Defined at line 412*

---

## sys_total_memory_gb

Get total memory in GB (rounded).

```fides
pub fn sys_total_memory_gb() -> i64 {
```

**Returns:** `i64`

*Defined at line 417*

---

## sys_pid

Get the current process ID.

```fides
pub fn sys_pid() -> i64 {
```

**Returns:** `i64`

*Defined at line 424*

---

## sys_ppid

Get the parent process ID (0 on Windows).

```fides
pub fn sys_ppid() -> i64 {
```

**Returns:** `i64`

*Defined at line 429*

---

## sys_uid

Get the user ID (0 on Windows, which has none).

```fides
pub fn sys_uid() -> i64 {
```

**Returns:** `i64`

*Defined at line 434*

---

## sys_gid

Get the group ID (0 on Windows, which has none).

```fides
pub fn sys_gid() -> i64 {
```

**Returns:** `i64`

*Defined at line 439*

---

## sys_clk_tck

Get the clock ticks per second.

```fides
pub fn sys_clk_tck() -> i64 {
```

**Returns:** `i64`

*Defined at line 446*

---

## sys_is_linux

Check if running on Linux.

```fides
pub fn sys_is_linux() -> bool {
```

**Returns:** `bool`

*Defined at line 457*

---

## sys_is_macos

Check if running on macOS.

```fides
pub fn sys_is_macos() -> bool {
```

**Returns:** `bool`

*Defined at line 462*

---

## sys_is_windows

Check if running on Windows.

```fides
pub fn sys_is_windows() -> bool {
```

**Returns:** `bool`

*Defined at line 468*

---

## sys_is_64bit

Check if running on a 64-bit system.

```fides
pub fn sys_is_64bit() -> bool {
```

**Returns:** `bool`

*Defined at line 474*

---

## sys_is_32bit

Check if running on a 32-bit system.

```fides
pub fn sys_is_32bit() -> bool {
```

**Returns:** `bool`

*Defined at line 479*

---

## sys_is_little_endian

Check if the system is little-endian. Every architecture fid can
target (x86_64, aarch64, riscv64, wasm32) is little-endian, so this
is true on all of them; kept as a function for forward compatibility.

```fides
pub fn sys_is_little_endian() -> bool {
```

**Returns:** `bool`

*Defined at line 488*

---

## sys_is_big_endian

Check if the system is big-endian.

```fides
pub fn sys_is_big_endian() -> bool {
```

**Returns:** `bool`

*Defined at line 506*

---

## SystemInfo

A struct containing system information.

```fides
pub struct SystemInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `arch` | `str` |
| `os_name` | `str` |
| `cpu_count` | `i64` |
| `memory_bytes` | `i64` |
| `page_size` | `i64` |
| `pid` | `i64` |
| `uid` | `i64` |

*Defined at line 513*

---

## sys_info

Gather system information into a struct.

```fides
pub fn sys_info() -> SystemInfo {
```

**Returns:** `SystemInfo`

*Defined at line 524*

---

## sys_info_string

Format system info as a human-readable string.

```fides
pub fn sys_info_string(info: SystemInfo) -> str {
```

**Parameters:**

- `info`: `SystemInfo`

**Returns:** `str`

*Defined at line 531*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
