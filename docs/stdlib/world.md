[All modules](index.md)

# world -- Fides Documentation

## Contents

- [world_fake_files](#world_fake_files) (function)
- [world_real_files](#world_real_files) (function)
- [world_files_faked](#world_files_faked) (function)
- [world_file_put](#world_file_put) (function)
- [world_file_exists](#world_file_exists) (function)
- [world_file_text](#world_file_text) (function)
- [world_file_get](#world_file_get) (function)
- [world_file_remove](#world_file_remove) (function)
- [world_file_list](#world_file_list) (function)
- [world_file_reads](#world_file_reads) (function)
- [world_file_writes](#world_file_writes) (function)
- [world_fs_read](#world_fs_read) (function)
- [world_fs_write](#world_fs_write) (function)
- [world_fs_append](#world_fs_append) (function)
- [world_dir_exists](#world_dir_exists) (function)
- [world_dir_list](#world_dir_list) (function)
- [world_fs_mkdir](#world_fs_mkdir) (function)
- [world_fs_mkdir_all](#world_fs_mkdir_all) (function)
- [world_dir_put](#world_dir_put) (function)
- [world_fs_rmdir](#world_fs_rmdir) (function)
- [world_fs_rename](#world_fs_rename) (function)
- [world_fake_clock](#world_fake_clock) (function)
- [world_clock_read_count](#world_clock_read_count) (function)
- [world_clock_advance](#world_clock_advance) (function)
- [world_clock_auto_step](#world_clock_auto_step) (function)
- [world_real_clock](#world_real_clock) (function)
- [world_clock_faked](#world_clock_faked) (function)
- [world_clock_ms](#world_clock_ms) (function)
- [world_clock_peek](#world_clock_peek) (function)
- [world_net_refuse](#world_net_refuse) (function)
- [world_fake_network](#world_fake_network) (function)
- [world_no_network](#world_no_network) (function)
- [world_real_network](#world_real_network) (function)
- [world_network_faked](#world_network_faked) (function)
- [world_net_exchange](#world_net_exchange) (function)
- [world_net_count](#world_net_count) (function)
- [world_net_host](#world_net_host) (function)
- [world_net_request](#world_net_request) (function)
- [world_http_response](#world_http_response) (function)
- [world_req_method](#world_req_method) (function)
- [world_req_path](#world_req_path) (function)
- [world_req_body](#world_req_body) (function)
- [world_req_header](#world_req_header) (function)
- [WORLD_SOCK_BASE:](#WORLD_SOCK_BASE:) (const)
- [world_sock_silent](#world_sock_silent) (function)
- [world_fake_sockets](#world_fake_sockets) (function)
- [world_no_sockets](#world_no_sockets) (function)
- [world_real_sockets](#world_real_sockets) (function)
- [world_sockets_faked](#world_sockets_faked) (function)
- [world_ip_value](#world_ip_value) (function)
- [world_ip_text](#world_ip_text) (function)
- [world_sock_open](#world_sock_open) (function)
- [world_sock_bind](#world_sock_bind) (function)
- [world_sock_connect](#world_sock_connect) (function)
- [world_sock_send](#world_sock_send) (function)
- [world_sock_send_to](#world_sock_send_to) (function)
- [world_sock_recv](#world_sock_recv) (function)
- [world_sock_last_from](#world_sock_last_from) (function)
- [world_sock_last_ip](#world_sock_last_ip) (function)
- [world_sock_last_port](#world_sock_last_port) (function)
- [world_sock_close](#world_sock_close) (function)
- [world_sock_deliver](#world_sock_deliver) (function)
- [world_sock_deliver_port](#world_sock_deliver_port) (function)
- [world_sock_sent_count](#world_sock_sent_count) (function)
- [world_sock_sent_peer](#world_sock_sent_peer) (function)
- [world_sock_sent_proto](#world_sock_sent_proto) (function)
- [world_sock_sent_bytes](#world_sock_sent_bytes) (function)
- [world_sock_sent_text](#world_sock_sent_text) (function)
- [world_sock_refused](#world_sock_refused) (function)
- [world_sock_open_count](#world_sock_open_count) (function)
- [world_hermetic](#world_hermetic) (function)
- [world_real](#world_real) (function)

## world_fake_files

Replace the file system with an empty in-memory one.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
world_fake_files();
assert(world_files_faked());
world_file_put("./cfg/app.ini", "port = 1");
assert(world_file_exists("cfg/app.ini"));
assert_eq(world_file_text("cfg\\app.ini"), "port = 1");
assert_eq(world_file_list().len(), 1);
assert(world_file_remove("cfg/app.ini"));
assert(!world_file_exists("cfg/app.ini"));
world_real_files();
assert(!world_files_faked());
```

```fides
pub fn world_fake_files() -> void {
```

**Returns:** `void`

*Defined at line 71*

---

## world_real_files

Go back to the real file system (the fake's files are dropped).

```fides
pub fn world_real_files() -> void {
```

**Returns:** `void`

*Defined at line 81*

---

## world_files_faked

True while the file system is faked.

```fides
pub fn world_files_faked() -> bool {
```

**Returns:** `bool`

*Defined at line 89*

---

## world_file_put

Create or replace a fake file (seeding does not count as a write).

```fides
pub fn world_file_put(path: str, content: str) -> void {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `void`

*Defined at line 115*

---

## world_file_exists

True when the fake file `path` exists.

```fides
pub fn world_file_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 126*

---

## world_file_text

The fake file's text ("" when it does not exist).

```fides
pub fn world_file_text(path: str) -> str {
```

**Parameters:**

- `path`: `str`

**Returns:** `str`

*Defined at line 131*

---

## world_file_get

The fake file's text, None when it does not exist.

```fides
pub fn world_file_get(path: str) -> Option<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Option<str>`

*Defined at line 140*

---

## world_file_remove

Remove a fake file; false when it did not exist.

```fides
pub fn world_file_remove(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 149*

---

## world_file_list

The fake files' paths, in creation order.

```fides
pub fn world_file_list() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 160*

---

## world_file_reads

How many reads / writes the code made through the fake (fs functions).

```fides
pub fn world_file_reads() -> i64 {
```

**Returns:** `i64`

*Defined at line 171*

---

## world_file_writes

```fides
pub fn world_file_writes() -> i64 {
```

**Returns:** `i64`

*Defined at line 175*

---

## world_fs_read

fs read hook: the file's text, None when missing.

```fides
pub fn world_fs_read(path: str) -> Option<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Option<str>`

*Defined at line 182*

---

## world_fs_write

fs write hook: replace the file's text (false when `path` is a
directory of the fake).

```fides
pub fn world_fs_write(path: str, content: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `bool`

*Defined at line 189*

---

## world_fs_append

fs append hook: add to the file (creating it; false when `path` is a
directory of the fake).

```fides
pub fn world_fs_append(path: str, content: str) -> bool {
```

**Parameters:**

- `path`: `str`
- `content`: `str`

**Returns:** `bool`

*Defined at line 200*

---

## world_dir_exists

True when `path` is a directory of the fake file system.

```fides
pub fn world_dir_exists(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 267*

---

## world_dir_list

The names in fake directory `path` (files and directories, each once,
sorted by name; no "." or ".."): empty when it is not a directory.

```fides
pub fn world_dir_list(path: str) -> Vec<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Vec<str>`

*Defined at line 343*

---

## world_fs_mkdir

fs mkdir hook: make one directory; false when something already has
that name or the directory holding it is missing (as mkdir).

```fides
pub fn world_fs_mkdir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 357*

---

## world_fs_mkdir_all

fs mkdir_all hook: make a directory and every missing one above it;
true when it exists afterwards (false when a file is in the way).

```fides
pub fn world_fs_mkdir_all(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 375*

---

## world_dir_put

Seed a directory (and those above it) without counting it as a write.

```fides
pub fn world_dir_put(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 396*

---

## world_fs_rmdir

fs rmdir hook: remove an empty directory; false when `path` is not a
directory or still holds something.

```fides
pub fn world_fs_rmdir(path: str) -> bool {
```

**Parameters:**

- `path`: `str`

**Returns:** `bool`

*Defined at line 404*

---

## world_fs_rename

fs rename hook: move a file (replacing a file of the new name), or a
directory with everything in it; false when `old_path` does not exist
or a directory is in the way.

```fides
pub fn world_fs_rename(old_path: str, new_path: str) -> bool {
```

**Parameters:**

- `old_path`: `str`
- `new_path`: `str`

**Returns:** `bool`

*Defined at line 429*

---

## world_fake_clock

Fix the clock at `ms` milliseconds since the Unix epoch. It moves only
when the test advances it (or by the auto step, below).

```fides
pub fn world_fake_clock(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 485*

---

## world_clock_read_count

How many times the code read the fake clock (clock_now_*, the
clock_elapsed_* functions, dt_now).

```fides
pub fn world_clock_read_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 494*

---

## world_clock_advance

Advance the fake clock by `ms`.

```fides
pub fn world_clock_advance(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 499*

---

## world_clock_auto_step

Make every reading of the fake clock advance it by `ms` afterwards
(code that waits for time to pass then sees it pass; 0 = frozen).

```fides
pub fn world_clock_auto_step(ms: i64) -> void {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `void`

*Defined at line 505*

---

## world_real_clock

Go back to the OS clock.

```fides
pub fn world_real_clock() -> void {
```

**Returns:** `void`

*Defined at line 510*

---

## world_clock_faked

True while the clock is faked.

```fides
pub fn world_clock_faked() -> bool {
```

**Returns:** `bool`

*Defined at line 515*

---

## world_clock_ms

The fake clock in milliseconds (the time hook; takes the auto step).

```fides
pub fn world_clock_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 520*

---

## world_clock_peek

The fake clock's current reading, without counting it as the code's
read or taking the auto step (for the test's own checks).

```fides
pub fn world_clock_peek() -> i64 {
```

**Returns:** `i64`

*Defined at line 529*

---

## world_net_refuse

The handler world_no_network installs: no answer (transport failure).

```fides
pub fn world_net_refuse(_host: str, _port: i64, _request: str) -> str {
```

**Parameters:**

- `_host`: `str`
- `_port`: `i64`
- `_request`: `str`

**Returns:** `str`

*Defined at line 541*

---

## world_fake_network

Answer every request with `handler(host, port, raw_request)`: the raw
HTTP request text the client would send, and the raw response text it
would read back ("" = connection failed). world_http_response builds
one; world_req_method/path/body/header read the request.

```fides
pub fn world_fake_network(handler: fn(str, i64, str) -> str) -> void {
```

**Parameters:**

- `handler`: `fn(str`

**Returns:** `void`

*Defined at line 549*

---

## world_no_network

Refuse every request (status 0), recording each attempt: a test that
asserts world_net_count() == 0 shows the code made none.

```fides
pub fn world_no_network() -> void {
```

**Returns:** `void`

*Defined at line 558*

---

## world_real_network

Go back to the real network.

```fides
pub fn world_real_network() -> void {
```

**Returns:** `void`

*Defined at line 563*

---

## world_network_faked

True while the network is faked.

```fides
pub fn world_network_faked() -> bool {
```

**Returns:** `bool`

*Defined at line 569*

---

## world_net_exchange

The network hook http_client calls: records the request, returns the
handler's raw response.

```fides
pub fn world_net_exchange(host: str, port: i64, request: str) -> str {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`
- `request`: `str`

**Returns:** `str`

*Defined at line 575*

---

## world_net_count

Requests the code made through the fake network.

```fides
pub fn world_net_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 583*

---

## world_net_host

"host:port" of request `i`.

```fides
pub fn world_net_host(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 588*

---

## world_net_request

The raw text of request `i`.

```fides
pub fn world_net_request(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 596*

---

## world_http_response

A raw HTTP/1.1 response with `status`, a text/plain body and its length.

```fides
pub fn world_http_response(status: i64, body: str) -> str {
```

**Parameters:**

- `status`: `i64`
- `body`: `str`

**Returns:** `str`

*Defined at line 604*

---

## world_req_method

The method of a raw request ("GET").

```fides
pub fn world_req_method(req: str) -> str {
```

**Parameters:**

- `req`: `str`

**Returns:** `str`

*Defined at line 642*

---

## world_req_path

The path of a raw request ("/api/x?y=1").

```fides
pub fn world_req_path(req: str) -> str {
```

**Parameters:**

- `req`: `str`

**Returns:** `str`

*Defined at line 647*

---

## world_req_body

The body of a raw request (after the blank line).

```fides
pub fn world_req_body(req: str) -> str {
```

**Parameters:**

- `req`: `str`

**Returns:** `str`

*Defined at line 652*

---

## world_req_header

The value of header `name` in a raw request ("" when absent; the name
is matched without regard to case).

```fides
pub fn world_req_header(req: str, name: str) -> str {
```

**Parameters:**

- `req`: `str`
- `name`: `str`

**Returns:** `str`

*Defined at line 662*

---

## WORLD_SOCK_BASE:

The first fake socket descriptor (2^40: no real descriptor is this big).

```fides
pub const WORLD_SOCK_BASE: i64 = 1099511627776;
```

*Defined at line 700*

---

## world_sock_silent

The handler world_no_sockets installs: never answers.

```fides
pub fn world_sock_silent(_proto: str, _peer: str, _data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `_proto`: `str`
- `_peer`: `str`
- `_data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 726*

---

## world_fake_sockets

Replace the sockets: every send goes to `handler(proto, peer, bytes)`,
whose non-empty answer the sending socket then receives.

```fides
pub fn world_fake_sockets(handler: fn(str, str, Vec<i64>) -> Vec<i64>) -> void {
```

**Parameters:**

- `handler`: `fn(str`

**Returns:** `void`

*Defined at line 752*

---

## world_no_sockets

Fake sockets that reach nobody: every connect and send fails (-1) and
is counted (world_sock_refused), so a test can show the code tried.

```fides
pub fn world_no_sockets() -> void {
```

**Returns:** `void`

*Defined at line 761*

---

## world_real_sockets

Go back to real sockets.

```fides
pub fn world_real_sockets() -> void {
```

**Returns:** `void`

*Defined at line 767*

---

## world_sockets_faked

True while the sockets are faked.

```fides
pub fn world_sockets_faked() -> bool {
```

**Returns:** `bool`

*Defined at line 775*

---

## world_ip_value

"a.b.c.d" as net_addr_create keeps it (inet_addr's value: the first
number in the low byte); -1 when it is not four numbers 0..255.

```fides
pub fn world_ip_value(ip: str) -> i64 {
```

**Parameters:**

- `ip`: `str`

**Returns:** `i64`

*Defined at line 781*

---

## world_ip_text

The "a.b.c.d" text of an address value (world_ip_value's inverse).

```fides
pub fn world_ip_text(ip: i64) -> str {
```

**Parameters:**

- `ip`: `i64`

**Returns:** `str`

*Defined at line 812*

---

## world_sock_open

net socket hook: a new fake socket ("tcp" when `tcp`, else "udp").

```fides
pub fn world_sock_open(tcp: bool) -> i64 {
```

**Parameters:**

- `tcp`: `bool`

**Returns:** `i64`

*Defined at line 829*

---

## world_sock_bind

net bind hook: 0, or -1 for a socket that is not a fake one or a port
another open fake socket of the same kind holds.

```fides
pub fn world_sock_bind(sock: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 843*

---

## world_sock_connect

net connect hook: 0, or -1 (not a fake socket, or no sockets).

```fides
pub fn world_sock_connect(sock: i64, ip: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `ip`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 860*

---

## world_sock_send

net send hook (a connected socket): the bytes sent, -1 when the socket
is not a connected fake one or there are no sockets.

```fides
pub fn world_sock_send(sock: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 910*

---

## world_sock_send_to

net send_to hook (a datagram to ip:port): the bytes sent, or -1.

```fides
pub fn world_sock_send_to(sock: i64, data: Vec<i64>, ip: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `Vec<i64>`
- `ip`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 923*

---

## world_sock_recv

net recv hook: up to `max` queued bytes (empty when none are queued);
world_sock_last_from then names the peer they came from.

```fides
pub fn world_sock_recv(sock: i64, max: i64) -> Vec<i64> {
```

**Parameters:**

- `sock`: `i64`
- `max`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 933*

---

## world_sock_last_from

The peer ("a.b.c.d:port") the last bytes world_sock_recv gave came from.

```fides
pub fn world_sock_last_from() -> str {
```

**Returns:** `str`

*Defined at line 963*

---

## world_sock_last_ip

The address value (world_ip_value) of world_sock_last_from.

```fides
pub fn world_sock_last_ip() -> i64 {
```

**Returns:** `i64`

*Defined at line 968*

---

## world_sock_last_port

The port of world_sock_last_from.

```fides
pub fn world_sock_last_port() -> i64 {
```

**Returns:** `i64`

*Defined at line 977*

---

## world_sock_close

net close hook: the socket reads and sends nothing afterwards, and what
was queued for it is dropped. 0, or -1 for a socket that is not open.

```fides
pub fn world_sock_close(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 987*

---

## world_sock_deliver

Hand `data` to fake socket `sock` as if `from` ("a.b.c.d:port") sent
it; false when `sock` is not an open fake socket.

```fides
pub fn world_sock_deliver(sock: i64, from: str, data: Vec<i64>) -> bool {
```

**Parameters:**

- `sock`: `i64`
- `from`: `str`
- `data`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1005*

---

## world_sock_deliver_port

world_sock_deliver to the open fake socket bound to `port`; false when
none is.

```fides
pub fn world_sock_deliver_port(port: i64, from: str, data: Vec<i64>) -> bool {
```

**Parameters:**

- `port`: `i64`
- `from`: `str`
- `data`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 1016*

---

## world_sock_sent_count

Sends the code made through fake sockets (datagrams and stream writes).

```fides
pub fn world_sock_sent_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 1029*

---

## world_sock_sent_peer

"a.b.c.d:port" of send `i`.

```fides
pub fn world_sock_sent_peer(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 1034*

---

## world_sock_sent_proto

"udp" or "tcp": the kind of socket send `i` went through.

```fides
pub fn world_sock_sent_proto(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 1042*

---

## world_sock_sent_bytes

The bytes of send `i`.

```fides
pub fn world_sock_sent_bytes(i: i64) -> Vec<i64> {
```

**Parameters:**

- `i`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1050*

---

## world_sock_sent_text

The bytes of send `i` as text.

```fides
pub fn world_sock_sent_text(i: i64) -> str {
```

**Parameters:**

- `i`: `i64`

**Returns:** `str`

*Defined at line 1064*

---

## world_sock_refused

Connects and sends world_no_sockets refused.

```fides
pub fn world_sock_refused() -> i64 {
```

**Returns:** `i64`

*Defined at line 1069*

---

## world_sock_open_count

Fake sockets still open (one the code forgot to close shows here).

```fides
pub fn world_sock_open_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 1074*

---

## world_hermetic

Turn every fake on: an empty file system, the clock fixed at the epoch
(0 ms), a network that refuses every request and sockets that reach
nobody. Code under test then cannot reach the real world through the
stdlib; seed what it needs.

```fides
pub fn world_hermetic() -> void {
```

**Returns:** `void`

*Defined at line 1090*

---

## world_real

Turn every fake off again.

```fides
pub fn world_real() -> void {
```

**Returns:** `void`

*Defined at line 1098*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
