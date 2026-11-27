[All modules](index.md)

# iter -- Fides Documentation

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
- [iter_zip](#iter_zip) (function)
- [iter_enumerate](#iter_enumerate) (function)
- [iter_windows](#iter_windows) (function)
- [iter_flat_map](#iter_flat_map) (function)
- [iter_take_while](#iter_take_while) (function)
- [iter_skip_while](#iter_skip_while) (function)
- [iter_position](#iter_position) (function)
- [iter_find_opt](#iter_find_opt) (function)
- [iter_nth](#iter_nth) (function)
- [iter_last](#iter_last) (function)
- [iter_min_opt](#iter_min_opt) (function)
- [iter_max_opt](#iter_max_opt) (function)
- [iter_min_by](#iter_min_by) (function)
- [iter_max_by](#iter_max_by) (function)
- [iter_rev](#iter_rev) (function)
- [iter_for_each](#iter_for_each) (function)

## iter_map

Map each element through a function, returning a new Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = iter_range(1, 4);
var sq = iter_map(v, fn(x: i64) -> i64 { x * x });
assert_eq(sq.get(2), 9);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var odd = iter_filter(iter_range(0, 6), fn(x: i64) -> bool { x % 2 == 1 });
assert_eq(odd.len(), 3);
assert_eq(odd.get(0), 1);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var prod = iter_reduce(iter_range(1, 5), fn(a: i64, x: i64) -> i64 { a * x }, 1);
println(int_to_str(prod));
assert_eq(prod, 24);
```

```fides
pub fn iter_reduce(v: Vec<i64>, f: fn(i64, i64) -> i64, initial: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `f`: `fn(i64`
- `initial`: `i64`

**Returns:** `i64`

*Defined at line 58*

---

## iter_sum

Sum all elements in a Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(iter_sum(iter_range(1, 11)), 55);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(iter_product(iter_range(1, 6)), 120);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(5); v.push(-2); v.push(9);
assert_eq(iter_min(v), -2);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(5); v.push(-2); v.push(9);
assert_eq(iter_max(v), 9);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = iter_range(10, 15);
assert_eq(iter_find_index(v, 12), 2);
assert_eq(iter_find_index(v, 99), -1);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var i = iter_find(iter_range(0, 10), fn(x: i64) -> bool { x * x > 20 });
assert_eq(i, 5);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(iter_any(iter_range(0, 5), fn(x: i64) -> bool { x == 3 }));
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(iter_all(iter_range(1, 5), fn(x: i64) -> bool { x > 0 }));
assert(!iter_all(iter_range(0, 5), fn(x: i64) -> bool { x > 0 }));
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(iter_count(iter_range(0, 10), fn(x: i64) -> bool { x % 3 == 0 }), 4);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var outer: Vec<Vec<i64>> = Vec::new();
outer.push(iter_range(0, 2));
outer.push(iter_range(5, 8));
var flat = iter_flatten(outer);
assert_eq(flat.len(), 5);
assert_eq(flat.get(2), 5);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = iter_take(iter_range(0, 10), 3);
assert_eq(t.len(), 3);
assert_eq(t.get(2), 2);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = iter_skip(iter_range(0, 10), 7);
assert_eq(s.len(), 3);
assert_eq(s.get(0), 7);
```

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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = iter_chain(iter_range(0, 2), iter_range(10, 12));
assert_eq(c.len(), 4);
assert_eq(c.get(2), 10);
```

```fides
pub fn iter_chain(a: Vec<i64>, b: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 293*

---

## iter_range

Generate a range [start, end) as a Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = iter_range(3, 6);
assert_eq(r.len(), 3);
assert_eq(r.get(0), 3);
assert_eq(iter_range(5, 5).len(), 0);
```

```fides
pub fn iter_range(start: i64, end: i64) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 317*

---

## iter_range_step

Generate a range with a step.
iter_range_step(0, 10, 2) → [0, 2, 4, 6, 8]


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = iter_range_step(0, 10, 3);
assert_eq(r.len(), 4);
assert_eq(r.get(3), 9);
```

```fides
pub fn iter_range_step(start: i64, end: i64, step: i64) -> Vec<i64> {
```

**Parameters:**

- `start`: `i64`
- `end`: `i64`
- `step`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 334*

---

## iter_repeat

Repeat a value n times.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = iter_repeat(7, 3);
assert_eq(r.len(), 3);
assert_eq(iter_sum(r), 21);
```

```fides
pub fn iter_repeat(value: i64, n: i64) -> Vec<i64> {
```

**Parameters:**

- `value`: `i64`
- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 353*

---

## iter_partition

Partition a Vec into two: elements that pass and elements that fail the predicate.
Returns a Vec of two Vecs: [passing, failing].


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var parts = iter_partition(iter_range(0, 5), fn(x: i64) -> bool { x < 2 });
var low: Vec<i64> = parts.get(0);
var high: Vec<i64> = parts.get(1);
assert_eq(low.len(), 2);
assert_eq(high.len(), 3);
```

```fides
pub fn iter_partition(v: Vec<i64>, pred: fn(i64) -> bool) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 374*

---

## iter_dedup

Deduplicate a Vec (remove consecutive duplicates).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(1); v.push(2); v.push(1);
var d = iter_dedup(v);
assert_eq(d.len(), 3);
```

```fides
pub fn iter_dedup(v: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 399*

---

## iter_chunk

Chunk a Vec into groups of `size` elements.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var chunks = iter_chunk(iter_range(0, 5), 2);
assert_eq(chunks.len(), 3);
var last: Vec<i64> = chunks.get(2);
assert_eq(last.len(), 1);
assert_eq(last.get(0), 4);
```

```fides
pub fn iter_chunk(v: Vec<i64>, size: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `size`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 423*

---

## iter_zip

Pair up two Vecs element-wise into a Vec of [a_i, b_i] pairs.
Stops at the shorter input.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var z = iter_zip(iter_range(0, 3), iter_range(10, 13));
var p: Vec<i64> = z.get(1);
assert_eq(p.get(0), 1);
assert_eq(p.get(1), 11);
```

```fides
pub fn iter_zip(a: Vec<i64>, b: Vec<i64>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `a`: `Vec<i64>`
- `b`: `Vec<i64>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 452*

---

## iter_enumerate

Pair each element with its index: a Vec of [index, value] pairs.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var e = iter_enumerate(iter_range(5, 8));
var p: Vec<i64> = e.get(2);
assert_eq(p.get(0), 2);
assert_eq(p.get(1), 7);
```

```fides
pub fn iter_enumerate(v: Vec<i64>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 476*

---

## iter_windows

Sliding windows of `size` elements as a Vec of Vecs.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var w = iter_windows(iter_range(0, 4), 3);
assert_eq(w.len(), 2);
var w1: Vec<i64> = w.get(1);
assert_eq(w1.get(0), 1);
```

```fides
pub fn iter_windows(v: Vec<i64>, size: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `size`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 496*

---

## iter_flat_map

Map each element to a Vec and flatten the results.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = iter_flat_map(iter_range(0, 3), fn(x: i64) -> Vec<i64> {
  var p: Vec<i64> = Vec::new();
  p.push(x); p.push(x * 10);
  return p;
});
assert_eq(r.len(), 6);
assert_eq(r.get(3), 10);
```

```fides
pub fn iter_flat_map(v: Vec<i64>, f: fn(i64) -> Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `f`: `fn(i64) -> Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 527*

---

## iter_take_while

Take elements while a predicate holds (stops at the first failure).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var t = iter_take_while(iter_range(0, 10), fn(x: i64) -> bool { x < 3 });
assert_eq(t.len(), 3);
```

```fides
pub fn iter_take_while(v: Vec<i64>, pred: fn(i64) -> bool) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec<i64>`

*Defined at line 547*

---

## iter_skip_while

Skip elements while a predicate holds, return the rest.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = iter_skip_while(iter_range(0, 10), fn(x: i64) -> bool { x < 3 });
assert_eq(s.len(), 7);
assert_eq(s.get(0), 3);
```

```fides
pub fn iter_skip_while(v: Vec<i64>, pred: fn(i64) -> bool) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Vec<i64>`

*Defined at line 568*

---

## iter_position

The index of the first element matching a predicate, or Option::None.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_position(iter_range(0, 10), fn(x: i64) -> bool { x * x > 20 }) {
  Option::Some(i) => { assert_eq(i, 5); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_position(v: Vec<i64>, pred: fn(i64) -> bool) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `pred`: `fn(i64) -> bool`

**Returns:** `Option<i64>`

*Defined at line 590*

---

## iter_find_opt

The index of the first element equal to `value`, or Option::None.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_find_opt(iter_range(10, 15), 12) {
  Option::Some(i) => { assert_eq(i, 2); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_find_opt(v: Vec<i64>, value: i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `value`: `i64`

**Returns:** `Option<i64>`

*Defined at line 608*

---

## iter_nth

The element at index `n`, or Option::None when out of range.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_nth(iter_range(5, 9), 2) {
  Option::Some(x) => { assert_eq(x, 7); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_nth(v: Vec<i64>, n: i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `n`: `i64`

**Returns:** `Option<i64>`

*Defined at line 626*

---

## iter_last

The last element, or Option::None if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_last(iter_range(5, 9)) {
  Option::Some(x) => { assert_eq(x, 8); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_last(v: Vec<i64>) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Option<i64>`

*Defined at line 640*

---

## iter_min_opt

The minimum element, or Option::None if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_min_opt(iter_range(3, 9)) {
  Option::Some(m) => { assert_eq(m, 3); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_min_opt(v: Vec<i64>) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Option<i64>`

*Defined at line 654*

---

## iter_max_opt

The maximum element, or Option::None if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match iter_max_opt(iter_range(3, 9)) {
  Option::Some(m) => { assert_eq(m, 8); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_max_opt(v: Vec<i64>) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Option<i64>`

*Defined at line 668*

---

## iter_min_by

The minimum element by a comparator (negative when a < b), or
Option::None if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(30); v.push(12); v.push(21);
match iter_min_by(v, fn(a: i64, b: i64) -> i64 { a - b }) {
  Option::Some(m) => { assert_eq(m, 12); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_min_by(v: Vec<i64>, cmp: fn(i64, i64) -> i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `cmp`: `fn(i64`

**Returns:** `Option<i64>`

*Defined at line 685*

---

## iter_max_by

The maximum element by a comparator, or Option::None if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(30); v.push(12); v.push(21);
match iter_max_by(v, fn(a: i64, b: i64) -> i64 { a - b }) {
  Option::Some(m) => { assert_eq(m, 30); }
  Option::None => { assert(false); }
}
```

```fides
pub fn iter_max_by(v: Vec<i64>, cmp: fn(i64, i64) -> i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `cmp`: `fn(i64`

**Returns:** `Option<i64>`

*Defined at line 709*

---

## iter_rev

A reversed copy of a Vec.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = iter_rev(iter_range(0, 3));
assert_eq(r.get(0), 2);
assert_eq(r.get(2), 0);
```

```fides
pub fn iter_rev(v: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 732*

---

## iter_for_each

Apply a function to each element for its side effect.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
iter_for_each(iter_range(0, 3), fn(x: i64) -> void { println(x); });
```

```fides
pub fn iter_for_each(v: Vec<i64>, f: fn(i64) -> void) -> void {
```

**Parameters:**

- `v`: `Vec<i64>`
- `f`: `fn(i64) -> void`

**Returns:** `void`

*Defined at line 746*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
