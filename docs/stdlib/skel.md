# skel — Fides Documentation

## Contents

- [Bone](#bone) (struct)
- [Skeleton](#skeleton) (struct)
- [skel_create](#skel_create) (function)
- [skel_add_bone](#skel_add_bone) (function)
- [skel_find_bone](#skel_find_bone) (function)
- [skel_compute_globals](#skel_compute_globals) (function)
- [skel_finalize](#skel_finalize) (function)
- [skel_bone_count](#skel_bone_count) (function)
- [Keyframe](#keyframe) (struct)
- [AnimationTrack](#animationtrack) (struct)
- [Animation](#animation) (struct)
- [skel_create_animation](#skel_create_animation) (function)
- [skel_set_keyframe](#skel_set_keyframe) (function)
- [skel_sample_animation](#skel_sample_animation) (function)
- [skel_compute_pose](#skel_compute_pose) (function)
- [VertexWeight](#vertexweight) (struct)
- [Skin](#skin) (struct)
- [skel_create_skin](#skel_create_skin) (function)
- [skel_set_vertex_weight](#skel_set_vertex_weight) (function)
- [skel_skin_vertex](#skel_skin_vertex) (function)
- [skel_blend_samples](#skel_blend_samples) (function)
- [skel_blend_poses](#skel_blend_poses) (function)

## Bone

```fides
pub struct Bone {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `parent` | `i64,         // parent bone index (-1 = root)` |
| `local_pos` | `vec3,     // local position relative to parent` |
| `local_rot` | `quat,     // local rotation relative to parent` |
| `local_scale` | `vec3,   // local scale` |
| `global_pos` | `vec3,    // computed global position` |
| `global_rot` | `quat,    // computed global rotation` |
| `global_scale` | `vec3,  // computed global scale` |
| `offset_matrix` | `mat4, // inverse bind pose matrix` |

*Defined at line 35*

---

## Skeleton

```fides
pub struct Skeleton {
```

**Fields:**

| Name | Type |
|------|------|
| `bones` | `Vec<Bone>` |
| `finalized` | `bool` |

*Defined at line 47*

---

## skel_create

```fides
pub fn skel_create() -> Skeleton {
```

**Returns:** `Skeleton`

*Defined at line 52*

---

## skel_add_bone

Add a bone to the skeleton. Returns the bone index.

```fides
pub fn skel_add_bone(skel: &mut Skeleton, parent: i64, name: str, pos: vec3, rot: quat) -> i64 {
```

**Parameters:**

- `skel`: `&mut Skeleton`
- `parent`: `i64`
- `name`: `str`
- `pos`: `vec3`
- `rot`: `quat`

**Returns:** `i64`

*Defined at line 60*

---

## skel_find_bone

Get bone index by name. Returns -1 if not found.

```fides
pub fn skel_find_bone(skel: &Skeleton, name: str) -> i64 {
```

**Parameters:**

- `skel`: `&Skeleton`
- `name`: `str`

**Returns:** `i64`

*Defined at line 77*

---

## skel_compute_globals

Compute global transforms for all bones (forward kinematics).

```fides
pub fn skel_compute_globals(skel: &mut Skeleton) -> void {
```

**Parameters:**

- `skel`: `&mut Skeleton`

**Returns:** `void`

*Defined at line 89*

---

## skel_finalize

Finalize the skeleton: compute globals and inverse bind matrices.

```fides
pub fn skel_finalize(skel: &mut Skeleton) -> void {
```

**Parameters:**

- `skel`: `&mut Skeleton`

**Returns:** `void`

*Defined at line 121*

---

## skel_bone_count

Get the number of bones.

```fides
pub fn skel_bone_count(skel: &Skeleton) -> i64 {
```

**Parameters:**

- `skel`: `&Skeleton`

**Returns:** `i64`

*Defined at line 142*

---

## Keyframe

```fides
pub struct Keyframe {
```

**Fields:**

| Name | Type |
|------|------|
| `time` | `f64,            // timestamp in seconds` |
| `translation` | `vec3` |
| `rotation` | `quat` |
| `scale` | `vec3` |

*Defined at line 148*

---

## AnimationTrack

```fides
pub struct AnimationTrack {
```

**Fields:**

| Name | Type |
|------|------|
| `keyframes` | `Vec<Keyframe>` |

*Defined at line 155*

---

## Animation

```fides
pub struct Animation {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `bone_count` | `i64` |
| `tracks` | `Vec<AnimationTrack>,  // one track per bone` |
| `duration` | `f64` |
| `looping` | `bool` |

*Defined at line 159*

---

## skel_create_animation

Create an animation with the given name and bone count.

```fides
pub fn skel_create_animation(name: str, bone_count: i64) -> Animation {
```

**Parameters:**

- `name`: `str`
- `bone_count`: `i64`

**Returns:** `Animation`

*Defined at line 168*

---

## skel_set_keyframe

Add a keyframe to a bone's track.

```fides
pub fn skel_set_keyframe(anim: &mut Animation, bone_idx: i64, time: f64, pos: vec3, rot: quat, scale: vec3) -> void {
```

**Parameters:**

- `anim`: `&mut Animation`
- `bone_idx`: `i64`
- `time`: `f64`
- `pos`: `vec3`
- `rot`: `quat`
- `scale`: `vec3`

**Returns:** `void`

*Defined at line 185*

---

## skel_sample_animation

Sample an animation at a given time, producing local transforms per bone.
Returns a vector of Keyframe (one per bone).

```fides
pub fn skel_sample_animation(anim: &Animation, time: f64) -> Vec<Keyframe> {
```

**Parameters:**

- `anim`: `&Animation`
- `time`: `f64`

**Returns:** `Vec<Keyframe>`

*Defined at line 220*

---

## skel_compute_pose

Compute the global pose of a skeleton at a given animation time.
Updates the skeleton's bone global transforms.

```fides
pub fn skel_compute_pose(skel: &mut Skeleton, anim: &Animation, time: f64) -> void {
```

**Parameters:**

- `skel`: `&mut Skeleton`
- `anim`: `&Animation`
- `time`: `f64`

**Returns:** `void`

*Defined at line 281*

---

## VertexWeight

```fides
pub struct VertexWeight {
```

**Fields:**

| Name | Type |
|------|------|
| `bone_indices` | `Vec<i64>,   // up to 4 bone indices` |
| `bone_weights` | `Vec<f64>,   // up to 4 bone weights (sum to 1.0)` |

*Defined at line 303*

---

## Skin

```fides
pub struct Skin {
```

**Fields:**

| Name | Type |
|------|------|
| `weights` | `Vec<VertexWeight>` |
| `bone_count` | `i64` |

*Defined at line 308*

---

## skel_create_skin

```fides
pub fn skel_create_skin(vertex_count: i64, bone_count: i64) -> Skin {
```

**Parameters:**

- `vertex_count`: `i64`
- `bone_count`: `i64`

**Returns:** `Skin`

*Defined at line 313*

---

## skel_set_vertex_weight

Set vertex skin weights.

```fides
pub fn skel_set_vertex_weight(skin: &mut Skin, vertex_idx: i64, b0: i64, b1: i64, b2: i64, b3: i64, w0: f64, w1: f64, w2: f64, w3: f64) -> void {
```

**Parameters:**

- `skin`: `&mut Skin`
- `vertex_idx`: `i64`
- `b0`: `i64`
- `b1`: `i64`
- `b2`: `i64`
- `b3`: `i64`
- `w0`: `f64`
- `w1`: `f64`
- `w2`: `f64`
- `w3`: `f64`

**Returns:** `void`

*Defined at line 337*

---

## skel_skin_vertex

Skin a vertex: compute final position from bone transforms and weights.

```fides
pub fn skel_skin_vertex(skin: &Skin, skel: &Skeleton, vertex_idx: i64, pos: vec3) -> vec3 {
```

**Parameters:**

- `skin`: `&Skin`
- `skel`: `&Skeleton`
- `vertex_idx`: `i64`
- `pos`: `vec3`

**Returns:** `vec3`

*Defined at line 352*

---

## skel_blend_samples

Blend two animation samples with a weight factor.

```fides
pub fn skel_blend_samples(a: Vec<Keyframe>, b: Vec<Keyframe>, weight: f64) -> Vec<Keyframe> {
```

**Parameters:**

- `a`: `Vec<Keyframe>`
- `b`: `Vec<Keyframe>`
- `weight`: `f64`

**Returns:** `Vec<Keyframe>`

*Defined at line 377*

---

## skel_blend_poses

Blend two animations and compute the resulting pose.

```fides
pub fn skel_blend_poses(skel: &mut Skeleton, anim_a: &Animation, anim_b: &Animation, time_a: f64, time_b: f64, blend_weight: f64) -> void {
```

**Parameters:**

- `skel`: `&mut Skeleton`
- `anim_a`: `&Animation`
- `anim_b`: `&Animation`
- `time_a`: `f64`
- `time_b`: `f64`
- `blend_weight`: `f64`

**Returns:** `void`

*Defined at line 395*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
