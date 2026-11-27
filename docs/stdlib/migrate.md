[All modules](index.md)

# migrate -- Fides Documentation

## Contents

- [Migration](#Migration) (struct)
- [Applied](#Applied) (struct)
- [MigStep](#MigStep) (struct)
- [MigPlan](#MigPlan) (struct)
- [MigReport](#MigReport) (struct)
- [MigDb](#MigDb) (struct)
- [migration_new](#migration_new) (function)
- [migrate_checksum](#migrate_checksum) (function)
- [migrate_parse_file](#migrate_parse_file) (function)
- [migrate_table_sql](#migrate_table_sql) (function)
- [migrate_record_sql](#migrate_record_sql) (function)
- [migrate_unrecord_sql](#migrate_unrecord_sql) (function)
- [migrate_parse_applied](#migrate_parse_applied) (function)
- [migrate_plan](#migrate_plan) (function)
- [migrate_plan_lines](#migrate_plan_lines) (function)
- [migrate_run](#migrate_run) (function)
- [migrate_status](#migrate_status) (function)

## Migration

```fides
pub struct Migration {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `name` | `str` |
| `up` | `str` |
| `down` | `str` |

*Defined at line 36*

---

## Applied

```fides
pub struct Applied {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `name` | `str` |
| `checksum` | `str` |
| `applied_at` | `i64` |

*Defined at line 43*

---

## MigStep

One step of a plan: `up` true applies, false reverts.

```fides
pub struct MigStep {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `name` | `str` |
| `up` | `bool` |
| `sql` | `str` |
| `checksum` | `str` |

*Defined at line 51*

---

## MigPlan

```fides
pub struct MigPlan {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `steps` | `Vec<MigStep>` |
| `current` | `i64,  // the newest applied version (0 when none)` |
| `target` | `i64` |

*Defined at line 59*

---

## MigReport

```fides
pub struct MigReport {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `done` | `Vec<str>,  // "up 3 add users" / "down 3 add users"` |
| `version` | `i64,  // the version after the run` |

*Defined at line 67*

---

## MigDb

The database seam (see the header).

```fides
pub struct MigDb {
```

**Fields:**

| Name | Type |
|------|------|
| `exec` | `fn(str) -> str` |
| `applied` | `fn() -> str` |

*Defined at line 75*

---

## migration_new

```fides
pub fn migration_new(version: i64, name: str, up: str, down: str) -> Migration {
```

**Parameters:**

- `version`: `i64`
- `name`: `str`
- `up`: `str`
- `down`: `str`

**Returns:** `Migration`

*Defined at line 80*

---

## migrate_checksum

Checksum of a script: the first 16 bytes of SHA-512 as 32 hex digits.
Line endings are normalized (CRLF -> LF) and trailing whitespace of the
whole script ignored, so a re-saved file keeps its checksum.

```fides
pub fn migrate_checksum(script: str) -> str {
```

**Parameters:**

- `script`: `str`

**Returns:** `str`

*Defined at line 89*

---

## migrate_parse_file

Read a migration file "0003_add_users.sql": the version from the digits
before '_', the name from the rest (underscores become spaces), the up
and down scripts from the "-- +up" and "-- +down" sections. version -1
when the name or sections are malformed.

```fides
pub fn migrate_parse_file(filename: str, text: str) -> Migration {
```

**Parameters:**

- `filename`: `str`
- `text`: `str`

**Returns:** `Migration`

*Defined at line 98*

---

## migrate_table_sql

CREATE TABLE for the bookkeeping table (PostgreSQL and SQLite alike).

```fides
pub fn migrate_table_sql() -> str {
```

**Returns:** `str`

*Defined at line 145*

---

## migrate_record_sql

The INSERT that records an applied migration.

```fides
pub fn migrate_record_sql(version: i64, name: str, checksum: str, now: i64) -> str {
```

**Parameters:**

- `version`: `i64`
- `name`: `str`
- `checksum`: `str`
- `now`: `i64`

**Returns:** `str`

*Defined at line 154*

---

## migrate_unrecord_sql

The DELETE that forgets a reverted migration.

```fides
pub fn migrate_unrecord_sql(version: i64) -> str {
```

**Parameters:**

- `version`: `i64`

**Returns:** `str`

*Defined at line 159*

---

## migrate_parse_applied

Rows of the table from "version|name|checksum|applied_at" lines.

```fides
pub fn migrate_parse_applied(rows: str) -> Vec<Applied> {
```

**Parameters:**

- `rows`: `str`

**Returns:** `Vec<Applied>`

*Defined at line 164*

---

## migrate_plan

Plan the way from the applied state to `target` (-1: the latest).

```fides
pub fn migrate_plan(ms: &Vec<Migration>, applied: Vec<Applied>, target: i64, allow_out_of_order: bool) -> MigPlan {
```

**Parameters:**

- `ms`: `&Vec<Migration>`
- `applied`: `Vec<Applied>`
- `target`: `i64`
- `allow_out_of_order`: `bool`

**Returns:** `MigPlan`

*Defined at line 221*

---

## migrate_plan_lines

One line per step: "up 2 add email" / "down 3 drop users".

```fides
pub fn migrate_plan_lines(p: &MigPlan) -> Vec<str> {
```

**Parameters:**

- `p`: `&MigPlan`

**Returns:** `Vec<str>`

*Defined at line 313*

---

## migrate_run

Plan against the database's table and run the steps: each step in a
transaction with its bookkeeping row; the first failure rolls back
that step and stops (earlier steps stay applied).

```fides
pub fn migrate_run(db: &MigDb, ms: &Vec<Migration>, target: i64, now: i64, allow_out_of_order: bool) -> MigReport {
```

**Parameters:**

- `db`: `&MigDb`
- `ms`: `&Vec<Migration>`
- `target`: `i64`
- `now`: `i64`
- `allow_out_of_order`: `bool`

**Returns:** `MigReport`

*Defined at line 333*

---

## migrate_status

"applied" / "pending" / "changed" for each known migration, in order.

```fides
pub fn migrate_status(ms: &Vec<Migration>, applied: Vec<Applied>) -> Vec<str> {
```

**Parameters:**

- `ms`: `&Vec<Migration>`
- `applied`: `Vec<Applied>`

**Returns:** `Vec<str>`

*Defined at line 404*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
