# iter — Fides Documentation

## Contents

- [iter_map](#iter_map) (function)
- [iter_filter](#iter_filter) (function)
- [iter_reduce](#iter_reduce) (function)
- [iter_sum](#iter_sum) (function)
- [iter_product](#iter_product) (function)
- [iter_min](#iter_min) (function)
- [iter_max](#iter_max) (function)
- [iter_find_index](#iter_find_index) (function)
- [iter_find](#iter_find) (function)
- [iter_any](#iter_any) (function)
- [iter_all](#iter_all) (function)
- [iter_count](#iter_count) (function)
- [iter_flatten](#iter_flatten) (function)
- [iter_take](#iter_take) (function)
- [iter_skip](#iter_skip) (function)
- [iter_chain](#iter_chain) (function)
- [iter_range](#iter_range) (function)
- [iter_range_step](#iter_range_step) (function)
- [iter_repeat](#iter_repeat) (function)
- [iter_partition](#iter_partition) (function)
- [iter_dedup](#iter_dedup) (function)
- [iter_chunk](#iter_chunk) (function)

## iter_map

Map each element through a function, returning a new Vec.

```fides
pub fn iter_map(v: Vec<i64>, f: fn(i64) -> i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `f`: `fn(i64) -> i64`

**Returns:** `Vec<i64>`

*Defined at line 23*

---

## iter_filter

Filter elements that satisfy a predicate, returning a new Vec.

```fides
pub fn iter_filter(v: Vec<i64>, pred: fn(i64) -> bool) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec<i64>`

*Defined at line 39*

---

## iter_reduce

Reduce a Vec to a single value using a function.
iter_reduce([1,2,3], add) → add(add(0, 1), 2), 3) = 6

```fides
pub fn iter_reduce(v: Vec<i64>, f: fn(i64, i64) -> i64, initial: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `f`: `fn(i64, i64) -> i64`
- `initial`: `i64`

**Returns:** `i64`

*Defined at line 58*

---

## iter_sum

Sum all elements in a Vec.

```fides
pub fn iter_sum(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 72*

---

## iter_product

Product of all elements in a Vec.

```fides
pub fn iter_product(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 86*

---

## iter_min

Find the minimum element. Returns 0 if empty.

```fides
pub fn iter_min(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 102*

---

## iter_max

Find the maximum element. Returns 0 if empty.

```fides
pub fn iter_max(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 123*

---

## iter_find_index

Find the first index of a value. Returns -1 if not found.

```fides
pub fn iter_find_index(v: Vec<i64>, value: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 146*

---

## iter_find

Find the first index matching a predicate. Returns -1 if not found.

```fides
pub fn iter_find(v: Vec<i64>, pred: fn(i64) -> bool) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `i64`

*Defined at line 162*

---

## iter_any

Check if any element satisfies a predicate.

```fides
pub fn iter_any(v: Vec<i64>, pred: fn(i64) -> bool) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `bool`

*Defined at line 177*

---

## iter_all

Check if all elements satisfy a predicate.

```fides
pub fn iter_all(v: Vec<i64>, pred: fn(i64) -> bool) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `bool`

*Defined at line 193*

---

## iter_count

Count elements that satisfy a predicate.

```fides
pub fn iter_count(v: Vec<i64>, pred: fn(i64) -> bool) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `i64`

*Defined at line 208*

---

## iter_flatten

Flatten a Vec of Vecs into a single Vec.
Note: Since Vec elements are i64 (opaque pointers), this treats
each element as a Vec pointer and flattens them.

```fides
pub fn iter_flatten(v: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 233*

---

## iter_take

Take the first n elements, returning a new Vec.

```fides
pub fn iter_take(v: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 254*

---

## iter_skip

Skip the first n elements, returning a new Vec with the rest.

```fides
pub fn iter_skip(v: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 274*

---

## iter_chain

Concatenate two Vecs.

```fides
pub fn iter_chain(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 292*

---

## iter_range

Generate a range [start, end) as a Vec.

```fides
pub fn iter_range(start: i64, end: i64) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 316*

---

## iter_range_step

Generate a range with a step.
iter_range_step(0, 10, 2) → [0, 2, 4, 6, 8]

```fides
pub fn iter_range_step(start: i64, end: i64, step: i64) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`
- `step`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 333*

---

## iter_repeat

Repeat a value n times.

```fides
pub fn iter_repeat(value: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `value`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 352*

---

## iter_partition

Partition a Vec into two: elements that pass and elements that fail the predicate.
Returns a Vec of two Vecs: [passing, failing].

```fides
pub fn iter_partition(v: Vec<i64>, pred: fn(i64) -> bool) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 373*

---

## iter_dedup

Deduplicate a Vec (remove consecutive duplicates).

```fides
pub fn iter_dedup(v: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 398*

---

## iter_chunk

Chunk a Vec into groups of `size` elements.

```fides
pub fn iter_chunk(v: Vec<i64>, size: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `size`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 422*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
