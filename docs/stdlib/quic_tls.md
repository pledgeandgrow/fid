[All modules](index.md)

# quic_tls -- Fides Documentation

## Contents

- [QT_CLIENT_HELLO:](#QT_CLIENT_HELLO:) (const)
- [QT_SERVER_HELLO:](#QT_SERVER_HELLO:) (const)
- [QT_NEW_SESSION_TICKET:](#QT_NEW_SESSION_TICKET:) (const)
- [QT_ENCRYPTED_EXTENSIONS:](#QT_ENCRYPTED_EXTENSIONS:) (const)
- [QT_CERTIFICATE:](#QT_CERTIFICATE:) (const)
- [QT_CERTIFICATE_VERIFY:](#QT_CERTIFICATE_VERIFY:) (const)
- [QT_FINISHED:](#QT_FINISHED:) (const)
- [QT_TLS_AES_128_GCM_SHA256:](#QT_TLS_AES_128_GCM_SHA256:) (const)
- [QT_TLS_CHACHA20_POLY1305_SHA256:](#QT_TLS_CHACHA20_POLY1305_SHA256:) (const)
- [QA_UNEXPECTED_MESSAGE:](#QA_UNEXPECTED_MESSAGE:) (const)
- [QA_HANDSHAKE_FAILURE:](#QA_HANDSHAKE_FAILURE:) (const)
- [QA_BAD_CERTIFICATE:](#QA_BAD_CERTIFICATE:) (const)
- [QA_UNSUPPORTED_CERTIFICATE:](#QA_UNSUPPORTED_CERTIFICATE:) (const)
- [QA_ILLEGAL_PARAMETER:](#QA_ILLEGAL_PARAMETER:) (const)
- [QA_DECODE_ERROR:](#QA_DECODE_ERROR:) (const)
- [QA_DECRYPT_ERROR:](#QA_DECRYPT_ERROR:) (const)
- [QA_PROTOCOL_VERSION:](#QA_PROTOCOL_VERSION:) (const)
- [QA_INTERNAL_ERROR:](#QA_INTERNAL_ERROR:) (const)
- [QA_MISSING_EXTENSION:](#QA_MISSING_EXTENSION:) (const)
- [QA_NO_APPLICATION_PROTOCOL:](#QA_NO_APPLICATION_PROTOCOL:) (const)
- [QTS_CLIENT_START:](#QTS_CLIENT_START:) (const)
- [QTS_WAIT_SH:](#QTS_WAIT_SH:) (const)
- [QTS_WAIT_EE:](#QTS_WAIT_EE:) (const)
- [QTS_WAIT_CERT:](#QTS_WAIT_CERT:) (const)
- [QTS_WAIT_CV:](#QTS_WAIT_CV:) (const)
- [QTS_WAIT_FIN:](#QTS_WAIT_FIN:) (const)
- [QTS_WAIT_CH:](#QTS_WAIT_CH:) (const)
- [QTS_WAIT_CLIENT_FIN:](#QTS_WAIT_CLIENT_FIN:) (const)
- [QTS_DONE:](#QTS_DONE:) (const)
- [QTS_FAILED:](#QTS_FAILED:) (const)
- [QuicTls](#QuicTls) (struct)
- [QuicTicket](#QuicTicket) (struct)
- [quic_ticket_none](#quic_ticket_none) (function)
- [quic_ticket_valid](#quic_ticket_valid) (function)
- [quic_self_signed_cert](#quic_self_signed_cert) (function)
- [quic_cert_ed25519_key](#quic_cert_ed25519_key) (function)
- [quic_cert_self_signed_ok](#quic_cert_self_signed_ok) (function)
- [quic_tls_new](#quic_tls_new) (function)
- [quic_tls_suite_aead](#quic_tls_suite_aead) (function)
- [quic_tls_set_roots](#quic_tls_set_roots) (function)
- [quic_tls_client_start](#quic_tls_client_start) (function)
- [quic_tls_input](#quic_tls_input) (function)
- [quic_tls_scheme_verify](#quic_tls_scheme_verify) (function)

## QT_CLIENT_HELLO:

Handshake message types (RFC 8446 §4).

```fides
pub const QT_CLIENT_HELLO: i64 = 1;
```

*Defined at line 69*

---

## QT_SERVER_HELLO:

```fides
pub const QT_SERVER_HELLO: i64 = 2;
```

*Defined at line 70*

---

## QT_NEW_SESSION_TICKET:

```fides
pub const QT_NEW_SESSION_TICKET: i64 = 4;
```

*Defined at line 71*

---

## QT_ENCRYPTED_EXTENSIONS:

```fides
pub const QT_ENCRYPTED_EXTENSIONS: i64 = 8;
```

*Defined at line 72*

---

## QT_CERTIFICATE:

```fides
pub const QT_CERTIFICATE: i64 = 11;
```

*Defined at line 73*

---

## QT_CERTIFICATE_VERIFY:

```fides
pub const QT_CERTIFICATE_VERIFY: i64 = 15;
```

*Defined at line 74*

---

## QT_FINISHED:

```fides
pub const QT_FINISHED: i64 = 20;
```

*Defined at line 75*

---

## QT_TLS_AES_128_GCM_SHA256:

Cipher suites (RFC 8446 §B.4).

```fides
pub const QT_TLS_AES_128_GCM_SHA256: i64 = 0x1301;
```

*Defined at line 78*

---

## QT_TLS_CHACHA20_POLY1305_SHA256:

```fides
pub const QT_TLS_CHACHA20_POLY1305_SHA256: i64 = 0x1303;
```

*Defined at line 79*

---

## QA_UNEXPECTED_MESSAGE:

Alerts (RFC 8446 §6).

```fides
pub const QA_UNEXPECTED_MESSAGE: i64 = 10;
```

*Defined at line 82*

---

## QA_HANDSHAKE_FAILURE:

```fides
pub const QA_HANDSHAKE_FAILURE: i64 = 40;
```

*Defined at line 83*

---

## QA_BAD_CERTIFICATE:

```fides
pub const QA_BAD_CERTIFICATE: i64 = 42;
```

*Defined at line 84*

---

## QA_UNSUPPORTED_CERTIFICATE:

```fides
pub const QA_UNSUPPORTED_CERTIFICATE: i64 = 43;
```

*Defined at line 85*

---

## QA_ILLEGAL_PARAMETER:

```fides
pub const QA_ILLEGAL_PARAMETER: i64 = 47;
```

*Defined at line 86*

---

## QA_DECODE_ERROR:

```fides
pub const QA_DECODE_ERROR: i64 = 50;
```

*Defined at line 87*

---

## QA_DECRYPT_ERROR:

```fides
pub const QA_DECRYPT_ERROR: i64 = 51;
```

*Defined at line 88*

---

## QA_PROTOCOL_VERSION:

```fides
pub const QA_PROTOCOL_VERSION: i64 = 70;
```

*Defined at line 89*

---

## QA_INTERNAL_ERROR:

```fides
pub const QA_INTERNAL_ERROR: i64 = 80;
```

*Defined at line 90*

---

## QA_MISSING_EXTENSION:

```fides
pub const QA_MISSING_EXTENSION: i64 = 109;
```

*Defined at line 91*

---

## QA_NO_APPLICATION_PROTOCOL:

```fides
pub const QA_NO_APPLICATION_PROTOCOL: i64 = 120;
```

*Defined at line 92*

---

## QTS_CLIENT_START:

States of the handshake.

```fides
pub const QTS_CLIENT_START: i64 = 0;
```

*Defined at line 95*

---

## QTS_WAIT_SH:

```fides
pub const QTS_WAIT_SH: i64 = 1;
```

*Defined at line 96*

---

## QTS_WAIT_EE:

```fides
pub const QTS_WAIT_EE: i64 = 2;
```

*Defined at line 97*

---

## QTS_WAIT_CERT:

```fides
pub const QTS_WAIT_CERT: i64 = 3;
```

*Defined at line 98*

---

## QTS_WAIT_CV:

```fides
pub const QTS_WAIT_CV: i64 = 4;
```

*Defined at line 99*

---

## QTS_WAIT_FIN:

```fides
pub const QTS_WAIT_FIN: i64 = 5;
```

*Defined at line 100*

---

## QTS_WAIT_CH:

```fides
pub const QTS_WAIT_CH: i64 = 6;
```

*Defined at line 101*

---

## QTS_WAIT_CLIENT_FIN:

```fides
pub const QTS_WAIT_CLIENT_FIN: i64 = 7;
```

*Defined at line 102*

---

## QTS_DONE:

```fides
pub const QTS_DONE: i64 = 8;
```

*Defined at line 103*

---

## QTS_FAILED:

```fides
pub const QTS_FAILED: i64 = 9;
```

*Defined at line 104*

---

## QuicTls

One end of the handshake.
- `out_initial`, `out_handshake`: bytes to send in CRYPTO frames at
that level (the connection takes them and clears the field);
- `hs_ready` once the handshake traffic secrets exist (`hs_client`,
`hs_server`), `ap_ready` once the application ones do (`ap_client`,
`ap_server`; a server has them as soon as it sent its Finished, and
may send 1-RTT packets but must not read them before `complete`);
- `complete` once the peer's Finished checked out (server) or ours
is queued (client);
- `alert` -1, or the alert the handshake failed with (`why` says why).

```fides
pub struct QuicTls {
```

**Fields:**

| Name | Type |
|------|------|
| `server` | `bool` |
| `state` | `i64` |
| `suites` | `Vec<i64>` |
| `alpn_offer` | `Vec<str>` |
| `alpn` | `str` |
| `server_name` | `str` |
| `peer_server_name` | `str` |
| `cert` | `Vec<i64>` |
| `key_seed` | `Vec<i64>` |
| `peer_key` | `Vec<i64>` |
| `peer_cert_key` | `Vec<i64>` |
| `peer_spki` | `Spki` |
| `peer_chain` | `Vec<X509Cert>` |
| `trust_anchors` | `Vec<X509Cert>` |
| `eph_secret` | `Vec<i64>` |
| `random` | `Vec<i64>` |
| `local_params` | `Vec<i64>` |
| `peer_params` | `Vec<i64>` |
| `suite` | `i64` |
| `transcript` | `Vec<i64>` |
| `hs_client` | `Vec<i64>` |
| `hs_server` | `Vec<i64>` |
| `master` | `Vec<i64>` |
| `ap_client` | `Vec<i64>` |
| `ap_server` | `Vec<i64>` |
| `in_initial` | `Vec<i64>` |
| `in_handshake` | `Vec<i64>` |
| `in_app` | `Vec<i64>` |
| `out_initial` | `Vec<i64>` |
| `out_handshake` | `Vec<i64>` |
| `hs_ready` | `bool` |
| `ap_ready` | `bool` |
| `complete` | `bool` |
| `alert` | `i64` |
| `why` | `str` |
| `key_share_first` | `bool` |
| `hrr` | `bool` |
| `hrr_suite` | `i64` |
| `cookie` | `Vec<i64>` |
| `now` | `i64` |
| `extra_rand` | `Vec<i64>` |
| `ticket_key` | `Vec<i64>` |
| `early_data` | `bool` |
| `offer` | `QuicTicket` |
| `has_offer` | `bool` |
| `psk` | `Vec<i64>` |
| `resumed` | `bool` |
| `early_offered` | `bool` |
| `early_ready` | `bool` |
| `early_accepted` | `bool` |
| `early_client` | `Vec<i64>` |
| `resumption` | `Vec<i64>` |
| `ticket` | `QuicTicket` |
| `has_ticket` | `bool` |
| `tickets_sent` | `i64` |
| `out_app` | `Vec<i64>` |

*Defined at line 120*

---

## QuicTicket

A session ticket a client keeps to resume a connection later (RFC 8446
§4.6.1): the server's opaque `ticket`, the PSK it stands for (secret:
keep it as you keep a key), the cipher suite, ALPN and server name of
the connection it came from, the age obfuscator, its lifetime and when
it was received (ms), the most early data the server takes (0: no
0-RTT; QUIC requires 0xffffffff), and the server's transport parameters
(the limits 0-RTT data must stay within, RFC 9000 §7.4.1).

```fides
pub struct QuicTicket {
```

**Fields:**

| Name | Type |
|------|------|
| `ticket` | `Vec<i64>` |
| `psk` | `Vec<i64>` |
| `suite` | `i64` |
| `alpn` | `str` |
| `server_name` | `str` |
| `age_add` | `i64` |
| `lifetime_ms` | `i64` |
| `received_ms` | `i64` |
| `max_early_data` | `i64` |
| `params` | `Vec<i64>` |

*Defined at line 199*

---

## quic_ticket_none

No ticket.

```fides
pub fn quic_ticket_none() -> QuicTicket {
```

**Returns:** `QuicTicket`

*Defined at line 213*

---

## quic_ticket_valid

Whether a ticket can still be offered at `now` (ms).

```fides
pub fn quic_ticket_valid(tk: &QuicTicket, now: i64) -> bool {
```

**Parameters:**

- `tk`: `&QuicTicket`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 220*

---

## quic_self_signed_cert

A self-signed X.509 v3 certificate (DER) for `name` whose key is the
Ed25519 key of the 32-byte `seed`, signed with that key.

```fides
pub fn quic_self_signed_cert(seed: Vec<i64>, name: str) -> Vec<i64> {
```

**Parameters:**

- `seed`: `Vec<i64>`
- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 582*

---

## quic_cert_ed25519_key

The Ed25519 public key of a certificate (DER); empty when it is not a
well-formed certificate for an Ed25519 key.

```fides
pub fn quic_cert_ed25519_key(cert: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cert`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 716*

---

## quic_cert_self_signed_ok

Whether a certificate is signed with Ed25519 by its own key.

```fides
pub fn quic_cert_self_signed_ok(cert: Vec<i64>) -> bool {
```

**Parameters:**

- `cert`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 725*

---

## quic_tls_new

A handshake end. `random` is 32 bytes and `eph_secret` the 32-byte
X25519 secret (the connection draws both). Client: `alpn` the protocols
it offers, `server_name` the SNI (may be empty), `peer_key` the server's
Ed25519 public key to pin (empty: accept any). Server: `alpn` the
protocols it accepts in its order of preference, `cert` its DER
certificate and `key_seed` the 32-byte seed of the certificate's key.
`suites` the cipher suites in order of preference. `params` the
encoded transport parameters to send.

```fides
pub fn quic_tls_new(server: bool, suites: Vec<i64>, alpn: Vec<str>, server_name: str, cert: Vec<i64>, key_seed: Vec<i64>, peer_key: Vec<i64>, random: Vec<i64>, eph_secret: Vec<i64>, params: Vec<i64>) -> QuicTls {
```

**Parameters:**

- `server`: `bool`
- `suites`: `Vec<i64>`
- `alpn`: `Vec<str>`
- `server_name`: `str`
- `cert`: `Vec<i64>`
- `key_seed`: `Vec<i64>`
- `peer_key`: `Vec<i64>`
- `random`: `Vec<i64>`
- `eph_secret`: `Vec<i64>`
- `params`: `Vec<i64>`

**Returns:** `QuicTls`

*Defined at line 743*

---

## quic_tls_suite_aead

The AEAD of QUIC packet protection for a cipher suite (quic.fd).

```fides
pub fn quic_tls_suite_aead(suite: i64) -> i64 {
```

**Parameters:**

- `suite`: `i64`

**Returns:** `i64`

*Defined at line 756*

---

## quic_tls_set_roots

The trust anchors a client verifies the server's certificate chain
against (x509_verify_chain at the wall clock); with `anchors` empty a
client accepts any certificate. Set before the first input; a pinned
`peer_key` takes precedence over anchors.

```fides
pub fn quic_tls_set_roots(t: &mut QuicTls, anchors: Vec<X509Cert>) {
```

**Parameters:**

- `t`: `&mut QuicTls`
- `anchors`: `Vec<X509Cert>`

*Defined at line 770*

---

## quic_tls_client_start

Start a client: queue the ClientHello at the Initial level. With a
ticket to offer (`offer`, `has_offer`) it offers resumption, and early
data when `early_data` is set and the ticket allows it: `early_ready`
then tells that `early_client` holds the 0-RTT traffic secret.

```fides
pub fn quic_tls_client_start(t: &mut QuicTls) {
```

**Parameters:**

- `t`: `&mut QuicTls`

*Defined at line 840*

---

## quic_tls_input

Feed the bytes of CRYPTO frames received at `level` (in order: the
connection reassembles them). Complete messages are processed; the
state, secrets, output and alert change accordingly.

```fides
pub fn quic_tls_input(t: &mut QuicTls, level: i64, data: Vec<i64>) {
```

**Parameters:**

- `t`: `&mut QuicTls`
- `level`: `i64`
- `data`: `Vec<i64>`

*Defined at line 999*

---

## quic_tls_scheme_verify

Whether a TLS 1.3 `scheme` signature over `content` verifies under the
certificate's SubjectPublicKeyInfo `k` (RFC 8446 §4.2.3/§4.4.3):
Ed25519, ECDSA on the named curves, RSA-PSS (salt length = digest
length) and RSA PKCS#1 v1.5. A key restricted to RSASSA-PSS does not
verify a PKCS#1 v1.5 signature. Exposed for tests and DTLS reuse.

```fides
pub fn quic_tls_scheme_verify(k: Spki, scheme: i64, content: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `k`: `Spki`
- `scheme`: `i64`
- `content`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 2015*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
