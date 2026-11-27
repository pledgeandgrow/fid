[All modules](index.md)

# compiler::green_thread -- Fides Documentation

## Contents

- [GREEN_THREAD_STACK_SIZE:](#GREEN_THREAD_STACK_SIZE:) (const)
- [MAX_GREEN_THREADS:](#MAX_GREEN_THREADS:) (const)
- [GT_READY:](#GT_READY:) (const)
- [GT_RUNNING:](#GT_RUNNING:) (const)
- [GT_SUSPENDED:](#GT_SUSPENDED:) (const)
- [GT_DONE:](#GT_DONE:) (const)
- [GreenThread](#GreenThread) (struct)
- [GreenThread_new](#GreenThread_new) (function)
- [GreenThread_empty](#GreenThread_empty) (function)
- [GreenThreadScheduler](#GreenThreadScheduler) (struct)
- [GreenThreadScheduler_new](#GreenThreadScheduler_new) (function)
- [GreenThreadScheduler_spawn](#GreenThreadScheduler_spawn) (function)
- [GreenThreadScheduler_current_thread](#GreenThreadScheduler_current_thread) (function)
- [GreenThreadScheduler_yield_current](#GreenThreadScheduler_yield_current) (function)
- [GreenThreadScheduler_finish_current](#GreenThreadScheduler_finish_current) (function)
- [GreenThreadScheduler_is_done](#GreenThreadScheduler_is_done) (function)
- [GreenThreadScheduler_get_result](#GreenThreadScheduler_get_result) (function)
- [GreenThreadScheduler_has_ready_threads](#GreenThreadScheduler_has_ready_threads) (function)
- [GreenThreadScheduler_run_all](#GreenThreadScheduler_run_all) (function)

## GREEN_THREAD_STACK_SIZE:

```fides
pub const GREEN_THREAD_STACK_SIZE: i64 = 65536;  // 64 * 1024
```

*Defined at line 22*

---

## MAX_GREEN_THREADS:

```fides
pub const MAX_GREEN_THREADS: i64 = 256;  // Green thread states
```

*Defined at line 23*

---

## GT_READY:

```fides
pub const GT_READY: i64 = 0;
```

*Defined at line 24*

---

## GT_RUNNING:

```fides
pub const GT_RUNNING: i64 = 1;
```

*Defined at line 25*

---

## GT_SUSPENDED:

```fides
pub const GT_SUSPENDED: i64 = 2;
```

*Defined at line 26*

---

## GT_DONE:

```fides
pub const GT_DONE: i64 = 3;
```

*Defined at line 27*

---

## GreenThread

```fides
pub struct GreenThread {
```

**Fields:**

| Name | Type |
|------|------|
| `id` | `i64` |
| `state` | `i64` |
| `saved_sp` | `i64` |
| `saved_ip` | `i64` |
| `func_name` | `str` |
| `args` | `Vec<i64>` |
| `result` | `i64` |
| `active` | `bool,  // false = slot is empty (replaces Option<GreenThread>)` |

*Defined at line 32*

---

## GreenThread_new

```fides
pub fn GreenThread_new(id: i64, func_name: str, args: Vec<i64>) -> GreenThread {
```

**Parameters:**

- `id`: `i64`
- `func_name`: `str`
- `args`: `Vec<i64>`

**Returns:** `GreenThread`

*Defined at line 45*

---

## GreenThread_empty

```fides
pub fn GreenThread_empty() -> GreenThread {
```

**Returns:** `GreenThread`

*Defined at line 51*

---

## GreenThreadScheduler

```fides
pub struct GreenThreadScheduler {
```

**Fields:**

| Name | Type |
|------|------|
| `threads` | `Vec<GreenThread>` |
| `ready_queue` | `Vec<i64>` |
| `current` | `i64` |
| `next_id` | `i64` |

*Defined at line 61*

---

## GreenThreadScheduler_new

```fides
pub fn GreenThreadScheduler_new() -> GreenThreadScheduler {
```

**Returns:** `GreenThreadScheduler`

*Defined at line 70*

---

## GreenThreadScheduler_spawn

```fides
pub fn GreenThreadScheduler_spawn(self: &mut GreenThreadScheduler, func_name: str, args: Vec<i64>) -> i64 {
```

**Parameters:**

- `self`: `&mut GreenThreadScheduler`
- `func_name`: `str`
- `args`: `Vec<i64>`

**Returns:** `i64`

*Defined at line 82*

---

## GreenThreadScheduler_current_thread

```fides
pub fn GreenThreadScheduler_current_thread(self: &GreenThreadScheduler) -> i64 {
```

**Parameters:**

- `self`: `&GreenThreadScheduler`

**Returns:** `i64`

*Defined at line 93*

---

## GreenThreadScheduler_yield_current

```fides
pub fn GreenThreadScheduler_yield_current(self: &mut GreenThreadScheduler) -> bool {
```

**Parameters:**

- `self`: `&mut GreenThreadScheduler`

**Returns:** `bool`

*Defined at line 97*

---

## GreenThreadScheduler_finish_current

```fides
pub fn GreenThreadScheduler_finish_current(self: &mut GreenThreadScheduler, result: i64) {
```

**Parameters:**

- `self`: `&mut GreenThreadScheduler`
- `result`: `i64`

*Defined at line 131*

---

## GreenThreadScheduler_is_done

```fides
pub fn GreenThreadScheduler_is_done(self: &GreenThreadScheduler, id: i64) -> bool {
```

**Parameters:**

- `self`: `&GreenThreadScheduler`
- `id`: `i64`

**Returns:** `bool`

*Defined at line 141*

---

## GreenThreadScheduler_get_result

```fides
pub fn GreenThreadScheduler_get_result(self: &GreenThreadScheduler, id: i64) -> i64 {
```

**Parameters:**

- `self`: `&GreenThreadScheduler`
- `id`: `i64`

**Returns:** `i64`

*Defined at line 152*

---

## GreenThreadScheduler_has_ready_threads

```fides
pub fn GreenThreadScheduler_has_ready_threads(self: &GreenThreadScheduler) -> bool {
```

**Parameters:**

- `self`: `&GreenThreadScheduler`

**Returns:** `bool`

*Defined at line 163*

---

## GreenThreadScheduler_run_all

```fides
pub fn GreenThreadScheduler_run_all(self: &mut GreenThreadScheduler) -> i64 {
```

**Parameters:**

- `self`: `&mut GreenThreadScheduler`

**Returns:** `i64`

*Defined at line 170*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
