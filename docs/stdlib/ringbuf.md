[All modules](index.md)

# ringbuf -- Fides Documentation

## Contents

- [rb_pow2_at_least](#rb_pow2_at_least) (function)
- [SpscRing](#SpscRing) (struct)
- [spsc_new](#spsc_new) (function)
- [spsc_push](#spsc_push) (function)
- [spsc_len](#spsc_len) (function)
- [spsc_is_empty](#spsc_is_empty) (function)
- [spsc_peek](#spsc_peek) (function)
- [spsc_pop_or](#spsc_pop_or) (function)
- [spsc_push_batch](#spsc_push_batch) (function)
- [spsc_pop_batch](#spsc_pop_batch) (function)
- [MpmcRing](#MpmcRing) (struct)
- [mpmc_new](#mpmc_new) (function)
- [MpmcTicket](#MpmcTicket) (struct)
- [mpmc_claim_push](#mpmc_claim_push) (function)
- [mpmc_publish_push](#mpmc_publish_push) (function)
- [mpmc_push](#mpmc_push) (function)
- [mpmc_claim_pop](#mpmc_claim_pop) (function)
- [mpmc_finish_pop](#mpmc_finish_pop) (function)
- [mpmc_pop_or](#mpmc_pop_or) (function)
- [mpmc_len](#mpmc_len) (function)
- [OverwriteRing](#OverwriteRing) (struct)
- [owring_new](#owring_new) (function)
- [owring_push](#owring_push) (function)
- [owring_items](#owring_items) (function)

## rb_pow2_at_least

Smallest power of two >= n (n >= 1).

```fides
pub fn rb_pow2_at_least(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 38*

---

## SpscRing

```fides
pub struct SpscRing {
```

**Fields:**

| Name | Type |
|------|------|
| `cap` | `i64` |
| `mask` | `i64` |
| `slots` | `Vec<Atomic>` |
| `head` | `Atomic` |
| `tail` | `Atomic` |

*Defined at line 48*

---

## spsc_new

```fides
pub fn spsc_new(cap: i64) -> SpscRing {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `SpscRing`

*Defined at line 56*

---

## spsc_push

Producer: false when full.

```fides
pub fn spsc_push(r: SpscRing, v: i64) -> bool {
```

**Parameters:**

- `r`: `SpscRing`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 70*

---

## spsc_len

Number of values in the ring (exact from the producer's or the
consumer's side; a snapshot from anywhere else).

```fides
pub fn spsc_len(r: SpscRing) -> i64 {
```

**Parameters:**

- `r`: `SpscRing`

**Returns:** `i64`

*Defined at line 83*

---

## spsc_is_empty

```fides
pub fn spsc_is_empty(r: SpscRing) -> bool {
```

**Parameters:**

- `r`: `SpscRing`

**Returns:** `bool`

*Defined at line 87*

---

## spsc_peek

Consumer: the oldest value without taking it (call when not empty).

```fides
pub fn spsc_peek(r: SpscRing) -> i64 {
```

**Parameters:**

- `r`: `SpscRing`

**Returns:** `i64`

*Defined at line 92*

---

## spsc_pop_or

Consumer: take the oldest value; `none` when empty.

```fides
pub fn spsc_pop_or(r: SpscRing, none: i64) -> i64 {
```

**Parameters:**

- `r`: `SpscRing`
- `none`: `i64`

**Returns:** `i64`

*Defined at line 98*

---

## spsc_push_batch

Producer: push as many of `vals` as fit (one head publication); count.

```fides
pub fn spsc_push_batch(r: SpscRing, vals: Vec<i64>) -> i64 {
```

**Parameters:**

- `r`: `SpscRing`
- `vals`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 110*

---

## spsc_pop_batch

Consumer: take up to `max` values (one tail publication).

```fides
pub fn spsc_pop_batch(r: SpscRing, max: i64) -> Vec<i64> {
```

**Parameters:**

- `r`: `SpscRing`
- `max`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 127*

---

## MpmcRing

```fides
pub struct MpmcRing {
```

**Fields:**

| Name | Type |
|------|------|
| `cap` | `i64` |
| `mask` | `i64` |
| `seq` | `Vec<Atomic>` |
| `data` | `Vec<Atomic>` |
| `enq` | `Atomic` |
| `deq` | `Atomic` |

*Defined at line 146*

---

## mpmc_new

```fides
pub fn mpmc_new(cap: i64) -> MpmcRing {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `MpmcRing`

*Defined at line 155*

---

## MpmcTicket

A claimed position, so a producer or consumer can be stepped through
the two halves of its protocol (claim, then publish) in tests that
interleave several of them.

```fides
pub struct MpmcTicket {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `pos` | `i64` |

*Defined at line 173*

---

## mpmc_claim_push

Producer, first half: claim a cell. ok = false when full.

```fides
pub fn mpmc_claim_push(r: MpmcRing) -> MpmcTicket {
```

**Parameters:**

- `r`: `MpmcRing`

**Returns:** `MpmcTicket`

*Defined at line 179*

---

## mpmc_publish_push

Producer, second half: write the value and release the cell.

```fides
pub fn mpmc_publish_push(r: MpmcRing, k: MpmcTicket, v: i64) {
```

**Parameters:**

- `r`: `MpmcRing`
- `k`: `MpmcTicket`
- `v`: `i64`

*Defined at line 205*

---

## mpmc_push

```fides
pub fn mpmc_push(r: MpmcRing, v: i64) -> bool {
```

**Parameters:**

- `r`: `MpmcRing`
- `v`: `i64`

**Returns:** `bool`

*Defined at line 210*

---

## mpmc_claim_pop

Consumer, first half: claim the next published cell. ok = false when
empty (or the next cell's producer has not published yet).

```fides
pub fn mpmc_claim_pop(r: MpmcRing) -> MpmcTicket {
```

**Parameters:**

- `r`: `MpmcRing`

**Returns:** `MpmcTicket`

*Defined at line 221*

---

## mpmc_finish_pop

Consumer, second half: read the value and free the cell for the next lap.

```fides
pub fn mpmc_finish_pop(r: MpmcRing, k: MpmcTicket) -> i64 {
```

**Parameters:**

- `r`: `MpmcRing`
- `k`: `MpmcTicket`

**Returns:** `i64`

*Defined at line 247*

---

## mpmc_pop_or

```fides
pub fn mpmc_pop_or(r: MpmcRing, none: i64) -> i64 {
```

**Parameters:**

- `r`: `MpmcRing`
- `none`: `i64`

**Returns:** `i64`

*Defined at line 253*

---

## mpmc_len

Approximate number of queued values (exact when no one is mid-operation).

```fides
pub fn mpmc_len(r: MpmcRing) -> i64 {
```

**Parameters:**

- `r`: `MpmcRing`

**Returns:** `i64`

*Defined at line 262*

---

## OverwriteRing

```fides
pub struct OverwriteRing {
```

**Fields:**

| Name | Type |
|------|------|
| `cap` | `i64` |
| `buf` | `Vec<i64>` |
| `head` | `i64` |
| `count` | `i64` |
| `dropped` | `i64` |

*Defined at line 268*

---

## owring_new

```fides
pub fn owring_new(cap: i64) -> OverwriteRing {
```

**Parameters:**

- `cap`: `i64`

**Returns:** `OverwriteRing`

*Defined at line 276*

---

## owring_push

Append; when full the oldest entry is overwritten (and counted).

```fides
pub fn owring_push(r: &mut OverwriteRing, v: i64) {
```

**Parameters:**

- `r`: `&mut OverwriteRing`
- `v`: `i64`

*Defined at line 284*

---

## owring_items

The entries, oldest first.

```fides
pub fn owring_items(r: &OverwriteRing) -> Vec<i64> {
```

**Parameters:**

- `r`: `&OverwriteRing`

**Returns:** `Vec<i64>`

*Defined at line 295*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
