[All modules](index.md)

# toml -- Fides Documentation

## Contents

- [TomlValue](#TomlValue) (struct)
- [TomlEntry](#TomlEntry) (struct)
- [TomlDoc](#TomlDoc) (struct)
- [TOML_MAX_DEPTH:](#TOML_MAX_DEPTH:) (const)
- [toml_max_depth](#toml_max_depth) (function)
- [toml_load](#toml_load) (function)
- [toml_node_kind](#toml_node_kind) (function)
- [toml_node_value](#toml_node_value) (function)
- [toml_node_items](#toml_node_items) (function)
- [toml_node_len](#toml_node_len) (function)
- [toml_node_keys](#toml_node_keys) (function)
- [toml_node_child](#toml_node_child) (function)
- [toml_node_at](#toml_node_at) (function)
- [toml_lookup](#toml_lookup) (function)
- [toml_node_json](#toml_node_json) (function)
- [toml_to_json](#toml_to_json) (function)
- [toml_parse](#toml_parse) (function)
- [toml_get](#toml_get) (function)
- [toml_has](#toml_has) (function)
- [toml_kind](#toml_kind) (function)
- [toml_get_str](#toml_get_str) (function)
- [toml_get_int](#toml_get_int) (function)
- [toml_get_float](#toml_get_float) (function)
- [toml_get_bool](#toml_get_bool) (function)
- [toml_keys](#toml_keys) (function)
- [toml_table_keys](#toml_table_keys) (function)

## TomlValue

A typed TOML value of the flat form. `kind` is one of
"str" | "int" | "float" | "bool" | "array" | "invalid", or for a date
or time the toml_load kind ("datetime", "date-local", ...).
`sval` holds the decoded scalar; `items` the element values of an
array ("" for an element that is itself an array or table).

```fides
pub struct TomlValue {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `sval` | `str` |
| `items` | `Vec<str>` |

*Defined at line 77*

---

## TomlEntry

One entry: dotted full path → value.

```fides
pub struct TomlEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `value` | `TomlValue` |

*Defined at line 84*

---

## TomlDoc

A parsed document (see the module header). `ok` false: `error` says
why and where. `first`/`next`/`last` link each node's children (first
child, next sibling, last child; -1 for none); the last four fields are
the parser's own state.

```fides
pub struct TomlDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `kind` | `Vec<str>` |
| `sval` | `Vec<str>` |
| `parent` | `Vec<i64>` |
| `key` | `Vec<str>` |
| `flags` | `Vec<i64>` |
| `sect` | `Vec<i64>` |
| `first` | `Vec<i64>` |
| `next` | `Vec<i64>` |
| `last` | `Vec<i64>` |
| `src` | `str` |
| `lenient` | `bool` |
| `cur` | `i64` |
| `section` | `i64` |
| `depth` | `i64` |

*Defined at line 101*

---

## TOML_MAX_DEPTH:

Arrays and inline tables nested deeper than this are an error in
toml_load, and a skipped line in toml_parse, instead of overflowing the
stack. Lower than JSON_MAX_DEPTH: the parser makes two or three calls a
level under the interpreter's limit of 1000.

```fides
pub const TOML_MAX_DEPTH: i64 = 128;
```

*Defined at line 131*

---

## toml_max_depth

The deepest nesting this build accepts: TOML_MAX_DEPTH on every backend
(WebAssembly parses 128 arrays deep; xml_max_depth is the one that
follows the backend).

```fides
pub fn toml_max_depth() -> i64 {
```

**Returns:** `i64`

*Defined at line 136*

---

## toml_load

Parse TOML text strictly (see the module header).

```fides
pub fn toml_load(text: str) -> TomlDoc {
```

**Parameters:**

- `text`: `str`

**Returns:** `TomlDoc`

*Defined at line 1362*

---

## toml_node_kind

The kind of `node` ("missing" for -1).

```fides
pub fn toml_node_kind(doc: TomlDoc, node: i64) -> str {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1367*

---

## toml_node_value

The value of a scalar node as text ("" for tables, arrays, -1).

```fides
pub fn toml_node_value(doc: TomlDoc, node: i64) -> str {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1375*

---

## toml_node_items

Child nodes of a table or array, in document order.

```fides
pub fn toml_node_items(doc: TomlDoc, node: i64) -> Vec<i64> {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1383*

---

## toml_node_len

Number of elements of an array or keys of a table.

```fides
pub fn toml_node_len(doc: TomlDoc, node: i64) -> i64 {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 1399*

---

## toml_node_keys

Keys of a table, in document order.

```fides
pub fn toml_node_keys(doc: TomlDoc, node: i64) -> Vec<str> {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `Vec<str>`

*Defined at line 1404*

---

## toml_node_child

The child of table `node` with key `k`, or -1.

```fides
pub fn toml_node_child(doc: TomlDoc, node: i64, k: str) -> i64 {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`
- `k`: `str`

**Returns:** `i64`

*Defined at line 1416*

---

## toml_node_at

Element `index` of array `node`, or -1.

```fides
pub fn toml_node_at(doc: TomlDoc, node: i64, index: i64) -> i64 {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`
- `index`: `i64`

**Returns:** `i64`

*Defined at line 1431*

---

## toml_lookup

The node at a dotted path from the root ("a.b.2.c": a number indexes
an array); -1 when absent. Keys holding '.' need toml_node_child.

```fides
pub fn toml_lookup(doc: TomlDoc, path: str) -> i64 {
```

**Parameters:**

- `doc`: `TomlDoc`
- `path`: `str`

**Returns:** `i64`

*Defined at line 1449*

---

## toml_node_json

`node` in the JSON form of the toml-test suite: tables as objects
(keys sorted bytewise), arrays as arrays, scalars as
{"type":"integer","value":"1"}.

```fides
pub fn toml_node_json(doc: TomlDoc, node: i64) -> str {
```

**Parameters:**

- `doc`: `TomlDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1587*

---

## toml_to_json

The whole document in the toml-test JSON form; "" when it failed.

```fides
pub fn toml_to_json(doc: TomlDoc) -> str {
```

**Parameters:**

- `doc`: `TomlDoc`

**Returns:** `str`

*Defined at line 1597*

---

## toml_parse

Parse TOML text into Vec<TomlEntry> (document order), leniently:
see the module header. Tables become path prefixes; array elements
are listed in their array's `items`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val doc = toml_parse("title = \"app\"\n[server]\nport = 8080\ndebug = true\n");
assert_eq(toml_get_str(doc, "title"), "app");
assert_eq(toml_get_int(doc, "server.port", 0), 8080);
assert(toml_get_bool(doc, "server.debug", false));
assert_eq(toml_get_int(doc, "server.missing", 7), 7);
assert_eq(toml_kind(doc, "server.port"), "int");
val d = toml_load("a = [1, 2, 3]\n");
assert_eq(toml_node_len(d, toml_lookup(d, "a")), 3);
assert_eq(toml_node_value(d, toml_lookup(d, "a.1")), "2");
```

```fides
pub fn toml_parse(text: str) -> Vec<TomlEntry> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<TomlEntry>`

*Defined at line 1630*

---

## toml_get

The TomlValue at dotted `path`; kind "invalid" when absent.

```fides
pub fn toml_get(doc: Vec<TomlEntry>, path: str) -> TomlValue {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`

**Returns:** `TomlValue`

*Defined at line 1676*

---

## toml_has

True if `path` exists.

```fides
pub fn toml_has(doc: Vec<TomlEntry>, path: str) -> bool {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`

**Returns:** `bool`

*Defined at line 1689*

---

## toml_kind

Kind string for `path` ("invalid" if missing).

```fides
pub fn toml_kind(doc: Vec<TomlEntry>, path: str) -> str {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`

**Returns:** `str`

*Defined at line 1694*

---

## toml_get_str

String value at `path` ("" when absent).

```fides
pub fn toml_get_str(doc: Vec<TomlEntry>, path: str) -> str {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`

**Returns:** `str`

*Defined at line 1699*

---

## toml_get_int

Int value at `path`, `default` when absent or not numeric.

```fides
pub fn toml_get_int(doc: Vec<TomlEntry>, path: str, default: i64) -> i64 {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 1704*

---

## toml_get_float

Float value at `path`, `default` when absent or not numeric.

```fides
pub fn toml_get_float(doc: Vec<TomlEntry>, path: str, default: f64) -> f64 {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`
- `default`: `f64`

**Returns:** `f64`

*Defined at line 1716*

---

## toml_get_bool

Bool value at `path`, `default` when absent or not a bool.

```fides
pub fn toml_get_bool(doc: Vec<TomlEntry>, path: str, default: bool) -> bool {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`
- `default`: `bool`

**Returns:** `bool`

*Defined at line 1725*

---

## toml_keys

All dotted key paths in document order.

```fides
pub fn toml_keys(doc: Vec<TomlEntry>) -> Vec<str> {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`

**Returns:** `Vec<str>`

*Defined at line 1734*

---

## toml_table_keys

Immediate child keys of table `path` ("" for the root table).
e.g. with entries a.b and a.c.d, toml_table_keys(doc,"a") → ["b","c"].

```fides
pub fn toml_table_keys(doc: Vec<TomlEntry>, path: str) -> Vec<str> {
```

**Parameters:**

- `doc`: `Vec<TomlEntry>`
- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 1746*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
