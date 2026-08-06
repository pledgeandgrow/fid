# TEST.md — Pledge Test Results

*Last updated: Aug 6, 2026 (HashMap support added)*

## Legend

- ✅ **PASS** — Correct output and/or exit code
- ❌ **FAIL** — Crash, wrong output, or no output
- ⚠️ **PARTIAL** — Works but with known limitations
- — **N/A** — Not applicable for this target

---

## Interpreter (`pledgec run`)

All tests below pass through the interpreter.

| Test File | Status | Output |
|-----------|--------|--------|
| `hello.p` | ✅ | `Hello, Pledge!\n50\ny is big` |
| `print_int.p` | ✅ | `42` |
| `print_str.p` | ✅ | `Hello!` |
| `print_test.p` | ✅ | — |
| `arithmetic.p` | ✅ | `30\n40` |
| `control_flow.p` | ✅ | `1\n30\n10\n6` |
| `functions.p` | ✅ | `7\n120` |
| `struct.p` | ✅ | `10\n20` |
| `strings.p` | ✅ | `Hello, World!\nPledge` |
| `test_float.p` | ✅ | `3.14159\n2.71828\n5.85987\n-3.5\n0` |
| `test_while_simple.p` | ✅ | `10` |
| `test_for_vec.p` | ✅ | `10\n20\n30\n40\n100` |
| `test_vec_one.p` | ✅ | `[42]` |
| `test_vec_two.p` | ✅ | `[1, 2]` |
| `test_vec_simple.p` | ✅ | `[]` |
| `test_vec_loop.p` | ✅ | `3` |
| `test_vec_len.p` | ✅ | exit=1 |
| `test_print_vec.p` | ✅ | `[1, 2, 3]` |
| `test_match.p` | ✅ | exit=300 |
| `test_option.p` | ✅ | `42\n-1` |
| `test_try.p` | ✅ | `42` |
| `test_defer.p` | ✅ | `main body\ncleanup B\ncleanup A` |
| `test_closure.p` | ✅ | `10` |
| `test_impl.p` | ✅ | `7\n13\n24` |
| `test_generics.p` | ✅ | `42\n100\n20\n50` |
| `test_capabilities.p` | ✅ | `42\n20\n10\n5\n10` |
| `test_hashmap.p` | ✅ | `2\n30\ntrue\n1\nfalse` |
| `test_fstring_vec.p` | ✅ | `vec = [1, 2]` |
| `ret42.p` | ✅ | exit=42 |
| `test_exit.p` | ✅ | exit=42 |
| `test_exit0.p` | ✅ | exit=0 |

---

## PE Emitter (`pledgec emit`)

Native Windows x64 PE executables. No external linker.

| Test File | Status | Output | Notes |
|-----------|--------|--------|-------|
| `hello.p` | ✅ | `Hello, Pledge!\n50\ny is big` | Strings, int print, if/else |
| `print_int.p` | ✅ | `42` | Integer printing |
| `print_str.p` | ✅ | `Hello!` | String printing |
| `strings.p` | ✅ | `Hello, World!\nPledge` | Multiple strings |
| `arithmetic.p` | ✅ | `30\n40` | Arithmetic ops |
| `control_flow.p` | ✅ | `1\n30\n10\n6` | If/else, while, for |
| `functions.p` | ✅ | `7\n120` | Function calls, recursion |
| `struct.p` | ✅ | `10\n20` | Struct fields |
| `test_float.p` | ✅ | `3.14159\n2.71828\n5.85987\n-3.5\n0` | Float printing |
| `test_while_simple.p` | ✅ | `10` | While loop |
| `test_for_vec.p` | ✅ | `10\n20\n30\n40\n100` | For loop over vec |
| `test_vec_one.p` | ✅ | `[42]` | Vec new + push + print |
| `test_vec_two.p` | ✅ | `[1, 2]` | Multiple pushes, grow logic |
| `test_vec_simple.p` | ✅ | `[]` | Empty vec print |
| `test_vec_loop.p` | ✅ | `3` | Vec push in while loop |
| `test_vec_len.p` | ✅ | exit=1 | Vec len after push |
| `test_print_vec.p` | ✅ | `[1, 2, 3]` | Vec with 3 elements |
| `test_match.p` | ✅ | exit=300 | Match on integer |
| `test_option.p` | ✅ | `42\n-1` | Option Some/None |
| `test_try.p` | ✅ | `42` | Try block |
| `test_defer.p` | ✅ | `main body\ncleanup B\ncleanup A` | Defer execution |
| `test_closure.p` | ✅ | `10` | Closure capture |
| `test_impl.p` | ✅ | `7\n13\n24` | Impl blocks, methods |
| `test_generics.p` | ✅ | `42\n100\n20\n50` | Generic functions |
| `test_capabilities.p` | ✅ | `42\n20\n10\n5\n10` | Capabilities |
| `ret42.p` | ✅ | exit=42 | Return value → exit code |
| `test_exit.p` | ✅ | exit=42 | Direct return |
| `test_exit0.p` | ✅ | exit=0 | Return 0 |
| `test_fstring_int.p` | ✅ | `val = 42` | F-string with integer interpolation |
| `test_fstring_vec.p` | ✅ | `vec = [1, 2]` | F-string with vec interpolation |
| `test_hashmap.p` | ✅ | `2\n30\ntrue\n1\nfalse` | HashMap insert/get/contains/remove/len/is_empty |

