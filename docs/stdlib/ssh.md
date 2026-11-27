[All modules](index.md)

# ssh -- Fides Documentation

## Contents

- [SSH_MSG_DISCONNECT:](#SSH_MSG_DISCONNECT:) (const)
- [SSH_MSG_IGNORE:](#SSH_MSG_IGNORE:) (const)
- [SSH_MSG_UNIMPLEMENTED:](#SSH_MSG_UNIMPLEMENTED:) (const)
- [SSH_MSG_DEBUG:](#SSH_MSG_DEBUG:) (const)
- [SSH_MSG_SERVICE_REQUEST:](#SSH_MSG_SERVICE_REQUEST:) (const)
- [SSH_MSG_SERVICE_ACCEPT:](#SSH_MSG_SERVICE_ACCEPT:) (const)
- [SSH_MSG_KEXINIT:](#SSH_MSG_KEXINIT:) (const)
- [SSH_MSG_NEWKEYS:](#SSH_MSG_NEWKEYS:) (const)
- [SSH_MSG_KEX_ECDH_INIT:](#SSH_MSG_KEX_ECDH_INIT:) (const)
- [SSH_MSG_KEX_ECDH_REPLY:](#SSH_MSG_KEX_ECDH_REPLY:) (const)
- [SSH_MSG_USERAUTH_REQUEST:](#SSH_MSG_USERAUTH_REQUEST:) (const)
- [SSH_MSG_USERAUTH_FAILURE:](#SSH_MSG_USERAUTH_FAILURE:) (const)
- [SSH_MSG_USERAUTH_SUCCESS:](#SSH_MSG_USERAUTH_SUCCESS:) (const)
- [SSH_MSG_USERAUTH_BANNER:](#SSH_MSG_USERAUTH_BANNER:) (const)
- [SSH_MSG_USERAUTH_PK_OK:](#SSH_MSG_USERAUTH_PK_OK:) (const)
- [SSH_MSG_GLOBAL_REQUEST:](#SSH_MSG_GLOBAL_REQUEST:) (const)
- [SSH_MSG_CHANNEL_OPEN:](#SSH_MSG_CHANNEL_OPEN:) (const)
- [SSH_MSG_CHANNEL_OPEN_CONFIRMATION:](#SSH_MSG_CHANNEL_OPEN_CONFIRMATION:) (const)
- [SSH_MSG_CHANNEL_OPEN_FAILURE:](#SSH_MSG_CHANNEL_OPEN_FAILURE:) (const)
- [SSH_MSG_CHANNEL_WINDOW_ADJUST:](#SSH_MSG_CHANNEL_WINDOW_ADJUST:) (const)
- [SSH_MSG_CHANNEL_DATA:](#SSH_MSG_CHANNEL_DATA:) (const)
- [SSH_MSG_CHANNEL_EXTENDED_DATA:](#SSH_MSG_CHANNEL_EXTENDED_DATA:) (const)
- [SSH_MSG_CHANNEL_EOF:](#SSH_MSG_CHANNEL_EOF:) (const)
- [SSH_MSG_CHANNEL_CLOSE:](#SSH_MSG_CHANNEL_CLOSE:) (const)
- [SSH_MSG_CHANNEL_REQUEST:](#SSH_MSG_CHANNEL_REQUEST:) (const)
- [SSH_MSG_CHANNEL_SUCCESS:](#SSH_MSG_CHANNEL_SUCCESS:) (const)
- [SSH_MSG_CHANNEL_FAILURE:](#SSH_MSG_CHANNEL_FAILURE:) (const)
- [SSH_DISCONNECT_PROTOCOL_ERROR:](#SSH_DISCONNECT_PROTOCOL_ERROR:) (const)
- [SSH_DISCONNECT_KEY_EXCHANGE_FAILED:](#SSH_DISCONNECT_KEY_EXCHANGE_FAILED:) (const)
- [SSH_DISCONNECT_MAC_ERROR:](#SSH_DISCONNECT_MAC_ERROR:) (const)
- [SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE:](#SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE:) (const)
- [SSH_DISCONNECT_BY_APPLICATION:](#SSH_DISCONNECT_BY_APPLICATION:) (const)
- [SSH_MAX_PACKET:](#SSH_MAX_PACKET:) (const)
- [SSH_ST_VERSION:](#SSH_ST_VERSION:) (const)
- [SSH_ST_KEX:](#SSH_ST_KEX:) (const)
- [SSH_ST_READY:](#SSH_ST_READY:) (const)
- [SSH_ST_CLOSED:](#SSH_ST_CLOSED:) (const)
- [SshReader](#SshReader) (struct)
- [SshMessage](#SshMessage) (struct)
- [SshAlgs](#SshAlgs) (struct)
- [SshSession](#SshSession) (struct)
- [ssh_put_byte](#ssh_put_byte) (function)
- [ssh_put_bool](#ssh_put_bool) (function)
- [ssh_put_u32](#ssh_put_u32) (function)
- [ssh_put_string](#ssh_put_string) (function)
- [ssh_put_text](#ssh_put_text) (function)
- [ssh_mpint](#ssh_mpint) (function)
- [ssh_reader](#ssh_reader) (function)
- [ssh_get_byte](#ssh_get_byte) (function)
- [ssh_get_bool](#ssh_get_bool) (function)
- [ssh_get_u32](#ssh_get_u32) (function)
- [ssh_get_string](#ssh_get_string) (function)
- [ssh_get_text](#ssh_get_text) (function)
- [ssh_get_namelist](#ssh_get_namelist) (function)
- [ssh_reader_done](#ssh_reader_done) (function)
- [ssh_ed25519_blob](#ssh_ed25519_blob) (function)
- [ssh_ed25519_blob_key](#ssh_ed25519_blob_key) (function)
- [ssh_ed25519_sign](#ssh_ed25519_sign) (function)
- [ssh_ed25519_verify](#ssh_ed25519_verify) (function)
- [ssh_fingerprint](#ssh_fingerprint) (function)
- [ssh_client_new](#ssh_client_new) (function)
- [ssh_server_new](#ssh_server_new) (function)
- [ssh_set_kex_seed](#ssh_set_kex_seed) (function)
- [ssh_ready](#ssh_ready) (function)
- [ssh_send](#ssh_send) (function)
- [ssh_version_line](#ssh_version_line) (function)
- [ssh_choose](#ssh_choose) (function)
- [ssh_negotiate](#ssh_negotiate) (function)
- [ssh_start](#ssh_start) (function)
- [ssh_rekey](#ssh_rekey) (function)
- [ssh_exchange_hash](#ssh_exchange_hash) (function)
- [ssh_derive_key](#ssh_derive_key) (function)
- [ssh_feed](#ssh_feed) (function)
- [ssh_next_message](#ssh_next_message) (function)
- [ssh_disconnect](#ssh_disconnect) (function)
- [ssh_service_request](#ssh_service_request) (function)
- [ssh_service_accept](#ssh_service_accept) (function)
- [ssh_parse_service](#ssh_parse_service) (function)
- [SshAuthRequest](#SshAuthRequest) (struct)
- [ssh_userauth_signed_data](#ssh_userauth_signed_data) (function)
- [ssh_userauth_publickey_query](#ssh_userauth_publickey_query) (function)
- [ssh_userauth_publickey](#ssh_userauth_publickey) (function)
- [ssh_userauth_password](#ssh_userauth_password) (function)
- [ssh_parse_userauth](#ssh_parse_userauth) (function)
- [ssh_userauth_check_signature](#ssh_userauth_check_signature) (function)
- [ssh_userauth_failure](#ssh_userauth_failure) (function)
- [ssh_userauth_success](#ssh_userauth_success) (function)
- [ssh_userauth_pk_ok](#ssh_userauth_pk_ok) (function)
- [ssh_userauth_banner](#ssh_userauth_banner) (function)
- [ssh_parse_userauth_failure](#ssh_parse_userauth_failure) (function)
- [SshChannelMsg](#SshChannelMsg) (struct)
- [ssh_channel_open](#ssh_channel_open) (function)
- [ssh_channel_open_confirm](#ssh_channel_open_confirm) (function)
- [ssh_channel_open_failure](#ssh_channel_open_failure) (function)
- [ssh_channel_window_adjust](#ssh_channel_window_adjust) (function)
- [ssh_channel_data](#ssh_channel_data) (function)
- [ssh_channel_extended_data](#ssh_channel_extended_data) (function)
- [ssh_channel_eof](#ssh_channel_eof) (function)
- [ssh_channel_close](#ssh_channel_close) (function)
- [ssh_channel_success](#ssh_channel_success) (function)
- [ssh_channel_failure](#ssh_channel_failure) (function)
- [ssh_channel_request](#ssh_channel_request) (function)
- [ssh_parse_channel](#ssh_parse_channel) (function)

## SSH_MSG_DISCONNECT:

```fides
pub const SSH_MSG_DISCONNECT: i64 = 1;
```

*Defined at line 55*

---

## SSH_MSG_IGNORE:

```fides
pub const SSH_MSG_IGNORE: i64 = 2;
```

*Defined at line 56*

---

## SSH_MSG_UNIMPLEMENTED:

```fides
pub const SSH_MSG_UNIMPLEMENTED: i64 = 3;
```

*Defined at line 57*

---

## SSH_MSG_DEBUG:

```fides
pub const SSH_MSG_DEBUG: i64 = 4;
```

*Defined at line 58*

---

## SSH_MSG_SERVICE_REQUEST:

```fides
pub const SSH_MSG_SERVICE_REQUEST: i64 = 5;
```

*Defined at line 59*

---

## SSH_MSG_SERVICE_ACCEPT:

```fides
pub const SSH_MSG_SERVICE_ACCEPT: i64 = 6;
```

*Defined at line 60*

---

## SSH_MSG_KEXINIT:

```fides
pub const SSH_MSG_KEXINIT: i64 = 20;
```

*Defined at line 61*

---

## SSH_MSG_NEWKEYS:

```fides
pub const SSH_MSG_NEWKEYS: i64 = 21;
```

*Defined at line 62*

---

## SSH_MSG_KEX_ECDH_INIT:

```fides
pub const SSH_MSG_KEX_ECDH_INIT: i64 = 30;
```

*Defined at line 63*

---

## SSH_MSG_KEX_ECDH_REPLY:

```fides
pub const SSH_MSG_KEX_ECDH_REPLY: i64 = 31;
```

*Defined at line 64*

---

## SSH_MSG_USERAUTH_REQUEST:

```fides
pub const SSH_MSG_USERAUTH_REQUEST: i64 = 50;
```

*Defined at line 65*

---

## SSH_MSG_USERAUTH_FAILURE:

```fides
pub const SSH_MSG_USERAUTH_FAILURE: i64 = 51;
```

*Defined at line 66*

---

## SSH_MSG_USERAUTH_SUCCESS:

```fides
pub const SSH_MSG_USERAUTH_SUCCESS: i64 = 52;
```

*Defined at line 67*

---

## SSH_MSG_USERAUTH_BANNER:

```fides
pub const SSH_MSG_USERAUTH_BANNER: i64 = 53;
```

*Defined at line 68*

---

## SSH_MSG_USERAUTH_PK_OK:

```fides
pub const SSH_MSG_USERAUTH_PK_OK: i64 = 60;
```

*Defined at line 69*

---

## SSH_MSG_GLOBAL_REQUEST:

```fides
pub const SSH_MSG_GLOBAL_REQUEST: i64 = 80;
```

*Defined at line 70*

---

## SSH_MSG_CHANNEL_OPEN:

```fides
pub const SSH_MSG_CHANNEL_OPEN: i64 = 90;
```

*Defined at line 71*

---

## SSH_MSG_CHANNEL_OPEN_CONFIRMATION:

```fides
pub const SSH_MSG_CHANNEL_OPEN_CONFIRMATION: i64 = 91;
```

*Defined at line 72*

---

## SSH_MSG_CHANNEL_OPEN_FAILURE:

```fides
pub const SSH_MSG_CHANNEL_OPEN_FAILURE: i64 = 92;
```

*Defined at line 73*

---

## SSH_MSG_CHANNEL_WINDOW_ADJUST:

```fides
pub const SSH_MSG_CHANNEL_WINDOW_ADJUST: i64 = 93;
```

*Defined at line 74*

---

## SSH_MSG_CHANNEL_DATA:

```fides
pub const SSH_MSG_CHANNEL_DATA: i64 = 94;
```

*Defined at line 75*

---

## SSH_MSG_CHANNEL_EXTENDED_DATA:

```fides
pub const SSH_MSG_CHANNEL_EXTENDED_DATA: i64 = 95;
```

*Defined at line 76*

---

## SSH_MSG_CHANNEL_EOF:

```fides
pub const SSH_MSG_CHANNEL_EOF: i64 = 96;
```

*Defined at line 77*

---

## SSH_MSG_CHANNEL_CLOSE:

```fides
pub const SSH_MSG_CHANNEL_CLOSE: i64 = 97;
```

*Defined at line 78*

---

## SSH_MSG_CHANNEL_REQUEST:

```fides
pub const SSH_MSG_CHANNEL_REQUEST: i64 = 98;
```

*Defined at line 79*

---

## SSH_MSG_CHANNEL_SUCCESS:

```fides
pub const SSH_MSG_CHANNEL_SUCCESS: i64 = 99;
```

*Defined at line 80*

---

## SSH_MSG_CHANNEL_FAILURE:

```fides
pub const SSH_MSG_CHANNEL_FAILURE: i64 = 100;
```

*Defined at line 81*

---

## SSH_DISCONNECT_PROTOCOL_ERROR:

```fides
pub const SSH_DISCONNECT_PROTOCOL_ERROR: i64 = 2;
```

*Defined at line 83*

---

## SSH_DISCONNECT_KEY_EXCHANGE_FAILED:

```fides
pub const SSH_DISCONNECT_KEY_EXCHANGE_FAILED: i64 = 3;
```

*Defined at line 84*

---

## SSH_DISCONNECT_MAC_ERROR:

```fides
pub const SSH_DISCONNECT_MAC_ERROR: i64 = 5;
```

*Defined at line 85*

---

## SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE:

```fides
pub const SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE: i64 = 9;
```

*Defined at line 86*

---

## SSH_DISCONNECT_BY_APPLICATION:

```fides
pub const SSH_DISCONNECT_BY_APPLICATION: i64 = 11;
```

*Defined at line 87*

---

## SSH_MAX_PACKET:

Largest packet accepted (RFC 4253 §6.1 asks for at least 35000).

```fides
pub const SSH_MAX_PACKET: i64 = 262144;
```

*Defined at line 90*

---

## SSH_ST_VERSION:

```fides
pub const SSH_ST_VERSION: i64 = 0;
```

*Defined at line 93*

---

## SSH_ST_KEX:

```fides
pub const SSH_ST_KEX: i64 = 1;
```

*Defined at line 94*

---

## SSH_ST_READY:

```fides
pub const SSH_ST_READY: i64 = 2;
```

*Defined at line 95*

---

## SSH_ST_CLOSED:

```fides
pub const SSH_ST_CLOSED: i64 = 3;
```

*Defined at line 96*

---

## SshReader

A reader over a payload; `ok` turns false on any short read.

```fides
pub struct SshReader {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |

*Defined at line 99*

---

## SshMessage

One application message from ssh_next_message.

```fides
pub struct SshMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `msg` | `i64` |
| `payload` | `Vec<i64>` |

*Defined at line 106*

---

## SshAlgs

What negotiation chose.

```fides
pub struct SshAlgs {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kex` | `str` |
| `host_key` | `str` |
| `enc_cs` | `str` |
| `enc_sc` | `str` |
| `comp_cs` | `str` |
| `comp_sc` | `str` |
| `strict` | `bool` |

*Defined at line 113*

---

## SshSession

One side of an SSH connection.

```fides
pub struct SshSession {
```

**Fields:**

| Name | Type |
|------|------|
| `server` | `bool` |
| `state` | `i64` |
| `err` | `str` |
| `ident` | `str` |
| `peer_ident` | `str` |
| `inbuf` | `Vec<i64>` |
| `out_key` | `Vec<i64>` |
| `in_key` | `Vec<i64>` |
| `out_seq` | `i64` |
| `in_seq` | `i64` |
| `pending_out_key` | `Vec<i64>` |
| `pending_in_key` | `Vec<i64>` |
| `packets_in` | `i64` |
| `my_kexinit` | `Vec<i64>` |
| `peer_kexinit` | `Vec<i64>` |
| `algs` | `SshAlgs` |
| `e_secret` | `Vec<i64>` |
| `e_pub` | `Vec<i64>` |
| `host_seed` | `Vec<i64>` |
| `host_key` | `Vec<i64>` |
| `trusted_host_key` | `Vec<i64>` |
| `exchange_hash` | `Vec<i64>` |
| `session_id` | `Vec<i64>` |
| `sent_newkeys` | `bool` |
| `got_newkeys` | `bool` |
| `kex_count` | `i64` |
| `seed` | `Vec<i64>` |
| `seed_ctr` | `i64` |
| `queue` | `Vec<i64>` |

*Defined at line 125*

---

## ssh_put_byte

Append a byte.

```fides
pub fn ssh_put_byte(b: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 183*

---

## ssh_put_bool

Append a boolean.

```fides
pub fn ssh_put_bool(b: &mut Vec<i64>, v: bool) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `v`: `bool`

*Defined at line 188*

---

## ssh_put_u32

Append a uint32 (big-endian).

```fides
pub fn ssh_put_u32(b: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 197*

---

## ssh_put_string

Append a string (uint32 length || bytes).

```fides
pub fn ssh_put_string(b: &mut Vec<i64>, s: Vec<i64>) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `s`: `Vec<i64>`

*Defined at line 205*

---

## ssh_put_text

Append text as a string.

```fides
pub fn ssh_put_text(b: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 215*

---

## ssh_mpint

The mpint encoding of an unsigned big-endian magnitude.

```fides
pub fn ssh_mpint(mag: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `mag`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 220*

---

## ssh_reader

A reader at the start of `data`.

```fides
pub fn ssh_reader(data: Vec<i64>) -> SshReader {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `SshReader`

*Defined at line 236*

---

## ssh_get_byte

Read a byte (0 and ok false at the end).

```fides
pub fn ssh_get_byte(r: &mut SshReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `i64`

*Defined at line 243*

---

## ssh_get_bool

Read a boolean (any non-zero byte is true).

```fides
pub fn ssh_get_bool(r: &mut SshReader) -> bool {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `bool`

*Defined at line 254*

---

## ssh_get_u32

Read a uint32.

```fides
pub fn ssh_get_u32(r: &mut SshReader) -> i64 {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `i64`

*Defined at line 259*

---

## ssh_get_string

Read a string's bytes.

```fides
pub fn ssh_get_string(r: &mut SshReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `Vec<i64>`

*Defined at line 270*

---

## ssh_get_text

Read a string as text.

```fides
pub fn ssh_get_text(r: &mut SshReader) -> str {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `str`

*Defined at line 282*

---

## ssh_get_namelist

Read a name-list.

```fides
pub fn ssh_get_namelist(r: &mut SshReader) -> Vec<str> {
```

**Parameters:**

- `r`: `&mut SshReader`

**Returns:** `Vec<str>`

*Defined at line 287*

---

## ssh_reader_done

True when every byte has been read and nothing was short.

```fides
pub fn ssh_reader_done(r: &SshReader) -> bool {
```

**Parameters:**

- `r`: `&SshReader`

**Returns:** `bool`

*Defined at line 296*

---

## ssh_ed25519_blob

The ssh-ed25519 public key blob: string "ssh-ed25519" || string key.

```fides
pub fn ssh_ed25519_blob(public_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `public_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 303*

---

## ssh_ed25519_blob_key

The 32-byte key inside an ssh-ed25519 blob, empty if it is not one.

```fides
pub fn ssh_ed25519_blob_key(blob: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `blob`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 311*

---

## ssh_ed25519_sign

An ssh-ed25519 signature blob over `data` with a 32-byte seed.

```fides
pub fn ssh_ed25519_sign(seed: Vec<i64>, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `seed`: `Vec<i64>`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 322*

---

## ssh_ed25519_verify

Check an ssh-ed25519 signature blob against a public key blob.

```fides
pub fn ssh_ed25519_verify(key_blob: Vec<i64>, data: Vec<i64>, sig_blob: Vec<i64>) -> bool {
```

**Parameters:**

- `key_blob`: `Vec<i64>`
- `data`: `Vec<i64>`
- `sig_blob`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 330*

---

## ssh_fingerprint

OpenSSH's fingerprint "SHA256:" || base64(SHA-256(blob)) without padding.

```fides
pub fn ssh_fingerprint(blob: Vec<i64>) -> str {
```

**Parameters:**

- `blob`: `Vec<i64>`

**Returns:** `str`

*Defined at line 345*

---

## ssh_client_new

A client with identification string `ident` ("SSH-2.0-...").

```fides
pub fn ssh_client_new(ident: str) -> SshSession {
```

**Parameters:**

- `ident`: `str`

**Returns:** `SshSession`

*Defined at line 368*

---

## ssh_server_new

A server with identification `ident` and an Ed25519 host key seed.

```fides
pub fn ssh_server_new(ident: str, host_seed: Vec<i64>) -> SshSession {
```

**Parameters:**

- `ident`: `str`
- `host_seed`: `Vec<i64>`

**Returns:** `SshSession`

*Defined at line 373*

---

## ssh_set_kex_seed

Seam: derive cookies, ephemeral keys and padding from `seed`
(SHA-256 in counter mode) instead of crypto_random_bytes, and pad with
zeros. For tests and reproducible vectors only.

```fides
pub fn ssh_set_kex_seed(s: &mut SshSession, seed: Vec<i64>) {
```

**Parameters:**

- `s`: `&mut SshSession`
- `seed`: `Vec<i64>`

*Defined at line 380*

---

## ssh_ready

True once the first key exchange is done in both directions.

```fides
pub fn ssh_ready(s: &SshSession) -> bool {
```

**Parameters:**

- `s`: `&SshSession`

**Returns:** `bool`

*Defined at line 405*

---

## ssh_send

The packet bytes carrying one application payload.

```fides
pub fn ssh_send(s: &mut SshSession, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut SshSession`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 463*

---

## ssh_version_line

Our identification line with CR LF.

```fides
pub fn ssh_version_line(s: &SshSession) -> Vec<i64> {
```

**Parameters:**

- `s`: `&SshSession`

**Returns:** `Vec<i64>`

*Defined at line 547*

---

## ssh_choose

The first client algorithm the server also supports ("" if none),
skipping the strict-KEX markers.

```fides
pub fn ssh_choose(client: Vec<str>, server: Vec<str>) -> str {
```

**Parameters:**

- `client`: `Vec<str>`
- `server`: `Vec<str>`

**Returns:** `str`

*Defined at line 637*

---

## ssh_negotiate

Negotiate from two KEXINIT payloads (client's first).

```fides
pub fn ssh_negotiate(ic: Vec<i64>, is: Vec<i64>, first_kex: bool) -> SshAlgs {
```

**Parameters:**

- `ic`: `Vec<i64>`
- `is`: `Vec<i64>`
- `first_kex`: `bool`

**Returns:** `SshAlgs`

*Defined at line 650*

---

## ssh_start

The first bytes to send: our identification line and our KEXINIT.

```fides
pub fn ssh_start(s: &mut SshSession) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut SshSession`

**Returns:** `Vec<i64>`

*Defined at line 692*

---

## ssh_rekey

Begin a re-key on an established session (either side may).

```fides
pub fn ssh_rekey(s: &mut SshSession) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut SshSession`

**Returns:** `Vec<i64>`

*Defined at line 697*

---

## ssh_exchange_hash

The exchange hash H = SHA-256(V_C || V_S || I_C || I_S || K_S || Q_C
|| Q_S || K), each a string except K, an mpint.

```fides
pub fn ssh_exchange_hash(v_c: str, v_s: str, i_c: Vec<i64>, i_s: Vec<i64>, k_s: Vec<i64>, q_c: Vec<i64>, q_s: Vec<i64>, k_mpint: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v_c`: `str`
- `v_s`: `str`
- `i_c`: `Vec<i64>`
- `i_s`: `Vec<i64>`
- `k_s`: `Vec<i64>`
- `q_c`: `Vec<i64>`
- `q_s`: `Vec<i64>`
- `k_mpint`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 715*

---

## ssh_derive_key

Key derivation (RFC 4253 §7.2): HASH(K || H || letter || session_id),
extended with HASH(K || H || K1 || ...) to `len` bytes.

```fides
pub fn ssh_derive_key(k_mpint: Vec<i64>, h: Vec<i64>, letter: i64, session_id: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `k_mpint`: `Vec<i64>`
- `h`: `Vec<i64>`
- `letter`: `i64`
- `session_id`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 729*

---

## ssh_feed

Feed bytes received from the peer. The transport messages are handled
here; the bytes returned must be sent to the peer. Application
messages wait in the queue (ssh_next_message).

```fides
pub fn ssh_feed(s: &mut SshSession, bytes: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut SshSession`
- `bytes`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 912*

---

## ssh_next_message

The next queued application message, ok false when none.

```fides
pub fn ssh_next_message(s: &mut SshSession) -> SshMessage {
```

**Parameters:**

- `s`: `&mut SshSession`

**Returns:** `SshMessage`

*Defined at line 959*

---

## ssh_disconnect

A DISCONNECT payload.

```fides
pub fn ssh_disconnect(reason: i64, text: str) -> Vec<i64> {
```

**Parameters:**

- `reason`: `i64`
- `text`: `str`

**Returns:** `Vec<i64>`

*Defined at line 974*

---

## ssh_service_request

SERVICE_REQUEST for a service ("ssh-userauth").

```fides
pub fn ssh_service_request(name: str) -> Vec<i64> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 986*

---

## ssh_service_accept

SERVICE_ACCEPT.

```fides
pub fn ssh_service_accept(name: str) -> Vec<i64> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 994*

---

## ssh_parse_service

The service named in a SERVICE_REQUEST or SERVICE_ACCEPT ("" if malformed).

```fides
pub fn ssh_parse_service(payload: Vec<i64>) -> str {
```

**Parameters:**

- `payload`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1002*

---

## SshAuthRequest

A parsed USERAUTH_REQUEST.

```fides
pub struct SshAuthRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `user` | `str` |
| `service` | `str` |
| `method` | `str` |
| `has_sig` | `bool` |
| `alg` | `str` |
| `key_blob` | `Vec<i64>` |
| `sig` | `Vec<i64>` |
| `password` | `str` |

*Defined at line 1015*

---

## ssh_userauth_signed_data

The data a publickey signature covers (RFC 4252 §7).

```fides
pub fn ssh_userauth_signed_data(session_id: Vec<i64>, user: str, alg: str, key_blob: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `session_id`: `Vec<i64>`
- `user`: `str`
- `alg`: `str`
- `key_blob`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1037*

---

## ssh_userauth_publickey_query

"Would this key be accepted?" (publickey without a signature).

```fides
pub fn ssh_userauth_publickey_query(user: str, public_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `user`: `str`
- `public_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1047*

---

## ssh_userauth_publickey

A signed publickey request with an Ed25519 user key seed.

```fides
pub fn ssh_userauth_publickey(s: &SshSession, user: str, seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `s`: `&SshSession`
- `user`: `str`
- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1056*

---

## ssh_userauth_password

A password request.

```fides
pub fn ssh_userauth_password(user: str, password: str) -> Vec<i64> {
```

**Parameters:**

- `user`: `str`
- `password`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1067*

---

## ssh_parse_userauth

Parse a USERAUTH_REQUEST (methods none, password, publickey).

```fides
pub fn ssh_parse_userauth(payload: Vec<i64>) -> SshAuthRequest {
```

**Parameters:**

- `payload`: `Vec<i64>`

**Returns:** `SshAuthRequest`

*Defined at line 1075*

---

## ssh_userauth_check_signature

Server: check a signed publickey request against this session.

```fides
pub fn ssh_userauth_check_signature(s: &SshSession, q: &SshAuthRequest) -> bool {
```

**Parameters:**

- `s`: `&SshSession`
- `q`: `&SshAuthRequest`

**Returns:** `bool`

*Defined at line 1105*

---

## ssh_userauth_failure

USERAUTH_FAILURE listing the methods that may continue.

```fides
pub fn ssh_userauth_failure(methods: str, partial: bool) -> Vec<i64> {
```

**Parameters:**

- `methods`: `str`
- `partial`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1113*

---

## ssh_userauth_success

USERAUTH_SUCCESS.

```fides
pub fn ssh_userauth_success() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 1122*

---

## ssh_userauth_pk_ok

USERAUTH_PK_OK for a queried key.

```fides
pub fn ssh_userauth_pk_ok(alg: str, key_blob: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `alg`: `str`
- `key_blob`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1129*

---

## ssh_userauth_banner

USERAUTH_BANNER.

```fides
pub fn ssh_userauth_banner(text: str) -> Vec<i64> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1138*

---

## ssh_parse_userauth_failure

The method list of a USERAUTH_FAILURE.

```fides
pub fn ssh_parse_userauth_failure(payload: Vec<i64>) -> str {
```

**Parameters:**

- `payload`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1147*

---

## SshChannelMsg

A parsed channel message; fields not in the message stay zero/empty.

```fides
pub struct SshChannelMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `msg` | `i64` |
| `recipient` | `i64` |
| `sender` | `i64` |
| `window` | `i64` |
| `max_packet` | `i64` |
| `kind` | `str` |
| `want_reply` | `bool` |
| `data` | `Vec<i64>` |
| `data_type` | `i64` |
| `text` | `str` |

*Defined at line 1158*

---

## ssh_channel_open

CHANNEL_OPEN for a "session" channel.

```fides
pub fn ssh_channel_open(sender: i64, window: i64, max_packet: i64) -> Vec<i64> {
```

**Parameters:**

- `sender`: `i64`
- `window`: `i64`
- `max_packet`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1173*

---

## ssh_channel_open_confirm

CHANNEL_OPEN_CONFIRMATION.

```fides
pub fn ssh_channel_open_confirm(recipient: i64, sender: i64, window: i64, max_packet: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `sender`: `i64`
- `window`: `i64`
- `max_packet`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1184*

---

## ssh_channel_open_failure

CHANNEL_OPEN_FAILURE (reason 1 = administratively prohibited, ...).

```fides
pub fn ssh_channel_open_failure(recipient: i64, reason: i64, text: str) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `reason`: `i64`
- `text`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1195*

---

## ssh_channel_window_adjust

CHANNEL_WINDOW_ADJUST.

```fides
pub fn ssh_channel_window_adjust(recipient: i64, add: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `add`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1213*

---

## ssh_channel_data

CHANNEL_DATA.

```fides
pub fn ssh_channel_data(recipient: i64, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1220*

---

## ssh_channel_extended_data

CHANNEL_EXTENDED_DATA (data_type 1 = stderr).

```fides
pub fn ssh_channel_extended_data(recipient: i64, data_type: i64, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `data_type`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1227*

---

## ssh_channel_eof

CHANNEL_EOF.

```fides
pub fn ssh_channel_eof(recipient: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1235*

---

## ssh_channel_close

CHANNEL_CLOSE.

```fides
pub fn ssh_channel_close(recipient: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1240*

---

## ssh_channel_success

CHANNEL_SUCCESS.

```fides
pub fn ssh_channel_success(recipient: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1245*

---

## ssh_channel_failure

CHANNEL_FAILURE.

```fides
pub fn ssh_channel_failure(recipient: i64) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1250*

---

## ssh_channel_request

CHANNEL_REQUEST "exec" with a command, or "subsystem" with a name.

```fides
pub fn ssh_channel_request(recipient: i64, kind: str, want_reply: bool, arg: str) -> Vec<i64> {
```

**Parameters:**

- `recipient`: `i64`
- `kind`: `str`
- `want_reply`: `bool`
- `arg`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1255*

---

## ssh_parse_channel

Parse any channel message (90-100).

```fides
pub fn ssh_parse_channel(payload: Vec<i64>) -> SshChannelMsg {
```

**Parameters:**

- `payload`: `Vec<i64>`

**Returns:** `SshChannelMsg`

*Defined at line 1266*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
