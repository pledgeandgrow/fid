[All modules](index.md)

# steering -- Fides Documentation

## Contents

- [SteerAgent](#SteerAgent) (struct)
- [steer_agent](#steer_agent) (function)
- [steer_seed](#steer_seed) (function)
- [steer_truncate](#steer_truncate) (function)
- [steer_heading](#steer_heading) (function)
- [steer_seek](#steer_seek) (function)
- [steer_flee](#steer_flee) (function)
- [steer_arrive](#steer_arrive) (function)
- [steer_pursue](#steer_pursue) (function)
- [steer_evade](#steer_evade) (function)
- [steer_wander](#steer_wander) (function)
- [SteerObstacle](#SteerObstacle) (struct)
- [steer_obstacle](#steer_obstacle) (function)
- [SteerHit](#SteerHit) (struct)
- [steer_detect](#steer_detect) (function)
- [steer_avoid](#steer_avoid) (function)
- [SteerPath](#SteerPath) (struct)
- [steer_path](#steer_path) (function)
- [steer_path_add](#steer_path_add) (function)
- [steer_closest_on_segment](#steer_closest_on_segment) (function)
- [steer_path_segment](#steer_path_segment) (function)
- [steer_follow_path](#steer_follow_path) (function)
- [steer_separation](#steer_separation) (function)
- [steer_alignment](#steer_alignment) (function)
- [steer_cohesion](#steer_cohesion) (function)
- [steer_flock](#steer_flock) (function)
- [steer_flock_step](#steer_flock_step) (function)
- [steer_apply](#steer_apply) (function)

## SteerAgent

```fides
pub struct SteerAgent {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `vec2` |
| `vel` | `vec2` |
| `max_speed` | `f64` |
| `max_force` | `f64` |
| `mass` | `f64` |
| `radius` | `f64` |
| `wander_angle` | `f64` |
| `rng` | `i64,  // state of the agent's generator (wander)` |

*Defined at line 29*

---

## steer_agent

An agent at rest at `pos`, mass 1, radius 0.5.

```fides
pub fn steer_agent(pos: vec2, max_speed: f64, max_force: f64) -> SteerAgent {
```

**Parameters:**

- `pos`: `vec2`
- `max_speed`: `f64`
- `max_force`: `f64`

**Returns:** `SteerAgent`

*Defined at line 43*

---

## steer_seed

Seed the agent's wander generator.

```fides
pub fn steer_seed(a: &mut SteerAgent, seed: i64) {
```

**Parameters:**

- `a`: `&mut SteerAgent`
- `seed`: `i64`

*Defined at line 50*

---

## steer_truncate

`v` shortened to length `max` when it is longer.

```fides
pub fn steer_truncate(v: vec2, max: f64) -> vec2 {
```

**Parameters:**

- `v`: `vec2`
- `max`: `f64`

**Returns:** `vec2`

*Defined at line 60*

---

## steer_heading

The direction the agent faces: its velocity, or +x when at rest.

```fides
pub fn steer_heading(a: &SteerAgent) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`

**Returns:** `vec2`

*Defined at line 73*

---

## steer_seek

Head for `target` at full speed.

```fides
pub fn steer_seek(a: &SteerAgent, target: vec2) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `target`: `vec2`

**Returns:** `vec2`

*Defined at line 96*

---

## steer_flee

Run from `threat` while it is closer than `panic_dist` (<= 0: always).

```fides
pub fn steer_flee(a: &SteerAgent, threat: vec2, panic_dist: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `threat`: `vec2`
- `panic_dist`: `f64`

**Returns:** `vec2`

*Defined at line 102*

---

## steer_arrive

Seek `target`, slowing down linearly inside `slow_radius` to stop on it.

```fides
pub fn steer_arrive(a: &SteerAgent, target: vec2, slow_radius: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `target`: `vec2`
- `slow_radius`: `f64`

**Returns:** `vec2`

*Defined at line 112*

---

## steer_pursue

Seek where a target at `tpos` moving at `tvel` is going to be.

```fides
pub fn steer_pursue(a: &SteerAgent, tpos: vec2, tvel: vec2, max_predict: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `tpos`: `vec2`
- `tvel`: `vec2`
- `max_predict`: `f64`

**Returns:** `vec2`

*Defined at line 141*

---

## steer_evade

Flee from where a pursuer at `tpos` moving at `tvel` is going to be.

```fides
pub fn steer_evade(a: &SteerAgent, tpos: vec2, tvel: vec2, max_predict: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `tpos`: `vec2`
- `tvel`: `vec2`
- `max_predict`: `f64`

**Returns:** `vec2`

*Defined at line 146*

---

## steer_wander

Wander: a target on a circle of `radius` at `distance` ahead; its angle
moves by up to +-`jitter` radians per call.

```fides
pub fn steer_wander(a: &mut SteerAgent, distance: f64, radius: f64, jitter: f64) -> vec2 {
```

**Parameters:**

- `a`: `&mut SteerAgent`
- `distance`: `f64`
- `radius`: `f64`
- `jitter`: `f64`

**Returns:** `vec2`

*Defined at line 152*

---

## SteerObstacle

```fides
pub struct SteerObstacle {
```

**Fields:**

| Name | Type |
|------|------|
| `center` | `vec2` |
| `radius` | `f64` |

*Defined at line 166*

---

## steer_obstacle

```fides
pub fn steer_obstacle(center: vec2, radius: f64) -> SteerObstacle {
```

**Parameters:**

- `center`: `vec2`
- `radius`: `f64`

**Returns:** `SteerObstacle`

*Defined at line 171*

---

## SteerHit

What the detection box of `steer_avoid` found.

```fides
pub struct SteerHit {
```

**Fields:**

| Name | Type |
|------|------|
| `index` | `i64` |
| `ahead` | `f64` |
| `lateral` | `f64` |
| `box_len` | `f64` |

*Defined at line 178*

---

## steer_detect

The nearest obstacle overlapping a box `min_len` long (longer with
speed: up to 2x at max_speed) and as wide as the agent, ahead of it.

```fides
pub fn steer_detect(a: &SteerAgent, obstacles: &Vec<SteerObstacle>, min_len: f64) -> SteerHit {
```

**Parameters:**

- `a`: `&SteerAgent`
- `obstacles`: `&Vec<SteerObstacle>`
- `min_len`: `f64`

**Returns:** `SteerHit`

*Defined at line 190*

---

## steer_avoid

Avoid circular obstacles: the nearest one in the detection box
(`steer_detect`) pushes the agent sideways, harder the closer it is, and
brakes it. Zero when the way is clear.

```fides
pub fn steer_avoid(a: &SteerAgent, obstacles: &Vec<SteerObstacle>, min_len: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `obstacles`: `&Vec<SteerObstacle>`
- `min_len`: `f64`

**Returns:** `vec2`

*Defined at line 232*

---

## SteerPath

```fides
pub struct SteerPath {
```

**Fields:**

| Name | Type |
|------|------|
| `points` | `Vec<vec2>` |
| `radius` | `f64` |

*Defined at line 253*

---

## steer_path

```fides
pub fn steer_path(radius: f64) -> SteerPath {
```

**Parameters:**

- `radius`: `f64`

**Returns:** `SteerPath`

*Defined at line 258*

---

## steer_path_add

```fides
pub fn steer_path_add(p: &mut SteerPath, pt: vec2) {
```

**Parameters:**

- `p`: `&mut SteerPath`
- `pt`: `vec2`

*Defined at line 264*

---

## steer_closest_on_segment

The point of segment a-b nearest to `p`.

```fides
pub fn steer_closest_on_segment(p: vec2, a: vec2, b: vec2) -> vec2 {
```

**Parameters:**

- `p`: `vec2`
- `a`: `vec2`
- `b`: `vec2`

**Returns:** `vec2`

*Defined at line 269*

---

## steer_path_segment

Index of the path segment (from point i to i+1) nearest to `p`.

```fides
pub fn steer_path_segment(path: &SteerPath, p: vec2) -> i64 {
```

**Parameters:**

- `path`: `&SteerPath`
- `p`: `vec2`

**Returns:** `i64`

*Defined at line 286*

---

## steer_follow_path

Follow `path`: look `lookahead` units ahead along the velocity; if that
point is outside the path's radius, seek the point `lookahead` further
along the nearest segment. Zero while the agent stays inside. On the
last segment the agent arrives at the final point.

```fides
pub fn steer_follow_path(a: &SteerAgent, path: &SteerPath, lookahead: f64) -> vec2 {
```

**Parameters:**

- `a`: `&SteerAgent`
- `path`: `&SteerPath`
- `lookahead`: `f64`

**Returns:** `vec2`

*Defined at line 306*

---

## steer_separation

Steer away from neighbours of `flock[self_i]` within `radius` (each
pushes with 1/distance).

```fides
pub fn steer_separation(flock: &Vec<SteerAgent>, self_i: i64, radius: f64) -> vec2 {
```

**Parameters:**

- `flock`: `&Vec<SteerAgent>`
- `self_i`: `i64`
- `radius`: `f64`

**Returns:** `vec2`

*Defined at line 337*

---

## steer_alignment

Match the average heading of neighbours within `radius`.

```fides
pub fn steer_alignment(flock: &Vec<SteerAgent>, self_i: i64, radius: f64) -> vec2 {
```

**Parameters:**

- `flock`: `&Vec<SteerAgent>`
- `self_i`: `i64`
- `radius`: `f64`

**Returns:** `vec2`

*Defined at line 361*

---

## steer_cohesion

Seek the centre of mass of neighbours within `radius`.

```fides
pub fn steer_cohesion(flock: &Vec<SteerAgent>, self_i: i64, radius: f64) -> vec2 {
```

**Parameters:**

- `flock`: `&Vec<SteerAgent>`
- `self_i`: `i64`
- `radius`: `f64`

**Returns:** `vec2`

*Defined at line 384*

---

## steer_flock

Weighted sum of separation, alignment and cohesion, limited to max_force.

```fides
pub fn steer_flock(flock: &Vec<SteerAgent>, self_i: i64, radius: f64, w_sep: f64, w_ali: f64, w_coh: f64) -> vec2 {
```

**Parameters:**

- `flock`: `&Vec<SteerAgent>`
- `self_i`: `i64`
- `radius`: `f64`
- `w_sep`: `f64`
- `w_ali`: `f64`
- `w_coh`: `f64`

**Returns:** `vec2`

*Defined at line 406*

---

## steer_flock_step

One flocking step for every agent: forces from the current state, then
all agents move (so the order of agents does not matter).

```fides
pub fn steer_flock_step(flock: &mut Vec<SteerAgent>, radius: f64, w_sep: f64, w_ali: f64, w_coh: f64, dt: f64) {
```

**Parameters:**

- `flock`: `&mut Vec<SteerAgent>`
- `radius`: `f64`
- `w_sep`: `f64`
- `w_ali`: `f64`
- `w_coh`: `f64`
- `dt`: `f64`

*Defined at line 415*

---

## steer_apply

Apply `force` for `dt` seconds: velocity += force / mass * dt (capped at
max_speed), position += velocity * dt.

```fides
pub fn steer_apply(a: &mut SteerAgent, force: vec2, dt: f64) {
```

**Parameters:**

- `a`: `&mut SteerAgent`
- `force`: `vec2`
- `dt`: `f64`

*Defined at line 435*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
