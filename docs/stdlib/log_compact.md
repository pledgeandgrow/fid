[All modules](index.md)

# log_compact -- Fides Documentation

## Contents

- [LOG_TRACE:](#LOG_TRACE:) (const)
- [LOG_DEBUG:](#LOG_DEBUG:) (const)
- [LOG_INFO:](#LOG_INFO:) (const)
- [LOG_WARN:](#LOG_WARN:) (const)
- [LOG_ERROR:](#LOG_ERROR:) (const)
- [log_level_name](#log_level_name) (function)
- [log_put_uvarint](#log_put_uvarint) (function)
- [log_zigzag](#log_zigzag) (function)
- [log_unzigzag](#log_unzigzag) (function)
- [log_put_svarint](#log_put_svarint) (function)
- [LogReader](#LogReader) (struct)
- [log_reader](#log_reader) (function)
- [log_get_uvarint](#log_get_uvarint) (function)
- [log_get_svarint](#log_get_svarint) (function)
- [log_arg_kinds](#log_arg_kinds) (function)
- [LogTable](#LogTable) (struct)
- [logtable_new](#logtable_new) (function)
- [logtable_register](#logtable_register) (function)
- [logtable_export](#logtable_export) (function)
- [logtable_import](#logtable_import) (function)
- [LogEncoder](#LogEncoder) (struct)
- [logenc_new](#logenc_new) (function)
- [logenc_emit](#logenc_emit) (function)
- [LogLine](#LogLine) (struct)
- [log_decode_one](#log_decode_one) (function)
- [log_decode_all](#log_decode_all) (function)

## LOG_TRACE:

```fides
pub const LOG_TRACE: i64 = 0;
```

*Defined at line 25*

---

## LOG_DEBUG:

```fides
pub const LOG_DEBUG: i64 = 1;
```

*Defined at line 26*

---

## LOG_INFO:

```fides
pub const LOG_INFO: i64 = 2;
```

*Defined at line 27*

---

## LOG_WARN:

```fides
pub const LOG_WARN: i64 = 3;
```

*Defined at line 28*

---

## LOG_ERROR:

```fides
pub const LOG_ERROR: i64 = 4;
```

*Defined at line 29*

---

## log_level_name

```fides
pub fn log_level_name(l: i64) -> str {
```

**Parameters:**

- `l`: `i64`

**Returns:** `str`

*Defined at line 31*

---

## log_put_uvarint

Unsigned LEB128 (the value is taken as a 64-bit unsigned pattern).

```fides
pub fn log_put_uvarint(out: &mut Vec<i64>, v0: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v0`: `i64`

*Defined at line 42*

---

## log_zigzag

```fides
pub fn log_zigzag(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 56*

---

## log_unzigzag

```fides
pub fn log_unzigzag(z: i64) -> i64 {
```

**Parameters:**

- `z`: `i64`

**Returns:** `i64`

*Defined at line 60*

---

## log_put_svarint

```fides
pub fn log_put_svarint(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 64*

---

## LogReader

A cursor over received bytes.

```fides
pub struct LogReader {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `pos` | `i64` |
| `bad` | `bool` |

*Defined at line 69*

---

## log_reader

```fides
pub fn log_reader(bytes: Vec<i64>) -> LogReader {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `LogReader`

*Defined at line 75*

---

## log_get_uvarint

```fides
pub fn log_get_uvarint(r: &mut LogReader) -> i64 {
```

**Parameters:**

- `r`: `&mut LogReader`

**Returns:** `i64`

*Defined at line 81*

---

## log_get_svarint

```fides
pub fn log_get_svarint(r: &mut LogReader) -> i64 {
```

**Parameters:**

- `r`: `&mut LogReader`

**Returns:** `i64`

*Defined at line 100*

---

## log_arg_kinds

The argument kinds of a format, one letter each: d u x b c s q.

```fides
pub fn log_arg_kinds(fmt: str) -> str {
```

**Parameters:**

- `fmt`: `str`

**Returns:** `str`

*Defined at line 107*

---

## LogTable

```fides
pub struct LogTable {
```

**Fields:**

| Name | Type |
|------|------|
| `formats` | `Vec<str>` |
| `levels` | `Vec<i64>` |
| `kinds` | `Vec<str>` |
| `index` | `HashMap<str` |

*Defined at line 142*

---

## logtable_new

```fides
pub fn logtable_new() -> LogTable {
```

**Returns:** `LogTable`

*Defined at line 150*

---

## logtable_register

Intern a format string at a level; the same (level, format) gives the
same index. Panics on a malformed placeholder.

```fides
pub fn logtable_register(t: &mut LogTable, level: i64, fmt: str) -> i64 {
```

**Parameters:**

- `t`: `&mut LogTable`
- `level`: `i64`
- `fmt`: `str`

**Returns:** `i64`

*Defined at line 158*

---

## logtable_export

The table as bytes for the host: varint count, then per entry level,
varint length and the UTF-8 bytes of the format.

```fides
pub fn logtable_export(t: &LogTable) -> Vec<i64> {
```

**Parameters:**

- `t`: `&LogTable`

**Returns:** `Vec<i64>`

*Defined at line 176*

---

## logtable_import

```fides
pub fn logtable_import(bytes: Vec<i64>) -> LogTable {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `LogTable`

*Defined at line 190*

---

## LogEncoder

```fides
pub struct LogEncoder {
```

**Fields:**

| Name | Type |
|------|------|
| `out` | `Vec<i64>` |
| `last_ts` | `i64` |
| `records` | `i64` |
| `min_level` | `i64` |
| `dropped` | `i64` |

*Defined at line 214*

---

## logenc_new

```fides
pub fn logenc_new(min_level: i64) -> LogEncoder {
```

**Parameters:**

- `min_level`: `i64`

**Returns:** `LogEncoder`

*Defined at line 222*

---

## logenc_emit

Emit one record for format `idx` at time `ts`. Integer-like
arguments come from `ints`, {s} arguments from `strs`, each in order.

```fides
pub fn logenc_emit(e: &mut LogEncoder, t: &LogTable, idx: i64, ts: i64, ints: Vec<i64>, strs: Vec<str>) {
```

**Parameters:**

- `e`: `&mut LogEncoder`
- `t`: `&LogTable`
- `idx`: `i64`
- `ts`: `i64`
- `ints`: `Vec<i64>`
- `strs`: `Vec<str>`

*Defined at line 230*

---

## LogLine

One decoded record.

```fides
pub struct LogLine {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ts` | `i64` |
| `level` | `i64` |
| `text` | `str` |

*Defined at line 323*

---

## log_decode_one

Decode the next record from `r` (absolute timestamp kept in `ts`).

```fides
pub fn log_decode_one(t: &LogTable, r: &mut LogReader, ts: &mut i64) -> LogLine {
```

**Parameters:**

- `t`: `&LogTable`
- `r`: `&mut LogReader`
- `ts`: `&mut i64`

**Returns:** `LogLine`

*Defined at line 331*

---

## log_decode_all

Decode a whole stream into lines "[ts] LEVEL text"; decoding stops at
the first broken record (with an "!" line saying why).

```fides
pub fn log_decode_all(t: &LogTable, bytes: Vec<i64>) -> Vec<str> {
```

**Parameters:**

- `t`: `&LogTable`
- `bytes`: `Vec<i64>`

**Returns:** `Vec<str>`

*Defined at line 413*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
