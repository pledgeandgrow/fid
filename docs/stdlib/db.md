[All modules](index.md)

# db -- Fides Documentation

## Contents

- [MemoryDB](#MemoryDB) (impl)
- [new](#new) (function)
- [Database](#Database) (impl)
- [query_one](#query_one) (function)
- [query_scalar](#query_scalar) (function)
- [insert](#insert) (function)
- [delete](#delete) (function)
- [update](#update) (function)
- [query_builder](#query_builder) (function)
- [db_quote](#db_quote) (function)
- [qb_param](#qb_param) (function)
- [qb_param_i64](#qb_param_i64) (function)
- [build_insert](#build_insert) (function)
- [build_select](#build_select) (function)
- [build_update](#build_update) (function)
- [build_delete](#build_delete) (function)
- [db_ident_safe](#db_ident_safe) (function)
- [get_schema](#get_schema) (function)
- [table_exists](#table_exists) (function)
- [create_table](#create_table) (function)

## MemoryDB

```fides
impl MemoryDB {
```

**Methods:**

### new

```fides
pub fn new() -> MemoryDB {
```

*Defined at line 34*

---

## new

Convenience function — alias for MemoryDB::new()

```fides
pub fn new() -> MemoryDB {
```

**Returns:** `MemoryDB`

*Defined at line 45*

---

## Database

```fides
impl Database for MemoryDB {
```

**Methods:**

### query

```fides
pub fn query (self: MemoryDB, sql: str) -> str {
```

### exec

```fides
pub fn exec(self: MemoryDB, sql: str) -> i64 {
```

### close

```fides
pub fn close(self: MemoryDB) {
```

*Defined at line 49*

---

## query_one

Helper: query a single row as a JSON object. Returns "" if no rows.

```fides
pub fn query_one<T: Database>(db: T, sql: str) -> str {
```

**Parameters:**

- `db`: `T`
- `sql`: `str`

**Returns:** `str`

*Defined at line 64*

---

## query_scalar

Helper: query a scalar value (first column of first row).

```fides
pub fn query_scalar<T: Database>(db: T, sql: str) -> str {
```

**Parameters:**

- `db`: `T`
- `sql`: `str`

**Returns:** `str`

*Defined at line 74*

---

## insert

Helper: execute an INSERT statement and return the number of rows affected.

```fides
pub fn insert<T: Database>(db: T, table: str, values: str) -> i64 {
```

**Parameters:**

- `db`: `T`
- `table`: `str`
- `values`: `str`

**Returns:** `i64`

*Defined at line 86*

---

## delete

Helper: execute a DELETE statement and return rows affected.

```fides
pub fn delete<T: Database>(db: T, table: str, condition: str) -> i64 {
```

**Parameters:**

- `db`: `T`
- `table`: `str`
- `condition`: `str`

**Returns:** `i64`

*Defined at line 93*

---

## update

Helper: execute an UPDATE statement and return rows affected.

```fides
pub fn update<T: Database>(db: T, table: str, set_clause: str, condition: str) -> i64 {
```

**Parameters:**

- `db`: `T`
- `table`: `str`
- `set_clause`: `str`
- `condition`: `str`

**Returns:** `i64`

*Defined at line 100*

---

## query_builder

Typed query builder — builds SQL queries with type-safe parameters.
Parameters are passed as JSON to avoid SQL injection.


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
use db;
var conn = db::new();
var qb = query_builder("SELECT * FROM users WHERE id = ?");
qb = qb_param(qb, "1");
var rows = conn.query(qb);
```

```fides
pub fn query_builder(sql: str) -> str {
```

**Parameters:**

- `sql`: `str`

**Returns:** `str`

*Defined at line 126*

---

## db_quote

Escape `s` as a single-quoted SQL string literal (' → '').

```fides
pub fn db_quote(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 131*

---

## qb_param

Add a parameter to a query builder. Replaces the FIRST `?` with the
escaped parameter — quote-doubled, and wrapped in quotes itself when
the `?` is not already inside a string literal, so the value can never
break out into query syntax. (Prepared statements are still strictly
safer: this is correct escaping, not a wire-level bind.)

```fides
pub fn qb_param(query: str, param: str) -> str {
```

**Parameters:**

- `query`: `str`
- `param`: `str`

**Returns:** `str`

*Defined at line 158*

---

## qb_param_i64

Add an integer parameter to a query builder.

```fides
pub fn qb_param_i64(query: str, param: i64) -> str {
```

**Parameters:**

- `query`: `str`
- `param`: `i64`

**Returns:** `str`

*Defined at line 173*

---

## build_insert

Build an INSERT query from a table name and column/value pairs.
columns: JSON array of column names (e.g. ["name", "age"])
values: JSON array of values (e.g. ["Alice", 30])
Returns: INSERT INTO table (col1, col2) VALUES (val1, val2)

```fides
pub fn build_insert(table: str, columns: str, values: str) -> str {
```

**Parameters:**

- `table`: `str`
- `columns`: `str`
- `values`: `str`

**Returns:** `str`

*Defined at line 186*

---

## build_select

Build a SELECT query from a table name and optional columns.
columns: JSON array of column names, or "" for *
where_clause: optional WHERE clause (e.g. "id = 1"), or ""
Returns: SELECT col1, col2 FROM table [WHERE clause]

```fides
pub fn build_select(table: str, columns: str, where_clause: str) -> str {
```

**Parameters:**

- `table`: `str`
- `columns`: `str`
- `where_clause`: `str`

**Returns:** `str`

*Defined at line 214*

---

## build_update

Build an UPDATE query from a table name, SET clause, and WHERE clause.

```fides
pub fn build_update(table: str, set_clause: str, where_clause: str) -> str {
```

**Parameters:**

- `table`: `str`
- `set_clause`: `str`
- `where_clause`: `str`

**Returns:** `str`

*Defined at line 237*

---

## build_delete

Build a DELETE query from a table name and WHERE clause.

```fides
pub fn build_delete(table: str, where_clause: str) -> str {
```

**Parameters:**

- `table`: `str`
- `where_clause`: `str`

**Returns:** `str`

*Defined at line 246*

---

## db_ident_safe

Is `name` usable as a bare SQL identifier (letters, digits, '_'; not
starting with a digit)? PRAGMA and identifier positions cannot take
quoted parameters, so they must be validated instead of escaped.

```fides
pub fn db_ident_safe(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 264*

---

## get_schema

Get the schema of a table as JSON.
Returns: [{"name": "id", "type": "INTEGER"}, {"name": "name", "type": "TEXT"}, ...]
The table name must be a safe bare identifier (db_ident_safe) —
returns "[]" otherwise.

```fides
pub fn get_schema<T: Database>(db: T, table: str) -> str {
```

**Parameters:**

- `db`: `T`
- `table`: `str`

**Returns:** `str`

*Defined at line 294*

---

## table_exists

Check if a table exists.

```fides
pub fn table_exists<T: Database>(db: T, table: str) -> bool {
```

**Parameters:**

- `db`: `T`
- `table`: `str`

**Returns:** `bool`

*Defined at line 302*

---

## create_table

Create a table from a schema definition.
schema: JSON array of column definitions
[{"name": "id", "type": "INTEGER PRIMARY KEY"}, {"name": "name", "type": "TEXT"}]

```fides
pub fn create_table<T: Database>(db: T, table: str, schema: str) -> i64 {
```

**Parameters:**

- `db`: `T`
- `table`: `str`
- `schema`: `str`

**Returns:** `i64`

*Defined at line 310*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
