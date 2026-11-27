[All modules](index.md)

# image -- Fides Documentation

## Contents

- [Image](#Image) (struct)
- [image_create](#image_create) (function)
- [image_get_pixel](#image_get_pixel) (function)
- [image_set_pixel](#image_set_pixel) (function)
- [image_fill](#image_fill) (function)
- [image_free](#image_free) (function)
- [image_png_decode](#image_png_decode) (function)
- [image_bmp_decode](#image_bmp_decode) (function)
- [image_load_png](#image_load_png) (function)
- [image_load_bmp](#image_load_bmp) (function)
- [image_load_jpg](#image_load_jpg) (function)
- [image_load](#image_load) (function)
- [image_png_encode](#image_png_encode) (function)
- [image_save_png](#image_save_png) (function)
- [image_save_bmp](#image_save_bmp) (function)
- [image_to_texture](#image_to_texture) (function)
- [image_flip_h](#image_flip_h) (function)
- [image_flip_v](#image_flip_v) (function)
- [image_subregion](#image_subregion) (function)
- [image_resize_nn](#image_resize_nn) (function)
- [Color](#Color) (struct)
- [color_pack](#color_pack) (function)
- [color_unpack](#color_unpack) (function)
- [COLOR_WHITE:](#COLOR_WHITE:) (const)
- [COLOR_BLACK:](#COLOR_BLACK:) (const)
- [COLOR_RED:](#COLOR_RED:) (const)
- [COLOR_GREEN:](#COLOR_GREEN:) (const)
- [COLOR_BLUE:](#COLOR_BLUE:) (const)
- [COLOR_YELLOW:](#COLOR_YELLOW:) (const)
- [COLOR_CYAN:](#COLOR_CYAN:) (const)
- [COLOR_MAGENTA:](#COLOR_MAGENTA:) (const)
- [COLOR_TRANSPARENT:](#COLOR_TRANSPARENT:) (const)
- [COLOR_GRAY:](#COLOR_GRAY:) (const)
- [COLOR_DARK_GRAY:](#COLOR_DARK_GRAY:) (const)
- [COLOR_LIGHT_GRAY:](#COLOR_LIGHT_GRAY:) (const)

## Image

```fides
pub struct Image {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `channels` | `i64` |
| `data` | `Vec<i64>,  // pixel bytes (row-major, top-to-bottom)` |

*Defined at line 26*

---

## image_create

Create a blank image (all black, fully opaque if RGBA).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var img = image_create(3, 2, 4);
assert_eq(img.data.len(), 24);
assert_eq(image_get_pixel(&img, 0, 0), 0x000000FF);
image_set_pixel(&mut img, 2, 1, 255, 128, 0, 255);
assert_eq(image_get_pixel(&img, 2, 1), 0xFF8000FF);
image_flip_h(&mut img);
assert_eq(image_get_pixel(&img, 0, 1), 0xFF8000FF);
val sub = image_subregion(&img, 0, 1, 2, 1);
assert_eq(sub.width, 2);
assert_eq(image_get_pixel(&sub, 0, 0), 0xFF8000FF);
```

```fides
pub fn image_create(width: i64, height: i64, channels: i64) -> Image {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `channels`: `i64`

**Returns:** `Image`

*Defined at line 47*

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

*Defined at line 75*

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

*Defined at line 91*

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

*Defined at line 105*

---

## image_free

Free image data (the Vec will be dropped automatically, but this clears it).

```fides
pub fn image_free(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 118*

---

## image_png_decode

Decode a PNG byte stream (8-bit depths, colour types 0/2/3/4/6, no
interlace) into a 4-channel Image. `width` 0 on failure.

```fides
pub fn image_png_decode(d: Vec<i64>) -> Image {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `Image`

*Defined at line 175*

---

## image_bmp_decode

Decode a BMP byte stream (uncompressed 24- or 32-bit BI_RGB) into a
4-channel Image. `width` 0 on failure.

```fides
pub fn image_bmp_decode(d: Vec<i64>) -> Image {
```

**Parameters:**

- `d`: `Vec<i64>`

**Returns:** `Image`

*Defined at line 361*

---

## image_load_png

Load a PNG image file (pure Fides decode: 8-bit depths, colour types
0/2/3/4/6, no interlace).
Returns an Image with width=0 on failure.

```fides
pub fn image_load_png(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 488*

---

## image_load_bmp

Load a BMP image file (uncompressed 24- or 32-bit BI_RGB).
Returns an Image with width=0 on failure.

```fides
pub fn image_load_bmp(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 494*

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

*Defined at line 500*

---

## image_load

Load an image, auto-detecting format from file extension.

```fides
pub fn image_load(path: str) -> Image {
```

**Parameters:**

- `path`: `str`

**Returns:** `Image`

*Defined at line 505*

---

## image_png_encode

Encode an Image as PNG bytes (8-bit RGBA, no interlace, filter 0).

```fides
pub fn image_png_encode(img: &Image) -> Vec<i64> {
```

**Parameters:**

- `img`: `&Image`

**Returns:** `Vec<i64>`

*Defined at line 554*

---

## image_save_png

Save an Image as a PNG file. Returns file_write_bytes' status.

```fides
pub fn image_save_png(img: &Image, path: str) -> i64 {
```

**Parameters:**

- `img`: `&Image`
- `path`: `str`

**Returns:** `i64`

*Defined at line 596*

---

## image_save_bmp

Save an Image as a 24-bit BMP file (BI_RGB, bottom-up rows).
Returns file_write_bytes' status.

```fides
pub fn image_save_bmp(img: &Image, path: str) -> i64 {
```

**Parameters:**

- `img`: `&Image`
- `path`: `str`

**Returns:** `i64`

*Defined at line 602*

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

*Defined at line 654*

---

## image_flip_h

Flip an image horizontally (mirror left-right).

```fides
pub fn image_flip_h(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 668*

---

## image_flip_v

Flip an image vertically (mirror top-bottom).

```fides
pub fn image_flip_v(img: &mut Image) -> void {
```

**Parameters:**

- `img`: `&mut Image`

**Returns:** `void`

*Defined at line 690*

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

*Defined at line 711*

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

*Defined at line 735*

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

*Defined at line 759*

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

*Defined at line 767*

---

## color_unpack

Unpack a color i64 into RGBA components.

```fides
pub fn color_unpack(c: i64) -> Color {
```

**Parameters:**

- `c`: `i64`

**Returns:** `Color`

*Defined at line 772*

---

## COLOR_WHITE:

```fides
pub const COLOR_WHITE: i64 = 0xFFFFFFFF;
```

*Defined at line 783*

---

## COLOR_BLACK:

```fides
pub const COLOR_BLACK: i64 = 0x000000FF;
```

*Defined at line 784*

---

## COLOR_RED:

```fides
pub const COLOR_RED: i64 = 0xFF0000FF;
```

*Defined at line 785*

---

## COLOR_GREEN:

```fides
pub const COLOR_GREEN: i64 = 0x00FF00FF;
```

*Defined at line 786*

---

## COLOR_BLUE:

```fides
pub const COLOR_BLUE: i64 = 0x0000FFFF;
```

*Defined at line 787*

---

## COLOR_YELLOW:

```fides
pub const COLOR_YELLOW: i64 = 0xFFFF00FF;
```

*Defined at line 788*

---

## COLOR_CYAN:

```fides
pub const COLOR_CYAN: i64 = 0x00FFFFFF;
```

*Defined at line 789*

---

## COLOR_MAGENTA:

```fides
pub const COLOR_MAGENTA: i64 = 0xFF00FFFF;
```

*Defined at line 790*

---

## COLOR_TRANSPARENT:

```fides
pub const COLOR_TRANSPARENT: i64 = 0x00000000;
```

*Defined at line 791*

---

## COLOR_GRAY:

```fides
pub const COLOR_GRAY: i64 = 0x808080FF;
```

*Defined at line 792*

---

## COLOR_DARK_GRAY:

```fides
pub const COLOR_DARK_GRAY: i64 = 0x404040FF;
```

*Defined at line 793*

---

## COLOR_LIGHT_GRAY:

```fides
pub const COLOR_LIGHT_GRAY: i64 = 0xC0C0C0FF;
```

*Defined at line 794*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
