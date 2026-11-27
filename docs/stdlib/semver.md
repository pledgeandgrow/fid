[All modules](index.md)

# semver -- Fides Documentation

## Contents

- [SemVer](#SemVer) (struct)
- [semver_parse](#semver_parse) (function)
- [semver_is_valid](#semver_is_valid) (function)
- [semver_is_valid_str](#semver_is_valid_str) (function)
- [semver_to_str](#semver_to_str) (function)
- [semver_compare](#semver_compare) (function)
- [semver_compare_str](#semver_compare_str) (function)
- [semver_eq](#semver_eq) (function)
- [semver_lt](#semver_lt) (function)
- [semver_gt](#semver_gt) (function)
- [semver_satisfies](#semver_satisfies) (function)
- [semver_satisfies_str](#semver_satisfies_str) (function)
- [semver_max](#semver_max) (function)
- [semver_max_satisfying](#semver_max_satisfying) (function)

## SemVer

A parsed semantic version. `pre` and `build` are raw strings
("" when absent). `major == -1` marks an invalid parse.

```fides
pub struct SemVer {
```

**Fields:**

| Name | Type |
|------|------|
| `major` | `i64` |
| `minor` | `i64` |
| `patch` | `i64` |
| `pre` | `str` |
| `build` | `str` |

*Defined at line 18*

---

## semver_parse

Parse a SemVer string. Returns a SemVer with major == -1 on failure;
check with semver_is_valid(v) or semver_is_valid_str(s) first.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = semver_parse("1.4.2-beta.1+build.7");
assert_eq(v.major, 1);
assert_eq(v.patch, 2);
assert_eq(v.pre, "beta.1");
assert_eq(v.build, "build.7");
assert(!semver_is_valid(semver_parse("1.x")));
```

```fides
pub fn semver_parse(s: str) -> SemVer {
```

**Parameters:**

- `s`: `str`

**Returns:** `SemVer`

*Defined at line 115*

---

## semver_is_valid

True if `v` is a successfully parsed version.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_is_valid(semver_parse("0.1.0")));
```

```fides
pub fn semver_is_valid(v: SemVer) -> bool {
```

**Parameters:**

- `v`: `SemVer`

**Returns:** `bool`

*Defined at line 159*

---

## semver_is_valid_str

True if `s` parses as a valid SemVer string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_is_valid_str("2.0.0-rc.1"));
assert(!semver_is_valid_str("01.0.0"));
```

```fides
pub fn semver_is_valid_str(s: str) -> bool {
```

**Parameters:**

- `s`: `str`

**Returns:** `bool`

*Defined at line 168*

---

## semver_to_str

Format back to canonical SemVer text (build metadata preserved).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(semver_to_str(semver_parse(" 1.2.3+meta ")), "1.2.3+meta");
```

```fides
pub fn semver_to_str(v: SemVer) -> str {
```

**Parameters:**

- `v`: `SemVer`

**Returns:** `str`

*Defined at line 176*

---

## semver_compare

Compare two versions: -1 if a < b, 0 if equal precedence, 1 if a > b.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(semver_compare(semver_parse("1.0.0-alpha"), semver_parse("1.0.0")), -1);
assert_eq(semver_compare(semver_parse("1.10.0"), semver_parse("1.9.9")), 1);
```

```fides
pub fn semver_compare(a: SemVer, b: SemVer) -> i64 {
```

**Parameters:**

- `a`: `SemVer`
- `b`: `SemVer`

**Returns:** `i64`

*Defined at line 253*

---

## semver_compare_str

Compare two version strings. Invalid versions sort before all valid ones.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(semver_compare_str("2.0.0", "2.0.0+build"), 0);
```

```fides
pub fn semver_compare_str(a: str, b: str) -> i64 {
```

**Parameters:**

- `a`: `str`
- `b`: `str`

**Returns:** `i64`

*Defined at line 279*

---

## semver_eq

Whether the versions have equal precedence (build metadata ignored).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_eq(semver_parse("1.0.0+a"), semver_parse("1.0.0+b")));
```

```fides
pub fn semver_eq(a: SemVer, b: SemVer) -> bool {
```

**Parameters:**

- `a`: `SemVer`
- `b`: `SemVer`

**Returns:** `bool`

*Defined at line 287*

---

## semver_lt

Whether `a` has lower precedence than `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_lt(semver_parse("1.0.0-rc.1"), semver_parse("1.0.0")));
```

```fides
pub fn semver_lt(a: SemVer, b: SemVer) -> bool {
```

**Parameters:**

- `a`: `SemVer`
- `b`: `SemVer`

**Returns:** `bool`

*Defined at line 295*

---

## semver_gt

Whether `a` has higher precedence than `b`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_gt(semver_parse("2.0.0"), semver_parse("1.99.99")));
```

```fides
pub fn semver_gt(a: SemVer, b: SemVer) -> bool {
```

**Parameters:**

- `a`: `SemVer`
- `b`: `SemVer`

**Returns:** `bool`

*Defined at line 303*

---

## semver_satisfies

True if version `v` satisfies range expression `range`.
Examples: "*", "1.2.3", "^1.2", "~1.4.0", ">=1.0 <2.0".


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var v = semver_parse("1.4.7");
assert(semver_satisfies(v, "^1.2"));
assert(semver_satisfies(v, "~1.4.0"));
assert(semver_satisfies(v, ">=1.0 <2.0"));
assert(!semver_satisfies(v, "^2"));
```

```fides
pub fn semver_satisfies(v: SemVer, range: str) -> bool {
```

**Parameters:**

- `v`: `SemVer`
- `range`: `str`

**Returns:** `bool`

*Defined at line 366*

---

## semver_satisfies_str

Convenience: semver_satisfies(semver_parse(v), range).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(semver_satisfies_str("0.3.1", "*"));
```

```fides
pub fn semver_satisfies_str(v: str, range: str) -> bool {
```

**Parameters:**

- `v`: `str`
- `range`: `str`

**Returns:** `bool`

*Defined at line 391*

---

## semver_max

Highest-precedence version in a list of version strings.
Returns "" when the list is empty or contains no valid versions.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var vs: Vec<str> = Vec::new();
vs.push("1.2.0"); vs.push("1.10.0"); vs.push("bogus"); vs.push("1.9.3");
assert_eq(semver_max(vs), "1.10.0");
```

```fides
pub fn semver_max(versions: Vec) -> str {
```

**Parameters:**

- `versions`: `Vec`

**Returns:** `str`

*Defined at line 557*

---

## semver_max_satisfying

Highest-precedence version satisfying `range`, or "" if none.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var vs: Vec<str> = Vec::new();
vs.push("1.2.0"); vs.push("1.10.0"); vs.push("2.0.0");
assert_eq(semver_max_satisfying(vs, "<2"), "1.10.0");
assert_eq(semver_max_satisfying(vs, ">3"), "");
```

```fides
pub fn semver_max_satisfying(versions: Vec, range: str) -> str {
```

**Parameters:**

- `versions`: `Vec`
- `range`: `str`

**Returns:** `str`

*Defined at line 582*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
