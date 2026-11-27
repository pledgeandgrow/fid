[All modules](index.md)

# sched -- Fides Documentation

## Contents

- [SCHED_RR:](#SCHED_RR:) (const)
- [SCHED_PRIO:](#SCHED_PRIO:) (const)
- [SCHED_FAIR:](#SCHED_FAIR:) (const)
- [TASK_READY:](#TASK_READY:) (const)
- [TASK_RUNNING:](#TASK_RUNNING:) (const)
- [TASK_BLOCKED:](#TASK_BLOCKED:) (const)
- [TASK_SLEEPING:](#TASK_SLEEPING:) (const)
- [TASK_EXITED:](#TASK_EXITED:) (const)
- [SCHED_WEIGHTS:](#SCHED_WEIGHTS:) (const)
- [Task](#Task) (struct)
- [Sched](#Sched) (struct)
- [sched_new](#sched_new) (function)
- [sched_set_fair](#sched_set_fair) (function)
- [sched_set_aging](#sched_set_aging) (function)
- [sched_nice_weight](#sched_nice_weight) (function)
- [sched_task](#sched_task) (function)
- [sched_current](#sched_current) (function)
- [sched_state_name](#sched_state_name) (function)
- [sched_fair_slice](#sched_fair_slice) (function)
- [sched_schedule](#sched_schedule) (function)
- [sched_spawn](#sched_spawn) (function)
- [sched_wake](#sched_wake) (function)
- [sched_block](#sched_block) (function)
- [sched_sleep](#sched_sleep) (function)
- [sched_exit](#sched_exit) (function)
- [sched_yield](#sched_yield) (function)
- [sched_tick](#sched_tick) (function)
- [sched_current_name](#sched_current_name) (function)

## SCHED_RR:

```fides
pub const SCHED_RR: i64 = 0;
```

*Defined at line 23*

---

## SCHED_PRIO:

```fides
pub const SCHED_PRIO: i64 = 1;
```

*Defined at line 24*

---

## SCHED_FAIR:

```fides
pub const SCHED_FAIR: i64 = 2;
```

*Defined at line 25*

---

## TASK_READY:

```fides
pub const TASK_READY: i64 = 0;
```

*Defined at line 27*

---

## TASK_RUNNING:

```fides
pub const TASK_RUNNING: i64 = 1;
```

*Defined at line 28*

---

## TASK_BLOCKED:

```fides
pub const TASK_BLOCKED: i64 = 2;
```

*Defined at line 29*

---

## TASK_SLEEPING:

```fides
pub const TASK_SLEEPING: i64 = 3;
```

*Defined at line 30*

---

## TASK_EXITED:

```fides
pub const TASK_EXITED: i64 = 4;
```

*Defined at line 31*

---

## SCHED_WEIGHTS:

Linux sched_prio_to_weight: nice -20 .. 19.

```fides
pub const SCHED_WEIGHTS: [i64; 40] = [88761, 71755, 56483, 46273, 36291, 29154, 23254, 18705, 14949, 11916, 9548, 7620, 6100, 4904, 3906, 3121, 2501, 1991, 1586, 1277, 1024, 820, 655, 526, 423, 335, 272, 215, 172, 137, 110, 87, 70, 56, 45, 36, 29, 23, 18, 15];
```

*Defined at line 34*

---

## Task

```fides
pub struct Task {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `state` | `i64` |
| `prio` | `i64` |
| `eff_prio` | `i64` |
| `weight` | `i64` |
| `vruntime` | `i64` |
| `slice_left` | `i64` |
| `waited` | `i64` |
| `runtime` | `i64` |
| `wake_at` | `i64` |
| `switches_in` | `i64` |

*Defined at line 36*

---

## Sched

```fides
pub struct Sched {
```

**Fields:**

| Name | Type |
|------|------|
| `policy` | `i64` |
| `tasks` | `Vec<Task>` |
| `rq` | `Vec<i64>` |
| `current` | `i64` |
| `now` | `i64` |
| `quantum` | `i64` |
| `aging_interval` | `i64` |
| `latency` | `i64` |
| `min_granularity` | `i64` |
| `wakeup_granularity` | `i64` |
| `min_vruntime` | `i64` |
| `switches` | `i64` |
| `idle_time` | `i64` |

*Defined at line 54*

---

## sched_new

```fides
pub fn sched_new(policy: i64, quantum: i64) -> Sched {
```

**Parameters:**

- `policy`: `i64`
- `quantum`: `i64`

**Returns:** `Sched`

*Defined at line 74*

---

## sched_set_fair

CFS parameters: period, minimum slice and wakeup granularity
(Linux defaults 6 ms, 0.75 ms, 1 ms).

```fides
pub fn sched_set_fair(s: &mut Sched, latency: i64, min_gran: i64, wakeup_gran: i64) {
```

**Parameters:**

- `s`: `&mut Sched`
- `latency`: `i64`
- `min_gran`: `i64`
- `wakeup_gran`: `i64`

*Defined at line 82*

---

## sched_set_aging

```fides
pub fn sched_set_aging(s: &mut Sched, interval: i64) {
```

**Parameters:**

- `s`: `&mut Sched`
- `interval`: `i64`

*Defined at line 88*

---

## sched_nice_weight

Weight for a nice value (clamped to -20..19).

```fides
pub fn sched_nice_weight(nice: i64) -> i64 {
```

**Parameters:**

- `nice`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## sched_task

```fides
pub fn sched_task(s: &Sched, id: i64) -> Task {
```

**Parameters:**

- `s`: `&Sched`
- `id`: `i64`

**Returns:** `Task`

*Defined at line 104*

---

## sched_current

```fides
pub fn sched_current(s: &Sched) -> i64 {
```

**Parameters:**

- `s`: `&Sched`

**Returns:** `i64`

*Defined at line 108*

---

## sched_state_name

```fides
pub fn sched_state_name(st: i64) -> str {
```

**Parameters:**

- `st`: `i64`

**Returns:** `str`

*Defined at line 112*

---

## sched_fair_slice

CFS slice of task id: its weight's share of the period, at least the
minimum granularity.

```fides
pub fn sched_fair_slice(s: &Sched, id: i64) -> i64 {
```

**Parameters:**

- `s`: `&Sched`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## sched_schedule

Choose the next task when the CPU is free. Returns the running id.

```fides
pub fn sched_schedule(s: &mut Sched) -> i64 {
```

**Parameters:**

- `s`: `&mut Sched`

**Returns:** `i64`

*Defined at line 254*

---

## sched_spawn

New ready task. prio is the base priority (SCHED_PRIO, 0 = highest)
or the nice value (SCHED_FAIR). Returns its id.

```fides
pub fn sched_spawn(s: &mut Sched, name: str, prio: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut Sched`
- `name`: `str`
- `prio`: `i64`

**Returns:** `i64`

*Defined at line 285*

---

## sched_wake

Make a blocked or sleeping task ready; it may preempt the current one.

```fides
pub fn sched_wake(s: &mut Sched, id: i64) -> bool {
```

**Parameters:**

- `s`: `&mut Sched`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 315*

---

## sched_block

Block a task (waiting for I/O, a lock, a message) until sched_wake.

```fides
pub fn sched_block(s: &mut Sched, id: i64) {
```

**Parameters:**

- `s`: `&mut Sched`
- `id`: `i64`

*Defined at line 356*

---

## sched_sleep

Put a task to sleep until absolute time `until`.

```fides
pub fn sched_sleep(s: &mut Sched, id: i64, until: i64) {
```

**Parameters:**

- `s`: `&mut Sched`
- `id`: `i64`
- `until`: `i64`

*Defined at line 361*

---

## sched_exit

```fides
pub fn sched_exit(s: &mut Sched, id: i64) {
```

**Parameters:**

- `s`: `&mut Sched`
- `id`: `i64`

*Defined at line 368*

---

## sched_yield

Give up the rest of the slice.

```fides
pub fn sched_yield(s: &mut Sched) {
```

**Parameters:**

- `s`: `&mut Sched`

*Defined at line 373*

---

## sched_tick

Account dt of CPU time to the current task, wake sleepers whose time
has come, age waiting tasks, and preempt when the policy says so.
Returns the task that runs next (-1: idle).

```fides
pub fn sched_tick(s: &mut Sched, dt: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut Sched`
- `dt`: `i64`

**Returns:** `i64`

*Defined at line 382*

---

## sched_current_name

Name of the running task, "-" when idle (for traces).

```fides
pub fn sched_current_name(s: &Sched) -> str {
```

**Parameters:**

- `s`: `&Sched`

**Returns:** `str`

*Defined at line 460*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
