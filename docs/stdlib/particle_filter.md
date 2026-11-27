[All modules](index.md)

# particle_filter -- Fides Documentation

## Contents

- [PfRng](#PfRng) (struct)
- [pfrng_new](#pfrng_new) (function)
- [pfrng_next](#pfrng_next) (function)
- [pfrng_uniform](#pfrng_uniform) (function)
- [pfrng_gauss](#pfrng_gauss) (function)
- [ParticleFilter](#ParticleFilter) (struct)
- [pf_new](#pf_new) (function)
- [pf_get](#pf_get) (function)
- [pf_set](#pf_set) (function)
- [pf_particle](#pf_particle) (function)
- [pf_weight_of](#pf_weight_of) (function)
- [pf_reset_weights](#pf_reset_weights) (function)
- [pf_init_uniform](#pf_init_uniform) (function)
- [pf_init_gaussian](#pf_init_gaussian) (function)
- [pf_predict_shift](#pf_predict_shift) (function)
- [pf_predict](#pf_predict) (function)
- [pf_normalize](#pf_normalize) (function)
- [pf_weight](#pf_weight) (function)
- [pf_neff](#pf_neff) (function)
- [pf_neff_of](#pf_neff_of) (function)
- [pf_systematic_indices](#pf_systematic_indices) (function)
- [pf_resample](#pf_resample) (function)
- [pf_resample_if_needed](#pf_resample_if_needed) (function)
- [pf_mean](#pf_mean) (function)
- [pf_variance](#pf_variance) (function)
- [pf_best](#pf_best) (function)
- [pf_gauss_likelihood](#pf_gauss_likelihood) (function)

## PfRng

SplitMix64 state.

```fides
pub struct PfRng {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `i64` |

*Defined at line 34*

---

## pfrng_new

A generator seeded with `seed`.

```fides
pub fn pfrng_new(seed: i64) -> PfRng {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `PfRng`

*Defined at line 39*

---

## pfrng_next

```fides
pub fn pfrng_next(r: &mut PfRng) -> i64 {
```

**Parameters:**

- `r`: `&mut PfRng`

**Returns:** `i64`

*Defined at line 47*

---

## pfrng_uniform

```fides
pub fn pfrng_uniform(r: &mut PfRng) -> f64 {
```

**Parameters:**

- `r`: `&mut PfRng`

**Returns:** `f64`

*Defined at line 58*

---

## pfrng_gauss

A standard normal draw (Box-Muller, one of the pair).

```fides
pub fn pfrng_gauss(r: &mut PfRng) -> f64 {
```

**Parameters:**

- `r`: `&mut PfRng`

**Returns:** `f64`

*Defined at line 64*

---

## ParticleFilter

n particles of dim floats each, their weights and the generator.

```fides
pub struct ParticleFilter {
```

**Fields:**

| Name | Type |
|------|------|
| `n` | `i64` |
| `dim` | `i64` |
| `states` | `Vec<f64>` |
| `weights` | `Vec<f64>` |
| `rng` | `PfRng` |
| `resamples` | `i64` |

*Defined at line 73*

---

## pf_new

n particles of dimension dim, all zero with equal weights.

```fides
pub fn pf_new(n: i64, dim: i64, seed: i64) -> ParticleFilter {
```

**Parameters:**

- `n`: `i64`
- `dim`: `i64`
- `seed`: `i64`

**Returns:** `ParticleFilter`

*Defined at line 83*

---

## pf_get

Component d of particle i.

```fides
pub fn pf_get(pf: &ParticleFilter, i: i64, d: i64) -> f64 {
```

**Parameters:**

- `pf`: `&ParticleFilter`
- `i`: `i64`
- `d`: `i64`

**Returns:** `f64`

*Defined at line 102*

---

## pf_set

Set component d of particle i.

```fides
pub fn pf_set(pf: &mut ParticleFilter, i: i64, d: i64, v: f64) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `i`: `i64`
- `d`: `i64`
- `v`: `f64`

*Defined at line 107*

---

## pf_particle

Particle i as a vector.

```fides
pub fn pf_particle(pf: &ParticleFilter, i: i64) -> Vec<f64> {
```

**Parameters:**

- `pf`: `&ParticleFilter`
- `i`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 112*

---

## pf_weight_of

Weight of particle i.

```fides
pub fn pf_weight_of(pf: &ParticleFilter, i: i64) -> f64 {
```

**Parameters:**

- `pf`: `&ParticleFilter`
- `i`: `i64`

**Returns:** `f64`

*Defined at line 123*

---

## pf_reset_weights

Give every particle the weight 1/n.

```fides
pub fn pf_reset_weights(pf: &mut ParticleFilter) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`

*Defined at line 128*

---

## pf_init_uniform

Particles uniform in the box [lo, hi] (one bound per dimension), equal
weights.

```fides
pub fn pf_init_uniform(pf: &mut ParticleFilter, lo: &Vec<f64>, hi: &Vec<f64>) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `lo`: `&Vec<f64>`
- `hi`: `&Vec<f64>`

*Defined at line 138*

---

## pf_init_gaussian

Particles drawn from N(mean, diag(std^2)), equal weights.

```fides
pub fn pf_init_gaussian(pf: &mut ParticleFilter, mean: &Vec<f64>, std: &Vec<f64>) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `mean`: `&Vec<f64>`
- `std`: `&Vec<f64>`

*Defined at line 153*

---

## pf_predict_shift

Move every particle by `delta` plus Gaussian noise of `std` per dimension.

```fides
pub fn pf_predict_shift(pf: &mut ParticleFilter, delta: &Vec<f64>, std: &Vec<f64>) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `delta`: `&Vec<f64>`
- `std`: `&Vec<f64>`

*Defined at line 168*

---

## pf_predict

Replace every particle x by f(x, u) plus Gaussian noise of `std` per
dimension.

```fides
pub fn pf_predict(pf: &mut ParticleFilter, f: fn(Vec<f64>, Vec<f64>) -> Vec<f64>, u: &Vec<f64>, std: &Vec<f64>) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `f`: `fn(Vec<f64>`
- `u`: `&Vec<f64>`
- `std`: `&Vec<f64>`

*Defined at line 184*

---

## pf_normalize

Scale the weights to sum to 1; returns the sum before. A zero (or
non-finite) sum resets them to uniform.

```fides
pub fn pf_normalize(pf: &mut ParticleFilter) -> f64 {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`

**Returns:** `f64`

*Defined at line 206*

---

## pf_weight

Multiply each weight by likelihood(particle, z) and normalize. Returns
the mean likelihood before normalization (0: every particle was ruled
out and the weights were reset).

```fides
pub fn pf_weight(pf: &mut ParticleFilter, likelihood: fn(Vec<f64>, Vec<f64>) -> f64, z: &Vec<f64>) -> f64 {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `likelihood`: `fn(Vec<f64>`
- `z`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 228*

---

## pf_neff

The effective sample size 1 / sum w^2 (weights normalized).

```fides
pub fn pf_neff(pf: &ParticleFilter) -> f64 {
```

**Parameters:**

- `pf`: `&ParticleFilter`

**Returns:** `f64`

*Defined at line 245*

---

## pf_neff_of

1 / sum w^2 of normalized weights.

```fides
pub fn pf_neff_of(w: &Vec<f64>) -> f64 {
```

**Parameters:**

- `w`: `&Vec<f64>`

**Returns:** `f64`

*Defined at line 250*

---

## pf_systematic_indices

The systematic resampling of normalized weights w with the draw u0 in
[0, 1/N): for each k the index i where the cumulative weight first
reaches u0 + k/N.

```fides
pub fn pf_systematic_indices(w: &Vec<f64>, u0: f64) -> Vec<i64> {
```

**Parameters:**

- `w`: `&Vec<f64>`
- `u0`: `f64`

**Returns:** `Vec<i64>`

*Defined at line 266*

---

## pf_resample

Systematic resampling: draw u0, copy the chosen particles, equal weights.

```fides
pub fn pf_resample(pf: &mut ParticleFilter) {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`

*Defined at line 285*

---

## pf_resample_if_needed

Resample when N_eff < ratio * N; returns whether it did.

```fides
pub fn pf_resample_if_needed(pf: &mut ParticleFilter, ratio: f64) -> bool {
```

**Parameters:**

- `pf`: `&mut ParticleFilter`
- `ratio`: `f64`

**Returns:** `bool`

*Defined at line 305*

---

## pf_mean

The weighted mean state.

```fides
pub fn pf_mean(pf: &ParticleFilter) -> Vec<f64> {
```

**Parameters:**

- `pf`: `&ParticleFilter`

**Returns:** `Vec<f64>`

*Defined at line 314*

---

## pf_variance

The weighted variance of each component.

```fides
pub fn pf_variance(pf: &ParticleFilter) -> Vec<f64> {
```

**Parameters:**

- `pf`: `&ParticleFilter`

**Returns:** `Vec<f64>`

*Defined at line 331*

---

## pf_best

The index of the heaviest particle.

```fides
pub fn pf_best(pf: &ParticleFilter) -> i64 {
```

**Parameters:**

- `pf`: `&ParticleFilter`

**Returns:** `i64`

*Defined at line 350*

---

## pf_gauss_likelihood

exp(-(x - mu)^2 / (2 sigma^2)): an unnormalized Gaussian likelihood.

```fides
pub fn pf_gauss_likelihood(x: f64, mu: f64, sigma: f64) -> f64 {
```

**Parameters:**

- `x`: `f64`
- `mu`: `f64`
- `sigma`: `f64`

**Returns:** `f64`

*Defined at line 363*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
