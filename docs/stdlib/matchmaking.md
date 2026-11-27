[All modules](index.md)

# matchmaking -- Fides Documentation

## Contents

- [MmLobby](#MmLobby) (struct)
- [MmLobbies](#MmLobbies) (struct)
- [MmLeave](#MmLeave) (struct)
- [mm_lobbies_new](#mm_lobbies_new) (function)
- [mm_lobby_of](#mm_lobby_of) (function)
- [mm_lobby_create](#mm_lobby_create) (function)
- [mm_lobby_get](#mm_lobby_get) (function)
- [mm_lobby_join](#mm_lobby_join) (function)
- [mm_lobby_leave](#mm_lobby_leave) (function)
- [mm_lobby_kick](#mm_lobby_kick) (function)
- [mm_lobby_set_open](#mm_lobby_set_open) (function)
- [mm_lobby_list_joinable](#mm_lobby_list_joinable) (function)
- [elo_expected](#elo_expected) (function)
- [elo_update](#elo_update) (function)
- [Glicko2](#Glicko2) (struct)
- [GLICKO2_SCALE:](#GLICKO2_SCALE:) (const)
- [glicko2_new](#glicko2_new) (function)
- [glicko2_update](#glicko2_update) (function)
- [glicko2_expected](#glicko2_expected) (function)
- [MmEntry](#MmEntry) (struct)
- [MmMatch](#MmMatch) (struct)
- [MmQueue](#MmQueue) (struct)
- [mm_queue_new](#mm_queue_new) (function)
- [mm_queue_add](#mm_queue_add) (function)
- [mm_queue_remove](#mm_queue_remove) (function)
- [mm_queue_window](#mm_queue_window) (function)
- [mm_queue_tick](#mm_queue_tick) (function)
- [mm_sha1](#mm_sha1) (function)
- [mm_hmac_sha1](#mm_hmac_sha1) (function)
- [mm_crc32](#mm_crc32) (function)
- [STUN_MAGIC:](#STUN_MAGIC:) (const)
- [STUN_FINGERPRINT_XOR:](#STUN_FINGERPRINT_XOR:) (const)
- [STUN_METHOD_BINDING:](#STUN_METHOD_BINDING:) (const)
- [STUN_CLASS_REQUEST:](#STUN_CLASS_REQUEST:) (const)
- [STUN_CLASS_INDICATION:](#STUN_CLASS_INDICATION:) (const)
- [STUN_CLASS_SUCCESS:](#STUN_CLASS_SUCCESS:) (const)
- [STUN_CLASS_ERROR:](#STUN_CLASS_ERROR:) (const)
- [STUN_ATTR_MAPPED_ADDRESS:](#STUN_ATTR_MAPPED_ADDRESS:) (const)
- [STUN_ATTR_USERNAME:](#STUN_ATTR_USERNAME:) (const)
- [STUN_ATTR_MESSAGE_INTEGRITY:](#STUN_ATTR_MESSAGE_INTEGRITY:) (const)
- [STUN_ATTR_ERROR_CODE:](#STUN_ATTR_ERROR_CODE:) (const)
- [STUN_ATTR_UNKNOWN_ATTRIBUTES:](#STUN_ATTR_UNKNOWN_ATTRIBUTES:) (const)
- [STUN_ATTR_REALM:](#STUN_ATTR_REALM:) (const)
- [STUN_ATTR_NONCE:](#STUN_ATTR_NONCE:) (const)
- [STUN_ATTR_XOR_MAPPED_ADDRESS:](#STUN_ATTR_XOR_MAPPED_ADDRESS:) (const)
- [STUN_ATTR_PRIORITY:](#STUN_ATTR_PRIORITY:) (const)
- [STUN_ATTR_USE_CANDIDATE:](#STUN_ATTR_USE_CANDIDATE:) (const)
- [STUN_ATTR_SOFTWARE:](#STUN_ATTR_SOFTWARE:) (const)
- [STUN_ATTR_ALTERNATE_SERVER:](#STUN_ATTR_ALTERNATE_SERVER:) (const)
- [STUN_ATTR_FINGERPRINT:](#STUN_ATTR_FINGERPRINT:) (const)
- [STUN_ATTR_ICE_CONTROLLED:](#STUN_ATTR_ICE_CONTROLLED:) (const)
- [STUN_ATTR_ICE_CONTROLLING:](#STUN_ATTR_ICE_CONTROLLING:) (const)
- [StunAttr](#StunAttr) (struct)
- [StunMsg](#StunMsg) (struct)
- [StunAddr](#StunAddr) (struct)
- [stun_type](#stun_type) (function)
- [stun_type_method](#stun_type_method) (function)
- [stun_type_class](#stun_type_class) (function)
- [stun_msg_new](#stun_msg_new) (function)
- [stun_add_attr](#stun_add_attr) (function)
- [stun_find_attr](#stun_find_attr) (function)
- [stun_encode](#stun_encode) (function)
- [stun_encode_padded](#stun_encode_padded) (function)
- [stun_is_message](#stun_is_message) (function)
- [stun_decode](#stun_decode) (function)
- [stun_check_integrity](#stun_check_integrity) (function)
- [stun_check_fingerprint](#stun_check_fingerprint) (function)
- [stun_xor_address_encode](#stun_xor_address_encode) (function)
- [stun_address_decode](#stun_address_decode) (function)
- [stun_addr_str](#stun_addr_str) (function)
- [stun_binding_request](#stun_binding_request) (function)
- [stun_binding_response](#stun_binding_response) (function)
- [stun_error_code_encode](#stun_error_code_encode) (function)
- [stun_error_code_decode](#stun_error_code_decode) (function)
- [stun_response_address](#stun_response_address) (function)
- [stun_addr_eq](#stun_addr_eq) (function)
- [NAT_NONE:](#NAT_NONE:) (const)
- [NAT_ENDPOINT_INDEPENDENT:](#NAT_ENDPOINT_INDEPENDENT:) (const)
- [NAT_ADDRESS_DEPENDENT:](#NAT_ADDRESS_DEPENDENT:) (const)
- [NAT_ADDRESS_PORT_DEPENDENT:](#NAT_ADDRESS_PORT_DEPENDENT:) (const)
- [nat_mapping_behaviour](#nat_mapping_behaviour) (function)
- [nat_behaviour_name](#nat_behaviour_name) (function)
- [nat_hole_punch_likely](#nat_hole_punch_likely) (function)
- [ICE_TYPE_HOST:](#ICE_TYPE_HOST:) (const)
- [ICE_TYPE_PEER_REFLEXIVE:](#ICE_TYPE_PEER_REFLEXIVE:) (const)
- [ICE_TYPE_SERVER_REFLEXIVE:](#ICE_TYPE_SERVER_REFLEXIVE:) (const)
- [ICE_TYPE_RELAYED:](#ICE_TYPE_RELAYED:) (const)
- [ice_candidate_priority](#ice_candidate_priority) (function)
- [ice_pair_priority](#ice_pair_priority) (function)

## MmLobby

```fides
pub struct MmLobby {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `host` | `i64` |
| `members` | `Vec<i64>` |
| `max_players` | `i64` |
| `open` | `bool` |

*Defined at line 36*

---

## MmLobbies

```fides
pub struct MmLobbies {
```

**Fields:**

| Name | Type |
|------|------|
| `lobbies` | `Vec<MmLobby>` |
| `next_id` | `i64` |

*Defined at line 46*

---

## MmLeave

What a player leaving did to its lobby.

```fides
pub struct MmLeave {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `closed` | `bool` |
| `host_changed` | `bool` |
| `new_host` | `i64` |

*Defined at line 52*

---

## mm_lobbies_new

```fides
pub fn mm_lobbies_new() -> MmLobbies {
```

**Returns:** `MmLobbies`

*Defined at line 59*

---

## mm_lobby_of

The lobby a player is in, or -1.

```fides
pub fn mm_lobby_of(l: &MmLobbies, player: i64) -> i64 {
```

**Parameters:**

- `l`: `&MmLobbies`
- `player`: `i64`

**Returns:** `i64`

*Defined at line 77*

---

## mm_lobby_create

Create a lobby hosted by `host`; returns its id, or -1 when the host is
already in a lobby or max_players < 1.

```fides
pub fn mm_lobby_create(l: &mut MmLobbies, host: i64, name: str, max_players: i64) -> i64 {
```

**Parameters:**

- `l`: `&mut MmLobbies`
- `host`: `i64`
- `name`: `str`
- `max_players`: `i64`

**Returns:** `i64`

*Defined at line 95*

---

## mm_lobby_get

```fides
pub fn mm_lobby_get(l: &MmLobbies, id: i64) -> Option<MmLobby> {
```

**Parameters:**

- `l`: `&MmLobbies`
- `id`: `i64`

**Returns:** `Option<MmLobby>`

*Defined at line 109*

---

## mm_lobby_join

Join a lobby. Err says why not.

```fides
pub fn mm_lobby_join(l: &mut MmLobbies, id: i64, player: i64) -> Result<i64, str> {
```

**Parameters:**

- `l`: `&mut MmLobbies`
- `id`: `i64`
- `player`: `i64`

**Returns:** `Result<i64, str>`

*Defined at line 118*

---

## mm_lobby_leave

Leave a lobby. When the host leaves, the longest-present member becomes
host; when the last member leaves, the lobby closes and is removed.

```fides
pub fn mm_lobby_leave(l: &mut MmLobbies, id: i64, player: i64) -> MmLeave {
```

**Parameters:**

- `l`: `&mut MmLobbies`
- `id`: `i64`
- `player`: `i64`

**Returns:** `MmLeave`

*Defined at line 144*

---

## mm_lobby_kick

The host removes a member (not itself).

```fides
pub fn mm_lobby_kick(l: &mut MmLobbies, id: i64, by: i64, player: i64) -> Result<i64, str> {
```

**Parameters:**

- `l`: `&mut MmLobbies`
- `id`: `i64`
- `by`: `i64`
- `player`: `i64`

**Returns:** `Result<i64, str>`

*Defined at line 181*

---

## mm_lobby_set_open

Open or close a lobby to new players (host only).

```fides
pub fn mm_lobby_set_open(l: &mut MmLobbies, id: i64, by: i64, open: bool) -> bool {
```

**Parameters:**

- `l`: `&mut MmLobbies`
- `id`: `i64`
- `by`: `i64`
- `open`: `bool`

**Returns:** `bool`

*Defined at line 200*

---

## mm_lobby_list_joinable

Ids of open lobbies with a free slot, in creation order.

```fides
pub fn mm_lobby_list_joinable(l: &MmLobbies) -> Vec<i64> {
```

**Parameters:**

- `l`: `&MmLobbies`

**Returns:** `Vec<i64>`

*Defined at line 212*

---

## elo_expected

Expected score of a player rated ra against rb.

```fides
pub fn elo_expected(ra: f64, rb: f64) -> f64 {
```

**Parameters:**

- `ra`: `f64`
- `rb`: `f64`

**Returns:** `f64`

*Defined at line 228*

---

## elo_update

New rating after scoring `score` (1 win, 0.5 draw, 0 loss) against rb.

```fides
pub fn elo_update(ra: f64, rb: f64, score: f64, k: f64) -> f64 {
```

**Parameters:**

- `ra`: `f64`
- `rb`: `f64`
- `score`: `f64`
- `k`: `f64`

**Returns:** `f64`

*Defined at line 233*

---

## Glicko2

```fides
pub struct Glicko2 {
```

**Fields:**

| Name | Type |
|------|------|
| `rating` | `f64` |
| `rd` | `f64` |
| `vol` | `f64` |

*Defined at line 239*

---

## GLICKO2_SCALE:

```fides
pub const GLICKO2_SCALE: f64 = 173.7178;
```

*Defined at line 245*

---

## glicko2_new

```fides
pub fn glicko2_new() -> Glicko2 {
```

**Returns:** `Glicko2`

*Defined at line 247*

---

## glicko2_update

One rating period: `opps` the opponents' ratings before the period,
`scores` the results against each (1, 0.5, 0). With no games the RD
grows by the volatility. `tau` constrains volatility change (0.3..1.2).

```fides
pub fn glicko2_update(p: Glicko2, opps: &Vec<Glicko2>, scores: &Vec<f64>, tau: f64) -> Glicko2 {
```

**Parameters:**

- `p`: `Glicko2`
- `opps`: `&Vec<Glicko2>`
- `scores`: `&Vec<f64>`
- `tau`: `f64`

**Returns:** `Glicko2`

*Defined at line 271*

---

## glicko2_expected

Glicko-2 expected score of a against b (both uncertainties).

```fides
pub fn glicko2_expected(a: Glicko2, b: Glicko2) -> f64 {
```

**Parameters:**

- `a`: `Glicko2`
- `b`: `Glicko2`

**Returns:** `f64`

*Defined at line 335*

---

## MmEntry

```fides
pub struct MmEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `player` | `i64` |
| `rating` | `f64` |
| `since` | `i64` |

*Defined at line 345*

---

## MmMatch

```fides
pub struct MmMatch {
```

**Fields:**

| Name | Type |
|------|------|
| `players` | `Vec<i64>` |
| `team_a` | `Vec<i64>` |
| `team_b` | `Vec<i64>` |
| `spread` | `f64` |
| `balance` | `f64` |

*Defined at line 351*

---

## MmQueue

Players wait until a group of `match_size` fits inside every member's
window: `base + widen * waited`, at most `max_window` rating points.

```fides
pub struct MmQueue {
```

**Fields:**

| Name | Type |
|------|------|
| `entries` | `Vec<MmEntry>` |
| `match_size` | `i64` |
| `base` | `f64` |
| `widen` | `f64` |
| `max_window` | `f64` |

*Defined at line 363*

---

## mm_queue_new

```fides
pub fn mm_queue_new(match_size: i64, base: f64, widen: f64, max_window: f64) -> MmQueue {
```

**Parameters:**

- `match_size`: `i64`
- `base`: `f64`
- `widen`: `f64`
- `max_window`: `f64`

**Returns:** `MmQueue`

*Defined at line 371*

---

## mm_queue_add

Enqueue; false when the player is already waiting.

```fides
pub fn mm_queue_add(q: &mut MmQueue, player: i64, rating: f64, now: i64) -> bool {
```

**Parameters:**

- `q`: `&mut MmQueue`
- `player`: `i64`
- `rating`: `f64`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 378*

---

## mm_queue_remove

```fides
pub fn mm_queue_remove(q: &mut MmQueue, player: i64) -> bool {
```

**Parameters:**

- `q`: `&mut MmQueue`
- `player`: `i64`

**Returns:** `bool`

*Defined at line 392*

---

## mm_queue_window

A waiting player's current window.

```fides
pub fn mm_queue_window(q: &MmQueue, e: MmEntry, now: i64) -> f64 {
```

**Parameters:**

- `q`: `&MmQueue`
- `e`: `MmEntry`
- `now`: `i64`

**Returns:** `f64`

*Defined at line 405*

---

## mm_queue_tick

Form every match possible now. Players are taken in rating order; the
group with the longest waiter goes first among overlapping candidates.
Teams are a snake draft (best to A, next two to B, ...).

```fides
pub fn mm_queue_tick(q: &mut MmQueue, now: i64) -> Vec<MmMatch> {
```

**Parameters:**

- `q`: `&mut MmQueue`
- `now`: `i64`

**Returns:** `Vec<MmMatch>`

*Defined at line 416*

---

## mm_sha1

```fides
pub fn mm_sha1(msg: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msg`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 496*

---

## mm_hmac_sha1

HMAC-SHA1 (RFC 2104).

```fides
pub fn mm_hmac_sha1(key: &Vec<i64>, msg: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `&Vec<i64>`
- `msg`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 588*

---

## mm_crc32

CRC-32 (IEEE 802.3, reflected, as zlib) of bytes[0..n).

```fides
pub fn mm_crc32(bytes: &Vec<i64>, n: i64) -> i64 {
```

**Parameters:**

- `bytes`: `&Vec<i64>`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 627*

---

## STUN_MAGIC:

```fides
pub const STUN_MAGIC: i64 = 0x2112A442;
```

*Defined at line 656*

---

## STUN_FINGERPRINT_XOR:

```fides
pub const STUN_FINGERPRINT_XOR: i64 = 0x5354554E;
```

*Defined at line 657*

---

## STUN_METHOD_BINDING:

```fides
pub const STUN_METHOD_BINDING: i64 = 1;
```

*Defined at line 658*

---

## STUN_CLASS_REQUEST:

```fides
pub const STUN_CLASS_REQUEST: i64 = 0;
```

*Defined at line 659*

---

## STUN_CLASS_INDICATION:

```fides
pub const STUN_CLASS_INDICATION: i64 = 1;
```

*Defined at line 660*

---

## STUN_CLASS_SUCCESS:

```fides
pub const STUN_CLASS_SUCCESS: i64 = 2;
```

*Defined at line 661*

---

## STUN_CLASS_ERROR:

```fides
pub const STUN_CLASS_ERROR: i64 = 3;
```

*Defined at line 662*

---

## STUN_ATTR_MAPPED_ADDRESS:

```fides
pub const STUN_ATTR_MAPPED_ADDRESS: i64 = 0x0001;
```

*Defined at line 664*

---

## STUN_ATTR_USERNAME:

```fides
pub const STUN_ATTR_USERNAME: i64 = 0x0006;
```

*Defined at line 665*

---

## STUN_ATTR_MESSAGE_INTEGRITY:

```fides
pub const STUN_ATTR_MESSAGE_INTEGRITY: i64 = 0x0008;
```

*Defined at line 666*

---

## STUN_ATTR_ERROR_CODE:

```fides
pub const STUN_ATTR_ERROR_CODE: i64 = 0x0009;
```

*Defined at line 667*

---

## STUN_ATTR_UNKNOWN_ATTRIBUTES:

```fides
pub const STUN_ATTR_UNKNOWN_ATTRIBUTES: i64 = 0x000A;
```

*Defined at line 668*

---

## STUN_ATTR_REALM:

```fides
pub const STUN_ATTR_REALM: i64 = 0x0014;
```

*Defined at line 669*

---

## STUN_ATTR_NONCE:

```fides
pub const STUN_ATTR_NONCE: i64 = 0x0015;
```

*Defined at line 670*

---

## STUN_ATTR_XOR_MAPPED_ADDRESS:

```fides
pub const STUN_ATTR_XOR_MAPPED_ADDRESS: i64 = 0x0020;
```

*Defined at line 671*

---

## STUN_ATTR_PRIORITY:

```fides
pub const STUN_ATTR_PRIORITY: i64 = 0x0024;
```

*Defined at line 672*

---

## STUN_ATTR_USE_CANDIDATE:

```fides
pub const STUN_ATTR_USE_CANDIDATE: i64 = 0x0025;
```

*Defined at line 673*

---

## STUN_ATTR_SOFTWARE:

```fides
pub const STUN_ATTR_SOFTWARE: i64 = 0x8022;
```

*Defined at line 674*

---

## STUN_ATTR_ALTERNATE_SERVER:

```fides
pub const STUN_ATTR_ALTERNATE_SERVER: i64 = 0x8023;
```

*Defined at line 675*

---

## STUN_ATTR_FINGERPRINT:

```fides
pub const STUN_ATTR_FINGERPRINT: i64 = 0x8028;
```

*Defined at line 676*

---

## STUN_ATTR_ICE_CONTROLLED:

```fides
pub const STUN_ATTR_ICE_CONTROLLED: i64 = 0x8029;
```

*Defined at line 677*

---

## STUN_ATTR_ICE_CONTROLLING:

```fides
pub const STUN_ATTR_ICE_CONTROLLING: i64 = 0x802A;
```

*Defined at line 678*

---

## StunAttr

```fides
pub struct StunAttr {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `value` | `Vec<i64>` |

*Defined at line 680*

---

## StunMsg

```fides
pub struct StunMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `method` | `i64` |
| `class` | `i64` |
| `txid` | `Vec<i64>` |
| `attrs` | `Vec<StunAttr>` |

*Defined at line 685*

---

## StunAddr

A transport address. family 1 = IPv4 (4 bytes), 2 = IPv6 (16 bytes).

```fides
pub struct StunAddr {
```

**Fields:**

| Name | Type |
|------|------|
| `family` | `i64` |
| `port` | `i64` |
| `ip` | `Vec<i64>` |

*Defined at line 693*

---

## stun_type

The 14-bit message type of a method and class.

```fides
pub fn stun_type(method: i64, class: i64) -> i64 {
```

**Parameters:**

- `method`: `i64`
- `class`: `i64`

**Returns:** `i64`

*Defined at line 700*

---

## stun_type_method

```fides
pub fn stun_type_method(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 704*

---

## stun_type_class

```fides
pub fn stun_type_class(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 708*

---

## stun_msg_new

```fides
pub fn stun_msg_new(method: i64, class: i64, txid: Vec<i64>) -> StunMsg {
```

**Parameters:**

- `method`: `i64`
- `class`: `i64`
- `txid`: `Vec<i64>`

**Returns:** `StunMsg`

*Defined at line 712*

---

## stun_add_attr

```fides
pub fn stun_add_attr(m: &mut StunMsg, kind: i64, value: Vec<i64>) {
```

**Parameters:**

- `m`: `&mut StunMsg`
- `kind`: `i64`
- `value`: `Vec<i64>`

*Defined at line 718*

---

## stun_find_attr

The first attribute of a kind (empty value and false when absent).

```fides
pub fn stun_find_attr(m: &StunMsg, kind: i64) -> Option<Vec<i64>> {
```

**Parameters:**

- `m`: `&StunMsg`
- `kind`: `i64`

**Returns:** `Option<Vec<i64>>`

*Defined at line 725*

---

## stun_encode

Encode a message. With a non-empty `key` a MESSAGE-INTEGRITY attribute
is appended (key = the short-term password, or MD5(user:realm:pass) for
long-term credentials), then FINGERPRINT when asked. Padding is zeros.

```fides
pub fn stun_encode(m: &StunMsg, key: &Vec<i64>, fingerprint: bool) -> Vec<i64> {
```

**Parameters:**

- `m`: `&StunMsg`
- `key`: `&Vec<i64>`
- `fingerprint`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 777*

---

## stun_encode_padded

stun_encode with attribute padding bytes of value `pad` (receivers must
ignore padding; RFC 5769's vectors pad with spaces).

```fides
pub fn stun_encode_padded(m: &StunMsg, key: &Vec<i64>, fingerprint: bool, pad: i64) -> Vec<i64> {
```

**Parameters:**

- `m`: `&StunMsg`
- `key`: `&Vec<i64>`
- `fingerprint`: `bool`
- `pad`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 783*

---

## stun_is_message

Is this datagram a STUN message (RFC 5389 section 7.3: top bits zero,
magic cookie, length a multiple of 4 matching the datagram)?

```fides
pub fn stun_is_message(b: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 817*

---

## stun_decode

Decode a message; attribute values are unpadded.

```fides
pub fn stun_decode(b: &Vec<i64>) -> Result<StunMsg, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<StunMsg, str>`

*Defined at line 826*

---

## stun_check_integrity

Check MESSAGE-INTEGRITY with `key`. False when absent or wrong.

```fides
pub fn stun_check_integrity(b: &Vec<i64>, key: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `key`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 875*

---

## stun_check_fingerprint

Check FINGERPRINT (must be the last attribute). False when absent.

```fides
pub fn stun_check_fingerprint(b: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 901*

---

## stun_xor_address_encode

XOR-MAPPED-ADDRESS value for an address (txid is the message's).

```fides
pub fn stun_xor_address_encode(a: &StunAddr, txid: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `&StunAddr`
- `txid`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 914*

---

## stun_address_decode

Decode XOR-MAPPED-ADDRESS (xored true) or MAPPED-ADDRESS (false).

```fides
pub fn stun_address_decode(v: &Vec<i64>, txid: &Vec<i64>, xored: bool) -> Result<StunAddr, str> {
```

**Parameters:**

- `v`: `&Vec<i64>`
- `txid`: `&Vec<i64>`
- `xored`: `bool`

**Returns:** `Result<StunAddr, str>`

*Defined at line 935*

---

## stun_addr_str

"a.b.c.d:port" or "[v6]:port" (RFC 5952: longest run of two or more
zero groups becomes "::").

```fides
pub fn stun_addr_str(a: &StunAddr) -> str {
```

**Parameters:**

- `a`: `&StunAddr`

**Returns:** `str`

*Defined at line 993*

---

## stun_binding_request

A Binding request; `key` empty for none.

```fides
pub fn stun_binding_request(txid: Vec<i64>, key: Vec<i64>, fingerprint: bool) -> Vec<i64> {
```

**Parameters:**

- `txid`: `Vec<i64>`
- `key`: `Vec<i64>`
- `fingerprint`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1039*

---

## stun_binding_response

A Binding success response telling the client its reflexive address.

```fides
pub fn stun_binding_response(txid: Vec<i64>, mapped: &StunAddr, software: str, key: Vec<i64>, fingerprint: bool) -> Vec<i64> {
```

**Parameters:**

- `txid`: `Vec<i64>`
- `mapped`: `&StunAddr`
- `software`: `str`
- `key`: `Vec<i64>`
- `fingerprint`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 1045*

---

## stun_error_code_encode

An ERROR-CODE value (code 300..699, reason text).

```fides
pub fn stun_error_code_encode(code: i64, reason: str) -> Vec<i64> {
```

**Parameters:**

- `code`: `i64`
- `reason`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1055*

---

## stun_error_code_decode

```fides
pub fn stun_error_code_decode(v: &Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 1070*

---

## stun_response_address

The reflexive address in a Binding success response: XOR-MAPPED-ADDRESS,
or MAPPED-ADDRESS from an old (RFC 3489) server.

```fides
pub fn stun_response_address(b: &Vec<i64>) -> Result<StunAddr, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<StunAddr, str>`

*Defined at line 1079*

---

## stun_addr_eq

```fides
pub fn stun_addr_eq(a: &StunAddr, b: &StunAddr) -> bool {
```

**Parameters:**

- `a`: `&StunAddr`
- `b`: `&StunAddr`

**Returns:** `bool`

*Defined at line 1107*

---

## NAT_NONE:

```fides
pub const NAT_NONE: i64 = 0;
```

*Defined at line 1123*

---

## NAT_ENDPOINT_INDEPENDENT:

```fides
pub const NAT_ENDPOINT_INDEPENDENT: i64 = 1;
```

*Defined at line 1124*

---

## NAT_ADDRESS_DEPENDENT:

```fides
pub const NAT_ADDRESS_DEPENDENT: i64 = 2;
```

*Defined at line 1125*

---

## NAT_ADDRESS_PORT_DEPENDENT:

```fides
pub const NAT_ADDRESS_PORT_DEPENDENT: i64 = 3;
```

*Defined at line 1126*

---

## nat_mapping_behaviour

Mapping behaviour (RFC 4787 / RFC 5780 section 4.3) from the reflexive
addresses one local socket got from: server A, server B's address with
A's port (`via_b_ip`), and server B's address and port (`via_b_ip_port`).

```fides
pub fn nat_mapping_behaviour(local: &StunAddr, via_a: &StunAddr, via_b_ip: &StunAddr, via_b_ip_port: &StunAddr) -> i64 {
```

**Parameters:**

- `local`: `&StunAddr`
- `via_a`: `&StunAddr`
- `via_b_ip`: `&StunAddr`
- `via_b_ip_port`: `&StunAddr`

**Returns:** `i64`

*Defined at line 1131*

---

## nat_behaviour_name

```fides
pub fn nat_behaviour_name(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 1144*

---

## nat_hole_punch_likely

Can two peers expect UDP hole punching to work? Both sides need
endpoint-independent mapping, or one side no NAT at all; otherwise use
a relay (TURN).

```fides
pub fn nat_hole_punch_likely(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 1160*

---

## ICE_TYPE_HOST:

```fides
pub const ICE_TYPE_HOST: i64 = 126;
```

*Defined at line 1167*

---

## ICE_TYPE_PEER_REFLEXIVE:

```fides
pub const ICE_TYPE_PEER_REFLEXIVE: i64 = 110;
```

*Defined at line 1168*

---

## ICE_TYPE_SERVER_REFLEXIVE:

```fides
pub const ICE_TYPE_SERVER_REFLEXIVE: i64 = 100;
```

*Defined at line 1169*

---

## ICE_TYPE_RELAYED:

```fides
pub const ICE_TYPE_RELAYED: i64 = 0;
```

*Defined at line 1170*

---

## ice_candidate_priority

Candidate priority (RFC 8445 section 5.1.2.1).

```fides
pub fn ice_candidate_priority(type_pref: i64, local_pref: i64, component: i64) -> i64 {
```

**Parameters:**

- `type_pref`: `i64`
- `local_pref`: `i64`
- `component`: `i64`

**Returns:** `i64`

*Defined at line 1173*

---

## ice_pair_priority

Candidate pair priority (RFC 8445 section 6.1.2.3); g = controlling
agent's candidate priority, d = controlled's.

```fides
pub fn ice_pair_priority(g: i64, d: i64) -> i64 {
```

**Parameters:**

- `g`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 1179*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
