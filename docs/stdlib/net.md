[All modules](index.md)

# net -- Fides Documentation

## Contents

- [AF_INET:](#AF_INET:) (const)
- [SOCK_DGRAM:](#SOCK_DGRAM:) (const)
- [SOCK_STREAM:](#SOCK_STREAM:) (const)
- [SOL_SOCKET:](#SOL_SOCKET:) (const)
- [SO_REUSEADDR:](#SO_REUSEADDR:) (const)
- [SO_BROADCAST:](#SO_BROADCAST:) (const)
- [SO_RCVTIMEO:](#SO_RCVTIMEO:) (const)
- [MSG_DONTWAIT:](#MSG_DONTWAIT:) (const)
- [NetAddr](#NetAddr) (struct)
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
- [net_send_file](#net_send_file) (function)
- [p_net_send_file](#p_net_send_file) (function)
- [net_send_bytes](#net_send_bytes) (function)
- [net_recv_bytes](#net_recv_bytes) (function)
- [net_send_bytes_to](#net_send_bytes_to) (function)
- [net_recv_bytes_from](#net_recv_bytes_from) (function)
- [NetBuffer](#NetBuffer) (struct)
- [net_buffer_create](#net_buffer_create) (function)
- [net_buffer_write_u8](#net_buffer_write_u8) (function)
- [net_buffer_write_u16](#net_buffer_write_u16) (function)
- [net_buffer_write_u32](#net_buffer_write_u32) (function)
- [net_buffer_read_u8](#net_buffer_read_u8) (function)
- [net_buffer_read_u16](#net_buffer_read_u16) (function)
- [net_buffer_read_u32](#net_buffer_read_u32) (function)
- [net_buffer_reset](#net_buffer_reset) (function)
- [net_buffer_remaining](#net_buffer_remaining) (function)
- [MSG_TYPE_DATA:](#MSG_TYPE_DATA:) (const)
- [MSG_TYPE_ACK:](#MSG_TYPE_ACK:) (const)
- [MSG_TYPE_PING:](#MSG_TYPE_PING:) (const)
- [MSG_TYPE_PONG:](#MSG_TYPE_PONG:) (const)
- [NET_RECV_IGNORED:](#NET_RECV_IGNORED:) (const)
- [NET_RECV_ACK:](#NET_RECV_ACK:) (const)
- [NET_RECV_DATA:](#NET_RECV_DATA:) (const)
- [NET_RECV_DUPLICATE:](#NET_RECV_DUPLICATE:) (const)
- [NET_RECV_PING:](#NET_RECV_PING:) (const)
- [NET_RECV_PONG:](#NET_RECV_PONG:) (const)
- [NET_RELIABLE_HEADER:](#NET_RELIABLE_HEADER:) (const)
- [ReliableMessage](#ReliableMessage) (struct)
- [ReliableConnection](#ReliableConnection) (struct)
- [NetReceived](#NetReceived) (struct)
- [net_addr_none](#net_addr_none) (function)
- [net_reliable_create](#net_reliable_create) (function)
- [net_reliable_packet](#net_reliable_packet) (function)
- [net_reliable_packet_id](#net_reliable_packet_id) (function)
- [net_reliable_take_outgoing](#net_reliable_take_outgoing) (function)
- [net_reliable_send_at](#net_reliable_send_at) (function)
- [net_reliable_send](#net_reliable_send) (function)
- [net_reliable_ping](#net_reliable_ping) (function)
- [net_reliable_recv_at](#net_reliable_recv_at) (function)
- [net_reliable_recv](#net_reliable_recv) (function)
- [net_reliable_update](#net_reliable_update) (function)
- [net_reliable_pending_count](#net_reliable_pending_count) (function)
- [net_reliable_rtt](#net_reliable_rtt) (function)

## AF_INET:

```fides
pub const AF_INET: i64 = 2;
```

*Defined at line 59*

---

## SOCK_DGRAM:

```fides
pub const SOCK_DGRAM: i64 = 2;
```

*Defined at line 60*

---

## SOCK_STREAM:

```fides
pub const SOCK_STREAM: i64 = 1;
```

*Defined at line 61*

---

## SOL_SOCKET:

```fides
pub const SOL_SOCKET: i64 = 1;
```

*Defined at line 62*

---

## SO_REUSEADDR:

```fides
pub const SO_REUSEADDR: i64 = 2;
```

*Defined at line 63*

---

## SO_BROADCAST:

```fides
pub const SO_BROADCAST: i64 = 6;
```

*Defined at line 64*

---

## SO_RCVTIMEO:

```fides
pub const SO_RCVTIMEO: i64 = 20;
```

*Defined at line 65*

---

## MSG_DONTWAIT:

```fides
pub const MSG_DONTWAIT: i64 = 0x40;
```

*Defined at line 66*

---

## NetAddr

```fides
pub struct NetAddr {
```

**Fields:**

| Name | Type |
|------|------|
| `ip` | `i64` |
| `port` | `i64` |
| `raw` | `i64,  // raw sockaddr_in pointer (native)` |

*Defined at line 69*

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

*Defined at line 78*

---

## net_addr_any

Create a "any" address (0.0.0.0) for binding.

```fides
pub fn net_addr_any(port: i64) -> NetAddr {
```

**Parameters:**

- `port`: `i64`

**Returns:** `NetAddr`

*Defined at line 96*

---

## net_socket_create

Create a UDP socket. Returns socket file descriptor or -1 on error.

```fides
pub fn net_socket_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 103*

---

## net_socket_create_tcp

Create a TCP socket. Returns socket file descriptor or -1 on error.

```fides
pub fn net_socket_create_tcp() -> i64 {
```

**Returns:** `i64`

*Defined at line 115*

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

*Defined at line 128*

---

## net_set_nonblocking

Set socket to non-blocking mode.

```fides
pub fn net_set_nonblocking(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 153*

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

*Defined at line 162*

---

## net_set_reuseaddr

Enable address reuse (SO_REUSEADDR).

```fides
pub fn net_set_reuseaddr(sock: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `i64`

*Defined at line 171*

---

## net_close

Close a socket.

```fides
pub fn net_close(sock: i64) -> void {
```

**Parameters:**

- `sock`: `i64`

**Returns:** `void`

*Defined at line 177*

---

## net_send_to

```fides
pub fn net_send_to(sock: i64, data: i64, len: i64, addr: &NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`
- `addr`: `&NetAddr`

**Returns:** `i64`

*Defined at line 196*

---

## net_recv_from

```fides
pub fn net_recv_from(sock: i64, buf: i64, len: i64, from_addr: &mut NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`
- `from_addr`: `&mut NetAddr`

**Returns:** `i64`

*Defined at line 212*

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

*Defined at line 231*

---

## net_send

```fides
pub fn net_send(sock: i64, data: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 251*

---

## net_recv

```fides
pub fn net_recv(sock: i64, buf: i64, len: i64) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `buf`: `i64`
- `len`: `i64`

**Returns:** `i64`

*Defined at line 266*

---

## net_send_file

Send the file at `path` on the connected stream socket `sock` (one from
p_socket_accept or p_net_connect): the number of bytes sent, or -1 when
the file cannot be read or the connection fails.

Zero-copy where the system allows it: the kernel sends the file's pages
itself, so they are never copied into the program -- sendfile on Linux
(C builds and native builds) and macOS, TransmitFile on Windows (C
builds). Elsewhere (the Windows native runtime, a socket or file
system without them) the file is read and sent, with the same bytes on
the wire. `fid run` only simulates sockets and has no send for this
(it stops with "Unknown runtime function: p_socket_write_str").

```fides
pub fn net_send_file(sock: i64, path: str) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 295*

---

## p_net_send_file

net_send_file's portable body: what a back end without a runtime
version runs (read the file, send it). The C back end runs its runtime's
__p_zc_send_file instead (compiler/codegen_c.fd fast_zero_copy_body) and
native Linux pert_net_send_file (stdlib/linux_runtime/sys.fd).

```fides
pub fn p_net_send_file(sock: i64, path: str) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 306*

---

## net_send_bytes

Send bytes on a connected socket: the number sent, or -1 on error.

```fides
pub fn net_send_bytes(sock: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 426*

---

## net_recv_bytes

Receive up to `max` bytes on a connected socket: the bytes ([] on an
error, a timeout or a closed connection).

```fides
pub fn net_recv_bytes(sock: i64, max: i64) -> Vec<i64> {
```

**Parameters:**

- `sock`: `i64`
- `max`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 448*

---

## net_send_bytes_to

Send bytes to an address (UDP): the number sent, or -1 on error.

```fides
pub fn net_send_bytes_to(sock: i64, data: Vec<i64>, addr: &NetAddr) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `data`: `Vec<i64>`
- `addr`: `&NetAddr`

**Returns:** `i64`

*Defined at line 471*

---

## net_recv_bytes_from

Receive up to `max` bytes from any sender (UDP): the bytes ([] on an
error or a timeout); the sender's address goes to from_addr.

```fides
pub fn net_recv_bytes_from(sock: i64, max: i64, from_addr: &mut NetAddr) -> Vec<i64> {
```

**Parameters:**

- `sock`: `i64`
- `max`: `i64`
- `from_addr`: `&mut NetAddr`

**Returns:** `Vec<i64>`

*Defined at line 493*

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

*Defined at line 518*

---

## net_buffer_create

```fides
pub fn net_buffer_create(size: i64) -> NetBuffer {
```

**Parameters:**

- `size`: `i64`

**Returns:** `NetBuffer`

*Defined at line 524*

---

## net_buffer_write_u8

```fides
pub fn net_buffer_write_u8(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 536*

---

## net_buffer_write_u16

```fides
pub fn net_buffer_write_u16(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 543*

---

## net_buffer_write_u32

```fides
pub fn net_buffer_write_u32(buf: &mut NetBuffer, val: i64) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`
- `val`: `i64`

**Returns:** `void`

*Defined at line 548*

---

## net_buffer_read_u8

```fides
pub fn net_buffer_read_u8(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 555*

---

## net_buffer_read_u16

```fides
pub fn net_buffer_read_u16(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 564*

---

## net_buffer_read_u32

```fides
pub fn net_buffer_read_u32(buf: &mut NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `i64`

*Defined at line 570*

---

## net_buffer_reset

```fides
pub fn net_buffer_reset(buf: &mut NetBuffer) -> void {
```

**Parameters:**

- `buf`: `&mut NetBuffer`

**Returns:** `void`

*Defined at line 578*

---

## net_buffer_remaining

```fides
pub fn net_buffer_remaining(buf: &NetBuffer) -> i64 {
```

**Parameters:**

- `buf`: `&NetBuffer`

**Returns:** `i64`

*Defined at line 582*

---

## MSG_TYPE_DATA:

```fides
pub const MSG_TYPE_DATA: i64 = 0;
```

*Defined at line 611*

---

## MSG_TYPE_ACK:

```fides
pub const MSG_TYPE_ACK: i64 = 1;
```

*Defined at line 612*

---

## MSG_TYPE_PING:

```fides
pub const MSG_TYPE_PING: i64 = 2;
```

*Defined at line 613*

---

## MSG_TYPE_PONG:

```fides
pub const MSG_TYPE_PONG: i64 = 3;
```

*Defined at line 614*

---

## NET_RECV_IGNORED:

What net_reliable_recv found in a packet (NetReceived.kind).

```fides
pub const NET_RECV_IGNORED: i64 = 0;
```

*Defined at line 616*

---

## NET_RECV_ACK:

```fides
pub const NET_RECV_ACK: i64 = 1;
```

*Defined at line 617*

---

## NET_RECV_DATA:

```fides
pub const NET_RECV_DATA: i64 = 2;
```

*Defined at line 618*

---

## NET_RECV_DUPLICATE:

```fides
pub const NET_RECV_DUPLICATE: i64 = 3;
```

*Defined at line 619*

---

## NET_RECV_PING:

```fides
pub const NET_RECV_PING: i64 = 4;
```

*Defined at line 620*

---

## NET_RECV_PONG:

```fides
pub const NET_RECV_PONG: i64 = 5;
```

*Defined at line 621*

---

## NET_RELIABLE_HEADER:

Bytes before the payload: the type byte and the u32 id.

```fides
pub const NET_RELIABLE_HEADER: i64 = 5;
```

*Defined at line 623*

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

*Defined at line 626*

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
| `rtt` | `f64` |
| `timeout_ms` | `i64` |
| `outgoing` | `Vec<Vec<i64>>,  // packets not sent, when sock < 0` |

*Defined at line 635*

---

## NetReceived

One packet, as net_reliable_recv understood it. `data` is the payload of
a new DATA message (kind NET_RECV_DATA), empty otherwise.

```fides
pub struct NetReceived {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `msg_id` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 650*

---

## net_addr_none

An address that is no address, for a connection that does no I/O
(socket below 0). It needs no socket library.

```fides
pub fn net_addr_none() -> NetAddr {
```

**Returns:** `NetAddr`

*Defined at line 658*

---

## net_reliable_create

```fides
pub fn net_reliable_create(sock: i64, remote: NetAddr) -> ReliableConnection {
```

**Parameters:**

- `sock`: `i64`
- `remote`: `NetAddr`

**Returns:** `ReliableConnection`

*Defined at line 664*

---

## net_reliable_packet

A packet: type byte, big-endian u32 id, then the payload bytes.

```fides
pub fn net_reliable_packet(msg_type: i64, msg_id: i64, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg_type`: `i64`
- `msg_id`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 671*

---

## net_reliable_packet_id

The id in a packet's header (0 for a packet too short to have one).

```fides
pub fn net_reliable_packet_id(packet: Vec<i64>) -> i64 {
```

**Parameters:**

- `packet`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 687*

---

## net_reliable_take_outgoing

The packets a connection with no socket would have sent, oldest first;
the queue is empty afterwards.

```fides
pub fn net_reliable_take_outgoing(conn: &mut ReliableConnection) -> Vec<Vec<i64>> {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 729*

---

## net_reliable_send_at

Send a reliable message (will be retransmitted until acked). `now_ms` is
the current time, from the same clock net_reliable_update is given.
Returns what the send returned (bytes, or -1). The message stays pending
either way: a failed send is retried as a lost one would be.

```fides
pub fn net_reliable_send_at(conn: &mut ReliableConnection, data: Vec<i64>, now_ms: i64) -> i64 {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `data`: `Vec<i64>`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 739*

---

## net_reliable_send

As net_reliable_send_at, at time 0 (the first net_reliable_update with a
time past the timeout sends it again).

```fides
pub fn net_reliable_send(conn: &mut ReliableConnection, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 756*

---

## net_reliable_ping

Ask the peer for a PONG carrying `id` (a keep-alive, or a probe of the
round trip). Not retransmitted.

```fides
pub fn net_reliable_ping(conn: &mut ReliableConnection, id: i64) -> i64 {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 762*

---

## net_reliable_recv_at

Process one incoming packet. `now_ms` is the current time (as given to
net_reliable_send_at and net_reliable_update), or below 0 when unknown.
An ACK marks its message delivered and, when the message was sent only
once and the time is known, refines the round-trip estimate (Karn: a
retransmitted message's ACK says nothing about the round trip). A DATA
packet is ACKed; a new one comes back as NET_RECV_DATA with its payload,
one already delivered as NET_RECV_DUPLICATE. A PING is answered with a
PONG. Anything else, or a packet too short for its header, is ignored.

```fides
pub fn net_reliable_recv_at(conn: &mut ReliableConnection, packet: Vec<i64>, now_ms: i64) -> NetReceived {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `packet`: `Vec<i64>`
- `now_ms`: `i64`

**Returns:** `NetReceived`

*Defined at line 800*

---

## net_reliable_recv

As net_reliable_recv_at with the time unknown (no round-trip sample).

```fides
pub fn net_reliable_recv(conn: &mut ReliableConnection, packet: Vec<i64>) -> NetReceived {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `packet`: `Vec<i64>`

**Returns:** `NetReceived`

*Defined at line 859*

---

## net_reliable_update

Update the connection: retransmit unacked messages that have timed out,
and forget the acked ones.

```fides
pub fn net_reliable_update(conn: &mut ReliableConnection, current_time: i64) -> void {
```

**Parameters:**

- `conn`: `&mut ReliableConnection`
- `current_time`: `i64`

**Returns:** `void`

*Defined at line 865*

---

## net_reliable_pending_count

Get the number of pending (unacked) messages.

```fides
pub fn net_reliable_pending_count(conn: &ReliableConnection) -> i64 {
```

**Parameters:**

- `conn`: `&ReliableConnection`

**Returns:** `i64`

*Defined at line 884*

---

## net_reliable_rtt

Get the estimated round-trip time in milliseconds.

```fides
pub fn net_reliable_rtt(conn: &ReliableConnection) -> f64 {
```

**Parameters:**

- `conn`: `&ReliableConnection`

**Returns:** `f64`

*Defined at line 898*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
