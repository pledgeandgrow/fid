# asset — Fides Documentation

## Contents

- [ASSET_TEXTURE:](#asset_texture:) (const)
- [ASSET_SOUND:](#asset_sound:) (const)
- [ASSET_FONT:](#asset_font:) (const)
- [ASSET_MESH:](#asset_mesh:) (const)
- [ASSET_TEXT:](#asset_text:) (const)
- [ASSET_DATA:](#asset_data:) (const)
- [AssetEntry](#assetentry) (struct)
- [AssetManager](#assetmanager) (struct)
- [asset_init](#asset_init) (function)
- [asset_set_root](#asset_set_root) (function)
- [asset_set_auto_reload](#asset_set_auto_reload) (function)
- [asset_find](#asset_find) (function)
- [asset_load_texture](#asset_load_texture) (function)
- [asset_load_sound](#asset_load_sound) (function)
- [asset_load_font](#asset_load_font) (function)
- [asset_load_mesh](#asset_load_mesh) (function)
- [asset_load_text](#asset_load_text) (function)
- [asset_handle](#asset_handle) (function)
- [asset_type](#asset_type) (function)
- [asset_path](#asset_path) (function)
- [asset_release](#asset_release) (function)
- [asset_unload](#asset_unload) (function)
- [asset_reload](#asset_reload) (function)
- [asset_hot_reload](#asset_hot_reload) (function)
- [asset_count](#asset_count) (function)
- [asset_memory_usage](#asset_memory_usage) (function)
- [asset_unload_all](#asset_unload_all) (function)
- [asset_shutdown](#asset_shutdown) (function)

## ASSET_TEXTURE:

```fides
pub const ASSET_TEXTURE: i64 = 0;
```

*Defined at line 32*

---

## ASSET_SOUND:

```fides
pub const ASSET_SOUND: i64 = 1;
```

*Defined at line 33*

---

## ASSET_FONT:

```fides
pub const ASSET_FONT: i64 = 2;
```

*Defined at line 34*

---

## ASSET_MESH:

```fides
pub const ASSET_MESH: i64 = 3;
```

*Defined at line 35*

---

## ASSET_TEXT:

```fides
pub const ASSET_TEXT: i64 = 4;
```

*Defined at line 36*

---

## ASSET_DATA:

```fides
pub const ASSET_DATA: i64 = 5;
```

*Defined at line 37*

---

## AssetEntry

```fides
pub struct AssetEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `asset_type` | `i64` |
| `handle` | `i64,          // native handle (texture, sound, etc.)` |
| `ref_count` | `i64` |
| `loaded` | `bool` |
| `last_modified` | `i64,   // file mtime (for hot reload)` |
| `size` | `i64,             // size in bytes` |

*Defined at line 41*

---

## AssetManager

```fides
pub struct AssetManager {
```

**Fields:**

| Name | Type |
|------|------|
| `assets` | `Vec<AssetEntry>` |
| `root_path` | `str,        // base path for relative paths` |
| `auto_reload` | `bool,     // check for file changes each frame` |
| `total_loaded` | `i64` |
| `total_memory` | `i64,     // estimated memory usage in bytes` |

*Defined at line 51*

---

## asset_init

Initialize the asset manager.

```fides
pub fn asset_init() -> void {
```

**Returns:** `void`

*Defined at line 68*

---

## asset_set_root

Set the root path for asset loading.

```fides
pub fn asset_set_root(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 75*

---

## asset_set_auto_reload

Enable or disable automatic hot-reload checking.

```fides
pub fn asset_set_auto_reload(enabled: bool) -> void {
```

**Parameters:**

- `enabled`: `bool`

**Returns:** `void`

*Defined at line 80*

---

## asset_find

Find an asset by path. Returns index or -1.

```fides
pub fn asset_find(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 85*

---

## asset_load_texture

Load a texture asset. Returns the asset index or -1 on failure.

```fides
pub fn asset_load_texture(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 97*

---

## asset_load_sound

Load a sound asset.

```fides
pub fn asset_load_sound(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 125*

---

## asset_load_font

Load a font asset.

```fides
pub fn asset_load_font(path: str, size: f64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `size`: `f64`

**Returns:** `i64`

*Defined at line 152*

---

## asset_load_mesh

Load a mesh asset (OBJ file).

```fides
pub fn asset_load_mesh(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 179*

---

## asset_load_text

Load a text file asset (shaders, configs).

```fides
pub fn asset_load_text(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 206*

---

## asset_handle

Get the native handle for an asset.

```fides
pub fn asset_handle(idx: i64) -> i64 {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `i64`

*Defined at line 234*

---

## asset_type

Get the asset type.

```fides
pub fn asset_type(idx: i64) -> i64 {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `i64`

*Defined at line 240*

---

## asset_path

Get the asset path.

```fides
pub fn asset_path(idx: i64) -> str {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `str`

*Defined at line 246*

---

## asset_release

Release a reference to an asset. When ref_count reaches 0, it's unloaded.

```fides
pub fn asset_release(idx: i64) -> void {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `void`

*Defined at line 252*

---

## asset_unload

Force unload an asset regardless of ref count.

```fides
pub fn asset_unload(idx: i64) -> void {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `void`

*Defined at line 267*

---

## asset_reload

Reload an asset (for hot reload).

```fides
pub fn asset_reload(idx: i64) -> bool {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `bool`

*Defined at line 280*

---

## asset_hot_reload

Check all assets for file modifications and reload changed ones.

```fides
pub fn asset_hot_reload() -> i64 {
```

**Returns:** `i64`

*Defined at line 311*

---

## asset_count

Get the total number of loaded assets.

```fides
pub fn asset_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 330*

---

## asset_memory_usage

Get estimated memory usage in bytes.

```fides
pub fn asset_memory_usage() -> i64 {
```

**Returns:** `i64`

*Defined at line 335*

---

## asset_unload_all

Unload all assets.

```fides
pub fn asset_unload_all() -> void {
```

**Returns:** `void`

*Defined at line 340*

---

## asset_shutdown

Shutdown the asset manager.

```fides
pub fn asset_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 350*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
