[All modules](index.md)

# ws -- Fides Documentation

## Contents

- [ws_sha1](#ws_sha1) (function)
- [WsConn](#WsConn) (struct)
- [WsFrame](#WsFrame) (struct)
- [ws_ok](#ws_ok) (function)
- [ws_failed](#ws_failed) (function)
- [ws_accept_key](#ws_accept_key) (function)
- [ws_set_tls](#ws_set_tls) (function)
- [ws_connect](#ws_connect) (function)
- [ws_handshake](#ws_handshake) (function)
- [ws_accept](#ws_accept) (function)
- [ws_read_request](#ws_read_request) (function)
- [ws_accept_on](#ws_accept_on) (function)
- [ws_frame_encode](#ws_frame_encode) (function)
- [ws_recv](#ws_recv) (function)
- [ws_send_text](#ws_send_text) (function)
- [ws_send_binary](#ws_send_binary) (function)
- [ws_send_ping](#ws_send_ping) (function)
- [ws_send_pong](#ws_send_pong) (function)
- [ws_close](#ws_close) (function)

## ws_sha1

SHA-1 of a byte Vec → 20-byte Vec.

```fides
pub fn ws_sha1(bytes: Vec) -> Vec {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `Vec`

*Defined at line 36*

---

## WsConn

A WebSocket connection. `fd` < 0 means handshake/connect failed.

```fides
pub struct WsConn {
```

**Fields:**

| Name | Type |
|------|------|
| `fd` | `i64` |
| `is_client` | `bool` |
| `open` | `bool` |
| `tls` | `i64,  // the TLS session (wss://), 0 for plain ws://` |

*Defined at line 140*

---

## WsFrame

A received frame/message.

```fides
pub struct WsFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `opcode` | `i64` |
| `text` | `str` |
| `bytes` | `Vec` |
| `fin` | `bool` |

*Defined at line 148*

---

## ws_ok

```fides
pub fn ws_ok(c: WsConn) -> bool {
```

**Parameters:**

- `c`: `WsConn`

**Returns:** `bool`

*Defined at line 158*

---

## ws_failed

A connection that failed (fd -1), for modules that open their own
(module `wss`).

```fides
pub fn ws_failed(is_client: bool) -> WsConn {
```

**Parameters:**

- `is_client`: `bool`

**Returns:** `WsConn`

*Defined at line 164*

---

## ws_accept_key

The RFC 6455 accept key for a given Sec-WebSocket-Key.

```fides
pub fn ws_accept_key(key: str) -> str {
```

**Parameters:**

- `key`: `str`

**Returns:** `str`

*Defined at line 173*

---

## ws_set_tls

For module `wss`: how to read, write and close a TLS session.

```fides
pub fn ws_set_tls(recv: fn(i64, i64, i64, i64) -> i64, send: fn(i64, i64, i64) -> i64, close: fn(i64) -> i64) {
```

**Parameters:**

- `recv`: `fn(i64`
- `send`: `fn(i64`
- `close`: `fn(i64) -> i64`

*Defined at line 209*

---

## ws_connect

Client-side connect: TCP + Upgrade handshake.
`host` may be a hostname or IP. Returns WsConn with fd<0 on failure.

```fides
pub fn ws_connect(host: str, port: i64, path: str) -> WsConn {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `path`: `str`

**Returns:** `WsConn`

*Defined at line 269*

---

## ws_handshake

The client's Upgrade handshake on a connected socket, or over the TLS
session `tls` on it (module `wss`). Returns WsConn with fd<0 on failure,
the socket then closed.

```fides
pub fn ws_handshake(fd: i64, tls: i64, host: str, path: str) -> WsConn {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`
- `host`: `str`
- `path`: `str`

**Returns:** `WsConn`

*Defined at line 284*

---

## ws_accept

Server-side accept: given an accepted socket fd and the client's
HTTP request text (read by the caller), validate the Upgrade and
send the 101 response. Returns WsConn with fd<0 on failure.

```fides
pub fn ws_accept(fd: i64, request: str) -> WsConn {
```

**Parameters:**

- `fd`: `i64`
- `request`: `str`

**Returns:** `WsConn`

*Defined at line 308*

---

## ws_read_request

The request of a client that wants to upgrade: its headers, read from
the socket or the TLS session.

```fides
pub fn ws_read_request(fd: i64, tls: i64) -> str {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`

**Returns:** `str`

*Defined at line 314*

---

## ws_accept_on

ws_accept over the TLS session `tls` on `fd` (0: none).

```fides
pub fn ws_accept_on(fd: i64, tls: i64, request: str) -> WsConn {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`
- `request`: `str`

**Returns:** `WsConn`

*Defined at line 319*

---

## ws_frame_encode

Encode a frame. Clients must mask (RFC 6455 §5.3); servers must not.

```fides
pub fn ws_frame_encode(opcode: i64, payload: Vec, mask: bool) -> Vec {
```

**Parameters:**

- `opcode`: `i64`
- `payload`: `Vec`
- `mask`: `bool`

**Returns:** `Vec`

*Defined at line 364*

---

## ws_recv

Receive one message; opcode -1 on connection failure/close.
Continuation frames are accumulated so a fragmented message
arrives as one WsFrame. A ping in the middle of one is answered and a
pong skipped; a close there ends it (opcode 8). A control frame before
any data frame is returned as it is.

```fides
pub fn ws_recv(c: WsConn) -> WsFrame {
```

**Parameters:**

- `c`: `WsConn`

**Returns:** `WsFrame`

*Defined at line 469*

---

## ws_send_text

Send a text message.

```fides
pub fn ws_send_text(c: WsConn, text: str) -> i64 {
```

**Parameters:**

- `c`: `WsConn`
- `text`: `str`

**Returns:** `i64`

*Defined at line 587*

---

## ws_send_binary

Send a binary message.

```fides
pub fn ws_send_binary(c: WsConn, data: Vec) -> i64 {
```

**Parameters:**

- `c`: `WsConn`
- `data`: `Vec`

**Returns:** `i64`

*Defined at line 592*

---

## ws_send_ping

Send a ping.

```fides
pub fn ws_send_ping(c: WsConn, text: str) -> i64 {
```

**Parameters:**

- `c`: `WsConn`
- `text`: `str`

**Returns:** `i64`

*Defined at line 597*

---

## ws_send_pong

Reply to a ping (callers handle it automatically in ws_recv_loop).

```fides
pub fn ws_send_pong(c: WsConn, payload: Vec) -> i64 {
```

**Parameters:**

- `c`: `WsConn`
- `payload`: `Vec`

**Returns:** `i64`

*Defined at line 602*

---

## ws_close

Close handshake + socket close.

```fides
pub fn ws_close(c: WsConn) -> void {
```

**Parameters:**

- `c`: `WsConn`

**Returns:** `void`

*Defined at line 607*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
