[All modules](index.md)

# ipc -- Fides Documentation

## Contents

- [IPC_RUNNING:](#IPC_RUNNING:) (const)
- [IPC_BLOCKED_SEND:](#IPC_BLOCKED_SEND:) (const)
- [IPC_BLOCKED_RECV:](#IPC_BLOCKED_RECV:) (const)
- [IPC_BLOCKED_REPLY:](#IPC_BLOCKED_REPLY:) (const)
- [CAP_SEND:](#CAP_SEND:) (const)
- [CAP_RECV:](#CAP_RECV:) (const)
- [CAP_GRANT:](#CAP_GRANT:) (const)
- [CAP_ALL:](#CAP_ALL:) (const)
- [CAP_NULL:](#CAP_NULL:) (const)
- [CAP_ENDPOINT:](#CAP_ENDPOINT:) (const)
- [IPC_OK:](#IPC_OK:) (const)
- [IPC_BLOCKED:](#IPC_BLOCKED:) (const)
- [IPC_ERR_CAP:](#IPC_ERR_CAP:) (const)
- [IPC_ERR_RIGHTS:](#IPC_ERR_RIGHTS:) (const)
- [IPC_ERR_NOREPLY:](#IPC_ERR_NOREPLY:) (const)
- [IPC_WOULDBLOCK:](#IPC_WOULDBLOCK:) (const)
- [IPC_ERR_BADGE:](#IPC_ERR_BADGE:) (const)
- [IPC_ERR_STATE:](#IPC_ERR_STATE:) (const)
- [Cap](#Cap) (struct)
- [IpcMsg](#IpcMsg) (struct)
- [IpcThread](#IpcThread) (struct)
- [Endpoint](#Endpoint) (struct)
- [IpcKernel](#IpcKernel) (struct)
- [ipc_msg](#ipc_msg) (function)
- [ipc_msg_with_cap](#ipc_msg_with_cap) (function)
- [ipc_new](#ipc_new) (function)
- [ipc_thread_new](#ipc_thread_new) (function)
- [ipc_thread](#ipc_thread) (function)
- [ipc_state](#ipc_state) (function)
- [ipc_state_name](#ipc_state_name) (function)
- [ipc_cap](#ipc_cap) (function)
- [ipc_endpoint_new](#ipc_endpoint_new) (function)
- [ipc_cap_copy](#ipc_cap_copy) (function)
- [ipc_mint](#ipc_mint) (function)
- [ipc_cap_delete](#ipc_cap_delete) (function)
- [ipc_revoke](#ipc_revoke) (function)
- [ipc_send](#ipc_send) (function)
- [ipc_nbsend](#ipc_nbsend) (function)
- [ipc_call](#ipc_call) (function)
- [ipc_recv](#ipc_recv) (function)
- [ipc_poll](#ipc_poll) (function)
- [ipc_reply](#ipc_reply) (function)
- [ipc_reply_recv](#ipc_reply_recv) (function)
- [ipc_msg_of](#ipc_msg_of) (function)
- [RingDesc](#RingDesc) (struct)
- [shmring_new](#shmring_new) (function)
- [shmring_count](#shmring_count) (function)
- [shmring_space](#shmring_space) (function)
- [shmring_push](#shmring_push) (function)
- [shmring_pop](#shmring_pop) (function)
- [shmring_set_event](#shmring_set_event) (function)
- [shmring_need_event](#shmring_need_event) (function)

## IPC_RUNNING:

```fides
pub const IPC_RUNNING: i64 = 0;
```

*Defined at line 25*

---

## IPC_BLOCKED_SEND:

```fides
pub const IPC_BLOCKED_SEND: i64 = 1;
```

*Defined at line 26*

---

## IPC_BLOCKED_RECV:

```fides
pub const IPC_BLOCKED_RECV: i64 = 2;
```

*Defined at line 27*

---

## IPC_BLOCKED_REPLY:

```fides
pub const IPC_BLOCKED_REPLY: i64 = 3;
```

*Defined at line 28*

---

## CAP_SEND:

```fides
pub const CAP_SEND: i64 = 1;
```

*Defined at line 30*

---

## CAP_RECV:

```fides
pub const CAP_RECV: i64 = 2;
```

*Defined at line 31*

---

## CAP_GRANT:

```fides
pub const CAP_GRANT: i64 = 4;
```

*Defined at line 32*

---

## CAP_ALL:

```fides
pub const CAP_ALL: i64 = 7;
```

*Defined at line 33*

---

## CAP_NULL:

```fides
pub const CAP_NULL: i64 = 0;
```

*Defined at line 35*

---

## CAP_ENDPOINT:

```fides
pub const CAP_ENDPOINT: i64 = 1;
```

*Defined at line 36*

---

## IPC_OK:

```fides
pub const IPC_OK: i64 = 0;
```

*Defined at line 38*

---

## IPC_BLOCKED:

```fides
pub const IPC_BLOCKED: i64 = 1;
```

*Defined at line 39*

---

## IPC_ERR_CAP:

```fides
pub const IPC_ERR_CAP: i64 = -1;
```

*Defined at line 40*

---

## IPC_ERR_RIGHTS:

```fides
pub const IPC_ERR_RIGHTS: i64 = -2;
```

*Defined at line 41*

---

## IPC_ERR_NOREPLY:

```fides
pub const IPC_ERR_NOREPLY: i64 = -3;
```

*Defined at line 42*

---

## IPC_WOULDBLOCK:

```fides
pub const IPC_WOULDBLOCK: i64 = -4;
```

*Defined at line 43*

---

## IPC_ERR_BADGE:

```fides
pub const IPC_ERR_BADGE: i64 = -5;
```

*Defined at line 44*

---

## IPC_ERR_STATE:

```fides
pub const IPC_ERR_STATE: i64 = -6;
```

*Defined at line 45*

---

## Cap

```fides
pub struct Cap {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `obj` | `i64` |
| `rights` | `i64` |
| `badge` | `i64` |
| `id` | `i64` |
| `parent` | `i64,  // id of the cap this one was derived from, -1 for an original` |

*Defined at line 47*

---

## IpcMsg

```fides
pub struct IpcMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `label` | `i64` |
| `words` | `Vec<i64>` |
| `cap` | `i64` |
| `badge` | `i64,  // set by the kernel from the sender's cap` |

*Defined at line 58*

---

## IpcThread

```fides
pub struct IpcThread {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `state` | `i64` |
| `cspace` | `Vec<Cap>` |
| `msg` | `IpcMsg` |
| `calling` | `bool` |
| `send_cap` | `i64` |
| `reply_to` | `i64` |
| `blocked_on` | `i64,  // endpoint, -1 none` |

*Defined at line 66*

---

## Endpoint

```fides
pub struct Endpoint {
```

**Fields:**

| Name | Type |
|------|------|
| `send_q` | `Vec<i64>` |
| `recv_q` | `Vec<i64>` |
| `alive` | `bool` |

*Defined at line 82*

---

## IpcKernel

```fides
pub struct IpcKernel {
```

**Fields:**

| Name | Type |
|------|------|
| `threads` | `Vec<IpcThread>` |
| `endpoints` | `Vec<Endpoint>` |
| `next_cap_id` | `i64` |
| `delivered` | `i64` |

*Defined at line 88*

---

## ipc_msg

```fides
pub fn ipc_msg(label: i64, words: Vec<i64>) -> IpcMsg {
```

**Parameters:**

- `label`: `i64`
- `words`: `Vec<i64>`

**Returns:** `IpcMsg`

*Defined at line 95*

---

## ipc_msg_with_cap

A message that also transfers the cap in slot cptr of the sender.

```fides
pub fn ipc_msg_with_cap(label: i64, words: Vec<i64>, cptr: i64) -> IpcMsg {
```

**Parameters:**

- `label`: `i64`
- `words`: `Vec<i64>`
- `cptr`: `i64`

**Returns:** `IpcMsg`

*Defined at line 102*

---

## ipc_new

```fides
pub fn ipc_new() -> IpcKernel {
```

**Returns:** `IpcKernel`

*Defined at line 108*

---

## ipc_thread_new

```fides
pub fn ipc_thread_new(k: &mut IpcKernel, name: str) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `name`: `str`

**Returns:** `i64`

*Defined at line 114*

---

## ipc_thread

```fides
pub fn ipc_thread(k: &IpcKernel, tid: i64) -> IpcThread {
```

**Parameters:**

- `k`: `&IpcKernel`
- `tid`: `i64`

**Returns:** `IpcThread`

*Defined at line 122*

---

## ipc_state

```fides
pub fn ipc_state(k: &IpcKernel, tid: i64) -> i64 {
```

**Parameters:**

- `k`: `&IpcKernel`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 126*

---

## ipc_state_name

```fides
pub fn ipc_state_name(st: i64) -> str {
```

**Parameters:**

- `st`: `i64`

**Returns:** `str`

*Defined at line 130*

---

## ipc_cap

The cap in slot cptr of thread tid (kind CAP_NULL when empty/invalid).

```fides
pub fn ipc_cap(k: &IpcKernel, tid: i64, cptr: i64) -> Cap {
```

**Parameters:**

- `k`: `&IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`

**Returns:** `Cap`

*Defined at line 162*

---

## ipc_endpoint_new

Create an endpoint; the creator gets a cap with all rights. Returns
the cptr.

```fides
pub fn ipc_endpoint_new(k: &mut IpcKernel, tid: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 180*

---

## ipc_cap_copy

Derive a cap into another thread's CSpace (rights masked, badge set
when non-zero and the source is unbadged). Returns the new cptr or an
error.

```fides
pub fn ipc_cap_copy(k: &mut IpcKernel, from: i64, cptr: i64, to: i64, rights: i64, badge: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `from`: `i64`
- `cptr`: `i64`
- `to`: `i64`
- `rights`: `i64`
- `badge`: `i64`

**Returns:** `i64`

*Defined at line 205*

---

## ipc_mint

Mint a derived cap in the same CSpace.

```fides
pub fn ipc_mint(k: &mut IpcKernel, tid: i64, cptr: i64, rights: i64, badge: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`
- `rights`: `i64`
- `badge`: `i64`

**Returns:** `i64`

*Defined at line 218*

---

## ipc_cap_delete

```fides
pub fn ipc_cap_delete(k: &mut IpcKernel, tid: i64, cptr: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`

**Returns:** `i64`

*Defined at line 230*

---

## ipc_revoke

Delete every cap derived (directly or not) from the cap in slot cptr;
the cap itself stays. Returns how many were deleted.

```fides
pub fn ipc_revoke(k: &mut IpcKernel, tid: i64, cptr: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`

**Returns:** `i64`

*Defined at line 240*

---

## ipc_send

Send m through endpoint cap cptr; blocks until a receiver takes it.

```fides
pub fn ipc_send(k: &mut IpcKernel, tid: i64, cptr: i64, m: IpcMsg) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`
- `m`: `IpcMsg`

**Returns:** `i64`

*Defined at line 361*

---

## ipc_nbsend

Send only when a receiver is already waiting (IPC_WOULDBLOCK otherwise).

```fides
pub fn ipc_nbsend(k: &mut IpcKernel, tid: i64, cptr: i64, m: IpcMsg) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`
- `m`: `IpcMsg`

**Returns:** `i64`

*Defined at line 366*

---

## ipc_call

Send and wait for the reply (delivered into the caller's msg).

```fides
pub fn ipc_call(k: &mut IpcKernel, tid: i64, cptr: i64, m: IpcMsg) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`
- `m`: `IpcMsg`

**Returns:** `i64`

*Defined at line 371*

---

## ipc_recv

Receive on endpoint cap cptr: takes a queued sender or blocks.

```fides
pub fn ipc_recv(k: &mut IpcKernel, tid: i64, cptr: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`

**Returns:** `i64`

*Defined at line 401*

---

## ipc_poll

Receive only when a sender is queued.

```fides
pub fn ipc_poll(k: &mut IpcKernel, tid: i64, cptr: i64) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`

**Returns:** `i64`

*Defined at line 406*

---

## ipc_reply

Answer the last call received (one-shot reply right).

```fides
pub fn ipc_reply(k: &mut IpcKernel, tid: i64, m: IpcMsg) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `m`: `IpcMsg`

**Returns:** `i64`

*Defined at line 411*

---

## ipc_reply_recv

Reply to the current caller, then wait for the next message: the
usual server loop step.

```fides
pub fn ipc_reply_recv(k: &mut IpcKernel, tid: i64, cptr: i64, m: IpcMsg) -> i64 {
```

**Parameters:**

- `k`: `&mut IpcKernel`
- `tid`: `i64`
- `cptr`: `i64`
- `m`: `IpcMsg`

**Returns:** `i64`

*Defined at line 431*

---

## ipc_msg_of

Message last delivered to tid.

```fides
pub fn ipc_msg_of(k: &IpcKernel, tid: i64) -> IpcMsg {
```

**Parameters:**

- `k`: `&IpcKernel`
- `tid`: `i64`

**Returns:** `IpcMsg`

*Defined at line 437*

---

## RingDesc

```fides
pub struct RingDesc {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `len` | `i64` |
| `flags` | `i64` |
| `cookie` | `i64` |

*Defined at line 448*

---

## shmring_new

```fides
pub fn shmring_new(capacity: i64) -> Vec<i64> {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 455*

---

## shmring_count

```fides
pub fn shmring_count(r: &Vec<i64>) -> i64 {
```

**Parameters:**

- `r`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 461*

---

## shmring_space

```fides
pub fn shmring_space(r: &Vec<i64>) -> i64 {
```

**Parameters:**

- `r`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 465*

---

## shmring_push

Producer side: false when the ring is full.

```fides
pub fn shmring_push(r: &mut Vec<i64>, d: RingDesc) -> bool {
```

**Parameters:**

- `r`: `&mut Vec<i64>`
- `d`: `RingDesc`

**Returns:** `bool`

*Defined at line 470*

---

## shmring_pop

Consumer side: the next descriptor, len = -1 when the ring is empty.

```fides
pub fn shmring_pop(r: &mut Vec<i64>) -> RingDesc {
```

**Parameters:**

- `r`: `&mut Vec<i64>`

**Returns:** `RingDesc`

*Defined at line 487*

---

## shmring_set_event

Consumer asks to be notified once the producer index passes idx.

```fides
pub fn shmring_set_event(r: &mut Vec<i64>, idx: i64) {
```

**Parameters:**

- `r`: `&mut Vec<i64>`
- `idx`: `i64`

*Defined at line 503*

---

## shmring_need_event

virtio's vring_need_event: notify when moving the index from old to
new crossed event (all arithmetic mod 2^16).

```fides
pub fn shmring_need_event(event: i64, new_idx: i64, old_idx: i64) -> bool {
```

**Parameters:**

- `event`: `i64`
- `new_idx`: `i64`
- `old_idx`: `i64`

**Returns:** `bool`

*Defined at line 509*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
