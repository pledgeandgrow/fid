[All modules](index.md)

# oauth2 -- Fides Documentation

## Contents

- [OAuthClient](#OAuthClient) (struct)
- [OAuthProvider](#OAuthProvider) (struct)
- [OAuthHttpRequest](#OAuthHttpRequest) (struct)
- [OAuthPkce](#OAuthPkce) (struct)
- [OAuthRedirect](#OAuthRedirect) (struct)
- [OAuthToken](#OAuthToken) (struct)
- [OidcIdToken](#OidcIdToken) (struct)
- [oauth2_form_encode](#oauth2_form_encode) (function)
- [oauth2_form_decode](#oauth2_form_decode) (function)
- [oauth2_form_get](#oauth2_form_get) (function)
- [oauth2_form_has](#oauth2_form_has) (function)
- [oauth2_random_token](#oauth2_random_token) (function)
- [oauth2_pkce_verifier_ok](#oauth2_pkce_verifier_ok) (function)
- [oauth2_pkce](#oauth2_pkce) (function)
- [oauth2_pkce_plain](#oauth2_pkce_plain) (function)
- [oauth2_pkce_check](#oauth2_pkce_check) (function)
- [oauth2_client](#oauth2_client) (function)
- [oauth2_discovery_url](#oauth2_discovery_url) (function)
- [oauth2_parse_discovery](#oauth2_parse_discovery) (function)
- [oauth2_authorize_url](#oauth2_authorize_url) (function)
- [oauth2_parse_redirect](#oauth2_parse_redirect) (function)
- [oauth2_basic_auth](#oauth2_basic_auth) (function)
- [oauth2_client_assertion](#oauth2_client_assertion) (function)
- [oauth2_code_request](#oauth2_code_request) (function)
- [oauth2_refresh_request](#oauth2_refresh_request) (function)
- [oauth2_client_credentials_request](#oauth2_client_credentials_request) (function)
- [oauth2_revoke_request](#oauth2_revoke_request) (function)
- [oauth2_bearer_request](#oauth2_bearer_request) (function)
- [oidc_userinfo_request](#oidc_userinfo_request) (function)
- [oauth2_parse_token_response](#oauth2_parse_token_response) (function)
- [oauth2_token_usable](#oauth2_token_usable) (function)
- [oidc_half_hash](#oidc_half_hash) (function)
- [oidc_validate_id_token](#oidc_validate_id_token) (function)

## OAuthClient

What a client knows about itself. `auth` is "client_secret_basic",
"client_secret_post", "client_secret_jwt" (HS256 assertion with the
secret), "private_key_jwt" (assertion signed with `key`) or "none"
(public client: client_id in the body).

```fides
pub struct OAuthClient {
```

**Fields:**

| Name | Type |
|------|------|
| `client_id` | `str` |
| `client_secret` | `str` |
| `redirect_uri` | `str` |
| `auth` | `str` |
| `key` | `Jwk` |
| `alg` | `str` |

*Defined at line 48*

---

## OAuthProvider

The provider's metadata (discovery document).

```fides
pub struct OAuthProvider {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `issuer` | `str` |
| `authorization_endpoint` | `str` |
| `token_endpoint` | `str` |
| `userinfo_endpoint` | `str` |
| `jwks_uri` | `str` |
| `revocation_endpoint` | `str` |
| `end_session_endpoint` | `str` |
| `id_token_algs` | `str` |
| `supports_s256` | `bool` |
| `iss_parameter` | `bool` |

*Defined at line 58*

---

## OAuthHttpRequest

An HTTP request to send: headers are "Name: value" lines joined by CR LF.

```fides
pub struct OAuthHttpRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str` |
| `url` | `str` |
| `headers` | `str` |
| `body` | `str` |

*Defined at line 74*

---

## OAuthPkce

A PKCE pair.

```fides
pub struct OAuthPkce {
```

**Fields:**

| Name | Type |
|------|------|
| `verifier` | `str` |
| `challenge` | `str` |
| `method` | `str` |

*Defined at line 82*

---

## OAuthRedirect

The result of the authorization redirect.

```fides
pub struct OAuthRedirect {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `code` | `str` |
| `state` | `str` |

*Defined at line 89*

---

## OAuthToken

A token response (or its error).

```fides
pub struct OAuthToken {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `err_description` | `str` |
| `access_token` | `str` |
| `token_type` | `str` |
| `expires_at` | `i64` |
| `refresh_token` | `str` |
| `scope` | `str` |
| `id_token` | `str` |

*Defined at line 97*

---

## OidcIdToken

A validated ID token: its claims and the subject.

```fides
pub struct OidcIdToken {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `sub` | `str` |
| `claims` | `str` |

*Defined at line 110*

---

## oauth2_form_encode

application/x-www-form-urlencoded encoding of one name or value
(RFC 6749 Appendix B): ALPHA DIGIT - . _ ~ kept, space as "+", every
other UTF-8 byte as %XX.

```fides
pub fn oauth2_form_encode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 126*

---

## oauth2_form_decode

Decode one form-encoded component ("+" is a space).

```fides
pub fn oauth2_form_decode(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 161*

---

## oauth2_form_get

The value of `name` in a form-encoded string ("" when absent).

```fides
pub fn oauth2_form_get(form: str, name: str) -> str {
```

**Parameters:**

- `form`: `str`
- `name`: `str`

**Returns:** `str`

*Defined at line 195*

---

## oauth2_form_has

True when the form has the parameter (even empty).

```fides
pub fn oauth2_form_has(form: str, name: str) -> bool {
```

**Parameters:**

- `form`: `str`
- `name`: `str`

**Returns:** `bool`

*Defined at line 213*

---

## oauth2_random_token

`n` random bytes as base64url text (state, nonce, PKCE verifier).

```fides
pub fn oauth2_random_token(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 232*

---

## oauth2_pkce_verifier_ok

True when a verifier is 43..128 unreserved characters (§4.1).

```fides
pub fn oauth2_pkce_verifier_ok(v: str) -> bool {
```

**Parameters:**

- `v`: `str`

**Returns:** `bool`

*Defined at line 239*

---

## oauth2_pkce

S256: challenge = BASE64URL(SHA-256(ASCII(verifier))).

```fides
pub fn oauth2_pkce(verifier: str) -> OAuthPkce {
```

**Parameters:**

- `verifier`: `str`

**Returns:** `OAuthPkce`

*Defined at line 257*

---

## oauth2_pkce_plain

The plain method (only for providers without S256).

```fides
pub fn oauth2_pkce_plain(verifier: str) -> OAuthPkce {
```

**Parameters:**

- `verifier`: `str`

**Returns:** `OAuthPkce`

*Defined at line 269*

---

## oauth2_pkce_check

The server side check of §4.6.

```fides
pub fn oauth2_pkce_check(verifier: str, challenge: str, method: str) -> bool {
```

**Parameters:**

- `verifier`: `str`
- `challenge`: `str`
- `method`: `str`

**Returns:** `bool`

*Defined at line 281*

---

## oauth2_client

A client with a secret (client_secret_basic), or a public client when
`secret` is empty (auth "none").

```fides
pub fn oauth2_client(client_id: str, secret: str, redirect_uri: str) -> OAuthClient {
```

**Parameters:**

- `client_id`: `str`
- `secret`: `str`
- `redirect_uri`: `str`

**Returns:** `OAuthClient`

*Defined at line 298*

---

## oauth2_discovery_url

The discovery document URL for an issuer (§4).

```fides
pub fn oauth2_discovery_url(issuer: str) -> str {
```

**Parameters:**

- `issuer`: `str`

**Returns:** `str`

*Defined at line 309*

---

## oauth2_parse_discovery

Parse a discovery document fetched for `expected_issuer`: the issuer
must be exactly that value (OIDC Discovery §4.3) and every endpoint
HTTPS.

```fides
pub fn oauth2_parse_discovery(doc: str, expected_issuer: str) -> OAuthProvider {
```

**Parameters:**

- `doc`: `str`
- `expected_issuer`: `str`

**Returns:** `OAuthProvider`

*Defined at line 345*

---

## oauth2_authorize_url

The URL to send the user's browser to. `nonce` and `challenge` are
left out when empty.

```fides
pub fn oauth2_authorize_url(p: &OAuthProvider, c: &OAuthClient, scope: str, state: str, nonce: str, challenge: str) -> str {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `scope`: `str`
- `state`: `str`
- `nonce`: `str`
- `challenge`: `str`

**Returns:** `str`

*Defined at line 377*

---

## oauth2_parse_redirect

Parse the redirect URL (its query; a fragment is ignored): checks the
state and, when present or required, the RFC 9207 iss parameter.

```fides
pub fn oauth2_parse_redirect(url: str, expected_state: str, p: &OAuthProvider) -> OAuthRedirect {
```

**Parameters:**

- `url`: `str`
- `expected_state`: `str`
- `p`: `&OAuthProvider`

**Returns:** `OAuthRedirect`

*Defined at line 397*

---

## oauth2_basic_auth

The HTTP Basic header value of §2.3.1 (id and secret form-encoded first).

```fides
pub fn oauth2_basic_auth(client_id: str, secret: str) -> str {
```

**Parameters:**

- `client_id`: `str`
- `secret`: `str`

**Returns:** `str`

*Defined at line 443*

---

## oauth2_client_assertion

A client assertion JWT (RFC 7523 §3): iss = sub = client_id, aud = the
token endpoint, a jti and a 5-minute lifetime, signed with the client
secret (HS256) or the client's key.

```fides
pub fn oauth2_client_assertion(c: &OAuthClient, audience: str, jti: str, now: i64) -> str {
```

**Parameters:**

- `c`: `&OAuthClient`
- `audience`: `str`
- `jti`: `str`
- `now`: `i64`

**Returns:** `str`

*Defined at line 450*

---

## oauth2_code_request

Exchange an authorization code (with its PKCE verifier, if any).

```fides
pub fn oauth2_code_request(p: &OAuthProvider, c: &OAuthClient, code: str, verifier: str, now: i64) -> OAuthHttpRequest {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `code`: `str`
- `verifier`: `str`
- `now`: `i64`

**Returns:** `OAuthHttpRequest`

*Defined at line 478*

---

## oauth2_refresh_request

Refresh an access token (scope optional, at most the original).

```fides
pub fn oauth2_refresh_request(p: &OAuthProvider, c: &OAuthClient, refresh_token: str, scope: str, now: i64) -> OAuthHttpRequest {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `refresh_token`: `str`
- `scope`: `str`
- `now`: `i64`

**Returns:** `OAuthHttpRequest`

*Defined at line 487*

---

## oauth2_client_credentials_request

The client credentials grant.

```fides
pub fn oauth2_client_credentials_request(p: &OAuthProvider, c: &OAuthClient, scope: str, now: i64) -> OAuthHttpRequest {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `scope`: `str`
- `now`: `i64`

**Returns:** `OAuthHttpRequest`

*Defined at line 495*

---

## oauth2_revoke_request

Revoke a token (RFC 7009 §2.1).

```fides
pub fn oauth2_revoke_request(p: &OAuthProvider, c: &OAuthClient, token: str, hint: str, now: i64) -> OAuthHttpRequest {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `token`: `str`
- `hint`: `str`
- `now`: `i64`

**Returns:** `OAuthHttpRequest`

*Defined at line 502*

---

## oauth2_bearer_request

A resource request with the access token (RFC 6750 §2.1).

```fides
pub fn oauth2_bearer_request(method: str, url: str, access_token: str) -> OAuthHttpRequest {
```

**Parameters:**

- `method`: `str`
- `url`: `str`
- `access_token`: `str`

**Returns:** `OAuthHttpRequest`

*Defined at line 509*

---

## oidc_userinfo_request

The UserInfo request (OIDC Core §5.3).

```fides
pub fn oidc_userinfo_request(p: &OAuthProvider, access_token: str) -> OAuthHttpRequest {
```

**Parameters:**

- `p`: `&OAuthProvider`
- `access_token`: `str`

**Returns:** `OAuthHttpRequest`

*Defined at line 516*

---

## oauth2_parse_token_response

Parse a token endpoint response (§5.1, error §5.2). `expires_at` is
now + expires_in, or 0 when the server gave no lifetime.

```fides
pub fn oauth2_parse_token_response(status: i64, body: str, now: i64) -> OAuthToken {
```

**Parameters:**

- `status`: `i64`
- `body`: `str`
- `now`: `i64`

**Returns:** `OAuthToken`

*Defined at line 528*

---

## oauth2_token_usable

True when the token is a Bearer token (case-insensitive, §7.1) not
expiring within `margin` seconds.

```fides
pub fn oauth2_token_usable(t: &OAuthToken, now: i64, margin: i64) -> bool {
```

**Parameters:**

- `t`: `&OAuthToken`
- `now`: `i64`
- `margin`: `i64`

**Returns:** `bool`

*Defined at line 560*

---

## oidc_half_hash

at_hash / c_hash: base64url of the left half of the token's hash with
the hash of the ID token's algorithm (SHA-256 for *256, SHA-512 for
*512 and EdDSA).

```fides
pub fn oidc_half_hash(alg: str, token: str) -> str {
```

**Parameters:**

- `alg`: `str`
- `token`: `str`

**Returns:** `str`

*Defined at line 578*

---

## oidc_validate_id_token

Validate an ID token: signature with a key of `keys` and an algorithm
the provider announced, iss, aud (and azp), exp/iat at `now` (60 s
leeway), the nonce sent in the request, and at_hash when an access
token is given and the token carries one. `max_age` < 0 skips the
auth_time check.

```fides
pub fn oidc_validate_id_token(id_token: str, keys: &Vec<Jwk>, p: &OAuthProvider, c: &OAuthClient, nonce: str, access_token: str, now: i64, max_age: i64) -> OidcIdToken {
```

**Parameters:**

- `id_token`: `str`
- `keys`: `&Vec<Jwk>`
- `p`: `&OAuthProvider`
- `c`: `&OAuthClient`
- `nonce`: `str`
- `access_token`: `str`
- `now`: `i64`
- `max_age`: `i64`

**Returns:** `OidcIdToken`

*Defined at line 599*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
