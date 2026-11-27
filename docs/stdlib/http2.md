[All modules](index.md)

# http2 -- Fides Documentation

## Contents

- [H2_DATA:](#H2_DATA:) (const)
- [H2_HEADERS:](#H2_HEADERS:) (const)
- [H2_PRIORITY:](#H2_PRIORITY:) (const)
- [H2_RST_STREAM:](#H2_RST_STREAM:) (const)
- [H2_SETTINGS:](#H2_SETTINGS:) (const)
- [H2_PUSH_PROMISE:](#H2_PUSH_PROMISE:) (const)
- [H2_PING:](#H2_PING:) (const)
- [H2_GOAWAY:](#H2_GOAWAY:) (const)
- [H2_WINDOW_UPDATE:](#H2_WINDOW_UPDATE:) (const)
- [H2_CONTINUATION:](#H2_CONTINUATION:) (const)
- [H2_FLAG_END_STREAM:](#H2_FLAG_END_STREAM:) (const)
- [H2_FLAG_ACK:](#H2_FLAG_ACK:) (const)
- [H2_FLAG_END_HEADERS:](#H2_FLAG_END_HEADERS:) (const)
- [H2_FLAG_PADDED:](#H2_FLAG_PADDED:) (const)
- [H2_FLAG_PRIORITY:](#H2_FLAG_PRIORITY:) (const)
- [H2_NO_ERROR:](#H2_NO_ERROR:) (const)
- [H2_PROTOCOL_ERROR:](#H2_PROTOCOL_ERROR:) (const)
- [H2_INTERNAL_ERROR:](#H2_INTERNAL_ERROR:) (const)
- [H2_FLOW_CONTROL_ERROR:](#H2_FLOW_CONTROL_ERROR:) (const)
- [H2_SETTINGS_TIMEOUT:](#H2_SETTINGS_TIMEOUT:) (const)
- [H2_STREAM_CLOSED:](#H2_STREAM_CLOSED:) (const)
- [H2_FRAME_SIZE_ERROR:](#H2_FRAME_SIZE_ERROR:) (const)
- [H2_REFUSED_STREAM:](#H2_REFUSED_STREAM:) (const)
- [H2_CANCEL:](#H2_CANCEL:) (const)
- [H2_COMPRESSION_ERROR:](#H2_COMPRESSION_ERROR:) (const)
- [H2_CONNECT_ERROR:](#H2_CONNECT_ERROR:) (const)
- [H2_ENHANCE_YOUR_CALM:](#H2_ENHANCE_YOUR_CALM:) (const)
- [H2_INADEQUATE_SECURITY:](#H2_INADEQUATE_SECURITY:) (const)
- [H2_HTTP_1_1_REQUIRED:](#H2_HTTP_1_1_REQUIRED:) (const)
- [H2_SETTINGS_HEADER_TABLE_SIZE:](#H2_SETTINGS_HEADER_TABLE_SIZE:) (const)
- [H2_SETTINGS_ENABLE_PUSH:](#H2_SETTINGS_ENABLE_PUSH:) (const)
- [H2_SETTINGS_MAX_CONCURRENT_STREAMS:](#H2_SETTINGS_MAX_CONCURRENT_STREAMS:) (const)
- [H2_SETTINGS_INITIAL_WINDOW_SIZE:](#H2_SETTINGS_INITIAL_WINDOW_SIZE:) (const)
- [H2_SETTINGS_MAX_FRAME_SIZE:](#H2_SETTINGS_MAX_FRAME_SIZE:) (const)
- [H2_SETTINGS_MAX_HEADER_LIST_SIZE:](#H2_SETTINGS_MAX_HEADER_LIST_SIZE:) (const)
- [H2_IDLE:](#H2_IDLE:) (const)
- [H2_OPEN:](#H2_OPEN:) (const)
- [H2_HALF_CLOSED_LOCAL:](#H2_HALF_CLOSED_LOCAL:) (const)
- [H2_HALF_CLOSED_REMOTE:](#H2_HALF_CLOSED_REMOTE:) (const)
- [H2_CLOSED:](#H2_CLOSED:) (const)
- [H2_DEFAULT_WINDOW:](#H2_DEFAULT_WINDOW:) (const)
- [H2_MAX_WINDOW:](#H2_MAX_WINDOW:) (const)
- [H2_DEFAULT_FRAME:](#H2_DEFAULT_FRAME:) (const)
- [H2_MAX_FRAME:](#H2_MAX_FRAME:) (const)
- [H2_OUR_MAX_HEADER_LIST:](#H2_OUR_MAX_HEADER_LIST:) (const)
- [H2_SETTINGS_ACK_MS:](#H2_SETTINGS_ACK_MS:) (const)
- [h2_preface](#h2_preface) (function)
- [h2_type_name](#h2_type_name) (function)
- [h2_error_name](#h2_error_name) (function)
- [H2Frame](#H2Frame) (struct)
- [h2_frame](#h2_frame) (function)
- [h2_put_frame](#h2_put_frame) (function)
- [h2_frame_encode](#h2_frame_encode) (function)
- [h2_frame_header](#h2_frame_header) (function)
- [h2_frame_decode](#h2_frame_decode) (function)
- [h2_frames_decode](#h2_frames_decode) (function)
- [h2_data_frame](#h2_data_frame) (function)
- [H2Priority](#H2Priority) (struct)
- [h2_priority](#h2_priority) (function)
- [h2_no_priority](#h2_no_priority) (function)
- [h2_headers_frame](#h2_headers_frame) (function)
- [h2_headers_frame_ex](#h2_headers_frame_ex) (function)
- [h2_priority_frame](#h2_priority_frame) (function)
- [h2_rst_stream_frame](#h2_rst_stream_frame) (function)
- [h2_settings_frame](#h2_settings_frame) (function)
- [h2_settings_ack](#h2_settings_ack) (function)
- [h2_ping_frame](#h2_ping_frame) (function)
- [h2_goaway_frame](#h2_goaway_frame) (function)
- [h2_window_update_frame](#h2_window_update_frame) (function)
- [h2_continuation_frame](#h2_continuation_frame) (function)
- [H2Info](#H2Info) (struct)
- [h2_parse](#h2_parse) (function)
- [h2_frame_describe](#h2_frame_describe) (function)
- [H2Stream](#H2Stream) (struct)
- [H2Conn](#H2Conn) (struct)
- [h2_conn_new](#h2_conn_new) (function)
- [h2_stream_index](#h2_stream_index) (function)
- [h2_send](#h2_send) (function)
- [http2_set_tls](#http2_set_tls) (function)
- [h2_flush](#h2_flush) (function)
- [h2_read_frame](#h2_read_frame) (function)
- [h2_conn_error](#h2_conn_error) (function)
- [h2_our_settings](#h2_our_settings) (function)
- [h2_settings_overdue](#h2_settings_overdue) (function)
- [h2_conn_start](#h2_conn_start) (function)
- [h2_header_list_size](#h2_header_list_size) (function)
- [h2_process](#h2_process) (function)
- [h2_send_headers](#h2_send_headers) (function)
- [h2_pump](#h2_pump) (function)
- [h2_request](#h2_request) (function)
- [h2_response_fields](#h2_response_fields) (function)
- [h2_job_text](#h2_job_text) (function)
- [h2_job_request](#h2_job_request) (function)
- [h2_handler_task](#h2_handler_task) (function)
- [h2_collect_jobs](#h2_collect_jobs) (function)
- [http2_set_handler](#http2_set_handler) (function)
- [http2_set_handler_tasks](#http2_set_handler_tasks) (function)
- [http2_stopped](#http2_stopped) (function)
- [http2_reset_stop](#http2_reset_stop) (function)
- [h2_server_step](#h2_server_step) (function)
- [h2_b64url_encode](#h2_b64url_encode) (function)
- [h2_b64url_decode](#h2_b64url_decode) (function)
- [h2_settings_header](#h2_settings_header) (function)
- [h2_upgrade_request](#h2_upgrade_request) (function)
- [h2_h1_response](#h2_h1_response) (function)
- [http2_serve_conn](#http2_serve_conn) (function)
- [http2_serve_session](#http2_serve_session) (function)
- [h2_conn_task](#h2_conn_task) (function)
- [h2_accept_task](#h2_accept_task) (function)
- [http2_stop](#http2_stop) (function)
- [http2_listen_tasks](#http2_listen_tasks) (function)
- [http2_serve](#http2_serve) (function)
- [Http2Response](#Http2Response) (struct)
- [http2_header](#http2_header) (function)
- [http2_connect](#http2_connect) (function)
- [http2_failed](#http2_failed) (function)
- [http2_connect_session](#http2_connect_session) (function)
- [http2_request](#http2_request) (function)
- [http2_wait](#http2_wait) (function)
- [http2_close](#http2_close) (function)
- [http2_get_upgrade](#http2_get_upgrade) (function)
- [http2_get](#http2_get) (function)
- [http2_post](#http2_post) (function)
- [http2_get_many](#http2_get_many) (function)

## H2_DATA:

```fides
pub const H2_DATA: i64 = 0;
```

*Defined at line 67*

---

## H2_HEADERS:

```fides
pub const H2_HEADERS: i64 = 1;
```

*Defined at line 68*

---

## H2_PRIORITY:

```fides
pub const H2_PRIORITY: i64 = 2;
```

*Defined at line 69*

---

## H2_RST_STREAM:

```fides
pub const H2_RST_STREAM: i64 = 3;
```

*Defined at line 70*

---

## H2_SETTINGS:

```fides
pub const H2_SETTINGS: i64 = 4;
```

*Defined at line 71*

---

## H2_PUSH_PROMISE:

```fides
pub const H2_PUSH_PROMISE: i64 = 5;
```

*Defined at line 72*

---

## H2_PING:

```fides
pub const H2_PING: i64 = 6;
```

*Defined at line 73*

---

## H2_GOAWAY:

```fides
pub const H2_GOAWAY: i64 = 7;
```

*Defined at line 74*

---

## H2_WINDOW_UPDATE:

```fides
pub const H2_WINDOW_UPDATE: i64 = 8;
```

*Defined at line 75*

---

## H2_CONTINUATION:

```fides
pub const H2_CONTINUATION: i64 = 9;
```

*Defined at line 76*

---

## H2_FLAG_END_STREAM:

```fides
pub const H2_FLAG_END_STREAM: i64 = 1;
```

*Defined at line 77*

---

## H2_FLAG_ACK:

```fides
pub const H2_FLAG_ACK: i64 = 1;
```

*Defined at line 78*

---

## H2_FLAG_END_HEADERS:

```fides
pub const H2_FLAG_END_HEADERS: i64 = 4;
```

*Defined at line 79*

---

## H2_FLAG_PADDED:

```fides
pub const H2_FLAG_PADDED: i64 = 8;
```

*Defined at line 80*

---

## H2_FLAG_PRIORITY:

```fides
pub const H2_FLAG_PRIORITY: i64 = 32;
```

*Defined at line 81*

---

## H2_NO_ERROR:

```fides
pub const H2_NO_ERROR: i64 = 0;
```

*Defined at line 82*

---

## H2_PROTOCOL_ERROR:

```fides
pub const H2_PROTOCOL_ERROR: i64 = 1;
```

*Defined at line 83*

---

## H2_INTERNAL_ERROR:

```fides
pub const H2_INTERNAL_ERROR: i64 = 2;
```

*Defined at line 84*

---

## H2_FLOW_CONTROL_ERROR:

```fides
pub const H2_FLOW_CONTROL_ERROR: i64 = 3;
```

*Defined at line 85*

---

## H2_SETTINGS_TIMEOUT:

```fides
pub const H2_SETTINGS_TIMEOUT: i64 = 4;
```

*Defined at line 86*

---

## H2_STREAM_CLOSED:

```fides
pub const H2_STREAM_CLOSED: i64 = 5;
```

*Defined at line 87*

---

## H2_FRAME_SIZE_ERROR:

```fides
pub const H2_FRAME_SIZE_ERROR: i64 = 6;
```

*Defined at line 88*

---

## H2_REFUSED_STREAM:

```fides
pub const H2_REFUSED_STREAM: i64 = 7;
```

*Defined at line 89*

---

## H2_CANCEL:

```fides
pub const H2_CANCEL: i64 = 8;
```

*Defined at line 90*

---

## H2_COMPRESSION_ERROR:

```fides
pub const H2_COMPRESSION_ERROR: i64 = 9;
```

*Defined at line 91*

---

## H2_CONNECT_ERROR:

```fides
pub const H2_CONNECT_ERROR: i64 = 10;
```

*Defined at line 92*

---

## H2_ENHANCE_YOUR_CALM:

```fides
pub const H2_ENHANCE_YOUR_CALM: i64 = 11;
```

*Defined at line 93*

---

## H2_INADEQUATE_SECURITY:

```fides
pub const H2_INADEQUATE_SECURITY: i64 = 12;
```

*Defined at line 94*

---

## H2_HTTP_1_1_REQUIRED:

```fides
pub const H2_HTTP_1_1_REQUIRED: i64 = 13;
```

*Defined at line 95*

---

## H2_SETTINGS_HEADER_TABLE_SIZE:

```fides
pub const H2_SETTINGS_HEADER_TABLE_SIZE: i64 = 1;
```

*Defined at line 96*

---

## H2_SETTINGS_ENABLE_PUSH:

```fides
pub const H2_SETTINGS_ENABLE_PUSH: i64 = 2;
```

*Defined at line 97*

---

## H2_SETTINGS_MAX_CONCURRENT_STREAMS:

```fides
pub const H2_SETTINGS_MAX_CONCURRENT_STREAMS: i64 = 3;
```

*Defined at line 98*

---

## H2_SETTINGS_INITIAL_WINDOW_SIZE:

```fides
pub const H2_SETTINGS_INITIAL_WINDOW_SIZE: i64 = 4;
```

*Defined at line 99*

---

## H2_SETTINGS_MAX_FRAME_SIZE:

```fides
pub const H2_SETTINGS_MAX_FRAME_SIZE: i64 = 5;
```

*Defined at line 100*

---

## H2_SETTINGS_MAX_HEADER_LIST_SIZE:

```fides
pub const H2_SETTINGS_MAX_HEADER_LIST_SIZE: i64 = 6;
```

*Defined at line 101*

---

## H2_IDLE:

Stream states (5.1); reserved states never occur (no push).

```fides
pub const H2_IDLE: i64 = 0;
```

*Defined at line 103*

---

## H2_OPEN:

```fides
pub const H2_OPEN: i64 = 1;
```

*Defined at line 104*

---

## H2_HALF_CLOSED_LOCAL:

```fides
pub const H2_HALF_CLOSED_LOCAL: i64 = 2;
```

*Defined at line 105*

---

## H2_HALF_CLOSED_REMOTE:

```fides
pub const H2_HALF_CLOSED_REMOTE: i64 = 3;
```

*Defined at line 106*

---

## H2_CLOSED:

```fides
pub const H2_CLOSED: i64 = 4;
```

*Defined at line 107*

---

## H2_DEFAULT_WINDOW:

The default and largest window, the default frame size (6.5.2).

```fides
pub const H2_DEFAULT_WINDOW: i64 = 65535;
```

*Defined at line 109*

---

## H2_MAX_WINDOW:

```fides
pub const H2_MAX_WINDOW: i64 = 2147483647;
```

*Defined at line 110*

---

## H2_DEFAULT_FRAME:

```fides
pub const H2_DEFAULT_FRAME: i64 = 16384;
```

*Defined at line 111*

---

## H2_MAX_FRAME:

```fides
pub const H2_MAX_FRAME: i64 = 16777215;
```

*Defined at line 112*

---

## H2_OUR_MAX_HEADER_LIST:

SETTINGS_MAX_HEADER_LIST_SIZE we announce and enforce (6.5.2): the
decoded size of a field section, each field's name and value and 32.

```fides
pub const H2_OUR_MAX_HEADER_LIST: i64 = 65536;
```

*Defined at line 127*

---

## H2_SETTINGS_ACK_MS:

How long the peer has to acknowledge our SETTINGS (6.5.3), ms.

```fides
pub const H2_SETTINGS_ACK_MS: i64 = 10000;
```

*Defined at line 129*

---

## h2_preface

The client connection preface (3.4): "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n".

```fides
pub fn h2_preface() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 134*

---

## h2_type_name

Name of a frame type, for logs and tests.

```fides
pub fn h2_type_name(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 139*

---

## h2_error_name

Name of an error code (7).

```fides
pub fn h2_error_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 174*

---

## H2Frame

A frame. `length` is the payload's length (also set when only the
header was read); `ftype` -1 means no frame (not enough bytes).

```fides
pub struct H2Frame {
```

**Fields:**

| Name | Type |
|------|------|
| `length` | `i64` |
| `ftype` | `i64` |
| `flags` | `i64` |
| `stream` | `i64` |
| `payload` | `Vec<i64>` |

*Defined at line 224*

---

## h2_frame

```fides
pub fn h2_frame(ftype: i64, flags: i64, stream: i64, payload: Vec<i64>) -> H2Frame {
```

**Parameters:**

- `ftype`: `i64`
- `flags`: `i64`
- `stream`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `H2Frame`

*Defined at line 232*

---

## h2_put_frame

Append a frame's 9-byte header and payload to `out`.

```fides
pub fn h2_put_frame(out: &mut Vec<i64>, f: H2Frame) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `f`: `H2Frame`

*Defined at line 274*

---

## h2_frame_encode

A frame as bytes: header then payload.

```fides
pub fn h2_frame_encode(f: H2Frame) -> Vec<i64> {
```

**Parameters:**

- `f`: `H2Frame`

**Returns:** `Vec<i64>`

*Defined at line 286*

---

## h2_frame_header

The frame header at `at` (payload left empty): ftype -1 when fewer than
9 bytes are there. The reserved bit of the stream id is dropped (4.1).

```fides
pub fn h2_frame_header(b: Vec<i64>, at: i64) -> H2Frame {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `H2Frame`

*Defined at line 294*

---

## h2_frame_decode

The whole frame at `at`: ftype -1 when it is not all there yet.

```fides
pub fn h2_frame_decode(b: Vec<i64>, at: i64) -> H2Frame {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `H2Frame`

*Defined at line 304*

---

## h2_frames_decode

Every frame in `b` from `at`, in order (stops at an incomplete one).

```fides
pub fn h2_frames_decode(b: Vec<i64>, at: i64) -> Vec<H2Frame> {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `Vec<H2Frame>`

*Defined at line 314*

---

## h2_data_frame

DATA (6.1). `pad` > 0 adds PADDED with that many zero bytes.

```fides
pub fn h2_data_frame(stream: i64, data: Vec<i64>, end_stream: bool, pad: i64) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `data`: `Vec<i64>`
- `end_stream`: `bool`
- `pad`: `i64`

**Returns:** `H2Frame`

*Defined at line 337*

---

## H2Priority

Priority fields of HEADERS and PRIORITY (5.3, deprecated by RFC 9113 but
still on the wire). `weight` is 1..256.

```fides
pub struct H2Priority {
```

**Fields:**

| Name | Type |
|------|------|
| `exclusive` | `bool` |
| `dep` | `i64` |
| `weight` | `i64` |

*Defined at line 354*

---

## h2_priority

```fides
pub fn h2_priority(exclusive: bool, dep: i64, weight: i64) -> H2Priority {
```

**Parameters:**

- `exclusive`: `bool`
- `dep`: `i64`
- `weight`: `i64`

**Returns:** `H2Priority`

*Defined at line 360*

---

## h2_no_priority

```fides
pub fn h2_no_priority() -> H2Priority {
```

**Returns:** `H2Priority`

*Defined at line 366*

---

## h2_headers_frame

HEADERS (6.2) carrying a header block fragment.

```fides
pub fn h2_headers_frame(stream: i64, block: Vec<i64>, end_stream: bool, end_headers: bool) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `block`: `Vec<i64>`
- `end_stream`: `bool`
- `end_headers`: `bool`

**Returns:** `H2Frame`

*Defined at line 382*

---

## h2_headers_frame_ex

HEADERS with padding (`pad` > 0) and priority fields (`has_priority`).

```fides
pub fn h2_headers_frame_ex(stream: i64, block: Vec<i64>, end_stream: bool, end_headers: bool, pad: i64, has_priority: bool, pr: H2Priority) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `block`: `Vec<i64>`
- `end_stream`: `bool`
- `end_headers`: `bool`
- `pad`: `i64`
- `has_priority`: `bool`
- `pr`: `H2Priority`

**Returns:** `H2Frame`

*Defined at line 387*

---

## h2_priority_frame

PRIORITY (6.3).

```fides
pub fn h2_priority_frame(stream: i64, pr: H2Priority) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `pr`: `H2Priority`

**Returns:** `H2Frame`

*Defined at line 410*

---

## h2_rst_stream_frame

RST_STREAM (6.4).

```fides
pub fn h2_rst_stream_frame(stream: i64, code: i64) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `code`: `i64`

**Returns:** `H2Frame`

*Defined at line 417*

---

## h2_settings_frame

SETTINGS (6.5) with these identifiers and values.

```fides
pub fn h2_settings_frame(ids: Vec<i64>, vals: Vec<i64>) -> H2Frame {
```

**Parameters:**

- `ids`: `Vec<i64>`
- `vals`: `Vec<i64>`

**Returns:** `H2Frame`

*Defined at line 424*

---

## h2_settings_ack

The acknowledgement of a SETTINGS frame.

```fides
pub fn h2_settings_ack() -> H2Frame {
```

**Returns:** `H2Frame`

*Defined at line 437*

---

## h2_ping_frame

PING (6.7): 8 opaque bytes (shorter data is padded with zeros).

```fides
pub fn h2_ping_frame(opaque: Vec<i64>, ack: bool) -> H2Frame {
```

**Parameters:**

- `opaque`: `Vec<i64>`
- `ack`: `bool`

**Returns:** `H2Frame`

*Defined at line 442*

---

## h2_goaway_frame

GOAWAY (6.8).

```fides
pub fn h2_goaway_frame(last_stream: i64, code: i64, debug: str) -> H2Frame {
```

**Parameters:**

- `last_stream`: `i64`
- `code`: `i64`
- `debug`: `str`

**Returns:** `H2Frame`

*Defined at line 461*

---

## h2_window_update_frame

WINDOW_UPDATE (6.9) for a stream, or the connection (stream 0).

```fides
pub fn h2_window_update_frame(stream: i64, increment: i64) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `increment`: `i64`

**Returns:** `H2Frame`

*Defined at line 470*

---

## h2_continuation_frame

CONTINUATION (6.10).

```fides
pub fn h2_continuation_frame(stream: i64, block: Vec<i64>, end_headers: bool) -> H2Frame {
```

**Parameters:**

- `stream`: `i64`
- `block`: `Vec<i64>`
- `end_headers`: `bool`

**Returns:** `H2Frame`

*Defined at line 477*

---

## H2Info

What a frame's payload says. `ok` false: `error` is the error code and
`stream_error` tells a stream error (RST_STREAM) from a connection error
(GOAWAY). `data` is DATA's data, the header block fragment of HEADERS
and CONTINUATION, PING's opaque bytes, or GOAWAY's debug data.

```fides
pub struct H2Info {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `i64` |
| `stream_error` | `bool` |
| `why` | `str` |
| `data` | `Vec<i64>` |
| `pad` | `i64` |
| `has_priority` | `bool` |
| `priority` | `H2Priority` |
| `code` | `i64` |
| `last_stream` | `i64` |
| `increment` | `i64` |
| `ids` | `Vec<i64>` |
| `vals` | `Vec<i64>` |

*Defined at line 489*

---

## h2_parse

Parse and check a frame's payload (RFC 9113 6). An unknown type is ok
and carries nothing (it must be ignored, 5.5).

```fides
pub fn h2_parse(f: H2Frame) -> H2Info {
```

**Parameters:**

- `f`: `H2Frame`

**Returns:** `H2Info`

*Defined at line 529*

---

## h2_frame_describe

One line describing a frame (tests, logs).

```fides
pub fn h2_frame_describe(f: H2Frame) -> str {
```

**Parameters:**

- `f`: `H2Frame`

**Returns:** `str`

*Defined at line 664*

---

## H2Stream

A stream of a connection.

```fides
pub struct H2Stream {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `state` | `i64` |
| `send_window` | `i64` |
| `recv_window` | `i64` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `got_headers` | `bool` |
| `body` | `Vec<i64>` |
| `pending` | `Vec<i64>` |
| `pending_end` | `bool` |
| `handled` | `bool` |
| `reset` | `i64,  // the peer reset it with this code (-1: no)` |

*Defined at line 714*

---

## H2Conn

One HTTP/2 connection, either end.

```fides
pub struct H2Conn {
```

**Fields:**

| Name | Type |
|------|------|
| `fd` | `i64` |
| `client` | `bool` |
| `authority` | `str` |
| `ok` | `bool` |
| `inbuf` | `Vec<i64>` |
| `inpos` | `i64` |
| `out` | `Vec<i64>` |
| `enc` | `Hpack` |
| `dec` | `Hpack` |
| `peer_window` | `i64` |
| `peer_max_frame` | `i64` |
| `peer_max_streams` | `i64` |
| `send_window` | `i64` |
| `recv_window` | `i64` |
| `streams` | `Vec<H2Stream>` |
| `next_id` | `i64` |
| `last_peer` | `i64` |
| `got_settings` | `bool` |
| `goaway` | `bool` |
| `error` | `i64` |
| `why` | `str` |
| `cont_stream` | `i64` |
| `cont_flags` | `i64` |
| `cont_block` | `Vec<i64>` |
| `tls` | `i64` |
| `settings_deadline` | `i64` |
| `peer_max_header_list` | `i64` |
| `tasks` | `bool` |
| `jobs` | `Vec<i64>` |
| `job_ids` | `Vec<i64>` |

*Defined at line 736*

---

## h2_conn_new

A connection over socket `fd`. With fd -1 nothing is read or written:
frames queued by h2_send stay in `out` (tests feed h2_process by hand).

```fides
pub fn h2_conn_new(fd: i64, client: bool, authority: str) -> H2Conn {
```

**Parameters:**

- `fd`: `i64`
- `client`: `bool`
- `authority`: `str`

**Returns:** `H2Conn`

*Defined at line 794*

---

## h2_stream_index

Index of stream `id` in the table, -1 when it is not there (idle, or
closed and forgotten).

```fides
pub fn h2_stream_index(c: &H2Conn, id: i64) -> i64 {
```

**Parameters:**

- `c`: `&H2Conn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 806*

---

## h2_send

Queue a frame (written by h2_flush).

```fides
pub fn h2_send(c: &mut H2Conn, f: H2Frame) {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `f`: `H2Frame`

*Defined at line 818*

---

## http2_set_tls

For module `http2_tls`: how to read, write and close a TLS session.

```fides
pub fn http2_set_tls(recv: fn(i64, i64, i64, i64) -> i64, send: fn(i64, i64, i64) -> i64, close: fn(i64) -> i64) {
```

**Parameters:**

- `recv`: `fn(i64`
- `send`: `fn(i64`
- `close`: `fn(i64) -> i64`

*Defined at line 841*

---

## h2_flush

Write the queued frames. False when the connection failed.

```fides
pub fn h2_flush(c: &mut H2Conn) -> bool {
```

**Parameters:**

- `c`: `&mut H2Conn`

**Returns:** `bool`

*Defined at line 880*

---

## h2_read_frame

Read the next frame (ftype -1 at the end of the input, on a timeout, or
when the frame is larger than we allow: then `c.error` is set).

```fides
pub fn h2_read_frame(c: &mut H2Conn, timeout_ms: i64) -> H2Frame {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `timeout_ms`: `i64`

**Returns:** `H2Frame`

*Defined at line 949*

---

## h2_conn_error

A connection error (5.4.1): GOAWAY with the last stream the peer opened,
then the connection is done.

```fides
pub fn h2_conn_error(c: &mut H2Conn, code: i64, why: str) {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `code`: `i64`
- `why`: `str`

*Defined at line 969*

---

## h2_our_settings

The SETTINGS frame an end sends first: streams at once, the stream
window, MAX_HEADER_LIST_SIZE, and (client) ENABLE_PUSH 0.

```fides
pub fn h2_our_settings(client: bool) -> H2Frame {
```

**Parameters:**

- `client`: `bool`

**Returns:** `H2Frame`

*Defined at line 1021*

---

## h2_settings_overdue

True when our SETTINGS have waited for their acknowledgement past the
deadline at `now_ms` (the ms clock, p_clock_now_ms): then the connection
has ended with GOAWAY SETTINGS_TIMEOUT (6.5.3).

```fides
pub fn h2_settings_overdue(c: &mut H2Conn, now_ms: i64) -> bool {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 1040*

---

## h2_conn_start

Queue what an end sends first: the preface (client), our SETTINGS, and
a WINDOW_UPDATE that opens the connection window.

```fides
pub fn h2_conn_start(c: &mut H2Conn) {
```

**Parameters:**

- `c`: `&mut H2Conn`

*Defined at line 1068*

---

## h2_header_list_size

The size of a field section as SETTINGS_MAX_HEADER_LIST_SIZE counts it
(6.5.2): each field's name and value in bytes, and 32.

```fides
pub fn h2_header_list_size(names: Vec<str>, vals: Vec<str>) -> i64 {
```

**Parameters:**

- `names`: `Vec<str>`
- `vals`: `Vec<str>`

**Returns:** `i64`

*Defined at line 1170*

---

## h2_process

Handle one frame the peer sent. False when the connection is over (an
error, or GOAWAY with nothing left to finish).

```fides
pub fn h2_process(c: &mut H2Conn, f: H2Frame) -> bool {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `f`: `H2Frame`

**Returns:** `bool`

*Defined at line 1477*

---

## h2_send_headers

Queue the header block of stream `id` as HEADERS and as many
CONTINUATION frames as the peer's frame size needs.

```fides
pub fn h2_send_headers(c: &mut H2Conn, id: i64, names: Vec<str>, vals: Vec<str>, end_stream: bool) {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `id`: `i64`
- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `end_stream`: `bool`

*Defined at line 1594*

---

## h2_pump

Send what the windows allow of every stream's pending body, a frame per
stream in turn, so streams share the connection.

```fides
pub fn h2_pump(c: &mut H2Conn) {
```

**Parameters:**

- `c`: `&mut H2Conn`

*Defined at line 1636*

---

## h2_request

The request a handler gets: method and path from the pseudo-header
fields, the other fields as headers (with "host" from :authority), the
body. `version` is "2".

```fides
pub fn h2_request(s: H2Stream) -> HttpRequest {
```

**Parameters:**

- `s`: `H2Stream`

**Returns:** `HttpRequest`

*Defined at line 1694*

---

## h2_response_fields

Turn a response of module `http` (build_response: "HTTP/1.1 200 OK",
headers, body) into HTTP/2 fields and a body. Text that is not such a
response is sent as a 200 text/plain body.

```fides
pub fn h2_response_fields(resp: str, names: &mut Vec<str>, vals: &mut Vec<str>) -> str {
```

**Parameters:**

- `resp`: `str`
- `names`: `&mut Vec<str>`
- `vals`: `&mut Vec<str>`

**Returns:** `str`

*Defined at line 1736*

---

## h2_job_text

A request as the text a handler task gets (h2_job_request reads it
back): method, target, the number of fields, each field's name and
value, a line each, then the body. The fields are those h2_request
gives (no pseudo-header; "host" from :authority when absent). Field
names and values hold no LF (h2_field_ok), so the lines are safe; the
body may hold anything.

```fides
pub fn h2_job_text(s: H2Stream) -> str {
```

**Parameters:**

- `s`: `H2Stream`

**Returns:** `str`

*Defined at line 1809*

---

## h2_job_request

The request h2_job_text wrote, as h2_request gives it.

```fides
pub fn h2_job_request(t: str) -> HttpRequest {
```

**Parameters:**

- `t`: `str`

**Returns:** `HttpRequest`

*Defined at line 1842*

---

## h2_handler_task

A handler's task (task_go needs a name). `slot` is three atomic words
the connection made: the request (h2_job_text, a raw string the task
takes), the response (a raw string the task leaves for the connection),
and a flag set once the response is there. The task is detached: the
connection frees the slot.

```fides
pub fn h2_handler_task(slot: i64) -> i64 {
```

**Parameters:**

- `slot`: `i64`

**Returns:** `i64`

*Defined at line 1876*

---

## h2_collect_jobs

Send the responses of the handler tasks that have finished. Returns how
many are still running.

```fides
pub fn h2_collect_jobs(c: &mut H2Conn) -> i64 {
```

**Parameters:**

- `c`: `&mut H2Conn`

**Returns:** `i64`

*Defined at line 1912*

---

## http2_set_handler

The handler the server calls (http2_serve and http2_listen_tasks set it).

```fides
pub fn http2_set_handler(handler: fn(HttpRequest) -> str) {
```

**Parameters:**

- `handler`: `fn(HttpRequest) -> str`

*Defined at line 2007*

---

## http2_set_handler_tasks

Whether a server runs each request's handler on a task of its own (the
default: requests of one connection are handled side by side) or on the
connection's task, one at a time. For servers started after the call.

```fides
pub fn http2_set_handler_tasks(on: bool) {
```

**Parameters:**

- `on`: `bool`

*Defined at line 2015*

---

## http2_stopped

True once http2_stop() asked the servers to stop.

```fides
pub fn http2_stopped() -> bool {
```

**Returns:** `bool`

*Defined at line 2020*

---

## http2_reset_stop

Let servers run again after http2_stop() (http2_listen_tasks does it).

```fides
pub fn http2_reset_stop() {
```

*Defined at line 2025*

---

## h2_server_step

After a frame: run the handlers of complete requests, send what the
windows allow, forget finished streams.

```fides
pub fn h2_server_step(c: &mut H2Conn) {
```

**Parameters:**

- `c`: `&mut H2Conn`

*Defined at line 2031*

---

## h2_b64url_encode

Bytes in base64url without padding (RFC 4648 section 5), as
HTTP2-Settings carries them.

```fides
pub fn h2_b64url_encode(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 2152*

---

## h2_b64url_decode

Decode base64url (padding '=' allowed at the end) into `out`. False on a
character outside the alphabet or a length no encoding gives.

```fides
pub fn h2_b64url_decode(s: str, out: &mut Vec<i64>) -> bool {
```

**Parameters:**

- `s`: `str`
- `out`: `&mut Vec<i64>`

**Returns:** `bool`

*Defined at line 2181*

---

## h2_settings_header

The value of HTTP2-Settings for an h2c Upgrade: our SETTINGS payload in
base64url (RFC 7540 3.2.1).

```fides
pub fn h2_settings_header(client: bool) -> str {
```

**Parameters:**

- `client`: `bool`

**Returns:** `str`

*Defined at line 2210*

---

## h2_upgrade_request

An HTTP/1.1 request on a cleartext server connection that asks for the
h2c Upgrade (RFC 7540 3.2): an Upgrade field with the token "h2c", a
Connection field with "Upgrade" and "HTTP2-Settings", and exactly one
HTTP2-Settings field holding a valid SETTINGS payload in base64url.
Then "HTTP/1.1 101 Switching Protocols" and our SETTINGS (the server's
connection preface) are queued, the HTTP2-Settings applied (the 101
acknowledges them: no ACK), and the request with `body` becomes stream
1, half-closed (remote), answered once the client's preface is in.
False, with nothing queued, when it does not ask or asks wrongly: the
caller answers it over HTTP/1.1.

```fides
pub fn h2_upgrade_request(c: &mut H2Conn, req: HttpRequest, body: Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `req`: `HttpRequest`
- `body`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 2224*

---

## h2_h1_response

A handler's response (module http's text, as h2_response_fields takes)
as an HTTP/1.1 response that closes the connection: "Connection: close"
added, no body for a HEAD request.

```fides
pub fn h2_h1_response(resp: str, head: bool) -> str {
```

**Parameters:**

- `resp`: `str`
- `head`: `bool`

**Returns:** `str`

*Defined at line 2306*

---

## http2_serve_conn

Serve one accepted cleartext connection until it closes: the preface
(prior knowledge), then frames of any number of streams. Before the
preface an HTTP/1.1 request may come instead: one asking for the h2c
Upgrade becomes stream 1 of an HTTP/2 connection (RFC 7540 3.2), any
other is answered over HTTP/1.1 and the connection closes. Returns 0.

```fides
pub fn http2_serve_conn(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 2443*

---

## http2_serve_session

Serve HTTP/2 on an accepted connection whose TLS session `tls` chose
"h2" by ALPN (module http2_tls): the preface, then frames, until it
closes. The session and the socket are closed at the end. Returns 0.

```fides
pub fn http2_serve_session(fd: i64, tls: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`

**Returns:** `i64`

*Defined at line 2476*

---

## h2_conn_task

A connection's task (task_go needs a name).

```fides
pub fn h2_conn_task(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 2491*

---

## h2_accept_task

Take connections off the listening socket, each its own task, until
http2_stop().

```fides
pub fn h2_accept_task(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 2497*

---

## http2_stop

Ask the server to stop accepting; connections finish their streams.

```fides
pub fn http2_stop() {
```

*Defined at line 2511*

---

## http2_listen_tasks

Serve a listening socket from tasks and return: they run whenever the
caller waits (task_sleep, task_run, a client call).

```fides
pub fn http2_listen_tasks(fd: i64, handler: fn(HttpRequest) -> str) {
```

**Parameters:**

- `fd`: `i64`
- `handler`: `fn(HttpRequest) -> str`

*Defined at line 2517*

---

## http2_serve

Serve HTTP/2 (h2c, prior knowledge) on `port` with a handler of module
`http`, until http2_stop(). False if the port cannot be opened.

```fides
pub fn http2_serve(port: i64, handler: fn(HttpRequest) -> str) -> bool {
```

**Parameters:**

- `port`: `i64`
- `handler`: `fn(HttpRequest) -> str`

**Returns:** `bool`

*Defined at line 2525*

---

## Http2Response

A response. `status` 0: no response (`error` says why).

```fides
pub struct Http2Response {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `body` | `str` |
| `stream` | `i64` |
| `error` | `str` |

*Defined at line 2538*

---

## http2_header

A response header (name in lowercase), "" when absent.

```fides
pub fn http2_header(r: Http2Response, name: str) -> str {
```

**Parameters:**

- `r`: `Http2Response`
- `name`: `str`

**Returns:** `str`

*Defined at line 2554*

---

## http2_connect

Connect to `host`:`port` and start HTTP/2 with prior knowledge: the
preface and our SETTINGS go out at once. `ok` false on failure.

```fides
pub fn http2_connect(host: str, port: i64) -> H2Conn {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `H2Conn`

*Defined at line 2567*

---

## http2_failed

A connection that failed before it started: `ok` false, `why` says why.

```fides
pub fn http2_failed(why: str) -> H2Conn {
```

**Parameters:**

- `why`: `str`

**Returns:** `H2Conn`

*Defined at line 2581*

---

## http2_connect_session

Start HTTP/2 as a client on the connected socket `fd` whose TLS session
`tls` chose "h2" by ALPN (module http2_tls): the preface and our
SETTINGS go out at once, requests say :scheme https. The connection owns
both (http2_close closes them).

```fides
pub fn http2_connect_session(fd: i64, tls: i64, host: str, port: i64) -> H2Conn {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`
- `host`: `str`
- `port`: `i64`

**Returns:** `H2Conn`

*Defined at line 2592*

---

## http2_request

Open a stream with a request; returns its id (-1 if the connection
cannot take one, or the request's fields are larger than the peer's
SETTINGS_MAX_HEADER_LIST_SIZE). `names`/`vals` are extra fields
(lowercase names).

```fides
pub fn http2_request(c: &mut H2Conn, method: str, path: str, names: Vec<str>, vals: Vec<str>, body: str) -> i64 {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `method`: `str`
- `path`: `str`
- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `body`: `str`

**Returns:** `i64`

*Defined at line 2604*

---

## http2_wait

Read frames until stream `id` has its whole response, and return it
(the stream is then forgotten).

```fides
pub fn http2_wait(c: &mut H2Conn, id: i64) -> Http2Response {
```

**Parameters:**

- `c`: `&mut H2Conn`
- `id`: `i64`

**Returns:** `Http2Response`

*Defined at line 2665*

---

## http2_close

Say goodbye (GOAWAY) and close the socket (and the TLS session).

```fides
pub fn http2_close(c: &mut H2Conn) {
```

**Parameters:**

- `c`: `&mut H2Conn`

*Defined at line 2715*

---

## http2_get_upgrade

GET `path` from `host`:`port` starting with HTTP/1.1 and the h2c
Upgrade (RFC 7540 3.2): "Upgrade: h2c" with our SETTINGS in
HTTP2-Settings. On "101 Switching Protocols" the request is stream 1 of
an HTTP/2 connection and its response comes over HTTP/2 (`stream` 1);
a server without HTTP/2 answers over HTTP/1.1, and that answer is
returned (`stream` 0).

```fides
pub fn http2_get_upgrade(host: str, port: i64, path: str) -> Http2Response {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `path`: `str`

**Returns:** `Http2Response`

*Defined at line 2751*

---

## http2_get

GET `path` from `host`:`port` over h2c with prior knowledge.

```fides
pub fn http2_get(host: str, port: i64, path: str) -> Http2Response {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `path`: `str`

**Returns:** `Http2Response`

*Defined at line 2822*

---

## http2_post

POST `body` to `path`.

```fides
pub fn http2_post(host: str, port: i64, path: str, content_type: str, body: str) -> Http2Response {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `path`: `str`
- `content_type`: `str`
- `body`: `str`

**Returns:** `Http2Response`

*Defined at line 2834*

---

## http2_get_many

GET every path at once on one connection (each its own stream, all sent
before any response is read); the responses in the order of `paths`.

```fides
pub fn http2_get_many(host: str, port: i64, paths: Vec<str>) -> Vec<Http2Response> {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `paths`: `Vec<str>`

**Returns:** `Vec<Http2Response>`

*Defined at line 2851*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
