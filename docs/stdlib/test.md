[All modules](index.md)

# test -- Fides Documentation

## Contents

- [TestRunner](#TestRunner) (struct)
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
- [TableTest](#TableTest) (struct)
- [table_new](#table_new) (function)
- [table_runs](#table_runs) (function)
- [table_check](#table_check) (function)
- [table_int](#table_int) (function)
- [table_str](#table_str) (function)
- [table_bool](#table_bool) (function)
- [table_failed](#table_failed) (function)
- [table_done](#table_done) (function)
- [expect_snapshot](#expect_snapshot) (function)
- [MockClock](#MockClock) (struct)
- [mock_clock](#mock_clock) (function)
- [mock_clock_fixed](#mock_clock_fixed) (function)
- [mock_clock_now](#mock_clock_now) (function)
- [mock_clock_reads](#mock_clock_reads) (function)
- [MockFS](#MockFS) (struct)
- [mock_fs](#mock_fs) (function)
- [mock_fs_add](#mock_fs_add) (function)
- [mock_fs_read](#mock_fs_read) (function)
- [mock_fs_has](#mock_fs_has) (function)
- [mock_fs_paths](#mock_fs_paths) (function)
- [MockNet](#MockNet) (struct)
- [mock_net](#mock_net) (function)
- [mock_net_add](#mock_net_add) (function)
- [mock_net_get](#mock_net_get) (function)

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

*Defined at line 18*

---

## test_runner_new

Create a new test runner.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = test_runner_new();
test_begin(&mut r, "counters");
test_pass(&mut r);
assert(test_all_passed(r));
test_fail(&mut r, "boom");
assert_eq(r.passed, 1);
assert_eq(r.failed, 1);
assert(!test_all_passed(r));
assert(test_assert_eq(3, 3, "eq"));
assert(test_assert_true(true, "t"));
```

```fides
pub fn test_runner_new() -> TestRunner {
```

**Returns:** `TestRunner`

*Defined at line 37*

---

## test_begin

Begin a named test.

```fides
pub fn test_begin(runner: &mut TestRunner, name: str) -> void {
```

**Parameters:**

- `runner`: `&mut TestRunner`
- `name`: `str`

**Returns:** `void`

*Defined at line 44*

---

## test_pass

End the current test (mark as passed).

```fides
pub fn test_pass(runner: &mut TestRunner) -> void {
```

**Parameters:**

- `runner`: `&mut TestRunner`

**Returns:** `void`

*Defined at line 51*

---

## test_fail

Mark the current test as failed.

```fides
pub fn test_fail(runner: &mut TestRunner, msg: str) -> void {
```

**Parameters:**

- `runner`: `&mut TestRunner`
- `msg`: `str`

**Returns:** `void`

*Defined at line 56*

---

## test_summary

Print test summary.

```fides
pub fn test_summary(runner: TestRunner) -> void {
```

**Parameters:**

- `runner`: `TestRunner`

**Returns:** `void`

*Defined at line 63*

---

## test_all_passed

Check if all tests passed.

```fides
pub fn test_all_passed(runner: TestRunner) -> bool {
```

**Parameters:**

- `runner`: `TestRunner`

**Returns:** `bool`

*Defined at line 81*

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

*Defined at line 88*

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

*Defined at line 102*

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

*Defined at line 114*

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

*Defined at line 124*

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

*Defined at line 135*

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

*Defined at line 154*

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

*Defined at line 168*

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

*Defined at line 182*

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

*Defined at line 199*

---

## test_assert_empty

Assert a Vec is empty.

```fides
pub fn test_assert_empty(v: Vec<i64>, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 214*

---

## test_assert_len

Assert a Vec has a specific length.

```fides
pub fn test_assert_len(v: Vec<i64>, expected: i64, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`
- `expected`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 226*

---

## test_assert_contains

Assert a Vec contains a value.

```fides
pub fn test_assert_contains(v: Vec<i64>, value: i64, msg: str) -> bool {
```

**Parameters:**

- `v`: `Vec<i64>`
- `value`: `i64`
- `msg`: `str`

**Returns:** `bool`

*Defined at line 240*

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

*Defined at line 254*

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

*Defined at line 263*

---

## TableTest

The rows of one table test.

```fides
pub struct TableTest {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `passed` | `i64` |
| `skipped` | `i64` |
| `failures` | `Vec<str>` |
| `filter` | `str` |

*Defined at line 290*

---

## table_new

A table test named `name` (FIDES_ROW, when set, filters its rows).

```fides
pub fn table_new(name: str) -> TableTest {
```

**Parameters:**

- `name`: `str`

**Returns:** `TableTest`

*Defined at line 301*

---

## table_runs

Whether row `row` runs under the filter (a skipped row is counted).

```fides
pub fn table_runs(t: &mut TableTest, row: str) -> bool {
```

**Parameters:**

- `t`: `&mut TableTest`
- `row`: `str`

**Returns:** `bool`

*Defined at line 316*

---

## table_check

Row `row` passed when `ok`; `why` says what was wrong otherwise.

```fides
pub fn table_check(t: &mut TableTest, row: str, ok: bool, why: str) -> bool {
```

**Parameters:**

- `t`: `&mut TableTest`
- `row`: `str`
- `ok`: `bool`
- `why`: `str`

**Returns:** `bool`

*Defined at line 325*

---

## table_int

Row `row`: `got` must equal `want`.

```fides
pub fn table_int(t: &mut TableTest, row: str, got: i64, want: i64) -> bool {
```

**Parameters:**

- `t`: `&mut TableTest`
- `row`: `str`
- `got`: `i64`
- `want`: `i64`

**Returns:** `bool`

*Defined at line 338*

---

## table_str

Row `row`: `got` must equal `want` (text).

```fides
pub fn table_str(t: &mut TableTest, row: str, got: str, want: str) -> bool {
```

**Parameters:**

- `t`: `&mut TableTest`
- `row`: `str`
- `got`: `str`
- `want`: `str`

**Returns:** `bool`

*Defined at line 343*

---

## table_bool

Row `row`: `got` must equal `want` (a truth value).

```fides
pub fn table_bool(t: &mut TableTest, row: str, got: bool, want: bool) -> bool {
```

**Parameters:**

- `t`: `&mut TableTest`
- `row`: `str`
- `got`: `bool`
- `want`: `bool`

**Returns:** `bool`

*Defined at line 348*

---

## table_failed

How many rows failed so far.

```fides
pub fn table_failed(t: &TableTest) -> i64 {
```

**Parameters:**

- `t`: `&TableTest`

**Returns:** `i64`

*Defined at line 353*

---

## table_done

Print "table <name>: <n> passed[, <k> skipped][, <f> failed]" and each
failed row, then panic when a row failed (so `fid test` reports the
test as failed). Returns the number of rows that passed.

```fides
pub fn table_done(t: TableTest) -> i64 {
```

**Parameters:**

- `t`: `TableTest`

**Returns:** `i64`

*Defined at line 360*

---

## expect_snapshot

expect_snapshot(name, value): compares `value` with the stored snapshot
in `<name>.snap` next to the test that calls it. `fid test --update`
(or FIDES_UPDATE=1) writes the file instead of comparing. A missing
file is a failure that says how to create it.

```fides
pub fn expect_snapshot(name: str, value: str) -> void {
```

**Parameters:**

- `name`: `str`
- `value`: `str`

**Returns:** `void`

*Defined at line 386*

---

## MockClock

A deterministic clock: a fixed sequence of times, advanced on each
read. Code under test takes `&mut MockClock` where it would call
`p_clock_now()`.

```fides
pub struct MockClock {
```

**Fields:**

| Name | Type |
|------|------|
| `times` | `Vec<i64>` |
| `at` | `i64` |

*Defined at line 424*

---

## mock_clock

A clock that returns the given times in order, then repeats the last.

```fides
pub fn mock_clock(times: Vec<i64>) -> MockClock {
```

**Parameters:**

- `times`: `Vec<i64>`

**Returns:** `MockClock`

*Defined at line 430*

---

## mock_clock_fixed

A clock that always returns the same time.

```fides
pub fn mock_clock_fixed(t: i64) -> MockClock {
```

**Parameters:**

- `t`: `i64`

**Returns:** `MockClock`

*Defined at line 435*

---

## mock_clock_now

Read the next time from the mock clock (the code under test's
"now"). Repeats the last time when the sequence runs out.

```fides
pub fn mock_clock_now(clock: &mut MockClock) -> i64 {
```

**Parameters:**

- `clock`: `&mut MockClock`

**Returns:** `i64`

*Defined at line 443*

---

## mock_clock_reads

How many times the clock was read.

```fides
pub fn mock_clock_reads(clock: &MockClock) -> i64 {
```

**Parameters:**

- `clock`: `&MockClock`

**Returns:** `i64`

*Defined at line 456*

---

## MockFS

An in-memory file system: paths to contents. Code under test takes
`&MockFS` where it would call `read_file()`.
An in-memory file system: paths to contents. Code under test takes
`&MockFS` where it would call `read_file()`.

```fides
pub struct MockFS {
```

**Fields:**

| Name | Type |
|------|------|
| `paths` | `Vec<str>` |
| `contents` | `Vec<str>` |

*Defined at line 465*

---

## mock_fs

A file system with no files.

```fides
pub fn mock_fs() -> MockFS {
```

**Returns:** `MockFS`

*Defined at line 471*

---

## mock_fs_add

Add or replace a file in the mock file system.

```fides
pub fn mock_fs_add(fs: &mut MockFS, path: str, content: str) {
```

**Parameters:**

- `fs`: `&mut MockFS`
- `path`: `str`
- `content`: `str`

*Defined at line 476*

---

## mock_fs_read

Read a file from the mock ("" when the path does not exist).

```fides
pub fn mock_fs_read(fs: &MockFS, path: str) -> str {
```

**Parameters:**

- `fs`: `&MockFS`
- `path`: `str`

**Returns:** `str`

*Defined at line 490*

---

## mock_fs_has

Does the mock hold this path?

```fides
pub fn mock_fs_has(fs: &MockFS, path: str) -> bool {
```

**Parameters:**

- `fs`: `&MockFS`
- `path`: `str`

**Returns:** `bool`

*Defined at line 502*

---

## mock_fs_paths

List the paths the mock holds.

```fides
pub fn mock_fs_paths(fs: &MockFS) -> Vec<str> {
```

**Parameters:**

- `fs`: `&MockFS`

**Returns:** `Vec<str>`

*Defined at line 514*

---

## MockNet

An in-memory network: URLs to canned responses.

```fides
pub struct MockNet {
```

**Fields:**

| Name | Type |
|------|------|
| `urls` | `Vec<str>` |
| `responses` | `Vec<str>` |

*Defined at line 519*

---

## mock_net

A network with no responses.

```fides
pub fn mock_net() -> MockNet {
```

**Returns:** `MockNet`

*Defined at line 525*

---

## mock_net_add

Add or replace a canned response.

```fides
pub fn mock_net_add(net: &mut MockNet, url: str, response: str) {
```

**Parameters:**

- `net`: `&mut MockNet`
- `url`: `str`
- `response`: `str`

*Defined at line 530*

---

## mock_net_get

"Fetch" a URL from the mock ("" when the URL has no response).

```fides
pub fn mock_net_get(net: &MockNet, url: str) -> str {
```

**Parameters:**

- `net`: `&MockNet`
- `url`: `str`

**Returns:** `str`

*Defined at line 544*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
