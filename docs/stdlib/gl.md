# gl — Fides Documentation

## Contents

- [GL_DEPTH_TEST:](#gl_depth_test:) (const)
- [GL_CULL_FACE:](#gl_cull_face:) (const)
- [GL_BLEND:](#gl_blend:) (const)
- [GL_TEXTURE_2D:](#gl_texture_2d:) (const)
- [GL_TEXTURE0:](#gl_texture0:) (const)
- [GL_TRIANGLES:](#gl_triangles:) (const)
- [GL_TRIANGLE_STRIP:](#gl_triangle_strip:) (const)
- [GL_TRIANGLE_FAN:](#gl_triangle_fan:) (const)
- [GL_LINES:](#gl_lines:) (const)
- [GL_LINE_STRIP:](#gl_line_strip:) (const)
- [GL_POINTS:](#gl_points:) (const)
- [GL_COLOR_BUFFER_BIT:](#gl_color_buffer_bit:) (const)
- [GL_DEPTH_BUFFER_BIT:](#gl_depth_buffer_bit:) (const)
- [GL_ARRAY_BUFFER:](#gl_array_buffer:) (const)
- [GL_ELEMENT_ARRAY_BUFFER:](#gl_element_array_buffer:) (const)
- [GL_STATIC_DRAW:](#gl_static_draw:) (const)
- [GL_DYNAMIC_DRAW:](#gl_dynamic_draw:) (const)
- [GL_STREAM_DRAW:](#gl_stream_draw:) (const)
- [GL_FLOAT:](#gl_float:) (const)
- [GL_UNSIGNED_INT:](#gl_unsigned_int:) (const)
- [GL_UNSIGNED_BYTE:](#gl_unsigned_byte:) (const)
- [GL_RGB:](#gl_rgb:) (const)
- [GL_RGBA:](#gl_rgba:) (const)
- [GL_TEXTURE_MIN_FILTER:](#gl_texture_min_filter:) (const)
- [GL_TEXTURE_MAG_FILTER:](#gl_texture_mag_filter:) (const)
- [GL_TEXTURE_WRAP_S:](#gl_texture_wrap_s:) (const)
- [GL_TEXTURE_WRAP_T:](#gl_texture_wrap_t:) (const)
- [GL_LINEAR:](#gl_linear:) (const)
- [GL_NEAREST:](#gl_nearest:) (const)
- [GL_CLAMP_TO_EDGE:](#gl_clamp_to_edge:) (const)
- [GL_REPEAT:](#gl_repeat:) (const)
- [GL_NO_ERROR:](#gl_no_error:) (const)
- [GL_INVALID_ENUM:](#gl_invalid_enum:) (const)
- [GL_INVALID_VALUE:](#gl_invalid_value:) (const)
- [GL_INVALID_OPERATION:](#gl_invalid_operation:) (const)
- [GL_OUT_OF_MEMORY:](#gl_out_of_memory:) (const)
- [GL_FRONT:](#gl_front:) (const)
- [GL_BACK:](#gl_back:) (const)
- [GL_CW:](#gl_cw:) (const)
- [GL_CCW:](#gl_ccw:) (const)
- [GL_SRC_ALPHA:](#gl_src_alpha:) (const)
- [GL_ONE_MINUS_SRC_ALPHA:](#gl_one_minus_src_alpha:) (const)
- [gl_init](#gl_init) (function)
- [gl_clear_screen](#gl_clear_screen) (function)
- [gl_set_viewport](#gl_set_viewport) (function)
- [gl_check_errors](#gl_check_errors) (function)
- [gl_gen_buffer](#gl_gen_buffer) (function)
- [gl_gen_vertex_array](#gl_gen_vertex_array) (function)
- [gl_gen_texture](#gl_gen_texture) (function)
- [gl_delete_buffer](#gl_delete_buffer) (function)
- [gl_delete_vertex_array](#gl_delete_vertex_array) (function)
- [gl_delete_texture](#gl_delete_texture) (function)
- [gl_draw_triangles](#gl_draw_triangles) (function)
- [gl_draw_triangles_indexed](#gl_draw_triangles_indexed) (function)
- [gl_set_texture_defaults](#gl_set_texture_defaults) (function)
- [gl_enable_blend](#gl_enable_blend) (function)
- [gl_error_string](#gl_error_string) (function)
- [GL_VERTEX_SHADER:](#gl_vertex_shader:) (const)
- [GL_FRAGMENT_SHADER:](#gl_fragment_shader:) (const)
- [GL_COMPILE_STATUS:](#gl_compile_status:) (const)
- [GL_LINK_STATUS:](#gl_link_status:) (const)
- [gl_compile_shader_src](#gl_compile_shader_src) (function)
- [gl_create_program_from_src](#gl_create_program_from_src) (function)
- [gl_set_uniform_mat4](#gl_set_uniform_mat4) (function)
- [gl_set_uniform_f](#gl_set_uniform_f) (function)
- [gl_set_uniform_i](#gl_set_uniform_i) (function)
- [gl_get_uniform](#gl_get_uniform) (function)
- [gl_get_attrib](#gl_get_attrib) (function)
- [DEFAULT_VERT_SHADER:](#default_vert_shader:) (const)
- [DEFAULT_FRAG_SHADER:](#default_frag_shader:) (const)
- [gl_create_default_program](#gl_create_default_program) (function)

## GL_DEPTH_TEST:

```fides
pub const GL_DEPTH_TEST: i64 = 0x0B71;
```

*Defined at line 83*

---

## GL_CULL_FACE:

```fides
pub const GL_CULL_FACE: i64 = 0x0B44;
```

*Defined at line 84*

---

## GL_BLEND:

```fides
pub const GL_BLEND: i64 = 0x0BE2;
```

*Defined at line 85*

---

## GL_TEXTURE_2D:

```fides
pub const GL_TEXTURE_2D: i64 = 0x0DE1;
```

*Defined at line 86*

---

## GL_TEXTURE0:

```fides
pub const GL_TEXTURE0: i64 = 0x84C0;
```

*Defined at line 87*

---

## GL_TRIANGLES:

```fides
pub const GL_TRIANGLES: i64 = 0x0004;
```

*Defined at line 89*

---

## GL_TRIANGLE_STRIP:

```fides
pub const GL_TRIANGLE_STRIP: i64 = 0x0005;
```

*Defined at line 90*

---

## GL_TRIANGLE_FAN:

```fides
pub const GL_TRIANGLE_FAN: i64 = 0x0006;
```

*Defined at line 91*

---

## GL_LINES:

```fides
pub const GL_LINES: i64 = 0x0001;
```

*Defined at line 92*

---

## GL_LINE_STRIP:

```fides
pub const GL_LINE_STRIP: i64 = 0x0003;
```

*Defined at line 93*

---

## GL_POINTS:

```fides
pub const GL_POINTS: i64 = 0x0000;
```

*Defined at line 94*

---

## GL_COLOR_BUFFER_BIT:

```fides
pub const GL_COLOR_BUFFER_BIT: i64 = 0x4000;
```

*Defined at line 96*

---

## GL_DEPTH_BUFFER_BIT:

```fides
pub const GL_DEPTH_BUFFER_BIT: i64 = 0x100;
```

*Defined at line 97*

---

## GL_ARRAY_BUFFER:

```fides
pub const GL_ARRAY_BUFFER: i64 = 0x8892;
```

*Defined at line 99*

---

## GL_ELEMENT_ARRAY_BUFFER:

```fides
pub const GL_ELEMENT_ARRAY_BUFFER: i64 = 0x8893;
```

*Defined at line 100*

---

## GL_STATIC_DRAW:

```fides
pub const GL_STATIC_DRAW: i64 = 0x88E4;
```

*Defined at line 102*

---

## GL_DYNAMIC_DRAW:

```fides
pub const GL_DYNAMIC_DRAW: i64 = 0x88E8;
```

*Defined at line 103*

---

## GL_STREAM_DRAW:

```fides
pub const GL_STREAM_DRAW: i64 = 0x88E0;
```

*Defined at line 104*

---

## GL_FLOAT:

```fides
pub const GL_FLOAT: i64 = 0x1406;
```

*Defined at line 106*

---

## GL_UNSIGNED_INT:

```fides
pub const GL_UNSIGNED_INT: i64 = 0x1405;
```

*Defined at line 107*

---

## GL_UNSIGNED_BYTE:

```fides
pub const GL_UNSIGNED_BYTE: i64 = 0x1401;
```

*Defined at line 108*

---

## GL_RGB:

```fides
pub const GL_RGB: i64 = 0x1907;
```

*Defined at line 110*

---

## GL_RGBA:

```fides
pub const GL_RGBA: i64 = 0x1908;
```

*Defined at line 111*

---

## GL_TEXTURE_MIN_FILTER:

```fides
pub const GL_TEXTURE_MIN_FILTER: i64 = 0x2801;
```

*Defined at line 112*

---

## GL_TEXTURE_MAG_FILTER:

```fides
pub const GL_TEXTURE_MAG_FILTER: i64 = 0x2800;
```

*Defined at line 113*

---

## GL_TEXTURE_WRAP_S:

```fides
pub const GL_TEXTURE_WRAP_S: i64 = 0x2802;
```

*Defined at line 114*

---

## GL_TEXTURE_WRAP_T:

```fides
pub const GL_TEXTURE_WRAP_T: i64 = 0x2803;
```

*Defined at line 115*

---

## GL_LINEAR:

```fides
pub const GL_LINEAR: i64 = 0x2601;
```

*Defined at line 116*

---

## GL_NEAREST:

```fides
pub const GL_NEAREST: i64 = 0x2600;
```

*Defined at line 117*

---

## GL_CLAMP_TO_EDGE:

```fides
pub const GL_CLAMP_TO_EDGE: i64 = 0x812F;
```

*Defined at line 118*

---

## GL_REPEAT:

```fides
pub const GL_REPEAT: i64 = 0x2901;
```

*Defined at line 119*

---

## GL_NO_ERROR:

```fides
pub const GL_NO_ERROR: i64 = 0;
```

*Defined at line 121*

---

## GL_INVALID_ENUM:

```fides
pub const GL_INVALID_ENUM: i64 = 0x0500;
```

*Defined at line 122*

---

## GL_INVALID_VALUE:

```fides
pub const GL_INVALID_VALUE: i64 = 0x0501;
```

*Defined at line 123*

---

## GL_INVALID_OPERATION:

```fides
pub const GL_INVALID_OPERATION: i64 = 0x0502;
```

*Defined at line 124*

---

## GL_OUT_OF_MEMORY:

```fides
pub const GL_OUT_OF_MEMORY: i64 = 0x0505;
```

*Defined at line 125*

---

## GL_FRONT:

```fides
pub const GL_FRONT: i64 = 0x0404;
```

*Defined at line 127*

---

## GL_BACK:

```fides
pub const GL_BACK: i64 = 0x0405;
```

*Defined at line 128*

---

## GL_CW:

```fides
pub const GL_CW: i64 = 0x0900;
```

*Defined at line 129*

---

## GL_CCW:

```fides
pub const GL_CCW: i64 = 0x0901;
```

*Defined at line 130*

---

## GL_SRC_ALPHA:

```fides
pub const GL_SRC_ALPHA: i64 = 0x0302;
```

*Defined at line 132*

---

## GL_ONE_MINUS_SRC_ALPHA:

```fides
pub const GL_ONE_MINUS_SRC_ALPHA: i64 = 0x0303;
```

*Defined at line 133*

---

## gl_init

Initialize OpenGL state (enable depth testing, etc.)

```fides
pub fn gl_init() -> void {
```

**Returns:** `void`

*Defined at line 138*

---

## gl_clear_screen

Clear the screen.

```fides
pub fn gl_clear_screen() -> void {
```

**Returns:** `void`

*Defined at line 145*

---

## gl_set_viewport

Set the viewport.

```fides
pub fn gl_set_viewport(w: i64, h: i64) -> void {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`

**Returns:** `void`

*Defined at line 150*

---

## gl_check_errors

Check for OpenGL errors. Returns 0 if no error.

```fides
pub fn gl_check_errors() -> i64 {
```

**Returns:** `i64`

*Defined at line 155*

---

## gl_gen_buffer

Generate a single buffer object.

```fides
pub fn gl_gen_buffer() -> i64 {
```

**Returns:** `i64`

*Defined at line 160*

---

## gl_gen_vertex_array

Generate a single vertex array object.

```fides
pub fn gl_gen_vertex_array() -> i64 {
```

**Returns:** `i64`

*Defined at line 168*

---

## gl_gen_texture

Generate a single texture.

```fides
pub fn gl_gen_texture() -> i64 {
```

**Returns:** `i64`

*Defined at line 176*

---

## gl_delete_buffer

Delete a single buffer.

```fides
pub fn gl_delete_buffer(buf: i64) -> void {
```

**Parameters:**

- `buf`: `i64`

**Returns:** `void`

*Defined at line 184*

---

## gl_delete_vertex_array

Delete a single vertex array.

```fides
pub fn gl_delete_vertex_array(arr: i64) -> void {
```

**Parameters:**

- `arr`: `i64`

**Returns:** `void`

*Defined at line 191*

---

## gl_delete_texture

Delete a single texture.

```fides
pub fn gl_delete_texture(tex: i64) -> void {
```

**Parameters:**

- `tex`: `i64`

**Returns:** `void`

*Defined at line 198*

---

## gl_draw_triangles

Draw triangles from currently bound VBO data.

```fides
pub fn gl_draw_triangles(first: i64, count: i64) -> void {
```

**Parameters:**

- `first`: `i64`
- `count`: `i64`

**Returns:** `void`

*Defined at line 205*

---

## gl_draw_triangles_indexed

Draw triangles from index buffer.

```fides
pub fn gl_draw_triangles_indexed(count: i64) -> void {
```

**Parameters:**

- `count`: `i64`

**Returns:** `void`

*Defined at line 210*

---

## gl_set_texture_defaults

Set default texture parameters for 2D textures.

```fides
pub fn gl_set_texture_defaults() -> void {
```

**Returns:** `void`

*Defined at line 215*

---

## gl_enable_blend

Enable alpha blending for transparency.

```fides
pub fn gl_enable_blend() -> void {
```

**Returns:** `void`

*Defined at line 223*

---

## gl_error_string

Get error string from error code.

```fides
pub fn gl_error_string(err: i64) -> str {
```

**Parameters:**

- `err`: `i64`

**Returns:** `str`

*Defined at line 229*

---

## GL_VERTEX_SHADER:

```fides
pub const GL_VERTEX_SHADER: i64 = 0x8B31;
```

*Defined at line 273*

---

## GL_FRAGMENT_SHADER:

```fides
pub const GL_FRAGMENT_SHADER: i64 = 0x8B30;
```

*Defined at line 274*

---

## GL_COMPILE_STATUS:

```fides
pub const GL_COMPILE_STATUS: i64 = 0x8B81;
```

*Defined at line 277*

---

## GL_LINK_STATUS:

```fides
pub const GL_LINK_STATUS: i64 = 0x8B82;
```

*Defined at line 278*

---

## gl_compile_shader_src

Compile a shader from source code. Returns shader ID or 0 on failure.

```fides
pub fn gl_compile_shader_src(shader_type: i64, source: str) -> i64 {
```

**Parameters:**

- `shader_type`: `i64`
- `source`: `str`

**Returns:** `i64`

*Defined at line 281*

---

## gl_create_program_from_src

Create a shader program from vertex and fragment source.
Returns program ID or 0 on failure.

```fides
pub fn gl_create_program_from_src(vert_src: str, frag_src: str) -> i64 {
```

**Parameters:**

- `vert_src`: `str`
- `frag_src`: `str`

**Returns:** `i64`

*Defined at line 307*

---

## gl_set_uniform_mat4

Set a uniform mat4 value.

```fides
pub fn gl_set_uniform_mat4(location: i64, mat: i64) -> void {
```

**Parameters:**

- `location`: `i64`
- `mat`: `i64`

**Returns:** `void`

*Defined at line 349*

---

## gl_set_uniform_f

Set a uniform float value.

```fides
pub fn gl_set_uniform_f(location: i64, value: f64) -> void {
```

**Parameters:**

- `location`: `i64`
- `value`: `f64`

**Returns:** `void`

*Defined at line 354*

---

## gl_set_uniform_i

Set a uniform int value.

```fides
pub fn gl_set_uniform_i(location: i64, value: i64) -> void {
```

**Parameters:**

- `location`: `i64`
- `value`: `i64`

**Returns:** `void`

*Defined at line 359*

---

## gl_get_uniform

Get the location of a uniform variable.

```fides
pub fn gl_get_uniform(program: i64, name: str) -> i64 {
```

**Parameters:**

- `program`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 364*

---

## gl_get_attrib

Get the location of an attribute variable.

```fides
pub fn gl_get_attrib(program: i64, name: str) -> i64 {
```

**Parameters:**

- `program`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 369*

---

## DEFAULT_VERT_SHADER:

Basic vertex shader for 3D rendering with MVP matrix.

```fides
pub const DEFAULT_VERT_SHADER: str = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nlayout(location = 1) in vec3 aNormal;\nlayout(location = 2) in vec2 aTexCoord;\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nout vec2 TexCoord;\nout vec3 Normal;\nvoid main() {\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n    TexCoord = aTexCoord;\n    Normal = mat3(transpose(inverse(model))) * aNormal;\n}\n";
```

*Defined at line 376*

---

## DEFAULT_FRAG_SHADER:

Basic fragment shader with texture sampling.

```fides
pub const DEFAULT_FRAG_SHADER: str = "#version 330 core\nin vec2 TexCoord;\nin vec3 Normal;\nout vec4 FragColor;\nuniform sampler2D texture1;\nvoid main() {\n    FragColor = texture(texture1, TexCoord);\n}\n";
```

*Defined at line 379*

---

## gl_create_default_program

Create the default shader program.

```fides
pub fn gl_create_default_program() -> i64 {
```

**Returns:** `i64`

*Defined at line 382*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
