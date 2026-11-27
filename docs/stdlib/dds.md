[All modules](index.md)

# dds -- Fides Documentation

## Contents

- [RTPS_PAD:](#RTPS_PAD:) (const)
- [RTPS_ACKNACK:](#RTPS_ACKNACK:) (const)
- [RTPS_HEARTBEAT:](#RTPS_HEARTBEAT:) (const)
- [RTPS_GAP:](#RTPS_GAP:) (const)
- [RTPS_INFO_TS:](#RTPS_INFO_TS:) (const)
- [RTPS_INFO_SRC:](#RTPS_INFO_SRC:) (const)
- [RTPS_INFO_DST:](#RTPS_INFO_DST:) (const)
- [RTPS_DATA:](#RTPS_DATA:) (const)
- [RTPS_KIND_WRITER_KEY:](#RTPS_KIND_WRITER_KEY:) (const)
- [RTPS_KIND_WRITER_NO_KEY:](#RTPS_KIND_WRITER_NO_KEY:) (const)
- [RTPS_KIND_READER_NO_KEY:](#RTPS_KIND_READER_NO_KEY:) (const)
- [RTPS_KIND_READER_KEY:](#RTPS_KIND_READER_KEY:) (const)
- [RTPS_ENTITYID_UNKNOWN:](#RTPS_ENTITYID_UNKNOWN:) (const)
- [RTPS_ENTITYID_PARTICIPANT:](#RTPS_ENTITYID_PARTICIPANT:) (const)
- [RTPS_ENTITYID_SPDP_WRITER:](#RTPS_ENTITYID_SPDP_WRITER:) (const)
- [RTPS_ENTITYID_SPDP_READER:](#RTPS_ENTITYID_SPDP_READER:) (const)
- [RTPS_ENTITYID_SEDP_PUB_WRITER:](#RTPS_ENTITYID_SEDP_PUB_WRITER:) (const)
- [RTPS_ENTITYID_SEDP_PUB_READER:](#RTPS_ENTITYID_SEDP_PUB_READER:) (const)
- [RTPS_ENTITYID_SEDP_SUB_WRITER:](#RTPS_ENTITYID_SEDP_SUB_WRITER:) (const)
- [RTPS_ENTITYID_SEDP_SUB_READER:](#RTPS_ENTITYID_SEDP_SUB_READER:) (const)
- [PID_PAD:](#PID_PAD:) (const)
- [PID_SENTINEL:](#PID_SENTINEL:) (const)
- [PID_PARTICIPANT_LEASE_DURATION:](#PID_PARTICIPANT_LEASE_DURATION:) (const)
- [PID_DOMAIN_ID:](#PID_DOMAIN_ID:) (const)
- [PID_PROTOCOL_VERSION:](#PID_PROTOCOL_VERSION:) (const)
- [PID_VENDORID:](#PID_VENDORID:) (const)
- [PID_DEFAULT_UNICAST_LOCATOR:](#PID_DEFAULT_UNICAST_LOCATOR:) (const)
- [PID_METATRAFFIC_UNICAST_LOCATOR:](#PID_METATRAFFIC_UNICAST_LOCATOR:) (const)
- [PID_METATRAFFIC_MULTICAST_LOCATOR:](#PID_METATRAFFIC_MULTICAST_LOCATOR:) (const)
- [PID_DEFAULT_MULTICAST_LOCATOR:](#PID_DEFAULT_MULTICAST_LOCATOR:) (const)
- [PID_PARTICIPANT_GUID:](#PID_PARTICIPANT_GUID:) (const)
- [PID_BUILTIN_ENDPOINT_SET:](#PID_BUILTIN_ENDPOINT_SET:) (const)
- [PID_ENTITY_NAME:](#PID_ENTITY_NAME:) (const)
- [RTPS_LOCATOR_UDPV4:](#RTPS_LOCATOR_UDPV4:) (const)
- [rtps_entity](#rtps_entity) (function)
- [rtps_hex](#rtps_hex) (function)
- [rtps_prefix](#rtps_prefix) (function)
- [rtps_port_spdp_multicast](#rtps_port_spdp_multicast) (function)
- [rtps_port_spdp_unicast](#rtps_port_spdp_unicast) (function)
- [rtps_port_user_multicast](#rtps_port_user_multicast) (function)
- [rtps_port_user_unicast](#rtps_port_user_unicast) (function)
- [rtps_header](#rtps_header) (function)
- [rtps_snset](#rtps_snset) (function)
- [rtps_sub_data](#rtps_sub_data) (function)
- [rtps_sub_heartbeat](#rtps_sub_heartbeat) (function)
- [rtps_sub_acknack](#rtps_sub_acknack) (function)
- [rtps_sub_gap](#rtps_sub_gap) (function)
- [rtps_sub_info_ts](#rtps_sub_info_ts) (function)
- [rtps_sub_info_dst](#rtps_sub_info_dst) (function)
- [RtpsSub](#RtpsSub) (struct)
- [RtpsMsg](#RtpsMsg) (struct)
- [rtps_parse](#rtps_parse) (function)
- [rtps_sub_name](#rtps_sub_name) (function)
- [RtpsLocator](#RtpsLocator) (struct)
- [rtps_locator_v4](#rtps_locator_v4) (function)
- [rtps_locator_str](#rtps_locator_str) (function)
- [SpdpData](#SpdpData) (struct)
- [spdp_new](#spdp_new) (function)
- [spdp_encode](#spdp_encode) (function)
- [spdp_decode](#spdp_decode) (function)
- [spdp_message](#spdp_message) (function)
- [RtpsChange](#RtpsChange) (struct)
- [RtpsReaderProxy](#RtpsReaderProxy) (struct)
- [RtpsWriter](#RtpsWriter) (struct)
- [rtps_writer_new](#rtps_writer_new) (function)
- [rtps_writer_add_reader](#rtps_writer_add_reader) (function)
- [rtps_writer_write](#rtps_writer_write) (function)
- [rtps_writer_remove](#rtps_writer_remove) (function)
- [rtps_writer_heartbeat](#rtps_writer_heartbeat) (function)
- [rtps_writer_flush](#rtps_writer_flush) (function)
- [rtps_writer_receive](#rtps_writer_receive) (function)
- [rtps_writer_all_acked](#rtps_writer_all_acked) (function)
- [rtps_writer_take_outbox](#rtps_writer_take_outbox) (function)
- [RtpsReader](#RtpsReader) (struct)
- [rtps_reader_new](#rtps_reader_new) (function)
- [rtps_reader_add_writer](#rtps_reader_add_writer) (function)
- [rtps_reader_receive](#rtps_reader_receive) (function)
- [rtps_reader_take_outbox](#rtps_reader_take_outbox) (function)

## RTPS_PAD:

```fides
pub const RTPS_PAD: i64 = 1;
```

*Defined at line 45*

---

## RTPS_ACKNACK:

```fides
pub const RTPS_ACKNACK: i64 = 6;
```

*Defined at line 46*

---

## RTPS_HEARTBEAT:

```fides
pub const RTPS_HEARTBEAT: i64 = 7;
```

*Defined at line 47*

---

## RTPS_GAP:

```fides
pub const RTPS_GAP: i64 = 8;
```

*Defined at line 48*

---

## RTPS_INFO_TS:

```fides
pub const RTPS_INFO_TS: i64 = 9;
```

*Defined at line 49*

---

## RTPS_INFO_SRC:

```fides
pub const RTPS_INFO_SRC: i64 = 12;
```

*Defined at line 50*

---

## RTPS_INFO_DST:

```fides
pub const RTPS_INFO_DST: i64 = 14;
```

*Defined at line 51*

---

## RTPS_DATA:

```fides
pub const RTPS_DATA: i64 = 21;
```

*Defined at line 52*

---

## RTPS_KIND_WRITER_KEY:

```fides
pub const RTPS_KIND_WRITER_KEY: i64 = 2;
```

*Defined at line 54*

---

## RTPS_KIND_WRITER_NO_KEY:

```fides
pub const RTPS_KIND_WRITER_NO_KEY: i64 = 3;
```

*Defined at line 55*

---

## RTPS_KIND_READER_NO_KEY:

```fides
pub const RTPS_KIND_READER_NO_KEY: i64 = 4;
```

*Defined at line 56*

---

## RTPS_KIND_READER_KEY:

```fides
pub const RTPS_KIND_READER_KEY: i64 = 7;
```

*Defined at line 57*

---

## RTPS_ENTITYID_UNKNOWN:

Entity ids as 32-bit values (the four octets, first one highest).

```fides
pub const RTPS_ENTITYID_UNKNOWN: i64 = 0;
```

*Defined at line 60*

---

## RTPS_ENTITYID_PARTICIPANT:

```fides
pub const RTPS_ENTITYID_PARTICIPANT: i64 = 0x000001c1;
```

*Defined at line 61*

---

## RTPS_ENTITYID_SPDP_WRITER:

```fides
pub const RTPS_ENTITYID_SPDP_WRITER: i64 = 0x000100c2;
```

*Defined at line 62*

---

## RTPS_ENTITYID_SPDP_READER:

```fides
pub const RTPS_ENTITYID_SPDP_READER: i64 = 0x000100c7;
```

*Defined at line 63*

---

## RTPS_ENTITYID_SEDP_PUB_WRITER:

```fides
pub const RTPS_ENTITYID_SEDP_PUB_WRITER: i64 = 0x000003c2;
```

*Defined at line 64*

---

## RTPS_ENTITYID_SEDP_PUB_READER:

```fides
pub const RTPS_ENTITYID_SEDP_PUB_READER: i64 = 0x000003c7;
```

*Defined at line 65*

---

## RTPS_ENTITYID_SEDP_SUB_WRITER:

```fides
pub const RTPS_ENTITYID_SEDP_SUB_WRITER: i64 = 0x000004c2;
```

*Defined at line 66*

---

## RTPS_ENTITYID_SEDP_SUB_READER:

```fides
pub const RTPS_ENTITYID_SEDP_SUB_READER: i64 = 0x000004c7;
```

*Defined at line 67*

---

## PID_PAD:

Parameter ids of SPDP data.

```fides
pub const PID_PAD: i64 = 0;
```

*Defined at line 70*

---

## PID_SENTINEL:

```fides
pub const PID_SENTINEL: i64 = 1;
```

*Defined at line 71*

---

## PID_PARTICIPANT_LEASE_DURATION:

```fides
pub const PID_PARTICIPANT_LEASE_DURATION: i64 = 2;
```

*Defined at line 72*

---

## PID_DOMAIN_ID:

```fides
pub const PID_DOMAIN_ID: i64 = 15;
```

*Defined at line 73*

---

## PID_PROTOCOL_VERSION:

```fides
pub const PID_PROTOCOL_VERSION: i64 = 21;
```

*Defined at line 74*

---

## PID_VENDORID:

```fides
pub const PID_VENDORID: i64 = 22;
```

*Defined at line 75*

---

## PID_DEFAULT_UNICAST_LOCATOR:

```fides
pub const PID_DEFAULT_UNICAST_LOCATOR: i64 = 49;
```

*Defined at line 76*

---

## PID_METATRAFFIC_UNICAST_LOCATOR:

```fides
pub const PID_METATRAFFIC_UNICAST_LOCATOR: i64 = 50;
```

*Defined at line 77*

---

## PID_METATRAFFIC_MULTICAST_LOCATOR:

```fides
pub const PID_METATRAFFIC_MULTICAST_LOCATOR: i64 = 51;
```

*Defined at line 78*

---

## PID_DEFAULT_MULTICAST_LOCATOR:

```fides
pub const PID_DEFAULT_MULTICAST_LOCATOR: i64 = 72;
```

*Defined at line 79*

---

## PID_PARTICIPANT_GUID:

```fides
pub const PID_PARTICIPANT_GUID: i64 = 80;
```

*Defined at line 80*

---

## PID_BUILTIN_ENDPOINT_SET:

```fides
pub const PID_BUILTIN_ENDPOINT_SET: i64 = 88;
```

*Defined at line 81*

---

## PID_ENTITY_NAME:

```fides
pub const PID_ENTITY_NAME: i64 = 98;
```

*Defined at line 82*

---

## RTPS_LOCATOR_UDPV4:

```fides
pub const RTPS_LOCATOR_UDPV4: i64 = 1;
```

*Defined at line 84*

---

## rtps_entity

A user entity id from a 24-bit key and a kind.

```fides
pub fn rtps_entity(key: i64, kind: i64) -> i64 {
```

**Parameters:**

- `key`: `i64`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 87*

---

## rtps_hex

Hex dump of bytes ("52 54 50 53 ...").

```fides
pub fn rtps_hex(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 177*

---

## rtps_prefix

A GUID prefix from a host id, app id and instance id (the layout most
vendors use: 4 bytes each, big-endian).

```fides
pub fn rtps_prefix(host: i64, app: i64, instance: i64) -> Vec<i64> {
```

**Parameters:**

- `host`: `i64`
- `app`: `i64`
- `instance`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 194*

---

## rtps_port_spdp_multicast

SPDP multicast port of a domain.

```fides
pub fn rtps_port_spdp_multicast(domain: i64) -> i64 {
```

**Parameters:**

- `domain`: `i64`

**Returns:** `i64`

*Defined at line 205*

---

## rtps_port_spdp_unicast

SPDP unicast port of participant `pid` in a domain.

```fides
pub fn rtps_port_spdp_unicast(domain: i64, pid: i64) -> i64 {
```

**Parameters:**

- `domain`: `i64`
- `pid`: `i64`

**Returns:** `i64`

*Defined at line 210*

---

## rtps_port_user_multicast

User traffic multicast port.

```fides
pub fn rtps_port_user_multicast(domain: i64) -> i64 {
```

**Parameters:**

- `domain`: `i64`

**Returns:** `i64`

*Defined at line 215*

---

## rtps_port_user_unicast

User traffic unicast port.

```fides
pub fn rtps_port_user_unicast(domain: i64, pid: i64) -> i64 {
```

**Parameters:**

- `domain`: `i64`
- `pid`: `i64`

**Returns:** `i64`

*Defined at line 220*

---

## rtps_header

The 20-byte header: "RTPS", version 2.3, vendor id, GUID prefix.

```fides
pub fn rtps_header(prefix: &Vec<i64>, vendor: i64) -> Vec<i64> {
```

**Parameters:**

- `prefix`: `&Vec<i64>`
- `vendor`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 227*

---

## rtps_snset

A SequenceNumberSet holding the sequence numbers in `set` (each in
base .. base + 255; others are ignored); numBits reaches the last one.

```fides
pub fn rtps_snset(base: i64, set: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `base`: `i64`
- `set`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 253*

---

## rtps_sub_data

DATA: a serialized payload (with its encapsulation header) from writer
to reader. The payload is padded with zeros to a multiple of 4.

```fides
pub fn rtps_sub_data(reader: i64, writer: i64, sn: i64, payload: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `reader`: `i64`
- `writer`: `i64`
- `sn`: `i64`
- `payload`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 292*

---

## rtps_sub_heartbeat

HEARTBEAT: the writer has first..last (F: no answer needed when all is
received; L: liveliness).

```fides
pub fn rtps_sub_heartbeat(reader: i64, writer: i64, first: i64, last: i64, count: i64, final_flag: bool, liveliness: bool) -> Vec<i64> {
```

**Parameters:**

- `reader`: `i64`
- `writer`: `i64`
- `first`: `i64`
- `last`: `i64`
- `count`: `i64`
- `final_flag`: `bool`
- `liveliness`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 309*

---

## rtps_sub_acknack

ACKNACK: everything before `base` received; `missing` requested again.

```fides
pub fn rtps_sub_acknack(reader: i64, writer: i64, base: i64, missing: &Vec<i64>, count: i64, final_flag: bool) -> Vec<i64> {
```

**Parameters:**

- `reader`: `i64`
- `writer`: `i64`
- `base`: `i64`
- `missing`: `&Vec<i64>`
- `count`: `i64`
- `final_flag`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 327*

---

## rtps_sub_gap

GAP: gap_start .. list_base - 1 and the members of `list` are
irrelevant to the reader.

```fides
pub fn rtps_sub_gap(reader: i64, writer: i64, gap_start: i64, list_base: i64, list: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `reader`: `i64`
- `writer`: `i64`
- `gap_start`: `i64`
- `list_base`: `i64`
- `list`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 342*

---

## rtps_sub_info_ts

INFO_TS with a timestamp (seconds, 1/2^32 fractions).

```fides
pub fn rtps_sub_info_ts(sec: i64, frac: i64) -> Vec<i64> {
```

**Parameters:**

- `sec`: `i64`
- `frac`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 352*

---

## rtps_sub_info_dst

INFO_DST: the following submessages are for this participant.

```fides
pub fn rtps_sub_info_dst(prefix: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `prefix`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 360*

---

## RtpsSub

One parsed submessage with the receiver context in force for it.

```fides
pub struct RtpsSub {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `flags` | `i64` |
| `reader` | `i64` |
| `writer` | `i64` |
| `sn` | `i64` |
| `first` | `i64` |
| `last` | `i64` |
| `count` | `i64` |
| `set_base` | `i64` |
| `set` | `Vec<i64>` |
| `payload` | `Vec<i64>` |
| `src_prefix` | `Vec<i64>` |
| `dst_prefix` | `Vec<i64>` |
| `has_ts` | `bool` |
| `ts_sec` | `i64` |
| `ts_frac` | `i64` |

*Defined at line 367*

---

## RtpsMsg

```fides
pub struct RtpsMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `major` | `i64` |
| `minor` | `i64` |
| `vendor` | `i64` |
| `prefix` | `Vec<i64>` |
| `subs` | `Vec<RtpsSub>` |

*Defined at line 388*

---

## rtps_parse

Parse an RTPS message.

```fides
pub fn rtps_parse(b: &Vec<i64>) -> RtpsMsg {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `RtpsMsg`

*Defined at line 431*

---

## rtps_sub_name

```fides
pub fn rtps_sub_name(id: i64) -> str {
```

**Parameters:**

- `id`: `i64`

**Returns:** `str`

*Defined at line 564*

---

## RtpsLocator

```fides
pub struct RtpsLocator {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `port` | `i64` |
| `addr` | `Vec<i64>` |

*Defined at line 594*

---

## rtps_locator_v4

A UDPv4 locator for a.b.c.d:port.

```fides
pub fn rtps_locator_v4(a: i64, b: i64, c: i64, d: i64, port: i64) -> RtpsLocator {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `c`: `i64`
- `d`: `i64`
- `port`: `i64`

**Returns:** `RtpsLocator`

*Defined at line 602*

---

## rtps_locator_str

```fides
pub fn rtps_locator_str(l: &RtpsLocator) -> str {
```

**Parameters:**

- `l`: `&RtpsLocator`

**Returns:** `str`

*Defined at line 618*

---

## SpdpData

```fides
pub struct SpdpData {
```

**Fields:**

| Name | Type |
|------|------|
| `prefix` | `Vec<i64>` |
| `vendor` | `i64` |
| `major` | `i64` |
| `minor` | `i64` |
| `domain` | `i64` |
| `lease_sec` | `i64` |
| `builtin_endpoints` | `i64` |
| `name` | `str` |
| `meta_unicast` | `Vec<RtpsLocator>` |
| `meta_multicast` | `Vec<RtpsLocator>` |
| `default_unicast` | `Vec<RtpsLocator>` |
| `ok` | `bool` |

*Defined at line 622*

---

## spdp_new

```fides
pub fn spdp_new(prefix: Vec<i64>, vendor: i64, domain: i64, name: str) -> SpdpData {
```

**Parameters:**

- `prefix`: `Vec<i64>`
- `vendor`: `i64`
- `domain`: `i64`
- `name`: `str`

**Returns:** `SpdpData`

*Defined at line 637*

---

## spdp_encode

The serialized payload (PL_CDR_LE) of a participant announcement.

```fides
pub fn spdp_encode(d: &SpdpData) -> Vec<i64> {
```

**Parameters:**

- `d`: `&SpdpData`

**Returns:** `Vec<i64>`

*Defined at line 665*

---

## spdp_decode

Parse a participant announcement payload (PL_CDR_LE or PL_CDR_BE);
unknown parameters are skipped.

```fides
pub fn spdp_decode(b: &Vec<i64>) -> SpdpData {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `SpdpData`

*Defined at line 732*

---

## spdp_message

A complete SPDP announcement message (to the SPDP multicast group).

```fides
pub fn spdp_message(d: &SpdpData, sn: i64, ts_sec: i64) -> Vec<i64> {
```

**Parameters:**

- `d`: `&SpdpData`
- `sn`: `i64`
- `ts_sec`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 786*

---

## RtpsChange

```fides
pub struct RtpsChange {
```

**Fields:**

| Name | Type |
|------|------|
| `sn` | `i64` |
| `payload` | `Vec<i64>` |

*Defined at line 795*

---

## RtpsReaderProxy

```fides
pub struct RtpsReaderProxy {
```

**Fields:**

| Name | Type |
|------|------|
| `prefix` | `Vec<i64>` |
| `reader_id` | `i64` |
| `acked` | `i64` |
| `sent` | `i64` |

*Defined at line 800*

---

## RtpsWriter

```fides
pub struct RtpsWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `prefix` | `Vec<i64>` |
| `writer_id` | `i64` |
| `vendor` | `i64` |
| `history` | `Vec<RtpsChange>` |
| `next_sn` | `i64` |
| `hb_count` | `i64` |
| `readers` | `Vec<RtpsReaderProxy>` |
| `outbox` | `Vec<Vec<i64>>` |
| `resent` | `i64` |
| `gaps_sent` | `i64` |

*Defined at line 809*

---

## rtps_writer_new

```fides
pub fn rtps_writer_new(prefix: Vec<i64>, writer_id: i64) -> RtpsWriter {
```

**Parameters:**

- `prefix`: `Vec<i64>`
- `writer_id`: `i64`

**Returns:** `RtpsWriter`

*Defined at line 822*

---

## rtps_writer_add_reader

Match a reader (by its GUID).

```fides
pub fn rtps_writer_add_reader(w: &mut RtpsWriter, prefix: Vec<i64>, reader_id: i64) {
```

**Parameters:**

- `w`: `&mut RtpsWriter`
- `prefix`: `Vec<i64>`
- `reader_id`: `i64`

*Defined at line 829*

---

## rtps_writer_write

Add a change to the history; its sequence number.

```fides
pub fn rtps_writer_write(w: &mut RtpsWriter, payload: Vec<i64>) -> i64 {
```

**Parameters:**

- `w`: `&mut RtpsWriter`
- `payload`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 836*

---

## rtps_writer_remove

Drop a change from the history (readers that ask for it get a GAP).

```fides
pub fn rtps_writer_remove(w: &mut RtpsWriter, sn: i64) {
```

**Parameters:**

- `w`: `&mut RtpsWriter`
- `sn`: `i64`

*Defined at line 846*

---

## rtps_writer_heartbeat

A HEARTBEAT to every matched reader (one message each) in the outbox.

```fides
pub fn rtps_writer_heartbeat(w: &mut RtpsWriter) {
```

**Parameters:**

- `w`: `&mut RtpsWriter`

*Defined at line 882*

---

## rtps_writer_flush

Send every change a reader has not been sent yet (one DATA message per
change), then a HEARTBEAT.

```fides
pub fn rtps_writer_flush(w: &mut RtpsWriter) {
```

**Parameters:**

- `w`: `&mut RtpsWriter`

*Defined at line 896*

---

## rtps_writer_receive

Handle a datagram for this writer (ACKNACKs): acknowledged changes are
recorded, requested ones are sent again, or as a GAP when gone.

```fides
pub fn rtps_writer_receive(w: &mut RtpsWriter, bytes: &Vec<i64>) {
```

**Parameters:**

- `w`: `&mut RtpsWriter`
- `bytes`: `&Vec<i64>`

*Defined at line 919*

---

## rtps_writer_all_acked

Whether every matched reader acknowledged everything written.

```fides
pub fn rtps_writer_all_acked(w: &RtpsWriter) -> bool {
```

**Parameters:**

- `w`: `&RtpsWriter`

**Returns:** `bool`

*Defined at line 965*

---

## rtps_writer_take_outbox

Take the outgoing datagrams.

```fides
pub fn rtps_writer_take_outbox(w: &mut RtpsWriter) -> Vec<Vec<i64>> {
```

**Parameters:**

- `w`: `&mut RtpsWriter`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 977*

---

## RtpsReader

```fides
pub struct RtpsReader {
```

**Fields:**

| Name | Type |
|------|------|
| `prefix` | `Vec<i64>` |
| `reader_id` | `i64` |
| `vendor` | `i64` |
| `writer_prefix` | `Vec<i64>` |
| `writer_id` | `i64` |
| `next` | `i64` |
| `pend_sn` | `Vec<i64>` |
| `pend_data` | `Vec<Vec<i64>>` |
| `pend_gap` | `Vec<bool>` |
| `delivered` | `Vec<Vec<i64>>` |
| `delivered_sn` | `Vec<i64>` |
| `ack_count` | `i64` |
| `outbox` | `Vec<Vec<i64>>` |
| `duplicates` | `i64` |

*Defined at line 985*

---

## rtps_reader_new

```fides
pub fn rtps_reader_new(prefix: Vec<i64>, reader_id: i64) -> RtpsReader {
```

**Parameters:**

- `prefix`: `Vec<i64>`
- `reader_id`: `i64`

**Returns:** `RtpsReader`

*Defined at line 1004*

---

## rtps_reader_add_writer

Match the (one) remote writer.

```fides
pub fn rtps_reader_add_writer(r: &mut RtpsReader, prefix: Vec<i64>, writer_id: i64) {
```

**Parameters:**

- `r`: `&mut RtpsReader`
- `prefix`: `Vec<i64>`
- `writer_id`: `i64`

*Defined at line 1011*

---

## rtps_reader_receive

Handle a datagram for this reader: DATA, GAP and HEARTBEAT from the
matched writer (an ACKNACK answering a HEARTBEAT goes to the outbox).

```fides
pub fn rtps_reader_receive(r: &mut RtpsReader, bytes: &Vec<i64>) {
```

**Parameters:**

- `r`: `&mut RtpsReader`
- `bytes`: `&Vec<i64>`

*Defined at line 1091*

---

## rtps_reader_take_outbox

Take the outgoing datagrams.

```fides
pub fn rtps_reader_take_outbox(r: &mut RtpsReader) -> Vec<Vec<i64>> {
```

**Parameters:**

- `r`: `&mut RtpsReader`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 1130*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
