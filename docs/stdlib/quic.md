# quic — QUIC v1 in pure Fides

QUIC (RFC 9000, 9001, 9002) is split into five modules, each usable on its
own:

| Module | What it does |
|--------|--------------|
| `quic` | Packet protection (RFC 9001 §5): Initial secrets, packet keys, header protection, key update, Retry tag, varints, packet numbers, long and short headers |
| `quic_frame` | Every frame of RFC 9000 §19 encoded and parsed, the frame-per-packet-type table (§12.4), ACK range sets, transport parameters (§18) |
| `quic_tls` | The TLS 1.3 handshake carried in CRYPTO frames: X25519, a self-signed Ed25519 X.509 certificate, ALPN, the transport-parameters extension |
| `quic_conn` | A connection, client or server, without I/O: datagrams and the time in, datagrams out |
| `quic_udp` | `quic_conn` over `udp` sockets (IPv4 and IPv6): one client connection, or a server socket that accepts and routes connections |

The sockets are [udp.md](udp.md)'s: the system's socket API, called
directly, so QUIC runs through C on Windows, Linux and macOS and natively
(`fid emit`) with nothing linked by hand.

Bytes are `Vec<i64>` holding 0..255 throughout. Times are milliseconds on
any monotonic clock (`quic_udp` uses `p_clock_now_ms`).

HTTP/3 and QPACK on top of these: [http3.md](http3.md).

## A connection without I/O

```fides
use quic_conn;

var alpn: Vec<str> = Vec::new();
alpn.push("h3");
var c: QuicConn = quic_connect(quic_config_client(alpn, "example.org"), now);

// every time something happens:
val d: Vec<i64> = quic_conn_send(& mut c, now);    // [] when nothing to send
quic_conn_recv(& mut c, datagram, now);            // a datagram from the peer
if quic_conn_timeout(& c) <= now { quic_conn_on_timeout(& mut c, now); }
```

Call `quic_conn_send` until it returns `[]`; every datagram is at most 1200
bytes. A server makes its connection from a client's first datagram:
`quic_is_new_initial(d)` tells whether it is one, `quic_accept(cfg, d, now)`
makes the connection, and `quic_datagram_dcid` / `quic_datagram_short_dcid`
(8-byte IDs) route later datagrams to it.

### Configuration

`quic_config_client(alpn, server_name)` and `quic_config_server(alpn, cert,
key_seed)` fill a `QuicConfig`; change fields before connecting:

| Field | Default | Meaning |
|-------|---------|---------|
| `peer_key` | `[]` | client: the server's Ed25519 public key to pin; `[]` accepts any server (an RSA or ECDSA one too) |
| `suites` | AES-128-GCM, ChaCha20-Poly1305 | TLS cipher suites by preference |
| `max_data` | 4 MiB | connection flow-control window we grant |
| `max_stream_data_bidi_local` / `_bidi_remote` / `_uni` | 1 MiB | stream windows we grant |
| `max_streams_bidi` / `max_streams_uni` | 100 | streams the peer may have open |
| `idle_timeout_ms` | 30000 | 0 for none |
| `seed` | `[]` | `[]`: keys and IDs from the OS; bytes: from SHA-256 of the seed (tests) |
| `version` | `QUIC_V1` | client: the version of its first packets (another one is answered with Version Negotiation, after which it goes on in v1) |
| `key_share` | true | client: the X25519 share in the first ClientHello; false waits for a HelloRetryRequest (one round trip more) |
| `retry` | false | server: validate addresses with a Retry first (`quic_retry_for`, `quic_accept_from`) |
| `token_key` | `[]` | server: 16 bytes that seal Retry tokens |
| `reset_key` | `[]` | 32 bytes the stateless reset tokens of our connection IDs come from; `[]`: a key of the connection's own (`quic_udp` shares one per socket) |
| `ticket_key` | `[]` | server: 16 bytes that seal session tickets; `[]`: no tickets |
| `early_data` | false | server: take 0-RTT on its tickets; client: send 0-RTT when `ticket` allows |
| `ticket` | none | client: resume with this ticket (`quic_conn_ticket` of an earlier connection) |
| `allow_migration` | true | the peer may change address (false sends disable_active_migration) |

