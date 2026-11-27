[All modules](index.md)

# http3 -- Fides Documentation

## Contents

- [H3F_DATA:](#H3F_DATA:) (const)
- [H3F_HEADERS:](#H3F_HEADERS:) (const)
- [H3F_CANCEL_PUSH:](#H3F_CANCEL_PUSH:) (const)
- [H3F_SETTINGS:](#H3F_SETTINGS:) (const)
- [H3F_PUSH_PROMISE:](#H3F_PUSH_PROMISE:) (const)
- [H3F_GOAWAY:](#H3F_GOAWAY:) (const)
- [H3F_MAX_PUSH_ID:](#H3F_MAX_PUSH_ID:) (const)
- [H3_STREAM_CONTROL:](#H3_STREAM_CONTROL:) (const)
- [H3_STREAM_PUSH:](#H3_STREAM_PUSH:) (const)
- [H3_STREAM_QPACK_ENCODER:](#H3_STREAM_QPACK_ENCODER:) (const)
- [H3_STREAM_QPACK_DECODER:](#H3_STREAM_QPACK_DECODER:) (const)
- [H3_SETTING_QPACK_MAX_TABLE_CAPACITY:](#H3_SETTING_QPACK_MAX_TABLE_CAPACITY:) (const)
- [H3_SETTING_MAX_FIELD_SECTION_SIZE:](#H3_SETTING_MAX_FIELD_SECTION_SIZE:) (const)
- [H3_SETTING_QPACK_BLOCKED_STREAMS:](#H3_SETTING_QPACK_BLOCKED_STREAMS:) (const)
- [H3_NO_ERROR:](#H3_NO_ERROR:) (const)
- [H3_GENERAL_PROTOCOL_ERROR:](#H3_GENERAL_PROTOCOL_ERROR:) (const)
- [H3_INTERNAL_ERROR:](#H3_INTERNAL_ERROR:) (const)
- [H3_STREAM_CREATION_ERROR:](#H3_STREAM_CREATION_ERROR:) (const)
- [H3_CLOSED_CRITICAL_STREAM:](#H3_CLOSED_CRITICAL_STREAM:) (const)
- [H3_FRAME_UNEXPECTED:](#H3_FRAME_UNEXPECTED:) (const)
- [H3_FRAME_ERROR:](#H3_FRAME_ERROR:) (const)
- [H3_EXCESSIVE_LOAD:](#H3_EXCESSIVE_LOAD:) (const)
- [H3_ID_ERROR:](#H3_ID_ERROR:) (const)
- [H3_SETTINGS_ERROR:](#H3_SETTINGS_ERROR:) (const)
- [H3_MISSING_SETTINGS:](#H3_MISSING_SETTINGS:) (const)
- [H3_REQUEST_REJECTED:](#H3_REQUEST_REJECTED:) (const)
- [H3_REQUEST_CANCELLED:](#H3_REQUEST_CANCELLED:) (const)
- [H3_REQUEST_INCOMPLETE:](#H3_REQUEST_INCOMPLETE:) (const)
- [H3_MESSAGE_ERROR:](#H3_MESSAGE_ERROR:) (const)
- [H3_CONNECT_ERROR:](#H3_CONNECT_ERROR:) (const)
- [H3_VERSION_FALLBACK:](#H3_VERSION_FALLBACK:) (const)
- [H3ST_HEADERS:](#H3ST_HEADERS:) (const)
- [H3ST_BODY:](#H3ST_BODY:) (const)
- [H3ST_TRAILERS:](#H3ST_TRAILERS:) (const)
- [H3ST_DONE:](#H3ST_DONE:) (const)
- [H3ST_FAILED:](#H3ST_FAILED:) (const)
- [H3_DEFAULT_MAX_FIELD_SECTION:](#H3_DEFAULT_MAX_FIELD_SECTION:) (const)
- [H3Message](#H3Message) (struct)
- [H3Stream](#H3Stream) (struct)
- [H3Uni](#H3Uni) (struct)
- [H3Conn](#H3Conn) (struct)
- [h3_frame](#h3_frame) (function)
- [h3_settings_frame](#h3_settings_frame) (function)
- [h3_varint_frame](#h3_varint_frame) (function)
- [h3_error_name](#h3_error_name) (function)
- [h3_check_fields](#h3_check_fields) (function)
- [h3_field](#h3_field) (function)
- [h3_new](#h3_new) (function)
- [h3_conn_error](#h3_conn_error) (function)
- [h3_failed](#h3_failed) (function)
- [h3_stream_index](#h3_stream_index) (function)
- [h3_poll](#h3_poll) (function)
- [h3_requests](#h3_requests) (function)
- [h3_done](#h3_done) (function)
- [h3_message](#h3_message) (function)
- [h3_response](#h3_response) (function)
- [h3_request](#h3_request) (function)
- [h3_respond_interim](#h3_respond_interim) (function)
- [h3_respond_trailers](#h3_respond_trailers) (function)
- [h3_respond](#h3_respond) (function)
- [h3_track](#h3_track) (function)
- [h3_reject](#h3_reject) (function)
- [h3_goaway](#h3_goaway) (function)
- [h3_close](#h3_close) (function)
- [h3_idle](#h3_idle) (function)

## H3F_DATA:

Frame types (§7.2).

```fides
pub const H3F_DATA: i64 = 0x00;
```

*Defined at line 53*

---

## H3F_HEADERS:

```fides
pub const H3F_HEADERS: i64 = 0x01;
```

*Defined at line 54*

---

## H3F_CANCEL_PUSH:

```fides
pub const H3F_CANCEL_PUSH: i64 = 0x03;
```

*Defined at line 55*

---

## H3F_SETTINGS:

```fides
pub const H3F_SETTINGS: i64 = 0x04;
```

*Defined at line 56*

---

## H3F_PUSH_PROMISE:

```fides
pub const H3F_PUSH_PROMISE: i64 = 0x05;
```

*Defined at line 57*

---

## H3F_GOAWAY:

```fides
pub const H3F_GOAWAY: i64 = 0x07;
```

*Defined at line 58*

---

## H3F_MAX_PUSH_ID:

```fides
pub const H3F_MAX_PUSH_ID: i64 = 0x0d;
```

*Defined at line 59*

---

## H3_STREAM_CONTROL:

Unidirectional stream types (§6.2, RFC 9204 §4.2).

```fides
pub const H3_STREAM_CONTROL: i64 = 0x00;
```

*Defined at line 62*

---

## H3_STREAM_PUSH:

```fides
pub const H3_STREAM_PUSH: i64 = 0x01;
```

*Defined at line 63*

---

## H3_STREAM_QPACK_ENCODER:

```fides
pub const H3_STREAM_QPACK_ENCODER: i64 = 0x02;
```

*Defined at line 64*

---

## H3_STREAM_QPACK_DECODER:

```fides
pub const H3_STREAM_QPACK_DECODER: i64 = 0x03;
```

*Defined at line 65*

---

## H3_SETTING_QPACK_MAX_TABLE_CAPACITY:

Settings (§7.2.4.1, RFC 9204 §5).

```fides
pub const H3_SETTING_QPACK_MAX_TABLE_CAPACITY: i64 = 0x01;
```

*Defined at line 68*

---

## H3_SETTING_MAX_FIELD_SECTION_SIZE:

```fides
pub const H3_SETTING_MAX_FIELD_SECTION_SIZE: i64 = 0x06;
```

*Defined at line 69*

---

## H3_SETTING_QPACK_BLOCKED_STREAMS:

```fides
pub const H3_SETTING_QPACK_BLOCKED_STREAMS: i64 = 0x07;
```

*Defined at line 70*

---

## H3_NO_ERROR:

Error codes (§8.1); QPACK's are in stdlib/qpack.fd.

```fides
pub const H3_NO_ERROR: i64 = 0x100;
```

*Defined at line 73*

---

## H3_GENERAL_PROTOCOL_ERROR:

```fides
pub const H3_GENERAL_PROTOCOL_ERROR: i64 = 0x101;
```

*Defined at line 74*

---

## H3_INTERNAL_ERROR:

```fides
pub const H3_INTERNAL_ERROR: i64 = 0x102;
```

*Defined at line 75*

---

## H3_STREAM_CREATION_ERROR:

```fides
pub const H3_STREAM_CREATION_ERROR: i64 = 0x103;
```

*Defined at line 76*

---

## H3_CLOSED_CRITICAL_STREAM:

```fides
pub const H3_CLOSED_CRITICAL_STREAM: i64 = 0x104;
```

*Defined at line 77*

---

## H3_FRAME_UNEXPECTED:

```fides
pub const H3_FRAME_UNEXPECTED: i64 = 0x105;
```

*Defined at line 78*

---

## H3_FRAME_ERROR:

```fides
pub const H3_FRAME_ERROR: i64 = 0x106;
```

*Defined at line 79*

---

## H3_EXCESSIVE_LOAD:

```fides
pub const H3_EXCESSIVE_LOAD: i64 = 0x107;
```

*Defined at line 80*

---

## H3_ID_ERROR:

```fides
pub const H3_ID_ERROR: i64 = 0x108;
```

*Defined at line 81*

---

## H3_SETTINGS_ERROR:

```fides
pub const H3_SETTINGS_ERROR: i64 = 0x109;
```

*Defined at line 82*

---

## H3_MISSING_SETTINGS:

```fides
pub const H3_MISSING_SETTINGS: i64 = 0x10a;
```

*Defined at line 83*

---

## H3_REQUEST_REJECTED:

```fides
pub const H3_REQUEST_REJECTED: i64 = 0x10b;
```

*Defined at line 84*

---

## H3_REQUEST_CANCELLED:

```fides
pub const H3_REQUEST_CANCELLED: i64 = 0x10c;
```

*Defined at line 85*

---

## H3_REQUEST_INCOMPLETE:

```fides
pub const H3_REQUEST_INCOMPLETE: i64 = 0x10d;
```

*Defined at line 86*

---

## H3_MESSAGE_ERROR:

```fides
pub const H3_MESSAGE_ERROR: i64 = 0x10e;
```

*Defined at line 87*

---

## H3_CONNECT_ERROR:

```fides
pub const H3_CONNECT_ERROR: i64 = 0x10f;
```

*Defined at line 88*

---

## H3_VERSION_FALLBACK:

```fides
pub const H3_VERSION_FALLBACK: i64 = 0x110;
```

*Defined at line 89*

---

## H3ST_HEADERS:

States of a request stream.

```fides
pub const H3ST_HEADERS: i64 = 0;
```

*Defined at line 92*

---

## H3ST_BODY:

```fides
pub const H3ST_BODY: i64 = 1;
```

*Defined at line 93*

---

## H3ST_TRAILERS:

```fides
pub const H3ST_TRAILERS: i64 = 2;
```

*Defined at line 94*

---

## H3ST_DONE:

```fides
pub const H3ST_DONE: i64 = 3;
```

*Defined at line 95*

---

## H3ST_FAILED:

```fides
pub const H3ST_FAILED: i64 = 4;
```

*Defined at line 96*

---

## H3_DEFAULT_MAX_FIELD_SECTION:

The field section size we accept (SETTINGS_MAX_FIELD_SECTION_SIZE).

```fides
pub const H3_DEFAULT_MAX_FIELD_SECTION: i64 = 65536;
```

*Defined at line 99*

---

## H3Message

A request (at a server) or a response (at a client).
- `ok` once the whole message arrived and is well-formed; else
`error` is -1 (not complete yet) or the HTTP/3 error code the stream
failed with (ours or the peer's RESET_STREAM), `why` says why;
- a request's `method`, `scheme`, `authority`, `path`; a response's
`status` and how many interim (1xx) responses came before it;
- `names` / `values`: the other fields, in order; `body`; trailers.

```fides
pub struct H3Message {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `i64` |
| `why` | `str` |
| `status` | `i64` |
| `interim` | `i64` |
| `method` | `str` |
| `scheme` | `str` |
| `authority` | `str` |
| `path` | `str` |
| `names` | `Vec<str>` |
| `values` | `Vec<str>` |
| `body` | `Vec<i64>` |
| `trailer_names` | `Vec<str>` |
| `trailer_values` | `Vec<str>` |

*Defined at line 112*

---

## H3Stream

A request stream: bytes not parsed yet (`buf` from `pos`), the DATA
frame or unknown frame being read (`data_left` / `skip_left` bytes to
come), the message so far.

```fides
pub struct H3Stream {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `buf` | `Vec<i64>` |
| `pos` | `i64` |
| `data_left` | `i64` |
| `skip_left` | `i64` |
| `state` | `i64` |
| `msg` | `H3Message` |
| `content_length` | `i64` |
| `req_method` | `str` |
| `given` | `bool` |

*Defined at line 132*

---

## H3Uni

A unidirectional stream the peer opened; `stype` -1 until its type
arrived.

```fides
pub struct H3Uni {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `stype` | `i64` |
| `buf` | `Vec<i64>` |
| `pos` | `i64` |
| `skip_left` | `i64` |
| `got_settings` | `bool` |

*Defined at line 147*

---

## H3Conn

The HTTP/3 state of one connection.
- `error` -1, or the HTTP/3 error code we closed the connection with
(`why`); the peer's close is in the QuicConn (`error`, `why`);
- `peer_settings` once the peer's SETTINGS arrived, with
`peer_max_field_section` (-1: no limit);
- `goaway_recv` / `goaway_sent`: -1, or the id of a GOAWAY.

```fides
pub struct H3Conn {
```

**Fields:**

| Name | Type |
|------|------|
| `server` | `bool` |
| `started` | `bool` |
| `ctrl_id` | `i64` |
| `enc_id` | `i64` |
| `dec_id` | `i64` |
| `peer_ctrl` | `i64` |
| `peer_enc` | `i64` |
| `peer_dec` | `i64` |
| `peer_settings` | `bool` |
| `peer_max_field_section` | `i64` |
| `peer_qpack_capacity` | `i64` |
| `peer_blocked_streams` | `i64` |
| `max_field_section` | `i64` |
| `huffman` | `bool` |
| `unis` | `Vec<H3Uni>` |
| `streams` | `Vec<H3Stream>` |
| `goaway_recv` | `i64` |
| `goaway_sent` | `i64` |
| `max_push_id_peer` | `i64` |
| `highest_request` | `i64` |
| `error` | `i64` |
| `why` | `str` |

*Defined at line 162*

---

## h3_frame

One frame: its type, length and payload (§7.1).

```fides
pub fn h3_frame(ftype: i64, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `ftype`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 233*

---

## h3_settings_frame

A SETTINGS frame from parallel lists of identifiers and values.

```fides
pub fn h3_settings_frame(ids: Vec<i64>, vals: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `ids`: `Vec<i64>`
- `vals`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 242*

---

## h3_varint_frame

A frame whose payload is one varint (GOAWAY, MAX_PUSH_ID, CANCEL_PUSH).

```fides
pub fn h3_varint_frame(ftype: i64, v: i64) -> Vec<i64> {
```

**Parameters:**

- `ftype`: `i64`
- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 254*

---

## h3_error_name

The name of an HTTP/3 or QPACK error code.

```fides
pub fn h3_error_name(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 284*

---

## h3_check_fields

Check a request's, response's or trailers' fields (§4.2, §4.3): ""
when they are fine, else what is wrong (H3_MESSAGE_ERROR).

```fides
pub fn h3_check_fields(names: Vec<str>, vals: Vec<str>, request: bool, trailers: bool) -> str {
```

**Parameters:**

- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `request`: `bool`
- `trailers`: `bool`

**Returns:** `str`

*Defined at line 426*

---

## h3_field

The first value of field `name` in a message, "" if none.

```fides
pub fn h3_field(m: &H3Message, name: str) -> str {
```

**Parameters:**

- `m`: `&H3Message`
- `name`: `str`

**Returns:** `str`

*Defined at line 573*

---

## h3_new

HTTP/3 state for a connection; `server` as the QuicConn's.

```fides
pub fn h3_new(server: bool) -> H3Conn {
```

**Parameters:**

- `server`: `bool`

**Returns:** `H3Conn`

*Defined at line 587*

---

## h3_conn_error

Close the connection with an HTTP/3 error (§8).

```fides
pub fn h3_conn_error(h: &mut H3Conn, q: &mut QuicConn, code: i64, why: str) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `code`: `i64`
- `why`: `str`

*Defined at line 594*

---

## h3_failed

Whether the HTTP/3 connection is over (our error or the QUIC
connection closing).

```fides
pub fn h3_failed(h: &H3Conn, q: &QuicConn) -> bool {
```

**Parameters:**

- `h`: `&H3Conn`
- `q`: `&QuicConn`

**Returns:** `bool`

*Defined at line 605*

---

## h3_stream_index

Index of request stream `id`, -1 if we do not track it.

```fides
pub fn h3_stream_index(h: &H3Conn, id: i64) -> i64 {
```

**Parameters:**

- `h`: `&H3Conn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 610*

---

## h3_poll

Move the HTTP/3 state on: open our control streams, take the streams
the peer opened, read the control and QPACK streams, read requests or
responses. Call after every quic_conn_recv / quic_conn_on_timeout.

```fides
pub fn h3_poll(h: &mut H3Conn, q: &mut QuicConn) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`

*Defined at line 683*

---

## h3_requests

Server: the requests that arrived whole since the last call.

```fides
pub fn h3_requests(h: &mut H3Conn) -> Vec<i64> {
```

**Parameters:**

- `h`: `&mut H3Conn`

**Returns:** `Vec<i64>`

*Defined at line 1221*

---

## h3_done

Whether stream `id`'s message is complete or failed (true for a stream
we do not track).

```fides
pub fn h3_done(h: &H3Conn, id: i64) -> bool {
```

**Parameters:**

- `h`: `&H3Conn`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 1238*

---

## h3_message

The message of stream `id` as it stands: a request at a server, a
response at a client (`ok` once complete).

```fides
pub fn h3_message(h: &H3Conn, id: i64) -> H3Message {
```

**Parameters:**

- `h`: `&H3Conn`
- `id`: `i64`

**Returns:** `H3Message`

*Defined at line 1248*

---

## h3_response

Client: the response of stream `id`; once it is done the stream is
forgotten.

```fides
pub fn h3_response(h: &mut H3Conn, id: i64) -> H3Message {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `id`: `i64`

**Returns:** `H3Message`

*Defined at line 1260*

---

## h3_request

Client: send a request on a new stream. `names` / `values` are the
other fields (lowercased here). The id of the stream, or -1: not
established yet, after a GOAWAY, at the server's stream limit, or the
fields are malformed or over the server's limit.

```fides
pub fn h3_request(h: &mut H3Conn, q: &mut QuicConn, method: str, scheme: str, authority: str, path: str, names: Vec<str>, values: Vec<str>, body: Vec<i64>) -> i64 {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `method`: `str`
- `scheme`: `str`
- `authority`: `str`
- `path`: `str`
- `names`: `Vec<str>`
- `values`: `Vec<str>`
- `body`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1317*

---

## h3_respond_interim

Server: an interim (1xx) response on request `id`, before the final
one (103 Early Hints, say). False if it cannot be sent.

```fides
pub fn h3_respond_interim(h: &mut H3Conn, q: &mut QuicConn, id: i64, status: i64, names: Vec<str>, values: Vec<str>) -> bool {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `id`: `i64`
- `status`: `i64`
- `names`: `Vec<str>`
- `values`: `Vec<str>`

**Returns:** `bool`

*Defined at line 1360*

---

## h3_respond_trailers

Server: the response to request `id`, with trailers when `tnames` is
not empty. The stream is forgotten afterwards; if the request was not
complete yet the client is asked to stop sending it (STOP_SENDING
H3_NO_ERROR, §4.1). False if it cannot be sent.

```fides
pub fn h3_respond_trailers(h: &mut H3Conn, q: &mut QuicConn, id: i64, status: i64, names: Vec<str>, values: Vec<str>, body: Vec<i64>, tnames: Vec<str>, tvalues: Vec<str>) -> bool {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `id`: `i64`
- `status`: `i64`
- `names`: `Vec<str>`
- `values`: `Vec<str>`
- `body`: `Vec<i64>`
- `tnames`: `Vec<str>`
- `tvalues`: `Vec<str>`

**Returns:** `bool`

*Defined at line 1384*

---

## h3_respond

Server: the response to request `id` (see h3_respond_trailers).

```fides
pub fn h3_respond(h: &mut H3Conn, q: &mut QuicConn, id: i64, status: i64, names: Vec<str>, values: Vec<str>, body: Vec<i64>) -> bool {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `id`: `i64`
- `status`: `i64`
- `names`: `Vec<str>`
- `values`: `Vec<str>`
- `body`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1415*

---

## h3_track

Client: read the response of a stream the application opened and wrote
itself (a request h3_request does not build); `method` as sent ("HEAD"
matters: its response has no content).

```fides
pub fn h3_track(h: &mut H3Conn, id: i64, method: str) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `id`: `i64`
- `method`: `str`

*Defined at line 1423*

---

## h3_reject

Server: refuse request `id` unprocessed (H3_REQUEST_REJECTED): the
client may send it again.

```fides
pub fn h3_reject(h: &mut H3Conn, q: &mut QuicConn, id: i64) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `id`: `i64`

*Defined at line 1432*

---

## h3_goaway

Graceful shutdown (§5.2): a server's GOAWAY names the first request it
will not process (the ones it has keep going); a client's says it
accepts no push. New requests are refused afterwards.

```fides
pub fn h3_goaway(h: &mut H3Conn, q: &mut QuicConn) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`

*Defined at line 1444*

---

## h3_close

Close the connection without an error (H3_NO_ERROR).

```fides
pub fn h3_close(h: &mut H3Conn, q: &mut QuicConn, now: i64) {
```

**Parameters:**

- `h`: `&mut H3Conn`
- `q`: `&mut QuicConn`
- `now`: `i64`

*Defined at line 1460*

---

## h3_idle

Whether nothing is in flight: no request waiting for a response
(client) or for the application (server).

```fides
pub fn h3_idle(h: &H3Conn) -> bool {
```

**Parameters:**

- `h`: `&H3Conn`

**Returns:** `bool`

*Defined at line 1466*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
