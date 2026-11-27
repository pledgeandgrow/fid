[All modules](index.md)

# udp -- Fides Documentation

## Contents

- [UDP_V4:](#UDP_V4:) (const)
- [UDP_V6:](#UDP_V6:) (const)
- [UdpAddr](#UdpAddr) (struct)
- [udp_addr_none](#udp_addr_none) (function)
- [udp_addr_ok](#udp_addr_ok) (function)
- [udp_addr_any](#udp_addr_any) (function)
- [udp_addr_loopback](#udp_addr_loopback) (function)
- [udp_addr](#udp_addr) (function)
- [udp_addr_ip_text](#udp_addr_ip_text) (function)
- [udp_addr_text](#udp_addr_text) (function)
- [udp_addr_same](#udp_addr_same) (function)
- [udp_addr_key](#udp_addr_key) (function)
- [udp_addr_mapped](#udp_addr_mapped) (function)
- [udp_addr_unmapped](#udp_addr_unmapped) (function)
- [udp_open](#udp_open) (function)
- [udp_bind](#udp_bind) (function)
- [udp_listen](#udp_listen) (function)
- [udp_local_addr](#udp_local_addr) (function)
- [udp_local_port](#udp_local_port) (function)
- [udp_send_on](#udp_send_on) (function)
- [udp_send](#udp_send) (function)
- [udp_wait](#udp_wait) (function)
- [udp_recv](#udp_recv) (function)
- [udp_close](#udp_close) (function)

## UDP_V4:

Address families.

```fides
pub const UDP_V4: i64 = 4;
```

*Defined at line 35*

---

## UDP_V6:

```fides
pub const UDP_V6: i64 = 6;
```

*Defined at line 36*

---

## UdpAddr

An address: `family` UDP_V4 or UDP_V6 (0: none or unparsable), `ip`
its 4 or 16 bytes, `port`, and `scope` the IPv6 zone index (0: none).

```fides
pub struct UdpAddr {
```

**Fields:**

| Name | Type |
|------|------|
| `family` | `i64` |
| `ip` | `Vec<i64>` |
| `port` | `i64` |
| `scope` | `i64` |

*Defined at line 40*

---

## udp_addr_none

No address (family 0).

```fides
pub fn udp_addr_none() -> UdpAddr {
```

**Returns:** `UdpAddr`

*Defined at line 498*

---

## udp_addr_ok

Whether an address was parsed (or received).

```fides
pub fn udp_addr_ok(a: &UdpAddr) -> bool {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `bool`

*Defined at line 505*

---

## udp_addr_any

Every address of a family (0.0.0.0 or ::), for binding.

```fides
pub fn udp_addr_any(family: i64, port: i64) -> UdpAddr {
```

**Parameters:**

- `family`: `i64`
- `port`: `i64`

**Returns:** `UdpAddr`

*Defined at line 510*

---

## udp_addr_loopback

The loopback address of a family (127.0.0.1 or ::1).

```fides
pub fn udp_addr_loopback(family: i64, port: i64) -> UdpAddr {
```

**Parameters:**

- `family`: `i64`
- `port`: `i64`

**Returns:** `UdpAddr`

*Defined at line 527*

---

## udp_addr

An address from text: dotted IPv4, or IPv6 (brackets allowed, a zone
index after '%'). Family 0 when it is neither.

```fides
pub fn udp_addr(text: str, port: i64) -> UdpAddr {
```

**Parameters:**

- `text`: `str`
- `port`: `i64`

**Returns:** `UdpAddr`

*Defined at line 686*

---

## udp_addr_ip_text

The address without the port: "192.0.2.1", or IPv6 in its RFC 5952
form ("2001:db8::1", the longest run of zero groups as "::").

```fides
pub fn udp_addr_ip_text(a: &UdpAddr) -> str {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `str`

*Defined at line 760*

---

## udp_addr_text

"192.0.2.1:443" or "[2001:db8::1]:443".

```fides
pub fn udp_addr_text(a: &UdpAddr) -> str {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `str`

*Defined at line 818*

---

## udp_addr_same

Whether two addresses are the same family, address, port and zone.

```fides
pub fn udp_addr_same(a: &UdpAddr, b: &UdpAddr) -> bool {
```

**Parameters:**

- `a`: `&UdpAddr`
- `b`: `&UdpAddr`

**Returns:** `bool`

*Defined at line 826*

---

## udp_addr_key

An address as bytes (family, port, address), to compare, hash or bind
a token to (QUIC address validation).

```fides
pub fn udp_addr_key(a: &UdpAddr) -> Vec<i64> {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `Vec<i64>`

*Defined at line 842*

---

## udp_addr_mapped

An IPv4 address as the IPv4-mapped IPv6 address ::ffff:a.b.c.d (what a
dual-stack IPv6 socket sends to), others unchanged.

```fides
pub fn udp_addr_mapped(a: &UdpAddr) -> UdpAddr {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `UdpAddr`

*Defined at line 857*

---

## udp_addr_unmapped

A v4-mapped IPv6 address as plain IPv4; others unchanged.

```fides
pub fn udp_addr_unmapped(a: &UdpAddr) -> UdpAddr {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `UdpAddr`

*Defined at line 882*

---

## udp_open

A UDP socket of a family (UDP_V4, UDP_V6): its descriptor, -1 on
failure. An IPv6 socket is made dual-stack (IPV6_V6ONLY off), so it
also talks to IPv4 peers, which it sees as v4-mapped addresses.

```fides
pub fn udp_open(family: i64) -> i64 {
```

**Parameters:**

- `family`: `i64`

**Returns:** `i64`

*Defined at line 980*

---

## udp_bind

Bind a socket to a local address (udp_addr_any for every address of
the family; port 0: one the system picks, udp_local_port tells which).
An IPv4 address is bound as v4-mapped on an IPv6 socket.

```fides
pub fn udp_bind(sock: i64, a: &UdpAddr) -> bool {
```

**Parameters:**

- `sock`: `i64`
- `a`: `&UdpAddr`

**Returns:** `bool`

*Defined at line 997*

---

## udp_listen

A socket of the address's family bound to it: -1 on failure.

```fides
pub fn udp_listen(a: &UdpAddr) -> i64 {
```

**Parameters:**

- `a`: `&UdpAddr`

**Returns:** `i64`

*Defined at line 1016*

---

## udp_local_addr

The address a socket is bound to (family 0 when unknown).

```fides
pub fn udp_local_addr(sock: i64) -> UdpAddr {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `UdpAddr`

*Defined at line 1029*

---

## udp_local_port

The port a socket is bound to, -1 when unknown.

```fides
pub fn udp_local_port(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 1053*

---

## udp_send_on

Send one datagram: the number of bytes sent, -1 on error. `to` is
written in the socket's family: pass `sock_family` UDP_V6 for an IPv6
socket and an IPv4 `to` is sent v4-mapped.

```fides
pub fn udp_send_on(sock: i64, sock_family: i64, data: Vec<i64>, to: &UdpAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `sock_family`: `i64`
- `data`: `Vec<i64>`
- `to`: `&UdpAddr`

**Returns:** `i64`

*Defined at line 1064*

---

## udp_send

Send one datagram on a socket of `to`'s own family.

```fides
pub fn udp_send(sock: i64, data: Vec<i64>, to: &UdpAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `Vec<i64>`
- `to`: `&UdpAddr`

**Returns:** `i64`

*Defined at line 1106*

---

## udp_wait

Whether a datagram is waiting, within `timeout_ms` (0: just look).

```fides
pub fn udp_wait(sock: i64, timeout_ms: i64) -> bool {
```

**Parameters:**

- `sock`: `i64`
- `timeout_ms`: `i64`

**Returns:** `bool`

*Defined at line 1111*

---

## udp_recv

Receive one datagram of up to `max` bytes, waiting up to `timeout_ms`:
its bytes ([] on a timeout or an error); the sender goes to `from`
(v4-mapped senders of a dual-stack socket as IPv4).

```fides
pub fn udp_recv(sock: i64, max: i64, timeout_ms: i64, from: &mut UdpAddr) -> Vec<i64> {
```

**Parameters:**

- `sock`: `i64`
- `max`: `i64`
- `timeout_ms`: `i64`
- `from`: `&mut UdpAddr`

**Returns:** `Vec<i64>`

*Defined at line 1125*

---

## udp_close

Close a socket.

```fides
pub fn udp_close(sock: i64) {
```

**Parameters:**

- `sock`: `i64`

*Defined at line 1174*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
