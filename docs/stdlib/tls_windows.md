[All modules](index.md)

# tls -- Fides Documentation

## Contents

- [tls_client](#tls_client) (function)
- [tls_connect](#tls_connect) (function)
- [tls_client13](#tls_client13) (function)
- [tls_connect13](#tls_connect13) (function)
- [tls_connect_unchecked](#tls_connect_unchecked) (function)
- [tls_connect13_unchecked](#tls_connect13_unchecked) (function)
- [tls_client_alpn](#tls_client_alpn) (function)
- [tls_connect_alpn](#tls_connect_alpn) (function)
- [tls_connect_alpn_unchecked](#tls_connect_alpn_unchecked) (function)
- [tls_server](#tls_server) (function)
- [tls_server13](#tls_server13) (function)
- [tls_server_close](#tls_server_close) (function)
- [tls_library](#tls_library) (function)
- [tls_accept](#tls_accept) (function)
- [tls_server_alpn](#tls_server_alpn) (function)
- [tls_session_alpn](#tls_session_alpn) (function)
- [tls_session_fd](#tls_session_fd) (function)
- [tls_session_protocol](#tls_session_protocol) (function)
- [tls_session_version](#tls_session_version) (function)
- [tls_session_send](#tls_session_send) (function)
- [tls_session_recv](#tls_session_recv) (function)
- [tls_session_close](#tls_session_close) (function)

## tls_client

A TLS session over the connected socket `fd`, checking `host`'s certificate.
Returns 0 on failure.

```fides
pub fn tls_client(fd: i64, host: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `host`: `str`

**Returns:** `i64`

*Defined at line 662*

---

## tls_connect

Connect to host:port. The session owns the socket. Returns 0 on failure.

```fides
pub fn tls_connect(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 668*

---

## tls_client13

A TLS 1.3-only session over the connected socket `fd`, checking `host`'s
certificate. Needs Windows 11 / Server 2022+. Returns 0 on failure.
tls_session_protocol on a success returns SP_PROT_TLS1_3_CLIENT (8192).

```fides
pub fn tls_client13(fd: i64, host: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `host`: `str`

**Returns:** `i64`

*Defined at line 683*

---

## tls_connect13

As tls_connect, but pinned to TLS 1.3. The session owns the socket.
Returns 0 when the OS or peer lacks TLS 1.3.

```fides
pub fn tls_connect13(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 689*

---

## tls_connect_unchecked

As tls_connect, but accept any certificate. For tests of a local server only:
it does not know whom it is talking to.

```fides
pub fn tls_connect_unchecked(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 703*

---

## tls_connect13_unchecked

As tls_connect13, but accept any certificate: for checking a local
TLS 1.3 server (tls_server13) with a self-signed certificate only.

```fides
pub fn tls_connect13_unchecked(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 718*

---

## tls_client_alpn

As tls_client, offering the ALPN protocols `protos` (comma-separated,
most wanted first: "h2,http/1.1"). tls_session_alpn tells what the
server chose ("" when it chose none). Returns 0 on failure.

```fides
pub fn tls_client_alpn(fd: i64, host: str, protos: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `host`: `str`
- `protos`: `str`

**Returns:** `i64`

*Defined at line 734*

---

## tls_connect_alpn

As tls_connect, offering the ALPN protocols `protos`. The session owns
the socket. Returns 0 on failure.

```fides
pub fn tls_connect_alpn(host: str, port: i64, protos: str) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `protos`: `str`

**Returns:** `i64`

*Defined at line 741*

---

## tls_connect_alpn_unchecked

As tls_connect_alpn, but accept any certificate: for checking a local
server with a self-signed certificate only.

```fides
pub fn tls_connect_alpn_unchecked(host: str, port: i64, protos: str) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `protos`: `str`

**Returns:** `i64`

*Defined at line 755*

---

## tls_server

Server credentials from a PKCS#12 file. Returns 0 on failure.

```fides
pub fn tls_server(pfx_path: str, password: str) -> i64 {
```

**Parameters:**

- `pfx_path`: `str`
- `password`: `str`

**Returns:** `i64`

*Defined at line 970*

---

## tls_server13

As tls_server, but every session it accepts is TLS 1.3 (a client
without 1.3 fails the handshake). Needs Windows 11 / Server 2022 or
later. Returns 0 on failure.

```fides
pub fn tls_server13(pfx_path: str, password: str) -> i64 {
```

**Parameters:**

- `pfx_path`: `str`
- `password`: `str`

**Returns:** `i64`

*Defined at line 992*

---

## tls_server_close

Free the server `server` (tls_server, tls_server13) once no session is
being accepted with it: its credentials, its ALPN list, and the private
key tls_server put in the user's key store (Schannel needs the key
there: importing the .pfx made a copy of it, which this deletes; the
.pfx stays). Sessions already accepted keep working until closed.
Returns 0.

```fides
pub fn tls_server_close(server: i64) -> i64 {
```

**Parameters:**

- `server`: `i64`

**Returns:** `i64`

*Defined at line 1017*

---

## tls_library

The TLS library under `use tls`: "schannel" here, "openssl" on Linux
and macOS.

```fides
pub fn tls_library() -> str {
```

**Returns:** `str`

*Defined at line 1045*

---

## tls_accept

The server's side of a TLS session over the accepted socket `fd`. Returns 0 on
failure.

```fides
pub fn tls_accept(server: i64, fd: i64) -> i64 {
```

**Parameters:**

- `server`: `i64`
- `fd`: `i64`

**Returns:** `i64`

*Defined at line 1051*

---

## tls_server_alpn

Make the server `server` (tls_server, tls_server13) answer ALPN: of the
protocols a client offers, it picks its first in `protos`
(comma-separated, "h2,http/1.1"); a client offering none of them, or no
ALPN, gets none. Call it once, before accepting. False when `protos`
holds no name.

```fides
pub fn tls_server_alpn(server: i64, protos: str) -> bool {
```

**Parameters:**

- `server`: `i64`
- `protos`: `str`

**Returns:** `bool`

*Defined at line 1072*

---

## tls_session_alpn

The protocol ALPN chose for the session ("h2", "http/1.1"), "" when
none was negotiated.

```fides
pub fn tls_session_alpn(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 1088*

---

## tls_session_fd

The session's socket.

```fides
pub fn tls_session_fd(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1132*

---

## tls_session_protocol

The negotiated protocol: SP_PROT_TLS1_3_CLIENT (8192) for TLS 1.3,
SP_PROT_TLS1_2_CLIENT (2048) for 1.2, -1 when not connected.

```fides
pub fn tls_session_protocol(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1138*

---

## tls_session_version

The negotiated TLS version as its wire number, the same on every
platform: 772 (0x0304) for TLS 1.3, 771 for 1.2, 770 for 1.1, 769 for
1.0; -1 when not connected or unknown.

```fides
pub fn tls_session_version(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1168*

---

## tls_session_send

Send `n` bytes at `addr`. Returns `n`, or -1.

```fides
pub fn tls_session_send(s: i64, addr: i64, n: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `addr`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 1187*

---

## tls_session_recv

Read up to `n` bytes into `addr`. Returns how many; 0 when the other side has
closed; below 0 on an error or a timeout.

```fides
pub fn tls_session_recv(s: i64, addr: i64, n: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `addr`: `i64`
- `n`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 1242*

---

## tls_session_close

End the session and free it. The socket stays open: closing it is the
caller's.

```fides
pub fn tls_session_close(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1450*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
