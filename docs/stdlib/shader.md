# shader — Fides Documentation

## Contents

- [UNIFORM_FLOAT:](#uniform_float:) (const)
- [UNIFORM_INT:](#uniform_int:) (const)
- [UNIFORM_VEC3:](#uniform_vec3:) (const)
- [UNIFORM_VEC4:](#uniform_vec4:) (const)
- [UNIFORM_MAT4:](#uniform_mat4:) (const)
- [UNIFORM_TEXTURE:](#uniform_texture:) (const)
- [UniformValue](#uniformvalue) (struct)
- [Material](#material) (struct)
- [BLEND_OPAQUE:](#blend_opaque:) (const)
- [BLEND_ALPHA:](#blend_alpha:) (const)
- [BLEND_ADDITIVE:](#blend_additive:) (const)
- [CULL_NONE:](#cull_none:) (const)
- [CULL_BACK:](#cull_back:) (const)
- [CULL_FRONT:](#cull_front:) (const)
- [material_create](#material_create) (function)
- [material_create_custom](#material_create_custom) (function)
- [material_set_float](#material_set_float) (function)
- [material_set_int](#material_set_int) (function)
- [material_set_vec3](#material_set_vec3) (function)
- [material_set_vec4](#material_set_vec4) (function)
- [material_set_mat4](#material_set_mat4) (function)
- [material_set_texture](#material_set_texture) (function)
- [material_bind](#material_bind) (function)
- [material_set_blend](#material_set_blend) (function)
- [material_set_cull](#material_set_cull) (function)
- [material_set_depth](#material_set_depth) (function)
- [material_destroy](#material_destroy) (function)
- [material_program](#material_program) (function)
- [material_is_valid](#material_is_valid) (function)
- [material_uniform_count](#material_uniform_count) (function)
- [SHADER_UNLIT_VERT:](#shader_unlit_vert:) (const)
- [SHADER_UNLIT_FRAG:](#shader_unlit_frag:) (const)
- [SHADER_DIFFUSE_VERT:](#shader_diffuse_vert:) (const)
- [SHADER_DIFFUSE_FRAG:](#shader_diffuse_frag:) (const)
- [SHADER_TEXT_VERT:](#shader_text_vert:) (const)
- [SHADER_TEXT_FRAG:](#shader_text_frag:) (const)
- [SHADER_POST_VERT:](#shader_post_vert:) (const)
- [SHADER_POST_FRAG:](#shader_post_frag:) (const)
- [MaterialCache](#materialcache) (struct)
- [material_cache_create](#material_cache_create) (function)
- [material_cache_get](#material_cache_get) (function)
- [material_cache_get_idx](#material_cache_get_idx) (function)
- [material_cache_clear](#material_cache_clear) (function)
- [material_cache_count](#material_cache_count) (function)

## UNIFORM_FLOAT:

```fides
pub const UNIFORM_FLOAT: i64 = 0;
```

*Defined at line 32*

---

## UNIFORM_INT:

```fides
pub const UNIFORM_INT: i64 = 1;
```

*Defined at line 33*

---

## UNIFORM_VEC3:

```fides
pub const UNIFORM_VEC3: i64 = 2;
```

*Defined at line 34*

---

## UNIFORM_VEC4:

```fides
pub const UNIFORM_VEC4: i64 = 3;
```

*Defined at line 35*

---

## UNIFORM_MAT4:

```fides
pub const UNIFORM_MAT4: i64 = 4;
```

*Defined at line 36*

---

## UNIFORM_TEXTURE:

```fides
pub const UNIFORM_TEXTURE: i64 = 5;
```

*Defined at line 37*

---

## UniformValue

```fides
pub struct UniformValue {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `kind` | `i64` |
| `float_val` | `f64` |
| `int_val` | `i64` |
| `vec_val` | `vec4,    // used for vec3 (xyz) and vec4` |
| `mat_val` | `mat4` |
| `texture_unit` | `i64` |
| `location` | `i64,    // cached uniform location` |

*Defined at line 39*

---

## Material

```fides
pub struct Material {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `program` | `i64,       // GL shader program handle` |
| `uniforms` | `Vec<UniformValue>` |
| `blend_mode` | `i64,   // 0 = opaque, 1 = alpha, 2 = additive` |
| `cull_mode` | `i64,    // 0 = none, 1 = back, 2 = front` |
| `depth_test` | `bool` |
| `depth_write` | `bool` |
| `valid` | `bool` |

*Defined at line 50*

---

## BLEND_OPAQUE:

```fides
pub const BLEND_OPAQUE: i64 = 0;
```

*Defined at line 61*

---

## BLEND_ALPHA:

```fides
pub const BLEND_ALPHA: i64 = 1;
```

*Defined at line 62*

---

## BLEND_ADDITIVE:

```fides
pub const BLEND_ADDITIVE: i64 = 2;
```

*Defined at line 63*

---

## CULL_NONE:

```fides
pub const CULL_NONE: i64 = 0;
```

*Defined at line 65*

---

## CULL_BACK:

```fides
pub const CULL_BACK: i64 = 1;
```

*Defined at line 66*

---

## CULL_FRONT:

```fides
pub const CULL_FRONT: i64 = 2;
```

*Defined at line 67*

---

## material_create

Create a material from a shader name.
Uses built-in shader templates.

```fides
pub fn material_create(shader_name: str) -> Material {
```

**Parameters:**

- `shader_name`: `str`

**Returns:** `Material`

*Defined at line 71*

---

## material_create_custom

Create a material from custom shader source.

```fides
pub fn material_create_custom(name: str, vert_src: str, frag_src: str) -> Material {
```

**Parameters:**

- `name`: `str`
- `vert_src`: `str`
- `frag_src`: `str`

**Returns:** `Material`

*Defined at line 108*

---

## material_set_float

Set a float uniform.

```fides
pub fn material_set_float(mat: &mut Material, name: str, val: f64) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `val`: `f64`

**Returns:** `void`

*Defined at line 133*

---

## material_set_int

Set an int uniform.

```fides
pub fn material_set_int(mat: &mut Material, name: str, val: i64) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `val`: `i64`

**Returns:** `void`

*Defined at line 151*

---

## material_set_vec3

Set a vec3 uniform.

```fides
pub fn material_set_vec3(mat: &mut Material, name: str, val: vec3) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `val`: `vec3`

**Returns:** `void`

*Defined at line 169*

---

## material_set_vec4

Set a vec4 uniform.

```fides
pub fn material_set_vec4(mat: &mut Material, name: str, val: vec4) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `val`: `vec4`

**Returns:** `void`

*Defined at line 187*

---

## material_set_mat4

Set a mat4 uniform.

```fides
pub fn material_set_mat4(mat: &mut Material, name: str, val: mat4) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `val`: `mat4`

**Returns:** `void`

*Defined at line 205*

---

## material_set_texture

Set a texture uniform (binds to a texture unit).

```fides
pub fn material_set_texture(mat: &mut Material, name: str, unit: i64) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `name`: `str`
- `unit`: `i64`

**Returns:** `void`

*Defined at line 223*

---

## material_bind

Bind the material (use program + set all uniforms).

```fides
pub fn material_bind(mat: &Material) -> void {
```

**Parameters:**

- `mat`: `&Material`

**Returns:** `void`

*Defined at line 241*

---

## material_set_blend

Set blend mode.

```fides
pub fn material_set_blend(mat: &mut Material, mode: i64) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `mode`: `i64`

**Returns:** `void`

*Defined at line 280*

---

## material_set_cull

Set cull mode.

```fides
pub fn material_set_cull(mat: &mut Material, mode: i64) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `mode`: `i64`

**Returns:** `void`

*Defined at line 285*

---

## material_set_depth

Set depth test/write.

```fides
pub fn material_set_depth(mat: &mut Material, test: bool, write: bool) -> void {
```

**Parameters:**

- `mat`: `&mut Material`
- `test`: `bool`
- `write`: `bool`

**Returns:** `void`

*Defined at line 290*

---

## material_destroy

Destroy the material (delete shader program).

```fides
pub fn material_destroy(mat: &mut Material) -> void {
```

**Parameters:**

- `mat`: `&mut Material`

**Returns:** `void`

*Defined at line 296*

---

## material_program

Get the shader program handle.

```fides
pub fn material_program(mat: &Material) -> i64 {
```

**Parameters:**

- `mat`: `&Material`

**Returns:** `i64`

*Defined at line 306*

---

## material_is_valid

Check if the material is valid.

```fides
pub fn material_is_valid(mat: &Material) -> bool {
```

**Parameters:**

- `mat`: `&Material`

**Returns:** `bool`

*Defined at line 311*

---

## material_uniform_count

Get the number of uniforms.

```fides
pub fn material_uniform_count(mat: &Material) -> i64 {
```

**Parameters:**

- `mat`: `&Material`

**Returns:** `i64`

*Defined at line 316*

---

## SHADER_UNLIT_VERT:

```fides
pub const SHADER_UNLIT_VERT: str = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nvoid main() {\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n}\n";
```

*Defined at line 322*

---

## SHADER_UNLIT_FRAG:

```fides
pub const SHADER_UNLIT_FRAG: str = "#version 330 core\nuniform vec3 color;\nout vec4 FragColor;\nvoid main() {\n    FragColor = vec4(color, 1.0);\n}\n";
```

*Defined at line 324*

---

## SHADER_DIFFUSE_VERT:

```fides
pub const SHADER_DIFFUSE_VERT: str = "#version 330 core\nlayout(location = 0) in vec3 aPos;\nlayout(location = 1) in vec3 aNormal;\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nout vec3 Normal;\nout vec3 FragPos;\nvoid main() {\n    FragPos = vec3(model * vec4(aPos, 1.0));\n    Normal = mat3(transpose(inverse(model))) * aNormal;\n    gl_Position = projection * view * vec4(FragPos, 1.0);\n}\n";
```

*Defined at line 326*

---

## SHADER_DIFFUSE_FRAG:

```fides
pub const SHADER_DIFFUSE_FRAG: str = "#version 330 core\nin vec3 Normal;\nin vec3 FragPos;\nuniform vec3 lightPos;\nuniform vec3 lightColor;\nuniform vec3 objectColor;\nout vec4 FragColor;\nvoid main() {\n    vec3 ambient = 0.1 * lightColor;\n    vec3 norm = normalize(Normal);\n    vec3 lightDir = normalize(lightPos - FragPos);\n    float diff = max(dot(norm, lightDir), 0.0);\n    vec3 diffuse = diff * lightColor;\n    vec3 result = (ambient + diffuse) * objectColor;\n    FragColor = vec4(result, 1.0);\n}\n";
```

*Defined at line 328*

---

## SHADER_TEXT_VERT:

```fides
pub const SHADER_TEXT_VERT: str = "#version 330 core\nlayout(location = 0) in vec2 aPos;\nlayout(location = 1) in vec2 aTexCoord;\nout vec2 TexCoord;\nuniform mat4 projection;\nvoid main() {\n    gl_Position = projection * vec4(aPos, 0.0, 1.0);\n    TexCoord = aTexCoord;\n}\n";
```

*Defined at line 330*

---

## SHADER_TEXT_FRAG:

```fides
pub const SHADER_TEXT_FRAG: str = "#version 330 core\nin vec2 TexCoord;\nout vec4 FragColor;\nuniform sampler2D text;\nuniform vec3 textColor;\nvoid main() {\n    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoord).r);\n    FragColor = vec4(textColor, 1.0) * sampled;\n}\n";
```

*Defined at line 332*

---

## SHADER_POST_VERT:

```fides
pub const SHADER_POST_VERT: str = "#version 330 core\nlayout(location = 0) in vec2 aPos;\nlayout(location = 1) in vec2 aTexCoord;\nout vec2 TexCoord;\nvoid main() {\n    gl_Position = vec4(aPos, 0.0, 1.0);\n    TexCoord = aTexCoord;\n}\n";
```

*Defined at line 334*

---

## SHADER_POST_FRAG:

```fides
pub const SHADER_POST_FRAG: str = "#version 330 core\nin vec2 TexCoord;\nout vec4 FragColor;\nuniform sampler2D screenTexture;\nvoid main() {\n    FragColor = texture(screenTexture, TexCoord);\n}\n";
```

*Defined at line 336*

---

## MaterialCache

```fides
pub struct MaterialCache {
```

**Fields:**

| Name | Type |
|------|------|
| `materials` | `Vec<Material>` |
| `names` | `Vec<str>` |

*Defined at line 340*

---

## material_cache_create

Create a new material cache.

```fides
pub fn material_cache_create() -> MaterialCache {
```

**Returns:** `MaterialCache`

*Defined at line 346*

---

## material_cache_get

Get or create a cached material by shader name.

```fides
pub fn material_cache_get(cache: &mut MaterialCache, shader_name: str) -> i64 {
```

**Parameters:**

- `cache`: `&mut MaterialCache`
- `shader_name`: `str`

**Returns:** `i64`

*Defined at line 356*

---

## material_cache_get_idx

Get a cached material by index.

```fides
pub fn material_cache_get_idx(cache: &MaterialCache, idx: i64) -> Material {
```

**Parameters:**

- `cache`: `&MaterialCache`
- `idx`: `i64`

**Returns:** `Material`

*Defined at line 371*

---

## material_cache_clear

Clear the material cache.

```fides
pub fn material_cache_clear(cache: &mut MaterialCache) -> void {
```

**Parameters:**

- `cache`: `&mut MaterialCache`

**Returns:** `void`

*Defined at line 376*

---

## material_cache_count

Get the number of cached materials.

```fides
pub fn material_cache_count(cache: &MaterialCache) -> i64 {
```

**Parameters:**

- `cache`: `&MaterialCache`

**Returns:** `i64`

*Defined at line 382*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
