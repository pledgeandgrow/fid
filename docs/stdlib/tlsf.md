[All modules](index.md)

# tlsf -- Fides Documentation

## Contents

- [TLSF_SL_LOG2:](#TLSF_SL_LOG2:) (const)
- [TLSF_SL_COUNT:](#TLSF_SL_COUNT:) (const)
- [TLSF_ALIGN:](#TLSF_ALIGN:) (const)
- [TLSF_FL_SHIFT:](#TLSF_FL_SHIFT:) (const)
- [TLSF_SMALL:](#TLSF_SMALL:) (const)
- [TLSF_FL_COUNT:](#TLSF_FL_COUNT:) (const)
- [TLSF_HDR:](#TLSF_HDR:) (const)
- [TLSF_MIN:](#TLSF_MIN:) (const)
- [TLSF_FREE:](#TLSF_FREE:) (const)
- [TLSF_PREV_FREE:](#TLSF_PREV_FREE:) (const)
- [TLSF_NIL:](#TLSF_NIL:) (const)
- [Tlsf](#Tlsf) (struct)
- [tlsf_fls](#tlsf_fls) (function)
- [tlsf_ffs](#tlsf_ffs) (function)
- [tlsf_mapping_insert](#tlsf_mapping_insert) (function)
- [tlsf_mapping_search](#tlsf_mapping_search) (function)
- [tlsf_rd](#tlsf_rd) (function)
- [tlsf_wr](#tlsf_wr) (function)
- [tlsf_new](#tlsf_new) (function)
- [tlsf_malloc](#tlsf_malloc) (function)
- [tlsf_free](#tlsf_free) (function)
- [tlsf_usable](#tlsf_usable) (function)
- [tlsf_realloc](#tlsf_realloc) (function)
- [TlsfStats](#TlsfStats) (struct)
- [tlsf_stats](#tlsf_stats) (function)
- [tlsf_check](#tlsf_check) (function)

## TLSF_SL_LOG2:

```fides
pub const TLSF_SL_LOG2: i64 = 4;
```

*Defined at line 28*

---

## TLSF_SL_COUNT:

```fides
pub const TLSF_SL_COUNT: i64 = 16;
```

*Defined at line 29*

---

## TLSF_ALIGN:

```fides
pub const TLSF_ALIGN: i64 = 8;
```

*Defined at line 30*

---

## TLSF_FL_SHIFT:

```fides
pub const TLSF_FL_SHIFT: i64 = 7;
```

*Defined at line 31*

---

## TLSF_SMALL:

```fides
pub const TLSF_SMALL: i64 = 128;
```

*Defined at line 32*

---

## TLSF_FL_COUNT:

```fides
pub const TLSF_FL_COUNT: i64 = 25;
```

*Defined at line 33*

---

## TLSF_HDR:

```fides
pub const TLSF_HDR: i64 = 16;
```

*Defined at line 34*

---

## TLSF_MIN:

```fides
pub const TLSF_MIN: i64 = 16;
```

*Defined at line 35*

---

## TLSF_FREE:

```fides
pub const TLSF_FREE: i64 = 1;
```

*Defined at line 36*

---

## TLSF_PREV_FREE:

```fides
pub const TLSF_PREV_FREE: i64 = 2;
```

*Defined at line 37*

---

## TLSF_NIL:

```fides
pub const TLSF_NIL: i64 = -1;
```

*Defined at line 38*

---

## Tlsf

```fides
pub struct Tlsf {
```

**Fields:**

| Name | Type |
|------|------|
| `mem` | `Vec<i64>` |
| `size` | `i64` |
| `fl_bitmap` | `i64` |
| `sl_bitmap` | `Vec<i64>` |
| `heads` | `Vec<i64>` |
| `allocs` | `i64` |
| `frees` | `i64` |

*Defined at line 40*

---

## tlsf_fls

Index of the most significant set bit (-1 for 0).

```fides
pub fn tlsf_fls(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 53*

---

## tlsf_ffs

Index of the least significant set bit (-1 for 0).

```fides
pub fn tlsf_ffs(x: i64) -> i64 {
```

**Parameters:**

- `x`: `i64`

**Returns:** `i64`

*Defined at line 86*

---

## tlsf_mapping_insert

(fl, sl) of a free block of `size` bytes, packed fl * 16 + sl.

```fides
pub fn tlsf_mapping_insert(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## tlsf_mapping_search

The list to search for a request: rounded up so every block there fits.

```fides
pub fn tlsf_mapping_search(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 104*

---

## tlsf_rd

```fides
pub fn tlsf_rd(t: &Tlsf, off: i64) -> i64 {
```

**Parameters:**

- `t`: `&Tlsf`
- `off`: `i64`

**Returns:** `i64`

*Defined at line 114*

---

## tlsf_wr

```fides
pub fn tlsf_wr(t: &mut Tlsf, off: i64, v: i64) {
```

**Parameters:**

- `t`: `&mut Tlsf`
- `off`: `i64`
- `v`: `i64`

*Defined at line 118*

---

## tlsf_new

An allocator over an arena of `bytes` bytes (rounded down to 8).

```fides
pub fn tlsf_new(bytes: i64) -> Tlsf {
```

**Parameters:**

- `bytes`: `i64`

**Returns:** `Tlsf`

*Defined at line 195*

---

## tlsf_malloc

Allocate `n` bytes; the payload's byte offset, -1 when no block fits.

```fides
pub fn tlsf_malloc(t: &mut Tlsf, n: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut Tlsf`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 223*

---

## tlsf_free

Free a payload offset from tlsf_malloc (panics on a double or bad free).

```fides
pub fn tlsf_free(t: &mut Tlsf, p: i64) {
```

**Parameters:**

- `t`: `&mut Tlsf`
- `p`: `i64`

*Defined at line 264*

---

## tlsf_usable

Usable bytes of an allocated payload.

```fides
pub fn tlsf_usable(t: &Tlsf, p: i64) -> i64 {
```

**Parameters:**

- `t`: `&Tlsf`
- `p`: `i64`

**Returns:** `i64`

*Defined at line 294*

---

## tlsf_realloc

Resize: in place when the next physical block is free and large
enough (or when shrinking), else allocate, copy and free. -1 on failure
(the old block stays valid).

```fides
pub fn tlsf_realloc(t: &mut Tlsf, p: i64, n: i64) -> i64 {
```

**Parameters:**

- `t`: `&mut Tlsf`
- `p`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 301*

---

## TlsfStats

```fides
pub struct TlsfStats {
```

**Fields:**

| Name | Type |
|------|------|
| `free_bytes` | `i64` |
| `used_bytes` | `i64` |
| `free_blocks` | `i64` |
| `used_blocks` | `i64` |
| `largest_free` | `i64` |

*Defined at line 344*

---

## tlsf_stats

```fides
pub fn tlsf_stats(t: &Tlsf) -> TlsfStats {
```

**Parameters:**

- `t`: `&Tlsf`

**Returns:** `TlsfStats`

*Defined at line 352*

---

## tlsf_check

Every invariant; "" when all hold, else the first violation.

```fides
pub fn tlsf_check(t: &Tlsf) -> str {
```

**Parameters:**

- `t`: `&Tlsf`

**Returns:** `str`

*Defined at line 375*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
