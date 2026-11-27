[All modules](index.md)

# arinc -- Fides Documentation

## Contents

- [ARINC_PARTITION_NORMAL:](#ARINC_PARTITION_NORMAL:) (const)
- [ARINC_PARTITION_IDLE:](#ARINC_PARTITION_IDLE:) (const)
- [ARINC_PARTITION_COLD_START:](#ARINC_PARTITION_COLD_START:) (const)
- [ARINC_PARTITION_WARM_START:](#ARINC_PARTITION_WARM_START:) (const)
- [ARINC_PARTITION_STOP:](#ARINC_PARTITION_STOP:) (const)
- [arinc_partition_state_name](#arinc_partition_state_name) (function)
- [ARINC_PORT_SOURCE:](#ARINC_PORT_SOURCE:) (const)
- [ARINC_PORT_DESTINATION:](#ARINC_PORT_DESTINATION:) (const)
- [ARINC_PORT_FIFO:](#ARINC_PORT_FIFO:) (const)
- [ARINC_PORT_PRIORITY:](#ARINC_PORT_PRIORITY:) (const)
- [ARINC_NO_ERROR:](#ARINC_NO_ERROR:) (const)
- [ARINC_NOT_AVAILABLE:](#ARINC_NOT_AVAILABLE:) (const)
- [ARINC_INVALID_PARAM:](#ARINC_INVALID_PARAM:) (const)
- [ARINC_INVALID_CONFIG:](#ARINC_INVALID_CONFIG:) (const)
- [ARINC_INVALID_MODE:](#ARINC_INVALID_MODE:) (const)
- [ARINC_INVALID_POINTER:](#ARINC_INVALID_POINTER:) (const)
- [ARINC_INVALID_SIZE:](#ARINC_INVALID_SIZE:) (const)
- [ARINC_TOO_MANY_OBJECTS:](#ARINC_TOO_MANY_OBJECTS:) (const)
- [ARINC_NOT_AVAILABLE_PART:](#ARINC_NOT_AVAILABLE_PART:) (const)
- [ARINC_NO_ACTION:](#ARINC_NO_ACTION:) (const)
- [ARINC_TIME_OUT:](#ARINC_TIME_OUT:) (const)
- [ARINC_QUEUE_EMPTY:](#ARINC_QUEUE_EMPTY:) (const)
- [ARINC_QUEUE_FULL:](#ARINC_QUEUE_FULL:) (const)
- [ARINC_PORT_FULL:](#ARINC_PORT_FULL:) (const)
- [ARINC_PORT_EMPTY:](#ARINC_PORT_EMPTY:) (const)
- [arinc_error_name](#arinc_error_name) (function)
- [QueuingPort](#QueuingPort) (struct)
- [arinc_queuing_port_create](#arinc_queuing_port_create) (function)
- [arinc_queuing_port_send](#arinc_queuing_port_send) (function)
- [arinc_queuing_port_receive](#arinc_queuing_port_receive) (function)
- [arinc_queuing_port_count](#arinc_queuing_port_count) (function)
- [arinc_queuing_port_is_empty](#arinc_queuing_port_is_empty) (function)
- [arinc_queuing_port_is_full](#arinc_queuing_port_is_full) (function)
- [arinc_queuing_port_name](#arinc_queuing_port_name) (function)
- [SamplingPort](#SamplingPort) (struct)
- [arinc_sampling_port_create](#arinc_sampling_port_create) (function)
- [arinc_sampling_port_write](#arinc_sampling_port_write) (function)
- [arinc_sampling_port_read](#arinc_sampling_port_read) (function)
- [arinc_sampling_port_is_valid](#arinc_sampling_port_is_valid) (function)
- [arinc_sampling_port_invalidate](#arinc_sampling_port_invalidate) (function)
- [arinc_sampling_port_name](#arinc_sampling_port_name) (function)
- [PartitionInfo](#PartitionInfo) (struct)
- [arinc_partition_create](#arinc_partition_create) (function)
- [arinc_partition_state](#arinc_partition_state) (function)
- [arinc_partition_set_state](#arinc_partition_set_state) (function)
- [arinc_partition_period](#arinc_partition_period) (function)
- [arinc_partition_duration](#arinc_partition_duration) (function)
- [arinc_partition_time_capacity](#arinc_partition_time_capacity) (function)
- [arinc_partition_name](#arinc_partition_name) (function)
- [ARINC_HM_LEVEL_MODULE:](#ARINC_HM_LEVEL_MODULE:) (const)
- [ARINC_HM_LEVEL_PARTITION:](#ARINC_HM_LEVEL_PARTITION:) (const)
- [ARINC_HM_LEVEL_PROCESS:](#ARINC_HM_LEVEL_PROCESS:) (const)
- [ARINC_HM_ERROR_MEMORY:](#ARINC_HM_ERROR_MEMORY:) (const)
- [ARINC_HM_ERROR_PROGRAM:](#ARINC_HM_ERROR_PROGRAM:) (const)
- [ARINC_HM_ERROR_TIME:](#ARINC_HM_ERROR_TIME:) (const)
- [ARINC_HM_ERROR_IO:](#ARINC_HM_ERROR_IO:) (const)
- [ARINC_HM_ERROR_POWER:](#ARINC_HM_ERROR_POWER:) (const)
- [ARINC_HM_ERROR_FREE_RUN:](#ARINC_HM_ERROR_FREE_RUN:) (const)
- [ARINC_HM_ERROR_APPLICATION:](#ARINC_HM_ERROR_APPLICATION:) (const)
- [ARINC_HM_ACTION_IGNORE:](#ARINC_HM_ACTION_IGNORE:) (const)
- [ARINC_HM_ACTION_PROCESS_RESTART:](#ARINC_HM_ACTION_PROCESS_RESTART:) (const)
- [ARINC_HM_ACTION_PARTITION_RESTART:](#ARINC_HM_ACTION_PARTITION_RESTART:) (const)
- [ARINC_HM_ACTION_PARTITION_STOP:](#ARINC_HM_ACTION_PARTITION_STOP:) (const)
- [ARINC_HM_ACTION_MODULE_RESET:](#ARINC_HM_ACTION_MODULE_RESET:) (const)
- [ARINC_HM_ACTION_MODULE_STOP:](#ARINC_HM_ACTION_MODULE_STOP:) (const)
- [HealthEvent](#HealthEvent) (struct)
- [arinc_hm_event](#arinc_hm_event) (function)
- [arinc_hm_level_name](#arinc_hm_level_name) (function)
- [arinc_hm_error_name](#arinc_hm_error_name) (function)
- [arinc_hm_action_name](#arinc_hm_action_name) (function)
- [arinc_hm_event_print](#arinc_hm_event_print) (function)
- [ScheduleEntry](#ScheduleEntry) (struct)
- [CyclicSchedule](#CyclicSchedule) (struct)
- [arinc_schedule_entry](#arinc_schedule_entry) (function)
- [arinc_cyclic_schedule](#arinc_cyclic_schedule) (function)
- [arinc_schedule_add_entry](#arinc_schedule_add_entry) (function)
- [arinc_schedule_count](#arinc_schedule_count) (function)
- [arinc_schedule_cycle_length](#arinc_schedule_cycle_length) (function)
- [arinc_schedule_partition_at](#arinc_schedule_partition_at) (function)
- [arinc_schedule_print](#arinc_schedule_print) (function)

## ARINC_PARTITION_NORMAL:

Partition states per ARINC 653 specification.

```fides
pub const ARINC_PARTITION_NORMAL: i64 = 0;
```

*Defined at line 31*

---

## ARINC_PARTITION_IDLE:

```fides
pub const ARINC_PARTITION_IDLE: i64 = 1;
```

*Defined at line 32*

---

## ARINC_PARTITION_COLD_START:

```fides
pub const ARINC_PARTITION_COLD_START: i64 = 2;
```

*Defined at line 33*

---

## ARINC_PARTITION_WARM_START:

```fides
pub const ARINC_PARTITION_WARM_START: i64 = 3;
```

*Defined at line 34*

---

## ARINC_PARTITION_STOP:

```fides
pub const ARINC_PARTITION_STOP: i64 = 4;
```

*Defined at line 35*

---

## arinc_partition_state_name

Get the name of a partition state.

```fides
pub fn arinc_partition_state_name(state: i64) -> str {
```

**Parameters:**

- `state`: `i64`

**Returns:** `str`

*Defined at line 37*

---

## ARINC_PORT_SOURCE:

Port directions

```fides
pub const ARINC_PORT_SOURCE: i64 = 0;
```

*Defined at line 59*

---

## ARINC_PORT_DESTINATION:

```fides
pub const ARINC_PORT_DESTINATION: i64 = 1;
```

*Defined at line 60*

---

## ARINC_PORT_FIFO:

Port disciplines (queuing ports)

```fides
pub const ARINC_PORT_FIFO: i64 = 0;
```

*Defined at line 62*

---

## ARINC_PORT_PRIORITY:

```fides
pub const ARINC_PORT_PRIORITY: i64 = 1;
```

*Defined at line 63*

---

## ARINC_NO_ERROR:

Port status codes

```fides
pub const ARINC_NO_ERROR: i64 = 0;
```

*Defined at line 65*

---

## ARINC_NOT_AVAILABLE:

```fides
pub const ARINC_NOT_AVAILABLE: i64 = 1;
```

*Defined at line 66*

---

## ARINC_INVALID_PARAM:

```fides
pub const ARINC_INVALID_PARAM: i64 = 2;
```

*Defined at line 67*

---

## ARINC_INVALID_CONFIG:

```fides
pub const ARINC_INVALID_CONFIG: i64 = 3;
```

*Defined at line 68*

---

## ARINC_INVALID_MODE:

```fides
pub const ARINC_INVALID_MODE: i64 = 4;
```

*Defined at line 69*

---

## ARINC_INVALID_POINTER:

```fides
pub const ARINC_INVALID_POINTER: i64 = 5;
```

*Defined at line 70*

---

## ARINC_INVALID_SIZE:

```fides
pub const ARINC_INVALID_SIZE: i64 = 6;
```

*Defined at line 71*

---

## ARINC_TOO_MANY_OBJECTS:

```fides
pub const ARINC_TOO_MANY_OBJECTS: i64 = 7;
```

*Defined at line 72*

---

## ARINC_NOT_AVAILABLE_PART:

```fides
pub const ARINC_NOT_AVAILABLE_PART: i64 = 8;
```

*Defined at line 73*

---

## ARINC_NO_ACTION:

```fides
pub const ARINC_NO_ACTION: i64 = 9;
```

*Defined at line 74*

---

## ARINC_TIME_OUT:

```fides
pub const ARINC_TIME_OUT: i64 = 10;
```

*Defined at line 75*

---

## ARINC_QUEUE_EMPTY:

```fides
pub const ARINC_QUEUE_EMPTY: i64 = 11;
```

*Defined at line 76*

---

## ARINC_QUEUE_FULL:

```fides
pub const ARINC_QUEUE_FULL: i64 = 12;
```

*Defined at line 77*

---

## ARINC_PORT_FULL:

```fides
pub const ARINC_PORT_FULL: i64 = 13;
```

*Defined at line 78*

---

## ARINC_PORT_EMPTY:

```fides
pub const ARINC_PORT_EMPTY: i64 = 14;
```

*Defined at line 79*

---

## arinc_error_name

Get the name of an ARINC error code.

```fides
pub fn arinc_error_name(code: i64) -> str {
```

**Parameters:**

- `code`: `i64`

**Returns:** `str`

*Defined at line 81*

---

## QueuingPort

A queuing port for inter-partition communication.
Messages are stored as flat byte arrays with length prefixes.
Since Vec stores i64 values, we use a flat buffer approach:
msg_lengths: Vec of message lengths (one per message)
msg_data:    Vec of all message bytes (concatenated)
msg_offsets: Vec of byte offsets into msg_data (start of each message)

```fides
pub struct QueuingPort {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `direction` | `i64` |
| `discipline` | `i64` |
| `max_message_size` | `i64` |
| `max_nb_message` | `i64` |
| `msg_lengths` | `Vec<i64>` |
| `msg_data` | `Vec<i64>` |
| `msg_offsets` | `Vec<i64>` |
| `count` | `i64` |
| `data_tail` | `i64` |

*Defined at line 145*

---

## arinc_queuing_port_create

Create a queuing port.
name: Port name (must be unique within the partition)
direction: ARINC_PORT_SOURCE or ARINC_PORT_DESTINATION
discipline: ARINC_PORT_FIFO or ARINC_PORT_PRIORITY
max_message_size: Maximum message size in bytes
max_nb_message: Maximum number of messages in the queue


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = arinc_queuing_port_create("cmd", 2, 8, ARINC_PORT_SOURCE, ARINC_PORT_FIFO);
assert(arinc_queuing_port_is_empty(p));
var m: Vec<i64> = Vec::new();
m.push(7); m.push(9);
assert_eq(arinc_queuing_port_send(&mut p, m), ARINC_NO_ERROR);
assert_eq(arinc_queuing_port_send(&mut p, m), ARINC_NO_ERROR);
assert_eq(arinc_queuing_port_send(&mut p, m), ARINC_PORT_FULL);
assert(arinc_queuing_port_is_full(p));
assert_eq(arinc_queuing_port_receive(p).get(1), 9);
assert_eq(arinc_error_name(ARINC_PORT_FULL), "PORT_FULL");
```

```fides
pub fn arinc_queuing_port_create(name: str, max_nb_message: i64, max_message_size: i64, direction: i64, discipline: i64) -> QueuingPort {
```

**Parameters:**

- `name`: `str`
- `max_nb_message`: `i64`
- `max_message_size`: `i64`
- `direction`: `i64`
- `discipline`: `i64`

**Returns:** `QueuingPort`

*Defined at line 176*

---

## arinc_queuing_port_send

Send a message through a queuing port.
Returns ARINC_NO_ERROR on success, ARINC_PORT_FULL if the queue is full.

```fides
pub fn arinc_queuing_port_send(port: &mut QueuingPort, message: Vec<i64>) -> i64 {
```

**Parameters:**

- `port`: `&mut QueuingPort`
- `message`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 184*

---

## arinc_queuing_port_receive

Receive a message from a queuing port.
Returns the message as a Vec of bytes, or an empty Vec if empty.
Reads the first (oldest) message. Does not remove from queue
(Fides Vec API limitation — use count to track depth).

```fides
pub fn arinc_queuing_port_receive(port: QueuingPort) -> Vec<i64> {
```

**Parameters:**

- `port`: `QueuingPort`

**Returns:** `Vec<i64>`

*Defined at line 203*

---

## arinc_queuing_port_count

Get the number of messages currently in the queue.

```fides
pub fn arinc_queuing_port_count(port: QueuingPort) -> i64 {
```

**Parameters:**

- `port`: `QueuingPort`

**Returns:** `i64`

*Defined at line 217*

---

## arinc_queuing_port_is_empty

Check if the queuing port is empty.

```fides
pub fn arinc_queuing_port_is_empty(port: QueuingPort) -> bool {
```

**Parameters:**

- `port`: `QueuingPort`

**Returns:** `bool`

*Defined at line 222*

---

## arinc_queuing_port_is_full

Check if the queuing port is full.

```fides
pub fn arinc_queuing_port_is_full(port: QueuingPort) -> bool {
```

**Parameters:**

- `port`: `QueuingPort`

**Returns:** `bool`

*Defined at line 227*

---

## arinc_queuing_port_name

Get the port name.

```fides
pub fn arinc_queuing_port_name(port: QueuingPort) -> str {
```

**Parameters:**

- `port`: `QueuingPort`

**Returns:** `str`

*Defined at line 232*

---

## SamplingPort

A sampling port for latest-value inter-partition communication.

```fides
pub struct SamplingPort {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `direction` | `i64` |
| `max_message_size` | `i64` |
| `refresh_period` | `i64` |
| `last_message` | `Vec<i64>` |
| `valid` | `bool` |
| `last_update_time` | `i64` |

*Defined at line 243*

---

## arinc_sampling_port_create

Create a sampling port.
refresh_period: Time in milliseconds after which data is considered stale.

```fides
pub fn arinc_sampling_port_create(name: str, max_message_size: i64, direction: i64, refresh_period: i64) -> SamplingPort {
```

**Parameters:**

- `name`: `str`
- `max_message_size`: `i64`
- `direction`: `i64`
- `refresh_period`: `i64`

**Returns:** `SamplingPort`

*Defined at line 255*

---

## arinc_sampling_port_write

Write a message to a sampling port (overwrites previous).

```fides
pub fn arinc_sampling_port_write(port: &mut SamplingPort, message: Vec<i64>, current_time: i64) -> i64 {
```

**Parameters:**

- `port`: `&mut SamplingPort`
- `message`: `Vec<i64>`
- `current_time`: `i64`

**Returns:** `i64`

*Defined at line 262*

---

## arinc_sampling_port_read

Read the current message from a sampling port.
Returns the message Vec (may be empty if no data written).
Use arinc_sampling_port_is_valid to check freshness.

```fides
pub fn arinc_sampling_port_read(port: SamplingPort) -> Vec<i64> {
```

**Parameters:**

- `port`: `SamplingPort`

**Returns:** `Vec<i64>`

*Defined at line 272*

---

## arinc_sampling_port_is_valid

Check if the sampling port has valid (fresh) data.

```fides
pub fn arinc_sampling_port_is_valid(port: SamplingPort, current_time: i64) -> bool {
```

**Parameters:**

- `port`: `SamplingPort`
- `current_time`: `i64`

**Returns:** `bool`

*Defined at line 277*

---

## arinc_sampling_port_invalidate

Invalidate the sampling port data (mark as stale).

```fides
pub fn arinc_sampling_port_invalidate(port: &mut SamplingPort) -> void {
```

**Parameters:**

- `port`: `&mut SamplingPort`

**Returns:** `void`

*Defined at line 283*

---

## arinc_sampling_port_name

Get the port name.

```fides
pub fn arinc_sampling_port_name(port: SamplingPort) -> str {
```

**Parameters:**

- `port`: `SamplingPort`

**Returns:** `str`

*Defined at line 288*

---

## PartitionInfo

Partition information.

```fides
pub struct PartitionInfo {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `state` | `i64` |
| `period` | `i64` |
| `duration` | `i64` |
| `time_capacity` | `i64,  // WCET budget in milliseconds` |

*Defined at line 299*

---

## arinc_partition_create

Create partition info.

```fides
pub fn arinc_partition_create(name: str, period: i64, duration: i64, time_capacity: i64) -> PartitionInfo {
```

**Parameters:**

- `name`: `str`
- `period`: `i64`
- `duration`: `i64`
- `time_capacity`: `i64`

**Returns:** `PartitionInfo`

*Defined at line 310*

---

## arinc_partition_state

Get the current partition state.

```fides
pub fn arinc_partition_state(part: PartitionInfo) -> i64 {
```

**Parameters:**

- `part`: `PartitionInfo`

**Returns:** `i64`

*Defined at line 317*

---

## arinc_partition_set_state

Set the partition state (state transition).
In a real system, this is an OS call. Here it's a struct update.

```fides
pub fn arinc_partition_set_state(part: &mut PartitionInfo, state: i64) -> void {
```

**Parameters:**

- `part`: `&mut PartitionInfo`
- `state`: `i64`

**Returns:** `void`

*Defined at line 323*

---

## arinc_partition_period

Get the partition period (ms).

```fides
pub fn arinc_partition_period(part: PartitionInfo) -> i64 {
```

**Parameters:**

- `part`: `PartitionInfo`

**Returns:** `i64`

*Defined at line 328*

---

## arinc_partition_duration

Get the partition duration (ms per period).

```fides
pub fn arinc_partition_duration(part: PartitionInfo) -> i64 {
```

**Parameters:**

- `part`: `PartitionInfo`

**Returns:** `i64`

*Defined at line 333*

---

## arinc_partition_time_capacity

Get the partition time capacity (WCET budget in ms).

```fides
pub fn arinc_partition_time_capacity(part: PartitionInfo) -> i64 {
```

**Parameters:**

- `part`: `PartitionInfo`

**Returns:** `i64`

*Defined at line 338*

---

## arinc_partition_name

Get the partition name.

```fides
pub fn arinc_partition_name(part: PartitionInfo) -> str {
```

**Parameters:**

- `part`: `PartitionInfo`

**Returns:** `str`

*Defined at line 343*

---

## ARINC_HM_LEVEL_MODULE:

Health monitoring error levels

```fides
pub const ARINC_HM_LEVEL_MODULE: i64 = 0;
```

*Defined at line 363*

---

## ARINC_HM_LEVEL_PARTITION:

```fides
pub const ARINC_HM_LEVEL_PARTITION: i64 = 1;
```

*Defined at line 364*

---

## ARINC_HM_LEVEL_PROCESS:

```fides
pub const ARINC_HM_LEVEL_PROCESS: i64 = 2;
```

*Defined at line 365*

---

## ARINC_HM_ERROR_MEMORY:

Health monitoring error types

```fides
pub const ARINC_HM_ERROR_MEMORY: i64 = 0;
```

*Defined at line 367*

---

## ARINC_HM_ERROR_PROGRAM:

```fides
pub const ARINC_HM_ERROR_PROGRAM: i64 = 1;
```

*Defined at line 368*

---

## ARINC_HM_ERROR_TIME:

```fides
pub const ARINC_HM_ERROR_TIME: i64 = 2;
```

*Defined at line 369*

---

## ARINC_HM_ERROR_IO:

```fides
pub const ARINC_HM_ERROR_IO: i64 = 3;
```

*Defined at line 370*

---

## ARINC_HM_ERROR_POWER:

```fides
pub const ARINC_HM_ERROR_POWER: i64 = 4;
```

*Defined at line 371*

---

## ARINC_HM_ERROR_FREE_RUN:

```fides
pub const ARINC_HM_ERROR_FREE_RUN: i64 = 5;
```

*Defined at line 372*

---

## ARINC_HM_ERROR_APPLICATION:

```fides
pub const ARINC_HM_ERROR_APPLICATION: i64 = 6;
```

*Defined at line 373*

---

## ARINC_HM_ACTION_IGNORE:

Health monitoring recovery actions

```fides
pub const ARINC_HM_ACTION_IGNORE: i64 = 0;
```

*Defined at line 375*

---

## ARINC_HM_ACTION_PROCESS_RESTART:

```fides
pub const ARINC_HM_ACTION_PROCESS_RESTART: i64 = 1;
```

*Defined at line 376*

---

## ARINC_HM_ACTION_PARTITION_RESTART:

```fides
pub const ARINC_HM_ACTION_PARTITION_RESTART: i64 = 2;
```

*Defined at line 377*

---

## ARINC_HM_ACTION_PARTITION_STOP:

```fides
pub const ARINC_HM_ACTION_PARTITION_STOP: i64 = 3;
```

*Defined at line 378*

---

## ARINC_HM_ACTION_MODULE_RESET:

```fides
pub const ARINC_HM_ACTION_MODULE_RESET: i64 = 4;
```

*Defined at line 379*

---

## ARINC_HM_ACTION_MODULE_STOP:

```fides
pub const ARINC_HM_ACTION_MODULE_STOP: i64 = 5;
```

*Defined at line 380*

---

## HealthEvent

A health monitoring event.

```fides
pub struct HealthEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `level` | `i64` |
| `error_type` | `i64` |
| `action` | `i64` |
| `partition_name` | `str` |
| `timestamp` | `i64` |

*Defined at line 382*

---

## arinc_hm_event

Create a health monitoring event.

```fides
pub fn arinc_hm_event(level: i64, error_type: i64, action: i64, partition_name: str, timestamp: i64) -> HealthEvent {
```

**Parameters:**

- `level`: `i64`
- `error_type`: `i64`
- `action`: `i64`
- `partition_name`: `str`
- `timestamp`: `i64`

**Returns:** `HealthEvent`

*Defined at line 391*

---

## arinc_hm_level_name

Get the error level name.

```fides
pub fn arinc_hm_level_name(level: i64) -> str {
```

**Parameters:**

- `level`: `i64`

**Returns:** `str`

*Defined at line 398*

---

## arinc_hm_error_name

Get the error type name.

```fides
pub fn arinc_hm_error_name(error_type: i64) -> str {
```

**Parameters:**

- `error_type`: `i64`

**Returns:** `str`

*Defined at line 412*

---

## arinc_hm_action_name

Get the recovery action name.

```fides
pub fn arinc_hm_action_name(action: i64) -> str {
```

**Parameters:**

- `action`: `i64`

**Returns:** `str`

*Defined at line 438*

---

## arinc_hm_event_print

Print a health monitoring event.

```fides
pub fn arinc_hm_event_print(event: HealthEvent) -> void {
```

**Parameters:**

- `event`: `HealthEvent`

**Returns:** `void`

*Defined at line 461*

---

## ScheduleEntry

A schedule entry: one partition's time slot in the cyclic schedule.

```fides
pub struct ScheduleEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `partition_name` | `str` |
| `start_time` | `i64` |
| `duration` | `i64,  // time slice duration (ms)` |

*Defined at line 482*

---

## CyclicSchedule

A cyclic schedule: a list of partition time slots.
Uses parallel arrays since Vec stores i64 values.

```fides
pub struct CyclicSchedule {
```

**Fields:**

| Name | Type |
|------|------|
| `partition_names` | `Vec<str>` |
| `start_times` | `Vec<i64>` |
| `durations` | `Vec<i64>` |
| `cycle_length` | `i64,  // total cycle duration (ms)` |

*Defined at line 491*

---

## arinc_schedule_entry

Create a schedule entry (returned for inspection, not stored in Vec).

```fides
pub fn arinc_schedule_entry(partition_name: str, start_time: i64, duration: i64) -> ScheduleEntry {
```

**Parameters:**

- `partition_name`: `str`
- `start_time`: `i64`
- `duration`: `i64`

**Returns:** `ScheduleEntry`

*Defined at line 502*

---

## arinc_cyclic_schedule

Create a cyclic schedule.

```fides
pub fn arinc_cyclic_schedule(cycle_length: i64) -> CyclicSchedule {
```

**Parameters:**

- `cycle_length`: `i64`

**Returns:** `CyclicSchedule`

*Defined at line 509*

---

## arinc_schedule_add_entry

Add a partition time slot to the schedule.

```fides
pub fn arinc_schedule_add_entry(schedule: &mut CyclicSchedule, entry: ScheduleEntry) -> void {
```

**Parameters:**

- `schedule`: `&mut CyclicSchedule`
- `entry`: `ScheduleEntry`

**Returns:** `void`

*Defined at line 516*

---

## arinc_schedule_count

Get the number of entries in the schedule.

```fides
pub fn arinc_schedule_count(schedule: CyclicSchedule) -> i64 {
```

**Parameters:**

- `schedule`: `CyclicSchedule`

**Returns:** `i64`

*Defined at line 523*

---

## arinc_schedule_cycle_length

Get the cycle length (ms).

```fides
pub fn arinc_schedule_cycle_length(schedule: CyclicSchedule) -> i64 {
```

**Parameters:**

- `schedule`: `CyclicSchedule`

**Returns:** `i64`

*Defined at line 528*

---

## arinc_schedule_partition_at

Find which partition should be running at a given time offset.
Returns the partition name, or "" if no partition is scheduled.

```fides
pub fn arinc_schedule_partition_at(schedule: CyclicSchedule, time_offset: i64) -> str {
```

**Parameters:**

- `schedule`: `CyclicSchedule`
- `time_offset`: `i64`

**Returns:** `str`

*Defined at line 534*

---

## arinc_schedule_print

Print the cyclic schedule.

```fides
pub fn arinc_schedule_print(schedule: CyclicSchedule) -> void {
```

**Parameters:**

- `schedule`: `CyclicSchedule`

**Returns:** `void`

*Defined at line 548*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
