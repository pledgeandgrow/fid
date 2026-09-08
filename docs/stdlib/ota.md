# ota — Fides Documentation

## Contents

- [OTA_STATE_IDLE:](#ota_state_idle:) (const)
- [OTA_STATE_DOWNLOADING:](#ota_state_downloading:) (const)
- [OTA_STATE_VERIFYING:](#ota_state_verifying:) (const)
- [OTA_STATE_APPLYING:](#ota_state_applying:) (const)
- [OTA_STATE_COMMITTED:](#ota_state_committed:) (const)
- [OTA_STATE_ROLLBACK:](#ota_state_rollback:) (const)
- [OTA_STATE_FAILED:](#ota_state_failed:) (const)
- [ota_state_name](#ota_state_name) (function)
- [OTAManifest](#otamanifest) (struct)
- [ota_manifest_new](#ota_manifest_new) (function)
- [ota_manifest_set_version](#ota_manifest_set_version) (function)
- [ota_manifest_set_signature](#ota_manifest_set_signature) (function)
- [ota_manifest_set_description](#ota_manifest_set_description) (function)
- [ota_verify_manifest](#ota_verify_manifest) (function)
- [ota_check_compatibility](#ota_check_compatibility) (function)
- [OTASession](#otasession) (struct)
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
- [DiffEntry](#diffentry) (struct)
- [ota_binary_diff](#ota_binary_diff) (function)
- [ota_diff_size](#ota_diff_size) (function)
- [ota_diff_region_count](#ota_diff_region_count) (function)

## OTA_STATE_IDLE:

```fides
pub const OTA_STATE_IDLE: i64        = 0;
```

*Defined at line 53*

---

## OTA_STATE_DOWNLOADING:

```fides
pub const OTA_STATE_DOWNLOADING: i64 = 1;
```

*Defined at line 54*

---

## OTA_STATE_VERIFYING:

```fides
pub const OTA_STATE_VERIFYING: i64   = 2;
```

*Defined at line 55*

---

## OTA_STATE_APPLYING:

```fides
pub const OTA_STATE_APPLYING: i64    = 3;
```

*Defined at line 56*

---

## OTA_STATE_COMMITTED:

```fides
pub const OTA_STATE_COMMITTED: i64   = 4;
```

*Defined at line 57*

---

## OTA_STATE_ROLLBACK:

```fides
pub const OTA_STATE_ROLLBACK: i64    = 5;
```

*Defined at line 58*

---

## OTA_STATE_FAILED:

```fides
pub const OTA_STATE_FAILED: i64      = 6;
```

*Defined at line 59*

---

## ota_state_name

Get OTA state name.

```fides
pub fn ota_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 62*

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
| `size` | `i64,             // in bytes` |
| `checksum` | `str,         // "sha256:hexstring"` |
| `signature` | `str,        // hex signature (empty if unsigned)` |
| `min_version` | `str,      // minimum current version required` |
| `max_version` | `str,      // maximum current version allowed` |
| `timestamp` | `i64,        // manifest creation time` |
| `description` | `str` |

*Defined at line 80*

---

## ota_manifest_new

Create a new OTA manifest.

```fides
pub fn ota_manifest_new(filename: str, size: i64, checksum: str) -> OTAManifest {
```

**Parameters:**

- `filename`: `str`
- `size`: `i64`
- `checksum`: `str`

**Returns:** `OTAManifest`

*Defined at line 93*

---

## ota_manifest_set_version

Set version information.

```fides
pub fn ota_manifest_set_version(manifest: OTAManifest, version: str,
```

**Parameters:**

- `manifest`: `OTAManifest`
- `version`: `str`

*Defined at line 108*

---

## ota_manifest_set_signature

Set signature.

```fides
pub fn ota_manifest_set_signature(manifest: OTAManifest, signature: str) -> void {
```

**Parameters:**

- `manifest`: `OTAManifest`
- `signature`: `str`

**Returns:** `void`

*Defined at line 116*

---

## ota_manifest_set_description

Set description.

```fides
pub fn ota_manifest_set_description(manifest: OTAManifest, desc: str) -> void {
```

**Parameters:**

- `manifest`: `OTAManifest`
- `desc`: `str`

**Returns:** `void`

*Defined at line 121*

---

## ota_verify_manifest

Verify manifest fields are valid.

```fides
pub fn ota_verify_manifest(manifest: OTAManifest) -> bool {
```

**Parameters:**

- `manifest`: `OTAManifest`

**Returns:** `bool`

*Defined at line 126*

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

*Defined at line 142*

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

*Defined at line 161*

---

## ota_session_new

Create a new OTA session from a manifest.

```fides
pub fn ota_session_new(manifest: OTAManifest) -> OTASession {
```

**Parameters:**

- `manifest`: `OTAManifest`

**Returns:** `OTASession`

*Defined at line 172*

---

## ota_session_start_download

Start downloading the update.

```fides
pub fn ota_session_start_download(session: OTASession) -> void {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `void`

*Defined at line 185*

---

## ota_session_download_progress

Update download progress.

```fides
pub fn ota_session_download_progress(session: OTASession, bytes: i64) -> void {
```

**Parameters:**

- `session`: `OTASession`
- `bytes`: `i64`

**Returns:** `void`

*Defined at line 191*

---

## ota_session_download_percent

Get download progress as percentage (0-100).

```fides
pub fn ota_session_download_percent(session: OTASession) -> i64 {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `i64`

*Defined at line 199*

---

## ota_session_verify

Verify the downloaded binary against the manifest checksum.
binary_data: the downloaded bytes.
Returns true if verification succeeds.

```fides
pub fn ota_session_verify(session: OTASession, binary_data: Vec) -> bool {
```

**Parameters:**

- `session`: `OTASession`
- `binary_data`: `Vec`

**Returns:** `bool`

*Defined at line 209*

---

## ota_session_apply

Apply the update (transition to applying state).

```fides
pub fn ota_session_apply(session: OTASession) -> void {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `void`

*Defined at line 230*

---

## ota_session_commit

Commit the update (mark as committed, can't rollback).

```fides
pub fn ota_session_commit(session: OTASession) -> void {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `void`

*Defined at line 235*

---

## ota_session_rollback

Rollback the update (revert to previous version).

```fides
pub fn ota_session_rollback(session: OTASession) -> void {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `void`

*Defined at line 240*

---

## ota_session_fail

Mark the session as failed.

```fides
pub fn ota_session_fail(session: OTASession, error_msg: str) -> void {
```

**Parameters:**

- `session`: `OTASession`
- `error_msg`: `str`

**Returns:** `void`

*Defined at line 245*

---

## ota_session_can_retry

Check if the session can be retried.

```fides
pub fn ota_session_can_retry(session: OTASession) -> bool {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `bool`

*Defined at line 251*

---

## ota_session_retry

Retry the download.

```fides
pub fn ota_session_retry(session: OTASession) -> void {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `void`

*Defined at line 256*

---

## ota_session_state_name

Get the session state name.

```fides
pub fn ota_session_state_name(session: OTASession) -> str {
```

**Parameters:**

- `session`: `OTASession`

**Returns:** `str`

*Defined at line 265*

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
| `data` | `Vec` |

*Defined at line 276*

---

## ota_binary_diff

Create a binary diff between old and new data.
Returns a Vec of DiffEntry (flattened as alternating offset/length/data).

```fides
pub fn ota_binary_diff(old_data: Vec, new_data: Vec) -> Vec {
```

**Parameters:**

- `old_data`: `Vec`
- `new_data`: `Vec`

**Returns:** `Vec`

*Defined at line 284*

---

## ota_diff_size

Compute the size of a binary diff (number of changed bytes).

```fides
pub fn ota_diff_size(diff: Vec) -> i64 {
```

**Parameters:**

- `diff`: `Vec`

**Returns:** `i64`

*Defined at line 323*

---

## ota_diff_region_count

Get the number of changed regions in a diff.

```fides
pub fn ota_diff_region_count(diff: Vec) -> i64 {
```

**Parameters:**

- `diff`: `Vec`

**Returns:** `i64`

*Defined at line 334*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
