[All modules](index.md)

# webgpu -- Fides Documentation

## Contents

- [WGPU_BUFFER_MAP_READ:](#WGPU_BUFFER_MAP_READ:) (const)
- [WGPU_BUFFER_MAP_WRITE:](#WGPU_BUFFER_MAP_WRITE:) (const)
- [WGPU_BUFFER_COPY_SRC:](#WGPU_BUFFER_COPY_SRC:) (const)
- [WGPU_BUFFER_COPY_DST:](#WGPU_BUFFER_COPY_DST:) (const)
- [WGPU_BUFFER_INDEX:](#WGPU_BUFFER_INDEX:) (const)
- [WGPU_BUFFER_VERTEX:](#WGPU_BUFFER_VERTEX:) (const)
- [WGPU_BUFFER_UNIFORM:](#WGPU_BUFFER_UNIFORM:) (const)
- [WGPU_BUFFER_STORAGE:](#WGPU_BUFFER_STORAGE:) (const)
- [WGPU_BUFFER_INDIRECT:](#WGPU_BUFFER_INDIRECT:) (const)
- [WGPU_BUFFER_QUERY_RESOLVE:](#WGPU_BUFFER_QUERY_RESOLVE:) (const)
- [WGPU_TEXTURE_COPY_SRC:](#WGPU_TEXTURE_COPY_SRC:) (const)
- [WGPU_TEXTURE_COPY_DST:](#WGPU_TEXTURE_COPY_DST:) (const)
- [WGPU_TEXTURE_BINDING:](#WGPU_TEXTURE_BINDING:) (const)
- [WGPU_TEXTURE_STORAGE_BINDING:](#WGPU_TEXTURE_STORAGE_BINDING:) (const)
- [WGPU_TEXTURE_RENDER_ATTACHMENT:](#WGPU_TEXTURE_RENDER_ATTACHMENT:) (const)
- [WGPU_STAGE_VERTEX:](#WGPU_STAGE_VERTEX:) (const)
- [WGPU_STAGE_FRAGMENT:](#WGPU_STAGE_FRAGMENT:) (const)
- [WGPU_STAGE_COMPUTE:](#WGPU_STAGE_COMPUTE:) (const)
- [WGPU_MAP_READ:](#WGPU_MAP_READ:) (const)
- [WGPU_MAP_WRITE:](#WGPU_MAP_WRITE:) (const)
- [WGPU_COLOR_WRITE_RED:](#WGPU_COLOR_WRITE_RED:) (const)
- [WGPU_COLOR_WRITE_GREEN:](#WGPU_COLOR_WRITE_GREEN:) (const)
- [WGPU_COLOR_WRITE_BLUE:](#WGPU_COLOR_WRITE_BLUE:) (const)
- [WGPU_COLOR_WRITE_ALPHA:](#WGPU_COLOR_WRITE_ALPHA:) (const)
- [WGPU_COLOR_WRITE_ALL:](#WGPU_COLOR_WRITE_ALL:) (const)
- [wgpu_buffer_usage_str](#wgpu_buffer_usage_str) (function)
- [wgpu_validate_buffer](#wgpu_validate_buffer) (function)
- [WgpuVertexFormat](#WgpuVertexFormat) (struct)
- [wgpu_vertex_format](#wgpu_vertex_format) (function)
- [WgpuVertexLayout](#WgpuVertexLayout) (struct)
- [wgpu_vertex_layout_new](#wgpu_vertex_layout_new) (function)
- [wgpu_vertex_add](#wgpu_vertex_add) (function)
- [wgpu_vertex_wgsl](#wgpu_vertex_wgsl) (function)
- [WgpuTextureFormat](#WgpuTextureFormat) (struct)
- [wgpu_texture_format](#wgpu_texture_format) (function)
- [wgpu_bytes_per_row](#wgpu_bytes_per_row) (function)
- [WgpuBglEntry](#WgpuBglEntry) (struct)
- [WgpuBglBuilder](#WgpuBglBuilder) (struct)
- [wgpu_bgl_new](#wgpu_bgl_new) (function)
- [wgpu_bgl_buffer](#wgpu_bgl_buffer) (function)
- [wgpu_bgl_dynamic_buffer](#wgpu_bgl_dynamic_buffer) (function)
- [wgpu_bgl_sampler](#wgpu_bgl_sampler) (function)
- [wgpu_bgl_texture](#wgpu_bgl_texture) (function)
- [wgpu_bgl_storage_texture](#wgpu_bgl_storage_texture) (function)
- [wgpu_bgl_validate](#wgpu_bgl_validate) (function)
- [wgpu_stage_name](#wgpu_stage_name) (function)
- [wgpu_bgl_wgsl](#wgpu_bgl_wgsl) (function)
- [WgslLayout](#WgslLayout) (struct)
- [WgslTypes](#WgslTypes) (struct)
- [wgsl_types_new](#wgsl_types_new) (function)
- [wgsl_struct](#wgsl_struct) (function)
- [wgsl_layout](#wgsl_layout) (function)

## WGPU_BUFFER_MAP_READ:

```fides
pub const WGPU_BUFFER_MAP_READ: i64 = 0x0001;
```

*Defined at line 96*

---

## WGPU_BUFFER_MAP_WRITE:

```fides
pub const WGPU_BUFFER_MAP_WRITE: i64 = 0x0002;
```

*Defined at line 97*

---

## WGPU_BUFFER_COPY_SRC:

```fides
pub const WGPU_BUFFER_COPY_SRC: i64 = 0x0004;
```

*Defined at line 98*

---

## WGPU_BUFFER_COPY_DST:

```fides
pub const WGPU_BUFFER_COPY_DST: i64 = 0x0008;
```

*Defined at line 99*

---

## WGPU_BUFFER_INDEX:

```fides
pub const WGPU_BUFFER_INDEX: i64 = 0x0010;
```

*Defined at line 100*

---

## WGPU_BUFFER_VERTEX:

```fides
pub const WGPU_BUFFER_VERTEX: i64 = 0x0020;
```

*Defined at line 101*

---

## WGPU_BUFFER_UNIFORM:

```fides
pub const WGPU_BUFFER_UNIFORM: i64 = 0x0040;
```

*Defined at line 102*

---

## WGPU_BUFFER_STORAGE:

```fides
pub const WGPU_BUFFER_STORAGE: i64 = 0x0080;
```

*Defined at line 103*

---

## WGPU_BUFFER_INDIRECT:

```fides
pub const WGPU_BUFFER_INDIRECT: i64 = 0x0100;
```

*Defined at line 104*

---

## WGPU_BUFFER_QUERY_RESOLVE:

```fides
pub const WGPU_BUFFER_QUERY_RESOLVE: i64 = 0x0200;
```

*Defined at line 105*

---

## WGPU_TEXTURE_COPY_SRC:

```fides
pub const WGPU_TEXTURE_COPY_SRC: i64 = 0x01;
```

*Defined at line 107*

---

## WGPU_TEXTURE_COPY_DST:

```fides
pub const WGPU_TEXTURE_COPY_DST: i64 = 0x02;
```

*Defined at line 108*

---

## WGPU_TEXTURE_BINDING:

```fides
pub const WGPU_TEXTURE_BINDING: i64 = 0x04;
```

*Defined at line 109*

---

## WGPU_TEXTURE_STORAGE_BINDING:

```fides
pub const WGPU_TEXTURE_STORAGE_BINDING: i64 = 0x08;
```

*Defined at line 110*

---

## WGPU_TEXTURE_RENDER_ATTACHMENT:

```fides
pub const WGPU_TEXTURE_RENDER_ATTACHMENT: i64 = 0x10;
```

*Defined at line 111*

---

## WGPU_STAGE_VERTEX:

```fides
pub const WGPU_STAGE_VERTEX: i64 = 0x1;
```

*Defined at line 113*

---

## WGPU_STAGE_FRAGMENT:

```fides
pub const WGPU_STAGE_FRAGMENT: i64 = 0x2;
```

*Defined at line 114*

---

## WGPU_STAGE_COMPUTE:

```fides
pub const WGPU_STAGE_COMPUTE: i64 = 0x4;
```

*Defined at line 115*

---

## WGPU_MAP_READ:

```fides
pub const WGPU_MAP_READ: i64 = 0x1;
```

*Defined at line 117*

---

## WGPU_MAP_WRITE:

```fides
pub const WGPU_MAP_WRITE: i64 = 0x2;
```

*Defined at line 118*

---

## WGPU_COLOR_WRITE_RED:

```fides
pub const WGPU_COLOR_WRITE_RED: i64 = 0x1;
```

*Defined at line 120*

---

## WGPU_COLOR_WRITE_GREEN:

```fides
pub const WGPU_COLOR_WRITE_GREEN: i64 = 0x2;
```

*Defined at line 121*

---

## WGPU_COLOR_WRITE_BLUE:

```fides
pub const WGPU_COLOR_WRITE_BLUE: i64 = 0x4;
```

*Defined at line 122*

---

## WGPU_COLOR_WRITE_ALPHA:

```fides
pub const WGPU_COLOR_WRITE_ALPHA: i64 = 0x8;
```

*Defined at line 123*

---

## WGPU_COLOR_WRITE_ALL:

```fides
pub const WGPU_COLOR_WRITE_ALL: i64 = 0xF;
```

*Defined at line 124*

---

## wgpu_buffer_usage_str

Usage flags as the spec spells them, joined by '|'.

```fides
pub fn wgpu_buffer_usage_str(u: i64) -> str {
```

**Parameters:**

- `u`: `i64`

**Returns:** `str`

*Defined at line 127*

---

## wgpu_validate_buffer

Problems of a GPUBufferDescriptor (createBuffer validation).

```fides
pub fn wgpu_validate_buffer(size: i64, usage: i64, mapped_at_creation: bool) -> Vec<str> {
```

**Parameters:**

- `size`: `i64`
- `usage`: `i64`
- `mapped_at_creation`: `bool`

**Returns:** `Vec<str>`

*Defined at line 154*

---

## WgpuVertexFormat

```fides
pub struct WgpuVertexFormat {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `size` | `i64` |
| `components` | `i64` |
| `wgsl` | `str` |
| `known` | `bool` |

*Defined at line 179*

---

## wgpu_vertex_format

A GPUVertexFormat by its spec name ("float32x3", "unorm8x4", ...).

```fides
pub fn wgpu_vertex_format(name: str) -> WgpuVertexFormat {
```

**Parameters:**

- `name`: `str`

**Returns:** `WgpuVertexFormat`

*Defined at line 189*

---

## WgpuVertexLayout

```fides
pub struct WgpuVertexLayout {
```

**Fields:**

| Name | Type |
|------|------|
| `locations` | `Vec<i64>` |
| `formats` | `Vec<str>` |
| `names` | `Vec<str>` |
| `offsets` | `Vec<i64>` |
| `stride` | `i64` |
| `errors` | `Vec<str>` |

*Defined at line 260*

---

## wgpu_vertex_layout_new

```fides
pub fn wgpu_vertex_layout_new() -> WgpuVertexLayout {
```

**Returns:** `WgpuVertexLayout`

*Defined at line 269*

---

## wgpu_vertex_add

Append an attribute at the next offset aligned to min(4, size).

```fides
pub fn wgpu_vertex_add(l: &mut WgpuVertexLayout, location: i64, format: str, name: str) {
```

**Parameters:**

- `l`: `&mut WgpuVertexLayout`
- `location`: `i64`
- `format`: `str`
- `name`: `str`

*Defined at line 276*

---

## wgpu_vertex_wgsl

The WGSL struct a vertex shader takes this buffer as.

```fides
pub fn wgpu_vertex_wgsl(l: &WgpuVertexLayout, struct_name: str) -> str {
```

**Parameters:**

- `l`: `&WgpuVertexLayout`
- `struct_name`: `str`

**Returns:** `str`

*Defined at line 316*

---

## WgpuTextureFormat

```fides
pub struct WgpuTextureFormat {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `known` | `bool` |
| `block_bytes` | `i64` |
| `block_w` | `i64` |
| `block_h` | `i64` |
| `sample_type` | `str` |
| `renderable` | `bool` |
| `blendable` | `bool` |
| `storage` | `bool` |
| `has_depth` | `bool` |
| `has_stencil` | `bool` |
| `srgb` | `bool` |

*Defined at line 328*

---

## wgpu_texture_format

The spec's texel format table entry of a core format name.

```fides
pub fn wgpu_texture_format(name: str) -> WgpuTextureFormat {
```

**Parameters:**

- `name`: `str`

**Returns:** `WgpuTextureFormat`

*Defined at line 352*

---

## wgpu_bytes_per_row

bytesPerRow of a copy: blocks per row times block bytes, aligned up to
256 for buffer-texture copies (copyBufferToTexture's requirement).

```fides
pub fn wgpu_bytes_per_row(format: str, width: i64) -> i64 {
```

**Parameters:**

- `format`: `str`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 480*

---

## WgpuBglEntry

```fides
pub struct WgpuBglEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `binding` | `i64` |
| `visibility` | `i64` |
| `kind` | `str` |
| `sub` | `str` |
| `extra` | `str` |
| `multisampled` | `bool` |
| `dynamic` | `bool` |
| `var_name` | `str` |
| `wgsl_type` | `str` |

*Defined at line 488*

---

## WgpuBglBuilder

```fides
pub struct WgpuBglBuilder {
```

**Fields:**

| Name | Type |
|------|------|
| `group` | `i64` |
| `entries` | `Vec<WgpuBglEntry>` |

*Defined at line 505*

---

## wgpu_bgl_new

```fides
pub fn wgpu_bgl_new(group: i64) -> WgpuBglBuilder {
```

**Parameters:**

- `group`: `i64`

**Returns:** `WgpuBglBuilder`

*Defined at line 510*

---

## wgpu_bgl_buffer

A buffer binding: kind "uniform", "storage" or "read-only-storage";
`ty` is the WGSL type of the variable.

```fides
pub fn wgpu_bgl_buffer(b: &mut WgpuBglBuilder, binding: i64, vis: i64, kind: str, name: str, ty: str) {
```

**Parameters:**

- `b`: `&mut WgpuBglBuilder`
- `binding`: `i64`
- `vis`: `i64`
- `kind`: `str`
- `name`: `str`
- `ty`: `str`

*Defined at line 524*

---

## wgpu_bgl_dynamic_buffer

```fides
pub fn wgpu_bgl_dynamic_buffer(b: &mut WgpuBglBuilder, binding: i64, vis: i64, kind: str, name: str, ty: str) {
```

**Parameters:**

- `b`: `&mut WgpuBglBuilder`
- `binding`: `i64`
- `vis`: `i64`
- `kind`: `str`
- `name`: `str`
- `ty`: `str`

*Defined at line 528*

---

## wgpu_bgl_sampler

```fides
pub fn wgpu_bgl_sampler(b: &mut WgpuBglBuilder, binding: i64, vis: i64, kind: str, name: str) {
```

**Parameters:**

- `b`: `&mut WgpuBglBuilder`
- `binding`: `i64`
- `vis`: `i64`
- `kind`: `str`
- `name`: `str`

*Defined at line 532*

---

## wgpu_bgl_texture

```fides
pub fn wgpu_bgl_texture(b: &mut WgpuBglBuilder, binding: i64, vis: i64, sample_type: str, view: str, multisampled: bool, name: str) {
```

**Parameters:**

- `b`: `&mut WgpuBglBuilder`
- `binding`: `i64`
- `vis`: `i64`
- `sample_type`: `str`
- `view`: `str`
- `multisampled`: `bool`
- `name`: `str`

*Defined at line 536*

---

## wgpu_bgl_storage_texture

```fides
pub fn wgpu_bgl_storage_texture(b: &mut WgpuBglBuilder, binding: i64, vis: i64, access: str, format: str, name: str) {
```

**Parameters:**

- `b`: `&mut WgpuBglBuilder`
- `binding`: `i64`
- `vis`: `i64`
- `access`: `str`
- `format`: `str`
- `name`: `str`

*Defined at line 540*

---

## wgpu_bgl_validate

Problems with the entries (GPUDevice.createBindGroupLayout validation
with the default limits).

```fides
pub fn wgpu_bgl_validate(b: &WgpuBglBuilder) -> Vec<str> {
```

**Parameters:**

- `b`: `&WgpuBglBuilder`

**Returns:** `Vec<str>`

*Defined at line 546*

---

## wgpu_stage_name

```fides
pub fn wgpu_stage_name(bit: i64) -> str {
```

**Parameters:**

- `bit`: `i64`

**Returns:** `str`

*Defined at line 669*

---

## wgpu_bgl_wgsl

The WGSL declarations of the layout's bindings, in binding order.

```fides
pub fn wgpu_bgl_wgsl(b: &WgpuBglBuilder) -> str {
```

**Parameters:**

- `b`: `&WgpuBglBuilder`

**Returns:** `str`

*Defined at line 705*

---

## WgslLayout

```fides
pub struct WgslLayout {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `align` | `i64` |
| `size` | `i64` |
| `offsets` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 755*

---

## WgslTypes

A registry of WGSL struct types: name -> members "name: type" list.

```fides
pub struct WgslTypes {
```

**Fields:**

| Name | Type |
|------|------|
| `names` | `Vec<str>` |
| `members` | `Vec<Vec<str>>` |
| `types` | `Vec<Vec<str>>` |

*Defined at line 765*

---

## wgsl_types_new

```fides
pub fn wgsl_types_new() -> WgslTypes {
```

**Returns:** `WgslTypes`

*Defined at line 771*

---

## wgsl_struct

Declare `struct name { m0: t0, m1: t1, ... }` from "m0: t0, m1: t1".

```fides
pub fn wgsl_struct(t: &mut WgslTypes, name: str, body: str) {
```

**Parameters:**

- `t`: `&mut WgslTypes`
- `name`: `str`
- `body`: `str`

*Defined at line 778*

---

## wgsl_layout

Alignment and size of a WGSL type. `uniform` applies the uniform
address space rules (RequiredAlignOf rounded to 16 for struct and
array members, array element stride a multiple of 16).

```fides
pub fn wgsl_layout(t: &WgslTypes, ty_in: str, uniform: bool) -> WgslLayout {
```

**Parameters:**

- `t`: `&WgslTypes`
- `ty_in`: `str`
- `uniform`: `bool`

**Returns:** `WgslLayout`

*Defined at line 851*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
