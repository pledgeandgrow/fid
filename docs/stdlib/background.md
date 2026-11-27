[All modules](index.md)

# background -- Fides Documentation

## Contents

- [BG_NET_ANY:](#BG_NET_ANY:) (const)
- [BG_NET_CONNECTED:](#BG_NET_CONNECTED:) (const)
- [BG_NET_UNMETERED:](#BG_NET_UNMETERED:) (const)
- [BG_BACKOFF_LINEAR:](#BG_BACKOFF_LINEAR:) (const)
- [BG_BACKOFF_EXPONENTIAL:](#BG_BACKOFF_EXPONENTIAL:) (const)
- [BG_SUCCESS:](#BG_SUCCESS:) (const)
- [BG_RETRY:](#BG_RETRY:) (const)
- [BG_FAILURE:](#BG_FAILURE:) (const)
- [BG_MIN_PERIOD_MS:](#BG_MIN_PERIOD_MS:) (const)
- [BG_MIN_FLEX_MS:](#BG_MIN_FLEX_MS:) (const)
- [BG_MIN_BACKOFF_MS:](#BG_MIN_BACKOFF_MS:) (const)
- [BG_MAX_BACKOFF_MS:](#BG_MAX_BACKOFF_MS:) (const)
- [BgSpec](#BgSpec) (struct)
- [bg_one_off](#bg_one_off) (function)
- [bg_periodic](#bg_periodic) (function)
- [BgNormalized](#BgNormalized) (struct)
- [bg_normalize](#bg_normalize) (function)
- [bg_backoff_delay](#bg_backoff_delay) (function)
- [bg_workmanager_calls](#bg_workmanager_calls) (function)
- [bg_identifier_ok](#bg_identifier_ok) (function)
- [BgIosPlan](#BgIosPlan) (struct)
- [bg_ios_plan](#bg_ios_plan) (function)
- [bg_ios_plist](#bg_ios_plist) (function)
- [BgDevice](#BgDevice) (struct)
- [bg_device](#bg_device) (function)
- [bg_constraints_met](#bg_constraints_met) (function)
- [BG_ENQUEUED:](#BG_ENQUEUED:) (const)
- [BG_SUCCEEDED:](#BG_SUCCEEDED:) (const)
- [BG_FAILED:](#BG_FAILED:) (const)
- [BG_CANCELLED:](#BG_CANCELLED:) (const)
- [BgTask](#BgTask) (struct)
- [BgScheduler](#BgScheduler) (struct)
- [bg_scheduler_new](#bg_scheduler_new) (function)
- [bg_enqueue](#bg_enqueue) (function)
- [bg_cancel](#bg_cancel) (function)
- [bg_set_device](#bg_set_device) (function)
- [bg_advance](#bg_advance) (function)
- [bg_state](#bg_state) (function)
- [bg_state_name](#bg_state_name) (function)

## BG_NET_ANY:

```fides
pub const BG_NET_ANY: i64 = 0;
```

*Defined at line 32*

---

## BG_NET_CONNECTED:

```fides
pub const BG_NET_CONNECTED: i64 = 1;
```

*Defined at line 33*

---

## BG_NET_UNMETERED:

```fides
pub const BG_NET_UNMETERED: i64 = 2;
```

*Defined at line 34*

---

## BG_BACKOFF_LINEAR:

```fides
pub const BG_BACKOFF_LINEAR: i64 = 0;
```

*Defined at line 36*

---

## BG_BACKOFF_EXPONENTIAL:

```fides
pub const BG_BACKOFF_EXPONENTIAL: i64 = 1;
```

*Defined at line 37*

---

## BG_SUCCESS:

```fides
pub const BG_SUCCESS: i64 = 0;
```

*Defined at line 39*

---

## BG_RETRY:

```fides
pub const BG_RETRY: i64 = 1;
```

*Defined at line 40*

---

## BG_FAILURE:

```fides
pub const BG_FAILURE: i64 = 2;
```

*Defined at line 41*

---

## BG_MIN_PERIOD_MS:

```fides
pub const BG_MIN_PERIOD_MS: i64 = 900000;  // 15 minutes
```

*Defined at line 43*

---

## BG_MIN_FLEX_MS:

```fides
pub const BG_MIN_FLEX_MS: i64 = 300000;  // 5 minutes
```

*Defined at line 44*

---

## BG_MIN_BACKOFF_MS:

```fides
pub const BG_MIN_BACKOFF_MS: i64 = 10000;  // 10 seconds
```

*Defined at line 45*

---

## BG_MAX_BACKOFF_MS:

```fides
pub const BG_MAX_BACKOFF_MS: i64 = 18000000;  // 5 hours
```

*Defined at line 46*

---

## BgSpec

```fides
pub struct BgSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `str` |
| `periodic` | `bool` |
| `interval_ms` | `i64` |
| `flex_ms` | `i64` |
| `initial_delay_ms` | `i64` |
| `network` | `i64` |
| `charging` | `bool` |
| `idle` | `bool` |
| `battery_not_low` | `bool` |
| `storage_not_low` | `bool` |
| `backoff` | `i64` |
| `backoff_ms` | `i64` |
| `max_attempts` | `i64,  // 0: unlimited` |

*Defined at line 48*

---

## bg_one_off

```fides
pub fn bg_one_off(id: str) -> BgSpec {
```

**Parameters:**

- `id`: `str`

**Returns:** `BgSpec`

*Defined at line 64*

---

## bg_periodic

```fides
pub fn bg_periodic(id: str, interval_ms: i64, flex_ms: i64) -> BgSpec {
```

**Parameters:**

- `id`: `str`
- `interval_ms`: `i64`
- `flex_ms`: `i64`

**Returns:** `BgSpec`

*Defined at line 70*

---

## BgNormalized

The spec as WorkManager would run it (limits applied) and what was
changed.

```fides
pub struct BgNormalized {
```

**Fields:**

| Name | Type |
|------|------|
| `spec` | `BgSpec` |
| `notes` | `Vec<str>` |

*Defined at line 80*

---

## bg_normalize

```fides
pub fn bg_normalize(s: BgSpec) -> BgNormalized {
```

**Parameters:**

- `s`: `BgSpec`

**Returns:** `BgNormalized`

*Defined at line 85*

---

## bg_backoff_delay

The delay before attempt `attempt` + 1 after `attempt` retries
(WorkManager: linear delay * n, exponential delay * 2^(n-1), at most
5 hours).

```fides
pub fn bg_backoff_delay(s: &BgSpec, attempt: i64) -> i64 {
```

**Parameters:**

- `s`: `&BgSpec`
- `attempt`: `i64`

**Returns:** `i64`

*Defined at line 117*

---

## bg_workmanager_calls

The WorkManager builder calls of a spec, in order (for jni.fd).

```fides
pub fn bg_workmanager_calls(s: BgSpec) -> Vec<str> {
```

**Parameters:**

- `s`: `BgSpec`

**Returns:** `Vec<str>`

*Defined at line 137*

---

## bg_identifier_ok

Is `id` a reverse-DNS identifier (two or more dot-separated parts of
letters, digits, '-' and '_')?

```fides
pub fn bg_identifier_ok(id: str) -> bool {
```

**Parameters:**

- `id`: `str`

**Returns:** `bool`

*Defined at line 183*

---

## BgIosPlan

The BGTaskScheduler request of a spec and the warnings for what iOS
cannot express.

```fides
pub struct BgIosPlan {
```

**Fields:**

| Name | Type |
|------|------|
| `request_class` | `str,  // BGAppRefreshTaskRequest or BGProcessingTaskRequest` |
| `requires_network` | `bool` |
| `requires_power` | `bool` |
| `earliest_begin_ms` | `i64,  // relative to submission` |
| `warnings` | `Vec<str>` |

*Defined at line 209*

---

## bg_ios_plan

```fides
pub fn bg_ios_plan(s: &BgSpec) -> BgIosPlan {
```

**Parameters:**

- `s`: `&BgSpec`

**Returns:** `BgIosPlan`

*Defined at line 217*

---

## bg_ios_plist

The Info.plist entry listing the task identifiers.

```fides
pub fn bg_ios_plist(ids: Vec<str>) -> str {
```

**Parameters:**

- `ids`: `Vec<str>`

**Returns:** `str`

*Defined at line 249*

---

## BgDevice

```fides
pub struct BgDevice {
```

**Fields:**

| Name | Type |
|------|------|
| `network` | `i64,  // 0 none, BG_NET_CONNECTED (metered), BG_NET_UNMETERED` |
| `charging` | `bool` |
| `idle` | `bool` |
| `battery_low` | `bool` |
| `storage_low` | `bool` |

*Defined at line 261*

---

## bg_device

```fides
pub fn bg_device(network: i64, charging: bool) -> BgDevice {
```

**Parameters:**

- `network`: `i64`
- `charging`: `bool`

**Returns:** `BgDevice`

*Defined at line 269*

---

## bg_constraints_met

Do the device's conditions satisfy the spec's constraints?

```fides
pub fn bg_constraints_met(s: &BgSpec, d: &BgDevice) -> bool {
```

**Parameters:**

- `s`: `&BgSpec`
- `d`: `&BgDevice`

**Returns:** `bool`

*Defined at line 276*

---

## BG_ENQUEUED:

```fides
pub const BG_ENQUEUED: i64 = 0;
```

*Defined at line 298*

---

## BG_SUCCEEDED:

```fides
pub const BG_SUCCEEDED: i64 = 1;
```

*Defined at line 299*

---

## BG_FAILED:

```fides
pub const BG_FAILED: i64 = 2;
```

*Defined at line 300*

---

## BG_CANCELLED:

```fides
pub const BG_CANCELLED: i64 = 3;
```

*Defined at line 301*

---

## BgTask

```fides
pub struct BgTask {
```

**Fields:**

| Name | Type |
|------|------|
| `spec` | `BgSpec` |
| `worker` | `fn(i64) -> i64` |
| `state` | `i64` |
| `next_ms` | `i64,  // earliest start` |
| `attempts` | `i64,  // attempts of the current run (retries)` |
| `runs` | `i64,  // worker calls in total` |
| `period_index` | `i64,  // periodic: the period being served (1-based)` |
| `enqueued_ms` | `i64` |

*Defined at line 303*

---

## BgScheduler

```fides
pub struct BgScheduler {
```

**Fields:**

| Name | Type |
|------|------|
| `now` | `i64` |
| `device` | `BgDevice` |
| `tasks` | `Vec<BgTask>` |
| `log` | `Vec<str>` |

*Defined at line 314*

---

## bg_scheduler_new

```fides
pub fn bg_scheduler_new(device: BgDevice) -> BgScheduler {
```

**Parameters:**

- `device`: `BgDevice`

**Returns:** `BgScheduler`

*Defined at line 321*

---

## bg_enqueue

Enqueue a task (KEEP: an enqueued task with the same id stays).

```fides
pub fn bg_enqueue(s: &mut BgScheduler, spec: BgSpec, worker: fn(i64) -> i64) -> bool {
```

**Parameters:**

- `s`: `&mut BgScheduler`
- `spec`: `BgSpec`
- `worker`: `fn(i64) -> i64`

**Returns:** `bool`

*Defined at line 328*

---

## bg_cancel

```fides
pub fn bg_cancel(s: &mut BgScheduler, id: str) -> bool {
```

**Parameters:**

- `s`: `&mut BgScheduler`
- `id`: `str`

**Returns:** `bool`

*Defined at line 347*

---

## bg_set_device

```fides
pub fn bg_set_device(s: &mut BgScheduler, d: BgDevice) {
```

**Parameters:**

- `s`: `&mut BgScheduler`
- `d`: `BgDevice`

*Defined at line 362*

---

## bg_advance

Advance simulated time to `to_ms` in steps of `step_ms`, running due
tasks at each step (the step bounds the timing precision).

```fides
pub fn bg_advance(s: &mut BgScheduler, to_ms: i64, step_ms: i64) {
```

**Parameters:**

- `s`: `&mut BgScheduler`
- `to_ms`: `i64`
- `step_ms`: `i64`

*Defined at line 411*

---

## bg_state

The state of a task by id (-1: unknown).

```fides
pub fn bg_state(s: &BgScheduler, id: str) -> i64 {
```

**Parameters:**

- `s`: `&BgScheduler`
- `id`: `str`

**Returns:** `i64`

*Defined at line 425*

---

## bg_state_name

```fides
pub fn bg_state_name(st: i64) -> str {
```

**Parameters:**

- `st`: `i64`

**Returns:** `str`

*Defined at line 436*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
