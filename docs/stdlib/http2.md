# http2, http2_tls, tls_alpn — HTTP/2 in Fides

`stdlib/http2.fd` is HTTP/2 (RFC 9113) with HPACK (`stdlib/hpack.fd`,
RFC 7541). `stdlib/http2_tls.fd` runs it over TLS ("h2"), the protocol
chosen by ALPN (RFC 7301) through module `tls`; `stdlib/tls_alpn.fd` holds
ALPN's wire format and the server's choice. Handlers are those of module
`http`: `fn(HttpRequest) -> str`, the response built by `build_response`.

## Ways in

| How the connection starts | Server | Client |
|---|---|---|
| Cleartext, prior knowledge (the preface at once) | `http2_serve`, `http2_listen_tasks` | `http2_get`, `http2_post`, `http2_get_many`, `http2_connect` + `http2_request` / `http2_wait` |
| Cleartext, HTTP/1.1 `Upgrade: h2c` (RFC 7540 3.2) | the same server: a request with `Upgrade: h2c`, `Connection: Upgrade, HTTP2-Settings` and one valid `HTTP2-Settings` gets `101 Switching Protocols` and becomes stream 1 | `http2_get_upgrade` (falls back to the HTTP/1.1 answer of a server without HTTP/2) |
| Cleartext, plain HTTP/1.1 | the same server answers it over HTTP/1.1 (`Connection: close`) with the same handler | — |
| TLS, ALPN "h2" | `http2_tls_serve(port, cert, key, handler)`, `http2_tls_listen` | `http2_tls_get`, `http2_tls_post`, `http2_tls_connect` (+ `_unchecked` for self-signed local servers) |
| TLS, ALPN "http/1.1" or none | the same TLS server hands the connection to module `http` | `https` |

```fides
use http; use http2; use http2_tls;

fn route(req: HttpRequest) -> str {
    return build_response(200, "text/plain", "over HTTP/" + req.version + "\n");
}

fn main() -> i64 {
    // h2 and HTTP/1.1 over TLS; cert/key as tls_server
    http2_tls_serve(8443, "cert.pem", "key.pem", route);
    return 0;
}
```

