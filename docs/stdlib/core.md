[All modules](index.md)

# core -- Fides Documentation

## Contents

- [println](#println) (function)
- [assert](#assert) (function)
- [assert_msg](#assert_msg) (function)
- [assert_eq](#assert_eq) (function)
- [assert_eq_msg](#assert_eq_msg) (function)
- [assert_ne](#assert_ne) (function)
- [assert_ne_msg](#assert_ne_msg) (function)
- [panic](#panic) (function)
- [Option](#Option) (enum)
- [](#) (impl)
- [Result](#Result) (enum)
- [](#) (impl)
- [](#) (impl)
- [max](#max) (function)
- [min](#min) (function)
- [abs](#abs) (function)
- [clamp](#clamp) (function)
- [create_allocator](#create_allocator) (function)
- [allocator_alloc](#allocator_alloc) (function)
- [allocator_free](#allocator_free) (function)
- [allocator_bytes_used](#allocator_bytes_used) (function)
- [Iterator](#Iterator) (trait)
- [Error](#Error) (trait)
- [rg_vec_fit](#rg_vec_fit) (function)
- [Vec_new](#Vec_new) (function)
- [Vec_new_sized](#Vec_new_sized) (function)
- [Vec_new_typed](#Vec_new_typed) (function)
- [vec_buf_free](#vec_buf_free) (function)
- [vec_box_free](#vec_box_free) (function)
- [Vec_filled_typed](#Vec_filled_typed) (function)
- [vec_tid](#vec_tid) (function)
- [vec_elem_drop](#vec_elem_drop) (function)
- [vec_elem_dup](#vec_elem_dup) (function)
- [P_TID_BOX:](#P_TID_BOX:) (const)
- [Box_dup](#Box_dup) (function)
- [Box_dup_t](#Box_dup_t) (function)
- [Box_drop_t](#Box_drop_t) (function)
- [Rc_new](#Rc_new) (function)
- [Rc_clone](#Rc_clone) (function)
- [Rc_get](#Rc_get) (function)
- [Rc_count](#Rc_count) (function)
- [Rc_drop](#Rc_drop) (function)
- [Arc_new](#Arc_new) (function)
- [Arc_clone](#Arc_clone) (function)
- [Arc_get](#Arc_get) (function)
- [Arc_count](#Arc_count) (function)
- [Arc_drop](#Arc_drop) (function)
- [Rc_new_t](#Rc_new_t) (function)
- [Rc_get_p](#Rc_get_p) (function)
- [Rc_get_t](#Rc_get_t) (function)
- [Rc_drop_t](#Rc_drop_t) (function)
- [Arc_new_t](#Arc_new_t) (function)
- [Arc_get_p](#Arc_get_p) (function)
- [Arc_get_t](#Arc_get_t) (function)
- [Arc_drop_t](#Arc_drop_t) (function)
- [Vec_push_mem](#Vec_push_mem) (function)
- [Vec_set_mem](#Vec_set_mem) (function)
- [Vec_insert_mem](#Vec_insert_mem) (function)
- [__p_mem_dup](#__p_mem_dup) (function)
- [__p_mem_drop](#__p_mem_drop) (function)
- [__p_elem_drop](#__p_elem_drop) (function)
- [__p_elem_dup](#__p_elem_dup) (function)
- [read_file](#read_file) (function)
- [env_var](#env_var) (function)
- [arg_at](#arg_at) (function)
- [read_line](#read_line) (function)
- [read_stdin](#read_stdin) (function)
- [p_zero_words](#p_zero_words) (function)
- [p_copy_words](#p_copy_words) (function)
- [P_TID_STR:](#P_TID_STR:) (const)
- [P_TID_VEC:](#P_TID_VEC:) (const)
- [P_TID_MAP:](#P_TID_MAP:) (const)
- [vec_code_word](#vec_code_word) (function)
- [vec_code](#vec_code) (function)
- [vec_esz](#vec_esz) (function)
- [vec_ld](#vec_ld) (function)
- [vec_st](#vec_st) (function)
- [p_mem_copy](#p_mem_copy) (function)
- [vec_sz_push](#vec_sz_push) (function)
- [vec_sz_get](#vec_sz_get) (function)
- [vec_sz_set](#vec_sz_set) (function)
- [vec_sz_pop](#vec_sz_pop) (function)
- [Vec_drop](#Vec_drop) (function)
- [Vec_init_typed](#Vec_init_typed) (function)
- [Vec_drop_inplace](#Vec_drop_inplace) (function)
- [Vec_init_filled](#Vec_init_filled) (function)
- [Vec_init_filled_at](#Vec_init_filled_at) (function)
- [vec_fill_at](#vec_fill_at) (function)
- [Vec_drop_stackbuf](#Vec_drop_stackbuf) (function)
- [vec_last_owner](#vec_last_owner) (function)
- [Vec_drop_strs](#Vec_drop_strs) (function)
- [Vec_drop_vecs](#Vec_drop_vecs) (function)
- [HashMap_drop](#HashMap_drop) (function)
- [Vec_share](#Vec_share) (function)
- [Vec_share_deep](#Vec_share_deep) (function)
- [vec_own](#vec_own) (function)
- [Str_drop](#Str_drop) (function)
- [Str_dup](#Str_dup) (function)
- [Vec_push](#Vec_push) (function)
- [Vec_pop](#Vec_pop) (function)
- [P_VEVCODE:](#P_VEVCODE:) (const)
- [VecV_new](#VecV_new) (function)
- [VecV_len](#VecV_len) (function)
- [vecv_esz](#vecv_esz) (function)
- [VecV_push](#VecV_push) (function)
- [VecV_get](#VecV_get) (function)
- [VecV_last](#VecV_last) (function)
- [Vec_len](#Vec_len) (function)
- [Vec_is_empty](#Vec_is_empty) (function)
- [Vec_elem_addr](#Vec_elem_addr) (function)
- [Vec_get](#Vec_get) (function)
- [vec_get_or](#vec_get_or) (function)
- [Vec_eq_words](#Vec_eq_words) (function)
- [Vec_eq_strs](#Vec_eq_strs) (function)
- [Vec_set](#Vec_set) (function)
- [Vec_clear](#Vec_clear) (function)
- [Vec_capacity](#Vec_capacity) (function)
- [Vec_contains](#Vec_contains) (function)
- [Vec_first](#Vec_first) (function)
- [Vec_last](#Vec_last) (function)
- [Vec_truncate](#Vec_truncate) (function)
- [Vec_reverse](#Vec_reverse) (function)
- [vec_raw_reverse](#vec_raw_reverse) (function)
- [Vec_extend](#Vec_extend) (function)
- [vec_raw_extend](#vec_raw_extend) (function)
- [Vec_insert](#Vec_insert) (function)
- [vec_raw_insert](#vec_raw_insert) (function)
- [Vec_remove](#Vec_remove) (function)
- [vec_raw_remove](#vec_raw_remove) (function)
- [Vec_clone](#Vec_clone) (function)
- [Vec_to_str](#Vec_to_str) (function)
- [Vec_sort](#Vec_sort) (function)
- [vec_raw_sort](#vec_raw_sort) (function)
- [Vec_sort_f64](#Vec_sort_f64) (function)
- [p_str_hash](#p_str_hash) (function)
- [p_str_equal](#p_str_equal) (function)
- [HashMap_new](#HashMap_new) (function)
- [HashMap_new_typed](#HashMap_new_typed) (function)
- [HashMap_seeded](#HashMap_seeded) (function)
- [HashMap_insert_i](#HashMap_insert_i) (function)
- [HashMap_get_i](#HashMap_get_i) (function)
- [HashMap_contains_i](#HashMap_contains_i) (function)
- [HashMap_contains_key](#HashMap_contains_key) (function)
- [HashMap_contains_key_i](#HashMap_contains_key_i) (function)
- [HashMap_remove_i](#HashMap_remove_i) (function)
- [HashMap_get_or_i](#HashMap_get_or_i) (function)
- [HashMap_keys_i](#HashMap_keys_i) (function)
- [HashMap_values_i](#HashMap_values_i) (function)
- [hm_fields_hash](#hm_fields_hash) (function)
- [hm_fields_eq](#hm_fields_eq) (function)
- [__p_key_strs](#__p_key_strs) (function)
- [HashMap_insert_mem](#HashMap_insert_mem) (function)
- [HashMap_insert_i_mem](#HashMap_insert_i_mem) (function)
- [HashMap_insert_k](#HashMap_insert_k) (function)
- [HashMap_insert_k_mem](#HashMap_insert_k_mem) (function)
- [HashMap_share](#HashMap_share) (function)
- [map_own](#map_own) (function)
- [HashMap_insert](#HashMap_insert) (function)
- [HashMap_get](#HashMap_get) (function)
- [HashMap_contains](#HashMap_contains) (function)
- [HashMap_remove](#HashMap_remove) (function)
- [HashMap_len](#HashMap_len) (function)
- [HashMap_is_empty](#HashMap_is_empty) (function)
- [HashMap_clear](#HashMap_clear) (function)
- [HashMap_keys](#HashMap_keys) (function)
- [HashMap_values](#HashMap_values) (function)
- [HashMap_clone](#HashMap_clone) (function)
- [p_sandbox_enter](#p_sandbox_enter) (function)
- [p_sandbox_exit](#p_sandbox_exit) (function)
- [p_send_copy](#p_send_copy) (function)
- [p_str_send_copy](#p_str_send_copy) (function)
- [Vec_send_copy](#Vec_send_copy) (function)
- [HashMap_send_copy](#HashMap_send_copy) (function)
- [HashMap_get_or](#HashMap_get_or) (function)
- [HashMap_to_str](#HashMap_to_str) (function)
- [p_str_from_ptr](#p_str_from_ptr) (function)
- [p_str_cstr_len](#p_str_cstr_len) (function)
- [p_str_adopt](#p_str_adopt) (function)
- [p_str_take](#p_str_take) (function)
- [p_str_hdr_len](#p_str_hdr_len) (function)
- [p_str_count_of](#p_str_count_of) (function)
- [p_str_append](#p_str_append) (function)
- [Int_to_str_into](#Int_to_str_into) (function)
- [Bool_to_str_into](#Bool_to_str_into) (function)
- [Str_concat_into](#Str_concat_into) (function)
- [p_malloc_uninit](#p_malloc_uninit) (function)
- [p_str_alloc](#p_str_alloc) (function)
- [p_str_alloc_uninit](#p_str_alloc_uninit) (function)
- [p_str_buf](#p_str_buf) (function)
- [p_str_free](#p_str_free) (function)
- [p_str_copy_n](#p_str_copy_n) (function)
- [String_from](#String_from) (function)
- [String_len](#String_len) (function)
- [String_is_empty](#String_is_empty) (function)
- [String_push_str](#String_push_str) (function)
- [String_contains](#String_contains) (function)
- [String_starts_with](#String_starts_with) (function)
- [String_ends_with](#String_ends_with) (function)
- [String_trim](#String_trim) (function)
- [String_to_uppercase](#String_to_uppercase) (function)
- [String_to_lowercase](#String_to_lowercase) (function)
- [String_slice](#String_slice) (function)
- [String_slice_len](#String_slice_len) (function)
- [String_find](#String_find) (function)
- [p_mem_find_byte](#p_mem_find_byte) (function)
- [String_rfind](#String_rfind) (function)
- [String_replace](#String_replace) (function)
- [p_bytes_eq](#p_bytes_eq) (function)
- [String_split](#String_split) (function)
- [String_parse_int](#String_parse_int) (function)
- [String_char_at](#String_char_at) (function)
- [p_str_concat_int](#p_str_concat_int) (function)
- [String_repeat](#String_repeat) (function)
- [String_to_string](#String_to_string) (function)
- [String_clone](#String_clone) (function)
- [String_bytes](#String_bytes) (function)
- [StringBuilder](#StringBuilder) (struct)
- [Vec](#Vec) (impl)
- [StringBuilder](#StringBuilder) (impl)
- [String_byte_at](#String_byte_at) (function)
- [String_char_count](#String_char_count) (function)
- [char_to_str](#char_to_str) (function)
- [char_is_digit](#char_is_digit) (function)
- [char_is_alpha](#char_is_alpha) (function)
- [char_is_whitespace](#char_is_whitespace) (function)
- [char_is_upper](#char_is_upper) (function)
- [char_is_lower](#char_is_lower) (function)
- [char_to_upper](#char_to_upper) (function)
- [char_to_lower](#char_to_lower) (function)
- [ByteBuffer](#ByteBuffer) (struct)
- [ByteBuffer](#ByteBuffer) (impl)
- [str_bytes](#str_bytes) (function)
- [str_from_bytes](#str_from_bytes) (function)
- [str_slice](#str_slice) (function)
- [str_slice_len](#str_slice_len) (function)
- [str_starts_with](#str_starts_with) (function)
- [str_ends_with](#str_ends_with) (function)
- [str_contains](#str_contains) (function)
- [str_find](#str_find) (function)
- [str_rfind](#str_rfind) (function)
- [str_trim_start](#str_trim_start) (function)
- [str_trim_end](#str_trim_end) (function)
- [str_trim](#str_trim) (function)
- [str_lines](#str_lines) (function)
- [str_trim_end_char](#str_trim_end_char) (function)
- [str_split](#str_split) (function)
- [str_split_char](#str_split_char) (function)
- [str_replace](#str_replace) (function)
- [str_clone](#str_clone) (function)
- [str_repeat](#str_repeat) (function)
- [str_to_upper](#str_to_upper) (function)
- [str_to_lower](#str_to_lower) (function)
- [str_parse_int](#str_parse_int) (function)
- [str_parse_hex](#str_parse_hex) (function)
- [str_parse_int_range](#str_parse_int_range) (function)
- [str_byte_at](#str_byte_at) (function)
- [str_len](#str_len) (function)
- [str_is_empty](#str_is_empty) (function)
- [str_concat](#str_concat) (function)
- [str_compare](#str_compare) (function)
- [str_equal](#str_equal) (function)
- [int_to_str](#int_to_str) (function)
- [u64_to_str](#u64_to_str) (function)
- [float_to_str](#float_to_str) (function)
- [str_parse_float](#str_parse_float) (function)
- [byte_to_str](#byte_to_str) (function)
- [fstr_fmt_i64](#fstr_fmt_i64) (function)
- [fstr_fmt_u64](#fstr_fmt_u64) (function)
- [fstr_fmt_f64](#fstr_fmt_f64) (function)
- [fstr_fmt_bool](#fstr_fmt_bool) (function)
- [fstr_fmt_str](#fstr_fmt_str) (function)
- [cold_path](#cold_path) (function)

## println

```fides
pub fn println(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 25*

---

## assert

```fides
pub fn assert(cond: bool) -> void {
```

**Parameters:**

- `cond`: `bool`

**Returns:** `void`

*Defined at line 31*

---

## assert_msg

```fides
pub fn assert_msg(cond: bool, msg: str) -> void {
```

**Parameters:**

- `cond`: `bool`
- `msg`: `str`

**Returns:** `void`

*Defined at line 39*

---

## assert_eq

```fides
pub fn assert_eq<T>(a: T, b: T) -> void {
```

**Parameters:**

- `a`: `T`
- `b`: `T`

**Returns:** `void`

*Defined at line 46*

---

## assert_eq_msg

```fides
pub fn assert_eq_msg<T>(a: T, b: T, msg: str) -> void {
```

**Parameters:**

- `a`: `T`
- `b`: `T`
- `msg`: `str`

**Returns:** `void`

*Defined at line 53*

---

## assert_ne

```fides
pub fn assert_ne<T>(a: T, b: T) -> void {
```

**Parameters:**

- `a`: `T`
- `b`: `T`

**Returns:** `void`

*Defined at line 60*

---

## assert_ne_msg

```fides
pub fn assert_ne_msg<T>(a: T, b: T, msg: str) -> void {
```

**Parameters:**

- `a`: `T`
- `b`: `T`
- `msg`: `str`

**Returns:** `void`

*Defined at line 67*

---

## panic

```fides
pub fn panic(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 74*

---

## Option

```fides
pub enum Option<T> {
```

**Variants:**

- `Some`
- `None`

*Defined at line 84*

---

## 

```fides
impl < T > Option {
```

**Methods:**

### is_some

```fides
pub fn is_some(self: Option<T>) -> bool {
```

### is_none

```fides
pub fn is_none(self: Option<T>) -> bool {
```

### unwrap

```fides
pub fn unwrap(self: Option<T>) -> T {
```

### unwrap_or

```fides
pub fn unwrap_or(self: Option<T>, default: T) -> T {
```

### map

```fides
pub fn map(self: Option<T>, f: fn(T) -> T) -> Option<T> {
```

### and_then

```fides
pub fn and_then(self: Option<T>, f: fn(T) -> T) -> Option<T> {
```

### or_else

```fides
pub fn or_else(self: Option<T>, default: fn() -> T) -> Option<T> {
```

### unwrap_or_else

```fides
pub fn unwrap_or_else(self: Option<T>, default: fn() -> T) -> T {
```

### expect

```fides
pub fn expect(self: Option<T>, msg: str) -> T {
```

*Defined at line 92*

---

## Result

```fides
pub enum Result<T, E> {
```

**Variants:**

- `Ok`
- `Err`

*Defined at line 198*

---

## 

```fides
impl < T, E > Result {
```

**Methods:**

### is_ok

```fides
pub fn is_ok(self: Result<T, E>) -> bool {
```

### is_err

```fides
pub fn is_err(self: Result<T, E>) -> bool {
```

### unwrap

```fides
pub fn unwrap(self: Result<T, E>) -> T {
```

### unwrap_or

```fides
pub fn unwrap_or(self: Result<T, E>, default: T) -> T {
```

### ok

```fides
pub fn ok(self: Result<T, E>) -> Option<T> {
```

### err

```fides
pub fn err(self: Result<T, E>) -> Option<E> {
```

### map

```fides
pub fn map(self: Result<T, E>, f: fn(T) -> T) -> Result<T, E> {
```

### map_err

```fides
pub fn map_err(self: Result<T, E>, f: fn(E) -> E) -> Result<T, E> {
```

### and_then

```fides
pub fn and_then(self: Result<T, E>, f: fn(T) -> T) -> Result<T, E> {
```

### unwrap_or_else

```fides
pub fn unwrap_or_else(self: Result<T, E>, default: fn() -> T) -> T {
```

### expect

```fides
pub fn expect(self: Result<T, E>, msg: str) -> T {
```

### context

For a Result whose error is a str: Err(e) -> Err("msg: e"), so an
error passed up with `?` says what was being done. Ok is returned
as it is (no allocation): the message is built only on failure.

```fides
pub fn context(self: Result<T, E>, msg: str) -> Result<T, str> {
```

### with_context

As context, with the message made by `f`, called only on Err.

```fides
pub fn with_context(self: Result<T, E>, f: fn() -> str) -> Result<T, str> {
```

*Defined at line 203*

---

## 

```fides
impl < T > Option {
```

**Methods:**

### context

Some(v) -> Ok(v); None -> Err(msg), so `?` can pass it up as an error.

```fides
pub fn context(self: Option<T>, msg: str) -> Result<T, str> {
```

*Defined at line 354*

---

## max

```fides
pub fn max(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 370*

---

## min

```fides
pub fn min(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 377*

---

## abs

```fides
pub fn abs(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 384*

---

## clamp

```fides
pub fn clamp(n: i64, lo: i64, hi: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `lo`: `i64`
- `hi`: `i64`

**Returns:** `i64`

*Defined at line 391*

---

## create_allocator

```fides
pub fn create_allocator(name: str) -> Allocator {
```

**Parameters:**

- `name`: `str`

**Returns:** `Allocator`

*Defined at line 410*

---

## allocator_alloc

```fides
pub fn allocator_alloc(alloc: &Allocator, size: i64) -> i64 {
```

**Parameters:**

- `alloc`: `&Allocator`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 414*

---

## allocator_free

```fides
pub fn allocator_free(alloc: &Allocator, ptr: i64) -> void {
```

**Parameters:**

- `alloc`: `&Allocator`
- `ptr`: `i64`

**Returns:** `void`

*Defined at line 418*

---

## allocator_bytes_used

```fides
pub fn allocator_bytes_used(alloc: &Allocator) -> i64 {
```

**Parameters:**

- `alloc`: `&Allocator`

**Returns:** `i64`

*Defined at line 422*

---

## Iterator

```fides
pub cap Iterator {
```

*Defined at line 432*

---

## Error

```fides
pub cap Error {
```

*Defined at line 441*

---

## rg_vec_fit

rg_fit for an element of the Vec `v` (the C backend's Vec_push and
Vec_set call it while a region is current).

```fides
pub fn rg_vec_fit(v: i64, e: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `e`: `i64`

**Returns:** `i64`

*Defined at line 720*

---

## Vec_new

```fides
pub fn Vec_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 751*

---

## Vec_new_sized

An empty Vec whose elements are stored at `code`'s size (D14; the
compiler calls it for a Vec<u8>, Vec<i32>, Vec<bool>, ...).

```fides
pub fn Vec_new_sized(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 773*

---

## Vec_new_typed

An empty Vec of elements stored at `code`'s size that owns them as
element type `tid` says (proposal 0004): 0 plain values, P_TID_STR,
P_TID_VEC, P_TID_MAP, or a struct/enum type the compiler numbered.

```fides
pub fn Vec_new_typed(code: i64, tid: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 780*

---

## vec_buf_free

Free a Vec's data buffer, made by vec_buf_alloc or not.

```fides
pub fn vec_buf_free(d: i64) {
```

**Parameters:**

- `d`: `i64`

*Defined at line 883*

---

## vec_box_free

Free a share box, unless it is the room in front of `data`.

```fides
pub fn vec_box_free(bx: i64, data: i64) {
```

**Parameters:**

- `bx`: `i64`
- `data`: `i64`

*Defined at line 901*

---

## Vec_filled_typed

`Vec::filled(n, x)`: `n` elements, each `x`, stored at `code`'s size and
owned as `tid` says (each element is one more owner of `x`; the caller
keeps its own). The C backend replaces the body with a memset or a word
loop (codegen_c.fd fast_core_vec_body).

```fides
pub fn Vec_filled_typed(n: i64, x: i64, code: i64, tid: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`
- `x`: `i64`
- `code`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 912*

---

## vec_tid

```fides
pub fn vec_tid(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 961*

---

## vec_elem_drop

Drop one owner of an element of type `tid` (proposal 0004).

```fides
pub fn vec_elem_drop(tid: i64, e: i64) {
```

**Parameters:**

- `tid`: `i64`
- `e`: `i64`

*Defined at line 980*

---

## vec_elem_dup

One more owner of an element of type `tid`: the element to store.

```fides
pub fn vec_elem_dup(tid: i64, e: i64) -> i64 {
```

**Parameters:**

- `tid`: `i64`
- `e`: `i64`

**Returns:** `i64`

*Defined at line 1004*

---

## P_TID_BOX:

```fides
pub const P_TID_BOX: i64 = 0x8000;
```

*Defined at line 1032*

---

## Box_dup

```fides
pub fn Box_dup(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 1035*

---

## Box_dup_t

```fides
pub fn Box_dup_t(b: i64, tid: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 1060*

---

## Box_drop_t

```fides
pub fn Box_drop_t(b: i64, tid: i64) {
```

**Parameters:**

- `b`: `i64`
- `tid`: `i64`

*Defined at line 1079*

---

## Rc_new

`Rc::new(x)`: the block [1, x], and its address.

```fides
pub fn Rc_new(val: i64) -> i64 {
```

**Parameters:**

- `val`: `i64`

**Returns:** `i64`

*Defined at line 1134*

---

## Rc_clone

One more owner of the block; the same handle.

```fides
pub fn Rc_clone(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1147*

---

## Rc_get

The word the block holds.

```fides
pub fn Rc_get(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1159*

---

## Rc_count

How many handles share the block (0 for no handle).

```fides
pub fn Rc_count(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1172*

---

## Rc_drop

One owner fewer; the last one frees the block.

```fides
pub fn Rc_drop(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 1186*

---

## Arc_new

`Arc::new(x)`: the block [1, x], and its address.

```fides
pub fn Arc_new(val: i64) -> i64 {
```

**Parameters:**

- `val`: `i64`

**Returns:** `i64`

*Defined at line 1207*

---

## Arc_clone

One more owner of the block, counted atomically; the same handle.

```fides
pub fn Arc_clone(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1219*

---

## Arc_get

The word the block holds.

```fides
pub fn Arc_get(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1227*

---

## Arc_count

How many handles share the block, read atomically (0 for no handle).

```fides
pub fn Arc_count(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1232*

---

## Arc_drop

One owner fewer, counted atomically; the last one frees the block.

```fides
pub fn Arc_drop(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 1240*

---

## Rc_new_t

`Rc::new(x)` of a payload `tid` describes: the block [1, tid, x's words].

```fides
pub fn Rc_new_t(p: i64, words: i64, tid: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `words`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 1262*

---

## Rc_get_p

The address of the payload a tagged block holds (a struct or enum's).

```fides
pub fn Rc_get_p(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1276*

---

## Rc_get_t

The handle a tagged block holds, one more owner of it for the caller.

```fides
pub fn Rc_get_t(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1284*

---

## Rc_drop_t

One owner fewer of a tagged block; the last one drops the payload as its
`tid` says (a handle's word, a struct or enum held inline), then frees.

```fides
pub fn Rc_drop_t(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 1300*

---

## Arc_new_t

`Arc::new(x)` of a payload `tid` describes: the block [1, tid, x's words],
its count atomic.

```fides
pub fn Arc_new_t(p: i64, words: i64, tid: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `words`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 1333*

---

## Arc_get_p

The address of the payload a tagged block holds (a struct or enum's).

```fides
pub fn Arc_get_p(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1346*

---

## Arc_get_t

The handle a tagged block holds, one more owner of it for the caller.

```fides
pub fn Arc_get_t(h: i64) -> i64 {
```

**Parameters:**

- `h`: `i64`

**Returns:** `i64`

*Defined at line 1351*

---

## Arc_drop_t

One owner fewer of a tagged block, counted atomically; the last one drops
the payload as its `tid` says, then frees the block.

```fides
pub fn Arc_drop_t(h: i64) {
```

**Parameters:**

- `h`: `i64`

*Defined at line 1357*

---

## Vec_push_mem

`v.push(x)` for a struct `x` of `size` bytes (the Vec takes what it
holds over).

```fides
pub fn Vec_push_mem(v: i64, x: i64, size: i64, tid: i64) {
```

**Parameters:**

- `v`: `i64`
- `x`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 1485*

---

## Vec_set_mem

`v.set(i, x)` / `v[i] = x` for a struct `x` of `size` bytes.

```fides
pub fn Vec_set_mem(v: i64, idx: i64, x: i64, size: i64, tid: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `x`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 1496*

---

## Vec_insert_mem

`v.insert(i, x)` for a struct `x` of `size` bytes.

```fides
pub fn Vec_insert_mem(v: i64, idx: i64, x: i64, size: i64, tid: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `x`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 1507*

---

## __p_mem_dup

One more owner of what the struct or enum of type `tid` at `p` holds
(its bytes were copied). The lowering writes the body.

```fides
pub fn __p_mem_dup(_tid: i64, _p: i64) {
```

**Parameters:**

- `_tid`: `i64`
- `_p`: `i64`

*Defined at line 1519*

---

## __p_mem_drop

Drop the owners held by the struct or enum of type `tid` (16 and up) at
`p`, which stays allocated. The lowering writes the body.

```fides
pub fn __p_mem_drop(_tid: i64, _p: i64) {
```

**Parameters:**

- `_tid`: `i64`
- `_p`: `i64`

*Defined at line 1524*

---

## __p_elem_drop

Drop or copy an element of a struct or enum type the compiler numbered
(16 and up). Core leaves these empty; the lowering writes their bodies
from the program's types.

```fides
pub fn __p_elem_drop(_tid: i64, _e: i64) {
```

**Parameters:**

- `_tid`: `i64`
- `_e`: `i64`

*Defined at line 1530*

---

## __p_elem_dup

```fides
pub fn __p_elem_dup(_tid: i64, _e: i64) -> i64 {
```

**Parameters:**

- `_tid`: `i64`
- `_e`: `i64`

**Returns:** `i64`

*Defined at line 1533*

---

## read_file

The contents of file `path`, or None when it cannot be read.

```fides
pub fn read_file(path: str) -> Option<str> {
```

**Parameters:**

- `path`: `str`

**Returns:** `Option<str>`

*Defined at line 1542*

---

## env_var

Environment variable `name`, or None when it is not set.

```fides
pub fn env_var(name: str) -> Option<str> {
```

**Parameters:**

- `name`: `str`

**Returns:** `Option<str>`

*Defined at line 1551*

---

## arg_at

Command-line argument `i` (0 is the program), or None past the last.

```fides
pub fn arg_at(i: i64) -> Option<str> {
```

**Parameters:**

- `i`: `i64`

**Returns:** `Option<str>`

*Defined at line 1560*

---

## read_line

The next line of standard input, or None at its end.

```fides
pub fn read_line() -> Option<str> {
```

**Returns:** `Option<str>`

*Defined at line 1569*

---

## read_stdin

The next `n` bytes of standard input, or None at its end.

```fides
pub fn read_stdin(n: i64) -> Option<str> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Option<str>`

*Defined at line 1578*

---

## p_zero_words

Zero `n` words at `p` (the compiler clears an owning struct variable
with it, proposal 0004).

```fides
pub fn p_zero_words(p: i64, n: i64) {
```

**Parameters:**

- `p`: `i64`
- `n`: `i64`

*Defined at line 1588*

---

## p_copy_words

Copy `n` words from `src` to `dst` (the compiler snapshots and restores
a fixed-size array local for txn { } with it).

```fides
pub fn p_copy_words(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 1603*

---

## P_TID_STR:

```fides
pub const P_TID_STR: i64 = 1;
```

*Defined at line 1646*

---

## P_TID_VEC:

```fides
pub const P_TID_VEC: i64 = 2;
```

*Defined at line 1647*

---

## P_TID_MAP:

```fides
pub const P_TID_MAP: i64 = 3;
```

*Defined at line 1648*

---

## vec_code_word

```fides
pub fn vec_code_word(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 1664*

---

## vec_code

```fides
pub fn vec_code(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 1674*

---

## vec_esz

```fides
pub fn vec_esz(code: i64) -> i64 {
```

**Parameters:**

- `code`: `i64`

**Returns:** `i64`

*Defined at line 1698*

---

## vec_ld

```fides
pub fn vec_ld(data: i64, i: i64, code: i64) -> i64 {
```

**Parameters:**

- `data`: `i64`
- `i`: `i64`
- `code`: `i64`

**Returns:** `i64`

*Defined at line 1714*

---

## vec_st

```fides
pub fn vec_st(data: i64, i: i64, code: i64, x: i64) {
```

**Parameters:**

- `data`: `i64`
- `i`: `i64`
- `code`: `i64`
- `x`: `i64`

*Defined at line 1785*

---

## p_mem_copy

Copy `n` bytes from `src` to `dst` (the areas do not overlap), a word at
a time. The native runtimes replace it (`pert_mem_copy`: msvcrt's
vectorized memcpy on Windows, aligned words on Linux) -- competitive
roadmap OP23; this body is what the interpreter and C run.

```fides
pub fn p_mem_copy(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 1834*

---

## vec_sz_push

```fides
pub fn vec_sz_push(v: i64, code: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `code`: `i64`
- `value`: `i64`

*Defined at line 1910*

---

## vec_sz_get

```fides
pub fn vec_sz_get(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 1917*

---

## vec_sz_set

```fides
pub fn vec_sz_set(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 1928*

---

## vec_sz_pop

```fides
pub fn vec_sz_pop(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 1943*

---

## Vec_drop

Drop one owner of a Vec (proposal 0004): its header is freed, and the
last owner of the buffer frees the buffer and drops the elements.

```fides
pub fn Vec_drop(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2050*

---

## Vec_init_typed

A Vec whose header is on the stack (LF13: a local that never leaves its
function): the header at `p` set up as Vec_new_typed's would be.

```fides
pub fn Vec_init_typed(p: i64, code: i64, tid: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `code`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 2064*

---

## Vec_drop_inplace

Drop what a Vec owns (its elements and its buffer, or one owner of a
shared buffer), but not its header: Vec_drop of a heap header, or the
whole drop of a header on the stack (LF13).

```fides
pub fn Vec_drop_inplace(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2087*

---

## Vec_init_filled

`Vec::filled(n, x)` with its header on the stack (LF13): the header at
`p` set up and filled as Vec_filled_typed's would be; the buffer is on
the heap.

```fides
pub fn Vec_init_filled(p: i64, n: i64, x: i64, code: i64, tid: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `n`: `i64`
- `x`: `i64`
- `code`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 2159*

---

## Vec_init_filled_at

`Vec::filled(n, x)` of plain values whose header and buffer are both on
the stack (LF13): header at `p`, `buf` holding n elements of `code`'s
size. The lowering passes such a Vec only to core functions that never
grow, share or free its buffer, and ends it with Vec_drop_stackbuf.

```fides
pub fn Vec_init_filled_at(p: i64, buf: i64, n: i64, x: i64, code: i64, tid: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `buf`: `i64`
- `n`: `i64`
- `x`: `i64`
- `code`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 2193*

---

## vec_fill_at

Store `n` elements, each `x`, at `data` (room for n elements of
`code`'s size): each one more owner of `x` (Vec_filled_typed's fill).

```fides
pub fn vec_fill_at(data: i64, n: i64, x: i64, code: i64, tid: i64) {
```

**Parameters:**

- `data`: `i64`
- `n`: `i64`
- `x`: `i64`
- `code`: `i64`
- `tid`: `i64`

*Defined at line 2214*

---

## Vec_drop_stackbuf

The drop of a Vec made by Vec_init_filled_at (LF13): its elements are
plain values and its buffer is on the stack, so nothing is freed; a
sensitive buffer is wiped and the header cleared (a loop makes it
again).

```fides
pub fn Vec_drop_stackbuf(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2238*

---

## vec_last_owner

Whether this Vec is the last owner of its data: sole (cap >= 0) or the
only one left in its share group.

```fides
pub fn vec_last_owner(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 2257*

---

## Vec_drop_strs

Older name: a Vec drops what it owns by its element type now.

```fides
pub fn Vec_drop_strs(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2278*

---

## Vec_drop_vecs

Older name: a Vec drops what it owns by its element type now.

```fides
pub fn Vec_drop_vecs(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2283*

---

## HashMap_drop

Drop one owner of a map (proposal 0004): its header is freed, and the
last owner of the tables drops the keys and values it owns.

```fides
pub fn HashMap_drop(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 2289*

---

## Vec_share

Copy-on-write: a second owner of a Vec's data (proposal 0004). Each owner
has its own header; a shared buffer gets a box {owners, capacity}, and the
capacity slot of each header holds -box-1 (negative, so cap >= 0 still
means "sole owner, real capacity"). The first change through one owner
copies the buffer (vec_own), the elements counted again by their type.

```fides
pub fn Vec_share(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2352*

---

## Vec_share_deep

Older name: the element type in the header says how elements are copied.

```fides
pub fn Vec_share_deep(v: i64, _kind: i64, _words: i64, _vec_mask: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `_kind`: `i64`
- `_words`: `i64`
- `_vec_mask`: `i64`

**Returns:** `i64`

*Defined at line 2436*

---

## vec_own

Before a change that writes a Vec's data (or one of its elements): if
another Vec shares the data (negative capacity: a box), this Vec takes the
buffer over as its last owner, or gets a copy of its own whose elements
have one more owner each, so the change is this Vec's alone.

```fides
pub fn vec_own(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 2444*

---

## Str_drop

Drop one owner of a string (proposal 0004): its last owner frees a
counted string. Literals, uncounted strings and null are left alone; a
string already freed through its count stops the program.

```fides
pub fn Str_drop(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 2541*

---

## Str_dup

One more owner of a string (proposal 0004); returns `s`.

```fides
pub fn Str_dup(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 2595*

---

## Vec_push

```fides
pub fn Vec_push(v: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `value`: `i64`

*Defined at line 2636*

---

## Vec_pop

The last element, taken out: the caller owns it (proposal 0004). From a
shared buffer the element stays the other owners' too, so it gets one
more owner.

```fides
pub fn Vec_pop(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2727*

---

## P_VEVCODE:

```fides
pub const P_VEVCODE: i64 = 247;
```

*Defined at line 2769*

---

## VecV_new

```fides
pub fn VecV_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 2771*

---

## VecV_len

```fides
pub fn VecV_len(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2790*

---

## vecv_esz

The words an element whose tag word is at `a` occupies: its size word
(one before the tag) holds the payload word count.

```fides
pub fn vecv_esz(a: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`

**Returns:** `i64`

*Defined at line 2805*

---

## VecV_push

Append variant `vi` with the `nwords` payload words at `elem` (a tag
word followed by the payload, an enum literal's layout) and return the
element's tag address: [nwords][vi][payload...] in the buffer, so what
the tag address points at reads exactly a fixed-mode enum does.

```fides
pub fn VecV_push(v: i64, _vi: i64, elem: i64, nwords: i64, tid: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `_vi`: `i64`
- `elem`: `i64`
- `nwords`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 2866*

---

## VecV_get

Element i's address (a walk: sequential data). Bounds-checked.

```fides
pub fn VecV_get(v: i64, i: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `i`: `i64`

**Returns:** `i64`

*Defined at line 2909*

---

## VecV_last

The last element's address (0 when empty) — what a push returned.

```fides
pub fn VecV_last(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2930*

---

## Vec_len

```fides
pub fn Vec_len(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 2955*

---

## Vec_is_empty

```fides
pub fn Vec_is_empty(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 2965*

---

## Vec_elem_addr

The address of element `idx`, for `&v[i]` and `&mut v[i]` (release D15):
the reference points into the buffer, so a write through it changes the
Vec. `write != 0` first takes the buffer for this Vec alone (it may be
shared until its first write). `is_struct != 0`: the element is a struct;
one stored in the buffer (code 16 and up) is reached at its address, one
stored boxed (a word) by the pointer in its slot. The borrow checker keeps
the Vec unchanged while the reference lives, so the address stays valid.

```fides
pub fn Vec_elem_addr(v: i64, idx: i64, write: i64, is_struct: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `write`: `i64`
- `is_struct`: `i64`

**Returns:** `i64`

*Defined at line 2982*

---

## Vec_get

```fides
pub fn Vec_get(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 3013*

---

## vec_get_or

```fides
pub fn vec_get_or(v: Vec<i64>, idx: i64, default: i64) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`
- `idx`: `i64`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 3054*

---

## Vec_eq_words

`==` on two Vecs of numbers or bools (proposal 0001): same length, and
equal element by element (word by word).

```fides
pub fn Vec_eq_words(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 3098*

---

## Vec_eq_strs

`==` on two Vec<str>: same length, and equal strings element by element.

```fides
pub fn Vec_eq_strs(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 3120*

---

## Vec_set

Element `idx` becomes `value`, which the Vec now owns; the old element is
dropped (proposal 0004).

```fides
pub fn Vec_set(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 3143*

---

## Vec_clear

Remove every element; a sole owner drops them.

```fides
pub fn Vec_clear(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3176*

---

## Vec_capacity

```fides
pub fn Vec_capacity(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 3180*

---

## Vec_contains

```fides
pub fn Vec_contains(v: i64, value: i64) -> bool {
```

**Parameters:**

- `v`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 3199*

---

## Vec_first

```fides
pub fn Vec_first(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 3247*

---

## Vec_last

```fides
pub fn Vec_last(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 3282*

---

## Vec_truncate

Keep the first `n` elements; a sole owner drops the rest (shared ones
stay the other owners').

```fides
pub fn Vec_truncate(v: i64, n: i64) {
```

**Parameters:**

- `v`: `i64`
- `n`: `i64`

*Defined at line 3320*

---

## Vec_reverse

```fides
pub fn Vec_reverse(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3348*

---

## vec_raw_reverse

```fides
pub fn vec_raw_reverse(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3353*

---

## Vec_extend

```fides
pub fn Vec_extend(v: i64, other: i64) {
```

**Parameters:**

- `v`: `i64`
- `other`: `i64`

*Defined at line 3415*

---

## vec_raw_extend

Append copies of `other`'s elements (each gets one more owner).

```fides
pub fn vec_raw_extend(v: i64, other: i64) {
```

**Parameters:**

- `v`: `i64`
- `other`: `i64`

*Defined at line 3421*

---

## Vec_insert

```fides
pub fn Vec_insert(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 3498*

---

## vec_raw_insert

```fides
pub fn vec_raw_insert(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 3503*

---

## Vec_remove

```fides
pub fn Vec_remove(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 3567*

---

## vec_raw_remove

```fides
pub fn vec_raw_remove(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 3572*

---

## Vec_clone

A Vec of its own with the same elements (each gets one more owner).

```fides
pub fn Vec_clone(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 3636*

---

## Vec_to_str

```fides
pub fn Vec_to_str(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 3674*

---

## Vec_sort

Sort ascending (elements compared as i64), in place: heapsort,
O(n log n) time and no extra memory. Doubles: Vec_sort_f64 (BG179).

```fides
pub fn Vec_sort(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3735*

---

## vec_raw_sort

```fides
pub fn vec_raw_sort(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3740*

---

## Vec_sort_f64

BG179: sort a `Vec<f64>` ascending as numbers, in place (heapsort, as
Vec_sort). Vec_sort compares the raw words as i64, which runs negative
doubles backwards; nothing in a Vec's header says its words are doubles,
so the caller (the lowering of `.sort()` on a `Vec<f64>`) picks this one.
The order is IEEE 754 totalOrder (-0.0 before 0.0) except that every NaN
goes last, whatever its sign bit (0.0 / 0.0 sets it on x86-64, not on
AArch64).

```fides
pub fn Vec_sort_f64(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 3882*

---

## p_str_hash

```fides
pub fn p_str_hash(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 3965*

---

## p_str_equal

```fides
pub fn p_str_equal(a: i64, b: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `bool`

*Defined at line 3992*

---

## HashMap_new

```fides
pub fn HashMap_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 4081*

---

## HashMap_new_typed

An empty map that owns its keys when `kown` (string keys) and its values
as element type `vtid` says (proposal 0004).

```fides
pub fn HashMap_new_typed(kown: bool, vtid: i64) -> i64 {
```

**Parameters:**

- `kown`: `bool`
- `vtid`: `i64`

**Returns:** `i64`

*Defined at line 4099*

---

## HashMap_seeded

Whether the map hashes with a seed of its own (it was flooded once).

```fides
pub fn HashMap_seeded(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 4712*

---

## HashMap_insert_i

```fides
pub fn HashMap_insert_i(m: i64, key: i64, value: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `value`: `i64`

*Defined at line 4775*

---

## HashMap_get_i

```fides
pub fn HashMap_get_i(m: i64, key: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `i64`

*Defined at line 4779*

---

## HashMap_contains_i

```fides
pub fn HashMap_contains_i(m: i64, key: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 4783*

---

## HashMap_contains_key

`m.contains_key(k)`: another name of `m.contains(k)`, as Rust spells it
(stdlib-agent-bugs 44).

```fides
pub fn HashMap_contains_key(m: i64, key: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 4789*

---

## HashMap_contains_key_i

```fides
pub fn HashMap_contains_key_i(m: i64, key: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 4793*

---

## HashMap_remove_i

```fides
pub fn HashMap_remove_i(m: i64, key: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

*Defined at line 4797*

---

## HashMap_get_or_i

```fides
pub fn HashMap_get_or_i(m: i64, key: i64, default: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 4801*

---

## HashMap_keys_i

```fides
pub fn HashMap_keys_i(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 4805*

---

## HashMap_values_i

```fides
pub fn HashMap_values_i(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 4809*

---

## hm_fields_hash

```fides
pub fn hm_fields_hash(p: i64, size: i64, strs: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `size`: `i64`
- `strs`: `i64`

**Returns:** `i64`

*Defined at line 5119*

---

## hm_fields_eq

Whether the `size`-byte keys at `a` and `b` are equal: word by word, a
string word (bit i of `strs`) by its text.

```fides
pub fn hm_fields_eq(a: i64, b: i64, size: i64, strs: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `size`: `i64`
- `strs`: `i64`

**Returns:** `bool`

*Defined at line 5142*

---

## __p_key_strs

Which words of a struct or tuple key of the element type `tid` (16 and
up, numbered by the compiler) hold strings: bit i for word i (nested
structs and tuples flattened, the first 64 words). The lowering writes
the body from the program's types; 0 (every word plain) without one.

```fides
pub fn __p_key_strs(_tid: i64) -> i64 {
```

**Parameters:**

- `_tid`: `i64`

**Returns:** `i64`

*Defined at line 5172*

---

## HashMap_insert_mem

`m.insert(key, x)` for a struct value `x` of `size` bytes and element
type `tid` (0: plain fields): the map takes the struct's owners over and
keeps its bytes in its values block (RT5).

```fides
pub fn HashMap_insert_mem(m: i64, key: i64, x: i64, size: i64, tid: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `x`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 5208*

---

## HashMap_insert_i_mem

HashMap_insert_mem with an integer key.

```fides
pub fn HashMap_insert_i_mem(m: i64, key: i64, x: i64, size: i64, tid: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `x`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 5223*

---

## HashMap_insert_k

`m.insert(key, value)` for a struct or tuple key of `ksize` bytes and
element type `ktid` (0: plain fields) at `kaddr`: the map takes the
key's owners over (a copy of its own, hm_key_box) and hashes it field by
field.

```fides
pub fn HashMap_insert_k(m: i64, kaddr: i64, value: i64, ksize: i64, ktid: i64) {
```

**Parameters:**

- `m`: `i64`
- `kaddr`: `i64`
- `value`: `i64`
- `ksize`: `i64`
- `ktid`: `i64`

*Defined at line 5231*

---

## HashMap_insert_k_mem

HashMap_insert_k with a struct value `x` of `size` bytes and element
type `tid`, kept inline (HashMap_insert_mem).

```fides
pub fn HashMap_insert_k_mem(m: i64, kaddr: i64, x: i64, ksize: i64, ktid: i64, size: i64, tid: i64) {
```

**Parameters:**

- `m`: `i64`
- `kaddr`: `i64`
- `x`: `i64`
- `ksize`: `i64`
- `ktid`: `i64`
- `size`: `i64`
- `tid`: `i64`

*Defined at line 5242*

---

## HashMap_share

```fides
pub fn HashMap_share(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 5257*

---

## map_own

```fides
pub fn map_own(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 5311*

---

## HashMap_insert

`key` maps to `value`; the map owns both (proposal 0004). An existing key
is kept and the new one dropped; the old value is dropped.

```fides
pub fn HashMap_insert(m: i64, key: i64, value: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `value`: `i64`

*Defined at line 5716*

---

## HashMap_get

```fides
pub fn HashMap_get(m: i64, key: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `i64`

*Defined at line 5850*

---

## HashMap_contains

```fides
pub fn HashMap_contains(m: i64, key: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 5904*

---

## HashMap_remove

Remove `key`; the map drops its key and value.

```fides
pub fn HashMap_remove(m: i64, key: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

*Defined at line 5938*

---

## HashMap_len

```fides
pub fn HashMap_len(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 5992*

---

## HashMap_is_empty

```fides
pub fn HashMap_is_empty(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 6002*

---

## HashMap_clear

Remove every entry; the map drops what it owns.

```fides
pub fn HashMap_clear(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 6013*

---

## HashMap_keys

The keys in a Vec of their own (sorted for a deterministic order); the
Vec owns copies of keys the map owns.

```fides
pub fn HashMap_keys(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 6051*

---

## HashMap_values

```fides
pub fn HashMap_values(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 6105*

---

## HashMap_clone

```fides
pub fn HashMap_clone(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 6134*

---

## p_sandbox_enter

```fides
pub fn p_sandbox_enter() -> i64 {
```

**Returns:** `i64`

*Defined at line 6208*

---

## p_sandbox_exit

```fides
pub fn p_sandbox_exit() -> i64 {
```

**Returns:** `i64`

*Defined at line 6212*

---

## p_send_copy

A copy of `e`, a value of element type `tid`, sharing nothing with it.

```fides
pub fn p_send_copy(e: i64, tid: i64) -> i64 {
```

**Parameters:**

- `e`: `i64`
- `tid`: `i64`

**Returns:** `i64`

*Defined at line 6225*

---

## p_str_send_copy

A new string with the bytes of `s` (a literal is never freed: kept).

```fides
pub fn p_str_send_copy(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 6247*

---

## Vec_send_copy

A Vec with copies of the elements of `v`, in a buffer of its own.

```fides
pub fn Vec_send_copy(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 6270*

---

## HashMap_send_copy

A HashMap with copies of the keys and values of `m`.

```fides
pub fn HashMap_send_copy(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 6309*

---

## HashMap_get_or

```fides
pub fn HashMap_get_or(m: i64, key: i64, default: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 6379*

---

## HashMap_to_str

```fides
pub fn HashMap_to_str(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 6436*

---

## p_str_from_ptr

```fides
pub fn p_str_from_ptr(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 6506*

---

## p_str_cstr_len

Bytes before the NUL at `s`, ignoring any header.

```fides
pub fn p_str_cstr_len(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 6517*

---

## p_str_adopt

A str made from `buf`, a raw string its caller owns and gives up (a
runtime function's result, proposal 0004): a counted string or a literal
is taken over as it is; anything else is copied, and freed when it is a
plain block of the allocator.

```fides
pub fn p_str_adopt(buf: i64) -> str {
```

**Parameters:**

- `buf`: `i64`

**Returns:** `str`

*Defined at line 6544*

---

## p_str_take

A str of its own for `s`, a raw string the caller borrows (proposal
0004): a counted string gets one more owner, a literal is itself,
anything else (a C string, a plain buffer) is copied; "" for null.

```fides
pub fn p_str_take(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 6586*

---

## p_str_hdr_len

The length in the header of `s`, or -1 when it has none.

```fides
pub fn p_str_hdr_len(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 6612*

---

## p_str_count_of

```fides
pub fn p_str_count_of(w: i64) -> i64 {
```

**Parameters:**

- `w`: `i64`

**Returns:** `i64`

*Defined at line 6648*

---

## p_str_append

`s + x` where the caller gives `s` up (`s = s + x`, `s += x`, proposal
0004): appended in place when `s` has one owner and room, else a new
string with room to grow, `s` dropped. The usual `s = s + ...` loop then
costs a copy only each time the length doubles.

```fides
pub fn p_str_append(s: i64, x: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `x`: `i64`

**Returns:** `str`

*Defined at line 6676*

---

## Int_to_str_into

`int_to_str` built in a caller's stack area `buf` of `cap` bytes (LF13,
Nov 21: a local `p_to_str` string that never leaves its function) as
[P_STR_STATIC][len] digits NUL, as Str_concat_into does for '+'. An
area that cannot be ours (or is too small for the worst i64: a minus
and 19 digits) gives the heap p_to_str(x). The value is kept <= 0 while
the digits render from the end, so INT64_MIN needs no negation.

```fides
pub fn Int_to_str_into(buf: i64, cap: i64, x: i64) -> str {
```

**Parameters:**

- `buf`: `i64`
- `cap`: `i64`
- `x`: `i64`

**Returns:** `str`

*Defined at line 6739*

---

## Bool_to_str_into

`bool_to_str` built in a caller's stack area `buf` of `cap` bytes (LF13,
Nov 22: a local `p_bool_to_str` string that never leaves its function)
as [P_STR_STATIC][len] bytes NUL, as Int_to_str_into does for digits. An
area that cannot be ours (or too small for "false": 16 header + 5 + 1)
gives the heap copy p_bool_to_str makes.

```fides
pub fn Bool_to_str_into(buf: i64, cap: i64, b: bool) -> str {
```

**Parameters:**

- `buf`: `i64`
- `cap`: `i64`
- `b`: `bool`

**Returns:** `str`

*Defined at line 6815*

---

## Str_concat_into

`a + b` built in a caller's stack area `buf` of `cap` bytes (LF13: a
local string that never leaves its function) as [P_STR_STATIC][len]
bytes NUL, so a drop leaves it alone. A result that does not fit, or an
operand that lies in the area, gives a counted heap string as
p_str_concat does.

```fides
pub fn Str_concat_into(buf: i64, cap: i64, a: i64, b: i64) -> str {
```

**Parameters:**

- `buf`: `i64`
- `cap`: `i64`
- `a`: `i64`
- `b`: `i64`

**Returns:** `str`

*Defined at line 6849*

---

## p_malloc_uninit

RT8: a block of `size` bytes as p_malloc gives (the same kind of block:
freed by p_free; counted, limited, recorded and placed in the current
region as p_malloc's are) whose bytes are unspecified, for core's
producers that write every byte they hand out. Core and the runtimes
only: a call from any other module is a type error (the type checker's
tc_core_only_builtin), so memory any other code sees reads as zero.
A runtime replaces it with its allocator's path that skips the zeroing:
the C runtime this body (codegen_c.fd fast_core_vec_body), pe_ir and
the machine layer with pert_malloc_uninit (stdlib/pe_runtime,
stdlib/linux_runtime), which every call reaches first. This body is what
the rest run (a zeroed block: the caller cannot tell) and what a stage 0
whose runtime has no such path compiles. A core function, not a
builtin: lowering passes it an environment word as to any core
function, and the runtime's pert_malloc_uninit takes one too.

```fides
pub fn p_malloc_uninit(size: i64) -> i64 {
```

**Parameters:**

- `size`: `i64`

**Returns:** `i64`

*Defined at line 6918*

---

## p_str_alloc

```fides
pub fn p_str_alloc(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 6927*

---

## p_str_alloc_uninit

RT8: p_str_alloc whose `len` bytes are not zeroed, the NUL after them
written: for core's producers that write every byte of the text (a
copy, a slice, a concatenation, a number's digits). Core and the
runtimes only (the type checker's tc_core_only_builtin): every other
string reads as zero until written.

```fides
pub fn p_str_alloc_uninit(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 6948*

---

## p_str_buf

p_str_alloc with the NUL written: the caller fills `len` bytes.

```fides
pub fn p_str_buf(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 6968*

---

## p_str_free

```fides
pub fn p_str_free(s: i64) {
```

**Parameters:**

- `s`: `i64`

*Defined at line 6983*

---

## p_str_copy_n

```fides
pub fn p_str_copy_n(dst: i64, src: i64, n: i64) {
```

**Parameters:**

- `dst`: `i64`
- `src`: `i64`
- `n`: `i64`

*Defined at line 7052*

---

## String_from

```fides
pub fn String_from(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7069*

---

## String_len

```fides
pub fn String_len(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 7079*

---

## String_is_empty

```fides
pub fn String_is_empty(s: i64) -> bool {
```

**Parameters:**

- `s`: `i64`

**Returns:** `bool`

*Defined at line 7083*

---

## String_push_str

```fides
pub fn String_push_str(s: i64, other: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `other`: `i64`

**Returns:** `str`

*Defined at line 7097*

---

## String_contains

```fides
pub fn String_contains(s: i64, sub: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `bool`

*Defined at line 7106*

---

## String_starts_with

```fides
pub fn String_starts_with(s: i64, prefix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `prefix`: `i64`

**Returns:** `bool`

*Defined at line 7110*

---

## String_ends_with

```fides
pub fn String_ends_with(s: i64, suffix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `suffix`: `i64`

**Returns:** `bool`

*Defined at line 7122*

---

## String_trim

```fides
pub fn String_trim(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7134*

---

## String_to_uppercase

```fides
pub fn String_to_uppercase(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7165*

---

## String_to_lowercase

```fides
pub fn String_to_lowercase(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7197*

---

## String_slice

```fides
pub fn String_slice(s: i64, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 7201*

---

## String_slice_len

String_slice for a caller that already knows `s` is `len` bytes long:
no strlen scan, so slicing a large source token by token stays linear.

```fides
pub fn String_slice_len(s: i64, len: i64, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `len`: `i64`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 7216*

---

## String_find

The first position of `sub` in `s`, or -1: the first byte is looked for,
then the rest compared a word at a time.

```fides
pub fn String_find(s: i64, sub: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `i64`

*Defined at line 7236*

---

## p_mem_find_byte

The index of the first byte `b` in the `n` bytes at `p`, or -1. The
native runtimes replace it (`pert_mem_find_byte`: msvcrt memchr on
Windows, a word at a time on Linux) -- OP23.

```fides
pub fn p_mem_find_byte(p: i64, n: i64, b: i64) -> i64 {
```

**Parameters:**

- `p`: `i64`
- `n`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 7269*

---

## String_rfind

```fides
pub fn String_rfind(s: i64, sub: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `i64`

*Defined at line 7300*

---

## String_replace

```fides
pub fn String_replace(s: i64, from: i64, to: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `from`: `i64`
- `to`: `i64`

**Returns:** `str`

*Defined at line 7323*

---

## p_bytes_eq

Whether the `n` bytes at `a` and `b` are equal (byte by byte: the
interpreter cannot read strings a word at a time; the C backend uses
memcmp, fast_core_vec_body).

```fides
pub fn p_bytes_eq(a: i64, b: i64, n: i64) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

**Returns:** `bool`

*Defined at line 7369*

---

## String_split

```fides
pub fn String_split(s: i64, delim: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `delim`: `i64`

**Returns:** `i64`

*Defined at line 7391*

---

## String_parse_int

```fides
pub fn String_parse_int(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 7418*

---

## String_char_at

```fides
pub fn String_char_at(s: i64, idx: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `idx`: `i64`

**Returns:** `str`

*Defined at line 7472*

---

## p_str_concat_int

`s + int_to_str(n)` in one allocation: the lowering's form of that
expression (no string for the number alone, made and freed again).

```fides
pub fn p_str_concat_int(s: i64, n: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `n`: `i64`

**Returns:** `str`

*Defined at line 7485*

---

## String_repeat

```fides
pub fn String_repeat(s: i64, n: i64) -> str {
```

**Parameters:**

- `s`: `i64`
- `n`: `i64`

**Returns:** `str`

*Defined at line 7534*

---

## String_to_string

```fides
pub fn String_to_string(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7548*

---

## String_clone

```fides
pub fn String_clone(s: i64) -> str {
```

**Parameters:**

- `s`: `i64`

**Returns:** `str`

*Defined at line 7552*

---

## String_bytes

```fides
pub fn String_bytes(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 7556*

---

## StringBuilder

```fides
pub struct StringBuilder {
```

**Fields:**

| Name | Type |
|------|------|
| `handle` | `i64` |
| `len` | `i64` |

*Defined at line 7571*

---

## Vec

The closure methods of Vec that reference §13.3 names (map, filter,
for_each, fold, any, all, find, position), in the prelude so they exist
without `use vec;` (the rest of them are stdlib/vec.fd's). Each element
and each result travels as one 64-bit word: an integer, a bool, a
string's address, a handle.

```fides
impl Vec {
```

**Methods:**

### map

```fides
pub fn map(self: Vec<i64>, f: fn(i64) -> i64) -> Vec<i64> {
```

### filter

```fides
pub fn filter(self: Vec<i64>, pred: fn(i64) -> bool) -> Vec<i64> {
```

### for_each

```fides
pub fn for_each(self: Vec<i64>, f: fn(i64) -> void) -> void {
```

### fold

```fides
pub fn fold(self: Vec<i64>, init: i64, f: fn(i64, i64) -> i64) -> i64 {
```

### any

```fides
pub fn any(self: Vec<i64>, pred: fn(i64) -> bool) -> bool {
```

### all

```fides
pub fn all(self: Vec<i64>, pred: fn(i64) -> bool) -> bool {
```

### find

```fides
pub fn find(self: Vec<i64>, pred: fn(i64) -> bool) -> i64 {
```

### position

```fides
pub fn position(self: Vec<i64>, pred: fn(i64) -> bool) -> i64 {
```

*Defined at line 7581*

---

## StringBuilder

```fides
impl StringBuilder {
```

**Methods:**

### new

```fides
pub fn new() -> StringBuilder {
```

### append_str

```fides
pub fn append_str(self: &mut StringBuilder, s: str) -> void {
```

### append_char

```fides
pub fn append_char(self: &mut StringBuilder, c: i64) -> void {
```

### append_int

```fides
pub fn append_int(self: &mut StringBuilder, n: i64) -> void {
```

### to_str

```fides
pub fn to_str(self: StringBuilder) -> str {
```

### len

```fides
pub fn len(self: StringBuilder) -> i64 {
```

### is_empty

```fides
pub fn is_empty(self: StringBuilder) -> bool {
```

### clear

```fides
pub fn clear(self: &mut StringBuilder) -> void {
```

*Defined at line 7697*

---

## String_byte_at

```fides
pub fn String_byte_at(s: i64, idx: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 7742*

---

## String_char_count

```fides
pub fn String_char_count(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 7783*

---

## char_to_str

```fides
pub fn char_to_str(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 7787*

---

## char_is_digit

```fides
pub fn char_is_digit(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 7791*

---

## char_is_alpha

```fides
pub fn char_is_alpha(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 7795*

---

## char_is_whitespace

```fides
pub fn char_is_whitespace(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 7799*

---

## char_is_upper

```fides
pub fn char_is_upper(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 7803*

---

## char_is_lower

```fides
pub fn char_is_lower(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 7807*

---

## char_to_upper

```fides
pub fn char_to_upper(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 7811*

---

## char_to_lower

```fides
pub fn char_to_lower(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 7818*

---

## ByteBuffer

```fides
pub struct ByteBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `len` | `i64` |

*Defined at line 7831*

---

## ByteBuffer

```fides
impl ByteBuffer {
```

**Methods:**

### new

```fides
pub fn new(_init_cap: i64) -> ByteBuffer {
```

### from_string

```fides
pub fn from_string(s: str) -> ByteBuffer {
```

### write_byte

```fides
pub fn write_byte(self: &mut ByteBuffer, b: i64) -> void {
```

### write_bytes

```fides
pub fn write_bytes(self: &mut ByteBuffer, src: i64, count: i64) -> void {
```

### write_vec

Append the low byte of each element of `bytes` (the safe form of
write_bytes: no raw address).

```fides
pub fn write_vec(self: &mut ByteBuffer, bytes: Vec<i64>) -> void {
```

### read_byte

```fides
pub fn read_byte(self: ByteBuffer, idx: i64) -> i64 {
```

### set_byte

```fides
pub fn set_byte(self: &mut ByteBuffer, idx: i64, b: i64) -> void {
```

### len

```fides
pub fn len(self: ByteBuffer) -> i64 {
```

### is_empty

```fides
pub fn is_empty(self: ByteBuffer) -> bool {
```

### clear

```fides
pub fn clear(self: &mut ByteBuffer) -> void {
```

### to_str

```fides
pub fn to_str(self: ByteBuffer) -> str {
```

*Defined at line 7836*

---

## str_bytes

Iterate over the bytes of a string, returning a Vec of byte values.
Usage: `value bytes = str_bytes("hello");  // [104, 101, 108, 108, 111]`

```fides
pub fn str_bytes(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 7941*

---

## str_from_bytes

Build a string from a Vec of byte values.
Linear: one counted allocation + a byte copy (was a concat per byte,
quadratic — a multi-MB body took minutes).

```fides
pub fn str_from_bytes(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 7962*

---

## str_slice

Get a substring by byte range [start, end).

```fides
pub fn str_slice(s: str, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 7981*

---

## str_slice_len

str_slice when the length of `s` is already known (`len` bytes).

```fides
pub fn str_slice_len(s: str, len: i64, start: i64, end: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `len`: `i64`
- `start`: `i64`
- `end`: `i64`

**Returns:** `str`

*Defined at line 7998*

---

## str_starts_with

Check if a string starts with a prefix.

```fides
pub fn str_starts_with(s: str, prefix: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `prefix`: `str`

**Returns:** `bool`

*Defined at line 8003*

---

## str_ends_with

Check if a string ends with a suffix.

```fides
pub fn str_ends_with(s: str, suffix: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `suffix`: `str`

**Returns:** `bool`

*Defined at line 8008*

---

## str_contains

Check if a string contains a substring.

```fides
pub fn str_contains(s: str, sub: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `bool`

*Defined at line 8013*

---

## str_find

Find the first index of a substring, or -1 if not found.

```fides
pub fn str_find(s: str, sub: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `i64`

*Defined at line 8018*

---

## str_rfind

Find the last index of a substring, or -1 if not found.

```fides
pub fn str_rfind(s: str, sub: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `i64`

*Defined at line 8023*

---

## str_trim_start

Trim leading whitespace from a string.

```fides
pub fn str_trim_start(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8028*

---

## str_trim_end

Trim trailing whitespace from a string.

```fides
pub fn str_trim_end(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8051*

---

## str_trim

Trim both leading and trailing whitespace.

```fides
pub fn str_trim(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8074*

---

## str_lines

Split a string into lines by newline (LF).

```fides
pub fn str_lines(s: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<str>`

*Defined at line 8079*

---

## str_trim_end_char

Trim trailing copies of `ch` (plus spaces/tabs) from the end.

```fides
pub fn str_trim_end_char(s: str, ch: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `ch`: `i64`

**Returns:** `str`

*Defined at line 8098*

---

## str_split

Split a string by delimiter, returning a Vec of string parts.

```fides
pub fn str_split(s: str, delim: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `delim`: `str`

**Returns:** `Vec<str>`

*Defined at line 8115*

---

## str_split_char

Split a string by a single character delimiter, returning a Vec of parts.
Faster than str_split for single-char delimiters.

```fides
pub fn str_split_char(s: str, delim: i64) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `delim`: `i64`

**Returns:** `Vec<str>`

*Defined at line 8161*

---

## str_replace

Replace all occurrences of `from` with `to` in a string.

```fides
pub fn str_replace(s: str, from: str, to: str) -> str {
```

**Parameters:**

- `s`: `str`
- `from`: `str`
- `to`: `str`

**Returns:** `str`

*Defined at line 8185*

---

## str_clone

An owned copy of a string: how a borrowed `&str` is kept past its owner
(release M11). Always a new allocation.

```fides
pub fn str_clone(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8191*

---

## str_repeat

Repeat a string n times.

```fides
pub fn str_repeat(s: str, n: i64) -> str {
```

**Parameters:**

- `s`: `str`
- `n`: `i64`

**Returns:** `str`

*Defined at line 8197*

---

## str_to_upper

Convert a string to uppercase.

```fides
pub fn str_to_upper(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8202*

---

## str_to_lower

Convert a string to lowercase.

```fides
pub fn str_to_lower(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 8207*

---

## str_parse_int

Parse a decimal integer from a string. Returns 0 on failure.

```fides
pub fn str_parse_int(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 8212*

---

## str_parse_hex

```fides
pub fn str_parse_hex(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 8219*

---

## str_parse_int_range

Parse an integer from a substring [start, end) of a string.
Useful for the lexer which has already extracted digit positions.

```fides
pub fn str_parse_int_range(s: str, start: i64, end: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `start`: `i64`
- `end`: `i64`

**Returns:** `i64`

*Defined at line 8307*

---

## str_byte_at

Get the byte at a given index, or -1 if out of bounds.

```fides
pub fn str_byte_at(s: str, idx: i64) -> i64 {
```

**Parameters:**

- `s`: `str`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 8340*

---

## str_len

Get the length of a string in bytes.

```fides
pub fn str_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 8345*

---

## str_is_empty

Check if a string is empty.

```fides
pub fn str_is_empty(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 8350*

---

## str_concat

Concatenate two strings.

```fides
pub fn str_concat(a: str, b: str) -> str {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `str`

*Defined at line 8355*

---

## str_compare

Compare two strings. Returns 0 if equal, negative if a < b, positive if a > b.

```fides
pub fn str_compare(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 8360*

---

## str_equal

Check if two strings are equal.

```fides
pub fn str_equal(a: str, b: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `bool`

*Defined at line 8365*

---

## int_to_str

Convert an integer to a string.

```fides
pub fn int_to_str(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 8370*

---

## u64_to_str

A u64 as unsigned decimal text (`print` of a u64 uses this).

```fides
pub fn u64_to_str(x: u64) -> str {
```

**Parameters:**

- `x`: `u64`

**Returns:** `str`

*Defined at line 8375*

---

## float_to_str

Convert a float to a string (SH-4.1).

```fides
pub fn float_to_str(f: f64) -> str {
```

**Parameters:**

- `f`: `f64`

**Returns:** `str`

*Defined at line 8385*

---

## str_parse_float

Parse a float from a string. Returns 0.0 on failure (SH-4.1).

```fides
pub fn str_parse_float(s: str) -> f64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `f64`

*Defined at line 8390*

---

## byte_to_str

Convert a byte value to a single-character string.

```fides
pub fn byte_to_str(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 8395*

---

## fstr_fmt_i64

```fides
pub fn fstr_fmt_i64(v: i64, spec: str) -> str {
```

**Parameters:**

- `v`: `i64`
- `spec`: `str`

**Returns:** `str`

*Defined at line 8606*

---

## fstr_fmt_u64

```fides
pub fn fstr_fmt_u64(v: u64, spec: str) -> str {
```

**Parameters:**

- `v`: `u64`
- `spec`: `str`

**Returns:** `str`

*Defined at line 8633*

---

## fstr_fmt_f64

```fides
pub fn fstr_fmt_f64(v: f64, spec: str) -> str {
```

**Parameters:**

- `v`: `f64`
- `spec`: `str`

**Returns:** `str`

*Defined at line 8650*

---

## fstr_fmt_bool

```fides
pub fn fstr_fmt_bool(b: bool, spec: str) -> str {
```

**Parameters:**

- `b`: `bool`
- `spec`: `str`

**Returns:** `str`

*Defined at line 8705*

---

## fstr_fmt_str

A str: `.n` truncates to n bytes, the rest is padding.

```fides
pub fn fstr_fmt_str(s: str, spec: str) -> str {
```

**Parameters:**

- `s`: `str`
- `spec`: `str`

**Returns:** `str`

*Defined at line 8714*

---

## cold_path

OP15: a hint that the code calling it is the unlikely side (Rust's
`std::hint::cold_path`): `if bad { cold_path(); ... }`. It does nothing;
the native backends lay out a block that calls it, or calls a `@cold`
function, after the hot code (compiler/ast.fd `is_cold_fn_hint`).

```fides
pub fn cold_path() {
```

*Defined at line 8727*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
