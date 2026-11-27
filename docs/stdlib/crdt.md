[All modules](index.md)

# crdt -- Fides Documentation

## Contents

- [crdt_esc](#crdt_esc) (function)
- [crdt_unesc](#crdt_unesc) (function)
- [CRDT_EQUAL:](#CRDT_EQUAL:) (const)
- [CRDT_BEFORE:](#CRDT_BEFORE:) (const)
- [CRDT_AFTER:](#CRDT_AFTER:) (const)
- [CRDT_CONCURRENT:](#CRDT_CONCURRENT:) (const)
- [VClock](#VClock) (struct)
- [vclock_new](#vclock_new) (function)
- [vclock_get](#vclock_get) (function)
- [vclock_set](#vclock_set) (function)
- [vclock_tick](#vclock_tick) (function)
- [vclock_merge](#vclock_merge) (function)
- [vclock_compare](#vclock_compare) (function)
- [vclock_encode](#vclock_encode) (function)
- [vclock_decode](#vclock_decode) (function)
- [GCounter](#GCounter) (struct)
- [gcounter_new](#gcounter_new) (function)
- [gcounter_inc](#gcounter_inc) (function)
- [gcounter_value](#gcounter_value) (function)
- [gcounter_merge](#gcounter_merge) (function)
- [gcounter_encode](#gcounter_encode) (function)
- [gcounter_decode](#gcounter_decode) (function)
- [PNCounter](#PNCounter) (struct)
- [pncounter_new](#pncounter_new) (function)
- [pncounter_add](#pncounter_add) (function)
- [pncounter_value](#pncounter_value) (function)
- [pncounter_merge](#pncounter_merge) (function)
- [pncounter_encode](#pncounter_encode) (function)
- [pncounter_decode](#pncounter_decode) (function)
- [LwwRegister](#LwwRegister) (struct)
- [lww_new](#lww_new) (function)
- [lww_set](#lww_set) (function)
- [lww_merge](#lww_merge) (function)
- [lww_encode](#lww_encode) (function)
- [lww_decode](#lww_decode) (function)
- [ORSet](#ORSet) (struct)
- [orset_new](#orset_new) (function)
- [orset_add](#orset_add) (function)
- [orset_remove](#orset_remove) (function)
- [orset_contains](#orset_contains) (function)
- [orset_values](#orset_values) (function)
- [orset_merge](#orset_merge) (function)
- [orset_delta](#orset_delta) (function)
- [orset_encode](#orset_encode) (function)
- [orset_decode](#orset_decode) (function)
- [RgaElem](#RgaElem) (struct)
- [Rga](#Rga) (struct)
- [rga_new](#rga_new) (function)
- [rga_integrate](#rga_integrate) (function)
- [rga_insert](#rga_insert) (function)
- [rga_insert_text](#rga_insert_text) (function)
- [rga_delete_id](#rga_delete_id) (function)
- [rga_delete_at](#rga_delete_at) (function)
- [rga_text](#rga_text) (function)
- [rga_merge](#rga_merge) (function)
- [rga_delta](#rga_delta) (function)
- [rga_encode](#rga_encode) (function)
- [rga_decode_into](#rga_decode_into) (function)

## crdt_esc

Escape '%', '|', ',', '=' and newlines as %XX.

```fides
pub fn crdt_esc(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 33*

---

## crdt_unesc

```fides
pub fn crdt_unesc(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 71*

---

## CRDT_EQUAL:

```fides
pub const CRDT_EQUAL: i64 = 0;
```

*Defined at line 94*

---

## CRDT_BEFORE:

```fides
pub const CRDT_BEFORE: i64 = 1;
```

*Defined at line 95*

---

## CRDT_AFTER:

```fides
pub const CRDT_AFTER: i64 = 2;
```

*Defined at line 96*

---

## CRDT_CONCURRENT:

```fides
pub const CRDT_CONCURRENT: i64 = 3;
```

*Defined at line 97*

---

## VClock

```fides
pub struct VClock {
```

**Fields:**

| Name | Type |
|------|------|
| `nodes` | `Vec<str>` |
| `counts` | `Vec<i64>` |

*Defined at line 99*

---

## vclock_new

```fides
pub fn vclock_new() -> VClock {
```

**Returns:** `VClock`

*Defined at line 104*

---

## vclock_get

```fides
pub fn vclock_get(c: &VClock, node: str) -> i64 {
```

**Parameters:**

- `c`: `&VClock`
- `node`: `str`

**Returns:** `i64`

*Defined at line 110*

---

## vclock_set

```fides
pub fn vclock_set(c: &mut VClock, node: str, v: i64) {
```

**Parameters:**

- `c`: `&mut VClock`
- `node`: `str`
- `v`: `i64`

*Defined at line 121*

---

## vclock_tick

```fides
pub fn vclock_tick(c: &mut VClock, node: str) -> i64 {
```

**Parameters:**

- `c`: `&mut VClock`
- `node`: `str`

**Returns:** `i64`

*Defined at line 139*

---

## vclock_merge

```fides
pub fn vclock_merge(a: &mut VClock, b: &VClock) {
```

**Parameters:**

- `a`: `&mut VClock`
- `b`: `&VClock`

*Defined at line 145*

---

## vclock_compare

How a relates to b.

```fides
pub fn vclock_compare(a: &VClock, b: &VClock) -> i64 {
```

**Parameters:**

- `a`: `&VClock`
- `b`: `&VClock`

**Returns:** `i64`

*Defined at line 157*

---

## vclock_encode

```fides
pub fn vclock_encode(c: &VClock) -> str {
```

**Parameters:**

- `c`: `&VClock`

**Returns:** `str`

*Defined at line 196*

---

## vclock_decode

```fides
pub fn vclock_decode(s: str) -> VClock {
```

**Parameters:**

- `s`: `str`

**Returns:** `VClock`

*Defined at line 209*

---

## GCounter

A G-Counter is a vector clock read as per-node counts.

```fides
pub struct GCounter {
```

**Fields:**

| Name | Type |
|------|------|
| `c` | `VClock` |

*Defined at line 230*

---

## gcounter_new

```fides
pub fn gcounter_new() -> GCounter {
```

**Returns:** `GCounter`

*Defined at line 234*

---

## gcounter_inc

```fides
pub fn gcounter_inc(g: &mut GCounter, node: str, n: i64) {
```

**Parameters:**

- `g`: `&mut GCounter`
- `node`: `str`
- `n`: `i64`

*Defined at line 240*

---

## gcounter_value

```fides
pub fn gcounter_value(g: &GCounter) -> i64 {
```

**Parameters:**

- `g`: `&GCounter`

**Returns:** `i64`

*Defined at line 247*

---

## gcounter_merge

```fides
pub fn gcounter_merge(a: &mut GCounter, b: &GCounter) {
```

**Parameters:**

- `a`: `&mut GCounter`
- `b`: `&GCounter`

*Defined at line 257*

---

## gcounter_encode

```fides
pub fn gcounter_encode(g: &GCounter) -> str {
```

**Parameters:**

- `g`: `&GCounter`

**Returns:** `str`

*Defined at line 260*

---

## gcounter_decode

```fides
pub fn gcounter_decode(s: str) -> GCounter {
```

**Parameters:**

- `s`: `str`

**Returns:** `GCounter`

*Defined at line 263*

---

## PNCounter

```fides
pub struct PNCounter {
```

**Fields:**

| Name | Type |
|------|------|
| `p` | `GCounter` |
| `n` | `GCounter` |

*Defined at line 272*

---

## pncounter_new

```fides
pub fn pncounter_new() -> PNCounter {
```

**Returns:** `PNCounter`

*Defined at line 277*

---

## pncounter_add

```fides
pub fn pncounter_add(c: &mut PNCounter, node: str, delta: i64) {
```

**Parameters:**

- `c`: `&mut PNCounter`
- `node`: `str`
- `delta`: `i64`

*Defined at line 283*

---

## pncounter_value

```fides
pub fn pncounter_value(c: &PNCounter) -> i64 {
```

**Parameters:**

- `c`: `&PNCounter`

**Returns:** `i64`

*Defined at line 291*

---

## pncounter_merge

```fides
pub fn pncounter_merge(a: &mut PNCounter, b: &PNCounter) {
```

**Parameters:**

- `a`: `&mut PNCounter`
- `b`: `&PNCounter`

*Defined at line 295*

---

## pncounter_encode

```fides
pub fn pncounter_encode(c: &PNCounter) -> str {
```

**Parameters:**

- `c`: `&PNCounter`

**Returns:** `str`

*Defined at line 300*

---

## pncounter_decode

```fides
pub fn pncounter_decode(s: str) -> PNCounter {
```

**Parameters:**

- `s`: `str`

**Returns:** `PNCounter`

*Defined at line 304*

---

## LwwRegister

```fides
pub struct LwwRegister {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `str` |
| `ts` | `i64` |
| `node` | `str` |

*Defined at line 320*

---

## lww_new

```fides
pub fn lww_new() -> LwwRegister {
```

**Returns:** `LwwRegister`

*Defined at line 326*

---

## lww_set

Set at (ts, node); ignored when an equal or later write is known.

```fides
pub fn lww_set(r: &mut LwwRegister, value: str, ts: i64, node: str) {
```

**Parameters:**

- `r`: `&mut LwwRegister`
- `value`: `str`
- `ts`: `i64`
- `node`: `str`

*Defined at line 340*

---

## lww_merge

```fides
pub fn lww_merge(a: &mut LwwRegister, b: &LwwRegister) {
```

**Parameters:**

- `a`: `&mut LwwRegister`
- `b`: `&LwwRegister`

*Defined at line 348*

---

## lww_encode

```fides
pub fn lww_encode(r: &LwwRegister) -> str {
```

**Parameters:**

- `r`: `&LwwRegister`

**Returns:** `str`

*Defined at line 352*

---

## lww_decode

```fides
pub fn lww_decode(s: str) -> LwwRegister {
```

**Parameters:**

- `s`: `str`

**Returns:** `LwwRegister`

*Defined at line 356*

---

## ORSet

```fides
pub struct ORSet {
```

**Fields:**

| Name | Type |
|------|------|
| `elems` | `Vec<str>,  // one entry per add: element and its tag` |
| `tag_node` | `Vec<str>` |
| `tag_seq` | `Vec<i64>` |
| `dead_node` | `Vec<str>,  // tombstoned tags` |
| `dead_seq` | `Vec<i64>` |
| `clock` | `VClock,  // the tags seen, per node` |

*Defined at line 368*

---

## orset_new

```fides
pub fn orset_new() -> ORSet {
```

**Returns:** `ORSet`

*Defined at line 377*

---

## orset_add

```fides
pub fn orset_add(s: &mut ORSet, node: str, e: str) {
```

**Parameters:**

- `s`: `&mut ORSet`
- `node`: `str`
- `e`: `str`

*Defined at line 425*

---

## orset_remove

Remove every tag of `e` this replica has seen.

```fides
pub fn orset_remove(s: &mut ORSet, e: str) {
```

**Parameters:**

- `s`: `&mut ORSet`
- `e`: `str`

*Defined at line 431*

---

## orset_contains

```fides
pub fn orset_contains(s: &ORSet, e: str) -> bool {
```

**Parameters:**

- `s`: `&ORSet`
- `e`: `str`

**Returns:** `bool`

*Defined at line 441*

---

## orset_values

The live elements, sorted, each once.

```fides
pub fn orset_values(s: &ORSet) -> Vec<str> {
```

**Parameters:**

- `s`: `&ORSet`

**Returns:** `Vec<str>`

*Defined at line 453*

---

## orset_merge

```fides
pub fn orset_merge(a: &mut ORSet, b: &ORSet) {
```

**Parameters:**

- `a`: `&mut ORSet`
- `b`: `&ORSet`

*Defined at line 476*

---

## orset_delta

The delta newer than `since`: adds with tags above it, and all
tombstones (they carry no time). Merging it equals merging the state.

```fides
pub fn orset_delta(s: &ORSet, since: &VClock) -> ORSet {
```

**Parameters:**

- `s`: `&ORSet`
- `since`: `&VClock`

**Returns:** `ORSet`

*Defined at line 492*

---

## orset_encode

"or" then one line per add "+node,seq,elem" and per tombstone
"-node,seq".

```fides
pub fn orset_encode(s: &ORSet) -> str {
```

**Parameters:**

- `s`: `&ORSet`

**Returns:** `str`

*Defined at line 511*

---

## orset_decode

```fides
pub fn orset_decode(text: str) -> ORSet {
```

**Parameters:**

- `text`: `str`

**Returns:** `ORSet`

*Defined at line 526*

---

## RgaElem

```fides
pub struct RgaElem {
```

**Fields:**

| Name | Type |
|------|------|
| `seq` | `i64,  // id: (seq, node)` |
| `node` | `str` |
| `origin_seq` | `i64,  // the element it was inserted after (0: head)` |
| `origin_node` | `str` |
| `value` | `str` |
| `deleted` | `bool` |

*Defined at line 549*

---

## Rga

```fides
pub struct Rga {
```

**Fields:**

| Name | Type |
|------|------|
| `elems` | `Vec<RgaElem>,  // document order` |
| `max_seq` | `i64` |
| `clock` | `VClock` |

*Defined at line 558*

---

## rga_new

```fides
pub fn rga_new() -> Rga {
```

**Returns:** `Rga`

*Defined at line 564*

---

## rga_integrate

Integrate an element (idempotent): after its origin, skipping the
elements with greater ids (concurrent inserts that win, and theirs).
False when its origin is not known yet.

```fides
pub fn rga_integrate(r: &mut Rga, e: RgaElem) -> bool {
```

**Parameters:**

- `r`: `&mut Rga`
- `e`: `RgaElem`

**Returns:** `bool`

*Defined at line 592*

---

## rga_insert

Insert `value` at visible position `pos` (0 = front) by `node`;
returns the new element (the operation to send).

```fides
pub fn rga_insert(r: &mut Rga, node: str, pos: i64, value: str) -> RgaElem {
```

**Parameters:**

- `r`: `&mut Rga`
- `node`: `str`
- `pos`: `i64`
- `value`: `str`

**Returns:** `RgaElem`

*Defined at line 635*

---

## rga_insert_text

Insert each character of `text` from position `pos`.

```fides
pub fn rga_insert_text(r: &mut Rga, node: str, pos: i64, text: str) {
```

**Parameters:**

- `r`: `&mut Rga`
- `node`: `str`
- `pos`: `i64`
- `text`: `str`

*Defined at line 658*

---

## rga_delete_id

Delete by id (an operation); false when unknown.

```fides
pub fn rga_delete_id(r: &mut Rga, seq: i64, node: str) -> bool {
```

**Parameters:**

- `r`: `&mut Rga`
- `seq`: `i64`
- `node`: `str`

**Returns:** `bool`

*Defined at line 667*

---

## rga_delete_at

Delete the visible element at `pos`; its id (seq) or -1.

```fides
pub fn rga_delete_at(r: &mut Rga, pos: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut Rga`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 679*

---

## rga_text

```fides
pub fn rga_text(r: &Rga) -> str {
```

**Parameters:**

- `r`: `&Rga`

**Returns:** `str`

*Defined at line 690*

---

## rga_merge

Merge another replica (or a delta): its elements in id order (origins
always have smaller ids), then its tombstones.

```fides
pub fn rga_merge(a: &mut Rga, b: &Rga) {
```

**Parameters:**

- `a`: `&mut Rga`
- `b`: `&Rga`

*Defined at line 719*

---

## rga_delta

The delta newer than `since`: elements with ids above it, plus every
tombstoned element (so deletes travel).

```fides
pub fn rga_delta(r: &Rga, since: &VClock) -> Rga {
```

**Parameters:**

- `r`: `&Rga`
- `since`: `&VClock`

**Returns:** `Rga`

*Defined at line 737*

---

## rga_encode

"rga" then one line per element "seq,node,oseq,onode,deleted,value".

```fides
pub fn rga_encode(r: &Rga) -> str {
```

**Parameters:**

- `r`: `&Rga`

**Returns:** `str`

*Defined at line 752*

---

## rga_decode_into

Parse rga_encode text into a replica (merged in id order, so a delta
whose origins the receiver already has decodes too).

```fides
pub fn rga_decode_into(r: &mut Rga, text: str) -> bool {
```

**Parameters:**

- `r`: `&mut Rga`
- `text`: `str`

**Returns:** `bool`

*Defined at line 769*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
