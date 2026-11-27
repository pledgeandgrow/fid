[All modules](index.md)

# editor -- Fides Documentation

## Contents

- [EditorEntity](#EditorEntity) (struct)
- [EditorState](#EditorState) (struct)
- [GIZMO_TRANSLATE:](#GIZMO_TRANSLATE:) (const)
- [GIZMO_ROTATE:](#GIZMO_ROTATE:) (const)
- [GIZMO_SCALE:](#GIZMO_SCALE:) (const)
- [editor_create](#editor_create) (function)
- [editor_init](#editor_init) (function)
- [editor_add_entity](#editor_add_entity) (function)
- [editor_select](#editor_select) (function)
- [editor_deselect_all](#editor_deselect_all) (function)
- [editor_get_selected](#editor_get_selected) (function)
- [editor_move_selected](#editor_move_selected) (function)
- [editor_rotate_selected](#editor_rotate_selected) (function)
- [editor_scale_selected](#editor_scale_selected) (function)
- [editor_set_gizmo_mode](#editor_set_gizmo_mode) (function)
- [editor_toggle_snap](#editor_toggle_snap) (function)
- [editor_set_snap_translate](#editor_set_snap_translate) (function)
- [editor_set_snap_rotate](#editor_set_snap_rotate) (function)
- [editor_set_snap_scale](#editor_set_snap_scale) (function)
- [editor_set_grid_size](#editor_set_grid_size) (function)
- [editor_toggle_grid](#editor_toggle_grid) (function)
- [editor_set_camera](#editor_set_camera) (function)
- [editor_delete_selected](#editor_delete_selected) (function)
- [editor_duplicate_selected](#editor_duplicate_selected) (function)
- [editor_toggle_visibility](#editor_toggle_visibility) (function)
- [editor_toggle_lock](#editor_toggle_lock) (function)
- [editor_entity_count](#editor_entity_count) (function)
- [editor_get_entity](#editor_get_entity) (function)
- [editor_selected_id](#editor_selected_id) (function)
- [editor_gizmo_mode](#editor_gizmo_mode) (function)
- [editor_snap_enabled](#editor_snap_enabled) (function)
- [editor_grid_visible](#editor_grid_visible) (function)
- [editor_show_inspector](#editor_show_inspector) (function)
- [editor_show_hierarchy](#editor_show_hierarchy) (function)
- [editor_clear](#editor_clear) (function)
- [editor_set_max_undo](#editor_set_max_undo) (function)

## EditorEntity

```fides
pub struct EditorEntity {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `name` | `str` |
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |
| `rx` | `f64` |
| `ry` | `f64` |
| `rz` | `f64` |
| `sx` | `f64` |
| `sy` | `f64` |
| `sz` | `f64` |
| `visible` | `bool` |
| `locked` | `bool` |
| `selected` | `bool` |

*Defined at line 19*

---

## EditorState

```fides
pub struct EditorState {
```

**Fields:**

| Name | Type |
|------|------|
| `entities` | `Vec<EditorEntity>` |
| `selected_id` | `i64` |
| `gizmo_mode` | `i64` |
| `snap_enabled` | `bool` |
| `snap_translate` | `f64` |
| `snap_rotate` | `f64` |
| `snap_scale` | `f64` |
| `grid_visible` | `bool` |
| `grid_size` | `f64` |
| `camera_x` | `f64` |
| `camera_y` | `f64` |
| `camera_z` | `f64` |
| `show_inspector` | `bool` |
| `show_hierarchy` | `bool` |
| `show_gizmo` | `bool` |
| `max_undo` | `i64` |

*Defined at line 36*

---

## GIZMO_TRANSLATE:

```fides
pub const GIZMO_TRANSLATE: i64 = 0;
```

*Defined at line 55*

---

## GIZMO_ROTATE:

```fides
pub const GIZMO_ROTATE: i64 = 1;
```

*Defined at line 56*

---

## GIZMO_SCALE:

```fides
pub const GIZMO_SCALE: i64 = 2;
```

*Defined at line 57*

---

## editor_create

Create a new editor state.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var ed = editor_create();
editor_add_entity(&mut ed, 7, "crate", 1.0, 0.0, 0.0);
editor_add_entity(&mut ed, 8, "lamp", 0.0, 2.0, 0.0);
editor_select(&mut ed, 7);
assert_eq(editor_get_selected(&ed).name, "crate");
editor_toggle_snap(&mut ed);
assert(!ed.snap_enabled);
editor_move_selected(&mut ed, 1.5, 0.0, 0.0);
assert(editor_get_selected(&ed).x == 2.5);
editor_deselect_all(&mut ed);
assert_eq(ed.selected_id, -1);
```

```fides
pub fn editor_create() -> EditorState {
```

**Returns:** `EditorState`

*Defined at line 72*

---

## editor_init

Initialize the editor.

```fides
pub fn editor_init(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 80*

---

## editor_add_entity

Add an entity to the editor.

```fides
pub fn editor_add_entity(ed: &mut EditorState, id: i64, name: str, x: f64, y: f64, z: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `id`: `i64`
- `name`: `str`
- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `void`

*Defined at line 86*

---

## editor_select

Select an entity by ID.

```fides
pub fn editor_select(ed: &mut EditorState, id: i64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `id`: `i64`

**Returns:** `void`

*Defined at line 93*

---

## editor_deselect_all

Deselect all entities.

```fides
pub fn editor_deselect_all(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 105*

---

## editor_get_selected

Get the selected entity.

```fides
pub fn editor_get_selected(ed: &EditorState) -> EditorEntity {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `EditorEntity`

*Defined at line 132*

---

## editor_move_selected

Move the selected entity.

```fides
pub fn editor_move_selected(ed: &mut EditorState, dx: f64, dy: f64, dz: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`

**Returns:** `void`

*Defined at line 143*

---

## editor_rotate_selected

Rotate the selected entity.

```fides
pub fn editor_rotate_selected(ed: &mut EditorState, dx: f64, dy: f64, dz: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`

**Returns:** `void`

*Defined at line 170*

---

## editor_scale_selected

Scale the selected entity.

```fides
pub fn editor_scale_selected(ed: &mut EditorState, dx: f64, dy: f64, dz: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `dx`: `f64`
- `dy`: `f64`
- `dz`: `f64`

**Returns:** `void`

*Defined at line 197*

---

## editor_set_gizmo_mode

Set the gizmo mode.

```fides
pub fn editor_set_gizmo_mode(ed: &mut EditorState, mode: i64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `mode`: `i64`

**Returns:** `void`

*Defined at line 234*

---

## editor_toggle_snap

Toggle grid snapping.

```fides
pub fn editor_toggle_snap(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 239*

---

## editor_set_snap_translate

Set the snap translate value.

```fides
pub fn editor_set_snap_translate(ed: &mut EditorState, val: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `val`: `f64`

**Returns:** `void`

*Defined at line 244*

---

## editor_set_snap_rotate

Set the snap rotate value.

```fides
pub fn editor_set_snap_rotate(ed: &mut EditorState, val: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `val`: `f64`

**Returns:** `void`

*Defined at line 249*

---

## editor_set_snap_scale

Set the snap scale value.

```fides
pub fn editor_set_snap_scale(ed: &mut EditorState, val: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `val`: `f64`

**Returns:** `void`

*Defined at line 254*

---

## editor_set_grid_size

Set the grid size.

```fides
pub fn editor_set_grid_size(ed: &mut EditorState, size: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `size`: `f64`

**Returns:** `void`

*Defined at line 259*

---

## editor_toggle_grid

Toggle grid visibility.

```fides
pub fn editor_toggle_grid(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 264*

---

## editor_set_camera

Set the editor camera position.

```fides
pub fn editor_set_camera(ed: &mut EditorState, x: f64, y: f64, z: f64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `void`

*Defined at line 269*

---

## editor_delete_selected

Delete the selected entity.

```fides
pub fn editor_delete_selected(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 276*

---

## editor_duplicate_selected

Duplicate the selected entity.

```fides
pub fn editor_duplicate_selected(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 286*

---

## editor_toggle_visibility

Toggle entity visibility.

```fides
pub fn editor_toggle_visibility(ed: &mut EditorState, id: i64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `id`: `i64`

**Returns:** `void`

*Defined at line 299*

---

## editor_toggle_lock

Toggle entity lock.

```fides
pub fn editor_toggle_lock(ed: &mut EditorState, id: i64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `id`: `i64`

**Returns:** `void`

*Defined at line 313*

---

## editor_entity_count

Get the number of entities.

```fides
pub fn editor_entity_count(ed: &EditorState) -> i64 {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `i64`

*Defined at line 327*

---

## editor_get_entity

Get an entity by index.

```fides
pub fn editor_get_entity(ed: &EditorState, idx: i64) -> EditorEntity {
```

**Parameters:**

- `ed`: `&EditorState`
- `idx`: `i64`

**Returns:** `EditorEntity`

*Defined at line 332*

---

## editor_selected_id

Get the selected entity ID.

```fides
pub fn editor_selected_id(ed: &EditorState) -> i64 {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `i64`

*Defined at line 337*

---

## editor_gizmo_mode

Get the gizmo mode.

```fides
pub fn editor_gizmo_mode(ed: &EditorState) -> i64 {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `i64`

*Defined at line 342*

---

## editor_snap_enabled

Check if snapping is enabled.

```fides
pub fn editor_snap_enabled(ed: &EditorState) -> bool {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `bool`

*Defined at line 347*

---

## editor_grid_visible

Check if grid is visible.

```fides
pub fn editor_grid_visible(ed: &EditorState) -> bool {
```

**Parameters:**

- `ed`: `&EditorState`

**Returns:** `bool`

*Defined at line 352*

---

## editor_show_inspector

Set inspector visibility.

```fides
pub fn editor_show_inspector(ed: &mut EditorState, show: bool) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `show`: `bool`

**Returns:** `void`

*Defined at line 357*

---

## editor_show_hierarchy

Set hierarchy visibility.

```fides
pub fn editor_show_hierarchy(ed: &mut EditorState, show: bool) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `show`: `bool`

**Returns:** `void`

*Defined at line 362*

---

## editor_clear

Clear all entities.

```fides
pub fn editor_clear(ed: &mut EditorState) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`

**Returns:** `void`

*Defined at line 367*

---

## editor_set_max_undo

Set the maximum undo stack depth.

```fides
pub fn editor_set_max_undo(ed: &mut EditorState, max: i64) -> void {
```

**Parameters:**

- `ed`: `&mut EditorState`
- `max`: `i64`

**Returns:** `void`

*Defined at line 373*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
