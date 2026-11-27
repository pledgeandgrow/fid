[All modules](index.md)

# spi -- Fides Documentation

## Contents

- [SPI_CPHA:](#SPI_CPHA:) (const)
- [SPI_CPOL:](#SPI_CPOL:) (const)
- [SPI_MODE_0:](#SPI_MODE_0:) (const)
- [SPI_MODE_1:](#SPI_MODE_1:) (const)
- [SPI_MODE_2:](#SPI_MODE_2:) (const)
- [SPI_MODE_3:](#SPI_MODE_3:) (const)
- [SPI_CS_HIGH:](#SPI_CS_HIGH:) (const)
- [SPI_LSB_FIRST:](#SPI_LSB_FIRST:) (const)
- [SPI_3WIRE:](#SPI_3WIRE:) (const)
- [SPI_LOOP:](#SPI_LOOP:) (const)
- [SPI_NO_CS:](#SPI_NO_CS:) (const)
- [SPI_READY:](#SPI_READY:) (const)
- [SPI_TX_DUAL:](#SPI_TX_DUAL:) (const)
- [SPI_TX_QUAD:](#SPI_TX_QUAD:) (const)
- [SPI_RX_DUAL:](#SPI_RX_DUAL:) (const)
- [SPI_RX_QUAD:](#SPI_RX_QUAD:) (const)
- [SPI_IOC_MAGIC:](#SPI_IOC_MAGIC:) (const)
- [SPI_IOC_TRANSFER_SIZE:](#SPI_IOC_TRANSFER_SIZE:) (const)
- [SPI_BUS_FAKE:](#SPI_BUS_FAKE:) (const)
- [SPI_BUS_FN:](#SPI_BUS_FN:) (const)
- [spi_ioc](#spi_ioc) (function)
- [spi_ioc_message](#spi_ioc_message) (function)
- [spi_ioc_wr_mode](#spi_ioc_wr_mode) (function)
- [spi_ioc_rd_mode](#spi_ioc_rd_mode) (function)
- [spi_ioc_wr_lsb_first](#spi_ioc_wr_lsb_first) (function)
- [spi_ioc_wr_bits_per_word](#spi_ioc_wr_bits_per_word) (function)
- [spi_ioc_wr_max_speed_hz](#spi_ioc_wr_max_speed_hz) (function)
- [spi_ioc_wr_mode32](#spi_ioc_wr_mode32) (function)
- [SpiConfig](#SpiConfig) (struct)
- [spi_config](#spi_config) (function)
- [spi_cpol](#spi_cpol) (function)
- [spi_cpha](#spi_cpha) (function)
- [spi_mode_name](#spi_mode_name) (function)
- [spi_word_bytes](#spi_word_bytes) (function)
- [spi_pack_words](#spi_pack_words) (function)
- [spi_unpack_words](#spi_unpack_words) (function)
- [spi_reverse_bits](#spi_reverse_bits) (function)
- [spi_transfer_ns](#spi_transfer_ns) (function)
- [SpiStep](#SpiStep) (struct)
- [spi_bitbang_steps](#spi_bitbang_steps) (function)
- [spi_slave_decode](#spi_slave_decode) (function)
- [SpiPins](#SpiPins) (struct)
- [spi_bitbang_xfer](#spi_bitbang_xfer) (function)
- [SpiXfer](#SpiXfer) (struct)
- [spi_xfer](#spi_xfer) (function)
- [spi_encode_request](#spi_encode_request) (function)
- [spi_decode_request](#spi_decode_request) (function)
- [SpiFakeDev](#SpiFakeDev) (struct)
- [spi_fake_regdev](#spi_fake_regdev) (function)
- [spi_fake_loopback](#spi_fake_loopback) (function)
- [spi_fake_set](#spi_fake_set) (function)
- [SpiBus](#SpiBus) (struct)
- [spi_bus_fake](#spi_bus_fake) (function)
- [spi_bus_custom](#spi_bus_custom) (function)
- [spi_fake_add](#spi_fake_add) (function)
- [spi_select](#spi_select) (function)
- [spi_fake_get](#spi_fake_get) (function)
- [spi_message](#spi_message) (function)
- [spi_transfer](#spi_transfer) (function)
- [spi_transfer_words](#spi_transfer_words) (function)
- [spi_write_then_read](#spi_write_then_read) (function)
- [spi_read_regs](#spi_read_regs) (function)
- [spi_read_reg8](#spi_read_reg8) (function)
- [spi_write_regs](#spi_write_regs) (function)
- [spi_image_size](#spi_image_size) (function)
- [spi_ioc_image](#spi_ioc_image) (function)
- [spi_ioc_rx](#spi_ioc_rx) (function)
- [spi_c_int](#spi_c_int) (function)
- [spi_linux_xfer](#spi_linux_xfer) (function)
- [spi_linux_open](#spi_linux_open) (function)
- [spi_linux_close](#spi_linux_close) (function)

## SPI_CPHA:

```fides
pub const SPI_CPHA: i64 = 0x01;
```

*Defined at line 42*

---

## SPI_CPOL:

```fides
pub const SPI_CPOL: i64 = 0x02;
```

*Defined at line 43*

---

## SPI_MODE_0:

```fides
pub const SPI_MODE_0: i64 = 0;
```

*Defined at line 44*

---

## SPI_MODE_1:

```fides
pub const SPI_MODE_1: i64 = 1;
```

*Defined at line 45*

---

## SPI_MODE_2:

```fides
pub const SPI_MODE_2: i64 = 2;
```

*Defined at line 46*

---

## SPI_MODE_3:

```fides
pub const SPI_MODE_3: i64 = 3;
```

*Defined at line 47*

---

## SPI_CS_HIGH:

```fides
pub const SPI_CS_HIGH: i64 = 0x04;
```

*Defined at line 48*

---

## SPI_LSB_FIRST:

```fides
pub const SPI_LSB_FIRST: i64 = 0x08;
```

*Defined at line 49*

---

## SPI_3WIRE:

```fides
pub const SPI_3WIRE: i64 = 0x10;
```

*Defined at line 50*

---

## SPI_LOOP:

```fides
pub const SPI_LOOP: i64 = 0x20;
```

*Defined at line 51*

---

## SPI_NO_CS:

```fides
pub const SPI_NO_CS: i64 = 0x40;
```

*Defined at line 52*

---

## SPI_READY:

```fides
pub const SPI_READY: i64 = 0x80;
```

*Defined at line 53*

---

## SPI_TX_DUAL:

```fides
pub const SPI_TX_DUAL: i64 = 0x100;
```

*Defined at line 54*

---

## SPI_TX_QUAD:

```fides
pub const SPI_TX_QUAD: i64 = 0x200;
```

*Defined at line 55*

---

## SPI_RX_DUAL:

```fides
pub const SPI_RX_DUAL: i64 = 0x400;
```

*Defined at line 56*

---

## SPI_RX_QUAD:

```fides
pub const SPI_RX_QUAD: i64 = 0x800;
```

*Defined at line 57*

---

## SPI_IOC_MAGIC:

```fides
pub const SPI_IOC_MAGIC: i64 = 0x6B;
```

*Defined at line 59*

---

## SPI_IOC_TRANSFER_SIZE:

```fides
pub const SPI_IOC_TRANSFER_SIZE: i64 = 32;
```

*Defined at line 60*

---

## SPI_BUS_FAKE:

```fides
pub const SPI_BUS_FAKE: i64 = 0;
```

*Defined at line 61*

---

## SPI_BUS_FN:

```fides
pub const SPI_BUS_FN: i64 = 1;
```

*Defined at line 62*

---

## spi_ioc

_IOC(dir, type, nr, size): dir<<30 | size<<16 | type<<8 | nr, with
dir 1 = write (user to kernel), 2 = read. (x86, ARM, RISC-V; not
the MIPS/PowerPC/SPARC encodings.)

```fides
pub fn spi_ioc(dir: i64, ty: i64, nr: i64, size: i64) -> i64 {
```

**Parameters:**

- `dir`: `i64`
- `ty`: `i64`
- `nr`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 69*

---

## spi_ioc_message

```fides
pub fn spi_ioc_message(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## spi_ioc_wr_mode

```fides
pub fn spi_ioc_wr_mode() -> i64 {
```

**Returns:** `i64`

*Defined at line 77*

---

## spi_ioc_rd_mode

```fides
pub fn spi_ioc_rd_mode() -> i64 {
```

**Returns:** `i64`

*Defined at line 81*

---

## spi_ioc_wr_lsb_first

```fides
pub fn spi_ioc_wr_lsb_first() -> i64 {
```

**Returns:** `i64`

*Defined at line 85*

---

## spi_ioc_wr_bits_per_word

```fides
pub fn spi_ioc_wr_bits_per_word() -> i64 {
```

**Returns:** `i64`

*Defined at line 89*

---

## spi_ioc_wr_max_speed_hz

```fides
pub fn spi_ioc_wr_max_speed_hz() -> i64 {
```

**Returns:** `i64`

*Defined at line 93*

---

## spi_ioc_wr_mode32

```fides
pub fn spi_ioc_wr_mode32() -> i64 {
```

**Returns:** `i64`

*Defined at line 97*

---

## SpiConfig

```fides
pub struct SpiConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `mode` | `i64` |
| `bits_per_word` | `i64` |
| `speed_hz` | `i64` |

*Defined at line 103*

---

## spi_config

```fides
pub fn spi_config(mode: i64, bits_per_word: i64, speed_hz: i64) -> SpiConfig {
```

**Parameters:**

- `mode`: `i64`
- `bits_per_word`: `i64`
- `speed_hz`: `i64`

**Returns:** `SpiConfig`

*Defined at line 109*

---

## spi_cpol

```fides
pub fn spi_cpol(mode: i64) -> i64 {
```

**Parameters:**

- `mode`: `i64`

**Returns:** `i64`

*Defined at line 115*

---

## spi_cpha

```fides
pub fn spi_cpha(mode: i64) -> i64 {
```

**Parameters:**

- `mode`: `i64`

**Returns:** `i64`

*Defined at line 119*

---

## spi_mode_name

```fides
pub fn spi_mode_name(mode: i64) -> str {
```

**Parameters:**

- `mode`: `i64`

**Returns:** `str`

*Defined at line 123*

---

## spi_word_bytes

Bytes one word takes in a spidev buffer.

```fides
pub fn spi_word_bytes(bits: i64) -> i64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `i64`

*Defined at line 128*

---

## spi_pack_words

Words to buffer bytes (each word masked to `bits`, little-endian).

```fides
pub fn spi_pack_words(words: &Vec<i64>, bits: i64) -> Vec<i64> {
```

**Parameters:**

- `words`: `&Vec<i64>`
- `bits`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 139*

---

## spi_unpack_words

```fides
pub fn spi_unpack_words(bytes: &Vec<i64>, bits: i64) -> Vec<i64> {
```

**Parameters:**

- `bytes`: `&Vec<i64>`
- `bits`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 156*

---

## spi_reverse_bits

Reverse the low `bits` bits of w (LSB-first transmission).

```fides
pub fn spi_reverse_bits(w: i64, bits: i64) -> i64 {
```

**Parameters:**

- `w`: `i64`
- `bits`: `i64`

**Returns:** `i64`

*Defined at line 175*

---

## spi_transfer_ns

Time on the wire for n words, in nanoseconds (clock only).

```fides
pub fn spi_transfer_ns(nwords: i64, bits: i64, speed_hz: i64) -> i64 {
```

**Parameters:**

- `nwords`: `i64`
- `bits`: `i64`
- `speed_hz`: `i64`

**Returns:** `i64`

*Defined at line 188*

---

## SpiStep

One step of a bit-banged transfer: drive SCK and MOSI, then, when
`sample` is true, read MISO (the receiving edge just happened).

```fides
pub struct SpiStep {
```

**Fields:**

| Name | Type |
|------|------|
| `sck` | `i64` |
| `mosi` | `i64` |
| `sample` | `bool` |

*Defined at line 199*

---

## spi_bitbang_steps

The pin steps that clock one word in `mode` (bits, MSB first unless
lsb_first). The clock idles at CPOL. CPHA=0: data is set up before the
leading edge and sampled on it, changed on the trailing edge. CPHA=1:
data changes on the leading edge, is sampled on the trailing edge.

```fides
pub fn spi_bitbang_steps(mode: i64, bits: i64, lsb_first: bool, word: i64) -> Vec<SpiStep> {
```

**Parameters:**

- `mode`: `i64`
- `bits`: `i64`
- `lsb_first`: `bool`
- `word`: `i64`

**Returns:** `Vec<SpiStep>`

*Defined at line 209*

---

## spi_slave_decode

A slave watching SCK/MOSI: it samples MOSI on the edge its mode
samples on and assembles words. Feeds on the steps of any master.

```fides
pub fn spi_slave_decode(mode: i64, bits: i64, lsb_first: bool, steps: &Vec<SpiStep>) -> Vec<i64> {
```

**Parameters:**

- `mode`: `i64`
- `bits`: `i64`
- `lsb_first`: `bool`
- `steps`: `&Vec<SpiStep>`

**Returns:** `Vec<i64>`

*Defined at line 254*

---

## SpiPins

GPIO pins for bit-banging (bare metal): each call drives or reads one
line; delay_ns waits half a clock period.

```fides
pub struct SpiPins {
```

**Fields:**

| Name | Type |
|------|------|
| `set_sck` | `fn(i64) -> i64` |
| `set_mosi` | `fn(i64) -> i64` |
| `get_miso` | `fn(i64) -> i64` |
| `set_cs` | `fn(i64) -> i64` |
| `delay_ns` | `fn(i64) -> i64` |
| `half_period_ns` | `i64` |

*Defined at line 289*

---

## spi_bitbang_xfer

Clock the words out over the pins; the words read from MISO.

```fides
pub fn spi_bitbang_xfer(pins: &SpiPins, cfg: &SpiConfig, words: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pins`: `&SpiPins`
- `cfg`: `&SpiConfig`
- `words`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 299*

---

## SpiXfer

One segment of a message: tx bytes go out while as many come in.
cs_change deselects the chip after this segment.

```fides
pub struct SpiXfer {
```

**Fields:**

| Name | Type |
|------|------|
| `tx` | `Vec<i64>` |
| `speed_hz` | `i64` |
| `bits_per_word` | `i64` |
| `delay_us` | `i64` |
| `cs_change` | `bool` |

*Defined at line 350*

---

## spi_xfer

```fides
pub fn spi_xfer(tx: Vec<i64>) -> SpiXfer {
```

**Parameters:**

- `tx`: `Vec<i64>`

**Returns:** `SpiXfer`

*Defined at line 358*

---

## spi_encode_request

The request a transport receives: [mode, cs, n, then per segment len,
speed_hz, bits_per_word, delay_us, cs_change, tx bytes...].

```fides
pub fn spi_encode_request(cfg: &SpiConfig, cs: i64, xs: &Vec<SpiXfer>) -> Vec<i64> {
```

**Parameters:**

- `cfg`: `&SpiConfig`
- `cs`: `i64`
- `xs`: `&Vec<SpiXfer>`

**Returns:** `Vec<i64>`

*Defined at line 366*

---

## spi_decode_request

```fides
pub fn spi_decode_request(req: &Vec<i64>) -> Vec<SpiXfer> {
```

**Parameters:**

- `req`: `&Vec<i64>`

**Returns:** `Vec<SpiXfer>`

*Defined at line 393*

---

## SpiFakeDev

A fake device on one chip select. kind 0: register device (the first
byte is the address; read when (byte & read_bit) != 0 under
read_bit_set, address = byte & addr_mask, auto-increment). kind 1:
loopback (MISO echoes MOSI).

```fides
pub struct SpiFakeDev {
```

**Fields:**

| Name | Type |
|------|------|
| `cs` | `i64` |
| `kind` | `i64` |
| `regs` | `Vec<i64>` |
| `read_bit` | `i64` |
| `addr_mask` | `i64` |
| `written` | `Vec<i64>` |

*Defined at line 422*

---

## spi_fake_regdev

```fides
pub fn spi_fake_regdev(cs: i64, nregs: i64) -> SpiFakeDev {
```

**Parameters:**

- `cs`: `i64`
- `nregs`: `i64`

**Returns:** `SpiFakeDev`

*Defined at line 431*

---

## spi_fake_loopback

```fides
pub fn spi_fake_loopback(cs: i64) -> SpiFakeDev {
```

**Parameters:**

- `cs`: `i64`

**Returns:** `SpiFakeDev`

*Defined at line 437*

---

## spi_fake_set

```fides
pub fn spi_fake_set(d: &mut SpiFakeDev, reg: i64, bytes: Vec<i64>) {
```

**Parameters:**

- `d`: `&mut SpiFakeDev`
- `reg`: `i64`
- `bytes`: `Vec<i64>`

*Defined at line 443*

---

## SpiBus

```fides
pub struct SpiBus {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `handle` | `i64` |
| `cs` | `i64` |
| `cfg` | `SpiConfig` |
| `xfer` | `fn(i64, Vec<i64>) -> Vec<i64>` |
| `devs` | `Vec<SpiFakeDev>` |
| `selects` | `i64` |

*Defined at line 451*

---

## spi_bus_fake

```fides
pub fn spi_bus_fake(cfg: SpiConfig) -> SpiBus {
```

**Parameters:**

- `cfg`: `SpiConfig`

**Returns:** `SpiBus`

*Defined at line 467*

---

## spi_bus_custom

```fides
pub fn spi_bus_custom(handle: i64, cfg: SpiConfig, xfer: fn(i64, Vec<i64>) -> Vec<i64>) -> SpiBus {
```

**Parameters:**

- `handle`: `i64`
- `cfg`: `SpiConfig`
- `xfer`: `fn(i64`

**Returns:** `SpiBus`

*Defined at line 473*

---

## spi_fake_add

```fides
pub fn spi_fake_add(bus: &mut SpiBus, d: SpiFakeDev) {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `d`: `SpiFakeDev`

*Defined at line 479*

---

## spi_select

```fides
pub fn spi_select(bus: &mut SpiBus, cs: i64) {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `cs`: `i64`

*Defined at line 483*

---

## spi_fake_get

```fides
pub fn spi_fake_get(bus: &SpiBus, cs: i64) -> SpiFakeDev {
```

**Parameters:**

- `bus`: `&SpiBus`
- `cs`: `i64`

**Returns:** `SpiFakeDev`

*Defined at line 487*

---

## spi_message

Run a message (segments under one chip select unless cs_change); the
bytes clocked in, all segments concatenated.

```fides
pub fn spi_message(bus: &mut SpiBus, xs: Vec<SpiXfer>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `xs`: `Vec<SpiXfer>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 577*

---

## spi_transfer

Full duplex: send tx, get as many bytes back.

```fides
pub fn spi_transfer(bus: &mut SpiBus, tx: Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `tx`: `Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 599*

---

## spi_transfer_words

Full duplex in words of the bus's bits_per_word.

```fides
pub fn spi_transfer_words(bus: &mut SpiBus, words: Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `words`: `Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 606*

---

## spi_write_then_read

Send then receive under one chip select (half duplex in two segments).

```fides
pub fn spi_write_then_read(bus: &mut SpiBus, tx: Vec<i64>, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `tx`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 619*

---

## spi_read_regs

Read n registers from a register device (address | 0x80, then dummies).

```fides
pub fn spi_read_regs(bus: &mut SpiBus, reg: i64, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `reg`: `i64`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 641*

---

## spi_read_reg8

```fides
pub fn spi_read_reg8(bus: &mut SpiBus, reg: i64) -> i64 {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 647*

---

## spi_write_regs

Write registers (address with the read bit clear, then the values).

```fides
pub fn spi_write_regs(bus: &mut SpiBus, reg: i64, values: Vec<i64>) -> bool {
```

**Parameters:**

- `bus`: `&mut SpiBus`
- `reg`: `i64`
- `values`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 659*

---

## spi_image_size

```fides
pub fn spi_image_size(xs: &Vec<SpiXfer>) -> i64 {
```

**Parameters:**

- `xs`: `&Vec<SpiXfer>`

**Returns:** `i64`

*Defined at line 687*

---

## spi_ioc_image

struct spi_ioc_transfer[n] at `base`, then each segment's tx buffer and
rx buffer. Fields: tx_buf@0 (u64), rx_buf@8 (u64), len@16 (u32),
speed_hz@20 (u32), delay_usecs@24 (u16), bits_per_word@26,
cs_change@27, tx_nbits@28, rx_nbits@29, word_delay_usecs@30, pad@31.

```fides
pub fn spi_ioc_image(xs: &Vec<SpiXfer>, base: i64) -> Vec<i64> {
```

**Parameters:**

- `xs`: `&Vec<SpiXfer>`
- `base`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 701*

---

## spi_ioc_rx

The rx bytes of every segment, from an image after the ioctl.

```fides
pub fn spi_ioc_rx(xs: &Vec<SpiXfer>, img: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `xs`: `&Vec<SpiXfer>`
- `img`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 730*

---

## spi_c_int

```fides
pub fn spi_c_int(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 747*

---

## spi_linux_xfer

```fides
pub fn spi_linux_xfer(fd: i64, req: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `fd`: `i64`
- `req`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 774*

---

## spi_linux_open

```fides
pub fn spi_linux_open(busno: i64, cs: i64, cfg: SpiConfig) -> Result<SpiBus, str> {
```

**Parameters:**

- `busno`: `i64`
- `cs`: `i64`
- `cfg`: `SpiConfig`

**Returns:** `Result<SpiBus, str>`

*Defined at line 817*

---

## spi_linux_close

```fides
pub fn spi_linux_close(bus: &mut SpiBus) {
```

**Parameters:**

- `bus`: `&mut SpiBus`

*Defined at line 860*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
