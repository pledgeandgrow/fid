[All modules](index.md)

# secrets -- Fides Documentation

## Contents

- [pbkdf2_sha512](#pbkdf2_sha512) (function)
- [pbkdf2_sha512_str](#pbkdf2_sha512_str) (function)
- [zeroize](#zeroize) (function)
- [VaultEntry](#VaultEntry) (struct)
- [Vault](#Vault) (struct)
- [VaultOpened](#VaultOpened) (struct)
- [vault_new](#vault_new) (function)
- [vault_set](#vault_set) (function)
- [vault_get](#vault_get) (function)
- [vault_has](#vault_has) (function)
- [vault_remove](#vault_remove) (function)
- [vault_labels](#vault_labels) (function)
- [vault_encode](#vault_encode) (function)
- [vault_decode](#vault_decode) (function)
- [vault_seal_with](#vault_seal_with) (function)
- [vault_seal](#vault_seal) (function)
- [secrets_random](#secrets_random) (function)
- [vault_open](#vault_open) (function)
- [Keychain](#Keychain) (struct)
- [keychain_store](#keychain_store) (function)
- [keychain_load](#keychain_load) (function)

## pbkdf2_sha512

PBKDF2 (RFC 8018) with HMAC-SHA512 as the PRF: derive `dklen` bytes from
`password` and `salt` over `iters` iterations. `iters` >= 1.

```fides
pub fn pbkdf2_sha512(password: Vec<i64>, salt: Vec<i64>, iters: i64, dklen: i64) -> Vec<i64> {
```

**Parameters:**

- `password`: `Vec<i64>`
- `salt`: `Vec<i64>`
- `iters`: `i64`
- `dklen`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 49*

---

## pbkdf2_sha512_str

PBKDF2-HMAC-SHA512 over the bytes of two strings.

```fides
pub fn pbkdf2_sha512_str(password: str, salt: str, iters: i64, dklen: i64) -> Vec<i64> {
```

**Parameters:**

- `password`: `str`
- `salt`: `str`
- `iters`: `i64`
- `dklen`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 91*

---

## zeroize

Overwrite every byte of `b` with zero (wipe a secret after use).

```fides
pub fn zeroize(b: &mut Vec<i64>) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`

*Defined at line 98*

---

## VaultEntry

```fides
pub struct VaultEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `label` | `str` |
| `value` | `Vec<i64>` |

*Defined at line 108*

---

## Vault

```fides
pub struct Vault {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<VaultEntry>` |

*Defined at line 113*

---

## VaultOpened

The result of opening a sealed vault.

```fides
pub struct VaultOpened {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `vault` | `Vault` |

*Defined at line 118*

---

## vault_new

```fides
pub fn vault_new() -> Vault {
```

**Returns:** `Vault`

*Defined at line 124*

---

## vault_set

Set (or replace) the secret under `label`.

```fides
pub fn vault_set(v: &mut Vault, label: str, value: Vec<i64>) {
```

**Parameters:**

- `v`: `&mut Vault`
- `label`: `str`
- `value`: `Vec<i64>`

*Defined at line 131*

---

## vault_get

The secret under `label`, or [] when absent.

```fides
pub fn vault_get(v: &Vault, label: str) -> Vec<i64> {
```

**Parameters:**

- `v`: `&Vault`
- `label`: `str`

**Returns:** `Vec<i64>`

*Defined at line 148*

---

## vault_has

```fides
pub fn vault_has(v: &Vault, label: str) -> bool {
```

**Parameters:**

- `v`: `&Vault`
- `label`: `str`

**Returns:** `bool`

*Defined at line 159*

---

## vault_remove

Remove `label`; returns whether it was present.

```fides
pub fn vault_remove(v: &mut Vault, label: str) -> bool {
```

**Parameters:**

- `v`: `&mut Vault`
- `label`: `str`

**Returns:** `bool`

*Defined at line 171*

---

## vault_labels

The labels in insertion order.

```fides
pub fn vault_labels(v: &Vault) -> Vec<str> {
```

**Parameters:**

- `v`: `&Vault`

**Returns:** `Vec<str>`

*Defined at line 184*

---

## vault_encode

The plaintext encoding of the entries (before encryption).

```fides
pub fn vault_encode(v: &Vault) -> Vec<i64> {
```

**Parameters:**

- `v`: `&Vault`

**Returns:** `Vec<i64>`

*Defined at line 200*

---

## vault_decode

Parse the plaintext back into a vault (ok=false on a malformed blob).

```fides
pub fn vault_decode(b: Vec<i64>) -> VaultOpened {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `VaultOpened`

*Defined at line 243*

---

## vault_seal_with

Seal a vault: derive a 32-byte key from the passphrase with PBKDF2 over
`iters`, then AES-256-GCM. `salt` and `iv` are given so a test is
reproducible; use vault_seal for random ones.

```fides
pub fn vault_seal_with(v: &Vault, passphrase: str, iters: i64, salt: Vec<i64>, iv: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `&Vault`
- `passphrase`: `str`
- `iters`: `i64`
- `salt`: `Vec<i64>`
- `iv`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 294*

---

## vault_seal

Seal with a fresh random 16-byte salt and 12-byte IV (p_os_random_u64).

```fides
pub fn vault_seal(v: &Vault, passphrase: str, iters: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `&Vault`
- `passphrase`: `str`
- `iters`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 328*

---

## secrets_random

`n` random bytes from the OS RNG.

```fides
pub fn secrets_random(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 333*

---

## vault_open

Open a sealed vault with the passphrase. ok=false when the magic is
wrong, the blob is truncated, or the tag does not verify (a wrong
passphrase or a tampered file both fail here).

```fides
pub fn vault_open(blob: Vec<i64>, passphrase: str) -> VaultOpened {
```

**Parameters:**

- `blob`: `Vec<i64>`
- `passphrase`: `str`

**Returns:** `VaultOpened`

*Defined at line 350*

---

## Keychain

A keychain backend: store, load (len<0 = absent) and delete a secret by
service+account. A caller supplies function values bound to DPAPI, the
macOS Keychain or the Secret Service.

```fides
pub struct Keychain {
```

**Fields:**

| Name | Type |
|------|------|
| `store` | `fn(str, str, Vec<i64>) -> bool` |
| `load` | `fn(str, str) -> Vec<i64>` |
| `remove` | `fn(str, str) -> bool` |

*Defined at line 417*

---

## keychain_store

Store a secret through a keychain backend.

```fides
pub fn keychain_store(k: &Keychain, service: str, account: str, secret: Vec<i64>) -> bool {
```

**Parameters:**

- `k`: `&Keychain`
- `service`: `str`
- `account`: `str`
- `secret`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 424*

---

## keychain_load

Load a secret through a keychain backend ([] when absent).

```fides
pub fn keychain_load(k: &Keychain, service: str, account: str) -> Vec<i64> {
```

**Parameters:**

- `k`: `&Keychain`
- `service`: `str`
- `account`: `str`

**Returns:** `Vec<i64>`

*Defined at line 430*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
