# scene — Fides Documentation

## Contents

- [EntityTemplate](#entitytemplate) (struct)
- [Entity](#entity) (struct)
- [SceneManager](#scenemanager) (struct)
- [scene_manager_create](#scene_manager_create) (function)
- [scene_register_template](#scene_register_template) (function)
- [scene_register_template_flags](#scene_register_template_flags) (function)
- [scene_get_template](#scene_get_template) (function)
- [scene_find_template](#scene_find_template) (function)
- [scene_spawn](#scene_spawn) (function)
- [scene_spawn_by_name](#scene_spawn_by_name) (function)
- [scene_despawn](#scene_despawn) (function)
- [scene_get_entity](#scene_get_entity) (function)
- [scene_find_entity](#scene_find_entity) (function)
- [scene_entity_count](#scene_entity_count) (function)
- [scene_count_by_template](#scene_count_by_template) (function)
- [scene_count_by_tag](#scene_count_by_tag) (function)
- [scene_update_entity](#scene_update_entity) (function)
- [scene_set_velocity](#scene_set_velocity) (function)
- [scene_damage](#scene_damage) (function)
- [scene_set_tag](#scene_set_tag) (function)
- [scene_query_radius](#scene_query_radius) (function)
- [scene_query_box](#scene_query_box) (function)
- [scene_nearest](#scene_nearest) (function)
- [scene_clear](#scene_clear) (function)
- [scene_clear_all](#scene_clear_all) (function)

## EntityTemplate

```fides
pub struct EntityTemplate {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str,         // template name (e.g., "player", "enemy")` |
| `max_health` | `i64,   // maximum health` |
| `width` | `i64,        // collision box width (pixels)` |
| `height` | `i64,       // collision box height (pixels)` |
| `flags` | `i64,        // bitmask: bit 0 = solid, bit 1 = hostile, bit 2 = static` |

*Defined at line 21*

---

## Entity

```fides
pub struct Entity {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64,           // unique entity ID` |
| `template_id` | `i64,  // index into templates list` |
| `x` | `f64,            // world position x` |
| `y` | `f64,            // world position y` |
| `vx` | `f64,           // velocity x` |
| `vy` | `f64,           // velocity y` |
| `health` | `i64,       // current health` |
| `active` | `bool,      // is this entity alive/active?` |
| `tag` | `i64,          // user-defined tag (for grouping/filtering)` |

*Defined at line 31*

---

## SceneManager

```fides
pub struct SceneManager {
```

**Fields:**

| Name | Type |
|------|------|
| `templates` | `Vec<EntityTemplate>` |
| `entities` | `Vec<Entity>` |
| `next_id` | `i64,      // next entity ID to assign` |
| `next_tag` | `i64,     // next tag to assign` |

*Defined at line 45*

---

## scene_manager_create

Create a new empty scene manager.

```fides
pub fn scene_manager_create() -> SceneManager {
```

**Returns:** `SceneManager`

*Defined at line 55*

---

## scene_register_template

Register a new entity template. Returns the template ID (index).

```fides
pub fn scene_register_template(sm: &mut SceneManager, name: str, max_health: i64, width: i64, height: i64) -> i64 {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `name`: `str`
- `max_health`: `i64`
- `width`: `i64`
- `height`: `i64`

**Returns:** `i64`

*Defined at line 67*

---

## scene_register_template_flags

Register a template with flags. Returns the template ID.

```fides
pub fn scene_register_template_flags(sm: &mut SceneManager, name: str, max_health: i64, width: i64, height: i64, flags: i64) -> i64 {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `name`: `str`
- `max_health`: `i64`
- `width`: `i64`
- `height`: `i64`
- `flags`: `i64`

**Returns:** `i64`

*Defined at line 80*

---

## scene_get_template

Get a template by ID.

```fides
pub fn scene_get_template(sm: SceneManager, template_id: i64) -> EntityTemplate {
```

**Parameters:**

- `sm`: `SceneManager`
- `template_id`: `i64`

**Returns:** `EntityTemplate`

*Defined at line 93*

---

## scene_find_template

Find a template by name. Returns -1 if not found.

```fides
pub fn scene_find_template(sm: SceneManager, name: str) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`
- `name`: `str`

**Returns:** `i64`

*Defined at line 107*

---

## scene_spawn

Spawn an entity from a template at a world position. Returns the entity ID.

```fides
pub fn scene_spawn(sm: &mut SceneManager, template_id: i64, x: f64, y: f64) -> i64 {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `template_id`: `i64`
- `x`: `f64`
- `y`: `f64`

**Returns:** `i64`

*Defined at line 122*

---

## scene_spawn_by_name

Spawn an entity by template name. Returns the entity ID, or -1 if not found.

```fides
pub fn scene_spawn_by_name(sm: &mut SceneManager, name: str, x: f64, y: f64) -> i64 {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `name`: `str`
- `x`: `f64`
- `y`: `f64`

**Returns:** `i64`

*Defined at line 147*

---

## scene_despawn

Despawn (remove) an entity by ID. Returns true if found and removed.

```fides
pub fn scene_despawn(sm: &mut SceneManager, entity_id: i64) -> bool {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `entity_id`: `i64`

**Returns:** `bool`

*Defined at line 154*

---

## scene_get_entity

Find an entity by ID. Returns the entity, or a default if not found.

```fides
pub fn scene_get_entity(sm: SceneManager, entity_id: i64) -> Entity {
```

**Parameters:**

- `sm`: `SceneManager`
- `entity_id`: `i64`

**Returns:** `Entity`

*Defined at line 172*

---

## scene_find_entity

Find the index of an entity by ID. Returns -1 if not found.

```fides
pub fn scene_find_entity(sm: SceneManager, entity_id: i64) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`
- `entity_id`: `i64`

**Returns:** `i64`

*Defined at line 195*

---

## scene_entity_count

Count active entities.

```fides
pub fn scene_entity_count(sm: SceneManager) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`

**Returns:** `i64`

*Defined at line 208*

---

## scene_count_by_template

Count entities with a specific template.

```fides
pub fn scene_count_by_template(sm: SceneManager, template_id: i64) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`
- `template_id`: `i64`

**Returns:** `i64`

*Defined at line 222*

---

## scene_count_by_tag

Count entities with a specific tag.

```fides
pub fn scene_count_by_tag(sm: SceneManager, tag: i64) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`
- `tag`: `i64`

**Returns:** `i64`

*Defined at line 236*

---

## scene_update_entity

Update an entity's position by its velocity (simple Euler integration).

```fides
pub fn scene_update_entity(sm: &mut SceneManager, entity_id: i64, dt: f64) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `entity_id`: `i64`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 252*

---

## scene_set_velocity

Set an entity's velocity.

```fides
pub fn scene_set_velocity(sm: &mut SceneManager, entity_id: i64, vx: f64, vy: f64) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `entity_id`: `i64`
- `vx`: `f64`
- `vy`: `f64`

**Returns:** `void`

*Defined at line 260*

---

## scene_damage

Apply damage to an entity. Returns true if the entity died.

```fides
pub fn scene_damage(sm: &mut SceneManager, entity_id: i64, amount: i64) -> bool {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `entity_id`: `i64`
- `amount`: `i64`

**Returns:** `bool`

*Defined at line 267*

---

## scene_set_tag

Set an entity's tag (for grouping/filtering).

```fides
pub fn scene_set_tag(sm: &mut SceneManager, entity_id: i64, tag: i64) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`
- `entity_id`: `i64`
- `tag`: `i64`

**Returns:** `void`

*Defined at line 275*

---

## scene_query_radius

Find all entities within a radius of a point. Returns a Vec of entity IDs.

```fides
pub fn scene_query_radius(sm: SceneManager, x: f64, y: f64, radius: f64) -> Vec<i64> {
```

**Parameters:**

- `sm`: `SceneManager`
- `x`: `f64`
- `y`: `f64`
- `radius`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 284*

---

## scene_query_box

Find all entities within an axis-aligned bounding box.

```fides
pub fn scene_query_box(sm: SceneManager, x0: f64, y0: f64, x1: f64, y1: f64) -> Vec<i64> {
```

**Parameters:**

- `sm`: `SceneManager`
- `x0`: `f64`
- `y0`: `f64`
- `x1`: `f64`
- `y1`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 303*

---

## scene_nearest

Find the nearest entity to a point. Returns entity ID or -1 if none.

```fides
pub fn scene_nearest(sm: SceneManager, x: f64, y: f64) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`
- `x`: `f64`
- `y`: `f64`

**Returns:** `i64`

*Defined at line 317*

---

## scene_clear

Clear all entities (but keep templates).

```fides
pub fn scene_clear(sm: &mut SceneManager) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`

**Returns:** `void`

*Defined at line 340*

---

## scene_clear_all

Clear all entities and templates.

```fides
pub fn scene_clear_all(sm: &mut SceneManager) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`

**Returns:** `void`

*Defined at line 345*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
