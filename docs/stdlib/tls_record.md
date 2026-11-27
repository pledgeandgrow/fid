[All modules](index.md)

# tls_record -- Fides Documentation

## Contents

- [TLS_CT_CHANGE_CIPHER_SPEC:](#TLS_CT_CHANGE_CIPHER_SPEC:) (const)
- [TLS_CT_ALERT:](#TLS_CT_ALERT:) (const)
- [TLS_CT_HANDSHAKE:](#TLS_CT_HANDSHAKE:) (const)
- [TLS_CT_APPLICATION_DATA:](#TLS_CT_APPLICATION_DATA:) (const)
- [TLS_MAX_PLAINTEXT:](#TLS_MAX_PLAINTEXT:) (const)
- [TLS_MAX_CIPHERTEXT:](#TLS_MAX_CIPHERTEXT:) (const)
- [TLS_REC_OK:](#TLS_REC_OK:) (const)
- [TLS_REC_BAD_TYPE:](#TLS_REC_BAD_TYPE:) (const)
- [TLS_REC_BAD_VERSION:](#TLS_REC_BAD_VERSION:) (const)
- [TLS_REC_OVERFLOW:](#TLS_REC_OVERFLOW:) (const)
- [TLS_REC_EMPTY:](#TLS_REC_EMPTY:) (const)
- [TLS_REC_DECODE:](#TLS_REC_DECODE:) (const)
- [TLS_REC_BAD_MAC:](#TLS_REC_BAD_MAC:) (const)
- [TlsRecord](#TlsRecord) (struct)
- [TlsRecords](#TlsRecords) (struct)
- [TlsHandshake](#TlsHandshake) (struct)
- [TlsHandshakes](#TlsHandshakes) (struct)
- [TlsAlert](#TlsAlert) (struct)
- [TlsInner](#TlsInner) (struct)
- [tls_content_type_ok](#tls_content_type_ok) (function)
- [tls_handshake_type_ok](#tls_handshake_type_ok) (function)
- [tls_records_parse](#tls_records_parse) (function)
- [tls_record_encode](#tls_record_encode) (function)
- [tls_handshake_parse](#tls_handshake_parse) (function)
- [tls_handshake_encode](#tls_handshake_encode) (function)
- [tls_alert_parse](#tls_alert_parse) (function)
- [tls_inner_parse](#tls_inner_parse) (function)
- [tls_record_nonce](#tls_record_nonce) (function)
- [tls_record_seal](#tls_record_seal) (function)
- [tls_record_open](#tls_record_open) (function)
- [tls_rec_error_name](#tls_rec_error_name) (function)

## TLS_CT_CHANGE_CIPHER_SPEC:

```fides
pub const TLS_CT_CHANGE_CIPHER_SPEC: i64 = 20;
```

*Defined at line 26*

---

## TLS_CT_ALERT:

```fides
pub const TLS_CT_ALERT: i64 = 21;
```

*Defined at line 27*

---

## TLS_CT_HANDSHAKE:

```fides
pub const TLS_CT_HANDSHAKE: i64 = 22;
```

*Defined at line 28*

---

## TLS_CT_APPLICATION_DATA:

```fides
pub const TLS_CT_APPLICATION_DATA: i64 = 23;
```

*Defined at line 29*

---

## TLS_MAX_PLAINTEXT:

The largest TLSPlaintext fragment (2^14) and TLSCiphertext (2^14 + 256).

```fides
pub const TLS_MAX_PLAINTEXT: i64 = 16384;
```

*Defined at line 32*

---

## TLS_MAX_CIPHERTEXT:

```fides
pub const TLS_MAX_CIPHERTEXT: i64 = 16640;
```

*Defined at line 33*

---

## TLS_REC_OK:

Error codes; each names the alert RFC 8446 §6.2 says to send.

```fides
pub const TLS_REC_OK: i64 = 0;
```

*Defined at line 36*

---

## TLS_REC_BAD_TYPE:

unexpected_message: an unknown content or handshake type

```fides
pub const TLS_REC_BAD_TYPE: i64 = 1;
```

*Defined at line 38*

---

## TLS_REC_BAD_VERSION:

protocol_version / decode_error: legacy_record_version is not 0x03XX

```fides
pub const TLS_REC_BAD_VERSION: i64 = 2;
```

*Defined at line 40*

---

## TLS_REC_OVERFLOW:

record_overflow: a length over the limit

```fides
pub const TLS_REC_OVERFLOW: i64 = 3;
```

*Defined at line 42*

---

## TLS_REC_EMPTY:

unexpected_message: a zero-length handshake or alert fragment, or an
inner plaintext with no content type

```fides
pub const TLS_REC_EMPTY: i64 = 4;
```

*Defined at line 45*

---

## TLS_REC_DECODE:

decode_error: a message shorter than its fixed part

```fides
pub const TLS_REC_DECODE: i64 = 5;
```

*Defined at line 47*

---

## TLS_REC_BAD_MAC:

bad_record_mac: the record does not authenticate

```fides
pub const TLS_REC_BAD_MAC: i64 = 6;
```

*Defined at line 49*

---

## TlsRecord

One record: its content type, legacy_record_version, fragment bytes.

```fides
pub struct TlsRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `ctype` | `i64` |
| `version` | `i64` |
| `fragment` | `Vec<i64>` |

*Defined at line 52*

---

## TlsRecords

The whole records at the front of a stream. `consumed` bytes were used:
the rest is an incomplete record (read more, then parse from there), or
where `error` (not TLS_REC_OK) stopped the parse.

```fides
pub struct TlsRecords {
```

**Fields:**

| Name | Type |
|------|------|
| `records` | `Vec<TlsRecord>` |
| `consumed` | `i64` |
| `error` | `i64` |

*Defined at line 61*

---

## TlsHandshake

One handshake message (RFC 8446 §4): msg_type and its body.

```fides
pub struct TlsHandshake {
```

**Fields:**

| Name | Type |
|------|------|
| `msg_type` | `i64` |
| `body` | `Vec<i64>` |

*Defined at line 68*

---

## TlsHandshakes

```fides
pub struct TlsHandshakes {
```

**Fields:**

| Name | Type |
|------|------|
| `messages` | `Vec<TlsHandshake>` |
| `consumed` | `i64` |
| `error` | `i64` |

*Defined at line 73*

---

## TlsAlert

An alert: level (1 warning, 2 fatal) and description, or an error.

```fides
pub struct TlsAlert {
```

**Fields:**

| Name | Type |
|------|------|
| `level` | `i64` |
| `description` | `i64` |
| `error` | `i64` |

*Defined at line 80*

---

## TlsInner

What a protected record holds: its real content type and content.

```fides
pub struct TlsInner {
```

**Fields:**

| Name | Type |
|------|------|
| `ctype` | `i64` |
| `content` | `Vec<i64>` |
| `error` | `i64` |

*Defined at line 87*

---

## tls_content_type_ok

A known content type (RFC 8446 §5.1).

```fides
pub fn tls_content_type_ok(t: i64) -> bool {
```

**Parameters:**

- `t`: `i64`

**Returns:** `bool`

*Defined at line 116*

---

## tls_handshake_type_ok

A handshake type TLS 1.3 sends (RFC 8446 §4).

```fides
pub fn tls_handshake_type_ok(t: i64) -> bool {
```

**Parameters:**

- `t`: `i64`

**Returns:** `bool`

*Defined at line 121*

---

## tls_records_parse

Split the records at the front of `data` (bytes 0..255).

```fides
pub fn tls_records_parse(data: Vec<i64>) -> TlsRecords {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `TlsRecords`

*Defined at line 126*

---

## tls_record_encode

The bytes of one plaintext record: header, then the fragment.

```fides
pub fn tls_record_encode(ctype: i64, fragment: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `ctype`: `i64`
- `fragment`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 165*

---

## tls_handshake_parse

Split the handshake messages at the front of `data` (the fragments of
handshake records joined: a message may span records).

```fides
pub fn tls_handshake_parse(data: Vec<i64>) -> TlsHandshakes {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `TlsHandshakes`

*Defined at line 178*

---

## tls_handshake_encode

The bytes of one handshake message.

```fides
pub fn tls_handshake_encode(msg_type: i64, body: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg_type`: `i64`
- `body`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 205*

---

## tls_alert_parse

An alert fragment: exactly two bytes, level 1 or 2.

```fides
pub fn tls_alert_parse(fragment: Vec<i64>) -> TlsAlert {
```

**Parameters:**

- `fragment`: `Vec<i64>`

**Returns:** `TlsAlert`

*Defined at line 217*

---

## tls_inner_parse

TLSInnerPlaintext (RFC 8446 §5.2): content, the content type, then zero
padding. The type is the last byte that is not zero.

```fides
pub fn tls_inner_parse(plain: Vec<i64>) -> TlsInner {
```

**Parameters:**

- `plain`: `Vec<i64>`

**Returns:** `TlsInner`

*Defined at line 236*

---

## tls_record_nonce

The per-record nonce (RFC 8446 §5.3): the 12-byte write IV XOR the
64-bit sequence number, big-endian, at its right end.

```fides
pub fn tls_record_nonce(iv: Vec<i64>, seq: i64) -> Vec<i64> {
```

**Parameters:**

- `iv`: `Vec<i64>`
- `seq`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 265*

---

## tls_record_seal

```fides
pub fn tls_record_seal(@secret key: Vec<i64>, iv: Vec<i64>, seq: i64, ctype: i64, content: Vec<i64>, pad: i64) -> Vec<i64> {
```

**Parameters:**

- `@secret key`: `Vec<i64>`
- `iv`: `Vec<i64>`
- `seq`: `i64`
- `ctype`: `i64`
- `content`: `Vec<i64>`
- `pad`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 286*

---

## tls_record_open

```fides
pub fn tls_record_open(@secret key: Vec<i64>, iv: Vec<i64>, seq: i64, rec: TlsRecord) -> TlsInner {
```

**Parameters:**

- `@secret key`: `Vec<i64>`
- `iv`: `Vec<i64>`
- `seq`: `i64`
- `rec`: `TlsRecord`

**Returns:** `TlsInner`

*Defined at line 310*

---

## tls_rec_error_name

The alert name of an error code (for logs).

```fides
pub fn tls_rec_error_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 345*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
