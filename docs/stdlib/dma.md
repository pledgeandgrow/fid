[All modules](index.md)

# dma -- Fides Documentation

## Contents

- [dma_align_up](#dma_align_up) (function)
- [dma_align_down](#dma_align_down) (function)
- [dma_is_aligned](#dma_is_aligned) (function)
- [dma_line_start](#dma_line_start) (function)
- [dma_line_count](#dma_line_count) (function)
- [dma_needs_bounce](#dma_needs_bounce) (function)
- [DMA_TO_DEVICE:](#DMA_TO_DEVICE:) (const)
- [DMA_FROM_DEVICE:](#DMA_FROM_DEVICE:) (const)
- [DMA_BIDIRECTIONAL:](#DMA_BIDIRECTIONAL:) (const)
- [dma_cache_plan](#dma_cache_plan) (function)
- [SgEntry](#SgEntry) (struct)
- [dma_sg_split](#dma_sg_split) (function)
- [dma_sg_split_all](#dma_sg_split_all) (function)
- [dma_sg_coalesce](#dma_sg_coalesce) (function)
- [dma_sg_total](#dma_sg_total) (function)
- [dma_sg_to_str](#dma_sg_to_str) (function)
- [DMAD_EOP:](#DMAD_EOP:) (const)
- [DMAD_SOP:](#DMAD_SOP:) (const)
- [DMAD_IRQ:](#DMAD_IRQ:) (const)
- [DMAD_WRAP:](#DMAD_WRAP:) (const)
- [DMAD_OWN:](#DMAD_OWN:) (const)
- [DMAD_MAX_LEN:](#DMAD_MAX_LEN:) (const)
- [DMAS_OK:](#DMAS_OK:) (const)
- [DMAS_BUS_ERROR:](#DMAS_BUS_ERROR:) (const)
- [DMAS_TRUNCATED:](#DMAS_TRUNCATED:) (const)
- [DmaDesc](#DmaDesc) (struct)
- [dmadesc_len](#dmadesc_len) (function)
- [dmadesc_status](#dmadesc_status) (function)
- [dmadesc_owned](#dmadesc_owned) (function)
- [dmadesc_bytes](#dmadesc_bytes) (function)
- [dmadesc_from_bytes](#dmadesc_from_bytes) (function)
- [DmaRing](#DmaRing) (struct)
- [dmaring_new](#dmaring_new) (function)
- [dmaring_free](#dmaring_free) (function)
- [dmaring_submit](#dmaring_submit) (function)
- [DmaDone](#DmaDone) (struct)
- [dmaring_reap](#dmaring_reap) (function)
- [DmaMem](#DmaMem) (struct)
- [dmamem_new](#dmamem_new) (function)
- [dmamem_in_range](#dmamem_in_range) (function)
- [DmaSimDevice](#DmaSimDevice) (struct)
- [dmasim_new](#dmasim_new) (function)
- [dmasim_run](#dmasim_run) (function)
- [DMACH_IDLE:](#DMACH_IDLE:) (const)
- [DMACH_CONFIGURED:](#DMACH_CONFIGURED:) (const)
- [DMACH_RUNNING:](#DMACH_RUNNING:) (const)
- [DMACH_PAUSED:](#DMACH_PAUSED:) (const)
- [DMACH_DONE:](#DMACH_DONE:) (const)
- [DMACH_ERROR:](#DMACH_ERROR:) (const)
- [DMAEV_CONFIGURE:](#DMAEV_CONFIGURE:) (const)
- [DMAEV_START:](#DMAEV_START:) (const)
- [DMAEV_PAUSE:](#DMAEV_PAUSE:) (const)
- [DMAEV_RESUME:](#DMAEV_RESUME:) (const)
- [DMAEV_COMPLETE:](#DMAEV_COMPLETE:) (const)
- [DMAEV_FAULT:](#DMAEV_FAULT:) (const)
- [DMAEV_ABORT:](#DMAEV_ABORT:) (const)
- [DMAEV_RESET:](#DMAEV_RESET:) (const)
- [dmach_state_name](#dmach_state_name) (function)
- [dmach_event_name](#dmach_event_name) (function)
- [dmach_next](#dmach_next) (function)
- [DmaChannel](#DmaChannel) (struct)
- [dmach_new](#dmach_new) (function)
- [dmach_event](#dmach_event) (function)

## dma_align_up

```fides
pub fn dma_align_up(x: i64, a: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 31*

---

## dma_align_down

```fides
pub fn dma_align_down(x: i64, a: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 35*

---

## dma_is_aligned

```fides
pub fn dma_is_aligned(x: i64, a: i64) -> bool {
```

**Parameters:**

- `x`: `i64`
- `a`: `i64`

**Returns:** `bool`

*Defined at line 39*

---

## dma_line_start

First cache line address and number of lines [addr, addr+len) touches.

```fides
pub fn dma_line_start(addr: i64, line: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `line`: `i64`

**Returns:** `i64`

*Defined at line 44*

---

## dma_line_count

```fides
pub fn dma_line_count(addr: i64, len: i64, line: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`
- `len`: `i64`
- `line`: `i64`

**Returns:** `i64`

*Defined at line 48*

---

## dma_needs_bounce

A device-to-memory buffer that does not start and end on line
boundaries shares its first or last line with other data.

```fides
pub fn dma_needs_bounce(addr: i64, len: i64, line: i64) -> bool {
```

**Parameters:**

- `addr`: `i64`
- `len`: `i64`
- `line`: `i64`

**Returns:** `bool`

*Defined at line 57*

---

## DMA_TO_DEVICE:

```fides
pub const DMA_TO_DEVICE: i64 = 1;
```

*Defined at line 61*

---

## DMA_FROM_DEVICE:

```fides
pub const DMA_FROM_DEVICE: i64 = 2;
```

*Defined at line 62*

---

## DMA_BIDIRECTIONAL:

```fides
pub const DMA_BIDIRECTIONAL: i64 = 3;
```

*Defined at line 63*

---

## dma_cache_plan

The cache operations around a transfer, as text lines:
"clean 0x...+N lines" before, "invalidate ..." after.

```fides
pub fn dma_cache_plan(addr: i64, len: i64, line: i64, dir: i64) -> Vec<str> {
```

**Parameters:**

- `addr`: `i64`
- `len`: `i64`
- `line`: `i64`
- `dir`: `i64`

**Returns:** `Vec<str>`

*Defined at line 67*

---

## SgEntry

```fides
pub struct SgEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `len` | `i64` |

*Defined at line 90*

---

## dma_sg_split

Split [addr, addr+len) into entries no longer than `max_seg` and not
crossing a multiple of `boundary` (0: no boundary).

```fides
pub fn dma_sg_split(addr: i64, len: i64, max_seg: i64, boundary: i64) -> Vec<SgEntry> {
```

**Parameters:**

- `addr`: `i64`
- `len`: `i64`
- `max_seg`: `i64`
- `boundary`: `i64`

**Returns:** `Vec<SgEntry>`

*Defined at line 97*

---

## dma_sg_split_all

Split every entry of a list (several buffers) with the same limits.

```fides
pub fn dma_sg_split_all(list: Vec<SgEntry>, max_seg: i64, boundary: i64) -> Vec<SgEntry> {
```

**Parameters:**

- `list`: `Vec<SgEntry>`
- `max_seg`: `i64`
- `boundary`: `i64`

**Returns:** `Vec<SgEntry>`

*Defined at line 122*

---

## dma_sg_coalesce

Merge entries where one ends where the next starts, while the merged
entry stays within `max_seg` and does not cross `boundary`.

```fides
pub fn dma_sg_coalesce(list: Vec<SgEntry>, max_seg: i64, boundary: i64) -> Vec<SgEntry> {
```

**Parameters:**

- `list`: `Vec<SgEntry>`
- `max_seg`: `i64`
- `boundary`: `i64`

**Returns:** `Vec<SgEntry>`

*Defined at line 139*

---

## dma_sg_total

```fides
pub fn dma_sg_total(list: Vec<SgEntry>) -> i64 {
```

**Parameters:**

- `list`: `Vec<SgEntry>`

**Returns:** `i64`

*Defined at line 164*

---

## dma_sg_to_str

```fides
pub fn dma_sg_to_str(list: Vec<SgEntry>) -> str {
```

**Parameters:**

- `list`: `Vec<SgEntry>`

**Returns:** `str`

*Defined at line 174*

---

## DMAD_EOP:

```fides
pub const DMAD_EOP: i64 = 0x01000000;
```

*Defined at line 188*

---

## DMAD_SOP:

```fides
pub const DMAD_SOP: i64 = 0x02000000;
```

*Defined at line 189*

---

## DMAD_IRQ:

```fides
pub const DMAD_IRQ: i64 = 0x04000000;
```

*Defined at line 190*

---

## DMAD_WRAP:

```fides
pub const DMAD_WRAP: i64 = 0x08000000;
```

*Defined at line 191*

---

## DMAD_OWN:

```fides
pub const DMAD_OWN: i64 = 0x80000000;
```

*Defined at line 192*

---

## DMAD_MAX_LEN:

```fides
pub const DMAD_MAX_LEN: i64 = 0xFFFF;
```

*Defined at line 193*

---

## DMAS_OK:

Status codes in the control word after completion.

```fides
pub const DMAS_OK: i64 = 0;
```

*Defined at line 196*

---

## DMAS_BUS_ERROR:

```fides
pub const DMAS_BUS_ERROR: i64 = 1;
```

*Defined at line 197*

---

## DMAS_TRUNCATED:

```fides
pub const DMAS_TRUNCATED: i64 = 2;
```

*Defined at line 198*

---

## DmaDesc

```fides
pub struct DmaDesc {
```

**Fields:**

| Name | Type |
|------|------|
| `addr` | `i64` |
| `ctrl` | `i64` |
| `aux` | `i64` |

*Defined at line 200*

---

## dmadesc_len

```fides
pub fn dmadesc_len(d: DmaDesc) -> i64 {
```

**Parameters:**

- `d`: `DmaDesc`

**Returns:** `i64`

*Defined at line 206*

---

## dmadesc_status

```fides
pub fn dmadesc_status(d: DmaDesc) -> i64 {
```

**Parameters:**

- `d`: `DmaDesc`

**Returns:** `i64`

*Defined at line 210*

---

## dmadesc_owned

```fides
pub fn dmadesc_owned(d: DmaDesc) -> bool {
```

**Parameters:**

- `d`: `DmaDesc`

**Returns:** `bool`

*Defined at line 214*

---

## dmadesc_bytes

The 16 bytes of a descriptor in memory (little-endian).

```fides
pub fn dmadesc_bytes(d: DmaDesc) -> Vec<i64> {
```

**Parameters:**

- `d`: `DmaDesc`

**Returns:** `Vec<i64>`

*Defined at line 219*

---

## dmadesc_from_bytes

Parse 16 bytes back into a descriptor.

```fides
pub fn dmadesc_from_bytes(b: Vec<i64>, at: i64) -> DmaDesc {
```

**Parameters:**

- `b`: `Vec<i64>`
- `at`: `i64`

**Returns:** `DmaDesc`

*Defined at line 240*

---

## DmaRing

A ring of `size` descriptors (a power of two). `head` is the next slot
the driver fills, `tail` the next one it reaps; `used` slots are in
flight.

```fides
pub struct DmaRing {
```

**Fields:**

| Name | Type |
|------|------|
| `size` | `i64` |
| `descs` | `Vec<DmaDesc>` |
| `head` | `i64` |
| `tail` | `i64` |
| `used` | `i64` |
| `doorbells` | `i64` |

*Defined at line 269*

---

## dmaring_new

```fides
pub fn dmaring_new(size: i64) -> DmaRing {
```

**Parameters:**

- `size`: `i64`

**Returns:** `DmaRing`

*Defined at line 278*

---

## dmaring_free

```fides
pub fn dmaring_free(r: &DmaRing) -> i64 {
```

**Parameters:**

- `r`: `&DmaRing`

**Returns:** `i64`

*Defined at line 299*

---

## dmaring_submit

Queue one transfer made of the entries of `sg` (each <= DMAD_MAX_LEN).
The first descriptor gets SOP, the last EOP (and IRQ when `irq`); OWN
is set on the first descriptor last, so the device never sees a half
written chain. Returns the first slot, -1 when the ring is too full.

```fides
pub fn dmaring_submit(r: &mut DmaRing, sg: Vec<SgEntry>, irq: bool, tag: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut DmaRing`
- `sg`: `Vec<SgEntry>`
- `irq`: `bool`
- `tag`: `i64`

**Returns:** `i64`

*Defined at line 307*

---

## DmaDone

A completed transfer.

```fides
pub struct DmaDone {
```

**Fields:**

| Name | Type |
|------|------|
| `first` | `i64` |
| `count` | `i64` |
| `bytes` | `i64` |
| `status` | `i64` |
| `tag` | `i64` |

*Defined at line 349*

---

## dmaring_reap

Reap the oldest transfer if the device has given all its descriptors
back (OWN clear from SOP to EOP). `count` 0: nothing completed yet.

```fides
pub fn dmaring_reap(r: &mut DmaRing) -> DmaDone {
```

**Parameters:**

- `r`: `&mut DmaRing`

**Returns:** `DmaDone`

*Defined at line 359*

---

## DmaMem

Byte-addressed memory for the simulated engine.

```fides
pub struct DmaMem {
```

**Fields:**

| Name | Type |
|------|------|
| `base` | `i64` |
| `bytes` | `Vec<i64>` |

*Defined at line 396*

---

## dmamem_new

```fides
pub fn dmamem_new(base: i64, size: i64) -> DmaMem {
```

**Parameters:**

- `base`: `i64`
- `size`: `i64`

**Returns:** `DmaMem`

*Defined at line 401*

---

## dmamem_in_range

```fides
pub fn dmamem_in_range(m: &DmaMem, addr: i64, len: i64) -> bool {
```

**Parameters:**

- `m`: `&DmaMem`
- `addr`: `i64`
- `len`: `i64`

**Returns:** `bool`

*Defined at line 407*

---

## DmaSimDevice

A memory-to-memory engine: each transfer copies its buffers, in order,
into the destination stream starting at `dst`. `pos` is the device's
next slot, `irqs` counts completion interrupts raised.

```fides
pub struct DmaSimDevice {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `i64` |
| `dst` | `i64` |
| `irqs` | `i64` |

*Defined at line 414*

---

## dmasim_new

```fides
pub fn dmasim_new(dst: i64) -> DmaSimDevice {
```

**Parameters:**

- `dst`: `i64`

**Returns:** `DmaSimDevice`

*Defined at line 420*

---

## dmasim_run

Process up to `budget` descriptors the device owns, as hardware: copy,
write status into the control word, clear OWN. A buffer outside
memory completes with DMAS_BUS_ERROR. Returns descriptors processed.

```fides
pub fn dmasim_run(dev: &mut DmaSimDevice, r: &mut DmaRing, m: &mut DmaMem, budget: i64) -> i64 {
```

**Parameters:**

- `dev`: `&mut DmaSimDevice`
- `r`: `&mut DmaRing`
- `m`: `&mut DmaMem`
- `budget`: `i64`

**Returns:** `i64`

*Defined at line 429*

---

## DMACH_IDLE:

```fides
pub const DMACH_IDLE: i64 = 0;
```

*Defined at line 465*

---

## DMACH_CONFIGURED:

```fides
pub const DMACH_CONFIGURED: i64 = 1;
```

*Defined at line 466*

---

## DMACH_RUNNING:

```fides
pub const DMACH_RUNNING: i64 = 2;
```

*Defined at line 467*

---

## DMACH_PAUSED:

```fides
pub const DMACH_PAUSED: i64 = 3;
```

*Defined at line 468*

---

## DMACH_DONE:

```fides
pub const DMACH_DONE: i64 = 4;
```

*Defined at line 469*

---

## DMACH_ERROR:

```fides
pub const DMACH_ERROR: i64 = 5;
```

*Defined at line 470*

---

## DMAEV_CONFIGURE:

```fides
pub const DMAEV_CONFIGURE: i64 = 0;
```

*Defined at line 472*

---

## DMAEV_START:

```fides
pub const DMAEV_START: i64 = 1;
```

*Defined at line 473*

---

## DMAEV_PAUSE:

```fides
pub const DMAEV_PAUSE: i64 = 2;
```

*Defined at line 474*

---

## DMAEV_RESUME:

```fides
pub const DMAEV_RESUME: i64 = 3;
```

*Defined at line 475*

---

## DMAEV_COMPLETE:

```fides
pub const DMAEV_COMPLETE: i64 = 4;
```

*Defined at line 476*

---

## DMAEV_FAULT:

```fides
pub const DMAEV_FAULT: i64 = 5;
```

*Defined at line 477*

---

## DMAEV_ABORT:

```fides
pub const DMAEV_ABORT: i64 = 6;
```

*Defined at line 478*

---

## DMAEV_RESET:

```fides
pub const DMAEV_RESET: i64 = 7;
```

*Defined at line 479*

---

## dmach_state_name

```fides
pub fn dmach_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 481*

---

## dmach_event_name

```fides
pub fn dmach_event_name(e: i64) -> str {
```

**Parameters:**

- `e`: `i64`

**Returns:** `str`

*Defined at line 489*

---

## dmach_next

The next state, or -1 when the engine does not allow the event there.
configure: idle/done -> configured; start: configured -> running;
pause/resume between running and paused; complete: running -> done;
fault: running/paused -> error; abort: configured/running/paused ->
idle; reset: any -> idle.

```fides
pub fn dmach_next(s: i64, e: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `e`: `i64`

**Returns:** `i64`

*Defined at line 502*

---

## DmaChannel

```fides
pub struct DmaChannel {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `history` | `Vec<str>` |
| `rejected` | `i64` |

*Defined at line 530*

---

## dmach_new

```fides
pub fn dmach_new() -> DmaChannel {
```

**Returns:** `DmaChannel`

*Defined at line 536*

---

## dmach_event

Apply an event; false (state unchanged) when not allowed.

```fides
pub fn dmach_event(c: &mut DmaChannel, e: i64) -> bool {
```

**Parameters:**

- `c`: `&mut DmaChannel`
- `e`: `i64`

**Returns:** `bool`

*Defined at line 543*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
