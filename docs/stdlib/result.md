[All modules](index.md)

# result -- Fides Documentation

## Contents

- [ResultValue](#ResultValue) (struct)
- [ResultValue_ok](#ResultValue_ok) (function)
- [ResultValue_err](#ResultValue_err) (function)
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


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = ResultValue_ok(42);
assert(resultvalue_is_ok(r));
assert_eq(resultvalue_unwrap(r), 42);
var e = ResultValue_err(404);
assert_eq(resultvalue_unwrap_or(e, 0), 0);
assert_eq(resultvalue_to_str(e), "Err(404)");
```

```fides
pub struct ResultValue {
```

**Fields:**

| Name | Type |
|------|------|
| `tag` | `i64` |
| `value` | `i64` |
| `error` | `i64` |

*Defined at line 28*

---

## ResultValue_ok

Create an Ok result.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_to_str(ResultValue_ok(1)), "Ok(1)");
```

```fides
pub fn ResultValue_ok(value: i64) -> ResultValue {
```

**Parameters:**

- `value`: `i64`

**Returns:** `ResultValue`

*Defined at line 41*

---

## ResultValue_err

Create an Err result.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(resultvalue_is_err(ResultValue_err(5)));
```

```fides
pub fn ResultValue_err(error: i64) -> ResultValue {
```

**Parameters:**

- `error`: `i64`

**Returns:** `ResultValue`

*Defined at line 51*

---

## resultvalue_is_ok

Check if a Result is Ok.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(!resultvalue_is_ok(ResultValue_err(1)));
```

```fides
pub fn resultvalue_is_ok(r: ResultValue) -> bool {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `bool`

*Defined at line 63*

---

## resultvalue_is_err

Check if a Result is Err.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(!resultvalue_is_err(ResultValue_ok(1)));
```

```fides
pub fn resultvalue_is_err(r: ResultValue) -> bool {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `bool`

*Defined at line 71*

---

## resultvalue_unwrap

Unwrap the value. Panics if Err.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_unwrap(ResultValue_ok(7)), 7);
```

```fides
pub fn resultvalue_unwrap(r: ResultValue) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `i64`

*Defined at line 81*

---

## resultvalue_unwrap_err

Unwrap the error. Panics if Ok.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_unwrap_err(ResultValue_err(9)), 9);
```

```fides
pub fn resultvalue_unwrap_err(r: ResultValue) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `i64`

*Defined at line 93*

---

## resultvalue_unwrap_or

Return the value if Ok, otherwise return `default`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_unwrap_or(ResultValue_err(9), -1), -1);
assert_eq(resultvalue_unwrap_or(ResultValue_ok(3), -1), 3);
```

```fides
pub fn resultvalue_unwrap_or(r: ResultValue, default: i64) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `default`: `i64`

**Returns:** `i64`

*Defined at line 106*

---

## resultvalue_unwrap_or_else

Return the value if Ok, otherwise call `fallback` and return its result.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = resultvalue_unwrap_or_else(ResultValue_err(4), fn(e: i64) -> i64 { e * 10 });
assert_eq(v, 40);
```

```fides
pub fn resultvalue_unwrap_or_else(r: ResultValue, fallback: fn(i64) -> i64) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `fallback`: `fn(i64) -> i64`

**Returns:** `i64`

*Defined at line 118*

---

## resultvalue_expect

Expect the value with a custom panic message.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_expect(ResultValue_ok(2), "must be ok"), 2);
```

```fides
pub fn resultvalue_expect(r: ResultValue, msg: str) -> i64 {
```

**Parameters:**

- `r`: `ResultValue`
- `msg`: `str`

**Returns:** `i64`

*Defined at line 129*

---

## resultvalue_map

Map an Ok value through a function. Err passes through unchanged.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = resultvalue_map(ResultValue_ok(2), fn(x: i64) -> i64 { x + 1 });
assert_eq(resultvalue_unwrap(r), 3);
var e = resultvalue_map(ResultValue_err(5), fn(x: i64) -> i64 { x + 1 });
assert_eq(resultvalue_unwrap_err(e), 5);
```

```fides
pub fn resultvalue_map(r: ResultValue, f: fn(i64) -> i64) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> i64`

**Returns:** `ResultValue`

*Defined at line 146*

---

## resultvalue_map_err

Map an Err value through a function. Ok passes through unchanged.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var e = resultvalue_map_err(ResultValue_err(5), fn(x: i64) -> i64 { x * 2 });
assert_eq(resultvalue_unwrap_err(e), 10);
```

```fides
pub fn resultvalue_map_err(r: ResultValue, f: fn(i64) -> i64) -> ResultValue {
```

**Parameters:**

- `r`: `ResultValue`
- `f`: `fn(i64) -> i64`

**Returns:** `ResultValue`

*Defined at line 158*

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

*Defined at line 167*

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

*Defined at line 175*

---

## resultvalue_ok_option

Convert Result to Option (Ok → Some, Err → None).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match resultvalue_ok_option(ResultValue_ok(6)) {
  Option::Some(v) => { assert_eq(v, 6); }
  Option::None => { panic("expected Some"); }
}
```

```fides
pub fn resultvalue_ok_option(r: ResultValue) -> Option {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `Option`

*Defined at line 191*

---

## resultvalue_err_option

Convert Result error to Option (Err → Some, Ok → None).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match resultvalue_err_option(ResultValue_ok(6)) {
  Option::Some(_v) => { panic("expected None"); }
  Option::None => { println("no error"); }
}
```

```fides
pub fn resultvalue_err_option(r: ResultValue) -> Option {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `Option`

*Defined at line 205*

---

## resultvalue_contains

Check if the Result contains the given value.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(resultvalue_contains(ResultValue_ok(3), 3));
assert(!resultvalue_contains(ResultValue_err(3), 3));
```

```fides
pub fn resultvalue_contains(r: ResultValue, value: i64) -> bool {
```

**Parameters:**

- `r`: `ResultValue`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 219*

---

## resultvalue_contains_err

Check if the Result contains the given error.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(resultvalue_contains_err(ResultValue_err(3), 3));
```

```fides
pub fn resultvalue_contains_err(r: ResultValue, error: i64) -> bool {
```

**Parameters:**

- `r`: `ResultValue`
- `error`: `i64`

**Returns:** `bool`

*Defined at line 227*

---

## resultvalue_to_str

Get a string representation of the Result.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(resultvalue_to_str(ResultValue_ok(42)), "Ok(42)");
```

```fides
pub fn resultvalue_to_str(r: ResultValue) -> str {
```

**Parameters:**

- `r`: `ResultValue`

**Returns:** `str`

*Defined at line 235*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
