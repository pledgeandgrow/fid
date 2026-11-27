[All modules](index.md)

# compiler::debugger -- Fides Documentation

## Contents

- [Breakpoint](#Breakpoint) (struct)
- [Breakpoint_new](#Breakpoint_new) (function)
- [Debugger](#Debugger) (struct)
- [Debugger_new](#Debugger_new) (function)
- [Debugger_parse_bp_spec](#Debugger_parse_bp_spec) (function)
- [Debugger_should_break](#Debugger_should_break) (function)
- [Debugger_add_breakpoint](#Debugger_add_breakpoint) (function)
- [Debugger_delete_breakpoint](#Debugger_delete_breakpoint) (function)
- [Debugger_list_breakpoints](#Debugger_list_breakpoints) (function)
- [Debugger_add_watch](#Debugger_add_watch) (function)
- [Debugger_update_watch](#Debugger_update_watch) (function)
- [Debugger_list_watches](#Debugger_list_watches) (function)
- [Debugger_list_source](#Debugger_list_source) (function)
- [format_value](#format_value) (function)
- [print_debugger_help](#print_debugger_help) (function)
- [parse_command](#parse_command) (function)
- [char_from_digit](#char_from_digit) (function)
- [hex_char](#hex_char) (function)
- [DebuggerState](#DebuggerState) (struct)
- [DebuggerState_new](#DebuggerState_new) (function)
- [DebuggerState_from_debugger](#DebuggerState_from_debugger) (function)
- [TraceEntry](#TraceEntry) (struct)
- [TraceEntry_new](#TraceEntry_new) (function)
- [print_locals](#print_locals) (function)
- [print_var](#print_var) (function)
- [show_ir](#show_ir) (function)
- [show_ir_with_current](#show_ir_with_current) (function)
- [show_effects](#show_effects) (function)
- [list_functions](#list_functions) (function)
- [DBG_RUN:](#DBG_RUN:) (const)
- [DBG_STEP:](#DBG_STEP:) (const)
- [DBG_NEXT:](#DBG_NEXT:) (const)
- [DBG_FINISH:](#DBG_FINISH:) (const)
- [Debugger_run](#Debugger_run) (function)
- [save_trace](#save_trace) (function)
- [load_trace](#load_trace) (function)
- [replay_trace](#replay_trace) (function)

## Breakpoint

```fides
pub struct Breakpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `func_name` | `str` |
| `line` | `i64` |
| `hit_count` | `i64` |

*Defined at line 39*

---

## Breakpoint_new

```fides
pub fn Breakpoint_new(id: i64, func_name: str, line: i64) -> Breakpoint {
```

**Parameters:**

- `id`: `i64`
- `func_name`: `str`
- `line`: `i64`

**Returns:** `Breakpoint`

*Defined at line 47*

---

## Debugger

```fides
pub struct Debugger {
```

**Fields:**

| Name | Type |
|------|------|
| `breakpoints` | `Vec<Breakpoint>` |
| `next_bp_id` | `i64` |
| `stepping` | `bool` |
| `step_over` | `bool` |
| `step_over_depth` | `i64` |
| `watches` | `Vec<(str, str)>` |
| `recording` | `bool` |
| `source_lines` | `Vec<str>` |
| `source_path` | `str` |
| `mode` | `i64` |
| `mode_depth` | `i64` |
| `force_stop` | `bool` |
| `quit` | `bool` |
| `echo` | `bool` |
| `user_fns` | `i64` |
| `frame_names` | `Vec<str>` |
| `frame_lines` | `Vec<i64>` |
| `frame_args` | `Vec<str>` |
| `last_cmd` | `str` |

*Defined at line 55*

---

## Debugger_new

```fides
pub fn Debugger_new(source: str, source_path: str) -> Debugger {
```

**Parameters:**

- `source`: `str`
- `source_path`: `str`

**Returns:** `Debugger`

*Defined at line 85*

---

## Debugger_parse_bp_spec

Parse a breakpoint spec: "func", "func:42", "42" (line in any function)

```fides
pub fn Debugger_parse_bp_spec(self: &Debugger, spec: str) -> (str, i64) {
```

**Parameters:**

- `self`: `&Debugger`
- `spec`: `str`

**Returns:** `(str, i64)`

*Defined at line 93*

---

## Debugger_should_break

Check if we should break at the current position

```fides
pub fn Debugger_should_break(self: &mut Debugger, func_name: str, source_line: i64, call_depth: i64) -> bool {
```

**Parameters:**

- `self`: `&mut Debugger`
- `func_name`: `str`
- `source_line`: `i64`
- `call_depth`: `i64`

**Returns:** `bool`

*Defined at line 109*

---

## Debugger_add_breakpoint

Add a breakpoint. Returns the new breakpoint id.

```fides
pub fn Debugger_add_breakpoint(self: &mut Debugger, func_name: str, line: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Debugger`
- `func_name`: `str`
- `line`: `i64`

**Returns:** `i64`

*Defined at line 140*

---

## Debugger_delete_breakpoint

Delete a breakpoint by id. Returns true if deleted.

```fides
pub fn Debugger_delete_breakpoint(self: &mut Debugger, id: i64) -> bool {
```

**Parameters:**

- `self`: `&mut Debugger`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 148*

---

## Debugger_list_breakpoints

List all breakpoints as a formatted string.

```fides
pub fn Debugger_list_breakpoints(self: &Debugger) -> str {
```

**Parameters:**

- `self`: `&Debugger`

**Returns:** `str`

*Defined at line 161*

---

## Debugger_add_watch

Add a watch on a variable.

```fides
pub fn Debugger_add_watch(self: &mut Debugger, name: str) -> void {
```

**Parameters:**

- `self`: `&mut Debugger`
- `name`: `str`

**Returns:** `void`

*Defined at line 184*

---

## Debugger_update_watch

Update a watch's last-seen value. Returns true if value changed.

```fides
pub fn Debugger_update_watch(self: &mut Debugger, name: str, value_str: str) -> bool {
```

**Parameters:**

- `self`: `&mut Debugger`
- `name`: `str`
- `value_str`: `str`

**Returns:** `bool`

*Defined at line 189*

---

## Debugger_list_watches

List all watches as a formatted string.

```fides
pub fn Debugger_list_watches(self: &Debugger) -> str {
```

**Parameters:**

- `self`: `&Debugger`

**Returns:** `str`

*Defined at line 205*

---

## Debugger_list_source

Show source around a line.

```fides
pub fn Debugger_list_source(self: &Debugger, line: i64, context: i64) -> str {
```

**Parameters:**

- `self`: `&Debugger`
- `line`: `i64`
- `context`: `i64`

**Returns:** `str`

*Defined at line 223*

---

## format_value

Format a Value for display.

```fides
pub fn format_value(v: Value) -> str {
```

**Parameters:**

- `v`: `Value`

**Returns:** `str`

*Defined at line 258*

---

## print_debugger_help

```fides
pub fn print_debugger_help() -> str {
```

**Returns:** `str`

*Defined at line 290*

---

## parse_command

Parse a command line into (cmd, arg).

```fides
pub fn parse_command(line: str) -> (str, str) {
```

**Parameters:**

- `line`: `str`

**Returns:** `(str, str)`

*Defined at line 297*

---

## char_from_digit

```fides
pub fn char_from_digit(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 335*

---

## hex_char

```fides
pub fn hex_char(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 341*

---

## DebuggerState

```fides
pub struct DebuggerState {
```

**Fields:**

| Name | Type |
|------|------|
| `breakpoints` | `Vec<Breakpoint>` |
| `next_bp_id` | `i64` |
| `stepping` | `bool` |
| `step_over` | `bool` |
| `watches` | `Vec<(str, str)>` |
| `recording` | `bool` |
| `last_cmd` | `str` |
| `quit` | `bool` |
| `current_func` | `str` |
| `current_block` | `str` |
| `current_inst` | `i64` |
| `current_source_line` | `i64` |
| `call_depth` | `i64` |
| `trace` | `Vec<TraceEntry>` |

*Defined at line 420*

---

## DebuggerState_new

```fides
pub fn DebuggerState_new() -> DebuggerState {
```

**Returns:** `DebuggerState`

*Defined at line 438*

---

## DebuggerState_from_debugger

Initialize DebuggerState from an existing Debugger (copies breakpoints/watches).

```fides
pub fn DebuggerState_from_debugger(dbg: &Debugger) -> DebuggerState {
```

**Parameters:**

- `dbg`: `&Debugger`

**Returns:** `DebuggerState`

*Defined at line 445*

---

## TraceEntry

```fides
pub struct TraceEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `func_name` | `str` |
| `block_id` | `i64` |
| `inst_id` | `i64` |
| `source_line` | `i64` |
| `value_repr` | `OptStr` |

*Defined at line 455*

---

## TraceEntry_new

```fides
pub fn TraceEntry_new(func_name: str, block_id: i64, inst_id: i64, source_line: i64) -> TraceEntry {
```

**Parameters:**

- `func_name`: `str`
- `block_id`: `i64`
- `inst_id`: `i64`
- `source_line`: `i64`

**Returns:** `TraceEntry`

*Defined at line 463*

---

## print_locals

```fides
pub fn print_locals(values: Vec<Value>, func: IrFunction) -> str {
```

**Parameters:**

- `values`: `Vec<Value>`
- `func`: `IrFunction`

**Returns:** `str`

*Defined at line 484*

---

## print_var

```fides
pub fn print_var(name: str, values: Vec<Value>, func: IrFunction) -> str {
```

**Parameters:**

- `name`: `str`
- `values`: `Vec<Value>`
- `func`: `IrFunction`

**Returns:** `str`

*Defined at line 544*

---

## show_ir

```fides
pub fn show_ir(func: IrFunction) -> str {
```

**Parameters:**

- `func`: `IrFunction`

**Returns:** `str`

*Defined at line 594*

---

## show_ir_with_current

```fides
pub fn show_ir_with_current(func: IrFunction, current_inst: i64) -> str {
```

**Parameters:**

- `func`: `IrFunction`
- `current_inst`: `i64`

**Returns:** `str`

*Defined at line 656*

---

## show_effects

```fides
pub fn show_effects(func: IrFunction) -> str {
```

**Parameters:**

- `func`: `IrFunction`

**Returns:** `str`

*Defined at line 720*

---

## list_functions

```fides
pub fn list_functions(module: IrModule) -> str {
```

**Parameters:**

- `module`: `IrModule`

**Returns:** `str`

*Defined at line 741*

---

## DBG_RUN:

```fides
pub const DBG_RUN: i64 = 0;  // to a breakpoint or the end
```

*Defined at line 776*

---

## DBG_STEP:

```fides
pub const DBG_STEP: i64 = 1;  // to the next source line, into calls
```

*Defined at line 777*

---

## DBG_NEXT:

```fides
pub const DBG_NEXT: i64 = 2;  // to the next line of this frame or a caller
```

*Defined at line 778*

---

## DBG_FINISH:

```fides
pub const DBG_FINISH: i64 = 3;  // until this frame returns
```

*Defined at line 779*

---

## Debugger_run

`fid debug <file.fd>`: build the file, then the session. Returns the
program's exit code (0 when the session was quit, 1 when the build or
the program failed).

```fides
pub fn Debugger_run(self: &mut Debugger, source: str, source_path: str) -> i64 {
```

**Parameters:**

- `self`: `&mut Debugger`
- `source`: `str`
- `source_path`: `str`

**Returns:** `i64`

*Defined at line 1976*

---

## save_trace

```fides
pub fn save_trace(trace: Vec<TraceEntry>, path: str) -> bool {
```

**Parameters:**

- `trace`: `Vec<TraceEntry>`
- `path`: `str`

**Returns:** `bool`

*Defined at line 2044*

---

## load_trace

```fides
pub fn load_trace(path: str) -> Vec<TraceEntry> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<TraceEntry>`

*Defined at line 2070*

---

## replay_trace

```fides
pub fn replay_trace(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2131*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
