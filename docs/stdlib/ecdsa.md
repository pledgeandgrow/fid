[All modules](index.md)

# ecdsa -- Fides Documentation

## Contents

- [EcCurve](#EcCurve) (struct)
- [EcPoint](#EcPoint) (struct)
- [ec_p256](#ec_p256) (function)
- [ec_p384](#ec_p384) (function)
- [ec_p521](#ec_p521) (function)
- [ec_curve_by_oid](#ec_curve_by_oid) (function)
- [EC_SHA384:](#EC_SHA384:) (const)
- [ec_sha384](#ec_sha384) (function)
- [ec_hash](#ec_hash) (function)
- [ec_hash_len](#ec_hash_len) (function)
- [ec_hmac](#ec_hmac) (function)
- [ec_infinity](#ec_infinity) (function)
- [ec_generator](#ec_generator) (function)
- [ec_double](#ec_double) (function)
- [ec_point_add](#ec_point_add) (function)
- [ec_scalar_mul](#ec_scalar_mul) (function)
- [ec_double_mul](#ec_double_mul) (function)
- [ec_affine_bytes](#ec_affine_bytes) (function)
- [ec_point_decode](#ec_point_decode) (function)
- [ec_point_encode](#ec_point_encode) (function)
- [ecdsa_rfc6979_k](#ecdsa_rfc6979_k) (function)
- [ec_public_key](#ec_public_key) (function)
- [ecdsa_sign_with_k](#ecdsa_sign_with_k) (function)
- [ecdsa_sign](#ecdsa_sign) (function)
- [ecdsa_verify](#ecdsa_verify) (function)
- [ecdh_shared](#ecdh_shared) (function)
- [ecdsa_sig_to_der](#ecdsa_sig_to_der) (function)
- [ecdsa_sig_from_der](#ecdsa_sig_from_der) (function)
- [ecdsa_verify_der](#ecdsa_verify_der) (function)

## EcCurve

A short-Weierstrass curve y^2 = x^3 - 3x + b over GF(p).

```fides
pub struct EcCurve {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `oid` | `str` |
| `len` | `i64` |
| `qbits` | `i64` |
| `fp` | `RsaMont` |
| `fq` | `RsaMont` |
| `p` | `Vec<i64>` |
| `n` | `Vec<i64>` |
| `b` | `Vec<i64>` |
| `gx` | `Vec<i64>` |
| `gy` | `Vec<i64>` |

*Defined at line 49*

---

## EcPoint

A point in Jacobian coordinates, Montgomery form; inf = the identity.

```fides
pub struct EcPoint {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `Vec<i64>` |
| `y` | `Vec<i64>` |
| `z` | `Vec<i64>` |
| `inf` | `bool` |

*Defined at line 64*

---

## ec_p256

NIST P-256 (SEC 2 secp256r1).

```fides
pub fn ec_p256() -> EcCurve {
```

**Returns:** `EcCurve`

*Defined at line 85*

---

## ec_p384

NIST P-384 (SEC 2 secp384r1).

```fides
pub fn ec_p384() -> EcCurve {
```

**Returns:** `EcCurve`

*Defined at line 90*

---

## ec_p521

NIST P-521 (SEC 2 secp521r1): 66-byte values, a 521-bit order.

```fides
pub fn ec_p521() -> EcCurve {
```

**Returns:** `EcCurve`

*Defined at line 95*

---

## ec_curve_by_oid

The curve named by an OID ("" name when unknown, then len is 0).

```fides
pub fn ec_curve_by_oid(oid: str) -> EcCurve {
```

**Parameters:**

- `oid`: `str`

**Returns:** `EcCurve`

*Defined at line 100*

---

## EC_SHA384:

```fides
pub const EC_SHA384: i64 = 4;
```

*Defined at line 120*

---

## ec_sha384

SHA-384 of bytes: the 48-byte digest.

```fides
pub fn ec_sha384(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 277*

---

## ec_hash

The digest of msg with hash h (a kdf.fd id or EC_SHA384).

```fides
pub fn ec_hash(h: i64, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 336*

---

## ec_hash_len

Digest length in bytes.

```fides
pub fn ec_hash_len(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 344*

---

## ec_hmac

HMAC (RFC 2104) with hash h, SHA-384 included (RFC 4231).

```fides
pub fn ec_hmac(h: i64, key: Vec<i64>, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `key`: `Vec<i64>`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 352*

---

## ec_infinity

The identity.

```fides
pub fn ec_infinity(c: &EcCurve) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`

**Returns:** `EcPoint`

*Defined at line 505*

---

## ec_generator

The base point G.

```fides
pub fn ec_generator(c: &EcCurve) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`

**Returns:** `EcPoint`

*Defined at line 512*

---

## ec_double

2P (dbl-2001-b, a = -3).

```fides
pub fn ec_double(c: &EcCurve, p: &EcPoint) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`
- `p`: `&EcPoint`

**Returns:** `EcPoint`

*Defined at line 609*

---

## ec_point_add

P + Q (add-2007-bl), doubling or the identity where it must.

```fides
pub fn ec_point_add(c: &EcCurve, p: &EcPoint, q: &EcPoint) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`
- `p`: `&EcPoint`
- `q`: `&EcPoint`

**Returns:** `EcPoint`

*Defined at line 650*

---

## ec_scalar_mul

k P for a scalar given as big-endian bytes (any length).

Constant operation pattern: four doublings, a 16-way masked table
lookup and a complete addition per nibble, whatever the scalar's bits —
the number of point operations and the field work in each do not
depend on `k`. (The table itself is built from `p`, public in every
use: the generator or the peer's key.)

```fides
pub fn ec_scalar_mul(c: &EcCurve, k: Vec<i64>, p: &EcPoint) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`
- `k`: `Vec<i64>`
- `p`: `&EcPoint`

**Returns:** `EcPoint`

*Defined at line 706*

---

## ec_double_mul

a G + b Q (Shamir's trick), scalars as len-byte big-endian values.

```fides
pub fn ec_double_mul(c: &EcCurve, a: Vec<i64>, b: Vec<i64>, q: &EcPoint) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`
- `a`: `Vec<i64>`
- `b`: `Vec<i64>`
- `q`: `&EcPoint`

**Returns:** `EcPoint`

*Defined at line 749*

---

## ec_affine_bytes

The affine coordinates of a point as X || Y bytes (empty for the
identity).

```fides
pub fn ec_affine_bytes(c: &EcCurve, p: &EcPoint) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `p`: `&EcPoint`

**Returns:** `Vec<i64>`

*Defined at line 788*

---

## ec_point_decode

Decode a SEC 1 point (04 || X || Y, or 02/03 || X); inf true when the
bytes are not a point on the curve.

```fides
pub fn ec_point_decode(c: &EcCurve, data: Vec<i64>) -> EcPoint {
```

**Parameters:**

- `c`: `&EcCurve`
- `data`: `Vec<i64>`

**Returns:** `EcPoint`

*Defined at line 810*

---

## ec_point_encode

SEC 1 encoding of a point: uncompressed (04 || X || Y) or compressed.

```fides
pub fn ec_point_encode(c: &EcCurve, p: &EcPoint, compressed: bool) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `p`: `&EcPoint`
- `compressed`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 848*

---

## ecdsa_rfc6979_k

The RFC 6979 nonce for private key d and message hash h1.

```fides
pub fn ecdsa_rfc6979_k(c: &EcCurve, hash: i64, d: Vec<i64>, h1: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `hash`: `i64`
- `d`: `Vec<i64>`
- `h1`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 922*

---

## ec_public_key

The public point d G of a private scalar (len bytes, 1..n-1) as an
uncompressed SEC 1 point; empty for an invalid scalar.

```fides
pub fn ec_public_key(c: &EcCurve, d: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `d`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 964*

---

## ecdsa_sign_with_k

ECDSA signature r || s (2 len bytes) of msg with the nonce k given;
empty when k or d is invalid or r or s comes out zero.

```fides
pub fn ecdsa_sign_with_k(c: &EcCurve, d: Vec<i64>, hash: i64, msg: Vec<i64>, k: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `d`: `Vec<i64>`
- `hash`: `i64`
- `msg`: `Vec<i64>`
- `k`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 975*

---

## ecdsa_sign

Deterministic ECDSA (RFC 6979): r || s.

```fides
pub fn ecdsa_sign(c: &EcCurve, d: Vec<i64>, hash: i64, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `d`: `Vec<i64>`
- `hash`: `i64`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1008*

---

## ecdsa_verify

Verify an r || s signature against a SEC 1 public key.

```fides
pub fn ecdsa_verify(c: &EcCurve, public_key: Vec<i64>, hash: i64, msg: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&EcCurve`
- `public_key`: `Vec<i64>`
- `hash`: `i64`
- `msg`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1014*

---

## ecdh_shared

ECDH: the x coordinate of d Q (len bytes); empty when the peer key is
not a valid point or the result is the identity.

```fides
pub fn ecdh_shared(c: &EcCurve, d: Vec<i64>, peer_public: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&EcCurve`
- `d`: `Vec<i64>`
- `peer_public`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1046*

---

## ecdsa_sig_to_der

r || s to the DER Ecdsa-Sig-Value SEQUENCE { r INTEGER, s INTEGER }.

```fides
pub fn ecdsa_sig_to_der(sig: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `sig`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1065*

---

## ecdsa_sig_from_der

A DER signature to r || s of 2 len bytes; empty when malformed.

```fides
pub fn ecdsa_sig_from_der(der: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `der`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1084*

---

## ecdsa_verify_der

Verify a DER-encoded signature (as X.509 and TLS carry them).

```fides
pub fn ecdsa_verify_der(c: &EcCurve, public_key: Vec<i64>, hash: i64, msg: Vec<i64>, der: Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&EcCurve`
- `public_key`: `Vec<i64>`
- `hash`: `i64`
- `msg`: `Vec<i64>`
- `der`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1108*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
