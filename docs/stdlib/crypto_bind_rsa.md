# RSA through the system library (`crypto_bind`, `crypto_bind_rsa`)

Competitive roadmap SL18 / release J5. RSA is done by the operating
system's reviewed library, not by Fides code: BCrypt (CNG) on Windows
(`stdlib/crypto_bind.fd`), libcrypto (OpenSSL 1.1.1 or 3.x, EVP_PKEY) on
Linux and macOS (`stdlib/crypto_bind_unix.fd`). `use crypto_bind` picks the
module of the target. Both take keys in the same format and give the same
answers.

## Keys

A key is a byte `Vec` in BCrypt's blob format, on every system
(`stdlib/crypto_bind_rsa.fd`, plain Fides):

| Function | |
|---|---|
| `cb_rsa_public_blob(&n, &e)` | public blob ("RSA1") from big-endian n and e |
| `cb_rsa_private_blob(&n, &e, &d, &p, &q, &dp, &dq, &qi)` | full private blob ("RSA3"); a secret |
| `cb_rsa_public_of(&blob)` | the public blob of any key blob |
| `cb_rsa_bits(&blob)`, `cb_rsa_modulus_len(&blob)` | key size in bits, in bytes |
| `cbr_blob_part(&blob, k)` | part k: 0 e, 1 n, 2 p, 3 q, 4 dp, 5 dq, 6 qi, 7 d |
| `cb_rsa_generate(bits, &mut priv, &mut pub)` | a new key (e = 65537); 0 on success |

## Operations

Hash names are `"SHA1"`, `"SHA256"`, `"SHA384"`, `"SHA512"`; the functions
take the digest, not the message. MGF1 uses the same hash. A signature or
ciphertext must be exactly as long as the modulus.

```fides
use crypto_bind; use crypto_bind_rsa; use crypto;

val h = crypto_sha256("message");
val sig = cb_rsa_sign_pkcs1(& priv, "SHA256", & h);          // RSASSA-PKCS1-v1_5
val ok = cb_rsa_verify_pkcs1(& pub, "SHA256", & h, & sig);
val pss = cb_rsa_sign_pss(& priv, "SHA256", & h, 32);         // RSASSA-PSS, salt 32
val ok2 = cb_rsa_verify_pss(& pub, "SHA256", & h, & pss, 32);
val ct = cb_rsa_encrypt_oaep(& pub, "SHA256", & msg, & label); // RSAES-OAEP
var out: Vec<i64> = Vec::new();
if cb_rsa_decrypt_oaep(& priv, "SHA256", & ct, & label, & mut out) { ... }
```

Signing and decryption take the private blob as `@secret` and return an
audited release (`@declassify`), as the other crypto_bind functions do.

## Tests

`tests/bootstrap/test_crypto_bind_rsa.fd` (C and native builds; `fid run`
cannot call the libraries): Project Wycheproof vectors for PKCS#1 v1.5
verify (52), PSS verify (53) and OAEP decrypt (37, with labels), valid and
invalid; deterministic PKCS#1 v1.5 signatures with the Wycheproof OAEP key
checked against an independent computation; PSS and OAEP round trips; a
generated 2048-bit key. Nov 17: the same output, byte for byte, through
BCrypt (Windows 11, `fid ccompile`) and OpenSSL 3.6.2 (WSL).

`scripts/verify/crypto_bind_unix_check.fd` checks the rest of the OpenSSL
twin (AES-GCM, PBKDF2, ECDSA P-256 and its blobs).
