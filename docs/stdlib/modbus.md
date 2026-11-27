[All modules](index.md)

# modbus -- Fides Documentation

## Contents

- [MODBUS_READ_COILS:](#MODBUS_READ_COILS:) (const)
- [MODBUS_READ_DISCRETE:](#MODBUS_READ_DISCRETE:) (const)
- [MODBUS_READ_HOLDING:](#MODBUS_READ_HOLDING:) (const)
- [MODBUS_READ_INPUT:](#MODBUS_READ_INPUT:) (const)
- [MODBUS_WRITE_COIL:](#MODBUS_WRITE_COIL:) (const)
- [MODBUS_WRITE_REGISTER:](#MODBUS_WRITE_REGISTER:) (const)
- [MODBUS_WRITE_COILS:](#MODBUS_WRITE_COILS:) (const)
- [MODBUS_WRITE_REGISTERS:](#MODBUS_WRITE_REGISTERS:) (const)
- [MODBUS_READ_WRITE:](#MODBUS_READ_WRITE:) (const)
- [MODBUS_EX_ILLEGAL_FUNCTION:](#MODBUS_EX_ILLEGAL_FUNCTION:) (const)
- [MODBUS_EX_ILLEGAL_ADDRESS:](#MODBUS_EX_ILLEGAL_ADDRESS:) (const)
- [MODBUS_EX_ILLEGAL_VALUE:](#MODBUS_EX_ILLEGAL_VALUE:) (const)
- [MODBUS_EX_DEVICE_FAILURE:](#MODBUS_EX_DEVICE_FAILURE:) (const)
- [modbus_crc16](#modbus_crc16) (function)
- [ModbusPdu](#ModbusPdu) (struct)
- [modbus_pack_bits](#modbus_pack_bits) (function)
- [modbus_unpack_bits](#modbus_unpack_bits) (function)
- [modbus_req_read](#modbus_req_read) (function)
- [modbus_req_write_coil](#modbus_req_write_coil) (function)
- [modbus_req_write_register](#modbus_req_write_register) (function)
- [modbus_req_write_coils](#modbus_req_write_coils) (function)
- [modbus_req_write_registers](#modbus_req_write_registers) (function)
- [modbus_req_read_write](#modbus_req_read_write) (function)
- [modbus_decode_request](#modbus_decode_request) (function)
- [modbus_encode_response](#modbus_encode_response) (function)
- [modbus_decode_response](#modbus_decode_response) (function)
- [modbus_rtu_frame](#modbus_rtu_frame) (function)
- [ModbusAdu](#ModbusAdu) (struct)
- [modbus_rtu_parse](#modbus_rtu_parse) (function)
- [modbus_rtu_frame_len](#modbus_rtu_frame_len) (function)
- [modbus_tcp_frame](#modbus_tcp_frame) (function)
- [modbus_tcp_frame_len](#modbus_tcp_frame_len) (function)
- [modbus_tcp_parse](#modbus_tcp_parse) (function)
- [ModbusMap](#ModbusMap) (struct)
- [modbus_map_new](#modbus_map_new) (function)
- [modbus_server_handle](#modbus_server_handle) (function)
- [modbus_server_rtu](#modbus_server_rtu) (function)
- [modbus_server_tcp](#modbus_server_tcp) (function)
- [ModbusClient](#ModbusClient) (struct)
- [modbus_client_new](#modbus_client_new) (function)
- [modbus_client_request](#modbus_client_request) (function)
- [modbus_client_pending](#modbus_client_pending) (function)
- [modbus_client_response](#modbus_client_response) (function)

## MODBUS_READ_COILS:

```fides
pub const MODBUS_READ_COILS: i64 = 1;
```

*Defined at line 33*

---

## MODBUS_READ_DISCRETE:

```fides
pub const MODBUS_READ_DISCRETE: i64 = 2;
```

*Defined at line 34*

---

## MODBUS_READ_HOLDING:

```fides
pub const MODBUS_READ_HOLDING: i64 = 3;
```

*Defined at line 35*

---

## MODBUS_READ_INPUT:

```fides
pub const MODBUS_READ_INPUT: i64 = 4;
```

*Defined at line 36*

---

## MODBUS_WRITE_COIL:

```fides
pub const MODBUS_WRITE_COIL: i64 = 5;
```

*Defined at line 37*

---

## MODBUS_WRITE_REGISTER:

```fides
pub const MODBUS_WRITE_REGISTER: i64 = 6;
```

*Defined at line 38*

---

## MODBUS_WRITE_COILS:

```fides
pub const MODBUS_WRITE_COILS: i64 = 15;
```

*Defined at line 39*

---

## MODBUS_WRITE_REGISTERS:

```fides
pub const MODBUS_WRITE_REGISTERS: i64 = 16;
```

*Defined at line 40*

---

## MODBUS_READ_WRITE:

```fides
pub const MODBUS_READ_WRITE: i64 = 23;
```

*Defined at line 41*

---

## MODBUS_EX_ILLEGAL_FUNCTION:

```fides
pub const MODBUS_EX_ILLEGAL_FUNCTION: i64 = 1;
```

*Defined at line 43*

---

## MODBUS_EX_ILLEGAL_ADDRESS:

```fides
pub const MODBUS_EX_ILLEGAL_ADDRESS: i64 = 2;
```

*Defined at line 44*

---

## MODBUS_EX_ILLEGAL_VALUE:

```fides
pub const MODBUS_EX_ILLEGAL_VALUE: i64 = 3;
```

*Defined at line 45*

---

## MODBUS_EX_DEVICE_FAILURE:

```fides
pub const MODBUS_EX_DEVICE_FAILURE: i64 = 4;
```

*Defined at line 46*

---

## modbus_crc16

CRC-16/MODBUS of `b` ("123456789" gives 0x4B37).

```fides
pub fn modbus_crc16(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 51*

---

## ModbusPdu

A request or response PDU, decoded. For reads `addr`/`qty`; for single
writes `addr`/`value`; for 23 also `waddr`/`wqty`. `regs` holds register
values, `bits` coil or input states (0/1). `exception` > 0 for an
exception response (its function is fc, without 0x80).

```fides
pub struct ModbusPdu {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `fc` | `i64` |
| `addr` | `i64` |
| `qty` | `i64` |
| `value` | `i64` |
| `waddr` | `i64` |
| `wqty` | `i64` |
| `regs` | `Vec<i64>` |
| `bits` | `Vec<i64>` |
| `exception` | `i64` |

*Defined at line 76*

---

## modbus_pack_bits

Pack 0/1 values, first in the lowest bit of the first byte.

```fides
pub fn modbus_pack_bits(bits: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `bits`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 110*

---

## modbus_unpack_bits

```fides
pub fn modbus_unpack_bits(bytes: &Vec<i64>, start: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `&Vec<i64>`
- `start`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 126*

---

## modbus_req_read

Read request (functions 1-4): function, address, quantity.

```fides
pub fn modbus_req_read(fc: i64, addr: i64, qty: i64) -> Vec<i64> {
```

**Parameters:**

- `fc`: `i64`
- `addr`: `i64`
- `qty`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 137*

---

## modbus_req_write_coil

Write Single Coil (5): on is 0xFF00, off 0x0000.

```fides
pub fn modbus_req_write_coil(addr: i64, on: bool) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `on`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 146*

---

## modbus_req_write_register

Write Single Register (6).

```fides
pub fn modbus_req_write_register(addr: i64, value: i64) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `value`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 159*

---

## modbus_req_write_coils

Write Multiple Coils (15).

```fides
pub fn modbus_req_write_coils(addr: i64, bits: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `bits`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 168*

---

## modbus_req_write_registers

Write Multiple Registers (16).

```fides
pub fn modbus_req_write_registers(addr: i64, regs: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `regs`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 180*

---

## modbus_req_read_write

Read/Write Multiple Registers (23): the write is done before the read.

```fides
pub fn modbus_req_read_write(raddr: i64, rqty: i64, waddr: i64, regs: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `raddr`: `i64`
- `rqty`: `i64`
- `waddr`: `i64`
- `regs`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 195*

---

## modbus_decode_request

Decode a request PDU. A malformed one has ok false and `exception` set
to what a server answers (1 unknown function, 3 bad quantity/length).

```fides
pub fn modbus_decode_request(b: &Vec<i64>) -> ModbusPdu {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `ModbusPdu`

*Defined at line 224*

---

## modbus_encode_response

Encode a response PDU: reads carry `bits` (1, 2) or `regs` (3, 4, 23);
writes echo addr/value or addr/qty; `exception` > 0 gives fc|0x80, code.

```fides
pub fn modbus_encode_response(p: &ModbusPdu) -> Vec<i64> {
```

**Parameters:**

- `p`: `&ModbusPdu`

**Returns:** `Vec<i64>`

*Defined at line 323*

---

## modbus_decode_response

Decode a response PDU to request `req` (also decoded): the function
must match, the byte count fit the quantity asked, writes must echo.

```fides
pub fn modbus_decode_response(b: &Vec<i64>, req: &ModbusPdu) -> ModbusPdu {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `req`: `&ModbusPdu`

**Returns:** `ModbusPdu`

*Defined at line 354*

---

## modbus_rtu_frame

An RTU frame: unit address, PDU, CRC (low byte first).

```fides
pub fn modbus_rtu_frame(unit: i64, pdu: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `unit`: `i64`
- `pdu`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 416*

---

## ModbusAdu

A frame split into unit and PDU.

```fides
pub struct ModbusAdu {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `unit` | `i64` |
| `tid` | `i64` |
| `pdu` | `Vec<i64>` |

*Defined at line 427*

---

## modbus_rtu_parse

Check an RTU frame's CRC and split it.

```fides
pub fn modbus_rtu_parse(f: &Vec<i64>) -> ModbusAdu {
```

**Parameters:**

- `f`: `&Vec<i64>`

**Returns:** `ModbusAdu`

*Defined at line 436*

---

## modbus_rtu_frame_len

The length of the RTU frame at the start of `buf` (`request` true for
frames to a server): 0 while more bytes are needed, -1 for an unknown
function code.

```fides
pub fn modbus_rtu_frame_len(buf: &Vec<i64>, request: bool) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`
- `request`: `bool`

**Returns:** `i64`

*Defined at line 468*

---

## modbus_tcp_frame

A Modbus TCP frame: MBAP header (transaction, protocol 0, length,
unit) and the PDU.

```fides
pub fn modbus_tcp_frame(tid: i64, unit: i64, pdu: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `tid`: `i64`
- `unit`: `i64`
- `pdu`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 511*

---

## modbus_tcp_frame_len

Length of the first TCP frame in `buf`: 0 while incomplete, -1 when
the header is invalid.

```fides
pub fn modbus_tcp_frame_len(buf: &Vec<i64>) -> i64 {
```

**Parameters:**

- `buf`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 523*

---

## modbus_tcp_parse

```fides
pub fn modbus_tcp_parse(f: &Vec<i64>) -> ModbusAdu {
```

**Parameters:**

- `f`: `&Vec<i64>`

**Returns:** `ModbusAdu`

*Defined at line 537*

---

## ModbusMap

The four data tables of a server, addressed from 0.

```fides
pub struct ModbusMap {
```

**Fields:**

| Name | Type |
|------|------|
| `coils` | `Vec<i64>` |
| `discrete` | `Vec<i64>` |
| `holding` | `Vec<i64>` |
| `input` | `Vec<i64>` |
| `writes` | `i64,  // write requests carried out` |

*Defined at line 560*

---

## modbus_map_new

```fides
pub fn modbus_map_new(ncoils: i64, ndiscrete: i64, nholding: i64, ninput: i64) -> ModbusMap {
```

**Parameters:**

- `ncoils`: `i64`
- `ndiscrete`: `i64`
- `nholding`: `i64`
- `ninput`: `i64`

**Returns:** `ModbusMap`

*Defined at line 568*

---

## modbus_server_handle

Carry out a request PDU on the map; the response PDU (an exception
response for an unknown function, bad value or address out of range).

```fides
pub fn modbus_server_handle(m: &mut ModbusMap, pdu: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `m`: `&mut ModbusMap`
- `pdu`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 597*

---

## modbus_server_rtu

Serve one RTU frame for unit `unit`: the response frame, or [] when the
frame is for another unit, has a bad CRC, or is a broadcast (unit 0,
carried out but not answered).

```fides
pub fn modbus_server_rtu(m: &mut ModbusMap, unit: i64, frame: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `m`: `&mut ModbusMap`
- `unit`: `i64`
- `frame`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 685*

---

## modbus_server_tcp

Serve one TCP frame: the response frame (same transaction and unit).

```fides
pub fn modbus_server_tcp(m: &mut ModbusMap, frame: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `m`: `&mut ModbusMap`
- `frame`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 698*

---

## ModbusClient

A client: TCP when `tcp`, else RTU (one request at a time).

```fides
pub struct ModbusClient {
```

**Fields:**

| Name | Type |
|------|------|
| `tcp` | `bool` |
| `next_tid` | `i64` |
| `pend_tids` | `Vec<i64>` |
| `pend_units` | `Vec<i64>` |
| `pend_pdus` | `Vec<Vec<i64>>` |

*Defined at line 710*

---

## modbus_client_new

```fides
pub fn modbus_client_new(tcp: bool) -> ModbusClient {
```

**Parameters:**

- `tcp`: `bool`

**Returns:** `ModbusClient`

*Defined at line 718*

---

## modbus_client_request

The frame to send for request `pdu` to `unit` (and remember it).

```fides
pub fn modbus_client_request(c: &mut ModbusClient, unit: i64, pdu: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `c`: `&mut ModbusClient`
- `unit`: `i64`
- `pdu`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 725*

---

## modbus_client_pending

Requests still waiting for their response.

```fides
pub fn modbus_client_pending(c: &ModbusClient) -> i64 {
```

**Parameters:**

- `c`: `&ModbusClient`

**Returns:** `i64`

*Defined at line 745*

---

## modbus_client_response

Match a response frame to its request and decode it.

```fides
pub fn modbus_client_response(c: &mut ModbusClient, frame: &Vec<i64>) -> ModbusPdu {
```

**Parameters:**

- `c`: `&mut ModbusClient`
- `frame`: `&Vec<i64>`

**Returns:** `ModbusPdu`

*Defined at line 750*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
