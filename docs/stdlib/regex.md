[All modules](index.md)

# regex -- Fides Documentation

## Contents

- [regex_matches](#regex_matches) (function)
- [Regex](#Regex) (struct)
- [regex_compile](#regex_compile) (function)
- [regex_match](#regex_match) (function)
- [regex_search](#regex_search) (function)
- [regex_find_re](#regex_find_re) (function)
- [regex_find_span](#regex_find_span) (function)
- [regex_group_count](#regex_group_count) (function)
- [regex_capture_spans_from](#regex_capture_spans_from) (function)
- [regex_capture_spans](#regex_capture_spans) (function)
- [regex_captures](#regex_captures) (function)
- [regex_group](#regex_group) (function)
- [regex_replace](#regex_replace) (function)
- [regex_split](#regex_split) (function)
- [regex_find_all](#regex_find_all) (function)
- [regex_replace_groups](#regex_replace_groups) (function)
- [regex_find](#regex_find) (function)
- [regex_contains](#regex_contains) (function)
- [regex_starts_with](#regex_starts_with) (function)
- [regex_ends_with](#regex_ends_with) (function)
- [regex_len](#regex_len) (function)

## regex_matches

Check if a string matches a simple wildcard pattern.
Supports: ? (single char), * (zero or more chars)


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_matches("report.txt", "*.txt"));
assert(regex_matches("cat", "c?t"));
assert(!regex_matches("cart", "c?t"));
```

```fides
pub fn regex_matches(s: str, pattern: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `pattern`: `str`

**Returns:** `bool`

*Defined at line 32*

---

## Regex

A compiled regex: token Vec + class bitmaps + program ranges + ok flag.

```fides
pub struct Regex {
```

**Fields:**

| Name | Type |
|------|------|
| `toks` | `Vec<i64>` |
| `cls` | `Vec<i64>` |
| `progs` | `Vec<i64>` |
| `ok` | `bool` |

*Defined at line 59*

---

## regex_compile

Compile `re` into a Regex (its `ok` flag is false on a syntax error).
The regex_* functions below compile their pattern themselves.

```fides
pub fn regex_compile(re: str) -> Regex {
```

**Parameters:**

- `re`: `str`

**Returns:** `Regex`

*Defined at line 756*

---

## regex_match

True when `s` fully matches `re` (anchors implied at both ends).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_match("hello123", "h[ae]l+o[0-9]+"));
assert(!regex_match("hello123!", "h[ae]l+o[0-9]+"));
assert(regex_match("2024-01-31", "\\d{4}-\\d{2}-\\d{2}"));
assert(regex_match("cat", "cat|dog"));
```

```fides
pub fn regex_match(s: str, re: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `bool`

*Defined at line 1103*

---

## regex_search

True when `re` matches anywhere inside `s`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_search("error: disk full", "disk|cpu"));
assert(!regex_search("all good", "^error"));
```

```fides
pub fn regex_search(s: str, re: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `bool`

*Defined at line 1124*

---

## regex_find_re

First position where `re` matches inside `s`, or -1.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_find_re("abc123def", "[0-9]+"), 3);
assert_eq(regex_find_re("abc", "[0-9]"), -1);
```

```fides
pub fn regex_find_re(s: str, re: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `i64`

*Defined at line 1133*

---

## regex_find_span

First match position and end, packed [start, end] in a Vec; [-1,-1] if none.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var sp = regex_find_span("id: 4521;", "\\d+");
assert_eq(sp.get(0), 4);
assert_eq(sp.get(1), 8);
```

```fides
pub fn regex_find_span(s: str, re: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1147*

---

## regex_group_count

The number of capturing groups in `re`.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_group_count("(a)(?:b)(c(d))"), 3);
```

```fides
pub fn regex_group_count(re: str) -> i64 {
```

**Parameters:**

- `re`: `str`

**Returns:** `i64`

*Defined at line 1170*

---

## regex_capture_spans_from

The first match at or after `from` and its groups as positions:
[start0, end0, start1, end1, ...], group 0 the whole match and -1, -1
for a group that took no part. [] when nothing matches (or `re` does
not compile).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var sp = regex_capture_spans_from("k=v; x=yz", "(\\w)=(\\w+)", 4);
// [5, 9, 5, 6, 7, 9]
```

```fides
pub fn regex_capture_spans_from(s: str, re: str, from: i64) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`
- `from`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 1182*

---

## regex_capture_spans

The first match and its groups as positions (see
regex_capture_spans_from).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var sp = regex_capture_spans("id=42", "(\\w+)=(\\d+)?");
// [0, 5, 0, 2, 3, 5]
```

```fides
pub fn regex_capture_spans(s: str, re: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `Vec<i64>`

*Defined at line 1221*

---

## regex_captures

The text of the first match and of each group: [whole, group 1, ...],
"" for a group that took no part (regex_capture_spans tells it from an
empty match). [] when nothing matches.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var c = regex_captures("on 2026-11-16", "(\\d{4})-(\\d{2})-(\\d{2})");
assert_eq(c.get(1), "2026");
assert_eq(c.get(3), "16");
```

```fides
pub fn regex_captures(s: str, re: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `Vec<str>`

*Defined at line 1233*

---

## regex_group

The text of group `g` of the first match (0 = the whole match), or
None when nothing matches or the group took no part.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_group("user=ann", "(\\w+)=(\\w+)", 2).unwrap(), "ann");
```

```fides
pub fn regex_group(s: str, re: str, g: i64) -> Option<str> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`
- `g`: `i64`

**Returns:** `Option<str>`

*Defined at line 1255*

---

## regex_replace

Replace every match of `re` in `s` with `rep` (literal text).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var r = regex_replace("a1b22c", "[0-9]+", "#");
println(r);
assert_eq(r, "a#b#c");
```

```fides
pub fn regex_replace(s: str, re: str, rep: str) -> str {
```

**Parameters:**

- `s`: `str`
- `re`: `str`
- `rep`: `str`

**Returns:** `str`

*Defined at line 1274*

---

## regex_split

Split `s` at every match of `re`; returns pieces (not the separators).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var parts = regex_split("a, b,,c", ",\\s*");
assert_eq(parts.len(), 4);
assert_eq(parts.get(1), "b");
assert_eq(parts.get(2), "");
```

```fides
pub fn regex_split(s: str, re: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `Vec<str>`

*Defined at line 1311*

---

## regex_find_all

Every match of `re` in `s`, left to right, not overlapping (an empty
match moves on one byte), as Python's re.findall without groups.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var all = regex_find_all("a1 b22 c333", "\\d+");
assert_eq(all.len(), 3);
assert_eq(all.get(2), "333");
```

```fides
pub fn regex_find_all(s: str, re: str) -> Vec<str> {
```

**Parameters:**

- `s`: `str`
- `re`: `str`

**Returns:** `Vec<str>`

*Defined at line 1345*

---

## regex_replace_groups

Replace every match of `re` in `s` with `rep`, where $0 (the whole
match), $1..$9 and ${n} stand for groups and $$ for '$' (Go's
Regexp.ReplaceAllString with numbered groups).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_replace_groups("2026-11-17", "(\\d+)-(\\d+)-(\\d+)", "$3/$2/$1"), "17/11/2026");
assert_eq(regex_replace_groups("a=1, b=2", "(\\w)=(\\d)", "${2}:$1"), "1:a, 2:b");
assert_eq(regex_replace_groups("cost 5", "\\d", "$$$0"), "cost $5");
```

```fides
pub fn regex_replace_groups(s: str, re: str, rep: str) -> str {
```

**Parameters:**

- `s`: `str`
- `re`: `str`
- `rep`: `str`

**Returns:** `str`

*Defined at line 1434*

---

## regex_find

Find the first occurrence of a literal substring in a string.
Returns the starting index, or -1 if not found.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_find("hello world", "o w"), 4);
```

```fides
pub fn regex_find(s: str, sub: str) -> i64 {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `i64`

*Defined at line 1470*

---

## regex_contains

Check if a string contains a substring.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_contains("hello", "ell"));
```

```fides
pub fn regex_contains(s: str, sub: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `sub`: `str`

**Returns:** `bool`

*Defined at line 1478*

---

## regex_starts_with

Check if a string starts with a prefix.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_starts_with("hello", "he"));
```

```fides
pub fn regex_starts_with(s: str, prefix: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `prefix`: `str`

**Returns:** `bool`

*Defined at line 1486*

---

## regex_ends_with

Check if a string ends with a suffix.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(regex_ends_with("hello", "lo"));
```

```fides
pub fn regex_ends_with(s: str, suffix: str) -> bool {
```

**Parameters:**

- `s`: `str`
- `suffix`: `str`

**Returns:** `bool`

*Defined at line 1494*

---

## regex_len

Get the length of a string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(regex_len("hello"), 5);
```

```fides
pub fn regex_len(s: str) -> i64 {
```

**Parameters:**

- `s`: `str`

**Returns:** `i64`

*Defined at line 1502*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