A server certificate: `quic_self_signed_cert(seed, "name")` makes a DER
X.509 certificate for the Ed25519 key of a 32-byte seed;
`quic_cert_ed25519_key(cert)` gives the public key a client pins.

### Streams

| Call | |
|------|---|
| `quic_open_bidi(& mut c)` / `quic_open_uni(& mut c)` | a new stream id, -1 at the peer's limit (or before the handshake) |
| `quic_stream_write(& mut c, id, bytes, fin)` | queue bytes (and FIN); false if the stream cannot be written |
| `quic_stream_read(& mut c, id)` | what arrived in order since the last read; reading reopens the windows |
| `quic_stream_finished(& c, id)` | the peer's FIN was read (true for a forgotten stream) |
| `quic_accept_streams(& mut c)` | streams the peer opened since the last call |
| `quic_readable_streams(& c)` | streams with data or a FIN to read |
| `quic_stream_reset(& mut c, id, code)` / `quic_stream_stop(& mut c, id, code)` | RESET_STREAM / STOP_SENDING |
| `quic_stream_reset_code(& c, id)` / `quic_stream_stop_code(& c, id)` | the peer's codes, -1 if none |

Stream ids follow RFC 9000 §2.1: the client's bidirectional streams are 0,
4, 8, ..., the server's unidirectional ones 3, 7, 11, ... A stream leaves
the table when both halves are over; a peer's stream then gives the peer
one more stream (MAX_STREAMS).

### State and closing

`quic_conn_established(& c)`, `quic_conn_done(& c)` (closing, draining or
closed) and `quic_conn_closed(& c)`. `c.alpn` is the protocol agreed on.
`quic_conn_close(& mut c, code, reason, now)` sends an application
CONNECTION_CLOSE. After a close, `c.error` is the code (ours or the peer's:
`c.peer_closed`), `c.app_error` whether it is an application code, `c.why`
the reason; a TLS failure is `0x100 + alert` (RFC 9001 §4.8).
`quic_conn_update_keys(& mut c)` starts a key update.

### Packets no connection takes (a server)

| Call | |
|------|---|
| `quic_packet_version(d)` | the version of a long header (0: Version Negotiation, -1: short header) |
| `quic_version_negotiation(d, versions, bits)` | the Version Negotiation packet answering `d` (RFC 9000 §6) |
| `quic_retry_for(& cfg, d, addr, now)` | a Retry for a client's first Initial: a new connection ID and a token sealed with `token_key`, bound to `addr` (any bytes naming the client's address: `udp_addr_key`) and the time (10 s) |
| `quic_accept_from(cfg, d, addr, now)` | `quic_accept` knowing the client's address; with `retry` set the Initial must carry a valid token from that address, else the connection comes back closed (`why` "retry required" / `error` INVALID_TOKEN) |
| `quic_reset_for(reset_key, d)` | the Stateless Reset for a short-header datagram whose connection is gone (RFC 9000 §10.3): the token of its connection ID, shorter than `d` |

`quic_udp_server_poll` does all of this itself.

### Connection IDs, migration, stateless reset

Once the handshake is confirmed, each end issues NEW_CONNECTION_ID up to
`min(peer's active_connection_id_limit, 4)` IDs, each with its stateless
reset token, and issues another when the peer retires one. A server routes
short headers with `quic_conn_owns_cid(& c, cid)` (or `quic_conn_cids`).

- A client moves to another local address with `quic_conn_migrate(& mut c,
  now)` (`quic_udp_client_migrate` opens the new socket): a fresh ID of the
  server's, the old one retired, PATH_CHALLENGE, congestion control and the
  RTT from scratch. False before the handshake is confirmed, when the server
  sent disable_active_migration, or without a spare ID.
