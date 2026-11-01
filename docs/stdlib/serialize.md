# serialize — Fides Documentation

## Contents

- [SerializeWriter](#serializewriter) (struct)
- [serialize_writer_create](#serialize_writer_create) (function)
- [serialize_write_u8](#serialize_write_u8) (function)
- [serialize_write_u16](#serialize_write_u16) (function)
- [serialize_write_i32](#serialize_write_i32) (function)
- [serialize_write_f64](#serialize_write_f64) (function)
- [serialize_write_bool](#serialize_write_bool) (function)
- [serialize_write_str](#serialize_write_str) (function)
- [serialize_write_bytes](#serialize_write_bytes) (function)
- [serialize_save](#serialize_save) (function)
- [serialize_data](#serialize_data) (function)
- [serialize_size](#serialize_size) (function)
- [SerializeReader](#serializereader) (struct)
- [serialize_reader_create](#serialize_reader_create) (function)
- [serialize_load](#serialize_load) (function)
- [serialize_read_u8](#serialize_read_u8) (function)
- [serialize_read_u16](#serialize_read_u16) (function)
- [serialize_read_i32](#serialize_read_i32) (function)
- [serialize_read_f64](#serialize_read_f64) (function)
- [serialize_read_bool](#serialize_read_bool) (function)
- [serialize_read_str](#serialize_read_str) (function)
- [serialize_read_bytes](#serialize_read_bytes) (function)
- [serialize_reader_remaining](#serialize_reader_remaining) (function)
- [serialize_reader_eof](#serialize_reader_eof) (function)
- [serialize_reader_reset](#serialize_reader_reset) (function)
- [JsonWriter](#jsonwriter) (struct)
- [json_writer_create](#json_writer_create) (function)
- [json_writer_pretty](#json_writer_pretty) (function)
- [json_object_begin](#json_object_begin) (function)
- [json_array_begin](#json_array_begin) (function)
- [json_array_end](#json_array_end) (function)
- [json_key](#json_key) (function)
- [json_write_string](#json_write_string) (function)
- [json_number](#json_number) (function)
- [json_int](#json_int) (function)
- [json_write_bool](#json_write_bool) (function)
- [json_write_null](#json_write_null) (function)
- [json_comma](#json_comma) (function)
- [json_result](#json_result) (function)
- [json_save](#json_save) (function)
- [JsonValue](#jsonvalue) (struct)
- [json_parse_int](#json_parse_int) (function)
- [json_parse_float](#json_parse_float) (function)
- [json_parse_string](#json_parse_string) (function)
- [json_parse_bool](#json_parse_bool) (function)

## SerializeWriter

```fides
pub struct SerializeWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |

*Defined at line 36*

---

## serialize_writer_create

```fides
pub fn serialize_writer_create() -> SerializeWriter {
```

**Returns:** `SerializeWriter`

*Defined at line 40*

---

## serialize_write_u8

```fides
pub fn serialize_write_u8(w: &mut SerializeWriter, val: i64) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `i64`

**Returns:** `void`

*Defined at line 44*

---

## serialize_write_u16

```fides
pub fn serialize_write_u16(w: &mut SerializeWriter, val: i64) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `i64`

**Returns:** `void`

*Defined at line 48*

---

## serialize_write_i32

```fides
pub fn serialize_write_i32(w: &mut SerializeWriter, val: i64) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `i64`

**Returns:** `void`

*Defined at line 53*

---

## serialize_write_f64

```fides
pub fn serialize_write_f64(w: &mut SerializeWriter, val: f64) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `f64`

**Returns:** `void`

*Defined at line 60*

---

## serialize_write_bool

```fides
pub fn serialize_write_bool(w: &mut SerializeWriter, val: bool) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `bool`

**Returns:** `void`

*Defined at line 70*

---

## serialize_write_str

```fides
pub fn serialize_write_str(w: &mut SerializeWriter, val: str) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `val`: `str`

**Returns:** `void`

*Defined at line 74*

---

## serialize_write_bytes

```fides
pub fn serialize_write_bytes(w: &mut SerializeWriter, data: Vec<i64>) -> void {
```

**Parameters:**

- `w`: `&mut SerializeWriter`
- `data`: `Vec<i64>`

**Returns:** `void`

*Defined at line 84*

---

## serialize_save

Save the writer's data to a file.

```fides
pub fn serialize_save(w: &SerializeWriter, path: str) -> bool {
```

**Parameters:**

- `w`: `&SerializeWriter`
- `path`: `str`

**Returns:** `bool`

*Defined at line 94*

---

## serialize_data

Get the raw bytes.

```fides
pub fn serialize_data(w: &SerializeWriter) -> Vec<i64> {
```

**Parameters:**

- `w`: `&SerializeWriter`

**Returns:** `Vec<i64>`

*Defined at line 100*

---

## serialize_size

Get the size in bytes.

```fides
pub fn serialize_size(w: &SerializeWriter) -> i64 {
```

**Parameters:**

- `w`: `&SerializeWriter`

**Returns:** `i64`

*Defined at line 105*

---

## SerializeReader

```fides
pub struct SerializeReader {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `pos` | `i64` |

*Defined at line 111*

---

## serialize_reader_create

```fides
pub fn serialize_reader_create(data: Vec<i64>) -> SerializeReader {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `SerializeReader`

*Defined at line 116*

---

## serialize_load

Load a file into a reader.

```fides
pub fn serialize_load(path: str) -> SerializeReader {
```

**Parameters:**

- `path`: `str`

**Returns:** `SerializeReader`

*Defined at line 121*

---

## serialize_read_u8

```fides
pub fn serialize_read_u8(r: &mut SerializeReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `i64`

*Defined at line 136*

---

## serialize_read_u16

```fides
pub fn serialize_read_u16(r: &mut SerializeReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `i64`

*Defined at line 143*

---

## serialize_read_i32

```fides
pub fn serialize_read_i32(r: &mut SerializeReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `i64`

*Defined at line 149*

---

## serialize_read_f64

```fides
pub fn serialize_read_f64(r: &mut SerializeReader) -> f64 {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `f64`

*Defined at line 157*

---

## serialize_read_bool

```fides
pub fn serialize_read_bool(r: &mut SerializeReader) -> bool {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `bool`

*Defined at line 167*

---

## serialize_read_str

```fides
pub fn serialize_read_str(r: &mut SerializeReader) -> str {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `str`

*Defined at line 171*

---

## serialize_read_bytes

```fides
pub fn serialize_read_bytes(r: &mut SerializeReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `Vec<i64>`

*Defined at line 184*

---

## serialize_reader_remaining

```fides
pub fn serialize_reader_remaining(r: &SerializeReader) -> i64 {
```

**Parameters:**

- `r`: `&SerializeReader`

**Returns:** `i64`

*Defined at line 195*

---

## serialize_reader_eof

```fides
pub fn serialize_reader_eof(r: &SerializeReader) -> bool {
```

**Parameters:**

- `r`: `&SerializeReader`

**Returns:** `bool`

*Defined at line 199*

---

## serialize_reader_reset

```fides
pub fn serialize_reader_reset(r: &mut SerializeReader) -> void {
```

**Parameters:**

- `r`: `&mut SerializeReader`

**Returns:** `void`

*Defined at line 203*

---

## JsonWriter

```fides
pub struct JsonWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `str` |
| `indent` | `i64` |
| `pretty` | `bool` |

*Defined at line 209*

---

## json_writer_create

```fides
pub fn json_writer_create() -> JsonWriter {
```

**Returns:** `JsonWriter`

*Defined at line 215*

---

## json_writer_pretty

```fides
pub fn json_writer_pretty() -> JsonWriter {
```

**Returns:** `JsonWriter`

*Defined at line 219*

---

## json_object_begin

```fides
pub fn json_object_begin(w: &mut JsonWriter) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`

**Returns:** `void`

*Defined at line 238*

---

## json_array_begin

```fides
pub fn json_array_begin(w: &mut JsonWriter) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`

**Returns:** `void`

*Defined at line 248*

---

## json_array_end

```fides
pub fn json_array_end(w: &mut JsonWriter) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`

**Returns:** `void`

*Defined at line 253*

---

## json_key

```fides
pub fn json_key(w: &mut JsonWriter, key: str) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`
- `key`: `str`

**Returns:** `void`

*Defined at line 258*

---

## json_write_string

```fides
pub fn json_write_string(w: &mut JsonWriter, val: str) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`
- `val`: `str`

**Returns:** `void`

*Defined at line 263*

---

## json_number

```fides
pub fn json_number(w: &mut JsonWriter, val: f64) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`
- `val`: `f64`

**Returns:** `void`

*Defined at line 267*

---

## json_int

```fides
pub fn json_int(w: &mut JsonWriter, val: i64) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`
- `val`: `i64`

**Returns:** `void`

*Defined at line 271*

---

## json_write_bool

```fides
pub fn json_write_bool(w: &mut JsonWriter, val: bool) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`
- `val`: `bool`

**Returns:** `void`

*Defined at line 275*

---

## json_write_null

```fides
pub fn json_write_null(w: &mut JsonWriter) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`

**Returns:** `void`

*Defined at line 279*

---

## json_comma

```fides
pub fn json_comma(w: &mut JsonWriter) -> void {
```

**Parameters:**

- `w`: `&mut JsonWriter`

**Returns:** `void`

*Defined at line 283*

---

## json_result

```fides
pub fn json_result(w: &JsonWriter) -> str {
```

**Parameters:**

- `w`: `&JsonWriter`

**Returns:** `str`

*Defined at line 287*

---

## json_save

Save JSON to a file.

```fides
pub fn json_save(w: &JsonWriter, path: str) -> bool {
```

**Parameters:**

- `w`: `&JsonWriter`
- `path`: `str`

**Returns:** `bool`

*Defined at line 292*

---

## JsonValue

```fides
pub struct JsonValue {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64,     // 0=null, 1=bool, 2=int, 3=float, 4=string, 5=array, 6=object` |
| `bool_val` | `bool` |
| `int_val` | `i64` |
| `float_val` | `f64` |
| `str_val` | `str` |

*Defined at line 301*

---

## json_parse_int

```fides
pub fn json_parse_int(source: str, key: str) -> i64 {
```

**Parameters:**

- `source`: `str`
- `key`: `str`

**Returns:** `i64`

*Defined at line 309*

---

## json_parse_float

```fides
pub fn json_parse_float(source: str, key: str) -> f64 {
```

**Parameters:**

- `source`: `str`
- `key`: `str`

**Returns:** `f64`

*Defined at line 317*

---

## json_parse_string

```fides
pub fn json_parse_string(source: str, key: str) -> str {
```

**Parameters:**

- `source`: `str`
- `key`: `str`

**Returns:** `str`

*Defined at line 325*

---

## json_parse_bool

```fides
pub fn json_parse_bool(source: str, key: str) -> bool {
```

**Parameters:**

- `source`: `str`
- `key`: `str`

**Returns:** `bool`

*Defined at line 335*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
