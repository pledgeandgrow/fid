# image — Fides Documentation

## Contents

- [Image](#image) (struct)
- [image_create](#image_create) (function)
- [image_get_pixel](#image_get_pixel) (function)
- [image_set_pixel](#image_set_pixel) (function)
- [image_fill](#image_fill) (function)
- [image_free](#image_free) (function)
- [image_load_png](#image_load_png) (function)
- [image_load_jpg](#image_load_jpg) (function)
- [image_load](#image_load) (function)
- [image_to_texture](#image_to_texture) (function)
- [image_flip_h](#image_flip_h) (function)
- [image_flip_v](#image_flip_v) (function)
- [image_subregion](#image_subregion) (function)
- [image_resize_nn](#image_resize_nn) (function)
- [Color](#color) (struct)
- [color_pack](#color_pack) (function)
- [color_unpack](#color_unpack) (function)
- [COLOR_WHITE:](#color_white:) (const)
- [COLOR_BLACK:](#color_black:) (const)
- [COLOR_RED:](#color_red:) (const)
- [COLOR_GREEN:](#color_green:) (const)
- [COLOR_BLUE:](#color_blue:) (const)
- [COLOR_YELLOW:](#color_yellow:) (const)
- [COLOR_CYAN:](#color_cyan:) (const)
- [COLOR_MAGENTA:](#color_magenta:) (const)
- [COLOR_TRANSPARENT:](#color_transparent:) (const)
- [COLOR_GRAY:](#color_gray:) (const)
- [COLOR_DARK_GRAY:](#color_dark_gray:) (const)
- [COLOR_LIGHT_GRAY:](#color_light_gray:) (const)

## Image

```fides
pub struct Image {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `channels` | `i64,       // 3 = RGB, 4 = RGBA` |
| `data` | `Vec<i64>,      // pixel bytes (row-major, top-to-bottom)` |

*Defined at line 25*

---

## image_create

Create a blank image (all black, fully opaque if RGBA).

```fides
pub fn image_create(width: i64, height: i64, channels: i64) -> Image {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `channels`: `i64`

**Returns:** `Image`

*Defined at line 33*

---

## image_get_pixel

Get a pixel at (x, y). Returns RGBA as a packed i64: 0xRRGGBBAA.

```fides
pub fn image_get_pixel(img: &Image, x: i64, y: i64) -> i64 {
```

**Parameters:**

- `img`: `&Image`
- `x`: `i64`
- `y`: `i64`

**Returns:** `i64`

*Defined at line 60*

---

## image_set_pixel

Set a pixel at (x, y) from RGBA components.

```fides
pub fn image_set_pixel(img: &mut Image, x: i64, y: i64, r: i64, g: i64, b: i64, a: i64) -> void {
```

**Parameters:**

- `img`: `&mut Image`
- `x`: `i64`
- `y`: `i64`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`
- `a`: `i64`

**Returns:** `void`

*Defined at line 76*

---

## image_fill

Fill the entire image with a solid color.

```fides
pub fn image_fill(img: &mut Image, r: i64, g: i64, b: i64, a: i64) -> void {
```

**Parameters:**

- `img`: `&mut Image`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`
- `a`: `i64`

**Returns:** `void`

*Defined at line 90*

---

## image_free

Free image data (the Vec will be dropped automatically, but this clears it).

```fides
pub fn image_free(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 103*

---

## image_load_png

Load a PNG image file.
Returns an Image with width=0 on failure.

```fides
pub fn image_load_png(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 127*

---

## image_load_jpg

Load a JPEG image file.
Returns an Image with width=0 on failure.

```fides
pub fn image_load_jpg(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 138*

---

## image_load

Load an image, auto-detecting format from file extension.

```fides
pub fn image_load(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 148*

---

## image_to_texture

Convert an Image to an SDL texture for rendering.
Returns texture handle or 0 on failure.

```fides
pub fn image_to_texture(ren: i64, img: &Image) -> i64 {
```

**Parameters:**

- `ren`: `i64`
- `img`: `&Image`

**Returns:** `i64`

*Defined at line 172*

---

## image_flip_h

Flip an image horizontally (mirror left-right).

```fides
pub fn image_flip_h(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 182*

---

## image_flip_v

Flip an image vertically (mirror top-bottom).

```fides
pub fn image_flip_v(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 204*

---

## image_subregion

Extract a sub-region of an image as a new Image.

```fides
pub fn image_subregion(img: &Image, x: i64, y: i64, w: i64, h: i64) -> Image {
```

**Parameters:**

- `img`: `&Image`
- `x`: `i64`
- `y`: `i64`
- `w`: `i64`
- `h`: `i64`

**Returns:** `Image`

*Defined at line 225*

---

## image_resize_nn

Resize an image using nearest-neighbor sampling.

```fides
pub fn image_resize_nn(img: &Image, new_w: i64, new_h: i64) -> Image {
```

**Parameters:**

- `img`: `&Image`
- `new_w`: `i64`
- `new_h`: `i64`

**Returns:** `Image`

*Defined at line 249*

---

## Color

```fides
pub struct Color {
```

**Fields:**

| Name | Type |
|------|------|
| `r` | `i64` |
| `g` | `i64` |
| `b` | `i64` |
| `a` | `i64` |

*Defined at line 273*

---

## color_pack

Pack RGBA components into a single i64: 0xRRGGBBAA.

```fides
pub fn color_pack(r: i64, g: i64, b: i64, a: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`
- `g`: `i64`
- `b`: `i64`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 281*

---

## color_unpack

Unpack a color i64 into RGBA components.

```fides
pub fn color_unpack(c: i64) -> Color {
```

**Parameters:**

- `c`: `i64`

**Returns:** `Color`

*Defined at line 286*

---

## COLOR_WHITE:

```fides
pub const COLOR_WHITE: i64 = 0xFFFFFFFF;
```

*Defined at line 295*

---

## COLOR_BLACK:

```fides
pub const COLOR_BLACK: i64 = 0x000000FF;
```

*Defined at line 296*

---

## COLOR_RED:

```fides
pub const COLOR_RED: i64 = 0xFF0000FF;
```

*Defined at line 297*

---

## COLOR_GREEN:

```fides
pub const COLOR_GREEN: i64 = 0x00FF00FF;
```

*Defined at line 298*

---

## COLOR_BLUE:

```fides
pub const COLOR_BLUE: i64 = 0x0000FFFF;
```

*Defined at line 299*

---

## COLOR_YELLOW:

```fides
pub const COLOR_YELLOW: i64 = 0xFFFF00FF;
```

*Defined at line 300*

---

## COLOR_CYAN:

```fides
pub const COLOR_CYAN: i64 = 0x00FFFFFF;
```

*Defined at line 301*

---

## COLOR_MAGENTA:

```fides
pub const COLOR_MAGENTA: i64 = 0xFF00FFFF;
```

*Defined at line 302*

---

## COLOR_TRANSPARENT:

```fides
pub const COLOR_TRANSPARENT: i64 = 0x00000000;
```

*Defined at line 303*

---

## COLOR_GRAY:

```fides
pub const COLOR_GRAY: i64 = 0x808080FF;
```

*Defined at line 304*

---

## COLOR_DARK_GRAY:

```fides
pub const COLOR_DARK_GRAY: i64 = 0x404040FF;
```

*Defined at line 305*

---

## COLOR_LIGHT_GRAY:

```fides
pub const COLOR_LIGHT_GRAY: i64 = 0xC0C0C0FF;
```

*Defined at line 306*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
