# scada — Fides Documentation

## Contents

- [MODBUS_FC_READ_COILS:](#modbus_fc_read_coils:) (const)
- [MODBUS_FC_READ_DISCRETE_INPUTS:](#modbus_fc_read_discrete_inputs:) (const)
- [MODBUS_FC_READ_HOLDING_REGS:](#modbus_fc_read_holding_regs:) (const)
- [MODBUS_FC_READ_INPUT_REGS:](#modbus_fc_read_input_regs:) (const)
- [MODBUS_FC_WRITE_SINGLE_COIL:](#modbus_fc_write_single_coil:) (const)
- [MODBUS_FC_WRITE_SINGLE_REG:](#modbus_fc_write_single_reg:) (const)
- [MODBUS_FC_WRITE_MULTIPLE_COILS:](#modbus_fc_write_multiple_coils:) (const)
- [MODBUS_FC_WRITE_MULTIPLE_REGS:](#modbus_fc_write_multiple_regs:) (const)
- [MODBUS_FC_READ_WRITE_MULTIPLE_REGS:](#modbus_fc_read_write_multiple_regs:) (const)
- [MODBUS_FC_DIAGNOSTICS:](#modbus_fc_diagnostics:) (const)
- [MODBUS_FC_GET_COMM_EVENT_COUNTER:](#modbus_fc_get_comm_event_counter:) (const)
- [MODBUS_FC_GET_COMM_EVENT_LOG:](#modbus_fc_get_comm_event_log:) (const)
- [MODBUS_FC_REPORT_SLAVE_ID:](#modbus_fc_report_slave_id:) (const)
- [MODBUS_FC_READ_FILE_RECORD:](#modbus_fc_read_file_record:) (const)
- [MODBUS_FC_WRITE_FILE_RECORD:](#modbus_fc_write_file_record:) (const)
- [MODBUS_FC_MASK_WRITE_REGISTER:](#modbus_fc_mask_write_register:) (const)
- [MODBUS_FC_READ_FIFO_QUEUE:](#modbus_fc_read_fifo_queue:) (const)
- [MODBUS_FC_ENCAPSULATED_INTERFACE:](#modbus_fc_encapsulated_interface:) (const)
- [MODBUS_EXC_ILLEGAL_FUNCTION:](#modbus_exc_illegal_function:) (const)
- [MODBUS_EXC_ILLEGAL_DATA_ADDRESS:](#modbus_exc_illegal_data_address:) (const)
- [MODBUS_EXC_ILLEGAL_DATA_VALUE:](#modbus_exc_illegal_data_value:) (const)
- [MODBUS_EXC_SLAVE_DEVICE_FAILURE:](#modbus_exc_slave_device_failure:) (const)
- [MODBUS_EXC_ACKNOWLEDGE:](#modbus_exc_acknowledge:) (const)
- [MODBUS_EXC_SLAVE_BUSY:](#modbus_exc_slave_busy:) (const)
- [MODBUS_EXC_NEGATIVE_ACK:](#modbus_exc_negative_ack:) (const)
- [MODBUS_EXC_MEMORY_PARITY_ERROR:](#modbus_exc_memory_parity_error:) (const)
- [MODBUS_EXC_GATEWAY_PATH_UNAVAILABLE:](#modbus_exc_gateway_path_unavailable:) (const)
- [MODBUS_EXC_GATEWAY_TARGET_NO_RESPONSE:](#modbus_exc_gateway_target_no_response:) (const)
- [modbus_fc_name](#modbus_fc_name) (function)
- [modbus_exc_name](#modbus_exc_name) (function)
- [ModbusFrame](#modbusframe) (struct)
- [modbus_crc16](#modbus_crc16) (function)
- [modbus_crc_verify](#modbus_crc_verify) (function)
- [modbus_read_holding_registers](#modbus_read_holding_registers) (function)
- [modbus_read_coils](#modbus_read_coils) (function)
- [modbus_write_single_register](#modbus_write_single_register) (function)
- [modbus_write_single_coil](#modbus_write_single_coil) (function)
- [modbus_write_multiple_registers](#modbus_write_multiple_registers) (function)
- [modbus_frame_serialize](#modbus_frame_serialize) (function)
- [modbus_frame_deserialize](#modbus_frame_deserialize) (function)
- [modbus_decode_registers](#modbus_decode_registers) (function)
- [modbus_decode_coils](#modbus_decode_coils) (function)
- [modbus_exception_code](#modbus_exception_code) (function)
- [ModbusTCPFrame](#modbustcpframe) (struct)
- [modbus_tcp_frame](#modbus_tcp_frame) (function)
- [modbus_tcp_serialize](#modbus_tcp_serialize) (function)
- [modbus_tcp_deserialize](#modbus_tcp_deserialize) (function)

## MODBUS_FC_READ_COILS:

```fides
pub const MODBUS_FC_READ_COILS: i64              = 0x01;
```

*Defined at line 26*

---

## MODBUS_FC_READ_DISCRETE_INPUTS:

```fides
pub const MODBUS_FC_READ_DISCRETE_INPUTS: i64    = 0x02;
```

*Defined at line 27*

---

## MODBUS_FC_READ_HOLDING_REGS:

```fides
pub const MODBUS_FC_READ_HOLDING_REGS: i64       = 0x03;
```

*Defined at line 28*

---

## MODBUS_FC_READ_INPUT_REGS:

```fides
pub const MODBUS_FC_READ_INPUT_REGS: i64         = 0x04;
```

*Defined at line 29*

---

## MODBUS_FC_WRITE_SINGLE_COIL:

```fides
pub const MODBUS_FC_WRITE_SINGLE_COIL: i64       = 0x05;
```

*Defined at line 30*

---

## MODBUS_FC_WRITE_SINGLE_REG:

```fides
pub const MODBUS_FC_WRITE_SINGLE_REG: i64        = 0x06;
```

*Defined at line 31*

---

## MODBUS_FC_WRITE_MULTIPLE_COILS:

```fides
pub const MODBUS_FC_WRITE_MULTIPLE_COILS: i64    = 0x0F;
```

*Defined at line 32*

---

## MODBUS_FC_WRITE_MULTIPLE_REGS:

```fides
pub const MODBUS_FC_WRITE_MULTIPLE_REGS: i64     = 0x10;
```

*Defined at line 33*

---

## MODBUS_FC_READ_WRITE_MULTIPLE_REGS:

```fides
pub const MODBUS_FC_READ_WRITE_MULTIPLE_REGS: i64 = 0x17;
```

*Defined at line 34*

---

## MODBUS_FC_DIAGNOSTICS:

```fides
pub const MODBUS_FC_DIAGNOSTICS: i64             = 0x08;
```

*Defined at line 35*

---

## MODBUS_FC_GET_COMM_EVENT_COUNTER:

```fides
pub const MODBUS_FC_GET_COMM_EVENT_COUNTER: i64  = 0x0B;
```

*Defined at line 36*

---

## MODBUS_FC_GET_COMM_EVENT_LOG:

```fides
pub const MODBUS_FC_GET_COMM_EVENT_LOG: i64      = 0x0C;
```

*Defined at line 37*

---

## MODBUS_FC_REPORT_SLAVE_ID:

```fides
pub const MODBUS_FC_REPORT_SLAVE_ID: i64         = 0x11;
```

*Defined at line 38*

---

## MODBUS_FC_READ_FILE_RECORD:

```fides
pub const MODBUS_FC_READ_FILE_RECORD: i64        = 0x14;
```

*Defined at line 39*

---

## MODBUS_FC_WRITE_FILE_RECORD:

```fides
pub const MODBUS_FC_WRITE_FILE_RECORD: i64       = 0x15;
```

*Defined at line 40*

---

## MODBUS_FC_MASK_WRITE_REGISTER:

```fides
pub const MODBUS_FC_MASK_WRITE_REGISTER: i64     = 0x16;
```

*Defined at line 41*

---

## MODBUS_FC_READ_FIFO_QUEUE:

```fides
pub const MODBUS_FC_READ_FIFO_QUEUE: i64         = 0x18;
```

*Defined at line 42*

---

## MODBUS_FC_ENCAPSULATED_INTERFACE:

```fides
pub const MODBUS_FC_ENCAPSULATED_INTERFACE: i64  = 0x2B;
```

*Defined at line 43*

---

## MODBUS_EXC_ILLEGAL_FUNCTION:

```fides
pub const MODBUS_EXC_ILLEGAL_FUNCTION: i64      = 0x01;
```

*Defined at line 47*

---

## MODBUS_EXC_ILLEGAL_DATA_ADDRESS:

```fides
pub const MODBUS_EXC_ILLEGAL_DATA_ADDRESS: i64  = 0x02;
```

*Defined at line 48*

---

## MODBUS_EXC_ILLEGAL_DATA_VALUE:

```fides
pub const MODBUS_EXC_ILLEGAL_DATA_VALUE: i64    = 0x03;
```

*Defined at line 49*

---

## MODBUS_EXC_SLAVE_DEVICE_FAILURE:

```fides
pub const MODBUS_EXC_SLAVE_DEVICE_FAILURE: i64  = 0x04;
```

*Defined at line 50*

---

## MODBUS_EXC_ACKNOWLEDGE:

```fides
pub const MODBUS_EXC_ACKNOWLEDGE: i64           = 0x05;
```

*Defined at line 51*

---

## MODBUS_EXC_SLAVE_BUSY:

```fides
pub const MODBUS_EXC_SLAVE_BUSY: i64            = 0x06;
```

*Defined at line 52*

---

## MODBUS_EXC_NEGATIVE_ACK:

```fides
pub const MODBUS_EXC_NEGATIVE_ACK: i64          = 0x07;
```

*Defined at line 53*

---

## MODBUS_EXC_MEMORY_PARITY_ERROR:

```fides
pub const MODBUS_EXC_MEMORY_PARITY_ERROR: i64   = 0x08;
```

*Defined at line 54*

---

## MODBUS_EXC_GATEWAY_PATH_UNAVAILABLE:

```fides
pub const MODBUS_EXC_GATEWAY_PATH_UNAVAILABLE: i64 = 0x0A;
```

*Defined at line 55*

---

## MODBUS_EXC_GATEWAY_TARGET_NO_RESPONSE:

```fides
pub const MODBUS_EXC_GATEWAY_TARGET_NO_RESPONSE: i64 = 0x0B;
```

*Defined at line 56*

---

## modbus_fc_name

```fides
pub fn modbus_fc_name(fc: i64) -> str {
```

**Parameters:**

- `fc`: `i64`

**Returns:** `str`

*Defined at line 60*

---

## modbus_exc_name

```fides
pub fn modbus_exc_name(exc: i64) -> str {
```

**Parameters:**

- `exc`: `i64`

**Returns:** `str`

*Defined at line 74*

---

## ModbusFrame

A Modbus RTU frame.

```fides
pub struct ModbusFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `slave_address` | `i64` |
| `function_code` | `i64` |
| `data` | `Vec` |
| `crc` | `i64` |
| `is_exception` | `bool` |

*Defined at line 94*

---

## modbus_crc16

```fides
pub fn modbus_crc16(data: Vec) -> i64 {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `i64`

*Defined at line 109*

---

## modbus_crc_verify

```fides
pub fn modbus_crc_verify(data: Vec) -> bool {
```

**Parameters:**

- `data`: `Vec`

**Returns:** `bool`

*Defined at line 127*

---

## modbus_read_holding_registers

Build a Read Holding Registers request (FC 0x03).
slave_addr: Modbus slave address (1-247)
start_addr: starting register address (0-65535)
quantity: number of registers to read (1-125)

```fides
pub fn modbus_read_holding_registers(slave_addr: i64, start_addr: i64, quantity: i64) -> ModbusFrame {
```

**Parameters:**

- `slave_addr`: `i64`
- `start_addr`: `i64`
- `quantity`: `i64`

**Returns:** `ModbusFrame`

*Defined at line 147*

---

## modbus_read_coils

Build a Read Coils request (FC 0x01).

```fides
pub fn modbus_read_coils(slave_addr: i64, start_addr: i64, quantity: i64) -> ModbusFrame {
```

**Parameters:**

- `slave_addr`: `i64`
- `start_addr`: `i64`
- `quantity`: `i64`

**Returns:** `ModbusFrame`

*Defined at line 163*

---

## modbus_write_single_register

Build a Write Single Register request (FC 0x06).

```fides
pub fn modbus_write_single_register(slave_addr: i64, reg_addr: i64, value: i64) -> ModbusFrame {
```

**Parameters:**

- `slave_addr`: `i64`
- `reg_addr`: `i64`
- `value`: `i64`

**Returns:** `ModbusFrame`

*Defined at line 179*

---

## modbus_write_single_coil

Build a Write Single Coil request (FC 0x05).
value: 0xFF00 for ON, 0x0000 for OFF.

```fides
pub fn modbus_write_single_coil(slave_addr: i64, coil_addr: i64, on: bool) -> ModbusFrame {
```

**Parameters:**

- `slave_addr`: `i64`
- `coil_addr`: `i64`
- `on`: `bool`

**Returns:** `ModbusFrame`

*Defined at line 196*

---

## modbus_write_multiple_registers

Build a Write Multiple Registers request (FC 0x10).

```fides
pub fn modbus_write_multiple_registers(slave_addr: i64, start_addr: i64,
```

**Parameters:**

- `slave_addr`: `i64`
- `start_addr`: `i64`

*Defined at line 217*

---

## modbus_frame_serialize

Serialize a Modbus RTU frame to bytes (including CRC).

```fides
pub fn modbus_frame_serialize(frame: ModbusFrame) -> Vec {
```

**Parameters:**

- `frame`: `ModbusFrame`

**Returns:** `Vec`

*Defined at line 244*

---

## modbus_frame_deserialize

Deserialize bytes into a Modbus RTU frame.
Returns a ModbusFrame. Check is_exception to detect exception responses.

```fides
pub fn modbus_frame_deserialize(bytes: Vec) -> ModbusFrame {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `ModbusFrame`

*Defined at line 264*

---

## modbus_decode_registers

Decode register values from a Read Holding Registers response.
Returns a Vec of register values (16-bit each).

```fides
pub fn modbus_decode_registers(frame: ModbusFrame) -> Vec {
```

**Parameters:**

- `frame`: `ModbusFrame`

**Returns:** `Vec`

*Defined at line 296*

---

## modbus_decode_coils

Decode coil status from a Read Coils response.
Returns a Vec of 0/1 values.

```fides
pub fn modbus_decode_coils(frame: ModbusFrame) -> Vec {
```

**Parameters:**

- `frame`: `ModbusFrame`

**Returns:** `Vec`

*Defined at line 319*

---

## modbus_exception_code

Get exception code from an exception response.
Returns -1 if not an exception response.

```fides
pub fn modbus_exception_code(frame: ModbusFrame) -> i64 {
```

**Parameters:**

- `frame`: `ModbusFrame`

**Returns:** `i64`

*Defined at line 343*

---

## ModbusTCPFrame

A Modbus TCP frame.

```fides
pub struct ModbusTCPFrame {
```

**Fields:**

| Name | Type |
|------|------|
| `transaction_id` | `i64` |
| `unit_id` | `i64` |
| `function_code` | `i64` |
| `data` | `Vec` |

*Defined at line 360*

---

## modbus_tcp_frame

Build a Modbus TCP frame from a function code and data.

```fides
pub fn modbus_tcp_frame(transaction_id: i64, unit_id: i64, fc: i64, data: Vec) -> ModbusTCPFrame {
```

**Parameters:**

- `transaction_id`: `i64`
- `unit_id`: `i64`
- `fc`: `i64`
- `data`: `Vec`

**Returns:** `ModbusTCPFrame`

*Defined at line 368*

---

## modbus_tcp_serialize

Serialize a Modbus TCP frame to bytes.

```fides
pub fn modbus_tcp_serialize(frame: ModbusTCPFrame) -> Vec {
```

**Parameters:**

- `frame`: `ModbusTCPFrame`

**Returns:** `Vec`

*Defined at line 378*

---

## modbus_tcp_deserialize

Deserialize bytes into a Modbus TCP frame.

```fides
pub fn modbus_tcp_deserialize(bytes: Vec) -> ModbusTCPFrame {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `ModbusTCPFrame`

*Defined at line 396*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
