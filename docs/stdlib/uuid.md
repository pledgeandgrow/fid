[All modules](index.md)

# uuid -- Fides Documentation

## Contents

- [UUID](#UUID) (struct)
- [uuid_v4](#uuid_v4) (function)
- [uuid_nil](#uuid_nil) (function)
- [uuid_from_bytes](#uuid_from_bytes) (function)
- [uuid_to_string](#uuid_to_string) (function)
- [uuid_to_string_upper](#uuid_to_string_upper) (function)
- [uuid_to_urn](#uuid_to_urn) (function)
- [uuid_parse](#uuid_parse) (function)
- [uuid_parse_opt](#uuid_parse_opt) (function)
- [uuid_equal](#uuid_equal) (function)
- [uuid_is_nil](#uuid_is_nil) (function)
- [uuid_version](#uuid_version) (function)
- [uuid_variant](#uuid_variant) (function)
- [uuid_ns_dns](#uuid_ns_dns) (function)
- [uuid_ns_url](#uuid_ns_url) (function)
- [uuid_ns_oid](#uuid_ns_oid) (function)
- [uuid_ns_x500](#uuid_ns_x500) (function)
- [uuid_v5](#uuid_v5) (function)
- [uuid_v7](#uuid_v7) (function)
- [uuid_v7_time_ms](#uuid_v7_time_ms) (function)

## UUID

A 128-bit UUID stored as 16 bytes in a Vec.

```fides
pub struct UUID {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>,  // 16 bytes` |

*Defined at line 19*

---

## uuid_v4

Generate a random UUID v4.
Bytes come from the OS entropy source (p_os_random_u64), so sequences
differ across processes and are not reproducible from random_seed().


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_v4();
var s = uuid_to_string(u);
println(s);
assert_eq(str_len(s), 36);
assert_eq(uuid_version(u), 4);
assert_eq(uuid_variant(u), 1);
assert(!uuid_equal(u, uuid_v4()));
```

```fides
pub fn uuid_v4() -> UUID {
```

**Returns:** `UUID`

*Defined at line 37*

---

## uuid_nil

Generate a nil UUID (all zeros).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(uuid_to_string(uuid_nil()), "00000000-0000-0000-0000-000000000000");
```

```fides
pub fn uuid_nil() -> UUID {
```

**Returns:** `UUID`

*Defined at line 65*

---

## uuid_from_bytes

Generate a UUID from a Vec of 16 bytes.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var b: Vec<i64> = Vec::new();
var i = 0;
while i < 16 { b.push(i); i = i + 1; }
assert_eq(uuid_to_string(uuid_from_bytes(b)), "00010203-0405-0607-0809-0a0b0c0d0e0f");
```

```fides
pub fn uuid_from_bytes(bytes: Vec<i64>) -> UUID {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `UUID`

*Defined at line 84*

---

## uuid_to_string

Format a UUID as a lowercase string: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_parse("123E4567-E89B-12D3-A456-426614174000");
assert_eq(uuid_to_string(u), "123e4567-e89b-12d3-a456-426614174000");
```

```fides
pub fn uuid_to_string(u: UUID) -> str {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `str`

*Defined at line 106*

---

## uuid_to_string_upper

Format a UUID as an uppercase string.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_parse("123e4567e89b12d3a456426614174000");
assert_eq(uuid_to_string_upper(u), "123E4567-E89B-12D3-A456-426614174000");
```

```fides
pub fn uuid_to_string_upper(u: UUID) -> str {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `str`

*Defined at line 128*

---

## uuid_to_urn

Format a UUID as a URN: "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(uuid_to_urn(uuid_nil()), "urn:uuid:00000000-0000-0000-0000-000000000000");
```

```fides
pub fn uuid_to_urn(u: UUID) -> str {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `str`

*Defined at line 137*

---

## uuid_parse

Parse a UUID string strictly: either the 36-char hyphenated form
"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" or the 32-char bare hex form.
An optional "urn:uuid:" prefix and surrounding braces are accepted.
Returns a UUID, or nil UUID if invalid.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_parse("123e4567-e89b-12d3-a456-426614174000");
assert_eq(uuid_version(u), 1);
assert(uuid_is_nil(uuid_parse("not-a-uuid")));
```

```fides
pub fn uuid_parse(s: str) -> UUID {
```

**Parameters:**

- `s`: `str`

**Returns:** `UUID`

*Defined at line 152*

---

## uuid_parse_opt

Strict parse returning Option::None on any malformed input instead of
silently skipping non-hex characters.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
match uuid_parse_opt("zz") {
  Option::Some(u) => println("bad"),
  Option::None => println("ok")
}
```

```fides
pub fn uuid_parse_opt(s: str) -> Option<UUID> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Option<UUID>`

*Defined at line 171*

---

## uuid_equal

Check if two UUIDs are equal.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var a = uuid_parse("123e4567-e89b-12d3-a456-426614174000");
var b = uuid_parse("123e4567e89b12d3a456426614174000");
assert(uuid_equal(a, b));
```

```fides
pub fn uuid_equal(a: UUID, b: UUID) -> bool {
```

**Parameters:**

- `a`: `UUID`
- `b`: `UUID`

**Returns:** `bool`

*Defined at line 231*

---

## uuid_is_nil

Check if a UUID is the nil UUID.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert(uuid_is_nil(uuid_nil()));
```

```fides
pub fn uuid_is_nil(u: UUID) -> bool {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `bool`

*Defined at line 246*

---

## uuid_version

Get the version of a UUID (1-5, or 0 for nil).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(uuid_version(uuid_nil()), 0);
```

```fides
pub fn uuid_version(u: UUID) -> i64 {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `i64`

*Defined at line 261*

---

## uuid_variant

Get the variant of a UUID.
Returns: 0 (NCS), 1 (RFC 4122), 2 (Microsoft), 3 (reserved).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
assert_eq(uuid_variant(uuid_parse("123e4567-e89b-12d3-a456-426614174000")), 1);
```

```fides
pub fn uuid_variant(u: UUID) -> i64 {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `i64`

*Defined at line 270*

---

## uuid_ns_dns

The RFC 4122 DNS namespace UUID.

```fides
pub fn uuid_ns_dns() -> UUID {
```

**Returns:** `UUID`

*Defined at line 287*

---

## uuid_ns_url

The RFC 4122 URL namespace UUID.

```fides
pub fn uuid_ns_url() -> UUID {
```

**Returns:** `UUID`

*Defined at line 292*

---

## uuid_ns_oid

The RFC 4122 OID namespace UUID.

```fides
pub fn uuid_ns_oid() -> UUID {
```

**Returns:** `UUID`

*Defined at line 297*

---

## uuid_ns_x500

The RFC 4122 X.500 namespace UUID.

```fides
pub fn uuid_ns_x500() -> UUID {
```

**Returns:** `UUID`

*Defined at line 302*

---

## uuid_v5

Generate a name-based UUID v5 (SHA-1 of namespace bytes + name).
Deterministic: the same namespace and name always give the same UUID.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_v5(uuid_ns_dns(), "example.com");
assert_eq(uuid_to_string(u), "cfbff0d1-9375-5685-968c-48ce8b15ae17");
```

```fides
pub fn uuid_v5(ns: UUID, name: str) -> UUID {
```

**Parameters:**

- `ns`: `UUID`
- `name`: `str`

**Returns:** `UUID`

*Defined at line 312*

---

## uuid_v7

Generate a time-ordered UUID v7 (RFC 9562): 48-bit unix epoch
milliseconds, then version/variant and random bits from OS entropy.
v7s sort by creation time to millisecond granularity.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
var u = uuid_v7();
assert_eq(uuid_version(u), 7);
assert_eq(uuid_variant(u), 1);
assert_eq(str_len(uuid_to_string(u)), 36);
```

```fides
pub fn uuid_v7() -> UUID {
```

**Returns:** `UUID`

*Defined at line 348*

---

## uuid_v7_time_ms

The 48-bit unix epoch milliseconds inside a v7 UUID (0 for other
versions).

```fides
pub fn uuid_v7_time_ms(u: UUID) -> i64 {
```

**Parameters:**

- `u`: `UUID`

**Returns:** `i64`

*Defined at line 375*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
