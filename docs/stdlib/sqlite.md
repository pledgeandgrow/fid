# sqlite — Fides Documentation

## Contents

- [SQLite](#sqlite) (struct)
- [SQLite](#sqlite) (impl)
- [SQLite](#sqlite) (impl)
- [last_insert_rowid](#last_insert_rowid) (function)
- [changes](#changes) (function)
- [error_message](#error_message) (function)
- [What it does and does not do](#what-it-does-and-does-not-do)

## SQLite

A connection to a SQLite database file.
The `handle` field holds the raw `sqlite3*` pointer as an i64.

```fides
struct SQLite {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |

The struct and its field are not `pub`, but the methods below are, so a program
uses the type through `SQLite::open(...)` and the method calls. The handle is
not meant to be touched.

*Defined at line 111*

---

## SQLite

Opening a connection.

```fides
impl SQLite {
```

**Methods:**

- `SQLite::open(path: str) -> SQLite`: opens `path` and creates the file if it
  does not exist. Panics with the SQLite error message when the open fails.
- `SQLite::open_memory() -> SQLite`: the same with `":memory:"`, a private
  database that disappears when it is closed.

*Defined at line 115*

---

## SQLite

Running SQL. The three methods mirror the `query` / `exec` / `close` shape of
the `Database` capability in `db.fd`, but the file does not declare
`impl Database for SQLite`, so the type is not accepted where a `Database`
bound is required.

```fides
impl SQLite {
```

**Methods:**

- `query(self: SQLite, sql: str) -> str`: runs one SELECT and returns every row as
  a JSON array of objects keyed by column name, for example
  `[{"id":1,"name":"Alice"},{"id":2,"name":"Bob"}]`; `[]` when there are no
  rows. Integers are written as numbers, floats with `float_to_str`, text as an
  escaped JSON string, and NULL as `null`. A BLOB is also written as `null`.
  Panics with the SQLite message when the statement does not compile.
- `exec(self: SQLite, sql: str) -> i64`: runs SQL that returns no rows (INSERT,
  UPDATE, DELETE, CREATE, DROP, BEGIN, COMMIT, ...). Returns
  `sqlite3_changes` of the connection. Panics with the SQLite message when the
  statement fails.
- `close(self: SQLite)`: closes the connection. There is no check that it was
  not closed already.

*Defined at line 148*

---

## last_insert_rowid

Get the rowid of the most recent successful INSERT.

```fides
pub fn last_insert_rowid(db: SQLite) -> i64 {
```

**Parameters:**

- `db`: `SQLite`

**Returns:** `i64`

*Defined at line 242*

---

## changes

Get the number of rows modified by the most recent INSERT/UPDATE/DELETE.

```fides
pub fn changes(db: SQLite) -> i64 {
```

**Parameters:**

- `db`: `SQLite`

**Returns:** `i64`

*Defined at line 247*

---

## error_message

Get the last error message from the database connection.

```fides
pub fn error_message(db: SQLite) -> str {
```

**Parameters:**

- `db`: `SQLite`

**Returns:** `str`

*Defined at line 252*

---

## What it does and does not do

A thin binding to the system `libsqlite3` through `extern "C" link "sqlite3"`.
It declares the SQLite functions it needs with `i64` pointers instead of
including `<sqlite3.h>`.

**Does**

- Opens file and in-memory databases, runs SQL, reads results back as JSON text.
- Runs every statement through the library, so transactions
  (`exec("BEGIN")`, `exec("COMMIT")`), pragmas, indexes, triggers and so on
  work as SQL.
- `exec` accepts several statements separated by `;` in one call
  (`sqlite3_exec`).

**Does not**

- Bind parameters in `query` or `exec`: a statement is a plain string, and building
  it by concatenation is open to SQL injection. Use `query_params`, which binds
  each value as text. It has not been run here: SQLite is not installed.
- Return an error value. Every failure of `open`, `query` and `exec` is a
  `p_panic` with the SQLite message. `error_message` only reports the last
  message of the connection.
- Run more than the first statement of a `query` string, and it does not
  report an error that happens while stepping through the rows: the result
  simply ends at that point.
- Read BLOB values (they come back as `null`) or write them.
- Give `exec` a meaningful count for DDL: it returns `sqlite3_changes`, which
  keeps the figure of the last INSERT/UPDATE/DELETE.
- Free the error text that `sqlite3_exec` allocates.
- Cope with text values that contain a NUL byte: text comes back as a C string, so it ends at the first NUL.
- Pool, cache or share connections; there is no prepared statement API.

**Where it runs**

The library has to be installed (`libsqlite3-dev` on Debian/Ubuntu, `sqlite3`
from Homebrew on macOS). The interpreter cannot call it: `fid run` stops with
"interpreter does not support SQLite". `fid ccompile` links `-lsqlite3` and
fails with "unable to find dynamic system library 'sqlite3'" when it is not
installed. `fid emit` produces an executable that imports `sqlite3.dll`; it
does not start when that DLL is missing. For this reason `fidesc test` skips
`test_sqlite` unless `FIDES_TEST_EXTERNAL=1` is set (see `tests/bootstrap/test_sqlite.fd`).

```fides
use sqlite;

fn main() -> i64 {
    var db = SQLite::open_memory();
    db.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
    db.exec("INSERT INTO users (name) VALUES ('Alice')");
    println(db.query("SELECT * FROM users"));   // [{"id":1,"name":"Alice"}]
    println(last_insert_rowid(db));             // 1
    db.close();
    return 0;
}
```

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
