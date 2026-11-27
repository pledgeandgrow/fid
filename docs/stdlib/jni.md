[All modules](index.md)

# jni -- Fides Documentation

## Contents

- [JNI_VERSION_1_6:](#JNI_VERSION_1_6:) (const)
- [JNI_VERSION_1_8:](#JNI_VERSION_1_8:) (const)
- [JNI_VERSION_9:](#JNI_VERSION_9:) (const)
- [JNI_VERSION_10:](#JNI_VERSION_10:) (const)
- [JNI_VERSION_21:](#JNI_VERSION_21:) (const)
- [JNI_OK:](#JNI_OK:) (const)
- [JNI_ERR:](#JNI_ERR:) (const)
- [JNI_EDETACHED:](#JNI_EDETACHED:) (const)
- [JNI_EVERSION:](#JNI_EVERSION:) (const)
- [JNI_ENOMEM:](#JNI_ENOMEM:) (const)
- [JNI_EEXIST:](#JNI_EEXIST:) (const)
- [JNI_EINVAL:](#JNI_EINVAL:) (const)
- [JNI_COMMIT:](#JNI_COMMIT:) (const)
- [JNI_ABORT:](#JNI_ABORT:) (const)
- [JNI_INVALID_REF:](#JNI_INVALID_REF:) (const)
- [JNI_LOCAL_REF:](#JNI_LOCAL_REF:) (const)
- [JNI_GLOBAL_REF:](#JNI_GLOBAL_REF:) (const)
- [JNI_WEAK_GLOBAL_REF:](#JNI_WEAK_GLOBAL_REF:) (const)
- [JNI_KIND_OBJECT:](#JNI_KIND_OBJECT:) (const)
- [JNI_KIND_BOOLEAN:](#JNI_KIND_BOOLEAN:) (const)
- [JNI_KIND_BYTE:](#JNI_KIND_BYTE:) (const)
- [JNI_KIND_CHAR:](#JNI_KIND_CHAR:) (const)
- [JNI_KIND_SHORT:](#JNI_KIND_SHORT:) (const)
- [JNI_KIND_INT:](#JNI_KIND_INT:) (const)
- [JNI_KIND_LONG:](#JNI_KIND_LONG:) (const)
- [JNI_KIND_FLOAT:](#JNI_KIND_FLOAT:) (const)
- [JNI_KIND_DOUBLE:](#JNI_KIND_DOUBLE:) (const)
- [JNI_KIND_VOID:](#JNI_KIND_VOID:) (const)
- [JNI_FN_GET_VERSION:](#JNI_FN_GET_VERSION:) (const)
- [JNI_FN_FIND_CLASS:](#JNI_FN_FIND_CLASS:) (const)
- [JNI_FN_THROW_NEW:](#JNI_FN_THROW_NEW:) (const)
- [JNI_FN_EXCEPTION_OCCURRED:](#JNI_FN_EXCEPTION_OCCURRED:) (const)
- [JNI_FN_EXCEPTION_DESCRIBE:](#JNI_FN_EXCEPTION_DESCRIBE:) (const)
- [JNI_FN_EXCEPTION_CLEAR:](#JNI_FN_EXCEPTION_CLEAR:) (const)
- [JNI_FN_PUSH_LOCAL_FRAME:](#JNI_FN_PUSH_LOCAL_FRAME:) (const)
- [JNI_FN_POP_LOCAL_FRAME:](#JNI_FN_POP_LOCAL_FRAME:) (const)
- [JNI_FN_NEW_GLOBAL_REF:](#JNI_FN_NEW_GLOBAL_REF:) (const)
- [JNI_FN_DELETE_GLOBAL_REF:](#JNI_FN_DELETE_GLOBAL_REF:) (const)
- [JNI_FN_DELETE_LOCAL_REF:](#JNI_FN_DELETE_LOCAL_REF:) (const)
- [JNI_FN_NEW_OBJECT_A:](#JNI_FN_NEW_OBJECT_A:) (const)
- [JNI_FN_GET_OBJECT_CLASS:](#JNI_FN_GET_OBJECT_CLASS:) (const)
- [JNI_FN_GET_METHOD_ID:](#JNI_FN_GET_METHOD_ID:) (const)
- [JNI_FN_GET_FIELD_ID:](#JNI_FN_GET_FIELD_ID:) (const)
- [JNI_FN_GET_STATIC_METHOD_ID:](#JNI_FN_GET_STATIC_METHOD_ID:) (const)
- [JNI_FN_NEW_STRING:](#JNI_FN_NEW_STRING:) (const)
- [JNI_FN_GET_STRING_LENGTH:](#JNI_FN_GET_STRING_LENGTH:) (const)
- [JNI_FN_NEW_STRING_UTF:](#JNI_FN_NEW_STRING_UTF:) (const)
- [JNI_FN_GET_STRING_UTF_LENGTH:](#JNI_FN_GET_STRING_UTF_LENGTH:) (const)
- [JNI_FN_GET_STRING_UTF_CHARS:](#JNI_FN_GET_STRING_UTF_CHARS:) (const)
- [JNI_FN_RELEASE_STRING_UTF_CHARS:](#JNI_FN_RELEASE_STRING_UTF_CHARS:) (const)
- [JNI_FN_GET_ARRAY_LENGTH:](#JNI_FN_GET_ARRAY_LENGTH:) (const)
- [JNI_FN_REGISTER_NATIVES:](#JNI_FN_REGISTER_NATIVES:) (const)
- [JNI_FN_GET_JAVA_VM:](#JNI_FN_GET_JAVA_VM:) (const)
- [JNI_FN_NEW_WEAK_GLOBAL_REF:](#JNI_FN_NEW_WEAK_GLOBAL_REF:) (const)
- [JNI_FN_DELETE_WEAK_GLOBAL_REF:](#JNI_FN_DELETE_WEAK_GLOBAL_REF:) (const)
- [JNI_FN_EXCEPTION_CHECK:](#JNI_FN_EXCEPTION_CHECK:) (const)
- [JNI_FN_GET_OBJECT_REF_TYPE:](#JNI_FN_GET_OBJECT_REF_TYPE:) (const)
- [JNI_TABLE_LEN:](#JNI_TABLE_LEN:) (const)
- [JNI_VM_DESTROY:](#JNI_VM_DESTROY:) (const)
- [JNI_VM_ATTACH_CURRENT_THREAD:](#JNI_VM_ATTACH_CURRENT_THREAD:) (const)
- [JNI_VM_DETACH_CURRENT_THREAD:](#JNI_VM_DETACH_CURRENT_THREAD:) (const)
- [JNI_VM_GET_ENV:](#JNI_VM_GET_ENV:) (const)
- [JNI_VM_ATTACH_AS_DAEMON:](#JNI_VM_ATTACH_AS_DAEMON:) (const)
- [jni_table_len](#jni_table_len) (function)
- [jni_fn_name](#jni_fn_name) (function)
- [jni_fn_index](#jni_fn_index) (function)
- [jni_fn_offset](#jni_fn_offset) (function)
- [jni_vm_fn_name](#jni_vm_fn_name) (function)
- [jni_read_table_entry](#jni_read_table_entry) (function)
- [jni_type_descriptor](#jni_type_descriptor) (function)
- [jni_method_sig](#jni_method_sig) (function)
- [JniSig](#JniSig) (struct)
- [jni_parse_sig](#jni_parse_sig) (function)
- [jni_valid_field_desc](#jni_valid_field_desc) (function)
- [jni_desc_kind](#jni_desc_kind) (function)
- [JNI_CALL_VIRTUAL:](#JNI_CALL_VIRTUAL:) (const)
- [JNI_CALL_NONVIRTUAL:](#JNI_CALL_NONVIRTUAL:) (const)
- [JNI_CALL_STATIC:](#JNI_CALL_STATIC:) (const)
- [jni_call_slot](#jni_call_slot) (function)
- [jni_get_field_slot](#jni_get_field_slot) (function)
- [jni_set_field_slot](#jni_set_field_slot) (function)
- [jni_mangle](#jni_mangle) (function)
- [jni_native_symbol](#jni_native_symbol) (function)
- [jni_utf16_units](#jni_utf16_units) (function)
- [jni_utf16_to_codepoints](#jni_utf16_to_codepoints) (function)
- [jni_utf16_to_str](#jni_utf16_to_str) (function)
- [jni_mutf8_encode](#jni_mutf8_encode) (function)
- [JniDecoded](#JniDecoded) (struct)
- [jni_mutf8_decode](#jni_mutf8_decode) (function)
- [jni_cbuf](#jni_cbuf) (function)
- [jni_cbuf_read](#jni_cbuf_read) (function)
- [jni_cbuf_free](#jni_cbuf_free) (function)
- [JValue](#JValue) (enum)
- [jni_f32_bits](#jni_f32_bits) (function)
- [jni_jvalue_word](#jni_jvalue_word) (function)
- [jni_jvalue_words](#jni_jvalue_words) (function)
- [jni_jvalue_array](#jni_jvalue_array) (function)
- [JniEnv](#JniEnv) (struct)
- [jni_env](#jni_env) (function)
- [jni_invoke](#jni_invoke) (function)
- [jni_get_version](#jni_get_version) (function)
- [jni_find_class](#jni_find_class) (function)
- [jni_get_method_id](#jni_get_method_id) (function)
- [jni_get_static_method_id](#jni_get_static_method_id) (function)
- [jni_get_field_id](#jni_get_field_id) (function)
- [jni_call_method](#jni_call_method) (function)
- [jni_new_string](#jni_new_string) (function)
- [jni_string_to_str](#jni_string_to_str) (function)
- [jni_exception_pending](#jni_exception_pending) (function)
- [jni_throw_new](#jni_throw_new) (function)
- [jni_new_global_ref](#jni_new_global_ref) (function)
- [jni_delete_global_ref](#jni_delete_global_ref) (function)
- [jni_delete_local_ref](#jni_delete_local_ref) (function)
- [jni_push_local_frame](#jni_push_local_frame) (function)
- [jni_pop_local_frame](#jni_pop_local_frame) (function)
- [jni_leak_report](#jni_leak_report) (function)

## JNI_VERSION_1_6:

```fides
pub const JNI_VERSION_1_6: i64 = 0x00010006;
```

*Defined at line 60*

---

## JNI_VERSION_1_8:

```fides
pub const JNI_VERSION_1_8: i64 = 0x00010008;
```

*Defined at line 61*

---

## JNI_VERSION_9:

```fides
pub const JNI_VERSION_9: i64 = 0x00090000;
```

*Defined at line 62*

---

## JNI_VERSION_10:

```fides
pub const JNI_VERSION_10: i64 = 0x000a0000;
```

*Defined at line 63*

---

## JNI_VERSION_21:

```fides
pub const JNI_VERSION_21: i64 = 0x00150000;
```

*Defined at line 64*

---

## JNI_OK:

```fides
pub const JNI_OK: i64 = 0;
```

*Defined at line 66*

---

## JNI_ERR:

```fides
pub const JNI_ERR: i64 = -1;
```

*Defined at line 67*

---

## JNI_EDETACHED:

```fides
pub const JNI_EDETACHED: i64 = -2;
```

*Defined at line 68*

---

## JNI_EVERSION:

```fides
pub const JNI_EVERSION: i64 = -3;
```

*Defined at line 69*

---

## JNI_ENOMEM:

```fides
pub const JNI_ENOMEM: i64 = -4;
```

*Defined at line 70*

---

## JNI_EEXIST:

```fides
pub const JNI_EEXIST: i64 = -5;
```

*Defined at line 71*

---

## JNI_EINVAL:

```fides
pub const JNI_EINVAL: i64 = -6;
```

*Defined at line 72*

---

## JNI_COMMIT:

```fides
pub const JNI_COMMIT: i64 = 1;
```

*Defined at line 73*

---

## JNI_ABORT:

```fides
pub const JNI_ABORT: i64 = 2;
```

*Defined at line 74*

---

## JNI_INVALID_REF:

```fides
pub const JNI_INVALID_REF: i64 = 0;
```

*Defined at line 77*

---

## JNI_LOCAL_REF:

```fides
pub const JNI_LOCAL_REF: i64 = 1;
```

*Defined at line 78*

---

## JNI_GLOBAL_REF:

```fides
pub const JNI_GLOBAL_REF: i64 = 2;
```

*Defined at line 79*

---

## JNI_WEAK_GLOBAL_REF:

```fides
pub const JNI_WEAK_GLOBAL_REF: i64 = 3;
```

*Defined at line 80*

---

## JNI_KIND_OBJECT:

```fides
pub const JNI_KIND_OBJECT: i64 = 0;
```

*Defined at line 83*

---

## JNI_KIND_BOOLEAN:

```fides
pub const JNI_KIND_BOOLEAN: i64 = 1;
```

*Defined at line 84*

---

## JNI_KIND_BYTE:

```fides
pub const JNI_KIND_BYTE: i64 = 2;
```

*Defined at line 85*

---

## JNI_KIND_CHAR:

```fides
pub const JNI_KIND_CHAR: i64 = 3;
```

*Defined at line 86*

---

## JNI_KIND_SHORT:

```fides
pub const JNI_KIND_SHORT: i64 = 4;
```

*Defined at line 87*

---

## JNI_KIND_INT:

```fides
pub const JNI_KIND_INT: i64 = 5;
```

*Defined at line 88*

---

## JNI_KIND_LONG:

```fides
pub const JNI_KIND_LONG: i64 = 6;
```

*Defined at line 89*

---

## JNI_KIND_FLOAT:

```fides
pub const JNI_KIND_FLOAT: i64 = 7;
```

*Defined at line 90*

---

## JNI_KIND_DOUBLE:

```fides
pub const JNI_KIND_DOUBLE: i64 = 8;
```

*Defined at line 91*

---

## JNI_KIND_VOID:

```fides
pub const JNI_KIND_VOID: i64 = 9;
```

*Defined at line 92*

---

## JNI_FN_GET_VERSION:

```fides
pub const JNI_FN_GET_VERSION: i64 = 4;
```

*Defined at line 95*

---

## JNI_FN_FIND_CLASS:

```fides
pub const JNI_FN_FIND_CLASS: i64 = 6;
```

*Defined at line 96*

---

## JNI_FN_THROW_NEW:

```fides
pub const JNI_FN_THROW_NEW: i64 = 14;
```

*Defined at line 97*

---

## JNI_FN_EXCEPTION_OCCURRED:

```fides
pub const JNI_FN_EXCEPTION_OCCURRED: i64 = 15;
```

*Defined at line 98*

---

## JNI_FN_EXCEPTION_DESCRIBE:

```fides
pub const JNI_FN_EXCEPTION_DESCRIBE: i64 = 16;
```

*Defined at line 99*

---

## JNI_FN_EXCEPTION_CLEAR:

```fides
pub const JNI_FN_EXCEPTION_CLEAR: i64 = 17;
```

*Defined at line 100*

---

## JNI_FN_PUSH_LOCAL_FRAME:

```fides
pub const JNI_FN_PUSH_LOCAL_FRAME: i64 = 19;
```

*Defined at line 101*

---

## JNI_FN_POP_LOCAL_FRAME:

```fides
pub const JNI_FN_POP_LOCAL_FRAME: i64 = 20;
```

*Defined at line 102*

---

## JNI_FN_NEW_GLOBAL_REF:

```fides
pub const JNI_FN_NEW_GLOBAL_REF: i64 = 21;
```

*Defined at line 103*

---

## JNI_FN_DELETE_GLOBAL_REF:

```fides
pub const JNI_FN_DELETE_GLOBAL_REF: i64 = 22;
```

*Defined at line 104*

---

## JNI_FN_DELETE_LOCAL_REF:

```fides
pub const JNI_FN_DELETE_LOCAL_REF: i64 = 23;
```

*Defined at line 105*

---

## JNI_FN_NEW_OBJECT_A:

```fides
pub const JNI_FN_NEW_OBJECT_A: i64 = 30;
```

*Defined at line 106*

---

## JNI_FN_GET_OBJECT_CLASS:

```fides
pub const JNI_FN_GET_OBJECT_CLASS: i64 = 31;
```

*Defined at line 107*

---

## JNI_FN_GET_METHOD_ID:

```fides
pub const JNI_FN_GET_METHOD_ID: i64 = 33;
```

*Defined at line 108*

---

## JNI_FN_GET_FIELD_ID:

```fides
pub const JNI_FN_GET_FIELD_ID: i64 = 94;
```

*Defined at line 109*

---

## JNI_FN_GET_STATIC_METHOD_ID:

```fides
pub const JNI_FN_GET_STATIC_METHOD_ID: i64 = 113;
```

*Defined at line 110*

---

## JNI_FN_NEW_STRING:

```fides
pub const JNI_FN_NEW_STRING: i64 = 163;
```

*Defined at line 111*

---

## JNI_FN_GET_STRING_LENGTH:

```fides
pub const JNI_FN_GET_STRING_LENGTH: i64 = 164;
```

*Defined at line 112*

---

## JNI_FN_NEW_STRING_UTF:

```fides
pub const JNI_FN_NEW_STRING_UTF: i64 = 167;
```

*Defined at line 113*

---

## JNI_FN_GET_STRING_UTF_LENGTH:

```fides
pub const JNI_FN_GET_STRING_UTF_LENGTH: i64 = 168;
```

*Defined at line 114*

---

## JNI_FN_GET_STRING_UTF_CHARS:

```fides
pub const JNI_FN_GET_STRING_UTF_CHARS: i64 = 169;
```

*Defined at line 115*

---

## JNI_FN_RELEASE_STRING_UTF_CHARS:

```fides
pub const JNI_FN_RELEASE_STRING_UTF_CHARS: i64 = 170;
```

*Defined at line 116*

---

## JNI_FN_GET_ARRAY_LENGTH:

```fides
pub const JNI_FN_GET_ARRAY_LENGTH: i64 = 171;
```

*Defined at line 117*

---

## JNI_FN_REGISTER_NATIVES:

```fides
pub const JNI_FN_REGISTER_NATIVES: i64 = 215;
```

*Defined at line 118*

---

## JNI_FN_GET_JAVA_VM:

```fides
pub const JNI_FN_GET_JAVA_VM: i64 = 219;
```

*Defined at line 119*

---

## JNI_FN_NEW_WEAK_GLOBAL_REF:

```fides
pub const JNI_FN_NEW_WEAK_GLOBAL_REF: i64 = 226;
```

*Defined at line 120*

---

## JNI_FN_DELETE_WEAK_GLOBAL_REF:

```fides
pub const JNI_FN_DELETE_WEAK_GLOBAL_REF: i64 = 227;
```

*Defined at line 121*

---

## JNI_FN_EXCEPTION_CHECK:

```fides
pub const JNI_FN_EXCEPTION_CHECK: i64 = 228;
```

*Defined at line 122*

---

## JNI_FN_GET_OBJECT_REF_TYPE:

```fides
pub const JNI_FN_GET_OBJECT_REF_TYPE: i64 = 232;
```

*Defined at line 123*

---

## JNI_TABLE_LEN:

Slots in the table of JNI 21 (GetModule is 233, IsVirtualThread 234).

```fides
pub const JNI_TABLE_LEN: i64 = 235;
```

*Defined at line 125*

---

## JNI_VM_DESTROY:

```fides
pub const JNI_VM_DESTROY: i64 = 3;
```

*Defined at line 128*

---

## JNI_VM_ATTACH_CURRENT_THREAD:

```fides
pub const JNI_VM_ATTACH_CURRENT_THREAD: i64 = 4;
```

*Defined at line 129*

---

## JNI_VM_DETACH_CURRENT_THREAD:

```fides
pub const JNI_VM_DETACH_CURRENT_THREAD: i64 = 5;
```

*Defined at line 130*

---

## JNI_VM_GET_ENV:

```fides
pub const JNI_VM_GET_ENV: i64 = 6;
```

*Defined at line 131*

---

## JNI_VM_ATTACH_AS_DAEMON:

```fides
pub const JNI_VM_ATTACH_AS_DAEMON: i64 = 7;
```

*Defined at line 132*

---

## jni_table_len

Number of slots in the JNIEnv table (235 for JNI 21).

```fides
pub fn jni_table_len() -> i64 {
```

**Returns:** `i64`

*Defined at line 278*

---

## jni_fn_name

The jni.h name of slot `index` ("" when out of range).

```fides
pub fn jni_fn_name(index: i64) -> str {
```

**Parameters:**

- `index`: `i64`

**Returns:** `str`

*Defined at line 284*

---

## jni_fn_index

The slot of a JNIEnv function by its jni.h name, -1 if unknown.

```fides
pub fn jni_fn_index(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 293*

---

## jni_fn_offset

Byte offset of slot `index` in the table on a 64-bit ABI.

```fides
pub fn jni_fn_offset(index: i64) -> i64 {
```

**Parameters:**

- `index`: `i64`

**Returns:** `i64`

*Defined at line 306*

---

## jni_vm_fn_name

The JavaVM (JNIInvokeInterface_) name of slot `index`.

```fides
pub fn jni_vm_fn_name(index: i64) -> str {
```

**Parameters:**

- `index`: `i64`

**Returns:** `str`

*Defined at line 311*

---

## jni_read_table_entry

Read slot `index` of a real JNIEnv*: the table pointer is the first
word at `env`, the function pointer the index-th word of the table.

```fides
pub fn jni_read_table_entry(env: i64, index: i64) -> i64 {
```

**Parameters:**

- `env`: `i64`
- `index`: `i64`

**Returns:** `i64`

*Defined at line 335*

---

## jni_type_descriptor

Descriptor of a Java type name: "int" -> "I", "java.lang.String" ->
"Ljava/lang/String;", "byte[][]" -> "[[B". A name that is already a
descriptor of an array or class ("[I", "Lx;") is returned as it is.

```fides
pub fn jni_type_descriptor(java_type: str) -> str {
```

**Parameters:**

- `java_type`: `str`

**Returns:** `str`

*Defined at line 353*

---

## jni_method_sig

Method signature from Java type names: (["int", "java.lang.String"],
"void") -> "(ILjava/lang/String;)V".

```fides
pub fn jni_method_sig(params: Vec<str>, ret: str) -> str {
```

**Parameters:**

- `params`: `Vec<str>`
- `ret`: `str`

**Returns:** `str`

*Defined at line 389*

---

## JniSig

A parsed method signature.

```fides
pub struct JniSig {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `args` | `Vec<str>` |
| `ret` | `str` |

*Defined at line 400*

---

## jni_parse_sig

Parse "(args)ret": every argument descriptor and the return type.

```fides
pub fn jni_parse_sig(sig: str) -> JniSig {
```

**Parameters:**

- `sig`: `str`

**Returns:** `JniSig`

*Defined at line 451*

---

## jni_valid_field_desc

Is `d` one valid field descriptor (no void)?

```fides
pub fn jni_valid_field_desc(d: str) -> bool {
```

**Parameters:**

- `d`: `str`

**Returns:** `bool`

*Defined at line 483*

---

## jni_desc_kind

The kind of a descriptor (JNI_KIND_*): arrays and classes are objects.

```fides
pub fn jni_desc_kind(d: str) -> i64 {
```

**Parameters:**

- `d`: `str`

**Returns:** `i64`

*Defined at line 488*

---

## JNI_CALL_VIRTUAL:

Call style for jni_call_slot.

```fides
pub const JNI_CALL_VIRTUAL: i64 = 0;
```

*Defined at line 524*

---

## JNI_CALL_NONVIRTUAL:

```fides
pub const JNI_CALL_NONVIRTUAL: i64 = 1;
```

*Defined at line 525*

---

## JNI_CALL_STATIC:

```fides
pub const JNI_CALL_STATIC: i64 = 2;
```

*Defined at line 526*

---

## jni_call_slot

The slot of Call[Nonvirtual|Static]<Kind>MethodA for a return kind.

```fides
pub fn jni_call_slot(style: i64, kind: i64) -> i64 {
```

**Parameters:**

- `style`: `i64`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 529*

---

## jni_get_field_slot

Slot of Get<Kind>Field / GetStatic<Kind>Field (static: true).

```fides
pub fn jni_get_field_slot(is_static: bool, kind: i64) -> i64 {
```

**Parameters:**

- `is_static`: `bool`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 541*

---

## jni_set_field_slot

Slot of Set<Kind>Field / SetStatic<Kind>Field.

```fides
pub fn jni_set_field_slot(is_static: bool, kind: i64) -> i64 {
```

**Parameters:**

- `is_static`: `bool`
- `kind`: `i64`

**Returns:** `i64`

*Defined at line 549*

---

## jni_mangle

Mangle a name per the JNI spec: '/' or '.' -> '_', '_' -> "_1",
';' -> "_2", '[' -> "_3", any non-alphanumeric ASCII or non-ASCII
UTF-16 unit -> "_0xxxx".

```fides
pub fn jni_mangle(name: str) -> str {
```

**Parameters:**

- `name`: `str`

**Returns:** `str`

*Defined at line 572*

---

## jni_native_symbol

The C symbol the VM looks up for a native method: "Java_" + mangled
class + "_" + mangled method; with `sig` non-empty (an overloaded
native) "__" + the mangled argument descriptors follows.

```fides
pub fn jni_native_symbol(class_name: str, method: str, sig: str) -> str {
```

**Parameters:**

- `class_name`: `str`
- `method`: `str`
- `sig`: `str`

**Returns:** `str`

*Defined at line 599*

---

## jni_utf16_units

UTF-16 code units of a (standard UTF-8) Fides string.

```fides
pub fn jni_utf16_units(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 617*

---

## jni_utf16_to_codepoints

Code points of UTF-16 units (an unpaired surrogate is kept as is).

```fides
pub fn jni_utf16_to_codepoints(units: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `units`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 636*

---

## jni_utf16_to_str

A Fides string from UTF-16 units (what GetStringChars returns).

```fides
pub fn jni_utf16_to_str(units: Vec<i64>) -> str {
```

**Parameters:**

- `units`: `Vec<i64>`

**Returns:** `str`

*Defined at line 659*

---

## jni_mutf8_encode

Modified UTF-8 bytes of a string (no terminating NUL).

```fides
pub fn jni_mutf8_encode(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 664*

---

## JniDecoded

Decoded modified UTF-8: `ok` false on a malformed byte sequence
(including a raw NUL or a 4-byte form, which modified UTF-8 never has).

```fides
pub struct JniDecoded {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `text` | `str` |
| `units` | `Vec<i64>` |

*Defined at line 687*

---

## jni_mutf8_decode

Decode modified UTF-8 bytes into UTF-16 units and a Fides string.

```fides
pub fn jni_mutf8_decode(b: Vec<i64>) -> JniDecoded {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `JniDecoded`

*Defined at line 694*

---

## jni_cbuf

Copy bytes into a new NUL-terminated C buffer (p_malloc); the caller
frees it with jni_cbuf_free once the VM call returned.

```fides
pub fn jni_cbuf(bytes: Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 722*

---

## jni_cbuf_read

Bytes of a NUL-terminated C buffer (what GetStringUTFChars returns).

```fides
pub fn jni_cbuf_read(addr: i64) -> Vec<i64> {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 741*

---

## jni_cbuf_free

Free a buffer of jni_cbuf.

```fides
pub fn jni_cbuf_free(addr: i64) {
```

**Parameters:**

- `addr`: `i64`

*Defined at line 763*

---

## JValue

An argument of a Call*MethodA call (a jvalue union member).

```fides
pub enum JValue {
```

**Variants:**

- `Z`
- `B`
- `C`
- `S`
- `I`
- `J`
- `F`
- `D`
- `L`

*Defined at line 773*

---

## jni_f32_bits

IEEE 754 single-precision bits of x, rounded to nearest even
(what a jfloat holds).

```fides
pub fn jni_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 787*

---

## jni_jvalue_word

The 64-bit word of a jvalue holding `v` (little-endian union: the
member sits in the low bytes).

```fides
pub fn jni_jvalue_word(v: JValue) -> i64 {
```

**Parameters:**

- `v`: `JValue`

**Returns:** `i64`

*Defined at line 840*

---

## jni_jvalue_words

jvalue words of an argument list.

```fides
pub fn jni_jvalue_words(args: Vec<JValue>) -> Vec<i64> {
```

**Parameters:**

- `args`: `Vec<JValue>`

**Returns:** `Vec<i64>`

*Defined at line 851*

---

## jni_jvalue_array

A jvalue array in C memory (8 bytes per argument); free it with
jni_cbuf_free. 0 for no arguments.

```fides
pub fn jni_jvalue_array(args: Vec<JValue>) -> i64 {
```

**Parameters:**

- `args`: `Vec<JValue>`

**Returns:** `i64`

*Defined at line 863*

---

## JniEnv

A JNIEnv* with the seam that reaches it (see the header).

```fides
pub struct JniEnv {
```

**Fields:**

| Name | Type |
|------|------|
| `env` | `i64` |
| `entry` | `fn(i64, i64) -> i64` |
| `call` | `fn(i64, Vec<i64>) -> i64` |
| `calls` | `i64,  // VM calls made through this env` |
| `local_refs` | `i64,  // local references this code created and holds` |
| `global_refs` | `i64,  // global references created and not deleted` |
| `frames` | `i64,  // local frames pushed and not popped` |

*Defined at line 885*

---

## jni_env

```fides
pub fn jni_env(env: i64, entry: fn(i64, i64) -> i64, call: fn(i64, Vec<i64>) -> i64) -> JniEnv {
```

**Parameters:**

- `env`: `i64`
- `entry`: `fn(i64`
- `call`: `fn(i64`

**Returns:** `JniEnv`

*Defined at line 895*

---

## jni_invoke

Call slot `index` of the env's table with the arguments after env.

```fides
pub fn jni_invoke(e: &mut JniEnv, index: i64, rest: Vec<i64>) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `index`: `i64`
- `rest`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 902*

---

## jni_get_version

```fides
pub fn jni_get_version(e: &mut JniEnv) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`

**Returns:** `i64`

*Defined at line 965*

---

## jni_find_class

FindClass("java/lang/String"): a local reference or 0.

```fides
pub fn jni_find_class(e: &mut JniEnv, name: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `name`: `str`

**Returns:** `i64`

*Defined at line 970*

---

## jni_get_method_id

```fides
pub fn jni_get_method_id(e: &mut JniEnv, cls: i64, name: str, sig: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `cls`: `i64`
- `name`: `str`
- `sig`: `str`

**Returns:** `i64`

*Defined at line 988*

---

## jni_get_static_method_id

```fides
pub fn jni_get_static_method_id(e: &mut JniEnv, cls: i64, name: str, sig: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `cls`: `i64`
- `name`: `str`
- `sig`: `str`

**Returns:** `i64`

*Defined at line 992*

---

## jni_get_field_id

```fides
pub fn jni_get_field_id(e: &mut JniEnv, cls: i64, name: str, sig: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `cls`: `i64`
- `name`: `str`
- `sig`: `str`

**Returns:** `i64`

*Defined at line 996*

---

## jni_call_method

Call a method through its Call<Kind>MethodA slot: `target` is the
object (virtual) or the class (static); the return kind comes from the
signature. The result is the raw return word (a reference, an integer,
or float bits; 0 for void).

```fides
pub fn jni_call_method(e: &mut JniEnv, style: i64, target: i64, mid: i64, sig: str, args: Vec<JValue>) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `style`: `i64`
- `target`: `i64`
- `mid`: `i64`
- `sig`: `str`
- `args`: `Vec<JValue>`

**Returns:** `i64`

*Defined at line 1004*

---

## jni_new_string

NewStringUTF of a Fides string (converted to modified UTF-8).

```fides
pub fn jni_new_string(e: &mut JniEnv, s: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `s`: `str`

**Returns:** `i64`

*Defined at line 1022*

---

## jni_string_to_str

A jstring's text: GetStringUTFChars, decode, ReleaseStringUTFChars.

```fides
pub fn jni_string_to_str(e: &mut JniEnv, jstr: i64) -> str {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `jstr`: `i64`

**Returns:** `str`

*Defined at line 1031*

---

## jni_exception_pending

The exception check pattern after every call that may throw: when an
exception is pending, describe it (stderr / logcat) and clear it, so
the next JNI call is legal. True when there was one.

```fides
pub fn jni_exception_pending(e: &mut JniEnv) -> bool {
```

**Parameters:**

- `e`: `&mut JniEnv`

**Returns:** `bool`

*Defined at line 1047*

---

## jni_throw_new

Throw a new exception of class `cls_name` with a message.

```fides
pub fn jni_throw_new(e: &mut JniEnv, cls_name: str, msg: str) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `cls_name`: `str`
- `msg`: `str`

**Returns:** `i64`

*Defined at line 1058*

---

## jni_new_global_ref

```fides
pub fn jni_new_global_ref(e: &mut JniEnv, obj: i64) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `obj`: `i64`

**Returns:** `i64`

*Defined at line 1068*

---

## jni_delete_global_ref

```fides
pub fn jni_delete_global_ref(e: &mut JniEnv, g: i64) {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `g`: `i64`

*Defined at line 1076*

---

## jni_delete_local_ref

```fides
pub fn jni_delete_local_ref(e: &mut JniEnv, l: i64) {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `l`: `i64`

*Defined at line 1084*

---

## jni_push_local_frame

PushLocalFrame(capacity): JNI_OK or an error code.

```fides
pub fn jni_push_local_frame(e: &mut JniEnv, capacity: i64) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `capacity`: `i64`

**Returns:** `i64`

*Defined at line 1093*

---

## jni_pop_local_frame

PopLocalFrame(result): frees the frame's local references and returns
`result` as a reference in the outer frame.

```fides
pub fn jni_pop_local_frame(e: &mut JniEnv, result: i64) -> i64 {
```

**Parameters:**

- `e`: `&mut JniEnv`
- `result`: `i64`

**Returns:** `i64`

*Defined at line 1103*

---

## jni_leak_report

Does the env still hold references or frames this code created?

```fides
pub fn jni_leak_report(e: &JniEnv) -> str {
```

**Parameters:**

- `e`: `&JniEnv`

**Returns:** `str`

*Defined at line 1110*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
