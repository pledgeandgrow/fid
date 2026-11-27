[All modules](index.md)

# openadr -- Fides Documentation

## Contents

- [OADR_STATUS_NONE:](#OADR_STATUS_NONE:) (const)
- [OADR_STATUS_FAR:](#OADR_STATUS_FAR:) (const)
- [OADR_STATUS_NEAR:](#OADR_STATUS_NEAR:) (const)
- [OADR_STATUS_ACTIVE:](#OADR_STATUS_ACTIVE:) (const)
- [OADR_STATUS_COMPLETED:](#OADR_STATUS_COMPLETED:) (const)
- [OADR_STATUS_CANCELLED:](#OADR_STATUS_CANCELLED:) (const)
- [openadr_status_name](#openadr_status_name) (function)
- [openadr_status_parse](#openadr_status_parse) (function)
- [VEN_UNREGISTERED:](#VEN_UNREGISTERED:) (const)
- [VEN_REGISTERED:](#VEN_REGISTERED:) (const)
- [VEN_POLLING:](#VEN_POLLING:) (const)
- [VEN_EVENT_ACTIVE:](#VEN_EVENT_ACTIVE:) (const)
- [OadrInterval](#OadrInterval) (struct)
- [OadrSignal](#OadrSignal) (struct)
- [OadrEvent](#OadrEvent) (struct)
- [openadr_distribute_event](#openadr_distribute_event) (function)
- [openadr_parse_distribute_event](#openadr_parse_distribute_event) (function)
- [openadr_signal_value_at](#openadr_signal_value_at) (function)
- [openadr_register_report](#openadr_register_report) (function)
- [openadr_registered_report](#openadr_registered_report) (function)
- [openadr_poll](#openadr_poll) (function)
- [openadr_created_event](#openadr_created_event) (function)
- [openadr_parse_opt](#openadr_parse_opt) (function)
- [OadrVen](#OadrVen) (struct)
- [openadr_ven_new](#openadr_ven_new) (function)
- [ven_register](#ven_register) (function)
- [ven_next_request](#ven_next_request) (function)
- [ven_poll](#ven_poll) (function)
- [ven_on_event](#ven_on_event) (function)
- [ven_opt](#ven_opt) (function)

## OADR_STATUS_NONE:

```fides
pub const OADR_STATUS_NONE: i64 = 0;
```

*Defined at line 39*

---

## OADR_STATUS_FAR:

```fides
pub const OADR_STATUS_FAR: i64 = 1;
```

*Defined at line 40*

---

## OADR_STATUS_NEAR:

```fides
pub const OADR_STATUS_NEAR: i64 = 2;
```

*Defined at line 41*

---

## OADR_STATUS_ACTIVE:

```fides
pub const OADR_STATUS_ACTIVE: i64 = 3;
```

*Defined at line 42*

---

## OADR_STATUS_COMPLETED:

```fides
pub const OADR_STATUS_COMPLETED: i64 = 4;
```

*Defined at line 43*

---

## OADR_STATUS_CANCELLED:

```fides
pub const OADR_STATUS_CANCELLED: i64 = 5;
```

*Defined at line 44*

---

## openadr_status_name

```fides
pub fn openadr_status_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 46*

---

## openadr_status_parse

```fides
pub fn openadr_status_parse(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 65*

---

## VEN_UNREGISTERED:

```fides
pub const VEN_UNREGISTERED: i64 = 0;
```

*Defined at line 85*

---

## VEN_REGISTERED:

```fides
pub const VEN_REGISTERED: i64 = 1;
```

*Defined at line 86*

---

## VEN_POLLING:

```fides
pub const VEN_POLLING: i64 = 2;
```

*Defined at line 87*

---

## VEN_EVENT_ACTIVE:

```fides
pub const VEN_EVENT_ACTIVE: i64 = 3;
```

*Defined at line 88*

---

## OadrInterval

One interval of a signal: a duration string and a float payload value.

```fides
pub struct OadrInterval {
```

**Fields:**

| Name | Type |
|------|------|
| `duration` | `str` |
| `value` | `f64` |

*Defined at line 93*

---

## OadrSignal

A signal: name, type, and its intervals.

```fides
pub struct OadrSignal {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `signal_type` | `str` |
| `intervals` | `Vec<OadrInterval>` |

*Defined at line 99*

---

## OadrEvent

A full EiEvent.

```fides
pub struct OadrEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `event_id` | `str` |
| `modification_number` | `i64` |
| `priority` | `i64` |
| `market_context` | `str` |
| `created` | `str` |
| `status` | `i64` |
| `dtstart` | `str` |
| `duration` | `str` |
| `signal` | `OadrSignal` |
| `ven_id` | `str` |
| `group_id` | `str` |

*Defined at line 106*

---

## openadr_distribute_event

Build an oadrDistributeEvent payload carrying one event.

```fides
pub fn openadr_distribute_event(requestID: str, vtnID: str, ev: &OadrEvent) -> str {
```

**Parameters:**

- `requestID`: `str`
- `vtnID`: `str`
- `ev`: `&OadrEvent`

**Returns:** `str`

*Defined at line 127*

---

## openadr_parse_distribute_event

Parse an oadrDistributeEvent payload into an OadrEvent (the first event).

```fides
pub fn openadr_parse_distribute_event(payload: str) -> OadrEvent {
```

**Parameters:**

- `payload`: `str`

**Returns:** `OadrEvent`

*Defined at line 216*

---

## openadr_signal_value_at

The signal value active at interval index `idx` (0-based). Returns 0.0
when out of range.

```fides
pub fn openadr_signal_value_at(ev: &OadrEvent, idx: i64) -> f64 {
```

**Parameters:**

- `ev`: `&OadrEvent`
- `idx`: `i64`

**Returns:** `f64`

*Defined at line 265*

---

## openadr_register_report

Build an oadrRegisterReport with one report description.

```fides
pub fn openadr_register_report(requestID: str, venID: str, reportSpecifierID: str, rid: str, reading_type: str) -> str {
```

**Parameters:**

- `requestID`: `str`
- `venID`: `str`
- `reportSpecifierID`: `str`
- `rid`: `str`
- `reading_type`: `str`

**Returns:** `str`

*Defined at line 275*

---

## openadr_registered_report

Build the oadrRegisteredReport response.

```fides
pub fn openadr_registered_report(requestID: str, responseCode: i64, venID: str) -> str {
```

**Parameters:**

- `requestID`: `str`
- `responseCode`: `i64`
- `venID`: `str`

**Returns:** `str`

*Defined at line 291*

---

## openadr_poll

Build an oadrPoll request.

```fides
pub fn openadr_poll(venID: str) -> str {
```

**Parameters:**

- `venID`: `str`

**Returns:** `str`

*Defined at line 305*

---

## openadr_created_event

Build an oadrCreatedEvent carrying one opt decision for an event.

```fides
pub fn openadr_created_event(requestID: str, venID: str, eventID: str, modificationNumber: i64, optIn: bool) -> str {
```

**Parameters:**

- `requestID`: `str`
- `venID`: `str`
- `eventID`: `str`
- `modificationNumber`: `i64`
- `optIn`: `bool`

**Returns:** `str`

*Defined at line 310*

---

## openadr_parse_opt

Read the optType from an oadrCreatedEvent payload.

```fides
pub fn openadr_parse_opt(payload: str) -> str {
```

**Parameters:**

- `payload`: `str`

**Returns:** `str`

*Defined at line 334*

---

## OadrVen

A VEN: its id, state, the id of the active event (if any) and the last
opt decision made.

```fides
pub struct OadrVen {
```

**Fields:**

| Name | Type |
|------|------|
| `ven_id` | `str` |
| `state` | `i64` |
| `active_event` | `str` |
| `last_opt_in` | `bool` |
| `poll_count` | `i64` |

*Defined at line 346*

---

## openadr_ven_new

```fides
pub fn openadr_ven_new(ven_id: str) -> OadrVen {
```

**Parameters:**

- `ven_id`: `str`

**Returns:** `OadrVen`

*Defined at line 354*

---

## ven_register

Register the VEN (after a successful registration response).

```fides
pub fn ven_register(v: &mut OadrVen) {
```

**Parameters:**

- `v`: `&mut OadrVen`

*Defined at line 361*

---

## ven_next_request

The next request the VEN should send: "oadrCreatePartyRegistration",
"oadrPoll", or "oadrCreatedEvent".

```fides
pub fn ven_next_request(v: &OadrVen) -> str {
```

**Parameters:**

- `v`: `&OadrVen`

**Returns:** `str`

*Defined at line 367*

---

## ven_poll

The VEN polls; counts the poll. Returns the request name.

```fides
pub fn ven_poll(v: &mut OadrVen) -> str {
```

**Parameters:**

- `v`: `&mut OadrVen`

**Returns:** `str`

*Defined at line 378*

---

## ven_on_event

The VEN received a distribute-event with the given event id; it becomes
event-active awaiting an opt decision.

```fides
pub fn ven_on_event(v: &mut OadrVen, event_id: str) {
```

**Parameters:**

- `v`: `&mut OadrVen`
- `event_id`: `str`

*Defined at line 388*

---

## ven_opt

The VEN decides opt-in/opt-out; returns to polling.

```fides
pub fn ven_opt(v: &mut OadrVen, opt_in: bool) {
```

**Parameters:**

- `v`: `&mut OadrVen`
- `opt_in`: `bool`

*Defined at line 394*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
