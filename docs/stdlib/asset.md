[All modules](index.md)

# asset -- Fides Documentation

## Contents

- [ASSET_TEXTURE:](#ASSET_TEXTURE:) (const)
- [ASSET_SOUND:](#ASSET_SOUND:) (const)
- [ASSET_FONT:](#ASSET_FONT:) (const)
- [ASSET_MESH:](#ASSET_MESH:) (const)
- [ASSET_TEXT:](#ASSET_TEXT:) (const)
- [ASSET_DATA:](#ASSET_DATA:) (const)
- [AssetEntry](#AssetEntry) (struct)
- [AssetManager](#AssetManager) (struct)
- [asset_init](#asset_init) (function)
- [asset_set_root](#asset_set_root) (function)
- [asset_set_auto_reload](#asset_set_auto_reload) (function)
- [asset_find](#asset_find) (function)
- [asset_load_texture](#asset_load_texture) (function)
- [asset_load_sound](#asset_load_sound) (function)
- [asset_load_font](#asset_load_font) (function)
- [asset_load_mesh](#asset_load_mesh) (function)
- [asset_load_image](#asset_load_image) (function)
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

*Defined at line 31*

---

## ASSET_SOUND:

```fides
pub const ASSET_SOUND: i64 = 1;
```

*Defined at line 32*

---

## ASSET_FONT:

```fides
pub const ASSET_FONT: i64 = 2;
```

*Defined at line 33*

---

## ASSET_MESH:

```fides
pub const ASSET_MESH: i64 = 3;
```

*Defined at line 34*

---

## ASSET_TEXT:

```fides
pub const ASSET_TEXT: i64 = 4;
```

*Defined at line 35*

---

## ASSET_DATA:

```fides
pub const ASSET_DATA: i64 = 5;
```

*Defined at line 36*

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
| `handle` | `i64` |
| `ref_count` | `i64` |
| `loaded` | `bool` |
| `last_modified` | `i64` |
| `size` | `i64,  // size in bytes` |

*Defined at line 39*

---

## AssetManager

```fides
pub struct AssetManager {
```

**Fields:**

| Name | Type |
|------|------|
| `assets` | `Vec<AssetEntry>` |
| `root_path` | `str` |
| `auto_reload` | `bool` |
| `total_loaded` | `i64` |
| `total_memory` | `i64,  // estimated memory usage in bytes` |

*Defined at line 51*

---

## asset_init

Initialize the asset manager.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
use file;
val p = "fid_doctest_asset.tmp";
file_write(p, "hello\n");
asset_init();
assert_eq(asset_count(), 0);
assert_eq(asset_load_text("fid_doctest_asset_missing.tmp"), -1);
val a = asset_load_text(p);
assert(a >= 0);
assert_eq(asset_type(a), ASSET_TEXT);
assert_eq(asset_path(a), p);
assert_eq(asset_load_text(p), a);
assert_eq(asset_count(), 1);
file_delete(p);
```

```fides
pub fn asset_init() -> void {
```

**Returns:** `void`

*Defined at line 80*

---

## asset_set_root

Set the root path for asset loading.

```fides
pub fn asset_set_root(path: str) -> void {
```

**Parameters:**

- `path`: `str`

**Returns:** `void`

*Defined at line 87*

---

## asset_set_auto_reload

Enable or disable automatic hot-reload checking.

```fides
pub fn asset_set_auto_reload(enabled: bool) -> void {
```

**Parameters:**

- `enabled`: `bool`

**Returns:** `void`

*Defined at line 92*

---

## asset_find

Find an asset by path. Returns index or -1.

```fides
pub fn asset_find(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 97*

---

## asset_load_texture

Load a texture asset. Returns the asset index or -1 on failure.

```fides
pub fn asset_load_texture(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 109*

---

## asset_load_sound

Load a sound asset.

```fides
pub fn asset_load_sound(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 139*

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

*Defined at line 170*

---

## asset_load_mesh

Load a mesh asset (OBJ file).

```fides
pub fn asset_load_mesh(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 199*

---

## asset_load_image

Load and decode an image file (PNG or BMP) directly.
Unlike asset_load_texture (a native-handle seam that returns -1 without
the SDL runtime), this decodes in pure Fides and returns the pixels.
`width` is 0 on failure.

```fides
pub fn asset_load_image(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 231*

---

## asset_load_text

Load a text file asset (shaders, configs).

```fides
pub fn asset_load_text(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 236*

---

## asset_handle

Get the native handle for an asset.

```fides
pub fn asset_handle(idx: i64) -> i64 {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `i64`

*Defined at line 260*

---

## asset_type

Get the asset type.

```fides
pub fn asset_type(idx: i64) -> i64 {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `i64`

*Defined at line 268*

---

## asset_path

Get the asset path.

```fides
pub fn asset_path(idx: i64) -> str {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `str`

*Defined at line 276*

---

## asset_release

Release a reference to an asset. When ref_count reaches 0, it's unloaded.

```fides
pub fn asset_release(idx: i64) -> void {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `void`

*Defined at line 284*

---

## asset_unload

Force unload an asset regardless of ref count.

```fides
pub fn asset_unload(idx: i64) -> void {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `void`

*Defined at line 307*

---

## asset_reload

Reload an asset (for hot reload).

```fides
pub fn asset_reload(idx: i64) -> bool {
```

**Parameters:**

- `idx`: `i64`

**Returns:** `bool`

*Defined at line 328*

---

## asset_hot_reload

Check all assets for file modifications and reload changed ones.

```fides
pub fn asset_hot_reload() -> i64 {
```

**Returns:** `i64`

*Defined at line 380*

---

## asset_count

Get the total number of loaded assets.

```fides
pub fn asset_count() -> i64 {
```

**Returns:** `i64`

*Defined at line 405*

---

## asset_memory_usage

Get estimated memory usage in bytes.

```fides
pub fn asset_memory_usage() -> i64 {
```

**Returns:** `i64`

*Defined at line 410*

---

## asset_unload_all

Unload all assets.

```fides
pub fn asset_unload_all() -> void {
```

**Returns:** `void`

*Defined at line 415*

---

## asset_shutdown

Shutdown the asset manager.

```fides
pub fn asset_shutdown() -> void {
```

**Returns:** `void`

*Defined at line 425*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
