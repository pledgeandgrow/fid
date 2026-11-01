# log — Fides Documentation

## Contents

- [LOG_TRACE:](#log_trace:) (const)
- [LOG_DEBUG:](#log_debug:) (const)
- [LOG_INFO:](#log_info:) (const)
- [LOG_WARN:](#log_warn:) (const)
- [LOG_ERROR:](#log_error:) (const)
- [LOG_OFF:](#log_off:) (const)
- [log_set_level](#log_set_level) (function)
- [log_get_level](#log_get_level) (function)
- [log_level_from_str](#log_level_from_str) (function)
- [log_set_timestamps](#log_set_timestamps) (function)
- [log_set_json](#log_set_json) (function)
- [log_to_stdout](#log_to_stdout) (function)
- [log_to_stderr](#log_to_stderr) (function)
- [log_to_file](#log_to_file) (function)
- [log_json_escape](#log_json_escape) (function)
- [log_logf](#log_logf) (function)
- [log_log](#log_log) (function)
- [log_trace](#log_trace) (function)
- [log_debug](#log_debug) (function)
- [log_info](#log_info) (function)
- [log_warn](#log_warn) (function)
- [log_error](#log_error) (function)
- [log_tracef](#log_tracef) (function)
- [log_debugf](#log_debugf) (function)
- [log_infof](#log_infof) (function)
- [log_warnf](#log_warnf) (function)
- [log_errorf](#log_errorf) (function)

## LOG_TRACE:

```fides
pub const LOG_TRACE: i64 = 10;
```

*Defined at line 29*

---

## LOG_DEBUG:

```fides
pub const LOG_DEBUG: i64 = 20;
```

*Defined at line 30*

---

## LOG_INFO:

```fides
pub const LOG_INFO: i64 = 30;
```

*Defined at line 31*

---

## LOG_WARN:

```fides
pub const LOG_WARN: i64 = 40;
```

*Defined at line 32*

---

## LOG_ERROR:

```fides
pub const LOG_ERROR: i64 = 50;
```

*Defined at line 33*

---

## LOG_OFF:

```fides
pub const LOG_OFF: i64 = 100;
```

*Defined at line 34*

---

## log_set_level

Only emit messages at or above this level.

```fides
pub fn log_set_level(level: i64) -> void {
```

**Parameters:**

- `level`: `i64`

**Returns:** `void`

*Defined at line 47*

---

## log_get_level

```fides
pub fn log_get_level() -> i64 {
```

**Returns:** `i64`

*Defined at line 51*

---

## log_level_from_str

Parse a level name ("trace".."error", case-insensitive).
Returns -1 for unknown names.

```fides
pub fn log_level_from_str(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 57*

---

## log_set_timestamps

Turn timestamps on/off. Off by default so tests are deterministic.

```fides
pub fn log_set_timestamps(on: bool) -> void {
```

**Parameters:**

- `on`: `bool`

**Returns:** `void`

*Defined at line 69*

---

## log_set_json

Switch between logfmt text (false) and JSON lines (true).

```fides
pub fn log_set_json(on: bool) -> void {
```

**Parameters:**

- `on`: `bool`

**Returns:** `void`

*Defined at line 74*

---

## log_to_stdout

Send output to stdout (the default).

```fides
pub fn log_to_stdout() -> void {
```

**Returns:** `void`

*Defined at line 79*

---

## log_to_stderr

Send output to stderr.

```fides
pub fn log_to_stderr() -> void {
```

**Returns:** `void`

*Defined at line 85*

---

## log_to_file

Append output to a file. "" returns to stdout.

```fides
pub fn log_to_file(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 91*

---

## log_json_escape

Escape a string for JSON output (quotes, backslashes, controls).

```fides
pub fn log_json_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 125*

---

## log_logf

Core: log at an explicit level with a field list (flat k,v pairs).

```fides
pub fn log_logf(level: i64, msg: str, fields: Vec) -> void {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 261*

---

## log_log

Log at an explicit level without fields.

```fides
pub fn log_log(level: i64, msg: str) -> void {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`

**Returns:** `void`

*Defined at line 273*

---

## log_trace

```fides
pub fn log_trace(msg: str) -> void { log_log(LOG_TRACE, msg); }
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 280*

---

## log_debug

```fides
pub fn log_debug(msg: str) -> void { log_log(LOG_DEBUG, msg); }
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 281*

---

## log_info

```fides
pub fn log_info(msg: str) -> void { log_log(LOG_INFO, msg); }
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 282*

---

## log_warn

```fides
pub fn log_warn(msg: str) -> void { log_log(LOG_WARN, msg); }
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 283*

---

## log_error

```fides
pub fn log_error(msg: str) -> void { log_log(LOG_ERROR, msg); }
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 284*

---

## log_tracef

```fides
pub fn log_tracef(msg: str, fields: Vec) -> void { log_logf(LOG_TRACE, msg, fields); }
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 286*

---

## log_debugf

```fides
pub fn log_debugf(msg: str, fields: Vec) -> void { log_logf(LOG_DEBUG, msg, fields); }
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 287*

---

## log_infof

```fides
pub fn log_infof(msg: str, fields: Vec) -> void { log_logf(LOG_INFO, msg, fields); }
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 288*

---

## log_warnf

```fides
pub fn log_warnf(msg: str, fields: Vec) -> void { log_logf(LOG_WARN, msg, fields); }
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 289*

---

## log_errorf

```fides
pub fn log_errorf(msg: str, fields: Vec) -> void { log_logf(LOG_ERROR, msg, fields); }
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec`

**Returns:** `void`

*Defined at line 290*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
