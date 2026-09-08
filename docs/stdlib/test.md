# test — Fides Documentation

## Contents

- [TestRunner](#testrunner) (struct)
- [test_runner_new](#test_runner_new) (function)
- [test_begin](#test_begin) (function)
- [test_pass](#test_pass) (function)
- [test_fail](#test_fail) (function)
- [test_summary](#test_summary) (function)
- [test_all_passed](#test_all_passed) (function)
- [test_assert_eq](#test_assert_eq) (function)
- [test_assert_ne](#test_assert_ne) (function)
- [test_assert_true](#test_assert_true) (function)
- [test_assert_false](#test_assert_false) (function)
- [test_assert_approx_eq](#test_assert_approx_eq) (function)
- [test_assert_gt](#test_assert_gt) (function)
- [test_assert_lt](#test_assert_lt) (function)
- [test_assert_in_range](#test_assert_in_range) (function)
- [test_assert_str_eq](#test_assert_str_eq) (function)
- [test_assert_empty](#test_assert_empty) (function)
- [test_assert_len](#test_assert_len) (function)
- [test_assert_contains](#test_assert_contains) (function)
- [test_assert_or_abort](#test_assert_or_abort) (function)
- [test_assert_eq_or_abort](#test_assert_eq_or_abort) (function)

## TestRunner

Global test counters (managed via a TestRunner struct since Fides has no globals).

```fides
pub struct TestRunner {
```

**Fields:**

| Name | Type |
|------|------|
| `passed` | `i64` |
| `failed` | `i64` |
| `current_test` | `str` |

*Defined at line 19*

---

## test_runner_new

Create a new test runner.

```fides
pub fn test_runner_new() -> TestRunner {
```

**Returns:** `TestRunner`

*Defined at line 26*

---

## test_begin

Begin a named test.

```fides
pub fn test_begin(runner: TestRunner, name: str) -> void {
```

**Parameters:**

- `runner`: `TestRunner`
- `name`: `str`

**Returns:** `void`

*Defined at line 31*

---

## test_pass

End the current test (mark as passed).

```fides
pub fn test_pass(runner: TestRunner) -> void {
```

**Parameters:**

- `runner`: `TestRunner`

**Returns:** `void`

*Defined at line 38*

---

## test_fail

Mark the current test as failed.

```fides
pub fn test_fail(runner: TestRunner, msg: str) -> void {
```

**Parameters:**

- `runner`: `TestRunner`
- `msg`: `str`

**Returns:** `void`

*Defined at line 43*

---

## test_summary

Print test summary.

```fides
pub fn test_summary(runner: TestRunner) -> void {
```

**Parameters:**

- `runner`: `TestRunner`

**Returns:** `void`

*Defined at line 50*

---

## test_all_passed

Check if all tests passed.

```fides
pub fn test_all_passed(runner: TestRunner) -> bool {
```

**Parameters:**

- `runner`: `TestRunner`

**Returns:** `bool`

*Defined at line 68*

---

## test_assert_eq

Assert two i64 values are equal. Prints message on failure.

```fides
pub fn test_assert_eq(a: i64, b: i64, msg: str) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 75*

---

## test_assert_ne

Assert two i64 values are NOT equal.

```fides
pub fn test_assert_ne(a: i64, b: i64, msg: str) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 89*

---

## test_assert_true

Assert a condition is true.

```fides
pub fn test_assert_true(cond: bool, msg: str) -> bool {
```

**Parameters:**

- `cond`: `bool`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 101*

---

## test_assert_false

Assert a condition is false.

```fides
pub fn test_assert_false(cond: bool, msg: str) -> bool {
```

**Parameters:**

- `cond`: `bool`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 111*

---

## test_assert_approx_eq

Assert two f64 values are approximately equal (within epsilon).

```fides
pub fn test_assert_approx_eq(a: f64, b: f64, epsilon: f64, msg: str) -> bool {
```

**Parameters:**

- `a`: `f64`
- `b`: `f64`
- `epsilon`: `f64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 122*

---

## test_assert_gt

Assert a value is greater than another.

```fides
pub fn test_assert_gt(a: i64, b: i64, msg: str) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 139*

---

## test_assert_lt

Assert a value is less than another.

```fides
pub fn test_assert_lt(a: i64, b: i64, msg: str) -> bool {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 153*

---

## test_assert_in_range

Assert a value is in a range [lo, hi].

```fides
pub fn test_assert_in_range(value: i64, lo: i64, hi: i64, msg: str) -> bool {
```

**Parameters:**

- `value`: `i64`
- `lo`: `i64`
- `hi`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 167*

---

## test_assert_str_eq

Assert a string equals another string.

```fides
pub fn test_assert_str_eq(a: str, b: str, msg: str) -> bool {
```

**Parameters:**

- `a`: `str`
- `b`: `str`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 184*

---

## test_assert_empty

Assert a Vec is empty.

```fides
pub fn test_assert_empty(v: Vec, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 199*

---

## test_assert_len

Assert a Vec has a specific length.

```fides
pub fn test_assert_len(v: Vec, expected: i64, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec`
- `expected`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 211*

---

## test_assert_contains

Assert a Vec contains a value.

```fides
pub fn test_assert_contains(v: Vec, value: i64, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec`
- `value`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 225*

---

## test_assert_or_abort

Assert and abort if false.

```fides
pub fn test_assert_or_abort(cond: bool, msg: str) -> void {
```

**Parameters:**

- `cond`: `bool`
- `msg`: `str`

**Returns:** `void`

*Defined at line 239*

---

## test_assert_eq_or_abort

Assert equal and abort if not.

```fides
pub fn test_assert_eq_or_abort(a: i64, b: i64, msg: str) -> void {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`
- `msg`: `str`

**Returns:** `void`

*Defined at line 248*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
