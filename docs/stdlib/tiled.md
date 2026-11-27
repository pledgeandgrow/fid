[All modules](index.md)

# tiled -- Fides Documentation

## Contents

- [TILED_FLIP_H:](#TILED_FLIP_H:) (const)
- [TILED_FLIP_V:](#TILED_FLIP_V:) (const)
- [TILED_FLIP_D:](#TILED_FLIP_D:) (const)
- [TiledMap](#TiledMap) (struct)
- [TiledObj](#TiledObj) (struct)
- [tiled_open](#tiled_open) (function)
- [tiled_gid](#tiled_gid) (function)
- [tiled_hflip](#tiled_hflip) (function)
- [tiled_vflip](#tiled_vflip) (function)
- [tiled_dflip](#tiled_dflip) (function)
- [tiled_layer_count](#tiled_layer_count) (function)
- [tiled_layer_name](#tiled_layer_name) (function)
- [tiled_layer_type](#tiled_layer_type) (function)
- [tiled_layer_visible](#tiled_layer_visible) (function)
- [tiled_tile_raw](#tiled_tile_raw) (function)
- [tiled_tile](#tiled_tile) (function)
- [tiled_to_tilemap](#tiled_to_tilemap) (function)
- [tiled_object_count](#tiled_object_count) (function)
- [tiled_object](#tiled_object) (function)
- [tiled_obj_prop](#tiled_obj_prop) (function)
- [tiled_map_prop](#tiled_map_prop) (function)

## TILED_FLIP_H:

```fides
pub const TILED_FLIP_H: i64 = 0x80000000;
```

*Defined at line 17*

---

## TILED_FLIP_V:

```fides
pub const TILED_FLIP_V: i64 = 0x40000000;
```

*Defined at line 18*

---

## TILED_FLIP_D:

```fides
pub const TILED_FLIP_D: i64 = 0x20000000;
```

*Defined at line 19*

---

## TiledMap

A parsed TMJ document: `json` is its text and `doc` its index (json.fd
json_index: scanned once; every lookup below walks the index instead of
rescanning the text, and tiled_to_tilemap reads each data array once).

```fides
pub struct TiledMap {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `json` | `str` |
| `doc` | `JsonDoc` |
| `width` | `i64` |
| `height` | `i64` |
| `tile_w` | `i64` |
| `tile_h` | `i64` |

*Defined at line 24*

---

## TiledObj

A Tiled object from an objectgroup layer.

```fides
pub struct TiledObj {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `kind` | `str` |
| `x` | `f64` |
| `y` | `f64` |
| `w` | `f64` |
| `h` | `f64` |
| `gid` | `i64` |
| `point` | `bool` |

*Defined at line 37*

---

## tiled_open

Parse a TMJ string. ok=false when the required top-level fields are
absent or the map isn't finite.


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
var m = tiled_open("{\"width\":2,\"height\":1,\"tilewidth\":16,\"tileheight\":16,\"layers\":[{\"type\":\"tilelayer\",\"data\":[1,2]}]}");
println(int_to_str(tiled_tile(m, 0, 1, 0)));   // 2
```

```fides
pub fn tiled_open(j: str) -> TiledMap {
```

**Parameters:**

- `j`: `str`

**Returns:** `TiledMap`

*Defined at line 58*

---

## tiled_gid

Strip the flip flags from a raw gid.

```fides
pub fn tiled_gid(raw: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## tiled_hflip

```fides
pub fn tiled_hflip(raw: i64) -> bool {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `bool`

*Defined at line 81*

---

## tiled_vflip

```fides
pub fn tiled_vflip(raw: i64) -> bool {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `bool`

*Defined at line 84*

---

## tiled_dflip

```fides
pub fn tiled_dflip(raw: i64) -> bool {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `bool`

*Defined at line 87*

---

## tiled_layer_count

```fides
pub fn tiled_layer_count(m: TiledMap) -> i64 {
```

**Parameters:**

- `m`: `TiledMap`

**Returns:** `i64`

*Defined at line 96*

---

## tiled_layer_name

Layer name/type/visibility/opacity for UI decisions.

```fides
pub fn tiled_layer_name(m: TiledMap, li: i64) -> str {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`

**Returns:** `str`

*Defined at line 106*

---

## tiled_layer_type

```fides
pub fn tiled_layer_type(m: TiledMap, li: i64) -> str {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`

**Returns:** `str`

*Defined at line 109*

---

## tiled_layer_visible

```fides
pub fn tiled_layer_visible(m: TiledMap, li: i64) -> bool {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`

**Returns:** `bool`

*Defined at line 112*

---

## tiled_tile_raw

Raw gid at (x,y) of tile layer `li` — flip bits included, 0 outside
bounds or for non-tile layers. Tiled stores data row-major. One call
walks the layer's data array up to the cell (no text is scanned); to
read every cell, tiled_to_tilemap reads each array once.

```fides
pub fn tiled_tile_raw(m: TiledMap, li: i64, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 120*

---

## tiled_tile

gid with flip flags stripped.

```fides
pub fn tiled_tile(m: TiledMap, li: i64, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 135*

---

## tiled_to_tilemap

Feed a tile layer into tilemap.fd — tiles > 0 become set cells.
`solid_layer`: when >= 0, cells occupied in that layer mark the tile
solid in the returned Tilemap. Both data arrays are fetched once;
per-cell work is a single array index.

```fides
pub fn tiled_to_tilemap(m: TiledMap, li: i64, solid_layer: i64) -> Tilemap {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`
- `solid_layer`: `i64`

**Returns:** `Tilemap`

*Defined at line 143*

---

## tiled_object_count

```fides
pub fn tiled_object_count(m: TiledMap, li: i64) -> i64 {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`

**Returns:** `i64`

*Defined at line 175*

---

## tiled_object

Object `oi` of objectgroup layer `li`; id=0 when out of range.

```fides
pub fn tiled_object(m: TiledMap, li: i64, oi: i64) -> TiledObj {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`
- `oi`: `i64`

**Returns:** `TiledObj`

*Defined at line 192*

---

## tiled_obj_prop

First custom property `name` on an object as str ("" when absent).
Properties are an array of {name, type, value} objects.

```fides
pub fn tiled_obj_prop(m: TiledMap, li: i64, oi: i64, name: str) -> str {
```

**Parameters:**

- `m`: `TiledMap`
- `li`: `i64`
- `oi`: `i64`
- `name`: `str`

**Returns:** `str`

*Defined at line 236*

---

## tiled_map_prop

Map-level custom property (same {name,type,value} array).

```fides
pub fn tiled_map_prop(m: TiledMap, name: str) -> str {
```

**Parameters:**

- `m`: `TiledMap`
- `name`: `str`

**Returns:** `str`

*Defined at line 241*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
