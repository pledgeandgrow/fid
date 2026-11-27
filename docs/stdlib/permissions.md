[All modules](index.md)

# permissions -- Fides Documentation

## Contents

- [PERM_CAMERA:](#PERM_CAMERA:) (const)
- [PERM_LOCATION_FINE:](#PERM_LOCATION_FINE:) (const)
- [PERM_LOCATION_COARSE:](#PERM_LOCATION_COARSE:) (const)
- [PERM_LOCATION_BACKGROUND:](#PERM_LOCATION_BACKGROUND:) (const)
- [PERM_MICROPHONE:](#PERM_MICROPHONE:) (const)
- [PERM_NOTIFICATIONS:](#PERM_NOTIFICATIONS:) (const)
- [PERM_BLUETOOTH:](#PERM_BLUETOOTH:) (const)
- [PERM_PHOTOS:](#PERM_PHOTOS:) (const)
- [PERM_COUNT:](#PERM_COUNT:) (const)
- [PERM_NOT_DETERMINED:](#PERM_NOT_DETERMINED:) (const)
- [PERM_GRANTED:](#PERM_GRANTED:) (const)
- [PERM_DENIED:](#PERM_DENIED:) (const)
- [PERM_PERMANENTLY_DENIED:](#PERM_PERMANENTLY_DENIED:) (const)
- [PERM_RESTRICTED:](#PERM_RESTRICTED:) (const)
- [PERM_LIMITED:](#PERM_LIMITED:) (const)
- [PERM_PROVISIONAL:](#PERM_PROVISIONAL:) (const)
- [PERM_ANDROID:](#PERM_ANDROID:) (const)
- [PERM_IOS:](#PERM_IOS:) (const)
- [perm_name](#perm_name) (function)
- [perm_status_name](#perm_status_name) (function)
- [perm_usable](#perm_usable) (function)
- [perm_android_names](#perm_android_names) (function)
- [perm_android_is_runtime](#perm_android_is_runtime) (function)
- [perm_android_manifest](#perm_android_manifest) (function)
- [perm_android_status](#perm_android_status) (function)
- [perm_ios_plist_key](#perm_ios_plist_key) (function)
- [perm_ios_av_status](#perm_ios_av_status) (function)
- [perm_ios_location_status](#perm_ios_location_status) (function)
- [perm_ios_photos_status](#perm_ios_photos_status) (function)
- [perm_ios_notification_status](#perm_ios_notification_status) (function)
- [perm_ios_bluetooth_status](#perm_ios_bluetooth_status) (function)
- [PlistEntries](#PlistEntries) (struct)
- [perm_ios_plist](#perm_ios_plist) (function)
- [PermCall](#PermCall) (struct)
- [perm_android_call](#perm_android_call) (function)
- [perm_ios_call](#perm_ios_call) (function)
- [PERM_STEP_DONE:](#PERM_STEP_DONE:) (const)
- [PERM_STEP_RATIONALE:](#PERM_STEP_RATIONALE:) (const)
- [PERM_STEP_REQUEST:](#PERM_STEP_REQUEST:) (const)
- [PERM_STEP_SETTINGS:](#PERM_STEP_SETTINGS:) (const)
- [PERM_STEP_BLOCKED:](#PERM_STEP_BLOCKED:) (const)
- [perm_step_name](#perm_step_name) (function)
- [PermFlow](#PermFlow) (struct)
- [perm_flow_new](#perm_flow_new) (function)
- [perm_flow_next](#perm_flow_next) (function)
- [perm_flow_rationale_shown](#perm_flow_rationale_shown) (function)
- [perm_flow_result](#perm_flow_result) (function)
- [perm_request_order](#perm_request_order) (function)

## PERM_CAMERA:

```fides
pub const PERM_CAMERA: i64 = 0;
```

*Defined at line 40*

---

## PERM_LOCATION_FINE:

```fides
pub const PERM_LOCATION_FINE: i64 = 1;
```

*Defined at line 41*

---

## PERM_LOCATION_COARSE:

```fides
pub const PERM_LOCATION_COARSE: i64 = 2;
```

*Defined at line 42*

---

## PERM_LOCATION_BACKGROUND:

```fides
pub const PERM_LOCATION_BACKGROUND: i64 = 3;
```

*Defined at line 43*

---

## PERM_MICROPHONE:

```fides
pub const PERM_MICROPHONE: i64 = 4;
```

*Defined at line 44*

---

## PERM_NOTIFICATIONS:

```fides
pub const PERM_NOTIFICATIONS: i64 = 5;
```

*Defined at line 45*

---

## PERM_BLUETOOTH:

```fides
pub const PERM_BLUETOOTH: i64 = 6;
```

*Defined at line 46*

---

## PERM_PHOTOS:

```fides
pub const PERM_PHOTOS: i64 = 7;
```

*Defined at line 47*

---

## PERM_COUNT:

```fides
pub const PERM_COUNT: i64 = 8;
```

*Defined at line 48*

---

## PERM_NOT_DETERMINED:

```fides
pub const PERM_NOT_DETERMINED: i64 = 0;
```

*Defined at line 51*

---

## PERM_GRANTED:

```fides
pub const PERM_GRANTED: i64 = 1;
```

*Defined at line 52*

---

## PERM_DENIED:

```fides
pub const PERM_DENIED: i64 = 2;  // may be asked again
```

*Defined at line 53*

---

## PERM_PERMANENTLY_DENIED:

```fides
pub const PERM_PERMANENTLY_DENIED: i64 = 3;  // only the settings app can change it
```

*Defined at line 54*

---

## PERM_RESTRICTED:

```fides
pub const PERM_RESTRICTED: i64 = 4;  // parental controls / MDM
```

*Defined at line 55*

---

## PERM_LIMITED:

```fides
pub const PERM_LIMITED: i64 = 5;  // approximate location, selected photos
```

*Defined at line 56*

---

## PERM_PROVISIONAL:

```fides
pub const PERM_PROVISIONAL: i64 = 6;  // iOS quiet notifications
```

*Defined at line 57*

---

## PERM_ANDROID:

```fides
pub const PERM_ANDROID: i64 = 0;
```

*Defined at line 59*

---

## PERM_IOS:

```fides
pub const PERM_IOS: i64 = 1;
```

*Defined at line 60*

---

## perm_name

```fides
pub fn perm_name(p: i64) -> str {
```

**Parameters:**

- `p`: `i64`

**Returns:** `str`

*Defined at line 62*

---

## perm_status_name

```fides
pub fn perm_status_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 70*

---

## perm_usable

May the app use the feature (granted, limited or provisional)?

```fides
pub fn perm_usable(status: i64) -> bool {
```

**Parameters:**

- `status`: `i64`

**Returns:** `bool`

*Defined at line 79*

---

## perm_android_names

The manifest permissions to declare and request for `p` on API level
`api` (the names the request dialog takes).

```fides
pub fn perm_android_names(p: i64, api: i64) -> Vec<str> {
```

**Parameters:**

- `p`: `i64`
- `api`: `i64`

**Returns:** `Vec<str>`

*Defined at line 87*

---

## perm_android_is_runtime

Is `p` asked for at run time on API `api` (a "dangerous" permission)?
Below API 23 everything is granted at install; notifications need no
request below 33; background location is part of the location
request below 29.

```fides
pub fn perm_android_is_runtime(p: i64, api: i64) -> bool {
```

**Parameters:**

- `p`: `i64`
- `api`: `i64`

**Returns:** `bool`

*Defined at line 132*

---

## perm_android_manifest

<uses-permission> lines for the manifest: every permission the set
needs on any API from `min_api` up, legacy ones capped with
android:maxSdkVersion.

```fides
pub fn perm_android_manifest(perms: Vec<i64>, min_api: i64) -> str {
```

**Parameters:**

- `perms`: `Vec<i64>`
- `min_api`: `i64`

**Returns:** `str`

*Defined at line 142*

---

## perm_android_status

The status from Android results: `granted` per requested name
(PackageManager.PERMISSION_GRANTED is 0, DENIED -1) for the names of
perm_android_names. Fine location with only coarse granted is LIMITED;
photos with only the user-selected subset is LIMITED.

```fides
pub fn perm_android_status(p: i64, api: i64, results: Vec<i64>, rationale: bool, denials: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `api`: `i64`
- `results`: `Vec<i64>`
- `rationale`: `bool`
- `denials`: `i64`

**Returns:** `i64`

*Defined at line 199*

---

## perm_ios_plist_key

The Info.plist usage description key of `p` ("" when none is needed:
notifications). Fine and coarse location share the when-in-use key.

```fides
pub fn perm_ios_plist_key(p: i64) -> str {
```

**Parameters:**

- `p`: `i64`

**Returns:** `str`

*Defined at line 249*

---

## perm_ios_av_status

AVAuthorizationStatus (camera, microphone) -> status.

```fides
pub fn perm_ios_av_status(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 272*

---

## perm_ios_location_status

CLAuthorizationStatus + CLAccuracyAuthorization (0 full, 1 reduced)
-> the status of permission `p` (a location permission).

```fides
pub fn perm_ios_location_status(p: i64, code: i64, accuracy: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `code`: `i64`
- `accuracy`: `i64`

**Returns:** `i64`

*Defined at line 290*

---

## perm_ios_photos_status

PHAuthorizationStatus -> status (4 limited).

```fides
pub fn perm_ios_photos_status(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 314*

---

## perm_ios_notification_status

UNAuthorizationStatus -> status (3 provisional, 4 ephemeral).

```fides
pub fn perm_ios_notification_status(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 322*

---

## perm_ios_bluetooth_status

CBManagerAuthorization -> status.

```fides
pub fn perm_ios_bluetooth_status(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 342*

---

## PlistEntries

Info.plist <key>/<string> entries for the permissions, each with its
description; `missing` lists the keys without a description.

```fides
pub struct PlistEntries {
```

**Fields:**

| Name | Type |
|------|------|
| `xml` | `str` |
| `missing` | `Vec<str>` |

*Defined at line 348*

---

## perm_ios_plist

```fides
pub fn perm_ios_plist(perms: Vec<i64>, descriptions: Vec<str>) -> PlistEntries {
```

**Parameters:**

- `perms`: `Vec<i64>`
- `descriptions`: `Vec<str>`

**Returns:** `PlistEntries`

*Defined at line 353*

---

## PermCall

```fides
pub struct PermCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str,  // JNI signature (Android) or the method types (iOS)` |

*Defined at line 405*

---

## perm_android_call

The JNI calls of the Android flow: "check", "rationale", "request".

```fides
pub fn perm_android_call(what: str) -> PermCall {
```

**Parameters:**

- `what`: `str`

**Returns:** `PermCall`

*Defined at line 412*

---

## perm_ios_call

The Objective-C request of `p` on iOS: receiver class and selector.

```fides
pub fn perm_ios_call(p: i64) -> PermCall {
```

**Parameters:**

- `p`: `i64`

**Returns:** `PermCall`

*Defined at line 429*

---

## PERM_STEP_DONE:

```fides
pub const PERM_STEP_DONE: i64 = 0;
```

*Defined at line 463*

---

## PERM_STEP_RATIONALE:

```fides
pub const PERM_STEP_RATIONALE: i64 = 1;
```

*Defined at line 464*

---

## PERM_STEP_REQUEST:

```fides
pub const PERM_STEP_REQUEST: i64 = 2;
```

*Defined at line 465*

---

## PERM_STEP_SETTINGS:

```fides
pub const PERM_STEP_SETTINGS: i64 = 3;
```

*Defined at line 466*

---

## PERM_STEP_BLOCKED:

```fides
pub const PERM_STEP_BLOCKED: i64 = 4;  // restricted or a prerequisite missing
```

*Defined at line 467*

---

## perm_step_name

```fides
pub fn perm_step_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 469*

---

## PermFlow

One permission being obtained.

```fides
pub struct PermFlow {
```

**Fields:**

| Name | Type |
|------|------|
| `perm` | `i64` |
| `platform` | `i64` |
| `api` | `i64,  // Android API level (ignored on iOS)` |
| `status` | `i64` |
| `denials` | `i64` |
| `requests` | `i64` |
| `rationale_shown` | `bool` |
| `prerequisite` | `i64,  // status of the foreground location (background only)` |

*Defined at line 478*

---

## perm_flow_new

```fides
pub fn perm_flow_new(perm: i64, platform: i64, api: i64) -> PermFlow {
```

**Parameters:**

- `perm`: `i64`
- `platform`: `i64`
- `api`: `i64`

**Returns:** `PermFlow`

*Defined at line 489*

---

## perm_flow_next

What to do next. `rationale` is what shouldShowRequestPermissionRationale
says now (Android; ignored on iOS).

```fides
pub fn perm_flow_next(f: &PermFlow, rationale: bool) -> i64 {
```

**Parameters:**

- `f`: `&PermFlow`
- `rationale`: `bool`

**Returns:** `i64`

*Defined at line 497*

---

## perm_flow_rationale_shown

The rationale was shown to the user.

```fides
pub fn perm_flow_rationale_shown(f: &mut PermFlow) {
```

**Parameters:**

- `f`: `&mut PermFlow`

*Defined at line 527*

---

## perm_flow_result

The system dialog answered: on Android `results` per name and the
rationale flag read after the answer; on iOS `results` holds the one
framework status code (already mapped with perm_ios_*: pass the status).

```fides
pub fn perm_flow_result(f: &mut PermFlow, results: Vec<i64>, rationale_after: bool) {
```

**Parameters:**

- `f`: `&mut PermFlow`
- `results`: `Vec<i64>`
- `rationale_after`: `bool`

*Defined at line 534*

---

## perm_request_order

The order to ask for a set: foreground location before background,
each once.

```fides
pub fn perm_request_order(perms: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `perms`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 565*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