---

## PE Emitter — Bug Fix History

### 2026-08-06: R12/R13 ModR/M encoding bug (critical fix)

**Root cause:** `mov_reg_ptr`, `mov_ptr_reg`, and related functions checked `base == Reg::Rsp` to decide if a SIB byte was needed. But R12 has the same low 3 bits (code 4) as RSP, so the x86 encoding requires a SIB byte for R12 too. Similarly, R13 (code 5) shares bits with RBP, requiring mod=01 with disp8=0 for zero-displacement addressing.

**Impact:** Any code using R12 or R13 as a base register in memory operands produced invalid x86 encoding, causing access violations. This was the root cause of the `p_vec_push` crash.

**Fix:** Changed all affected functions to check `base.code() == 4` (covers RSP and R12) and `base.code() == 5` (covers RBP and R13) instead of comparing `Reg` enum variants directly.

**Files changed:** `src/pe.rs` — 11 functions fixed:
- `mov_reg_ptr`, `mov_ptr_reg`, `mov_ptr_reg32`, `mov_ptr_reg16`
- `mov_reg8_ptr`, `mov_reg32_ptr`, `mov_reg16_ptr`
- `emit_mem_reg`, `mov_reg_mem_disp`, `mov_mem_disp_reg`, `mov_mem_disp_reg32`

### 2026-08-06: HashMap support (p_map_* stubs)

**Root cause 1:** `p_map_insert` grow path called `p_malloc` which clobbered `r8` (len). The subsequent copy loop used `r8` as loop bound — with garbage in `r8`, it iterated wildly and crashed.

**Fix 1:** Save `r8` to `[rbp-24]` before `p_malloc`, restore after.

**Root cause 2:** All map string compare loops (`p_map_insert`, `p_map_get`, `p_map_contains`, `p_map_remove`) had the same `mov_reg8_mem` bug as `p_str_concat` — 8-bit load into 64-bit register without zeroing first.

**Fix 2:** Added `xor_self` before each `mov_reg8_mem` in all map stubs.

**Root cause 3:** `p_map_new` and `p_map_insert` had redundant `sub/add rsp 32` around `p_malloc`/`p_free` calls.

**Fix 3:** Removed redundant stack adjustments.

**Missing stub:** `p_map_is_empty` was not implemented.

**Fix 4:** Added `p_map_is_empty` stub — returns 1 if `len == 0`, else 0.

### 2026-08-06: F-string support (p_str_concat + p_to_str dispatch)

**Root cause 1:** `p_str_concat` strlen and copy loops used `mov_reg8_mem` (8-bit load) into a register, then `test_rr` (64-bit test) to check for null terminator. The 8-bit load only sets the low byte, leaving upper bytes with stale pointer values. The 64-bit test always saw non-zero bits, so null detection never triggered — loops ran past string ends and crashed.

**Fix 1:** Added `xor_self` before each `mov_reg8_mem` call to zero the register, so `test_rr` correctly reflects only the loaded byte.

**Root cause 2:** `p_to_str` only handled integers. F-strings with Vec arguments called `p_to_str` with a Vec pointer, which produced garbage.

**Fix 2:** Added type-based dispatch for `p_to_str` calls (same pattern as `print` dispatch). Added new runtime stubs:
- `p_to_str_vec` — formats Vec as `[1, 2, ...]` using `p_str_concat` + `p_to_str` per element
- `p_to_str_str` — returns input pointer unchanged
- `p_to_str_bool` — returns `"true"` or `"false"`
- `p_to_str_float` — truncates to integer, delegates to `p_to_str` (TODO: proper float formatting)

### 2026-08-06: p_vec_push full grow logic

Replaced the simplified (always-allocate) push with full implementation:
- Checks `len < cap` to skip grow
- Grows: `new_cap = cap == 0 ? 4 : cap * 2`
- Allocates new buffer via `p_malloc`
- Copies old elements to new buffer
- Frees old buffer if non-null
- Updates vec struct (ptr, cap)
- Stores value at `ptr[len]`, increments len
