# net — Fides Documentation

## Contents

- [AF_INET:](#af_inet:) (const)
- [SOCK_DGRAM:](#sock_dgram:) (const)
- [SOCK_STREAM:](#sock_stream:) (const)
- [SOL_SOCKET:](#sol_socket:) (const)
- [SO_REUSEADDR:](#so_reuseaddr:) (const)
- [SO_BROADCAST:](#so_broadcast:) (const)
- [SO_RCVTIMEO:](#so_rcvtimeo:) (const)
- [MSG_DONTWAIT:](#msg_dontwait:) (const)
- [NetAddr](#netaddr) (struct)
- [net_addr_create](#net_addr_create) (function)
- [net_addr_any](#net_addr_any) (function)
- [net_socket_create](#net_socket_create) (function)
- [net_socket_create_tcp](#net_socket_create_tcp) (function)
- [net_bind](#net_bind) (function)
- [net_set_nonblocking](#net_set_nonblocking) (function)
- [net_set_recv_timeout](#net_set_recv_timeout) (function)
- [net_set_reuseaddr](#net_set_reuseaddr) (function)
- [net_close](#net_close) (function)
- [net_send_to](#net_send_to) (function)
- [net_recv_from](#net_recv_from) (function)
- [net_connect](#net_connect) (function)
- [net_send](#net_send) (function)
- [net_recv](#net_recv) (function)
- [NetBuffer](#netbuffer) (struct)
- [net_buffer_create](#net_buffer_create) (function)
- [net_buffer_write_u8](#net_buffer_write_u8) (function)
- [net_buffer_write_u16](#net_buffer_write_u16) (function)
- [net_buffer_write_u32](#net_buffer_write_u32) (function)
- [net_buffer_read_u8](#net_buffer_read_u8) (function)
- [net_buffer_read_u16](#net_buffer_read_u16) (function)
- [net_buffer_read_u32](#net_buffer_read_u32) (function)
- [net_buffer_reset](#net_buffer_reset) (function)
- [net_buffer_remaining](#net_buffer_remaining) (function)
- [MSG_TYPE_DATA:](#msg_type_data:) (const)
- [MSG_TYPE_ACK:](#msg_type_ack:) (const)
- [MSG_TYPE_PING:](#msg_type_ping:) (const)
- [MSG_TYPE_PONG:](#msg_type_pong:) (const)
- [ReliableMessage](#reliablemessage) (struct)
- [ReliableConnection](#reliableconnection) (struct)
- [net_reliable_create](#net_reliable_create) (function)
- [net_reliable_send](#net_reliable_send) (function)
- [net_reliable_recv](#net_reliable_recv) (function)
- [net_reliable_update](#net_reliable_update) (function)
- [net_reliable_pending_count](#net_reliable_pending_count) (function)
- [net_reliable_rtt](#net_reliable_rtt) (function)

## AF_INET:

```fides
pub const AF_INET: i64 = 2;
```

*Defined at line 49*

---

## SOCK_DGRAM:

```fides
pub const SOCK_DGRAM: i64 = 2;
```

*Defined at line 50*

---

## SOCK_STREAM:

```fides
pub const SOCK_STREAM: i64 = 1;
```

*Defined at line 51*

---

## SOL_SOCKET:

```fides
pub const SOL_SOCKET: i64 = 1;
```

*Defined at line 52*

---

## SO_REUSEADDR:

```fides
pub const SO_REUSEADDR: i64 = 2;
```

*Defined at line 53*

---

## SO_BROADCAST:

```fides
pub const SO_BROADCAST: i64 = 6;
```

*Defined at line 54*

---

## SO_RCVTIMEO:

```fides
pub const SO_RCVTIMEO: i64 = 20;
```

*Defined at line 55*

---

## MSG_DONTWAIT:

```fides
pub const MSG_DONTWAIT: i64 = 0x40;
```

*Defined at line 57*

---

## NetAddr

```fides
pub struct NetAddr {
```

**Fields:**

| Name | Type |
|------|------|
| `ip` | `i64,        // IPv4 address as i64 (from inet_addr)` |
| `port` | `i64,      // port number` |
| `raw` | `i64,       // raw sockaddr_in pointer (native)` |

*Defined at line 61*

---

## net_addr_create

Create a network address from IP string and port.

```fides
pub fn net_addr_create(ip_str: str, port: i64) -> NetAddr {
```

**Parameters:**

- `ip_str`: `str`
- `port`: `i64`

**Returns:** `NetAddr`

*Defined at line 68*

---

## net_addr_any

Create a "any" address (0.0.0.0) for binding.

```fides
pub fn net_addr_any(port: i64) -> NetAddr {
```

**Parameters:**

- `port`: `i64`

**Returns:** `NetAddr`

*Defined at line 77*

---

## net_socket_create

Create a UDP socket. Returns socket file descriptor or -1 on error.

```fides
pub fn net_socket_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 84*

---

## net_socket_create_tcp

Create a TCP socket. Returns socket file descriptor or -1 on error.

```fides
pub fn net_socket_create_tcp() -> i64 {
```

**Returns:** `i64`

*Defined at line 89*

---

## net_bind

Bind a socket to a port (for server use).
Returns 0 on success, -1 on error.

```fides
pub fn net_bind(sock: i64, port: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 95*

---

## net_set_nonblocking

Set socket to non-blocking mode.

```fides
pub fn net_set_nonblocking(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 103*

---

## net_set_recv_timeout

Set receive timeout in milliseconds.

```fides
pub fn net_set_recv_timeout(sock: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 108*

---

## net_set_reuseaddr

Enable address reuse (SO_REUSEADDR).

```fides
pub fn net_set_reuseaddr(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 113*

---

## net_close

Close a socket.

```fides
pub fn net_close(sock: i64) -> void {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `void`

*Defined at line 119*

---

## net_send_to

Send data to a specific address (UDP).
Returns number of bytes sent, or -1 on error.

```fides
pub fn net_send_to(sock: i64, data: i64, len: i64, addr: &NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`
- `addr`: `&NetAddr`

**Returns:** `i64`

*Defined at line 127*

---

## net_recv_from

Receive data from any sender (UDP).
Returns number of bytes received, or -1 on error (or timeout).
The sender's address is stored in from_addr.

```fides
pub fn net_recv_from(sock: i64, buf: i64, len: i64, from_addr: &mut NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`
- `from_addr`: `&mut NetAddr`

**Returns:** `i64`

*Defined at line 134*

---

## net_connect

Connect to a server (TCP).

```fides
pub fn net_connect(sock: i64, addr: &NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `addr`: `&NetAddr`

**Returns:** `i64`

*Defined at line 139*

---

## net_send

Send data on a connected socket (TCP or connected UDP).

```fides
pub fn net_send(sock: i64, data: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## net_recv

Receive data on a connected socket.

```fides
pub fn net_recv(sock: i64, buf: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 149*

---

## NetBuffer

```fides
pub struct NetBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `size` | `i64` |
| `pos` | `i64` |

*Defined at line 155*

---

## net_buffer_create

```fides
pub fn net_buffer_create(size: i64) -> NetBuffer {
```

**Parameters:**

- `size`: `i64`

**Returns:** `NetBuffer`

*Defined at line 161*

---

## net_buffer_write_u8

```fides
pub fn net_buffer_write_u8(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 175*

---

## net_buffer_write_u16

```fides
pub fn net_buffer_write_u16(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 182*

---

## net_buffer_write_u32

```fides
pub fn net_buffer_write_u32(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 187*

---

## net_buffer_read_u8

```fides
pub fn net_buffer_read_u8(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 194*

---

## net_buffer_read_u16

```fides
pub fn net_buffer_read_u16(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 201*

---

## net_buffer_read_u32

```fides
pub fn net_buffer_read_u32(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 207*

---

## net_buffer_reset

```fides
pub fn net_buffer_reset(buf: &mut NetBuffer) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `void`

*Defined at line 215*

---

## net_buffer_remaining

```fides
pub fn net_buffer_remaining(buf: &NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&NetBuffer`

**Returns:** `i64`

*Defined at line 219*

---

## MSG_TYPE_DATA:

```fides
pub const MSG_TYPE_DATA: i64 = 0;
```

*Defined at line 235*

---

## MSG_TYPE_ACK:

```fides
pub const MSG_TYPE_ACK: i64 = 1;
```

*Defined at line 236*

---

## MSG_TYPE_PING:

```fides
pub const MSG_TYPE_PING: i64 = 2;
```

*Defined at line 237*

---

## MSG_TYPE_PONG:

```fides
pub const MSG_TYPE_PONG: i64 = 3;
```

*Defined at line 238*

---

## ReliableMessage

```fides
pub struct ReliableMessage {
```

**Fields:**

| Name | Type |
|------|------|
| `msg_id` | `i64` |
| `msg_type` | `i64` |
| `data` | `Vec<i64>` |
| `sent_time` | `i64` |
| `acked` | `bool` |
| `send_count` | `i64` |

*Defined at line 240*

---

## ReliableConnection

```fides
pub struct ReliableConnection {
```

**Fields:**

| Name | Type |
|------|------|
| `sock` | `i64` |
| `remote` | `NetAddr` |
| `next_msg_id` | `i64` |
| `pending` | `Vec<ReliableMessage>` |
| `received_ids` | `Vec<i64>` |
| `rtt` | `f64,              // round-trip time estimate (ms)` |
| `timeout_ms` | `i64,       // retransmission timeout` |

*Defined at line 249*

---

## net_reliable_create

```fides
pub fn net_reliable_create(sock: i64, remote: NetAddr) -> ReliableConnection {
```

**Parameters:**

- `sock`: `i64`
- `remote`: `NetAddr`

**Returns:** `ReliableConnection`

*Defined at line 259*

---

## net_reliable_send

Send a reliable message (will be retransmitted until acked).

```fides
pub fn net_reliable_send(conn: &mut ReliableConnection, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 272*

---

## net_reliable_recv

Process incoming data (check for ACKs or data messages).

```fides
pub fn net_reliable_recv(conn: &mut ReliableConnection, data: Vec<i64>) -> void {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `data`: `Vec<i64>`

**Returns:** `void`

*Defined at line 297*

---

## net_reliable_update

Update the connection: retransmit unacked messages that have timed out.

```fides
pub fn net_reliable_update(conn: &mut ReliableConnection, current_time: i64) -> void {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `current_time`: `i64`

**Returns:** `void`

*Defined at line 347*

---

## net_reliable_pending_count

Get the number of pending (unacked) messages.

```fides
pub fn net_reliable_pending_count(conn: &ReliableConnection) -> i64 {
```

**Parameters:**

- `conn`: `&ReliableConnection`

**Returns:** `i64`

*Defined at line 389*

---

## net_reliable_rtt

Get the estimated round-trip time in milliseconds.

```fides
pub fn net_reliable_rtt(conn: &ReliableConnection) -> f64 {
```

**Parameters:**

- `conn`: `&ReliableConnection`

**Returns:** `f64`

*Defined at line 394*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
