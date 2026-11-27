[All modules](index.md)

# exi -- Fides Documentation

## Contents

- [EXI_DT_STRING:](#EXI_DT_STRING:) (const)
- [EXI_DT_BINARY:](#EXI_DT_BINARY:) (const)
- [EXI_DT_BOOLEAN:](#EXI_DT_BOOLEAN:) (const)
- [EXI_DT_UNSIGNED:](#EXI_DT_UNSIGNED:) (const)
- [EXI_DT_INTEGER:](#EXI_DT_INTEGER:) (const)
- [EXI_DT_NBIT:](#EXI_DT_NBIT:) (const)
- [EXI_DT_ENUM:](#EXI_DT_ENUM:) (const)
- [EXI_DT_LIST:](#EXI_DT_LIST:) (const)
- [EXI_SE:](#EXI_SE:) (const)
- [EXI_EE:](#EXI_EE:) (const)
- [EXI_AT:](#EXI_AT:) (const)
- [EXI_CH:](#EXI_CH:) (const)
- [EXI_XSI:](#EXI_XSI:) (const)
- [EXI_OPTIONS_NS:](#EXI_OPTIONS_NS:) (const)
- [EXI_BIT_PACKED:](#EXI_BIT_PACKED:) (const)
- [EXI_BYTE_ALIGNED:](#EXI_BYTE_ALIGNED:) (const)
- [EXI_PRE_COMPRESS:](#EXI_PRE_COMPRESS:) (const)
- [exi_nbits](#exi_nbits) (function)
- [ExiWriter](#ExiWriter) (struct)
- [exi_writer](#exi_writer) (function)
- [exi_w_raw](#exi_w_raw) (function)
- [exi_w_nbit](#exi_w_nbit) (function)
- [exi_w_align](#exi_w_align) (function)
- [exi_w_uint](#exi_w_uint) (function)
- [exi_w_bool](#exi_w_bool) (function)
- [exi_w_int](#exi_w_int) (function)
- [exi_w_binary](#exi_w_binary) (function)
- [exi_w_bytes](#exi_w_bytes) (function)
- [ExiReader](#ExiReader) (struct)
- [exi_reader](#exi_reader) (function)
- [exi_r_fail](#exi_r_fail) (function)
- [exi_r_raw](#exi_r_raw) (function)
- [exi_r_nbit](#exi_r_nbit) (function)
- [exi_r_align](#exi_r_align) (function)
- [exi_r_uint](#exi_r_uint) (function)
- [exi_r_bool](#exi_r_bool) (function)
- [exi_r_int](#exi_r_int) (function)
- [exi_r_binary](#exi_r_binary) (function)
- [exi_codepoints](#exi_codepoints) (function)
- [exi_from_codepoints](#exi_from_codepoints) (function)
- [exi_w_string](#exi_w_string) (function)
- [ExiStrings](#ExiStrings) (struct)
- [exi_strings](#exi_strings) (function)
- [exi_qid](#exi_qid) (function)
- [exi_w_value](#exi_w_value) (function)
- [exi_r_value](#exi_r_value) (function)
- [exi_w_qname](#exi_w_qname) (function)
- [exi_r_qname](#exi_r_qname) (function)
- [ExiSchema](#ExiSchema) (struct)
- [exi_schema_none](#exi_schema_none) (function)
- [exi_schema_new](#exi_schema_new) (function)
- [exi_dt](#exi_dt) (function)
- [exi_dt_enum](#exi_dt_enum) (function)
- [exi_dt_list](#exi_dt_list) (function)
- [exi_enum_name](#exi_enum_name) (function)
- [exi_enum_index](#exi_enum_index) (function)
- [exi_type](#exi_type) (function)
- [exi_type_simple](#exi_type_simple) (function)
- [exi_type_opaque](#exi_type_opaque) (function)
- [exi_attr](#exi_attr) (function)
- [exi_qname_less](#exi_qname_less) (function)
- [exi_particle](#exi_particle) (function)
- [exi_alt](#exi_alt) (function)
- [exi_alt_sorted](#exi_alt_sorted) (function)
- [exi_elem](#exi_elem) (function)
- [exi_nillable](#exi_nillable) (function)
- [exi_global](#exi_global) (function)
- [exi_global_at](#exi_global_at) (function)
- [exi_schema_finish](#exi_schema_finish) (function)
- [ExiEvent](#ExiEvent) (struct)
- [exi_ev](#exi_ev) (function)
- [exi_se](#exi_se) (function)
- [exi_ee](#exi_ee) (function)
- [exi_ch_str](#exi_ch_str) (function)
- [exi_ch_int](#exi_ch_int) (function)
- [exi_ch_bool](#exi_ch_bool) (function)
- [exi_ch_bin](#exi_ch_bin) (function)
- [exi_at_str](#exi_at_str) (function)
- [exi_at_int](#exi_at_int) (function)
- [exi_push_leaf](#exi_push_leaf) (function)
- [ExiOptions](#ExiOptions) (struct)
- [exi_options](#exi_options) (function)
- [exi_options_copy](#exi_options_copy) (function)
- [exi_options_unsupported](#exi_options_unsupported) (function)
- [exi_options_events](#exi_options_events) (function)
- [exi_options_from](#exi_options_from) (function)
- [ExiCtx](#ExiCtx) (struct)
- [ExiCands](#ExiCands) (struct)
- [exi_cands](#exi_cands) (function)
- [ExiResult](#ExiResult) (struct)
- [exi_encode](#exi_encode) (function)
- [ExiDoc](#ExiDoc) (struct)
- [exi_decode](#exi_decode) (function)
- [exi_find_nth](#exi_find_nth) (function)
- [exi_find](#exi_find) (function)
- [exi_count](#exi_count) (function)
- [exi_value_of](#exi_value_of) (function)
- [exi_get](#exi_get) (function)
- [exi_has](#exi_has) (function)
- [exi_attr_of](#exi_attr_of) (function)
- [exi_events_text](#exi_events_text) (function)

## EXI_DT_STRING:

```fides
pub const EXI_DT_STRING: i64 = 0;
```

*Defined at line 72*

---

## EXI_DT_BINARY:

```fides
pub const EXI_DT_BINARY: i64 = 1;  // base64Binary, hexBinary
```

*Defined at line 73*

---

## EXI_DT_BOOLEAN:

```fides
pub const EXI_DT_BOOLEAN: i64 = 2;
```

*Defined at line 74*

---

## EXI_DT_UNSIGNED:

```fides
pub const EXI_DT_UNSIGNED: i64 = 3;  // integers with a lower bound >= 0
```

*Defined at line 75*

---

## EXI_DT_INTEGER:

```fides
pub const EXI_DT_INTEGER: i64 = 4;  // other integers
```

*Defined at line 76*

---

## EXI_DT_NBIT:

```fides
pub const EXI_DT_NBIT: i64 = 5;  // bounded integers, range <= 4096
```

*Defined at line 77*

---

## EXI_DT_ENUM:

```fides
pub const EXI_DT_ENUM: i64 = 6;
```

*Defined at line 78*

---

## EXI_DT_LIST:

```fides
pub const EXI_DT_LIST: i64 = 7;
```

*Defined at line 79*

---

## EXI_SE:

```fides
pub const EXI_SE: i64 = 1;
```

*Defined at line 82*

---

## EXI_EE:

```fides
pub const EXI_EE: i64 = 2;
```

*Defined at line 83*

---

## EXI_AT:

```fides
pub const EXI_AT: i64 = 3;
```

*Defined at line 84*

---

## EXI_CH:

```fides
pub const EXI_CH: i64 = 4;
```

*Defined at line 85*

---

## EXI_XSI:

```fides
pub const EXI_XSI: str = "http://www.w3.org/2001/XMLSchema-instance";
```

*Defined at line 87*

---

## EXI_OPTIONS_NS:

```fides
pub const EXI_OPTIONS_NS: str = "http://www.w3.org/2009/exi";
```

*Defined at line 88*

---

## EXI_BIT_PACKED:

```fides
pub const EXI_BIT_PACKED: i64 = 0;
```

*Defined at line 91*

---

## EXI_BYTE_ALIGNED:

```fides
pub const EXI_BYTE_ALIGNED: i64 = 1;
```

*Defined at line 92*

---

## EXI_PRE_COMPRESS:

```fides
pub const EXI_PRE_COMPRESS: i64 = 2;
```

*Defined at line 93*

---

## exi_nbits

Bits needed to tell n values apart: ceil(log2(n)), 0 for n <= 1.

```fides
pub fn exi_nbits(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 96*

---

## ExiWriter

```fides
pub struct ExiWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `out` | `Vec<i64>` |
| `cur` | `i64` |
| `fill` | `i64` |
| `aligned` | `bool,  // byte-aligned body` |

*Defined at line 106*

---

## exi_writer

An empty bit-packed EXI writer.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(exi_nbits(5), 3);
assert_eq(exi_nbits(1), 0);
var w = exi_writer();
exi_w_nbit(&mut w, 5, 3);
exi_w_uint(&mut w, 300);
exi_w_bool(&mut w, true);
exi_w_int(&mut w, -7);
var r = exi_reader(exi_w_bytes(&w));
assert_eq(exi_r_nbit(&mut r, 3), 5);
assert_eq(exi_r_uint(&mut r), 300);
assert(exi_r_bool(&mut r));
assert_eq(exi_r_int(&mut r), -7);
```

```fides
pub fn exi_writer() -> ExiWriter {
```

**Returns:** `ExiWriter`

*Defined at line 128*

---

## exi_w_raw

n bits of v, most significant first, whatever the alignment.

```fides
pub fn exi_w_raw(w: &mut ExiWriter, v: i64, n: i64) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `v`: `i64`
- `n`: `i64`

*Defined at line 135*

---

## exi_w_nbit

An n-bit unsigned integer: n bits, or in a byte-aligned body the
fewest whole bytes, least significant first.

```fides
pub fn exi_w_nbit(w: &mut ExiWriter, v: i64, n: i64) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `v`: `i64`
- `n`: `i64`

*Defined at line 151*

---

## exi_w_align

Pad with zero bits to the next byte boundary.

```fides
pub fn exi_w_align(w: &mut ExiWriter) {
```

**Parameters:**

- `w`: `&mut ExiWriter`

*Defined at line 167*

---

## exi_w_uint

Unsigned Integer: 7 bits per octet, least significant group first, the
top bit set on every octet but the last.

```fides
pub fn exi_w_uint(w: &mut ExiWriter, v_in: i64) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `v_in`: `i64`

*Defined at line 175*

---

## exi_w_bool

```fides
pub fn exi_w_bool(w: &mut ExiWriter, b: bool) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `b`: `bool`

*Defined at line 189*

---

## exi_w_int

Integer: a sign bit, then |v| (positive) or |v| - 1 (negative).

```fides
pub fn exi_w_int(w: &mut ExiWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `v`: `i64`

*Defined at line 198*

---

## exi_w_binary

Binary: the length, then the bytes.

```fides
pub fn exi_w_binary(w: &mut ExiWriter, b: &Vec<i64>) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `b`: `&Vec<i64>`

*Defined at line 209*

---

## exi_w_bytes

The bytes written so far, the last one padded with zero bits.

```fides
pub fn exi_w_bytes(w: &ExiWriter) -> Vec<i64> {
```

**Parameters:**

- `w`: `&ExiWriter`

**Returns:** `Vec<i64>`

*Defined at line 219*

---

## ExiReader

```fides
pub struct ExiReader {
```

**Fields:**

| Name | Type |
|------|------|
| `b` | `Vec<i64>` |
| `pos` | `i64,  // in bits` |
| `ok` | `bool` |
| `error` | `str` |
| `aligned` | `bool` |

*Defined at line 237*

---

## exi_reader

```fides
pub fn exi_reader(b: Vec<i64>) -> ExiReader {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `ExiReader`

*Defined at line 245*

---

## exi_r_fail

```fides
pub fn exi_r_fail(r: &mut ExiReader, why: str) {
```

**Parameters:**

- `r`: `&mut ExiReader`
- `why`: `str`

*Defined at line 251*

---

## exi_r_raw

```fides
pub fn exi_r_raw(r: &mut ExiReader, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut ExiReader`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 258*

---

## exi_r_nbit

```fides
pub fn exi_r_nbit(r: &mut ExiReader, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut ExiReader`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 274*

---

## exi_r_align

```fides
pub fn exi_r_align(r: &mut ExiReader) {
```

**Parameters:**

- `r`: `&mut ExiReader`

*Defined at line 290*

---

## exi_r_uint

```fides
pub fn exi_r_uint(r: &mut ExiReader) -> i64 {
```

**Parameters:**

- `r`: `&mut ExiReader`

**Returns:** `i64`

*Defined at line 296*

---

## exi_r_bool

```fides
pub fn exi_r_bool(r: &mut ExiReader) -> bool {
```

**Parameters:**

- `r`: `&mut ExiReader`

**Returns:** `bool`

*Defined at line 317*

---

## exi_r_int

```fides
pub fn exi_r_int(r: &mut ExiReader) -> i64 {
```

**Parameters:**

- `r`: `&mut ExiReader`

**Returns:** `i64`

*Defined at line 321*

---

## exi_r_binary

```fides
pub fn exi_r_binary(r: &mut ExiReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut ExiReader`

**Returns:** `Vec<i64>`

*Defined at line 330*

---

## exi_codepoints

The Unicode code points of a UTF-8 string.

```fides
pub fn exi_codepoints(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 348*

---

## exi_from_codepoints

UTF-8 text of code points.

```fides
pub fn exi_from_codepoints(cps: &Vec<i64>) -> str {
```

**Parameters:**

- `cps`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 375*

---

## exi_w_string

A String literal: the number of code points, then each one.

```fides
pub fn exi_w_string(w: &mut ExiWriter, s: str) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `s`: `str`

*Defined at line 423*

---

## ExiStrings

The string table of one stream. QNames are numbered (qn_*) so that
local value partitions and built-in grammars can be keyed by number.

```fides
pub struct ExiStrings {
```

**Fields:**

| Name | Type |
|------|------|
| `uris` | `Vec<str>` |
| `ln_uri` | `Vec<i64>,  // local-name partitions, flattened` |
| `ln_name` | `Vec<str>` |
| `qn_uri` | `Vec<str>` |
| `qn_name` | `Vec<str>` |
| `gv` | `Vec<str>,  // global value partition` |
| `gv_key` | `Vec<i64>,  // the QName whose local partition holds it` |
| `gv_local` | `Vec<i64>,  // its id there` |
| `lv_key` | `Vec<i64>,  // local entries, in insertion order` |
| `lv_val` | `Vec<str>` |
| `lv_live` | `Vec<bool>` |
| `next_g` | `i64` |
| `max_len` | `i64,  // valueMaxLength, -1: unbounded` |
| `capacity` | `i64,  // valuePartitionCapacity, -1: unbounded` |

*Defined at line 433*

---

## exi_strings

```fides
pub fn exi_strings(max_len: i64, capacity: i64) -> ExiStrings {
```

**Parameters:**

- `max_len`: `i64`
- `capacity`: `i64`

**Returns:** `ExiStrings`

*Defined at line 450*

---

## exi_qid

The number of a QName (added when new).

```fides
pub fn exi_qid(t: &mut ExiStrings, uri: str, name: str) -> i64 {
```

**Parameters:**

- `t`: `&mut ExiStrings`
- `uri`: `str`
- `name`: `str`

**Returns:** `i64`

*Defined at line 473*

---

## exi_w_value

A string value of QName `key` (7.3.3): a local hit, a global hit or a
literal that is then added to the tables.

```fides
pub fn exi_w_value(w: &mut ExiWriter, t: &mut ExiStrings, key: i64, s: str) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `t`: `&mut ExiStrings`
- `key`: `i64`
- `s`: `str`

*Defined at line 575*

---

## exi_r_value

```fides
pub fn exi_r_value(r: &mut ExiReader, t: &mut ExiStrings, key: i64) -> str {
```

**Parameters:**

- `r`: `&mut ExiReader`
- `t`: `&mut ExiStrings`
- `key`: `i64`

**Returns:** `str`

*Defined at line 594*

---

## exi_w_qname

A QName (7.1.7): URI (n-bit, 0 = new) and local name (0 + id, or
length + 1 and the characters). Prefixes are not preserved.

```fides
pub fn exi_w_qname(w: &mut ExiWriter, t: &mut ExiStrings, uri: str, name: str) {
```

**Parameters:**

- `w`: `&mut ExiWriter`
- `t`: `&mut ExiStrings`
- `uri`: `str`
- `name`: `str`

*Defined at line 627*

---

## exi_r_qname

Read a QName; returns its number (exi_qid).

```fides
pub fn exi_r_qname(r: &mut ExiReader, t: &mut ExiStrings) -> i64 {
```

**Parameters:**

- `r`: `&mut ExiReader`
- `t`: `&mut ExiStrings`

**Returns:** `i64`

*Defined at line 669*

---

## ExiSchema

Schema-informed grammar tables. Build with exi_dt*, exi_type*,
exi_attr, exi_elem / exi_particle + exi_alt, exi_global*, then call
exi_schema_finish. An ExiSchema from exi_schema_none() (no globals)
selects the built-in grammars.

```fides
pub struct ExiSchema {
```

**Fields:**

| Name | Type |
|------|------|
| `informed` | `bool` |
| `dt_kind` | `Vec<i64>` |
| `dt_lo` | `Vec<i64>` |
| `dt_hi` | `Vec<i64>` |
| `dt_first` | `Vec<i64>,  // enumeration names: enames[first .. first+count]` |
| `dt_count` | `Vec<i64>` |
| `dt_item` | `Vec<i64>,  // list item datatype` |
| `enames` | `Vec<str>` |
| `ty_dt` | `Vec<i64>,  // simple content datatype; -1: element content` |
| `ty_ok` | `Vec<bool>,  // false: declared without a grammar here` |
| `ty_xsitype` | `Vec<bool>,  // has named sub-types (strict: AT(xsi:type))` |
| `ty_attrs` | `Vec<Vec<i64>>` |
| `ty_parts` | `Vec<Vec<i64>>` |
| `at_uri` | `Vec<str>` |
| `at_name` | `Vec<str>` |
| `at_dt` | `Vec<i64>` |
| `at_req` | `Vec<bool>` |
| `pa_min` | `Vec<i64>` |
| `pa_max` | `Vec<i64>,  // -1: unbounded` |
| `pa_alts` | `Vec<Vec<i64>>` |
| `el_uri` | `Vec<str>` |
| `el_name` | `Vec<str>,  // "*": wildcard` |
| `el_type` | `Vec<i64>` |
| `el_nil` | `Vec<bool>` |
| `globals` | `Vec<i64>,  // element declarations` |
| `gpos` | `Vec<i64>,  // their position in the document grammar` |
| `nglobals` | `i64,  // number of global elements of the schema` |

*Defined at line 723*

---

## exi_schema_none

```fides
pub fn exi_schema_none() -> ExiSchema {
```

**Returns:** `ExiSchema`

*Defined at line 753*

---

## exi_schema_new

```fides
pub fn exi_schema_new() -> ExiSchema {
```

**Returns:** `ExiSchema`

*Defined at line 759*

---

## exi_dt

A datatype: kind EXI_DT_*, with [lo, hi] for EXI_DT_NBIT.

```fides
pub fn exi_dt(s: &mut ExiSchema, kind: i64, lo: i64, hi: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `kind`: `i64`
- `lo`: `i64`
- `hi`: `i64`

**Returns:** `i64`

*Defined at line 766*

---

## exi_dt_enum

An enumeration of the values in `names`, separated by "|", in schema
order.

```fides
pub fn exi_dt_enum(s: &mut ExiSchema, names: str) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `names`: `str`

**Returns:** `i64`

*Defined at line 778*

---

## exi_dt_list

A list of `item` values.

```fides
pub fn exi_dt_list(s: &mut ExiSchema, item: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `item`: `i64`

**Returns:** `i64`

*Defined at line 792*

---

## exi_enum_name

The name of enumeration value `i` of datatype `d`.

```fides
pub fn exi_enum_name(s: &ExiSchema, d: i64, i: i64) -> str {
```

**Parameters:**

- `s`: `&ExiSchema`
- `d`: `i64`
- `i`: `i64`

**Returns:** `str`

*Defined at line 799*

---

## exi_enum_index

```fides
pub fn exi_enum_index(s: &ExiSchema, d: i64, name: str) -> i64 {
```

**Parameters:**

- `s`: `&ExiSchema`
- `d`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 806*

---

## exi_type

A complex type with element content (particles added with exi_elem /
exi_particle) and attributes (exi_attr).

```fides
pub fn exi_type(s: &mut ExiSchema) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`

**Returns:** `i64`

*Defined at line 828*

---

## exi_type_simple

A type with simple content of datatype `dt` (an element of a simple
type, or a complex type with simple content and attributes).

```fides
pub fn exi_type_simple(s: &mut ExiSchema, dt: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `dt`: `i64`

**Returns:** `i64`

*Defined at line 834*

---

## exi_type_opaque

A type named by the schema but without a grammar here: an element of
this type is refused by the encoder and the decoder.

```fides
pub fn exi_type_opaque(s: &mut ExiSchema) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`

**Returns:** `i64`

*Defined at line 840*

---

## exi_attr

An attribute use of type `t`; kept sorted by local name, then URI.

```fides
pub fn exi_attr(s: &mut ExiSchema, t: i64, uri: str, name: str, dt: i64, required: bool) {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `t`: `i64`
- `uri`: `str`
- `name`: `str`
- `dt`: `i64`
- `required`: `bool`

*Defined at line 847*

---

## exi_qname_less

(local name, URI) order of QNames.

```fides
pub fn exi_qname_less(n1: str, u1: str, n2: str, u2: str) -> bool {
```

**Parameters:**

- `n1`: `str`
- `u1`: `str`
- `n2`: `str`
- `u2`: `str`

**Returns:** `bool`

*Defined at line 873*

---

## exi_particle

A particle of type `t` (in schema order); its terms are added with
exi_alt. max -1 is unbounded.

```fides
pub fn exi_particle(s: &mut ExiSchema, t: i64, min: i64, max: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `t`: `i64`
- `min`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 883*

---

## exi_alt

Add element `name` of type `et` as a term of particle `p` (choices keep
the order of the calls; see exi_alt_sorted for substitution groups).

```fides
pub fn exi_alt(s: &mut ExiSchema, p: i64, uri: str, name: str, et: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `p`: `i64`
- `uri`: `str`
- `name`: `str`
- `et`: `i64`

**Returns:** `i64`

*Defined at line 896*

---

## exi_alt_sorted

Add a member of a substitution group: the terms stay sorted by local
name, then URI (8.5.4.1.6).

```fides
pub fn exi_alt_sorted(s: &mut ExiSchema, p: i64, uri: str, name: str, et: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `p`: `i64`
- `uri`: `str`
- `name`: `str`
- `et`: `i64`

**Returns:** `i64`

*Defined at line 910*

---

## exi_elem

A particle holding one element.

```fides
pub fn exi_elem(s: &mut ExiSchema, t: i64, uri: str, name: str, et: i64, min: i64, max: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `t`: `i64`
- `uri`: `str`
- `name`: `str`
- `et`: `i64`
- `min`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 933*

---

## exi_nillable

Mark element declaration `e` nillable.

```fides
pub fn exi_nillable(s: &mut ExiSchema, e: i64) {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `e`: `i64`

*Defined at line 939*

---

## exi_global

A global element; positions are assigned by exi_schema_finish in
(local name, URI) order.

```fides
pub fn exi_global(s: &mut ExiSchema, uri: str, name: str, et: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `uri`: `str`
- `name`: `str`
- `et`: `i64`

**Returns:** `i64`

*Defined at line 945*

---

## exi_global_at

A global element at a fixed position `pos` of a document grammar with
`total` global elements (for schemas whose other globals have no
grammar here).

```fides
pub fn exi_global_at(s: &mut ExiSchema, pos: i64, total: i64, uri: str, name: str, et: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut ExiSchema`
- `pos`: `i64`
- `total`: `i64`
- `uri`: `str`
- `name`: `str`
- `et`: `i64`

**Returns:** `i64`

*Defined at line 959*

---

## exi_schema_finish

```fides
pub fn exi_schema_finish(s: &mut ExiSchema) {
```

**Parameters:**

- `s`: `&mut ExiSchema`

*Defined at line 966*

---

## ExiEvent

One XML event. Values (AT, CH): `sval` for strings and enumeration
names, `ival` for integers, booleans (0/1) and enumeration indexes,
`bval` for binary values and lists of integers (a list of strings is
`sval` with the items separated by spaces).

```fides
pub struct ExiEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `uri` | `str` |
| `name` | `str` |
| `ival` | `i64` |
| `sval` | `str` |
| `bval` | `Vec<i64>` |

*Defined at line 1005*

---

## exi_ev

```fides
pub fn exi_ev(kind: i64, uri: str, name: str) -> ExiEvent {
```

**Parameters:**

- `kind`: `i64`
- `uri`: `str`
- `name`: `str`

**Returns:** `ExiEvent`

*Defined at line 1014*

---

## exi_se

```fides
pub fn exi_se(uri: str, name: str) -> ExiEvent {
```

**Parameters:**

- `uri`: `str`
- `name`: `str`

**Returns:** `ExiEvent`

*Defined at line 1020*

---

## exi_ee

```fides
pub fn exi_ee() -> ExiEvent {
```

**Returns:** `ExiEvent`

*Defined at line 1024*

---

## exi_ch_str

```fides
pub fn exi_ch_str(v: str) -> ExiEvent {
```

**Parameters:**

- `v`: `str`

**Returns:** `ExiEvent`

*Defined at line 1028*

---

## exi_ch_int

```fides
pub fn exi_ch_int(v: i64) -> ExiEvent {
```

**Parameters:**

- `v`: `i64`

**Returns:** `ExiEvent`

*Defined at line 1034*

---

## exi_ch_bool

```fides
pub fn exi_ch_bool(v: bool) -> ExiEvent {
```

**Parameters:**

- `v`: `bool`

**Returns:** `ExiEvent`

*Defined at line 1040*

---

## exi_ch_bin

```fides
pub fn exi_ch_bin(v: Vec<i64>) -> ExiEvent {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `ExiEvent`

*Defined at line 1048*

---

## exi_at_str

```fides
pub fn exi_at_str(uri: str, name: str, v: str) -> ExiEvent {
```

**Parameters:**

- `uri`: `str`
- `name`: `str`
- `v`: `str`

**Returns:** `ExiEvent`

*Defined at line 1054*

---

## exi_at_int

```fides
pub fn exi_at_int(uri: str, name: str, v: i64) -> ExiEvent {
```

**Parameters:**

- `uri`: `str`
- `name`: `str`
- `v`: `i64`

**Returns:** `ExiEvent`

*Defined at line 1060*

---

## exi_push_leaf

Push SE(name), CH(value), EE: a simple element.

```fides
pub fn exi_push_leaf(evs: &mut Vec<ExiEvent>, uri: str, name: str, value: ExiEvent) {
```

**Parameters:**

- `evs`: `&mut Vec<ExiEvent>`
- `uri`: `str`
- `name`: `str`
- `value`: `ExiEvent`

*Defined at line 1067*

---

## ExiOptions

```fides
pub struct ExiOptions {
```

**Fields:**

| Name | Type |
|------|------|
| `alignment` | `i64,  // EXI_BIT_PACKED, EXI_BYTE_ALIGNED, EXI_PRE_COMPRESS` |
| `compression` | `bool` |
| `strict` | `bool` |
| `fragment` | `bool` |
| `self_contained` | `bool` |
| `value_max_length` | `i64,  // -1: unbounded` |
| `value_capacity` | `i64,  // -1: unbounded` |
| `block_size` | `i64` |
| `schema_id` | `str` |
| `has_schema_id` | `bool` |
| `schema_id_nil` | `bool` |
| `preserve_dtd` | `bool` |
| `preserve_prefixes` | `bool` |
| `preserve_lexical` | `bool` |
| `preserve_comments` | `bool` |
| `preserve_pis` | `bool` |
| `cookie` | `bool,  // write the "$EXI" cookie` |
| `in_header` | `bool,  // write the options into the header` |

*Defined at line 1075*

---

## exi_options

The default options (5.4), not written into the header.

```fides
pub fn exi_options() -> ExiOptions {
```

**Returns:** `ExiOptions`

*Defined at line 1097*

---

## exi_options_copy

A copy of `o`.

```fides
pub fn exi_options_copy(o: &ExiOptions) -> ExiOptions {
```

**Parameters:**

- `o`: `&ExiOptions`

**Returns:** `ExiOptions`

*Defined at line 1104*

---

## exi_options_unsupported

Why these options cannot be used for a body here ("" when they can).

```fides
pub fn exi_options_unsupported(o: &ExiOptions) -> str {
```

**Parameters:**

- `o`: `&ExiOptions`

**Returns:** `str`

*Defined at line 1111*

---

## exi_options_events

The events of the Options document of `o` (only non-default values).

```fides
pub fn exi_options_events(o: &ExiOptions) -> Vec<ExiEvent> {
```

**Parameters:**

- `o`: `&ExiOptions`

**Returns:** `Vec<ExiEvent>`

*Defined at line 1179*

---

## exi_options_from

Options from the events of an Options document.

```fides
pub fn exi_options_from(evs: &Vec<ExiEvent>) -> ExiOptions {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`

**Returns:** `ExiOptions`

*Defined at line 1261*

---

## ExiCtx

Encoder/decoder state: the bit stream, string table, element stack
and built-in grammars of one EXI stream.

```fides
pub struct ExiCtx {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `ExiWriter` |
| `r` | `ExiReader` |
| `st` | `ExiStrings` |
| `strict` | `bool` |
| `ok` | `bool` |
| `error` | `str` |
| `f_type` | `Vec<i64>,  // schema type, -1 for a built-in grammar` |
| `f_elem` | `Vec<i64>,  // element declaration (schema)` |
| `f_ph` | `Vec<i64>,  // 0 start tag, 1 content, 2 after CH, 3 nil` |
| `f_ai` | `Vec<i64>,  // next attribute` |
| `f_j` | `Vec<i64>,  // particle` |
| `f_c` | `Vec<i64>,  // occurrences of that particle so far` |
| `f_fresh` | `Vec<bool>,  // no event yet in this element` |
| `f_key` | `Vec<i64>,  // element QName number` |
| `f_bg` | `Vec<i64>,  // built-in grammar` |
| `f_bst` | `Vec<i64>,  // 0 StartTagContent, 1 ElementContent` |
| `bg_key` | `Vec<i64>` |
| `bg_st_k` | `Vec<Vec<i64>>,  // kinds EXI_SE / EXI_AT / EXI_EE / EXI_CH` |
| `bg_st_q` | `Vec<Vec<i64>>,  // QName numbers` |
| `bg_ec_k` | `Vec<Vec<i64>>` |
| `bg_ec_q` | `Vec<Vec<i64>>` |
| `out` | `Vec<ExiEvent>` |

*Defined at line 1332*

---

## ExiCands

The productions of a schema-informed state, in event-code order.

```fides
pub struct ExiCands {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>,  // EXI_AT, EXI_SE, EXI_EE, EXI_CH; 5 AT(xsi:type), 6 AT(xsi:nil)` |
| `a` | `Vec<i64>,  // AT: attribute position; SE: particle position` |
| `b` | `Vec<i64>,  // SE: term position` |
| `n` | `Vec<i64>,  // SE: occurrences after this one` |

*Defined at line 1399*

---

## exi_cands

The productions of the top element's current state.

```fides
pub fn exi_cands(s: &ExiSchema, c: &ExiCtx) -> ExiCands {
```

**Parameters:**

- `s`: `&ExiSchema`
- `c`: `&ExiCtx`

**Returns:** `ExiCands`

*Defined at line 1414*

---

## ExiResult

```fides
pub struct ExiResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `bytes` | `Vec<i64>` |

*Defined at line 1904*

---

## exi_encode

Encode a document (or fragment, with o.fragment) of events.

```fides
pub fn exi_encode(s: &ExiSchema, o: &ExiOptions, evs: &Vec<ExiEvent>) -> ExiResult {
```

**Parameters:**

- `s`: `&ExiSchema`
- `o`: `&ExiOptions`
- `evs`: `&Vec<ExiEvent>`

**Returns:** `ExiResult`

*Defined at line 2080*

---

## ExiDoc

```fides
pub struct ExiDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `events` | `Vec<ExiEvent>` |
| `options` | `ExiOptions` |
| `bits` | `i64,  // bits consumed, header included` |

*Defined at line 2105*

---

## exi_decode

Decode an EXI stream; `o` gives the options when the header has none.

```fides
pub fn exi_decode(s: &ExiSchema, o: &ExiOptions, b: Vec<i64>) -> ExiDoc {
```

**Parameters:**

- `s`: `&ExiSchema`
- `o`: `&ExiOptions`
- `b`: `Vec<i64>`

**Returns:** `ExiDoc`

*Defined at line 2270*

---

## exi_find_nth

Index of the SE event of the `nth` (from 0) element at `path` (local
names from the root, separated by "/"); -1 if there is none.

```fides
pub fn exi_find_nth(evs: &Vec<ExiEvent>, path: str, nth: i64) -> i64 {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `path`: `str`
- `nth`: `i64`

**Returns:** `i64`

*Defined at line 2297*

---

## exi_find

```fides
pub fn exi_find(evs: &Vec<ExiEvent>, path: str) -> i64 {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `path`: `str`

**Returns:** `i64`

*Defined at line 2319*

---

## exi_count

```fides
pub fn exi_count(evs: &Vec<ExiEvent>, path: str) -> i64 {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `path`: `str`

**Returns:** `i64`

*Defined at line 2323*

---

## exi_value_of

The CH event of the element whose SE is at `se` (an empty CH if none).

```fides
pub fn exi_value_of(evs: &Vec<ExiEvent>, se: i64) -> ExiEvent {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `se`: `i64`

**Returns:** `ExiEvent`

*Defined at line 2332*

---

## exi_get

```fides
pub fn exi_get(evs: &Vec<ExiEvent>, path: str) -> ExiEvent {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `path`: `str`

**Returns:** `ExiEvent`

*Defined at line 2343*

---

## exi_has

```fides
pub fn exi_has(evs: &Vec<ExiEvent>, path: str) -> bool {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `path`: `str`

**Returns:** `bool`

*Defined at line 2347*

---

## exi_attr_of

The value of attribute `name` of the element whose SE is at `se`.

```fides
pub fn exi_attr_of(evs: &Vec<ExiEvent>, se: i64, name: str) -> ExiEvent {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`
- `se`: `i64`
- `name`: `str`

**Returns:** `ExiEvent`

*Defined at line 2352*

---

## exi_events_text

A readable line per event (indented by depth), for logs and tests.

```fides
pub fn exi_events_text(evs: &Vec<ExiEvent>) -> str {
```

**Parameters:**

- `evs`: `&Vec<ExiEvent>`

**Returns:** `str`

*Defined at line 2364*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
