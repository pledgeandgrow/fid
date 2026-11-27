[All modules](index.md)

# dhcp -- Fides Documentation

## Contents

- [DHCP_DISCOVER:](#DHCP_DISCOVER:) (const)
- [DHCP_OFFER:](#DHCP_OFFER:) (const)
- [DHCP_REQUEST:](#DHCP_REQUEST:) (const)
- [DHCP_DECLINE:](#DHCP_DECLINE:) (const)
- [DHCP_ACK:](#DHCP_ACK:) (const)
- [DHCP_NAK:](#DHCP_NAK:) (const)
- [DHCP_RELEASE:](#DHCP_RELEASE:) (const)
- [DHCP_INFORM:](#DHCP_INFORM:) (const)
- [OPT_PAD:](#OPT_PAD:) (const)
- [OPT_SUBNET:](#OPT_SUBNET:) (const)
- [OPT_ROUTER:](#OPT_ROUTER:) (const)
- [OPT_DNS:](#OPT_DNS:) (const)
- [OPT_HOSTNAME:](#OPT_HOSTNAME:) (const)
- [OPT_DOMAIN:](#OPT_DOMAIN:) (const)
- [OPT_REQUESTED_IP:](#OPT_REQUESTED_IP:) (const)
- [OPT_LEASE:](#OPT_LEASE:) (const)
- [OPT_MSG_TYPE:](#OPT_MSG_TYPE:) (const)
- [OPT_SERVER_ID:](#OPT_SERVER_ID:) (const)
- [OPT_PARAM_LIST:](#OPT_PARAM_LIST:) (const)
- [OPT_MESSAGE:](#OPT_MESSAGE:) (const)
- [OPT_T1:](#OPT_T1:) (const)
- [OPT_T2:](#OPT_T2:) (const)
- [OPT_CLIENT_ID:](#OPT_CLIENT_ID:) (const)
- [OPT_END:](#OPT_END:) (const)
- [DHCP_SERVER_PORT:](#DHCP_SERVER_PORT:) (const)
- [DHCP_CLIENT_PORT:](#DHCP_CLIENT_PORT:) (const)
- [DHCP_BROADCAST:](#DHCP_BROADCAST:) (const)
- [dhcp_type_name](#dhcp_type_name) (function)
- [DhcpMsg](#DhcpMsg) (struct)
- [dhcp_msg_new](#dhcp_msg_new) (function)
- [dhcp_opt_set](#dhcp_opt_set) (function)
- [dhcp_opt_u8](#dhcp_opt_u8) (function)
- [dhcp_opt_u32](#dhcp_opt_u32) (function)
- [dhcp_opt_str](#dhcp_opt_str) (function)
- [dhcp_opt](#dhcp_opt) (function)
- [dhcp_has](#dhcp_has) (function)
- [dhcp_opt_get_u32](#dhcp_opt_get_u32) (function)
- [dhcp_opt_ips](#dhcp_opt_ips) (function)
- [dhcp_msg_type](#dhcp_msg_type) (function)
- [dhcp_encode](#dhcp_encode) (function)
- [dhcp_decode](#dhcp_decode) (function)
- [DHCPC_INIT:](#DHCPC_INIT:) (const)
- [DHCPC_SELECTING:](#DHCPC_SELECTING:) (const)
- [DHCPC_REQUESTING:](#DHCPC_REQUESTING:) (const)
- [DHCPC_BOUND:](#DHCPC_BOUND:) (const)
- [DHCPC_RENEWING:](#DHCPC_RENEWING:) (const)
- [DHCPC_REBINDING:](#DHCPC_REBINDING:) (const)
- [dhcpc_state_name](#dhcpc_state_name) (function)
- [DhcpClient](#DhcpClient) (struct)
- [dhcpc_new](#dhcpc_new) (function)
- [dhcpc_start](#dhcpc_start) (function)
- [dhcpc_input](#dhcpc_input) (function)
- [dhcpc_tick](#dhcpc_tick) (function)
- [dhcpc_release](#dhcpc_release) (function)
- [dhcpc_decline](#dhcpc_decline) (function)
- [DhcpLease](#DhcpLease) (struct)
- [DhcpServer](#DhcpServer) (struct)
- [dhcps_new](#dhcps_new) (function)
- [DhcpOut](#DhcpOut) (struct)
- [dhcps_handle](#dhcps_handle) (function)
- [dhcps_active](#dhcps_active) (function)
- [dhcpc_poll](#dhcpc_poll) (function)
- [dhcps_poll](#dhcps_poll) (function)
- [dhcpc_apply](#dhcpc_apply) (function)

## DHCP_DISCOVER:

```fides
pub const DHCP_DISCOVER: i64 = 1;
```

*Defined at line 29*

---

## DHCP_OFFER:

```fides
pub const DHCP_OFFER: i64 = 2;
```

*Defined at line 30*

---

## DHCP_REQUEST:

```fides
pub const DHCP_REQUEST: i64 = 3;
```

*Defined at line 31*

---

## DHCP_DECLINE:

```fides
pub const DHCP_DECLINE: i64 = 4;
```

*Defined at line 32*

---

## DHCP_ACK:

```fides
pub const DHCP_ACK: i64 = 5;
```

*Defined at line 33*

---

## DHCP_NAK:

```fides
pub const DHCP_NAK: i64 = 6;
```

*Defined at line 34*

---

## DHCP_RELEASE:

```fides
pub const DHCP_RELEASE: i64 = 7;
```

*Defined at line 35*

---

## DHCP_INFORM:

```fides
pub const DHCP_INFORM: i64 = 8;
```

*Defined at line 36*

---

## OPT_PAD:

```fides
pub const OPT_PAD: i64 = 0;
```

*Defined at line 38*

---

## OPT_SUBNET:

```fides
pub const OPT_SUBNET: i64 = 1;
```

*Defined at line 39*

---

## OPT_ROUTER:

```fides
pub const OPT_ROUTER: i64 = 3;
```

*Defined at line 40*

---

## OPT_DNS:

```fides
pub const OPT_DNS: i64 = 6;
```

*Defined at line 41*

---

## OPT_HOSTNAME:

```fides
pub const OPT_HOSTNAME: i64 = 12;
```

*Defined at line 42*

---

## OPT_DOMAIN:

```fides
pub const OPT_DOMAIN: i64 = 15;
```

*Defined at line 43*

---

## OPT_REQUESTED_IP:

```fides
pub const OPT_REQUESTED_IP: i64 = 50;
```

*Defined at line 44*

---

## OPT_LEASE:

```fides
pub const OPT_LEASE: i64 = 51;
```

*Defined at line 45*

---

## OPT_MSG_TYPE:

```fides
pub const OPT_MSG_TYPE: i64 = 53;
```

*Defined at line 46*

---

## OPT_SERVER_ID:

```fides
pub const OPT_SERVER_ID: i64 = 54;
```

*Defined at line 47*

---

## OPT_PARAM_LIST:

```fides
pub const OPT_PARAM_LIST: i64 = 55;
```

*Defined at line 48*

---

## OPT_MESSAGE:

```fides
pub const OPT_MESSAGE: i64 = 56;
```

*Defined at line 49*

---

## OPT_T1:

```fides
pub const OPT_T1: i64 = 58;
```

*Defined at line 50*

---

## OPT_T2:

```fides
pub const OPT_T2: i64 = 59;
```

*Defined at line 51*

---

## OPT_CLIENT_ID:

```fides
pub const OPT_CLIENT_ID: i64 = 61;
```

*Defined at line 52*

---

## OPT_END:

```fides
pub const OPT_END: i64 = 255;
```

*Defined at line 53*

---

## DHCP_SERVER_PORT:

```fides
pub const DHCP_SERVER_PORT: i64 = 67;
```

*Defined at line 55*

---

## DHCP_CLIENT_PORT:

```fides
pub const DHCP_CLIENT_PORT: i64 = 68;
```

*Defined at line 56*

---

## DHCP_BROADCAST:

```fides
pub const DHCP_BROADCAST: i64 = 0xFFFFFFFF;
```

*Defined at line 57*

---

## dhcp_type_name

```fides
pub fn dhcp_type_name(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 59*

---

## DhcpMsg

```fides
pub struct DhcpMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `op` | `i64` |
| `xid` | `i64` |
| `secs` | `i64` |
| `flags` | `i64` |
| `ciaddr` | `i64` |
| `yiaddr` | `i64` |
| `siaddr` | `i64` |
| `giaddr` | `i64` |
| `chaddr` | `Vec<i64>` |
| `opt_codes` | `Vec<i64>` |
| `opt_vals` | `Vec<Vec<i64>>` |
| `ok` | `bool` |

*Defined at line 89*

---

## dhcp_msg_new

```fides
pub fn dhcp_msg_new(op: i64, xid: i64, mac: Vec<i64>) -> DhcpMsg {
```

**Parameters:**

- `op`: `i64`
- `xid`: `i64`
- `mac`: `Vec<i64>`

**Returns:** `DhcpMsg`

*Defined at line 105*

---

## dhcp_opt_set

```fides
pub fn dhcp_opt_set(m: &mut DhcpMsg, code: i64, val: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut DhcpMsg`
- `code`: `i64`
- `val`: `Vec<i64>`

*Defined at line 111*

---

## dhcp_opt_u8

```fides
pub fn dhcp_opt_u8(m: &mut DhcpMsg, code: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut DhcpMsg`
- `code`: `i64`
- `v`: `i64`

*Defined at line 124*

---

## dhcp_opt_u32

```fides
pub fn dhcp_opt_u32(m: &mut DhcpMsg, code: i64, v: i64) {
```

**Parameters:**

- `m`: `&mut DhcpMsg`
- `code`: `i64`
- `v`: `i64`

*Defined at line 130*

---

## dhcp_opt_str

```fides
pub fn dhcp_opt_str(m: &mut DhcpMsg, code: i64, s: str) {
```

**Parameters:**

- `m`: `&mut DhcpMsg`
- `code`: `i64`
- `s`: `str`

*Defined at line 136*

---

## dhcp_opt

Option value, empty when absent.

```fides
pub fn dhcp_opt(m: &DhcpMsg, code: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&DhcpMsg`
- `code`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 141*

---

## dhcp_has

```fides
pub fn dhcp_has(m: &DhcpMsg, code: i64) -> bool {
```

**Parameters:**

- `m`: `&DhcpMsg`
- `code`: `i64`

**Returns:** `bool`

*Defined at line 152*

---

## dhcp_opt_get_u32

First 4 bytes of an option as a 32-bit number, `missing` when absent.

```fides
pub fn dhcp_opt_get_u32(m: &DhcpMsg, code: i64, missing: i64) -> i64 {
```

**Parameters:**

- `m`: `&DhcpMsg`
- `code`: `i64`
- `missing`: `i64`

**Returns:** `i64`

*Defined at line 157*

---

## dhcp_opt_ips

Addresses carried by an option (router, DNS lists).

```fides
pub fn dhcp_opt_ips(m: &DhcpMsg, code: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&DhcpMsg`
- `code`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 166*

---

## dhcp_msg_type

```fides
pub fn dhcp_msg_type(m: &DhcpMsg) -> i64 {
```

**Parameters:**

- `m`: `&DhcpMsg`

**Returns:** `i64`

*Defined at line 177*

---

## dhcp_encode

Wire bytes: 236-byte BOOTP header, cookie, options, END, padded to
300 bytes. Options longer than 255 bytes are split (RFC 3396).

```fides
pub fn dhcp_encode(m: &DhcpMsg) -> Vec<i64> {
```

**Parameters:**

- `m`: `&DhcpMsg`

**Returns:** `Vec<i64>`

*Defined at line 187*

---

## dhcp_decode

Parse a message (ok = false when malformed or not DHCP). Repeated
options are concatenated (RFC 3396).

```fides
pub fn dhcp_decode(b: Vec<i64>) -> DhcpMsg {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `DhcpMsg`

*Defined at line 250*

---

## DHCPC_INIT:

```fides
pub const DHCPC_INIT: i64 = 0;
```

*Defined at line 303*

---

## DHCPC_SELECTING:

```fides
pub const DHCPC_SELECTING: i64 = 1;
```

*Defined at line 304*

---

## DHCPC_REQUESTING:

```fides
pub const DHCPC_REQUESTING: i64 = 2;
```

*Defined at line 305*

---

## DHCPC_BOUND:

```fides
pub const DHCPC_BOUND: i64 = 3;
```

*Defined at line 306*

---

## DHCPC_RENEWING:

```fides
pub const DHCPC_RENEWING: i64 = 4;
```

*Defined at line 307*

---

## DHCPC_REBINDING:

```fides
pub const DHCPC_REBINDING: i64 = 5;
```

*Defined at line 308*

---

## dhcpc_state_name

```fides
pub fn dhcpc_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 310*

---

## DhcpClient

```fides
pub struct DhcpClient {
```

**Fields:**

| Name | Type |
|------|------|
| `mac` | `Vec<i64>` |
| `hostname` | `str` |
| `state` | `i64` |
| `xid` | `i64` |
| `started` | `i64` |
| `retry_at` | `i64` |
| `backoff` | `i64` |
| `offered` | `i64` |
| `server` | `i64` |
| `ip` | `i64` |
| `mask` | `i64` |
| `router` | `i64` |
| `dns` | `Vec<i64>` |
| `lease_s` | `i64` |
| `bound_at` | `i64` |
| `t1_at` | `i64` |
| `t2_at` | `i64` |
| `expire_at` | `i64` |
| `out_dst` | `Vec<i64>` |
| `out_msgs` | `Vec<Vec<i64>>` |
| `log` | `Vec<str>` |

*Defined at line 329*

---

## dhcpc_new

```fides
pub fn dhcpc_new(mac: Vec<i64>, xid: i64, hostname: str) -> DhcpClient {
```

**Parameters:**

- `mac`: `Vec<i64>`
- `xid`: `i64`
- `hostname`: `str`

**Returns:** `DhcpClient`

*Defined at line 353*

---

## dhcpc_start

Begin (or restart) acquisition: DISCOVER, then SELECTING.

```fides
pub fn dhcpc_start(c: &mut DhcpClient, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `now`: `i64`

*Defined at line 423*

---

## dhcpc_input

Handle a message from a server.

```fides
pub fn dhcpc_input(c: &mut DhcpClient, bytes: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `bytes`: `Vec<i64>`
- `now`: `i64`

*Defined at line 434*

---

## dhcpc_tick

Timers: retransmissions with backoff, T1 -> RENEWING, T2 ->
REBINDING, lease end -> back to INIT and a new DISCOVER.

```fides
pub fn dhcpc_tick(c: &mut DhcpClient, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `now`: `i64`

*Defined at line 487*

---

## dhcpc_release

Give the address back (unicast RELEASE) and return to INIT.

```fides
pub fn dhcpc_release(c: &mut DhcpClient, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `now`: `i64`

*Defined at line 532*

---

## dhcpc_decline

Refuse the offered/acked address (e.g. ARP showed it in use), restart.

```fides
pub fn dhcpc_decline(c: &mut DhcpClient, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `now`: `i64`

*Defined at line 546*

---

## DhcpLease

```fides
pub struct DhcpLease {
```

**Fields:**

| Name | Type |
|------|------|
| `mac` | `Vec<i64>` |
| `ip` | `i64` |
| `expires` | `i64` |
| `bound` | `bool` |
| `declined` | `bool` |

*Defined at line 556*

---

## DhcpServer

```fides
pub struct DhcpServer {
```

**Fields:**

| Name | Type |
|------|------|
| `ip` | `i64` |
| `mask` | `i64` |
| `router` | `i64` |
| `dns` | `Vec<i64>` |
| `domain` | `str` |
| `pool_start` | `i64` |
| `pool_end` | `i64` |
| `lease_s` | `i64` |
| `offer_hold` | `i64` |
| `leases` | `Vec<DhcpLease>` |
| `log` | `Vec<str>` |

*Defined at line 565*

---

## dhcps_new

```fides
pub fn dhcps_new(ip: i64, mask: i64, pool_start: i64, pool_end: i64, lease_s: i64) -> DhcpServer {
```

**Parameters:**

- `ip`: `i64`
- `mask`: `i64`
- `pool_start`: `i64`
- `pool_end`: `i64`
- `lease_s`: `i64`

**Returns:** `DhcpServer`

*Defined at line 579*

---

## DhcpOut

```fides
pub struct DhcpOut {
```

**Fields:**

| Name | Type |
|------|------|
| `dst` | `i64` |
| `bytes` | `Vec<i64>` |

*Defined at line 703*

---

## dhcps_handle

Handle one client message; the reply to send (dst 0: none).

```fides
pub fn dhcps_handle(s: &mut DhcpServer, bytes: Vec<i64>, now: i64) -> DhcpOut {
```

**Parameters:**

- `s`: `&mut DhcpServer`
- `bytes`: `Vec<i64>`
- `now`: `i64`

**Returns:** `DhcpOut`

*Defined at line 725*

---

## dhcps_active

Leases bound and not expired.

```fides
pub fn dhcps_active(s: &DhcpServer, now: i64) -> i64 {
```

**Parameters:**

- `s`: `&DhcpServer`
- `now`: `i64`

**Returns:** `i64`

*Defined at line 794*

---

## dhcpc_poll

Client side: feed datagrams from the port-68 socket, run the timers,
send what the client queued.

```fides
pub fn dhcpc_poll(c: &mut DhcpClient, net: &mut NetStack, sock: i64, now: i64) {
```

**Parameters:**

- `c`: `&mut DhcpClient`
- `net`: `&mut NetStack`
- `sock`: `i64`
- `now`: `i64`

*Defined at line 811*

---

## dhcps_poll

Server side: answer every queued request on the port-67 socket.

```fides
pub fn dhcps_poll(s: &mut DhcpServer, net: &mut NetStack, sock: i64, now: i64) {
```

**Parameters:**

- `s`: `&mut DhcpServer`
- `net`: `&mut NetStack`
- `sock`: `i64`
- `now`: `i64`

*Defined at line 830*

---

## dhcpc_apply

Configure a stack with the client's lease (address 0 when unbound).

```fides
pub fn dhcpc_apply(c: &DhcpClient, net: &mut NetStack) {
```

**Parameters:**

- `c`: `&DhcpClient`
- `net`: `&mut NetStack`

*Defined at line 842*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
