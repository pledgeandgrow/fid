[All modules](index.md)

# flash -- Fides Documentation

## Contents

- [flash_crc32](#flash_crc32) (function)
- [FlashDev](#FlashDev) (struct)
- [flash_new](#flash_new) (function)
- [flash_size](#flash_size) (function)
- [flash_erase](#flash_erase) (function)
- [flash_read](#flash_read) (function)
- [flash_read_bytes](#flash_read_bytes) (function)
- [flash_program](#flash_program) (function)
- [flash_power_on](#flash_power_on) (function)
- [flash_wear](#flash_wear) (function)
- [FKV_REC_MAGIC:](#FKV_REC_MAGIC:) (const)
- [FKV_COMMIT:](#FKV_COMMIT:) (const)
- [FKV_TOMBSTONE:](#FKV_TOMBSTONE:) (const)
- [FlashKv](#FlashKv) (struct)
- [fkv_record_size](#fkv_record_size) (function)
- [fkv_mount](#fkv_mount) (function)
- [fkv_put](#fkv_put) (function)
- [fkv_delete](#fkv_delete) (function)
- [fkv_has](#fkv_has) (function)
- [fkv_get](#fkv_get) (function)
- [fkv_get_str](#fkv_get_str) (function)
- [fkv_put_str](#fkv_put_str) (function)
- [fkv_count](#fkv_count) (function)

## flash_crc32

```fides
pub fn flash_crc32(bytes: Vec<i64>, from: i64, n: i64, seed: i64) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`
- `from`: `i64`
- `n`: `i64`
- `seed`: `i64`

**Returns:** `i64`

*Defined at line 44*

---

## FlashDev

```fides
pub struct FlashDev {
```

**Fields:**

| Name | Type |
|------|------|
| `page_size` | `i64` |
| `pages` | `i64` |
| `unit` | `i64` |
| `once` | `bool` |
| `data` | `Vec<i64>` |
| `erases` | `Vec<i64>` |
| `fail_after` | `i64` |
| `dead` | `bool` |
| `programmed` | `i64` |
| `error` | `str` |

*Defined at line 65*

---

## flash_new

A device of `pages` pages, erased.

```fides
pub fn flash_new(page_size: i64, pages: i64, unit: i64, once: bool) -> FlashDev {
```

**Parameters:**

- `page_size`: `i64`
- `pages`: `i64`
- `unit`: `i64`
- `once`: `bool`

**Returns:** `FlashDev`

*Defined at line 79*

---

## flash_size

```fides
pub fn flash_size(f: &FlashDev) -> i64 {
```

**Parameters:**

- `f`: `&FlashDev`

**Returns:** `i64`

*Defined at line 88*

---

## flash_erase

Erase one page (atomic in this model). False when powered off.

```fides
pub fn flash_erase(f: &mut FlashDev, page: i64) -> bool {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `page`: `i64`

**Returns:** `bool`

*Defined at line 93*

---

## flash_read

```fides
pub fn flash_read(f: &FlashDev, addr: i64) -> i64 {
```

**Parameters:**

- `f`: `&FlashDev`
- `addr`: `i64`

**Returns:** `i64`

*Defined at line 112*

---

## flash_read_bytes

```fides
pub fn flash_read_bytes(f: &FlashDev, addr: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `f`: `&FlashDev`
- `addr`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 116*

---

## flash_program

Program `bytes` at `addr` (unit-aligned, a whole number of units).
Checks first (nothing is written when a rule is broken): every byte
must only clear bits, and with `once` every unit must still be erased.
Then the bytes are written one by one; an injected power cut stops it
part way (f.dead, false).

```fides
pub fn flash_program(f: &mut FlashDev, addr: i64, bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `addr`: `i64`
- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 131*

---

## flash_power_on

Power comes back: the device works again (contents as they were).

```fides
pub fn flash_power_on(f: &mut FlashDev) {
```

**Parameters:**

- `f`: `&mut FlashDev`

*Defined at line 187*

---

## flash_wear

Smallest and largest erase count over the pages, as [min, max].

```fides
pub fn flash_wear(f: &FlashDev) -> Vec<i64> {
```

**Parameters:**

- `f`: `&FlashDev`

**Returns:** `Vec<i64>`

*Defined at line 194*

---

## FKV_REC_MAGIC:

```fides
pub const FKV_REC_MAGIC: i64 = 0x4B;
```

*Defined at line 216*

---

## FKV_COMMIT:

```fides
pub const FKV_COMMIT: i64 = 0x5A;
```

*Defined at line 217*

---

## FKV_TOMBSTONE:

```fides
pub const FKV_TOMBSTONE: i64 = 0x01;
```

*Defined at line 218*

---

## FlashKv

```fides
pub struct FlashKv {
```

**Fields:**

| Name | Type |
|------|------|
| `first_page` | `i64` |
| `npages` | `i64` |
| `active` | `i64` |
| `seq` | `i64` |
| `woff` | `i64` |
| `index` | `HashMap<str` |
| `torn` | `i64` |
| `compactions` | `i64` |
| `error` | `str` |

*Defined at line 220*

---

## fkv_record_size

Bytes a record with these lengths takes, commit unit included.

```fides
pub fn fkv_record_size(f: &FlashDev, klen: i64, vlen: i64) -> i64 {
```

**Parameters:**

- `f`: `&FlashDev`
- `klen`: `i64`
- `vlen`: `i64`

**Returns:** `i64`

*Defined at line 242*

---

## fkv_mount

Mount the store on pages [first_page, first_page + npages): replay
the pages oldest first, erase pages that are not store pages, format
an empty device, and finish an interrupted compaction.

```fides
pub fn fkv_mount(f: &mut FlashDev, first_page: i64, npages: i64) -> FlashKv {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `first_page`: `i64`
- `npages`: `i64`

**Returns:** `FlashKv`

*Defined at line 399*

---

## fkv_put

Store `val` under `key`. False on error (kv.error says why).

```fides
pub fn fkv_put(f: &mut FlashDev, kv: &mut FlashKv, key: str, val: Vec<i64>) -> bool {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `kv`: `&mut FlashKv`
- `key`: `str`
- `val`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 595*

---

## fkv_delete

Delete `key` (a tombstone record). True when it existed.

```fides
pub fn fkv_delete(f: &mut FlashDev, kv: &mut FlashKv, key: str) -> bool {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `kv`: `&mut FlashKv`
- `key`: `str`

**Returns:** `bool`

*Defined at line 615*

---

## fkv_has

```fides
pub fn fkv_has(kv: &FlashKv, key: str) -> bool {
```

**Parameters:**

- `kv`: `&FlashKv`
- `key`: `str`

**Returns:** `bool`

*Defined at line 633*

---

## fkv_get

The value of `key` (empty when absent: check fkv_has).

```fides
pub fn fkv_get(f: &FlashDev, kv: &FlashKv, key: str) -> Vec<i64> {
```

**Parameters:**

- `f`: `&FlashDev`
- `kv`: `&FlashKv`
- `key`: `str`

**Returns:** `Vec<i64>`

*Defined at line 638*

---

## fkv_get_str

Value as text (for string values).

```fides
pub fn fkv_get_str(f: &FlashDev, kv: &FlashKv, key: str) -> str {
```

**Parameters:**

- `f`: `&FlashDev`
- `kv`: `&FlashKv`
- `key`: `str`

**Returns:** `str`

*Defined at line 646*

---

## fkv_put_str

```fides
pub fn fkv_put_str(f: &mut FlashDev, kv: &mut FlashKv, key: str, s: str) -> bool {
```

**Parameters:**

- `f`: `&mut FlashDev`
- `kv`: `&mut FlashKv`
- `key`: `str`
- `s`: `str`

**Returns:** `bool`

*Defined at line 657*

---

## fkv_count

```fides
pub fn fkv_count(kv: &FlashKv) -> i64 {
```

**Parameters:**

- `kv`: `&FlashKv`

**Returns:** `i64`

*Defined at line 661*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
