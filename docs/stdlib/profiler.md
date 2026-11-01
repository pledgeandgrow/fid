# profiler — Fides Documentation

## Contents

- [ProfileSample](#profilesample) (struct)
- [FrameProfile](#frameprofile) (struct)
- [Profiler](#profiler) (struct)
- [profiler_create](#profiler_create) (function)
- [profiler_init](#profiler_init) (function)
- [profiler_set_enabled](#profiler_set_enabled) (function)
- [profiler_begin_frame](#profiler_begin_frame) (function)
- [profiler_begin](#profiler_begin) (function)
- [profiler_end](#profiler_end) (function)
- [profiler_count_draw_call](#profiler_count_draw_call) (function)
- [profiler_count_triangles](#profiler_count_triangles) (function)
- [profiler_count_texture_bind](#profiler_count_texture_bind) (function)
- [profiler_count_shader_switch](#profiler_count_shader_switch) (function)
- [profiler_end_frame](#profiler_end_frame) (function)
- [profiler_frame_time](#profiler_frame_time) (function)
- [profiler_fps](#profiler_fps) (function)
- [profiler_draw_calls](#profiler_draw_calls) (function)
- [profiler_triangles](#profiler_triangles) (function)
- [profiler_min_frame](#profiler_min_frame) (function)
- [profiler_max_frame](#profiler_max_frame) (function)
- [profiler_frame_count](#profiler_frame_count) (function)
- [profiler_get_sample](#profiler_get_sample) (function)
- [profiler_last_frame](#profiler_last_frame) (function)
- [profiler_reset](#profiler_reset) (function)
- [profiler_frame_time_history](#profiler_frame_time_history) (function)
- [profiler_shutdown](#profiler_shutdown) (function)
- [profiler_set_max_frames](#profiler_set_max_frames) (function)
- [profiler_sample_count](#profiler_sample_count) (function)
- [profiler_get_sample_idx](#profiler_get_sample_idx) (function)

## ProfileSample

```fides
pub struct ProfileSample {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `start_ms` | `f64` |
| `elapsed_ms` | `f64` |
| `depth` | `i64` |
| `parent` | `i64` |

*Defined at line 25*

---

## FrameProfile

```fides
pub struct FrameProfile {
```

**Fields:**

| Name | Type |
|------|------|
| `frame_number` | `i64` |
| `frame_time_ms` | `f64` |
| `draw_calls` | `i64` |
| `triangles` | `i64` |
| `texture_binds` | `i64` |
| `shader_switches` | `i64` |

*Defined at line 33*

---

## Profiler

```fides
pub struct Profiler {
```

**Fields:**

| Name | Type |
|------|------|
| `sample_names` | `Vec<str>` |
| `sample_starts` | `Vec<f64>` |
| `sample_elapsed` | `Vec<f64>` |
| `sample_depths` | `Vec<i64>` |
| `sample_parents` | `Vec<i64>` |
| `sample_stack` | `Vec<i64>` |
| `hist_frame_times` | `Vec<f64>` |
| `hist_draw_calls` | `Vec<i64>` |
| `hist_triangles` | `Vec<i64>` |
| `max_frames` | `i64` |
| `enabled` | `bool` |
| `frame_count` | `i64` |
| `avg_fps` | `f64` |
| `min_frame_ms` | `f64` |
| `max_frame_ms` | `f64` |
| `total_draw_calls` | `i64` |
| `current_depth` | `i64` |
| `cur_frame_number` | `i64` |
| `cur_frame_time` | `f64` |
| `cur_draw_calls` | `i64` |
| `cur_triangles` | `i64` |
| `cur_texture_binds` | `i64` |
| `cur_shader_switches` | `i64` |
| `cur_frame_start` | `f64` |

*Defined at line 42*

---

## profiler_create

Create a new profiler.

```fides
pub fn profiler_create() -> Profiler {
```

**Returns:** `Profiler`

*Defined at line 70*

---

## profiler_init

Initialize/reset the profiler.

```fides
pub fn profiler_init(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 100*

---

## profiler_set_enabled

Enable/disable profiling.

```fides
pub fn profiler_set_enabled(p: &mut Profiler, enabled: bool) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`
- `enabled`: `bool`

**Returns:** `void`

*Defined at line 119*

---

## profiler_begin_frame

Begin a new frame.

```fides
pub fn profiler_begin_frame(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 124*

---

## profiler_begin

Begin a profiled scope.

```fides
pub fn profiler_begin(p: &mut Profiler, name: str) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`
- `name`: `str`

**Returns:** `void`

*Defined at line 143*

---

## profiler_end

End a profiled scope.

```fides
pub fn profiler_end(p: &mut Profiler, name: str) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`
- `name`: `str`

**Returns:** `void`

*Defined at line 163*

---

## profiler_count_draw_call

Record a draw call.

```fides
pub fn profiler_count_draw_call(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 176*

---

## profiler_count_triangles

Record triangles rendered.

```fides
pub fn profiler_count_triangles(p: &mut Profiler, count: i64) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`
- `count`: `i64`

**Returns:** `void`

*Defined at line 181*

---

## profiler_count_texture_bind

Record a texture bind.

```fides
pub fn profiler_count_texture_bind(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 186*

---

## profiler_count_shader_switch

Record a shader switch.

```fides
pub fn profiler_count_shader_switch(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 191*

---

## profiler_end_frame

End the current frame and store the profile.

```fides
pub fn profiler_end_frame(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 196*

---

## profiler_frame_time

Get the current frame time in milliseconds.

```fides
pub fn profiler_frame_time(p: &Profiler) -> f64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `f64`

*Defined at line 233*

---

## profiler_fps

Get the average FPS.

```fides
pub fn profiler_fps(p: &Profiler) -> f64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `f64`

*Defined at line 239*

---

## profiler_draw_calls

Get the number of draw calls in the last frame.

```fides
pub fn profiler_draw_calls(p: &Profiler) -> i64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `i64`

*Defined at line 244*

---

## profiler_triangles

Get the number of triangles in the last frame.

```fides
pub fn profiler_triangles(p: &Profiler) -> i64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `i64`

*Defined at line 250*

---

## profiler_min_frame

Get the minimum frame time.

```fides
pub fn profiler_min_frame(p: &Profiler) -> f64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `f64`

*Defined at line 256*

---

## profiler_max_frame

Get the maximum frame time.

```fides
pub fn profiler_max_frame(p: &Profiler) -> f64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `f64`

*Defined at line 261*

---

## profiler_frame_count

Get the number of stored frames.

```fides
pub fn profiler_frame_count(p: &Profiler) -> i64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `i64`

*Defined at line 266*

---

## profiler_get_sample

Get a sample from the current frame by name.

```fides
pub fn profiler_get_sample(p: &Profiler, name: str) -> ProfileSample {
```

**Parameters:**

- `p`: `&Profiler`
- `name`: `str`

**Returns:** `ProfileSample`

*Defined at line 271*

---

## profiler_last_frame

Get the last frame profile.

```fides
pub fn profiler_last_frame(p: &Profiler) -> FrameProfile {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `FrameProfile`

*Defined at line 292*

---

## profiler_reset

Reset profiler statistics.

```fides
pub fn profiler_reset(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 304*

---

## profiler_frame_time_history

Get the frame time history as a string.

```fides
pub fn profiler_frame_time_history(p: &Profiler) -> str {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `str`

*Defined at line 316*

---

## profiler_shutdown

Shutdown the profiler.

```fides
pub fn profiler_shutdown(p: &mut Profiler) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`

**Returns:** `void`

*Defined at line 328*

---

## profiler_set_max_frames

Set the maximum number of frames to keep in history.

```fides
pub fn profiler_set_max_frames(p: &mut Profiler, max: i64) -> void {
```

**Parameters:**

- `p`: `&mut Profiler`
- `max`: `i64`

**Returns:** `void`

*Defined at line 342*

---

## profiler_sample_count

Get the number of samples in the current frame.

```fides
pub fn profiler_sample_count(p: &Profiler) -> i64 {
```

**Parameters:**

- `p`: `&Profiler`

**Returns:** `i64`

*Defined at line 352*

---

## profiler_get_sample_idx

Get a sample by index.

```fides
pub fn profiler_get_sample_idx(p: &Profiler, idx: i64) -> ProfileSample {
```

**Parameters:**

- `p`: `&Profiler`
- `idx`: `i64`

**Returns:** `ProfileSample`

*Defined at line 357*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
