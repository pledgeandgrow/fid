[All modules](index.md)

# typed_strings -- Fides Documentation

## Contents

- [SQLString](#SQLString) (struct)
- [SQLString](#SQLString) (impl)
- [sql_query](#sql_query) (function)
- [sql_literal](#sql_literal) (function)
- [sql_escape](#sql_escape) (function)
- [sql_bind](#sql_bind) (function)
- [sql_bind_int](#sql_bind_int) (function)
- [ShellString](#ShellString) (struct)
- [ShellString](#ShellString) (impl)
- [shell_cmd](#shell_cmd) (function)
- [shell_escape](#shell_escape) (function)
- [HTMLString](#HTMLString) (struct)
- [HTMLString](#HTMLString) (impl)
- [html_literal](#html_literal) (function)
- [html_escape](#html_escape) (function)
- [html_attr_escape](#html_attr_escape) (function)
- [JSONString](#JSONString) (struct)
- [JSONString](#JSONString) (impl)
- [json_escape](#json_escape) (function)
- [PathString](#PathString) (struct)
- [PathString](#PathString) (impl)
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

*Defined at line 37*

---

## SQLString

```fides
impl SQLString {
```

**Methods:**

### literal

Create a SQLString from a literal query (no user input).
Use this for static queries: `SQLString::literal("SELECT * FROM users")`

```fides
pub fn literal(sql: str) -> SQLString {
```

### raw

Get the raw string value. Use with caution — only for passing to
database drivers that handle parameterization.

```fides
pub fn raw(self: SQLString) -> str {
```

### is_safe

Check if the SQL string contains potentially dangerous characters.
This is a heuristic — not a complete security check.

```fides
pub fn is_safe(self: SQLString) -> bool {
```

*Defined at line 41*

---

## sql_query

Create a SQLString from a literal query string.
This is the safe constructor — only use with static query text.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val q = sql_bind(sql_query("SELECT * FROM t WHERE name = '?'"), "O'Brien");
assert_eq(q.raw(), "SELECT * FROM t WHERE name = 'O''Brien'");
assert_eq(sql_bind_int(sql_query("LIMIT ?"), 10).raw(), "LIMIT 10");
assert_eq(sql_escape("it's").raw(), "it''s");
```

```fides
pub fn sql_query(sql: str) -> SQLString {
```

**Parameters:**

- `sql`: `str`

**Returns:** `SQLString`

*Defined at line 94*

---

## sql_literal

Create a SQLString from a literal query (alias for sql_query).

```fides
pub fn sql_literal(sql: str) -> SQLString {
```

**Parameters:**

- `sql`: `str`

**Returns:** `SQLString`

*Defined at line 101*

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

*Defined at line 110*

---

## sql_bind

Bind a string parameter into a SQL query. Replaces the FIRST `?`
placeholder (further `?`s stay for later binds — str_replace would
have bound them all to this one value). The value is single-quote
escaped; a `?` sitting inside a quoted literal (the recommended
`WHERE x = '?'` form) gets the escaped text, a bare `?` gets the
escaped text wrapped in quotes — either way the parameter can never
break out into query syntax.

```fides
pub fn sql_bind(query: SQLString, param: str) -> SQLString {
```

**Parameters:**

- `query`: `SQLString`
- `param`: `str`

**Returns:** `SQLString`

*Defined at line 143*

---

## sql_bind_int

Bind an integer parameter into a SQL query. Replaces the first `?`
placeholder; integers need no quoting.

```fides
pub fn sql_bind_int(query: SQLString, param: i64) -> SQLString {
```

**Parameters:**

- `query`: `SQLString`
- `param`: `i64`

**Returns:** `SQLString`

*Defined at line 163*

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

*Defined at line 180*

---

## ShellString

```fides
impl ShellString {
```

**Methods:**

### literal

Create a ShellString from a literal command (no user input).

```fides
pub fn literal(cmd: str) -> ShellString {
```

### raw

Get the raw string value.

```fides
pub fn raw(self: ShellString) -> str {
```

### is_safe

Check if the shell string contains potentially dangerous characters.

```fides
pub fn is_safe(self: ShellString) -> bool {
```

*Defined at line 184*

---

## shell_cmd

Create a ShellString from a literal command.

```fides
pub fn shell_cmd(cmd: str) -> ShellString {
```

**Parameters:**

- `cmd`: `str`

**Returns:** `ShellString`

*Defined at line 235*

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

*Defined at line 243*

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

*Defined at line 256*

---

## HTMLString

```fides
impl HTMLString {
```

**Methods:**

### literal

Create an HTMLString from literal HTML (no user input).

```fides
pub fn literal(html: str) -> HTMLString {
```

### raw

Get the raw string value.

```fides
pub fn raw(self: HTMLString) -> str {
```

*Defined at line 260*

---

## html_literal

Create an HTMLString from literal HTML content.

```fides
pub fn html_literal(html: str) -> HTMLString {
```

**Parameters:**

- `html`: `str`

**Returns:** `HTMLString`

*Defined at line 275*

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

*Defined at line 283*

---

## html_attr_escape

Escape a string for safe inclusion in an HTML attribute.

```fides
pub fn html_attr_escape(input: str) -> HTMLString {
```

**Parameters:**

- `input`: `str`

**Returns:** `HTMLString`

*Defined at line 296*

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

*Defined at line 310*

---

## JSONString

```fides
impl JSONString {
```

**Methods:**

### raw

Get the raw string value.

```fides
pub fn raw(self: JSONString) -> str {
```

*Defined at line 314*

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

*Defined at line 323*

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

*Defined at line 338*

---

## PathString

```fides
impl PathString {
```

**Methods:**

### raw

Get the raw string value.

```fides
pub fn raw(self: PathString) -> str {
```

### is_safe

Check if the path is safe (no directory traversal).

```fides
pub fn is_safe(self: PathString) -> bool {
```

*Defined at line 342*

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

*Defined at line 360*

---

## path_literal

Create a PathString from a literal path (trusted).

```fides
pub fn path_literal(path: str) -> PathString {
```

**Parameters:**

- `path`: `str`

**Returns:** `PathString`

*Defined at line 372*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
