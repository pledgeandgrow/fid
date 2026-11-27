[All modules](index.md)

# flowfield -- Fides Documentation

## Contents

- [Flowfield](#Flowfield) (struct)
- [flowfield_create](#flowfield_create) (function)
- [flowfield_set_target](#flowfield_set_target) (function)
- [flowfield_set_blocked](#flowfield_set_blocked) (function)
- [flowfield_set_passable](#flowfield_set_passable) (function)
- [flowfield_is_blocked](#flowfield_is_blocked) (function)
- [flowfield_compute](#flowfield_compute) (function)
- [flowfield_compute_dirs](#flowfield_compute_dirs) (function)
- [flowfield_get_dir](#flowfield_get_dir) (function)
- [flowfield_get_distance](#flowfield_get_distance) (function)
- [flowfield_has_path](#flowfield_has_path) (function)
- [flowfield_is_computed](#flowfield_is_computed) (function)
- [flowfield_add_cost](#flowfield_add_cost) (function)
- [flowfield_reset](#flowfield_reset) (function)

## Flowfield

```fides
pub struct Flowfield {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `distance` | `Vec<i64>` |
| `dir_x` | `Vec<i64>` |
| `dir_y` | `Vec<i64>` |
| `target_x` | `i64` |
| `target_y` | `i64` |
| `computed` | `bool` |

*Defined at line 24*

---

## flowfield_create

Create a new empty flowfield (all cells unvisited).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var ff = flowfield_create(3, 3);
flowfield_set_target(&mut ff, 0, 0);
flowfield_set_blocked(&mut ff, 1, 0);
assert(flowfield_is_blocked(ff, 1, 0));
assert(!flowfield_is_computed(ff));
flowfield_compute(&mut ff);
assert(flowfield_is_computed(ff));
assert_eq(flowfield_get_distance(ff, 0, 1), 1);
assert_eq(flowfield_get_distance(ff, 2, 0), 4);
assert(flowfield_has_path(ff, 2, 2));
assert(!flowfield_has_path(ff, 1, 0));
```

```fides
pub fn flowfield_create(width: i64, height: i64) -> Flowfield {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`

**Returns:** `Flowfield`

*Defined at line 54*

---

## flowfield_set_target

Set the destination/target cell.

```fides
pub fn flowfield_set_target(ff: &mut Flowfield, x: i64, y: i64) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 74*

---

## flowfield_set_blocked

Mark a cell as blocked (impassable).

```fides
pub fn flowfield_set_blocked(ff: &mut Flowfield, x: i64, y: i64) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 84*

---

## flowfield_set_passable

Mark a cell as passable (clear blocked status).

```fides
pub fn flowfield_set_passable(ff: &mut Flowfield, x: i64, y: i64) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 93*

---

## flowfield_is_blocked

Check if a cell is blocked.

```fides
pub fn flowfield_is_blocked(ff: Flowfield, x: i64, y: i64) -> bool {
```

**Parameters:**

- `ff`: `Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 102*

---

## flowfield_compute

Compute the distance field using BFS from the target.
Uses a simple queue (Vec with head index) since Fides has no deque yet.

```fides
pub fn flowfield_compute(ff: &mut Flowfield) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`

**Returns:** `void`

*Defined at line 117*

---

## flowfield_compute_dirs

Compute direction vectors from the distance field.
Each cell's direction points toward the neighbor with the lowest distance.

```fides
pub fn flowfield_compute_dirs(ff: &mut Flowfield) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`

**Returns:** `void`

*Defined at line 189*

---

## flowfield_get_dir

Get the direction vector at a cell. Returns (dx, dy).
The vector is normalized (one of: (0,0), (±1,0), (0,±1)).

```fides
pub fn flowfield_get_dir(ff: Flowfield, x: i64, y: i64) -> (i64, i64) {
```

**Parameters:**

- `ff`: `Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `(i64, i64)`

*Defined at line 258*

---

## flowfield_get_distance

Get the distance to the target from a cell. -1 = blocked, -2 = unreachable.

```fides
pub fn flowfield_get_distance(ff: Flowfield, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `ff`: `Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 267*

---

## flowfield_has_path

Check if a cell has a valid path to the target.

```fides
pub fn flowfield_has_path(ff: Flowfield, x: i64, y: i64) -> bool {
```

**Parameters:**

- `ff`: `Flowfield`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 275*

---

## flowfield_is_computed

Check if the flowfield has been computed.

```fides
pub fn flowfield_is_computed(ff: Flowfield) -> bool {
```

**Parameters:**

- `ff`: `Flowfield`

**Returns:** `bool`

*Defined at line 284*

---

## flowfield_add_cost

Add a cost penalty to a cell (higher = less desirable path).

```fides
pub fn flowfield_add_cost(ff: &mut Flowfield, x: i64, y: i64, cost: i64) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`
- `x`: `i64`
- `y`: `i64`
- `cost`: `i64`

**Returns:** `void`

*Defined at line 294*

---

## flowfield_reset

Reset the flowfield (clear distances and directions, keep blocked tiles).

```fides
pub fn flowfield_reset(ff: &mut Flowfield) -> void {
```

**Parameters:**

- `ff`: `&mut Flowfield`

**Returns:** `void`

*Defined at line 308*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
