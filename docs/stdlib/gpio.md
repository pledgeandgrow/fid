[All modules](index.md)

# gpio -- Fides Documentation

## Contents

- [GPIO_IN:](#GPIO_IN:) (const)
- [GPIO_OUT:](#GPIO_OUT:) (const)
- [GPIO_PULL_NONE:](#GPIO_PULL_NONE:) (const)
- [GPIO_PULL_UP:](#GPIO_PULL_UP:) (const)
- [GPIO_PULL_DOWN:](#GPIO_PULL_DOWN:) (const)
- [GPIO_DRIVE_PUSH_PULL:](#GPIO_DRIVE_PUSH_PULL:) (const)
- [GPIO_DRIVE_OPEN_DRAIN:](#GPIO_DRIVE_OPEN_DRAIN:) (const)
- [GPIO_DRIVE_OPEN_SOURCE:](#GPIO_DRIVE_OPEN_SOURCE:) (const)
- [GPIO_EDGE_NONE:](#GPIO_EDGE_NONE:) (const)
- [GPIO_EDGE_RISING:](#GPIO_EDGE_RISING:) (const)
- [GPIO_EDGE_FALLING:](#GPIO_EDGE_FALLING:) (const)
- [GPIO_EDGE_BOTH:](#GPIO_EDGE_BOTH:) (const)
- [GPIO_FLOATING:](#GPIO_FLOATING:) (const)
- [GPIO_V2_LINE_FLAG_USED:](#GPIO_V2_LINE_FLAG_USED:) (const)
- [GPIO_V2_LINE_FLAG_ACTIVE_LOW:](#GPIO_V2_LINE_FLAG_ACTIVE_LOW:) (const)
- [GPIO_V2_LINE_FLAG_INPUT:](#GPIO_V2_LINE_FLAG_INPUT:) (const)
- [GPIO_V2_LINE_FLAG_OUTPUT:](#GPIO_V2_LINE_FLAG_OUTPUT:) (const)
- [GPIO_V2_LINE_FLAG_EDGE_RISING:](#GPIO_V2_LINE_FLAG_EDGE_RISING:) (const)
- [GPIO_V2_LINE_FLAG_EDGE_FALLING:](#GPIO_V2_LINE_FLAG_EDGE_FALLING:) (const)
- [GPIO_V2_LINE_FLAG_OPEN_DRAIN:](#GPIO_V2_LINE_FLAG_OPEN_DRAIN:) (const)
- [GPIO_V2_LINE_FLAG_OPEN_SOURCE:](#GPIO_V2_LINE_FLAG_OPEN_SOURCE:) (const)
- [GPIO_V2_LINE_FLAG_BIAS_PULL_UP:](#GPIO_V2_LINE_FLAG_BIAS_PULL_UP:) (const)
- [GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN:](#GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN:) (const)
- [GPIO_V2_LINE_FLAG_BIAS_DISABLED:](#GPIO_V2_LINE_FLAG_BIAS_DISABLED:) (const)
- [GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME:](#GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME:) (const)
- [GPIO_V2_LINE_ATTR_ID_FLAGS:](#GPIO_V2_LINE_ATTR_ID_FLAGS:) (const)
- [GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES:](#GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES:) (const)
- [GPIO_V2_LINE_ATTR_ID_DEBOUNCE:](#GPIO_V2_LINE_ATTR_ID_DEBOUNCE:) (const)
- [GPIO_V2_LINE_EVENT_RISING_EDGE:](#GPIO_V2_LINE_EVENT_RISING_EDGE:) (const)
- [GPIO_V2_LINE_EVENT_FALLING_EDGE:](#GPIO_V2_LINE_EVENT_FALLING_EDGE:) (const)
- [GPIO_V2_LINES_MAX:](#GPIO_V2_LINES_MAX:) (const)
- [GPIO_V2_LINE_NUM_ATTRS_MAX:](#GPIO_V2_LINE_NUM_ATTRS_MAX:) (const)
- [GPIO_V2_REQUEST_SIZE:](#GPIO_V2_REQUEST_SIZE:) (const)
- [GPIO_V2_CONFIG_SIZE:](#GPIO_V2_CONFIG_SIZE:) (const)
- [GPIO_V2_EVENT_SIZE:](#GPIO_V2_EVENT_SIZE:) (const)
- [gpio_ioc](#gpio_ioc) (function)
- [gpio_get_chipinfo_ioctl](#gpio_get_chipinfo_ioctl) (function)
- [gpio_v2_get_line_ioctl](#gpio_v2_get_line_ioctl) (function)
- [gpio_v2_line_set_config_ioctl](#gpio_v2_line_set_config_ioctl) (function)
- [gpio_v2_line_get_values_ioctl](#gpio_v2_line_get_values_ioctl) (function)
- [gpio_v2_line_set_values_ioctl](#gpio_v2_line_set_values_ioctl) (function)
- [GpioConfig](#GpioConfig) (struct)
- [gpio_input](#gpio_input) (function)
- [gpio_output](#gpio_output) (function)
- [gpio_edge_input](#gpio_edge_input) (function)
- [gpio_v2_flags](#gpio_v2_flags) (function)
- [GpioDebounce](#GpioDebounce) (struct)
- [gpio_debounce_new](#gpio_debounce_new) (function)
- [gpio_debounce_feed](#gpio_debounce_feed) (function)
- [gpio_debounce_deadline](#gpio_debounce_deadline) (function)
- [gpio_edge_matches](#gpio_edge_matches) (function)
- [GpioEvent](#GpioEvent) (struct)
- [GpioLine](#GpioLine) (struct)
- [GpioChip](#GpioChip) (struct)
- [gpio_fake_chip](#gpio_fake_chip) (function)
- [gpio_fake_physical](#gpio_fake_physical) (function)
- [gpio_request](#gpio_request) (function)
- [gpio_release](#gpio_release) (function)
- [gpio_read](#gpio_read) (function)
- [gpio_write](#gpio_write) (function)
- [gpio_toggle](#gpio_toggle) (function)
- [gpio_fake_drive](#gpio_fake_drive) (function)
- [gpio_tick](#gpio_tick) (function)
- [gpio_take_events](#gpio_take_events) (function)
- [gpio_v2_config_into](#gpio_v2_config_into) (function)
- [gpio_v2_request_image](#gpio_v2_request_image) (function)
- [gpio_v2_request_fd](#gpio_v2_request_fd) (function)
- [gpio_v2_values_image](#gpio_v2_values_image) (function)
- [gpio_v2_decode_events](#gpio_v2_decode_events) (function)
- [gpio_event_name](#gpio_event_name) (function)
- [gpio_c_int](#gpio_c_int) (function)
- [GpioLinuxLines](#GpioLinuxLines) (struct)
- [gpio_linux_request](#gpio_linux_request) (function)
- [gpio_linux_get](#gpio_linux_get) (function)
- [gpio_linux_set](#gpio_linux_set) (function)
- [gpio_linux_read_events](#gpio_linux_read_events) (function)
- [gpio_linux_release](#gpio_linux_release) (function)
- [GpioSysfsWrite](#GpioSysfsWrite) (struct)
- [gpio_sysfs_pin_dir](#gpio_sysfs_pin_dir) (function)
- [gpio_sysfs_setup](#gpio_sysfs_setup) (function)
- [gpio_sysfs_apply](#gpio_sysfs_apply) (function)
- [gpio_sysfs_parse_value](#gpio_sysfs_parse_value) (function)
- [gpio_sysfs_read](#gpio_sysfs_read) (function)
- [gpio_sysfs_write](#gpio_sysfs_write) (function)
- [gpio_sysfs_unexport](#gpio_sysfs_unexport) (function)

## GPIO_IN:

```fides
pub const GPIO_IN: i64 = 0;
```

*Defined at line 42*

---

## GPIO_OUT:

```fides
pub const GPIO_OUT: i64 = 1;
```

*Defined at line 43*

---

## GPIO_PULL_NONE:

```fides
pub const GPIO_PULL_NONE: i64 = 0;
```

*Defined at line 44*

---

## GPIO_PULL_UP:

```fides
pub const GPIO_PULL_UP: i64 = 1;
```

*Defined at line 45*

---

## GPIO_PULL_DOWN:

```fides
pub const GPIO_PULL_DOWN: i64 = 2;
```

*Defined at line 46*

---

## GPIO_DRIVE_PUSH_PULL:

```fides
pub const GPIO_DRIVE_PUSH_PULL: i64 = 0;
```

*Defined at line 47*

---

## GPIO_DRIVE_OPEN_DRAIN:

```fides
pub const GPIO_DRIVE_OPEN_DRAIN: i64 = 1;
```

*Defined at line 48*

---

## GPIO_DRIVE_OPEN_SOURCE:

```fides
pub const GPIO_DRIVE_OPEN_SOURCE: i64 = 2;
```

*Defined at line 49*

---

## GPIO_EDGE_NONE:

```fides
pub const GPIO_EDGE_NONE: i64 = 0;
```

*Defined at line 50*

---

## GPIO_EDGE_RISING:

```fides
pub const GPIO_EDGE_RISING: i64 = 1;
```

*Defined at line 51*

---

## GPIO_EDGE_FALLING:

```fides
pub const GPIO_EDGE_FALLING: i64 = 2;
```

*Defined at line 52*

---

## GPIO_EDGE_BOTH:

```fides
pub const GPIO_EDGE_BOTH: i64 = 3;
```

*Defined at line 53*

---

## GPIO_FLOATING:

```fides
pub const GPIO_FLOATING: i64 = 0 - 1;
```

*Defined at line 54*

---

## GPIO_V2_LINE_FLAG_USED:

```fides
pub const GPIO_V2_LINE_FLAG_USED: i64 = 0x1;
```

*Defined at line 57*

---

## GPIO_V2_LINE_FLAG_ACTIVE_LOW:

```fides
pub const GPIO_V2_LINE_FLAG_ACTIVE_LOW: i64 = 0x2;
```

*Defined at line 58*

---

## GPIO_V2_LINE_FLAG_INPUT:

```fides
pub const GPIO_V2_LINE_FLAG_INPUT: i64 = 0x4;
```

*Defined at line 59*

---

## GPIO_V2_LINE_FLAG_OUTPUT:

```fides
pub const GPIO_V2_LINE_FLAG_OUTPUT: i64 = 0x8;
```

*Defined at line 60*

---

## GPIO_V2_LINE_FLAG_EDGE_RISING:

```fides
pub const GPIO_V2_LINE_FLAG_EDGE_RISING: i64 = 0x10;
```

*Defined at line 61*

---

## GPIO_V2_LINE_FLAG_EDGE_FALLING:

```fides
pub const GPIO_V2_LINE_FLAG_EDGE_FALLING: i64 = 0x20;
```

*Defined at line 62*

---

## GPIO_V2_LINE_FLAG_OPEN_DRAIN:

```fides
pub const GPIO_V2_LINE_FLAG_OPEN_DRAIN: i64 = 0x40;
```

*Defined at line 63*

---

## GPIO_V2_LINE_FLAG_OPEN_SOURCE:

```fides
pub const GPIO_V2_LINE_FLAG_OPEN_SOURCE: i64 = 0x80;
```

*Defined at line 64*

---

## GPIO_V2_LINE_FLAG_BIAS_PULL_UP:

```fides
pub const GPIO_V2_LINE_FLAG_BIAS_PULL_UP: i64 = 0x100;
```

*Defined at line 65*

---

## GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN:

```fides
pub const GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN: i64 = 0x200;
```

*Defined at line 66*

---

## GPIO_V2_LINE_FLAG_BIAS_DISABLED:

```fides
pub const GPIO_V2_LINE_FLAG_BIAS_DISABLED: i64 = 0x400;
```

*Defined at line 67*

---

## GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME:

```fides
pub const GPIO_V2_LINE_FLAG_EVENT_CLOCK_REALTIME: i64 = 0x800;
```

*Defined at line 68*

---

## GPIO_V2_LINE_ATTR_ID_FLAGS:

```fides
pub const GPIO_V2_LINE_ATTR_ID_FLAGS: i64 = 1;
```

*Defined at line 69*

---

## GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES:

```fides
pub const GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES: i64 = 2;
```

*Defined at line 70*

---

## GPIO_V2_LINE_ATTR_ID_DEBOUNCE:

```fides
pub const GPIO_V2_LINE_ATTR_ID_DEBOUNCE: i64 = 3;
```

*Defined at line 71*

---

## GPIO_V2_LINE_EVENT_RISING_EDGE:

```fides
pub const GPIO_V2_LINE_EVENT_RISING_EDGE: i64 = 1;
```

*Defined at line 72*

---

## GPIO_V2_LINE_EVENT_FALLING_EDGE:

```fides
pub const GPIO_V2_LINE_EVENT_FALLING_EDGE: i64 = 2;
```

*Defined at line 73*

---

## GPIO_V2_LINES_MAX:

```fides
pub const GPIO_V2_LINES_MAX: i64 = 64;
```

*Defined at line 74*

---

## GPIO_V2_LINE_NUM_ATTRS_MAX:

```fides
pub const GPIO_V2_LINE_NUM_ATTRS_MAX: i64 = 10;
```

*Defined at line 75*

---

## GPIO_V2_REQUEST_SIZE:

```fides
pub const GPIO_V2_REQUEST_SIZE: i64 = 592;
```

*Defined at line 76*

---

## GPIO_V2_CONFIG_SIZE:

```fides
pub const GPIO_V2_CONFIG_SIZE: i64 = 272;
```

*Defined at line 77*

---

## GPIO_V2_EVENT_SIZE:

```fides
pub const GPIO_V2_EVENT_SIZE: i64 = 48;
```

*Defined at line 78*

---

## gpio_ioc

_IOC(dir, type, nr, size) (asm-generic encoding).

```fides
pub fn gpio_ioc(dir: i64, ty: i64, nr: i64, size: i64) -> i64 {
```

**Parameters:**

- `dir`: `i64`
- `ty`: `i64`
- `nr`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 81*

---

## gpio_get_chipinfo_ioctl

```fides
pub fn gpio_get_chipinfo_ioctl() -> i64 {
```

**Returns:** `i64`

*Defined at line 85*

---

## gpio_v2_get_line_ioctl

```fides
pub fn gpio_v2_get_line_ioctl() -> i64 {
```

**Returns:** `i64`

*Defined at line 89*

---

## gpio_v2_line_set_config_ioctl

```fides
pub fn gpio_v2_line_set_config_ioctl() -> i64 {
```

**Returns:** `i64`

*Defined at line 93*

---

## gpio_v2_line_get_values_ioctl

```fides
pub fn gpio_v2_line_get_values_ioctl() -> i64 {
```

**Returns:** `i64`

*Defined at line 97*

---

## gpio_v2_line_set_values_ioctl

```fides
pub fn gpio_v2_line_set_values_ioctl() -> i64 {
```

**Returns:** `i64`

*Defined at line 101*

---

## GpioConfig

```fides
pub struct GpioConfig {
```

**Fields:**

| Name | Type |
|------|------|
| `dir` | `i64` |
| `pull` | `i64` |
| `active_low` | `bool` |
| `drive` | `i64` |
| `edge` | `i64` |
| `debounce_ns` | `i64` |
| `initial` | `i64` |

*Defined at line 107*

---

## gpio_input

```fides
pub fn gpio_input(pull: i64) -> GpioConfig {
```

**Parameters:**

- `pull`: `i64`

**Returns:** `GpioConfig`

*Defined at line 117*

---

## gpio_output

```fides
pub fn gpio_output(initial: i64) -> GpioConfig {
```

**Parameters:**

- `initial`: `i64`

**Returns:** `GpioConfig`

*Defined at line 123*

---

## gpio_edge_input

An input that reports edges, debounced over debounce_ns.

```fides
pub fn gpio_edge_input(pull: i64, edge: i64, debounce_ns: i64) -> GpioConfig {
```

**Parameters:**

- `pull`: `i64`
- `edge`: `i64`
- `debounce_ns`: `i64`

**Returns:** `GpioConfig`

*Defined at line 130*

---

## gpio_v2_flags

The v2 flags word for a configuration.

```fides
pub fn gpio_v2_flags(c: &GpioConfig) -> i64 {
```

**Parameters:**

- `c`: `&GpioConfig`

**Returns:** `i64`

*Defined at line 137*

---

## GpioDebounce

Debounce state of one input. `stable` is the accepted level (-1 before
the first sample), `candidate` a level seen since `since_ns`.

```fides
pub struct GpioDebounce {
```

**Fields:**

| Name | Type |
|------|------|
| `stable` | `i64` |
| `candidate` | `i64` |
| `since_ns` | `i64` |
| `period_ns` | `i64` |

*Defined at line 171*

---

## gpio_debounce_new

```fides
pub fn gpio_debounce_new(period_ns: i64) -> GpioDebounce {
```

**Parameters:**

- `period_ns`: `i64`

**Returns:** `GpioDebounce`

*Defined at line 178*

---

## gpio_debounce_feed

Feed a raw level sampled at t_ns. Returns 0 (no change), 1 (the
stable level rose) or 2 (it fell). The first sample sets the level
without an edge. A change counts once the level has held for the
period; a bounce back resets the wait. Call it again with the same
level later (poll / timer) to let a pending change through.

```fides
pub fn gpio_debounce_feed(d: &mut GpioDebounce, level: i64, t_ns: i64) -> i64 {
```

**Parameters:**

- `d`: `&mut GpioDebounce`
- `level`: `i64`
- `t_ns`: `i64`

**Returns:** `i64`

*Defined at line 189*

---

## gpio_debounce_deadline

When a pending change will be accepted (or -1 if none is pending).

```fides
pub fn gpio_debounce_deadline(d: &GpioDebounce) -> i64 {
```

**Parameters:**

- `d`: `&GpioDebounce`

**Returns:** `i64`

*Defined at line 217*

---

## gpio_edge_matches

Whether an edge (1 rising / 2 falling) passes an edge filter.

```fides
pub fn gpio_edge_matches(edge_cfg: i64, ev: i64) -> bool {
```

**Parameters:**

- `edge_cfg`: `i64`
- `ev`: `i64`

**Returns:** `bool`

*Defined at line 225*

---

## GpioEvent

```fides
pub struct GpioEvent {
```

**Fields:**

| Name | Type |
|------|------|
| `timestamp_ns` | `i64` |
| `id` | `i64` |
| `offset` | `i64` |
| `seqno` | `i64` |
| `line_seqno` | `i64` |

*Defined at line 237*

---

## GpioLine

```fides
pub struct GpioLine {
```

**Fields:**

| Name | Type |
|------|------|
| `offset` | `i64` |
| `cfg` | `GpioConfig` |
| `requested` | `bool` |
| `out_value` | `i64` |
| `ext_level` | `i64` |
| `deb` | `GpioDebounce` |
| `line_seqno` | `i64` |

*Defined at line 245*

---

## GpioChip

```fides
pub struct GpioChip {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `lines` | `Vec<GpioLine>` |
| `events` | `Vec<GpioEvent>` |
| `seqno` | `i64` |

*Defined at line 255*

---

## gpio_fake_chip

```fides
pub fn gpio_fake_chip(name: str, nlines: i64) -> GpioChip {
```

**Parameters:**

- `name`: `str`
- `nlines`: `i64`

**Returns:** `GpioChip`

*Defined at line 262*

---

## gpio_fake_physical

The physical level on the pin: the output when one drives it (an
open-drain output only drives low, open-source only high), else the
outside driver, else the pull (a floating pin with no pull reads 0).

```fides
pub fn gpio_fake_physical(l: &GpioLine) -> i64 {
```

**Parameters:**

- `l`: `&GpioLine`

**Returns:** `i64`

*Defined at line 279*

---

## gpio_request

Request (configure) a line. Fails on a line already requested.

```fides
pub fn gpio_request(chip: &mut GpioChip, offset: i64, cfg: GpioConfig, t_ns: i64) -> Result<i64, str> {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `offset`: `i64`
- `cfg`: `GpioConfig`
- `t_ns`: `i64`

**Returns:** `Result<i64, str>`

*Defined at line 310*

---

## gpio_release

```fides
pub fn gpio_release(chip: &mut GpioChip, offset: i64) {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `offset`: `i64`

*Defined at line 334*

---

## gpio_read

Read a line's logical value (active-low applied).

```fides
pub fn gpio_read(chip: &GpioChip, offset: i64) -> i64 {
```

**Parameters:**

- `chip`: `&GpioChip`
- `offset`: `i64`

**Returns:** `i64`

*Defined at line 344*

---

## gpio_write

Set an output's logical value. False if the line is not an output.

```fides
pub fn gpio_write(chip: &mut GpioChip, offset: i64, value: i64) -> bool {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `offset`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 350*

---

## gpio_toggle

```fides
pub fn gpio_toggle(chip: &mut GpioChip, offset: i64) -> bool {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `offset`: `i64`

**Returns:** `bool`

*Defined at line 364*

---

## gpio_fake_drive

Drive a pin from outside (a button, another chip) at time t_ns; level
GPIO_FLOATING lets go of it.

```fides
pub fn gpio_fake_drive(chip: &mut GpioChip, offset: i64, level: i64, t_ns: i64) {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `offset`: `i64`
- `level`: `i64`
- `t_ns`: `i64`

*Defined at line 391*

---

## gpio_tick

Let time pass: every edge input is sampled at t_ns (a pending change
that has now held for its debounce period becomes an event).

```fides
pub fn gpio_tick(chip: &mut GpioChip, t_ns: i64) {
```

**Parameters:**

- `chip`: `&mut GpioChip`
- `t_ns`: `i64`

*Defined at line 400*

---

## gpio_take_events

Take the queued events, oldest first.

```fides
pub fn gpio_take_events(chip: &mut GpioChip) -> Vec<GpioEvent> {
```

**Parameters:**

- `chip`: `&mut GpioChip`

**Returns:** `Vec<GpioEvent>`

*Defined at line 409*

---

## gpio_v2_config_into

struct gpio_v2_line_config (272 bytes) at `off` in out: flags, then
attributes for per-line output values and debounce. outputs_mask /
output_bits index the request's lines (bit i = offsets[i]).

```fides
pub fn gpio_v2_config_into(out: &mut Vec<i64>, off: i64, flags: i64, out_mask: i64, out_bits: i64, debounce_us: i64, deb_mask: i64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `off`: `i64`
- `flags`: `i64`
- `out_mask`: `i64`
- `out_bits`: `i64`
- `debounce_us`: `i64`
- `deb_mask`: `i64`

*Defined at line 438*

---

## gpio_v2_request_image

struct gpio_v2_line_request (592 bytes): offsets[64]@0, consumer[32]@256,
config@288, num_lines@560, event_buffer_size@564, fd@588. All lines
share `cfg`; outputs start at cfg.initial.

```fides
pub fn gpio_v2_request_image(offsets: &Vec<i64>, consumer: str, cfg: &GpioConfig) -> Vec<i64> {
```

**Parameters:**

- `offsets`: `&Vec<i64>`
- `consumer`: `str`
- `cfg`: `&GpioConfig`

**Returns:** `Vec<i64>`

*Defined at line 461*

---

## gpio_v2_request_fd

The line-request fd the kernel wrote back into a request image.

```fides
pub fn gpio_v2_request_fd(img: &Vec<i64>) -> i64 {
```

**Parameters:**

- `img`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 497*

---

## gpio_v2_values_image

struct gpio_v2_line_values { u64 bits; u64 mask; }

```fides
pub fn gpio_v2_values_image(bits: i64, mask: i64) -> Vec<i64> {
```

**Parameters:**

- `bits`: `i64`
- `mask`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 506*

---

## gpio_v2_decode_events

Decode the records of a read() on a line-request fd (48 bytes each).

```fides
pub fn gpio_v2_decode_events(bytes: &Vec<i64>) -> Vec<GpioEvent> {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `Vec<GpioEvent>`

*Defined at line 514*

---

## gpio_event_name

```fides
pub fn gpio_event_name(id: i64) -> str {
```

**Parameters:**

- `id`: `i64`

**Returns:** `str`

*Defined at line 526*

---

## gpio_c_int

```fides
pub fn gpio_c_int(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 536*

---

## GpioLinuxLines

Lines requested from a Linux gpiochip (the kernel's line-request fd).

```fides
pub struct GpioLinuxLines {
```

**Fields:**

| Name | Type |
|------|------|
| `fd` | `i64` |
| `offsets` | `Vec<i64>` |

*Defined at line 545*

---

## gpio_linux_request

```fides
pub fn gpio_linux_request(chip: i64, offsets: Vec<i64>, consumer: str, cfg: GpioConfig) -> Result<GpioLinuxLines, str> {
```

**Parameters:**

- `chip`: `i64`
- `offsets`: `Vec<i64>`
- `consumer`: `str`
- `cfg`: `GpioConfig`

**Returns:** `Result<GpioLinuxLines, str>`

*Defined at line 577*

---

## gpio_linux_get

```fides
pub fn gpio_linux_get(lines: &GpioLinuxLines) -> i64 {
```

**Parameters:**

- `lines`: `&GpioLinuxLines`

**Returns:** `i64`

*Defined at line 602*

---

## gpio_linux_set

```fides
pub fn gpio_linux_set(lines: &GpioLinuxLines, bits: i64, mask: i64) -> bool {
```

**Parameters:**

- `lines`: `&GpioLinuxLines`
- `bits`: `i64`
- `mask`: `i64`

**Returns:** `bool`

*Defined at line 612*

---

## gpio_linux_read_events

```fides
pub fn gpio_linux_read_events(lines: &GpioLinuxLines) -> Vec<GpioEvent> {
```

**Parameters:**

- `lines`: `&GpioLinuxLines`

**Returns:** `Vec<GpioEvent>`

*Defined at line 619*

---

## gpio_linux_release

```fides
pub fn gpio_linux_release(lines: &mut GpioLinuxLines) {
```

**Parameters:**

- `lines`: `&mut GpioLinuxLines`

*Defined at line 637*

---

## GpioSysfsWrite

One write of the sysfs sequence: the file and what goes in it.

```fides
pub struct GpioSysfsWrite {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `value` | `str` |

*Defined at line 650*

---

## gpio_sysfs_pin_dir

```fides
pub fn gpio_sysfs_pin_dir(root: str, pin: i64) -> str {
```

**Parameters:**

- `root`: `str`
- `pin`: `i64`

**Returns:** `str`

*Defined at line 655*

---

## gpio_sysfs_setup

The writes that export and configure a pin: export, active_low,
direction ("in", or "high"/"low" to set an output glitch-free), edge.

```fides
pub fn gpio_sysfs_setup(root: str, pin: i64, cfg: &GpioConfig) -> Vec<GpioSysfsWrite> {
```

**Parameters:**

- `root`: `str`
- `pin`: `i64`
- `cfg`: `&GpioConfig`

**Returns:** `Vec<GpioSysfsWrite>`

*Defined at line 661*

---

## gpio_sysfs_apply

Perform writes; the number that failed.

```fides
pub fn gpio_sysfs_apply(writes: &Vec<GpioSysfsWrite>) -> i64 {
```

**Parameters:**

- `writes`: `&Vec<GpioSysfsWrite>`

**Returns:** `i64`

*Defined at line 710*

---

## gpio_sysfs_parse_value

Parse a sysfs value file ("0\n" / "1\n"); -1 when unreadable.

```fides
pub fn gpio_sysfs_parse_value(text: str) -> i64 {
```

**Parameters:**

- `text`: `str`

**Returns:** `i64`

*Defined at line 724*

---

## gpio_sysfs_read

```fides
pub fn gpio_sysfs_read(root: str, pin: i64) -> i64 {
```

**Parameters:**

- `root`: `str`
- `pin`: `i64`

**Returns:** `i64`

*Defined at line 738*

---

## gpio_sysfs_write

```fides
pub fn gpio_sysfs_write(root: str, pin: i64, value: i64) -> bool {
```

**Parameters:**

- `root`: `str`
- `pin`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 749*

---

## gpio_sysfs_unexport

```fides
pub fn gpio_sysfs_unexport(root: str, pin: i64) -> bool {
```

**Parameters:**

- `root`: `str`
- `pin`: `i64`

**Returns:** `bool`

*Defined at line 753*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
