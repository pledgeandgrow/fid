[All modules](index.md)

# physics3d -- Fides Documentation

## Contents

- [COLLIDER_SPHERE:](#COLLIDER_SPHERE:) (const)
- [COLLIDER_BOX:](#COLLIDER_BOX:) (const)
- [COLLIDER_PLANE:](#COLLIDER_PLANE:) (const)
- [Sphere](#Sphere) (struct)
- [Aabb3](#Aabb3) (struct)
- [Plane3](#Plane3) (struct)
- [RigidBody3](#RigidBody3) (struct)
- [phys3d_create_body](#phys3d_create_body) (function)
- [phys3d_create_sphere](#phys3d_create_sphere) (function)
- [phys3d_create_static_plane](#phys3d_create_static_plane) (function)
- [phys3d_set_gravity](#phys3d_set_gravity) (function)
- [phys3d_get_gravity](#phys3d_get_gravity) (function)
- [phys3d_apply_force](#phys3d_apply_force) (function)
- [phys3d_apply_impulse](#phys3d_apply_impulse) (function)
- [phys3d_apply_gravity](#phys3d_apply_gravity) (function)
- [phys3d_step](#phys3d_step) (function)
- [Contact3](#Contact3) (struct)
- [phys3d_sphere_vs_sphere](#phys3d_sphere_vs_sphere) (function)
- [phys3d_aabb_vs_aabb](#phys3d_aabb_vs_aabb) (function)
- [phys3d_sphere_vs_plane](#phys3d_sphere_vs_plane) (function)
- [phys3d_sphere_vs_aabb](#phys3d_sphere_vs_aabb) (function)
- [phys3d_resolve_contact](#phys3d_resolve_contact) (function)
- [phys3d_check_collision](#phys3d_check_collision) (function)
- [phys3d_resolve_all](#phys3d_resolve_all) (function)

## COLLIDER_SPHERE:

```fides
pub const COLLIDER_SPHERE: i64 = 0;
```

*Defined at line 27*

---

## COLLIDER_BOX:

```fides
pub const COLLIDER_BOX: i64 = 1;
```

*Defined at line 28*

---

## COLLIDER_PLANE:

```fides
pub const COLLIDER_PLANE: i64 = 2;
```

*Defined at line 29*

---

## Sphere

```fides
pub struct Sphere {
```

**Fields:**

| Name | Type |
|------|------|
| `center` | `vec3` |
| `radius` | `f64` |

*Defined at line 30*

---

## Aabb3

```fides
pub struct Aabb3 {
```

**Fields:**

| Name | Type |
|------|------|
| `min` | `vec3` |
| `max` | `vec3` |

*Defined at line 35*

---

## Plane3

```fides
pub struct Plane3 {
```

**Fields:**

| Name | Type |
|------|------|
| `normal` | `vec3` |
| `distance` | `f64,  // distance from origin along normal` |

*Defined at line 40*

---

## RigidBody3

```fides
pub struct RigidBody3 {
```

**Fields:**

| Name | Type |
|------|------|
| `position` | `vec3` |
| `velocity` | `vec3` |
| `acceleration` | `vec3` |
| `force` | `vec3` |
| `mass` | `f64` |
| `inv_mass` | `f64` |
| `restitution` | `f64` |
| `friction` | `f64` |
| `is_static` | `bool` |
| `collider_type` | `i64` |
| `sphere` | `Sphere` |
| `box` | `Aabb3` |
| `radius` | `f64` |
| `half_extents` | `vec3,  // for box collider` |

*Defined at line 47*

---

## phys3d_create_body

```fides
pub fn phys3d_create_body(pos: vec3, half_ext: vec3, mass: f64) -> RigidBody3 {
```

**Parameters:**

- `pos`: `vec3`
- `half_ext`: `vec3`
- `mass`: `f64`

**Returns:** `RigidBody3`

*Defined at line 69*

---

## phys3d_create_sphere

```fides
pub fn phys3d_create_sphere(pos: vec3, radius: f64, mass: f64) -> RigidBody3 {
```

**Parameters:**

- `pos`: `vec3`
- `radius`: `f64`
- `mass`: `f64`

**Returns:** `RigidBody3`

*Defined at line 83*

---

## phys3d_create_static_plane

```fides
pub fn phys3d_create_static_plane(normal: vec3, distance: f64) -> RigidBody3 {
```

**Parameters:**

- `normal`: `vec3`
- `distance`: `f64`

**Returns:** `RigidBody3`

*Defined at line 97*

---

## phys3d_set_gravity

```fides
pub fn phys3d_set_gravity(g: vec3) -> void {
```

**Parameters:**

- `g`: `vec3`

**Returns:** `void`

*Defined at line 112*

---

## phys3d_get_gravity

```fides
pub fn phys3d_get_gravity() -> vec3 {
```

**Returns:** `vec3`

*Defined at line 118*

---

## phys3d_apply_force

```fides
pub fn phys3d_apply_force(body: &mut RigidBody3, force: vec3) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `force`: `vec3`

**Returns:** `void`

*Defined at line 122*

---

## phys3d_apply_impulse

```fides
pub fn phys3d_apply_impulse(body: &mut RigidBody3, impulse: vec3) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `impulse`: `vec3`

**Returns:** `void`

*Defined at line 126*

---

## phys3d_apply_gravity

```fides
pub fn phys3d_apply_gravity(body: &mut RigidBody3, dt: f64) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 133*

---

## phys3d_step

```fides
pub fn phys3d_step(body: &mut RigidBody3, dt: f64) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 142*

---

## Contact3

```fides
pub struct Contact3 {
```

**Fields:**

| Name | Type |
|------|------|
| `normal` | `vec3` |
| `depth` | `f64` |
| `point` | `vec3` |
| `has_contact` | `bool` |

*Defined at line 171*

---

## phys3d_sphere_vs_sphere


*Example -- run as a test by `scripts/doctest.py`:*

```fides
val a = phys3d_create_sphere(vec3(0.0, 0.0, 0.0), 1.0, 1.0);
val b = phys3d_create_sphere(vec3(1.5, 0.0, 0.0), 1.0, 1.0);
val c = phys3d_sphere_vs_sphere(&a, &b);
assert(c.has_contact);
assert(c.depth == 0.5);
assert(c.normal.x == 1.0);
val far = phys3d_create_sphere(vec3(5.0, 0.0, 0.0), 1.0, 1.0);
assert(!phys3d_sphere_vs_sphere(&a, &far).has_contact);
```

```fides
pub fn phys3d_sphere_vs_sphere(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 191*

---

## phys3d_aabb_vs_aabb

```fides
pub fn phys3d_aabb_vs_aabb(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 215*

---

## phys3d_sphere_vs_plane

```fides
pub fn phys3d_sphere_vs_plane(sphere: &RigidBody3, plane: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `sphere`: `&RigidBody3`
- `plane`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 282*

---

## phys3d_sphere_vs_aabb

```fides
pub fn phys3d_sphere_vs_aabb(sphere: &RigidBody3, box_body: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `sphere`: `&RigidBody3`
- `box_body`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 308*

---

## phys3d_resolve_contact

```fides
pub fn phys3d_resolve_contact(a: &mut RigidBody3, b: &mut RigidBody3, contact: &Contact3) -> void {
```

**Parameters:**

- `a`: `&mut RigidBody3`
- `b`: `&mut RigidBody3`
- `contact`: `&Contact3`

**Returns:** `void`

*Defined at line 361*

---

## phys3d_check_collision

```fides
pub fn phys3d_check_collision(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 436*

---

## phys3d_resolve_all

Resolve all collisions in a list of bodies (brute force, O(n^2)).

```fides
pub fn phys3d_resolve_all(bodies: &mut Vec<RigidBody3>, dt: f64) -> void {
```

**Parameters:**

- `bodies`: `&mut Vec<RigidBody3>`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 466*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
