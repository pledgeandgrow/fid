[All modules](index.md)

# webview -- Fides Documentation

## Contents

- [RPC_PARSE_ERROR:](#RPC_PARSE_ERROR:) (const)
- [RPC_INVALID_REQUEST:](#RPC_INVALID_REQUEST:) (const)
- [RPC_METHOD_NOT_FOUND:](#RPC_METHOD_NOT_FOUND:) (const)
- [RPC_INVALID_PARAMS:](#RPC_INVALID_PARAMS:) (const)
- [RPC_INTERNAL_ERROR:](#RPC_INTERNAL_ERROR:) (const)
- [RPC_ORIGIN_REFUSED:](#RPC_ORIGIN_REFUSED:) (const)
- [webview_origin](#webview_origin) (function)
- [webview_origin_allowed](#webview_origin_allowed) (function)
- [webview_error_result](#webview_error_result) (function)
- [webview_response](#webview_response) (function)
- [webview_error_response](#webview_error_response) (function)
- [webview_request](#webview_request) (function)
- [webview_notification](#webview_notification) (function)
- [WebviewPending](#WebviewPending) (struct)
- [WebviewHandler](#WebviewHandler) (struct)
- [WebviewBridge](#WebviewBridge) (struct)
- [webview_bridge_new](#webview_bridge_new) (function)
- [webview_register](#webview_register) (function)
- [webview_receive](#webview_receive) (function)
- [webview_call_js](#webview_call_js) (function)
- [webview_emit](#webview_emit) (function)
- [webview_expire](#webview_expire) (function)
- [webview_js_string](#webview_js_string) (function)
- [webview_js_eval](#webview_js_eval) (function)
- [WEBVIEW_ANDROID:](#WEBVIEW_ANDROID:) (const)
- [WEBVIEW_IOS:](#WEBVIEW_IOS:) (const)
- [webview_bridge_script](#webview_bridge_script) (function)
- [WebviewCall](#WebviewCall) (struct)
- [webview_android_calls](#webview_android_calls) (function)
- [webview_ios_calls](#webview_ios_calls) (function)

## RPC_PARSE_ERROR:

```fides
pub const RPC_PARSE_ERROR: i64 = -32700;
```

*Defined at line 32*

---

## RPC_INVALID_REQUEST:

```fides
pub const RPC_INVALID_REQUEST: i64 = -32600;
```

*Defined at line 33*

---

## RPC_METHOD_NOT_FOUND:

```fides
pub const RPC_METHOD_NOT_FOUND: i64 = -32601;
```

*Defined at line 34*

---

## RPC_INVALID_PARAMS:

```fides
pub const RPC_INVALID_PARAMS: i64 = -32602;
```

*Defined at line 35*

---

## RPC_INTERNAL_ERROR:

```fides
pub const RPC_INTERNAL_ERROR: i64 = -32603;
```

*Defined at line 36*

---

## RPC_ORIGIN_REFUSED:

```fides
pub const RPC_ORIGIN_REFUSED: i64 = -32000;
```

*Defined at line 37*

---

## webview_origin

The origin of a URL ("scheme://host[:port]", default port dropped,
lower case); "null" for opaque ones (data:, about:, blob: without an
inner origin).

```fides
pub fn webview_origin(url: str) -> str {
```

**Parameters:**

- `url`: `str`

**Returns:** `str`

*Defined at line 44*

---

## webview_origin_allowed

Is `origin` allowed by the list? Entries are origins
("https://example.com", "app://local") or wildcards
("https://*.example.com" matches subdomains, not the bare domain).

```fides
pub fn webview_origin_allowed(allow: &Vec<str>, origin: str) -> bool {
```

**Parameters:**

- `allow`: `&Vec<str>`
- `origin`: `str`

**Returns:** `bool`

*Defined at line 85*

---

## webview_error_result

An error result for a handler to return: dispatch turns it into a
JSON-RPC error response.

```fides
pub fn webview_error_result(code: i64, message: str) -> str {
```

**Parameters:**

- `code`: `i64`
- `message`: `str`

**Returns:** `str`

*Defined at line 114*

---

## webview_response

```fides
pub fn webview_response(id: str, result: str) -> str {
```

**Parameters:**

- `id`: `str`
- `result`: `str`

**Returns:** `str`

*Defined at line 118*

---

## webview_error_response

```fides
pub fn webview_error_response(id: str, code: i64, message: str) -> str {
```

**Parameters:**

- `id`: `str`
- `code`: `i64`
- `message`: `str`

**Returns:** `str`

*Defined at line 122*

---

## webview_request

```fides
pub fn webview_request(id: i64, method: str, params: str) -> str {
```

**Parameters:**

- `id`: `i64`
- `method`: `str`
- `params`: `str`

**Returns:** `str`

*Defined at line 126*

---

## webview_notification

```fides
pub fn webview_notification(method: str, params: str) -> str {
```

**Parameters:**

- `method`: `str`
- `params`: `str`

**Returns:** `str`

*Defined at line 130*

---

## WebviewPending

```fides
pub struct WebviewPending {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `method` | `str` |
| `sent_ms` | `i64` |

*Defined at line 136*

---

## WebviewHandler

A registered handler (a struct: a Vec of bare function values is not
callable in the interpreter).

```fides
pub struct WebviewHandler {
```

**Fields:**

| Name | Type |
|------|------|
| `f` | `fn(str) -> str` |

*Defined at line 144*

---

## WebviewBridge

```fides
pub struct WebviewBridge {
```

**Fields:**

| Name | Type |
|------|------|
| `allow` | `Vec<str>` |
| `methods` | `Vec<str>` |
| `handlers` | `Vec<WebviewHandler>` |
| `next_id` | `i64` |
| `pending` | `Vec<WebviewPending>` |
| `resolved_ids` | `Vec<i64>,  // calls into JS answered, in order` |
| `resolved_vals` | `Vec<str>,  // their result (or error) JSON` |
| `resolved_ok` | `Vec<bool>` |
| `refused` | `i64,  // messages from origins not allowed` |

*Defined at line 148*

---

## webview_bridge_new

```fides
pub fn webview_bridge_new(allow: Vec<str>) -> WebviewBridge {
```

**Parameters:**

- `allow`: `Vec<str>`

**Returns:** `WebviewBridge`

*Defined at line 160*

---

## webview_register

Register a Fides method JavaScript may call: the handler gets the
params JSON ("null" when absent) and returns the result JSON, or a
webview_error_result.

```fides
pub fn webview_register(b: &mut WebviewBridge, method: str, handler: fn(str) -> str) {
```

**Parameters:**

- `b`: `&mut WebviewBridge`
- `method`: `str`
- `handler`: `fn(str) -> str`

*Defined at line 169*

---

## webview_receive

Take a message JavaScript posted from `origin`; returns the text to
send back ("" when there is nothing to send).

```fides
pub fn webview_receive(b: &mut WebviewBridge, origin: str, text: str) -> str {
```

**Parameters:**

- `b`: `&mut WebviewBridge`
- `origin`: `str`
- `text`: `str`

**Returns:** `str`

*Defined at line 253*

---

## webview_call_js

Call a JavaScript method registered with FidesBridge.on: returns the
request text (deliver it with webview_js_eval); the answer arrives
through webview_receive and lands in resolved_*.

```fides
pub fn webview_call_js(b: &mut WebviewBridge, method: str, params: str, now_ms: i64) -> str {
```

**Parameters:**

- `b`: `&mut WebviewBridge`
- `method`: `str`
- `params`: `str`
- `now_ms`: `i64`

**Returns:** `str`

*Defined at line 289*

---

## webview_emit

An event for JavaScript listeners (no answer expected).

```fides
pub fn webview_emit(event: str, params: str) -> str {
```

**Parameters:**

- `event`: `str`
- `params`: `str`

**Returns:** `str`

*Defined at line 299*

---

## webview_expire

Drop calls into JavaScript older than `timeout_ms`; their ids.

```fides
pub fn webview_expire(b: &mut WebviewBridge, now_ms: i64, timeout_ms: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `&mut WebviewBridge`
- `now_ms`: `i64`
- `timeout_ms`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 328*

---

## webview_js_string

A JavaScript string literal (double quotes) safe inside a script and
an HTML <script> element: \, ", control characters, U+2028, U+2029 and
"</" are escaped.

```fides
pub fn webview_js_string(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 350*

---

## webview_js_eval

The text to evaluate in the page to deliver `message` to the bridge.

```fides
pub fn webview_js_eval(message: str) -> str {
```

**Parameters:**

- `message`: `str`

**Returns:** `str`

*Defined at line 397*

---

## WEBVIEW_ANDROID:

```fides
pub const WEBVIEW_ANDROID: i64 = 0;
```

*Defined at line 401*

---

## WEBVIEW_IOS:

```fides
pub const WEBVIEW_IOS: i64 = 1;
```

*Defined at line 402*

---

## webview_bridge_script

The script injected at document start: window.FidesBridge.
`channel` is the interface name (Android addJavascriptInterface) or the
script message handler name (iOS).

```fides
pub fn webview_bridge_script(platform: i64, channel: str) -> str {
```

**Parameters:**

- `platform`: `i64`
- `channel`: `str`

**Returns:** `str`

*Defined at line 407*

---

## WebviewCall

```fides
pub struct WebviewCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |

*Defined at line 436*

---

## webview_android_calls

```fides
pub fn webview_android_calls() -> Vec<WebviewCall> {
```

**Returns:** `Vec<WebviewCall>`

*Defined at line 442*

---

## webview_ios_calls

```fides
pub fn webview_ios_calls() -> Vec<WebviewCall> {
```

**Returns:** `Vec<WebviewCall>`

*Defined at line 465*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
