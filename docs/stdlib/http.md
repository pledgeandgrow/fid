# http — Fides Documentation

## Contents

- [HttpRequest](#httprequest) (struct)
- [http_header](#http_header) (function)
- [http_keep_alive](#http_keep_alive) (function)
- [http_parse_head](#http_parse_head) (function)
- [parse_request](#parse_request) (function)
- [build_response](#build_response) (function)
- [build_response_headers](#build_response_headers) (function)
- [status_text](#status_text) (function)
- [http_set_tls](#http_set_tls) (function)
- [http_conn_new](#http_conn_new) (function)
- [http_conn_close](#http_conn_close) (function)
- [http_read_message](#http_read_message) (function)
- [http_conn_write](#http_conn_write) (function)
- [http_conn_task](#http_conn_task) (function)
- [http_accept_task](#http_accept_task) (function)
- [http_serve](#http_serve) (function)
- [http_serve_scoped](#http_serve_scoped) (function)
- [http_set_scoped](#http_set_scoped) (function)
- [http_listen_tasks](#http_listen_tasks) (function)
- [http_serve_on](#http_serve_on) (function)
- [handle_request](#handle_request) (function)
- [serve](#serve) (function)
- [http_request_arena_new](#http_request_arena_new) (function)
- [http_request_alloc](#http_request_alloc) (function)
- [http_request_arena_free](#http_request_arena_free) (function)
- [http_parse_request_arena](#http_parse_request_arena) (function)
- [http_build_response_arena](#http_build_response_arena) (function)
- [router_new](#router_new) (function)
- [router_add](#router_add) (function)
- [router_dispatch](#router_dispatch) (function)
- [router_dispatch_request](#router_dispatch_request) (function)
- [apply_middleware](#apply_middleware) (function)
- [logging_middleware](#logging_middleware) (function)
- [cors_middleware](#cors_middleware) (function)

## HttpRequest

A message as it arrived. For a response, `method` is the protocol word and
`target` the status code.

```fides
pub struct HttpRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `str` |
| `target` | `str` |
| `path` | `str` |
| `query` | `str` |
| `version` | `str` |
| `names` | `Vec<str>` |
| `vals` | `Vec<str>` |
| `body` | `str` |

*Defined at line 42*

---

## http_header

The value of header `name`, any case, or "".

```fides
pub fn http_header(req: HttpRequest, name: str) -> str {
```

**Parameters:**

- `req`: `HttpRequest`
- `name`: `str`

**Returns:** `str`

*Defined at line 70*

---

## http_keep_alive

Should the connection stay open after this request?

```fides
pub fn http_keep_alive(req: HttpRequest) -> bool {
```

**Parameters:**

- `req`: `HttpRequest`

**Returns:** `bool`

*Defined at line 82*

---

## http_parse_head

Parse a head: the first line and the headers.

```fides
pub fn http_parse_head(head: str) -> HttpRequest {
```

**Parameters:**

- `head`: `str`

**Returns:** `HttpRequest`

*Defined at line 118*

---

## parse_request

Parse raw request text into a JSON string with method, path, version,
headers, and body fields.

```fides
pub fn parse_request(raw: str) -> str {
```

**Parameters:**

- `raw`: `str`

**Returns:** `str`

*Defined at line 194*

---

## build_response

Build an HTTP response from status, content-type, and body.

```fides
pub fn build_response(status: i64, content_type: str, body: str) -> str {
```

**Parameters:**

- `status`: `i64`
- `content_type`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 215*

---

## build_response_headers

A response with extra headers, each "Name: value\r\n".

```fides
pub fn build_response_headers(status: i64, content_type: str, headers: str, body: str) -> str {
```

**Parameters:**

- `status`: `i64`
- `content_type`: `str`
- `headers`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 220*

---

## status_text

HTTP status codes as strings.

```fides
pub fn status_text(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 227*

---

## http_set_tls

For module `https`: how to read, write, close and accept a TLS session.

```fides
pub fn http_set_tls(recv: fn(i64, i64, i64, i64) -> i64, send: fn(i64, i64, i64) -> i64, close: fn(i64) -> i64,
```

**Parameters:**

- `recv`: `fn(i64`
- `send`: `fn(i64`
- `close`: `fn(i64) -> i64`

*Defined at line 297*

---

## http_conn_new

A connection on a connected socket, with a TLS session or 0.

```fides
pub fn http_conn_new(fd: i64, tls: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `tls`: `i64`

**Returns:** `i64`

*Defined at line 306*

---

## http_conn_close

```fides
pub fn http_conn_close(c: i64) {
```

**Parameters:**

- `c`: `i64`

*Defined at line 317*

---

## http_read_message

Read a head and its body off the connection. `method` is "" at the end of the
input, on a timeout, or when the message is too large. A response with no
length reads to the end when `to_end` is set.

```fides
pub fn http_read_message(c: i64, timeout_ms: i64, to_end: bool) -> HttpRequest {
```

**Parameters:**

- `c`: `i64`
- `timeout_ms`: `i64`
- `to_end`: `bool`

**Returns:** `HttpRequest`

*Defined at line 482*

---

## http_conn_write

Write the whole string. Returns false if the connection failed.

```fides
pub fn http_conn_write(c: i64, data: str) -> bool {
```

**Parameters:**

- `c`: `i64`
- `data`: `str`

**Returns:** `bool`

*Defined at line 513*

---

## http_conn_task

Serve one connection until it closes. The TLS handshake is here, so a client
that stalls holds up only its own task.

```fides
pub fn http_conn_task(c: i64) -> i64 {
```

**Parameters:**

- `c`: `i64`

**Returns:** `i64`

*Defined at line 534*

---

## http_accept_task

Take connections off the listening socket, each its own task.

```fides
pub fn http_accept_task(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 568*

---

## http_serve

Serve HTTP on `port` until the process ends. Returns false if the port
cannot be opened.

Nothing can dangle here: what a request and its response hold stays allocated,
since `handler` may have kept it. A long-running server wants
http_serve_scoped.

```fides
pub fn http_serve(port: i64, handler: fn(HttpRequest) -> str) -> bool {
```

**Parameters:**

- `port`: `i64`
- `handler`: `fn(HttpRequest) -> str`

**Returns:** `bool`

*Defined at line 584*

---

## http_serve_scoped

```fides
pub fn http_serve_scoped(port: i64, handler: fn(HttpRequest) -> str) -> bool {
```

**Parameters:**

- `port`: `i64`
- `handler`: `fn(HttpRequest) -> str`

**Returns:** `bool`

*Defined at line 598*

---

## http_set_scoped

For module `https`.

```fides
pub fn http_set_scoped() {
```

*Defined at line 604*

---

## http_listen_tasks

Serve a listening socket from tasks and return: the caller goes on, and they
run whenever it waits.

```fides
pub fn http_listen_tasks(fd: i64, handler: fn(HttpRequest) -> str) {
```

**Parameters:**

- `fd`: `i64`
- `handler`: `fn(HttpRequest) -> str`

*Defined at line 610*

---

## http_serve_on

Serve a listening socket until the process ends, over TLS if `secure` (module
`https` has set it up).

```fides
pub fn http_serve_on(fd: i64, secure: bool, handler: fn(HttpRequest) -> str) {
```

**Parameters:**

- `fd`: `i64`
- `secure`: `bool`
- `handler`: `fn(HttpRequest) -> str`

*Defined at line 617*

---

## handle_request

Default handler: echoes the request path.

```fides
pub fn handle_request(req: HttpRequest) -> str {
```

**Parameters:**

- `req`: `HttpRequest`

**Returns:** `str`

*Defined at line 625*

---

## serve

Serve `handle_request` on `port`.

```fides
pub fn serve(port: i64) -> void {
```

**Parameters:**

- `port`: `i64`

**Returns:** `void`

*Defined at line 630*

---

## http_request_arena_new

Create a new HTTP request arena. Returns a mark for later restore.
All allocations made via http_request_alloc will be freed when
http_request_arena_free is called.

```fides
pub fn http_request_arena_new() -> http_request_arena {
```

**Returns:** `http_request_arena`

*Defined at line 664*

---

## http_request_alloc

Allocate `size` bytes within an HTTP request arena.
The memory is freed when http_request_arena_free is called.

```fides
pub fn http_request_alloc(arena: http_request_arena, size: i64) -> i64 {
```

**Parameters:**

- `arena`: `http_request_arena`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 670*

---

## http_request_arena_free

Free an HTTP request arena. All allocations made via http_request_alloc
are freed at once. Call this after the response has been sent.

```fides
pub fn http_request_arena_free(arena: http_request_arena) -> void {
```

**Parameters:**

- `arena`: `http_request_arena`

**Returns:** `void`

*Defined at line 676*

---

## http_parse_request_arena

Parse an HTTP request using a request arena. The parsed request
and all intermediate allocations go into the arena.
Returns a JSON string with method, path, headers, and body.

```fides
pub fn http_parse_request_arena(arena: http_request_arena, raw: str) -> str {
```

**Parameters:**

- `arena`: `http_request_arena`
- `raw`: `str`

**Returns:** `str`

*Defined at line 683*

---

## http_build_response_arena

Build an HTTP response using a request arena. The response string
and any intermediate allocations go into the arena.

```fides
pub fn http_build_response_arena(arena: http_request_arena, status: i64, content_type: str, body: str) -> str {
```

**Parameters:**

- `arena`: `http_request_arena`
- `status`: `i64`
- `content_type`: `str`
- `body`: `str`

**Returns:** `str`

*Defined at line 693*

---

## router_new

Create a new router.

```fides
pub fn router_new() -> Router {
```

**Returns:** `Router`

*Defined at line 720*

---

## router_add

Add a route to the router.
method: "GET", "POST", "PUT", "DELETE", etc.
path: path pattern (e.g. "/", "/users", "/users/:id")
handler: function to call when the route matches

```fides
pub fn router_add(router: Router, method: str, path: str, handler: route_handler) -> void {
```

**Parameters:**

- `router`: `Router`
- `method`: `str`
- `path`: `str`
- `handler`: `route_handler`

**Returns:** `void`

*Defined at line 728*

---

## router_dispatch

Match a request against the router. Returns the response string.
If no route matches, returns a 404 response.

```fides
pub fn router_dispatch(router: Router, method: str, path: str) -> str {
```

**Parameters:**

- `router`: `Router`
- `method`: `str`
- `path`: `str`

**Returns:** `str`

*Defined at line 735*

---

## router_dispatch_request

Match a request against the router with the full request JSON.
The handler receives the full request JSON string.

```fides
pub fn router_dispatch_request(router: Router, request: str) -> str {
```

**Parameters:**

- `router`: `Router`
- `request`: `str`

**Returns:** `str`

*Defined at line 750*

---

## apply_middleware

Apply middleware to a handler. Returns a new handler that
calls the middleware first, then the original handler.
This is a simple composition — for now, middleware functions
must be manually composed.

```fides
pub fn apply_middleware(handler: route_handler, mw: middleware) -> route_handler {
```

**Parameters:**

- `handler`: `route_handler`
- `mw`: `middleware`

**Returns:** `route_handler`

*Defined at line 778*

---

## logging_middleware

Logging middleware — logs the request path before calling the handler.

```fides
pub fn logging_middleware(handler: route_handler) -> route_handler {
```

**Parameters:**

- `handler`: `route_handler`

**Returns:** `route_handler`

*Defined at line 783*

---

## cors_middleware

CORS middleware — adds CORS headers to the response.

```fides
pub fn cors_middleware(handler: route_handler) -> route_handler {
```

**Parameters:**

- `handler`: `route_handler`

**Returns:** `route_handler`

*Defined at line 792*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
