[All modules](index.md)

# dns -- Fides Documentation

## Contents

- [dns_build_query](#dns_build_query) (function)
- [dns_build_query_id](#dns_build_query_id) (function)
- [dns_v6_text](#dns_v6_text) (function)
- [dns_parse_response](#dns_parse_response) (function)
- [DnsRecord](#DnsRecord) (struct)
- [dns_parse_records](#dns_parse_records) (function)
- [dns_reply_matches](#dns_reply_matches) (function)
- [dns_parse_reply](#dns_parse_reply) (function)
- [DnsCache](#DnsCache) (struct)
- [dns_cache_new](#dns_cache_new) (function)
- [dns_cache_get](#dns_cache_get) (function)
- [dns_cache_put](#dns_cache_put) (function)
- [dns_cache_size](#dns_cache_size) (function)
- [dns_cache_resolve](#dns_cache_resolve) (function)
- [dns_query](#dns_query) (function)
- [dns_query_records](#dns_query_records) (function)
- [dns_resolve_all](#dns_resolve_all) (function)
- [dns_resolve_all_via](#dns_resolve_all_via) (function)
- [dns_resolve](#dns_resolve) (function)
- [dns_resolve_via](#dns_resolve_via) (function)
- [dns_resolve_v6](#dns_resolve_v6) (function)
- [dns_hostname_safe](#dns_hostname_safe) (function)
- [dns_resolve_system](#dns_resolve_system) (function)
- [dns_lookup](#dns_lookup) (function)

## dns_build_query

Build a DNS query packet for `hostname` (A record by default), with a
random query ID (bytes 0 and 1 of the packet).
qtype: 1=A, 28=AAAA, 5=CNAME, 15=MX, 16=TXT.
Returns a Vec<i64> of bytes; empty when `hostname` is not a valid name.

```fides
pub fn dns_build_query(hostname: str, qtype: i64) -> Vec {
```

**Parameters:**

- `hostname`: `str`
- `qtype`: `i64`

**Returns:** `Vec`

*Defined at line 34*

---

## dns_build_query_id

dns_build_query with the query ID `id` (0..65535) given. BG221 (Nov 24):
a fully qualified name's trailing dot ends the name (it was sent as an
empty label, which ended the name early and shifted QTYPE/QCLASS); an
empty label, a label over 63 bytes or a name over 253 bytes gives an
empty Vec (a long label was cut to 63 bytes: another name was asked).

```fides
pub fn dns_build_query_id(hostname: str, qtype: i64, id: i64) -> Vec {
```

**Parameters:**

- `hostname`: `str`
- `qtype`: `i64`
- `id`: `i64`

**Returns:** `Vec`

*Defined at line 53*

---

## dns_v6_text

An IPv6 address as text (RFC 5952): lowercase hex groups without
leading zeros, the longest run of two or more zero groups as "::".

```fides
pub fn dns_v6_text(groups: Vec<i64>) -> str {
```

**Parameters:**

- `groups`: `Vec<i64>`

**Returns:** `str`

*Defined at line 164*

---

## dns_parse_response

Parse a raw DNS response (malloc'd bytes, `len` long) and return the
resolved IPs as Vec<str>. qtype 1 → dotted-quad, 28 → hex groups.

```fides
pub fn dns_parse_response(buf: i64, len: i64) -> Vec<str> {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`

**Returns:** `Vec<str>`

*Defined at line 202*

---

## DnsRecord

One answer of a response: its type (1 A, 28 AAAA), time to live in
seconds, and address as text.

```fides
pub struct DnsRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `rtype` | `i64` |
| `ttl` | `i64` |
| `addr` | `str` |

*Defined at line 215*

---

## dns_parse_records

The A and AAAA answers of a raw response (`len` bytes at `buf`). Every
read is checked against `len`: a truncated packet gives the answers
before the cut.

BG147 (Nov 24): only a response is read: QR set, a standard query
(opcode 0), RCODE 0 (NXDOMAIN or any other error: no answers) and one
question. An answer counts only when its class is IN and its owner is
the question's name or a name a CNAME answer before it led to (the
first datagram from anyone, with any owner, was the answer). The query
ID and the question are checked against the query sent by
dns_parse_reply, which dns_query_records uses.

```fides
pub fn dns_parse_records(buf: i64, len: i64) -> Vec<DnsRecord> {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`

**Returns:** `Vec<DnsRecord>`

*Defined at line 296*

---

## dns_reply_matches

BG147: is the `len`-byte packet at `buf` the reply to the query with ID
`id` for `hostname` and `qtype`? Its ID, QR bit (a response), opcode
(standard query) and its one question (name, ignoring case and a
trailing dot; type; class IN) must match. RCODE is not looked at: a
matching NXDOMAIN is the answer (with no records).

```fides
pub fn dns_reply_matches(buf: i64, len: i64, id: i64, hostname: str, qtype: i64) -> bool {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`
- `id`: `i64`
- `hostname`: `str`
- `qtype`: `i64`

**Returns:** `bool`

*Defined at line 365*

---

## dns_parse_reply

The answers of type `qtype` in the reply to the query with ID `id` for
`hostname` (dns_build_query_id): none when the packet is not that reply
(dns_reply_matches) or carries an error (dns_parse_records).

```fides
pub fn dns_parse_reply(buf: i64, len: i64, id: i64, hostname: str, qtype: i64) -> Vec<DnsRecord> {
```

**Parameters:**

- `buf`: `i64`
- `len`: `i64`
- `id`: `i64`
- `hostname`: `str`
- `qtype`: `i64`

**Returns:** `Vec<DnsRecord>`

*Defined at line 387*

---

## DnsCache

```fides
pub struct DnsCache {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `addrs` | `Vec<Vec<str>>` |
| `expires` | `Vec<i64>` |

*Defined at line 408*

---

## dns_cache_new

```fides
pub fn dns_cache_new() -> DnsCache {
```

**Returns:** `DnsCache`

*Defined at line 414*

---

## dns_cache_get

The cached addresses of `name` at time `now`; empty when there are
none or they have expired.

```fides
pub fn dns_cache_get(c: &DnsCache, name: str, now: i64) -> Vec<str> {
```

**Parameters:**

- `c`: `&DnsCache`
- `name`: `str`
- `now`: `i64`

**Returns:** `Vec<str>`

*Defined at line 433*

---

## dns_cache_put

Keep `addrs` for `name` until `now + ttl` (seconds; names are not case
sensitive). A ttl of 0 or less keeps nothing.

```fides
pub fn dns_cache_put(c: &mut DnsCache, name: str, addrs: Vec<str>, ttl: i64, now: i64) {
```

**Parameters:**

- `c`: `&mut DnsCache`
- `name`: `str`
- `addrs`: `Vec<str>`
- `ttl`: `i64`
- `now`: `i64`

*Defined at line 444*

---

## dns_cache_size

How many names the cache holds unexpired at `now`.

```fides
pub fn dns_cache_size(c: &DnsCache, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&DnsCache`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 464*

---

## dns_cache_resolve

The A records of `hostname`: from the cache while they last, else a
query to `server` (its answers then cached for their smallest TTL).

```fides
pub fn dns_cache_resolve(c: &mut DnsCache, hostname: str, server: str, now: i64) -> Vec<str> {
```

**Parameters:**

- `c`: `&mut DnsCache`
- `hostname`: `str`
- `server`: `str`
- `now`: `i64`

**Returns:** `Vec<str>`

*Defined at line 478*

---

## dns_query

Query `server` (dotted-quad) for `hostname` qtype; returns Vec<str> of
answer addresses (empty on failure/timeout). Timeout ~2s.

```fides
pub fn dns_query(hostname: str, qtype: i64, server: str, port: i64, timeout_ms: i64) -> Vec<str> {
```

**Parameters:**

- `hostname`: `str`
- `qtype`: `i64`
- `server`: `str`
- `port`: `i64`
- `timeout_ms`: `i64`

**Returns:** `Vec<str>`

*Defined at line 502*

---

## dns_query_records

The answers to a query, with their TTLs (empty on failure or timeout).

```fides
pub fn dns_query_records(hostname: str, qtype: i64, server: str, port: i64, timeout_ms: i64) -> Vec<DnsRecord> {
```

**Parameters:**

- `hostname`: `str`
- `qtype`: `i64`
- `server`: `str`
- `port`: `i64`
- `timeout_ms`: `i64`

**Returns:** `Vec<DnsRecord>`

*Defined at line 514*

---

## dns_resolve_all

Resolve `hostname` to all A records via the default resolver.

```fides
pub fn dns_resolve_all(hostname: str) -> Vec<str> {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `Vec<str>`

*Defined at line 598*

---

## dns_resolve_all_via

Resolve via a specific resolver IP.

```fides
pub fn dns_resolve_all_via(hostname: str, server: str) -> Vec<str> {
```

**Parameters:**

- `hostname`: `str`
- `server`: `str`

**Returns:** `Vec<str>`

*Defined at line 603*

---

## dns_resolve

First A record for `hostname`, or "" on failure.

```fides
pub fn dns_resolve(hostname: str) -> str {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `str`

*Defined at line 608*

---

## dns_resolve_via

```fides
pub fn dns_resolve_via(hostname: str, server: str) -> str {
```

**Parameters:**

- `hostname`: `str`
- `server`: `str`

**Returns:** `str`

*Defined at line 616*

---

## dns_resolve_v6

All AAAA records for `hostname`.

```fides
pub fn dns_resolve_v6(hostname: str) -> Vec<str> {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `Vec<str>`

*Defined at line 625*

---

## dns_hostname_safe

Is `hostname` safe to pass to a shell resolver command? Only the DNS
alphabet is allowed: letters, digits, '.', '-', '_' and ':' (IPv6
literals). Anything else (spaces, ';', '&', '$', quotes, ...) makes
the name unsafe and the lookup returns "".

```fides
pub fn dns_hostname_safe(hostname: str) -> bool {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `bool`

*Defined at line 712*

---

## dns_resolve_system

Resolve via the OS resolver: nslookup on Windows, getent on Unix.
Returns first IPv4 address or "" on failure. Slower than a wire query
(process spawn) but needs no socket FFI. Names outside the DNS
alphabet are rejected before the command is built (shell injection).

```fides
pub fn dns_resolve_system(hostname: str) -> str {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `str`

*Defined at line 742*

---

## dns_lookup

Recommended entry point: resolves through the OS resolver, which
works on every backend (the wire-format path needs pledge_net
linked and panics under `fid run` if it isn't).
Use dns_resolve()/dns_resolve_via() explicitly for the UDP path.

```fides
pub fn dns_lookup(hostname: str) -> str {
```

**Parameters:**

- `hostname`: `str`

**Returns:** `str`

*Defined at line 773*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
