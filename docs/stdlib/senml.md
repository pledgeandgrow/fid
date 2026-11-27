[All modules](index.md)

# senml -- Fides Documentation

## Contents

- [SENML_BN:](#SENML_BN:) (const)
- [SENML_BT:](#SENML_BT:) (const)
- [SENML_BU:](#SENML_BU:) (const)
- [SENML_BV:](#SENML_BV:) (const)
- [SENML_BS:](#SENML_BS:) (const)
- [SENML_BVER:](#SENML_BVER:) (const)
- [SENML_N:](#SENML_N:) (const)
- [SENML_U:](#SENML_U:) (const)
- [SENML_V:](#SENML_V:) (const)
- [SENML_VS:](#SENML_VS:) (const)
- [SENML_VB:](#SENML_VB:) (const)
- [SENML_VD:](#SENML_VD:) (const)
- [SENML_S:](#SENML_S:) (const)
- [SENML_T:](#SENML_T:) (const)
- [SENML_UT:](#SENML_UT:) (const)
- [SENML_REL_LIMIT:](#SENML_REL_LIMIT:) (const)
- [SenmlRecord](#SenmlRecord) (struct)
- [SenmlPack](#SenmlPack) (struct)
- [senml_record](#senml_record) (function)
- [senml_value](#senml_value) (function)
- [senml_has](#senml_has) (function)
- [senml_set_bn](#senml_set_bn) (function)
- [senml_set_bt](#senml_set_bt) (function)
- [senml_set_bu](#senml_set_bu) (function)
- [senml_set_bv](#senml_set_bv) (function)
- [senml_set_bs](#senml_set_bs) (function)
- [senml_set_bver](#senml_set_bver) (function)
- [senml_set_n](#senml_set_n) (function)
- [senml_set_u](#senml_set_u) (function)
- [senml_set_v](#senml_set_v) (function)
- [senml_set_vs](#senml_set_vs) (function)
- [senml_set_vb](#senml_set_vb) (function)
- [senml_set_vd](#senml_set_vd) (function)
- [senml_set_s](#senml_set_s) (function)
- [senml_set_t](#senml_set_t) (function)
- [senml_set_ut](#senml_set_ut) (function)
- [senml_b64url_encode](#senml_b64url_encode) (function)
- [senml_b64url_decode](#senml_b64url_decode) (function)
- [senml_num](#senml_num) (function)
- [senml_json_record](#senml_json_record) (function)
- [senml_json_encode](#senml_json_encode) (function)
- [senml_json_decode](#senml_json_decode) (function)
- [senml_cbor_key](#senml_cbor_key) (function)
- [senml_cbor_put_num](#senml_cbor_put_num) (function)
- [senml_cbor_encode](#senml_cbor_encode) (function)
- [senml_cbor_decode](#senml_cbor_decode) (function)
- [senml_name_ok](#senml_name_ok) (function)
- [senml_resolve](#senml_resolve) (function)

## SENML_BN:

```fides
pub const SENML_BN: i64 = 1;
```

*Defined at line 40*

---

## SENML_BT:

```fides
pub const SENML_BT: i64 = 2;
```

*Defined at line 41*

---

## SENML_BU:

```fides
pub const SENML_BU: i64 = 4;
```

*Defined at line 42*

---

## SENML_BV:

```fides
pub const SENML_BV: i64 = 8;
```

*Defined at line 43*

---

## SENML_BS:

```fides
pub const SENML_BS: i64 = 16;
```

*Defined at line 44*

---

## SENML_BVER:

```fides
pub const SENML_BVER: i64 = 32;
```

*Defined at line 45*

---

## SENML_N:

```fides
pub const SENML_N: i64 = 64;
```

*Defined at line 46*

---

## SENML_U:

```fides
pub const SENML_U: i64 = 128;
```

*Defined at line 47*

---

## SENML_V:

```fides
pub const SENML_V: i64 = 256;
```

*Defined at line 48*

---

## SENML_VS:

```fides
pub const SENML_VS: i64 = 512;
```

*Defined at line 49*

---

## SENML_VB:

```fides
pub const SENML_VB: i64 = 1024;
```

*Defined at line 50*

---

## SENML_VD:

```fides
pub const SENML_VD: i64 = 2048;
```

*Defined at line 51*

---

## SENML_S:

```fides
pub const SENML_S: i64 = 4096;
```

*Defined at line 52*

---

## SENML_T:

```fides
pub const SENML_T: i64 = 8192;
```

*Defined at line 53*

---

## SENML_UT:

```fides
pub const SENML_UT: i64 = 16384;
```

*Defined at line 54*

---

## SENML_REL_LIMIT:

Times below this are relative to "now" (RFC 8428 4.5.3).

```fides
pub const SENML_REL_LIMIT: f64 = 268435456.0;
```

*Defined at line 57*

---

## SenmlRecord

```fides
pub struct SenmlRecord {
```

**Fields:**

| Name | Type |
|------|------|
| `fields` | `i64` |
| `bn` | `str` |
| `bt` | `f64` |
| `bu` | `str` |
| `bv` | `f64` |
| `bs` | `f64` |
| `bver` | `i64` |
| `n` | `str` |
| `u` | `str` |
| `v` | `f64` |
| `vs` | `str` |
| `vb` | `bool` |
| `vd` | `Vec<i64>` |
| `s` | `f64` |
| `t` | `f64` |
| `ut` | `f64` |

*Defined at line 59*

---

## SenmlPack

A decoded or resolved pack. `ok` false: `error` says why.

```fides
pub struct SenmlPack {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `records` | `Vec<SenmlRecord>` |

*Defined at line 79*

---

## senml_record

```fides
pub fn senml_record() -> SenmlRecord {
```

**Returns:** `SenmlRecord`

*Defined at line 85*

---

## senml_value

A record with a name, a unit ("" for none) and a numeric value.

```fides
pub fn senml_value(name: str, unit: str, v: f64) -> SenmlRecord {
```

**Parameters:**

- `name`: `str`
- `unit`: `str`
- `v`: `f64`

**Returns:** `SenmlRecord`

*Defined at line 92*

---

## senml_has

```fides
pub fn senml_has(r: &SenmlRecord, bit: i64) -> bool {
```

**Parameters:**

- `r`: `&SenmlRecord`
- `bit`: `i64`

**Returns:** `bool`

*Defined at line 102*

---

## senml_set_bn

```fides
pub fn senml_set_bn(r: &mut SenmlRecord, x: str) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `str`

*Defined at line 106*

---

## senml_set_bt

```fides
pub fn senml_set_bt(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 111*

---

## senml_set_bu

```fides
pub fn senml_set_bu(r: &mut SenmlRecord, x: str) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `str`

*Defined at line 116*

---

## senml_set_bv

```fides
pub fn senml_set_bv(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 121*

---

## senml_set_bs

```fides
pub fn senml_set_bs(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 126*

---

## senml_set_bver

```fides
pub fn senml_set_bver(r: &mut SenmlRecord, x: i64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `i64`

*Defined at line 131*

---

## senml_set_n

```fides
pub fn senml_set_n(r: &mut SenmlRecord, x: str) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `str`

*Defined at line 136*

---

## senml_set_u

```fides
pub fn senml_set_u(r: &mut SenmlRecord, x: str) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `str`

*Defined at line 141*

---

## senml_set_v

```fides
pub fn senml_set_v(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 146*

---

## senml_set_vs

```fides
pub fn senml_set_vs(r: &mut SenmlRecord, x: str) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `str`

*Defined at line 151*

---

## senml_set_vb

```fides
pub fn senml_set_vb(r: &mut SenmlRecord, x: bool) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `bool`

*Defined at line 156*

---

## senml_set_vd

```fides
pub fn senml_set_vd(r: &mut SenmlRecord, x: Vec<i64>) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `Vec<i64>`

*Defined at line 161*

---

## senml_set_s

```fides
pub fn senml_set_s(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 166*

---

## senml_set_t

```fides
pub fn senml_set_t(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 171*

---

## senml_set_ut

```fides
pub fn senml_set_ut(r: &mut SenmlRecord, x: f64) {
```

**Parameters:**

- `r`: `&mut SenmlRecord`
- `x`: `f64`

*Defined at line 176*

---

## senml_b64url_encode

```fides
pub fn senml_b64url_encode(b: Vec<i64>) -> str {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `str`

*Defined at line 187*

---

## senml_b64url_decode

Decode base64url (padding optional); false in `ok` of the result when a
character is outside the alphabet.

```fides
pub fn senml_b64url_decode(s: str, ok: &mut bool) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`
- `ok`: `&mut bool`

**Returns:** `Vec<i64>`

*Defined at line 207*

---

## senml_num

A number as JSON text: integral values without a fraction.

```fides
pub fn senml_num(x: f64) -> str {
```

**Parameters:**

- `x`: `f64`

**Returns:** `str`

*Defined at line 235*

---

## senml_json_record

One record as a JSON object.

```fides
pub fn senml_json_record(r: &SenmlRecord) -> str {
```

**Parameters:**

- `r`: `&SenmlRecord`

**Returns:** `str`

*Defined at line 253*

---

## senml_json_encode

A pack as compact JSON: [{...},{...}]. "" when a number is NaN or
infinite (JSON has no such numbers).

```fides
pub fn senml_json_encode(records: Vec<SenmlRecord>) -> str {
```

**Parameters:**

- `records`: `Vec<SenmlRecord>`

**Returns:** `str`

*Defined at line 310*

---

## senml_json_decode

Read a JSON pack.

```fides
pub fn senml_json_decode(text: str) -> SenmlPack {
```

**Parameters:**

- `text`: `str`

**Returns:** `SenmlPack`

*Defined at line 334*

---

## senml_cbor_key

The CBOR map key of a label (Table 4), or 99 for none.

```fides
pub fn senml_cbor_key(label: str) -> i64 {
```

**Parameters:**

- `label`: `str`

**Returns:** `i64`

*Defined at line 456*

---

## senml_cbor_put_num

A number in CBOR: an integer when integral (and within +-2^53), else
the shortest float that keeps it.

```fides
pub fn senml_cbor_put_num(out: &mut Vec<i64>, x: f64) {
```

**Parameters:**

- `out`: `&mut Vec<i64>`
- `x`: `f64`

*Defined at line 556*

---

## senml_cbor_encode

A pack as CBOR: a definite-length array of maps.

```fides
pub fn senml_cbor_encode(records: Vec<SenmlRecord>) -> Vec<i64> {
```

**Parameters:**

- `records`: `Vec<SenmlRecord>`

**Returns:** `Vec<i64>`

*Defined at line 642*

---

## senml_cbor_decode

Read a CBOR pack (keys: Table 4 integers, or the JSON labels as text).

```fides
pub fn senml_cbor_decode(bytes: Vec<i64>) -> SenmlPack {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `SenmlPack`

*Defined at line 655*

---

## senml_name_ok

Whether a resolved name is valid (RFC 8428 4.5.1): it starts with a
letter or digit and holds only letters, digits and - : . / _

```fides
pub fn senml_name_ok(name: str) -> bool {
```

**Parameters:**

- `name`: `str`

**Returns:** `bool`

*Defined at line 738*

---

## senml_resolve

Resolve a pack: each record gets its full name, absolute time (`now`
is the current time in seconds since 1970, for relative times), unit,
value and sum; base fields are gone (bver stays when it is not 10).
Records with no value and no sum are dropped; the rest are sorted by
time, keeping the pack's order for equal times.

```fides
pub fn senml_resolve(records: Vec<SenmlRecord>, now: f64) -> SenmlPack {
```

**Parameters:**

- `records`: `Vec<SenmlRecord>`
- `now`: `f64`

**Returns:** `SenmlPack`

*Defined at line 765*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
