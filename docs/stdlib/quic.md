[All modules](index.md)

# quic -- Fides Documentation

## Contents

- [QUIC_V1:](#QUIC_V1:) (const)
- [QUIC_INITIAL:](#QUIC_INITIAL:) (const)
- [QUIC_0RTT:](#QUIC_0RTT:) (const)
- [QUIC_HANDSHAKE:](#QUIC_HANDSHAKE:) (const)
- [QUIC_RETRY:](#QUIC_RETRY:) (const)
- [QUIC_AEAD_AES_128_GCM:](#QUIC_AEAD_AES_128_GCM:) (const)
- [QUIC_AEAD_AES_256_GCM:](#QUIC_AEAD_AES_256_GCM:) (const)
- [QUIC_AEAD_CHACHA20_POLY1305:](#QUIC_AEAD_CHACHA20_POLY1305:) (const)
- [QUIC_VARINT_MAX:](#QUIC_VARINT_MAX:) (const)
- [QuicKeys](#QuicKeys) (struct)
- [QuicVarint](#QuicVarint) (struct)
- [QuicLongHeader](#QuicLongHeader) (struct)
- [QuicOpened](#QuicOpened) (struct)
- [quic_slice](#quic_slice) (function)
- [quic_varint_encode](#quic_varint_encode) (function)
- [quic_varint_encode_n](#quic_varint_encode_n) (function)
- [quic_varint_decode](#quic_varint_decode) (function)
- [quic_decode_pn](#quic_decode_pn) (function)
- [quic_pn_length](#quic_pn_length) (function)
- [quic_initial_secret](#quic_initial_secret) (function)
- [quic_client_initial_secret](#quic_client_initial_secret) (function)
- [quic_server_initial_secret](#quic_server_initial_secret) (function)
- [quic_aead_key_len](#quic_aead_key_len) (function)
- [quic_keys_from_secret](#quic_keys_from_secret) (function)
- [quic_initial_keys](#quic_initial_keys) (function)
- [quic_next_secret](#quic_next_secret) (function)
- [quic_updated_keys](#quic_updated_keys) (function)
- [quic_nonce](#quic_nonce) (function)
- [quic_hp_mask](#quic_hp_mask) (function)
- [quic_long_header](#quic_long_header) (function)
- [quic_short_header](#quic_short_header) (function)
- [quic_parse_long](#quic_parse_long) (function)
- [quic_protect](#quic_protect) (function)
- [quic_unprotect](#quic_unprotect) (function)
- [quic_unprotect_long](#quic_unprotect_long) (function)
- [quic_unprotect_short](#quic_unprotect_short) (function)
- [quic_retry_tag](#quic_retry_tag) (function)
- [quic_retry_valid](#quic_retry_valid) (function)
- [quic_retry_packet](#quic_retry_packet) (function)
- [QuicVersionNeg](#QuicVersionNeg) (struct)
- [quic_packet_version](#quic_packet_version) (function)
- [quic_version_supported](#quic_version_supported) (function)
- [quic_version_negotiation](#quic_version_negotiation) (function)
- [quic_parse_version_negotiation](#quic_parse_version_negotiation) (function)
- [quic_reset_token](#quic_reset_token) (function)
- [quic_stateless_reset](#quic_stateless_reset) (function)
- [quic_is_stateless_reset](#quic_is_stateless_reset) (function)

## QUIC_V1:

QUIC version 1 (RFC 9000).

```fides
pub const QUIC_V1: i64 = 1;
```

*Defined at line 42*

---

## QUIC_INITIAL:

Long-header packet types (RFC 9000 §17.2, bits 4-5 of the first byte).

```fides
pub const QUIC_INITIAL: i64 = 0;
```

*Defined at line 45*

---

## QUIC_0RTT:

```fides
pub const QUIC_0RTT: i64 = 1;
```

*Defined at line 46*

---

## QUIC_HANDSHAKE:

```fides
pub const QUIC_HANDSHAKE: i64 = 2;
```

*Defined at line 47*

---

## QUIC_RETRY:

```fides
pub const QUIC_RETRY: i64 = 3;
```

*Defined at line 48*

---

## QUIC_AEAD_AES_128_GCM:

The AEADs QUIC v1 uses (RFC 9001 §5.3): the TLS 1.3 cipher suite's.

```fides
pub const QUIC_AEAD_AES_128_GCM: i64 = 1;
```

*Defined at line 51*

---

## QUIC_AEAD_AES_256_GCM:

```fides
pub const QUIC_AEAD_AES_256_GCM: i64 = 2;
```

*Defined at line 52*

---

## QUIC_AEAD_CHACHA20_POLY1305:

```fides
pub const QUIC_AEAD_CHACHA20_POLY1305: i64 = 3;
```

*Defined at line 53*

---

## QUIC_VARINT_MAX:

The largest varint (2^62 - 1).

```fides
pub const QUIC_VARINT_MAX: i64 = 4611686018427387903;
```

*Defined at line 56*

---

## QuicKeys

The packet-protection keys of one direction at one encryption level:
the AEAD key, the 12-byte IV and the header-protection key.
`aead` is 0 for an unknown AEAD (every function here then fails).

```fides
pub struct QuicKeys {
```

**Fields:**

| Name | Type |
|------|------|
| `aead` | `i64` |
| `key` | `Vec<i64>` |
| `iv` | `Vec<i64>` |
| `hp` | `Vec<i64>` |

*Defined at line 61*

---

## QuicVarint

A decoded varint: `ok` false when the bytes run out; `len` the bytes read.

```fides
pub struct QuicVarint {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `value` | `i64` |
| `len` | `i64` |

*Defined at line 69*

---

## QuicLongHeader

A parsed long header (RFC 9000 §17.2) of a protected packet: the
fields before the packet number, where the packet number starts
(`pn_offset`) and where the packet ends (`end`: coalesced packets
follow). The first byte is still protected: its low four bits are not
meaningful until header protection is removed. A Retry has no packet
number: `pn_offset` and `end` are the packet's length.

```fides
pub struct QuicLongHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ptype` | `i64` |
| `version` | `i64` |
| `dcid` | `Vec<i64>` |
| `scid` | `Vec<i64>` |
| `token` | `Vec<i64>` |
| `pn_offset` | `i64` |
| `end` | `i64` |

*Defined at line 81*

---

## QuicOpened

A received packet with protection removed: the unprotected header
(through the packet number), the full packet number, the frames and
where the packet ended. `ok` false: malformed, or the AEAD rejected it
(then nothing decrypted is returned).

```fides
pub struct QuicOpened {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `header` | `Vec<i64>` |
| `pn` | `i64` |
| `payload` | `Vec<i64>` |
| `end` | `i64` |

*Defined at line 96*

---

## quic_slice

Bytes `from` .. `to` of `v` (clamped to `v`), as a new Vec.

```fides
pub fn quic_slice(v: Vec<i64>, from: i64, to: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 113*

---

## quic_varint_encode

The varint encoding of 0 <= v < 2^62 on the fewest bytes (1, 2, 4 or
8). Empty for a value out of range.

```fides
pub fn quic_varint_encode(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 143*

---

## quic_varint_encode_n

The varint encoding of v on exactly `n` bytes (1, 2, 4 or 8): QUIC
allows a longer encoding than needed (the Length field of a long header
is often 2 bytes). Empty when v does not fit or n is not a varint length.

```fides
pub fn quic_varint_encode_n(v: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 159*

---

## quic_varint_decode

The varint at `off` of `b`.

```fides
pub fn quic_varint_decode(b: Vec<i64>, off: i64) -> QuicVarint {
```

**Parameters:**

- `b`: `Vec<i64>`
- `off`: `i64`

**Returns:** `QuicVarint`

*Defined at line 191*

---

## quic_decode_pn

The full packet number of a truncated one of `pn_nbits` bits (8, 16,
24 or 32), given the largest packet number received so far in its
packet number space (-1 when none was).

```fides
pub fn quic_decode_pn(largest_pn: i64, truncated_pn: i64, pn_nbits: i64) -> i64 {
```

**Parameters:**

- `largest_pn`: `i64`
- `truncated_pn`: `i64`
- `pn_nbits`: `i64`

**Returns:** `i64`

*Defined at line 220*

---

## quic_pn_length

The packet number length (1..4 bytes) to send `pn` with, when the
peer has acknowledged up to `largest_acked` (-1: nothing yet): enough
for twice the unacknowledged range (RFC 9000 §17.1, Appendix A.2).

```fides
pub fn quic_pn_length(pn: i64, largest_acked: i64) -> i64 {
```

**Parameters:**

- `pn`: `i64`
- `largest_acked`: `i64`

**Returns:** `i64`

*Defined at line 238*

---

## quic_initial_secret

```fides
pub fn quic_initial_secret(dcid: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dcid`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 287*

---

## quic_client_initial_secret

```fides
pub fn quic_client_initial_secret(dcid: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dcid`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 299*

---

## quic_server_initial_secret

```fides
pub fn quic_server_initial_secret(dcid: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dcid`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 305*

---

## quic_aead_key_len

The key length of an AEAD (also its header-protection key's length);
0 for an unknown one.

```fides
pub fn quic_aead_key_len(aead: i64) -> i64 {
```

**Parameters:**

- `aead`: `i64`

**Returns:** `i64`

*Defined at line 311*

---

## quic_keys_from_secret

```fides
pub fn quic_keys_from_secret(@secret secret: Vec<i64>, aead: i64) -> QuicKeys {
```

**Parameters:**

- `@secret secret`: `Vec<i64>`
- `aead`: `i64`

**Returns:** `QuicKeys`

*Defined at line 325*

---

## quic_initial_keys

```fides
pub fn quic_initial_keys(dcid: Vec<i64>, server: bool) -> QuicKeys {
```

**Parameters:**

- `dcid`: `Vec<i64>`
- `server`: `bool`

**Returns:** `QuicKeys`

*Defined at line 341*

---

## quic_next_secret

```fides
pub fn quic_next_secret(@secret secret: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret secret`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 351*

---

## quic_updated_keys

```fides
pub fn quic_updated_keys(@secret old: QuicKeys, @secret next_secret: Vec<i64>, aead: i64) -> QuicKeys {
```

**Parameters:**

- `@secret old`: `QuicKeys`
- `@secret next_secret`: `Vec<i64>`
- `aead`: `i64`

**Returns:** `QuicKeys`

*Defined at line 360*

---

## quic_nonce

```fides
pub fn quic_nonce(@secret iv: Vec<i64>, pn: i64) -> Vec<i64> {
```

**Parameters:**

- `@secret iv`: `Vec<i64>`
- `pn`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 370*

---

## quic_hp_mask

```fides
pub fn quic_hp_mask(@secret keys: QuicKeys, sample: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret keys`: `QuicKeys`
- `sample`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 394*

---

## quic_long_header

An unprotected long header (RFC 9000 §17.2): the first byte (form,
fixed bit, type, packet number length), version, connection IDs, the
token of an Initial, the Length field (2 bytes, as RFC 9001 Appendix A
sends it; 4 when it does not fit) covering the packet number,
`payload_len` bytes of frames and the 16-byte tag, then the packet
number truncated to `pn_len` (1..4) bytes. Empty for a Retry (it has no
packet number) or a bad length.

```fides
pub fn quic_long_header(ptype: i64, version: i64, dcid: Vec<i64>, scid: Vec<i64>, token: Vec<i64>, pn: i64, pn_len: i64, payload_len: i64) -> Vec<i64> {
```

**Parameters:**

- `ptype`: `i64`
- `version`: `i64`
- `dcid`: `Vec<i64>`
- `scid`: `Vec<i64>`
- `token`: `Vec<i64>`
- `pn`: `i64`
- `pn_len`: `i64`
- `payload_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 465*

---

## quic_short_header

An unprotected short (1-RTT) header (RFC 9000 §17.3.1): the first byte
with the spin and key-phase bits, the Destination Connection ID and the
packet number truncated to `pn_len` (1..4) bytes.

```fides
pub fn quic_short_header(dcid: Vec<i64>, pn: i64, pn_len: i64, key_phase: bool, spin: bool) -> Vec<i64> {
```

**Parameters:**

- `dcid`: `Vec<i64>`
- `pn`: `i64`
- `pn_len`: `i64`
- `key_phase`: `bool`
- `spin`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 500*

---

## quic_parse_long

Parse the long header of the packet at the start of `p` (RFC 9000
§17.2): `ok` false for a short header, a Version Negotiation packet,
a connection ID over 20 bytes or a packet cut short.

```fides
pub fn quic_parse_long(p: Vec<i64>) -> QuicLongHeader {
```

**Parameters:**

- `p`: `Vec<i64>`

**Returns:** `QuicLongHeader`

*Defined at line 525*

---

## quic_protect

```fides
pub fn quic_protect(@secret keys: QuicKeys, header: Vec<i64>, pn: i64, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret keys`: `QuicKeys`
- `header`: `Vec<i64>`
- `pn`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 591*

---

## quic_unprotect

```fides
pub fn quic_unprotect(@secret keys: QuicKeys, p: Vec<i64>, pn_offset: i64, end: i64, largest_pn: i64) -> QuicOpened {
```

**Parameters:**

- `@secret keys`: `QuicKeys`
- `p`: `Vec<i64>`
- `pn_offset`: `i64`
- `end`: `i64`
- `largest_pn`: `i64`

**Returns:** `QuicOpened`

*Defined at line 641*

---

## quic_unprotect_long

```fides
pub fn quic_unprotect_long(@secret keys: QuicKeys, p: Vec<i64>, largest_pn: i64) -> QuicOpened {
```

**Parameters:**

- `@secret keys`: `QuicKeys`
- `p`: `Vec<i64>`
- `largest_pn`: `i64`

**Returns:** `QuicOpened`

*Defined at line 685*

---

## quic_unprotect_short

```fides
pub fn quic_unprotect_short(@secret keys: QuicKeys, p: Vec<i64>, dcid_len: i64, largest_pn: i64) -> QuicOpened {
```

**Parameters:**

- `@secret keys`: `QuicKeys`
- `p`: `Vec<i64>`
- `dcid_len`: `i64`
- `largest_pn`: `i64`

**Returns:** `QuicOpened`

*Defined at line 697*

---

## quic_retry_tag

The Retry Integrity Tag: AES-128-GCM with QUIC v1's fixed key and
nonce over the Retry Pseudo-Packet (the original Destination Connection
ID, length-prefixed, then the Retry packet without its tag), nothing
encrypted.

```fides
pub fn quic_retry_tag(odcid: Vec<i64>, retry_without_tag: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `odcid`: `Vec<i64>`
- `retry_without_tag`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 710*

---

## quic_retry_valid

Whether a received Retry packet's tag matches the Destination
Connection ID the client first sent; compared over all 16 bytes.

```fides
pub fn quic_retry_valid(odcid: Vec<i64>, retry: Vec<i64>) -> bool {
```

**Parameters:**

- `odcid`: `Vec<i64>`
- `retry`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 750*

---

## quic_retry_packet

A Retry packet (RFC 9000 §17.2.5) a server sends to the client whose
Initial carried Source Connection ID `dcid` and Destination Connection
ID `odcid`: its new connection ID `scid`, the token the client must
send back, and the integrity tag. `unused` gives the four unused bits
of the first byte.

```fides
pub fn quic_retry_packet(dcid: Vec<i64>, scid: Vec<i64>, odcid: Vec<i64>, token: Vec<i64>, unused: i64) -> Vec<i64> {
```

**Parameters:**

- `dcid`: `Vec<i64>`
- `scid`: `Vec<i64>`
- `odcid`: `Vec<i64>`
- `token`: `Vec<i64>`
- `unused`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 773*

---

## QuicVersionNeg

A Version Negotiation packet: the connection IDs (echoed from the
client's packet, swapped) and the versions the server supports.

```fides
pub struct QuicVersionNeg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `dcid` | `Vec<i64>` |
| `scid` | `Vec<i64>` |
| `versions` | `Vec<i64>` |

*Defined at line 797*

---

## quic_packet_version

The version field of the long-header packet a datagram starts with:
0 for a Version Negotiation packet, -1 for a short header or a
datagram too short to tell.

```fides
pub fn quic_packet_version(d: Vec<i64>) -> i64 {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 807*

---

## quic_version_supported

Whether a version is one this implementation speaks (QUIC v1).

```fides
pub fn quic_version_supported(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 815*

---

## quic_version_negotiation

The Version Negotiation packet answering the long-header packet that
starts `d` (its connection IDs swapped, which may be up to 255 bytes
for an unknown version), listing `versions`; `unused` gives the seven
unused bits of the first byte. [] for anything that must not be
answered: a short header, a Version Negotiation packet, a datagram cut
short.

```fides
pub fn quic_version_negotiation(d: Vec<i64>, versions: Vec<i64>, unused: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `Vec<i64>`
- `versions`: `Vec<i64>`
- `unused`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 825*

---

## quic_parse_version_negotiation

Parse a Version Negotiation packet; `ok` false when `p` is not one (a
version list must hold whole 4-byte versions).

```fides
pub fn quic_parse_version_negotiation(p: Vec<i64>) -> QuicVersionNeg {
```

**Parameters:**

- `p`: `Vec<i64>`

**Returns:** `QuicVersionNeg`

*Defined at line 863*

---

## quic_reset_token

```fides
pub fn quic_reset_token(@secret key: Vec<i64>, cid: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret key`: `Vec<i64>`
- `cid`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 897*

---

## quic_stateless_reset

A Stateless Reset answering a packet of `trigger_len` bytes: a short
header's first byte, unpredictable bytes from `random` and the 16-byte
token, shorter than the packet that caused it (so that two endpoints
cannot reset each other forever) and at most 43 bytes. [] when the
trigger is too short to answer (a reset is at least 21 bytes).

```fides
pub fn quic_stateless_reset(token: Vec<i64>, trigger_len: i64, random: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `token`: `Vec<i64>`
- `trigger_len`: `i64`
- `random`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 906*

---

## quic_is_stateless_reset

Whether a datagram ends with the stateless reset token `token` (§10.3.1:
a short-header-looking datagram of at least 21 bytes), compared over
all 16 bytes.

```fides
pub fn quic_is_stateless_reset(d: Vec<i64>, token: Vec<i64>) -> bool {
```

**Parameters:**

- `d`: `Vec<i64>`
- `token`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 928*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
