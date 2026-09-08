# streaming — Fides Documentation

## Contents

- [Chunk](#chunk) (struct)
- [StreamManager](#streammanager) (struct)
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
| `cx` | `i64,          // chunk x coordinate (world_x / chunk_size)` |
| `cy` | `i64,          // chunk y coordinate` |
| `state` | `i64,       // 0 = unloaded, 1 = loading, 2 = loaded, 3 = unloading` |
| `last_used` | `i64,   // tick counter for LRU eviction` |
| `data` | `Vec<i64>,   // chunk data (tile IDs, entity IDs, etc.)` |

*Defined at line 19*

---

## StreamManager

```fides
pub struct StreamManager {
```

**Fields:**

| Name | Type |
|------|------|
| `chunk_size` | `i64,       // pixel size of each chunk` |
| `view_radius` | `i64,      // how many chunks around focus to keep loaded` |
| `focus_x` | `i64,          // focus point x (player position)` |
| `focus_y` | `i64,          // focus point y` |
| `chunks` | `Vec<Chunk>,    // all known chunks` |
| `tick` | `i64,             // global tick counter` |
| `load_count` | `i64,       // total chunks loaded` |
| `unload_count` | `i64,     // total chunks unloaded` |

*Defined at line 29*

---

## streaming_create

Create a new stream manager.
chunk_size = pixel dimensions of each chunk (e.g., 32 or 64).
view_radius = number of chunks to keep loaded in each direction from focus.

```fides
pub fn streaming_create(chunk_size: i64, view_radius: i64) -> StreamManager {
```

**Parameters:**

- `chunk_size`: `i64`
- `view_radius`: `i64`

**Returns:** `StreamManager`

*Defined at line 45*

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

*Defined at line 61*

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

*Defined at line 67*

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

*Defined at line 72*

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

*Defined at line 79*

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

*Defined at line 92*

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

*Defined at line 103*

---

## streaming_loaded_count

Get the number of currently loaded chunks.

```fides
pub fn streaming_loaded_count(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 185*

---

## streaming_total_loaded

Get total chunks ever loaded.

```fides
pub fn streaming_total_loaded(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 199*

---

## streaming_total_unloaded

Get total chunks ever unloaded.

```fides
pub fn streaming_total_unloaded(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 204*

---

## streaming_view_diameter

Get the view diameter (in pixels) of the loaded area.

```fides
pub fn streaming_view_diameter(mgr: StreamManager) -> i64 {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `i64`

*Defined at line 209*

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

*Defined at line 214*

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

*Defined at line 224*

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

*Defined at line 236*

---

## streaming_stats

Get a summary string of the stream manager state.

```fides
pub fn streaming_stats(mgr: StreamManager) -> str {
```

**Parameters:**

- `mgr`: `StreamManager`

**Returns:** `str`

*Defined at line 249*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
