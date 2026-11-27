[All modules](index.md)

# compiler::hotreload -- Fides Documentation

## Contents

- [FunctionSnapshot](#FunctionSnapshot) (struct)
- [FunctionSnapshot_new](#FunctionSnapshot_new) (function)
- [FunctionSnapshot_add](#FunctionSnapshot_add) (function)
- [FunctionSnapshot_diff](#FunctionSnapshot_diff) (function)
- [FunctionSnapshot_function_names](#FunctionSnapshot_function_names) (function)
- [FunctionSnapshot_from_ir](#FunctionSnapshot_from_ir) (function)
- [FunctionDiff](#FunctionDiff) (struct)
- [FunctionDiff_new](#FunctionDiff_new) (function)
- [FunctionDiff_is_empty](#FunctionDiff_is_empty) (function)
- [FunctionDiff_total_changes](#FunctionDiff_total_changes) (function)
- [hash_str](#hash_str) (function)
- [JitResult](#JitResult) (struct)
- [JitResult_new](#JitResult_new) (function)
- [compile_and_run](#compile_and_run) (function)
- [PersistentState](#PersistentState) (struct)
- [PersistentState_for_source](#PersistentState_for_source) (function)
- [PersistentState_load](#PersistentState_load) (function)
- [PersistentState_save](#PersistentState_save) (function)
- [PersistentState_set](#PersistentState_set) (function)
- [PersistentState_get](#PersistentState_get) (function)
- [PersistentState_clear](#PersistentState_clear) (function)
- [Watcher](#Watcher) (struct)
- [Watcher_new](#Watcher_new) (function)
- [Watcher_watch](#Watcher_watch) (function)
- [Watcher_poll](#Watcher_poll) (function)
- [Watcher_run](#Watcher_run) (function)
- [Debugger](#Debugger) (struct)
- [Debugger_new](#Debugger_new) (function)
- [Debugger_enter_function](#Debugger_enter_function) (function)
- [Debugger_exit_function](#Debugger_exit_function) (function)
- [Debugger_step](#Debugger_step) (function)
- [Debugger_stack_depth](#Debugger_stack_depth) (function)
- [Debugger_current_function](#Debugger_current_function) (function)
- [Debugger_current_offset](#Debugger_current_offset) (function)
- [char_from_digit](#char_from_digit) (function)

## FunctionSnapshot

A snapshot of function hashes from a compilation.
Used to detect which functions changed between reloads.

```fides
pub struct FunctionSnapshot {
```

**Fields:**

| Name | Type |
|------|------|
| `hashes` | `Vec<(str, i64)>` |

*Defined at line 26*

---

## FunctionSnapshot_new

```fides
pub fn FunctionSnapshot_new() -> FunctionSnapshot {
```

**Returns:** `FunctionSnapshot`

*Defined at line 30*

---

## FunctionSnapshot_add

Add a function hash to the snapshot.

```fides
pub fn FunctionSnapshot_add(self: &mut FunctionSnapshot, name: str, content: str) -> void {
```

**Parameters:**

- `self`: `&mut FunctionSnapshot`
- `name`: `str`
- `content`: `str`

**Returns:** `void`

*Defined at line 66*

---

## FunctionSnapshot_diff

Compare this snapshot with a previous one and return the diff.

```fides
pub fn FunctionSnapshot_diff(self: &FunctionSnapshot, prev: &FunctionSnapshot) -> FunctionDiff {
```

**Parameters:**

- `self`: `&FunctionSnapshot`
- `prev`: `&FunctionSnapshot`

**Returns:** `FunctionDiff`

*Defined at line 72*

---

## FunctionSnapshot_function_names

Get all function names in this snapshot.

```fides
pub fn FunctionSnapshot_function_names(self: &FunctionSnapshot) -> Vec<str> {
```

**Parameters:**

- `self`: `&FunctionSnapshot`

**Returns:** `Vec<str>`

*Defined at line 123*

---

## FunctionSnapshot_from_ir

Build a snapshot from an IrModule by hashing each function's content.
Adapted from src/hotreload.rs:37 (FunctionSnapshot::from_l2).

The Rust reference builds from an L2Module (PIR L2), hashing each
function's debug repr (name, params, ret_ty, entry, blocks, values).
In the self-hosted Fides compiler, we adapt this to build from an IrModule
(the SSA IR before PIR lowering) instead, because:
1. L2Module types in compiler/pir.fd require additional PIR pipeline
imports (lower_ir_to_l1, lower_l1_to_l2) that add complexity
2. The IrModule is directly available from lower_to_ir_module()
3. The IR captures the same function-level structure (name, params,
return type, blocks, instructions, value metas)

The content string for each function includes:
- function name
- return type (via ir_type_to_str)
- entry block ID
- each param: value ID + type
- each block: block ID + instruction value IDs + block param IDs/types
- each instruction: inst_kind_name (operation kind)
- each value meta: value ID + type
This ensures the hash changes when any structural element of the function
changes, providing equivalent change detection to the L2-based approach.

```fides
pub fn FunctionSnapshot_from_ir(module: &IrModule) -> FunctionSnapshot {
```

**Parameters:**

- `module`: `&IrModule`

**Returns:** `FunctionSnapshot`

*Defined at line 157*

---

## FunctionDiff

The result of comparing two function snapshots.

```fides
pub struct FunctionDiff {
```

**Fields:**

| Name | Type |
|------|------|
| `added` | `Vec<str>` |
| `modified` | `Vec<str>` |
| `removed` | `Vec<str>` |

*Defined at line 221*

---

## FunctionDiff_new

```fides
pub fn FunctionDiff_new(added: Vec<str>, modified: Vec<str>, removed: Vec<str>) -> FunctionDiff {
```

**Parameters:**

- `added`: `Vec<str>`
- `modified`: `Vec<str>`
- `removed`: `Vec<str>`

**Returns:** `FunctionDiff`

*Defined at line 227*

---

## FunctionDiff_is_empty

Returns true if nothing changed.

```fides
pub fn FunctionDiff_is_empty(self: &FunctionDiff) -> bool {
```

**Parameters:**

- `self`: `&FunctionDiff`

**Returns:** `bool`

*Defined at line 234*

---

## FunctionDiff_total_changes

Total number of changed functions.

```fides
pub fn FunctionDiff_total_changes(self: &FunctionDiff) -> i64 {
```

**Parameters:**

- `self`: `&FunctionDiff`

**Returns:** `i64`

*Defined at line 239*

---

## hash_str

```fides
pub fn hash_str(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 245*

---

## JitResult

Result of a JIT execution.
Ported from src/hotreload.rs:128.

In the Rust reference, this is the result of compiling to a native ELF64
binary and running it as a child process. In the self-hosted Fides compiler,
we adapt this to use the IR interpreter instead of native code generation,
since the full PIR → codegen → ELF64 pipeline is wired into
compiler/compiler.fd. The fields retain the same meaning:
- exit_code: interpreter exit code (0 = success, 1 = error)
- stdout/stderr: captured output (structural; interpreter prints directly)
- compile_time_ms: time to lower source to IR
- exec_time_ms: time to run the interpreter
- binary_size: 0 (no native binary produced in interpreter mode)

```fides
pub struct JitResult {
```

**Fields:**

| Name | Type |
|------|------|
| `exit_code` | `i32` |
| `stdout` | `str` |
| `stderr` | `str` |
| `compile_time_ms` | `i64` |
| `exec_time_ms` | `i64` |
| `binary_size` | `i64` |

*Defined at line 271*

---

## JitResult_new

```fides
pub fn JitResult_new(exit_code: i32, stdout: str, stderr: str, compile_time_ms: i64, exec_time_ms: i64, binary_size: i64,) -> JitResult {
```

**Parameters:**

- `exit_code`: `i32`
- `stdout`: `str`
- `stderr`: `str`
- `compile_time_ms`: `i64`
- `exec_time_ms`: `i64`
- `binary_size`: `i64`

**Returns:** `JitResult`

*Defined at line 280*

---

## compile_and_run

Compile source code and run it via the IR interpreter.
Adapted from src/hotreload.rs:146 (compile_and_run).

The Rust reference compiles to a native ELF64 executable and runs it as a
child process. In the self-hosted Fides compiler, we adapt this to:
1. Call lower_to_ir_module(source) to build IR (from compiler::compiler)
2. Run the IR interpreter via interpret_ir_module (from compiler::interp)
3. Return a JitResult with success/failure info

The `source_path` and `do_opt` parameters are accepted for API compatibility
with the Rust reference but are not used in interpreter mode (optimization
is handled by the PIR pipeline in compiler.fd when using native codegen).

```fides
pub fn compile_and_run(source: str, _source_path: str, _do_opt: bool) -> JitResult {
```

**Parameters:**

- `source`: `str`
- `_source_path`: `str`
- `_do_opt`: `bool`

**Returns:** `JitResult`

*Defined at line 298*

---

## PersistentState

State that persists across hot reloads.
Stored in a temp file so it survives between child process executions.

```fides
pub struct PersistentState {
```

**Fields:**

| Name | Type |
|------|------|
| `state_file` | `str` |
| `data` | `Vec<(str, str)>` |

*Defined at line 339*

---

## PersistentState_for_source

```fides
pub fn PersistentState_for_source(source_path: str) -> PersistentState {
```

**Parameters:**

- `source_path`: `str`

**Returns:** `PersistentState`

*Defined at line 344*

---

## PersistentState_load

Load state from the temp file.

```fides
pub fn PersistentState_load(self: &mut PersistentState) -> void {
```

**Parameters:**

- `self`: `&mut PersistentState`

**Returns:** `void`

*Defined at line 355*

---

## PersistentState_save

Save state to the temp file.

```fides
pub fn PersistentState_save(self: &PersistentState) -> void {
```

**Parameters:**

- `self`: `&PersistentState`

**Returns:** `void`

*Defined at line 385*

---

## PersistentState_set

Set a state value.

```fides
pub fn PersistentState_set(self: &mut PersistentState, key: str, value: str) -> void {
```

**Parameters:**

- `self`: `&mut PersistentState`
- `key`: `str`
- `value`: `str`

**Returns:** `void`

*Defined at line 401*

---

## PersistentState_get

Get a state value. Returns empty string if not found.

```fides
pub fn PersistentState_get(self: &PersistentState, key: str) -> str {
```

**Parameters:**

- `self`: `&PersistentState`
- `key`: `str`

**Returns:** `str`

*Defined at line 415*

---

## PersistentState_clear

Clear all state.

```fides
pub fn PersistentState_clear(self: &mut PersistentState) -> void {
```

**Parameters:**

- `self`: `&mut PersistentState`

**Returns:** `void`

*Defined at line 429*

---

## Watcher

File watcher: monitors source files for changes.

```fides
pub struct Watcher {
```

**Fields:**

| Name | Type |
|------|------|
| `files` | `Vec<(str, i64)>,  // (path, last_modified_ms)` |

*Defined at line 438*

---

## Watcher_new

```fides
pub fn Watcher_new() -> Watcher {
```

**Returns:** `Watcher`

*Defined at line 442*

---

## Watcher_watch

Add a file to watch

```fides
pub fn Watcher_watch(self: &mut Watcher, path: str) -> void {
```

**Parameters:**

- `self`: `&mut Watcher`
- `path`: `str`

**Returns:** `void`

*Defined at line 449*

---

## Watcher_poll

Check all watched files for changes. Returns index of first changed file, or -1.

```fides
pub fn Watcher_poll(self: &mut Watcher) -> i64 {
```

**Parameters:**

- `self`: `&mut Watcher`

**Returns:** `i64`

*Defined at line 455*

---

## Watcher_run

Run the watcher loop, polling at the given interval (in milliseconds).
Adapted from src/hotreload.rs:328 (Watcher::run).

The Rust reference runs an infinite loop calling self.poll() and sleeping
between iterations. In the self-hosted Fides compiler, we adapt this to:
1. Loop with a simple counter limit (max_iterations) to avoid infinite
loops in case of issues (no thread::sleep available, and an infinite
loop would hang the compiler process)
2. Call Watcher_poll to check for changes
3. If changes detected (poll returns >= 0), call compile_and_run on the
changed file's source
4. Return the number of recompilations triggered

The `interval_ms` parameter is accepted for API compatibility but is not
used (no sleep facility available in the Fides runtime).

```fides
pub fn Watcher_run(self: &mut Watcher, interval_ms: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut Watcher`
- `interval_ms`: `i64`

**Returns:** `i64`

*Defined at line 485*

---

## Debugger

Debug state for the hot-reload debugger.
Tracks call stack and current position for debugging hot-reloaded code.

```fides
pub struct Debugger {
```

**Fields:**

| Name | Type |
|------|------|
| `current_function` | `str` |
| `has_current` | `bool` |
| `current_offset` | `i64` |
| `step_into` | `bool` |
| `call_stack` | `Vec<str>` |

*Defined at line 519*

---

## Debugger_new

```fides
pub fn Debugger_new() -> Debugger {
```

**Returns:** `Debugger`

*Defined at line 527*

---

## Debugger_enter_function

Enter a function (push onto call stack)

```fides
pub fn Debugger_enter_function(self: &mut Debugger, name: str) -> void {
```

**Parameters:**

- `self`: `&mut Debugger`
- `name`: `str`

**Returns:** `void`

*Defined at line 534*

---

## Debugger_exit_function

Exit the current function (pop from call stack)

```fides
pub fn Debugger_exit_function(self: &mut Debugger) -> void {
```

**Parameters:**

- `self`: `&mut Debugger`

**Returns:** `void`

*Defined at line 542*

---

## Debugger_step

Advance the instruction pointer

```fides
pub fn Debugger_step(self: &mut Debugger) -> void {
```

**Parameters:**

- `self`: `&mut Debugger`

**Returns:** `void`

*Defined at line 557*

---

## Debugger_stack_depth

Get the current call stack depth

```fides
pub fn Debugger_stack_depth(self: &Debugger) -> i64 {
```

**Parameters:**

- `self`: `&Debugger`

**Returns:** `i64`

*Defined at line 562*

---

## Debugger_current_function

Get the current function name

```fides
pub fn Debugger_current_function(self: &Debugger) -> str {
```

**Parameters:**

- `self`: `&Debugger`

**Returns:** `str`

*Defined at line 567*

---

## Debugger_current_offset

Get the current instruction offset

```fides
pub fn Debugger_current_offset(self: &Debugger) -> i64 {
```

**Parameters:**

- `self`: `&Debugger`

**Returns:** `i64`

*Defined at line 572*

---

## char_from_digit

```fides
pub fn char_from_digit(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 619*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
