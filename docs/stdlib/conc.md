[All modules](index.md)

# conc -- Fides Documentation

## Contents

- [pert_thread_spawn_v](#pert_thread_spawn_v) (function)
- [pert_executor_spawn_v](#pert_executor_spawn_v) (function)
- [pe_thread_main](#pe_thread_main) (function)
- [pe_worker_main](#pe_worker_main) (function)
- [pe_co_main](#pe_co_main) (function)
- [pert_c_p_co_stack](#pert_c_p_co_stack) (function)
- [pert_co_go](#pert_co_go) (function)
- [pert_co_spawn_v](#pert_co_spawn_v) (function)
- [pert_co_async_v](#pert_co_async_v) (function)
- [pert_co_yield](#pert_co_yield) (function)
- [pert_co_tick](#pert_co_tick) (function)
- [pert_co_run](#pert_co_run) (function)
- [pert_co_sleep](#pert_co_sleep) (function)
- [pert_co_detach](#pert_co_detach) (function)
- [pert_co_done](#pert_co_done) (function)
- [pert_co_result](#pert_co_result) (function)
- [pert_co_wait_fd](#pert_co_wait_fd) (function)
- [pert_task_token](#pert_task_token) (function)
- [pert_task_set_token](#pert_task_set_token) (function)
- [pe_co_print_chain](#pe_co_print_chain) (function)
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
- [pert_chan_try_recv_into](#pert_chan_try_recv_into) (function)
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

*Defined at line 342*

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

*Defined at line 374*

---

## pe_thread_main

Runs on the new thread: its signal stack, the function, its result and
done flag.

```fides
pub fn pe_thread_main(block: i64) -> i64 {
```

**Parameters:**

- `block`: `i64`

**Returns:** `i64`

*Defined at line 387*

---

## pe_worker_main

A worker: run queued tasks for good, asleep while there are none.

```fides
pub fn pe_worker_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 951*

---

## pe_co_main

The body of every task: its function, its result, then back for good.

```fides
pub fn pe_co_main(block: i64) -> i64 {
```

**Parameters:**

- `block`: `i64`

**Returns:** `i64`

*Defined at line 1053*

---

## pert_c_p_co_stack

LG11: the C runtime's `extern "C" fn p_co_stack(bytes) -> i64` (the
stack of the tasks started from now on; also asked as
p_thread_spawn("__p_co_stack", 1, bytes)). Here every task stack is
PE_TASK_STACK and tasks run on several workers at once: -2, nothing
changes (fid run's interpreter then runs each task to its end when it
starts).

```fides
pub fn pert_c_p_co_stack(bytes: i64) -> i64 {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `i64`

*Defined at line 1153*

---

## pert_co_go

```fides
pub fn pert_co_go(name: str, arg: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 1160*

---

## pert_co_spawn_v

spawn f(args): a green task as a tagged future.

```fides
pub fn pert_co_spawn_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 1170*

---

## pert_co_async_v

async f(args): the task runs at once, here, until it first gives
control back.

```fides
pub fn pert_co_async_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 1178*

---

## pert_co_yield

```fides
pub fn pert_co_yield() {
```

*Defined at line 1185*

---

## pert_co_tick

Loop-backedge safepoint: yield only when other tasks are queued (on the
global queue or this worker's ring: the other rings have their own
workers, and thieves).

```fides
pub fn pert_co_tick() {
```

*Defined at line 1194*

---

## pert_co_run

`task_run()`: wait until every task has finished; how many there have
been.

```fides
pub fn pert_co_run() -> i64 {
```

**Returns:** `i64`

*Defined at line 1210*

---

## pert_co_sleep

CA7: sleep, in slices when the slot holds a token: 1 when the token
stopped it, 0 when the whole time passed.

```fides
pub fn pert_co_sleep(ms: i64) -> i64 {
```

**Parameters:**

- `ms`: `i64`

**Returns:** `i64`

*Defined at line 1287*

---

## pert_co_detach

```fides
pub fn pert_co_detach(t: i64) {
```

**Parameters:**

- `t`: `i64`

*Defined at line 1333*

---

## pert_co_done

```fides
pub fn pert_co_done(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1358*

---

## pert_co_result

```fides
pub fn pert_co_result(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 1362*

---

## pert_co_wait_fd

Wait until fd is readable (or writable): 1, or 0 when `timeout_ms`
(>= 0) passes first. A task gives control back meanwhile. CA7: with a
token in the slot, it waits in slices and gives 0 once it is cancelled.

```fides
pub fn pert_co_wait_fd(fd: i64, write: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `write`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 1681*

---

## pert_task_token

The token in the running task's slot (p_task_token), 0 if none.

```fides
pub fn pert_task_token() -> i64 {
```

**Returns:** `i64`

*Defined at line 1771*

---

## pert_task_set_token

Put `tok` in the slot (p_task_set_token); the token it held.

```fides
pub fn pert_task_set_token(tok: i64) -> i64 {
```

**Parameters:**

- `tok`: `i64`

**Returns:** `i64`

*Defined at line 1784*

---

## pe_co_print_chain

`task chain: ...` on fd when a task is running; nothing otherwise.

```fides
pub fn pe_co_print_chain(fd: i64) {
```

**Parameters:**

- `fd`: `i64`

*Defined at line 1875*

---

## pert_future_ready

```fides
pub fn pert_future_ready(value: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`

**Returns:** `i64`

*Defined at line 1920*

---

## pert_future_poll

```fides
pub fn pert_future_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 1924*

---

## pert_task_poll

```fides
pub fn pert_task_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 1938*

---

## pert_task_is_ready

```fides
pub fn pert_task_is_ready(f: i64) -> bool {
```

**Parameters:**

- `f`: `i64`

**Returns:** `bool`

*Defined at line 1942*

---

## pert_task_poll_nonblocking

```fides
pub fn pert_task_poll_nonblocking(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 1956*

---

## pert_executor_new

```fides
pub fn pert_executor_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 1976*

---

## pert_executor_block_on

```fides
pub fn pert_executor_block_on(exec: i64, f: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`
- `f`: `i64`

**Returns:** `i64`

*Defined at line 2054*

---

## pert_executor_run_all

Join every thread and task still running; the number joined. `spawn
scope { }` exits through here (BG196: the tasks started inside it).

```fides
pub fn pert_executor_run_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2060*

---

## pert_executor_poll_all

Join the finished threads; the number still running.

```fides
pub fn pert_executor_poll_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2078*

---

## pert_executor_cancel

Cancel the tasks of a scope: a task cannot be stopped mid-way, so wait
for those still running, drop their results (0), and return how many
were cancelled.

```fides
pub fn pert_executor_cancel(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 2101*

---

## pert_shared_alloc

```fides
pub fn pert_shared_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 2124*

---

## pert_atomic_load

```fides
pub fn pert_atomic_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 2131*

---

## pert_atomic_store

```fides
pub fn pert_atomic_store(addr: i64, value: i64) {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

*Defined at line 2136*

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

*Defined at line 2142*

---

## pert_atomic_sub

The previous value.

```fides
pub fn pert_atomic_sub(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 2148*

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

*Defined at line 2152*

---

## pert_mutex_init

A mutex: a futex word, 0 free, 1 held, 2 held with waiters (not
recursive). A thread waits asleep in the kernel; a task (or a thread
with tasks to run) lets the others run, so a task holding it across a
yield gets to release it.

```fides
pub fn pert_mutex_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 2161*

---

## pert_mutex_lock

```fides
pub fn pert_mutex_lock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 2166*

---

## pert_mutex_unlock

```fides
pub fn pert_mutex_unlock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 2186*

---

## pert_tls_create

```fides
pub fn pert_tls_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 2198*

---

## pert_tls_get

```fides
pub fn pert_tls_get(key: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`

**Returns:** `i64`

*Defined at line 2218*

---

## pert_tls_set

```fides
pub fn pert_tls_set(key: i64, value: i64) {
```

**Parameters:**

- `key`: `i64`
- `value`: `i64`

*Defined at line 2225*

---

## pert_chan_create

```fides
pub fn pert_chan_create(capacity: i64) -> i64 {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `i64`

*Defined at line 2239*

---

## pert_chan_send

Wait until there is room or the channel is closed; false if closed, or
(CA7) once the token in the slot is cancelled.

```fides
pub fn pert_chan_send(ch: i64, value: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 2277*

---

## pert_chan_recv

Wait until there is a value or the channel is closed and empty (then
the pending value; CA7: also once the token in the slot is cancelled).

```fides
pub fn pert_chan_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 2318*

---

## pert_chan_try_recv

A value if one is waiting; else 0 if closed, the pending value if not.

```fides
pub fn pert_chan_try_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 2346*

---

## pert_chan_try_recv_into

BG241: select's receive, "a value was taken" out of band: 1 with the
value (0 from a closed, empty channel) stored at `out`, 0 when nothing
is ready (a value equal to the pending word is a value).

```fides
pub fn pert_chan_try_recv_into(ch: i64, out: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`
- `out`: `i64`

**Returns:** `i64`

*Defined at line 2368*

---

## pert_chan_close

```fides
pub fn pert_chan_close(ch: i64) {
```

**Parameters:**

- `ch`: `i64`

*Defined at line 2389*

---

## pert_chan_is_closed

```fides
pub fn pert_chan_is_closed(ch: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `bool`

*Defined at line 2399*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
