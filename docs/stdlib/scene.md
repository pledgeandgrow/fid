[All modules](index.md)

# scene -- Fides Documentation

## Contents

- [EntityTemplate](#EntityTemplate) (struct)
- [Entity](#Entity) (struct)
- [SceneManager](#SceneManager) (struct)
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
| `name` | `str` |
| `max_health` | `i64` |
| `width` | `i64` |
| `height` | `i64` |
| `flags` | `i64,  // bitmask: bit 0 = solid, bit 1 = hostile, bit 2 = static` |

*Defined at line 20*

---

## Entity

```fides
pub struct Entity {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `template_id` | `i64` |
| `x` | `f64` |
| `y` | `f64` |
| `vx` | `f64` |
| `vy` | `f64` |
| `health` | `i64` |
| `active` | `bool` |
| `tag` | `i64,  // user-defined tag (for grouping/filtering)` |

*Defined at line 34*

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
| `next_id` | `i64` |
| `next_tag` | `i64,  // next tag to assign` |

*Defined at line 56*

---

## scene_manager_create

Create a new empty scene manager.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var sm = scene_manager_create();
val orc = scene_register_template(&mut sm, "orc", 30, 16, 16);
assert_eq(scene_find_template(sm, "orc"), orc);
assert_eq(scene_find_template(sm, "elf"), -1);
val e = scene_spawn_by_name(&mut sm, "orc", 1.0, 2.0);
assert(e > 0);
assert_eq(scene_get_entity(sm, e).health, 30);
assert(scene_despawn(&mut sm, e));
assert_eq(scene_spawn(&mut sm, 7, 0.0, 0.0), -1);
```

```fides
pub fn scene_manager_create() -> SceneManager {
```

**Returns:** `SceneManager`

*Defined at line 78*

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

*Defined at line 87*

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

*Defined at line 96*

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

*Defined at line 105*

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

*Defined at line 115*

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

*Defined at line 130*

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

*Defined at line 156*

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

*Defined at line 174*

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

*Defined at line 189*

---

## scene_entity_count

Count active entities.

```fides
pub fn scene_entity_count(sm: SceneManager) -> i64 {
```

**Parameters:**

- `sm`: `SceneManager`

**Returns:** `i64`

*Defined at line 202*

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

*Defined at line 216*

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

*Defined at line 230*

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

*Defined at line 246*

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

*Defined at line 258*

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

*Defined at line 270*

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

*Defined at line 291*

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

*Defined at line 304*

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

*Defined at line 323*

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

*Defined at line 337*

---

## scene_clear

Clear all entities (but keep templates).

```fides
pub fn scene_clear(sm: &mut SceneManager) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`

**Returns:** `void`

*Defined at line 360*

---

## scene_clear_all

Clear all entities and templates.

```fides
pub fn scene_clear_all(sm: &mut SceneManager) -> void {
```

**Parameters:**

- `sm`: `&mut SceneManager`

**Returns:** `void`

*Defined at line 365*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
