[All modules](index.md)

# log -- Fides Documentation

## Contents

- [LOG_TRACE:](#LOG_TRACE:) (const)
- [LOG_DEBUG:](#LOG_DEBUG:) (const)
- [LOG_INFO:](#LOG_INFO:) (const)
- [LOG_WARN:](#LOG_WARN:) (const)
- [LOG_ERROR:](#LOG_ERROR:) (const)
- [LOG_OFF:](#LOG_OFF:) (const)
- [log_set_level](#log_set_level) (function)
- [log_get_level](#log_get_level) (function)
- [log_level_from_str](#log_level_from_str) (function)
- [log_set_timestamps](#log_set_timestamps) (function)
- [log_set_json](#log_set_json) (function)
- [log_to_stdout](#log_to_stdout) (function)
- [log_to_stderr](#log_to_stderr) (function)
- [log_to_file](#log_to_file) (function)
- [log_json_escape](#log_json_escape) (function)
- [log_render_text](#log_render_text) (function)
- [log_render_json](#log_render_json) (function)
- [log_logf](#log_logf) (function)
- [log_log](#log_log) (function)
- [log_set_module_level](#log_set_module_level) (function)
- [log_module_level](#log_module_level) (function)
- [log_clear_module_level](#log_clear_module_level) (function)
- [logmf](#logmf) (function)
- [logm](#logm) (function)
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

*Defined at line 28*

---

## LOG_DEBUG:

```fides
pub const LOG_DEBUG: i64 = 20;
```

*Defined at line 29*

---

## LOG_INFO:

```fides
pub const LOG_INFO: i64 = 30;
```

*Defined at line 30*

---

## LOG_WARN:

```fides
pub const LOG_WARN: i64 = 40;
```

*Defined at line 31*

---

## LOG_ERROR:

```fides
pub const LOG_ERROR: i64 = 50;
```

*Defined at line 32*

---

## LOG_OFF:

```fides
pub const LOG_OFF: i64 = 100;
```

*Defined at line 33*

---

## log_set_level

Only emit messages at or above this level.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_set_level(LOG_WARN);
assert_eq(log_get_level(), LOG_WARN);
log_info("not printed");
log_warn("printed");
```

```fides
pub fn log_set_level(level: i64) -> void {
```

**Parameters:**

- `level`: `i64`

**Returns:** `void`

*Defined at line 50*

---

## log_get_level

The current global level (LOG_INFO unless changed).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(log_get_level(), LOG_INFO);
```

```fides
pub fn log_get_level() -> i64 {
```

**Returns:** `i64`

*Defined at line 58*

---

## log_level_from_str

Parse a level name ("trace".."error", case-insensitive).
Returns -1 for unknown names.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(log_level_from_str("WARN"), LOG_WARN);
assert_eq(log_level_from_str("verbose"), -1);
```

```fides
pub fn log_level_from_str(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 68*

---

## log_set_timestamps

Turn timestamps on/off. Off by default so tests are deterministic.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_set_timestamps(false);
log_info("no ts= field on this line");
```

```fides
pub fn log_set_timestamps(on: bool) -> void {
```

**Parameters:**

- `on`: `bool`

**Returns:** `void`

*Defined at line 96*

---

## log_set_json

Switch between logfmt text (false) and JSON lines (true).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_set_json(true);
log_info("as a JSON line");
```

```fides
pub fn log_set_json(on: bool) -> void {
```

**Parameters:**

- `on`: `bool`

**Returns:** `void`

*Defined at line 109*

---

## log_to_stdout

Send output to stdout (the default).

```fides
pub fn log_to_stdout() -> void {
```

**Returns:** `void`

*Defined at line 118*

---

## log_to_stderr

Send output to stderr.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_to_stderr();
log_error("goes to stderr");
log_to_stdout();
```

```fides
pub fn log_to_stderr() -> void {
```

**Returns:** `void`

*Defined at line 129*

---

## log_to_file

Append output to a file. "" returns to stdout.

```fides
pub fn log_to_file(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 135*

---

## log_json_escape

Escape a string for JSON output (quotes, backslashes, controls).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(log_json_escape("a\"b\n"), "a\\\"b\\n");
```

```fides
pub fn log_json_escape(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 179*

---

## log_render_text

Build the logfmt text line. Public so a caller (or a test) can check the
output against the format without emitting it.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var f: Vec<str> = Vec::new();
f.push("port"); f.push("8080"); f.push("host"); f.push("a b");
var line = log_render_text(LOG_INFO, "listening", f);
println(line);
assert_eq(line, "level=info msg=listening port=8080 host=\"a b\"");
```

```fides
pub fn log_render_text(level: i64, msg: str, fields: Vec<str>) -> str {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `str`

*Defined at line 279*

---

## log_render_json

Build the JSON line. Public so a caller (or a test) can parse the output
against the JSON spec without emitting it (CO4).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var f: Vec<str> = Vec::new();
f.push("port"); f.push("8080");
assert_eq(log_render_json(LOG_WARN, "slow", f), "{\"level\":\"warn\",\"msg\":\"slow\",\"port\":8080}");
```

```fides
pub fn log_render_json(level: i64, msg: str, fields: Vec<str>) -> str {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `str`

*Defined at line 304*

---

## log_logf

Core: log at an explicit level with a field list (flat k,v pairs).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var f: Vec<str> = Vec::new();
f.push("user"); f.push("ada");
log_logf(LOG_INFO, "login", f);
```

```fides
pub fn log_logf(level: i64, msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 353*

---

## log_log

Log at an explicit level without fields.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_log(LOG_ERROR, "disk full");
```

```fides
pub fn log_log(level: i64, msg: str) -> void {
```

**Parameters:**

- `level`: `i64`
- `msg`: `str`

**Returns:** `void`

*Defined at line 370*

---

## log_set_module_level

Emit `module`'s messages only at or above `level`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_set_module_level("db", LOG_ERROR);
assert_eq(log_module_level("db"), LOG_ERROR);
assert_eq(log_module_level("http"), LOG_INFO);
logm(LOG_INFO, "db", "not printed");
```

```fides
pub fn log_set_module_level(module: str, level: i64) -> void {
```

**Parameters:**

- `module`: `str`
- `level`: `i64`

**Returns:** `void`

*Defined at line 388*

---

## log_module_level

The level that applies to `module`: its override, else the global one.

```fides
pub fn log_module_level(module: str) -> i64 {
```

**Parameters:**

- `module`: `str`

**Returns:** `i64`

*Defined at line 396*

---

## log_clear_module_level

Drop `module`'s override: it follows the global level again.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_set_module_level("db", LOG_OFF);
log_clear_module_level("db");
assert_eq(log_module_level("db"), log_get_level());
```

```fides
pub fn log_clear_module_level(module: str) -> void {
```

**Parameters:**

- `module`: `str`

**Returns:** `void`

*Defined at line 413*

---

## logmf

Log `msg` at `level` for `module`, honoring its override; the module
name is emitted as the "module" field.

```fides
pub fn logmf(level: i64, module: str, msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `level`: `i64`
- `module`: `str`
- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 422*

---

## logm

As logmf, without fields.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
logm(LOG_ERROR, "db", "connection lost");
```

```fides
pub fn logm(level: i64, module: str, msg: str) -> void {
```

**Parameters:**

- `level`: `i64`
- `module`: `str`
- `msg`: `str`

**Returns:** `void`

*Defined at line 447*

---

## log_trace

```fides
pub fn log_trace(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 454*

---

## log_debug

```fides
pub fn log_debug(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 457*

---

## log_info

Log `msg` at INFO level (log_trace, log_debug, log_warn and log_error
are the same at their level).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
log_info("server started");
```

```fides
pub fn log_info(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 465*

---

## log_warn

```fides
pub fn log_warn(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 468*

---

## log_error

```fides
pub fn log_error(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 471*

---

## log_tracef

```fides
pub fn log_tracef(msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 475*

---

## log_debugf

```fides
pub fn log_debugf(msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 478*

---

## log_infof

Log `msg` at INFO level with flat key, value fields (log_tracef,
log_debugf, log_warnf and log_errorf are the same at their level).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var f: Vec<str> = Vec::new();
f.push("port"); f.push("8080");
log_infof("listening", f);
```

```fides
pub fn log_infof(msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 488*

---

## log_warnf

```fides
pub fn log_warnf(msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 491*

---

## log_errorf

```fides
pub fn log_errorf(msg: str, fields: Vec<str>) -> void {
```

**Parameters:**

- `msg`: `str`
- `fields`: `Vec<str>`

**Returns:** `void`

*Defined at line 494*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
