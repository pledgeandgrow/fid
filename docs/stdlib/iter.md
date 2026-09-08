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
pub fn iter_map(v: Vec, f: fn(i64) -> i64) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `f`: `fn(i64) -> i64`

**Returns:** `Vec`

*Defined at line 20*

---

## iter_filter

Filter elements that satisfy a predicate, returning a new Vec.

```fides
pub fn iter_filter(v: Vec, pred: fn(i64) -> bool) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec`

*Defined at line 31*

---

## iter_reduce

Reduce a Vec to a single value using a function.
iter_reduce([1,2,3], add) → add(add(0, 1), 2), 3) = 6

```fides
pub fn iter_reduce(v: Vec, f: fn(i64, i64) -> i64, initial: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec`
- `f`: `fn(i64`
- `initial`: `i64`

**Returns:** `i64`

*Defined at line 45*

---

## iter_sum

Sum all elements in a Vec.

```fides
pub fn iter_sum(v: Vec) -> i64 {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `i64`

*Defined at line 56*

---

## iter_product

Product of all elements in a Vec.

```fides
pub fn iter_product(v: Vec) -> i64 {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `i64`

*Defined at line 67*

---

## iter_min

Find the minimum element. Returns 0 if empty.

```fides
pub fn iter_min(v: Vec) -> i64 {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `i64`

*Defined at line 78*

---

## iter_max

Find the maximum element. Returns 0 if empty.

```fides
pub fn iter_max(v: Vec) -> i64 {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `i64`

*Defined at line 90*

---

## iter_find_index

Find the first index of a value. Returns -1 if not found.

```fides
pub fn iter_find_index(v: Vec, value: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec`
- `value`: `i64`

**Returns:** `i64`

*Defined at line 104*

---

## iter_find

Find the first index matching a predicate. Returns -1 if not found.

```fides
pub fn iter_find(v: Vec, pred: fn(i64) -> bool) -> i64 {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `i64`

*Defined at line 114*

---

## iter_any

Check if any element satisfies a predicate.

```fides
pub fn iter_any(v: Vec, pred: fn(i64) -> bool) -> bool {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `bool`

*Defined at line 124*

---

## iter_all

Check if all elements satisfy a predicate.

```fides
pub fn iter_all(v: Vec, pred: fn(i64) -> bool) -> bool {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `bool`

*Defined at line 134*

---

## iter_count

Count elements that satisfy a predicate.

```fides
pub fn iter_count(v: Vec, pred: fn(i64) -> bool) -> i64 {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `i64`

*Defined at line 144*

---

## iter_flatten

Flatten a Vec of Vecs into a single Vec.
Note: Since Vec elements are i64 (opaque pointers), this treats
each element as a Vec pointer and flattens them.

```fides
pub fn iter_flatten(v: Vec) -> Vec {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `Vec`

*Defined at line 159*

---

## iter_take

Take the first n elements, returning a new Vec.

```fides
pub fn iter_take(v: Vec, n: i64) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `n`: `i64`

**Returns:** `Vec`

*Defined at line 175*

---

## iter_skip

Skip the first n elements, returning a new Vec with the rest.

```fides
pub fn iter_skip(v: Vec, n: i64) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `n`: `i64`

**Returns:** `Vec`

*Defined at line 188*

---

## iter_chain

Concatenate two Vecs.

```fides
pub fn iter_chain(a: Vec, b: Vec) -> Vec {
```

**Parameters:**

- `a`: `Vec`
- `b`: `Vec`

**Returns:** `Vec`

*Defined at line 200*

---

## iter_range

Generate a range [start, end) as a Vec.

```fides
pub fn iter_range(start: i64, end: i64) -> Vec {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`

**Returns:** `Vec`

*Defined at line 218*

---

## iter_range_step

Generate a range with a step.
iter_range_step(0, 10, 2) → [0, 2, 4, 6, 8]

```fides
pub fn iter_range_step(start: i64, end: i64, step: i64) -> Vec {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`
- `step`: `i64`

**Returns:** `Vec`

*Defined at line 230*

---

## iter_repeat

Repeat a value n times.

```fides
pub fn iter_repeat(value: i64, n: i64) -> Vec {
```

**Parameters:**

- `value`: `i64`
- `n`: `i64`

**Returns:** `Vec`

*Defined at line 242*

---

## iter_partition

Partition a Vec into two: elements that pass and elements that fail the predicate.
Returns a Vec of two Vecs: [passing, failing].

```fides
pub fn iter_partition(v: Vec, pred: fn(i64) -> bool) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec`

*Defined at line 256*

---

## iter_dedup

Deduplicate a Vec (remove consecutive duplicates).

```fides
pub fn iter_dedup(v: Vec) -> Vec {
```

**Parameters:**

- `v`: `Vec`

**Returns:** `Vec`

*Defined at line 275*

---

## iter_chunk

Chunk a Vec into groups of `size` elements.

```fides
pub fn iter_chunk(v: Vec, size: i64) -> Vec {
```

**Parameters:**

- `v`: `Vec`
- `size`: `i64`

**Returns:** `Vec`

*Defined at line 290*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
