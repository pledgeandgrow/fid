[All modules](index.md)

# rtos -- Fides Documentation

## Contents

- [RT_READY:](#RT_READY:) (const)
- [RT_RUNNING:](#RT_RUNNING:) (const)
- [RT_BLOCKED:](#RT_BLOCKED:) (const)
- [RT_DELAYED:](#RT_DELAYED:) (const)
- [RT_SUSPENDED:](#RT_SUSPENDED:) (const)
- [RT_OK:](#RT_OK:) (const)
- [RT_WOULD_BLOCK:](#RT_WOULD_BLOCK:) (const)
- [RT_TIMEOUT:](#RT_TIMEOUT:) (const)
- [RT_ERROR:](#RT_ERROR:) (const)
- [RT_PENDING:](#RT_PENDING:) (const)
- [RTW_NONE:](#RTW_NONE:) (const)
- [RTW_SEM:](#RTW_SEM:) (const)
- [RTW_MUTEX:](#RTW_MUTEX:) (const)
- [RTW_QRECV:](#RTW_QRECV:) (const)
- [RTW_QSEND:](#RTW_QSEND:) (const)
- [RtTask](#RtTask) (struct)
- [RtSem](#RtSem) (struct)
- [RtMutex](#RtMutex) (struct)
- [RtQueue](#RtQueue) (struct)
- [RtTimer](#RtTimer) (struct)
- [Rtos](#Rtos) (struct)
- [rtos_new](#rtos_new) (function)
- [rtos_name](#rtos_name) (function)
- [rtos_task_create](#rtos_task_create) (function)
- [rtos_current](#rtos_current) (function)
- [rtos_schedule](#rtos_schedule) (function)
- [rtos_delay](#rtos_delay) (function)
- [rtos_suspend](#rtos_suspend) (function)
- [rtos_resume](#rtos_resume) (function)
- [rtos_yield](#rtos_yield) (function)
- [rtos_sem_create](#rtos_sem_create) (function)
- [rtos_sem_take](#rtos_sem_take) (function)
- [rtos_sem_give](#rtos_sem_give) (function)
- [rtos_mutex_create](#rtos_mutex_create) (function)
- [rtos_mutex_lock](#rtos_mutex_lock) (function)
- [rtos_mutex_unlock](#rtos_mutex_unlock) (function)
- [rtos_queue_create](#rtos_queue_create) (function)
- [rtos_queue_send](#rtos_queue_send) (function)
- [rtos_queue_recv](#rtos_queue_recv) (function)
- [rtos_task_msg](#rtos_task_msg) (function)
- [rtos_task_result](#rtos_task_result) (function)
- [rtos_timer_create](#rtos_timer_create) (function)
- [rtos_timer_start](#rtos_timer_start) (function)
- [rtos_timer_stop](#rtos_timer_stop) (function)
- [rtos_tick](#rtos_tick) (function)
- [rtos_state_name](#rtos_state_name) (function)
- [rtos_dump](#rtos_dump) (function)

## RT_READY:

```fides
pub const RT_READY: i64 = 0;
```

*Defined at line 39*

---

## RT_RUNNING:

```fides
pub const RT_RUNNING: i64 = 1;
```

*Defined at line 40*

---

## RT_BLOCKED:

```fides
pub const RT_BLOCKED: i64 = 2;
```

*Defined at line 41*

---

## RT_DELAYED:

```fides
pub const RT_DELAYED: i64 = 3;
```

*Defined at line 42*

---

## RT_SUSPENDED:

```fides
pub const RT_SUSPENDED: i64 = 4;
```

*Defined at line 43*

---

## RT_OK:

```fides
pub const RT_OK: i64 = 0;
```

*Defined at line 46*

---

## RT_WOULD_BLOCK:

```fides
pub const RT_WOULD_BLOCK: i64 = 1;
```

*Defined at line 47*

---

## RT_TIMEOUT:

```fides
pub const RT_TIMEOUT: i64 = 2;
```

*Defined at line 48*

---

## RT_ERROR:

```fides
pub const RT_ERROR: i64 = 3;
```

*Defined at line 49*

---

## RT_PENDING:

Returned by a call whose caller is now blocked.

```fides
pub const RT_PENDING: i64 = 4;
```

*Defined at line 51*

---

## RTW_NONE:

```fides
pub const RTW_NONE: i64 = 0;
```

*Defined at line 54*

---

## RTW_SEM:

```fides
pub const RTW_SEM: i64 = 1;
```

*Defined at line 55*

---

## RTW_MUTEX:

```fides
pub const RTW_MUTEX: i64 = 2;
```

*Defined at line 56*

---

## RTW_QRECV:

```fides
pub const RTW_QRECV: i64 = 3;
```

*Defined at line 57*

---

## RTW_QSEND:

```fides
pub const RTW_QSEND: i64 = 4;
```

*Defined at line 58*

---

## RtTask

```fides
pub struct RtTask {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `base_prio` | `i64` |
| `prio` | `i64` |
| `state` | `i64` |
| `wake` | `i64` |
| `wait_kind` | `i64` |
| `wait_obj` | `i64` |
| `result` | `i64` |
| `msg` | `i64` |
| `seq` | `i64` |
| `slice` | `i64` |
| `runs` | `i64` |

*Defined at line 60*

---

## RtSem

```fides
pub struct RtSem {
```

**Fields:**

| Name | Type |
|------|------|
| `count` | `i64` |
| `max` | `i64` |
| `waiters` | `Vec<i64>` |

*Defined at line 75*

---

## RtMutex

```fides
pub struct RtMutex {
```

**Fields:**

| Name | Type |
|------|------|
| `owner` | `i64` |
| `depth` | `i64` |
| `waiters` | `Vec<i64>` |

*Defined at line 81*

---

## RtQueue

```fides
pub struct RtQueue {
```

**Fields:**

| Name | Type |
|------|------|
| `cap` | `i64` |
| `items` | `Vec<i64>` |
| `recv_waiters` | `Vec<i64>` |
| `send_waiters` | `Vec<i64>` |

*Defined at line 87*

---

## RtTimer

```fides
pub struct RtTimer {
```

**Fields:**

| Name | Type |
|------|------|
| `period` | `i64` |
| `expiry` | `i64` |
| `reload` | `bool` |
| `active` | `bool` |
| `fired` | `i64` |

*Defined at line 94*

---

## Rtos

```fides
pub struct Rtos {
```

**Fields:**

| Name | Type |
|------|------|
| `tasks` | `Vec<RtTask>` |
| `sems` | `Vec<RtSem>` |
| `mutexes` | `Vec<RtMutex>` |
| `queues` | `Vec<RtQueue>` |
| `timers` | `Vec<RtTimer>` |
| `max_tasks` | `i64` |
| `max_sems` | `i64` |
| `max_mutexes` | `i64` |
| `max_queues` | `i64` |
| `max_timers` | `i64` |
| `tick` | `i64` |
| `current` | `i64` |
| `next_seq` | `i64` |
| `slice_len` | `i64` |
| `switches` | `i64` |
| `log` | `Vec<str>` |
| `timer_events` | `Vec<str>` |

*Defined at line 102*

---

## rtos_new

An RTOS with room for the given numbers of objects (the idle task is
task 0 and counts towards max_tasks). `slice` ticks per round-robin turn.

```fides
pub fn rtos_new(max_tasks: i64, max_sems: i64, max_mutexes: i64, max_queues: i64, max_timers: i64, slice: i64) -> Rtos {
```

**Parameters:**

- `max_tasks`: `i64`
- `max_sems`: `i64`
- `max_mutexes`: `i64`
- `max_queues`: `i64`
- `max_timers`: `i64`
- `slice`: `i64`

**Returns:** `Rtos`

*Defined at line 124*

---

## rtos_name

```fides
pub fn rtos_name(r: &Rtos, id: i64) -> str {
```

**Parameters:**

- `r`: `&Rtos`
- `id`: `i64`

**Returns:** `str`

*Defined at line 144*

---

## rtos_task_create

Create a ready task; -1 when the task table is full. Does not switch:
call rtos_schedule (or let the next API call or tick do it).

```fides
pub fn rtos_task_create(r: &mut Rtos, name: str, prio: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `name`: `str`
- `prio`: `i64`

**Returns:** `i64`

*Defined at line 150*

---

## rtos_current

```fides
pub fn rtos_current(r: &Rtos) -> i64 {
```

**Parameters:**

- `r`: `&Rtos`

**Returns:** `i64`

*Defined at line 161*

---

## rtos_schedule

Pick the task to run and switch to it when it is not the current one.

```fides
pub fn rtos_schedule(r: &mut Rtos) {
```

**Parameters:**

- `r`: `&mut Rtos`

*Defined at line 168*

---

## rtos_delay

Put the current task to sleep for `ticks` ticks.

```fides
pub fn rtos_delay(r: &mut Rtos, ticks: i64) {
```

**Parameters:**

- `r`: `&mut Rtos`
- `ticks`: `i64`

*Defined at line 260*

---

## rtos_suspend

```fides
pub fn rtos_suspend(r: &mut Rtos, id: i64) {
```

**Parameters:**

- `r`: `&mut Rtos`
- `id`: `i64`

*Defined at line 271*

---

## rtos_resume

```fides
pub fn rtos_resume(r: &mut Rtos, id: i64) {
```

**Parameters:**

- `r`: `&mut Rtos`
- `id`: `i64`

*Defined at line 281*

---

## rtos_yield

Yield to the next task of the same priority.

```fides
pub fn rtos_yield(r: &mut Rtos) {
```

**Parameters:**

- `r`: `&mut Rtos`

*Defined at line 290*

---

## rtos_sem_create

```fides
pub fn rtos_sem_create(r: &mut Rtos, initial: i64, max: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `initial`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 306*

---

## rtos_sem_take

Take: RT_OK, RT_WOULD_BLOCK (timeout 0) or RT_PENDING (blocked).

```fides
pub fn rtos_sem_take(r: &mut Rtos, s: i64, timeout: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `s`: `i64`
- `timeout`: `i64`

**Returns:** `i64`

*Defined at line 317*

---

## rtos_sem_give

Give (also from an interrupt handler: it never blocks). RT_ERROR when
the count is at its maximum.

```fides
pub fn rtos_sem_give(r: &mut Rtos, s: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `s`: `i64`

**Returns:** `i64`

*Defined at line 336*

---

## rtos_mutex_create

```fides
pub fn rtos_mutex_create(r: &mut Rtos) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`

**Returns:** `i64`

*Defined at line 357*

---

## rtos_mutex_lock

```fides
pub fn rtos_mutex_lock(r: &mut Rtos, m: i64, timeout: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `m`: `i64`
- `timeout`: `i64`

**Returns:** `i64`

*Defined at line 406*

---

## rtos_mutex_unlock

Unlock (only by the owner): the mutex goes to the best waiter, the
old owner's priority is recomputed.

```fides
pub fn rtos_mutex_unlock(r: &mut Rtos, m: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `m`: `i64`

**Returns:** `i64`

*Defined at line 433*

---

## rtos_queue_create

```fides
pub fn rtos_queue_create(r: &mut Rtos, cap: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `cap`: `i64`

**Returns:** `i64`

*Defined at line 465*

---

## rtos_queue_send

Send `v`: to a waiting receiver directly, else into the queue, else
block (the message waits with the sender).

```fides
pub fn rtos_queue_send(r: &mut Rtos, q: i64, v: i64, timeout: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `q`: `i64`
- `v`: `i64`
- `timeout`: `i64`

**Returns:** `i64`

*Defined at line 477*

---

## rtos_queue_recv

Receive: RT_OK with the message in the current task's `msg`, or block.

```fides
pub fn rtos_queue_recv(r: &mut Rtos, q: i64, timeout: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `q`: `i64`
- `timeout`: `i64`

**Returns:** `i64`

*Defined at line 510*

---

## rtos_task_msg

The message the task received last (after RT_OK, or after waking).

```fides
pub fn rtos_task_msg(r: &Rtos, id: i64) -> i64 {
```

**Parameters:**

- `r`: `&Rtos`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 544*

---

## rtos_task_result

```fides
pub fn rtos_task_result(r: &Rtos, id: i64) -> i64 {
```

**Parameters:**

- `r`: `&Rtos`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 548*

---

## rtos_timer_create

```fides
pub fn rtos_timer_create(r: &mut Rtos, period: i64, reload: bool) -> i64 {
```

**Parameters:**

- `r`: `&mut Rtos`
- `period`: `i64`
- `reload`: `bool`

**Returns:** `i64`

*Defined at line 554*

---

## rtos_timer_start

```fides
pub fn rtos_timer_start(r: &mut Rtos, id: i64) {
```

**Parameters:**

- `r`: `&mut Rtos`
- `id`: `i64`

*Defined at line 564*

---

## rtos_timer_stop

```fides
pub fn rtos_timer_stop(r: &mut Rtos, id: i64) {
```

**Parameters:**

- `r`: `&mut Rtos`
- `id`: `i64`

*Defined at line 571*

---

## rtos_tick

The timer interrupt: advance time, wake sleepers and timeouts, run
software timers, rotate equal-priority tasks, reschedule.

```fides
pub fn rtos_tick(r: &mut Rtos) {
```

**Parameters:**

- `r`: `&mut Rtos`

*Defined at line 608*

---

## rtos_state_name

```fides
pub fn rtos_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 662*

---

## rtos_dump

"name:prio/state" for every task.

```fides
pub fn rtos_dump(r: &Rtos) -> str {
```

**Parameters:**

- `r`: `&Rtos`

**Returns:** `str`

*Defined at line 668*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
