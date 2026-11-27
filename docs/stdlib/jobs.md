[All modules](index.md)

# jobs -- Fides Documentation

## Contents

- [JOB_PENDING:](#JOB_PENDING:) (const)
- [JOB_RUNNING:](#JOB_RUNNING:) (const)
- [JOB_DONE:](#JOB_DONE:) (const)
- [JOB_DEAD:](#JOB_DEAD:) (const)
- [Job](#Job) (struct)
- [JobBackend](#JobBackend) (struct)
- [jobs_mem_backend](#jobs_mem_backend) (function)
- [jobs_encode](#jobs_encode) (function)
- [jobs_decode](#jobs_decode) (function)
- [JobQueue](#JobQueue) (struct)
- [jobs_queue_new](#jobs_queue_new) (function)
- [jobs_load](#jobs_load) (function)
- [jobs_enqueue](#jobs_enqueue) (function)
- [jobs_schedule](#jobs_schedule) (function)
- [jobs_reserve](#jobs_reserve) (function)
- [jobs_reserve_id](#jobs_reserve_id) (function)
- [jobs_complete](#jobs_complete) (function)
- [jobs_backoff_ms](#jobs_backoff_ms) (function)
- [jobs_fail](#jobs_fail) (function)
- [jobs_dead](#jobs_dead) (function)
- [jobs_retry_dead](#jobs_retry_dead) (function)
- [jobs_count](#jobs_count) (function)
- [jobs_get](#jobs_get) (function)
- [jobs_next_due](#jobs_next_due) (function)
- [jobs_tick](#jobs_tick) (function)
- [jobs_add_cron](#jobs_add_cron) (function)
- [jobs_cron_next](#jobs_cron_next) (function)
- [CronSpec](#CronSpec) (struct)
- [cron_parse](#cron_parse) (function)
- [cron_days_from_civil](#cron_days_from_civil) (function)
- [cron_civil_from_days](#cron_civil_from_days) (function)
- [cron_format](#cron_format) (function)
- [cron_next](#cron_next) (function)

## JOB_PENDING:

```fides
pub const JOB_PENDING: i64 = 0;
```

*Defined at line 49*

---

## JOB_RUNNING:

```fides
pub const JOB_RUNNING: i64 = 1;
```

*Defined at line 50*

---

## JOB_DONE:

```fides
pub const JOB_DONE: i64 = 2;
```

*Defined at line 51*

---

## JOB_DEAD:

```fides
pub const JOB_DEAD: i64 = 3;
```

*Defined at line 52*

---

## Job

```fides
pub struct Job {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `queue` | `str` |
| `payload` | `str` |
| `priority` | `i64` |
| `state` | `i64` |
| `attempts` | `i64` |
| `max_attempts` | `i64` |
| `run_at` | `i64` |
| `created` | `i64` |
| `lease_until` | `i64` |
| `last_error` | `str` |

*Defined at line 54*

---

## JobBackend

Where job records live. put(handle, id, record) stores or replaces,
get(handle, id) gives the record ("" when none), del(handle, id)
removes, ids(handle) lists the stored ids.

```fides
pub struct JobBackend {
```

**Fields:**

| Name | Type |
|------|------|
| `h` | `i64` |
| `put` | `fn(i64, i64, str) -> bool` |
| `get` | `fn(i64, i64) -> str` |
| `del` | `fn(i64, i64) -> bool` |
| `ids` | `fn(i64) -> Vec<i64>` |

*Defined at line 73*

---

## jobs_mem_backend

A new, empty in-memory backend.

```fides
pub fn jobs_mem_backend() -> JobBackend {
```

**Returns:** `JobBackend`

*Defined at line 142*

---

## jobs_encode

A job as one line of tab-separated fields (the backend's record).

```fides
pub fn jobs_encode(j: &Job) -> str {
```

**Parameters:**

- `j`: `&Job`

**Returns:** `str`

*Defined at line 197*

---

## jobs_decode

Read a record back; None when it is not one.

```fides
pub fn jobs_decode(rec: str) -> Option<Job> {
```

**Parameters:**

- `rec`: `str`

**Returns:** `Option<Job>`

*Defined at line 202*

---

## JobQueue

```fides
pub struct JobQueue {
```

**Fields:**

| Name | Type |
|------|------|
| `jobs` | `Vec<Job>` |
| `cron_names` | `Vec<str>` |
| `cron_queues` | `Vec<str>` |
| `cron_payloads` | `Vec<str>` |
| `cron_specs` | `Vec<CronSpec>` |
| `cron_next_s` | `Vec<i64>` |
| `backend` | `JobBackend` |
| `next_id` | `i64` |
| `rng` | `i64` |
| `backoff_base_ms` | `i64` |
| `backoff_max_ms` | `i64` |
| `keep_done` | `bool` |
| `error` | `str` |

*Defined at line 214*

---

## jobs_queue_new

A queue over `backend`; `seed` drives the backoff jitter.

```fides
pub fn jobs_queue_new(backend: JobBackend, seed: i64) -> JobQueue {
```

**Parameters:**

- `backend`: `JobBackend`
- `seed`: `i64`

**Returns:** `JobQueue`

*Defined at line 232*

---

## jobs_load

Rebuild a queue from the records a backend holds.

```fides
pub fn jobs_load(backend: JobBackend, seed: i64) -> JobQueue {
```

**Parameters:**

- `backend`: `JobBackend`
- `seed`: `i64`

**Returns:** `JobQueue`

*Defined at line 243*

---

## jobs_enqueue

Add a job: runs after `delay_ms` from `now_ms`, at most `max_attempts`
times. Returns its id.

```fides
pub fn jobs_enqueue(q: &mut JobQueue, queue: str, payload: str, priority: i64, delay_ms: i64, max_attempts: i64, now_ms: i64) -> i64 {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `queue`: `str`
- `payload`: `str`
- `priority`: `i64`
- `delay_ms`: `i64`
- `max_attempts`: `i64`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 287*

---

## jobs_schedule

Add a job that runs at `run_at_ms` (or later).

```fides
pub fn jobs_schedule(q: &mut JobQueue, queue: str, payload: str, priority: i64, run_at_ms: i64, max_attempts: i64, now_ms: i64) -> i64 {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `queue`: `str`
- `payload`: `str`
- `priority`: `i64`
- `run_at_ms`: `i64`
- `max_attempts`: `i64`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 292*

---

## jobs_reserve

The due job to run next in `queue` ("" = any queue), now marked running
until now + lease_ms.

```fides
pub fn jobs_reserve(q: &mut JobQueue, queue: str, now_ms: i64, lease_ms: i64) -> Option<Job> {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `queue`: `str`
- `now_ms`: `i64`
- `lease_ms`: `i64`

**Returns:** `Option<Job>`

*Defined at line 338*

---

## jobs_reserve_id

jobs_reserve giving the job's id (-1 when none is due); read the job
with jobs_get.

```fides
pub fn jobs_reserve_id(q: &mut JobQueue, queue: str, now_ms: i64, lease_ms: i64) -> i64 {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `queue`: `str`
- `now_ms`: `i64`
- `lease_ms`: `i64`

**Returns:** `i64`

*Defined at line 349*

---

## jobs_complete

The job finished: removed (or kept as done with keep_done).

```fides
pub fn jobs_complete(q: &mut JobQueue, id: i64, now_ms: i64) -> bool {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `id`: `i64`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 359*

---

## jobs_backoff_ms

The delay before retry number `attempt` (1 = after the first failure):
base * 2^(attempt-1) capped at max, then half of it fixed and half
random ("equal jitter").

```fides
pub fn jobs_backoff_ms(q: &mut JobQueue, attempt: i64) -> i64 {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `attempt`: `i64`

**Returns:** `i64`

*Defined at line 392*

---

## jobs_fail

The job failed: retried after the backoff, or dead when out of attempts.

```fides
pub fn jobs_fail(q: &mut JobQueue, id: i64, error: str, now_ms: i64) -> bool {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `id`: `i64`
- `error`: `str`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 421*

---

## jobs_dead

The dead-letter list.

```fides
pub fn jobs_dead(q: &JobQueue) -> Vec<Job> {
```

**Parameters:**

- `q`: `&JobQueue`

**Returns:** `Vec<Job>`

*Defined at line 431*

---

## jobs_retry_dead

Put a dead job back with its attempts reset.

```fides
pub fn jobs_retry_dead(q: &mut JobQueue, id: i64, now_ms: i64) -> bool {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `id`: `i64`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 444*

---

## jobs_count

Jobs in a state (JOB_PENDING ...); pending ones not yet due count too.

```fides
pub fn jobs_count(q: &JobQueue, state: i64) -> i64 {
```

**Parameters:**

- `q`: `&JobQueue`
- `state`: `i64`

**Returns:** `i64`

*Defined at line 459*

---

## jobs_get

```fides
pub fn jobs_get(q: &JobQueue, id: i64) -> Option<Job> {
```

**Parameters:**

- `q`: `&JobQueue`
- `id`: `i64`

**Returns:** `Option<Job>`

*Defined at line 471*

---

## jobs_next_due

When the next pending job is due (-1 when none): a worker sleeps until.

```fides
pub fn jobs_next_due(q: &JobQueue) -> i64 {
```

**Parameters:**

- `q`: `&JobQueue`

**Returns:** `i64`

*Defined at line 480*

---

## jobs_tick

Housekeeping at `now_ms`: expired leases count as failures, and each
cron occurrence that has come enqueues one job (priority 0, at most 3
attempts, run-at the occurrence). Returns how many jobs it enqueued or
requeued.

```fides
pub fn jobs_tick(q: &mut JobQueue, now_ms: i64) -> i64 {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `now_ms`: `i64`

**Returns:** `i64`

*Defined at line 497*

---

## jobs_add_cron

A recurring job: from now on, one job in `queue` per occurrence of the
cron expression. false (q.error set) when the expression is bad.

```fides
pub fn jobs_add_cron(q: &mut JobQueue, name: str, expr: str, queue: str, payload: str, now_ms: i64) -> bool {
```

**Parameters:**

- `q`: `&mut JobQueue`
- `name`: `str`
- `expr`: `str`
- `queue`: `str`
- `payload`: `str`
- `now_ms`: `i64`

**Returns:** `bool`

*Defined at line 528*

---

## jobs_cron_next

The next occurrence (Unix seconds) of the cron job `name`, -1 if none.

```fides
pub fn jobs_cron_next(q: &JobQueue, name: str) -> i64 {
```

**Parameters:**

- `q`: `&JobQueue`
- `name`: `str`

**Returns:** `i64`

*Defined at line 544*

---

## CronSpec

A parsed cron expression: allowed values of each field as bitmasks
(minute 0-59, hour 0-23, dom 1-31, month 1-12, dow 0-6 Sunday=0).

```fides
pub struct CronSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `minutes` | `i64` |
| `hours` | `i64` |
| `doms` | `i64` |
| `months` | `i64` |
| `dows` | `i64` |
| `dom_star` | `bool` |
| `dow_star` | `bool` |
| `ok` | `bool` |
| `error` | `str` |

*Defined at line 559*

---

## cron_parse

Parse a 5-field cron expression or an @macro.

```fides
pub fn cron_parse(expr: str) -> CronSpec {
```

**Parameters:**

- `expr`: `str`

**Returns:** `CronSpec`

*Defined at line 672*

---

## cron_days_from_civil

Days since 1970-01-01 of a civil date (proleptic Gregorian).

```fides
pub fn cron_days_from_civil(y: i64, m: i64, d: i64) -> i64 {
```

**Parameters:**

- `y`: `i64`
- `m`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 731*

---

## cron_civil_from_days

[year, month, day] of a day number (days since 1970-01-01).

```fides
pub fn cron_civil_from_days(z0: i64) -> Vec<i64> {
```

**Parameters:**

- `z0`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 751*

---

## cron_format

"YYYY-MM-DD HH:MM Ddd" of a Unix time (UTC), for logs and tests.

```fides
pub fn cron_format(t: i64) -> str {
```

**Parameters:**

- `t`: `i64`

**Returns:** `str`

*Defined at line 778*

---

## cron_next

The first time strictly after `after_s` (Unix seconds, UTC) that the
expression matches, or -1 when none comes within 8 years.

```fides
pub fn cron_next(s: &CronSpec, after_s: i64) -> i64 {
```

**Parameters:**

- `s`: `&CronSpec`
- `after_s`: `i64`

**Returns:** `i64`

*Defined at line 816*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
