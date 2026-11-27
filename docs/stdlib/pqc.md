[All modules](index.md)

# pqc -- Fides Documentation

## Contents

- [keccak_f1600](#keccak_f1600) (function)
- [keccak_sponge](#keccak_sponge) (function)
- [sha3_256](#sha3_256) (function)
- [sha3_512](#sha3_512) (function)
- [shake128](#shake128) (function)
- [shake256](#shake256) (function)
- [MLKEM_Q:](#MLKEM_Q:) (const)
- [MLKEM_K:](#MLKEM_K:) (const)
- [MlkemTables](#MlkemTables) (struct)
- [MlkemKeyPair](#MlkemKeyPair) (struct)
- [MlkemEncap](#MlkemEncap) (struct)
- [mlkem768_keygen](#mlkem768_keygen) (function)
- [mlkem768_ek_valid](#mlkem768_ek_valid) (function)
- [mlkem768_encaps](#mlkem768_encaps) (function)
- [mlkem768_decaps](#mlkem768_decaps) (function)
- [MLDSA_Q:](#MLDSA_Q:) (const)
- [MldsaParams](#MldsaParams) (struct)
- [mldsa44_params](#mldsa44_params) (function)
- [mldsa65_params](#mldsa65_params) (function)
- [MldsaKeyPair](#MldsaKeyPair) (struct)
- [mldsa_keygen](#mldsa_keygen) (function)
- [mldsa_sign_internal](#mldsa_sign_internal) (function)
- [mldsa_verify_internal](#mldsa_verify_internal) (function)
- [mldsa_sign](#mldsa_sign) (function)
- [mldsa_sign_deterministic](#mldsa_sign_deterministic) (function)
- [mldsa_verify](#mldsa_verify) (function)

## keccak_f1600

```fides
pub fn keccak_f1600(a: &mut Vec<i64>) {
```

**Parameters:**

- `a`: `&mut Vec<i64>`

*Defined at line 110*

---

## keccak_sponge

The sponge: absorb msg with the domain/padding byte `ds` at rate
`rate` bytes, then squeeze `out_len` bytes.

```fides
pub fn keccak_sponge(rate: i64, ds: i64, msg: Vec<i64>, out_len: i64) -> Vec<i64> {
```

**Parameters:**

- `rate`: `i64`
- `ds`: `i64`
- `msg`: `Vec<i64>`
- `out_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 169*

---

## sha3_256

SHA3-256.

```fides
pub fn sha3_256(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 213*

---

## sha3_512

SHA3-512.

```fides
pub fn sha3_512(msg: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 217*

---

## shake128

SHAKE128 with `n` output bytes.

```fides
pub fn shake128(msg: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 221*

---

## shake256

SHAKE256 with `n` output bytes.

```fides
pub fn shake256(msg: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `msg`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 225*

---

## MLKEM_Q:

```fides
pub const MLKEM_Q: i64 = 3329;
```

*Defined at line 231*

---

## MLKEM_K:

```fides
pub const MLKEM_K: i64 = 3;
```

*Defined at line 232*

---

## MlkemTables

Precomputed tables, built once per operation.

```fides
pub struct MlkemTables {
```

**Fields:**

| Name | Type |
|------|------|
| `zetas` | `Vec<i64>` |
| `gammas` | `Vec<i64>` |

*Defined at line 279*

---

## MlkemKeyPair

A key pair: ek (1184 bytes) and dk (2400 bytes).

```fides
pub struct MlkemKeyPair {
```

**Fields:**

| Name | Type |
|------|------|
| `ek` | `Vec<i64>` |
| `dk` | `Vec<i64>` |

*Defined at line 628*

---

## MlkemEncap

The result of encapsulation: ciphertext (1088 bytes) and key (32).

```fides
pub struct MlkemEncap {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ct` | `Vec<i64>` |
| `key` | `Vec<i64>` |

*Defined at line 634*

---

## mlkem768_keygen

ML-KEM.KeyGen_internal(d, z) from the 64-byte seed d || z.

```fides
pub fn mlkem768_keygen(seed: Vec<i64>) -> MlkemKeyPair {
```

**Parameters:**

- `seed`: `Vec<i64>`

**Returns:** `MlkemKeyPair`

*Defined at line 641*

---

## mlkem768_ek_valid

The encapsulation key check of FIPS 203 §7.2: the right length and
every coefficient below q (decoding then encoding changes nothing).

```fides
pub fn mlkem768_ek_valid(ek: Vec<i64>) -> bool {
```

**Parameters:**

- `ek`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 694*

---

## mlkem768_encaps

ML-KEM.Encaps_internal(ek, m) with the caller's 32 random bytes m.

```fides
pub fn mlkem768_encaps(ek: Vec<i64>, m: Vec<i64>) -> MlkemEncap {
```

**Parameters:**

- `ek`: `Vec<i64>`
- `m`: `Vec<i64>`

**Returns:** `MlkemEncap`

*Defined at line 715*

---

## mlkem768_decaps

ML-KEM.Decaps_internal(dk, c): the shared key, or the implicit-rejection
key J(z || c) when the ciphertext does not re-encrypt to itself.
Empty only when the lengths are wrong.

```fides
pub fn mlkem768_decaps(dk: Vec<i64>, c: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dk`: `Vec<i64>`
- `c`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 734*

---

## MLDSA_Q:

```fides
pub const MLDSA_Q: i64 = 8380417;
```

*Defined at line 776*

---

## MldsaParams

The parameter set (FIPS 204 Table 1).

```fides
pub struct MldsaParams {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `k` | `i64` |
| `l` | `i64` |
| `eta` | `i64` |
| `tau` | `i64` |
| `beta` | `i64` |
| `gamma1` | `i64` |
| `gamma2` | `i64` |
| `omega` | `i64` |
| `ctilde` | `i64` |
| `pk_len` | `i64` |
| `sk_len` | `i64` |
| `sig_len` | `i64` |

*Defined at line 779*

---

## mldsa44_params

ML-DSA-44 (security category 2).

```fides
pub fn mldsa44_params() -> MldsaParams {
```

**Returns:** `MldsaParams`

*Defined at line 796*

---

## mldsa65_params

ML-DSA-65 (security category 3).

```fides
pub fn mldsa65_params() -> MldsaParams {
```

**Returns:** `MldsaParams`

*Defined at line 803*

---

## MldsaKeyPair

A key pair: pk and sk as FIPS 204 encodes them.

```fides
pub struct MldsaKeyPair {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pk` | `Vec<i64>` |
| `sk` | `Vec<i64>` |

*Defined at line 810*

---

## mldsa_keygen

KeyGen_internal (Algorithm 6) from the 32-byte seed xi
(crypto_random_bytes(32) in real use).

```fides
pub fn mldsa_keygen(p: &MldsaParams, xi: Vec<i64>) -> MldsaKeyPair {
```

**Parameters:**

- `p`: `&MldsaParams`
- `xi`: `Vec<i64>`

**Returns:** `MldsaKeyPair`

*Defined at line 1247*

---

## mldsa_sign_internal

Sign_internal (Algorithm 7) of the already-formatted message M' with
the 32 bytes rnd; empty when sk or rnd has the wrong length.

```fides
pub fn mldsa_sign_internal(p: &MldsaParams, sk: Vec<i64>, mp: Vec<i64>, rnd: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `p`: `&MldsaParams`
- `sk`: `Vec<i64>`
- `mp`: `Vec<i64>`
- `rnd`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1341*

---

## mldsa_verify_internal

Verify_internal (Algorithm 8) of a signature over M'.

```fides
pub fn mldsa_verify_internal(p: &MldsaParams, pk: Vec<i64>, mp: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `p`: `&MldsaParams`
- `pk`: `Vec<i64>`
- `mp`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1554*

---

## mldsa_sign

ML-DSA.Sign (Algorithm 2), hedged: rnd is 32 fresh random bytes
(crypto_random_bytes(32) in real use). ctx is at most 255 bytes (often
empty). Empty when an input has the wrong size.

```fides
pub fn mldsa_sign(p: &MldsaParams, sk: Vec<i64>, msg: Vec<i64>, ctx: Vec<i64>, rnd: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `p`: `&MldsaParams`
- `sk`: `Vec<i64>`
- `msg`: `Vec<i64>`
- `ctx`: `Vec<i64>`
- `rnd`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1634*

---

## mldsa_sign_deterministic

ML-DSA.Sign, deterministic variant (rnd = 32 zero bytes).

```fides
pub fn mldsa_sign_deterministic(p: &MldsaParams, sk: Vec<i64>, msg: Vec<i64>, ctx: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `p`: `&MldsaParams`
- `sk`: `Vec<i64>`
- `msg`: `Vec<i64>`
- `ctx`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1642*

---

## mldsa_verify

ML-DSA.Verify (Algorithm 3).

```fides
pub fn mldsa_verify(p: &MldsaParams, pk: Vec<i64>, msg: Vec<i64>, ctx: Vec<i64>, sig: Vec<i64>) -> bool {
```

**Parameters:**

- `p`: `&MldsaParams`
- `pk`: `Vec<i64>`
- `msg`: `Vec<i64>`
- `ctx`: `Vec<i64>`
- `sig`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1653*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
