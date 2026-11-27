[All modules](index.md)

# quic_frame -- Fides Documentation

## Contents

- [QF_PADDING:](#QF_PADDING:) (const)
- [QF_PING:](#QF_PING:) (const)
- [QF_ACK:](#QF_ACK:) (const)
- [QF_ACK_ECN:](#QF_ACK_ECN:) (const)
- [QF_RESET_STREAM:](#QF_RESET_STREAM:) (const)
- [QF_STOP_SENDING:](#QF_STOP_SENDING:) (const)
- [QF_CRYPTO:](#QF_CRYPTO:) (const)
- [QF_NEW_TOKEN:](#QF_NEW_TOKEN:) (const)
- [QF_STREAM:](#QF_STREAM:) (const)
- [QF_MAX_DATA:](#QF_MAX_DATA:) (const)
- [QF_MAX_STREAM_DATA:](#QF_MAX_STREAM_DATA:) (const)
- [QF_MAX_STREAMS_BIDI:](#QF_MAX_STREAMS_BIDI:) (const)
- [QF_MAX_STREAMS_UNI:](#QF_MAX_STREAMS_UNI:) (const)
- [QF_DATA_BLOCKED:](#QF_DATA_BLOCKED:) (const)
- [QF_STREAM_DATA_BLOCKED:](#QF_STREAM_DATA_BLOCKED:) (const)
- [QF_STREAMS_BLOCKED_BIDI:](#QF_STREAMS_BLOCKED_BIDI:) (const)
- [QF_STREAMS_BLOCKED_UNI:](#QF_STREAMS_BLOCKED_UNI:) (const)
- [QF_NEW_CONNECTION_ID:](#QF_NEW_CONNECTION_ID:) (const)
- [QF_RETIRE_CONNECTION_ID:](#QF_RETIRE_CONNECTION_ID:) (const)
- [QF_PATH_CHALLENGE:](#QF_PATH_CHALLENGE:) (const)
- [QF_PATH_RESPONSE:](#QF_PATH_RESPONSE:) (const)
- [QF_CONNECTION_CLOSE:](#QF_CONNECTION_CLOSE:) (const)
- [QF_CONNECTION_CLOSE_APP:](#QF_CONNECTION_CLOSE_APP:) (const)
- [QF_HANDSHAKE_DONE:](#QF_HANDSHAKE_DONE:) (const)
- [QE_NO_ERROR:](#QE_NO_ERROR:) (const)
- [QE_INTERNAL_ERROR:](#QE_INTERNAL_ERROR:) (const)
- [QE_CONNECTION_REFUSED:](#QE_CONNECTION_REFUSED:) (const)
- [QE_FLOW_CONTROL_ERROR:](#QE_FLOW_CONTROL_ERROR:) (const)
- [QE_STREAM_LIMIT_ERROR:](#QE_STREAM_LIMIT_ERROR:) (const)
- [QE_STREAM_STATE_ERROR:](#QE_STREAM_STATE_ERROR:) (const)
- [QE_FINAL_SIZE_ERROR:](#QE_FINAL_SIZE_ERROR:) (const)
- [QE_FRAME_ENCODING_ERROR:](#QE_FRAME_ENCODING_ERROR:) (const)
- [QE_TRANSPORT_PARAMETER_ERROR:](#QE_TRANSPORT_PARAMETER_ERROR:) (const)
- [QE_CONNECTION_ID_LIMIT_ERROR:](#QE_CONNECTION_ID_LIMIT_ERROR:) (const)
- [QE_PROTOCOL_VIOLATION:](#QE_PROTOCOL_VIOLATION:) (const)
- [QE_INVALID_TOKEN:](#QE_INVALID_TOKEN:) (const)
- [QE_APPLICATION_ERROR:](#QE_APPLICATION_ERROR:) (const)
- [QE_CRYPTO_BUFFER_EXCEEDED:](#QE_CRYPTO_BUFFER_EXCEEDED:) (const)
- [QE_KEY_UPDATE_ERROR:](#QE_KEY_UPDATE_ERROR:) (const)
- [QE_AEAD_LIMIT_REACHED:](#QE_AEAD_LIMIT_REACHED:) (const)
- [QE_NO_VIABLE_PATH:](#QE_NO_VIABLE_PATH:) (const)
- [QE_CRYPTO_ERROR:](#QE_CRYPTO_ERROR:) (const)
- [QL_INITIAL:](#QL_INITIAL:) (const)
- [QL_HANDSHAKE:](#QL_HANDSHAKE:) (const)
- [QL_APP:](#QL_APP:) (const)
- [QL_0RTT:](#QL_0RTT:) (const)
- [QuicFrame](#QuicFrame) (struct)
- [QuicFrames](#QuicFrames) (struct)
- [QuicParams](#QuicParams) (struct)
- [QuicParamsParsed](#QuicParamsParsed) (struct)
- [quic_frame_blank](#quic_frame_blank) (function)
- [quic_frame_is_stream](#quic_frame_is_stream) (function)
- [quic_frame_ack_eliciting](#quic_frame_ack_eliciting) (function)
- [quic_frame_allowed](#quic_frame_allowed) (function)
- [quic_frame_put](#quic_frame_put) (function)
- [quic_frame_encode](#quic_frame_encode) (function)
- [quic_stream_frame_overhead](#quic_stream_frame_overhead) (function)
- [quic_crypto_frame_overhead](#quic_crypto_frame_overhead) (function)
- [quic_frames_parse](#quic_frames_parse) (function)
- [quic_ranges_add](#quic_ranges_add) (function)
- [quic_ranges_contain](#quic_ranges_contain) (function)
- [QTP_ORIGINAL_DCID:](#QTP_ORIGINAL_DCID:) (const)
- [QTP_MAX_IDLE_TIMEOUT:](#QTP_MAX_IDLE_TIMEOUT:) (const)
- [QTP_STATELESS_RESET_TOKEN:](#QTP_STATELESS_RESET_TOKEN:) (const)
- [QTP_MAX_UDP_PAYLOAD_SIZE:](#QTP_MAX_UDP_PAYLOAD_SIZE:) (const)
- [QTP_INITIAL_MAX_DATA:](#QTP_INITIAL_MAX_DATA:) (const)
- [QTP_INITIAL_MAX_STREAM_DATA_BIDI_LOCAL:](#QTP_INITIAL_MAX_STREAM_DATA_BIDI_LOCAL:) (const)
- [QTP_INITIAL_MAX_STREAM_DATA_BIDI_REMOTE:](#QTP_INITIAL_MAX_STREAM_DATA_BIDI_REMOTE:) (const)
- [QTP_INITIAL_MAX_STREAM_DATA_UNI:](#QTP_INITIAL_MAX_STREAM_DATA_UNI:) (const)
- [QTP_INITIAL_MAX_STREAMS_BIDI:](#QTP_INITIAL_MAX_STREAMS_BIDI:) (const)
- [QTP_INITIAL_MAX_STREAMS_UNI:](#QTP_INITIAL_MAX_STREAMS_UNI:) (const)
- [QTP_ACK_DELAY_EXPONENT:](#QTP_ACK_DELAY_EXPONENT:) (const)
- [QTP_MAX_ACK_DELAY:](#QTP_MAX_ACK_DELAY:) (const)
- [QTP_DISABLE_ACTIVE_MIGRATION:](#QTP_DISABLE_ACTIVE_MIGRATION:) (const)
- [QTP_PREFERRED_ADDRESS:](#QTP_PREFERRED_ADDRESS:) (const)
- [QTP_ACTIVE_CONNECTION_ID_LIMIT:](#QTP_ACTIVE_CONNECTION_ID_LIMIT:) (const)
- [QTP_INITIAL_SCID:](#QTP_INITIAL_SCID:) (const)
- [QTP_RETRY_SCID:](#QTP_RETRY_SCID:) (const)
- [quic_params_default](#quic_params_default) (function)
- [quic_params_encode](#quic_params_encode) (function)
- [quic_params_parse](#quic_params_parse) (function)

## QF_PADDING:

```fides
pub const QF_PADDING: i64 = 0x00;
```

*Defined at line 25*

---

## QF_PING:

```fides
pub const QF_PING: i64 = 0x01;
```

*Defined at line 26*

---

## QF_ACK:

```fides
pub const QF_ACK: i64 = 0x02;
```

*Defined at line 27*

---

## QF_ACK_ECN:

```fides
pub const QF_ACK_ECN: i64 = 0x03;
```

*Defined at line 28*

---

## QF_RESET_STREAM:

```fides
pub const QF_RESET_STREAM: i64 = 0x04;
```

*Defined at line 29*

---

## QF_STOP_SENDING:

```fides
pub const QF_STOP_SENDING: i64 = 0x05;
```

*Defined at line 30*

---

## QF_CRYPTO:

```fides
pub const QF_CRYPTO: i64 = 0x06;
```

*Defined at line 31*

---

## QF_NEW_TOKEN:

```fides
pub const QF_NEW_TOKEN: i64 = 0x07;
```

*Defined at line 32*

---

## QF_STREAM:

STREAM is 0x08..0x0f: OFF (0x04), LEN (0x02) and FIN (0x01) bits.

```fides
pub const QF_STREAM: i64 = 0x08;
```

*Defined at line 34*

---

## QF_MAX_DATA:

```fides
pub const QF_MAX_DATA: i64 = 0x10;
```

*Defined at line 35*

---

## QF_MAX_STREAM_DATA:

```fides
pub const QF_MAX_STREAM_DATA: i64 = 0x11;
```

*Defined at line 36*

---

## QF_MAX_STREAMS_BIDI:

```fides
pub const QF_MAX_STREAMS_BIDI: i64 = 0x12;
```

*Defined at line 37*

---

## QF_MAX_STREAMS_UNI:

```fides
pub const QF_MAX_STREAMS_UNI: i64 = 0x13;
```

*Defined at line 38*

---

## QF_DATA_BLOCKED:

```fides
pub const QF_DATA_BLOCKED: i64 = 0x14;
```

*Defined at line 39*

---

## QF_STREAM_DATA_BLOCKED:

```fides
pub const QF_STREAM_DATA_BLOCKED: i64 = 0x15;
```

*Defined at line 40*

---

## QF_STREAMS_BLOCKED_BIDI:

```fides
pub const QF_STREAMS_BLOCKED_BIDI: i64 = 0x16;
```

*Defined at line 41*

---

## QF_STREAMS_BLOCKED_UNI:

```fides
pub const QF_STREAMS_BLOCKED_UNI: i64 = 0x17;
```

*Defined at line 42*

---

## QF_NEW_CONNECTION_ID:

```fides
pub const QF_NEW_CONNECTION_ID: i64 = 0x18;
```

*Defined at line 43*

---

## QF_RETIRE_CONNECTION_ID:

```fides
pub const QF_RETIRE_CONNECTION_ID: i64 = 0x19;
```

*Defined at line 44*

---

## QF_PATH_CHALLENGE:

```fides
pub const QF_PATH_CHALLENGE: i64 = 0x1a;
```

*Defined at line 45*

---

## QF_PATH_RESPONSE:

```fides
pub const QF_PATH_RESPONSE: i64 = 0x1b;
```

*Defined at line 46*

---

## QF_CONNECTION_CLOSE:

```fides
pub const QF_CONNECTION_CLOSE: i64 = 0x1c;
```

*Defined at line 47*

---

## QF_CONNECTION_CLOSE_APP:

```fides
pub const QF_CONNECTION_CLOSE_APP: i64 = 0x1d;
```

*Defined at line 48*

---

## QF_HANDSHAKE_DONE:

```fides
pub const QF_HANDSHAKE_DONE: i64 = 0x1e;
```

*Defined at line 49*

---

## QE_NO_ERROR:

```fides
pub const QE_NO_ERROR: i64 = 0x0;
```

*Defined at line 53*

---

## QE_INTERNAL_ERROR:

```fides
pub const QE_INTERNAL_ERROR: i64 = 0x1;
```

*Defined at line 54*

---

## QE_CONNECTION_REFUSED:

```fides
pub const QE_CONNECTION_REFUSED: i64 = 0x2;
```

*Defined at line 55*

---

## QE_FLOW_CONTROL_ERROR:

```fides
pub const QE_FLOW_CONTROL_ERROR: i64 = 0x3;
```

*Defined at line 56*

---

## QE_STREAM_LIMIT_ERROR:

```fides
pub const QE_STREAM_LIMIT_ERROR: i64 = 0x4;
```

*Defined at line 57*

---

## QE_STREAM_STATE_ERROR:

```fides
pub const QE_STREAM_STATE_ERROR: i64 = 0x5;
```

*Defined at line 58*

---

## QE_FINAL_SIZE_ERROR:

```fides
pub const QE_FINAL_SIZE_ERROR: i64 = 0x6;
```

*Defined at line 59*

---

## QE_FRAME_ENCODING_ERROR:

```fides
pub const QE_FRAME_ENCODING_ERROR: i64 = 0x7;
```

*Defined at line 60*

---

## QE_TRANSPORT_PARAMETER_ERROR:

```fides
pub const QE_TRANSPORT_PARAMETER_ERROR: i64 = 0x8;
```

*Defined at line 61*

---

## QE_CONNECTION_ID_LIMIT_ERROR:

```fides
pub const QE_CONNECTION_ID_LIMIT_ERROR: i64 = 0x9;
```

*Defined at line 62*

---

## QE_PROTOCOL_VIOLATION:

```fides
pub const QE_PROTOCOL_VIOLATION: i64 = 0xa;
```

*Defined at line 63*

---

## QE_INVALID_TOKEN:

```fides
pub const QE_INVALID_TOKEN: i64 = 0xb;
```

*Defined at line 64*

---

## QE_APPLICATION_ERROR:

```fides
pub const QE_APPLICATION_ERROR: i64 = 0xc;
```

*Defined at line 65*

---

## QE_CRYPTO_BUFFER_EXCEEDED:

```fides
pub const QE_CRYPTO_BUFFER_EXCEEDED: i64 = 0xd;
```

*Defined at line 66*

---

## QE_KEY_UPDATE_ERROR:

```fides
pub const QE_KEY_UPDATE_ERROR: i64 = 0xe;
```

*Defined at line 67*

---

## QE_AEAD_LIMIT_REACHED:

```fides
pub const QE_AEAD_LIMIT_REACHED: i64 = 0xf;
```

*Defined at line 68*

---

## QE_NO_VIABLE_PATH:

```fides
pub const QE_NO_VIABLE_PATH: i64 = 0x10;
```

*Defined at line 69*

---

## QE_CRYPTO_ERROR:

CRYPTO_ERROR is 0x100 + the TLS alert (RFC 9001 §4.8).

```fides
pub const QE_CRYPTO_ERROR: i64 = 0x100;
```

*Defined at line 71*

---

## QL_INITIAL:

Encryption levels, which are also the packet number spaces (RFC 9000
§12.3) and where each frame may appear (§12.4): Initial, Handshake and
1-RTT ("application"). 0-RTT is not used by this implementation.

```fides
pub const QL_INITIAL: i64 = 0;
```

*Defined at line 76*

---

## QL_HANDSHAKE:

```fides
pub const QL_HANDSHAKE: i64 = 1;
```

*Defined at line 77*

---

## QL_APP:

```fides
pub const QL_APP: i64 = 2;
```

*Defined at line 78*

---

## QL_0RTT:

```fides
pub const QL_0RTT: i64 = 3;
```

*Defined at line 79*

---

## QuicFrame

One frame. Which fields mean something depends on `ftype`:
STREAM:            stream_id, offset, data, fin (ftype is QF_STREAM
whatever bits it was sent with)
CRYPTO:            offset, data
ACK / ACK_ECN:     ranges (pairs low, high, the highest first: the
packet numbers acknowledged), ack_delay (as sent,
before the exponent), ecn (three counts, ACK_ECN)
RESET_STREAM:      stream_id, error_code, final_size
STOP_SENDING:      stream_id, error_code
NEW_TOKEN:         data (the token)
MAX_DATA, DATA_BLOCKED, MAX_STREAMS_*, STREAMS_BLOCKED_*: limit
MAX_STREAM_DATA, STREAM_DATA_BLOCKED: stream_id, limit
NEW_CONNECTION_ID: seq, retire_prior_to, data (the connection ID),
reset_token (16 bytes)
RETIRE_CONNECTION_ID: seq
PATH_CHALLENGE / PATH_RESPONSE: data (8 bytes)
CONNECTION_CLOSE:  error_code, frame_type (0x1c only), data (the
reason phrase)

```fides
pub struct QuicFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `ftype` | `i64` |
| `stream_id` | `i64` |
| `offset` | `i64` |
| `data` | `Vec<i64>` |
| `fin` | `bool` |
| `ranges` | `Vec<i64>` |
| `ack_delay` | `i64` |
| `ecn` | `Vec<i64>` |
| `error_code` | `i64` |
| `final_size` | `i64` |
| `frame_type` | `i64` |
| `limit` | `i64` |
| `seq` | `i64` |
| `retire_prior_to` | `i64` |
| `reset_token` | `Vec<i64>` |

*Defined at line 99*

---

## QuicFrames

The frames of a payload. `error` is -1 when every frame parsed, else
the transport error to close the connection with (FRAME_ENCODING_ERROR,
or PROTOCOL_VIOLATION for a frame the packet type may not carry, RFC
9000 §12.4) and `why` says what was wrong; `frames` then holds the
frames before the bad one.

```fides
pub struct QuicFrames {
```

**Fields:**

| Name | Type |
|------|------|
| `frames` | `Vec<QuicFrame>` |
| `error` | `i64` |
| `why` | `str` |
| `ack_eliciting` | `bool` |

*Defined at line 122*

---

## QuicParams

QUIC transport parameters (RFC 9000 §18.2). `has_*` tell whether an
optional connection ID was present. Defaults are the RFC's.

```fides
pub struct QuicParams {
```

**Fields:**

| Name | Type |
|------|------|
| `has_original_dcid` | `bool` |
| `original_dcid` | `Vec<i64>` |
| `max_idle_timeout` | `i64` |
| `stateless_reset_token` | `Vec<i64>` |
| `max_udp_payload_size` | `i64` |
| `initial_max_data` | `i64` |
| `initial_max_stream_data_bidi_local` | `i64` |
| `initial_max_stream_data_bidi_remote` | `i64` |
| `initial_max_stream_data_uni` | `i64` |
| `initial_max_streams_bidi` | `i64` |
| `initial_max_streams_uni` | `i64` |
| `ack_delay_exponent` | `i64` |
| `max_ack_delay` | `i64` |
| `disable_active_migration` | `bool` |
| `active_connection_id_limit` | `i64` |
| `has_initial_scid` | `bool` |
| `initial_scid` | `Vec<i64>` |
| `has_retry_scid` | `bool` |
| `retry_scid` | `Vec<i64>` |

*Defined at line 131*

---

## QuicParamsParsed

Transport parameters parsed from the peer: `error` -1 when they are
acceptable, else TRANSPORT_PARAMETER_ERROR with `why`.

```fides
pub struct QuicParamsParsed {
```

**Fields:**

| Name | Type |
|------|------|
| `params` | `QuicParams` |
| `error` | `i64` |
| `why` | `str` |

*Defined at line 155*

---

## quic_frame_blank

A frame of type `ftype` with every field zero or empty.

```fides
pub fn quic_frame_blank(ftype: i64) -> QuicFrame {
```

**Parameters:**

- `ftype`: `i64`

**Returns:** `QuicFrame`

*Defined at line 181*

---

## quic_frame_is_stream

Whether a frame type is a STREAM frame (0x08..0x0f).

```fides
pub fn quic_frame_is_stream(ftype: i64) -> bool {
```

**Parameters:**

- `ftype`: `i64`

**Returns:** `bool`

*Defined at line 188*

---

## quic_frame_ack_eliciting

Whether receiving the frame calls for an acknowledgment (RFC 9002 §2):
every frame but ACK, PADDING and CONNECTION_CLOSE.

```fides
pub fn quic_frame_ack_eliciting(ftype: i64) -> bool {
```

**Parameters:**

- `ftype`: `i64`

**Returns:** `bool`

*Defined at line 194*

---

## quic_frame_allowed

Whether a frame type may appear in a packet of encryption level `level`
(RFC 9000 §12.4, Table 3): Initial and Handshake packets carry only
PADDING, PING, ACK, CRYPTO and CONNECTION_CLOSE of type 0x1c; 0-RTT no
ACK, CRYPTO, HANDSHAKE_DONE, NEW_TOKEN, PATH_RESPONSE or
RETIRE_CONNECTION_ID; 1-RTT every frame.

```fides
pub fn quic_frame_allowed(ftype: i64, level: i64) -> bool {
```

**Parameters:**

- `ftype`: `i64`
- `level`: `i64`

**Returns:** `bool`

*Defined at line 203*

---

## quic_frame_put

Append the encoding of `f` to `out` (RFC 9000 §19). A STREAM frame is
always sent with its Length field (type 0x0a/0x0b, 0x0e/0x0f when the
offset is not zero); a frame type this module does not know adds
nothing.

```fides
pub fn quic_frame_put(out: &mut Vec<i64>, f: QuicFrame) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `f`: `QuicFrame`

*Defined at line 219*

---

## quic_frame_encode

The encoding of one frame.

```fides
pub fn quic_frame_encode(f: QuicFrame) -> Vec<i64> {
```

**Parameters:**

- `f`: `QuicFrame`

**Returns:** `Vec<i64>`

*Defined at line 346*

---

## quic_stream_frame_overhead

How many bytes the encoding of a STREAM frame takes, `data_len` bytes
of data at `offset` on `stream_id` (the connection uses it to fill a
packet to its size).

```fides
pub fn quic_stream_frame_overhead(stream_id: i64, offset: i64, data_len: i64) -> i64 {
```

**Parameters:**

- `stream_id`: `i64`
- `offset`: `i64`
- `data_len`: `i64`

**Returns:** `i64`

*Defined at line 355*

---

## quic_crypto_frame_overhead

The same for a CRYPTO frame.

```fides
pub fn quic_crypto_frame_overhead(offset: i64, data_len: i64) -> i64 {
```

**Parameters:**

- `offset`: `i64`
- `data_len`: `i64`

**Returns:** `i64`

*Defined at line 364*

---

## quic_frames_parse

Parse the frames of a decrypted payload received at `level` (QL_*).
An empty payload is a PROTOCOL_VIOLATION (RFC 9000 §12.4).

```fides
pub fn quic_frames_parse(payload: Vec<i64>, level: i64) -> QuicFrames {
```

**Parameters:**

- `payload`: `Vec<i64>`
- `level`: `i64`

**Returns:** `QuicFrames`

*Defined at line 559*

---

## quic_ranges_add

Add packet number `pn` to a set of ranges kept as pairs (low, high),
highest first, merging neighbours; at most `max_ranges` ranges are kept
(the lowest are dropped). Returns the new set.

```fides
pub fn quic_ranges_add(ranges: Vec<i64>, pn: i64, max_ranges: i64) -> Vec<i64> {
```

**Parameters:**

- `ranges`: `Vec<i64>`
- `pn`: `i64`
- `max_ranges`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 602*

---

## quic_ranges_contain

Whether `pn` is in a set of ranges.

```fides
pub fn quic_ranges_contain(ranges: Vec<i64>, pn: i64) -> bool {
```

**Parameters:**

- `ranges`: `Vec<i64>`
- `pn`: `i64`

**Returns:** `bool`

*Defined at line 662*

---

## QTP_ORIGINAL_DCID:

```fides
pub const QTP_ORIGINAL_DCID: i64 = 0x00;
```

*Defined at line 675*

---

## QTP_MAX_IDLE_TIMEOUT:

```fides
pub const QTP_MAX_IDLE_TIMEOUT: i64 = 0x01;
```

*Defined at line 676*

---

## QTP_STATELESS_RESET_TOKEN:

```fides
pub const QTP_STATELESS_RESET_TOKEN: i64 = 0x02;
```

*Defined at line 677*

---

## QTP_MAX_UDP_PAYLOAD_SIZE:

```fides
pub const QTP_MAX_UDP_PAYLOAD_SIZE: i64 = 0x03;
```

*Defined at line 678*

---

## QTP_INITIAL_MAX_DATA:

```fides
pub const QTP_INITIAL_MAX_DATA: i64 = 0x04;
```

*Defined at line 679*

---

## QTP_INITIAL_MAX_STREAM_DATA_BIDI_LOCAL:

```fides
pub const QTP_INITIAL_MAX_STREAM_DATA_BIDI_LOCAL: i64 = 0x05;
```

*Defined at line 680*

---

## QTP_INITIAL_MAX_STREAM_DATA_BIDI_REMOTE:

```fides
pub const QTP_INITIAL_MAX_STREAM_DATA_BIDI_REMOTE: i64 = 0x06;
```

*Defined at line 681*

---

## QTP_INITIAL_MAX_STREAM_DATA_UNI:

```fides
pub const QTP_INITIAL_MAX_STREAM_DATA_UNI: i64 = 0x07;
```

*Defined at line 682*

---

## QTP_INITIAL_MAX_STREAMS_BIDI:

```fides
pub const QTP_INITIAL_MAX_STREAMS_BIDI: i64 = 0x08;
```

*Defined at line 683*

---

## QTP_INITIAL_MAX_STREAMS_UNI:

```fides
pub const QTP_INITIAL_MAX_STREAMS_UNI: i64 = 0x09;
```

*Defined at line 684*

---

## QTP_ACK_DELAY_EXPONENT:

```fides
pub const QTP_ACK_DELAY_EXPONENT: i64 = 0x0a;
```

*Defined at line 685*

---

## QTP_MAX_ACK_DELAY:

```fides
pub const QTP_MAX_ACK_DELAY: i64 = 0x0b;
```

*Defined at line 686*

---

## QTP_DISABLE_ACTIVE_MIGRATION:

```fides
pub const QTP_DISABLE_ACTIVE_MIGRATION: i64 = 0x0c;
```

*Defined at line 687*

---

## QTP_PREFERRED_ADDRESS:

```fides
pub const QTP_PREFERRED_ADDRESS: i64 = 0x0d;
```

*Defined at line 688*

---

## QTP_ACTIVE_CONNECTION_ID_LIMIT:

```fides
pub const QTP_ACTIVE_CONNECTION_ID_LIMIT: i64 = 0x0e;
```

*Defined at line 689*

---

## QTP_INITIAL_SCID:

```fides
pub const QTP_INITIAL_SCID: i64 = 0x0f;
```

*Defined at line 690*

---

## QTP_RETRY_SCID:

```fides
pub const QTP_RETRY_SCID: i64 = 0x10;
```

*Defined at line 691*

---

## quic_params_default

Transport parameters with the RFC's defaults (everything absent: no
data or streams allowed until the peer says so).

```fides
pub fn quic_params_default() -> QuicParams {
```

**Returns:** `QuicParams`

*Defined at line 695*

---

## quic_params_encode

The quic_transport_parameters extension body of `p`. Values equal to
the default are still sent when the RFC's default is not "absent", so
what is sent is always explicit.

```fides
pub fn quic_params_encode(p: QuicParams) -> Vec<i64> {
```

**Parameters:**

- `p`: `QuicParams`

**Returns:** `Vec<i64>`

*Defined at line 717*

---

## quic_params_parse

Parse the peer's transport parameters. `from_server` tells who sent
them: a client may not send the server-only ones (original and retry
connection IDs, the stateless reset token, the preferred address). A
parameter twice, a value out of its range or a length that does not
match is a TRANSPORT_PARAMETER_ERROR; unknown parameters are skipped.

```fides
pub fn quic_params_parse(b: Vec<i64>, from_server: bool) -> QuicParamsParsed {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from_server`: `bool`

**Returns:** `QuicParamsParsed`

*Defined at line 762*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
