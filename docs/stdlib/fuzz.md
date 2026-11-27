[All modules](index.md)

# fuzz -- Fides Documentation

## Contents

- [FuzzRng](#FuzzRng) (struct)
- [Fuzzer](#Fuzzer) (struct)
- [FuzzReport](#FuzzReport) (struct)
- [fuzz_rng_new](#fuzz_rng_new) (function)
- [fuzz_rng_next](#fuzz_rng_next) (function)
- [fuzz_rng_below](#fuzz_rng_below) (function)
- [fuzz_mut_bitflip](#fuzz_mut_bitflip) (function)
- [fuzz_mut_byte](#fuzz_mut_byte) (function)
- [fuzz_mut_insert](#fuzz_mut_insert) (function)
- [fuzz_mut_delete](#fuzz_mut_delete) (function)
- [fuzz_mut_delete_chunk](#fuzz_mut_delete_chunk) (function)
- [fuzz_mut_dup_chunk](#fuzz_mut_dup_chunk) (function)
- [fuzz_mut_arith](#fuzz_mut_arith) (function)
- [fuzz_interesting](#fuzz_interesting) (function)
- [fuzz_mut_interesting](#fuzz_mut_interesting) (function)
- [fuzz_mut_dict](#fuzz_mut_dict) (function)
- [fuzz_mut_splice](#fuzz_mut_splice) (function)
- [fuzz_mutate_once](#fuzz_mutate_once) (function)
- [fuzz_hash](#fuzz_hash) (function)
- [fuzz_new](#fuzz_new) (function)
- [fuzz_add_seed](#fuzz_add_seed) (function)
- [fuzz_add_dict](#fuzz_add_dict) (function)
- [fuzz_edge_count](#fuzz_edge_count) (function)
- [fuzz_crash_code](#fuzz_crash_code) (function)
- [fuzz_edges_of](#fuzz_edges_of) (function)
- [fuzz_run](#fuzz_run) (function)
- [fuzz_corpus_cover](#fuzz_corpus_cover) (function)
- [fuzz_corpus_minimize](#fuzz_corpus_minimize) (function)
- [fuzz_minimize](#fuzz_minimize) (function)
- [fuzz_simplify](#fuzz_simplify) (function)
- [fuzz_edge](#fuzz_edge) (function)

## FuzzRng

The deterministic generator: splitmix64.

```fides
pub struct FuzzRng {
```

**Fields:**

| Name | Type |
|------|------|
| `state` | `u64` |

*Defined at line 51*

---

## Fuzzer

```fides
pub struct Fuzzer {
```

**Fields:**

| Name | Type |
|------|------|
| `rng` | `FuzzRng` |
| `corpus` | `Vec<Vec<i64>>` |
| `corpus_edges` | `Vec<Vec<i64>>` |
| `seen` | `HashMap<i64` |
| `hashes` | `HashMap<i64` |
| `dict` | `Vec<Vec<i64>>` |
| `max_len` | `i64` |
| `execs` | `i64` |
| `crashes` | `Vec<Vec<i64>>` |
| `crash_codes` | `Vec<i64>` |
| `stop_on_crash` | `bool` |

*Defined at line 55*

---

## FuzzReport

The outcome of fuzz_run. `crash_input` is the first crash of this run.

```fides
pub struct FuzzReport {
```

**Fields:**

| Name | Type |
|------|------|
| `execs` | `i64` |
| `edges` | `i64` |
| `corpus` | `i64` |
| `crashed` | `bool` |
| `crash_input` | `Vec<i64>` |
| `crash_code` | `i64` |
| `crash_exec` | `i64` |

*Defined at line 72*

---

## fuzz_rng_new

```fides
pub fn fuzz_rng_new(seed: i64) -> FuzzRng {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `FuzzRng`

*Defined at line 84*

---

## fuzz_rng_next

The next 64 random bits.

```fides
pub fn fuzz_rng_next(r: &mut FuzzRng) -> u64 {
```

**Parameters:**

- `r`: `&mut FuzzRng`

**Returns:** `u64`

*Defined at line 104*

---

## fuzz_rng_below

A number in 0..n-1 (n >= 1).

```fides
pub fn fuzz_rng_below(r: &mut FuzzRng, n: i64) -> i64 {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `n`: `i64`

**Returns:** `i64`

*Defined at line 110*

---

## fuzz_mut_bitflip

Flip one bit.

```fides
pub fn fuzz_mut_bitflip(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 152*

---

## fuzz_mut_byte

Replace one byte with a random one.

```fides
pub fn fuzz_mut_byte(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 164*

---

## fuzz_mut_insert

Insert a random byte.

```fides
pub fn fuzz_mut_insert(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 175*

---

## fuzz_mut_delete

Delete one byte.

```fides
pub fn fuzz_mut_delete(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 182*

---

## fuzz_mut_delete_chunk

Delete a chunk of 1..len/2 bytes.

```fides
pub fn fuzz_mut_delete_chunk(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 191*

---

## fuzz_mut_dup_chunk

Copy a chunk of the input to another place in it.

```fides
pub fn fuzz_mut_dup_chunk(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 201*

---

## fuzz_mut_arith

Add or subtract 1..16 from one byte (mod 256).

```fides
pub fn fuzz_mut_arith(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 217*

---

## fuzz_interesting

The interesting values of AFL, as unsigned numbers of 1, 2 or 4 bytes.

```fides
pub fn fuzz_interesting(width: i64) -> Vec<i64> {
```

**Parameters:**

- `width`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 232*

---

## fuzz_mut_interesting

Overwrite 1, 2 or 4 bytes with an interesting value (random byte order).

```fides
pub fn fuzz_mut_interesting(r: &mut FuzzRng, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 270*

---

## fuzz_mut_dict

Insert a dictionary token (or overwrite with it, half the time).

```fides
pub fn fuzz_mut_dict(r: &mut FuzzRng, b: &Vec<i64>, dict: &Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`
- `dict`: `&Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 296*

---

## fuzz_mut_splice

The head of `a` and the tail of `b`, cut at random points.

```fides
pub fn fuzz_mut_splice(r: &mut FuzzRng, a: &Vec<i64>, b: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 308*

---

## fuzz_mutate_once

One random mutation (corpus gives splice partners).

```fides
pub fn fuzz_mutate_once(r: &mut FuzzRng, b: &Vec<i64>, dict: &Vec<Vec<i64>>, corpus: &Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut FuzzRng`
- `b`: `&Vec<i64>`
- `dict`: `&Vec<Vec<i64>>`
- `corpus`: `&Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 326*

---

## fuzz_hash

```fides
pub fn fuzz_hash(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 362*

---

## fuzz_new

```fides
pub fn fuzz_new(seed: i64) -> Fuzzer {
```

**Parameters:**

- `seed`: `i64`

**Returns:** `Fuzzer`

*Defined at line 372*

---

## fuzz_add_seed

Add a seed input (kept even before it is run; its edges are recorded
the first time fuzz_run sees it).

```fides
pub fn fuzz_add_seed(f: &mut Fuzzer, b: Vec<i64>) {
```

**Parameters:**

- `f`: `&mut Fuzzer`
- `b`: `Vec<i64>`

*Defined at line 380*

---

## fuzz_add_dict

```fides
pub fn fuzz_add_dict(f: &mut Fuzzer, token: Vec<i64>) {
```

**Parameters:**

- `f`: `&mut Fuzzer`
- `token`: `Vec<i64>`

*Defined at line 390*

---

## fuzz_edge_count

The number of distinct edges seen.

```fides
pub fn fuzz_edge_count(f: &Fuzzer) -> i64 {
```

**Parameters:**

- `f`: `&Fuzzer`

**Returns:** `i64`

*Defined at line 397*

---

## fuzz_crash_code

The crash code a target reported (0: no crash).

```fides
pub fn fuzz_crash_code(res: &Vec<i64>) -> i64 {
```

**Parameters:**

- `res`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 402*

---

## fuzz_edges_of

The edges a target reported (its non-negative ids).

```fides
pub fn fuzz_edges_of(res: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `res`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 414*

---

## fuzz_run

Run `iterations` mutated inputs (the seeds are run first). Stops at the
first crash when `stop_on_crash` (the default).

```fides
pub fn fuzz_run(f: &mut Fuzzer, target: fn(Vec<i64>) -> Vec<i64>, iterations: i64) -> FuzzReport {
```

**Parameters:**

- `f`: `&mut Fuzzer`
- `target`: `fn(Vec<i64>) -> Vec<i64>`
- `iterations`: `i64`

**Returns:** `FuzzReport`

*Defined at line 448*

---

## fuzz_corpus_cover

The indexes of a smallest-first greedy cover of every edge the corpus
reaches: keep these and drop the rest.

```fides
pub fn fuzz_corpus_cover(f: &Fuzzer) -> Vec<i64> {
```

**Parameters:**

- `f`: `&Fuzzer`

**Returns:** `Vec<i64>`

*Defined at line 532*

---

## fuzz_corpus_minimize

Replace the corpus with its cover; returns how many inputs were dropped.

```fides
pub fn fuzz_corpus_minimize(f: &mut Fuzzer) -> i64 {
```

**Parameters:**

- `f`: `&mut Fuzzer`

**Returns:** `i64`

*Defined at line 569*

---

## fuzz_minimize

The smallest input ddmin finds that still crashes with the same code
(1-minimal: removing any single byte loses the crash). Returns the input
unchanged when it does not crash.

```fides
pub fn fuzz_minimize(target: fn(Vec<i64>) -> Vec<i64>, input: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `target`: `fn(Vec<i64>) -> Vec<i64>`
- `input`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 617*

---

## fuzz_simplify

Crash minimization by byte simplification after ddmin: each byte that
can become `fill` (e.g. '0') while still crashing does.

```fides
pub fn fuzz_simplify(target: fn(Vec<i64>) -> Vec<i64>, input: Vec<i64>, fill: i64) -> Vec<i64> {
```

**Parameters:**

- `target`: `fn(Vec<i64>) -> Vec<i64>`
- `input`: `Vec<i64>`
- `fill`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 667*

---

## fuzz_edge

A coverage helper for targets: an edge id from a source location id and
the previous location (AFL's prev ^ cur).

```fides
pub fn fuzz_edge(prev: i64, cur: i64) -> i64 {
```

**Parameters:**

- `prev`: `i64`
- `cur`: `i64`

**Returns:** `i64`

*Defined at line 690*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
