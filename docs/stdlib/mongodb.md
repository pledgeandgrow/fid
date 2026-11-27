[All modules](index.md)

# mongodb -- Fides Documentation

## Contents

- [BSON_DOUBLE:](#BSON_DOUBLE:) (const)
- [BSON_STRING:](#BSON_STRING:) (const)
- [BSON_DOCUMENT:](#BSON_DOCUMENT:) (const)
- [BSON_ARRAY:](#BSON_ARRAY:) (const)
- [BSON_BINARY:](#BSON_BINARY:) (const)
- [BSON_UNDEFINED:](#BSON_UNDEFINED:) (const)
- [BSON_OBJECTID:](#BSON_OBJECTID:) (const)
- [BSON_BOOL:](#BSON_BOOL:) (const)
- [BSON_DATETIME:](#BSON_DATETIME:) (const)
- [BSON_NULL:](#BSON_NULL:) (const)
- [BSON_REGEX:](#BSON_REGEX:) (const)
- [BSON_JS:](#BSON_JS:) (const)
- [BSON_INT32:](#BSON_INT32:) (const)
- [BSON_TIMESTAMP:](#BSON_TIMESTAMP:) (const)
- [BSON_INT64:](#BSON_INT64:) (const)
- [BSON_DECIMAL128:](#BSON_DECIMAL128:) (const)
- [BSON_MINKEY:](#BSON_MINKEY:) (const)
- [BSON_MAXKEY:](#BSON_MAXKEY:) (const)
- [BSON_MAX_DEPTH:](#BSON_MAX_DEPTH:) (const)
- [MONGO_OP_MSG:](#MONGO_OP_MSG:) (const)
- [BsonWriter](#BsonWriter) (struct)
- [bson_writer](#bson_writer) (function)
- [bson_put_double](#bson_put_double) (function)
- [bson_put_string](#bson_put_string) (function)
- [bson_put_int32](#bson_put_int32) (function)
- [bson_put_int64](#bson_put_int64) (function)
- [bson_put_bool](#bson_put_bool) (function)
- [bson_put_null](#bson_put_null) (function)
- [bson_put_datetime](#bson_put_datetime) (function)
- [bson_put_timestamp](#bson_put_timestamp) (function)
- [bson_put_objectid](#bson_put_objectid) (function)
- [bson_put_binary](#bson_put_binary) (function)
- [bson_put_regex](#bson_put_regex) (function)
- [bson_put_decimal128](#bson_put_decimal128) (function)
- [bson_put_minkey](#bson_put_minkey) (function)
- [bson_put_maxkey](#bson_put_maxkey) (function)
- [bson_put_document](#bson_put_document) (function)
- [bson_put_array_doc](#bson_put_array_doc) (function)
- [bson_begin_doc](#bson_begin_doc) (function)
- [bson_begin_array](#bson_begin_array) (function)
- [bson_end](#bson_end) (function)
- [bson_finish](#bson_finish) (function)
- [bson_empty](#bson_empty) (function)
- [bson_objectid](#bson_objectid) (function)
- [bson_objectid_hex](#bson_objectid_hex) (function)
- [bson_objectid_from_hex](#bson_objectid_from_hex) (function)
- [BsonDoc](#BsonDoc) (struct)
- [bson_decode_at](#bson_decode_at) (function)
- [bson_decode](#bson_decode) (function)
- [bson_doc_len](#bson_doc_len) (function)
- [bson_kind](#bson_kind) (function)
- [bson_key](#bson_key) (function)
- [bson_count](#bson_count) (function)
- [bson_child](#bson_child) (function)
- [bson_get](#bson_get) (function)
- [bson_path](#bson_path) (function)
- [bson_string](#bson_string) (function)
- [bson_int](#bson_int) (function)
- [bson_double](#bson_double) (function)
- [bson_bytes](#bson_bytes) (function)
- [bson_to_json](#bson_to_json) (function)
- [bson_encode_node](#bson_encode_node) (function)
- [bson_put_node](#bson_put_node) (function)
- [mongo_crc32c](#mongo_crc32c) (function)
- [mongo_op_msg](#mongo_op_msg) (function)
- [MongoMessage](#MongoMessage) (struct)
- [mongo_read_message](#mongo_read_message) (function)
- [mongo_cmd_find](#mongo_cmd_find) (function)
- [mongo_cmd_insert](#mongo_cmd_insert) (function)
- [mongo_update_stmt](#mongo_update_stmt) (function)
- [mongo_delete_stmt](#mongo_delete_stmt) (function)
- [mongo_cmd_aggregate](#mongo_cmd_aggregate) (function)
- [mongo_cmd_get_more](#mongo_cmd_get_more) (function)
- [mongo_cmd_kill_cursors](#mongo_cmd_kill_cursors) (function)
- [mongo_cmd_simple](#mongo_cmd_simple) (function)
- [MongoTransport](#MongoTransport) (struct)
- [mongo_tcp_transport](#mongo_tcp_transport) (function)
- [MongoClient](#MongoClient) (struct)
- [mongo_client_new](#mongo_client_new) (function)
- [MongoReply](#MongoReply) (struct)
- [mongo_run](#mongo_run) (function)
- [mongo_command](#mongo_command) (function)
- [mongo_ping](#mongo_ping) (function)
- [mongo_insert](#mongo_insert) (function)
- [mongo_update](#mongo_update) (function)
- [mongo_delete](#mongo_delete) (function)
- [MongoCursor](#MongoCursor) (struct)
- [mongo_find](#mongo_find) (function)
- [mongo_aggregate](#mongo_aggregate) (function)
- [mongo_cursor_more](#mongo_cursor_more) (function)
- [mongo_cursor_next](#mongo_cursor_next) (function)
- [mongo_cursor_close](#mongo_cursor_close) (function)
- [MongoHello](#MongoHello) (struct)
- [mongo_cmd_hello](#mongo_cmd_hello) (function)
- [mongo_parse_hello](#mongo_parse_hello) (function)
- [mongo_hello](#mongo_hello) (function)
- [mongo_pick_mechanism](#mongo_pick_mechanism) (function)
- [MongoScram](#MongoScram) (struct)
- [MONGO_SCRAM_MIN_ITERATIONS:](#MONGO_SCRAM_MIN_ITERATIONS:) (const)
- [mongo_scram_escape](#mongo_scram_escape) (function)
- [mongo_password_digest](#mongo_password_digest) (function)
- [mongo_saslprep](#mongo_saslprep) (function)
- [mongo_scram_nonce](#mongo_scram_nonce) (function)
- [mongo_scram_new](#mongo_scram_new) (function)
- [mongo_scram_client_first](#mongo_scram_client_first) (function)
- [mongo_scram_cache_size](#mongo_scram_cache_size) (function)
- [mongo_scram_client_final](#mongo_scram_client_final) (function)
- [mongo_scram_check_final](#mongo_scram_check_final) (function)
- [mongo_cmd_sasl_start](#mongo_cmd_sasl_start) (function)
- [mongo_cmd_sasl_continue](#mongo_cmd_sasl_continue) (function)
- [mongo_auth_scram](#mongo_auth_scram) (function)
- [mongo_login](#mongo_login) (function)

## BSON_DOUBLE:

```fides
pub const BSON_DOUBLE: i64 = 1;
```

*Defined at line 68*

---

## BSON_STRING:

```fides
pub const BSON_STRING: i64 = 2;
```

*Defined at line 69*

---

## BSON_DOCUMENT:

```fides
pub const BSON_DOCUMENT: i64 = 3;
```

*Defined at line 70*

---

## BSON_ARRAY:

```fides
pub const BSON_ARRAY: i64 = 4;
```

*Defined at line 71*

---

## BSON_BINARY:

```fides
pub const BSON_BINARY: i64 = 5;
```

*Defined at line 72*

---

## BSON_UNDEFINED:

```fides
pub const BSON_UNDEFINED: i64 = 6;
```

*Defined at line 73*

---

## BSON_OBJECTID:

```fides
pub const BSON_OBJECTID: i64 = 7;
```

*Defined at line 74*

---

## BSON_BOOL:

```fides
pub const BSON_BOOL: i64 = 8;
```

*Defined at line 75*

---

## BSON_DATETIME:

```fides
pub const BSON_DATETIME: i64 = 9;
```

*Defined at line 76*

---

## BSON_NULL:

```fides
pub const BSON_NULL: i64 = 10;
```

*Defined at line 77*

---

## BSON_REGEX:

```fides
pub const BSON_REGEX: i64 = 11;
```

*Defined at line 78*

---

## BSON_JS:

```fides
pub const BSON_JS: i64 = 13;
```

*Defined at line 79*

---

## BSON_INT32:

```fides
pub const BSON_INT32: i64 = 16;
```

*Defined at line 80*

---

## BSON_TIMESTAMP:

```fides
pub const BSON_TIMESTAMP: i64 = 17;
```

*Defined at line 81*

---

## BSON_INT64:

```fides
pub const BSON_INT64: i64 = 18;
```

*Defined at line 82*

---

## BSON_DECIMAL128:

```fides
pub const BSON_DECIMAL128: i64 = 19;
```

*Defined at line 83*

---

## BSON_MINKEY:

```fides
pub const BSON_MINKEY: i64 = 255;
```

*Defined at line 84*

---

## BSON_MAXKEY:

```fides
pub const BSON_MAXKEY: i64 = 127;
```

*Defined at line 85*

---

## BSON_MAX_DEPTH:

```fides
pub const BSON_MAX_DEPTH: i64 = 100;
```

*Defined at line 87*

---

## MONGO_OP_MSG:

```fides
pub const MONGO_OP_MSG: i64 = 2013;
```

*Defined at line 88*

---

## BsonWriter

Builds one document. Nested documents and arrays are opened with
bson_begin_doc / bson_begin_array and closed with bson_end; inside an
array the keys "0", "1", ... are given for you (pass "").

```fides
pub struct BsonWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `out` | `Vec<i64>` |
| `starts` | `Vec<i64>` |
| `is_array` | `Vec<bool>` |
| `counts` | `Vec<i64>` |

*Defined at line 95*

---

## bson_writer

```fides
pub fn bson_writer() -> BsonWriter {
```

**Returns:** `BsonWriter`

*Defined at line 102*

---

## bson_put_double

```fides
pub fn bson_put_double(w: &mut BsonWriter, key: str, v: f64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `v`: `f64`

*Defined at line 164*

---

## bson_put_string

A UTF-8 string (int32 length including the NUL, bytes, NUL).

```fides
pub fn bson_put_string(w: &mut BsonWriter, key: str, v: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `v`: `str`

*Defined at line 170*

---

## bson_put_int32

```fides
pub fn bson_put_int32(w: &mut BsonWriter, key: str, v: i64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `v`: `i64`

*Defined at line 178*

---

## bson_put_int64

```fides
pub fn bson_put_int64(w: &mut BsonWriter, key: str, v: i64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `v`: `i64`

*Defined at line 183*

---

## bson_put_bool

```fides
pub fn bson_put_bool(w: &mut BsonWriter, key: str, v: bool) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `v`: `bool`

*Defined at line 188*

---

## bson_put_null

```fides
pub fn bson_put_null(w: &mut BsonWriter, key: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`

*Defined at line 197*

---

## bson_put_datetime

UTC datetime: milliseconds since the Unix epoch.

```fides
pub fn bson_put_datetime(w: &mut BsonWriter, key: str, ms: i64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `ms`: `i64`

*Defined at line 202*

---

## bson_put_timestamp

Timestamp: increment (low 32 bits) and seconds (high 32 bits).

```fides
pub fn bson_put_timestamp(w: &mut BsonWriter, key: str, seconds: i64, increment: i64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `seconds`: `i64`
- `increment`: `i64`

*Defined at line 208*

---

## bson_put_objectid

An ObjectId from its 12 bytes.

```fides
pub fn bson_put_objectid(w: &mut BsonWriter, key: str, id: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `id`: `Vec<i64>`

*Defined at line 215*

---

## bson_put_binary

Binary data with a subtype (0 generic, 4 UUID, ...).

```fides
pub fn bson_put_binary(w: &mut BsonWriter, key: str, subtype: i64, data: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `subtype`: `i64`
- `data`: `Vec<i64>`

*Defined at line 229*

---

## bson_put_regex

A regular expression; options sorted as the spec asks ("imsx").

```fides
pub fn bson_put_regex(w: &mut BsonWriter, key: str, pattern: str, options: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `pattern`: `str`
- `options`: `str`

*Defined at line 237*

---

## bson_put_decimal128

Decimal128 from its 16 bytes (little-endian, as on the wire).

```fides
pub fn bson_put_decimal128(w: &mut BsonWriter, key: str, bytes16: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `bytes16`: `Vec<i64>`

*Defined at line 254*

---

## bson_put_minkey

```fides
pub fn bson_put_minkey(w: &mut BsonWriter, key: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`

*Defined at line 259*

---

## bson_put_maxkey

```fides
pub fn bson_put_maxkey(w: &mut BsonWriter, key: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`

*Defined at line 263*

---

## bson_put_document

A whole encoded document as an element.

```fides
pub fn bson_put_document(w: &mut BsonWriter, key: str, doc: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `doc`: `Vec<i64>`

*Defined at line 268*

---

## bson_put_array_doc

A whole encoded document as an array element (its keys must be 0, 1..).

```fides
pub fn bson_put_array_doc(w: &mut BsonWriter, key: str, doc: Vec<i64>) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `doc`: `Vec<i64>`

*Defined at line 274*

---

## bson_begin_doc

```fides
pub fn bson_begin_doc(w: &mut BsonWriter, key: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`

*Defined at line 287*

---

## bson_begin_array

```fides
pub fn bson_begin_array(w: &mut BsonWriter, key: str) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`

*Defined at line 291*

---

## bson_end

Close the innermost open document or array.

```fides
pub fn bson_end(w: &mut BsonWriter) {
```

**Parameters:**

- `w`: `&mut BsonWriter`

*Defined at line 296*

---

## bson_finish

Close everything and give the document's bytes.

```fides
pub fn bson_finish(w: &mut BsonWriter) -> Vec<i64> {
```

**Parameters:**

- `w`: `&mut BsonWriter`

**Returns:** `Vec<i64>`

*Defined at line 308*

---

## bson_empty

The empty document {}.

```fides
pub fn bson_empty() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 319*

---

## bson_objectid

An ObjectId: `seconds` big-endian, 5 bytes from `random5`, a 3-byte
big-endian `counter`.

```fides
pub fn bson_objectid(seconds: i64, random5: i64, counter: i64) -> Vec<i64> {
```

**Parameters:**

- `seconds`: `i64`
- `random5`: `i64`
- `counter`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 328*

---

## bson_objectid_hex

```fides
pub fn bson_objectid_hex(id: Vec<i64>) -> str {
```

**Parameters:**

- `id`: `Vec<i64>`

**Returns:** `str`

*Defined at line 360*

---

## bson_objectid_from_hex

The 12 bytes of a 24-digit hex ObjectId (empty when not one).

```fides
pub fn bson_objectid_from_hex(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 378*

---

## BsonDoc

A decoded document as a flat tree. Node i: kind (BSON_*; the root is
BSON_DOCUMENT), key, and by kind: ival (int32, int64, bool 0/1,
datetime ms, timestamp seconds with the increment in ival2, binary
subtype), fval (double), sval (string, regex pattern, JS code), sval2
(regex options), bytes (binary data, ObjectId, decimal128), children
(documents and arrays).

```fides
pub struct BsonDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>` |
| `key` | `Vec<str>` |
| `ival` | `Vec<i64>` |
| `ival2` | `Vec<i64>` |
| `fval` | `Vec<f64>` |
| `sval` | `Vec<str>` |
| `sval2` | `Vec<str>` |
| `bytes` | `Vec<Vec<i64>>` |
| `first` | `Vec<i64>` |
| `nkids` | `Vec<i64>` |
| `kids` | `Vec<i64>` |
| `root` | `i64` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 405*

---

## bson_decode_at

Decode one document from `b` at `at`; `used` (by bson_doc_len) is its
length.

```fides
pub fn bson_decode_at(b: Vec<i64>, at: i64) -> BsonDoc {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `BsonDoc`

*Defined at line 611*

---

## bson_decode

```fides
pub fn bson_decode(b: Vec<i64>) -> BsonDoc {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `BsonDoc`

*Defined at line 624*

---

## bson_doc_len

The int32 length of the document at `at` (0 when too short).

```fides
pub fn bson_doc_len(b: Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 636*

---

## bson_kind

```fides
pub fn bson_kind(d: &BsonDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 643*

---

## bson_key

```fides
pub fn bson_key(d: &BsonDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 647*

---

## bson_count

```fides
pub fn bson_count(d: &BsonDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 651*

---

## bson_child

```fides
pub fn bson_child(d: &BsonDoc, i: i64, k: i64) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 655*

---

## bson_get

The element `key` of document node `i`, or -1.

```fides
pub fn bson_get(d: &BsonDoc, i: i64, key: str) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`
- `key`: `str`

**Returns:** `i64`

*Defined at line 660*

---

## bson_path

Follow a dotted path ("cursor.firstBatch") from node i; -1 if absent.

```fides
pub fn bson_path(d: &BsonDoc, i: i64, path: str) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 673*

---

## bson_string

```fides
pub fn bson_string(d: &BsonDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 684*

---

## bson_int

A number node as an integer (int32, int64, double truncated, bool).

```fides
pub fn bson_int(d: &BsonDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 692*

---

## bson_double

A number node as a float.

```fides
pub fn bson_double(d: &BsonDoc, i: i64) -> f64 {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `f64`

*Defined at line 704*

---

## bson_bytes

```fides
pub fn bson_bytes(d: &BsonDoc, i: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 714*

---

## bson_to_json

Relaxed Extended JSON (v2) of node i.

```fides
pub fn bson_to_json(d: &BsonDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 770*

---

## bson_encode_node

The bytes of document (or array) node i, as a document.

```fides
pub fn bson_encode_node(d: &BsonDoc, i: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&BsonDoc`
- `i`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 903*

---

## bson_put_node

Put node i of another document into w under `key`.

```fides
pub fn bson_put_node(w: &mut BsonWriter, key: str, d: &BsonDoc, i: i64) {
```

**Parameters:**

- `w`: `&mut BsonWriter`
- `key`: `str`
- `d`: `&BsonDoc`
- `i`: `i64`

*Defined at line 915*

---

## mongo_crc32c

CRC-32C (Castagnoli), as OP_MSG checksums use.

```fides
pub fn mongo_crc32c(b: Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 922*

---

## mongo_op_msg

An OP_MSG: body document, then document sequences (identifier and
documents), flag bits; with checksum when flags has bit 0.

```fides
pub fn mongo_op_msg(request_id: i64, response_to: i64, flags: i64, body: Vec<i64>, seq_ids: Vec<str>, seq_docs: Vec<Vec<Vec<i64>> >) -> Vec<i64> {
```

**Parameters:**

- `request_id`: `i64`
- `response_to`: `i64`
- `flags`: `i64`
- `body`: `Vec<i64>`
- `seq_ids`: `Vec<str>`
- `seq_docs`: `Vec<Vec<Vec<i64>> >`

**Returns:** `Vec<i64>`

*Defined at line 943*

---

## MongoMessage

A message read back. complete false: more bytes needed. The body and
each sequence's documents are decoded.

```fides
pub struct MongoMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `complete` | `bool` |
| `ok` | `bool` |
| `error` | `str` |
| `length` | `i64` |
| `request_id` | `i64` |
| `response_to` | `i64` |
| `flags` | `i64` |
| `body` | `BsonDoc` |
| `body_bytes` | `Vec<i64>` |
| `seq_ids` | `Vec<str>` |
| `seq_docs` | `Vec<Vec<Vec<i64>> >` |

*Defined at line 978*

---

## mongo_read_message

Parse the OP_MSG at the start of `b`.

```fides
pub fn mongo_read_message(b: Vec<i64>) -> MongoMessage {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `MongoMessage`

*Defined at line 999*

---

## mongo_cmd_find

{find: coll, filter, [sort], [projection], [skip], [limit], batchSize,
$db}. Empty documents (5 bytes) for filter/sort/projection are left out
(filter is always sent).

```fides
pub fn mongo_cmd_find(db: str, coll: str, filter: Vec<i64>, sort: Vec<i64>, projection: Vec<i64>, skip: i64, limit: i64, batch: i64) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `coll`: `str`
- `filter`: `Vec<i64>`
- `sort`: `Vec<i64>`
- `projection`: `Vec<i64>`
- `skip`: `i64`
- `limit`: `i64`
- `batch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1102*

---

## mongo_cmd_insert

{insert: coll, ordered: true, $db}; the documents go in a "documents"
sequence section.

```fides
pub fn mongo_cmd_insert(db: str, coll: str) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `coll`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1127*

---

## mongo_update_stmt

One update statement {q, u, upsert, multi}.

```fides
pub fn mongo_update_stmt(q: Vec<i64>, u: Vec<i64>, upsert: bool, multi: bool) -> Vec<i64> {
```

**Parameters:**

- `q`: `Vec<i64>`
- `u`: `Vec<i64>`
- `upsert`: `bool`
- `multi`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1136*

---

## mongo_delete_stmt

One delete statement {q, limit} (limit 1: one document, 0: all).

```fides
pub fn mongo_delete_stmt(q: Vec<i64>, limit: i64) -> Vec<i64> {
```

**Parameters:**

- `q`: `Vec<i64>`
- `limit`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1146*

---

## mongo_cmd_aggregate

{aggregate: coll, pipeline: [stages], cursor: {batchSize}, $db}.

```fides
pub fn mongo_cmd_aggregate(db: str, coll: str, stages: Vec<Vec<i64>>, batch: i64) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `coll`: `str`
- `stages`: `Vec<Vec<i64>>`
- `batch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1162*

---

## mongo_cmd_get_more

{getMore: id, collection, batchSize, $db}.

```fides
pub fn mongo_cmd_get_more(db: str, coll: str, cursor_id: i64, batch: i64) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `coll`: `str`
- `cursor_id`: `i64`
- `batch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1182*

---

## mongo_cmd_kill_cursors

{killCursors: coll, cursors: [id], $db}.

```fides
pub fn mongo_cmd_kill_cursors(db: str, coll: str, cursor_id: i64) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `coll`: `str`
- `cursor_id`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1194*

---

## mongo_cmd_simple

{name: 1, $db} (hello, ping, buildInfo ...).

```fides
pub fn mongo_cmd_simple(db: str, name: str) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1205*

---

## MongoTransport

```fides
pub struct MongoTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `conn` | `i64` |
| `send` | `fn(i64, Vec<i64>) -> i64` |
| `recv` | `fn(i64, i64) -> Vec<i64>` |

*Defined at line 1214*

---

## mongo_tcp_transport

```fides
pub fn mongo_tcp_transport(sock: i64) -> MongoTransport {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `MongoTransport`

*Defined at line 1220*

---

## MongoClient

```fides
pub struct MongoClient {
```

**Fields:**

| Name | Type |
|------|------|
| `tr` | `MongoTransport` |
| `db` | `str` |
| `next_id` | `i64` |
| `inbuf` | `Vec<i64>` |
| `ok` | `bool` |
| `error` | `str` |
| `checksums` | `bool` |

*Defined at line 1226*

---

## mongo_client_new

```fides
pub fn mongo_client_new(tr: MongoTransport, db: str) -> MongoClient {
```

**Parameters:**

- `tr`: `MongoTransport`
- `db`: `str`

**Returns:** `MongoClient`

*Defined at line 1236*

---

## MongoReply

A command's reply. ok: the server said ok: 1. n, n_modified: write
counts; write_errors: their messages. doc is the reply body.

```fides
pub struct MongoReply {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `code` | `i64` |
| `code_name` | `str` |
| `errmsg` | `str` |
| `n` | `i64` |
| `n_modified` | `i64` |
| `write_errors` | `Vec<str>` |
| `doc` | `BsonDoc` |

*Defined at line 1244*

---

## mongo_run

Send a command (body + optional sequences) and read its reply.

```fides
pub fn mongo_run(c: &mut MongoClient, body: Vec<i64>, seq_ids: Vec<str>, seq_docs: Vec<Vec<Vec<i64>> >) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `body`: `Vec<i64>`
- `seq_ids`: `Vec<str>`
- `seq_docs`: `Vec<Vec<Vec<i64>> >`

**Returns:** `MongoReply`

*Defined at line 1282*

---

## mongo_command

```fides
pub fn mongo_command(c: &mut MongoClient, body: Vec<i64>) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `body`: `Vec<i64>`

**Returns:** `MongoReply`

*Defined at line 1336*

---

## mongo_ping

```fides
pub fn mongo_ping(c: &mut MongoClient) -> bool {
```

**Parameters:**

- `c`: `&mut MongoClient`

**Returns:** `bool`

*Defined at line 1342*

---

## mongo_insert

insert: the documents (encoded) into `coll`.

```fides
pub fn mongo_insert(c: &mut MongoClient, coll: str, docs: Vec<Vec<i64>>) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `coll`: `str`
- `docs`: `Vec<Vec<i64>>`

**Returns:** `MongoReply`

*Defined at line 1348*

---

## mongo_update

update: statements made with mongo_update_stmt.

```fides
pub fn mongo_update(c: &mut MongoClient, coll: str, stmts: Vec<Vec<i64>>) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `coll`: `str`
- `stmts`: `Vec<Vec<i64>>`

**Returns:** `MongoReply`

*Defined at line 1357*

---

## mongo_delete

delete: statements made with mongo_delete_stmt.

```fides
pub fn mongo_delete(c: &mut MongoClient, coll: str, stmts: Vec<Vec<i64>>) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `coll`: `str`
- `stmts`: `Vec<Vec<i64>>`

**Returns:** `MongoReply`

*Defined at line 1366*

---

## MongoCursor

A cursor: the documents of the current batch, the server's cursor id
(0 once exhausted), and where the next document is.

```fides
pub struct MongoCursor {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `id` | `i64` |
| `ns` | `str` |
| `coll` | `str` |
| `batch` | `Vec<Vec<i64>>` |
| `pos` | `i64` |
| `batch_size` | `i64` |
| `batches` | `i64` |

*Defined at line 1376*

---

## mongo_find

find: a cursor over the matching documents.

```fides
pub fn mongo_find(c: &mut MongoClient, coll: str, filter: Vec<i64>, sort: Vec<i64>, limit: i64, batch_size: i64) -> MongoCursor {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `coll`: `str`
- `filter`: `Vec<i64>`
- `sort`: `Vec<i64>`
- `limit`: `i64`
- `batch_size`: `i64`

**Returns:** `MongoCursor`

*Defined at line 1417*

---

## mongo_aggregate

aggregate: a cursor over the pipeline's output.

```fides
pub fn mongo_aggregate(c: &mut MongoClient, coll: str, stages: Vec<Vec<i64>>, batch_size: i64) -> MongoCursor {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `coll`: `str`
- `stages`: `Vec<Vec<i64>>`
- `batch_size`: `i64`

**Returns:** `MongoCursor`

*Defined at line 1424*

---

## mongo_cursor_more

Is there another document? Runs getMore when the batch is used up and
the server still holds the cursor.

```fides
pub fn mongo_cursor_more(c: &mut MongoClient, cur: &mut MongoCursor) -> bool {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `cur`: `&mut MongoCursor`

**Returns:** `bool`

*Defined at line 1431*

---

## mongo_cursor_next

The next document (call after mongo_cursor_more said true).

```fides
pub fn mongo_cursor_next(cur: &mut MongoCursor) -> BsonDoc {
```

**Parameters:**

- `cur`: `&mut MongoCursor`

**Returns:** `BsonDoc`

*Defined at line 1458*

---

## mongo_cursor_close

Drop a cursor before its end: killCursors on the server.

```fides
pub fn mongo_cursor_close(c: &mut MongoClient, cur: &mut MongoCursor) -> bool {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `cur`: `&mut MongoCursor`

**Returns:** `bool`

*Defined at line 1465*

---

## MongoHello

What the server said in its hello reply.

```fides
pub struct MongoHello {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `writable_primary` | `bool` |
| `min_wire` | `i64` |
| `max_wire` | `i64` |
| `max_bson_size` | `i64` |
| `max_message_size` | `i64` |
| `max_write_batch` | `i64` |
| `compression` | `Vec<str>` |
| `sasl_mechs` | `Vec<str>` |
| `compressor` | `str` |

*Defined at line 1479*

---

## mongo_cmd_hello

The connection handshake (MongoDB handshake spec): {hello: 1, client:
{application: {name}, driver: {name, version}, os: {type}}, compression:
[...], saslSupportedMechs: "db.user", $db: "admin"}. `app` "" leaves
application out; `sasl_user` "" leaves saslSupportedMechs out.

```fides
pub fn mongo_cmd_hello(app: str, os_type: str, compressors: Vec<str>, sasl_db: str, sasl_user: str) -> Vec<i64> {
```

**Parameters:**

- `app`: `str`
- `os_type`: `str`
- `compressors`: `Vec<str>`
- `sasl_db`: `str`
- `sasl_user`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1497*

---

## mongo_parse_hello

Read a hello reply; `compressor` is the first of the client's
`compressors` (its order of preference) that the server also lists, ""
when none.

```fides
pub fn mongo_parse_hello(r: &MongoReply, compressors: Vec<str>) -> MongoHello {
```

**Parameters:**

- `r`: `&MongoReply`
- `compressors`: `Vec<str>`

**Returns:** `MongoHello`

*Defined at line 1544*

---

## mongo_hello

Run the hello handshake. The negotiated compressor is recorded in the
result only: this client still sends every message uncompressed, which
the protocol allows (a server compresses only replies to compressed
requests), since OP_COMPRESSED is not implemented.

```fides
pub fn mongo_hello(c: &mut MongoClient, app: str, compressors: Vec<str>, sasl_db: str, sasl_user: str) -> MongoHello {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `app`: `str`
- `compressors`: `Vec<str>`
- `sasl_db`: `str`
- `sasl_user`: `str`

**Returns:** `MongoHello`

*Defined at line 1585*

---

## mongo_pick_mechanism

The SCRAM mechanism for an account from hello's saslSupportedMechs:
SCRAM-SHA-256 when listed or when the list is absent (MongoDB 4.0 and
later default to it), else SCRAM-SHA-1.

```fides
pub fn mongo_pick_mechanism(h: &MongoHello) -> str {
```

**Parameters:**

- `h`: `&MongoHello`

**Returns:** `str`

*Defined at line 1593*

---

## MongoScram

The state of one SCRAM conversation, client side. `hash` is KDF_SHA1
or KDF_SHA256; `secret` is the password as Hi() takes it (for MongoDB's
SCRAM-SHA-1, the hex MD5 digest of "user:mongo:password").

```fides
pub struct MongoScram {
```

**Fields:**

| Name | Type |
|------|------|
| `hash` | `i64` |
| `mechanism` | `str` |
| `user` | `str` |
| `secret` | `Vec<i64>` |
| `cnonce` | `str` |
| `client_first_bare` | `str` |
| `auth_message` | `str` |
| `server_signature` | `Vec<i64>` |
| `salted` | `Vec<i64>` |
| `iterations` | `i64` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 1612*

---

## MONGO_SCRAM_MIN_ITERATIONS:

The fewest PBKDF2 iterations accepted from a server (MongoDB's
authentication spec: drivers refuse fewer than 4096).

```fides
pub const MONGO_SCRAM_MIN_ITERATIONS: i64 = 4096;
```

*Defined at line 1629*

---

## mongo_scram_escape

A SASL name: "=" as "=3D" and "," as "=2C" (RFC 5802 §5.1).

```fides
pub fn mongo_scram_escape(user: str) -> str {
```

**Parameters:**

- `user`: `str`

**Returns:** `str`

*Defined at line 1632*

---

## mongo_password_digest

MongoDB's SCRAM-SHA-1 password: hex(MD5(user ":mongo:" password)).

```fides
pub fn mongo_password_digest(user: str, password: str) -> str {
```

**Parameters:**

- `user`: `str`
- `password`: `str`

**Returns:** `str`

*Defined at line 1637*

---

## mongo_saslprep

SASLprep (RFC 4013) of a password: non-ASCII spaces become spaces,
characters "mapped to nothing" are removed, and a prohibited character
(or malformed UTF-8) gives "" with *ok false. Not done: NFKC
normalization (text already in NFKC, which includes all ASCII, is
unaffected) and the bidirectional check.

```fides
pub fn mongo_saslprep(s: str, ok: &mut bool) -> str {
```

**Parameters:**

- `s`: `str`
- `ok`: `&mut bool`

**Returns:** `str`

*Defined at line 1733*

---

## mongo_scram_nonce

A random client nonce: 24 bytes from the system generator, base64.

```fides
pub fn mongo_scram_nonce() -> str {
```

**Returns:** `str`

*Defined at line 1760*

---

## mongo_scram_new

Start a conversation. `mechanism` is "SCRAM-SHA-1" or "SCRAM-SHA-256";
`cnonce` is the client nonce (printable, no ","), from
mongo_scram_nonce() in real use. With `mongo_secret` true the password
is prepared as MongoDB does (SHA-1: the MD5 digest; SHA-256: SASLprep);
false uses the password bytes as they are (RFC 5802's examples).

```fides
pub fn mongo_scram_new(mechanism: str, user: str, password: str, cnonce: str, mongo_secret: bool) -> MongoScram {
```

**Parameters:**

- `mechanism`: `str`
- `user`: `str`
- `password`: `str`
- `cnonce`: `str`
- `mongo_secret`: `bool`

**Returns:** `MongoScram`

*Defined at line 1769*

---

## mongo_scram_client_first

client-first-message: "n,," (no channel binding) + the bare part.

```fides
pub fn mongo_scram_client_first(s: &MongoScram) -> str {
```

**Parameters:**

- `s`: `&MongoScram`

**Returns:** `str`

*Defined at line 1804*

---

## mongo_scram_cache_size

How many SaltedPassword values are cached.

```fides
pub fn mongo_scram_cache_size() -> i64 {
```

**Returns:** `i64`

*Defined at line 1830*

---

## mongo_scram_client_final

Answer server-first-message ("r=...,s=...,i=..."): the
client-final-message with its proof, or "" with s.ok false (a nonce that
does not extend ours, a bad salt, too few iterations, a mandatory
extension, a server error).

```fides
pub fn mongo_scram_client_final(s: &mut MongoScram, server_first: str) -> str {
```

**Parameters:**

- `s`: `&mut MongoScram`
- `server_first`: `str`

**Returns:** `str`

*Defined at line 1844*

---

## mongo_scram_check_final

Check server-final-message ("v=<ServerSignature>"): true when the
server proved it knows the password; s.error says why not.

```fides
pub fn mongo_scram_check_final(s: &mut MongoScram, server_final: str) -> bool {
```

**Parameters:**

- `s`: `&mut MongoScram`
- `server_final`: `str`

**Returns:** `bool`

*Defined at line 1905*

---

## mongo_cmd_sasl_start

{saslStart: 1, mechanism, payload: BinData(0, ...), autoAuthorize: 1,
options: {skipEmptyExchange: true}, $db}.

```fides
pub fn mongo_cmd_sasl_start(db: str, mechanism: str, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `mechanism`: `str`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1942*

---

## mongo_cmd_sasl_continue

{saslContinue: 1, conversationId, payload: BinData(0, ...), $db}.

```fides
pub fn mongo_cmd_sasl_continue(db: str, conversation: i64, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`
- `conversation`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1956*

---

## mongo_auth_scram

Authenticate with SCRAM-SHA-1 or SCRAM-SHA-256 against `source`
(usually "admin"): saslStart with client-first, saslContinue with the
proof, the server signature checked, then empty saslContinue rounds
until the server says done (none when the server honours
skipEmptyExchange, 4.4 and later). The reply is ok only when every step
was.

```fides
pub fn mongo_auth_scram(c: &mut MongoClient, mechanism: str, source: str, user: str, password: str, cnonce: str) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `mechanism`: `str`
- `source`: `str`
- `user`: `str`
- `password`: `str`
- `cnonce`: `str`

**Returns:** `MongoReply`

*Defined at line 1997*

---

## mongo_login

Connect-time sequence: hello (asking for the account's mechanisms),
then SCRAM with the mechanism it names (mongo_hello and
mongo_auth_scram separately when the hello result is wanted).

```fides
pub fn mongo_login(c: &mut MongoClient, app: str, compressors: Vec<str>, source: str, user: str, password: str, cnonce: str) -> MongoReply {
```

**Parameters:**

- `c`: `&mut MongoClient`
- `app`: `str`
- `compressors`: `Vec<str>`
- `source`: `str`
- `user`: `str`
- `password`: `str`
- `cnonce`: `str`

**Returns:** `MongoReply`

*Defined at line 2036*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
