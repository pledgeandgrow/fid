[All modules](index.md)

# mqtt -- Fides Documentation

## Contents

- [MQTT_CONNECT:](#MQTT_CONNECT:) (const)
- [MQTT_CONNACK:](#MQTT_CONNACK:) (const)
- [MQTT_PUBLISH:](#MQTT_PUBLISH:) (const)
- [MQTT_PUBACK:](#MQTT_PUBACK:) (const)
- [MQTT_PUBREC:](#MQTT_PUBREC:) (const)
- [MQTT_PUBREL:](#MQTT_PUBREL:) (const)
- [MQTT_PUBCOMP:](#MQTT_PUBCOMP:) (const)
- [MQTT_SUBSCRIBE:](#MQTT_SUBSCRIBE:) (const)
- [MQTT_SUBACK:](#MQTT_SUBACK:) (const)
- [MQTT_UNSUBSCRIBE:](#MQTT_UNSUBSCRIBE:) (const)
- [MQTT_UNSUBACK:](#MQTT_UNSUBACK:) (const)
- [MQTT_PINGREQ:](#MQTT_PINGREQ:) (const)
- [MQTT_PINGRESP:](#MQTT_PINGRESP:) (const)
- [MQTT_DISCONNECT:](#MQTT_DISCONNECT:) (const)
- [MQTT_AUTH:](#MQTT_AUTH:) (const)
- [MQTT_V311:](#MQTT_V311:) (const)
- [MQTT_V5:](#MQTT_V5:) (const)
- [MQTT_P_PAYLOAD_FORMAT:](#MQTT_P_PAYLOAD_FORMAT:) (const)
- [MQTT_P_MESSAGE_EXPIRY:](#MQTT_P_MESSAGE_EXPIRY:) (const)
- [MQTT_P_CONTENT_TYPE:](#MQTT_P_CONTENT_TYPE:) (const)
- [MQTT_P_RESPONSE_TOPIC:](#MQTT_P_RESPONSE_TOPIC:) (const)
- [MQTT_P_CORRELATION_DATA:](#MQTT_P_CORRELATION_DATA:) (const)
- [MQTT_P_SUBSCRIPTION_ID:](#MQTT_P_SUBSCRIPTION_ID:) (const)
- [MQTT_P_SESSION_EXPIRY:](#MQTT_P_SESSION_EXPIRY:) (const)
- [MQTT_P_ASSIGNED_CLIENT_ID:](#MQTT_P_ASSIGNED_CLIENT_ID:) (const)
- [MQTT_P_SERVER_KEEP_ALIVE:](#MQTT_P_SERVER_KEEP_ALIVE:) (const)
- [MQTT_P_AUTH_METHOD:](#MQTT_P_AUTH_METHOD:) (const)
- [MQTT_P_AUTH_DATA:](#MQTT_P_AUTH_DATA:) (const)
- [MQTT_P_REASON_STRING:](#MQTT_P_REASON_STRING:) (const)
- [MQTT_P_RECEIVE_MAXIMUM:](#MQTT_P_RECEIVE_MAXIMUM:) (const)
- [MQTT_P_TOPIC_ALIAS_MAXIMUM:](#MQTT_P_TOPIC_ALIAS_MAXIMUM:) (const)
- [MQTT_P_TOPIC_ALIAS:](#MQTT_P_TOPIC_ALIAS:) (const)
- [MQTT_P_MAXIMUM_QOS:](#MQTT_P_MAXIMUM_QOS:) (const)
- [MQTT_P_USER_PROPERTY:](#MQTT_P_USER_PROPERTY:) (const)
- [MQTT_P_MAXIMUM_PACKET_SIZE:](#MQTT_P_MAXIMUM_PACKET_SIZE:) (const)
- [MQTT_ST_DISCONNECTED:](#MQTT_ST_DISCONNECTED:) (const)
- [MQTT_ST_CONNECTING:](#MQTT_ST_CONNECTING:) (const)
- [MQTT_ST_CONNECTED:](#MQTT_ST_CONNECTED:) (const)
- [MqttProps](#MqttProps) (struct)
- [MqttPacket](#MqttPacket) (struct)
- [mqtt_props_new](#mqtt_props_new) (function)
- [mqtt_packet](#mqtt_packet) (function)
- [mqtt_prop_kind](#mqtt_prop_kind) (function)
- [mqtt_prop_int](#mqtt_prop_int) (function)
- [mqtt_prop_str](#mqtt_prop_str) (function)
- [mqtt_prop_bin](#mqtt_prop_bin) (function)
- [mqtt_prop_user](#mqtt_prop_user) (function)
- [mqtt_prop_find](#mqtt_prop_find) (function)
- [mqtt_prop_get_int](#mqtt_prop_get_int) (function)
- [mqtt_prop_get_str](#mqtt_prop_get_str) (function)
- [mqtt_put_varint](#mqtt_put_varint) (function)
- [mqtt_encode](#mqtt_encode) (function)
- [mqtt_frame_len](#mqtt_frame_len) (function)
- [MqttDecoded](#MqttDecoded) (struct)
- [mqtt_decode](#mqtt_decode) (function)
- [mqtt_topic_valid](#mqtt_topic_valid) (function)
- [mqtt_filter_valid](#mqtt_filter_valid) (function)
- [mqtt_topic_matches](#mqtt_topic_matches) (function)
- [MqttClient](#MqttClient) (struct)
- [mqtt_client_new](#mqtt_client_new) (function)
- [mqtt_client_alloc_id](#mqtt_client_alloc_id) (function)
- [mqtt_client_connect](#mqtt_client_connect) (function)
- [mqtt_client_inflight](#mqtt_client_inflight) (function)
- [mqtt_client_publish](#mqtt_client_publish) (function)
- [mqtt_client_subscribe](#mqtt_client_subscribe) (function)
- [mqtt_client_unsubscribe](#mqtt_client_unsubscribe) (function)
- [mqtt_client_disconnect](#mqtt_client_disconnect) (function)
- [mqtt_client_lost](#mqtt_client_lost) (function)
- [mqtt_client_take_outbox](#mqtt_client_take_outbox) (function)
- [mqtt_client_take_messages](#mqtt_client_take_messages) (function)
- [mqtt_client_feed](#mqtt_client_feed) (function)
- [mqtt_client_tick](#mqtt_client_tick) (function)

## MQTT_CONNECT:

```fides
pub const MQTT_CONNECT: i64 = 1;
```

*Defined at line 38*

---

## MQTT_CONNACK:

```fides
pub const MQTT_CONNACK: i64 = 2;
```

*Defined at line 39*

---

## MQTT_PUBLISH:

```fides
pub const MQTT_PUBLISH: i64 = 3;
```

*Defined at line 40*

---

## MQTT_PUBACK:

```fides
pub const MQTT_PUBACK: i64 = 4;
```

*Defined at line 41*

---

## MQTT_PUBREC:

```fides
pub const MQTT_PUBREC: i64 = 5;
```

*Defined at line 42*

---

## MQTT_PUBREL:

```fides
pub const MQTT_PUBREL: i64 = 6;
```

*Defined at line 43*

---

## MQTT_PUBCOMP:

```fides
pub const MQTT_PUBCOMP: i64 = 7;
```

*Defined at line 44*

---

## MQTT_SUBSCRIBE:

```fides
pub const MQTT_SUBSCRIBE: i64 = 8;
```

*Defined at line 45*

---

## MQTT_SUBACK:

```fides
pub const MQTT_SUBACK: i64 = 9;
```

*Defined at line 46*

---

## MQTT_UNSUBSCRIBE:

```fides
pub const MQTT_UNSUBSCRIBE: i64 = 10;
```

*Defined at line 47*

---

## MQTT_UNSUBACK:

```fides
pub const MQTT_UNSUBACK: i64 = 11;
```

*Defined at line 48*

---

## MQTT_PINGREQ:

```fides
pub const MQTT_PINGREQ: i64 = 12;
```

*Defined at line 49*

---

## MQTT_PINGRESP:

```fides
pub const MQTT_PINGRESP: i64 = 13;
```

*Defined at line 50*

---

## MQTT_DISCONNECT:

```fides
pub const MQTT_DISCONNECT: i64 = 14;
```

*Defined at line 51*

---

## MQTT_AUTH:

```fides
pub const MQTT_AUTH: i64 = 15;
```

*Defined at line 52*

---

## MQTT_V311:

```fides
pub const MQTT_V311: i64 = 4;
```

*Defined at line 55*

---

## MQTT_V5:

```fides
pub const MQTT_V5: i64 = 5;
```

*Defined at line 56*

---

## MQTT_P_PAYLOAD_FORMAT:

```fides
pub const MQTT_P_PAYLOAD_FORMAT: i64 = 0x01;
```

*Defined at line 59*

---

## MQTT_P_MESSAGE_EXPIRY:

```fides
pub const MQTT_P_MESSAGE_EXPIRY: i64 = 0x02;
```

*Defined at line 60*

---

## MQTT_P_CONTENT_TYPE:

```fides
pub const MQTT_P_CONTENT_TYPE: i64 = 0x03;
```

*Defined at line 61*

---

## MQTT_P_RESPONSE_TOPIC:

```fides
pub const MQTT_P_RESPONSE_TOPIC: i64 = 0x08;
```

*Defined at line 62*

---

## MQTT_P_CORRELATION_DATA:

```fides
pub const MQTT_P_CORRELATION_DATA: i64 = 0x09;
```

*Defined at line 63*

---

## MQTT_P_SUBSCRIPTION_ID:

```fides
pub const MQTT_P_SUBSCRIPTION_ID: i64 = 0x0B;
```

*Defined at line 64*

---

## MQTT_P_SESSION_EXPIRY:

```fides
pub const MQTT_P_SESSION_EXPIRY: i64 = 0x11;
```

*Defined at line 65*

---

## MQTT_P_ASSIGNED_CLIENT_ID:

```fides
pub const MQTT_P_ASSIGNED_CLIENT_ID: i64 = 0x12;
```

*Defined at line 66*

---

## MQTT_P_SERVER_KEEP_ALIVE:

```fides
pub const MQTT_P_SERVER_KEEP_ALIVE: i64 = 0x13;
```

*Defined at line 67*

---

## MQTT_P_AUTH_METHOD:

```fides
pub const MQTT_P_AUTH_METHOD: i64 = 0x15;
```

*Defined at line 68*

---

## MQTT_P_AUTH_DATA:

```fides
pub const MQTT_P_AUTH_DATA: i64 = 0x16;
```

*Defined at line 69*

---

## MQTT_P_REASON_STRING:

```fides
pub const MQTT_P_REASON_STRING: i64 = 0x1F;
```

*Defined at line 70*

---

## MQTT_P_RECEIVE_MAXIMUM:

```fides
pub const MQTT_P_RECEIVE_MAXIMUM: i64 = 0x21;
```

*Defined at line 71*

---

## MQTT_P_TOPIC_ALIAS_MAXIMUM:

```fides
pub const MQTT_P_TOPIC_ALIAS_MAXIMUM: i64 = 0x22;
```

*Defined at line 72*

---

## MQTT_P_TOPIC_ALIAS:

```fides
pub const MQTT_P_TOPIC_ALIAS: i64 = 0x23;
```

*Defined at line 73*

---

## MQTT_P_MAXIMUM_QOS:

```fides
pub const MQTT_P_MAXIMUM_QOS: i64 = 0x24;
```

*Defined at line 74*

---

## MQTT_P_USER_PROPERTY:

```fides
pub const MQTT_P_USER_PROPERTY: i64 = 0x26;
```

*Defined at line 75*

---

## MQTT_P_MAXIMUM_PACKET_SIZE:

```fides
pub const MQTT_P_MAXIMUM_PACKET_SIZE: i64 = 0x27;
```

*Defined at line 76*

---

## MQTT_ST_DISCONNECTED:

```fides
pub const MQTT_ST_DISCONNECTED: i64 = 0;
```

*Defined at line 79*

---

## MQTT_ST_CONNECTING:

```fides
pub const MQTT_ST_CONNECTING: i64 = 1;
```

*Defined at line 80*

---

## MQTT_ST_CONNECTED:

```fides
pub const MQTT_ST_CONNECTED: i64 = 2;
```

*Defined at line 81*

---

## MqttProps

v5 properties of one packet, in order. Each entry: its identifier and
one of an integer (byte, two/four-byte, varint), a string (and for a
user property its value in `strs2`) or binary data.

```fides
pub struct MqttProps {
```

**Fields:**

| Name | Type |
|------|------|
| `ids` | `Vec<i64>` |
| `ints` | `Vec<i64>` |
| `strs` | `Vec<str>` |
| `strs2` | `Vec<str>` |
| `bins` | `Vec<Vec<i64>>` |

*Defined at line 86*

---

## MqttPacket

One control packet. Fields not used by `ptype` are left at defaults.

```fides
pub struct MqttPacket {
```

**Fields:**

| Name | Type |
|------|------|
| `ptype` | `i64` |
| `version` | `i64` |
| `client_id` | `str` |
| `clean` | `bool` |
| `keep_alive` | `i64` |
| `has_username` | `bool` |
| `username` | `str` |
| `has_password` | `bool` |
| `password` | `Vec<i64>` |
| `has_will` | `bool` |
| `will_topic` | `str` |
| `will_payload` | `Vec<i64>` |
| `will_qos` | `i64` |
| `will_retain` | `bool` |
| `will_props` | `MqttProps` |
| `session_present` | `bool` |
| `reason` | `i64` |
| `topic` | `str` |
| `payload` | `Vec<i64>` |
| `qos` | `i64` |
| `retain` | `bool` |
| `dup` | `bool` |
| `packet_id` | `i64` |
| `filters` | `Vec<str>` |
| `options` | `Vec<i64>` |
| `codes` | `Vec<i64>` |
| `props` | `MqttProps` |

*Defined at line 95*

---

## mqtt_props_new

```fides
pub fn mqtt_props_new() -> MqttProps {
```

**Returns:** `MqttProps`

*Defined at line 133*

---

## mqtt_packet

```fides
pub fn mqtt_packet(ptype: i64, version: i64) -> MqttPacket {
```

**Parameters:**

- `ptype`: `i64`
- `version`: `i64`

**Returns:** `MqttPacket`

*Defined at line 139*

---

## mqtt_prop_kind

How a property's value is encoded: 1 byte, 2 two-byte integer, 4
four-byte integer, 5 variable byte integer, 6 UTF-8 string, 7 binary,
8 string pair; 0 for an unknown identifier.

```fides
pub fn mqtt_prop_kind(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 150*

---

## mqtt_prop_int

Add an integer property (byte, two-byte, four-byte or varint kind).

```fides
pub fn mqtt_prop_int(p: &mut MqttProps, id: i64, n: i64) {
```

**Parameters:**

- `p`: `&mut MqttProps`
- `id`: `i64`
- `n`: `i64`

*Defined at line 184*

---

## mqtt_prop_str

Add a string property (content type, response topic, reason string ...).

```fides
pub fn mqtt_prop_str(p: &mut MqttProps, id: i64, s: str) {
```

**Parameters:**

- `p`: `&mut MqttProps`
- `id`: `i64`
- `s`: `str`

*Defined at line 189*

---

## mqtt_prop_bin

Add a binary property (correlation data, authentication data).

```fides
pub fn mqtt_prop_bin(p: &mut MqttProps, id: i64, b: Vec<i64>) {
```

**Parameters:**

- `p`: `&mut MqttProps`
- `id`: `i64`
- `b`: `Vec<i64>`

*Defined at line 194*

---

## mqtt_prop_user

Add a user property (name, value).

```fides
pub fn mqtt_prop_user(p: &mut MqttProps, name: str, value: str) {
```

**Parameters:**

- `p`: `&mut MqttProps`
- `name`: `str`
- `value`: `str`

*Defined at line 199*

---

## mqtt_prop_find

Index of the first property `id`, or -1.

```fides
pub fn mqtt_prop_find(p: &MqttProps, id: i64) -> i64 {
```

**Parameters:**

- `p`: `&MqttProps`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 204*

---

## mqtt_prop_get_int

An integer property's value, or `def` when it is absent.

```fides
pub fn mqtt_prop_get_int(p: &MqttProps, id: i64, def: i64) -> i64 {
```

**Parameters:**

- `p`: `&MqttProps`
- `id`: `i64`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 216*

---

## mqtt_prop_get_str

```fides
pub fn mqtt_prop_get_str(p: &MqttProps, id: i64) -> str {
```

**Parameters:**

- `p`: `&MqttProps`
- `id`: `i64`

**Returns:** `str`

*Defined at line 224*

---

## mqtt_put_varint

Append a variable byte integer (0 .. 268435455).

```fides
pub fn mqtt_put_varint(out: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 235*

---

## mqtt_encode

Encode a packet (fixed header, variable header, payload).

```fides
pub fn mqtt_encode(p: &MqttPacket) -> Vec<i64> {
```

**Parameters:**

- `p`: `&MqttPacket`

**Returns:** `Vec<i64>`

*Defined at line 331*

---

## mqtt_frame_len

The length of the first complete packet in `buf`: 0 while more bytes
are needed, -1 when the remaining length is malformed.

```fides
pub fn mqtt_frame_len(buf: &Vec<i64>) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 444*

---

## MqttDecoded

A decoded packet. `ok` false: `error` says why.

```fides
pub struct MqttDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `packet` | `MqttPacket` |

*Defined at line 467*

---

## mqtt_decode

Decode one whole packet (as mqtt_frame_len delimits it). `version` is
the protocol level of the connection (a CONNECT carries its own).

```fides
pub fn mqtt_decode(bytes: Vec<i64>, version: i64) -> MqttDecoded {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `version`: `i64`

**Returns:** `MqttDecoded`

*Defined at line 588*

---

## mqtt_topic_valid

A topic name to publish to: not empty, no wildcard, no NUL.

```fides
pub fn mqtt_topic_valid(topic: str) -> bool {
```

**Parameters:**

- `topic`: `str`

**Returns:** `bool`

*Defined at line 758*

---

## mqtt_filter_valid

A topic filter: '+' fills a whole level, '#' is the whole last level.

```fides
pub fn mqtt_filter_valid(filter: str) -> bool {
```

**Parameters:**

- `filter`: `str`

**Returns:** `bool`

*Defined at line 775*

---

## mqtt_topic_matches

Whether topic name `topic` matches filter `filter`.

```fides
pub fn mqtt_topic_matches(filter: str, topic: str) -> bool {
```

**Parameters:**

- `filter`: `str`
- `topic`: `str`

**Returns:** `bool`

*Defined at line 803*

---

## MqttClient

A client of one broker connection. Feed it received bytes, take the
bytes it wants sent; see the module header.

```fides
pub struct MqttClient {
```

**Fields:**

| Name | Type |
|------|------|
| `version` | `i64` |
| `client_id` | `str` |
| `keep_alive` | `i64,  // seconds, 0: off` |
| `state` | `i64,  // MQTT_ST_*` |
| `error` | `str,  // why the connection was given up` |
| `connack_reason` | `i64` |
| `session_present` | `bool` |
| `receive_max` | `i64,  // the server's Receive Maximum (v5), else 65535` |
| `next_id` | `i64` |
| `outbox` | `Vec<Vec<i64>>` |
| `inbuf` | `Vec<i64>` |
| `messages` | `Vec<MqttPacket>` |
| `out_ids` | `Vec<i64>` |
| `out_wait` | `Vec<i64>` |
| `out_pkts` | `Vec<MqttPacket>` |
| `in_ids` | `Vec<i64>` |
| `ack_ids` | `Vec<i64>` |
| `ack_types` | `Vec<i64>` |
| `completed` | `Vec<i64>` |
| `suback_ids` | `Vec<i64>` |
| `suback_codes` | `Vec<Vec<i64>>` |
| `last_send` | `i64` |
| `last_recv` | `i64` |
| `ping_sent` | `i64,  // time of an unanswered PINGREQ, -1: none` |

*Defined at line 837*

---

## mqtt_client_new

```fides
pub fn mqtt_client_new(client_id: str, version: i64, keep_alive: i64) -> MqttClient {
```

**Parameters:**

- `client_id`: `str`
- `version`: `i64`
- `keep_alive`: `i64`

**Returns:** `MqttClient`

*Defined at line 869*

---

## mqtt_client_alloc_id

A free packet identifier (1..65535), or 0 when all are in use.

```fides
pub fn mqtt_client_alloc_id(c: &mut MqttClient) -> i64 {
```

**Parameters:**

- `c`: `&mut MqttClient`

**Returns:** `i64`

*Defined at line 885*

---

## mqtt_client_connect

Queue CONNECT. `clean` false asks the server to keep the session.

```fides
pub fn mqtt_client_connect(c: &mut MqttClient, clean: bool, now: i64) {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `clean`: `bool`
- `now`: `i64`

*Defined at line 902*

---

## mqtt_client_inflight

Inflight outgoing QoS 1/2 messages.

```fides
pub fn mqtt_client_inflight(c: &MqttClient) -> i64 {
```

**Parameters:**

- `c`: `&MqttClient`

**Returns:** `i64`

*Defined at line 922*

---

## mqtt_client_publish

Queue PUBLISH. The packet id (0 for QoS 0); -1 when not connected, the
topic is invalid, or the server's Receive Maximum is reached.

```fides
pub fn mqtt_client_publish(c: &mut MqttClient, topic: str, payload: Vec<i64>, qos: i64, retain: bool, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `topic`: `str`
- `payload`: `Vec<i64>`
- `qos`: `i64`
- `retain`: `bool`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 928*

---

## mqtt_client_subscribe

Queue SUBSCRIBE for `filters` with options (QoS) `qos`; the packet id or -1.

```fides
pub fn mqtt_client_subscribe(c: &mut MqttClient, filters: Vec<str>, qos: Vec<i64>, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `filters`: `Vec<str>`
- `qos`: `Vec<i64>`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 957*

---

## mqtt_client_unsubscribe

```fides
pub fn mqtt_client_unsubscribe(c: &mut MqttClient, filters: Vec<str>, now: i64) -> i64 {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `filters`: `Vec<str>`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 982*

---

## mqtt_client_disconnect

Queue DISCONNECT; the client is disconnected afterwards.

```fides
pub fn mqtt_client_disconnect(c: &mut MqttClient, now: i64) {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `now`: `i64`

*Defined at line 1000*

---

## mqtt_client_lost

The connection dropped (the transport reports it).

```fides
pub fn mqtt_client_lost(c: &mut MqttClient, why: str) {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `why`: `str`

*Defined at line 1009*

---

## mqtt_client_take_outbox

Bytes the client wants written, in order (the outbox empties).

```fides
pub fn mqtt_client_take_outbox(c: &mut MqttClient) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut MqttClient`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 1015*

---

## mqtt_client_take_messages

Application messages received (PUBLISH packets), in order.

```fides
pub fn mqtt_client_take_messages(c: &mut MqttClient) -> Vec<MqttPacket> {
```

**Parameters:**

- `c`: `&mut MqttClient`

**Returns:** `Vec<MqttPacket>`

*Defined at line 1022*

---

## mqtt_client_feed

Give the client received bytes (any amount; partial packets wait for
the rest). False when the connection must be closed (`error` says why).

```fides
pub fn mqtt_client_feed(c: &mut MqttClient, bytes: Vec<i64>, now: i64) -> bool {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `bytes`: `Vec<i64>`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 1053*

---

## mqtt_client_tick

Let time pass: sends PINGREQ when nothing was sent for keep_alive
seconds, gives the connection up when a PINGREQ stays unanswered for
keep_alive seconds. False when it was given up.

```fides
pub fn mqtt_client_tick(c: &mut MqttClient, now: i64) -> bool {
```

**Parameters:**

- `c`: `&mut MqttClient`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 1223*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
