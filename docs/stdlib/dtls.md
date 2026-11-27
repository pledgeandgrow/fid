[All modules](index.md)

# dtls -- Fides Documentation

## Contents

- [DTLS_NEW:](#DTLS_NEW:) (const)
- [DTLS_CONNECTING:](#DTLS_CONNECTING:) (const)
- [DTLS_CONNECTED:](#DTLS_CONNECTED:) (const)
- [DTLS_FAILED:](#DTLS_FAILED:) (const)
- [DTLS_CLOSED:](#DTLS_CLOSED:) (const)
- [DTLS_CT_CCS:](#DTLS_CT_CCS:) (const)
- [DTLS_CT_ALERT:](#DTLS_CT_ALERT:) (const)
- [DTLS_CT_HANDSHAKE:](#DTLS_CT_HANDSHAKE:) (const)
- [DTLS_CT_APP:](#DTLS_CT_APP:) (const)
- [DTLS_HS_CLIENT_HELLO:](#DTLS_HS_CLIENT_HELLO:) (const)
- [DTLS_HS_SERVER_HELLO:](#DTLS_HS_SERVER_HELLO:) (const)
- [DTLS_HS_HELLO_VERIFY:](#DTLS_HS_HELLO_VERIFY:) (const)
- [DTLS_HS_CERTIFICATE:](#DTLS_HS_CERTIFICATE:) (const)
- [DTLS_HS_SERVER_KEY_EXCHANGE:](#DTLS_HS_SERVER_KEY_EXCHANGE:) (const)
- [DTLS_HS_CERTIFICATE_REQUEST:](#DTLS_HS_CERTIFICATE_REQUEST:) (const)
- [DTLS_HS_SERVER_HELLO_DONE:](#DTLS_HS_SERVER_HELLO_DONE:) (const)
- [DTLS_HS_CERTIFICATE_VERIFY:](#DTLS_HS_CERTIFICATE_VERIFY:) (const)
- [DTLS_HS_CLIENT_KEY_EXCHANGE:](#DTLS_HS_CLIENT_KEY_EXCHANGE:) (const)
- [DTLS_HS_FINISHED:](#DTLS_HS_FINISHED:) (const)
- [DTLS_ALERT_CLOSE_NOTIFY:](#DTLS_ALERT_CLOSE_NOTIFY:) (const)
- [DTLS_ALERT_UNEXPECTED_MESSAGE:](#DTLS_ALERT_UNEXPECTED_MESSAGE:) (const)
- [DTLS_ALERT_HANDSHAKE_FAILURE:](#DTLS_ALERT_HANDSHAKE_FAILURE:) (const)
- [DTLS_ALERT_BAD_CERTIFICATE:](#DTLS_ALERT_BAD_CERTIFICATE:) (const)
- [DTLS_ALERT_ILLEGAL_PARAMETER:](#DTLS_ALERT_ILLEGAL_PARAMETER:) (const)
- [DTLS_ALERT_DECODE_ERROR:](#DTLS_ALERT_DECODE_ERROR:) (const)
- [DTLS_ALERT_DECRYPT_ERROR:](#DTLS_ALERT_DECRYPT_ERROR:) (const)
- [DTLS_ALERT_PROTOCOL_VERSION:](#DTLS_ALERT_PROTOCOL_VERSION:) (const)
- [DTLS_SUITE:](#DTLS_SUITE:) (const)
- [DtlsConfig](#DtlsConfig) (struct)
- [DtlsConn](#DtlsConn) (struct)
- [DtlsSrtpKeys](#DtlsSrtpKeys) (struct)
- [dtls_prf](#dtls_prf) (function)
- [dtls_fingerprint](#dtls_fingerprint) (function)
- [dtls_peer_fingerprint](#dtls_peer_fingerprint) (function)
- [dtls_export](#dtls_export) (function)
- [dtls_srtp_keys](#dtls_srtp_keys) (function)
- [dtls_state_name](#dtls_state_name) (function)
- [dtls_config](#dtls_config) (function)
- [dtls_new](#dtls_new) (function)
- [dtls_drain](#dtls_drain) (function)
- [dtls_read_app](#dtls_read_app) (function)
- [dtls_next_timeout](#dtls_next_timeout) (function)
- [dtls_receive](#dtls_receive) (function)
- [dtls_close](#dtls_close) (function)
- [dtls_send_app](#dtls_send_app) (function)
- [dtls_poll](#dtls_poll) (function)
- [dtls_start](#dtls_start) (function)

## DTLS_NEW:

```fides
pub const DTLS_NEW: i64 = 0;
```

*Defined at line 82*

---

## DTLS_CONNECTING:

```fides
pub const DTLS_CONNECTING: i64 = 1;
```

*Defined at line 83*

---

## DTLS_CONNECTED:

```fides
pub const DTLS_CONNECTED: i64 = 2;
```

*Defined at line 84*

---

## DTLS_FAILED:

```fides
pub const DTLS_FAILED: i64 = 3;
```

*Defined at line 85*

---

## DTLS_CLOSED:

```fides
pub const DTLS_CLOSED: i64 = 4;
```

*Defined at line 86*

---

## DTLS_CT_CCS:

```fides
pub const DTLS_CT_CCS: i64 = 20;
```

*Defined at line 88*

---

## DTLS_CT_ALERT:

```fides
pub const DTLS_CT_ALERT: i64 = 21;
```

*Defined at line 89*

---

## DTLS_CT_HANDSHAKE:

```fides
pub const DTLS_CT_HANDSHAKE: i64 = 22;
```

*Defined at line 90*

---

## DTLS_CT_APP:

```fides
pub const DTLS_CT_APP: i64 = 23;
```

*Defined at line 91*

---

## DTLS_HS_CLIENT_HELLO:

```fides
pub const DTLS_HS_CLIENT_HELLO: i64 = 1;
```

*Defined at line 93*

---

## DTLS_HS_SERVER_HELLO:

```fides
pub const DTLS_HS_SERVER_HELLO: i64 = 2;
```

*Defined at line 94*

---

## DTLS_HS_HELLO_VERIFY:

```fides
pub const DTLS_HS_HELLO_VERIFY: i64 = 3;
```

*Defined at line 95*

---

## DTLS_HS_CERTIFICATE:

```fides
pub const DTLS_HS_CERTIFICATE: i64 = 11;
```

*Defined at line 96*

---

## DTLS_HS_SERVER_KEY_EXCHANGE:

```fides
pub const DTLS_HS_SERVER_KEY_EXCHANGE: i64 = 12;
```

*Defined at line 97*

---

## DTLS_HS_CERTIFICATE_REQUEST:

```fides
pub const DTLS_HS_CERTIFICATE_REQUEST: i64 = 13;
```

*Defined at line 98*

---

## DTLS_HS_SERVER_HELLO_DONE:

```fides
pub const DTLS_HS_SERVER_HELLO_DONE: i64 = 14;
```

*Defined at line 99*

---

## DTLS_HS_CERTIFICATE_VERIFY:

```fides
pub const DTLS_HS_CERTIFICATE_VERIFY: i64 = 15;
```

*Defined at line 100*

---

## DTLS_HS_CLIENT_KEY_EXCHANGE:

```fides
pub const DTLS_HS_CLIENT_KEY_EXCHANGE: i64 = 16;
```

*Defined at line 101*

---

## DTLS_HS_FINISHED:

```fides
pub const DTLS_HS_FINISHED: i64 = 20;
```

*Defined at line 102*

---

## DTLS_ALERT_CLOSE_NOTIFY:

```fides
pub const DTLS_ALERT_CLOSE_NOTIFY: i64 = 0;
```

*Defined at line 104*

---

## DTLS_ALERT_UNEXPECTED_MESSAGE:

```fides
pub const DTLS_ALERT_UNEXPECTED_MESSAGE: i64 = 10;
```

*Defined at line 105*

---

## DTLS_ALERT_HANDSHAKE_FAILURE:

```fides
pub const DTLS_ALERT_HANDSHAKE_FAILURE: i64 = 40;
```

*Defined at line 106*

---

## DTLS_ALERT_BAD_CERTIFICATE:

```fides
pub const DTLS_ALERT_BAD_CERTIFICATE: i64 = 42;
```

*Defined at line 107*

---

## DTLS_ALERT_ILLEGAL_PARAMETER:

```fides
pub const DTLS_ALERT_ILLEGAL_PARAMETER: i64 = 47;
```

*Defined at line 108*

---

## DTLS_ALERT_DECODE_ERROR:

```fides
pub const DTLS_ALERT_DECODE_ERROR: i64 = 50;
```

*Defined at line 109*

---

## DTLS_ALERT_DECRYPT_ERROR:

```fides
pub const DTLS_ALERT_DECRYPT_ERROR: i64 = 51;
```

*Defined at line 110*

---

## DTLS_ALERT_PROTOCOL_VERSION:

```fides
pub const DTLS_ALERT_PROTOCOL_VERSION: i64 = 70;
```

*Defined at line 111*

---

## DTLS_SUITE:

TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256

```fides
pub const DTLS_SUITE: i64 = 49195;
```

*Defined at line 114*

---

## DtlsConfig

What one endpoint is: its role, certificate and P-256 private key,
the fingerprint the remote SDP gave ("" accepts any certificate: the
caller must then check dtls_peer_fingerprint itself), the SRTP
profiles it offers or accepts in preference order, the MTU, and 32
bytes of entropy. `peer` names the remote transport address (it goes
into the server's cookie).

```fides
pub struct DtlsConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `is_server` | `bool` |
| `cert_der` | `Vec<i64>` |
| `key` | `Vec<i64>` |
| `remote_fingerprint` | `str` |
| `srtp_profiles` | `Vec<i64>` |
| `cookie` | `bool` |
| `cookie_secret` | `Vec<i64>` |
| `peer` | `str` |
| `mtu` | `i64` |
| `seed` | `Vec<i64>` |
| `require_ems` | `bool` |
| `require_client_cert` | `bool` |
| `rto_ms` | `i64` |

*Defined at line 150*

---

## DtlsConn

A DTLS connection: state, record and handshake layers, and the I/O
queues (`out`: datagrams to send; `app_in`: application data read).

```fides
pub struct DtlsConn {
```

**Fields:**

| Name | Type |
|------|------|
| `cfg` | `DtlsConfig` |
| `state` | `i64` |
| `err` | `str` |
| `hs` | `i64` |
| `now` | `i64` |
| `rng_key` | `Vec<i64>` |
| `rng_ctr` | `i64` |
| `r_epoch` | `i64` |
| `w_epoch` | `i64` |
| `w_seqs` | `Vec<i64>` |
| `r_tops` | `Vec<i64>` |
| `r_wins` | `Vec<Vec<i64>>` |
| `pending` | `Vec<Vec<i64>>` |
| `cur_rec_seq` | `i64` |
| `cw_key` | `Vec<i64>` |
| `sw_key` | `Vec<i64>` |
| `cw_iv` | `Vec<i64>` |
| `sw_iv` | `Vec<i64>` |
| `hs_send_seq` | `i64` |
| `hs_recv_seq` | `i64` |
| `rb_seq` | `Vec<i64>` |
| `rb_type` | `Vec<i64>` |
| `rb_len` | `Vec<i64>` |
| `rb_got` | `Vec<i64>` |
| `rb_data` | `Vec<Vec<i64>>` |
| `rb_have` | `Vec<Vec<i64>>` |
| `transcript` | `Vec<i64>` |
| `client_random` | `Vec<i64>` |
| `server_random` | `Vec<i64>` |
| `cookie` | `Vec<i64>` |
| `eph_key` | `Vec<i64>` |
| `peer_eph` | `Vec<i64>` |
| `peer_cert` | `Vec<i64>` |
| `peer_key` | `Vec<i64>` |
| `cert_requested` | `bool` |
| `master` | `Vec<i64>` |
| `ems` | `bool` |
| `srtp_profile` | `i64` |
| `fl_types` | `Vec<i64>` |
| `fl_epochs` | `Vec<i64>` |
| `fl_msgs` | `Vec<Vec<i64>>` |
| `fresh_peer_flight` | `bool` |
| `peer_flight_seq` | `i64` |
| `timer_at` | `i64` |
| `rto` | `i64` |
| `rtx_round` | `i64` |
| `last_trigger` | `i64` |
| `out` | `Vec<Vec<i64>>` |
| `app_in` | `Vec<Vec<i64>>` |
| `retransmissions` | `i64` |
| `records_sent` | `i64` |
| `records_dropped` | `i64` |
| `hello_verify` | `i64` |

*Defined at line 168*

---

## DtlsSrtpKeys

SRTP master keys and salts from the DTLS-SRTP exporter (RFC 5764 §4.2).

```fides
pub struct DtlsSrtpKeys {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `profile` | `i64` |
| `client_key` | `Vec<i64>` |
| `client_salt` | `Vec<i64>` |
| `server_key` | `Vec<i64>` |
| `server_salt` | `Vec<i64>` |

*Defined at line 230*

---

## dtls_prf

The TLS 1.2 PRF with SHA-256 (RFC 5246 §5): P_SHA256(secret, label ||
seed), `n` bytes.

```fides
pub fn dtls_prf(secret: Vec<i64>, label: str, seed: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `secret`: `Vec<i64>`
- `label`: `str`
- `seed`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 332*

---

## dtls_fingerprint

"sha-256 AB:CD:..." of a DER certificate (the SDP a=fingerprint value).

```fides
pub fn dtls_fingerprint(der: Vec<i64>) -> str {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `str`

*Defined at line 349*

---

## dtls_peer_fingerprint

The peer certificate's fingerprint ("" before it arrived).

```fides
pub fn dtls_peer_fingerprint(c: &DtlsConn) -> str {
```

**Parameters:**

- `c`: `&DtlsConn`

**Returns:** `str`

*Defined at line 369*

---

## dtls_export

RFC 5705 keying material exporter without a context value; empty
before the handshake is done.

```fides
pub fn dtls_export(c: &DtlsConn, label: str, n: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `&DtlsConn`
- `label`: `str`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 378*

---

## dtls_srtp_keys

The SRTP master keys and salts of both directions (RFC 5764 §4.2).

```fides
pub fn dtls_srtp_keys(c: &DtlsConn) -> DtlsSrtpKeys {
```

**Parameters:**

- `c`: `&DtlsConn`

**Returns:** `DtlsSrtpKeys`

*Defined at line 387*

---

## dtls_state_name

```fides
pub fn dtls_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 405*

---

## dtls_config

A configuration from PEM text: the certificate ("CERTIFICATE") and its
P-256 key ("PRIVATE KEY", PKCS#8), the remote fingerprint, the three
SRTP profiles in the order WebRTC prefers, a 1200-byte MTU, cookies on
for a server, and a seed from the OS random generator. A key that does
not parse leaves `key` empty (dtls_new then fails the connection).

```fides
pub fn dtls_config(is_server: bool, cert_pem: str, key_pem: str, remote_fingerprint: str) -> DtlsConfig {
```

**Parameters:**

- `is_server`: `bool`
- `cert_pem`: `str`
- `key_pem`: `str`
- `remote_fingerprint`: `str`

**Returns:** `DtlsConfig`

*Defined at line 428*

---

## dtls_new

```fides
pub fn dtls_new(cfg: DtlsConfig) -> DtlsConn {
```

**Parameters:**

- `cfg`: `DtlsConfig`

**Returns:** `DtlsConn`

*Defined at line 451*

---

## dtls_drain

Datagrams to send, in order; the queue is emptied.

```fides
pub fn dtls_drain(c: &mut DtlsConn) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut DtlsConn`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 491*

---

## dtls_read_app

Application data received, in order; the queue is emptied.

```fides
pub fn dtls_read_app(c: &mut DtlsConn) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut DtlsConn`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 498*

---

## dtls_next_timeout

When dtls_poll next has work (-1: no timer running).

```fides
pub fn dtls_next_timeout(c: &DtlsConn) -> i64 {
```

**Parameters:**

- `c`: `&DtlsConn`

**Returns:** `i64`

*Defined at line 505*

---

## dtls_receive

Feed one received datagram (one or more records).

```fides
pub fn dtls_receive(c: &mut DtlsConn, dg: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut DtlsConn`
- `dg`: `Vec<i64>`
- `now`: `i64`

*Defined at line 563*

---

## dtls_close

Send close_notify and close.

```fides
pub fn dtls_close(c: &mut DtlsConn) {
```

**Parameters:**

- `c`: `&mut DtlsConn`

*Defined at line 684*

---

## dtls_send_app

Send application data (one record); false before the handshake is done.

```fides
pub fn dtls_send_app(c: &mut DtlsConn, data: Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&mut DtlsConn`
- `data`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 693*

---

## dtls_poll

Run the retransmission timer at time `now` (milliseconds).

```fides
pub fn dtls_poll(c: &mut DtlsConn, now: i64) {
```

**Parameters:**

- `c`: `&mut DtlsConn`
- `now`: `i64`

*Defined at line 825*

---

## dtls_start

Start the handshake (a client sends its ClientHello; a server waits).

```fides
pub fn dtls_start(c: &mut DtlsConn, now: i64) {
```

**Parameters:**

- `c`: `&mut DtlsConn`
- `now`: `i64`

*Defined at line 1287*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
