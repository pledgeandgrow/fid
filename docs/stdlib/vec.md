[All modules](index.md)

# vec -- Fides Documentation

## Contents

- [Vec](#Vec) (struct)
- [Vec](#Vec) (impl)
- [vec_take](#vec_take) (function)
- [vec_skip](#vec_skip) (function)
- [vec_zip](#vec_zip) (function)
- [vec_enumerate](#vec_enumerate) (function)
- [vec_flat_map](#vec_flat_map) (function)
- [vec_chunk](#vec_chunk) (function)
- [vec_window](#vec_window) (function)
- [vec_dedup](#vec_dedup) (function)
- [vec_sort_by](#vec_sort_by) (function)
- [vec_contains_val](#vec_contains_val) (function)
- [vec_find_opt](#vec_find_opt) (function)
- [vec_position_opt](#vec_position_opt) (function)
- [vec_get_opt](#vec_get_opt) (function)
- [vec_first_opt](#vec_first_opt) (function)
- [vec_last_opt](#vec_last_opt) (function)

## Vec

A growable array. The basic operations (push, pop, len, get, set, ...)
are core's; this module adds map, filter, fold and the other helpers.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3); v.push(4);
var doubled = v.map(fn(x: i64) -> i64 { x * 2 });
var evens = v.filter(fn(x: i64) -> bool { x % 2 == 0 });
var sum = v.fold(0, fn(acc: i64, x: i64) -> i64 { acc + x });
println(int_to_str(sum));
assert_eq(doubled.get(3), 8);
assert_eq(evens.len(), 2);
assert_eq(sum, 10);
assert(v.any(fn(x: i64) -> bool { x > 3 }));
assert_eq(v.position(fn(x: i64) -> bool { x == 3 }), 2);
```

```fides
pub struct Vec<T> {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `i64,  // raw pointer (opaque at Fides level)` |
| `len` | `i64` |
| `cap` | `i64` |

*Defined at line 38*

---

## Vec

```fides
impl Vec {
```

**Methods:**

### find_opt

```fides
pub fn find_opt(self: Vec<i64>, pred: fn(i64) -> bool) -> Option<i64> {
```

### position_opt

```fides
pub fn position_opt(self: Vec<i64>, pred: fn(i64) -> bool) -> Option<i64> {
```

### take

```fides
pub fn take(self: Vec<i64>, n: i64) -> Vec<i64> {
```

### skip

```fides
pub fn skip(self: Vec<i64>, n: i64) -> Vec<i64> {
```

### zip

```fides
pub fn zip(self: Vec<i64>, other: Vec<i64>) -> Vec<i64> {
```

### enumerate

```fides
pub fn enumerate(self: Vec<i64>) -> Vec<i64> {
```

### flat_map

```fides
pub fn flat_map(self: Vec<i64>, f: fn(i64) -> i64) -> Vec<i64> {
```

### chunk

```fides
pub fn chunk(self: Vec<i64>, size: i64) -> Vec<i64> {
```

### window

```fides
pub fn window(self: Vec<i64>, size: i64) -> Vec<i64> {
```

### dedup

```fides
pub fn dedup(self: &mut Vec<i64>) -> void {
```

### sort_by

```fides
pub fn sort_by(self: &mut Vec<i64>, cmp: fn(i64, i64) -> i64) -> void {
```

### contains_val

```fides
pub fn contains_val(self: Vec<i64>, value: i64) -> bool {
```

*Defined at line 44*

---

## vec_take

The first `n` elements (all of them if there are fewer) as a new Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(10); v.push(20); v.push(30);
var t = vec_take(v, 2);
assert_eq(t.len(), 2);
assert_eq(t.get(1), 20);
```

```fides
pub fn vec_take(self: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 130*

---

## vec_skip

Everything after the first `n` elements, as a new Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(10); v.push(20); v.push(30);
var t = vec_skip(v, 1);
assert_eq(t.len(), 2);
assert_eq(t.get(0), 20);
```

```fides
pub fn vec_skip(self: Vec<i64>, n: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 153*

---

## vec_zip

The elements of both Vecs interleaved pairwise (a0, b0, a1, b1, ...),
up to the length of the shorter one.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a: Vec<i64> = Vec::new();
a.push(1); a.push(2);
var b: Vec<i64> = Vec::new();
b.push(10); b.push(20); b.push(30);
var z = vec_zip(a, b);
assert_eq(z.len(), 4);
assert_eq(z.get(1), 10);
assert_eq(z.get(2), 2);
```

```fides
pub fn vec_zip(self: Vec<i64>, other: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `other`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 180*

---

## vec_enumerate

Index and element pairs flattened into one Vec (0, v0, 1, v1, ...).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(7); v.push(8);
var e = vec_enumerate(v);
assert_eq(e.len(), 4);
assert_eq(e.get(2), 1);
assert_eq(e.get(3), 8);
```

```fides
pub fn vec_enumerate(self: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 206*

---

## vec_flat_map

Concatenation of the Vecs that `f` returns for each element (`f`
returns a Vec handle as an i64).

```fides
pub fn vec_flat_map(self: Vec<i64>, f: fn(i64) -> i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `f`: `fn(i64) -> i64`

**Returns:** `Vec<i64>`

*Defined at line 220*

---

## vec_chunk

The start index of each chunk of `size` elements.
A size <= 0 gives an empty Vec (never an infinite loop).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3); v.push(4); v.push(5);
var starts = vec_chunk(v, 2);
assert_eq(starts.len(), 3);
assert_eq(starts.get(2), 4);
```

```fides
pub fn vec_chunk(self: Vec<i64>, size: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 246*

---

## vec_window

The start index of every full window of `size` consecutive elements.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3); v.push(4);
var starts = vec_window(v, 3);
assert_eq(starts.len(), 2);
assert_eq(starts.get(1), 1);
```

```fides
pub fn vec_window(self: Vec<i64>, size: i64) -> Vec<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 268*

---

## vec_dedup

Remove consecutive duplicates in place (sort first to remove all).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(1); v.push(2); v.push(2); v.push(1);
vec_dedup(&mut v);
assert_eq(v.len(), 3);
assert_eq(v.get(2), 1);
```

```fides
pub fn vec_dedup(self: &mut Vec<i64>) -> void {
```

**Parameters:**

- `self`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 290*

---

## vec_sort_by

Sort in place with a comparison function (negative, zero or positive,
like `a - b`). Heapsort: O(n log n) worst case, in place, no recursion.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(3); v.push(1); v.push(2);
vec_sort_by(&mut v, fn(a: i64, b: i64) -> i64 { b - a });
assert_eq(v.get(0), 3);
assert_eq(v.get(2), 1);
```

```fides
pub fn vec_sort_by(self: &mut Vec<i64>, cmp: fn(i64, i64) -> i64) -> void {
```

**Parameters:**

- `self`: `&mut Vec<i64>`
- `cmp`: `fn(i64`

**Returns:** `void`

*Defined at line 316*

---

## vec_contains_val

Whether the Vec holds `value`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(4); v.push(5);
assert(vec_contains_val(v, 5));
assert(!vec_contains_val(v, 6));
```

```fides
pub fn vec_contains_val(self: Vec<i64>, value: i64) -> bool {
```

**Parameters:**

- `self`: `Vec<i64>`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 369*

---

## vec_find_opt

The first element satisfying `pred`, or Option::None.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(4); v.push(5);
match vec_find_opt(v, fn(x: i64) -> bool { x == 5 }) {
  Option::Some(x) => { assert_eq(x, 5); }
  Option::None => { assert(false); }
}
```

```fides
pub fn vec_find_opt(self: Vec<i64>, pred: fn(i64) -> bool) -> Option<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Option<i64>`

*Defined at line 390*

---

## vec_position_opt

The index of the first element satisfying `pred`, or Option::None.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(4); v.push(5);
match vec_position_opt(v, fn(x: i64) -> bool { x == 5 }) {
  Option::Some(i) => { assert_eq(i, 1); }
  Option::None => { assert(false); }
}
```

```fides
pub fn vec_position_opt(self: Vec<i64>, pred: fn(i64) -> bool) -> Option<i64> {
```

**Parameters:**

- `self`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Option<i64>`

*Defined at line 412*

---

## vec_get_opt

The element at `idx`, or Option::None when out of bounds — the safe
version of v.get (which panics).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(7);
match vec_get_opt(v, 5) {
  Option::Some(x) => { assert(false); }
  Option::None => { assert(true); }
}
```

```fides
pub fn vec_get_opt(v: Vec<i64>, idx: i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `idx`: `i64`

**Returns:** `Option<i64>`

*Defined at line 434*

---

## vec_first_opt

The first element, or Option::None when empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
match vec_first_opt(v) {
  Option::Some(x) => { assert(false); }
  Option::None => { assert(true); }
}
```

```fides
pub fn vec_first_opt(v: Vec<i64>) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Option<i64>`

*Defined at line 449*

---

## vec_last_opt

The last element, or Option::None when empty.

```fides
pub fn vec_last_opt(v: Vec<i64>) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Option<i64>`

*Defined at line 457*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
