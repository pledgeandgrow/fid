[All modules](index.md)

# sort -- Fides Documentation

## Contents

- [sort_quick](#sort_quick) (function)
- [sort_quick_desc](#sort_quick_desc) (function)
- [sort_insertion](#sort_insertion) (function)
- [sort_bubble](#sort_bubble) (function)
- [sort_selection](#sort_selection) (function)
- [sort_by](#sort_by) (function)
- [sort_by_key](#sort_by_key) (function)
- [sort_binary_search_opt](#sort_binary_search_opt) (function)
- [sort_is_sorted](#sort_is_sorted) (function)
- [sort_is_sorted_desc](#sort_is_sorted_desc) (function)
- [sort_copy](#sort_copy) (function)
- [sort_reverse](#sort_reverse) (function)
- [sort_binary_search](#sort_binary_search) (function)
- [sort_lower_bound](#sort_lower_bound) (function)
- [sort_quick_range](#sort_quick_range) (function)
- [sort_quick_desc_range](#sort_quick_desc_range) (function)
- [sort_by_range](#sort_by_range) (function)

## sort_quick

Sort a Vec in ascending order (introsort: median-of-3 quicksort with a
heapsort fallback, insertion sort on small ranges, recursion bounded to
the smaller side). Worst case O(n log n), recursion depth O(log n) even
on sorted or adversarial input.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(3); v.push(1); v.push(4); v.push(1); v.push(5);
sort_quick(&mut v);
assert_eq(v.get(0), 1);
assert_eq(v.get(4), 5);
assert(sort_is_sorted(v));
```

```fides
pub fn sort_quick(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 30*

---

## sort_quick_desc

Sort a Vec in descending order (same introsort, descending comparator).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(3); v.push(1); v.push(4);
sort_quick_desc(&mut v);
assert_eq(v.get(0), 4);
assert(sort_is_sorted_desc(v));
```

```fides
pub fn sort_quick_desc(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 45*

---

## sort_insertion

Sort a Vec using insertion sort. O(n²) but fast for small arrays.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(2); v.push(-1); v.push(0);
sort_insertion(&mut v);
assert_eq(v.get(0), -1);
```

```fides
pub fn sort_insertion(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 59*

---

## sort_bubble

Sort a Vec using bubble sort. O(n²), simple but slow.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(9); v.push(8); v.push(7);
sort_bubble(&mut v);
assert_eq(v.get(0), 7);
```

```fides
pub fn sort_bubble(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 86*

---

## sort_selection

Sort a Vec using selection sort. O(n²), minimal swaps.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(5); v.push(2); v.push(8);
sort_selection(&mut v);
assert_eq(v.get(1), 5);
```

```fides
pub fn sort_selection(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 111*

---

## sort_by

Sort a Vec using a custom comparator function.
Comparator returns: negative if a < b, 0 if equal, positive if a > b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(-3); v.push(1); v.push(-2);
sort_by(&mut v, fn(a: i64, b: i64) -> i64 { a * a - b * b });
assert_eq(v.get(0), 1);
assert_eq(v.get(2), -3);
```

```fides
pub fn sort_by(v: &mut Vec<i64>, cmp: fn(i64, i64) -> i64) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `cmp`: `fn(i64`

**Returns:** `void`

*Defined at line 143*

---

## sort_by_key

Sort a Vec stably by an integer key extracted from each element
(merge sort over a permutation). Elements with equal keys keep their
relative order. O(n log n), O(n) extra space.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(30); v.push(12); v.push(21);
sort_by_key(&mut v, fn(x: i64) -> i64 { x / 10 });
assert_eq(v.get(0), 12);
assert_eq(v.get(2), 30);
```

```fides
pub fn sort_by_key(v: &mut Vec<i64>, key: fn(i64) -> i64) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `key`: `fn(i64) -> i64`

**Returns:** `void`

*Defined at line 160*

---

## sort_binary_search_opt

Find the index of a value in a sorted Vec using binary search, or
Option::None if not found.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(3); v.push(5); v.push(7);
match sort_binary_search_opt(v, 5) {
  Option::Some(i) => { assert_eq(i, 2); }
  Option::None => { assert(false); }
}
```

```fides
pub fn sort_binary_search_opt(v: Vec<i64>, target: i64) -> Option<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`
- `target`: `i64`

**Returns:** `Option<i64>`

*Defined at line 203*

---

## sort_is_sorted

Check if a Vec is sorted in ascending order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(2);
assert(sort_is_sorted(v));
v.push(0);
assert(!sort_is_sorted(v));
```

```fides
pub fn sort_is_sorted(v: Vec<i64>) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 221*

---

## sort_is_sorted_desc

Check if a Vec is sorted in descending order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(3); v.push(3); v.push(1);
assert(sort_is_sorted_desc(v));
```

```fides
pub fn sort_is_sorted_desc(v: Vec<i64>) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 239*

---

## sort_copy

Return a sorted copy of a Vec (ascending).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(2); v.push(1);
var s = sort_copy(v);
assert_eq(s.get(0), 1);
assert_eq(v.get(0), 2);
```

```fides
pub fn sort_copy(v: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 259*

---

## sort_reverse

Reverse a Vec in place.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3);
sort_reverse(&mut v);
assert_eq(v.get(0), 3);
assert_eq(v.get(2), 1);
```

```fides
pub fn sort_reverse(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 278*

---

## sort_binary_search

Find the index of a value in a sorted Vec using binary search.
Returns the index if found, -1 if not found.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(3); v.push(5); v.push(7);
assert_eq(sort_binary_search(v, 5), 2);
assert_eq(sort_binary_search(v, 4), -1);
```

```fides
pub fn sort_binary_search(v: Vec<i64>, target: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `target`: `i64`

**Returns:** `i64`

*Defined at line 297*

---

## sort_lower_bound

Find the insertion point for a value in a sorted Vec.
Returns the index where the value should be inserted to maintain order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(3); v.push(5);
assert_eq(sort_lower_bound(v, 4), 2);
assert_eq(sort_lower_bound(v, 0), 0);
assert_eq(sort_lower_bound(v, 9), 3);
```

```fides
pub fn sort_lower_bound(v: Vec<i64>, target: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `target`: `i64`

**Returns:** `i64`

*Defined at line 324*

---

## sort_quick_range

Sort the elements at indexes lo..=hi in place, ascending (introsort).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(9); v.push(3); v.push(2); v.push(1);
sort_quick_range(&mut v, 1, 3);
assert_eq(v.get(0), 9);
assert_eq(v.get(1), 1);
```

```fides
pub fn sort_quick_range(v: &mut Vec<i64>, lo: i64, hi: i64) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `lo`: `i64`
- `hi`: `i64`

**Returns:** `void`

*Defined at line 348*

---

## sort_quick_desc_range

Sort the elements at indexes lo..=hi in place, descending (introsort).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3);
sort_quick_desc_range(&mut v, 0, 2);
assert_eq(v.get(0), 3);
```

```fides
pub fn sort_quick_desc_range(v: &mut Vec<i64>, lo: i64, hi: i64) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `lo`: `i64`
- `hi`: `i64`

**Returns:** `void`

*Defined at line 362*

---

## sort_by_range

Sort the elements at indexes lo..=hi in place with a comparison
function (negative when a comes before b) — introsort.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v: Vec<i64> = Vec::new();
v.push(3); v.push(2); v.push(1);
sort_by_range(&mut v, 0, 1, fn(a: i64, b: i64) -> i64 { a - b });
assert_eq(v.get(0), 2);
assert_eq(v.get(2), 1);
```

```fides
pub fn sort_by_range(v: &mut Vec<i64>, lo: i64, hi: i64, cmp: fn(i64, i64) -> i64) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `lo`: `i64`
- `hi`: `i64`
- `cmp`: `fn(i64`

**Returns:** `void`

*Defined at line 378*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
