# core — Fides Documentation

## Contents

- [println](#println) (function)
- [assert](#assert) (function)
- [assert_msg](#assert_msg) (function)
- [assert_eq](#assert_eq) (function)
- [assert_eq_msg](#assert_eq_msg) (function)
- [Option](#option) (enum)
- [Option](#option) (impl)
- [Result](#result) (enum)
- [Result](#result) (impl)
- [max](#max) (function)
- [min](#min) (function)
- [abs](#abs) (function)
- [clamp](#clamp) (function)
- [create_allocator](#create_allocator) (function)
- [allocator_alloc](#allocator_alloc) (function)
- [allocator_free](#allocator_free) (function)
- [allocator_bytes_used](#allocator_bytes_used) (function)
- [Iterator](#iterator) (trait)
- [Error](#error) (trait)
- [Vec_new](#vec_new) (function)
- [Vec_push](#vec_push) (function)
- [Vec_pop](#vec_pop) (function)
- [Vec_len](#vec_len) (function)
- [Vec_is_empty](#vec_is_empty) (function)
- [Vec_get](#vec_get) (function)
- [Vec_set](#vec_set) (function)
- [Vec_clear](#vec_clear) (function)
- [Vec_capacity](#vec_capacity) (function)
- [Vec_contains](#vec_contains) (function)
- [Vec_first](#vec_first) (function)
- [Vec_last](#vec_last) (function)
- [Vec_truncate](#vec_truncate) (function)
- [Vec_reverse](#vec_reverse) (function)
- [Vec_extend](#vec_extend) (function)
- [Vec_insert](#vec_insert) (function)
- [Vec_remove](#vec_remove) (function)
- [Vec_clone](#vec_clone) (function)
- [Vec_to_str](#vec_to_str) (function)
- [Vec_sort](#vec_sort) (function)
- [HashMap_new](#hashmap_new) (function)
- [HashMap_insert](#hashmap_insert) (function)
- [HashMap_get](#hashmap_get) (function)
- [HashMap_contains](#hashmap_contains) (function)
- [HashMap_remove](#hashmap_remove) (function)
- [HashMap_len](#hashmap_len) (function)
- [HashMap_is_empty](#hashmap_is_empty) (function)
- [HashMap_clear](#hashmap_clear) (function)
- [HashMap_keys](#hashmap_keys) (function)
- [HashMap_values](#hashmap_values) (function)
- [HashMap_clone](#hashmap_clone) (function)
- [HashMap_get_or](#hashmap_get_or) (function)
- [HashMap_to_str](#hashmap_to_str) (function)
- [String_from](#string_from) (function)
- [String_len](#string_len) (function)
- [String_is_empty](#string_is_empty) (function)
- [String_push_str](#string_push_str) (function)
- [String_contains](#string_contains) (function)
- [String_starts_with](#string_starts_with) (function)
- [String_ends_with](#string_ends_with) (function)
- [String_trim](#string_trim) (function)
- [String_to_uppercase](#string_to_uppercase) (function)
- [String_to_lowercase](#string_to_lowercase) (function)
- [String_slice](#string_slice) (function)
- [String_find](#string_find) (function)
- [String_rfind](#string_rfind) (function)
- [String_replace](#string_replace) (function)
- [String_split](#string_split) (function)
- [String_parse_int](#string_parse_int) (function)
- [String_char_at](#string_char_at) (function)
- [String_repeat](#string_repeat) (function)
- [String_to_string](#string_to_string) (function)
- [String_clone](#string_clone) (function)
- [String_bytes](#string_bytes) (function)
- [StringBuilder](#stringbuilder) (struct)
- [StringBuilder](#stringbuilder) (impl)
- [String_byte_at](#string_byte_at) (function)
- [String_char_count](#string_char_count) (function)
- [char_to_str](#char_to_str) (function)
- [char_is_digit](#char_is_digit) (function)
- [char_is_alpha](#char_is_alpha) (function)
- [char_is_whitespace](#char_is_whitespace) (function)
- [char_is_upper](#char_is_upper) (function)
- [char_is_lower](#char_is_lower) (function)
- [char_to_upper](#char_to_upper) (function)
- [char_to_lower](#char_to_lower) (function)
- [ByteBuffer](#bytebuffer) (struct)
- [ByteBuffer](#bytebuffer) (impl)
- [str_bytes](#str_bytes) (function)
- [str_from_bytes](#str_from_bytes) (function)
- [str_slice](#str_slice) (function)
- [str_starts_with](#str_starts_with) (function)
- [str_ends_with](#str_ends_with) (function)
- [str_contains](#str_contains) (function)
- [str_find](#str_find) (function)
- [str_rfind](#str_rfind) (function)
- [str_trim_start](#str_trim_start) (function)
- [str_trim_end](#str_trim_end) (function)
- [str_trim](#str_trim) (function)
- [str_split](#str_split) (function)
- [str_split_char](#str_split_char) (function)
- [str_replace](#str_replace) (function)
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
- [float_to_str](#float_to_str) (function)
- [str_parse_float](#str_parse_float) (function)
- [byte_to_str](#byte_to_str) (function)

## println

```fides
pub fn println(s: str) -> void {
```

**Parameters:**

- `s`: `str`

**Returns:** `void`

*Defined at line 10*

---

## assert

```fides
pub fn assert(cond: bool) -> void {
```

**Parameters:**

- `cond`: `bool`

**Returns:** `void`

*Defined at line 16*

---

## assert_msg

```fides
pub fn assert_msg(cond: bool, msg: str) -> void {
```

**Parameters:**

- `cond`: `bool`
- `msg`: `str`

**Returns:** `void`

*Defined at line 24*

---

## assert_eq

```fides
pub fn assert_eq<T>(a: T, b: T) -> void {
```

**Parameters:**

- `a`: `T`
- `b`: `T`

**Returns:** `void`

*Defined at line 31*

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

*Defined at line 38*

---

## Option

```fides
pub enum Option<T> {
```

**Variants:**

- `pub`
- `Some`
- `None`

*Defined at line 48*

---

## Option

```fides
impl Option {
```

*Defined at line 53*

---

## Result

```fides
pub enum Result<T, E> {
```

**Variants:**

- `pub`
- `Ok`
- `Err`

*Defined at line 126*

---

## Result

```fides
impl Result {
```

*Defined at line 131*

---

## max

```fides
pub fn max(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 216*

---

## min

```fides
pub fn min(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 221*

---

## abs

```fides
pub fn abs(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 226*

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

*Defined at line 231*

---

## create_allocator

```fides
pub fn create_allocator(name: str) -> Allocator {
```

**Parameters:**

- `name`: `str`

**Returns:** `Allocator`

*Defined at line 246*

---

## allocator_alloc

```fides
pub fn allocator_alloc(alloc: &Allocator, size: i64) -> i64 {
```

**Parameters:**

- `alloc`: `&Allocator`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 250*

---

## allocator_free

```fides
pub fn allocator_free(alloc: &Allocator, ptr: i64) -> void {
```

**Parameters:**

- `alloc`: `&Allocator`
- `ptr`: `i64`

**Returns:** `void`

*Defined at line 254*

---

## allocator_bytes_used

```fides
pub fn allocator_bytes_used(alloc: &Allocator) -> i64 {
```

**Parameters:**

- `alloc`: `&Allocator`

**Returns:** `i64`

*Defined at line 258*

---

## Iterator

```fides
pub cap Iterator {
```

*Defined at line 268*

---

## Error

```fides
pub cap Error {
```

*Defined at line 277*

---

## Vec_new

```fides
pub fn Vec_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 292*

---

## Vec_push

```fides
pub fn Vec_push(v: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `value`: `i64`

*Defined at line 305*

---

## Vec_pop

```fides
pub fn Vec_pop(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 366*

---

## Vec_len

```fides
pub fn Vec_len(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 399*

---

## Vec_is_empty

```fides
pub fn Vec_is_empty(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 408*

---

## Vec_get

```fides
pub fn Vec_get(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 417*

---

## Vec_set

```fides
pub fn Vec_set(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 447*

---

## Vec_clear

```fides
pub fn Vec_clear(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 475*

---

## Vec_capacity

```fides
pub fn Vec_capacity(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 482*

---

## Vec_contains

```fides
pub fn Vec_contains(v: i64, value: i64) -> bool {
```

**Parameters:**

- `v`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 491*

---

## Vec_first

```fides
pub fn Vec_first(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 521*

---

## Vec_last

```fides
pub fn Vec_last(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 547*

---

## Vec_truncate

```fides
pub fn Vec_truncate(v: i64, n: i64) {
```

**Parameters:**

- `v`: `i64`
- `n`: `i64`

*Defined at line 574*

---

## Vec_reverse

```fides
pub fn Vec_reverse(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 587*

---

## Vec_extend

```fides
pub fn Vec_extend(v: i64, other: i64) {
```

**Parameters:**

- `v`: `i64`
- `other`: `i64`

*Defined at line 617*

---

## Vec_insert

```fides
pub fn Vec_insert(v: i64, idx: i64, value: i64) {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`
- `value`: `i64`

*Defined at line 644*

---

## Vec_remove

```fides
pub fn Vec_remove(v: i64, idx: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 694*

---

## Vec_clone

```fides
pub fn Vec_clone(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 746*

---

## Vec_to_str

```fides
pub fn Vec_to_str(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 775*

---

## Vec_sort

```fides
pub fn Vec_sort(v: i64) {
```

**Parameters:**

- `v`: `i64`

*Defined at line 809*

---

## HashMap_new

```fides
pub fn HashMap_new() -> i64 {
```

**Returns:** `i64`

*Defined at line 933*

---

## HashMap_insert

```fides
pub fn HashMap_insert(m: i64, key: i64, value: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`
- `value`: `i64`

*Defined at line 1084*

---

## HashMap_get

```fides
pub fn HashMap_get(m: i64, key: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `i64`

*Defined at line 1143*

---

## HashMap_contains

```fides
pub fn HashMap_contains(m: i64, key: i64) -> bool {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

**Returns:** `bool`

*Defined at line 1179*

---

## HashMap_remove

```fides
pub fn HashMap_remove(m: i64, key: i64) {
```

**Parameters:**

- `m`: `i64`
- `key`: `i64`

*Defined at line 1202*

---

## HashMap_len

```fides
pub fn HashMap_len(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 1238*

---

## HashMap_is_empty

```fides
pub fn HashMap_is_empty(m: i64) -> bool {
```

**Parameters:**

- `m`: `i64`

**Returns:** `bool`

*Defined at line 1247*

---

## HashMap_clear

```fides
pub fn HashMap_clear(m: i64) {
```

**Parameters:**

- `m`: `i64`

*Defined at line 1256*

---

## HashMap_keys

```fides
pub fn HashMap_keys(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 1282*

---

## HashMap_values

```fides
pub fn HashMap_values(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 1311*

---

## HashMap_clone

```fides
pub fn HashMap_clone(m: i64) -> i64 {
```

**Parameters:**

- `m`: `i64`

**Returns:** `i64`

*Defined at line 1327*

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

*Defined at line 1363*

---

## HashMap_to_str

```fides
pub fn HashMap_to_str(m: i64) -> str {
```

**Parameters:**

- `m`: `i64`

**Returns:** `str`

*Defined at line 1402*

---

## String_from

```fides
pub fn String_from(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1527*

---

## String_len

```fides
pub fn String_len(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1543*

---

## String_is_empty

```fides
pub fn String_is_empty(s: i64) -> bool {
```

**Parameters:**

- `s`: `i64`

**Returns:** `bool`

*Defined at line 1547*

---

## String_push_str

```fides
pub fn String_push_str(s: i64, other: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `other`: `i64`

**Returns:** `i64`

*Defined at line 1555*

---

## String_contains

```fides
pub fn String_contains(s: i64, sub: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `bool`

*Defined at line 1567*

---

## String_starts_with

```fides
pub fn String_starts_with(s: i64, prefix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `prefix`: `i64`

**Returns:** `bool`

*Defined at line 1600*

---

## String_ends_with

```fides
pub fn String_ends_with(s: i64, suffix: i64) -> bool {
```

**Parameters:**

- `s`: `i64`
- `suffix`: `i64`

**Returns:** `bool`

*Defined at line 1623*

---

## String_trim

```fides
pub fn String_trim(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1648*

---

## String_to_uppercase

```fides
pub fn String_to_uppercase(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1704*

---

## String_to_lowercase

```fides
pub fn String_to_lowercase(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1731*

---

## String_slice

```fides
pub fn String_slice(s: i64, start: i64, end: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `start`: `i64`
- `end`: `i64`

**Returns:** `i64`

*Defined at line 1758*

---

## String_find

```fides
pub fn String_find(s: i64, sub: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `i64`

*Defined at line 1785*

---

## String_rfind

```fides
pub fn String_rfind(s: i64, sub: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `sub`: `i64`

**Returns:** `i64`

*Defined at line 1818*

---

## String_replace

```fides
pub fn String_replace(s: i64, from: i64, to: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `from`: `i64`
- `to`: `i64`

**Returns:** `i64`

*Defined at line 1851*

---

## String_split

```fides
pub fn String_split(s: i64, delim: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `delim`: `i64`

**Returns:** `i64`

*Defined at line 1913*

---

## String_parse_int

```fides
pub fn String_parse_int(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 1965*

---

## String_char_at

```fides
pub fn String_char_at(s: i64, idx: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 2001*

---

## String_repeat

```fides
pub fn String_repeat(s: i64, n: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 2018*

---

## String_to_string

```fides
pub fn String_to_string(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 2038*

---

## String_clone

```fides
pub fn String_clone(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 2042*

---

## String_bytes

```fides
pub fn String_bytes(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 2046*

---

## StringBuilder

```fides
pub struct StringBuilder {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `str` |
| `len` | `i64` |

*Defined at line 2061*

---

## StringBuilder

```fides
impl StringBuilder {
```

*Defined at line 2066*

---

## String_byte_at

```fides
pub fn String_byte_at(s: i64, idx: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`
- `idx`: `i64`

**Returns:** `i64`

*Defined at line 2108*

---

## String_char_count

```fides
pub fn String_char_count(s: i64) -> i64 {
```

**Parameters:**

- `s`: `i64`

**Returns:** `i64`

*Defined at line 2122*

---

## char_to_str

```fides
pub fn char_to_str(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 2126*

---

## char_is_digit

```fides
pub fn char_is_digit(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 2130*

---

## char_is_alpha

```fides
pub fn char_is_alpha(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 2134*

---

## char_is_whitespace

```fides
pub fn char_is_whitespace(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 2138*

---

## char_is_upper

```fides
pub fn char_is_upper(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 2142*

---

## char_is_lower

```fides
pub fn char_is_lower(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 2146*

---

## char_to_upper

```fides
pub fn char_to_upper(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 2150*

---

## char_to_lower

```fides
pub fn char_to_lower(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 2155*

---

## ByteBuffer

```fides
pub struct ByteBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec` |
| `len` | `i64` |

*Defined at line 2166*

---

## ByteBuffer

```fides
impl ByteBuffer {
```

*Defined at line 2171*

---

## str_bytes

Iterate over the bytes of a string, returning a Vec of byte values.
Usage: `value bytes = str_bytes("hello");  // [104, 101, 108, 108, 111]`

```fides
pub fn str_bytes(s: str) -> Vec {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec`

*Defined at line 2250*

---

## str_from_bytes

Build a string from a Vec of byte values.

```fides
pub fn str_from_bytes(bytes: Vec) -> str {
```

**Parameters:**

- `bytes`: `Vec`

**Returns:** `str`

*Defined at line 2266*

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

*Defined at line 2278*

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

*Defined at line 2289*

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

*Defined at line 2294*

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

*Defined at line 2299*

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

*Defined at line 2304*

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

*Defined at line 2309*

---

## str_trim_start

Trim leading whitespace from a string.

```fides
pub fn str_trim_start(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2314*

---

## str_trim_end

Trim trailing whitespace from a string.

```fides
pub fn str_trim_end(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2330*

---

## str_trim

Trim both leading and trailing whitespace.

```fides
pub fn str_trim(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2346*

---

## str_split

Split a string by delimiter, returning a Vec of string parts.

```fides
pub fn str_split(s: str, delim: str) -> Vec {
```

**Parameters:**

- `s`: `str`
- `delim`: `str`

**Returns:** `Vec`

*Defined at line 2351*

---

## str_split_char

Split a string by a single character delimiter, returning a Vec of parts.
Faster than str_split for single-char delimiters.

```fides
pub fn str_split_char(s: str, delim: i64) -> Vec {
```

**Parameters:**

- `s`: `str`
- `delim`: `i64`

**Returns:** `Vec`

*Defined at line 2389*

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

*Defined at line 2410*

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

*Defined at line 2415*

---

## str_to_upper

Convert a string to uppercase.

```fides
pub fn str_to_upper(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2420*

---

## str_to_lower

Convert a string to lowercase.

```fides
pub fn str_to_lower(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2425*

---

## str_parse_int

Parse a decimal integer from a string. Returns 0 on failure.

```fides
pub fn str_parse_int(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 2430*

---

## str_parse_hex

Parse a hexadecimal integer from a string. Supports optional "0x" prefix.
Returns 0 on failure.

```fides
pub fn str_parse_hex(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 2436*

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

*Defined at line 2491*

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

*Defined at line 2513*

---

## str_len

Get the length of a string in bytes.

```fides
pub fn str_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 2518*

---

## str_is_empty

Check if a string is empty.

```fides
pub fn str_is_empty(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 2523*

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

*Defined at line 2528*

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

*Defined at line 2533*

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

*Defined at line 2538*

---

## int_to_str

Convert an integer to a string.

```fides
pub fn int_to_str(n: i64) -> str {
```

**Parameters:**

- `n`: `i64`

**Returns:** `str`

*Defined at line 2543*

---

## float_to_str

Convert a float to a string (SH-4.1).

```fides
pub fn float_to_str(f: f64) -> str {
```

**Parameters:**

- `f`: `f64`

**Returns:** `str`

*Defined at line 2548*

---

## str_parse_float

Parse a float from a string. Returns 0.0 on failure (SH-4.1).

```fides
pub fn str_parse_float(s: str) -> f64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `f64`

*Defined at line 2553*

---

## byte_to_str

Convert a byte value to a single-character string.

```fides
pub fn byte_to_str(b: i64) -> str {
```

**Parameters:**

- `b`: `i64`

**Returns:** `str`

*Defined at line 2558*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
