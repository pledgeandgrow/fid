[All modules](index.md)

# syscall_abi -- Fides Documentation

## Contents

- [ABI_X86_64:](#ABI_X86_64:) (const)
- [ABI_AARCH64:](#ABI_AARCH64:) (const)
- [ABI_RISCV64:](#ABI_RISCV64:) (const)
- [SARG_INT:](#SARG_INT:) (const)
- [SARG_FD:](#SARG_FD:) (const)
- [SARG_SIZE:](#SARG_SIZE:) (const)
- [SARG_FLAGS:](#SARG_FLAGS:) (const)
- [SARG_ADDR:](#SARG_ADDR:) (const)
- [SARG_IN:](#SARG_IN:) (const)
- [SARG_OUT:](#SARG_OUT:) (const)
- [SARG_INPTR:](#SARG_INPTR:) (const)
- [SARG_OUTPTR:](#SARG_OUTPTR:) (const)
- [SARG_STR:](#SARG_STR:) (const)
- [UPROT_READ:](#UPROT_READ:) (const)
- [UPROT_WRITE:](#UPROT_WRITE:) (const)
- [SYS_PATH_MAX:](#SYS_PATH_MAX:) (const)
- [EPERM:](#EPERM:) (const)
- [ENOENT:](#ENOENT:) (const)
- [ESRCH:](#ESRCH:) (const)
- [EINTR:](#EINTR:) (const)
- [EIO:](#EIO:) (const)
- [EBADF:](#EBADF:) (const)
- [ECHILD:](#ECHILD:) (const)
- [EAGAIN:](#EAGAIN:) (const)
- [ENOMEM:](#ENOMEM:) (const)
- [EACCES:](#EACCES:) (const)
- [EFAULT:](#EFAULT:) (const)
- [EBUSY:](#EBUSY:) (const)
- [EEXIST:](#EEXIST:) (const)
- [ENOTDIR:](#ENOTDIR:) (const)
- [EISDIR:](#EISDIR:) (const)
- [EINVAL:](#EINVAL:) (const)
- [EMFILE:](#EMFILE:) (const)
- [ENOSPC:](#ENOSPC:) (const)
- [ESPIPE:](#ESPIPE:) (const)
- [EROFS:](#EROFS:) (const)
- [EPIPE:](#EPIPE:) (const)
- [ERANGE:](#ERANGE:) (const)
- [ENAMETOOLONG:](#ENAMETOOLONG:) (const)
- [ENOSYS:](#ENOSYS:) (const)
- [ENOTEMPTY:](#ENOTEMPTY:) (const)
- [ELOOP:](#ELOOP:) (const)
- [ENOTSOCK:](#ENOTSOCK:) (const)
- [EOPNOTSUPP:](#EOPNOTSUPP:) (const)
- [EADDRINUSE:](#EADDRINUSE:) (const)
- [ECONNRESET:](#ECONNRESET:) (const)
- [ETIMEDOUT:](#ETIMEDOUT:) (const)
- [ECONNREFUSED:](#ECONNREFUSED:) (const)
- [errno_name](#errno_name) (function)
- [errno_message](#errno_message) (function)
- [errno_from_name](#errno_from_name) (function)
- [sys_is_error](#sys_is_error) (function)
- [sys_errno](#sys_errno) (function)
- [sys_libc_return](#sys_libc_return) (function)
- [sys_nr_reg](#sys_nr_reg) (function)
- [sys_arg_reg](#sys_arg_reg) (function)
- [sys_ret_reg](#sys_ret_reg) (function)
- [sys_reg_names](#sys_reg_names) (function)
- [sys_trap_bytes](#sys_trap_bytes) (function)
- [SysFrame](#SysFrame) (struct)
- [sys_frame](#sys_frame) (function)
- [sys_set_return](#sys_set_return) (function)
- [sys_load_regs](#sys_load_regs) (function)
- [SysDesc](#SysDesc) (struct)
- [SyscallTable](#SyscallTable) (struct)
- [syscall_table_linux](#syscall_table_linux) (function)
- [syscall_nr](#syscall_nr) (function)
- [syscall_name](#syscall_name) (function)
- [URegion](#URegion) (struct)
- [UserSpace](#UserSpace) (struct)
- [userspace_new](#userspace_new) (function)
- [userspace_map](#userspace_map) (function)
- [userspace_load](#userspace_load) (function)
- [uaccess_ok](#uaccess_ok) (function)
- [uaccess_read](#uaccess_read) (function)
- [uaccess_write](#uaccess_write) (function)
- [uaccess_strlen](#uaccess_strlen) (function)
- [uaccess_str](#uaccess_str) (function)
- [syscall_validate](#syscall_validate) (function)
- [syscall_dispatch](#syscall_dispatch) (function)
- [syscall_trap](#syscall_trap) (function)

## ABI_X86_64:

```fides
pub const ABI_X86_64: i64 = 0;
```

*Defined at line 26*

---

## ABI_AARCH64:

```fides
pub const ABI_AARCH64: i64 = 1;
```

*Defined at line 27*

---

## ABI_RISCV64:

```fides
pub const ABI_RISCV64: i64 = 2;
```

*Defined at line 28*

---

## SARG_INT:

```fides
pub const SARG_INT: i64 = 0;
```

*Defined at line 31*

---

## SARG_FD:

```fides
pub const SARG_FD: i64 = 1;
```

*Defined at line 32*

---

## SARG_SIZE:

```fides
pub const SARG_SIZE: i64 = 2;
```

*Defined at line 33*

---

## SARG_FLAGS:

```fides
pub const SARG_FLAGS: i64 = 3;
```

*Defined at line 34*

---

## SARG_ADDR:

```fides
pub const SARG_ADDR: i64 = 4;
```

*Defined at line 35*

---

## SARG_IN:

```fides
pub const SARG_IN: i64 = 5;
```

*Defined at line 37*

---

## SARG_OUT:

```fides
pub const SARG_OUT: i64 = 6;
```

*Defined at line 39*

---

## SARG_INPTR:

```fides
pub const SARG_INPTR: i64 = 7;
```

*Defined at line 41*

---

## SARG_OUTPTR:

```fides
pub const SARG_OUTPTR: i64 = 8;
```

*Defined at line 43*

---

## SARG_STR:

```fides
pub const SARG_STR: i64 = 9;  // NUL-terminated user string
```

*Defined at line 45*

---

## UPROT_READ:

```fides
pub const UPROT_READ: i64 = 1;
```

*Defined at line 47*

---

## UPROT_WRITE:

```fides
pub const UPROT_WRITE: i64 = 2;
```

*Defined at line 48*

---

## SYS_PATH_MAX:

```fides
pub const SYS_PATH_MAX: i64 = 4096;
```

*Defined at line 50*

---

## EPERM:

```fides
pub const EPERM: i64 = 1;
```

*Defined at line 54*

---

## ENOENT:

```fides
pub const ENOENT: i64 = 2;
```

*Defined at line 55*

---

## ESRCH:

```fides
pub const ESRCH: i64 = 3;
```

*Defined at line 56*

---

## EINTR:

```fides
pub const EINTR: i64 = 4;
```

*Defined at line 57*

---

## EIO:

```fides
pub const EIO: i64 = 5;
```

*Defined at line 58*

---

## EBADF:

```fides
pub const EBADF: i64 = 9;
```

*Defined at line 59*

---

## ECHILD:

```fides
pub const ECHILD: i64 = 10;
```

*Defined at line 60*

---

## EAGAIN:

```fides
pub const EAGAIN: i64 = 11;
```

*Defined at line 61*

---

## ENOMEM:

```fides
pub const ENOMEM: i64 = 12;
```

*Defined at line 62*

---

## EACCES:

```fides
pub const EACCES: i64 = 13;
```

*Defined at line 63*

---

## EFAULT:

```fides
pub const EFAULT: i64 = 14;
```

*Defined at line 64*

---

## EBUSY:

```fides
pub const EBUSY: i64 = 16;
```

*Defined at line 65*

---

## EEXIST:

```fides
pub const EEXIST: i64 = 17;
```

*Defined at line 66*

---

## ENOTDIR:

```fides
pub const ENOTDIR: i64 = 20;
```

*Defined at line 67*

---

## EISDIR:

```fides
pub const EISDIR: i64 = 21;
```

*Defined at line 68*

---

## EINVAL:

```fides
pub const EINVAL: i64 = 22;
```

*Defined at line 69*

---

## EMFILE:

```fides
pub const EMFILE: i64 = 24;
```

*Defined at line 70*

---

## ENOSPC:

```fides
pub const ENOSPC: i64 = 28;
```

*Defined at line 71*

---

## ESPIPE:

```fides
pub const ESPIPE: i64 = 29;
```

*Defined at line 72*

---

## EROFS:

```fides
pub const EROFS: i64 = 30;
```

*Defined at line 73*

---

## EPIPE:

```fides
pub const EPIPE: i64 = 32;
```

*Defined at line 74*

---

## ERANGE:

```fides
pub const ERANGE: i64 = 34;
```

*Defined at line 75*

---

## ENAMETOOLONG:

```fides
pub const ENAMETOOLONG: i64 = 36;
```

*Defined at line 76*

---

## ENOSYS:

```fides
pub const ENOSYS: i64 = 38;
```

*Defined at line 77*

---

## ENOTEMPTY:

```fides
pub const ENOTEMPTY: i64 = 39;
```

*Defined at line 78*

---

## ELOOP:

```fides
pub const ELOOP: i64 = 40;
```

*Defined at line 79*

---

## ENOTSOCK:

```fides
pub const ENOTSOCK: i64 = 88;
```

*Defined at line 80*

---

## EOPNOTSUPP:

```fides
pub const EOPNOTSUPP: i64 = 95;
```

*Defined at line 81*

---

## EADDRINUSE:

```fides
pub const EADDRINUSE: i64 = 98;
```

*Defined at line 82*

---

## ECONNRESET:

```fides
pub const ECONNRESET: i64 = 104;
```

*Defined at line 83*

---

## ETIMEDOUT:

```fides
pub const ETIMEDOUT: i64 = 110;
```

*Defined at line 84*

---

## ECONNREFUSED:

```fides
pub const ECONNREFUSED: i64 = 111;
```

*Defined at line 85*

---

## errno_name

"ENOENT" for 2; "E?" for numbers not in the table.

```fides
pub fn errno_name(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 265*

---

## errno_message

strerror text.

```fides
pub fn errno_message(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 274*

---

## errno_from_name

Number for a name ("EINVAL" -> 22), -1 when unknown.

```fides
pub fn errno_from_name(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 283*

---

## sys_is_error

Kernel return values -4095..-1 are -errno.

```fides
pub fn sys_is_error(r: i64) -> bool {
```

**Parameters:**

- `r`: `i64`

**Returns:** `bool`

*Defined at line 296*

---

## sys_errno

errno carried by a raw syscall result, 0 for success (libc's check).

```fides
pub fn sys_errno(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 301*

---

## sys_libc_return

What libc returns to the caller: -1 on error (errno set), else r.

```fides
pub fn sys_libc_return(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 309*

---

## sys_nr_reg

```fides
pub fn sys_nr_reg(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 321*

---

## sys_arg_reg

Register holding argument i (0..5).

```fides
pub fn sys_arg_reg(arch: i64, i: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 332*

---

## sys_ret_reg

```fides
pub fn sys_ret_reg(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 357*

---

## sys_reg_names

Names of the number register and the argument registers.

```fides
pub fn sys_reg_names(arch: i64) -> str {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `str`

*Defined at line 365*

---

## sys_trap_bytes

Bytes of the trap instruction: syscall (0F 05), svc #0, ecall.

```fides
pub fn sys_trap_bytes(arch: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 376*

---

## SysFrame

```fides
pub struct SysFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `nr` | `i64` |
| `args` | `Vec<i64>` |

*Defined at line 394*

---

## sys_frame

Number and arguments out of a trapped register file.

```fides
pub fn sys_frame(arch: i64, regs: Vec<i64>) -> SysFrame {
```

**Parameters:**

- `arch`: `i64`
- `regs`: `Vec<i64>`

**Returns:** `SysFrame`

*Defined at line 400*

---

## sys_set_return

Store the result where user space expects it.

```fides
pub fn sys_set_return(arch: i64, regs: &mut Vec<i64>, value: i64) {
```

**Parameters:**

- `arch`: `i64`
- `regs`: `&mut Vec<i64>`
- `value`: `i64`

*Defined at line 413*

---

## sys_load_regs

Register file for a call (what a libc syscall() wrapper loads).

```fides
pub fn sys_load_regs(arch: i64, nr: i64, args: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `nr`: `i64`
- `args`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 418*

---

## SysDesc

```fides
pub struct SysDesc {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `nr` | `i64` |
| `kinds` | `Vec<i64>` |
| `lens` | `Vec<i64>` |
| `nullable` | `Vec<bool>` |

*Defined at line 431*

---

## SyscallTable

```fides
pub struct SyscallTable {
```

**Fields:**

| Name | Type |
|------|------|
| `arch` | `i64` |
| `descs` | `Vec<SysDesc>` |
| `by_nr` | `HashMap<i64` |
| `calls` | `i64` |
| `rejected` | `i64` |

*Defined at line 440*

---

## syscall_table_linux

The Linux table (a common subset) for an architecture.

```fides
pub fn syscall_table_linux(arch: i64) -> SyscallTable {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `SyscallTable`

*Defined at line 534*

---

## syscall_nr

Number of `name` on the table's architecture, -1 when absent.

```fides
pub fn syscall_nr(t: &SyscallTable, name: str) -> i64 {
```

**Parameters:**

- `t`: `&SyscallTable`
- `name`: `str`

**Returns:** `i64`

*Defined at line 593*

---

## syscall_name

Name for a number, "" when unknown.

```fides
pub fn syscall_name(t: &SyscallTable, nr: i64) -> str {
```

**Parameters:**

- `t`: `&SyscallTable`
- `nr`: `i64`

**Returns:** `str`

*Defined at line 605*

---

## URegion

```fides
pub struct URegion {
```

**Fields:**

| Name | Type |
|------|------|
| `start` | `i64` |
| `end` | `i64` |
| `prot` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 614*

---

## UserSpace

```fides
pub struct UserSpace {
```

**Fields:**

| Name | Type |
|------|------|
| `regions` | `Vec<URegion>` |
| `lo` | `i64` |
| `hi` | `i64,  // end of the user half (TASK_SIZE)` |

*Defined at line 621*

---

## userspace_new

x86-64 / 48-bit layout: user addresses 0x10000 .. 0x7FFFFFFFF000.

```fides
pub fn userspace_new() -> UserSpace {
```

**Returns:** `UserSpace`

*Defined at line 629*

---

## userspace_map

```fides
pub fn userspace_map(us: &mut UserSpace, start: i64, len: i64, prot: i64) {
```

**Parameters:**

- `us`: `&mut UserSpace`
- `start`: `i64`
- `len`: `i64`
- `prot`: `i64`

*Defined at line 635*

---

## userspace_load

Kernel-side load of bytes into user memory, ignoring protections
(an exec loader filling a read-only segment).

```fides
pub fn userspace_load(us: &mut UserSpace, p: i64, bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `us`: `&mut UserSpace`
- `p`: `i64`
- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 643*

---

## uaccess_ok

access_ok plus the region walk: [p, p+n) is user memory with prot.

```fides
pub fn uaccess_ok(us: &UserSpace, p: i64, n: i64, prot: i64) -> bool {
```

**Parameters:**

- `us`: `&UserSpace`
- `p`: `i64`
- `n`: `i64`
- `prot`: `i64`

**Returns:** `bool`

*Defined at line 671*

---

## uaccess_read

copy_from_user: the bytes, or an empty Vec on a fault (n > 0).

```fides
pub fn uaccess_read(us: &UserSpace, p: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `us`: `&UserSpace`
- `p`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 695*

---

## uaccess_write

copy_to_user: false on a fault (nothing written).

```fides
pub fn uaccess_write(us: &mut UserSpace, p: i64, bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `us`: `&mut UserSpace`
- `p`: `i64`
- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 710*

---

## uaccess_strlen

Length of the user string at p (strnlen_user): -EFAULT when it runs
into unmapped or unreadable memory, -ENAMETOOLONG past max bytes.

```fides
pub fn uaccess_strlen(us: &UserSpace, p: i64, max: i64) -> i64 {
```

**Parameters:**

- `us`: `&UserSpace`
- `p`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 727*

---

## uaccess_str

strncpy_from_user: the string ("" on a fault; check uaccess_strlen).

```fides
pub fn uaccess_str(us: &UserSpace, p: i64) -> str {
```

**Parameters:**

- `us`: `&UserSpace`
- `p`: `i64`

**Returns:** `str`

*Defined at line 751*

---

## syscall_validate

Validate the arguments of a call against its description: 0, or the
-errno the kernel returns without running the call.

```fides
pub fn syscall_validate(d: &SysDesc, us: &UserSpace, args: Vec<i64>) -> i64 {
```

**Parameters:**

- `d`: `&SysDesc`
- `us`: `&UserSpace`
- `args`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 763*

---

## syscall_dispatch

Dispatch one trapped call: look the number up (-ENOSYS when absent),
validate, run handler(name, args), and return the value to put in the
result register.

```fides
pub fn syscall_dispatch(t: &mut SyscallTable, us: &UserSpace, f: SysFrame, handler: fn(str, Vec<i64>) -> i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SyscallTable`
- `us`: `&UserSpace`
- `f`: `SysFrame`
- `handler`: `fn(str`

**Returns:** `i64`

*Defined at line 809*

---

## syscall_trap

Full trap path on a register file: frame out, dispatch, result in.

```fides
pub fn syscall_trap(t: &mut SyscallTable, us: &UserSpace, regs: &mut Vec<i64>, handler: fn(str, Vec<i64>) -> i64) -> i64 {
```

**Parameters:**

- `t`: `&mut SyscallTable`
- `us`: `&UserSpace`
- `regs`: `&mut Vec<i64>`
- `handler`: `fn(str`

**Returns:** `i64`

*Defined at line 825*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
