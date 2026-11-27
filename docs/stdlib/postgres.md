[All modules](index.md)

# postgres -- Fides Documentation

## Contents

- [PG_PROTOCOL_3:](#PG_PROTOCOL_3:) (const)
- [PG_AUTH:](#PG_AUTH:) (const)
- [PG_PARAMETER_STATUS:](#PG_PARAMETER_STATUS:) (const)
- [PG_BACKEND_KEY:](#PG_BACKEND_KEY:) (const)
- [PG_READY:](#PG_READY:) (const)
- [PG_ROW_DESCRIPTION:](#PG_ROW_DESCRIPTION:) (const)
- [PG_DATA_ROW:](#PG_DATA_ROW:) (const)
- [PG_COMMAND_COMPLETE:](#PG_COMMAND_COMPLETE:) (const)
- [PG_ERROR:](#PG_ERROR:) (const)
- [PG_NOTICE:](#PG_NOTICE:) (const)
- [PG_EMPTY_QUERY:](#PG_EMPTY_QUERY:) (const)
- [PG_SSL_REQUEST_CODE:](#PG_SSL_REQUEST_CODE:) (const)
- [PG_SSL_DISABLE:](#PG_SSL_DISABLE:) (const)
- [PG_SSL_PREFER:](#PG_SSL_PREFER:) (const)
- [PG_SSL_REQUIRE:](#PG_SSL_REQUIRE:) (const)
- [pg_i32](#pg_i32) (function)
- [pg_i16](#pg_i16) (function)
- [pg_msg](#pg_msg) (function)
- [pg_startup_message](#pg_startup_message) (function)
- [pg_query_message](#pg_query_message) (function)
- [pg_parse_message](#pg_parse_message) (function)
- [pg_bind_message](#pg_bind_message) (function)
- [pg_bind_message_nulls](#pg_bind_message_nulls) (function)
- [pg_describe_portal_message](#pg_describe_portal_message) (function)
- [pg_execute_message](#pg_execute_message) (function)
- [pg_sync_message](#pg_sync_message) (function)
- [pg_terminate_message](#pg_terminate_message) (function)
- [pg_password_message](#pg_password_message) (function)
- [pg_sasl_initial_message](#pg_sasl_initial_message) (function)
- [pg_sasl_response_message](#pg_sasl_response_message) (function)
- [pg_ssl_request_message](#pg_ssl_request_message) (function)
- [pg_ssl_answer](#pg_ssl_answer) (function)
- [PgIo](#PgIo) (struct)
- [pg_plain_send](#pg_plain_send) (function)
- [pg_plain_recv](#pg_plain_recv) (function)
- [pg_plain_close](#pg_plain_close) (function)
- [pg_plain_start](#pg_plain_start) (function)
- [pg_plain_io](#pg_plain_io) (function)
- [PgMsg](#PgMsg) (struct)
- [PgIn](#PgIn) (struct)
- [pg_in_bytes](#pg_in_bytes) (function)
- [pg_next](#pg_next) (function)
- [pg_error_text](#pg_error_text) (function)
- [pg_error_code](#pg_error_code) (function)
- [PgResult](#PgResult) (struct)
- [pg_collect](#pg_collect) (function)
- [pg_md5](#pg_md5) (function)
- [pg_hex](#pg_hex) (function)
- [pg_md5_password](#pg_md5_password) (function)
- [PgScram](#PgScram) (struct)
- [pg_scram_new](#pg_scram_new) (function)
- [pg_scram_nonce](#pg_scram_nonce) (function)
- [pg_scram_client_first](#pg_scram_client_first) (function)
- [pg_scram_client_final](#pg_scram_client_final) (function)
- [pg_scram_verify_server](#pg_scram_verify_server) (function)
- [PgConn](#PgConn) (struct)
- [pg_conn_new](#pg_conn_new) (function)
- [pg_flush](#pg_flush) (function)
- [pg_startup_step](#pg_startup_step) (function)
- [pg_connect](#pg_connect) (function)
- [pg_connect_io](#pg_connect_io) (function)
- [pg_parameter](#pg_parameter) (function)
- [pg_query](#pg_query) (function)
- [pg_extended_query](#pg_extended_query) (function)
- [pg_extended_query_nulls](#pg_extended_query_nulls) (function)
- [pg_query_params](#pg_query_params) (function)
- [pg_query_params_nulls](#pg_query_params_nulls) (function)
- [pg_exec](#pg_exec) (function)
- [pg_close](#pg_close) (function)

## PG_PROTOCOL_3:

Protocol version 3.0 (major 3 in the high 16 bits).

```fides
pub const PG_PROTOCOL_3: i64 = 196608;
```

*Defined at line 63*

---

## PG_AUTH:

```fides
pub const PG_AUTH: i64 = 82;  // 'R' Authentication*
```

*Defined at line 70*

---

## PG_PARAMETER_STATUS:

```fides
pub const PG_PARAMETER_STATUS: i64 = 83;  // 'S' (frontend: Sync)
```

*Defined at line 71*

---

## PG_BACKEND_KEY:

```fides
pub const PG_BACKEND_KEY: i64 = 75;  // 'K'
```

*Defined at line 72*

---

## PG_READY:

```fides
pub const PG_READY: i64 = 90;  // 'Z' ReadyForQuery
```

*Defined at line 73*

---

## PG_ROW_DESCRIPTION:

```fides
pub const PG_ROW_DESCRIPTION: i64 = 84;  // 'T'
```

*Defined at line 74*

---

## PG_DATA_ROW:

```fides
pub const PG_DATA_ROW: i64 = 68;  // 'D' (frontend: Describe)
```

*Defined at line 75*

---

## PG_COMMAND_COMPLETE:

```fides
pub const PG_COMMAND_COMPLETE: i64 = 67;  // 'C'
```

*Defined at line 76*

---

## PG_ERROR:

```fides
pub const PG_ERROR: i64 = 69;  // 'E' (frontend: Execute)
```

*Defined at line 77*

---

## PG_NOTICE:

```fides
pub const PG_NOTICE: i64 = 78;  // 'N'
```

*Defined at line 78*

---

## PG_EMPTY_QUERY:

```fides
pub const PG_EMPTY_QUERY: i64 = 73;  // 'I'
```

*Defined at line 79*

---

## PG_SSL_REQUEST_CODE:

The SSLRequest code: 1234 in the high 16 bits, 5679 in the low ones.

```fides
pub const PG_SSL_REQUEST_CODE: i64 = 80877103;
```

*Defined at line 82*

---

## PG_SSL_DISABLE:

sslmode: never ask for TLS.

```fides
pub const PG_SSL_DISABLE: i64 = 0;
```

*Defined at line 84*

---

## PG_SSL_PREFER:

sslmode: TLS when the server has it, in clear when it answers 'N'.

```fides
pub const PG_SSL_PREFER: i64 = 1;
```

*Defined at line 86*

---

## PG_SSL_REQUIRE:

sslmode: TLS or no connection.

```fides
pub const PG_SSL_REQUIRE: i64 = 2;
```

*Defined at line 88*

---

## pg_i32

A signed big-endian Int32 at `off` of `b`.

```fides
pub fn pg_i32(b: Vec<i64>, off: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 141*

---

## pg_i16

A signed big-endian Int16 at `off` of `b`.

```fides
pub fn pg_i16(b: Vec<i64>, off: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 153*

---

## pg_msg

A message: its tag byte, the Int32 length (itself and the body), the
body.

```fides
pub fn pg_msg(tag: i64, body: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `tag`: `i64`
- `body`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 192*

---

## pg_startup_message

StartupMessage (no tag): protocol 3.0, then name/value Strings, then a
NUL. `database` "" leaves it to the server (the user's name).

```fides
pub fn pg_startup_message(user: str, database: str) -> Vec<i64> {
```

**Parameters:**

- `user`: `str`
- `database`: `str`

**Returns:** `Vec<i64>`

*Defined at line 202*

---

## pg_query_message

Query ('Q'): the simple query protocol.

```fides
pub fn pg_query_message(sql: str) -> Vec<i64> {
```

**Parameters:**

- `sql`: `str`

**Returns:** `Vec<i64>`

*Defined at line 221*

---

## pg_parse_message

Parse ('P'): prepare `sql` as statement `name` ("" the unnamed one),
no parameter types given (the server infers them).

```fides
pub fn pg_parse_message(name: str, sql: str) -> Vec<i64> {
```

**Parameters:**

- `name`: `str`
- `sql`: `str`

**Returns:** `Vec<i64>`

*Defined at line 229*

---

## pg_bind_message

Bind ('B'): portal `portal` from statement `stmt` with text parameters,
results in text.

```fides
pub fn pg_bind_message(portal: str, stmt: str, params: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `portal`: `str`
- `stmt`: `str`
- `params`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 239*

---

## pg_bind_message_nulls

Bind ('B') where `nulls[i]` true sends parameter i as SQL NULL (length
-1, its text ignored); `nulls` shorter than `params` means not NULL for
the rest.

```fides
pub fn pg_bind_message_nulls(portal: str, stmt: str, params: Vec<str>, nulls: Vec<bool>) -> Vec<i64> {
```

**Parameters:**

- `portal`: `str`
- `stmt`: `str`
- `params`: `Vec<str>`
- `nulls`: `Vec<bool>`

**Returns:** `Vec<i64>`

*Defined at line 246*

---

## pg_describe_portal_message

Describe ('D') of portal `portal`: the server answers RowDescription or
NoData.

```fides
pub fn pg_describe_portal_message(portal: str) -> Vec<i64> {
```

**Parameters:**

- `portal`: `str`

**Returns:** `Vec<i64>`

*Defined at line 271*

---

## pg_execute_message

Execute ('E') portal `portal`, at most `max_rows` rows (0: all).

```fides
pub fn pg_execute_message(portal: str, max_rows: i64) -> Vec<i64> {
```

**Parameters:**

- `portal`: `str`
- `max_rows`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 279*

---

## pg_sync_message

Sync ('S'): ends an extended query; the server answers ReadyForQuery.

```fides
pub fn pg_sync_message() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 287*

---

## pg_terminate_message

Terminate ('X').

```fides
pub fn pg_terminate_message() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 292*

---

## pg_password_message

PasswordMessage ('p') with a cleartext or an "md5..." password.

```fides
pub fn pg_password_message(password: str) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`

**Returns:** `Vec<i64>`

*Defined at line 297*

---

## pg_sasl_initial_message

SASLInitialResponse ('p'): the mechanism, then the client's first
message with its Int32 length.

```fides
pub fn pg_sasl_initial_message(mechanism: str, data: str) -> Vec<i64> {
```

**Parameters:**

- `mechanism`: `str`
- `data`: `str`

**Returns:** `Vec<i64>`

*Defined at line 305*

---

## pg_sasl_response_message

SASLResponse ('p'): the data alone.

```fides
pub fn pg_sasl_response_message(data: str) -> Vec<i64> {
```

**Parameters:**

- `data`: `str`

**Returns:** `Vec<i64>`

*Defined at line 314*

---

## pg_ssl_request_message

SSLRequest (no tag): Int32 8, Int32 80877103. The server answers one
byte, 'S' (go on with the TLS handshake) or 'N' (no TLS here).

```fides
pub fn pg_ssl_request_message() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 322*

---

## pg_ssl_answer

What the server's answer to SSLRequest means under `sslmode`: 1 start
TLS, 0 go on in clear, -1 give up ('N' under PG_SSL_REQUIRE, an
ErrorResponse 'E' from a server too old for SSLRequest, anything else,
or -1 for no answer).

```fides
pub fn pg_ssl_answer(answer: i64, sslmode: i64) -> i64 {
```

**Parameters:**

- `answer`: `i64`
- `sslmode`: `i64`

**Returns:** `i64`

*Defined at line 333*

---

## PgIo

How bytes reach the server: over the socket itself, or over a TLS
session made on it (module postgres_tls). `conn` below is the socket
for the first and the session for the second.

```fides
pub struct PgIo {
```

**Fields:**

| Name | Type |
|------|------|
| `send` | `fn(i64, i64, i64) -> i64` |
| `recv` | `fn(i64, i64, i64, i64) -> i64` |
| `close` | `fn(i64) -> i64` |
| `start` | `fn(i64, str) -> i64` |

*Defined at line 348*

---

## pg_plain_send

Write over the socket itself.

```fides
pub fn pg_plain_send(conn: i64, addr: i64, n: i64) -> i64 {
```

**Parameters:**

- `conn`: `i64`
- `addr`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 362*

---

## pg_plain_recv

Read from the socket itself.

```fides
pub fn pg_plain_recv(conn: i64, addr: i64, n: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `conn`: `i64`
- `addr`: `i64`
- `n`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 367*

---

## pg_plain_close

Nothing to end on a plain socket.

```fides
pub fn pg_plain_close(_conn: i64) -> i64 {
```

**Parameters:**

- `_conn`: `i64`

**Returns:** `i64`

*Defined at line 372*

---

## pg_plain_start

No TLS: every handshake fails.

```fides
pub fn pg_plain_start(_fd: i64, _host: str) -> i64 {
```

**Parameters:**

- `_fd`: `i64`
- `_host`: `str`

**Returns:** `i64`

*Defined at line 377*

---

## pg_plain_io

The socket itself, no TLS.

```fides
pub fn pg_plain_io() -> PgIo {
```

**Returns:** `PgIo`

*Defined at line 382*

---

## PgMsg

One backend message: tag -1 at the end of the input, on a timeout or on
a malformed length.

```fides
pub struct PgMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `i64` |
| `body` | `Vec<i64>` |

*Defined at line 439*

---

## PgIn

What the server sent and has not been read yet. fd -1: only `buf`
(tests put bytes there).

```fides
pub struct PgIn {
```

**Fields:**

| Name | Type |
|------|------|
| `fd` | `i64` |
| `buf` | `Vec<i64>` |
| `pos` | `i64` |
| `conn` | `i64` |
| `recv` | `fn(i64, i64, i64, i64) -> i64` |

*Defined at line 446*

---

## pg_in_bytes

A reader over bytes already received (no socket).

```fides
pub fn pg_in_bytes(bytes: Vec<i64>) -> PgIn {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `PgIn`

*Defined at line 456*

---

## pg_next

The next message.

```fides
pub fn pg_next(r: &mut PgIn) -> PgMsg {
```

**Parameters:**

- `r`: `&mut PgIn`

**Returns:** `PgMsg`

*Defined at line 524*

---

## pg_error_text

An ErrorResponse or NoticeResponse body as "SEVERITY SQLSTATE: message"
(with " (detail)" when the server gave one).

```fides
pub fn pg_error_text(body: Vec<i64>) -> str {
```

**Parameters:**

- `body`: `Vec<i64>`

**Returns:** `str`

*Defined at line 551*

---

## pg_error_code

The SQLSTATE field ('C') of an ErrorResponse body.

```fides
pub fn pg_error_code(body: Vec<i64>) -> str {
```

**Parameters:**

- `body`: `Vec<i64>`

**Returns:** `str`

*Defined at line 581*

---

## PgResult

The outcome of a query.

```fides
pub struct PgResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `sqlstate` | `str` |
| `columns` | `Vec<str>` |
| `types` | `Vec<i64>` |
| `rows` | `str` |
| `nrows` | `i64` |
| `tag` | `str` |
| `affected` | `i64` |
| `status` | `i64` |

*Defined at line 597*

---

## pg_collect

Read the answer to a query up to ReadyForQuery: row sets, command tags
and errors (the first error is kept; the server still finishes with
ReadyForQuery).

```fides
pub fn pg_collect(r: &mut PgIn) -> PgResult {
```

**Parameters:**

- `r`: `&mut PgIn`

**Returns:** `PgResult`

*Defined at line 673*

---

## pg_md5

MD5 of bytes: 16 bytes.

```fides
pub fn pg_md5(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 902*

---

## pg_hex

Lowercase hex of bytes.

```fides
pub fn pg_hex(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 984*

---

## pg_md5_password

The answer to AuthenticationMD5Password: "md5" followed by
md5_hex(md5_hex(password + user) + salt), salt the server's 4 bytes.

```fides
pub fn pg_md5_password(user: str, password: str, salt: Vec<i64>) -> str {
```

**Parameters:**

- `user`: `str`
- `password`: `str`
- `salt`: `Vec<i64>`

**Returns:** `str`

*Defined at line 990*

---

## PgScram

The client's side of one SCRAM exchange.

```fides
pub struct PgScram {
```

**Fields:**

| Name | Type |
|------|------|
| `user` | `str` |
| `password` | `str` |
| `cnonce` | `str` |
| `client_first_bare` | `str` |
| `server_sig` | `Vec<i64>` |
| `verified` | `bool` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 1000*

---

## pg_scram_new

A SCRAM exchange. PostgreSQL takes the user from the startup message
and ignores the SCRAM one: `user` is "" there.

```fides
pub fn pg_scram_new(user: str, password: str, cnonce: str) -> PgScram {
```

**Parameters:**

- `user`: `str`
- `password`: `str`
- `cnonce`: `str`

**Returns:** `PgScram`

*Defined at line 1015*

---

## pg_scram_nonce

A fresh client nonce: 18 random bytes in base64 (24 printable
characters, never a comma).

```fides
pub fn pg_scram_nonce() -> str {
```

**Returns:** `str`

*Defined at line 1023*

---

## pg_scram_client_first

client-first-message: "n,," (no channel binding) then
"n=<user>,r=<nonce>".

```fides
pub fn pg_scram_client_first(s: &mut PgScram) -> str {
```

**Parameters:**

- `s`: `&mut PgScram`

**Returns:** `str`

*Defined at line 1047*

---

## pg_scram_client_final

client-final-message for the server's first message, with the proof;
"" and `s.ok` false when the server's message is unusable (a nonce
that does not extend ours, no salt, a bad iteration count, an error).

```fides
pub fn pg_scram_client_final(s: &mut PgScram, server_first: str) -> str {
```

**Parameters:**

- `s`: `&mut PgScram`
- `server_first`: `str`

**Returns:** `str`

*Defined at line 1080*

---

## pg_scram_verify_server

True when the server's final message carries the signature we expect
(it proves the server knows the password too).

```fides
pub fn pg_scram_verify_server(s: &PgScram, server_final: str) -> bool {
```

**Parameters:**

- `s`: `&PgScram`
- `server_final`: `str`

**Returns:** `bool`

*Defined at line 1122*

---

## PgConn

A connection. `ok` false: `error` says why (it cannot be used).

```fides
pub struct PgConn {
```

**Fields:**

| Name | Type |
|------|------|
| `fd` | `i64` |
| `ok` | `bool` |
| `error` | `str` |
| `user` | `str` |
| `password` | `str` |
| `auth` | `str` |
| `param_names` | `Vec<str>` |
| `param_vals` | `Vec<str>` |
| `pid` | `i64` |
| `secret` | `i64` |
| `out` | `Vec<i64>` |
| `conn` | `i64` |
| `io` | `PgIo` |
| `tls` | `bool` |

*Defined at line 1136*

---

## pg_conn_new

A connection over socket `fd` (-1: none, for tests) that has not
started.

```fides
pub fn pg_conn_new(fd: i64, user: str, password: str) -> PgConn {
```

**Parameters:**

- `fd`: `i64`
- `user`: `str`
- `password`: `str`

**Returns:** `PgConn`

*Defined at line 1161*

---

## pg_flush

Send what is queued in `c.out` (nothing happens with fd -1: the bytes
stay for the caller to look at).

```fides
pub fn pg_flush(c: &mut PgConn) -> bool {
```

**Parameters:**

- `c`: `&mut PgConn`

**Returns:** `bool`

*Defined at line 1169*

---

## pg_startup_step

One message of the startup phase: answers authentication requests into
`c.out`, records parameters and the key. Returns 0 to go on, 1 when the
server is ready (c.ok true), -1 on failure (c.error says why).

```fides
pub fn pg_startup_step(c: &mut PgConn, s: &mut PgScram, m: PgMsg) -> i64 {
```

**Parameters:**

- `c`: `&mut PgConn`
- `s`: `&mut PgScram`
- `m`: `PgMsg`

**Returns:** `i64`

*Defined at line 1202*

---

## pg_connect

Connect and log in. `database` "" is the server's default (the user's
name). The result's `ok` says whether it worked, `error` why not.

```fides
pub fn pg_connect(host: str, port: i64, user: str, password: str, database: str) -> PgConn {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `user`: `str`
- `password`: `str`
- `database`: `str`

**Returns:** `PgConn`

*Defined at line 1303*

---

## pg_connect_io

Log in over the connected socket `fd`: under `sslmode` PG_SSL_PREFER or
PG_SSL_REQUIRE first SSLRequest and, when the server answers 'S', the
TLS handshake `tls.start(fd, host)`; then the startup exchange through
`plain` (in clear) or `tls`. On failure the session and the socket are
closed and `ok` is false (`error` says why).

```fides
pub fn pg_connect_io(fd: i64, host: str, user: str, password: str, database: str, sslmode: i64, plain: PgIo, tls: PgIo) -> PgConn {
```

**Parameters:**

- `fd`: `i64`
- `host`: `str`
- `user`: `str`
- `password`: `str`
- `database`: `str`
- `sslmode`: `i64`
- `plain`: `PgIo`
- `tls`: `PgIo`

**Returns:** `PgConn`

*Defined at line 1318*

---

## pg_parameter

A server parameter the server reported at startup ("server_version",
"TimeZone", ...); "" when it did not.

```fides
pub fn pg_parameter(c: &PgConn, name: str) -> str {
```

**Parameters:**

- `c`: `&PgConn`
- `name`: `str`

**Returns:** `str`

*Defined at line 1380*

---

## pg_query

Run SQL with the simple query protocol: one or more statements, no
parameters. Never put outside input in `sql`: use pg_query_params.

```fides
pub fn pg_query(c: &PgConn, sql: str) -> PgResult {
```

**Parameters:**

- `c`: `&PgConn`
- `sql`: `str`

**Returns:** `PgResult`

*Defined at line 1405*

---

## pg_extended_query

The bytes pg_query_params sends: Parse, Bind, Describe, Execute, Sync
on the unnamed statement and portal.

```fides
pub fn pg_extended_query(sql: str, params: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `sql`: `str`
- `params`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 1415*

---

## pg_extended_query_nulls

pg_extended_query where `nulls[i]` true binds parameter i as NULL.

```fides
pub fn pg_extended_query_nulls(sql: str, params: Vec<str>, nulls: Vec<bool>) -> Vec<i64> {
```

**Parameters:**

- `sql`: `str`
- `params`: `Vec<str>`
- `nulls`: `Vec<bool>`

**Returns:** `Vec<i64>`

*Defined at line 1420*

---

## pg_query_params

Run one statement with parameters `$1`, `$2`, ... bound to `params`
(text; the server converts them to the column types). The values never
go through the SQL text.

```fides
pub fn pg_query_params(c: &PgConn, sql: str, params: Vec<str>) -> PgResult {
```

**Parameters:**

- `c`: `&PgConn`
- `sql`: `str`
- `params`: `Vec<str>`

**Returns:** `PgResult`

*Defined at line 1432*

---

## pg_query_params_nulls

pg_query_params where `nulls[i]` true binds `$<i+1>` as SQL NULL (the
text in params[i] is then ignored).

```fides
pub fn pg_query_params_nulls(c: &PgConn, sql: str, params: Vec<str>, nulls: Vec<bool>) -> PgResult {
```

**Parameters:**

- `c`: `&PgConn`
- `sql`: `str`
- `params`: `Vec<str>`
- `nulls`: `Vec<bool>`

**Returns:** `PgResult`

*Defined at line 1443*

---

## pg_exec

Run a statement for its effect: the rows it changed, -1 on an error.

```fides
pub fn pg_exec(c: &PgConn, sql: str) -> i64 {
```

**Parameters:**

- `c`: `&PgConn`
- `sql`: `str`

**Returns:** `i64`

*Defined at line 1451*

---

## pg_close

Say goodbye (Terminate), end the TLS session if any and close the
socket.

```fides
pub fn pg_close(c: &mut PgConn) {
```

**Parameters:**

- `c`: `&mut PgConn`

*Defined at line 1461*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
