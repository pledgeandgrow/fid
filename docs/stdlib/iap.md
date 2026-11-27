[All modules](index.md)

# iap -- Fides Documentation

## Contents

- [IAP_CONSUMABLE:](#IAP_CONSUMABLE:) (const)
- [IAP_NON_CONSUMABLE:](#IAP_NON_CONSUMABLE:) (const)
- [IAP_AUTO_RENEWABLE:](#IAP_AUTO_RENEWABLE:) (const)
- [IAP_NON_RENEWING:](#IAP_NON_RENEWING:) (const)
- [IapProduct](#IapProduct) (struct)
- [iap_currency_exponent](#iap_currency_exponent) (function)
- [iap_format_price](#iap_format_price) (function)
- [iap_days_from_civil](#iap_days_from_civil) (function)
- [iap_civil_from_days](#iap_civil_from_days) (function)
- [iap_parse_period](#iap_parse_period) (function)
- [iap_add_period](#iap_add_period) (function)
- [iap_parse_rfc3339](#iap_parse_rfc3339) (function)
- [IAP_IDLE:](#IAP_IDLE:) (const)
- [IAP_PURCHASING:](#IAP_PURCHASING:) (const)
- [IAP_PENDING:](#IAP_PENDING:) (const)
- [IAP_PURCHASED:](#IAP_PURCHASED:) (const)
- [IAP_VERIFIED:](#IAP_VERIFIED:) (const)
- [IAP_DELIVERED:](#IAP_DELIVERED:) (const)
- [IAP_FINISHED:](#IAP_FINISHED:) (const)
- [IAP_FAILED:](#IAP_FAILED:) (const)
- [IAP_CANCELED:](#IAP_CANCELED:) (const)
- [IAP_REFUNDED:](#IAP_REFUNDED:) (const)
- [IAP_EV_BUY:](#IAP_EV_BUY:) (const)
- [IAP_EV_DEFERRED:](#IAP_EV_DEFERRED:) (const)
- [IAP_EV_STORE_OK:](#IAP_EV_STORE_OK:) (const)
- [IAP_EV_STORE_FAILED:](#IAP_EV_STORE_FAILED:) (const)
- [IAP_EV_USER_CANCELED:](#IAP_EV_USER_CANCELED:) (const)
- [IAP_EV_VERIFY_OK:](#IAP_EV_VERIFY_OK:) (const)
- [IAP_EV_VERIFY_FAILED:](#IAP_EV_VERIFY_FAILED:) (const)
- [IAP_EV_DELIVER:](#IAP_EV_DELIVER:) (const)
- [IAP_EV_FINISH:](#IAP_EV_FINISH:) (const)
- [IAP_EV_REFUND:](#IAP_EV_REFUND:) (const)
- [IAP_PLAY_ACK_WINDOW_MS:](#IAP_PLAY_ACK_WINDOW_MS:) (const)
- [iap_state_name](#iap_state_name) (function)
- [iap_next](#iap_next) (function)
- [IapPurchase](#IapPurchase) (struct)
- [iap_purchase_new](#iap_purchase_new) (function)
- [iap_purchase_event](#iap_purchase_event) (function)
- [iap_play_ack_overdue](#iap_play_ack_overdue) (function)
- [IapTransaction](#IapTransaction) (struct)
- [iap_transaction_empty](#iap_transaction_empty) (function)
- [IAP_SUB_ACTIVE:](#IAP_SUB_ACTIVE:) (const)
- [IAP_SUB_EXPIRED:](#IAP_SUB_EXPIRED:) (const)
- [IAP_SUB_BILLING_RETRY:](#IAP_SUB_BILLING_RETRY:) (const)
- [IAP_SUB_GRACE_PERIOD:](#IAP_SUB_GRACE_PERIOD:) (const)
- [IAP_SUB_REVOKED:](#IAP_SUB_REVOKED:) (const)
- [iap_sub_status_name](#iap_sub_status_name) (function)
- [iap_subscription_status](#iap_subscription_status) (function)
- [iap_entitled](#iap_entitled) (function)
- [iap_appstore_host](#iap_appstore_host) (function)
- [iap_appstore_transaction_url](#iap_appstore_transaction_url) (function)
- [iap_appstore_subscriptions_url](#iap_appstore_subscriptions_url) (function)
- [iap_appstore_history_url](#iap_appstore_history_url) (function)
- [iap_api_token_input](#iap_api_token_input) (function)
- [iap_api_token](#iap_api_token) (function)
- [IapJws](#IapJws) (struct)
- [iap_jws_decode](#iap_jws_decode) (function)
- [iap_appstore_kind](#iap_appstore_kind) (function)
- [iap_appstore_transaction](#iap_appstore_transaction) (function)
- [IapNotification](#IapNotification) (struct)
- [iap_appstore_notification](#iap_appstore_notification) (function)
- [iap_notification_event](#iap_notification_event) (function)
- [iap_receipt_status](#iap_receipt_status) (function)
- [iap_receipt_transactions](#iap_receipt_transactions) (function)
- [iap_play_product_url](#iap_play_product_url) (function)
- [iap_play_acknowledge_url](#iap_play_acknowledge_url) (function)
- [iap_play_consume_url](#iap_play_consume_url) (function)
- [iap_play_subscription_url](#iap_play_subscription_url) (function)
- [iap_play_token_valid](#iap_play_token_valid) (function)
- [IapPlayProduct](#IapPlayProduct) (struct)
- [iap_play_product](#iap_play_product) (function)
- [iap_play_product_state](#iap_play_product_state) (function)
- [iap_play_subscription_status](#iap_play_subscription_status) (function)
- [IapPlayLocal](#IapPlayLocal) (struct)
- [iap_play_local](#iap_play_local) (function)

## IAP_CONSUMABLE:

```fides
pub const IAP_CONSUMABLE: i64 = 0;
```

*Defined at line 41*

---

## IAP_NON_CONSUMABLE:

```fides
pub const IAP_NON_CONSUMABLE: i64 = 1;
```

*Defined at line 42*

---

## IAP_AUTO_RENEWABLE:

```fides
pub const IAP_AUTO_RENEWABLE: i64 = 2;
```

*Defined at line 43*

---

## IAP_NON_RENEWING:

```fides
pub const IAP_NON_RENEWING: i64 = 3;
```

*Defined at line 44*

---

## IapProduct

```fides
pub struct IapProduct {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `kind` | `i64` |
| `price_micros` | `i64,  // 4990000 = 4.99` |
| `currency` | `str,  // ISO 4217` |
| `period` | `str,  // ISO 8601 for subscriptions, "" otherwise` |
| `intro_period` | `str,  // free trial / intro offer length, "" none` |

*Defined at line 46*

---

## iap_currency_exponent

Digits after the decimal point of a currency (ISO 4217 minor unit).

```fides
pub fn iap_currency_exponent(cur: str) -> i64 {
```

**Parameters:**

- `cur`: `str`

**Returns:** `i64`

*Defined at line 56*

---

## iap_format_price

"4.99 USD", "500 JPY", "1.250 KWD" (rounded half up to the exponent).

```fides
pub fn iap_format_price(micros: i64, cur: str) -> str {
```

**Parameters:**

- `micros`: `i64`
- `cur`: `str`

**Returns:** `str`

*Defined at line 69*

---

## iap_days_from_civil

Days since 1970-01-01 of a civil date (proleptic Gregorian).

```fides
pub fn iap_days_from_civil(y: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 107*

---

## iap_civil_from_days

The civil date [y, m, d] of a day number.

```fides
pub fn iap_civil_from_days(z0: i64) -> Vec<i64> {
```

**Parameters:**

- `z0`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 130*

---

## iap_parse_period

An ISO 8601 period "PnYnMnWnD" as [years, months, days]; empty when
malformed.

```fides
pub fn iap_parse_period(p: str) -> Vec<i64> {
```

**Parameters:**

- `p`: `str`

**Returns:** `Vec<i64>`

*Defined at line 173*

---

## iap_add_period

`ms` (UTC epoch) plus a period; a day past the end of the target
month is clamped (Jan 31 + P1M = Feb 28/29). -1 when malformed.

```fides
pub fn iap_add_period(ms: i64, period: str) -> i64 {
```

**Parameters:**

- `ms`: `i64`
- `period`: `str`

**Returns:** `i64`

*Defined at line 221*

---

## iap_parse_rfc3339

RFC 3339 "2024-05-01T12:00:00.000Z" (or with +hh:mm) -> epoch ms;
-1 when malformed.

```fides
pub fn iap_parse_rfc3339(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 249*

---

## IAP_IDLE:

```fides
pub const IAP_IDLE: i64 = 0;
```

*Defined at line 295*

---

## IAP_PURCHASING:

```fides
pub const IAP_PURCHASING: i64 = 1;
```

*Defined at line 296*

---

## IAP_PENDING:

```fides
pub const IAP_PENDING: i64 = 2;
```

*Defined at line 297*

---

## IAP_PURCHASED:

```fides
pub const IAP_PURCHASED: i64 = 3;
```

*Defined at line 298*

---

## IAP_VERIFIED:

```fides
pub const IAP_VERIFIED: i64 = 4;
```

*Defined at line 299*

---

## IAP_DELIVERED:

```fides
pub const IAP_DELIVERED: i64 = 5;
```

*Defined at line 300*

---

## IAP_FINISHED:

```fides
pub const IAP_FINISHED: i64 = 6;  // StoreKit finish() / Play acknowledge / consume
```

*Defined at line 301*

---

## IAP_FAILED:

```fides
pub const IAP_FAILED: i64 = 7;
```

*Defined at line 302*

---

## IAP_CANCELED:

```fides
pub const IAP_CANCELED: i64 = 8;
```

*Defined at line 303*

---

## IAP_REFUNDED:

```fides
pub const IAP_REFUNDED: i64 = 9;
```

*Defined at line 304*

---

## IAP_EV_BUY:

```fides
pub const IAP_EV_BUY: i64 = 0;
```

*Defined at line 306*

---

## IAP_EV_DEFERRED:

```fides
pub const IAP_EV_DEFERRED: i64 = 1;
```

*Defined at line 307*

---

## IAP_EV_STORE_OK:

```fides
pub const IAP_EV_STORE_OK: i64 = 2;
```

*Defined at line 308*

---

## IAP_EV_STORE_FAILED:

```fides
pub const IAP_EV_STORE_FAILED: i64 = 3;
```

*Defined at line 309*

---

## IAP_EV_USER_CANCELED:

```fides
pub const IAP_EV_USER_CANCELED: i64 = 4;
```

*Defined at line 310*

---

## IAP_EV_VERIFY_OK:

```fides
pub const IAP_EV_VERIFY_OK: i64 = 5;
```

*Defined at line 311*

---

## IAP_EV_VERIFY_FAILED:

```fides
pub const IAP_EV_VERIFY_FAILED: i64 = 6;
```

*Defined at line 312*

---

## IAP_EV_DELIVER:

```fides
pub const IAP_EV_DELIVER: i64 = 7;
```

*Defined at line 313*

---

## IAP_EV_FINISH:

```fides
pub const IAP_EV_FINISH: i64 = 8;
```

*Defined at line 314*

---

## IAP_EV_REFUND:

```fides
pub const IAP_EV_REFUND: i64 = 9;
```

*Defined at line 315*

---

## IAP_PLAY_ACK_WINDOW_MS:

```fides
pub const IAP_PLAY_ACK_WINDOW_MS: i64 = 259200000;  // 3 days
```

*Defined at line 317*

---

## iap_state_name

```fides
pub fn iap_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 319*

---

## iap_next

The next state, or -1 when the event is not allowed in `s`.

```fides
pub fn iap_next(s: i64, ev: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `ev`: `i64`

**Returns:** `i64`

*Defined at line 328*

---

## IapPurchase

```fides
pub struct IapPurchase {
```

**Fields:**

| Name | Type |
|------|------|
| `product` | `IapProduct` |
| `state` | `i64` |
| `purchased_ms` | `i64` |
| `transaction_id` | `str` |
| `history` | `Vec<i64>` |
| `refused` | `i64` |

*Defined at line 395*

---

## iap_purchase_new

```fides
pub fn iap_purchase_new(p: IapProduct) -> IapPurchase {
```

**Parameters:**

- `p`: `IapProduct`

**Returns:** `IapPurchase`

*Defined at line 404*

---

## iap_purchase_event

Apply an event; false when it is not allowed (a non-consumable that
was finished cannot be bought again, a consumable can).

```fides
pub fn iap_purchase_event(p: &mut IapPurchase, ev: i64, now: i64) -> bool {
```

**Parameters:**

- `p`: `&mut IapPurchase`
- `ev`: `i64`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 414*

---

## iap_play_ack_overdue

Play refunds a purchase not acknowledged within three days.

```fides
pub fn iap_play_ack_overdue(p: &IapPurchase, now: i64) -> bool {
```

**Parameters:**

- `p`: `&IapPurchase`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 432*

---

## IapTransaction

```fides
pub struct IapTransaction {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `transaction_id` | `str` |
| `original_id` | `str` |
| `product_id` | `str` |
| `bundle_id` | `str` |
| `kind` | `i64` |
| `purchase_ms` | `i64` |
| `expires_ms` | `i64,  // -1 none` |
| `revocation_ms` | `i64,  // -1 none` |
| `quantity` | `i64` |
| `environment` | `str,  // Production, Sandbox` |
| `ownership` | `str,  // PURCHASED, FAMILY_SHARED` |
| `account_token` | `str` |

*Defined at line 441*

---

## iap_transaction_empty

```fides
pub fn iap_transaction_empty() -> IapTransaction {
```

**Returns:** `IapTransaction`

*Defined at line 457*

---

## IAP_SUB_ACTIVE:

```fides
pub const IAP_SUB_ACTIVE: i64 = 1;
```

*Defined at line 463*

---

## IAP_SUB_EXPIRED:

```fides
pub const IAP_SUB_EXPIRED: i64 = 2;
```

*Defined at line 464*

---

## IAP_SUB_BILLING_RETRY:

```fides
pub const IAP_SUB_BILLING_RETRY: i64 = 3;
```

*Defined at line 465*

---

## IAP_SUB_GRACE_PERIOD:

```fides
pub const IAP_SUB_GRACE_PERIOD: i64 = 4;
```

*Defined at line 466*

---

## IAP_SUB_REVOKED:

```fides
pub const IAP_SUB_REVOKED: i64 = 5;
```

*Defined at line 467*

---

## iap_sub_status_name

```fides
pub fn iap_sub_status_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 469*

---

## iap_subscription_status

The status (the numbering of the App Store Server API) at `now` from
the latest transaction, whether the store is retrying billing, and the
end of the grace period (-1: none).

```fides
pub fn iap_subscription_status(t: &IapTransaction, now: i64, billing_retry: bool, grace_until_ms: i64) -> i64 {
```

**Parameters:**

- `t`: `&IapTransaction`
- `now`: `i64`
- `billing_retry`: `bool`
- `grace_until_ms`: `i64`

**Returns:** `i64`

*Defined at line 491*

---

## iap_entitled

Does the user get the content now (active or in grace period)?

```fides
pub fn iap_entitled(status: i64) -> bool {
```

**Parameters:**

- `status`: `i64`

**Returns:** `bool`

*Defined at line 508*

---

## iap_appstore_host

```fides
pub fn iap_appstore_host(sandbox: bool) -> str {
```

**Parameters:**

- `sandbox`: `bool`

**Returns:** `str`

*Defined at line 514*

---

## iap_appstore_transaction_url

```fides
pub fn iap_appstore_transaction_url(sandbox: bool, transaction_id: str) -> str {
```

**Parameters:**

- `sandbox`: `bool`
- `transaction_id`: `str`

**Returns:** `str`

*Defined at line 521*

---

## iap_appstore_subscriptions_url

```fides
pub fn iap_appstore_subscriptions_url(sandbox: bool, original_id: str) -> str {
```

**Parameters:**

- `sandbox`: `bool`
- `original_id`: `str`

**Returns:** `str`

*Defined at line 525*

---

## iap_appstore_history_url

```fides
pub fn iap_appstore_history_url(sandbox: bool, transaction_id: str, revision: str) -> str {
```

**Parameters:**

- `sandbox`: `bool`
- `transaction_id`: `str`
- `revision`: `str`

**Returns:** `str`

*Defined at line 529*

---

## iap_api_token_input

The signing input of an App Store Server API token: header
{"alg":"ES256","kid":..,"typ":"JWT"}, payload {"iss","iat","exp",
"aud":"appstoreconnect-v1","bid"} (exp at most 60 minutes later).

```fides
pub fn iap_api_token_input(key_id: str, issuer_id: str, bundle_id: str, iat: i64, ttl_secs: i64) -> str {
```

**Parameters:**

- `key_id`: `str`
- `issuer_id`: `str`
- `bundle_id`: `str`
- `iat`: `i64`
- `ttl_secs`: `i64`

**Returns:** `str`

*Defined at line 553*

---

## iap_api_token

The full token (signature from the seam as 64-byte r||s).

```fides
pub fn iap_api_token(key_id: str, issuer_id: str, bundle_id: str, iat: i64, sign: fn(Vec<i64>) -> Vec<i64>) -> str {
```

**Parameters:**

- `key_id`: `str`
- `issuer_id`: `str`
- `bundle_id`: `str`
- `iat`: `i64`
- `sign`: `fn(Vec<i64>) -> Vec<i64>`

**Returns:** `str`

*Defined at line 565*

---

## IapJws

The decoded parts of a JWS (compact serialization).

```fides
pub struct IapJws {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `header` | `str` |
| `payload` | `str` |
| `verified` | `bool` |

*Defined at line 575*

---

## iap_jws_decode

Split and decode a JWS; `verify` checks the signature (header b64url,
payload b64url, signature bytes); `verified` is its answer.

```fides
pub fn iap_jws_decode(jws: str, verify: fn(str, str, Vec<i64>) -> bool) -> IapJws {
```

**Parameters:**

- `jws`: `str`
- `verify`: `fn(str`

**Returns:** `IapJws`

*Defined at line 584*

---

## iap_appstore_kind

The kind of an App Store "type" string.

```fides
pub fn iap_appstore_kind(t: str) -> i64 {
```

**Parameters:**

- `t`: `str`

**Returns:** `i64`

*Defined at line 627*

---

## iap_appstore_transaction

A JWSTransactionDecodedPayload (JSON) as a transaction.

```fides
pub fn iap_appstore_transaction(payload: str) -> IapTransaction {
```

**Parameters:**

- `payload`: `str`

**Returns:** `IapTransaction`

*Defined at line 644*

---

## IapNotification

An App Store Server Notification V2 (the decoded signedPayload).

```fides
pub struct IapNotification {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kind` | `str,  // SUBSCRIBED, DID_RENEW, EXPIRED, REFUND ...` |
| `subtype` | `str` |
| `uuid` | `str` |
| `environment` | `str` |
| `signed_transaction` | `str` |
| `signed_renewal` | `str` |

*Defined at line 666*

---

## iap_appstore_notification

```fides
pub fn iap_appstore_notification(payload: str) -> IapNotification {
```

**Parameters:**

- `payload`: `str`

**Returns:** `IapNotification`

*Defined at line 676*

---

## iap_notification_event

The purchase-state event a notification means (-1: no change).

```fides
pub fn iap_notification_event(kind: str) -> i64 {
```

**Parameters:**

- `kind`: `str`

**Returns:** `i64`

*Defined at line 684*

---

## iap_receipt_status

verifyReceipt (legacy) status codes.

```fides
pub fn iap_receipt_status(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 698*

---

## iap_receipt_transactions

Entries of latest_receipt_info (legacy receipts; *_ms fields are
strings there).

```fides
pub fn iap_receipt_transactions(response: str) -> Vec<IapTransaction> {
```

**Parameters:**

- `response`: `str`

**Returns:** `Vec<IapTransaction>`

*Defined at line 737*

---

## iap_play_product_url

```fides
pub fn iap_play_product_url(package: str, product_id: str, token: str) -> str {
```

**Parameters:**

- `package`: `str`
- `product_id`: `str`
- `token`: `str`

**Returns:** `str`

*Defined at line 767*

---

## iap_play_acknowledge_url

```fides
pub fn iap_play_acknowledge_url(package: str, product_id: str, token: str) -> str {
```

**Parameters:**

- `package`: `str`
- `product_id`: `str`
- `token`: `str`

**Returns:** `str`

*Defined at line 771*

---

## iap_play_consume_url

```fides
pub fn iap_play_consume_url(package: str, product_id: str, token: str) -> str {
```

**Parameters:**

- `package`: `str`
- `product_id`: `str`
- `token`: `str`

**Returns:** `str`

*Defined at line 775*

---

## iap_play_subscription_url

```fides
pub fn iap_play_subscription_url(package: str, token: str) -> str {
```

**Parameters:**

- `package`: `str`
- `token`: `str`

**Returns:** `str`

*Defined at line 779*

---

## iap_play_token_valid

A purchase token looks like Play's: [A-Za-z0-9._-] and long enough.

```fides
pub fn iap_play_token_valid(t: str) -> bool {
```

**Parameters:**

- `t`: `str`

**Returns:** `bool`

*Defined at line 784*

---

## IapPlayProduct

A ProductPurchase resource: purchaseState 0 purchased, 1 canceled,
2 pending; consumptionState and acknowledgementState 0 / 1.

```fides
pub struct IapPlayProduct {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `order_id` | `str` |
| `purchase_state` | `i64` |
| `consumed` | `bool` |
| `acknowledged` | `bool` |
| `purchase_ms` | `i64` |
| `quantity` | `i64` |

*Defined at line 801*

---

## iap_play_product

```fides
pub fn iap_play_product(json_text: str) -> IapPlayProduct {
```

**Parameters:**

- `json_text`: `str`

**Returns:** `IapPlayProduct`

*Defined at line 811*

---

## iap_play_product_state

The purchase state of a ProductPurchase.

```fides
pub fn iap_play_product_state(p: &IapPlayProduct) -> i64 {
```

**Parameters:**

- `p`: `&IapPlayProduct`

**Returns:** `i64`

*Defined at line 818*

---

## iap_play_subscription_status

The SubscriptionPurchaseV2 subscriptionState and first line item's
expiry as a status at `now`.

```fides
pub fn iap_play_subscription_status(json_text: str, now: i64) -> i64 {
```

**Parameters:**

- `json_text`: `str`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 833*

---

## IapPlayLocal

Check a purchase from BillingClient (Purchase.getOriginalJson() and
getSignature()) with the seam, then read its token and product.

```fides
pub struct IapPlayLocal {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `verified` | `bool` |
| `token` | `str` |
| `product_id` | `str` |
| `state` | `i64` |
| `acknowledged` | `bool` |

*Defined at line 861*

---

## iap_play_local

```fides
pub fn iap_play_local(original_json: str, signature_b64: str, verify: fn(str, str) -> bool) -> IapPlayLocal {
```

**Parameters:**

- `original_json`: `str`
- `signature_b64`: `str`
- `verify`: `fn(str`

**Returns:** `IapPlayLocal`

*Defined at line 870*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
