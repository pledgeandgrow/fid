[All modules](index.md)

# rsa -- Fides Documentation

## Contents

- [RsaPublicKey](#RsaPublicKey) (struct)
- [RsaPrivateKey](#RsaPrivateKey) (struct)
- [RsaPlain](#RsaPlain) (struct)
- [rsa_os2ip](#rsa_os2ip) (function)
- [rsa_int_bytes](#rsa_int_bytes) (function)
- [rsa_i2osp](#rsa_i2osp) (function)
- [rsa_bit_len](#rsa_bit_len) (function)
- [rsa_mod](#rsa_mod) (function)
- [rsa_modinv](#rsa_modinv) (function)
- [rsa_gcd](#rsa_gcd) (function)
- [RSA_LIMB_BITS:](#RSA_LIMB_BITS:) (const)
- [RSA_LIMB_MASK:](#RSA_LIMB_MASK:) (const)
- [RsaMont](#RsaMont) (struct)
- [rsa_limbs_from_bytes](#rsa_limbs_from_bytes) (function)
- [rsa_limbs_to_bytes](#rsa_limbs_to_bytes) (function)
- [rsa_limbs_cmp](#rsa_limbs_cmp) (function)
- [rsa_mont_new](#rsa_mont_new) (function)
- [rsa_mont_mul](#rsa_mont_mul) (function)
- [rsa_mont_add](#rsa_mont_add) (function)
- [rsa_mont_sub](#rsa_mont_sub) (function)
- [rsa_mont_to](#rsa_mont_to) (function)
- [rsa_mont_from](#rsa_mont_from) (function)
- [rsa_mont_pow](#rsa_mont_pow) (function)
- [rsa_modexp](#rsa_modexp) (function)
- [rsa_public](#rsa_public) (function)
- [rsa_public_from_der](#rsa_public_from_der) (function)
- [rsa_public_from_spki](#rsa_public_from_spki) (function)
- [rsa_public_of](#rsa_public_of) (function)
- [rsa_private_from_der](#rsa_private_from_der) (function)
- [rsa_private_from_pkcs8](#rsa_private_from_pkcs8) (function)
- [rsa_public_to_der](#rsa_public_to_der) (function)
- [rsa_private_to_der](#rsa_private_to_der) (function)
- [rsa_private_from_primes](#rsa_private_from_primes) (function)
- [rsa_ep](#rsa_ep) (function)
- [rsa_dp](#rsa_dp) (function)
- [rsa_private_check](#rsa_private_check) (function)
- [rsa_ct_eq](#rsa_ct_eq) (function)
- [rsa_mgf1](#rsa_mgf1) (function)
- [rsa_digest_info_prefix](#rsa_digest_info_prefix) (function)
- [rsa_emsa_pkcs1](#rsa_emsa_pkcs1) (function)
- [rsa_pkcs1_sign](#rsa_pkcs1_sign) (function)
- [rsa_pkcs1_verify](#rsa_pkcs1_verify) (function)
- [rsa_emsa_pss_encode](#rsa_emsa_pss_encode) (function)
- [rsa_emsa_pss_verify](#rsa_emsa_pss_verify) (function)
- [rsa_pss_sign](#rsa_pss_sign) (function)
- [rsa_pss_verify](#rsa_pss_verify) (function)
- [rsa_oaep_encrypt](#rsa_oaep_encrypt) (function)
- [rsa_oaep_decrypt](#rsa_oaep_decrypt) (function)
- [rsa_is_probable_prime](#rsa_is_probable_prime) (function)
- [rsa_random_prime](#rsa_random_prime) (function)
- [rsa_generate](#rsa_generate) (function)

## RsaPublicKey

```fides
pub struct RsaPublicKey {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `n` | `BigInt` |
| `e` | `BigInt` |
| `k` | `i64` |
| `bits` | `i64` |

*Defined at line 44*

---

## RsaPrivateKey

```fides
pub struct RsaPrivateKey {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `n` | `BigInt` |
| `e` | `BigInt` |
| `d` | `BigInt` |
| `p` | `BigInt` |
| `q` | `BigInt` |
| `dp` | `BigInt` |
| `dq` | `BigInt` |
| `qinv` | `BigInt` |
| `k` | `i64` |
| `bits` | `i64` |

*Defined at line 52*

---

## RsaPlain

A decryption result: ok false on any failure.

```fides
pub struct RsaPlain {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `data` | `Vec<i64>` |

*Defined at line 67*

---

## rsa_os2ip

OS2IP: big-endian bytes to a non-negative integer.

```fides
pub fn rsa_os2ip(b: Vec<i64>) -> BigInt {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `BigInt`

*Defined at line 75*

---

## rsa_int_bytes

The minimal big-endian bytes of a non-negative integer (empty for 0).

```fides
pub fn rsa_int_bytes(x: BigInt) -> Vec<i64> {
```

**Parameters:**

- `x`: `BigInt`

**Returns:** `Vec<i64>`

*Defined at line 96*

---

## rsa_i2osp

I2OSP: a non-negative integer as exactly `len` big-endian bytes. An
integer too large for `len` bytes gives an empty vector.

```fides
pub fn rsa_i2osp(x: BigInt, len: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `BigInt`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 122*

---

## rsa_bit_len

The number of bits of a non-negative integer.

```fides
pub fn rsa_bit_len(x: BigInt) -> i64 {
```

**Parameters:**

- `x`: `BigInt`

**Returns:** `i64`

*Defined at line 142*

---

## rsa_mod

a mod m in 0..m-1 (bigint_mod keeps the sign of a).

```fides
pub fn rsa_mod(a: BigInt, m: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `m`: `BigInt`

**Returns:** `BigInt`

*Defined at line 157*

---

## rsa_modinv

The inverse of a modulo m (extended Euclid); zero when none exists.

```fides
pub fn rsa_modinv(a: BigInt, m: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `m`: `BigInt`

**Returns:** `BigInt`

*Defined at line 166*

---

## rsa_gcd

Greatest common divisor of non-negative integers.

```fides
pub fn rsa_gcd(a: BigInt, b: BigInt) -> BigInt {
```

**Parameters:**

- `a`: `BigInt`
- `b`: `BigInt`

**Returns:** `BigInt`

*Defined at line 187*

---

## RSA_LIMB_BITS:

```fides
pub const RSA_LIMB_BITS: i64 = 28;
```

*Defined at line 207*

---

## RSA_LIMB_MASK:

```fides
pub const RSA_LIMB_MASK: i64 = 0xfffffff;
```

*Defined at line 208*

---

## RsaMont

```fides
pub struct RsaMont {
```

**Fields:**

| Name | Type |
|------|------|
| `s` | `i64` |
| `n` | `Vec<i64>` |
| `n0` | `i64` |
| `one` | `Vec<i64>` |
| `r2` | `Vec<i64>` |

*Defined at line 210*

---

## rsa_limbs_from_bytes

Big-endian bytes to s limbs (the high bytes beyond s limbs are dropped).

```fides
pub fn rsa_limbs_from_bytes(b: Vec<i64>, s: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `Vec<i64>`
- `s`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 219*

---

## rsa_limbs_to_bytes

Limbs to exactly `len` big-endian bytes.

```fides
pub fn rsa_limbs_to_bytes(x: &Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `x`: `&Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 245*

---

## rsa_limbs_cmp

Compare s-limb numbers: -1, 0 or 1.

```fides
pub fn rsa_limbs_cmp(a: &Vec<i64>, b: &Vec<i64>, s: i64) -> i64 {
```

**Parameters:**

- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`
- `s`: `i64`

**Returns:** `i64`

*Defined at line 272*

---

## rsa_mont_new

A context for an odd modulus given as big-endian bytes.

```fides
pub fn rsa_mont_new(modulus: Vec<i64>) -> RsaMont {
```

**Parameters:**

- `modulus`: `Vec<i64>`

**Returns:** `RsaMont`

*Defined at line 304*

---

## rsa_mont_mul

Montgomery product a b R^-1 mod n of values below n.

```fides
pub fn rsa_mont_mul(c: &RsaMont, a: &Vec<i64>, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 362*

---

## rsa_mont_add

(a + b) mod n for values below n.

```fides
pub fn rsa_mont_add(c: &RsaMont, a: &Vec<i64>, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 412*

---

## rsa_mont_sub

(a - b) mod n for values below n.

```fides
pub fn rsa_mont_sub(c: &RsaMont, a: &Vec<i64>, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 429*

---

## rsa_mont_to

x (below n, plain) into Montgomery form.

```fides
pub fn rsa_mont_to(c: &RsaMont, x: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `x`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 458*

---

## rsa_mont_from

A Montgomery-form value back to plain form.

```fides
pub fn rsa_mont_from(c: &RsaMont, x: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `x`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 463*

---

## rsa_mont_pow

a^e in Montgomery form (a in Montgomery form, e big-endian bytes),
with a fixed 4-bit window.

```fides
pub fn rsa_mont_pow(c: &RsaMont, a: &Vec<i64>, e: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RsaMont`
- `a`: `&Vec<i64>`
- `e`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 476*

---

## rsa_modexp

base^e mod m for an odd modulus m, through Montgomery arithmetic.

```fides
pub fn rsa_modexp(base: BigInt, e: BigInt, m: BigInt) -> BigInt {
```

**Parameters:**

- `base`: `BigInt`
- `e`: `BigInt`
- `m`: `BigInt`

**Returns:** `BigInt`

*Defined at line 519*

---

## rsa_public

A public key from n and e.

```fides
pub fn rsa_public(n: BigInt, e: BigInt) -> RsaPublicKey {
```

**Parameters:**

- `n`: `BigInt`
- `e`: `BigInt`

**Returns:** `RsaPublicKey`

*Defined at line 546*

---

## rsa_public_from_der

RSAPublicKey ::= SEQUENCE { modulus INTEGER, publicExponent INTEGER }.

```fides
pub fn rsa_public_from_der(der: Vec<i64>) -> RsaPublicKey {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `RsaPublicKey`

*Defined at line 558*

---

## rsa_public_from_spki

A public key from a SubjectPublicKeyInfo ("PUBLIC KEY" PEM block).

```fides
pub fn rsa_public_from_spki(der: Vec<i64>) -> RsaPublicKey {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `RsaPublicKey`

*Defined at line 574*

---

## rsa_public_of

The public half of a private key.

```fides
pub fn rsa_public_of(sk: RsaPrivateKey) -> RsaPublicKey {
```

**Parameters:**

- `sk`: `RsaPrivateKey`

**Returns:** `RsaPublicKey`

*Defined at line 583*

---

## rsa_private_from_der

RSAPrivateKey (RFC 8017 A.1.2): version 0 with n, e, d, p, q, dP, dQ,
qInv ("RSA PRIVATE KEY" PEM block). Multi-prime keys are refused.

```fides
pub fn rsa_private_from_der(der: Vec<i64>) -> RsaPrivateKey {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `RsaPrivateKey`

*Defined at line 589*

---

## rsa_private_from_pkcs8

A private key from PKCS#8 ("PRIVATE KEY" PEM block).

```fides
pub fn rsa_private_from_pkcs8(der: Vec<i64>) -> RsaPrivateKey {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `RsaPrivateKey`

*Defined at line 616*

---

## rsa_public_to_der

RSAPublicKey DER.

```fides
pub fn rsa_public_to_der(pk: RsaPublicKey) -> Vec<i64> {
```

**Parameters:**

- `pk`: `RsaPublicKey`

**Returns:** `Vec<i64>`

*Defined at line 629*

---

## rsa_private_to_der

RSAPrivateKey DER.

```fides
pub fn rsa_private_to_der(sk: RsaPrivateKey) -> Vec<i64> {
```

**Parameters:**

- `sk`: `RsaPrivateKey`

**Returns:** `Vec<i64>`

*Defined at line 634*

---

## rsa_private_from_primes

A private key from its primes and public exponent: d, dP, dQ and qInv
are computed (d modulo lcm(p-1, q-1), as FIPS 186-5 asks).

```fides
pub fn rsa_private_from_primes(p_in: BigInt, q_in: BigInt, e: BigInt) -> RsaPrivateKey {
```

**Parameters:**

- `p_in`: `BigInt`
- `q_in`: `BigInt`
- `e`: `BigInt`

**Returns:** `RsaPrivateKey`

*Defined at line 649*

---

## rsa_ep

RSAEP / RSAVP1: m^e mod n.

```fides
pub fn rsa_ep(pk: RsaPublicKey, m: BigInt) -> BigInt {
```

**Parameters:**

- `pk`: `RsaPublicKey`
- `m`: `BigInt`

**Returns:** `BigInt`

*Defined at line 674*

---

## rsa_dp

RSADP / RSASP1 with the CRT: m1 = c^dP mod p, m2 = c^dQ mod q,
h = qInv (m1 - m2) mod p, m = m2 + q h.

```fides
pub fn rsa_dp(sk: RsaPrivateKey, c: BigInt) -> BigInt {
```

**Parameters:**

- `sk`: `RsaPrivateKey`
- `c`: `BigInt`

**Returns:** `BigInt`

*Defined at line 680*

---

## rsa_private_check

Whether the private key's parts agree: n = p q, and a test value comes
back through the CRT private operation and the public one.

```fides
pub fn rsa_private_check(sk: RsaPrivateKey) -> bool {
```

**Parameters:**

- `sk`: `RsaPrivateKey`

**Returns:** `bool`

*Defined at line 689*

---

## rsa_ct_eq

Equality of byte vectors without an early exit.

```fides
pub fn rsa_ct_eq(a: Vec<i64>, b: Vec<i64>) -> bool {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 726*

---

## rsa_mgf1

MGF1 (RFC 8017 B.2.1) with hash `h`.

```fides
pub fn rsa_mgf1(h: i64, seed: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `seed`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 740*

---

## rsa_digest_info_prefix

The DER DigestInfo prefix of EMSA-PKCS1-v1_5 for a hash.

```fides
pub fn rsa_digest_info_prefix(h: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 761*

---

## rsa_emsa_pkcs1

EMSA-PKCS1-v1_5: 00 01 ff..ff 00 DigestInfo; empty when k is too small.

```fides
pub fn rsa_emsa_pkcs1(h: i64, msg: Vec<i64>, k: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `msg`: `Vec<i64>`
- `k`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 784*

---

## rsa_pkcs1_sign

Sign `msg` with RSASSA-PKCS1-v1_5: k bytes (empty on a bad key).

```fides
pub fn rsa_pkcs1_sign(sk: RsaPrivateKey, h: i64, msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `sk`: `RsaPrivateKey`
- `h`: `i64`
- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 802*

---

## rsa_pkcs1_verify

Verify an RSASSA-PKCS1-v1_5 signature.

```fides
pub fn rsa_pkcs1_verify(pk: RsaPublicKey, h: i64, msg: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `pk`: `RsaPublicKey`
- `h`: `i64`
- `msg`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 811*

---

## rsa_emsa_pss_encode

EMSA-PSS-ENCODE of a message digest with the given salt; emBits is
modBits - 1. Empty when the encoding does not fit.

```fides
pub fn rsa_emsa_pss_encode(h: i64, mhash: Vec<i64>, salt: Vec<i64>, em_bits: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `i64`
- `mhash`: `Vec<i64>`
- `salt`: `Vec<i64>`
- `em_bits`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 828*

---

## rsa_emsa_pss_verify

EMSA-PSS-VERIFY. `salt_len` -1 accepts any salt length (it is read
back from the padding).

```fides
pub fn rsa_emsa_pss_verify(h: i64, mhash: Vec<i64>, em: Vec<i64>, em_bits: i64, salt_len: i64) -> bool {
```

**Parameters:**

- `h`: `i64`
- `mhash`: `Vec<i64>`
- `em`: `Vec<i64>`
- `em_bits`: `i64`
- `salt_len`: `i64`

**Returns:** `bool`

*Defined at line 865*

---

## rsa_pss_sign

Sign with RSASSA-PSS (MGF1 with the same hash). The salt is the
caller's random bytes, usually as long as the hash.

```fides
pub fn rsa_pss_sign(sk: RsaPrivateKey, h: i64, msg: Vec<i64>, salt: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `sk`: `RsaPrivateKey`
- `h`: `i64`
- `msg`: `Vec<i64>`
- `salt`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 915*

---

## rsa_pss_verify

Verify an RSASSA-PSS signature; salt_len -1 accepts any length.

```fides
pub fn rsa_pss_verify(pk: RsaPublicKey, h: i64, msg: Vec<i64>, sig: Vec<i64>, salt_len: i64) -> bool {
```

**Parameters:**

- `pk`: `RsaPublicKey`
- `h`: `i64`
- `msg`: `Vec<i64>`
- `sig`: `Vec<i64>`
- `salt_len`: `i64`

**Returns:** `bool`

*Defined at line 927*

---

## rsa_oaep_encrypt

Encrypt with RSAES-OAEP. `seed` is hLen random bytes from the caller.
Empty when the message is too long (k - 2 hLen - 2 bytes at most).

```fides
pub fn rsa_oaep_encrypt(pk: RsaPublicKey, h: i64, msg: Vec<i64>, label: Vec<i64>, seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pk`: `RsaPublicKey`
- `h`: `i64`
- `msg`: `Vec<i64>`
- `label`: `Vec<i64>`
- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 949*

---

## rsa_oaep_decrypt

Decrypt RSAES-OAEP. Every failure looks the same (ok false, no data).

```fides
pub fn rsa_oaep_decrypt(sk: RsaPrivateKey, h: i64, ct: Vec<i64>, label: Vec<i64>) -> RsaPlain {
```

**Parameters:**

- `sk`: `RsaPrivateKey`
- `h`: `i64`
- `ct`: `Vec<i64>`
- `label`: `Vec<i64>`

**Returns:** `RsaPlain`

*Defined at line 983*

---

## rsa_is_probable_prime

Miller-Rabin with `rounds` bases drawn from rand (n odd and > 3),
after trial division by the primes below 600.

```fides
pub fn rsa_is_probable_prime(n: BigInt, rounds: i64, rand: fn(i64) -> Vec<i64>) -> bool {
```

**Parameters:**

- `n`: `BigInt`
- `rounds`: `i64`
- `rand`: `fn(i64) -> Vec<i64>`

**Returns:** `bool`

*Defined at line 1069*

---

## rsa_random_prime

A random prime of exactly `bits` bits (top two bits set, so a product
of two has 2*bits bits) with p mod e != 1.

```fides
pub fn rsa_random_prime(bits: i64, e: BigInt, rand: fn(i64) -> Vec<i64>) -> BigInt {
```

**Parameters:**

- `bits`: `i64`
- `e`: `BigInt`
- `rand`: `fn(i64) -> Vec<i64>`

**Returns:** `BigInt`

*Defined at line 1137*

---

## rsa_generate

A new key of `bits` bits (even, at least 128) with exponent e
(65537 in practice).

```fides
pub fn rsa_generate(bits: i64, e: i64, rand: fn(i64) -> Vec<i64>) -> RsaPrivateKey {
```

**Parameters:**

- `bits`: `i64`
- `e`: `i64`
- `rand`: `fn(i64) -> Vec<i64>`

**Returns:** `RsaPrivateKey`

*Defined at line 1154*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
