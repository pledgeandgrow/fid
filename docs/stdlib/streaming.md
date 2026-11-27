[All modules](index.md)

# streaming -- Fides Documentation

## Contents

- [Chunk](#Chunk) (struct)
- [StreamManager](#StreamManager) (struct)
- [streaming_create](#streaming_create) (function)
- [streaming_set_focus](#streaming_set_focus) (function)
- [streaming_world_to_chunk](#streaming_world_to_chunk) (function)
- [streaming_chunk_to_world](#streaming_chunk_to_world) (function)
- [streaming_find_chunk](#streaming_find_chunk) (function)
- [streaming_is_chunk_loaded](#streaming_is_chunk_loaded) (function)
- [streaming_update](#streaming_update) (function)
- [streaming_loaded_count](#streaming_loaded_count) (function)
- [streaming_total_loaded](#streaming_total_loaded) (function)
- [streaming_total_unloaded](#streaming_total_unloaded) (function)
- [streaming_view_diameter](#streaming_view_diameter) (function)
- [streaming_is_position_loaded](#streaming_is_position_loaded) (function)
- [streaming_get_chunk_data](#streaming_get_chunk_data) (function)
- [streaming_set_chunk_data](#streaming_set_chunk_data) (function)
- [streaming_stats](#streaming_stats) (function)

## Chunk

```fides
pub struct Chunk {
```

**Fields:**

| Name | Type |
|------|------|
| `cx` | `i64` |
| `cy` | `i64` |
| `state` | `i64` |
| `last_used` | `i64` |
| `data` | `Vec<i64>,  // chunk data (tile IDs, entity IDs, etc.)` |

*Defined at line 18*

---

## StreamManager

```fides
pub struct StreamManager {
```

**Fields:**

| Name | Type |
|------|------|
| `chunk_size` | `i64` |
| `view_radius` | `i64` |
| `focus_x` | `i64` |
| `focus_y` | `i64` |
| `chunks` | `Vec<Chunk>` |
| `tick` | `i64` |
| `load_count` | `i64` |
| `unload_count` | `i64,  // total chunks unloaded` |

*Defined at line 32*

---

## streaming_create

Create a new stream manager.
chunk_size = pixel dimensions of each chunk (e.g., 32 or 64).
view_radius = number of chunks to keep loaded in each direction from focus.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var m = streaming_create(32, 1);
val (cx, cy) = streaming_world_to_chunk(m, 70, 10);
assert_eq(cx, 2);
assert_eq(cy, 0);
streaming_set_focus(&mut m, 0, 0);
streaming_update(&mut m);
assert(streaming_is_chunk_loaded(m, 0, 0));
assert(!streaming_is_chunk_loaded(m, 5, 5));
assert(streaming_loaded_count(m) > 0);
```

```fides
pub fn streaming_create(chunk_size: i64, view_radius: i64) -> StreamManager {
```

**Parameters:**

- `chunk_size`: `i64`
- `view_radius`: `i64`

**Returns:** `StreamManager`

*Defined at line 66*

---

## streaming_set_focus

Set the focus point (usually the player's world position).

```fides
pub fn streaming_set_focus(mgr: &mut StreamManager, x: i64, y: i64) -> void {
```

**Parameters:**

- `mgr`: `&mut StreamManager`
- `x`: `i64`
- `y`: `i64`

**Returns:** `void`

*Defined at line 75*

---

## streaming_world_to_chunk

Get the chunk coordinate for a world position.

```fides
pub fn streaming_world_to_chunk(mgr: StreamManager, x: i64, y: i64) -> (i64, i64) {
```

**Parameters:**

- `mgr`: `StreamManager`
- `x`: `i64`
- `y`: `i64`

**Returns:** `(i64, i64)`

*Defined at line 81*

---

## streaming_chunk_to_world

Get the world-space top-left corner of a chunk.

```fides
pub fn streaming_chunk_to_world(mgr: StreamManager, cx: i64, cy: i64) -> (i64, i64) {
```

**Parameters:**

- `mgr`: `StreamManager`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `(i64, i64)`

*Defined at line 86*

---

## streaming_find_chunk

Find the index of a chunk in the manager's list. Returns -1 if not found.

```fides
pub fn streaming_find_chunk(mgr: StreamManager, cx: i64, cy: i64) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## streaming_is_chunk_loaded

Check if a chunk is currently loaded.

```fides
pub fn streaming_is_chunk_loaded(mgr: StreamManager, cx: i64, cy: i64) -> bool {
```

**Parameters:**

- `mgr`: `StreamManager`
- `cx`: `i64`
- `cy`: `i64`

**Returns:** `bool`

*Defined at line 106*

---

## streaming_update

Update the stream manager: load chunks within view radius, unload those outside.
Returns the number of chunks that changed state.

```fides
pub fn streaming_update(mgr: &mut StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `&mut StreamManager`

**Returns:** `i64`

*Defined at line 119*

---

## streaming_loaded_count

Get the number of currently loaded chunks.

```fides
pub fn streaming_loaded_count(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 201*

---

## streaming_total_loaded

Get total chunks ever loaded.

```fides
pub fn streaming_total_loaded(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 215*

---

## streaming_total_unloaded

Get total chunks ever unloaded.

```fides
pub fn streaming_total_unloaded(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 220*

---

## streaming_view_diameter

Get the view diameter (in pixels) of the loaded area.

```fides
pub fn streaming_view_diameter(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 225*

---

## streaming_is_position_loaded

Check if a world position is within the loaded area.

```fides
pub fn streaming_is_position_loaded(mgr: StreamManager, x: i64, y: i64) -> bool {
```

**Parameters:**

- `mgr`: `StreamManager`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 230*

---

## streaming_get_chunk_data

Get data from a chunk at a local (within-chunk) position.
Returns 0 if chunk not loaded or out of bounds.

```fides
pub fn streaming_get_chunk_data(mgr: StreamManager, cx: i64, cy: i64, local_x: i64, local_y: i64) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`
- `cx`: `i64`
- `cy`: `i64`
- `local_x`: `i64`
- `local_y`: `i64`

**Returns:** `i64`

*Defined at line 240*

---

## streaming_set_chunk_data

Set data in a chunk at a local position.

```fides
pub fn streaming_set_chunk_data(mgr: &mut StreamManager, cx: i64, cy: i64, local_x: i64, local_y: i64, value: i64) -> void {
```

**Parameters:**

- `mgr`: `&mut StreamManager`
- `cx`: `i64`
- `cy`: `i64`
- `local_x`: `i64`
- `local_y`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 256*

---

## streaming_stats

Get a summary string of the stream manager state.

```fides
pub fn streaming_stats(mgr: StreamManager) -> str {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `str`

*Defined at line 275*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