- A server gives each datagram's address with `quic_conn_recv_from(& mut c,
  d, addr, now)` and sends to `quic_conn_peer_addr(& c)`. When the client's
  newest packet that is not only a probe comes from another address (a
  migration, a NAT rebinding), the connection moves there, validates it with
  PATH_CHALLENGE (sending at most 3x what came from it until the answer) and
  goes back to the old address if no PATH_RESPONSE comes.
  `quic_conn_path_validated`, `quic_conn_migrations`.
- A Stateless Reset from the peer (its token at the end of a datagram that
  does not open) ends the connection: draining, `quic_conn_was_reset(& c)`.

### Resumption, 0-RTT, HelloRetryRequest

A server with `ticket_key` sends one session ticket after the handshake. The
client's `quic_conn_has_ticket` / `quic_conn_ticket` give it (a
`QuicTicket`: it holds the PSK, keep it as a secret). A later connection with
`cfg.ticket = ticket` resumes (PSK with (EC)DHE, no certificate:
`quic_conn_resumed`); with `early_data` on both sides the client can open
streams and write right after `quic_connect` (`quic_conn_early_open`): the
data goes in 0-RTT packets with the first flight, within the limits the
ticket remembers. `quic_conn_early_accepted` tells whether the server took
it; when it did not (or after a Retry or a Version Negotiation), the data
goes again in 1-RTT packets by itself. The server takes 0-RTT only for the
ticket's cipher suite and ALPN, a ticket age within 10 s of its own view,
and transport limits not below the ticket's.

There is no anti-replay store: a server with `early_data` accepts the same
0-RTT data again if an attacker replays the client's first flight. Turn it
on only for requests that are safe to repeat (RFC 9001 §9.2, RFC 8446
§8).

A client with `key_share` false sends no X25519 share; the server answers
with a HelloRetryRequest and the client sends a second ClientHello with
the share (no 0-RTT then).

## Over UDP

```fides
use quic_udp;

var cl: QuicUdpClient = quic_udp_connect("2001:db8::1", 4433, cfg);   // or "192.0.2.1"
while !quic_conn_established(& cl.conn) { quic_udp_client_poll(& mut cl, 50); }

var sv: QuicUdpServer = quic_udp_listen(4433, server_cfg);   // dual-stack ::
loop { quic_udp_server_poll(& mut sv, 50); /* sv.conns: the connections */ }
```

`quic_udp_listen_on(ip, port, cfg)` binds one address (port 0: one the
system picks, `quic_udp_server_port`). The server socket answers Version
Negotiation, Retry (`cfg.retry`; a random `token_key` when none is given)
and Stateless Reset (one random `reset_key` per socket when none is given:
give your own to answer after a restart), routes datagrams by connection ID
and follows clients that move.

## What is not there

Client certificates, RSA/ECDSA server certificates (a client without a
pinned key accepts them without checking their signature, so it can reach
public servers; a server only has Ed25519), certificate chains (a client
either pins the server's key or accepts any), 0-RTT anti-replay, a
preferred address, ECN, path MTU discovery (every datagram is at most 1200
bytes), pacing, version_information (RFC 9368: after Version Negotiation a
client goes on in v1 without downgrade protection). Host names are not
resolved by `quic_udp` (give an address).

## Tests

- `tests/bootstrap/test_stdlib_quic_protect.fd`: RFC 9001 Appendix A
- `tests/bootstrap/test_stdlib_quic_frames.fd`: frames, ranges, parameters
- `tests/bootstrap/test_stdlib_quic_conn.fd`: client and server in one
  process over a simulated lossy network (through C: the interpreter is too
  slow for the handshake's cryptography)
- `tests/bootstrap/test_stdlib_quic_more.fd`: HelloRetryRequest, Retry,
  Version Negotiation, NEW_CONNECTION_ID, migration and NAT rebinding,
  stateless reset, resumption, 0-RTT accepted and refused (through C)
- `tests/bootstrap/test_stdlib_udp_addr.fd`: IPv4/IPv6 address text
- `tests/net/udp_loopback.fd`: UDP sockets, IPv4, IPv6 and dual stack
- `tests/net/quic_udp_echo.fd`: QUIC over UDP sockets
- `tests/net/quic_udp_more.fd`: Retry, migration to a new socket, Version
  Negotiation, 0-RTT and a stateless reset over real sockets on ::1
- `tests/net/quic_curl_interop.fd`, `tests/net/http3_curl_server.fd`: curl
  (ngtcp2, nghttp3) against our server
