[All modules](index.md)

# virtio -- Fides Documentation

## Contents

- [VQ_F_NEXT](#VQ_F_NEXT) (function)
- [VQ_F_WRITE](#VQ_F_WRITE) (function)
- [VQ_F_INDIRECT](#VQ_F_INDIRECT) (function)
- [vq_desc_off](#vq_desc_off) (function)
- [vq_avail_off](#vq_avail_off) (function)
- [vq_used_off](#vq_used_off) (function)
- [vq_total_size](#vq_total_size) (function)
- [vq_desc](#vq_desc) (function)
- [vq_desc_flags](#vq_desc_flags) (function)
- [vq_avail_idx](#vq_avail_idx) (function)
- [vq_avail_push](#vq_avail_push) (function)
- [vq_used_idx](#vq_used_idx) (function)
- [vq_used_id](#vq_used_id) (function)
- [vq_used_len](#vq_used_len) (function)
- [VqFree](#VqFree) (struct)
- [vq_free_new](#vq_free_new) (function)
- [vq_free_take](#vq_free_take) (function)
- [vq_free_chain](#vq_free_chain) (function)

## VQ_F_NEXT

```fides
pub fn VQ_F_NEXT() -> i64 {
```

**Returns:** `i64`

*Defined at line 9*

---

## VQ_F_WRITE

```fides
pub fn VQ_F_WRITE() -> i64 {
```

**Returns:** `i64`

*Defined at line 12*

---

## VQ_F_INDIRECT

```fides
pub fn VQ_F_INDIRECT() -> i64 {
```

**Returns:** `i64`

*Defined at line 15*

---

## vq_desc_off

Byte offset of the descriptor table inside a queue block.

```fides
pub fn vq_desc_off() -> i64 {
```

**Returns:** `i64`

*Defined at line 22*

---

## vq_avail_off

Byte offset of the avail ring (driver area) — right after 16-byte
descriptors.

```fides
pub fn vq_avail_off(qsz: i64) -> i64 {
```

**Parameters:**

- `qsz`: `i64`

**Returns:** `i64`

*Defined at line 28*

---

## vq_used_off

Byte offset of the used ring (device area): avail ring is
4 + 2*qsz + 2 bytes, then padded to the 4-byte alignment used by
virtio's avail-side alignment rule.

```fides
pub fn vq_used_off(qsz: i64) -> i64 {
```

**Parameters:**

- `qsz`: `i64`

**Returns:** `i64`

*Defined at line 35*

---

## vq_total_size

Total queue-block bytes for queue size qsz (used ring is
4 + 8*qsz + 2 bytes of used elems and avail_event).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(vq_avail_off(8), 128);
assert_eq(vq_used_off(8), 152);
var q: Vec<i64> = Vec::filled(vq_total_size(8), 0);
vq_desc(&mut q, 8, 0, 0x1000, 512, VQ_F_WRITE(), 0);
assert_eq(vq_desc_flags(q, 0), VQ_F_WRITE());
vq_avail_push(&mut q, 8, 0);
assert_eq(vq_avail_idx(q, 8), 1);
assert_eq(vq_used_idx(q, 8), 0);
```

```fides
pub fn vq_total_size(qsz: i64) -> i64 {
```

**Parameters:**

- `qsz`: `i64`

**Returns:** `i64`

*Defined at line 56*

---

## vq_desc

Write descriptor i: {addr:u64, len:u32, flags:u16, next:u16}.

```fides
pub fn vq_desc(q: &mut Vec<i64>, _qsz: i64, i: i64, addr: i64, len: i64, flags: i64, next: i64) {
```

**Parameters:**

- `q`: `&mut Vec<i64>`
- `_qsz`: `i64`
- `i`: `i64`
- `addr`: `i64`
- `len`: `i64`
- `flags`: `i64`
- `next`: `i64`

*Defined at line 86*

---

## vq_desc_flags

```fides
pub fn vq_desc_flags(q: Vec<i64>, i: i64) -> i64 {
```

**Parameters:**

- `q`: `Vec<i64>`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## vq_avail_idx

Avail: number of entries pushed so far (idx field).

```fides
pub fn vq_avail_idx(q: Vec<i64>, qsz: i64) -> i64 {
```

**Parameters:**

- `q`: `Vec<i64>`
- `qsz`: `i64`

**Returns:** `i64`

*Defined at line 99*

---

## vq_avail_push

Push descriptor head `head` into the avail ring.

```fides
pub fn vq_avail_push(q: &mut Vec<i64>, qsz: i64, head: i64) {
```

**Parameters:**

- `q`: `&mut Vec<i64>`
- `qsz`: `i64`
- `head`: `i64`

*Defined at line 104*

---

## vq_used_idx

Used: number of completions the device has written.

```fides
pub fn vq_used_idx(q: Vec<i64>, qsz: i64) -> i64 {
```

**Parameters:**

- `q`: `Vec<i64>`
- `qsz`: `i64`

**Returns:** `i64`

*Defined at line 112*

---

## vq_used_id

Read used element i as id*4294967296+len... not portable for len
near 2^31; use vq_used_id / vq_used_len instead.

```fides
pub fn vq_used_id(q: Vec<i64>, qsz: i64, i: i64) -> i64 {
```

**Parameters:**

- `q`: `Vec<i64>`
- `qsz`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 118*

---

## vq_used_len

```fides
pub fn vq_used_len(q: Vec<i64>, qsz: i64, i: i64) -> i64 {
```

**Parameters:**

- `q`: `Vec<i64>`
- `qsz`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 122*

---

## VqFree

A chain-head tracker for the driver side: which descriptor ids are
free, and the head of the free list.

```fides
pub struct VqFree {
```

**Fields:**

| Name | Type |
|------|------|
| `qsz` | `i64` |
| `head` | `i64` |
| `count` | `i64` |
| `next` | `Vec<i64>,  // next[i] = next free desc or -1` |

*Defined at line 128*

---

## vq_free_new

All qsz descriptors initially free (0 -> 1 -> ... -> -1).

```fides
pub fn vq_free_new(qsz: i64) -> VqFree {
```

**Parameters:**

- `qsz`: `i64`

**Returns:** `VqFree`

*Defined at line 137*

---

## vq_free_take

Take n free descriptor ids into a caller Vec; returns false when the
queue has fewer than n.

```fides
pub fn vq_free_take(f: &mut VqFree, n: i64, into: &mut Vec<i64>) -> bool {
```

**Parameters:**

- `f`: `&mut VqFree`
- `n`: `i64`
- `into`: `&mut Vec<i64>`

**Returns:** `bool`

*Defined at line 155*

---

## vq_free_chain

Return a chain head's descriptors to the free list (follows NEXT
flags in the queue block).

```fides
pub fn vq_free_chain(f: &mut VqFree, q: Vec<i64>, head: i64) {
```

**Parameters:**

- `f`: `&mut VqFree`
- `q`: `Vec<i64>`
- `head`: `i64`

*Defined at line 172*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
