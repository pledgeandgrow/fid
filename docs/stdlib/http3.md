# http3 — HTTP/3 and QPACK in pure Fides

HTTP/3 (RFC 9114) runs on the QUIC modules of [quic.md](quic.md). Three
modules:

| Module | What it does |
|--------|--------------|
| `qpack` | QPACK field compression (RFC 9204) with the static table only: field sections, Huffman-coded strings, the encoder- and decoder-stream instructions a peer may send to a table of capacity 0 |
| `http3` | The HTTP/3 layer of one `QuicConn`, client or server, without I/O: control and QPACK streams, SETTINGS, requests and responses, GOAWAY, the errors of §8 |
| `http3_udp` | `http3` over `quic_udp`'s sockets: `h3_udp_fetch` for one request, a server that returns whole requests |

Bytes are `Vec<i64>` of 0..255; a field list is two parallel `Vec<str>`,
names and values. Field names are lowercased when sent.

## A request, over UDP

```fides
use quic_conn; use http3; use http3_udp;

var alpn: Vec<str> = Vec::new();
alpn.push("h3");
val m: H3Message = h3_udp_fetch("104.18.29.112", 443, quic_config_client(alpn, "cloudflare-quic.com"),
    "GET", "cloudflare-quic.com", "/", names, values, body, 15000);
if m.ok { println(int_to_str(m.status) + " " + str_from_bytes(m.body)); }
```

Without `cfg.peer_key` the client accepts any server (like `curl -k`):
RSA and ECDSA certificates are taken without checking their signature,
since the QUIC TLS has neither (quic.md, "What is not there").

## A server, over UDP

```fides
var sv: H3UdpServer = h3_udp_listen(4433, quic_config_server(alpn, cert, seed));
loop {
    val reqs: Vec<H3UdpRequest> = h3_udp_server_poll(& mut sv, 20);
    // r.msg.method, r.msg.path, r.msg.authority, h3_field(& r.msg, "user-agent"), r.msg.body
    h3_udp_respond(& mut sv, r.conn, r.id, 200, names, values, body);
}
```

## Without I/O

`H3Conn` holds the HTTP/3 state; the `QuicConn` stays the program's. After
every `quic_conn_recv` / `quic_conn_on_timeout` call `h3_poll(& mut h,
& mut q)`, then `quic_conn_send` until it returns `[]`.

| Call | |
|------|---|
| `h3_new(server)` | the state for a client or a server connection |
| `h3_poll(& mut h, & mut q)` | open our control and QPACK streams (once established), read the peer's, read requests or responses |
| `h3_request(& mut h, & mut q, method, scheme, authority, path, names, values, body)` | client: a request on a new stream; -1 before the handshake, after GOAWAY, at the stream limit, or for malformed fields |
| `h3_done(& h, id)` / `h3_response(& mut h, id)` | client: whether the response is complete or failed / take it |
| `h3_track(& mut h, id, method)` | client: read the response of a stream the program wrote itself |
| `h3_requests(& mut h)` / `h3_message(& h, id)` | server: requests that arrived whole since the last call / one of them |
| `h3_respond(& mut h, & mut q, id, status, names, values, body)` | server: the response; `h3_respond_trailers` with trailers, `h3_respond_interim` for a 1xx before it, `h3_reject` for H3_REQUEST_REJECTED |
| `h3_goaway(& mut h, & mut q)` | graceful shutdown: a server keeps answering what it has, refuses newer requests |
| `h3_close(& mut h, & mut q, now)` | CONNECTION_CLOSE with H3_NO_ERROR |

An `H3Message` is `ok` once whole and well-formed; otherwise `error` is -1
(not complete) or the HTTP/3 code it failed with (the peer's RESET_STREAM
or ours: H3_MESSAGE_ERROR for malformed fields or a wrong content-length,
H3_REQUEST_INCOMPLETE, H3_REQUEST_REJECTED after GOAWAY). Requests have
`method`, `scheme`, `authority`, `path`; responses `status` and
`interim` (how many 1xx came first); both `names` / `values` (the other
fields), `body`, `trailer_names` / `trailer_values`.

A connection error closes the QUIC connection with the HTTP/3 code (`h.error`,
`h.why`): H3_FRAME_UNEXPECTED (DATA before HEADERS, a frame on the wrong
stream), H3_FRAME_ERROR (a malformed or cut-short frame), H3_SETTINGS_ERROR,
H3_MISSING_SETTINGS, H3_CLOSED_CRITICAL_STREAM, H3_STREAM_CREATION_ERROR,
H3_ID_ERROR, QPACK_DECOMPRESSION_FAILED (a dynamic-table reference), the
QPACK stream errors. Unknown and reserved frame types, stream types and
settings are skipped (§9).

## QPACK

`qpack_encode(names, values, huffman)` makes a field section with
Required Insert Count 0: an indexed line for a static entry, a literal with
a static name reference, or a literal name; `authorization` is sent never
indexed. `qpack_decode(block, max_size)` reads one; a dynamic or post-base
reference is QPACK_DECOMPRESSION_FAILED, a section over `max_size` is
`too_large`. `qpack_static_name(i)` / `qpack_static_value(i)` give the 99
entries of Appendix A.

`http3` advertises SETTINGS_QPACK_MAX_TABLE_CAPACITY 0 and
QPACK_BLOCKED_STREAMS 0, so peers never reference a dynamic table; it never
uses the peer's table either.

## What is not there

Server push (a client never sends MAX_PUSH_ID; a server never pushes), the
QPACK dynamic table, extended CONNECT and WebTransport, HTTP datagrams,
priorities (the `priority` field passes through untouched), streaming a
body in pieces (a request or response is queued whole; QUIC flow control
paces it).

## Tests

- `tests/bootstrap/test_stdlib_qpack.fd`: RFC 9204 B.1, the static table,
  Huffman, the errors (runs on the interpreter)
- `tests/bootstrap/test_stdlib_http3.fd`: a client and a server in one
  process over a simulated lossy network, every error above (through C)
- `tests/net/http3_udp_loopback.fd`: the same over UDP sockets (Windows,
  Linux, macOS; through C or `fid emit`: the sockets are `stdlib/udp.fd`'s)
- `tests/net/http3_curl_server.fd`: curl (ngtcp2/nghttp3) against our server
- `tests/net/http3_public.fd`: our client against a public HTTP/3 server
