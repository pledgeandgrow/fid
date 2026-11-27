[All modules](index.md)

# ecs -- Fides Documentation

## Contents

- [ecs_create_entity](#ecs_create_entity) (function)
- [ecs_entity_count](#ecs_entity_count) (function)
- [ecs_is_alive](#ecs_is_alive) (function)
- [ecs_has](#ecs_has) (function)
- [ecs_add](#ecs_add) (function)
- [ecs_get](#ecs_get) (function)
- [ecs_remove](#ecs_remove) (function)
- [ecs_dispatch_event](#ecs_dispatch_event) (function)
- [ecs_register_component](#ecs_register_component) (function)
- [ecs_register_system](#ecs_register_system) (function)
- [ecs_run_systems](#ecs_run_systems) (function)
- [ecs_query_entities](#ecs_query_entities) (function)
- [ecs_count_with_component](#ecs_count_with_component) (function)
- [ecs_destroy_entity](#ecs_destroy_entity) (function)
- [ecs_reset](#ecs_reset) (function)
- [ecs_frame](#ecs_frame) (function)
- [ecs_spawn](#ecs_spawn) (function)
- [ecs_spawn2](#ecs_spawn2) (function)
- [ecs_spawn3](#ecs_spawn3) (function)

## ecs_create_entity

```fides
pub fn ecs_create_entity() -> i64 {
```

**Returns:** `i64`

*Defined at line 27*

---

## ecs_entity_count

```fides
pub fn ecs_entity_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 32*

---

## ecs_is_alive

True while the entity has been created and not destroyed.
Destroyed IDs are never reused, so this stays false forever after
ecs_destroy_entity.

```fides
pub fn ecs_is_alive(entity: i64) -> bool {
```

**Parameters:**

- `entity`: `i64`

**Returns:** `bool`

*Defined at line 73*

---

## ecs_has

```fides
pub fn ecs_has(component_name: str, entity: i64) -> i64 {
```

**Parameters:**

- `component_name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 80*

---

## ecs_add

```fides
pub fn ecs_add<T>(component_name: str, entity: i64, value: T) {
```

**Parameters:**

- `component_name`: `str`
- `entity`: `i64`
- `value`: `T`

*Defined at line 86*

---

## ecs_get

```fides
pub fn ecs_get(component_name: str, entity: i64) -> i64 {
```

**Parameters:**

- `component_name`: `str`
- `entity`: `i64`

**Returns:** `i64`

*Defined at line 93*

---

## ecs_remove

```fides
pub fn ecs_remove(component_name: str, entity: i64) {
```

**Parameters:**

- `component_name`: `str`
- `entity`: `i64`

*Defined at line 98*

---

## ecs_dispatch_event

```fides
pub fn ecs_dispatch_event(_event_name: str) {  // Event dispatch is handled by calling __on_<event_name> directly
```

**Parameters:**

- `_event_name`: `str`

*Defined at line 109*

---

## ecs_register_component

Register a component type. Called once per component type at startup.
In comptime mode, this would generate the storage layout.

```fides
pub fn ecs_register_component(_component_name: str) -> void {  // The runtime ECS auto-registers components on first use.
```

**Parameters:**

- `_component_name`: `str`

**Returns:** `void`

*Defined at line 126*

---

## ecs_register_system

Register a system. A system is a function that runs every frame
and queries entities with specific components.
In comptime mode, this would generate the query iteration code.

```fides
pub fn ecs_register_system(_system_name: str, _handler: fn() -> void) -> void {  // Store the system handler for later execution.
```

**Parameters:**

- `_system_name`: `str`
- `_handler`: `fn() -> void`

**Returns:** `void`

*Defined at line 133*

---

## ecs_run_systems

Run all registered systems. Called once per frame.

```fides
pub fn ecs_run_systems() -> void {  // In the runtime, systems are called directly by name.
```

**Returns:** `void`

*Defined at line 138*

---

## ecs_query_entities

Query entities that have all of the specified components.
Returns a Vec of entity IDs.
In comptime mode, this would generate specialized iteration code.

```fides
pub fn ecs_query_entities(components: str) -> Vec<i64> {
```

**Parameters:**

- `components`: `str`

**Returns:** `Vec<i64>`

*Defined at line 145*

---

## ecs_count_with_component

Get the number of entities that have a specific component.

```fides
pub fn ecs_count_with_component(component_name: str) -> i64 {
```

**Parameters:**

- `component_name`: `str`

**Returns:** `i64`

*Defined at line 170*

---

## ecs_destroy_entity

Destroy an entity and all its components.
Removes every registered component and marks the ID dead. The ID is
never reused and ecs_entity_count() does not decrease (it is the
high-water mark of IDs ever handed out).

```fides
pub fn ecs_destroy_entity(entity: i64) -> void {
```

**Parameters:**

- `entity`: `i64`

**Returns:** `void`

*Defined at line 187*

---

## ecs_reset

Reset the ECS world — destroy all entities and components.
Every registered component is removed from every entity. The entity
ID counter is owned by the runtime and keeps its high-water mark, so
post-reset entities get fresh IDs above all previous ones.

```fides
pub fn ecs_reset() -> void {
```

**Returns:** `void`

*Defined at line 203*

---

## ecs_frame

Run one frame of the ECS: call all systems, then clean up.
This is a convenience function for game loops.

```fides
pub fn ecs_frame() -> void {
```

**Returns:** `void`

*Defined at line 231*

---

## ecs_spawn

Create an entity with a single component (convenience function).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
component Health { hp: i64 }
fn main() -> i64 {
    val before = ecs_entity_count();
    val e = ecs_spawn("Health", Health { hp: 5 });
    assert_eq(ecs_entity_count(), before + 1);
    assert_eq(ecs_has("Nothing", e), 0);
    val e2 = ecs_spawn2("Health", Health { hp: 1 }, "Health", Health { hp: 2 });
    assert_eq(e2, e + 1);
    assert_eq(ecs_query_entities("[]").len(), ecs_entity_count());
    return 0;
}
```

```fides
pub fn ecs_spawn<T>(component_name: str, value: T) -> i64 {
```

**Parameters:**

- `component_name`: `str`
- `value`: `T`

**Returns:** `i64`

*Defined at line 250*

---

## ecs_spawn2

Create an entity with two components (convenience function).

```fides
pub fn ecs_spawn2<T, U>(comp1: str, val1: T, comp2: str, val2: U) -> i64 {
```

**Parameters:**

- `comp1`: `str`
- `val1`: `T`
- `comp2`: `str`
- `val2`: `U`

**Returns:** `i64`

*Defined at line 257*

---

## ecs_spawn3

Create an entity with three components (convenience function).

```fides
pub fn ecs_spawn3<T, U, V>(comp1: str, val1: T, comp2: str, val2: U, comp3: str, val3: V) -> i64 {
```

**Parameters:**

- `comp1`: `str`
- `val1`: `T`
- `comp2`: `str`
- `val2`: `U`
- `comp3`: `str`
- `val3`: `V`

**Returns:** `i64`

*Defined at line 265*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
