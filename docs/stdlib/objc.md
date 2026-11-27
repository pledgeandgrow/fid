[All modules](index.md)

# objc -- Fides Documentation

## Contents

- [OBJC_ENC_CGPOINT:](#OBJC_ENC_CGPOINT:) (const)
- [OBJC_ENC_CGSIZE:](#OBJC_ENC_CGSIZE:) (const)
- [OBJC_ENC_CGRECT:](#OBJC_ENC_CGRECT:) (const)
- [OBJC_ENC_NSRANGE:](#OBJC_ENC_NSRANGE:) (const)
- [objc_encode_type](#objc_encode_type) (function)
- [objc_bool_encoding](#objc_bool_encoding) (function)
- [objc_method_types](#objc_method_types) (function)
- [ObjcType](#ObjcType) (struct)
- [ObjcTypes](#ObjcTypes) (struct)
- [objc_parse_types](#objc_parse_types) (function)
- [objc_type_size](#objc_type_size) (function)
- [objc_annotate_types](#objc_annotate_types) (function)
- [objc_selector_args](#objc_selector_args) (function)
- [objc_selector_valid](#objc_selector_valid) (function)
- [objc_method_check](#objc_method_check) (function)
- [ObjcRuntime](#ObjcRuntime) (struct)
- [objc_runtime_native](#objc_runtime_native) (function)
- [objc_msg](#objc_msg) (function)
- [objc_class_msg](#objc_class_msg) (function)
- [objc_new](#objc_new) (function)
- [objc_retain](#objc_retain) (function)
- [objc_release](#objc_release) (function)
- [objc_autorelease](#objc_autorelease) (function)
- [objc_responds](#objc_responds) (function)
- [objc_cstr_read](#objc_cstr_read) (function)
- [objc_nsstring](#objc_nsstring) (function)
- [objc_str](#objc_str) (function)
- [objc_nsnumber_int](#objc_nsnumber_int) (function)
- [objc_nsnumber_value](#objc_nsnumber_value) (function)
- [objc_nsarray](#objc_nsarray) (function)
- [objc_nsarray_items](#objc_nsarray_items) (function)
- [objc_nsdictionary_strs](#objc_nsdictionary_strs) (function)
- [objc_dict_get_str](#objc_dict_get_str) (function)
- [objc_set_double](#objc_set_double) (function)
- [objc_get_double](#objc_get_double) (function)
- [ObjcMethodSpec](#ObjcMethodSpec) (struct)
- [ObjcClassSpec](#ObjcClassSpec) (struct)
- [objc_class_spec](#objc_class_spec) (function)
- [objc_spec_add_method](#objc_spec_add_method) (function)
- [objc_spec_check](#objc_spec_check) (function)
- [objc_class_begin](#objc_class_begin) (function)
- [objc_class_end](#objc_class_end) (function)
- [objc_pool_push](#objc_pool_push) (function)
- [objc_pool_pop](#objc_pool_pop) (function)

## OBJC_ENC_CGPOINT:

```fides
pub const OBJC_ENC_CGPOINT: str = "{CGPoint=dd}";
```

*Defined at line 75*

---

## OBJC_ENC_CGSIZE:

```fides
pub const OBJC_ENC_CGSIZE: str = "{CGSize=dd}";
```

*Defined at line 76*

---

## OBJC_ENC_CGRECT:

```fides
pub const OBJC_ENC_CGRECT: str = "{CGRect={CGPoint=dd}{CGSize=dd}}";
```

*Defined at line 77*

---

## OBJC_ENC_NSRANGE:

```fides
pub const OBJC_ENC_NSRANGE: str = "{_NSRange=QQ}";
```

*Defined at line 78*

---

## objc_encode_type

The encoding of a C / Objective-C type name on arm64 (BOOL is bool,
"B"; use objc_bool_encoding for x86_64). Unknown names give "?".

```fides
pub fn objc_encode_type(t: str) -> str {
```

**Parameters:**

- `t`: `str`

**Returns:** `str`

*Defined at line 82*

---

## objc_bool_encoding

BOOL's encoding: "B" on arm64 (iOS, Apple silicon), "c" on x86_64.

```fides
pub fn objc_bool_encoding(arm64: bool) -> str {
```

**Parameters:**

- `arm64`: `bool`

**Returns:** `str`

*Defined at line 166*

---

## objc_method_types

A method's type string: return type, self, _cmd, then the arguments.

```fides
pub fn objc_method_types(ret: str, args: Vec<str>) -> str {
```

**Parameters:**

- `ret`: `str`
- `args`: `Vec<str>`

**Returns:** `str`

*Defined at line 174*

---

## ObjcType

One parsed type of an encoding string.

```fides
pub struct ObjcType {
```

**Fields:**

| Name | Type |
|------|------|
| `enc` | `str,  // the type's encoding text, qualifiers removed` |
| `size` | `i64,  // bytes on LP64 (0 for void or an opaque struct)` |
| `align` | `i64` |

*Defined at line 185*

---

## ObjcTypes

A parsed encoding: the types in order; `ok` false on malformed text.

```fides
pub struct ObjcTypes {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `types` | `Vec<ObjcType>` |

*Defined at line 192*

---

## objc_parse_types

Parse a type string such as "v@:@" or the runtime's "v24@0:8@16"
(offset digits between types are skipped).

```fides
pub fn objc_parse_types(s: str) -> ObjcTypes {
```

**Parameters:**

- `s`: `str`

**Returns:** `ObjcTypes`

*Defined at line 385*

---

## objc_type_size

The size in bytes of one encoded type (LP64), -1 if malformed.

```fides
pub fn objc_type_size(enc: str) -> i64 {
```

**Parameters:**

- `enc`: `str`

**Returns:** `i64`

*Defined at line 414*

---

## objc_annotate_types

The annotated form the runtime's method_getTypeEncoding gives:
"v@:@" -> "v24@0:8@16" (each argument in 8-byte slots).

```fides
pub fn objc_annotate_types(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 424*

---

## objc_selector_args

Arguments a selector takes: its number of colons.

```fides
pub fn objc_selector_args(sel: str) -> i64 {
```

**Parameters:**

- `sel`: `str`

**Returns:** `i64`

*Defined at line 444*

---

## objc_selector_valid

Is `sel` a well-formed selector name (identifier parts, each part of a
multi-argument selector ending in ':')?

```fides
pub fn objc_selector_valid(sel: str) -> bool {
```

**Parameters:**

- `sel`: `str`

**Returns:** `bool`

*Defined at line 458*

---

## objc_method_check

"" when `types` fits selector `sel`, else why not.

```fides
pub fn objc_method_check(sel: str, types: str) -> str {
```

**Parameters:**

- `sel`: `str`
- `types`: `str`

**Returns:** `str`

*Defined at line 485*

---

## ObjcRuntime

Everything this module asks of libobjc.

```fides
pub struct ObjcRuntime {
```

**Fields:**

| Name | Type |
|------|------|
| `get_class` | `fn(str) -> i64` |
| `sel` | `fn(str) -> i64` |
| `send` | `fn(i64, i64, Vec<i64>) -> i64,  // receiver, SEL, up to 4 args` |
| `allocate_class` | `fn(i64, str) -> i64,  // superclass, name` |
| `register_class` | `fn(i64) -> void` |
| `get_protocol` | `fn(str) -> i64` |
| `add_protocol` | `fn(i64, i64) -> i64` |

*Defined at line 512*

---

## objc_runtime_native

The runtime of the device: libobjc.

```fides
pub fn objc_runtime_native() -> ObjcRuntime {
```

**Returns:** `ObjcRuntime`

*Defined at line 595*

---

## objc_msg

Send `sel_name` to `recv` with word arguments.

```fides
pub fn objc_msg(rt: &ObjcRuntime, recv: i64, sel_name: str, args: Vec<i64>) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `recv`: `i64`
- `sel_name`: `str`
- `args`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 602*

---

## objc_class_msg

Send to a class by name: [ClassName sel...].

```fides
pub fn objc_class_msg(rt: &ObjcRuntime, cls_name: str, sel_name: str, args: Vec<i64>) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `cls_name`: `str`
- `sel_name`: `str`
- `args`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 625*

---

## objc_new

[[Class alloc] init]

```fides
pub fn objc_new(rt: &ObjcRuntime, cls_name: str) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `cls_name`: `str`

**Returns:** `i64`

*Defined at line 631*

---

## objc_retain

```fides
pub fn objc_retain(rt: &ObjcRuntime, obj: i64) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`

**Returns:** `i64`

*Defined at line 636*

---

## objc_release

```fides
pub fn objc_release(rt: &ObjcRuntime, obj: i64) {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`

*Defined at line 639*

---

## objc_autorelease

```fides
pub fn objc_autorelease(rt: &ObjcRuntime, obj: i64) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`

**Returns:** `i64`

*Defined at line 642*

---

## objc_responds

Does `obj` respond to a selector ([obj respondsToSelector:])?

```fides
pub fn objc_responds(rt: &ObjcRuntime, obj: i64, sel_name: str) -> bool {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`
- `sel_name`: `str`

**Returns:** `bool`

*Defined at line 647*

---

## objc_cstr_read

Text of a NUL-terminated C string at `addr` ("" for NULL).

```fides
pub fn objc_cstr_read(addr: i64) -> str {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `str`

*Defined at line 655*

---

## objc_nsstring

An autoreleased NSString with the text of `s`.

```fides
pub fn objc_nsstring(rt: &ObjcRuntime, s: str) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `s`: `str`

**Returns:** `i64`

*Defined at line 677*

---

## objc_str

The text of an NSString (its UTF8String), "" for nil.

```fides
pub fn objc_str(rt: &ObjcRuntime, ns: i64) -> str {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `ns`: `i64`

**Returns:** `str`

*Defined at line 682*

---

## objc_nsnumber_int

[NSNumber numberWithLongLong:]

```fides
pub fn objc_nsnumber_int(rt: &ObjcRuntime, v: i64) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `v`: `i64`

**Returns:** `i64`

*Defined at line 690*

---

## objc_nsnumber_value

[num longLongValue]

```fides
pub fn objc_nsnumber_value(rt: &ObjcRuntime, num: i64) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `num`: `i64`

**Returns:** `i64`

*Defined at line 695*

---

## objc_nsarray

An NSMutableArray of objects.

```fides
pub fn objc_nsarray(rt: &ObjcRuntime, objs: Vec<i64>) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `objs`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 700*

---

## objc_nsarray_items

Objects of an NSArray (count, objectAtIndex:).

```fides
pub fn objc_nsarray_items(rt: &ObjcRuntime, arr: i64) -> Vec<i64> {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `arr`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 711*

---

## objc_nsdictionary_strs

An NSMutableDictionary with string keys and values.

```fides
pub fn objc_nsdictionary_strs(rt: &ObjcRuntime, keys: Vec<str>, vals: Vec<str>) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `keys`: `Vec<str>`
- `vals`: `Vec<str>`

**Returns:** `i64`

*Defined at line 723*

---

## objc_dict_get_str

[dict objectForKey:@"key"] as text ("" when absent).

```fides
pub fn objc_dict_get_str(rt: &ObjcRuntime, dict: i64, key: str) -> str {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `dict`: `i64`
- `key`: `str`

**Returns:** `str`

*Defined at line 734*

---

## objc_set_double

Set a double (CGFloat, NSTimeInterval) property by key-value coding:
the value travels as an NSDecimalNumber made from its text.

```fides
pub fn objc_set_double(rt: &ObjcRuntime, obj: i64, key: str, v: f64) {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`
- `key`: `str`
- `v`: `f64`

*Defined at line 740*

---

## objc_get_double

Read a double property by key-value coding (valueForKey: boxes it).

```fides
pub fn objc_get_double(rt: &ObjcRuntime, obj: i64, key: str) -> f64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `obj`: `i64`
- `key`: `str`

**Returns:** `f64`

*Defined at line 746*

---

## ObjcMethodSpec

One method a delegate class gets.

```fides
pub struct ObjcMethodSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `sel` | `str` |
| `types` | `str` |

*Defined at line 757*

---

## ObjcClassSpec

A class to create at run time (a delegate, a target for actions).

```fides
pub struct ObjcClassSpec {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `superclass` | `str` |
| `protocols` | `Vec<str>` |
| `methods` | `Vec<ObjcMethodSpec>` |

*Defined at line 763*

---

## objc_class_spec

```fides
pub fn objc_class_spec(name: str, superclass: str) -> ObjcClassSpec {
```

**Parameters:**

- `name`: `str`
- `superclass`: `str`

**Returns:** `ObjcClassSpec`

*Defined at line 770*

---

## objc_spec_add_method

```fides
pub fn objc_spec_add_method(spec: &mut ObjcClassSpec, sel: str, types: str) {
```

**Parameters:**

- `spec`: `&mut ObjcClassSpec`
- `sel`: `str`
- `types`: `str`

*Defined at line 776*

---

## objc_spec_check

Every problem of a spec ("" lines none): a bad class name, a method
whose types do not fit its selector, a selector given twice.

```fides
pub fn objc_spec_check(spec: &ObjcClassSpec) -> Vec<str> {
```

**Parameters:**

- `spec`: `&ObjcClassSpec`

**Returns:** `Vec<str>`

*Defined at line 784*

---

## objc_class_begin

Allocate the class pair and add its protocols: the class to give
methods (class_addMethod) before objc_class_end; 0 when the spec is
invalid, the superclass is missing or the name is taken.

```fides
pub fn objc_class_begin(rt: &ObjcRuntime, spec: &ObjcClassSpec) -> i64 {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `spec`: `&ObjcClassSpec`

**Returns:** `i64`

*Defined at line 811*

---

## objc_class_end

Register a class made by objc_class_begin; it can be instantiated now.

```fides
pub fn objc_class_end(rt: &ObjcRuntime, cls: i64) {
```

**Parameters:**

- `rt`: `&ObjcRuntime`
- `cls`: `i64`

*Defined at line 839*

---

## objc_pool_push

Push and pop an autorelease pool around work (device builds).

```fides
pub fn objc_pool_push() -> i64 {
```

**Returns:** `i64`

*Defined at line 845*

---

## objc_pool_pop

```fides
pub fn objc_pool_pop(pool: i64) {
```

**Parameters:**

- `pool`: `i64`

*Defined at line 854*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