`cert` and `key` are what `tls_server` takes: a `.pfx` file and its
password on Windows (Schannel imports the key into the user's key store),
PEM files elsewhere (OpenSSL).

## ALPN in module `tls`

| Function | What it does |
|---|---|
| `tls_client_alpn(fd, host, protos)` | a client session offering `protos` ("h2,http/1.1", most wanted first) |
| `tls_connect_alpn(host, port, protos)` | connect, then as `tls_client_alpn` |
| `tls_connect_alpn_unchecked(host, port, protos)` | as above, any certificate (local tests) |
| `tls_server_alpn(server, protos)` | the server picks, of what a client offers, its first in `protos`; call once before accepting |
| `tls_session_alpn(s)` | the protocol chosen, "" when none |

Schannel takes the lists as `SECBUFFER_APPLICATION_PROTOCOLS` input buffers
(the client's first `InitializeSecurityContext`, the server's first
`AcceptSecurityContext`) and reports the choice through
`SECPKG_ATTR_APPLICATION_PROTOCOL` (Windows 8.1 or later); Schannel orders a
server's choice itself. OpenSSL takes the client's list with
`SSL_set_alpn_protos` and calls a Fides function (`ts_alpn_select`, a C
callback) to choose on a server: the server's first name the client offers
(RFC 7301 3.2). `tls_unix` also notes SIGPIPE instead of dying of it: OpenSSL
writes with `write()`, and a peer that has gone away would end the process.

## Limits and timers

- We announce `MAX_CONCURRENT_STREAMS` 100, `INITIAL_WINDOW_SIZE` 1 MiB (the
  connection window is opened to 16 MiB), `MAX_HEADER_LIST_SIZE`
  `H2_OUR_MAX_HEADER_LIST` (65536), and (client) `ENABLE_PUSH` 0. Frames
  stay at 16384 bytes.
- `MAX_HEADER_LIST_SIZE` is enforced on what we receive (`h2_header_list_size`:
  each field's name, value and 32 bytes): a larger request gets 431 (and
  `RST_STREAM NO_ERROR` when it had not ended, 8.1); a client refuses a
  larger response with `RST_STREAM CANCEL`. The block is decoded first, so
  the HPACK table stays in step. A request larger than the peer's limit is
  not sent (`http2_request` returns -1).
- Our SETTINGS must be acknowledged within `H2_SETTINGS_ACK_MS` (10 s): past
  that the connection ends with `GOAWAY SETTINGS_TIMEOUT` (6.5.3,
  `h2_settings_overdue`). Reads wait no longer than the deadline.
- A field name with a control byte, space, DEL or a colon after the first
  byte, or a value with NUL, CR or LF, is malformed (8.2.1): a stream error.
- A request's DATA must add up to its content-length (8.1.1).

## Handlers on tasks

A server runs each request's handler on a task of its own
(`h2_handler_task`): the requests of one connection are handled side by
side, and a slow one does not hold up the others. The request goes to the
task as text (`h2_job_text`, read back by `h2_job_request`), the response
comes back through the task's slot, and `h2_collect_jobs` sends it on its
stream; a response whose stream was reset meanwhile is dropped. While
handlers run, the connection waits for input a few milliseconds at a time.
`http2_set_handler_tasks(false)` runs handlers on the connection's task, one
at a time (connections driven by hand, as the tests do, never use tasks).

## Left out

- **Server push** (RFC 9113 8.4): not sent, as browsers no longer take it
  (Chrome removed it in 2022). We send `ENABLE_PUSH 0`; a `PUSH_PROMISE` is a
  connection error.
- **Priorities** (5.3): RFC 9113 deprecates the scheme of RFC 7540. PRIORITY
  frames and the priority fields of HEADERS are parsed and checked (a stream
  depending on itself is a stream error) and otherwise ignored; streams are
  answered as their handlers finish and share the windows round-robin, a
  DATA frame per stream in turn. RFC 9218's Priority field is not read
  either.
- A chunked request body before an h2c Upgrade (the server answers 411).

## Tests

| Test | What it checks |
|---|---|
| `tests/bootstrap/test_http2_frames.fd` | every frame type, the section 6 checks, bytes from python-h2, a connection by hand |
| `tests/bootstrap/test_stdlib_http2_length.fd` | content-length (8.1.1) |
| `tests/bootstrap/test_http2_limits.fd` | SETTINGS timeout, MAX_HEADER_LIST_SIZE both ways, malformed values |
| `tests/bootstrap/test_http2_upgrade.fd` | base64url, the h2c Upgrade (accepted and refused), HTTP/1.1 answers |
| `tests/bootstrap/test_http2_tasks.fd` | handlers on tasks, the request text round trip |
| `tests/bootstrap/test_tls_alpn.fd` | ALPN wire format and the server's choice |
| `tests/net/http2_live.fd` | sockets: prior knowledge, Upgrade, HTTP/1.1, a fast request answered while a slow one runs (C and native builds) |
| `tests/tls/http2_tls_live.fd` | TLS with a self-signed certificate: ALPN choices, HTTP/1.1 and h2 over TLS |

Checked by hand on Nov 17 (Linux, OpenSSL 3, curl 8.20 with nghttp2):
`curl --http2` against `http2_tls_serve` (h2 chosen, a 100 KB POST, two
requests on one connection), `curl --http1.1` against it (HTTP/1.1), and
`curl --http2` (the h2c Upgrade, POST with `Expect: 100-continue` and HEAD
included), `--http2-prior-knowledge` and `--http1.1` against the cleartext
server. On Windows, our client over Schannel against a public h2 server
(ALPN "h2", a GET over HTTP/2), C and native builds.
