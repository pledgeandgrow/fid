# typed_strings — Fides Documentation

## Contents

- [SQLString](#sqlstring) (struct)
- [SQLString](#sqlstring) (impl)
- [sql_query](#sql_query) (function)
- [sql_literal](#sql_literal) (function)
- [sql_escape](#sql_escape) (function)
- [sql_bind](#sql_bind) (function)
- [sql_bind_int](#sql_bind_int) (function)
- [ShellString](#shellstring) (struct)
- [ShellString](#shellstring) (impl)
- [shell_cmd](#shell_cmd) (function)
- [shell_escape](#shell_escape) (function)
- [HTMLString](#htmlstring) (struct)
- [HTMLString](#htmlstring) (impl)
- [html_literal](#html_literal) (function)
- [html_escape](#html_escape) (function)
- [html_attr_escape](#html_attr_escape) (function)
- [JSONString](#jsonstring) (struct)
- [JSONString](#jsonstring) (impl)
- [json_escape](#json_escape) (function)
- [PathString](#pathstring) (struct)
- [PathString](#pathstring) (impl)
- [path_safe](#path_safe) (function)
- [path_literal](#path_literal) (function)

## SQLString

A SQL query string. Must be constructed via `sql_query()` or `sql_literal()`.
Cannot be concatenated with untrusted input — use `sql_bind()` for parameters.

```fides
pub struct SQLString {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |

*Defined at line 38*

---

## SQLString

```fides
impl SQLString {
```

*Defined at line 42*

---

## sql_query

Create a SQLString from a literal query string.
This is the safe constructor — only use with static query text.

```fides
pub fn sql_query(sql: str) -> SQLString {
```

**Parameters:**

- `sql`: `str`

**Returns:** `SQLString`

*Defined at line 73*

---

## sql_literal

Create a SQLString from a literal query (alias for sql_query).

```fides
pub fn sql_literal(sql: str) -> SQLString {
```

**Parameters:**

- `sql`: `str`

**Returns:** `SQLString`

*Defined at line 78*

---

## sql_escape

Escape a string for safe inclusion in SQL. Returns a SQLString.
Uses single-quote doubling (standard SQL escaping).
WARNING: parameterized queries (sql_bind) are always safer than escaping.

```fides
pub fn sql_escape(input: str) -> SQLString {
```

**Parameters:**

- `input`: `str`

**Returns:** `SQLString`

*Defined at line 85*

---

## sql_bind

Bind a parameter into a SQL query. Replaces the first `?` placeholder
with the string-escaped value. Returns a new SQLString.
This is the safe way to include user input in SQL queries.

```fides
pub fn sql_bind(query: SQLString, param: str) -> SQLString {
```

**Parameters:**

- `query`: `SQLString`
- `param`: `str`

**Returns:** `SQLString`

*Defined at line 93*

---

## sql_bind_int

Bind an integer parameter into a SQL query.

```fides
pub fn sql_bind_int(query: SQLString, param: i64) -> SQLString {
```

**Parameters:**

- `query`: `SQLString`
- `param`: `i64`

**Returns:** `SQLString`

*Defined at line 100*

---

## ShellString

A shell command string. Must be constructed via `shell_cmd()` or `shell_escape()`.
Prevents shell injection — untrusted input can't be concatenated into commands.

```fides
pub struct ShellString {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |

*Defined at line 109*

---

## ShellString

```fides
impl ShellString {
```

*Defined at line 113*

---

## shell_cmd

Create a ShellString from a literal command.

```fides
pub fn shell_cmd(cmd: str) -> ShellString {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `ShellString`

*Defined at line 142*

---

## shell_escape

Escape a string for safe inclusion in a shell command.
Wraps in single quotes and escapes any single quotes inside.

```fides
pub fn shell_escape(input: str) -> ShellString {
```

**Parameters:**

- `input`: `str`

**Returns:** `ShellString`

*Defined at line 148*

---

## HTMLString

An HTML-safe string. Must be constructed via `html_escape()` or `html_literal()`.
Prevents XSS — untrusted input can't be inserted into HTML without escaping.

```fides
pub struct HTMLString {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |

*Defined at line 159*

---

## HTMLString

```fides
impl HTMLString {
```

*Defined at line 163*

---

## html_literal

Create an HTMLString from literal HTML content.

```fides
pub fn html_literal(html: str) -> HTMLString {
```

**Parameters:**

- `html`: `str`

**Returns:** `HTMLString`

*Defined at line 176*

---

## html_escape

Escape a string for safe inclusion in HTML.
Replaces &, <, >, ", ' with their HTML entity equivalents.

```fides
pub fn html_escape(input: str) -> HTMLString {
```

**Parameters:**

- `input`: `str`

**Returns:** `HTMLString`

*Defined at line 182*

---

## html_attr_escape

Escape a string for safe inclusion in an HTML attribute.

```fides
pub fn html_attr_escape(input: str) -> HTMLString {
```

**Parameters:**

- `input`: `str`

**Returns:** `HTMLString`

*Defined at line 193*

---

## JSONString

A JSON-safe string. Must be constructed via `json_escape()`.
Prevents JSON injection — untrusted input can't break JSON structure.

```fides
pub struct JSONString {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |

*Defined at line 205*

---

## JSONString

```fides
impl JSONString {
```

*Defined at line 209*

---

## json_escape

Escape a string for safe inclusion in JSON.
Replaces ", \, and control characters with their JSON escape sequences.

```fides
pub fn json_escape(input: str) -> JSONString {
```

**Parameters:**

- `input`: `str`

**Returns:** `JSONString`

*Defined at line 218*

---

## PathString

A filesystem path string. Must be constructed via `path_safe()`.
Prevents path traversal — rejects ".." and absolute paths from user input.

```fides
pub struct PathString {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |

*Defined at line 231*

---

## PathString

```fides
impl PathString {
```

*Defined at line 235*

---

## path_safe

Create a PathString from user input, rejecting path traversal attempts.
Returns a PathString with empty value if the path is unsafe.

```fides
pub fn path_safe(input: str) -> PathString {
```

**Parameters:**

- `input`: `str`

**Returns:** `PathString`

*Defined at line 251*

---

## path_literal

Create a PathString from a literal path (trusted).

```fides
pub fn path_literal(path: str) -> PathString {
```

**Parameters:**

- `path`: `str`

**Returns:** `PathString`

*Defined at line 259*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
