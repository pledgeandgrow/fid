[All modules](index.md)

# sync -- Fides Documentation

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
- [SYNC_CHAN_PENDING:](#SYNC_CHAN_PENDING:) (const)
- [sync_channel_create](#sync_channel_create) (function)
- [sync_channel_send](#sync_channel_send) (function)
- [sync_channel_recv](#sync_channel_recv) (function)
- [sync_channel_try_recv](#sync_channel_try_recv) (function)
- [sync_channel_close](#sync_channel_close) (function)
- [sync_channel_is_closed](#sync_channel_is_closed) (function)
- [with_lock](#with_lock) (function)
- [with_write_lock](#with_write_lock) (function)
- [with_read_lock](#with_read_lock) (function)
- [SyncMutex](#SyncMutex) (struct)
- [SyncMutex](#SyncMutex) (impl)
- [SyncRwLock](#SyncRwLock) (struct)
- [SyncRwLock](#SyncRwLock) (impl)
- [SyncCounter](#SyncCounter) (struct)
- [SyncCounter](#SyncCounter) (impl)
- [SyncTlsKey](#SyncTlsKey) (struct)
- [SyncTlsKey](#SyncTlsKey) (impl)
- [SyncChannel](#SyncChannel) (struct)
- [SyncChannel](#SyncChannel) (impl)

## sync_atomic_alloc

Allocate a shared atomic i64 slot, initialized to 0.
Returns an address that can be used with atomic operations.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val a = sync_atomic_alloc(5);
assert_eq(sync_atomic_add(a, 2), 5);
assert_eq(sync_atomic_load(a), 7);
assert(sync_atomic_cas(a, 7, 9));
assert(!sync_atomic_cas(a, 7, 1));
sync_atomic_store(a, 0);
assert_eq(sync_atomic_load(a), 0);
val m = sync_mutex_create();
sync_mutex_lock(m);
sync_mutex_unlock(m);
```

```fides
pub fn sync_atomic_alloc(initial: i64) -> i64 {
```

**Parameters:**

- `initial`: `i64`

**Returns:** `i64`

*Defined at line 34*

---

## sync_atomic_load

Atomically load the value at an address.

```fides
pub fn sync_atomic_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 41*

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

*Defined at line 46*

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

*Defined at line 51*

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

*Defined at line 56*

---

## sync_mutex_create

Create a mutex. Returns a mutex handle.

```fides
pub fn sync_mutex_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 61*

---

## sync_mutex_lock

Lock a mutex (blocks if contended by another thread).

```fides
pub fn sync_mutex_lock(m: i64) -> void {
```

**Parameters:**

- `m`: `i64`

**Returns:** `void`

*Defined at line 66*

---

## sync_mutex_unlock

Unlock a mutex.

```fides
pub fn sync_mutex_unlock(m: i64) -> void {
```

**Parameters:**

- `m`: `i64`

**Returns:** `void`

*Defined at line 71*

---

## sync_tls_create

Create a thread-local storage key. Returns a key handle.
Each thread can store its own value for this key.

```fides
pub fn sync_tls_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 92*

---

## sync_tls_get

Get the thread-local value for the current thread. Returns 0 if unset.

```fides
pub fn sync_tls_get(key: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`

**Returns:** `i64`

*Defined at line 97*

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

*Defined at line 102*

---

## sync_rwlock_create

Create a reader-writer lock. Returns a handle.
Allows multiple concurrent readers or one exclusive writer.

```fides
pub fn sync_rwlock_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 112*

---

## sync_rwlock_read_lock

Acquire a read lock (shared — multiple readers allowed).

```fides
pub fn sync_rwlock_read_lock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 133*

---

## sync_rwlock_read_unlock

Release a read lock.

```fides
pub fn sync_rwlock_read_unlock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 153*

---

## sync_rwlock_write_lock

Acquire a write lock (exclusive — no other readers or writers).

```fides
pub fn sync_rwlock_write_lock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 169*

---

## sync_rwlock_write_unlock

Release a write lock.

```fides
pub fn sync_rwlock_write_unlock(rwlock: i64) -> void {
```

**Parameters:**

- `rwlock`: `i64`

**Returns:** `void`

*Defined at line 178*

---

## atomic_u64_new

Create an AtomicU64 initialized to the given value.

```fides
pub fn atomic_u64_new(initial: i64) -> i64 {
```

**Parameters:**

- `initial`: `i64`

**Returns:** `i64`

*Defined at line 191*

---

## atomic_u64_load

Atomically load an AtomicU64.

```fides
pub fn atomic_u64_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 196*

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

*Defined at line 201*

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

*Defined at line 206*

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

*Defined at line 211*

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

*Defined at line 218*

---

## SYNC_CHAN_PENDING:

```fides
pub const SYNC_CHAN_PENDING: i64 = -9223372036854775807;
```

*Defined at line 241*

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

*Defined at line 244*

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

*Defined at line 250*

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

*Defined at line 256*

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

*Defined at line 263*

---

## sync_channel_close

Close the channel. Signals receivers that no more values will be sent.

```fides
pub fn sync_channel_close(ch: i64) -> void {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `void`

*Defined at line 268*

---

## sync_channel_is_closed

Check if the channel has been closed.

```fides
pub fn sync_channel_is_closed(ch: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `bool`

*Defined at line 273*

---

## with_lock

Lock `m`, run `f`, and release the lock when `f` returns — including the
`?`-propagation and panic paths, because the release rides `defer`.

val m = sync_mutex_create();
val total = with_lock(m, tally);

Prefer this over manual lock/unlock pairs: it cannot forget to unlock.

```fides
pub fn with_lock(m: i64, f: fn() -> i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `f`: `fn() -> i64`

**Returns:** `i64`

*Defined at line 286*

---

## with_write_lock

Same, for read-write locks: holds the write lock for `f`'s duration.

```fides
pub fn with_write_lock(rwlock: i64, f: fn() -> i64) -> i64 {
```

**Parameters:**

- `rwlock`: `i64`
- `f`: `fn() -> i64`

**Returns:** `i64`

*Defined at line 295*

---

## with_read_lock

Same, for read-write locks: holds the read lock for `f`'s duration.

```fides
pub fn with_read_lock(rwlock: i64, f: fn() -> i64) -> i64 {
```

**Parameters:**

- `rwlock`: `i64`
- `f`: `fn() -> i64`

**Returns:** `i64`

*Defined at line 304*

---

## SyncMutex

```fides
pub struct SyncMutex {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |

*Defined at line 333*

---

## SyncMutex

```fides
impl SyncMutex {
```

**Methods:**

### new

A new, unlocked mutex.

```fides
pub fn new() -> SyncMutex {
```

### lock

Lock it (blocks while another thread holds it).

```fides
pub fn lock(self: SyncMutex) -> void {
```

### unlock

Unlock it.

```fides
pub fn unlock(self: SyncMutex) -> void {
```

### locked

Lock, run `f`, unlock (also on the error and panic paths).

```fides
pub fn locked(self: SyncMutex, f: fn() -> i64) -> i64 {
```

*Defined at line 337*

---

## SyncRwLock

```fides
pub struct SyncRwLock {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |

*Defined at line 363*

---

## SyncRwLock

```fides
impl SyncRwLock {
```

**Methods:**

### new

A new, unlocked reader-writer lock.

```fides
pub fn new() -> SyncRwLock {
```

### read_lock

```fides
pub fn read_lock(self: SyncRwLock) -> void {
```

### read_unlock

```fides
pub fn read_unlock(self: SyncRwLock) -> void {
```

### write_lock

```fides
pub fn write_lock(self: SyncRwLock) -> void {
```

### write_unlock

```fides
pub fn write_unlock(self: SyncRwLock) -> void {
```

### with_read

Hold the read lock while `f` runs.

```fides
pub fn with_read(self: SyncRwLock, f: fn() -> i64) -> i64 {
```

### with_write

Hold the write lock while `f` runs.

```fides
pub fn with_write(self: SyncRwLock, f: fn() -> i64) -> i64 {
```

*Defined at line 367*

---

## SyncCounter

```fides
pub struct SyncCounter {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |

*Defined at line 404*

---

## SyncCounter

```fides
impl SyncCounter {
```

**Methods:**

### new

A new counter holding `initial`.

Example:
val c = SyncCounter::new(1);
assert_eq(c.add(4), 1);
assert_eq(c.load(), 5);
assert(c.cas(5, 6));
c.store(0);
assert_eq(c.load(), 0);

```fides
pub fn new(initial: i64) -> SyncCounter {
```

### load

```fides
pub fn load(self: SyncCounter) -> i64 {
```

### store

```fides
pub fn store(self: SyncCounter, value: i64) -> void {
```

### add

Add `value`; the value before the add.

```fides
pub fn add(self: SyncCounter, value: i64) -> i64 {
```

### cas

Set it to `new_val` if it holds `expected`: true when it did.

```fides
pub fn cas(self: SyncCounter, expected: i64, new_val: i64) -> bool {
```

*Defined at line 408*

---

## SyncTlsKey

```fides
pub struct SyncTlsKey {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `i64` |

*Defined at line 445*

---

## SyncTlsKey

```fides
impl SyncTlsKey {
```

**Methods:**

### new

```fides
pub fn new() -> SyncTlsKey {
```

### get

This thread's value.

```fides
pub fn get(self: SyncTlsKey) -> i64 {
```

### set

Set this thread's value.

```fides
pub fn set(self: SyncTlsKey, value: i64) -> void {
```

*Defined at line 449*

---

## SyncChannel

```fides
pub struct SyncChannel {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |

*Defined at line 469*

---

## SyncChannel

```fides
impl SyncChannel {
```

**Methods:**

### new

A channel holding up to `capacity` values.

```fides
pub fn new(capacity: i64) -> SyncChannel {
```

### send

Send (blocks while full): false when the channel is closed.

```fides
pub fn send(self: SyncChannel, value: i64) -> bool {
```

### recv

Receive (blocks while empty): SYNC_CHAN_PENDING once closed and empty.

```fides
pub fn recv(self: SyncChannel) -> i64 {
```

### try_recv

Receive without blocking: SYNC_CHAN_PENDING when empty.

```fides
pub fn try_recv(self: SyncChannel) -> i64 {
```

### close

```fides
pub fn close(self: SyncChannel) -> void {
```

### is_closed

```fides
pub fn is_closed(self: SyncChannel) -> bool {
```

*Defined at line 473*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
