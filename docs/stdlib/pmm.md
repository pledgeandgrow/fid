[All modules](index.md)

# pmm -- Fides Documentation

## Contents

- [PAGE_SIZE](#PAGE_SIZE) (function)
- [Pmm](#Pmm) (struct)
- [pmm_new](#pmm_new) (function)
- [pmm_mark](#pmm_mark) (function)
- [pmm_mark_range](#pmm_mark_range) (function)
- [pmm_free](#pmm_free) (function)
- [pmm_alloc](#pmm_alloc) (function)
- [pmm_frame_addr](#pmm_frame_addr) (function)
- [pmm_addr_frame](#pmm_addr_frame) (function)
- [PT_PRESENT](#PT_PRESENT) (function)
- [PT_WRITE](#PT_WRITE) (function)
- [PT_USER](#PT_USER) (function)
- [PT_PWT](#PT_PWT) (function)
- [PT_PCD](#PT_PCD) (function)
- [PT_HUGE](#PT_HUGE) (function)
- [PT_NX](#PT_NX) (function)
- [pg_pml4i](#pg_pml4i) (function)
- [pg_pdpti](#pg_pdpti) (function)
- [pg_pdi](#pg_pdi) (function)
- [pg_pti](#pg_pti) (function)
- [pg_offset](#pg_offset) (function)
- [pg_entry](#pg_entry) (function)
- [pg_entry_phys](#pg_entry_phys) (function)
- [pg_is_present](#pg_is_present) (function)
- [pg_is_huge](#pg_is_huge) (function)

## PAGE_SIZE

```fides
pub fn PAGE_SIZE() -> i64 {
```

**Returns:** `i64`

*Defined at line 7*

---

## Pmm

```fides
pub struct Pmm {
```

**Fields:**

| Name | Type |
|------|------|
| `total` | `i64` |
| `used` | `i64` |
| `map` | `Vec<i64>` |
| `hint` | `i64,  // last alloc word — scan resumes here` |

*Defined at line 11*

---

## pmm_new

Allocator for `frames` frames, all free. Map bytes = ceil(frames/64).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var p = pmm_new(100);
pmm_mark_range(&mut p, 0, 3);
assert_eq(pmm_alloc(&mut p), 3);
assert_eq(pmm_alloc(&mut p), 4);
pmm_free(&mut p, 1);
assert_eq(pmm_alloc(&mut p), 1);
assert_eq(pmm_frame_addr(2), 8192);
assert_eq(pmm_addr_frame(8200), 2);
assert_eq(PAGE_SIZE(), 4096);
```

```fides
pub fn pmm_new(frames: i64) -> Pmm {
```

**Parameters:**

- `frames`: `i64`

**Returns:** `Pmm`

*Defined at line 32*

---

## pmm_mark

Mark frame f used (no bounds check beyond the map).

```fides
pub fn pmm_mark(p: &mut Pmm, f: i64) {
```

**Parameters:**

- `p`: `&mut Pmm`
- `f`: `i64`

*Defined at line 46*

---

## pmm_mark_range

Mark all frames in [start_frame, start_frame+n) used — for kernel
image, bitmap itself, reserved e820 ranges.

```fides
pub fn pmm_mark_range(p: &mut Pmm, start: i64, n: i64) {
```

**Parameters:**

- `p`: `&mut Pmm`
- `start`: `i64`
- `n`: `i64`

*Defined at line 58*

---

## pmm_free

Free frame f.

```fides
pub fn pmm_free(p: &mut Pmm, f: i64) {
```

**Parameters:**

- `p`: `&mut Pmm`
- `f`: `i64`

*Defined at line 69*

---

## pmm_alloc

First free frame index or -1. Scans whole words, so the bit walk
only runs on words that are not all-ones.

```fides
pub fn pmm_alloc(p: &mut Pmm) -> i64 {
```

**Parameters:**

- `p`: `&mut Pmm`

**Returns:** `i64`

*Defined at line 81*

---

## pmm_frame_addr

```fides
pub fn pmm_frame_addr(f: i64) -> i64 {
```

**Parameters:**

- `f`: `i64`

**Returns:** `i64`

*Defined at line 113*

---

## pmm_addr_frame

```fides
pub fn pmm_addr_frame(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 116*

---

## PT_PRESENT

```fides
pub fn PT_PRESENT() -> i64 {
```

**Returns:** `i64`

*Defined at line 124*

---

## PT_WRITE

```fides
pub fn PT_WRITE() -> i64 {
```

**Returns:** `i64`

*Defined at line 127*

---

## PT_USER

```fides
pub fn PT_USER() -> i64 {
```

**Returns:** `i64`

*Defined at line 130*

---

## PT_PWT

```fides
pub fn PT_PWT() -> i64 {
```

**Returns:** `i64`

*Defined at line 133*

---

## PT_PCD

```fides
pub fn PT_PCD() -> i64 {
```

**Returns:** `i64`

*Defined at line 136*

---

## PT_HUGE

```fides
pub fn PT_HUGE() -> i64 {
```

**Returns:** `i64`

*Defined at line 139*

---

## PT_NX

```fides
pub fn PT_NX() -> i64 {
```

**Returns:** `i64`

*Defined at line 143*

---

## pg_pml4i

```fides
pub fn pg_pml4i(va: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`

**Returns:** `i64`

*Defined at line 148*

---

## pg_pdpti

```fides
pub fn pg_pdpti(va: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`

**Returns:** `i64`

*Defined at line 151*

---

## pg_pdi

```fides
pub fn pg_pdi(va: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`

**Returns:** `i64`

*Defined at line 154*

---

## pg_pti

```fides
pub fn pg_pti(va: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`

**Returns:** `i64`

*Defined at line 157*

---

## pg_offset

```fides
pub fn pg_offset(va: i64) -> i64 {
```

**Parameters:**

- `va`: `i64`

**Returns:** `i64`

*Defined at line 160*

---

## pg_entry

```fides
pub fn pg_entry(phys: i64, flags: i64) -> i64 {
```

**Parameters:**

- `phys`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 166*

---

## pg_entry_phys

```fides
pub fn pg_entry_phys(e: i64) -> i64 {
```

**Parameters:**

- `e`: `i64`

**Returns:** `i64`

*Defined at line 172*

---

## pg_is_present

```fides
pub fn pg_is_present(e: i64) -> bool {
```

**Parameters:**

- `e`: `i64`

**Returns:** `bool`

*Defined at line 176*

---

## pg_is_huge

```fides
pub fn pg_is_huge(e: i64) -> bool {
```

**Parameters:**

- `e`: `i64`

**Returns:** `bool`

*Defined at line 179*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
