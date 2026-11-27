[All modules](index.md)

# set -- Fides Documentation

## Contents

- [HashSet_new](#HashSet_new) (function)
- [HashSet_insert](#HashSet_insert) (function)
- [HashSet_contains](#HashSet_contains) (function)
- [HashSet_remove](#HashSet_remove) (function)
- [HashSet_len](#HashSet_len) (function)
- [HashSet_is_empty](#HashSet_is_empty) (function)
- [HashSet_clear](#HashSet_clear) (function)
- [HashSet_to_vec](#HashSet_to_vec) (function)
- [HashSet_union](#HashSet_union) (function)
- [HashSet_intersect](#HashSet_intersect) (function)
- [HashSet_difference](#HashSet_difference) (function)
- [HashSet_symmetric_difference](#HashSet_symmetric_difference) (function)
- [HashSet_is_subset](#HashSet_is_subset) (function)
- [HashSet_equals](#HashSet_equals) (function)

## HashSet_new

Create an empty set.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "alice");
HashSet_insert(&mut s, "bob");
HashSet_insert(&mut s, "alice");
println(int_to_str(HashSet_len(s)));
assert_eq(HashSet_len(s), 2);
assert(HashSet_contains(s, "bob"));
```

```fides
pub fn HashSet_new() -> HashMap<str, i64> {
```

**Returns:** `HashMap<str, i64>`

*Defined at line 22*

---

## HashSet_insert

Insert `v` into the set. Idempotent: inserting an existing value is a no-op.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "x");
assert(HashSet_contains(s, "x"));
```

```fides
pub fn HashSet_insert(s: &mut HashMap<str, i64>, v: str) -> void {
```

**Parameters:**

- `s`: `&mut HashMap<str`
- `v`: `str`

**Returns:** `void`

*Defined at line 32*

---

## HashSet_contains

True when `v` is a member of the set.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "x");
assert(!HashSet_contains(s, "y"));
```

```fides
pub fn HashSet_contains(s: HashMap<str, i64>, v: str) -> bool {
```

**Parameters:**

- `s`: `HashMap<str`
- `v`: `str`

**Returns:** `bool`

*Defined at line 42*

---

## HashSet_remove

Remove `v` from the set. No-op when absent.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "x");
HashSet_remove(&mut s, "x");
assert(!HashSet_contains(s, "x"));
assert(HashSet_is_empty(s));
```

```fides
pub fn HashSet_remove(s: &mut HashMap<str, i64>, v: str) -> void {
```

**Parameters:**

- `s`: `&mut HashMap<str`
- `v`: `str`

**Returns:** `void`

*Defined at line 54*

---

## HashSet_len

Number of elements in the set.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
assert_eq(HashSet_len(s), 0);
```

```fides
pub fn HashSet_len(s: HashMap<str, i64>) -> i64 {
```

**Parameters:**

- `s`: `HashMap<str`

**Returns:** `i64`

*Defined at line 63*

---

## HashSet_is_empty

True when the set is empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
assert(HashSet_is_empty(s));
```

```fides
pub fn HashSet_is_empty(s: HashMap<str, i64>) -> bool {
```

**Parameters:**

- `s`: `HashMap<str`

**Returns:** `bool`

*Defined at line 72*

---

## HashSet_clear

Remove all elements, keeping the allocated table.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "a");
HashSet_insert(&mut s, "b");
HashSet_clear(&mut s);
assert_eq(HashSet_len(s), 0);
```

```fides
pub fn HashSet_clear(s: &mut HashMap<str, i64>) -> void {
```

**Parameters:**

- `s`: `&mut HashMap<str`

**Returns:** `void`

*Defined at line 84*

---

## HashSet_to_vec

The set's members as a Vec (order unspecified).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet_new();
HashSet_insert(&mut s, "a");
var v: Vec<str> = HashSet_to_vec(s);
assert_eq(v.len(), 1);
```

```fides
pub fn HashSet_to_vec(s: HashMap<str, i64>) -> Vec<str> {
```

**Parameters:**

- `s`: `HashMap<str`

**Returns:** `Vec<str>`

*Defined at line 95*

---

## HashSet_union

A new set with every member of `a` and every member of `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x");
var b = HashSet_new(); HashSet_insert(&mut b, "y");
var u = HashSet_union(a, b);
assert_eq(HashSet_len(u), 2);
```

```fides
pub fn HashSet_union(a: HashMap<str, i64>, b: HashMap<str, i64>) -> HashMap<str, i64> {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `HashMap<str, i64>`

*Defined at line 106*

---

## HashSet_intersect

A new set with the members present in both `a` and `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x"); HashSet_insert(&mut a, "y");
var b = HashSet_new(); HashSet_insert(&mut b, "y");
var s = HashSet_intersect(a, b);
assert(HashSet_contains(s, "y") && !HashSet_contains(s, "x"));
```

```fides
pub fn HashSet_intersect(a: HashMap<str, i64>, b: HashMap<str, i64>) -> HashMap<str, i64> {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `HashMap<str, i64>`

*Defined at line 130*

---

## HashSet_difference

A new set with the members of `a` that are not in `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x"); HashSet_insert(&mut a, "y");
var b = HashSet_new(); HashSet_insert(&mut b, "y");
var d = HashSet_difference(a, b);
assert(HashSet_contains(d, "x") && !HashSet_contains(d, "y"));
```

```fides
pub fn HashSet_difference(a: HashMap<str, i64>, b: HashMap<str, i64>) -> HashMap<str, i64> {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `HashMap<str, i64>`

*Defined at line 151*

---

## HashSet_symmetric_difference

A new set with members in exactly one of `a`, `b` (XOR of membership).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x"); HashSet_insert(&mut a, "y");
var b = HashSet_new(); HashSet_insert(&mut b, "y"); HashSet_insert(&mut b, "z");
var d = HashSet_symmetric_difference(a, b);
assert(HashSet_contains(d, "x") && HashSet_contains(d, "z") && !HashSet_contains(d, "y"));
```

```fides
pub fn HashSet_symmetric_difference(a: HashMap<str, i64>, b: HashMap<str, i64>) -> HashMap<str, i64> {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `HashMap<str, i64>`

*Defined at line 172*

---

## HashSet_is_subset

True when every member of `a` is in `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x");
var b = HashSet_new(); HashSet_insert(&mut b, "x"); HashSet_insert(&mut b, "y");
assert(HashSet_is_subset(a, b) && !HashSet_is_subset(b, a));
```

```fides
pub fn HashSet_is_subset(a: HashMap<str, i64>, b: HashMap<str, i64>) -> bool {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `bool`

*Defined at line 201*

---

## HashSet_equals

True when `a` and `b` have the same members.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = HashSet_new(); HashSet_insert(&mut a, "x");
var b = HashSet_new(); HashSet_insert(&mut b, "x");
assert(HashSet_equals(a, b));
```

```fides
pub fn HashSet_equals(a: HashMap<str, i64>, b: HashMap<str, i64>) -> bool {
```

**Parameters:**

- `a`: `HashMap<str`
- `b`: `HashMap<str`

**Returns:** `bool`

*Defined at line 219*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
