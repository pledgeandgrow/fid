# conc — Fides Documentation

From `stdlib/pe_runtime/conc.fd`: the concurrency part of the runtime of
native Windows executables (`fid emit`, pe_ir), which runs builtin `p_X`
as `pert_X`. It is merged into those builds, not imported with `use`.

## Contents

- [pert_thread_spawn_v](#pert_thread_spawn_v) (function)
- [pert_executor_spawn_v](#pert_executor_spawn_v) (function)
- [pert_thread_main](#pert_thread_main) (function)
- [pert_co_go](#pert_co_go) (function)
- [pert_co_yield](#pert_co_yield) (function)
- [pert_co_run](#pert_co_run) (function)
- [pert_co_done](#pert_co_done) (function)
- [pert_co_result](#pert_co_result) (function)
- [pert_future_ready](#pert_future_ready) (function)
- [pert_future_poll](#pert_future_poll) (function)
- [pert_task_poll](#pert_task_poll) (function)
- [pert_task_is_ready](#pert_task_is_ready) (function)
- [pert_task_poll_nonblocking](#pert_task_poll_nonblocking) (function)
- [pert_executor_new](#pert_executor_new) (function)
- [pert_executor_block_on](#pert_executor_block_on) (function)
- [pert_executor_run_all](#pert_executor_run_all) (function)
- [pert_executor_poll_all](#pert_executor_poll_all) (function)
- [pert_executor_cancel](#pert_executor_cancel) (function)
- [pert_shared_alloc](#pert_shared_alloc) (function)
- [pert_atomic_load](#pert_atomic_load) (function)
- [pert_atomic_store](#pert_atomic_store) (function)
- [pert_atomic_add](#pert_atomic_add) (function)
- [pert_atomic_sub](#pert_atomic_sub) (function)
- [pert_atomic_cas](#pert_atomic_cas) (function)
- [pert_mutex_init](#pert_mutex_init) (function)
- [pert_mutex_lock](#pert_mutex_lock) (function)
- [pert_mutex_unlock](#pert_mutex_unlock) (function)
- [pert_tls_create](#pert_tls_create) (function)
- [pert_tls_get](#pert_tls_get) (function)
- [pert_tls_set](#pert_tls_set) (function)
- [pert_chan_create](#pert_chan_create) (function)
- [pert_chan_send](#pert_chan_send) (function)
- [pert_chan_recv](#pert_chan_recv) (function)
- [pert_chan_try_recv](#pert_chan_try_recv) (function)
- [pert_chan_close](#pert_chan_close) (function)
- [pert_chan_is_closed](#pert_chan_is_closed) (function)

## pert_thread_spawn_v

Start the program function `name` with argv[0..nargs) on a new thread.

```fides
pub fn pert_thread_spawn_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 310*

---

## pert_executor_spawn_v

```fides
pub fn pert_executor_spawn_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 339*

---

## pert_thread_main

Runs on the new thread: the function, then its result and done flag.

```fides
pub fn pert_thread_main(block: i64) -> i64 {
```

**Parameters:**

- `block`: `i64`

**Returns:** `i64`

*Defined at line 344*

---

## pert_co_go

```fides
pub fn pert_co_go(name: str, arg: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 2026*

---

## pert_co_yield

```fides
pub fn pert_co_yield() {
```

*Defined at line 2053*

---

## pert_co_run

```fides
pub fn pert_co_run() -> i64 {
```

**Returns:** `i64`

*Defined at line 2126*

---

## pert_co_done

```fides
pub fn pert_co_done(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 2574*

---

## pert_co_result

```fides
pub fn pert_co_result(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 2578*

---

## pert_future_ready

```fides
pub fn pert_future_ready(value: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`

**Returns:** `i64`

*Defined at line 2610*

---

## pert_future_poll

```fides
pub fn pert_future_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2614*

---

## pert_task_poll

```fides
pub fn pert_task_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2628*

---

## pert_task_is_ready

```fides
pub fn pert_task_is_ready(f: i64) -> bool {
```

**Parameters:**

- `f`: `i64`

**Returns:** `bool`

*Defined at line 2632*

---

## pert_task_poll_nonblocking

```fides
pub fn pert_task_poll_nonblocking(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2646*

---

## pert_executor_new

```fides
pub fn pert_executor_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 2666*

---

## pert_executor_block_on

```fides
pub fn pert_executor_block_on(exec: i64, f: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`
- `f`: `i64`

**Returns:** `i64`

*Defined at line 2672*

---

## pert_executor_run_all

Join every task still running; the number joined.

```fides
pub fn pert_executor_run_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2679*

---

## pert_executor_poll_all

Join the finished tasks; the number still running.

```fides
pub fn pert_executor_poll_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2702*

---

## pert_executor_cancel

Cancel the tasks of a scope: a task cannot be stopped mid-way, so wait
for those still running (none outlives its scope), drop their results
(0), and return how many were cancelled.

```fides
pub fn pert_executor_cancel(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2721*

---

## pert_shared_alloc

```fides
pub fn pert_shared_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 2740*

---

## pert_atomic_load

```fides
pub fn pert_atomic_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 2747*

---

## pert_atomic_store

```fides
pub fn pert_atomic_store(addr: i64, value: i64) {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

*Defined at line 2752*

---

## pert_atomic_add

The previous value.

```fides
pub fn pert_atomic_add(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 2758*

---

## pert_atomic_sub

```fides
pub fn pert_atomic_sub(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 2765*

---

## pert_atomic_cas

```fides
pub fn pert_atomic_cas(addr: i64, expected: i64, new_value: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`
- `expected`: `i64`
- `new_value`: `i64`

**Returns:** `bool`

*Defined at line 2769*

---

## pert_mutex_init

A mutex: an SRW lock (not recursive, released by its owner, as a
default pthread mutex).

```fides
pub fn pert_mutex_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 2776*

---

## pert_mutex_lock

```fides
pub fn pert_mutex_lock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 2782*

---

## pert_mutex_unlock

```fides
pub fn pert_mutex_unlock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 2787*

---

## pert_tls_create

```fides
pub fn pert_tls_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 2792*

---

## pert_tls_get

```fides
pub fn pert_tls_get(key: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`

**Returns:** `i64`

*Defined at line 2796*

---

## pert_tls_set

```fides
pub fn pert_tls_set(key: i64, value: i64) {
```

**Parameters:**

- `key`: `i64`
- `value`: `i64`

*Defined at line 2800*

---

## pert_chan_create

```fides
pub fn pert_chan_create(capacity: i64) -> i64 {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `i64`

*Defined at line 2808*

---

## pert_chan_send

Block until there is room or the channel is closed; false if closed.

```fides
pub fn pert_chan_send(ch: i64, value: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 2822*

---

## pert_chan_recv

Block until there is a value or the channel is closed and empty.

```fides
pub fn pert_chan_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 2866*

---

## pert_chan_try_recv

A value if one is waiting; else 0 if closed, the pending value if not.

```fides
pub fn pert_chan_try_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 2896*

---

## pert_chan_close

```fides
pub fn pert_chan_close(ch: i64) {
```

**Parameters:**

- `ch`: `i64`

*Defined at line 2914*

---

## pert_chan_is_closed

```fides
pub fn pert_chan_is_closed(ch: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `bool`

*Defined at line 2925*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
