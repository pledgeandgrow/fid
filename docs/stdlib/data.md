[All modules](index.md)

# data -- Fides Documentation

## Contents

- [load_csv](#load_csv) (function)
- [parse_csv](#parse_csv) (function)
- [parse_csv_row](#parse_csv_row) (function)
- [csv_headers](#csv_headers) (function)
- [csv_row](#csv_row) (function)
- [csv_get](#csv_get) (function)
- [csv_row_count](#csv_row_count) (function)
- [load_json](#load_json) (function)
- [json_get_value](#json_get_value) (function)
- [infer_json_schema](#infer_json_schema) (function)
- [comptime_csv_schema](#comptime_csv_schema) (function)
- [comptime_json_schema](#comptime_json_schema) (function)
- [infer_op_shape](#infer_op_shape) (function)
- [infer_shapes](#infer_shapes) (function)

## load_csv

CSV data — stored as a JSON array of objects (one per row).
Each object has keys = column headers, values = string cells.
This format works with the existing json module.
Load a CSV file and parse it into a JSON array of row objects.
The first line is treated as the header row.
Returns a JSON string: [{"col1":"val1","col2":"val2"}, ...]

```fides
pub fn load_csv(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 30*

---

## parse_csv

Parse CSV text into a JSON array of row objects.
Handles quoted fields with commas, but not multi-line quoted fields.

```fides
pub fn parse_csv(content: str) -> str {
```

**Parameters:**

- `content`: `str`

**Returns:** `str`

*Defined at line 40*

---

## parse_csv_row

Parse a single CSV row into a Vec of cell values.
Handles quoted fields: "hello, world" → hello, world


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val cells = parse_csv_row("a,\"b, c\",d");
assert_eq(cells.len(), 3);
assert_eq(cells.get(1), "b, c");
val t = parse_csv("name,age\nann,30\nbob,41\n");
assert_eq(csv_row_count(t), 2);
assert_eq(csv_get(csv_row(t, 1), "name"), "bob");
```

```fides
pub fn parse_csv_row(line: str) -> Vec<str> {
```

**Parameters:**

- `line`: `str`

**Returns:** `Vec<str>`

*Defined at line 92*

---

## csv_headers

Get the column headers from a parsed CSV (as JSON array of objects).
Returns a JSON array of header strings.

```fides
pub fn csv_headers(csv_json: str) -> str {
```

**Parameters:**

- `csv_json`: `str`

**Returns:** `str`

*Defined at line 127*

---

## csv_row

Get a specific row from a parsed CSV.
Returns a JSON object with column→value mappings.

```fides
pub fn csv_row(csv_json: str, row_idx: i64) -> str {
```

**Parameters:**

- `csv_json`: `str`
- `row_idx`: `i64`

**Returns:** `str`

*Defined at line 146*

---

## csv_get

Get a cell value from a CSV row by column name.

```fides
pub fn csv_get(row_json: str, col_name: str) -> str {
```

**Parameters:**

- `row_json`: `str`
- `col_name`: `str`

**Returns:** `str`

*Defined at line 151*

---

## csv_row_count

Get the number of rows in a parsed CSV.

```fides
pub fn csv_row_count(csv_json: str) -> i64 {
```

**Parameters:**

- `csv_json`: `str`

**Returns:** `i64`

*Defined at line 156*

---

## load_json

Load a JSON file and return the parsed JSON string.
"" when the file cannot be read.

```fides
pub fn load_json(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 164*

---

## json_get_value

Get a value from a JSON object by key.

```fides
pub fn json_get_value(json_str: str, key: str) -> str {
```

**Parameters:**

- `json_str`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 170*

---

## infer_json_schema

Infer the "schema" of a JSON object — return a description of the
types of each top-level key. Returns a JSON object mapping keys to
type names: {"name":"string","age":"number","active":"boolean"}

```fides
pub fn infer_json_schema(json_str: str) -> str {
```

**Parameters:**

- `json_str`: `str`

**Returns:** `str`

*Defined at line 177*

---

## comptime_csv_schema

Comptime CSV schema inference placeholder.
When comptime gen is available, this will be replaced by a comptime
function that generates a typed struct from the CSV header.

```fides
pub fn comptime_csv_schema(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 231*

---

## comptime_json_schema

Comptime JSON schema inference placeholder.

```fides
pub fn comptime_json_schema(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 255*

---

## infer_op_shape

Infer the output shape of a tensor operation.
Returns a JSON array representing the shape, e.g. [3, 224, 224]

```fides
pub fn infer_op_shape(op: str, input_shape: str) -> str {
```

**Parameters:**

- `op`: `str`
- `input_shape`: `str`

**Returns:** `str`

*Defined at line 268*

---

## infer_shapes

Infer shapes through a sequence of operations.
ops: JSON array of operation names
input_shape: JSON array of dimension sizes
Returns: JSON array of output shape

```fides
pub fn infer_shapes(ops: str, input_shape: str) -> str {
```

**Parameters:**

- `ops`: `str`
- `input_shape`: `str`

**Returns:** `str`

*Defined at line 297*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
