[All modules](index.md)

# gltf -- Fides Documentation

## Contents

- [GLB_MAGIC:](#GLB_MAGIC:) (const)
- [GLB_JSON:](#GLB_JSON:) (const)
- [GLB_BIN:](#GLB_BIN:) (const)
- [GltfDoc](#GltfDoc) (struct)
- [gltf_open](#gltf_open) (function)
- [gltf_open_json](#gltf_open_json) (function)
- [gltf_mesh_count](#gltf_mesh_count) (function)
- [gltf_node_count](#gltf_node_count) (function)
- [gltf_comp_size](#gltf_comp_size) (function)
- [gltf_type_count](#gltf_type_count) (function)
- [GltfAcc](#GltfAcc) (struct)
- [gltf_acc](#gltf_acc) (function)
- [gltf_acc_uint_at](#gltf_acc_uint_at) (function)
- [gltf_acc_int_at](#gltf_acc_int_at) (function)
- [gltf_acc_f32_at](#gltf_acc_f32_at) (function)
- [gltf_mesh](#gltf_mesh) (function)
- [gltf_material_color](#gltf_material_color) (function)

## GLB_MAGIC:

```fides
pub const GLB_MAGIC: i64 = 0x46546C67;  // "glTF" little-endian
```

*Defined at line 18*

---

## GLB_JSON:

```fides
pub const GLB_JSON: i64 = 0x4E4F534A;  // "JSON"
```

*Defined at line 19*

---

## GLB_BIN:

```fides
pub const GLB_BIN: i64 = 0x004E4942;  // "BIN\0"
```

*Defined at line 20*

---

## GltfDoc

Parsed asset: `json` is the JSON document's text and `doc` its index
(json.fd json_index: scanned once, every lookup below walks its nodes
instead of rescanning the text); `bin_off`/`bin_len` locate the BIN
chunk inside `b` for GLB sources (bin_len 0 for JSON-only .gltf —
caller supplies the .bin blob as `b` then).

```fides
pub struct GltfDoc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `json` | `str` |
| `doc` | `JsonDoc` |
| `bin_off` | `i64` |
| `bin_len` | `i64` |

*Defined at line 51*

---

## gltf_open

Parse a .glb blob: 12-byte header + length-prefixed chunks.
ok=false when magic/version/length don't check out.

```fides
pub fn gltf_open(b: Vec<i64>) -> GltfDoc {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `GltfDoc`

*Defined at line 61*

---

## gltf_open_json

Wrap a standalone .gltf JSON string (buffers external — pass the .bin
bytes as `b` to the accessor readers).


*Example (native only) -- run as a test by `scripts/doctest.py`:*

```fides
var d = gltf_open_json("{\"meshes\":[{\"name\":\"tri\"}]}");
println(int_to_str(gltf_mesh_count(d)));   // 1
```

```fides
pub fn gltf_open_json(j: str) -> GltfDoc {
```

**Parameters:**

- `j`: `str`

**Returns:** `GltfDoc`

*Defined at line 121*

---

## gltf_mesh_count

```fides
pub fn gltf_mesh_count(d: GltfDoc) -> i64 {
```

**Parameters:**

- `d`: `GltfDoc`

**Returns:** `i64`

*Defined at line 145*

---

## gltf_node_count

```fides
pub fn gltf_node_count(d: GltfDoc) -> i64 {
```

**Parameters:**

- `d`: `GltfDoc`

**Returns:** `i64`

*Defined at line 148*

---

## gltf_comp_size

componentType -> byte size, or 0 when unknown.

```fides
pub fn gltf_comp_size(ct: i64) -> i64 {
```

**Parameters:**

- `ct`: `i64`

**Returns:** `i64`

*Defined at line 155*

---

## gltf_type_count

type string -> number of components per element.

```fides
pub fn gltf_type_count(t: str) -> i64 {
```

**Parameters:**

- `t`: `str`

**Returns:** `i64`

*Defined at line 169*

---

## GltfAcc

Resolved accessor — the JSON lookups happen once in gltf_acc; the
per-element readers below then touch only the binary buffer (a real
model's POSITION accessor would otherwise re-parse JSON per vertex).

```fides
pub struct GltfAcc {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `off` | `i64` |
| `stride` | `i64` |
| `count` | `i64` |
| `ctype` | `i64` |
| `csize` | `i64` |
| `ncomp` | `i64` |
| `norm` | `bool,  // normalized flag` |

*Defined at line 197*

---

## gltf_acc

Resolve accessor `ai`: bufferView + byteOffset + byteStride. `base`
is the buffer's offset inside `b` (d.bin_off for GLB, 0 for .bin).

```fides
pub fn gltf_acc(d: GltfDoc, ai: i64, base: i64) -> GltfAcc {
```

**Parameters:**

- `d`: `GltfDoc`
- `ai`: `i64`
- `base`: `i64`

**Returns:** `GltfAcc`

*Defined at line 215*

---

## gltf_acc_uint_at

Unsigned component j of element i — BYTE/UNSIGNED_*/unsigned ints.

```fides
pub fn gltf_acc_uint_at(b: Vec<i64>, a: GltfAcc, i: i64, j: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `a`: `GltfAcc`
- `i`: `i64`
- `j`: `i64`

**Returns:** `i64`

*Defined at line 249*

---

## gltf_acc_int_at

Signed variant (BYTE=5120, SHORT=5122).

```fides
pub fn gltf_acc_int_at(b: Vec<i64>, a: GltfAcc, i: i64, j: i64) -> i64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `a`: `GltfAcc`
- `i`: `i64`
- `j`: `i64`

**Returns:** `i64`

*Defined at line 270*

---

## gltf_acc_f32_at

Float component — FLOAT (5126) directly, integer types as raw value
or /max when the accessor is normalized.

```fides
pub fn gltf_acc_f32_at(b: Vec<i64>, a: GltfAcc, i: i64, j: i64) -> f64 {
```

**Parameters:**

- `b`: `Vec<i64>`
- `a`: `GltfAcc`
- `i`: `i64`
- `j`: `i64`

**Returns:** `f64`

*Defined at line 283*

---

## gltf_mesh

Build mesh.fd's Mesh from mesh `mi`, primitive `pi` (default 0).
Reads POSITION/NORMAL/TEXCOORD_0 attributes and the index accessor.
Unindexed primitives assume consecutive triangles. valid=false when
the mesh/primitive or its POSITION accessor is missing.

```fides
pub fn gltf_mesh(b: Vec<i64>, d: GltfDoc, mi: i64, pi: i64) -> Mesh {
```

**Parameters:**

- `b`: `Vec<i64>`
- `d`: `GltfDoc`
- `mi`: `i64`
- `pi`: `i64`

**Returns:** `Mesh`

*Defined at line 324*

---

## gltf_material_color

baseColorFactor of material `mi` as [r,g,b,a] (defaults 1,1,1,1).

```fides
pub fn gltf_material_color(d: GltfDoc, mi: i64) -> Vec<f64> {
```

**Parameters:**

- `d`: `GltfDoc`
- `mi`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 422*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
