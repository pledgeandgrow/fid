[All modules](index.md)

# jose -- Fides Documentation

## Contents

- [JoseBytes](#JoseBytes) (struct)
- [Jwk](#Jwk) (struct)
- [JwsParts](#JwsParts) (struct)
- [JwsVerified](#JwsVerified) (struct)
- [JwtOptions](#JwtOptions) (struct)
- [JwtResult](#JwtResult) (struct)
- [JweDecrypted](#JweDecrypted) (struct)
- [jose_b64url_encode](#jose_b64url_encode) (function)
- [jose_b64url_str](#jose_b64url_str) (function)
- [jose_b64url_decode](#jose_b64url_decode) (function)
- [jose_b64url_bytes](#jose_b64url_bytes) (function)
- [jwk_empty](#jwk_empty) (function)
- [jwk_parse](#jwk_parse) (function)
- [jwks_parse](#jwks_parse) (function)
- [jwks_find](#jwks_find) (function)
- [jwk_oct](#jwk_oct) (function)
- [jose_ec_len](#jose_ec_len) (function)
- [jwk_ec_private](#jwk_ec_private) (function)
- [jwk_ec_public](#jwk_ec_public) (function)
- [jwk_p256_private](#jwk_p256_private) (function)
- [jwk_p256_public](#jwk_p256_public) (function)
- [jwk_ed25519_private](#jwk_ed25519_private) (function)
- [jwk_x25519_private](#jwk_x25519_private) (function)
- [jwk_rsa_public](#jwk_rsa_public) (function)
- [jwk_public](#jwk_public) (function)
- [jwk_to_json](#jwk_to_json) (function)
- [jwks_to_json](#jwks_to_json) (function)
- [jwk_thumbprint_input](#jwk_thumbprint_input) (function)
- [jwk_thumbprint](#jwk_thumbprint) (function)
- [jwk_thumbprint_b64](#jwk_thumbprint_b64) (function)
- [jwk_ec_point](#jwk_ec_point) (function)
- [jws_alg_fits_key](#jws_alg_fits_key) (function)
- [jose_alg_allowed](#jose_alg_allowed) (function)
- [jws_raw_sign](#jws_raw_sign) (function)
- [jws_raw_verify](#jws_raw_verify) (function)
- [jws_sign_salted](#jws_sign_salted) (function)
- [jws_sign](#jws_sign) (function)
- [jws_header](#jws_header) (function)
- [jws_decode](#jws_decode) (function)
- [jws_verify](#jws_verify) (function)
- [jws_verify_jwks](#jws_verify_jwks) (function)
- [jwt_options](#jwt_options) (function)
- [jwt_check_claims](#jwt_check_claims) (function)
- [jwt_verify](#jwt_verify) (function)
- [jwt_verify_jwks](#jwt_verify_jwks) (function)
- [jwt_sign](#jwt_sign) (function)
- [jwe_enc_key_len](#jwe_enc_key_len) (function)
- [jwe_concat_kdf](#jwe_concat_kdf) (function)
- [jwe_ecdh](#jwe_ecdh) (function)
- [jwe_encrypt_dir](#jwe_encrypt_dir) (function)
- [jwe_encrypt_ecdh_es](#jwe_encrypt_ecdh_es) (function)
- [jose_aes_kw_wrap](#jose_aes_kw_wrap) (function)
- [jose_aes_kw_unwrap](#jose_aes_kw_unwrap) (function)
- [jwe_kw_key_len](#jwe_kw_key_len) (function)
- [jwe_encrypt_kw](#jwe_encrypt_kw) (function)
- [jwe_encrypt_rsa_oaep](#jwe_encrypt_rsa_oaep) (function)
- [jwe_decrypt](#jwe_decrypt) (function)

## JoseBytes

Bytes with a validity flag (base64url decoding, key agreement).

```fides
pub struct JoseBytes {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `data` | `Vec<i64>` |

*Defined at line 70*

---

## Jwk

A JSON Web Key. Byte fields hold the decoded base64url members;
absent members are empty. `kty` is "oct", "EC", "OKP" or "RSA".

```fides
pub struct Jwk {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kty` | `str` |
| `kid` | `str` |
| `alg` | `str` |
| `usage` | `str` |
| `crv` | `str` |
| `k` | `Vec<i64>` |
| `x` | `Vec<i64>` |
| `y` | `Vec<i64>` |
| `d` | `Vec<i64>` |
| `n` | `Vec<i64>` |
| `e` | `Vec<i64>` |
| `p` | `Vec<i64>` |
| `q` | `Vec<i64>` |

*Defined at line 77*

---

## JwsParts

The parts of a compact JWS before verification.

```fides
pub struct JwsParts {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `header` | `str` |
| `payload` | `Vec<i64>` |
| `signature` | `Vec<i64>` |
| `signing_input` | `Vec<i64>` |

*Defined at line 95*

---

## JwsVerified

The result of verifying a JWS.

```fides
pub struct JwsVerified {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `alg` | `str` |
| `header` | `str` |
| `payload` | `Vec<i64>` |

*Defined at line 104*

---

## JwtOptions

What a JWT check needs: the current time, the allowed clock skew, and
the expected issuer and audience ("" = do not check the issuer; an
empty audience accepts only tokens without an "aud" claim).

```fides
pub struct JwtOptions {
```

**Fields:**

| Name | Type |
|------|------|
| `now` | `i64` |
| `leeway` | `i64` |
| `iss` | `str` |
| `aud` | `str` |
| `require_exp` | `bool` |

*Defined at line 115*

---

## JwtResult

The result of verifying a JWT: the claims set (JSON text) when ok.

```fides
pub struct JwtResult {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `header` | `str` |
| `claims` | `str` |

*Defined at line 124*

---

## JweDecrypted

The result of decrypting a JWE.

```fides
pub struct JweDecrypted {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `header` | `str` |
| `plain` | `Vec<i64>` |

*Defined at line 132*

---

## jose_b64url_encode

base64url of bytes, no padding.

```fides
pub fn jose_b64url_encode(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 228*

---

## jose_b64url_str

base64url of a string's bytes.

```fides
pub fn jose_b64url_str(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 254*

---

## jose_b64url_decode

Strict base64url decoding: only the URL-safe alphabet, no padding, no
length of 1 mod 4, and the unused low bits of the last character zero.

```fides
pub fn jose_b64url_decode(s: str) -> JoseBytes {
```

**Parameters:**

- `s`: `str`

**Returns:** `JoseBytes`

*Defined at line 260*

---

## jose_b64url_bytes

base64url text to bytes; empty on any error.

```fides
pub fn jose_b64url_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 298*

---

## jwk_empty

An empty, invalid key.

```fides
pub fn jwk_empty() -> Jwk {
```

**Returns:** `Jwk`

*Defined at line 305*

---

## jwk_parse

Parse one JWK object. `ok` is false when a member is malformed or a
member the key type requires is missing or has the wrong size.

```fides
pub fn jwk_parse(json: str) -> Jwk {
```

**Parameters:**

- `json`: `str`

**Returns:** `Jwk`

*Defined at line 324*

---

## jwks_parse

Parse a JWK Set {"keys": [...]}. Keys that do not parse are skipped
(RFC 7517 §5: unknown key types are ignored).

```fides
pub fn jwks_parse(json: str) -> Vec<Jwk> {
```

**Parameters:**

- `json`: `str`

**Returns:** `Vec<Jwk>`

*Defined at line 378*

---

## jwks_find

Index of the key with this "kid" in a set, -1 when none.

```fides
pub fn jwks_find(keys: &Vec<Jwk>, kid: str) -> i64 {
```

**Parameters:**

- `keys`: `&Vec<Jwk>`
- `kid`: `str`

**Returns:** `i64`

*Defined at line 397*

---

## jwk_oct

A symmetric key.

```fides
pub fn jwk_oct(key: Vec<i64>) -> Jwk {
```

**Parameters:**

- `key`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 410*

---

## jose_ec_len

The coordinate length of a JOSE curve name: 32 (P-256), 48 (P-384),
66 (P-521), 0 for any other.

```fides
pub fn jose_ec_len(crv: str) -> i64 {
```

**Parameters:**

- `crv`: `str`

**Returns:** `i64`

*Defined at line 420*

---

## jwk_ec_private

An EC key pair ("P-256", "P-384" or "P-521") from its private scalar.

```fides
pub fn jwk_ec_private(crv: str, d: Vec<i64>) -> Jwk {
```

**Parameters:**

- `crv`: `str`
- `d`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 444*

---

## jwk_ec_public

An EC public key from an uncompressed SEC 1 point 04 || X || Y.

```fides
pub fn jwk_ec_public(crv: str, point: Vec<i64>) -> Jwk {
```

**Parameters:**

- `crv`: `str`
- `point`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 465*

---

## jwk_p256_private

A P-256 key pair from its 32-byte private scalar.

```fides
pub fn jwk_p256_private(d: Vec<i64>) -> Jwk {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 480*

---

## jwk_p256_public

A P-256 public key from an uncompressed SEC 1 point 04 || X || Y.

```fides
pub fn jwk_p256_public(point: Vec<i64>) -> Jwk {
```

**Parameters:**

- `point`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 485*

---

## jwk_ed25519_private

An Ed25519 key pair from its 32-byte seed.

```fides
pub fn jwk_ed25519_private(seed: Vec<i64>) -> Jwk {
```

**Parameters:**

- `seed`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 490*

---

## jwk_x25519_private

An X25519 key pair from its 32-byte secret.

```fides
pub fn jwk_x25519_private(secret: Vec<i64>) -> Jwk {
```

**Parameters:**

- `secret`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 504*

---

## jwk_rsa_public

An RSA public key from big-endian n and e.

```fides
pub fn jwk_rsa_public(n: Vec<i64>, e: Vec<i64>) -> Jwk {
```

**Parameters:**

- `n`: `Vec<i64>`
- `e`: `Vec<i64>`

**Returns:** `Jwk`

*Defined at line 518*

---

## jwk_public

The public part of a key (private members removed; oct keys have no
public part and come back invalid).

```fides
pub fn jwk_public(key: &Jwk) -> Jwk {
```

**Parameters:**

- `key`: `&Jwk`

**Returns:** `Jwk`

*Defined at line 529*

---

## jwk_to_json

The key as JSON. Private members are written only when `private` is
true. Member order: kty, crv, the key members, then kid, use, alg.

```fides
pub fn jwk_to_json(key: &Jwk, private: bool) -> str {
```

**Parameters:**

- `key`: `&Jwk`
- `private`: `bool`

**Returns:** `str`

*Defined at line 551*

---

## jwks_to_json

A JWK Set holding the public parts of these keys.

```fides
pub fn jwks_to_json(keys: &Vec<Jwk>) -> str {
```

**Parameters:**

- `keys`: `&Vec<Jwk>`

**Returns:** `str`

*Defined at line 587*

---

## jwk_thumbprint_input

The RFC 7638 thumbprint input: the required public members in
lexicographic order, no white space.

```fides
pub fn jwk_thumbprint_input(key: &Jwk) -> str {
```

**Parameters:**

- `key`: `&Jwk`

**Returns:** `str`

*Defined at line 603*

---

## jwk_thumbprint

SHA-256 JWK thumbprint (RFC 7638 §3).

```fides
pub fn jwk_thumbprint(key: &Jwk) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Jwk`

**Returns:** `Vec<i64>`

*Defined at line 617*

---

## jwk_thumbprint_b64

The thumbprint as base64url (what ACME and "kid" values use).

```fides
pub fn jwk_thumbprint_b64(key: &Jwk) -> str {
```

**Parameters:**

- `key`: `&Jwk`

**Returns:** `str`

*Defined at line 622*

---

## jwk_ec_point

The uncompressed SEC 1 point of an EC key.

```fides
pub fn jwk_ec_point(key: &Jwk) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Jwk`

**Returns:** `Vec<i64>`

*Defined at line 635*

---

## jws_alg_fits_key

True when `alg` may be used with this key (type, curve, size).

```fides
pub fn jws_alg_fits_key(alg: str, key: &Jwk) -> bool {
```

**Parameters:**

- `alg`: `str`
- `key`: `&Jwk`

**Returns:** `bool`

*Defined at line 672*

---

## jose_alg_allowed

True when `alg` is one of the comma-separated names in `allowed`.

```fides
pub fn jose_alg_allowed(alg: str, allowed: str) -> bool {
```

**Parameters:**

- `alg`: `str`
- `allowed`: `str`

**Returns:** `bool`

*Defined at line 696*

---

## jws_raw_sign

Sign the bytes `input` with `alg`; empty when the key does not fit.
`salt` is used by PS256/PS512 only (hash-length bytes).

```fides
pub fn jws_raw_sign(alg: str, key: &Jwk, input: Vec<i64>, salt: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `alg`: `str`
- `key`: `&Jwk`
- `input`: `Vec<i64>`
- `salt`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 713*

---

## jws_raw_verify

Check a signature over `input`.

```fides
pub fn jws_raw_verify(alg: str, key: &Jwk, input: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `alg`: `str`
- `key`: `&Jwk`
- `input`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 748*

---

## jws_sign_salted

Compact JWS with the protected header given as exact JSON text (its
bytes are what is signed) and a PSS salt; "" when the header has no
usable "alg" or the key does not fit it.

```fides
pub fn jws_sign_salted(header_json: str, payload: Vec<i64>, key: &Jwk, salt: Vec<i64>) -> str {
```

**Parameters:**

- `header_json`: `str`
- `payload`: `Vec<i64>`
- `key`: `&Jwk`
- `salt`: `Vec<i64>`

**Returns:** `str`

*Defined at line 780*

---

## jws_sign

Compact JWS; PSS salts come from crypto_random_bytes.

```fides
pub fn jws_sign(header_json: str, payload: Vec<i64>, key: &Jwk) -> str {
```

**Parameters:**

- `header_json`: `str`
- `payload`: `Vec<i64>`
- `key`: `&Jwk`

**Returns:** `str`

*Defined at line 794*

---

## jws_header

A protected header {"alg":..,"typ":..,"kid":..}; empty typ/kid are left out.

```fides
pub fn jws_header(alg: str, typ: str, kid: str) -> str {
```

**Parameters:**

- `alg`: `str`
- `typ`: `str`
- `kid`: `str`

**Returns:** `str`

*Defined at line 804*

---

## jws_decode

Split and decode a compact JWS without checking the signature.

```fides
pub fn jws_decode(token: str) -> JwsParts {
```

**Parameters:**

- `token`: `str`

**Returns:** `JwsParts`

*Defined at line 822*

---

## jws_verify

Verify a compact JWS with one key, accepting only the algorithms in
`allowed` (comma-separated, e.g. "ES256,EdDSA").

```fides
pub fn jws_verify(token: str, key: &Jwk, allowed: str) -> JwsVerified {
```

**Parameters:**

- `token`: `str`
- `key`: `&Jwk`
- `allowed`: `str`

**Returns:** `JwsVerified`

*Defined at line 869*

---

## jws_verify_jwks

Verify against a key set: the key named by the header's "kid", or,
without a kid, each key in turn.

```fides
pub fn jws_verify_jwks(token: str, keys: &Vec<Jwk>, allowed: str) -> JwsVerified {
```

**Parameters:**

- `token`: `str`
- `keys`: `&Vec<Jwk>`
- `allowed`: `str`

**Returns:** `JwsVerified`

*Defined at line 879*

---

## jwt_options

Options: the time now, 60 s leeway, issuer and audience, exp required.

```fides
pub fn jwt_options(now: i64, iss: str, aud: str) -> JwtOptions {
```

**Parameters:**

- `now`: `i64`
- `iss`: `str`
- `aud`: `str`

**Returns:** `JwtOptions`

*Defined at line 907*

---

## jwt_check_claims

Check the registered claims; "" when they pass, else the reason.

```fides
pub fn jwt_check_claims(claims: str, o: &JwtOptions) -> str {
```

**Parameters:**

- `claims`: `str`
- `o`: `&JwtOptions`

**Returns:** `str`

*Defined at line 949*

---

## jwt_verify

Verify a JWT's signature with one key, then its claims.

```fides
pub fn jwt_verify(token: str, key: &Jwk, allowed: str, o: &JwtOptions) -> JwtResult {
```

**Parameters:**

- `token`: `str`
- `key`: `&Jwk`
- `allowed`: `str`
- `o`: `&JwtOptions`

**Returns:** `JwtResult`

*Defined at line 1013*

---

## jwt_verify_jwks

Verify a JWT against a key set, then its claims.

```fides
pub fn jwt_verify_jwks(token: str, keys: &Vec<Jwk>, allowed: str, o: &JwtOptions) -> JwtResult {
```

**Parameters:**

- `token`: `str`
- `keys`: `&Vec<Jwk>`
- `allowed`: `str`
- `o`: `&JwtOptions`

**Returns:** `JwtResult`

*Defined at line 1018*

---

## jwt_sign

Sign a claims set (JSON text) as a JWT with header {"alg","typ":"JWT","kid"}.

```fides
pub fn jwt_sign(claims_json: str, key: &Jwk, alg: str) -> str {
```

**Parameters:**

- `claims_json`: `str`
- `key`: `&Jwk`
- `alg`: `str`

**Returns:** `str`

*Defined at line 1023*

---

## jwe_enc_key_len

Content-encryption key length in bytes of an "enc" value, 0 if unknown.

```fides
pub fn jwe_enc_key_len(enc: str) -> i64 {
```

**Parameters:**

- `enc`: `str`

**Returns:** `i64`

*Defined at line 1030*

---

## jwe_concat_kdf

The Concat KDF of NIST SP 800-56A §5.8.1 as RFC 7518 §4.6.2 uses it:
SHA-256 rounds over counter || Z || OtherInfo, where OtherInfo is
AlgorithmID || PartyUInfo || PartyVInfo (each length-prefixed) ||
keydatalen in bits.

```fides
pub fn jwe_concat_kdf(z: Vec<i64>, alg_id: str, apu: Vec<i64>, apv: Vec<i64>, key_len: i64) -> Vec<i64> {
```

**Parameters:**

- `z`: `Vec<i64>`
- `alg_id`: `str`
- `apu`: `Vec<i64>`
- `apv`: `Vec<i64>`
- `key_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1047*

---

## jwe_ecdh

The ECDH shared secret Z between a private key and a peer public key
of the same curve (P-256, P-384, P-521 or X25519).

```fides
pub fn jwe_ecdh(mine: &Jwk, peer: &Jwk) -> JoseBytes {
```

**Parameters:**

- `mine`: `&Jwk`
- `peer`: `&Jwk`

**Returns:** `JoseBytes`

*Defined at line 1069*

---

## jwe_encrypt_dir

Compact JWE with a direct key: the header (exact JSON text) must say
"alg":"dir" and an "enc" whose key length matches the oct key. `iv` is
12 random bytes (never reuse one with the same key).

```fides
pub fn jwe_encrypt_dir(header_json: str, key: &Jwk, plain: Vec<i64>, iv: Vec<i64>) -> str {
```

**Parameters:**

- `header_json`: `str`
- `key`: `&Jwk`
- `plain`: `Vec<i64>`
- `iv`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1117*

---

## jwe_encrypt_ecdh_es

Compact JWE with ECDH-ES direct key agreement to `recipient` (EC P-256,
P-384, P-521 or OKP X25519 public key). `eph_secret` is the ephemeral
private key (random, the curve's length), `apu`/`apv` the optional
party infos.

```fides
pub fn jwe_encrypt_ecdh_es(recipient: &Jwk, enc: str, plain: Vec<i64>, eph_secret: Vec<i64>, iv: Vec<i64>, apu: Vec<i64>, apv: Vec<i64>) -> str {
```

**Parameters:**

- `recipient`: `&Jwk`
- `enc`: `str`
- `plain`: `Vec<i64>`
- `eph_secret`: `Vec<i64>`
- `iv`: `Vec<i64>`
- `apu`: `Vec<i64>`
- `apv`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1134*

---

## jose_aes_kw_wrap

```fides
pub fn jose_aes_kw_wrap(@secret kek: Vec<i64>, @secret data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `@secret kek`: `Vec<i64>`
- `@secret data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1272*

---

## jose_aes_kw_unwrap

```fides
pub fn jose_aes_kw_unwrap(@secret kek: Vec<i64>, wrapped: Vec<i64>) -> JoseBytes {
```

**Parameters:**

- `@secret kek`: `Vec<i64>`
- `wrapped`: `Vec<i64>`

**Returns:** `JoseBytes`

*Defined at line 1308*

---

## jwe_kw_key_len

KEK length of an AES key-wrap "alg", 0 when not one.

```fides
pub fn jwe_kw_key_len(alg: str) -> i64 {
```

**Parameters:**

- `alg`: `str`

**Returns:** `i64`

*Defined at line 1357*

---

## jwe_encrypt_kw

Compact JWE with AES key wrap: the header (exact JSON text) names
"alg" A128KW/A192KW/A256KW and an "enc"; `key` is the oct KEK of that
size, `cek` the random content key for "enc", `iv` 12 random bytes.

```fides
pub fn jwe_encrypt_kw(header_json: str, key: &Jwk, plain: Vec<i64>, cek: Vec<i64>, iv: Vec<i64>) -> str {
```

**Parameters:**

- `header_json`: `str`
- `key`: `&Jwk`
- `plain`: `Vec<i64>`
- `cek`: `Vec<i64>`
- `iv`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1384*

---

## jwe_encrypt_rsa_oaep

Compact JWE with RSAES-OAEP: the header names "alg" RSA-OAEP or
RSA-OAEP-256 and an "enc"; `key` is the recipient's RSA public key,
`cek` the random content key, `iv` 12 random bytes, `seed` the OAEP
seed (hash-length random bytes).

```fides
pub fn jwe_encrypt_rsa_oaep(header_json: str, key: &Jwk, plain: Vec<i64>, cek: Vec<i64>, iv: Vec<i64>, seed: Vec<i64>) -> str {
```

**Parameters:**

- `header_json`: `str`
- `key`: `&Jwk`
- `plain`: `Vec<i64>`
- `cek`: `Vec<i64>`
- `iv`: `Vec<i64>`
- `seed`: `Vec<i64>`

**Returns:** `str`

*Defined at line 1406*

---

## jwe_decrypt

Decrypt a compact JWE with `key` (an oct key for "dir" and the AES
key-wrap algorithms, the recipient's private EC/OKP key for "ECDH-ES",
the recipient's RSA private key with p and q for RSA-OAEP), accepting
only the "alg" values in `allowed`.

```fides
pub fn jwe_decrypt(token: str, key: &Jwk, allowed: str) -> JweDecrypted {
```

**Parameters:**

- `token`: `str`
- `key`: `&Jwk`
- `allowed`: `str`

**Returns:** `JweDecrypted`

*Defined at line 1438*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
