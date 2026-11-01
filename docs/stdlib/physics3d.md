# physics3d — Fides Documentation

## Contents

- [COLLIDER_SPHERE:](#collider_sphere:) (const)
- [COLLIDER_BOX:](#collider_box:) (const)
- [COLLIDER_PLANE:](#collider_plane:) (const)
- [Sphere](#sphere) (struct)
- [Aabb3](#aabb3) (struct)
- [Plane3](#plane3) (struct)
- [RigidBody3](#rigidbody3) (struct)
- [phys3d_create_body](#phys3d_create_body) (function)
- [phys3d_create_sphere](#phys3d_create_sphere) (function)
- [phys3d_create_static_plane](#phys3d_create_static_plane) (function)
- [phys3d_set_gravity](#phys3d_set_gravity) (function)
- [phys3d_get_gravity](#phys3d_get_gravity) (function)
- [phys3d_apply_force](#phys3d_apply_force) (function)
- [phys3d_apply_impulse](#phys3d_apply_impulse) (function)
- [phys3d_apply_gravity](#phys3d_apply_gravity) (function)
- [phys3d_step](#phys3d_step) (function)
- [Contact3](#contact3) (struct)
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

*Defined at line 29*

---

## COLLIDER_BOX:

```fides
pub const COLLIDER_BOX: i64 = 1;
```

*Defined at line 30*

---

## COLLIDER_PLANE:

```fides
pub const COLLIDER_PLANE: i64 = 2;
```

*Defined at line 31*

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

*Defined at line 33*

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

*Defined at line 38*

---

## Plane3

```fides
pub struct Plane3 {
```

**Fields:**

| Name | Type |
|------|------|
| `normal` | `vec3` |
| `distance` | `f64,    // distance from origin along normal` |

*Defined at line 43*

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
| `force` | `vec3,          // accumulated force` |
| `mass` | `f64` |
| `inv_mass` | `f64,        // 1/mass (0 = static)` |
| `restitution` | `f64,     // bounciness (0-1)` |
| `friction` | `f64,         // friction coefficient (0-1)` |
| `is_static` | `bool` |
| `collider_type` | `i64` |
| `sphere` | `Sphere` |
| `box` | `Aabb3` |
| `radius` | `f64,           // for sphere collider` |
| `half_extents` | `vec3,   // for box collider` |

*Defined at line 50*

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

*Defined at line 67*

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

*Defined at line 91*

---

## phys3d_create_static_plane

```fides
pub fn phys3d_create_static_plane(normal: vec3, distance: f64) -> RigidBody3 {
```

**Parameters:**

- `normal`: `vec3`
- `distance`: `f64`

**Returns:** `RigidBody3`

*Defined at line 112*

---

## phys3d_set_gravity

```fides
pub fn phys3d_set_gravity(g: vec3) -> void {
```

**Parameters:**

- `g`: `vec3`

**Returns:** `void`

*Defined at line 137*

---

## phys3d_get_gravity

```fides
pub fn phys3d_get_gravity() -> vec3 {
```

**Returns:** `vec3`

*Defined at line 143*

---

## phys3d_apply_force

```fides
pub fn phys3d_apply_force(body: &mut RigidBody3, force: vec3) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `force`: `vec3`

**Returns:** `void`

*Defined at line 147*

---

## phys3d_apply_impulse

```fides
pub fn phys3d_apply_impulse(body: &mut RigidBody3, impulse: vec3) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `impulse`: `vec3`

**Returns:** `void`

*Defined at line 151*

---

## phys3d_apply_gravity

```fides
pub fn phys3d_apply_gravity(body: &mut RigidBody3, dt: f64) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 156*

---

## phys3d_step

```fides
pub fn phys3d_step(body: &mut RigidBody3, dt: f64) -> void {
```

**Parameters:**

- `body`: `&mut RigidBody3`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 163*

---

## Contact3

```fides
pub struct Contact3 {
```

**Fields:**

| Name | Type |
|------|------|
| `normal` | `vec3,       // collision normal (from A to B)` |
| `depth` | `f64,         // penetration depth` |
| `point` | `vec3,        // contact point` |
| `has_contact` | `bool` |

*Defined at line 190*

---

## phys3d_sphere_vs_sphere

```fides
pub fn phys3d_sphere_vs_sphere(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 198*

---

## phys3d_aabb_vs_aabb

```fides
pub fn phys3d_aabb_vs_aabb(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 225*

---

## phys3d_sphere_vs_plane

```fides
pub fn phys3d_sphere_vs_plane(sphere: &RigidBody3, plane: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `sphere`: `&RigidBody3`
- `plane`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 288*

---

## phys3d_sphere_vs_aabb

```fides
pub fn phys3d_sphere_vs_aabb(sphere: &RigidBody3, box_body: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `sphere`: `&RigidBody3`
- `box_body`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 314*

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

*Defined at line 358*

---

## phys3d_check_collision

```fides
pub fn phys3d_check_collision(a: &RigidBody3, b: &RigidBody3) -> Contact3 {
```

**Parameters:**

- `a`: `&RigidBody3`
- `b`: `&RigidBody3`

**Returns:** `Contact3`

*Defined at line 429*

---

## phys3d_resolve_all

Resolve all collisions in a list of bodies (brute force, O(n^2)).

```fides
pub fn phys3d_resolve_all(bodies: Vec<RigidBody3>, dt: f64) -> void {
```

**Parameters:**

- `bodies`: `Vec<RigidBody3>`
- `dt`: `f64`

**Returns:** `void`

*Defined at line 462*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
