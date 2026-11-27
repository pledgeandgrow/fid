[All modules](index.md)

# entropy -- Fides Documentation

## Contents

- [entropy_fmt](#entropy_fmt) (function)
- [entropy_erfc](#entropy_erfc) (function)
- [entropy_normal_cdf](#entropy_normal_cdf) (function)
- [entropy_lgamma](#entropy_lgamma) (function)
- [entropy_igamc](#entropy_igamc) (function)
- [entropy_histogram](#entropy_histogram) (function)
- [entropy_shannon](#entropy_shannon) (function)
- [entropy_min_mcv](#entropy_min_mcv) (function)
- [entropy_chi_square](#entropy_chi_square) (function)
- [entropy_chi_square_p](#entropy_chi_square_p) (function)
- [entropy_mean](#entropy_mean) (function)
- [entropy_serial_correlation](#entropy_serial_correlation) (function)
- [entropy_bits](#entropy_bits) (function)
- [entropy_bits_from_str](#entropy_bits_from_str) (function)
- [entropy_monobit](#entropy_monobit) (function)
- [entropy_block_frequency](#entropy_block_frequency) (function)
- [entropy_runs](#entropy_runs) (function)
- [entropy_longest_run](#entropy_longest_run) (function)
- [entropy_cusum](#entropy_cusum) (function)
- [entropy_report](#entropy_report) (function)

## entropy_fmt

A non-negative-or-negative float with `digits` decimals, rounded half up.

```fides
pub fn entropy_fmt(x: f64, digits: i64) -> str {
```

**Parameters:**

- `x`: `f64`
- `digits`: `i64`

**Returns:** `str`

*Defined at line 38*

---

## entropy_erfc

The complementary error function erfc(x) (Cody 1969).

```fides
pub fn entropy_erfc(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 88*

---

## entropy_normal_cdf

The standard normal distribution function.

```fides
pub fn entropy_normal_cdf(x: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `f64`

*Defined at line 163*

---

## entropy_lgamma

ln Gamma(x) for x > 0: Stirling's series from x >= 10 (error below
1e-15), shifted up by the recurrence Gamma(x + 1) = x Gamma(x) below.

```fides
pub fn entropy_lgamma(x_in: f64) -> f64 {
```

**Parameters:**

- `x_in`: `f64`

**Returns:** `f64`

*Defined at line 169*

---

## entropy_igamc

The regularized upper incomplete gamma function Q(a, x) ("igamc").

```fides
pub fn entropy_igamc(a: f64, x: f64) -> f64 {
```

**Parameters:**

- `a`: `f64`
- `x`: `f64`

**Returns:** `f64`

*Defined at line 182*

---

## entropy_histogram

Counts of each byte value (256 entries).

```fides
pub fn entropy_histogram(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 236*

---

## entropy_shannon

Shannon entropy in bits per byte (0 for empty input, 8 at most).

```fides
pub fn entropy_shannon(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 257*

---

## entropy_min_mcv

SP 800-90B §6.3.1 most-common-value estimate: -log2 of the 99% upper
bound on the probability of the most common byte (bits per byte).

```fides
pub fn entropy_min_mcv(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 277*

---

## entropy_chi_square

Chi-square statistic of the byte counts against a uniform distribution.

```fides
pub fn entropy_chi_square(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 300*

---

## entropy_chi_square_p

The p-value of the chi-square statistic with 255 degrees of freedom.

```fides
pub fn entropy_chi_square_p(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 317*

---

## entropy_mean

Arithmetic mean of the bytes (127.5 for uniform data).

```fides
pub fn entropy_mean(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 322*

---

## entropy_serial_correlation

Serial correlation coefficient of successive bytes (wrapping around,
as `ent` computes it): near 0 for random data, 1 for a constant ramp.

```fides
pub fn entropy_serial_correlation(data: Vec<i64>) -> f64 {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 337*

---

## entropy_bits

The bits of the bytes, most significant first (0 or 1 each).

```fides
pub fn entropy_bits(data: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 365*

---

## entropy_bits_from_str

Bits from a text of '0' and '1' (other characters are skipped).

```fides
pub fn entropy_bits_from_str(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 380*

---

## entropy_monobit

§2.1 Frequency (monobit) test: p-value.

```fides
pub fn entropy_monobit(bits: Vec<i64>) -> f64 {
```

**Parameters:**

- `bits`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 397*

---

## entropy_block_frequency

§2.2 Frequency test within blocks of m bits: p-value.

```fides
pub fn entropy_block_frequency(bits: Vec<i64>, m: i64) -> f64 {
```

**Parameters:**

- `bits`: `Vec<i64>`
- `m`: `i64`

**Returns:** `f64`

*Defined at line 416*

---

## entropy_runs

§2.3 Runs test: p-value (0 when the frequency prerequisite fails).

```fides
pub fn entropy_runs(bits: Vec<i64>) -> f64 {
```

**Parameters:**

- `bits`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 439*

---

## entropy_longest_run

§2.4 Longest run of ones in a block: p-value. The block size follows
the table of §2.4.2: M = 8 (n >= 128), 128 (n >= 6272), 10^4 (n >= 750000).

```fides
pub fn entropy_longest_run(bits: Vec<i64>) -> f64 {
```

**Parameters:**

- `bits`: `Vec<i64>`

**Returns:** `f64`

*Defined at line 470*

---

## entropy_cusum

§2.13 Cumulative sums test, forward (reverse = false) or backward:
p-value.

```fides
pub fn entropy_cusum(bits: Vec<i64>, reverse: bool) -> f64 {
```

**Parameters:**

- `bits`: `Vec<i64>`
- `reverse`: `bool`

**Returns:** `f64`

*Defined at line 552*

---

## entropy_report

A one-line summary of a buffer: entropy, min-entropy, chi-square
p-value, monobit and runs p-values (6 decimals each).

```fides
pub fn entropy_report(data: Vec<i64>) -> str {
```

**Parameters:**

- `data`: `Vec<i64>`

**Returns:** `str`

*Defined at line 599*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
