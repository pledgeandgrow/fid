# tilemap — Fides Documentation

## Contents

- [TILE_EMPTY:](#tile_empty:) (const)
- [TILE_WALL:](#tile_wall:) (const)
- [TILE_FLOOR:](#tile_floor:) (const)
- [TILE_WATER:](#tile_water:) (const)
- [TILE_GRASS:](#tile_grass:) (const)
- [TILE_DIRT:](#tile_dirt:) (const)
- [TILE_STONE:](#tile_stone:) (const)
- [TILE_SAND:](#tile_sand:) (const)
- [TILE_LAVA:](#tile_lava:) (const)
- [TILE_ICE:](#tile_ice:) (const)
- [Tilemap](#tilemap) (struct)
- [tilemap_create](#tilemap_create) (function)
- [tilemap_from_grid](#tilemap_from_grid) (function)
- [tilemap_get](#tilemap_get) (function)
- [tilemap_set](#tilemap_set) (function)
- [tilemap_set_with_solid](#tilemap_set_with_solid) (function)
- [tilemap_is_solid](#tilemap_is_solid) (function)
- [tilemap_is_passable](#tilemap_is_passable) (function)
- [tilemap_get_at_pixel](#tilemap_get_at_pixel) (function)
- [tilemap_is_solid_at_pixel](#tilemap_is_solid_at_pixel) (function)
- [tilemap_box_collides](#tilemap_box_collides) (function)
- [tilemap_move_x](#tilemap_move_x) (function)
- [tilemap_move_y](#tilemap_move_y) (function)
- [tilemap_auto_tile](#tilemap_auto_tile) (function)
- [tilemap_auto_tile_4dir](#tilemap_auto_tile_4dir) (function)
- [tilemap_fill_rect](#tilemap_fill_rect) (function)
- [tilemap_fill_all](#tilemap_fill_all) (function)
- [tilemap_fill_border](#tilemap_fill_border) (function)
- [tilemap_count_tile](#tilemap_count_tile) (function)
- [tilemap_count_solid](#tilemap_count_solid) (function)
- [tilemap_line_of_sight](#tilemap_line_of_sight) (function)

## TILE_EMPTY:

```fides
pub const TILE_EMPTY: i64 = 0;
```

*Defined at line 21*

---

## TILE_WALL:

```fides
pub const TILE_WALL: i64 = 1;
```

*Defined at line 22*

---

## TILE_FLOOR:

```fides
pub const TILE_FLOOR: i64 = 2;
```

*Defined at line 23*

---

## TILE_WATER:

```fides
pub const TILE_WATER: i64 = 3;
```

*Defined at line 24*

---

## TILE_GRASS:

```fides
pub const TILE_GRASS: i64 = 4;
```

*Defined at line 25*

---

## TILE_DIRT:

```fides
pub const TILE_DIRT: i64 = 5;
```

*Defined at line 26*

---

## TILE_STONE:

```fides
pub const TILE_STONE: i64 = 6;
```

*Defined at line 27*

---

## TILE_SAND:

```fides
pub const TILE_SAND: i64 = 7;
```

*Defined at line 28*

---

## TILE_LAVA:

```fides
pub const TILE_LAVA: i64 = 8;
```

*Defined at line 29*

---

## TILE_ICE:

```fides
pub const TILE_ICE: i64 = 9;
```

*Defined at line 30*

---

## Tilemap

```fides
pub struct Tilemap {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `tile_size` | `i64,       // pixel size of each tile` |
| `tiles` | `Vec<i64>,      // flat grid: tiles[y * width + x]` |
| `solid_flags` | `Vec<i64>, // 1 = solid (blocks movement), 0 = passable` |

*Defined at line 37*

---

## tilemap_create

Create a new tilemap filled with empty tiles.

```fides
pub fn tilemap_create(width: i64, height: i64, tile_size: i64) -> Tilemap {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `tile_size`: `i64`

**Returns:** `Tilemap`

*Defined at line 48*

---

## tilemap_from_grid

Create a tilemap from an existing tile grid (flat Vec<i64>).

```fides
pub fn tilemap_from_grid(width: i64, height: i64, tile_size: i64, tiles: Vec<i64>) -> Tilemap {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `tile_size`: `i64`
- `tiles`: `Vec<i64>`

**Returns:** `Tilemap`

*Defined at line 67*

---

## tilemap_get

Get the tile ID at (x, y). Returns TILE_EMPTY if out of bounds.

```fides
pub fn tilemap_get(tm: Tilemap, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 94*

---

## tilemap_set

Set the tile ID at (x, y). Does nothing if out of bounds.
Updates the solid flag based on the tile ID (non-empty = solid by default).

```fides
pub fn tilemap_set(tm: &mut Tilemap, x: i64, y: i64, tile_id: i64) -> void {
```

**Parameters:**

- `tm`: `&mut Tilemap`
- `x`: `i64`
- `y`: `i64`
- `tile_id`: `i64`

**Returns:** `void`

*Defined at line 103*

---

## tilemap_set_with_solid

Set the tile ID at (x, y) with explicit solid flag.

```fides
pub fn tilemap_set_with_solid(tm: &mut Tilemap, x: i64, y: i64, tile_id: i64, is_solid: bool) -> void {
```

**Parameters:**

- `tm`: `&mut Tilemap`
- `x`: `i64`
- `y`: `i64`
- `tile_id`: `i64`
- `is_solid`: `bool`

**Returns:** `void`

*Defined at line 117*

---

## tilemap_is_solid

Check if a tile is solid (blocks movement).

```fides
pub fn tilemap_is_solid(tm: Tilemap, x: i64, y: i64) -> bool {
```

**Parameters:**

- `tm`: `Tilemap`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 131*

---

## tilemap_is_passable

Check if a tile is passable (not solid and in bounds).

```fides
pub fn tilemap_is_passable(tm: Tilemap, x: i64, y: i64) -> bool {
```

**Parameters:**

- `tm`: `Tilemap`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 140*

---

## tilemap_get_at_pixel

Get the tile at a pixel position (converts pixels to tile coords).

```fides
pub fn tilemap_get_at_pixel(tm: Tilemap, px: i64, py: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `px`: `i64`
- `py`: `i64`

**Returns:** `i64`

*Defined at line 145*

---

## tilemap_is_solid_at_pixel

Check if a pixel position is solid.

```fides
pub fn tilemap_is_solid_at_pixel(tm: Tilemap, px: i64, py: i64) -> bool {
```

**Parameters:**

- `tm`: `Tilemap`
- `px`: `i64`
- `py`: `i64`

**Returns:** `bool`

*Defined at line 152*

---

## tilemap_box_collides

Check if an axis-aligned bounding box (AABB) collides with any solid tile.
(px, py) = top-left corner, (pw, ph) = width/height in pixels.

```fides
pub fn tilemap_box_collides(tm: Tilemap, px: i64, py: i64, pw: i64, ph: i64) -> bool {
```

**Parameters:**

- `tm`: `Tilemap`
- `px`: `i64`
- `py`: `i64`
- `pw`: `i64`
- `ph`: `i64`

**Returns:** `bool`

*Defined at line 162*

---

## tilemap_move_x

Resolve horizontal movement: move px by dx, stopping at solid tiles.
Returns the adjusted x position (may be less than px + dx if blocked).

```fides
pub fn tilemap_move_x(tm: Tilemap, px: i64, py: i64, pw: i64, ph: i64, dx: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `px`: `i64`
- `py`: `i64`
- `pw`: `i64`
- `ph`: `i64`
- `dx`: `i64`

**Returns:** `i64`

*Defined at line 185*

---

## tilemap_move_y

Resolve vertical movement: move py by dy, stopping at solid tiles.
Returns the adjusted y position.

```fides
pub fn tilemap_move_y(tm: Tilemap, px: i64, py: i64, pw: i64, ph: i64, dy: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `px`: `i64`
- `py`: `i64`
- `pw`: `i64`
- `ph`: `i64`
- `dy`: `i64`

**Returns:** `i64`

*Defined at line 204*

---

## tilemap_auto_tile

Compute the 8-direction neighbor bitmask for auto-tiling.

```fides
pub fn tilemap_auto_tile(tm: Tilemap, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 238*

---

## tilemap_auto_tile_4dir

Get a simplified 4-direction mask (N/S/E/W only) for basic auto-tiling.

```fides
pub fn tilemap_auto_tile_4dir(tm: Tilemap, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 265*

---

## tilemap_fill_rect

Fill a rectangular region with a tile ID.

```fides
pub fn tilemap_fill_rect(tm: &mut Tilemap, x0: i64, y0: i64, x1: i64, y1: i64, tile_id: i64) -> void {
```

**Parameters:**

- `tm`: `&mut Tilemap`
- `x0`: `i64`
- `y0`: `i64`
- `x1`: `i64`
- `y1`: `i64`
- `tile_id`: `i64`

**Returns:** `void`

*Defined at line 282*

---

## tilemap_fill_all

Fill the entire map with one tile.

```fides
pub fn tilemap_fill_all(tm: &mut Tilemap, tile_id: i64) -> void {
```

**Parameters:**

- `tm`: `&mut Tilemap`
- `tile_id`: `i64`

**Returns:** `void`

*Defined at line 295*

---

## tilemap_fill_border

Fill the border (edges) of the map with a tile ID.

```fides
pub fn tilemap_fill_border(tm: &mut Tilemap, tile_id: i64) -> void {
```

**Parameters:**

- `tm`: `&mut Tilemap`
- `tile_id`: `i64`

**Returns:** `void`

*Defined at line 300*

---

## tilemap_count_tile

Count tiles of a specific type.

```fides
pub fn tilemap_count_tile(tm: Tilemap, tile_id: i64) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`
- `tile_id`: `i64`

**Returns:** `i64`

*Defined at line 316*

---

## tilemap_count_solid

Count all solid tiles.

```fides
pub fn tilemap_count_solid(tm: Tilemap) -> i64 {
```

**Parameters:**

- `tm`: `Tilemap`

**Returns:** `i64`

*Defined at line 329*

---

## tilemap_line_of_sight

Check if there's a clear line of sight between two tiles (no solid tiles in between).
Uses Bresenham's line algorithm.

```fides
pub fn tilemap_line_of_sight(tm: Tilemap, x0: i64, y0: i64, x1: i64, y1: i64) -> bool {
```

**Parameters:**

- `tm`: `Tilemap`
- `x0`: `i64`
- `y0`: `i64`
- `x1`: `i64`
- `y1`: `i64`

**Returns:** `bool`

*Defined at line 345*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
