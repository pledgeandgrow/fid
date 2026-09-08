# crypto — Fides Documentation

## Contents

- [K0:](#k0:) (const)
- [K1:](#k1:) (const)
- [K2:](#k2:) (const)
- [K3:](#k3:) (const)
- [K4:](#k4:) (const)
- [K5:](#k5:) (const)
- [K6:](#k6:) (const)
- [K7:](#k7:) (const)
- [K8:](#k8:) (const)
- [K9:](#k9:) (const)
- [K10:](#k10:) (const)
- [K11:](#k11:) (const)
- [K12:](#k12:) (const)
- [K13:](#k13:) (const)
- [K14:](#k14:) (const)
- [K15:](#k15:) (const)
- [K16:](#k16:) (const)
- [K17:](#k17:) (const)
- [K18:](#k18:) (const)
- [K19:](#k19:) (const)
- [K20:](#k20:) (const)
- [K21:](#k21:) (const)
- [K22:](#k22:) (const)
- [K23:](#k23:) (const)
- [K24:](#k24:) (const)
- [K25:](#k25:) (const)
- [K26:](#k26:) (const)
- [K27:](#k27:) (const)
- [K28:](#k28:) (const)
- [K29:](#k29:) (const)
- [K30:](#k30:) (const)
- [K31:](#k31:) (const)
- [K32:](#k32:) (const)
- [K33:](#k33:) (const)
- [K34:](#k34:) (const)
- [K35:](#k35:) (const)
- [K36:](#k36:) (const)
- [K37:](#k37:) (const)
- [K38:](#k38:) (const)
- [K39:](#k39:) (const)
- [K40:](#k40:) (const)
- [K41:](#k41:) (const)
- [K42:](#k42:) (const)
- [K43:](#k43:) (const)
- [K44:](#k44:) (const)
- [K45:](#k45:) (const)
- [K46:](#k46:) (const)
- [K47:](#k47:) (const)
- [K48:](#k48:) (const)
- [K49:](#k49:) (const)
- [K50:](#k50:) (const)
- [K51:](#k51:) (const)
- [K52:](#k52:) (const)
- [K53:](#k53:) (const)
- [K54:](#k54:) (const)
- [K55:](#k55:) (const)
- [K56:](#k56:) (const)
- [K57:](#k57:) (const)
- [K58:](#k58:) (const)
- [K59:](#k59:) (const)
- [K60:](#k60:) (const)
- [K61:](#k61:) (const)
- [K62:](#k62:) (const)
- [K63:](#k63:) (const)
- [H0_INIT:](#h0_init:) (const)
- [H1_INIT:](#h1_init:) (const)
- [H2_INIT:](#h2_init:) (const)
- [H3_INIT:](#h3_init:) (const)
- [H4_INIT:](#h4_init:) (const)
- [H5_INIT:](#h5_init:) (const)
- [H6_INIT:](#h6_init:) (const)
- [H7_INIT:](#h7_init:) (const)
- [crypto_sha256](#crypto_sha256) (function)
- [crypto_sha256_hex](#crypto_sha256_hex) (function)
- [crypto_sha256_hex_str](#crypto_sha256_hex_str) (function)
- [NonceTracker](#noncetracker) (struct)
- [crypto_nonce_tracker_new](#crypto_nonce_tracker_new) (function)
- [crypto_nonce_next](#crypto_nonce_next) (function)
- [crypto_nonce_is_used](#crypto_nonce_is_used) (function)
- [crypto_nonce_register](#crypto_nonce_register) (function)
- [crypto_nonce_reset](#crypto_nonce_reset) (function)
- [crypto_hmac_sha256](#crypto_hmac_sha256) (function)
- [crypto_constant_time_eq](#crypto_constant_time_eq) (function)
- [CRYPTO_APPROVED_ALGORITHMS:](#crypto_approved_algorithms:) (const)
- [CRYPTO_REJECTED_ALGORITHMS:](#crypto_rejected_algorithms:) (const)

## K0:

```fides
pub const K0: i64  = 0x428a2f98;
```

*Defined at line 29*

---

## K1:

```fides
pub const K1: i64  = 0x71374491;
```

*Defined at line 30*

---

## K2:

```fides
pub const K2: i64  = 0xb5c0fbcf;
```

*Defined at line 31*

---

## K3:

```fides
pub const K3: i64  = 0xe9b5dba5;
```

*Defined at line 32*

---

## K4:

```fides
pub const K4: i64  = 0x3956c25b;
```

*Defined at line 33*

---

## K5:

```fides
pub const K5: i64  = 0x59f111f1;
```

*Defined at line 34*

---

## K6:

```fides
pub const K6: i64  = 0x923f82a4;
```

*Defined at line 35*

---

## K7:

```fides
pub const K7: i64  = 0xab1c5ed5;
```

*Defined at line 36*

---

## K8:

```fides
pub const K8: i64  = 0xd807aa98;
```

*Defined at line 37*

---

## K9:

```fides
pub const K9: i64  = 0x12835b01;
```

*Defined at line 38*

---

## K10:

```fides
pub const K10: i64 = 0x243185be;
```

*Defined at line 39*

---

## K11:

```fides
pub const K11: i64 = 0x550c7dc3;
```

*Defined at line 40*

---

## K12:

```fides
pub const K12: i64 = 0x72be5d74;
```

*Defined at line 41*

---

## K13:

```fides
pub const K13: i64 = 0x80deb1fe;
```

*Defined at line 42*

---

## K14:

```fides
pub const K14: i64 = 0x9bdc06a7;
```

*Defined at line 43*

---

## K15:

```fides
pub const K15: i64 = 0xc19bf174;
```

*Defined at line 44*

---

## K16:

```fides
pub const K16: i64 = 0xe49b69c1;
```

*Defined at line 45*

---

## K17:

```fides
pub const K17: i64 = 0xefbe4786;
```

*Defined at line 46*

---

## K18:

```fides
pub const K18: i64 = 0x0fc19dc6;
```

*Defined at line 47*

---

## K19:

```fides
pub const K19: i64 = 0x240ca1cc;
```

*Defined at line 48*

---

## K20:

```fides
pub const K20: i64 = 0x2de92c6f;
```

*Defined at line 49*

---

## K21:

```fides
pub const K21: i64 = 0x4a7484aa;
```

*Defined at line 50*

---

## K22:

```fides
pub const K22: i64 = 0x5cb0a9dc;
```

*Defined at line 51*

---

## K23:

```fides
pub const K23: i64 = 0x76f988da;
```

*Defined at line 52*

---

## K24:

```fides
pub const K24: i64 = 0x983e5152;
```

*Defined at line 53*

---

## K25:

```fides
pub const K25: i64 = 0xa831c66d;
```

*Defined at line 54*

---

## K26:

```fides
pub const K26: i64 = 0xb00327c8;
```

*Defined at line 55*

---

## K27:

```fides
pub const K27: i64 = 0xbf597fc7;
```

*Defined at line 56*

---

## K28:

```fides
pub const K28: i64 = 0xc6e00bf3;
```

*Defined at line 57*

---

## K29:

```fides
pub const K29: i64 = 0xd5a79147;
```

*Defined at line 58*

---

## K30:

```fides
pub const K30: i64 = 0x06ca6351;
```

*Defined at line 59*

---

## K31:

```fides
pub const K31: i64 = 0x14292967;
```

*Defined at line 60*

---

## K32:

```fides
pub const K32: i64 = 0x27b70a85;
```

*Defined at line 61*

---

## K33:

```fides
pub const K33: i64 = 0x2e1b2138;
```

*Defined at line 62*

---

## K34:

```fides
pub const K34: i64 = 0x4d2c6dfc;
```

*Defined at line 63*

---

## K35:

```fides
pub const K35: i64 = 0x53380d13;
```

*Defined at line 64*

---

## K36:

```fides
pub const K36: i64 = 0x650a7354;
```

*Defined at line 65*

---

## K37:

```fides
pub const K37: i64 = 0x766a0abb;
```

*Defined at line 66*

---

## K38:

```fides
pub const K38: i64 = 0x81c2c92e;
```

*Defined at line 67*

---

## K39:

```fides
pub const K39: i64 = 0x92722c85;
```

*Defined at line 68*

---

## K40:

```fides
pub const K40: i64 = 0xa2bfe8a1;
```

*Defined at line 69*

---

## K41:

```fides
pub const K41: i64 = 0xa81a664b;
```

*Defined at line 70*

---

## K42:

```fides
pub const K42: i64 = 0xc24b8b70;
```

*Defined at line 71*

---

## K43:

```fides
pub const K43: i64 = 0xc76c51a3;
```

*Defined at line 72*

---

## K44:

```fides
pub const K44: i64 = 0xd192e819;
```

*Defined at line 73*

---

## K45:

```fides
pub const K45: i64 = 0xd6990624;
```

*Defined at line 74*

---

## K46:

```fides
pub const K46: i64 = 0xf40e3585;
```

*Defined at line 75*

---

## K47:

```fides
pub const K47: i64 = 0x106aa070;
```

*Defined at line 76*

---

## K48:

```fides
pub const K48: i64 = 0x19a4c116;
```

*Defined at line 77*

---

## K49:

```fides
pub const K49: i64 = 0x1e376c08;
```

*Defined at line 78*

---

## K50:

```fides
pub const K50: i64 = 0x2748774c;
```

*Defined at line 79*

---

## K51:

```fides
pub const K51: i64 = 0x34b0bcb5;
```

*Defined at line 80*

---

## K52:

```fides
pub const K52: i64 = 0x391c0cb3;
```

*Defined at line 81*

---

## K53:

```fides
pub const K53: i64 = 0x4ed8aa4a;
```

*Defined at line 82*

---

## K54:

```fides
pub const K54: i64 = 0x5b9cca4f;
```

*Defined at line 83*

---

## K55:

```fides
pub const K55: i64 = 0x682e6ff3;
```

*Defined at line 84*

---

## K56:

```fides
pub const K56: i64 = 0x748f82ee;
```

*Defined at line 85*

---

## K57:

```fides
pub const K57: i64 = 0x78a5636f;
```

*Defined at line 86*

---

## K58:

```fides
pub const K58: i64 = 0x84c87814;
```

*Defined at line 87*

---

## K59:

```fides
pub const K59: i64 = 0x8cc70208;
```

*Defined at line 88*

---

## K60:

```fides
pub const K60: i64 = 0x90befffa;
```

*Defined at line 89*

---

## K61:

```fides
pub const K61: i64 = 0xa4506ceb;
```

*Defined at line 90*

---

## K62:

```fides
pub const K62: i64 = 0xbef9a3f7;
```

*Defined at line 91*

---

## K63:

```fides
pub const K63: i64 = 0xc67178f2;
```

*Defined at line 92*

---

## H0_INIT:

```fides
pub const H0_INIT: i64 = 0x6a09e667;
```

*Defined at line 95*

---

## H1_INIT:

```fides
pub const H1_INIT: i64 = 0xbb67ae85;
```

*Defined at line 96*

---

## H2_INIT:

```fides
pub const H2_INIT: i64 = 0x3c6ef372;
```

*Defined at line 97*

---

## H3_INIT:

```fides
pub const H3_INIT: i64 = 0xa54ff53a;
```

*Defined at line 98*

---

## H4_INIT:

```fides
pub const H4_INIT: i64 = 0x510e527f;
```

*Defined at line 99*

---

## H5_INIT:

```fides
pub const H5_INIT: i64 = 0x9b05688c;
```

*Defined at line 100*

---

## H6_INIT:

```fides
pub const H6_INIT: i64 = 0x1f83d9ab;
```

*Defined at line 101*

---

## H7_INIT:

```fides
pub const H7_INIT: i64 = 0x5be0cd19;
```

*Defined at line 102*

---

## crypto_sha256

Compute SHA-256 hash of a string.
Returns a 32-byte Vec of hash values (each byte 0-255).

```fides
pub fn crypto_sha256(input: str) -> Vec {
```

**Parameters:**

- `input`: `str`

**Returns:** `Vec`

*Defined at line 155*

---

## crypto_sha256_hex

Convert SHA-256 hash bytes to hex string.

```fides
pub fn crypto_sha256_hex(hash: Vec) -> str {
```

**Parameters:**

- `hash`: `Vec`

**Returns:** `str`

*Defined at line 325*

---

## crypto_sha256_hex_str

Compute SHA-256 and return as hex string.

```fides
pub fn crypto_sha256_hex_str(input: str) -> str {
```

**Parameters:**

- `input`: `str`

**Returns:** `str`

*Defined at line 339*

---

## NonceTracker

A nonce tracker — keeps track of used nonces to prevent reuse.

```fides
pub struct NonceTracker {
```

**Fields:**

| Name | Type |
|------|------|
| `used` | `Vec` |
| `next` | `i64` |

*Defined at line 353*

---

## crypto_nonce_tracker_new

Create a new nonce tracker.

```fides
pub fn crypto_nonce_tracker_new() -> NonceTracker {
```

**Returns:** `NonceTracker`

*Defined at line 359*

---

## crypto_nonce_next

Generate and track a nonce. Returns the next sequential nonce.

```fides
pub fn crypto_nonce_next(tracker: NonceTracker) -> i64 {
```

**Parameters:**

- `tracker`: `NonceTracker`

**Returns:** `i64`

*Defined at line 367*

---

## crypto_nonce_is_used

Check if a nonce has been used.

```fides
pub fn crypto_nonce_is_used(tracker: NonceTracker, nonce: i64) -> bool {
```

**Parameters:**

- `tracker`: `NonceTracker`
- `nonce`: `i64`

**Returns:** `bool`

*Defined at line 375*

---

## crypto_nonce_register

Register a specific nonce value as used. Returns false if already used.

```fides
pub fn crypto_nonce_register(tracker: NonceTracker, nonce: i64) -> bool {
```

**Parameters:**

- `tracker`: `NonceTracker`
- `nonce`: `i64`

**Returns:** `bool`

*Defined at line 385*

---

## crypto_nonce_reset

Reset nonce tracking (for testing only).

```fides
pub fn crypto_nonce_reset(tracker: NonceTracker) -> void {
```

**Parameters:**

- `tracker`: `NonceTracker`

**Returns:** `void`

*Defined at line 397*

---

## crypto_hmac_sha256

Compute HMAC-SHA256.
Returns a 32-byte Vec.

```fides
pub fn crypto_hmac_sha256(key: str, message: str) -> Vec {
```

**Parameters:**

- `key`: `str`
- `message`: `str`

**Returns:** `Vec`

*Defined at line 412*

---

## crypto_constant_time_eq

Compare two byte arrays in constant time (prevents timing attacks).
Returns true if equal, false otherwise. Always processes all bytes.

```fides
pub fn crypto_constant_time_eq(a: Vec, b: Vec) -> bool {
```

**Parameters:**

- `a`: `Vec`
- `b`: `Vec`

**Returns:** `bool`

*Defined at line 458*

---

## CRYPTO_APPROVED_ALGORITHMS:

```fides
pub const CRYPTO_APPROVED_ALGORITHMS: str = "SHA-256, HMAC-SHA256";
```

*Defined at line 484*

---

## CRYPTO_REJECTED_ALGORITHMS:

```fides
pub const CRYPTO_REJECTED_ALGORITHMS: str = "MD5, SHA-1, custom";
```

*Defined at line 485*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
