# chunk — Fides Documentation

## Contents

- [CHUNK_SIZE_X:](#chunk_size_x:) (const)
- [CHUNK_SIZE_Y:](#chunk_size_y:) (const)
- [CHUNK_SIZE_Z:](#chunk_size_z:) (const)
- [Chunk](#chunk) (struct)
- [ChunkWorld](#chunkworld) (struct)
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

*Defined at line 25*

---

## CHUNK_SIZE_Y:

```fides
pub const CHUNK_SIZE_Y: i64 = 256;
```

*Defined at line 26*

---

## CHUNK_SIZE_Z:

```fides
pub const CHUNK_SIZE_Z: i64 = 16;
```

*Defined at line 27*

---

## Chunk

```fides
pub struct Chunk {
```

**Fields:**

| Name | Type |
|------|------|
| `cx` | `i64,           // chunk X coordinate (world_x / CHUNK_SIZE_X)` |
| `cz` | `i64,           // chunk Z coordinate` |
| `blocks` | `Vec<i64>,  // CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z` |
| `loaded` | `bool` |
| `dirty` | `bool,        // needs remeshing` |
| `vertex_count` | `i64` |

*Defined at line 29*

---

## ChunkWorld

```fides
pub struct ChunkWorld {
```

**Fields:**

| Name | Type |
|------|------|
| `chunks` | `Vec<Chunk>` |
| `render_distance` | `i64,  // chunks to load around player` |
| `player_cx` | `i64` |
| `player_cz` | `i64` |
| `max_chunks` | `i64` |

*Defined at line 38*

---

## chunk_world_create

Create a new chunk world.

```fides
pub fn chunk_world_create(max_chunks: i64) -> ChunkWorld {
```

**Parameters:**

- `max_chunks`: `i64`

**Returns:** `ChunkWorld`

*Defined at line 47*

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

*Defined at line 58*

---

## chunk_world_to_chunk_x

Convert world coordinates to chunk-relative coordinates.

```fides
pub fn chunk_world_to_chunk_x(wx: i64) -> i64 {
```

**Parameters:**

- `wx`: `i64`

**Returns:** `i64`

*Defined at line 75*

---

## chunk_world_to_chunk_z

```fides
pub fn chunk_world_to_chunk_z(wz: i64) -> i64 {
```

**Parameters:**

- `wz`: `i64`

**Returns:** `i64`

*Defined at line 82*

---

## chunk_world_to_local_x

```fides
pub fn chunk_world_to_local_x(wx: i64) -> i64 {
```

**Parameters:**

- `wx`: `i64`

**Returns:** `i64`

*Defined at line 89*

---

## chunk_world_to_local_z

```fides
pub fn chunk_world_to_local_z(wz: i64) -> i64 {
```

**Parameters:**

- `wz`: `i64`

**Returns:** `i64`

*Defined at line 95*

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

*Defined at line 107*

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

*Defined at line 116*

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

*Defined at line 124*

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

*Defined at line 136*

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

*Defined at line 156*

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

*Defined at line 168*

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

*Defined at line 210*

---

## chunk_world_chunk_count

Get the number of loaded chunks.

```fides
pub fn chunk_world_chunk_count(world: &ChunkWorld) -> i64 {
```

**Parameters:**

- `world`: `&ChunkWorld`

**Returns:** `i64`

*Defined at line 215*

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

*Defined at line 220*

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

*Defined at line 225*

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

*Defined at line 234*

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

*Defined at line 241*

---

## chunk_block_count

Count non-air blocks in a chunk.

```fides
pub fn chunk_block_count(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 250*

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

*Defined at line 261*

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

*Defined at line 273*

---

## chunk_clear

Clear all blocks in a chunk.

```fides
pub fn chunk_clear(chunk: &mut Chunk) -> void {
```

**Parameters:**

- `chunk`: `&mut Chunk`

**Returns:** `void`

*Defined at line 290*

---

## chunk_get_cx

Get the chunk X coordinate.

```fides
pub fn chunk_get_cx(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 300*

---

## chunk_get_cz

Get the chunk Z coordinate.

```fides
pub fn chunk_get_cz(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 305*

---

## chunk_total_blocks

Get the total number of blocks in a chunk.

```fides
pub fn chunk_total_blocks(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 310*

---

## chunk_world_origin_x

Get the world-space origin of a chunk.

```fides
pub fn chunk_world_origin_x(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 315*

---

## chunk_world_origin_z

```fides
pub fn chunk_world_origin_z(chunk: &Chunk) -> i64 {
```

**Parameters:**

- `chunk`: `&Chunk`

**Returns:** `i64`

*Defined at line 319*

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

*Defined at line 324*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
