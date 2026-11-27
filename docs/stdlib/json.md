[All modules](index.md)

# json -- Fides Documentation

## Contents

- [json_is_valid](#json_is_valid) (function)
- [json_string](#json_string) (function)
- [json_unescape](#json_unescape) (function)
- [json_parse](#json_parse) (function)
- [json_dump](#json_dump) (function)
- [json_get](#json_get) (function)
- [json_at](#json_at) (function)
- [json_type](#json_type) (function)
- [json_len](#json_len) (function)
- [json_pretty](#json_pretty) (function)
- [json_pair](#json_pair) (function)
- [json_parse_safe](#json_parse_safe) (function)
- [json_type_safe](#json_type_safe) (function)
- [json_get_safe](#json_get_safe) (function)
- [json_at_safe](#json_at_safe) (function)
- [json_len_safe](#json_len_safe) (function)
- [json_pretty_safe](#json_pretty_safe) (function)
- [json_get_str](#json_get_str) (function)
- [json_get_int](#json_get_int) (function)
- [json_get_num](#json_get_num) (function)
- [json_get_bool](#json_get_bool) (function)
- [json_has](#json_has) (function)
- [json_at_str](#json_at_str) (function)
- [json_at_int](#json_at_int) (function)
- [json_keys](#json_keys) (function)
- [json_items](#json_items) (function)
- [json_path](#json_path) (function)
- [json_obj_set](#json_obj_set) (function)
- [json_arr_push](#json_arr_push) (function)
- [json_num](#json_num) (function)
- [json_bool](#json_bool) (function)
- [json_null](#json_null) (function)
- [JSON_NULL:](#JSON_NULL:) (const)
- [JSON_FALSE:](#JSON_FALSE:) (const)
- [JSON_TRUE:](#JSON_TRUE:) (const)
- [JSON_NUMBER:](#JSON_NUMBER:) (const)
- [JSON_STRING:](#JSON_STRING:) (const)
- [JSON_ARRAY:](#JSON_ARRAY:) (const)
- [JSON_OBJECT:](#JSON_OBJECT:) (const)
- [JsonDoc](#JsonDoc) (struct)
- [json_index](#json_index) (function)
- [jd_root](#jd_root) (function)
- [jd_nodes](#jd_nodes) (function)
- [jd_kind](#jd_kind) (function)
- [jd_type](#jd_type) (function)
- [jd_len](#jd_len) (function)
- [jd_raw](#jd_raw) (function)
- [jd_at](#jd_at) (function)
- [jd_get](#jd_get) (function)
- [jd_has](#jd_has) (function)
- [jd_key](#jd_key) (function)
- [jd_member](#jd_member) (function)
- [jd_children](#jd_children) (function)
- [jd_str](#jd_str) (function)
- [jd_int](#jd_int) (function)
- [jd_num](#jd_num) (function)
- [jd_bool](#jd_bool) (function)
- [jd_is_null](#jd_is_null) (function)
- [jd_path](#jd_path) (function)

## json_is_valid

True when `s` is well-formed JSON (RFC 8259 grammar, no trailing junk).
Pure Fides — never panics.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(json_is_valid("{\"a\": [1, 2, true]}"));
assert(!json_is_valid("{\"a\": }"));
```

```fides
pub fn json_is_valid(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 450*

---

## json_string

Serialize a plain string as a JSON string literal (quotes + escaping).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var lit = json_string("say \"hi\"");
println(lit);
assert_eq(lit, "\"say \\\"hi\\\"\"");
```

```fides
pub fn json_string(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 676*

---

## json_unescape

Decode a JSON string literal (with surrounding quotes) to plain text.
Handles \" \\ \/ \n \r \t \b \f and \uXXXX (UTF-8 encoded).
On malformed input returns the input unchanged.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_unescape("\"a\\nb\""), "a\nb");
```

```fides
pub fn json_unescape(lit: str) -> str {
```

**Parameters:**

- `lit`: `str`

**Returns:** `str`

*Defined at line 705*

---

## json_parse

Parse a JSON string and return its canonical (minified, keys sorted
recursively) form. PANICS on parse error — use json_parse_safe for a
non-panicking version.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = json_parse("{ \"b\": 2, \"a\": [1, 2] }");
println(c);
assert_eq(c, "{\"a\":[1,2],\"b\":2}");
```

```fides
pub fn json_parse(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 815*

---

## json_dump

Alias for json_parse.

```fides
pub fn json_dump(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 823*

---

## json_get

Extract a field from a JSON object. Returns the raw JSON value as a
string, or "null" when the key is absent.
PANICS if `s` is not a valid JSON object — use json_get_safe instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var doc = "{\"name\": \"Ada\", \"age\": 36}";
assert_eq(json_get(doc, "age"), "36");
assert_eq(json_get(doc, "name"), "\"Ada\"");
assert_eq(json_get(doc, "missing"), "null");
```

```fides
pub fn json_get(s: str, key: str) -> str {
```

**Parameters:**

- `s`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 836*

---

## json_at

Extract an element from a JSON array by index.
PANICS on out-of-bounds or non-array — use json_at_safe instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_at("[10, 20, 30]", 1), "20");
```

```fides
pub fn json_at(s: str, idx: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 858*

---

## json_type

Type name of a JSON value: "object"|"array"|"string"|"number"|
"boolean"|"null". PANICS on invalid JSON — use json_type_safe instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_type("[1]"), "array");
assert_eq(json_type("\"x\""), "string");
assert_eq(json_type("3.5"), "number");
```

```fides
pub fn json_type(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 877*

---

## json_len

Length of a JSON object or array. PANICS for scalar types or invalid
JSON — use json_len_safe instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_len("[1, 2, 3]"), 3);
assert_eq(json_len("{\"a\": 1}"), 1);
```

```fides
pub fn json_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 890*

---

## json_pretty

Pretty-print a JSON string with 2-space indentation (canonical key
order). PANICS on invalid JSON — use json_pretty_safe instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = json_pretty("{\"a\":1}");
println(p);
assert_eq(p, "{\n  \"a\": 1\n}");
```

```fides
pub fn json_pretty(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 919*

---

## json_pair

Build a JSON object from a key and raw JSON value. Returns {"key":value}.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_pair("n", "1"), "{\"n\":1}");
```

```fides
pub fn json_pair(key: str, value: str) -> str {
```

**Parameters:**

- `key`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 980*

---

## json_parse_safe

Canonicalize `s`, or "" when it is not valid JSON.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_parse_safe("[1, 2]"), "[1,2]");
assert_eq(json_parse_safe("[1,"), "");
```

```fides
pub fn json_parse_safe(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 991*

---

## json_type_safe

Type name of a JSON value, or "error" when `s` is not valid JSON.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_type_safe("null"), "null");
assert_eq(json_type_safe("{oops"), "error");
```

```fides
pub fn json_type_safe(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 1003*

---

## json_get_safe

Field of a JSON object as raw JSON text; "" on any failure, "null"
when the key is absent (matching historical json_get semantics).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_get_safe("{\"a\": true}", "a"), "true");
assert_eq(json_get_safe("not json", "a"), "");
```

```fides
pub fn json_get_safe(s: str, key: str) -> str {
```

**Parameters:**

- `s`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 1016*

---

## json_at_safe

Element of a JSON array as raw JSON text; "" on any failure.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_at_safe("[1, 2]", 5), "");
```

```fides
pub fn json_at_safe(s: str, idx: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 1037*

---

## json_len_safe

Length of a JSON object/array; -1 otherwise.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_len_safe("[]"), 0);
assert_eq(json_len_safe("42"), -1);
```

```fides
pub fn json_len_safe(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 1054*

---

## json_pretty_safe

Pretty-print, or "" on invalid JSON.

```fides
pub fn json_pretty_safe(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 1066*

---

## json_get_str

Field of an object as a plain (unescaped) string; "" when the field is
missing, not a string, or the input is invalid.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_get_str("{\"name\": \"Ada\"}", "name"), "Ada");
assert_eq(json_get_str("{\"n\": 1}", "n"), "");
```

```fides
pub fn json_get_str(s: str, key: str) -> str {
```

**Parameters:**

- `s`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 1081*

---

## json_get_int

Field of an object as an integer; `def` when missing/not a number.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var doc = "{\"port\": 8080}";
assert_eq(json_get_int(doc, "port", 80), 8080);
assert_eq(json_get_int(doc, "timeout", 30), 30);
```

```fides
pub fn json_get_int(s: str, key: str, def: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `key`: `str`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 1095*

---

## json_get_num

Field of an object as a float; `def` when missing/not a number.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(json_get_num("{\"x\": 2.5}", "x", 0.0) == 2.5);
```

```fides
pub fn json_get_num(s: str, key: str, def: f64) -> f64 {
```

**Parameters:**

- `s`: `str`
- `key`: `str`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 1107*

---

## json_get_bool

Field of an object as a bool; `def` when missing/not a boolean.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(json_get_bool("{\"debug\": true}", "debug", false));
assert(!json_get_bool("{}", "debug", false));
```

```fides
pub fn json_get_bool(s: str, key: str, def: bool) -> bool {
```

**Parameters:**

- `s`: `str`
- `key`: `str`
- `def`: `bool`

**Returns:** `bool`

*Defined at line 1120*

---

## json_has

True when `key` exists on object `s` (even if its value is null).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(json_has("{\"a\": null}", "a"));
assert(!json_has("{\"a\": null}", "b"));
```

```fides
pub fn json_has(s: str, key: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `key`: `str`

**Returns:** `bool`

*Defined at line 1136*

---

## json_at_str

Array element as a plain string; "" when out of bounds or not a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_at_str("[\"x\", \"y\"]", 1), "y");
```

```fides
pub fn json_at_str(s: str, idx: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 1153*

---

## json_at_int

Array element as an integer; `def` otherwise.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_at_int("[4, 5]", 0, -1), 4);
assert_eq(json_at_int("[4, 5]", 9, -1), -1);
```

```fides
pub fn json_at_int(s: str, idx: i64, def: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `idx`: `i64`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 1166*

---

## json_keys

Top-level keys of a JSON object as a Vec of plain strings, in document
order. Empty Vec when `s` is not an object.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var keys = json_keys("{\"b\": 1, \"a\": 2}");
assert_eq(keys.len(), 2);
assert_eq(keys.get(0), "b");
```

```fides
pub fn json_keys(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 1183*

---

## json_items

Top-level elements of a JSON array as a Vec of raw JSON strings.
Empty Vec when `s` is not an array.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var items = json_items("[1, \"two\", [3]]");
assert_eq(items.len(), 3);
assert_eq(items.get(2), "[3]");
```

```fides
pub fn json_items(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 1205*

---

## json_path

Navigate a dot-separated path: "a.b.0.c" reads field a, then b, then
array index 0, then c. Returns raw JSON text or "" on any miss
("null" when the final lookup lands on a missing key).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var doc = "{\"users\": [{\"name\": \"Ada\"}]}";
assert_eq(json_path(doc, "users.0.name"), "\"Ada\"");
```

```fides
pub fn json_path(s: str, path: str) -> str {
```

**Parameters:**

- `s`: `str`
- `path`: `str`

**Returns:** `str`

*Defined at line 1229*

---

## json_obj_set

Set (or replace) field `key` on JSON object `obj` with raw JSON `value`.
Works on any valid object text; returns obj unchanged if invalid.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var o = json_obj_set("{\"a\": 1}", "b", "2");
assert_eq(json_get_int(o, "b", 0), 2);
assert_eq(json_get_int(o, "a", 0), 1);
```

```fides
pub fn json_obj_set(obj: str, key: str, value: str) -> str {
```

**Parameters:**

- `obj`: `str`
- `key`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 1261*

---

## json_arr_push

Append raw JSON `value` to JSON array `arr`; returns arr if invalid.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = json_arr_push("[1]", "2");
assert_eq(json_len(a), 2);
assert_eq(json_at(a, 1), "2");
```

```fides
pub fn json_arr_push(arr: str, value: str) -> str {
```

**Parameters:**

- `arr`: `str`
- `value`: `str`

**Returns:** `str`

*Defined at line 1296*

---

## json_num

Number literal for building JSON.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_num(-5), "-5");
```

```fides
pub fn json_num(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 1313*

---

## json_bool

Bool literal for building JSON.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_bool(true), "true");
```

```fides
pub fn json_bool(b: bool) -> str {
```

**Parameters:**

- `b`: `bool`

**Returns:** `str`

*Defined at line 1321*

---

## json_null

Null literal.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(json_null(), "null");
```

```fides
pub fn json_null() -> str {
```

**Returns:** `str`

*Defined at line 1332*

---

## JSON_NULL:

Node kinds (jd_kind).

```fides
pub const JSON_NULL: i64 = 0;
```

*Defined at line 1355*

---

## JSON_FALSE:

```fides
pub const JSON_FALSE: i64 = 1;
```

*Defined at line 1356*

---

## JSON_TRUE:

```fides
pub const JSON_TRUE: i64 = 2;
```

*Defined at line 1357*

---

## JSON_NUMBER:

```fides
pub const JSON_NUMBER: i64 = 3;
```

*Defined at line 1358*

---

## JSON_STRING:

```fides
pub const JSON_STRING: i64 = 4;
```

*Defined at line 1359*

---

## JSON_ARRAY:

```fides
pub const JSON_ARRAY: i64 = 5;
```

*Defined at line 1360*

---

## JSON_OBJECT:

```fides
pub const JSON_OBJECT: i64 = 6;
```

*Defined at line 1361*

---

## JsonDoc

A document indexed by json_index.

```fides
pub struct JsonDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `text` | `str` |
| `ok` | `bool` |
| `kind` | `Vec<i64>` |
| `start` | `Vec<i64>` |
| `end` | `Vec<i64>` |
| `next` | `Vec<i64>` |
| `count` | `Vec<i64>` |

*Defined at line 1367*

---

## json_index

Index a document: one scan that also validates it (`ok`).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val d: JsonDoc = json_index("[1, 2, \"x\", [true, null]]");
assert(d.ok);
assert_eq(jd_len(& d, jd_root(& d)), 4);
assert_eq(jd_str(& d, jd_at(& d, jd_root(& d), 2)), "x");
assert_eq(jd_raw(& d, jd_at(& d, jd_root(& d), 3)), "[true, null]");
```

```fides
pub fn json_index(s: str) -> JsonDoc {
```

**Parameters:**

- `s`: `str`

**Returns:** `JsonDoc`

*Defined at line 1506*

---

## jd_root

The root node: 0, or -1 when the document is not valid.

```fides
pub fn jd_root(d: &JsonDoc) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`

**Returns:** `i64`

*Defined at line 1519*

---

## jd_nodes

How many nodes the document has.

```fides
pub fn jd_nodes(d: &JsonDoc) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`

**Returns:** `i64`

*Defined at line 1527*

---

## jd_kind

The kind of a node (JSON_NULL .. JSON_OBJECT), -1 for no node.

```fides
pub fn jd_kind(d: &JsonDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 1536*

---

## jd_type

The kind as json_type names it: "null", "boolean", "number", "string",
"array", "object"; "error" for no node.

```fides
pub fn jd_type(d: &JsonDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1545*

---

## jd_len

Elements of an array or members of an object; 0 for anything else.

```fides
pub fn jd_len(d: &JsonDoc, node: i64) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `i64`

*Defined at line 1569*

---

## jd_raw

The raw text of a node (a string keeps its quotes); "" for no node.

```fides
pub fn jd_raw(d: &JsonDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1577*

---

## jd_at

Element `idx` of an array, -1 when out of range or not an array.

```fides
pub fn jd_at(d: &JsonDoc, node: i64, idx: i64) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 1585*

---

## jd_get

The value of field `key` of an object (the first, when a key repeats),
-1 when it is missing or `node` is not an object.

```fides
pub fn jd_get(d: &JsonDoc, node: i64, key: str) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `key`: `str`

**Returns:** `i64`

*Defined at line 1623*

---

## jd_has

Whether an object has field `key` (even when it is null).

```fides
pub fn jd_has(d: &JsonDoc, node: i64, key: str) -> bool {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `key`: `str`

**Returns:** `bool`

*Defined at line 1642*

---

## jd_key

The key of member `idx` of an object, decoded; "" when out of range.

```fides
pub fn jd_key(d: &JsonDoc, node: i64, idx: i64) -> str {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 1661*

---

## jd_member

The value of member `idx` of an object, -1 when out of range.

```fides
pub fn jd_member(d: &JsonDoc, node: i64, idx: i64) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 1670*

---

## jd_children

The elements of an array, or the members' values of an object, in
order, found in one walk; empty for anything else. jd_at and jd_member
walk from the first child at every call, so a loop over every element
should take this once and index it.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val d: JsonDoc = json_index("[10, 20, 30]");
val kids: Vec<i64> = jd_children(& d, jd_root(& d));
assert_eq(jd_int(& d, kids.get(2), 0), 30);
```

```fides
pub fn jd_children(d: &JsonDoc, node: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1687*

---

## jd_str

A string node decoded; "" for anything else.

```fides
pub fn jd_str(d: &JsonDoc, node: i64) -> str {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `str`

*Defined at line 1711*

---

## jd_int

A number node as an integer (read as json_get_int reads it); `def` for
anything else.

```fides
pub fn jd_int(d: &JsonDoc, node: i64, def: i64) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 1720*

---

## jd_num

A number node as a float; `def` for anything else.

```fides
pub fn jd_num(d: &JsonDoc, node: i64, def: f64) -> f64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 1728*

---

## jd_bool

A true or false node; `def` for anything else.

```fides
pub fn jd_bool(d: &JsonDoc, node: i64, def: bool) -> bool {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `def`: `bool`

**Returns:** `bool`

*Defined at line 1736*

---

## jd_is_null

Whether a node is null (false for no node).

```fides
pub fn jd_is_null(d: &JsonDoc, node: i64) -> bool {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`

**Returns:** `bool`

*Defined at line 1748*

---

## jd_path

Follow a dot-separated path from `node`, as json_path does: "a.b.0.c"
is field a, field b, element 0, field c. -1 on any miss.

```fides
pub fn jd_path(d: &JsonDoc, node: i64, path: str) -> i64 {
```

**Parameters:**

- `d`: `&JsonDoc`
- `node`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 1754*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
