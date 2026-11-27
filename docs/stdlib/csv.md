[All modules](index.md)

# csv -- Fides Documentation

## Contents

- [CsvRow](#CsvRow) (struct)
- [CsvRecord](#CsvRecord) (struct)
- [csv_parse](#csv_parse) (function)
- [csv_parse_dialect](#csv_parse_dialect) (function)
- [csv_parse_records](#csv_parse_records) (function)
- [csv_record_get](#csv_record_get) (function)
- [csv_quote_field](#csv_quote_field) (function)
- [csv_quote_field_dialect](#csv_quote_field_dialect) (function)
- [csv_serialize](#csv_serialize) (function)
- [csv_serialize_dialect](#csv_serialize_dialect) (function)
- [csv_row](#csv_row) (function)
- [csv_field](#csv_field) (function)
- [csv_row_count](#csv_row_count) (function)
- [csv_col_count](#csv_col_count) (function)
- [csv_column](#csv_column) (function)
- [CSV_QUOTE_MINIMAL:](#CSV_QUOTE_MINIMAL:) (const)
- [CSV_QUOTE_ALL:](#CSV_QUOTE_ALL:) (const)
- [CSV_QUOTE_NONNUMERIC:](#CSV_QUOTE_NONNUMERIC:) (const)
- [CSV_QUOTE_NONE:](#CSV_QUOTE_NONE:) (const)
- [CsvOptions](#CsvOptions) (struct)
- [CsvResult](#CsvResult) (struct)
- [csv_options](#csv_options) (function)
- [csv_read](#csv_read) (function)
- [csv_write_field](#csv_write_field) (function)
- [csv_write](#csv_write) (function)

## CsvRow

One CSV record: a list of string fields.

```fides
pub struct CsvRow {
```

**Fields:**

| Name | Type |
|------|------|
| `fields` | `Vec<str>` |

*Defined at line 19*

---

## CsvRecord

A header→value record as produced by csv_parse_records:
`keys` and `values` are parallel; index-aligned.

```fides
pub struct CsvRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `values` | `Vec<str>` |

*Defined at line 25*

---

## csv_parse

Parse CSV text into a Vec of CsvRow. A trailing newline does not add
a phantom row; "a,b\n\n" keeps the empty second row.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows = csv_parse("name,qty\n\"Smith, J\",3\n");
assert_eq(csv_row_count(rows), 2);
assert_eq(csv_field(rows, 1, 0), "Smith, J");
assert_eq(csv_field(rows, 1, 1), "3");
```

```fides
pub fn csv_parse(text: str) -> Vec<CsvRow> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<CsvRow>`

*Defined at line 42*

---

## csv_parse_dialect

Full-dialect parse with a custom `delim` and `quote` character.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows = csv_parse_dialect("a;b\n1;2", ';', '"');
assert_eq(csv_field(rows, 1, 1), "2");
```

```fides
pub fn csv_parse_dialect(text: str, delim: i64, quote: i64) -> Vec<CsvRow> {
```

**Parameters:**

- `text`: `str`
- `delim`: `i64`
- `quote`: `i64`

**Returns:** `Vec<CsvRow>`

*Defined at line 51*

---

## csv_parse_records

Parse with the first line as headers; returns one CsvRecord per data row.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var recs = csv_parse_records("id,name\n1,Ada\n2,Bob\n");
assert_eq(recs.len(), 2);
assert_eq(csv_record_get(recs.get(1), "name"), "Bob");
```

```fides
pub fn csv_parse_records(text: str) -> Vec<CsvRecord> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<CsvRecord>`

*Defined at line 119*

---

## csv_record_get

Look up `key` in a CsvRecord; "" when absent.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var recs = csv_parse_records("id,name\n1,Ada\n");
assert_eq(csv_record_get(recs.get(0), "id"), "1");
assert_eq(csv_record_get(recs.get(0), "email"), "");
```

```fides
pub fn csv_record_get(rec: CsvRecord, key: str) -> str {
```

**Parameters:**

- `rec`: `CsvRecord`
- `key`: `str`

**Returns:** `str`

*Defined at line 153*

---

## csv_quote_field

Quote a field if it contains the delimiter, quotes, or newlines.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(csv_quote_field("plain"), "plain");
assert_eq(csv_quote_field("a,b"), "\"a,b\"");
assert_eq(csv_quote_field("say \"hi\""), "\"say \"\"hi\"\"\"");
```

```fides
pub fn csv_quote_field(field: str) -> str {
```

**Parameters:**

- `field`: `str`

**Returns:** `str`

*Defined at line 186*

---

## csv_quote_field_dialect

Quote a field for a dialect with the given delimiter and quote character.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(csv_quote_field_dialect("a;b", ';', '"'), "\"a;b\"");
assert_eq(csv_quote_field_dialect("a,b", ';', '"'), "a,b");
```

```fides
pub fn csv_quote_field_dialect(field: str, delim: i64, quote: i64) -> str {
```

**Parameters:**

- `field`: `str`
- `delim`: `i64`
- `quote`: `i64`

**Returns:** `str`

*Defined at line 195*

---

## csv_serialize

Serialize rows back to CSV text with LF line endings.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows: Vec<CsvRow> = Vec::new();
var f: Vec<str> = Vec::new();
f.push("x"); f.push("y,z");
rows.push(csv_row(f));
assert_eq(csv_serialize(rows), "x,\"y,z\"\n");
```

```fides
pub fn csv_serialize(rows: Vec<CsvRow>) -> str {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`

**Returns:** `str`

*Defined at line 233*

---

## csv_serialize_dialect

Serialize rows with a custom delimiter, LF line endings.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows = csv_parse("a,b\n1,2\n");
assert_eq(csv_serialize_dialect(rows, '\t'), "a\tb\n1\t2\n");
```

```fides
pub fn csv_serialize_dialect(rows: Vec<CsvRow>, delim: i64) -> str {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`
- `delim`: `i64`

**Returns:** `str`

*Defined at line 242*

---

## csv_row

Build a CsvRow from a Vec of str.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var f: Vec<str> = Vec::new();
f.push("1"); f.push("2");
var r = csv_row(f);
assert_eq(r.fields.len(), 2);
```

```fides
pub fn csv_row(fields: Vec<str>) -> CsvRow {
```

**Parameters:**

- `fields`: `Vec<str>`

**Returns:** `CsvRow`

*Defined at line 269*

---

## csv_field

Field at (row, col) or "" when out of range.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows = csv_parse("a,b\n");
assert_eq(csv_field(rows, 0, 1), "b");
assert_eq(csv_field(rows, 5, 0), "");
```

```fides
pub fn csv_field(rows: Vec<CsvRow>, r: i64, c: i64) -> str {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`
- `r`: `i64`
- `c`: `i64`

**Returns:** `str`

*Defined at line 283*

---

## csv_row_count

Number of parsed rows.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(csv_row_count(csv_parse("a\nb\nc\n")), 3);
```

```fides
pub fn csv_row_count(rows: Vec<CsvRow>) -> i64 {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`

**Returns:** `i64`

*Defined at line 298*

---

## csv_col_count

Column count of row `r` (0 if out of range).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var rows = csv_parse("a,b,c\nd\n");
assert_eq(csv_col_count(rows, 0), 3);
assert_eq(csv_col_count(rows, 1), 1);
```

```fides
pub fn csv_col_count(rows: Vec<CsvRow>, r: i64) -> i64 {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`
- `r`: `i64`

**Returns:** `i64`

*Defined at line 308*

---

## csv_column

Extract column `c` across all rows (missing cells become "").


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var col = csv_column(csv_parse("a,1\nb,2\n"), 1);
assert_eq(col.len(), 2);
assert_eq(col.get(1), "2");
```

```fides
pub fn csv_column(rows: Vec<CsvRow>, c: i64) -> Vec<str> {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`
- `c`: `i64`

**Returns:** `Vec<str>`

*Defined at line 321*

---

## CSV_QUOTE_MINIMAL:

Writer quoting: only fields that need it (delimiter, quote, line end,
leading space).

```fides
pub const CSV_QUOTE_MINIMAL: i64 = 0;
```

*Defined at line 341*

---

## CSV_QUOTE_ALL:

Writer quoting: every field.

```fides
pub const CSV_QUOTE_ALL: i64 = 1;
```

*Defined at line 343*

---

## CSV_QUOTE_NONNUMERIC:

Writer quoting: every field that is not a number.

```fides
pub const CSV_QUOTE_NONNUMERIC: i64 = 2;
```

*Defined at line 345*

---

## CSV_QUOTE_NONE:

Writer quoting: never; the delimiter, quote and line ends are escaped
with `escape` instead (Python's QUOTE_NONE with an escapechar).

```fides
pub const CSV_QUOTE_NONE: i64 = 3;
```

*Defined at line 348*

---

## CsvOptions

Options of `csv_read` and `csv_write`; `csv_options()` gives RFC 4180.

```fides
pub struct CsvOptions {
```

**Fields:**

| Name | Type |
|------|------|
| `delim` | `i64` |
| `quote` | `i64` |
| `comment` | `i64` |
| `trim_space` | `bool` |
| `lazy_quotes` | `bool` |
| `fields` | `i64` |
| `skip_empty` | `bool` |
| `quote_style` | `i64` |
| `escape` | `i64` |
| `crlf` | `bool` |

*Defined at line 351*

---

## CsvResult

What `csv_read` gives: the records read, and on an error its message,
line and column (1-based; the column counts bytes). Records before the
error are kept in `rows`.

```fides
pub struct CsvResult {
```

**Fields:**

| Name | Type |
|------|------|
| `rows` | `Vec<CsvRow>` |
| `ok` | `bool` |
| `err` | `str` |
| `line` | `i64` |
| `col` | `i64` |

*Defined at line 381*

---

## csv_options

RFC 4180 options: ',' and '"', no comments, strict quotes, any number
of fields, empty lines skipped, minimal quoting, LF line ends.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var o = csv_options();
assert_eq(o.delim, ',');
assert(!o.lazy_quotes);
```

```fides
pub fn csv_options() -> CsvOptions {
```

**Returns:** `CsvOptions`

*Defined at line 396*

---

## csv_read

Read CSV text with `opts`, checking it as Go's encoding/csv does:
`bare " in non-quoted field`, `extraneous or missing " in quoted field`
and `wrong number of fields` stop the read with the line and column.
A CRLF inside a quoted field reads as LF.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = csv_read("a,b\n1,\"x\"y\n", csv_options());
assert(!r.ok);
assert_eq(r.line, 2);
assert_eq(r.col, 6);
var o = csv_options();
o.comment = '#';
var r2 = csv_read("# note\nk,v\n", o);
assert(r2.ok);
assert_eq(csv_field(r2.rows, 0, 1), "v");
```

```fides
pub fn csv_read(text: str, opts: CsvOptions) -> CsvResult {
```

**Parameters:**

- `text`: `str`
- `opts`: `CsvOptions`

**Returns:** `CsvResult`

*Defined at line 424*

---

## csv_write_field

One field as `csv_write` writes it under `opts`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var o = csv_options();
assert_eq(csv_write_field(" lead", o), "\" lead\"");
o.quote_style = CSV_QUOTE_NONNUMERIC;
assert_eq(csv_write_field("2.5e3", o), "2.5e3");
assert_eq(csv_write_field("x", o), "\"x\"");
```

```fides
pub fn csv_write_field(field: str, opts: CsvOptions) -> str {
```

**Parameters:**

- `field`: `str`
- `opts`: `CsvOptions`

**Returns:** `str`

*Defined at line 616*

---

## csv_write

Write rows under `opts` (delimiter, quoting style, line ends). A row
of one empty field is written as `""` so that it reads back as a
record, not as an empty line.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var o = csv_options();
o.crlf = true;
o.quote_style = CSV_QUOTE_ALL;
var rows = csv_parse("a,b\n");
assert_eq(csv_write(rows, o), "\"a\",\"b\"\r\n");
```

```fides
pub fn csv_write(rows: Vec<CsvRow>, opts: CsvOptions) -> str {
```

**Parameters:**

- `rows`: `Vec<CsvRow>`
- `opts`: `CsvOptions`

**Returns:** `str`

*Defined at line 680*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
