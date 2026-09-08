# physics — Fides Documentation

## Contents

- [AABB](#aabb) (struct)
- [physics_aabb_new](#physics_aabb_new) (function)
- [physics_aabb_collides](#physics_aabb_collides) (function)
- [physics_aabb_contains_point](#physics_aabb_contains_point) (function)
- [physics_aabb_center](#physics_aabb_center) (function)
- [physics_aabb_overlap](#physics_aabb_overlap) (function)
- [physics_aabb_mtv](#physics_aabb_mtv) (function)
- [Circle](#circle) (struct)
- [physics_circle_new](#physics_circle_new) (function)
- [physics_circle_collides](#physics_circle_collides) (function)
- [physics_circle_contains](#physics_circle_contains) (function)
- [physics_circle_distance](#physics_circle_distance) (function)
- [Ray](#ray) (struct)
- [physics_ray_new](#physics_ray_new) (function)
- [physics_ray_vs_aabb](#physics_ray_vs_aabb) (function)
- [physics_ray_vs_circle](#physics_ray_vs_circle) (function)
- [Body](#body) (struct)
- [physics_body_create](#physics_body_create) (function)
- [physics_body_set_velocity](#physics_body_set_velocity) (function)
- [physics_body_set_acceleration](#physics_body_set_acceleration) (function)
- [physics_body_set_mass](#physics_body_set_mass) (function)
- [physics_body_set_restitution](#physics_body_set_restitution) (function)
- [physics_body_apply_force](#physics_body_apply_force) (function)
- [physics_body_apply_impulse](#physics_body_apply_impulse) (function)
- [physics_body_update](#physics_body_update) (function)
- [physics_body_aabb](#physics_body_aabb) (function)
- [physics_resolve_collision](#physics_resolve_collision) (function)
- [physics_resolve_vs_static](#physics_resolve_vs_static) (function)
- [physics_elastic_collision](#physics_elastic_collision) (function)

## AABB

```fides
pub struct AABB {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64,       // top-left x` |
| `y` | `f64,       // top-left y` |
| `width` | `f64,   // box width` |
| `height` | `f64,  // box height` |

*Defined at line 18*

---

## physics_aabb_new

Create an AABB.

```fides
pub fn physics_aabb_new(x: f64, y: f64, width: f64, height: f64) -> AABB {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `width`: `f64`
- `height`: `f64`

**Returns:** `AABB`

*Defined at line 26*

---

## physics_aabb_collides

Check if two AABBs overlap.

```fides
pub fn physics_aabb_collides(a: AABB, b: AABB) -> bool {
```

**Parameters:**

- `a`: `AABB`
- `b`: `AABB`

**Returns:** `bool`

*Defined at line 31*

---

## physics_aabb_contains_point

Check if a point is inside an AABB.

```fides
pub fn physics_aabb_contains_point(box: AABB, px: f64, py: f64) -> bool {
```

**Parameters:**

- `box`: `AABB`
- `px`: `f64`
- `py`: `f64`

**Returns:** `bool`

*Defined at line 40*

---

## physics_aabb_center

Get the center of an AABB.

```fides
pub fn physics_aabb_center(box: AABB) -> (f64, f64) {
```

**Parameters:**

- `box`: `AABB`

**Returns:** `(f64, f64)`

*Defined at line 45*

---

## physics_aabb_overlap

Get the overlap region of two overlapping AABBs. Returns (x, y, w, h).
If they don't overlap, returns (0, 0, 0, 0).

```fides
pub fn physics_aabb_overlap(a: AABB, b: AABB) -> (f64, f64, f64, f64) {
```

**Parameters:**

- `a`: `AABB`
- `b`: `AABB`

**Returns:** `(f64, f64, f64, f64)`

*Defined at line 51*

---

## physics_aabb_mtv

Compute the minimum translation vector to separate two overlapping AABBs.
Returns (dx, dy) — the direction to push `a` out of `b`.

```fides
pub fn physics_aabb_mtv(a: AABB, b: AABB) -> (f64, f64) {
```

**Parameters:**

- `a`: `AABB`
- `b`: `AABB`

**Returns:** `(f64, f64)`

*Defined at line 69*

---

## Circle

```fides
pub struct Circle {
```

**Fields:**

| Name | Type |
|------|------|
| `cx` | `f64,    // center x` |
| `cy` | `f64,    // center y` |
| `radius` | `f64` |

*Defined at line 101*

---

## physics_circle_new

Create a circle.

```fides
pub fn physics_circle_new(cx: f64, cy: f64, radius: f64) -> Circle {
```

**Parameters:**

- `cx`: `f64`
- `cy`: `f64`
- `radius`: `f64`

**Returns:** `Circle`

*Defined at line 108*

---

## physics_circle_collides

Check if two circles overlap.

```fides
pub fn physics_circle_collides(a: Circle, b: Circle) -> bool {
```

**Parameters:**

- `a`: `Circle`
- `b`: `Circle`

**Returns:** `bool`

*Defined at line 113*

---

## physics_circle_contains

Check if a point is inside a circle.

```fides
pub fn physics_circle_contains(c: Circle, px: f64, py: f64) -> bool {
```

**Parameters:**

- `c`: `Circle`
- `px`: `f64`
- `py`: `f64`

**Returns:** `bool`

*Defined at line 121*

---

## physics_circle_distance

Get the distance between two circle centers.

```fides
pub fn physics_circle_distance(a: Circle, b: Circle) -> f64 {
```

**Parameters:**

- `a`: `Circle`
- `b`: `Circle`

**Returns:** `f64`

*Defined at line 128*

---

## Ray

```fides
pub struct Ray {
```

**Fields:**

| Name | Type |
|------|------|
| `origin_x` | `f64` |
| `origin_y` | `f64` |
| `dir_x` | `f64,    // normalized direction` |
| `dir_y` | `f64` |
| `max_dist` | `f64, // maximum ray distance` |

*Defined at line 146*

---

## physics_ray_new

Create a ray from origin in a direction with max distance.

```fides
pub fn physics_ray_new(ox: f64, oy: f64, dx: f64, dy: f64, max_dist: f64) -> Ray {
```

**Parameters:**

- `ox`: `f64`
- `oy`: `f64`
- `dx`: `f64`
- `dy`: `f64`
- `max_dist`: `f64`

**Returns:** `Ray`

*Defined at line 155*

---

## physics_ray_vs_aabb

Ray vs AABB intersection. Returns the hit distance, or -1.0 if no hit.
Uses the slab method.

```fides
pub fn physics_ray_vs_aabb(ray: Ray, box: AABB) -> f64 {
```

**Parameters:**

- `ray`: `Ray`
- `box`: `AABB`

**Returns:** `f64`

*Defined at line 182*

---

## physics_ray_vs_circle

Ray vs circle intersection. Returns hit distance or -1.0.

```fides
pub fn physics_ray_vs_circle(ray: Ray, c: Circle) -> f64 {
```

**Parameters:**

- `ray`: `Ray`
- `c`: `Circle`

**Returns:** `f64`

*Defined at line 222*

---

## Body

```fides
pub struct Body {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64,          // position x` |
| `y` | `f64,          // position y` |
| `vx` | `f64,         // velocity x (px/s)` |
| `vy` | `f64,         // velocity y (px/s)` |
| `ax` | `f64,         // acceleration x (px/s²)` |
| `ay` | `f64,         // acceleration y (px/s²)` |
| `width` | `f64,      // collision box width` |
| `height` | `f64,     // collision box height` |
| `mass` | `f64,       // mass (for collision response)` |
| `restitution` | `f64, // bounciness (0 = no bounce, 1 = perfect bounce)` |
| `grounded` | `bool,   // is the body on the ground?` |
| `active` | `bool,     // is the body being simulated?` |

*Defined at line 263*

---

## physics_body_create

Create a new rigid body.

```fides
pub fn physics_body_create(x: f64, y: f64, width: f64, height: f64) -> Body {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `width`: `f64`
- `height`: `f64`

**Returns:** `Body`

*Defined at line 279*

---

## physics_body_set_velocity

Set the body's velocity.

```fides
pub fn physics_body_set_velocity(body: &mut Body, vx: f64, vy: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `vx`: `f64`
- `vy`: `f64`

**Returns:** `void`

*Defined at line 297*

---

## physics_body_set_acceleration

Set the body's acceleration.

```fides
pub fn physics_body_set_acceleration(body: &mut Body, ax: f64, ay: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `ax`: `f64`
- `ay`: `f64`

**Returns:** `void`

*Defined at line 303*

---

## physics_body_set_mass

Set the body's mass.

```fides
pub fn physics_body_set_mass(body: &mut Body, mass: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `mass`: `f64`

**Returns:** `void`

*Defined at line 309*

---

## physics_body_set_restitution

Set the body's restitution (bounciness).

```fides
pub fn physics_body_set_restitution(body: &mut Body, r: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `r`: `f64`

**Returns:** `void`

*Defined at line 314*

---

## physics_body_apply_force

Apply a force to the body (F = ma → a += F/m).

```fides
pub fn physics_body_apply_force(body: &mut Body, fx: f64, fy: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `fx`: `f64`
- `fy`: `f64`

**Returns:** `void`

*Defined at line 319*

---

## physics_body_apply_impulse

Apply an impulse (instant velocity change).

```fides
pub fn physics_body_apply_impulse(body: &mut Body, ix: f64, iy: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `ix`: `f64`
- `iy`: `f64`

**Returns:** `void`

*Defined at line 327*

---

## physics_body_update

Update the body's physics: integrate velocity and acceleration.
gravity = downward acceleration (px/s²), e.g., 980.0 for Earth-like.

```fides
pub fn physics_body_update(body: &mut Body, dt: f64, gravity: f64) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `dt`: `f64`
- `gravity`: `f64`

**Returns:** `void`

*Defined at line 336*

---

## physics_body_aabb

Get the body's AABB.

```fides
pub fn physics_body_aabb(body: Body) -> AABB {
```

**Parameters:**

- `body`: `Body`

**Returns:** `AABB`

*Defined at line 359*

---

## physics_resolve_collision

Resolve collision between two bodies using their AABBs.
Pushes body `a` out of body `b` and adjusts velocity.

```fides
pub fn physics_resolve_collision(a: &mut Body, b: Body) -> void {
```

**Parameters:**

- `a`: `&mut Body`
- `b`: `Body`

**Returns:** `void`

*Defined at line 372*

---

## physics_resolve_vs_static

Resolve collision between a body and a static AABB (e.g., a tile).
The body is pushed out and velocity is zeroed in the push direction.

```fides
pub fn physics_resolve_vs_static(body: &mut Body, tile: AABB) -> void {
```

**Parameters:**

- `body`: `&mut Body`
- `tile`: `AABB`

**Returns:** `void`

*Defined at line 403*

---

## physics_elastic_collision

Elastic collision response between two dynamic bodies.
Exchanges momentum based on mass and restitution.

```fides
pub fn physics_elastic_collision(a: &mut Body, b: &mut Body) -> void {
```

**Parameters:**

- `a`: `&mut Body`
- `b`: `&mut Body`

**Returns:** `void`

*Defined at line 443*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
