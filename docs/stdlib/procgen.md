[All modules](index.md)

# procgen -- Fides Documentation

## Contents

- [PgRng](#PgRng) (struct)
- [pg_rng_u32](#pg_rng_u32) (function)
- [pg_rng_seeded](#pg_rng_seeded) (function)
- [pg_rng_new](#pg_rng_new) (function)
- [pg_rng_below](#pg_rng_below) (function)
- [pg_rng_range](#pg_rng_range) (function)
- [pg_rng_f64](#pg_rng_f64) (function)
- [pg_rng_chance](#pg_rng_chance) (function)
- [PG_WALL:](#PG_WALL:) (const)
- [PG_FLOOR:](#PG_FLOOR:) (const)
- [PG_CORRIDOR:](#PG_CORRIDOR:) (const)
- [PgGrid](#PgGrid) (struct)
- [pg_grid_new](#pg_grid_new) (function)
- [pg_get](#pg_get) (function)
- [pg_set](#pg_set) (function)
- [pg_walkable](#pg_walkable) (function)
- [pg_count_walkable](#pg_count_walkable) (function)
- [pg_border_is_wall](#pg_border_is_wall) (function)
- [pg_grid_regions](#pg_grid_regions) (function)
- [pg_region_count](#pg_region_count) (function)
- [pg_grid_connected](#pg_grid_connected) (function)
- [pg_keep_largest_region](#pg_keep_largest_region) (function)
- [pg_grid_to_str](#pg_grid_to_str) (function)
- [pg_grid_hash](#pg_grid_hash) (function)
- [PgRoom](#PgRoom) (struct)
- [PgDungeon](#PgDungeon) (struct)
- [pg_room_center_x](#pg_room_center_x) (function)
- [pg_room_center_y](#pg_room_center_y) (function)
- [pg_rooms_touch](#pg_rooms_touch) (function)
- [pg_corridor](#pg_corridor) (function)
- [pg_bsp_dungeon](#pg_bsp_dungeon) (function)
- [pg_drunkard](#pg_drunkard) (function)
- [pg_cave_step](#pg_cave_step) (function)
- [pg_cave](#pg_cave) (function)
- [LSystem](#LSystem) (struct)
- [ls_new](#ls_new) (function)
- [ls_rule](#ls_rule) (function)
- [ls_expand](#ls_expand) (function)
- [LsTurtle](#LsTurtle) (struct)
- [ls_turtle](#ls_turtle) (function)
- [pg_poisson_disk](#pg_poisson_disk) (function)

## PgRng

```fides
pub struct PgRng {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `inc` | `i64` |

*Defined at line 42*

---

## pg_rng_u32

```fides
pub fn pg_rng_u32(r: &mut PgRng) -> i64 {
```

**Parameters:**

- `r`: `&mut PgRng`

**Returns:** `i64`

*Defined at line 57*

---

## pg_rng_seeded

```fides
pub fn pg_rng_seeded(initstate: i64, initseq: i64) -> PgRng {
```

**Parameters:**

- `initstate`: `i64`
- `initseq`: `i64`

**Returns:** `PgRng`

*Defined at line 67*

---

## pg_rng_new

A generator from one seed (stream 54, as pcg32's demo).

```fides
pub fn pg_rng_new(seed: i64) -> PgRng {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `PgRng`

*Defined at line 78*

---

## pg_rng_below

Uniform in 0 .. n-1 (n >= 1), without modulo bias.

```fides
pub fn pg_rng_below(r: &mut PgRng, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut PgRng`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 83*

---

## pg_rng_range

Uniform in lo ..= hi.

```fides
pub fn pg_rng_range(r: &mut PgRng, lo: i64, hi: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut PgRng`
- `lo`: `i64`
- `hi`: `i64`

**Returns:** `i64`

*Defined at line 98*

---

## pg_rng_f64

Uniform in [0, 1) with 53 random bits.

```fides
pub fn pg_rng_f64(r: &mut PgRng) -> f64 {
```

**Parameters:**

- `r`: `&mut PgRng`

**Returns:** `f64`

*Defined at line 106*

---

## pg_rng_chance

True with probability pct/100.

```fides
pub fn pg_rng_chance(r: &mut PgRng, pct: i64) -> bool {
```

**Parameters:**

- `r`: `&mut PgRng`
- `pct`: `i64`

**Returns:** `bool`

*Defined at line 113*

---

## PG_WALL:

```fides
pub const PG_WALL: i64 = 0;
```

*Defined at line 119*

---

## PG_FLOOR:

```fides
pub const PG_FLOOR: i64 = 1;
```

*Defined at line 120*

---

## PG_CORRIDOR:

```fides
pub const PG_CORRIDOR: i64 = 2;
```

*Defined at line 121*

---

## PgGrid

```fides
pub struct PgGrid {
```

**Fields:**

| Name | Type |
|------|------|
| `w` | `i64` |
| `h` | `i64` |
| `cells` | `Vec<i64>` |

*Defined at line 123*

---

## pg_grid_new

```fides
pub fn pg_grid_new(w: i64, h: i64, fill: i64) -> PgGrid {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `fill`: `i64`

**Returns:** `PgGrid`

*Defined at line 129*

---

## pg_get

```fides
pub fn pg_get(g: &PgGrid, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `g`: `&PgGrid`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 135*

---

## pg_set

```fides
pub fn pg_set(g: &mut PgGrid, x: i64, y: i64, v: i64) {
```

**Parameters:**

- `g`: `&mut PgGrid`
- `x`: `i64`
- `y`: `i64`
- `v`: `i64`

*Defined at line 142*

---

## pg_walkable

```fides
pub fn pg_walkable(g: &PgGrid, x: i64, y: i64) -> bool {
```

**Parameters:**

- `g`: `&PgGrid`
- `x`: `i64`
- `y`: `i64`

**Returns:** `bool`

*Defined at line 149*

---

## pg_count_walkable

```fides
pub fn pg_count_walkable(g: &PgGrid) -> i64 {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `i64`

*Defined at line 153*

---

## pg_border_is_wall

Is every border tile a wall?

```fides
pub fn pg_border_is_wall(g: &PgGrid) -> bool {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `bool`

*Defined at line 166*

---

## pg_grid_regions

Label 4-connected walkable regions: label per tile (-1 for walls);
the region count is the largest label + 1.

```fides
pub fn pg_grid_regions(g: &PgGrid) -> Vec<i64> {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `Vec<i64>`

*Defined at line 186*

---

## pg_region_count

```fides
pub fn pg_region_count(g: &PgGrid) -> i64 {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `i64`

*Defined at line 232*

---

## pg_grid_connected

One connected walkable region (or none)?

```fides
pub fn pg_grid_connected(g: &PgGrid) -> bool {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `bool`

*Defined at line 246*

---

## pg_keep_largest_region

Fill every region but the largest with wall.

```fides
pub fn pg_keep_largest_region(g: &mut PgGrid) {
```

**Parameters:**

- `g`: `&mut PgGrid`

*Defined at line 251*

---

## pg_grid_to_str

The grid as text: '#' wall, '.' floor, ',' corridor; one line per row.

```fides
pub fn pg_grid_to_str(g: &PgGrid) -> str {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `str`

*Defined at line 285*

---

## pg_grid_hash

```fides
pub fn pg_grid_hash(g: &PgGrid) -> i64 {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `i64`

*Defined at line 309*

---

## PgRoom

```fides
pub struct PgRoom {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `i64` |
| `y` | `i64` |
| `w` | `i64` |
| `h` | `i64` |

*Defined at line 321*

---

## PgDungeon

```fides
pub struct PgDungeon {
```

**Fields:**

| Name | Type |
|------|------|
| `grid` | `PgGrid` |
| `rooms` | `Vec<PgRoom>` |

*Defined at line 328*

---

## pg_room_center_x

```fides
pub fn pg_room_center_x(r: PgRoom) -> i64 {
```

**Parameters:**

- `r`: `PgRoom`

**Returns:** `i64`

*Defined at line 333*

---

## pg_room_center_y

```fides
pub fn pg_room_center_y(r: PgRoom) -> i64 {
```

**Parameters:**

- `r`: `PgRoom`

**Returns:** `i64`

*Defined at line 337*

---

## pg_rooms_touch

Do two rooms overlap or touch (share an edge or corner)?

```fides
pub fn pg_rooms_touch(a: PgRoom, b: PgRoom) -> bool {
```

**Parameters:**

- `a`: `PgRoom`
- `b`: `PgRoom`

**Returns:** `bool`

*Defined at line 342*

---

## pg_corridor

An L-shaped corridor from (x0, y0) to (x1, y1); horizontal first when
`h_first`.

```fides
pub fn pg_corridor(g: &mut PgGrid, x0: i64, y0: i64, x1: i64, y1: i64, h_first: bool) {
```

**Parameters:**

- `g`: `&mut PgGrid`
- `x0`: `i64`
- `y0`: `i64`
- `x1`: `i64`
- `y1`: `i64`
- `h_first`: `bool`

*Defined at line 357*

---

## pg_bsp_dungeon

A dungeon of rooms and corridors on a w x h map. Leaves are at least
`min_leaf` tiles on a side (min_leaf >= 5); rooms are at least 3x3.

```fides
pub fn pg_bsp_dungeon(w: i64, h: i64, min_leaf: i64, seed: i64) -> PgDungeon {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `min_leaf`: `i64`
- `seed`: `i64`

**Returns:** `PgDungeon`

*Defined at line 432*

---

## pg_drunkard

Carve from the centre with random steps until `pct` percent of the
inner tiles are floor.

```fides
pub fn pg_drunkard(w: i64, h: i64, pct: i64, seed: i64) -> PgGrid {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `pct`: `i64`
- `seed`: `i64`

**Returns:** `PgGrid`

*Defined at line 548*

---

## pg_cave_step

One automaton step: wall where 5+ of the 9 tiles around are walls.

```fides
pub fn pg_cave_step(g: &PgGrid) -> PgGrid {
```

**Parameters:**

- `g`: `&PgGrid`

**Returns:** `PgGrid`

*Defined at line 605*

---

## pg_cave

A cave: `wall_pct` percent random walls, `steps` automaton steps, then
only the largest connected cave kept.

```fides
pub fn pg_cave(w: i64, h: i64, wall_pct: i64, steps: i64, seed: i64) -> PgGrid {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `wall_pct`: `i64`
- `steps`: `i64`
- `seed`: `i64`

**Returns:** `PgGrid`

*Defined at line 623*

---

## LSystem

```fides
pub struct LSystem {
```

**Fields:**

| Name | Type |
|------|------|
| `axiom` | `str` |
| `symbols` | `Vec<i64>` |
| `replacements` | `Vec<str>` |
| `max_len` | `i64` |

*Defined at line 648*

---

## ls_new

```fides
pub fn ls_new(axiom: str) -> LSystem {
```

**Parameters:**

- `axiom`: `str`

**Returns:** `LSystem`

*Defined at line 656*

---

## ls_rule

Rule: every `sym` (one character) becomes `repl`.

```fides
pub fn ls_rule(l: &mut LSystem, sym: str, repl: str) {
```

**Parameters:**

- `l`: `&mut LSystem`
- `sym`: `str`
- `repl`: `str`

*Defined at line 663*

---

## ls_expand

The string after n rewriting steps (symbols without a rule stay).

```fides
pub fn ls_expand(l: &LSystem, n: i64) -> str {
```

**Parameters:**

- `l`: `&LSystem`
- `n`: `i64`

**Returns:** `str`

*Defined at line 678*

---

## LsTurtle

Turtle drawing: segments as x0 y0 x1 y1, the final pose, the bounds.

```fides
pub struct LsTurtle {
```

**Fields:**

| Name | Type |
|------|------|
| `segments` | `Vec<f64>` |
| `x` | `f64` |
| `y` | `f64` |
| `heading` | `f64` |
| `min_x` | `f64` |
| `min_y` | `f64` |
| `max_x` | `f64` |
| `max_y` | `f64` |
| `max_depth` | `i64` |

*Defined at line 714*

---

## ls_turtle

Interpret s from (0,0) heading +x: F and G draw `step` forward, f moves,
+ turns left by `angle_deg`, - right, | half a turn, [ ] save/restore.

```fides
pub fn ls_turtle(s: str, step: f64, angle_deg: f64) -> LsTurtle {
```

**Parameters:**

- `s`: `str`
- `step`: `f64`
- `angle_deg`: `f64`

**Returns:** `LsTurtle`

*Defined at line 744*

---

## pg_poisson_disk

Points (x, y pairs) in [0,w) x [0,h), pairwise at least r apart, filled
to saturation (Bridson). k = tries per active point (30 is usual).

```fides
pub fn pg_poisson_disk(w: f64, h: f64, r: f64, k: i64, seed: i64) -> Vec<f64> {
```

**Parameters:**

- `w`: `f64`
- `h`: `f64`
- `r`: `f64`
- `k`: `i64`
- `seed`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 798*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
