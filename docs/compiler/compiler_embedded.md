[All modules](index.md)

# compiler::embedded -- Fides Documentation

## Contents

- [FixedBufferAllocator](#FixedBufferAllocator) (struct)
- [FixedBufferAllocator_new](#FixedBufferAllocator_new) (function)
- [FixedBufferAllocator_alloc](#FixedBufferAllocator_alloc) (function)
- [FixedBufferAllocator_free](#FixedBufferAllocator_free) (function)
- [FixedBufferAllocator_reset](#FixedBufferAllocator_reset) (function)
- [FixedBufferAllocator_usage](#FixedBufferAllocator_usage) (function)
- [FixedBufferAllocator_peak](#FixedBufferAllocator_peak) (function)
- [FixedBufferAllocator_capacity](#FixedBufferAllocator_capacity) (function)
- [FixedBufferAllocator_free_space](#FixedBufferAllocator_free_space) (function)
- [FixedBufferAllocator_read](#FixedBufferAllocator_read) (function)
- [FixedBufferAllocator_write](#FixedBufferAllocator_write) (function)
- [SizeReport](#SizeReport) (struct)
- [SizeReport_new](#SizeReport_new) (function)
- [BinarySizeOptimizer](#BinarySizeOptimizer) (struct)
- [BinarySizeOptimizer_new](#BinarySizeOptimizer_new) (function)
- [BinarySizeOptimizer_analyze](#BinarySizeOptimizer_analyze) (function)
- [BinarySizeOptimizer_recommendations](#BinarySizeOptimizer_recommendations) (function)
- [RealTimeStatus](#RealTimeStatus) (enum)
- [RealTimeAnalyzer](#RealTimeAnalyzer) (struct)
- [RealTimeAnalyzer_new](#RealTimeAnalyzer_new) (function)
- [RealTimeAnalyzer_check_function](#RealTimeAnalyzer_check_function) (function)
- [RealTimeAnalyzer_status](#RealTimeAnalyzer_status) (function)
- [RealTimeAnalyzer_verified_functions](#RealTimeAnalyzer_verified_functions) (function)
- [DeviceType](#DeviceType) (enum)
- [RegisterAccess](#RegisterAccess) (enum)
- [HalRegister](#HalRegister) (struct)
- [HalRegister_new](#HalRegister_new) (function)
- [HalDevice](#HalDevice) (struct)
- [HalDevice_new](#HalDevice_new) (function)
- [HalFramework](#HalFramework) (struct)
- [HalFramework_new](#HalFramework_new) (function)
- [HalFramework_register_device](#HalFramework_register_device) (function)
- [HalFramework_get_device](#HalFramework_get_device) (function)
- [HalFramework_generate_bindings](#HalFramework_generate_bindings) (function)
- [HalFramework_default_riscv_hal](#HalFramework_default_riscv_hal) (function)
- [char_from_digit](#char_from_digit) (function)
- [hex_char](#hex_char) (function)

## FixedBufferAllocator

```fides
pub struct FixedBufferAllocator {
```

**Fields:**

| Name | Type |
|------|------|
| `buffer` | `Vec<i64>` |
| `offset` | `i64` |
| `allocations` | `Vec<(i64, i64)>` |
| `total_allocated` | `i64` |
| `peak_usage` | `i64` |

*Defined at line 16*

---

## FixedBufferAllocator_new

```fides
pub fn FixedBufferAllocator_new(capacity: i64) -> FixedBufferAllocator {
```

**Parameters:**

- `capacity`: `i64`

**Returns:** `FixedBufferAllocator`

*Defined at line 24*

---

## FixedBufferAllocator_alloc

Allocate `size` bytes. Returns a pointer (offset into buffer) or -1 if out of memory.

```fides
pub fn FixedBufferAllocator_alloc(self: &mut FixedBufferAllocator, size: i64) -> i64 {
```

**Parameters:**

- `self`: `&mut FixedBufferAllocator`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 37*

---

## FixedBufferAllocator_free

Free a previously allocated pointer.
Note: This is a bump allocator -- freed memory is not reclaimed.

```fides
pub fn FixedBufferAllocator_free(self: &mut FixedBufferAllocator, ptr: i64) -> void {
```

**Parameters:**

- `self`: `&mut FixedBufferAllocator`
- `ptr`: `i64`

**Returns:** `void`

*Defined at line 58*

---

## FixedBufferAllocator_reset

Reset the allocator to its initial state.

```fides
pub fn FixedBufferAllocator_reset(self: &mut FixedBufferAllocator) -> void {
```

**Parameters:**

- `self`: `&mut FixedBufferAllocator`

**Returns:** `void`

*Defined at line 72*

---

## FixedBufferAllocator_usage

Get the current memory usage (bytes allocated).

```fides
pub fn FixedBufferAllocator_usage(self: &FixedBufferAllocator) -> i64 {
```

**Parameters:**

- `self`: `&FixedBufferAllocator`

**Returns:** `i64`

*Defined at line 80*

---

## FixedBufferAllocator_peak

Get the peak memory usage.

```fides
pub fn FixedBufferAllocator_peak(self: &FixedBufferAllocator) -> i64 {
```

**Parameters:**

- `self`: `&FixedBufferAllocator`

**Returns:** `i64`

*Defined at line 85*

---

## FixedBufferAllocator_capacity

Get the total capacity.

```fides
pub fn FixedBufferAllocator_capacity(self: &FixedBufferAllocator) -> i64 {
```

**Parameters:**

- `self`: `&FixedBufferAllocator`

**Returns:** `i64`

*Defined at line 90*

---

## FixedBufferAllocator_free_space

Get the remaining free memory.

```fides
pub fn FixedBufferAllocator_free_space(self: &FixedBufferAllocator) -> i64 {
```

**Parameters:**

- `self`: `&FixedBufferAllocator`

**Returns:** `i64`

*Defined at line 95*

---

## FixedBufferAllocator_read

Read a byte at the given offset.

```fides
pub fn FixedBufferAllocator_read(self: &FixedBufferAllocator, offset: i64) -> i64 {
```

**Parameters:**

- `self`: `&FixedBufferAllocator`
- `offset`: `i64`

**Returns:** `i64`

*Defined at line 100*

---

## FixedBufferAllocator_write

Write a byte at the given offset.

```fides
pub fn FixedBufferAllocator_write(self: &mut FixedBufferAllocator, offset: i64, value: i64) -> bool {
```

**Parameters:**

- `self`: `&mut FixedBufferAllocator`
- `offset`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 108*

---

## SizeReport

```fides
pub struct SizeReport {
```

**Fields:**

| Name | Type |
|------|------|
| `total_size` | `i64` |
| `text_size` | `i64` |
| `data_size` | `i64` |
| `bss_size` | `i64` |
| `debug_size` | `i64` |
| `meets_target` | `bool` |

*Defined at line 118*

---

## SizeReport_new

```fides
pub fn SizeReport_new() -> SizeReport {
```

**Returns:** `SizeReport`

*Defined at line 127*

---

## BinarySizeOptimizer

```fides
pub struct BinarySizeOptimizer {
```

**Fields:**

| Name | Type |
|------|------|
| `target_size` | `i64` |
| `strip_debug` | `bool` |
| `remove_unused` | `bool` |
| `merge_strings` | `bool` |

*Defined at line 133*

---

## BinarySizeOptimizer_new

```fides
pub fn BinarySizeOptimizer_new(target_size: i64) -> BinarySizeOptimizer {
```

**Parameters:**

- `target_size`: `i64`

**Returns:** `BinarySizeOptimizer`

*Defined at line 140*

---

## BinarySizeOptimizer_analyze

Analyze a binary and report size breakdown.

```fides
pub fn BinarySizeOptimizer_analyze(self: &BinarySizeOptimizer, binary: Vec<i64>) -> SizeReport {
```

**Parameters:**

- `self`: `&BinarySizeOptimizer`
- `binary`: `Vec<i64>`

**Returns:** `SizeReport`

*Defined at line 147*

---

## BinarySizeOptimizer_recommendations

Get optimization recommendations for reducing binary size.

```fides
pub fn BinarySizeOptimizer_recommendations(self: &BinarySizeOptimizer, binary: Vec<i64>) -> Vec<str> {
```

**Parameters:**

- `self`: `&BinarySizeOptimizer`
- `binary`: `Vec<i64>`

**Returns:** `Vec<str>`

*Defined at line 159*

---

## RealTimeStatus

```fides
pub enum RealTimeStatus {
```

**Variants:**

- `Safe`
- `// estimated_cycles`
- `Violation`

*Defined at line 185*

---

## RealTimeAnalyzer

```fides
pub struct RealTimeAnalyzer {
```

**Fields:**

| Name | Type |
|------|------|
| `max_cycles` | `i64` |
| `verified` | `Vec<(str, RealTimeStatus)>` |

*Defined at line 191*

---

## RealTimeAnalyzer_new

```fides
pub fn RealTimeAnalyzer_new(max_cycles: i64) -> RealTimeAnalyzer {
```

**Parameters:**

- `max_cycles`: `i64`

**Returns:** `RealTimeAnalyzer`

*Defined at line 196*

---

## RealTimeAnalyzer_check_function

Check if a function is real-time safe.

```fides
pub fn RealTimeAnalyzer_check_function(self: &mut RealTimeAnalyzer, name: str, has_loops: bool, has_recursion: bool, has_dyn_dispatch: bool, has_alloc: bool, has_io: bool, estimated_cycles: i64) -> RealTimeStatus {
```

**Parameters:**

- `self`: `&mut RealTimeAnalyzer`
- `name`: `str`
- `has_loops`: `bool`
- `has_recursion`: `bool`
- `has_dyn_dispatch`: `bool`
- `has_alloc`: `bool`
- `has_io`: `bool`
- `estimated_cycles`: `i64`

**Returns:** `RealTimeStatus`

*Defined at line 203*

---

## RealTimeAnalyzer_status

Get the verification status for a function.

```fides
pub fn RealTimeAnalyzer_status(self: &RealTimeAnalyzer, name: str) -> RealTimeStatus {
```

**Parameters:**

- `self`: `&RealTimeAnalyzer`
- `name`: `str`

**Returns:** `RealTimeStatus`

*Defined at line 246*

---

## RealTimeAnalyzer_verified_functions

Get all verified function names.

```fides
pub fn RealTimeAnalyzer_verified_functions(self: &RealTimeAnalyzer) -> Vec<str> {
```

**Parameters:**

- `self`: `&RealTimeAnalyzer`

**Returns:** `Vec<str>`

*Defined at line 260*

---

## DeviceType

```fides
pub enum DeviceType {
```

**Variants:**

- `Gpio`
- `Uart`
- `Spi`
- `I2c`
- `Timer`
- `InterruptController`
- `Mmu`
- `Custom`

*Defined at line 273*

---

## RegisterAccess

```fides
pub enum RegisterAccess {
```

**Variants:**

- `ReadOnly`
- `WriteOnly`
- `ReadWrite`
- `WriteToClear`

*Defined at line 284*

---

## HalRegister

```fides
pub struct HalRegister {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |
| `width` | `i64` |
| `access` | `RegisterAccess` |
| `reset_value` | `i64` |
| `description` | `str` |

*Defined at line 291*

---

## HalRegister_new

```fides
pub fn HalRegister_new(name: str, offset: i64, width: i64, access: RegisterAccess, reset_value: i64, description: str) -> HalRegister {
```

**Parameters:**

- `name`: `str`
- `offset`: `i64`
- `width`: `i64`
- `access`: `RegisterAccess`
- `reset_value`: `i64`
- `description`: `str`

**Returns:** `HalRegister`

*Defined at line 300*

---

## HalDevice

```fides
pub struct HalDevice {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `device_type` | `DeviceType` |
| `base_address` | `i64` |
| `registers` | `Vec<HalRegister>` |

*Defined at line 306*

---

## HalDevice_new

```fides
pub fn HalDevice_new(name: str, device_type: DeviceType, base_address: i64, registers: Vec<HalRegister>) -> HalDevice {
```

**Parameters:**

- `name`: `str`
- `device_type`: `DeviceType`
- `base_address`: `i64`
- `registers`: `Vec<HalRegister>`

**Returns:** `HalDevice`

*Defined at line 313*

---

## HalFramework

```fides
pub struct HalFramework {
```

**Fields:**

| Name | Type |
|------|------|
| `devices` | `Vec<HalDevice>` |

*Defined at line 319*

---

## HalFramework_new

```fides
pub fn HalFramework_new() -> HalFramework {
```

**Returns:** `HalFramework`

*Defined at line 323*

---

## HalFramework_register_device

Register a hardware device

```fides
pub fn HalFramework_register_device(self: &mut HalFramework, device: HalDevice) -> void {
```

**Parameters:**

- `self`: `&mut HalFramework`
- `device`: `HalDevice`

**Returns:** `void`

*Defined at line 330*

---

## HalFramework_get_device

Get a device by name

```fides
pub fn HalFramework_get_device(self: &HalFramework, name: str) -> i64 {
```

**Parameters:**

- `self`: `&HalFramework`
- `name`: `str`

**Returns:** `i64`

*Defined at line 335*

---

## HalFramework_generate_bindings

Generate Fides FFI bindings for all registered devices.

```fides
pub fn HalFramework_generate_bindings(self: &HalFramework) -> str {
```

**Parameters:**

- `self`: `&HalFramework`

**Returns:** `str`

*Defined at line 347*

---

## HalFramework_default_riscv_hal

Create a default HAL for a common microcontroller (RISC-V HiFive1)

```fides
pub fn HalFramework_default_riscv_hal() -> HalFramework {
```

**Returns:** `HalFramework`

*Defined at line 431*

---

## char_from_digit

```fides
pub fn char_from_digit(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 498*

---

## hex_char

```fides
pub fn hex_char(d: i64) -> str {
```

**Parameters:**

- `d`: `i64`

**Returns:** `str`

*Defined at line 504*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
