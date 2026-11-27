[All modules](index.md)

# collections -- Fides Documentation

## Contents

- [Deque](#Deque) (struct)
- [Deque](#Deque) (impl)
- [LinkedList](#LinkedList) (struct)
- [LinkedList](#LinkedList) (impl)
- [Set](#Set) (struct)
- [Set](#Set) (impl)
- [hs_rd](#hs_rd) (function)
- [hs_wr](#hs_wr) (function)
- [hs_twrd](#hs_twrd) (function)
- [hs_twwr](#hs_twwr) (function)
- [HashSet](#HashSet) (struct)
- [HashSet](#HashSet) (impl)
- [BTreeMap](#BTreeMap) (struct)
- [BTreeMap](#BTreeMap) (impl)
- [BTreeSet](#BTreeSet) (struct)
- [BTreeSet](#BTreeSet) (impl)
- [RingBuffer](#RingBuffer) (struct)
- [RingBuffer](#RingBuffer) (impl)

## Deque

A double-ended queue of i64 values on a ring buffer: push and pop at
both ends are O(1) (growth amortized), with no shifting or compaction
passes. Empty/out-of-range reads return Option::None; the `*_or`
variants supply a default.
Elements are i64 (words); see queue.fd's note on generic element types.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var d = Deque::new();
d.push_back(2);
d.push_front(1);
d.push_back(3);
assert_eq(d.len(), 3);
assert_eq(d.get_or(0, 0), 1);
assert_eq(d.pop_front_or(0), 1);
assert_eq(d.pop_back_or(0), 3);
assert_eq(d.peek_front_or(0), 2);
```

```fides
pub struct Deque<T> {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `head` | `i64` |
| `count` | `i64` |

*Defined at line 34*

---

## Deque

```fides
impl Deque {
```

**Methods:**

### new

Create a new empty deque.

```fides
pub fn new() -> Deque {
```

### grow

```fides
fn grow(self: &mut Deque) -> void {
```

### push_front

Add a value to the front. O(1).

```fides
pub fn push_front(self: &mut Deque, value: i64) -> void {
```

### push_back

Add a value to the back. O(1) amortized.

```fides
pub fn push_back(self: &mut Deque, value: i64) -> void {
```

### pop_front

Remove and return the front value, or Option::None if empty.

```fides
pub fn pop_front(self: &mut Deque) -> Option<i64> {
```

### pop_front_or

pop_front, or `default` when the deque is empty.

```fides
pub fn pop_front_or(self: &mut Deque, default: i64) -> i64 {
```

### pop_back

Remove and return the back value, or Option::None if empty.

```fides
pub fn pop_back(self: &mut Deque) -> Option<i64> {
```

### pop_back_or

pop_back, or `default` when the deque is empty.

```fides
pub fn pop_back_or(self: &mut Deque, default: i64) -> i64 {
```

### peek_front

Peek at the front value without removing it, or Option::None.

```fides
pub fn peek_front(self: Deque) -> Option<i64> {
```

### peek_front_or

peek_front, or `default` when the deque is empty.

```fides
pub fn peek_front_or(self: Deque, default: i64) -> i64 {
```

### peek_back

Peek at the back value without removing it, or Option::None.

```fides
pub fn peek_back(self: Deque) -> Option<i64> {
```

### peek_back_or

peek_back, or `default` when the deque is empty.

```fides
pub fn peek_back_or(self: Deque, default: i64) -> i64 {
```

### is_empty

Check if the deque is empty.

```fides
pub fn is_empty(self: Deque) -> bool {
```

### len

Get the number of elements.

```fides
pub fn len(self: Deque) -> i64 {
```

### clear

Clear all elements, keeping the allocated ring.

```fides
pub fn clear(self: &mut Deque) -> void {
```

### get

Get element at index (0 = front), or Option::None when out of range.

```fides
pub fn get(self: Deque, idx: i64) -> Option<i64> {
```

### get_or

get, or `default` when out of range.

```fides
pub fn get_or(self: Deque, idx: i64, default: i64) -> i64 {
```

### to_vec

The elements in order (front first) as a Vec.

```fides
pub fn to_vec(self: Deque) -> Vec<i64> {
```

### drain

Drain all elements into a Vec (front first).

```fides
pub fn drain(self: &mut Deque) -> Vec<i64> {
```

*Defined at line 42*

---

## LinkedList

A singly linked list using parallel arrays for values and next pointers.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var l = LinkedList::new();
l.push_back(2);
l.push_front(1);
l.push_back(3);
assert_eq(l.len(), 3);
assert_eq(l.get(2), 3);
assert_eq(l.find(2), 1);
assert_eq(l.pop_front(), 1);
assert(l.contains(3));
```

```fides
pub struct LinkedList {
```

**Fields:**

| Name | Type |
|------|------|
| `values` | `Vec<i64>` |
| `nexts` | `Vec<i64>` |
| `head` | `i64` |
| `tail` | `i64` |
| `size` | `i64` |

*Defined at line 257*

---

## LinkedList

```fides
impl LinkedList {
```

**Methods:**

### new

Create a new empty linked list.

```fides
pub fn new() -> LinkedList {
```

### push_back

Add a value to the end of the list.

```fides
pub fn push_back(self: &mut LinkedList, value: i64) -> void {
```

### push_front

Add a value to the front of the list.

```fides
pub fn push_front(self: &mut LinkedList, value: i64) -> void {
```

### pop_front

Remove and return the front value. Returns 0 if empty.

```fides
pub fn pop_front(self: &mut LinkedList) -> i64 {
```

### peek_front

Peek at the front value. Returns 0 if empty.

```fides
pub fn peek_front(self: LinkedList) -> i64 {
```

### is_empty

Check if the list is empty.

```fides
pub fn is_empty(self: LinkedList) -> bool {
```

### len

Get the number of elements.

```fides
pub fn len(self: LinkedList) -> i64 {
```

### get

Get the value at a given index (O(n)).

```fides
pub fn get(self: LinkedList, idx: i64) -> i64 {
```

### to_vec

Convert to a Vec.

```fides
pub fn to_vec(self: LinkedList) -> Vec<i64> {
```

### from_vec

Create a linked list from a Vec.

```fides
pub fn from_vec(v: Vec<i64>) -> LinkedList {
```

### find

Find the first index of a value. Returns -1 if not found.

```fides
pub fn find(self: LinkedList, value: i64) -> i64 {
```

### contains

Check if the list contains a value.

```fides
pub fn contains(self: LinkedList, value: i64) -> bool {
```

*Defined at line 269*

---

## Set

A simple set of i64 values backed by a Vec.
Not efficient for large sets — use HashMap for that.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = Set::new();
a.insert(1); a.insert(2); a.insert(2);
var b = Set::new();
b.insert(2); b.insert(3);
assert_eq(a.len(), 2);
assert_eq(Set::union(a, b).len(), 3);
assert_eq(Set::intersection(a, b).len(), 1);
assert(Set::difference(a, b).contains(1));
```

```fides
pub struct Set {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |

*Defined at line 406*

---

## Set

```fides
impl Set {
```

**Methods:**

### new

Create a new empty set.

```fides
pub fn new() -> Set {
```

### insert

Insert a value into the set (no-op if already present).

```fides
pub fn insert(self: &mut Set, value: i64) -> void {
```

### remove

Remove a value from the set. Returns true if it was present.

```fides
pub fn remove(self: &mut Set, value: i64) -> bool {
```

### contains

Check if the set contains a value.

```fides
pub fn contains(self: Set, value: i64) -> bool {
```

### len

Get the number of elements.

```fides
pub fn len(self: Set) -> i64 {
```

### is_empty

Check if the set is empty.

```fides
pub fn is_empty(self: Set) -> bool {
```

### clear

Clear the set.

```fides
pub fn clear(self: &mut Set) -> void {
```

### to_vec

Convert to a Vec.

```fides
pub fn to_vec(self: Set) -> Vec<i64> {
```

### from_vec

Create a set from a Vec.

```fides
pub fn from_vec(v: Vec<i64>) -> Set {
```

### union

Union: returns a new set with elements from both sets.

```fides
pub fn union(a: Set, b: Set) -> Set {
```

### intersection

Intersection: returns a new set with elements in both sets.

```fides
pub fn intersection(a: Set, b: Set) -> Set {
```

### difference

Difference: returns a new set with elements in a but not in b.

```fides
pub fn difference(a: Set, b: Set) -> Set {
```

*Defined at line 410*

---

## hs_rd

```fides
pub fn hs_rd(base: i64, idx: i64) -> i64 {
```

**Parameters:**

- `base`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 554*

---

## hs_wr

```fides
pub fn hs_wr(base: i64, idx: i64, v: i64) -> void {
```

**Parameters:**

- `base`: `i64`
- `idx`: `i64`
- `v`: `i64`

**Returns:** `void`

*Defined at line 566*

---

## hs_twrd

```fides
pub fn hs_twrd(table: i64, word: i64) -> i64 {
```

**Parameters:**

- `table`: `i64`
- `word`: `i64`

**Returns:** `i64`

*Defined at line 576*

---

## hs_twwr

```fides
pub fn hs_twwr(table: i64, word: i64, v: i64) -> void {
```

**Parameters:**

- `table`: `i64`
- `word`: `i64`
- `v`: `i64`

**Returns:** `void`

*Defined at line 587*

---

## HashSet

A set of i64 values in a hash table (open addressing).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = HashSet::new();
s.insert(10); s.insert(20); s.insert(10);
assert_eq(s.len(), 2);
assert(s.contains(20));
assert(s.remove(10));
assert(!s.contains(10));
s.insert(0); s.insert(-1);
assert(s.contains(0) && s.contains(-1));
```

```fides
pub struct HashSet {
```

**Fields:**

| Name | Type |
|------|------|
| `table` | `i64` |

*Defined at line 606*

---

## HashSet

```fides
impl HashSet {
```

**Methods:**

### new

Create a new empty hash set.

```fides
pub fn new() -> HashSet {
```

### find_slot

```fides
fn find_slot(self: HashSet, key: i64) -> i64 {
```

### grow

```fides
fn grow(self: HashSet) -> void {
```

### insert

Insert a value into the set (no-op if already present). Any i64 is a
valid member, including 0 and -1.

```fides
pub fn insert(self: HashSet, value: i64) -> void {
```

### contains

Check if the set contains a value.

```fides
pub fn contains(self: HashSet, value: i64) -> bool {
```

### remove

Remove a value from the set. Returns true if it was present.

```fides
pub fn remove(self: HashSet, value: i64) -> bool {
```

### len

Get the number of elements.

```fides
pub fn len(self: HashSet) -> i64 {
```

### is_empty

Check if the set is empty.

```fides
pub fn is_empty(self: HashSet) -> bool {
```

### clear

Clear the set, keeping the allocated table.

```fides
pub fn clear(self: HashSet) -> void {
```

### destroy

Free the table's slot arrays. After destroy() the set is empty and
may be reused; the small table block itself is reclaimed with the
set's owner like any struct.

```fides
pub fn destroy(self: HashSet) -> void {
```

### to_vec

Convert to a Vec of all elements.

```fides
pub fn to_vec(self: HashSet) -> Vec<i64> {
```

### from_vec

Create a hash set from a Vec.

```fides
pub fn from_vec(v: Vec<i64>) -> HashSet {
```

### union

Union: returns a new set with elements from both sets.

```fides
pub fn union(a: HashSet, b: HashSet) -> HashSet {
```

### intersection

Intersection: returns a new set with elements in both sets.

```fides
pub fn intersection(a: HashSet, b: HashSet) -> HashSet {
```

### difference

Difference: returns a new set with elements in a but not in b.

```fides
pub fn difference(a: HashSet, b: HashSet) -> HashSet {
```

*Defined at line 612*

---

## BTreeMap

An ordered map from i64 keys to i64 values (a sorted Vec of keys and a
parallel Vec of values); keys() and values() come in key order.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = BTreeMap::new();
m.insert(30, 300); m.insert(10, 100); m.insert(20, 200);
assert_eq(m.get(20), 200);
assert_eq(m.get_or(99, -1), -1);
assert_eq(m.first_key(), 10);
assert_eq(m.last_key(), 30);
assert_eq(m.keys().get(1), 20);
```

```fides
pub struct BTreeMap {
```

**Fields:**

| Name | Type |
|------|------|
| `keys` | `Vec<i64>` |
| `values` | `Vec<i64>,  // parallel values (values[i] corresponds to keys[i])` |

*Defined at line 951*

---

## BTreeMap

```fides
impl BTreeMap {
```

**Methods:**

### new

Create a new empty BTreeMap.

```fides
pub fn new() -> BTreeMap {
```

### binary_search

Binary search for a key. Returns index if found, or -(insertion_point + 1) if not.

```fides
fn binary_search(self: BTreeMap, key: i64) -> i64 {
```

### insert

Insert a key-value pair. If the key exists, updates the value.

```fides
pub fn insert(self: &mut BTreeMap, key: i64, value: i64) -> void {
```

### get

Get the value for a key. Returns 0 if not found.

```fides
pub fn get(self: BTreeMap, key: i64) -> i64 {
```

### get_or

Get the value for a key, or a default if not found.

```fides
pub fn get_or(self: BTreeMap, key: i64, default: i64) -> i64 {
```

### contains

Check if the key exists.

```fides
pub fn contains(self: BTreeMap, key: i64) -> bool {
```

### remove

Remove a key-value pair. Returns true if the key was present.

```fides
pub fn remove(self: &mut BTreeMap, key: i64) -> bool {
```

### len

Get the number of entries.

```fides
pub fn len(self: BTreeMap) -> i64 {
```

### is_empty

Check if the map is empty.

```fides
pub fn is_empty(self: BTreeMap) -> bool {
```

### clear

Clear the map.

```fides
pub fn clear(self: &mut BTreeMap) -> void {
```

### keys

Get all keys in sorted order.

```fides
pub fn keys(self: BTreeMap) -> Vec<i64> {
```

### values

Get all values (in key order).

```fides
pub fn values(self: BTreeMap) -> Vec<i64> {
```

### first_key

Get the first (smallest) key. Returns 0 if empty.

```fides
pub fn first_key(self: BTreeMap) -> i64 {
```

### last_key

Get the last (largest) key. Returns 0 if empty.

```fides
pub fn last_key(self: BTreeMap) -> i64 {
```

### from_vecs

Create a BTreeMap from a Vec of keys and a Vec of values.
Inserts all pairs (does not assume sorted input).

```fides
pub fn from_vecs(ks: Vec<i64>, vs: Vec<i64>) -> BTreeMap {
```

*Defined at line 957*

---

## BTreeSet

An ordered set of i64 values (a sorted Vec); to_vec() is ascending.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var s = BTreeSet::new();
s.insert(5); s.insert(1); s.insert(3); s.insert(1);
assert_eq(s.len(), 3);
assert_eq(s.first(), 1);
assert_eq(s.last(), 5);
assert_eq(s.to_vec().get(1), 3);
```

```fides
pub struct BTreeSet {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>,  // sorted keys` |

*Defined at line 1103*

---

## BTreeSet

```fides
impl BTreeSet {
```

**Methods:**

### new

Create a new empty BTreeSet.

```fides
pub fn new() -> BTreeSet {
```

### binary_search

Binary search for a key. Returns index if found, or -(insertion_point + 1) if not.

```fides
fn binary_search(self: BTreeSet, key: i64) -> i64 {
```

### insert

Insert a value into the set (no-op if already present).

```fides
pub fn insert(self: &mut BTreeSet, value: i64) -> void {
```

### contains

Check if the set contains a value.

```fides
pub fn contains(self: BTreeSet, value: i64) -> bool {
```

### remove

Remove a value from the set. Returns true if it was present.

```fides
pub fn remove(self: &mut BTreeSet, value: i64) -> bool {
```

### len

Get the number of elements.

```fides
pub fn len(self: BTreeSet) -> i64 {
```

### is_empty

Check if the set is empty.

```fides
pub fn is_empty(self: BTreeSet) -> bool {
```

### clear

Clear the set.

```fides
pub fn clear(self: &mut BTreeSet) -> void {
```

### to_vec

Convert to a Vec (sorted order).

```fides
pub fn to_vec(self: BTreeSet) -> Vec<i64> {
```

### from_vec

Create a BTreeSet from a Vec.

```fides
pub fn from_vec(v: Vec<i64>) -> BTreeSet {
```

### first

Get the first (smallest) element. Returns 0 if empty.

```fides
pub fn first(self: BTreeSet) -> i64 {
```

### last

Get the last (largest) element. Returns 0 if empty.

```fides
pub fn last(self: BTreeSet) -> i64 {
```

### union

Union: returns a new set with elements from both sets.

```fides
pub fn union(a: BTreeSet, b: BTreeSet) -> BTreeSet {
```

### intersection

Intersection: returns a new set with elements in both sets.

```fides
pub fn intersection(a: BTreeSet, b: BTreeSet) -> BTreeSet {
```

### difference

Difference: returns a new set with elements in a but not in b.

```fides
pub fn difference(a: BTreeSet, b: BTreeSet) -> BTreeSet {
```

*Defined at line 1107*

---

## RingBuffer

A fixed-capacity circular buffer: push overwrites the oldest element
when full, try_push refuses instead.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = RingBuffer::new(3);
r.push(1); r.push(2); r.push(3); r.push(4);
assert(r.is_full());
assert_eq(r.peek_front(), 2);
assert(!r.try_push(5));
assert_eq(r.pop_front(), 2);
assert_eq(r.to_vec().get(1), 4);
```

```fides
pub struct RingBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `capacity` | `i64` |
| `head` | `i64` |
| `len` | `i64,  // number of live elements` |

*Defined at line 1265*

---

## RingBuffer

```fides
impl RingBuffer {
```

**Methods:**

### new

Create a new ring buffer with the given capacity.

```fides
pub fn new(cap: i64) -> RingBuffer {
```

### push

Push a value to the back. If full, overwrites the oldest element.

```fides
pub fn push(self: &mut RingBuffer, value: i64) -> void {
```

### try_push

Push a value to the back. If full, returns false (no overwrite).

```fides
pub fn try_push(self: &mut RingBuffer, value: i64) -> bool {
```

### pop_front

Remove and return the front (oldest) value. Returns 0 if empty.

```fides
pub fn pop_front(self: &mut RingBuffer) -> i64 {
```

### peek_front

Peek at the front value without removing it. Returns 0 if empty.

```fides
pub fn peek_front(self: RingBuffer) -> i64 {
```

### peek_back

Peek at the back (newest) value without removing it. Returns 0 if empty.

```fides
pub fn peek_back(self: RingBuffer) -> i64 {
```

### is_empty

Check if the buffer is empty.

```fides
pub fn is_empty(self: RingBuffer) -> bool {
```

### is_full

Check if the buffer is full.

```fides
pub fn is_full(self: RingBuffer) -> bool {
```

### len

Get the number of live elements.

```fides
pub fn len(self: RingBuffer) -> i64 {
```

### cap

Get the capacity.

```fides
pub fn cap(self: RingBuffer) -> i64 {
```

### free

Get the number of free slots.

```fides
pub fn free(self: RingBuffer) -> i64 {
```

### clear

Clear all elements.

```fides
pub fn clear(self: &mut RingBuffer) -> void {
```

### get

Get element at logical index (0 = front).

```fides
pub fn get(self: RingBuffer, idx: i64) -> i64 {
```

### set

Set element at logical index (0 = front).

```fides
pub fn set(self: &mut RingBuffer, idx: i64, value: i64) -> void {
```

### to_vec

Convert to a Vec (front to back order).

```fides
pub fn to_vec(self: RingBuffer) -> Vec<i64> {
```

*Defined at line 1275*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
