[All modules](index.md)

# mdns -- Fides Documentation

## Contents

- [MDNS_PORT:](#MDNS_PORT:) (const)
- [MDNS_ADDR4:](#MDNS_ADDR4:) (const)
- [MDNS_ADDR6:](#MDNS_ADDR6:) (const)
- [MDNS_A:](#MDNS_A:) (const)
- [MDNS_PTR:](#MDNS_PTR:) (const)
- [MDNS_TXT:](#MDNS_TXT:) (const)
- [MDNS_AAAA:](#MDNS_AAAA:) (const)
- [MDNS_SRV:](#MDNS_SRV:) (const)
- [MDNS_ANY:](#MDNS_ANY:) (const)
- [MDNS_IN:](#MDNS_IN:) (const)
- [MDNS_TTL_HOST:](#MDNS_TTL_HOST:) (const)
- [MDNS_TTL_OTHER:](#MDNS_TTL_OTHER:) (const)
- [MdnsRR](#MdnsRR) (struct)
- [MdnsMsg](#MdnsMsg) (struct)
- [mdns_msg](#mdns_msg) (function)
- [mdns_rr](#mdns_rr) (function)
- [mdns_is_response](#mdns_is_response) (function)
- [mdns_question](#mdns_question) (function)
- [mdns_labels](#mdns_labels) (function)
- [mdns_name_eq](#mdns_name_eq) (function)
- [mdns_encode](#mdns_encode) (function)
- [MdnsDecoded](#MdnsDecoded) (struct)
- [mdns_decode](#mdns_decode) (function)
- [mdns_rr_text](#mdns_rr_text) (function)
- [mdns_ipv4_text](#mdns_ipv4_text) (function)
- [mdns_ipv6_text](#mdns_ipv6_text) (function)
- [MdnsService](#MdnsService) (struct)
- [mdns_service](#mdns_service) (function)
- [mdns_service_type_name](#mdns_service_type_name) (function)
- [mdns_instance_name](#mdns_instance_name) (function)
- [mdns_announce](#mdns_announce) (function)
- [mdns_query](#mdns_query) (function)
- [mdns_enum_name](#mdns_enum_name) (function)
- [mdns_respond](#mdns_respond) (function)
- [mdns_txt_get](#mdns_txt_get) (function)

## MDNS_PORT:

```fides
pub const MDNS_PORT: i64 = 5353;
```

*Defined at line 32*

---

## MDNS_ADDR4:

```fides
pub const MDNS_ADDR4: str = "224.0.0.251";
```

*Defined at line 33*

---

## MDNS_ADDR6:

```fides
pub const MDNS_ADDR6: str = "ff02::fb";
```

*Defined at line 34*

---

## MDNS_A:

```fides
pub const MDNS_A: i64 = 1;
```

*Defined at line 36*

---

## MDNS_PTR:

```fides
pub const MDNS_PTR: i64 = 12;
```

*Defined at line 37*

---

## MDNS_TXT:

```fides
pub const MDNS_TXT: i64 = 16;
```

*Defined at line 38*

---

## MDNS_AAAA:

```fides
pub const MDNS_AAAA: i64 = 28;
```

*Defined at line 39*

---

## MDNS_SRV:

```fides
pub const MDNS_SRV: i64 = 33;
```

*Defined at line 40*

---

## MDNS_ANY:

```fides
pub const MDNS_ANY: i64 = 255;
```

*Defined at line 41*

---

## MDNS_IN:

```fides
pub const MDNS_IN: i64 = 1;
```

*Defined at line 42*

---

## MDNS_TTL_HOST:

```fides
pub const MDNS_TTL_HOST: i64 = 120;
```

*Defined at line 44*

---

## MDNS_TTL_OTHER:

```fides
pub const MDNS_TTL_OTHER: i64 = 4500;
```

*Defined at line 45*

---

## MdnsRR

One resource record. `target` is the name in PTR and SRV data; `txt`
the TXT strings; `addr` the 4 or 16 address bytes; `rdata` the raw data
of other types.

```fides
pub struct MdnsRR {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `rtype` | `i64` |
| `rclass` | `i64` |
| `cache_flush` | `bool` |
| `ttl` | `i64` |
| `target` | `str` |
| `priority` | `i64` |
| `weight` | `i64` |
| `port` | `i64` |
| `txt` | `Vec<str>` |
| `addr` | `Vec<i64>` |
| `rdata` | `Vec<i64>` |

*Defined at line 50*

---

## MdnsMsg

```fides
pub struct MdnsMsg {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `flags` | `i64,  // 0 for a query, 0x8400 for a response (QR, AA)` |
| `q_names` | `Vec<str>` |
| `q_types` | `Vec<i64>` |
| `q_unicast` | `Vec<bool>` |
| `answers` | `Vec<MdnsRR>` |
| `authority` | `Vec<MdnsRR>` |
| `additional` | `Vec<MdnsRR>` |

*Defined at line 65*

---

## mdns_msg

```fides
pub fn mdns_msg(flags: i64) -> MdnsMsg {
```

**Parameters:**

- `flags`: `i64`

**Returns:** `MdnsMsg`

*Defined at line 76*

---

## mdns_rr

```fides
pub fn mdns_rr(name: str, rtype: i64, ttl: i64) -> MdnsRR {
```

**Parameters:**

- `name`: `str`
- `rtype`: `i64`
- `ttl`: `i64`

**Returns:** `MdnsRR`

*Defined at line 82*

---

## mdns_is_response

```fides
pub fn mdns_is_response(m: &MdnsMsg) -> bool {
```

**Parameters:**

- `m`: `&MdnsMsg`

**Returns:** `bool`

*Defined at line 88*

---

## mdns_question

Add a question; `unicast` sets the QU bit.

```fides
pub fn mdns_question(m: &mut MdnsMsg, name: str, qtype: i64, unicast: bool) {
```

**Parameters:**

- `m`: `&mut MdnsMsg`
- `name`: `str`
- `qtype`: `i64`
- `unicast`: `bool`

*Defined at line 93*

---

## mdns_labels

The labels of a name; "\." inside a label is a dot, "\\" a backslash.

```fides
pub fn mdns_labels(name: str) -> Vec<str> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Vec<str>`

*Defined at line 102*

---

## mdns_name_eq

Names compare without regard to ASCII case.

```fides
pub fn mdns_name_eq(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 158*

---

## mdns_encode

Encode a message with name compression ([] when a label is over 63
bytes, a name over 255 or a TXT string over 255).

```fides
pub fn mdns_encode(m: &MdnsMsg) -> Vec<i64> {
```

**Parameters:**

- `m`: `&MdnsMsg`

**Returns:** `Vec<i64>`

*Defined at line 274*

---

## MdnsDecoded

```fides
pub struct MdnsDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `msg` | `MdnsMsg` |

*Defined at line 474*

---

## mdns_decode

```fides
pub fn mdns_decode(b: Vec<i64>) -> MdnsDecoded {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `MdnsDecoded`

*Defined at line 480*

---

## mdns_rr_text

"My Printer._ipp._tcp.local 4500 PTR ..." for logs and tests.

```fides
pub fn mdns_rr_text(x: &MdnsRR) -> str {
```

**Parameters:**

- `x`: `&MdnsRR`

**Returns:** `str`

*Defined at line 519*

---

## mdns_ipv4_text

```fides
pub fn mdns_ipv4_text(a: &Vec<i64>) -> str {
```

**Parameters:**

- `a`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 542*

---

## mdns_ipv6_text

IPv6 text with the longest run of zero groups as "::" (RFC 5952).

```fides
pub fn mdns_ipv6_text(a: &Vec<i64>) -> str {
```

**Parameters:**

- `a`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 556*

---

## MdnsService

```fides
pub struct MdnsService {
```

**Fields:**

| Name | Type |
|------|------|
| `instance` | `str,  // "My Printer"` |
| `stype` | `str,  // "_ipp._tcp"` |
| `domain` | `str,  // "local"` |
| `host` | `str,  // "printer.local"` |
| `port` | `i64` |
| `txt` | `Vec<str>,  // "key=value"` |
| `ipv4` | `Vec<i64>,  // 4 bytes, or []` |
| `ipv6` | `Vec<i64>,  // 16 bytes, or []` |

*Defined at line 613*

---

## mdns_service

```fides
pub fn mdns_service(instance: str, stype: str, host: str, port: i64) -> MdnsService {
```

**Parameters:**

- `instance`: `str`
- `stype`: `str`
- `host`: `str`
- `port`: `i64`

**Returns:** `MdnsService`

*Defined at line 624*

---

## mdns_service_type_name

"_ipp._tcp.local"

```fides
pub fn mdns_service_type_name(s: &MdnsService) -> str {
```

**Parameters:**

- `s`: `&MdnsService`

**Returns:** `str`

*Defined at line 631*

---

## mdns_instance_name

"My Printer._ipp._tcp.local" (dots in the instance escaped).

```fides
pub fn mdns_instance_name(s: &MdnsService) -> str {
```

**Parameters:**

- `s`: `&MdnsService`

**Returns:** `str`

*Defined at line 636*

---

## mdns_announce

The announcement of a service (send it twice, one second apart, RFC
6762 8.3); `goodbye` sets every TTL to 0 (10.1).

```fides
pub fn mdns_announce(s: &MdnsService, goodbye: bool) -> MdnsMsg {
```

**Parameters:**

- `s`: `&MdnsService`
- `goodbye`: `bool`

**Returns:** `MdnsMsg`

*Defined at line 685*

---

## mdns_query

A query for `name`/`qtype`; `known` answers the asker already has go
in the answer section (known-answer suppression).

```fides
pub fn mdns_query(name: str, qtype: i64, unicast: bool, known: Vec<MdnsRR>) -> MdnsMsg {
```

**Parameters:**

- `name`: `str`
- `qtype`: `i64`
- `unicast`: `bool`
- `known`: `Vec<MdnsRR>`

**Returns:** `MdnsMsg`

*Defined at line 702*

---

## mdns_enum_name

The DNS-SD enumeration name of `domain`: "_services._dns-sd._udp.local".

```fides
pub fn mdns_enum_name(domain: str) -> str {
```

**Parameters:**

- `domain`: `str`

**Returns:** `str`

*Defined at line 710*

---

## mdns_respond

Answer a query from `services`: PTR for a service type (and the
enumeration name), SRV/TXT for an instance, A/AAAA for a host, with
the records a resolver needs next as additional records. Answers the
query already lists (with at least half our TTL) are left out. An
empty response (no answers) means: stay silent.

```fides
pub fn mdns_respond(services: &Vec<MdnsService>, q: &MdnsMsg) -> MdnsMsg {
```

**Parameters:**

- `services`: `&Vec<MdnsService>`
- `q`: `&MdnsMsg`

**Returns:** `MdnsMsg`

*Defined at line 749*

---

## mdns_txt_get

Read a TXT string "key=value": the value of `key` ("" when absent).

```fides
pub fn mdns_txt_get(txt: &Vec<str>, key: str) -> str {
```

**Parameters:**

- `txt`: `&Vec<str>`
- `key`: `str`

**Returns:** `str`

*Defined at line 817*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
