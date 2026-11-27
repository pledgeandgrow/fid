[All modules](index.md)

# compiler::native_debugger -- Fides Documentation

## Contents

- [Regs](#Regs) (struct)
- [Regs_new](#Regs_new) (function)
- [UserRegs](#UserRegs) (struct)
- [UserRegs_new](#UserRegs_new) (function)
- [UserRegs_to_regs](#UserRegs_to_regs) (function)
- [NativeBreakpoint](#NativeBreakpoint) (struct)
- [NativeBreakpoint_new](#NativeBreakpoint_new) (function)
- [NativeDebugger](#NativeDebugger) (struct)
- [NativeDebugger_new](#NativeDebugger_new) (function)
- [NativeDebugger_set_source](#NativeDebugger_set_source) (function)
- [NativeDebugger_set_binary](#NativeDebugger_set_binary) (function)
- [NativeDebugger_add_breakpoint](#NativeDebugger_add_breakpoint) (function)
- [NativeDebugger_parse_bp_spec](#NativeDebugger_parse_bp_spec) (function)
- [NativeDebugger_list_breakpoints](#NativeDebugger_list_breakpoints) (function)
- [format_regs](#format_regs) (function)
- [print_native_help](#print_native_help) (function)
- [parse_command](#parse_command) (function)
- [hex_char](#hex_char) (function)
- [char_from_digit](#char_from_digit) (function)
- [NativeDebugger_compile_to_native](#NativeDebugger_compile_to_native) (function)
- [NativeDebugger_run](#NativeDebugger_run) (function)
- [NativeDebugger_run_ptrace](#NativeDebugger_run_ptrace) (function)
- [NativeDebugger_get_regs](#NativeDebugger_get_regs) (function)

## Regs

```fides
pub struct Regs {
```

**Fields:**

| Name | Type |
|------|------|
| `rax` | `i64` |
| `rbx` | `i64` |
| `rcx` | `i64` |
| `rdx` | `i64` |
| `rsi` | `i64` |
| `rdi` | `i64` |
| `rsp` | `i64` |
| `rbp` | `i64` |
| `rip` | `i64` |
| `r8` | `i64` |
| `r9` | `i64` |
| `r10` | `i64` |
| `r11` | `i64` |
| `r12` | `i64` |
| `r13` | `i64` |
| `r14` | `i64` |
| `r15` | `i64` |

*Defined at line 26*

---

## Regs_new

```fides
pub fn Regs_new() -> Regs {
```

**Returns:** `Regs`

*Defined at line 46*

---

## UserRegs

```fides
pub struct UserRegs {
```

**Fields:**

| Name | Type |
|------|------|
| `r15` | `i64` |
| `r14` | `i64` |
| `r13` | `i64` |
| `r12` | `i64` |
| `rbp` | `i64` |
| `rbx` | `i64` |
| `r11` | `i64` |
| `r10` | `i64` |
| `r9` | `i64` |
| `r8` | `i64` |
| `rax` | `i64` |
| `rcx` | `i64` |
| `rdx` | `i64` |
| `rsi` | `i64` |
| `rdi` | `i64` |
| `orig_rax` | `i64` |
| `rip` | `i64` |
| `cs` | `i64` |
| `eflags` | `i64` |
| `rsp` | `i64` |
| `ss` | `i64` |
| `fs_base` | `i64` |
| `gs_base` | `i64` |
| `ds` | `i64` |
| `es` | `i64` |
| `fs` | `i64` |
| `gs` | `i64` |

*Defined at line 56*

---

## UserRegs_new

```fides
pub fn UserRegs_new() -> UserRegs {
```

**Returns:** `UserRegs`

*Defined at line 86*

---

## UserRegs_to_regs

Convert a UserRegs (full ptrace layout) to the compact Regs struct.

```fides
pub fn UserRegs_to_regs(ur: UserRegs) -> Regs {
```

**Parameters:**

- `ur`: `UserRegs`

**Returns:** `Regs`

*Defined at line 93*

---

## NativeBreakpoint

```fides
pub struct NativeBreakpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `func_name` | `str` |
| `line` | `i64` |
| `address` | `i64,  // address in binary (0 = not yet resolved)` |

*Defined at line 101*

---

## NativeBreakpoint_new

```fides
pub fn NativeBreakpoint_new(func_name: str, line: i64, address: i64) -> NativeBreakpoint {
```

**Parameters:**

- `func_name`: `str`
- `line`: `i64`
- `address`: `i64`

**Returns:** `NativeBreakpoint`

*Defined at line 108*

---

## NativeDebugger

```fides
pub struct NativeDebugger {
```

**Fields:**

| Name | Type |
|------|------|
| `binary_path` | `str` |
| `source_path` | `str` |
| `source_lines` | `Vec<str>` |
| `breakpoints` | `Vec<NativeBreakpoint>` |

*Defined at line 116*

---

## NativeDebugger_new

```fides
pub fn NativeDebugger_new(source_path: str) -> NativeDebugger {
```

**Parameters:**

- `source_path`: `str`

**Returns:** `NativeDebugger`

*Defined at line 123*

---

## NativeDebugger_set_source

Set the source lines from the source file content.

```fides
pub fn NativeDebugger_set_source(self: &mut NativeDebugger, source: str) -> void {
```

**Parameters:**

- `self`: `&mut NativeDebugger`
- `source`: `str`

**Returns:** `void`

*Defined at line 136*

---

## NativeDebugger_set_binary

Set the binary path after compilation.

```fides
pub fn NativeDebugger_set_binary(self: &mut NativeDebugger, path: str) -> void {
```

**Parameters:**

- `self`: `&mut NativeDebugger`
- `path`: `str`

**Returns:** `void`

*Defined at line 141*

---

## NativeDebugger_add_breakpoint

Add a breakpoint. Returns the new breakpoint index.

```fides
pub fn NativeDebugger_add_breakpoint(self: &mut NativeDebugger, func_name: str, line: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut NativeDebugger`
- `func_name`: `str`
- `line`: `i64`

**Returns:** `i64`

*Defined at line 148*

---

## NativeDebugger_parse_bp_spec

Parse a breakpoint spec: "func", "func:42", "42" (line in any function)

```fides
pub fn NativeDebugger_parse_bp_spec(spec: str) -> (str, i64) {
```

**Parameters:**

- `spec`: `str`

**Returns:** `(str, i64)`

*Defined at line 155*

---

## NativeDebugger_list_breakpoints

List all breakpoints as a formatted string.

```fides
pub fn NativeDebugger_list_breakpoints(self: &NativeDebugger) -> str {
```

**Parameters:**

- `self`: `&NativeDebugger`

**Returns:** `str`

*Defined at line 171*

---

## format_regs

Format registers as a multi-line string.

```fides
pub fn format_regs(r: Regs) -> str {
```

**Parameters:**

- `r`: `Regs`

**Returns:** `str`

*Defined at line 200*

---

## print_native_help

```fides
pub fn print_native_help() -> str {
```

**Returns:** `str`

*Defined at line 206*

---

## parse_command

Parse a command line into (cmd, arg).

```fides
pub fn parse_command(line: str) -> (str, str) {
```

**Parameters:**

- `line`: `str`

**Returns:** `(str, str)`

*Defined at line 213*

---

## hex_char

```fides
pub fn hex_char(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 244*

---

## char_from_digit

```fides
pub fn char_from_digit(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 250*

---

## NativeDebugger_compile_to_native

```fides
pub fn NativeDebugger_compile_to_native(self: &mut NativeDebugger) -> str {
```

**Parameters:**

- `self`: `&mut NativeDebugger`

**Returns:** `str`

*Defined at line 326*

---

## NativeDebugger_run

```fides
pub fn NativeDebugger_run(self: &mut NativeDebugger) -> i64 {
```

**Parameters:**

- `self`: `&mut NativeDebugger`

**Returns:** `i64`

*Defined at line 382*

---

## NativeDebugger_run_ptrace

```fides
pub fn NativeDebugger_run_ptrace(self: &mut NativeDebugger) -> i64 {
```

**Parameters:**

- `self`: `&mut NativeDebugger`

**Returns:** `i64`

*Defined at line 424*

---

## NativeDebugger_get_regs

```fides
pub fn NativeDebugger_get_regs(self: &NativeDebugger, _pid: i64) -> Regs {
```

**Parameters:**

- `self`: `&NativeDebugger`
- `_pid`: `i64`

**Returns:** `Regs`

*Defined at line 516*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
