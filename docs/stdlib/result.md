# result — Fides Documentation

## Contents

- [ResultValue](#resultvalue) (struct)
- [ResultValue_ok](#resultvalue_ok) (function)
- [ResultValue_err](#resultvalue_err) (function)
- [resultvalue_is_ok](#resultvalue_is_ok) (function)
- [resultvalue_is_err](#resultvalue_is_err) (function)
- [resultvalue_unwrap](#resultvalue_unwrap) (function)
- [resultvalue_unwrap_err](#resultvalue_unwrap_err) (function)
- [resultvalue_unwrap_or](#resultvalue_unwrap_or) (function)
- [resultvalue_unwrap_or_else](#resultvalue_unwrap_or_else) (function)
- [resultvalue_expect](#resultvalue_expect) (function)
- [resultvalue_map](#resultvalue_map) (function)
- [resultvalue_map_err](#resultvalue_map_err) (function)
- [resultvalue_and_then](#resultvalue_and_then) (function)
- [resultvalue_or_else](#resultvalue_or_else) (function)
- [resultvalue_ok_option](#resultvalue_ok_option) (function)
- [resultvalue_err_option](#resultvalue_err_option) (function)
- [resultvalue_contains](#resultvalue_contains) (function)
- [resultvalue_contains_err](#resultvalue_contains_err) (function)
- [resultvalue_to_str](#resultvalue_to_str) (function)

## ResultValue

A Result type — either Ok(value) or Err(error).
Uses a tag (1 = Ok, 0 = Err) and two i64 fields.

```fides
pub struct ResultValue {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `i64,   // 1 = Ok, 0 = Err` |
| `value` | `i64` |
| `error` | `i64` |

*Defined at line 22*

---

## ResultValue_ok

Create an Ok result.

```fides
pub fn ResultValue_ok(value: i64) -> ResultValue {
```

**Parameters:**

- `value`: `i64`

**Returns:** `ResultValue`

*Defined at line 31*

---

## ResultValue_err

Create an Err result.

```fides
pub fn ResultValue_err(error: i64) -> ResultValue {
```

**Parameters:**

- `error`: `i64`

**Returns:** `ResultValue`

*Defined at line 36*

---

## resultvalue_is_ok

Check if a Result is Ok.

```fides
pub fn resultvalue_is_ok(r: ResultValue) -> bool {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `bool`

*Defined at line 43*

---

## resultvalue_is_err

Check if a Result is Err.

```fides
pub fn resultvalue_is_err(r: ResultValue) -> bool {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `bool`

*Defined at line 48*

---

## resultvalue_unwrap

Unwrap the value. Panics if Err.

```fides
pub fn resultvalue_unwrap(r: ResultValue) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `i64`

*Defined at line 55*

---

## resultvalue_unwrap_err

Unwrap the error. Panics if Ok.

```fides
pub fn resultvalue_unwrap_err(r: ResultValue) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `i64`

*Defined at line 64*

---

## resultvalue_unwrap_or

Return the value if Ok, otherwise return `default`.

```fides
pub fn resultvalue_unwrap_or(r: ResultValue, default: i64) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 73*

---

## resultvalue_unwrap_or_else

Return the value if Ok, otherwise call `fallback` and return its result.

```fides
pub fn resultvalue_unwrap_or_else(r: ResultValue, fallback: fn(i64) -> i64) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `fallback`: `fn(i64) -> i64`

**Returns:** `i64`

*Defined at line 79*

---

## resultvalue_expect

Expect the value with a custom panic message.

```fides
pub fn resultvalue_expect(r: ResultValue, msg: str) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `msg`: `str`

**Returns:** `i64`

*Defined at line 85*

---

## resultvalue_map

Map an Ok value through a function. Err passes through unchanged.

```fides
pub fn resultvalue_map(r: ResultValue, f: fn(i64) -> i64) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> i64`

**Returns:** `ResultValue`

*Defined at line 96*

---

## resultvalue_map_err

Map an Err value through a function. Ok passes through unchanged.

```fides
pub fn resultvalue_map_err(r: ResultValue, f: fn(i64) -> i64) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> i64`

**Returns:** `ResultValue`

*Defined at line 104*

---

## resultvalue_and_then

Chain: if Ok, apply f to the value to get a new Result.
If Err, pass through unchanged (like flatMap / and_then).

```fides
pub fn resultvalue_and_then(r: ResultValue, f: fn(i64) -> ResultValue) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> ResultValue`

**Returns:** `ResultValue`

*Defined at line 113*

---

## resultvalue_or_else

If Err, apply f to the error to get a new Result (orElse).

```fides
pub fn resultvalue_or_else(r: ResultValue, f: fn(i64) -> ResultValue) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> ResultValue`

**Returns:** `ResultValue`

*Defined at line 121*

---

## resultvalue_ok_option

Convert Result to Option (Ok → Some, Err → None).

```fides
pub fn resultvalue_ok_option(r: ResultValue) -> Option {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `Option`

*Defined at line 131*

---

## resultvalue_err_option

Convert Result error to Option (Err → Some, Ok → None).

```fides
pub fn resultvalue_err_option(r: ResultValue) -> Option {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `Option`

*Defined at line 139*

---

## resultvalue_contains

Check if the Result contains the given value.

```fides
pub fn resultvalue_contains(r: ResultValue, value: i64) -> bool {
```

**Parameters:**

- `r`: `ResultValue`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 149*

---

## resultvalue_contains_err

Check if the Result contains the given error.

```fides
pub fn resultvalue_contains_err(r: ResultValue, error: i64) -> bool {
```

**Parameters:**

- `r`: `ResultValue`
- `error`: `i64`

**Returns:** `bool`

*Defined at line 154*

---

## resultvalue_to_str

Get a string representation of the Result.

```fides
pub fn resultvalue_to_str(r: ResultValue) -> str {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `str`

*Defined at line 159*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
