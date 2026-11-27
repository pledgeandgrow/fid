[All modules](index.md)

# coap -- Fides Documentation

## Contents

- [COAP_CON:](#COAP_CON:) (const)
- [COAP_NON:](#COAP_NON:) (const)
- [COAP_ACK:](#COAP_ACK:) (const)
- [COAP_RST:](#COAP_RST:) (const)
- [COAP_EMPTY:](#COAP_EMPTY:) (const)
- [COAP_GET:](#COAP_GET:) (const)
- [COAP_POST:](#COAP_POST:) (const)
- [COAP_PUT:](#COAP_PUT:) (const)
- [COAP_DELETE:](#COAP_DELETE:) (const)
- [COAP_CREATED:](#COAP_CREATED:) (const)
- [COAP_DELETED:](#COAP_DELETED:) (const)
- [COAP_VALID:](#COAP_VALID:) (const)
- [COAP_CHANGED:](#COAP_CHANGED:) (const)
- [COAP_CONTENT:](#COAP_CONTENT:) (const)
- [COAP_CONTINUE:](#COAP_CONTINUE:) (const)
- [COAP_BAD_REQUEST:](#COAP_BAD_REQUEST:) (const)
- [COAP_NOT_FOUND:](#COAP_NOT_FOUND:) (const)
- [COAP_METHOD_NOT_ALLOWED:](#COAP_METHOD_NOT_ALLOWED:) (const)
- [COAP_REQUEST_INCOMPLETE:](#COAP_REQUEST_INCOMPLETE:) (const)
- [COAP_TOO_LARGE:](#COAP_TOO_LARGE:) (const)
- [COAP_INTERNAL_ERROR:](#COAP_INTERNAL_ERROR:) (const)
- [COAP_O_IF_MATCH:](#COAP_O_IF_MATCH:) (const)
- [COAP_O_URI_HOST:](#COAP_O_URI_HOST:) (const)
- [COAP_O_ETAG:](#COAP_O_ETAG:) (const)
- [COAP_O_IF_NONE_MATCH:](#COAP_O_IF_NONE_MATCH:) (const)
- [COAP_O_OBSERVE:](#COAP_O_OBSERVE:) (const)
- [COAP_O_URI_PORT:](#COAP_O_URI_PORT:) (const)
- [COAP_O_LOCATION_PATH:](#COAP_O_LOCATION_PATH:) (const)
- [COAP_O_URI_PATH:](#COAP_O_URI_PATH:) (const)
- [COAP_O_CONTENT_FORMAT:](#COAP_O_CONTENT_FORMAT:) (const)
- [COAP_O_MAX_AGE:](#COAP_O_MAX_AGE:) (const)
- [COAP_O_URI_QUERY:](#COAP_O_URI_QUERY:) (const)
- [COAP_O_ACCEPT:](#COAP_O_ACCEPT:) (const)
- [COAP_O_LOCATION_QUERY:](#COAP_O_LOCATION_QUERY:) (const)
- [COAP_O_BLOCK2:](#COAP_O_BLOCK2:) (const)
- [COAP_O_BLOCK1:](#COAP_O_BLOCK1:) (const)
- [COAP_O_SIZE2:](#COAP_O_SIZE2:) (const)
- [COAP_O_PROXY_URI:](#COAP_O_PROXY_URI:) (const)
- [COAP_O_PROXY_SCHEME:](#COAP_O_PROXY_SCHEME:) (const)
- [COAP_O_SIZE1:](#COAP_O_SIZE1:) (const)
- [COAP_O_NO_RESPONSE:](#COAP_O_NO_RESPONSE:) (const)
- [COAP_CF_TEXT:](#COAP_CF_TEXT:) (const)
- [COAP_CF_LINK:](#COAP_CF_LINK:) (const)
- [COAP_CF_JSON:](#COAP_CF_JSON:) (const)
- [COAP_CF_CBOR:](#COAP_CF_CBOR:) (const)
- [COAP_ACK_TIMEOUT:](#COAP_ACK_TIMEOUT:) (const)
- [COAP_MAX_RETRANSMIT:](#COAP_MAX_RETRANSMIT:) (const)
- [COAP_EXCHANGE_LIFETIME:](#COAP_EXCHANGE_LIFETIME:) (const)
- [CoapMsg](#CoapMsg) (struct)
- [coap_msg](#coap_msg) (function)
- [coap_code_str](#coap_code_str) (function)
- [coap_is_request](#coap_is_request) (function)
- [coap_is_response](#coap_is_response) (function)
- [coap_opt_critical](#coap_opt_critical) (function)
- [coap_opt_unsafe](#coap_opt_unsafe) (function)
- [coap_opt_no_cache_key](#coap_opt_no_cache_key) (function)
- [coap_uint_bytes](#coap_uint_bytes) (function)
- [coap_bytes_uint](#coap_bytes_uint) (function)
- [coap_add_option](#coap_add_option) (function)
- [coap_add_uint](#coap_add_uint) (function)
- [coap_add_str](#coap_add_str) (function)
- [coap_remove_option](#coap_remove_option) (function)
- [coap_set_uint](#coap_set_uint) (function)
- [coap_has_option](#coap_has_option) (function)
- [coap_get_option](#coap_get_option) (function)
- [coap_get_uint](#coap_get_uint) (function)
- [coap_get_strs](#coap_get_strs) (function)
- [coap_set_uri](#coap_set_uri) (function)
- [coap_uri_path](#coap_uri_path) (function)
- [coap_encode](#coap_encode) (function)
- [CoapDecoded](#CoapDecoded) (struct)
- [coap_decode](#coap_decode) (function)
- [coap_unknown_critical](#coap_unknown_critical) (function)
- [coap_block_value](#coap_block_value) (function)
- [coap_block_num](#coap_block_num) (function)
- [coap_block_more](#coap_block_more) (function)
- [coap_block_szx](#coap_block_szx) (function)
- [coap_block_size](#coap_block_size) (function)
- [coap_block_str](#coap_block_str) (function)
- [coap_block_slice](#coap_block_slice) (function)
- [coap_block2_respond](#coap_block2_respond) (function)
- [CoapBlockAsm](#CoapBlockAsm) (struct)
- [coap_block_asm_new](#coap_block_asm_new) (function)
- [coap_block_asm_add](#coap_block_asm_add) (function)
- [coap_block1_count](#coap_block1_count) (function)
- [coap_block1_request](#coap_block1_request) (function)
- [coap_observe_fresh](#coap_observe_fresh) (function)
- [CoapObservers](#CoapObservers) (struct)
- [coap_observers_new](#coap_observers_new) (function)
- [coap_observers_request](#coap_observers_request) (function)
- [coap_observers_next_seq](#coap_observers_next_seq) (function)
- [coap_observers_notify](#coap_observers_notify) (function)
- [coap_observers_cancel](#coap_observers_cancel) (function)
- [COAP_EV_RESPONSE:](#COAP_EV_RESPONSE:) (const)
- [COAP_EV_TIMEOUT:](#COAP_EV_TIMEOUT:) (const)
- [COAP_EV_RESET:](#COAP_EV_RESET:) (const)
- [COAP_EV_REQUEST:](#COAP_EV_REQUEST:) (const)
- [CoapEvent](#CoapEvent) (struct)
- [CoapEndpoint](#CoapEndpoint) (struct)
- [coap_ep_new](#coap_ep_new) (function)
- [coap_ep_mid](#coap_ep_mid) (function)
- [coap_ep_token](#coap_ep_token) (function)
- [coap_ep_request](#coap_ep_request) (function)
- [coap_ep_forget](#coap_ep_forget) (function)
- [coap_ep_take_outbox](#coap_ep_take_outbox) (function)
- [coap_ep_take_events](#coap_ep_take_events) (function)
- [coap_ep_pending](#coap_ep_pending) (function)
- [coap_ep_receive](#coap_ep_receive) (function)
- [coap_ep_respond](#coap_ep_respond) (function)
- [coap_ep_ack_empty](#coap_ep_ack_empty) (function)
- [coap_ep_send](#coap_ep_send) (function)
- [coap_ep_tick](#coap_ep_tick) (function)

## COAP_CON:

```fides
pub const COAP_CON: i64 = 0;
```

*Defined at line 36*

---

## COAP_NON:

```fides
pub const COAP_NON: i64 = 1;
```

*Defined at line 37*

---

## COAP_ACK:

```fides
pub const COAP_ACK: i64 = 2;
```

*Defined at line 38*

---

## COAP_RST:

```fides
pub const COAP_RST: i64 = 3;
```

*Defined at line 39*

---

## COAP_EMPTY:

```fides
pub const COAP_EMPTY: i64 = 0;
```

*Defined at line 42*

---

## COAP_GET:

```fides
pub const COAP_GET: i64 = 1;
```

*Defined at line 43*

---

## COAP_POST:

```fides
pub const COAP_POST: i64 = 2;
```

*Defined at line 44*

---

## COAP_PUT:

```fides
pub const COAP_PUT: i64 = 3;
```

*Defined at line 45*

---

## COAP_DELETE:

```fides
pub const COAP_DELETE: i64 = 4;
```

*Defined at line 46*

---

## COAP_CREATED:

```fides
pub const COAP_CREATED: i64 = 65;  // 2.01
```

*Defined at line 47*

---

## COAP_DELETED:

```fides
pub const COAP_DELETED: i64 = 66;  // 2.02
```

*Defined at line 48*

---

## COAP_VALID:

```fides
pub const COAP_VALID: i64 = 67;  // 2.03
```

*Defined at line 49*

---

## COAP_CHANGED:

```fides
pub const COAP_CHANGED: i64 = 68;  // 2.04
```

*Defined at line 50*

---

## COAP_CONTENT:

```fides
pub const COAP_CONTENT: i64 = 69;  // 2.05
```

*Defined at line 51*

---

## COAP_CONTINUE:

```fides
pub const COAP_CONTINUE: i64 = 95;  // 2.31 (RFC 7959)
```

*Defined at line 52*

---

## COAP_BAD_REQUEST:

```fides
pub const COAP_BAD_REQUEST: i64 = 128;  // 4.00
```

*Defined at line 53*

---

## COAP_NOT_FOUND:

```fides
pub const COAP_NOT_FOUND: i64 = 132;  // 4.04
```

*Defined at line 54*

---

## COAP_METHOD_NOT_ALLOWED:

```fides
pub const COAP_METHOD_NOT_ALLOWED: i64 = 133;  // 4.05
```

*Defined at line 55*

---

## COAP_REQUEST_INCOMPLETE:

```fides
pub const COAP_REQUEST_INCOMPLETE: i64 = 136;  // 4.08
```

*Defined at line 56*

---

## COAP_TOO_LARGE:

```fides
pub const COAP_TOO_LARGE: i64 = 141;  // 4.13
```

*Defined at line 57*

---

## COAP_INTERNAL_ERROR:

```fides
pub const COAP_INTERNAL_ERROR: i64 = 160;  // 5.00
```

*Defined at line 58*

---

## COAP_O_IF_MATCH:

```fides
pub const COAP_O_IF_MATCH: i64 = 1;
```

*Defined at line 61*

---

## COAP_O_URI_HOST:

```fides
pub const COAP_O_URI_HOST: i64 = 3;
```

*Defined at line 62*

---

## COAP_O_ETAG:

```fides
pub const COAP_O_ETAG: i64 = 4;
```

*Defined at line 63*

---

## COAP_O_IF_NONE_MATCH:

```fides
pub const COAP_O_IF_NONE_MATCH: i64 = 5;
```

*Defined at line 64*

---

## COAP_O_OBSERVE:

```fides
pub const COAP_O_OBSERVE: i64 = 6;
```

*Defined at line 65*

---

## COAP_O_URI_PORT:

```fides
pub const COAP_O_URI_PORT: i64 = 7;
```

*Defined at line 66*

---

## COAP_O_LOCATION_PATH:

```fides
pub const COAP_O_LOCATION_PATH: i64 = 8;
```

*Defined at line 67*

---

## COAP_O_URI_PATH:

```fides
pub const COAP_O_URI_PATH: i64 = 11;
```

*Defined at line 68*

---

## COAP_O_CONTENT_FORMAT:

```fides
pub const COAP_O_CONTENT_FORMAT: i64 = 12;
```

*Defined at line 69*

---

## COAP_O_MAX_AGE:

```fides
pub const COAP_O_MAX_AGE: i64 = 14;
```

*Defined at line 70*

---

## COAP_O_URI_QUERY:

```fides
pub const COAP_O_URI_QUERY: i64 = 15;
```

*Defined at line 71*

---

## COAP_O_ACCEPT:

```fides
pub const COAP_O_ACCEPT: i64 = 17;
```

*Defined at line 72*

---

## COAP_O_LOCATION_QUERY:

```fides
pub const COAP_O_LOCATION_QUERY: i64 = 20;
```

*Defined at line 73*

---

## COAP_O_BLOCK2:

```fides
pub const COAP_O_BLOCK2: i64 = 23;
```

*Defined at line 74*

---

## COAP_O_BLOCK1:

```fides
pub const COAP_O_BLOCK1: i64 = 27;
```

*Defined at line 75*

---

## COAP_O_SIZE2:

```fides
pub const COAP_O_SIZE2: i64 = 28;
```

*Defined at line 76*

---

## COAP_O_PROXY_URI:

```fides
pub const COAP_O_PROXY_URI: i64 = 35;
```

*Defined at line 77*

---

## COAP_O_PROXY_SCHEME:

```fides
pub const COAP_O_PROXY_SCHEME: i64 = 39;
```

*Defined at line 78*

---

## COAP_O_SIZE1:

```fides
pub const COAP_O_SIZE1: i64 = 60;
```

*Defined at line 79*

---

## COAP_O_NO_RESPONSE:

```fides
pub const COAP_O_NO_RESPONSE: i64 = 258;
```

*Defined at line 80*

---

## COAP_CF_TEXT:

```fides
pub const COAP_CF_TEXT: i64 = 0;
```

*Defined at line 83*

---

## COAP_CF_LINK:

```fides
pub const COAP_CF_LINK: i64 = 40;
```

*Defined at line 84*

---

## COAP_CF_JSON:

```fides
pub const COAP_CF_JSON: i64 = 50;
```

*Defined at line 85*

---

## COAP_CF_CBOR:

```fides
pub const COAP_CF_CBOR: i64 = 60;
```

*Defined at line 86*

---

## COAP_ACK_TIMEOUT:

```fides
pub const COAP_ACK_TIMEOUT: i64 = 2000;
```

*Defined at line 89*

---

## COAP_MAX_RETRANSMIT:

```fides
pub const COAP_MAX_RETRANSMIT: i64 = 4;
```

*Defined at line 90*

---

## COAP_EXCHANGE_LIFETIME:

```fides
pub const COAP_EXCHANGE_LIFETIME: i64 = 247000;
```

*Defined at line 91*

---

## CoapMsg

One CoAP message. Options are kept sorted by number (repeated options
in the order they were added).

```fides
pub struct CoapMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `mtype` | `i64` |
| `code` | `i64` |
| `mid` | `i64` |
| `token` | `Vec<i64>` |
| `opt_nums` | `Vec<i64>` |
| `opt_vals` | `Vec<Vec<i64>>` |
| `payload` | `Vec<i64>` |

*Defined at line 95*

---

## coap_msg

```fides
pub fn coap_msg(mtype: i64, code: i64, mid: i64) -> CoapMsg {
```

**Parameters:**

- `mtype`: `i64`
- `code`: `i64`
- `mid`: `i64`

**Returns:** `CoapMsg`

*Defined at line 105*

---

## coap_code_str

"2.05" for 69.

```fides
pub fn coap_code_str(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 112*

---

## coap_is_request

```fides
pub fn coap_is_request(m: &CoapMsg) -> bool {
```

**Parameters:**

- `m`: `&CoapMsg`

**Returns:** `bool`

*Defined at line 121*

---

## coap_is_response

```fides
pub fn coap_is_response(m: &CoapMsg) -> bool {
```

**Parameters:**

- `m`: `&CoapMsg`

**Returns:** `bool`

*Defined at line 125*

---

## coap_opt_critical

```fides
pub fn coap_opt_critical(num: i64) -> bool {
```

**Parameters:**

- `num`: `i64`

**Returns:** `bool`

*Defined at line 130*

---

## coap_opt_unsafe

```fides
pub fn coap_opt_unsafe(num: i64) -> bool {
```

**Parameters:**

- `num`: `i64`

**Returns:** `bool`

*Defined at line 134*

---

## coap_opt_no_cache_key

```fides
pub fn coap_opt_no_cache_key(num: i64) -> bool {
```

**Parameters:**

- `num`: `i64`

**Returns:** `bool`

*Defined at line 138*

---

## coap_uint_bytes

A uint option value in the fewest bytes (0 is no bytes).

```fides
pub fn coap_uint_bytes(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 145*

---

## coap_bytes_uint

```fides
pub fn coap_bytes_uint(b: Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 155*

---

## coap_add_option

Add option `num` with value bytes `val` (after any option <= num).

```fides
pub fn coap_add_option(m: &mut CoapMsg, num: i64, val: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `num`: `i64`
- `val`: `Vec<i64>`

*Defined at line 166*

---

## coap_add_uint

```fides
pub fn coap_add_uint(m: &mut CoapMsg, num: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `num`: `i64`
- `v`: `i64`

*Defined at line 175*

---

## coap_add_str

```fides
pub fn coap_add_str(m: &mut CoapMsg, num: i64, s: str) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `num`: `i64`
- `s`: `str`

*Defined at line 179*

---

## coap_remove_option

Remove every option `num`.

```fides
pub fn coap_remove_option(m: &mut CoapMsg, num: i64) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `num`: `i64`

*Defined at line 184*

---

## coap_set_uint

Replace option `num` by one uint value.

```fides
pub fn coap_set_uint(m: &mut CoapMsg, num: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `num`: `i64`
- `v`: `i64`

*Defined at line 197*

---

## coap_has_option

```fides
pub fn coap_has_option(m: &CoapMsg, num: i64) -> bool {
```

**Parameters:**

- `m`: `&CoapMsg`
- `num`: `i64`

**Returns:** `bool`

*Defined at line 202*

---

## coap_get_option

The first value of option `num` ([] when absent).

```fides
pub fn coap_get_option(m: &CoapMsg, num: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&CoapMsg`
- `num`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 207*

---

## coap_get_uint

A uint option's value, or `def` when absent.

```fides
pub fn coap_get_uint(m: &CoapMsg, num: i64, def: i64) -> i64 {
```

**Parameters:**

- `m`: `&CoapMsg`
- `num`: `i64`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 219*

---

## coap_get_strs

Every value of option `num` as strings.

```fides
pub fn coap_get_strs(m: &CoapMsg, num: i64) -> Vec<str> {
```

**Parameters:**

- `m`: `&CoapMsg`
- `num`: `i64`

**Returns:** `Vec<str>`

*Defined at line 227*

---

## coap_set_uri

Set Uri-Path (and Uri-Query after a '?') from "/a/b?x=1&y=2".

```fides
pub fn coap_set_uri(m: &mut CoapMsg, uri: str) {
```

**Parameters:**

- `m`: `&mut CoapMsg`
- `uri`: `str`

*Defined at line 240*

---

## coap_uri_path

"/a/b" from the Uri-Path options ("/" when none).

```fides
pub fn coap_uri_path(m: &CoapMsg) -> str {
```

**Parameters:**

- `m`: `&CoapMsg`

**Returns:** `str`

*Defined at line 269*

---

## coap_encode

Encode a message (RFC 7252 section 3).

```fides
pub fn coap_encode(m: &CoapMsg) -> Vec<i64> {
```

**Parameters:**

- `m`: `&CoapMsg`

**Returns:** `Vec<i64>`

*Defined at line 306*

---

## CoapDecoded

A decoded message. `ok` false: `error` says why (a message format
error: answer a CON with Reset, ignore a NON).

```fides
pub struct CoapDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `msg` | `CoapMsg` |

*Defined at line 336*

---

## coap_decode

```fides
pub fn coap_decode(b: Vec<i64>) -> CoapDecoded {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `CoapDecoded`

*Defined at line 348*

---

## coap_unknown_critical

The lowest critical option number not in `known`, or 0 when every
critical option is understood (a request with one gets 4.02).

```fides
pub fn coap_unknown_critical(m: &CoapMsg, known: Vec<i64>) -> i64 {
```

**Parameters:**

- `m`: `&CoapMsg`
- `known`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 442*

---

## coap_block_value

A Block1/Block2 option value: NUM << 4 | M << 3 | SZX.

```fides
pub fn coap_block_value(num: i64, more: bool, szx: i64) -> i64 {
```

**Parameters:**

- `num`: `i64`
- `more`: `bool`
- `szx`: `i64`

**Returns:** `i64`

*Defined at line 457*

---

## coap_block_num

```fides
pub fn coap_block_num(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 465*

---

## coap_block_more

```fides
pub fn coap_block_more(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 469*

---

## coap_block_szx

```fides
pub fn coap_block_szx(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 473*

---

## coap_block_size

Block size for SZX (0..6): 16 .. 1024 bytes.

```fides
pub fn coap_block_size(szx: i64) -> i64 {
```

**Parameters:**

- `szx`: `i64`

**Returns:** `i64`

*Defined at line 478*

---

## coap_block_str

The RFC's notation "NUM/M/SIZE", e.g. "2/0/64".

```fides
pub fn coap_block_str(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 483*

---

## coap_block_slice

Bytes of block `num` of `body` with blocks of 2^(szx+4).

```fides
pub fn coap_block_slice(body: &Vec<i64>, num: i64, szx: i64) -> Vec<i64> {
```

**Parameters:**

- `body`: `&Vec<i64>`
- `num`: `i64`
- `szx`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 492*

---

## coap_block2_respond

Server side of Block2: put block `num` of `body` in response `resp`
(payload, Block2, and Size2 on the first block). False when `num` is
past the end (answer 4.02 Bad Option, RFC 7959 2.4).

```fides
pub fn coap_block2_respond(resp: &mut CoapMsg, body: &Vec<i64>, num: i64, szx: i64) -> bool {
```

**Parameters:**

- `resp`: `&mut CoapMsg`
- `body`: `&Vec<i64>`
- `num`: `i64`
- `szx`: `i64`

**Returns:** `bool`

*Defined at line 506*

---

## CoapBlockAsm

A Block2 reassembler (client): add each response; `done` when the
last block came, `next` is the Block2 value to ask for otherwise.

```fides
pub struct CoapBlockAsm {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `next` | `i64` |
| `done` | `bool` |
| `ok` | `bool` |
| `etag` | `Vec<i64>` |

*Defined at line 522*

---

## coap_block_asm_new

```fides
pub fn coap_block_asm_new() -> CoapBlockAsm {
```

**Returns:** `CoapBlockAsm`

*Defined at line 530*

---

## coap_block_asm_add

Add a response. False on a block out of order or an ETag change (the
representation changed: start again).

```fides
pub fn coap_block_asm_add(a: &mut CoapBlockAsm, resp: &CoapMsg) -> bool {
```

**Parameters:**

- `a`: `&mut CoapBlockAsm`
- `resp`: `&CoapMsg`

**Returns:** `bool`

*Defined at line 538*

---

## coap_block1_count

How many Block1 requests an upload of `n` bytes takes.

```fides
pub fn coap_block1_count(n: i64, szx: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `szx`: `i64`

**Returns:** `i64`

*Defined at line 585*

---

## coap_block1_request

Block1 request `num` of an upload: a copy of `base` with the slice and
Block1 (Size1 on the first).

```fides
pub fn coap_block1_request(base: &CoapMsg, body: &Vec<i64>, num: i64, szx: i64) -> CoapMsg {
```

**Parameters:**

- `base`: `&CoapMsg`
- `body`: `&Vec<i64>`
- `num`: `i64`
- `szx`: `i64`

**Returns:** `CoapMsg`

*Defined at line 595*

---

## coap_observe_fresh

Whether a notification (v2 received at t2 seconds) is newer than the
last one (v1 at t1): RFC 7641 section 3.4.

```fides
pub fn coap_observe_fresh(v1: i64, t1: i64, v2: i64, t2: i64) -> bool {
```

**Parameters:**

- `v1`: `i64`
- `t1`: `i64`
- `v2`: `i64`
- `t2`: `i64`

**Returns:** `bool`

*Defined at line 613*

---

## CoapObservers

Observers of the resources of a server: one entry per (path, token).

```fides
pub struct CoapObservers {
```

**Fields:**

| Name | Type |
|------|------|
| `paths` | `Vec<str>` |
| `tokens` | `Vec<Vec<i64>>` |
| `seq` | `i64` |

*Defined at line 625*

---

## coap_observers_new

```fides
pub fn coap_observers_new() -> CoapObservers {
```

**Returns:** `CoapObservers`

*Defined at line 631*

---

## coap_observers_request

Handle a GET's Observe option: 0 registers, 1 deregisters. True when
the response should carry an Observe sequence (registered).

```fides
pub fn coap_observers_request(o: &mut CoapObservers, req: &CoapMsg) -> bool {
```

**Parameters:**

- `o`: `&mut CoapObservers`
- `req`: `&CoapMsg`

**Returns:** `bool`

*Defined at line 639*

---

## coap_observers_next_seq

The next Observe sequence number (24 bits, wrapping).

```fides
pub fn coap_observers_next_seq(o: &mut CoapObservers) -> i64 {
```

**Parameters:**

- `o`: `&mut CoapObservers`

**Returns:** `i64`

*Defined at line 663*

---

## coap_observers_notify

Notifications for a new state of `path`: one 2.05 per observer (type
`mtype`, mids from `first_mid`), all with the same sequence number.

```fides
pub fn coap_observers_notify(o: &mut CoapObservers, path: str, payload: Vec<i64>, content_format: i64, mtype: i64, first_mid: i64) -> Vec<CoapMsg> {
```

**Parameters:**

- `o`: `&mut CoapObservers`
- `path`: `str`
- `payload`: `Vec<i64>`
- `content_format`: `i64`
- `mtype`: `i64`
- `first_mid`: `i64`

**Returns:** `Vec<CoapMsg>`

*Defined at line 671*

---

## coap_observers_cancel

A client answered a notification with Reset: forget that observer.

```fides
pub fn coap_observers_cancel(o: &mut CoapObservers, token: &Vec<i64>) {
```

**Parameters:**

- `o`: `&mut CoapObservers`
- `token`: `&Vec<i64>`

*Defined at line 692*

---

## COAP_EV_RESPONSE:

What happened to a request (CoapEvent.kind).

```fides
pub const COAP_EV_RESPONSE: i64 = 1;  // a response (or notification)
```

*Defined at line 707*

---

## COAP_EV_TIMEOUT:

```fides
pub const COAP_EV_TIMEOUT: i64 = 2;  // no ACK after MAX_RETRANSMIT
```

*Defined at line 708*

---

## COAP_EV_RESET:

```fides
pub const COAP_EV_RESET: i64 = 3;  // the peer answered with RST
```

*Defined at line 709*

---

## COAP_EV_REQUEST:

```fides
pub const COAP_EV_REQUEST: i64 = 4;  // a request for us (server side)
```

*Defined at line 710*

---

## CoapEvent

```fides
pub struct CoapEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `token` | `Vec<i64>` |
| `msg` | `CoapMsg` |

*Defined at line 712*

---

## CoapEndpoint

One endpoint towards one peer (see the module header).

```fides
pub struct CoapEndpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `next_mid` | `i64` |
| `next_token` | `i64` |
| `rng` | `i64` |
| `outbox` | `Vec<Vec<i64>>` |
| `events` | `Vec<CoapEvent>` |
| `rt_mids` | `Vec<i64>` |
| `rt_bytes` | `Vec<Vec<i64>>` |
| `rt_due` | `Vec<i64>` |
| `rt_wait` | `Vec<i64>` |
| `rt_count` | `Vec<i64>` |
| `rt_tokens` | `Vec<Vec<i64>>` |
| `req_tokens` | `Vec<Vec<i64>>` |
| `req_observe` | `Vec<bool>` |
| `seen_mids` | `Vec<i64>` |
| `seen_reply` | `Vec<Vec<i64>>` |
| `seen_at` | `Vec<i64>` |

*Defined at line 719*

---

## coap_ep_new

```fides
pub fn coap_ep_new(first_mid: i64, seed: i64) -> CoapEndpoint {
```

**Parameters:**

- `first_mid`: `i64`
- `seed`: `i64`

**Returns:** `CoapEndpoint`

*Defined at line 741*

---

## coap_ep_mid

```fides
pub fn coap_ep_mid(e: &mut CoapEndpoint) -> i64 {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`

**Returns:** `i64`

*Defined at line 747*

---

## coap_ep_token

A fresh 4-byte token.

```fides
pub fn coap_ep_token(e: &mut CoapEndpoint) -> Vec<i64> {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`

**Returns:** `Vec<i64>`

*Defined at line 754*

---

## coap_ep_request

Send a request: gives it a mid and a token (unless it has one), and
tracks it. The token, to match the response events with.

```fides
pub fn coap_ep_request(e: &mut CoapEndpoint, req: &mut CoapMsg, now: i64) -> Vec<i64> {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `req`: `&mut CoapMsg`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 783*

---

## coap_ep_forget

Stop waiting for responses with this token (ends an observation on
our side; the next notification is answered with Reset).

```fides
pub fn coap_ep_forget(e: &mut CoapEndpoint, token: &Vec<i64>) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `token`: `&Vec<i64>`

*Defined at line 800*

---

## coap_ep_take_outbox

```fides
pub fn coap_ep_take_outbox(e: &mut CoapEndpoint) -> Vec<Vec<i64>> {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 808*

---

## coap_ep_take_events

```fides
pub fn coap_ep_take_events(e: &mut CoapEndpoint) -> Vec<CoapEvent> {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`

**Returns:** `Vec<CoapEvent>`

*Defined at line 814*

---

## coap_ep_pending

Exchanges still open (requests waiting, CONs waiting for an ACK).

```fides
pub fn coap_ep_pending(e: &CoapEndpoint) -> i64 {
```

**Parameters:**

- `e`: `&CoapEndpoint`

**Returns:** `i64`

*Defined at line 821*

---

## coap_ep_receive

Give the endpoint one datagram received from the peer.

```fides
pub fn coap_ep_receive(e: &mut CoapEndpoint, bytes: Vec<i64>, now: i64) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `bytes`: `Vec<i64>`
- `now`: `i64`

*Defined at line 868*

---

## coap_ep_respond

Answer request `req` (from a COAP_EV_REQUEST event): a piggybacked ACK
for a CON, a NON for a NON. `resp` gives code, options and payload.

```fides
pub fn coap_ep_respond(e: &mut CoapEndpoint, req: &CoapMsg, resp: &mut CoapMsg, now: i64) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `req`: `&CoapMsg`
- `resp`: `&mut CoapMsg`
- `now`: `i64`

*Defined at line 944*

---

## coap_ep_ack_empty

Acknowledge a CON request now and answer it later (coap_ep_send_separate).

```fides
pub fn coap_ep_ack_empty(e: &mut CoapEndpoint, req: &CoapMsg) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `req`: `&CoapMsg`

*Defined at line 962*

---

## coap_ep_send

Send a separate response (or a notification) as CON or NON; a CON is
retransmitted until the peer ACKs it.

```fides
pub fn coap_ep_send(e: &mut CoapEndpoint, m: &mut CoapMsg, now: i64) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `m`: `&mut CoapMsg`
- `now`: `i64`

*Defined at line 974*

---

## coap_ep_tick

Let time pass: retransmit CONs whose timeout ran out (doubling it),
give up after MAX_RETRANSMIT, forget old duplicate-detection entries.

```fides
pub fn coap_ep_tick(e: &mut CoapEndpoint, now: i64) {
```

**Parameters:**

- `e`: `&mut CoapEndpoint`
- `now`: `i64`

*Defined at line 985*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
