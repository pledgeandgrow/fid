[All modules](index.md)

# quic_conn -- Fides Documentation

## Contents

- [QS_HANDSHAKE:](#QS_HANDSHAKE:) (const)
- [QS_ESTABLISHED:](#QS_ESTABLISHED:) (const)
- [QS_CLOSING:](#QS_CLOSING:) (const)
- [QS_DRAINING:](#QS_DRAINING:) (const)
- [QS_CLOSED:](#QS_CLOSED:) (const)
- [QUIC_DATAGRAM:](#QUIC_DATAGRAM:) (const)
- [QuicConfig](#QuicConfig) (struct)
- [QRecv](#QRecv) (struct)
- [QStream](#QStream) (struct)
- [QSent](#QSent) (struct)
- [QSpace](#QSpace) (struct)
- [QuicConn](#QuicConn) (struct)
- [quic_config_client](#quic_config_client) (function)
- [quic_config_server](#quic_config_server) (function)
- [quic_connect](#quic_connect) (function)
- [quic_datagram_dcid](#quic_datagram_dcid) (function)
- [quic_datagram_short_dcid](#quic_datagram_short_dcid) (function)
- [quic_is_new_initial](#quic_is_new_initial) (function)
- [quic_accept](#quic_accept) (function)
- [quic_initial_token](#quic_initial_token) (function)
- [QuicTokenCheck](#QuicTokenCheck) (struct)
- [quic_retry_for](#quic_retry_for) (function)
- [quic_token_check](#quic_token_check) (function)
- [quic_reset_for](#quic_reset_for) (function)
- [quic_accept_from](#quic_accept_from) (function)
- [quic_conn_close](#quic_conn_close) (function)
- [quic_conn_established](#quic_conn_established) (function)
- [quic_conn_done](#quic_conn_done) (function)
- [quic_conn_closed](#quic_conn_closed) (function)
- [quic_stream_index](#quic_stream_index) (function)
- [quic_open_bidi](#quic_open_bidi) (function)
- [quic_open_uni](#quic_open_uni) (function)
- [quic_stream_write](#quic_stream_write) (function)
- [quic_stream_unsent](#quic_stream_unsent) (function)
- [quic_stream_read](#quic_stream_read) (function)
- [quic_stream_finished](#quic_stream_finished) (function)
- [quic_stream_reset_code](#quic_stream_reset_code) (function)
- [quic_stream_stop_code](#quic_stream_stop_code) (function)
- [quic_stream_reset](#quic_stream_reset) (function)
- [quic_stream_stop](#quic_stream_stop) (function)
- [quic_accept_streams](#quic_accept_streams) (function)
- [quic_readable_streams](#quic_readable_streams) (function)
- [quic_conn_recv](#quic_conn_recv) (function)
- [quic_conn_recv_from](#quic_conn_recv_from) (function)
- [quic_conn_peer_addr](#quic_conn_peer_addr) (function)
- [quic_conn_owns_cid](#quic_conn_owns_cid) (function)
- [quic_conn_was_reset](#quic_conn_was_reset) (function)
- [quic_conn_version_negotiated](#quic_conn_version_negotiated) (function)
- [quic_conn_resumed](#quic_conn_resumed) (function)
- [quic_conn_early_accepted](#quic_conn_early_accepted) (function)
- [quic_conn_early_open](#quic_conn_early_open) (function)
- [quic_conn_ticket](#quic_conn_ticket) (function)
- [quic_conn_has_ticket](#quic_conn_has_ticket) (function)
- [quic_conn_update_keys](#quic_conn_update_keys) (function)
- [quic_conn_migrate](#quic_conn_migrate) (function)
- [quic_conn_path_validated](#quic_conn_path_validated) (function)
- [quic_conn_migrations](#quic_conn_migrations) (function)
- [quic_conn_cids](#quic_conn_cids) (function)
- [quic_conn_timeout](#quic_conn_timeout) (function)
- [quic_conn_on_timeout](#quic_conn_on_timeout) (function)
- [quic_conn_send](#quic_conn_send) (function)

## QS_HANDSHAKE:

Connection states.

```fides
pub const QS_HANDSHAKE: i64 = 0;
```

*Defined at line 88*

---

## QS_ESTABLISHED:

```fides
pub const QS_ESTABLISHED: i64 = 1;
```

*Defined at line 89*

---

## QS_CLOSING:

```fides
pub const QS_CLOSING: i64 = 2;
```

*Defined at line 90*

---

## QS_DRAINING:

```fides
pub const QS_DRAINING: i64 = 3;
```

*Defined at line 91*

---

## QS_CLOSED:

```fides
pub const QS_CLOSED: i64 = 4;
```

*Defined at line 92*

---

## QUIC_DATAGRAM:

The size of every datagram we send at most (and of a client's
Initial datagrams exactly): QUIC's minimum, which every path carries.

```fides
pub const QUIC_DATAGRAM: i64 = 1200;
```

*Defined at line 96*

---

## QuicConfig

What a connection is configured with. quic_config_client and
quic_config_server fill the defaults.
- `alpn`: protocols offered (client) or accepted, by preference (server)
- `server_name`: the SNI a client sends
- `cert`, `key_seed`: the server's DER certificate and the 32-byte
Ed25519 seed of its key (quic_self_signed_cert)
- `peer_key`: the server's Ed25519 public key a client pins ([]: any)
- `suites`: TLS cipher suites by preference (quic_tls.fd)
- flow-control windows and stream limits we grant the peer
- `idle_timeout_ms`: 0 for none
- `seed`: [] to draw connection IDs and keys from the OS; bytes to
draw them from SHA-256 of the seed (reproducible tests)
- `version`: the QUIC version a client's first packets carry
(QUIC_V1; another one gets a Version Negotiation packet, after
which the client goes on in QUIC v1)
- `key_share`: a client sends its X25519 share in the first
ClientHello (false: the server asks with a HelloRetryRequest)
- `retry`: a server answers a client's first Initial with a Retry
and takes the connection when the client returns the token (address
validation, RFC 9000 §8.1.2); `token_key` (16 bytes) seals the tokens
- `reset_key`: 32 bytes the stateless reset tokens of our connection
IDs come from (RFC 9000 §10.3); [] for a key of the connection's own
(a server should share one across connections and restarts)
- `ticket_key`: a server's 16-byte key for session tickets ([]: none
sent); `early_data`: a server takes 0-RTT data on its tickets, a
client sends 0-RTT data when `ticket` allows it
- `ticket`: a client resumes with this ticket (quic_conn_ticket of an
earlier connection; quic_ticket_none() for none)
- `allow_migration`: the peer may move to another address (else
disable_active_migration is sent)

```fides
pub struct QuicConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `alpn` | `Vec<str>` |
| `server_name` | `str` |
| `cert` | `Vec<i64>` |
| `key_seed` | `Vec<i64>` |
| `peer_key` | `Vec<i64>` |
| `suites` | `Vec<i64>` |
| `max_data` | `i64` |
| `max_stream_data_bidi_local` | `i64` |
| `max_stream_data_bidi_remote` | `i64` |
| `max_stream_data_uni` | `i64` |
| `max_streams_bidi` | `i64` |
| `max_streams_uni` | `i64` |
| `idle_timeout_ms` | `i64` |
| `seed` | `Vec<i64>` |
| `version` | `i64` |
| `key_share` | `bool` |
| `retry` | `bool` |
| `token_key` | `Vec<i64>` |
| `reset_key` | `Vec<i64>` |
| `ticket_key` | `Vec<i64>` |
| `early_data` | `bool` |
| `ticket` | `QuicTicket` |
| `allow_migration` | `bool` |
| `roots` | `Vec<X509Cert>` |

*Defined at line 143*

---

## QRecv

Received bytes of a stream or of CRYPTO frames: `ready` holds the bytes
from offset `base` that arrived in order and were not read yet; pieces
that arrived ahead wait in `chunks` (at `chunk_off`). `highest` is the
largest offset seen (flow control), `fin_size` the final size (-1 until
known).

```fides
pub struct QRecv {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `ready` | `Vec<i64>` |
| `chunk_off` | `Vec<i64>` |
| `chunks` | `Vec<Vec<i64>>` |
| `highest` | `i64` |
| `fin_size` | `i64` |

*Defined at line 178*

---

## QStream

One stream. The sending half: `pending` bytes from `pending_pos` wait
to be sent at offset `send_off`, within `send_max` (the peer's limit).
The receiving half: `rx`, within `recv_max` (our limit, raised by
`recv_window` as the application reads).

```fides
pub struct QStream {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `pending` | `Vec<i64>` |
| `pending_pos` | `i64` |
| `send_off` | `i64` |
| `send_max` | `i64` |
| `fin_queued` | `bool` |
| `fin_sent` | `bool` |
| `fin_acked` | `bool` |
| `blocked_at` | `i64` |
| `reset_sent` | `bool` |
| `stop_code` | `i64` |
| `rx` | `QRecv` |
| `recv_max` | `i64` |
| `recv_window` | `i64` |
| `max_pending` | `bool` |
| `fin_read` | `bool` |
| `peer_reset` | `i64` |
| `stop_sent` | `bool` |

*Defined at line 191*

---

## QSent

A packet we sent and the peer has not acknowledged: its retransmittable
frames come back if it is lost.

```fides
pub struct QSent {
```

**Fields:**

| Name | Type |
|------|------|
| `pn` | `i64` |
| `time` | `i64` |
| `size` | `i64` |
| `ack_eliciting` | `bool` |
| `frames` | `Vec<QuicFrame>` |

*Defined at line 214*

---

## QSpace

A packet number space (Initial, Handshake, application).

```fides
pub struct QSpace {
```

**Fields:**

| Name | Type |
|------|------|
| `tx` | `QuicKeys` |
| `rx` | `QuicKeys` |
| `has_tx` | `bool` |
| `has_rx` | `bool` |
| `discarded` | `bool` |
| `next_pn` | `i64` |
| `largest_rx` | `i64` |
| `largest_rx_time` | `i64` |
| `recv` | `Vec<i64>` |
| `ack_needed` | `bool` |
| `largest_acked` | `i64` |
| `sent` | `Vec<QSent>` |
| `loss_time` | `i64` |
| `last_eliciting` | `i64` |
| `crypto_pending` | `Vec<i64>` |
| `crypto_off` | `i64` |
| `crypto_in` | `QRecv` |
| `retx` | `Vec<QuicFrame>` |
| `probes` | `i64` |

*Defined at line 223*

---

## QuicConn

One QUIC connection. The fields an application reads:
- `state` (QS_*), `alpn` once established;
- `error` -1, or the error code it closed with (ours or the peer's:
`peer_closed` tells which), `app_error` whether it is an
application error code, `why` the reason;
- `packets_sent`, `packets_received`, `packets_lost` (statistics).

```fides
pub struct QuicConn {
```

**Fields:**

| Name | Type |
|------|------|
| `server` | `bool` |
| `state` | `i64` |
| `cfg` | `QuicConfig` |
| `tls` | `QuicTls` |
| `alpn` | `str` |
| `scid` | `Vec<i64>` |
| `dcid` | `Vec<i64>` |
| `dcid_seq` | `i64` |
| `odcid` | `Vec<i64>` |
| `peer_first_scid` | `Vec<i64>` |
| `got_peer_packet` | `bool` |
| `retried` | `bool` |
| `retry_scid` | `Vec<i64>` |
| `token` | `Vec<i64>` |
| `initial_crypto` | `Vec<i64>` |
| `spare_cids` | `Vec<Vec<i64>>` |
| `spare_seqs` | `Vec<i64>` |
| `spaces` | `Vec<QSpace>` |
| `streams` | `Vec<QStream>` |
| `rr` | `i64` |
| `new_streams` | `Vec<i64>` |
| `peer` | `QuicParams` |
| `has_peer_params` | `bool` |
| `max_data_local` | `i64` |
| `recv_total` | `i64` |
| `consumed_total` | `i64` |
| `max_data_pending` | `bool` |
| `max_data_peer` | `i64` |
| `sent_total` | `i64` |
| `data_blocked_at` | `i64` |
| `max_streams_bidi_local` | `i64` |
| `max_streams_uni_local` | `i64` |
| `streams_bidi_pending` | `bool` |
| `streams_uni_pending` | `bool` |
| `peer_bidi_opened` | `i64` |
| `peer_uni_opened` | `i64` |
| `max_streams_bidi_peer` | `i64` |
| `max_streams_uni_peer` | `i64` |
| `local_bidi_opened` | `i64` |
| `local_uni_opened` | `i64` |
| `ctrl` | `Vec<QuicFrame>` |
| `srtt` | `i64` |
| `rttvar` | `i64` |
| `min_rtt` | `i64` |
| `latest_rtt` | `i64` |
| `has_rtt` | `bool` |
| `pto_count` | `i64` |
| `cwnd` | `i64` |
| `ssthresh` | `i64` |
| `bytes_in_flight` | `i64` |
| `recovery_start` | `i64` |
| `addr_validated` | `bool` |
| `bytes_received` | `i64` |
| `bytes_sent` | `i64` |
| `hs_confirmed` | `bool` |
| `handshake_done_pending` | `bool` |
| `key_phase` | `bool` |
| `ap_tx_secret` | `Vec<i64>` |
| `ap_rx_secret` | `Vec<i64>` |
| `prev_rx` | `QuicKeys` |
| `has_prev_rx` | `bool` |
| `key_update_pn` | `i64` |
| `close_pending` | `bool` |
| `close_deadline` | `i64` |
| `error` | `i64` |
| `app_error` | `bool` |
| `why` | `str` |
| `peer_closed` | `bool` |
| `idle_ms` | `i64` |
| `last_activity` | `i64` |
| `last_send` | `i64` |
| `rand_ctr` | `i64` |
| `packets_sent` | `i64` |
| `packets_received` | `i64` |
| `packets_lost` | `i64` |
| `now` | `i64` |
| `idle_restarted` | `bool` |
| `version` | `i64` |
| `vn_done` | `bool` |
| `retry_odcid` | `Vec<i64>` |
| `local_cids` | `Vec<Vec<i64>>` |
| `local_seqs` | `Vec<i64>` |
| `next_local_seq` | `i64` |
| `cids_issued` | `bool` |
| `reset_key` | `Vec<i64>` |
| `rx_dcid_seq` | `i64` |
| `dcid_token` | `Vec<i64>` |
| `spare_tokens` | `Vec<Vec<i64>>` |
| `reset_received` | `bool` |
| `early_tx` | `QuicKeys` |
| `early_rx` | `QuicKeys` |
| `has_early_tx` | `bool` |
| `has_early_rx` | `bool` |
| `early_packets` | `i64` |
| `first_ap_pn` | `i64` |
| `peer_addr` | `Vec<i64>` |
| `prev_addr` | `Vec<i64>` |
| `rx_addr` | `Vec<i64>` |
| `path_challenge` | `Vec<i64>` |
| `path_deadline` | `i64` |
| `path_validated` | `bool` |
| `path_rx` | `i64` |
| `path_tx` | `i64` |
| `migrations` | `i64` |

*Defined at line 251*

---

## quic_config_client

A client's configuration: the protocols it offers and the server name
it sends (may be ""). Set `peer_key` to pin the server's Ed25519 key,
or `roots` to verify the certificate chain against trust anchors (the
leaf must then name `server_name`); with neither, any certificate is
accepted though its CertificateVerify is still checked.

```fides
pub fn quic_config_client(alpn: Vec<str>, server_name: str) -> QuicConfig {
```

**Parameters:**

- `alpn`: `Vec<str>`
- `server_name`: `str`

**Returns:** `QuicConfig`

*Defined at line 458*

---

## quic_config_server

A server's configuration: the protocols it accepts, its DER certificate
and the 32-byte Ed25519 seed of the certificate's key.

```fides
pub fn quic_config_server(alpn: Vec<str>, cert: Vec<i64>, key_seed: Vec<i64>) -> QuicConfig {
```

**Parameters:**

- `alpn`: `Vec<str>`
- `cert`: `Vec<i64>`
- `key_seed`: `Vec<i64>`

**Returns:** `QuicConfig`

*Defined at line 466*

---

## quic_connect

A client connection: draws its connection IDs and keys and queues the
ClientHello. quic_conn_send then gives the first datagram.

```fides
pub fn quic_connect(cfg: QuicConfig, now: i64) -> QuicConn {
```

**Parameters:**

- `cfg`: `QuicConfig`
- `now`: `i64`

**Returns:** `QuicConn`

*Defined at line 592*

---

## quic_datagram_dcid

The Destination Connection ID of the long-header packet a datagram
starts with, [] for anything else: a server routes a new client's
Initial by it.

```fides
pub fn quic_datagram_dcid(d: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 653*

---

## quic_datagram_short_dcid

The Destination Connection ID of a short-header packet, for a server
whose connection IDs are all `cid_len` bytes.

```fides
pub fn quic_datagram_short_dcid(d: Vec<i64>, cid_len: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `Vec<i64>`
- `cid_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 669*

---

## quic_is_new_initial

Whether a datagram starts with a client Initial a server should accept
a connection from (RFC 9000 §14.1, §7.2): QUIC v1, at least 1200
bytes, a Destination Connection ID of at least 8 bytes.

```fides
pub fn quic_is_new_initial(d: Vec<i64>) -> bool {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 679*

---

## quic_accept

A server connection from a client's first datagram (check it with
quic_is_new_initial first). The datagram is processed: quic_conn_send
then gives the server's first flight. A datagram that does not start a
connection gives a closed connection (`state` QS_CLOSED).

```fides
pub fn quic_accept(cfg: QuicConfig, d: Vec<i64>, now: i64) -> QuicConn {
```

**Parameters:**

- `cfg`: `QuicConfig`
- `d`: `Vec<i64>`
- `now`: `i64`

**Returns:** `QuicConn`

*Defined at line 691*

---

## quic_initial_token

The token of the client Initial a datagram starts with ([] for none).

```fides
pub fn quic_initial_token(d: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 696*

---

## QuicTokenCheck

What a Retry token holds once opened: `ok`, the client's first
Destination Connection ID and the ID our Retry gave it.

```fides
pub struct QuicTokenCheck {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `odcid` | `Vec<i64>` |
| `retry_scid` | `Vec<i64>` |

*Defined at line 709*

---

## quic_retry_for

The Retry a server sends for a client's first Initial (`d`, without a
token) when its configuration has `retry` (RFC 9000 §8.1.2): a new
connection ID, and a token sealed with `token_key` binding the
client's address (`addr`, as the I/O layer names it), the time and
both connection IDs. [] when `d` is no new Initial or there is no
16-byte token key.

```fides
pub fn quic_retry_for(cfg: &QuicConfig, d: Vec<i64>, addr: Vec<i64>, now: i64) -> Vec<i64> {
```

**Parameters:**

- `cfg`: `&QuicConfig`
- `d`: `Vec<i64>`
- `addr`: `Vec<i64>`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 746*

---

## quic_token_check

Open a Retry token a client returned from `addr`: ok when our key
opens it for that address and it is at most QCN_RETRY_TOKEN_MS old.

```fides
pub fn quic_token_check(cfg: &QuicConfig, token: Vec<i64>, addr: Vec<i64>, now: i64) -> QuicTokenCheck {
```

**Parameters:**

- `cfg`: `&QuicConfig`
- `token`: `Vec<i64>`
- `addr`: `Vec<i64>`
- `now`: `i64`

**Returns:** `QuicTokenCheck`

*Defined at line 782*

---

## quic_reset_for

The Stateless Reset (RFC 9000 §10.3) an endpoint whose connection
IDs' tokens come from `reset_key` sends for a short-header datagram `d`
it has no connection for (our IDs are 8 bytes). [] when `d` must not be
answered (too short, a long header, no 32-byte key).

```fides
pub fn quic_reset_for(reset_key: Vec<i64>, d: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `reset_key`: `Vec<i64>`
- `d`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 820*

---

## quic_accept_from

A server connection from a client's first datagram, received from
`addr` (any bytes naming the client's address, e.g. udp_addr_key; []
when unknown). With `retry` configured the Initial must carry a valid
token (quic_retry_for made it): the client's address is then validated
and the transport parameters carry both Retry connection IDs; an
Initial without one, or with a bad one, gives a closed connection.

```fides
pub fn quic_accept_from(cfg: QuicConfig, d: Vec<i64>, addr: Vec<i64>, now: i64) -> QuicConn {
```

**Parameters:**

- `cfg`: `QuicConfig`
- `d`: `Vec<i64>`
- `addr`: `Vec<i64>`
- `now`: `i64`

**Returns:** `QuicConn`

*Defined at line 834*

---

## quic_conn_close

Close the connection with an application error code and reason
(CONNECTION_CLOSE type 0x1d once the handshake is done).

```fides
pub fn quic_conn_close(c: &mut QuicConn, code: i64, reason: str, now: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `code`: `i64`
- `reason`: `str`
- `now`: `i64`

*Defined at line 908*

---

## quic_conn_established

Whether the connection can carry streams: the handshake is complete
and it is not closing.

```fides
pub fn quic_conn_established(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 922*

---

## quic_conn_done

Whether the connection is over (closing, draining or closed): nothing
more will be delivered.

```fides
pub fn quic_conn_done(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 928*

---

## quic_conn_closed

Whether the connection is closed for good (its timers are over).

```fides
pub fn quic_conn_closed(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 933*

---

## quic_stream_index

Index of stream `id` in the table, -1 when it is not there.

```fides
pub fn quic_stream_index(c: &QuicConn, id: i64) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 1189*

---

## quic_open_bidi

Open a bidirectional stream: its id, or -1 when the peer's stream
limit is reached (or the connection is over).

```fides
pub fn quic_open_bidi(c: &mut QuicConn) -> i64 {
```

**Parameters:**

- `c`: `&mut QuicConn`

**Returns:** `i64`

*Defined at line 1285*

---

## quic_open_uni

Open a unidirectional stream (we send, the peer receives): its id, or
-1 when the peer's limit is reached.

```fides
pub fn quic_open_uni(c: &mut QuicConn) -> i64 {
```

**Parameters:**

- `c`: `&mut QuicConn`

**Returns:** `i64`

*Defined at line 1302*

---

## quic_stream_write

Queue bytes on a stream, and the FIN after them when `fin`. False when
the stream cannot be written: unknown, receive-only, finished or reset.

```fides
pub fn quic_stream_write(c: &mut QuicConn, id: i64, data: Vec<i64>, fin: bool) -> bool {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `id`: `i64`
- `data`: `Vec<i64>`
- `fin`: `bool`

**Returns:** `bool`

*Defined at line 1319*

---

## quic_stream_unsent

The bytes queued on a stream and not sent yet.

```fides
pub fn quic_stream_unsent(c: &QuicConn, id: i64) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 1340*

---

## quic_stream_read

Read what arrived in order on a stream since the last read ([] when
nothing new). Reading opens the flow-control windows again.

```fides
pub fn quic_stream_read(c: &mut QuicConn, id: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `id`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1351*

---

## quic_stream_finished

Whether the peer finished a stream and everything up to the FIN was
read (also true for a stream already forgotten).

```fides
pub fn quic_stream_finished(c: &QuicConn, id: i64) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 1379*

---

## quic_stream_reset_code

The error code the peer reset a stream with (RESET_STREAM), -1 if it
did not.

```fides
pub fn quic_stream_reset_code(c: &QuicConn, id: i64) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 1390*

---

## quic_stream_stop_code

The error code the peer asked us to stop sending with (STOP_SENDING),
-1 if it did not.

```fides
pub fn quic_stream_stop_code(c: &QuicConn, id: i64) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 1400*

---

## quic_stream_reset

Abandon sending on a stream: RESET_STREAM with `code`.

```fides
pub fn quic_stream_reset(c: &mut QuicConn, id: i64, code: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `id`: `i64`
- `code`: `i64`

*Defined at line 1409*

---

## quic_stream_stop

Ask the peer to stop sending on a stream: STOP_SENDING with `code`;
what still arrives is dropped.

```fides
pub fn quic_stream_stop(c: &mut QuicConn, id: i64, code: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `id`: `i64`
- `code`: `i64`

*Defined at line 1432*

---

## quic_accept_streams

The streams the peer opened since the last call.

```fides
pub fn quic_accept_streams(c: &mut QuicConn) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut QuicConn`

**Returns:** `Vec<i64>`

*Defined at line 1450*

---

## quic_readable_streams

The streams with bytes to read or a FIN not read yet.

```fides
pub fn quic_readable_streams(c: &QuicConn) -> Vec<i64> {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `Vec<i64>`

*Defined at line 1457*

---

## quic_conn_recv

Process one UDP datagram from the peer (all the packets coalesced in
it). Anything that does not decrypt is dropped, as RFC 9000 asks.

```fides
pub fn quic_conn_recv(c: &mut QuicConn, d: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `d`: `Vec<i64>`
- `now`: `i64`

*Defined at line 1540*

---

## quic_conn_recv_from

quic_conn_recv of a datagram that came from `addr` (any bytes naming
the peer's address and port, e.g. udp_addr_key): the connection
follows the peer to a new address when a packet that is not only a
probe arrives from it (RFC 9000 §9.3), validating it with
PATH_CHALLENGE meanwhile; quic_conn_peer_addr tells where to send.

```fides
pub fn quic_conn_recv_from(c: &mut QuicConn, d: Vec<i64>, addr: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `d`: `Vec<i64>`
- `addr`: `Vec<i64>`
- `now`: `i64`

*Defined at line 1627*

---

## quic_conn_peer_addr

Where the connection's datagrams go: the peer address quic_conn_recv_from
last settled on ([] when it was never given one).

```fides
pub fn quic_conn_peer_addr(c: &QuicConn) -> Vec<i64> {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `Vec<i64>`

*Defined at line 1641*

---

## quic_conn_owns_cid

Whether `cid` is one of this connection's IDs (a server routes short
headers by it: the peer may use any ID we issued).

```fides
pub fn quic_conn_owns_cid(c: &QuicConn, cid: Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`
- `cid`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1659*

---

## quic_conn_was_reset

Whether a stateless reset ended the connection.

```fides
pub fn quic_conn_was_reset(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1688*

---

## quic_conn_version_negotiated

Whether the client went on in QUIC v1 after a Version Negotiation.

```fides
pub fn quic_conn_version_negotiated(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1753*

---

## quic_conn_resumed

Whether this connection resumed a session (a ticket), and whether its
0-RTT data was taken (client: the server said so; server: we did).

```fides
pub fn quic_conn_resumed(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1798*

---

## quic_conn_early_accepted

```fides
pub fn quic_conn_early_accepted(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1802*

---

## quic_conn_early_open

Whether a client can write stream data now that goes out as 0-RTT.

```fides
pub fn quic_conn_early_open(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1807*

---

## quic_conn_ticket

The latest session ticket the server gave this (client) connection,
for QuicConfig.ticket of a later one; quic_conn_has_ticket tells
whether there is one.

```fides
pub fn quic_conn_ticket(c: &QuicConn) -> QuicTicket {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `QuicTicket`

*Defined at line 1814*

---

## quic_conn_has_ticket

```fides
pub fn quic_conn_has_ticket(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 1818*

---

## quic_conn_update_keys

Start a key update (RFC 9001 §6.1): new 1-RTT keys for sending, the
key-phase bit flips. False when one cannot start yet: the handshake is
not confirmed, or the last update was not acknowledged.

```fides
pub fn quic_conn_update_keys(c: &mut QuicConn) -> bool {
```

**Parameters:**

- `c`: `&mut QuicConn`

**Returns:** `bool`

*Defined at line 1905*

---

## quic_conn_migrate

Client connection migration (RFC 9000 §9.2): call it when the client
now sends from another local address (quic_udp_client_migrate opens a
new socket). It moves to a fresh connection ID of the server's, starts
path validation and restarts congestion control. False when it may
not: before the handshake is confirmed, the server asked for no
migration (disable_active_migration), or it gave us no spare ID.

```fides
pub fn quic_conn_migrate(c: &mut QuicConn, now: i64) -> bool {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 2068*

---

## quic_conn_path_validated

Whether the current path is validated (our PATH_CHALLENGE answered),
and how many times the connection moved to another path.

```fides
pub fn quic_conn_path_validated(c: &QuicConn) -> bool {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `bool`

*Defined at line 2080*

---

## quic_conn_migrations

```fides
pub fn quic_conn_migrations(c: &QuicConn) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `i64`

*Defined at line 2084*

---

## quic_conn_cids

The connection IDs this connection answers to (a server routes by them).

```fides
pub fn quic_conn_cids(c: &QuicConn) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2138*

---

## quic_conn_timeout

When quic_conn_on_timeout should next be called (-1: no timer). The
earliest of the loss timer, the probe timeout, the idle timeout and the
end of the closing or draining period.

```fides
pub fn quic_conn_timeout(c: &QuicConn) -> i64 {
```

**Parameters:**

- `c`: `&QuicConn`

**Returns:** `i64`

*Defined at line 2707*

---

## quic_conn_on_timeout

Run the timers that are due at `now`: loss detection, a probe (one or
two ack-eliciting packets go out with the next quic_conn_send calls),
the idle timeout and the end of closing or draining.

```fides
pub fn quic_conn_on_timeout(c: &mut QuicConn, now: i64) {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `now`: `i64`

*Defined at line 2754*

---

## quic_conn_send

The next datagram to send, [] when there is nothing (call again until
it gives []). Initial, Handshake and 1-RTT packets are coalesced; a
datagram with an Initial packet is padded to 1200 bytes.

```fides
pub fn quic_conn_send(c: &mut QuicConn, now: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut QuicConn`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3178*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
