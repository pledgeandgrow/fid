[All modules](index.md)

# chunk -- Fides Documentation

## Contents

- [CHUNK_SIZE_X:](#CHUNK_SIZE_X:) (const)
- [CHUNK_SIZE_Y:](#CHUNK_SIZE_Y:) (const)
- [CHUNK_SIZE_Z:](#CHUNK_SIZE_Z:) (const)
- [Chunk](#Chunk) (struct)
- [ChunkWorld](#ChunkWorld) (struct)
- [chunk_world_create](#chunk_world_create) (function)
- [chunk_create](#chunk_create) (function)
- [chunk_world_to_chunk_x](#chunk_world_to_chunk_x) (function)
- [chunk_world_to_chunk_z](#chunk_world_to_chunk_z) (function)
- [chunk_world_to_local_x](#chunk_world_to_local_x) (function)
- [chunk_world_to_local_z](#chunk_world_to_local_z) (function)
- [chunk_set_block](#chunk_set_block) (function)
- [chunk_get_block](#chunk_get_block) (function)
- [chunk_world_find](#chunk_world_find) (function)
- [chunk_world_set](#chunk_world_set) (function)
- [chunk_world_get](#chunk_world_get) (function)
- [chunk_world_update](#chunk_world_update) (function)
- [chunk_world_set_render_distance](#chunk_world_set_render_distance) (function)
- [chunk_world_chunk_count](#chunk_world_chunk_count) (function)
- [chunk_world_is_loaded](#chunk_world_is_loaded) (function)
- [chunk_world_mark_dirty](#chunk_world_mark_dirty) (function)
- [chunk_world_is_dirty](#chunk_world_is_dirty) (function)
- [chunk_world_clear_dirty](#chunk_world_clear_dirty) (function)
- [chunk_block_count](#chunk_block_count) (function)
- [chunk_height_at](#chunk_height_at) (function)
- [chunk_fill_flat](#chunk_fill_flat) (function)
- [chunk_clear](#chunk_clear) (function)
- [chunk_get_cx](#chunk_get_cx) (function)
- [chunk_get_cz](#chunk_get_cz) (function)
- [chunk_total_blocks](#chunk_total_blocks) (function)
- [chunk_world_origin_x](#chunk_world_origin_x) (function)
- [chunk_world_origin_z](#chunk_world_origin_z) (function)
- [chunk_visible_face_count](#chunk_visible_face_count) (function)

## CHUNK_SIZE_X:

```fides
pub const CHUNK_SIZE_X: i64 = 16;
```

*Defined at line 22*

---

## CHUNK_SIZE_Y:

```fides
pub const CHUNK_SIZE_Y: i64 = 256;
```

*Defined at line 23*

---

## CHUNK_SIZE_Z:

```fides
pub const CHUNK_SIZE_Z: i64 = 16;
```

*Defined at line 24*

---

## Chunk

```fides
pub struct Chunk {
```

**Fields:**

| Name | Type |
|------|------|
| `cx` | `i64` |
| `cz` | `i64` |
| `blocks` | `Vec<i64>` |
| `loaded` | `bool` |
| `dirty` | `bool` |
| `vertex_count` | `i64` |

*Defined at line 25*

---

## ChunkWorld

```fides
pub struct ChunkWorld {
```

**Fields:**

| Name | Type |
|------|------|
| `chunks` | `Vec<Chunk>` |
| `render_distance` | `i64` |
| `player_cx` | `i64` |
| `player_cz` | `i64` |
| `max_chunks` | `i64` |

*Defined at line 38*

---

## chunk_world_create

Create a new chunk world.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(chunk_world_to_chunk_x(-1), -1);
assert_eq(chunk_world_to_chunk_x(16), 1);
assert_eq(chunk_world_to_local_x(-1), 15);
var w = chunk_world_create(2);
assert_eq(chunk_world_chunk_count(&w), 0);
assert_eq(chunk_world_get(&w, 0, 5, 0), 0);
assert(!chunk_world_is_loaded(&w, -1, 0));
assert_eq(chunk_world_find(&w, 3, 3), -1);
```

```fides
pub fn chunk_world_create(max_chunks: i64) -> ChunkWorld {
```

**Parameters:**

- `max_chunks`: `i64`

**Returns:** `ChunkWorld`

*Defined at line 58*

---

## chunk_create

Create an empty chunk.

```fides
pub fn chunk_create(cx: i64, cz: i64) -> Chunk {
```

**Parameters:**

- `cx`: `i64`
- `cz`: `i64`

**Returns:** `Chunk`

*Defined at line 65*

---

## chunk_world_to_chunk_x

Convert world coordinates to chunk-relative coordinates.

```fides
pub fn chunk_world_to_chunk_x(wx: i64) -> i64 {
```

**Parameters:**

- `wx`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## chunk_world_to_chunk_z

```fides
pub fn chunk_world_to_chunk_z(wz: i64) -> i64 {
```

**Parameters:**

- `wz`: `i64`

**Returns:** `i64`

*Defined at line 85*

---

## chunk_world_to_local_x

```fides
pub fn chunk_world_to_local_x(wx: i64) -> i64 {
```

**Parameters:**

- `wx`: `i64`

**Returns:** `i64`

*Defined at line 92*

---

## chunk_world_to_local_z

```fides
pub fn chunk_world_to_local_z(wz: i64) -> i64 {
```

**Parameters:**

- `wz`: `i64`

**Returns:** `i64`

*Defined at line 100*

---

## chunk_set_block

Set a block in a chunk.

```fides
pub fn chunk_set_block(chunk: &mut Chunk, lx: i64, ly: i64, lz: i64, block_type: i64) -> void {
```

**Parameters:**

- `chunk`: `&mut Chunk`
- `lx`: `i64`
- `ly`: `i64`
- `lz`: `i64`
- `block_type`: `i64`

**Returns:** `void`

*Defined at line 114*

---

## chunk_get_block

Get a block from a chunk.

```fides
pub fn chunk_get_block(chunk: &Chunk, lx: i64, ly: i64, lz: i64) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`
- `lx`: `i64`
- `ly`: `i64`
- `lz`: `i64`

**Returns:** `i64`

*Defined at line 129*

---

## chunk_world_find

Find a chunk by its coordinates. Returns index or -1.

```fides
pub fn chunk_world_find(world: &ChunkWorld, cx: i64, cz: i64) -> i64 {
```

**Parameters:**

- `world`: `&ChunkWorld`
- `cx`: `i64`
- `cz`: `i64`

**Returns:** `i64`

*Defined at line 143*

---

## chunk_world_set

Set a block in the world (loads chunk if needed).

```fides
pub fn chunk_world_set(world: &mut ChunkWorld, wx: i64, wy: i64, wz: i64, block_type: i64) -> void {
```

**Parameters:**

- `world`: `&mut ChunkWorld`
- `wx`: `i64`
- `wy`: `i64`
- `wz`: `i64`
- `block_type`: `i64`

**Returns:** `void`

*Defined at line 155*

---

## chunk_world_get

Get a block from the world. Returns 0 if chunk not loaded.

```fides
pub fn chunk_world_get(world: &ChunkWorld, wx: i64, wy: i64, wz: i64) -> i64 {
```

**Parameters:**

- `world`: `&ChunkWorld`
- `wx`: `i64`
- `wy`: `i64`
- `wz`: `i64`

**Returns:** `i64`

*Defined at line 177*

---

## chunk_world_update

Update the chunk world: load chunks near player, unload far ones.

```fides
pub fn chunk_world_update(world: &mut ChunkWorld, px: i64, pz: i64) -> void {
```

**Parameters:**

- `world`: `&mut ChunkWorld`
- `px`: `i64`
- `pz`: `i64`

**Returns:** `void`

*Defined at line 191*

---

## chunk_world_set_render_distance

Set the render distance (in chunks).

```fides
pub fn chunk_world_set_render_distance(world: &mut ChunkWorld, distance: i64) -> void {
```

**Parameters:**

- `world`: `&mut ChunkWorld`
- `distance`: `i64`

**Returns:** `void`

*Defined at line 237*

---

## chunk_world_chunk_count

Get the number of loaded chunks.

```fides
pub fn chunk_world_chunk_count(world: &ChunkWorld) -> i64 {
```

**Parameters:**

- `world`: `&ChunkWorld`

**Returns:** `i64`

*Defined at line 242*

---

## chunk_world_is_loaded

Check if a chunk is loaded.

```fides
pub fn chunk_world_is_loaded(world: &ChunkWorld, cx: i64, cz: i64) -> bool {
```

**Parameters:**

- `world`: `&ChunkWorld`
- `cx`: `i64`
- `cz`: `i64`

**Returns:** `bool`

*Defined at line 247*

---

## chunk_world_mark_dirty

Mark a chunk as dirty (needs remeshing).

```fides
pub fn chunk_world_mark_dirty(world: &mut ChunkWorld, cx: i64, cz: i64) -> void {
```

**Parameters:**

- `world`: `&mut ChunkWorld`
- `cx`: `i64`
- `cz`: `i64`

**Returns:** `void`

*Defined at line 252*

---

## chunk_world_is_dirty

Check if a chunk needs remeshing.

```fides
pub fn chunk_world_is_dirty(world: &ChunkWorld, cx: i64, cz: i64) -> bool {
```

**Parameters:**

- `world`: `&ChunkWorld`
- `cx`: `i64`
- `cz`: `i64`

**Returns:** `bool`

*Defined at line 263*

---

## chunk_world_clear_dirty

Mark chunk as meshed (not dirty).

```fides
pub fn chunk_world_clear_dirty(world: &mut ChunkWorld, cx: i64, cz: i64) -> void {
```

**Parameters:**

- `world`: `&mut ChunkWorld`
- `cx`: `i64`
- `cz`: `i64`

**Returns:** `void`

*Defined at line 272*

---

## chunk_block_count

Count non-air blocks in a chunk.

```fides
pub fn chunk_block_count(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 283*

---

## chunk_height_at

Get the highest non-air block at a given (lx, lz) column.

```fides
pub fn chunk_height_at(chunk: &Chunk, lx: i64, lz: i64) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`
- `lx`: `i64`
- `lz`: `i64`

**Returns:** `i64`

*Defined at line 296*

---

## chunk_fill_flat

Fill a chunk with a flat terrain (blocks below height).

```fides
pub fn chunk_fill_flat(chunk: &mut Chunk, height: i64, block_type: i64) -> void {
```

**Parameters:**

- `chunk`: `&mut Chunk`
- `height`: `i64`
- `block_type`: `i64`

**Returns:** `void`

*Defined at line 314*

---

## chunk_clear

Clear all blocks in a chunk.

```fides
pub fn chunk_clear(chunk: &mut Chunk) -> void {
```

**Parameters:**

- `chunk`: `&mut Chunk`

**Returns:** `void`

*Defined at line 331*

---

## chunk_get_cx

Get the chunk X coordinate.

```fides
pub fn chunk_get_cx(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 341*

---

## chunk_get_cz

Get the chunk Z coordinate.

```fides
pub fn chunk_get_cz(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 346*

---

## chunk_total_blocks

Get the total number of blocks in a chunk.

```fides
pub fn chunk_total_blocks(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 351*

---

## chunk_world_origin_x

Get the world-space origin of a chunk.

```fides
pub fn chunk_world_origin_x(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 356*

---

## chunk_world_origin_z

```fides
pub fn chunk_world_origin_z(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 360*

---

## chunk_visible_face_count

Count visible faces in a chunk (for meshing).

```fides
pub fn chunk_visible_face_count(world: &ChunkWorld, chunk: &Chunk) -> i64 {
```

**Parameters:**

- `world`: `&ChunkWorld`
- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 365*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
