[All modules](index.md)

# pcap -- Fides Documentation

## Contents

- [PCAP_LINKTYPE_NULL:](#PCAP_LINKTYPE_NULL:) (const)
- [PCAP_LINKTYPE_ETHERNET:](#PCAP_LINKTYPE_ETHERNET:) (const)
- [PCAP_LINKTYPE_RAW:](#PCAP_LINKTYPE_RAW:) (const)
- [PCAP_LINKTYPE_LINUX_SLL:](#PCAP_LINKTYPE_LINUX_SLL:) (const)
- [PCAP_LINKTYPE_IEEE802_11:](#PCAP_LINKTYPE_IEEE802_11:) (const)
- [PCAPNG_SHB:](#PCAPNG_SHB:) (const)
- [PCAPNG_IDB:](#PCAPNG_IDB:) (const)
- [PCAPNG_SPB:](#PCAPNG_SPB:) (const)
- [PCAPNG_EPB:](#PCAPNG_EPB:) (const)
- [PcapPacket](#PcapPacket) (struct)
- [PcapFile](#PcapFile) (struct)
- [PcapngIface](#PcapngIface) (struct)
- [PcapngFile](#PcapngFile) (struct)
- [pcap_new](#pcap_new) (function)
- [pcap_add](#pcap_add) (function)
- [pcap_add_truncated](#pcap_add_truncated) (function)
- [pcap_read](#pcap_read) (function)
- [pcap_write](#pcap_write) (function)
- [pcapng_units_per_sec](#pcapng_units_per_sec) (function)
- [pcapng_ts_split](#pcapng_ts_split) (function)
- [pcapng_ts_join](#pcapng_ts_join) (function)
- [pcapng_new](#pcapng_new) (function)
- [pcapng_iface](#pcapng_iface) (function)
- [pcapng_add_iface](#pcapng_add_iface) (function)
- [pcapng_add](#pcapng_add) (function)
- [pcapng_read](#pcapng_read) (function)
- [pcapng_write](#pcapng_write) (function)
- [pcap_to_pcapng](#pcap_to_pcapng) (function)
- [pcap_is_pcapng](#pcap_is_pcapng) (function)

## PCAP_LINKTYPE_NULL:

Link-layer header types (www.tcpdump.org/linktypes.html).

```fides
pub const PCAP_LINKTYPE_NULL: i64 = 0;
```

*Defined at line 32*

---

## PCAP_LINKTYPE_ETHERNET:

```fides
pub const PCAP_LINKTYPE_ETHERNET: i64 = 1;
```

*Defined at line 33*

---

## PCAP_LINKTYPE_RAW:

```fides
pub const PCAP_LINKTYPE_RAW: i64 = 101;
```

*Defined at line 34*

---

## PCAP_LINKTYPE_LINUX_SLL:

```fides
pub const PCAP_LINKTYPE_LINUX_SLL: i64 = 113;
```

*Defined at line 35*

---

## PCAP_LINKTYPE_IEEE802_11:

```fides
pub const PCAP_LINKTYPE_IEEE802_11: i64 = 105;
```

*Defined at line 36*

---

## PCAPNG_SHB:

pcapng block types.

```fides
pub const PCAPNG_SHB: i64 = 0x0A0D0D0A;
```

*Defined at line 39*

---

## PCAPNG_IDB:

```fides
pub const PCAPNG_IDB: i64 = 1;
```

*Defined at line 40*

---

## PCAPNG_SPB:

```fides
pub const PCAPNG_SPB: i64 = 3;
```

*Defined at line 41*

---

## PCAPNG_EPB:

```fides
pub const PCAPNG_EPB: i64 = 6;
```

*Defined at line 42*

---

## PcapPacket

One captured packet. `incl_len` is data.len(); `orig_len` the length on
the wire. `iface` is the pcapng interface index (0 for classic pcap).

```fides
pub struct PcapPacket {
```

**Fields:**

| Name | Type |
|------|------|
| `ts_sec` | `i64` |
| `ts_nsec` | `i64` |
| `orig_len` | `i64` |
| `data` | `Vec<i64>` |
| `iface` | `i64` |
| `comment` | `str` |
| `flags` | `i64,  // epb_flags, -1 when absent` |

*Defined at line 46*

---

## PcapFile

A classic pcap file. `nanos`: timestamps are written/read in ns.

```fides
pub struct PcapFile {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `big_endian` | `bool` |
| `nanos` | `bool` |
| `major` | `i64` |
| `minor` | `i64` |
| `thiszone` | `i64` |
| `sigfigs` | `i64` |
| `snaplen` | `i64` |
| `linktype` | `i64` |
| `packets` | `Vec<PcapPacket>` |

*Defined at line 57*

---

## PcapngIface

A pcapng interface (IDB). `tsresol` is the raw if_tsresol byte.

```fides
pub struct PcapngIface {
```

**Fields:**

| Name | Type |
|------|------|
| `linktype` | `i64` |
| `snaplen` | `i64` |
| `name` | `str` |
| `description` | `str` |
| `tsresol` | `i64` |

*Defined at line 72*

---

## PcapngFile

A pcapng file: every section's interfaces in order (a packet's `iface`
indexes this list, already offset by the section it came from).

```fides
pub struct PcapngFile {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `big_endian` | `bool` |
| `major` | `i64` |
| `minor` | `i64` |
| `sections` | `i64` |
| `hardware` | `str` |
| `os` | `str` |
| `app` | `str` |
| `interfaces` | `Vec<PcapngIface>` |
| `packets` | `Vec<PcapPacket>` |

*Defined at line 82*

---

## pcap_new

An empty little-endian, microsecond pcap file (version 2.4).

```fides
pub fn pcap_new(linktype: i64, snaplen: i64) -> PcapFile {
```

**Parameters:**

- `linktype`: `i64`
- `snaplen`: `i64`

**Returns:** `PcapFile`

*Defined at line 172*

---

## pcap_add

Append a packet (its whole length captured).

```fides
pub fn pcap_add(f: &mut PcapFile, sec: i64, nsec: i64, data: Vec<i64>) {
```

**Parameters:**

- `f`: `&mut PcapFile`
- `sec`: `i64`
- `nsec`: `i64`
- `data`: `Vec<i64>`

*Defined at line 186*

---

## pcap_add_truncated

Append a packet cut to `snaplen` bytes; `orig_len` keeps the full size.

```fides
pub fn pcap_add_truncated(f: &mut PcapFile, sec: i64, nsec: i64, data: Vec<i64>) {
```

**Parameters:**

- `f`: `&mut PcapFile`
- `sec`: `i64`
- `nsec`: `i64`
- `data`: `Vec<i64>`

*Defined at line 191*

---

## pcap_read

Parse a classic pcap file.

```fides
pub fn pcap_read(b: Vec<i64>) -> PcapFile {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `PcapFile`

*Defined at line 200*

---

## pcap_write

Serialize a classic pcap file in its byte order and timestamp unit.

```fides
pub fn pcap_write(f: &PcapFile) -> Vec<i64> {
```

**Parameters:**

- `f`: `&PcapFile`

**Returns:** `Vec<i64>`

*Defined at line 256*

---

## pcapng_units_per_sec

Units per second of an if_tsresol byte (10^n or 2^n).

```fides
pub fn pcapng_units_per_sec(tsresol: i64) -> i64 {
```

**Parameters:**

- `tsresol`: `i64`

**Returns:** `i64`

*Defined at line 294*

---

## pcapng_ts_split

Split a timestamp in interface units into [seconds, nanoseconds].

```fides
pub fn pcapng_ts_split(ts: u64, tsresol: i64) -> Vec<i64> {
```

**Parameters:**

- `ts`: `u64`
- `tsresol`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 315*

---

## pcapng_ts_join

A timestamp of [seconds, nanoseconds] in interface units.

```fides
pub fn pcapng_ts_join(sec: i64, nsec: i64, tsresol: i64) -> u64 {
```

**Parameters:**

- `sec`: `i64`
- `nsec`: `i64`
- `tsresol`: `i64`

**Returns:** `u64`

*Defined at line 336*

---

## pcapng_new

An empty pcapng file (little-endian, version 1.0, no interfaces).

```fides
pub fn pcapng_new() -> PcapngFile {
```

**Returns:** `PcapngFile`

*Defined at line 359*

---

## pcapng_iface

An interface with microsecond timestamps.

```fides
pub fn pcapng_iface(linktype: i64, snaplen: i64, name: str) -> PcapngIface {
```

**Parameters:**

- `linktype`: `i64`
- `snaplen`: `i64`
- `name`: `str`

**Returns:** `PcapngIface`

*Defined at line 366*

---

## pcapng_add_iface

Add an interface; returns its index.

```fides
pub fn pcapng_add_iface(f: &mut PcapngFile, i: PcapngIface) -> i64 {
```

**Parameters:**

- `f`: `&mut PcapngFile`
- `i`: `PcapngIface`

**Returns:** `i64`

*Defined at line 373*

---

## pcapng_add

Add a packet seen on interface `iface`.

```fides
pub fn pcapng_add(f: &mut PcapngFile, iface: i64, sec: i64, nsec: i64, data: Vec<i64>) {
```

**Parameters:**

- `f`: `&mut PcapngFile`
- `iface`: `i64`
- `sec`: `i64`
- `nsec`: `i64`
- `data`: `Vec<i64>`

*Defined at line 379*

---

## pcapng_read

Parse a pcapng file (every section).

```fides
pub fn pcapng_read(b: Vec<i64>) -> PcapngFile {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `PcapngFile`

*Defined at line 384*

---

## pcapng_write

Serialize as one section: SHB, one IDB per interface, one EPB per packet.
Options are written only when set; an option list ends with opt_endofopt.

```fides
pub fn pcapng_write(f: &PcapngFile) -> Vec<i64> {
```

**Parameters:**

- `f`: `&PcapngFile`

**Returns:** `Vec<i64>`

*Defined at line 564*

---

## pcap_to_pcapng

Convert a classic pcap file into a one-interface pcapng file (keeps the
nanosecond resolution when the pcap file had it).

```fides
pub fn pcap_to_pcapng(f: &PcapFile) -> PcapngFile {
```

**Parameters:**

- `f`: `&PcapFile`

**Returns:** `PcapngFile`

*Defined at line 641*

---

## pcap_is_pcapng

True when the bytes start like a pcapng file, false for anything else.

```fides
pub fn pcap_is_pcapng(b: &Vec<i64>) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `bool`

*Defined at line 657*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
