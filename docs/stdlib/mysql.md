[All modules](index.md)

# mysql -- Fides Documentation

## Contents

- [MYSQL_CLIENT_LONG_PASSWORD:](#MYSQL_CLIENT_LONG_PASSWORD:) (const)
- [MYSQL_CLIENT_FOUND_ROWS:](#MYSQL_CLIENT_FOUND_ROWS:) (const)
- [MYSQL_CLIENT_LONG_FLAG:](#MYSQL_CLIENT_LONG_FLAG:) (const)
- [MYSQL_CLIENT_CONNECT_WITH_DB:](#MYSQL_CLIENT_CONNECT_WITH_DB:) (const)
- [MYSQL_CLIENT_PROTOCOL_41:](#MYSQL_CLIENT_PROTOCOL_41:) (const)
- [MYSQL_CLIENT_TRANSACTIONS:](#MYSQL_CLIENT_TRANSACTIONS:) (const)
- [MYSQL_CLIENT_SECURE_CONNECTION:](#MYSQL_CLIENT_SECURE_CONNECTION:) (const)
- [MYSQL_CLIENT_MULTI_RESULTS:](#MYSQL_CLIENT_MULTI_RESULTS:) (const)
- [MYSQL_CLIENT_PLUGIN_AUTH:](#MYSQL_CLIENT_PLUGIN_AUTH:) (const)
- [MYSQL_CLIENT_PLUGIN_AUTH_LENENC:](#MYSQL_CLIENT_PLUGIN_AUTH_LENENC:) (const)
- [MYSQL_CLIENT_DEPRECATE_EOF:](#MYSQL_CLIENT_DEPRECATE_EOF:) (const)
- [MYSQL_TYPE_DECIMAL:](#MYSQL_TYPE_DECIMAL:) (const)
- [MYSQL_TYPE_TINY:](#MYSQL_TYPE_TINY:) (const)
- [MYSQL_TYPE_SHORT:](#MYSQL_TYPE_SHORT:) (const)
- [MYSQL_TYPE_LONG:](#MYSQL_TYPE_LONG:) (const)
- [MYSQL_TYPE_FLOAT:](#MYSQL_TYPE_FLOAT:) (const)
- [MYSQL_TYPE_DOUBLE:](#MYSQL_TYPE_DOUBLE:) (const)
- [MYSQL_TYPE_NULL:](#MYSQL_TYPE_NULL:) (const)
- [MYSQL_TYPE_TIMESTAMP:](#MYSQL_TYPE_TIMESTAMP:) (const)
- [MYSQL_TYPE_LONGLONG:](#MYSQL_TYPE_LONGLONG:) (const)
- [MYSQL_TYPE_INT24:](#MYSQL_TYPE_INT24:) (const)
- [MYSQL_TYPE_DATE:](#MYSQL_TYPE_DATE:) (const)
- [MYSQL_TYPE_TIME:](#MYSQL_TYPE_TIME:) (const)
- [MYSQL_TYPE_DATETIME:](#MYSQL_TYPE_DATETIME:) (const)
- [MYSQL_TYPE_YEAR:](#MYSQL_TYPE_YEAR:) (const)
- [MYSQL_TYPE_VARCHAR:](#MYSQL_TYPE_VARCHAR:) (const)
- [MYSQL_TYPE_NEWDECIMAL:](#MYSQL_TYPE_NEWDECIMAL:) (const)
- [MYSQL_TYPE_BLOB:](#MYSQL_TYPE_BLOB:) (const)
- [MYSQL_TYPE_VAR_STRING:](#MYSQL_TYPE_VAR_STRING:) (const)
- [MYSQL_TYPE_STRING:](#MYSQL_TYPE_STRING:) (const)
- [MYSQL_MAX_PAYLOAD:](#MYSQL_MAX_PAYLOAD:) (const)
- [mysql_sha1](#mysql_sha1) (function)
- [mysql_sha256](#mysql_sha256) (function)
- [mysql_native_scramble](#mysql_native_scramble) (function)
- [mysql_sha2_scramble](#mysql_sha2_scramble) (function)
- [mysql_packets](#mysql_packets) (function)
- [MysqlPacket](#MysqlPacket) (struct)
- [mysql_read_packet](#mysql_read_packet) (function)
- [mysql_put_lenenc](#mysql_put_lenenc) (function)
- [MysqlReader](#MysqlReader) (struct)
- [mysql_reader](#mysql_reader) (function)
- [mysql_get_le](#mysql_get_le) (function)
- [mysql_get_lenenc](#mysql_get_lenenc) (function)
- [mysql_get_bytes](#mysql_get_bytes) (function)
- [mysql_get_str](#mysql_get_str) (function)
- [mysql_get_lenenc_str](#mysql_get_lenenc_str) (function)
- [mysql_get_cstr](#mysql_get_cstr) (function)
- [MysqlHandshake](#MysqlHandshake) (struct)
- [mysql_parse_handshake](#mysql_parse_handshake) (function)
- [mysql_client_caps](#mysql_client_caps) (function)
- [mysql_scramble_for](#mysql_scramble_for) (function)
- [mysql_handshake_response](#mysql_handshake_response) (function)
- [MysqlStatus](#MysqlStatus) (struct)
- [mysql_is_eof](#mysql_is_eof) (function)
- [mysql_parse_status](#mysql_parse_status) (function)
- [mysql_com_query](#mysql_com_query) (function)
- [mysql_com_init_db](#mysql_com_init_db) (function)
- [mysql_com_ping](#mysql_com_ping) (function)
- [mysql_com_quit](#mysql_com_quit) (function)
- [mysql_com_stmt_prepare](#mysql_com_stmt_prepare) (function)
- [mysql_com_stmt_close](#mysql_com_stmt_close) (function)
- [MysqlParam](#MysqlParam) (struct)
- [mysql_param_int](#mysql_param_int) (function)
- [mysql_param_double](#mysql_param_double) (function)
- [mysql_param_text](#mysql_param_text) (function)
- [mysql_param_varchar](#mysql_param_varchar) (function)
- [mysql_param_null](#mysql_param_null) (function)
- [mysql_com_stmt_execute](#mysql_com_stmt_execute) (function)
- [MysqlColumn](#MysqlColumn) (struct)
- [mysql_parse_column](#mysql_parse_column) (function)
- [mysql_parse_text_row](#mysql_parse_text_row) (function)
- [mysql_parse_binary_row](#mysql_parse_binary_row) (function)
- [MysqlResult](#MysqlResult) (struct)
- [mysql_cell](#mysql_cell) (function)
- [mysql_is_null](#mysql_is_null) (function)
- [mysql_column_index](#mysql_column_index) (function)
- [mysql_read_result](#mysql_read_result) (function)
- [MysqlStmt](#MysqlStmt) (struct)
- [mysql_read_prepare](#mysql_read_prepare) (function)
- [MysqlAuthOpts](#MysqlAuthOpts) (struct)
- [mysql_auth_opts](#mysql_auth_opts) (function)
- [mysql_rsa_key_from_pem](#mysql_rsa_key_from_pem) (function)
- [mysql_xor_password](#mysql_xor_password) (function)
- [mysql_rsa_encrypt_password](#mysql_rsa_encrypt_password) (function)
- [mysql_rsa_decrypt_password](#mysql_rsa_decrypt_password) (function)
- [MysqlTransport](#MysqlTransport) (struct)
- [mysql_tcp_transport](#mysql_tcp_transport) (function)
- [MysqlConn](#MysqlConn) (struct)
- [mysql_next_payload](#mysql_next_payload) (function)
- [mysql_connect](#mysql_connect) (function)
- [mysql_connect_opts](#mysql_connect_opts) (function)
- [mysql_query](#mysql_query) (function)
- [mysql_ping](#mysql_ping) (function)
- [mysql_prepare](#mysql_prepare) (function)
- [mysql_execute](#mysql_execute) (function)
- [mysql_close_stmt](#mysql_close_stmt) (function)
- [mysql_close](#mysql_close) (function)

## MYSQL_CLIENT_LONG_PASSWORD:

```fides
pub const MYSQL_CLIENT_LONG_PASSWORD: i64 = 1;
```

*Defined at line 61*

---

## MYSQL_CLIENT_FOUND_ROWS:

```fides
pub const MYSQL_CLIENT_FOUND_ROWS: i64 = 2;
```

*Defined at line 62*

---

## MYSQL_CLIENT_LONG_FLAG:

```fides
pub const MYSQL_CLIENT_LONG_FLAG: i64 = 4;
```

*Defined at line 63*

---

## MYSQL_CLIENT_CONNECT_WITH_DB:

```fides
pub const MYSQL_CLIENT_CONNECT_WITH_DB: i64 = 8;
```

*Defined at line 64*

---

## MYSQL_CLIENT_PROTOCOL_41:

```fides
pub const MYSQL_CLIENT_PROTOCOL_41: i64 = 512;
```

*Defined at line 65*

---

## MYSQL_CLIENT_TRANSACTIONS:

```fides
pub const MYSQL_CLIENT_TRANSACTIONS: i64 = 8192;
```

*Defined at line 66*

---

## MYSQL_CLIENT_SECURE_CONNECTION:

```fides
pub const MYSQL_CLIENT_SECURE_CONNECTION: i64 = 32768;
```

*Defined at line 67*

---

## MYSQL_CLIENT_MULTI_RESULTS:

```fides
pub const MYSQL_CLIENT_MULTI_RESULTS: i64 = 131072;
```

*Defined at line 68*

---

## MYSQL_CLIENT_PLUGIN_AUTH:

```fides
pub const MYSQL_CLIENT_PLUGIN_AUTH: i64 = 524288;
```

*Defined at line 69*

---

## MYSQL_CLIENT_PLUGIN_AUTH_LENENC:

```fides
pub const MYSQL_CLIENT_PLUGIN_AUTH_LENENC: i64 = 2097152;
```

*Defined at line 70*

---

## MYSQL_CLIENT_DEPRECATE_EOF:

```fides
pub const MYSQL_CLIENT_DEPRECATE_EOF: i64 = 16777216;
```

*Defined at line 71*

---

## MYSQL_TYPE_DECIMAL:

```fides
pub const MYSQL_TYPE_DECIMAL: i64 = 0;
```

*Defined at line 74*

---

## MYSQL_TYPE_TINY:

```fides
pub const MYSQL_TYPE_TINY: i64 = 1;
```

*Defined at line 75*

---

## MYSQL_TYPE_SHORT:

```fides
pub const MYSQL_TYPE_SHORT: i64 = 2;
```

*Defined at line 76*

---

## MYSQL_TYPE_LONG:

```fides
pub const MYSQL_TYPE_LONG: i64 = 3;
```

*Defined at line 77*

---

## MYSQL_TYPE_FLOAT:

```fides
pub const MYSQL_TYPE_FLOAT: i64 = 4;
```

*Defined at line 78*

---

## MYSQL_TYPE_DOUBLE:

```fides
pub const MYSQL_TYPE_DOUBLE: i64 = 5;
```

*Defined at line 79*

---

## MYSQL_TYPE_NULL:

```fides
pub const MYSQL_TYPE_NULL: i64 = 6;
```

*Defined at line 80*

---

## MYSQL_TYPE_TIMESTAMP:

```fides
pub const MYSQL_TYPE_TIMESTAMP: i64 = 7;
```

*Defined at line 81*

---

## MYSQL_TYPE_LONGLONG:

```fides
pub const MYSQL_TYPE_LONGLONG: i64 = 8;
```

*Defined at line 82*

---

## MYSQL_TYPE_INT24:

```fides
pub const MYSQL_TYPE_INT24: i64 = 9;
```

*Defined at line 83*

---

## MYSQL_TYPE_DATE:

```fides
pub const MYSQL_TYPE_DATE: i64 = 10;
```

*Defined at line 84*

---

## MYSQL_TYPE_TIME:

```fides
pub const MYSQL_TYPE_TIME: i64 = 11;
```

*Defined at line 85*

---

## MYSQL_TYPE_DATETIME:

```fides
pub const MYSQL_TYPE_DATETIME: i64 = 12;
```

*Defined at line 86*

---

## MYSQL_TYPE_YEAR:

```fides
pub const MYSQL_TYPE_YEAR: i64 = 13;
```

*Defined at line 87*

---

## MYSQL_TYPE_VARCHAR:

```fides
pub const MYSQL_TYPE_VARCHAR: i64 = 15;
```

*Defined at line 88*

---

## MYSQL_TYPE_NEWDECIMAL:

```fides
pub const MYSQL_TYPE_NEWDECIMAL: i64 = 246;
```

*Defined at line 89*

---

## MYSQL_TYPE_BLOB:

```fides
pub const MYSQL_TYPE_BLOB: i64 = 252;
```

*Defined at line 90*

---

## MYSQL_TYPE_VAR_STRING:

```fides
pub const MYSQL_TYPE_VAR_STRING: i64 = 253;
```

*Defined at line 91*

---

## MYSQL_TYPE_STRING:

```fides
pub const MYSQL_TYPE_STRING: i64 = 254;
```

*Defined at line 92*

---

## MYSQL_MAX_PAYLOAD:

```fides
pub const MYSQL_MAX_PAYLOAD: i64 = 16777215;
```

*Defined at line 94*

---

## mysql_sha1

SHA-1 (FIPS 180-4) of bytes: 20 bytes.

```fides
pub fn mysql_sha1(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 146*

---

## mysql_sha256

SHA-256 (FIPS 180-4) of bytes: 32 bytes.

```fides
pub fn mysql_sha256(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 276*

---

## mysql_native_scramble

mysql_native_password's auth response (empty for an empty password).

```fides
pub fn mysql_native_scramble(password: str, nonce: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`
- `nonce`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 369*

---

## mysql_sha2_scramble

caching_sha2_password's fast-auth response.

```fides
pub fn mysql_sha2_scramble(password: str, nonce: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`
- `nonce`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 379*

---

## mysql_packets

One payload as packets (split at 16 MiB - 1; a payload of exactly a
multiple of that gets an empty packet after it), sequence ids from `seq`.

```fides
pub fn mysql_packets(payload: Vec<i64>, seq: i64) -> Vec<i64> {
```

**Parameters:**

- `payload`: `Vec<i64>`
- `seq`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 392*

---

## MysqlPacket

A payload read from packets at `at`: complete false when bytes are
missing; used = bytes taken; seq = the last packet's sequence id.

```fides
pub struct MysqlPacket {
```

**Fields:**

| Name | Type |
|------|------|
| `complete` | `bool` |
| `payload` | `Vec<i64>` |
| `seq` | `i64` |
| `used` | `i64` |

*Defined at line 421*

---

## mysql_read_packet

```fides
pub fn mysql_read_packet(b: Vec<i64>, at: i64) -> MysqlPacket {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `MysqlPacket`

*Defined at line 428*

---

## mysql_put_lenenc

Append a length-encoded integer.

```fides
pub fn mysql_put_lenenc(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 460*

---

## MysqlReader

A cursor over a payload (readers stop at the end: ok turns false).

```fides
pub struct MysqlReader {
```

**Fields:**

| Name | Type |
|------|------|
| `b` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |

*Defined at line 500*

---

## mysql_reader

```fides
pub fn mysql_reader(b: Vec<i64>) -> MysqlReader {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `MysqlReader`

*Defined at line 506*

---

## mysql_get_le

```fides
pub fn mysql_get_le(r: &mut MysqlReader, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut MysqlReader`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 513*

---

## mysql_get_lenenc

A length-encoded integer; -1 for the NULL marker 0xFB.

```fides
pub fn mysql_get_lenenc(r: &mut MysqlReader) -> i64 {
```

**Parameters:**

- `r`: `&mut MysqlReader`

**Returns:** `i64`

*Defined at line 530*

---

## mysql_get_bytes

```fides
pub fn mysql_get_bytes(r: &mut MysqlReader, n: i64) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut MysqlReader`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 551*

---

## mysql_get_str

```fides
pub fn mysql_get_str(r: &mut MysqlReader, n: i64) -> str {
```

**Parameters:**

- `r`: `&mut MysqlReader`
- `n`: `i64`

**Returns:** `str`

*Defined at line 567*

---

## mysql_get_lenenc_str

```fides
pub fn mysql_get_lenenc_str(r: &mut MysqlReader) -> str {
```

**Parameters:**

- `r`: `&mut MysqlReader`

**Returns:** `str`

*Defined at line 571*

---

## mysql_get_cstr

```fides
pub fn mysql_get_cstr(r: &mut MysqlReader) -> str {
```

**Parameters:**

- `r`: `&mut MysqlReader`

**Returns:** `str`

*Defined at line 579*

---

## MysqlHandshake

```fides
pub struct MysqlHandshake {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `protocol` | `i64` |
| `server_version` | `str` |
| `connection_id` | `i64` |
| `nonce` | `Vec<i64>` |
| `capabilities` | `i64` |
| `charset` | `i64` |
| `status` | `i64` |
| `plugin` | `str` |

*Defined at line 597*

---

## mysql_parse_handshake

Parse a Handshake v10 payload.

```fides
pub fn mysql_parse_handshake(payload: Vec<i64>) -> MysqlHandshake {
```

**Parameters:**

- `payload`: `Vec<i64>`

**Returns:** `MysqlHandshake`

*Defined at line 611*

---

## mysql_client_caps

The capabilities this client asks for, within what the server has.

```fides
pub fn mysql_client_caps(server_caps: i64, with_db: bool) -> i64 {
```

**Parameters:**

- `server_caps`: `i64`
- `with_db`: `bool`

**Returns:** `i64`

*Defined at line 667*

---

## mysql_scramble_for

The auth response for a plugin and nonce.

```fides
pub fn mysql_scramble_for(plugin: str, password: str, nonce: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `plugin`: `str`
- `password`: `str`
- `nonce`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 676*

---

## mysql_handshake_response

HandshakeResponse41's payload (utf8mb4, character set 45).

```fides
pub fn mysql_handshake_response(caps: i64, user: str, auth: Vec<i64>, database: str, plugin: str) -> Vec<i64> {
```

**Parameters:**

- `caps`: `i64`
- `user`: `str`
- `auth`: `Vec<i64>`
- `database`: `str`
- `plugin`: `str`

**Returns:** `Vec<i64>`

*Defined at line 689*

---

## MysqlStatus

What an OK, ERR or EOF packet says. kind: "OK", "ERR", "EOF" or "".

```fides
pub struct MysqlStatus {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `affected_rows` | `i64` |
| `last_insert_id` | `i64` |
| `status_flags` | `i64` |
| `warnings` | `i64` |
| `info` | `str` |
| `error_code` | `i64` |
| `sql_state` | `str` |
| `message` | `str` |

*Defined at line 725*

---

## mysql_is_eof

Is this payload an EOF packet (0xFE, shorter than 9 bytes)?

```fides
pub fn mysql_is_eof(p: Vec<i64>) -> bool {
```

**Parameters:**

- `p`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 744*

---

## mysql_parse_status

Parse an OK (0x00, or 0xFE ending a resultset under DEPRECATE_EOF),
ERR (0xFF) or EOF (0xFE, short) payload.

```fides
pub fn mysql_parse_status(p: Vec<i64>) -> MysqlStatus {
```

**Parameters:**

- `p`: `Vec<i64>`

**Returns:** `MysqlStatus`

*Defined at line 750*

---

## mysql_com_query

```fides
pub fn mysql_com_query(sql: str) -> Vec<i64> {
```

**Parameters:**

- `sql`: `str`

**Returns:** `Vec<i64>`

*Defined at line 788*

---

## mysql_com_init_db

```fides
pub fn mysql_com_init_db(db: str) -> Vec<i64> {
```

**Parameters:**

- `db`: `str`

**Returns:** `Vec<i64>`

*Defined at line 795*

---

## mysql_com_ping

```fides
pub fn mysql_com_ping() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 802*

---

## mysql_com_quit

```fides
pub fn mysql_com_quit() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 808*

---

## mysql_com_stmt_prepare

```fides
pub fn mysql_com_stmt_prepare(sql: str) -> Vec<i64> {
```

**Parameters:**

- `sql`: `str`

**Returns:** `Vec<i64>`

*Defined at line 814*

---

## mysql_com_stmt_close

```fides
pub fn mysql_com_stmt_close(stmt_id: i64) -> Vec<i64> {
```

**Parameters:**

- `stmt_id`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 821*

---

## MysqlParam

A parameter for COM_STMT_EXECUTE: type MYSQL_TYPE_LONGLONG (int),
MYSQL_TYPE_DOUBLE (num), MYSQL_TYPE_VAR_STRING (text) or MYSQL_TYPE_NULL.

```fides
pub struct MysqlParam {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `ival` | `i64` |
| `num` | `f64` |
| `text` | `str` |

*Defined at line 830*

---

## mysql_param_int

```fides
pub fn mysql_param_int(v: i64) -> MysqlParam {
```

**Parameters:**

- `v`: `i64`

**Returns:** `MysqlParam`

*Defined at line 837*

---

## mysql_param_double

```fides
pub fn mysql_param_double(v: f64) -> MysqlParam {
```

**Parameters:**

- `v`: `f64`

**Returns:** `MysqlParam`

*Defined at line 843*

---

## mysql_param_text

```fides
pub fn mysql_param_text(v: str) -> MysqlParam {
```

**Parameters:**

- `v`: `str`

**Returns:** `MysqlParam`

*Defined at line 849*

---

## mysql_param_varchar

A text parameter sent with type VARCHAR (as some clients do).

```fides
pub fn mysql_param_varchar(v: str) -> MysqlParam {
```

**Parameters:**

- `v`: `str`

**Returns:** `MysqlParam`

*Defined at line 856*

---

## mysql_param_null

```fides
pub fn mysql_param_null() -> MysqlParam {
```

**Returns:** `MysqlParam`

*Defined at line 862*

---

## mysql_com_stmt_execute

COM_STMT_EXECUTE (no cursor, one iteration, types always sent).

```fides
pub fn mysql_com_stmt_execute(stmt_id: i64, params: Vec<MysqlParam>) -> Vec<i64> {
```

**Parameters:**

- `stmt_id`: `i64`
- `params`: `Vec<MysqlParam>`

**Returns:** `Vec<i64>`

*Defined at line 869*

---

## MysqlColumn

```fides
pub struct MysqlColumn {
```

**Fields:**

| Name | Type |
|------|------|
| `schema` | `str` |
| `table` | `str` |
| `name` | `str` |
| `org_name` | `str` |
| `charset` | `i64` |
| `length` | `i64` |
| `kind` | `i64` |
| `flags` | `i64` |
| `decimals` | `i64` |

*Defined at line 922*

---

## mysql_parse_column

Parse a ColumnDefinition41 payload.

```fides
pub fn mysql_parse_column(p: Vec<i64>) -> MysqlColumn {
```

**Parameters:**

- `p`: `Vec<i64>`

**Returns:** `MysqlColumn`

*Defined at line 935*

---

## mysql_parse_text_row

A text-protocol row: each value as text, nulls[i] true for NULL.

```fides
pub fn mysql_parse_text_row(p: Vec<i64>, ncols: i64, vals: &mut Vec<str>, nulls: &mut Vec<bool>) {
```

**Parameters:**

- `p`: `Vec<i64>`
- `ncols`: `i64`
- `vals`: `&mut Vec<str>`
- `nulls`: `&mut Vec<bool>`

*Defined at line 955*

---

## mysql_parse_binary_row

A binary-protocol row (0x00, NULL bitmap with offset 2, values) as text
values: integers in decimal, floats with float_to_str, dates as
"YYYY-MM-DD[ HH:MM:SS[.ffffff]]", times as "[-][D ]HH:MM:SS".

```fides
pub fn mysql_parse_binary_row(p: Vec<i64>, cols: Vec<MysqlColumn>, vals: &mut Vec<str>, nulls: &mut Vec<bool>) {
```

**Parameters:**

- `p`: `Vec<i64>`
- `cols`: `Vec<MysqlColumn>`
- `vals`: `&mut Vec<str>`
- `nulls`: `&mut Vec<bool>`

*Defined at line 992*

---

## MysqlResult

A command's result: an OK (affected rows, insert id), an error, or a
resultset with columns and rows (vals[row * ncols + col]).

```fides
pub struct MysqlResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error_code` | `i64` |
| `sql_state` | `str` |
| `message` | `str` |
| `affected_rows` | `i64` |
| `last_insert_id` | `i64` |
| `warnings` | `i64` |
| `columns` | `Vec<MysqlColumn>` |
| `nrows` | `i64` |
| `vals` | `Vec<str>` |
| `nulls` | `Vec<bool>` |

*Defined at line 1114*

---

## mysql_cell

```fides
pub fn mysql_cell(r: &MysqlResult, row: i64, col: i64) -> str {
```

**Parameters:**

- `r`: `&MysqlResult`
- `row`: `i64`
- `col`: `i64`

**Returns:** `str`

*Defined at line 1134*

---

## mysql_is_null

```fides
pub fn mysql_is_null(r: &MysqlResult, row: i64, col: i64) -> bool {
```

**Parameters:**

- `r`: `&MysqlResult`
- `row`: `i64`
- `col`: `i64`

**Returns:** `bool`

*Defined at line 1138*

---

## mysql_column_index

The column index of `name`, or -1.

```fides
pub fn mysql_column_index(r: &MysqlResult, name: str) -> i64 {
```

**Parameters:**

- `r`: `&MysqlResult`
- `name`: `str`

**Returns:** `i64`

*Defined at line 1143*

---

## mysql_read_result

Read a COM_QUERY or COM_STMT_EXECUTE answer from its payloads, in
order. deprecate_eof: the session has CLIENT_DEPRECATE_EOF. binary:
rows are in the binary protocol. Returns how many payloads it used
(0 when the payloads stop before the answer is whole).

```fides
pub fn mysql_read_result(payloads: Vec<Vec<i64>>, deprecate_eof: bool, binary: bool, res: &mut MysqlResult) -> i64 {
```

**Parameters:**

- `payloads`: `Vec<Vec<i64>>`
- `deprecate_eof`: `bool`
- `binary`: `bool`
- `res`: `&mut MysqlResult`

**Returns:** `i64`

*Defined at line 1175*

---

## MysqlStmt

COM_STMT_PREPARE's answer: statement id, parameter count, columns.

```fides
pub struct MysqlStmt {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `id` | `i64` |
| `nparams` | `i64` |
| `params` | `Vec<MysqlColumn>` |
| `columns` | `Vec<MysqlColumn>` |
| `warnings` | `i64` |
| `error` | `str` |

*Defined at line 1224*

---

## mysql_read_prepare

Read a COM_STMT_PREPARE answer from its payloads; used: payloads taken
(0 when incomplete).

```fides
pub fn mysql_read_prepare(payloads: Vec<Vec<i64>>, deprecate_eof: bool, used: &mut i64) -> MysqlStmt {
```

**Parameters:**

- `payloads`: `Vec<Vec<i64>>`
- `deprecate_eof`: `bool`
- `used`: `&mut i64`

**Returns:** `MysqlStmt`

*Defined at line 1236*

---

## MysqlAuthOpts

How a connection may send a password the server must see in full
(sha256_password, and caching_sha2_password's full authentication):
secure: the transport is already encrypted or local (TLS, a Unix
socket): the password is sent as it is, NUL-terminated;
server_public_key: the server's RSA public key in PEM ("PUBLIC KEY"
or "RSA PUBLIC KEY"), "" when not configured (MySQL's
--server-public-key-path);
get_server_public_key: caching_sha2_password may ask the server for
its key over the unencrypted connection (MySQL's
--get-server-public-key; off by default, as there: a key fetched
that way can be replaced by anyone on the path). sha256_password
always asks when it has no key, as libmysqlclient does.

```fides
pub struct MysqlAuthOpts {
```

**Fields:**

| Name | Type |
|------|------|
| `secure` | `bool` |
| `server_public_key` | `str` |
| `get_server_public_key` | `bool` |

*Defined at line 1305*

---

## mysql_auth_opts

The defaults: not secure, no key, do not ask for one.

```fides
pub fn mysql_auth_opts() -> MysqlAuthOpts {
```

**Returns:** `MysqlAuthOpts`

*Defined at line 1312*

---

## mysql_rsa_key_from_pem

An RSA public key from PEM text: SubjectPublicKeyInfo ("PUBLIC KEY",
what MySQL sends) or PKCS#1 ("RSA PUBLIC KEY"). ok false otherwise.

```fides
pub fn mysql_rsa_key_from_pem(text: str) -> RsaPublicKey {
```

**Parameters:**

- `text`: `str`

**Returns:** `RsaPublicKey`

*Defined at line 1320*

---

## mysql_xor_password

The password, NUL-terminated, XORed with the nonce repeated (MySQL's
xor_string over the 20-byte scramble).

```fides
pub fn mysql_xor_password(password: str, nonce: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`
- `nonce`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1339*

---

## mysql_rsa_encrypt_password

The password as MySQL sends it to a server's RSA key: (password NUL)
XOR nonce, encrypted with RSAES-OAEP (SHA-1, MGF1-SHA-1, empty label:
OpenSSL's RSA_PKCS1_OAEP_PADDING). `seed` is 20 random bytes. Empty when
the key is bad or the password too long for it (k - 42 bytes at most,
the NUL included).

```fides
pub fn mysql_rsa_encrypt_password(password: str, nonce: Vec<i64>, pk: RsaPublicKey, seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`
- `nonce`: `Vec<i64>`
- `pk`: `RsaPublicKey`
- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1358*

---

## mysql_rsa_decrypt_password

The server side of the same exchange (for tests and servers): decrypt
with the private key and undo the XOR. ok false when it fails.

```fides
pub fn mysql_rsa_decrypt_password(sk: RsaPrivateKey, nonce: Vec<i64>, ct: Vec<i64>) -> RsaPlain {
```

**Parameters:**

- `sk`: `RsaPrivateKey`
- `nonce`: `Vec<i64>`
- `ct`: `Vec<i64>`

**Returns:** `RsaPlain`

*Defined at line 1373*

---

## MysqlTransport

```fides
pub struct MysqlTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `conn` | `i64` |
| `send` | `fn(i64, Vec<i64>) -> i64` |
| `recv` | `fn(i64, i64) -> Vec<i64>` |

*Defined at line 1400*

---

## mysql_tcp_transport

```fides
pub fn mysql_tcp_transport(sock: i64) -> MysqlTransport {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `MysqlTransport`

*Defined at line 1406*

---

## MysqlConn

A connection. `server_public_key` holds the PEM key the server sent
(when one was asked for), so a caller can pin it for later connections.

```fides
pub struct MysqlConn {
```

**Fields:**

| Name | Type |
|------|------|
| `tr` | `MysqlTransport` |
| `inbuf` | `Vec<i64>` |
| `seq` | `i64` |
| `ok` | `bool` |
| `error` | `str` |
| `caps` | `i64` |
| `server_version` | `str` |
| `connection_id` | `i64` |
| `plugin` | `str` |
| `server_public_key` | `str` |
| `full_auth` | `bool` |

*Defined at line 1414*

---

## mysql_next_payload

The next payload from the server (empty and c.ok false on error).

```fides
pub fn mysql_next_payload(c: &mut MysqlConn) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut MysqlConn`

**Returns:** `Vec<i64>`

*Defined at line 1448*

---

## mysql_connect

Connect: read the handshake, answer it, and follow the authentication
exchange to its OK. c.ok false (c.error says why) on failure. With the
default options a caching_sha2_password account that needs full
authentication is refused (no TLS, no key): see mysql_connect_opts.

```fides
pub fn mysql_connect(tr: MysqlTransport, user: str, password: str, database: str) -> MysqlConn {
```

**Parameters:**

- `tr`: `MysqlTransport`
- `user`: `str`
- `password`: `str`
- `database`: `str`

**Returns:** `MysqlConn`

*Defined at line 1483*

---

## mysql_connect_opts

Connect with explicit options for the password exchanges that need TLS
or the server's RSA key (sha256_password; caching_sha2_password's full
authentication, which the server asks for when its cache has no entry
for the account: AuthMoreData 0x01 0x04).

```fides
pub fn mysql_connect_opts(tr: MysqlTransport, user: str, password: str, database: str, opts: MysqlAuthOpts) -> MysqlConn {
```

**Parameters:**

- `tr`: `MysqlTransport`
- `user`: `str`
- `password`: `str`
- `database`: `str`
- `opts`: `MysqlAuthOpts`

**Returns:** `MysqlConn`

*Defined at line 1546*

---

## mysql_query

COM_QUERY: the OK, the error or the rows (text protocol).

```fides
pub fn mysql_query(c: &mut MysqlConn, sql: str) -> MysqlResult {
```

**Parameters:**

- `c`: `&mut MysqlConn`
- `sql`: `str`

**Returns:** `MysqlResult`

*Defined at line 1673*

---

## mysql_ping

```fides
pub fn mysql_ping(c: &mut MysqlConn) -> bool {
```

**Parameters:**

- `c`: `&mut MysqlConn`

**Returns:** `bool`

*Defined at line 1678*

---

## mysql_prepare

COM_STMT_PREPARE.

```fides
pub fn mysql_prepare(c: &mut MysqlConn, sql: str) -> MysqlStmt {
```

**Parameters:**

- `c`: `&mut MysqlConn`
- `sql`: `str`

**Returns:** `MysqlStmt`

*Defined at line 1685*

---

## mysql_execute

COM_STMT_EXECUTE with parameters: the OK, error or rows (binary).

```fides
pub fn mysql_execute(c: &mut MysqlConn, stmt: &MysqlStmt, params: Vec<MysqlParam>) -> MysqlResult {
```

**Parameters:**

- `c`: `&mut MysqlConn`
- `stmt`: `&MysqlStmt`
- `params`: `Vec<MysqlParam>`

**Returns:** `MysqlResult`

*Defined at line 1705*

---

## mysql_close_stmt

COM_STMT_CLOSE (the server sends no answer).

```fides
pub fn mysql_close_stmt(c: &mut MysqlConn, stmt: &MysqlStmt) {
```

**Parameters:**

- `c`: `&mut MysqlConn`
- `stmt`: `&MysqlStmt`

*Defined at line 1716*

---

## mysql_close

```fides
pub fn mysql_close(c: &mut MysqlConn) {
```

**Parameters:**

- `c`: `&mut MysqlConn`

*Defined at line 1720*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
