[All modules](index.md)

# tcpip -- Fides Documentation

## Contents

- [net_be16](#net_be16) (function)
- [net_be32](#net_be32) (function)
- [net_put16](#net_put16) (function)
- [net_put32](#net_put32) (function)
- [net_slice](#net_slice) (function)
- [net_sum](#net_sum) (function)
- [net_checksum](#net_checksum) (function)
- [net_l4_checksum](#net_l4_checksum) (function)
- [ip4](#ip4) (function)
- [ip_str](#ip_str) (function)
- [seq_add](#seq_add) (function)
- [seq_diff](#seq_diff) (function)
- [seq_lt](#seq_lt) (function)
- [seq_le](#seq_le) (function)
- [ETH_IPV4:](#ETH_IPV4:) (const)
- [ETH_ARP:](#ETH_ARP:) (const)
- [IP_ICMP:](#IP_ICMP:) (const)
- [IP_TCP:](#IP_TCP:) (const)
- [IP_UDP:](#IP_UDP:) (const)
- [TCP_FIN:](#TCP_FIN:) (const)
- [TCP_SYN:](#TCP_SYN:) (const)
- [TCP_RST:](#TCP_RST:) (const)
- [TCP_PSH:](#TCP_PSH:) (const)
- [TCP_ACK:](#TCP_ACK:) (const)
- [TCP_CLOSED:](#TCP_CLOSED:) (const)
- [TCP_LISTEN:](#TCP_LISTEN:) (const)
- [TCP_SYN_SENT:](#TCP_SYN_SENT:) (const)
- [TCP_SYN_RCVD:](#TCP_SYN_RCVD:) (const)
- [TCP_ESTABLISHED:](#TCP_ESTABLISHED:) (const)
- [TCP_FIN_WAIT_1:](#TCP_FIN_WAIT_1:) (const)
- [TCP_FIN_WAIT_2:](#TCP_FIN_WAIT_2:) (const)
- [TCP_CLOSE_WAIT:](#TCP_CLOSE_WAIT:) (const)
- [TCP_CLOSING:](#TCP_CLOSING:) (const)
- [TCP_LAST_ACK:](#TCP_LAST_ACK:) (const)
- [TCP_TIME_WAIT:](#TCP_TIME_WAIT:) (const)
- [tcp_state_name](#tcp_state_name) (function)
- [ArpEntry](#ArpEntry) (struct)
- [PendingPkt](#PendingPkt) (struct)
- [UdpSock](#UdpSock) (struct)
- [UdpDatagram](#UdpDatagram) (struct)
- [FragBuf](#FragBuf) (struct)
- [TcpConn](#TcpConn) (struct)
- [NetStack](#NetStack) (struct)
- [net_new](#net_new) (function)
- [net_take_tx](#net_take_tx) (function)
- [arp_lookup](#arp_lookup) (function)
- [arp_cache_size](#arp_cache_size) (function)
- [ip_send](#ip_send) (function)
- [net_input](#net_input) (function)
- [icmp_ping](#icmp_ping) (function)
- [udp_bind](#udp_bind) (function)
- [udp_sendto](#udp_sendto) (function)
- [udp_recv](#udp_recv) (function)
- [TcpSeg](#TcpSeg) (struct)
- [tcp_build](#tcp_build) (function)
- [tcp_parse](#tcp_parse) (function)
- [tcp_listen](#tcp_listen) (function)
- [tcp_connect](#tcp_connect) (function)
- [tcp_accept](#tcp_accept) (function)
- [tcp_state](#tcp_state) (function)
- [tcp_conn](#tcp_conn) (function)
- [tcp_send](#tcp_send) (function)
- [tcp_recv](#tcp_recv) (function)
- [tcp_available](#tcp_available) (function)
- [tcp_peer_closed](#tcp_peer_closed) (function)
- [tcp_close](#tcp_close) (function)
- [tcp_abort](#tcp_abort) (function)
- [net_tick](#net_tick) (function)

## net_be16

```fides
pub fn net_be16(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 36*

---

## net_be32

```fides
pub fn net_be32(b: Vec<i64>, o: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`

**Returns:** `i64`

*Defined at line 40*

---

## net_put16

```fides
pub fn net_put16(b: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 44*

---

## net_put32

```fides
pub fn net_put32(b: &mut Vec<i64>, v: i64) {
```

**Parameters:**

- `b`: `&mut Vec<i64>`
- `v`: `i64`

*Defined at line 49*

---

## net_slice

```fides
pub fn net_slice(b: Vec<i64>, from: i64, to: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `Vec<i64>`
- `from`: `i64`
- `to`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 59*

---

## net_sum

Ones' complement sum of b[o .. o+n) folded to 16 bits, starting
from `init` (not complemented).

```fides
pub fn net_sum(b: Vec<i64>, o: i64, n: i64, init: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `n`: `i64`
- `init`: `i64`

**Returns:** `i64`

*Defined at line 79*

---

## net_checksum

RFC 1071 Internet checksum of b[o .. o+n).

```fides
pub fn net_checksum(b: Vec<i64>, o: i64, n: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `o`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 96*

---

## net_l4_checksum

UDP/TCP checksum with the IPv4 pseudo-header.

```fides
pub fn net_l4_checksum(src: i64, dst: i64, proto: i64, seg: Vec<i64>) -> i64 {
```

**Parameters:**

- `src`: `i64`
- `dst`: `i64`
- `proto`: `i64`
- `seg`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 109*

---

## ip4

```fides
pub fn ip4(a: i64, b: i64, c: i64, d: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 113*

---

## ip_str

```fides
pub fn ip_str(ip: i64) -> str {
```

**Parameters:**

- `ip`: `i64`

**Returns:** `str`

*Defined at line 117*

---

## seq_add

```fides
pub fn seq_add(a: i64, n: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 123*

---

## seq_diff

a - b as a signed 32-bit distance.

```fides
pub fn seq_diff(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## seq_lt

```fides
pub fn seq_lt(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 136*

---

## seq_le

```fides
pub fn seq_le(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 140*

---

## ETH_IPV4:

```fides
pub const ETH_IPV4: i64 = 0x0800;
```

*Defined at line 146*

---

## ETH_ARP:

```fides
pub const ETH_ARP: i64 = 0x0806;
```

*Defined at line 147*

---

## IP_ICMP:

```fides
pub const IP_ICMP: i64 = 1;
```

*Defined at line 148*

---

## IP_TCP:

```fides
pub const IP_TCP: i64 = 6;
```

*Defined at line 149*

---

## IP_UDP:

```fides
pub const IP_UDP: i64 = 17;
```

*Defined at line 150*

---

## TCP_FIN:

```fides
pub const TCP_FIN: i64 = 0x01;
```

*Defined at line 152*

---

## TCP_SYN:

```fides
pub const TCP_SYN: i64 = 0x02;
```

*Defined at line 153*

---

## TCP_RST:

```fides
pub const TCP_RST: i64 = 0x04;
```

*Defined at line 154*

---

## TCP_PSH:

```fides
pub const TCP_PSH: i64 = 0x08;
```

*Defined at line 155*

---

## TCP_ACK:

```fides
pub const TCP_ACK: i64 = 0x10;
```

*Defined at line 156*

---

## TCP_CLOSED:

```fides
pub const TCP_CLOSED: i64 = 0;
```

*Defined at line 158*

---

## TCP_LISTEN:

```fides
pub const TCP_LISTEN: i64 = 1;
```

*Defined at line 159*

---

## TCP_SYN_SENT:

```fides
pub const TCP_SYN_SENT: i64 = 2;
```

*Defined at line 160*

---

## TCP_SYN_RCVD:

```fides
pub const TCP_SYN_RCVD: i64 = 3;
```

*Defined at line 161*

---

## TCP_ESTABLISHED:

```fides
pub const TCP_ESTABLISHED: i64 = 4;
```

*Defined at line 162*

---

## TCP_FIN_WAIT_1:

```fides
pub const TCP_FIN_WAIT_1: i64 = 5;
```

*Defined at line 163*

---

## TCP_FIN_WAIT_2:

```fides
pub const TCP_FIN_WAIT_2: i64 = 6;
```

*Defined at line 164*

---

## TCP_CLOSE_WAIT:

```fides
pub const TCP_CLOSE_WAIT: i64 = 7;
```

*Defined at line 165*

---

## TCP_CLOSING:

```fides
pub const TCP_CLOSING: i64 = 8;
```

*Defined at line 166*

---

## TCP_LAST_ACK:

```fides
pub const TCP_LAST_ACK: i64 = 9;
```

*Defined at line 167*

---

## TCP_TIME_WAIT:

```fides
pub const TCP_TIME_WAIT: i64 = 10;
```

*Defined at line 168*

---

## tcp_state_name

```fides
pub fn tcp_state_name(st: i64) -> str {
```

**Parameters:**

- `st`: `i64`

**Returns:** `str`

*Defined at line 170*

---

## ArpEntry

```fides
pub struct ArpEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `ip` | `i64` |
| `mac` | `Vec<i64>` |
| `resolved` | `bool` |
| `expires` | `i64` |
| `asked_at` | `i64` |
| `tries` | `i64` |

*Defined at line 206*

---

## PendingPkt

```fides
pub struct PendingPkt {
```

**Fields:**

| Name | Type |
|------|------|
| `hop` | `i64` |
| `packet` | `Vec<i64>` |

*Defined at line 215*

---

## UdpSock

```fides
pub struct UdpSock {
```

**Fields:**

| Name | Type |
|------|------|
| `port` | `i64` |
| `q_src` | `Vec<i64>` |
| `q_sport` | `Vec<i64>` |
| `q_data` | `Vec<Vec<i64>>` |

*Defined at line 220*

---

## UdpDatagram

```fides
pub struct UdpDatagram {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `src` | `i64` |
| `sport` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 227*

---

## FragBuf

```fides
pub struct FragBuf {
```

**Fields:**

| Name | Type |
|------|------|
| `src` | `i64` |
| `dst` | `i64` |
| `id` | `i64` |
| `proto` | `i64` |
| `offs` | `Vec<i64>` |
| `datas` | `Vec<Vec<i64>>` |
| `total` | `i64` |
| `deadline` | `i64` |

*Defined at line 234*

---

## TcpConn

```fides
pub struct TcpConn {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `lport` | `i64` |
| `rport` | `i64` |
| `rip` | `i64` |
| `listener` | `i64` |
| `accepted` | `bool` |
| `iss` | `i64` |
| `irs` | `i64` |
| `snd_una` | `i64` |
| `snd_nxt` | `i64` |
| `snd_wnd` | `i64` |
| `snd_wl1` | `i64` |
| `snd_wl2` | `i64` |
| `rcv_nxt` | `i64` |
| `rcv_cap` | `i64` |
| `adv_wnd` | `i64` |
| `mss` | `i64` |
| `cwnd` | `i64` |
| `sendbuf` | `Vec<i64>` |
| `app_closed` | `bool` |
| `fin_seq` | `i64` |
| `peer_fin` | `i64` |
| `recvbuf` | `Vec<i64>` |
| `ooo_seq` | `Vec<i64>` |
| `ooo_data` | `Vec<Vec<i64>>` |
| `rto` | `i64` |
| `srtt` | `i64` |
| `rttvar` | `i64` |
| `rtx_at` | `i64` |
| `rtx_count` | `i64` |
| `rtt_seq` | `i64` |
| `rtt_start` | `i64` |
| `tw_at` | `i64` |
| `error` | `str` |
| `retransmits` | `i64` |
| `ooo_segments` | `i64` |
| `dup_acks` | `i64` |

*Defined at line 246*

---

## NetStack

```fides
pub struct NetStack {
```

**Fields:**

| Name | Type |
|------|------|
| `mac` | `Vec<i64>` |
| `ip` | `i64` |
| `mask` | `i64` |
| `gw` | `i64` |
| `mtu` | `i64` |
| `now` | `i64` |
| `arp` | `Vec<ArpEntry>` |
| `pending` | `Vec<PendingPkt>` |
| `tx` | `Vec<Vec<i64>>` |
| `udp` | `Vec<UdpSock>` |
| `tcp` | `Vec<TcpConn>` |
| `listening` | `Vec<i64>` |
| `frags` | `Vec<FragBuf>` |
| `ip_id` | `i64` |
| `isn_seed` | `i64` |
| `conns_made` | `i64` |
| `ping_ids` | `Vec<i64>` |
| `ping_from` | `Vec<i64>` |
| `ping_rtt` | `Vec<i64>` |
| `ping_sent_at` | `Vec<i64>` |
| `unreachable` | `Vec<i64>` |
| `arp_timeout` | `i64` |
| `tcp_rcvbuf` | `i64` |
| `tcp_sndbuf` | `i64` |
| `msl` | `i64` |
| `rto_min` | `i64` |
| `rx_frames` | `i64` |
| `dropped` | `i64` |
| `frag_timeouts` | `i64` |
| `reassembled` | `i64` |

*Defined at line 293*

---

## net_new

```fides
pub fn net_new(mac: Vec<i64>, ip: i64, mask: i64, gw: i64) -> NetStack {
```

**Parameters:**

- `mac`: `Vec<i64>`
- `ip`: `i64`
- `mask`: `i64`
- `gw`: `i64`

**Returns:** `NetStack`

*Defined at line 327*

---

## net_take_tx

Frames waiting for the interface; the queue is emptied.

```fides
pub fn net_take_tx(s: &mut NetStack) -> Vec<Vec<i64>> {
```

**Parameters:**

- `s`: `&mut NetStack`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 334*

---

## arp_lookup

MAC of ip from the cache (empty when unknown or still resolving).

```fides
pub fn arp_lookup(s: &NetStack, ip: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&NetStack`
- `ip`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 396*

---

## arp_cache_size

```fides
pub fn arp_cache_size(s: &NetStack) -> i64 {
```

**Parameters:**

- `s`: `&NetStack`

**Returns:** `i64`

*Defined at line 404*

---

## ip_send

Send an IP datagram, fragmenting it to the MTU. Returns the number of
packets it took.

```fides
pub fn ip_send(s: &mut NetStack, dst: i64, proto: i64, payload: Vec<i64>) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `dst`: `i64`
- `proto`: `i64`
- `payload`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 514*

---

## net_input

Hand one received Ethernet frame to the stack.

```fides
pub fn net_input(s: &mut NetStack, f: Vec<i64>) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `f`: `Vec<i64>`

*Defined at line 647*

---

## icmp_ping

Send an echo request (ping) with id/seq and payload.

```fides
pub fn icmp_ping(s: &mut NetStack, dst: i64, id: i64, seq: i64, data: Vec<i64>) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `dst`: `i64`
- `id`: `i64`
- `seq`: `i64`
- `data`: `Vec<i64>`

*Defined at line 679*

---

## udp_bind

Open a socket on port; returns its handle (-1 when the port is taken).

```fides
pub fn udp_bind(s: &mut NetStack, port: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 714*

---

## udp_sendto

```fides
pub fn udp_sendto(s: &mut NetStack, sock: i64, dst: i64, dport: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `sock`: `i64`
- `dst`: `i64`
- `dport`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 728*

---

## udp_recv

Next queued datagram of the socket (ok = false when none).

```fides
pub fn udp_recv(s: &mut NetStack, sock: i64) -> UdpDatagram {
```

**Parameters:**

- `s`: `&mut NetStack`
- `sock`: `i64`

**Returns:** `UdpDatagram`

*Defined at line 745*

---

## TcpSeg

```fides
pub struct TcpSeg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `sport` | `i64` |
| `dport` | `i64` |
| `seq` | `i64` |
| `ack` | `i64` |
| `flags` | `i64` |
| `window` | `i64` |
| `mss` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 794*

---

## tcp_build

Build a TCP segment (checksummed for src -> dst).

```fides
pub fn tcp_build(src: i64, dst: i64, sport: i64, dport: i64, seq: i64, ack: i64, flags: i64, window: i64, mss: i64, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `i64`
- `dst`: `i64`
- `sport`: `i64`
- `dport`: `i64`
- `seq`: `i64`
- `ack`: `i64`
- `flags`: `i64`
- `window`: `i64`
- `mss`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 808*

---

## tcp_parse

```fides
pub fn tcp_parse(src: i64, dst: i64, t: Vec<i64>) -> TcpSeg {
```

**Parameters:**

- `src`: `i64`
- `dst`: `i64`
- `t`: `Vec<i64>`

**Returns:** `TcpSeg`

*Defined at line 833*

---

## tcp_listen

Start listening on a port.

```fides
pub fn tcp_listen(s: &mut NetStack, port: i64) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `port`: `i64`

*Defined at line 996*

---

## tcp_connect

Active open; returns the connection handle.

```fides
pub fn tcp_connect(s: &mut NetStack, dst: i64, dport: i64, lport: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `dst`: `i64`
- `dport`: `i64`
- `lport`: `i64`

**Returns:** `i64`

*Defined at line 1003*

---

## tcp_accept

An established connection on a listening port not yet accepted, or -1.

```fides
pub fn tcp_accept(s: &mut NetStack, port: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 1014*

---

## tcp_state

```fides
pub fn tcp_state(s: &NetStack, ci: i64) -> i64 {
```

**Parameters:**

- `s`: `&NetStack`
- `ci`: `i64`

**Returns:** `i64`

*Defined at line 1028*

---

## tcp_conn

```fides
pub fn tcp_conn(s: &NetStack, ci: i64) -> TcpConn {
```

**Parameters:**

- `s`: `&NetStack`
- `ci`: `i64`

**Returns:** `TcpConn`

*Defined at line 1032*

---

## tcp_send

Queue bytes for sending; returns how many fit the send buffer.

```fides
pub fn tcp_send(s: &mut NetStack, ci: i64, data: Vec<i64>) -> i64 {
```

**Parameters:**

- `s`: `&mut NetStack`
- `ci`: `i64`
- `data`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 1037*

---

## tcp_recv

Take up to max received bytes. Sends a window update when reading
reopened the window by at least one MSS.

```fides
pub fn tcp_recv(s: &mut NetStack, ci: i64, max: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&mut NetStack`
- `ci`: `i64`
- `max`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1059*

---

## tcp_available

Bytes waiting to be read.

```fides
pub fn tcp_available(s: &NetStack, ci: i64) -> i64 {
```

**Parameters:**

- `s`: `&NetStack`
- `ci`: `i64`

**Returns:** `i64`

*Defined at line 1075*

---

## tcp_peer_closed

Whether the peer closed its side (FIN received).

```fides
pub fn tcp_peer_closed(s: &NetStack, ci: i64) -> bool {
```

**Parameters:**

- `s`: `&NetStack`
- `ci`: `i64`

**Returns:** `bool`

*Defined at line 1080*

---

## tcp_close

Close our side: FIN after the queued data.

```fides
pub fn tcp_close(s: &mut NetStack, ci: i64) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `ci`: `i64`

*Defined at line 1086*

---

## tcp_abort

Abort with a RST.

```fides
pub fn tcp_abort(s: &mut NetStack, ci: i64) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `ci`: `i64`

*Defined at line 1100*

---

## net_tick

Advance the stack's clock and run its timers.

```fides
pub fn net_tick(s: &mut NetStack, now: i64) {
```

**Parameters:**

- `s`: `&mut NetStack`
- `now`: `i64`

*Defined at line 1501*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
