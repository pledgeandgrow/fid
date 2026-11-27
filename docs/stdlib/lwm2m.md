[All modules](index.md)

# lwm2m -- Fides Documentation

## Contents

- [LWM2M_STRING:](#LWM2M_STRING:) (const)
- [LWM2M_INT:](#LWM2M_INT:) (const)
- [LWM2M_FLOAT:](#LWM2M_FLOAT:) (const)
- [LWM2M_BOOL:](#LWM2M_BOOL:) (const)
- [LWM2M_OPAQUE:](#LWM2M_OPAQUE:) (const)
- [LWM2M_TIME:](#LWM2M_TIME:) (const)
- [LWM2M_OBJLNK:](#LWM2M_OBJLNK:) (const)
- [LWM2M_CF_TEXT:](#LWM2M_CF_TEXT:) (const)
- [LWM2M_CF_LINK:](#LWM2M_CF_LINK:) (const)
- [LWM2M_CF_OPAQUE:](#LWM2M_CF_OPAQUE:) (const)
- [LWM2M_CF_SENML_JSON:](#LWM2M_CF_SENML_JSON:) (const)
- [LWM2M_CF_TLV:](#LWM2M_CF_TLV:) (const)
- [LWM2M_TLV_OBJ_INST:](#LWM2M_TLV_OBJ_INST:) (const)
- [LWM2M_TLV_RES_INST:](#LWM2M_TLV_RES_INST:) (const)
- [LWM2M_TLV_MULTI_RES:](#LWM2M_TLV_MULTI_RES:) (const)
- [LWM2M_TLV_RES:](#LWM2M_TLV_RES:) (const)
- [Lwm2mStore](#Lwm2mStore) (struct)
- [lwm2m_store_new](#lwm2m_store_new) (function)
- [lwm2m_define](#lwm2m_define) (function)
- [lwm2m_define_device](#lwm2m_define_device) (function)
- [lwm2m_find](#lwm2m_find) (function)
- [lwm2m_add_instance](#lwm2m_add_instance) (function)
- [lwm2m_has_instance](#lwm2m_has_instance) (function)
- [lwm2m_instances](#lwm2m_instances) (function)
- [lwm2m_delete_instance](#lwm2m_delete_instance) (function)
- [lwm2m_delete_resource](#lwm2m_delete_resource) (function)
- [lwm2m_set_str](#lwm2m_set_str) (function)
- [lwm2m_set_int](#lwm2m_set_int) (function)
- [lwm2m_set_time](#lwm2m_set_time) (function)
- [lwm2m_set_float](#lwm2m_set_float) (function)
- [lwm2m_set_bool](#lwm2m_set_bool) (function)
- [lwm2m_set_opaque](#lwm2m_set_opaque) (function)
- [lwm2m_set_objlnk](#lwm2m_set_objlnk) (function)
- [lwm2m_get_int](#lwm2m_get_int) (function)
- [lwm2m_get_str](#lwm2m_get_str) (function)
- [lwm2m_get_float](#lwm2m_get_float) (function)
- [lwm2m_int_bytes](#lwm2m_int_bytes) (function)
- [lwm2m_bytes_int](#lwm2m_bytes_int) (function)
- [lwm2m_tlv_resource](#lwm2m_tlv_resource) (function)
- [lwm2m_tlv_instance](#lwm2m_tlv_instance) (function)
- [lwm2m_tlv_object](#lwm2m_tlv_object) (function)
- [Lwm2mTlv](#Lwm2mTlv) (struct)
- [lwm2m_tlv_parse](#lwm2m_tlv_parse) (function)
- [lwm2m_tlv_write](#lwm2m_tlv_write) (function)
- [lwm2m_senml_json](#lwm2m_senml_json) (function)
- [lwm2m_parse_path](#lwm2m_parse_path) (function)
- [lwm2m_senml_write](#lwm2m_senml_write) (function)
- [lwm2m_links](#lwm2m_links) (function)
- [lwm2m_handle_request](#lwm2m_handle_request) (function)
- [LWM2M_UNREGISTERED:](#LWM2M_UNREGISTERED:) (const)
- [LWM2M_REGISTERING:](#LWM2M_REGISTERING:) (const)
- [LWM2M_REGISTERED:](#LWM2M_REGISTERED:) (const)
- [LWM2M_UPDATING:](#LWM2M_UPDATING:) (const)
- [LWM2M_DEREGISTERING:](#LWM2M_DEREGISTERING:) (const)
- [Lwm2mClient](#Lwm2mClient) (struct)
- [lwm2m_client_new](#lwm2m_client_new) (function)
- [lwm2m_register_msg](#lwm2m_register_msg) (function)
- [lwm2m_update_msg](#lwm2m_update_msg) (function)
- [lwm2m_deregister_msg](#lwm2m_deregister_msg) (function)
- [lwm2m_client_register](#lwm2m_client_register) (function)
- [lwm2m_client_update](#lwm2m_client_update) (function)
- [lwm2m_client_deregister](#lwm2m_client_deregister) (function)
- [lwm2m_client_take_outbox](#lwm2m_client_take_outbox) (function)
- [lwm2m_client_receive](#lwm2m_client_receive) (function)
- [lwm2m_client_tick](#lwm2m_client_tick) (function)

## LWM2M_STRING:

```fides
pub const LWM2M_STRING: i64 = 0;
```

*Defined at line 35*

---

## LWM2M_INT:

```fides
pub const LWM2M_INT: i64 = 1;
```

*Defined at line 36*

---

## LWM2M_FLOAT:

```fides
pub const LWM2M_FLOAT: i64 = 2;
```

*Defined at line 37*

---

## LWM2M_BOOL:

```fides
pub const LWM2M_BOOL: i64 = 3;
```

*Defined at line 38*

---

## LWM2M_OPAQUE:

```fides
pub const LWM2M_OPAQUE: i64 = 4;
```

*Defined at line 39*

---

## LWM2M_TIME:

```fides
pub const LWM2M_TIME: i64 = 5;
```

*Defined at line 40*

---

## LWM2M_OBJLNK:

```fides
pub const LWM2M_OBJLNK: i64 = 6;
```

*Defined at line 41*

---

## LWM2M_CF_TEXT:

```fides
pub const LWM2M_CF_TEXT: i64 = 0;
```

*Defined at line 44*

---

## LWM2M_CF_LINK:

```fides
pub const LWM2M_CF_LINK: i64 = 40;
```

*Defined at line 45*

---

## LWM2M_CF_OPAQUE:

```fides
pub const LWM2M_CF_OPAQUE: i64 = 42;
```

*Defined at line 46*

---

## LWM2M_CF_SENML_JSON:

```fides
pub const LWM2M_CF_SENML_JSON: i64 = 110;
```

*Defined at line 47*

---

## LWM2M_CF_TLV:

```fides
pub const LWM2M_CF_TLV: i64 = 11542;
```

*Defined at line 48*

---

## LWM2M_TLV_OBJ_INST:

```fides
pub const LWM2M_TLV_OBJ_INST: i64 = 0;
```

*Defined at line 51*

---

## LWM2M_TLV_RES_INST:

```fides
pub const LWM2M_TLV_RES_INST: i64 = 1;
```

*Defined at line 52*

---

## LWM2M_TLV_MULTI_RES:

```fides
pub const LWM2M_TLV_MULTI_RES: i64 = 2;
```

*Defined at line 53*

---

## LWM2M_TLV_RES:

```fides
pub const LWM2M_TLV_RES: i64 = 3;
```

*Defined at line 54*

---

## Lwm2mStore

Every value of every resource, sorted by (object, instance, resource,
resource instance); ri is -1 for a single resource.

```fides
pub struct Lwm2mStore {
```

**Fields:**

| Name | Type |
|------|------|
| `objs` | `Vec<i64>` |
| `insts` | `Vec<i64>` |
| `ress` | `Vec<i64>` |
| `ris` | `Vec<i64>` |
| `types` | `Vec<i64>` |
| `ints` | `Vec<i64>` |
| `floats` | `Vec<f64>` |
| `strs` | `Vec<str>` |
| `bins` | `Vec<Vec<i64>>` |
| `inst_obj` | `Vec<i64>` |
| `inst_id` | `Vec<i64>` |
| `def_obj` | `Vec<i64>` |
| `def_res` | `Vec<i64>` |
| `def_type` | `Vec<i64>` |
| `def_multi` | `Vec<bool>` |
| `def_exec` | `Vec<bool>` |
| `executed` | `Vec<str>` |

*Defined at line 58*

---

## lwm2m_store_new

```fides
pub fn lwm2m_store_new() -> Lwm2mStore {
```

**Returns:** `Lwm2mStore`

*Defined at line 81*

---

## lwm2m_define

Define resource `res` of object `obj`.

```fides
pub fn lwm2m_define(s: &mut Lwm2mStore, obj: i64, res: i64, ty: i64, multiple: bool, executable: bool) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `res`: `i64`
- `ty`: `i64`
- `multiple`: `bool`
- `executable`: `bool`

*Defined at line 88*

---

## lwm2m_define_device

Definitions of the Device object (3) of LwM2M 1.0 Appendix E.4.

```fides
pub fn lwm2m_define_device(s: &mut Lwm2mStore) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`

*Defined at line 108*

---

## lwm2m_find

Index of a value, or -1.

```fides
pub fn lwm2m_find(s: &Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64) -> i64 {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## lwm2m_add_instance

Make object instance /obj/inst exist.

```fides
pub fn lwm2m_add_instance(s: &mut Lwm2mStore, obj: i64, inst: i64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`

*Defined at line 156*

---

## lwm2m_has_instance

```fides
pub fn lwm2m_has_instance(s: &Lwm2mStore, obj: i64, inst: i64) -> bool {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`

**Returns:** `bool`

*Defined at line 172*

---

## lwm2m_instances

Instances of object `obj`, ascending.

```fides
pub fn lwm2m_instances(s: &Lwm2mStore, obj: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 184*

---

## lwm2m_delete_instance

Delete object instance /obj/inst and its values.

```fides
pub fn lwm2m_delete_instance(s: &mut Lwm2mStore, obj: i64, inst: i64) -> bool {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`

**Returns:** `bool`

*Defined at line 197*

---

## lwm2m_delete_resource

Remove resource /obj/inst/res (every instance of it).

```fides
pub fn lwm2m_delete_resource(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`

*Defined at line 233*

---

## lwm2m_set_str

Set a value; `ri` -1 for a single resource, else the resource instance.

```fides
pub fn lwm2m_set_str(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: str) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `str`

*Defined at line 271*

---

## lwm2m_set_int

```fides
pub fn lwm2m_set_int(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: i64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `i64`

*Defined at line 275*

---

## lwm2m_set_time

```fides
pub fn lwm2m_set_time(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: i64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `i64`

*Defined at line 279*

---

## lwm2m_set_float

```fides
pub fn lwm2m_set_float(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: f64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `f64`

*Defined at line 283*

---

## lwm2m_set_bool

```fides
pub fn lwm2m_set_bool(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: bool) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `bool`

*Defined at line 287*

---

## lwm2m_set_opaque

```fides
pub fn lwm2m_set_opaque(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, v: Vec<i64>) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `v`: `Vec<i64>`

*Defined at line 295*

---

## lwm2m_set_objlnk

An object link to /lobj/linst.

```fides
pub fn lwm2m_set_objlnk(s: &mut Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, lobj: i64, linst: i64) {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `lobj`: `i64`
- `linst`: `i64`

*Defined at line 300*

---

## lwm2m_get_int

```fides
pub fn lwm2m_get_int(s: &Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, def: i64) -> i64 {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `def`: `i64`

**Returns:** `i64`

*Defined at line 304*

---

## lwm2m_get_str

```fides
pub fn lwm2m_get_str(s: &Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64) -> str {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`

**Returns:** `str`

*Defined at line 312*

---

## lwm2m_get_float

```fides
pub fn lwm2m_get_float(s: &Lwm2mStore, obj: i64, inst: i64, res: i64, ri: i64, def: f64) -> f64 {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`
- `ri`: `i64`
- `def`: `f64`

**Returns:** `f64`

*Defined at line 320*

---

## lwm2m_int_bytes

```fides
pub fn lwm2m_int_bytes(v: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 370*

---

## lwm2m_bytes_int

```fides
pub fn lwm2m_bytes_int(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 390*

---

## lwm2m_tlv_resource

TLV of one resource /obj/inst/res.

```fides
pub fn lwm2m_tlv_resource(s: &Lwm2mStore, obj: i64, inst: i64, res: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 480*

---

## lwm2m_tlv_instance

TLV of object instance /obj/inst (its resources, as a Read of
/obj/inst returns them).

```fides
pub fn lwm2m_tlv_instance(s: &Lwm2mStore, obj: i64, inst: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 486*

---

## lwm2m_tlv_object

TLV of object /obj: one object-instance TLV per instance.

```fides
pub fn lwm2m_tlv_object(s: &Lwm2mStore, obj: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 491*

---

## Lwm2mTlv

TLV entries at one level: kind, id and where the value is.

```fides
pub struct Lwm2mTlv {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `kinds` | `Vec<i64>` |
| `ids` | `Vec<i64>` |
| `values` | `Vec<Vec<i64>>` |

*Defined at line 503*

---

## lwm2m_tlv_parse

Split TLV bytes into their top-level entries.

```fides
pub fn lwm2m_tlv_parse(b: &Vec<i64>) -> Lwm2mTlv {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Lwm2mTlv`

*Defined at line 511*

---

## lwm2m_tlv_write

Write TLV resources into /obj/inst (a Write; `replace` first removes
the resources the TLV holds instances of). False on malformed or
untyped data.

```fides
pub fn lwm2m_tlv_write(s: &mut Lwm2mStore, obj: i64, inst: i64, b: &Vec<i64>, replace: bool) -> bool {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `b`: `&Vec<i64>`
- `replace`: `bool`

**Returns:** `bool`

*Defined at line 621*

---

## lwm2m_senml_json

SenML JSON of object instance /obj/inst (res >= 0: that resource only).

```fides
pub fn lwm2m_senml_json(s: &Lwm2mStore, obj: i64, inst: i64, res: i64) -> str {
```

**Parameters:**

- `s`: `&Lwm2mStore`
- `obj`: `i64`
- `inst`: `i64`
- `res`: `i64`

**Returns:** `str`

*Defined at line 707*

---

## lwm2m_parse_path

```fides
pub fn lwm2m_parse_path(p: str) -> Vec<i64> {
```

**Parameters:**

- `p`: `str`

**Returns:** `Vec<i64>`

*Defined at line 731*

---

## lwm2m_senml_write

Write a SenML JSON pack into the store (names resolve to full paths
/O/I/R or /O/I/R/RI). False on bad JSON, paths or types.

```fides
pub fn lwm2m_senml_write(s: &mut Lwm2mStore, text: str) -> bool {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `text`: `str`

**Returns:** `bool`

*Defined at line 765*

---

## lwm2m_links

"</1/0>,</3/0>": every object instance, for Register and Update.

```fides
pub fn lwm2m_links(s: &Lwm2mStore) -> str {
```

**Parameters:**

- `s`: `&Lwm2mStore`

**Returns:** `str`

*Defined at line 846*

---

## lwm2m_handle_request

Answer a request of the LwM2M server (Device Management interface):
GET Read (TLV, or SenML JSON when Accept is 110; Discover when Accept
is 40), PUT Write (replace), POST Write (partial) on an instance or
Execute on an executable resource, DELETE of an instance. The response
has code, options and payload; mid and token are set when it is sent.

```fides
pub fn lwm2m_handle_request(s: &mut Lwm2mStore, req: &CoapMsg) -> CoapMsg {
```

**Parameters:**

- `s`: `&mut Lwm2mStore`
- `req`: `&CoapMsg`

**Returns:** `CoapMsg`

*Defined at line 889*

---

## LWM2M_UNREGISTERED:

```fides
pub const LWM2M_UNREGISTERED: i64 = 0;
```

*Defined at line 990*

---

## LWM2M_REGISTERING:

```fides
pub const LWM2M_REGISTERING: i64 = 1;
```

*Defined at line 991*

---

## LWM2M_REGISTERED:

```fides
pub const LWM2M_REGISTERED: i64 = 2;
```

*Defined at line 992*

---

## LWM2M_UPDATING:

```fides
pub const LWM2M_UPDATING: i64 = 3;
```

*Defined at line 993*

---

## LWM2M_DEREGISTERING:

```fides
pub const LWM2M_DEREGISTERING: i64 = 4;
```

*Defined at line 994*

---

## Lwm2mClient

```fides
pub struct Lwm2mClient {
```

**Fields:**

| Name | Type |
|------|------|
| `endpoint` | `str` |
| `lifetime` | `i64,  // seconds` |
| `binding` | `str,  // "U"` |
| `version` | `str,  // "1.0"` |
| `state` | `i64` |
| `location` | `str,  // "/rd/5a3f" from the 2.01's Location-Path` |
| `registered_at` | `i64` |
| `pending` | `Vec<i64>,  // token of the registration request in flight` |
| `error` | `str` |
| `ep` | `CoapEndpoint` |

*Defined at line 996*

---

## lwm2m_client_new

```fides
pub fn lwm2m_client_new(endpoint: str, lifetime: i64, first_mid: i64, seed: i64) -> Lwm2mClient {
```

**Parameters:**

- `endpoint`: `str`
- `lifetime`: `i64`
- `first_mid`: `i64`
- `seed`: `i64`

**Returns:** `Lwm2mClient`

*Defined at line 1009*

---

## lwm2m_register_msg

The Register request: CON POST /rd?ep=..&lt=..&lwm2m=..&b=.. with the
instances in link format.

```fides
pub fn lwm2m_register_msg(c: &Lwm2mClient, s: &Lwm2mStore) -> CoapMsg {
```

**Parameters:**

- `c`: `&Lwm2mClient`
- `s`: `&Lwm2mStore`

**Returns:** `CoapMsg`

*Defined at line 1017*

---

## lwm2m_update_msg

The Update request: CON POST to the registration location (with the
links when `with_links`).

```fides
pub fn lwm2m_update_msg(c: &Lwm2mClient, s: &Lwm2mStore, with_links: bool) -> CoapMsg {
```

**Parameters:**

- `c`: `&Lwm2mClient`
- `s`: `&Lwm2mStore`
- `with_links`: `bool`

**Returns:** `CoapMsg`

*Defined at line 1031*

---

## lwm2m_deregister_msg

```fides
pub fn lwm2m_deregister_msg(c: &Lwm2mClient) -> CoapMsg {
```

**Parameters:**

- `c`: `&Lwm2mClient`

**Returns:** `CoapMsg`

*Defined at line 1041*

---

## lwm2m_client_register

Send Register.

```fides
pub fn lwm2m_client_register(c: &mut Lwm2mClient, s: &Lwm2mStore, now: i64) {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`
- `s`: `&Lwm2mStore`
- `now`: `i64`

*Defined at line 1053*

---

## lwm2m_client_update

Send Update now (also done by lwm2m_client_tick at 90% of the lifetime).

```fides
pub fn lwm2m_client_update(c: &mut Lwm2mClient, s: &Lwm2mStore, with_links: bool, now: i64) {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`
- `s`: `&Lwm2mStore`
- `with_links`: `bool`
- `now`: `i64`

*Defined at line 1059*

---

## lwm2m_client_deregister

```fides
pub fn lwm2m_client_deregister(c: &mut Lwm2mClient, now: i64) {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`
- `now`: `i64`

*Defined at line 1064*

---

## lwm2m_client_take_outbox

```fides
pub fn lwm2m_client_take_outbox(c: &mut Lwm2mClient) -> Vec<Vec<i64>> {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 1069*

---

## lwm2m_client_receive

One datagram from the server.

```fides
pub fn lwm2m_client_receive(c: &mut Lwm2mClient, s: &mut Lwm2mStore, bytes: Vec<i64>, now: i64) {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`
- `s`: `&mut Lwm2mStore`
- `bytes`: `Vec<i64>`
- `now`: `i64`

*Defined at line 1141*

---

## lwm2m_client_tick

Let time pass: CoAP retransmissions, and Update at 90% of the lifetime.

```fides
pub fn lwm2m_client_tick(c: &mut Lwm2mClient, s: &mut Lwm2mStore, now: i64) {
```

**Parameters:**

- `c`: `&mut Lwm2mClient`
- `s`: `&mut Lwm2mStore`
- `now`: `i64`

*Defined at line 1147*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
