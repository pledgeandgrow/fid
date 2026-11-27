[All modules](index.md)

# opcua -- Fides Documentation

## Contents

- [UA_BOOLEAN:](#UA_BOOLEAN:) (const)
- [UA_SBYTE:](#UA_SBYTE:) (const)
- [UA_BYTE:](#UA_BYTE:) (const)
- [UA_INT16:](#UA_INT16:) (const)
- [UA_UINT16:](#UA_UINT16:) (const)
- [UA_INT32:](#UA_INT32:) (const)
- [UA_UINT32:](#UA_UINT32:) (const)
- [UA_INT64:](#UA_INT64:) (const)
- [UA_UINT64:](#UA_UINT64:) (const)
- [UA_FLOAT:](#UA_FLOAT:) (const)
- [UA_DOUBLE:](#UA_DOUBLE:) (const)
- [UA_STRING:](#UA_STRING:) (const)
- [UA_DATETIME:](#UA_DATETIME:) (const)
- [UA_BYTESTRING:](#UA_BYTESTRING:) (const)
- [UA_STATUSCODE:](#UA_STATUSCODE:) (const)
- [UA_ID_NUMERIC:](#UA_ID_NUMERIC:) (const)
- [UA_ID_STRING:](#UA_ID_STRING:) (const)
- [UA_ID_GUID:](#UA_ID_GUID:) (const)
- [UA_ID_OPAQUE:](#UA_ID_OPAQUE:) (const)
- [UA_ATTR_VALUE:](#UA_ATTR_VALUE:) (const)
- [UA_GOOD:](#UA_GOOD:) (const)
- [UA_BAD_NODE_ID_UNKNOWN:](#UA_BAD_NODE_ID_UNKNOWN:) (const)
- [UA_READ_REQUEST_ID:](#UA_READ_REQUEST_ID:) (const)
- [UA_READ_RESPONSE_ID:](#UA_READ_RESPONSE_ID:) (const)
- [UA_EPOCH_DELTA:](#UA_EPOCH_DELTA:) (const)
- [ua_put_uint](#ua_put_uint) (function)
- [ua_put_bool](#ua_put_bool) (function)
- [ua_put_byte](#ua_put_byte) (function)
- [ua_put_u16](#ua_put_u16) (function)
- [ua_put_u32](#ua_put_u32) (function)
- [ua_put_i64](#ua_put_i64) (function)
- [ua_put_double](#ua_put_double) (function)
- [ua_put_float](#ua_put_float) (function)
- [ua_put_string](#ua_put_string) (function)
- [ua_put_null_string](#ua_put_null_string) (function)
- [ua_put_bytestring](#ua_put_bytestring) (function)
- [ua_datetime_from_unix_ms](#ua_datetime_from_unix_ms) (function)
- [ua_datetime_to_unix_ms](#ua_datetime_to_unix_ms) (function)
- [ua_f64_to_f32_bits](#ua_f64_to_f32_bits) (function)
- [ua_f32_bits_to_f64](#ua_f32_bits_to_f64) (function)
- [UaReader](#UaReader) (struct)
- [ua_reader](#ua_reader) (function)
- [ua_fail](#ua_fail) (function)
- [ua_get_uint](#ua_get_uint) (function)
- [ua_get_int](#ua_get_int) (function)
- [ua_get_byte](#ua_get_byte) (function)
- [ua_get_u16](#ua_get_u16) (function)
- [ua_get_u32](#ua_get_u32) (function)
- [ua_get_i32](#ua_get_i32) (function)
- [ua_get_i64](#ua_get_i64) (function)
- [ua_get_double](#ua_get_double) (function)
- [ua_get_float](#ua_get_float) (function)
- [ua_get_bytestring](#ua_get_bytestring) (function)
- [ua_get_string](#ua_get_string) (function)
- [UaNodeId](#UaNodeId) (struct)
- [ua_node_num](#ua_node_num) (function)
- [ua_node_str](#ua_node_str) (function)
- [ua_node_guid](#ua_node_guid) (function)
- [ua_guid_bytes](#ua_guid_bytes) (function)
- [ua_guid_text](#ua_guid_text) (function)
- [ua_put_nodeid](#ua_put_nodeid) (function)
- [ua_get_nodeid](#ua_get_nodeid) (function)
- [ua_nodeid_text](#ua_nodeid_text) (function)
- [ua_nodeid_eq](#ua_nodeid_eq) (function)
- [ua_put_qualified_name](#ua_put_qualified_name) (function)
- [ua_put_localized_text](#ua_put_localized_text) (function)
- [UaVariant](#UaVariant) (struct)
- [ua_variant](#ua_variant) (function)
- [ua_var_int](#ua_var_int) (function)
- [ua_var_double](#ua_var_double) (function)
- [ua_var_float](#ua_var_float) (function)
- [ua_var_string](#ua_var_string) (function)
- [ua_var_len](#ua_var_len) (function)
- [ua_put_variant](#ua_put_variant) (function)
- [ua_get_variant](#ua_get_variant) (function)
- [UaDataValue](#UaDataValue) (struct)
- [ua_datavalue](#ua_datavalue) (function)
- [ua_put_datavalue](#ua_put_datavalue) (function)
- [ua_get_datavalue](#ua_get_datavalue) (function)
- [ua_put_empty_extension](#ua_put_empty_extension) (function)
- [ua_message](#ua_message) (function)
- [ua_frame_len](#ua_frame_len) (function)
- [UaHello](#UaHello) (struct)
- [ua_hello](#ua_hello) (function)
- [ua_encode_hello](#ua_encode_hello) (function)
- [ua_decode_hello](#ua_decode_hello) (function)
- [ua_msg_chunks](#ua_msg_chunks) (function)
- [UaMsg](#UaMsg) (struct)
- [ua_msg_join](#ua_msg_join) (function)
- [UaReadRequest](#UaReadRequest) (struct)
- [ua_read_request](#ua_read_request) (function)
- [ua_read_add](#ua_read_add) (function)
- [ua_encode_read_request](#ua_encode_read_request) (function)
- [ua_decode_read_request](#ua_decode_read_request) (function)
- [UaReadResponse](#UaReadResponse) (struct)
- [ua_read_response](#ua_read_response) (function)
- [ua_encode_read_response](#ua_encode_read_response) (function)
- [ua_decode_read_response](#ua_decode_read_response) (function)
- [UA_POLICY_NONE:](#UA_POLICY_NONE:) (const)
- [UA_POLICY_BASIC256SHA256:](#UA_POLICY_BASIC256SHA256:) (const)
- [UA_ALG_RSA_OAEP:](#UA_ALG_RSA_OAEP:) (const)
- [UA_ALG_RSA_SHA256:](#UA_ALG_RSA_SHA256:) (const)
- [UA_MODE_NONE:](#UA_MODE_NONE:) (const)
- [UA_MODE_SIGN:](#UA_MODE_SIGN:) (const)
- [UA_MODE_SIGN_ENCRYPT:](#UA_MODE_SIGN_ENCRYPT:) (const)
- [UA_OPN_REQUEST_ID:](#UA_OPN_REQUEST_ID:) (const)
- [UA_OPN_RESPONSE_ID:](#UA_OPN_RESPONSE_ID:) (const)
- [UA_CLO_REQUEST_ID:](#UA_CLO_REQUEST_ID:) (const)
- [UA_SERVICE_FAULT_ID:](#UA_SERVICE_FAULT_ID:) (const)
- [UA_CREATE_SESSION_REQUEST_ID:](#UA_CREATE_SESSION_REQUEST_ID:) (const)
- [UA_CREATE_SESSION_RESPONSE_ID:](#UA_CREATE_SESSION_RESPONSE_ID:) (const)
- [UA_ACTIVATE_SESSION_REQUEST_ID:](#UA_ACTIVATE_SESSION_REQUEST_ID:) (const)
- [UA_ACTIVATE_SESSION_RESPONSE_ID:](#UA_ACTIVATE_SESSION_RESPONSE_ID:) (const)
- [UA_CLOSE_SESSION_REQUEST_ID:](#UA_CLOSE_SESSION_REQUEST_ID:) (const)
- [UA_CLOSE_SESSION_RESPONSE_ID:](#UA_CLOSE_SESSION_RESPONSE_ID:) (const)
- [UA_ANONYMOUS_TOKEN_ID:](#UA_ANONYMOUS_TOKEN_ID:) (const)
- [UA_USERNAME_TOKEN_ID:](#UA_USERNAME_TOKEN_ID:) (const)
- [UA_TOKEN_ANONYMOUS:](#UA_TOKEN_ANONYMOUS:) (const)
- [UA_TOKEN_USERNAME:](#UA_TOKEN_USERNAME:) (const)
- [UA_BAD_SECURITY_CHECKS_FAILED:](#UA_BAD_SECURITY_CHECKS_FAILED:) (const)
- [UA_BAD_USER_ACCESS_DENIED:](#UA_BAD_USER_ACCESS_DENIED:) (const)
- [UA_BAD_IDENTITY_TOKEN_INVALID:](#UA_BAD_IDENTITY_TOKEN_INVALID:) (const)
- [UA_BAD_IDENTITY_TOKEN_REJECTED:](#UA_BAD_IDENTITY_TOKEN_REJECTED:) (const)
- [UA_BAD_SESSION_ID_INVALID:](#UA_BAD_SESSION_ID_INVALID:) (const)
- [ua_p_sha256](#ua_p_sha256) (function)
- [UaKeys](#UaKeys) (struct)
- [ua_no_private_key](#ua_no_private_key) (function)
- [ua_no_keys](#ua_no_keys) (function)
- [ua_derive_keys](#ua_derive_keys) (function)
- [ua_aes_tables](#ua_aes_tables) (function)
- [ua_aes_round_keys](#ua_aes_round_keys) (function)
- [ua_aes_decrypt_block](#ua_aes_decrypt_block) (function)
- [ua_aes_cbc_encrypt](#ua_aes_cbc_encrypt) (function)
- [ua_aes_cbc_decrypt](#ua_aes_cbc_decrypt) (function)
- [UaRng](#UaRng) (struct)
- [ua_rng](#ua_rng) (function)
- [ua_rng_bytes](#ua_rng_bytes) (function)
- [UaRequestHeader](#UaRequestHeader) (struct)
- [ua_request_header](#ua_request_header) (function)
- [ua_put_request_header](#ua_put_request_header) (function)
- [ua_get_request_header](#ua_get_request_header) (function)
- [UaResponseHeader](#UaResponseHeader) (struct)
- [ua_put_response_header](#ua_put_response_header) (function)
- [ua_get_response_header](#ua_get_response_header) (function)
- [ua_body_type](#ua_body_type) (function)
- [ua_encode_service_fault](#ua_encode_service_fault) (function)
- [UaOpnRequest](#UaOpnRequest) (struct)
- [ua_encode_opn_request](#ua_encode_opn_request) (function)
- [ua_decode_opn_request](#ua_decode_opn_request) (function)
- [UaOpnResponse](#UaOpnResponse) (struct)
- [ua_encode_opn_response](#ua_encode_opn_response) (function)
- [ua_decode_opn_response](#ua_decode_opn_response) (function)
- [ua_encode_clo_request](#ua_encode_clo_request) (function)
- [UaChannel](#UaChannel) (struct)
- [ua_channel](#ua_channel) (function)
- [ua_cert_public_key](#ua_cert_public_key) (function)
- [ua_channel_set_peer](#ua_channel_set_peer) (function)
- [ua_thumbprint](#ua_thumbprint) (function)
- [ua_channel_nonce](#ua_channel_nonce) (function)
- [ua_channel_install](#ua_channel_install) (function)
- [ua_channel_issue](#ua_channel_issue) (function)
- [UaOpened](#UaOpened) (struct)
- [ua_message_type](#ua_message_type) (function)
- [ua_opn_seal](#ua_opn_seal) (function)
- [ua_opn_open](#ua_opn_open) (function)
- [ua_sym_seal](#ua_sym_seal) (function)
- [ua_sym_open](#ua_sym_open) (function)
- [ua_get_localized_text](#ua_get_localized_text) (function)
- [UaCreateSessionRequest](#UaCreateSessionRequest) (struct)
- [ua_encode_create_session_request](#ua_encode_create_session_request) (function)
- [ua_decode_create_session_request](#ua_decode_create_session_request) (function)
- [UaEndpoint](#UaEndpoint) (struct)
- [ua_put_endpoint](#ua_put_endpoint) (function)
- [ua_get_endpoint](#ua_get_endpoint) (function)
- [UaCreateSessionResponse](#UaCreateSessionResponse) (struct)
- [ua_encode_create_session_response](#ua_encode_create_session_response) (function)
- [ua_decode_create_session_response](#ua_decode_create_session_response) (function)
- [UaActivateSessionRequest](#UaActivateSessionRequest) (struct)
- [ua_encode_activate_session_request](#ua_encode_activate_session_request) (function)
- [ua_decode_activate_session_request](#ua_decode_activate_session_request) (function)
- [UaActivateSessionResponse](#UaActivateSessionResponse) (struct)
- [ua_encode_activate_session_response](#ua_encode_activate_session_response) (function)
- [ua_decode_activate_session_response](#ua_decode_activate_session_response) (function)
- [ua_encode_close_session_request](#ua_encode_close_session_request) (function)
- [ua_decode_close_session_request](#ua_decode_close_session_request) (function)
- [ua_encode_close_session_response](#ua_encode_close_session_response) (function)
- [ua_decode_response_header](#ua_decode_response_header) (function)
- [ua_session_sign](#ua_session_sign) (function)
- [ua_session_verify](#ua_session_verify) (function)
- [ua_password_encrypt](#ua_password_encrypt) (function)
- [ua_password_decrypt](#ua_password_decrypt) (function)
- [UA_CL_IDLE:](#UA_CL_IDLE:) (const)
- [UA_CL_HELLO:](#UA_CL_HELLO:) (const)
- [UA_CL_OPENING:](#UA_CL_OPENING:) (const)
- [UA_CL_CREATING:](#UA_CL_CREATING:) (const)
- [UA_CL_ACTIVATING:](#UA_CL_ACTIVATING:) (const)
- [UA_CL_ACTIVE:](#UA_CL_ACTIVE:) (const)
- [UA_CL_CLOSING:](#UA_CL_CLOSING:) (const)
- [UA_CL_CLOSED:](#UA_CL_CLOSED:) (const)
- [UA_CL_FAILED:](#UA_CL_FAILED:) (const)
- [UaClient](#UaClient) (struct)
- [ua_client](#ua_client) (function)
- [ua_client_start](#ua_client_start) (function)
- [ua_client_renew](#ua_client_renew) (function)
- [ua_client_needs_renew](#ua_client_needs_renew) (function)
- [ua_client_read](#ua_client_read) (function)
- [ua_client_close](#ua_client_close) (function)
- [ua_take_frame](#ua_take_frame) (function)
- [ua_client_feed](#ua_client_feed) (function)

## UA_BOOLEAN:

```fides
pub const UA_BOOLEAN: i64 = 1;
```

*Defined at line 78*

---

## UA_SBYTE:

```fides
pub const UA_SBYTE: i64 = 2;
```

*Defined at line 79*

---

## UA_BYTE:

```fides
pub const UA_BYTE: i64 = 3;
```

*Defined at line 80*

---

## UA_INT16:

```fides
pub const UA_INT16: i64 = 4;
```

*Defined at line 81*

---

## UA_UINT16:

```fides
pub const UA_UINT16: i64 = 5;
```

*Defined at line 82*

---

## UA_INT32:

```fides
pub const UA_INT32: i64 = 6;
```

*Defined at line 83*

---

## UA_UINT32:

```fides
pub const UA_UINT32: i64 = 7;
```

*Defined at line 84*

---

## UA_INT64:

```fides
pub const UA_INT64: i64 = 8;
```

*Defined at line 85*

---

## UA_UINT64:

```fides
pub const UA_UINT64: i64 = 9;
```

*Defined at line 86*

---

## UA_FLOAT:

```fides
pub const UA_FLOAT: i64 = 10;
```

*Defined at line 87*

---

## UA_DOUBLE:

```fides
pub const UA_DOUBLE: i64 = 11;
```

*Defined at line 88*

---

## UA_STRING:

```fides
pub const UA_STRING: i64 = 12;
```

*Defined at line 89*

---

## UA_DATETIME:

```fides
pub const UA_DATETIME: i64 = 13;
```

*Defined at line 90*

---

## UA_BYTESTRING:

```fides
pub const UA_BYTESTRING: i64 = 15;
```

*Defined at line 91*

---

## UA_STATUSCODE:

```fides
pub const UA_STATUSCODE: i64 = 19;
```

*Defined at line 92*

---

## UA_ID_NUMERIC:

```fides
pub const UA_ID_NUMERIC: i64 = 0;
```

*Defined at line 95*

---

## UA_ID_STRING:

```fides
pub const UA_ID_STRING: i64 = 1;
```

*Defined at line 96*

---

## UA_ID_GUID:

```fides
pub const UA_ID_GUID: i64 = 2;
```

*Defined at line 97*

---

## UA_ID_OPAQUE:

```fides
pub const UA_ID_OPAQUE: i64 = 3;
```

*Defined at line 98*

---

## UA_ATTR_VALUE:

```fides
pub const UA_ATTR_VALUE: i64 = 13;
```

*Defined at line 101*

---

## UA_GOOD:

```fides
pub const UA_GOOD: i64 = 0;
```

*Defined at line 102*

---

## UA_BAD_NODE_ID_UNKNOWN:

```fides
pub const UA_BAD_NODE_ID_UNKNOWN: i64 = 0x80340000;
```

*Defined at line 103*

---

## UA_READ_REQUEST_ID:

```fides
pub const UA_READ_REQUEST_ID: i64 = 631;
```

*Defined at line 104*

---

## UA_READ_RESPONSE_ID:

```fides
pub const UA_READ_RESPONSE_ID: i64 = 634;
```

*Defined at line 105*

---

## UA_EPOCH_DELTA:

Ticks (100 ns) from 1601-01-01 to 1970-01-01.

```fides
pub const UA_EPOCH_DELTA: i64 = 116444736000000000;
```

*Defined at line 108*

---

## ua_put_uint

```fides
pub fn ua_put_uint(out: &mut Vec<i64>, v: i64, width: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`
- `width`: `i64`

*Defined at line 113*

---

## ua_put_bool

```fides
pub fn ua_put_bool(out: &mut Vec<i64>, b: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `b`: `bool`

*Defined at line 121*

---

## ua_put_byte

```fides
pub fn ua_put_byte(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 129*

---

## ua_put_u16

```fides
pub fn ua_put_u16(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 133*

---

## ua_put_u32

```fides
pub fn ua_put_u32(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 137*

---

## ua_put_i64

```fides
pub fn ua_put_i64(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 141*

---

## ua_put_double

```fides
pub fn ua_put_double(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 145*

---

## ua_put_float

```fides
pub fn ua_put_float(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 149*

---

## ua_put_string

A String: Int32 length, UTF-8 bytes.

```fides
pub fn ua_put_string(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 154*

---

## ua_put_null_string

The null String (length -1).

```fides
pub fn ua_put_null_string(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 165*

---

## ua_put_bytestring

```fides
pub fn ua_put_bytestring(out: &mut Vec<i64>, b: &Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `b`: `&Vec<i64>`

*Defined at line 169*

---

## ua_datetime_from_unix_ms

A DateTime from milliseconds since 1970 (UTC).

```fides
pub fn ua_datetime_from_unix_ms(ms: i64) -> i64 {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `i64`

*Defined at line 179*

---

## ua_datetime_to_unix_ms

```fides
pub fn ua_datetime_to_unix_ms(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 183*

---

## ua_f64_to_f32_bits

```fides
pub fn ua_f64_to_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 191*

---

## ua_f32_bits_to_f64

```fides
pub fn ua_f32_bits_to_f64(s: i64) -> f64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `f64`

*Defined at line 237*

---

## UaReader

A cursor over encoded bytes; `ok` turns false on truncated or bad data.

```fides
pub struct UaReader {
```

**Fields:**

| Name | Type |
|------|------|
| `b` | `Vec<i64>` |
| `pos` | `i64` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 278*

---

## ua_reader

```fides
pub fn ua_reader(b: Vec<i64>) -> UaReader {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaReader`

*Defined at line 285*

---

## ua_fail

```fides
pub fn ua_fail(r: &mut UaReader, why: str) {
```

**Parameters:**

- `r`: `&mut UaReader`
- `why`: `str`

*Defined at line 291*

---

## ua_get_uint

```fides
pub fn ua_get_uint(r: &mut UaReader, width: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 299*

---

## ua_get_int

```fides
pub fn ua_get_int(r: &mut UaReader, width: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 317*

---

## ua_get_byte

```fides
pub fn ua_get_byte(r: &mut UaReader) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `i64`

*Defined at line 325*

---

## ua_get_u16

```fides
pub fn ua_get_u16(r: &mut UaReader) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `i64`

*Defined at line 329*

---

## ua_get_u32

```fides
pub fn ua_get_u32(r: &mut UaReader) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `i64`

*Defined at line 333*

---

## ua_get_i32

```fides
pub fn ua_get_i32(r: &mut UaReader) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `i64`

*Defined at line 337*

---

## ua_get_i64

```fides
pub fn ua_get_i64(r: &mut UaReader) -> i64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `i64`

*Defined at line 341*

---

## ua_get_double

```fides
pub fn ua_get_double(r: &mut UaReader) -> f64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `f64`

*Defined at line 345*

---

## ua_get_float

```fides
pub fn ua_get_float(r: &mut UaReader) -> f64 {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `f64`

*Defined at line 349*

---

## ua_get_bytestring

A ByteString (a null one, length -1, reads as []).

```fides
pub fn ua_get_bytestring(r: &mut UaReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `Vec<i64>`

*Defined at line 354*

---

## ua_get_string

A String (a null String reads as "").

```fides
pub fn ua_get_string(r: &mut UaReader) -> str {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `str`

*Defined at line 374*

---

## UaNodeId

```fides
pub struct UaNodeId {
```

**Fields:**

| Name | Type |
|------|------|
| `ns` | `i64` |
| `kind` | `i64,  // UA_ID_*` |
| `num` | `i64,  // numeric identifier` |
| `text` | `str,  // string identifier` |
| `bytes` | `Vec<i64>,  // Guid (16 bytes as encoded) or opaque identifier` |

*Defined at line 380*

---

## ua_node_num

```fides
pub fn ua_node_num(ns: i64, id: i64) -> UaNodeId {
```

**Parameters:**

- `ns`: `i64`
- `id`: `i64`

**Returns:** `UaNodeId`

*Defined at line 388*

---

## ua_node_str

```fides
pub fn ua_node_str(ns: i64, s: str) -> UaNodeId {
```

**Parameters:**

- `ns`: `i64`
- `s`: `str`

**Returns:** `UaNodeId`

*Defined at line 394*

---

## ua_node_guid

A Guid NodeId from its text form "72962B91-FA75-4AE6-8D28-B404DC7DAF63".

```fides
pub fn ua_node_guid(ns: i64, guid: str) -> UaNodeId {
```

**Parameters:**

- `ns`: `i64`
- `guid`: `str`

**Returns:** `UaNodeId`

*Defined at line 401*

---

## ua_guid_bytes

The 16 encoded bytes of a Guid: Data1 (4), Data2 (2), Data3 (2)
little-endian, then Data4 (8) as written.

```fides
pub fn ua_guid_bytes(g: str) -> Vec<i64> {
```

**Parameters:**

- `g`: `str`

**Returns:** `Vec<i64>`

*Defined at line 422*

---

## ua_guid_text

The text form of an encoded Guid (uppercase).

```fides
pub fn ua_guid_text(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 459*

---

## ua_put_nodeid

Encode a NodeId (shortest numeric form: TwoByte, FourByte, Numeric).

```fides
pub fn ua_put_nodeid(out: &mut Vec<i64>, n: &UaNodeId) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `&UaNodeId`

*Defined at line 492*

---

## ua_get_nodeid

Decode a NodeId (an ExpandedNodeId's namespace URI and server index,
if flagged, are read and dropped).

```fides
pub fn ua_get_nodeid(r: &mut UaReader) -> UaNodeId {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaNodeId`

*Defined at line 527*

---

## ua_nodeid_text

"ns=1;s=Hot", "i=85", "ns=4;g=...", "ns=2;b=<hex>" (Part 6 5.3.1.10).

```fides
pub fn ua_nodeid_text(n: &UaNodeId) -> str {
```

**Parameters:**

- `n`: `&UaNodeId`

**Returns:** `str`

*Defined at line 568*

---

## ua_nodeid_eq

```fides
pub fn ua_nodeid_eq(a: &UaNodeId, b: &UaNodeId) -> bool {
```

**Parameters:**

- `a`: `&UaNodeId`
- `b`: `&UaNodeId`

**Returns:** `bool`

*Defined at line 593*

---

## ua_put_qualified_name

```fides
pub fn ua_put_qualified_name(out: &mut Vec<i64>, ns: i64, name: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `ns`: `i64`
- `name`: `str`

*Defined at line 609*

---

## ua_put_localized_text

LocalizedText with the fields that are not empty.

```fides
pub fn ua_put_localized_text(out: &mut Vec<i64>, locale: str, text: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `locale`: `str`
- `text`: `str`

*Defined at line 615*

---

## UaVariant

A Variant: type id, scalar or one-dimensional array; values in `ints`
(Boolean .. UInt64, DateTime, StatusCode), `floats` (Float, Double),
`strs` (String) or `bins` (ByteString). vtype 0 is the null Variant.

```fides
pub struct UaVariant {
```

**Fields:**

| Name | Type |
|------|------|
| `vtype` | `i64` |
| `is_array` | `bool` |
| `ints` | `Vec<i64>` |
| `floats` | `Vec<f64>` |
| `strs` | `Vec<str>` |
| `bins` | `Vec<Vec<i64>>` |

*Defined at line 637*

---

## ua_variant

```fides
pub fn ua_variant(vtype: i64) -> UaVariant {
```

**Parameters:**

- `vtype`: `i64`

**Returns:** `UaVariant`

*Defined at line 646*

---

## ua_var_int

```fides
pub fn ua_var_int(vtype: i64, v: i64) -> UaVariant {
```

**Parameters:**

- `vtype`: `i64`
- `v`: `i64`

**Returns:** `UaVariant`

*Defined at line 652*

---

## ua_var_double

```fides
pub fn ua_var_double(v: f64) -> UaVariant {
```

**Parameters:**

- `v`: `f64`

**Returns:** `UaVariant`

*Defined at line 658*

---

## ua_var_float

```fides
pub fn ua_var_float(v: f64) -> UaVariant {
```

**Parameters:**

- `v`: `f64`

**Returns:** `UaVariant`

*Defined at line 664*

---

## ua_var_string

```fides
pub fn ua_var_string(s: str) -> UaVariant {
```

**Parameters:**

- `s`: `str`

**Returns:** `UaVariant`

*Defined at line 670*

---

## ua_var_len

Number of values held.

```fides
pub fn ua_var_len(v: &UaVariant) -> i64 {
```

**Parameters:**

- `v`: `&UaVariant`

**Returns:** `i64`

*Defined at line 677*

---

## ua_put_variant

```fides
pub fn ua_put_variant(out: &mut Vec<i64>, v: &UaVariant) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `&UaVariant`

*Defined at line 723*

---

## ua_get_variant

```fides
pub fn ua_get_variant(r: &mut UaReader) -> UaVariant {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaVariant`

*Defined at line 760*

---

## UaDataValue

```fides
pub struct UaDataValue {
```

**Fields:**

| Name | Type |
|------|------|
| `has_value` | `bool` |
| `value` | `UaVariant` |
| `has_status` | `bool` |
| `status` | `i64` |
| `has_source_ts` | `bool` |
| `source_ts` | `i64` |
| `has_server_ts` | `bool` |
| `server_ts` | `i64` |
| `source_pico` | `i64,  // -1: absent` |
| `server_pico` | `i64` |

*Defined at line 798*

---

## ua_datavalue

```fides
pub fn ua_datavalue(v: UaVariant) -> UaDataValue {
```

**Parameters:**

- `v`: `UaVariant`

**Returns:** `UaDataValue`

*Defined at line 811*

---

## ua_put_datavalue

```fides
pub fn ua_put_datavalue(out: &mut Vec<i64>, d: &UaDataValue) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `d`: `&UaDataValue`

*Defined at line 817*

---

## ua_get_datavalue

```fides
pub fn ua_get_datavalue(r: &mut UaReader) -> UaDataValue {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaDataValue`

*Defined at line 858*

---

## ua_put_empty_extension

An ExtensionObject with no body (TypeId i=0, encoding 0).

```fides
pub fn ua_put_empty_extension(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 887*

---

## ua_message

A message (or chunk): 3-letter type, chunk type 'F'/'C'/'A' and body;
the size field is computed.

```fides
pub fn ua_message(mtype: str, chunk: i64, body: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `mtype`: `str`
- `chunk`: `i64`
- `body`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 936*

---

## ua_frame_len

The size of the first whole message in `buf`: 0 while incomplete.

```fides
pub fn ua_frame_len(buf: &Vec<i64>) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 954*

---

## UaHello

```fides
pub struct UaHello {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `recv_buf` | `i64` |
| `send_buf` | `i64` |
| `max_msg` | `i64` |
| `max_chunks` | `i64` |
| `url` | `str,  // Hello only` |
| `mtype` | `str,  // "HEL", "ACK" or "ERR"` |
| `error` | `i64,  // ERR: the status code` |
| `reason` | `str,  // ERR: the reason` |
| `ok` | `bool` |

*Defined at line 976*

---

## ua_hello

```fides
pub fn ua_hello(url: str, buf_size: i64) -> UaHello {
```

**Parameters:**

- `url`: `str`
- `buf_size`: `i64`

**Returns:** `UaHello`

*Defined at line 989*

---

## ua_encode_hello

Encode Hello ("HEL") or Acknowledge ("ACK") per h.mtype, or Error.

```fides
pub fn ua_encode_hello(h: &UaHello) -> Vec<i64> {
```

**Parameters:**

- `h`: `&UaHello`

**Returns:** `Vec<i64>`

*Defined at line 996*

---

## ua_decode_hello

Decode a HEL, ACK or ERR message.

```fides
pub fn ua_decode_hello(b: Vec<i64>) -> UaHello {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaHello`

*Defined at line 1015*

---

## ua_msg_chunks

The chunks of one MSG with body `body`: each at most `max_chunk` bytes
(header 8 + security 8 + sequence 8 + part); 'C' then a final 'F'.
Sequence numbers start at `seq`.

```fides
pub fn ua_msg_chunks(channel: i64, token: i64, seq: i64, request_id: i64, body: &Vec<i64>, max_chunk: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `channel`: `i64`
- `token`: `i64`
- `seq`: `i64`
- `request_id`: `i64`
- `body`: `&Vec<i64>`
- `max_chunk`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 1057*

---

## UaMsg

One message joined from its chunks.

```fides
pub struct UaMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `channel` | `i64` |
| `token` | `i64` |
| `first_seq` | `i64` |
| `request_id` | `i64` |
| `body` | `Vec<i64>` |

*Defined at line 1088*

---

## ua_msg_join

Join MSG chunks (in order, one request id, consecutive sequence
numbers, the last one 'F'). An 'A' chunk aborts the message.

```fides
pub fn ua_msg_join(chunks: &Vec<Vec<i64>>) -> UaMsg {
```

**Parameters:**

- `chunks`: `&Vec<Vec<i64>>`

**Returns:** `UaMsg`

*Defined at line 1100*

---

## UaReadRequest

```fides
pub struct UaReadRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `auth_token` | `UaNodeId` |
| `timestamp` | `i64` |
| `handle` | `i64` |
| `timeout_hint` | `i64` |
| `max_age` | `f64` |
| `timestamps` | `i64,  // 0 source, 1 server, 2 both, 3 neither` |
| `nodes` | `Vec<UaNodeId>` |
| `attrs` | `Vec<i64>` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 1151*

---

## ua_read_request

```fides
pub fn ua_read_request(handle: i64, timestamp: i64) -> UaReadRequest {
```

**Parameters:**

- `handle`: `i64`
- `timestamp`: `i64`

**Returns:** `UaReadRequest`

*Defined at line 1164*

---

## ua_read_add

Ask for attribute `attr` of `node` (UA_ATTR_VALUE for the value).

```fides
pub fn ua_read_add(q: &mut UaReadRequest, node: UaNodeId, attr: i64) {
```

**Parameters:**

- `q`: `&mut UaReadRequest`
- `node`: `UaNodeId`
- `attr`: `i64`

*Defined at line 1171*

---

## ua_encode_read_request

The body of a ReadRequest: its encoding NodeId (i=631) and the request.

```fides
pub fn ua_encode_read_request(q: &UaReadRequest) -> Vec<i64> {
```

**Parameters:**

- `q`: `&UaReadRequest`

**Returns:** `Vec<i64>`

*Defined at line 1177*

---

## ua_decode_read_request

```fides
pub fn ua_decode_read_request(b: Vec<i64>) -> UaReadRequest {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaReadRequest`

*Defined at line 1201*

---

## UaReadResponse

```fides
pub struct UaReadResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `timestamp` | `i64` |
| `handle` | `i64` |
| `service_result` | `i64` |
| `results` | `Vec<UaDataValue>` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 1239*

---

## ua_read_response

```fides
pub fn ua_read_response(handle: i64, timestamp: i64) -> UaReadResponse {
```

**Parameters:**

- `handle`: `i64`
- `timestamp`: `i64`

**Returns:** `UaReadResponse`

*Defined at line 1248*

---

## ua_encode_read_response

The body of a ReadResponse (encoding NodeId i=634).

```fides
pub fn ua_encode_read_response(p: &UaReadResponse) -> Vec<i64> {
```

**Parameters:**

- `p`: `&UaReadResponse`

**Returns:** `Vec<i64>`

*Defined at line 1255*

---

## ua_decode_read_response

```fides
pub fn ua_decode_read_response(b: Vec<i64>) -> UaReadResponse {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaReadResponse`

*Defined at line 1274*

---

## UA_POLICY_NONE:

```fides
pub const UA_POLICY_NONE: str = "http://opcfoundation.org/UA/SecurityPolicy#None";
```

*Defined at line 1315*

---

## UA_POLICY_BASIC256SHA256:

```fides
pub const UA_POLICY_BASIC256SHA256: str = "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256";
```

*Defined at line 1316*

---

## UA_ALG_RSA_OAEP:

```fides
pub const UA_ALG_RSA_OAEP: str = "http://www.w3.org/2001/04/xmlenc#rsa-oaep";
```

*Defined at line 1317*

---

## UA_ALG_RSA_SHA256:

```fides
pub const UA_ALG_RSA_SHA256: str = "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256";
```

*Defined at line 1318*

---

## UA_MODE_NONE:

```fides
pub const UA_MODE_NONE: i64 = 1;
```

*Defined at line 1321*

---

## UA_MODE_SIGN:

```fides
pub const UA_MODE_SIGN: i64 = 2;
```

*Defined at line 1322*

---

## UA_MODE_SIGN_ENCRYPT:

```fides
pub const UA_MODE_SIGN_ENCRYPT: i64 = 3;
```

*Defined at line 1323*

---

## UA_OPN_REQUEST_ID:

```fides
pub const UA_OPN_REQUEST_ID: i64 = 446;
```

*Defined at line 1326*

---

## UA_OPN_RESPONSE_ID:

```fides
pub const UA_OPN_RESPONSE_ID: i64 = 449;
```

*Defined at line 1327*

---

## UA_CLO_REQUEST_ID:

```fides
pub const UA_CLO_REQUEST_ID: i64 = 452;
```

*Defined at line 1328*

---

## UA_SERVICE_FAULT_ID:

```fides
pub const UA_SERVICE_FAULT_ID: i64 = 397;
```

*Defined at line 1329*

---

## UA_CREATE_SESSION_REQUEST_ID:

```fides
pub const UA_CREATE_SESSION_REQUEST_ID: i64 = 461;
```

*Defined at line 1330*

---

## UA_CREATE_SESSION_RESPONSE_ID:

```fides
pub const UA_CREATE_SESSION_RESPONSE_ID: i64 = 464;
```

*Defined at line 1331*

---

## UA_ACTIVATE_SESSION_REQUEST_ID:

```fides
pub const UA_ACTIVATE_SESSION_REQUEST_ID: i64 = 467;
```

*Defined at line 1332*

---

## UA_ACTIVATE_SESSION_RESPONSE_ID:

```fides
pub const UA_ACTIVATE_SESSION_RESPONSE_ID: i64 = 470;
```

*Defined at line 1333*

---

## UA_CLOSE_SESSION_REQUEST_ID:

```fides
pub const UA_CLOSE_SESSION_REQUEST_ID: i64 = 473;
```

*Defined at line 1334*

---

## UA_CLOSE_SESSION_RESPONSE_ID:

```fides
pub const UA_CLOSE_SESSION_RESPONSE_ID: i64 = 476;
```

*Defined at line 1335*

---

## UA_ANONYMOUS_TOKEN_ID:

```fides
pub const UA_ANONYMOUS_TOKEN_ID: i64 = 321;
```

*Defined at line 1336*

---

## UA_USERNAME_TOKEN_ID:

```fides
pub const UA_USERNAME_TOKEN_ID: i64 = 324;
```

*Defined at line 1337*

---

## UA_TOKEN_ANONYMOUS:

```fides
pub const UA_TOKEN_ANONYMOUS: i64 = 0;
```

*Defined at line 1340*

---

## UA_TOKEN_USERNAME:

```fides
pub const UA_TOKEN_USERNAME: i64 = 1;
```

*Defined at line 1341*

---

## UA_BAD_SECURITY_CHECKS_FAILED:

```fides
pub const UA_BAD_SECURITY_CHECKS_FAILED: i64 = 0x80130000;
```

*Defined at line 1344*

---

## UA_BAD_USER_ACCESS_DENIED:

```fides
pub const UA_BAD_USER_ACCESS_DENIED: i64 = 0x801F0000;
```

*Defined at line 1345*

---

## UA_BAD_IDENTITY_TOKEN_INVALID:

```fides
pub const UA_BAD_IDENTITY_TOKEN_INVALID: i64 = 0x80200000;
```

*Defined at line 1346*

---

## UA_BAD_IDENTITY_TOKEN_REJECTED:

```fides
pub const UA_BAD_IDENTITY_TOKEN_REJECTED: i64 = 0x80210000;
```

*Defined at line 1347*

---

## UA_BAD_SESSION_ID_INVALID:

```fides
pub const UA_BAD_SESSION_ID_INVALID: i64 = 0x80250000;
```

*Defined at line 1348*

---

## ua_p_sha256

P_SHA256(secret, seed) (RFC 5246 5): HMAC-SHA256(secret, A(i) + seed)
with A(0) = seed, A(i) = HMAC-SHA256(secret, A(i-1)), `len` bytes.

```fides
pub fn ua_p_sha256(secret: Vec<i64>, seed: Vec<i64>, len: i64) -> Vec<i64> {
```

**Parameters:**

- `secret`: `Vec<i64>`
- `seed`: `Vec<i64>`
- `len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1392*

---

## UaKeys

The symmetric keys of one direction of a channel.

```fides
pub struct UaKeys {
```

**Fields:**

| Name | Type |
|------|------|
| `sign` | `Vec<i64>,  // HMAC-SHA256 key, 32 bytes` |
| `enc` | `Vec<i64>,  // AES-256 key, 32 bytes` |
| `iv` | `Vec<i64>,  // AES-CBC IV, 16 bytes` |

*Defined at line 1408*

---

## ua_no_private_key

The RsaPrivateKey of a channel end without one (SecurityPolicy None).

```fides
pub fn ua_no_private_key() -> RsaPrivateKey {
```

**Returns:** `RsaPrivateKey`

*Defined at line 1415*

---

## ua_no_keys

```fides
pub fn ua_no_keys() -> UaKeys {
```

**Returns:** `UaKeys`

*Defined at line 1419*

---

## ua_derive_keys

Basic256Sha256 keys: P_SHA256(secret, seed) split 32 / 32 / 16. The
client's keys are ua_derive_keys(serverNonce, clientNonce), the
server's ua_derive_keys(clientNonce, serverNonce).

```fides
pub fn ua_derive_keys(secret: Vec<i64>, seed: Vec<i64>) -> UaKeys {
```

**Parameters:**

- `secret`: `Vec<i64>`
- `seed`: `Vec<i64>`

**Returns:** `UaKeys`

*Defined at line 1428*

---

## ua_aes_tables

The S-box (entries 0..255) and its inverse (256..511), generated from
the multiplicative group (powers of 3) and the affine map.

```fides
pub fn ua_aes_tables() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 1463*

---

## ua_aes_round_keys

Round keys as bytes, 16 per round (FIPS 197 5.2); empty for a key that
is not 16, 24 or 32 bytes.

```fides
pub fn ua_aes_round_keys(key: &Vec<i64>, tab: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `tab`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1489*

---

## ua_aes_decrypt_block

Decrypt one 16-byte block with round keys `rk` (FIPS 197 5.3).

```fides
pub fn ua_aes_decrypt_block(rk: &Vec<i64>, tab: &Vec<i64>, blk: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `rk`: `&Vec<i64>`
- `tab`: `&Vec<i64>`
- `blk`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1532*

---

## ua_aes_cbc_encrypt

AES-CBC encryption of whole blocks (no padding added here).

```fides
pub fn ua_aes_cbc_encrypt(key: &Vec<i64>, iv: &Vec<i64>, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `iv`: `&Vec<i64>`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1579*

---

## ua_aes_cbc_decrypt

```fides
pub fn ua_aes_cbc_decrypt(key: &Vec<i64>, iv: &Vec<i64>, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `iv`: `&Vec<i64>`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1602*

---

## UaRng

The channel's random generator: HMAC-SHA256(key, counter) blocks. The
seam: the caller seeds it (crypto_random_bytes(32) in real use, a
fixed seed in tests); nonces and OAEP seeds come from it.

```fides
pub struct UaRng {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `Vec<i64>` |
| `ctr` | `i64` |

*Defined at line 1627*

---

## ua_rng

```fides
pub fn ua_rng(seed: Vec<i64>) -> UaRng {
```

**Parameters:**

- `seed`: `Vec<i64>`

**Returns:** `UaRng`

*Defined at line 1632*

---

## ua_rng_bytes

```fides
pub fn ua_rng_bytes(g: &mut UaRng, n: i64) -> Vec<i64> {
```

**Parameters:**

- `g`: `&mut UaRng`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1638*

---

## UaRequestHeader

```fides
pub struct UaRequestHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `auth` | `UaNodeId` |
| `timestamp` | `i64` |
| `handle` | `i64` |
| `timeout` | `i64` |

*Defined at line 1656*

---

## ua_request_header

```fides
pub fn ua_request_header(auth: UaNodeId, timestamp: i64, handle: i64) -> UaRequestHeader {
```

**Parameters:**

- `auth`: `UaNodeId`
- `timestamp`: `i64`
- `handle`: `i64`

**Returns:** `UaRequestHeader`

*Defined at line 1663*

---

## ua_put_request_header

```fides
pub fn ua_put_request_header(out: &mut Vec<i64>, h: &UaRequestHeader) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `h`: `&UaRequestHeader`

*Defined at line 1669*

---

## ua_get_request_header

```fides
pub fn ua_get_request_header(r: &mut UaReader) -> UaRequestHeader {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaRequestHeader`

*Defined at line 1679*

---

## UaResponseHeader

```fides
pub struct UaResponseHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `timestamp` | `i64` |
| `handle` | `i64` |
| `result` | `i64` |

*Defined at line 1692*

---

## ua_put_response_header

```fides
pub fn ua_put_response_header(out: &mut Vec<i64>, h: &UaResponseHeader) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `h`: `&UaResponseHeader`

*Defined at line 1698*

---

## ua_get_response_header

```fides
pub fn ua_get_response_header(r: &mut UaReader) -> UaResponseHeader {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaResponseHeader`

*Defined at line 1707*

---

## ua_body_type

The numeric encoding id at the start of a service body (-1 if none).

```fides
pub fn ua_body_type(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 1725*

---

## ua_encode_service_fault

A ServiceFault (the response to a request a server refuses).

```fides
pub fn ua_encode_service_fault(h: &UaResponseHeader) -> Vec<i64> {
```

**Parameters:**

- `h`: `&UaResponseHeader`

**Returns:** `Vec<i64>`

*Defined at line 1745*

---

## UaOpnRequest

```fides
pub struct UaOpnRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaRequestHeader` |
| `renew` | `bool,  // RequestType Renew (else Issue)` |
| `mode` | `i64` |
| `nonce` | `Vec<i64>` |
| `lifetime` | `i64,  // requested, ms` |
| `error` | `str` |

*Defined at line 1754*

---

## ua_encode_opn_request

```fides
pub fn ua_encode_opn_request(q: &UaOpnRequest) -> Vec<i64> {
```

**Parameters:**

- `q`: `&UaOpnRequest`

**Returns:** `Vec<i64>`

*Defined at line 1763*

---

## ua_decode_opn_request

```fides
pub fn ua_decode_opn_request(b: Vec<i64>) -> UaOpnRequest {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaOpnRequest`

*Defined at line 1779*

---

## UaOpnResponse

```fides
pub struct UaOpnResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaResponseHeader` |
| `channel_id` | `i64` |
| `token_id` | `i64` |
| `created_at` | `i64` |
| `lifetime` | `i64,  // revised, ms` |
| `nonce` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 1798*

---

## ua_encode_opn_response

```fides
pub fn ua_encode_opn_response(p: &UaOpnResponse) -> Vec<i64> {
```

**Parameters:**

- `p`: `&UaOpnResponse`

**Returns:** `Vec<i64>`

*Defined at line 1808*

---

## ua_decode_opn_response

```fides
pub fn ua_decode_opn_response(b: Vec<i64>) -> UaOpnResponse {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaOpnResponse`

*Defined at line 1821*

---

## ua_encode_clo_request

The body of a CloseSecureChannelRequest (sent as a CLO message).

```fides
pub fn ua_encode_clo_request(h: &UaRequestHeader) -> Vec<i64> {
```

**Parameters:**

- `h`: `&UaRequestHeader`

**Returns:** `Vec<i64>`

*Defined at line 1844*

---

## UaChannel

One end of a secure channel. Tokens: `token_id` with send_keys /
recv_keys is current; `old_token` (receive only) is the one a renewal
replaced; a server keeps a renewed token in `next_*` until the client
first uses it (Part 4 5.5.2: the server switches then).

```fides
pub struct UaChannel {
```

**Fields:**

| Name | Type |
|------|------|
| `is_client` | `bool` |
| `policy` | `str` |
| `mode` | `i64` |
| `own_cert` | `Vec<i64>` |
| `own_key` | `RsaPrivateKey` |
| `peer_cert` | `Vec<i64>` |
| `peer_key` | `RsaPublicKey` |
| `rng` | `UaRng` |
| `channel_id` | `i64` |
| `token_id` | `i64` |
| `send_keys` | `UaKeys` |
| `recv_keys` | `UaKeys` |
| `old_token` | `i64` |
| `old_recv` | `UaKeys` |
| `next_token` | `i64` |
| `next_send` | `UaKeys` |
| `next_recv` | `UaKeys` |
| `local_nonce` | `Vec<i64>` |
| `remote_nonce` | `Vec<i64>` |
| `send_seq` | `i64` |
| `recv_seq` | `i64,  // -1 before the first message` |
| `lifetime` | `i64` |
| `created` | `i64,  // caller's clock (ms) when the token was issued` |

*Defined at line 1857*

---

## ua_channel

A channel end. `own_key` / certificates may be empty for
SecurityPolicy None; a server learns the client certificate from the
first OpenSecureChannel request (and should check it against its trust
list: the caller's decision, after ua_opn_open).

```fides
pub fn ua_channel(is_client: bool, policy: str, mode: i64, own_cert: Vec<i64>, own_key: RsaPrivateKey, peer_cert: Vec<i64>, rng_seed: Vec<i64>) -> UaChannel {
```

**Parameters:**

- `is_client`: `bool`
- `policy`: `str`
- `mode`: `i64`
- `own_cert`: `Vec<i64>`
- `own_key`: `RsaPrivateKey`
- `peer_cert`: `Vec<i64>`
- `rng_seed`: `Vec<i64>`

**Returns:** `UaChannel`

*Defined at line 1887*

---

## ua_cert_public_key

The RSA public key of a DER certificate.

```fides
pub fn ua_cert_public_key(der: Vec<i64>) -> RsaPublicKey {
```

**Parameters:**

- `der`: `Vec<i64>`

**Returns:** `RsaPublicKey`

*Defined at line 1898*

---

## ua_channel_set_peer

```fides
pub fn ua_channel_set_peer(c: &mut UaChannel, cert: Vec<i64>) {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `cert`: `Vec<i64>`

*Defined at line 1906*

---

## ua_thumbprint

SHA-1 of a DER certificate (the thumbprint OPC UA puts in headers).

```fides
pub fn ua_thumbprint(cert: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `cert`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1912*

---

## ua_channel_nonce

A new nonce for an OpenSecureChannel request or response (32 bytes;
empty for SecurityPolicy None).

```fides
pub fn ua_channel_nonce(c: &mut UaChannel) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut UaChannel`

**Returns:** `Vec<i64>`

*Defined at line 1922*

---

## ua_channel_install

Client: install the token of an OpenSecureChannel response (the old
one stays valid for receiving).

```fides
pub fn ua_channel_install(c: &mut UaChannel, p: &UaOpnResponse, now: i64) {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `p`: `&UaOpnResponse`
- `now`: `i64`

*Defined at line 1943*

---

## ua_channel_issue

Server: issue (renew false) or renew a token from the client's nonce:
returns the response to send. A first token is current at once; a
renewed one waits in next_* until the client uses it.

```fides
pub fn ua_channel_issue(c: &mut UaChannel, q: &UaOpnRequest, channel_id: i64, now: i64, lifetime: i64) -> UaOpnResponse {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `q`: `&UaOpnRequest`
- `channel_id`: `i64`
- `now`: `i64`
- `lifetime`: `i64`

**Returns:** `UaOpnResponse`

*Defined at line 1962*

---

## UaOpened

A message opened from the wire.

```fides
pub struct UaOpened {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `mtype` | `str,  // "OPN", "MSG", "CLO"` |
| `channel_id` | `i64` |
| `token_id` | `i64` |
| `policy` | `str,  // OPN: the sender's SecurityPolicyUri` |
| `sender_cert` | `Vec<i64>` |
| `seq` | `i64` |
| `request_id` | `i64` |
| `body` | `Vec<i64>` |

*Defined at line 1993*

---

## ua_message_type

The 3-letter type of a message.

```fides
pub fn ua_message_type(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 2013*

---

## ua_opn_seal

An OPN message with asymmetric security (Part 6 6.7.2): the
asymmetric security header (policy URI, the sender's certificate, the
receiver's thumbprint), then sequence header, body, padding and an
RSA-PKCS1-v1.5-SHA256 signature, RSA-OAEP-SHA1 encrypted for the
receiver in blocks of k - 42 bytes. SecurityPolicy None: in clear.
Empty if the keys are missing or larger than 2048 bits.

```fides
pub fn ua_opn_seal(c: &mut UaChannel, request_id: i64, body: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `request_id`: `i64`
- `body`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2054*

---

## ua_opn_open

Open an OPN message: check the policy and the thumbprint of our
certificate, decrypt, verify the sender's signature (a server takes the
sender certificate as its peer on the first request), check padding
and sequence number.

```fides
pub fn ua_opn_open(c: &mut UaChannel, msg: Vec<i64>) -> UaOpened {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `msg`: `Vec<i64>`

**Returns:** `UaOpened`

*Defined at line 2114*

---

## ua_sym_seal

The chunks of a MSG or CLO message with symmetric security (Part 6
6.7.2): HMAC-SHA256 signature with the current send keys, and in mode
SignAndEncrypt padding and AES-256-CBC. Each chunk is at most
`max_chunk` bytes. Mode None / policy None: plain chunks.

```fides
pub fn ua_sym_seal(c: &mut UaChannel, mtype: str, request_id: i64, body: &Vec<i64>, max_chunk: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `mtype`: `str`
- `request_id`: `i64`
- `body`: `&Vec<i64>`
- `max_chunk`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2198*

---

## ua_sym_open

Open the chunks of one MSG/CLO message: token (current, previous, or
on a server the renewed one, which then becomes current), decryption,
signature, padding, sequence numbers, one request id, a final chunk.

```fides
pub fn ua_sym_open(c: &mut UaChannel, chunks: &Vec<Vec<i64>>) -> UaOpened {
```

**Parameters:**

- `c`: `&mut UaChannel`
- `chunks`: `&Vec<Vec<i64>>`

**Returns:** `UaOpened`

*Defined at line 2263*

---

## ua_get_localized_text

```fides
pub fn ua_get_localized_text(r: &mut UaReader) -> str {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `str`

*Defined at line 2367*

---

## UaCreateSessionRequest

```fides
pub struct UaCreateSessionRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaRequestHeader` |
| `app_uri` | `str` |
| `app_name` | `str` |
| `endpoint_url` | `str` |
| `session_name` | `str` |
| `nonce` | `Vec<i64>` |
| `cert` | `Vec<i64>` |
| `timeout` | `f64,  // requested session timeout, ms` |
| `max_response` | `i64` |
| `error` | `str` |

*Defined at line 2417*

---

## ua_encode_create_session_request

```fides
pub fn ua_encode_create_session_request(q: &UaCreateSessionRequest) -> Vec<i64> {
```

**Parameters:**

- `q`: `&UaCreateSessionRequest`

**Returns:** `Vec<i64>`

*Defined at line 2430*

---

## ua_decode_create_session_request

```fides
pub fn ua_decode_create_session_request(b: Vec<i64>) -> UaCreateSessionRequest {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaCreateSessionRequest`

*Defined at line 2445*

---

## UaEndpoint

An EndpointDescription with its user token policies (parallel lists).

```fides
pub struct UaEndpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `url` | `str` |
| `server_uri` | `str` |
| `cert` | `Vec<i64>` |
| `mode` | `i64` |
| `policy` | `str` |
| `token_ids` | `Vec<str>` |
| `token_types` | `Vec<i64>` |
| `token_policies` | `Vec<str>,  // SecurityPolicyUri of the token ("" = the channel's)` |
| `level` | `i64` |

*Defined at line 2468*

---

## ua_put_endpoint

```fides
pub fn ua_put_endpoint(out: &mut Vec<i64>, e: &UaEndpoint) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `e`: `&UaEndpoint`

*Defined at line 2480*

---

## ua_get_endpoint

```fides
pub fn ua_get_endpoint(r: &mut UaReader) -> UaEndpoint {
```

**Parameters:**

- `r`: `&mut UaReader`

**Returns:** `UaEndpoint`

*Defined at line 2504*

---

## UaCreateSessionResponse

```fides
pub struct UaCreateSessionResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaResponseHeader` |
| `session_id` | `UaNodeId` |
| `auth_token` | `UaNodeId` |
| `timeout` | `f64` |
| `nonce` | `Vec<i64>` |
| `cert` | `Vec<i64>` |
| `endpoints` | `Vec<UaEndpoint>` |
| `sig_alg` | `str` |
| `signature` | `Vec<i64>,  // server's signature of client certificate + client nonce` |
| `max_request` | `i64` |
| `error` | `str` |

*Defined at line 2528*

---

## ua_encode_create_session_response

```fides
pub fn ua_encode_create_session_response(p: &UaCreateSessionResponse) -> Vec<i64> {
```

**Parameters:**

- `p`: `&UaCreateSessionResponse`

**Returns:** `Vec<i64>`

*Defined at line 2542*

---

## ua_decode_create_session_response

```fides
pub fn ua_decode_create_session_response(b: Vec<i64>) -> UaCreateSessionResponse {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaCreateSessionResponse`

*Defined at line 2563*

---

## UaActivateSessionRequest

```fides
pub struct UaActivateSessionRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaRequestHeader` |
| `sig_alg` | `str` |
| `signature` | `Vec<i64>,  // client's signature of server certificate + server nonce` |
| `locales` | `Vec<str>` |
| `token_type` | `i64,  // UA_ANONYMOUS_TOKEN_ID or UA_USERNAME_TOKEN_ID` |
| `policy_id` | `str` |
| `user` | `str` |
| `password` | `Vec<i64>,  // as on the wire (encrypted unless policy None)` |
| `enc_alg` | `str` |
| `error` | `str` |

*Defined at line 2606*

---

## ua_encode_activate_session_request

```fides
pub fn ua_encode_activate_session_request(q: &UaActivateSessionRequest) -> Vec<i64> {
```

**Parameters:**

- `q`: `&UaActivateSessionRequest`

**Returns:** `Vec<i64>`

*Defined at line 2619*

---

## ua_decode_activate_session_request

```fides
pub fn ua_decode_activate_session_request(b: Vec<i64>) -> UaActivateSessionRequest {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaActivateSessionRequest`

*Defined at line 2650*

---

## UaActivateSessionResponse

```fides
pub struct UaActivateSessionResponse {
```

**Fields:**

| Name | Type |
|------|------|
| `hdr` | `UaResponseHeader` |
| `nonce` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 2703*

---

## ua_encode_activate_session_response

```fides
pub fn ua_encode_activate_session_response(p: &UaActivateSessionResponse) -> Vec<i64> {
```

**Parameters:**

- `p`: `&UaActivateSessionResponse`

**Returns:** `Vec<i64>`

*Defined at line 2709*

---

## ua_decode_activate_session_response

```fides
pub fn ua_decode_activate_session_response(b: Vec<i64>) -> UaActivateSessionResponse {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaActivateSessionResponse`

*Defined at line 2719*

---

## ua_encode_close_session_request

```fides
pub fn ua_encode_close_session_request(h: &UaRequestHeader, delete_subscriptions: bool) -> Vec<i64> {
```

**Parameters:**

- `h`: `&UaRequestHeader`
- `delete_subscriptions`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 2754*

---

## ua_decode_close_session_request

The request header of a CloseSessionRequest (error set if not one).

```fides
pub fn ua_decode_close_session_request(b: Vec<i64>) -> UaRequestHeader {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `UaRequestHeader`

*Defined at line 2763*

---

## ua_encode_close_session_response

```fides
pub fn ua_encode_close_session_response(h: &UaResponseHeader) -> Vec<i64> {
```

**Parameters:**

- `h`: `&UaResponseHeader`

**Returns:** `Vec<i64>`

*Defined at line 2776*

---

## ua_decode_response_header

The response header of a CloseSessionResponse or ServiceFault.

```fides
pub fn ua_decode_response_header(b: Vec<i64>, want: i64) -> UaResponseHeader {
```

**Parameters:**

- `b`: `Vec<i64>`
- `want`: `i64`

**Returns:** `UaResponseHeader`

*Defined at line 2784*

---

## ua_session_sign

The session signature of Basic256Sha256: RSA-PKCS1-v1.5-SHA256 of the
peer's certificate followed by the peer's nonce.

```fides
pub fn ua_session_sign(key: RsaPrivateKey, peer_cert: &Vec<i64>, peer_nonce: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `RsaPrivateKey`
- `peer_cert`: `&Vec<i64>`
- `peer_nonce`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2797*

---

## ua_session_verify

```fides
pub fn ua_session_verify(key: RsaPublicKey, own_cert: &Vec<i64>, own_nonce: &Vec<i64>, sig: &Vec<i64>) -> bool {
```

**Parameters:**

- `key`: `RsaPublicKey`
- `own_cert`: `&Vec<i64>`
- `own_nonce`: `&Vec<i64>`
- `sig`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 2801*

---

## ua_password_encrypt

A UserNameIdentityToken secret (Part 4 7.36.2.2): RSA-OAEP-SHA1 of
length (UInt32) + password + server nonce under the server's key.

```fides
pub fn ua_password_encrypt(key: RsaPublicKey, password: str, server_nonce: &Vec<i64>, seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `RsaPublicKey`
- `password`: `str`
- `server_nonce`: `&Vec<i64>`
- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2807*

---

## ua_password_decrypt

The password from a token secret, or ok false if it does not decrypt
or does not end with the server nonce the session sent.

```fides
pub fn ua_password_decrypt(key: RsaPrivateKey, secret: &Vec<i64>, server_nonce: &Vec<i64>) -> RsaPlain {
```

**Parameters:**

- `key`: `RsaPrivateKey`
- `secret`: `&Vec<i64>`
- `server_nonce`: `&Vec<i64>`

**Returns:** `RsaPlain`

*Defined at line 2818*

---

## UA_CL_IDLE:

```fides
pub const UA_CL_IDLE: i64 = 0;
```

*Defined at line 2842*

---

## UA_CL_HELLO:

```fides
pub const UA_CL_HELLO: i64 = 1;  // HEL sent
```

*Defined at line 2843*

---

## UA_CL_OPENING:

```fides
pub const UA_CL_OPENING: i64 = 2;  // OPN sent
```

*Defined at line 2844*

---

## UA_CL_CREATING:

```fides
pub const UA_CL_CREATING: i64 = 3;  // CreateSession sent
```

*Defined at line 2845*

---

## UA_CL_ACTIVATING:

```fides
pub const UA_CL_ACTIVATING: i64 = 4;  // ActivateSession sent
```

*Defined at line 2846*

---

## UA_CL_ACTIVE:

```fides
pub const UA_CL_ACTIVE: i64 = 5;  // session active
```

*Defined at line 2847*

---

## UA_CL_CLOSING:

```fides
pub const UA_CL_CLOSING: i64 = 6;  // CloseSession sent
```

*Defined at line 2848*

---

## UA_CL_CLOSED:

```fides
pub const UA_CL_CLOSED: i64 = 7;  // CLO sent
```

*Defined at line 2849*

---

## UA_CL_FAILED:

```fides
pub const UA_CL_FAILED: i64 = 8;
```

*Defined at line 2850*

---

## UaClient

A client: the connection state machine over a byte-stream transport.
The seam: the caller owns the TCP connection; ua_client_start and the
requests return frames to write, ua_client_feed takes the bytes read
(any split) and returns frames to write in reply.

```fides
pub struct UaClient {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `error` | `str` |
| `ch` | `UaChannel` |
| `url` | `str` |
| `app_uri` | `str` |
| `user` | `str,  // "" for an anonymous identity` |
| `password` | `str` |
| `session_id` | `UaNodeId` |
| `auth_token` | `UaNodeId` |
| `client_nonce` | `Vec<i64>` |
| `server_nonce` | `Vec<i64>` |
| `server_cert` | `Vec<i64>` |
| `handle` | `i64` |
| `request_id` | `i64` |
| `now` | `i64,  // caller's clock, ms since 1970` |
| `renewing` | `bool` |
| `renewals` | `i64` |
| `inbox` | `Vec<i64>,  // bytes not yet framed` |
| `parts` | `Vec<Vec<i64>>,  // chunks of a message in progress` |
| `reads` | `Vec<UaReadResponse>` |
| `max_chunk` | `i64` |

*Defined at line 2856*

---

## ua_client

```fides
pub fn ua_client(url: str, app_uri: str, ch: UaChannel, user: str, password: str) -> UaClient {
```

**Parameters:**

- `url`: `str`
- `app_uri`: `str`
- `ch`: `UaChannel`
- `user`: `str`
- `password`: `str`

**Returns:** `UaClient`

*Defined at line 2880*

---

## ua_client_start

Connect: the Hello frame.

```fides
pub fn ua_client_start(c: &mut UaClient) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaClient`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2905*

---

## ua_client_renew

Renew the channel's SecurityToken (send when ua_client_needs_renew).

```fides
pub fn ua_client_renew(c: &mut UaClient) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaClient`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2935*

---

## ua_client_needs_renew

Whether 75 % of the token's lifetime has passed (Part 4 5.5.2).

```fides
pub fn ua_client_needs_renew(c: &UaClient) -> bool {
```

**Parameters:**

- `c`: `&UaClient`

**Returns:** `bool`

*Defined at line 2944*

---

## ua_client_read

A Read of the Value attribute of `nodes`; the response is appended to
c.reads when it arrives.

```fides
pub fn ua_client_read(c: &mut UaClient, nodes: &Vec<UaNodeId>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaClient`
- `nodes`: `&Vec<UaNodeId>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2950*

---

## ua_client_close

Close the session (then the channel, when the response arrives).

```fides
pub fn ua_client_close(c: &mut UaClient) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaClient`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2966*

---

## ua_take_frame

Take the first whole frame from a byte stream (empty if incomplete).

```fides
pub fn ua_take_frame(buf: &mut Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `buf`: `&mut Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2976*

---

## ua_client_feed

Feed bytes read from the connection; returns the frames to write.

```fides
pub fn ua_client_feed(c: &mut UaClient, bytes: &Vec<i64>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut UaClient`
- `bytes`: `&Vec<i64>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2988*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
