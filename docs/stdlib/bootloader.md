[All modules](index.md)

# bootloader -- Fides Documentation

## Contents

- [BL_HDR_SIZE:](#BL_HDR_SIZE:) (const)
- [BL_SIG_SIZE:](#BL_SIG_SIZE:) (const)
- [SLOT_EMPTY:](#SLOT_EMPTY:) (const)
- [SLOT_PENDING:](#SLOT_PENDING:) (const)
- [SLOT_CONFIRMED:](#SLOT_CONFIRMED:) (const)
- [SLOT_BAD:](#SLOT_BAD:) (const)
- [bl_state_name](#bl_state_name) (function)
- [bl_crc32](#bl_crc32) (function)
- [ImageHeader](#ImageHeader) (struct)
- [bl_version_key](#bl_version_key) (function)
- [bl_version_str](#bl_version_str) (function)
- [bl_header_bytes](#bl_header_bytes) (function)
- [ImageCheck](#ImageCheck) (struct)
- [bl_parse_header](#bl_parse_header) (function)
- [bl_build_image](#bl_build_image) (function)
- [bl_verify_image](#bl_verify_image) (function)
- [SlotMeta](#SlotMeta) (struct)
- [BootMeta](#BootMeta) (struct)
- [bl_meta_new](#bl_meta_new) (function)
- [bl_install](#bl_install) (function)
- [bl_meta_encode](#bl_meta_encode) (function)
- [MetaRead](#MetaRead) (struct)
- [bl_meta_decode](#bl_meta_decode) (function)
- [MetaPick](#MetaPick) (struct)
- [bl_meta_pick](#bl_meta_pick) (function)
- [bl_meta_next_copy](#bl_meta_next_copy) (function)
- [BootDecision](#BootDecision) (struct)
- [bl_decide](#bl_decide) (function)
- [bl_confirm](#bl_confirm) (function)
- [bl_meta_str](#bl_meta_str) (function)

## BL_HDR_SIZE:

```fides
pub const BL_HDR_SIZE: i64 = 48;
```

*Defined at line 40*

---

## BL_SIG_SIZE:

```fides
pub const BL_SIG_SIZE: i64 = 64;
```

*Defined at line 41*

---

## SLOT_EMPTY:

```fides
pub const SLOT_EMPTY: i64 = 0;
```

*Defined at line 43*

---

## SLOT_PENDING:

```fides
pub const SLOT_PENDING: i64 = 1;
```

*Defined at line 44*

---

## SLOT_CONFIRMED:

```fides
pub const SLOT_CONFIRMED: i64 = 2;
```

*Defined at line 45*

---

## SLOT_BAD:

```fides
pub const SLOT_BAD: i64 = 3;
```

*Defined at line 46*

---

## bl_state_name

```fides
pub fn bl_state_name(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 48*

---

## bl_crc32

```fides
pub fn bl_crc32(bytes: Vec<i64>, from: i64, n: i64) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `from`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 86*

---

## ImageHeader

```fides
pub struct ImageHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `major` | `i64` |
| `minor` | `i64` |
| `patch` | `i64` |
| `build` | `i64` |
| `counter` | `i64` |
| `size` | `i64` |
| `load_addr` | `i64` |
| `entry` | `i64` |
| `crc` | `i64` |
| `flags` | `i64` |

*Defined at line 107*

---

## bl_version_key

Version as one comparable number.

```fides
pub fn bl_version_key(h: ImageHeader) -> i64 {
```

**Parameters:**

- `h`: `ImageHeader`

**Returns:** `i64`

*Defined at line 121*

---

## bl_version_str

```fides
pub fn bl_version_str(h: ImageHeader) -> str {
```

**Parameters:**

- `h`: `ImageHeader`

**Returns:** `str`

*Defined at line 125*

---

## bl_header_bytes

```fides
pub fn bl_header_bytes(h: ImageHeader) -> Vec<i64> {
```

**Parameters:**

- `h`: `ImageHeader`

**Returns:** `Vec<i64>`

*Defined at line 129*

---

## ImageCheck

Parsed header; `ok` false (and `why`) when it is not one.

```fides
pub struct ImageCheck {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `why` | `str` |
| `hdr` | `ImageHeader` |

*Defined at line 154*

---

## bl_parse_header

```fides
pub fn bl_parse_header(img: Vec<i64>) -> ImageCheck {
```

**Parameters:**

- `img`: `Vec<i64>`

**Returns:** `ImageCheck`

*Defined at line 166*

---

## bl_build_image

Build a signed image: the header is completed with the payload size
and CRC, then header || payload is signed with the 32-byte seed.

```fides
pub fn bl_build_image(h0: ImageHeader, payload: Vec<i64>, seed: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `h0`: `ImageHeader`
- `payload`: `Vec<i64>`
- `seed`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 199*

---

## bl_verify_image

Full check of an image: header, CRC, then the Ed25519 signature.

```fides
pub fn bl_verify_image(img: Vec<i64>, public_key: Vec<i64>) -> ImageCheck {
```

**Parameters:**

- `img`: `Vec<i64>`
- `public_key`: `Vec<i64>`

**Returns:** `ImageCheck`

*Defined at line 226*

---

## SlotMeta

```fides
pub struct SlotMeta {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `tries` | `i64` |
| `version` | `i64` |
| `counter` | `i64` |

*Defined at line 261*

---

## BootMeta

```fides
pub struct BootMeta {
```

**Fields:**

| Name | Type |
|------|------|
| `generation` | `i64` |
| `min_counter` | `i64` |
| `slots` | `Vec<SlotMeta>` |
| `boots` | `i64` |

*Defined at line 268*

---

## bl_meta_new

```fides
pub fn bl_meta_new(nslots: i64) -> BootMeta {
```

**Parameters:**

- `nslots`: `i64`

**Returns:** `BootMeta`

*Defined at line 275*

---

## bl_install

Record that `img` was written to `slot`: pending with `tries` trial
boots (tries 0: install as confirmed, for factory images).

```fides
pub fn bl_install(m: &mut BootMeta, slot: i64, img: Vec<i64>, tries: i64) {
```

**Parameters:**

- `m`: `&mut BootMeta`
- `slot`: `i64`
- `img`: `Vec<i64>`
- `tries`: `i64`

*Defined at line 291*

---

## bl_meta_encode

Serialized metadata copy: "FDBM", generation, min counter, boots,
slot count, per slot state/tries/version/counter, CRC-32 of the rest.

```fides
pub fn bl_meta_encode(m: BootMeta) -> Vec<i64> {
```

**Parameters:**

- `m`: `BootMeta`

**Returns:** `Vec<i64>`

*Defined at line 307*

---

## MetaRead

```fides
pub struct MetaRead {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `meta` | `BootMeta` |

*Defined at line 330*

---

## bl_meta_decode

```fides
pub fn bl_meta_decode(b: Vec<i64>) -> MetaRead {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `MetaRead`

*Defined at line 335*

---

## MetaPick

The valid copy with the newest generation; index 0 or 1 in `which`,
-1 when neither is valid.

```fides
pub struct MetaPick {
```

**Fields:**

| Name | Type |
|------|------|
| `which` | `i64` |
| `meta` | `BootMeta` |

*Defined at line 371*

---

## bl_meta_pick

```fides
pub fn bl_meta_pick(copy0: Vec<i64>, copy1: Vec<i64>) -> MetaPick {
```

**Parameters:**

- `copy0`: `Vec<i64>`
- `copy1`: `Vec<i64>`

**Returns:** `MetaPick`

*Defined at line 376*

---

## bl_meta_next_copy

Which copy an update should overwrite: the one not picked, so the
picked copy survives a power cut during the write.

```fides
pub fn bl_meta_next_copy(p: MetaPick) -> i64 {
```

**Parameters:**

- `p`: `MetaPick`

**Returns:** `i64`

*Defined at line 396*

---

## BootDecision

```fides
pub struct BootDecision {
```

**Fields:**

| Name | Type |
|------|------|
| `slot` | `i64` |
| `trial` | `bool` |
| `reason` | `str` |
| `meta` | `BootMeta` |
| `log` | `Vec<str>` |

*Defined at line 405*

---

## bl_decide

Decide what to boot from slot images `imgs` and the metadata. Returns
the slot (-1: nothing bootable, stay in recovery), whether it is a
trial boot, why, and the metadata to write back (generation advanced,
trial counts used, failed slots marked bad).

```fides
pub fn bl_decide(m0: BootMeta, imgs: Vec<Vec<i64>>, pk: Vec<i64>) -> BootDecision {
```

**Parameters:**

- `m0`: `BootMeta`
- `imgs`: `Vec<Vec<i64>>`
- `pk`: `Vec<i64>`

**Returns:** `BootDecision`

*Defined at line 430*

---

## bl_confirm

The running image in `slot` is healthy: confirm it and raise the
anti-rollback minimum to its security counter.

```fides
pub fn bl_confirm(m0: BootMeta, slot: i64) -> BootMeta {
```

**Parameters:**

- `m0`: `BootMeta`
- `slot`: `i64`

**Returns:** `BootMeta`

*Defined at line 489*

---

## bl_meta_str

```fides
pub fn bl_meta_str(m: BootMeta) -> str {
```

**Parameters:**

- `m`: `BootMeta`

**Returns:** `str`

*Defined at line 502*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
