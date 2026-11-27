[All modules](index.md)

# ocpp -- Fides Documentation

## Contents

- [OCPP_CALL:](#OCPP_CALL:) (const)
- [OCPP_CALLRESULT:](#OCPP_CALLRESULT:) (const)
- [OCPP_CALLERROR:](#OCPP_CALLERROR:) (const)
- [CP_AVAILABLE:](#CP_AVAILABLE:) (const)
- [CP_PREPARING:](#CP_PREPARING:) (const)
- [CP_CHARGING:](#CP_CHARGING:) (const)
- [CP_SUSPENDED_EV:](#CP_SUSPENDED_EV:) (const)
- [CP_FINISHING:](#CP_FINISHING:) (const)
- [CP_FAULTED:](#CP_FAULTED:) (const)
- [EV_PLUG_IN:](#EV_PLUG_IN:) (const)
- [EV_AUTHORIZE:](#EV_AUTHORIZE:) (const)
- [EV_START:](#EV_START:) (const)
- [EV_SUSPEND:](#EV_SUSPEND:) (const)
- [EV_RESUME:](#EV_RESUME:) (const)
- [EV_STOP:](#EV_STOP:) (const)
- [EV_UNPLUG:](#EV_UNPLUG:) (const)
- [EV_FAULT:](#EV_FAULT:) (const)
- [EV_FAULT_CLEAR:](#EV_FAULT_CLEAR:) (const)
- [ocpp_state_name](#ocpp_state_name) (function)
- [ocpp_build_call](#ocpp_build_call) (function)
- [ocpp_build_result](#ocpp_build_result) (function)
- [ocpp_build_error](#ocpp_build_error) (function)
- [OcppFrame](#OcppFrame) (struct)
- [ocpp_parse](#ocpp_parse) (function)
- [OcppClient](#OcppClient) (struct)
- [ocpp_client_new](#ocpp_client_new) (function)
- [ocpp_client_call](#ocpp_client_call) (function)
- [ocpp_client_match](#ocpp_client_match) (function)
- [ocpp_client_pending](#ocpp_client_pending) (function)
- [ocpp_boot_notification](#ocpp_boot_notification) (function)
- [ocpp_boot_conf](#ocpp_boot_conf) (function)
- [ocpp_heartbeat_conf](#ocpp_heartbeat_conf) (function)
- [ocpp_authorize](#ocpp_authorize) (function)
- [ocpp_id_tag_info](#ocpp_id_tag_info) (function)
- [ocpp_start_transaction](#ocpp_start_transaction) (function)
- [ocpp_start_conf](#ocpp_start_conf) (function)
- [ocpp_stop_transaction](#ocpp_stop_transaction) (function)
- [ocpp_status_notification](#ocpp_status_notification) (function)
- [ocpp_meter_values](#ocpp_meter_values) (function)
- [ocpp_remote_start](#ocpp_remote_start) (function)
- [ocpp_remote_stop](#ocpp_remote_stop) (function)
- [ocpp_transaction_event](#ocpp_transaction_event) (function)
- [ChargePoint](#ChargePoint) (struct)
- [cp_new](#cp_new) (function)
- [cp_step](#cp_step) (function)
- [CentralSystem](#CentralSystem) (struct)
- [cs_new](#cs_new) (function)
- [cs_accept_tag](#cs_accept_tag) (function)
- [cs_handle](#cs_handle) (function)

## OCPP_CALL:

```fides
pub const OCPP_CALL: i64 = 2;
```

*Defined at line 36*

---

## OCPP_CALLRESULT:

```fides
pub const OCPP_CALLRESULT: i64 = 3;
```

*Defined at line 37*

---

## OCPP_CALLERROR:

```fides
pub const OCPP_CALLERROR: i64 = 4;
```

*Defined at line 38*

---

## CP_AVAILABLE:

```fides
pub const CP_AVAILABLE: i64 = 0;
```

*Defined at line 41*

---

## CP_PREPARING:

```fides
pub const CP_PREPARING: i64 = 1;
```

*Defined at line 42*

---

## CP_CHARGING:

```fides
pub const CP_CHARGING: i64 = 2;
```

*Defined at line 43*

---

## CP_SUSPENDED_EV:

```fides
pub const CP_SUSPENDED_EV: i64 = 3;
```

*Defined at line 44*

---

## CP_FINISHING:

```fides
pub const CP_FINISHING: i64 = 4;
```

*Defined at line 45*

---

## CP_FAULTED:

```fides
pub const CP_FAULTED: i64 = 5;
```

*Defined at line 46*

---

## EV_PLUG_IN:

```fides
pub const EV_PLUG_IN: i64 = 0;
```

*Defined at line 49*

---

## EV_AUTHORIZE:

```fides
pub const EV_AUTHORIZE: i64 = 1;
```

*Defined at line 50*

---

## EV_START:

```fides
pub const EV_START: i64 = 2;
```

*Defined at line 51*

---

## EV_SUSPEND:

```fides
pub const EV_SUSPEND: i64 = 3;
```

*Defined at line 52*

---

## EV_RESUME:

```fides
pub const EV_RESUME: i64 = 4;
```

*Defined at line 53*

---

## EV_STOP:

```fides
pub const EV_STOP: i64 = 5;
```

*Defined at line 54*

---

## EV_UNPLUG:

```fides
pub const EV_UNPLUG: i64 = 6;
```

*Defined at line 55*

---

## EV_FAULT:

```fides
pub const EV_FAULT: i64 = 7;
```

*Defined at line 56*

---

## EV_FAULT_CLEAR:

```fides
pub const EV_FAULT_CLEAR: i64 = 8;
```

*Defined at line 57*

---

## ocpp_state_name

```fides
pub fn ocpp_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 59*

---

## ocpp_build_call

Build a CALL frame: [2, "<uniqueId>", "<action>", <payload JSON>].

```fides
pub fn ocpp_build_call(unique_id: str, action: str, payload_json: str) -> str {
```

**Parameters:**

- `unique_id`: `str`
- `action`: `str`
- `payload_json`: `str`

**Returns:** `str`

*Defined at line 84*

---

## ocpp_build_result

Build a CALLRESULT frame: [3, "<uniqueId>", <payload JSON>].

```fides
pub fn ocpp_build_result(unique_id: str, payload_json: str) -> str {
```

**Parameters:**

- `unique_id`: `str`
- `payload_json`: `str`

**Returns:** `str`

*Defined at line 89*

---

## ocpp_build_error

Build a CALLERROR frame: [4, "<uniqueId>", "<code>", "<desc>", {details}].

```fides
pub fn ocpp_build_error(unique_id: str, code: str, description: str, details_json: str) -> str {
```

**Parameters:**

- `unique_id`: `str`
- `code`: `str`
- `description`: `str`
- `details_json`: `str`

**Returns:** `str`

*Defined at line 94*

---

## OcppFrame

A parsed OCPP frame.

```fides
pub struct OcppFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `msg_type` | `i64` |
| `unique_id` | `str` |
| `action` | `str,  // CALL only` |
| `payload` | `str,  // CALL / CALLRESULT payload JSON` |
| `error_code` | `str,  // CALLERROR` |
| `error_desc` | `str,  // CALLERROR` |

*Defined at line 99*

---

## ocpp_parse

Parse any OCPP-J frame.

```fides
pub fn ocpp_parse(frame: str) -> OcppFrame {
```

**Parameters:**

- `frame`: `str`

**Returns:** `OcppFrame`

*Defined at line 110*

---

## OcppClient

A client that hands out unique ids and remembers open CALLs so a result
can be matched to its action.

```fides
pub struct OcppClient {
```

**Fields:**

| Name | Type |
|------|------|
| `next_id` | `i64` |
| `pend_ids` | `Vec<str>` |
| `pend_actions` | `Vec<str>` |

*Defined at line 142*

---

## ocpp_client_new

```fides
pub fn ocpp_client_new() -> OcppClient {
```

**Returns:** `OcppClient`

*Defined at line 148*

---

## ocpp_client_call

Produce the next unique id and remember a CALL of `action`; returns the
full CALL frame.

```fides
pub fn ocpp_client_call(c: &mut OcppClient, action: str, payload_json: str) -> str {
```

**Parameters:**

- `c`: `&mut OcppClient`
- `action`: `str`
- `payload_json`: `str`

**Returns:** `str`

*Defined at line 156*

---

## ocpp_client_match

Given a received result/error frame, return the action of the CALL it
answers (and forget that CALL); "" if none is open for its id.

```fides
pub fn ocpp_client_match(c: &mut OcppClient, frame: str) -> str {
```

**Parameters:**

- `c`: `&mut OcppClient`
- `frame`: `str`

**Returns:** `str`

*Defined at line 166*

---

## ocpp_client_pending

```fides
pub fn ocpp_client_pending(c: &OcppClient) -> i64 {
```

**Parameters:**

- `c`: `&OcppClient`

**Returns:** `i64`

*Defined at line 184*

---

## ocpp_boot_notification

BootNotification request payload.

```fides
pub fn ocpp_boot_notification(vendor: str, model: str) -> str {
```

**Parameters:**

- `vendor`: `str`
- `model`: `str`

**Returns:** `str`

*Defined at line 191*

---

## ocpp_boot_conf

BootNotification confirmation payload.

```fides
pub fn ocpp_boot_conf(status: str, current_time: str, interval: i64) -> str {
```

**Parameters:**

- `status`: `str`
- `current_time`: `str`
- `interval`: `i64`

**Returns:** `str`

*Defined at line 196*

---

## ocpp_heartbeat_conf

Heartbeat confirmation payload (just currentTime).

```fides
pub fn ocpp_heartbeat_conf(current_time: str) -> str {
```

**Parameters:**

- `current_time`: `str`

**Returns:** `str`

*Defined at line 201*

---

## ocpp_authorize

Authorize request payload.

```fides
pub fn ocpp_authorize(id_tag: str) -> str {
```

**Parameters:**

- `id_tag`: `str`

**Returns:** `str`

*Defined at line 206*

---

## ocpp_id_tag_info

idTagInfo + Authorize/StartTransaction confirmation with a status.

```fides
pub fn ocpp_id_tag_info(status: str) -> str {
```

**Parameters:**

- `status`: `str`

**Returns:** `str`

*Defined at line 211*

---

## ocpp_start_transaction

StartTransaction request payload.

```fides
pub fn ocpp_start_transaction(connector_id: i64, id_tag: str, meter_start: i64, timestamp: str) -> str {
```

**Parameters:**

- `connector_id`: `i64`
- `id_tag`: `str`
- `meter_start`: `i64`
- `timestamp`: `str`

**Returns:** `str`

*Defined at line 216*

---

## ocpp_start_conf

StartTransaction confirmation (transactionId + idTagInfo status).

```fides
pub fn ocpp_start_conf(transaction_id: i64, status: str) -> str {
```

**Parameters:**

- `transaction_id`: `i64`
- `status`: `str`

**Returns:** `str`

*Defined at line 221*

---

## ocpp_stop_transaction

StopTransaction request payload.

```fides
pub fn ocpp_stop_transaction(transaction_id: i64, meter_stop: i64, timestamp: str, reason: str) -> str {
```

**Parameters:**

- `transaction_id`: `i64`
- `meter_stop`: `i64`
- `timestamp`: `str`
- `reason`: `str`

**Returns:** `str`

*Defined at line 226*

---

## ocpp_status_notification

StatusNotification request payload (connector, status name, error code).

```fides
pub fn ocpp_status_notification(connector_id: i64, status: str, error_code: str) -> str {
```

**Parameters:**

- `connector_id`: `i64`
- `status`: `str`
- `error_code`: `str`

**Returns:** `str`

*Defined at line 231*

---

## ocpp_meter_values

MeterValues request payload: one sampled value (energy in Wh).

```fides
pub fn ocpp_meter_values(connector_id: i64, transaction_id: i64, timestamp: str, energy_wh: i64) -> str {
```

**Parameters:**

- `connector_id`: `i64`
- `transaction_id`: `i64`
- `timestamp`: `str`
- `energy_wh`: `i64`

**Returns:** `str`

*Defined at line 236*

---

## ocpp_remote_start

RemoteStartTransaction request payload.

```fides
pub fn ocpp_remote_start(id_tag: str, connector_id: i64) -> str {
```

**Parameters:**

- `id_tag`: `str`
- `connector_id`: `i64`

**Returns:** `str`

*Defined at line 243*

---

## ocpp_remote_stop

RemoteStopTransaction request payload.

```fides
pub fn ocpp_remote_stop(transaction_id: i64) -> str {
```

**Parameters:**

- `transaction_id`: `i64`

**Returns:** `str`

*Defined at line 248*

---

## ocpp_transaction_event

TransactionEvent request (2.0.1): eventType Started/Updated/Ended,
timestamp, triggerReason, a sequence number and the transaction id.

```fides
pub fn ocpp_transaction_event(event_type: str, timestamp: str, trigger: str, seq_no: i64, transaction_id: str) -> str {
```

**Parameters:**

- `event_type`: `str`
- `timestamp`: `str`
- `trigger`: `str`
- `seq_no`: `i64`
- `transaction_id`: `str`

**Returns:** `str`

*Defined at line 256*

---

## ChargePoint

A charge point with one connector's state and its current transaction.

```fides
pub struct ChargePoint {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `authorized` | `bool` |
| `transaction_id` | `i64` |
| `meter_wh` | `i64` |
| `faulted` | `bool` |

*Defined at line 263*

---

## cp_new

```fides
pub fn cp_new() -> ChargePoint {
```

**Returns:** `ChargePoint`

*Defined at line 271*

---

## cp_step

Advance the state machine by one event; returns the new state. Unknown
transitions leave the state unchanged. A fault forces Faulted until
cleared; from Faulted only EV_FAULT_CLEAR escapes (to Available).

```fides
pub fn cp_step(cp: &mut ChargePoint, event: i64) -> i64 {
```

**Parameters:**

- `cp`: `&mut ChargePoint`
- `event`: `i64`

**Returns:** `i64`

*Defined at line 280*

---

## CentralSystem

A simple central system: a set of accepted id tags and a transaction-id
counter.

```fides
pub struct CentralSystem {
```

**Fields:**

| Name | Type |
|------|------|
| `accepted_tags` | `Vec<str>` |
| `next_tx` | `i64` |
| `current_time` | `str` |
| `heartbeat_interval` | `i64` |

*Defined at line 334*

---

## cs_new

```fides
pub fn cs_new(current_time: str, interval: i64) -> CentralSystem {
```

**Parameters:**

- `current_time`: `str`
- `interval`: `i64`

**Returns:** `CentralSystem`

*Defined at line 341*

---

## cs_accept_tag

```fides
pub fn cs_accept_tag(cs: &mut CentralSystem, tag: str) {
```

**Parameters:**

- `cs`: `&mut CentralSystem`
- `tag`: `str`

*Defined at line 347*

---

## cs_handle

Handle a CALL frame; returns the CALLRESULT (or CALLERROR) frame to send
back. Supports BootNotification, Heartbeat, Authorize, StartTransaction
and StopTransaction; other actions get a NotImplemented CALLERROR.

```fides
pub fn cs_handle(cs: &mut CentralSystem, frame: str) -> str {
```

**Parameters:**

- `cs`: `&mut CentralSystem`
- `frame`: `str`

**Returns:** `str`

*Defined at line 365*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
