[All modules](index.md)

# i2c -- Fides Documentation

## Contents

- [I2C_RETRIES:](#I2C_RETRIES:) (const)
- [I2C_TIMEOUT:](#I2C_TIMEOUT:) (const)
- [I2C_SLAVE:](#I2C_SLAVE:) (const)
- [I2C_TENBIT:](#I2C_TENBIT:) (const)
- [I2C_FUNCS:](#I2C_FUNCS:) (const)
- [I2C_SLAVE_FORCE:](#I2C_SLAVE_FORCE:) (const)
- [I2C_RDWR:](#I2C_RDWR:) (const)
- [I2C_PEC:](#I2C_PEC:) (const)
- [I2C_SMBUS:](#I2C_SMBUS:) (const)
- [I2C_RDWR_MAX_MSGS:](#I2C_RDWR_MAX_MSGS:) (const)
- [I2C_M_RD:](#I2C_M_RD:) (const)
- [I2C_M_TEN:](#I2C_M_TEN:) (const)
- [I2C_M_DMA_SAFE:](#I2C_M_DMA_SAFE:) (const)
- [I2C_M_RECV_LEN:](#I2C_M_RECV_LEN:) (const)
- [I2C_M_NO_RD_ACK:](#I2C_M_NO_RD_ACK:) (const)
- [I2C_M_IGNORE_NAK:](#I2C_M_IGNORE_NAK:) (const)
- [I2C_M_REV_DIR_ADDR:](#I2C_M_REV_DIR_ADDR:) (const)
- [I2C_M_NOSTART:](#I2C_M_NOSTART:) (const)
- [I2C_M_STOP:](#I2C_M_STOP:) (const)
- [I2C_O_RDWR:](#I2C_O_RDWR:) (const)
- [I2C_BUS_FAKE:](#I2C_BUS_FAKE:) (const)
- [I2C_BUS_FN:](#I2C_BUS_FN:) (const)
- [I2cMsg](#I2cMsg) (struct)
- [i2c_msg_write](#i2c_msg_write) (function)
- [i2c_msg_read](#i2c_msg_read) (function)
- [i2c_msg_ten](#i2c_msg_ten) (function)
- [i2c_msg_is_read](#i2c_msg_is_read) (function)
- [i2c_addr7_valid](#i2c_addr7_valid) (function)
- [i2c_addr10_valid](#i2c_addr10_valid) (function)
- [i2c_addr_wire](#i2c_addr_wire) (function)
- [i2c_addr_unwire7](#i2c_addr_unwire7) (function)
- [i2c_encode_request](#i2c_encode_request) (function)
- [i2c_decode_request](#i2c_decode_request) (function)
- [I2cFakeDev](#I2cFakeDev) (struct)
- [i2c_fake_dev](#i2c_fake_dev) (function)
- [i2c_fake_dev16](#i2c_fake_dev16) (function)
- [i2c_fake_dev_ten](#i2c_fake_dev_ten) (function)
- [i2c_fake_dev_set](#i2c_fake_dev_set) (function)
- [I2cBus](#I2cBus) (struct)
- [i2c_bus_fake](#i2c_bus_fake) (function)
- [i2c_bus_custom](#i2c_bus_custom) (function)
- [i2c_fake_add](#i2c_fake_add) (function)
- [i2c_fake_get](#i2c_fake_get) (function)
- [i2c_fake_put](#i2c_fake_put) (function)
- [i2c_transfer](#i2c_transfer) (function)
- [i2c_write](#i2c_write) (function)
- [i2c_read](#i2c_read) (function)
- [i2c_write_read](#i2c_write_read) (function)
- [i2c_write_read10](#i2c_write_read10) (function)
- [i2c_read_regs](#i2c_read_regs) (function)
- [i2c_read_reg8](#i2c_read_reg8) (function)
- [i2c_read_reg16_be](#i2c_read_reg16_be) (function)
- [i2c_read_reg16_le](#i2c_read_reg16_le) (function)
- [i2c_write_reg8](#i2c_write_reg8) (function)
- [i2c_write_reg16_be](#i2c_write_reg16_be) (function)
- [i2c_write_regs](#i2c_write_regs) (function)
- [i2c_update_bits](#i2c_update_bits) (function)
- [i2c_read_a16](#i2c_read_a16) (function)
- [i2c_write_a16](#i2c_write_a16) (function)
- [i2c_scan](#i2c_scan) (function)
- [i2c_crc8_smbus](#i2c_crc8_smbus) (function)
- [i2c_smbus_pec_read](#i2c_smbus_pec_read) (function)
- [i2c_smbus_pec_write](#i2c_smbus_pec_write) (function)
- [i2c_rdwr_size](#i2c_rdwr_size) (function)
- [i2c_rdwr_image](#i2c_rdwr_image) (function)
- [i2c_rdwr_reads](#i2c_rdwr_reads) (function)
- [i2c_c_int](#i2c_c_int) (function)
- [i2c_linux_xfer](#i2c_linux_xfer) (function)
- [i2c_linux_open](#i2c_linux_open) (function)
- [i2c_linux_close](#i2c_linux_close) (function)

## I2C_RETRIES:

```fides
pub const I2C_RETRIES: i64 = 0x0701;
```

*Defined at line 49*

---

## I2C_TIMEOUT:

```fides
pub const I2C_TIMEOUT: i64 = 0x0702;
```

*Defined at line 50*

---

## I2C_SLAVE:

```fides
pub const I2C_SLAVE: i64 = 0x0703;
```

*Defined at line 51*

---

## I2C_TENBIT:

```fides
pub const I2C_TENBIT: i64 = 0x0704;
```

*Defined at line 52*

---

## I2C_FUNCS:

```fides
pub const I2C_FUNCS: i64 = 0x0705;
```

*Defined at line 53*

---

## I2C_SLAVE_FORCE:

```fides
pub const I2C_SLAVE_FORCE: i64 = 0x0706;
```

*Defined at line 54*

---

## I2C_RDWR:

```fides
pub const I2C_RDWR: i64 = 0x0707;
```

*Defined at line 55*

---

## I2C_PEC:

```fides
pub const I2C_PEC: i64 = 0x0708;
```

*Defined at line 56*

---

## I2C_SMBUS:

```fides
pub const I2C_SMBUS: i64 = 0x0720;
```

*Defined at line 57*

---

## I2C_RDWR_MAX_MSGS:

```fides
pub const I2C_RDWR_MAX_MSGS: i64 = 42;
```

*Defined at line 58*

---

## I2C_M_RD:

```fides
pub const I2C_M_RD: i64 = 0x0001;
```

*Defined at line 60*

---

## I2C_M_TEN:

```fides
pub const I2C_M_TEN: i64 = 0x0010;
```

*Defined at line 61*

---

## I2C_M_DMA_SAFE:

```fides
pub const I2C_M_DMA_SAFE: i64 = 0x0200;
```

*Defined at line 62*

---

## I2C_M_RECV_LEN:

```fides
pub const I2C_M_RECV_LEN: i64 = 0x0400;
```

*Defined at line 63*

---

## I2C_M_NO_RD_ACK:

```fides
pub const I2C_M_NO_RD_ACK: i64 = 0x0800;
```

*Defined at line 64*

---

## I2C_M_IGNORE_NAK:

```fides
pub const I2C_M_IGNORE_NAK: i64 = 0x1000;
```

*Defined at line 65*

---

## I2C_M_REV_DIR_ADDR:

```fides
pub const I2C_M_REV_DIR_ADDR: i64 = 0x2000;
```

*Defined at line 66*

---

## I2C_M_NOSTART:

```fides
pub const I2C_M_NOSTART: i64 = 0x4000;
```

*Defined at line 67*

---

## I2C_M_STOP:

```fides
pub const I2C_M_STOP: i64 = 0x8000;
```

*Defined at line 68*

---

## I2C_O_RDWR:

```fides
pub const I2C_O_RDWR: i64 = 2;
```

*Defined at line 70*

---

## I2C_BUS_FAKE:

```fides
pub const I2C_BUS_FAKE: i64 = 0;
```

*Defined at line 73*

---

## I2C_BUS_FN:

```fides
pub const I2C_BUS_FN: i64 = 1;
```

*Defined at line 74*

---

## I2cMsg

One message of a transaction. For a write `data` holds the bytes and
`len` is data.len(); for a read `len` bytes are read (data is empty).

```fides
pub struct I2cMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `flags` | `i64` |
| `len` | `i64` |
| `data` | `Vec<i64>` |

*Defined at line 80*

---

## i2c_msg_write

```fides
pub fn i2c_msg_write(addr: i64, data: Vec<i64>) -> I2cMsg {
```

**Parameters:**

- `addr`: `i64`
- `data`: `Vec<i64>`

**Returns:** `I2cMsg`

*Defined at line 87*

---

## i2c_msg_read

```fides
pub fn i2c_msg_read(addr: i64, n: i64) -> I2cMsg {
```

**Parameters:**

- `addr`: `i64`
- `n`: `i64`

**Returns:** `I2cMsg`

*Defined at line 93*

---

## i2c_msg_ten

The same message addressed with a 10-bit address.

```fides
pub fn i2c_msg_ten(m: I2cMsg) -> I2cMsg {
```

**Parameters:**

- `m`: `I2cMsg`

**Returns:** `I2cMsg`

*Defined at line 100*

---

## i2c_msg_is_read

```fides
pub fn i2c_msg_is_read(m: &I2cMsg) -> bool {
```

**Parameters:**

- `m`: `&I2cMsg`

**Returns:** `bool`

*Defined at line 106*

---

## i2c_addr7_valid

A 7-bit address a device may have (0x08..0x77; the rest are reserved
for general call, CBUS, HS-mode masters and 10-bit headers).

```fides
pub fn i2c_addr7_valid(addr: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `bool`

*Defined at line 112*

---

## i2c_addr10_valid

```fides
pub fn i2c_addr10_valid(addr: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `bool`

*Defined at line 116*

---

## i2c_addr_wire

The address bytes a master sends after START. 7-bit: one byte
addr<<1 | R/W. 10-bit write: 11110 A9 A8 0, then A7..A0. 10-bit read:
the full write header, then after a repeated start 11110 A9 A8 1.

```fides
pub fn i2c_addr_wire(addr: i64, ten_bit: bool, read: bool) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`
- `ten_bit`: `bool`
- `read`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 123*

---

## i2c_addr_unwire7

Decode the first address byte of a 7-bit transfer: [addr, read?1:0].

```fides
pub fn i2c_addr_unwire7(b: i64) -> Vec<i64> {
```

**Parameters:**

- `b`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 143*

---

## i2c_encode_request

The request a transport receives: [n, then per message addr, flags,
len, and for writes the len data bytes].

```fides
pub fn i2c_encode_request(msgs: &Vec<I2cMsg>) -> Vec<i64> {
```

**Parameters:**

- `msgs`: `&Vec<I2cMsg>`

**Returns:** `Vec<i64>`

*Defined at line 154*

---

## i2c_decode_request

Decode a request back into messages (a transport's view).

```fides
pub fn i2c_decode_request(req: &Vec<i64>) -> Vec<I2cMsg> {
```

**Parameters:**

- `req`: `&Vec<i64>`

**Returns:** `Vec<I2cMsg>`

*Defined at line 176*

---

## I2cFakeDev

A register-pointer device: the first `reg_width` bytes of a write set
the pointer (big-endian), further bytes are stored at the pointer and
advance it; a read returns bytes from the pointer, advancing it. If
`queue` is not empty a read takes its bytes from the queue first (a
command device such as the SHT3x answers that way).

```fides
pub struct I2cFakeDev {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `ten_bit` | `bool` |
| `reg_width` | `i64` |
| `regs` | `Vec<i64>` |
| `ptr` | `i64` |
| `queue` | `Vec<i64>` |
| `written` | `Vec<i64>` |

*Defined at line 213*

---

## i2c_fake_dev

```fides
pub fn i2c_fake_dev(addr: i64, nregs: i64) -> I2cFakeDev {
```

**Parameters:**

- `addr`: `i64`
- `nregs`: `i64`

**Returns:** `I2cFakeDev`

*Defined at line 223*

---

## i2c_fake_dev16

A device with a 16-bit register pointer (24Cxx EEPROM style).

```fides
pub fn i2c_fake_dev16(addr: i64, nregs: i64) -> I2cFakeDev {
```

**Parameters:**

- `addr`: `i64`
- `nregs`: `i64`

**Returns:** `I2cFakeDev`

*Defined at line 230*

---

## i2c_fake_dev_ten

```fides
pub fn i2c_fake_dev_ten(addr: i64, nregs: i64) -> I2cFakeDev {
```

**Parameters:**

- `addr`: `i64`
- `nregs`: `i64`

**Returns:** `I2cFakeDev`

*Defined at line 236*

---

## i2c_fake_dev_set

```fides
pub fn i2c_fake_dev_set(d: &mut I2cFakeDev, reg: i64, bytes: Vec<i64>) {
```

**Parameters:**

- `d`: `&mut I2cFakeDev`
- `reg`: `i64`
- `bytes`: `Vec<i64>`

*Defined at line 242*

---

## I2cBus

```fides
pub struct I2cBus {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `handle` | `i64` |
| `xfer` | `fn(i64, Vec<i64>) -> Vec<i64>` |
| `devs` | `Vec<I2cFakeDev>` |
| `log` | `Vec<str>` |

*Defined at line 253*

---

## i2c_bus_fake

```fides
pub fn i2c_bus_fake() -> I2cBus {
```

**Returns:** `I2cBus`

*Defined at line 268*

---

## i2c_bus_custom

A bus driven by your own transport (a bare-metal controller driver):
xfer(handle, i2c_encode_request(msgs)) returns [0, read bytes...] or
[negative error].

```fides
pub fn i2c_bus_custom(handle: i64, xfer: fn(i64, Vec<i64>) -> Vec<i64>) -> I2cBus {
```

**Parameters:**

- `handle`: `i64`
- `xfer`: `fn(i64`

**Returns:** `I2cBus`

*Defined at line 277*

---

## i2c_fake_add

```fides
pub fn i2c_fake_add(bus: &mut I2cBus, d: I2cFakeDev) {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `d`: `I2cFakeDev`

*Defined at line 283*

---

## i2c_fake_get

The fake device at an address (a copy), for checking what was written.

```fides
pub fn i2c_fake_get(bus: &I2cBus, addr: i64) -> I2cFakeDev {
```

**Parameters:**

- `bus`: `&I2cBus`
- `addr`: `i64`

**Returns:** `I2cFakeDev`

*Defined at line 300*

---

## i2c_fake_put

```fides
pub fn i2c_fake_put(bus: &mut I2cBus, d: I2cFakeDev) {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `d`: `I2cFakeDev`

*Defined at line 311*

---

## i2c_transfer

Run one combined transaction (START, messages joined by repeated
starts, STOP). The result holds the bytes of every read message, in
order.

```fides
pub fn i2c_transfer(bus: &mut I2cBus, msgs: Vec<I2cMsg>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `msgs`: `Vec<I2cMsg>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 404*

---

## i2c_write

```fides
pub fn i2c_write(bus: &mut I2cBus, addr: i64, data: Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `data`: `Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 433*

---

## i2c_read

```fides
pub fn i2c_read(bus: &mut I2cBus, addr: i64, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 439*

---

## i2c_write_read

Write `out`, repeated START, read n bytes: the usual register read.

```fides
pub fn i2c_write_read(bus: &mut I2cBus, addr: i64, out: Vec<i64>, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `out`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 446*

---

## i2c_write_read10

The same with a 10-bit address.

```fides
pub fn i2c_write_read10(bus: &mut I2cBus, addr: i64, out: Vec<i64>, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `out`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 454*

---

## i2c_read_regs

Read n registers starting at reg (auto-increment).

```fides
pub fn i2c_read_regs(bus: &mut I2cBus, addr: i64, reg: i64, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 468*

---

## i2c_read_reg8

Read one 8-bit register; -1 when the device does not answer.

```fides
pub fn i2c_read_reg8(bus: &mut I2cBus, addr: i64, reg: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 473*

---

## i2c_read_reg16_be

16-bit register, most significant byte first; -1 on error.

```fides
pub fn i2c_read_reg16_be(bus: &mut I2cBus, addr: i64, reg: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 485*

---

## i2c_read_reg16_le

16-bit register, least significant byte first; -1 on error.

```fides
pub fn i2c_read_reg16_le(bus: &mut I2cBus, addr: i64, reg: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 497*

---

## i2c_write_reg8

```fides
pub fn i2c_write_reg8(bus: &mut I2cBus, addr: i64, reg: i64, value: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 508*

---

## i2c_write_reg16_be

```fides
pub fn i2c_write_reg16_be(bus: &mut I2cBus, addr: i64, reg: i64, value: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 522*

---

## i2c_write_regs

```fides
pub fn i2c_write_regs(bus: &mut I2cBus, addr: i64, reg: i64, values: Vec<i64>) -> bool {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `values`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 537*

---

## i2c_update_bits

Read-modify-write: the bits in mask take the bits of value.

```fides
pub fn i2c_update_bits(bus: &mut I2cBus, addr: i64, reg: i64, mask: i64, value: i64) -> bool {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `mask`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 556*

---

## i2c_read_a16

Read n bytes from a device with a 16-bit register pointer.

```fides
pub fn i2c_read_a16(bus: &mut I2cBus, addr: i64, reg: i64, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 569*

---

## i2c_write_a16

```fides
pub fn i2c_write_a16(bus: &mut I2cBus, addr: i64, reg: i64, values: Vec<i64>) -> bool {
```

**Parameters:**

- `bus`: `&mut I2cBus`
- `addr`: `i64`
- `reg`: `i64`
- `values`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 576*

---

## i2c_scan

Probe every usable 7-bit address with a one-byte read, as i2cdetect -r
does; the addresses that answer, in order.

```fides
pub fn i2c_scan(bus: &mut I2cBus) -> Vec<i64> {
```

**Parameters:**

- `bus`: `&mut I2cBus`

**Returns:** `Vec<i64>`

*Defined at line 597*

---

## i2c_crc8_smbus

CRC-8 with polynomial 0x07, initial value 0 (SMBus packet error code).

```fides
pub fn i2c_crc8_smbus(bytes: &Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 616*

---

## i2c_smbus_pec_read

PEC of an SMBus "read byte/word/block" transaction: the CRC covers the
write address byte, the command, the read address byte and the data.

```fides
pub fn i2c_smbus_pec_read(addr: i64, cmd: i64, data: &Vec<i64>) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `cmd`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 637*

---

## i2c_smbus_pec_write

PEC of an SMBus write: address byte, command, data.

```fides
pub fn i2c_smbus_pec_write(addr: i64, cmd: i64, data: &Vec<i64>) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `cmd`: `i64`
- `data`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 651*

---

## i2c_rdwr_size

Bytes needed by i2c_rdwr_image for these messages.

```fides
pub fn i2c_rdwr_size(msgs: &Vec<I2cMsg>) -> i64 {
```

**Parameters:**

- `msgs`: `&Vec<I2cMsg>`

**Returns:** `i64`

*Defined at line 674*

---

## i2c_rdwr_image

The bytes of an I2C_RDWR argument placed at address `base` (LP64):
0: struct i2c_rdwr_ioctl_data { struct i2c_msg *msgs; __u32 nmsgs; }
16: struct i2c_msg { __u16 addr; __u16 flags; __u16 len; __u8 *buf; }[n]
(16 bytes each: addr@0 flags@2 len@4 buf@8)
then each message's buffer (write data, or room for the read).

```fides
pub fn i2c_rdwr_image(msgs: &Vec<I2cMsg>, base: i64) -> Vec<i64> {
```

**Parameters:**

- `msgs`: `&Vec<I2cMsg>`
- `base`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 689*

---

## i2c_rdwr_reads

After the ioctl: the read messages' bytes, taken from the image.

```fides
pub fn i2c_rdwr_reads(msgs: &Vec<I2cMsg>, img: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `msgs`: `&Vec<I2cMsg>`
- `img`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 716*

---

## i2c_c_int

A C int result widened to i64 (the declarations return 64 bits; the
functions return int).

```fides
pub fn i2c_c_int(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 737*

---

## i2c_linux_xfer

```fides
pub fn i2c_linux_xfer(fd: i64, req: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `fd`: `i64`
- `req`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 747*

---

## i2c_linux_open

```fides
pub fn i2c_linux_open(n: i64) -> Result<I2cBus, str> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Result<I2cBus, str>`

*Defined at line 790*

---

## i2c_linux_close

```fides
pub fn i2c_linux_close(bus: &mut I2cBus) {
```

**Parameters:**

- `bus`: `&mut I2cBus`

*Defined at line 806*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
