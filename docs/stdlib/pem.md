[All modules](index.md)

# pem -- Fides Documentation

## Contents

- [PemBlock](#PemBlock) (struct)
- [PEM_OID_RSA:](#PEM_OID_RSA:) (const)
- [PEM_OID_RSA_PSS:](#PEM_OID_RSA_PSS:) (const)
- [PEM_OID_EC:](#PEM_OID_EC:) (const)
- [PEM_OID_ED25519:](#PEM_OID_ED25519:) (const)
- [PEM_OID_X25519:](#PEM_OID_X25519:) (const)
- [PEM_OID_ED448:](#PEM_OID_ED448:) (const)
- [PEM_OID_P256:](#PEM_OID_P256:) (const)
- [PEM_OID_P384:](#PEM_OID_P384:) (const)
- [PEM_OID_P521:](#PEM_OID_P521:) (const)
- [PEM_OID_PBES2:](#PEM_OID_PBES2:) (const)
- [PEM_OID_PKCS7_DATA:](#PEM_OID_PKCS7_DATA:) (const)
- [PEM_OID_PKCS7_SIGNED:](#PEM_OID_PKCS7_SIGNED:) (const)
- [pem_oid_name](#pem_oid_name) (function)
- [pem_encode](#pem_encode) (function)
- [pem_decode_all](#pem_decode_all) (function)
- [pem_decode](#pem_decode) (function)
- [pem_is_legacy_encrypted](#pem_is_legacy_encrypted) (function)
- [Spki](#Spki) (struct)
- [pem_alg_id](#pem_alg_id) (function)
- [spki_parse_at](#spki_parse_at) (function)
- [spki_parse](#spki_parse) (function)
- [spki_encode](#spki_encode) (function)
- [Pkcs8Key](#Pkcs8Key) (struct)
- [pkcs8_parse](#pkcs8_parse) (function)
- [pkcs8_encode_ed25519](#pkcs8_encode_ed25519) (function)
- [pkcs8_encode_x25519](#pkcs8_encode_x25519) (function)
- [pkcs8_encode](#pkcs8_encode) (function)
- [pkcs8_encode_ec](#pkcs8_encode_ec) (function)
- [pkcs8_encryption_alg](#pkcs8_encryption_alg) (function)
- [pem_is_pkcs12](#pem_is_pkcs12) (function)

## PemBlock

One armored block: its label, any RFC 1421 header lines (joined with
"\n"), and the decoded bytes.

```fides
pub struct PemBlock {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `label` | `str` |
| `headers` | `str` |
| `der` | `Vec<i64>` |

*Defined at line 32*

---

## PEM_OID_RSA:

```fides
pub const PEM_OID_RSA: str = "1.2.840.113549.1.1.1";
```

*Defined at line 39*

---

## PEM_OID_RSA_PSS:

```fides
pub const PEM_OID_RSA_PSS: str = "1.2.840.113549.1.1.10";
```

*Defined at line 40*

---

## PEM_OID_EC:

```fides
pub const PEM_OID_EC: str = "1.2.840.10045.2.1";
```

*Defined at line 41*

---

## PEM_OID_ED25519:

```fides
pub const PEM_OID_ED25519: str = "1.3.101.112";
```

*Defined at line 42*

---

## PEM_OID_X25519:

```fides
pub const PEM_OID_X25519: str = "1.3.101.110";
```

*Defined at line 43*

---

## PEM_OID_ED448:

```fides
pub const PEM_OID_ED448: str = "1.3.101.113";
```

*Defined at line 44*

---

## PEM_OID_P256:

```fides
pub const PEM_OID_P256: str = "1.2.840.10045.3.1.7";
```

*Defined at line 45*

---

## PEM_OID_P384:

```fides
pub const PEM_OID_P384: str = "1.3.132.0.34";
```

*Defined at line 46*

---

## PEM_OID_P521:

```fides
pub const PEM_OID_P521: str = "1.3.132.0.35";
```

*Defined at line 47*

---

## PEM_OID_PBES2:

```fides
pub const PEM_OID_PBES2: str = "1.2.840.113549.1.5.13";
```

*Defined at line 48*

---

## PEM_OID_PKCS7_DATA:

```fides
pub const PEM_OID_PKCS7_DATA: str = "1.2.840.113549.1.7.1";
```

*Defined at line 49*

---

## PEM_OID_PKCS7_SIGNED:

```fides
pub const PEM_OID_PKCS7_SIGNED: str = "1.2.840.113549.1.7.2";
```

*Defined at line 50*

---

## pem_oid_name

A readable name of a key or curve OID ("" when unknown).

```fides
pub fn pem_oid_name(oid: str) -> str {
```

**Parameters:**

- `oid`: `str`

**Returns:** `str`

*Defined at line 53*

---

## pem_encode

Armor DER bytes: base64 in lines of 64 characters, "\n" line ends.

```fides
pub fn pem_encode(label: str, der: Vec<i64>) -> str {
```

**Parameters:**

- `label`: `str`
- `der`: `Vec<i64>`

**Returns:** `str`

*Defined at line 90*

---

## pem_decode_all

Every block in a text, in order. Text outside blocks is ignored (as
RFC 7468 allows explanatory text); a malformed block is returned with
ok false so the caller can see that something was there.

```fides
pub fn pem_decode_all(text: str) -> Vec<PemBlock> {
```

**Parameters:**

- `text`: `str`

**Returns:** `Vec<PemBlock>`

*Defined at line 119*

---

## pem_decode

The first well-formed block with the given label ("" = any label).

```fides
pub fn pem_decode(text: str, label: str) -> PemBlock {
```

**Parameters:**

- `text`: `str`
- `label`: `str`

**Returns:** `PemBlock`

*Defined at line 171*

---

## pem_is_legacy_encrypted

Whether a block uses legacy RFC 1421 encryption (OpenSSL's
"Proc-Type: 4,ENCRYPTED" header).

```fides
pub fn pem_is_legacy_encrypted(b: PemBlock) -> bool {
```

**Parameters:**

- `b`: `PemBlock`

**Returns:** `bool`

*Defined at line 186*

---

## Spki

SubjectPublicKeyInfo: algorithm OID, parameter OID (the curve of an EC
key; "" otherwise) and the public key bytes (the BIT STRING content:
RSAPublicKey DER, a SEC1 point, or 32 raw bytes for Ed25519/X25519).

```fides
pub struct Spki {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `alg` | `str` |
| `params` | `str` |
| `key` | `Vec<i64>` |

*Defined at line 195*

---

## pem_alg_id

The OID of an AlgorithmIdentifier SEQUENCE and its OID parameter ("" if
absent or NULL or not an OID), as two strings joined by a space.

```fides
pub fn pem_alg_id(buf: &Vec<i64>, t: Asn1Tlv) -> str {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `str`

*Defined at line 210*

---

## spki_parse_at

Parse a SubjectPublicKeyInfo element inside a larger buffer.

```fides
pub fn spki_parse_at(buf: &Vec<i64>, t: Asn1Tlv) -> Spki {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `t`: `Asn1Tlv`

**Returns:** `Spki`

*Defined at line 242*

---

## spki_parse

Parse a DER SubjectPublicKeyInfo (a "PUBLIC KEY" PEM block).

```fides
pub fn spki_parse(der: Vec<i64>) -> Spki {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `Spki`

*Defined at line 263*

---

## spki_encode

Encode a SubjectPublicKeyInfo (params "" for none; RSA keys get the
NULL parameter RFC 3279 requires).

```fides
pub fn spki_encode(alg: str, params: str, key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `alg`: `str`
- `params`: `str`
- `key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 280*

---

## Pkcs8Key

A PKCS#8 private key. `key` is the privateKey OCTET STRING content;
`secret` is the raw secret it wraps (Ed25519/X25519 seed, EC scalar;
for RSA the RSAPrivateKey DER, same as key); `public_key` is the public
key when the container carries one (OneAsymmetricKey [1], or the SEC1
publicKey of an EC key).

```fides
pub struct Pkcs8Key {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `version` | `i64` |
| `alg` | `str` |
| `alg_name` | `str` |
| `params` | `str` |
| `key` | `Vec<i64>` |
| `secret` | `Vec<i64>` |
| `public_key` | `Vec<i64>` |

*Defined at line 295*

---

## pkcs8_parse

Parse a DER PrivateKeyInfo / OneAsymmetricKey ("PRIVATE KEY" block).

```fides
pub fn pkcs8_parse(der: Vec<i64>) -> Pkcs8Key {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `Pkcs8Key`

*Defined at line 313*

---

## pkcs8_encode_ed25519

PKCS#8 v1 (RFC 8410) for a 32-byte Ed25519 seed.

```fides
pub fn pkcs8_encode_ed25519(seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 389*

---

## pkcs8_encode_x25519

PKCS#8 v1 for a 32-byte X25519 private key.

```fides
pub fn pkcs8_encode_x25519(secret: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `secret`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 394*

---

## pkcs8_encode

PKCS#8 v1 around an algorithm's private key bytes (`private_key` is
the content of the privateKey OCTET STRING).

```fides
pub fn pkcs8_encode(alg: str, params: str, private_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `alg`: `str`
- `params`: `str`
- `private_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 400*

---

## pkcs8_encode_ec

PKCS#8 for an EC key: the SEC1 ECPrivateKey with its public point.

```fides
pub fn pkcs8_encode_ec(curve_oid: str, scalar: Vec<i64>, public_point: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `curve_oid`: `str`
- `scalar`: `Vec<i64>`
- `public_point`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 409*

---

## pkcs8_encryption_alg

The encryption scheme OID of an EncryptedPrivateKeyInfo ("ENCRYPTED
PRIVATE KEY" block), or "" when the bytes are not one.

```fides
pub fn pkcs8_encryption_alg(der: Vec<i64>) -> str {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `str`

*Defined at line 421*

---

## pem_is_pkcs12

Whether DER bytes have the shape of a PKCS#12 PFX: SEQUENCE { INTEGER
3, ContentInfo { data or signedData OID, [0] ... }, ... }.

```fides
pub fn pem_is_pkcs12(der: Vec<i64>) -> bool {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 435*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
