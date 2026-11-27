[All modules](index.md)

# metal -- Fides Documentation

## Contents

- [mtl_class](#mtl_class) (function)
- [mtl_sel](#mtl_sel) (function)
- [mtl_msg0](#mtl_msg0) (function)
- [mtl_msg1](#mtl_msg1) (function)
- [mtl_msg2](#mtl_msg2) (function)
- [mtl_msg3](#mtl_msg3) (function)
- [mtl_msg_d4](#mtl_msg_d4) (function)
- [mtl_cstr](#mtl_cstr) (function)
- [mtl_read_cstr](#mtl_read_cstr) (function)
- [mtl_nsstring](#mtl_nsstring) (function)
- [mtl_nsstring_str](#mtl_nsstring_str) (function)
- [mtl_size_mem](#mtl_size_mem) (function)
- [mtl_device_default](#mtl_device_default) (function)
- [mtl_device_name](#mtl_device_name) (function)
- [mtl_new_command_queue](#mtl_new_command_queue) (function)
- [mtl_new_buffer](#mtl_new_buffer) (function)
- [mtl_new_buffer_with_bytes](#mtl_new_buffer_with_bytes) (function)
- [mtl_buffer_contents](#mtl_buffer_contents) (function)
- [mtl_buffer_length](#mtl_buffer_length) (function)
- [mtl_release](#mtl_release) (function)
- [mtl_new_library_with_source](#mtl_new_library_with_source) (function)
- [mtl_new_function](#mtl_new_function) (function)
- [mtl_new_compute_pipeline](#mtl_new_compute_pipeline) (function)
- [mtl_render_pipeline_descriptor](#mtl_render_pipeline_descriptor) (function)
- [mtl_new_render_pipeline](#mtl_new_render_pipeline) (function)
- [mtl_command_buffer](#mtl_command_buffer) (function)
- [mtl_compute_encoder](#mtl_compute_encoder) (function)
- [mtl_encoder_set_pipeline](#mtl_encoder_set_pipeline) (function)
- [mtl_encoder_set_buffer](#mtl_encoder_set_buffer) (function)
- [mtl_encoder_dispatch](#mtl_encoder_dispatch) (function)
- [mtl_end_encoding](#mtl_end_encoding) (function)
- [mtl_commit_and_wait](#mtl_commit_and_wait) (function)
- [MTL_RESOURCE_CPU_CACHE_DEFAULT:](#MTL_RESOURCE_CPU_CACHE_DEFAULT:) (const)
- [MTL_RESOURCE_CPU_CACHE_WRITE_COMBINED:](#MTL_RESOURCE_CPU_CACHE_WRITE_COMBINED:) (const)
- [MTL_RESOURCE_STORAGE_SHARED:](#MTL_RESOURCE_STORAGE_SHARED:) (const)
- [MTL_RESOURCE_STORAGE_MANAGED:](#MTL_RESOURCE_STORAGE_MANAGED:) (const)
- [MTL_RESOURCE_STORAGE_PRIVATE:](#MTL_RESOURCE_STORAGE_PRIVATE:) (const)
- [MTL_RESOURCE_STORAGE_MEMORYLESS:](#MTL_RESOURCE_STORAGE_MEMORYLESS:) (const)
- [MTL_RESOURCE_HAZARD_DEFAULT:](#MTL_RESOURCE_HAZARD_DEFAULT:) (const)
- [MTL_RESOURCE_HAZARD_UNTRACKED:](#MTL_RESOURCE_HAZARD_UNTRACKED:) (const)
- [MTL_RESOURCE_HAZARD_TRACKED:](#MTL_RESOURCE_HAZARD_TRACKED:) (const)
- [mtl_resource_options](#mtl_resource_options) (function)
- [mtl_options_storage](#mtl_options_storage) (function)
- [MTL_PIXEL_FORMAT_INVALID:](#MTL_PIXEL_FORMAT_INVALID:) (const)
- [MTL_PIXEL_FORMAT_A8_UNORM:](#MTL_PIXEL_FORMAT_A8_UNORM:) (const)
- [MTL_PIXEL_FORMAT_R8_UNORM:](#MTL_PIXEL_FORMAT_R8_UNORM:) (const)
- [MTL_PIXEL_FORMAT_R8_UNORM_SRGB:](#MTL_PIXEL_FORMAT_R8_UNORM_SRGB:) (const)
- [MTL_PIXEL_FORMAT_R8_UINT:](#MTL_PIXEL_FORMAT_R8_UINT:) (const)
- [MTL_PIXEL_FORMAT_R16_FLOAT:](#MTL_PIXEL_FORMAT_R16_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RG8_UNORM:](#MTL_PIXEL_FORMAT_RG8_UNORM:) (const)
- [MTL_PIXEL_FORMAT_B5G6R5_UNORM:](#MTL_PIXEL_FORMAT_B5G6R5_UNORM:) (const)
- [MTL_PIXEL_FORMAT_R32_UINT:](#MTL_PIXEL_FORMAT_R32_UINT:) (const)
- [MTL_PIXEL_FORMAT_R32_SINT:](#MTL_PIXEL_FORMAT_R32_SINT:) (const)
- [MTL_PIXEL_FORMAT_R32_FLOAT:](#MTL_PIXEL_FORMAT_R32_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RG16_FLOAT:](#MTL_PIXEL_FORMAT_RG16_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RGBA8_UNORM:](#MTL_PIXEL_FORMAT_RGBA8_UNORM:) (const)
- [MTL_PIXEL_FORMAT_RGBA8_UNORM_SRGB:](#MTL_PIXEL_FORMAT_RGBA8_UNORM_SRGB:) (const)
- [MTL_PIXEL_FORMAT_RGBA8_UINT:](#MTL_PIXEL_FORMAT_RGBA8_UINT:) (const)
- [MTL_PIXEL_FORMAT_BGRA8_UNORM:](#MTL_PIXEL_FORMAT_BGRA8_UNORM:) (const)
- [MTL_PIXEL_FORMAT_BGRA8_UNORM_SRGB:](#MTL_PIXEL_FORMAT_BGRA8_UNORM_SRGB:) (const)
- [MTL_PIXEL_FORMAT_RGB10A2_UNORM:](#MTL_PIXEL_FORMAT_RGB10A2_UNORM:) (const)
- [MTL_PIXEL_FORMAT_RG11B10_FLOAT:](#MTL_PIXEL_FORMAT_RG11B10_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RGB9E5_FLOAT:](#MTL_PIXEL_FORMAT_RGB9E5_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_BGR10A2_UNORM:](#MTL_PIXEL_FORMAT_BGR10A2_UNORM:) (const)
- [MTL_PIXEL_FORMAT_RG32_FLOAT:](#MTL_PIXEL_FORMAT_RG32_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RGBA16_FLOAT:](#MTL_PIXEL_FORMAT_RGBA16_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_RGBA32_FLOAT:](#MTL_PIXEL_FORMAT_RGBA32_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_BC1_RGBA:](#MTL_PIXEL_FORMAT_BC1_RGBA:) (const)
- [MTL_PIXEL_FORMAT_BC3_RGBA:](#MTL_PIXEL_FORMAT_BC3_RGBA:) (const)
- [MTL_PIXEL_FORMAT_BC7_RGBA_UNORM:](#MTL_PIXEL_FORMAT_BC7_RGBA_UNORM:) (const)
- [MTL_PIXEL_FORMAT_ASTC_4X4_LDR:](#MTL_PIXEL_FORMAT_ASTC_4X4_LDR:) (const)
- [MTL_PIXEL_FORMAT_DEPTH16_UNORM:](#MTL_PIXEL_FORMAT_DEPTH16_UNORM:) (const)
- [MTL_PIXEL_FORMAT_DEPTH32_FLOAT:](#MTL_PIXEL_FORMAT_DEPTH32_FLOAT:) (const)
- [MTL_PIXEL_FORMAT_STENCIL8:](#MTL_PIXEL_FORMAT_STENCIL8:) (const)
- [MTL_PIXEL_FORMAT_DEPTH24_UNORM_STENCIL8:](#MTL_PIXEL_FORMAT_DEPTH24_UNORM_STENCIL8:) (const)
- [MTL_PIXEL_FORMAT_DEPTH32_FLOAT_STENCIL8:](#MTL_PIXEL_FORMAT_DEPTH32_FLOAT_STENCIL8:) (const)
- [MtlPixelFormat](#MtlPixelFormat) (struct)
- [mtl_pixel_format](#mtl_pixel_format) (function)
- [mtl_bytes_per_row](#mtl_bytes_per_row) (function)
- [MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED:](#MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED:) (const)
- [MTL_VERTEX_FORMAT_SHORT2:](#MTL_VERTEX_FORMAT_SHORT2:) (const)
- [MTL_VERTEX_FORMAT_HALF2:](#MTL_VERTEX_FORMAT_HALF2:) (const)
- [MTL_VERTEX_FORMAT_HALF4:](#MTL_VERTEX_FORMAT_HALF4:) (const)
- [MTL_VERTEX_FORMAT_FLOAT:](#MTL_VERTEX_FORMAT_FLOAT:) (const)
- [MTL_VERTEX_FORMAT_FLOAT2:](#MTL_VERTEX_FORMAT_FLOAT2:) (const)
- [MTL_VERTEX_FORMAT_FLOAT3:](#MTL_VERTEX_FORMAT_FLOAT3:) (const)
- [MTL_VERTEX_FORMAT_FLOAT4:](#MTL_VERTEX_FORMAT_FLOAT4:) (const)
- [MTL_VERTEX_FORMAT_INT:](#MTL_VERTEX_FORMAT_INT:) (const)
- [MTL_VERTEX_FORMAT_UINT:](#MTL_VERTEX_FORMAT_UINT:) (const)
- [MTL_VERTEX_FORMAT_INT1010102_NORMALIZED:](#MTL_VERTEX_FORMAT_INT1010102_NORMALIZED:) (const)
- [MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED_BGRA:](#MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED_BGRA:) (const)
- [MTL_VERTEX_FORMAT_UCHAR:](#MTL_VERTEX_FORMAT_UCHAR:) (const)
- [MTL_VERTEX_FORMAT_USHORT:](#MTL_VERTEX_FORMAT_USHORT:) (const)
- [MTL_VERTEX_FORMAT_HALF:](#MTL_VERTEX_FORMAT_HALF:) (const)
- [MtlVertexFormat](#MtlVertexFormat) (struct)
- [mtl_vertex_format](#mtl_vertex_format) (function)
- [MTL_MAX_VERTEX_ATTRIBUTES:](#MTL_MAX_VERTEX_ATTRIBUTES:) (const)
- [MTL_MAX_BUFFER_INDEX:](#MTL_MAX_BUFFER_INDEX:) (const)
- [MTL_STEP_PER_VERTEX:](#MTL_STEP_PER_VERTEX:) (const)
- [MTL_STEP_PER_INSTANCE:](#MTL_STEP_PER_INSTANCE:) (const)
- [MtlVertexLayout](#MtlVertexLayout) (struct)
- [mtl_vertex_layout_new](#mtl_vertex_layout_new) (function)
- [mtl_vertex_add](#mtl_vertex_add) (function)
- [mtl_vertex_msl](#mtl_vertex_msl) (function)
- [mtl_vertex_apply](#mtl_vertex_apply) (function)
- [mtl_validate_render_pipeline](#mtl_validate_render_pipeline) (function)
- [MtlDispatch](#MtlDispatch) (struct)
- [mtl_dispatch_2d](#mtl_dispatch_2d) (function)

## mtl_class

```fides
pub fn mtl_class(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 62*

---

## mtl_sel

```fides
pub fn mtl_sel(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 66*

---

## mtl_msg0

```fides
pub fn mtl_msg0(obj: i64, sel: str) -> i64 {
```

**Parameters:**

- `obj`: `i64`
- `sel`: `str`

**Returns:** `i64`

*Defined at line 70*

---

## mtl_msg1

```fides
pub fn mtl_msg1(obj: i64, sel: str, a: i64) -> i64 {
```

**Parameters:**

- `obj`: `i64`
- `sel`: `str`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 74*

---

## mtl_msg2

```fides
pub fn mtl_msg2(obj: i64, sel: str, a: i64, b: i64) -> i64 {
```

**Parameters:**

- `obj`: `i64`
- `sel`: `str`
- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 78*

---

## mtl_msg3

```fides
pub fn mtl_msg3(obj: i64, sel: str, a: i64, b: i64, c: i64) -> i64 {
```

**Parameters:**

- `obj`: `i64`
- `sel`: `str`
- `a`: `i64`
- `b`: `i64`
- `c`: `i64`

**Returns:** `i64`

*Defined at line 82*

---

## mtl_msg_d4

A message whose arguments are four doubles (e.g. a MTLClearColor,
which AArch64 passes in d0..d3 as a homogeneous float aggregate).

```fides
pub fn mtl_msg_d4(obj: i64, sel: str, x0: f64, x1: f64, x2: f64, x3: f64) -> i64 {
```

**Parameters:**

- `obj`: `i64`
- `sel`: `str`
- `x0`: `f64`
- `x1`: `f64`
- `x2`: `f64`
- `x3`: `f64`

**Returns:** `i64`

*Defined at line 88*

---

## mtl_cstr

A NUL-terminated copy of s in C memory (free with p_free).

```fides
pub fn mtl_cstr(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 93*

---

## mtl_read_cstr

The C string at an address (as from -[NSString UTF8String]).

```fides
pub fn mtl_read_cstr(p: i64) -> str {
```

**Parameters:**

- `p`: `i64`

**Returns:** `str`

*Defined at line 111*

---

## mtl_nsstring

An autoreleased NSString.

```fides
pub fn mtl_nsstring(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 134*

---

## mtl_nsstring_str

```fides
pub fn mtl_nsstring_str(ns: i64) -> str {
```

**Parameters:**

- `ns`: `i64`

**Returns:** `str`

*Defined at line 138*

---

## mtl_size_mem

Three NSUIntegers in C memory: an MTLSize to pass by reference
(AArch64 passes structs over 16 bytes as a pointer to a copy).

```fides
pub fn mtl_size_mem(w: i64, h: i64, d: i64) -> i64 {
```

**Parameters:**

- `w`: `i64`
- `h`: `i64`
- `d`: `i64`

**Returns:** `i64`

*Defined at line 144*

---

## mtl_device_default

```fides
pub fn mtl_device_default() -> i64 {
```

**Returns:** `i64`

*Defined at line 160*

---

## mtl_device_name

```fides
pub fn mtl_device_name(dev: i64) -> str {
```

**Parameters:**

- `dev`: `i64`

**Returns:** `str`

*Defined at line 164*

---

## mtl_new_command_queue

```fides
pub fn mtl_new_command_queue(dev: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`

**Returns:** `i64`

*Defined at line 168*

---

## mtl_new_buffer

-[MTLDevice newBufferWithLength:options:]

```fides
pub fn mtl_new_buffer(dev: i64, length: i64, options: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`
- `length`: `i64`
- `options`: `i64`

**Returns:** `i64`

*Defined at line 173*

---

## mtl_new_buffer_with_bytes

-[MTLDevice newBufferWithBytes:length:options:] (bytes: a C address).

```fides
pub fn mtl_new_buffer_with_bytes(dev: i64, bytes: i64, length: i64, options: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`
- `bytes`: `i64`
- `length`: `i64`
- `options`: `i64`

**Returns:** `i64`

*Defined at line 178*

---

## mtl_buffer_contents

The CPU address of a shared buffer's storage.

```fides
pub fn mtl_buffer_contents(buf: i64) -> i64 {
```

**Parameters:**

- `buf`: `i64`

**Returns:** `i64`

*Defined at line 183*

---

## mtl_buffer_length

```fides
pub fn mtl_buffer_length(buf: i64) -> i64 {
```

**Parameters:**

- `buf`: `i64`

**Returns:** `i64`

*Defined at line 187*

---

## mtl_release

```fides
pub fn mtl_release(obj: i64) {
```

**Parameters:**

- `obj`: `i64`

*Defined at line 191*

---

## mtl_new_library_with_source

Compile MSL source; returns the MTLLibrary (0 on error; the NSError is
written to an 8-byte slot when `error_slot` is not 0).

```fides
pub fn mtl_new_library_with_source(dev: i64, source: str, error_slot: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`
- `source`: `str`
- `error_slot`: `i64`

**Returns:** `i64`

*Defined at line 197*

---

## mtl_new_function

```fides
pub fn mtl_new_function(lib: i64, name: str) -> i64 {
```

**Parameters:**

- `lib`: `i64`
- `name`: `str`

**Returns:** `i64`

*Defined at line 201*

---

## mtl_new_compute_pipeline

```fides
pub fn mtl_new_compute_pipeline(dev: i64, function: i64, error_slot: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`
- `function`: `i64`
- `error_slot`: `i64`

**Returns:** `i64`

*Defined at line 205*

---

## mtl_render_pipeline_descriptor

A new MTLRenderPipelineDescriptor with vertex/fragment functions and
the first color attachment's pixel format set.

```fides
pub fn mtl_render_pipeline_descriptor(vertex_fn: i64, fragment_fn: i64, color_format: i64, depth_format: i64, samples: i64) -> i64 {
```

**Parameters:**

- `vertex_fn`: `i64`
- `fragment_fn`: `i64`
- `color_format`: `i64`
- `depth_format`: `i64`
- `samples`: `i64`

**Returns:** `i64`

*Defined at line 211*

---

## mtl_new_render_pipeline

```fides
pub fn mtl_new_render_pipeline(dev: i64, descriptor: i64, error_slot: i64) -> i64 {
```

**Parameters:**

- `dev`: `i64`
- `descriptor`: `i64`
- `error_slot`: `i64`

**Returns:** `i64`

*Defined at line 222*

---

## mtl_command_buffer

```fides
pub fn mtl_command_buffer(queue: i64) -> i64 {
```

**Parameters:**

- `queue`: `i64`

**Returns:** `i64`

*Defined at line 226*

---

## mtl_compute_encoder

```fides
pub fn mtl_compute_encoder(cmd: i64) -> i64 {
```

**Parameters:**

- `cmd`: `i64`

**Returns:** `i64`

*Defined at line 230*

---

## mtl_encoder_set_pipeline

```fides
pub fn mtl_encoder_set_pipeline(enc: i64, pipeline: i64) {
```

**Parameters:**

- `enc`: `i64`
- `pipeline`: `i64`

*Defined at line 234*

---

## mtl_encoder_set_buffer

```fides
pub fn mtl_encoder_set_buffer(enc: i64, buf: i64, offset: i64, index: i64) {
```

**Parameters:**

- `enc`: `i64`
- `buf`: `i64`
- `offset`: `i64`
- `index`: `i64`

*Defined at line 238*

---

## mtl_encoder_dispatch

-[MTLComputeCommandEncoder dispatchThreadgroups:threadsPerThreadgroup:]
(AArch64: the two MTLSize arguments by reference).

```fides
pub fn mtl_encoder_dispatch(enc: i64, d: MtlDispatch) {
```

**Parameters:**

- `enc`: `i64`
- `d`: `MtlDispatch`

*Defined at line 244*

---

## mtl_end_encoding

```fides
pub fn mtl_end_encoding(enc: i64) {
```

**Parameters:**

- `enc`: `i64`

*Defined at line 248*

---

## mtl_commit_and_wait

```fides
pub fn mtl_commit_and_wait(cmd: i64) {
```

**Parameters:**

- `cmd`: `i64`

*Defined at line 252*

---

## MTL_RESOURCE_CPU_CACHE_DEFAULT:

```fides
pub const MTL_RESOURCE_CPU_CACHE_DEFAULT: i64 = 0;
```

*Defined at line 259*

---

## MTL_RESOURCE_CPU_CACHE_WRITE_COMBINED:

```fides
pub const MTL_RESOURCE_CPU_CACHE_WRITE_COMBINED: i64 = 1;
```

*Defined at line 260*

---

## MTL_RESOURCE_STORAGE_SHARED:

```fides
pub const MTL_RESOURCE_STORAGE_SHARED: i64 = 0;
```

*Defined at line 261*

---

## MTL_RESOURCE_STORAGE_MANAGED:

```fides
pub const MTL_RESOURCE_STORAGE_MANAGED: i64 = 0x10;
```

*Defined at line 262*

---

## MTL_RESOURCE_STORAGE_PRIVATE:

```fides
pub const MTL_RESOURCE_STORAGE_PRIVATE: i64 = 0x20;
```

*Defined at line 263*

---

## MTL_RESOURCE_STORAGE_MEMORYLESS:

```fides
pub const MTL_RESOURCE_STORAGE_MEMORYLESS: i64 = 0x30;
```

*Defined at line 264*

---

## MTL_RESOURCE_HAZARD_DEFAULT:

```fides
pub const MTL_RESOURCE_HAZARD_DEFAULT: i64 = 0;
```

*Defined at line 265*

---

## MTL_RESOURCE_HAZARD_UNTRACKED:

```fides
pub const MTL_RESOURCE_HAZARD_UNTRACKED: i64 = 0x100;
```

*Defined at line 266*

---

## MTL_RESOURCE_HAZARD_TRACKED:

```fides
pub const MTL_RESOURCE_HAZARD_TRACKED: i64 = 0x200;
```

*Defined at line 267*

---

## mtl_resource_options

MTLResourceOptions from its parts: storage mode 0..3 (shared, managed,
private, memoryless), CPU cache mode 0..1, hazard tracking 0..2.

```fides
pub fn mtl_resource_options(storage: i64, cache: i64, hazard: i64) -> i64 {
```

**Parameters:**

- `storage`: `i64`
- `cache`: `i64`
- `hazard`: `i64`

**Returns:** `i64`

*Defined at line 271*

---

## mtl_options_storage

```fides
pub fn mtl_options_storage(o: i64) -> i64 {
```

**Parameters:**

- `o`: `i64`

**Returns:** `i64`

*Defined at line 275*

---

## MTL_PIXEL_FORMAT_INVALID:

```fides
pub const MTL_PIXEL_FORMAT_INVALID: i64 = 0;
```

*Defined at line 281*

---

## MTL_PIXEL_FORMAT_A8_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_A8_UNORM: i64 = 1;
```

*Defined at line 282*

---

## MTL_PIXEL_FORMAT_R8_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_R8_UNORM: i64 = 10;
```

*Defined at line 283*

---

## MTL_PIXEL_FORMAT_R8_UNORM_SRGB:

```fides
pub const MTL_PIXEL_FORMAT_R8_UNORM_SRGB: i64 = 11;
```

*Defined at line 284*

---

## MTL_PIXEL_FORMAT_R8_UINT:

```fides
pub const MTL_PIXEL_FORMAT_R8_UINT: i64 = 13;
```

*Defined at line 285*

---

## MTL_PIXEL_FORMAT_R16_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_R16_FLOAT: i64 = 25;
```

*Defined at line 286*

---

## MTL_PIXEL_FORMAT_RG8_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_RG8_UNORM: i64 = 30;
```

*Defined at line 287*

---

## MTL_PIXEL_FORMAT_B5G6R5_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_B5G6R5_UNORM: i64 = 40;
```

*Defined at line 288*

---

## MTL_PIXEL_FORMAT_R32_UINT:

```fides
pub const MTL_PIXEL_FORMAT_R32_UINT: i64 = 53;
```

*Defined at line 289*

---

## MTL_PIXEL_FORMAT_R32_SINT:

```fides
pub const MTL_PIXEL_FORMAT_R32_SINT: i64 = 54;
```

*Defined at line 290*

---

## MTL_PIXEL_FORMAT_R32_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_R32_FLOAT: i64 = 55;
```

*Defined at line 291*

---

## MTL_PIXEL_FORMAT_RG16_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RG16_FLOAT: i64 = 65;
```

*Defined at line 292*

---

## MTL_PIXEL_FORMAT_RGBA8_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_RGBA8_UNORM: i64 = 70;
```

*Defined at line 293*

---

## MTL_PIXEL_FORMAT_RGBA8_UNORM_SRGB:

```fides
pub const MTL_PIXEL_FORMAT_RGBA8_UNORM_SRGB: i64 = 71;
```

*Defined at line 294*

---

## MTL_PIXEL_FORMAT_RGBA8_UINT:

```fides
pub const MTL_PIXEL_FORMAT_RGBA8_UINT: i64 = 73;
```

*Defined at line 295*

---

## MTL_PIXEL_FORMAT_BGRA8_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_BGRA8_UNORM: i64 = 80;
```

*Defined at line 296*

---

## MTL_PIXEL_FORMAT_BGRA8_UNORM_SRGB:

```fides
pub const MTL_PIXEL_FORMAT_BGRA8_UNORM_SRGB: i64 = 81;
```

*Defined at line 297*

---

## MTL_PIXEL_FORMAT_RGB10A2_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_RGB10A2_UNORM: i64 = 90;
```

*Defined at line 298*

---

## MTL_PIXEL_FORMAT_RG11B10_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RG11B10_FLOAT: i64 = 92;
```

*Defined at line 299*

---

## MTL_PIXEL_FORMAT_RGB9E5_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RGB9E5_FLOAT: i64 = 93;
```

*Defined at line 300*

---

## MTL_PIXEL_FORMAT_BGR10A2_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_BGR10A2_UNORM: i64 = 94;
```

*Defined at line 301*

---

## MTL_PIXEL_FORMAT_RG32_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RG32_FLOAT: i64 = 105;
```

*Defined at line 302*

---

## MTL_PIXEL_FORMAT_RGBA16_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RGBA16_FLOAT: i64 = 115;
```

*Defined at line 303*

---

## MTL_PIXEL_FORMAT_RGBA32_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_RGBA32_FLOAT: i64 = 125;
```

*Defined at line 304*

---

## MTL_PIXEL_FORMAT_BC1_RGBA:

```fides
pub const MTL_PIXEL_FORMAT_BC1_RGBA: i64 = 130;
```

*Defined at line 305*

---

## MTL_PIXEL_FORMAT_BC3_RGBA:

```fides
pub const MTL_PIXEL_FORMAT_BC3_RGBA: i64 = 134;
```

*Defined at line 306*

---

## MTL_PIXEL_FORMAT_BC7_RGBA_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_BC7_RGBA_UNORM: i64 = 152;
```

*Defined at line 307*

---

## MTL_PIXEL_FORMAT_ASTC_4X4_LDR:

```fides
pub const MTL_PIXEL_FORMAT_ASTC_4X4_LDR: i64 = 204;
```

*Defined at line 308*

---

## MTL_PIXEL_FORMAT_DEPTH16_UNORM:

```fides
pub const MTL_PIXEL_FORMAT_DEPTH16_UNORM: i64 = 250;
```

*Defined at line 309*

---

## MTL_PIXEL_FORMAT_DEPTH32_FLOAT:

```fides
pub const MTL_PIXEL_FORMAT_DEPTH32_FLOAT: i64 = 252;
```

*Defined at line 310*

---

## MTL_PIXEL_FORMAT_STENCIL8:

```fides
pub const MTL_PIXEL_FORMAT_STENCIL8: i64 = 253;
```

*Defined at line 311*

---

## MTL_PIXEL_FORMAT_DEPTH24_UNORM_STENCIL8:

```fides
pub const MTL_PIXEL_FORMAT_DEPTH24_UNORM_STENCIL8: i64 = 255;
```

*Defined at line 312*

---

## MTL_PIXEL_FORMAT_DEPTH32_FLOAT_STENCIL8:

```fides
pub const MTL_PIXEL_FORMAT_DEPTH32_FLOAT_STENCIL8: i64 = 260;
```

*Defined at line 313*

---

## MtlPixelFormat

```fides
pub struct MtlPixelFormat {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `i64` |
| `name` | `str` |
| `bytes` | `i64` |
| `block_w` | `i64` |
| `block_h` | `i64` |
| `depth` | `bool` |
| `stencil` | `bool` |
| `srgb` | `bool` |
| `compressed` | `bool` |
| `color_renderable` | `bool` |

*Defined at line 315*

---

## mtl_pixel_format

Properties of an MTLPixelFormat value (name "" when not in the table).

```fides
pub fn mtl_pixel_format(v: i64) -> MtlPixelFormat {
```

**Parameters:**

- `v`: `i64`

**Returns:** `MtlPixelFormat`

*Defined at line 335*

---

## mtl_bytes_per_row

Bytes per row of a texture region of a pixel format.

```fides
pub fn mtl_bytes_per_row(v: i64, width: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `width`: `i64`

**Returns:** `i64`

*Defined at line 610*

---

## MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED:

```fides
pub const MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED: i64 = 9;
```

*Defined at line 617*

---

## MTL_VERTEX_FORMAT_SHORT2:

```fides
pub const MTL_VERTEX_FORMAT_SHORT2: i64 = 16;
```

*Defined at line 618*

---

## MTL_VERTEX_FORMAT_HALF2:

```fides
pub const MTL_VERTEX_FORMAT_HALF2: i64 = 25;
```

*Defined at line 619*

---

## MTL_VERTEX_FORMAT_HALF4:

```fides
pub const MTL_VERTEX_FORMAT_HALF4: i64 = 27;
```

*Defined at line 620*

---

## MTL_VERTEX_FORMAT_FLOAT:

```fides
pub const MTL_VERTEX_FORMAT_FLOAT: i64 = 28;
```

*Defined at line 621*

---

## MTL_VERTEX_FORMAT_FLOAT2:

```fides
pub const MTL_VERTEX_FORMAT_FLOAT2: i64 = 29;
```

*Defined at line 622*

---

## MTL_VERTEX_FORMAT_FLOAT3:

```fides
pub const MTL_VERTEX_FORMAT_FLOAT3: i64 = 30;
```

*Defined at line 623*

---

## MTL_VERTEX_FORMAT_FLOAT4:

```fides
pub const MTL_VERTEX_FORMAT_FLOAT4: i64 = 31;
```

*Defined at line 624*

---

## MTL_VERTEX_FORMAT_INT:

```fides
pub const MTL_VERTEX_FORMAT_INT: i64 = 32;
```

*Defined at line 625*

---

## MTL_VERTEX_FORMAT_UINT:

```fides
pub const MTL_VERTEX_FORMAT_UINT: i64 = 36;
```

*Defined at line 626*

---

## MTL_VERTEX_FORMAT_INT1010102_NORMALIZED:

```fides
pub const MTL_VERTEX_FORMAT_INT1010102_NORMALIZED: i64 = 40;
```

*Defined at line 627*

---

## MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED_BGRA:

```fides
pub const MTL_VERTEX_FORMAT_UCHAR4_NORMALIZED_BGRA: i64 = 42;
```

*Defined at line 628*

---

## MTL_VERTEX_FORMAT_UCHAR:

```fides
pub const MTL_VERTEX_FORMAT_UCHAR: i64 = 45;
```

*Defined at line 629*

---

## MTL_VERTEX_FORMAT_USHORT:

```fides
pub const MTL_VERTEX_FORMAT_USHORT: i64 = 49;
```

*Defined at line 630*

---

## MTL_VERTEX_FORMAT_HALF:

```fides
pub const MTL_VERTEX_FORMAT_HALF: i64 = 53;
```

*Defined at line 631*

---

## MtlVertexFormat

```fides
pub struct MtlVertexFormat {
```

**Fields:**

| Name | Type |
|------|------|
| `value` | `i64` |
| `name` | `str` |
| `size` | `i64` |
| `components` | `i64` |
| `msl` | `str` |

*Defined at line 633*

---

## mtl_vertex_format

MTLVertexFormat by value (name "" when unknown).

```fides
pub fn mtl_vertex_format(v: i64) -> MtlVertexFormat {
```

**Parameters:**

- `v`: `i64`

**Returns:** `MtlVertexFormat`

*Defined at line 653*

---

## MTL_MAX_VERTEX_ATTRIBUTES:

```fides
pub const MTL_MAX_VERTEX_ATTRIBUTES: i64 = 31;
```

*Defined at line 759*

---

## MTL_MAX_BUFFER_INDEX:

```fides
pub const MTL_MAX_BUFFER_INDEX: i64 = 30;
```

*Defined at line 760*

---

## MTL_STEP_PER_VERTEX:

```fides
pub const MTL_STEP_PER_VERTEX: i64 = 1;
```

*Defined at line 761*

---

## MTL_STEP_PER_INSTANCE:

```fides
pub const MTL_STEP_PER_INSTANCE: i64 = 2;
```

*Defined at line 762*

---

## MtlVertexLayout

```fides
pub struct MtlVertexLayout {
```

**Fields:**

| Name | Type |
|------|------|
| `buffer_index` | `i64` |
| `attributes` | `Vec<i64>` |
| `formats` | `Vec<i64>` |
| `names` | `Vec<str>` |
| `offsets` | `Vec<i64>` |
| `stride` | `i64` |
| `step` | `i64` |
| `errors` | `Vec<str>` |

*Defined at line 764*

---

## mtl_vertex_layout_new

```fides
pub fn mtl_vertex_layout_new(buffer_index: i64) -> MtlVertexLayout {
```

**Parameters:**

- `buffer_index`: `i64`

**Returns:** `MtlVertexLayout`

*Defined at line 775*

---

## mtl_vertex_add

Append an attribute at the next 4-byte aligned offset.

```fides
pub fn mtl_vertex_add(l: &mut MtlVertexLayout, attribute: i64, format: i64, name: str) {
```

**Parameters:**

- `l`: `&mut MtlVertexLayout`
- `attribute`: `i64`
- `format`: `i64`
- `name`: `str`

*Defined at line 786*

---

## mtl_vertex_msl

The MSL struct a vertex function takes as [[stage_in]].

```fides
pub fn mtl_vertex_msl(l: &MtlVertexLayout, struct_name: str) -> str {
```

**Parameters:**

- `l`: `&MtlVertexLayout`
- `struct_name`: `str`

**Returns:** `str`

*Defined at line 811*

---

## mtl_vertex_apply

Fill a real MTLVertexDescriptor (from +[MTLVertexDescriptor
vertexDescriptor]) with this layout.

```fides
pub fn mtl_vertex_apply(l: &MtlVertexLayout, descriptor: i64) {
```

**Parameters:**

- `l`: `&MtlVertexLayout`
- `descriptor`: `i64`

*Defined at line 823*

---

## mtl_validate_render_pipeline

Problems of a render pipeline description: color attachment formats
(up to 8, color-renderable), the depth format (a depth format or
Invalid), and the sample count (1, 2, 4 or 8).

```fides
pub fn mtl_validate_render_pipeline(color_formats: &Vec<i64>, depth_format: i64, samples: i64) -> Vec<str> {
```

**Parameters:**

- `color_formats`: `&Vec<i64>`
- `depth_format`: `i64`
- `samples`: `i64`

**Returns:** `Vec<str>`

*Defined at line 844*

---

## MtlDispatch

```fides
pub struct MtlDispatch {
```

**Fields:**

| Name | Type |
|------|------|
| `tg_w` | `i64` |
| `tg_h` | `i64` |
| `groups_w` | `i64` |
| `groups_h` | `i64` |

*Defined at line 873*

---

## mtl_dispatch_2d

Threadgroup size and count for a w x h grid: threadExecutionWidth wide,
maxTotalThreadsPerThreadgroup / width tall (Apple's recommended shape),
enough groups to cover the grid.

```fides
pub fn mtl_dispatch_2d(width: i64, height: i64, exec_width: i64, max_threads: i64) -> MtlDispatch {
```

**Parameters:**

- `width`: `i64`
- `height`: `i64`
- `exec_width`: `i64`
- `max_threads`: `i64`

**Returns:** `MtlDispatch`

*Defined at line 883*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
