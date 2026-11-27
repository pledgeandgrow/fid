[All modules](index.md)

# biometrics -- Fides Documentation

## Contents

- [BIO_STRONG:](#BIO_STRONG:) (const)
- [BIO_WEAK:](#BIO_WEAK:) (const)
- [BIO_DEVICE_CREDENTIAL:](#BIO_DEVICE_CREDENTIAL:) (const)
- [BIO_AVAILABLE:](#BIO_AVAILABLE:) (const)
- [BIO_NO_HARDWARE:](#BIO_NO_HARDWARE:) (const)
- [BIO_HW_UNAVAILABLE:](#BIO_HW_UNAVAILABLE:) (const)
- [BIO_NOT_ENROLLED:](#BIO_NOT_ENROLLED:) (const)
- [BIO_SECURITY_UPDATE:](#BIO_SECURITY_UPDATE:) (const)
- [BIO_PASSCODE_NOT_SET:](#BIO_PASSCODE_NOT_SET:) (const)
- [BIO_LOCKED_OUT:](#BIO_LOCKED_OUT:) (const)
- [BIO_UNSUPPORTED:](#BIO_UNSUPPORTED:) (const)
- [BIO_UNKNOWN:](#BIO_UNKNOWN:) (const)
- [BIO_KIND_NONE:](#BIO_KIND_NONE:) (const)
- [BIO_KIND_FINGERPRINT:](#BIO_KIND_FINGERPRINT:) (const)
- [BIO_KIND_FACE:](#BIO_KIND_FACE:) (const)
- [BIO_KIND_IRIS:](#BIO_KIND_IRIS:) (const)
- [BIO_SUCCESS:](#BIO_SUCCESS:) (const)
- [BIO_NOT_RECOGNIZED:](#BIO_NOT_RECOGNIZED:) (const)
- [BIO_USER_CANCELED:](#BIO_USER_CANCELED:) (const)
- [BIO_FALLBACK:](#BIO_FALLBACK:) (const)
- [BIO_LOCKOUT:](#BIO_LOCKOUT:) (const)
- [BIO_LOCKOUT_PERMANENT:](#BIO_LOCKOUT_PERMANENT:) (const)
- [BIO_NOT_AVAILABLE:](#BIO_NOT_AVAILABLE:) (const)
- [BIO_SYSTEM_CANCELED:](#BIO_SYSTEM_CANCELED:) (const)
- [BIO_TIMEOUT:](#BIO_TIMEOUT:) (const)
- [BIO_ERROR:](#BIO_ERROR:) (const)
- [bio_availability_name](#bio_availability_name) (function)
- [bio_result_name](#bio_result_name) (function)
- [bio_android_availability](#bio_android_availability) (function)
- [bio_ios_availability](#bio_ios_availability) (function)
- [bio_ios_kind](#bio_ios_kind) (function)
- [bio_android_kind](#bio_android_kind) (function)
- [bio_android_authenticators_ok](#bio_android_authenticators_ok) (function)
- [bio_ios_policy](#bio_ios_policy) (function)
- [BioPrompt](#BioPrompt) (struct)
- [bio_prompt_new](#bio_prompt_new) (function)
- [bio_prompt_check_android](#bio_prompt_check_android) (function)
- [bio_prompt_check_ios](#bio_prompt_check_ios) (function)
- [bio_android_error](#bio_android_error) (function)
- [bio_ios_error](#bio_ios_error) (function)
- [bio_next_action](#bio_next_action) (function)
- [BIO_STATE_IDLE:](#BIO_STATE_IDLE:) (const)
- [BIO_STATE_PROMPTING:](#BIO_STATE_PROMPTING:) (const)
- [BIO_STATE_SUCCEEDED:](#BIO_STATE_SUCCEEDED:) (const)
- [BIO_STATE_FAILED:](#BIO_STATE_FAILED:) (const)
- [BIO_STATE_LOCKED:](#BIO_STATE_LOCKED:) (const)
- [BioSession](#BioSession) (struct)
- [bio_session_new](#bio_session_new) (function)
- [bio_session_start](#bio_session_start) (function)
- [bio_session_android_failed](#bio_session_android_failed) (function)
- [bio_session_android_error](#bio_session_android_error) (function)
- [bio_session_android_succeeded](#bio_session_android_succeeded) (function)
- [bio_session_ios](#bio_session_ios) (function)
- [BioCall](#BioCall) (struct)
- [bio_android_calls](#bio_android_calls) (function)
- [bio_ios_calls](#bio_ios_calls) (function)

## BIO_STRONG:

```fides
pub const BIO_STRONG: i64 = 0x000F;
```

*Defined at line 32*

---

## BIO_WEAK:

```fides
pub const BIO_WEAK: i64 = 0x00FF;
```

*Defined at line 33*

---

## BIO_DEVICE_CREDENTIAL:

```fides
pub const BIO_DEVICE_CREDENTIAL: i64 = 0x8000;
```

*Defined at line 34*

---

## BIO_AVAILABLE:

```fides
pub const BIO_AVAILABLE: i64 = 0;
```

*Defined at line 37*

---

## BIO_NO_HARDWARE:

```fides
pub const BIO_NO_HARDWARE: i64 = 1;
```

*Defined at line 38*

---

## BIO_HW_UNAVAILABLE:

```fides
pub const BIO_HW_UNAVAILABLE: i64 = 2;
```

*Defined at line 39*

---

## BIO_NOT_ENROLLED:

```fides
pub const BIO_NOT_ENROLLED: i64 = 3;
```

*Defined at line 40*

---

## BIO_SECURITY_UPDATE:

```fides
pub const BIO_SECURITY_UPDATE: i64 = 4;
```

*Defined at line 41*

---

## BIO_PASSCODE_NOT_SET:

```fides
pub const BIO_PASSCODE_NOT_SET: i64 = 5;
```

*Defined at line 42*

---

## BIO_LOCKED_OUT:

```fides
pub const BIO_LOCKED_OUT: i64 = 6;
```

*Defined at line 43*

---

## BIO_UNSUPPORTED:

```fides
pub const BIO_UNSUPPORTED: i64 = 7;
```

*Defined at line 44*

---

## BIO_UNKNOWN:

```fides
pub const BIO_UNKNOWN: i64 = 8;
```

*Defined at line 45*

---

## BIO_KIND_NONE:

```fides
pub const BIO_KIND_NONE: i64 = 0;
```

*Defined at line 48*

---

## BIO_KIND_FINGERPRINT:

```fides
pub const BIO_KIND_FINGERPRINT: i64 = 1;
```

*Defined at line 49*

---

## BIO_KIND_FACE:

```fides
pub const BIO_KIND_FACE: i64 = 2;
```

*Defined at line 50*

---

## BIO_KIND_IRIS:

```fides
pub const BIO_KIND_IRIS: i64 = 3;
```

*Defined at line 51*

---

## BIO_SUCCESS:

```fides
pub const BIO_SUCCESS: i64 = 0;
```

*Defined at line 54*

---

## BIO_NOT_RECOGNIZED:

```fides
pub const BIO_NOT_RECOGNIZED: i64 = 1;  // try again, prompt still up
```

*Defined at line 55*

---

## BIO_USER_CANCELED:

```fides
pub const BIO_USER_CANCELED: i64 = 2;
```

*Defined at line 56*

---

## BIO_FALLBACK:

```fides
pub const BIO_FALLBACK: i64 = 3;  // user chose password / passcode
```

*Defined at line 57*

---

## BIO_LOCKOUT:

```fides
pub const BIO_LOCKOUT: i64 = 4;  // temporary
```

*Defined at line 58*

---

## BIO_LOCKOUT_PERMANENT:

```fides
pub const BIO_LOCKOUT_PERMANENT: i64 = 5;  // device credential needed
```

*Defined at line 59*

---

## BIO_NOT_AVAILABLE:

```fides
pub const BIO_NOT_AVAILABLE: i64 = 6;
```

*Defined at line 60*

---

## BIO_SYSTEM_CANCELED:

```fides
pub const BIO_SYSTEM_CANCELED: i64 = 7;  // app went to background, another prompt
```

*Defined at line 61*

---

## BIO_TIMEOUT:

```fides
pub const BIO_TIMEOUT: i64 = 8;
```

*Defined at line 62*

---

## BIO_ERROR:

```fides
pub const BIO_ERROR: i64 = 9;
```

*Defined at line 63*

---

## bio_availability_name

```fides
pub fn bio_availability_name(a: i64) -> str {
```

**Parameters:**

- `a`: `i64`

**Returns:** `str`

*Defined at line 65*

---

## bio_result_name

```fides
pub fn bio_result_name(r: i64) -> str {
```

**Parameters:**

- `r`: `i64`

**Returns:** `str`

*Defined at line 73*

---

## bio_android_availability

BiometricManager.canAuthenticate(int) -> availability.

```fides
pub fn bio_android_availability(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 84*

---

## bio_ios_availability

canEvaluatePolicy:error: -> availability (`ok` the BOOL, `la_error`
the NSError code when it is NO).

```fides
pub fn bio_ios_availability(ok: bool, la_error: i64) -> i64 {
```

**Parameters:**

- `ok`: `bool`
- `la_error`: `i64`

**Returns:** `i64`

*Defined at line 108*

---

## bio_ios_kind

LABiometryType (0 none, 1 touchID, 2 faceID, 4 opticID) -> kind.

```fides
pub fn bio_ios_kind(biometry_type: i64) -> i64 {
```

**Parameters:**

- `biometry_type`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## bio_android_kind

PackageManager features -> kind (face before iris before fingerprint
is not implied: the first feature present wins in that order).

```fides
pub fn bio_android_kind(fingerprint: bool, face: bool, iris: bool) -> i64 {
```

**Parameters:**

- `fingerprint`: `bool`
- `face`: `bool`
- `iris`: `bool`

**Returns:** `i64`

*Defined at line 143*

---

## bio_android_authenticators_ok

Does BiometricPrompt accept this authenticator set on `api`? (STRONG
or WEAK, either with DEVICE_CREDENTIAL; DEVICE_CREDENTIAL alone from
API 30; STRONG | DEVICE_CREDENTIAL not on API 28-29.)

```fides
pub fn bio_android_authenticators_ok(auth: i64, api: i64) -> bool {
```

**Parameters:**

- `auth`: `i64`
- `api`: `i64`

**Returns:** `bool`

*Defined at line 159*

---

## bio_ios_policy

The LAPolicy of an authenticator set: 1 (biometrics only) or 2
(biometrics or the passcode).

```fides
pub fn bio_ios_policy(auth: i64) -> i64 {
```

**Parameters:**

- `auth`: `i64`

**Returns:** `i64`

*Defined at line 179*

---

## BioPrompt

```fides
pub struct BioPrompt {
```

**Fields:**

| Name | Type |
|------|------|
| `title` | `str` |
| `subtitle` | `str` |
| `description` | `str` |
| `negative_text` | `str,  // Android negative button` |
| `reason` | `str,  // iOS localizedReason` |
| `fallback_title` | `str,  // iOS: "" hides the fallback button` |
| `cancel_title` | `str` |
| `confirmation_required` | `bool` |
| `authenticators` | `i64` |

*Defined at line 188*

---

## bio_prompt_new

```fides
pub fn bio_prompt_new(title: str, reason: str) -> BioPrompt {
```

**Parameters:**

- `title`: `str`
- `reason`: `str`

**Returns:** `BioPrompt`

*Defined at line 200*

---

## bio_prompt_check_android

Problems of a prompt on Android (API `api`).

```fides
pub fn bio_prompt_check_android(p: &BioPrompt, api: i64) -> Vec<str> {
```

**Parameters:**

- `p`: `&BioPrompt`
- `api`: `i64`

**Returns:** `Vec<str>`

*Defined at line 207*

---

## bio_prompt_check_ios

Problems of a prompt on iOS. `has_face_id_key`: Info.plist has
NSFaceIDUsageDescription (needed when the device has Face ID).

```fides
pub fn bio_prompt_check_ios(p: &BioPrompt, kind: i64, has_face_id_key: bool) -> Vec<str> {
```

**Parameters:**

- `p`: `&BioPrompt`
- `kind`: `i64`
- `has_face_id_key`: `bool`

**Returns:** `Vec<str>`

*Defined at line 227*

---

## bio_android_error

BiometricPrompt.AuthenticationCallback.onAuthenticationError codes.

```fides
pub fn bio_android_error(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 241*

---

## bio_ios_error

LAError codes of evaluatePolicy's reply.

```fides
pub fn bio_ios_error(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 267*

---

## bio_next_action

What the app should do after a result: "done", "retry", "fallback"
(offer password / device credential), "wait" (lockout), "give-up".

```fides
pub fn bio_next_action(result: i64) -> str {
```

**Parameters:**

- `result`: `i64`

**Returns:** `str`

*Defined at line 291*

---

## BIO_STATE_IDLE:

```fides
pub const BIO_STATE_IDLE: i64 = 0;
```

*Defined at line 309*

---

## BIO_STATE_PROMPTING:

```fides
pub const BIO_STATE_PROMPTING: i64 = 1;
```

*Defined at line 310*

---

## BIO_STATE_SUCCEEDED:

```fides
pub const BIO_STATE_SUCCEEDED: i64 = 2;
```

*Defined at line 311*

---

## BIO_STATE_FAILED:

```fides
pub const BIO_STATE_FAILED: i64 = 3;
```

*Defined at line 312*

---

## BIO_STATE_LOCKED:

```fides
pub const BIO_STATE_LOCKED: i64 = 4;
```

*Defined at line 313*

---

## BioSession

```fides
pub struct BioSession {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `failures` | `i64,  // consecutive not-recognized attempts` |
| `max_failures` | `i64,  // 5 on both platforms` |
| `lockout_ms` | `i64,  // 30000 on Android` |
| `locked_until` | `i64` |
| `last_result` | `i64` |
| `used_credential` | `bool,  // succeeded with PIN / pattern / passcode` |

*Defined at line 315*

---

## bio_session_new

```fides
pub fn bio_session_new() -> BioSession {
```

**Returns:** `BioSession`

*Defined at line 325*

---

## bio_session_start

Start a prompt at `now` ms: false while locked out.

```fides
pub fn bio_session_start(s: &mut BioSession, now: i64) -> bool {
```

**Parameters:**

- `s`: `&mut BioSession`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 332*

---

## bio_session_android_failed

onAuthenticationFailed: a finger or face not recognized; the prompt
stays up until the platform's limit, which locks it out.

```fides
pub fn bio_session_android_failed(s: &mut BioSession, now: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut BioSession`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 359*

---

## bio_session_android_error

onAuthenticationError(code).

```fides
pub fn bio_session_android_error(s: &mut BioSession, code: i64, now: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut BioSession`
- `code`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 370*

---

## bio_session_android_succeeded

onAuthenticationSucceeded(result): authenticationType 1 is the device
credential, 2 a biometric.

```fides
pub fn bio_session_android_succeeded(s: &mut BioSession, auth_type: i64, now: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut BioSession`
- `auth_type`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 378*

---

## bio_session_ios

evaluatePolicy's reply: success, or the LAError code.

```fides
pub fn bio_session_ios(s: &mut BioSession, success: bool, la_error: i64, now: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut BioSession`
- `success`: `bool`
- `la_error`: `i64`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 385*

---

## BioCall

```fides
pub struct BioCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |

*Defined at line 400*

---

## bio_android_calls

```fides
pub fn bio_android_calls() -> Vec<BioCall> {
```

**Returns:** `Vec<BioCall>`

*Defined at line 406*

---

## bio_ios_calls

```fides
pub fn bio_ios_calls() -> Vec<BioCall> {
```

**Returns:** `Vec<BioCall>`

*Defined at line 433*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
