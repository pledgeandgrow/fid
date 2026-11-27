[All modules](index.md)

# wireguard -- Fides Documentation

## Contents

- [WG_CONSTRUCTION:](#WG_CONSTRUCTION:) (const)
- [WG_IDENTIFIER:](#WG_IDENTIFIER:) (const)
- [WG_REKEY_AFTER_MESSAGES:](#WG_REKEY_AFTER_MESSAGES:) (const)
- [WG_REJECT_AFTER_MESSAGES:](#WG_REJECT_AFTER_MESSAGES:) (const)
- [WG_REKEY_AFTER_TIME:](#WG_REKEY_AFTER_TIME:) (const)
- [WG_REJECT_AFTER_TIME:](#WG_REJECT_AFTER_TIME:) (const)
- [WG_REKEY_ATTEMPT_TIME:](#WG_REKEY_ATTEMPT_TIME:) (const)
- [WG_REKEY_TIMEOUT:](#WG_REKEY_TIMEOUT:) (const)
- [WG_KEEPALIVE_TIMEOUT:](#WG_KEEPALIVE_TIMEOUT:) (const)
- [WG_COOKIE_LIFETIME:](#WG_COOKIE_LIFETIME:) (const)
- [WG_WINDOW_WORDS:](#WG_WINDOW_WORDS:) (const)
- [WG_MAC_OK:](#WG_MAC_OK:) (const)
- [WG_MAC_BAD:](#WG_MAC_BAD:) (const)
- [WG_MAC_NEED_COOKIE:](#WG_MAC_NEED_COOKIE:) (const)
- [WG_TIMER_NONE:](#WG_TIMER_NONE:) (const)
- [WG_TIMER_INITIATION:](#WG_TIMER_INITIATION:) (const)
- [WG_TIMER_KEEPALIVE:](#WG_TIMER_KEEPALIVE:) (const)
- [WG_TIMER_GAVE_UP:](#WG_TIMER_GAVE_UP:) (const)
- [WG_TIMER_EXPIRED:](#WG_TIMER_EXPIRED:) (const)
- [WgReplay](#WgReplay) (struct)
- [WgKeypair](#WgKeypair) (struct)
- [WgPeer](#WgPeer) (struct)
- [WgReceived](#WgReceived) (struct)
- [WgTimerOut](#WgTimerOut) (struct)
- [WgCookieChecker](#WgCookieChecker) (struct)
- [wg_hash](#wg_hash) (function)
- [wg_mac](#wg_mac) (function)
- [wg_mac1_key](#wg_mac1_key) (function)
- [wg_cookie_key](#wg_cookie_key) (function)
- [wg_tai64n](#wg_tai64n) (function)
- [wg_replay_new](#wg_replay_new) (function)
- [wg_replay_check](#wg_replay_check) (function)
- [wg_replay_update](#wg_replay_update) (function)
- [wg_peer_new](#wg_peer_new) (function)
- [wg_set_next_ephemeral](#wg_set_next_ephemeral) (function)
- [wg_set_next_index](#wg_set_next_index) (function)
- [wg_create_initiation](#wg_create_initiation) (function)
- [wg_initiation_static](#wg_initiation_static) (function)
- [wg_consume_initiation](#wg_consume_initiation) (function)
- [wg_create_response](#wg_create_response) (function)
- [wg_consume_response](#wg_consume_response) (function)
- [wg_receiver_index](#wg_receiver_index) (function)
- [wg_encrypt](#wg_encrypt) (function)
- [wg_decrypt](#wg_decrypt) (function)
- [wg_cookie_checker_new](#wg_cookie_checker_new) (function)
- [wg_cookie_set_secret](#wg_cookie_set_secret) (function)
- [wg_check_macs](#wg_check_macs) (function)
- [wg_create_cookie_reply](#wg_create_cookie_reply) (function)
- [wg_consume_cookie_reply](#wg_consume_cookie_reply) (function)
- [wg_timers](#wg_timers) (function)

## WG_CONSTRUCTION:

```fides
pub const WG_CONSTRUCTION: str = "Noise_IKpsk2_25519_ChaChaPoly_BLAKE2s";
```

*Defined at line 57*

---

## WG_IDENTIFIER:

```fides
pub const WG_IDENTIFIER: str = "WireGuard v1 zx2c4 Jason@zx2c4.com";
```

*Defined at line 58*

---

## WG_REKEY_AFTER_MESSAGES:

```fides
pub const WG_REKEY_AFTER_MESSAGES: i64 = 1152921504606846976;
```

*Defined at line 59*

---

## WG_REJECT_AFTER_MESSAGES:

```fides
pub const WG_REJECT_AFTER_MESSAGES: i64 = 4611686018427387904;
```

*Defined at line 60*

---

## WG_REKEY_AFTER_TIME:

```fides
pub const WG_REKEY_AFTER_TIME: i64 = 120;
```

*Defined at line 61*

---

## WG_REJECT_AFTER_TIME:

```fides
pub const WG_REJECT_AFTER_TIME: i64 = 180;
```

*Defined at line 62*

---

## WG_REKEY_ATTEMPT_TIME:

```fides
pub const WG_REKEY_ATTEMPT_TIME: i64 = 90;
```

*Defined at line 63*

---

## WG_REKEY_TIMEOUT:

```fides
pub const WG_REKEY_TIMEOUT: i64 = 5;
```

*Defined at line 64*

---

## WG_KEEPALIVE_TIMEOUT:

```fides
pub const WG_KEEPALIVE_TIMEOUT: i64 = 10;
```

*Defined at line 65*

---

## WG_COOKIE_LIFETIME:

```fides
pub const WG_COOKIE_LIFETIME: i64 = 120;
```

*Defined at line 66*

---

## WG_WINDOW_WORDS:

Replay window: 64 words of 32 bits, 2016 counters behind the newest.

```fides
pub const WG_WINDOW_WORDS: i64 = 64;
```

*Defined at line 68*

---

## WG_MAC_OK:

```fides
pub const WG_MAC_OK: i64 = 0;
```

*Defined at line 70*

---

## WG_MAC_BAD:

```fides
pub const WG_MAC_BAD: i64 = 1;
```

*Defined at line 71*

---

## WG_MAC_NEED_COOKIE:

```fides
pub const WG_MAC_NEED_COOKIE: i64 = 2;
```

*Defined at line 72*

---

## WG_TIMER_NONE:

```fides
pub const WG_TIMER_NONE: i64 = 0;
```

*Defined at line 74*

---

## WG_TIMER_INITIATION:

```fides
pub const WG_TIMER_INITIATION: i64 = 1;
```

*Defined at line 75*

---

## WG_TIMER_KEEPALIVE:

```fides
pub const WG_TIMER_KEEPALIVE: i64 = 2;
```

*Defined at line 76*

---

## WG_TIMER_GAVE_UP:

```fides
pub const WG_TIMER_GAVE_UP: i64 = 3;
```

*Defined at line 77*

---

## WG_TIMER_EXPIRED:

```fides
pub const WG_TIMER_EXPIRED: i64 = 4;
```

*Defined at line 78*

---

## WgReplay

A sliding replay window (RFC 6479): `last` is the highest counter
accepted, -1 before the first.

```fides
pub struct WgReplay {
```

**Fields:**

| Name | Type |
|------|------|
| `last` | `i64` |
| `bits` | `Vec<i64>` |

*Defined at line 82*

---

## WgKeypair

Transport keys of one handshake.

```fides
pub struct WgKeypair {
```

**Fields:**

| Name | Type |
|------|------|
| `valid` | `bool` |
| `send_key` | `Vec<i64>` |
| `recv_key` | `Vec<i64>` |
| `send_counter` | `i64` |
| `replay` | `WgReplay` |
| `local_index` | `i64` |
| `remote_index` | `i64` |
| `created` | `i64` |
| `initiator` | `bool` |

*Defined at line 88*

---

## WgPeer

One peer as seen from this side.

```fides
pub struct WgPeer {
```

**Fields:**

| Name | Type |
|------|------|
| `s` | `Vec<i64>` |
| `s_pub` | `Vec<i64>` |
| `remote` | `Vec<i64>` |
| `psk` | `Vec<i64>` |
| `hs` | `NoiseHandshake` |
| `hs_state` | `i64` |
| `hs_local_index` | `i64` |
| `hs_remote_index` | `i64` |
| `hs_sent_at` | `i64` |
| `hs_first_at` | `i64` |
| `last_timestamp` | `Vec<i64>` |
| `current` | `WgKeypair` |
| `previous` | `WgKeypair` |
| `next` | `WgKeypair` |
| `last_sent` | `i64` |
| `last_data_sent` | `i64` |
| `last_received` | `i64` |
| `persistent_keepalive` | `i64` |
| `cookie` | `Vec<i64>` |
| `cookie_time` | `i64` |
| `last_mac1` | `Vec<i64>` |
| `next_e` | `Vec<i64>` |
| `next_index` | `i64` |
| `err` | `str` |

*Defined at line 101*

---

## WgReceived

A received data packet: `data` keeps its zero padding (an IP packet
carries its own length); `keepalive` when it was empty.

```fides
pub struct WgReceived {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `err` | `str` |
| `data` | `Vec<i64>` |
| `keepalive` | `bool` |

*Defined at line 130*

---

## WgTimerOut

What the timers want sent now.

```fides
pub struct WgTimerOut {
```

**Fields:**

| Name | Type |
|------|------|
| `action` | `i64` |
| `packet` | `Vec<i64>` |

*Defined at line 138*

---

## WgCookieChecker

The responder side of cookies: the local static public key and the
secret, renewed every two minutes.

```fides
pub struct WgCookieChecker {
```

**Fields:**

| Name | Type |
|------|------|
| `s_pub` | `Vec<i64>` |
| `secret` | `Vec<i64>` |
| `secret_time` | `i64` |

*Defined at line 145*

---

## wg_hash

HASH(x) = BLAKE2s-256.

```fides
pub fn wg_hash(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 240*

---

## wg_mac

MAC(key, x) = keyed BLAKE2s with 16-byte output.

```fides
pub fn wg_mac(key: Vec<i64>, data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `key`: `Vec<i64>`
- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 245*

---

## wg_mac1_key

The mac1 key HASH("mac1----" || public key) of the receiving side.

```fides
pub fn wg_mac1_key(public_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `public_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 250*

---

## wg_cookie_key

The cookie key HASH("cookie--" || public key) of the receiving side.

```fides
pub fn wg_cookie_key(public_key: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `public_key`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 255*

---

## wg_tai64n

TAI64N of a Unix time: 8 bytes big-endian 2^62 + 10 + seconds, then 4
bytes big-endian nanoseconds with the low 24 bits cleared (as
wireguard-go and the Linux module, so the timestamp is no fine clock).

```fides
pub fn wg_tai64n(unix_sec: i64, nsec: i64) -> Vec<i64> {
```

**Parameters:**

- `unix_sec`: `i64`
- `nsec`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 262*

---

## wg_replay_new

An empty window.

```fides
pub fn wg_replay_new() -> WgReplay {
```

**Returns:** `WgReplay`

*Defined at line 296*

---

## wg_replay_check

True when `counter` may still be accepted (not too old, not seen).

```fides
pub fn wg_replay_check(w: &WgReplay, counter: i64) -> bool {
```

**Parameters:**

- `w`: `&WgReplay`
- `counter`: `i64`

**Returns:** `bool`

*Defined at line 303*

---

## wg_replay_update

Record `counter` (call after the packet authenticated); false when it
is a replay or too old.

```fides
pub fn wg_replay_update(w: &mut WgReplay, counter: i64) -> bool {
```

**Parameters:**

- `w`: `&mut WgReplay`
- `counter`: `i64`

**Returns:** `bool`

*Defined at line 319*

---

## wg_peer_new

A peer: our 32-byte static secret, the peer's static public key, and
the pre-shared key (32 bytes, or empty for none = 32 zero bytes).

```fides
pub fn wg_peer_new(secret: Vec<i64>, remote_public: Vec<i64>, psk: Vec<i64>) -> WgPeer {
```

**Parameters:**

- `secret`: `Vec<i64>`
- `remote_public`: `Vec<i64>`
- `psk`: `Vec<i64>`

**Returns:** `WgPeer`

*Defined at line 357*

---

## wg_set_next_ephemeral

Seam: the next ephemeral secret this peer uses (32 bytes).

```fides
pub fn wg_set_next_ephemeral(p: &mut WgPeer, e: Vec<i64>) {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `e`: `Vec<i64>`

*Defined at line 368*

---

## wg_set_next_index

Seam: the next sender index this peer announces.

```fides
pub fn wg_set_next_index(p: &mut WgPeer, idx: i64) {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `idx`: `i64`

*Defined at line 373*

---

## wg_create_initiation

Handshake initiation (type 1, 148 bytes) carrying TAI64N(now, nsec).

```fides
pub fn wg_create_initiation(p: &mut WgPeer, now: i64, nsec: i64) -> Vec<i64> {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `now`: `i64`
- `nsec`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 420*

---

## wg_initiation_static

The initiator's static public key inside an initiation addressed to
`secret` (for a device choosing among its peers); empty when the
message does not decrypt.

```fides
pub fn wg_initiation_static(secret: Vec<i64>, pkt: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `secret`: `Vec<i64>`
- `pkt`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 449*

---

## wg_consume_initiation

Process a handshake initiation from this peer: mac1, the Noise
message, the initiator's identity and a timestamp newer than the last.

```fides
pub fn wg_consume_initiation(p: &mut WgPeer, pkt: Vec<i64>, now: i64) -> bool {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `pkt`: `Vec<i64>`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 464*

---

## wg_create_response

Handshake response (type 2, 92 bytes) to the initiation just consumed.
The new keys wait in the `next` slot until the initiator's first data
message confirms them.

```fides
pub fn wg_create_response(p: &mut WgPeer, now: i64) -> Vec<i64> {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 505*

---

## wg_consume_response

Process the response to our initiation: the new keys become current.

```fides
pub fn wg_consume_response(p: &mut WgPeer, pkt: Vec<i64>, now: i64) -> bool {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `pkt`: `Vec<i64>`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 536*

---

## wg_receiver_index

The receiver index of a data packet (type 4), -1 for other packets.

```fides
pub fn wg_receiver_index(pkt: &Vec<i64>) -> i64 {
```

**Parameters:**

- `pkt`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 565*

---

## wg_encrypt

Encrypt one packet (zero-padded to a multiple of 16) under the current
keys; empty when there are no usable keys (none yet, too old, or the
counter is exhausted). An empty `plain` is a keepalive.

```fides
pub fn wg_encrypt(p: &mut WgPeer, plain: Vec<i64>, now: i64) -> Vec<i64> {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `plain`: `Vec<i64>`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 577*

---

## wg_decrypt

Decrypt a data packet with whichever keypair its receiver index names.
The first packet under `next` keys promotes them to current.

```fides
pub fn wg_decrypt(p: &mut WgPeer, pkt: Vec<i64>, now: i64) -> WgReceived {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `pkt`: `Vec<i64>`
- `now`: `i64`

**Returns:** `WgReceived`

*Defined at line 636*

---

## wg_cookie_checker_new

A cookie checker for our static public key.

```fides
pub fn wg_cookie_checker_new(s_pub: Vec<i64>) -> WgCookieChecker {
```

**Parameters:**

- `s_pub`: `Vec<i64>`

**Returns:** `WgCookieChecker`

*Defined at line 668*

---

## wg_cookie_set_secret

Seam: set the cookie secret (32 bytes) as of `now`.

```fides
pub fn wg_cookie_set_secret(c: &mut WgCookieChecker, secret: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut WgCookieChecker`
- `secret`: `Vec<i64>`
- `now`: `i64`

*Defined at line 675*

---

## wg_check_macs

Check the MACs of an initiation or response from address `src`
(IP bytes || port bytes). Under load a valid mac2 (made with the
cookie for that address) is required, else WG_MAC_NEED_COOKIE.

```fides
pub fn wg_check_macs(c: &mut WgCookieChecker, pkt: Vec<i64>, src: Vec<i64>, now: i64, under_load: bool) -> i64 {
```

**Parameters:**

- `c`: `&mut WgCookieChecker`
- `pkt`: `Vec<i64>`
- `src`: `Vec<i64>`
- `now`: `i64`
- `under_load`: `bool`

**Returns:** `i64`

*Defined at line 691*

---

## wg_create_cookie_reply

A cookie reply (type 3, 64 bytes) to `pkt` from `src`; `nonce24` is
24 random bytes.

```fides
pub fn wg_create_cookie_reply(c: &mut WgCookieChecker, pkt: Vec<i64>, src: Vec<i64>, nonce24: Vec<i64>, now: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut WgCookieChecker`
- `pkt`: `Vec<i64>`
- `src`: `Vec<i64>`
- `nonce24`: `Vec<i64>`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 712*

---

## wg_consume_cookie_reply

Take the cookie from a reply to our last message; mac2 of the next
messages uses it for two minutes.

```fides
pub fn wg_consume_cookie_reply(p: &mut WgPeer, pkt: Vec<i64>, now: i64) -> bool {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `pkt`: `Vec<i64>`
- `now`: `i64`

**Returns:** `bool`

*Defined at line 731*

---

## wg_timers

Run the timers at `now`: at most one packet to send (a fresh
initiation or a keepalive), or a note that the handshake was given up
or all keys were dropped.

```fides
pub fn wg_timers(p: &mut WgPeer, now: i64) -> WgTimerOut {
```

**Parameters:**

- `p`: `&mut WgPeer`
- `now`: `i64`

**Returns:** `WgTimerOut`

*Defined at line 761*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
