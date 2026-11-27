[All modules](index.md)

# secure_store -- Fides Documentation

## Contents

- [SECURE_OK:](#SECURE_OK:) (const)
- [SECURE_NOT_FOUND:](#SECURE_NOT_FOUND:) (const)
- [SECURE_DUPLICATE:](#SECURE_DUPLICATE:) (const)
- [SECURE_AUTH_FAILED:](#SECURE_AUTH_FAILED:) (const)
- [SECURE_LOCKED:](#SECURE_LOCKED:) (const)
- [SECURE_CANCELED:](#SECURE_CANCELED:) (const)
- [SECURE_CORRUPT:](#SECURE_CORRUPT:) (const)
- [SECURE_BAD_PARAM:](#SECURE_BAD_PARAM:) (const)
- [SECURE_OTHER:](#SECURE_OTHER:) (const)
- [secure_error_name](#secure_error_name) (function)
- [secure_from_osstatus](#secure_from_osstatus) (function)
- [secure_hmac_sha512](#secure_hmac_sha512) (function)
- [secure_pbkdf2_sha512](#secure_pbkdf2_sha512) (function)
- [secure_os_random](#secure_os_random) (function)
- [SECURE_FILE_VERSION:](#SECURE_FILE_VERSION:) (const)
- [SECURE_FILE_HEADER:](#SECURE_FILE_HEADER:) (const)
- [SecureFile](#SecureFile) (struct)
- [secure_file_new](#secure_file_new) (function)
- [secure_put](#secure_put) (function)
- [secure_has](#secure_has) (function)
- [secure_get](#secure_get) (function)
- [secure_delete](#secure_delete) (function)
- [secure_encode_entries](#secure_encode_entries) (function)
- [secure_file_seal](#secure_file_seal) (function)
- [SecureOpen](#SecureOpen) (struct)
- [secure_file_open](#secure_file_open) (function)
- [secure_file_save](#secure_file_save) (function)
- [secure_file_load](#secure_file_load) (function)
- [secure_file_rekey](#secure_file_rekey) (function)
- [KeystoreStep](#KeystoreStep) (struct)
- [keystore_create_key_steps](#keystore_create_key_steps) (function)
- [keystore_encrypt_steps](#keystore_encrypt_steps) (function)
- [keystore_pack](#keystore_pack) (function)
- [KeystorePacked](#KeystorePacked) (struct)
- [keystore_unpack](#keystore_unpack) (function)
- [KSEC_CLASS:](#KSEC_CLASS:) (const)
- [KSEC_CLASS_GENERIC_PASSWORD:](#KSEC_CLASS_GENERIC_PASSWORD:) (const)
- [KSEC_ATTR_SERVICE:](#KSEC_ATTR_SERVICE:) (const)
- [KSEC_ATTR_ACCOUNT:](#KSEC_ATTR_ACCOUNT:) (const)
- [KSEC_ATTR_ACCESSIBLE:](#KSEC_ATTR_ACCESSIBLE:) (const)
- [KSEC_VALUE_DATA:](#KSEC_VALUE_DATA:) (const)
- [KSEC_RETURN_DATA:](#KSEC_RETURN_DATA:) (const)
- [KSEC_MATCH_LIMIT:](#KSEC_MATCH_LIMIT:) (const)
- [KSEC_MATCH_LIMIT_ONE:](#KSEC_MATCH_LIMIT_ONE:) (const)
- [KSEC_ACCESSIBLE_WHEN_UNLOCKED:](#KSEC_ACCESSIBLE_WHEN_UNLOCKED:) (const)
- [KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK:](#KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK:) (const)
- [KSEC_ACCESSIBLE_WHEN_UNLOCKED_THIS_DEVICE:](#KSEC_ACCESSIBLE_WHEN_UNLOCKED_THIS_DEVICE:) (const)
- [KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK_THIS_DEVICE:](#KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK_THIS_DEVICE:) (const)
- [KSEC_ACCESSIBLE_WHEN_PASSCODE_SET_THIS_DEVICE:](#KSEC_ACCESSIBLE_WHEN_PASSCODE_SET_THIS_DEVICE:) (const)
- [KeychainQuery](#KeychainQuery) (struct)
- [keychain_add_query](#keychain_add_query) (function)
- [keychain_get_query](#keychain_get_query) (function)
- [keychain_update_query](#keychain_update_query) (function)
- [keychain_delete_query](#keychain_delete_query) (function)
- [keychain_put_plan](#keychain_put_plan) (function)
- [keychain_query_text](#keychain_query_text) (function)

## SECURE_OK:

```fides
pub const SECURE_OK: i64 = 0;
```

*Defined at line 57*

---

## SECURE_NOT_FOUND:

```fides
pub const SECURE_NOT_FOUND: i64 = 1;
```

*Defined at line 58*

---

## SECURE_DUPLICATE:

```fides
pub const SECURE_DUPLICATE: i64 = 2;
```

*Defined at line 59*

---

## SECURE_AUTH_FAILED:

```fides
pub const SECURE_AUTH_FAILED: i64 = 3;  // wrong passphrase, bad tag, biometrics failed
```

*Defined at line 60*

---

## SECURE_LOCKED:

```fides
pub const SECURE_LOCKED: i64 = 4;  // device locked / interaction not allowed
```

*Defined at line 61*

---

## SECURE_CANCELED:

```fides
pub const SECURE_CANCELED: i64 = 5;
```

*Defined at line 62*

---

## SECURE_CORRUPT:

```fides
pub const SECURE_CORRUPT: i64 = 6;
```

*Defined at line 63*

---

## SECURE_BAD_PARAM:

```fides
pub const SECURE_BAD_PARAM: i64 = 7;
```

*Defined at line 64*

---

## SECURE_OTHER:

```fides
pub const SECURE_OTHER: i64 = 8;
```

*Defined at line 65*

---

## secure_error_name

```fides
pub fn secure_error_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 67*

---

## secure_from_osstatus

An OSStatus of the Security framework as an error code.

```fides
pub fn secure_from_osstatus(st: i64) -> i64 {
```

**Parameters:**

- `st`: `i64`

**Returns:** `i64`

*Defined at line 76*

---

## secure_hmac_sha512

HMAC-SHA512 (RFC 2104, block 128 bytes).

```fides
pub fn secure_hmac_sha512(key: Vec<i64>, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `Vec<i64>`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 107*

---

## secure_pbkdf2_sha512

PBKDF2 with HMAC-SHA512: `dk_len` bytes.

```fides
pub fn secure_pbkdf2_sha512(password: Vec<i64>, salt: Vec<i64>, iterations: i64, dk_len: i64) -> Vec<i64> {
```

**Parameters:**

- `password`: `Vec<i64>`
- `salt`: `Vec<i64>`
- `iterations`: `i64`
- `dk_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 140*

---

## secure_os_random

n bytes from the operating system's generator.

```fides
pub fn secure_os_random(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 174*

---

## SECURE_FILE_VERSION:

```fides
pub const SECURE_FILE_VERSION: i64 = 1;
```

*Defined at line 228*

---

## SECURE_FILE_HEADER:

```fides
pub const SECURE_FILE_HEADER: i64 = 37;  // 4 + 1 + 4 + 16 + 12
```

*Defined at line 229*

---

## SecureFile

```fides
pub struct SecureFile {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<str>` |
| `vals` | `Vec<Vec<i64>>` |
| `passphrase` | `str` |
| `iterations` | `i64` |
| `random` | `fn(i64) -> Vec<i64>` |
| `saves` | `i64` |

*Defined at line 231*

---

## secure_file_new

```fides
pub fn secure_file_new(passphrase: str, iterations: i64, random: fn(i64) -> Vec<i64>) -> SecureFile {
```

**Parameters:**

- `passphrase`: `str`
- `iterations`: `i64`
- `random`: `fn(i64) -> Vec<i64>`

**Returns:** `SecureFile`

*Defined at line 240*

---

## secure_put

Store a value (replacing an existing one).

```fides
pub fn secure_put(s: &mut SecureFile, key: str, value: Vec<i64>) {
```

**Parameters:**

- `s`: `&mut SecureFile`
- `key`: `str`
- `value`: `Vec<i64>`

*Defined at line 258*

---

## secure_has

```fides
pub fn secure_has(s: &SecureFile, key: str) -> bool {
```

**Parameters:**

- `s`: `&SecureFile`
- `key`: `str`

**Returns:** `bool`

*Defined at line 268*

---

## secure_get

The value of a key (empty when absent: check secure_has).

```fides
pub fn secure_get(s: &SecureFile, key: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `&SecureFile`
- `key`: `str`

**Returns:** `Vec<i64>`

*Defined at line 273*

---

## secure_delete

```fides
pub fn secure_delete(s: &mut SecureFile, key: str) -> bool {
```

**Parameters:**

- `s`: `&mut SecureFile`
- `key`: `str`

**Returns:** `bool`

*Defined at line 281*

---

## secure_encode_entries

The entries as bytes (before encryption).

```fides
pub fn secure_encode_entries(s: &SecureFile) -> Vec<i64> {
```

**Parameters:**

- `s`: `&SecureFile`

**Returns:** `Vec<i64>`

*Defined at line 292*

---

## secure_file_seal

Seal the map into the file format (a new salt and nonce each time).

```fides
pub fn secure_file_seal(s: &mut SecureFile) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut SecureFile`

**Returns:** `Vec<i64>`

*Defined at line 309*

---

## SecureOpen

The result of opening a file.

```fides
pub struct SecureOpen {
```

**Fields:**

| Name | Type |
|------|------|
| `error` | `i64,  // SECURE_OK, SECURE_AUTH_FAILED, SECURE_CORRUPT` |
| `store` | `SecureFile` |

*Defined at line 327*

---

## secure_file_open

Open sealed bytes with a passphrase. The returned store saves with
the OS generator; set `store.random` to change it.

```fides
pub fn secure_file_open(blob: Vec<i64>, passphrase: str) -> SecureOpen {
```

**Parameters:**

- `blob`: `Vec<i64>`
- `passphrase`: `str`

**Returns:** `SecureOpen`

*Defined at line 338*

---

## secure_file_save

Seal and write to `path` (binary-safe); true on success.

```fides
pub fn secure_file_save(s: &mut SecureFile, path: str) -> bool {
```

**Parameters:**

- `s`: `&mut SecureFile`
- `path`: `str`

**Returns:** `bool`

*Defined at line 408*

---

## secure_file_load

Read and open the file at `path` (SECURE_NOT_FOUND when absent).

```fides
pub fn secure_file_load(path: str, passphrase: str) -> SecureOpen {
```

**Parameters:**

- `path`: `str`
- `passphrase`: `str`

**Returns:** `SecureOpen`

*Defined at line 414*

---

## secure_file_rekey

Change the passphrase (and iteration count): the next save uses it.

```fides
pub fn secure_file_rekey(s: &mut SecureFile, passphrase: str, iterations: i64) {
```

**Parameters:**

- `s`: `&mut SecureFile`
- `passphrase`: `str`
- `iterations`: `i64`

*Defined at line 424*

---

## KeystoreStep

```fides
pub struct KeystoreStep {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |
| `args` | `str,  // what to pass, for the JNI layer` |

*Defined at line 431*

---

## keystore_create_key_steps

The steps creating an AES-256-GCM key under `alias` in AndroidKeyStore
(PURPOSE_ENCRYPT | PURPOSE_DECRYPT = 3, BLOCK_MODE_GCM, no padding;
optionally bound to user authentication).

```fides
pub fn keystore_create_key_steps(alias: str, require_auth: bool) -> Vec<KeystoreStep> {
```

**Parameters:**

- `alias`: `str`
- `require_auth`: `bool`

**Returns:** `Vec<KeystoreStep>`

*Defined at line 447*

---

## keystore_encrypt_steps

The steps encrypting with the key: Cipher "AES/GCM/NoPadding",
ENCRYPT_MODE (1) lets the keystore choose the IV, then getIV, doFinal.

```fides
pub fn keystore_encrypt_steps(alias: str) -> Vec<KeystoreStep> {
```

**Parameters:**

- `alias`: `str`

**Returns:** `Vec<KeystoreStep>`

*Defined at line 466*

---

## keystore_pack

The stored form of a Keystore-encrypted value: version 1, the 12-byte
IV, then ciphertext with its 16-byte tag; base64 for SharedPreferences.

```fides
pub fn keystore_pack(iv: Vec<i64>, ct_and_tag: Vec<i64>) -> str {
```

**Parameters:**

- `iv`: `Vec<i64>`
- `ct_and_tag`: `Vec<i64>`

**Returns:** `str`

*Defined at line 480*

---

## KeystorePacked

```fides
pub struct KeystorePacked {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `iv` | `Vec<i64>` |
| `ct_and_tag` | `Vec<i64>` |

*Defined at line 488*

---

## keystore_unpack

Split a keystore_pack string (the decrypt side passes the IV in a
GCMParameterSpec(128, iv) to Cipher.init(DECRYPT_MODE = 2, ...)).

```fides
pub fn keystore_unpack(s: str) -> KeystorePacked {
```

**Parameters:**

- `s`: `str`

**Returns:** `KeystorePacked`

*Defined at line 496*

---

## KSEC_CLASS:

```fides
pub const KSEC_CLASS: str = "class";
```

*Defined at line 511*

---

## KSEC_CLASS_GENERIC_PASSWORD:

```fides
pub const KSEC_CLASS_GENERIC_PASSWORD: str = "genp";
```

*Defined at line 512*

---

## KSEC_ATTR_SERVICE:

```fides
pub const KSEC_ATTR_SERVICE: str = "svce";
```

*Defined at line 513*

---

## KSEC_ATTR_ACCOUNT:

```fides
pub const KSEC_ATTR_ACCOUNT: str = "acct";
```

*Defined at line 514*

---

## KSEC_ATTR_ACCESSIBLE:

```fides
pub const KSEC_ATTR_ACCESSIBLE: str = "pdmn";
```

*Defined at line 515*

---

## KSEC_VALUE_DATA:

```fides
pub const KSEC_VALUE_DATA: str = "v_Data";
```

*Defined at line 516*

---

## KSEC_RETURN_DATA:

```fides
pub const KSEC_RETURN_DATA: str = "r_Data";
```

*Defined at line 517*

---

## KSEC_MATCH_LIMIT:

```fides
pub const KSEC_MATCH_LIMIT: str = "m_Limit";
```

*Defined at line 518*

---

## KSEC_MATCH_LIMIT_ONE:

```fides
pub const KSEC_MATCH_LIMIT_ONE: str = "m_LimitOne";
```

*Defined at line 519*

---

## KSEC_ACCESSIBLE_WHEN_UNLOCKED:

```fides
pub const KSEC_ACCESSIBLE_WHEN_UNLOCKED: str = "ak";
```

*Defined at line 520*

---

## KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK:

```fides
pub const KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK: str = "ck";
```

*Defined at line 521*

---

## KSEC_ACCESSIBLE_WHEN_UNLOCKED_THIS_DEVICE:

```fides
pub const KSEC_ACCESSIBLE_WHEN_UNLOCKED_THIS_DEVICE: str = "aku";
```

*Defined at line 522*

---

## KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK_THIS_DEVICE:

```fides
pub const KSEC_ACCESSIBLE_AFTER_FIRST_UNLOCK_THIS_DEVICE: str = "cku";
```

*Defined at line 523*

---

## KSEC_ACCESSIBLE_WHEN_PASSCODE_SET_THIS_DEVICE:

```fides
pub const KSEC_ACCESSIBLE_WHEN_PASSCODE_SET_THIS_DEVICE: str = "akpu";
```

*Defined at line 524*

---

## KeychainQuery

A Keychain query: keys and values (values starting with "data:" are
the base64 of an NSData, "bool:" a CFBoolean; the rest are strings).

```fides
pub struct KeychainQuery {
```

**Fields:**

| Name | Type |
|------|------|
| `op` | `str,  // SecItemAdd, SecItemCopyMatching, SecItemUpdate, SecItemDelete` |
| `keys` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `update_keys` | `Vec<str>,  // SecItemUpdate's attributes to change` |
| `update_vals` | `Vec<str>` |

*Defined at line 528*

---

## keychain_add_query

```fides
pub fn keychain_add_query(service: str, account: str, value: Vec<i64>, accessible: str) -> KeychainQuery {
```

**Parameters:**

- `service`: `str`
- `account`: `str`
- `value`: `Vec<i64>`
- `accessible`: `str`

**Returns:** `KeychainQuery`

*Defined at line 549*

---

## keychain_get_query

```fides
pub fn keychain_get_query(service: str, account: str) -> KeychainQuery {
```

**Parameters:**

- `service`: `str`
- `account`: `str`

**Returns:** `KeychainQuery`

*Defined at line 558*

---

## keychain_update_query

```fides
pub fn keychain_update_query(service: str, account: str, value: Vec<i64>) -> KeychainQuery {
```

**Parameters:**

- `service`: `str`
- `account`: `str`
- `value`: `Vec<i64>`

**Returns:** `KeychainQuery`

*Defined at line 567*

---

## keychain_delete_query

```fides
pub fn keychain_delete_query(service: str, account: str) -> KeychainQuery {
```

**Parameters:**

- `service`: `str`
- `account`: `str`

**Returns:** `KeychainQuery`

*Defined at line 574*

---

## keychain_put_plan

The queries of a put: add, and on errSecDuplicateItem update instead.

```fides
pub fn keychain_put_plan(service: str, account: str, value: Vec<i64>, add_status: i64) -> Vec<KeychainQuery> {
```

**Parameters:**

- `service`: `str`
- `account`: `str`
- `value`: `Vec<i64>`
- `add_status`: `i64`

**Returns:** `Vec<KeychainQuery>`

*Defined at line 579*

---

## keychain_query_text

One line per key: "key=value", for logs and tests.

```fides
pub fn keychain_query_text(q: &KeychainQuery) -> str {
```

**Parameters:**

- `q`: `&KeychainQuery`

**Returns:** `str`

*Defined at line 589*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
