[All modules](index.md)

# push -- Fides Documentation

## Contents

- [APNS_MAX_PAYLOAD:](#APNS_MAX_PAYLOAD:) (const)
- [APNS_MAX_PAYLOAD_VOIP:](#APNS_MAX_PAYLOAD_VOIP:) (const)
- [APNS_HOST:](#APNS_HOST:) (const)
- [APNS_HOST_SANDBOX:](#APNS_HOST_SANDBOX:) (const)
- [APNS_JWT_REFRESH_SECS:](#APNS_JWT_REFRESH_SECS:) (const)
- [APNS_JWT_MIN_REFRESH_SECS:](#APNS_JWT_MIN_REFRESH_SECS:) (const)
- [push_b64url](#push_b64url) (function)
- [push_b64url_decode](#push_b64url_decode) (function)
- [ApnsPayload](#ApnsPayload) (struct)
- [apns_payload_new](#apns_payload_new) (function)
- [apns_payload_custom](#apns_payload_custom) (function)
- [apns_aps_json](#apns_aps_json) (function)
- [apns_payload_json](#apns_payload_json) (function)
- [apns_payload_check](#apns_payload_check) (function)
- [apns_payload_fit](#apns_payload_fit) (function)
- [ApnsRequest](#ApnsRequest) (struct)
- [apns_request](#apns_request) (function)
- [apns_token_valid](#apns_token_valid) (function)
- [apns_token_hex](#apns_token_hex) (function)
- [fcm_token_valid](#fcm_token_valid) (function)
- [FcmMessage](#FcmMessage) (struct)
- [fcm_message_new](#fcm_message_new) (function)
- [fcm_data_key_ok](#fcm_data_key_ok) (function)
- [fcm_add_data](#fcm_add_data) (function)
- [fcm_send_url](#fcm_send_url) (function)
- [fcm_message_json](#fcm_message_json) (function)
- [apns_jwt_signing_input](#apns_jwt_signing_input) (function)
- [push_der_to_raw](#push_der_to_raw) (function)
- [apns_provider_token](#apns_provider_token) (function)
- [apns_jwt_should_refresh](#apns_jwt_should_refresh) (function)
- [PUSH_ACTION_FOREGROUND:](#PUSH_ACTION_FOREGROUND:) (const)
- [PUSH_ACTION_DESTRUCTIVE:](#PUSH_ACTION_DESTRUCTIVE:) (const)
- [PUSH_ACTION_AUTH:](#PUSH_ACTION_AUTH:) (const)
- [PushAction](#PushAction) (struct)
- [PushCategory](#PushCategory) (struct)
- [push_action_un_options](#push_action_un_options) (function)
- [push_categories_check](#push_categories_check) (function)
- [push_response_action](#push_response_action) (function)
- [PushReceived](#PushReceived) (struct)
- [push_parse_received](#push_parse_received) (function)

## APNS_MAX_PAYLOAD:

```fides
pub const APNS_MAX_PAYLOAD: i64 = 4096;
```

*Defined at line 45*

---

## APNS_MAX_PAYLOAD_VOIP:

```fides
pub const APNS_MAX_PAYLOAD_VOIP: i64 = 5120;
```

*Defined at line 46*

---

## APNS_HOST:

```fides
pub const APNS_HOST: str = "api.push.apple.com";
```

*Defined at line 47*

---

## APNS_HOST_SANDBOX:

```fides
pub const APNS_HOST_SANDBOX: str = "api.sandbox.push.apple.com";
```

*Defined at line 48*

---

## APNS_JWT_REFRESH_SECS:

```fides
pub const APNS_JWT_REFRESH_SECS: i64 = 3000;  // 50 minutes
```

*Defined at line 49*

---

## APNS_JWT_MIN_REFRESH_SECS:

```fides
pub const APNS_JWT_MIN_REFRESH_SECS: i64 = 1200;  // 20 minutes
```

*Defined at line 50*

---

## push_b64url

base64url without padding (RFC 7515 §2).

```fides
pub fn push_b64url(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 55*

---

## push_b64url_decode

Decode base64url (padding optional).

```fides
pub fn push_b64url_decode(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 74*

---

## ApnsPayload

```fides
pub struct ApnsPayload {
```

**Fields:**

| Name | Type |
|------|------|
| `title` | `str` |
| `subtitle` | `str` |
| `body` | `str` |
| `loc_key` | `str,  // title-loc-key is not used; body loc-key` |
| `loc_args` | `Vec<str>` |
| `badge` | `i64,  // -1: leave the badge alone` |
| `sound` | `str,  // "" none, "default", or a file name` |
| `content_available` | `bool` |
| `mutable_content` | `bool` |
| `category` | `str` |
| `thread_id` | `str` |
| `interruption_level` | `str,  // passive, active, time-sensitive, critical` |
| `relevance` | `i64,  // relevance-score in percent, -1 none` |
| `custom_keys` | `Vec<str>` |
| `custom_vals` | `Vec<str>,  // raw JSON values` |

*Defined at line 96*

---

## apns_payload_new

```fides
pub fn apns_payload_new() -> ApnsPayload {
```

**Returns:** `ApnsPayload`

*Defined at line 114*

---

## apns_payload_custom

Add a custom top-level key with a raw JSON value ("aps" is refused).

```fides
pub fn apns_payload_custom(p: &mut ApnsPayload, key: str, raw_json: str) -> bool {
```

**Parameters:**

- `p`: `&mut ApnsPayload`
- `key`: `str`
- `raw_json`: `str`

**Returns:** `bool`

*Defined at line 121*

---

## apns_aps_json

The aps dictionary alone.

```fides
pub fn apns_aps_json(p: &ApnsPayload) -> str {
```

**Parameters:**

- `p`: `&ApnsPayload`

**Returns:** `str`

*Defined at line 172*

---

## apns_payload_json

The full payload: {"aps":{...}, custom...}.

```fides
pub fn apns_payload_json(p: &ApnsPayload) -> str {
```

**Parameters:**

- `p`: `&ApnsPayload`

**Returns:** `str`

*Defined at line 219*

---

## apns_payload_check

Problems that make APNs refuse or mis-deliver the payload ("" none).

```fides
pub fn apns_payload_check(p: &ApnsPayload, voip: bool) -> str {
```

**Parameters:**

- `p`: `&ApnsPayload`
- `voip`: `bool`

**Returns:** `str`

*Defined at line 230*

---

## apns_payload_fit

Shorten the body (UTF-8 safe, ending in "...") until the payload fits
`max` bytes. Returns false when even an empty body does not fit.

```fides
pub fn apns_payload_fit(p: &mut ApnsPayload, max: i64) -> bool {
```

**Parameters:**

- `p`: `&mut ApnsPayload`
- `max`: `i64`

**Returns:** `bool`

*Defined at line 264*

---

## ApnsRequest

```fides
pub struct ApnsRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `path` | `str` |
| `header_names` | `Vec<str>` |
| `header_vals` | `Vec<str>` |
| `body` | `str` |

*Defined at line 289*

---

## apns_request

The HTTP/2 request for one device: path, headers, body. `push_type`
is alert, background, voip, complication, fileprovider, mdm,
location or liveactivity; `expiration` 0 means deliver once or drop.

```fides
pub fn apns_request(token_hex: str, topic: str, push_type: str, p: &ApnsPayload, expiration: i64, collapse_id: str, jwt: str) -> ApnsRequest {
```

**Parameters:**

- `token_hex`: `str`
- `topic`: `str`
- `push_type`: `str`
- `p`: `&ApnsPayload`
- `expiration`: `i64`
- `collapse_id`: `str`
- `jwt`: `str`

**Returns:** `ApnsRequest`

*Defined at line 301*

---

## apns_token_valid

An APNs device token: an even number of hex digits, 32 bytes today
(up to 100 bytes accepted, as Apple asks providers to).

```fides
pub fn apns_token_valid(hex: str) -> bool {
```

**Parameters:**

- `hex`: `str`

**Returns:** `bool`

*Defined at line 371*

---

## apns_token_hex

The token string of the bytes iOS hands to
application:didRegisterForRemoteNotificationsWithDeviceToken:.

```fides
pub fn apns_token_hex(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 381*

---

## fcm_token_valid

An FCM registration token: [A-Za-z0-9_:-] and at least 100 characters
(they are about 150 to 165 long).

```fides
pub fn fcm_token_valid(t: str) -> bool {
```

**Parameters:**

- `t`: `str`

**Returns:** `bool`

*Defined at line 387*

---

## FcmMessage

```fides
pub struct FcmMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `token` | `str` |
| `topic` | `str` |
| `condition` | `str` |
| `title` | `str` |
| `body` | `str` |
| `image` | `str` |
| `data_keys` | `Vec<str>` |
| `data_vals` | `Vec<str>` |
| `android_priority` | `str,  // "" , "normal", "high"` |
| `ttl_secs` | `i64,  // -1: none` |
| `collapse_key` | `str` |
| `channel_id` | `str` |
| `apns` | `ApnsPayload,  // apns override; used when it has content` |
| `apns_priority` | `str` |

*Defined at line 405*

---

## fcm_message_new

```fides
pub fn fcm_message_new() -> FcmMessage {
```

**Returns:** `FcmMessage`

*Defined at line 422*

---

## fcm_data_key_ok

Is `key` allowed in FCM data (not "from", "notification",
"message_type", or a "google." / "gcm." prefix)?

```fides
pub fn fcm_data_key_ok(key: str) -> bool {
```

**Parameters:**

- `key`: `str`

**Returns:** `bool`

*Defined at line 430*

---

## fcm_add_data

```fides
pub fn fcm_add_data(m: &mut FcmMessage, key: str, value: str) -> bool {
```

**Parameters:**

- `m`: `&mut FcmMessage`
- `key`: `str`
- `value`: `str`

**Returns:** `bool`

*Defined at line 443*

---

## fcm_send_url

The send URL of a Firebase project.

```fides
pub fn fcm_send_url(project_id: str) -> str {
```

**Parameters:**

- `project_id`: `str`

**Returns:** `str`

*Defined at line 453*

---

## fcm_message_json

The request body {"message":{...}}; "" when the target is not exactly
one of token, topic, condition.

```fides
pub fn fcm_message_json(m: &FcmMessage) -> str {
```

**Parameters:**

- `m`: `&FcmMessage`

**Returns:** `str`

*Defined at line 459*

---

## apns_jwt_signing_input

The signing input: base64url(header) "." base64url(claims).

```fides
pub fn apns_jwt_signing_input(key_id: str, team_id: str, iat: i64) -> str {
```

**Parameters:**

- `key_id`: `str`
- `team_id`: `str`
- `iat`: `i64`

**Returns:** `str`

*Defined at line 536*

---

## push_der_to_raw

A raw 64-byte r||s from a DER ECDSA-Sig-Value (SEQUENCE of two
INTEGERs); empty when malformed. A 64-byte input is returned as is.

```fides
pub fn push_der_to_raw(sig: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `sig`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 544*

---

## apns_provider_token

The provider token: signing input "." base64url(r||s); "" when the
signer fails.

```fides
pub fn apns_provider_token(key_id: str, team_id: str, iat: i64, sign: fn(Vec<i64>) -> Vec<i64>) -> str {
```

**Parameters:**

- `key_id`: `str`
- `team_id`: `str`
- `iat`: `i64`
- `sign`: `fn(Vec<i64>) -> Vec<i64>`

**Returns:** `str`

*Defined at line 594*

---

## apns_jwt_should_refresh

Should a token issued at `iat` be replaced at `now` (seconds)?

```fides
pub fn apns_jwt_should_refresh(iat: i64, now: i64) -> bool {
```

**Parameters:**

- `iat`: `i64`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 604*

---

## PUSH_ACTION_FOREGROUND:

```fides
pub const PUSH_ACTION_FOREGROUND: i64 = 1;  // UNNotificationActionOptionForeground (4)
```

*Defined at line 610*

---

## PUSH_ACTION_DESTRUCTIVE:

```fides
pub const PUSH_ACTION_DESTRUCTIVE: i64 = 2;  // ...Destructive (2)
```

*Defined at line 611*

---

## PUSH_ACTION_AUTH:

```fides
pub const PUSH_ACTION_AUTH: i64 = 4;  // ...AuthenticationRequired (1)
```

*Defined at line 612*

---

## PushAction

```fides
pub struct PushAction {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `title` | `str` |
| `options` | `i64` |
| `text_input` | `bool` |

*Defined at line 614*

---

## PushCategory

```fides
pub struct PushCategory {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `actions` | `Vec<PushAction>` |

*Defined at line 621*

---

## push_action_un_options

UNNotificationActionOptions bits of PUSH_ACTION_* flags.

```fides
pub fn push_action_un_options(opts: i64) -> i64 {
```

**Parameters:**

- `opts`: `i64`

**Returns:** `i64`

*Defined at line 627*

---

## push_categories_check

Problems of a category set: empty or duplicate identifiers, more than
10 actions (iOS shows at most 10), reserved identifiers.

```fides
pub fn push_categories_check(cats: Vec<PushCategory>) -> Vec<str> {
```

**Parameters:**

- `cats`: `Vec<PushCategory>`

**Returns:** `Vec<str>`

*Defined at line 643*

---

## push_response_action

What a response's actionIdentifier means: "open" (default action),
"dismiss", or the custom action identifier.

```fides
pub fn push_response_action(action_identifier: str) -> str {
```

**Parameters:**

- `action_identifier`: `str`

**Returns:** `str`

*Defined at line 686*

---

## PushReceived

```fides
pub struct PushReceived {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `title` | `str` |
| `body` | `str` |
| `badge` | `i64` |
| `category` | `str` |
| `silent` | `bool,  // content-available without an alert` |
| `data_keys` | `Vec<str>,  // custom keys (APNs) or data keys (FCM)` |
| `data_vals` | `Vec<str>,  // raw JSON values` |

*Defined at line 698*

---

## push_parse_received

Parse an APNs userInfo or an FCM data message as JSON text.

```fides
pub fn push_parse_received(text: str) -> PushReceived {
```

**Parameters:**

- `text`: `str`

**Returns:** `PushReceived`

*Defined at line 718*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
