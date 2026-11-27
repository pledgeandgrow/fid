[All modules](index.md)

# os_sandbox -- Fides Documentation

## Contents

- [BPF_LD:](#BPF_LD:) (const)
- [BPF_JMP:](#BPF_JMP:) (const)
- [BPF_RET:](#BPF_RET:) (const)
- [BPF_W:](#BPF_W:) (const)
- [BPF_ABS:](#BPF_ABS:) (const)
- [BPF_JEQ:](#BPF_JEQ:) (const)
- [BPF_JGE:](#BPF_JGE:) (const)
- [BPF_JGT:](#BPF_JGT:) (const)
- [BPF_K:](#BPF_K:) (const)
- [SB_RET_KILL_PROCESS:](#SB_RET_KILL_PROCESS:) (const)
- [SB_RET_KILL_THREAD:](#SB_RET_KILL_THREAD:) (const)
- [SB_RET_TRAP:](#SB_RET_TRAP:) (const)
- [SB_RET_ERRNO:](#SB_RET_ERRNO:) (const)
- [SB_RET_USER_NOTIF:](#SB_RET_USER_NOTIF:) (const)
- [SB_RET_TRACE:](#SB_RET_TRACE:) (const)
- [SB_RET_LOG:](#SB_RET_LOG:) (const)
- [SB_RET_ALLOW:](#SB_RET_ALLOW:) (const)
- [SB_RET_DATA:](#SB_RET_DATA:) (const)
- [SB_VERDICT_ALLOW:](#SB_VERDICT_ALLOW:) (const)
- [SB_VERDICT_KILL:](#SB_VERDICT_KILL:) (const)
- [SB_VERDICT_TRAP:](#SB_VERDICT_TRAP:) (const)
- [SB_VERDICT_LOG:](#SB_VERDICT_LOG:) (const)
- [SB_VERDICT_NOTIFY:](#SB_VERDICT_NOTIFY:) (const)
- [SB_VERDICT_ERRNO:](#SB_VERDICT_ERRNO:) (const)
- [SB_ARCH_X86_64:](#SB_ARCH_X86_64:) (const)
- [SB_ARCH_AARCH64:](#SB_ARCH_AARCH64:) (const)
- [SB_ARCH_I386:](#SB_ARCH_I386:) (const)
- [SB_OFF_NR:](#SB_OFF_NR:) (const)
- [SB_OFF_ARCH:](#SB_OFF_ARCH:) (const)
- [SB_OFF_ARG0:](#SB_OFF_ARG0:) (const)
- [BpfInsn](#BpfInsn) (struct)
- [bpf](#bpf) (function)
- [sb_ret](#sb_ret) (function)
- [BpfRule](#BpfRule) (struct)
- [BpfPolicy](#BpfPolicy) (struct)
- [bpf_policy_new](#bpf_policy_new) (function)
- [bpf_allow](#bpf_allow) (function)
- [bpf_allow_arg](#bpf_allow_arg) (function)
- [bpf_rule](#bpf_rule) (function)
- [bpf_assemble](#bpf_assemble) (function)
- [bpf_encode](#bpf_encode) (function)
- [bpf_compile](#bpf_compile) (function)
- [bpf_decode](#bpf_decode) (function)
- [bpf_len](#bpf_len) (function)
- [sb_args](#sb_args) (function)
- [sb_args0](#sb_args0) (function)
- [bpf_run](#bpf_run) (function)
- [sb_verdict_name](#sb_verdict_name) (function)
- [sb_call_x64](#sb_call_x64) (function)
- [sb_call_a64](#sb_call_a64) (function)
- [bpf_allow_name](#bpf_allow_name) (function)
- [LL_FS_EXECUTE:](#LL_FS_EXECUTE:) (const)
- [LL_FS_WRITE_FILE:](#LL_FS_WRITE_FILE:) (const)
- [LL_FS_READ_FILE:](#LL_FS_READ_FILE:) (const)
- [LL_FS_READ_DIR:](#LL_FS_READ_DIR:) (const)
- [LL_FS_REMOVE_DIR:](#LL_FS_REMOVE_DIR:) (const)
- [LL_FS_REMOVE_FILE:](#LL_FS_REMOVE_FILE:) (const)
- [LL_FS_MAKE_CHAR:](#LL_FS_MAKE_CHAR:) (const)
- [LL_FS_MAKE_DIR:](#LL_FS_MAKE_DIR:) (const)
- [LL_FS_MAKE_REG:](#LL_FS_MAKE_REG:) (const)
- [LL_FS_MAKE_SOCK:](#LL_FS_MAKE_SOCK:) (const)
- [LL_FS_MAKE_FIFO:](#LL_FS_MAKE_FIFO:) (const)
- [LL_FS_MAKE_BLOCK:](#LL_FS_MAKE_BLOCK:) (const)
- [LL_FS_MAKE_SYM:](#LL_FS_MAKE_SYM:) (const)
- [LL_FS_REFER:](#LL_FS_REFER:) (const)
- [LL_FS_TRUNCATE:](#LL_FS_TRUNCATE:) (const)
- [LL_NET_BIND_TCP:](#LL_NET_BIND_TCP:) (const)
- [LL_NET_CONNECT_TCP:](#LL_NET_CONNECT_TCP:) (const)
- [ll_fs_mask_for_abi](#ll_fs_mask_for_abi) (function)
- [LlPathRule](#LlPathRule) (struct)
- [LlPortRule](#LlPortRule) (struct)
- [LlRuleset](#LlRuleset) (struct)
- [ll_new](#ll_new) (function)
- [ll_allow_path](#ll_allow_path) (function)
- [ll_allow_port](#ll_allow_port) (function)
- [ll_ruleset_attr](#ll_ruleset_attr) (function)
- [ll_path_beneath_attr](#ll_path_beneath_attr) (function)
- [ll_net_port_attr](#ll_net_port_attr) (function)
- [ll_unsupported_fs](#ll_unsupported_fs) (function)
- [JOB_LIMIT_ACTIVE_PROCESS:](#JOB_LIMIT_ACTIVE_PROCESS:) (const)
- [JOB_LIMIT_PROCESS_MEMORY:](#JOB_LIMIT_PROCESS_MEMORY:) (const)
- [JOB_LIMIT_JOB_MEMORY:](#JOB_LIMIT_JOB_MEMORY:) (const)
- [JOB_LIMIT_KILL_ON_JOB_CLOSE:](#JOB_LIMIT_KILL_ON_JOB_CLOSE:) (const)
- [JOB_LIMIT_BREAKAWAY_OK:](#JOB_LIMIT_BREAKAWAY_OK:) (const)
- [JOB_LIMIT_SILENT_BREAKAWAY:](#JOB_LIMIT_SILENT_BREAKAWAY:) (const)
- [JobLimits](#JobLimits) (struct)
- [job_new](#job_new) (function)
- [job_limit_flags](#job_limit_flags) (function)
- [job_basic_limit_bytes](#job_basic_limit_bytes) (function)
- [APPCAP_INTERNET_CLIENT:](#APPCAP_INTERNET_CLIENT:) (const)
- [APPCAP_INTERNET_CLIENT_SERVER:](#APPCAP_INTERNET_CLIENT_SERVER:) (const)
- [APPCAP_PRIVATE_NETWORK:](#APPCAP_PRIVATE_NETWORK:) (const)
- [APPCAP_DOCUMENTS_LIBRARY:](#APPCAP_DOCUMENTS_LIBRARY:) (const)
- [APPCAP_PICTURES_LIBRARY:](#APPCAP_PICTURES_LIBRARY:) (const)
- [AppContainer](#AppContainer) (struct)
- [appcontainer_new](#appcontainer_new) (function)
- [appcontainer_add](#appcontainer_add) (function)
- [appcap_sid_parts](#appcap_sid_parts) (function)

## BPF_LD:

```fides
pub const BPF_LD: i64 = 0x00;
```

*Defined at line 42*

---

## BPF_JMP:

```fides
pub const BPF_JMP: i64 = 0x05;
```

*Defined at line 43*

---

## BPF_RET:

```fides
pub const BPF_RET: i64 = 0x06;
```

*Defined at line 44*

---

## BPF_W:

```fides
pub const BPF_W: i64 = 0x00;
```

*Defined at line 45*

---

## BPF_ABS:

```fides
pub const BPF_ABS: i64 = 0x20;
```

*Defined at line 46*

---

## BPF_JEQ:

```fides
pub const BPF_JEQ: i64 = 0x10;
```

*Defined at line 47*

---

## BPF_JGE:

```fides
pub const BPF_JGE: i64 = 0x30;
```

*Defined at line 48*

---

## BPF_JGT:

```fides
pub const BPF_JGT: i64 = 0x20;
```

*Defined at line 49*

---

## BPF_K:

```fides
pub const BPF_K: i64 = 0x00;
```

*Defined at line 50*

---

## SB_RET_KILL_PROCESS:

```fides
pub const SB_RET_KILL_PROCESS: i64 = 0x80000000;
```

*Defined at line 53*

---

## SB_RET_KILL_THREAD:

```fides
pub const SB_RET_KILL_THREAD: i64 = 0x00000000;
```

*Defined at line 54*

---

## SB_RET_TRAP:

```fides
pub const SB_RET_TRAP: i64 = 0x00030000;
```

*Defined at line 55*

---

## SB_RET_ERRNO:

```fides
pub const SB_RET_ERRNO: i64 = 0x00050000;
```

*Defined at line 56*

---

## SB_RET_USER_NOTIF:

```fides
pub const SB_RET_USER_NOTIF: i64 = 0x7fc00000;
```

*Defined at line 57*

---

## SB_RET_TRACE:

```fides
pub const SB_RET_TRACE: i64 = 0x7ff00000;
```

*Defined at line 58*

---

## SB_RET_LOG:

```fides
pub const SB_RET_LOG: i64 = 0x7ffc0000;
```

*Defined at line 59*

---

## SB_RET_ALLOW:

```fides
pub const SB_RET_ALLOW: i64 = 0x7fff0000;
```

*Defined at line 60*

---

## SB_RET_DATA:

```fides
pub const SB_RET_DATA: i64 = 0x0000ffff;  // mask for ERRNO / TRACE data
```

*Defined at line 61*

---

## SB_VERDICT_ALLOW:

```fides
pub const SB_VERDICT_ALLOW: i64 = 0;
```

*Defined at line 64*

---

## SB_VERDICT_KILL:

```fides
pub const SB_VERDICT_KILL: i64 = 1;
```

*Defined at line 65*

---

## SB_VERDICT_TRAP:

```fides
pub const SB_VERDICT_TRAP: i64 = 2;
```

*Defined at line 66*

---

## SB_VERDICT_LOG:

```fides
pub const SB_VERDICT_LOG: i64 = 3;
```

*Defined at line 67*

---

## SB_VERDICT_NOTIFY:

```fides
pub const SB_VERDICT_NOTIFY: i64 = 4;
```

*Defined at line 68*

---

## SB_VERDICT_ERRNO:

```fides
pub const SB_VERDICT_ERRNO: i64 = 100;  // + errno in the low part
```

*Defined at line 69*

---

## SB_ARCH_X86_64:

```fides
pub const SB_ARCH_X86_64: i64 = 0xC000003E;
```

*Defined at line 72*

---

## SB_ARCH_AARCH64:

```fides
pub const SB_ARCH_AARCH64: i64 = 0xC00000B7;
```

*Defined at line 73*

---

## SB_ARCH_I386:

```fides
pub const SB_ARCH_I386: i64 = 0x40000003;
```

*Defined at line 74*

---

## SB_OFF_NR:

```fides
pub const SB_OFF_NR: i64 = 0;
```

*Defined at line 77*

---

## SB_OFF_ARCH:

```fides
pub const SB_OFF_ARCH: i64 = 4;
```

*Defined at line 78*

---

## SB_OFF_ARG0:

```fides
pub const SB_OFF_ARG0: i64 = 16;
```

*Defined at line 79*

---

## BpfInsn

One classic-BPF instruction (struct sock_filter).

```fides
pub struct BpfInsn {
```

**Fields:**

| Name | Type |
|------|------|
| `code` | `i64` |
| `jt` | `i64` |
| `jf` | `i64` |
| `k` | `i64` |

*Defined at line 82*

---

## bpf

```fides
pub fn bpf(code: i64, jt: i64, jf: i64, k: i64) -> BpfInsn {
```

**Parameters:**

- `code`: `i64`
- `jt`: `i64`
- `jf`: `i64`
- `k`: `i64`

**Returns:** `BpfInsn`

*Defined at line 89*

---

## sb_ret

A verdict value (the 32-bit seccomp return) from a policy verdict code.

```fides
pub fn sb_ret(verdict: i64) -> i64 {
```

**Parameters:**

- `verdict`: `i64`

**Returns:** `i64`

*Defined at line 96*

---

## BpfRule

One rule: call number `nr` gets `verdict`. When `has_arg`, it applies
only when argument `arg_index` equals `arg_value`.

```fides
pub struct BpfRule {
```

**Fields:**

| Name | Type |
|------|------|
| `nr` | `i64` |
| `verdict` | `i64` |
| `has_arg` | `bool` |
| `arg_index` | `i64` |
| `arg_value` | `i64` |

*Defined at line 122*

---

## BpfPolicy

A policy: a list of per-call rules and a default verdict for everything
else. `kill_x32` rejects the x32 ABI (numbers >= 0x40000000) on x86-64.

```fides
pub struct BpfPolicy {
```

**Fields:**

| Name | Type |
|------|------|
| `rules` | `Vec<BpfRule>` |
| `default_verdict` | `i64` |
| `kill_x32` | `bool` |

*Defined at line 132*

---

## bpf_policy_new

```fides
pub fn bpf_policy_new(default_verdict: i64) -> BpfPolicy {
```

**Parameters:**

- `default_verdict`: `i64`

**Returns:** `BpfPolicy`

*Defined at line 138*

---

## bpf_allow

Allow one call number.

```fides
pub fn bpf_allow(p: &mut BpfPolicy, nr: i64) {
```

**Parameters:**

- `p`: `&mut BpfPolicy`
- `nr`: `i64`

*Defined at line 145*

---

## bpf_allow_arg

Allow one call only when argument `idx` (0..5) equals `value`.

```fides
pub fn bpf_allow_arg(p: &mut BpfPolicy, nr: i64, idx: i64, value: i64) {
```

**Parameters:**

- `p`: `&mut BpfPolicy`
- `nr`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 152*

---

## bpf_rule

Give one call number a specific verdict (e.g. SB_VERDICT_ERRNO + 1).

```fides
pub fn bpf_rule(p: &mut BpfPolicy, nr: i64, verdict: i64) {
```

**Parameters:**

- `p`: `&mut BpfPolicy`
- `nr`: `i64`
- `verdict`: `i64`

*Defined at line 159*

---

## bpf_assemble

The program as a list of instructions. Shape: load arch; if arch != the
target, default; (x86-64) load nr; if nr >= 0x40000000 and kill_x32,
default; load nr; per rule a compare that returns its verdict; default.

```fides
pub fn bpf_assemble(p: &BpfPolicy, arch: i64) -> Vec<BpfInsn> {
```

**Parameters:**

- `p`: `&BpfPolicy`
- `arch`: `i64`

**Returns:** `Vec<BpfInsn>`

*Defined at line 170*

---

## bpf_encode

Encode instructions as struct sock_filter bytes (8 bytes each).

```fides
pub fn bpf_encode(prog: &Vec<BpfInsn>) -> Vec<i64> {
```

**Parameters:**

- `prog`: `&Vec<BpfInsn>`

**Returns:** `Vec<i64>`

*Defined at line 214*

---

## bpf_compile

Compile a policy straight to sock_filter bytes.

```fides
pub fn bpf_compile(p: &BpfPolicy, arch: i64) -> Vec<i64> {
```

**Parameters:**

- `p`: `&BpfPolicy`
- `arch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 230*

---

## bpf_decode

Decode sock_filter bytes back to instructions.

```fides
pub fn bpf_decode(b: &Vec<i64>) -> Vec<BpfInsn> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Vec<BpfInsn>`

*Defined at line 235*

---

## bpf_len

The number of instructions a policy compiles to.

```fides
pub fn bpf_len(p: &BpfPolicy, arch: i64) -> i64 {
```

**Parameters:**

- `p`: `&BpfPolicy`
- `arch`: `i64`

**Returns:** `i64`

*Defined at line 248*

---

## sb_args

The six arguments of a call as a Vec (index 0..5).

```fides
pub fn sb_args(a0: i64, a1: i64, a2: i64, a3: i64, a4: i64, a5: i64) -> Vec<i64> {
```

**Parameters:**

- `a0`: `i64`
- `a1`: `i64`
- `a2`: `i64`
- `a3`: `i64`
- `a4`: `i64`
- `a5`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 255*

---

## sb_args0

No arguments (all zero).

```fides
pub fn sb_args0() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 267*

---

## bpf_run

Run an encoded program (sock_filter bytes) over one record; returns the
32-bit seccomp verdict the program would produce for it.

```fides
pub fn bpf_run(prog_bytes: &Vec<i64>, arch: i64, nr: i64, args: Vec<i64>) -> i64 {
```

**Parameters:**

- `prog_bytes`: `&Vec<i64>`
- `arch`: `i64`
- `nr`: `i64`
- `args`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 299*

---

## sb_verdict_name

A readable verdict name for a 32-bit seccomp return.

```fides
pub fn sb_verdict_name(ret: i64) -> str {
```

**Parameters:**

- `ret`: `i64`

**Returns:** `str`

*Defined at line 336*

---

## sb_call_x64

The x86-64 call number of a name, or -1 when not in the table.

```fides
pub fn sb_call_x64(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 485*

---

## sb_call_a64

The AArch64 call number of a name, or -1.

```fides
pub fn sb_call_a64(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 490*

---

## bpf_allow_name

Allow a call by name for the given architecture (ignored when unknown).

```fides
pub fn bpf_allow_name(p: &mut BpfPolicy, name: str, arch: i64) {
```

**Parameters:**

- `p`: `&mut BpfPolicy`
- `name`: `str`
- `arch`: `i64`

*Defined at line 495*

---

## LL_FS_EXECUTE:

```fides
pub const LL_FS_EXECUTE: i64 = 1;
```

*Defined at line 505*

---

## LL_FS_WRITE_FILE:

```fides
pub const LL_FS_WRITE_FILE: i64 = 2;
```

*Defined at line 506*

---

## LL_FS_READ_FILE:

```fides
pub const LL_FS_READ_FILE: i64 = 4;
```

*Defined at line 507*

---

## LL_FS_READ_DIR:

```fides
pub const LL_FS_READ_DIR: i64 = 8;
```

*Defined at line 508*

---

## LL_FS_REMOVE_DIR:

```fides
pub const LL_FS_REMOVE_DIR: i64 = 16;
```

*Defined at line 509*

---

## LL_FS_REMOVE_FILE:

```fides
pub const LL_FS_REMOVE_FILE: i64 = 32;
```

*Defined at line 510*

---

## LL_FS_MAKE_CHAR:

```fides
pub const LL_FS_MAKE_CHAR: i64 = 64;
```

*Defined at line 511*

---

## LL_FS_MAKE_DIR:

```fides
pub const LL_FS_MAKE_DIR: i64 = 128;
```

*Defined at line 512*

---

## LL_FS_MAKE_REG:

```fides
pub const LL_FS_MAKE_REG: i64 = 256;
```

*Defined at line 513*

---

## LL_FS_MAKE_SOCK:

```fides
pub const LL_FS_MAKE_SOCK: i64 = 512;
```

*Defined at line 514*

---

## LL_FS_MAKE_FIFO:

```fides
pub const LL_FS_MAKE_FIFO: i64 = 1024;
```

*Defined at line 515*

---

## LL_FS_MAKE_BLOCK:

```fides
pub const LL_FS_MAKE_BLOCK: i64 = 2048;
```

*Defined at line 516*

---

## LL_FS_MAKE_SYM:

```fides
pub const LL_FS_MAKE_SYM: i64 = 4096;
```

*Defined at line 517*

---

## LL_FS_REFER:

```fides
pub const LL_FS_REFER: i64 = 8192;
```

*Defined at line 518*

---

## LL_FS_TRUNCATE:

```fides
pub const LL_FS_TRUNCATE: i64 = 16384;
```

*Defined at line 519*

---

## LL_NET_BIND_TCP:

```fides
pub const LL_NET_BIND_TCP: i64 = 1;
```

*Defined at line 521*

---

## LL_NET_CONNECT_TCP:

```fides
pub const LL_NET_CONNECT_TCP: i64 = 2;
```

*Defined at line 522*

---

## ll_fs_mask_for_abi

The filesystem access rights each Landlock ABI version added. ABI 1
handles bits 1..0x2000, ABI 2 adds REFER, ABI 3 adds TRUNCATE.

```fides
pub fn ll_fs_mask_for_abi(abi: i64) -> i64 {
```

**Parameters:**

- `abi`: `i64`

**Returns:** `i64`

*Defined at line 526*

---

## LlPathRule

A path-beneath rule: the rights allowed on everything under a directory.

```fides
pub struct LlPathRule {
```

**Fields:**

| Name | Type |
|------|------|
| `allowed` | `i64` |
| `path` | `str` |

*Defined at line 538*

---

## LlPortRule

A TCP port rule: the action (bind/connect) allowed on a port.

```fides
pub struct LlPortRule {
```

**Fields:**

| Name | Type |
|------|------|
| `allowed` | `i64` |
| `port` | `i64` |

*Defined at line 544*

---

## LlRuleset

```fides
pub struct LlRuleset {
```

**Fields:**

| Name | Type |
|------|------|
| `handled_fs` | `i64` |
| `handled_net` | `i64` |
| `paths` | `Vec<LlPathRule>` |
| `ports` | `Vec<LlPortRule>` |

*Defined at line 549*

---

## ll_new

A ruleset that handles (and so by default denies) the given rights.

```fides
pub fn ll_new(handled_fs: i64, handled_net: i64) -> LlRuleset {
```

**Parameters:**

- `handled_fs`: `i64`
- `handled_net`: `i64`

**Returns:** `LlRuleset`

*Defined at line 557*

---

## ll_allow_path

Allow `rights` on everything beneath `path`.

```fides
pub fn ll_allow_path(r: &mut LlRuleset, path: str, rights: i64) {
```

**Parameters:**

- `r`: `&mut LlRuleset`
- `path`: `str`
- `rights`: `i64`

*Defined at line 564*

---

## ll_allow_port

Allow `action` (LL_NET_*) on `port`.

```fides
pub fn ll_allow_port(r: &mut LlRuleset, port: i64, action: i64) {
```

**Parameters:**

- `r`: `&mut LlRuleset`
- `port`: `i64`
- `action`: `i64`

*Defined at line 571*

---

## ll_ruleset_attr

struct landlock_ruleset_attr: two u64 (handled_access_fs,
handled_access_net) masked to the rights an ABI version supports.

```fides
pub fn ll_ruleset_attr(r: &LlRuleset, abi: i64) -> Vec<i64> {
```

**Parameters:**

- `r`: `&LlRuleset`
- `abi`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 587*

---

## ll_path_beneath_attr

struct landlock_path_beneath_attr: u64 allowed_access, then an s32 fd
placeholder (the kernel wants an open fd; -1 here since this module does
not open files).

```fides
pub fn ll_path_beneath_attr(rule: &LlPathRule, abi: i64) -> Vec<i64> {
```

**Parameters:**

- `rule`: `&LlPathRule`
- `abi`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 602*

---

## ll_net_port_attr

struct landlock_net_port_attr (ABI 4+): u64 allowed_access, u64 port.

```fides
pub fn ll_net_port_attr(rule: &LlPortRule) -> Vec<i64> {
```

**Parameters:**

- `rule`: `&LlPortRule`

**Returns:** `Vec<i64>`

*Defined at line 615*

---

## ll_unsupported_fs

The rights a ruleset asks for that the given ABI cannot enforce (the
best-effort downgrade drops these). 0 when the ABI covers everything.

```fides
pub fn ll_unsupported_fs(r: &LlRuleset, abi: i64) -> i64 {
```

**Parameters:**

- `r`: `&LlRuleset`
- `abi`: `i64`

**Returns:** `i64`

*Defined at line 624*

---

## JOB_LIMIT_ACTIVE_PROCESS:

```fides
pub const JOB_LIMIT_ACTIVE_PROCESS: i64 = 0x00000008;
```

*Defined at line 631*

---

## JOB_LIMIT_PROCESS_MEMORY:

```fides
pub const JOB_LIMIT_PROCESS_MEMORY: i64 = 0x00000100;
```

*Defined at line 632*

---

## JOB_LIMIT_JOB_MEMORY:

```fides
pub const JOB_LIMIT_JOB_MEMORY: i64 = 0x00000200;
```

*Defined at line 633*

---

## JOB_LIMIT_KILL_ON_JOB_CLOSE:

```fides
pub const JOB_LIMIT_KILL_ON_JOB_CLOSE: i64 = 0x00002000;
```

*Defined at line 634*

---

## JOB_LIMIT_BREAKAWAY_OK:

```fides
pub const JOB_LIMIT_BREAKAWAY_OK: i64 = 0x00000800;
```

*Defined at line 635*

---

## JOB_LIMIT_SILENT_BREAKAWAY:

```fides
pub const JOB_LIMIT_SILENT_BREAKAWAY: i64 = 0x00001000;
```

*Defined at line 636*

---

## JobLimits

```fides
pub struct JobLimits {
```

**Fields:**

| Name | Type |
|------|------|
| `active_process_limit` | `i64` |
| `process_memory_limit` | `i64` |
| `job_memory_limit` | `i64` |
| `kill_on_close` | `bool` |
| `ui_restrictions` | `i64` |

*Defined at line 638*

---

## job_new

```fides
pub fn job_new() -> JobLimits {
```

**Returns:** `JobLimits`

*Defined at line 646*

---

## job_limit_flags

The LimitFlags word a JobLimits produces.

```fides
pub fn job_limit_flags(j: &JobLimits) -> i64 {
```

**Parameters:**

- `j`: `&JobLimits`

**Returns:** `i64`

*Defined at line 653*

---

## job_basic_limit_bytes

The extended-limit-information blob's basic section, abbreviated:
per_process_user_time (8, 0), per_job_user_time (8, 0), limit_flags (4),
minimum/maximum working set (8+8, 0), active process limit (4),
affinity (8, 0), priority class (4, 0), scheduling class (4, 0). This is
the exact head of JOBOBJECT_BASIC_LIMIT_INFORMATION (48 bytes, 64-bit).

```fides
pub fn job_basic_limit_bytes(j: &JobLimits) -> Vec<i64> {
```

**Parameters:**

- `j`: `&JobLimits`

**Returns:** `Vec<i64>`

*Defined at line 675*

---

## APPCAP_INTERNET_CLIENT:

```fides
pub const APPCAP_INTERNET_CLIENT: i64 = 3;
```

*Defined at line 692*

---

## APPCAP_INTERNET_CLIENT_SERVER:

```fides
pub const APPCAP_INTERNET_CLIENT_SERVER: i64 = 4;
```

*Defined at line 693*

---

## APPCAP_PRIVATE_NETWORK:

```fides
pub const APPCAP_PRIVATE_NETWORK: i64 = 5;
```

*Defined at line 694*

---

## APPCAP_DOCUMENTS_LIBRARY:

```fides
pub const APPCAP_DOCUMENTS_LIBRARY: i64 = 7;
```

*Defined at line 695*

---

## APPCAP_PICTURES_LIBRARY:

```fides
pub const APPCAP_PICTURES_LIBRARY: i64 = 8;
```

*Defined at line 696*

---

## AppContainer

An AppContainer profile: a name and the capability RIDs it is granted.

```fides
pub struct AppContainer {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `capabilities` | `Vec<i64>` |

*Defined at line 699*

---

## appcontainer_new

```fides
pub fn appcontainer_new(name: str) -> AppContainer {
```

**Parameters:**

- `name`: `str`

**Returns:** `AppContainer`

*Defined at line 704*

---

## appcontainer_add

```fides
pub fn appcontainer_add(a: &mut AppContainer, cap: i64) {
```

**Parameters:**

- `a`: `&mut AppContainer`
- `cap`: `i64`

*Defined at line 710*

---

## appcap_sid_parts

The capability SID subauthorities: every AppContainer capability SID is
S-1-15-3-<rid>, so this returns [15, 3, rid] (identifier authority,
first subauthority, the capability rid).

```fides
pub fn appcap_sid_parts(rid: i64) -> Vec<i64> {
```

**Parameters:**

- `rid`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 717*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
