[All modules](index.md)

# grpc -- Fides Documentation

## Contents

- [GRPC_OK:](#GRPC_OK:) (const)
- [GRPC_CANCELLED:](#GRPC_CANCELLED:) (const)
- [GRPC_UNKNOWN:](#GRPC_UNKNOWN:) (const)
- [GRPC_INVALID_ARGUMENT:](#GRPC_INVALID_ARGUMENT:) (const)
- [GRPC_DEADLINE_EXCEEDED:](#GRPC_DEADLINE_EXCEEDED:) (const)
- [GRPC_NOT_FOUND:](#GRPC_NOT_FOUND:) (const)
- [GRPC_ALREADY_EXISTS:](#GRPC_ALREADY_EXISTS:) (const)
- [GRPC_PERMISSION_DENIED:](#GRPC_PERMISSION_DENIED:) (const)
- [GRPC_RESOURCE_EXHAUSTED:](#GRPC_RESOURCE_EXHAUSTED:) (const)
- [GRPC_FAILED_PRECONDITION:](#GRPC_FAILED_PRECONDITION:) (const)
- [GRPC_ABORTED:](#GRPC_ABORTED:) (const)
- [GRPC_OUT_OF_RANGE:](#GRPC_OUT_OF_RANGE:) (const)
- [GRPC_UNIMPLEMENTED:](#GRPC_UNIMPLEMENTED:) (const)
- [GRPC_INTERNAL:](#GRPC_INTERNAL:) (const)
- [GRPC_UNAVAILABLE:](#GRPC_UNAVAILABLE:) (const)
- [GRPC_DATA_LOSS:](#GRPC_DATA_LOSS:) (const)
- [GRPC_UNAUTHENTICATED:](#GRPC_UNAUTHENTICATED:) (const)
- [GRPC_MAX_MESSAGE:](#GRPC_MAX_MESSAGE:) (const)
- [grpc_status_name](#grpc_status_name) (function)
- [grpc_status_from_http](#grpc_status_from_http) (function)
- [grpc_frame](#grpc_frame) (function)
- [GrpcDeframer](#GrpcDeframer) (struct)
- [grpc_deframer_new](#grpc_deframer_new) (function)
- [grpc_deframer_feed](#grpc_deframer_feed) (function)
- [grpc_deframer_pending](#grpc_deframer_pending) (function)
- [grpc_message_encode](#grpc_message_encode) (function)
- [grpc_message_decode](#grpc_message_decode) (function)
- [grpc_timeout_encode](#grpc_timeout_encode) (function)
- [grpc_timeout_parse](#grpc_timeout_parse) (function)
- [GrpcRequest](#GrpcRequest) (struct)
- [grpc_unary_request](#grpc_unary_request) (function)
- [grpc_request_set_timeout](#grpc_request_set_timeout) (function)
- [grpc_request_add_metadata](#grpc_request_add_metadata) (function)
- [grpc_request_gzip](#grpc_request_gzip) (function)
- [grpc_client_frames](#grpc_client_frames) (function)
- [GrpcResponse](#GrpcResponse) (struct)
- [grpc_client_read](#grpc_client_read) (function)
- [grpc_response_reader](#grpc_response_reader) (function)
- [GrpcCall](#GrpcCall) (struct)
- [GrpcReply](#GrpcReply) (struct)
- [grpc_reply_new](#grpc_reply_new) (function)
- [grpc_reply_ok](#grpc_reply_ok) (function)
- [grpc_reply_error](#grpc_reply_error) (function)
- [grpc_call_header](#grpc_call_header) (function)
- [GrpcServer](#GrpcServer) (struct)
- [grpc_server_new](#grpc_server_new) (function)
- [grpc_server_add](#grpc_server_add) (function)
- [grpc_server_services](#grpc_server_services) (function)
- [GrpcServerResponse](#GrpcServerResponse) (struct)
- [grpc_server_handle](#grpc_server_handle) (function)
- [grpc_server_response_frames](#grpc_server_response_frames) (function)
- [grpc_server_frames](#grpc_server_frames) (function)
- [grpc_frames_bytes](#grpc_frames_bytes) (function)

## GRPC_OK:

```fides
pub const GRPC_OK: i64 = 0;
```

*Defined at line 51*

---

## GRPC_CANCELLED:

```fides
pub const GRPC_CANCELLED: i64 = 1;
```

*Defined at line 52*

---

## GRPC_UNKNOWN:

```fides
pub const GRPC_UNKNOWN: i64 = 2;
```

*Defined at line 53*

---

## GRPC_INVALID_ARGUMENT:

```fides
pub const GRPC_INVALID_ARGUMENT: i64 = 3;
```

*Defined at line 54*

---

## GRPC_DEADLINE_EXCEEDED:

```fides
pub const GRPC_DEADLINE_EXCEEDED: i64 = 4;
```

*Defined at line 55*

---

## GRPC_NOT_FOUND:

```fides
pub const GRPC_NOT_FOUND: i64 = 5;
```

*Defined at line 56*

---

## GRPC_ALREADY_EXISTS:

```fides
pub const GRPC_ALREADY_EXISTS: i64 = 6;
```

*Defined at line 57*

---

## GRPC_PERMISSION_DENIED:

```fides
pub const GRPC_PERMISSION_DENIED: i64 = 7;
```

*Defined at line 58*

---

## GRPC_RESOURCE_EXHAUSTED:

```fides
pub const GRPC_RESOURCE_EXHAUSTED: i64 = 8;
```

*Defined at line 59*

---

## GRPC_FAILED_PRECONDITION:

```fides
pub const GRPC_FAILED_PRECONDITION: i64 = 9;
```

*Defined at line 60*

---

## GRPC_ABORTED:

```fides
pub const GRPC_ABORTED: i64 = 10;
```

*Defined at line 61*

---

## GRPC_OUT_OF_RANGE:

```fides
pub const GRPC_OUT_OF_RANGE: i64 = 11;
```

*Defined at line 62*

---

## GRPC_UNIMPLEMENTED:

```fides
pub const GRPC_UNIMPLEMENTED: i64 = 12;
```

*Defined at line 63*

---

## GRPC_INTERNAL:

```fides
pub const GRPC_INTERNAL: i64 = 13;
```

*Defined at line 64*

---

## GRPC_UNAVAILABLE:

```fides
pub const GRPC_UNAVAILABLE: i64 = 14;
```

*Defined at line 65*

---

## GRPC_DATA_LOSS:

```fides
pub const GRPC_DATA_LOSS: i64 = 15;
```

*Defined at line 66*

---

## GRPC_UNAUTHENTICATED:

```fides
pub const GRPC_UNAUTHENTICATED: i64 = 16;
```

*Defined at line 67*

---

## GRPC_MAX_MESSAGE:

The default largest message accepted (4 MiB, as gRPC implementations).

```fides
pub const GRPC_MAX_MESSAGE: i64 = 4194304;
```

*Defined at line 70*

---

## grpc_status_name

```fides
pub fn grpc_status_name(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 72*

---

## grpc_status_from_http

The status of a response that has an HTTP status but no grpc-status
(the spec's "HTTP to gRPC Status Code Mapping").

```fides
pub fn grpc_status_from_http(http_status: i64) -> i64 {
```

**Parameters:**

- `http_status`: `i64`

**Returns:** `i64`

*Defined at line 129*

---

## grpc_frame

A Length-Prefixed-Message: flag (1 = compressed), length, bytes.

```fides
pub fn grpc_frame(msg: Vec<i64>, compressed: bool) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`
- `compressed`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 151*

---

## GrpcDeframer

Takes the bytes of a stream's DATA frames as they come and gives the
whole messages. ok false on a bad flag byte or a length over `max`.

```fides
pub struct GrpcDeframer {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `Vec<i64>` |
| `messages` | `Vec<Vec<i64>>` |
| `compressed` | `Vec<bool>` |
| `max` | `i64` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 173*

---

## grpc_deframer_new

```fides
pub fn grpc_deframer_new(max: i64) -> GrpcDeframer {
```

**Parameters:**

- `max`: `i64`

**Returns:** `GrpcDeframer`

*Defined at line 182*

---

## grpc_deframer_feed

```fides
pub fn grpc_deframer_feed(d: &mut GrpcDeframer, bytes: Vec<i64>) {
```

**Parameters:**

- `d`: `&mut GrpcDeframer`
- `bytes`: `Vec<i64>`

*Defined at line 188*

---

## grpc_deframer_pending

Bytes held that are not a whole message yet (non-zero at the end of a
stream means it was cut).

```fides
pub fn grpc_deframer_pending(d: &GrpcDeframer) -> i64 {
```

**Parameters:**

- `d`: `&GrpcDeframer`

**Returns:** `i64`

*Defined at line 237*

---

## grpc_message_encode

Percent-encode a status message: bytes outside 0x20..0x7E and '%'.

```fides
pub fn grpc_message_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 244*

---

## grpc_message_decode

Decode a grpc-message value; a bad escape is kept as it is (as the
spec asks of receivers).

```fides
pub fn grpc_message_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 277*

---

## grpc_timeout_encode

A grpc-timeout value for `ms` milliseconds: "<n>m" while it fits in 8
digits, else seconds, minutes, hours.

```fides
pub fn grpc_timeout_encode(ms: i64) -> str {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `str`

*Defined at line 296*

---

## grpc_timeout_parse

A grpc-timeout value in milliseconds (rounded up), or -1 when bad.

```fides
pub fn grpc_timeout_parse(v: str) -> i64 {
```

**Parameters:**

- `v`: `str`

**Returns:** `i64`

*Defined at line 312*

---

## GrpcRequest

A call as headers and message bytes (before framing into HTTP/2).

```fides
pub struct GrpcRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `body` | `Vec<i64>` |

*Defined at line 352*

---

## grpc_unary_request

A unary request: `msg` is the protobuf encoding of the request message.

```fides
pub fn grpc_unary_request(path: str, authority: str, msg: Vec<i64>) -> GrpcRequest {
```

**Parameters:**

- `path`: `str`
- `authority`: `str`
- `msg`: `Vec<i64>`

**Returns:** `GrpcRequest`

*Defined at line 359*

---

## grpc_request_set_timeout

```fides
pub fn grpc_request_set_timeout(r: &mut GrpcRequest, ms: i64) {
```

**Parameters:**

- `r`: `&mut GrpcRequest`
- `ms`: `i64`

*Defined at line 378*

---

## grpc_request_add_metadata

Custom metadata (lowercase name; a "-bin" name takes base64 values).

```fides
pub fn grpc_request_add_metadata(r: &mut GrpcRequest, name: str, value: str) {
```

**Parameters:**

- `r`: `&mut GrpcRequest`
- `name`: `str`
- `value`: `str`

*Defined at line 384*

---

## grpc_request_gzip

Send the message gzip-compressed (grpc-encoding: gzip).

```fides
pub fn grpc_request_gzip(r: &mut GrpcRequest, msg: Vec<i64>) {
```

**Parameters:**

- `r`: `&mut GrpcRequest`
- `msg`: `Vec<i64>`

*Defined at line 390*

---

## grpc_client_frames

The client's frames for a request on `stream`: HEADERS, then DATA with
END_STREAM. `max_frame` is the peer's SETTINGS_MAX_FRAME_SIZE.

```fides
pub fn grpc_client_frames(enc: &mut Hpack, stream: i64, r: GrpcRequest, max_frame: i64) -> Vec<H2Frame> {
```

**Parameters:**

- `enc`: `&mut Hpack`
- `stream`: `i64`
- `r`: `GrpcRequest`
- `max_frame`: `i64`

**Returns:** `Vec<H2Frame>`

*Defined at line 453*

---

## GrpcResponse

What a client received. status is the gRPC status (from grpc-status,
or mapped from :status when there is none); messages are decompressed.

```fides
pub struct GrpcResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `http_status` | `i64` |
| `status` | `i64` |
| `message` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `trailer_names` | `Vec<str>` |
| `trailer_vals` | `Vec<str>` |
| `messages` | `Vec<Vec<i64>>` |
| `trailers_only` | `bool` |

*Defined at line 462*

---

## grpc_client_read

Read a response from the frames of its stream (in the order received).

```fides
pub fn grpc_client_read(dec: &mut Hpack, frames: Vec<H2Frame>) -> GrpcResponse {
```

**Parameters:**

- `dec`: `&mut Hpack`
- `frames`: `Vec<H2Frame>`

**Returns:** `GrpcResponse`

*Defined at line 568*

---

## grpc_response_reader

A reader over the response's first message (for a unary call).

```fides
pub fn grpc_response_reader(r: &GrpcResponse) -> PbReader {
```

**Parameters:**

- `r`: `&GrpcResponse`

**Returns:** `PbReader`

*Defined at line 653*

---

## GrpcCall

One call as a handler sees it. deadline_ms: the grpc-timeout in ms, or
-1 when the client set none.

```fides
pub struct GrpcCall {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `service` | `str` |
| `method` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `message` | `Vec<i64>` |
| `deadline_ms` | `i64` |

*Defined at line 665*

---

## GrpcReply

What a handler answers: status, message, the response message's bytes
and extra trailers (and initial metadata in names/vals).

```fides
pub struct GrpcReply {
```

**Fields:**

| Name | Type |
|------|------|
| `status` | `i64` |
| `message` | `str` |
| `body` | `Vec<i64>` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `trailer_names` | `Vec<str>` |
| `trailer_vals` | `Vec<str>` |

*Defined at line 677*

---

## grpc_reply_new

```fides
pub fn grpc_reply_new() -> GrpcReply {
```

**Returns:** `GrpcReply`

*Defined at line 687*

---

## grpc_reply_ok

Answer OK with a protobuf message.

```fides
pub fn grpc_reply_ok(r: &mut GrpcReply, w: PbWriter) {
```

**Parameters:**

- `r`: `&mut GrpcReply`
- `w`: `PbWriter`

*Defined at line 694*

---

## grpc_reply_error

```fides
pub fn grpc_reply_error(r: &mut GrpcReply, code: i64, message: str) {
```

**Parameters:**

- `r`: `&mut GrpcReply`
- `code`: `i64`
- `message`: `str`

*Defined at line 699*

---

## grpc_call_header

```fides
pub fn grpc_call_header(c: &GrpcCall, name: str) -> str {
```

**Parameters:**

- `c`: `&GrpcCall`
- `name`: `str`

**Returns:** `str`

*Defined at line 704*

---

## GrpcServer

```fides
pub struct GrpcServer {
```

**Fields:**

| Name | Type |
|------|------|
| `methods` | `Vec<GrpcMethod>` |
| `max_message` | `i64` |
| `calls` | `i64` |

*Defined at line 714*

---

## grpc_server_new

```fides
pub fn grpc_server_new() -> GrpcServer {
```

**Returns:** `GrpcServer`

*Defined at line 720*

---

## grpc_server_add

Register a handler for "/service/method" (service with its package:
"helloworld.Greeter"). The handler fills the reply; its result is not
used.

```fides
pub fn grpc_server_add(s: &mut GrpcServer, service: str, method: str, handler: fn(&GrpcCall, &mut GrpcReply) -> bool) {
```

**Parameters:**

- `s`: `&mut GrpcServer`
- `service`: `str`
- `method`: `str`
- `handler`: `fn(&GrpcCall`

*Defined at line 729*

---

## grpc_server_services

The services registered, each once, in order.

```fides
pub fn grpc_server_services(s: &GrpcServer) -> Vec<str> {
```

**Parameters:**

- `s`: `&GrpcServer`

**Returns:** `Vec<str>`

*Defined at line 736*

---

## GrpcServerResponse

The server's answer, before HTTP/2 framing: http_status other than 200
means a plain HTTP error. trailers_only: no DATA, the status goes in the
one header block.

```fides
pub struct GrpcServerResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `http_status` | `i64` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `body` | `Vec<i64>` |
| `trailer_names` | `Vec<str>` |
| `trailer_vals` | `Vec<str>` |
| `trailers_only` | `bool` |
| `status` | `i64` |

*Defined at line 760*

---

## grpc_server_handle

Dispatch a request given as its header list and DATA bytes.

```fides
pub fn grpc_server_handle(s: &mut GrpcServer, names: Vec<str>, vals: Vec<str>, data: Vec<i64>) -> GrpcServerResponse {
```

**Parameters:**

- `s`: `&mut GrpcServer`
- `names`: `Vec<str>`
- `vals`: `Vec<str>`
- `data`: `Vec<i64>`

**Returns:** `GrpcServerResponse`

*Defined at line 798*

---

## grpc_server_response_frames

The frames of a server response on `stream`.

```fides
pub fn grpc_server_response_frames(enc: &mut Hpack, stream: i64, r: GrpcServerResponse, max_frame: i64) -> Vec<H2Frame> {
```

**Parameters:**

- `enc`: `&mut Hpack`
- `stream`: `i64`
- `r`: `GrpcServerResponse`
- `max_frame`: `i64`

**Returns:** `Vec<H2Frame>`

*Defined at line 910*

---

## grpc_server_frames

Serve one stream: its request frames in, the response frames out.
A stream that does not end, or whose frames are malformed, is reset
(RST_STREAM PROTOCOL_ERROR).

```fides
pub fn grpc_server_frames(s: &mut GrpcServer, dec: &mut Hpack, enc: &mut Hpack, frames: Vec<H2Frame>, max_frame: i64) -> Vec<H2Frame> {
```

**Parameters:**

- `s`: `&mut GrpcServer`
- `dec`: `&mut Hpack`
- `enc`: `&mut Hpack`
- `frames`: `Vec<H2Frame>`
- `max_frame`: `i64`

**Returns:** `Vec<H2Frame>`

*Defined at line 925*

---

## grpc_frames_bytes

Frames as wire bytes (to write to the connection).

```fides
pub fn grpc_frames_bytes(frames: Vec<H2Frame>) -> Vec<i64> {
```

**Parameters:**

- `frames`: `Vec<H2Frame>`

**Returns:** `Vec<i64>`

*Defined at line 946*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
