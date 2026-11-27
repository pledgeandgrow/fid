[All modules](index.md)

# crypto -- Fides Documentation

## Contents

- [K0:](#K0:) (const)
- [K1:](#K1:) (const)
- [K2:](#K2:) (const)
- [K3:](#K3:) (const)
- [K4:](#K4:) (const)
- [K5:](#K5:) (const)
- [K6:](#K6:) (const)
- [K7:](#K7:) (const)
- [K8:](#K8:) (const)
- [K9:](#K9:) (const)
- [K10:](#K10:) (const)
- [K11:](#K11:) (const)
- [K12:](#K12:) (const)
- [K13:](#K13:) (const)
- [K14:](#K14:) (const)
- [K15:](#K15:) (const)
- [K16:](#K16:) (const)
- [K17:](#K17:) (const)
- [K18:](#K18:) (const)
- [K19:](#K19:) (const)
- [K20:](#K20:) (const)
- [K21:](#K21:) (const)
- [K22:](#K22:) (const)
- [K23:](#K23:) (const)
- [K24:](#K24:) (const)
- [K25:](#K25:) (const)
- [K26:](#K26:) (const)
- [K27:](#K27:) (const)
- [K28:](#K28:) (const)
- [K29:](#K29:) (const)
- [K30:](#K30:) (const)
- [K31:](#K31:) (const)
- [K32:](#K32:) (const)
- [K33:](#K33:) (const)
- [K34:](#K34:) (const)
- [K35:](#K35:) (const)
- [K36:](#K36:) (const)
- [K37:](#K37:) (const)
- [K38:](#K38:) (const)
- [K39:](#K39:) (const)
- [K40:](#K40:) (const)
- [K41:](#K41:) (const)
- [K42:](#K42:) (const)
- [K43:](#K43:) (const)
- [K44:](#K44:) (const)
- [K45:](#K45:) (const)
- [K46:](#K46:) (const)
- [K47:](#K47:) (const)
- [K48:](#K48:) (const)
- [K49:](#K49:) (const)
- [K50:](#K50:) (const)
- [K51:](#K51:) (const)
- [K52:](#K52:) (const)
- [K53:](#K53:) (const)
- [K54:](#K54:) (const)
- [K55:](#K55:) (const)
- [K56:](#K56:) (const)
- [K57:](#K57:) (const)
- [K58:](#K58:) (const)
- [K59:](#K59:) (const)
- [K60:](#K60:) (const)
- [K61:](#K61:) (const)
- [K62:](#K62:) (const)
- [K63:](#K63:) (const)
- [H0_INIT:](#H0_INIT:) (const)
- [H1_INIT:](#H1_INIT:) (const)
- [H2_INIT:](#H2_INIT:) (const)
- [H3_INIT:](#H3_INIT:) (const)
- [H4_INIT:](#H4_INIT:) (const)
- [H5_INIT:](#H5_INIT:) (const)
- [H6_INIT:](#H6_INIT:) (const)
- [H7_INIT:](#H7_INIT:) (const)
- [crypto_sha256](#crypto_sha256) (function)
- [crypto_sha256_bytes](#crypto_sha256_bytes) (function)
- [crypto_sha256_hex](#crypto_sha256_hex) (function)
- [crypto_sha256_hex_str](#crypto_sha256_hex_str) (function)
- [NonceTracker](#NonceTracker) (struct)
- [crypto_nonce_tracker_new](#crypto_nonce_tracker_new) (function)
- [crypto_nonce_next](#crypto_nonce_next) (function)
- [crypto_nonce_is_used](#crypto_nonce_is_used) (function)
- [crypto_nonce_register](#crypto_nonce_register) (function)
- [crypto_nonce_reset](#crypto_nonce_reset) (function)
- [crypto_hmac_sha256](#crypto_hmac_sha256) (function)
- [crypto_hmac_sha256_bytes](#crypto_hmac_sha256_bytes) (function)
- [crypto_pbkdf2_sha256](#crypto_pbkdf2_sha256) (function)
- [crypto_constant_time_eq](#crypto_constant_time_eq) (function)
- [CRYPTO_APPROVED_ALGORITHMS:](#CRYPTO_APPROVED_ALGORITHMS:) (const)
- [CRYPTO_REJECTED_ALGORITHMS:](#CRYPTO_REJECTED_ALGORITHMS:) (const)
- [crypto_random_u64](#crypto_random_u64) (function)
- [crypto_random_bytes](#crypto_random_bytes) (function)

## K0:

```fides
pub const K0: i64 = 0x428a2f98;
```

*Defined at line 30*

---

## K1:

```fides
pub const K1: i64 = 0x71374491;
```

*Defined at line 31*

---

## K2:

```fides
pub const K2: i64 = 0xb5c0fbcf;
```

*Defined at line 32*

---

## K3:

```fides
pub const K3: i64 = 0xe9b5dba5;
```

*Defined at line 33*

---

## K4:

```fides
pub const K4: i64 = 0x3956c25b;
```

*Defined at line 34*

---

## K5:

```fides
pub const K5: i64 = 0x59f111f1;
```

*Defined at line 35*

---

## K6:

```fides
pub const K6: i64 = 0x923f82a4;
```

*Defined at line 36*

---

## K7:

```fides
pub const K7: i64 = 0xab1c5ed5;
```

*Defined at line 37*

---

## K8:

```fides
pub const K8: i64 = 0xd807aa98;
```

*Defined at line 38*

---

## K9:

```fides
pub const K9: i64 = 0x12835b01;
```

*Defined at line 39*

---

## K10:

```fides
pub const K10: i64 = 0x243185be;
```

*Defined at line 40*

---

## K11:

```fides
pub const K11: i64 = 0x550c7dc3;
```

*Defined at line 41*

---

## K12:

```fides
pub const K12: i64 = 0x72be5d74;
```

*Defined at line 42*

---

## K13:

```fides
pub const K13: i64 = 0x80deb1fe;
```

*Defined at line 43*

---

## K14:

```fides
pub const K14: i64 = 0x9bdc06a7;
```

*Defined at line 44*

---

## K15:

```fides
pub const K15: i64 = 0xc19bf174;
```

*Defined at line 45*

---

## K16:

```fides
pub const K16: i64 = 0xe49b69c1;
```

*Defined at line 46*

---

## K17:

```fides
pub const K17: i64 = 0xefbe4786;
```

*Defined at line 47*

---

## K18:

```fides
pub const K18: i64 = 0x0fc19dc6;
```

*Defined at line 48*

---

## K19:

```fides
pub const K19: i64 = 0x240ca1cc;
```

*Defined at line 49*

---

## K20:

```fides
pub const K20: i64 = 0x2de92c6f;
```

*Defined at line 50*

---

## K21:

```fides
pub const K21: i64 = 0x4a7484aa;
```

*Defined at line 51*

---

## K22:

```fides
pub const K22: i64 = 0x5cb0a9dc;
```

*Defined at line 52*

---

## K23:

```fides
pub const K23: i64 = 0x76f988da;
```

*Defined at line 53*

---

## K24:

```fides
pub const K24: i64 = 0x983e5152;
```

*Defined at line 54*

---

## K25:

```fides
pub const K25: i64 = 0xa831c66d;
```

*Defined at line 55*

---

## K26:

```fides
pub const K26: i64 = 0xb00327c8;
```

*Defined at line 56*

---

## K27:

```fides
pub const K27: i64 = 0xbf597fc7;
```

*Defined at line 57*

---

## K28:

```fides
pub const K28: i64 = 0xc6e00bf3;
```

*Defined at line 58*

---

## K29:

```fides
pub const K29: i64 = 0xd5a79147;
```

*Defined at line 59*

---

## K30:

```fides
pub const K30: i64 = 0x06ca6351;
```

*Defined at line 60*

---

## K31:

```fides
pub const K31: i64 = 0x14292967;
```

*Defined at line 61*

---

## K32:

```fides
pub const K32: i64 = 0x27b70a85;
```

*Defined at line 62*

---

## K33:

```fides
pub const K33: i64 = 0x2e1b2138;
```

*Defined at line 63*

---

## K34:

```fides
pub const K34: i64 = 0x4d2c6dfc;
```

*Defined at line 64*

---

## K35:

```fides
pub const K35: i64 = 0x53380d13;
```

*Defined at line 65*

---

## K36:

```fides
pub const K36: i64 = 0x650a7354;
```

*Defined at line 66*

---

## K37:

```fides
pub const K37: i64 = 0x766a0abb;
```

*Defined at line 67*

---

## K38:

```fides
pub const K38: i64 = 0x81c2c92e;
```

*Defined at line 68*

---

## K39:

```fides
pub const K39: i64 = 0x92722c85;
```

*Defined at line 69*

---

## K40:

```fides
pub const K40: i64 = 0xa2bfe8a1;
```

*Defined at line 70*

---

## K41:

```fides
pub const K41: i64 = 0xa81a664b;
```

*Defined at line 71*

---

## K42:

```fides
pub const K42: i64 = 0xc24b8b70;
```

*Defined at line 72*

---

## K43:

```fides
pub const K43: i64 = 0xc76c51a3;
```

*Defined at line 73*

---

## K44:

```fides
pub const K44: i64 = 0xd192e819;
```

*Defined at line 74*

---

## K45:

```fides
pub const K45: i64 = 0xd6990624;
```

*Defined at line 75*

---

## K46:

```fides
pub const K46: i64 = 0xf40e3585;
```

*Defined at line 76*

---

## K47:

```fides
pub const K47: i64 = 0x106aa070;
```

*Defined at line 77*

---

## K48:

```fides
pub const K48: i64 = 0x19a4c116;
```

*Defined at line 78*

---

## K49:

```fides
pub const K49: i64 = 0x1e376c08;
```

*Defined at line 79*

---

## K50:

```fides
pub const K50: i64 = 0x2748774c;
```

*Defined at line 80*

---

## K51:

```fides
pub const K51: i64 = 0x34b0bcb5;
```

*Defined at line 81*

---

## K52:

```fides
pub const K52: i64 = 0x391c0cb3;
```

*Defined at line 82*

---

## K53:

```fides
pub const K53: i64 = 0x4ed8aa4a;
```

*Defined at line 83*

---

## K54:

```fides
pub const K54: i64 = 0x5b9cca4f;
```

*Defined at line 84*

---

## K55:

```fides
pub const K55: i64 = 0x682e6ff3;
```

*Defined at line 85*

---

## K56:

```fides
pub const K56: i64 = 0x748f82ee;
```

*Defined at line 86*

---

## K57:

```fides
pub const K57: i64 = 0x78a5636f;
```

*Defined at line 87*

---

## K58:

```fides
pub const K58: i64 = 0x84c87814;
```

*Defined at line 88*

---

## K59:

```fides
pub const K59: i64 = 0x8cc70208;
```

*Defined at line 89*

---

## K60:

```fides
pub const K60: i64 = 0x90befffa;
```

*Defined at line 90*

---

## K61:

```fides
pub const K61: i64 = 0xa4506ceb;
```

*Defined at line 91*

---

## K62:

```fides
pub const K62: i64 = 0xbef9a3f7;
```

*Defined at line 92*

---

## K63:

```fides
pub const K63: i64 = 0xc67178f2;
```

*Defined at line 93*

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
pub fn crypto_sha256(input: str) -> Vec<i64> {
```

**Parameters:**

- `input`: `str`

**Returns:** `Vec<i64>`

*Defined at line 163*

---

## crypto_sha256_bytes

Compute SHA-256 of a byte vector (each element 0-255): binary-safe.
Returns a 32-byte Vec.

```fides
pub fn crypto_sha256_bytes(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 169*

---

## crypto_sha256_hex

Convert SHA-256 hash bytes to hex string.

```fides
pub fn crypto_sha256_hex(hash: Vec<i64>) -> str {
```

**Parameters:**

- `hash`: `Vec<i64>`

**Returns:** `str`

*Defined at line 370*

---

## crypto_sha256_hex_str

Compute SHA-256 and return as hex string.

```fides
pub fn crypto_sha256_hex_str(input: str) -> str {
```

**Parameters:**

- `input`: `str`

**Returns:** `str`

*Defined at line 384*

---

## NonceTracker

A nonce tracker — keeps track of used nonces to prevent reuse.

```fides
pub struct NonceTracker {
```

**Fields:**

| Name | Type |
|------|------|
| `used` | `Vec<i64>` |
| `next` | `i64` |

*Defined at line 398*

---

## crypto_nonce_tracker_new

Create a new nonce tracker.

```fides
pub fn crypto_nonce_tracker_new() -> NonceTracker {
```

**Returns:** `NonceTracker`

*Defined at line 404*

---

## crypto_nonce_next

Generate and track a nonce. Returns the next sequential nonce.

```fides
pub fn crypto_nonce_next(tracker: &mut NonceTracker) -> i64 {
```

**Parameters:**

- `tracker`: `&mut NonceTracker`

**Returns:** `i64`

*Defined at line 411*

---

## crypto_nonce_is_used

Check if a nonce has been used.

```fides
pub fn crypto_nonce_is_used(tracker: &NonceTracker, nonce: i64) -> bool {
```

**Parameters:**

- `tracker`: `&NonceTracker`
- `nonce`: `i64`

**Returns:** `bool`

*Defined at line 419*

---

## crypto_nonce_register

Register a specific nonce value as used. Returns false if already used.

```fides
pub fn crypto_nonce_register(tracker: &mut NonceTracker, nonce: i64) -> bool {
```

**Parameters:**

- `tracker`: `&mut NonceTracker`
- `nonce`: `i64`

**Returns:** `bool`

*Defined at line 429*

---

## crypto_nonce_reset

Reset nonce tracking (for testing only).

```fides
pub fn crypto_nonce_reset(tracker: &mut NonceTracker) -> void {
```

**Parameters:**

- `tracker`: `&mut NonceTracker`

**Returns:** `void`

*Defined at line 441*

---

## crypto_hmac_sha256

```fides
pub fn crypto_hmac_sha256(@secret key: str, message: str) -> Vec<i64> {
```

**Parameters:**

- `@secret key`: `str`
- `message`: `str`

**Returns:** `Vec<i64>`

*Defined at line 457*

---

## crypto_hmac_sha256_bytes

```fides
pub fn crypto_hmac_sha256_bytes(@secret key: Vec<i64>, message: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret key`: `Vec<i64>`
- `message`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 513*

---

## crypto_pbkdf2_sha256

```fides
pub fn crypto_pbkdf2_sha256(@secret password: Vec<i64>, salt: Vec<i64>, iterations: i64, len: i64) -> Vec<i64> {
```

**Parameters:**

- `@secret password`: `Vec<i64>`
- `salt`: `Vec<i64>`
- `iterations`: `i64`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 568*

---

## crypto_constant_time_eq

Compare two byte arrays in constant time (prevents timing attacks).
Returns true if equal, false otherwise. Always processes all bytes.

```fides
pub fn crypto_constant_time_eq(a: Vec<i64>, b: Vec<i64>) -> bool {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 616*

---

## CRYPTO_APPROVED_ALGORITHMS:

```fides
pub const CRYPTO_APPROVED_ALGORITHMS: str = "SHA-256, HMAC-SHA256, PBKDF2-HMAC-SHA256";
```

*Defined at line 649*

---

## CRYPTO_REJECTED_ALGORITHMS:

```fides
pub const CRYPTO_REJECTED_ALGORITHMS: str = "MD5, SHA-1, custom";
```

*Defined at line 650*

---

## crypto_random_u64

64 random bits.

```fides
pub fn crypto_random_u64() -> i64 {
```

**Returns:** `i64`

*Defined at line 657*

---

## crypto_random_bytes

`n` random bytes, each 0..255.

```fides
pub fn crypto_random_bytes(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 662*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
