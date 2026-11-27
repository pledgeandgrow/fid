[All modules](index.md)

# conc -- Fides Documentation

## Contents

- [pert_mmap_anon](#pert_mmap_anon) (function)
- [pert_mmap_len](#pert_mmap_len) (function)
- [pert_munmap](#pert_munmap) (function)
- [pert_thread_spawn_v](#pert_thread_spawn_v) (function)
- [pert_executor_spawn_v](#pert_executor_spawn_v) (function)
- [pert_thread_main](#pert_thread_main) (function)
- [pe_co_cur](#pe_co_cur) (function)
- [pert_task_token](#pert_task_token) (function)
- [pert_task_set_token](#pert_task_set_token) (function)
- [pert_clock_mono_ms](#pert_clock_mono_ms) (function)
- [pert_task_chain](#pert_task_chain) (function)
- [pert_worker_main](#pert_worker_main) (function)
- [pert_poller_main](#pert_poller_main) (function)
- [pert_co_pool_call](#pert_co_pool_call) (function)
- [pert_pool_main](#pert_pool_main) (function)
- [pert_c_p_co_stack](#pert_c_p_co_stack) (function)
- [pert_co_go](#pert_co_go) (function)
- [pert_co_spawn_v](#pert_co_spawn_v) (function)
- [pert_co_async_v](#pert_co_async_v) (function)
- [pert_co_yield](#pert_co_yield) (function)
- [pert_co_tick](#pert_co_tick) (function)
- [pert_co_run](#pert_co_run) (function)
- [pert_co_sleep](#pert_co_sleep) (function)
- [pert_co_wait_fd](#pert_co_wait_fd) (function)
- [pert_socket_listen](#pert_socket_listen) (function)
- [pert_unix_listen](#pert_unix_listen) (function)
- [pert_unix_connect](#pert_unix_connect) (function)
- [pert_socket_accept](#pert_socket_accept) (function)
- [pert_socket_read](#pert_socket_read) (function)
- [pert_socket_write](#pert_socket_write) (function)
- [pert_socket_write_str](#pert_socket_write_str) (function)
- [pert_socket_close](#pert_socket_close) (function)
- [pert_net_close](#pert_net_close) (function)
- [pert_socket_recv_str](#pert_socket_recv_str) (function)
- [pert_net_connect](#pert_net_connect) (function)
- [pert_co_accept](#pert_co_accept) (function)
- [pert_co_connect](#pert_co_connect) (function)
- [pert_co_recv](#pert_co_recv) (function)
- [pert_co_send](#pert_co_send) (function)
- [pert_co_detach](#pert_co_detach) (function)
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
- [pert_chan_try_recv_into](#pert_chan_try_recv_into) (function)
- [pert_chan_close](#pert_chan_close) (function)
- [pert_chan_is_closed](#pert_chan_is_closed) (function)
- [pert_fs_copy_file](#pert_fs_copy_file) (function)
- [pert_net_send_file](#pert_net_send_file) (function)
- [pert_fs_map](#pert_fs_map) (function)
- [pert_fs_unmap](#pert_fs_unmap) (function)
- [pert_prof_sampler_main](#pert_prof_sampler_main) (function)
- [pert_prof_watch_main](#pert_prof_watch_main) (function)
- [pert_prof_boot](#pert_prof_boot) (function)
- [pert_prof_exit](#pert_prof_exit) (function)
- [pert_prof_start](#pert_prof_start) (function)
- [pert_prof_stop](#pert_prof_stop) (function)
- [pert_prof_report](#pert_prof_report) (function)
- [pert_prof_samples](#pert_prof_samples) (function)
- [pert_limits_boot](#pert_limits_boot) (function)
- [pert_lim_watch_main](#pert_lim_watch_main) (function)
- [pert_lim_take](#pert_lim_take) (function)
- [pert_lim_give](#pert_lim_give) (function)
- [pert_alloc_chunk](#pert_alloc_chunk) (function)
- [pert_alloc_recommit](#pert_alloc_recommit) (function)
- [pert_alloc_trim](#pert_alloc_trim) (function)
- [pert_alloc_thread_end](#pert_alloc_thread_end) (function)

## pert_mmap_anon

`len` zeroed read/write bytes of their own pages; 0 on failure.

```fides
pub fn pert_mmap_anon(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 152*

---

## pert_mmap_len

The length of mapping `addr`, 0 when it is not one.

```fides
pub fn pert_mmap_len(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 172*

---

## pert_munmap

Unmap `addr`: 0, or -1 when it is not a mapping.

```fides
pub fn pert_munmap(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 184*

---

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

*Defined at line 404*

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

*Defined at line 435*

---

## pert_thread_main

Runs on the new thread: the function, then its result and done flag.

```fides
pub fn pert_thread_main(block: i64) -> i64 {
```

**Parameters:**

- `block`: `i64`

**Returns:** `i64`

*Defined at line 440*

---

## pe_co_cur

The task running on this thread, or 0 (worker scheduler / main / pool).

```fides
pub fn pe_co_cur() -> i64 {
```

**Returns:** `i64`

*Defined at line 662*

---

## pert_task_token

The token in the running task's slot (p_task_token), 0 if none.

```fides
pub fn pert_task_token() -> i64 {
```

**Returns:** `i64`

*Defined at line 1084*

---

## pert_task_set_token

Put `tok` in the slot (p_task_set_token); the token it held.

```fides
pub fn pert_task_set_token(tok: i64) -> i64 {
```

**Parameters:**

- `tok`: `i64`

**Returns:** `i64`

*Defined at line 1096*

---

## pert_clock_mono_ms

C6: milliseconds since the system started (GetTickCount64), the clock
of deadlines (timeout blocks, tokens): a change of the wall clock does
not move it.

```fides
pub fn pert_clock_mono_ms() -> i64 {
```

**Returns:** `i64`

*Defined at line 1112*

---

## pert_task_chain

`task chain: ...` on stderr when a task is running; nothing otherwise.

```fides
pub fn pert_task_chain() {
```

*Defined at line 1198*

---

## pert_worker_main

Worker OS thread: scheduler context + pop-run loop forever.

```fides
pub fn pert_worker_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 1842*

---

## pert_poller_main

Poller OS thread: the only WSAPoll caller. Each cycle it rebuilds the
wait set from the registry, polls with a deadline-bounded timeout, then
wakes: fd-ready tasks, expired sleepers/timeouts, and joiners whose
awaited task finished. It also owns deadlock detection.

```fides
pub fn pert_poller_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 1876*

---

## pert_co_pool_call

Run program fn `name` on the blocking pool instead of this worker.
Off-task, just calls it directly. Blocking builtins (file IO, DNS) route
through here so a syscall never stalls a scheduler worker.

```fides
pub fn pert_co_pool_call(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 2232*

---

## pert_pool_main

Pool OS thread: run each submitted job, then requeue the waiting task.

```fides
pub fn pert_pool_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 2272*

---

## pert_c_p_co_stack

LG11: the C runtime's `extern "C" fn p_co_stack(bytes) -> i64` (the
stack of the tasks started from now on; also asked as
p_thread_spawn("__p_co_stack", 1, bytes)). Here a task's stack has one
size and tasks run on several workers at once: -2, nothing changes
(fid run's interpreter then runs each task to its end when it starts).

```fides
pub fn pert_c_p_co_stack(bytes: i64) -> i64 {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `i64`

*Defined at line 2305*

---

## pert_co_go

```fides
pub fn pert_co_go(name: str, arg: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 2312*

---

## pert_co_spawn_v

spawn f(args): a green task as a tagged future (L3B/M3).

```fides
pub fn pert_co_spawn_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 2321*

---

## pert_co_async_v

```fides
pub fn pert_co_async_v(name: str, nargs: i64, argv: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `nargs`: `i64`
- `argv`: `i64`

**Returns:** `i64`

*Defined at line 2326*

---

## pert_co_yield

```fides
pub fn pert_co_yield() {
```

*Defined at line 2341*

---

## pert_co_tick

Loop-backedge safepoint (M5): yield only when work is queued behind us,
so tight loops can't starve other tasks but don't pay when uncontended.

```fides
pub fn pert_co_tick() {
```

*Defined at line 2403*

---

## pert_co_run

`task_run()`: workers are already running every task; wait until the
registry drains. Returns how many tasks there have been.

```fides
pub fn pert_co_run() -> i64 {
```

**Returns:** `i64`

*Defined at line 2414*

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

*Defined at line 2471*

---

## pert_co_wait_fd

CA7: with a token in the slot, the wait goes in slices and gives 0 (not
ready) once the token is cancelled.

```fides
pub fn pert_co_wait_fd(fd: i64, write: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `write`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 2518*

---

## pert_socket_listen

A TCP socket on all addresses, listening, or -1: IPv6 and IPv4 clients on
one socket (IPV6_V6ONLY off, Windows has it on by default), IPv4 alone
where the system has no IPv6.

```fides
pub fn pert_socket_listen(port: i64) -> i64 {
```

**Parameters:**

- `port`: `i64`

**Returns:** `i64`

*Defined at line 2599*

---

## pert_unix_listen

roadmap L4B (stdlib/unix_socket.fd): a Unix-domain stream socket
listening at `path`, or -1 (Windows 10 1803+ has AF_UNIX in ws2_32).
pert_socket_accept / read / write / close work on it.

```fides
pub fn pert_unix_listen(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2641*

---

## pert_unix_connect

A Unix-domain stream socket connected to the listener at `path`, or -1.

```fides
pub fn pert_unix_connect(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 2646*

---

## pert_socket_accept

```fides
pub fn pert_socket_accept(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 2683*

---

## pert_socket_read

```fides
pub fn pert_socket_read(fd: i64, buf: i64, n: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 2691*

---

## pert_socket_write

```fides
pub fn pert_socket_write(fd: i64, data: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `data`: `str`

**Returns:** `i64`

*Defined at line 2699*

---

## pert_socket_write_str

```fides
pub fn pert_socket_write_str(fd: i64, data: str) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `data`: `str`

**Returns:** `i64`

*Defined at line 2707*

---

## pert_socket_close

```fides
pub fn pert_socket_close(fd: i64) {
```

**Parameters:**

- `fd`: `i64`

*Defined at line 2711*

---

## pert_net_close

```fides
pub fn pert_net_close(fd: i64) {
```

**Parameters:**

- `fd`: `i64`

*Defined at line 2715*

---

## pert_socket_recv_str

```fides
pub fn pert_socket_recv_str(fd: i64) -> str {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `str`

*Defined at line 2719*

---

## pert_net_connect

Task-aware connect: getaddrinfo + connect can block a worker for seconds,
so on a green task the whole call runs on the blocking pool (M4).

```fides
pub fn pert_net_connect(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 2734*

---

## pert_co_accept

```fides
pub fn pert_co_accept(fd: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`

**Returns:** `i64`

*Defined at line 2806*

---

## pert_co_connect

```fides
pub fn pert_co_connect(host: str, port: i64) -> i64 {
```

**Parameters:**

- `host`: `str`
- `port`: `i64`

**Returns:** `i64`

*Defined at line 2815*

---

## pert_co_recv

```fides
pub fn pert_co_recv(fd: i64, buf: i64, n: i64, timeout_ms: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`
- `timeout_ms`: `i64`

**Returns:** `i64`

*Defined at line 2823*

---

## pert_co_send

```fides
pub fn pert_co_send(fd: i64, buf: i64, n: i64) -> i64 {
```

**Parameters:**

- `fd`: `i64`
- `buf`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 2839*

---

## pert_co_detach

```fides
pub fn pert_co_detach(t: i64) {
```

**Parameters:**

- `t`: `i64`

*Defined at line 2854*

---

## pert_co_done

```fides
pub fn pert_co_done(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 2886*

---

## pert_co_result

```fides
pub fn pert_co_result(t: i64) -> i64 {
```

**Parameters:**

- `t`: `i64`

**Returns:** `i64`

*Defined at line 2890*

---

## pert_future_ready

```fides
pub fn pert_future_ready(value: i64) -> i64 {
```

**Parameters:**

- `value`: `i64`

**Returns:** `i64`

*Defined at line 2937*

---

## pert_future_poll

```fides
pub fn pert_future_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2941*

---

## pert_task_poll

```fides
pub fn pert_task_poll(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2955*

---

## pert_task_is_ready

```fides
pub fn pert_task_is_ready(f: i64) -> bool {
```

**Parameters:**

- `f`: `i64`

**Returns:** `bool`

*Defined at line 2959*

---

## pert_task_poll_nonblocking

```fides
pub fn pert_task_poll_nonblocking(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 2973*

---

## pert_executor_new

```fides
pub fn pert_executor_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 2993*

---

## pert_executor_block_on

```fides
pub fn pert_executor_block_on(exec: i64, f: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`
- `f`: `i64`

**Returns:** `i64`

*Defined at line 3005*

---

## pert_executor_run_all

Join every task still running — OS threads and green tasks alike; the
number joined. `spawn scope { }` exits through here, so a scope waits for
both kinds of work it started (BG196: the green tasks started inside it,
not every live one).

```fides
pub fn pert_executor_run_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 3013*

---

## pert_executor_poll_all

Join the finished tasks; the number still running.

```fides
pub fn pert_executor_poll_all(exec: i64) -> i64 {
```

**Parameters:**

- `exec`: `i64`

**Returns:** `i64`

*Defined at line 3032*

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

*Defined at line 3055*

---

## pert_shared_alloc

```fides
pub fn pert_shared_alloc(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 3078*

---

## pert_atomic_load

```fides
pub fn pert_atomic_load(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 3085*

---

## pert_atomic_store

```fides
pub fn pert_atomic_store(addr: i64, value: i64) {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

*Defined at line 3090*

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

*Defined at line 3096*

---

## pert_atomic_sub

```fides
pub fn pert_atomic_sub(addr: i64, value: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 3103*

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

*Defined at line 3107*

---

## pert_mutex_init

A mutex (not recursive, as a default pthread mutex): [0] an SRW lock
held only to look and change, [1] the condition variable threads wait
on, [2] 1 while held, [3..5) the tasks parked on it. BG195: it was the
SRW lock itself, held by the OS thread: a task holding it across a
suspension blocked the next task of its worker inside
AcquireSRWLockExclusive, and with it the worker. A task waits parked, so
the holder can run on and release it, from any worker.

```fides
pub fn pert_mutex_init() -> i64 {
```

**Returns:** `i64`

*Defined at line 3119*

---

## pert_mutex_lock

```fides
pub fn pert_mutex_lock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 3126*

---

## pert_mutex_unlock

```fides
pub fn pert_mutex_unlock(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 3141*

---

## pert_tls_create

```fides
pub fn pert_tls_create() -> i64 {
```

**Returns:** `i64`

*Defined at line 3152*

---

## pert_tls_get

```fides
pub fn pert_tls_get(key: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`

**Returns:** `i64`

*Defined at line 3156*

---

## pert_tls_set

```fides
pub fn pert_tls_set(key: i64, value: i64) {
```

**Parameters:**

- `key`: `i64`
- `value`: `i64`

*Defined at line 3160*

---

## pert_chan_create

```fides
pub fn pert_chan_create(capacity: i64) -> i64 {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `i64`

*Defined at line 3169*

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

*Defined at line 3193*

---

## pert_chan_recv

Block until there is a value or the channel is closed and empty.

```fides
pub fn pert_chan_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 3245*

---

## pert_chan_try_recv

A value if one is waiting; else 0 if closed, the pending value if not.

```fides
pub fn pert_chan_try_recv(ch: i64) -> i64 {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `i64`

*Defined at line 3280*

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

*Defined at line 3301*

---

## pert_chan_close

```fides
pub fn pert_chan_close(ch: i64) {
```

**Parameters:**

- `ch`: `i64`

*Defined at line 3321*

---

## pert_chan_is_closed

```fides
pub fn pert_chan_is_closed(ch: i64) -> bool {
```

**Parameters:**

- `ch`: `i64`

**Returns:** `bool`

*Defined at line 3334*

---

## pert_fs_copy_file

p_fs_copy_file: `src` to `dst` (created or emptied first; a file copied
onto itself is left as it is): the bytes copied, or -1.

```fides
pub fn pert_fs_copy_file(src: str, dst: str) -> i64 {
```

**Parameters:**

- `src`: `str`
- `dst`: `str`

**Returns:** `i64`

*Defined at line 3479*

---

## pert_net_send_file

p_net_send_file: the file at `path` on the connected socket `sock`: the
bytes sent, or -1.

```fides
pub fn pert_net_send_file(sock: i64, path: str) -> i64 {
```

**Parameters:**

- `sock`: `i64`
- `path`: `str`

**Returns:** `i64`

*Defined at line 3523*

---

## pert_fs_map

p_fs_map (fs.fd FileView): `size` bytes of the file `path` mapped
read-only, or 0 (the file is not that size now, or cannot be mapped).
The view keeps the mapping and the file open by itself.

```fides
pub fn pert_fs_map(path: str, size: i64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 3590*

---

## pert_fs_unmap

p_fs_unmap: undo pert_fs_map: 0, or -1.

```fides
pub fn pert_fs_unmap(addr: i64, size: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 3615*

---

## pert_prof_sampler_main

The sampler thread: a round every millisecond while the profiler is on
(the timer resolution raised to 0.5 ms, as win.fd's), every 20 ms a
look whether it is on again.

```fides
pub fn pert_prof_sampler_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 4126*

---

## pert_prof_watch_main

The watcher thread: the report on request (the named event
Local\fid-prof-<pid>, Ctrl-Break) and every FID_PROF_EVERY seconds.
The periodic reports keep their own schedule on the monotonic clock
(GetTickCount64): one asked for in between does not move the next.

```fides
pub fn pert_prof_watch_main(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 4165*

---

## pert_prof_boot

Before main (the entry point): FID_PROF set, not empty and not "0",
starts the profiler on the main thread.

```fides
pub fn pert_prof_boot() {
```

*Defined at line 4353*

---

## pert_prof_exit

After main returns (the entry point): the report; main's result back.

```fides
pub fn pert_prof_exit(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 4375*

---

## pert_prof_start

p_prof_start(): count samples from now on (earlier ones are dropped):
0, or -1 when this program cannot be sampled.

```fides
pub fn pert_prof_start() -> i64 {
```

**Returns:** `i64`

*Defined at line 4382*

---

## pert_prof_stop

p_prof_stop(): stop counting; the samples taken.

```fides
pub fn pert_prof_stop() -> i64 {
```

**Returns:** `i64`

*Defined at line 4387*

---

## pert_prof_report

p_prof_report(): the profile so far on stderr; the samples taken.

```fides
pub fn pert_prof_report() -> i64 {
```

**Returns:** `i64`

*Defined at line 4397*

---

## pert_prof_samples

p_prof_samples(name): the samples with function `name` on the stack.

```fides
pub fn pert_prof_samples(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 4403*

---

## pert_limits_boot

Before main (the entry point): FID_MEM_LIMIT and FID_CPU_LIMIT, and
FID_MEM_RETURN (RT7, pert_alloc_trim).

```fides
pub fn pert_limits_boot() {
```

*Defined at line 4527*

---

## pert_lim_watch_main

The CPU watcher's thread: every 10 ms, the process's CPU time.

```fides
pub fn pert_lim_watch_main(arg: i64) -> i64 {
```

**Parameters:**

- `arg`: `i64`

**Returns:** `i64`

*Defined at line 4564*

---

## pert_lim_take

p_malloc (machine code) takes a block of n bytes: with FID_MEM_LIMIT
set, past the limit the program stops (exit code 125). Returns 0.

```fides
pub fn pert_lim_take(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 4583*

---

## pert_lim_give

p_free (machine code) gives a block of n bytes back.

```fides
pub fn pert_lim_give(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 4604*

---

## pert_alloc_chunk

p_malloc found no room in this thread's chunk (or it has none): a new
one for list array `arr`, its old one retired. 0 when the system has no
memory left. `st` is the allocator state in .rdata.

```fides
pub fn pert_alloc_chunk(st: i64, arr: i64) -> i64 {
```

**Parameters:**

- `st`: `i64`
- `arr`: `i64`

**Returns:** `i64`

*Defined at line 4836*

---

## pert_alloc_recommit

p_malloc reuses the free block at payload p (n bytes with its header)
whose inside pages a trim gave back: they are committed again (they read
zero), the bytes around them are zeroed. 0 when the system has no
memory left. The block is off the lists; [408] counted all n bytes off.

```fides
pub fn pert_alloc_recommit(arr: i64, p: i64, n: i64) -> i64 {
```

**Parameters:**

- `arr`: `i64`
- `p`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 4890*

---

## pert_alloc_trim

p_free put more than [416] bytes on the lists of `arr` (this thread's):
release its empty chunks, decommit its big free blocks, set the next
threshold. `st` is the allocator state.

```fides
pub fn pert_alloc_trim(st: i64, arr: i64) -> i64 {
```

**Parameters:**

- `st`: `i64`
- `arr`: `i64`

**Returns:** `i64`

*Defined at line 4936*

---

## pert_alloc_thread_end

The OS thread running this ends (pert_thread_main): its chunk is retired
and its free lists go to the pool the next trim adopts; its list array
becomes the pool, or goes back to the heap.

```fides
pub fn pert_alloc_thread_end() {
```

*Defined at line 5063*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
