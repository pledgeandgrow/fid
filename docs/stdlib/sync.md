# sync — Fides Documentation

## Contents

- [sync_atomic_alloc](#sync_atomic_alloc) (function)
- [sync_atomic_load](#sync_atomic_load) (function)
- [sync_atomic_store](#sync_atomic_store) (function)
- [sync_atomic_cas](#sync_atomic_cas) (function)
- [sync_atomic_add](#sync_atomic_add) (function)
- [sync_mutex_create](#sync_mutex_create) (function)
- [sync_mutex_lock](#sync_mutex_lock) (function)
- [sync_mutex_unlock](#sync_mutex_unlock) (function)
- [sync_tls_create](#sync_tls_create) (function)
- [sync_tls_get](#sync_tls_get) (function)
- [sync_tls_set](#sync_tls_set) (function)
- [sync_rwlock_create](#sync_rwlock_create) (function)
- [sync_rwlock_read_lock](#sync_rwlock_read_lock) (function)
- [sync_rwlock_read_unlock](#sync_rwlock_read_unlock) (function)
- [sync_rwlock_write_lock](#sync_rwlock_write_lock) (function)
- [sync_rwlock_write_unlock](#sync_rwlock_write_unlock) (function)
- [atomic_u64_new](#atomic_u64_new) (function)
- [atomic_u64_load](#atomic_u64_load) (function)
- [atomic_u64_store](#atomic_u64_store) (function)
- [atomic_u64_fetch_add](#atomic_u64_fetch_add) (function)
- [atomic_u64_cas](#atomic_u64_cas) (function)
- [sync_spawn](#sync_spawn) (function)
- [SYNC_CHAN_PENDING:](#sync_chan_pending:) (const)
- [sync_channel_create](#sync_channel_create) (function)
- [sync_channel_send](#sync_channel_send) (function)
- [sync_channel_recv](#sync_channel_recv) (function)
- [sync_channel_try_recv](#sync_channel_try_recv) (function)
- [sync_channel_close](#sync_channel_close) (function)
- [sync_channel_is_closed](#sync_channel_is_closed) (function)

## sync_atomic_alloc

Allocate a shared atomic i64 slot, initialized to 0.
Returns an address that can be used with atomic operations.

```fides
pub fn sync_atomic_alloc(initial: i64) -> i64 {
```

**Parameters:**

- `initial`: `i64`

**Returns:** `i64`

*Defined at line 23*

---

## sync_atomic_load

Atomically load the value at an address.

```fides
pub fn sync_atomic_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 30*

---

## sync_atomic_store

Atomically store a value at an address.

```fides
pub fn sync_atomic_store(addr: i64, value: i64) -> void {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 35*

---

## sync_atomic_cas

Atomically compare-and-swap. Returns true if the swap succeeded.

```fides
pub fn sync_atomic_cas(addr: i64, expected: i64, new_val: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`
- `expected`: `i64`
- `new_val`: `i64`

**Returns:** `bool`

*Defined at line 40*

---

## sync_atomic_add

Atomically add a value. Returns the old value before the add.

```fides
pub fn sync_atomic_add(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 45*

---

## sync_mutex_create

Create a mutex. Returns a mutex handle.

```fides
pub fn sync_mutex_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 50*

---

## sync_mutex_lock

Lock a mutex (blocks if contended by another thread).

```fides
pub fn sync_mutex_lock(m: i64) -> void {
```

**Parameters:**

- `m`: `i64`

**Returns:** `void`

*Defined at line 55*

---

## sync_mutex_unlock

Unlock a mutex.

```fides
pub fn sync_mutex_unlock(m: i64) -> void {
```

**Parameters:**

- `m`: `i64`

**Returns:** `void`

*Defined at line 60*

---

## sync_tls_create

Create a thread-local storage key. Returns a key handle.
Each thread can store its own value for this key.

```fides
pub fn sync_tls_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 81*

---

## sync_tls_get

Get the thread-local value for the current thread. Returns 0 if unset.

```fides
pub fn sync_tls_get(key: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`

**Returns:** `i64`

*Defined at line 86*

---

## sync_tls_set

Set the thread-local value for the current thread.

```fides
pub fn sync_tls_set(key: i64, value: i64) -> void {
```

**Parameters:**

- `key`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 91*

---

## sync_rwlock_create

Create a reader-writer lock. Returns a handle.
Allows multiple concurrent readers or one exclusive writer.

```fides
pub fn sync_rwlock_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 101*

---

## sync_rwlock_read_lock

Acquire a read lock (shared — multiple readers allowed).

```fides
pub fn sync_rwlock_read_lock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 120*

---

## sync_rwlock_read_unlock

Release a read lock.

```fides
pub fn sync_rwlock_read_unlock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 136*

---

## sync_rwlock_write_lock

Acquire a write lock (exclusive — no other readers or writers).

```fides
pub fn sync_rwlock_write_lock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 150*

---

## sync_rwlock_write_unlock

Release a write lock.

```fides
pub fn sync_rwlock_write_unlock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 158*

---

## atomic_u64_new

Create an AtomicU64 initialized to the given value.

```fides
pub fn atomic_u64_new(initial: i64) -> i64 {
```

**Parameters:**

- `initial`: `i64`

**Returns:** `i64`

*Defined at line 170*

---

## atomic_u64_load

Atomically load an AtomicU64.

```fides
pub fn atomic_u64_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 175*

---

## atomic_u64_store

Atomically store a value to an AtomicU64.

```fides
pub fn atomic_u64_store(addr: i64, value: i64) -> void {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 180*

---

## atomic_u64_fetch_add

Atomically add a value to an AtomicU64. Returns the old value.

```fides
pub fn atomic_u64_fetch_add(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 185*

---

## atomic_u64_cas

Atomically compare-and-swap. Returns true if successful.

```fides
pub fn atomic_u64_cas(addr: i64, expected: i64, new_val: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`
- `expected`: `i64`
- `new_val`: `i64`

**Returns:** `bool`

*Defined at line 190*

---

## sync_spawn

Spawn a function as a new OS thread.
Returns a Future handle. Use `await` to join the thread and get the result.
The function must take i64 parameters and return i64.

```fides
pub fn sync_spawn(func_name: str, num_args: i64) -> i64 {
```

**Parameters:**

- `func_name`: `str`
- `num_args`: `i64`

**Returns:** `i64`

*Defined at line 197*

---

## SYNC_CHAN_PENDING:

```fides
pub const SYNC_CHAN_PENDING: i64 = -9223372036854775807;
```

*Defined at line 220*

---

## sync_channel_create

Create a bounded channel with the given capacity.
Returns a channel handle (i64).

```fides
pub fn sync_channel_create(capacity: i64) -> i64 {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `i64`

*Defined at line 224*

---

## sync_channel_send

Send a value through the channel. Blocks if the buffer is full.
Returns true if the value was sent, false if the channel is closed.

```fides
pub fn sync_channel_send(ch: i64, value: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 230*

---

## sync_channel_recv

Receive a value from the channel. Blocks if the buffer is empty.
Returns the value, or SYNC_CHAN_PENDING if the channel is closed and empty.

```fides
pub fn sync_channel_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 236*

---

## sync_channel_try_recv

Non-blocking receive. Returns the value if available,
SYNC_CHAN_PENDING if the channel is empty (but not closed),
or 0 if the channel is closed and empty.

```fides
pub fn sync_channel_try_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 243*

---

## sync_channel_close

Close the channel. Signals receivers that no more values will be sent.

```fides
pub fn sync_channel_close(ch: i64) -> void {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `void`

*Defined at line 248*

---

## sync_channel_is_closed

Check if the channel has been closed.

```fides
pub fn sync_channel_is_closed(ch: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `bool`

*Defined at line 253*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
