[All modules](index.md)

# hpke -- Fides Documentation

## Contents

- [HPKE_MODE_BASE:](#HPKE_MODE_BASE:) (const)
- [HPKE_MODE_PSK:](#HPKE_MODE_PSK:) (const)
- [HPKE_MODE_AUTH:](#HPKE_MODE_AUTH:) (const)
- [HPKE_MODE_AUTH_PSK:](#HPKE_MODE_AUTH_PSK:) (const)
- [HpkeContext](#HpkeContext) (struct)
- [HpkeEncap](#HpkeEncap) (struct)
- [HpkeSender](#HpkeSender) (struct)
- [HpkeKeyPair](#HpkeKeyPair) (struct)
- [hpke_kem_suite](#hpke_kem_suite) (function)
- [hpke_suite](#hpke_suite) (function)
- [hpke_labeled_extract](#hpke_labeled_extract) (function)
- [hpke_labeled_expand](#hpke_labeled_expand) (function)
- [hpke_derive_keypair](#hpke_derive_keypair) (function)
- [hpke_encap](#hpke_encap) (function)
- [hpke_decap](#hpke_decap) (function)
- [hpke_auth_encap](#hpke_auth_encap) (function)
- [hpke_auth_decap](#hpke_auth_decap) (function)
- [hpke_key_schedule](#hpke_key_schedule) (function)
- [hpke_key_schedule_context](#hpke_key_schedule_context) (function)
- [hpke_setup_base_s](#hpke_setup_base_s) (function)
- [hpke_setup_base_r](#hpke_setup_base_r) (function)
- [hpke_setup_psk_s](#hpke_setup_psk_s) (function)
- [hpke_setup_psk_r](#hpke_setup_psk_r) (function)
- [hpke_setup_auth_s](#hpke_setup_auth_s) (function)
- [hpke_setup_auth_r](#hpke_setup_auth_r) (function)
- [hpke_nonce](#hpke_nonce) (function)
- [hpke_seal](#hpke_seal) (function)
- [hpke_open](#hpke_open) (function)
- [hpke_export](#hpke_export) (function)
- [hpke_seal_base](#hpke_seal_base) (function)
- [hpke_open_base](#hpke_open_base) (function)

## HPKE_MODE_BASE:

```fides
pub const HPKE_MODE_BASE: i64 = 0;
```

*Defined at line 33*

---

## HPKE_MODE_PSK:

```fides
pub const HPKE_MODE_PSK: i64 = 1;
```

*Defined at line 34*

---

## HPKE_MODE_AUTH:

```fides
pub const HPKE_MODE_AUTH: i64 = 2;
```

*Defined at line 35*

---

## HPKE_MODE_AUTH_PSK:

```fides
pub const HPKE_MODE_AUTH_PSK: i64 = 3;
```

*Defined at line 36*

---

## HpkeContext

An encryption context (one direction).

```fides
pub struct HpkeContext {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `key` | `Vec<i64>` |
| `base_nonce` | `Vec<i64>` |
| `exporter_secret` | `Vec<i64>` |
| `seq` | `i64` |

*Defined at line 39*

---

## HpkeEncap

What Encap gives: the encapsulated key for the receiver and the KEM
shared secret.

```fides
pub struct HpkeEncap {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `enc` | `Vec<i64>` |
| `shared_secret` | `Vec<i64>` |

*Defined at line 49*

---

## HpkeSender

A sender's setup: the encapsulated key and the context.

```fides
pub struct HpkeSender {
```

**Fields:**

| Name | Type |
|------|------|
| `enc` | `Vec<i64>` |
| `ctx` | `HpkeContext` |

*Defined at line 56*

---

## HpkeKeyPair

A key pair: 32-byte X25519 secret and public keys.

```fides
pub struct HpkeKeyPair {
```

**Fields:**

| Name | Type |
|------|------|
| `sk` | `Vec<i64>` |
| `pk` | `Vec<i64>` |

*Defined at line 62*

---

## hpke_kem_suite

suite_id of the KEM: "KEM" || I2OSP(0x0020, 2).

```fides
pub fn hpke_kem_suite() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 90*

---

## hpke_suite

suite_id of the whole suite: "HPKE" || kem || kdf || aead.

```fides
pub fn hpke_suite() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 95*

---

## hpke_labeled_extract

LabeledExtract(salt, label, ikm) = Extract(salt, "HPKE-v1" || suite || label || ikm).

```fides
pub fn hpke_labeled_extract(suite: Vec<i64>, salt: Vec<i64>, label: str, ikm: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `suite`: `Vec<i64>`
- `salt`: `Vec<i64>`
- `label`: `str`
- `ikm`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 100*

---

## hpke_labeled_expand

LabeledExpand(prk, label, info, L) = Expand(prk, I2OSP(L, 2) || "HPKE-v1" || suite || label || info, L).

```fides
pub fn hpke_labeled_expand(suite: Vec<i64>, prk: Vec<i64>, label: str, info: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `suite`: `Vec<i64>`
- `prk`: `Vec<i64>`
- `label`: `str`
- `info`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 106*

---

## hpke_derive_keypair

DeriveKeyPair(ikm): sk = LabeledExpand(dkp_prk, "sk", "", 32).

```fides
pub fn hpke_derive_keypair(ikm: Vec<i64>) -> HpkeKeyPair {
```

**Parameters:**

- `ikm`: `Vec<i64>`

**Returns:** `HpkeKeyPair`

*Defined at line 114*

---

## hpke_encap

Encap(pkR) with the ephemeral key derived from ikm_e.

```fides
pub fn hpke_encap(pk_r: Vec<i64>, ikm_e: Vec<i64>) -> HpkeEncap {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `HpkeEncap`

*Defined at line 147*

---

## hpke_decap

Decap(enc, skR): the shared secret, empty on a bad encapsulation.

```fides
pub fn hpke_decap(enc: Vec<i64>, sk_r: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `enc`: `Vec<i64>`
- `sk_r`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 160*

---

## hpke_auth_encap

AuthEncap(pkR, skS): authenticates the sender's static key too.

```fides
pub fn hpke_auth_encap(pk_r: Vec<i64>, sk_s: Vec<i64>, ikm_e: Vec<i64>) -> HpkeEncap {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `sk_s`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `HpkeEncap`

*Defined at line 169*

---

## hpke_auth_decap

AuthDecap(enc, skR, pkS).

```fides
pub fn hpke_auth_decap(enc: Vec<i64>, sk_r: Vec<i64>, pk_s: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `enc`: `Vec<i64>`
- `sk_r`: `Vec<i64>`
- `pk_s`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 183*

---

## hpke_key_schedule

KeySchedule(mode, shared_secret, info, psk, psk_id). psk and psk_id
must both be empty (base/auth) or both non-empty (psk modes).

```fides
pub fn hpke_key_schedule(mode: i64, shared_secret: Vec<i64>, info: Vec<i64>, psk: Vec<i64>, psk_id: Vec<i64>) -> HpkeContext {
```

**Parameters:**

- `mode`: `i64`
- `shared_secret`: `Vec<i64>`
- `info`: `Vec<i64>`
- `psk`: `Vec<i64>`
- `psk_id`: `Vec<i64>`

**Returns:** `HpkeContext`

*Defined at line 203*

---

## hpke_key_schedule_context

The key schedule context bytes (mode || psk_id_hash || info_hash),
as RFC 9180's test vectors list them.

```fides
pub fn hpke_key_schedule_context(mode: i64, info: Vec<i64>, psk_id: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `mode`: `i64`
- `info`: `Vec<i64>`
- `psk_id`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 225*

---

## hpke_setup_base_s

SetupBaseS(pkR, info).

```fides
pub fn hpke_setup_base_s(pk_r: Vec<i64>, info: Vec<i64>, ikm_e: Vec<i64>) -> HpkeSender {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `HpkeSender`

*Defined at line 233*

---

## hpke_setup_base_r

SetupBaseR(enc, skR, info).

```fides
pub fn hpke_setup_base_r(enc: Vec<i64>, sk_r: Vec<i64>, info: Vec<i64>) -> HpkeContext {
```

**Parameters:**

- `enc`: `Vec<i64>`
- `sk_r`: `Vec<i64>`
- `info`: `Vec<i64>`

**Returns:** `HpkeContext`

*Defined at line 246*

---

## hpke_setup_psk_s

SetupPSKS(pkR, info, psk, psk_id).

```fides
pub fn hpke_setup_psk_s(pk_r: Vec<i64>, info: Vec<i64>, psk: Vec<i64>, psk_id: Vec<i64>, ikm_e: Vec<i64>) -> HpkeSender {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `psk`: `Vec<i64>`
- `psk_id`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `HpkeSender`

*Defined at line 251*

---

## hpke_setup_psk_r

SetupPSKR(enc, skR, info, psk, psk_id).

```fides
pub fn hpke_setup_psk_r(enc: Vec<i64>, sk_r: Vec<i64>, info: Vec<i64>, psk: Vec<i64>, psk_id: Vec<i64>) -> HpkeContext {
```

**Parameters:**

- `enc`: `Vec<i64>`
- `sk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `psk`: `Vec<i64>`
- `psk_id`: `Vec<i64>`

**Returns:** `HpkeContext`

*Defined at line 264*

---

## hpke_setup_auth_s

SetupAuthS(pkR, info, skS).

```fides
pub fn hpke_setup_auth_s(pk_r: Vec<i64>, info: Vec<i64>, sk_s: Vec<i64>, ikm_e: Vec<i64>) -> HpkeSender {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `sk_s`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `HpkeSender`

*Defined at line 269*

---

## hpke_setup_auth_r

SetupAuthR(enc, skR, info, pkS).

```fides
pub fn hpke_setup_auth_r(enc: Vec<i64>, sk_r: Vec<i64>, info: Vec<i64>, pk_s: Vec<i64>) -> HpkeContext {
```

**Parameters:**

- `enc`: `Vec<i64>`
- `sk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `pk_s`: `Vec<i64>`

**Returns:** `HpkeContext`

*Defined at line 282*

---

## hpke_nonce

The nonce of the current message: base_nonce XOR I2OSP(seq, 12).

```fides
pub fn hpke_nonce(ctx: &HpkeContext) -> Vec<i64> {
```

**Parameters:**

- `ctx`: `&HpkeContext`

**Returns:** `Vec<i64>`

*Defined at line 287*

---

## hpke_seal

Seal one message (ciphertext || tag) and advance the sequence number.

```fides
pub fn hpke_seal(ctx: &mut HpkeContext, aad: Vec<i64>, plain: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `ctx`: `&mut HpkeContext`
- `aad`: `Vec<i64>`
- `plain`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 299*

---

## hpke_open

Open one message; the sequence number advances only on success.

```fides
pub fn hpke_open(ctx: &mut HpkeContext, aad: Vec<i64>, ct: Vec<i64>) -> AeadOpened {
```

**Parameters:**

- `ctx`: `&mut HpkeContext`
- `aad`: `Vec<i64>`
- `ct`: `Vec<i64>`

**Returns:** `AeadOpened`

*Defined at line 309*

---

## hpke_export

Export(exporter_context, L): a secret for another protocol.

```fides
pub fn hpke_export(ctx: &HpkeContext, exporter_context: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `ctx`: `&HpkeContext`
- `exporter_context`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 323*

---

## hpke_seal_base

Single-shot base mode: enc (32 bytes) || ciphertext || tag.

```fides
pub fn hpke_seal_base(pk_r: Vec<i64>, info: Vec<i64>, aad: Vec<i64>, plain: Vec<i64>, ikm_e: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `aad`: `Vec<i64>`
- `plain`: `Vec<i64>`
- `ikm_e`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 328*

---

## hpke_open_base

Single-shot base mode open of enc || ciphertext || tag.

```fides
pub fn hpke_open_base(sk_r: Vec<i64>, info: Vec<i64>, aad: Vec<i64>, msg: Vec<i64>) -> AeadOpened {
```

**Parameters:**

- `sk_r`: `Vec<i64>`
- `info`: `Vec<i64>`
- `aad`: `Vec<i64>`
- `msg`: `Vec<i64>`

**Returns:** `AeadOpened`

*Defined at line 338*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
