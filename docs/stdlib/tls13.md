[All modules](index.md)

# tls13 -- Fides Documentation

## Contents

- [TLS13_HASH_LEN:](#TLS13_HASH_LEN:) (const)
- [Tls13TrafficKeys](#Tls13TrafficKeys) (struct)
- [Tls13Schedule](#Tls13Schedule) (struct)
- [tls13_transcript_hash](#tls13_transcript_hash) (function)
- [tls13_empty_hash](#tls13_empty_hash) (function)
- [tls13_derive_secret](#tls13_derive_secret) (function)
- [tls13_early_secret](#tls13_early_secret) (function)
- [tls13_binder_key](#tls13_binder_key) (function)
- [tls13_handshake_secret](#tls13_handshake_secret) (function)
- [tls13_master_secret](#tls13_master_secret) (function)
- [tls13_traffic_keys](#tls13_traffic_keys) (function)
- [tls13_next_traffic_secret](#tls13_next_traffic_secret) (function)
- [tls13_finished_key](#tls13_finished_key) (function)
- [tls13_finished_verify_data](#tls13_finished_verify_data) (function)
- [tls13_finished_check](#tls13_finished_check) (function)
- [tls13_schedule_handshake](#tls13_schedule_handshake) (function)
- [tls13_schedule_application](#tls13_schedule_application) (function)
- [tls13_schedule_resumption](#tls13_schedule_resumption) (function)

## TLS13_HASH_LEN:

The hash length of the SHA-256 suites: every secret is this long.

```fides
pub const TLS13_HASH_LEN: i64 = 32;
```

*Defined at line 40*

---

## Tls13TrafficKeys

A traffic secret's write key and IV (RFC 8446 §7.3).

```fides
pub struct Tls13TrafficKeys {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `Vec<i64>` |
| `iv` | `Vec<i64>` |

*Defined at line 43*

---

## Tls13Schedule

The key schedule of one connection. A field the schedule has not
reached yet is empty: tls13_schedule_handshake fills `early` to
`master`, tls13_schedule_application the application secrets and the
exporter, tls13_schedule_resumption `resumption`.

```fides
pub struct Tls13Schedule {
```

**Fields:**

| Name | Type |
|------|------|
| `early` | `Vec<i64>` |
| `handshake` | `Vec<i64>` |
| `client_hs` | `Vec<i64>` |
| `server_hs` | `Vec<i64>` |
| `master` | `Vec<i64>` |
| `client_ap` | `Vec<i64>` |
| `server_ap` | `Vec<i64>` |
| `exporter` | `Vec<i64>` |
| `resumption` | `Vec<i64>` |

*Defined at line 52*

---

## tls13_transcript_hash

Transcript-Hash (§4.4.1): SHA-256 of the concatenated handshake
messages (each with its 4-byte header).

```fides
pub fn tls13_transcript_hash(messages: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `messages`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 109*

---

## tls13_empty_hash

The hash of no messages, SHA-256("").

```fides
pub fn tls13_empty_hash() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 114*

---

## tls13_derive_secret

```fides
pub fn tls13_derive_secret(@secret secret: Vec<i64>, label: str, transcript_hash: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret secret`: `Vec<i64>`
- `label`: `str`
- `transcript_hash`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 122*

---

## tls13_early_secret

```fides
pub fn tls13_early_secret(@secret psk: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret psk`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 129*

---

## tls13_binder_key

```fides
pub fn tls13_binder_key(@secret early: Vec<i64>, external: bool) -> Vec<i64> {
```

**Parameters:**

- `@secret early`: `Vec<i64>`
- `external`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 140*

---

## tls13_handshake_secret

```fides
pub fn tls13_handshake_secret(@secret early: Vec<i64>, @secret shared: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret early`: `Vec<i64>`
- `@secret shared`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 150*

---

## tls13_master_secret

```fides
pub fn tls13_master_secret(@secret handshake: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret handshake`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 156*

---

## tls13_traffic_keys

```fides
pub fn tls13_traffic_keys(@secret secret: Vec<i64>, key_len: i64) -> Tls13TrafficKeys {
```

**Parameters:**

- `@secret secret`: `Vec<i64>`
- `key_len`: `i64`

**Returns:** `Tls13TrafficKeys`

*Defined at line 163*

---

## tls13_next_traffic_secret

```fides
pub fn tls13_next_traffic_secret(@secret secret: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret secret`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 172*

---

## tls13_finished_key

```fides
pub fn tls13_finished_key(@secret base_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret base_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 179*

---

## tls13_finished_verify_data

```fides
pub fn tls13_finished_verify_data(@secret base_key: Vec<i64>, transcript_hash: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret base_key`: `Vec<i64>`
- `transcript_hash`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 187*

---

## tls13_finished_check

```fides
pub fn tls13_finished_check(@secret base_key: Vec<i64>, transcript_hash: Vec<i64>, received: Vec<i64>) -> bool {
```

**Parameters:**

- `@secret base_key`: `Vec<i64>`
- `transcript_hash`: `Vec<i64>`
- `received`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 193*

---

## tls13_schedule_handshake

```fides
pub fn tls13_schedule_handshake(@secret psk: Vec<i64>, @secret shared: Vec<i64>, hello_hash: Vec<i64>) -> Tls13Schedule {
```

**Parameters:**

- `@secret psk`: `Vec<i64>`
- `@secret shared`: `Vec<i64>`
- `hello_hash`: `Vec<i64>`

**Returns:** `Tls13Schedule`

*Defined at line 212*

---

## tls13_schedule_application

```fides
pub fn tls13_schedule_application(@secret s: Tls13Schedule, finished_hash: Vec<i64>) -> Tls13Schedule {
```

**Parameters:**

- `@secret s`: `Tls13Schedule`
- `finished_hash`: `Vec<i64>`

**Returns:** `Tls13Schedule`

*Defined at line 224*

---

## tls13_schedule_resumption

```fides
pub fn tls13_schedule_resumption(@secret s: Tls13Schedule, client_finished_hash: Vec<i64>) -> Tls13Schedule {
```

**Parameters:**

- `@secret s`: `Tls13Schedule`
- `client_finished_hash`: `Vec<i64>`

**Returns:** `Tls13Schedule`

*Defined at line 233*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
