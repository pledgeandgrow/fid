[All modules](index.md)

# sql_builder -- Fides Documentation

## Contents

- [SQL_POSTGRES:](#SQL_POSTGRES:) (const)
- [SQL_SQLITE:](#SQL_SQLITE:) (const)
- [SqlVal](#SqlVal) (struct)
- [sv_null](#sv_null) (function)
- [sv_int](#sv_int) (function)
- [sv_real](#sv_real) (function)
- [sv_text](#sv_text) (function)
- [sv_bool](#sv_bool) (function)
- [sv_show](#sv_show) (function)
- [SqlCond](#SqlCond) (enum)
- [sc_cmp](#sc_cmp) (function)
- [sc_eq](#sc_eq) (function)
- [sc_ne](#sc_ne) (function)
- [sc_lt](#sc_lt) (function)
- [sc_le](#sc_le) (function)
- [sc_gt](#sc_gt) (function)
- [sc_ge](#sc_ge) (function)
- [sc_like](#sc_like) (function)
- [sc_is_null](#sc_is_null) (function)
- [sc_not_null](#sc_not_null) (function)
- [sc_in](#sc_in) (function)
- [sc_not_in](#sc_not_in) (function)
- [sc_between](#sc_between) (function)
- [sc_and](#sc_and) (function)
- [sc_or](#sc_or) (function)
- [sc_not](#sc_not) (function)
- [sc_raw](#sc_raw) (function)
- [sc_and2](#sc_and2) (function)
- [sc_or2](#sc_or2) (function)
- [SqlJoin](#SqlJoin) (struct)
- [SqlQuery](#SqlQuery) (struct)
- [SqlOut](#SqlOut) (struct)
- [sql_select](#sql_select) (function)
- [sql_insert](#sql_insert) (function)
- [sql_update](#sql_update) (function)
- [sql_delete](#sql_delete) (function)
- [sq_columns](#sq_columns) (function)
- [sq_alias](#sq_alias) (function)
- [sq_distinct](#sq_distinct) (function)
- [sq_join](#sq_join) (function)
- [sq_where](#sq_where) (function)
- [sq_group_by](#sq_group_by) (function)
- [sq_having](#sq_having) (function)
- [sq_order](#sq_order) (function)
- [sq_limit](#sq_limit) (function)
- [sq_offset](#sq_offset) (function)
- [sq_insert_columns](#sq_insert_columns) (function)
- [sq_row](#sq_row) (function)
- [sq_set](#sq_set) (function)
- [sq_returning](#sq_returning) (function)
- [sq_on_conflict_do_nothing](#sq_on_conflict_do_nothing) (function)
- [sq_allow_all_rows](#sq_allow_all_rows) (function)
- [sql_quote_ident](#sql_quote_ident) (function)
- [sql_quote_col](#sql_quote_col) (function)
- [sql_build](#sql_build) (function)
- [sql_params_show](#sql_params_show) (function)

## SQL_POSTGRES:

```fides
pub const SQL_POSTGRES: i64 = 0;
```

*Defined at line 36*

---

## SQL_SQLITE:

```fides
pub const SQL_SQLITE: i64 = 1;
```

*Defined at line 37*

---

## SqlVal

A bound value. kind: 0 NULL, 1 integer, 2 real, 3 text, 4 boolean.

```fides
pub struct SqlVal {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `i` | `i64` |
| `f` | `f64` |
| `s` | `str` |
| `b` | `bool` |

*Defined at line 40*

---

## sv_null

```fides
pub fn sv_null() -> SqlVal {
```

**Returns:** `SqlVal`

*Defined at line 48*

---

## sv_int

```fides
pub fn sv_int(v: i64) -> SqlVal {
```

**Parameters:**

- `v`: `i64`

**Returns:** `SqlVal`

*Defined at line 53*

---

## sv_real

```fides
pub fn sv_real(v: f64) -> SqlVal {
```

**Parameters:**

- `v`: `f64`

**Returns:** `SqlVal`

*Defined at line 58*

---

## sv_text

```fides
pub fn sv_text(v: str) -> SqlVal {
```

**Parameters:**

- `v`: `str`

**Returns:** `SqlVal`

*Defined at line 63*

---

## sv_bool

```fides
pub fn sv_bool(v: bool) -> SqlVal {
```

**Parameters:**

- `v`: `bool`

**Returns:** `SqlVal`

*Defined at line 68*

---

## sv_show

A value as readable text (for logs and tests; never put into SQL).

```fides
pub fn sv_show(v: &SqlVal) -> str {
```

**Parameters:**

- `v`: `&SqlVal`

**Returns:** `str`

*Defined at line 75*

---

## SqlCond

A WHERE / HAVING condition tree.

```fides
pub enum SqlCond {
```

**Variants:**

- `Cmp`
- `Null`
- `In`
- `Between`
- `And`
- `Or`
- `Not`
- `Raw`

*Defined at line 95*

---

## sc_cmp

```fides
pub fn sc_cmp(col: str, op: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `op`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 106*

---

## sc_eq

```fides
pub fn sc_eq(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 109*

---

## sc_ne

```fides
pub fn sc_ne(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 112*

---

## sc_lt

```fides
pub fn sc_lt(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 115*

---

## sc_le

```fides
pub fn sc_le(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 118*

---

## sc_gt

```fides
pub fn sc_gt(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 121*

---

## sc_ge

```fides
pub fn sc_ge(col: str, v: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 124*

---

## sc_like

```fides
pub fn sc_like(col: str, pattern: str) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `pattern`: `str`

**Returns:** `SqlCond`

*Defined at line 127*

---

## sc_is_null

```fides
pub fn sc_is_null(col: str) -> SqlCond {
```

**Parameters:**

- `col`: `str`

**Returns:** `SqlCond`

*Defined at line 130*

---

## sc_not_null

```fides
pub fn sc_not_null(col: str) -> SqlCond {
```

**Parameters:**

- `col`: `str`

**Returns:** `SqlCond`

*Defined at line 133*

---

## sc_in

```fides
pub fn sc_in(col: str, vals: Vec<SqlVal>) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `vals`: `Vec<SqlVal>`

**Returns:** `SqlCond`

*Defined at line 136*

---

## sc_not_in

```fides
pub fn sc_not_in(col: str, vals: Vec<SqlVal>) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `vals`: `Vec<SqlVal>`

**Returns:** `SqlCond`

*Defined at line 139*

---

## sc_between

```fides
pub fn sc_between(col: str, lo: SqlVal, hi: SqlVal) -> SqlCond {
```

**Parameters:**

- `col`: `str`
- `lo`: `SqlVal`
- `hi`: `SqlVal`

**Returns:** `SqlCond`

*Defined at line 142*

---

## sc_and

```fides
pub fn sc_and(cs: Vec<SqlCond>) -> SqlCond {
```

**Parameters:**

- `cs`: `Vec<SqlCond>`

**Returns:** `SqlCond`

*Defined at line 145*

---

## sc_or

```fides
pub fn sc_or(cs: Vec<SqlCond>) -> SqlCond {
```

**Parameters:**

- `cs`: `Vec<SqlCond>`

**Returns:** `SqlCond`

*Defined at line 148*

---

## sc_not

```fides
pub fn sc_not(c: SqlCond) -> SqlCond {
```

**Parameters:**

- `c`: `SqlCond`

**Returns:** `SqlCond`

*Defined at line 151*

---

## sc_raw

A raw SQL fragment; each `?` in it becomes the next parameter.

```fides
pub fn sc_raw(sql: str, vals: Vec<SqlVal>) -> SqlCond {
```

**Parameters:**

- `sql`: `str`
- `vals`: `Vec<SqlVal>`

**Returns:** `SqlCond`

*Defined at line 155*

---

## sc_and2

Two conditions joined with AND / OR (a convenience).

```fides
pub fn sc_and2(a: SqlCond, b: SqlCond) -> SqlCond {
```

**Parameters:**

- `a`: `SqlCond`
- `b`: `SqlCond`

**Returns:** `SqlCond`

*Defined at line 160*

---

## sc_or2

```fides
pub fn sc_or2(a: SqlCond, b: SqlCond) -> SqlCond {
```

**Parameters:**

- `a`: `SqlCond`
- `b`: `SqlCond`

**Returns:** `SqlCond`

*Defined at line 167*

---

## SqlJoin

```fides
pub struct SqlJoin {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str,  // INNER LEFT RIGHT FULL CROSS` |
| `table` | `str` |
| `alias` | `str` |
| `on_left` | `str` |
| `on_right` | `str` |

*Defined at line 174*

---

## SqlQuery

A statement under construction. kind: 0 select, 1 insert, 2 update,
3 delete.

```fides
pub struct SqlQuery {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `table` | `str` |
| `alias` | `str` |
| `distinct` | `bool` |
| `columns` | `Vec<str>` |
| `joins` | `Vec<SqlJoin>` |
| `wheres` | `Vec<SqlCond>` |
| `group_by` | `Vec<str>` |
| `havings` | `Vec<SqlCond>` |
| `order_cols` | `Vec<str>` |
| `order_desc` | `Vec<bool>` |
| `limit` | `i64` |
| `offset` | `i64` |
| `insert_cols` | `Vec<str>` |
| `rows` | `Vec<Vec<SqlVal>>` |
| `set_cols` | `Vec<str>` |
| `set_vals` | `Vec<SqlVal>` |
| `returning` | `Vec<str>` |
| `on_conflict_nothing` | `bool` |
| `allow_all_rows` | `bool` |

*Defined at line 184*

---

## SqlOut

The built statement: SQL text and the parameters in placeholder order.

```fides
pub struct SqlOut {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `sql` | `str` |
| `params` | `Vec<SqlVal>` |

*Defined at line 208*

---

## sql_select

```fides
pub fn sql_select(table: str) -> SqlQuery {
```

**Parameters:**

- `table`: `str`

**Returns:** `SqlQuery`

*Defined at line 221*

---

## sql_insert

```fides
pub fn sql_insert(table: str) -> SqlQuery {
```

**Parameters:**

- `table`: `str`

**Returns:** `SqlQuery`

*Defined at line 224*

---

## sql_update

```fides
pub fn sql_update(table: str) -> SqlQuery {
```

**Parameters:**

- `table`: `str`

**Returns:** `SqlQuery`

*Defined at line 227*

---

## sql_delete

```fides
pub fn sql_delete(table: str) -> SqlQuery {
```

**Parameters:**

- `table`: `str`

**Returns:** `SqlQuery`

*Defined at line 230*

---

## sq_columns

Columns, comma-separated ("id, name, t.x"). An entry holding '(' or a
space (count(*), x AS y) is written as given, not quoted.

```fides
pub fn sq_columns(q: SqlQuery, cols: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `cols`: `str`

**Returns:** `SqlQuery`

*Defined at line 250*

---

## sq_alias

```fides
pub fn sq_alias(q: SqlQuery, alias: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `alias`: `str`

**Returns:** `SqlQuery`

*Defined at line 261*

---

## sq_distinct

```fides
pub fn sq_distinct(q: SqlQuery) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`

**Returns:** `SqlQuery`

*Defined at line 267*

---

## sq_join

A join: kind INNER/LEFT/RIGHT/FULL/CROSS, ON left = right (columns).

```fides
pub fn sq_join(q: SqlQuery, kind: str, table: str, alias: str, on_left: str, on_right: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `kind`: `str`
- `table`: `str`
- `alias`: `str`
- `on_left`: `str`
- `on_right`: `str`

**Returns:** `SqlQuery`

*Defined at line 274*

---

## sq_where

Add a condition (several are ANDed).

```fides
pub fn sq_where(q: SqlQuery, c: SqlCond) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `c`: `SqlCond`

**Returns:** `SqlQuery`

*Defined at line 283*

---

## sq_group_by

```fides
pub fn sq_group_by(q: SqlQuery, cols: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `cols`: `str`

**Returns:** `SqlQuery`

*Defined at line 289*

---

## sq_having

```fides
pub fn sq_having(q: SqlQuery, c: SqlCond) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `c`: `SqlCond`

**Returns:** `SqlQuery`

*Defined at line 300*

---

## sq_order

```fides
pub fn sq_order(q: SqlQuery, col: str, desc: bool) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `col`: `str`
- `desc`: `bool`

**Returns:** `SqlQuery`

*Defined at line 306*

---

## sq_limit

```fides
pub fn sq_limit(q: SqlQuery, n: i64) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `n`: `i64`

**Returns:** `SqlQuery`

*Defined at line 313*

---

## sq_offset

```fides
pub fn sq_offset(q: SqlQuery, n: i64) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `n`: `i64`

**Returns:** `SqlQuery`

*Defined at line 319*

---

## sq_insert_columns

INSERT columns (comma-separated); then add rows with sq_row.

```fides
pub fn sq_insert_columns(q: SqlQuery, cols: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `cols`: `str`

**Returns:** `SqlQuery`

*Defined at line 326*

---

## sq_row

```fides
pub fn sq_row(q: SqlQuery, vals: Vec<SqlVal>) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `vals`: `Vec<SqlVal>`

**Returns:** `SqlQuery`

*Defined at line 332*

---

## sq_set

UPDATE ... SET col = value.

```fides
pub fn sq_set(q: SqlQuery, col: str, v: SqlVal) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `col`: `str`
- `v`: `SqlVal`

**Returns:** `SqlQuery`

*Defined at line 339*

---

## sq_returning

```fides
pub fn sq_returning(q: SqlQuery, cols: str) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`
- `cols`: `str`

**Returns:** `SqlQuery`

*Defined at line 346*

---

## sq_on_conflict_do_nothing

```fides
pub fn sq_on_conflict_do_nothing(q: SqlQuery) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`

**Returns:** `SqlQuery`

*Defined at line 352*

---

## sq_allow_all_rows

Let an UPDATE / DELETE run without a WHERE (it touches every row).

```fides
pub fn sq_allow_all_rows(q: SqlQuery) -> SqlQuery {
```

**Parameters:**

- `q`: `SqlQuery`

**Returns:** `SqlQuery`

*Defined at line 359*

---

## sql_quote_ident

Quote one identifier: "a""b" for a"b.

```fides
pub fn sql_quote_ident(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 368*

---

## sql_quote_col

Quote a column reference: t.col -> "t"."col"; * and t.* keep the star;
an expression (holding '(' or a space) is returned unchanged.

```fides
pub fn sql_quote_col(col: str) -> str {
```

**Parameters:**

- `col`: `str`

**Returns:** `str`

*Defined at line 374*

---

## sql_build

Build the statement for a dialect.

```fides
pub fn sql_build(q: &SqlQuery, dialect: i64) -> SqlOut {
```

**Parameters:**

- `q`: `&SqlQuery`
- `dialect`: `i64`

**Returns:** `SqlOut`

*Defined at line 563*

---

## sql_params_show

The parameters as readable text: [1, 'x', NULL].

```fides
pub fn sql_params_show(out: &SqlOut) -> str {
```

**Parameters:**

- `out`: `&SqlOut`

**Returns:** `str`

*Defined at line 718*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
