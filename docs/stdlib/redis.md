[All modules](index.md)

# redis -- Fides Documentation

## Contents

- [RESP_SIMPLE:](#RESP_SIMPLE:) (const)
- [RESP_ERROR:](#RESP_ERROR:) (const)
- [RESP_INT:](#RESP_INT:) (const)
- [RESP_BULK:](#RESP_BULK:) (const)
- [RESP_ARRAY:](#RESP_ARRAY:) (const)
- [RESP_NULL:](#RESP_NULL:) (const)
- [RESP_BOOL:](#RESP_BOOL:) (const)
- [RESP_DOUBLE:](#RESP_DOUBLE:) (const)
- [RESP_BIGNUM:](#RESP_BIGNUM:) (const)
- [RESP_BULK_ERROR:](#RESP_BULK_ERROR:) (const)
- [RESP_VERBATIM:](#RESP_VERBATIM:) (const)
- [RESP_MAP:](#RESP_MAP:) (const)
- [RESP_SET:](#RESP_SET:) (const)
- [RESP_ATTR:](#RESP_ATTR:) (const)
- [RESP_PUSH:](#RESP_PUSH:) (const)
- [RESP_MAX_DEPTH:](#RESP_MAX_DEPTH:) (const)
- [RESP_MAX_BULK:](#RESP_MAX_BULK:) (const)
- [RESP_MAX_COUNT:](#RESP_MAX_COUNT:) (const)
- [resp_put_simple](#resp_put_simple) (function)
- [resp_put_error](#resp_put_error) (function)
- [resp_put_int](#resp_put_int) (function)
- [resp_put_bulk](#resp_put_bulk) (function)
- [resp_put_bulk_bytes](#resp_put_bulk_bytes) (function)
- [resp_put_null](#resp_put_null) (function)
- [resp_put_null_bulk](#resp_put_null_bulk) (function)
- [resp_put_aggregate](#resp_put_aggregate) (function)
- [resp_put_bool](#resp_put_bool) (function)
- [resp_put_double](#resp_put_double) (function)
- [resp_put_bignum](#resp_put_bignum) (function)
- [resp_put_bulk_error](#resp_put_bulk_error) (function)
- [resp_put_verbatim](#resp_put_verbatim) (function)
- [resp_encode_command](#resp_encode_command) (function)
- [resp_encode_inline](#resp_encode_inline) (function)
- [RespDoc](#RespDoc) (struct)
- [resp_decode_at](#resp_decode_at) (function)
- [resp_decode](#resp_decode) (function)
- [resp_kind](#resp_kind) (function)
- [resp_text](#resp_text) (function)
- [resp_int](#resp_int) (function)
- [resp_double](#resp_double) (function)
- [resp_count](#resp_count) (function)
- [resp_child](#resp_child) (function)
- [resp_is_error](#resp_is_error) (function)
- [resp_attrs](#resp_attrs) (function)
- [resp_map_get](#resp_map_get) (function)
- [resp_to_str](#resp_to_str) (function)
- [resp_encode_node](#resp_encode_node) (function)
- [RedisPubSub](#RedisPubSub) (struct)
- [redis_pubsub_parse](#redis_pubsub_parse) (function)
- [redis_cmd1](#redis_cmd1) (function)
- [redis_cmd2](#redis_cmd2) (function)
- [redis_cmd3](#redis_cmd3) (function)
- [redis_cmd4](#redis_cmd4) (function)
- [redis_cmd_set](#redis_cmd_set) (function)
- [RedisTransport](#RedisTransport) (struct)
- [redis_tcp_transport](#redis_tcp_transport) (function)
- [RedisClient](#RedisClient) (struct)
- [redis_client_new](#redis_client_new) (function)
- [redis_queue](#redis_queue) (function)
- [redis_read_reply](#redis_read_reply) (function)
- [redis_flush](#redis_flush) (function)
- [redis_call](#redis_call) (function)
- [redis_get](#redis_get) (function)
- [redis_set](#redis_set) (function)
- [redis_set_ex](#redis_set_ex) (function)
- [redis_incr](#redis_incr) (function)
- [redis_incrby](#redis_incrby) (function)
- [redis_del](#redis_del) (function)
- [redis_hset](#redis_hset) (function)
- [redis_hget](#redis_hget) (function)
- [redis_lpush](#redis_lpush) (function)
- [redis_rpush](#redis_rpush) (function)
- [redis_lrange](#redis_lrange) (function)
- [redis_expire](#redis_expire) (function)
- [redis_ttl](#redis_ttl) (function)
- [redis_publish](#redis_publish) (function)
- [redis_subscribe](#redis_subscribe) (function)
- [redis_next_event](#redis_next_event) (function)
- [redis_hello3](#redis_hello3) (function)

## RESP_SIMPLE:

```fides
pub const RESP_SIMPLE: i64 = 43;  // '+'
```

*Defined at line 46*

---

## RESP_ERROR:

```fides
pub const RESP_ERROR: i64 = 45;  // '-'
```

*Defined at line 47*

---

## RESP_INT:

```fides
pub const RESP_INT: i64 = 58;  // ':'
```

*Defined at line 48*

---

## RESP_BULK:

```fides
pub const RESP_BULK: i64 = 36;  // '$'
```

*Defined at line 49*

---

## RESP_ARRAY:

```fides
pub const RESP_ARRAY: i64 = 42;  // '*'
```

*Defined at line 50*

---

## RESP_NULL:

```fides
pub const RESP_NULL: i64 = 95;  // '_'
```

*Defined at line 51*

---

## RESP_BOOL:

```fides
pub const RESP_BOOL: i64 = 35;  // '#'
```

*Defined at line 52*

---

## RESP_DOUBLE:

```fides
pub const RESP_DOUBLE: i64 = 44;  // ','
```

*Defined at line 53*

---

## RESP_BIGNUM:

```fides
pub const RESP_BIGNUM: i64 = 40;  // '('
```

*Defined at line 54*

---

## RESP_BULK_ERROR:

```fides
pub const RESP_BULK_ERROR: i64 = 33;  // '!'
```

*Defined at line 55*

---

## RESP_VERBATIM:

```fides
pub const RESP_VERBATIM: i64 = 61;  // '='
```

*Defined at line 56*

---

## RESP_MAP:

```fides
pub const RESP_MAP: i64 = 37;  // '%'
```

*Defined at line 57*

---

## RESP_SET:

```fides
pub const RESP_SET: i64 = 126;  // '~'
```

*Defined at line 58*

---

## RESP_ATTR:

```fides
pub const RESP_ATTR: i64 = 124;  // '|'
```

*Defined at line 59*

---

## RESP_PUSH:

```fides
pub const RESP_PUSH: i64 = 62;  // '>'
```

*Defined at line 60*

---

## RESP_MAX_DEPTH:

Deepest nesting the decoder accepts.

```fides
pub const RESP_MAX_DEPTH: i64 = 64;
```

*Defined at line 63*

---

## RESP_MAX_BULK:

Largest bulk string accepted (Redis' own proto-max-bulk-len default).

```fides
pub const RESP_MAX_BULK: i64 = 536870912;
```

*Defined at line 65*

---

## RESP_MAX_COUNT:

Largest element count of an aggregate (hiredis' limit, 2^32 - 1).

```fides
pub const RESP_MAX_COUNT: i64 = 4294967295;
```

*Defined at line 67*

---

## resp_put_simple

+text\r\n (text must not hold CR or LF)

```fides
pub fn resp_put_simple(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 92*

---

## resp_put_error

-ERR message\r\n

```fides
pub fn resp_put_error(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 99*

---

## resp_put_int

:n\r\n

```fides
pub fn resp_put_int(out: &mut Vec<i64>, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `n`: `i64`

*Defined at line 106*

---

## resp_put_bulk

$len\r\nbytes\r\n (binary safe: a str carries its length)

```fides
pub fn resp_put_bulk(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 111*

---

## resp_put_bulk_bytes

A bulk string from bytes.

```fides
pub fn resp_put_bulk_bytes(out: &mut Vec<i64>, b: Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `b`: `Vec<i64>`

*Defined at line 118*

---

## resp_put_null

RESP3 null: _\r\n

```fides
pub fn resp_put_null(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 129*

---

## resp_put_null_bulk

RESP2 null bulk string: $-1\r\n

```fides
pub fn resp_put_null_bulk(out: &mut Vec<i64>) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`

*Defined at line 135*

---

## resp_put_aggregate

The head of an aggregate: kind is RESP_ARRAY, RESP_SET, RESP_PUSH, or
RESP_MAP / RESP_ATTR with n = the number of pairs. The n items follow.

```fides
pub fn resp_put_aggregate(out: &mut Vec<i64>, kind: i64, n: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `kind`: `i64`
- `n`: `i64`

*Defined at line 141*

---

## resp_put_bool

#t\r\n or #f\r\n

```fides
pub fn resp_put_bool(out: &mut Vec<i64>, b: bool) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `b`: `bool`

*Defined at line 146*

---

## resp_put_double

,1.5\r\n (inf, -inf and nan as RESP3 writes them)

```fides
pub fn resp_put_double(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 157*

---

## resp_put_bignum

(digits\r\n: a big number given as its decimal text

```fides
pub fn resp_put_bignum(out: &mut Vec<i64>, digits: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `digits`: `str`

*Defined at line 172*

---

## resp_put_bulk_error

!len\r\nmessage\r\n

```fides
pub fn resp_put_bulk_error(out: &mut Vec<i64>, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `s`: `str`

*Defined at line 179*

---

## resp_put_verbatim

=len\r\nfmt:text\r\n (fmt is three bytes, e.g. "txt" or "mkd")

```fides
pub fn resp_put_verbatim(out: &mut Vec<i64>, fmt: str, s: str) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `fmt`: `str`
- `s`: `str`

*Defined at line 186*

---

## resp_encode_command

A command as clients send it: an array of bulk strings.

```fides
pub fn resp_encode_command(args: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `args`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 195*

---

## resp_encode_inline

The inline form some tools send ("PING\r\n"): words joined by spaces.

```fides
pub fn resp_encode_inline(args: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `args`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 207*

---

## RespDoc

A decoded reply as a flat tree. Node i has kind[i] (RESP_*) and:
SIMPLE/ERROR/BULK/BULK_ERROR/BIGNUM/DOUBLE: text = its bytes as a str;
VERBATIM: text = the text after "fmt:", fmt = the format;
INT: num = the value; BOOL: num = 1 or 0; DOUBLE: fnum = the value;
ARRAY/SET/PUSH: children = the items; MAP/ATTR: key, value, key, ...
An attribute (|) is decoded, kept in `attrs` of the node it describes,
and not counted as an item. ok false: `error` says why; need_more true:
the bytes end before a whole reply (read more and decode again).

```fides
pub struct RespDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `Vec<i64>` |
| `num` | `Vec<i64>` |
| `fnum` | `Vec<f64>` |
| `text` | `Vec<str>` |
| `fmt` | `Vec<str>` |
| `first` | `Vec<i64>` |
| `nkids` | `Vec<i64>` |
| `attrs` | `Vec<i64>` |
| `kids` | `Vec<i64>` |
| `root` | `i64` |
| `ok` | `bool` |
| `need_more` | `bool` |
| `error` | `str` |
| `used` | `i64` |

*Defined at line 231*

---

## resp_decode_at

Decode one reply starting at byte `at`. On success `used` is the
number of bytes it took (the next reply starts at at + used).

```fides
pub fn resp_decode_at(bytes: Vec<i64>, at: i64) -> RespDoc {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `at`: `i64`

**Returns:** `RespDoc`

*Defined at line 580*

---

## resp_decode

```fides
pub fn resp_decode(bytes: Vec<i64>) -> RespDoc {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `RespDoc`

*Defined at line 596*

---

## resp_kind

```fides
pub fn resp_kind(d: &RespDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 602*

---

## resp_text

```fides
pub fn resp_text(d: &RespDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 606*

---

## resp_int

```fides
pub fn resp_int(d: &RespDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 610*

---

## resp_double

```fides
pub fn resp_double(d: &RespDoc, i: i64) -> f64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `f64`

*Defined at line 614*

---

## resp_count

Items of an array/set/push; key+value entries (2 per pair) of a map.

```fides
pub fn resp_count(d: &RespDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 619*

---

## resp_child

```fides
pub fn resp_child(d: &RespDoc, i: i64, k: i64) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`
- `k`: `i64`

**Returns:** `i64`

*Defined at line 623*

---

## resp_is_error

```fides
pub fn resp_is_error(d: &RespDoc, i: i64) -> bool {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `bool`

*Defined at line 627*

---

## resp_attrs

The attribute node attached to node i (-1 when none).

```fides
pub fn resp_attrs(d: &RespDoc, i: i64) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 633*

---

## resp_map_get

The value of map node `i` whose key is the string `key` (-1 if none).

```fides
pub fn resp_map_get(d: &RespDoc, i: i64, key: str) -> i64 {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`
- `key`: `str`

**Returns:** `i64`

*Defined at line 638*

---

## resp_to_str

A one-line rendering of node i (for logs and tests).

```fides
pub fn resp_to_str(d: &RespDoc, i: i64) -> str {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `str`

*Defined at line 672*

---

## resp_encode_node

Re-encode node i (attributes included) to its RESP3 bytes.

```fides
pub fn resp_encode_node(d: &RespDoc, i: i64, out: &mut Vec<i64>) {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`
- `out`: `&mut Vec<i64>`

*Defined at line 740*

---

## RedisPubSub

A pub/sub event. kind: "message", "pmessage", "smessage", "subscribe",
"unsubscribe", "psubscribe", "punsubscribe", "pong", or "" when the
reply is not a pub/sub event. count: the subscription count for the
(un)subscribe kinds.

```fides
pub struct RedisPubSub {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `pattern` | `str` |
| `channel` | `str` |
| `payload` | `str` |
| `count` | `i64` |

*Defined at line 794*

---

## redis_pubsub_parse

Read a pub/sub event from node i: a RESP2 array or a RESP3 push.

```fides
pub fn redis_pubsub_parse(d: &RespDoc, i: i64) -> RedisPubSub {
```

**Parameters:**

- `d`: `&RespDoc`
- `i`: `i64`

**Returns:** `RedisPubSub`

*Defined at line 803*

---

## redis_cmd1

```fides
pub fn redis_cmd1(a: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`

**Returns:** `Vec<str>`

*Defined at line 841*

---

## redis_cmd2

```fides
pub fn redis_cmd2(a: str, b: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `Vec<str>`

*Defined at line 847*

---

## redis_cmd3

```fides
pub fn redis_cmd3(a: str, b: str, c: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`

**Returns:** `Vec<str>`

*Defined at line 853*

---

## redis_cmd4

```fides
pub fn redis_cmd4(a: str, b: str, c: str, e: str) -> Vec<str> {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `c`: `str`
- `e`: `str`

**Returns:** `Vec<str>`

*Defined at line 859*

---

## redis_cmd_set

SET key value [EX seconds] [NX]: ex_seconds 0 = no expiry.

```fides
pub fn redis_cmd_set(key: str, value: str, ex_seconds: i64, only_if_absent: bool) -> Vec<str> {
```

**Parameters:**

- `key`: `str`
- `value`: `str`
- `ex_seconds`: `i64`
- `only_if_absent`: `bool`

**Returns:** `Vec<str>`

*Defined at line 866*

---

## RedisTransport

```fides
pub struct RedisTransport {
```

**Fields:**

| Name | Type |
|------|------|
| `conn` | `i64` |
| `send` | `fn(i64, Vec<i64>) -> i64` |
| `recv` | `fn(i64, i64) -> Vec<i64>` |

*Defined at line 880*

---

## redis_tcp_transport

A transport over a connected TCP socket (stdlib net).

```fides
pub fn redis_tcp_transport(sock: i64) -> RedisTransport {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `RedisTransport`

*Defined at line 887*

---

## RedisClient

A client: `queued` holds the commands not yet written, `inbuf` the
bytes read and not yet decoded. ok false after a transport or protocol
error (`error` says which); a Redis error reply does not set it.

```fides
pub struct RedisClient {
```

**Fields:**

| Name | Type |
|------|------|
| `tr` | `RedisTransport` |
| `queued` | `Vec<i64>` |
| `nqueued` | `i64` |
| `inbuf` | `Vec<i64>` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 896*

---

## redis_client_new

```fides
pub fn redis_client_new(tr: RedisTransport) -> RedisClient {
```

**Parameters:**

- `tr`: `RedisTransport`

**Returns:** `RedisClient`

*Defined at line 905*

---

## redis_queue

Queue a command (pipelining): nothing is written yet.

```fides
pub fn redis_queue(c: &mut RedisClient, args: Vec<str>) {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `args`: `Vec<str>`

*Defined at line 912*

---

## redis_read_reply

Read one reply (blocking on the transport until it is whole).

```fides
pub fn redis_read_reply(c: &mut RedisClient) -> RespDoc {
```

**Parameters:**

- `c`: `&mut RedisClient`

**Returns:** `RespDoc`

*Defined at line 938*

---

## redis_flush

Write every queued command, then read one reply for each, in order.

```fides
pub fn redis_flush(c: &mut RedisClient) -> Vec<RespDoc> {
```

**Parameters:**

- `c`: `&mut RedisClient`

**Returns:** `Vec<RespDoc>`

*Defined at line 973*

---

## redis_call

Send one command and read its reply.

```fides
pub fn redis_call(c: &mut RedisClient, args: Vec<str>) -> RespDoc {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `args`: `Vec<str>`

**Returns:** `RespDoc`

*Defined at line 989*

---

## redis_get

GET: None when the key does not exist (or on an error).

```fides
pub fn redis_get(c: &mut RedisClient, key: str) -> Option<str> {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`

**Returns:** `Option<str>`

*Defined at line 1014*

---

## redis_set

SET: true on +OK.

```fides
pub fn redis_set(c: &mut RedisClient, key: str, value: str) -> bool {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `value`: `str`

**Returns:** `bool`

*Defined at line 1024*

---

## redis_set_ex

SET with EX and/or NX: false when NX found the key (a null reply).

```fides
pub fn redis_set_ex(c: &mut RedisClient, key: str, value: str, ex_seconds: i64, only_if_absent: bool) -> bool {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `value`: `str`
- `ex_seconds`: `i64`
- `only_if_absent`: `bool`

**Returns:** `bool`

*Defined at line 1031*

---

## redis_incr

INCR: the new value (-1 with c.error set on an error reply).

```fides
pub fn redis_incr(c: &mut RedisClient, key: str) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`

**Returns:** `i64`

*Defined at line 1047*

---

## redis_incrby

```fides
pub fn redis_incrby(c: &mut RedisClient, key: str, by: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `by`: `i64`

**Returns:** `i64`

*Defined at line 1051*

---

## redis_del

DEL: how many keys were removed.

```fides
pub fn redis_del(c: &mut RedisClient, key: str) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`

**Returns:** `i64`

*Defined at line 1056*

---

## redis_hset

HSET key field value: 1 when the field is new, 0 when updated.

```fides
pub fn redis_hset(c: &mut RedisClient, key: str, field: str, value: str) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `field`: `str`
- `value`: `str`

**Returns:** `i64`

*Defined at line 1061*

---

## redis_hget

```fides
pub fn redis_hget(c: &mut RedisClient, key: str, field: str) -> Option<str> {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `field`: `str`

**Returns:** `Option<str>`

*Defined at line 1065*

---

## redis_lpush

LPUSH key v1 v2 ...: the list's length after.

```fides
pub fn redis_lpush(c: &mut RedisClient, key: str, values: Vec<str>) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `values`: `Vec<str>`

**Returns:** `i64`

*Defined at line 1085*

---

## redis_rpush

```fides
pub fn redis_rpush(c: &mut RedisClient, key: str, values: Vec<str>) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `values`: `Vec<str>`

**Returns:** `i64`

*Defined at line 1089*

---

## redis_lrange

LRANGE key start stop: the elements.

```fides
pub fn redis_lrange(c: &mut RedisClient, key: str, start: i64, stop: i64) -> Vec<str> {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `start`: `i64`
- `stop`: `i64`

**Returns:** `Vec<str>`

*Defined at line 1094*

---

## redis_expire

EXPIRE key seconds: true when the timeout was set.

```fides
pub fn redis_expire(c: &mut RedisClient, key: str, seconds: i64) -> bool {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`
- `seconds`: `i64`

**Returns:** `bool`

*Defined at line 1112*

---

## redis_ttl

TTL key: seconds left, -1 no expiry, -2 no key.

```fides
pub fn redis_ttl(c: &mut RedisClient, key: str) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `key`: `str`

**Returns:** `i64`

*Defined at line 1117*

---

## redis_publish

PUBLISH channel message: how many subscribers received it.

```fides
pub fn redis_publish(c: &mut RedisClient, channel: str, message: str) -> i64 {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `channel`: `str`
- `message`: `str`

**Returns:** `i64`

*Defined at line 1127*

---

## redis_subscribe

SUBSCRIBE: writes the command and reads one confirmation per channel.
Afterwards read events with redis_next_event.

```fides
pub fn redis_subscribe(c: &mut RedisClient, channels: Vec<str>) -> Vec<RedisPubSub> {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `channels`: `Vec<str>`

**Returns:** `Vec<RedisPubSub>`

*Defined at line 1133*

---

## redis_next_event

The next pub/sub event (blocking).

```fides
pub fn redis_next_event(c: &mut RedisClient) -> RedisPubSub {
```

**Parameters:**

- `c`: `&mut RedisClient`

**Returns:** `RedisPubSub`

*Defined at line 1162*

---

## redis_hello3

HELLO 3 [AUTH user pass]: switch to RESP3. The server's map reply.

```fides
pub fn redis_hello3(c: &mut RedisClient, user: str, pass: str) -> RespDoc {
```

**Parameters:**

- `c`: `&mut RedisClient`
- `user`: `str`
- `pass`: `str`

**Returns:** `RespDoc`

*Defined at line 1173*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
