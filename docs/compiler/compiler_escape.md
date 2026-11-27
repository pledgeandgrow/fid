[All modules](index.md)

# compiler::escape -- Fides Documentation

## Contents

- [EscRoot](#EscRoot) (struct)
- [EscVar](#EscVar) (struct)
- [OptEscVar](#OptEscVar) (enum)
- [EscFunc](#EscFunc) (struct)
- [EscapeChecker](#EscapeChecker) (struct)
- [esc_contains_ref](#esc_contains_ref) (function)
- [EscapeChecker_new](#EscapeChecker_new) (function)
- [esc_idents_stmts](#esc_idents_stmts) (function)
- [esc_par_workers](#esc_par_workers) (function)
- [par_own_str](#par_own_str) (function)
- [par_own_strs](#par_own_strs) (function)
- [par_own_strss](#par_own_strss) (function)
- [par_own_i64s](#par_own_i64s) (function)
- [par_own_i64ss](#par_own_i64ss) (function)
- [par_own_bools](#par_own_bools) (function)
- [par_own_boolss](#par_own_boolss) (function)
- [par_unit_group](#par_unit_group) (function)
- [par_chunk_bounds](#par_chunk_bounds) (function)
- [esc_par_worker](#esc_par_worker) (function)
- [escape_check](#escape_check) (function)
- [escape_check_skipped](#escape_check_skipped) (function)

## EscRoot

What a reference-holding value points into: a local variable (or
by-value parameter) declared at block depth `depth`, or the function's
`index`-th parameter, a reference the caller supplied.

```fides
pub struct EscRoot {
```

**Fields:**

| Name | Type |
|------|------|
| `is_param` | `bool` |
| `name` | `str` |
| `depth` | `i64` |
| `index` | `i64` |

*Defined at line 24*

---

## EscVar

```fides
pub struct EscVar {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `depth` | `i64` |
| `roots` | `Vec<EscRoot>` |
| `is_ref` | `bool` |
| `fn_noref` | `bool` |

*Defined at line 31*

---

## OptEscVar

```fides
pub enum OptEscVar {
```

**Variants:**

- `Some`
- `None`

*Defined at line 45*

---

## EscFunc

A function body with the name it is called by (`Type_method` for methods).

```fides
pub struct EscFunc {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `skey` | `str` |
| `func` | `Func` |

*Defined at line 51*

---

## EscapeChecker

```fides
pub struct EscapeChecker {
```

**Fields:**

| Name | Type |
|------|------|
| `all` | `Vec<EscFunc>` |
| `func_index` | `i64` |
| `method_index` | `i64` |
| `method_keys` | `Vec<Vec<str>>` |
| `globals` | `i64` |
| `summary_index` | `i64` |
| `summaries` | `Vec<Vec<i64>>` |
| `scopes` | `Vec<Vec<EscVar>>` |
| `func_name` | `str` |
| `flows` | `Vec<i64>` |
| `loop_depths` | `Vec<i64>` |
| `closure_base` | `i64` |
| `unsafe_depth` | `i64` |
| `line` | `i64` |
| `report` | `bool` |
| `errors` | `Vec<str>` |
| `type_names` | `i64` |
| `ret_has_ref` | `Vec<bool>` |
| `ret_is_ref` | `Vec<bool>` |
| `param_has_ref` | `Vec<Vec<bool>>,  // esc_contains_ref(params[j].ty)` |
| `has_decl_flows` | `Vec<bool>` |
| `decl_flows` | `Vec<Vec<i64>>` |
| `param_names` | `Vec<Vec<str>>` |
| `param_rt` | `Vec<Vec<bool>>` |
| `par` | `bool` |
| `param_tys` | `Vec<Vec<str>>` |
| `ret_noref` | `Vec<bool>` |
| `ret_tparam` | `Vec<bool>` |

*Defined at line 58*

---

## esc_contains_ref

Whether `t` holds a reference anywhere (function types do not hold their
parameters' or result's references).

```fides
pub fn esc_contains_ref(t: Type) -> bool {
```

**Parameters:**

- `t`: `Type`

**Returns:** `bool`

*Defined at line 358*

---

## EscapeChecker_new

```fides
pub fn EscapeChecker_new(program: Program) -> EscapeChecker {
```

**Parameters:**

- `program`: `Program`

**Returns:** `EscapeChecker`

*Defined at line 484*

---

## esc_idents_stmts

```fides
pub fn esc_idents_stmts(stmts: Vec<Stmt>, out: &mut Vec<str>) {
```

**Parameters:**

- `stmts`: `Vec<Stmt>`
- `out`: `&mut Vec<str>`

*Defined at line 2153*

---

## esc_par_workers

The worker count FID_PAR asks for: 1 (sequential) unless it is a number
of 2 or more (16 at most).

```fides
pub fn esc_par_workers() -> i64 {
```

**Returns:** `i64`

*Defined at line 2458*

---

## par_own_str

`s` in a buffer of its own: it shares no owner count with `s`.

```fides
pub fn par_own_str(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 2484*

---

## par_own_strs

```fides
pub fn par_own_strs(v: Vec<str>) -> Vec<str> {
```

**Parameters:**

- `v`: `Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 2488*

---

## par_own_strss

```fides
pub fn par_own_strss(v: Vec<Vec<str>>) -> Vec<Vec<str>> {
```

**Parameters:**

- `v`: `Vec<Vec<str>>`

**Returns:** `Vec<Vec<str>>`

*Defined at line 2498*

---

## par_own_i64s

```fides
pub fn par_own_i64s(v: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `v`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2508*

---

## par_own_i64ss

```fides
pub fn par_own_i64ss(v: Vec<Vec<i64>>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `v`: `Vec<Vec<i64>>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 2518*

---

## par_own_bools

```fides
pub fn par_own_bools(v: Vec<bool>) -> Vec<bool> {
```

**Parameters:**

- `v`: `Vec<bool>`

**Returns:** `Vec<bool>`

*Defined at line 2528*

---

## par_own_boolss

```fides
pub fn par_own_boolss(v: Vec<Vec<bool>>) -> Vec<Vec<bool>> {
```

**Parameters:**

- `v`: `Vec<Vec<bool>>`

**Returns:** `Vec<Vec<bool>>`

*Defined at line 2538*

---

## par_unit_group

The group a reporting unit (by its skip-set key) must share a chunk
with: "" for a free function (any chunk), "impl:S" for a method of an
impl block of S, "trait:" for every trait impl's method.

```fides
pub fn par_unit_group(skey: str) -> str {
```

**Parameters:**

- `skey`: `str`

**Returns:** `str`

*Defined at line 2551*

---

## par_chunk_bounds

Chunks of `skeys` (in order, about n / nw each, none splitting a group
of par_unit_group) as [start0, end0, start1, end1, ...]; nw at most.

```fides
pub fn par_chunk_bounds(skeys: Vec<str>, nw: i64) -> Vec<i64> {
```

**Parameters:**

- `skeys`: `Vec<str>`
- `nw`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2567*

---

## esc_par_worker

A worker of the parallel reporting pass: units s..e of g_esc_par_units,
with a checker of its own; its messages in order. The units are passed
to esc_analyze_func, never kept (that would copy what the master holds).

```fides
pub fn esc_par_worker(s: i64, e: i64) -> Vec<str> {
```

**Parameters:**

- `s`: `i64`
- `e`: `i64`

**Returns:** `Vec<str>`

*Defined at line 2608*

---

## escape_check

Run the escape check over every function of `program`.

```fides
pub fn escape_check(program: Program) -> Vec<str> {
```

**Parameters:**

- `program`: `Program`

**Returns:** `Vec<str>`

*Defined at line 2682*

---

## escape_check_skipped

```fides
pub fn escape_check_skipped(program: Program, skip: i64) -> Vec<str> {
```

**Parameters:**

- `program`: `Program`
- `skip`: `i64`

**Returns:** `Vec<str>`

*Defined at line 2689*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
