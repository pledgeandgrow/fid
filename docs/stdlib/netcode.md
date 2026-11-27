[All modules](index.md)

# netcode -- Fides Documentation

## Contents

- [NcGame](#NcGame) (struct)
- [nc_game](#nc_game) (function)
- [nc_checksum](#nc_checksum) (function)
- [NcSession](#NcSession) (struct)
- [nc_session_new](#nc_session_new) (function)
- [nc_input_known](#nc_input_known) (function)
- [nc_input_for](#nc_input_for) (function)
- [nc_input_predicted](#nc_input_predicted) (function)
- [nc_session_add_local_input](#nc_session_add_local_input) (function)
- [nc_session_add_remote_input](#nc_session_add_remote_input) (function)
- [nc_session_resolve](#nc_session_resolve) (function)
- [nc_session_advance](#nc_session_advance) (function)
- [nc_session_lead](#nc_session_lead) (function)
- [NcInterp](#NcInterp) (struct)
- [nc_interp_new](#nc_interp_new) (function)
- [nc_interp_push](#nc_interp_push) (function)
- [nc_interp_sample](#nc_interp_sample) (function)
- [nc_words_to_bytes](#nc_words_to_bytes) (function)
- [nc_bytes_to_words](#nc_bytes_to_words) (function)
- [nc_delta_encode](#nc_delta_encode) (function)
- [nc_delta_decode](#nc_delta_decode) (function)
- [nc_seq_newer](#nc_seq_newer) (function)
- [nc_seq_diff](#nc_seq_diff) (function)
- [NcChannel](#NcChannel) (struct)
- [NC_HEADER_LEN:](#NC_HEADER_LEN:) (const)
- [nc_channel_new](#nc_channel_new) (function)
- [nc_channel_send](#nc_channel_send) (function)
- [nc_channel_unacked](#nc_channel_unacked) (function)
- [nc_channel_build_packet](#nc_channel_build_packet) (function)
- [nc_channel_receive](#nc_channel_receive) (function)
- [nc_channel_take](#nc_channel_take) (function)

## NcGame

The game as the rollback session sees it. `h` is the game instance.

```fides
pub struct NcGame {
```

**Fields:**

| Name | Type |
|------|------|
| `save` | `fn(i64) -> Vec<i64>` |
| `load` | `fn(i64, Vec<i64>) -> void` |
| `advance` | `fn(i64, Vec<i64>) -> void` |

*Defined at line 52*

---

## nc_game

```fides
pub fn nc_game(save: fn(i64) -> Vec<i64>, load: fn(i64, Vec<i64>) -> void, advance: fn(i64, Vec<i64>) -> void) -> NcGame {
```

**Parameters:**

- `save`: `fn(i64) -> Vec<i64>`
- `load`: `fn(i64`
- `advance`: `fn(i64`

**Returns:** `NcGame`

*Defined at line 58*

---

## nc_checksum

```fides
pub fn nc_checksum(state: &Vec<i64>) -> i64 {
```

**Parameters:**

- `state`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 66*

---

## NcSession

```fides
pub struct NcSession {
```

**Fields:**

| Name | Type |
|------|------|
| `game` | `NcGame` |
| `handle` | `i64` |
| `players` | `i64` |
| `local` | `i64` |
| `delay` | `i64` |
| `max_prediction` | `i64` |
| `frame` | `i64` |
| `confirmed` | `i64` |
| `ring` | `i64` |
| `in_frame` | `Vec<i64>` |
| `in_val` | `Vec<i64>` |
| `used_frame` | `Vec<i64>` |
| `used_val` | `Vec<i64>` |
| `snap` | `Vec<Vec<i64>>` |
| `snap_frame` | `Vec<i64>` |
| `contiguous` | `Vec<i64>` |
| `rollback_from` | `i64` |
| `rollbacks` | `i64` |
| `resimulated` | `i64` |
| `stalls` | `i64` |
| `next_local` | `i64` |

*Defined at line 84*

---

## nc_session_new

A session for `players` players, this peer being `local`. Frames below
`delay` have input 0 for everyone (local input is scheduled `delay`
frames ahead; every peer must use the same delay).

```fides
pub fn nc_session_new(game: NcGame, handle: i64, players: i64, local: i64, max_prediction: i64, delay: i64) -> NcSession {
```

**Parameters:**

- `game`: `NcGame`
- `handle`: `i64`
- `players`: `i64`
- `local`: `i64`
- `max_prediction`: `i64`
- `delay`: `i64`

**Returns:** `NcSession`

*Defined at line 115*

---

## nc_input_known

Is the input of player p at frame f known?

```fides
pub fn nc_input_known(s: &NcSession, p: i64, f: i64) -> bool {
```

**Parameters:**

- `s`: `&NcSession`
- `p`: `i64`
- `f`: `i64`

**Returns:** `bool`

*Defined at line 143*

---

## nc_input_for

The input to use for player p at frame f: the real one when known,
else the newest known input before f (searching back over the window),
else 0.

```fides
pub fn nc_input_for(s: &NcSession, p: i64, f: i64) -> i64 {
```

**Parameters:**

- `s`: `&NcSession`
- `p`: `i64`
- `f`: `i64`

**Returns:** `i64`

*Defined at line 176*

---

## nc_input_predicted

Was frame f simulated with a guess for player p (no real input yet)?

```fides
pub fn nc_input_predicted(s: &NcSession, p: i64, f: i64) -> bool {
```

**Parameters:**

- `s`: `&NcSession`
- `p`: `i64`
- `f`: `i64`

**Returns:** `bool`

*Defined at line 192*

---

## nc_session_add_local_input

Schedule this peer's input; returns the frame it belongs to (send the
pair to the other peers). Returns -1 when that frame is too far ahead.

```fides
pub fn nc_session_add_local_input(s: &mut NcSession, v: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut NcSession`
- `v`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## nc_session_add_remote_input

A remote player's input for frame f. Returns false when it is refused
(outside the window); duplicates are accepted and ignored.

```fides
pub fn nc_session_add_remote_input(s: &mut NcSession, p: i64, f: i64, v: i64) -> bool {
```

**Parameters:**

- `s`: `&mut NcSession`
- `p`: `i64`
- `f`: `i64`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 219*

---

## nc_session_resolve

Simulate again from a pending rollback point up to the current frame.
Returns the number of frames simulated again.

```fides
pub fn nc_session_resolve(s: &mut NcSession) -> i64 {
```

**Parameters:**

- `s`: `&mut NcSession`

**Returns:** `i64`

*Defined at line 268*

---

## nc_session_advance

Roll back if needed, then simulate the current frame. Returns false
(and simulates nothing new) when the prediction window is full or this
peer's own input for the frame is missing.

```fides
pub fn nc_session_advance(s: &mut NcSession) -> bool {
```

**Parameters:**

- `s`: `&mut NcSession`

**Returns:** `bool`

*Defined at line 294*

---

## nc_session_lead

Frames simulated ahead of the confirmed frame (how much is a guess).

```fides
pub fn nc_session_lead(s: &NcSession) -> i64 {
```

**Parameters:**

- `s`: `&NcSession`

**Returns:** `i64`

*Defined at line 305*

---

## NcInterp

Snapshots received from a server (time, values), kept in time order.

```fides
pub struct NcInterp {
```

**Fields:**

| Name | Type |
|------|------|
| `times` | `Vec<f64>` |
| `values` | `Vec<Vec<f64>>` |
| `cap` | `i64` |

*Defined at line 312*

---

## nc_interp_new

```fides
pub fn nc_interp_new(cap: i64) -> NcInterp {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `NcInterp`

*Defined at line 318*

---

## nc_interp_push

Add a snapshot; out-of-order ones go to their place, an equal time
replaces, the oldest drops beyond `cap`.

```fides
pub fn nc_interp_push(b: &mut NcInterp, t: f64, vals: Vec<f64>) {
```

**Parameters:**

- `b`: `&mut NcInterp`
- `t`: `f64`
- `vals`: `Vec<f64>`

*Defined at line 326*

---

## nc_interp_sample

The values at time t: linear between the two snapshots around t, the
first or last snapshot outside them (no extrapolation), empty when there
is none.

```fides
pub fn nc_interp_sample(b: &NcInterp, t: f64) -> Vec<f64> {
```

**Parameters:**

- `b`: `&NcInterp`
- `t`: `f64`

**Returns:** `Vec<f64>`

*Defined at line 346*

---

## nc_words_to_bytes

Words as bytes (8 per word, little-endian).

```fides
pub fn nc_words_to_bytes(w: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `w`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 378*

---

## nc_bytes_to_words

```fides
pub fn nc_bytes_to_words(b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 394*

---

## nc_delta_encode

cur against base: varint length, then XOR bytes with zero runs folded.

```fides
pub fn nc_delta_encode(base: &Vec<i64>, cur: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `base`: `&Vec<i64>`
- `cur`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 420*

---

## nc_delta_decode

Inverse of nc_delta_encode. Err on truncated or overlong input.

```fides
pub fn nc_delta_decode(base: &Vec<i64>, d: &Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `base`: `&Vec<i64>`
- `d`: `&Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 454*

---

## nc_seq_newer

Is 16-bit sequence a newer than b (with wrap-around)?

```fides
pub fn nc_seq_newer(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 514*

---

## nc_seq_diff

a - b for 16-bit sequences, in -32768..32767.

```fides
pub fn nc_seq_diff(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 520*

---

## NcChannel

```fides
pub struct NcChannel {
```

**Fields:**

| Name | Type |
|------|------|
| `local_seq` | `i64` |
| `remote_seq` | `i64` |
| `recv_bits` | `i64` |
| `next_send_id` | `i64` |
| `out_id` | `Vec<i64>` |
| `out_data` | `Vec<Vec<i64>>` |
| `out_sent_at` | `Vec<i64>` |
| `out_acked` | `Vec<bool>` |
| `flight_seq` | `Vec<i64>` |
| `flight_ids` | `Vec<Vec<i64>>` |
| `next_recv_id` | `i64` |
| `pend_id` | `Vec<i64>` |
| `pend_data` | `Vec<Vec<i64>>` |
| `delivered` | `Vec<Vec<i64>>` |
| `resend_after` | `i64` |
| `max_packet` | `i64` |
| `packets_sent` | `i64` |
| `packets_received` | `i64` |
| `resends` | `i64` |
| `duplicates` | `i64` |
| `bad_packets` | `i64` |

*Defined at line 530*

---

## NC_HEADER_LEN:

```fides
pub const NC_HEADER_LEN: i64 = 10;
```

*Defined at line 554*

---

## nc_channel_new

A channel resending unacknowledged messages every `resend_after` ticks,
packets at most `max_packet` bytes (a message must fit in one).

```fides
pub fn nc_channel_new(resend_after: i64, max_packet: i64) -> NcChannel {
```

**Parameters:**

- `resend_after`: `i64`
- `max_packet`: `i64`

**Returns:** `NcChannel`

*Defined at line 558*

---

## nc_channel_send

Queue a message; returns its id. Err when it can never fit a packet.

```fides
pub fn nc_channel_send(c: &mut NcChannel, msg: Vec<i64>) -> Result<i64, str> {
```

**Parameters:**

- `c`: `&mut NcChannel`
- `msg`: `Vec<i64>`

**Returns:** `Result<i64, str>`

*Defined at line 565*

---

## nc_channel_unacked

Messages queued and not yet acknowledged.

```fides
pub fn nc_channel_unacked(c: &NcChannel) -> i64 {
```

**Parameters:**

- `c`: `&NcChannel`

**Returns:** `i64`

*Defined at line 579*

---

## nc_channel_build_packet

The next packet to send at time `now`: acks plus every message that was
never sent or was sent `resend_after` ticks ago or more, oldest first.

```fides
pub fn nc_channel_build_packet(c: &mut NcChannel, now: i64) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut NcChannel`
- `now`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 602*

---

## nc_channel_receive

Take in a received packet. Returns false (and changes nothing) when it
is malformed.

```fides
pub fn nc_channel_receive(c: &mut NcChannel, p: &Vec<i64>) -> bool {
```

**Parameters:**

- `c`: `&mut NcChannel`
- `p`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 692*

---

## nc_channel_take

The messages delivered (in send order) since the last call.

```fides
pub fn nc_channel_take(c: &mut NcChannel) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut NcChannel`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 811*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
