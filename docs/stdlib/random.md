[All modules](index.md)

# random -- Fides Documentation

## Contents

- [RngState](#RngState) (struct)
- [RngState](#RngState) (impl)
- [random_seed](#random_seed) (function)
- [random_seed_time](#random_seed_time) (function)
- [random_raw](#random_raw) (function)
- [random_int](#random_int) (function)
- [random_int_range](#random_int_range) (function)
- [random_i64](#random_i64) (function)
- [random_float](#random_float) (function)
- [random_float_max](#random_float_max) (function)
- [random_float_range](#random_float_range) (function)
- [random_gaussian](#random_gaussian) (function)
- [random_bool](#random_bool) (function)
- [random_choice](#random_choice) (function)
- [random_choice_idx](#random_choice_idx) (function)
- [random_shuffle](#random_shuffle) (function)
- [random_byte](#random_byte) (function)
- [random_bytes](#random_bytes) (function)
- [random_hex](#random_hex) (function)

## RngState

RNG state for one generator. `state` is the xorshift64* word; `seeded`
is false until the generator has taken a seed (the global one seeds
itself from OS entropy on first use).

```fides
pub struct RngState {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |
| `seeded` | `bool` |

*Defined at line 27*

---

## RngState

```fides
impl RngState {
```

**Methods:**

### new

A generator with a deterministic sequence for `seed`.

```fides
pub fn new(seed: i64) -> RngState {
```

### new_os

A generator seeded from OS entropy (different sequence every run).

```fides
pub fn new_os() -> RngState {
```

### next

Next raw word of the xorshift64* sequence.

```fides
pub fn next(self: &mut RngState) -> i64 {
```

### next_bounded

A value in [0, bound) with no modulo bias: a power-of-two window of
raw bits, resampled on rejection. bound <= 0 gives 0.

```fides
pub fn next_bounded(self: &mut RngState, bound: i64) -> i64 {
```

*Defined at line 55*

---

## random_seed

Seed the global generator with a fixed seed (deterministic sequence).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(42);
var a = random_int(1000);
random_seed(42);
assert_eq(random_int(1000), a);
```

```fides
pub fn random_seed(seed: i64) -> void {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `void`

*Defined at line 120*

---

## random_seed_time

Seed with a per-run seed: FIDES_TEST_SEED when set (test determinism),
else OS entropy mixed with the clock.

```fides
pub fn random_seed_time() -> void {
```

**Returns:** `void`

*Defined at line 127*

---

## random_raw

Generate the next raw 64-bit word of the global generator.

```fides
pub fn random_raw() -> i64 {
```

**Returns:** `i64`

*Defined at line 145*

---

## random_int

Generate a random i64 in [0, max) (unbiased; the low bits are not just
taken modulo max).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(7);
var i = 0;
while i < 100 {
  var r = random_int(6);
  assert(r >= 0 && r < 6);
  i = i + 1;
}
```

```fides
pub fn random_int(max: i64) -> i64 {
```

**Parameters:**

- `max`: `i64`

**Returns:** `i64`

*Defined at line 160*

---

## random_int_range

Generate a random i64 in [min, max].


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(7);
var r = random_int_range(10, 20);
assert(r >= 10 && r <= 20);
```

```fides
pub fn random_int_range(min: i64, max: i64) -> i64 {
```

**Parameters:**

- `min`: `i64`
- `max`: `i64`

**Returns:** `i64`

*Defined at line 170*

---

## random_i64

Generate a random i64 over the whole range (any bits may be set).

```fides
pub fn random_i64() -> i64 {
```

**Returns:** `i64`

*Defined at line 178*

---

## random_float

Generate a random f64 in [0.0, 1.0): 53 raw bits over 2^53, so 1.0 is
never returned.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(3);
var f = random_float();
assert(f >= 0.0 && f < 1.0);
```

```fides
pub fn random_float() -> f64 {
```

**Returns:** `f64`

*Defined at line 191*

---

## random_float_max

Generate a random f64 in [0.0, max).

```fides
pub fn random_float_max(max: f64) -> f64 {
```

**Parameters:**

- `max`: `f64`

**Returns:** `f64`

*Defined at line 197*

---

## random_float_range

Generate a random f64 in [min, max).

```fides
pub fn random_float_range(min: f64, max: f64) -> f64 {
```

**Parameters:**

- `min`: `f64`
- `max`: `f64`

**Returns:** `f64`

*Defined at line 202*

---

## random_gaussian

Generate a Gaussian (normal) sample with the given mean and standard
deviation (Box-Muller).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(3);
var g = random_gaussian(0.0, 1.0);
```

```fides
pub fn random_gaussian(mean: f64, stddev: f64) -> f64 {
```

**Parameters:**

- `mean`: `f64`
- `stddev`: `f64`

**Returns:** `f64`

*Defined at line 212*

---

## random_bool

Generate a random boolean (a whole bounded draw, not a fixed bit of the
sequence, so consecutive calls do not correlate).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(9);
var t = 0;
var i = 0;
while i < 100 {
  if random_bool() { t = t + 1; }
  i = i + 1;
}
assert(t > 0 && t < 100);
```

```fides
pub fn random_bool() -> bool {
```

**Returns:** `bool`

*Defined at line 236*

---

## random_choice

Pick a random element from a Vec. Returns 0 if empty.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(5);
var v: Vec<i64> = Vec::new();
v.push(10); v.push(20); v.push(30);
var c = random_choice(v);
assert(c == 10 || c == 20 || c == 30);
```

```fides
pub fn random_choice(v: Vec<i64>) -> i64 {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 248*

---

## random_choice_idx

Pick a random index from a Vec of the given length.

```fides
pub fn random_choice_idx(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 256*

---

## random_shuffle

Shuffle a Vec in place (Fisher-Yates).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(11);
var v: Vec<i64> = Vec::new();
v.push(1); v.push(2); v.push(3); v.push(4);
random_shuffle(&mut v);
assert_eq(v.len(), 4);
assert_eq(v.get(0) + v.get(1) + v.get(2) + v.get(3), 10);
```

```fides
pub fn random_shuffle(v: &mut Vec<i64>) -> void {
```

**Parameters:**

- `v`: `&mut Vec<i64>`

**Returns:** `void`

*Defined at line 271*

---

## random_byte

Generate a random byte (0-255).


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(2);
var b = random_byte();
assert(b >= 0 && b <= 255);
```

```fides
pub fn random_byte() -> i64 {
```

**Returns:** `i64`

*Defined at line 290*

---

## random_bytes

Generate `n` random bytes as a Vec of values 0-255.

```fides
pub fn random_bytes(n: i64) -> Vec<i64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 295*

---

## random_hex

Generate a random hex string of the given length.


*Example -- run as a test by `scripts/doctest.py`:*

```fides
random_seed(2);
var h = random_hex(8);
assert_eq(str_len(h), 8);
```

```fides
pub fn random_hex(length: i64) -> str {
```

**Parameters:**

- `length`: `i64`

**Returns:** `str`

*Defined at line 311*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
