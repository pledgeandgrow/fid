[All modules](index.md)

# acme -- Fides Documentation

## Contents

- [ACME_ST_DIRECTORY:](#ACME_ST_DIRECTORY:) (const)
- [ACME_ST_ACCOUNT:](#ACME_ST_ACCOUNT:) (const)
- [ACME_ST_ORDER:](#ACME_ST_ORDER:) (const)
- [ACME_ST_AUTHZ:](#ACME_ST_AUTHZ:) (const)
- [ACME_ST_PUBLISH:](#ACME_ST_PUBLISH:) (const)
- [ACME_ST_CHALLENGE:](#ACME_ST_CHALLENGE:) (const)
- [ACME_ST_POLL_AUTHZ:](#ACME_ST_POLL_AUTHZ:) (const)
- [ACME_ST_FINALIZE:](#ACME_ST_FINALIZE:) (const)
- [ACME_ST_POLL_ORDER:](#ACME_ST_POLL_ORDER:) (const)
- [ACME_ST_CERT:](#ACME_ST_CERT:) (const)
- [ACME_ST_DONE:](#ACME_ST_DONE:) (const)
- [ACME_ST_ERROR:](#ACME_ST_ERROR:) (const)
- [AcmeDirectory](#AcmeDirectory) (struct)
- [AcmeHttpRequest](#AcmeHttpRequest) (struct)
- [AcmeProblem](#AcmeProblem) (struct)
- [AcmeOrder](#AcmeOrder) (struct)
- [AcmeAuthz](#AcmeAuthz) (struct)
- [AcmeCsrInfo](#AcmeCsrInfo) (struct)
- [AcmeClient](#AcmeClient) (struct)
- [acme_parse_directory](#acme_parse_directory) (function)
- [acme_parse_problem](#acme_parse_problem) (function)
- [acme_parse_order](#acme_parse_order) (function)
- [acme_parse_authz](#acme_parse_authz) (function)
- [acme_jws](#acme_jws) (function)
- [acme_eab](#acme_eab) (function)
- [acme_new_account_payload](#acme_new_account_payload) (function)
- [acme_new_order_payload](#acme_new_order_payload) (function)
- [acme_finalize_payload](#acme_finalize_payload) (function)
- [acme_revoke_payload](#acme_revoke_payload) (function)
- [acme_deactivate_payload](#acme_deactivate_payload) (function)
- [acme_key_authorization](#acme_key_authorization) (function)
- [acme_http01_path](#acme_http01_path) (function)
- [acme_dns01_name](#acme_dns01_name) (function)
- [acme_dns01_value](#acme_dns01_value) (function)
- [acme_csr](#acme_csr) (function)
- [acme_csr_pem](#acme_csr_pem) (function)
- [acme_csr_check](#acme_csr_check) (function)
- [acme_client_new](#acme_client_new) (function)
- [acme_set_csr_key](#acme_set_csr_key) (function)
- [acme_set_eab](#acme_set_eab) (function)
- [acme_next_request](#acme_next_request) (function)
- [acme_challenge_published](#acme_challenge_published) (function)
- [acme_handle_response](#acme_handle_response) (function)

## ACME_ST_DIRECTORY:

```fides
pub const ACME_ST_DIRECTORY: i64 = 0;
```

*Defined at line 54*

---

## ACME_ST_ACCOUNT:

```fides
pub const ACME_ST_ACCOUNT: i64 = 1;
```

*Defined at line 55*

---

## ACME_ST_ORDER:

```fides
pub const ACME_ST_ORDER: i64 = 2;
```

*Defined at line 56*

---

## ACME_ST_AUTHZ:

```fides
pub const ACME_ST_AUTHZ: i64 = 3;
```

*Defined at line 57*

---

## ACME_ST_PUBLISH:

```fides
pub const ACME_ST_PUBLISH: i64 = 4;
```

*Defined at line 58*

---

## ACME_ST_CHALLENGE:

```fides
pub const ACME_ST_CHALLENGE: i64 = 5;
```

*Defined at line 59*

---

## ACME_ST_POLL_AUTHZ:

```fides
pub const ACME_ST_POLL_AUTHZ: i64 = 6;
```

*Defined at line 60*

---

## ACME_ST_FINALIZE:

```fides
pub const ACME_ST_FINALIZE: i64 = 7;
```

*Defined at line 61*

---

## ACME_ST_POLL_ORDER:

```fides
pub const ACME_ST_POLL_ORDER: i64 = 8;
```

*Defined at line 62*

---

## ACME_ST_CERT:

```fides
pub const ACME_ST_CERT: i64 = 9;
```

*Defined at line 63*

---

## ACME_ST_DONE:

```fides
pub const ACME_ST_DONE: i64 = 10;
```

*Defined at line 64*

---

## ACME_ST_ERROR:

```fides
pub const ACME_ST_ERROR: i64 = 11;
```

*Defined at line 65*

---

## AcmeDirectory

The directory object.

```fides
pub struct AcmeDirectory {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `new_nonce` | `str` |
| `new_account` | `str` |
| `new_order` | `str` |
| `revoke_cert` | `str` |
| `key_change` | `str` |
| `terms_of_service` | `str` |
| `eab_required` | `bool` |

*Defined at line 68*

---

## AcmeHttpRequest

One HTTP request ("" method: nothing to send right now).

```fides
pub struct AcmeHttpRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str` |
| `url` | `str` |
| `headers` | `str` |
| `body` | `str` |

*Defined at line 80*

---

## AcmeProblem

An error document (application/problem+json).

```fides
pub struct AcmeProblem {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kind` | `str` |
| `detail` | `str` |
| `status` | `i64` |

*Defined at line 88*

---

## AcmeOrder

An order object.

```fides
pub struct AcmeOrder {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `status` | `str` |
| `authorizations` | `Vec<str>` |
| `finalize` | `str` |
| `certificate` | `str` |

*Defined at line 96*

---

## AcmeAuthz

An authorization with its challenges (parallel lists).

```fides
pub struct AcmeAuthz {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `status` | `str` |
| `identifier` | `str` |
| `wildcard` | `bool` |
| `chal_types` | `Vec<str>` |
| `chal_urls` | `Vec<str>` |
| `chal_tokens` | `Vec<str>` |
| `chal_status` | `Vec<str>` |

*Defined at line 105*

---

## AcmeCsrInfo

What a CSR says, once its signature checked.

```fides
pub struct AcmeCsrInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `common_name` | `str` |
| `domains` | `Vec<str>` |
| `key_alg` | `str` |
| `spki` | `Vec<i64>` |

*Defined at line 117*

---

## AcmeClient

The client's state machine.

```fides
pub struct AcmeClient {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `err` | `str` |
| `directory_url` | `str` |
| `dir` | `AcmeDirectory` |
| `key` | `Jwk` |
| `alg` | `str` |
| `kid` | `str` |
| `contacts` | `Vec<str>` |
| `domains` | `Vec<str>` |
| `eab_kid` | `str` |
| `eab_key` | `Vec<i64>` |
| `nonces` | `Vec<str>` |
| `fetching_nonce` | `bool` |
| `bad_nonce_retries` | `i64` |
| `order_url` | `str` |
| `finalize_url` | `str` |
| `cert_url` | `str` |
| `authz_urls` | `Vec<str>` |
| `authz_index` | `i64` |
| `prefer` | `str` |
| `chal_type` | `str` |
| `chal_url` | `str` |
| `chal_token` | `str` |
| `key_authorization` | `str` |
| `http01_path` | `str` |
| `dns01_name` | `str` |
| `dns01_value` | `str` |
| `next_poll` | `i64` |
| `polls` | `i64` |
| `max_polls` | `i64` |
| `csr_key` | `Jwk` |
| `certificate` | `str` |

*Defined at line 127*

---

## acme_parse_directory

Parse the directory; newNonce, newAccount and newOrder are required.

```fides
pub fn acme_parse_directory(body: str) -> AcmeDirectory {
```

**Parameters:**

- `body`: `str`

**Returns:** `AcmeDirectory`

*Defined at line 175*

---

## acme_parse_problem

Parse a problem document; ok false when the body is not one.

```fides
pub fn acme_parse_problem(body: str) -> AcmeProblem {
```

**Parameters:**

- `body`: `str`

**Returns:** `AcmeProblem`

*Defined at line 197*

---

## acme_parse_order

Parse an order object.

```fides
pub fn acme_parse_order(body: str) -> AcmeOrder {
```

**Parameters:**

- `body`: `str`

**Returns:** `AcmeOrder`

*Defined at line 220*

---

## acme_parse_authz

Parse an authorization object.

```fides
pub fn acme_parse_authz(body: str) -> AcmeAuthz {
```

**Parameters:**

- `body`: `str`

**Returns:** `AcmeAuthz`

*Defined at line 236*

---

## acme_jws

A flattened JWS for an ACME request. With an empty `kid` the header
carries the public JWK (newAccount, revocation by certificate key).
An empty `payload` is POST-as-GET.

```fides
pub fn acme_jws(key: &Jwk, alg: str, url: str, nonce: str, kid: str, payload: str) -> str {
```

**Parameters:**

- `key`: `&Jwk`
- `alg`: `str`
- `url`: `str`
- `nonce`: `str`
- `kid`: `str`
- `payload`: `str`

**Returns:** `str`

*Defined at line 273*

---

## acme_eab

The externalAccountBinding object (§7.3.4): an HS256 JWS over the
account's public JWK with the CA-issued MAC key.

```fides
pub fn acme_eab(eab_kid: str, mac_key: Vec<i64>, account_key: &Jwk, url: str) -> str {
```

**Parameters:**

- `eab_kid`: `str`
- `mac_key`: `Vec<i64>`
- `account_key`: `&Jwk`
- `url`: `str`

**Returns:** `str`

*Defined at line 297*

---

## acme_new_account_payload

newAccount payload (§7.3); `eab` is acme_eab's object or "".

```fides
pub fn acme_new_account_payload(contacts: &Vec<str>, agree_tos: bool, eab: str) -> str {
```

**Parameters:**

- `contacts`: `&Vec<str>`
- `agree_tos`: `bool`
- `eab`: `str`

**Returns:** `str`

*Defined at line 319*

---

## acme_new_order_payload

newOrder payload (§7.4) for DNS names.

```fides
pub fn acme_new_order_payload(domains: &Vec<str>) -> str {
```

**Parameters:**

- `domains`: `&Vec<str>`

**Returns:** `str`

*Defined at line 331*

---

## acme_finalize_payload

finalize payload: the CSR's DER as base64url.

```fides
pub fn acme_finalize_payload(csr_der: Vec<i64>) -> str {
```

**Parameters:**

- `csr_der`: `Vec<i64>`

**Returns:** `str`

*Defined at line 345*

---

## acme_revoke_payload

revokeCert payload (§7.6) with an RFC 5280 reason code.

```fides
pub fn acme_revoke_payload(cert_der: Vec<i64>, reason: i64) -> str {
```

**Parameters:**

- `cert_der`: `Vec<i64>`
- `reason`: `i64`

**Returns:** `str`

*Defined at line 350*

---

## acme_deactivate_payload

The payload that deactivates an account or authorization (§7.3.6, §7.5.2).

```fides
pub fn acme_deactivate_payload() -> str {
```

**Returns:** `str`

*Defined at line 355*

---

## acme_key_authorization

keyAuthorization = token || "." || base64url(JWK thumbprint) (§8.1).

```fides
pub fn acme_key_authorization(token: str, account_key: &Jwk) -> str {
```

**Parameters:**

- `token`: `str`
- `account_key`: `&Jwk`

**Returns:** `str`

*Defined at line 362*

---

## acme_http01_path

Where http-01 must serve the key authorization (§8.3).

```fides
pub fn acme_http01_path(token: str) -> str {
```

**Parameters:**

- `token`: `str`

**Returns:** `str`

*Defined at line 367*

---

## acme_dns01_name

The dns-01 record name for a domain (wildcards drop "*.").

```fides
pub fn acme_dns01_name(domain: str) -> str {
```

**Parameters:**

- `domain`: `str`

**Returns:** `str`

*Defined at line 372*

---

## acme_dns01_value

The dns-01 TXT value: base64url(SHA-256(keyAuthorization)) (§8.4).

```fides
pub fn acme_dns01_value(token: str, account_key: &Jwk) -> str {
```

**Parameters:**

- `token`: `str`
- `account_key`: `&Jwk`

**Returns:** `str`

*Defined at line 381*

---

## acme_csr

The DER CSR for `domains` (first one also the CN), signed with a P-256
(ecdsa-with-SHA256) or Ed25519 private key; empty for other keys.

```fides
pub fn acme_csr(key: &Jwk, domains: &Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Jwk`
- `domains`: `&Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 403*

---

## acme_csr_pem

The CSR as a PEM "CERTIFICATE REQUEST" block.

```fides
pub fn acme_csr_pem(key: &Jwk, domains: &Vec<str>) -> str {
```

**Parameters:**

- `key`: `&Jwk`
- `domains`: `&Vec<str>`

**Returns:** `str`

*Defined at line 433*

---

## acme_csr_check

Parse a DER CSR, check its self-signature (P-256/SHA-256 or Ed25519)
and return its CN and subjectAltName DNS names.

```fides
pub fn acme_csr_check(der: Vec<i64>) -> AcmeCsrInfo {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `AcmeCsrInfo`

*Defined at line 445*

---

## acme_client_new

A client for `domains` with an account key and its JWS algorithm
("ES256", "EdDSA", "RS256"). The certificate key defaults to the
account key; set another with acme_set_csr_key (recommended).

```fides
pub fn acme_client_new(directory_url: str, key: Jwk, alg: str, contacts: Vec<str>, domains: Vec<str>) -> AcmeClient {
```

**Parameters:**

- `directory_url`: `str`
- `key`: `Jwk`
- `alg`: `str`
- `contacts`: `Vec<str>`
- `domains`: `Vec<str>`

**Returns:** `AcmeClient`

*Defined at line 537*

---

## acme_set_csr_key

The certificate's key (P-256 or Ed25519 with its private part).

```fides
pub fn acme_set_csr_key(c: &mut AcmeClient, key: Jwk) {
```

**Parameters:**

- `c`: `&mut AcmeClient`
- `key`: `Jwk`

*Defined at line 544*

---

## acme_set_eab

External account binding credentials from the CA.

```fides
pub fn acme_set_eab(c: &mut AcmeClient, kid: str, mac_key: Vec<i64>) {
```

**Parameters:**

- `c`: `&mut AcmeClient`
- `kid`: `str`
- `mac_key`: `Vec<i64>`

*Defined at line 549*

---

## acme_next_request

The next request to send at `now` (method "" when waiting: for the
challenge to be published, for a poll time, or at the end).

```fides
pub fn acme_next_request(c: &mut AcmeClient, now: i64) -> AcmeHttpRequest {
```

**Parameters:**

- `c`: `&mut AcmeClient`
- `now`: `i64`

**Returns:** `AcmeHttpRequest`

*Defined at line 596*

---

## acme_challenge_published

The caller has published the challenge response (http-01 file or
dns-01 record): tell the server to validate.

```fides
pub fn acme_challenge_published(c: &mut AcmeClient) {
```

**Parameters:**

- `c`: `&mut AcmeClient`

*Defined at line 645*

---

## acme_handle_response

Handle the response to the last request. `nonce` is the Replay-Nonce
header ("" if none), `location` the Location header, `retry_after`
the Retry-After seconds (0 if none).

```fides
pub fn acme_handle_response(c: &mut AcmeClient, status: i64, location: str, nonce: str, retry_after: i64, body: str, now: i64) {
```

**Parameters:**

- `c`: `&mut AcmeClient`
- `status`: `i64`
- `location`: `str`
- `nonce`: `str`
- `retry_after`: `i64`
- `body`: `str`
- `now`: `i64`

*Defined at line 721*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
