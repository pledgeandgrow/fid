[All modules](index.md)

# can -- Fides Documentation

## Contents

- [CanFrame](#CanFrame) (struct)
- [can_frame_standard](#can_frame_standard) (function)
- [can_frame_extended](#can_frame_extended) (function)
- [can_frame_remote_standard](#can_frame_remote_standard) (function)
- [can_frame_remote_extended](#can_frame_remote_extended) (function)
- [can_frame_id](#can_frame_id) (function)
- [can_frame_dlc](#can_frame_dlc) (function)
- [can_frame_is_extended](#can_frame_is_extended) (function)
- [can_frame_is_remote](#can_frame_is_remote) (function)
- [can_frame_data](#can_frame_data) (function)
- [can_frame_byte](#can_frame_byte) (function)
- [can_validate_standard_id](#can_validate_standard_id) (function)
- [can_validate_extended_id](#can_validate_extended_id) (function)
- [can_validate_dlc](#can_validate_dlc) (function)
- [j1939_id](#j1939_id) (function)
- [j1939_priority](#j1939_priority) (function)
- [j1939_pdu_format](#j1939_pdu_format) (function)
- [j1939_pdu_specific](#j1939_pdu_specific) (function)
- [j1939_source_addr](#j1939_source_addr) (function)
- [j1939_is_pdu1](#j1939_is_pdu1) (function)
- [j1939_is_pdu2](#j1939_is_pdu2) (function)
- [j1939_pgn](#j1939_pgn) (function)
- [UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:](#UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:) (const)
- [UDS_SERVICE_ECU_RESET:](#UDS_SERVICE_ECU_RESET:) (const)
- [UDS_SERVICE_CLEAR_DTC:](#UDS_SERVICE_CLEAR_DTC:) (const)
- [UDS_SERVICE_READ_DTC:](#UDS_SERVICE_READ_DTC:) (const)
- [UDS_SERVICE_READ_DATA_BY_ID:](#UDS_SERVICE_READ_DATA_BY_ID:) (const)
- [UDS_SERVICE_READ_MEMORY_BY_ADDR:](#UDS_SERVICE_READ_MEMORY_BY_ADDR:) (const)
- [UDS_SERVICE_WRITE_DATA_BY_ID:](#UDS_SERVICE_WRITE_DATA_BY_ID:) (const)
- [UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_ID:](#UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_ID:) (const)
- [UDS_SERVICE_ROUTINE_CONTROL:](#UDS_SERVICE_ROUTINE_CONTROL:) (const)
- [UDS_SERVICE_REQUEST_DOWNLOAD:](#UDS_SERVICE_REQUEST_DOWNLOAD:) (const)
- [UDS_SERVICE_REQUEST_UPLOAD:](#UDS_SERVICE_REQUEST_UPLOAD:) (const)
- [UDS_SERVICE_TRANSFER_DATA:](#UDS_SERVICE_TRANSFER_DATA:) (const)
- [UDS_SERVICE_REQUEST_TRANSFER_EXIT:](#UDS_SERVICE_REQUEST_TRANSFER_EXIT:) (const)
- [UDS_SERVICE_REQUEST_FILE_TRANSFER:](#UDS_SERVICE_REQUEST_FILE_TRANSFER:) (const)
- [UDS_SERVICE_TESTER_PRESENT:](#UDS_SERVICE_TESTER_PRESENT:) (const)
- [UDS_SERVICE_ACCESS_TIMING:](#UDS_SERVICE_ACCESS_TIMING:) (const)
- [UDS_SERVICE_SECURED_DATA_TRANSMISSION:](#UDS_SERVICE_SECURED_DATA_TRANSMISSION:) (const)
- [UDS_SERVICE_CONTROL_DTC_SETTING:](#UDS_SERVICE_CONTROL_DTC_SETTING:) (const)
- [UDS_SERVICE_RESPONSE_ON_EVENT:](#UDS_SERVICE_RESPONSE_ON_EVENT:) (const)
- [UDS_SERVICE_LINK_CONTROL:](#UDS_SERVICE_LINK_CONTROL:) (const)
- [UDS_SERVICE_READ_DATA_BY_PERIODIC_ID:](#UDS_SERVICE_READ_DATA_BY_PERIODIC_ID:) (const)
- [UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_ID:](#UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_ID:) (const)
- [UDS_SERVICE_DEFINE_DATA_BY_ID:](#UDS_SERVICE_DEFINE_DATA_BY_ID:) (const)
- [UDS_SERVICE_WRITE_MEMORY_BY_ADDR:](#UDS_SERVICE_WRITE_MEMORY_BY_ADDR:) (const)
- [UDS_SERVICE_COMMUNICATION_CONTROL:](#UDS_SERVICE_COMMUNICATION_CONTROL:) (const)
- [UDS_SERVICE_AUTHENTICATION:](#UDS_SERVICE_AUTHENTICATION:) (const)
- [UDS_NRC_GENERAL_REJECT:](#UDS_NRC_GENERAL_REJECT:) (const)
- [UDS_NRC_SERVICE_NOT_SUPPORTED:](#UDS_NRC_SERVICE_NOT_SUPPORTED:) (const)
- [UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED:](#UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED:) (const)
- [UDS_NRC_INVALID_FORMAT:](#UDS_NRC_INVALID_FORMAT:) (const)
- [UDS_NRC_RESPONSE_TOO_LONG:](#UDS_NRC_RESPONSE_TOO_LONG:) (const)
- [UDS_NRC_BUSY_REPEAT_REQUEST:](#UDS_NRC_BUSY_REPEAT_REQUEST:) (const)
- [UDS_NRC_CONDITIONS_NOT_CORRECT:](#UDS_NRC_CONDITIONS_NOT_CORRECT:) (const)
- [UDS_NRC_REQUEST_SEQUENCE_ERROR:](#UDS_NRC_REQUEST_SEQUENCE_ERROR:) (const)
- [UDS_NRC_REQUEST_OUT_OF_RANGE:](#UDS_NRC_REQUEST_OUT_OF_RANGE:) (const)
- [UDS_NRC_SECURITY_ACCESS_DENIED:](#UDS_NRC_SECURITY_ACCESS_DENIED:) (const)
- [UDS_NRC_INVALID_KEY:](#UDS_NRC_INVALID_KEY:) (const)
- [UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS:](#UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS:) (const)
- [UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED:](#UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED:) (const)
- [UDS_NRC_RESPONSE_PENDING:](#UDS_NRC_RESPONSE_PENDING:) (const)
- [UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_ACTIVE:](#UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_ACTIVE:) (const)
- [UDS_NRC_SERVICE_NOT_SUPPORTED_ACTIVE:](#UDS_NRC_SERVICE_NOT_SUPPORTED_ACTIVE:) (const)
- [uds_service_id](#uds_service_id) (function)
- [uds_sub_function](#uds_sub_function) (function)
- [uds_data](#uds_data) (function)
- [uds_response_positive](#uds_response_positive) (function)
- [uds_response_negative](#uds_response_negative) (function)
- [uds_is_positive_response](#uds_is_positive_response) (function)
- [uds_is_negative_response](#uds_is_negative_response) (function)
- [uds_nrc](#uds_nrc) (function)
- [DTC_SYSTEM_POWERTRAIN:](#DTC_SYSTEM_POWERTRAIN:) (const)
- [DTC_SYSTEM_CHASSIS:](#DTC_SYSTEM_CHASSIS:) (const)
- [DTC_SYSTEM_BODY:](#DTC_SYSTEM_BODY:) (const)
- [DTC_SYSTEM_NETWORK:](#DTC_SYSTEM_NETWORK:) (const)
- [dtc_encode](#dtc_encode) (function)
- [dtc_system](#dtc_system) (function)
- [dtc_code](#dtc_code) (function)
- [dtc_system_char](#dtc_system_char) (function)
- [obd_read_dtcs](#obd_read_dtcs) (function)
- [obd_clear_dtcs](#obd_clear_dtcs) (function)
- [obd_supported_pids](#obd_supported_pids) (function)
- [obd_engine_rpm_request](#obd_engine_rpm_request) (function)
- [obd_decode_engine_rpm](#obd_decode_engine_rpm) (function)
- [obd_vehicle_speed_request](#obd_vehicle_speed_request) (function)
- [obd_decode_vehicle_speed](#obd_decode_vehicle_speed) (function)
- [obd_coolant_temp_request](#obd_coolant_temp_request) (function)
- [obd_decode_coolant_temp](#obd_decode_coolant_temp) (function)
- [OBD_BROADCAST_ID:](#OBD_BROADCAST_ID:) (const)
- [OBD_RESPONSE_BASE:](#OBD_RESPONSE_BASE:) (const)
- [obd_request_to_ecu](#obd_request_to_ecu) (function)
- [obd_is_response_from_ecu](#obd_is_response_from_ecu) (function)
- [obd_is_response](#obd_is_response) (function)
- [UDS_SESSION_DEFAULT:](#UDS_SESSION_DEFAULT:) (const)
- [UDS_SESSION_PROGRAMMING:](#UDS_SESSION_PROGRAMMING:) (const)
- [UDS_SESSION_EXTENDED:](#UDS_SESSION_EXTENDED:) (const)
- [UDS_SESSION_SAFETY:](#UDS_SESSION_SAFETY:) (const)
- [UDS_SESSION_PROTECTION:](#UDS_SESSION_PROTECTION:) (const)
- [uds_session_control](#uds_session_control) (function)
- [UDS_RESET_HARD:](#UDS_RESET_HARD:) (const)
- [UDS_RESET_KEY_OFF:](#UDS_RESET_KEY_OFF:) (const)
- [UDS_RESET_SOFT:](#UDS_RESET_SOFT:) (const)
- [UDS_RESET_ENABLE_RAPID:](#UDS_RESET_ENABLE_RAPID:) (const)
- [UDS_RESET_DISABLE_RAPID:](#UDS_RESET_DISABLE_RAPID:) (const)
- [uds_ecu_reset](#uds_ecu_reset) (function)
- [uds_security_access_request_seed](#uds_security_access_request_seed) (function)
- [uds_security_access_send_key](#uds_security_access_send_key) (function)
- [uds_read_data_by_id](#uds_read_data_by_id) (function)
- [UDS_DID_VIN:](#UDS_DID_VIN:) (const)
- [UDS_DID_ECU_SERIAL_NUMBER:](#UDS_DID_ECU_SERIAL_NUMBER:) (const)
- [UDS_DID_ECU_MANUFACTURE_DATE:](#UDS_DID_ECU_MANUFACTURE_DATE:) (const)
- [UDS_DID_ECU_SOFTWARE_VERSION:](#UDS_DID_ECU_SOFTWARE_VERSION:) (const)
- [UDS_DID_ECU_HARDWARE_VERSION:](#UDS_DID_ECU_HARDWARE_VERSION:) (const)
- [UDS_DID_BOOT_SOFTWARE_IDENTIFICATION:](#UDS_DID_BOOT_SOFTWARE_IDENTIFICATION:) (const)
- [can_frame_print](#can_frame_print) (function)

## CanFrame

A CAN frame (standard or extended, data or remote).
Layout: [id: i64, dlc: i64, is_extended: bool, is_remote: bool, data: Vec]

```fides
pub struct CanFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `dlc` | `i64` |
| `is_extended` | `bool` |
| `is_remote` | `bool` |
| `data` | `Vec<i64>` |

*Defined at line 39*

---

## can_frame_standard

Create a standard (11-bit ID) data frame.
ID must be in range 0x000-0x7FF. DLC is the data length code (0-8).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d: Vec<i64> = Vec::new();
d.push(0x02); d.push(0x01); d.push(0x0C);
var f = can_frame_standard(0x7DF, d);
assert_eq(can_frame_id(f), 0x7DF);
assert_eq(can_frame_dlc(f), 3);
assert(!can_frame_is_extended(f));
assert_eq(can_frame_byte(f, 2), 0x0C);
assert(can_validate_standard_id(0x7FF));
assert(!can_validate_standard_id(0x800));
```

```fides
pub fn can_frame_standard(id: i64, data: Vec<i64>) -> CanFrame {
```

**Parameters:**

- `id`: `i64`
- `data`: `Vec<i64>`

**Returns:** `CanFrame`

*Defined at line 62*

---

## can_frame_extended

Create an extended (29-bit ID) data frame.
ID must be in range 0x00000000-0x1FFFFFFF.

```fides
pub fn can_frame_extended(id: i64, data: Vec<i64>) -> CanFrame {
```

**Parameters:**

- `id`: `i64`
- `data`: `Vec<i64>`

**Returns:** `CanFrame`

*Defined at line 70*

---

## can_frame_remote_standard

Create a standard remote frame (request for data, no payload).

```fides
pub fn can_frame_remote_standard(id: i64, dlc: i64) -> CanFrame {
```

**Parameters:**

- `id`: `i64`
- `dlc`: `i64`

**Returns:** `CanFrame`

*Defined at line 77*

---

## can_frame_remote_extended

Create an extended remote frame.

```fides
pub fn can_frame_remote_extended(id: i64, dlc: i64) -> CanFrame {
```

**Parameters:**

- `id`: `i64`
- `dlc`: `i64`

**Returns:** `CanFrame`

*Defined at line 84*

---

## can_frame_id

Get the CAN ID (11 or 29 bit).

```fides
pub fn can_frame_id(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 93*

---

## can_frame_dlc

Get the data length code (0-8 for classic CAN, 0-64 for CAN FD).

```fides
pub fn can_frame_dlc(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 98*

---

## can_frame_is_extended

Check if the frame uses an extended (29-bit) ID.

```fides
pub fn can_frame_is_extended(frame: CanFrame) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `bool`

*Defined at line 103*

---

## can_frame_is_remote

Check if the frame is a remote frame (no data payload).

```fides
pub fn can_frame_is_remote(frame: CanFrame) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `bool`

*Defined at line 108*

---

## can_frame_data

Get the data payload as a Vec of bytes.

```fides
pub fn can_frame_data(frame: CanFrame) -> Vec<i64> {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `Vec<i64>`

*Defined at line 113*

---

## can_frame_byte

Get a single byte from the frame data at the given index.
Returns -1 if index is out of bounds.

```fides
pub fn can_frame_byte(frame: CanFrame, index: i64) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`
- `index`: `i64`

**Returns:** `i64`

*Defined at line 119*

---

## can_validate_standard_id

Validate a standard CAN ID (11-bit, 0x000-0x7FF).

```fides
pub fn can_validate_standard_id(id: i64) -> bool {
```

**Parameters:**

- `id`: `i64`

**Returns:** `bool`

*Defined at line 129*

---

## can_validate_extended_id

Validate an extended CAN ID (29-bit, 0x00000000-0x1FFFFFFF).

```fides
pub fn can_validate_extended_id(id: i64) -> bool {
```

**Parameters:**

- `id`: `i64`

**Returns:** `bool`

*Defined at line 134*

---

## can_validate_dlc

Validate a DLC value (0-8 for classic CAN).

```fides
pub fn can_validate_dlc(dlc: i64) -> bool {
```

**Parameters:**

- `dlc`: `i64`

**Returns:** `bool`

*Defined at line 139*

---

## j1939_id

Build a J1939 CAN ID from its components.
priority: 0-7 (0 = highest)
reserved: 0 or 1
data_page: 0 or 1
pdu_format: 0-255
pdu_specific: 0-255
source_addr: 0-255

```fides
pub fn j1939_id(priority: i64, reserved: i64, data_page: i64, pdu_format: i64, pdu_specific: i64, source_addr: i64) -> i64 {
```

**Parameters:**

- `priority`: `i64`
- `reserved`: `i64`
- `data_page`: `i64`
- `pdu_format`: `i64`
- `pdu_specific`: `i64`
- `source_addr`: `i64`

**Returns:** `i64`

*Defined at line 159*

---

## j1939_priority

Extract the priority (0-7) from a J1939 CAN ID.

```fides
pub fn j1939_priority(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 170*

---

## j1939_pdu_format

Extract the PDU Format (0-255) from a J1939 CAN ID.

```fides
pub fn j1939_pdu_format(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 175*

---

## j1939_pdu_specific

Extract the PDU Specific (destination or group extension) from a J1939 CAN ID.

```fides
pub fn j1939_pdu_specific(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 180*

---

## j1939_source_addr

Extract the Source Address (0-255) from a J1939 CAN ID.

```fides
pub fn j1939_source_addr(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 185*

---

## j1939_is_pdu1

Check if a J1939 frame is PDU1 (destination-specific, PF >= 240).

```fides
pub fn j1939_is_pdu1(id: i64) -> bool {
```

**Parameters:**

- `id`: `i64`

**Returns:** `bool`

*Defined at line 190*

---

## j1939_is_pdu2

Check if a J1939 frame is PDU2 (broadcast, PF < 240).

```fides
pub fn j1939_is_pdu2(id: i64) -> bool {
```

**Parameters:**

- `id`: `i64`

**Returns:** `bool`

*Defined at line 195*

---

## j1939_pgn

Extract the PGN (Parameter Group Number) from a J1939 CAN ID.
For PDU1 (PF >= 240): PGN = (DP << 16) | (PF << 8) | 0
For PDU2 (PF < 240):  PGN = (DP << 16) | (PF << 8) | PS

```fides
pub fn j1939_pgn(id: i64) -> i64 {
```

**Parameters:**

- `id`: `i64`

**Returns:** `i64`

*Defined at line 202*

---

## UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:

UDS Service IDs (ISO 14229-1)

```fides
pub const UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL: i64 = 0x10;
```

*Defined at line 222*

---

## UDS_SERVICE_ECU_RESET:

```fides
pub const UDS_SERVICE_ECU_RESET: i64 = 0x11;
```

*Defined at line 223*

---

## UDS_SERVICE_CLEAR_DTC:

```fides
pub const UDS_SERVICE_CLEAR_DTC: i64 = 0x14;
```

*Defined at line 224*

---

## UDS_SERVICE_READ_DTC:

```fides
pub const UDS_SERVICE_READ_DTC: i64 = 0x19;
```

*Defined at line 225*

---

## UDS_SERVICE_READ_DATA_BY_ID:

```fides
pub const UDS_SERVICE_READ_DATA_BY_ID: i64 = 0x22;
```

*Defined at line 226*

---

## UDS_SERVICE_READ_MEMORY_BY_ADDR:

```fides
pub const UDS_SERVICE_READ_MEMORY_BY_ADDR: i64 = 0x23;
```

*Defined at line 227*

---

## UDS_SERVICE_WRITE_DATA_BY_ID:

```fides
pub const UDS_SERVICE_WRITE_DATA_BY_ID: i64 = 0x2E;
```

*Defined at line 228*

---

## UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_ID:

```fides
pub const UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_ID: i64 = 0x2F;
```

*Defined at line 229*

---

## UDS_SERVICE_ROUTINE_CONTROL:

```fides
pub const UDS_SERVICE_ROUTINE_CONTROL: i64 = 0x31;
```

*Defined at line 230*

---

## UDS_SERVICE_REQUEST_DOWNLOAD:

```fides
pub const UDS_SERVICE_REQUEST_DOWNLOAD: i64 = 0x34;
```

*Defined at line 231*

---

## UDS_SERVICE_REQUEST_UPLOAD:

```fides
pub const UDS_SERVICE_REQUEST_UPLOAD: i64 = 0x35;
```

*Defined at line 232*

---

## UDS_SERVICE_TRANSFER_DATA:

```fides
pub const UDS_SERVICE_TRANSFER_DATA: i64 = 0x36;
```

*Defined at line 233*

---

## UDS_SERVICE_REQUEST_TRANSFER_EXIT:

```fides
pub const UDS_SERVICE_REQUEST_TRANSFER_EXIT: i64 = 0x37;
```

*Defined at line 234*

---

## UDS_SERVICE_REQUEST_FILE_TRANSFER:

```fides
pub const UDS_SERVICE_REQUEST_FILE_TRANSFER: i64 = 0x38;
```

*Defined at line 235*

---

## UDS_SERVICE_TESTER_PRESENT:

```fides
pub const UDS_SERVICE_TESTER_PRESENT: i64 = 0x3E;
```

*Defined at line 236*

---

## UDS_SERVICE_ACCESS_TIMING:

```fides
pub const UDS_SERVICE_ACCESS_TIMING: i64 = 0x83;
```

*Defined at line 237*

---

## UDS_SERVICE_SECURED_DATA_TRANSMISSION:

```fides
pub const UDS_SERVICE_SECURED_DATA_TRANSMISSION: i64 = 0x84;
```

*Defined at line 238*

---

## UDS_SERVICE_CONTROL_DTC_SETTING:

```fides
pub const UDS_SERVICE_CONTROL_DTC_SETTING: i64 = 0x85;
```

*Defined at line 239*

---

## UDS_SERVICE_RESPONSE_ON_EVENT:

```fides
pub const UDS_SERVICE_RESPONSE_ON_EVENT: i64 = 0x86;
```

*Defined at line 240*

---

## UDS_SERVICE_LINK_CONTROL:

```fides
pub const UDS_SERVICE_LINK_CONTROL: i64 = 0x87;
```

*Defined at line 241*

---

## UDS_SERVICE_READ_DATA_BY_PERIODIC_ID:

```fides
pub const UDS_SERVICE_READ_DATA_BY_PERIODIC_ID: i64 = 0x2A;
```

*Defined at line 242*

---

## UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_ID:

```fides
pub const UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_ID: i64 = 0x2C;
```

*Defined at line 243*

---

## UDS_SERVICE_DEFINE_DATA_BY_ID:

```fides
pub const UDS_SERVICE_DEFINE_DATA_BY_ID: i64 = 0x2B;
```

*Defined at line 244*

---

## UDS_SERVICE_WRITE_MEMORY_BY_ADDR:

```fides
pub const UDS_SERVICE_WRITE_MEMORY_BY_ADDR: i64 = 0x3D;
```

*Defined at line 245*

---

## UDS_SERVICE_COMMUNICATION_CONTROL:

```fides
pub const UDS_SERVICE_COMMUNICATION_CONTROL: i64 = 0x28;
```

*Defined at line 246*

---

## UDS_SERVICE_AUTHENTICATION:

```fides
pub const UDS_SERVICE_AUTHENTICATION: i64 = 0x29;
```

*Defined at line 247*

---

## UDS_NRC_GENERAL_REJECT:

UDS Negative Response Codes (NRC)

```fides
pub const UDS_NRC_GENERAL_REJECT: i64 = 0x10;
```

*Defined at line 249*

---

## UDS_NRC_SERVICE_NOT_SUPPORTED:

```fides
pub const UDS_NRC_SERVICE_NOT_SUPPORTED: i64 = 0x11;
```

*Defined at line 250*

---

## UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED:

```fides
pub const UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED: i64 = 0x12;
```

*Defined at line 251*

---

## UDS_NRC_INVALID_FORMAT:

```fides
pub const UDS_NRC_INVALID_FORMAT: i64 = 0x13;
```

*Defined at line 252*

---

## UDS_NRC_RESPONSE_TOO_LONG:

```fides
pub const UDS_NRC_RESPONSE_TOO_LONG: i64 = 0x14;
```

*Defined at line 253*

---

## UDS_NRC_BUSY_REPEAT_REQUEST:

```fides
pub const UDS_NRC_BUSY_REPEAT_REQUEST: i64 = 0x21;
```

*Defined at line 254*

---

## UDS_NRC_CONDITIONS_NOT_CORRECT:

```fides
pub const UDS_NRC_CONDITIONS_NOT_CORRECT: i64 = 0x22;
```

*Defined at line 255*

---

## UDS_NRC_REQUEST_SEQUENCE_ERROR:

```fides
pub const UDS_NRC_REQUEST_SEQUENCE_ERROR: i64 = 0x24;
```

*Defined at line 256*

---

## UDS_NRC_REQUEST_OUT_OF_RANGE:

```fides
pub const UDS_NRC_REQUEST_OUT_OF_RANGE: i64 = 0x31;
```

*Defined at line 257*

---

## UDS_NRC_SECURITY_ACCESS_DENIED:

```fides
pub const UDS_NRC_SECURITY_ACCESS_DENIED: i64 = 0x33;
```

*Defined at line 258*

---

## UDS_NRC_INVALID_KEY:

```fides
pub const UDS_NRC_INVALID_KEY: i64 = 0x35;
```

*Defined at line 259*

---

## UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS:

```fides
pub const UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS: i64 = 0x36;
```

*Defined at line 260*

---

## UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED:

```fides
pub const UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED: i64 = 0x37;
```

*Defined at line 261*

---

## UDS_NRC_RESPONSE_PENDING:

```fides
pub const UDS_NRC_RESPONSE_PENDING: i64 = 0x78;
```

*Defined at line 262*

---

## UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_ACTIVE:

```fides
pub const UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_ACTIVE: i64 = 0x7E;
```

*Defined at line 263*

---

## UDS_NRC_SERVICE_NOT_SUPPORTED_ACTIVE:

```fides
pub const UDS_NRC_SERVICE_NOT_SUPPORTED_ACTIVE: i64 = 0x7F;
```

*Defined at line 264*

---

## uds_service_id

Extract the UDS service ID from a CAN frame's first data byte.
Returns -1 if the frame has no data.

```fides
pub fn uds_service_id(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 267*

---

## uds_sub_function

Extract the UDS sub-function from a CAN frame's second data byte.
Returns -1 if the frame has fewer than 2 bytes.

```fides
pub fn uds_sub_function(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 276*

---

## uds_data

Extract UDS data payload (bytes after service ID + sub-function).
Returns a Vec of the remaining bytes.

```fides
pub fn uds_data(frame: CanFrame) -> Vec<i64> {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `Vec<i64>`

*Defined at line 285*

---

## uds_response_positive

Build a positive UDS response frame.
The response service ID is (request_service_id | 0x40).
Format: [Service ID + 0x40] [Sub-function] [data...]

```fides
pub fn uds_response_positive(request: CanFrame, response_data: Vec<i64>) -> CanFrame {
```

**Parameters:**

- `request`: `CanFrame`
- `response_data`: `Vec<i64>`

**Returns:** `CanFrame`

*Defined at line 299*

---

## uds_response_negative

Build a negative UDS response frame.
Format: [0x7F] [Service ID] [NRC]

```fides
pub fn uds_response_negative(request: CanFrame, nrc: i64) -> CanFrame {
```

**Parameters:**

- `request`: `CanFrame`
- `nrc`: `i64`

**Returns:** `CanFrame`

*Defined at line 315*

---

## uds_is_positive_response

Check if a CAN frame is a UDS positive response.

```fides
pub fn uds_is_positive_response(frame: CanFrame) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `bool`

*Defined at line 325*

---

## uds_is_negative_response

Check if a CAN frame is a UDS negative response.

```fides
pub fn uds_is_negative_response(frame: CanFrame) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `bool`

*Defined at line 331*

---

## uds_nrc

Extract the NRC from a negative response frame.
Returns -1 if the frame is not a negative response.

```fides
pub fn uds_nrc(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 337*

---

## DTC_SYSTEM_POWERTRAIN:

DTC system prefixes

```fides
pub const DTC_SYSTEM_POWERTRAIN: i64 = 0;
```

*Defined at line 356*

---

## DTC_SYSTEM_CHASSIS:

```fides
pub const DTC_SYSTEM_CHASSIS: i64 = 1;
```

*Defined at line 357*

---

## DTC_SYSTEM_BODY:

```fides
pub const DTC_SYSTEM_BODY: i64 = 2;
```

*Defined at line 358*

---

## DTC_SYSTEM_NETWORK:

```fides
pub const DTC_SYSTEM_NETWORK: i64 = 3;
```

*Defined at line 359*

---

## dtc_encode

Encode a DTC from system (0-3) and code digits (3 hex digits as i64).
Returns a 2-byte packed i64: [system << 14 | code]

```fides
pub fn dtc_encode(system: i64, code: i64) -> i64 {
```

**Parameters:**

- `system`: `i64`
- `code`: `i64`

**Returns:** `i64`

*Defined at line 362*

---

## dtc_system

Decode the system prefix (0=Powertrain, 1=Chassis, 2=Body, 3=Network).

```fides
pub fn dtc_system(dtc: i64) -> i64 {
```

**Parameters:**

- `dtc`: `i64`

**Returns:** `i64`

*Defined at line 367*

---

## dtc_code

Decode the DTC code digits (14-bit).

```fides
pub fn dtc_code(dtc: i64) -> i64 {
```

**Parameters:**

- `dtc`: `i64`

**Returns:** `i64`

*Defined at line 372*

---

## dtc_system_char

Get the DTC system as a character: 'P', 'C', 'B', 'U'.

```fides
pub fn dtc_system_char(dtc: i64) -> str {
```

**Parameters:**

- `dtc`: `i64`

**Returns:** `str`

*Defined at line 377*

---

## obd_read_dtcs

OBD-II Mode 03: Read DTCs. Build a request frame.

```fides
pub fn obd_read_dtcs() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 394*

---

## obd_clear_dtcs

OBD-II Mode 04: Clear DTCs. Build a request frame.

```fides
pub fn obd_clear_dtcs() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 401*

---

## obd_supported_pids

OBD-II Mode 01 PID 00: Supported PIDs 01-20.

```fides
pub fn obd_supported_pids() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 408*

---

## obd_engine_rpm_request

OBD-II Mode 01 PID 0C: Engine RPM.
Response: [0x41, 0x0C, RPM_High, RPM_Low] where RPM = ((A*256)+B)/4

```fides
pub fn obd_engine_rpm_request() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 417*

---

## obd_decode_engine_rpm

Decode engine RPM from an OBD-II Mode 01 PID 0C response.
Returns RPM as i64, or -1 if the frame is invalid.

```fides
pub fn obd_decode_engine_rpm(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 426*

---

## obd_vehicle_speed_request

OBD-II Mode 01 PID 0D: Vehicle Speed (km/h).

```fides
pub fn obd_vehicle_speed_request() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 442*

---

## obd_decode_vehicle_speed

Decode vehicle speed from an OBD-II Mode 01 PID 0D response.
Returns speed in km/h, or -1 if the frame is invalid.

```fides
pub fn obd_decode_vehicle_speed(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 451*

---

## obd_coolant_temp_request

OBD-II Mode 01 PID 05: Engine Coolant Temperature (°C).
Response: [0x41, 0x05, Temp] where Temp = A - 40

```fides
pub fn obd_coolant_temp_request() -> CanFrame {
```

**Returns:** `CanFrame`

*Defined at line 466*

---

## obd_decode_coolant_temp

Decode coolant temperature from an OBD-II Mode 01 PID 05 response.
Returns temperature in °C, or -1 if the frame is invalid.

```fides
pub fn obd_decode_coolant_temp(frame: CanFrame) -> i64 {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `i64`

*Defined at line 475*

---

## OBD_BROADCAST_ID:

OBD-II functional broadcast ID (requests to all ECUs).

```fides
pub const OBD_BROADCAST_ID: i64 = 0x7DF;
```

*Defined at line 491*

---

## OBD_RESPONSE_BASE:

OBD-II ECU response ID base (0x7E8-0x7EF for 8 ECUs).

```fides
pub const OBD_RESPONSE_BASE: i64 = 0x7E8;
```

*Defined at line 493*

---

## obd_request_to_ecu

Build an OBD-II request to a specific ECU (0-7).

```fides
pub fn obd_request_to_ecu(ecu: i64, mode: i64, pid: i64) -> CanFrame {
```

**Parameters:**

- `ecu`: `i64`
- `mode`: `i64`
- `pid`: `i64`

**Returns:** `CanFrame`

*Defined at line 495*

---

## obd_is_response_from_ecu

Check if a frame is an OBD-II response from a specific ECU.

```fides
pub fn obd_is_response_from_ecu(frame: CanFrame, ecu: i64) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`
- `ecu`: `i64`

**Returns:** `bool`

*Defined at line 503*

---

## obd_is_response

Check if a frame is an OBD-II response from any ECU.

```fides
pub fn obd_is_response(frame: CanFrame) -> bool {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `bool`

*Defined at line 508*

---

## UDS_SESSION_DEFAULT:

UDS Session Types

```fides
pub const UDS_SESSION_DEFAULT: i64 = 0x01;
```

*Defined at line 515*

---

## UDS_SESSION_PROGRAMMING:

```fides
pub const UDS_SESSION_PROGRAMMING: i64 = 0x02;
```

*Defined at line 516*

---

## UDS_SESSION_EXTENDED:

```fides
pub const UDS_SESSION_EXTENDED: i64 = 0x03;
```

*Defined at line 517*

---

## UDS_SESSION_SAFETY:

```fides
pub const UDS_SESSION_SAFETY: i64 = 0x04;
```

*Defined at line 518*

---

## UDS_SESSION_PROTECTION:

```fides
pub const UDS_SESSION_PROTECTION: i64 = 0x05;
```

*Defined at line 519*

---

## uds_session_control

Build a UDS Diagnostic Session Control request.

```fides
pub fn uds_session_control(session: i64) -> CanFrame {
```

**Parameters:**

- `session`: `i64`

**Returns:** `CanFrame`

*Defined at line 521*

---

## UDS_RESET_HARD:

UDS Reset Types

```fides
pub const UDS_RESET_HARD: i64 = 0x01;
```

*Defined at line 531*

---

## UDS_RESET_KEY_OFF:

```fides
pub const UDS_RESET_KEY_OFF: i64 = 0x02;
```

*Defined at line 532*

---

## UDS_RESET_SOFT:

```fides
pub const UDS_RESET_SOFT: i64 = 0x03;
```

*Defined at line 533*

---

## UDS_RESET_ENABLE_RAPID:

```fides
pub const UDS_RESET_ENABLE_RAPID: i64 = 0x04;
```

*Defined at line 534*

---

## UDS_RESET_DISABLE_RAPID:

```fides
pub const UDS_RESET_DISABLE_RAPID: i64 = 0x05;
```

*Defined at line 535*

---

## uds_ecu_reset

Build a UDS ECU Reset request.

```fides
pub fn uds_ecu_reset(reset_type: i64) -> CanFrame {
```

**Parameters:**

- `reset_type`: `i64`

**Returns:** `CanFrame`

*Defined at line 537*

---

## uds_security_access_request_seed

Build a UDS Security Access - Request Seed frame.

```fides
pub fn uds_security_access_request_seed(level: i64) -> CanFrame {
```

**Parameters:**

- `level`: `i64`

**Returns:** `CanFrame`

*Defined at line 547*

---

## uds_security_access_send_key

Build a UDS Security Access - Send Key frame.

```fides
pub fn uds_security_access_send_key(level: i64, key: Vec<i64>) -> CanFrame {
```

**Parameters:**

- `level`: `i64`
- `key`: `Vec<i64>`

**Returns:** `CanFrame`

*Defined at line 555*

---

## uds_read_data_by_id

Build a UDS Read Data By Identifier request.
did: 16-bit Data Identifier (0x0000-0xFFFF)

```fides
pub fn uds_read_data_by_id(did: i64) -> CanFrame {
```

**Parameters:**

- `did`: `i64`

**Returns:** `CanFrame`

*Defined at line 569*

---

## UDS_DID_VIN:

Common UDS Data Identifiers

```fides
pub const UDS_DID_VIN: i64 = 0xF190;
```

*Defined at line 578*

---

## UDS_DID_ECU_SERIAL_NUMBER:

```fides
pub const UDS_DID_ECU_SERIAL_NUMBER: i64 = 0xF18C;
```

*Defined at line 579*

---

## UDS_DID_ECU_MANUFACTURE_DATE:

```fides
pub const UDS_DID_ECU_MANUFACTURE_DATE: i64 = 0xF18B;
```

*Defined at line 580*

---

## UDS_DID_ECU_SOFTWARE_VERSION:

```fides
pub const UDS_DID_ECU_SOFTWARE_VERSION: i64 = 0xF191;
```

*Defined at line 581*

---

## UDS_DID_ECU_HARDWARE_VERSION:

```fides
pub const UDS_DID_ECU_HARDWARE_VERSION: i64 = 0xF192;
```

*Defined at line 582*

---

## UDS_DID_BOOT_SOFTWARE_IDENTIFICATION:

```fides
pub const UDS_DID_BOOT_SOFTWARE_IDENTIFICATION: i64 = 0xF195;
```

*Defined at line 583*

---

## can_frame_print

Print a CAN frame in a human-readable format.
Format: "CAN [STD|EXT] ID:0xXXX DLC:N [DATA]"

```fides
pub fn can_frame_print(frame: CanFrame) -> void {
```

**Parameters:**

- `frame`: `CanFrame`

**Returns:** `void`

*Defined at line 588*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
