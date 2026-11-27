[All modules](index.md)

# noise_proto -- Fides Documentation

## Contents

- [NOISE_SHA256:](#NOISE_SHA256:) (const)
- [NOISE_SHA512:](#NOISE_SHA512:) (const)
- [NOISE_BLAKE2S:](#NOISE_BLAKE2S:) (const)
- [NOISE_CHACHAPOLY:](#NOISE_CHACHAPOLY:) (const)
- [NOISE_AESGCM:](#NOISE_AESGCM:) (const)
- [NOISE_MAX_MSG:](#NOISE_MAX_MSG:) (const)
- [NoiseBytes](#NoiseBytes) (struct)
- [NoiseCipher](#NoiseCipher) (struct)
- [NoiseTransport](#NoiseTransport) (struct)
- [NoiseHandshake](#NoiseHandshake) (struct)
- [noise_blake2s](#noise_blake2s) (function)
- [noise_hash](#noise_hash) (function)
- [noise_hash_len](#noise_hash_len) (function)
- [noise_hmac](#noise_hmac) (function)
- [noise_hkdf](#noise_hkdf) (function)
- [noise_cipher_new](#noise_cipher_new) (function)
- [noise_encrypt_with_nonce](#noise_encrypt_with_nonce) (function)
- [noise_decrypt_with_nonce](#noise_decrypt_with_nonce) (function)
- [noise_encrypt](#noise_encrypt) (function)
- [noise_decrypt](#noise_decrypt) (function)
- [noise_rekey_key](#noise_rekey_key) (function)
- [noise_rekey](#noise_rekey) (function)
- [noise_pattern](#noise_pattern) (function)
- [noise_pattern_with_mods](#noise_pattern_with_mods) (function)
- [noise_handshake_new](#noise_handshake_new) (function)
- [noise_set_ephemeral](#noise_set_ephemeral) (function)
- [noise_handshake_done](#noise_handshake_done) (function)
- [noise_my_turn](#noise_my_turn) (function)
- [noise_handshake_hash](#noise_handshake_hash) (function)
- [noise_write_message](#noise_write_message) (function)
- [noise_read_message](#noise_read_message) (function)
- [noise_split](#noise_split) (function)

## NOISE_SHA256:

```fides
pub const NOISE_SHA256: i64 = 1;
```

*Defined at line 46*

---

## NOISE_SHA512:

```fides
pub const NOISE_SHA512: i64 = 2;
```

*Defined at line 47*

---

## NOISE_BLAKE2S:

```fides
pub const NOISE_BLAKE2S: i64 = 3;
```

*Defined at line 48*

---

## NOISE_CHACHAPOLY:

```fides
pub const NOISE_CHACHAPOLY: i64 = 1;
```

*Defined at line 49*

---

## NOISE_AESGCM:

```fides
pub const NOISE_AESGCM: i64 = 2;
```

*Defined at line 50*

---

## NOISE_MAX_MSG:

Largest Noise message (§3).

```fides
pub const NOISE_MAX_MSG: i64 = 65535;
```

*Defined at line 52*

---

## NoiseBytes

Bytes with a success flag.

```fides
pub struct NoiseBytes {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `data` | `Vec<i64>` |

*Defined at line 55*

---

## NoiseCipher

A CipherState (§5.1): key k (empty = none) and nonce n.

```fides
pub struct NoiseCipher {
```

**Fields:**

| Name | Type |
|------|------|
| `k` | `Vec<i64>` |
| `n` | `i64` |
| `cipher` | `i64` |

*Defined at line 61*

---

## NoiseTransport

The two transport ciphers after Split, oriented for one side, and the
handshake hash (for channel binding).

```fides
pub struct NoiseTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `send` | `NoiseCipher` |
| `recv` | `NoiseCipher` |
| `hash` | `Vec<i64>` |

*Defined at line 69*

---

## NoiseHandshake

A HandshakeState (§5.3) with its SymmetricState (§5.2) folded in.

```fides
pub struct NoiseHandshake {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `initiator` | `bool` |
| `hash` | `i64` |
| `cipher` | `i64` |
| `ck` | `Vec<i64>` |
| `h` | `Vec<i64>` |
| `k` | `Vec<i64>` |
| `n` | `i64` |
| `s` | `Vec<i64>` |
| `s_pub` | `Vec<i64>` |
| `e` | `Vec<i64>` |
| `e_pub` | `Vec<i64>` |
| `rs` | `Vec<i64>` |
| `re` | `Vec<i64>` |
| `psk` | `Vec<i64>` |
| `has_psk` | `bool` |
| `msgs` | `Vec<str>` |
| `idx` | `i64` |
| `e_next` | `Vec<i64>` |

*Defined at line 77*

---

## noise_blake2s

BLAKE2s with an optional key (0..32 bytes) and `outlen` (1..32)
output bytes; empty on bad parameters.

```fides
pub fn noise_blake2s(key: Vec<i64>, msg: Vec<i64>, outlen: i64) -> Vec<i64> {
```

**Parameters:**

- `key`: `Vec<i64>`
- `msg`: `Vec<i64>`
- `outlen`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 240*

---

## noise_hash

HASH(data) for a hash id.

```fides
pub fn noise_hash(hash: i64, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `hash`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 291*

---

## noise_hash_len

HASHLEN.

```fides
pub fn noise_hash_len(hash: i64) -> i64 {
```

**Parameters:**

- `hash`: `i64`

**Returns:** `i64`

*Defined at line 302*

---

## noise_hmac

HMAC-HASH (RFC 2104) with the hash's block length.

```fides
pub fn noise_hmac(hash: i64, key: Vec<i64>, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `hash`: `i64`
- `key`: `Vec<i64>`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 310*

---

## noise_hkdf

HKDF(chaining_key, input_key_material, num_outputs): the outputs
concatenated (2 or 3 times HASHLEN bytes).

```fides
pub fn noise_hkdf(hash: i64, ck: Vec<i64>, ikm: Vec<i64>, num: i64) -> Vec<i64> {
```

**Parameters:**

- `hash`: `i64`
- `ck`: `Vec<i64>`
- `ikm`: `Vec<i64>`
- `num`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 335*

---

## noise_cipher_new

A cipher state holding key `k` (32 bytes) with nonce 0.

```fides
pub fn noise_cipher_new(cipher: i64, k: Vec<i64>) -> NoiseCipher {
```

**Parameters:**

- `cipher`: `i64`
- `k`: `Vec<i64>`

**Returns:** `NoiseCipher`

*Defined at line 355*

---

## noise_encrypt_with_nonce

ENCRYPT(k, n, ad, plaintext) with an explicit 64-bit nonce (WireGuard
and other protocols that carry the counter in the packet).

```fides
pub fn noise_encrypt_with_nonce(cipher: i64, k: Vec<i64>, n: i64, ad: Vec<i64>, plain: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cipher`: `i64`
- `k`: `Vec<i64>`
- `n`: `i64`
- `ad`: `Vec<i64>`
- `plain`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 408*

---

## noise_decrypt_with_nonce

DECRYPT(k, n, ad, ciphertext) with an explicit nonce.

```fides
pub fn noise_decrypt_with_nonce(cipher: i64, k: Vec<i64>, n: i64, ad: Vec<i64>, ct: Vec<i64>) -> NoiseBytes {
```

**Parameters:**

- `cipher`: `i64`
- `k`: `Vec<i64>`
- `n`: `i64`
- `ad`: `Vec<i64>`
- `ct`: `Vec<i64>`

**Returns:** `NoiseBytes`

*Defined at line 413*

---

## noise_encrypt

EncryptWithAd: the plaintext unchanged when there is no key yet.

```fides
pub fn noise_encrypt(c: &mut NoiseCipher, ad: Vec<i64>, plain: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut NoiseCipher`
- `ad`: `Vec<i64>`
- `plain`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 418*

---

## noise_decrypt

DecryptWithAd: the nonce advances only on success.

```fides
pub fn noise_decrypt(c: &mut NoiseCipher, ad: Vec<i64>, ct: Vec<i64>) -> NoiseBytes {
```

**Parameters:**

- `c`: `&mut NoiseCipher`
- `ad`: `Vec<i64>`
- `ct`: `Vec<i64>`

**Returns:** `NoiseBytes`

*Defined at line 428*

---

## noise_rekey_key

REKEY(k): the first 32 bytes of ENCRYPT(k, 2^64 - 1, "", 32 zeros) (§4.2).

```fides
pub fn noise_rekey_key(cipher: i64, k: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cipher`: `i64`
- `k`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 442*

---

## noise_rekey

Rekey() of a cipher state; the nonce is kept (§11.3).

```fides
pub fn noise_rekey(c: &mut NoiseCipher) {
```

**Parameters:**

- `c`: `&mut NoiseCipher`

*Defined at line 448*

---

## noise_pattern

The message token lists of a fundamental pattern, each message as a
comma-separated list; empty when the name is unknown. The first
element is the pre-message line: "i:s", "r:s", "i:s r:s" or "".

```fides
pub fn noise_pattern(name: str) -> Vec<str> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 460*

---

## noise_pattern_with_mods

The pattern with "pskN" modifiers applied (e.g. "IKpsk2", "NNpsk0+psk2").

```fides
pub fn noise_pattern_with_mods(full: str) -> Vec<str> {
```

**Parameters:**

- `full`: `str`

**Returns:** `Vec<str>`

*Defined at line 541*

---

## noise_handshake_new

Initialize(handshake_pattern, initiator, prologue, s, e, rs, re) for a
protocol name. `s` is our 32-byte static private key (empty if the
pattern sends none), `rs` the peer's static public key when the
pattern has it as a pre-message (empty otherwise), `psk` 32 bytes when
the name has psk modifiers. ok is false on an unsupported name or a
missing key.

```fides
pub fn noise_handshake_new(protocol: str, initiator: bool, prologue: Vec<i64>, s: Vec<i64>, rs: Vec<i64>, psk: Vec<i64>) -> NoiseHandshake {
```

**Parameters:**

- `protocol`: `str`
- `initiator`: `bool`
- `prologue`: `Vec<i64>`
- `s`: `Vec<i64>`
- `rs`: `Vec<i64>`
- `psk`: `Vec<i64>`

**Returns:** `NoiseHandshake`

*Defined at line 642*

---

## noise_set_ephemeral

Seam: the private key of the next ephemeral key this side generates
(32 bytes). Without it, ephemeral keys come from crypto_random_bytes.

```fides
pub fn noise_set_ephemeral(hs: &mut NoiseHandshake, e: Vec<i64>) {
```

**Parameters:**

- `hs`: `&mut NoiseHandshake`
- `e`: `Vec<i64>`

*Defined at line 732*

---

## noise_handshake_done

True when every handshake message has been written or read.

```fides
pub fn noise_handshake_done(hs: &NoiseHandshake) -> bool {
```

**Parameters:**

- `hs`: `&NoiseHandshake`

**Returns:** `bool`

*Defined at line 737*

---

## noise_my_turn

True when it is this side's turn to write.

```fides
pub fn noise_my_turn(hs: &NoiseHandshake) -> bool {
```

**Parameters:**

- `hs`: `&NoiseHandshake`

**Returns:** `bool`

*Defined at line 742*

---

## noise_handshake_hash

The handshake hash h so far (channel binding once done).

```fides
pub fn noise_handshake_hash(hs: &NoiseHandshake) -> Vec<i64> {
```

**Parameters:**

- `hs`: `&NoiseHandshake`

**Returns:** `Vec<i64>`

*Defined at line 747*

---

## noise_write_message

WriteMessage(payload, message_buffer): the next handshake message, or
empty with ok false (hs.err says why).

```fides
pub fn noise_write_message(hs: &mut NoiseHandshake, payload: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `hs`: `&mut NoiseHandshake`
- `payload`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 793*

---

## noise_read_message

ReadMessage(message, payload_buffer): the decrypted payload. On any
failure ok is false and the handshake is dead (hs.ok false).

```fides
pub fn noise_read_message(hs: &mut NoiseHandshake, msg: Vec<i64>) -> NoiseBytes {
```

**Parameters:**

- `hs`: `&mut NoiseHandshake`
- `msg`: `Vec<i64>`

**Returns:** `NoiseBytes`

*Defined at line 850*

---

## noise_split

Split() (§5.2) oriented for this side: the initiator sends with the
first key, the responder with the second.

```fides
pub fn noise_split(hs: &NoiseHandshake) -> NoiseTransport {
```

**Parameters:**

- `hs`: `&NoiseHandshake`

**Returns:** `NoiseTransport`

*Defined at line 924*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
