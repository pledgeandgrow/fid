[All modules](index.md)

# packet -- Fides Documentation

## Contents

- [ETH_IPV4:](#ETH_IPV4:) (const)
- [ETH_ARP:](#ETH_ARP:) (const)
- [ETH_IPV6:](#ETH_IPV6:) (const)
- [ETH_VLAN:](#ETH_VLAN:) (const)
- [ETH_QINQ:](#ETH_QINQ:) (const)
- [IP_ICMP:](#IP_ICMP:) (const)
- [IP_TCP:](#IP_TCP:) (const)
- [IP_UDP:](#IP_UDP:) (const)
- [IP_ICMPV6:](#IP_ICMPV6:) (const)
- [TCP_FIN:](#TCP_FIN:) (const)
- [TCP_SYN:](#TCP_SYN:) (const)
- [TCP_RST:](#TCP_RST:) (const)
- [TCP_PSH:](#TCP_PSH:) (const)
- [TCP_ACK:](#TCP_ACK:) (const)
- [TCP_URG:](#TCP_URG:) (const)
- [pk_hex16](#pk_hex16) (function)
- [inet_sum](#inet_sum) (function)
- [inet_checksum](#inet_checksum) (function)
- [pk_checksum_v4](#pk_checksum_v4) (function)
- [pk_checksum_v6](#pk_checksum_v6) (function)
- [mac_to_str](#mac_to_str) (function)
- [mac_parse](#mac_parse) (function)
- [ipv4_to_str](#ipv4_to_str) (function)
- [ipv4_parse](#ipv4_parse) (function)
- [ipv6_to_str](#ipv6_to_str) (function)
- [ipv6_parse](#ipv6_parse) (function)
- [EthHdr](#EthHdr) (struct)
- [eth_decode](#eth_decode) (function)
- [eth_build](#eth_build) (function)
- [ArpPkt](#ArpPkt) (struct)
- [arp_decode](#arp_decode) (function)
- [arp_build](#arp_build) (function)
- [Ipv4Hdr](#Ipv4Hdr) (struct)
- [ipv4_hdr](#ipv4_hdr) (function)
- [ipv4_decode](#ipv4_decode) (function)
- [ipv4_header_bytes](#ipv4_header_bytes) (function)
- [ipv4_build](#ipv4_build) (function)
- [Ipv6Hdr](#Ipv6Hdr) (struct)
- [ipv6_decode](#ipv6_decode) (function)
- [ipv6_build](#ipv6_build) (function)
- [IcmpHdr](#IcmpHdr) (struct)
- [icmp_decode](#icmp_decode) (function)
- [icmpv6_decode](#icmpv6_decode) (function)
- [icmp_build](#icmp_build) (function)
- [icmpv6_build](#icmpv6_build) (function)
- [TcpOpt](#TcpOpt) (struct)
- [TcpHdr](#TcpHdr) (struct)
- [tcp_hdr](#tcp_hdr) (function)
- [tcp_opt](#tcp_opt) (function)
- [tcp_opt_mss](#tcp_opt_mss) (function)
- [tcp_opt_wscale](#tcp_opt_wscale) (function)
- [tcp_opt_sack_ok](#tcp_opt_sack_ok) (function)
- [tcp_opt_timestamps](#tcp_opt_timestamps) (function)
- [tcp_opt_value](#tcp_opt_value) (function)
- [tcp_flags_str](#tcp_flags_str) (function)
- [tcp_decode](#tcp_decode) (function)
- [tcp_decode_v4](#tcp_decode_v4) (function)
- [tcp_decode_v6](#tcp_decode_v6) (function)
- [tcp_build_v4](#tcp_build_v4) (function)
- [tcp_build_v6](#tcp_build_v6) (function)
- [UdpHdr](#UdpHdr) (struct)
- [udp_decode](#udp_decode) (function)
- [udp_decode_v4](#udp_decode_v4) (function)
- [udp_decode_v6](#udp_decode_v6) (function)
- [udp_build_v4](#udp_build_v4) (function)
- [udp_build_v6](#udp_build_v6) (function)
- [DnsQuestion](#DnsQuestion) (struct)
- [DnsHdr](#DnsHdr) (struct)
- [dns_read_name](#dns_read_name) (function)
- [dns_decode](#dns_decode) (function)
- [dns_encode_name](#dns_encode_name) (function)
- [dns_build](#dns_build) (function)
- [dns_query_new](#dns_query_new) (function)
- [pkt_dissect](#pkt_dissect) (function)

## ETH_IPV4:

```fides
pub const ETH_IPV4: i64 = 0x0800;
```

*Defined at line 28*

---

## ETH_ARP:

```fides
pub const ETH_ARP: i64 = 0x0806;
```

*Defined at line 29*

---

## ETH_IPV6:

```fides
pub const ETH_IPV6: i64 = 0x86DD;
```

*Defined at line 30*

---

## ETH_VLAN:

```fides
pub const ETH_VLAN: i64 = 0x8100;
```

*Defined at line 31*

---

## ETH_QINQ:

```fides
pub const ETH_QINQ: i64 = 0x88A8;
```

*Defined at line 32*

---

## IP_ICMP:

```fides
pub const IP_ICMP: i64 = 1;
```

*Defined at line 33*

---

## IP_TCP:

```fides
pub const IP_TCP: i64 = 6;
```

*Defined at line 34*

---

## IP_UDP:

```fides
pub const IP_UDP: i64 = 17;
```

*Defined at line 35*

---

## IP_ICMPV6:

```fides
pub const IP_ICMPV6: i64 = 58;
```

*Defined at line 36*

---

## TCP_FIN:

```fides
pub const TCP_FIN: i64 = 1;
```

*Defined at line 38*

---

## TCP_SYN:

```fides
pub const TCP_SYN: i64 = 2;
```

*Defined at line 39*

---

## TCP_RST:

```fides
pub const TCP_RST: i64 = 4;
```

*Defined at line 40*

---

## TCP_PSH:

```fides
pub const TCP_PSH: i64 = 8;
```

*Defined at line 41*

---

## TCP_ACK:

```fides
pub const TCP_ACK: i64 = 16;
```

*Defined at line 42*

---

## TCP_URG:

```fides
pub const TCP_URG: i64 = 32;
```

*Defined at line 43*

---

## pk_hex16

0x-prefixed lowercase hex of a 16-bit value, four digits.

```fides
pub fn pk_hex16(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 89*

---

## inet_sum

One's-complement sum of `b` as 16-bit words (odd length: a zero pad),
added to `start` and folded to 16 bits (not inverted).

```fides
pub fn inet_sum(b: &Vec<i64>, start: i64) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `start`: `i64`

**Returns:** `i64`

*Defined at line 97*

---

## inet_checksum

The Internet checksum (RFC 1071) of `b`.

```fides
pub fn inet_checksum(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 114*

---

## pk_checksum_v4

The upper-layer checksum of `seg` (its checksum field zero or not: the
caller zeroes it when building) over the IPv4 pseudo-header.

```fides
pub fn pk_checksum_v4(src: &Vec<i64>, dst: &Vec<i64>, proto: i64, seg: &Vec<i64>) -> i64 {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `proto`: `i64`
- `seg`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 142*

---

## pk_checksum_v6

The upper-layer checksum of `seg` over the IPv6 pseudo-header.

```fides
pub fn pk_checksum_v6(src: &Vec<i64>, dst: &Vec<i64>, next: i64, seg: &Vec<i64>) -> i64 {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `next`: `i64`
- `seg`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 147*

---

## mac_to_str

aa:bb:cc:dd:ee:ff

```fides
pub fn mac_to_str(m: &Vec<i64>) -> str {
```

**Parameters:**

- `m`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 152*

---

## mac_parse

Parse aa:bb:cc:dd:ee:ff (or with '-'); [] when malformed.

```fides
pub fn mac_parse(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 166*

---

## ipv4_to_str

192.168.0.1

```fides
pub fn ipv4_to_str(a: &Vec<i64>) -> str {
```

**Parameters:**

- `a`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 209*

---

## ipv4_parse

Parse a dotted quad; [] when malformed.

```fides
pub fn ipv4_parse(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 214*

---

## ipv6_to_str

RFC 5952 text of a 16-byte IPv6 address (lowercase, the longest run of
two or more zero groups as "::", the first such run on a tie).

```fides
pub fn ipv6_to_str(a: &Vec<i64>) -> str {
```

**Parameters:**

- `a`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 250*

---

## ipv6_parse

Parse IPv6 text (with "::"; no embedded IPv4); [] when malformed.

```fides
pub fn ipv6_parse(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 339*

---

## EthHdr

An Ethernet II header. `vlans` holds each 802.1Q/802.1ad tag's TCI
(priority << 13 | DEI << 12 | VLAN id), outermost first; `ethertype` is
the type after the tags; `payload` the offset of the next header.

```fides
pub struct EthHdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `dst` | `Vec<i64>` |
| `src` | `Vec<i64>` |
| `vlans` | `Vec<i64>` |
| `ethertype` | `i64` |
| `payload` | `i64` |

*Defined at line 382*

---

## eth_decode

```fides
pub fn eth_decode(b: &Vec<i64>) -> EthHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `EthHdr`

*Defined at line 391*

---

## eth_build

An Ethernet II frame; `vlans` are TCIs to tag it with (802.1Q each).

```fides
pub fn eth_build(dst: &Vec<i64>, src: &Vec<i64>, vlans: &Vec<i64>, ethertype: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `dst`: `&Vec<i64>`
- `src`: `&Vec<i64>`
- `vlans`: `&Vec<i64>`
- `ethertype`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 414*

---

## ArpPkt

```fides
pub struct ArpPkt {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `htype` | `i64` |
| `ptype` | `i64` |
| `op` | `i64` |
| `sha` | `Vec<i64>` |
| `spa` | `Vec<i64>` |
| `tha` | `Vec<i64>` |
| `tpa` | `Vec<i64>` |

*Defined at line 431*

---

## arp_decode

```fides
pub fn arp_decode(b: &Vec<i64>, off: i64) -> ArpPkt {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `ArpPkt`

*Defined at line 442*

---

## arp_build

An Ethernet/IPv4 ARP packet (op 1 request, 2 reply).

```fides
pub fn arp_build(op: i64, sha: &Vec<i64>, spa: &Vec<i64>, tha: &Vec<i64>, tpa: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `op`: `i64`
- `sha`: `&Vec<i64>`
- `spa`: `&Vec<i64>`
- `tha`: `&Vec<i64>`
- `tpa`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 467*

---

## Ipv4Hdr

An IPv4 header. `flags` is the 3-bit field (2 = DF, 1 = MF), `frag` the
fragment offset in 8-byte units; `payload` the offset after the header,
`end` the offset after the packet (from the total length).

```fides
pub struct Ipv4Hdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `ihl` | `i64` |
| `tos` | `i64` |
| `total_len` | `i64` |
| `id` | `i64` |
| `flags` | `i64` |
| `frag` | `i64` |
| `ttl` | `i64` |
| `proto` | `i64` |
| `checksum` | `i64` |
| `checksum_ok` | `bool` |
| `src` | `Vec<i64>` |
| `dst` | `Vec<i64>` |
| `options` | `Vec<i64>` |
| `payload` | `i64` |
| `end` | `i64` |

*Defined at line 486*

---

## ipv4_hdr

A fresh header for building: ttl 64, DF set, no options.

```fides
pub fn ipv4_hdr(proto: i64, src: Vec<i64>, dst: Vec<i64>) -> Ipv4Hdr {
```

**Parameters:**

- `proto`: `i64`
- `src`: `Vec<i64>`
- `dst`: `Vec<i64>`

**Returns:** `Ipv4Hdr`

*Defined at line 506*

---

## ipv4_decode

```fides
pub fn ipv4_decode(b: &Vec<i64>, off: i64) -> Ipv4Hdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `Ipv4Hdr`

*Defined at line 512*

---

## ipv4_header_bytes

The header bytes with its checksum computed (total length from the
payload length; options padded to 32 bits).

```fides
pub fn ipv4_header_bytes(h: &Ipv4Hdr, payload_len: i64) -> Vec<i64> {
```

**Parameters:**

- `h`: `&Ipv4Hdr`
- `payload_len`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 545*

---

## ipv4_build

Header plus payload.

```fides
pub fn ipv4_build(h: &Ipv4Hdr, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `h`: `&Ipv4Hdr`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 570*

---

## Ipv6Hdr

An IPv6 header. `next` is the upper-layer protocol after any extension
headers (hop-by-hop, routing, fragment, destination options), found at
`payload`; `first_next` the Next Header field of the fixed header;
`end` the offset after the packet (from the payload length).

```fides
pub struct Ipv6Hdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `tclass` | `i64` |
| `flow` | `i64` |
| `payload_len` | `i64` |
| `first_next` | `i64` |
| `next` | `i64` |
| `hop_limit` | `i64` |
| `src` | `Vec<i64>` |
| `dst` | `Vec<i64>` |
| `payload` | `i64` |
| `end` | `i64` |

*Defined at line 582*

---

## ipv6_decode

```fides
pub fn ipv6_decode(b: &Vec<i64>, off: i64) -> Ipv6Hdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`

**Returns:** `Ipv6Hdr`

*Defined at line 596*

---

## ipv6_build

A fixed IPv6 header (no extension headers) plus payload.

```fides
pub fn ipv6_build(tclass: i64, flow: i64, next: i64, hop_limit: i64, src: &Vec<i64>, dst: &Vec<i64>, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `tclass`: `i64`
- `flow`: `i64`
- `next`: `i64`
- `hop_limit`: `i64`
- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 630*

---

## IcmpHdr

An ICMP or ICMPv6 message. `rest` is the 4-byte "rest of header" (for
echo: identifier << 16 | sequence); `data` what follows.

```fides
pub struct IcmpHdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kind` | `i64` |
| `code` | `i64` |
| `checksum` | `i64` |
| `checksum_ok` | `bool` |
| `rest` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 646*

---

## icmp_decode

ICMP over IPv4 at `off` up to `end`; the checksum covers the message.

```fides
pub fn icmp_decode(b: &Vec<i64>, off: i64, end: i64) -> IcmpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `end`: `i64`

**Returns:** `IcmpHdr`

*Defined at line 673*

---

## icmpv6_decode

ICMPv6: the checksum covers the IPv6 pseudo-header too.

```fides
pub fn icmpv6_decode(b: &Vec<i64>, off: i64, end: i64, src: &Vec<i64>, dst: &Vec<i64>) -> IcmpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `end`: `i64`
- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`

**Returns:** `IcmpHdr`

*Defined at line 683*

---

## icmp_build

```fides
pub fn icmp_build(kind: i64, code: i64, rest: i64, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `kind`: `i64`
- `code`: `i64`
- `rest`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 702*

---

## icmpv6_build

```fides
pub fn icmpv6_build(src: &Vec<i64>, dst: &Vec<i64>, kind: i64, code: i64, rest: i64, data: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `kind`: `i64`
- `code`: `i64`
- `rest`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 710*

---

## TcpOpt

A TCP option: kind and its data (without kind and length bytes).

```fides
pub struct TcpOpt {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 721*

---

## TcpHdr

A TCP header; `flags` holds the 8 flag bits (CWR ECE URG ACK PSH RST
SYN FIN). `checksum_ok` is set by tcp_decode_v4/v6.

```fides
pub struct TcpHdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `src_port` | `i64` |
| `dst_port` | `i64` |
| `seq` | `i64` |
| `ack` | `i64` |
| `data_off` | `i64` |
| `flags` | `i64` |
| `window` | `i64` |
| `checksum` | `i64` |
| `checksum_ok` | `bool` |
| `urgent` | `i64` |
| `options` | `Vec<TcpOpt>` |
| `payload` | `Vec<i64>` |

*Defined at line 728*

---

## tcp_hdr

A header to build: no options, window 65535.

```fides
pub fn tcp_hdr(src_port: i64, dst_port: i64, seq: i64, ack: i64, flags: i64) -> TcpHdr {
```

**Parameters:**

- `src_port`: `i64`
- `dst_port`: `i64`
- `seq`: `i64`
- `ack`: `i64`
- `flags`: `i64`

**Returns:** `TcpHdr`

*Defined at line 745*

---

## tcp_opt

```fides
pub fn tcp_opt(kind: i64, data: Vec<i64>) -> TcpOpt {
```

**Parameters:**

- `kind`: `i64`
- `data`: `Vec<i64>`

**Returns:** `TcpOpt`

*Defined at line 751*

---

## tcp_opt_mss

MSS (kind 2), window scale (3), SACK permitted (4), timestamps (8).

```fides
pub fn tcp_opt_mss(mss: i64) -> TcpOpt {
```

**Parameters:**

- `mss`: `i64`

**Returns:** `TcpOpt`

*Defined at line 758*

---

## tcp_opt_wscale

```fides
pub fn tcp_opt_wscale(shift: i64) -> TcpOpt {
```

**Parameters:**

- `shift`: `i64`

**Returns:** `TcpOpt`

*Defined at line 764*

---

## tcp_opt_sack_ok

```fides
pub fn tcp_opt_sack_ok() -> TcpOpt {
```

**Returns:** `TcpOpt`

*Defined at line 770*

---

## tcp_opt_timestamps

```fides
pub fn tcp_opt_timestamps(tsval: i64, tsecr: i64) -> TcpOpt {
```

**Parameters:**

- `tsval`: `i64`
- `tsecr`: `i64`

**Returns:** `TcpOpt`

*Defined at line 774*

---

## tcp_opt_value

The option of `kind`'s value as a number (MSS, window scale, the
timestamp value), or -1 when absent.

```fides
pub fn tcp_opt_value(h: &TcpHdr, kind: i64) -> i64 {
```

**Parameters:**

- `h`: `&TcpHdr`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 783*

---

## tcp_flags_str

Flag letters, "SYN,ACK" style.

```fides
pub fn tcp_flags_str(f: i64) -> str {
```

**Parameters:**

- `f`: `i64`

**Returns:** `str`

*Defined at line 802*

---

## tcp_decode

Decode a TCP segment at `off` up to `end` (no checksum check).

```fides
pub fn tcp_decode(b: &Vec<i64>, off: i64, end: i64) -> TcpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `end`: `i64`

**Returns:** `TcpHdr`

*Defined at line 841*

---

## tcp_decode_v4

```fides
pub fn tcp_decode_v4(b: &Vec<i64>, ip: &Ipv4Hdr) -> TcpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `ip`: `&Ipv4Hdr`

**Returns:** `TcpHdr`

*Defined at line 885*

---

## tcp_decode_v6

```fides
pub fn tcp_decode_v6(b: &Vec<i64>, ip: &Ipv6Hdr) -> TcpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `ip`: `&Ipv6Hdr`

**Returns:** `TcpHdr`

*Defined at line 894*

---

## tcp_build_v4

A TCP segment for IPv4 with its checksum.

```fides
pub fn tcp_build_v4(src: &Vec<i64>, dst: &Vec<i64>, h: &TcpHdr, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `h`: `&TcpHdr`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 936*

---

## tcp_build_v6

A TCP segment for IPv6 with its checksum.

```fides
pub fn tcp_build_v6(src: &Vec<i64>, dst: &Vec<i64>, h: &TcpHdr, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `h`: `&TcpHdr`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 945*

---

## UdpHdr

```fides
pub struct UdpHdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `src_port` | `i64` |
| `dst_port` | `i64` |
| `length` | `i64` |
| `checksum` | `i64` |
| `checksum_ok` | `bool` |
| `payload` | `Vec<i64>` |

*Defined at line 955*

---

## udp_decode

Decode a UDP datagram at `off`, its length from the UDP header (must fit
before `end`); no checksum check.

```fides
pub fn udp_decode(b: &Vec<i64>, off: i64, end: i64) -> UdpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `off`: `i64`
- `end`: `i64`

**Returns:** `UdpHdr`

*Defined at line 967*

---

## udp_decode_v4

UDP over IPv4; a zero checksum means "none" (RFC 768) and is accepted.

```fides
pub fn udp_decode_v4(b: &Vec<i64>, ip: &Ipv4Hdr) -> UdpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `ip`: `&Ipv4Hdr`

**Returns:** `UdpHdr`

*Defined at line 987*

---

## udp_decode_v6

UDP over IPv6, where the checksum is mandatory (RFC 8200 8.1).

```fides
pub fn udp_decode_v6(b: &Vec<i64>, ip: &Ipv6Hdr) -> UdpHdr {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `ip`: `&Ipv6Hdr`

**Returns:** `UdpHdr`

*Defined at line 997*

---

## udp_build_v4

A UDP datagram for IPv4 with its checksum (0 is sent as 0xFFFF).

```fides
pub fn udp_build_v4(src: &Vec<i64>, dst: &Vec<i64>, sp: i64, dp: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `sp`: `i64`
- `dp`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1017*

---

## udp_build_v6

```fides
pub fn udp_build_v6(src: &Vec<i64>, dst: &Vec<i64>, sp: i64, dp: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `src`: `&Vec<i64>`
- `dst`: `&Vec<i64>`
- `sp`: `i64`
- `dp`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 1028*

---

## DnsQuestion

```fides
pub struct DnsQuestion {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `qtype` | `i64` |
| `qclass` | `i64` |

*Defined at line 1041*

---

## DnsHdr

The DNS header (RFC 1035 4.1.1) and the question section.

```fides
pub struct DnsHdr {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `id` | `i64` |
| `qr` | `bool` |
| `opcode` | `i64` |
| `aa` | `bool` |
| `tc` | `bool` |
| `rd` | `bool` |
| `ra` | `bool` |
| `rcode` | `i64` |
| `qdcount` | `i64` |
| `ancount` | `i64` |
| `nscount` | `i64` |
| `arcount` | `i64` |
| `questions` | `Vec<DnsQuestion>` |

*Defined at line 1048*

---

## dns_read_name

The offset after a DNS name at `off` of message `m` (offset 0 = the DNS
header), or -1 when malformed; the name text goes into `name`.
Compression pointers are followed (at most 64); labels join with '.'
and the root name is ".".

```fides
pub fn dns_read_name(m: &Vec<i64>, off: i64, name: &mut str) -> i64 {
```

**Parameters:**

- `m`: `&Vec<i64>`
- `off`: `i64`
- `name`: `&mut str`

**Returns:** `i64`

*Defined at line 1069*

---

## dns_decode

Decode the header and questions of a DNS message (a UDP payload).

```fides
pub fn dns_decode(m: &Vec<i64>) -> DnsHdr {
```

**Parameters:**

- `m`: `&Vec<i64>`

**Returns:** `DnsHdr`

*Defined at line 1115*

---

## dns_encode_name

A DNS name in wire form (no compression).

```fides
pub fn dns_encode_name(name: str) -> Vec<i64> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1154*

---

## dns_build

A DNS message of a header and its questions (counts from `d`, except
qdcount which is the number of questions).

```fides
pub fn dns_build(d: &DnsHdr) -> Vec<i64> {
```

**Parameters:**

- `d`: `&DnsHdr`

**Returns:** `Vec<i64>`

*Defined at line 1174*

---

## dns_query_new

A recursive query for one name (id, qtype; class IN).

```fides
pub fn dns_query_new(id: i64, name: str, qtype: i64) -> DnsHdr {
```

**Parameters:**

- `id`: `i64`
- `name`: `str`
- `qtype`: `i64`

**Returns:** `DnsHdr`

*Defined at line 1211*

---

## pkt_dissect

One summary line per layer of an Ethernet frame.

```fides
pub fn pkt_dissect(b: Vec<i64>) -> Vec<str> {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `Vec<str>`

*Defined at line 1274*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
