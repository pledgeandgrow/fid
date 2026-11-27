[All modules](index.md)

# compiler::borrowck -- Fides Documentation

## Contents

- [SharedBorrowEntry](#SharedBorrowEntry) (struct)
- [SharedBorrowEntry_new](#SharedBorrowEntry_new) (function)
- [MutBorrowEntry](#MutBorrowEntry) (struct)
- [MutBorrowEntry_new](#MutBorrowEntry_new) (function)
- [BorrowOfEntry](#BorrowOfEntry) (struct)
- [BorrowOfEntry_new](#BorrowOfEntry_new) (function)
- [ReservedMutBorrow](#ReservedMutBorrow) (struct)
- [ReservedMutBorrow_new](#ReservedMutBorrow_new) (function)
- [StringI64Entry](#StringI64Entry) (struct)
- [StringI64Entry_new](#StringI64Entry_new) (function)
- [StringVec](#StringVec) (struct)
- [StringVec_new](#StringVec_new) (function)
- [bc_box_expr_unbox](#bc_box_expr_unbox) (function)
- [BorrowckMode](#BorrowckMode) (enum)
- [borrowck_mode_parse](#borrowck_mode_parse) (function)
- [OptBorrowckMode](#OptBorrowckMode) (enum)
- [BorrowChecker](#BorrowChecker) (struct)
- [BorrowChecker_free_maps](#BorrowChecker_free_maps) (function)
- [BorrowChecker_new](#BorrowChecker_new) (function)
- [bc_par_workers](#bc_par_workers) (function)
- [bc_par_worker](#bc_par_worker) (function)
- [bc_sig_ties](#bc_sig_ties) (function)
- [check](#check) (function)
- [check_func](#check_func) (function)
- [bc_skips](#bc_skips) (function)
- [pattern_bindings](#pattern_bindings) (function)
- [str_to_lower](#str_to_lower) (function)

## SharedBorrowEntry

```fides
pub struct SharedBorrowEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `borrowers` | `Vec<str>` |

*Defined at line 42*

---

## SharedBorrowEntry_new

```fides
pub fn SharedBorrowEntry_new(key: str) -> SharedBorrowEntry {
```

**Parameters:**

- `key`: `str`

**Returns:** `SharedBorrowEntry`

*Defined at line 47*

---

## MutBorrowEntry

```fides
pub struct MutBorrowEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `borrower` | `OptStr` |

*Defined at line 54*

---

## MutBorrowEntry_new

```fides
pub fn MutBorrowEntry_new(key: str, borrower: OptStr) -> MutBorrowEntry {
```

**Parameters:**

- `key`: `str`
- `borrower`: `OptStr`

**Returns:** `MutBorrowEntry`

*Defined at line 59*

---

## BorrowOfEntry

```fides
pub struct BorrowOfEntry {
```

**Fields:**

| Name | Type |
|------|------|
| `borrower` | `str` |
| `borrowed` | `str` |
| `is_mut` | `bool` |
| `line` | `i64` |

*Defined at line 66*

---

## BorrowOfEntry_new

```fides
pub fn BorrowOfEntry_new(borrower: str, borrowed: str, is_mut: bool, line: i64) -> BorrowOfEntry {
```

**Parameters:**

- `borrower`: `str`
- `borrowed`: `str`
- `is_mut`: `bool`
- `line`: `i64`

**Returns:** `BorrowOfEntry`

*Defined at line 73*

---

## ReservedMutBorrow

```fides
pub struct ReservedMutBorrow {
```

**Fields:**

| Name | Type |
|------|------|
| `path` | `str` |
| `borrower` | `str` |
| `line` | `i64` |

*Defined at line 80*

---

## ReservedMutBorrow_new

```fides
pub fn ReservedMutBorrow_new(path: str, borrower: str, line: i64) -> ReservedMutBorrow {
```

**Parameters:**

- `path`: `str`
- `borrower`: `str`
- `line`: `i64`

**Returns:** `ReservedMutBorrow`

*Defined at line 86*

---

## StringI64Entry

```fides
pub struct StringI64Entry {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `val` | `i64` |

*Defined at line 93*

---

## StringI64Entry_new

```fides
pub fn StringI64Entry_new(key: str, val: i64) -> StringI64Entry {
```

**Parameters:**

- `key`: `str`
- `val`: `i64`

**Returns:** `StringI64Entry`

*Defined at line 98*

---

## StringVec

```fides
pub struct StringVec {
```

**Fields:**

| Name | Type |
|------|------|
| `items` | `Vec<str>` |

*Defined at line 105*

---

## StringVec_new

```fides
pub fn StringVec_new() -> StringVec {
```

**Returns:** `StringVec`

*Defined at line 109*

---

## bc_box_expr_unbox

```fides
pub fn bc_box_expr_unbox(b: Box<Expr>) -> Expr {
```

**Parameters:**

- `b`: `Box<Expr>`

**Returns:** `Expr`

*Defined at line 120*

---

## BorrowckMode

```fides
pub enum BorrowckMode {
```

**Variants:**

- `Off`
- `Warn`
- `On`

*Defined at line 250*

---

## borrowck_mode_parse

```fides
pub fn borrowck_mode_parse(s: str) -> OptBorrowckMode {
```

**Parameters:**

- `s`: `str`

**Returns:** `OptBorrowckMode`

*Defined at line 257*

---

## OptBorrowckMode

```fides
pub enum OptBorrowckMode {
```

**Variants:**

- `Some`
- `None`

*Defined at line 295*

---

## BorrowChecker

```fides
pub struct BorrowChecker {
```

**Fields:**

| Name | Type |
|------|------|
| `shared_borrows` | `Vec<SharedBorrowEntry>` |
| `mut_borrow` | `Vec<MutBorrowEntry>` |
| `borrow_of` | `Vec<BorrowOfEntry>` |
| `scope_stack` | `Vec<StringVec>` |
| `unsafe_depth` | `i64` |
| `current_func` | `OptStr` |
| `temp_counter` | `i64` |
| `stmt_line` | `i64` |
| `temp_checkpoints` | `Vec<i64>` |
| `reserved_mut_borrows` | `Vec<ReservedMutBorrow>` |
| `in_reservation_phase` | `bool` |
| `ref_into_elem` | `bool` |
| `dead_at` | `Vec<StringVec>` |
| `last_use` | `Vec<StringI64Entry>` |
| `last_use_index` | `i64` |
| `sb_ix` | `i64` |
| `mb_ix` | `i64` |
| `bo_ix` | `i64` |
| `sb_pfx` | `i64` |
| `mb_pfx` | `i64` |
| `iterating` | `Vec<str>` |
| `skip_bodies` | `i64` |
| `stmt_index` | `i64` |
| `errors` | `Vec<str>` |
| `warnings` | `Vec<str>` |
| `mode` | `BorrowckMode` |
| `par_lt` | `bool` |
| `lt_ties` | `Vec<Vec<i64>>` |
| `lt_self_mut` | `Vec<bool>` |
| `mh_to` | `Vec<str>` |
| `mh_from` | `Vec<str>` |
| `alias_seen` | `bool` |
| `ref_vars` | `i64` |

*Defined at line 304*

---

## BorrowChecker_free_maps

Free the borrow checker's index maps (raw handles) once checked.

```fides
pub fn BorrowChecker_free_maps(self: &mut BorrowChecker) {
```

**Parameters:**

- `self`: `&mut BorrowChecker`

*Defined at line 410*

---

## BorrowChecker_new

```fides
pub fn BorrowChecker_new(mode: BorrowckMode) -> BorrowChecker {
```

**Parameters:**

- `mode`: `BorrowckMode`

**Returns:** `BorrowChecker`

*Defined at line 427*

---

## bc_par_workers

```fides
pub fn bc_par_workers() -> i64 {
```

**Returns:** `i64`

*Defined at line 446*

---

## bc_par_worker

```fides
pub fn bc_par_worker(s: i64, e: i64) -> Vec<str> {
```

**Parameters:**

- `s`: `i64`
- `e`: `i64`

**Returns:** `Vec<str>`

*Defined at line 480*

---

## bc_sig_ties

LG3: the parameters `f`'s result borrows from. With lifetimes, those its
signature ties (`func_lifetime_ties`); without lifetimes, a result of
reference type may borrow from any reference parameter (the escape check
infers which; the borrow checker takes all of them, which is sound).
Empty: the result borrows nothing.

```fides
pub fn bc_sig_ties(f: Func) -> Vec<i64> {
```

**Parameters:**

- `f`: `Func`

**Returns:** `Vec<i64>`

*Defined at line 537*

---

## check

```fides
pub fn check(self: &mut BorrowChecker, program: Program) -> void {
```

**Parameters:**

- `self`: `&mut BorrowChecker`
- `program`: `Program`

**Returns:** `void`

*Defined at line 1041*

---

## check_func

```fides
pub fn check_func(self: &mut BorrowChecker, func: Func) -> void {
```

**Parameters:**

- `self`: `&mut BorrowChecker`
- `func`: `Func`

**Returns:** `void`

*Defined at line 1150*

---

## bc_skips

Whether `key` ("fn:name", "impl:Type:method") is a body not to check.

```fides
pub fn bc_skips(self: &BorrowChecker, key: str) -> bool {
```

**Parameters:**

- `self`: `&BorrowChecker`
- `key`: `str`

**Returns:** `bool`

*Defined at line 1770*

---

## pattern_bindings

```fides
pub fn pattern_bindings(p: Pattern) -> Vec<str> {
```

**Parameters:**

- `p`: `Pattern`

**Returns:** `Vec<str>`

*Defined at line 3875*

---

## str_to_lower

```fides
pub fn str_to_lower(s: str) -> str {
```

**Parameters:**

- `s`: `str`

**Returns:** `str`

*Defined at line 3901*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
