[All modules](index.md)

# ota -- Fides Documentation

## Contents

- [ota_hash_hex](#ota_hash_hex) (function)
- [OTA_STATE_IDLE:](#OTA_STATE_IDLE:) (const)
- [OTA_STATE_DOWNLOADING:](#OTA_STATE_DOWNLOADING:) (const)
- [OTA_STATE_VERIFYING:](#OTA_STATE_VERIFYING:) (const)
- [OTA_STATE_APPLYING:](#OTA_STATE_APPLYING:) (const)
- [OTA_STATE_COMMITTED:](#OTA_STATE_COMMITTED:) (const)
- [OTA_STATE_ROLLBACK:](#OTA_STATE_ROLLBACK:) (const)
- [OTA_STATE_FAILED:](#OTA_STATE_FAILED:) (const)
- [ota_state_name](#ota_state_name) (function)
- [OTAManifest](#OTAManifest) (struct)
- [ota_manifest_new](#ota_manifest_new) (function)
- [ota_manifest_set_version](#ota_manifest_set_version) (function)
- [ota_manifest_set_signature](#ota_manifest_set_signature) (function)
- [ota_manifest_set_description](#ota_manifest_set_description) (function)
- [ota_verify_payload](#ota_verify_payload) (function)
- [ota_verify_manifest](#ota_verify_manifest) (function)
- [ota_check_compatibility](#ota_check_compatibility) (function)
- [OTASession](#OTASession) (struct)
- [ota_session_new](#ota_session_new) (function)
- [ota_session_start_download](#ota_session_start_download) (function)
- [ota_session_download_progress](#ota_session_download_progress) (function)
- [ota_session_download_percent](#ota_session_download_percent) (function)
- [ota_session_verify](#ota_session_verify) (function)
- [ota_session_apply](#ota_session_apply) (function)
- [ota_session_commit](#ota_session_commit) (function)
- [ota_session_rollback](#ota_session_rollback) (function)
- [ota_session_fail](#ota_session_fail) (function)
- [ota_session_can_retry](#ota_session_can_retry) (function)
- [ota_session_retry](#ota_session_retry) (function)
- [ota_session_state_name](#ota_session_state_name) (function)
- [DiffEntry](#DiffEntry) (struct)
- [ota_binary_diff](#ota_binary_diff) (function)
- [ota_diff_size](#ota_diff_size) (function)
- [ota_diff_region_count](#ota_diff_region_count) (function)

## ota_hash_hex

```fides
pub fn ota_hash_hex(data: Vec<i64>) -> str {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 37*

---

## OTA_STATE_IDLE:

```fides
pub const OTA_STATE_IDLE: i64 = 0;
```

*Defined at line 56*

---

## OTA_STATE_DOWNLOADING:

```fides
pub const OTA_STATE_DOWNLOADING: i64 = 1;
```

*Defined at line 57*

---

## OTA_STATE_VERIFYING:

```fides
pub const OTA_STATE_VERIFYING: i64 = 2;
```

*Defined at line 58*

---

## OTA_STATE_APPLYING:

```fides
pub const OTA_STATE_APPLYING: i64 = 3;
```

*Defined at line 59*

---

## OTA_STATE_COMMITTED:

```fides
pub const OTA_STATE_COMMITTED: i64 = 4;
```

*Defined at line 60*

---

## OTA_STATE_ROLLBACK:

```fides
pub const OTA_STATE_ROLLBACK: i64 = 5;
```

*Defined at line 61*

---

## OTA_STATE_FAILED:

```fides
pub const OTA_STATE_FAILED: i64 = 6;
```

*Defined at line 62*

---

## ota_state_name

Get OTA state name.

```fides
pub fn ota_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 64*

---

## OTAManifest

An OTA update manifest.

```fides
pub struct OTAManifest {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `str` |
| `filename` | `str` |
| `size` | `i64` |
| `checksum` | `str` |
| `signature` | `str` |
| `min_version` | `str` |
| `max_version` | `str` |
| `timestamp` | `i64` |
| `description` | `str` |

*Defined at line 96*

---

## ota_manifest_new

Create a new OTA manifest.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = ota_manifest_new("fw.bin", 5, "sha256:2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
assert(ota_verify_manifest(m));
assert(ota_verify_payload(m, "hello"));
assert(!ota_verify_payload(m, "hellO"));
assert(!ota_verify_manifest(ota_manifest_new("fw.bin", 5, "md5:x")));
```

```fides
pub fn ota_manifest_new(filename: str, size: i64, checksum: str) -> OTAManifest {
```

**Parameters:**

- `filename`: `str`
- `size`: `i64`
- `checksum`: `str`

**Returns:** `OTAManifest`

*Defined at line 122*

---

## ota_manifest_set_version

Set version information.

```fides
pub fn ota_manifest_set_version(manifest: &mut OTAManifest, version: str, min_ver: str, max_ver: str) -> void {
```

**Parameters:**

- `manifest`: `&mut OTAManifest`
- `version`: `str`
- `min_ver`: `str`
- `max_ver`: `str`

**Returns:** `void`

*Defined at line 129*

---

## ota_manifest_set_signature

Set signature.

```fides
pub fn ota_manifest_set_signature(manifest: &mut OTAManifest, signature: str) -> void {
```

**Parameters:**

- `manifest`: `&mut OTAManifest`
- `signature`: `str`

**Returns:** `void`

*Defined at line 136*

---

## ota_manifest_set_description

Set description.

```fides
pub fn ota_manifest_set_description(manifest: &mut OTAManifest, desc: str) -> void {
```

**Parameters:**

- `manifest`: `&mut OTAManifest`
- `desc`: `str`

**Returns:** `void`

*Defined at line 141*

---

## ota_verify_payload

Does the update match its manifest: the size, and the SHA-256 of its bytes?
This is integrity only: a manifest proves who made it only if it is signed.

```fides
pub fn ota_verify_payload(manifest: OTAManifest, payload: str) -> bool {
```

**Parameters:**

- `manifest`: `OTAManifest`
- `payload`: `str`

**Returns:** `bool`

*Defined at line 147*

---

## ota_verify_manifest

Are the manifest's fields well formed? This only looks at the manifest: it
says nothing about the update itself, which is ota_verify_payload's work.
The checksum must be "sha256:" + 64 lowercase hex digits — anything
shorter is a truncated or foreign hash, not a SHA-256.

```fides
pub fn ota_verify_manifest(manifest: OTAManifest) -> bool {
```

**Parameters:**

- `manifest`: `OTAManifest`

**Returns:** `bool`

*Defined at line 158*

---

## ota_check_compatibility

Check if the current version is compatible with the manifest.

```fides
pub fn ota_check_compatibility(manifest: OTAManifest, current_version: str) -> bool {
```

**Parameters:**

- `manifest`: `OTAManifest`
- `current_version`: `str`

**Returns:** `bool`

*Defined at line 190*

---

## OTASession

An OTA update session tracking the update process.

```fides
pub struct OTASession {
```

**Fields:**

| Name | Type |
|------|------|
| `manifest` | `OTAManifest` |
| `state` | `i64` |
| `bytes_downloaded` | `i64` |
| `bytes_verified` | `i64` |
| `error_msg` | `str` |
| `retry_count` | `i64` |
| `max_retries` | `i64` |

*Defined at line 209*

---

## ota_session_new

Create a new OTA session from a manifest.

```fides
pub fn ota_session_new(manifest: OTAManifest) -> OTASession {
```

**Parameters:**

- `manifest`: `OTAManifest`

**Returns:** `OTASession`

*Defined at line 220*

---

## ota_session_start_download

Start downloading the update.

```fides
pub fn ota_session_start_download(session: &mut OTASession) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`

**Returns:** `void`

*Defined at line 227*

---

## ota_session_download_progress

Update download progress.

```fides
pub fn ota_session_download_progress(session: &mut OTASession, bytes: i64) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`
- `bytes`: `i64`

**Returns:** `void`

*Defined at line 233*

---

## ota_session_download_percent

Get download progress as percentage (0-100).

```fides
pub fn ota_session_download_percent(session: OTASession) -> i64 {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `i64`

*Defined at line 241*

---

## ota_session_verify

Verify the downloaded binary against the manifest checksum.
binary_data: the downloaded bytes.
Returns true if verification succeeds.

```fides
pub fn ota_session_verify(session: &mut OTASession, binary_data: Vec<i64>) -> bool {
```

**Parameters:**

- `session`: `&mut OTASession`
- `binary_data`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 251*

---

## ota_session_apply

Apply the update (transition to applying state).

```fides
pub fn ota_session_apply(session: &mut OTASession) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`

**Returns:** `void`

*Defined at line 272*

---

## ota_session_commit

Commit the update (mark as committed, can't rollback).

```fides
pub fn ota_session_commit(session: &mut OTASession) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`

**Returns:** `void`

*Defined at line 277*

---

## ota_session_rollback

Rollback the update (revert to previous version).

```fides
pub fn ota_session_rollback(session: &mut OTASession) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`

**Returns:** `void`

*Defined at line 282*

---

## ota_session_fail

Mark the session as failed.

```fides
pub fn ota_session_fail(session: &mut OTASession, error_msg: str) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`
- `error_msg`: `str`

**Returns:** `void`

*Defined at line 287*

---

## ota_session_can_retry

Check if the session can be retried.

```fides
pub fn ota_session_can_retry(session: OTASession) -> bool {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `bool`

*Defined at line 293*

---

## ota_session_retry

Retry the download.

```fides
pub fn ota_session_retry(session: &mut OTASession) -> void {
```

**Parameters:**

- `session`: `&mut OTASession`

**Returns:** `void`

*Defined at line 298*

---

## ota_session_state_name

Get the session state name.

```fides
pub fn ota_session_state_name(session: OTASession) -> str {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `str`

*Defined at line 307*

---

## DiffEntry

A binary diff entry.

```fides
pub struct DiffEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `length` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 318*

---

## ota_binary_diff

Create a binary diff between old and new data.
Returns a Vec of DiffEntry (flattened as alternating offset/length/data).

```fides
pub fn ota_binary_diff(old_data: Vec<i64>, new_data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `old_data`: `Vec<i64>`
- `new_data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 326*

---

## ota_diff_size

Compute the size of a binary diff (number of changed bytes).

```fides
pub fn ota_diff_size(diff: Vec<i64>) -> i64 {
```

**Parameters:**

- `diff`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 365*

---

## ota_diff_region_count

Get the number of changed regions in a diff.

```fides
pub fn ota_diff_region_count(diff: Vec<i64>) -> i64 {
```

**Parameters:**

- `diff`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 376*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
